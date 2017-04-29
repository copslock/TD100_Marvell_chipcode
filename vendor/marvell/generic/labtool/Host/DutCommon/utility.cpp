/** @file utility.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

//#include <stdio.h>
//#include <stdarg.h>
//#include <afx.h>
#include "Clss_os.h"

#include "utility.h"
#if !defined(_W8787_) && !defined(_W8782_)
static char LogFileName[_MAX_FNAME]="Test.txt";
#endif
/*
extern "C" int STDCALL CleanDebugLogFile(void)
{
	FILE *hFile=0; 
	hFile = fopen(LogFileName,"w"); 
	fclose(hFile); 
	return 0;
}
*/
/*
extern "C" int STDCALL DebugLogRite(const char *string, ... )
{
   
    va_list   va;
	int err=0;
	FILE *hFile=0; 

//	CTime theTime = CTime::GetCurrentTime();

	hFile = fopen(LogFileName,"a+"); 
//	err = printf(theTime.Format("%m:%d:%Y %H:%M:%S\t"));
//	fprintf(hFile, theTime.Format("%m:%d:%Y %H:%M:%S\t"));

	va_start(va, string);
	vfprintf(hFile, string, va);
	fflush(hFile);
	err = vprintf(string, va);
	fflush(stdout);
    va_end(va);

	fclose(hFile); 

	return err;
}
*/



int CountBit1(char data)
{

	int cnt=0;
	while (data!=0)
	{
		data ^=data &(-data);
		cnt++;
	}
	return cnt;
}

