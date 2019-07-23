/*
DLL library file. This file implements the serial setup of a AD9739 DAC.

Parses an SVF file and writes corresponding JTAG operations to an Ultraview device.
Author:		J. Libove		Date 6/4/05
Modified:	B. Illingworth	Date 05/18/09 
*/







// For use as a standalone application to reprogram device, define STANDALONE
// this will compile the application so that it opens a new handle to the device.
// Alternatively, the application will use a device handle passed to it, in this case
// define the correct load_svf() declaration in your user application


// Define STANDALONE only if building the load_svf program !!!
//#define STANDALONE 			// For non-standalone, set TDO_CHECK=PRINT_TDO=1


#ifdef STANDALONE
#define TDO_CHECK		0	// Extremely slow if true, (make 0 for STANDALONE)
#define PRINT_TDO		0	// must have TDO_CHECK	   NOT STANDALONE
#else
// Must have TDO_CHECK=PRINT_TDO=1 for get_usercode.svf (e.g. to readback from prom)
#define TDO_CHECK		1	// Extremely slow if true, (make 0 for STANDALONE)
#define PRINT_TDO		1	// must have TDO_CHECK	   NOT STANDALONE
#endif


#define ENABLE_OUTPUT		1
#define PRINT_TDO_HEX		0	// must have TDO_CHECK
#define PRINT_SIR_TDO		0
#define PRINT_COMMENTS		0
#define PRINT_COMMANDS		0
#define TDI_CHECK		0


#include "DllLibMain.h"



#ifdef _WINDOWS
// Start Windows Includes
#include <Windows.h>
#include <Winbase.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winioctl.h>	// only needed for direct call of IOCTL

#ifndef CTL_CODE
#pragma message("CTL_CODE undefined. Include winioctl.h or wdm.h")
#endif

#include "Pcie5vDefines.h"
#include "DllDriverApi.h"			// API to AD83000x device driver and device global settings
#else
// Start Linux Includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h> 
#include <math.h>
#endif



extern DEVICE_HANDLE;                   // Handle to the low-level driver API



typedef unsigned int u_int;

#define WRITE_TMS if(ENABLE_OUTPUT) { SET_PIO_REG(BoardNum, PIO_OFFSET_PROMPROG, tms); }        
#define WRITE_TMS_RUNTEST if(ENABLE_OUTPUT) { SET_PIO_REG(BoardNum, PIO_OFFSET_PROMPROG, tms); }


//#define	DEFAULT_DATAFILE "checkidcode.svf"
//#define	DEFAULT_DATAFILE "blankcheck.svf"
//#define	DEFAULT_DATAFILE "erase.svf"
//#define	DEFAULT_DATAFILE "ad83000x.svf"					// STAND_ALONE
//#define	DEFAULT_DATAFILE "get_usercode.svf"			// NOT_STAND_ALONE

char svf_filename[25];
unsigned long serial_number = 0xFFFFFFFF;		// 051809 the device serial number

char *datarray;
static char outputChars[100000000];  

long didx;			// Index in datarray 

#ifndef _WINDOWS
FILE * promFile;	// data file in Linux
#else
HANDLE promFile;	
#endif


long startcount; 	
long tdoval;		// JTAG TDO bit read back from RAM controller Xilinx 
long readnum();




enum { IDLE, IRPAUSE, DRPAUSE };

unsigned long endir_state = IDLE;
unsigned long enddr_state = IDLE;



unsigned int is_ISC_ERASE = 0;
unsigned int is_config = 0;
unsigned int is_XSC_DATA_UC = 0;
unsigned int reprogram = 0;



int smaskignore()
{
	long j = 0;
	char c;

	while((datarray[didx]) != ')')
	{  
		c = datarray[didx];

		// we are expecting a hex character, but could get formatting chars like newline
		if( c!='0' &&   c!='1' && c!='2' && c!='3' && c!='4' && c!='5' && c!='6' && c!='7' && c!='8'  && c!='9' && c!='a' && c!='b' && c!='c' && c!='d' && c!='e' && c!='f')
		{  
			// An invalid character
		}
		else
		{
			// A valid character, CURRENTLY WE IGNORE SMASK
			j++;
		} 

		didx++;
	}

	return(0);
}




char make_hex(char m)
{
	char data_char;
	int index;

	// If character is "0" through "9" convert to 0 through 9 
	if (m >= 48 && m <= 57)
	{ 
		data_char = m - 48;
	}
	// If character is "a" thru "f" convert to hex a thru f 
	else if (m >= 97 && m <= 102)
	{ 
		data_char = m - 87;   // add 10 to number 
	}
	// If character is "A" thru "F" convert to hex a thru f 
	else if (m >= 65 && m <= 70)
	{ 
		data_char = m - 55;
	}
	else 
	{
		//printf("ILLEGAL %c found !!\n", m);
		fflush(stdout);

		return -1;	// we didn't find a hex character
	}

	return data_char;
}





