/*

Implements the dynamic loading of the DLL and fills out the DLL structure with the exported DLL function handles.

Implements the cross platform functions, which facilitate cross platform compilation.

*/



#include "AppDll.h"
#include <stdio.h>
#include <time.h>

#include "DllLibMain.h"

#ifdef _MATLAB
#include <mex.h>
#endif
/*
The #include <mex.h> location is assumed to be the default install directory C:\Program Files\MATLAB\R2013a\extern\include.
For those wish to recompile the DLL, keep in mind the MATLAB version you are running and where the MATLAB directory is installed
on your local machine. To modify the path, switch the Solution Configuration to Matlab. Access Project -> AcqSynth Properties. 
In the VC++ Directories tab, modify the Include Directories and Library Directories to point to the correct locations.
*/

#define EXPORT_FCNS


#ifdef _WINDOWS
#include <mutex>
#include "shrhelp.h"
#endif

CDllDriverApi *pAPI;
int busy;


//extern std::mutex makeAtomic[MAX_DEVICES];
////////////////////////////////////////////////////////////////////////////////////
//Memory Management Primitives

// Wrapper for memory allocation.
// Will return 1 if there was an error allocating the buffer
// Will return 0 if buffer was allocated successfully

#ifndef _MATLAB
EXPORTED_FUNCTION int x_MemAlloc(void ** buf_addr, size_t buf_size) 
#else
int x_MemAlloc(void ** buf_addr, size_t buf_size) 
#endif
{
    
#ifndef _WINDOWS
    int error; 
    error = posix_memalign(buf_addr, 4096, buf_size);	
    return error;

//Windows Section
#else
    *buf_addr = VirtualAlloc(NULL, buf_size, MEM_COMMIT, PAGE_READWRITE);
    if(buf_addr == NULL){
        return 1;
    }
    else{
        return 0;
    }

#endif


    return 1;
}

// Wrapper for freeing the memory allocation 
#ifndef _MATLAB
EXPORTED_FUNCTION void x_FreeMem(void * buf)
#else
void x_FreeMem(void * buf)
#endif

{

#ifndef _WINDOWS
    free(buf);

//Windows Section
#else
    VirtualFree(buf, 0, MEM_RELEASE);
#endif

}

////////////////////////////////////////////////////////////////////////////////////
//File Primitives

#ifndef _MATLAB
EXPORTED_FUNCTION HANDLE x_CreateFile(char * filename)
#else
HANDLE x_CreateFile(char * filename)
#endif
{
    HANDLE fd;
#ifndef _WINDOWS
    fd = open(filename,O_CREAT|O_WRONLY|O_TRUNC,0666);

#else
    fd = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
#endif 
    if (fd < 0 ) {printf("open %s failed\n", filename);}
    return fd;
}

 
#ifndef _MATLAB
EXPORTED_FUNCTION HANDLE x_OpenFile(char * filename)
#else
HANDLE x_OpenFile(char * filename)
#endif
{
	    HANDLE fd;


#ifndef _WINDOWS
    fd =  open(filename,O_RDWR);

#else
    fd =  CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
#endif 
    if ((long)fd < 0 ) 
	{
		printf("open %s failed\n", filename);
//		DWORD error = GetLastError();
//		printf("open error code = %ld\n", error);
//    LPVOID lpMsgBuf;
//    LPVOID lpDisplayBuf;
//    DWORD dw = GetLastError();

//    FormatMessage(
//        FORMAT_MESSAGE_ALLOCATE_BUFFER |
//        FORMAT_MESSAGE_FROM_SYSTEM |
//        FORMAT_MESSAGE_IGNORE_INSERTS,
//        NULL,
//        dw,
//        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//        (LPTSTR) &lpMsgBuf,
//        0, NULL );
//		printf("%s\n",lpMsgBuf);

	}
	else
	{
		printf("open success HANDLE=%ld\n",(long)fd);
	}

	fflush(stdout);
    return fd;
}

