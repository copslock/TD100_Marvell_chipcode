/** @file DutShared_spiAccess.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include <stdio.h>
#include "../../DutCommon/Dut_error.hc"
#include "../../DutCommon/Clss_os.h"
#include "../../DutCommon/utilities_os.h"
#include "../../DutCommon/utilities.h"

#define SUPPORT_ANNEX41

/*
#ifndef _LINUX_
	#include "stdafx.h"
	#include <stdlib.h>
	#include <time.h>
	#include <direct.h>
	#include "../DutWlanApi.hc"
#else	//_LINUX_
	#include "DutWlanApi.hc"
	#include <malloc.h>
	#include <string.h>
	#define TRUE 1
	#define FALSE 0

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

#include DUTCLASS_SHARED_H_PATH	//"DutSharedClss.h"

#if defined(_WLAN_)
#include DUTCLASS_WLAN_H_PATH	//"../DutWlanApi878XDll/DutWlanApiClss.h"
#endif	//#if defined(_WLAN_)
#include DUTCLASS_WLAN_HD_PATH	//"../DutWlanApi878XDll/DutWlanApiClss.hd"

#if defined(_BT_)
#include DUTCLASS_BT_H_PATH		//"../DutBtApi878XDll/DutBtApiClss.h"   
#endif	//#if defined(_BT_)

#if defined(_UDP_) || defined (_IF_UART_)
#define MAXSPIPAGE_BYTE 0x100 
#else
#define MAXSPIPAGE_BYTE 0x80
#endif


int DutSharedClssHeader DumpE2PData(int deviceType, void *pObj, 
									DWORD StartAddress, 
									DWORD LenInDw,
									DWORD *pAddressArray, 
									DWORD *pDataArray)
{
	int status=0, index=0;
//	DWORD Address=0;
	int step=0;
 	int lenDw=0;
	bool slow=0; 
	
	if(g_NoEeprom) return ERROR_NOT_IMPLEMENTED;
	
	slow = (LenInDw > (MAXSPIPAGE_BYTE/sizeof(DWORD)));

	step =  LenInDw;
	
	for (index =0; index < step; index +=(MAXSPIPAGE_BYTE/sizeof(DWORD)) )
	{
		lenDw = (LenInDw - index);  
		
		if(lenDw > (MAXSPIPAGE_BYTE/sizeof(DWORD))) 
			lenDw =MAXSPIPAGE_BYTE/sizeof(DWORD);
 		
//		 DebugLogRite("deviceType %d: lenDw %d %d\n", 
//				deviceType, lenDw, MAXSPIPAGE_BYTE);

#ifndef _LINUX_
 		switch(deviceType)
		{ 
#if defined(_WLAN_)
 			case DEVICETYPE_WLAN:
				status = ((class DutWlanApiClss*)pObj)->Wifi_SpiReadData(StartAddress+index*sizeof(DWORD), 
						lenDw, 
						pDataArray+index);   
 				break;
#endif	//#if defined(_WLAN_)

#if defined(_BT_)
			case DEVICETYPE_BT: 
	 			status = ((class DutBtApiClss*)pObj)->Bt_SpiReadData(StartAddress+index*sizeof(DWORD), 
						lenDw, 
						pDataArray+index);   
 				break;
#endif	//#if defined(_BT_)
			default:
			status= ERROR_UNSUPPORTEDDEVICETYPE;
				break;
		}
#else // #ifndef _LINUX_
 		switch(deviceType)
		{ 
#if defined(_WLAN_)
 			case DEVICETYPE_WLAN:
				status = Wifi_SpiReadData(StartAddress+index*sizeof(DWORD), 
						lenDw, 
						pDataArray+index);   
 				break;
#endif	//#if defined(_WLAN_)

#if defined(_BT_)
			case DEVICETYPE_BT: 
	 			status = Bt_SpiReadData(StartAddress+index*sizeof(DWORD), 
						lenDw, 
						pDataArray+index);   
 			break;
#endif	//#if defined(_BT_)
		default:
			status= ERROR_UNSUPPORTEDDEVICETYPE;
			break;
		}
#endif // #ifndef _LINUX_

 		if(slow) DebugLogRite(".");
	}
 	if(slow) DebugLogRite("\n");

	if(pAddressArray)
		for (index =0; index < (signed long) LenInDw; index ++) 
		{
			pAddressArray[index] = StartAddress+index*4; 
		}
 

	return status;
}


int DutSharedClssHeader WriteLumpedData(int deviceType, void *pObj, 
										DWORD StartAddress, 
								   DWORD LenInDw, 
									DWORD *pDataArray)
{
	int status=0, index=0;
	DWORD Address=0;
 	int lenDw=0;
	bool slow=0; 

	if(g_NoEeprom) return ERROR_NOT_IMPLEMENTED;

	slow = (LenInDw > (MAXSPIPAGE_BYTE/sizeof(DWORD)));


	for (index =0; index < (signed long) LenInDw; index +=(MAXSPIPAGE_BYTE/sizeof(DWORD)) )
	{
		lenDw = (LenInDw - index); //*MAXSPIPAGE_BYTE;
		
		if(lenDw > (MAXSPIPAGE_BYTE/sizeof(DWORD))) 
			lenDw =(MAXSPIPAGE_BYTE/sizeof(DWORD));

#ifndef _LINUX_
 		switch(deviceType)
		{ 
#if defined(_WLAN_)
 			case DEVICETYPE_WLAN:
				status = ((class DutWlanApiClss*)pObj)->Wifi_SpiWriteData(StartAddress+index*sizeof(DWORD), 
						lenDw, 
						pDataArray+index,false);   
 				break;
#endif	//#if defined(_WLAN_)

#if defined(_BT_)
			case DEVICETYPE_BT: 
	 			status = ((class DutBtApiClss*)pObj)->Bt_SpiWriteData(StartAddress+index*sizeof(DWORD), 
						lenDw, 
						pDataArray+index,false);   
 				break;
#endif	//#if defined(_BT_)
			default:
				status= ERROR_UNSUPPORTEDDEVICETYPE;
				break;
		}
#else // #ifndef _LINUX_
 		switch(deviceType)
		{ 
#if defined(_WLAN_)
 			case DEVICETYPE_WLAN:
				status = Wifi_SpiWriteData(StartAddress+index*sizeof(DWORD), 
						lenDw, 
						pDataArray+index,false);   
 				break;
#endif	//#if defined(_WLAN_)

#if defined(_BT_)
			case DEVICETYPE_BT: 
	 			status = Bt_SpiWriteData(StartAddress+index*sizeof(DWORD), 
						lenDw, 
						pDataArray+index,false);   
 				break;
#endif	//#if defined(_BT_)
			default:
				status= ERROR_UNSUPPORTEDDEVICETYPE;
				break;
		}
#endif // #ifndef _LINUX_

 		if(slow) DebugLogRite(".");
	}
 	if(slow) DebugLogRite("\n");

	return status;
}

typedef struct  PkHeader
{
 	WORD		cmd;		// message type
	WORD		len;
	WORD		seq;
	WORD		result;
	DWORD       MfgCmd;
} PACK PkHeader;

typedef struct {
	PkHeader	header;
	DWORD		action;
	DWORD		Error; 
	DWORD signature;
	DWORD sector_erase;
    DWORD address;
	DWORD lenInByte;
	BYTE byte[0];
} PACK spi_CmdEeprom_t;

int DutSharedClssHeader Spi_GetHeaderTbl(int deviceType, void *pObj,  DWORD *HeaderTbl)
{
  	DWORD offset=0,  *AddArray=NULL;
  	int status=0;
	
	//		Menu
	//
	AddArray = (DWORD*) malloc((VMR_HEADER_LENGTH)); 
//	status = Spi_DumpData(0,  (VMR_HEADER_LENGTH)/sizeof(DWORD),  
	status = DumpE2PData(deviceType,  pObj, 0,  (VMR_HEADER_LENGTH)/sizeof(DWORD),  
					 AddArray, HeaderTbl);

#ifndef BIG_ENDIAN
	//	Header table is in big endian
	EndianSwapeByte(HeaderTbl, (VMR_HEADER_LENGTH)/sizeof(DWORD)); 
#endif //#ifndef BIG_ENDIAN

	free(AddArray); 

	return status; 
}

int DutSharedClssHeader Spi_SetHeaderTbl(int deviceType, void *pObj,  DWORD *HeaderTbl)
{
 
  	DWORD offset=0,  *AddArray=NULL;
 	int status;
	//		Menu
	//
	if(DeBugLog) 
		DebugLogRite("### Get Header Table###\n");

#ifndef BIG_ENDIAN
	//	Header table is in big endian
	EndianSwapeByte(HeaderTbl, (VMR_HEADER_LENGTH)/sizeof(DWORD)); 
#endif

// 	status = Spi_WriteLumpedData(deviceType, pObj, 0,  (VMR_HEADER_LENGTH)/sizeof(DWORD),  
//					 HeaderTbl);
 	status = WriteLumpedData(deviceType, pObj, 0,  (VMR_HEADER_LENGTH)/sizeof(DWORD),  
					 HeaderTbl);

	free(AddArray); 

	return status; 
}


int DutSharedClssHeader SpiGetHeaderTbl( int deviceType, void *pObj, 
										DWORD *HeaderTbl)
{
  	DWORD offset=0;
  	int status=0;
	
	//		Menu
	//
	status = DumpE2PData(deviceType, pObj, 
						0,  (VMR_HEADER_LENGTH)/sizeof(DWORD),  
					 NULL, HeaderTbl);

#ifndef BIG_ENDIAN
//	Header table is in big endian
	EndianSwapeByte(HeaderTbl, (VMR_HEADER_LENGTH)/sizeof(DWORD));  
#endif //#ifndef BIG_ENDIAN

	return status; 
}

int DutSharedClssHeader SpiSetHeaderTbl(int deviceType, void *pObj, 
										DWORD *HeaderTbl)
{
 
  	DWORD offset=0;//,  *AddArray=NULL;
 	int status;
	//		Menu
	//
//	DebugLogRite("\n\n");
	if(DeBugLog) 
		DebugLogRite("### Get Header Table###\n");

#ifndef BIG_ENDIAN
	//	Header table is in big endian
	EndianSwapeByte(HeaderTbl, (VMR_HEADER_LENGTH)/sizeof(DWORD)); 
#endif //#ifndef BIG_ENDIAN

 	status = WriteLumpedData(deviceType, pObj, 
						0,  (VMR_HEADER_LENGTH)/sizeof(DWORD),  
					 HeaderTbl);

//	free(AddArray); 

	return status; 
}
 

int DutSharedClssHeader SpiGetBoot2Version(int deviceType, void *pObj,  
										   DWORD *pBoot2Version)
{

#define u8 BYTE
#define u16 WORD
#define u32 DWORD

typedef struct _IMAGE_INFO {
    u8 fwType:3;
    u8 fwCompressed:1;
    u8 fwFormat:2;
    u8 extFormat:2;
} IMAGE_INFO_t;

typedef struct _DFU_PREFIX {
    u32          pfSignature;
    u16          pfLength;
    u8           pfVersion;
    IMAGE_INFO_t    imageInfo;
    u16          chipId0;
    u16          chipId1;
    u8           chipRevStart;
    u8           chipRevEnd;
    u16          reserved;
    u32          fwVersion;
    u32          fwOffset;
    u32          extSize;
    u32          imageSize;
    u32          pfCrc;
} DFU_PREFIX_t;

	DWORD status=0;
	int index=0, len=1;
	DWORD address=0x08,*addArray=NULL, *readback=0;
	DWORD boot2addr;
	addArray=(DWORD*)malloc(len*sizeof(DWORD)); 
	readback=(DWORD*)malloc(len*sizeof(DWORD)); 
	memset(addArray,0,len*sizeof(DWORD));
	memset(readback,0,len*sizeof(DWORD));
	status = DumpE2PData(deviceType, pObj, address, len, addArray, readback);
	boot2addr=(readback[0]-sizeof(DFU_PREFIX_t));
	free(addArray);
	free(readback);
	if(status) return status;



	//DFU_PREFIX_t m_prefix;
	len=(sizeof(DFU_PREFIX_t)/sizeof(DWORD));
	addArray=(DWORD*)malloc(len*sizeof(DWORD)); 
	readback=(DWORD*)malloc(len*sizeof(DWORD)); 
	status = DumpE2PData(deviceType, pObj, boot2addr, len, addArray, readback);
	if(status)
	{
		free(addArray);
		free(readback);
		return status;
	}
	//memcpy((void *)&m_prefix,readback,sizeof(m_prefix));
	//DebugLogRite("Boot pfSignature \t%04X\n",m_prefix.pfSignature);
	//DebugLogRite("BOOT2 Version \t0x%08X\n",m_prefix.fwVersion);
//	DebugLogRite("BOOT2 Version \t0x%08X\n",((DFU_PREFIX_t *)readback)->fwVersion);	
	if(pBoot2Version) (*pBoot2Version) = ((DFU_PREFIX_t *)readback)->fwVersion;
	free(addArray);
	free(readback);
	return status;
}

int DutSharedClssHeader SpiGetProdFwVersion(int deviceType, void *pObj,  
										   DWORD *pProdFwVersion)
{

#define u8 BYTE
#define u16 WORD
#define u32 DWORD


	DWORD status=0;
	int index=0, len=1;
	DWORD address=0x11020,*addArray=NULL, *readback=0;
//	DWORD prodFw;

    typedef struct mfg_CmdFwVers_t
    {
   	    DWORD		FwVersion;
	}	mfg_CmdFwVers_t;
    
    
	//DFU_PREFIX_t m_prefix;
	len=(sizeof(DWORD));
	addArray=(DWORD*)malloc(len*sizeof(DWORD)); 
	readback=(DWORD*)malloc(len*sizeof(DWORD)); 
	status = DumpE2PData(deviceType, pObj, address, len, addArray, readback);
	if(status)
	{
		free(addArray);
		free(readback);
		return status;
	}
	if(pProdFwVersion) (*pProdFwVersion) = ((mfg_CmdFwVers_t *)readback)->FwVersion;
	free(addArray);
	free(readback);
	return status;
}
