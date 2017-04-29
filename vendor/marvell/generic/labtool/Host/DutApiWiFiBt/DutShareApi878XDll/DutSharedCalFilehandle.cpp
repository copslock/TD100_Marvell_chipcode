/** @file DutSharedCalFilehandle.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include <stdio.h>
#include "../../DutCommon/Dut_error.hc"
#include "../../DutCommon/Clss_os.h"
#include "../../DutCommon/utilities_os.h"
#include "../../DutCommon/utilities.h"

 /*
#ifndef _LINUX_
	#include "stdafx.h"
	#include <stdlib.h>
	#include <time.h>
	#include <direct.h>
	#include "../DutWlanApi.hc"
#define LN_LINE_END		(2)
int txtPtrCnt(int a) {return  (2*3 -1+LN_LINE_END) + (4*3 -1+LN_LINE_END) + a*3 + 2*(a/(sizeof(DWORD)*4)+1)-1;}
#else	//_LINUX_
	#include "DutWlanApi.hc"
	#include <malloc.h>
	#include <string.h>
	#define TRUE 1
	#define FALSE 0
#define LN_LINE_END		(1)
int txtPtrCnt(int a) {return  (2*3 -1+LN_LINE_END) + (4*3 -1+LN_LINE_END) + a*3 + (a/(sizeof(DWORD)*4));}

	typedef unsigned long DWORD;
	typedef unsigned short WORD;
	typedef unsigned int BOOL;
	typedef unsigned char BYTE;
	extern int DebugLogRite(const char*,...);
	extern int WritePrivateProfileString(char*, char*, char*, char*);
	extern int GetPrivateProfileSection(char*, char*, int, char*);
	extern int GetPrivateProfileSectionNames(char*, int, char*);
	extern int GetPrivateProfileInt(char*, char*, int, char*);
	extern int debugLog;
#endif	//#ifndef _LINUX_
 */
 
 //*** SPI signature and so on shall be shared 
#include DUTCLASS_WLAN_HD_PATH	//	"../DutWlanApi878XDll/DutWlanApiClss.hd"

#if defined(_WLAN_)
#include DUTCLASS_WLAN_H_PATH	//	"../DutWlanApi878XDll/DutWlanApiClss.h" 
#endif	// #if defined(_WLAN_)

#if defined(_BT_) 
#include DUTCLASS_BT_H_PATH		//"..D/utBtApi878XDll/DutBtApiClss.h"   
#endif	//#if defined(_BT_)
#include DUTCLASS_SHARED_H_PATH	// "DutSharedClss.h"
 

int DutSharedClssHeader GetWriteTemplateFlag(void)
{
	return g_writeTemplate;
}

void DutSharedClssHeader SetWriteTemplateFlag(int Flag)
{	
	g_writeTemplate = Flag;
	return; 
}

int DutSharedClssHeader InitCalDataFile(char *FileName)
{
	int status=0, i=0, cnt=0;
//	char FileName[_MAX_PATH]="BtCal.txt";
	char line[256]="";
	DWORD	tempDw=0;
	FILE *hFile=0;

	hFile = fopen(FileName,"w");
	fclose(hFile); 
	status = remove(FileName);
#ifndef _LINUX_
	if(ERROR_FILE_NOT_FOUND == errno) 
	{ 
		perror(""); 
	}

	if(ERROR_ACCESS_DENIED == errno) 
	{ 
		perror(""); 
		return ERROR_FILEACCESS;
	}
#else // #ifndef _LINUX_
	if (status != 0)
		printf("errno %d\n", errno);

#endif // #ifndef _LINUX_


	hFile = fopen(FileName,"w"); 
 
 	if(NULL== hFile) return ERROR_ERROR;

	sprintf(line, "01 00\n%02X 00 00 00\n", STRUCTUREREV_D);
	fprintf(hFile, "%s", line);
 
	fclose(hFile); 

	return status;
}


int DutSharedClssHeader UpDateLenCalDataFile(char *FileName, int AddLength)
{
	int status=0, i=0, cnt=0;
//	char FileName[_MAX_PATH]="BtCal.txt";
	char line[256]="";
	BYTE	tempB=0;
	FILE *hFile=0;  
	int NewLength=0; 

 	if(0 == g_NoEeprom) 
		return ERROR_NONE;

	hFile = fopen(FileName,"r+"); 
	
	if(NULL== hFile) return ERROR_ERROR;

 	cnt = 2*3 -1 +LN_LINE_END +2*3;
 
	fseek(hFile, cnt, SEEK_SET);
	i = fscanf(hFile, "%s", line); 
	sscanf(line, "%x", &tempB);  
//	NewLength = atoi(line);
	NewLength = tempB;
	NewLength<<=8;
	i = fscanf(hFile, "%s", line); 
	sscanf(line, "%x", &tempB);  
//	NewLength = atoi(line);
	NewLength |= tempB;
//	NewLength |= (BYTE)atoi(line);

	NewLength += AddLength;

	fseek(hFile, cnt, SEEK_SET);
	sprintf(line, "%02X %02X\n", 
		(BYTE)(NewLength), 
		(BYTE)(NewLength>>8));
	fprintf(hFile, "%s", line);
 
	fclose(hFile); 

	return status;
}


