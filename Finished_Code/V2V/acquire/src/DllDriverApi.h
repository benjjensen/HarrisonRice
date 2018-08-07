// DllDriverApi.h  class interface 
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AD83000X_API_H___INCLUDED_)
#define AFX_AD83000X_API_H___INCLUDED_


#include "DllLibMain.h"

//#define DEBUG_TO_FILE


#ifdef _WINDOWS
#include "AppDll.h"
typedef void * HANDLE;
#else
#define EXPORTED_FUNCTION
#include "AppDll.h"
#define HANDLE int
#endif




class CDllDriverApi  
{

	public:

	// Member Variables
	short * BoardSerialNum;
	HANDLE * m_AllDeviceHandles;    // Keep handles to all devices found PUBLIC -> set hCurrentDevice at top level
	HANDLE hCurrentDevice;          // Handle to the device currently attached to
	unsigned short NumDevices;      // The number of devices found
	unsigned long bytesRead;
#ifdef _WINDOWS
	DRV_VERSION drvVersion;
#endif
	// Member Functions
	bool ApiInitialize();	
	unsigned long ApiGetDeviceStatus();
	
	// Exported DLL Member Functions
	unsigned short ApiGetNumDevices();
	short ApiGetSerialNumber(unsigned short board_index);
	HANDLE ApiSetCurrentDevice(unsigned short BoardNum);
	HANDLE GetCurrentDeviceHandle();
	void ApiSetPreventUnderOverRuns(unsigned short BoardNum, bool Value);
	void ApiSetPioRegister(unsigned short BoardNum, unsigned long pio_offset, unsigned long data);
	unsigned long ApiGetPioRegister(unsigned short BoardNum, unsigned long pio_offset);
	void ApiSetShadowRegister(unsigned short BoardNum, unsigned long pio_offset, unsigned long data, unsigned long spiAddress, unsigned long mask_info);
	unsigned long ApiGetShadowRegister(unsigned short BoardNum, unsigned long pio_offset, unsigned long spiAddress, unsigned long mask_info);
	unsigned long ApiGetOverruns(unsigned short BoardNum);
#ifdef _WINDOWS
	DRV_VERSION ApiGetDriverVersion();
#endif



	void SetAD83000xReg(unsigned short BoardNum, unsigned int FunctionCode, unsigned int Setting); // WORD made to unsigned int
	void RunBoard(unsigned short BoardNum, bool run);
	void SetNumBlocksPerBoard(unsigned short BoardNum,unsigned long Data);	
	unsigned long GetBlocksSynthesized();
	void SetArmed(unsigned short BoardNum);
	void WriteConfigSpace(unsigned long Data);
	
	// Constructor / Destructor
	CDllDriverApi();
	virtual ~CDllDriverApi();
};



#endif
