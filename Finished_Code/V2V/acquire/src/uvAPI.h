/**
 * @file   uvAPI.h
 * @date   October, 2014
 *
 * @brief  Ultraview Data Acquisition Board DLL header File
 *
 * For Windows applications include this and uvAPI.cpp with projects accessing Ultraview boards
 * For LINUX applications include this and DO NOT include uvAPI.cpp with projects accessing Ultraview boards
 *
 **/


#ifndef UVAPI_H
#define UVAPI_H

/** @brief Defines 1MByte buffer size used in DMA transfers from Ultraview Boards */
#ifndef DIG_BLOCK_SIZE
#define DIG_BLOCK_SIZE 1024 * 1024
#endif

#define DIG_BLOCKSIZE DIG_BLOCK_SIZE

//Channels
/** @brief Defines __Channels__ values for AD16-250 */
#define IN0		1
/** @brief Defines __Channels__ values for AD16-250 */
#define IN1		2
/** @brief Defines __Channels__ values for AD16-250 */
#define IN2		4
/** @brief Defines __Channels__ values for AD16-250 */
#define IN3		8

#define CLOCK_NONE 0
#define CLOCK_INTERNAL 1
#define CLOCK_EXTERNAL 2
#define CLOCK_SELECTED_ADDR 0

#define CH_MODE_NONE			0
#define FOUR_CH_MODE            3
#define TWO_CH_MODE				2
#define ONE_CH_MODE				1
#define CH_MODE_ADDR            4

#define CH_SELECT_NONE			0  // NOT configured
#define DESCLKIQ_MODE			1  // both inputs externally connected
#define DESIQ_MODE				2  // both inputs externally connected
#define DESQ_MODE					3  // ain0
#define DESI_MODE					4  // ain1
#define CH_SELECT_ADDR 8

// TriggerTypes
#define NO_TRIGGER					0
#define WAVEFORM_TRIGGER			1
#define SYNC_SELECTIVE_RECORDING	2
#define HETERODYNE					3
#define TTL_TRIGGER_EDGE			4
#define TRIGGER_SELECT_ADDR         12

// Slope
#define FALLING_EDGE	0
#define RISING_EDGE		1
#define SLOPE_SELECT_ADDR 16




#define AD12_DECI_VAL_LIMIT 4




#ifdef _WINDOWS

