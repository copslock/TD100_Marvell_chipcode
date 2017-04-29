/** @file utilities.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#ifdef _LINUX_
#define _stdcall
#endif	//ifdef _LINUX_

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "utilities.h"
static int guiHandle=0; 
static BOOLEAN printFlag=1;

char OUTPUT_FILE[256]="Test.txt";

int _stdcall SetGuiLogHandle(int h) {return guiHandle =h;}

int _stdcall PrintOnScreen(BOOLEAN flag){ return printFlag=flag;}

int _stdcall DebugLogRite(const char *string, ... ) 
{
    va_list   va;
	int err=0;
	FILE *hFile=0; 

	hFile = fopen(OUTPUT_FILE,"a+"); 

	va_start(va, string);
	if(hFile)
		vfprintf(hFile, string, va);
	if(!guiHandle)
	{
		if(printFlag) 
		{
			err = vprintf(string, va);
		}
		else
		{
			printFlag=1;
		}
	}
	else
	{	char tempString[1024]="";
		vsprintf(tempString, string, va); 
	}
    va_end(va);

	if(hFile)
		fclose(hFile); 

	return 0;
}

int _stdcall SetOutputFileName(char *pFileName)
{
	int status = 0;

	if (pFileName)
	{
		DebugLogRite("Set the output file name to %s\n", pFileName);
		memcpy(OUTPUT_FILE, pFileName, 256);
	}
	return status;
}

void EndianSwapByteArray(BYTE *data, int cnt)
{
	int index=0;
	BYTE *ptrByte=NULL;
	DWORD temp=0;
	 
	ptrByte = (BYTE*)malloc(cnt);
	memcpy(ptrByte, data, cnt);

	for (index=0; index <cnt; index ++)
	{
 		*(data+index) = *(ptrByte+(cnt-1)-index);
	}
 
	free(ptrByte);
}

////////////
void EndianSwapByte(DWORD *data, int cnt)
{
	int index=0;
	DWORD temp=0;
 
	for (index=0; index <cnt; index ++)
	{
		temp = *(data+index) ;
		temp =	  ((temp &0xFF000000)>>24) 
				+ ((temp &0x00FF0000)>>8)
				+ ((temp &0x0000FF00)<<8)  
				+ ((temp &0x000000FF)<<24); 
		*(data+index) = temp;
	}
}

void EndianSwapWord(DWORD *data, int cnt)
{
	int index=0;
 	DWORD temp=0;
 
	for (index=0; index <cnt; index ++)
	{
 		temp = *(data+index) ;
 		temp =	 ((temp &0xFFFF0000)>>16) + ((temp &0x0000FFFF)<<16); 
 		*(data+index) = temp;
	}
 
}
 
void EndianSwapBytesWord(DWORD *data, int cnt)
{
	int index=0;
 	DWORD temp=0;
 
	for (index=0; index <cnt; index ++)
	{
 		temp = *(data+index) ;
 		temp =	  ((temp &0xFFFF0000)>>16)   
				+ ((temp &0x0000FF00)<<8)  
				+ ((temp &0x000000FF)<<24); 
 		*(data+index) = temp;
	}
}

void EndianSwapWordBytes(DWORD *data, int cnt)
{
	int index=0;
 	DWORD temp=0;
 
	for (index=0; index <cnt; index ++)
	{
 		temp = *(data+index) ;
 		temp =	  ((temp &0xFF000000)>>24) 
				+ ((temp &0x00FF0000)>>8)
				+ ((temp &0x0000FFFF)<<16); 
 		*(data+index) = temp;
	}
}

#define MAX_LENGTH	256
BOOL IsFileExist(char *FileName)
{
	FILE * hFile=NULL;
	BOOL status = false;

#ifndef _LINUX_
	if(FileName[1] !=':')
	{
 		char line[MAX_LENGTH]="";
		if(GetCurrentDirectory( _MAX_PATH, line) != NULL)  
		{
			strcat(line, "\\"); 
			strcat(line, FileName); 
			strcpy(FileName, line);
		}
	}
#endif	//#ifndef _LINUX_

	hFile = fopen(FileName, "r");
	if (NULL != hFile)
	{
		status = true;
	 	fclose(hFile);
	}

	return status;
}

void SortChar(int NumOfData, char IndexArray[], 
			  char Data1[], char Data2[], char Data3[], char Data4[],
			  char Data5[], char Data6[], char Data7[], char Data8[])
{
	int indexSmall=0, indexData=0;
	char tempChar=0; 

	if(NumOfData <=1) return ;

	for (indexData=0; indexData < NumOfData; indexData++)
	{
		for (indexSmall=0; indexSmall < NumOfData; indexSmall++)
		{
			if(IndexArray[indexData] < IndexArray[indexSmall])
			{
				// swape data and index
				tempChar = IndexArray[indexData];
				IndexArray[indexData] = IndexArray[indexSmall];
				IndexArray[indexSmall] = tempChar;

				if(Data1)
				{
					tempChar = Data1[indexData];
					Data1[indexData] = Data1[indexSmall];
					Data1[indexSmall] = tempChar;
				}
				if(Data2)
				{
					tempChar = Data2[indexData];
					Data2[indexData] = Data2[indexSmall];
					Data2[indexSmall] = tempChar;
				}
				if(Data3)
				{
					tempChar = Data3[indexData];
					Data3[indexData] = Data3[indexSmall];
					Data3[indexSmall] = tempChar;
				}
				if(Data4)
				{
					tempChar = Data4[indexData];
					Data4[indexData] = Data4[indexSmall];
					Data4[indexSmall] = tempChar;
				}
				if(Data5)
				{
					tempChar = Data5[indexData];
					Data5[indexData] = Data5[indexSmall];
					Data5[indexSmall] = tempChar;
				}
				if(Data6)
				{
					tempChar = Data6[indexData];
					Data6[indexData] = Data6[indexSmall];
					Data6[indexSmall] = tempChar;
				}
				if(Data7)
				{
					tempChar = Data7[indexData];
					Data7[indexData] = Data7[indexSmall];
					Data7[indexSmall] = tempChar;
				}
				if(Data8)
				{
					tempChar = Data8[indexData];
					Data8[indexData] = Data8[indexSmall];
					Data8[indexSmall] = tempChar;
				}
			}
		}
	}

}

void EndianSwapeByteArray(BYTE *data, int cnt)
{
	int index=0;
	BYTE *ptrByte=NULL;
	DWORD temp=0;
	 
	ptrByte = (BYTE*)malloc(cnt);
	memcpy(ptrByte, data, cnt);

	for (index=0; index <cnt; index ++)
	{
 		*(data+index) = *(ptrByte+(cnt-1)-index);
	}
 
	free(ptrByte);
}

void EndianSwapeByte(DWORD *data, int cnt)
{
	int index=0;
	DWORD temp=0;
 
	for (index=0; index <cnt; index ++)
	{
		temp = *(data+index) ;
		temp =	  ((temp &0xFF000000)>>24) 
				+ ((temp &0x00FF0000)>>8)
				+ ((temp &0x0000FF00)<<8)  
				+ ((temp &0x000000FF)<<24); 
		*(data+index) = temp;
	}
}

int String2CharArray(char* string, char *deliminators, int NumOfData, char *data)
{
	int cnt=0;
	char* ptr=NULL;

	ptr = strtok(string, deliminators);
	while (cnt<NumOfData && NULL != ptr) 
	{	
 		data[cnt] = atoi(ptr); 
		cnt++;
 		ptr = strtok(NULL, deliminators);

	}

	return cnt;
}

int String2IntArray(char* string, char *deliminators, int NumOfData, int *data)
{
	int cnt=0;
	char* ptr=NULL;

	ptr = strtok(string, deliminators);
	while (cnt<NumOfData && NULL != ptr) 
	{	
		data[cnt] = atoi(ptr); 
		cnt++;
		ptr = strtok(NULL, deliminators);
	}
	
	return cnt;
}

int String2ByteArray(char* string, char *deliminators, int NumOfData, BYTE *data)
{
	int cnt=0;
	char* ptr=NULL;

	ptr = strtok(string, deliminators);
	while (cnt<NumOfData && NULL != ptr) 
	{	
 		data[cnt] = atoi(ptr); 
		cnt++;
 		ptr = strtok(NULL, deliminators);
	}

	return cnt;
}

int CharArray2String(char* string, char *deliminators, int NumOfData, char *data)
{
	int index=0;

	sprintf(string, " ");

	for(index=0; index<NumOfData; index++)
	{	
		sprintf(string, "%s%d%s",string, data[index], deliminators);
	}

	return 0;
}

int IntArray2String(char* string, char *deliminators, int NumOfData, int *data)
{
	int index=0;
	
	sprintf(string, " ");

	for(index=0; index<NumOfData; index++)
	{	
		sprintf(string, "%s%d%s",string, data[index], deliminators);
	}

	return 0;
}

int ByteArray2String(char* string, char *deliminators, int NumOfData, BYTE *data)
{
	int index=0;

	sprintf(string, " ");

	for(index=0; index<NumOfData; index++)
	{	
		sprintf(string, "%s%d%s",string, data[index], deliminators);
	}

	return 0;
}

void SortIntChar(int NumOfData, int IndexArray[], 
				 char Data1[], char Data2[], char Data3[], char Data4[], 
				 char Data5[], char Data6[], char Data7[], char Data8[], 
				 char Data9[], char Data10[])
{
	int indexSmall=0, indexData=0, tempInt=0;
	char tempChar=0; 

	for (indexData=0; indexData < NumOfData; indexData++)
	{
		for (indexSmall=0; indexSmall < NumOfData; indexSmall++)
		{
			if(IndexArray[indexData] < IndexArray[indexSmall])
			{
				// swape data and index
				tempInt = IndexArray[indexData];
				IndexArray[indexData] = IndexArray[indexSmall];
				IndexArray[indexSmall] = tempInt;

				if(Data1)
				{
					tempChar = Data1[indexData];
					Data1[indexData] = Data1[indexSmall];
					Data1[indexSmall] = tempChar;
				}
				if(Data2)
				{
					tempChar = Data2[indexData];
					Data2[indexData] = Data2[indexSmall];
					Data2[indexSmall] = tempChar;
				}
				if(Data3)
				{
					tempChar = Data3[indexData];
					Data3[indexData] = Data3[indexSmall];
					Data3[indexSmall] = tempChar;
				}
				if(Data4)
				{
					tempChar = Data4[indexData];
					Data4[indexData] = Data4[indexSmall];
					Data4[indexSmall] = tempChar;
				}
				if(Data5)
				{
					tempChar = Data5[indexData];
					Data5[indexData] = Data5[indexSmall];
					Data5[indexSmall] = tempChar;
				}
				if(Data6)
				{
					tempChar = Data6[indexData];
					Data6[indexData] = Data6[indexSmall];
					Data6[indexSmall] = tempChar;
				}
				if(Data7)
				{
					tempChar = Data7[indexData];
					Data7[indexData] = Data7[indexSmall];
					Data7[indexSmall] = tempChar;
				}
				if(Data8)
				{
					tempChar = Data8[indexData];
					Data8[indexData] = Data8[indexSmall];
					Data8[indexSmall] = tempChar;
				}
				if(Data9)
				{
					tempChar = Data9[indexData];
					Data9[indexData] = Data9[indexSmall];
					Data9[indexSmall] = tempChar;
				}
				if(Data10)
				{
					tempChar = Data10[indexData];
					Data10[indexData] = Data10[indexSmall];
					Data10[indexSmall] = tempChar;
				}
			}
		}
	}
}


/************************************************************************
 * SortDoubleChar()
 ************************************************************************/
