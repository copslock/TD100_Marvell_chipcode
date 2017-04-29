/** @file utilities.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#ifndef _LINUX_
#include <windows.h>

#ifdef __cplusplus
extern "C" { 			/* Assume C declarations for C++   */
#endif  /* __cplusplus */

#else
#include "Clss_os.h"
#define _stdcall
//typedef unsigned long DWORD;
//typedef unsigned int BOOL;
//typedef unsigned int BOOLEAN;
#endif	//#ifndef _LINUX_
#include <stdio.h>
//#define DebugLogRite DebugLogRiteEx
//#define SetOutputFileName SetOutputFileNameEx
//#define PrintOnScreen PrintOnScreenEx

#if defined(DUTAPI878XDLL_EXPORTS)
__declspec(dllexport) void EndianSwapeByte(DWORD *data, int cnt);
__declspec(dllexport) void EndianSwapeByteArray(BYTE *data, int cnt);
__declspec(dllexport) void SortChar(int NumOfData, char IndexArray[], 
			  char Data1[], char Data2[], char Data3[], char Data4[],
			  char Data5[], char Data6[], char Data7[], char Data8[]); 
__declspec(dllexport) void SortIntChar(int NumOfData, int IndexArray[], 
				 char Data1[], char Data2[], char Data3[], char Data4[], 
				 char Data5[], char Data6[], char Data7[], char Data8[], 
				 char Data9[], char Data10[]);
__declspec(dllexport) void SortDoubleChar(int NumOfData, double IndexArray[],
					char Data1[], char Data2[], char Data3[], char Data4[],
					char Data5[], char Data6[], char Data7[], char Data8[],
					char Data9[], char Data10[]);
__declspec(dllexport) int _stdcall DebugLogRite(const char *string, ... );
__declspec(dllexport) int _stdcall SetOutputFileName(char *pFileName);
__declspec(dllexport) int _stdcall PrintOnScreen(BOOLEAN flag);
__declspec(dllexport) void EndianSwapByteArray(BYTE *data, int cnt);
__declspec(dllexport) void EndianSwapByte(DWORD *data, int cnt);
__declspec(dllexport) void EndianSwapWord(DWORD *data, int cnt); 
__declspec(dllexport) void EndianSwapBytesWord(DWORD *data, int cnt);
__declspec(dllexport) void EndianSwapWordBytes(DWORD *data, int cnt);
__declspec(dllexport) BOOL IsFileExist(char *filenameL);
//__declspec(dllexport)boll  IsFileExist(char *FileName); 
__declspec(dllexport) int String2CharArray(char* string, char *deliminators, int NumOfData, char *data);
__declspec(dllexport) int String2IntArray(char* string, char *deliminators, int NumOfData, int *data);
__declspec(dllexport) int String2ByteArray(char* string, char *deliminators, int NumOfData, BYTE *data);
__declspec(dllexport) int CharArray2String(char* string, char *deliminators, int NumOfData, char *data);
__declspec(dllexport) int IntArray2String(char* string, char *deliminators, int NumOfData, int *data); 
__declspec(dllexport) int ByteArray2String(char* string, char *deliminators, int NumOfData, BYTE *data);

__declspec(dllexport) int ctoi(char inVal);
__declspec(dllexport) int BlockReadFromBin(BYTE *buffer, int blockSize, FILE *srcFile);
__declspec(dllexport) int BlockReadFromTextFile(BYTE *buffer, int blockSize, FILE *srcFile);

#else
void EndianSwapeByte(DWORD *data, int cnt);
void EndianSwapeByteArray(BYTE *data, int cnt);
void SortChar(int NumOfData, char IndexArray[], 
			  char Data1[], char Data2[], char Data3[], char Data4[],
			  char Data5[], char Data6[], char Data7[], char Data8[]);

void SortIntChar(int NumOfData, int IndexArray[], 
				 char Data1[], char Data2[], char Data3[], char Data4[], 
				 char Data5[], char Data6[], char Data7[], char Data8[], 
				 char Data9[], char Data10[]);
void SortDoubleChar(int NumOfData, double IndexArray[],
					char Data1[], char Data2[], char Data3[], char Data4[],
					char Data5[], char Data6[], char Data7[], char Data8[],
					char Data9[], char Data10[]);




 
int _stdcall DebugLogRite(const char *string, ... );
int _stdcall SetOutputFileName(char  *pFileName);
int _stdcall PrintOnScreen(BOOLEAN flag);
void EndianSwapByteArray(BYTE *data, int cnt);
void EndianSwapByte(DWORD *data, int cnt);
void EndianSwapWord(DWORD *data, int cnt); 
void EndianSwapBytesWord(DWORD *data, int cnt);
void EndianSwapWordBytes(DWORD *data, int cnt);
BOOL IsFileExist(char *filenameL);
//int  IsFileExist(char *FileName);   

int String2CharArray(char* string, char *deliminators, int NumOfData, char *data);
int String2IntArray(char* string, char *deliminators, int NumOfData, int *data);
int String2ByteArray(char* string, char *deliminators, int NumOfData, BYTE *data);
int CharArray2String(char* string, char *deliminators, int NumOfData, char *data);
int IntArray2String(char* string, char *deliminators, int NumOfData, int *data); 
int ByteArray2String(char* string, char *deliminators, int NumOfData, BYTE *data);

int ctoi(char inVal);
int BlockReadFromBin(BYTE *buffer, int blockSize, FILE *srcFile);
int BlockReadFromTextFile(BYTE *buffer, int blockSize, FILE *srcFile);
int maximum(int Num, int *Array);

int minimum(int Num, int *Array);
#ifndef _LINUX_
__int64 atoi64(char *s);
#endif	//#ifndef _LINUX_

#endif


#ifndef _LINUX_
#ifdef __cplusplus
}			/* Assume C declarations for C++   */
#endif  /* __cplusplus */
#endif	//#ifndef _LINUX_