int serout(unsigned short BoardNum, char cmd, unsigned long count)	// executes SIR(cmd=2) or SDR(cmd==1), count= number of bits to transfer 
{
	unsigned int i, j, k, l, index, portvect, leading0, tdoMaskFound, tdiOK;
	long num_chars, lfcount, maxChars;
	unsigned long readVal, oldVal, numBits;
	unsigned int tms = 0;
	unsigned int tdoval;
	char m, data_char, tdoChar, readTDI;
	char tdoExpected[32768], tdoCaptured[32768], tdoMask[32768];
	int mod;


	startcount = count; 	
	numBits = count;


	//printf("\n\nserout() cmd=%d numBits=%d\n", cmd, numBits);	


	// TAP state should be IDLE...
	// Preamble starts with 2 clocks with TMS hi if SIR, moves to SELECT-IR_SCAN
	// or only 1 clock with TMS hi if SDR, moves to SELECT-DR_SCAN

	tms = 0x04;  // First preamble TMS is always 1 for either SIR or SDR 

	// serially output 1 or 2 bit TMS-hi start portion of preamble
	// moves TAP state machine to SELECT-DR_SCAN or SELECT-IR_SCAN states
	for(i = 0; i < (u_int)cmd; i++)
	{ 
		WRITE_TMS	// Writes TMS to board using either Windows or Linux driver call
	}


	tms = 0x00;  // Last two bits of preamble, TMS is always 0 for either SIR or SDR, moves to SHIFT-DR or SHIFT-IR state

	// serially output 2-bit end portion of preamble 
	// moves TAP state machine from Select-(IR/DR) to Capture-(IR/DR) to Shift-(IR/DR)
	for(i = 0; i < 2; i++)
	{
		WRITE_TMS	// Writes TMS to board using either Windows or Linux driver call
	}


	num_chars = (startcount + 3) / 4;
	maxChars = num_chars;

	//printf("\nWANT %d CHARS:\n",num_chars);
	leading0 = 0;
	lfcount = 0;
	index = 0;

	while( datarray[didx] != ')' )
	{

		if(PRINT_COMMANDS){printf("%c", datarray[didx]);}
		char c = datarray[didx];

		if( c!='0' &&   c!='1' && c!='2' && c!='3' && c!='4' && c!='5' && c!='6' && c!='7' && c!='8'  && c!='9' && c!='a' && c!='b' && c!='c' && c!='d' && c!='e' && c!='f'&& c!='A' && c!='B' && c!='C' && c!='D' && c!='E' && c!='F')
		{
			index--;   // Do not include invalid characters (formatting) in count read 
			didx++;
		}
		else
		{ 
			outputChars[index] = datarray[didx++];  // Store data in array 
		}

		index++;
	}

	if(datarray[didx] != ')'){printf("\nCLOSE PAREN NOT FOUND AT END OF DATA PAYLOAD didx=%d num_chars=%d\n", didx, num_chars ); }
	if(PRINT_COMMANDS){printf("%c", datarray[didx]);}	// print the ')'
	didx++;
	if(PRINT_COMMANDS){printf("%c", datarray[didx]);}	// print the ' '


	lfcount = 0;
	tdoChar = 0;
	tdoMaskFound = 0;
	tdiOK = 1;

	//// we are still in SHIFT-(IR/DR) State

	if( PRINT_TDO || TDO_CHECK )
	{
		oldVal = GET_PIO_REG(BoardNum, PIO_OFFSET_PROMPROG);
		tdoval = 0x00000001 & oldVal;
		tdoChar = tdoval;
		if(PRINT_TDO && reprogram) { printf("\nLSB=(%x), ", tdoval);	} // This is the LSB of the data register
	}

	while(numBits > 0)
	{
		m = outputChars[--num_chars];  
		data_char = make_hex(m);

		//printf("\n D=(%x) %d  %d :  ", data_char, num_chars, numBits );

		// If HEX character is not illegal character, then output it serially 			 				
		for(i = 0; ( (i<4) && (numBits>0) ); i++)
		{

			if(numBits == 1)						// if this is the last data char, shift in the data and move to EXIT1
			{ tms = 0x04; }					// move to EXIT1
			else { tms = 0x00; }					// stay in SHIFT

			portvect = tms | (data_char & 0x01);	// Output LS bit 

			if(ENABLE_OUTPUT)
			{

				if( TDO_CHECK || TDI_CHECK ) { oldVal = GET_PIO_REG(BoardNum, PIO_OFFSET_PROMPROG); }	// Only read back from the board if we are checking TDI or TDO values

				//#ifdef _WINDOWS
				SET_PIO_REG(BoardNum, PIO_OFFSET_PROMPROG, portvect);
				//Sleep(1);	// DWORD (integer) input is time in milli-seconds
				//#else
				//				uvdma_set_prom_reg(pAPI, portvect);
				//usleep(1);
				//#endif

				if( TDO_CHECK || TDI_CHECK )	// Only read back from the board if we are checking TDI or TDO values
				{
					readVal = GET_PIO_REG(BoardNum, PIO_OFFSET_PROMPROG);
					tdoval = 0x00000001 & readVal;
					readTDI = (readVal&0x10) >> 4;

					mod = (numBits-2)%4;
					tdoChar |= tdoval << 3-mod;

					if(mod == 0)
					{
						// we are done with a char, save the Hex number into the captured TDO array
						tdoCaptured[num_chars] = tdoChar;
						tdoChar = 0;	// reset to build next Hex char
					}

					if( PRINT_TDO && reprogram )printf("%x, ", readTDI);
					if( PRINT_TDO && reprogram )printf("(%x)  ", tdoval);
				}


				// Check TDI for both SIR and SDR commands
				// Note: after PROM pulses PROG_B on Xilinx we will not be able to read/write to Xilinx !! (after reset is pulsed this could be a false error)
				if( TDI_CHECK )
				{
					if((data_char & 0x01) != readTDI) 
					{
						tdiOK = 0;

						printf("\nTDI MIS-READ: TDI=%x, readTDI=%x, readVal=%x  oldVal=%x  numBits=%d  didx=%d\n\n", (data_char & 0x01), readTDI, readVal, oldVal, numBits, didx); 
						fflush(stdout);

						// print out the preceding characters to see where error occured
						for(l=500; l>=0; l--)
						{
							printf("%c", datarray[didx-l]);
						}
						return -1;
					}
				}
			}


			data_char >>= 1;  // Shift to bang out next bit on next pass through this loop 


			// we just transmitted the last data_char if numBits == 1, so need to PAUSE-DR or IDLE
			// SDR(cmd==1) SIR(cmd=2)
			if(numBits == 1) 
			{    

				// we must leave the TAP state machine in PAUSE, give one TMS low and return
				/*	if( ((cmd==1) && (enddr_state == DRPAUSE) ) || ((cmd==2) && (endir_state == IRPAUSE) ) )
				{
					tms = 0;		
					if(ENABLE_OUTPUT) { pAPI.SetPromReg(tms);	}	// move to PAUSE
					printf("TAP LEFT IN PAUSE\n"); fflush(stdout);
				}
				// we must leave the TAP state machine in IDLE
				else
				{
			*/

				//Sleep(1);
				tms = 0x04;		
				WRITE_TMS	// Writes TMS to board using either Windows or Linux driver call
					tms = 0;		// move to IDLE
				WRITE_TMS	// Writes TMS to board using either Windows or Linux driver call

					/*					if( is_ISC_ERASE )
					{
						tms = 0x04;		
						if(ENABLE_OUTPUT) {	pAPI.SetPromReg(tms);	}	// move to SELECT_DR_SCAN
						printf("\n!ISC_ERASE! LEAVING TAP IN SELECT_DR_SCAN\n");
						fflush(stdout);
						is_ISC_ERASE = 0;
					}

				}
*/
			}

			numBits--;	// one more data bit done


		} // END for each bit in this char


	}




	// We did all the data in this SIR or SDR command, read out the reset of this command (until we see the semi colon and new line)
	// dont check for newline here, bc this command is arbitraily long and will wrap lines
	while(datarray[didx] != ';')	
	{
		// we will see SMASK, TDO, and MASK with linefeed characters...
		if(PRINT_COMMANDS){printf("%c", datarray[didx]);}

		if( (datarray[didx] == 'T') && (datarray[didx+1] == 'D') && (datarray[didx+2] == 'O') )
		{

			if(PRINT_COMMANDS){printf("DO (");}
			didx += 5; // move past "TDO (" to the expected TDO value

			for(l=0; l<maxChars; l++)
			{
				if(PRINT_COMMANDS){printf("%c", datarray[didx]);}

				tdoExpected[l] = make_hex(datarray[didx]) ;
				didx++; // move to next character
			}

			if(PRINT_COMMANDS){printf("%c ", datarray[didx]);}
			didx++;
		}

		if( (datarray[didx] == 'S') && (datarray[didx+1] == 'M') && (datarray[didx+2] == 'A') && (datarray[didx+3] == 'S') && (datarray[didx+4] == 'K') )
		{

			if(PRINT_COMMANDS){printf("MASK (");}
			didx += 7; // move past "SMASK (" to the expected SMASK value

			// ignore the SMASK
			for(l=0; l<maxChars; l++)
			{
				if(PRINT_COMMANDS){printf("%c", datarray[didx]);}
				didx++;
			}

			if(PRINT_COMMANDS){printf("%c ", datarray[didx]);}
			didx++;
		}

		if( (datarray[didx] == 'M') && (datarray[didx+1] == 'A') && (datarray[didx+2] == 'S') && (datarray[didx+3] == 'K') )
		{
			tdoMaskFound = 1;

			if(PRINT_COMMANDS){printf("ASK (");}
			didx += 6; // move past "MASK (" to the expected MASK value

			for(l=0; l<maxChars; l++)
			{
				if(PRINT_COMMANDS){printf("%c", datarray[didx]);}

				tdoMask[l] = make_hex(datarray[didx]) ;
				didx++; // move to next character
			}

			if(PRINT_COMMANDS){printf("%c", datarray[didx]);}
			didx++;
		}

		didx++;
	}




	// print the captured/expected TDO values
	if( PRINT_TDO_HEX && reprogram )
	{ 
		printf("\n");
		printf("tdoCaptured = 0x");
		for(l=0; l<maxChars; l++)
		{
			printf("%x", tdoCaptured[l]);
		}
		printf("\n");

	}


	// If we found a TDO mask for this command then compare capture/expected TDO values
	if( tdoMaskFound && TDO_CHECK )
	{
		int tdoMatch = 1;

		// 051809 
		if(is_XSC_DATA_UC)
		{
			serial_number = 0;

			for(l=0; l<maxChars; l++)
			{
				serial_number |= (tdoCaptured[l] << (28-(4*l)));
			}

			//printf("Device serial number is = %x", serial_number);
			is_XSC_DATA_UC = 0;	// clear for next time
		}

		//*
		if( reprogram )
		{
			printf("tdoCaptured = 0x");
			for(l=0; l<maxChars; l++)
			{
				printf("%x", tdoCaptured[l]);
			}
			printf("\n");
			printf("tdoExpected = 0x");
			for(l=0; l<maxChars; l++)
			{
				printf("%x", tdoExpected[l]);
			}
			printf("\n");

			printf("tdoMask = 0x");
			for(l=0; l<maxChars; l++)
			{
				printf("%x", tdoMask[l]);
			}
			printf("\n");
		}
		//*/

		// Mask the TDO values
		tdoCaptured[maxChars, 0] &= tdoMask[maxChars, 0];
		tdoExpected[maxChars, 0] &= tdoMask[maxChars, 0];

		// Check the TDO values
		for(l=0; l<maxChars; l++)
		{
			if( tdoCaptured[l] != tdoExpected[l] )
			{
				tdoMatch = 0;
			}
		}

		// dont ignore TDO MISMATCH for reprogram files (e.g. read serial number)
		if( reprogram )
		{
			if( tdoMatch == 0 )
			{
				printf("\n");
				printf("tdoCaptured = 0x");
				for(l=0; l<maxChars; l++){ printf("%x", tdoCaptured[l]); }
				printf("\n");
				printf("tdoExpected = 0x");
				for(l=0; l<maxChars; l++){ printf("%x", tdoExpected[l]); }
				printf("\n");								
				printf("!! TDO MISMATCH !!\n");

				return -1;

			}
		}

	}


	// SDR(cmd=1) or SIR(cmd=2)
	if( (cmd==2) && PRINT_SIR_TDO )
	{
		printf("\n[");
		for(l=0; l<maxChars; l++){ printf("%x", tdoCaptured[l]); }	
		printf("]\n");
	}


	// we just issued the configuration command
	if(is_config == 1)
	{
		//#ifdef _WINDOWS
		printf("\nReinstating Configuration Space\n");
		Sleep(2000);
		if(ENABLE_OUTPUT) { pAPI->WriteConfigSpace(0); }	// reinstate the configuration space 
		//#else
		//		printf("\nReinstating Configuration Space\n");
		//		usleep(2000000);	// 2 seconds to reconfigure
		//		if(ENABLE_OUTPUT) { uvdma_write_config_space(pAPI); }
		//#endif		
		is_config = 0;
	} 

	return(0);
}