#define MAX_DEVICES 4


	#include "Windows.h"
	typedef bool (* DLL_SetupBoard)(unsigned short BoardNum);  
	typedef void (* DLL_setupAcquire)(unsigned short BoardNum,unsigned int NumBlocks);
	typedef void (* DLL_SetSetupDoneBit)(unsigned short BoardNum, unsigned int Enable);

	typedef int (* DLL_x_MemAlloc)(void ** buf_addr, size_t buf_size);
	typedef int (* DLL_x_Read)(unsigned short BoardNum, void * sysMem, DWORD rd_size);
	typedef int (* DLL_x_Write)(HANDLE fd, void * sysMem, DWORD rd_size);
	typedef void (* DLL_x_FreeMem)(void * buf);
	typedef HANDLE (* DLL_x_CreateFile)(char * filename);
	typedef HANDLE (* DLL_x_OpenFile)(char * filename);
	typedef void (* DLL_x_Close)(HANDLE fd);
	typedef void ( *DLL_SetInternalClockEnable) (unsigned short BoardNum, unsigned int Enable);
	typedef unsigned int (* DLL_GetSample)(unsigned short BoardNum, void * pBuffer, unsigned int index, unsigned short channel);
	typedef unsigned int (* DLL_getNumChannels)(unsigned short BoardNum);
	typedef unsigned int (* DLL_getAllChannels)(unsigned short BoardNum);
	typedef unsigned int (* DLL_AdcClockGetFreq)(unsigned short BoardNum);
	typedef void (* DLL_ADC12D2000_Channel_Mode_Select) (unsigned short BoardNum, unsigned int Chan_mode, unsigned int Chan_select, unsigned int Calibrate);
	typedef unsigned long (* DLL_DllApiGetOverruns)(unsigned short BoardNum);

	typedef void (* DLL_SET_ECL_TRIGGER_DELAY)(unsigned short BoardNum, unsigned int Value);
	typedef void (* DLL_ECLTriggerEnable)(unsigned short BoardNum, unsigned int Enable);
	typedef void (* DLL_SetAdcDecimation)(unsigned short BoardNum, unsigned int adc_deci_value);
	typedef void (* DLL_SET_CAPTURE_COUNT)(unsigned short BoardNum, unsigned int count);
	typedef void (* DLL_SET_CAPTURE_DEPTH)(unsigned short BoardNum, unsigned int size);
	typedef void (* DLL_SetAcquireDisableInvert)(unsigned short BoardNum, unsigned int Invert);

	typedef unsigned int (* DLL_ADC12D2000_GetOneChanModeValue)(unsigned short BoardNum);

	typedef unsigned long (* DLL_getOverruns)(unsigned short BoardNum);	
	typedef bool (* DLL_is_adc12d2000)(unsigned short BoardNum);
	typedef bool (* DLL_is_AD5474)(unsigned short BoardNum);
	typedef bool (* DLL_is_ISLA216P)(unsigned short BoardNum);
	typedef bool (* DLL_has_microsynth)(unsigned short BoardNum);
	typedef void (* DLL_microsynth_freq)(unsigned short BoardNum, double Frequency);
	typedef void (*DLL_SelectAdcChannels)(unsigned short BoardNum, unsigned int Channels);
	typedef void (* DLL_SelectTrigger)(unsigned short BoardNum, unsigned int TriggerType, unsigned int TriggerSlope, unsigned int TriggerCh);
	typedef void (* DLL_ConfigureWaveformTrigger)(unsigned short BoardNum, unsigned int Threshold, unsigned int Hysteresis);
	typedef void (*DLL_ConfigureSegmentedCapture)(unsigned short BoardNum, unsigned int CaptureCount, unsigned int CaptureDepth, unsigned int ClearAverager);
	typedef unsigned int (* DLL_ConfigureAverager)(unsigned short BoardNum, unsigned int NumAverages, unsigned int AveragerLength, unsigned int ClearSegmentedCapture);
	typedef void (* DLL_SetFiducialMarks)(unsigned short BoardNum, unsigned int Enable);
	typedef void (* DLL_SetPreTriggerMemory)(unsigned short BoardNum, unsigned int Value);
	typedef unsigned int (*DLL_IsTriggerEnabled)(unsigned short BoardNum);


	/**
	* @class uvAPI
	* uvAPI is implemented in uvAPI.cpp and uvAPI.h  It provides a wrapper to the DLL calls required to work with Ultraview data acquisition boards.
	* once uvAPI is constructed, all installed boards are controlled through uvAPI.  Specific board access uses the __BoardNum__ argument to the relevant functions 
	*
	*
	**/		

	class uvAPI
	{
	public:
		uvAPI();
		~uvAPI();

		/**
        * __bool setupBoard__(unsigned short __BoardNum__) Reads from ultra_config.dat, initializes and calibrates the board.
		* Should be called to set the board to a known state at the beginning of an application.
		* 
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
		* 
		* @returns true if __BoardNum__ is installed, false otherwise 
		*
		* Example Usage:
		* @code
		* uvAPI *uv = new uvAPI;
		* unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);
		* @endcode
		*
		**/		
        DLL_SetupBoard setupBoard;

		/**
        * __void SetupAcquire__(unsigned short __BoardNum__,unsigned int __NumBlocks__)  Sets up the board referred to by __BoardNum__ to acquire __NumBlocks__.  The function returns immediately.
		* 
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
        * @param NumBlocks is the is the number of 1Mbyte blocks (1 to 2^32) to acquire into on-board memory (circular queue).  If more than 8192 blocks are specified, then over-runs are possibe.
		* 
		* Example Usage:
		* @code
		* uvAPI *uv = new uvAPI;
		* unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);
		* unsigned int numBlocksToAcquire=8192;
        * uv->SetupAcquire(BoardNum,numBocks);  // acquire 8192 blocks into onboard memory
		* @endcode
		*
		**/		
        DLL_setupAcquire SetupAcquire;

		/**
        * __int X_MemAlloc__(void ** __buf_addr__, size_t __buf_size__)  Special malloc for DMA page aligned memory allocation.
		* 
		* @param buf_addr is is the returned memory buffer for subsequent DMA operations
		* @param buf_size Must be fixed to 1Mbyte (1024 * 1024)
		* 
		* @returns 0 on success,  1 on failure.
		*
		* Example Usage:
		* @code
		* #define DIG_BLOCK_SIZE 1024 * 1024
		*
		* uvAPI *uv = new uvAPI;
		* unsigned char * sysMem = NULL;
        * uv->X_MemAlloc((void**)&sysMem, DIG_BLOCK_SIZE);
		* if (!sysMem)
		* {
		*	std::cout << "failed to allocate block buffer" << std::endl;
		*	return  -1;
		* }
		* @endcode
		*
		**/		
		DLL_x_MemAlloc X_MemAlloc;

		/**
        * __int X_Read__(unsigned short __BoardNum__, void * __sysMem__, DWORD __rd_size__);  Read 1 block of data.
		* 
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
		* @param sysMem is a buffer allocated using x_MemAlloc()
		* @param rd_size Must be fixed to 1Mbyte (1024 * 1024)
		* 
		* @returns read status.
		*
        * @see uv_acquire.cpp for usage example
		*
		**/		
		DLL_x_Read X_Read;

		DLL_x_Write	X_Write;

		DLL_x_CreateFile X_CreateFile;

		DLL_x_OpenFile X_OpenFile;

		DLL_x_Close X_Close;

		/**
        * __void X_FreeMem__(void *buf);  Free memory allocated by x_MemAlloc
		* 
		* @param buf is the previously allocated memory.
		*
        * @see acquire.cpp  for usage example
		*
		**/		
		DLL_x_FreeMem X_FreeMem;

		/**
		* __void selClock__(unsigned short __BoardNum__,unsigned int __Enable__)  Selects between internal or external clocks.
		* 
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
		* @param Enable = 1 for internal clock, 0 for external clock.
		* 
		* Example Usage:
		* @code
		* uvAPI *uv = new uvAPI;
		* unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);  // internal clock is selected inside setupBoard()
		* unsigned int Enable=0;
		* uv->SetInternalClockEnable(BoardNum,Enable);  // Select external clock
		* @endcode
		*
		**/		
		DLL_SetInternalClockEnable selClock;

		/**
		* __unsigned int getNumChannels__(unsigned short __BoardNum__)  returns the number ADC channels the board is using.  Needed to extract sample data from an acquired buffer of data (getSample16() )
		* 
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
		* 
		* Example Usage:
		* @code
		* uvAPI *uv = new uvAPI;
		* unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);  // internal clock is selected inside setupBoard()
		* unsigned int numChannels = uv->getNumChannels(BoardNum);
		* @endcode
		*
		**/		
        DLL_getNumChannels GetNumChannels;

		/**
		* __unsigned int getAllChannels__(unsigned short __BoardNum__)  returns the number of available channels on the ADC.
		* 
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
		* 
		* Example Usage:
		* @code
		* uvAPI *uv = new uvAPI;
		* unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);  // internal clock is selected inside setupBoard()
		* unsigned int numChannels = uv->getAllChannels(BoardNum);
		* @endcode
		*
		**/		
        DLL_getAllChannels GetAllChannels;

		/**
		* __unsigned int getAdcClockFreq__(unsigned short __BoardNum__)  returns the ADC clock frequency in MHz that the board is running.
		* 
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
		* 
		* Example Usage:
		* @code
		* uvAPI *uv = new uvAPI;
		* unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);  // internal clock is selected inside setupBoard()
		* unsigned int adcClockFreq = uv->AdcClockGetFreq(BoardNum);
		* @endcode
		*
		**/		
		DLL_AdcClockGetFreq getAdcClockFreq;

		/**
		* __unsigned int ADC12D2000_Channel_Mode_Select__(unsigned short __BoardNum__, unsigned int __Chan_mode__, unsigned int __Chan_select__, unsigned int __Calibrate__ )  returns the channel data for the given sample_num.
		* 
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
		* @param Chan_mode : \n
		* TWO_CH_MODE = 2\n
		* ONE_CH_MODE = 1\n
		* @param Chan_select : \n
		* DESCLKIQ_MODE = 3 _both inputs externally connected_\n
		* DESIQ_MODE = 2 _both inputs externally connected_\n
		* DESQ = 1 ain0\n
		* DESI = 0 ain1	indicates which consecutive sample to extract.\n
		* @param Calibrate  is 1 for perform ADC calibration, 0 to inhibit ADC calibration.
		* 
		* Example Usage:
		* @code
		* uvAPI *uv = new uvAPI;
		* unsigned char * sysMem = NULL;
		* uv->x_MemAlloc((void**)&sysMem, DIG_BLOCK_SIZE);
		* unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);
		* uv->ADC12D2000_Channel_Mode_Select(BoardNum,1,0,1);
		* unsigned int numBlocksToAcquire=1;
		* unsigned int numChannels = uv->getNumChannels(BoardNum);
        * uv->SetupAcquire(BoardNum,numBocks);
		* uv->x_Read(BoardNum,sysMem,DIG_BLOCK_SIZE);
		* for (size_t i=0;i<100;i++)
		* {
		*	std::cout << "sampleNum " << i;
		*	for ( unsigned int j=0;j<numChannels;j++)
		*	{
		*		unsigned int val = uv->getSample16(numChannels,  sysMem, i, j);
		*		std::cout << " ch" << j << " = " << val;
		*	}
		*	std::cout << std::end;
		* }
		* @endcode
		*
		**/		

        DLL_SelectAdcChannels selectAdcChannels;

		DLL_DllApiGetOverruns	DllApiGetOverruns;

        DLL_SetAdcDecimation	setAdcDecimation;

        DLL_SelectTrigger selectTrigger;

        DLL_SetPreTriggerMemory setPreTriggerMemory;

        DLL_ConfigureWaveformTrigger configureWaveformTrigger;

        DLL_ConfigureSegmentedCapture configureSegmentedCapture;

        DLL_ConfigureAverager configureAverager;

        DLL_SetFiducialMarks setFiducialMarks;

		DLL_SetAcquireDisableInvert		SetTTLInvert;

		DLL_SetSetupDoneBit setSetupDoneBit;

		DLL_SET_CAPTURE_COUNT	SetCaptureCount;

		DLL_SET_CAPTURE_DEPTH	SetCaptureDepth;

		// 12 bit only
		DLL_SET_ECL_TRIGGER_DELAY	SetECLTriggerDelay;

		DLL_ECLTriggerEnable	SetECLTriggerEnable;

		DLL_ADC12D2000_GetOneChanModeValue  ADC12D2000_GetOneChanModeValue;

		DLL_getOverruns getOverruns;

		DLL_is_ISLA216P IS_ISLA216P;

		DLL_is_AD5474 IS_AD5474;
		DLL_has_microsynth has_microsynth;

		DLL_microsynth_freq microsynth_freq;

		DLL_is_adc12d2000 IS_adc12d2000;

		DLL_ADC12D2000_Channel_Mode_Select ADC12D2000_Channel_Mode;

        DLL_IsTriggerEnabled isTriggerEnabled;

		/**
		* __unsigned int getSample16__(unsigned int __numChannels__,  unsigned char __*buf__, size_t __sample_num__, unsigned int __channel__ )  returns the channel data for the given sample_num.
		* 
		* @param numChannels  indicates how the __buf's__ data is organized.  typically this is 1,2, or 4 (AD16-250x4)
		* @param buf  is a pointer to the data from which to extract a sample value.
		* @param sample_num  indicates which consecutive sample to extract.
		* @param channel  indicates which channel's data to extract.
		* 
		* Example Usage:
		* @code
		* uvAPI *uv = new uvAPI;
		* unsigned char * sysMem = NULL;
		* uv->x_MemAlloc((void**)&sysMem, DIG_BLOCK_SIZE);
		* unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);
		* unsigned int numBlocksToAcquire=1;
		* unsigned int numChannels = uv->getNumChannels(BoardNum);
        * uv->SetupAcquire(BoardNum,numBocks);
		* uv->x_Read(BoardNum,sysMem,DIG_BLOCK_SIZE);
		* for (size_t i=0;i<100;i++)
		* {
		*	std::cout << "sampleNum " << i;
		*	for ( unsigned int j=0;j<numChannels;j++)
		*	{
		*		unsigned int val = uv->getSample16(numChannels,  sysMem, i, j);
		*		std::cout << " ch" << j << " = " << val;
		*	}
		*	std::cout << std::end;
		* }
		* @endcode
		*
		**/		
		unsigned int getSample16(unsigned int numChannels,  unsigned char *buf, size_t sample_num, unsigned int channel );
		unsigned int getSample12(unsigned int numChannels,  unsigned char *buf, size_t sample_num, unsigned int channel );


		unsigned char getSample8(unsigned int numChannels,  unsigned char *buf, size_t sample_num, unsigned int channel );


	private:
		HINSTANCE dllHandle; 
		bool	libIsLoaded;

			int numDevices;
			int serialNum[MAX_DEVICES];
			unsigned int adcResolution[MAX_DEVICES];

	};

