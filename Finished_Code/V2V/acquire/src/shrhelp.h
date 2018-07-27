/* Copyright 2007 The MathWorks, Inc. */

/* shrhelp.h
 * Shared library helper include file for cross platform portability
 * define EXPORT_FCNS before including this file in source files that build the library
 * no defines are needed to use the library. */
 
#ifndef SHRHELP
#define SHRHELP

#ifdef _WIN32
#ifdef EXPORT_FCNS

	#ifdef __cplusplus
		#define EXPORTED_FUNCTION extern "C" __declspec (dllexport)
	#else
		#define EXPORTED_FUNCTION __declspec (dllexport)
	#endif

#else
	#ifdef __cplusplus
		#define EXPORTED_FUNCTION extern "C" __declspec (dllimport)
	#else
		#define EXPORTED_FUNCTION __declspec (dllimport)
	#endif
#endif
#else
#define EXPORTED_FUNCTION
#endif

#endif