// Wrapper for seeking within a File. Offset must be from the beginning of the file.
void x_FileSeek(HANDLE fd, off_t offset)
{
#ifndef _WINDOWS
    lseek(fd, offset ,SEEK_SET);
#else
    LARGE_INTEGER largeint;
    largeint.QuadPart = 0;
    largeint.HighPart = 0;
    largeint.LowPart = 0;
    SetFilePointerEx(fd, largeint, NULL, FILE_BEGIN);
#endif
}


// Return the file size in DIG_BLOCKSIZE units
unsigned long x_GetFileSizeInBlocks(HANDLE fd)
{
    unsigned long NumBlocks;
#ifndef _WINDOWS
    struct stat stat_struct;
    fstat(fd, &stat_struct);
    NumBlocks = (int)(stat_struct.st_size/DIG_BLOCKSIZE);
#else

    double bignum, logbase2;
    LARGE_INTEGER largeint;

    GetFileSizeEx(fd, &largeint);
    //printf("highpart=%d, lowpart=%d \n", largeint.HighPart, largeint.LowPart); 

    logbase2 = log((double)DIG_BLOCKSIZE)/log((double)2);       // log base2 of the blocksize

    bignum = ((largeint.HighPart << (32-(int)logbase2)));       // HighPart of the large integer in number of blocks
    NumBlocks = (unsigned int) bignum + (largeint.LowPart/DIG_BLOCKSIZE);

#endif
    return NumBlocks;
}


// Wrapper for closing a file.
#ifndef _MATLAB
EXPORTED_FUNCTION void x_Close(HANDLE fd)
#else
void x_Close(HANDLE fd)
#endif
{
#ifndef _WINDOWS
    close(fd);
#else
    CloseHandle(fd);
#endif
}


////////////////////////////////////////////////////////////////////////////////////
//I/O Primitives

// Return is an error value.
#ifndef _MATLAB
EXPORTED_FUNCTION int x_Read (unsigned short BoardNum, void * sysMem, size_t rd_size)
#else
int x_Read (unsigned short BoardNum, void * sysMem, size_t rd_size)
#endif
{
 	 int ret;

#ifdef DEBUG_TO_FILE
	FILE *debugFile;
	debugFile = fopen("c:\\uvdma\\debug.txt","a");
	long tm = GetTickCount();
	fprintf(debugFile,"LOCKED  x_Read\t%08ld\t%d\n",tm  ,BoardNum);
	fflush(debugFile);
	fclose(debugFile);
#endif


#ifndef _WINDOWS
//	makeAtomic[BoardNum].lock();
    return read(pAPI->m_AllDeviceHandles[BoardNum], sysMem, rd_size);
//	makeAtomic[BoardNum].unlock();
#else

//	makeAtomic[BoardNum].lock();
	ULONG bytesRead;

	if(BoardNum < pAPI->NumDevices)
    {
        ret = ReadFile( pAPI->m_AllDeviceHandles[BoardNum], sysMem, (DWORD)rd_size, &bytesRead, NULL); // 063010 use rd_size, not DIG_BLOCKSIZE !!!
		if(!ret)		// If single block local memory buffer
		{
			printf("ReadFile failed.  bytesRead= %d\n", bytesRead);
			printf("attempted to read %d\n", DIG_BLOCKSIZE);
			LPVOID lpMsgBuf;
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				0, // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL 
				);
			printf("%s\n", lpMsgBuf);
			printf("end of !ret");
		}
	}
//	makeAtomic[BoardNum].unlock();
    return ret;
#endif
}


// Return is an error value.
#ifndef _MATLAB
EXPORTED_FUNCTION int x_Write(HANDLE fd, void * sysMem, DWORD rd_size)
#else
int x_Write(HANDLE fd, void * sysMem, DWORD rd_size)
#endif
{
    int ret;
#ifndef _WINDOWS
    return write(fd, sysMem, rd_size);
#else
    ULONG bytesRead;
    ret = WriteFile(fd, sysMem, rd_size, &bytesRead, NULL); // 063010 use rd_size here not DIG_BLOCKSIZE !!!
    if(!ret)
    {
        printf("WriteFile Failed\n\n");
	
        LPVOID lpMsgBuf;
	
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, NULL );

            printf("WriteFile Error::  %s\n", lpMsgBuf);
    }
    return ret;
#endif
    return 0;

}
















