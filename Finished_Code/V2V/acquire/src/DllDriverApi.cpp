/**
 * The DLL to device driver interface. This file is the implementation of the CDllDriverApi class. This class
 * contains all the handles to each device in the system and implements all calls to the device driver (expect system
 * calls directly from user programs e.g. read()).
 */

/*
"PIO_OFFSET_RS_SERIAL_RD           0x07 // 123109
"PIO_OFFSET_ADC_CHECKSUM           0x08 // 012610
"PIO_OFFSET_ADC_UNDEROVER          0x09 // 012610



#define	PIO_OFFSET_CONTROL                0x00
#define	PIO_OFFSET_BLOCKSIZE              0x01
#define	PIO_OFFSET_NUMBLOCKS              0x02
#define	PIO_OFFSET_PCIE_RD_START_BLOCK    0x03
#define	PIO_OFFSET_ADCSERIAL              0x04
#define	PIO_OFFSET_PROMPROG               0x05
#define	PIO_OFFSET_CAPTUREADJ             0x06
"PIO_OFFSET_DACSERIAL              0x07
"PIO_OFFSET_ADC_DAC_SERIAL         0x08
"PIO_OFFSET_DAC_RD_START_BLOCK     0x09
"PIO_OFFSET_PCIE_RD_END_BLOCK      0x0A
"PIO_OFFSET_DAC_RD_END_BLOCK       0x0B
"PIO_OFFSET_RS_SERIAL_WR           0x0C // 123109
"PIO_OFFSET_DRAM_ADJUST            0x0D // 122810
"PIO_OFFSET_DAC_WR_START_BLOCK     0x0E // 122910
"PIO_OFFSET_DAC_WR_END_BLOCK       0x0F // 122910
"PIO_OFFSET_ADC_DATA_SETUP         0x10 // 122910
"REG_11					          0x11 // 122910
"REG_12					          0x12 // 122910
"PIO_OFFSET_AVERAGER_CYCLES        0x13 // 122910
"PIO_OFFSET_TRIGGER_THRESHOLD      0x14 // 122910
"PIO_OFFSET_PRE_TRIGGER            0x15 // 122910
"PIO_OFFSET_ISLA_CAPTURE           0x16 // 122910
"PIO_OFFSET_AVERAGER_LEN			  0x17 // 110912
"REG_18							  0x18 // 110912
"REG_19			                  0x19 // 110912
"PIO_OFFSET_ECL_TRIGGER_DELAY	  0x1A // 060313
"PIO_OFFSET_12BIT_GLOBAL_IDELAY	  0x1B // 062313
#define	PIO_OFFSET_CAPTURE_DEPTH		  0x1C // 062513
#define	PIO_OFFSET_CAPTURE_COUNT		  0x1D // 062513
"PIO_OFFSET_SYNTH_REG			  0x1E // 062613
"PIO_OFFSET_FIRMWARE_VERSION_RD	  0x1F // 060513
 */



#include "DllDriverApi.h"
//#ifdef _MATLAB
//#include <mex.h>
//#endif
/*
The #include <mex.h> location is assumed to be the default install directory C:\Program Files\MATLAB\R2013a\extern\include.
For those wish to recompile the DLL, keep in mind the MATLAB version you are running and where the MATLAB directory is installed
on your local machine. To modify the path, switch the Solution Configuration to Matlab. Access Project -> AcqSynth Properties.
In the VC++ Directories tab, modify the Include Directories and Library Directories to point to the correct locations.
 */

#ifdef _WINDOWS
#include "DllWinDeviceList.h"
#include "Pcie5vDefines.h"
#include "DllWinGUIDs.h"

#include <winioctl.h>
#ifndef CTL_CODE
#pragma message("CTL_CODE undefined. Include winioctl.h or wdm.h")
#endif
#else
//    #include "Pcie5vDefines.h"
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>
#endif

extern "C" unsigned long LoadSvf(unsigned short BoardNum, char *filename, int reprogram_in);

#ifndef _WINDOWS
extern void Sleep(unsigned int num_milliseconds);
#endif