// Executes RUNTEST instuction (the wait cycle instruction)
// For a PROM we do not need to give the physical clock, this is just a wait period
// to guaruntee that the PROM has finish the previously requested programming operation
// runcount is number of 1Mhz clock cycle we must wait; e.g. number of microseconds
int runtest(unsigned short BoardNum, unsigned long runcount)		
{
	u_int tms = 0;

	//	ULONG sleeptime;
	//	sleeptime = ceil( (double) runcount/(1000) );	// 1K clock cycles at 1MHz = 1ms
	//	if(sleeptime < 1) { sleeptime = 1;}
	//	Sleep(sleeptime);

	#ifdef _WINDOWS
	while(runcount > 0)
	{
		WRITE_TMS_RUNTEST	// Writes TMS to board using either Windows or Linux driver call
			runcount--;
	}
	#else
	usleep(runcount);
	#endif

	return(0);
}





int rreset(unsigned short BoardNum)   // Executes RESET
{
	u_int tms, resetcount;

	tms = 0x04;	
	resetcount = 5;

	while(resetcount > 0)
	{
		WRITE_TMS	// Writes TMS to board using either Windows or Linux driver call
			resetcount--;
	}

	return(0);
}



int ridle(unsigned short BoardNum)   // Executes IDLE 
{
	u_int tms, idlecount;

	tms = 0;	
	idlecount = 1;

	while(idlecount > 0)
	{
		WRITE_TMS	// Writes TMS to board using either Windows or Linux driver call
			idlecount--;
	}

	return(0);
}