#else // Linux code below









#define  DWORD size_t
#include "DllLibMain.h"
#include <iostream>
#include <stdint.h>


extern CDllDriverApi *pAPI;
extern config_struct dev_config[MAX_DEVICES];  // Configuration data structure


/**
* @class uvAPI
* For Windows:
*
* uvAPI is implemented in uvAPI.cpp and uvAPI.h  It provides a wrapper to the DLL calls required to work with Ultraview data acquisition boards.
* once uvAPI is constructed, all installed boards are controlled through uvAPI.  Specific board access uses the __BoardNum__ argument to the relevant functions
*
* For LINUX:
*
* uvAPI is implemented in uvAPI.h  It provides a wrapper to the library calls required to work with Ultraview data acquisition boards.
* once uvAPI is constructed, all installed boards are controlled through uvAPI.  Specific board access uses the __BoardNum__ argument to the relevant functions
*
**/

class uvAPI {

	public:
		uvAPI()
		{
			pAPI = new CDllDriverApi;
			pAPI->ApiInitialize();
			numDevices = pAPI->ApiGetNumDevices();
			for (int i=0;i<numDevices;i++)
			{
                devHandle[i] = pAPI->m_AllDeviceHandles[i];
				serialNum[i] = pAPI->ApiGetSerialNumber(i);
				GetBoardConfigInfo(i);
				adcResolution[i] = dev_config[i].adc_res;
				std::cout << std::endl << i << " adc resolution " << adcResolution[i] << std::endl;
			}
			std::cout << "Loaded uvAPI" << std::endl;
		}
		~uvAPI()
		{
			delete pAPI;
			std::cout << "Un-Loaded uvAPI" << std::endl;
		}