#ifndef _WINDOWS
int flag = 0;
struct uvdma_reg_values arg;
const char devnameprefix[12] = "/dev/uvdma";
#endif

unsigned long IoctlData[4];
unsigned long bytesWritten;
unsigned long data2;

#ifdef _WINDOWS
DRV_VERSION drvVersion = {0, 0, 0};
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDllDriverApi::CDllDriverApi() {
    BoardSerialNum = NULL;
    m_AllDeviceHandles = NULL;
    hCurrentDevice = INVALID_HANDLE_VALUE;
    bytesRead = 0;
    NumDevices = 0;
}

CDllDriverApi::~CDllDriverApi() {
    if(m_AllDeviceHandles != NULL) {
        int i;

        for(i = 0; i < NumDevices; i++) {
            x_Close(m_AllDeviceHandles[i]);
        }

        delete m_AllDeviceHandles;
    }

    if(BoardSerialNum != NULL) {
        delete BoardSerialNum;
    }
}

/**
 * Initialize the low-level API. Find all devices that our GUID is assigned to and get device handles for each.
 */
bool CDllDriverApi::ApiInitialize() {

    // Create space for all the board handles
    m_AllDeviceHandles = new HANDLE[MAX_DEVICES];

    BoardSerialNum = new short [MAX_DEVICES];
    for(unsigned short i = 0; i < MAX_DEVICES; i++)
        BoardSerialNum[i] = -1;


#ifdef _WINDOWS
    ApiGetDriverVersion();
    CDeviceList devices(GUID_DEVINTERFACE_AD83000X);
    int ndevices = devices.Initialize();

    // Save number of devices found in member variable so we can delete the handles created later
    NumDevices = ndevices;

    if(ndevices == 0) {
        printf("No devices to report\n");
        printf("If using 64-bit OS insure system is booted with F8 -> Disable Driver Signature Enforcement\n");
        return false;
    }

    char linkPath[MAX_PATH];

    for(unsigned short i = 0; i < ndevices; ++i) { // for each device
        CDeviceListEntry* dp = &devices.m_list[i];

        strcpy_s(linkPath, devices.m_list[i].m_linkname);
        //printf("linkPath is: %s \n\n", linkPath);
        //printf("Friendly Name is: %s \n\n", devices.m_list[i].m_friendlyname);
        //printf("LinkPath: %s \n", linkPath);  //110309
        // Sharing Parameter (3) must be zero for a communications resource;  Param 5 must be OPEN_EXISTING; Param 7 must be NULL
        m_AllDeviceHandles[i] = CreateFile(linkPath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL); // FILE_FLAG_OVERLAPPED

        BoardSerialNum[i] = LoadSvf(i, "get_usercode.svf", 0);
        printf("BoardNum[%d] = %d\n", i, BoardSerialNum[i]);
    }


    if(m_AllDeviceHandles == INVALID_HANDLE_VALUE) {
        printf("Can't Connect to device\n");
        return false;
    }
    else {
        hCurrentDevice = m_AllDeviceHandles[0];
    }

    return true;
#else

    // get number of devices found. Try to open device 0, if that fails then no devices are present. If it succeeds then use that as the handle to the driver
    // to find the total number of devices found.
    int i;
    char devname[50];
    char itoaval[10];

    strcpy(devname, devnameprefix);
    sprintf(itoaval, "%d", 0);
    strcat(devname, itoaval);

    printf("Opening device name %s \n", devname);
    fflush(stdout);
    m_AllDeviceHandles[0] = -1;
    m_AllDeviceHandles[0] = open(devname, O_RDWR);

    if(m_AllDeviceHandles[0] <= 0) {
        printf("No devices to report\n");
        return false;
    }

    hCurrentDevice = m_AllDeviceHandles[0];

    ioctl(hCurrentDevice, IOCTL_GET_NUM_DEVICES, &arg, 0);
    NumDevices = arg.data;

    i = 0;
    BoardSerialNum[i] = LoadSvf(i, (char*)"get_usercode.svf", 0);
    printf("BoardNum[%d] = %d\n", i, BoardSerialNum[i]);

    // for each device
    for(i = 1; i < NumDevices; i++) {
        strcpy(devname, devnameprefix);
        sprintf(itoaval, "%d", i);
        strcat(devname, itoaval);

        m_AllDeviceHandles[i] = open(devname, O_RDWR);

        if(m_AllDeviceHandles[i] <= 0) {
            printf("Error opening device %d\n", i);
            return false;
        }
        BoardSerialNum[i] = LoadSvf(i, (char*)"get_usercode.svf", 0);
        printf("BoardNum[%d] = %d\n", i, BoardSerialNum[i]);
    }

    return true;
#endif

    //printf("In ApiInitialize() \n");
    //exit(0);

}

