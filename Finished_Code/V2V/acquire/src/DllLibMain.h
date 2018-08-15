/*
    Definitions for DllLibMain.cpp.
 */

#if !defined(_AD83000_FUNCS_H_)
#define _AD83000_FUNCS_H_


#define EXPORT_FCNS
#include "DllDriverApi.h"

#ifdef _WINDOWS
#include "shrhelp.h"
#include "Pcie5vDefines.h"
#include "AppDll.h"
#else
#define EXPORTED_FUNCTION
#include "Pcie5vDefines.h"
#include "AppDll.h"
#endif

#define FILE_HANDLE_TYPE             int
#define SAMPLE_TYPE                  unsigned int    // ADC samples read from any resolution board are converted to this type

//#define DLL_PRINT_EN        // Enable DLL console print messages (Windows Only)
#define DLL_MSG_LEN 250     // Do not exceed this length for a debug print message!




////////////////////////
// These are all uncessary as Linux now uses the API
#define ATTACH_TO_DEVICE                pAPI->ApiInitialize
#define GET_NUMDEVICES                  pAPI->ApiGetNumDevices
#define GET_SERIALNUMBER				pAPI->ApiGetSerialNumber
//TEMPORARY TILL LABVIEW UPDATED
#define SET_CURRENTDEVICE               pAPI->ApiSetCurrentDevice
#define GET_CURRENTDEVICEHANDLE			pAPI->GetCurrentDeviceHandle
#define SET_PREVENT_UNDEROVERRUNS       pAPI->ApiSetPreventUnderOverRuns
#define GET_DEVICE_STATUS               pAPI->ApiGetDeviceStatus
#define SET_PIO_REG                     pAPI->ApiSetPioRegister
#define GET_PIO_REG                     pAPI->ApiGetPioRegister
#define SET_SHADOW_REG                  pAPI->ApiSetShadowRegister
#define GET_SHADOW_REG                  pAPI->ApiGetShadowRegister
#define GET_OVERRUNS                    pAPI->ApiGetOverruns
#define GET_DRIVER_VERSION				pAPI->ApiGetDriverVersion
#define SET_CONTROL_REG                 pAPI->SetAD83000xReg  
#define RUNBOARD                        pAPI->RunBoard
#define SET_DA_DMA_BLOCKS_PER_BOARD     pAPI->SetNumBlocksPerBoard	
#define GET_BLOCKS_SYNTHESIZED          pAPI->GetBlocksSynthesized
#define SET_ARMED                       pAPI->SetArmed    


// This should be called DRIVER_API_HANDLE
// The user should never see the driver API object, so this is defined in DLL header file.
#define DEVICE_HANDLE                   CDllDriverApi * pAPI
#define PDEVICE_HANDLE                  CDllDriverApi **
#define DEVICE_CURBOARDHANDLE           pAPI->hCurrentDevice
#define SLEEP                           Sleep

////////////////////




// Platform specific mappings
#ifdef _WINDOWS

#else
// Under Linux DLLEXP is not needed, define this to be whitespace
#define DLLEXP 
#endif	// End platform specific mappings

//ADC12D2000 SPI ADRESSES
#define CONFIGURATION_REGISTER_1	0x0
#define Adc12d2000_CAL				15
#define Adc12d2000_DPS				14
#define Adc12d2000_OVS				13
#define Adc12d2000_TPM				12
#define Adc12d2000_PDI				11
#define Adc12d2000_PDQ				10
#define Adc12d2000_LFS				8
#define Adc12d2000_DES				7
#define Adc12d2000_DEQ				6
#define Adc12d2000_DIQ				5
#define Adc12d2000_2SC				4
#define Adc12d2000_TSE				3
#define Adc12d2000_SDR				2
#define	RESERVED_1					0x1
#define	I_CHANNEL_OFFSET			0x2
#define Adc12d2000_OM				0
#define Adc12d2000_OS				12
#define	I_CHANNEL_FSR				0x3
#define Adc12d2000_FM				0
#define	CALIBRATION_ADJUST			0x4
#define	CALIBRATION_VALUES			0x5
#define	Adc12d2000_SS				14
#define	BIAS_ADJUST					0x6
#define Adc12d2000_MPA				0
#define	DES_TIMING_ADJUST			0x7
#define Adc12d2000_DTA				9
#define	RESERVED_8					0x8
#define	RESERVED_9					0x9
#define	Q_CHANNEL_OFFSET			0xA
//#define Adc12d2000_OM				0
//#define Adc12d2000_OS				12
#define	Q_CHANNEL_FSR				0xB
//#define Adc12d2000_FM				0
#define	APERTURE_DELAY_COARSE		0xC
#define Adc12d2000_CAM				4
#define Adc12d2000_STA				3
#define Adc12d2000_DCC				2
#define	APERTURE_DELAY_FINE			0XD
#define Adc12d2000_FAM				10
#define Adc12d2000_SA				8
#define	CONFIGURATION_REGISTER_2	0xE
#define Adc12d2000_DCK					6
#define Adc12d2000_DR					0
#define	RESERVED_F					0xF

///** @brief Defines __Chan_mode__ values for AD12D2000 */
//#define TWO_CH_MODE	2
///** @brief Defines __Chan_mode__ values for AD12D2000 */
//#define	ONE_CH_MODE	1
///** @brief Defines __Chan_select__ values for AD12D2000 */
//#define DESCLKIQ_MODE 3
///** @brief Defines __Chan_select__ values for AD12D2000 */
//#define DESIQ_MODE 2
///** @brief Defines __Chan_select__ values for AD12D2000 */
//#define DESQ 1
///** @brief Defines __Chan_select__ values for AD12D2000 */
//#define DESI 0
///** @brief Defines __selClock__ values */
//#define CLOCK_INTERNAL 1
///** @brief Defines __selClock__ values */
//#define CLOCK_EXTERNAL 0

// Read and Write defines for makeADC12D2000
#define ADC12D2000_READ				1
#define	ADC12D2000_WRITE			0

// Bit Positions defines for makeADC12D2000
#define ADC12D2000_SPI_READWRITE_BIT_POS	23
#define ADC12D2000_SPI_RESERVED_23_BIT_POS	22
#define ADC12D2000_SPI_RESERVED_22_BIT_POS	21
#define ADC12D2000_SPI_ADDRESS_BIT_POS		17
#define ADC12D2000_SPI_RESERVED_16_BIT_POS	16
#define ADC12D2000_SPI_DATA_BIT_POS			0


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

// Implemented in DllLibCheckMem.cpp
void CheckMem(unsigned short BoardNum, int num_blocks, FILE_HANDLE_TYPE pFile, void *pBuffer);
void CheckMemWindows(unsigned short BoardNum, int num_blocks, const char * filename, void *pBuffer);

// Setup boards before acquisition/synthesis

/**
 * __bool SetupBoard__(unsigned short __BoardNum__) Reads from ultra_config.dat, initializes and calibrates the board.
 * Should be called to set the board to a known state at the beginning of an application.
 * Setupboard defaults the board configuration to:
 * Channel mode: 2
 * Internal clock: On
 * Decimation: 1 (none)
 * ECL trigger: 0 (off)
 * Capture count: 0
 * Capture depth: 0
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * 
 * @returns true if __BoardNum__ is installed, false otherwise 
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * x_Load(dll);
 * bool success = dll.hDllSetupBoard(BoardNum);
 * @endcode
 */	
