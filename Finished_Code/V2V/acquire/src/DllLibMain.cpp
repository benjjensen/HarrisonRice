/**
 * DLL library file. This file contains the bulk of the high-level DLL functionality.
 * 
 * 
 * AN EXPLANATION OF HOW EACH BOARD'S STATUS IS UNIQUELY KEPT TRACK OF AND EACH BOARD IS UNIQUELY ACCESSED.
 * 
 * When the bus is enumerated a seperate instance of the device driver is created for each board in the system. Each one
 * of these instances contains unique variables for each board. Therefore when we want to target a specific board we need
 * to make sure we use the correct device handle. 
 * 
 * The DLL has two components, the low-level functions in the API files that directly access a device, and the higher-level 
 * functions contained in this file and others that do not directly access the device. For simplicity and transparency to the user
 * all the user does is select which device in the system is the current one to be operated on using pAPI->ApiSetCurrentDevice (which
 * is exported to the user). All future calls to the DLL will target the selected device. User level system calls that target a device
 * such as read()/write() must call the lower-level DLL function that returns pAPI->hCurrentDevice.
 * 
 * pAPI->hCurrentDevice is the handle to the currently selected device. pAPI is the DLL to driver API object containing the board handles and board accessor
 * functions. Just as the device driver has unique instances of each board's driver variables the DLL must do the same with other variables
 * such as having unique instances of all the PIO register values for each board.
 * 
 * We keep track of the board variables that the user might change in the DLL and not in the driver because its easier to recompile
 * and distribute the DLL and it provides a good layer of abstraction.
 */

#define EXPORT_FCNS
#include "shrhelp.h"

#include "DllLibMain.h"  
#include "DllDriverApi.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

#ifdef _MATLAB
#include <mex.h>
#endif
/*
 * The #include <mex.h> location is assumed to be the default install directory C:\Program Files\MATLAB\R2013a\extern\include.
 * For those wish to recompile the DLL, keep in mind the MATLAB version you are running and where the MATLAB directory is installed
 * on your local machine. To modify the path, switch the Solution Configuration to Matlab. Access Project -> AcqSynth Properties. 
 * In the VC++ Directories tab, modify the Include Directories and Library Directories to point to the correct locations.
 */

#ifndef _WINDOWS
#include <unistd.h>
//    #include "AppDll.h"
#else
#include "shrhelp.h"
#include "AppDll.h"
#endif

//for large files
#define _FILE_OFFSET_BITS 64

extern "C" unsigned long LoadSvf(unsigned short BoardNum, char *filename, int reprogram_in);

//extern std::mutex makeAtomic[MAX_DEVICES];
extern  int busy;

// These are global variables used througout the DLL source files. They are declared here and must be imported in other source files.
extern DEVICE_HANDLE;                   // Handle to the low-level driver API
//int BoardNum;                           // The index of the current board that we are accessing.
cal_struct cal_data[MAX_DEVICES];       // Calibration data for each board
config_struct dev_config[MAX_DEVICES];  // Configuration data structure

unsigned int adcClockFrequency = 0;


#define ADC_IDELAY_CE		0
#define ADC_IDELAY_RST		1
#define ADC_IDELAY_INC		2
#define ADC_AUTO_IDELAY		6

#define MAX_ADC 4
#define ISLA_NUMREG 0xD1

#define FIDUCIAL 0
#define SYNC_SEL_RECORD 1
#define IC_ON	19


#ifdef _WINDOWS
DRV_VERSION driver_struct;
bool ReadDriverVersion()
{
	driver_struct = GET_DRIVER_VERSION();
	return true;
}
#endif


// we need to calibrate when we:
// 1. change from internal or external clock
// 2. change from 2 channel mode to 1 channel mode     QUESTION REQUIREMENT
// 3. change from ch0 to ch1 in 1 ch mode     QUESTION REQUIREMENT

#define CLOCK_NONE 0
#define CLOCK_INTERNAL 1
#define CLOCK_EXTERNAL 2
#define CLOCK_SELECTED_ADDR 0
//int clockMode[MAX_DEVICES] = {CLOCK_NONE,CLOCK_NONE,CLOCK_NONE,CLOCK_NONE,CLOCK_NONE};

#define CH_MODE_NONE			0
#define FOUR_CH_MODE            3
#define TWO_CH_MODE				2
#define ONE_CH_MODE				1
#define CH_MODE_ADDR            4
//int channelMode[MAX_DEVICES] = {CH_MODE_NONE,CH_MODE_NONE,CH_MODE_NONE,CH_MODE_NONE,CH_MODE_NONE};


#define CH_SELECT_NONE			0  // NOT configured
#define DESCLKIQ_MODE			1  // both inputs externally connected
#define DESIQ_MODE				2  // both inputs externally connected
#define DESQ_MODE					3  // ain0
#define DESI_MODE					4  // ain1
#define CH_SELECT_ADDR 8

// TriggerTypes
#define TRIGGER_NONE				0
#define NO_TRIGGER					1
#define WAVEFORM_TRIGGER			2
#define SYNC_SELECTIVE_RECORDING	3
#define HETERODYNE					4
#define TTL_TRIGGER_EDGE			5
#define TRIGGER_SELECT_ADDR         12

// Slope
#define SLOPE_NONE		0
#define FALLING_EDGE	1
#define RISING_EDGE		2
#define SLOPE_SELECT_ADDR 16


#define REVERSE_STACKING_THRESHOLD 1500

// ISLA FIRMWARE OpCode
#define CLK_DIV_RST_COMMAND             0
#define CLK_DIV_RST_POL_COMMAND         1
#define SPI_COMMAND                     4
#define SET_CAL_BITS_COMMAND            8
#define CLEAR_CAL_BITS_COMMAND         12
#define RESET_N_COMMAND                 0   // note firmware decodes this differently than clkdivrst by using the ADC_sel bits.  Firmware should be updated with unique value for clarity

// ISLA SPI BIT POSITIONS
#define CAL_BIT_POS 28
#define COMMAND_BIT_POS 24
#define COMMAND_BIT_POS_LEN 4
#define ADC_BIT_POS (COMMAND_BIT_POS + COMMAND_BIT_POS_LEN)
#define RW_BIT_POS 23
#define ADDR_BIT_POS 8
#define DATA_BIT_POS 0


//ISLA216P  SPI ADDRESSES
#define CHIP_PORT_CONFIG        0X00
#define SDO_Active				7
#define LSB_First				6
#define Soft_Reset				5
#define ReservedBit4			4
#define Mirror_ReservedBit4		3
#define Mirror_Soft_Reset		2
#define Mirror_LSB_First		1
#define Mirror_SDO_Active		0
#define CHIP_ID                 0x08
#define CHIP_VERSION            0x09
#define OFFSET_COARSE_ADC0      0x20
#define Offset_Coarse		0
#define OFFSET_FINE_ADC0        0x21
#define Offset_Fine			0
#define GAIN_COARSE_ADC0        0x22
#define Gain_Coarse			0
#define GAIN_MEDIUM_ADC0        0x23
#define Gain_Medium			0
#define GAIN_FINE_ADC0          0x24
#define Gain_Fine			0
#define MODES_ADC0				0x25
#define Mode_BitPos			0
#define PinControl_Mode		0
#define Normal_Mode			1
#define Nap_Mode			2
#define Sleep_Mode			4
#define OFFSET_COARSE_ADC1      0x26
//#define Offset_Coarse		0
#define OFFSET_FINE_ADC1        0x27
//#define Offset_Fine		0
#define GAIN_COARSE_ADC1        0x28
//#define Gain_Coarse		0
#define GAIN_MEDIUM_ADC1        0x29
//#define Gain_Medium		0
#define GAIN_FINE_ADC1          0x2a
//#define Gain_Fine			0
#define MODES_ADC1				0x2b
//#define PinControl_Mode	0
//#define Normal_Mode		1
//#define Nap_Mode			2
//#define Sleep_Mode		4
#define RESERVED31_REG			0x31
#define RESERVED53_REG			0x53
#define RESERVED55_REG			0x55
#define SKEW_DIFF_REG			0x70
#define Differential_Skew	0
#define PHASE_SLIP_REG			0x71
#define NextClockEdge		0
#define CLOCK_DIVIDE_REG        0x72
#define PinControl			0
#define DivideByOne			1
#define DivideByTwo			2
#define DivideByFour		4
#define OUTPUT_MODE_B	        0x74
#define DLL_Range			6
#define DLL_Fast		0
#define DLL_Slow		1
#define CAL_STATUS				0xB6
#define TEST_IO					0xC0
#define TestMode_BitPos			4
#define TestMode_Off		0
#define MidscaleShort		1
#define Positive_FullScale	2
#define Negative_FullScale	3
#define Ramp				10
#define OFFSET_GAIN_ADJUST_ENABLE_REG 0xFE
#define GainAdjustEnable	0

// ISLA OpCode
#define CLK_DIV_RST  0
#define CLK_DIV_RST_POL  1
#define N_RESET  2
#define SET_CAL_BITS  3
#define CLEAR_CAL_BITS   4
#define SPI_READ   5
#define SPI_WRITE  6
#define SOFT_RESET_WORD 0x22

// Trigger Types
#define NO_TRIGGER					0
#define WAVEFORM_TRIGGER			1
#define SYNC_SELECTIVE_RECORDING	2
#define HETERODYNE					3
#define TTL_TRIGGER_EDGE			4

//Channels
#define IN0		1
#define IN1		2
#define IN2		4
#define IN3		8




//int channelSelect[MAX_DEVICES] = {CH_SELECT_NONE,CH_SELECT_NONE,CH_SELECT_NONE,CH_SELECT_NONE,CH_SELECT_NONE};




// When the DLL was loaded it initialized handles to each device in the system.
// This function will setup for capture the board with index ss->board_num.

EXPORTED_FUNCTION bool SetupBoard(unsigned short BoardNum)
{
	#ifdef DEBUG_TO_FILE
	DEBUG_TO_FILE
		FILE *debugFile;
	debugFile = fopen("c:\\uvdma\\debug.txt","a");
	long tm = GetTickCount();
	fprintf(debugFile,"CDllDriverApi::SetupBoard\t\t\t%08ld\t%d\t begin\n",tm  ,BoardNum);
	fflush(debugFile);
	fclose(debugFile);
	#endif

	unsigned int num_devices, adc_clock_freq, prev_channelselect;
	// Ensure that we have checked number of devices found in system.
	num_devices = GET_NUMDEVICES();

	//printf("pAPI=%p pAPI(dec)=%d ss->board_num=%d ss->num_devices=%d \n", pAPI, pAPI, ss->board_num, ss->num_devices);

	if( (pAPI <= NULL) || (BoardNum > num_devices-1))
	{
		printf("Unable to open requested board. %d devices found.\n", num_devices);
		return false;
	}

	//    printf("num_devices = %d\n",num_devices);
	//    fflush(stdout);

	// All future accesses to the DLL will target this board, until SetupBoard is called again with a different board number.
	//ss->adc_calibration_ok = true; // initialize this to true for non-bit boards that don't need to be calibrated

	//ReadDriverVersion(ss);

	// The configuration file ultra_config.dat contains critical information for operation of the board. Read the serial
	// number contained in the board's PROM and use it to search the configuration file for information on this board. 
	// This must be done before anything else as all other functions require this information.
	//    if (dev_config[BoardNum].config_file_found != 1)   // getboardconfiginfo is safer now that we do not read serial number inside
	GetBoardConfigInfo(BoardNum);  // not thread safe,  pre-called by uvAPI
	//    if (dev_config[BoardNum].is_ISLA216P)
	//        printf("(dev_config[BoardNum].is_ISLA216P)\n");
	//    if (dev_config[BoardNum].is_adc12d2000)
	//        printf("(dev_config[BoardNum].is_adc12d2000)\n");
	//    fflush(stdout);

	// Make sure the board type is supported    commented out DSchriebman 033015
	if (!((dev_config[BoardNum].is_ISLA216P) || (dev_config[BoardNum].is_ISLA214P) ||  (dev_config[BoardNum].is_AD5474)  ||  (dev_config[BoardNum].is_adc12d2000)))
	{
		//printf ("No supported board type found in ultra_config.dat\n");
		//fflush(stdout);
		//return false;
	}

	unsigned int is_SetupOnceAlready =  GetSetupDoneBitValue(BoardNum);
	//    printf("is_SetupOnceAlready = %d\n",is_SetupOnceAlready);
	//    fflush(stdout);
	// If this is the first time running SetupBoard, initialize default values to the board.
	// If not, skip default. This is done first to ensure proper clock readouts which depend on adc_chan_used
	if (!is_SetupOnceAlready)
	{        
		SET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,0 ,0, 0);

		long tmp;
		tmp =  GET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,0,0);
		tmp &= ~(0xf << CLOCK_SELECTED_ADDR);
		tmp |=  (CLOCK_NONE << CLOCK_SELECTED_ADDR);
		SET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,tmp,0, 0);
		tmp =  GET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,0,0);
		tmp &= ~(0xf << CH_MODE_ADDR);
		tmp |=  (CH_MODE_NONE << CH_MODE_ADDR);
		SET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,tmp,0, 0);
		tmp =  GET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,0,0);
		tmp &= ~(0xf << CH_SELECT_ADDR);
		tmp |=  (CH_SELECT_NONE << CH_SELECT_ADDR);
		SET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,tmp,0, 0);

		//		long clkSelected = (GET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,0,0) >> CLOCK_SELECTED_ADDR) & 0xf;
		//		long chMode		 = (GET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,0,0) >> CH_MODE_ADDR) & 0xf ;
		//		long chSelected  = (GET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,0,0) >> CH_SELECT_ADDR) & 0xf;

		//		printf("clkSelected = %ld\n",clkSelected);
		//		printf("chMode = %ld\n",chMode);
		//		printf("chSelected = %ld\n",chSelected);

		printf("First setup call. Setting default parameters.\n");
		if (dev_config[BoardNum].is_adc12d2000)
		{
			SET_CONTROL_REG(BoardNum,INT_CLOCK_EN, 1);
		}
		bool synthsetup = SetupMicrosynth(BoardNum);
		printf ("microsynth setup success = %d:\n",synthsetup);

		// Default the board to acquire data from all channels
		if (dev_config[BoardNum].is_ISLA216P)
		{
			if (dev_config[BoardNum].adc_chan == 4)
				SelectAdcChannels(BoardNum, (IN0 | IN1 | IN2 | IN3));
			else if (dev_config[BoardNum].adc_chan == 2)
				SelectAdcChannels(BoardNum, (IN0 | IN3));
			else if (dev_config[BoardNum].adc_chan == 1)
				SelectAdcChannels(BoardNum, IN0);
			else
				SelectAdcChannels(BoardNum, (IN0 | IN3));
		}
		if ((dev_config[BoardNum].is_ISLA214P)  || (dev_config[BoardNum].is_AD5474))
		{
			if (dev_config[BoardNum].adc_chan == 2)
				SelectAdcChannels(BoardNum, ((IN0 | IN2) | (IN1 | IN3)));
			else if (dev_config[BoardNum].adc_chan == 1)
				SelectAdcChannels(BoardNum, (IN0 | IN2));
		}

		if (dev_config[BoardNum].is_adc12d2000)
		{
			SET_CONTROL_REG(BoardNum,INT_CLOCK_EN, 1);    //internal clock must be set before the channel mode select or it will default to external
			ADC12D2000_INITIALIZE(BoardNum);
			// Default to two channel mode
			ADC12D2000_Channel_Mode_Select(BoardNum, TWO_CH_MODE, CH_SELECT_NONE, 0);
			// Default to no decimation
			SetAdcDecimation(BoardNum, 1);
		}

		// Default to Internal Clock On
		SetInternalClockEnable(BoardNum, CLOCK_INTERNAL);

		// Default to AcquireDisableInvert Off
		SetAcquireDisableInvert(BoardNum, 0);

		// Default the board to no trigger (free-run)
		if ((dev_config[BoardNum].is_ISLA216P) || (dev_config[BoardNum].is_ISLA214P) ||  (dev_config[BoardNum].is_AD5474))
		{
			SelectTrigger(BoardNum, NO_TRIGGER, FALLING_EDGE, 0);
			// The desired depth (in multiples of 8) is actually half the value sent to the FPGA
			SET_CAPTURE_DEPTH(BoardNum, 0);
			SLEEP(1);

			SET_CAPTURE_COUNT(BoardNum, 0);
			SLEEP(1);

			// Default the waveform trigger threshold to mid-scale
			// Default to FALLING_EDGE because that is the same as AcquireDisableInvert which is active LOW (free-ru
			unsigned int adc_fs = 1;

			for (int i=0;i<dev_config[BoardNum].adc_res;i++)
			{
				adc_fs *= 2;
			}
			unsigned int threshold = adc_fs /2 ;
			unsigned int hysterysis = adc_fs / 512;
			printf("SetupBoard: %d\t%d\n",threshold,hysterysis);
			ConfigureWaveformTrigger(BoardNum, threshold,hysterysis);

			// Default the segmented capture to zero (off)
			ConfigureSegmentedCapture(BoardNum, 0, 0, 0);

			// Default the Averager to zero (off)
			ConfigureAverager(BoardNum, 0, 4096, 0); // always a safe length, we will set the proper length when we configure averager on

			// Default Fiducial mark to 0 (off)
			SetFiducialMarks(BoardNum, 0);

			//Initialize pretrigger memory
			CONFIGURE_PRETRIGGER(BoardNum);
		}

		if (dev_config[BoardNum].is_adc12d2000)
		{
			SET_ECL_TRIGGER_DELAY(BoardNum, dev_config[BoardNum].ecl_trigger_delay);
			// Now that the firmware knows the DCM phase shift value to use in its ADC unreset state machine
			// either reset/unreset the board, or hold it in reset and await an external ECL trigger/sync signal.
			ECLTriggerEnable(BoardNum, 0);    // 100510
		}
	}
	//    else
	//    {
	//        if ((dev_config[BoardNum].is_ISLA216P) || (dev_config[BoardNum].is_ISLA214P))
	//        {
	//            // this is here to compensate for the unexplained behavior when changing from low to high freq.  Note that the DLL range is set by reading adc clkfreq, which doesn't seem to work without added nreset
	//            int sleeptime = 0;
	//            int adc_clock_freq = AdcClockGetFreq(BoardNum);
	//
	//            ADC_NRESET(BoardNum, 0xf);  // ONLY for ISLA devices
	//
	//    //		printf("DLL clock frequency = %d\n",adc_clock_freq);
	//            if(adc_clock_freq == 0)
	//                printf("adc_clock_freq = 0");
	//            else
	//            {
	//                if (dev_config[BoardNum].is_ISLA216P)
	//                    sleeptime = 550*250/adc_clock_freq;
	//                else
	//                    sleeptime = 550*500/adc_clock_freq;
	//            }
	//
	//            Sleep(sleeptime);
	//        }
	//    }
	// When SetupBoard has already been called once, this function protects adc_chan_used from being overriden
	// by the default value from GetBoardConfigInfo. If not set here, AdcClockGetFreq will be incorrect.
	//dev_config[BoardNum].adc_chan_used = getNumChannels(BoardNum);  // REVIEW this MI

	// Setup the most important driver and board settings, common to all boards
	CONFIGURE_CONTROL_REGISTER(BoardNum);  // this no longer turns on internal clock

	MainBoardSetup(BoardNum);




	if ( (dev_config[BoardNum].is_ISLA216P) || (dev_config[BoardNum].is_ISLA214P) )
	{
		prev_channelselect = GetChannelSelectValue(BoardNum);
		int tmp =0;
		for (int i=0;i<4;i++)
		{
			if ((prev_channelselect >> i) & 0x01) tmp ++;
		}

		if (tmp >2) tmp = 4;
		dev_config[BoardNum].adc_chan_used = tmp;
		//		ISLA_AdcDacSerialSetup(BoardNum);			//already called in calibrate board
		// SET_ADC_OFFSETS(BoardNum);
	}    
	if (( (dev_config[BoardNum].is_ISLA216P) || (dev_config[BoardNum].is_ISLA214P) || (dev_config[BoardNum].is_AD5474) ) && !is_SetupOnceAlready)
	{
		CalibrateBoard(BoardNum);
		//		ISLA_AdcDacSerialSetup(BoardNum);			//already called in calibrate board
		// SET_ADC_OFFSETS(BoardNum);
		printf("\n\nFirst setup done\n\n");
	}

	if ( !dev_config[BoardNum].is_ISLA216P && !dev_config[BoardNum].is_ISLA214P && !dev_config[BoardNum].is_AD5474 && !dev_config[BoardNum].is_adc12d2000 && !is_SetupOnceAlready)
	{
		//AD8 calibrate
		CalibrateBoard(BoardNum);
	}
	// issue I2E engine start and power settings
	//	SET_RESERVED53_REG(BoardNum, 0xF, 0x1);
	//	SET_RESERVED55_REG(BoardNum, 0xF, 0x1);
	//	SET_RESERVED31_REG(BoardNum, 0xF, 0x21);
	// Get the ADC clock frequency so we can setup the board for capture
	adc_clock_freq = AdcClockGetFreq(BoardNum);

	// Adjust DRAM IDELAY capture values (122810)
	//DramIdelayShift(DRAM_DQS_IDELAY_VAL, DRAM_DQ_IDELAY_VAL);

	if ((!is_SetupOnceAlready) && (dev_config[BoardNum].is_adc12d2000))
	{
		printf("ADC12D2000_Calibrate_ADC(%d)\n",BoardNum);

		ADC12D2000_Calibrate_ADC(BoardNum);
	}

	SetSetupDoneBit(BoardNum, 1);

	#ifdef DEBUG_TO_FILE

	debugFile = fopen("c:\\uvdma\\debug.txt","a");
	tm = GetTickCount();
	fprintf(debugFile,"CDllDriverApi::SetupBoard\t\t\t%08ld\t%d\t end\n",tm  ,BoardNum);
	fflush(debugFile);
	fclose(debugFile);
	#endif
	return true;
}


// Setup the most important driver variables and board parameters the order of these calls is important.
void MainBoardSetup(unsigned short BoardNum)
{
	#ifdef DEBUG_TO_FILE
	FILE *debugFile;
	debugFile = fopen("c:\\uvdma\\debug.txt","a");
	long tm = GetTickCount();
	fprintf(debugFile,"CDllDriverApi::MainBoardSetup\t\t\t%08ld\t%d\t begin\n",tm  ,BoardNum);
	fflush(debugFile);
	fclose(debugFile);
	#endif	
	unsigned long readval = 0;
	unsigned int onBoardMem;

	onBoardMem = GetOnBoardMemorySize(BoardNum);

	readval = GET_PIO_REG(BoardNum, PIO_OFFSET_FIRMWARE_VERSION_RD);
	//	printf("Firmware Version = %ld\n",readval);

	readval = GET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ);


	dev_config[BoardNum].board_rev = (readval & 0xE0000000) >> 29;
	dev_config[BoardNum].board_type = (readval & 0x000F0000) >> 16;
	//    printf("board_type=%d board_rev=%d\n", dev_config[BoardNum].board_type, dev_config[BoardNum].board_rev);

	//	SET_NUMBLOCKS_TO_ACQUIRE(BoardNum, 0);

	// Must be set before setting PCIe/DAC start/end block
	SET_BLOCKSIZE(BoardNum, DIG_BLOCKSIZE);
	SLEEP(1);

	// Tell driver to monitor A/D and D/A under/overruns
	if (PREVENT_UNDER_OVER_RUNS)
	{
		SET_PREVENT_UNDEROVERRUNS(BoardNum, 1);
	}
	else
	{
		SET_PREVENT_UNDEROVERRUNS(BoardNum, 0);
	}

	// auto set in internal_clock_en routine
	if (SEL_READ_WHOLE_ROW)
	{
		SET_CONTROL_REG(BoardNum,READ_WHOLE_ROW, 1);
	}
	else
	{
		SET_CONTROL_REG(BoardNum,READ_WHOLE_ROW, 0);
	}

	if (SEL_WRITE_WHOLE_ROW)
	{
		SET_CONTROL_REG(BoardNum,WRITE_WHOLE_ROW, 1);
	}
	else
	{
		SET_CONTROL_REG(BoardNum,WRITE_WHOLE_ROW, 0);
	}

	if (SEL_INTERLEAVED_RW)
	{
		SET_CONTROL_REG(BoardNum,INTERLEAVED_RW, 1);
	}
	else
	{
		SET_CONTROL_REG(BoardNum,INTERLEAVED_RW, 0);
	}


	SLEEP(1);
	SET_DA_DMA_BLOCKS_PER_BOARD(BoardNum, onBoardMem);

	// Reset to start PCIe DRAM reads at block 0	
	SET_PCIE_RD_START_BLOCK(BoardNum,0);


	//	printf("SET_PCIE_RD_END_BLOCK %d\n", (onBoardMem - 1));
	SET_PCIE_RD_END_BLOCK(BoardNum, onBoardMem-1);   // device driver still converts these to DRAM address
	SLEEP(1);

	SET_SHADOW_REG(BoardNum, PIO_OFFSET_DAC_WR_START_BLOCK, 0, 0, 0);
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_DAC_WR_END_BLOCK, 8191, 0, 0);
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_DAC_RD_START_BLOCK, 0, 0, 0);
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_DAC_RD_END_BLOCK, 8191, 0, 0);

	// Set internal or external clock

	SetContinuousAutoRefresh (BoardNum, dev_config[BoardNum].continuous_arf);

	CONFIGURE_ADCDATASETUP_REGISTER(BoardNum);

	CONFIGURE_DRAMADJ_REGISTER(BoardNum);

	CONFIGURE_CAPTUREADJ_REGISTER(BoardNum);
	// For 12/14 bit boards
	// Reset/Unreset before calling AdcClockGetFreq, otherwise no strobe
	// Reset/Unreset the IDELAY control and the IDELAYs
	//pio_reg[BoardNum].CaptureAdjReg |= (1 << IDELAY_RST) | (1<<IDELAY_CTRL_RST);
	//SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
	//SetIdelayResetCtrlReset(1);
	//SLEEP(1);

	//pio_reg[BoardNum].CaptureAdjReg &= ~(1<<IDELAY_RST);
	//pio_reg[BoardNum].CaptureAdjReg &= ~(1<<IDELAY_CTRL_RST);	
	//SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
	//SetIdelayResetCtrlReset(0);
	//SLEEP(1);

	//	CONFIGURE_PRETRIGGER(BoardNum);   

	#ifdef DEBUG_TO_FILE
	debugFile = fopen("c:\\uvdma\\debug.txt","a");
	tm = GetTickCount();
	fprintf(debugFile,"CDllDriverApi::MainBoardSetup\t\t\t%08ld\t%d\t end\n",tm  ,BoardNum);
	fflush(debugFile);
	fclose(debugFile);
	#endif	
	SLEEP(1);

}

//ADC12D2000 12-bit SPI word
unsigned int makeADC12D2000_SPI(unsigned int ReadWrite,unsigned int spiAddress,unsigned int spiData)
{
	unsigned int spiWord = 0;
	unsigned int tmp;

	tmp = (ReadWrite << ADC12D2000_SPI_READWRITE_BIT_POS);
	spiWord |= tmp;
	tmp = (1 << ADC12D2000_SPI_RESERVED_23_BIT_POS);
	spiWord |= tmp;
	tmp = (0 << ADC12D2000_SPI_RESERVED_22_BIT_POS);
	spiWord |= tmp;
	tmp = (spiAddress << ADC12D2000_SPI_ADDRESS_BIT_POS);
	spiWord |= tmp;
	tmp = (0 << ADC12D2000_SPI_RESERVED_16_BIT_POS);
	spiWord |= tmp;
	tmp = (spiData << ADC12D2000_SPI_DATA_BIT_POS);
	spiWord |= tmp;

	return spiWord;
}

EXPORTED_FUNCTION unsigned long get_overruns(unsigned short BoardNum)
{
	return GET_OVERRUNS(BoardNum);
}


EXPORTED_FUNCTION void ADC12D2000_Channel_Mode_Select (unsigned short BoardNum, unsigned int Chan_mode, unsigned int Chan_select, unsigned int Calibrate)
{
	unsigned int val = 0;
	unsigned int clock_freq;

	// makeAtomic[BoardNum].lock();
	busy++;


	#ifdef DEBUG_TO_FILE
	FILE *debugFile;
	debugFile = fopen("c:\\uvdma\\debug.txt","a");
	long tm = GetTickCount();
	fprintf(debugFile,"LOCKED =%d @%lu ADC12D2000_Channel_Mode_Select\t\t\t%08ld\t%d\t begin\n",busy, (unsigned long)&busy,   tm  ,BoardNum);
	fprintf(debugFile,"Chan_mode = %d\n",Chan_mode);
	fprintf(debugFile,"Chan_select = %d\n",Chan_select);
	fprintf(debugFile,"Calibrate = %d\n",Calibrate);
	fflush(debugFile);
	fclose(debugFile);
	#endif

	long chanMode	 = (GET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,0,0) >> CH_MODE_ADDR) & 0xf ;
	long chanSelect  = (GET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,0,0) >> CH_SELECT_ADDR) & 0xf;

	//    printf("ADC12D2000_Channel_Mode_Select:  Chan_mode = %d prior = %d    Chan_select = %d chanSelect = %d\n",Chan_mode,chanMode,Chan_select,chanSelect);
	if ((chanMode != Chan_mode) || (chanSelect != Chan_select))
	{
		//        printf("ADC12D2000_Channel_Mode_Select:setting registers\n");

		long tmp;
		tmp =  GET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,0,0);
		tmp &= ~(0xf << CH_MODE_ADDR);
		tmp |=  (Chan_mode << CH_MODE_ADDR);
		SET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,tmp,0, 0);
		tmp =  GET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,0,0);
		tmp &= ~(0xf << CH_SELECT_ADDR);
		tmp |=  (Chan_select << CH_SELECT_ADDR);
		SET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,tmp,0, 0);

		chanMode	= (GET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,0,0) >> CH_MODE_ADDR) & 0xf ;
		chanSelect  = (GET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,0,0) >> CH_SELECT_ADDR) & 0xf;

		//		printf("Chan_mode = %d chanMode = %d    Chan_select = %d chanSelect = %d\n",Chan_mode,chanMode,Chan_select,chanSelect);

		clock_freq = AdcClockGetFreq(BoardNum);

		if (clock_freq <= 300)
			ADC12D2000_SetLowFrequencySelect(BoardNum, 1);
		else
			ADC12D2000_SetLowFrequencySelect(BoardNum, 0);

		switch (Chan_mode)
		{
			case ONE_CH_MODE:
				dev_config[BoardNum].adc_chan_used = 1;
				switch (Chan_select)
				{
					case DESI_MODE:
						ADC12D2000_SetDualEdgeSamplingMode(BoardNum, 1);
						ADC12D2000_SetDESIQInputSelect(BoardNum, 0);
						ADC12D2000_SetDESIQCLKMode(BoardNum, 0);
						ADC12D2000_SetDESQInputSelect(BoardNum, 0);
						break;
					case DESQ_MODE:
						ADC12D2000_SetDualEdgeSamplingMode(BoardNum, 1);
						ADC12D2000_SetDESQInputSelect(BoardNum, 1);
						ADC12D2000_SetDESIQInputSelect(BoardNum, 0);
						ADC12D2000_SetDESIQCLKMode(BoardNum, 0);		
						break;
					case DESIQ_MODE:
						ADC12D2000_SetDualEdgeSamplingMode(BoardNum, 1);
						ADC12D2000_SetDESQInputSelect(BoardNum, 0);
						ADC12D2000_SetDESIQInputSelect(BoardNum, 1);
						ADC12D2000_SetDESIQCLKMode(BoardNum, 0);
						break;
					case DESCLKIQ_MODE:
						ADC12D2000_SetDualEdgeSamplingMode(BoardNum, 0);
						ADC12D2000_SetDESQInputSelect(BoardNum, 0);
						ADC12D2000_SetDESIQInputSelect(BoardNum, 0);
						ADC12D2000_SetDESIQCLKMode(BoardNum, 1);
						break;
				}
				break;
			case TWO_CH_MODE:
				dev_config[BoardNum].adc_chan_used = 2;
				ADC12D2000_SetDualEdgeSamplingMode(BoardNum, 0);
				ADC12D2000_SetDESQInputSelect(BoardNum, 0);
				ADC12D2000_SetDESIQInputSelect(BoardNum, 0);
				ADC12D2000_SetDESIQCLKMode(BoardNum, 0);
				break;
		}

		ADC12D2000_Calibrate_ADC(BoardNum);
		ADC12D2000_Set_Offsets(BoardNum, Chan_mode, Chan_select);
		ADC12D2000_Set_Gains(BoardNum, Chan_mode, Chan_select);
	}
	else
	{
		if (Calibrate == 1)
		{
			ADC12D2000_Calibrate_ADC(BoardNum);
			ADC12D2000_Set_Offsets(BoardNum, Chan_mode, Chan_select);
			ADC12D2000_Set_Gains(BoardNum, Chan_mode, Chan_select);
		}
	}
	/*
	if (ss->labview == 1)
	{
		//Calibrate using same command as used above
		val = makeADC12D2000_SPI (ADC12D2000_WRITE,CONFIGURATION_REGISTER_1,(0x0000 | config_reg1_copy)); //
		SET_PIO_REG(PIO_OFFSET_ADCSERIAL, val);
		Sleep(1);

		val = makeADC12D2000_SPI (ADC12D2000_WRITE,CONFIGURATION_REGISTER_1,(0x8000 | config_reg1_copy)); //
		SET_PIO_REG(PIO_OFFSET_ADCSERIAL, val);
		Sleep(1);

		val = makeADC12D2000_SPI (ADC12D2000_WRITE,CONFIGURATION_REGISTER_1,(0x0000 | config_reg1_copy)); //
		SET_PIO_REG(PIO_OFFSET_ADCSERIAL, val);
		Sleep(1);
	}
	*/
	#ifdef DEBUG_TO_FILE
	debugFile = fopen("c:\\uvdma\\debug.txt","a");
	tm = GetTickCount();
	fprintf(debugFile,"ADC12D2000_Channel_Mode_Select\t\t\t%08ld\t%d\t end\n",tm  ,BoardNum);
	fflush(debugFile);
	fclose(debugFile);
	#else
	SLEEP(10);
	#endif
	//	makeAtomic[BoardNum].unlock();

}

void ADC12D2000_Set_Offsets (unsigned short BoardNum, unsigned int Chan_mode, unsigned int Chan_select)
{
	switch (Chan_mode)
	{
		case ONE_CH_MODE:
			switch (Chan_select)
			{
				case DESI_MODE:
					ADC12D2000_SET_I_CHANNEL_OFFSET_ADJUST(BoardNum, dev_config[BoardNum].adc3_off_neg, dev_config[BoardNum].adc3_off);
					ADC12D2000_SET_Q_CHANNEL_OFFSET_ADJUST(BoardNum, dev_config[BoardNum].ADC12D2000_desi_q_offset_neg, dev_config[BoardNum].ADC12D2000_desi_q_offset);
					break;
				case DESQ_MODE:
					ADC12D2000_SET_I_CHANNEL_OFFSET_ADJUST(BoardNum, dev_config[BoardNum].ADC12D2000_desq_i_offset_neg, dev_config[BoardNum].ADC12D2000_desq_i_offset);
					ADC12D2000_SET_Q_CHANNEL_OFFSET_ADJUST(BoardNum, dev_config[BoardNum].adc3_off_neg_Q, dev_config[BoardNum].adc3_off_Q);
					break;
				case DESIQ_MODE:
					ADC12D2000_SET_I_CHANNEL_OFFSET_ADJUST(BoardNum, dev_config[BoardNum].ADC12D2000_desiq_i_offset_neg, dev_config[BoardNum].ADC12D2000_desiq_i_offset);
					ADC12D2000_SET_Q_CHANNEL_OFFSET_ADJUST(BoardNum, dev_config[BoardNum].ADC12D2000_desiq_q_offset_neg, dev_config[BoardNum].ADC12D2000_desiq_q_offset);
					break;
				case DESCLKIQ_MODE:
					ADC12D2000_SET_I_CHANNEL_OFFSET_ADJUST(BoardNum, dev_config[BoardNum].ADC12D2000_desclkiq_i_offset_neg, dev_config[BoardNum].ADC12D2000_desclkiq_i_offset);
					ADC12D2000_SET_Q_CHANNEL_OFFSET_ADJUST(BoardNum, dev_config[BoardNum].ADC12D2000_desclkiq_q_offset_neg, dev_config[BoardNum].ADC12D2000_desclkiq_q_offset);
					break;
			}
			break;
		case TWO_CH_MODE:
			ADC12D2000_SET_I_CHANNEL_OFFSET_ADJUST(BoardNum, dev_config[BoardNum].adc3_off_neg, dev_config[BoardNum].adc3_off);
			ADC12D2000_SET_Q_CHANNEL_OFFSET_ADJUST(BoardNum, dev_config[BoardNum].adc3_off_neg_Q, dev_config[BoardNum].adc3_off_Q);
			break;
	}
}

void ADC12D2000_Set_Gains (unsigned short BoardNum, unsigned int Chan_mode, unsigned int Chan_select)
{
	switch (Chan_mode)
	{
		case ONE_CH_MODE:
			switch (Chan_select)
			{
				case DESI_MODE:
					ADC12D2000_SET_I_CHANNEL_FULL_SCALE_RANGE_ADJUST(BoardNum, dev_config[BoardNum].adc3_fsr);
					ADC12D2000_SET_Q_CHANNEL_FULL_SCALE_RANGE_ADJUST(BoardNum, dev_config[BoardNum].adc3_fsr_Q);
					break;
				case DESQ_MODE:
					ADC12D2000_SET_I_CHANNEL_FULL_SCALE_RANGE_ADJUST(BoardNum, dev_config[BoardNum].adc3_fsr);
					ADC12D2000_SET_Q_CHANNEL_FULL_SCALE_RANGE_ADJUST(BoardNum, dev_config[BoardNum].adc3_fsr_Q);
					break;
				case DESIQ_MODE:
					ADC12D2000_SET_I_CHANNEL_FULL_SCALE_RANGE_ADJUST(BoardNum, dev_config[BoardNum].adc3_fsr);
					ADC12D2000_SET_Q_CHANNEL_FULL_SCALE_RANGE_ADJUST(BoardNum, dev_config[BoardNum].adc3_fsr_Q);
					break;
				case DESCLKIQ_MODE:
					ADC12D2000_SET_I_CHANNEL_FULL_SCALE_RANGE_ADJUST(BoardNum, dev_config[BoardNum].ADC12D2000_desclkiq_i_fsr);
					ADC12D2000_SET_Q_CHANNEL_FULL_SCALE_RANGE_ADJUST(BoardNum, dev_config[BoardNum].ADC12D2000_desclkiq_q_fsr);
					break;
			}
			break;
		case TWO_CH_MODE:
			ADC12D2000_SET_I_CHANNEL_FULL_SCALE_RANGE_ADJUST(BoardNum, dev_config[BoardNum].adc3_fsr);
			ADC12D2000_SET_Q_CHANNEL_FULL_SCALE_RANGE_ADJUST(BoardNum, dev_config[BoardNum].adc3_fsr_Q);
			break;
	}
}

////////////////////////////////////////////////////////////////
// Adc Section - START 
////////////////////////////////////////////////////////////////

// 8-bit board trigger/synchronization
// Now that the firmware knows the DCM phase shift value to use in its ADC unreset state machine
// either reset/unreset the board, or hold it in reset and await an external ECL trigger/sync signal.
EXPORTED_FUNCTION void ECLTriggerEnable(unsigned short BoardNum, unsigned int Enable)
{

	#ifdef DEBUG_TO_FILE
	FILE *debugFile;
	debugFile = fopen("c:\\uvdma\\debug.txt","a");
	long tm = GetTickCount();
	fprintf(debugFile,"ECLTriggerEnable\t\t\t%08ld\t%d\t begin\n",tm  ,BoardNum);
	fprintf(debugFile,"Enable = %d\n",Enable);
	fflush(debugFile);
	fclose(debugFile);
	#endif

	if (Enable > 0)
	{
		// Place 8-bit ADC Reset Sync F/F in reset
		AdcReset(BoardNum);

		// Tristate
		AdcTristate(BoardNum);

		// if we set this here, then we are unable to use the exported SET_ECL_TRIGGER_DELAY, as we will always overwrite with dev_config val
		//		SET_ECL_TRIGGER_DELAY(BoardNum, dev_config[BoardNum].ecl_trigger_delay);

		SET_ARMED(BoardNum);
		SLEEP(10);
		/*
		if(ss->adc_ecl_trigger_create)
\t    {
\1	printf("In reset ARMED, Hit return: CaptureAdjReg=0x%x\n", pio_reg[BoardNum].CaptureAdjReg);
	loop_bool = 1;
		while (loop_bool == 1)  {	if((c = getchar()) == '\n'){loop_bool = 0;}   }
		pio_reg[BoardNum].CaptureAdjReg &= ~(1<< ADC_DEBUG_SYNC );
		SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
		SLEEP(1);
		// Hit External Sync
		pio_reg[BoardNum].CaptureAdjReg |= (1<< ADC_DEBUG_SYNC );
		SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
		SLEEP(1);
	}
		*/
	}
	// Don't await trigger, just reset/unreset the DCM locking state machine manually
	else
	{
		// Do a final 8-bit reset/unreset now that the firmware knows the DCM phase shift value to use in its ADC unreset state machine
		// Reset 8-bit ADCs, unreset edge is what causes unreset 121009


		//		AdcReset(BoardNum);

		// Unreset 8-bit ADCs, creates strobe, 121009
		AdcUnreset(BoardNum);
	}

	#ifdef DEBUG_TO_FILE
	debugFile = fopen("c:\\uvdma\\debug.txt","a");
	tm = GetTickCount();
	fprintf(debugFile,"ECLTriggerEnable\t\t\t%08ld\t%d\t end\n",tm  ,BoardNum);
	fflush(debugFile);
	fclose(debugFile);
	#endif
}

EXPORTED_FUNCTION unsigned int AdcClockGetFreq(unsigned short BoardNum)
{
	unsigned int clock_freq;
	if (!dev_config[BoardNum].is_AD5474 && !dev_config[BoardNum].is_adc12d2000 && !dev_config[BoardNum].is_ISLA216P){
		unsigned long readval;
		double clock_f;
		int strobe_count;
		unsigned int val;

		readval = GET_PIO_REG(BoardNum, PIO_OFFSET_PROMPROG);

		// Ensure ADC DCM is locked
		if( (readval & (1 << 9)) && (readval != 0xFFFFFFFF) )
		{
			/*
				// Bring bit11 of CaptureAdjReg lo->hi to inititate a strobe count
				pio_reg[BoardNum].CaptureAdjReg &= 0xFFFFF7FF;				//  (clear init strobe count)
			SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
			pio_reg[BoardNum].CaptureAdjReg |= 0x00000800;				//  (set strobe count)	3rd LS Nibble:	bit11->Init strobe count, bit9->single channel select
			SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
			pio_reg[BoardNum].CaptureAdjReg &= 0xFFFFF7FF;				//  (clear init strobe count)
			SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
			SLEEP(1);	// give time for strobe counter to finish
			*/
			// Bring bit11 of CaptureAdjReg lo->hi to inititate a strobe count

			val = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
			val &= 0xFFFFF7FF;				//  (clear init strobe count)
			SET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, val);
			val |= 0x00000800;				//  (set strobe count)	3rd LS Nibble:	bit11->Init strobe count, bit9->single channel select
			SET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, val);
			val &= 0xFFFFF7FF;				//  (clear init strobe count)
			SET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, val);
			SLEEP(10);	// give time for strobe counter to finish

			readval = GET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ);
			strobe_count = readval & 0x0000FFFF;
			//printf("AdcClockGetFreq::strobe_count=%ld\n",strobe_count);
			//printf("strobe_count = %d\n", strobe_count);

			//printf("strobe_count=%d\n", strobe_count);
			clock_f = (double)((dev_config[BoardNum].strobe_count_mult)/1024.0)*(double)(10*strobe_count);	// Compatible with 8/14 bit boards
			clock_freq = (unsigned int) clock_f;
		}
	}
	if (dev_config[BoardNum].is_adc12d2000)
	{
		unsigned long readval;
		double clock_f;
		int strobe_count;
		unsigned int val;

		readval = GET_PIO_REG(BoardNum, PIO_OFFSET_PROMPROG);

		// Ensure ADC DCM is locked
		if( (readval & (1 << 9)) && (readval != 0xFFFFFFFF) )
		{
			/*
		// Bring bit11 of CaptureAdjReg lo->hi to inititate a strobe count
		pio_reg[BoardNum].CaptureAdjReg &= 0xFFFFF7FF;				//  (clear init strobe count)
		SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
		pio_reg[BoardNum].CaptureAdjReg |= 0x00000800;				//  (set strobe count)	3rd LS Nibble:	bit11->Init strobe count, bit9->single channel select
		SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
		pio_reg[BoardNum].CaptureAdjReg &= 0xFFFFF7FF;				//  (clear init strobe count)
		SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
		SLEEP(1);	// give time for strobe counter to finish
		*/
			// Bring bit11 of CaptureAdjReg lo->hi to inititate a strobe count

			val = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
			val &= 0xFFFFF7FF;				//  (clear init strobe count)
			SET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, val);
			val |= 0x00000800;				//  (set strobe count)	3rd LS Nibble:	bit11->Init strobe count, bit9->single channel select
			SET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, val);
			val &= 0xFFFFF7FF;				//  (clear init strobe count)
			SET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, val);
			SLEEP(20);	// give time for strobe counter to finish

			readval = GET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ);
			strobe_count = readval & 0x0000FFFF;

			//printf("strobe_count=%d\n", strobe_count);
			clock_f = (double)((dev_config[BoardNum].strobe_count_mult)/1024.0)*(double)(10*strobe_count);	// Compatible with 8/14 bit boards
			clock_f *=2;            //hack to fix clock frequency, revist later Schriebman 11/18/14
			//printf("dev_config[BoardNum].is_adc12d2000 = %d\n",dev_config[BoardNum].is_adc12d2000);
			//printf("dev_config[BoardNum].adc_chan_used = %d\n",dev_config[BoardNum].adc_chan_used);
			if (dev_config[BoardNum].adc_chan_used == 2){
				clock_freq = (unsigned int) clock_f;}
			else{
				clock_freq = (unsigned int) clock_f;}   //was *2 DSchriebman 032615


			//            printf("AD12 Input clock frequency ~= %d Mhz \n", clock_freq);
		}
		else
		{
			printf("AD12 AdcClockGetFreq() failed. ADC DCM not locked!\n");
			clock_freq = 0;
		}

		if ((clock_f > REVERSE_STACKING_THRESHOLD) && (GetNumBlocksToAcquireValue(BoardNum) > 2))  //  use sample clock rate, not conversion rate!!!
		{
			SET_CONTROL_REG(BoardNum,READ_WHOLE_ROW, 1);
			SET_CONTROL_REG(BoardNum,WRITE_WHOLE_ROW, 1);
			SET_CONTROL_REG(BoardNum,INTERLEAVED_RW, 0);
			SLEEP(1);
			//	    SetContinuousAutoRefresh (BoardNum, 1);
		}
		else
		{
			//		SET_CONTROL_REG(BoardNum,READ_WHOLE_ROW, 1);
			//		SET_CONTROL_REG(BoardNum,WRITE_WHOLE_ROW, 1);
			//		SET_CONTROL_REG(BoardNum,INTERLEAVED_RW, 0);
			//	SetContinuousAutoRefresh (BoardNum, 0);
			SET_CONTROL_REG(BoardNum,READ_WHOLE_ROW, 1);
			SET_CONTROL_REG(BoardNum,WRITE_WHOLE_ROW, 1);
			SET_CONTROL_REG(BoardNum,INTERLEAVED_RW, 1);
			SLEEP(1);
		}
	}
	if (dev_config[BoardNum].is_ISLA216P)
	{
		unsigned long readval;
		double clock_f;
		int strobe_count;
		unsigned int val;

		readval = GET_PIO_REG(BoardNum, PIO_OFFSET_PROMPROG);

		// Ensure ADC DCM is locked
		if( (readval & (1 << 9)) && (readval != 0xFFFFFFFF) )
		{
			/*
		// Bring bit11 of CaptureAdjReg lo->hi to inititate a strobe count
		pio_reg[BoardNum].CaptureAdjReg &= 0xFFFFF7FF;				//  (clear init strobe count)
		SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
		pio_reg[BoardNum].CaptureAdjReg |= 0x00000800;				//  (set strobe count)	3rd LS Nibble:	bit11->Init strobe count, bit9->single channel select
		SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
		pio_reg[BoardNum].CaptureAdjReg &= 0xFFFFF7FF;				//  (clear init strobe count)
		SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
		SLEEP(1);	// give time for strobe counter to finish
		*/
			// Bring bit11 of CaptureAdjReg lo->hi to inititate a strobe count

			val = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
			val &= 0xFFFFF7FF;				//  (clear init strobe count)
			SET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, val);
			val |= 0x00000800;				//  (set strobe count)	3rd LS Nibble:	bit11->Init strobe count, bit9->single channel select
			SET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, val);
			val &= 0xFFFFF7FF;				//  (clear init strobe count)
			SET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, val);
			readval = GET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ);
			strobe_count = readval & 0x0000FFFF;
			//            printf("strobe_count = %d\n", strobe_count);
			SLEEP(20);	// give time for strobe counter to finish

			readval = GET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ);
			strobe_count = readval & 0x0000FFFF;
			//            printf("strobe_count = %d\n", strobe_count);
			clock_f = (double)((dev_config[BoardNum].strobe_count_mult)/1024.0)*(double)(10*strobe_count);	// Compatible with 8/14 bit boards

			unsigned tmp = (GET_SHADOW_REG(BoardNum, PIO_OFFSET_ADC_DATA_SETUP,0,0)  >> NUM_ADC_CHAN_OFFSET);
			tmp &= 0x0f;
			unsigned chUsed = 0;
			for (int i=0;i<4;i++)
			{
				if ( (tmp & (1 << i)) > 0)
					chUsed ++;
			}
			if (!((chUsed == 1) || (chUsed == 2) || (chUsed == 4)))
				chUsed = 4;

			//printf("dev_config[BoardNum].is_adc12d2000 = %d\n",dev_config[BoardNum].is_adc12d2000);
			//printf("dev_config[BoardNum].adc_chan_used = %d\n",dev_config[BoardNum].adc_chan_used);
			clock_f *= 2.0;
			clock_f /= chUsed;
			clock_freq = clock_f;
			//            printf("AD16 ch_sel = %d\tch used = %d\tInput clock frequency ~= %d Mhz \n", tmp, chUsed,clock_freq);
		}
		else
		{
			printf("AdcClockGetFreq() failed. ADC DCM not locked!\n");
			clock_freq = 0;
		}
	}
	if (dev_config[BoardNum].is_AD5474){
		unsigned long readval;
		double clock_f;
		int strobe_count;
		unsigned int val;

		readval = GET_PIO_REG(BoardNum, PIO_OFFSET_PROMPROG);

		// Ensure ADC DCM is locked
		if( (readval & (1 << 9)) && (readval != 0xFFFFFFFF) )
		{
			/*
			// Bring bit11 of CaptureAdjReg lo->hi to inititate a strobe count
			pio_reg[BoardNum].CaptureAdjReg &= 0xFFFFF7FF;				//  (clear init strobe count)
			SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
				pio_reg[BoardNum].CaptureAdjReg |= 0x00000800;				//  (set strobe count)	3rd LS Nibble:	bit11->Init strobe count, bit9->single channel select
			SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
			pio_reg[BoardNum].CaptureAdjReg &= 0xFFFFF7FF;				//  (clear init strobe count)
			SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
			SLEEP(1);	// give time for strobe counter to finish
			*/
			// Bring bit11 of CaptureAdjReg lo->hi to inititate a strobe count

			val = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
			val &= 0xFFFFF7FF;				//  (clear init strobe count)
			SET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, val);
			val |= 0x00000800;				//  (set strobe count)	3rd LS Nibble:	bit11->Init strobe count, bit9->single channel select
			SET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, val);
			val &= 0xFFFFF7FF;				//  (clear init strobe count)
			SET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, val);
			SLEEP(10);	// give time for strobe counter to finish

			readval = GET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ);
			strobe_count = readval & 0x0000FFFF;
			//printf("AdcClockGetFreq::strobe_count=%ld\n",strobe_count);
			//printf("strobe_count = %d\n", strobe_count);

			//printf("strobe_count=%d\n", strobe_count);
			clock_f = (double)((dev_config[BoardNum].strobe_count_mult)/1024.0)*(double)(10*strobe_count);	// Compatible with 8/14 bit boards

			//printf("dev_config[BoardNum].is_adc12d2000 = %d\n",dev_config[BoardNum].is_adc12d2000);
			//printf("dev_config[BoardNum].adc_chan_used = %d\n",dev_config[BoardNum].adc_chan_used);

			unsigned int chanSelVal = GetChannelSelectValue(BoardNum);
			int numCh = 0;
			switch (chanSelVal)
			{
				case 5:
					numCh = 1;
					break;
				case 10:
					numCh = 1;
					break;
				case (15):
					numCh = 2;
					break;
				default:
					printf("chanSelVal = %d does not match valid channel config\n", chanSelVal);
					break;
			}
			//printf("AdcClockGetFreq::chanSelVal=%d\n",chanSelVal);
			//printf("AdcClockGetFreq::numCh=%d\n",numCh);

			//1 channel 14-bit
			if ((dev_config[BoardNum].adc_res == 14) && (numCh == 1))
			{
				clock_freq = (unsigned int) clock_f*2;
			}
			//2 channel 14-bit
			else if ((dev_config[BoardNum].adc_res == 14) && (numCh == 2))
			{
				clock_freq = (unsigned int) clock_f;
			}
			else
			{
				clock_freq = (unsigned int) clock_f/2;
			}

			//printf("AdcClockGetFreq::clock_freq ~= %d Mhz \n", clock_freq);
		}
		else
		{
			printf("AdcClockGetFreq:: failed. ADC DCM not locked!\n");
			clock_freq = 0;
		}

	}


	if (clock_freq < 1){
		clock_freq = 1;}
	adcClockFrequency = clock_freq;
	return clock_freq;
}

////////////////////////////////////////////////////////////////
// Adc Section - END 
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
// Adc Section - START
////////////////////////////////////////////////////////////////

//EXPORTED_FUNCTION unsigned int AdcClockGetFreq(unsigned short BoardNum)
//{
//	unsigned long readval;
//    double clock_f;
//	int strobe_count;
//	unsigned int val, clock_freq;

//	readval = GET_PIO_REG(BoardNum, PIO_OFFSET_PROMPROG);

//	// Ensure ADC DCM is locked
//	if( (readval & (1 << 9)) && (readval != 0xFFFFFFFF) )
//	{
//		/*
//		// Bring bit11 of CaptureAdjReg lo->hi to inititate a strobe count
//		pio_reg[BoardNum].CaptureAdjReg &= 0xFFFFF7FF;				//  (clear init strobe count)
//		SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
//		pio_reg[BoardNum].CaptureAdjReg |= 0x00000800;				//  (set strobe count)	3rd LS Nibble:	bit11->Init strobe count, bit9->single channel select
//		SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
//		pio_reg[BoardNum].CaptureAdjReg &= 0xFFFFF7FF;				//  (clear init strobe count)
//		SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
//		SLEEP(1);	// give time for strobe counter to finish
//		*/
//		// Bring bit11 of CaptureAdjReg lo->hi to inititate a strobe count

//		val = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
//		val &= 0xFFFFF7FF;				//  (clear init strobe count)
//		SET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, val);
//		val |= 0x00000800;				//  (set strobe count)	3rd LS Nibble:	bit11->Init strobe count, bit9->single channel select
//		SET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, val);
//		val &= 0xFFFFF7FF;				//  (clear init strobe count)
//		SET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, val);
//		SLEEP(10);	// give time for strobe counter to finish

//		readval = GET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ);
//		strobe_count = readval & 0x0000FFFF;
//		//printf("strobe_count = %d\n", strobe_count);

//        //printf("strobe_count=%d\n", strobe_count);
//        clock_f = (double)((dev_config[BoardNum].strobe_count_mult)/1024.0)*(double)(10*strobe_count);	// Compatible with 8/14 bit boards

//		//printf("dev_config[BoardNum].is_adc12d2000 = %d\n",dev_config[BoardNum].is_adc12d2000);
//		//printf("dev_config[BoardNum].adc_chan_used = %d\n",dev_config[BoardNum].adc_chan_used);

//		//1 channel 16-bit
//		if ((dev_config[BoardNum].adc_res == 16) && (dev_config[BoardNum].adc_chan_used == 1))
//		{
//			clock_freq = (unsigned int) clock_f*4;
//		}
//		//2 channel 16-bit
//		else if ((dev_config[BoardNum].adc_res == 16) && (dev_config[BoardNum].adc_chan_used == 2))
//		{
//			clock_freq = (unsigned int) clock_f*2;
//		}
//		//4 channel 16-bit
//		else if ((dev_config[BoardNum].adc_res == 16) && (dev_config[BoardNum].adc_chan_used == 4))
//		{
//			clock_freq = (unsigned int) clock_f;
//		}
//		else
//		{
//			clock_freq = (unsigned int) clock_f;
//		}

//		//printf("Input clock frequency ~= %d Mhz \n", clock_freq);
//	}
//	else
//	{
//		printf("AdcClockGetFreq() failed. ADC DCM not locked!\n");
//		clock_freq = 0;
//	}

//	return clock_freq;
//}

void CalibrateBoard(unsigned short BoardNum)
{

	if (!dev_config[BoardNum].is_AD5474 && !dev_config[BoardNum].is_ISLA216P && !dev_config[BoardNum].is_adc12d2000){
		AdcCalibrate(BoardNum);
		printf("AD8 configured\n");
	}

	if (dev_config[BoardNum].is_AD5474){
		ISLA_AdcDacSerialSetup(BoardNum);
		ISLA_GainAdjust(BoardNum);
		SET_ADC_OFFSETS(BoardNum);
		printf("AD14 configured\n");
	}

	if (dev_config[BoardNum].is_ISLA216P)
	{
		unsigned long readval = 0;
		static unsigned char **adcReg = NULL;

		unsigned long val = 0l;

		unsigned  cal_status[4] = {0,0,0,0};
		unsigned int adc_clock_freq;

		adcReg = (unsigned char **) malloc(MAX_ADC * sizeof *adcReg);
		if (adcReg == NULL)
		{
			printf("unable to allocate memory for ADC registers\n");
			return;
		}
		else
		{
			for (int i=0;i<MAX_ADC;i++)
			{
				adcReg[i] = (unsigned char *)malloc (ISLA_NUMREG * sizeof *adcReg[i]);
				if (adcReg[i] == NULL)
				{
					i--;
					for (;i>=0;i--)
						free(adcReg[i]);
					free(adcReg);
					return;
				}
			}
		}

		//	if (dev_config[BoardNum].ISLA_2X_CLK == 1)
		//		SET_2X_CLOCK(BoardNum);
		//	else
		//		SET_1X_CLOCK(BoardNum);

		for (int i=0;i<4;i++)
			cal_status[i] = (((readval >> CAL_BIT_POS) >> i) & 1);

		printf("cal_status = %d\n", cal_status[0]);

		readISLA_Gain_Offset(BoardNum, adcReg);

		//printISLA_Gain_Offset(adcReg);

		if ((cal_status[0] == 0) || (cal_status[3] == 0))
		{
			printf("Calibrating ISLA ADCs\n");

			SET_CLOCK_DIV_RST_POL(BoardNum, 0xF, 1);

			adc_clock_freq = AdcClockGetFreq(BoardNum);

			ADC_NRESET(BoardNum, 0xF);

			int sleeptime = 550;

			if(adc_clock_freq == 0)
				printf("adc_clock_freq = 0\n");
			else
			{
				if (dev_config[BoardNum].is_ISLA216P)
					sleeptime = 550*250/adc_clock_freq;
				else
					sleeptime = 550*500/adc_clock_freq;
			}
			Sleep(sleeptime);// 6s  550ms required at 250MSPS  6s accounts for worst case cal time at 25MSPS


			//		if (dev_config[BoardNum].ISLA_2X_CLK == 1)
			//			SET_2X_CLOCK(BoardNum);
			//		else
			//			SET_1X_CLOCK(BoardNum);

			ADC_CLKDIVRST(BoardNum, 0xF);

			// Phase slip if external clock //////////////
			unsigned int int_clk_on = GetInternalClockValue(BoardNum);

			// Phase slip if using external clock on isla214
			if ((int_clk_on == 0) && (dev_config[BoardNum].is_ISLA214P))
				SET_PHASE_SLIP_REG(BoardNum, 0xf, 0x1);



			int range = 0;
			int lowClockTest = 90;
			if (dev_config[BoardNum].is_ISLA214P)  //214P low frequency twice as high as 216p
			{
				lowClockTest *= 2;
			}


			printf("adc_clock_freq=%d\n",adc_clock_freq);
			if (adc_clock_freq < lowClockTest)
			{
				range = 1;
				printf("using slow clock\n");
			}
			if (range==0){
				printf("using fast clock\n");
			}

			for (int i=0;i<MAX_ADC;i++)
			{
				SET_DLL_RANGE_REG(BoardNum, (1<<i), range);
			}

			//	/*
			AdcIDelayAuto(BoardNum, 0xF);
			unsigned int val = GetPretriggerValue(BoardNum);
			CONFIGURE_PRETRIGGER(BoardNum);
			SetPreTriggerMemory(BoardNum,val);

			//		ISLA_AdcIdelayCalibrate(BoardNum);

			// set cal_status = 1
			//		ADC_SETCALBITS(BoardNum, 1);

			//readISLA_Gain_Offset(BoardNum, adcReg);
			//printISLA_registers(adcReg);
			printf("done calibrating. adc gains and offsets auto set by nreset  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
		}

		ISLA_AdcDacSerialSetup(BoardNum);

		ISLA_GainAdjust(BoardNum);

		SET_ADC_OFFSETS(BoardNum);

		//printf("free memory array\n");
		for (int i=0;i<MAX_ADC;i++)
		{
			//printf("freeing adcReg[%d]\n",i);
			free(adcReg[i]);
		}

		//printf("free memory pointer\n");
		free(adcReg);
		printf("CalibrateBoard done\n\n");
	}
}

void SET_ADC_OFFSETS(unsigned short BoardNum)			//added by DSchriebman 7/25/14 to correct offsets
{
	//if value read in from ultra_config.dat, propogate it to the board
	//default to no change if calibration not available in ultra_config.dat

	if (dev_config[BoardNum].ISLA_ADC0_OFFSET_COARSE0 != 0x100){
		SET_OFFSET_COARSE_ADC0(BoardNum,0,dev_config[BoardNum].ISLA_ADC0_OFFSET_COARSE0);}
	if (dev_config[BoardNum].ISLA_ADC1_OFFSET_COARSE0 != 0x100){
		SET_OFFSET_COARSE_ADC0(BoardNum,1,dev_config[BoardNum].ISLA_ADC1_OFFSET_COARSE0);}
	if (dev_config[BoardNum].ISLA_ADC2_OFFSET_COARSE0 != 0x100){
		SET_OFFSET_COARSE_ADC0(BoardNum,2,dev_config[BoardNum].ISLA_ADC2_OFFSET_COARSE0);}
	if (dev_config[BoardNum].ISLA_ADC3_OFFSET_COARSE0 != 0x100){
		SET_OFFSET_COARSE_ADC0(BoardNum,3,dev_config[BoardNum].ISLA_ADC3_OFFSET_COARSE0);}

	if (dev_config[BoardNum].ISLA_ADC0_OFFSET_COARSE1 != 0x100){
		SET_OFFSET_COARSE_ADC1(BoardNum,0,dev_config[BoardNum].ISLA_ADC0_OFFSET_COARSE1);}
	if (dev_config[BoardNum].ISLA_ADC1_OFFSET_COARSE1 != 0x100){
		SET_OFFSET_COARSE_ADC1(BoardNum,1,dev_config[BoardNum].ISLA_ADC1_OFFSET_COARSE1);}
	if (dev_config[BoardNum].ISLA_ADC2_OFFSET_COARSE1 != 0x100){
		SET_OFFSET_COARSE_ADC1(BoardNum,2,dev_config[BoardNum].ISLA_ADC2_OFFSET_COARSE1);}
	if (dev_config[BoardNum].ISLA_ADC3_OFFSET_COARSE1 != 0x100){
		SET_OFFSET_COARSE_ADC1(BoardNum,3,dev_config[BoardNum].ISLA_ADC3_OFFSET_COARSE1);}

	if (dev_config[BoardNum].ISLA_ADC0_OFFSET_FINE0 != 0x100){
		SET_OFFSET_FINE_ADC0(BoardNum,0,dev_config[BoardNum].ISLA_ADC0_OFFSET_FINE0);}
	if (dev_config[BoardNum].ISLA_ADC1_OFFSET_FINE0 != 0x100){
		SET_OFFSET_FINE_ADC0(BoardNum,1,dev_config[BoardNum].ISLA_ADC1_OFFSET_FINE0);}
	if (dev_config[BoardNum].ISLA_ADC2_OFFSET_FINE0 != 0x100){
		SET_OFFSET_FINE_ADC0(BoardNum,2,dev_config[BoardNum].ISLA_ADC2_OFFSET_FINE0);}
	if (dev_config[BoardNum].ISLA_ADC3_OFFSET_FINE0 != 0x100){
		SET_OFFSET_FINE_ADC0(BoardNum,3,dev_config[BoardNum].ISLA_ADC3_OFFSET_FINE0);}

	if (dev_config[BoardNum].ISLA_ADC0_OFFSET_FINE1 != 0x100){
		SET_OFFSET_FINE_ADC1(BoardNum,0,dev_config[BoardNum].ISLA_ADC0_OFFSET_FINE1);}
	if (dev_config[BoardNum].ISLA_ADC1_OFFSET_FINE1 != 0x100){
		SET_OFFSET_FINE_ADC1(BoardNum,1,dev_config[BoardNum].ISLA_ADC1_OFFSET_FINE1);}
	if (dev_config[BoardNum].ISLA_ADC2_OFFSET_FINE1 != 0x100){
		SET_OFFSET_FINE_ADC1(BoardNum,2,dev_config[BoardNum].ISLA_ADC2_OFFSET_FINE1);}
	if (dev_config[BoardNum].ISLA_ADC3_OFFSET_FINE1 != 0x100){
		SET_OFFSET_FINE_ADC1(BoardNum,3,dev_config[BoardNum].ISLA_ADC3_OFFSET_FINE1);}

	//    printf("calibration forced away from nreset values\n");

}

void ISLA_GainAdjust (unsigned short BoardNum)
{

	unsigned long val = 0l;

	static unsigned char **regVal = NULL;

	regVal = (unsigned char **) malloc(MAX_ADC * sizeof *regVal);

	if (regVal == NULL)
	{
		printf("unable to allocate memory for ADC registers\n");
		return;
	}
	else
	{
		for (int i=0;i<MAX_ADC;i++)
		{
			regVal[i] = (unsigned char *)malloc (ISLA_NUMREG * sizeof *regVal[i]);
			if (regVal[i] == NULL)
			{
				i--;
				for (;i>=0;i--)
					free(regVal[i]);
				free(regVal);
				return;
			}
		}
	}

	readISLA_Gain_Offset(BoardNum, regVal);
	//printISLA_registers(regVal);
	printf("regVal[0][CHIP_ID]=%x\n",regVal[0][CHIP_ID]);					//still not sure why these conditions don't get met, value seems right
	if (0==0)//(regVal[0][CHIP_ID] == 0x49)  // ADC is present
	{
		// core 0
		SET_GAIN_COARSE_ADC0(BoardNum, 0, dev_config[BoardNum].ISLA_ADC0_GAIN_COARSE0);
		SET_GAIN_MEDIUM_ADC0(BoardNum, 0, dev_config[BoardNum].ISLA_ADC0_GAIN_MEDIUM0);
		SET_GAIN_FINE_ADC0(BoardNum, 0, dev_config[BoardNum].ISLA_ADC0_GAIN_FINE0);

		// core 1
		SET_GAIN_COARSE_ADC1(BoardNum, 0, dev_config[BoardNum].ISLA_ADC0_GAIN_COARSE1);
		SET_GAIN_MEDIUM_ADC1(BoardNum, 0, dev_config[BoardNum].ISLA_ADC0_GAIN_MEDIUM1);
		SET_GAIN_FINE_ADC1(BoardNum, 0, dev_config[BoardNum].ISLA_ADC0_GAIN_FINE1);
	}

	if (0==0)//(regVal[1][CHIP_ID] == 0x49)  // ADC is present
	{
		// core 0
		SET_GAIN_COARSE_ADC0(BoardNum, 1, dev_config[BoardNum].ISLA_ADC1_GAIN_COARSE0);
		SET_GAIN_MEDIUM_ADC0(BoardNum, 1, dev_config[BoardNum].ISLA_ADC1_GAIN_MEDIUM0);
		SET_GAIN_FINE_ADC0(BoardNum, 1, dev_config[BoardNum].ISLA_ADC1_GAIN_FINE0);

		// core 1
		SET_GAIN_COARSE_ADC1(BoardNum, 1, dev_config[BoardNum].ISLA_ADC1_GAIN_COARSE1);
		SET_GAIN_MEDIUM_ADC1(BoardNum, 1, dev_config[BoardNum].ISLA_ADC1_GAIN_MEDIUM1);
		SET_GAIN_FINE_ADC1(BoardNum, 1, dev_config[BoardNum].ISLA_ADC1_GAIN_FINE1);
	}

	if (0==0)//(regVal[2][CHIP_ID] == 0x49)  // ADC is present
	{
		// core 0
		SET_GAIN_COARSE_ADC0(BoardNum, 2, dev_config[BoardNum].ISLA_ADC2_GAIN_COARSE0);
		SET_GAIN_MEDIUM_ADC0(BoardNum, 2, dev_config[BoardNum].ISLA_ADC2_GAIN_MEDIUM0);
		SET_GAIN_FINE_ADC0(BoardNum, 2, dev_config[BoardNum].ISLA_ADC2_GAIN_FINE0);

		// core 1
		SET_GAIN_COARSE_ADC1(BoardNum, 2, dev_config[BoardNum].ISLA_ADC2_GAIN_COARSE1);
		SET_GAIN_MEDIUM_ADC1(BoardNum, 2, dev_config[BoardNum].ISLA_ADC2_GAIN_MEDIUM1);
		SET_GAIN_FINE_ADC1(BoardNum, 2, dev_config[BoardNum].ISLA_ADC2_GAIN_FINE1);
	}

	if (0==0)//(regVal[3][CHIP_ID] == 0x49)  // ADC is present
	{
		// core 0
		SET_GAIN_COARSE_ADC0(BoardNum, 3, dev_config[BoardNum].ISLA_ADC3_GAIN_COARSE0);
		SET_GAIN_MEDIUM_ADC0(BoardNum, 3, dev_config[BoardNum].ISLA_ADC3_GAIN_MEDIUM0);
		SET_GAIN_FINE_ADC0(BoardNum, 3, dev_config[BoardNum].ISLA_ADC3_GAIN_FINE0);

		// core 1
		SET_GAIN_COARSE_ADC1(BoardNum, 3, dev_config[BoardNum].ISLA_ADC3_GAIN_COARSE1);
		SET_GAIN_MEDIUM_ADC1(BoardNum, 3, dev_config[BoardNum].ISLA_ADC3_GAIN_MEDIUM1);
		SET_GAIN_FINE_ADC1(BoardNum, 3, dev_config[BoardNum].ISLA_ADC3_GAIN_FINE1);
	}

	int tmp;

	//   for (int adc_ch=0;adc_ch<4;adc_ch++)
	//   {
	//       //printf ("adc%d chip id= %x\n",adc_ch,regVal[adc_ch][CHIP_ID] );
	//       if (0==0)//(regVal[adc_ch][CHIP_ID] == 0x49)  // ADC is present
	//	{
	//		tmp = 0x01;
	//		//printf ("Setting gain adjust enable reg\n");
	//		//val = makeISLA_SPI(SPI_WRITE,(1<<adc_ch),OFFSET_GAIN_ADJUST_ENABLE_REG,tmp);
	//		//SET_PIO_REG(PIO_OFFSET_ADCSERIAL,val);
	//		//Sleep(1);
	//		SET_OFFSET_GAIN_ENABLE_REG(BoardNum, adc_ch, tmp);

	//		tmp = 0x00;
	//		//printf ("Clearing gain adjust enable reg\n");
	//		//val = makeISLA_SPI(SPI_WRITE,(1<<adc_ch),OFFSET_GAIN_ADJUST_ENABLE_REG,tmp);
	//		//SET_PIO_REG(PIO_OFFSET_ADCSERIAL,val);
	//		//Sleep(1);
	//		SET_OFFSET_GAIN_ENABLE_REG(BoardNum, adc_ch, tmp);
	//	}
	//}

	//printf("ISLA_GainAdjust complete\n");

	readISLA_Gain_Offset(BoardNum, regVal);
	printISLA_registers(regVal);

	//printf("free memory array\n");
	for (int i=0;i<MAX_ADC;i++)
	{
		//printf("freeing adcReg[%d]\n",i);
		free(regVal[i]);

	}
	//printf("free memory pointer\n");
	free(regVal);
}

void CONFIGURE_CHIP_PORT_CONFIG(unsigned short BoardNum, unsigned int adcNum)
{
	SetISLA_SDOActive(BoardNum, adcNum, 0);
	SetISLA_LSBFirst(BoardNum, adcNum, 0);
	SetISLA_SoftReset(BoardNum, adcNum, 0);
	SetISLA_ReservedBit4(BoardNum, adcNum);
}

void SetISLA_SDOActive(unsigned short BoardNum, unsigned int adcNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), CHIP_PORT_CONFIG);
	tmp &= ~(0x1 << SDO_Active);
	tmp &= ~(0x1 << Mirror_SDO_Active);
	val = ((Enable & 0x1) << SDO_Active);
	tmp |= val;
	val = ((Enable & 0x1) << Mirror_SDO_Active);
	tmp |= val;
	WriteISLA_SPI(BoardNum, (1 <<adcNum), CHIP_PORT_CONFIG, tmp);
}

void SetISLA_LSBFirst(unsigned short BoardNum, unsigned int adcNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), CHIP_PORT_CONFIG);
	tmp &= ~(0x1 << LSB_First);
	tmp &= ~(0x1 << Mirror_LSB_First);
	val = ((Enable & 0x1) << LSB_First);
	tmp |= val;
	val = ((Enable & 0x1) << Mirror_LSB_First);
	tmp |= val;
	WriteISLA_SPI(BoardNum, (1 <<adcNum), CHIP_PORT_CONFIG, tmp);
}

void SetISLA_SoftReset(unsigned short BoardNum, unsigned int adcNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), CHIP_PORT_CONFIG);
	tmp &= ~(0x1 << Soft_Reset);
	tmp &= ~(0x1 << Mirror_Soft_Reset);
	val = ((Enable & 0x1) << Soft_Reset);
	tmp |= val;
	val = ((Enable & 0x1) << Mirror_Soft_Reset);
	tmp |= val;
	WriteISLA_SPI(BoardNum, (1 <<adcNum), CHIP_PORT_CONFIG, tmp);
}

void SetISLA_ReservedBit4(unsigned short BoardNum, unsigned int adcNum)
{
	unsigned int val, tmp;

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), CHIP_PORT_CONFIG);
	tmp &= ~(0x1 << ReservedBit4);
	tmp &= ~(0x1 << Mirror_ReservedBit4);
	val = ((1 & 0x1) << ReservedBit4);
	tmp |= val;
	val = ((1 & 0x1) << Mirror_ReservedBit4);
	tmp |= val;
	WriteISLA_SPI(BoardNum, (1 <<adcNum), CHIP_PORT_CONFIG, tmp);
}

EXPORTED_FUNCTION void SET_OFFSET_COARSE_ADC0(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
{
	unsigned int val, tmp;

	SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x1);

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), OFFSET_COARSE_ADC0);
	tmp &= ~(0xff << Offset_Coarse);
	val = ((Value & 0xff) << Offset_Coarse);
	tmp |= val;
	WriteISLA_SPI(BoardNum, (1 <<adcNum), OFFSET_COARSE_ADC0, tmp);
	SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x0);
}

EXPORTED_FUNCTION void SET_OFFSET_FINE_ADC0(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
{
	unsigned int val, tmp;

	SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x1);

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), OFFSET_FINE_ADC0);
	tmp &= ~(0xff << Offset_Fine);
	val = ((Value & 0xff) << Offset_Fine);
	tmp |= val;
	WriteISLA_SPI(BoardNum, (1 <<adcNum), OFFSET_FINE_ADC0, tmp);

	SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x0);
}

EXPORTED_FUNCTION void SET_GAIN_COARSE_ADC0(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
{
	unsigned int val, tmp;

	SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x1);

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), GAIN_COARSE_ADC0);
	tmp &= ~(0xf << Gain_Coarse);
	val = ((Value & 0xf) << Gain_Coarse);
	tmp |= val;
	WriteISLA_SPI(BoardNum, (1 <<adcNum), GAIN_COARSE_ADC0, tmp);

	SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x0);
}

EXPORTED_FUNCTION void SET_GAIN_MEDIUM_ADC0(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
{
	unsigned int val, tmp;

	SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x1);

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), GAIN_MEDIUM_ADC0);
	tmp &= ~(0xff << Gain_Medium);
	val = ((Value & 0xff) << Gain_Medium);
	tmp |= val;
	WriteISLA_SPI(BoardNum, (1 <<adcNum), GAIN_MEDIUM_ADC0, tmp);

	SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x0);
}

EXPORTED_FUNCTION void SET_GAIN_FINE_ADC0(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
{
	unsigned int val, tmp;

	SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x1);

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), GAIN_FINE_ADC0);
	tmp &= ~(0xff << Gain_Fine);
	val = ((Value & 0xff) << Gain_Fine);
	tmp |= val;
	WriteISLA_SPI(BoardNum, (1 <<adcNum), GAIN_FINE_ADC0, tmp);

	SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x0);
}

void SET_MODES_ADC0(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
{
	unsigned int val, tmp;

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), MODES_ADC0);
	tmp &= ~(0x7 << Mode_BitPos);
	val = ((Value & 0x7) << Mode_BitPos);
	tmp |= val;
	WriteISLA_SPI(BoardNum, adcNum, MODES_ADC0, tmp);

	SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (1 <<adcNum), 0x0);
}

EXPORTED_FUNCTION void SET_OFFSET_COARSE_ADC1(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
{
	unsigned int val, tmp;

	SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x1);

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), OFFSET_COARSE_ADC1);
	tmp &= ~(0xff << Offset_Coarse);
	val = ((Value & 0xff) << Offset_Coarse);
	tmp |= val;
	WriteISLA_SPI(BoardNum, (1 <<adcNum), OFFSET_COARSE_ADC1, tmp);

	SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x0);
}

EXPORTED_FUNCTION void SET_OFFSET_FINE_ADC1(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
{
	unsigned int val, tmp;

	SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x1);

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), OFFSET_FINE_ADC1);
	tmp &= ~(0xff << Offset_Fine);
	val = ((Value & 0xff) << Offset_Fine);
	tmp |= val;
	WriteISLA_SPI(BoardNum, (1 <<adcNum), OFFSET_FINE_ADC1, tmp);

	SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x0);
}

EXPORTED_FUNCTION void SET_GAIN_COARSE_ADC1(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
{
	unsigned int val, tmp;


	SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x1);

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), GAIN_COARSE_ADC1);
	tmp &= ~(0xf << Gain_Coarse);
	val = ((Value & 0xf) << Gain_Coarse);
	tmp |= val;
	WriteISLA_SPI(BoardNum, (1 <<adcNum), GAIN_COARSE_ADC1, tmp);

	SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x0);
}

EXPORTED_FUNCTION void SET_GAIN_MEDIUM_ADC1(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
{
	unsigned int val, tmp;


	SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x1);

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), GAIN_MEDIUM_ADC1);
	tmp &= ~(0xff << Gain_Medium);
	val = ((Value & 0xff) << Gain_Medium);
	tmp |= val;
	WriteISLA_SPI(BoardNum, (1 <<adcNum), GAIN_MEDIUM_ADC1, tmp);

	SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x0);
}

EXPORTED_FUNCTION void SET_GAIN_FINE_ADC1(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
{
	unsigned int val, tmp;

	SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x1);

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), GAIN_FINE_ADC1);
	tmp &= ~(0xff << Gain_Fine);
	val = ((Value & 0xff) << Gain_Fine);
	tmp |= val;
	WriteISLA_SPI(BoardNum, (1 <<adcNum), GAIN_FINE_ADC1, tmp);

	SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x0);
}

void SET_MODES_ADC1(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
{
	unsigned int val, tmp;

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), MODES_ADC1);
	tmp &= ~(0x7 << Mode_BitPos);
	val = ((Value & 0x7) << Mode_BitPos);
	tmp |= val;
	WriteISLA_SPI(BoardNum, (1 <<adcNum), MODES_ADC1, tmp);
}

void SET_RESERVED31_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
{
	unsigned int val, tmp;

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), RESERVED31_REG);
	tmp &= ~(0xff << 0);
	val = ((Value & 0xff) << 0);
	tmp |= val;
	WriteISLA_SPI(BoardNum, adcNum, RESERVED31_REG, tmp);
}

void SET_RESERVED53_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
{
	unsigned int val, tmp;

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), RESERVED53_REG);
	tmp &= ~(0xff << 0);
	val = ((Value & 0xff) << 0);
	tmp |= val;
	WriteISLA_SPI(BoardNum, adcNum, RESERVED53_REG, tmp);
}

void SET_RESERVED55_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
{
	unsigned int val, tmp;

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), RESERVED55_REG);
	tmp &= ~(0xff << 0);
	val = ((Value & 0xff) << 0);
	tmp |= val;
	WriteISLA_SPI(BoardNum, adcNum, RESERVED55_REG, tmp);
}

void SET_PHASE_SLIP_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
{
	unsigned int val, tmp;

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), PHASE_SLIP_REG);
	tmp &= ~(0x1 << NextClockEdge);
	val = ((Value & 0x1) << NextClockEdge);
	tmp |= val;
	WriteISLA_SPI(BoardNum, (1 <<adcNum), PHASE_SLIP_REG, tmp);
}

void SET_CLOCK_DIVIDE_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
{
	unsigned int val, tmp;

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), CLOCK_DIVIDE_REG);
	tmp &= ~(0x7 << Mode_BitPos);
	val = ((Value & 0x7) << Mode_BitPos);
	tmp |= val;
	WriteISLA_SPI(BoardNum, (1 <<adcNum), CLOCK_DIVIDE_REG, tmp);
}

void SET_2X_CLOCK(unsigned short BoardNum)
{
	for (int i=0;i<MAX_ADC;i++)
	{
		long val = ReadISLA_REG(BoardNum, i, CLOCK_DIVIDE_REG);
		val &= 0xff;
		if (!(val == 0x02))
		{
			SET_CLOCK_DIVIDE_REG(BoardNum, i, DivideByTwo);
			Sleep(1);
		}
	}
}

void SET_1X_CLOCK(unsigned short BoardNum)
{
	for (int i=0;i<MAX_ADC;i++)
	{
		long val = ReadISLA_REG(BoardNum, i, CLOCK_DIVIDE_REG);
		val &= 0xff;
		if (!(val == 0x01))
		{
			SET_CLOCK_DIVIDE_REG(BoardNum, i, DivideByOne);
			Sleep(1);
		}
	}
}

EXPORTED_FUNCTION void SET_DLL_RANGE_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
{
	unsigned int val, tmp;

	//	tmp = ReadISLA_SPI(BoardNum, (adcNum), OUTPUT_MODE_B);
	tmp = (0x1 << 4);		//bit 4 must be preserved, all else 0, and preserved might be the wrong info so we are forcing it on
	val = ((Value & 0x1) << DLL_Range);
	tmp |= val;
	WriteISLA_SPI(BoardNum, (adcNum), OUTPUT_MODE_B, tmp);			//0x10 works
}

EXPORTED_FUNCTION void SET_TEST_PATTERN_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
{
	unsigned int val, tmp;

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), TEST_IO);
	tmp &= ~(0xf << TestMode_BitPos);
	val = ((Value & 0xf) << TestMode_BitPos);
	tmp |= val;
	WriteISLA_SPI(BoardNum, (1 <<adcNum), TEST_IO, tmp);
}

EXPORTED_FUNCTION void SET_OFFSET_GAIN_ENABLE_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), OFFSET_GAIN_ADJUST_ENABLE_REG);
	tmp &= ~(0x1 << GainAdjustEnable);
	val = ((Enable & 0x1) << GainAdjustEnable);
	tmp |= val;
	WriteISLA_SPI(BoardNum, (1 <<adcNum), OFFSET_GAIN_ADJUST_ENABLE_REG, tmp);
}

void SET_CLOCK_DIV_RST_POL(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
{
	unsigned long val;
	val = makeISLA_SPI(CLK_DIV_RST_POL,0,0, Value);
	SET_PIO_REG(BoardNum, PIO_OFFSET_ADCSERIAL, val);
	Sleep(1);
}

EXPORTED_FUNCTION void ADC_NRESET(unsigned short BoardNum, unsigned int adcNum)
{
	unsigned long val;
	val = makeISLA_SPI(N_RESET,adcNum,0,0);
	SET_PIO_REG(BoardNum, PIO_OFFSET_ADCSERIAL, val);
	Sleep(1);
}

void ADC_CLKDIVRST(unsigned short BoardNum, unsigned int adcNum)
{
	long val;

	//printf("ADC_CLKDIVRST: %d\n",adcNum);
	val = makeISLA_SPI(CLK_DIV_RST,adcNum,0,0);
	SET_PIO_REG(BoardNum, PIO_OFFSET_ADCSERIAL, val);
	Sleep(10);
}

void ADC_SETCALBITS(unsigned short BoardNum, unsigned int Value)
{
	unsigned long val;
	val = makeISLA_SPI(SET_CAL_BITS,0x2,0,Value);
	SET_PIO_REG(BoardNum, PIO_OFFSET_ADCSERIAL, val);
	Sleep(1);
}

void SET_ISLA_CAPTURE_DELAY(unsigned short BoardNum, unsigned int Value)
{
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_ISLA_CAPTURE, Value, 0, 0);
}

void readISLA_Gain_Offset(unsigned short BoardNum, unsigned char *regVal[])
{
	for (int i=0;i<MAX_ADC;i++)
	{
		for (int j=0;j<ISLA_NUMREG;j++)
		{
			regVal[i][j] = 0;
		}
	}
	for (int i=0;i<MAX_ADC;i++)
	{
		regVal[i][CHIP_ID] = ReadISLA_REG(BoardNum, i,CHIP_ID);
		if (regVal[i][CHIP_ID] == 0x49)
		{
			regVal[i][OFFSET_COARSE_ADC0] = ReadISLA_REG(BoardNum, i, OFFSET_COARSE_ADC0);
			regVal[i][OFFSET_FINE_ADC0] = ReadISLA_REG(BoardNum, i, OFFSET_FINE_ADC0);
			regVal[i][GAIN_COARSE_ADC0] = ReadISLA_REG(BoardNum, i, GAIN_COARSE_ADC0);
			regVal[i][GAIN_MEDIUM_ADC0] = ReadISLA_REG(BoardNum, i, GAIN_MEDIUM_ADC0);
			regVal[i][GAIN_FINE_ADC0] = ReadISLA_REG(BoardNum, i, GAIN_FINE_ADC0);
			regVal[i][OFFSET_COARSE_ADC1] = ReadISLA_REG(BoardNum, i, OFFSET_COARSE_ADC1);
			regVal[i][OFFSET_FINE_ADC1] = ReadISLA_REG(BoardNum, i, OFFSET_FINE_ADC1);
			regVal[i][GAIN_COARSE_ADC1] = ReadISLA_REG(BoardNum, i, GAIN_COARSE_ADC1);
			regVal[i][GAIN_MEDIUM_ADC1] = ReadISLA_REG(BoardNum, i, GAIN_MEDIUM_ADC1);
			regVal[i][GAIN_FINE_ADC1] = ReadISLA_REG(BoardNum, i, GAIN_FINE_ADC1);
		}
	}
}

void printISLA_Gain_Offset(unsigned char **regVal)
{
	for (int i=0;i<MAX_ADC;i++)
	{
		printf("%02x  ",regVal[i][CHIP_ID]);
		printf("%02x  ",regVal[i][OFFSET_COARSE_ADC0]);
		printf("%02x  ",regVal[i][OFFSET_FINE_ADC0]);
		printf("%02x  ",regVal[i][GAIN_COARSE_ADC0]);
		printf("%02x  ",regVal[i][GAIN_MEDIUM_ADC0]);
		printf("%02x  ",regVal[i][GAIN_FINE_ADC0]);
		printf("  --%02x  ",regVal[i][OFFSET_COARSE_ADC1]);
		printf("%02x  ",regVal[i][OFFSET_FINE_ADC1]);
		printf("%02x  ",regVal[i][GAIN_COARSE_ADC1]);
		printf("%02x  ",regVal[i][GAIN_MEDIUM_ADC1]);
		printf("%02x  ",regVal[i][GAIN_FINE_ADC1]);
		printf("\n");
	}
}

#define AUTO_PHASE_OPCODE 1
#define DCLK_OPCODE 2
#define DATA_OPCODE 3
void AdcIDelayManual(unsigned short BoardNum, unsigned int  adcNum, unsigned int dclk, unsigned int data)
{
	int iDelayReg;

	// send low to all bits
	iDelayReg = 0;
	SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);

	// set opcode for selected ADC
	if (adcNum < MAX_ADC)
	{
		// dclk shift
		iDelayReg = 0;
		iDelayReg |= (DCLK_OPCODE << (ADC_AUTO_IDELAY + 8*adcNum));
		iDelayReg |= ((dclk & 0x3f) << (8*adcNum));
		SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);
		iDelayReg = 0;
		SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);
		// command starts at this point so give it a millisec to complete
		SLEEP(1);
		// data shift
		iDelayReg = 0;
		iDelayReg |= (DATA_OPCODE << (ADC_AUTO_IDELAY + 8*adcNum));
		iDelayReg |= ((data & 0x3f) << (8*adcNum));
		SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);
		iDelayReg = 0;
		SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);
		// command starts at this point so give it a millisec to complete
		SLEEP(1);
	}
}

void AdcIDelayAuto(unsigned short BoardNum, unsigned int adcNum)
{
	unsigned val;

	switch (adcNum)
	{
		case 0:
			val = dev_config[BoardNum].ISLA_ADC0_IDELAY_OFFSET & 0x3f;
			AdcIDelayManual(BoardNum, adcNum,val,val);
			break;
		case 1:
			val = dev_config[BoardNum].ISLA_ADC1_IDELAY_OFFSET & 0x3f;
			AdcIDelayManual(BoardNum, adcNum,val,val);
			break;
		case 2:
			val = dev_config[BoardNum].ISLA_ADC2_IDELAY_OFFSET & 0x3f;
			AdcIDelayManual(BoardNum, adcNum,val,val);
			break;
		case 3:
			val = dev_config[BoardNum].ISLA_ADC3_IDELAY_OFFSET & 0x3f;
			AdcIDelayManual(BoardNum, adcNum,val,val);
			break;
		case 15:
			//printf("ADC idelay AUTO %d  %d  %d  %d\n",dev_config[BoardNum].ISLA_ADC0_IDELAY_OFFSET,dev_config[BoardNum].ISLA_ADC1_IDELAY_OFFSET,dev_config[BoardNum].ISLA_ADC2_IDELAY_OFFSET,dev_config[BoardNum].ISLA_ADC3_IDELAY_OFFSET);
			val = dev_config[BoardNum].ISLA_ADC0_IDELAY_OFFSET & 0x3f;
			AdcIDelayManual(BoardNum, 0,val,val);
			val = dev_config[BoardNum].ISLA_ADC1_IDELAY_OFFSET & 0x3f;
			AdcIDelayManual(BoardNum, 1,val,val);
			val = dev_config[BoardNum].ISLA_ADC2_IDELAY_OFFSET & 0x3f;
			AdcIDelayManual(BoardNum, 2,val,val);
			val = dev_config[BoardNum].ISLA_ADC3_IDELAY_OFFSET & 0x3f;
			AdcIDelayManual(BoardNum, 3,val,val);
			break;
		default:
			break;
	}
}

#define EDGE_OFFSET 15

int ISLA_AdcIdelayCalibrate(unsigned short BoardNum)
{
	bool calibrateDone = false;
	int i_delay_val = 0;
	unsigned long val;
	bool rising;
	bool falling;
	bool risingTrue = false;
	int hysterysis = 0;
	int idelayOffset[4];
	int idelayOffsetReadBack[4];
	int iDelayReg = 0;

	printf("ISLA_AdcIdelayCalibrate\n");

	idelayOffset[0] = dev_config[BoardNum].ISLA_ADC0_IDELAY_OFFSET & 0x3f;
	idelayOffset[1] = dev_config[BoardNum].ISLA_ADC1_IDELAY_OFFSET & 0x3f;
	idelayOffset[2] = dev_config[BoardNum].ISLA_ADC2_IDELAY_OFFSET & 0x3f;
	idelayOffset[3] = dev_config[BoardNum].ISLA_ADC3_IDELAY_OFFSET & 0x3f;

	//	SEND LOW TO ALL AUTO_IDELAY BITS
	SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);

	for (int adcNum=0;adcNum<4;adcNum++)
	{
		iDelayReg |= (1<< (ADC_AUTO_IDELAY + 8*adcNum));
		iDelayReg |= (idelayOffset[adcNum] << (8 * adcNum));
	}
	//printf("PIO_OFFSET_ISLA_CAPTURE  hi  %x\n",iDelayReg);
	SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);
	SLEEP(1);
	//	SEND LOW TO ALL AUTO_IDELAY BITS to finish the command
	for (int adcNum=0;adcNum<4;adcNum++)
	{
		iDelayReg &= ~(1<< (ADC_AUTO_IDELAY + 8*adcNum));
	}
	//printf("PIO_OFFSET_ISLA_CAPTURE lo  %x\n",iDelayReg);
	SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);
	SLEEP(1);

	val = GET_PIO_REG(BoardNum, PIO_OFFSET_ISLA_CAPTURE);
	//printf("PIO_OFFSET_ISLA_CAPTURE returned %x\n",val);
	for (int adcNum=0;adcNum<4;adcNum++)
	{
		//iDelayReg |= (1 << (ADC_AUTO_IDELAY + 8*adcNum));
		idelayOffsetReadBack[adcNum] = (val  >> (6 * adcNum + 8)) & 0x3f;
	}

	for (int adcNum=0;adcNum<4;adcNum++)
	{
		//printf("adc%d idelay cal = %d\n",adcNum,idelayOffsetReadBack[adcNum]);
	}

	Sleep(1);

	iDelayReg = 0;
	for (int adcNum=0;adcNum<4;adcNum++)
	{
		iDelayReg |= (2<< (ADC_AUTO_IDELAY + 8*adcNum));
		iDelayReg |= (idelayOffset[adcNum] << (8 * adcNum));
		//printf("iDelayReg %d = %d\n", adcNum, idelayOffset[adcNum]);
	}

	SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);

	for (int adcNum=0;adcNum<4;adcNum++)
	{
		iDelayReg &= ~(3<< (ADC_AUTO_IDELAY + 8*adcNum));
	}
	SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);
	Sleep(1);

	iDelayReg = 0;
	for (int adcNum=0;adcNum<4;adcNum++)
	{
		iDelayReg |= (3<< (ADC_AUTO_IDELAY + 8*adcNum));
		iDelayReg |= (idelayOffset[adcNum] << (8 * adcNum));
	}
	SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);

	for (int adcNum=0;adcNum<4;adcNum++)
	{
		iDelayReg &= ~(3<< (ADC_AUTO_IDELAY + 8*adcNum));
	}
	SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);

	return 0;
}

#define AD56X8_SETUP_INTERNAL_REFERENCE_REG 8
#define AD56X8_SET_POWERUP_REG  4

#define AD56X8_WRITE_AND_UPDATE_DAC 3
#define AD56X8_CMD_BIT_POS 24
#define AD56X8_ADDR_BIT_POS 20
#define AD56X8_16BIT_DAC_DATA_BIT_POS 4
#define OSC_ENABLE 31

void ISLA_AdcDacSerialSetup(unsigned short BoardNum)
{
	// Setup for AD5628, the ADC DAC
	// Range in Decimal of AD5628- 12 bits, 0-4095, 0x000-0xFFF
	// Using the 8-bit struct, to be compatible with other functions

	// 121009
	// Reference Setup. Bits 31:28 are don't care. Bits 27:24 are command.
	// Bits 23:20 are address (don' care). Bits 19:1 are don't care data. Bit0 is reference ON, use SEL_ADC_DAC_REF.

	unsigned char offsetCh[4] = {4,0,6,2};
	unsigned char biasCh[4] = {5,1,7,3};
	unsigned char gainCh[4];

	unsigned adc_bias[4];
	unsigned adc_offset[4];
	unsigned adc_gain[2];

	if (dev_config[BoardNum].is_AD5474)
	{
		offsetCh[0] = 6;
		offsetCh[1] = 4;
		biasCh[0] = 7;
		biasCh[1] = 5;
		gainCh[0] = 0;
		gainCh[1] = 2;
	}


	adc_bias[0] = dev_config[BoardNum].adc_bias0;
	adc_bias[1] = dev_config[BoardNum].adc_bias1;
	adc_bias[2] = dev_config[BoardNum].adc_bias2;
	adc_bias[3] = dev_config[BoardNum].adc_bias3;

	adc_offset[0] = dev_config[BoardNum].adc_offs0;
	adc_offset[1] = dev_config[BoardNum].adc_offs1;
	adc_offset[2] = dev_config[BoardNum].adc_offs2;
	adc_offset[3] = dev_config[BoardNum].adc_offs3;

	adc_gain[0] =  dev_config[BoardNum].adc_gain0;
	adc_gain[1] =  dev_config[BoardNum].adc_gain1;

	//	printf("----------------- %d  %d\n", dev_config[BoardNum].is_ISLA216P, dev_config[BoardNum].adc_chan);
	if ((dev_config[BoardNum].is_ISLA216P > 0) && (dev_config[BoardNum].adc_chan == 2))
	{
		adc_bias[1] = dev_config[BoardNum].adc_bias3;
		adc_offset[1] = dev_config[BoardNum].adc_offs3;
		offsetCh[1] = 2;
		biasCh[1] = 3;

	}
	//  Doubt this is really required
	AdcDacSerialWrite(BoardNum, AD56X8_SET_POWERUP_REG, 0, 0x0f);

	// ABSOLUTELY NECESSARY
	AdcDacSerialWrite(BoardNum, AD56X8_SETUP_INTERNAL_REFERENCE_REG, 0x00, 0x0000);


	if (dev_config[BoardNum].is_AD5474)
	{
		for (int i=0;i<dev_config[BoardNum].adc_chan;i++)
		{
			//printf("ADC%d  5474_AdcDacSerialSetup  bias = %d\t offset= %d\t gain= %d\n",i,adc_bias[i],adc_offset[i],adc_gain[i]);
			AdcDacSerialWrite(BoardNum, AD56X8_WRITE_AND_UPDATE_DAC, (offsetCh[i] & 0xf), adc_offset[i]);
			printf("adc_offset[%d] = %x\n", i, adc_offset[i]);
			AdcDacSerialWrite(BoardNum, AD56X8_WRITE_AND_UPDATE_DAC, (biasCh[i] & 0xf), adc_bias[i]);
			printf("adc_bias[%d] = %x\n", i, adc_bias[i]);
			AdcDacSerialWrite(BoardNum, AD56X8_WRITE_AND_UPDATE_DAC, (gainCh[i] & 0xf), adc_gain[i]);
			printf("adc_gain[%d] = %x\n", i, adc_gain[i]);
		}
	}
	else
	{
		for (int i=0;i<dev_config[BoardNum].adc_chan;i++)
		{
			//printf("ADC%d  ISLA_AdcDacSerialSetup  bias = %d\t offset= %d\n",i,adc_bias[i],adc_offset[i]);
			AdcDacSerialWrite(BoardNum, AD56X8_WRITE_AND_UPDATE_DAC, (offsetCh[i] & 0xf), adc_offset[i]);
			printf("adc_offset[%d] = %x\n", i, adc_offset[i]);
			AdcDacSerialWrite(BoardNum, AD56X8_WRITE_AND_UPDATE_DAC, (biasCh[i] & 0xf), adc_bias[i]);
			printf("adc_bias[%d] = %x\n", i, adc_bias[i]);
		}
	}
}

EXPORTED_FUNCTION void AdcDacSerialWrite(unsigned short BoardNum, unsigned int cmd, unsigned int addr, unsigned int adc_dac_data)
{
	unsigned long ad5628_spi_word = 0;

	ad5628_spi_word = (1 << OSC_ENABLE);
	ad5628_spi_word |= (cmd << AD56X8_CMD_BIT_POS);
	ad5628_spi_word |= (addr << AD56X8_ADDR_BIT_POS);
	ad5628_spi_word |= (adc_dac_data << AD56X8_16BIT_DAC_DATA_BIT_POS);
	ad5628_spi_word |= SEL_ADC_DAC_REF;
	SET_PIO_REG(BoardNum, PIO_OFFSET_ADC_DAC_SERIAL, ad5628_spi_word);

	SLEEP(1); // Ensure back to back serial operations do not conflict
}

void printISLA_registers(unsigned char **regVal)
{
	for (int i=0;i<MAX_ADC;i++)
	{
		printf("%02x  ",regVal[i][CHIP_ID]);
		printf("%02x  ",regVal[i][OFFSET_COARSE_ADC0]);
		printf("%02x  ",regVal[i][OFFSET_FINE_ADC0]);
		printf("%02x  ",regVal[i][GAIN_COARSE_ADC0]);
		printf("%02x  ",regVal[i][GAIN_MEDIUM_ADC0]);
		printf("%02x  ",regVal[i][GAIN_FINE_ADC0]);
		printf("  --%02x  ",regVal[i][OFFSET_COARSE_ADC1]);
		printf("%02x  ",regVal[i][OFFSET_FINE_ADC1]);
		printf("%02x  ",regVal[i][GAIN_COARSE_ADC1]);
		printf("%02x  ",regVal[i][GAIN_MEDIUM_ADC1]);
		printf("%02x  ",regVal[i][GAIN_FINE_ADC1]);
		printf("\n");
	}
}

EXPORTED_FUNCTION void SelectAdcChannels(unsigned short BoardNum, unsigned int Channels)
{
	//	  printf("Channels=%d\n",Channels);
	//    printf("SelectAdcChannels:1\n");
	//    AdcClockGetFreq(BoardNum);
	// Single channel mode, if Channels = 0,1
	if (!is_ISLA216P(BoardNum) && !is_adc12d2000(BoardNum) && !is_AD5474(BoardNum)){
		SetNumAdcChannel(BoardNum, Channels);
		AdcClockGetFreq(BoardNum);

	}

	if (is_ISLA216P(BoardNum))
	{
		if ((Channels == 1) || (Channels == 2) || (Channels == 4) || (Channels == 8))
		{
			dev_config[BoardNum].adc_chan_used = 1;
			// Sets adc_chan_used in ADCDATASETUP register
			SetNumAdcChannel(BoardNum, Channels);
			AdcClockGetFreq(BoardNum);

			// Sets CAPTUREADJ register to work in single channel mode
			SetDualADCSingleChannel(BoardNum, 1);
			AdcClockGetFreq(BoardNum);
		}
		else
		{
			if ((Channels == 7) || (Channels == 11) || (Channels == 13) || (Channels == 14))
			{
				printf("Data acquisition board cannot acquire with 3 channels. Defaulting board to acquire with 4 channels\n");
				dev_config[BoardNum].adc_chan_used = 4;
				Channels = 15;
			}
			else if (Channels == 15)
			{
				dev_config[BoardNum].adc_chan_used = 4;
				//			printf("4 channel mode working!\n");
			}
			else
			{
				dev_config[BoardNum].adc_chan_used = 2;
				//			printf("2 channel mode working!\n");
			}
			// Sets adc_chan_used in ADCDATASETUP register
			SetNumAdcChannel(BoardNum, Channels);
			AdcClockGetFreq(BoardNum);

			// Sets CAPTUREADJ register to work in multi channel mode
			SetDualADCSingleChannel(BoardNum, 0);
			AdcClockGetFreq(BoardNum);
		}
	}
	if (is_adc12d2000(BoardNum))
	{
		if (Channels == 1)
		{
			ADC12D2000_Channel_Mode_Select(BoardNum, ONE_CH_MODE, DESQ_MODE, 1);
			//			SetDualADCSingleChannel(BoardNum, 1);	//might need this
		}
		if (Channels == 2)
		{
			ADC12D2000_Channel_Mode_Select(BoardNum, ONE_CH_MODE, DESI_MODE, 1);
		}
		if (Channels == 4)
		{
			ADC12D2000_Channel_Mode_Select(BoardNum, ONE_CH_MODE, DESCLKIQ_MODE, 1);
		}
		if (Channels == 8)
		{
			ADC12D2000_Channel_Mode_Select(BoardNum, ONE_CH_MODE, DESIQ_MODE, 1);
		}
		if ((Channels == 3) || (Channels == 5) || (Channels == 6) || (Channels == 9) || (Channels == 10) || (Channels == 12))
		{
			ADC12D2000_Channel_Mode_Select(BoardNum, TWO_CH_MODE, 1, 1);
		}
		SetNumAdcChannel(BoardNum, Channels);
		AdcClockGetFreq(BoardNum);
	}
	if (is_AD5474(BoardNum)){
		// Single channel mode, if Channels = 0,1
		if ((Channels == IN0) || (Channels == IN1))
		{
			dev_config[BoardNum].adc_chan_used = 1;
			// Sets adc_chan_used in ADCDATASETUP register
			SetNumAdcChannel(BoardNum, Channels*5);

			// Sets CAPTUREADJ register to work in single channel mode
			SetDualADCSingleChannel(BoardNum, 1);
		}
		else
		{
			dev_config[BoardNum].adc_chan_used = 2;
			Channels = 15;

			// Sets adc_chan_used in ADCDATASETUP register
			SetNumAdcChannel(BoardNum, Channels);

			// Sets CAPTUREADJ register to work in multi channel mode
			SetDualADCSingleChannel(BoardNum, 0);
		}
		AdcClockGetFreq(BoardNum);

	}
}



long makeISLA_SPI(unsigned action,unsigned channels,unsigned spiAddress,unsigned spiData )
{
	long spiWord = -1;
	long tmp;

	switch (action)
	{
		case CLK_DIV_RST:
			spiWord  = (CLK_DIV_RST_COMMAND << COMMAND_BIT_POS);
			break;
		case CLK_DIV_RST_POL:
			spiWord  = (CLK_DIV_RST_POL_COMMAND << COMMAND_BIT_POS);
			break;
		case N_RESET:
			spiWord  = (RESET_N_COMMAND << COMMAND_BIT_POS);
			tmp = ((channels & 0xf) << ADC_BIT_POS);
			spiWord |= tmp;
			break;
		case SET_CAL_BITS:
			spiWord  = (SET_CAL_BITS_COMMAND << COMMAND_BIT_POS);
			tmp = ((channels & 0xf) << ADC_BIT_POS);
			spiWord |= tmp;
			break;
		case CLEAR_CAL_BITS:
			spiWord  = (CLEAR_CAL_BITS_COMMAND << COMMAND_BIT_POS);
			tmp = ((channels & 0xf) << ADC_BIT_POS);
			spiWord |= tmp;
			break;
		case SPI_READ:
			spiWord  = (SPI_COMMAND << COMMAND_BIT_POS);
			tmp = ((channels & 0xf) << ADC_BIT_POS);
			spiWord |= tmp;
			tmp = (1 << RW_BIT_POS);
			spiWord |= tmp;
			tmp = ((spiAddress & 0x1fff) << ADDR_BIT_POS);
			spiWord |= tmp;
			tmp = (0xff << DATA_BIT_POS);
			spiWord |= tmp;  // set the data bits to one to guard against fw collision with ADC data.  FW does this for us so not strictly needed
			break;
		case SPI_WRITE:
			spiWord  = (SPI_COMMAND << COMMAND_BIT_POS);
			tmp = ((channels & 0xf) << ADC_BIT_POS);
			spiWord |= tmp;
			tmp = (0 << RW_BIT_POS);
			spiWord |= tmp;
			tmp = ((spiAddress & 0x1fff) << ADDR_BIT_POS);
			spiWord |= tmp;
			tmp = ((spiData & 0xff) << DATA_BIT_POS);
			spiWord |= tmp;  // set the data bits to one to guard against fw collision with ADC data.  FW does this for us so not strictly needed
			break;
		default:
			break;
	}

	return spiWord;
}

void WriteISLA_SPI(unsigned short BoardNum, unsigned int adcNum, unsigned int spiAddress, unsigned int spiData)
{
	unsigned int tmp, val;
	unsigned int mask_info;

	mask_info = 0;
	tmp = ( adcNum << SPI_DEVICE_BIT_POS );
	mask_info |= tmp;
	tmp = ( WORD_0 << DATA_WORD_BIT_POS );
	mask_info |= tmp;
	tmp = ( AD16_DATA_LENGTH << DATA_LENGTH_BIT_POS);
	mask_info |= tmp;

	val = makeISLA_SPI(SPI_WRITE, adcNum, spiAddress, spiData);
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_ADCSERIAL, val, spiAddress,  mask_info);
	SLEEP(1);
}

unsigned int ReadISLA_SPI(unsigned short BoardNum, unsigned int adcNum, unsigned int spiAddress)
{
	unsigned int data0;
	unsigned int tmp, val;
	unsigned int mask_info;

	mask_info = 0;
	tmp = (adcNum << SPI_DEVICE_BIT_POS);
	mask_info |= tmp;
	tmp = (WORD_0 << DATA_WORD_BIT_POS);
	mask_info |= tmp;

	data0 = GET_SHADOW_REG(BoardNum, PIO_OFFSET_ADCSERIAL, spiAddress, mask_info);

	return data0;
}

unsigned char ReadISLA_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int spiAddress)
{
	long val = makeISLA_SPI(SPI_READ,(1<<adcNum),spiAddress,0);
	SET_PIO_REG(BoardNum, PIO_OFFSET_ADCSERIAL,val); // 24 us to shift data
	Sleep(1);  // 1 ms
	return  (unsigned char) (GET_PIO_REG(BoardNum, PIO_OFFSET_ADCSERIAL) & 0xff);
}

////////////////////////////////////////////////////////////////
// Adc Section - START
////////////////////////////////////////////////////////////////

//EXPORTED_FUNCTION unsigned int AdcClockGetFreq(unsigned short BoardNum)
//{
//    unsigned long readval;
//    double clock_f;
//    int strobe_count;
//    unsigned int val, clock_freq;

//    readval = GET_PIO_REG(BoardNum, PIO_OFFSET_PROMPROG);

//    // Ensure ADC DCM is locked
//    if( (readval & (1 << 9)) && (readval != 0xFFFFFFFF) )
//    {
//        /*
//        // Bring bit11 of CaptureAdjReg lo->hi to inititate a strobe count
//        pio_reg[BoardNum].CaptureAdjReg &= 0xFFFFF7FF;				//  (clear init strobe count)
//        SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
//        pio_reg[BoardNum].CaptureAdjReg |= 0x00000800;				//  (set strobe count)	3rd LS Nibble:	bit11->Init strobe count, bit9->single channel select
//        SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
//        pio_reg[BoardNum].CaptureAdjReg &= 0xFFFFF7FF;				//  (clear init strobe count)
//        SET_PIO_REG(PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg);
//        SLEEP(1);	// give time for strobe counter to finish
//        */
//        // Bring bit11 of CaptureAdjReg lo->hi to inititate a strobe count

//        val = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
//        val &= 0xFFFFF7FF;				//  (clear init strobe count)
//        SET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, val);
//        val |= 0x00000800;				//  (set strobe count)	3rd LS Nibble:	bit11->Init strobe count, bit9->single channel select
//        SET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, val);
//        val &= 0xFFFFF7FF;				//  (clear init strobe count)
//        SET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, val);
//        SLEEP(10);	// give time for strobe counter to finish

//        readval = GET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ);
//        strobe_count = readval & 0x0000FFFF;
//        //printf("strobe_count = %d\n", strobe_count);

//        //printf("strobe_count=%d\n", strobe_count);
//        clock_f = (double)((dev_config[BoardNum].strobe_count_mult)/1024.0)*(double)(10*strobe_count);	// Compatible with 8/14 bit boards

//        //printf("dev_config[BoardNum].is_adc12d2000 = %d\n",dev_config[BoardNum].is_adc12d2000);
//        //printf("dev_config[BoardNum].adc_chan_used = %d\n",dev_config[BoardNum].adc_chan_used);

//        //1 channel 16-bit
//        if ((dev_config[BoardNum].adc_res == 16) && (dev_config[BoardNum].adc_chan_used == 1))
//        {
//            clock_freq = (unsigned int) clock_f*4;
//        }
//        //2 channel 16-bit
//        else if ((dev_config[BoardNum].adc_res == 16) && (dev_config[BoardNum].adc_chan_used == 2))
//        {
//            clock_freq = (unsigned int) clock_f*2;
//        }
//        //4 channel 16-bit
//        else if ((dev_config[BoardNum].adc_res == 16) && (dev_config[BoardNum].adc_chan_used == 4))
//        {
//            clock_freq = (unsigned int) clock_f;
//        }
//        else
//        {
//            clock_freq = (unsigned int) clock_f;
//        }

//        //printf("Input clock frequency ~= %d Mhz \n", clock_freq);
//    }
//    else
//    {
//        printf("AdcClockGetFreq() failed. ADC DCM not locked!\n");
//        clock_freq = 0;
//    }

//    return clock_freq;
//}




//void CalibrateBoard(unsigned short BoardNum)
//{
//    unsigned long readval = 0;
//    static unsigned char **adcReg = NULL;

//    unsigned long val = 0l;

//    unsigned  cal_status[4] = {0,0,0,0};
//    unsigned int adc_clock_freq;

//    adcReg = (unsigned char **) malloc(MAX_ADC * sizeof *adcReg);
//    if (adcReg == NULL)
//    {
//        printf("unable to allocate memory for ADC registers\n");
//        return;
//    }
//    else
//    {
//        for (int i=0;i<MAX_ADC;i++)
//        {
//            adcReg[i] = (unsigned char *)malloc (ISLA_NUMREG * sizeof *adcReg[i]);
//            if (adcReg[i] == NULL)
//            {
//                i--;
//                for (;i>=0;i--)
//                    free(adcReg[i]);
//                free(adcReg);
//                return;
//            }
//        }
//    }

////	if (dev_config[BoardNum].ISLA_2X_CLK == 1)
////		SET_2X_CLOCK(BoardNum);
////	else
////		SET_1X_CLOCK(BoardNum);

//    for (int i=0;i<4;i++)
//        cal_status[i] = (((readval >> CAL_BIT_POS) >> i) & 1);

//    printf("cal_status = %d\n", cal_status[0]);

//    readISLA_Gain_Offset(BoardNum, adcReg);

//    //printISLA_Gain_Offset(adcReg);

//    if ((cal_status[0] == 0) || (cal_status[3] == 0))
//    {
//        printf("Calibrating ISLA ADCs\n");

//        SET_CLOCK_DIV_RST_POL(BoardNum, 0xF, 1);

//        adc_clock_freq = AdcClockGetFreq(BoardNum);

//        ADC_NRESET(BoardNum, 0xF);

//        int sleeptime = 550;

//        if(adc_clock_freq == 0)
//            printf("adc_clock_freq = 0\n");
//        else
//        {
//            if (dev_config[BoardNum].is_ISLA216P)
//                sleeptime = 550*250/adc_clock_freq;
//            else
//                sleeptime = 550*500/adc_clock_freq;
//        }
//        Sleep(sleeptime);// 6s  550ms required at 250MSPS  6s accounts for worst case cal time at 25MSPS


////		if (dev_config[BoardNum].ISLA_2X_CLK == 1)
////			SET_2X_CLOCK(BoardNum);
////		else
////			SET_1X_CLOCK(BoardNum);

//        ADC_CLKDIVRST(BoardNum, 0xF);

//        // Phase slip if external clock //////////////
//        unsigned int int_clk_on = GetInternalClockValue(BoardNum);

//        // Phase slip if using external clock on isla214
//        if ((int_clk_on == 0) && (dev_config[BoardNum].is_ISLA214P))
//            SET_PHASE_SLIP_REG(BoardNum, 0xf, 0x1);



//        int range = 0;
//        int lowClockTest = 90;
//        if (dev_config[BoardNum].is_ISLA214P)  //214P low frequency twice as high as 216p
//        {
//            lowClockTest *= 2;
//        }


//        printf("adc_clock_freq=%d\n",adc_clock_freq);
//        if (adc_clock_freq < lowClockTest)
//        {
//            range = 1;
//            printf("using slow clock\n");
//        }
//        if (range==0){
//            printf("using fast clock\n");
//        }

//        for (int i=0;i<MAX_ADC;i++)
//        {
//            SET_DLL_RANGE_REG(BoardNum, (1<<i), range);
//        }

////	/*
//        AdcIDelayAuto(BoardNum, 0xF);

////		ISLA_AdcIdelayCalibrate(BoardNum);

//        // set cal_status = 1
////		ADC_SETCALBITS(BoardNum, 1);

//        readISLA_Gain_Offset(BoardNum, adcReg);
//        //printISLA_registers(adcReg);
//        printf("done calibrating. adc gains and offsets auto set by nreset  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
//    }

//    ISLA_AdcDacSerialSetup(BoardNum);

//    ISLA_GainAdjust(BoardNum);

//        //printf("free memory array\n");
//    for (int i=0;i<MAX_ADC;i++)
//    {
//        //printf("freeing adcReg[%d]\n",i);
//        free(adcReg[i]);
//    }

//    //printf("free memory pointer\n");
//    free(adcReg);
//    printf("CalibrateBoard done\n\n");

//}

//void SET_ADC_OFFSETS(unsigned short BoardNum)			//added by DSchriebman 7/25/14 to correct offsets
//{
//    //if value read in from ultra_config.dat, propogate it to the board
//    //default to slight positive effect offset (0x60) if no calibration available in ultra_config.dat

//    if (dev_config[BoardNum].ISLA_ADC0_OFFSET_COARSE0 != 0x100){
//        SET_OFFSET_COARSE_ADC0(BoardNum,0,dev_config[BoardNum].ISLA_ADC0_OFFSET_COARSE0);}
//    else{
//        SET_OFFSET_COARSE_ADC0(BoardNum,0,0x60);}
//    if (dev_config[BoardNum].ISLA_ADC1_OFFSET_COARSE0 != 0x100){
//        SET_OFFSET_COARSE_ADC0(BoardNum,1,dev_config[BoardNum].ISLA_ADC1_OFFSET_COARSE0);}
//    else{
//        SET_OFFSET_COARSE_ADC0(BoardNum,1,0x60);}
//    if (dev_config[BoardNum].ISLA_ADC2_OFFSET_COARSE0 != 0x100){
//        SET_OFFSET_COARSE_ADC0(BoardNum,2,dev_config[BoardNum].ISLA_ADC2_OFFSET_COARSE0);}
//    else{
//        SET_OFFSET_COARSE_ADC0(BoardNum,2,0x60);}
//    if (dev_config[BoardNum].ISLA_ADC3_OFFSET_COARSE0 != 0x100){
//        SET_OFFSET_COARSE_ADC0(BoardNum,3,dev_config[BoardNum].ISLA_ADC3_OFFSET_COARSE0);}
//    else{
//        SET_OFFSET_COARSE_ADC0(BoardNum,3,0x60);}

//    if (dev_config[BoardNum].ISLA_ADC0_OFFSET_COARSE1 != 0x100){
//        SET_OFFSET_COARSE_ADC1(BoardNum,0,dev_config[BoardNum].ISLA_ADC0_OFFSET_COARSE1);}
//    else{
//        SET_OFFSET_COARSE_ADC1(BoardNum,0,0x60);}
//    if (dev_config[BoardNum].ISLA_ADC1_OFFSET_COARSE1 != 0x100){
//        SET_OFFSET_COARSE_ADC1(BoardNum,1,dev_config[BoardNum].ISLA_ADC1_OFFSET_COARSE1);}
//    else{
//        SET_OFFSET_COARSE_ADC1(BoardNum,1,0x60);}
//    if (dev_config[BoardNum].ISLA_ADC2_OFFSET_COARSE1 != 0x100){
//        SET_OFFSET_COARSE_ADC1(BoardNum,2,dev_config[BoardNum].ISLA_ADC2_OFFSET_COARSE1);}
//    else{
//        SET_OFFSET_COARSE_ADC1(BoardNum,2,0x60);}
//    if (dev_config[BoardNum].ISLA_ADC3_OFFSET_COARSE1 != 0x100){
//        SET_OFFSET_COARSE_ADC1(BoardNum,3,dev_config[BoardNum].ISLA_ADC3_OFFSET_COARSE1);}
//    else{
//        SET_OFFSET_COARSE_ADC1(BoardNum,3,0x60);}

//    //default to no fine offset (0x80) if no value found in ultra_config.dat
//    if (dev_config[BoardNum].ISLA_ADC0_OFFSET_FINE0 != 0x100){
//        SET_OFFSET_FINE_ADC0(BoardNum,0,dev_config[BoardNum].ISLA_ADC0_OFFSET_FINE0);}
//    else{
//        SET_OFFSET_FINE_ADC0(BoardNum,0,0x80);}
//    if (dev_config[BoardNum].ISLA_ADC1_OFFSET_FINE0 != 0x100){
//        SET_OFFSET_FINE_ADC0(BoardNum,1,dev_config[BoardNum].ISLA_ADC1_OFFSET_FINE0);}
//    else{
//        SET_OFFSET_FINE_ADC0(BoardNum,1,0x80);}
//    if (dev_config[BoardNum].ISLA_ADC2_OFFSET_FINE0 != 0x100){
//        SET_OFFSET_FINE_ADC0(BoardNum,2,dev_config[BoardNum].ISLA_ADC2_OFFSET_FINE0);}
//    else{
//        SET_OFFSET_FINE_ADC0(BoardNum,2,0x80);}
//    if (dev_config[BoardNum].ISLA_ADC3_OFFSET_FINE0 != 0x100){
//        SET_OFFSET_FINE_ADC0(BoardNum,3,dev_config[BoardNum].ISLA_ADC3_OFFSET_FINE0);}
//    else{
//        SET_OFFSET_FINE_ADC0(BoardNum,3,0x80);}

//    if (dev_config[BoardNum].ISLA_ADC0_OFFSET_FINE1 != 0x100){
//        SET_OFFSET_FINE_ADC1(BoardNum,0,dev_config[BoardNum].ISLA_ADC0_OFFSET_FINE1);}
//    else{
//        SET_OFFSET_FINE_ADC1(BoardNum,0,0x80);}
//    if (dev_config[BoardNum].ISLA_ADC1_OFFSET_FINE1 != 0x100){
//        SET_OFFSET_FINE_ADC1(BoardNum,1,dev_config[BoardNum].ISLA_ADC1_OFFSET_FINE1);}
//    else{
//        SET_OFFSET_FINE_ADC1(BoardNum,1,0x80);}
//    if (dev_config[BoardNum].ISLA_ADC2_OFFSET_FINE1 != 0x100){
//        SET_OFFSET_FINE_ADC1(BoardNum,2,dev_config[BoardNum].ISLA_ADC2_OFFSET_FINE1);}
//    else{
//        SET_OFFSET_FINE_ADC1(BoardNum,2,0x80);}
//    if (dev_config[BoardNum].ISLA_ADC3_OFFSET_FINE1 != 0x100){
//        SET_OFFSET_FINE_ADC1(BoardNum,3,dev_config[BoardNum].ISLA_ADC3_OFFSET_FINE1);}
//    else{
//        SET_OFFSET_FINE_ADC1(BoardNum,3,0x80);}


//    printf("calibration forced away from nreset values\n");

//}

//void ISLA_GainAdjust (unsigned short BoardNum)
//{

//    unsigned long val = 0l;

//    static unsigned char **regVal = NULL;

//    regVal = (unsigned char **) malloc(MAX_ADC * sizeof *regVal);

//    if (regVal == NULL)
//    {
//        printf("unable to allocate memory for ADC registers\n");
//        return;
//    }
//    else
//    {
//        for (int i=0;i<MAX_ADC;i++)
//        {
//            regVal[i] = (unsigned char *)malloc (ISLA_NUMREG * sizeof *regVal[i]);
//            if (regVal[i] == NULL)
//            {
//                i--;
//                for (;i>=0;i--)
//                    free(regVal[i]);
//                free(regVal);
//                return;
//            }
//        }
//    }

//    readISLA_Gain_Offset(BoardNum, regVal);
//    //printISLA_registers(regVal);
//    printf("regVal[0][CHIP_ID]=%x\n",regVal[0][CHIP_ID]);					//still not sure why these conditions don't get met, value seems right
//    if (0==0)//(regVal[0][CHIP_ID] == 0x49)  // ADC is present
//    {
//        // core 0
//        SET_GAIN_COARSE_ADC0(BoardNum, 0, dev_config[BoardNum].ISLA_ADC0_GAIN_COARSE0);
//        SET_GAIN_MEDIUM_ADC0(BoardNum, 0, dev_config[BoardNum].ISLA_ADC0_GAIN_MEDIUM0);
//        SET_GAIN_FINE_ADC0(BoardNum, 0, dev_config[BoardNum].ISLA_ADC0_GAIN_FINE0);

//        // core 1
//        SET_GAIN_COARSE_ADC1(BoardNum, 0, dev_config[BoardNum].ISLA_ADC0_GAIN_COARSE1);
//        SET_GAIN_MEDIUM_ADC1(BoardNum, 0, dev_config[BoardNum].ISLA_ADC0_GAIN_MEDIUM1);
//        SET_GAIN_FINE_ADC1(BoardNum, 0, dev_config[BoardNum].ISLA_ADC0_GAIN_FINE1);
//    }

//    if (0==0)//(regVal[1][CHIP_ID] == 0x49)  // ADC is present
//    {
//        // core 0
//        SET_GAIN_COARSE_ADC0(BoardNum, 1, dev_config[BoardNum].ISLA_ADC1_GAIN_COARSE0);
//        SET_GAIN_MEDIUM_ADC0(BoardNum, 1, dev_config[BoardNum].ISLA_ADC1_GAIN_MEDIUM0);
//        SET_GAIN_FINE_ADC0(BoardNum, 1, dev_config[BoardNum].ISLA_ADC1_GAIN_FINE0);

//        // core 1
//        SET_GAIN_COARSE_ADC1(BoardNum, 1, dev_config[BoardNum].ISLA_ADC1_GAIN_COARSE1);
//        SET_GAIN_MEDIUM_ADC1(BoardNum, 1, dev_config[BoardNum].ISLA_ADC1_GAIN_MEDIUM1);
//        SET_GAIN_FINE_ADC1(BoardNum, 1, dev_config[BoardNum].ISLA_ADC1_GAIN_FINE1);
//    }

//    if (0==0)//(regVal[2][CHIP_ID] == 0x49)  // ADC is present
//    {
//        // core 0
//        SET_GAIN_COARSE_ADC0(BoardNum, 2, dev_config[BoardNum].ISLA_ADC2_GAIN_COARSE0);
//        SET_GAIN_MEDIUM_ADC0(BoardNum, 2, dev_config[BoardNum].ISLA_ADC2_GAIN_MEDIUM0);
//        SET_GAIN_FINE_ADC0(BoardNum, 2, dev_config[BoardNum].ISLA_ADC2_GAIN_FINE0);

//        // core 1
//        SET_GAIN_COARSE_ADC1(BoardNum, 2, dev_config[BoardNum].ISLA_ADC2_GAIN_COARSE1);
//        SET_GAIN_MEDIUM_ADC1(BoardNum, 2, dev_config[BoardNum].ISLA_ADC2_GAIN_MEDIUM1);
//        SET_GAIN_FINE_ADC1(BoardNum, 2, dev_config[BoardNum].ISLA_ADC2_GAIN_FINE1);
//    }

//    if (0==0)//(regVal[3][CHIP_ID] == 0x49)  // ADC is present
//    {
//        // core 0
//        SET_GAIN_COARSE_ADC0(BoardNum, 3, dev_config[BoardNum].ISLA_ADC3_GAIN_COARSE0);
//        SET_GAIN_MEDIUM_ADC0(BoardNum, 3, dev_config[BoardNum].ISLA_ADC3_GAIN_MEDIUM0);
//        SET_GAIN_FINE_ADC0(BoardNum, 3, dev_config[BoardNum].ISLA_ADC3_GAIN_FINE0);

//        // core 1
//        SET_GAIN_COARSE_ADC1(BoardNum, 3, dev_config[BoardNum].ISLA_ADC3_GAIN_COARSE1);
//        SET_GAIN_MEDIUM_ADC1(BoardNum, 3, dev_config[BoardNum].ISLA_ADC3_GAIN_MEDIUM1);
//        SET_GAIN_FINE_ADC1(BoardNum, 3, dev_config[BoardNum].ISLA_ADC3_GAIN_FINE1);
//    }

//    int tmp;

// //   for (int adc_ch=0;adc_ch<4;adc_ch++)
// //   {
// //       //printf ("adc%d chip id= %x\n",adc_ch,regVal[adc_ch][CHIP_ID] );
// //       if (0==0)//(regVal[adc_ch][CHIP_ID] == 0x49)  // ADC is present
//    //	{
//    //		tmp = 0x01;
//    //		//printf ("Setting gain adjust enable reg\n");
//    //		//val = makeISLA_SPI(SPI_WRITE,(1<<adc_ch),OFFSET_GAIN_ADJUST_ENABLE_REG,tmp);
//    //		//SET_PIO_REG(PIO_OFFSET_ADCSERIAL,val);
//    //		//Sleep(1);
//    //		SET_OFFSET_GAIN_ENABLE_REG(BoardNum, adc_ch, tmp);

//    //		tmp = 0x00;
//    //		//printf ("Clearing gain adjust enable reg\n");
//    //		//val = makeISLA_SPI(SPI_WRITE,(1<<adc_ch),OFFSET_GAIN_ADJUST_ENABLE_REG,tmp);
//    //		//SET_PIO_REG(PIO_OFFSET_ADCSERIAL,val);
//    //		//Sleep(1);
//    //		SET_OFFSET_GAIN_ENABLE_REG(BoardNum, adc_ch, tmp);
//    //	}
//    //}

//    //printf("ISLA_GainAdjust complete\n");

//    readISLA_Gain_Offset(BoardNum, regVal);
//    //printISLA_registers(regVal);

//    //printf("free memory array\n");
//    for (int i=0;i<MAX_ADC;i++)
//    {
//        //printf("freeing adcReg[%d]\n",i);
//        free(regVal[i]);

//    }
//    //printf("free memory pointer\n");
//    free(regVal);
//}

//void CONFIGURE_CHIP_PORT_CONFIG(unsigned short BoardNum, unsigned int adcNum)
//{
//    SetISLA_SDOActive(BoardNum, adcNum, 0);
//    SetISLA_LSBFirst(BoardNum, adcNum, 0);
//    SetISLA_SoftReset(BoardNum, adcNum, 0);
//    SetISLA_ReservedBit4(BoardNum, adcNum);
//}

//void SetISLA_SDOActive(unsigned short BoardNum, unsigned int adcNum, unsigned int Enable)
//{
//    unsigned int val, tmp;

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), CHIP_PORT_CONFIG);
//    tmp &= ~(0x1 << SDO_Active);
//    tmp &= ~(0x1 << Mirror_SDO_Active);
//    val = ((Enable & 0x1) << SDO_Active);
//    tmp |= val;
//    val = ((Enable & 0x1) << Mirror_SDO_Active);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, (1 <<adcNum), CHIP_PORT_CONFIG, tmp);
//}

//void SetISLA_LSBFirst(unsigned short BoardNum, unsigned int adcNum, unsigned int Enable)
//{
//    unsigned int val, tmp;

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), CHIP_PORT_CONFIG);
//    tmp &= ~(0x1 << LSB_First);
//    tmp &= ~(0x1 << Mirror_LSB_First);
//    val = ((Enable & 0x1) << LSB_First);
//    tmp |= val;
//    val = ((Enable & 0x1) << Mirror_LSB_First);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, (1 <<adcNum), CHIP_PORT_CONFIG, tmp);
//}

//void SetISLA_SoftReset(unsigned short BoardNum, unsigned int adcNum, unsigned int Enable)
//{
//    unsigned int val, tmp;

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), CHIP_PORT_CONFIG);
//    tmp &= ~(0x1 << Soft_Reset);
//    tmp &= ~(0x1 << Mirror_Soft_Reset);
//    val = ((Enable & 0x1) << Soft_Reset);
//    tmp |= val;
//    val = ((Enable & 0x1) << Mirror_Soft_Reset);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, (1 <<adcNum), CHIP_PORT_CONFIG, tmp);
//}

//void SetISLA_ReservedBit4(unsigned short BoardNum, unsigned int adcNum)
//{
//    unsigned int val, tmp;

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), CHIP_PORT_CONFIG);
//    tmp &= ~(0x1 << ReservedBit4);
//    tmp &= ~(0x1 << Mirror_ReservedBit4);
//    val = ((1 & 0x1) << ReservedBit4);
//    tmp |= val;
//    val = ((1 & 0x1) << Mirror_ReservedBit4);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, (1 <<adcNum), CHIP_PORT_CONFIG, tmp);
//}

//EXPORTED_FUNCTION void SET_OFFSET_COARSE_ADC0(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
//{
//    unsigned int val, tmp;

//    SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x1);

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), OFFSET_COARSE_ADC0);
//    tmp &= ~(0xff << Offset_Coarse);
//    val = ((Value & 0xff) << Offset_Coarse);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, (1 <<adcNum), OFFSET_COARSE_ADC0, tmp);
//    SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x0);
//}

//EXPORTED_FUNCTION void SET_OFFSET_FINE_ADC0(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
//{
//    unsigned int val, tmp;

//    SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x1);

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), OFFSET_FINE_ADC0);
//    tmp &= ~(0xff << Offset_Fine);
//    val = ((Value & 0xff) << Offset_Fine);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, (1 <<adcNum), OFFSET_FINE_ADC0, tmp);

//    SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x0);
//}

//EXPORTED_FUNCTION void SET_GAIN_COARSE_ADC0(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
//{
//    unsigned int val, tmp;

//    SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x1);

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), GAIN_COARSE_ADC0);
//    tmp &= ~(0xf << Gain_Coarse);
//    val = ((Value & 0xf) << Gain_Coarse);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, (1 <<adcNum), GAIN_COARSE_ADC0, tmp);

//    SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x0);
//}

//EXPORTED_FUNCTION void SET_GAIN_MEDIUM_ADC0(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
//{
//    unsigned int val, tmp;

//    SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x1);

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), GAIN_MEDIUM_ADC0);
//    tmp &= ~(0xff << Gain_Medium);
//    val = ((Value & 0xff) << Gain_Medium);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, (1 <<adcNum), GAIN_MEDIUM_ADC0, tmp);

//    SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x0);
//}

//EXPORTED_FUNCTION void SET_GAIN_FINE_ADC0(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
//{
//    unsigned int val, tmp;

//    SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x1);

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), GAIN_FINE_ADC0);
//    tmp &= ~(0xff << Gain_Fine);
//    val = ((Value & 0xff) << Gain_Fine);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, (1 <<adcNum), GAIN_FINE_ADC0, tmp);

//    SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x0);
//}

//void SET_MODES_ADC0(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
//{
//    unsigned int val, tmp;

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), MODES_ADC0);
//    tmp &= ~(0x7 << Mode_BitPos);
//    val = ((Value & 0x7) << Mode_BitPos);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, adcNum, MODES_ADC0, tmp);

//    SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (1 <<adcNum), 0x0);
//}

//EXPORTED_FUNCTION void SET_OFFSET_COARSE_ADC1(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
//{
//    unsigned int val, tmp;

//    SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x1);

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), OFFSET_COARSE_ADC1);
//    tmp &= ~(0xff << Offset_Coarse);
//    val = ((Value & 0xff) << Offset_Coarse);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, (1 <<adcNum), OFFSET_COARSE_ADC1, tmp);

//    SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x0);
//}

//EXPORTED_FUNCTION void SET_OFFSET_FINE_ADC1(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
//{
//    unsigned int val, tmp;

//    SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x1);

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), OFFSET_FINE_ADC1);
//    tmp &= ~(0xff << Offset_Fine);
//    val = ((Value & 0xff) << Offset_Fine);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, (1 <<adcNum), OFFSET_FINE_ADC1, tmp);

//    SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x0);
//}

//EXPORTED_FUNCTION void SET_GAIN_COARSE_ADC1(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
//{
//    unsigned int val, tmp;


//    SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x1);

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), GAIN_COARSE_ADC1);
//    tmp &= ~(0xf << Gain_Coarse);
//    val = ((Value & 0xf) << Gain_Coarse);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, (1 <<adcNum), GAIN_COARSE_ADC1, tmp);

//    SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x0);
//}

//EXPORTED_FUNCTION void SET_GAIN_MEDIUM_ADC1(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
//{
//    unsigned int val, tmp;


//    SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x1);

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), GAIN_MEDIUM_ADC1);
//    tmp &= ~(0xff << Gain_Medium);
//    val = ((Value & 0xff) << Gain_Medium);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, (1 <<adcNum), GAIN_MEDIUM_ADC1, tmp);

//    SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x0);
//}

//EXPORTED_FUNCTION void SET_GAIN_FINE_ADC1(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
//{
//    unsigned int val, tmp;

//    SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x1);

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), GAIN_FINE_ADC1);
//    tmp &= ~(0xff << Gain_Fine);
//    val = ((Value & 0xff) << Gain_Fine);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, (1 <<adcNum), GAIN_FINE_ADC1, tmp);

//    SET_OFFSET_GAIN_ENABLE_REG(BoardNum, (adcNum), 0x0);
//}

//void SET_MODES_ADC1(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
//{
//    unsigned int val, tmp;

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), MODES_ADC1);
//    tmp &= ~(0x7 << Mode_BitPos);
//    val = ((Value & 0x7) << Mode_BitPos);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, (1 <<adcNum), MODES_ADC1, tmp);
//}

//void SET_RESERVED31_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
//{
//    unsigned int val, tmp;

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), RESERVED31_REG);
//    tmp &= ~(0xff << 0);
//    val = ((Value & 0xff) << 0);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, adcNum, RESERVED31_REG, tmp);
//}

//void SET_RESERVED53_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
//{
//    unsigned int val, tmp;

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), RESERVED53_REG);
//    tmp &= ~(0xff << 0);
//    val = ((Value & 0xff) << 0);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, adcNum, RESERVED53_REG, tmp);
//}

//void SET_RESERVED55_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
//{
//    unsigned int val, tmp;

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), RESERVED55_REG);
//    tmp &= ~(0xff << 0);
//    val = ((Value & 0xff) << 0);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, adcNum, RESERVED55_REG, tmp);
//}

//void SET_PHASE_SLIP_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
//{
//    unsigned int val, tmp;

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), PHASE_SLIP_REG);
//    tmp &= ~(0x1 << NextClockEdge);
//    val = ((Value & 0x1) << NextClockEdge);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, (1 <<adcNum), PHASE_SLIP_REG, tmp);
//}

//void SET_CLOCK_DIVIDE_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
//{
//    unsigned int val, tmp;

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), CLOCK_DIVIDE_REG);
//    tmp &= ~(0x7 << Mode_BitPos);
//    val = ((Value & 0x7) << Mode_BitPos);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, (1 <<adcNum), CLOCK_DIVIDE_REG, tmp);
//}

//void SET_2X_CLOCK(unsigned short BoardNum)
//{
//    for (int i=0;i<MAX_ADC;i++)
//    {
//        long val = ReadISLA_REG(BoardNum, i, CLOCK_DIVIDE_REG);
//        val &= 0xff;
//        if (!(val == 0x02))
//        {
//            SET_CLOCK_DIVIDE_REG(BoardNum, i, DivideByTwo);
//            Sleep(1);
//        }
//    }
//}

//void SET_1X_CLOCK(unsigned short BoardNum)
//{
//    for (int i=0;i<MAX_ADC;i++)
//    {
//        long val = ReadISLA_REG(BoardNum, i, CLOCK_DIVIDE_REG);
//        val &= 0xff;
//        if (!(val == 0x01))
//        {
//            SET_CLOCK_DIVIDE_REG(BoardNum, i, DivideByOne);
//            Sleep(1);
//        }
//    }
//}

//EXPORTED_FUNCTION void SET_DLL_RANGE_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
//{
//    unsigned int val, tmp;

////	tmp = ReadISLA_SPI(BoardNum, (adcNum), OUTPUT_MODE_B);
//    tmp = (0x1 << 4);		//bit 4 must be preserved, all else 0, and preserved might be the wrong info so we are forcing it on
//    val = ((Value & 0x1) << DLL_Range);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, (adcNum), OUTPUT_MODE_B, tmp);			//0x10 works
//}

//EXPORTED_FUNCTION void SET_TEST_PATTERN_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
//{
//    unsigned int val, tmp;

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), TEST_IO);
//    tmp &= ~(0xf << TestMode_BitPos);
//    val = ((Value & 0xf) << TestMode_BitPos);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, (1 <<adcNum), TEST_IO, tmp);
//}

//EXPORTED_FUNCTION void SET_OFFSET_GAIN_ENABLE_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Enable)
//{
//    unsigned int val, tmp;

//    tmp = ReadISLA_SPI(BoardNum, (1 <<adcNum), OFFSET_GAIN_ADJUST_ENABLE_REG);
//    tmp &= ~(0x1 << GainAdjustEnable);
//    val = ((Enable & 0x1) << GainAdjustEnable);
//    tmp |= val;
//    WriteISLA_SPI(BoardNum, (1 <<adcNum), OFFSET_GAIN_ADJUST_ENABLE_REG, tmp);
//}

//void SET_CLOCK_DIV_RST_POL(unsigned short BoardNum, unsigned int adcNum, unsigned int Value)
//{
//    unsigned long val;
//    val = makeISLA_SPI(CLK_DIV_RST_POL,0,0, Value);
//    SET_PIO_REG(BoardNum, PIO_OFFSET_ADCSERIAL, val);
//    Sleep(1);
//}
//EXPORTED_FUNCTION void ADC_NRESET(unsigned short BoardNum, unsigned int adcNum)
//{
//    unsigned long val;
//    val = makeISLA_SPI(N_RESET,adcNum,0,0);
//    SET_PIO_REG(BoardNum, PIO_OFFSET_ADCSERIAL, val);
//    Sleep(1);
//}

//void ADC_CLKDIVRST(unsigned short BoardNum, unsigned int adcNum)
//{
//    long val;

//    //printf("ADC_CLKDIVRST: %d\n",adcNum);
//    val = makeISLA_SPI(CLK_DIV_RST,adcNum,0,0);
//    SET_PIO_REG(BoardNum, PIO_OFFSET_ADCSERIAL, val);
//    Sleep(10);
//}

//void ADC_SETCALBITS(unsigned short BoardNum, unsigned int Value)
//{
//    unsigned long val;
//    val = makeISLA_SPI(SET_CAL_BITS,0x2,0,Value);
//    SET_PIO_REG(BoardNum, PIO_OFFSET_ADCSERIAL, val);
//    Sleep(1);
//}

//void SET_ISLA_CAPTURE_DELAY(unsigned short BoardNum, unsigned int Value)
//{
//    SET_SHADOW_REG(BoardNum, PIO_OFFSET_ISLA_CAPTURE, Value, 0, 0);
//}

//void readISLA_Gain_Offset(unsigned short BoardNum, unsigned char *regVal[])
//{
//    for (int i=0;i<MAX_ADC;i++)
//    {
//        for (int j=0;j<ISLA_NUMREG;j++)
//        {
//            regVal[i][j] = 0;
//        }
//    }
//    for (int i=0;i<MAX_ADC;i++)
//    {
//        regVal[i][CHIP_ID] = ReadISLA_REG(BoardNum, i,CHIP_ID);
//        if (regVal[i][CHIP_ID] == 0x49)
//        {
//            regVal[i][OFFSET_COARSE_ADC0] = ReadISLA_REG(BoardNum, i, OFFSET_COARSE_ADC0);
//            regVal[i][OFFSET_FINE_ADC0] = ReadISLA_REG(BoardNum, i, OFFSET_FINE_ADC0);
//            regVal[i][GAIN_COARSE_ADC0] = ReadISLA_REG(BoardNum, i, GAIN_COARSE_ADC0);
//            regVal[i][GAIN_MEDIUM_ADC0] = ReadISLA_REG(BoardNum, i, GAIN_MEDIUM_ADC0);
//            regVal[i][GAIN_FINE_ADC0] = ReadISLA_REG(BoardNum, i, GAIN_FINE_ADC0);
//            regVal[i][OFFSET_COARSE_ADC1] = ReadISLA_REG(BoardNum, i, OFFSET_COARSE_ADC1);
//            regVal[i][OFFSET_FINE_ADC1] = ReadISLA_REG(BoardNum, i, OFFSET_FINE_ADC1);
//            regVal[i][GAIN_COARSE_ADC1] = ReadISLA_REG(BoardNum, i, GAIN_COARSE_ADC1);
//            regVal[i][GAIN_MEDIUM_ADC1] = ReadISLA_REG(BoardNum, i, GAIN_MEDIUM_ADC1);
//            regVal[i][GAIN_FINE_ADC1] = ReadISLA_REG(BoardNum, i, GAIN_FINE_ADC1);
//        }
//    }
//}

//void printISLA_Gain_Offset(unsigned char **regVal)
//{
//    for (int i=0;i<MAX_ADC;i++)
//    {
//        printf("%02x  ",regVal[i][CHIP_ID]);
//        printf("%02x  ",regVal[i][OFFSET_COARSE_ADC0]);
//        printf("%02x  ",regVal[i][OFFSET_FINE_ADC0]);
//        printf("%02x  ",regVal[i][GAIN_COARSE_ADC0]);
//        printf("%02x  ",regVal[i][GAIN_MEDIUM_ADC0]);
//        printf("%02x  ",regVal[i][GAIN_FINE_ADC0]);
//        printf("  --%02x  ",regVal[i][OFFSET_COARSE_ADC1]);
//        printf("%02x  ",regVal[i][OFFSET_FINE_ADC1]);
//        printf("%02x  ",regVal[i][GAIN_COARSE_ADC1]);
//        printf("%02x  ",regVal[i][GAIN_MEDIUM_ADC1]);
//        printf("%02x  ",regVal[i][GAIN_FINE_ADC1]);
//        printf("\n");
//    }
//}

//#define AUTO_PHASE_OPCODE 1
//#define DCLK_OPCODE 2
//#define DATA_OPCODE 3
//void AdcIDelayManual(unsigned short BoardNum, unsigned int  adcNum, unsigned int dclk, unsigned int data)
//{
//    int iDelayReg;

//    // send low to all bits
//    iDelayReg = 0;
//    SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);

//    // set opcode for selected ADC
//    if (adcNum < MAX_ADC)
//    {
//        // dclk shift
//        iDelayReg = 0;
//        iDelayReg |= (DCLK_OPCODE << (ADC_AUTO_IDELAY + 8*adcNum));
//        iDelayReg |= ((dclk & 0x3f) << (8*adcNum));
//        SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);
//        iDelayReg = 0;
//        SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);
//        // command starts at this point so give it a millisec to complete
//        SLEEP(1);
//        // data shift
//        iDelayReg = 0;
//        iDelayReg |= (DATA_OPCODE << (ADC_AUTO_IDELAY + 8*adcNum));
//        iDelayReg |= ((data & 0x3f) << (8*adcNum));
//        SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);
//        iDelayReg = 0;
//        SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);
//        // command starts at this point so give it a millisec to complete
//        SLEEP(1);
//    }
//}

//void AdcIDelayAuto(unsigned short BoardNum, unsigned int adcNum)
//{
//    unsigned val;

//    switch (adcNum)
//    {
//    case 0:
//        val = dev_config[BoardNum].ISLA_ADC0_IDELAY_OFFSET & 0x3f;
//        AdcIDelayManual(BoardNum, adcNum,val,val);
//        break;
//    case 1:
//        val = dev_config[BoardNum].ISLA_ADC1_IDELAY_OFFSET & 0x3f;
//        AdcIDelayManual(BoardNum, adcNum,val,val);
//        break;
//    case 2:
//        val = dev_config[BoardNum].ISLA_ADC2_IDELAY_OFFSET & 0x3f;
//        AdcIDelayManual(BoardNum, adcNum,val,val);
//        break;
//    case 3:
//        val = dev_config[BoardNum].ISLA_ADC3_IDELAY_OFFSET & 0x3f;
//        AdcIDelayManual(BoardNum, adcNum,val,val);
//        break;
//    case 15:
//        //printf("ADC idelay AUTO %d  %d  %d  %d\n",dev_config[BoardNum].ISLA_ADC0_IDELAY_OFFSET,dev_config[BoardNum].ISLA_ADC1_IDELAY_OFFSET,dev_config[BoardNum].ISLA_ADC2_IDELAY_OFFSET,dev_config[BoardNum].ISLA_ADC3_IDELAY_OFFSET);
//        val = dev_config[BoardNum].ISLA_ADC0_IDELAY_OFFSET & 0x3f;
//        AdcIDelayManual(BoardNum, 0,val,val);
//        val = dev_config[BoardNum].ISLA_ADC1_IDELAY_OFFSET & 0x3f;
//        AdcIDelayManual(BoardNum, 1,val,val);
//        val = dev_config[BoardNum].ISLA_ADC2_IDELAY_OFFSET & 0x3f;
//        AdcIDelayManual(BoardNum, 2,val,val);
//        val = dev_config[BoardNum].ISLA_ADC3_IDELAY_OFFSET & 0x3f;
//        AdcIDelayManual(BoardNum, 3,val,val);
//        break;
//    default:
//        break;
//    }
//}

//#define EDGE_OFFSET 15

//int ISLA_AdcIdelayCalibrate(unsigned short BoardNum)
//{
//    bool calibrateDone = false;
//    int i_delay_val = 0;
//    unsigned long val;
//    bool rising;
//    bool falling;
//    bool risingTrue = false;
//    int hysterysis = 0;
//    int idelayOffset[4];
//    int idelayOffsetReadBack[4];
//    int iDelayReg = 0;

//    printf("ISLA_AdcIdelayCalibrate\n");

//    idelayOffset[0] = dev_config[BoardNum].ISLA_ADC0_IDELAY_OFFSET & 0x3f;
//    idelayOffset[1] = dev_config[BoardNum].ISLA_ADC1_IDELAY_OFFSET & 0x3f;
//    idelayOffset[2] = dev_config[BoardNum].ISLA_ADC2_IDELAY_OFFSET & 0x3f;
//    idelayOffset[3] = dev_config[BoardNum].ISLA_ADC3_IDELAY_OFFSET & 0x3f;

//    //	SEND LOW TO ALL AUTO_IDELAY BITS
//    SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);

//    for (int adcNum=0;adcNum<4;adcNum++)
//    {
//        iDelayReg |= (1<< (ADC_AUTO_IDELAY + 8*adcNum));
//        iDelayReg |= (idelayOffset[adcNum] << (8 * adcNum));
//    }
//    //printf("PIO_OFFSET_ISLA_CAPTURE  hi  %x\n",iDelayReg);
//    SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);
//    SLEEP(1);
//    //	SEND LOW TO ALL AUTO_IDELAY BITS to finish the command
//    for (int adcNum=0;adcNum<4;adcNum++)
//    {
//        iDelayReg &= ~(1<< (ADC_AUTO_IDELAY + 8*adcNum));
//    }
//    //printf("PIO_OFFSET_ISLA_CAPTURE lo  %x\n",iDelayReg);
//    SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);
//    SLEEP(1);

//    val = GET_PIO_REG(BoardNum, PIO_OFFSET_ISLA_CAPTURE);
//    //printf("PIO_OFFSET_ISLA_CAPTURE returned %x\n",val);
//    for (int adcNum=0;adcNum<4;adcNum++)
//    {
//        //iDelayReg |= (1 << (ADC_AUTO_IDELAY + 8*adcNum));
//        idelayOffsetReadBack[adcNum] = (val  >> (6 * adcNum + 8)) & 0x3f;
//    }

//    for (int adcNum=0;adcNum<4;adcNum++)
//    {
//        //printf("adc%d idelay cal = %d\n",adcNum,idelayOffsetReadBack[adcNum]);
//    }

//    Sleep(1);

//    iDelayReg = 0;
//    for (int adcNum=0;adcNum<4;adcNum++)
//    {
//        iDelayReg |= (2<< (ADC_AUTO_IDELAY + 8*adcNum));
//        iDelayReg |= (idelayOffset[adcNum] << (8 * adcNum));
//        //printf("iDelayReg %d = %d\n", adcNum, idelayOffset[adcNum]);
//    }

//    SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);

//    for (int adcNum=0;adcNum<4;adcNum++)
//    {
//        iDelayReg &= ~(3<< (ADC_AUTO_IDELAY + 8*adcNum));
//    }
//    SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);
//    Sleep(1);

//    iDelayReg = 0;
//    for (int adcNum=0;adcNum<4;adcNum++)
//    {
//        iDelayReg |= (3<< (ADC_AUTO_IDELAY + 8*adcNum));
//        iDelayReg |= (idelayOffset[adcNum] << (8 * adcNum));
//    }
//    SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);

//    for (int adcNum=0;adcNum<4;adcNum++)
//    {
//        iDelayReg &= ~(3<< (ADC_AUTO_IDELAY + 8*adcNum));
//    }
//    SET_ISLA_CAPTURE_DELAY(BoardNum, iDelayReg);

//    return 0;
//}

//#define AD56X8_SETUP_INTERNAL_REFERENCE_REG 8
//#define AD56X8_SET_POWERUP_REG  4

//#define AD56X8_WRITE_AND_UPDATE_DAC 3
//#define AD56X8_CMD_BIT_POS 24
//#define AD56X8_ADDR_BIT_POS 20
//#define AD56X8_16BIT_DAC_DATA_BIT_POS 4
//#define OSC_ENABLE 31

//void ISLA_AdcDacSerialSetup(unsigned short BoardNum)
//{
//    // Setup for AD5628, the ADC DAC
//    // Range in Decimal of AD5628- 12 bits, 0-4095, 0x000-0xFFF
//    // Using the 8-bit struct, to be compatible with other functions

//    // 121009
//    // Reference Setup. Bits 31:28 are don't care. Bits 27:24 are command.
//    // Bits 23:20 are address (don' care). Bits 19:1 are don't care data. Bit0 is reference ON, use SEL_ADC_DAC_REF.

//    unsigned char offsetCh[4] = {4,0,6,2};
//    unsigned char biasCh[4] = {5,1,7,3};
//    unsigned char gainCh[4];

//    unsigned adc_bias[4];
//    unsigned adc_offset[4];
//    unsigned adc_gain[2];

//    if (dev_config[BoardNum].is_AD5474)
//    {
//        offsetCh[0] = 6;
//        offsetCh[1] = 4;
//        biasCh[0] = 7;
//        biasCh[1] = 5;
//        gainCh[0] = 0;
//        gainCh[1] = 2;
//    }

//    adc_bias[0] = dev_config[BoardNum].adc_bias0;
//    adc_bias[1] = dev_config[BoardNum].adc_bias1;
//    adc_bias[2] = dev_config[BoardNum].adc_bias2;
//    adc_bias[3] = dev_config[BoardNum].adc_bias3;

//    adc_offset[0] = dev_config[BoardNum].adc_offs0;
//    adc_offset[1] = dev_config[BoardNum].adc_offs1;
//    adc_offset[2] = dev_config[BoardNum].adc_offs2;
//    adc_offset[3] = dev_config[BoardNum].adc_offs3;

//    adc_gain[0] =  dev_config[BoardNum].adc_gain0;
//    adc_gain[1] =  dev_config[BoardNum].adc_gain1;

////  Doubt this is really required
//    AdcDacSerialWrite(BoardNum, AD56X8_SET_POWERUP_REG, 0, 0x0f);

//// ABSOLUTELY NECESSARY
//    AdcDacSerialWrite(BoardNum, AD56X8_SETUP_INTERNAL_REFERENCE_REG, 0x00, 0x0000);


//    if (dev_config[BoardNum].is_AD5474)
//    {
//        for (int i=0;i<dev_config[BoardNum].adc_chan;i++)
//        {
//            //printf("ADC%d  5474_AdcDacSerialSetup  bias = %d\t offset= %d\t gain= %d\n",i,adc_bias[i],adc_offset[i],adc_gain[i]);
//            AdcDacSerialWrite(BoardNum, AD56X8_WRITE_AND_UPDATE_DAC, (offsetCh[i] & 0xf), adc_offset[i]);
//            printf("adc_offset[%d] = %x\n", i, adc_offset[i]);
//            AdcDacSerialWrite(BoardNum, AD56X8_WRITE_AND_UPDATE_DAC, (biasCh[i] & 0xf), adc_bias[i]);
//            printf("adc_bias[%d] = %x\n", i, adc_bias[i]);
//            AdcDacSerialWrite(BoardNum, AD56X8_WRITE_AND_UPDATE_DAC, (gainCh[i] & 0xf), adc_gain[i]);
//            printf("adc_gain[%d] = %x\n", i, adc_gain[i]);
//        }
//    }
//    else
//    {
//        for (int i=0;i<dev_config[BoardNum].adc_chan;i++)
//        {
//            //printf("ADC%d  ISLA_AdcDacSerialSetup  bias = %d\t offset= %d\n",i,adc_bias[i],adc_offset[i]);
//            AdcDacSerialWrite(BoardNum, AD56X8_WRITE_AND_UPDATE_DAC, (offsetCh[i] & 0xf), adc_offset[i]);
//            printf("adc_offset[%d] = %x\n", i, adc_offset[i]);
//            AdcDacSerialWrite(BoardNum, AD56X8_WRITE_AND_UPDATE_DAC, (biasCh[i] & 0xf), adc_bias[i]);
//            printf("adc_bias[%d] = %x\n", i, adc_bias[i]);
//        }
//    }
//}

//EXPORTED_FUNCTION void AdcDacSerialWrite(unsigned short BoardNum, unsigned int cmd, unsigned int addr, unsigned int adc_dac_data)
//{
//    unsigned long ad5628_spi_word = 0;

//    ad5628_spi_word = (1 << OSC_ENABLE);
//    ad5628_spi_word |= (cmd << AD56X8_CMD_BIT_POS);
//    ad5628_spi_word |= (addr << AD56X8_ADDR_BIT_POS);
//    ad5628_spi_word |= (adc_dac_data << AD56X8_16BIT_DAC_DATA_BIT_POS);
//    ad5628_spi_word |= SEL_ADC_DAC_REF;
//    SET_PIO_REG(BoardNum, PIO_OFFSET_ADC_DAC_SERIAL, ad5628_spi_word);

//    SLEEP(1); // Ensure back to back serial operations do not conflict
//}

//void printISLA_registers(unsigned char **regVal)
//{
//    for (int i=0;i<MAX_ADC;i++)
//    {
//        printf("%02x  ",regVal[i][CHIP_ID]);
//        printf("%02x  ",regVal[i][OFFSET_COARSE_ADC0]);
//        printf("%02x  ",regVal[i][OFFSET_FINE_ADC0]);
//        printf("%02x  ",regVal[i][GAIN_COARSE_ADC0]);
//        printf("%02x  ",regVal[i][GAIN_MEDIUM_ADC0]);
//        printf("%02x  ",regVal[i][GAIN_FINE_ADC0]);
//        printf("  --%02x  ",regVal[i][OFFSET_COARSE_ADC1]);
//        printf("%02x  ",regVal[i][OFFSET_FINE_ADC1]);
//        printf("%02x  ",regVal[i][GAIN_COARSE_ADC1]);
//        printf("%02x  ",regVal[i][GAIN_MEDIUM_ADC1]);
//        printf("%02x  ",regVal[i][GAIN_FINE_ADC1]);
//        printf("\n");
//    }
//}

//EXPORTED_FUNCTION void SelectAdcChannels(unsigned short BoardNum, unsigned int Channels)
//{
//    // Single channel mode, if Channels = 0,1
//    if ((Channels == 1) || (Channels == 2) || (Channels == 4) || (Channels == 8))
//    {
//        dev_config[BoardNum].adc_chan_used = 1;
//        // Sets adc_chan_used in ADCDATASETUP register
//        SetNumAdcChannel(BoardNum, Channels);

//        // Sets CAPTUREADJ register to work in single channel mode
//        SetDualADCSingleChannel(BoardNum, 1);
//    }
//    else
//    {
//        if ((Channels == 7) || (Channels == 11) || (Channels == 13) || (Channels == 14))
//        {
//            printf("Data acquisition board cannot acquire with 3 channels. Defaulting board to acquire with 4 channels\n");
//            dev_config[BoardNum].adc_chan_used = 4;
//            Channels = 0xf;
//        }
//        else if (Channels == 0xf)
//        {
//            dev_config[BoardNum].adc_chan_used = 4;
//        }
//        else
//        {
//            dev_config[BoardNum].adc_chan_used = 2;
//        }
//        // Sets adc_chan_used in ADCDATASETUP register
//        SetNumAdcChannel(BoardNum, Channels);

//        // Sets CAPTUREADJ register to work in multi channel mode
//        SetDualADCSingleChannel(BoardNum, 0);
//    }
//}
//////////////////////////////////////////////////////////////////
//// Adc Section - END
//////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////
// Adc Section - END
////////////////////////////////////////////////////////////////


// DRAM timing adjustment IDELAY shift  (122810)
EXPORTED_FUNCTION void DramIdelayShift(unsigned short BoardNum, int dqs_idelay_val, int dq_idelay_val)
{
	int idelay_cnt;


	// Reset/Unreset the DQ and the DQS IDELAYs
	//pio_reg[BoardNum].DRAMAdjReg |= (1 << DRAM_IDELAY_DQ_RST) | (1<<DRAM_IDELAY_DQS_RST);
	//SET_PIO_REG(PIO_OFFSET_DRAM_ADJUST, pio_reg[BoardNum].DRAMAdjReg);
	//SLEEP(1);

	SetDRAMIDelay_DQ_CE_INC(BoardNum, 0);
	Sleep(1);
	SetDRAMIDelay_DQS_CE_INC(BoardNum, 0);
	Sleep(1);
	SetDRAMIDelay_DQS_DQ_RST(BoardNum, 0);
	Sleep(1);


	SetDRAMIDelay_DQS_DQ_RST(BoardNum, 1);
	Sleep(1);
	//pio_reg[BoardNum].DRAMAdjReg &= ~(1<<DRAM_IDELAY_DQ_RST);
	//pio_reg[BoardNum].DRAMAdjReg &= ~(1<<DRAM_IDELAY_DQS_RST);	
	//SET_PIO_REG(PIO_OFFSET_DRAM_ADJUST, pio_reg[BoardNum].DRAMAdjReg);
	//SLEEP(1);

	SetDRAMIDelay_DQS_DQ_RST(BoardNum, 0);
	Sleep(1);


	for(idelay_cnt = 0; idelay_cnt< dq_idelay_val; idelay_cnt++)
	{
		// Clear the idelay clock enable (firmware looks for rising edge)
		SetDRAMIDelay_DQ_CE_INC(BoardNum, 0);
		SLEEP(1);

		// Initiate the idelay increment/decrement
		SetDRAMIDelay_DQ_CE_INC(BoardNum, 1);
		SLEEP(1);
	}


	for(idelay_cnt = 0; idelay_cnt<dqs_idelay_val; idelay_cnt++)
	{
		// Clear the idelay clock enable (firmware looks for rising edge)
		SetDRAMIDelay_DQS_CE_INC(BoardNum, 0);
		SLEEP(1);

		// Initiate the idelay increment/decrement
		SetDRAMIDelay_DQS_CE_INC(BoardNum, 1);
		SLEEP(1);
	}
}

// Access type to memory buffer depends on converter resolution and number of channels. This function
// returns the sample value for the given channel at the given index into the memory buffer passed into the function.
EXPORTED_FUNCTION SAMPLE_TYPE GetSample(unsigned short BoardNum, void * pBuffer, unsigned int index, unsigned short channel)
{

	SAMPLE_TYPE return_val = 0;


	switch (dev_config[BoardNum].adc_res)
	{
		case 8:

			// Truth Table for 8-bit boards - NSS 102610
			// single channel mode is along the top, ADC08D1520 is along the side
			// Values inside the table are the number of data streams for that situation
			//
			//		   single channel mode
			//
			//		   |  0  |  1  |
			//	    	---------------------
			// ADC081520 	 0 |  2  |  1  |
			//	    	---------------------
			//	      	 1 |  4  |  2  |
			//          	---------------------	

			// Single ADC083000
			if( dev_config[BoardNum].adc_chan_used == 1 )	
			{
				unsigned char * pSample = (unsigned char *) pBuffer;    // Typecast void * buffer to appropriate type
				return_val = (pSample[index] >> (8*channel)) & 0xFF;    // Mask and shift to get the sample for the requested channel at the requested index
			}            
			// Dual ADC083000 or single ADC08D1520	101110
			else if(dev_config[BoardNum].adc_chan_used == 2 )	
			{
				unsigned short * pSample = (unsigned short *) pBuffer;  // Typecast void * buffer to appropriate type
				return_val = (pSample[index] >> (8*channel)) & 0xFF;    // Mask and shift to get the sample for the requested channel at the requested index
			}
			// Dual ADC08D1520
			else if( dev_config[BoardNum].adc_chan_used == 4 )	
			{
				unsigned int * pSample = (unsigned int *) pBuffer;      // Typecast void * buffer to appropriate type
				return_val = (pSample[index] >> (8*channel)) & 0xFF;    // Mask and shift to get the sample for the requested channel at the requested index
			}

			break;


		case 12:

			// This data is MSB-aligned with 14-bit data.
			// To handle this, we mask out the top two bits (same way as 14-bit data), and shift out the bottom two bits.
			if( dev_config[BoardNum].adc_chan_used == 1 )	
			{
				unsigned short * pSample = (unsigned short *) pBuffer;  // Typecast void * buffer to appropriate type
				return_val = (pSample[index] & 0x00003FFF);        // Mask and shift to get the sample for the requested channel at the requested index
			}            
			else if(dev_config[BoardNum].adc_chan_used == 2 )	
			{
				unsigned int * pSample = (unsigned int *) pBuffer;      // Typecast void * buffer to appropriate type 

				if (channel == 0)
				{
					return_val = (pSample[index] & 0x3FFF0000) >> 16;   // Mask and shift to get the sample for the requested channel at the requested index
				}
				else
				{
					return_val = (pSample[index] & 0x00003FFF);    // Mask and shift to get the sample for the requested channel at the requested index
				}

			}

			break;


		case 14:

			if( dev_config[BoardNum].adc_chan_used == 1 )	
			{
				unsigned short * pSample = (unsigned short *) pBuffer;  // Typecast void * buffer to appropriate type
				return_val = (pSample[index] & 0x00003FFF);             // Mask and shift to get the sample for the requested channel at the requested index
			}            
			else if(dev_config[BoardNum].adc_chan_used == 2 )	
			{
				unsigned int * pSample = (unsigned int *) pBuffer;      // Typecast void * buffer to appropriate type 

				if (channel == 0)
				{
					return_val = (pSample[index] & 0x3FFF0000) >> 16;   // Mask and shift to get the sample for the requested channel at the requested index
				}
				else
				{
					return_val = (pSample[index] & 0x00003FFF);         // Mask and shift to get the sample for the requested channel at the requested index
				}

			}

			break;


		case 16:

			if( dev_config[BoardNum].adc_chan_used == 1 )	
			{
				unsigned short * pSample = (unsigned short *) pBuffer;  // Typecast void * buffer to appropriate type
				return_val = (pSample[index] & 0x0000FFFF);             // Mask and shift to get the sample for the requested channel at the requested index
			}            
			else if(dev_config[BoardNum].adc_chan_used == 2 )	
			{
				unsigned int * pSample = (unsigned int *) pBuffer;      // Typecast void * buffer to appropriate type 

				if (channel == 0)
				{
					return_val = (pSample[index] & 0xFFFF0000) >> 16;   // Mask and shift to get the sample for the requested channel at the requested index
				}
				else
				{
					return_val = (pSample[index] & 0x0000FFFF);         // Mask and shift to get the sample for the requested channel at the requested index
				}

			}
			else if(dev_config[BoardNum].adc_chan_used == 4 )	
			{
				unsigned long * pSample = (unsigned long *) pBuffer;      // Typecast void * buffer to appropriate type 

				if (channel == 0)
				{
					return_val = (pSample[index] & 0xFFFF000000000000) >> 48;         // Mask and shift to get the sample for the requested channel at the requested index
				}
				else if (channel == 1)
				{
					return_val = (pSample[index] & 0x0000FFFF00000000) >> 32;         // Mask and shift to get the sample for the requested channel at the requested index
				}
				else if (channel == 2)
				{
					return_val = (pSample[index] & 0x00000000FFFF0000) >> 16;         // Mask and shift to get the sample for the requested channel at the requested index
				}
				else if (channel == 3)
				{
					return_val = (pSample[index] & 0x000000000000FFFF);   // Mask and shift to get the sample for the requested channel at the requested index
				}

			}

			break;

	}


	return return_val;
}

// Read the serial number and board configuration information from the configuration file
bool GetBoardConfigInfo (unsigned short BoardNum)
{

	unsigned int serial_number=0xFFFFFFFF;
	char serial_alpha[25];
	unsigned int config_done = 0;
	unsigned int config_index;
	unsigned int config_index_end;
	unsigned int next_board_found = 0;
	unsigned short config_info_found = 0;		// was the config info for this board found in the config file
	unsigned short data_found = 0;
	bool return_sucess = true;

	unsigned long i;
	unsigned long bytesRead, configFileSize;
	char *config_array;

	config_array = (char *) malloc(1024*1024);  // the configuration data, 102810 increased to 1MB, 110410 used malloc

	// INITIALIZE WITH DEFAULT VALUES
	dev_config[BoardNum].pre_trigger_len = 512;
	dev_config[BoardNum].serial_number = 0xFFFFFFFF;
	dev_config[BoardNum].continuous_arf = true;

	// Configuration data required by every board:
	dev_config[BoardNum].adc_res = 8;
	dev_config[BoardNum].adc_chan = 1;
	dev_config[BoardNum].dac_res = 14;
	dev_config[BoardNum].dac_chan = 0;
	dev_config[BoardNum].fpga_type = 50;
	dev_config[BoardNum].adc_chan_used = 2;
	//printf("channels used set to 2 by getconfiginfo\n");

	dev_config[BoardNum].dram_dqs = 0;
	dev_config[BoardNum].dram_dq = 8;

	// Configuration data required by 8-bit boards
	dev_config[BoardNum].is_adc08d1520 = 0;
	dev_config[BoardNum].clk500_phs = 0x00;
	dev_config[BoardNum].clk1000_phs = 0x00;
	dev_config[BoardNum].shift_adc2 = 0;

	// Full Scale Range (Gain Range): 0x000->(560mVpp)(MIN VALUE)    1FF->(840mVpp)(MAX VALUE)    0x100->(700mVpp)(DEFAULT VALUE)
	dev_config[BoardNum].adc2_fsr = 0x100;
	dev_config[BoardNum].adc2_fsr_Q = 0x100;     // 101210 RHS ADC Q-Channel FSR

	dev_config[BoardNum].adc3_fsr = 0x100;
	dev_config[BoardNum].adc3_fsr_Q = 0x100;     // 101210 LHS ADC Q-Channel FSR

	dev_config[BoardNum].adc2_off_neg = 0;   
	dev_config[BoardNum].adc2_off_neg_Q = 0;     // 101210 RHS ADC Q-Channel Offset sign

	dev_config[BoardNum].adc3_off_neg = 0;       
	dev_config[BoardNum].adc3_off_neg_Q = 0;     // 101210 LHS ADC Q-Channel Offset sign

	dev_config[BoardNum].adc2_off = 0x0000;
	dev_config[BoardNum].adc2_off_Q = 0x0000;        // 101210 RHS ADC Q-Channel Offset

	dev_config[BoardNum].adc3_off = 0x0000;	
	dev_config[BoardNum].adc3_off_Q = 0x0000;        // 101210 LHS ADC Q-Channel Offset

	dev_config[BoardNum].adc_bias0 = 0x0000;
	dev_config[BoardNum].adc_bias1 = 0x0000;
	dev_config[BoardNum].adc_bias2 = 0x0000;
	dev_config[BoardNum].adc_bias3 = 0x0000;

	dev_config[BoardNum].adc_offs0 = 0x0000;
	dev_config[BoardNum].adc_offs1 = 0x0000;
	dev_config[BoardNum].adc_offs2 = 0x0000;
	dev_config[BoardNum].adc_offs3 = 0x0000;

	//Configuration data required by ADC12D2000
	dev_config[BoardNum].is_adc12d2000 = false;

	dev_config[BoardNum].ADC12D2000_desi_q_offset = 0x0000;
	dev_config[BoardNum].ADC12D2000_desi_q_offset_neg = 0x0;
	dev_config[BoardNum].ADC12D2000_desq_i_offset = 0x0000;
	dev_config[BoardNum].ADC12D2000_desq_i_offset_neg = 0x0;

	dev_config[BoardNum].ADC12D2000_desiq_q_offset = 0x0000;
	dev_config[BoardNum].ADC12D2000_desiq_q_offset_neg = 0x0;
	dev_config[BoardNum].ADC12D2000_desiq_i_offset = 0x0000;
	dev_config[BoardNum].ADC12D2000_desiq_i_offset_neg = 0x0;

	dev_config[BoardNum].ADC12D2000_desclkiq_q_offset = 0x0000;
	dev_config[BoardNum].ADC12D2000_desclkiq_q_offset_neg = 0x0;
	dev_config[BoardNum].ADC12D2000_desclkiq_i_offset = 0x0000;
	dev_config[BoardNum].ADC12D2000_desclkiq_i_offset_neg = 0x0;

	dev_config[BoardNum].ADC12D2000_desclkiq_q_fsr = 0x100;
	dev_config[BoardNum].ADC12D2000_desclkiq_i_fsr = 0x100;

	dev_config[BoardNum].ADC12D2000_CALIBRATION_VALUES = 0x0000;
	dev_config[BoardNum].ADC12D2000_BIAS_ADJUST = 0x1C0E;
	dev_config[BoardNum].ADC12D2000_DES_TIMING_ADJUST = 0x40;
	dev_config[BoardNum].ADC12D2000_APERTURE_DELAY_COARSE = 0x0000;
	dev_config[BoardNum].ADC12D2000_APERTURE_DELAY_FINE = 0x0000;

	dev_config[BoardNum].MICROSYNTH = 0;
	dev_config[BoardNum].MICROSYNTH_OSC_FREQ = 50000000;	//default oscilator


	//Configuration data required by AD14-400s with upgraded trigger capabilities
	dev_config[BoardNum].is_AD5474 = false;

	//Configuration data required by ISLA214P
	dev_config[BoardNum].is_ISLA214P = false;

	//Configuration data required by ISLA216P
	dev_config[BoardNum].is_ISLA216P = false;

	dev_config[BoardNum].ISLA_ADC0_GAIN_COARSE0 = 0x00;
	dev_config[BoardNum].ISLA_ADC0_GAIN_MEDIUM0 = 0x00;
	dev_config[BoardNum].ISLA_ADC0_GAIN_FINE0 = 0x00;
	dev_config[BoardNum].ISLA_ADC0_OFFSET_FINE0 = 0x100;
	dev_config[BoardNum].ISLA_ADC0_OFFSET_COARSE0 = 0x100;

	dev_config[BoardNum].ISLA_ADC1_GAIN_COARSE0 = 0x00;
	dev_config[BoardNum].ISLA_ADC1_GAIN_MEDIUM0 = 0x000;
	dev_config[BoardNum].ISLA_ADC1_GAIN_FINE0 = 0x00;
	dev_config[BoardNum].ISLA_ADC1_OFFSET_FINE0 = 0x100;
	dev_config[BoardNum].ISLA_ADC1_OFFSET_COARSE0 = 0x100;

	dev_config[BoardNum].ISLA_ADC2_GAIN_COARSE0 = 0x00;
	dev_config[BoardNum].ISLA_ADC2_GAIN_MEDIUM0 = 0x00;
	dev_config[BoardNum].ISLA_ADC2_GAIN_FINE0 = 0x00;
	dev_config[BoardNum].ISLA_ADC2_OFFSET_FINE0 = 0x100;
	dev_config[BoardNum].ISLA_ADC2_OFFSET_COARSE0 = 0x100;

	dev_config[BoardNum].ISLA_ADC3_GAIN_COARSE0 = 0x00;
	dev_config[BoardNum].ISLA_ADC3_GAIN_MEDIUM0 = 0x00;
	dev_config[BoardNum].ISLA_ADC3_GAIN_FINE0 = 0x00;
	dev_config[BoardNum].ISLA_ADC3_OFFSET_FINE0 = 0x100;
	dev_config[BoardNum].ISLA_ADC3_OFFSET_COARSE0 = 0x100;


	dev_config[BoardNum].ISLA_ADC0_GAIN_COARSE1 = 0x00;
	dev_config[BoardNum].ISLA_ADC0_GAIN_MEDIUM1 = 0x00;
	dev_config[BoardNum].ISLA_ADC0_GAIN_FINE1 = 0x00;
	dev_config[BoardNum].ISLA_ADC0_OFFSET_FINE1 = 0x100;
	dev_config[BoardNum].ISLA_ADC0_OFFSET_COARSE1 = 0x100;

	dev_config[BoardNum].ISLA_ADC1_GAIN_COARSE1 = 0x00;
	dev_config[BoardNum].ISLA_ADC1_GAIN_MEDIUM1 = 0x00;
	dev_config[BoardNum].ISLA_ADC1_GAIN_FINE1 = 0x00;
	dev_config[BoardNum].ISLA_ADC1_OFFSET_FINE1 = 0x100;
	dev_config[BoardNum].ISLA_ADC1_OFFSET_COARSE1 = 0x100;

	dev_config[BoardNum].ISLA_ADC2_GAIN_COARSE1 = 0x00;
	dev_config[BoardNum].ISLA_ADC2_GAIN_MEDIUM1 = 0x00;
	dev_config[BoardNum].ISLA_ADC2_GAIN_FINE1 = 0x00;
	dev_config[BoardNum].ISLA_ADC2_OFFSET_FINE1 = 0x100;
	dev_config[BoardNum].ISLA_ADC2_OFFSET_COARSE1 = 0x100;

	dev_config[BoardNum].ISLA_ADC3_GAIN_COARSE1 = 0x00;
	dev_config[BoardNum].ISLA_ADC3_GAIN_MEDIUM1 = 0x00;
	dev_config[BoardNum].ISLA_ADC3_GAIN_FINE1 = 0x00;
	dev_config[BoardNum].ISLA_ADC3_OFFSET_FINE1 = 0x100;
	dev_config[BoardNum].ISLA_ADC3_OFFSET_COARSE1 = 0x100;

	dev_config[BoardNum].ISLA_ADC0_IDELAY_OFFSET = 0x00;
	dev_config[BoardNum].ISLA_ADC1_IDELAY_OFFSET = 0x00;
	dev_config[BoardNum].ISLA_ADC2_IDELAY_OFFSET = 0x00;
	dev_config[BoardNum].ISLA_ADC3_IDELAY_OFFSET = 0x00;

	dev_config[BoardNum].ISLA_2X_CLK = 0;

	dev_config[BoardNum].ecl_trigger_delay = 4000;


	// get device serial number, user get_usercode.svf, reprogram=0 (must be zero to allow TDO mismatch)
	//serial_number = load_svf(&pAPI, "get_usercode.svf", 0);
	//    serial_number = LoadSvf(BoardNum, "get_usercode.svf", 0);
	serial_number = pAPI->ApiGetSerialNumber(BoardNum);

	#ifdef _WINDOWS
	_itoa_s(serial_number, serial_alpha, 10);
	#else
	sprintf(serial_alpha, "%d", serial_number);
	#endif

	if(serial_number == -1)
	{
		printf("Device Serial Number Unspecified\n");
	}
	else
	{
		//	  printf("Device Serial Number = %d, 0x%x\n", serial_number, serial_number);
	}

	// 062810


	#ifdef _WINDOWS

	// Try to open from current directory
	HANDLE configFile = CreateFile("ultra_config.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if(configFile == INVALID_HANDLE_VALUE)
	{ 
		printf("ultra_config.dat configuration file not found!\n"); 
		dev_config[BoardNum].config_file_found = 0;       
	}
	else
	{
		printf("ultra_config.dat opened\n"); 
		dev_config[BoardNum].config_file_found = 1;
	}

	LARGE_INTEGER lpFileSize;

	if( GetFileSizeEx(configFile, &lpFileSize) )
	{
		configFileSize = lpFileSize.LowPart;         
		ReadFile(configFile, config_array, configFileSize, &bytesRead, NULL);
		CloseHandle(configFile);
	}
	else
	{
		configFileSize = 0;
		printf("Error reading from ultra_config.dat!!\n"); 
	}

	#else
	FILE * configFile = fopen("ultra_config.dat", "rb");
	if((configFile == NULL) || (configFile == 0)) 
	{ 
		printf("ultra_config.dat configuration file not found.\n"); 
		dev_config[BoardNum].config_file_found = 0;
	}
	else
	{
		dev_config[BoardNum].config_file_found = 1;
		// get the file size
		fseek( (FILE *) configFile, 0, SEEK_END);
		configFileSize = ftell( (FILE *) configFile);
		rewind( (FILE *) configFile);
		bytesRead = fread(config_array, 1, configFileSize, (FILE *) configFile);
	}

	#endif


	// setup the configuration data
	if(dev_config[BoardNum].config_file_found)
	{

		config_info_found = 0;
		char serial_name[30] = "--";
		strcat(serial_name, serial_alpha);  // Look for --SERIAL_NUMBER in the config file, where SERIAL_NUMBER is the actual serial number in decimal

		for(i=0; i<configFileSize; i++)
		{
			//printf("config_array %c \n",config_array[i]);
			if( (strncmp(serial_name, config_array+i, 6)) == 0 )
			{
				config_index=i;     // record the position in the file where the this board's config data begins
				i=configFileSize+1;
				config_info_found=1; 	
				//printf("SERIAL FOUND \n");
			}
		}

		for (i=(config_index+6); i<configFileSize; i++)
		{
			//printf("config_array[%d] = %c \n", i, config_array[i]);
			if( ((strncmp("--", config_array+i,2)) == 0) && (next_board_found == 0))
			{
				config_index_end=i;
				next_board_found = 1;
			}
		}

		if(config_info_found == 0)
		{
			printf("Error! Serial number and configuration data not found in configuration file.\n"); 
			return_sucess = false;
		}


		// If not at end of file and device config info found
		if((config_index < configFileSize) && config_info_found)
		{
			// Configuration data required by every board:
			data_found = 1;
			// Read the board's ADC resolution
			data_found &= SearchConfigData("ADC_RES=", 8, 2, 10, &dev_config[BoardNum].adc_res, config_index, config_array, config_index_end);
			// Read the number of ADC channels on the board
			data_found &= SearchConfigData("ADC_CHAN=", 9, 2, 10, &dev_config[BoardNum].adc_chan, config_index, config_array, config_index_end);
			printf("ADC chan read from board:%d\n",dev_config[BoardNum].adc_chan);
			// Read the board's DAC resolution
			data_found &= SearchConfigData("DAC_RES=", 8, 2, 10, &dev_config[BoardNum].dac_res, config_index, config_array, config_index_end);
			// Read the number of DAC channels on the board
			data_found &= SearchConfigData("DAC_CHAN=", 9, 2, 10, &dev_config[BoardNum].dac_chan, config_index, config_array, config_index_end);

			if(!data_found){ printf("ERROR!!! Channel/Resolution information not found in config file!\n"); return_sucess = false; }

			// Read the board's pretrigger memory
			data_found &= SearchConfigData("PRE_TRIGGER=", 12, 5, 10, &dev_config[BoardNum].pre_trigger_len, config_index, config_array, config_index_end);

			data_found &= SearchConfigData("FPGA_TYPE=", 10, 3, 10, &dev_config[BoardNum].fpga_type, config_index, config_array, config_index_end);

			data_found &= SearchConfigData("DRAM_DQS=", 9, 2, 10, &dev_config[BoardNum].dram_dqs, config_index, config_array, config_index_end);
			data_found &= SearchConfigData("DRAM_DQ=", 8, 2, 10, &dev_config[BoardNum].dram_dq, config_index, config_array, config_index_end);

			data_found &= SearchConfigData("ECL_TRIGGER_DELAY=", 18, 8, 10, &dev_config[BoardNum].ecl_trigger_delay, config_index, config_array, config_index_end);

			// If board is an ADC board
			if( dev_config[BoardNum].adc_chan > 0)
			{
				// If board is an 8-bit board
				if(dev_config[BoardNum].adc_res == 8)
				{
					// Configuration data required for 8-bit ADC boards
					data_found = 1;

					// Get the RHS, I-Channel offset
					data_found &= SearchConfigData("ADC2OFF=", 8, 3, 16, &dev_config[BoardNum].adc2_off, config_index, config_array, config_index_end);
					// Get the LHS, I-Channel offset
					data_found &= SearchConfigData("ADC3OFF=", 8, 3, 16, &dev_config[BoardNum].adc3_off, config_index, config_array, config_index_end);

					// Get the RHS, I-Channel FSR
					data_found &= SearchConfigData("ADC2FSR=", 8, 3, 16, &dev_config[BoardNum].adc2_fsr, config_index, config_array, config_index_end);
					// Get the LHS, I-Channel FSR
					data_found &= SearchConfigData("ADC3FSR=", 8, 3, 16, &dev_config[BoardNum].adc3_fsr, config_index, config_array, config_index_end);

					// Get the RHS, I-Channel Offset sign
					data_found &= SearchConfigData("ADC2OFFNEG=", 11, 1, 16, &dev_config[BoardNum].adc2_off_neg, config_index, config_array, config_index_end);  // 1 digit value
					// Get the LHS, I-Channel Offset sign
					data_found &= SearchConfigData("ADC3OFFNEG=", 11, 1, 16, &dev_config[BoardNum].adc3_off_neg, config_index, config_array, config_index_end);  // 1 digit value

					if(!data_found){ printf("ERROR!!! 8-bit ADC information not found in config file!\n"); return_sucess = false; }

					// Does board use ADC08D1520?
					data_found &= SearchConfigData("ADC08D1520=", 11, 1, 10, &dev_config[BoardNum].is_adc08d1520, config_index, config_array, config_index_end);

					if(dev_config[BoardNum].is_adc08d1520)
					{
						// Configuration data required for 8-bit ADC08D1520 boards
						data_found = 1;

						// Get the RHS, I-Channel offset80
						data_found &= SearchConfigData("ADC2OFF_Q=", 10, 3, 16, &dev_config[BoardNum].adc2_off_Q, config_index, config_array, config_index_end);
						// Get the LHS, I-Channel offset
						data_found &= SearchConfigData("ADC3OFF_Q=", 10, 3, 16, &dev_config[BoardNum].adc3_off_Q, config_index, config_array, config_index_end);

						// Get the RHS, I-Channel FSR
						data_found &= SearchConfigData("ADC2FSR_Q=", 10, 3, 16, &dev_config[BoardNum].adc2_fsr_Q, config_index, config_array, config_index_end);
						// Get the LHS, I-Channel FSR
						data_found &= SearchConfigData("ADC3FSR_Q=", 10, 3, 16, &dev_config[BoardNum].adc3_fsr_Q, config_index, config_array, config_index_end);

						// Get the RHS, I-Channel Offset sign
						data_found &= SearchConfigData("ADC2OFFNEG_Q=", 13, 1, 16, &dev_config[BoardNum].adc2_off_neg_Q, config_index, config_array, config_index_end);  // 1 digit value
						// Get the LHS, I-Channel Offset sign
						data_found &= SearchConfigData("ADC3OFFNEG_Q=", 13, 1, 16, &dev_config[BoardNum].adc3_off_neg_Q, config_index, config_array, config_index_end);  // 1 digit value

						//if(!data_found){ printf("ERROR!!! 8-bit ADC08D1520 Q-channel information not found in config file!\n"); return_sucess = false; }
					}

					// Get the 500 Mhz clock fine phase shift value
					data_found &= SearchConfigData("CLK500PHS=", 10, 3, 16, &dev_config[BoardNum].clk500_phs, config_index, config_array, config_index_end);
					// Get the 1000 Mhz clock fine phase shift value
					data_found &= SearchConfigData("CLK1000PHS=", 11, 3, 16, &dev_config[BoardNum].clk1000_phs, config_index, config_array, config_index_end);
					// Fine phase shift LHS or RHS ADC? (Basically the clock phase shift direction)
					data_found &= SearchConfigData("SHIFTADC2=", 10, 1, 16, &dev_config[BoardNum].shift_adc2, config_index, config_array, config_index_end);     // 1 digit value
				} // end if 8-bit board
				// If board is an 12-bit board
				else if(dev_config[BoardNum].adc_res == 12)
				{
					// Configuration data required for 12-bit ADC boards; NOTE: Ch0 = Q, Ch1 = I
					data_found = 1;

					// Get the LHS, I-Channel offset
					data_found &= SearchConfigData("ADC3OFF=", 8, 4, 16, &dev_config[BoardNum].adc3_off, config_index, config_array, config_index_end);

					// Get the LHS, I-Channel FSR
					data_found &= SearchConfigData("ADC3FSR=", 8, 4, 16, &dev_config[BoardNum].adc3_fsr, config_index, config_array, config_index_end);

					// Get the LHS, I-Channel Offset sign
					data_found &= SearchConfigData("ADC3OFFNEG=", 11, 1, 16, &dev_config[BoardNum].adc3_off_neg, config_index, config_array, config_index_end);  // 1 digit value

					// Get the LHS, Q-Channel offset
					data_found &= SearchConfigData("ADC3OFF_Q=", 10, 4, 16, &dev_config[BoardNum].adc3_off_Q, config_index, config_array, config_index_end);

					// Get the LHS, Q-Channel FSR
					data_found &= SearchConfigData("ADC3FSR_Q=", 10, 4, 16, &dev_config[BoardNum].adc3_fsr_Q, config_index, config_array, config_index_end);

					// Get the LHS, Q-Channel Offset sign
					data_found &= SearchConfigData("ADC3OFFNEG_Q=", 13, 1, 16, &dev_config[BoardNum].adc3_off_neg_Q, config_index, config_array, config_index_end);  // 1 digit value

					data_found &= SearchConfigData("ADC12D2000=", 11, 1, 10, &dev_config[BoardNum].is_adc12d2000, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ADC12D2000_CALIBRATION_VALUES=", 30, 4, 16, &dev_config[BoardNum].ADC12D2000_CALIBRATION_VALUES, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ADC12D2000_BIAS_ADJUST=", 23, 4, 16, &dev_config[BoardNum].ADC12D2000_BIAS_ADJUST, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ADC12D2000_DES_TIMING_ADJUST=", 29, 2, 16, &dev_config[BoardNum].ADC12D2000_DES_TIMING_ADJUST, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ADC12D2000_APERTURE_DELAY_COARSE=", 33, 4, 16, &dev_config[BoardNum].ADC12D2000_APERTURE_DELAY_COARSE, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ADC12D2000_APERTURE_DELAY_FINE=", 31, 4, 16, &dev_config[BoardNum].ADC12D2000_APERTURE_DELAY_FINE, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ADC12D2000_DESI_Q_OFFSET=", 25, 4, 16, &dev_config[BoardNum].ADC12D2000_desi_q_offset, config_index, config_array, config_index_end);					
					data_found &= SearchConfigData("ADC12D2000_DESI_Q_OFFSET_NEG=", 29, 1, 16, &dev_config[BoardNum].ADC12D2000_desi_q_offset_neg, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ADC12D2000_DESQ_I_OFFSET=", 25, 4, 16, &dev_config[BoardNum].ADC12D2000_desq_i_offset, config_index, config_array, config_index_end);	
					data_found &= SearchConfigData("ADC12D2000_DESQ_I_OFFSET_NEG=", 29, 1, 16, &dev_config[BoardNum].ADC12D2000_desq_i_offset_neg, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ADC12D2000_DESIQ_Q_OFFSET=", 26, 4, 16, &dev_config[BoardNum].ADC12D2000_desiq_q_offset, config_index, config_array, config_index_end);					
					data_found &= SearchConfigData("ADC12D2000_DESIQ_Q_OFFSET_NEG=", 30, 1, 16, &dev_config[BoardNum].ADC12D2000_desiq_q_offset_neg, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ADC12D2000_DESIQ_I_OFFSET=", 26, 4, 16, &dev_config[BoardNum].ADC12D2000_desiq_i_offset, config_index, config_array, config_index_end);	
					data_found &= SearchConfigData("ADC12D2000_DESIQ_I_OFFSET_NEG=", 30, 1, 16, &dev_config[BoardNum].ADC12D2000_desiq_i_offset_neg, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ADC12D2000_DESCLKIQ_Q_OFFSET=", 29, 4, 16, &dev_config[BoardNum].ADC12D2000_desclkiq_q_offset, config_index, config_array, config_index_end);					
					data_found &= SearchConfigData("ADC12D2000_DESCLKIQ_Q_OFFSET_NEG=", 33, 1, 16, &dev_config[BoardNum].ADC12D2000_desclkiq_q_offset_neg, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ADC12D2000_DESCLKIQ_I_OFFSET=", 29, 4, 16, &dev_config[BoardNum].ADC12D2000_desclkiq_i_offset, config_index, config_array, config_index_end);	
					data_found &= SearchConfigData("ADC12D2000_DESCLKIQ_I_OFFSET_NEG=", 33, 1, 16, &dev_config[BoardNum].ADC12D2000_desclkiq_i_offset_neg, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ADC12D2000_DESCLKIQ_Q_FSR=", 26, 4, 16, &dev_config[BoardNum].ADC12D2000_desclkiq_q_fsr, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ADC12D2000_DESCLKIQ_I_FSR=", 26, 4, 16, &dev_config[BoardNum].ADC12D2000_desclkiq_i_fsr, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ISLA_ADC0_IDELAY_OFFSET=", 24, 2, 10, &dev_config[BoardNum].ISLA_ADC0_IDELAY_OFFSET, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC1_IDELAY_OFFSET=", 24, 2, 10, &dev_config[BoardNum].ISLA_ADC1_IDELAY_OFFSET, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC2_IDELAY_OFFSET=", 24, 2, 10, &dev_config[BoardNum].ISLA_ADC2_IDELAY_OFFSET, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC3_IDELAY_OFFSET=", 24, 2, 10, &dev_config[BoardNum].ISLA_ADC3_IDELAY_OFFSET, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("MICROSYNTH=", 11, 1, 10, &dev_config[BoardNum].MICROSYNTH, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("MICROSYNTH_OSC_FREQ=", 20, 9, 10, &dev_config[BoardNum].MICROSYNTH_OSC_FREQ, config_index, config_array, config_index_end);

				}	// end if 12-bit board
				else if (dev_config[BoardNum].adc_res == 14)
				{
					// Configuration data required for non 8-bit ADC boards
					data_found = 1;

					data_found &= SearchConfigData("AD5474=", 7, 1, 10, &dev_config[BoardNum].is_AD5474, config_index, config_array, configFileSize);

					data_found &= SearchConfigData("ISLA214P=", 9, 1, 10, &dev_config[BoardNum].is_ISLA214P, config_index, config_array, configFileSize);

					data_found &= SearchConfigData("ISLA_ADC0_GAIN_COARSE0=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC0_GAIN_COARSE0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC0_GAIN_MEDIUM0=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC0_GAIN_MEDIUM0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC0_GAIN_FINE0=", 21, 2, 16, &dev_config[BoardNum].ISLA_ADC0_GAIN_FINE0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC0_OFFSET_FINE0=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC0_OFFSET_FINE0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC0_OFFSET_COARSE0=", 25, 2, 16, &dev_config[BoardNum].ISLA_ADC0_OFFSET_COARSE0, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ISLA_ADC1_GAIN_COARSE0=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC1_GAIN_COARSE0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC1_GAIN_MEDIUM0=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC1_GAIN_MEDIUM0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC1_GAIN_FINE0=", 21, 2, 16, &dev_config[BoardNum].ISLA_ADC1_GAIN_FINE0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC1_OFFSET_FINE0=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC1_OFFSET_FINE0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC1_OFFSET_COARSE0=", 25, 2, 16, &dev_config[BoardNum].ISLA_ADC1_OFFSET_COARSE0, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ISLA_ADC0_GAIN_COARSE1=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC0_GAIN_COARSE1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC0_GAIN_MEDIUM1=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC0_GAIN_MEDIUM1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC0_GAIN_FINE1=", 21, 2, 16, &dev_config[BoardNum].ISLA_ADC0_GAIN_FINE1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC0_OFFSET_FINE1=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC0_OFFSET_FINE1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC0_OFFSET_COARSE1=", 25, 2, 16, &dev_config[BoardNum].ISLA_ADC0_OFFSET_COARSE1, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ISLA_ADC1_GAIN_COARSE1=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC1_GAIN_COARSE1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC1_GAIN_MEDIUM1=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC1_GAIN_MEDIUM1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC1_GAIN_FINE1=", 21, 2, 16, &dev_config[BoardNum].ISLA_ADC1_GAIN_FINE1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC1_OFFSET_FINE1=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC1_OFFSET_FINE1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC1_OFFSET_COARSE1=", 25, 2, 16, &dev_config[BoardNum].ISLA_ADC1_OFFSET_COARSE1, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ISLA_ADC0_IDELAY_OFFSET=", 24, 2, 16, &dev_config[BoardNum].ISLA_ADC0_IDELAY_OFFSET, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC1_IDELAY_OFFSET=", 24, 2, 16, &dev_config[BoardNum].ISLA_ADC1_IDELAY_OFFSET, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC2_IDELAY_OFFSET=", 24, 2, 16, &dev_config[BoardNum].ISLA_ADC2_IDELAY_OFFSET, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC3_IDELAY_OFFSET=", 24, 2, 16, &dev_config[BoardNum].ISLA_ADC3_IDELAY_OFFSET, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ADCGAIN0=", 9, 4, 16, &dev_config[BoardNum].adc_gain0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ADCGAIN1=", 9, 4, 16, &dev_config[BoardNum].adc_gain1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ADCOFFS0=", 9, 4, 16, &dev_config[BoardNum].adc_offs0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ADCOFFS1=", 9, 4, 16, &dev_config[BoardNum].adc_offs1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ADCBIAS0=", 9, 4, 16, &dev_config[BoardNum].adc_bias0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ADCBIAS1=", 9, 4, 16, &dev_config[BoardNum].adc_bias1, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ISLA_2X_CLK=", 12, 1, 16, &dev_config[BoardNum].ISLA_2X_CLK, config_index, config_array, config_index_end);

					//if(!data_found){ printf("ERROR!!! 14-bit ADC board information not found in config file!\n"); return_sucess = false; }
				}
				else if (dev_config[BoardNum].adc_res == 16)
				{

					// Configuration data required for non 8-bit ADC boards
					data_found = 1;

					data_found &= SearchConfigData("ISLA216P=", 9, 1, 10, &dev_config[BoardNum].is_ISLA216P, config_index, config_array, configFileSize);

					data_found &= SearchConfigData("ISLA_ADC0_GAIN_COARSE0=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC0_GAIN_COARSE0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC0_GAIN_MEDIUM0=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC0_GAIN_MEDIUM0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC0_GAIN_FINE0=", 21, 2, 16, &dev_config[BoardNum].ISLA_ADC0_GAIN_FINE0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC0_OFFSET_FINE0=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC0_OFFSET_FINE0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC0_OFFSET_COARSE0=", 25, 2, 16, &dev_config[BoardNum].ISLA_ADC0_OFFSET_COARSE0, config_index, config_array, config_index_end);


					data_found &= SearchConfigData("ISLA_ADC1_GAIN_COARSE0=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC1_GAIN_COARSE0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC1_GAIN_MEDIUM0=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC1_GAIN_MEDIUM0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC1_GAIN_FINE0=", 21, 2, 16, &dev_config[BoardNum].ISLA_ADC1_GAIN_FINE0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC1_OFFSET_FINE0=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC1_OFFSET_FINE0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC1_OFFSET_COARSE0=", 25, 2, 16, &dev_config[BoardNum].ISLA_ADC1_OFFSET_COARSE0, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ISLA_ADC2_GAIN_COARSE0=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC2_GAIN_COARSE0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC2_GAIN_MEDIUM0=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC2_GAIN_MEDIUM0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC2_GAIN_FINE0=", 21, 2, 16, &dev_config[BoardNum].ISLA_ADC2_GAIN_FINE0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC2_OFFSET_FINE0=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC2_OFFSET_FINE0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC2_OFFSET_COARSE0=", 25, 2, 16, &dev_config[BoardNum].ISLA_ADC2_OFFSET_COARSE0, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ISLA_ADC3_GAIN_COARSE0=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC3_GAIN_COARSE0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC3_GAIN_MEDIUM0=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC3_GAIN_MEDIUM0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC3_GAIN_FINE0=", 21, 2, 16, &dev_config[BoardNum].ISLA_ADC3_GAIN_FINE0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC3_OFFSET_FINE0=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC3_OFFSET_FINE0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC3_OFFSET_COARSE0=", 25, 2, 16, &dev_config[BoardNum].ISLA_ADC3_OFFSET_COARSE0, config_index, config_array, config_index_end);


					data_found &= SearchConfigData("ISLA_ADC0_GAIN_COARSE1=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC0_GAIN_COARSE1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC0_GAIN_MEDIUM1=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC0_GAIN_MEDIUM1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC0_GAIN_FINE1=", 21, 2, 16, &dev_config[BoardNum].ISLA_ADC0_GAIN_FINE1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC0_OFFSET_FINE1=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC0_OFFSET_FINE1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC0_OFFSET_COARSE1=", 25, 2, 16, &dev_config[BoardNum].ISLA_ADC0_OFFSET_COARSE1, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ISLA_ADC1_GAIN_COARSE1=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC1_GAIN_COARSE1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC1_GAIN_MEDIUM1=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC1_GAIN_MEDIUM1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC1_GAIN_FINE1=", 21, 2, 16, &dev_config[BoardNum].ISLA_ADC1_GAIN_FINE1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC1_OFFSET_FINE1=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC1_OFFSET_FINE1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC1_OFFSET_COARSE1=", 25, 2, 16, &dev_config[BoardNum].ISLA_ADC1_OFFSET_COARSE1, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ISLA_ADC2_GAIN_COARSE1=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC2_GAIN_COARSE1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC2_GAIN_MEDIUM1=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC2_GAIN_MEDIUM1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC2_GAIN_FINE1=", 21, 2, 16, &dev_config[BoardNum].ISLA_ADC2_GAIN_FINE1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC2_OFFSET_FINE1=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC2_OFFSET_FINE1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC2_OFFSET_COARSE1=", 25, 2, 16, &dev_config[BoardNum].ISLA_ADC2_OFFSET_COARSE1, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ISLA_ADC3_GAIN_COARSE1=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC3_GAIN_COARSE1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC3_GAIN_MEDIUM1=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC3_GAIN_MEDIUM1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC3_GAIN_FINE1=", 21, 2, 16, &dev_config[BoardNum].ISLA_ADC3_GAIN_FINE1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC3_OFFSET_FINE1=", 23, 2, 16, &dev_config[BoardNum].ISLA_ADC3_OFFSET_FINE1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC3_OFFSET_COARSE1=", 25, 2, 16, &dev_config[BoardNum].ISLA_ADC3_OFFSET_COARSE1, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ISLA_ADC0_IDELAY_OFFSET=", 24, 2, 16, &dev_config[BoardNum].ISLA_ADC0_IDELAY_OFFSET, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC1_IDELAY_OFFSET=", 24, 2, 16, &dev_config[BoardNum].ISLA_ADC1_IDELAY_OFFSET, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC2_IDELAY_OFFSET=", 24, 2, 16, &dev_config[BoardNum].ISLA_ADC2_IDELAY_OFFSET, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ISLA_ADC3_IDELAY_OFFSET=", 24, 2, 16, &dev_config[BoardNum].ISLA_ADC3_IDELAY_OFFSET, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ADCBIAS0=", 9, 4, 16, &dev_config[BoardNum].adc_bias0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ADCBIAS1=", 9, 4, 16, &dev_config[BoardNum].adc_bias1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ADCBIAS2=", 9, 4, 16, &dev_config[BoardNum].adc_bias2, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ADCBIAS3=", 9, 4, 16, &dev_config[BoardNum].adc_bias3, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ADCOFFS0=", 9, 4, 16, &dev_config[BoardNum].adc_offs0, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ADCOFFS1=", 9, 4, 16, &dev_config[BoardNum].adc_offs1, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ADCOFFS2=", 9, 4, 16, &dev_config[BoardNum].adc_offs2, config_index, config_array, config_index_end);
					data_found &= SearchConfigData("ADCOFFS3=", 9, 4, 16, &dev_config[BoardNum].adc_offs3, config_index, config_array, config_index_end);

					data_found &= SearchConfigData("ISLA_2X_CLK=", 12, 1, 16, &dev_config[BoardNum].ISLA_2X_CLK, config_index, config_array, config_index_end);
				}
			}


		}

	}


	dev_config[BoardNum].serial_number = serial_number;

	dev_config[BoardNum].max_adc_value = 4095;
	dev_config[BoardNum].strobe_count_mult = 4;
	dev_config[BoardNum].adc_error_value = 128;

	/*
	// If board is an ADC board
	if( dev_config[BoardNum].adc_chan > 0) 
	{
		if(!ss->single_chan_mode)
		{
			dev_config[BoardNum].samples_per_block = DIG_BLOCKSIZE/4;
			dev_config[BoardNum].adc_chan_used = 2;
		}
		else
		{
			dev_config[BoardNum].samples_per_block = DIG_BLOCKSIZE/2;
			dev_config[BoardNum].adc_chan_used = 1;     
		}
	}
	*/

	// mike ingle added
	dev_config[BoardNum].continuous_arf = true;

	if(dev_config[BoardNum].adc_res == 8)
	{
		AdcSetGain(BoardNum,dev_config[BoardNum].adc2_fsr, true, false);		                        // I-Channel    RHS ADC
		AdcSetGain(BoardNum,dev_config[BoardNum].adc3_fsr, false, false);                             // I-Channel    LHS ADC
		AdcSetOffset(BoardNum,dev_config[BoardNum].adc3_off, dev_config[BoardNum].adc3_off_neg, false, false);	// I-Channel    LHS ADC
		AdcSetOffset(BoardNum,dev_config[BoardNum].adc2_off, dev_config[BoardNum].adc2_off_neg, true, false);	// I-Channel    RHS ADC

		if(dev_config[BoardNum].is_adc08d1520)
		{
			AdcSetGain(BoardNum,dev_config[BoardNum].adc2_fsr_Q, true, true);		                            // Q-Channel    RHS ADC
			AdcSetGain(BoardNum,dev_config[BoardNum].adc3_fsr_Q, false, true);                                // Q-Channel    LHS ADC
			AdcSetOffset(BoardNum,dev_config[BoardNum].adc3_off_Q, dev_config[BoardNum].adc3_off_neg_Q, false, true);	// Q-Channel    LHS ADC
			AdcSetOffset(BoardNum,dev_config[BoardNum].adc2_off_Q, dev_config[BoardNum].adc2_off_neg_Q, true, true);	    // Q-Channel    RHS ADC
		}
		set_adc_data_setup_reg(BoardNum);
	}


	if(config_array != NULL){ free(config_array); }

	return true;
}

// Search the configuration data file for a specific entry for the current board, whose data begins at start_index.
unsigned short SearchConfigData(char * search_term, int term_length, int value_length, int value_radix, unsigned int * config_data, int start_index, char *config_array, unsigned long FileSize)
{

	int i;
	unsigned long j;
	char alpha_data[25];
	unsigned short config_info_found = 0;


	for(j=start_index; j<FileSize-8; j++)
	{
		if( (strncmp(search_term, config_array+j, term_length)) == 0 )
		{
			i=j;
			j=FileSize+1;
			config_info_found=1; 	
		}
	}

	if(config_info_found)
	{
		i+= term_length;                                // move to the data portion of the current entry

		for(j=0; j< (unsigned long)value_length; j++)
		{
			alpha_data[j] = config_array[i+j];
		}

		alpha_data[j] = '\n';   // Terminate the string

		*config_data = strtol(alpha_data, NULL, value_radix);     // Convert to a number
	}

	return config_info_found;
}

void CheckDeviceStatus(unsigned short BoardNum)
{
	unsigned long status = 0;
	unsigned long bus_error = 0;
	short have_error=0;

	status = GET_DEVICE_STATUS();
	bus_error = (status & 0x0000000F);

	if(bus_error & 0x1){
		printf("Bus Error: Unsupported Request\n");
		have_error = 1;
	}
	if(bus_error & 0x2){
		printf("Bus Error: Configuration Request Retry Status\n");
		have_error = 1;
	}
	if(bus_error & 0x4){
		printf("Bus Error: Completer Abort\n");
		have_error = 1;
	}

	if(have_error == 0)
	{
		//printf("Device Status Clear\n");
	}
}

void CONFIGURE_CONTROL_REGISTER(unsigned short BoardNum)
{
	SetSoftwareRunBit(BoardNum, 0);  // need to figure out what this does, and whether to set it
	SetSoftwareStopBit(BoardNum, 0);
	SetInterleavedReadWrite(BoardNum, 1);
	SetReadWholeRow(BoardNum, 0);
	//SetInternalClockEnable(BoardNum, 1);
	SetWriteWholeRow(BoardNum, 1);
}

void SetSoftwareRunBit (unsigned short BoardNum, unsigned int Enable)
{
	SET_CONTROL_REG(BoardNum,SOFTWARE_RUN_BIT, Enable);

	// Shadow reg does not seem to work?
	/*
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CONTROL, 0, 0);
	tmp = tmp & 0xf7ffffff; 
	val = ((Enable & 0x1) << SOFTWARE_RUN_BIT);
	tmp |= val;

	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CONTROL, tmp, 0, 0);
*/
}

void SetSoftwareStopBit (unsigned short BoardNum, unsigned int Enable)
{
	SET_CONTROL_REG(BoardNum, SOFTWARE_STOP, Enable);

	// Shadow reg does not seem to work?
	/*
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CONTROL, 0, 0);
	tmp = tmp & 0xfbffffff; 
	val = ((Enable & 0x1) << SOFTWARE_STOP);
	tmp |= val;

	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CONTROL, tmp, 0, 0);
*/
}

void SetInterleavedReadWrite (unsigned short BoardNum, unsigned int Enable)
{
	SET_CONTROL_REG(BoardNum,INTERLEAVED_RW, Enable);
	// Shadow reg does not seem to work?
	/*
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CONTROL, 0, 0);
	tmp = tmp & 0xfdffffff; 
	val = ((Enable & 0x1) << INTERLEAVED_RW);
	tmp |= val;

	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CONTROL, tmp, 0, 0);
*/
}

void SetReadWholeRow (unsigned short BoardNum, unsigned int Enable)
{
	SET_CONTROL_REG(BoardNum,READ_WHOLE_ROW, Enable);
	/*
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CONTROL, 0, 0);
	tmp = tmp & 0xfeffffff; 
	val = ((Enable & 0x1) << READ_WHOLE_ROW);
	tmp |= val;

	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CONTROL, tmp, 0, 0);
*/
}

EXPORTED_FUNCTION void SetSetupDoneBit(unsigned short BoardNum, unsigned int Enable)
{
	SET_CONTROL_REG(BoardNum, SETUP_DONE, Enable);
}

EXPORTED_FUNCTION bool SetupMicrosynth(unsigned short BoardNum)
{
	if (!dev_config[BoardNum].MICROSYNTH){        //to check if system contains microsynth oscillator or not, not sure if needed
		printf("no microsynth\n");
		return false;
	}

	#define UL unsigned long int

	#define REG15_CONST ((UL)1<<25)+((UL)1<<20)+((UL)1<<19)+((UL)1<<18)+((UL)1<<17)+((UL)1<<16)+((UL)1<<15)+(1<<14)+(1<<13)+15
	#define REG13_CONST (1<<14)+(1<<8)+13
	#define REG10_CONST 0x210050CA
	#define REG9_CONST 0x03C7C039
	#define REG8_CONST 0x207DDBF8
	//note:10/9/8 all constant
	#define REG7_CONST 7
	#define REG6_CONST (1<<10)+6
	#define REG5_CONST 5
	#define REG4_CONST 4
	#define REG3_CONST ((UL)1<<29)+3
	#define REG2_CONST ((UL)1<<26)+2
	#define REG1_CONST 1
	#define REG0_CONST 0

	#define OFFSET_VCO_CAP_MAN 12           //register 15 1 bit
	#define OFFSET_VCO_CAPCODE 4            //register 15 8 bits
	#define OFFSET_DLD_ERR_CNT 28           //register 13 4 bits
	#define OFFSET_DLD_PASS_CNT 18          //register 13 10 bits
	#define OFFSET_DLD_TOL 15               //register 13 3 bits
	#define OFFSET_FL_SELECT 26             //register 7 5 bits
	#define OFFSET_FL_PINMODE 23            //register 7 3 bits
	#define OFFSET_FL_INV 22                //register 7 1 bit
	#define OFFSET_MUXOUT_SELECT 17         //register 7 5 bits
	#define OFFSET_MUX_INV 16               //register 7 1 bit
	#define OFFSET_MUXOUT_PINMODE 13        //register 7 3 bits
	#define OFFSET_LD_SELECT 8              //register 7 5 bits
	#define OFFSET_LD_INV 7                 //register 7 1 bit
	#define OFFSET_LD_PINMODE 4             //register 7 3 bits
	#define OFFSET_RD_DIAGNOSTICS 11        //register 6 20 bits
	#define OFFSET_RDADDR 5                 //register 6 4 bits
	#define OFFSET_uWIRE_LOCK 4             //register 6 1 bit
	#define OFFSET_OUT_LDEN 24              //register 5 1 bit
	#define OFFSET_OSC_FREQ 21              //register 5 3 bits
	#define OFFSET_BUFEN_DIS 20             //register 5 1 bit
	#define OFFSET_VCO_SEL_MODE 15          //register 5 2 bits
	#define OFFSET_OUTB_MUX 13              //register 5 2 bits
	#define OFFSET_OUTA_MUX 11              //register 5 2 bits
	#define OFFSET_0_DLY 10                 //register 5 1 bit
	#define OFFSET_MODE 8                   //register 5 2 bits
	#define OFFSET_PWDN_MODE 5              //register 5 3 bits
	#define OFFSET_RESET 4                  //register 5 1 bit
	#define OFFSET_PFD_DLY 29               //register 4 3 bits
	#define OFFSET_FL_FRCE 28               //register 4 1 bit
	#define OFFSET_FL_TOC 16                //register 4 12 bits
	#define OFFSET_FL_CPG 11                //register 4 5 bits
	#define OFFSET_CPG_BLEED 4              //register 4 6 bits
	#define OFFSET_VCO_DIV 18               //register 3 5 bits
	#define OFFSET_OUTB_PWR 12              //register 3 6 bits
	#define OFFSET_OUTA_PWR 6               //register 3 6 bits
	#define OFFSET_OUTB_PD 5                //register 3 1 bit
	#define OFFSET_OUTA_PD 4                //register 3 1 bit
	#define OFFSET_OSC_2X 30                //register 2 1 bit
	#define OFFSET_CPP 27                   //register 2 1 bit
	#define OFFSET_PLL_DEN 4                //register 2 22 bits
	#define OFFSET_CPG 27                   //register 1 5 bits
	#define OFFSET_VCO_SEL 25               //register 1 2 bits
	#define OFFSET_PLL_NUM_MSB 15           //register 1 10 bits
	#define OFFSET_FRAC_ORDER 12            //register 1 3 bits
	#define OFFSET_PLL_R 4                  //register 1 8 bits
	#define OFFSET_ID 31                    //register 0 1 bit
	#define OFFSET_FRAC_DITHER 29           //register 0 2 bits
	#define OFFSET_NO_FCAL 28               //register 0, 1 bit
	#define OFFSET_PLL_N 16                 //register 0, 12 bits
	#define OFFSET_PLL_NUM_LSB 4            //register 0, 12 bits

	#define PLL_DEN 4000000                 //this is close to the maximum (4,194,303) while still providing decent hand calculatability
	#define PLL_R 1
	#define CPG 18

	double Fosc = dev_config[BoardNum].MICROSYNTH_OSC_FREQ;         //this is the oscillator frequency
	printf("reference oscillator frequency=%f\n",Fosc);
	if (Fosc>900000000 || Fosc<5000000){
		printf("reference oscillator out of range! 5-900MHz acceptable");
	}
	unsigned int osc_freq_val=0;
	if (Fosc > 64000000){
		osc_freq_val=1;
	}
	if (Fosc > 128000000){
		osc_freq_val=2;
	}
	if (Fosc > 256000000){
		osc_freq_val=3;
	}
	if (Fosc > 512000000){
		osc_freq_val=4;
	}

	//***********************************************************************************************************************************************
	double Frequency = 1800000000;   //!!!ENTER FREQUENCY DESIRED HERE!!!   !!!ENTER FREQUENCY DESIRED HERE!!!   !!!ENTER FREQUENCY DESIRED HERE!!!
	//***********************************************************************************************************************************************

	unsigned int mux_vco_div=1;
	int VCO_DIV=18;
	//First time Setup parameters
	double FoutOverK = (double)((double)Frequency / (double)Fosc); //this equals PLL/VCO_DIV_TRUEDIVIDER
	//#ifdef debug
	//printf("FoutOverK:%f\n",FoutOverK);
	//#endif
	double tempK;    //to keep track of vco_div value that is needed
	for (int i=1;i<=38;i++){
		tempK = (double)FoutOverK * (double)i;
		//    printf("i:%d\n",i);
		if (tempK >= 1880000000.0/Fosc && tempK <=3760000000.0/Fosc){     //this should be correct for all references oscillators
			//#ifdef debug
			//printf("VCO_DIV_VALUE:%d  tempK:%f\n",i,tempK);
			//#endif
			VCO_DIV = i;
			VCO_DIV = (VCO_DIV / 2) - 1;
			break;
		}
		if (i>1){
			i++;
		}
	}
	if (VCO_DIV == -1){
		mux_vco_div=0;
		//#ifdef debug
		printf("VCO divider turned off\n");
		//#endif
	}
	double VCO_FREQUENCY = (Fosc/1000000) * tempK;
	//#ifdef debug
	printf("VCO_DIV:%d\nVCO_freq:%fMHz\n",(VCO_DIV+1)*2,VCO_FREQUENCY);
	//#endif

	//  printf("VCO_DIV:%d\nVCO_freq:%fMHz\n",(VCO_DIV+1)*2,VCO_FREQUENCY);

	unsigned int VCO_CORE_SELECT=0;
	if (VCO_FREQUENCY <=2200){
		VCO_CORE_SELECT=0;
	}
	else if (VCO_FREQUENCY <=2700){
		VCO_CORE_SELECT=1;
	}
	else if (VCO_FREQUENCY <=3200){
		VCO_CORE_SELECT=2;
	}
	else if (VCO_FREQUENCY <=3900){
		VCO_CORE_SELECT=3;
	}

	//#ifdef debug
	//printf("VCO CORE SELECTED:%d\n",VCO_CORE_SELECT);
	//#endif
	double numeratorwhole;
	double numeratorfrac = modf(tempK,&numeratorwhole);
	//#ifdef debug
	//printf("PLL_N:%f +\nPLL_NUM:%f =\n%f\n",numeratorwhole,numeratorfrac,tempK);
	//#endif
	UL numwhole = (UL)numeratorwhole;
	UL numfrac = (UL)(double)((double)numeratorfrac * (double)PLL_DEN);
	//#ifdef debug
	//printf ("numwhole:%lu\nnumfrac:%lu\n",numwhole,numfrac);
	//#endif

	UL PLL_NUM = numfrac;
	unsigned int PLL_N = numwhole;

	UL PLL_NUM1_temp = (PLL_NUM & 0x3FF000)>>12;    //MSB 10 bits modded lower frequency (offset)
	UL PLL_NUM2_temp = (PLL_NUM & 0x000FFF);        //LSB 12 bits modded lower frequency (offset)
	unsigned int PLL_NUM1 = PLL_NUM1_temp;
	unsigned int PLL_NUM2 = PLL_NUM2_temp;

	//#ifdef debug
	//printf("num1:%d,%x\n",PLL_NUM1,PLL_NUM1);
	//printf("num2:%d,%x\n",PLL_NUM2,PLL_NUM2);
	//#endif

	//  printf("PLL_NUM:%lu\n",PLL_NUM);

	UL reg5init = (1<<OFFSET_RESET)+REG5_CONST;
	SET_PIO_REG(BoardNum, PIO_OFFSET_SYNTH_REG, reg5init);
	Sleep(5);
	UL reg15 = (0<<OFFSET_VCO_CAP_MAN)+(128<<OFFSET_VCO_CAPCODE)+REG15_CONST;
	SET_PIO_REG(BoardNum, PIO_OFFSET_SYNTH_REG, reg15);
	Sleep(5);
	UL reg13 = ((UL)4<<OFFSET_DLD_ERR_CNT)+((UL)32<<OFFSET_DLD_PASS_CNT)+((UL)5<<OFFSET_DLD_TOL)+REG13_CONST;
	SET_PIO_REG(BoardNum, PIO_OFFSET_SYNTH_REG, reg13);
	Sleep(5);
	UL reg10 = REG10_CONST;
	SET_PIO_REG(BoardNum, PIO_OFFSET_SYNTH_REG, reg10);
	Sleep(5);
	UL reg9 = REG9_CONST;
	SET_PIO_REG(BoardNum, PIO_OFFSET_SYNTH_REG, reg9);
	Sleep(5);
	UL reg8 = REG8_CONST;
	SET_PIO_REG(BoardNum, PIO_OFFSET_SYNTH_REG, reg8);
	Sleep(5);
	UL reg7 = ((UL)0<<OFFSET_FL_SELECT)+((UL)2<<OFFSET_FL_PINMODE)+((UL)0<<OFFSET_FL_INV)+((UL)1<<OFFSET_MUXOUT_SELECT)+((UL)0<<OFFSET_MUX_INV)+(1<<OFFSET_MUXOUT_PINMODE)+(2<<OFFSET_LD_SELECT)+(0<<OFFSET_LD_INV)+(3<<OFFSET_LD_PINMODE)+REG7_CONST;
	SET_PIO_REG(BoardNum, PIO_OFFSET_SYNTH_REG, reg7);
	Sleep(5);
	UL reg6 = ((UL)0<<OFFSET_RD_DIAGNOSTICS)+(0<<OFFSET_RDADDR)+(0<<OFFSET_uWIRE_LOCK)+REG6_CONST;
	SET_PIO_REG(BoardNum, PIO_OFFSET_SYNTH_REG, reg6);
	Sleep(5);
	UL reg5 = ((UL)0<<OFFSET_OUT_LDEN)+((UL)osc_freq_val<<OFFSET_OSC_FREQ)+((UL)0<<OFFSET_BUFEN_DIS)+((UL)1<<OFFSET_VCO_SEL_MODE)+(mux_vco_div<<OFFSET_OUTB_MUX)+(mux_vco_div<<OFFSET_OUTA_MUX)+(0<<OFFSET_0_DLY)+(0<<OFFSET_MODE)+(0<<OFFSET_PWDN_MODE)+(0<<OFFSET_RESET)+REG5_CONST;
	SET_PIO_REG(BoardNum, PIO_OFFSET_SYNTH_REG, reg5);
	Sleep(5);
	UL reg4 = ((UL)0<<OFFSET_PFD_DLY)+((UL)0<<OFFSET_FL_FRCE)+((UL)64<<OFFSET_FL_TOC)+((UL)31<<OFFSET_FL_CPG)+(0<<OFFSET_CPG_BLEED)+REG4_CONST;
	SET_PIO_REG(BoardNum, PIO_OFFSET_SYNTH_REG, reg4);
	Sleep(5);
	UL reg3 = ((UL)VCO_DIV<<OFFSET_VCO_DIV)+((UL)30<<OFFSET_OUTB_PWR)+(30<<OFFSET_OUTA_PWR)+(1<<OFFSET_OUTB_PD)+(0<<OFFSET_OUTA_PD)+REG3_CONST;
	SET_PIO_REG(BoardNum, PIO_OFFSET_SYNTH_REG, reg3);
	Sleep(5);
	UL reg2 = ((UL)0<<OFFSET_OSC_2X)+((UL)1<<OFFSET_CPP)+((UL)PLL_DEN<<OFFSET_PLL_DEN)+REG2_CONST;
	SET_PIO_REG(BoardNum, PIO_OFFSET_SYNTH_REG, reg2);
	Sleep(5);
	UL reg1 = ((UL)CPG<<OFFSET_CPG)+((UL)VCO_CORE_SELECT<<OFFSET_VCO_SEL)+((UL)(PLL_NUM1)<<OFFSET_PLL_NUM_MSB)+(3<<OFFSET_FRAC_ORDER)+(PLL_R<<OFFSET_PLL_R)+REG1_CONST;
	SET_PIO_REG(BoardNum, PIO_OFFSET_SYNTH_REG, reg1);
	Sleep(5);
	UL reg0 = ((UL)0<<OFFSET_ID)+((UL)0<<OFFSET_FRAC_DITHER)+((UL)0<<OFFSET_NO_FCAL)+((UL)(PLL_N)<<OFFSET_PLL_N)+((UL)(PLL_NUM2)<<OFFSET_PLL_NUM_LSB)+REG0_CONST;
	SET_PIO_REG(BoardNum, PIO_OFFSET_SYNTH_REG, reg0);
	Sleep(50);
	SET_PIO_REG(BoardNum, PIO_OFFSET_SYNTH_REG, reg0);               //register 0 must be written again after wait
	Sleep(5);
	return true;
}

EXPORTED_FUNCTION void FreqMicrosynth(unsigned short BoardNum, double Frequency)
{
	bool specialcase=0;		//page 19 in manual
	if (dev_config[BoardNum].MICROSYNTH){        //to check if system contains microsynth oscillator or not, not sure if needed

		double Fosc = dev_config[BoardNum].MICROSYNTH_OSC_FREQ;         //this is the oscillator frequency
		if (Fosc>900000000 || Fosc<5000000){
			printf("reference oscillator out of range! 5-900MHz acceptable");
		}
		unsigned int osc_freq_val=0;
		if (Fosc > 64000000){
			osc_freq_val=1;
		}
		if (Fosc > 128000000){
			osc_freq_val=2;
		}
		if (Fosc > 256000000){
			osc_freq_val=3;
		}
		if (Fosc > 512000000){
			osc_freq_val=4;
		}
		unsigned int mux_vco_div=1;
		int VCO_DIV=18;
		//First time Setup parameters
		double FoutOverK = (double)((double)Frequency / (double)Fosc); //this equals PLL/VCO_DIV_TRUEDIVIDER
		//#ifdef debug
		//printf("FoutOverK:%f\n",FoutOverK);
		//#endif
		//  printf("sizeofdouble(should be 8):%d\n",sizeof(double));
		double tempK;    //to keep track of vco_div value that is needed
		for (int i=1;i<=38;i++){
			tempK = (double)FoutOverK * (double)i;
			//    printf("i:%d\n",i);
			if (tempK >= 1880000000.0/Fosc && tempK <=3760000000.0/Fosc){     //this should be correct for all references oscillators
				//#ifdef debug
				//printf("VCO_DIV_VALUE:%d  tempK:%f\n",i,tempK);
				//#endif
				VCO_DIV = i;
				VCO_DIV = (VCO_DIV / 2) - 1;
				if (VCO_DIV==4){
					specialcase = 1;
				}
				break;
			}
			if (i>1){
				i++;
			}
		}
		if (VCO_DIV == -1){
			mux_vco_div=0;
			//#ifdef debug
			printf("VCO divider turned off\n");
			//#endif
		}
		double VCO_FREQUENCY = (Fosc/1000000) * tempK;
		//#ifdef debug
		printf("VCO_DIV:%d\nVCO_freq:%fMHz\n",(VCO_DIV+1)*2,VCO_FREQUENCY);
		//#endif

		//  printf("VCO_DIV:%d\nVCO_freq:%fMHz\n",(VCO_DIV+1)*2,VCO_FREQUENCY);

		unsigned int VCO_CORE_SELECT=0;
		if (VCO_FREQUENCY <=2200){
			VCO_CORE_SELECT=0;
		}
		else if (VCO_FREQUENCY <=2700){
			VCO_CORE_SELECT=1;
		}
		else if (VCO_FREQUENCY <=3200){
			VCO_CORE_SELECT=2;
		}
		else if (VCO_FREQUENCY <=3900){
			VCO_CORE_SELECT=3;
		}

		//#ifdef debug
		//printf("VCO CORE SELECTED:%d\n",VCO_CORE_SELECT);
		//#endif
		double numeratorwhole;
		double numeratorfrac = modf(tempK,&numeratorwhole);
		//#ifdef debug
		//printf("PLL_N:%f +\nPLL_NUM:%f =\n%f\n",numeratorwhole,numeratorfrac,tempK);
		//#endif
		UL numwhole = (UL)numeratorwhole;
		UL numfrac = (UL)(double)((double)numeratorfrac * (double)PLL_DEN);
		//#ifdef debug
		//  printf ("numwhole:%lu\nnumfrac:%lu\n",numwhole,numfrac);
		//#endif

		UL PLL_NUM = numfrac;
		unsigned int PLL_N = numwhole;

		UL PLL_NUM1_temp = (PLL_NUM & 0x3FF000)>>12;    //MSB 10 bits modded lower frequency (offset)
		UL PLL_NUM2_temp = (PLL_NUM & 0x000FFF);        //LSB 12 bits modded lower frequency (offset)
		unsigned int PLL_NUM1 = PLL_NUM1_temp;
		unsigned int PLL_NUM2 = PLL_NUM2_temp;

		//#ifdef debug
		//  printf("num1:%d,%x\n",PLL_NUM1,PLL_NUM1);
		//  printf("num2:%d,%x\n",PLL_NUM2,PLL_NUM2);
		//#endif


		UL reg5 = ((UL)0<<OFFSET_OUT_LDEN)+((UL)osc_freq_val<<OFFSET_OSC_FREQ)+((UL)0<<OFFSET_BUFEN_DIS)+((UL)1<<OFFSET_VCO_SEL_MODE)+(mux_vco_div<<OFFSET_OUTB_MUX)+(mux_vco_div<<OFFSET_OUTA_MUX)+(0<<OFFSET_0_DLY)+(0<<OFFSET_MODE)+(0<<OFFSET_PWDN_MODE)+(0<<OFFSET_RESET)+REG5_CONST;
		SET_PIO_REG(BoardNum, PIO_OFFSET_SYNTH_REG, reg5);
		Sleep(5);
		if (specialcase){    //page 19 of manual
			UL reg3 = ((UL)6<<OFFSET_VCO_DIV)+((UL)30<<OFFSET_OUTB_PWR)+(30<<OFFSET_OUTA_PWR)+(1<<OFFSET_OUTB_PD)+(0<<OFFSET_OUTA_PD)+REG3_CONST;
			SET_PIO_REG(BoardNum, PIO_OFFSET_SYNTH_REG, reg3);
			Sleep(5);
		}
		UL reg3 = ((UL)VCO_DIV<<OFFSET_VCO_DIV)+((UL)30<<OFFSET_OUTB_PWR)+(30<<OFFSET_OUTA_PWR)+(1<<OFFSET_OUTB_PD)+(0<<OFFSET_OUTA_PD)+REG3_CONST;
		SET_PIO_REG(BoardNum, PIO_OFFSET_SYNTH_REG, reg3);
		Sleep(5);
		UL reg1 = ((UL)CPG<<OFFSET_CPG)+((UL)VCO_CORE_SELECT<<OFFSET_VCO_SEL)+((UL)(PLL_NUM1)<<OFFSET_PLL_NUM_MSB)+(3<<OFFSET_FRAC_ORDER)+(PLL_R<<OFFSET_PLL_R)+REG1_CONST;
		SET_PIO_REG(BoardNum, PIO_OFFSET_SYNTH_REG, reg1);
		Sleep(5);
		UL reg0 = ((UL)0<<OFFSET_ID)+((UL)0<<OFFSET_FRAC_DITHER)+((UL)0<<OFFSET_NO_FCAL)+((UL)(PLL_N)<<OFFSET_PLL_N)+((UL)(PLL_NUM2)<<OFFSET_PLL_NUM_LSB)+REG0_CONST;
		SET_PIO_REG(BoardNum, PIO_OFFSET_SYNTH_REG, reg0);
		//  Sleep(20);
		//  SET_PIO_REG(BoardNum, PIO_OFFSET_SYNTH_REG, reg0);               //register 0 must be written again after wait
		Sleep(5);


		AdcReset(BoardNum);
		Sleep(1);
		AdcUnreset(BoardNum);
		SLEEP(1);

		unsigned int adcClkFrequency = AdcClockGetFreq(BoardNum);
		// printf("AD12 ADC Clock Frequency = %dMHz\n",adcClkFrequency);
		SLEEP(1);
		ADC12D2000_Calibrate_ADC(BoardNum);
		SLEEP(1);
		long tmp;
		tmp =  GET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,0,0);
		tmp &= ~(0xf << CLOCK_SELECTED_ADDR);
		tmp |=  (1 << CLOCK_SELECTED_ADDR);
		SET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,tmp,0, 0);
	}
}

unsigned int GetSetupDoneBitValue(unsigned short BoardNum)
{
	unsigned int tmp;

	tmp = GET_PIO_REG(BoardNum, PIO_OFFSET_CONTROL);
	tmp &= ( 1 << SETUP_DONE);
	tmp = (tmp >> SETUP_DONE);

	return tmp;
}

EXPORTED_FUNCTION void SetInternalClockEnable (unsigned short BoardNum, unsigned int Enable)
{    
	if (dev_config[BoardNum].config_file_found != 1){
		GetBoardConfigInfo(BoardNum);			//called to properly populate dev_config if this routine is called before setupboard
	}
	unsigned int clkSelected = (unsigned int)(GET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,0,0) >> CLOCK_SELECTED_ADDR) & 0xf;
	unsigned int clkVal = 0;  // CLOCK_EXTERNAL
	if (Enable == CLOCK_INTERNAL)
		clkVal = 1;

	if (!dev_config[BoardNum].is_AD5474 && !dev_config[BoardNum].is_ISLA216P && !dev_config[BoardNum].is_ISLA214P && !dev_config[BoardNum].is_adc12d2000){
		//printf("setting ad8 clock\n");
		SET_CONTROL_REG(BoardNum,INT_CLOCK_EN, clkVal);
	}
	if (dev_config[BoardNum].is_AD5474){
		SET_CONTROL_REG(BoardNum,INT_CLOCK_EN, clkVal);

	}
	if (dev_config[BoardNum].is_adc12d2000)
	{
		//	makeAtomic[BoardNum].lock();
		busy++;

		//        printf ("AD12 LOCKED = %d SetInternalClockEnable: Enable = %d  clkSelected = %d\n",busy,Enable,clkSelected);
		#ifdef DEBUG_TO_FILE
		FILE *debugFile;
		debugFile = fopen("c:\\uvdma\\debug.txt","a");
		long tm = GetTickCount();
		fprintf(debugFile,"LOCKED =%d @%lu SetInternalClockEnable\t\t\t%08ld\t%d\t begin\n",busy, (unsigned long)&busy,   tm  ,BoardNum);
		fprintf(debugFile,"Enable = %d\n",Enable);
		fprintf(debugFile,"clkSelected = %d\n",clkSelected);
		fflush(debugFile);
		fclose(debugFile);
		#endif

		if (Enable != clkSelected)
		{            
			SET_CONTROL_REG(BoardNum,INT_CLOCK_EN, clkVal);
			//		SLEEP(10);
			//		SET_CONTROL_REG(BoardNum,INT_CLOCK_EN, Enable);
			//		SLEEP(10);
			//		SET_CONTROL_REG(BoardNum,INT_CLOCK_EN, Enable);
			//		SLEEP(10);


			AdcReset(BoardNum);
			if (Enable == CLOCK_INTERNAL)
			{
				SLEEP(500); // allow osc to stabilize
			}
			AdcUnreset(BoardNum);
			SLEEP(1);

			unsigned int adcClkFrequency = AdcClockGetFreq(BoardNum);
			//            printf("AD12 ADC Clock Frequency = %dMHz\n",adcClkFrequency);
			SLEEP(1);
			ADC12D2000_Calibrate_ADC(BoardNum);
			SLEEP(1);
			long tmp;
			tmp =  GET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,0,0);
			tmp &= ~(0xf << CLOCK_SELECTED_ADDR);
			tmp |=  (Enable << CLOCK_SELECTED_ADDR);
			SET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,tmp,0, 0);

		}
		// Shadow reg does not seem to work?
		/*
		unsigned int val, tmp, mask;
		tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CONTROL, 0, 0);
		mask = (1 << INT_CLOCK_EN);
		val = ((Enable & 0x1) << INT_CLOCK_EN);
		tmp &= ~mask;
		tmp |= val;
		SET_SHADOW_REG(BoardNum, PIO_OFFSET_CONTROL, tmp, 0, 0);
		SetMezzPowerDown(BoardNum, !Enable);
	*/

		#ifdef DEBUG_TO_FILE
		debugFile = fopen("c:\\uvdma\\debug.txt","a");
		tm = GetTickCount();
		fprintf(debugFile,"SetInternalClockEnable\t\t\t%08ld\t%d\t end\n",tm  ,BoardNum);
		fflush(debugFile);
		fclose(debugFile);
		#endif

		//	makeAtomic[BoardNum].unlock();

	}
	if (dev_config[BoardNum].is_ISLA216P)
	{
		unsigned int adc_clock_freq, int_clk_on;
		int sleeptime = 550;

		//        printf ("AD16 board %d\tinternal clock setting = %d  prior = %d\n",BoardNum, Enable,clkSelected);
		//        fflush(stdout);

		if (clkSelected != Enable)
		{
			printf ("board %d\tinternal clock setting changed enable = %d  prior = %d\n",BoardNum, Enable,clkSelected);
			fflush(stdout);
			for (int i=0;i<MAX_ADC;i++)
			{
				SET_DLL_RANGE_REG(BoardNum, (1<<i), 0);  // high range
			}

			SET_CONTROL_REG(BoardNum,INT_CLOCK_EN, clkVal);

			if (Enable == CLOCK_INTERNAL)
			{
				SLEEP(50); // allow osc to stabilize
			}


			unsigned int adcClkFrequency = AdcClockGetFreq(BoardNum);
			printf("AD16 ADC Clock Frequency = %dMHz\n",adcClkFrequency);

			CalibrateBoard(BoardNum);

			long tmp;
			tmp =  GET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,0,0);
			tmp &= ~(0xf << CLOCK_SELECTED_ADDR);
			tmp |=  (Enable << CLOCK_SELECTED_ADDR);
			SET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,tmp,0, 0);
			/*           if ((dev_config[BoardNum].is_ISLA216P) || (dev_config[BoardNum].is_ISLA214P))
			{
				adc_clock_freq = AdcClockGetFreq(BoardNum);
				ADC_NRESET(BoardNum, 0xf);  // ONLY for ISLA devices
	//			printf("DLL clock frequency = %d\n",adc_clock_freq);
				if(adc_clock_freq == 0)
					printf("adc_clock_freq = 0");
				else
				{
					if (dev_config[BoardNum].is_ISLA216P)
						sleeptime = 550*250/adc_clock_freq;
					else
						sleeptime = 550*500/adc_clock_freq;
				}
				Sleep(sleeptime);
		//		if (dev_config[BoardNum].ISLA_2X_CLK == 1)		//temp comment 7/28
		//			SET_2X_CLOCK(BoardNum);
		//		else
		//			SET_1X_CLOCK(BoardNum);
				ADC_CLKDIVRST(BoardNum, 0xf);
				CONFIGURE_PRETRIGGER(BoardNum);
				int_clk_on = GetInternalClockValue(BoardNum);
				// Phase slip if using external clock
				if ((int_clk_on == 0) && (dev_config[BoardNum].is_ISLA214P))
					SET_PHASE_SLIP_REG(BoardNum, 0xf, 0x1);
				//ISLA_GainAdjust(BoardNum);
				// issue I2E engine start and power settings
				SET_RESERVED53_REG(BoardNum, 0xF, 0x1);
				SET_RESERVED55_REG(BoardNum, 0xF, 0x1);
				SET_RESERVED31_REG(BoardNum, 0xF, 0x21);
*/
		}
	}
}

EXPORTED_FUNCTION void SetWriteWholeRow (unsigned short BoardNum, unsigned int Enable)
{
	SET_CONTROL_REG(BoardNum,WRITE_WHOLE_ROW, Enable);


	// Shadow reg does not seem to work?
	/*	
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CONTROL, 0, 0);
	tmp = tmp & 0xfffbffff; 
	val = ((Enable & 0x1) << WRITE_WHOLE_ROW);
	tmp |= val;

	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CONTROL, tmp, 0, 0);
*/
}


void SET_NUMBLOCKS_TO_ACQUIRE (unsigned short BoardNum, unsigned int NumBlocks)
{
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_NUMBLOCKS, NumBlocks, 0, 0);
}


EXPORTED_FUNCTION void SET_BLOCKSIZE(unsigned short BoardNum, unsigned int Blocksize)
{
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_BLOCKSIZE, Blocksize, 0, 0);
}


EXPORTED_FUNCTION void SET_PCIE_RD_START_BLOCK (unsigned short BoardNum, unsigned int Offset)
{
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_PCIE_RD_START_BLOCK, Offset, 0, 0);
}

EXPORTED_FUNCTION void SET_PCIE_RD_END_BLOCK (unsigned short BoardNum, unsigned int Offset)
{
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_PCIE_RD_END_BLOCK, Offset, 0, 0);
}

EXPORTED_FUNCTION void SET_DAC_RD_START_BLOCK (unsigned short BoardNum, unsigned int Offset)
{
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_DAC_RD_START_BLOCK, Offset, 0, 0);
}

EXPORTED_FUNCTION void SET_DAC_RD_END_BLOCK (unsigned short BoardNum, unsigned int Offset)
{
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_DAC_RD_END_BLOCK, Offset, 0, 0);
}

EXPORTED_FUNCTION void SET_DAC_WR_START_BLOCK (unsigned short BoardNum, unsigned int Offset)
{
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_DAC_WR_START_BLOCK, Offset, 0, 0);
}

EXPORTED_FUNCTION void SET_DAC_WR_END_BLOCK (unsigned short BoardNum, unsigned int Offset)
{
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_DAC_WR_END_BLOCK, Offset, 0, 0);
}

void CONFIGURE_PRETRIGGER(unsigned short BoardNum)
{
	SetPreTriggerMemory(BoardNum, 0);
	SetPreTriggerMemory(BoardNum, 511);
	SetPreTriggerMemory(BoardNum, 0);
}

void SetPreTrigger (unsigned short BoardNum, unsigned int Value)
{
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_PRE_TRIGGER, Value, 0, 0);
}


EXPORTED_FUNCTION void SET_ECL_TRIGGER_DELAY (unsigned short BoardNum, unsigned int Value)
{
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_ECL_TRIGGER_DELAY, Value, 0, 0);
}


EXPORTED_FUNCTION void SET_CAPTURE_DEPTH (unsigned short BoardNum, unsigned int size)
{
	unsigned int captureLen = ConvertSegmentedCaptureLengthValue(BoardNum,size);

	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTURE_DEPTH, captureLen, 0, 0);
}


EXPORTED_FUNCTION void SET_CAPTURE_COUNT (unsigned short BoardNum, unsigned int count)
{
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTURE_COUNT, count, 0, 0);
}


//Used for driver_version >= 1.0.0
void CONFIGURE_ADCDATASETUP_REGISTER (unsigned short BoardNum)
{
	SetAdcResolution(BoardNum, dev_config[BoardNum].adc_res);
	SetAdcUser(BoardNum, 0);
	if (dev_config[BoardNum].is_adc12d2000)
	{
		SetNumAdcChannel(BoardNum, dev_config[BoardNum].adc_chan_used);
		//SetAdcDecimation(BoardNum, 1);
		SetADC12D2000NDM(BoardNum, 0);
		SetADC12D2000ECEBAR(BoardNum, 0);
	}
}

//Used for driver_version >= 1.0.0
void SetAdcResolution(unsigned short BoardNum, unsigned int adc_res)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_ADC_DATA_SETUP, 0, 0);
	tmp &= ~(0x1f << ADC_RESOLUTION_OFFSET); 
	val = ((adc_res & 0x1f) << ADC_RESOLUTION_OFFSET);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_ADC_DATA_SETUP, tmp, 0, 0);
}

//Used for driver_version >= 1.0.0
void SetNumAdcChannel(unsigned short BoardNum, unsigned int adc_chan_used)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_ADC_DATA_SETUP, 0, 0);
	tmp &= ~(0x0f << NUM_ADC_CHAN_OFFSET);
	val = ((adc_chan_used & 0x0f) << NUM_ADC_CHAN_OFFSET);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_ADC_DATA_SETUP, tmp, 0, 0);
}

//Used for driver_version >= 1.0.0
EXPORTED_FUNCTION void SetAdcDecimation(unsigned short BoardNum, unsigned int adc_deci_offset)
{
	unsigned int val, tmp;


	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_ADC_DATA_SETUP, 0, 0);
	tmp &= ~(0xf << ADC_DECI_OFFSET);
	val = ((adc_deci_offset & 0xf) << ADC_DECI_OFFSET);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_ADC_DATA_SETUP, tmp, 0, 0);
}

//Used for driver_version >= 1.0.0
void SetAdcUser(unsigned short BoardNum, unsigned int user_val)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_ADC_DATA_SETUP, 0, 0);
	tmp &= ~(0xf << ADC_USER_OFFSET);
	val = ((user_val & 0xf) << ADC_USER_OFFSET);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_ADC_DATA_SETUP, tmp, 0, 0);
}

//Used for driver_version >= 1.0.0
void SetADC12D2000NDM(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_ADC_DATA_SETUP, 0, 0);
	tmp &= ~(0x1 << AD12_ADC_NDM);
	val = ((Enable & 0x1) << AD12_ADC_NDM);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_ADC_DATA_SETUP, tmp, 0, 0);
}

//Used for driver_version >= 1.0.0
void SetADC12D2000ECEBAR(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_ADC_DATA_SETUP, 0, 0);
	tmp &= ~(0x1 << AD12_ADC_ECEBAR); 
	val = ((Enable & 0x1) << AD12_ADC_ECEBAR);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_ADC_DATA_SETUP, tmp, 0, 0);
}


void CONFIGURE_DRAMADJ_REGISTER(unsigned short BoardNum)
{
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_DRAM_ADJUST, 0, 0, 0);

	//	printf("DQS = %d\nDQ = %d\n",dev_config[BoardNum].dram_dqs,dev_config[BoardNum].dram_dq);
	DramIdelayShift(BoardNum, dev_config[BoardNum].dram_dqs, dev_config[BoardNum].dram_dq);
	SetDRAMReadTime(BoardNum, SEL_DRAM_RD_TIME);
}

void SetDRAMIDelay_DQ_CE_INC(unsigned short BoardNum, unsigned int High)
{
	unsigned int val, tmp, mask;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_DRAM_ADJUST, 0, 0);

	mask = (1 <<  DRAM_IDELAY_DQ_CE);
	mask |= (1 <<  DRAM_IDELAY_DQ_INC);

	val = ((High & 0x1) << DRAM_IDELAY_DQ_CE);
	val |= ((High & 0x1) << DRAM_IDELAY_DQ_INC);

	tmp &= ~mask;
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_DRAM_ADJUST, tmp, 0, 0);
}

void SetDRAMIDelay_DQS_CE_INC(unsigned short BoardNum, unsigned int High)
{
	unsigned int val, tmp, mask;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_DRAM_ADJUST, 0, 0);

	mask = (1 <<  DRAM_IDELAY_DQS_CE);
	mask |= (1 <<  DRAM_IDELAY_DQS_INC);

	val = ((High & 0x1) << DRAM_IDELAY_DQS_CE);
	val |= ((High & 0x1) << DRAM_IDELAY_DQS_INC);

	tmp &= ~mask;
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_DRAM_ADJUST, tmp, 0, 0);
}

void SetDRAMIDelay_DQS_DQ_RST(unsigned short BoardNum, unsigned int High)
{
	unsigned int val, tmp, mask;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_DRAM_ADJUST, 0, 0);

	mask = (1 <<  DRAM_IDELAY_DQ_RST);
	mask |= (1 <<  DRAM_IDELAY_DQS_RST);

	val = ((High & 0x1) << DRAM_IDELAY_DQS_RST);
	val |= ((High & 0x1) << DRAM_IDELAY_DQ_RST);

	tmp &= ~mask;
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_DRAM_ADJUST, tmp, 0, 0);
}

void SetDRAMReadTime(unsigned short BoardNum, unsigned int Value)
{
	unsigned int val, tmp, mask;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_DRAM_ADJUST, 0, 0);

	mask = (7 <<  DRAM_RD_TIME);

	val = ((Value & 0x7) << DRAM_RD_TIME);

	tmp &= ~mask;
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_DRAM_ADJUST, tmp, 0, 0);
}

//Used for driver_version >= 1.0.0
void CONFIGURE_CAPTUREADJ_REGISTER (unsigned short BoardNum)
{
	SetADCSingleChannelSelect(BoardNum, 0);
	SetDualADCSingleChannel(BoardNum, 0);
	SetAcquireDisableEdge(BoardNum, 0);
	SetAcquireDisableReset(BoardNum, 0);
	//SetAcquireDisableInvert(BoardNum, 0);
	SetContinuousAutoRefresh(BoardNum, dev_config[BoardNum].continuous_arf);
	SetADCDebugSync(BoardNum, 0);

	SetIdelayResetCtrlReset(BoardNum, 1);
	SetIdelayResetCtrlReset(BoardNum, 0);
}

//Used for driver_version >= 1.0.0
EXPORTED_FUNCTION void AdcReset(unsigned short BoardNum)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ~( 0x7 << ADC_HOLD_TRIGGER);
	val = ( 1 << ADC_RST_P);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, tmp, 0, 0);
	SLEEP(1);
}

//Used for driver_version >= 1.0.0
EXPORTED_FUNCTION void AdcUnreset(unsigned short BoardNum)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ~( 0x7 << ADC_HOLD_TRIGGER);
	val = ( 1 << ADC_RST_N);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, tmp, 0, 0);
	SLEEP(1);
}

//Used for driver_version >= 1.0.0
void AdcTristate(unsigned short BoardNum)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ~( 0x7 << ADC_HOLD_TRIGGER); 
	val = ( 1 << ADC_HOLD_TRIGGER);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, tmp, 0, 0);
	SLEEP(1);
}

//Used for driver_version >= 1.0.0
void SetIdelayReset(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ~( 0x1 << IDELAY_RST); 
	val = ((Enable & 0x1) << IDELAY_RST);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, tmp, 0, 0);
}

//Used for driver_version >= 1.0.0
void SetIdelayCE(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ~( 0x1 << IDELAY_CE);
	val = ((Enable & 0x1) << IDELAY_CE);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, tmp, 0, 0);
}

//Used for driver_version >= 1.0.0
void SetIdelayInc(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ~( 0x1 << IDELAY_INC);
	val = ((Enable & 0x1) << IDELAY_INC);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, tmp, 0, 0);
}

//Used for driver_version >= 1.0.0
void SetIdelayCtrlReset(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ~( 0x1 << IDELAY_CTRL_RST);
	val = ((Enable & 0x1) << IDELAY_CTRL_RST);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, tmp, 0, 0);
}

void SetIdelayResetCtrlReset(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ~( 0x1 << IDELAY_RST);
	val = ((Enable & 0x1) << IDELAY_RST);
	tmp |= val;
	tmp &= ~( 0x1 << IDELAY_CTRL_RST);
	val = ((Enable & 0x1) << IDELAY_CTRL_RST);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, tmp, 0, 0);
}
//Used for driver_version >= 1.0.0
void SetAcquireDisableEdge(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ~( 0x1 << ACQUIRE_DISABLE_EDGE);
	val = ((Enable & 0x1) << ACQUIRE_DISABLE_EDGE);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, tmp, 0, 0);
}

//Used for driver_version >= 1.0.0
void SetAcquireDisableReset(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ~( 0x1 << ACQUIRE_DISABLE_RESET);
	val = ((Enable & 0x1) << ACQUIRE_DISABLE_RESET);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, tmp, 0, 0);
}

//Used for driver_version >= 1.0.0
EXPORTED_FUNCTION void SetAcquireDisableInvert(unsigned short BoardNum, unsigned int Invert)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ~( 0x1 << ACQUIRE_DISABLE_INV);
	val = ((Invert & 0x1) << ACQUIRE_DISABLE_INV);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, tmp, 0, 0);
}

//Used for driver_version >= 1.0.0
EXPORTED_FUNCTION void SetContinuousAutoRefresh(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ~( 0x1 << CONTINUOUS_ARF); 
	val = ((Enable & 0x1) << CONTINUOUS_ARF);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, tmp, 0, 0);
}

//Used for driver_version >= 1.0.0
void SetADCDebugSync(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ~( 0x1 << ADC_DEBUG_SYNC); 
	val = ((Enable & 0x1) << ADC_DEBUG_SYNC);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, tmp, 0, 0);
}

//Used for driver_version >= 1.0.0
void SetADCSingleChannelSelect(unsigned short BoardNum, unsigned int CH1)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ~( 0x1 << SINGLE_CHAN_SELECT);
	val = ((CH1 & 0x1) << SINGLE_CHAN_SELECT);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, tmp, 0, 0);
}

//Used for driver_version >= 1.0.0
void SetMezzPowerDown(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ~( 0x1 << MEZZ_OSC_POWER_DOWN);
	val = ((Enable & 0x1) << MEZZ_OSC_POWER_DOWN);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, tmp, 0, 0);
}

//Used for driver_version >= 1.0.0
void SetADCCLKDiv(unsigned short BoardNum, unsigned int Value)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ~( 0xf << ADC_CLK_DIV);
	val = ((Value & 0xf) << ADC_CLK_DIV);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, tmp, 0, 0);
}

//Used for driver_version >= 1.0.0
void SetDualADCSingleChannel(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ~( 0x1 << DUAL_ADC_SNGL_CHAN);
	val = ((Enable & 0x1) << DUAL_ADC_SNGL_CHAN);
	tmp |= val;
	//printf("SetDualADCSingleChannel: PIO_OFFSET_CAPTUREADJ %x\n",tmp );

	//SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, tmp, 0, 0);
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 4100, 0, 0);    //this code is correct for 2 channel mode

}


//Used for driver_version >= 1.0.0
void Write_ADC12D2000(unsigned short BoardNum, unsigned int spiAddress, unsigned int spiData)
{
	unsigned int tmp, val;
	unsigned int mask_info;

	mask_info = 0;
	tmp = ( AD12 << SPI_DEVICE_BIT_POS );
	mask_info |= tmp;
	tmp = ( WORD_0 << DATA_WORD_BIT_POS );
	mask_info |= tmp;
	tmp = ( AD12_DATA_LENGTH << DATA_LENGTH_BIT_POS);
	mask_info |= tmp;

	val = makeADC12D2000_SPI(ADC12D2000_WRITE, spiAddress, spiData);
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_ADCSERIAL, val, spiAddress,  mask_info);
	SLEEP(1);
}

//Used for driver_version >= 1.0.0
unsigned int Read_ADC12D2000_shadow(unsigned short BoardNum, unsigned int spiAddress)
{
	unsigned int data0;
	unsigned int tmp, val;
	unsigned int mask_info;

	mask_info = 0;
	tmp = (AD12 << SPI_DEVICE_BIT_POS);
	mask_info |= tmp;
	tmp = (WORD_0 << DATA_WORD_BIT_POS);
	mask_info |= tmp;

	data0 = GET_SHADOW_REG(BoardNum, PIO_OFFSET_ADCSERIAL, spiAddress, mask_info);

	return data0;
}


unsigned int Read_ADC12D2000_register (unsigned short BoardNum, unsigned int spiAddress)
{
	unsigned int val;

	val = makeADC12D2000_SPI(ADC12D2000_READ, spiAddress, 0);

	SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, val);
	SLEEP(1);

	val = (GET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL)     );

	return val;
}

void setPOR_values(unsigned short BoardNum)
{
	Write_ADC12D2000(BoardNum, CONFIGURATION_REGISTER_1, 0x2000);
	Write_ADC12D2000(BoardNum, CONFIGURATION_REGISTER_2, 0x0003);
}

void ADC12D2000_INITIALIZE(unsigned short BoardNum)
{

	setPOR_values(BoardNum);
	ADC12D2000_SET_CONFIGURATION_REGISTER_1(BoardNum);
	ADC12D2000_SET_RESERVED_ADDR_0x1(BoardNum);
	//ADC12D2000_SET_I_CHANNEL_OFFSET_ADJUST;
	ADC12D2000_SET_I_CHANNEL_FULL_SCALE_RANGE_ADJUST(BoardNum, dev_config[BoardNum].adc3_fsr);
	ADC12D2000_SET_CALIBRATION_ADJUST(BoardNum);
	ADC12D2000_SET_CALIBRATION_VALUES(BoardNum, dev_config[BoardNum].ADC12D2000_CALIBRATION_VALUES);
	ADC12D2000_SET_BIAS_ADJUST(BoardNum, dev_config[BoardNum].ADC12D2000_BIAS_ADJUST);
	ADC12D2000_SET_DES_TIMING_ADJUST(BoardNum, dev_config[BoardNum].ADC12D2000_DES_TIMING_ADJUST);  // ok to here
	ADC12D2000_SET_RESERVED_ADDR_0x8(BoardNum); 
	ADC12D2000_SET_RESERVED_ADDR_0x9(BoardNum);  // good to here
	//ADC12D2000_SET_Q_CHANNEL_OFFSET_ADJUST;
	ADC12D2000_SET_Q_CHANNEL_FULL_SCALE_RANGE_ADJUST(BoardNum, dev_config[BoardNum].adc3_fsr_Q);
	ADC12D2000_SET_APERTURE_DELAY_COURSE_ADJUST(BoardNum);
	ADC12D2000_SET_APERTURE_DELAY_FINE_ADJUST(BoardNum);
	ADC12D2000_SET_CONFIGURATION_REGISTER_2(BoardNum);
	ADC12D2000_SET_RESERVED_ADDR_0xF(BoardNum);
	ADC12D2000_Set_Offsets(BoardNum, 2, 1);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SET_CONFIGURATION_REGISTER_1(unsigned short BoardNum)
{
	ADC12D2000_SetDCLKPhaseSelect(BoardNum, 0);
	ADC12D2000_SetOutputVoltageSelect(BoardNum, 1);
	ADC12D2000_SetTestPatternMode(BoardNum, 0);
	ADC12D2000_SetPowerDownIChannel(BoardNum, 0);
	ADC12D2000_SetPowerDownQChannel(BoardNum, 0);
	// consider using actual freq
	ADC12D2000_SetLowFrequencySelect(BoardNum, 0);
	// consider using channel select to get these correct
	ADC12D2000_SetDualEdgeSamplingMode(BoardNum, 0);
	ADC12D2000_SetDESQInputSelect(BoardNum, 0);
	ADC12D2000_SetDESIQInputSelect(BoardNum, 0);
	ADC12D2000_SetTwosComplimentOutput(BoardNum, 0);
	ADC12D2000_SetSingleDataRate(BoardNum, 0);

}

//Used for driver_version >= 1.0.0
void ADC12D2000_Calibrate_ADC(unsigned short BoardNum)
{
	unsigned int val;
	//    printf("calibrating AD12 adcClockFrequency=%d\n",adcClockFrequency);

	val = Read_ADC12D2000_shadow(BoardNum, CONFIGURATION_REGISTER_1);

	if ((val & ( 1 << Adc12d2000_CAL)) > 0)
	{
		// set cal bit low
		val &= ~( 1 << Adc12d2000_CAL); 
		Write_ADC12D2000(BoardNum, CONFIGURATION_REGISTER_1, val);
		SLEEP(1);
	}

	// set cal bit high
	val |= ( 1 << Adc12d2000_CAL);
	Write_ADC12D2000(BoardNum, CONFIGURATION_REGISTER_1, val);
	SLEEP(1);

	long clkSelected = (GET_SHADOW_REG(BoardNum,PIO_OFFSET_PERSISTENT_STORAGE,0,0) >> CLOCK_SELECTED_ADDR) & 0xf;
	if (adcClockFrequency <= 100)		//changed from 0 to 100 to setup faster when clock is missing
		adcClockFrequency = 100;  //this is an error
	unsigned int sleepTime = 420 * 1000 / adcClockFrequency;
	SLEEP(sleepTime);
	//	if (clkSelected == 0)
	//	{
	// external
	//		SLEEP(1370);  // worst case for 300MHz adclock
	//	}
	//	else
	//	{
	//		SLEEP(420);  // 1GHz internal adclock
	//	}
	// set cal bit low
	val &= ~( 1 << Adc12d2000_CAL); 
	Write_ADC12D2000(BoardNum, CONFIGURATION_REGISTER_1, val);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SetDCLKPhaseSelect(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, CONFIGURATION_REGISTER_1);
	tmp &= ~(0x1 << Adc12d2000_DPS); 
	val = ((Enable & 0x1) << Adc12d2000_DPS);
	tmp |= val;
	Write_ADC12D2000(BoardNum, CONFIGURATION_REGISTER_1, tmp);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SetOutputVoltageSelect(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, CONFIGURATION_REGISTER_1);
	tmp &= ~(0x1 << Adc12d2000_OVS); 
	val = ((Enable & 0x1) << Adc12d2000_OVS);
	tmp |= val;
	Write_ADC12D2000(BoardNum, CONFIGURATION_REGISTER_1, tmp);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SetTestPatternMode(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, CONFIGURATION_REGISTER_1);
	tmp &= ~(0x1 << Adc12d2000_TPM); 
	val = ((Enable & 0x1) << Adc12d2000_TPM);
	tmp |= val;
	Write_ADC12D2000(BoardNum, CONFIGURATION_REGISTER_1, tmp);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SetPowerDownIChannel(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, CONFIGURATION_REGISTER_1);
	tmp &= ~(0x1 << Adc12d2000_PDI); 
	val = ((Enable & 0x1) << Adc12d2000_PDI);
	tmp |= val;
	Write_ADC12D2000(BoardNum, CONFIGURATION_REGISTER_1, tmp);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SetPowerDownQChannel(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, CONFIGURATION_REGISTER_1);
	tmp &= ~(0x1 << Adc12d2000_PDQ); 
	val = ((Enable & 0x1) << Adc12d2000_PDQ);
	tmp |= val;
	Write_ADC12D2000(BoardNum, CONFIGURATION_REGISTER_1, tmp);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SetLowFrequencySelect(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	#ifdef DEBUG_TO_FILE
	FILE *debugFile;
	debugFile = fopen("c:\\uvdma\\debug.txt","a");
	long tm = GetTickCount();
	fprintf(debugFile,"ADC12D2000_SetLowFrequencySelect\t\t\t%08ld\t%d\t%d\n",tm  ,BoardNum, Enable);
	fflush(debugFile);
	fclose(debugFile);
	#endif

	tmp = Read_ADC12D2000_shadow(BoardNum, CONFIGURATION_REGISTER_1);
	tmp &= ~(0x1 << Adc12d2000_LFS); 
	val = ((Enable & 0x1) << Adc12d2000_LFS);
	tmp |= val;
	Write_ADC12D2000(BoardNum, CONFIGURATION_REGISTER_1, tmp);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SetDualEdgeSamplingMode(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, CONFIGURATION_REGISTER_1);
	tmp &= ~(0x1 << Adc12d2000_DES); 
	val = ((Enable & 0x1) << Adc12d2000_DES);
	tmp |= val;
	Write_ADC12D2000(BoardNum, CONFIGURATION_REGISTER_1, tmp);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SetDESQInputSelect(unsigned short BoardNum, unsigned int Q_Input)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, CONFIGURATION_REGISTER_1);
	tmp &= ~(0x1 << Adc12d2000_DEQ); 
	val = ((Q_Input & 0x1) << Adc12d2000_DEQ);
	tmp |= val;
	Write_ADC12D2000(BoardNum, CONFIGURATION_REGISTER_1, tmp);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SetDESIQInputSelect(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, CONFIGURATION_REGISTER_1);
	tmp &= ~(0x1 << Adc12d2000_DIQ); 
	val = ((Enable & 0x1) << Adc12d2000_DIQ);
	tmp |= val;
	Write_ADC12D2000(BoardNum, CONFIGURATION_REGISTER_1, tmp);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SetTwosComplimentOutput(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, CONFIGURATION_REGISTER_1);
	tmp &= ~(0x1 << Adc12d2000_2SC); 
	val = ((Enable & 0x1) << Adc12d2000_2SC);
	tmp |= val;
	Write_ADC12D2000(BoardNum, CONFIGURATION_REGISTER_1, tmp);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SetSingleDataRate(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, CONFIGURATION_REGISTER_1);
	tmp &= ~(0x1 << Adc12d2000_SDR); 
	val = ((Enable & 0x1) << Adc12d2000_SDR);
	tmp |= val;
	Write_ADC12D2000(BoardNum, CONFIGURATION_REGISTER_1, tmp);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SET_RESERVED_ADDR_0x1(unsigned short BoardNum)
{
	Write_ADC12D2000(BoardNum, RESERVED_1, 0x2907);  // correct per data sheet says ANDY
}

//Used for driver_version >= 1.0.0
EXPORTED_FUNCTION void ADC12D2000_SET_I_CHANNEL_OFFSET_ADJUST(unsigned short BoardNum, unsigned int Sign, unsigned int Offset)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, I_CHANNEL_OFFSET);
	tmp &= ~(0x1 << Adc12d2000_OS);
	val = ((Sign & 0x1) << Adc12d2000_OS);
	tmp |= val;
	tmp &= ~(0xfff << Adc12d2000_OM);
	val = ((Offset & 0xfff) << Adc12d2000_OM);
	tmp |= val;
	Write_ADC12D2000(BoardNum, I_CHANNEL_OFFSET, tmp);
}

//Used for driver_version >= 1.0.0
EXPORTED_FUNCTION void ADC12D2000_SET_I_CHANNEL_FULL_SCALE_RANGE_ADJUST(unsigned short BoardNum, unsigned int FSR)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, I_CHANNEL_FSR);
	tmp &= ~(0x7fff << Adc12d2000_FM);
	val = ((FSR & 0x7fff) << Adc12d2000_FM);
	tmp |= val;
	Write_ADC12D2000(BoardNum, I_CHANNEL_FSR, tmp);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SET_CALIBRATION_ADJUST(unsigned short BoardNum)
{
	Write_ADC12D2000(BoardNum, CALIBRATION_ADJUST, 0xdb4b);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SET_CALIBRATION_VALUES(unsigned short BoardNum, unsigned int Value)
{
	Write_ADC12D2000(BoardNum, CALIBRATION_VALUES, (Value & 0xffff));
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SET_BIAS_ADJUST(unsigned short BoardNum, unsigned int Value)
{
	Write_ADC12D2000(BoardNum, BIAS_ADJUST, (Value & 0xffff));
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SET_DES_TIMING_ADJUST(unsigned short BoardNum, unsigned int Value)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, DES_TIMING_ADJUST);
	tmp &= ~(0x7f << Adc12d2000_DTA);
	val = (0x142 | ((Value & 0x7f) << Adc12d2000_DTA));
	tmp |= val;
	Write_ADC12D2000(BoardNum, DES_TIMING_ADJUST, tmp);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SET_RESERVED_ADDR_0x8(unsigned short BoardNum)
{
	Write_ADC12D2000(BoardNum, RESERVED_8, 0x0f0f);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SET_RESERVED_ADDR_0x9(unsigned short BoardNum)
{
	Write_ADC12D2000(BoardNum, RESERVED_9, 0x0000);
}

//Used for driver_version >= 1.0.0
EXPORTED_FUNCTION void ADC12D2000_SET_Q_CHANNEL_OFFSET_ADJUST(unsigned short BoardNum, unsigned int Sign, unsigned int Offset)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, Q_CHANNEL_OFFSET);
	tmp &= ~(0x1 << Adc12d2000_OS);
	val = ((Sign & 0x1) << Adc12d2000_OS);
	tmp |= val;
	tmp &= ~(0xfff << Adc12d2000_OM);
	val = ((Offset & 0xfff) << Adc12d2000_OM);
	tmp |= val;
	Write_ADC12D2000(BoardNum, Q_CHANNEL_OFFSET, tmp);
}

//Used for driver_version >= 1.0.0
EXPORTED_FUNCTION void ADC12D2000_SET_Q_CHANNEL_FULL_SCALE_RANGE_ADJUST(unsigned short BoardNum, unsigned int FSR)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, Q_CHANNEL_FSR);
	tmp &= ~(0x7fff << Adc12d2000_FM);
	val = ((FSR & 0x7fff) << Adc12d2000_FM);
	tmp |= val;
	Write_ADC12D2000(BoardNum, Q_CHANNEL_FSR, tmp);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SET_APERTURE_DELAY_COURSE_ADJUST(unsigned short BoardNum)
{
	ADC12D2000_SetApertureDelayCoarseAdjustEnable(BoardNum, 0);
	ADC12D2000_SetApertureDelayCoarseAdjust(BoardNum, dev_config[BoardNum].ADC12D2000_APERTURE_DELAY_COARSE);
	ADC12D2000_SetDutyCycleCorrect(BoardNum, 1);

}

void ADC12D2000_SetApertureDelayCoarseAdjust(unsigned short BoardNum, unsigned int Value)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, APERTURE_DELAY_COARSE);
	tmp &= ~(0xfff << Adc12d2000_CAM);
	val = ((Value & 0xfff) << Adc12d2000_CAM);
	tmp |= val;
	Write_ADC12D2000(BoardNum, APERTURE_DELAY_COARSE, tmp);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SetApertureDelayCoarseAdjustEnable(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, APERTURE_DELAY_COARSE);
	tmp &= ~(0x1 << Adc12d2000_STA); 
	val = ((Enable & 0x1) << Adc12d2000_STA);
	tmp |= val;
	Write_ADC12D2000(BoardNum, APERTURE_DELAY_COARSE, tmp);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SetDutyCycleCorrect(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, APERTURE_DELAY_COARSE);
	tmp &= ~(0x1 << Adc12d2000_DCC); 
	val = ((Enable & 0x1) << Adc12d2000_DCC);
	tmp |= val;
	Write_ADC12D2000(BoardNum, APERTURE_DELAY_COARSE, tmp);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SET_APERTURE_DELAY_FINE_ADJUST(unsigned short BoardNum)
{
	ADC12D2000_SetApertureDelayFineAdjustEnable(BoardNum, 0);
	ADC12D2000_SetApertureDelayFineAdjust(BoardNum, dev_config[BoardNum].ADC12D2000_APERTURE_DELAY_FINE);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SetApertureDelayFineAdjust(unsigned short BoardNum, unsigned int Value)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, APERTURE_DELAY_FINE);
	tmp &= ~(0x3f << Adc12d2000_FAM); 
	val = ((Value & 0x3f) << Adc12d2000_FAM);
	tmp |= val;
	Write_ADC12D2000(BoardNum, APERTURE_DELAY_FINE, tmp);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SetApertureDelayFineAdjustEnable(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, APERTURE_DELAY_FINE);
	tmp &= ~(0x1 << Adc12d2000_SA); 
	val = ((Enable & 0x1) << Adc12d2000_SA);
	tmp |= val;
	Write_ADC12D2000(BoardNum, APERTURE_DELAY_FINE, tmp);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SET_CONFIGURATION_REGISTER_2(unsigned short BoardNum)
{
	ADC12D2000_SetDESIQCLKMode(BoardNum, 0);
	ADC12D2000_SetDisableReset(BoardNum, 0);
	ADC12D2000_Set_CFR2_Constants(BoardNum);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SetDESIQCLKMode(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, CONFIGURATION_REGISTER_2);
	tmp &= ~(0x1 << Adc12d2000_DCK); 
	val = ((Enable & 0x1) << Adc12d2000_DCK);
	tmp |= val;
	Write_ADC12D2000(BoardNum, CONFIGURATION_REGISTER_2, tmp);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SetDisableReset(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, CONFIGURATION_REGISTER_2);
	tmp &= ~(0x1 << Adc12d2000_DR);
	val = ((Enable & 0x1) << Adc12d2000_DR);
	tmp |= val;
	Write_ADC12D2000(BoardNum, CONFIGURATION_REGISTER_2, tmp);
}

void ADC12D2000_Set_CFR2_Constants(unsigned short BoardNum)
{
	unsigned int val, tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, CONFIGURATION_REGISTER_2);
	tmp &= ~(0x1 << 1);
	val = 0x0002;
	tmp |= val;
	Write_ADC12D2000(BoardNum, CONFIGURATION_REGISTER_2, tmp);
}

//Used for driver_version >= 1.0.0
void ADC12D2000_SET_RESERVED_ADDR_0xF(unsigned short BoardNum)
{
	Write_ADC12D2000(BoardNum, RESERVED_F, 0x001e);
}


/**
 * LABVIEW is really 2 programs.  setup acquire runs in the second program witout access to the global variables the other DLL functions have.
 */
EXPORTED_FUNCTION void setupAcquire(unsigned short BoardNum,unsigned int NumBlocks )
{
	//	makeAtomic[BoardNum].lock();
	busy++;

	#ifdef DEBUG_TO_FILE
	FILE *debugFile;
	debugFile = fopen("c:\\uvdma\\debug.txt","a");
	long tm = GetTickCount();
	fprintf(debugFile,"LOCKED = %d @%lu CDllDriverApi::setupAcquire\t\t\t%08ld\t%d\t begin\n",busy,(unsigned long)&busy, tm  ,BoardNum);
	fflush(debugFile);
	fclose(debugFile);
	#endif
	// if a large acquire has been setup, it is possible that it was interrupted.  It needs to be stopped


	#ifdef _WINDOWS
	if (GetNumBlocksToAcquireValue(BoardNum) > 2)
	{
		//		SET_CONTROL_REG(BoardNum,SOFTWARE_RUN_BIT, true);  //MI changed 02May2014
		SET_CONTROL_REG(BoardNum,SOFTWARE_STOP, true);
		SET_CONTROL_REG(BoardNum,SOFTWARE_RUN_BIT, false);  //MI changed 02May2014
		//		SET_CONTROL_REG(BoardNum,SOFTWARE_STOP,false);	
		SLEEP(10);
	}
	/*
	if ((adcClockFrequency > 1200) && (NumBlocks > 2))
	{
		SET_CONTROL_REG(BoardNum,READ_WHOLE_ROW, 1);
		SET_CONTROL_REG(BoardNum,WRITE_WHOLE_ROW, 1);
		SET_CONTROL_REG(BoardNum,INTERLEAVED_RW, 0);
	}
	else
	{
		SET_CONTROL_REG(BoardNum,READ_WHOLE_ROW, 0);
		SET_CONTROL_REG(BoardNum,WRITE_WHOLE_ROW, 0);
		SET_CONTROL_REG(BoardNum,INTERLEAVED_RW, 1);
	}
	*/


	//	SLEEP(1);
	//printf("setupAcquire(boardnum = %d, numBlocks = %d)\n",BoardNum,NumBlocks);
	SET_PCIE_RD_START_BLOCK (BoardNum, 0);
	SET_NUMBLOCKS_TO_ACQUIRE (BoardNum, NumBlocks);
	SET_CONTROL_REG(BoardNum,SOFTWARE_STOP,false);	
	#else
	SET_PCIE_RD_START_BLOCK (BoardNum, 0);
	SET_NUMBLOCKS_TO_ACQUIRE (BoardNum, NumBlocks);
	SET_CONTROL_REG(BoardNum,SOFTWARE_STOP,false);
	#endif

	// TODO figure out if this sleep is necessary
	//if (GetNumBlocksToAcquireValue(BoardNum) > 2)
		//SLEEP(50);




	//	SLEEP(100);

	#ifdef DEBUG_TO_FILE
	debugFile = fopen("c:\\uvdma\\debug.txt","a");
	tm = GetTickCount();
	fprintf(debugFile,"CDllDriverApi::setupAcquire\t\t\t%08ld\t%d\t end\n",tm  ,BoardNum);
	fflush(debugFile);
	fclose(debugFile);
	#endif
	//	makeAtomic[BoardNum].unlock();

}

EXPORTED_FUNCTION unsigned int getNumChannels(unsigned short BoardNum)
{
	//	unsigned int channels, is_onechannelmode;

	//	is_onechannelmode = ADC12D2000_GetDualEdgeSamplingMode(BoardNum);

	//	switch (is_onechannelmode)
	//	{
	//	case 0:
	//		channels = 2;
	//		break;
	//	case 1:
	//		channels = 1;
	//		break;
	//	}

	//	return channels;
	return 	dev_config[BoardNum].adc_chan_used;

}

EXPORTED_FUNCTION unsigned int getAllChannels(unsigned short BoardNum)
{
	if (dev_config[BoardNum].config_file_found == 1){
		return dev_config[BoardNum].adc_chan;
	}
	else{
		return 0;
	}
}

EXPORTED_FUNCTION unsigned int ADC12D2000_GetDualEdgeSamplingMode(unsigned short BoardNum)
{
	unsigned int tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, CONFIGURATION_REGISTER_1);
	tmp &= (0x1 << Adc12d2000_DES);
	tmp = (tmp >> Adc12d2000_DES);

	return tmp;
}

/*
#define DESCLKIQ_MODE			3
#define DESIQ_MODE				2
#define DESQ					1
#define DESI					0
*/
EXPORTED_FUNCTION unsigned int ADC12D2000_GetOneChanModeValue(unsigned short BoardNum)
{
	unsigned int tmp, is_DESQ, is_DESIQ, is_DESCLKIQ;

	is_DESQ = ADC12D2000_GetDESQInput(BoardNum);
	is_DESIQ = ADC12D2000_GetDESIQInput(BoardNum);
	is_DESCLKIQ = ADC12D2000_GetDESCLKIQInput(BoardNum);

	if (is_DESCLKIQ == 1)
		return 3;
	else if (is_DESIQ == 1)
		return 2;
	else if (is_DESQ == 1)
		return 1;
	else
		return 0;
}


#define DES_MODE_BITS 5
#define DES_Q_MODE 6
#define DES_IQ_MODE 5

unsigned int ADC12D2000_GetDESQInput(unsigned short BoardNum)
{
	unsigned int tmp;
	unsigned int is_DES_Q_MODE = 0;
	unsigned int isDES_CLKIQ_MODE = ADC12D2000_GetDESCLKIQInput(BoardNum);

	if (isDES_CLKIQ_MODE == 0)
	{
		tmp = Read_ADC12D2000_shadow(BoardNum, CONFIGURATION_REGISTER_1);
		tmp = (tmp >> DES_MODE_BITS);
		tmp &= 7;
		if (tmp == DES_Q_MODE)
			is_DES_Q_MODE = 1;
	}

	return is_DES_Q_MODE;
}

unsigned int ADC12D2000_GetDESIQInput(unsigned short BoardNum)
{
	unsigned int tmp;
	unsigned int is_DES_IQ_MODE = 0;
	unsigned int isDES_CLKIQ_MODE = ADC12D2000_GetDESCLKIQInput(BoardNum);

	if (isDES_CLKIQ_MODE == 0)
	{
		tmp = Read_ADC12D2000_shadow(BoardNum, CONFIGURATION_REGISTER_1);
		tmp = (tmp >> DES_MODE_BITS);
		tmp &= 7;
		if (tmp == DES_IQ_MODE)
			is_DES_IQ_MODE = 1;
	}

	return is_DES_IQ_MODE;
}

unsigned int ADC12D2000_GetDESCLKIQInput(unsigned short BoardNum)
{
	unsigned int tmp;

	tmp = Read_ADC12D2000_shadow(BoardNum, CONFIGURATION_REGISTER_2);
	tmp &= (0x1 << Adc12d2000_DCK);
	tmp = (tmp >> Adc12d2000_DCK);

	return tmp;
}


EXPORTED_FUNCTION unsigned int GetEclTriggerDelayValue(unsigned short BoardNum)
{
	return GET_SHADOW_REG(BoardNum, PIO_OFFSET_ECL_TRIGGER_DELAY, 0, 0);
}

EXPORTED_FUNCTION unsigned int GetCaptureCountValue(unsigned short BoardNum)
{
	return GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTURE_COUNT, 0, 0);
}

EXPORTED_FUNCTION unsigned int GetCaptureDepthValue(unsigned short BoardNum)
{
	unsigned int val = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTURE_DEPTH, 0, 0);
	return UnconvertSegmentedCaptureLengthValue(BoardNum,val);
}

EXPORTED_FUNCTION unsigned int GetECLTriggerEnableValue(unsigned short BoardNum)
{
	unsigned int tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ( 1 << ADC_HOLD_TRIGGER); 
	tmp = ( tmp >> ADC_HOLD_TRIGGER); 

	return tmp;
}


void SetSyncSelectiveRecording(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ~( 0x1 << SYNC_SEL_RECORD);
	val = ((Enable & 0x1) << SYNC_SEL_RECORD);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, tmp, 0, 0);
}

EXPORTED_FUNCTION void SelectTrigger(unsigned short BoardNum, unsigned int TriggerType, unsigned int TriggerSlope, unsigned int TriggerCh)
{

	unsigned int tmp, val, AnalogDigitalBit, TriggerSelect;

	// When NO_TRIGGER is selected, disable Averager and Segmented Capture.
	// When a trigger is selected, evaluate if Averager or Segmented Capture was last used. If one was last used, re-enable those settings.
	// If none were used, do nothing.
	switch (TriggerType)
	{
		case NO_TRIGGER:
			AnalogDigitalBit = 1;
			TriggerSelect = 0;
			SetAcquireDisableEdge(BoardNum, 0);		// In order to include TTL trigger edge, must manually call function.
			SetAcquireDisableInvert(BoardNum, 0);	// Must return back to active LOW for free-run
			SetSyncSelectiveRecording(BoardNum, 0);

			// If NO_TRIGGER is selected, disable Segmented Capture and Averager while retaining values in the shadow register.
			SetSegmentedCapture_NonShadow(BoardNum, 0, 0);
			SetAverager_NonShadow(BoardNum, 0, 4096);
			break;
		case WAVEFORM_TRIGGER:
			AnalogDigitalBit = 3;
			TriggerSelect = TriggerCh;
			SetAcquireDisableEdge(BoardNum, 1);
			SetSyncSelectiveRecording(BoardNum, 0);
			SetAcquireDisableInvert(BoardNum, TriggerSlope);

			// Check to see if the Averager or Segmented Capture was previously used. If so, re-eanble those values by reading from the shadow registers. If not, do nothing.
			SetAveragerOrSegmentedCapture(BoardNum);
			break;
		case SYNC_SELECTIVE_RECORDING:
			AnalogDigitalBit = 0;
			TriggerSelect = 0;
			SetAcquireDisableEdge(BoardNum, 0);
			SetSyncSelectiveRecording(BoardNum, 1);
			SetAcquireDisableInvert(BoardNum, TriggerSlope);

			// If SYNC_SELECTIVE_RECORDING is selected, disable Segmented Capture and Averager while retaining values in the shadow register. If not, do nothing.
			SetSegmentedCapture_NonShadow(BoardNum, 0, 0);
			SetAverager_NonShadow(BoardNum, 0, 4096);
			break;
		case HETERODYNE:
			AnalogDigitalBit = 0;
			TriggerSelect = 1;
			SetAcquireDisableEdge(BoardNum, 1);
			SetSyncSelectiveRecording(BoardNum, 0);
			SetAcquireDisableInvert(BoardNum, TriggerSlope);

			// Check to see if the Averager or Segmented Capture was previously used. If so, re-eanble those values by reading from the shadow registers. If not, do nothing.
			SetAveragerOrSegmentedCapture(BoardNum);
			break;
		case TTL_TRIGGER_EDGE:
			AnalogDigitalBit = 0;
			TriggerSelect = 0;
			SetAcquireDisableEdge(BoardNum, 1);
			SetSyncSelectiveRecording(BoardNum, 0);
			SetAcquireDisableInvert(BoardNum, TriggerSlope);

			// If the TTL_TRIGGER_EDGE is selected to start acquisition, do not use with Averager or Segmented Capture
			SetSegmentedCapture_NonShadow(BoardNum, 0, 0);
			SetAverager_NonShadow(BoardNum, 0, 0);
			break;
		default:	// NO_TRIGGER
			AnalogDigitalBit = 0;
			TriggerSelect = 0;
			SetAcquireDisableEdge(BoardNum, 0);
			SetSyncSelectiveRecording(BoardNum, 0);
			SetAcquireDisableInvert(BoardNum, 0);

			// If NO_TRIGGER is selected, disable Segmented Capture and Averager while retaining values in the shadow register.
			SetSegmentedCapture_NonShadow(BoardNum, 0, 0);
			SetAverager_NonShadow(BoardNum, 0, 0);
			break;
	}

	unsigned int shadow = tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_PRE_TRIGGER, 0, 0);
	tmp &= ~(0x3 << TRIGGER_MODE);
	val = ((TriggerSelect & 0x3) << TRIGGER_MODE);
	tmp |= val;
	tmp &= ~(0x3 << ANALOG_DIGITAL_BIT);
	val = ((AnalogDigitalBit & 0x3) << ANALOG_DIGITAL_BIT);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_PRE_TRIGGER, tmp, 0, 0);
}

// Trigger Types
//#define NO_TRIGGER					0
//#define WAVEFORM_TRIGGER				1
//#define SYNC_SELECTIVE_RECORDING		2
//#define HETERODYNE					3
//#define TTL_TRIGGER_EDGE				4

EXPORTED_FUNCTION unsigned int IsTriggerEnabled(unsigned short BoardNum)
{
	unsigned int analogdigitalbit_regval, triggerselect_regval, acquiredisableedge_regval, syncselrecord_regval;

	analogdigitalbit_regval = GetAnalogDigitalBitValue(BoardNum);
	triggerselect_regval = GetTriggerSelectValue(BoardNum);
	acquiredisableedge_regval = GetAcquireDisableEdgeValue(BoardNum);
	syncselrecord_regval = GetSyncSelectiveRecordingValue(BoardNum);
	//	printf("IsTriggerEnabled: %d\t%d\t%d\t%d\n",analogdigitalbit_regval,triggerselect_regval,acquiredisableedge_regval,syncselrecord_regval);
	//	fflush(stdout);
	if ( (analogdigitalbit_regval == 1) && (acquiredisableedge_regval == 0) && (syncselrecord_regval == 0) ) // NO_TRIGGER
		return 0;
	else if ( (analogdigitalbit_regval == 3) && (acquiredisableedge_regval == 1) ) // WAVEFORM_TRIGGER
		return 1;
	else if ( (analogdigitalbit_regval == 0) && (triggerselect_regval == 0) && (syncselrecord_regval == 1) && (acquiredisableedge_regval == 1)) // SYNC_SELECTIVE_RECORDING
		return 2;
	else if ( (analogdigitalbit_regval == 0) && (triggerselect_regval == 1) && (syncselrecord_regval == 0) && (acquiredisableedge_regval == 1)) // HETERODYNE
		return 3;
	else if ( (analogdigitalbit_regval == 0) && (triggerselect_regval == 0) && (syncselrecord_regval == 0) && (acquiredisableedge_regval == 1) ) // TTL_TRIGGER_EDGE
		return 4;
	else
		return 5;
}

unsigned int GetAnalogDigitalBitValue(unsigned short BoardNum)
{
	unsigned int tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_PRE_TRIGGER, 0, 0);
	tmp &= (0x3 << ANALOG_DIGITAL_BIT);
	tmp = (tmp >> ANALOG_DIGITAL_BIT);

	return tmp;
}

unsigned int GetTriggerSelectValue(unsigned short BoardNum)
{
	unsigned int tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_PRE_TRIGGER, 0, 0);
	tmp &= (0x3 << TRIGGER_MODE);
	tmp = (tmp >> TRIGGER_MODE);

	return tmp;
}

// Check to see if the Averager or Segmented Capture was previously used. If so, re-eanble those values by reading from the shadow registers.
// Only one can be on at a time.
void SetAveragerOrSegmentedCapture(unsigned short BoardNum)
{
	unsigned int tmp, val, AnalogDigitalBit, TriggerSelect, capturecount_regval, capturedepth_regval, numaverages_regval, averagerlength_regval;

	// Check to see if Averager or Segmented Capture was last used by reading the last value set by the shadow register
	capturedepth_regval = GetCaptureDepthValue(BoardNum);
	numaverages_regval = GetNumAveragesValue(BoardNum);

	// Read these values to be stored if necessary
	capturecount_regval = GetCaptureCountValue(BoardNum);
	averagerlength_regval = GetAveragerLengthValue(BoardNum);

	// Segmented Capture was last used. Re-enable with same values.
	if (capturecount_regval != 0)
		ConfigureSegmentedCapture(BoardNum, capturecount_regval, capturedepth_regval, 1);

	// Segmented Capture was last used. Re-enable with same values.
	if (numaverages_regval != 0)
		ConfigureAverager(BoardNum, numaverages_regval, averagerlength_regval, 1);
}

EXPORTED_FUNCTION void ConfigureWaveformTrigger(unsigned short BoardNum, unsigned int Threshold, unsigned int Hysteresis)
{
	//    printf("ConfigureWaveformTrigger: %d\t%d\n",Threshold,Hysteresis);
	unsigned int max_value;

	// Identify the maximum amplitude value
	switch(dev_config[BoardNum].adc_res)
	{
		case 16:
			max_value = 65535;
			break;
		case 14:
			max_value = 16383;
			break;
		case 12:
			max_value = 4095;
			break;
	}

	// Make Hysteresis an offset of Threshold. In most cases, Threshold only needs to be changed.
	if ( (Threshold+Hysteresis) > max_value)
		Hysteresis = max_value;
	else
		Hysteresis = Threshold+Hysteresis;

	// Waveform trigger depends on these and should be set at the same time
	SetWaveformTriggerThreshold(BoardNum, Threshold);
	SetWaveformTriggerHysteresis(BoardNum, Hysteresis);
}

void SetWaveformTriggerThreshold(unsigned short BoardNum, unsigned int Threshold)
{
	unsigned int tmp, val;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_TRIGGER_THRESHOLD, 0, 0);
	tmp &= ~(0xffff << THRESH_A);
	val = ((Threshold & 0xffff) << THRESH_A);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_TRIGGER_THRESHOLD, tmp, 0, 0);
}

void SetWaveformTriggerHysteresis(unsigned short BoardNum, unsigned int Hysteresis)
{
	unsigned int tmp, val;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_TRIGGER_THRESHOLD, 0, 0);
	tmp &= ~(0xffff << THRESH_B);
	val = ((Hysteresis & 0xffff) << THRESH_B);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_TRIGGER_THRESHOLD, tmp, 0, 0);
}

EXPORTED_FUNCTION unsigned int ConfigureSegmentedCapture(unsigned short BoardNum, unsigned int CaptureCount, unsigned int CaptureDepth, unsigned int ClearAverager)
{
	unsigned int tmp;
	// Segmented capture depends on the sample length "size", and the number of times, "count", to capture "size" length of samplesfrom a trigger
	SET_CAPTURE_DEPTH(BoardNum, CaptureDepth);
	SET_CAPTURE_COUNT(BoardNum, CaptureCount);

	// The Averager and Segmented capture cannot operate at the same time. When ConfigureSegmentedCapture is called, set NumAveragers and AveragerLength to zero (off).
	if (ClearAverager == 1)
		ConfigureAverager(BoardNum, 0, 0, 0);

	tmp = IsTriggerEnabled(BoardNum);

	if ( (tmp == 0) && (CaptureCount !=0) )
	{
		printf("ERROR: Segmented Capture requires a trigger source to be present. Use SelectTrigger to select a trigger. Defaulting no Segmented Capture.\n");
		printf("CaptureCount and CaptureDepth will be stored in shadow registers and will be re-enabled when SelectTrigger is called.\n");
		SetSegmentedCapture_NonShadow(BoardNum, 0, 0);
	}

	return tmp;
}

EXPORTED_FUNCTION unsigned int ConfigureAverager(unsigned short BoardNum, unsigned int NumAverages, unsigned int AveragerLength, unsigned int ClearSegmentedCapture)
{
	if (is_adc12d2000(BoardNum)){
		NumAverages-=1;         //if remigrating to windows this will need to be adjusted in user-space
	}
	unsigned int tmp, val;
	// Sets the number of averages (accumulations), and the sample length of each average (accumulation)

	// The Averager and Segmented capture cannot operate at the same time. When ConfigureAverager is called, set CaptureCount and CaptureDepth to zero (off).

	val = IsTriggerEnabled(BoardNum);
	if (NumAverages < 1)
	{
		tmp = SET_AVERAGER_LENGTH(BoardNum, AveragerLength);
		if (is_adc12d2000(BoardNum)){
			ConfigureSegmentedCapture(BoardNum, 0 , 0, 0);
		}
	}
	else
	{
		tmp = SET_AVERAGER_LENGTH(BoardNum, AveragerLength);
		if (is_adc12d2000(BoardNum)){
			ConfigureSegmentedCapture(BoardNum, 0 , AveragerLength, 0);
		}
	}
	SET_NUM_AVERAGES(BoardNum, 1);
	SET_NUM_AVERAGES(BoardNum, NumAverages);

	if ( (val == 0) && (NumAverages != 0) )
	{
		printf("ERROR: Averager requires a trigger source to be present. Use SelectTrigger to select a trigger. Defaulting no Segmented Capture.\n");
		printf("NumAverages and AveragerLength will be stored in shadow registers and will be re-enabled when SelectTrigger is called.\n");
		SetAverager_NonShadow(BoardNum, 0, 0);
	}

	return tmp;
}

EXPORTED_FUNCTION void SetFiducialMarks(unsigned short BoardNum, unsigned int Enable)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ~( 0x1 << FIDUCIAL);
	val = ((Enable & 0x1) << FIDUCIAL);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, tmp, 0, 0);
}

void SetSegmentedCapture_NonShadow(unsigned short BoardNum, unsigned int CaptureCount, unsigned int CaptureDepth)
{
	// Segmented capture depends on the sample length "size", and the number of times, "count", to capture "size" length of samplesfrom a trigger
	SetCaptureDepth_NonShadow(BoardNum, CaptureDepth);
	SetCaptureCount_NonShadow(BoardNum, CaptureCount);
}

void SetAverager_NonShadow(unsigned short BoardNum, unsigned int NumAverages, unsigned int AveragerLength)
{
	// Segmented capture depends on the sample length "size", and the number of times, "count", to capture "size" length of samplesfrom a trigger
	SetNumAverages_NonShadow(BoardNum, NumAverages);
	SetAveragerLength_NonShadow(BoardNum, AveragerLength);
}

EXPORTED_FUNCTION unsigned int getFPGAType(unsigned short BoardNum)
{
	return 	dev_config[BoardNum].fpga_type;
}

EXPORTED_FUNCTION unsigned int GetDecimationValue(unsigned short BoardNum)
{
	unsigned int tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_ADC_DATA_SETUP, 0, 0);
	tmp &= (0xf << ADC_DECI_OFFSET);
	tmp = (tmp >> ADC_DECI_OFFSET);

	return tmp;
}

EXPORTED_FUNCTION unsigned int GetPretriggerValue(unsigned short BoardNum)
{
	unsigned int tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_PRE_TRIGGER, 0, 0);
	tmp &= (0x7ff << PRE_TRIGGER);
	tmp = (tmp >> PRE_TRIGGER);

	switch(dev_config[BoardNum].pre_trigger_len)
	{
		case 512:
			break;
		case 4096:
			tmp *= 2;
			break;
		default:
			break;
	}
	if ((dev_config[BoardNum].is_ISLA214P) || (dev_config[BoardNum].is_AD5474))
	{
		tmp *= 2;  // 14 bit adcs send data in 2 streams in parallel effectively doubling pretrigger memory
	}

	return tmp;
}

EXPORTED_FUNCTION unsigned int GetInternalClockValue(unsigned short BoardNum)
{
	unsigned int tmp;

	tmp = GET_PIO_REG(BoardNum, PIO_OFFSET_CONTROL);
	tmp &= ( 1 << INT_CLOCK_EN);
	tmp = (tmp >> INT_CLOCK_EN);

	return tmp;
}

EXPORTED_FUNCTION unsigned int GetAcquireDisableEdgeValue(unsigned short BoardNum)
{
	unsigned int tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ( 0x1 << ACQUIRE_DISABLE_EDGE);
	tmp = ( tmp >> ACQUIRE_DISABLE_EDGE);

	return tmp;
}

EXPORTED_FUNCTION unsigned int GetAcquireDisableInvertValue(unsigned short BoardNum)
{
	unsigned int tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ( 0x1 << ACQUIRE_DISABLE_INV);
	tmp = ( tmp >> ACQUIRE_DISABLE_INV);

	return tmp;
}

EXPORTED_FUNCTION unsigned int GetChannelSelectValue(unsigned short BoardNum)
{
	unsigned int tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_ADC_DATA_SETUP, 0, 0);
	tmp &= ( 0xf << NUM_ADC_CHAN_OFFSET);
	tmp = ( tmp >> NUM_ADC_CHAN_OFFSET);

	return tmp;
}


EXPORTED_FUNCTION unsigned int GetSyncSelectiveRecordingValue(unsigned short BoardNum)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ( 0x1 << SYNC_SEL_RECORD);
	tmp = ( tmp >> SYNC_SEL_RECORD);

	return tmp;
}

EXPORTED_FUNCTION unsigned int GetDRAMDQSValue(unsigned short BoardNum)
{
	return dev_config[BoardNum].dram_dqs;
}

EXPORTED_FUNCTION unsigned int GetDRAMDQValue(unsigned short BoardNum)
{
	return dev_config[BoardNum].dram_dq;
}

EXPORTED_FUNCTION unsigned int GetNumBlocksToAcquireValue(unsigned short BoardNum)
{
	return GET_SHADOW_REG(BoardNum, PIO_OFFSET_NUMBLOCKS, 0, 0);
}

EXPORTED_FUNCTION unsigned int Is_ISLA214P(unsigned short BoardNum)
{
	if (dev_config[BoardNum].is_ISLA214P)
		return 1;
	else
		return 0;
}

EXPORTED_FUNCTION unsigned int GetOnBoardMemorySize(unsigned short BoardNum)
{
	unsigned int readval;
	readval = GET_PIO_REG(BoardNum, PIO_OFFSET_PROMPROG);

	// Bit 8-> 4GB four rank RDIMMs, Bit 10-> 4GB dual rank UDIMMs
	if((readval & (1<<8)) || (readval & (1<<10)))
	{
		return 8192;
	}
	else
	{
		return 4096;
	}
}

EXPORTED_FUNCTION unsigned int ConvertAveragerLengthValue(unsigned short BoardNum, unsigned int AveragerLength)
{
	//printf("AveragerLength = %d\n", AveragerLength);
	//printf("fpga_type = %d\n", dev_config[BoardNum].fpga_type);
	//printf("adc_chan_used = %d\n", dev_config[BoardNum].adc_chan_used);
	unsigned int max_averager_length = 32768;

	switch (dev_config[BoardNum].fpga_type)
	{
		case 50:
			max_averager_length = 32768;
			break;
		case 110:
			max_averager_length = 65536;
			break;
		case 155:
			max_averager_length = 262144;
			break;
	}
	if (dev_config[BoardNum].is_ISLA216P)
		max_averager_length /= 2;

	switch (dev_config[BoardNum].adc_chan_used)
	{
		case 1:
			if (AveragerLength <= max_averager_length)
			{
				return (AveragerLength/16 - 1);
			}
			else
			{
				return (max_averager_length/16 - 1);
			}
			break;
		case 2:
			if (AveragerLength <= max_averager_length/2)
			{
				return (AveragerLength/8 - 1);
			}
			else
			{
				return (max_averager_length/16 - 1);
			}
			break;
		case 4:
			if (AveragerLength <= max_averager_length/4)
			{
				return (AveragerLength/4 - 1);
			}
			else
			{
				return (max_averager_length/16 - 1);
			}
			break;
	}
}

EXPORTED_FUNCTION unsigned int UnconvertAveragerLengthValue(unsigned short BoardNum, unsigned int Value)
{
	unsigned int multiplier = 16;
	multiplier /= dev_config[BoardNum].adc_chan_used;

	Value += 1;
	Value *= multiplier;

	return Value;
}

EXPORTED_FUNCTION unsigned int ConvertSegmentedCaptureLengthValue(unsigned short BoardNum, unsigned int AveragerLength)
{
	//printf("AveragerLength = %d\n", AveragerLength);
	//printf("fpga_type = %d\n", dev_config[BoardNum].fpga_type);
	//printf("adc_chan_used = %d\n", dev_config[BoardNum].adc_chan_used);
	unsigned int max_averager_length = 32768;

	switch (dev_config[BoardNum].fpga_type)
	{
		case 50:
			max_averager_length = 32768;
			break;
		case 110:
			max_averager_length = 65536;
			break;
		case 155:
			max_averager_length = 262144;
			break;
	}
	if (dev_config[BoardNum].is_ISLA216P)
		max_averager_length /= 2;

	switch (dev_config[BoardNum].adc_chan_used)
	{
		case 1:
			if (AveragerLength <= max_averager_length)
			{
				return (AveragerLength/16);
			}
			else
			{
				return (max_averager_length/16);
			}
			break;
		case 2:
			if (AveragerLength <= max_averager_length/2)
			{
				return (AveragerLength/8);
			}
			else
			{
				return (max_averager_length/16);
			}
			break;
		case 4:
			if (AveragerLength <= max_averager_length/4)
			{
				return (AveragerLength/4);
			}
			else
			{
				return (max_averager_length/16);
			}
			break;
	}
}

EXPORTED_FUNCTION unsigned int UnconvertSegmentedCaptureLengthValue(unsigned short BoardNum, unsigned int Value)
{
	unsigned int multiplier = 16;
	multiplier /= dev_config[BoardNum].adc_chan_used;

	Value *= multiplier;

	return Value;
}

EXPORTED_FUNCTION unsigned int GetWaveformThresholdValue(unsigned short BoardNum)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_TRIGGER_THRESHOLD, 0, 0);
	tmp &= ( 0xffff << THRESH_A);
	tmp = ( tmp >> THRESH_A);

	return tmp;
}

EXPORTED_FUNCTION unsigned int GetWaveformHysteresisValue(unsigned short BoardNum)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_TRIGGER_THRESHOLD, 0, 0);
	tmp &= ( 0xffff << THRESH_B);
	tmp = ( tmp >> THRESH_B);

	val = GetWaveformThresholdValue(BoardNum);

	return (tmp - val);
}

EXPORTED_FUNCTION unsigned int GetFiducialMarks(unsigned short BoardNum)
{
	unsigned int val, tmp;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, 0, 0);
	tmp &= ( 0x1 << FIDUCIAL);
	tmp = ( tmp >> FIDUCIAL);

	return tmp;
}


EXPORTED_FUNCTION void SetMotorReg(unsigned short BoardNum, unsigned int axis, unsigned long val)
{
	unsigned long reg_offset;

	switch (axis)
	{
		case 0:
			reg_offset = PIO_OFFSET_MOTOR_0;
			break;
		case 1:
			reg_offset = PIO_OFFSET_MOTOR_1;
			break;
		default:
			printf("motor axis %d out of range.  Must be in 0..1\n",axis);
			return;
			break;
	}

	SET_PIO_REG(BoardNum,reg_offset,val);
}

EXPORTED_FUNCTION unsigned long GetMotorReg(unsigned short BoardNum, unsigned int axis)
{
	unsigned long reg_offset;

	switch (axis)
	{
		case 0:
			reg_offset = PIO_OFFSET_MOTOR_0;
			break;
		case 1:
			reg_offset = PIO_OFFSET_MOTOR_1;
			break;
		default:
			printf("motor axis %d out of range.  Must be in 0..1\n",axis);
			return 0;
			break;
	}

	return GET_PIO_REG(BoardNum, reg_offset);
}


void SET_NUM_AVERAGES(unsigned short BoardNum, unsigned int NumAverages)
{
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_AVERAGER_CYCLES, NumAverages, 0, 0);
}

unsigned int SET_AVERAGER_LENGTH(unsigned short BoardNum, unsigned int AveragerLength)
{
	unsigned int tmp;

	tmp  = ConvertAveragerLengthValue(BoardNum, AveragerLength);

	SET_SHADOW_REG(BoardNum, PIO_OFFSET_AVERAGER_LEN, tmp, 0, 0);

	tmp = UnconvertAveragerLengthValue(BoardNum, tmp);

	return tmp;
}

EXPORTED_FUNCTION unsigned int GetNumAveragesValue(unsigned short BoardNum)
{
	return GET_SHADOW_REG(BoardNum, PIO_OFFSET_AVERAGER_CYCLES, 0, 0);
}

EXPORTED_FUNCTION unsigned int GetAveragerLengthValue(unsigned short BoardNum)
{
	unsigned int tmp, AveragerLength;

	tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_AVERAGER_LEN, 0, 0);

	AveragerLength = UnconvertAveragerLengthValue(BoardNum, tmp);

	return AveragerLength;
}

EXPORTED_FUNCTION void SetPreTriggerMemory (unsigned short BoardNum, unsigned int Value)
{
	unsigned int tmp, val, adjustedVal;

	adjustedVal = Value;
	switch(dev_config[BoardNum].pre_trigger_len)
	{
		case 512:
			break;
		case 4096:
			adjustedVal /= 2;		
			break;
		default:
			break;
	}
	if ((dev_config[BoardNum].is_ISLA214P) || (dev_config[BoardNum].is_AD5474))
	{
		adjustedVal /= 2;  // 14 bit adcs send data in 2 streams in parallel effectively doubling pretrigger memory
	}

	unsigned int shadow = tmp = GET_SHADOW_REG(BoardNum, PIO_OFFSET_PRE_TRIGGER, 0, 0);
	tmp &= ~(0x7ff << PRE_TRIGGER);   // zero out pre trigger
	val = ((adjustedVal & 0x7ff) << PRE_TRIGGER);
	tmp |= val;
	SET_SHADOW_REG(BoardNum, PIO_OFFSET_PRE_TRIGGER, tmp, 0, 0);
}

void SetCaptureDepth_NonShadow (unsigned short BoardNum, unsigned int size)
{
	SET_PIO_REG(BoardNum, PIO_OFFSET_CAPTURE_DEPTH, size);
}

void SetCaptureCount_NonShadow (unsigned short BoardNum, unsigned int count)
{
	SET_PIO_REG(BoardNum, PIO_OFFSET_CAPTURE_COUNT, count);
}

void SetNumAverages_NonShadow (unsigned short BoardNum, unsigned int NumAverages)
{
	SET_PIO_REG(BoardNum, PIO_OFFSET_AVERAGER_CYCLES, NumAverages);
}

void SetAveragerLength_NonShadow (unsigned short BoardNum, unsigned int AveragerLength)
{
	unsigned int tmp;

	tmp  = ConvertAveragerLengthValue(BoardNum, AveragerLength);
	SET_PIO_REG(BoardNum, PIO_OFFSET_AVERAGER_LEN, tmp);
}

EXPORTED_FUNCTION bool is_ISLA216P(unsigned short BoardNum)
{
	if (dev_config[BoardNum].config_file_found != 1)
		GetBoardConfigInfo(BoardNum);  // not thread safe,  pre-called by uvAPI
	if (dev_config[BoardNum].is_ISLA216P == 0)
		return false;
	else
		return true;
}

EXPORTED_FUNCTION bool is_AD5474(unsigned short BoardNum)
{
	if (dev_config[BoardNum].config_file_found != 1)
		GetBoardConfigInfo(BoardNum);  // not thread safe,  pre-called by uvAPI
	if (dev_config[BoardNum].is_AD5474 == 0)
		return false;
	else
		return true;
}
EXPORTED_FUNCTION bool has_microsynth(unsigned short BoardNum)
{
	if (dev_config[BoardNum].config_file_found != 1) 
		GetBoardConfigInfo(BoardNum);  // not thread safe,  pre-called by uvAPI

	if (dev_config[BoardNum].MICROSYNTH == 0)
		return false;
	else
		return true;
}

EXPORTED_FUNCTION bool is_adc12d2000(unsigned short BoardNum)
{
	if (dev_config[BoardNum].config_file_found != 1) 
		GetBoardConfigInfo(BoardNum);  // not thread safe,  pre-called by uvAPI

	if (dev_config[BoardNum].is_adc12d2000 == 0)
		return false;
	else
		return true;
}




// ADC08D1520 or ADC083000 Full-Scale Voltage Adjust, Select Right-Hand-Side ADC, Select Q-Channel (ADC08D1520 Only)
void AdcSetGain(unsigned short BoardNum, unsigned int adc_fsr, unsigned short RHS_ADC, unsigned short sel_Q_chan)
{

	unsigned long adc_word, adc_dat;
	unsigned short adc_adr, adc_we, adc_cal, adc_rst, adc_mask3, adc_mask2;

	adc_mask3 = RHS_ADC;  adc_mask2 = !RHS_ADC;  adc_rst = 0;  adc_cal = 0;  adc_we = 1;

	if( dev_config[BoardNum].is_adc08d1520 )
	{
		if(sel_Q_chan){ adc_adr = 0x0B; }
		else{ adc_adr = 0x03; }
	}
	else
	{
		adc_adr = 0x03;
	}

	// FSR adjustment range is 0000 0000 0 (560mVpp) - 1111 1111 1 (840mVpp)    1000 0000 0 (700mVpp - default value)
	adc_dat = (adc_fsr << 7) | 0x7F;	// 15:7 are data (9 bit adjustment), 6:0 must be set to 1
	adc_word =  (adc_mask2 << 25) | (adc_mask3 << 24) | (adc_rst << 22) | (adc_cal << 21) | (adc_we << 20) | (adc_adr << 16) | adc_dat;

	SET_PIO_REG(BoardNum, PIO_OFFSET_ADCSERIAL, adc_word);
	SLEEP(1);
}


// ADC08D1520 or ADC083000 Voltage Offset, offset negative, Select Right-Hand-Side ADC, Select Q-Channel (ADC08D1520 Only)
void AdcSetOffset(unsigned short BoardNum, unsigned short adc_offset, unsigned short adc_neg, unsigned short RHS_ADC, unsigned short sel_Q_chan)
{
	unsigned long adc_word, adc_dat;
	unsigned short adc_adr, adc_we, adc_cal, adc_rst, adc_mask3, adc_mask2;

	adc_mask3 = RHS_ADC;  adc_mask2 = !RHS_ADC;  adc_rst = 0;  adc_cal = 0;  adc_we = 1;

	if( dev_config[BoardNum].is_adc08d1520 )
	{
		if(sel_Q_chan){ adc_adr = 0x0A; }
		else{ adc_adr = 0x02; }
	}
	else
	{
		adc_adr = 0x02;
	}


	adc_dat = (adc_offset << 8) | (adc_neg << 7) | 0x7F;
	adc_word =  (adc_mask2 << 25) | (adc_mask3 << 24) | (adc_rst << 22) | (adc_cal << 21) | (adc_we << 20) | (adc_adr << 16) | adc_dat;

	SET_PIO_REG(BoardNum, PIO_OFFSET_ADCSERIAL, adc_word);
	SLEEP(1);
}



// Apply a fine phase shift to 8-bit ADC clocks to insure data alignment at high RF input frequencies 092010
void AdcClockAdjust(unsigned short BoardNum)
{
	unsigned long adc_word;
	unsigned long adc_dat;
	unsigned short adc_adr, adc_we, adc_cal, adc_rst, adc_mask3, adc_mask2, fine_phase_val, coarse_phase_val;
	double phase_val;
	int y_int;

	// Phase shift value is applied to ADC3
	// Phase shift of 215 found best with 500MHz clock
	// Phase shift of 124 found best with 1000MHz clock.
	// y=mx+b
	// fine_phase_val = clock_freq * (124-215)/(1000-500) + 306

	y_int = 2*dev_config[BoardNum].clk500_phs - dev_config[BoardNum].clk1000_phs;
	phase_val = (double)AdcClockGetFreq(BoardNum) * (dev_config[BoardNum].clk1000_phs-dev_config[BoardNum].clk500_phs);       // multiply first
	phase_val =  phase_val/(1000-500)  + y_int;
	fine_phase_val = (unsigned short) phase_val;
	coarse_phase_val = 0;


	//if(ss->verbose){ printf("clk500phs=%d  clk1000phs=%d  y_int=%d \n", clk500phs, clk1000phs, y_int ); }
	//if(ss->verbose){ printf("ADC clock fine_phase_val=%d shiftadc2=%d\n", fine_phase_val, shiftadc2 ); }

	// 100710 ADC083000 Requires clock phase adjusment to be enabled. ADC08D1520 doesn't.
	if( !dev_config[BoardNum].is_adc08d1520 )
	{
		// Enable clock phase adjustment with zero coarse adjust, both channels
		adc_mask3 = 0;
		adc_mask2 = 0;
		adc_rst = 0;
		adc_cal = 0;
		adc_we = 1;
		adc_adr = 0x0E;

		// Coarse 0xF3FF		0x93FF
		// Disable Adjust		0x03FF
		// Enable Adjust, coarse = 0	0x83FF
		adc_dat = 0x83FF; 	// 15:7 are data (9 bit adjustment), 6:0 must be set to 1
		adc_word =  (adc_mask2 << 25) | (adc_mask3 << 24) | (adc_rst << 22) | (adc_cal << 21) | (adc_we << 20) | (adc_adr << 16) | adc_dat;

		SET_PIO_REG(BoardNum, PIO_OFFSET_ADCSERIAL, adc_word);
		SLEEP(1);
	}


	// Reset phase adjustment to zero for both channels
	adc_mask3 = 0;
	adc_mask2 = 0;
	adc_rst = 0;
	adc_cal = 0;
	adc_we = 1;

	if( dev_config[BoardNum].is_adc08d1520 )
	{
		// Reset the Fine Phase Adjustment
		adc_adr = 0x0E;     // Address 0x0E for ADC08D1520
		adc_dat = 0x00FF; 	// 15:8 are data (8 bit adjustment), 7:0 must be set to 1. Fine no adjust -> 0x00FF
		adc_word =  (adc_mask2 << 25) | (adc_mask3 << 24) | (adc_rst << 22) | (adc_cal << 21) | (adc_we << 20) | (adc_adr << 16) | adc_dat;
		SET_PIO_REG(BoardNum, PIO_OFFSET_ADCSERIAL, adc_word);
		SLEEP(ADC_SERIAL_WAIT);

		// Reset the Intermediate/Coarse Adjustment
		adc_adr = 0x0F;     // Address 0x0F for ADC08D1520
		adc_dat = 0x007F; 	// bit 15 is clock polarity, 14:10 are CPA, 9:7 are IPA, 6:0 must be set to 1. Fine no adjust -> 0x00FF
		adc_word =  (adc_mask2 << 25) | (adc_mask3 << 24) | (adc_rst << 22) | (adc_cal << 21) | (adc_we << 20) | (adc_adr << 16) | adc_dat;
		SET_PIO_REG(BoardNum, PIO_OFFSET_ADCSERIAL, adc_word);
		SLEEP(ADC_SERIAL_WAIT);
	}
	else
	{
		adc_adr = 0x0D;

		// Fine no adjust 	0x007F
		adc_dat = 0x007F; 	// 15:7 are data (9 bit adjustment), 6:0 must be set to 1
		adc_word =  (adc_mask2 << 25) | (adc_mask3 << 24) | (adc_rst << 22) | (adc_cal << 21) | (adc_we << 20) | (adc_adr << 16) | adc_dat;

		SET_PIO_REG(BoardNum, PIO_OFFSET_ADCSERIAL, adc_word);
		SLEEP(ADC_SERIAL_WAIT);
	}




	// Apply fine phase adjust
	adc_mask3 = !(dev_config[BoardNum].shift_adc2);
	adc_mask2 = dev_config[BoardNum].shift_adc2;
	adc_rst = 0;
	adc_cal = 0;
	adc_we = 1;

	if( dev_config[BoardNum].is_adc08d1520 )
	{

		if(fine_phase_val > 0x00FF)
		{
			// 65ps, 11ps, 0.2ps Nominal Adjustment values for Coarse, Intermediate and Fine phase adjustments
			// Fine Phase Adjustment is 8bits, Intermediate Phase Adjustment is 3bits, leaving use one bit for the coarse adjustment
			coarse_phase_val = fine_phase_val >> 8;
			fine_phase_val &= 0x00FF;   // use only the lower 8-bits for the FPA
		}

		//if(verbose){ printf("coarse_phase_val=%d  fine_phase_val=%d \n", coarse_phase_val, fine_phase_val ); }

		// Set the Fine Phase Adjustment
		adc_adr = 0x0E;                             // Address 0x0E for ADC08D1520
		adc_dat = (fine_phase_val << 8) | 0x00FF; 	// 15:8 are data (8 bit adjustment), 7:0 must be set to 1. Fine no adjust -> 0x00FF
		adc_word =  (adc_mask2 << 25) | (adc_mask3 << 24) | (adc_rst << 22) | (adc_cal << 21) | (adc_we << 20) | (adc_adr << 16) | adc_dat;
		SET_PIO_REG(BoardNum, PIO_OFFSET_ADCSERIAL, adc_word);
		SLEEP(ADC_SERIAL_WAIT);

		// Reset the Intermediate/Coarse Adjustment
		adc_adr = 0x0F;                                 // Address 0x0F for ADC08D1520
		adc_dat = (coarse_phase_val << 8) | 0x007F; 	// bit 15 is clock polarity, 14:10 are CPA, 9:7 are IPA, 6:0 must be set to 1. Fine no adjust -> 0x00FF
		adc_word =  (adc_mask2 << 25) | (adc_mask3 << 24) | (adc_rst << 22) | (adc_cal << 21) | (adc_we << 20) | (adc_adr << 16) | adc_dat;
		SET_PIO_REG(BoardNum, PIO_OFFSET_ADCSERIAL, adc_word);
		SLEEP(ADC_SERIAL_WAIT);
	}
	else
	{
		adc_adr = 0x0D;

		// Fine no adjust 	0x007F
		adc_dat = (fine_phase_val << 9) | 0x007F;       // 15:7 are data (9 bit adjustment), 6:0 must be set to 1
		adc_word =  (adc_mask2 << 25) | (adc_mask3 << 24) | (adc_rst << 22) | (adc_cal << 21) | (adc_we << 20) | (adc_adr << 16) | adc_dat;

		if(PRINT_TODO){ printf("fine_phase_val shifted correct amount for ADC083000?? << 7\n"); }

		SET_PIO_REG(BoardNum, PIO_OFFSET_ADCSERIAL, adc_word);
		SLEEP(ADC_SERIAL_WAIT);
	}

}



void AdcClockOk(unsigned short BoardNum)    //this will need changes to work, should return something or mod a global DSchriebman
{
	unsigned long readval;

	readval = GET_PIO_REG(BoardNum, PIO_OFFSET_PROMPROG);

	bool adc_clock_ok;

	// only intitate acqusition if ADC DCM is locked
	if( (readval & (1 << 9)) && (readval != 0xFFFFFFFF) )
	{
		adc_clock_ok = true;
	}
	else
	{
		printf("AdcClockOk() Failed! readval=%x\n", readval);
		adc_clock_ok = false;
	}
	// we don't use dcm anymore!
	adc_clock_ok = true;
}


// Calibrate 8-bit National ADCs
void AdcCalibrate(unsigned short BoardNum)
{
	bool non_des=true;

	unsigned long readval;

	AdcDcmPrelimSetup(BoardNum);

	//AdcIdelayShift(BoardNum);

	AdcClockAdjust(BoardNum);

	AdcDcmSetup(BoardNum);

	SetDualSingleMode(BoardNum);

	if (dev_config[BoardNum].is_adc08d1520)
	{
		//initialize all adc08d1520 spi registers
		SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x00107FFF);	//1 0000 01111111 11111111	Calibration			0x0
		SLEEP(ADC_SERIAL_WAIT);


		//SET_PIO_REG(PIO_OFFSET_ADCSERIAL, 0x0011B2FF);	//1 0001 10110010 11111111	Configuration 		0x1
		//SLEEP(ADC_SERIAL_WAIT);

		//SET_PIO_REG(PIO_OFFSET_ADCSERIAL, 0x0012007F);	//1 0010 00000000 11111111	I-Channel Offset	0x2
		//SLEEP(ADC_SERIAL_WAIT);
		//SET_PIO_REG(PIO_OFFSET_ADCSERIAL, 0x0013807F);	//1 0011 10000000 01111111	I-Channel FSR		0x3
		//SLEEP(ADC_SERIAL_WAIT);

		//if (ss->adc_clock_freq > 900)
		//{
		//SET_PIO_REG(PIO_OFFSET_ADCSERIAL, 0x001903FF);	//1 1001 00000011 11111111	Extended Config		0x9
		//SLEEP(ADC_SERIAL_WAIT);
		//}
		//else
		//{
		//SET_PIO_REG(PIO_OFFSET_ADCSERIAL, 0x001907FF);	//1 1001 00000011 11111111	Extended Config		0x9
		//SLEEP(ADC_SERIAL_WAIT);
		//}
		//SET_PIO_REG(PIO_OFFSET_ADCSERIAL, 0x001A007F);	//1 1010 00000000 11111111	Q-Channel Offset	0xA
		//SLEEP(ADC_SERIAL_WAIT);
		//SET_PIO_REG(PIO_OFFSET_ADCSERIAL, 0x001B807F);	//1 1011 10000000 01111111	Q-Channel FSR		0xB
		//SLEEP(ADC_SERIAL_WAIT);
		//SET_PIO_REG(PIO_OFFSET_ADCSERIAL, 0x001E00FF);	//1 1110 00000000 11111111	Sample CLK FPA		0xE
		//SLEEP(ADC_SERIAL_WAIT);
		//SET_PIO_REG(PIO_OFFSET_ADCSERIAL, 0x001F007F);	//1 1111 00000000 01111111	Sample CLK IPA		0xF
		//SLEEP(ADC_SERIAL_WAIT);

		//adc08d1520 single channel mode (DES)
		if (dev_config[BoardNum].adc_chan_used==1)      //probably not correct DSchriebman
		{
			if (0==0)       //should be selected channel=0 DSchriebman
			{
				//When in DES mode, must write same offset and fsr values to both converters in adc08d1520
				SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, (dev_config[BoardNum].adc3_off << 8)|(dev_config[BoardNum].adc3_off_neg << 7)|0x0012007F);	//1 0010 00000000 11111111	I-Channel Offset	0x2
				SLEEP(ADC_SERIAL_WAIT);
				SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, (dev_config[BoardNum].adc3_fsr << 7)|0x0013807F);	//1 0011 10000000 01111111	I-Channel FSR		0x3
				SLEEP(ADC_SERIAL_WAIT);
				SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, (dev_config[BoardNum].adc3_off << 8)|(dev_config[BoardNum].adc3_off_neg << 7)|0x001A007F);	//1 0010 00000000 11111111	Q-Channel Offset	0xA
				SLEEP(ADC_SERIAL_WAIT);
				SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, (dev_config[BoardNum].adc3_fsr << 7)|0x001B807F);	//1 1011 10000000 01111111	Q-Channel FSR		0xB
				SLEEP(ADC_SERIAL_WAIT);

				if(non_des == 0)
				{
					//enable DES non-DLF adc08d1520 channel 0
					if (AdcClockGetFreq(BoardNum) > 900)
					{
						SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x1973FF);
						SLEEP(ADC_SERIAL_WAIT);
						//printf("DLF channel 0 not enabled adc_clock_freq = %d\n", ss->adc_clock_freq);
					}
					//enable DES DLF adc08d1520 channel 0
					else
					{
						SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x1977FF);
						SLEEP(ADC_SERIAL_WAIT);
						//printf("DLF channel 0 enabled adc_clock_freq = %d\n", ss->adc_clock_freq);
					}
				}
				else
				{
					//Non-DES non-DLF SCM
					if (AdcClockGetFreq(BoardNum) > 900)
					{
						SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x001943FF);	//1 1001 00000011 11111111	Extended Config		0x9
						SLEEP(ADC_SERIAL_WAIT);
						//printf("dual channel DLF not enabled adc_clock_freq = %d\n", ss->adc_clock_freq);
					}
					//Non-DES non-DLF
					else
					{
						SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x001947FF);	//1 1001 00000011 11111111	Extended Config		0x9
						SLEEP(ADC_SERIAL_WAIT);
						//printf("dual channel DLF enabled adc_clock_freq = %d\n", ss->adc_clock_freq);
					}
				}

			}
			else
			{
				//When in DES mode, must write same offset and fsr values to both converters in adc08d1520
				SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, (dev_config[BoardNum].adc3_off_Q << 8)|(dev_config[BoardNum].adc3_off_neg_Q << 7)|0x0012007F);	//1 0010 00000000 11111111	I-Channel Offset	0x2
				SLEEP(ADC_SERIAL_WAIT);
				SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, (dev_config[BoardNum].adc3_fsr_Q << 7)|0x0013807F);	//1 0011 10000000 01111111	I-Channel FSR		0x3
				SLEEP(ADC_SERIAL_WAIT);
				SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, (dev_config[BoardNum].adc3_off_Q << 8)|(dev_config[BoardNum].adc3_off_neg_Q << 7)|0x001A007F);	//1 0010 00000000 11111111	Q-Channel Offset	0xA
				SLEEP(ADC_SERIAL_WAIT);
				SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, (dev_config[BoardNum].adc3_fsr_Q << 7)|0x001B807F);	//1 1011 10000000 01111111	Q-Channel FSR		0xB
				SLEEP(ADC_SERIAL_WAIT);

				if(non_des == 0)
				{
					//enable DES non-DLF adc08d1520 channel 1
					if (AdcClockGetFreq(BoardNum) > 900)
					{
						SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x1963FF);
						SLEEP(ADC_SERIAL_WAIT);
						//printf("DLF channel 1 not enabled adc_clock_freq = %d\n", ss->adc_clock_freq);
					}
					//enable DES DLF adc08d1520 channel 1
					else
					{
						SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x1967FF);
						SLEEP(ADC_SERIAL_WAIT);
						//printf("DLF channel 1 enabled adc_clock_freq = %d\n", ss->adc_clock_freq);
					}
				}
				else
				{
					//Non-DES non-DLF SCM
					if (AdcClockGetFreq(BoardNum) > 900)
					{
						SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x001943FF);	//1 1001 00000011 11111111	Extended Config		0x9
						SLEEP(ADC_SERIAL_WAIT);
						//printf("dual channel DLF not enabled adc_clock_freq = %d\n", ss->adc_clock_freq);
					}
					//Non-DES non-DLF
					else
					{
						SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x001947FF);	//1 1001 00000011 11111111	Extended Config		0x9
						SLEEP(ADC_SERIAL_WAIT);
						//printf("dual channel DLF enabled adc_clock_freq = %d\n", ss->adc_clock_freq);
					}
				}
			}
		}
		else
		{
			//dual channel mode (Non-DES)

			SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, (dev_config[BoardNum].adc3_off << 8)|(dev_config[BoardNum].adc3_off_neg << 7)|0x0012007F);	//1 0010 00000000 11111111	I-Channel Offset	0x2
			SLEEP(ADC_SERIAL_WAIT);

			SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, (dev_config[BoardNum].adc3_fsr << 7)|0x0013807F);	//1 0011 10000000 01111111	I-Channel FSR		0x3
			SLEEP(ADC_SERIAL_WAIT);

			SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, (dev_config[BoardNum].adc3_off_Q << 8)|(dev_config[BoardNum].adc3_off_neg_Q << 7)|0x001A007F);	//1 0010 00000000 11111111	Q-Channel Offset	0xA
			SLEEP(ADC_SERIAL_WAIT);

			SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, (dev_config[BoardNum].adc3_fsr_Q << 7)|0x001B807F);	//1 1011 10000000 01111111	Q-Channel FSR		0xB
			SLEEP(ADC_SERIAL_WAIT);

			//Non-DES non-DLF
			if (AdcClockGetFreq(BoardNum) > 900)
			{
				SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x001943FF);	//1 1001 00000011 11111111	Extended Config		0x9
				SLEEP(ADC_SERIAL_WAIT);
				//printf("dual channel DLF not enabled adc_clock_freq = %d\n", ss->adc_clock_freq);
			}
			//Non-DES non-DLF
			else
			{
				SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x001947FF);	//1 1001 00000011 11111111	Extended Config		0x9
				SLEEP(ADC_SERIAL_WAIT);
				//printf("dual channel DLF enabled adc_clock_freq = %d\n", ss->adc_clock_freq);
			}
		}

		// Calibrate the ADCs
		SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x00107FFF);      //  RST=0, CAL=0, WE=0
		SLEEP(ADC_SERIAL_WAIT);

		SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x0010FFFF);      //  RST=0, CAL=1, WE=0
		SLEEP(ADC_CAL_WAIT);

		SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x00107FFF);      //  RST=0, CAL= 0, WE=0
		SLEEP(ADC_SERIAL_WAIT);

	}
	else
	{
		// Calibrate the ADCs
		SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x00000000);      //  RST=0, CAL=0, WE=0
		SLEEP(ADC_SERIAL_WAIT);

		SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x00200000);      //  RST=0, CAL=1, WE=0
		SLEEP(ADC_SERIAL_WAIT);

		SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x00000000);      //  RST=0, CAL= 0, WE=0
		SLEEP(ADC_CAL_WAIT);
	}

	readval = GET_PIO_REG(BoardNum,PIO_OFFSET_PROMPROG);

	if((readval & (1 << 14)))
	{
		//if(ss->verbose){ printf("ADC Calibration Succeded\n"); }
		printf("ss->adc_calibration_ok = true\n");
	}
	else
	{
		//printf("ADC Calibration Failed! PROMREG=0x%x\n", readval);
		printf("ss->adc_calibration_ok = false\n");
	}

}


////////////////////////////////////////////////////////////////
// Adc Section - END
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
// AdcDcm Section - START
////////////////////////////////////////////////////////////////

void AdcDcmPrelimSetup(unsigned short BoardNum)
{
	// Setup the 8-bit ADCs to make strobes
	if( dev_config[BoardNum].is_adc08d1520 )
	{
		// Set the ADCs for basic DDR capture and unreset DCM to check ADC clock frequency
		// RST=0, CAL=0, WE=1, Adr = 1h (Config. Reg.), Data = F2FF  (Demux Mode, DDR Clock Phs. = 0deg)
		SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x0011B2FF);
		SLEEP(ADC_SERIAL_WAIT);
	}
	else
	{
		// Set the ADCs for basic DDR capture and unreset DCM to check ADC clock frequency
		// RST=0, CAL=0, WE=1, Adr = 1h (Config. Reg.), Data = F2FF  (Diff. Rst, DDR Clock Phs. = 0deg)
		SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x0011F2FF);
		SLEEP(ADC_SERIAL_WAIT);
	}

	// For initial setup set to zero phase shift applied
	AdcDcmSetPs(BoardNum, 0, 0);

	// Reset 8-bit ADCs, unreset edge is what causes unreset 121009
	AdcReset(BoardNum);

	// Unreset 8-bit ADCs, creates strobe, 121009
	AdcUnreset(BoardNum);
}

void AdcDcmSetup(unsigned short BoardNum)
{

	int num_ps_shift, ps_positive;
	int ps_array[20];
	unsigned int clock_freq_index;

	memset(ps_array, 0, sizeof(ps_array));  // zero the phase shift array
	clock_freq_index = (unsigned int) (AdcClockGetFreq(BoardNum)+50)/100; // round to the closest 100MHz and use the corresponding value from the array


	if( dev_config[BoardNum].is_adc08d1520 )
	{
		ps_array[17] = -75;    // Rev D. 1700MHz  -75 or +200, quadrature
		ps_array[16] = -50;    // Rev D. 1600MHz  -50, quadrature
		ps_array[15] = -25;    // Rev D. 1500MHz  -25, quadrature
		ps_array[14] = 25;     // Rev D. 1400MHz  +25 or -225, quadrature
		ps_array[13] = 50;     // Rev D. 1300MHz   +50 or -200, quadrature
		ps_array[12] = 75;     // Rev D. 1200MHz   +75 or -200, quadrature
		ps_array[11] = 100;    // Rev D. 1100MHz   -175 or +100, co-phase
		ps_array[10] = 125;    // Rev D. 1000MHz   + or - 125, co-phase
		ps_array[9] = 150;     // Rev D. 0900MHz   + or - 125 or 150, co-phase
		ps_array[8] = 175;     // Rev D. 0800MHz   +175, co-phase
		ps_array[7] = 175;     // Rev D. 0700MHz   +175, co-phase
		ps_array[6] = -25;     // Rev D. 0600MHz   -25, co-phase
		ps_array[5] = 0;       // Rev D. 0500MHz   0, co-phase

		num_ps_shift = ps_array[clock_freq_index];
		//num_ps_shift = 120;

	}
	else
	{
		if(dev_config[BoardNum].board_rev == 0)
		{
			// Pre-Rev D Phase Shift
			num_ps_shift = (int) (120+(-140.0/600.0)*(AdcClockGetFreq(BoardNum))); // 120+(-140/600)*clock_freq
			////num_ps_shift = 130;
			////ps_positive = 1;
		}
		else
		{
			// Rev D. 1800MHz->PS=+90	Co-Phase Strobe
			// Rev D. 1700MHz->PS=+130	Quadrature Strobe (PS=200 best on 1037!, 160 OK)
			// Rev D. 1600MHz->PS=+160	Quadrature Strobe
			// Rev D. 1500MHz->PS=+190	Quadrature Strobe (PS=0 best on 1037!, 200 OK)
			// Rev D. 1400MHz->PS=+200	Quadrature Strobe

			// Rev D. 1300MHz->PS=0		Quadrature Strobe (or +210)
			// Rev D. 1200MHz->PS=20	Quadrature Strobe (or +220)
			// Rev D. 1100MHz->PS=40	Quadrature Strobe
			// Rev D. 1000MHz->PS=60	Quadrature Strobe
			// Rev D. 0900MHz->PS=80	Quadrature Strobe (110 is better)

			// Rev D. 0800MHz->PS=160	Quadrature Strobe or Co-Phase (100 is OK)
			// Rev D. 0700MHz->PS=180	Co-Phase Strobe
			// Rev D. 0600MHz->PS=200	Co-Phase Strobe
			// Rev D. 0500MHz->PS=220	Co-Phase Strobe

			if( (AdcClockGetFreq(BoardNum) <= 800) )
			{
				num_ps_shift = (int) (160 + 20*(800-AdcClockGetFreq(BoardNum))/100);
			}
			else if( (AdcClockGetFreq(BoardNum) > 800) && (AdcClockGetFreq(BoardNum) < 1300) )
			{
				num_ps_shift = (int) (20*(1300-AdcClockGetFreq(BoardNum))/100);
			}
			else if( (AdcClockGetFreq(BoardNum) >= 1300) )
			{
				num_ps_shift = (int) (120 + 30*(1800-AdcClockGetFreq(BoardNum))/100); // 092410 y-int was 90
			}
		}
	}


	if( num_ps_shift > 0)
	{
		if(abs(num_ps_shift)>255) { num_ps_shift=255; }
		ps_positive = 1;
	}
	else
	{
		if(abs(num_ps_shift)>255) { num_ps_shift=-255; }
		ps_positive = 0;
	}


	if(dev_config[BoardNum].adc_res == 8)
	{
		//if(ss->verbose){ printf("clock_freq_index=%d num_ps_shift=%d  ps_pos=%d\n", clock_freq_index, num_ps_shift, ps_positive ); }
	}


	// Select the ADC strobe to data alignment for initial data capture, co-phase or quadrature
	// between 625Mhz & 1150MHz or above 1700MHz (BRI-092010)
	if( ((AdcClockGetFreq(BoardNum) > 400) && (AdcClockGetFreq(BoardNum) < 1150)) ||
	   ((AdcClockGetFreq(BoardNum) > 1700) && (AdcClockGetFreq(BoardNum) < 1900)))
	{

		//if(ss->verbose){ printf("Selecting co-phase ADC strobe\n"); }

		if( dev_config[BoardNum].is_adc08d1520 )
		{
			// Set the ADCs for basic DDR capture and unreset DCM to check ADC clock frequency
			// RST=0, CAL=0, WE=1, Adr = 1h (Config. Reg.), Data = F2FF  (Demux Mode, DDR Clock Phs. = 0deg)
			SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x0011B2FF);
			SLEEP(1);
		}
		else
		{
			if(PRINT_TODO){ printf("Do we even use the digital shift?\n"); }

			// (select 0deg clock phase)
			// RST=0, CAL=0, WE=1, Adr = 1h (Config. Reg.), Data = F2FF  (Diff. Rst, DDR Clock Phs. = 0deg)
			SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x0011F2FF);
			SLEEP(1);

			//pio_reg[BoardNum].CaptureAdjReg |= 0x00000001;				// (enable digital shift)		1st LS Nibble:	bit0->enable digital shift
			//CaptureAdjReg &= 0xFFFFFFFE;				// (disable digital shift)		1st LS Nibble:	bit0->enable digital shift
			//SET_PIO_REG(BoardNum,PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg); Disable for now DSchriebman
			SLEEP(1);
		}
	}
	else
	{

		if(dev_config[BoardNum].adc_res == 8)
		{
			//if(ss->verbose){ printf("Selecting quadrature ADC strobe\n"); }
		}

		if( dev_config[BoardNum].is_adc08d1520 )
		{
			// (select 90deg clock phase)
			// RST=0, CAL=0, WE=1, Adr = 1h (Config. Reg.), Data = F2FF  (Demux Mode, DDR Clock Phs. = 90deg)
			SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x0011BAFF);
			SLEEP(1);
		}
		else
		{
			// (select 90deg clock phase)
			// RST=0, CAL=0, WE=1, Adr = 1h (Config. Reg.), Data = FAFF  (Diff. Rst, DDR Clock Phs. = 90deg)
			SET_PIO_REG(BoardNum,PIO_OFFSET_ADCSERIAL, 0x0011FAFF);
			SLEEP(1);
		}
	}


	// Set the 8-bit ADC DCM phase shift value, this just tells the firmware the phase shift value to use when the ADCs are unreset 120909
	AdcDcmSetPs(BoardNum, num_ps_shift, ps_positive);

}

// Set the 8-bit ADC DCM phase shift value
void AdcDcmSetPs(unsigned short BoardNum, int num_ps_shift, int ps_positive)
{
	unsigned long templong, clearlong;
	templong = (unsigned long) abs(num_ps_shift);
	templong = templong << DCM_PS_VAL;
	templong |= (ps_positive << DCM_PS_POL);

	clearlong = (0x000000FF << DCM_PS_VAL);  // Phase shift value is eight bits, clear them first
	clearlong |= (1 << DCM_PS_POL);

	//pio_reg[BoardNum].CaptureAdjReg &= ~(clearlong);         // clear the bits first
	//pio_reg[BoardNum].CaptureAdjReg |= templong;             // set the bits
	//SET_PIO_REG(BoardNum,PIO_OFFSET_CAPTUREADJ, pio_reg[BoardNum].CaptureAdjReg); Disabled for now DSchriebman
	SLEEP(1);
}

void AdcDcmPsCal(unsigned short BoardNum)
{

	unsigned long readval	= 0;
	unsigned long total_glitches;
	int num_ps_shift, cal_ps_index, ps_positive, ps_delta;
	unsigned int chan_i;

	ps_delta = 255/(ADC_DCM_PS_ITER/2);	// 10


	// Given that the offsets for the ADC are such the we see 50% 7F/80 test the different phase shifts to get the best recapture timing
	for(cal_ps_index=-ADC_DCM_PS_ITER/2; cal_ps_index <= ADC_DCM_PS_ITER/2; cal_ps_index+=1) // -255 to +256 PS range for DCM
	{

		num_ps_shift = ps_delta*abs(cal_ps_index);

		if(cal_ps_index <0)
		{
			ps_positive = 0;
		}
		else
		{
			ps_positive = 1;
		}

		//if(ss->verbose){ printf("PS test: ps_mag=%d  ps_pos=%d\n", num_ps_shift, ps_positive); }


		AdcDcmSetPs(BoardNum, num_ps_shift, ps_positive);

		// Do a final 8-bit reset/unreset now the firmware knows the DCM phase shift value to use in its ADC unreset state machine
		// Reset 8-bit ADCs, unreset edge is what causes unreset 121009
		AdcReset(BoardNum);

		// Unreset 8-bit ADCs, creates strobe, 121009
		AdcUnreset(BoardNum);

		AdcClockOk(BoardNum);

		// only intitate acqusition if ADC DCM is locked
		if( 1==1 )      //this should be based on clockok DSchriebman
		{
			//RUNBOARD(false);
			SET_PIO_REG(BoardNum,PIO_OFFSET_BLOCKSIZE, DIG_BLOCKSIZE);
			SLEEP(1);
			SET_PIO_REG(BoardNum,PIO_OFFSET_NUMBLOCKS, CHECKMEM_NUM_BLOCKS);
			SLEEP(1);
			SET_PIO_REG(BoardNum,PIO_OFFSET_PCIE_RD_START_BLOCK, 0);

			//CheckMem(CHECKMEM_NUM_BLOCKS, (int)NULL, ss->pBuffer);        //commenting out for now DSchriebman

			// Store the discontinuity data for each run so we can plot it all at the same time
			for(chan_i=0; chan_i<dev_config[BoardNum].adc_chan_used; chan_i++)
			{
				cal_data[BoardNum].glitches_ps[chan_i][cal_ps_index+(ADC_DCM_PS_ITER/2)] = cal_data[BoardNum].glitches[chan_i];
			}
		}
		else
		{
			printf("ADC DCM not locked!! Check Clock!! Exiting..\n");
		}

	} // end strobe phase shift calibration


	printf("Summed glitches for all channels at each PS value:\n");

	for(cal_ps_index=-ADC_DCM_PS_ITER/2; cal_ps_index <= ADC_DCM_PS_ITER/2; cal_ps_index+=1) // -255 to +256 PS range for DCM
	{
		total_glitches = 0;

		for(chan_i=0; chan_i<dev_config[BoardNum].adc_chan_used; chan_i++)
		{
			total_glitches += cal_data[BoardNum].glitches_ps[chan_i][cal_ps_index+(ADC_DCM_PS_ITER/2)];
		}

		printf("PS=%d:  %d\n", (ps_delta*cal_ps_index), total_glitches );
	}

}


void set_adc_data_setup_reg (unsigned short BoardNum)
{
	//int pio_regBoardNumAdcDataSetup = 0;    //this will need to be dealt with DSchriebman
	int pio_regBoardNumAdcDataSetup = GET_PIO_REG(BoardNum, PIO_OFFSET_ADC_DATA_SETUP);
	int non_des=0;
	int adc_deci_value=1;
	//if(ss->verbose){printf("set_adc_data_setup_reg(): ss->adc_chan %d\n",ss->adc_chan);}

	if (dev_config[BoardNum].adc_chan == 1)
	{
		//if(ss->verbose){printf("1 adc_data_setup = %x\n",pio_reg[BoardNum].AdcDataSetup);}

		//In non-DES SCM, Decimation must be at least x2. Enforce decimation at least x2
		if (non_des == 1)
		{
			if (adc_deci_value == 1)
			{
				printf("In Non-DES SCM. Decimation must be at least x2. Forcing decimation x2");
				adc_deci_value = 2;
			}
		}
		pio_regBoardNumAdcDataSetup |= (adc_deci_value << ADC_DECI_OFFSET);

		//if(ss->verbose){printf("2 adc_data_setup = %x\n",pio_reg[BoardNum].AdcDataSetup);}
		pio_regBoardNumAdcDataSetup |= (dev_config[BoardNum].adc_res << ADC_RESOLUTION_OFFSET);
		//if(ss->verbose){printf("3 adc_data_setup = %x\n",pio_reg[BoardNum].AdcDataSetup);}
		//  devconfig is wrong coming in
		pio_regBoardNumAdcDataSetup |= (dev_config[BoardNum].adc_chan << NUM_ADC_CHAN_OFFSET);
		//pio_regBoardNumAdcDataSetup |= (1 << NUM_ADC_CHAN_OFFSET);
		//if(ss->verbose){printf("4 adc_data_setup = %x\n",pio_reg[BoardNum].AdcDataSetup);}
		//pio_regBoardNumAdcDataSetup |= ((ss->adc_user_value & 0x0f) << ADC_USER_OFFSET);  //not sure what user value is DSchriebman
		//if(ss->verbose){printf("adc_data_setup = %x\n",pio_reg[BoardNum].AdcDataSetup);}



		printf("dev_config[BoardNum].adc_chan_used  %d\n",dev_config[BoardNum].adc_chan_used  );
		printf("dev_config[BoardNum].adc_chan  %d\n",dev_config[BoardNum].adc_chan  );
		printf("PIO_OFFSET_ADC_DATA_SETUP %x\n",pio_regBoardNumAdcDataSetup );
		SET_PIO_REG(BoardNum,PIO_OFFSET_ADC_DATA_SETUP, pio_regBoardNumAdcDataSetup);
		Sleep(1);
	}
	if (dev_config[BoardNum].adc_chan == 2)
	{
		{
			//if(ss->verbose){printf("1 adc_data_setup = %x\n",pio_reg[BoardNum].AdcDataSetup);}
			pio_regBoardNumAdcDataSetup |= (adc_deci_value << ADC_DECI_OFFSET);
			//if(ss->verbose){printf("2 adc_data_setup = %x\n",pio_reg[BoardNum].AdcDataSetup);}
			pio_regBoardNumAdcDataSetup |= (dev_config[BoardNum].adc_res << ADC_RESOLUTION_OFFSET);
			//if(ss->verbose){printf("3 adc_data_setup = %x\n",pio_reg[BoardNum].AdcDataSetup);}
			pio_regBoardNumAdcDataSetup |= (dev_config[BoardNum].adc_chan_used << NUM_ADC_CHAN_OFFSET);
			//if(ss->verbose){printf("4 adc_data_setup = %x\n",pio_reg[BoardNum].AdcDataSetup);}
			//pio_regBoardNumAdcDataSetup |= ((ss->adc_user_value & 0x0f) << ADC_USER_OFFSET);  //not sure what user value is DSchriebman
			//if(ss->verbose){printf("adc_data_setup = %x\n",pio_reg[BoardNum].AdcDataSetup);}
			//pio_regBoardNumAdcDataSetup |= (0 << AD12_ADC_ECEBAR);
			//pio_regBoardNumAdcDataSetup |= (0 << AD12_ADC_NDM);
			SET_PIO_REG(BoardNum,PIO_OFFSET_ADC_DATA_SETUP, pio_regBoardNumAdcDataSetup);
			Sleep(1);
		}
	}
	//ss->AdcDataSetup =  pio_reg[BoardNum].AdcDataSetup;
}

// Set the DUAL_ADC_SNGL_CHAN bit in the Capture Adjustment Register, channel_sel has no affect on 12/14-bit boards
bool SetDualSingleMode(unsigned short BoardNum)
{
	bool adc08d1520_scm = false;
	int single_chan_mode = 0;
	int single_chan_select = 0;
	//int pio_regBoardNumCaptureAdjReg = 0;   //this needs to pull current value
	int pio_regBoardNumCaptureAdjReg = GET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ);

	if(adc08d1520_scm && dev_config[BoardNum].is_adc08d1520)
	{
		//The DualSingleMode was made for AD83000x2 toggling which ADC on mainboard to select. ADC08D1520 outputs data to same place.
		pio_regBoardNumCaptureAdjReg |= (0 << SINGLE_CHAN_SELECT);	// select channel if using single channel mode (033010)
		pio_regBoardNumCaptureAdjReg |= (single_chan_mode << DUAL_ADC_SNGL_CHAN);	// set the DUAL_ADC_SNGL_CHAN bit
	}
	else if (dev_config[BoardNum].is_adc12d2000)
	{
		//The DualSingleMode was made for AD83000x2 toggling which ADC on mainboard to select. ADC12D2000 acts like AD83000x2 all the time.
		pio_regBoardNumCaptureAdjReg |= (0 << SINGLE_CHAN_SELECT);	// select channel if using single channel mode (033010)
		pio_regBoardNumCaptureAdjReg |= (0 << DUAL_ADC_SNGL_CHAN);	// set the DUAL_ADC_SNGL_CHAN bit
	}
	else
	{
		pio_regBoardNumCaptureAdjReg |= (single_chan_mode << DUAL_ADC_SNGL_CHAN);	// set the DUAL_ADC_SNGL_CHAN bit
		pio_regBoardNumCaptureAdjReg |= (single_chan_select << SINGLE_CHAN_SELECT);
	}
	// CAPTURE ADJUSTMENT REGISTER SETUP
	// 3rd LS Nibble:	bit11->Init strobe count, bit9->single channel select
	// 2nd LS Nibble:	0->no decimation, 1->div2, 2->div4, 3->div8, 4->div16, others are illegal
	// 1st LS Nibble:	bit2-> dual ADC single channel mode bit0->enable digital shift
	SET_PIO_REG(BoardNum, PIO_OFFSET_CAPTUREADJ, pio_regBoardNumCaptureAdjReg);

	printf("setdualsinglemode:pio_regBoardNumCaptureAdjReg: %x\n",pio_regBoardNumCaptureAdjReg);

	return true;
}






EXPORTED_FUNCTION unsigned long GetOverruns(unsigned short BoardNum)
{
	return pAPI->ApiGetOverruns(BoardNum);

}

#ifndef _WINDOWS
#include <time.h>

void Sleep(unsigned int num_milliseconds)
{
	usleep(1000*num_milliseconds);
}
long GetTickCount()
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return (t.tv_nsec / 1000);
}

#endif