/**
 * Return number of devices found in the system
 */
unsigned short CDllDriverApi::ApiGetNumDevices() {
    //    printf("In ApiGetNumDevices() \n");
    return NumDevices;
}

/**
 * Return number of devices found in the system
 */
short CDllDriverApi::ApiGetSerialNumber(unsigned short board_index) {
    short serialNumber = -1;
    if(board_index < MAX_DEVICES) {
        serialNumber = BoardSerialNum[board_index];
        if(serialNumber < 0)
            serialNumber = LoadSvf(board_index, (char*)"get_usercode.svf", 0);
    }
    return serialNumber;
    //printf("In ApiGetSerialNumber() \n");
    //return LoadSvf(board_index,"get_usercode.svf", 0);
}

/**
 * All functions in this file use hCurrentDevice as the device handle they target
 * This function sets hCurrentDevice to the requested device.
 * In order for this to work all higher level functions that don't operate through these
 * lower level functions must also use hCurrentDevice.
 */
HANDLE CDllDriverApi::ApiSetCurrentDevice(unsigned short BoardNum) {
    if(BoardNum < NumDevices) {
        hCurrentDevice = m_AllDeviceHandles[BoardNum];
        return hCurrentDevice;
    }
    else {
        return INVALID_HANDLE_VALUE;
    }
}

HANDLE CDllDriverApi::GetCurrentDeviceHandle() {
    return hCurrentDevice;
}

/**
 * Signal to the device driver to monitor the rate of AD/DA interrupts versus DMA interrupts to check for overruns
 * When set to false the driver will allow the user to read from the board without doing A/D at all (e.g. for a memory checker program)
 */
void CDllDriverApi::ApiSetPreventUnderOverRuns(unsigned short BoardNum, bool Value) {
    if(BoardNum < NumDevices) {
        hCurrentDevice = m_AllDeviceHandles[BoardNum];
        //return hCurrentDevice;
    }
    else {
        hCurrentDevice = INVALID_HANDLE_VALUE;
    }
    //HANDLE hCurrentDevice = device_number;

#ifdef _WINDOWS
    DeviceIoControl(hCurrentDevice, IOCTL_SET_PREVENT_UNDEROVERRUNS, &Value, sizeof (ULONG), NULL, 0, &bytesRead, NULL);
#else
    flag = 0;
    arg.data = Value;
    ioctl(hCurrentDevice, IOCTL_SET_PREVENT_UNDEROVER_RUNS, &arg, flag);
#endif
}

unsigned long CDllDriverApi::ApiGetDeviceStatus() {
#ifdef _WINDOWS
    if(!DeviceIoControl(hCurrentDevice, IOCTL_GET_STATUS, &IoctlData[0], sizeof (ULONG), &IoctlData[0], sizeof (ULONG), &bytesWritten, NULL)) {
        DWORD dw = GetLastError();
        printf("DeviceIOControl Error: %d\n", dw);
        return FALSE;
    }
    return IoctlData[0];
#else
    flag = 0;
    ioctl(hCurrentDevice, IOCTL_GET_STATUS, &arg, flag);
    return arg.data;
#endif
}

/**
 * Set the user PIO register at pio_offset to data value.
 */