void
SortDoubleChar(int NumOfData, double IndexArray[],
               char Data1[], char Data2[], char Data3[], char Data4[],
               char Data5[], char Data6[], char Data7[], char Data8[],
               char Data9[], char Data10[]) 
{
    int         indexSmall = 0, indexData = 0;
    double      tempDouble = 0;
    char        tempChar = 0;

    for (indexData = 0; indexData < NumOfData; indexData++) 
	{
        for (indexSmall = 0; indexSmall < NumOfData; indexSmall++) 
		{
            if(IndexArray[indexData] < IndexArray[indexSmall]) 
			{
                // swape data and index
                tempDouble = IndexArray[indexData];
                IndexArray[indexData] = IndexArray[indexSmall];
                IndexArray[indexSmall] = tempDouble;

                if(Data1) 
				{
                    tempChar = Data1[indexData];
                    Data1[indexData] = Data1[indexSmall];
                    Data1[indexSmall] = tempChar;
                }
                if(Data2) 
				{
                    tempChar = Data2[indexData];
                    Data2[indexData] = Data2[indexSmall];
                    Data2[indexSmall] = tempChar;
                }
                if(Data3) 
				{
                    tempChar = Data3[indexData];
                    Data3[indexData] = Data3[indexSmall];
                    Data3[indexSmall] = tempChar;
                }
                if(Data4) 
				{
                    tempChar = Data4[indexData];
                    Data4[indexData] = Data4[indexSmall];
                    Data4[indexSmall] = tempChar;
                }
                if(Data5) 
				{
                    tempChar = Data5[indexData];
                    Data5[indexData] = Data5[indexSmall];
                    Data5[indexSmall] = tempChar;
                }
                if(Data6) 
				{
                    tempChar = Data6[indexData];
                    Data6[indexData] = Data6[indexSmall];
                    Data6[indexSmall] = tempChar;
                }
                if(Data7) 
				{
                    tempChar = Data7[indexData];
                    Data7[indexData] = Data7[indexSmall];
                    Data7[indexSmall] = tempChar;
                }
                if(Data8) 
				{
                    tempChar = Data8[indexData];
                    Data8[indexData] = Data8[indexSmall];
                    Data8[indexSmall] = tempChar;
                }
                if(Data9) 
				{
                    tempChar = Data9[indexData];
                    Data9[indexData] = Data9[indexSmall];
                    Data9[indexSmall] = tempChar;
                }
                if(Data10) 
				{
                    tempChar = Data10[indexData];
                    Data10[indexData] = Data10[indexSmall];
                    Data10[indexSmall] = tempChar;
                }
            }
        }
    }
}