		/**
        * Reads from ultra_config.dat, initializes and calibrates the board.
		* Should be called to set the board to a known state at the beginning of an application.
        * __Note:__ call setSetupDoneBit() before calling setupBoard() to force a full calibration and setup.
        * The first time the board is operated, the setupDone bit is false, and the board undergoes a complete calibration when setupBoard() is called.
        * Subsequently, only mimimal configuration is performed.
        *
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
		*
		* @returns true if __BoardNum__ is installed, false otherwise
		*
		* Example Usage:
		* @code
		* uvAPI *uv = new uvAPI;
		* unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);
		* @endcode
		*
		**/
		bool setupBoard(unsigned short BoardNum)
		{
			return SetupBoard(BoardNum);
		}


        unsigned int GetAllChannels(unsigned short BoardNum)
        {
            return getAllChannels(BoardNum);
        }

		/**
        * Sets up the board referred to by __BoardNum__ to acquire __NumBlocks__.
        *
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
        * @param NumBlocks is the is the number of 1Mbyte blocks (1 to 2^32) to acquire into on-board memory (circular queue).  If more than 8192 blocks are specified, then over-runs are possibe.
        *
		* Example Usage:
		* @code
		* uvAPI *uv = new uvAPI;
		* unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);
		* unsigned int numBlocksToAcquire=8192;
        * uv->SetupAcquire(BoardNum,numBocks);  // acquire 8192 blocks into onboard memory
		* @endcode
		*
		**/
		void SetupAcquire(unsigned short BoardNum,unsigned int NumBlocks)
		{
			setupAcquire(BoardNum, NumBlocks);
		}


		/**
        * Special malloc for DMA page aligned memory allocation.
		*
		* @param buf_addr is is the returned memory buffer for subsequent DMA operations
		* @param buf_size Must be fixed to 1Mbyte (1024 * 1024)
		*
		* @returns 0 on success,  1 on failure.
		*
		* Example Usage:
		* @code
		* #define DIG_BLOCK_SIZE 1024 * 1024
		*
		* uvAPI *uv = new uvAPI;
		* unsigned char * sysMem = NULL;
        * uv->X_MemAlloc((void**)&sysMem, DIG_BLOCK_SIZE);
		* if (!sysMem)
		* {
		*	std::cout << "failed to allocate block buffer" << std::endl;
		*	return  -1;
		* }
		* @endcode
		*
		**/
        int X_MemAlloc(void ** buf_addr, size_t buf_size)
		{
			return x_MemAlloc(buf_addr,buf_size);
		}

		/**
        * Read 1 block of data.
		*
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
        * @param sysMem is a buffer allocated using X_MemAlloc()
		* @param rd_size Must be fixed to 1Mbyte (1024 * 1024)
		*
		* @returns read status.
		*
        * @see acquire.cpp for usage example
		*
		**/
        int X_Read(unsigned short BoardNum, void * sysMem, DWORD rd_size)
		{
		   return  x_Read(BoardNum, sysMem, rd_size);
		}

		/**
        * Free memory allocated by X_MemAlloc
		*
		* @param buf is the previously allocated memory.
		*
        * @see acquire.cpp  for usage example
		*
		**/
        void X_FreeMem(void * buf)
		{
			x_FreeMem(buf);
		}

        /**
        * Creates a file. Existing files will be over-written.
        *
        * @param filename is the filename to create.
        *
        * @returns HANDLE to opened file, .
        *
        * @see acquire.cpp  for usage example
        *
        **/
        HANDLE X_CreateFile(char * filename)
		{
            return x_CreateFile(filename);
		}

        /**
        * Writes to a file previously opened by X_CreateFile().
        *
        * @param fd is the HANDLE to write to.
        * @param sysMem is the data to write.
        * @param wr_size size of write
        *
        * @see acquire.cpp  for usage example
        *
        **/

        int X_Write(HANDLE fd, void * sysMem, DWORD wr_size)
		{
            return x_Write(fd, sysMem, wr_size);
		}

        /**
        * Closes a file, previously opened by X_CreateFile().
        *
        * @param fd is the HANDLE to close.
        *
        * @see acquire.cpp  for usage example
        *
        **/
        void X_Close(HANDLE fd)
		{
			x_Close(fd);
		}

        /**
        * Returns the number of over-runs.
        *
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
        *
        * @returns The number of over-runs since setupAcquire().
        *
        * @see acquire.cpp  for usage example
        *
        **/
		unsigned long getOverruns(unsigned short BoardNum)
		{
			return pAPI->ApiGetOverruns(BoardNum);
		}