// Reads a decimal number in an ascii field, and returns the number 
long readnum()
{   
	long accum = 0;

	// Read digits and accumulate into dec number 
	while((datarray[didx]) != ' ')
	{  
		if(PRINT_COMMANDS){printf("%c", datarray[didx]);}

		accum *= 10;    // shift left 

		// If char is "0"-"9" convert to num 
		if (datarray[didx] >= 48 && datarray[didx] <= 57){  
			accum += datarray[didx++] - 48;
		}
		else{
			printf("\nILLEGAL DIGIT NOT 0-9");
			return(-1);
		}
	}

	didx++;
	return((long)accum);
}






void configure(unsigned short BoardNum)
{
	unsigned long i, j, tms, bytesRead, portvect;
	char cmdtype;		// 1 for SIR, 2 for SDR, 0 otherwise 
	long sizeread;		// amount of data actually read from file 
	int num_devs;		// number of devices

	#ifndef _WINDOWS
	promFile = fopen(svf_filename, "rb");
	if(promFile == NULL) { printf("ERROR: could not read SVF file!\n"); return; }
	// get the file size
	fseek( (FILE *) promFile, 0, SEEK_END);
	unsigned long dwSize = ftell( (FILE *) promFile);
	rewind( (FILE *) promFile);
	bytesRead = fread(datarray, 1, dwSize, (FILE *) promFile);

	#else


	promFile = CreateFile(svf_filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

	if(promFile == INVALID_HANDLE_VALUE) 
	{ 
		printf("ERROR: could not read SVF file!\n"); return;

		#ifdef _WINDOWS
		char CurrentDir[200];
		GetCurrentDirectory(200, CurrentDir);
		printf("LoadSvf() configure() CurrentDir= %s! \n", CurrentDir);
		printf("LoadSvf() configure() ERROR: could not read SVF file %s! \n", svf_filename);
		#endif

	}

	DWORD dwSize = 0;
	dwSize = GetFileSize(promFile, NULL);

	ReadFile(promFile, datarray, dwSize, &bytesRead, NULL);
	CloseHandle(promFile);
	#endif



	if(reprogram)
	{
		printf("Read %d bytes from file\n", bytesRead);
		printf("Programming Can Take Over 30 Minutes.\nDO NOT SHUT SYSTEM UNTIL PROGRAMMING COMPLETES (Sucess Message Will be Printed)", bytesRead);
	}



	// print the first part of the file for debugging...
	//	for(didx = 0; didx < 500; didx++)	// dwSize
	//	{
	//		printf("%c", datarray[didx]);
	//	}
	//	printf("\n");


	cmdtype = 0;


	// for each character in the file...parse away...
	for(didx = 0; didx < bytesRead; didx++) 
	{
		cmdtype = 0;

		if(datarray[didx] == '/' && PRINT_COMMENTS)
		{
			printf("%c", datarray[didx]);	// print the comments
		}
		else if(datarray[didx] != '/' && PRINT_COMMANDS)
		{
			printf("%c", datarray[didx]);	// print the comments
		}


		if( (datarray[didx] == '/') && (datarray[didx+1] == '/') )
		{
			didx++;	// we already printed first '/' so skip it


			// if its a comment then read out the comment, this code assumes single line comment
			while(datarray[didx] != '\n')
			{
				if(PRINT_COMMENTS) printf("%c", datarray[didx]);
				didx++;

				if( (datarray[didx] == 'I') && (datarray[didx+1] == 'S') && (datarray[didx+2] == 'C') && (datarray[didx+3] == '_') && (datarray[didx+4] == 'E') && (datarray[didx+5] == 'R') && (datarray[didx+6] == 'A') && (datarray[didx+7] == 'S') && (datarray[didx+8] == 'E') )
				{
					is_ISC_ERASE = 1;	// The next instruction is an ISC_ERASE operation
				}
				if( (datarray[didx] == 'c') && (datarray[didx+1] == 'o') && (datarray[didx+2] == 'n') && (datarray[didx+3] == 'f') && (datarray[didx+4] == 'i') && (datarray[didx+5] == 'g') )
				{
					is_config = 1;	// The next instruction is an ISC_ERASE operation
				}
				if( (datarray[didx] == 'X') && (datarray[didx+1] == 'S') && (datarray[didx+2] == 'C') && (datarray[didx+3] == '_') && 
				   (datarray[didx+4] == 'D') && (datarray[didx+5] == 'A')  && (datarray[didx+6] == 'T')  && (datarray[didx+7] == 'A')  && 
				   (datarray[didx+8] == '_')  && (datarray[didx+9] == 'U')  && (datarray[didx+10] == 'C')  )
				{
					is_XSC_DATA_UC = 1;
				}

			}
			if(PRINT_COMMENTS) printf("\n");
			if(PRINT_COMMENTS) fflush(stdout);
		}

		// Parse the SDR, SIR and SMASK commands
		else if(datarray[didx] == 'S')				
		{   
			didx++;

			if(datarray[didx] == 'D')			// See if SDR command 
			{ 
				cmdtype = 1;
				//printf("D");
			}
			else if(datarray[didx] == 'I')		// See if SIR command 
			{  
				cmdtype = 2;
				//printf("I");
			}
			else if(datarray[didx] == 'M')		// See if SMASK command 
			{		
				printf("%c", datarray[didx++]);
				/*
				if(datarray[didx++] != 'A'){printf("\nFRAME ERR"); continue;}
				if(datarray[didx++] != 'S'){printf("\nFRAME ERR"); continue;}
				if(datarray[didx++] != 'K'){printf("\nFRAME ERR"); continue;}
				if(datarray[didx++] != ' '){printf("\nFRAME ERR"); continue;}
				if(datarray[didx++] != '('){printf("\nFRAME ERR"); continue;}
				printf("ASK (");
			//*/
				cmdtype = 3;
				if (smaskignore() <0)
				{
					printf("\n *** SMASK IGNORE FAILED **");
					return;
				}
			}

			// See if STATE command
			else if(datarray[didx] == 'T')		
			{   
				if(PRINT_COMMANDS){printf("%c", datarray[didx]); }
				didx++;

				if( (datarray[didx] == 'A') && (datarray[didx+1] == 'T') && (datarray[didx+2] == 'E') )
				{
					if(PRINT_COMMANDS){printf("%c%c%c%c", datarray[didx], datarray[didx+1], datarray[didx+2], datarray[didx+3]  ); }
					didx+=4;

					if( (datarray[didx] == 'I') && (datarray[didx+1] == 'D') && (datarray[didx+2] == 'L') && (datarray[didx+3] == 'E') )
					{
						// move to TAP IDLE state, this means TMS low for one cycle if you are already in RESET

						//printf("\n STATE IDLE IGNORED\n");

						if(PRINT_COMMANDS){printf("%c%c%c%c%c", datarray[didx], datarray[didx+1], datarray[didx+2], datarray[didx+3], datarray[didx+4]  ); }
						didx+=5;

						// Note: When RESET is specified as the tap_state, Xilinx tools interpret the state transition as requiring the 
						// guaranteed TAP transition to the Test-Logic-Reset state, i.e. hold TMS High for a minimum of five TCK cycles.

						tms = 0x04;		// set bit2=TMS high, bit1=TCK is don't care (clock goes high then low on each write), clear bit0=TDI 
						for(j=0; j<5; j++)
						{
							WRITE_TMS	// Writes TMS to board using either Windows or Linux driver call
						}

						tms = 0x00;		// clear bit2=TMS, bit1=TCK is don't care (clock goes high then low on each write), clear bit0=TDI 
						WRITE_TMS	// Writes TMS to board using either Windows or Linux driver call
					}
					else if( (datarray[didx] == 'R') && (datarray[didx+1] == 'E') && (datarray[didx+2] == 'S') && (datarray[didx+3] == 'E') && (datarray[didx+4] == 'T') )
					{
						// move to TAP RESET state

						// Note: When RESET is specified as the tap_state, Xilinx tools interpret the state transition as requiring the 
						// guaranteed TAP transition to the Test-Logic-Reset state, i.e. hold TMS High for a minimum of five TCK cycles.

						//printf("\n STATE RESET IGNORED\n");
						/*
						if(PRINT_COMMANDS){printf("%c%c%c%c%c%c", datarray[didx], datarray[didx+1], datarray[didx+2], datarray[didx+3], datarray[didx+4], datarray[didx+5]  ); }
						didx+=6;

						portvect = 0x04;		// set bit2=TMS high, bit1=TCK is don't care (clock goes high then low on each write), clear bit0=TDI 

						for(j=0; j<5; j++)
						{
							if(ENABLE_OUTPUT)
							{
								pAPI.SetPromReg((ULONG) portvect);
							}
						}
*/

					}
					else{printf("SUB STATE FRAME ERROR\n"); return; }
				}
				else{ printf("STATE FRAME ERROR\n"); return; }

				while(datarray[didx] != '\n')
				{
					if(PRINT_COMMANDS){printf("%c", datarray[didx]);}
					didx++;
				}

				if(PRINT_COMMANDS){printf("\n");	fflush(stdout); }
			}



			if((cmdtype == 1) || (cmdtype == 2))
			{
				if(PRINT_COMMANDS){printf("%c", datarray[didx]); }	 // Print D or I 
				didx++;

				if(datarray[didx++] != 'R')		// check for R
				{   
					printf("\n ABORTED-NO R"); cmdtype = 0; return;
				}
				if(datarray[didx++] != ' ')		// check for space 
				{  
					printf("\n ABORTED-NO SPACE"); cmdtype = 0; return;
				}

				if(PRINT_COMMANDS){printf("%c%c", datarray[didx-2], datarray[didx-1]); }	// Print space 

				// Read the length field
				if((sizeread = (long) readnum()) > 0)
				{ 
					if(datarray[didx++] != 'T'){printf("\nFRAME ERR"); return;}
					if(datarray[didx++] != 'D'){printf("\nFRAME ERR"); return;}
					if(datarray[didx++] != 'I'){printf("\nFRAME ERR"); return;}
					if(datarray[didx++] != ' '){printf("\nFRAME ERR"); return;}
					if(datarray[didx++] != '('){printf("\nFRAME ERR"); return;}

					//printf("TDI (");
					if(PRINT_COMMANDS){printf(" TDI ("); }

					serout(BoardNum, cmdtype, sizeread);				// OUTPUT THE BITSTREAM !!!
				}
				else 
				{
					printf("\n******ABORTED SIZE READ**");
					break;
				} 

			}
		}

		// Check to see if RUNTEST or RESET command
		else if(datarray[didx] == 'R')			 
		{  
			didx++;
			if(datarray[didx] == 'U')
			{
				didx++;
				cmdtype = 4;

				if(PRINT_COMMANDS){printf("%c", datarray[didx]); }

				if(datarray[didx++] != 'N'){printf("\nFRAME ERR");  return;}
				if(datarray[didx++] != 'T'){printf("\nFRAME ERR");  return;}
				if(datarray[didx++] != 'E'){printf("\nFRAME ERR");  return;}
				if(datarray[didx++] != 'S'){printf("\nFRAME ERR");  return;}
				if(datarray[didx++] != 'T'){printf("\nFRAME ERR");  return;}
				if(datarray[didx++] != ' '){printf("\nFRAME ERR");  return;}
				if(PRINT_COMMANDS){printf("NTEST ");}

				if((sizeread = (long) readnum()) <= 0)	// Read number of cycles to output
				{  
					printf("\n******ABORTED RUNTEST**");
					cmdtype = 0;
					break;
				}
				else
				{
					if(datarray[didx++] != 'T'){printf("\nFRAME ERR");  return;}
					if(datarray[didx++] != 'C'){printf("\nFRAME ERR");  return;}
					if(datarray[didx++] != 'K'){printf("\nFRAME ERR");  return;}
					if(datarray[didx++] != ';'){printf("\nFRAME ERR");  return;}

					if(PRINT_COMMANDS){printf("TCK;");}

					runtest(BoardNum, sizeread);
				}
			}
			// Check to see if RESET command
			else if(datarray[didx] == 'E')
			{   
				didx++;
				cmdtype = 5;

				if(PRINT_COMMANDS){printf("%c", datarray[didx]);}

				if(datarray[didx++] != 'S'){printf("\nFRAME ERROR");  return;}
				if(datarray[didx++] != 'E'){printf("\nFRAME ERROR");  return;}
				if(datarray[didx++] != 'T'){printf("\nFRAME ERROR");  return;}
				if(PRINT_COMMANDS){printf("SET");}

				rreset(BoardNum);
			}
		}

		// Parse the IDLE command 
		else if(datarray[didx] == 'I')
		{  
			didx++;

			if(datarray[didx++] != 'D'){printf("\nFRAME ERROR");  return;}
			if(datarray[didx++] != 'L'){printf("\nFRAME ERROR");  return;}
			if(datarray[didx++] != 'E'){printf("\nFRAME ERROR");  return;}

			if(PRINT_COMMANDS){printf("DLE");}
			cmdtype = 6;			
			ridle(BoardNum);
		}

		// Parse the HIR, HDR command, Global Header command APPEND these bits to all SIR or SDR commands
		else if(datarray[didx] == 'H')		
		{   
			didx++;
			if(datarray[didx] == 'I')
			{

				if(PRINT_COMMANDS){printf("%c", datarray[didx]); }
				didx++;
				if(PRINT_COMMANDS){printf("%c", datarray[didx]); }
				if(datarray[didx++] != 'R'){printf("\nFRAME ERR");  return;}
				else if(PRINT_COMMANDS){printf("(IGNORE)");}
				cmdtype = 8;

				didx++;
				if(datarray[didx] != '0'){printf("\nMUST USE GLOBAL PADDING!!\n");  return;}
			}
			else if(datarray[didx] == 'D')
			{
				if(PRINT_COMMANDS){printf("%c", datarray[didx]);}
				didx++;
				if(PRINT_COMMANDS){printf("%c", datarray[didx]); }
				if(datarray[didx++] != 'R'){printf("\nFRAME ERR");  return;}
				else if(PRINT_COMMANDS){printf("(IGNORE) ");}
				cmdtype = 9;

				didx++;
				if(datarray[didx] != '0'){printf("\nMUST USE GLOBAL PADDING!!\n");  return;}
			}
		}

		// Parse the TIR, TRST command , Global Trailer PRECEDE all SIR or SDR commands with these bits
		else if(datarray[didx] == 'T')	 
		{ 
			didx++;

			if(datarray[didx] == 'I')
			{
				if(PRINT_COMMANDS){printf("%c", datarray[didx]); }
				didx++;
				if(PRINT_COMMANDS){printf("%c", datarray[didx]); }
				if(datarray[didx++] != 'R'){printf("\nFRAME ERR");  return;}
				else if(PRINT_COMMANDS){printf("(IGNORE) ");}
				cmdtype = 10;
			}
			else if(datarray[didx] == 'R')
			{
				if(PRINT_COMMANDS){printf("%c", datarray[didx]); }
				didx++;
				if(PRINT_COMMANDS){printf("%c", datarray[didx]); }

				if(datarray[didx++] != 'S'){printf("\nFRAME ERR");  return;}
				if(datarray[didx++] != 'T'){printf("\nFRAME ERR");  return;}
				else if(PRINT_COMMANDS){printf("T(IGNORE) ");}
				cmdtype = 11;
			}

			// read out the remainder of this command
			while(datarray[didx] != '\n')
			{
				if(PRINT_COMMANDS){printf("%c", datarray[didx]); }
				didx++;
			}

			if(PRINT_COMMANDS){printf("\n");	fflush(stdout); }
		}

		// Parse the ENDIR, ENDDR commands 
		else if(datarray[didx] == 'E')	
		{  
			didx++;

			if( (datarray[didx] == 'N') && (datarray[didx+1] == 'D') )
			{ 
				if(PRINT_COMMANDS){printf("%c%c", datarray[didx], datarray[didx+1]);}
				didx+=2;
			}
			else{ printf("ENDSTATE FRAME ERR\n"); return; }

			// If its an ENDIR command, set the SIR endstate for all following SIR commands
			if( (datarray[didx] == 'I') && (datarray[didx+1] == 'R'))
			{ 
				if(PRINT_COMMANDS){printf("%c%c%c", datarray[didx], datarray[didx+1], datarray[didx+2]);}
				didx+=3;

				if( (datarray[didx] == 'I') && (datarray[didx+1] == 'D') && (datarray[didx+2] == 'L') && (datarray[didx+3] == 'E') )
				{
					endir_state = IDLE;
					if(PRINT_COMMANDS){printf("%c%c%c%c%c", datarray[didx], datarray[didx+1], datarray[didx+2], datarray[didx+3], datarray[didx+4] );}
					if(PRINT_COMMANDS){printf(" endIR_state = IDLE");	fflush(stdout); }

					didx+=5;
					cmdtype = 12;
				}
				else if( (datarray[didx] == 'I') && (datarray[didx+1] == 'R') && (datarray[didx+2] == 'P') && (datarray[didx+3] == 'A') && (datarray[didx+4] == 'U') && (datarray[didx+5] == 'S') && (datarray[didx+6] == 'E') )
				{
					endir_state = IRPAUSE;
					if(PRINT_COMMANDS){printf("%c%c%c%c%c%c%c%c", datarray[didx], datarray[didx+1], datarray[didx+2], datarray[didx+3], datarray[didx+4], datarray[didx+5], datarray[didx+6], datarray[didx+7] ); }					
					if(PRINT_COMMANDS){printf(" endIR_state = IRPAUSE");	fflush(stdout); }

					didx+=8;
					cmdtype = 12;
				}
				else{ printf("ENDSTATE FRAME ERR\n"); return; }
			}

			// If its an ENDDR command, set the SDR endstate for all following SIR commands
			else if( (datarray[didx] == 'D') && (datarray[didx+1] == 'R'))
			{ 
				if(PRINT_COMMANDS){printf("%c%c%c", datarray[didx], datarray[didx+1], datarray[didx+2]); }
				didx+=3;

				if( (datarray[didx] == 'I') && (datarray[didx+1] == 'D') && (datarray[didx+2] == 'L') && (datarray[didx+3] == 'E') )
				{
					enddr_state = IDLE;
					if(PRINT_COMMANDS){printf("%c%c%c%c%c", datarray[didx], datarray[didx+1], datarray[didx+2], datarray[didx+3], datarray[didx+4] );}
					if(PRINT_COMMANDS){printf(" endDR_state = IDLE");	fflush(stdout); }

					didx+=5;
					cmdtype = 12;
				}
				else if( (datarray[didx] == 'I') && (datarray[didx+1] == 'R') && (datarray[didx+2] == 'P') && (datarray[didx+3] == 'A') && (datarray[didx+4] == 'U') && (datarray[didx+5] == 'S') && (datarray[didx+6] == 'E') )
				{
					enddr_state = DRPAUSE;
					if(PRINT_COMMANDS){printf("%c%c%c%c%c%c%c%c", datarray[didx], datarray[didx+1], datarray[didx+2], datarray[didx+3], datarray[didx+4], datarray[didx+5], datarray[didx+6], datarray[didx+7] ); }
					if(PRINT_COMMANDS){printf(" endDR_state = DRPAUSE");	fflush(stdout); }

					didx+=8;
					cmdtype = 12;
				}
				else{ printf("ENDSTATE FRAME ERR\n"); return; }
			}
			else{ printf("ENDSTATE FRAME ERR\n"); return; }
		}

		// Parse the FREQUENCY command
		else if(datarray[didx] == 'F')	
		{   
			didx++;

			if( (datarray[didx] == 'R') && (datarray[didx+1] == 'E') && (datarray[didx+2] == 'Q') && (datarray[didx+3] == 'U') && (datarray[didx+4] == 'E') && (datarray[didx+5] == 'N') && (datarray[didx+6] == 'C') && (datarray[didx+7] == 'Y') )
			{
				enddr_state = IRPAUSE;
				if(PRINT_COMMANDS){printf("%c%c%c%c%c%c%c%c%c", datarray[didx], datarray[didx+1], datarray[didx+2], datarray[didx+3], datarray[didx+4], datarray[didx+5], datarray[didx+6], datarray[didx+7], datarray[didx+8] ); }
				if(PRINT_COMMANDS){printf("(IGNORE) "); }
				didx+=9;
				cmdtype = 13;

				while(datarray[didx] != '\n')
				{
					if(PRINT_COMMANDS){printf("%c", datarray[didx]); }
					didx++;
				}

				if(PRINT_COMMANDS){printf("\n"); }
				fflush(stdout);
			}
			else{ printf("FREQUENCY FRAME ERR\n"); return; }
		}
	}

}









extern "C" unsigned long LoadSvf(unsigned short BoardNum, char *filename, int reprogram_in)	
{
	int dev_num = 1;			 // to look for multiple devices
	char char_buf[10];
	unsigned long readVal = 0;

	datarray = (char *) malloc(10*1024*1024);  // the SVF instructions/data could be many MB if programming a PROM

	if(datarray == NULL)
	{
		printf("LoadSvf() Error allocating data array\n"); 
	}


	strcpy(svf_filename, filename);
	reprogram = reprogram_in;	// limited print statments

	if(reprogram){printf("\n");}

	#ifdef _WINDOWS		
	time_t ltime;
	time(&ltime);
	#else
	struct timeval tv_start;
	struct timeval tv_end;
	gettimeofday(&tv_start, NULL);
	#endif

	if(ENABLE_OUTPUT) 
	{
		// reinstate the configuration space iff reprogramming
		if(reprogram)
		{
			pAPI->WriteConfigSpace(readVal);	 
		}

		if(reprogram){printf("\nReinstating Configuration Space\n");}

		readVal = GET_PIO_REG(BoardNum, PIO_OFFSET_PROMPROG);		// Check if Xilinx Programming Cable is Installed

		if( !(0x00000080 & readVal) )
		{
			printf("! Programming Cable Installed, Remove Before Loading SVF! PromReg = 0x%x \n", readVal);
			return -1;
		}        
	}


	configure(BoardNum);	// configure this device


	#ifdef _WINDOWS
	time_t elapsed = ltime;
	time(&ltime);
	elapsed = ltime - elapsed;
	unsigned long min = floor((double)elapsed/60);
	unsigned long sec = elapsed%60;
	#else
	double start_time, end_time, elapsed;
	gettimeofday(&tv_end, NULL);
	start_time = tv_start.tv_sec * 1000000 + tv_start.tv_usec;
	end_time = tv_end.tv_sec * 1000000 + tv_end.tv_usec;
	elapsed = tv_end.tv_sec - tv_start.tv_sec;

	unsigned long min = (unsigned long) floor( elapsed/60 );
	unsigned long sec = (unsigned long) elapsed%60;
	#endif

	if(reprogram){printf("\n Success! Programming Took %ldmin %ldsec\n", min, sec);}

	free(datarray);

	return serial_number;
}