int ctoi(char inVal)
{
	switch (inVal)
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return (inVal-'0');
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
		return (inVal-'a')+10;
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
		return (inVal-'A')+10;
	default:
		break;
	}
		return 0;
}
 

int BlockReadFromBin(BYTE *buffer, int blockSize, FILE *srcFile)
{
    return fread(buffer, 1, blockSize, srcFile);
}

int BlockReadFromTextFile(BYTE *buffer, int blockSize, FILE *srcFile)
{
	//expected format for the file is as following, one byte per line, each line ended with 0x13 and 0x10
	// 00
	// 11
	// aa
	// ...
	int i=0;
	int oneChar=0;
	char line[400]="";

	while (!feof(srcFile) && i<blockSize)
	{
		// get one line
		fgets(line, 256, srcFile);

        if (isxdigit(line[0]))
        {
			sscanf(line, "%x", &oneChar);
			buffer[i] = (BYTE)oneChar;
		    i++;
        }
		else
			DebugLogRite("%s\n",line);
	}

	return i;
}


int maximum(int Num, int *Array)
{	
	int result=0, i=0;

	if (0 == Num) return result;
	result = Array[0];
	for (i=1; i<Num; i++)
	{	if(result <Array[i])
			result = Array[i];
	}
	
	return result;
}