		/**
        * Selects between internal or external clocks.
		*
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
        * @param Enable = CLOCK_INTERNAL for internal clock, CLOCK_EXTERNAL for external clock.
		*
		* Example Usage:
        *
		* @code
		* uvAPI *uv = new uvAPI;
		* unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);  // internal clock is selected inside setupBoard()
        * unsigned int Enable=CLOCK_EXTERNAL;
        * uv->selClock(BoardNum,Enable);  // Select external clock
		* @endcode
		*
		**/
		void selClock (unsigned short BoardNum, unsigned int Enable)
		{
			SetInternalClockEnable (BoardNum,Enable);
		}


        /**
        * Sets Delay after ECL trigger goes from zero to one before data begins acquiring.  Applies to AD12 and AD8 only.
        *
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
        * @param Value = Delay in DCLK cycles (ADC outputs data four 12 bit words/DCLK cycle)  In single channel mode there are 4 samples per DCLK.  In two channel mode, there are 2 samples/DCLK.
        *
        * Example Usage:
        *
        * @code
        * uvAPI *uv = new uvAPI;
        * unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);
        * unsigned int Value=137;
        * uv->SetECLTriggerDelay(BoardNum,Value);
        * @endcode
        *
        **/
		void SetECLTriggerDelay(unsigned short BoardNum, unsigned int Value)
		{
			SET_ECL_TRIGGER_DELAY( BoardNum, Value);
		}

        /**
        * Enables (or disables) ECL trigger mode. Applies to AD12 and AD8 only.
        *
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
        * @param Enable = 1 Enables ECLtrigger.  The board will be forced into reset, awaiting an external Trigger
        *
        * Example Usage:
        *
        * @code
        * uvAPI *uv = new uvAPI;
        * unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);
        * unsigned int Value=137;
        * uv->SetECLTriggerDelay(BoardNum,Value);
        * unsigned int Enable = 1;
        * uv->SetECLTriggerEnable(BoardNum,Enable);  // The board will now await ECL trigger.
        * @endcode
        *
        **/
        void SetECLTriggerEnable (unsigned short BoardNum, unsigned int Enable)
		{
			ECLTriggerEnable(BoardNum, Enable);
		}

        /**
        * Sets ADC decimation (return every n samples). Applies to AD12 and AD8 only.  Future Firmware on AD14 and AD16 boards may support Decimation
        *
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
        * @param adc_deci_value for AD12 must be  <1,2,4, or 8>  for AD8 must be <1,2,4,8, or 16>
        *
        * Example Usage:
        *
        * @code
        * uvAPI *uv = new uvAPI;
        * unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);
        * unsigned int Value=4;
        * uv->setAdcDecimation(BoardNum,Value);
        * @endcode
        *
        **/
        void setAdcDecimation(unsigned short BoardNum, unsigned int adc_deci_value)
		{
			int calc_deci_val=1;
			int pwr_2 = 1;
			while (pwr_2 < adc_deci_value)
			{
				pwr_2 *= 2;
				calc_deci_val++;
			}
			if ((calc_deci_val > AD12_DECI_VAL_LIMIT) || (pwr_2 != adc_deci_value))
			{
				std::cout << "board " << BoardNum << "setAdcDecimation " << adc_deci_value << " out of range, must be in \"1,2,4,8\"" << std::endl;
			}
			else
			{
				// Firmware uses n such that decimation is 2^(n-1)  ie for a decimation of 1 (none) n= 1, for decimation of 2, n= 2, for 4 n=3...
				SetAdcDecimation(BoardNum,calc_deci_val);
			}
		}

        /**
        * Sets number of SetCaptureCount() frames to save before returning to normal operation.  Applies to AD14 and AD16 only. Future Firmware on AD8 and AD12 boards may support SetCaptureCount()
        *
        * note:  Use configureSegmentedCapture() instead.
        *
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
        * @param count 1 to 2^32 frames. Zero turns off capture count feature.  Note:  if SetCaptureCount() is set to zero, then this feature is off.
        *
        * Example Usage:
        *
        * @code
        * uvAPI *uv = new uvAPI;
        * unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);
        * unsigned int captureDepth=1100;
        * unsigned int captureCount=13;
        * uv->SetCaptureDepth(BoardNum,captureDepth);  // 1100 samples per trigger.
        * uv->SetCaptureCount(BoardNum,captureCount);  // 13 frames of 1100 samples, followed by normal operation.
        * {...}
        * captureDepth=1100;
        * captureCount=0;
        * uv->SetCaptureDepth(BoardNum,captureDepth);  // 1100 samples per trigger.
        * uv->SetCaptureCount(BoardNum,captureCount);  // frames of 1100 samples, until acquisition completes.
        * {...}
        * captureDepth=0;
        * captureCount=0;
        * uv->SetCaptureDepth(BoardNum,captureDepth);  // Turns off captureDepth feature.  After trigger, data is returned continuously.
        * uv->SetCaptureCount(BoardNum,captureCount);  // Feature is off.
        *
        * @endcode
        *
        **/
        void SetCaptureCount(unsigned short BoardNum, unsigned int count)
		{
			SET_CAPTURE_COUNT( BoardNum, count);
		}

        /**
        * Sets number samples per trigger event.  Applies to AD14 and AD16 only. Future Firmware on AD8 and AD12 boards may support SetCaptureDepth()
        *
        * note:  Use configureSegmentedCapture() instead.
        *
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
        * @param size 1 to 2^32 samples per trigger. Zero turns off capture depth feature.
        *
        * Example Usage:
        *
        * @code
        * uvAPI *uv = new uvAPI;
        * unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);
        * unsigned int captureDepth=1100;
        * unsigned int captureCount=13;
        * uv->SetCaptureDepth(BoardNum,captureDepth);  // 1100 samples per trigger.
        * uv->SetCaptureCount(BoardNum,captureCount);  // 13 frames of 1100 samples, followed by normal operation.
        * {...}
        * captureDepth=1100;
        * captureCount=0;
        * uv->SetCaptureDepth(BoardNum,captureDepth);  // 1100 samples per trigger.
        * uv->SetCaptureCount(BoardNum,captureCount);  // frames of 1100 samples, until acquisition completes.
        * {...}
        * captureDepth=0;
        * captureCount=0;
        * uv->SetCaptureDepth(BoardNum,captureDepth);  // Turns off captureDepth feature.  After trigger, data is returned continuously.
        * uv->SetCaptureCount(BoardNum,captureCount);  // Feature is off.
        *
        * @endcode
        *
        **/
        void SetCaptureDepth (unsigned short BoardNum, unsigned int size)
		{
			SET_CAPTURE_DEPTH(BoardNum, size);
		}