void CDllDriverApi::ApiSetPioRegister(unsigned short BoardNum, unsigned long pio_offset, unsigned long data) {

#ifdef DEBUG_TO_FILE
    if(pio_offset != PIO_OFFSET_PROMPROG) {
        FILE *debugFile;
        debugFile = fopen("c:\\uvdma\\debug.txt", "a");
        long tm = GetTickCount();
        if((pio_offset >= 0) && (pio_offset < 32)) {
            fprintf(debugFile, "CDllDriverApi::ApiSetPioRegister\t%08ld\t%d\t%ld\t%s\t0\t0x%08lx\n", tm, BoardNum, pio_offset, reg_names[pio_offset], data);
        }
        else {
            fprintf(debugFile, "CDllDriverApi::ApiSetPioRegister\t%08ld\t%d\t%ld\t0\t0x%08lx\n", tm, BoardNum, pio_offset, data);
        }
        fflush(debugFile);
        fclose(debugFile);
    }
#endif

    if(BoardNum < NumDevices) {
        hCurrentDevice = m_AllDeviceHandles[BoardNum];
        //return hCurrentDevice;
    }
    else {
        hCurrentDevice = INVALID_HANDLE_VALUE;
    }
    //HANDLE hCurrentDevice = device_number;

    data2 = data;

#ifdef _WINDOWS
    IoctlData[0] = pio_offset; // User PIO Offset
    IoctlData[1] = data2; // User PIO Data
    IoctlData[2] = 0; // DON'T CARE, WON'T BE USED
    IoctlData[3] = 0; // DON'T CARE, WON'T BE USED

    if(!DeviceIoControl(hCurrentDevice, IOCTL_SET_USER_PIO_REG, IoctlData, 4 * sizeof (ULONG), NULL, 0, &bytesWritten, NULL)) {
        DWORD dw = GetLastError();
        printf("DeviceIOControl Error: %d\n", dw);
    }
#else
    // Specify the user PIO offset, driver will write to address multiplexer and then write to the user PIO write data register
    arg.pio_offset = pio_offset;
    arg.data = data2;
    flag = 0;
    ioctl(hCurrentDevice, IOCTL_SET_USER_PIO_REG, &arg, flag);
#endif
    //	Sleep(1);
}

unsigned long CDllDriverApi::ApiGetPioRegister(unsigned short BoardNum, unsigned long pio_offset) {
    if(BoardNum < NumDevices) {
        hCurrentDevice = m_AllDeviceHandles[BoardNum];
        //return hCurrentDevice;
    }
    else {
        hCurrentDevice = INVALID_HANDLE_VALUE;
    }
    //HANDLE hCurrentDevice = device_number;

#ifdef _WINDOWS
    IoctlData[0] = pio_offset;
    DeviceIoControl(hCurrentDevice, IOCTL_GET_USER_PIO_REG, &IoctlData[0], sizeof (ULONG), &IoctlData[0], sizeof (ULONG), &bytesWritten, NULL);
    return IoctlData[0];
#else
    // Specify the user PIO offset, driver will write to address multiplexer and then read from the user PIO read data register
    arg.pio_offset = pio_offset;
    flag = 0;
    ioctl(hCurrentDevice, IOCTL_GET_USER_PIO_REG, &arg, flag);
    return arg.data;
#endif
    //	Sleep(1);
}

/**
 * Set the user PIO register at pio_offset to data value.
 */
