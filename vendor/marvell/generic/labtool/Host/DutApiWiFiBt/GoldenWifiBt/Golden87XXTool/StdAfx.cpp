// stdafx.cpp : source file that includes just the standard includes
//	GoldenAPTool.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
#include <stdio.h>
#include <stdarg.h>
int DebugLogRite(const char *string, ... )
{
   
    va_list   va;
	int err=0;
	FILE *hFile=0; 

	hFile = fopen("Test.txt","a+"); 

	va_start(va, string);
	err = vprintf(string, va);
	vfprintf(hFile, string, va);
    va_end(va);

	fclose(hFile); 

	return err;
}