EXPORTED_FUNCTION bool SetupBoard(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int getAllChannels( unsigned short BoardNum);

unsigned  readRegTen();

// Setup routines applicable to all boards
/**
 * __void MainBoardSetup__(unsigned short __BoardNum__) configures FPGA and DRAM settings, settings related to the mainboard.
 *
 * Does not need to be called by user.  __bool SetupBoard__(unsigned short __BoardNum__) includes this function. Not exported.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 *
 */
void MainBoardSetup(unsigned short BoardNum);

/**
 * __void DramIdelayShift__(unsigned short __BoardNum__, int __dqs_idelay_val__, int __dq_idelay_val__) sets DRAM DQS and DQS values.
 * DQS and DQ values read from ultra_config.dat.  __bool SetupBoard__(unsigned short __BoardNum__) includes this function.
 *
 * Does not need to be called by user unless recalibrating DRAM delay values.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param dqs_idelay_val is the DRAM DQS value. (0-15).
 * @param dq_idelay_val is the DRAM DQ value. (0-15).
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int dqs = 5, dq = 5;
 * x_Load(dll);
 * dll.hDllDramIdelayShift(BoardNum, dqs, dq);
 * @endcode
 */
EXPORTED_FUNCTION void DramIdelayShift(unsigned short BoardNum, int dqs_idelay_val, int dq_idelay_val);

/**
 * __void GetBoardConfigInfo__(unsigned short __BoardNum__) reads ultra_config.dat for ALL calibration values and populates __config_struct__ __dev_config[BoardNum]__.
 * __bool SetupBoard__(unsigned short __BoardNum__) includes this function.
 *
 * Does not need to be called by user.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param dqs_idelay_val is the DRAM DQS value. (0-15).
 * @param dq_idelay_val is the DRAM DQ value. (0-15).
 *
 * @returns true if ultra_config.dat is found, false otherwise
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * x_Load(dll);
 * bool success = dll.hDllGetBoardConfigInfo(BoardNum);
 * @endcode
 */
bool GetBoardConfigInfo(unsigned short BoardNum);

/**
 * __void AdcClockGetFreq__(unsigned short __BoardNum__) reads the ADC clock frequency.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 *
 * @returns ADC clock frequency.
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int adc_freq;
 * x_Load(dll);
 * adc_freq = dll.hDllAdcClockGetFreq(BoardNum);
 * @endcode
 */
EXPORTED_FUNCTION unsigned int AdcClockGetFreq(unsigned short BoardNum);

/**
 * __void CheckDeviceStatus__(unsigned short __BoardNum__) driver API call to query device for possible bus error. Prints statement if bus error found.
 *
 * Does not need to be called by user.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 *
 */
void CheckDeviceStatus(unsigned short BoardNum);

/**
 * __unsigned short SearchConfigData__(char * search_term, int term_length, int value_length, int value_radix, unsigned int * config_data, int start_index, char *config_array, unsigned long FileSize)
 * Low-level function to search file for string constant "xxx=" and returns value. Used with __void GetBoardConfigInfo__(unsigned short __BoardNum__) to collect calibration info.
 *
 * @param char * search_term is the string to search for in file
 * @param char * term_length is the length of search_term
 * @param char * value_length is the total digits for the calibration data.
 * @param char * value_radix specifies if calibration data is in binary(1), hex (16), ro decimal (10).
 *
 * @returns 1 if data was found, 0 if data was not found.
 */
unsigned short SearchConfigData(char * search_term, int term_length, int value_length, int value_radix, unsigned int * config_data, int start_index, char *config_array, unsigned long FileSize);

/**
 * __void ECLTriggerEnable__(unsigned short __BoardNum__, unsigned int __Enable__) enables or disables ECL triggering at the start of acquisition.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable = 1 sets the board to use ECL triggering. Enable = 0 sets the board to not use ECL triggering.
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int ECLTrigger = 1;
 * x_Load(dll);
 * dll.hDllECLTriggerEnable(BoardNum, ECLTrigger);
 * @endcode
 */
EXPORTED_FUNCTION void ECLTriggerEnable(unsigned short BoardNum, unsigned int Enable);

/**
 * __unsigned int makeADC12D2000_SPI__(unsigned int __ReadWrite__,unsigned int __spiAddress__,unsigned int __spiData__) generates the SPI word to write to ADC12D2000.
 * Low-level function that is called by higher level ADC SPI functions.
 *
 * Does not need to be called by user.
 *
 * @param ReadWrite = 0 sends a write command. ReadWrite = 1 sends a read command.
 * @param spiAddress is the SPI address to write to.
 * @param spiData is the data to write to the SPI address.
 *
 * @returns ADC12D2000 SPI word.
 */
unsigned int makeADC12D2000_SPI(unsigned int  ReadWrite,unsigned int spiAddress,unsigned int spiData);

/**
 * __void ADC12D2000_Channel_Mode_Select__(unsigned short __BoardNum__, unsigned int __Chan_mode__, unsigned int __Chan_select__, unsigned int __Calibrate__) selects whether to operate the
 * board in dual channel mode or single channel mode, when in single channel mode, what type of single channel to select, and whether to calibrate the ADC. __ADC12D2000_Set_Offsets__ is called
 * to properly adjust offsets for different modes. 
 * DES = dual edge sampling. When in DES mode, the ADC interleaves both cores and samples the same signal at twice the clock frequency.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Chan_mode = 2 for dual channel mode (non-DES). Chan_mode  = 1 for single channel mode (DES).
 * @param Chan_select = 0 for DESI (Ch.1), Chan_select = 1 for DESQ (Ch.0), Chan_select = 2 for DESIQ, Chan_select = 3 for DESCLKIQ.
 * @param Calibrate = 1 to calibrate the ADC after changing modes. Calibrate = 0 to skip calibration.
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int NumChannels = 2, OneChanSelect = 0, Calibrate = 1;
 * x_Load(dll);
 * dll.hDllADC12D2000_Channel_Mode_Select(BoardNum, NumChannels, OneChanSelect, Calibrate);	Calibrates the ADC to work in two channel mode (non-DES)
 * @endcode
 */
EXPORTED_FUNCTION void ADC12D2000_Channel_Mode_Select(unsigned short BoardNum, unsigned int Chan_mode, unsigned int Chan_select, unsigned int Calibrate);

/**
 * __void ADC12D2000_Set_Offsets__(unsigned short __BoardNum__, unsigned int __Chan_mode__, unsigned int __Chan_select__, unsigned int __Calibrate__) sets the SPI
 * offsets for the proper channel mode. Offset values are taken from ultra_config.dat. 
 * DES = dual edge sampling. When in DES mode, the ADC interleaves both cores and samples the same signal at twice the clock frequency.
 *
 * The user does not need to call this function. Set by __void ADC12D2000_Channel_Mode_Select__.
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Chan_mode = 2 for dual channel mode (non-DES). Chan_mode  = 1 for single channel mode (DES).
 * @param Chan_select = 0 for DESI (Ch.1), Chan_select = 1 for DESQ (Ch.0), Chan_select = 2 for DESIQ, Chan_select = 3 for DESCLKIQ.
 * @param Calibrate = 1 to calibrate the ADC after changing modes. Calibrate = 0 to skip calibration.
 */
void ADC12D2000_Set_Offsets (unsigned short BoardNum, unsigned int Chan_mode, unsigned int Chan_select);

void ADC12D2000_Set_Gains (unsigned short BoardNum, unsigned int Chan_mode, unsigned int Chan_select);

/**
 * __void CONFIGURE_CONTROL_REGISTER__(unsigned short __BoardNum__) sets the control register on the mainboard.
 * By default, SoftwareRunBit = 0
 *			  SoftwareStopBit = 0
 *			  InterleavedReadWrite = 1
 *			  ReadWholeRow = 0
 *			  InternalClockEnable = 1
 *			  WriteWholeRow = 1
 *
 * The user does not need to call this function. Set by __SetupBoard__.
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 */
void CONFIGURE_CONTROL_REGISTER(unsigned short BoardNum);

/**
 * __void SetSoftwareRunBit__(unsigned short __BoardNum__, unsigned int __Enable__) sends a run command to the user control register.
 * Inititation of acquistion is caused by sending a 0 followed by a 1. Stopping the board s followed by 0.
 *
 * The user does not need to call this function. Set by __CONFIGURE_CONTROL_REGISTER__.
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable = 0->1 starts acquisition. Enable = 0 stops acquisition
 */
void SetSoftwareRunBit (unsigned short BoardNum, unsigned int Enable);

/** 
 * __void SetSoftwareStopBit__(unsigned short __BoardNum__, unsigned int __Enable__)__
 * If true the driver stops the acquiring, this is required for continuous streaming applications. 
 * In this usage the board may acquire a few extra blocks due to the software latency.
 * When false the firmware will stop the board after acquiring an exact number of blocks (16-bits -> max. value is (2^16)-1 = 65535)
 * Leave false unless acquiring more than 65535MB of data
 * When true driver will stop the board after (2^32)-1 1MB blocks, this can be changed so driver doesn't ever stop board
 * 2^32 MB at 1600MB/s is over 31 days of strait recording
 * When true user should set blocks_to_acquire larger than the maximum record length and stop board using RUNBOARD(false)
 *
 * The user does not need to call this function. Set by __CONFIGURE_CONTROL_REGISTER__.
 *
 * Currently untested feature. Leave false.
 *
 */
void SetSoftwareStopBit (unsigned short BoardNum, unsigned int Enable);

/**
 * __void SetInterleavedReadWrite__(unsigned short __BoardNum__, unsigned int __Enable__) sets the board to read from the board
 * when data is available during acquisition, or waits until all data is collected before reading from the board.
 * By default, SetInterleavedReadWrite = 1
 *
 * The user does not need to call this function. Set by __CONFIGURE_CONTROL_REGISTER__.
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable = 1 interleaves read during acquisition. Enable = 0 reads after acquisition is complete.
 */
void SetInterleavedReadWrite (unsigned short BoardNum, unsigned int Enable);

/**
 * __void SetReadWholeRow__(unsigned short __BoardNum__, unsigned int __Enable__)
 * During reverse reads, read 16KB (DRAM row) at a time increases chances of overruns ('0' -> 4KB reads)
 * By default, SetReadWholeRow = 0
 *
 * The user does not need to call this function. Set by __CONFIGURE_CONTROL_REGISTER__.
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable = 1 reads whole row at a time.
 */
void SetReadWholeRow (unsigned short BoardNum, unsigned int Enable);

/**
 * __void SetInternalClockEnable__(unsigned short __BoardNum__, unsigned int __Enable__) sets the board the drive
 * the ADC with the supplied internal clock.
 * By default, SetInternalClockEnable = 1
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable = 1 enables the internal clock. Enable = 0 disables the internal clock, and allows an external clock to be driven.
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int InternalClock = 1
 * x_Load(dll);
 * dll.hDllSetInternalClockEnable(BoardNum, InternalClock);	Sets the ADC to use the internal clock.
 * @endcode
 */
EXPORTED_FUNCTION void SetInternalClockEnable (unsigned short BoardNum, unsigned int Enable);

EXPORTED_FUNCTION bool SetupMicrosynth (unsigned short BoardNum);
EXPORTED_FUNCTION void FreqMicrosynth (unsigned short BoardNum, double Frequency);

EXPORTED_FUNCTION void SetSetupDoneBit(unsigned short BoardNum, unsigned int Enable);

unsigned int GetSetupDoneBitValue(unsigned short BoardNum);

/**
 * __void SetWriteWholeRow__(unsigned short __BoardNum__, unsigned int __Enable__)
 * During PCIe D/A we read a PAGE of data across the bus at a time. These FIFOs
 * are not as deep as the A/D FIFOs and do not store a whole DRAM of data (16KB). 
 * By default, SetWriteWholeRow = 1
 * 
 * The user does not need to call this function. Set by __CONFIGURE_CONTROL_REGISTER__.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable = 1 reads a page at a time.
 *
 */
EXPORTED_FUNCTION void SetWriteWholeRow (unsigned short BoardNum, unsigned int Enable);

/**
 * __void SET_NUMBLOCKS_TO_ACQUIRE__(unsigned short __BoardNum__, unsigned int __NumBlocks__) sets the board to acquire
 * a specified number of 1MB blocks of data.
 *
 * The user does not need to call this function. Set by __setupAcquire__.
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param NumBlocks = (1-8192) 1MB blocks.
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int NumBlocks = 8192;
 * x_Load(dll);
 * dll.hDllSET_NUMBLOCKS_TO_ACQUIRE(BoardNum, NumBlocks);	Sets the board to acquire 1 1MB block.
 * @endcode
 */
void SET_NUMBLOCKS_TO_ACQUIRE (unsigned short BoardNum, unsigned int NumBlocks);

/**
 * __void SET_BLOCKSIZE__(unsigned short __BoardNum__, unsigned int __Blocksize__) sets the blocksize to a known value
 * By default, Blocksize = 1048576 (1MB). Cannot be changed.
 *
 * The user does not need to call this function. Set by __SetupBoard__.
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Blocksize must be 1048576.
 */
EXPORTED_FUNCTION void SET_BLOCKSIZE(unsigned short BoardNum, unsigned int Blocksize);

/**
 * __void SET_PCIE_RD_START_BLOCK__(unsigned short __BoardNum__, unsigned int __Offset__) sets the board to start read from
 * PCIe DRAM address __Offset__. This function is typically used when initializing the board before acquisition. When not in
 * acquisition, this function can be used to read any PCIe DRAM address on the board.
 *
 * The user does not need to call this function. Set by __setupAcquire__.
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Offset = (1-8192) 1MB blocks.
 */
EXPORTED_FUNCTION void SET_PCIE_RD_START_BLOCK (unsigned short BoardNum, unsigned int Offset);

/**
 * __void SET_PCIE_RD_END_BLOCK__(unsigned short __BoardNum__, unsigned int __Offset__) sets the end block PCIe DRAM address
 * Set to (total MB blocks on installed Ultraview Data Acquisition board) - 1.
 * 
 *
 * The user does not need to call this function. Set by __SetupBoard__.
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Offset = 8191 for 8GB boards. Offset = 4095 for 4GB boards.
 */
EXPORTED_FUNCTION void SET_PCIE_RD_END_BLOCK(unsigned short BoardNum, unsigned int Offset);
EXPORTED_FUNCTION void SET_DAC_RD_START_BLOCK (unsigned short BoardNum, unsigned int Offset);
EXPORTED_FUNCTION void SET_DAC_RD_END_BLOCK (unsigned short BoardNum, unsigned int Offset);
EXPORTED_FUNCTION void SET_DAC_WR_START_BLOCK (unsigned short BoardNum, unsigned int Offset);
EXPORTED_FUNCTION void SET_DAC_WR_END_BLOCK (unsigned short BoardNum, unsigned int Offset);

/**
 * __void CONFIGURE_PRETRIGGER__(unsigned short BoardNum) initializes the pretrigger count
 * Sends Value 511, then 0 to reset the pretrigger trigger counter.
 *
 * The user does not need to call this function. Set by __SetupBoard__.
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 */
void CONFIGURE_PRETRIGGER(unsigned short BoardNum);


EXPORTED_FUNCTION void SetPreTriggerMemory (unsigned short BoardNum, unsigned int Value);
EXPORTED_FUNCTION void SelectTrigger(unsigned short BoardNum, unsigned int TriggerType, unsigned int TriggerSlope, unsigned int TriggerCh);
EXPORTED_FUNCTION unsigned int IsTriggerEnabled(unsigned short BoardNum);

void SetAveragerOrSegmentedCapture(unsigned short BoardNum);

EXPORTED_FUNCTION void ConfigureWaveformTrigger(unsigned short BoardNum, unsigned int Threshold, unsigned int Hysteresis);

void SetWaveformTriggerThreshold(unsigned short BoardNum, unsigned int Threshold);
void SetWaveformTriggerHysteresis(unsigned short BoardNum, unsigned int Hysteresis);

EXPORTED_FUNCTION unsigned int ConfigureSegmentedCapture(unsigned short BoardNum, unsigned int CaptureCount, unsigned int CaptureDepth, unsigned int ClearAverager);
EXPORTED_FUNCTION unsigned int GetCaptureDepthValue(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int GetCaptureCountValue(unsigned short BoardNum);

void SetSegmentedCapture_NonShadow(unsigned short BoardNum, unsigned int CaptureCount, unsigned int CaptureDepth);
void SetCaptureDepth_NonShadow (unsigned short BoardNum, unsigned int size);
void SetCaptureCount_NonShadow (unsigned short BoardNum, unsigned int count);

EXPORTED_FUNCTION unsigned int ConfigureAverager(unsigned short BoardNum, unsigned int NumAverages, unsigned int AveragerLength, unsigned int ClearSegmentedCapture);
EXPORTED_FUNCTION unsigned int ConvertSegmentedCaptureLengthValue(unsigned short BoardNum, unsigned int AveragerLength);
EXPORTED_FUNCTION unsigned int UnconvertSegmentedCaptureLengthValue(unsigned short BoardNum, unsigned int Value);

void SET_NUM_AVERAGES(unsigned short BoardNum, unsigned int NumAverages);
unsigned int SET_AVERAGER_LENGTH(unsigned short BoardNum, unsigned int AveragerLength);
void SetAverager_NonShadow(unsigned short BoardNum, unsigned int NumAverages, unsigned int AveragerLength);
void SetNumAverages_NonShadow (unsigned short BoardNum, unsigned int NumAverages);
void SetAveragerLength_NonShadow (unsigned short BoardNum, unsigned int AveragerLength);
EXPORTED_FUNCTION void SetFiducialMarks(unsigned short BoardNum, unsigned int Enable);
EXPORTED_FUNCTION unsigned int AdcClockGetFreq(unsigned short BoardNum);








































/**
 * __void SetPreTrigger__(unsigned short BoardNum, unsigned int Value) sets the pretrigger value
 * By default, Value = 0.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Value is (0-511).
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int PretriggerValue = 10;
 * x_Load(dll);
 * dll.hDllSetPreTrigger(BoardNum, PretriggerValue);
 * @endcode
 */
EXPORTED_FUNCTION void SetPreTrigger (unsigned short BoardNum, unsigned int Value);

/**
 * __void SET_ECL_TRIGGER_DELAY__(unsigned short __BoardNum__, unsigned int __Value__) sets the amount of delay
 * to wait before acquiring data to memory after an ECL trigger event has occurred.
 * By default, Value = 0.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Value is (0-(2^32-1)).
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int ECLTriggerVal = 10;
 * x_Load(dll);
 * dll.hDllSET_ECL_TRIGGER_DELAY(BoardNum, ECLTriggerVal);
 * @endcode
 */
EXPORTED_FUNCTION void SET_ECL_TRIGGER_DELAY (unsigned short BoardNum, unsigned int Value);

/**
 * __void SET_CAPTURE_DEPTH__(unsigned short __BoardNum__, unsigned int __size__) sets size of each segmented
 * capture with the number of ECL trigger counts set by __SET_CAPTURE_COUNT__. After the number of capture counts has been satisfied,
 * the system with fill the remaining block with data to complete the minimum 1MB DMA transfer.
 *
 * Typically used when capturing multiple ECL trigger events while desiring a fixed number of samples to acquire after each trigger.
 * 
 * By default, Value = 0.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param size is (0-(2^32-1)).
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int count = 5, size = 1024;
 * x_Load(dll);
 * dll.hDllSET_CAPTURE_DEPTH(BoardNum, size);		Sets the board to acquire 1024 samples for 5 ECL trigger events and completes the 1MB block
 * dll.hDllSET_CAPTURE_COUNT (BoardNum, count);
 * @endcode
 */
EXPORTED_FUNCTION void SET_CAPTURE_DEPTH (unsigned short BoardNum, unsigned int size);

/**
 * __void SET_CAPTURE_COUNT__(unsigned short __BoardNum__, unsigned int __count__) sets the number of trigger events to capture data from
 * After each trigger event, the board will capture a specified number of samples determined by function __void SET_CAPTURE_DEPTH__. 
 * After the number of capture counts has been satisfied, the system with fill the remaining block with data to complete the minimum 1MB DMA transfer.
 *
 * Typically used when capturing multiple ECL trigger events while desiring a fixed number of samples to acquire after each trigger.
 * 
 * By default, Value = 0.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param size is (0-(2^32-1)).
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int count = 5, size = 1024;
 * x_Load(dll);
 * dll.hDllSET_CAPTURE_DEPTH(BoardNum, size);		Sets the board to acquire 1024 samples for 5 ECL trigger events and completes the 1MB block
 * dll.hDllSET_CAPTURE_COUNT (BoardNum, count);
 * @endcode
 */
EXPORTED_FUNCTION void SET_CAPTURE_COUNT (unsigned short BoardNum, unsigned int count);

/**
 * __void CONFIGURE_ADCDATASETUP_REGISTER__(unsigned short __BoardNum__) programs the board's firmware to configure the board's resolution type, number of
 * ADC channels to collect data from, and ADC decimation value.
 * By default, ADC resolution = 12 (fixed).
 *			  NumAdcChannel = 2 (fixed, firmware acts as AD83000x2).
 *			  AdcDecimation = 1 (no decimation);
 *
 * The user does not need to call this function. Set by __SetupBoard__.
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 */
void CONFIGURE_ADCDATASETUP_REGISTER (unsigned short BoardNum);

/**
 * __void SetAdcResolution__(unsigned short __BoardNum__, unsigned int __adc_res__) programs the board's firmware to configure the board's resolution type
 * By default, ADC resolution = 12 (fixed).
 *
 * The user does not need to call this function. Set by __CONFIGURE_ADCDATASETUP_REGISTER__.
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 */
void SetAdcResolution(unsigned short BoardNum, unsigned int adc_res);

/**
 * __void SetNumAdcChannel__(unsigned short __BoardNum__, unsigned int __adc_chan_used__) programs the board's firmware to collect from which ADC channel
 * By default, NumAdcChannel = 2 (fixed, firmware acts as AD83000x2).
 *
 * The user does not need to call this function. Set by __CONFIGURE_ADCDATASETUP_REGISTER__.
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 */
void SetNumAdcChannel(unsigned short BoardNum, unsigned int adc_chan_used);

/**
 * __void SetAdcDecimation__(unsigned short __BoardNum__, unsigned int __adc_deci_offset__) sets the ADC decimation value.
 * This function is used to divide the sampling rate by 1, 2 , 4, or 8. For example, if ADC decimation is set to 2, every other sample will be collected.
 * If ADC decimation is set to 4, every 4th sample will be collected.
 *
 * Typically used when wanting to sample at a rate lower than the ADC specification.
 * 
 * By default, adc_deci_offset = 1.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param adc_deci_offset is (1,2,3,4) to decimate (1,2,4,8) respectively.
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int adc_deci_offset = 2;
 * x_Load(dll);
 * dll.hDllSetAdcDecimation(BoardNum, adc_deci_offset);
 * @endcode
 */
EXPORTED_FUNCTION void SetAdcDecimation(unsigned short BoardNum, unsigned int adc_deci_offset);

/**
 * __void SetAdcUser__(unsigned short __BoardNum__, unsigned int __user_val__) customer requested function. (unsupported)
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param user_val.
 */
void SetAdcUser(unsigned short BoardNum, unsigned int user_val);				//

/**
 * __void SetADC12D2000ECEBAR__(unsigned short __BoardNum__, unsigned int __Enable__) must be set to 0 for ADC's 
 * extended control mode.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable = 0 (must be set 0).
 *
 * The user does not need to call this function. Set by __SetupBoard__.
 */
void SetADC12D2000ECEBAR(unsigned short BoardNum, unsigned int Enable);			//

/**
 * __void SetADC12D2000NDM__(unsigned short __BoardNum__, unsigned int __Enable__) must be set to 0 for ADC's 
 * non-demux mode.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable = 0 (must be set 0).
 *
 * The user does not need to call this function. Set by __SetupBoard__.
 */
void SetADC12D2000NDM(unsigned short BoardNum, unsigned int Enable);			//

/**
 * __void CONFIGURE_DRAMADJ_REGISTER__(unsigned short __BoardNum__) calibrates the board's DRAM memory.
 *
 * The user does not need to call this function. Set by __SetupBoard__.
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 */
void CONFIGURE_DRAMADJ_REGISTER (unsigned short BoardNum);

/**
 * __void SetDRAMIDelay_DQ_CE_INC__(unsigned short __BoardNum__, unsigned int __High__) low-level function used to calibrate the board's DRAM memory.
 *
 * The user does not need to call this function. Used in __CONFIGURE_DRAMADJ_REGISTER__.
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param High
 */
void SetDRAMIDelay_DQ_CE_INC(unsigned short BoardNum, unsigned int High);		//

/**
 * __void SetDRAMIDelay_DQS_CE_INC__(unsigned short __BoardNum__, unsigned int __High__) low-level function used to calibrate the board's DRAM memory.
 *
 * The user does not need to call this function. Used in __CONFIGURE_DRAMADJ_REGISTER__.
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param High
 */
void SetDRAMIDelay_DQS_CE_INC(unsigned short BoardNum, unsigned int High);		//

/**
 * __void SetDRAMIDelay_DQS_DQ_RST__(unsigned short __BoardNum__, unsigned int __High__) low-level function used to calibrate the board's DRAM memory.
 *
 * The user does not need to call this function. Used in __CONFIGURE_DRAMADJ_REGISTER__.
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param High
 */
void SetDRAMIDelay_DQS_DQ_RST(unsigned short BoardNum, unsigned int High);		//

/**
 * __void SetDRAMReadTime(unsigned short __BoardNum__, unsigned int __Value__) low-level function used to calibrate the board's DRAM memory.
 *
 * The user does not need to call this function. Used in __CONFIGURE_DRAMADJ_REGISTER__.
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Value
 */
void SetDRAMReadTime(unsigned short BoardNum, unsigned int Value);				//

/**
 * __void CONFIGURE_DRAMADJ_REGISTER__(unsigned short __BoardNum__) calibrates the board's DRAM memory.
 *
 * The user does not need to call this function. Set by __SetupBoard__.
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 */
void CONFIGURE_CAPTUREADJ_REGISTER (unsigned short BoardNum);

/**
 * __void AdcReset__(unsigned short __BoardNum) puts the board in reset.
 *
 * Typically used before __void AdcUnreset__ or __void AdcTristate__ to reset capture timing. Reset and
 * Unreset whenever settings that affect data capture are changed (e.g. changing from internal to external clock).
 * See __ECLTriggerEnable__ for proper usage.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * x_Load(dll);
 * dll.hDllAdcReset(BoardNum);
 * dll.hDllAdcUnreset(BoardNum);
 * @endcode
 */
EXPORTED_FUNCTION void AdcReset(unsigned short BoardNum);

/**
 * __void AdcUnreset(unsigned short BoardNum) puts the board in unreset.
 *
 * Typically used after __void AdcReset__ to reset capture timing. Reset and
 * Unreset whenever settings that affect data capture are changed (e.g. changing from internal to external clock).
 * See __ECLTriggerEnable__ for proper usage.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * x_Load(dll);
 * dll.hDllAdcReset(BoardNum);
 * dll.hDllAdcUnreset(BoardNum);
 * @endcode
 */
EXPORTED_FUNCTION void AdcUnreset(unsigned short BoardNum);

/**
 * __void AdcTristate__(unsigned short __BoardNum__) put the board into tristate.
 *
 * Typically used after __void AdcReset__ to reset capture timing and put the board to await an ECL triggered event.
 * The board should be armed after __AdcTristate__ to await ECL trigger.
 * See __ECLTriggerEnable__ for proper usage.
 *
 * Should not need to be called by user. Use __ECLTriggerEnable__.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 */
EXPORTED_FUNCTION void AdcTristate(unsigned short BoardNum);

/**
 * __void SetIdelayReset__(unsigned short __BoardNum__, unsigned int __Enable__)
 *
 * Does not need to be called by user.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void SetIdelayReset(unsigned short BoardNum, unsigned int Enable);				//

/**
 * __void SetIdelayCE__(unsigned short __BoardNum__, unsigned int __Enable__)
 *
 * Does not need to be called by user.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void SetIdelayCE(unsigned short BoardNum, unsigned int Enable);					//

/**
 * __void SetIdelayInc__(unsigned short __BoardNum__, unsigned int __Enable__)
 *
 * Does not need to be called by user.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void SetIdelayInc(unsigned short BoardNum, unsigned int Enable);				//

/**
 * __void SetIdelayCtrlReset__(unsigned short __BoardNum__, unsigned int __Enable__)
 *
 * Does not need to be called by user.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void SetIdelayCtrlReset(unsigned short BoardNum, unsigned int Enable);			//

/**
 * __void SetIdelayResetCtrlReset__(unsigned short __BoardNum__, unsigned int __Enable__)
 *
 * Does not need to be called by user.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void SetIdelayResetCtrlReset(unsigned short BoardNum, unsigned int Enable);		//

/**
 * __void SetAcquireDisableEdge__(unsigned short __BoardNum__, unsigned int __Enable__)
 *
 * Unused by AD12-2000x2 boards
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void SetAcquireDisableEdge(unsigned short BoardNum, unsigned int Enable);

/**
 * __void SetAcquireDisableReset__(unsigned short __BoardNum__, unsigned int __Enable__)
 *
 * Unused by AD12-2000x2 boards
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void SetAcquireDisableReset(unsigned short BoardNum, unsigned int Enable);

/**
 * __void SetAcquireDisableInvert__(unsigned short __BoardNum__, unsigned int __Invert__) inverts the selective recording feature.
 * By default Invert =  0 (active LOW). When the TTL selective recording input is LOW, the board will acquire data and will not acquire
 * when the TTL selective recording is HIGH.
 *
 * Settng Invert = 1 (active HIGH), the board will acquire data when the TTL selective input is HIGH, and will not acquire when the TTL
 * selective recording input is LOW.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Invert = 1 for active HIGH. Invert = 0 for active LOW (default).
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int Invert = 0;
 * x_Load(dll);
 * dll.hDllSetAcquireDisableInvert(BoardNum, Invert);	Sets the board for active LOW.
 * @endcode
 */
EXPORTED_FUNCTION void SetAcquireDisableInvert(unsigned short BoardNum, unsigned int Invert);

/**
 * __void SetContinuousAutoRefresh__(unsigned short __BoardNum__, unsigned int __Enable__) refreshes the DRAM memory.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable = 1 sets continuous autorefresh true.
 *
 * Does not need to be called by user.
 */
EXPORTED_FUNCTION void SetContinuousAutoRefresh(unsigned short BoardNum, unsigned int Enable);

/**
 * __void SetADCDebugSync__(unsigned short __BoardNum__, unsigned int __Enable__)
 * Forces external sync signal to the capture adjust register. (untested)
 *
 * Does not need to be called by user.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void SetADCDebugSync(unsigned short BoardNum, unsigned int Enable);				//

/**
 * __void SetADCSingleChannelSelect__(unsigned short __BoardNum__, unsigned int __CH1__)
 *
 * Unused by AD12-2000x2 boards
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param CH1
 */
void SetADCSingleChannelSelect(unsigned short BoardNum, unsigned int CH1);		//

/**
 * __void SetMezzPowerDown__(unsigned short __BoardNum__, unsigned int __Enable__)
 *
 * Does not need to be called by user.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void SetMezzPowerDown(unsigned short BoardNum, unsigned int Enable);			//


/**
 * __void SetADCCLKDiv__(unsigned short __BoardNum__, unsigned int __Value__)
 *
 * Unused by AD12-2000x2 boards
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Value
 */
void SetADCCLKDiv(unsigned short BoardNum, unsigned int Value);					//

/**
 * __void SetDualADCSingleChannel__(unsigned short __BoardNum__, unsigned int __Enable__)
 *
 * Must be set to 0 for AD12-2000x2 boards. Does not need to be called by user. Set by __SetupBoard__.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Value
 */
void SetDualADCSingleChannel(unsigned short BoardNum, unsigned int Enable);		//

/**
 * __void Write_ADC12D2000__(unsigned short __BoardNum__, unsigned int __spiAddress__, unsigned int __spiData__)
 * Low level function to write an entire register to an ADC SPI address.
 *
 * Does not need to be called by user. Used in higher level write functions.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param spiAddress is the SPI register address for the ADC12D2000
 * @param spiData is the register bits in spiAddress
 */
void Write_ADC12D2000(unsigned short BoardNum, unsigned int spiAddress, unsigned int spiData);

/**
 * __unsigned int Read_ADC12D2000__(unsigned short __BoardNum__, unsigned int __spiAddress__)
 * Low level function to read to an ADC SPI address.
 *
 * Does not need to be called by user. Used in higher level read functions.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param spiAddress is the SPI register address for the ADC12D2000
 * @param spiData is the register bits in spiAddress
 *
 * @returns SPI address data bits
 */
unsigned int Read_ADC12D2000_shadow(unsigned short BoardNum, unsigned int spiAddress);
unsigned int Read_ADC12D2000_register(unsigned short BoardNum, unsigned int spiAddress);

/**
 * __void ADC12D2000_INITIALIZE__(unsigned short __BoardNum__) initializes ALL ADC SPI registers and sets ADC to two-channel mode.
 *
 * Does not need to be called by user. Set by __SetupBaoard__. If user wishes to adjust channel mode,
 * use __ADC12D2000_Channel_Mode_Select__.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 */
void ADC12D2000_INITIALIZE(unsigned short BoardNum);

/**
 * __void ADC12D2000_SET_CONFIGURATION_REGISTER_1__(unsigned short __BoardNum__) initializes the ADC's SPI Configuration_Register_1 and sets ADC to two-channel mode.
 *
 * Does not need to be called by user. Set by __ADC12D2000_INITIALIZE__. If user wishes to adjust channel mode,
 * use __ADC12D2000_Channel_Mode_Select__.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 */
void ADC12D2000_SET_CONFIGURATION_REGISTER_1(unsigned short BoardNum);

/**
 * __void ADC12D2000_Calibrate_ADC(unsigned short BoardNum) calibrates the ADC via SPI.
 *
 * When switching channels, __ADC12D2000_Channel_Mode_Select__ includes a calibrate option.
 * When switching clocks, this function should be sent.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * x_Load(dll);
 * dll.hDllADC12D2000_Calibrate_ADC(BoardNum);
 * @endcode
 */
void ADC12D2000_Calibrate_ADC(unsigned short BoardNum);

/**
 * __void ADC12D2000_SetDCLKPhaseSelect__(unsigned short __BoardNum__, unsigned int __Enable__).
 * Must be set to 0.
 *
 * Does not need to be called by user. Set by __ADC12D2000_SET_CONFIGURATION_REGISTER_1__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void ADC12D2000_SetDCLKPhaseSelect(unsigned short BoardNum, unsigned int Enable);

/**
 * __void ADC12D2000_SetOutputVoltageSelect__(unsigned short __BoardNum__, unsigned int __Enable__).
 * Must be set to 1.
 *
 * Does not need to be called by user. Set by __ADC12D2000_SET_CONFIGURATION_REGISTER_1__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void ADC12D2000_SetOutputVoltageSelect(unsigned short BoardNum, unsigned int Enable);

/**
 * __void ADC12D2000_SetTestPatternMode__(unsigned short __BoardNum__, unsigned int __Enable__).
 * By default, Enable = 0.
 *
 * Does not need to be called by user. Set by __ADC12D2000_SET_CONFIGURATION_REGISTER_1__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable = 0 normal data mode. Enable = 1 test pattern mode.
 *
 ** Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int TestPattern = 1;
 * x_Load(dll);
 * dll.hDllADC12D2000_SetTestPatternMode(BoardNum, TestPattern);		Sets the ADC to output test pattern data
 * @endcode
 */
void ADC12D2000_SetTestPatternMode(unsigned short BoardNum, unsigned int Enable);

/**
 * __void ADC12D2000_SetPowerDownIChannel__(unsigned short __BoardNum__, unsigned int __Enable__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_SET_CONFIGURATION_REGISTER_1__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void ADC12D2000_SetPowerDownIChannel(unsigned short BoardNum, unsigned int Enable);

/**
 * __void ADC12D2000_SetPowerDownQChannel__(unsigned short __BoardNum__, unsigned int __Enable__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_SET_CONFIGURATION_REGISTER_1__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void ADC12D2000_SetPowerDownQChannel(unsigned short BoardNum, unsigned int Enable);

/**
 * __void ADC12D2000_SetLowFrequencySelect__(unsigned short __BoardNum__, unsigned int __Enable__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_Channel_Mode_Select__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void ADC12D2000_SetLowFrequencySelect(unsigned short BoardNum, unsigned int Enable);

/**
 * __void ADC12D2000_SetDualEdgeSamplingMode(unsigned short __BoardNum__, unsigned int __Enable__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_Channel_Mode_Select__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void ADC12D2000_SetDualEdgeSamplingMode(unsigned short BoardNum, unsigned int Enable);

EXPORTED_FUNCTION unsigned int ADC12D2000_GetDualEdgeSamplingMode(unsigned short BoardNum);

/**
 * __void ADC12D2000_SetDESQInputSelect__(unsigned short __BoardNum__, unsigned int __Q_Input__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_Channel_Mode_Select__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void ADC12D2000_SetDESQInputSelect(unsigned short BoardNum, unsigned int Q_Input);

/**
 * __void ADC12D2000_SetDESIQInputSelect__(unsigned short __BoardNum__, unsigned int __Enable__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_Channel_Mode_Select__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void ADC12D2000_SetDESIQInputSelect(unsigned short BoardNum, unsigned int Enable);

/**
 * __void ADC12D2000_SetTwosComplimentOutput__(unsigned short __BoardNum__, unsigned int __Enable__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_SET_CONFIGURATION_REGISTER_1__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void ADC12D2000_SetTwosComplimentOutput(unsigned short BoardNum, unsigned int Enable);					//

/**
 * __void ADC12D2000_SetSingleDataRate__(unsigned short __BoardNum__, unsigned int __Enable__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_SET_CONFIGURATION_REGISTER_1__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void ADC12D2000_SetSingleDataRate(unsigned short BoardNum, unsigned int Enable);						//

/**
 * __void ADC12D2000_SET_RESERVED_ADDR_0x1__(unsigned short __BoardNum__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_INITIALIZE__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 */
void ADC12D2000_SET_RESERVED_ADDR_0x1(unsigned short BoardNum);											//

/**
 * __void ADC12D2000_SET_I_CHANNEL_OFFSET_ADJUST__(unsigned short __BoardNum__, unsigned int __Sign__, unsigned int __Offset__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_INITIALIZE__. Values read from ultra_config.dat.
 * Set by __SetupBoard__ by default, and set by __ADC12D2000_Channel_Select_Mode__ when changing channel modes.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Sign = 0  for positive offset. Sign = 1 for negative offset.
 * @param Offset is (0-0xFFF).
 */
EXPORTED_FUNCTION void ADC12D2000_SET_I_CHANNEL_OFFSET_ADJUST(unsigned short BoardNum, unsigned int Sign, unsigned int Offset);

/**
 * __void ADC12D2000_SET_I_CHANNEL_FULL_SCALE_RANGE_ADJUST__(unsigned short __BoardNum__, unsigned int __FSR__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_INITIALIZE__. Values read from ultra_config.dat.
 * Set by __SetupBoard__ by default, and set by __ADC12D2000_Channel_Select_Mode__ when changing channel modes.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param FSR is (0-0x7FFF).
 */
EXPORTED_FUNCTION void ADC12D2000_SET_I_CHANNEL_FULL_SCALE_RANGE_ADJUST(unsigned short BoardNum, unsigned int FSR);

/**
 * __void ADC12D2000_SET_CALIBRATION_ADJUST(unsigned short BoardNum).
 *
 * Does not need to be called by user. Set by __ADC12D2000_INITIALIZE__.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 */
void ADC12D2000_SET_CALIBRATION_ADJUST(unsigned short BoardNum);

/**
 * __void ADC12D2000_SET_CALIBRATION_VALUES__(unsigned short __BoardNum__, unsigned int __Value__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_INITIALIZE__. Values read from ultra_config.dat.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Value.
 */
void ADC12D2000_SET_CALIBRATION_VALUES(unsigned short BoardNum, unsigned int Value);

/**
 * __void ADC12D2000_SET_BIAS_ADJUST__(unsigned short __BoardNum__, unsigned int __Value__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_INITIALIZE__. Value read from ultra_config.dat.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Value.
 */
void ADC12D2000_SET_BIAS_ADJUST(unsigned short BoardNum, unsigned int Value);							//

/**
 * __void ADC12D2000_SET_DES_TIMING_ADJUST__(unsigned short __BoardNum__, unsigned int __Value__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_INITIALIZE__. Value read from ultra_config.dat.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Value.
 */
void ADC12D2000_SET_DES_TIMING_ADJUST(unsigned short BoardNum, unsigned int Value);						//

/**
 * __void ADC12D2000_SET_RESERVED_ADDR_8__(unsigned short __BoardNum__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_INITIALIZE__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 */
void ADC12D2000_SET_RESERVED_ADDR_0x8(unsigned short BoardNum);											//

/**
 * __void ADC12D2000_SET_RESERVED_ADDR_9__(unsigned short __BoardNum__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_INITIALIZE__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 */
void ADC12D2000_SET_RESERVED_ADDR_0x9(unsigned short BoardNum);											//

/**
 * __void ADC12D2000_SET_Q_CHANNEL_OFFSET_ADJUST__(unsigned short __BoardNum__, unsigned int __Sign__, unsigned int __Offset__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_INITIALIZE__. Values read from ultra_config.dat.
 * Set by __SetupBoard__ by default, and set by __ADC12D2000_Channel_Select_Mode__ when changing channel modes.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Sign = 0  for positive offset. Sign = 1 for negative offset.
 * @param Offset is (0-0xFFF).
 */
EXPORTED_FUNCTION void ADC12D2000_SET_Q_CHANNEL_OFFSET_ADJUST(unsigned short BoardNum, unsigned int Sign, unsigned int Offset);

/**
 * __void ADC12D2000_SET_Q_CHANNEL_FULL_SCALE_RANGE_ADJUST__(unsigned short __BoardNum__, unsigned int __FSR__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_INITIALIZE__. Values read from ultra_config.dat.
 * Set by __SetupBoard__ by default, and set by __ADC12D2000_Channel_Select_Mode__ when changing channel modes.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param FSR is (0-0x7FFF).
 */
EXPORTED_FUNCTION void ADC12D2000_SET_Q_CHANNEL_FULL_SCALE_RANGE_ADJUST(unsigned short BoardNum, unsigned int FSR);

/**
 * __void ADC12D2000_SET_APERTURE_DELAY_COURSE_ADJUST(unsigned short __BoardNum__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_INITIALIZE__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 */
void ADC12D2000_SET_APERTURE_DELAY_COURSE_ADJUST(unsigned short BoardNum);								//

/**
 * __void ADC12D2000_SetApertureDelayCoarseAdjust__(unsigned short __BoardNum__, unsigned int __Value__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_SET_APERTURE_DELAY_COURSE_ADJUST__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Value.
 */
void ADC12D2000_SetApertureDelayCoarseAdjust(unsigned short BoardNum, unsigned int Value);				//

/**
 * __void ADC12D2000_SetApertureDelayCoarseAdjustEnable__(unsigned short __BoardNum__, unsigned int __Enable__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_SET_APERTURE_DELAY_COURSE_ADJUST__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void ADC12D2000_SetApertureDelayCoarseAdjustEnable(unsigned short BoardNum, unsigned int Enable);		//

/**
 * __void ADC12D2000_SetDutyCycleCorrect__(unsigned short __BoardNum__, unsigned int __Enable__).
 *
 * Does not need to be called by user. Set by ADC12D2000_SET_APERTURE_DELAY_COURSE_ADJUST. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void ADC12D2000_SetDutyCycleCorrect(unsigned short BoardNum, unsigned int Enable);						//

/**
 * __void ADC12D2000_SET_APERTURE_DELAY_FINE_ADJUST__(unsigned short __BoardNum__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_INITIALIZE__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 */
void ADC12D2000_SET_APERTURE_DELAY_FINE_ADJUST(unsigned short BoardNum);								//

/**
 * __void ADC12D2000_SetApertureDelayFineAdjust__(unsigned short __BoardNum__, unsigned int __Value__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_SET_APERTURE_DELAY_FINE_ADJUST__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Value
 */
void ADC12D2000_SetApertureDelayFineAdjust(unsigned short BoardNum, unsigned int Value);				//

/**
 * __void ADC12D2000_SetApertureDelayFineAdjustEnable__(unsigned short __BoardNum__, unsigned int __Enable__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_SET_APERTURE_DELAY_FINE_ADJUST__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void ADC12D2000_SetApertureDelayFineAdjustEnable(unsigned short BoardNum, unsigned int Enable);			//

/**
 * __void ADC12D2000_SET_CONFIGURATION_REGISTER_2__(unsigned short __BoardNum__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_INITIALIZE__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 */
void ADC12D2000_SET_CONFIGURATION_REGISTER_2(unsigned short BoardNum);

/**
 * __void ADC12D2000_SetDESIQCLKMode__(unsigned short __BoardNum__, unsigned int __Enable__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_SET_CONFIGURATION_REGISTER_2__ by default. 
 * Set by __ADC12D2000_Channel_Mode_Select__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param Enable
 */
void ADC12D2000_SetDESIQCLKMode(unsigned short BoardNum, unsigned int Enable);

/**
 * __void ADC12D2000_SetDisableReset__(unsigned short __BoardNum__, unsigned int __Enable__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_SET_CONFIGURATION_REGISTER_2__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 */
void ADC12D2000_SetDisableReset(unsigned short BoardNum, unsigned int Enable);

/**
 * __void ADC12D2000_SetDisableReset__(unsigned short __BoardNum__, unsigned int __Enable__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_SET_CONFIGURATION_REGISTER_2__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 */
void ADC12D2000_Set_CFR2_Constants(unsigned short BoardNum);											//

/**
 * __void ADC12D2000_SET_RESERVED_ADDR_0xF__(unsigned short __BoardNum__).
 *
 * Does not need to be called by user. Set by __ADC12D2000_INITIALIZE__. 
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 */
void ADC12D2000_SET_RESERVED_ADDR_0xF(unsigned short BoardNum);

/**
 * __void setupAcquire__(unsigned short __BoardNum__,unsigned int __NumBlocks__)  Sets up the board referred to by __BoardNum__ to acquire __NumBlocks__.  The function returns immediately.
 * 
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 * @param NumBlocks is the is the number of 1Mbyte blocks (1-8192) to acquire into on-board memory.
 * 
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * bool success = dll.hDllSetupBoard(BoardNum);
 * unsigned int numBlocksToAcquire=8192;
 * dll.hDllsetupAcquire(BoardNum,numBlocksToAcquire);  // acquire 8192 blocks into onboard memory
 * @endcode
 */	
EXPORTED_FUNCTION void setupAcquire(unsigned short BoardNum,unsigned int NumBlocks );

/**
 * __unsigned int getNumChannels__(unsigned short __BoardNum__) returns the number of channels the board is set to use.
 * This function is dependent upon __dev_config[BoardNum]__ being populated by __GetBoardConfigInfo__ and/or set by
 * __ADC12D2000_Channel_Mode_Select__.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 *
 * @returns Number of channels being used (dev_config[BoardNum].adc_chan_used).
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int NumChanUsed;
 * NumChanUsed = dll.hDllgetNumChannels(BoardNum);
 * @endcode
 */
EXPORTED_FUNCTION unsigned int getNumChannels(unsigned short BoardNum);

/**
 * __unsigned int ADC12D2000_GetDESQInput__(unsigned short __BoardNum__) returns whether the ADC is in DESI or DESQ mode.
 * Low-level function called by __ADC12D2000_GetOneChanModeValue__.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 *
 * @returns 0 for DESI, 1 for DESQ.
 */
unsigned int ADC12D2000_GetDESQInput(unsigned short BoardNum);

/**
 * __unsigned int ADC12D2000_GetDESIQInput__(unsigned short __BoardNum__) returns whether the ADC is in DESIQ mode or not.
 * Low-level function called by __ADC12D2000_GetOneChanModeValue__.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 *
 * @returns 0 if not in DESIQ, or 1 if in DESIQ.
 */
unsigned int ADC12D2000_GetDESIQInput(unsigned short BoardNum);

/**
 * __unsigned int ADC12D2000_GetDESCLKIQInput(unsigned short BoardNum) returns whether the ADC is in DESCLKIQ mode or not.
 * Low-level function called by __ADC12D2000_GetOneChanModeValue__.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 *
 * @returns 0 if not in DESCLKIQ, or 1 if in DESCLKIQ.
 */
unsigned int ADC12D2000_GetDESCLKIQInput(unsigned short BoardNum);

/**
 * __unsigned int ADC12D2000_GetOneChanModeValue__(unsigned short __BoardNum__) reads the shadow registers to determine
 * what kind of one channel mode the board is currently configured for.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 *
 * @returns 0 for DESI, 1 for DESQ, 2 for DESIQ, 3 for DESCLKIQ.
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int OneChanMode;
 * OneChanMode = dll.hDllADC12D2000_GetOneChanModeValue(BoardNum);
 * @endcode
 */
EXPORTED_FUNCTION unsigned int ADC12D2000_GetOneChanModeValue(unsigned short BoardNum);

/**
 * __unsigned int GetDecimationValue__(unsigned short __BoardNum__) returns the decimation value currently set.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 *
 * @returns 1 for no decimation, 2 for decimation by 2, 3 for decimation by 4, 4 for decimation by 8.
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int adc_deci_value;
 * adc_deci_value = dll.hDllGetDecimationValue(BoardNum);
 * @endcode
 */
EXPORTED_FUNCTION unsigned int GetDecimationValue(unsigned short BoardNum);

/**
 * __unsigned int GetPretriggerValue__(unsigned short __BoardNum__) returns the pretrigger value currently set.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 *
 * @returns (0-511) pretrigger value.
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int pretriggerval;
 * pretriggerval = dll.hDllGetPretriggerValue(BoardNum);
 * @endcode
 */
EXPORTED_FUNCTION unsigned int GetPretriggerValue(unsigned short BoardNum);

/**
 * __unsigned int GetEclTriggerDelayValue__(unsigned short __BoardNum__) returns the ECL trigger delay value currently set.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 *
 * @returns (0 - (2^32-1)) delay value.
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int ECLTrigDelayVal;
 * ECLTrigDelayVal = dll.hDllGetEclTriggerDelayValue(BoardNum);
 * @endcode
 */
EXPORTED_FUNCTION unsigned int GetEclTriggerDelayValue(unsigned short BoardNum);

/**
 * __unsigned int GetCaptureCountValue__(unsigned short __BoardNum__) returns the capture delay value currently set.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 *
 * @returns (0 - (2^32-1)).
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int CaptureCountVal;
 * CaptureCountVal = dll.hDllGetCaptureCountValue(BoardNum);
 * @endcode
 */
EXPORTED_FUNCTION unsigned int GetCaptureCountValue(unsigned short BoardNum);

/**
 * __unsigned int GetCaptureDepthValue__(unsigned short __BoardNum__) returns the capture depth value currently set.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 *
 * @returns (0 - (2^32-1)).
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int CaptureDepthVal;
 * CaptureDepthVal = dll.hDllGetCaptureDepthValue(BoardNum);
 * @endcode
 */
EXPORTED_FUNCTION unsigned int GetCaptureDepthValue(unsigned short BoardNum);

/**
 * __unsigned int GetInternalClockValue__(unsigned short __BoardNum__) returns value indicating whether internal clock is enabled or not.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 *
 * @returns 0 if internal clock is disabled (using external clock), 1 if internal clock is enabled.
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int InternalClockVal;
 * InternalClockVal = dll.hDllGetInternalClockValue(BoardNum);
 * @endcode
 */
EXPORTED_FUNCTION unsigned int GetInternalClockValue(unsigned short BoardNum);

/**
 * __unsigned int GetECLTriggerEnableValue__(unsigned short __BoardNum__) returns value indicating if ECL triggering is enabled.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 *
 * @returns 0 if ECL triggering is disabled, 1 if ECL trigger is enabled.
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int ECLTriggerEnableVal;
 * ECLTriggerEnableVal = dll.hDllGetECLTriggerEnableValue(BoardNum);
 * @endcode
 */
EXPORTED_FUNCTION unsigned int GetECLTriggerEnableValue(unsigned short BoardNum);

/**
 * __GetAcquireDisableInvertValue(unsigned short BoardNum) returns value indicating if selective recording invert is enabled.
 *
 * @param BoardNum is the index (0-4) for an installed Ultraview Data Acquisition board.
 *
 * @returns 0 if selective recording invert is disabled, 1 if selective recording invertr is enabled.
 *
 * Example Usage:
 * @code
 * dllstruct dll;
 * unsigned short BoardNum = 0;
 * unsigned int AcquireDisableInvertVal;
 * AcquireDisableInvertVal = dll.hDllGetAcquireDisableInvertValue(BoardNum);
 * @endcode
 */
EXPORTED_FUNCTION unsigned int GetAcquireDisableInvertValue(unsigned short BoardNum);

EXPORTED_FUNCTION unsigned int GetDRAMDQSValue(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int GetDRAMDQValue(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int GetNumBlocksToAcquireValue(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int GetOnBoardMemorySize(unsigned short BoardNum);

long makeISLA_SPI(unsigned action,unsigned channels,unsigned spiAddress,unsigned spiData );
void WriteISLA_SPI(unsigned short BoardNum, unsigned int adcNum, unsigned int spiAddress, unsigned int spiData);
unsigned int ReadISLA_SPI(unsigned short BoardNum, unsigned int adcNum, unsigned int spiAddress);
unsigned char ReadISLA_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int spiAddress);


void CalibrateBoard(unsigned short BoardNum);
void CONFIGURE_CHIP_PORT_CONFIG(unsigned short BoardNum, unsigned int adcNum);
void SetISLA_SDOActive(unsigned short BoardNum, unsigned int adcNum, unsigned int Enable);
void SetISLA_LSBFirst(unsigned short BoardNum, unsigned int adcNum, unsigned int Enable);
void SetISLA_SoftReset(unsigned short BoardNum, unsigned int adcNum, unsigned int Enable);
void SetISLA_ReservedBit4(unsigned short BoardNum, unsigned int adcNum);
EXPORTED_FUNCTION void SET_OFFSET_COARSE_ADC0(unsigned short BoardNum, unsigned int adcNum, unsigned int Value);
EXPORTED_FUNCTION void SET_OFFSET_FINE_ADC0(unsigned short BoardNum, unsigned int adcNum, unsigned int Value);
EXPORTED_FUNCTION void SET_GAIN_COARSE_ADC0(unsigned short BoardNum, unsigned int adcNum, unsigned int Value);
EXPORTED_FUNCTION void SET_GAIN_MEDIUM_ADC0(unsigned short BoardNum, unsigned int adcNum, unsigned int Value);
EXPORTED_FUNCTION void SET_GAIN_FINE_ADC0(unsigned short BoardNum, unsigned int adcNum, unsigned int Value);
void SET_MODES_ADC0(unsigned short BoardNum, unsigned int adcNum, unsigned int Value);
EXPORTED_FUNCTION void SET_OFFSET_COARSE_ADC1(unsigned short BoardNum, unsigned int adcNum, unsigned int Value);
EXPORTED_FUNCTION void SET_OFFSET_FINE_ADC1(unsigned short BoardNum, unsigned int adcNum, unsigned int Value);
EXPORTED_FUNCTION void SET_GAIN_COARSE_ADC1(unsigned short BoardNum, unsigned int adcNum, unsigned int Value);
EXPORTED_FUNCTION void SET_GAIN_MEDIUM_ADC1(unsigned short BoardNum, unsigned int adcNum, unsigned int Value);
EXPORTED_FUNCTION void SET_GAIN_FINE_ADC1(unsigned short BoardNum, unsigned int adcNum, unsigned int Value);
void SET_MODES_ADC1(unsigned short BoardNum, unsigned int adcNum, unsigned int Value);
void SET_RESERVED31_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Value);
void SET_RESERVED53_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Value);
void SET_RESERVED55_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Value);
void SET_PHASE_SLIP_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Value);
void SET_CLOCK_DIVIDE_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Value);
void SET_2X_CLOCK(unsigned short BoardNum);
void SET_1X_CLOCK(unsigned short BoardNum);
EXPORTED_FUNCTION void SET_DLL_RANGE_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Value);
EXPORTED_FUNCTION void SET_TEST_PATTERN_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Value);
EXPORTED_FUNCTION void SET_OFFSET_GAIN_ENABLE_REG(unsigned short BoardNum, unsigned int adcNum, unsigned int Enable);