void CDllDriverApi::ApiSetShadowRegister(unsigned short BoardNum, unsigned long pio_offset, unsigned long data, unsigned long spiAddress, unsigned long mask_info) {
#ifdef DEBUG_TO_FILE
    if(pio_offset != PIO_OFFSET_PROMPROG) {
        FILE *debugFile;
        debugFile = fopen("c:\\uvdma\\debug.txt", "a");
        long tm = GetTickCount();
        if((pio_offset >= 0) && (pio_offset < 32)) {
            fprintf(debugFile, "CDllDriverApi::ApiSetShadowRegister\t%08ld\t%d\t%ld\t%s\t%ld\t0x%08lx\n", tm, BoardNum, pio_offset, reg_names[pio_offset], spiAddress, data);
        }
        else {
            fprintf(debugFile, "CDllDriverApi::ApiSetShadowRegister\t%08ld\t%d\t%ld\t%ld\t0x%08lx\n", tm, BoardNum, pio_offset, spiAddress, data);
        }
        fflush(debugFile);
        fclose(debugFile);
    }
#endif

    if(BoardNum < NumDevices) {
        hCurrentDevice = m_AllDeviceHandles[BoardNum];
        //return hCurrentDevice;
    }
    else {
        hCurrentDevice = INVALID_HANDLE_VALUE;
    }
    //HANDLE hCurrentDevice = device_number;

    data2 = data;

#ifdef _WINDOWS
    IoctlData[0] = pio_offset; // User PIO Offset
    IoctlData[1] = data2; // User PIO Data
    IoctlData[2] = spiAddress; // DON'T CARE, WON'T BE USED
    IoctlData[3] = mask_info; // DON'T CARE, WON'T BE USED

    if(!DeviceIoControl(hCurrentDevice, IOCTL_SET_SHADOW_REG, IoctlData, 4 * sizeof (ULONG), NULL, 0, &bytesWritten, NULL)) {
        DWORD dw = GetLastError();
        printf("DeviceIOControl Error: %d\n", dw);
    }
#else
    // Specify the user PIO offset, driver will write to address multiplexer and then write to the user PIO write data register
    arg.pio_offset = pio_offset;
    arg.data = data2;
    arg.spiAddress = spiAddress;
    arg.mask_info = mask_info;
    flag = 0;
    ioctl(hCurrentDevice, IOCTL_SET_SHADOW_REG, &arg, flag);
#endif
    Sleep(1);
}

/**
 * Set the user PIO register at pio_offset to data value.
 */
unsigned long CDllDriverApi::ApiGetShadowRegister(unsigned short BoardNum, unsigned long pio_offset, unsigned long spiAddress, unsigned long mask_info) {
    if(BoardNum < NumDevices) {
        hCurrentDevice = m_AllDeviceHandles[BoardNum];
        //return hCurrentDevice;
    }
    else {
        hCurrentDevice = INVALID_HANDLE_VALUE;
    }
    //HANDLE hCurrentDevice = device_number;

#ifdef _WINDOWS
    IoctlData[0] = pio_offset;
    IoctlData[1] = spiAddress;
    IoctlData[2] = mask_info;
    IoctlData[3] = 0;
    DeviceIoControl(hCurrentDevice, IOCTL_GET_SHADOW_REG, &IoctlData, 4 * sizeof (ULONG), &IoctlData[0], sizeof (ULONG), &bytesWritten, NULL);
    return IoctlData[0];
#else
    // Specify the user PIO offset, driver will write to address multiplexer and then read from the user PIO read data register
    arg.pio_offset = pio_offset;
    arg.spiAddress = spiAddress;
    arg.mask_info = mask_info;
    flag = 0;
    ioctl(hCurrentDevice, IOCTL_GET_SHADOW_REG, &arg, flag);
    return arg.data;
#endif
    Sleep(1);
}

#ifdef _WINDOWS

DRV_VERSION CDllDriverApi::ApiGetDriverVersion() {

    bool success;
    success = DeviceIoControl(hCurrentDevice, (DWORD)IOCTL_GET_VERSION, &IoctlData, 4 * sizeof (ULONG), &IoctlData, 4 * sizeof (ULONG), &bytesWritten, NULL);
    //printf("Driver Version %d.%d.%d\n", IoctlData[2], IoctlData[1], IoctlData[0]);
    drvVersion.major = IoctlData[2];
    drvVersion.minor = IoctlData[1];
    drvVersion.minor = IoctlData[0];
    return drvVersion;
}
#else

#endif

unsigned long CDllDriverApi::ApiGetOverruns(unsigned short BoardNum) {
    if(BoardNum < NumDevices) {
        hCurrentDevice = m_AllDeviceHandles[BoardNum];
        //return hCurrentDevice;
    }
    else {
        hCurrentDevice = INVALID_HANDLE_VALUE;
    }
    //HANDLE hCurrentDevice = device_number;

#ifdef _WINDOWS
    DeviceIoControl(hCurrentDevice, IOCTL_GET_OVERRUNS, &IoctlData[0], sizeof (ULONG), &IoctlData[0], sizeof (ULONG), &bytesWritten, NULL);
    return IoctlData[0];
#else
    flag = 0;
    ioctl(hCurrentDevice, IOCTL_GET_OVERRUNS, &arg, flag);
    return arg.data;
#endif
}