int minimum(int Num, int *Array)
{	
	int result=0, i=0;
	
	if (0 == Num) return result;
	result = Array[0];
	for (i=1; i<Num; i++)
	{	if(result > Array[i])
			result = Array[i];
	}

	return result;
}


#ifndef _LINUX_
 __int64 aHextoI64(const char *s) 
{ 
   const __int64 max64 = (__int64)0-1; 
   __int64 v = 0; 
   int digit=0;
   
   while ((*s>=TEXT('0') && *s<=TEXT('9')) || (*s>=TEXT('a') && *s<=TEXT('f')) || (*s>=TEXT('A') && *s<=TEXT('F')))
   { 
	if (*s>=TEXT('a') && *s<=TEXT('f'))  
		digit = *s++ - TEXT('a')+10; 
	else if(*s>=TEXT('A') && *s<=TEXT('F'))
		digit = *s++ - TEXT('A')+10; 
	else
		digit = *s++ - TEXT('0'); 
    
//	if (v > (max64-digit)/16) 
//	{
//	DebugLogRite("overflow __int64 %012I64X, %X %012I64X  %f %012I64X \n", 
//						v, digit, (max64-digit),  (max64-digit)/16.0, 
//						(__int64)((max64-digit)/16.0) );
//		return 0; /* overflow */ 
//	}
     v = 16*v + digit; 
   } 
   return v; 
}


__int64 aDectoI64(const char *s) 
{ 
   const   __int64 max64 = (  __int64)0-1; 
   __int64 v = 0; 
   int digit=0;
   
   while (*s>=TEXT('0') && *s<=TEXT('9'))
   { 
 		digit = *s++ - TEXT('0'); 
    
//	if (v > (max64-digit)/10) 
//	{
//	DebugLogRite("overflow __int64 %012I64X, %X %012I64X  %f %012I64X \n", 
//						v, digit, (max64-digit),  (max64-digit)/10.0, 
//						(  __int64)((max64-digit)/10.0) );
//		return 0; /* overflow */ 
//	}
     v = 10*v + digit; 
   } 
   return v; 
}

__int64 atoi64(char *s) 
{
 
	if(strstr(s, "0X")||strstr(s, "0x"))
		return aHextoI64(s+2);
	if(strchr(s, 'H') || strchr(s, 'h'))
		return aHextoI64(s);
	else
		return aDectoI64(s);
}
#endif	//#ifndef _LINUX_

