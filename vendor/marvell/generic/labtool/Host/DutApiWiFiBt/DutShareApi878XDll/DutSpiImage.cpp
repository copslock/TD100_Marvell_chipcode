/** @file DutSpiImage.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

//#include <stdio.h>
#include "../../DutCommon/Dut_error.hc"
#include "../../DutCommon/Clss_os.h"
#include "../../DutCommon/utilities_os.h"
#include "../../DutCommon/mathUtility.h"
#include "../../DutCommon/utilities.h"

#if defined(_WLAN_)
#include DUTCLASS_WLAN_H_PATH	//"../DutWlanApi878XDll/DutWlanApiClss.h"
#endif	// #if defined(_WLAN_)
#include DUTCLASS_WLAN_HD_PATH	//"../DutWlanApi878XDll/DutWlanApiClss.hd"

#if defined(_BT_)
#include DUTCLASS_BT_H_PATH		//"../DutBtApi878XDll/DutBtApiClss.h"   
#endif	//#if defined(_BT_)

static char line[255];

#ifdef _PCI_
	static char	FileName[255]="CB82.hex";
#endif

#ifdef _CF_
	static char	FileName[255]="cf8385h.hex";
#endif
 
#ifdef _BT_USB_ 
	static char	FileName[255]="8780prestage.bin";
#endif

#ifdef _NDIS_ 
	static char	FileName[255]="CB8XXX.hex";
#endif

#if defined (_IF_SDIO_) || defined (_IF_UDP_) || defined  (_IF_UART_)
#if defined(_W8782_)
	static char	FileName_Usb[255]="w8782_2040_default.bin";
	static char	FileName_Sdio[255]="w8782_2040_default.bin";
	char	FileName[255]=""; 
#else
	static char	FileName[255]="/etc/labtool/eesd8787.sbin";
#endif
#endif


int FileLoad(char *pFileName, unsigned long *pFileContent, int SizeInDword)
{
	FILE	*hFile = NULL;
	int	i = 0, index = 0;
	char	tempchar = 0;
	struct stat	sbuf={0};

	if(0 == SizeInDword) return 0;

	i = stat(pFileName, &sbuf);

#ifdef ANDROID
        DebugLogRite("// Loading from File:\t%s \n", pFileName);
#else
        DebugLogRite("// Loading from File:\t%s \n"
                "// Created on:\t\t%s\n// Last Modified on:\t%s\n//Last Access on:\t%s\n\n",
                pFileName, ctime(&sbuf.st_ctime), ctime(&sbuf.st_mtime), ctime(&sbuf.st_atime));
#endif

	hFile = fopen(FileName, "rb"); // check this  
	if (NULL == hFile)
	{	
		
		return 1;
	}

	i = 0;
	do 
	{
		index = 0;
		pFileContent[i] =0;
		do 
		{
			tempchar= fgetc(hFile);
			pFileContent[i] |= (0xFF & tempchar);
			index++;
			if(index <4) pFileContent[i] <<=8;
		} while (index <4); 
		i++;
	}while (i <SizeInDword);
 
	fclose(hFile);

	return 0;
}

int DutSharedClssHeader CompareE2PData( int deviceType, void *pObj, 
										DWORD  *RefArray, int SizeInByte)
{
	DWORD startingAddress=0, Length=0;
    static	DWORD *AddArray=NULL, *DataArray=NULL;
	int Index=0, errorCount=0, status=0; 
	
	Length = SizeInByte/sizeof(DWORD);
 	AddArray = 	(DWORD*)malloc(SizeInByte); 
	DataArray = (DWORD*)malloc(SizeInByte); 
			
	status = DumpE2PData(deviceType, pObj, 
						0, 
						Length,
						AddArray, 
						DataArray);

	if(status) { goto ERROR_HANDLE;} 

	for (Index =0; Index < (signed long)Length; Index ++)
		if(DataArray[Index] != RefArray[Index])
		{
			errorCount++;
			DebugLogRite("%08X\t:\t%08X\t:\t%08X\n", 
					AddArray[Index], 
					DataArray[Index], 
					RefArray[Index]
 					); 
		}
	//	else
	//	{
	//		DebugLogRite("."); 
	//	}
	DebugLogRite("Total Error Count %d\n", errorCount);

ERROR_HANDLE:
	free(AddArray);
	free(DataArray);

    return status;  
} 


#ifdef _LINUX_	
int DutSharedClssHeader FlexSpiDL(int deviceType, void *pObj, bool erase, char *fileName)
{
	int         status=0;
	off_t		m_size=0;
   	static int  HexSize=0 ; // in  DWORD
    char        line[200]=""; 
	static	    DWORD *SpiFlex=NULL; //unsigned long *SpiFlex=NULL;
 
	if(strlen(fileName) > 0) 
		strcpy(FileName, fileName);
	else
	{
#if defined(_W8782_)
	    DebugLogRite("^^ for Usb8782: hex file name shall be like [%s]: ", FileName_Usb);
	    DebugLogRite("^^ for Sd8782: hex file name shall be like [%s]: ", FileName_Sdio);
		DebugLogRite("^^ Please enter hex file name [%s]: ", FileName);
#else	// #if defined(_W8782_)
		DebugLogRite("^^ Please enter New hex file name [%s]: ", FileName);
#endif	// #if defined(_W8782_)
	    fgets(line, sizeof(line), stdin);
	    sscanf(line, "%s", FileName);
	}

	if (access(FileName, F_OK)) 
	{
       	DebugLogRite("File [%s] doesn't exist\n", FileName);
       	return 1;
   	}
	else
   		DebugLogRite("File [%s] accessable\n", FileName);

	m_size = filelength(FileName);
	if (m_size<=0)
	{
       	DebugLogRite("File [%s] doesn't exist\n", FileName);
   		return ERROR_FILENOTEXIST;
	}
 
	SpiFlex = (DWORD *)malloc(m_size);
	memset(SpiFlex, m_size, 0);
 	HexSize = m_size / sizeof(DWORD); 
 	
 	if(!FileLoad(FileName, SpiFlex, HexSize))
	{	
		status = WriteLumpedData(deviceType, pObj, 0, HexSize, SpiFlex);  
	}
	else
	{
        DebugLogRite("File Loading failed\n");
	}

	free(SpiFlex);
 
	return status;
}
 
#else // #ifdef _LINUX_	 
int DutSharedClssHeader FlexSpiDL(int deviceType, void *pObj, bool erase, char *fileName)  
{
#ifdef _FLASH_

#ifdef _W8780_
#define FLASH_SECTIONSIZE	(0x400)
#else
#define FLASH_SECTIONSIZE	(0x400)
#endif //#ifdef _W8780_

#endif //#ifdef _FLASH_

	int status=0;
 	off_t		m_size;
	unsigned long *SpiFlex=NULL;
    int HexSize=0 ; // in  DWORD
    char line[200]="";
	int sectIndex=0, sectCnt=0;
        
    if(!strcmp(fileName, ""))
    {
        fileName = FileName;
        DebugLogRite("^^ Please enter New hex file name [%s]: ", fileName);
        fgets(line, sizeof(line), stdin);
        sscanf(line, "%s", fileName);
    }
    
    m_size = filelength(fileName);
	if (m_size<=0)
	{
        	DebugLogRite("File doesn't exist\n");
       		return ERROR_FILENOTEXIST;
	}

    	HexSize = m_size / sizeof(DWORD);

    	SpiFlex = (unsigned long *)malloc(m_size);
 	memset(SpiFlex, m_size, 0);
   	if(!FileLoad(fileName, SpiFlex, HexSize))
	{
#ifdef _FLASH_
//		erase all needed section:
		sectCnt = ceil(1.0*HexSize/FLASH_SECTIONSIZE);
        for (sectIndex=0; sectIndex<sectCnt; sectIndex++)
		{
#ifndef _LINUX_
			switch(deviceType)
			{ 
				case DEVICETYPE_WLAN:
					status = ((class DutWlanApiClss*)pObj)->Wifi_EraseEEPROM(sectIndex); 
 					break;
				case DEVICETYPE_BT: 
					status = ((class DutBtApiClss*)pObj)->Bt_EraseEEPROM(sectIndex); 
 					break;
				default:
					status= ERROR_UNSUPPORTEDDEVICETYPE;
					break;
 			}
#else // #ifndef _LINUX_
			switch(deviceType)
			{ 
				case DEVICETYPE_WLAN:
					status = EraseEEPROM(sectIndex); 
 					break;
				case DEVICETYPE_BT: 
					status = EraseEEPROM(sectIndex); 
 					break;
				default:
					status= ERROR_UNSUPPORTEDDEVICETYPE;
					break;
				status = WriteLumpedData(deviceType, pObj, 0, HexSize, SpiFlex);
				if(status) return status;
			}
#endif // #ifndef _LINUX_
		}
#endif // #ifdef _FLASH_
		status = WriteLumpedData(deviceType, pObj, 0, HexSize, SpiFlex);
		if(status) return status;
	}
    free(SpiFlex);

	return status;
}
#endif //#ifdef _LINUX_	


int DutSharedClssHeader FlexSpiCompare(int deviceType, void*pObj, char *fileName)  
{
	static int HexSize=0 ; // in  DWORD
 	static	off_t		m_size=0;
	int status=0;
	static	DWORD *SpiFlex=NULL; //unsigned long *SpiFlex=NULL;
    
	if(strlen(fileName) > 0) strcpy(FileName, fileName);
	else
	{
	    DebugLogRite("^^ Please enter New hex file name [%s]: ", FileName);
	    fgets(line, sizeof(line), stdin);
	    sscanf(line, "%s", FileName);
	}
/*
	if (access(FileName, F_OK)) 
	{
        	DebugLogRite("File [%s] doesn't exist\n", FileName);
        	return 1;
    	}
   	else
       		DebugLogRite("File [%s] accessable\n", FileName);
*/

 	m_size = filelength(FileName);
	if (m_size<=0)
	{
        	DebugLogRite("File [%s] doesn't exist\n", FileName);
       		return ERROR_FILENOTEXIST;
	}
 

	SpiFlex = (DWORD *)malloc(m_size);
 	memset(SpiFlex, m_size, 0);
	HexSize = m_size / sizeof(DWORD);
 
	if(!FileLoad(FileName, SpiFlex,  HexSize))  
	{ 
 		status = CompareE2PData(deviceType, pObj, 
						SpiFlex, HexSize*sizeof(DWORD) );
	}
	else
	{
		DebugLogRite("File Loading failed\n"); 
	}

	free(SpiFlex);  
	 
	return status;
}