/**
 * Inititation of acquistion is caused by a rising edge, write run value to user control register
 */
void CDllDriverApi::RunBoard(unsigned short BoardNum, bool run) {
#ifdef DEBUG_TO_FILE
    char *runTxt[2] = {"false", "true"};
    int index = 0;
    FILE * debugFile;
    debugFile = fopen("c:\\uvdma\\debug.txt", "a");
    long tm = GetTickCount();
    if(run) {
        index = 1;
    }

    fprintf(debugFile, "CDllDriverApi::RunBoard\t%s\n", tm, BoardNum, runTxt[index]);
    fflush(debugFile);
    fclose(debugFile);
#endif

    if(BoardNum < NumDevices) {
        hCurrentDevice = m_AllDeviceHandles[BoardNum];
        //return hCurrentDevice;
    }
    else {
        hCurrentDevice = INVALID_HANDLE_VALUE;
    }
    //HANDLE hCurrentDevice = device_number;

#ifdef _WINDOWS
    IoctlData[0] = PIO_OFFSET_CONTROL; // User PIO Offset
    IoctlData[1] = 0; // User PIO Data, DON'T CARE, WON'T BE USED
    IoctlData[2] = SOFTWARE_RUN_BIT; // user_control bit_pos
    IoctlData[3] = run; // user_control bit_val

    DeviceIoControl(hCurrentDevice, IOCTL_SET_USER_PIO_REG, IoctlData, 4 * sizeof (ULONG), NULL, 0, &bytesWritten, NULL);
#else
    // issue an IOCTL to set the software run bit in the control register to requested value
    arg.changed_bits = (1 << SOFTWARE_RUN_BIT);
    arg.new_bit_value = (run << SOFTWARE_RUN_BIT);

    // Specify the user PIO offset, driver will write to address multiplexer and then write to the user PIO write data register
    arg.pio_offset = PIO_OFFSET_CONTROL;
    arg.data = 0; // data doesn't matter
    ioctl(hCurrentDevice, IOCTL_SET_USER_PIO_REG, &arg, flag);
#endif
}

/**
 * Primitive used by higher level functions to change a field in the board's register to
 * the desired setting. Function returns the status of the change and prints to screen in event of error.
 */
void CDllDriverApi::SetAD83000xReg(unsigned short BoardNum, unsigned int FunctionCode, unsigned int Setting) {
#ifdef DEBUG_TO_FILE
    FILE *debugFile;
    debugFile = fopen("c:\\uvdma\\debug.txt", "a");
    long tm = GetTickCount();
    if(FunctionCode < 32)
        fprintf(debugFile, "CDllDriverApi::SetAD83000xReg\t\t%08ld\t%d\t%d\t%s\t0x%x\n", tm, BoardNum, FunctionCode, controlRegBitNames[FunctionCode], Setting);
    else
        fprintf(debugFile, "CDllDriverApi::SetAD83000xReg\t\t%08ld\t%d\t%d\t\t0x%x\n", tm, BoardNum, FunctionCode, Setting);

    fflush(debugFile);
    fclose(debugFile);
#endif

    //    long tm = GetTickCount();
    //    printf("CDllDriverApi::SetAD83000xReg\t\t%08ld\t%d\t%d\t%s\t0x%x\n",tm  ,BoardNum,  FunctionCode, controlRegBitNames[FunctionCode],  Setting);
    fflush(stdout);
    if(BoardNum < NumDevices) {
        hCurrentDevice = m_AllDeviceHandles[BoardNum];
        //return hCurrentDevice;
    }
    else {
        hCurrentDevice = INVALID_HANDLE_VALUE;
    }
    //HANDLE hCurrentDevice = device_number;

#ifdef _WINDOWS
    IoctlData[0] = PIO_OFFSET_CONTROL; // User PIO Offset
    IoctlData[1] = 0; // User PIO Data, DON'T CARE, WON'T BE USED
    IoctlData[2] = FunctionCode; // user_control bit_pos
    IoctlData[3] = Setting; // user_control bit_val

    DeviceIoControl(hCurrentDevice, IOCTL_SET_USER_PIO_REG, IoctlData, 4 * sizeof (ULONG), NULL, 0, &bytesWritten, NULL);
#else
    // 031909 made simpiler to user, also makes compatible with Windows format (facilitates dual compilation)
    arg.changed_bits = (1 << FunctionCode);
    arg.new_bit_value = (Setting << FunctionCode);

    // Specify the user PIO offset, driver will write to address multiplexer and then write to the user PIO write data register
    arg.pio_offset = PIO_OFFSET_CONTROL;
    arg.data = 0; // data doesn't matter
    ioctl(hCurrentDevice, IOCTL_SET_USER_PIO_REG, &arg, flag);
#endif
}

