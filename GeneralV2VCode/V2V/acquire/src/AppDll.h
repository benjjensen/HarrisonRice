/*

Defines the DLL structure with handles to the various DLL functions, and definitions of the exported DLL functions.

Defines the cross platform functions, which facilitate cross platform compilation.

*/

#ifndef _APP_FUNCS_H_
#define _APP_FUNCS_H_



#include "Pcie5vDefines.h"

#define EXPORT_FCNS

#include "DllDriverApi.h"

#ifndef _WINDOWS
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <fcntl.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <time.h>
    #include <unistd.h>
    #include <signal.h>
    #include <math.h>
#else
#include "shrhelp.h"

// Stdafx.h : Include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN    // Exclude rarely-used items from Windows headers.
#endif

// Modify the following defines if you have to target an OS before the ones specified in the following code. See MSDN for the 
// latest information about corresponding values for different operating systems.
#ifndef WINVER                  // Permit use of features specific to Windows 95 and Windows NT 4.0 or later.
#define WINVER 0x0400	        // Change this to the appropriate value to target 
#endif

#ifndef _WIN32_WINNT            // Permit use of features specific to Windows NT 4.0 or later.
#define _WIN32_WINNT 0x0502
#endif			

#ifndef _WIN32_WINDOWS          // Permit use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410   // Change this to the appropriate value to target 
#endif

#ifndef _WIN32_IE               // Permit use of features specific to Internet Explorer 4.0 or later.
#define _WIN32_IE 0x0400        // Change this to the appropriate value to target 
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS  // Some CString constructors will be explicit.

// Turns off MFC feature that hides of some common warning messages that are frequently and safely ignored .
#define _AFX_ALL_WARNINGS

#include <afxtempl.h>	// Must be included due to DeviceList dependence on objects (CArrray ... This file will require linking to MFC 


    #include "Windows.h"
    #include <time.h>
    #include <math.h>

    // only needed for direct call of IOCTL
    #include <winioctl.h>
    #ifndef CTL_CODE
        #pragma message("CTL_CODE undefined. Include winioctl.h or wdm.h")
    #endif

#endif

#define SAMPLE_TYPE                  unsigned int    // ADC samples read from any resolution board are converted to this type
#define FILE_HANDLE_TYPE             int

// Platform specific mappings
#ifdef _WINDOWS

    #define TIME_VAR_TYPE                   DWORD       // 111110, changed from using time() to GetTickCount()

#else
    #define HANDLE                         int              // HANDLE is only defined in Windows
    #define INVALID_HANDLE_VALUE           -1
    #define TIME_VAR_TYPE                  struct timeval
    #define HINSTANCE                      void *           // In Linux the "DLL" is just a static library that is linked with at compile time and this pointer isn't used
    #define DWORD                          size_t
#endif

// Memory Management Primitives
#ifndef _MATLAB
EXPORTED_FUNCTION int x_MemAlloc(void ** buf_addr, size_t buf_size) ;
EXPORTED_FUNCTION int x_Read (unsigned short BoardNum, void * sysMem, size_t rd_size);
EXPORTED_FUNCTION void x_FreeMem(void * buf);
EXPORTED_FUNCTION SAMPLE_TYPE GetSample(unsigned short BoardNum, void * pBuffer, unsigned int index, unsigned short channel);
EXPORTED_FUNCTION int x_Write(HANDLE fd, void * sysMem, DWORD rd_size);
EXPORTED_FUNCTION HANDLE x_CreateFile(char * filename);
EXPORTED_FUNCTION HANDLE x_OpenFile(char * filename);
EXPORTED_FUNCTION void x_Close(HANDLE fd);
EXPORTED_FUNCTION void x_FreeMem(void * buf);
#else
int x_MemAlloc(void ** buf_addr, size_t buf_size) ;
int x_Read (unsigned short BoardNum, void * sysMem, size_t rd_size);
int x_Write(HANDLE fd, void * sysMem, DWORD rd_size);
HANDLE x_CreateFile(char * filename);
HANDLE x_OpenFile(char * filename);
void x_Close(HANDLE fd);
void x_FreeMem(void * buf);
#endif

void x_FileSeek(HANDLE fd, off_t offset);
unsigned long x_GetFileSizeInBlocks(HANDLE fd);

#ifdef _WINDOWS
#else
#define DWORD size_t
#endif

#endif