        /**
        * Applies to selective recording input. In normal operation (no call to SetTTLInvert(), or SetTTLInvert(BoardNum,0)), a zero on the selective recording input allows normal operation.  A one on the selecive recording input disables operation.
        *
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
        * @param Invert 1 reverses operation of selective recording 0 is normal operation.
        *
        * Example Usage:
        *
        * @code
        * uvAPI *uv = new uvAPI;
        * unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);
        * uv->SetTTLInvert(BoardNum,1);  //Board will operate while a one is present on selective recording input.
        * @endcode
        *
        **/
        void SetTTLInvert(unsigned short BoardNum, unsigned int Invert)
		{
			SetAcquireDisableInvert(BoardNum, Invert);
		}

        /**
        * Sets up AD12 for either 1 or 2 channel mode operation. Calibrates ADC if mode changed since last call, or calibrate = 1
        *
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
        * @param Chan_mode < TWO_CH_MODE, ONE_CH_MODE >
        * @param Chan_select < DESQ_MODE, DESI_MODE, DESIQ_MODE, DESCLKIQ_MODE >  If the board is in ONE_CH_MODE, Chan_select selects the appropriate configuration.  Use DESI and DESQ, or see the ADC12D2000RF data sheet. for the other modes.
        * @param Calibrate <0,1>  1 forces calibrate even if mode did not change.  0 calibrates only if mode changed
        *
        * Example Usage:
        *
        * @code
        * uvAPI *uv = new uvAPI;
        * unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);
        * uv->ADC12D2000_Channel_Mode(BoardNum,ONE_CH_MODE,DESI_MODE,0);  //One channel mode DESI selected
        * @endcode
        *
        **/
        void ADC12D2000_Channel_Mode(unsigned short BoardNum, unsigned int Chan_mode, unsigned int Chan_select, unsigned int Calibrate)
		{
			ADC12D2000_Channel_Mode_Select(BoardNum,  Chan_mode,  Chan_select,Calibrate);
		}

//		unsigned int ADC12D2000_GetOneChanModeValue(unsigned short BoardNum)
//		{

//		}

        /**
        * Returns the number of channels being used.  Used to help decode buffer data.  See getSample<nn> helper functions.
        *
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
        *
        * @returns The number of channels in the buffer data.
        *
        * @see  glitchTest.cpp  glitchTest::testForGlitches()
        *
        **/
        unsigned int GetNumChannels(unsigned short BoardNum)
		{
			return getNumChannels(BoardNum);
		}

        /**
        * Returns the effective sampling frequency.  This is a rough measurement based on the PCIe ref clock.
        *
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
        *
        * @returns The Sampling clock frequency in MHz.
        *
        **/
        unsigned int getAdcClockFreq(unsigned short BoardNum)
		{
			return AdcClockGetFreq(BoardNum);
		}

        /**
        * Returns the requested sample from the buffer.  For 16 bit boards
        *
        * @param numChannels number of channels of data in the buffer. See GetNumChannels();
        * @param buf is buffer filled by X_Read().
        * @param sample_num is the sequence number of the sample desired.
        * @param channel is the channel (0 to (numChannels - 1)) desired.
        *
        * @returns sample val in buffer
        *
        **/
        unsigned int getSample16(unsigned int numChannels,  unsigned char *buf, size_t sample_num, unsigned int channel )
		{
			uint16_t  *uint16ptr = (uint16_t  *)buf;

			unsigned int val = uint16ptr[sample_num * numChannels + channel];

			return val;
		}

        /**
        * Returns the requested sample from the buffer.  For 12 bit boards
        *
        * @param numChannels number of channels of data in the buffer. See GetNumChannels();
        * @param buf is buffer filled by X_Read().
        * @param sample_num is the sequence number of the sample desired.
        * @param channel is the channel (0 to (numChannels - 1)) desired.
        *
        * @returns sample val in buffer
        *
        **/
        unsigned int getSample12(unsigned int numChannels,  unsigned char *buf, size_t sample_num, unsigned int channel )
		{
			uint16_t  *uint16ptr = (uint16_t  *)buf;

			unsigned int val = uint16ptr[sample_num * numChannels + channel] & 0x0fff;

			return val;
		}

        /**
        * Returns the requested sample from the buffer.  For 8 bit boards
        *
        * @param numChannels number of channels of data in the buffer. See GetNumChannels();
        * @param buf is buffer filled by X_Read().
        * @param sample_num is the sequence number of the sample desired.
        * @param channel is the channel (0 to (numChannels - 1)) desired.
        *
        * @returns sample val in buffer
        *
        **/
        unsigned char getSample8(unsigned int numChannels, unsigned char *buf, size_t sample_num, unsigned int channel )
		{
			unsigned char *uint8ptr = (unsigned char *)buf;

			unsigned char val = uint8ptr[sample_num * numChannels + channel];

			return val;
		}

        /**
        * Returns the requested averaged sample from the buffer.  For 14 and 16 bit boards.  Future FW may support 8 and or 12 bit boards
        *
        * @param numChannels number of channels of data in the buffer. See GetNumChannels();
        * @param buf is buffer filled by X_Read().
        * @param sample_num is the sequence number of the sample desired.
        * @param channel is the channel (0 to (numChannels - 1)) desired.
        *
        * @returns sample val in buffer
        *
        **/
        unsigned int getAveragedSample(unsigned int numChannels,  unsigned char *buf, size_t sample_num, unsigned int channel)
        {
            uint32_t  *uint32ptr = (uint32_t  *)buf;

            unsigned int val = uint32ptr[sample_num * numChannels + channel];

            return val;

        }