void SET_CLOCK_DIV_RST_POL(unsigned short BoardNum, unsigned int adcNum, unsigned int Value);
EXPORTED_FUNCTION void ADC_NRESET(unsigned short BoardNum, unsigned int adcNum);
void ADC_CLKDIVRST(unsigned short BoardNum, unsigned int adcNum);
void ADC_SETCALBITS(unsigned short BoardNum, unsigned int Value);
void readISLA_Gain_Offset(unsigned short BoardNum, unsigned char *regVal[]);
void printISLA_Gain_Offset(unsigned char **regVal);
void ISLA_GainAdjust (unsigned short BoardNum);
void AdcIDelayManual(unsigned short BoardNum, unsigned int  adcNum, unsigned int dclk, unsigned int data);
void AdcIDelayAuto(unsigned short BoardNum, unsigned int adcNum);
int ISLA_AdcIdelayCalibrate(unsigned short BoardNum);
void ISLA_AdcDacSerialSetup(unsigned short BoardNum);
EXPORTED_FUNCTION void AdcDacSerialWrite(unsigned short BoardNum, unsigned int cmd, unsigned int addr, unsigned int adc_dac_data);
void printISLA_registers(unsigned char **regVal);
EXPORTED_FUNCTION void SelectAdcChannels(unsigned short BoardNum, unsigned int Channels);