int DutSharedClssHeader ReadCalDataFile(char *FileName, DWORD StartAddress, 
								   DWORD LenInDw, 
									DWORD *pDataArray)
{
	int status=0, cnt=0;
 	char line[256]="";
	DWORD	tempDw=0, i=0;
	FILE *hFile=0; 
	long ptrB=0, ptrE=0;
 
	hFile = fopen(FileName,"r"); 
 
	if(NULL  == hFile) 
	{
		memset((void*)pDataArray, 0, LenInDw*sizeof(DWORD));
		return ERROR_NONE;
	}
//	if(0 == g_NoEeprom)
//		StartAddress -=(VMR_HEADER_LENGTH);
	// put pointer to a location
 
	
 	fseek(hFile, 0, SEEK_SET);
	ptrB = ftell(hFile);
 	fseek(hFile, 0, SEEK_END);
	ptrE = ftell(hFile);

	// put pointer to a location
	cnt = txtPtrCnt(StartAddress);
	if(cnt > (ptrE-ptrB)) 
	{
		memset((void*)pDataArray, 0, LenInDw*sizeof(DWORD));
		return ERROR_NONE;
	}
 	else
		fseek(hFile, cnt, SEEK_SET);


	for (i=0; i<LenInDw; i++)
	{
		tempDw =0;
		for (cnt = 0; cnt<4; cnt++)
		{
			DWORD tempB=0;
			int IsEof=0;

			tempDw<<=8;
			strcpy(line, "");
			IsEof = fscanf(hFile, "%s", line);
			if(EOF == IsEof) break; 
			
			sscanf(line, "%x", &tempB);
			tempDw |=(0xFF&tempB);						
		}
		(*(pDataArray+i)) =tempDw;
			 
	}
	fclose(hFile); 


	return status;
}

int DutSharedClssHeader WriteCalDataFile(char *FileName, 
										 DWORD StartAddress, 
								   DWORD LenInDw, 
									DWORD *pDataArray)
{

#define NOEEPROMFILE_LINELEGTH	(4) // DWORD
	int status=0, lineLen=0, cnt=0;
 	char line[256]="";
	DWORD	i=0, tempDw=0;
	long ptrB=0, ptrE=0;
	FILE *hFile=0; 
	bool newFile=0;
 
 
	hFile = fopen(FileName,"r+"); 
 
 	if(NULL  == hFile) 
	{
		newFile=true;

		InitCalDataFile(FileName);
		hFile = fopen(FileName,"r+"); 
	}
 	if(NULL  == hFile) return ERROR_FILEACCESS;
//	if(0 == g_NoEeprom)
//		StartAddress -=(VMR_HEADER_LENGTH);

 	fseek(hFile, 0, SEEK_SET);
	ptrB = ftell(hFile);
 	fseek(hFile, 0, SEEK_END);
	ptrE = ftell(hFile);

	// put pointer to a location
	cnt = txtPtrCnt(StartAddress);
// 	cnt = (2*3 -1+LN_LINE_END) + (4*3 -1+LN_LINE_END) + StartAddress*3 + 2*(StartAddress/(sizeof(DWORD)*4)+1)-1;
// 	cnt = (2*3 -1+LN_LINE_END) + (4*3 -1+LN_LINE_END) + StartAddress*3 + 2*(StartAddress/(sizeof(DWORD)*4)+1)-1;
	if(cnt > (ptrE-ptrB)) 
		fseek(hFile, 0, SEEK_END);
	else
		fseek(hFile, cnt, SEEK_SET);

	for (i=0; i<LenInDw; i+=lineLen) //NOEEPROMFILE_LINELEGTH)
	{
		lineLen = (LenInDw-i)>NOEEPROMFILE_LINELEGTH? NOEEPROMFILE_LINELEGTH:(LenInDw-i);
		if(0 != (i*4+StartAddress)%16) 
			lineLen=(16-(i*4+StartAddress)%16)/4;
 
		for (cnt = 0; cnt<lineLen; cnt++)
		{
			tempDw = (*(pDataArray+i+cnt));
			sprintf(line, " %02X %02X %02X %02X", 
				(0xFF&(tempDw>>24)), (0xFF&(tempDw>>16)), 
				(0xFF&(tempDw>>8)),  (0xFF&(tempDw>>0)));

			fprintf(hFile, "%s", line);
		}
			fprintf(hFile, "\n");
	}
//	if(i>LenInDw) 
//		fprintf(hFile, "\n");

	fclose(hFile); 


//	if(!newFile) 
		status = UpDateLenCalDataFile(FileName, LenInDw*sizeof(DWORD));
	return status;
}