void CDllDriverApi::SetArmed(unsigned short BoardNum) {
#ifdef DEBUG_TO_FILE
    FILE *debugFile;
    debugFile = fopen("c:\\uvdma\\debug.txt", "a");
    long tm = GetTickCount();
    fprintf(debugFile, "CDllDriverApi::SetArmed\t\t\t%08ld\t%d\n", tm, BoardNum);
    fflush(debugFile);
    fclose(debugFile);
#endif

    if(BoardNum < NumDevices) {
        hCurrentDevice = m_AllDeviceHandles[BoardNum];
        //return hCurrentDevice;
    }
    else {
        hCurrentDevice = INVALID_HANDLE_VALUE;
    }
    //HANDLE hCurrentDevice = device_number;

#ifdef _WINDOWS
    DeviceIoControl(hCurrentDevice, IOCTL_SET_ARMED, NULL, 0, &IoctlData[0], sizeof (ULONG), &bytesRead, NULL);
#else
    ioctl(hCurrentDevice, IOCTL_SET_ARMED, &arg, flag);
#endif
}

void CDllDriverApi::WriteConfigSpace(unsigned long Data) {
#ifdef _WINDOWS
    DeviceIoControl(hCurrentDevice, IOCTL_WRITE_CONFIG_SPACE, &Data, sizeof (ULONG), NULL, 0, &bytesRead, NULL);
#else
    ioctl(hCurrentDevice, IOCTL_WRITE_CONFIG_SPACE, &arg, flag);
#endif
}

unsigned long CDllDriverApi::GetBlocksSynthesized() {
#ifdef _WINDOWS
    DeviceIoControl(hCurrentDevice, IOCTL_GET_BLOCKS_SYNTHESIZED, &IoctlData[0], sizeof (ULONG), &IoctlData[0], sizeof (ULONG), &bytesWritten, NULL);
    return IoctlData[0];
#else
    ioctl(hCurrentDevice, IOCTL_GET_BLOCKS_SYNTHESIZED, &arg, flag);
    return arg.data;
#endif
}

void CDllDriverApi::SetNumBlocksPerBoard(unsigned short BoardNum, unsigned long Data) {
#ifdef DEBUG_TO_FILE
    FILE *debugFile;
    debugFile = fopen("c:\\uvdma\\debug.txt", "a");
    long tm = GetTickCount();
    fprintf(debugFile, "CDllDriverApi::SetNumBlocksPerBoard\t%08ld\t%d\t%ld\n", tm, BoardNum, Data);
    fflush(debugFile);
    fclose(debugFile);
#endif

    if(BoardNum < NumDevices) {
        hCurrentDevice = m_AllDeviceHandles[BoardNum];
        //return hCurrentDevice;
    }
    else {
        hCurrentDevice = INVALID_HANDLE_VALUE;
    }

#ifdef _WINDOWS
    DeviceIoControl(hCurrentDevice, IOCTL_SET_NUM_BLOCKS_PER_BOARD, &Data, sizeof (ULONG), NULL, 0, &bytesRead, NULL);
#else
    arg.data = Data;
    ioctl(hCurrentDevice, IOCTL_SET_NUM_BLOCKS_PER_BOARD, &arg, flag);
#endif
}