        /**
        * Sets up AD14 and AD16 boards for selected 1,2,or 4 channel operation
        *
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
        * @param Channels bitwise OR of deired channels in <IN0,IN1,IN2,IN3>
        *
        * Example Usage:
        *
        * @code
        * uvAPI *uv = new uvAPI;
        * unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);
        * uv->selectAdcChannels(BoardNum,(IN0 | IN3));  //two channels,  IN0 and IN3
        * @endcode
        *
        **/
        void selectAdcChannels(unsigned short BoardNum, unsigned int Channels)
		{
			SelectAdcChannels(BoardNum,Channels);
		}

        /**
        * Sets up AD14 and AD16 boards trigger operation.  Future FW may enable waveform triggering on AD8 and AD12 boards
        *
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
        * @param TriggerType <NO_TRIGGER,WAVEFORM_TRIGGER,SYNC_SELECTIVE_RECORDING,HETERODYNE,TTL_TRIGGER_EDGE>
        * @param TriggerSlope <FALLING_EDGE,RISING_EDGE>
        * @param TriggerCh  channel to trigger from <IN0,IN1,IN2,IN3>
        *
        * Example Usage:
        *
        * @code
        * uvAPI *uv = new uvAPI;
        * unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);
        * unsigned int threshold = 32768;
        * unsigned int hystereis = 200;
        * uv->configureWaveformTrigger(BoardNum,threshold,hystereis);
        * uv->selectTrigger(BoardNum,WAVEFORM_TRIGGER,RISING_EDGE,IN0);  //Waveform triggereing on input IN0 as configured by
        * @endcode
        *
        **/
        void selectTrigger(unsigned short BoardNum, unsigned int TriggerType, unsigned int TriggerSlope, unsigned int TriggerCh)
		{
			SelectTrigger(BoardNum,TriggerType,TriggerSlope, TriggerCh);
		}

        /**
        * Sets up AD14 and AD16 boards Waveform triggering.  Future FW may enable waveform triggering on AD8 and AD12 boards
        *
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
        * @param Threshold 0 to ADC full scale (65536 for AD16)
        * @param Hysteresis 0 to ADC full scale.  Typically this should be a few times the noise on your signal.
        *
        * Example Usage:
        *
        * @code
        * uvAPI *uv = new uvAPI;
        * unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);
        * unsigned int threshold = 32768;
        * unsigned int hystereis = 200;
        * uv->configureWaveformTrigger(BoardNum,threshold,hystereis);
        * uv->selectTrigger(BoardNum,WAVEFORM_TRIGGER,RISING_EDGE,IN0);  //Waveform triggereing on input IN0
        * @endcode
        *
        **/
        void configureWaveformTrigger(unsigned short BoardNum, unsigned int Threshold, unsigned int Hysteresis)
		{
			ConfigureWaveformTrigger(BoardNum,Threshold,Hysteresis);
		}

        /**
        * Sets up AD14 and AD16 boards segmented capture operation.  Future FW may enable waveform triggering on AD8 and AD12 boards
        *
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
        * @param CaptureCount 1 to 2^32 frames. Zero turns off capture count feature.
        * @param CaptureDepth 1 to 2^32 samples per trigger. Zero turns off segmented capture.
        * @param ClearAverager <0,1>  A one turns off the averager (both segmented capture and averging cannot run at the same time)
        *
        * Example Usage:
        *
        * @code
        * uvAPI *uv = new uvAPI;
        * unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);
        * unsigned int threshold = 32768;
        * unsigned int hystereis = 200;
        * uv->configureWaveformTrigger(BoardNum,threshold,hystereis);
        * uv->selectTrigger(BoardNum,WAVEFORM_TRIGGER,RISING_EDGE,IN0);  //Waveform triggereing on input IN0
        * uv->configureSegmentedCapture(BoardNum,1100,33,1);  // 1100 sample segments, 33 times then continuous, clear averager
        * @endcode
        *
        **/
        void configureSegmentedCapture(unsigned short BoardNum, unsigned int CaptureCount, unsigned int CaptureDepth, unsigned int ClearAverager)
		{
			ConfigureSegmentedCapture(BoardNum,CaptureCount,CaptureDepth,ClearAverager);
		}

        /**
        * Sets up AD14 and AD16 boards averager (really triggered accumulator) operation.  Future FW may enable waveform triggering on AD8 and AD12 boards
        *
        * Note: Data becomes 32 bit when averager is on. use getAveragedSample() to access
        *
        * Note: Minimum data transfer remains 1MB or 256k samples of averaged data.  Several Averager segments will be acquired per block transferred.  Very deep averaging can make the system appear unresponsive.
        *
        * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
        * @param NumAverages 0 to (2^16-1).  0 is off.  1 is flow through averager.
        * @param AveragerLength  2^n ie 8,16, .. 128, ... 131072.  Note Averager depth * numChannels = 131072 max for a 155T board.
        * @param ClearSegmentedCapture <0,1>  A one turns off segmented capture (both segmented capture and averging cannot run at the same time)
        *
        * Example Usage:
        *
        * @code
        * uvAPI *uv = new uvAPI;
        * unsigned short BoardNum = 0;
        * bool success = uv->setupBoard(BoardNum);
        * unsigned int threshold = 32768;
        * unsigned int hystereis = 200;
        * uv->configureWaveformTrigger(BoardNum,threshold,hystereis);
        * uv->selectTrigger(BoardNum,WAVEFORM_TRIGGER,RISING_EDGE,IN0);  //Waveform triggereing on input IN0
        * uv->configureAverager(BoardNum,30,8192,1);  // 30 averages into an 8192 deep buffer, clear averager
        * @endcode
        *
        **/
         unsigned int configureAverager(unsigned short BoardNum, unsigned int NumAverages, unsigned int AveragerLength, unsigned int ClearSegmentedCapture)
		 {
			 return ConfigureAverager(BoardNum,NumAverages,AveragerLength,ClearSegmentedCapture);
		 }

