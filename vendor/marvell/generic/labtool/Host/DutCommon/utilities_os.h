/** @file utilities_os.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#if !defined(_UTILITES_OS_H_)
#define _UTILITES_OS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _LINUX_
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
//int TestOnlyOnce;

extern int debugLog;
//#define _MAX_PATH 256 //260?
#else
#include "stdafx.h"
#include <tchar.h>
#endif

//#include "DutApi83xxpDll.h"
#ifndef MAX_LENGTH
#define MAX_LENGTH 256
#endif 

#ifdef _LINUX_
int GetPrivateProfileSection(char *tag, char *key, int len, char *FileName);
int WritePrivateProfileString(char *tag, char *key, char *buf, char *FileName);
int GetPrivateProfileString(char *tag, char *key, char *defValue, char *buf, 
							int bufLenInByte, char *FileName);
int GetPrivateProfileInt(char *tag,char *key , int defValue, char *FileName);

long GetTickCount();
int strnicmp(const char *str1, const char *str2, size_t count);
#define LN_LINE_END		(1)
#else //#ifdef _LINUX_

#define LN_LINE_END		(2)
#endif //#ifdef _LINUX_
off_t filelength(const char *filename);
//void FlexSpiCompare(void);  
//void FlexSpiDL(void);  
//int FileLoad(char	*FileName, unsigned long *FileContent, int SizeInDword);
int txtPtrCnt(int a);

#endif
