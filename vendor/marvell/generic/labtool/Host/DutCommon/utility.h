/** @file utility.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

//#include <windows.h>


#ifdef _USRDLL 
//extern "C" __declspec(dllexport) int  _stdcall DebugLogRite(const char *string, ... );  
//extern "C" __declspec(dllexport) int  _stdcall CleanDebugLogFile(void);  
#else
//extern "C" int  DebugLogRite(const char *string, ... );  
//extern "C" int  CleanDebugLogFile(void);
#endif

#define MFG_VERSION(a, b, c, d) ((a<<24)| (b<<16)|(c<<8)|d)

int CountBit1(char data);