         /**
         * Places marks in data at trigger point (sets sample value to +FS then -FS)
         *
         * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
         * @param Enable 1 Turns on Fiducial marks.  0 turns them off.
         *
         * Example Usage:
         *
         * @code
         * uvAPI *uv = new uvAPI;
         * unsigned short BoardNum = 0;
         * bool success = uv->setupBoard(BoardNum);
         * uv->configureWaveformTrigger(BoardNum,threshold,hystereis);
         * uv->selectTrigger(BoardNum,SYNC_SELECTIVE_RECORDING,RISING_EDGE,0); // records while selective recording input is valid
         * uv->setFiducialMarks(BoardNum,1);  //turn on fiducial marks any time selective recording goes from not-valid to valid
         * @endcode
         *
         **/
         void setFiducialMarks(unsigned short BoardNum, unsigned int Enable)
		 {
			 SetFiducialMarks(BoardNum, Enable);
		 }

         /**
         * Sets the number of samples prior to trigger to be recorded
         *
         * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
         * @param Value <0 to 4095>.
         *
         * Example Usage:
         *
         * @code
         * uvAPI *uv = new uvAPI;
         * unsigned short BoardNum = 0;
         * bool success = uv->setupBoard(BoardNum);
         * uv->configureWaveformTrigger(BoardNum,threshold,hystereis);
         * uv->selectTrigger(BoardNum,SYNC_SELECTIVE_RECORDING,RISING_EDGE,0); // records while selective recording input is valid
         * uv->setPreTriggerMemory(BoardNum,300);  //set 300 samples of pre-trigger memory
         * @endcode
         *
         **/
         void setPreTriggerMemory(unsigned short BoardNum, unsigned int Value)
		 {
			 SetPreTriggerMemory(BoardNum, Value);
		 }

         /**
         * Clears setupDone flag, forcing subsequent calls to setupBoard() to perform full calibration.
         *
         * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
         * @param Enable = 0 To clear setupDone flag.
         *
         * Example Usage:
         *
         * @code
         * uvAPI *uv = new uvAPI;
         * unsigned short BoardNum = 0;
         * unsigned int Enable=0;
         * uv->setSetupDoneBit(BoardNum,Enable);  //Force full calibration by setupBaord()
         * bool success = uv->setupBoard(BoardNum);
         * @endcode
         *
         **/
         void setSetupDoneBit(unsigned short BoardNum, unsigned int enable)
		 {
			 SetSetupDoneBit(BoardNum,enable);
		 }

         /**
         * Returns whether the board is an AD16
         *
         * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
         *
         * @returns true if the board is an AD16
         *
         **/
         bool  IS_ISLA216P(unsigned short BoardNum)
		 {
			 return  is_ISLA216P(BoardNum);
		 }

         /**
         * Returns whether the board is an AD14
         *
         * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
         *
         * @returns true if the board is an AD14
         *
         **/
         bool  IS_AD5474(unsigned short BoardNum)
		 {
			 return  is_AD5474(BoardNum);
		 }

         /**
         * Returns whether the board is an AD12

         *
         * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
         *
         * @returns true if the board is an AD12
         *

         **/
         bool  IS_adc12d2000(unsigned short BoardNum)
		 {
			 return  is_adc12d2000(BoardNum);
		 }

		bool HAS_microsynth(unsigned short BoardNum)
		{
			return has_microsynth(BoardNum);
		}
		void MICROSYNTH_freq(unsigned short BoardNum, double Frequency)
		{
            FreqMicrosynth(BoardNum, Frequency);
		}
		
         /**
         * Returns the number of ultraview boards in the system
         *
         * @returns Number of boards in system
         *
         **/
         int getNumDevices()
		 {
			 return numDevices;
		 }

         /**
         * Returns the serial number of the chosen board
         *
         * note:  Depends on get_usercode.svf being in the active directory
         *
         * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
         *
         * @returns serial number if BoardNum is valid (and get_usercode.svf present), else -1.
         *
         **/
         int getSerialNumber(unsigned short BoardNum)
		 {
			 int serialNumber = -1;

			 if (BoardNum < numDevices)
				serialNumber = serialNum[BoardNum];
			 return serialNumber;
		 }

         /**
         * Returns the adc resolution of the chosen board
         *
         * note:  Uses ultraconfig.dat, in the active directory
         *
         * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
         *
         * @returns ADC resolution if BoardNum is valid else 0.
         *
         **/
         unsigned int getAdcRes(unsigned short BoardNum)
		 {
			 unsigned int adcRes = 0;

			 if (BoardNum < numDevices)
				adcRes = adcResolution[BoardNum];
			 return adcRes;
		 }


         /**
         * Returns the trigger mode of the chosen AD14 or AD16 board.
         *
         * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
         *
         * @returns 0 for NO_TRIGGER, 1 for WAVEFORM_TRIGGER, 2 for SYNC_SELECTIVE_RECORDING, 3 for HETERODYNE, 4 for TTL_TRIGGER_EDGE
         *
         **/
         unsigned int isTriggerEnabled(unsigned short BoardNum)
         {
             return IsTriggerEnabled(BoardNum);
         }

         /**
         * Returns the HANDLE of the selected board.
         *
         * @param BoardNum is the index (0-3) for an installed Ultraview Data Acquisition board.
         *
         * @returns board HANDLE
         *
         **/
         HANDLE getHandle(unsigned short BoardNum)
         {
             HANDLE  handle = -1;
             if (BoardNum < numDevices)
                handle = devHandle[BoardNum];
             return handle;
         }


		private:
			int numDevices;
			int serialNum[MAX_DEVICES];
			unsigned int adcResolution[MAX_DEVICES];
            HANDLE  devHandle[MAX_DEVICES];
	};

#endif

#endif // UVAPI_H