// Access type to memory buffer depends on converter resolution and number of channels. This function
// returns the sample value for the given channel at the given index into the memory buffer passed into the function.
EXPORTED_FUNCTION SAMPLE_TYPE GetSample(unsigned short BoardNum, void * pBuffer, unsigned int index, unsigned short channel);

unsigned int GetAnalogDigitalBitValue(unsigned short BoardNum);
unsigned int GetTriggerSelectValue(unsigned short BoardNum);


EXPORTED_FUNCTION unsigned int GetNumAveragesValue(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int GetAveragerLengthValue(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int getNumChannels(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int getFPGAType(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int GetPretriggerValue(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int GetCaptureCountValue(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int GetCaptureDepthValue(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int GetInternalClockValue(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int GetAcquireDisableEdgeValue(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int GetAcquireDisableInvertValue(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int GetDRAMDQSValue(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int GetDRAMDQValue(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int GetNumBlocksToAcquireValue(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int Is_ISLA214P(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int GetOnBoardMemorySize(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int GetChannelSelectValue(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int GetSyncSelectiveRecordingValue(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int GetWaveformThresholdValue(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int GetWaveformHysteresisValue(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned int GetFiducialMarks(unsigned short BoardNum);

EXPORTED_FUNCTION void SET_ADC_OFFSETS(unsigned short BoardNum);
EXPORTED_FUNCTION bool is_ISLA216P(unsigned short BoardNum);
EXPORTED_FUNCTION bool is_AD5474(unsigned short BoardNum);
EXPORTED_FUNCTION bool is_adc12d2000(unsigned short BoardNum);
EXPORTED_FUNCTION bool has_microsynth(unsigned short BoardNum);
EXPORTED_FUNCTION unsigned long GetOverruns(unsigned short BoardNum);

void AdcSetGain(unsigned short BoardNum, unsigned int adc_fsr, unsigned short RHS_ADC, unsigned short sel_Q_chan);
void AdcSetOffset(unsigned short BoardNum, unsigned short adc_offset, unsigned short adc_neg, unsigned short RHS_ADC, unsigned short sel_Q_chan);
void AdcClockAdjust(unsigned short BoardNum);
void AdcClockOk(unsigned short BoardNum);
void AdcCalibrate(unsigned short BoardNum);
void AdcDcmPrelimSetup(unsigned short BoardNum);
void AdcDcmSetup(unsigned short BoardNum);
void AdcDcmSetPs(unsigned short BoardNum, int num_ps_shift, int ps_positive);
void AdcDcmPsCal(unsigned short BoardNum);
void set_adc_data_setup_reg (unsigned short BoardNum);
bool SetDualSingleMode(unsigned short BoardNum);




#ifndef _WINDOWS
// This is a system function in Windows, but is implemented by us in Linux
void Sleep(unsigned int num_milliseconds);

#include <time.h>

long GetTickCount();

#endif


#endif 






