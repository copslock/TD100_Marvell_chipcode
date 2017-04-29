/** @file DutSharedCal.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include <stdio.h>
#include "../../DutCommon/Clss_os.h"
#include "../../DutCommon/utilities_os.h"
#include "../../DutCommon/utilities.h"

//#include DUTCLASS_SHARED_STDH_PATH

#include DUTCLASS_SHARED_H_PATH // "DutSharedClss.h"
 
#include DUTCLASS_WLAN_HD_PATH	//	"../DutWlanApi878XDll/DutWlanApiClss.hd"

#if defined(_WLAN_)
#include DUTCLASS_WLAN_H_PATH	//"../DutWlanApi878XDll/DutWlanApiClss.h"
#endif	// #if defined(_WLAN_)

#if defined(_BT_)
#include DUTCLASS_BT_H_PATH		//"../DutBtApi878XDll/DutBtApiClss.h"   
#endif	//#if defined(_BT_)

#if 1

#ifndef _FLASH_
#define SUPPORT_ANNEX41
#endif // #ifndef _FLASH_
int DutSharedClssHeader GetHeaderTbl(int deviceType, void *pObj, DWORD HeaderTbl[], DWORD *Version, int *CsC)
{	
	int status=ERROR_NONE;
	BYTE *dataTmp=NULL, *ptr=NULL; 
	int index=0; 
 

	status = SpiGetHeaderTbl(deviceType, pObj, HeaderTbl);
	if(status) return status;
 
	return CheckHeaderTbl(HeaderTbl, Version, CsC);
}

int DutSharedClssHeader GetHeaderPresent(int deviceType, void *pObj, int *pPresent, DWORD *pVersion, int *CsC)
{	
	int status=ERROR_NONE;
	BYTE *dataTmp=NULL, *ptr=NULL; 
	int index=0; 
	DWORD HeaderTbl[VMR_HEADER_LENGTH]={0};

	status = SpiGetHeaderTbl(deviceType, pObj, HeaderTbl);
	if(status) return status;
 
	if (VMR_SIGNATURE == HeaderTbl[0]) 
		*pPresent = 1;
	else
	{
		*pPresent = 0;
		return 0;
	}

	if(pVersion) 
	{
		*pVersion =  ((HeaderTbl[VMR_SPIVERSION_OFFSET/sizeof(DWORD)] >>16 ) &0xFFFF);
		
 		if(0x0 == (*pVersion) || 0xFFFF == (*pVersion))
 			*pVersion = 0xFFFFFFFF;

		DebugLogRite("Version 0x%08X \n",*pVersion);
	}
 // DoCsCheck 
	if(CsC) DoCs0Check(HeaderTbl, VMR_HEADER_LENGTH, CsC);

	return status;
}

int DutSharedClssHeader CheckCalDataSpace(int deviceType, void *pObj, int* pAvalable)
{

	int status=ERROR_NONE;
	int headerPresent=0; 
 	DWORD address=0, signature=0;

 
	status = DumpE2PData(deviceType, pObj, 
					0,  1, 
					 &address, &signature);

    if(VMR_SIGNATURE == signature ) headerPresent=1;
	if(status) return status;

	if(headerPresent)
	{
		status = DumpE2PData(deviceType, pObj, 
							VMR_OFFSET_CAL_OFFSET,  1, 
							&address, &signature);

#ifdef _FLASH_
		address = signature;
		status = DumpE2PData(deviceType, pObj, 
							address,  1, 
							&address, &signature);
		
		if(signature == 0xFFFFFFFF) 
			(*pAvalable) = true;
		else 
			(*pAvalable) = false;

#else //#ifdef _FLASH_
		if(signature == 0xFFFFFFFF) 
		{
			signature = 0x80;
			status = WriteLumpedData(deviceType, pObj, 
							VMR_OFFSET_CAL_OFFSET,  1, 
							&signature);
			signature = 0x40;
			status = WriteLumpedData(deviceType, pObj, 
							VMR_OFFSET_CAL_LENGTH,  1, 
							&signature);
		}
		(*pAvalable) = true;

#endif //#ifdef _FLASH_

	}
	else
	{
		printf("pAvalable=false\n");
		(*pAvalable) = false;
	}
	return status;

}
 
int DutSharedClssHeader GetEepromSize(int deviceType,  void *pObj, int *pSizeInKb, int *pMaxAddress)
{	
	int status=ERROR_NONE;
	int headerPresent; 
 	DWORD address=0, signature=0;

	if(pSizeInKb) 
		*pSizeInKb=0;
	if(pMaxAddress) 
		*pMaxAddress=0;

	status = GetHeaderPresent(deviceType, pObj, &headerPresent, NULL, NULL);
	if(status) return status;
 
	if(headerPresent)
	{
		do
		{
			address += 0x100;
			status = DumpE2PData(deviceType, pObj, 
								address,  1, &address, &signature);

		}while(VMR_SIGNATURE != signature && !status);

		if(pSizeInKb) 
			*pSizeInKb		=  address*sizeof(BYTE)/0x400; 
		if(pMaxAddress) 
			*pMaxAddress	= address-1;
	}

	return status;
}

int DutSharedClssHeader SetHeaderTbl(int deviceType,  void *pObj, DWORD HeaderTbl[])
{	
  	int status=ERROR_NONE;

	BYTE *dataTmp=NULL, *ptr=NULL; 
	WORD sum=0;
	int index=0; 

  	DoCs0((DWORD*)HeaderTbl, VMR_HEADER_LENGTH);

	status = SpiSetHeaderTbl(deviceType, pObj, HeaderTbl);

	return status;
}

int DutSharedClssHeader GetCalTbl(int deviceType, void *pObj, 
								  DWORD *CalData,  DWORD *length, int *CsC)
{	
	int status=ERROR_NONE;
	CardSpecData *DataArray=NULL ;

	status = getCalMainTbl(deviceType, pObj, CalData, length);
	//	Spi_GetCalMainTbl(CalData, length);
 	if((*length) != sizeof(CardSpecData)) return ERROR_MISMATCH;
	
	DataArray = (CardSpecData*)CalData; 

// DoCsCheck
	DoCs1Check((BYTE*)CalData, *length, CsC);
 
  	return status;
}
  
int DutSharedClssHeader getCalMainTbl(int deviceType, void *pObj, 
									  DWORD CalData[], DWORD *sizeOfTblInByte)
{
  	DWORD offset=0,  *AddArray=NULL;
 	DWORD len=0; 
	int status=0;
	//		Menu
	//
	if(DeBugLog) 
		DebugLogRite("### Get Cal Table###\n");

	if(g_NoEeprom)
	{
		if(deviceType)
		{
			status = ReadCalDataFile(g_NoEepromBtFlexFileName, 0,  (*sizeOfTblInByte)/sizeof(DWORD),  CalData);
		}
		else
		{
			status = ReadCalDataFile(g_NoEepromWlanFlexFileName, 0,  (*sizeOfTblInByte)/sizeof(DWORD),  CalData);
		}
	}
	else
	{
#ifndef _LINUX_
  		switch(deviceType)
		{ 
#if defined(_WLAN_)
			case DEVICETYPE_WLAN:
				status = ((class DutWlanApiClss*)pObj)->Wifi_SpiReadData(VMR_OFFSET_CAL_LENGTH, 
																	1,	&len);
				if(status) return status;

				(len) &= 0xFFFF;

				status = ((class DutWlanApiClss*)pObj)->Wifi_SpiReadData( VMR_OFFSET_CAL_OFFSET, 
														1, &offset);
				if(status) return status;
				break;
#endif	//#if defined(_WLAN_)

#if defined(_BT_)
			case DEVICETYPE_BT: 
				status = ((class DutBtApiClss*)pObj)->Bt_SpiReadData( VMR_OFFSET_CAL_LENGTH, 1,	&len);
				if(status) return status;

				(len) &= 0xFFFF;

				status = ((class DutBtApiClss*)pObj)->Bt_SpiReadData( VMR_OFFSET_CAL_OFFSET, 1,	&offset);
				if(status) return status;
				
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
				status = Wifi_SpiReadData(VMR_OFFSET_CAL_LENGTH, 
																	1,	&len);
				if(status) return status;

				(len) &= 0xFFFF;

				status = Wifi_SpiReadData( VMR_OFFSET_CAL_OFFSET, 
														1, &offset);
				if(status) return status;
				break;
#endif	//#if defined(_WLAN_)

#if defined(_BT_)
			case DEVICETYPE_BT: 
				status = Bt_SpiReadData( VMR_OFFSET_CAL_LENGTH, 1,	&len);
				if(status) return status;

				(len) &= 0xFFFF;

				status = Bt_SpiReadData( VMR_OFFSET_CAL_OFFSET, 1,	&offset);
				if(status) return status;
				
 
				break;
#endif	//#if defined(_BT_)
			default:
				status= ERROR_UNSUPPORTEDDEVICETYPE;
				break;
		}
#endif // #ifndef _LINUX_
	
		if(0xFFFFFFFF == offset || 0xFFFF == (len))
			return ERROR_MISMATCH;


		if(*sizeOfTblInByte != (len))
			return ERROR_MISMATCH;

 
		AddArray = (DWORD*) malloc((*sizeOfTblInByte));  
		status = DumpE2PData(deviceType, pObj, offset,  (*sizeOfTblInByte)/sizeof(DWORD),  AddArray, CalData );
				
		free(AddArray); 
	}
	return status; 
}

int DutSharedClssHeader setCalMainTbl(int deviceType, void *pObj, DWORD *DataArray, DWORD *sizeOfTblInByte)
{
 int index=0, status=0; 
 DWORD offset=0, dataTmp ={0};

	//		Menu
	//
//	DebugLogRite("\n\n");
	if(DeBugLog) 
		DebugLogRite("### Set Cal Table###\n");
	
 
// Initialize Slow Bus
 if(0 == g_NoEeprom)
 {
#ifndef _LINUX_
		switch(deviceType)
		{ 
#if defined(_WLAN_)
			case DEVICETYPE_WLAN:
				status = ((class DutWlanApiClss*)pObj)->Wifi_SpiReadData(VMR_OFFSET_CAL_OFFSET, 1,	&offset);
				if(status) return status;

				status = ((class DutWlanApiClss*)pObj)->Wifi_SpiReadData(VMR_OFFSET_CAL_LENGTH,	 1, &dataTmp);
				if(status) return status;

				break;
#endif	//#if defined(_WLAN_)

#if defined(_BT_)
 			case DEVICETYPE_BT: 
				status = ((class DutBtApiClss*)pObj)->Bt_SpiReadData(VMR_OFFSET_CAL_OFFSET, 1,	&offset);
				if(status) return status;

				status = ((class DutBtApiClss*)pObj)->Bt_SpiReadData(VMR_OFFSET_CAL_LENGTH,	 1, &dataTmp);
				if(status) return status;
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
				status = Wifi_SpiReadData(VMR_OFFSET_CAL_OFFSET, 1,	&offset);
				if(status) return status;

				status = Wifi_SpiReadData(VMR_OFFSET_CAL_LENGTH,	 1, &dataTmp);
				if(status) return status;
				break;
#endif	//#if defined(_WLAN_)

#if defined(_BT_)
 			case DEVICETYPE_BT: 
				status = Bt_SpiReadData(VMR_OFFSET_CAL_OFFSET, 1,	&offset);
				if(status) return status;

				status = Bt_SpiReadData(VMR_OFFSET_CAL_LENGTH,	 1, &dataTmp);
				if(status) return status;
 				break;
#endif	//#if defined(_BT_)
			default:
				status= ERROR_UNSUPPORTEDDEVICETYPE;
				break;
		}

#endif // #ifndef _LINUX_

		*sizeOfTblInByte =(dataTmp&0xFFFF);

		if(0xFFFFFFFF == offset || 0xFFFF == (*sizeOfTblInByte))
			return ERROR_MISMATCH; 
		status = WriteLumpedData(deviceType, pObj,
									offset, (*sizeOfTblInByte)/sizeof(DWORD),  DataArray );
 }

	if(g_NoEeprom)
	{
		if(deviceType)
		{
			status = InitCalDataFile(g_NoEepromBtFlexFileName);
			if(status) return status;

			status = WriteCalDataFile(g_NoEepromBtFlexFileName, 
							offset, 
							(*sizeOfTblInByte)/sizeof(DWORD),  
							DataArray);
			if(status) return status;

		}
		else
		{

			status = InitCalDataFile(g_NoEepromWlanFlexFileName);
			if(status) return status;

			status = WriteCalDataFile(g_NoEepromWlanFlexFileName,
							 offset, 
							(*sizeOfTblInByte)/sizeof(DWORD),  
							DataArray);
			if(status) return status;
		}
	}

	return status;
}

int DutSharedClssHeader GetNextAnnexPtr(int deviceType, void *pObj, int myPtr, int *nextPointer) 
{	
  	int status=ERROR_NONE;
	DWORD address; 

	if(g_NoEeprom)
	{
		if(deviceType)
		{
			status = ReadCalDataFile(g_NoEepromBtFlexFileName, myPtr+4, 1, (DWORD*)nextPointer);
		}
		else
		{
			status = ReadCalDataFile(g_NoEepromWlanFlexFileName, myPtr+4, 1, (DWORD*)nextPointer);
		}
		return status;
	}
 	else
	{
		status = DumpE2PData(deviceType, pObj, 
							myPtr+4, 1,  &address,  (DWORD*)nextPointer);
	}
	return status;
}
 
// out, out, out
int DutSharedClssHeader GetFirstAnnexInfo(int deviceType, void *pObj, 
										  int *pointer, int *sizeOfTblInByte, int* type,
							 int *DeviceId, int *PathId, 
							 int *BandId,	int *SubBandId)
{   // go to the main data structure to get Pointer, 
   	int status=0;
	ANNEX_TYPE_HEADER_EXTENDED_VC address={0}, Header={0};

	CardSpecData MainTbl={0};
	int length=sizeof(CardSpecData); // in byte
 	int CsC=0; 

	status = GetCalTbl(deviceType, pObj, (DWORD*)&MainTbl, (DWORD*)&length,  &CsC);
 
	if(status) return status;
	if(length != sizeof(CardSpecData)) return ERROR_MISMATCH; 
	if(	0xFF != MainTbl.StructureRevision && 
		0x00 != MainTbl.StructureRevision && 
		0x0D >= MainTbl.StructureRevision)	
	{ 
 		if(pointer) *pointer =MainTbl.PtrFirstAnnex;

		// use pointer to get header infor for the annex

		if(g_NoEeprom)
		{
			if(deviceType)
			{
				status = ReadCalDataFile(g_NoEepromBtFlexFileName, *pointer, 
					sizeof(ANNEX_TYPE_HEADER_EXTENDED_VC)/sizeof(DWORD), 
					(DWORD*)&Header);
			}
			else
			{
				status = ReadCalDataFile(g_NoEepromWlanFlexFileName, *pointer, 
					sizeof(ANNEX_TYPE_HEADER_EXTENDED_VC)/sizeof(DWORD), 
					(DWORD*)&Header);
			}

		}
		else
		{
			status = DumpE2PData(deviceType, pObj, *pointer,
							sizeof(ANNEX_TYPE_HEADER_EXTENDED_VC)/sizeof(DWORD),
							(DWORD*)&address, (DWORD*)&Header);
		}
		if(sizeOfTblInByte) *sizeOfTblInByte = Header.typeHdr.LengthInByte;
		if(type)		*type = Header.typeHdr.AnnexType;
		if(DeviceId)	*DeviceId = Header.DeviceId; 
		if(PathId)		*PathId = Header.PathId; 
		if(BandId)		*BandId = Header.BandId;
		if(SubBandId)	*SubBandId = Header.SubBandId;

	}
	else
	{
 	//	status = ERROR_MISMATCH;
		if(pointer) *pointer =ANNEX_ADDR_END;
 
		// use pointer to get header infor for the annex
		if(sizeOfTblInByte) *sizeOfTblInByte = 0;
		if(type)		*type = 0;
		if(DeviceId)	*DeviceId = 0; 
		if(PathId)		*PathId = 0; 
		if(BandId)		*BandId = 0;
		if(SubBandId)	*SubBandId = 0;
		 
	}

	return status;
}

int DutSharedClssHeader GetAnnexInfo(int deviceType, void *pObj, int pointer,	int *sizeOfTblInByte, int* type,
							 int *DeviceId, int *PathId, 
							 int *BandId,	int *SubBandId) 
{    
	ANNEX_TYPE_HEADER_EXTENDED_VC address={0}, Header={0};
  	int status=0;
	// use pointer to get header infor for the annex
	if(g_NoEeprom)
	{
		if(deviceType)
		{
			status = ReadCalDataFile(g_NoEepromBtFlexFileName, pointer, 
						sizeof(ANNEX_TYPE_HEADER_EXTENDED_VC)/sizeof(DWORD), 
						(DWORD*)&Header);
		}
		else
		{
			status = ReadCalDataFile(g_NoEepromWlanFlexFileName, pointer, 
						sizeof(ANNEX_TYPE_HEADER_EXTENDED_VC)/sizeof(DWORD), 
						(DWORD*)&Header);
		}

	}
	else
	{
		status = DumpE2PData(deviceType, pObj, pointer,
						sizeof(ANNEX_TYPE_HEADER_EXTENDED_VC)/sizeof(DWORD),
						(DWORD*)&address, (DWORD*)&Header);
	}
	if(sizeOfTblInByte) *sizeOfTblInByte = Header.typeHdr.LengthInByte;
	if(type)		*type = Header.typeHdr.AnnexType;
	if(DeviceId)	*DeviceId = Header.DeviceId; 
	if(PathId)		*PathId = Header.PathId; 
	if(BandId)		*BandId = Header.BandId;
	if(SubBandId)	*SubBandId = Header.SubBandId;

	if( ((*type) == ANNEX_TYPE_NUM50) 
		||((*type) == ANNEX_TYPE_NUM52)
		||((*type) == ANNEX_TYPE_NUM53)
		||((*type) == ANNEX_TYPE_NUM54))
	{
    	if(DeviceId)	*DeviceId = (Header.DeviceId)&0x0f; 
	    if(PathId)		*PathId = (Header.DeviceId)&0xf0 >>8; 
    	if(BandId)		*BandId = 1;
	    if(SubBandId)	*SubBandId = Header.PathId;
	}
	
	if( ((*type) == ANNEX_TYPE_NUM55)
		||((*type) == ANNEX_TYPE_NUM56)
		||((*type) == ANNEX_TYPE_NUM57))
	{
		if(DeviceId)	*DeviceId = 0; 
		if(PathId)		*PathId = 0; 
	}

	return 0;
}

int DutSharedClssHeader SetAnnexTblInfo(int deviceType, void*pObj, 
										ANNEX_TYPE_HEADER_VC	*pAnnex ,
									   int LenofAnnex, int *pNextOffsetInMem, 
									   int *pUsedSize, int *pTotalSize, 
#ifdef _FLASH_
									   bool lastAnnex,
#endif //#ifdef _FLASH_
									   char *pFlexFileNameNoE2prom)
{

	int status=0;
			
	int sizeOfTblInByte = 0;
	int offsetInMemThis = 0;
 
	sizeOfTblInByte = LenofAnnex;
	offsetInMemThis = (*pNextOffsetInMem); 
	(*pNextOffsetInMem) += sizeOfTblInByte; 
	if(pUsedSize) (*pUsedSize) += sizeOfTblInByte; 

#ifdef _FLASH_
	if(lastAnnex)
	{
		(*pNextOffsetInMem) = ANNEX_ADDR_END;
	}
	else
#else //#ifdef _FLASH_
	{
		if(		(pUsedSize && pTotalSize)
			&&	(*pUsedSize) == (*pTotalSize) 
		  )
			(*pNextOffsetInMem) = ANNEX_ADDR_END;
	}
#endif //#ifdef _FLASH_

	pAnnex->PtrNextAnnex= (*pNextOffsetInMem); 
	pAnnex->Cs = 0; 
	pAnnex->Cs = DoCs1((BYTE*)(pAnnex), LenofAnnex); 

	status = SetMainAnnexTbl(deviceType, pObj, 
				&sizeOfTblInByte,  offsetInMemThis,  (DWORD*)pAnnex);  


	return status;
}

int DutSharedClssHeader GetCalDataRevC(int deviceType, void*pObj, 
									   int *CsC, int *CsCValue, 
									   char *FlexFileNameNoE2prom)
{
	int status=0;
 
	void *ptr=NULL; 
	int CsCTemp=0; 
 	int CntTotal=0;

	int endOfCal=ANNEX_ADDR_END;
  	int pointer=0, nextPtr=0, type=0, len=0, structureRev=0;
	int DeviceId=0,  PathId=0,  BandId=0, SubBandId=0;
	 int SocId=0;
  
	if(FlexFileNameNoE2prom)
	{
		if(deviceType)
			strcpy(g_NoEepromBtFlexFileName, FlexFileNameNoE2prom);
		else
			strcpy(g_NoEepromWlanFlexFileName, FlexFileNameNoE2prom);
	}

	PurgeAnnexPointers();
 

	if(NULL == pMain)
		pMain = (CardSpecData*) malloc(sizeof(CardSpecData));

	len = sizeof(CardSpecData); 
 	status = 	GetCalTbl( deviceType,  pObj, (DWORD*)pMain, (DWORD*)&len,  &CsCTemp);
	if(status) return status;

  	if(	(0xFF != ((CardSpecData*)pMain)->StructureRevision && 
		0x00 != ((CardSpecData*)pMain)->StructureRevision && 
		(STRUCTUREREV != ((CardSpecData*)pMain)->StructureRevision))
		|| !CsC)
	{
		if(STRUCTUREREV != ((CardSpecData*)pMain)->StructureRevision) 
			status = ERROR_MISMATCH;
		else
			status = ERROR_DATANOTEXIST;
		free(pMain);
		pMain =NULL; 
 	
		return status;
	}

 	(*CsCValue)+=CsCTemp; 
	CntTotal ++; // for main
	// now handling annexes
	status  =   GetFirstAnnexInfo( deviceType,  pObj, 
					&pointer, (int*)&len, &type, 
									&DeviceId,  &PathId,  
									&BandId, &SubBandId);

	
 	while (ANNEX_ADDR_END != pointer 
		&& (g_NoEeprom?true:ANNEX_ADDR_BLANK != pointer)
		)
	{
		if(0xFF ==  type && 0xFFFF == len) 
			break;

		CntTotal++; 
// get that annex
 		if(DeBugLog)
		{
			DebugLogRite("found Annextype %d at %X\n", type, pointer);
// 			DebugLogRite("found Device %d Path %d Band %d SubBand %d \n", 
//						DeviceId, PathId, BandId, SubBandId);
		}

		switch(type)
		{	   
		case ANNEX_TYPE_NUM24: 
 			if(sizeof(ANNEX_TYPE_24_BBUSETTING_VC) != len) break;

			if(NULL == pAnnex24)
				pAnnex24 =  malloc(sizeof(ANNEX_TYPE_24_BBUSETTING_VC));

			status = GetMainAnnexTbl( deviceType, pObj, &len, pointer, 
							(DWORD*)pAnnex24, 
							&CsCTemp);
 			(*CsCValue)+=CsCTemp; 
			break;

		case ANNEX_TYPE_NUM25: 
 			if(sizeof(ANNEX_TYPE_25_SOCSETTING_VC) != len) break;

			if(NULL == pAnnex25)
				pAnnex25 = 
							malloc(sizeof(ANNEX_TYPE_25_SOCSETTING_VC));

			status = GetMainAnnexTbl( deviceType, pObj, &len, pointer, 
							(DWORD*)pAnnex25, 
							&CsCTemp);
 			(*CsCValue)+=CsCTemp; 
			break;

		case ANNEX_TYPE_NUM27:
 			if(sizeof(ANNEX_TYPE_27_LED_VC) != len) break;

			if(NULL == pAnnex27)
				pAnnex27 = malloc(sizeof(ANNEX_TYPE_27_LED_VC));

			status = GetMainAnnexTbl( deviceType, pObj, &len, pointer, 
							(DWORD*)pAnnex27, 
							&CsCTemp);
 			(*CsCValue)+=CsCTemp; 
			break;

		case ANNEX_TYPE_NUM28:
			if(sizeof(ANNEX_TYPE_28_USBSTRINGS_VC) != len) break;

			if(NULL == pAnnex28)
				pAnnex28 = malloc(sizeof(ANNEX_TYPE_28_USBSTRINGS_VC));

			status = GetMainAnnexTbl( deviceType, pObj, &len, pointer, 
							(DWORD*)pAnnex28, 
							&CsCTemp);
 			(*CsCValue)+=CsCTemp; 
			break;
		
		case ANNEX_TYPE_NUM31:
 			if(sizeof(ANNEX_TYPE_31_47_RSSI_G_VC) != len) break;

			if(MIMODEVICE0!= DeviceId	&& MIMODEVICE1 != DeviceId) break;
 
			if(NULL == pAnnex31[DeviceId])
				pAnnex31[DeviceId] = 
							malloc(sizeof(ANNEX_TYPE_31_47_RSSI_G_VC));

			status = GetMainAnnexTbl( deviceType, pObj, &len, pointer, 
							(DWORD*)pAnnex31[DeviceId], 
							&CsCTemp);
 			(*CsCValue)+=CsCTemp; 
			break;
 
		case ANNEX_TYPE_NUM32:
 			if(sizeof(ANNEX_TYPE_32_48_RSSI_A_VC) != len) break;

			if(MIMODEVICE0!= DeviceId	&& MIMODEVICE1 != DeviceId) break;
 
			if(NULL == pAnnex32[DeviceId])
				pAnnex32[DeviceId] = 
							malloc(sizeof(ANNEX_TYPE_32_48_RSSI_A_VC));

			status = GetMainAnnexTbl( deviceType, pObj, &len, pointer, 
							(DWORD*)pAnnex32[DeviceId], 
							&CsCTemp);
 			(*CsCValue)+=CsCTemp; 
			break;
 
		case ANNEX_TYPE_NUM40:
 			if(sizeof(ANNEX_TYPE_40_PM_VC) != len) break;

			 

			if(NULL == pAnnex40)
				pAnnex40 = malloc(sizeof(ANNEX_TYPE_40_PM_VC));

			status = GetMainAnnexTbl( deviceType, pObj, &len, pointer, 
							(DWORD*)pAnnex40, 
							&CsCTemp);
 			(*CsCValue)+=CsCTemp; 
			break;
 
  		case ANNEX_TYPE_NUM42:
			if(NULL == pAnnex42)
				pAnnex42 = malloc(len);

			status = GetMainAnnexTbl( deviceType, pObj, &len, pointer, 
							(DWORD*)pAnnex42, 
							&CsCTemp);
 			(*CsCValue)+=CsCTemp; 
			break;

		case ANNEX_TYPE_NUM45:
 			if(sizeof(ANNEX_TYPE_45_SLEEPMODE_VC) != len) break;
	
			if(NULL == pAnnex45)
				pAnnex45 = malloc(sizeof(ANNEX_TYPE_45_SLEEPMODE_VC));

			status = GetMainAnnexTbl( deviceType, pObj, &len, pointer, 
							(DWORD*)pAnnex45, 
							&CsCTemp);
 			(*CsCValue)+=CsCTemp; 
			break;

		case ANNEX_TYPE_NUM49:			
			if(MIMODEVICE0 != DeviceId	&& MIMODEVICE1 != DeviceId) break;
			if( RXPATH_PATHA != PathId	&& 
				RXPATH_PATHB != PathId	&& 
				RXPATH_PATHC != PathId)	
				break;
			if(NULL == pAnnex49[DeviceId][PathId])
				pAnnex49[DeviceId][PathId] = malloc(sizeof(ANNEX_TYPE_49_2G_POWERTABLE_VD));

			status = GetMainAnnexTbl( deviceType, pObj, &len, pointer, 
							(DWORD*)pAnnex49[DeviceId][PathId], 
							&CsCTemp);
 			(*CsCValue)+=CsCTemp; 
			break;

		case ANNEX_TYPE_NUM50:
        	if(sizeof(ANNEX_TYPE_50_5G_POWERTABLE_VD) != len) break;

			if(MIMODEVICE0!= DeviceId	&& MIMODEVICE1 != DeviceId) break;
			if(TXPATH_PATHA!= PathId	&& TXPATH_PATHB != PathId)	break;
			if(BAND_802_11G != BandId	&& BAND_802_11A != BandId)	break;
			if(SubBandId <0) break;
			if (BAND_802_11G == BandId	&& NUM_SUBBAND_G <= SubBandId) break;
			if (BAND_802_11A == BandId	&& NUM_SUBBAND_A <= SubBandId) break;
		
			if(NULL == pAnnex50[DeviceId][PathId][SubBandId])
				pAnnex50[DeviceId][PathId][SubBandId] = 
							malloc(sizeof(ANNEX_TYPE_50_5G_POWERTABLE_VD));

			status = GetMainAnnexTbl(deviceType, pObj, &len, pointer, 
				(DWORD*)pAnnex50[DeviceId][PathId][SubBandId], &CsCTemp);
 				(*CsCValue)+=CsCTemp; 
			break;

		case ANNEX_TYPE_NUM52:
 			if(sizeof(ANNEX_TYPE_52_LOLEAKAGE_VD) != len) break;

			if(MIMODEVICE0 != DeviceId	&& MIMODEVICE1 != DeviceId) break;
			if( RXPATH_PATHA != PathId	&& 
				RXPATH_PATHB != PathId	&& 
				RXPATH_PATHC != PathId)	
				break;

			if(NULL == pAnnex52[DeviceId][PathId])
				pAnnex52[DeviceId][PathId] = malloc(sizeof(ANNEX_TYPE_52_LOLEAKAGE_VD));

			status = GetMainAnnexTbl( deviceType, pObj, &len, pointer, 
							(DWORD*)pAnnex52[DeviceId][PathId], 
							&CsCTemp);
 			(*CsCValue)+=CsCTemp; 
			break;

		case ANNEX_TYPE_NUM53:
 			if(sizeof(ANNEX_TYPE_53_TX_IQ_LOLEAKAGE_VD) != len) break;

			if(MIMODEVICE0 != DeviceId	&& MIMODEVICE1 != DeviceId) break;
			if( RXPATH_PATHA != PathId	&& 
				RXPATH_PATHB != PathId	&& 
				RXPATH_PATHC != PathId)	
				break;

			if(NULL == pAnnex53[DeviceId][PathId])
				pAnnex53[DeviceId][PathId] = malloc(sizeof(ANNEX_TYPE_53_TX_IQ_LOLEAKAGE_VD));

			status = GetMainAnnexTbl( deviceType, pObj, &len, pointer, 
							(DWORD*)pAnnex53[DeviceId][PathId], 
							&CsCTemp);
 			(*CsCValue)+=CsCTemp; 
			break;
		  
		case ANNEX_TYPE_NUM54:
 			if(sizeof(ANNEX_TYPE_54_RX_IQ_LOLEAKAGE_VD) != len) break;

			if(MIMODEVICE0 != DeviceId	&& MIMODEVICE1 != DeviceId) break;
			if( RXPATH_PATHA != PathId	&& 
				RXPATH_PATHB != PathId	&& 
				RXPATH_PATHC != PathId)	
				break;

			if(NULL == pAnnex54[DeviceId][PathId])
				pAnnex54[DeviceId][PathId] = malloc(sizeof(ANNEX_TYPE_54_RX_IQ_LOLEAKAGE_VD));

			status = GetMainAnnexTbl( deviceType, pObj, &len, pointer, 
							(DWORD*)pAnnex54[DeviceId][PathId], 
							&CsCTemp);
 			(*CsCValue)+=CsCTemp; 
			break;

		case ANNEX_TYPE_NUM55:
 			if(sizeof(ANNEX_TYPE_55_BT_CONFIGURE) != len) break;
			if(NULL == pAnnex55)
				pAnnex55 = malloc(sizeof(ANNEX_TYPE_55_BT_CONFIGURE));

			status = GetMainAnnexTbl( deviceType, pObj, &len, pointer, 
							(DWORD*)pAnnex55, 
							&CsCTemp);
 			(*CsCValue)+=CsCTemp; 
			break;

		case ANNEX_TYPE_NUM56:
 			if(sizeof(ANNEX_TYPE_56_BT_SECURITY_DATA) != len) break;

			if(NULL == pAnnex56)
				pAnnex56 = malloc(sizeof(ANNEX_TYPE_56_BT_SECURITY_DATA));

			status = GetMainAnnexTbl( deviceType, pObj, &len, pointer, 
							(DWORD*)pAnnex56, 
							&CsCTemp);
 			(*CsCValue)+=CsCTemp; 
			break;

		case ANNEX_TYPE_NUM57:
			{
			if(MIMODEVICE0 != DeviceId	&& MIMODEVICE1 != DeviceId) break;
			if(TXPATH_PATHA!= PathId	&& TXPATH_PATHB != PathId)	break;

			void* pTemp=malloc(len);

			status = GetMainAnnexTbl( deviceType, pObj, &len, pointer, 
							(DWORD*)pTemp, 
							&CsCTemp);
			SubBandId = ((ANNEX_TYPE_57_DPD_DATA*)pTemp)->SubBandId;
			BandId = ((ANNEX_TYPE_57_DPD_DATA*)pTemp)->BandId;

			if(NULL == pAnnex57[DeviceId][PathId][SubBandId+BandId])
				pAnnex57[DeviceId][PathId][SubBandId+BandId] = pTemp;
			else
			{
				memcpy(&(pAnnex57[DeviceId][PathId][SubBandId+BandId]), pTemp, len);
				free(pTemp);
			}

			(*CsCValue)+=CsCTemp;
			break;
			}

		default:
			{	
				void *ptr=NULL;

				ptr =  malloc(len);

				status = GetMainAnnexTbl( deviceType, pObj, &len, pointer, 
							(DWORD*)ptr, 
							&CsCTemp);
				free(ptr);
 				(*CsCValue)+=CsCTemp; 
			}

			if(DeBugLog)
			{
				DebugLogRite("unknown type of annex %d\n", type); 
			}
			break;
		}
 			
	if(DeBugLog)
	{
		DebugLogRite("CsC for this annex %d\n", CsCTemp); 
	}


		// get next pointer
		// status =Spi_DumpData(ptr+4, 1, (DWORD*)&address,  (DWORD*)&nextPtr);
			status =GetNextAnnexPtr(deviceType, pObj, pointer, &nextPtr); 
			if(status) return status;
			if(ANNEX_ADDR_END == nextPtr || ANNEX_ADDR_BLANK == nextPtr) 
			{	  
				break;
			}
			pointer = nextPtr;
			GetAnnexInfo( deviceType, pObj, 
						pointer, &len, &type, 
						&DeviceId,  &PathId,  
						&BandId, &SubBandId); 
	};


	if(DeBugLog)
	{
		DebugLogRite("CsCValue = %d (CntTotal = %d)\n", 
						*CsCValue, CntTotal); 
	}
  
	if(CsC)
	{
		if((*CsCValue) == CntTotal ) 
			*CsC = 1;
		else 
			*CsC = 0;
	}

  
	return status;

}

int DutSharedClssHeader SetCalDataRevC(int deviceType, void *pObj, 
									   int PurgeAfter, char *FlexFileNameNoE2prom) 
{
	int status=0;
 
	int i=0, j=0, k=0;
	int sizeOfTblInByte=0, offsetInMem=0, NextoffsetInMem=0; 
	int totalSize=0, usedSize=0;
	int offsetInMemAnnex_Next=0; 
	int endOfCal=ANNEX_ADDR_END, UsedSpace=0;
	int MaxAddrInEeprom =0x3FF; 

	if(FlexFileNameNoE2prom)
    {
		if(deviceType)
			strcpy(g_NoEepromBtFlexFileName, FlexFileNameNoE2prom);
		else
			strcpy(g_NoEepromWlanFlexFileName, FlexFileNameNoE2prom);
    }
    
	if(NULL == pMain) return ERROR_DATAMAINNOTSET;

	if(!g_NoEeprom)
	{
		status = CheckCalDataSpace(deviceType, pObj, &totalSize);
		if(status) return status;
	
		if(false == totalSize) 
			return ERROR_SPACENOTAVAILABLE;
	}
	totalSize=0;

// total size
//	if(pMain) totalSize+=	 sizeof(CardSpecData);
	if(pAnnex24)  totalSize+=	 sizeof(ANNEX_TYPE_24_BBUSETTING_VC);
	if(pAnnex25)  totalSize+=	 sizeof(ANNEX_TYPE_25_SOCSETTING_VC);
	if(pAnnex27)	totalSize+=	 sizeof(ANNEX_TYPE_27_LED_VC);
	if(pAnnex28)	totalSize+=	 sizeof(ANNEX_TYPE_28_USBSTRINGS_VC);
	for (i=0; i<MAXNUM_MIMODEVICE ; i++)
	{
		if(pAnnex31[i])  totalSize+=	 sizeof(ANNEX_TYPE_31_47_RSSI_G_VC);
		if(pAnnex32[i])  totalSize+=	 sizeof(ANNEX_TYPE_32_48_RSSI_A_VC);
	}
	if(pAnnex40)  totalSize+=	 sizeof(ANNEX_TYPE_40_PM_VC);
	if(pAnnex42)  totalSize+=	 ((ANNEX_TYPE_42_THERMALCAL_VC*)pAnnex42)->typeHdr.LengthInByte;
	if(pAnnex45)  totalSize+=	 sizeof(ANNEX_TYPE_45_SLEEPMODE_VC);

	for (i=0; i<MAXNUM_MIMODEVICE ; i++)
	{
		for (j=0; j<MAXNUM_RXPATH ; j++)
		{
			if(pAnnex47[i][j]) totalSize+=	 sizeof(ANNEX_TYPE_31_47_RSSI_G_VC);

			if(pAnnex48[i][j]) totalSize+=	 sizeof(ANNEX_TYPE_32_48_RSSI_A_VC);
			if(pAnnex49[i][j])  totalSize+=	 sizeof(ANNEX_TYPE_49_2G_POWERTABLE_VD);
			
			for (k=0; k<NUM_SUBBAND_A ; k++)
			{
				if(pAnnex50[i][j][k])  totalSize+=	 sizeof(ANNEX_TYPE_50_5G_POWERTABLE_VD);
			}
			
			if(pAnnex52[i][j]) totalSize+=	 sizeof(ANNEX_TYPE_52_LOLEAKAGE_VD);
			if(pAnnex53[i][j]) totalSize+=	 sizeof(ANNEX_TYPE_53_TX_IQ_LOLEAKAGE_VD);
			if(pAnnex54[i][j]) totalSize+=	 sizeof(ANNEX_TYPE_54_RX_IQ_LOLEAKAGE_VD);
			for(int k=0; k<NUM_SUBBAND_A_REVD+NUM_SUBBAND_G_REVD;k++)
			{
				if(pAnnex57[i][j][k]) totalSize+=	 ( ((ANNEX_TYPE_57_DPD_DATA*)pAnnex57[i][j][k])->typeHdr.LengthInByte);
			}
		}
	}
  
	if(pAnnex55) totalSize+=	 sizeof(ANNEX_TYPE_55_BT_CONFIGURE);
	if(pAnnex56) totalSize+=	 sizeof(ANNEX_TYPE_56_BT_SECURITY_DATA);

// find space
#if !defined (_FLASH_) 
	if(!g_NoEeprom)
	{
		status = GetEepromSize(deviceType, pObj, NULL, &MaxAddrInEeprom);
		if(status) return status;
	
		if(0 == MaxAddrInEeprom) 
			return ERROR_SPACENOTAVAILABLE;

		if(DeBugLog) 
			DebugLogRite("MaxAddrInEeprom 0x%X\n", MaxAddrInEeprom);

	}
	else
#endif //#if !defined (_FLASH_) 
	{
		MaxAddrInEeprom = ((CardSpecData*)pMain)->SpiSize.size;
		MaxAddrInEeprom *= (((CardSpecData*)pMain)->SpiSize.unit)?1024*1024/8:1024/8;
		MaxAddrInEeprom --;
	}
		
	// find pointer for first
	status  = GetNextOpenSpace(deviceType, pObj, MaxAddrInEeprom,  0,
					 totalSize, &offsetInMemAnnex_Next);
	if(status) return status;

	if(pMain)	 
	{ 
		CardSpecData *pAnnex=NULL;
				
		pAnnex = (CardSpecData*) (pMain);
         
		NextoffsetInMem = offsetInMemAnnex_Next; 
		sizeOfTblInByte = sizeof(CardSpecData);

		pAnnex->PtrFirstAnnex= NextoffsetInMem; 
		pAnnex->Cs = 0; 
		pAnnex->Cs = DoCs1((BYTE*)(pAnnex),   
								sizeof(CardSpecData)); 

 		status = setCalMainTbl(deviceType, pObj, (DWORD*)pAnnex, (DWORD*)&sizeOfTblInByte);
		NextoffsetInMem = offsetInMemAnnex_Next; 
	}

// start to write Annex
// Annex 24
	if(pAnnex24)  
	{ 
		ANNEX_TYPE_24_BBUSETTING_VC *pAnnex=NULL;
		
		pAnnex = (ANNEX_TYPE_24_BBUSETTING_VC*) (pAnnex24);

		status = SetAnnexTblInfo(deviceType, pObj, 
								(ANNEX_TYPE_HEADER_VC*)pAnnex, 
								sizeof(ANNEX_TYPE_24_BBUSETTING_VC),
								&NextoffsetInMem, 
								&usedSize, &totalSize,
#ifdef _FLASH_
								false,
#endif //#ifdef _FLASH_
								FlexFileNameNoE2prom);
	}

// Annex 25
	if(pAnnex25)  
	{ 
		ANNEX_TYPE_25_SOCSETTING_VC *pAnnex=NULL;
		
		pAnnex = (ANNEX_TYPE_25_SOCSETTING_VC*) (pAnnex25);

		status = SetAnnexTblInfo(deviceType, pObj, 
								(ANNEX_TYPE_HEADER_VC*)pAnnex, 
								sizeof(ANNEX_TYPE_25_SOCSETTING_VC),
								&NextoffsetInMem, 
								&usedSize, &totalSize,
#ifdef _FLASH_
								false,
#endif //#ifdef _FLASH_
								FlexFileNameNoE2prom);
	}
		
// Annex 27
	if(pAnnex27)  
	{ 
		ANNEX_TYPE_27_LED_VC *pAnnex=NULL;
		
		pAnnex = (ANNEX_TYPE_27_LED_VC*) (pAnnex27);

		status = SetAnnexTblInfo(deviceType, pObj, 
								(ANNEX_TYPE_HEADER_VC*)pAnnex, 
								sizeof(ANNEX_TYPE_27_LED_VC),
								&NextoffsetInMem, 
								&usedSize, &totalSize,
#ifdef _FLASH_
								false,
#endif //#ifdef _FLASH_
								FlexFileNameNoE2prom); 
	}
		
// Annex 28
	if(pAnnex28)  
	{ 
		ANNEX_TYPE_28_USBSTRINGS_VC *pAnnex=NULL;
		
		pAnnex = (ANNEX_TYPE_28_USBSTRINGS_VC*) (pAnnex28);

		status = SetAnnexTblInfo(deviceType, pObj, 
								(ANNEX_TYPE_HEADER_VC*)pAnnex, 
								sizeof(ANNEX_TYPE_28_USBSTRINGS_VC),
								&NextoffsetInMem, 
								&usedSize, &totalSize,
#ifdef _FLASH_
								false,
#endif //#ifdef _FLASH_
								FlexFileNameNoE2prom); 
	}

	for (i=0; i<MAXNUM_MIMODEVICE ; i++)
	{
        // Annex 31
		if(pAnnex31[i])  
		{ 
			ANNEX_TYPE_31_47_RSSI_G_VC *pAnnex=NULL;
			
			pAnnex = (ANNEX_TYPE_31_47_RSSI_G_VC*) (pAnnex31[i]);

			status = SetAnnexTblInfo(deviceType, pObj, 
								(ANNEX_TYPE_HEADER_VC*)pAnnex, 
								sizeof(ANNEX_TYPE_31_47_RSSI_G_VC),
								&NextoffsetInMem, 
								&usedSize, &totalSize,
#ifdef _FLASH_
								false,
#endif //#ifdef _FLASH_
								FlexFileNameNoE2prom);
		}

		//annex 32
		if(pAnnex32[i])  
		{ 
			ANNEX_TYPE_32_48_RSSI_A_VC *pAnnex=NULL;
			
			pAnnex = (ANNEX_TYPE_32_48_RSSI_A_VC*) (pAnnex32[i]);

		status = SetAnnexTblInfo(deviceType, pObj, 
								(ANNEX_TYPE_HEADER_VC*)pAnnex, 
								sizeof(ANNEX_TYPE_32_48_RSSI_A_VC),
								&NextoffsetInMem, 
								&usedSize, &totalSize,
#ifdef _FLASH_
								false,
#endif //#ifdef _FLASH_
								FlexFileNameNoE2prom);
		}
	}

	if(pAnnex42)  
	{ 
		ANNEX_TYPE_42_THERMALCAL_VC *pAnnex=NULL;
		
		pAnnex = (ANNEX_TYPE_42_THERMALCAL_VC*) (pAnnex42);

		status = SetAnnexTblInfo(deviceType, pObj, 
								(ANNEX_TYPE_HEADER_VC*)pAnnex, 
								((ANNEX_TYPE_42_THERMALCAL_VC*)pAnnex42)->typeHdr.LengthInByte,
								&NextoffsetInMem, 
								&usedSize, &totalSize,
#ifdef _FLASH_
								false,
#endif //#ifdef _FLASH_
								FlexFileNameNoE2prom); 
	}

	if(pAnnex45)  
	{ 
		ANNEX_TYPE_45_SLEEPMODE_VC *pAnnex=NULL;
		
		pAnnex = (ANNEX_TYPE_45_SLEEPMODE_VC*) (pAnnex45);

		status = SetAnnexTblInfo(deviceType, pObj, 
								(ANNEX_TYPE_HEADER_VC*)pAnnex, 
								sizeof(ANNEX_TYPE_45_SLEEPMODE_VC),
								&NextoffsetInMem, 
								&usedSize, &totalSize,
#ifdef _FLASH_
								false,
#endif //#ifdef _FLASH_
								FlexFileNameNoE2prom);
	}

 	for (i=0; i<MAXNUM_MIMODEVICE ; i++)
	{
		for (j=0; j<MAXNUM_TXPATH ; j++)
		{
            // Annex 47
			if(pAnnex47[i][j])  
			{ 
				ANNEX_TYPE_31_47_RSSI_G_VC *pAnnex=NULL;
				
				pAnnex = (ANNEX_TYPE_31_47_RSSI_G_VC*) (pAnnex47[i][j]);

				status = SetAnnexTblInfo(deviceType, pObj, 
								(ANNEX_TYPE_HEADER_VC*)pAnnex, 
								sizeof(ANNEX_TYPE_31_47_RSSI_G_VC),
								&NextoffsetInMem, 
								&usedSize, &totalSize,
#ifdef _FLASH_
								false,
#endif //#ifdef _FLASH_
								FlexFileNameNoE2prom);
			}

			// Annex 48
			if(pAnnex48[i][j])  
			{ 
				ANNEX_TYPE_32_48_RSSI_A_VC *pAnnex=NULL;
				
				pAnnex = (ANNEX_TYPE_32_48_RSSI_A_VC*) (pAnnex48[i][j]);

				status = SetAnnexTblInfo(deviceType, pObj, 
								(ANNEX_TYPE_HEADER_VC*)pAnnex, 
								sizeof(ANNEX_TYPE_32_48_RSSI_A_VC),
								&NextoffsetInMem, 
								&usedSize, &totalSize,
#ifdef _FLASH_
								true,
#endif //#ifdef _FLASH_
								FlexFileNameNoE2prom);

			}

			//Annex 49
			if(pAnnex49[i][j])  
			{ 
				ANNEX_TYPE_49_2G_POWERTABLE_VD *pAnnex=NULL;
				
				pAnnex = (ANNEX_TYPE_49_2G_POWERTABLE_VD*) (pAnnex49[i][j]);
				
				status = SetAnnexTblInfo(deviceType, pObj, 
								(ANNEX_TYPE_HEADER_VC*)pAnnex, 
								sizeof(ANNEX_TYPE_49_2G_POWERTABLE_VD),
								&NextoffsetInMem, 
								&usedSize, &totalSize,
#ifdef _FLASH_
								false,
#endif //#ifdef _FLASH_
								FlexFileNameNoE2prom);
			}
			
			for (k=0; k<NUM_SUBBAND_A ; k++)
			{
				if(pAnnex50[i][j][k])  
				{ 
					ANNEX_TYPE_50_5G_POWERTABLE_VD *pAnnex=NULL;
					
					pAnnex = (ANNEX_TYPE_50_5G_POWERTABLE_VD*) (pAnnex50[i][j][k]);

					status = SetAnnexTblInfo(deviceType, pObj, 
								(ANNEX_TYPE_HEADER_VC*)pAnnex, 
								sizeof(ANNEX_TYPE_50_5G_POWERTABLE_VD),
								&NextoffsetInMem, 
								&usedSize, &totalSize,
#ifdef _FLASH_
								false,
#endif //#ifdef _FLASH_
								FlexFileNameNoE2prom);
				}
			}
			
			//Annex 52
			if(pAnnex52[i][j])  
			{ 
				ANNEX_TYPE_52_LOLEAKAGE_VD *pAnnex=NULL;
				
				pAnnex = (ANNEX_TYPE_52_LOLEAKAGE_VD*) (pAnnex52[i][j]);

				status = SetAnnexTblInfo(deviceType, pObj, 
								(ANNEX_TYPE_HEADER_VC*)pAnnex, 
								sizeof(ANNEX_TYPE_52_LOLEAKAGE_VD),
								&NextoffsetInMem, 
								&usedSize, &totalSize,
#ifdef _FLASH_
								true,
#endif //#ifdef _FLASH_
								FlexFileNameNoE2prom);
			}

			//Annex 53
			if(pAnnex53[i][j])  
			{ 
				ANNEX_TYPE_53_TX_IQ_LOLEAKAGE_VD *pAnnex=NULL;
				
				pAnnex = (ANNEX_TYPE_53_TX_IQ_LOLEAKAGE_VD*) (pAnnex53[i][j]);

				status = SetAnnexTblInfo(deviceType, pObj, 
								(ANNEX_TYPE_HEADER_VC*)pAnnex, 
								sizeof(ANNEX_TYPE_53_TX_IQ_LOLEAKAGE_VD),
								&NextoffsetInMem, 
								&usedSize, &totalSize,
#ifdef _FLASH_
								true,
#endif //#ifdef _FLASH_
								FlexFileNameNoE2prom);
			}

			//Annex 54
			if(pAnnex54[i][j])  
			{ 
				ANNEX_TYPE_54_RX_IQ_LOLEAKAGE_VD *pAnnex=NULL;
				
				pAnnex = (ANNEX_TYPE_54_RX_IQ_LOLEAKAGE_VD*) (pAnnex54[i][j]);

				status = SetAnnexTblInfo(deviceType, pObj, 
								(ANNEX_TYPE_HEADER_VC*)pAnnex, 
								sizeof(ANNEX_TYPE_54_RX_IQ_LOLEAKAGE_VD),
								&NextoffsetInMem, 
								&usedSize, &totalSize,
#ifdef _FLASH_
								true,
#endif //#ifdef _FLASH_
								FlexFileNameNoE2prom);
			}
			//Annex 57
			for(int k=0; k<NUM_SUBBAND_A_REVD+NUM_SUBBAND_G_REVD;k++)
			{
				if(pAnnex57[i][j][k])  
				{	 
					ANNEX_TYPE_57_DPD_DATA *pAnnex=NULL;
			
				pAnnex = (ANNEX_TYPE_57_DPD_DATA*) (pAnnex57[i][j][k]);

				status = SetAnnexTblInfo(deviceType, pObj, 
										(ANNEX_TYPE_HEADER_VC*)pAnnex, 
										((ANNEX_TYPE_57_DPD_DATA*)pAnnex57[i][j][k])->typeHdr.LengthInByte,
										&NextoffsetInMem, 
										&usedSize, &totalSize,
	#ifdef _FLASH_
										false,
	#endif //#ifdef _FLASH_
										FlexFileNameNoE2prom); 
				}
			} //for (k=0
		}  // for (j=0
	}  // for (i=0

	//Annex55
			if(pAnnex55)  
			{ 
				ANNEX_TYPE_55_BT_CONFIGURE *pAnnex=NULL;
				
				pAnnex = (ANNEX_TYPE_55_BT_CONFIGURE*) (pAnnex55);

				status = SetAnnexTblInfo(deviceType, pObj, 
								(ANNEX_TYPE_HEADER_VC*)pAnnex, 
								sizeof(ANNEX_TYPE_55_BT_CONFIGURE),
								&NextoffsetInMem, 
								&usedSize, &totalSize,
#ifdef _FLASH_
								true,
#endif //#ifdef _FLASH_
								FlexFileNameNoE2prom);

			}
//Annex56---------------------------------------------
//Annex56
			if(pAnnex56)  
			{ 
				ANNEX_TYPE_56_BT_SECURITY_DATA *pAnnex=NULL;
				
				pAnnex = (ANNEX_TYPE_56_BT_SECURITY_DATA*) (pAnnex56);

				status = SetAnnexTblInfo(deviceType, pObj, 
								(ANNEX_TYPE_HEADER_VC*)pAnnex, 
								sizeof(ANNEX_TYPE_56_BT_SECURITY_DATA),
								&NextoffsetInMem, 
								&usedSize, &totalSize,
#ifdef _FLASH_
								true,
#endif //#ifdef _FLASH_
								FlexFileNameNoE2prom);

			}

	if(PurgeAfter) PurgeAnnexPointers();

	return status; 
}

int DutSharedClssHeader UpdateCalDataRevC_Bt(int deviceType, void *pObj, 
											 int PurgeAfter, char *FlexFileNameNoE2prom) 
{
	int status=0;
 
	int i=0, j=0, k=0;
	int sizeOfTblInByte=0, offsetInMem=0, NextoffsetInMem=0; 
	int totalSize=0, usedSize=0;
	int offsetInMemAnnex_Next=0; 
	int endOfCal=ANNEX_ADDR_END, UsedSpace=0;
	int MaxAddrInEeprom =0x3FF; 

	DWORD BtAnnexAddress=ANNEX_ADDR_END; 
	int BtAnnexType=0, BtAnnexSize=0;
	DWORD ExistEndOfCal=ANNEX_ADDR_END; 
	DWORD ExistLastAnnexAddress=ANNEX_ADDR_END;
	int	ExistLastAnnexType=0;
	BYTE structVer=STRUCTUREREV;
 
	if(FlexFileNameNoE2prom)
	{
		if(deviceType)
			strcpy(g_NoEepromBtFlexFileName, FlexFileNameNoE2prom);
		else
			strcpy(g_NoEepromWlanFlexFileName, FlexFileNameNoE2prom);
	}

	if(!g_NoEeprom)
	{
#if !defined (_FLASH_) 
		{
		status = GetEepromSize(deviceType, pObj, NULL, &MaxAddrInEeprom);
		if(status) return status;
	
		if(0 == MaxAddrInEeprom) 
			return ERROR_SPACENOTAVAILABLE;

		if(DeBugLog) 
			DebugLogRite("MaxAddrInEeprom 0x%X\n", MaxAddrInEeprom);
	}
#else // #if !defined (_FLASH_) 

	{
		MaxAddrInEeprom = 0x7FF;
	}
#endif //#if !defined (_FLASH_) 

		if(DeBugLog) 
			DebugLogRite("MaxAddrInEeprom 0x%X\n", MaxAddrInEeprom);

	 
		// find the end of 
		status = FindLastAnnex(deviceType, pObj, 
					&ExistLastAnnexAddress, 
					&ExistLastAnnexType, 
					  &ExistEndOfCal);
		if(status) return status;

		status = FindBtAnnex(deviceType, pObj, 
							&BtAnnexAddress, &BtAnnexType, &BtAnnexSize);
		if(status) return status;

		if(DeBugLog) 
		{
			DebugLogRite("BtAnnexAddress = %X \n",BtAnnexAddress);
			DebugLogRite("BtAnnexType = %X \n",BtAnnexType);
			DebugLogRite("BtAnnexSize = %X \n",BtAnnexSize);
		}
	// if there is a BT, right version
	//	just update it
	// if there are WLAN but no BT, 
	//	change the last's header and add BT
	// if nothing, add main and BT
 		if(ANNEX_TYPE_BT_CAL == BtAnnexType)
		{
#ifndef _FLASH_ //flash can not updateAnnexHeader
	// if there is a BT, right version
	//	just update it
		if(DeBugLog) 
			DebugLogRite("if there is a BT, right version, just update it\n");

			status =GetNextAnnexPtr(deviceType, pObj, 
									BtAnnexAddress, &NextoffsetInMem); 
			if(status) return status;
 
			status = UpdateBtAnnex(deviceType, pObj, 
								BtAnnexAddress, NextoffsetInMem, PurgeAfter);
			if(status) return status;
#else //#ifndef _FLASH_ //flash can not updateAnnexHeader
			DebugLogRite("There are BT data in flash already. Will not do anything. \n");
#endif //#ifndef _FLASH_ //flash can not updateAnnexHeader
		}
	
		if( (ANNEX_TYPE_RESERVED != ExistLastAnnexType || ANNEX_ADDR_END != ExistLastAnnexAddress)
			&& 
			ANNEX_TYPE_RESERVED == BtAnnexType)
		{
	// if there are WLAN but no BT, 
	//	change the last's header and add BT
		if(DeBugLog) 
			DebugLogRite("if there are WLAN but no BT, change the last's header and add BT\n");

		if(ANNEX_ADDR_END == ExistEndOfCal) 		
			return ERROR_SPACENOTAVAILABLE;
#ifndef _FLASH_ //flash can not updateAnnexHeader
		status = UpdateAnnexHeader(deviceType, pObj, 
							ExistLastAnnexAddress, ExistEndOfCal);
			if(DeBugLog) 
				DebugLogRite("UpdateAnnexHeader %d\n", status);
		if(status) return status;
#endif
		if(DeBugLog) 
		{
			DebugLogRite("ExistLastAnnexAddress 0x%x\n", ExistLastAnnexAddress);
			DebugLogRite("ExistEndOfCal 0x%x\n", ExistEndOfCal);
			DebugLogRite("after ExistLastAnnexAddress 0x%x\n", ExistLastAnnexAddress);
			DebugLogRite("after ExistEndOfCal 0x%x\n", ExistEndOfCal);
		}
		status = AddBtAnnex(deviceType, pObj, 
							ExistEndOfCal,PurgeAfter);
//			if(debugLog) 
//				DebugLogRite("AddBtAnnex %d\n", status);
		if(status) return status;
		
		if(g_NoEeprom)
		{
			if(deviceType)
			{
				status = UpDateLenCalDataFile(g_NoEepromBtFlexFileName, 
											sizeof(ANNEX_TYPE_43_BTCAL_VC)+sizeof(ANNEX_TYPE_44_BTCFG_VC));
			}
			else
			{
				status = UpDateLenCalDataFile(g_NoEepromWlanFlexFileName, 
											sizeof(ANNEX_TYPE_43_BTCAL_VC)+sizeof(ANNEX_TYPE_44_BTCFG_VC));
			}
			if(DeBugLog) 
				DebugLogRite("UpDateLenCalDataFile 43 %d\n", status);
			if(status) return status;
			}
		}
	}

	if( ANNEX_TYPE_RESERVED == ExistLastAnnexType && 
		ANNEX_ADDR_END == ExistLastAnnexAddress&& 
		ANNEX_TYPE_RESERVED == BtAnnexType)
	{
// if nothing, add main and BT
		if(DeBugLog) 
			DebugLogRite("if nothing, add main and BT\n");
 		
		totalSize = sizeof(ANNEX_TYPE_55_BT_CONFIGURE)+sizeof(ANNEX_TYPE_56_BT_SECURITY_DATA);

	    status  = GetNextOpenSpace(deviceType, pObj, MaxAddrInEeprom,  0,
					 totalSize, &offsetInMemAnnex_Next);
    	if(status) return status;
		
	    if(ANNEX_ADDR_END == offsetInMemAnnex_Next) 		
		    return ERROR_SPACENOTAVAILABLE;

 		if(NULL == pMain)
		{
			status = setCalMainDataRevD(&structVer);
			if(status) return status;
		}

		if(pMain)	 
		{ 
			CardSpecData *pAnnex=NULL;
					
			pAnnex = (CardSpecData*) (pMain);

			NextoffsetInMem = offsetInMemAnnex_Next; 
			sizeOfTblInByte = sizeof(CardSpecData);
 			pAnnex->StructureRevision = STRUCTUREREV;
  			pAnnex->LengthInByte = sizeof(CardSpecData);
			pAnnex->PtrFirstAnnex= NextoffsetInMem; 
			pAnnex->Cs = 0; 
			pAnnex->Cs = DoCs1((BYTE*)(pAnnex),   
									sizeof(CardSpecData)); 
				
 			status = setCalMainTbl(deviceType, pObj, 
									(DWORD*)pAnnex, 
									(DWORD*)&sizeOfTblInByte);
			NextoffsetInMem = offsetInMemAnnex_Next; 
		}
		
 		if(pAnnex45)   
		{ 
			ANNEX_TYPE_45_SLEEPMODE_VC *pAnnex=NULL;
			
			pAnnex = (ANNEX_TYPE_45_SLEEPMODE_VC*) (pAnnex45);

			sizeOfTblInByte = sizeof(ANNEX_TYPE_45_SLEEPMODE_VC);
			SetAnnexTblInfo(deviceType, pObj,
							(ANNEX_TYPE_HEADER_VC	*)pAnnex,
							sizeOfTblInByte, 
							(int*)&NextoffsetInMem, 
							NULL, NULL, 
	#ifdef _FLASH_
							false,
	#endif //#ifdef _FLASH_
							NULL);
		} // end of annex 45

		status = AddBtAnnex(deviceType, pObj, NextoffsetInMem, PurgeAfter);
		if(status) return status;

		if(g_NoEeprom)
		{
			if(deviceType)
			{
				status = UpDateLenCalDataFile(g_NoEepromBtFlexFileName, 
									sizeof(CardSpecData)+sizeof(ANNEX_TYPE_43_BTCAL_VC)+sizeof(ANNEX_TYPE_44_BTCFG_VC));
			}
			else
			{
				status = UpDateLenCalDataFile(g_NoEepromWlanFlexFileName, 
									sizeof(CardSpecData)+sizeof(ANNEX_TYPE_43_BTCAL_VC)+sizeof(ANNEX_TYPE_44_BTCFG_VC));
			}
			if(status) return status;
		}
	}

	if(PurgeAfter) PurgeAnnexPointers();

	return status; 

}

int DutSharedClssHeader GetMainAnnexTbl(int deviceType, void *pObj, 
										int *sizeOfTblInByte, int offsetInMem, 
								DWORD* tbl, int *CsC)
{
  	DWORD offset=0,  *AddArray=NULL;
 	DWORD len=0; 
	int status=0;
	
	if(DeBugLog)
		DebugLogRite("### Get Annex Table from 0x%08X  ###\n", offsetInMem );

	if(g_NoEeprom)
	{
		if(deviceType)
		{
		status = ReadCalDataFile(g_NoEepromBtFlexFileName, offsetInMem,  
									(*sizeOfTblInByte)/sizeof(DWORD),  
									tbl );
		}
		else
		{
		status = ReadCalDataFile(g_NoEepromWlanFlexFileName, offsetInMem,  
									(*sizeOfTblInByte)/sizeof(DWORD),  
									tbl );
		}
	}
	else
	{
		AddArray = (DWORD*) malloc((*sizeOfTblInByte)); 

		status = DumpE2PData(deviceType, pObj, 
							offsetInMem,  (*sizeOfTblInByte)/sizeof(DWORD),  
							AddArray, tbl ); 
		free(AddArray); 
	}
	if (status) return status; 

	DoCs1Check((BYTE*)tbl, *sizeOfTblInByte, CsC);

	if(DeBugLog)
		DebugLogRite("### Annex Table  CsC = %d ###\n", *CsC);

	return status; 

}

int DutSharedClssHeader SetMainAnnexTbl(int deviceType, void*pObj, 
										int *sizeOfTblInByte, int offsetInMem, 
										DWORD* tbl)
{
	int status=0;

	if(DeBugLog)
		DebugLogRite("### Set Annex Table %d to 0x%08X ###\n", 
			((ANNEX_TYPE_HEADER_VC*)(tbl))->AnnexType, offsetInMem);

	if (g_NoEeprom)
	{
		if(deviceType)
		{
			status = WriteCalDataFile(g_NoEepromBtFlexFileName, offsetInMem, 
							(*sizeOfTblInByte)/sizeof(DWORD),  tbl);
		}
		else
		{
			status = WriteCalDataFile(g_NoEepromWlanFlexFileName, offsetInMem, 
							(*sizeOfTblInByte)/sizeof(DWORD),  tbl);
		}
	}
	else
		status = WriteLumpedData(deviceType, pObj, offsetInMem, 
						(*sizeOfTblInByte)/sizeof(DWORD),  tbl );

	return status;
}

int DutSharedClssHeader GetNextOpenSpace(int deviceType, void*pObj, 
										 int PartMaxAddress, bool includeCal, int sizeOfTblInByte, int* pointer)
{
	DWORD HeaderTblData[VMR_HEADER_LENGTH/sizeof(DWORD)]={0};
	DWORD Version=0; 
	int CsC=0; 
	DWORD EndOfCIS=0, EndOfBoot=0, EndOfCal=0, EndOfAnnex=0;
	DWORD OpenPtr=0, nextPtr=0, address=0, sizeOfTbl=0, type=0 ; 
	CardSpecData CalTable={0}; 
 	DWORD len=sizeof(CardSpecData);  
	int 	ptr=0, status=0;

	if(g_NoEeprom)
	{
		OpenPtr = sizeof(CardSpecData);
 	}
	else
	{
	// this function is needed with add a new annex, 
// Get open space: the open space is after CIS or Boot. 
// Get the pointer of the end of CIS/ boot/ CalMain from header. 
// the end of them. the end of the last annex. 
// 	DebugLogRite("### Get Open Space ###\n");

	status=GetHeaderTbl(deviceType,  pObj, HeaderTblData, &Version, &CsC);
	if( status) return status;
	// CIS length, pointer
	if(HeaderTblData[6]!=0xFFFFFFFF)
		EndOfCIS = (HeaderTblData[5]&0xFFFF) + HeaderTblData[6]; //0x14, 0x18
	else
		EndOfCIS=VMR_HEADER_LENGTH;

	// Boot length pointer
#if !defined (_FLASH_) 
	if(HeaderTblData[2] !=0xFFFFFFFF) 
		EndOfBoot = (HeaderTblData[1]&0xFFFF) + HeaderTblData[2]; //0x04, 0x08
	else
		EndOfBoot=VMR_HEADER_LENGTH;
#else // #if !defined (_FLASH_) 

	 EndOfBoot = 0x0;// EndofBoot .....
#endif // #if !defined (_FLASH_) 

// Cal length pointer
	if(HeaderTblData[18] !=0xFFFFFFFF)
		EndOfCal = (HeaderTblData[19]&0xFFFF) + HeaderTblData[18]; //0x44/4, 0x48/4
	else
		EndOfCal=VMR_HEADER_LENGTH;

	// add annexes

//	if(STRUCTUREREV_A == Version) 
//		getEndOfAnnex(&EndOfAnnex); 
//	else
		EndOfAnnex =0;
  
	// the max value is the open space's location
	// the max of EndOfCIS, EndOfBoot, EndOfCal, EndOfAnnex; 
	OpenPtr= EndOfCIS;
	if(EndOfBoot> OpenPtr) OpenPtr = EndOfBoot;
	if(EndOfCal> OpenPtr) OpenPtr = EndOfCal;
	if(includeCal) 
	{
		if(EndOfAnnex> OpenPtr) 
			OpenPtr = EndOfAnnex;
	}

	if( (OpenPtr + (DWORD)sizeOfTblInByte) > ((DWORD)PartMaxAddress+1)) 
	{
		*pointer = 0;
		return ERROR_SPACENOTAVAILABLE;
	}
	}

   *pointer = OpenPtr;
		
   return status;

}

int DutSharedClssHeader FindLastAnnex(int deviceType, void*pObj, 
									  DWORD* pLastAnnexPointer, int *pAnnexType, DWORD* pEndOfAnnexPointer)
{
	int ptr=0, nextPtr=0, sizeOfTbl=0, type=0;
	int status=0;
	int EndOfAnnex=ANNEX_ADDR_END; 
	int DeviceId=0,  PathId=0,  BandId=0, SubBandId=0;

 // get Cal Main. if not programed, no annexes, if programmed, get last annex
	status = GetFirstAnnexInfo(	deviceType, pObj, 
								&ptr, &sizeOfTbl, &type, 
									&DeviceId,  &PathId,  
									&BandId, &SubBandId);
 	if(ANNEX_ADDR_END != ptr && ANNEX_ADDR_BLANK != ptr)// && -1 == sizeOfTbl)
	{
		
		if(ANNEX_LENGTH_INVALID == (WORD)sizeOfTbl)
		{
			EndOfAnnex = (ptr);
		}
		else
		{
			EndOfAnnex = (ptr + sizeOfTbl);
			do
			{
				// get next pointer
				//status =Spi_DumpData(ptr+4, 1, (DWORD*)&address,  (DWORD*)&nextPtr);
				status =GetNextAnnexPtr(deviceType, pObj, 
								ptr, &nextPtr); 
				if(status) return status;

				if(ANNEX_ADDR_END == nextPtr || ANNEX_ADDR_BLANK == nextPtr) break;

				GetAnnexInfo(	deviceType, pObj, 
								nextPtr, &sizeOfTbl, &type, 
										&DeviceId,  &PathId,  
										&BandId, &SubBandId); 

				if(sizeOfTbl == 0xFFFF) 
				{
					EndOfAnnex = nextPtr;
				}
				else
				{
					EndOfAnnex = (nextPtr + sizeOfTbl);
				}
				ptr = nextPtr;
				if(DeBugLog)
					DebugLogRite("nextPtr = %X \n",nextPtr);
			}while (1);
		}
	}

	if(pLastAnnexPointer) *pLastAnnexPointer = ptr; 
	if(pAnnexType) *pAnnexType = type;
	if(pEndOfAnnexPointer) *pEndOfAnnexPointer = EndOfAnnex;
	
	if(DeBugLog)
	{
		DebugLogRite("pLastAnnexPointer = %X \n",*pLastAnnexPointer);
		DebugLogRite("pAnnexType = %X \n",*pAnnexType);
		DebugLogRite("pEndOfAnnexPointer = %X \n",*pEndOfAnnexPointer);
	}
	return status;
}

int DutSharedClssHeader FindBtAnnex(int deviceType, void*pObj, 
									DWORD* pointer, int *pAnnexType, int *pAnnexSize)
{
	int ptr=0, nextPtr=0, sizeOfTbl=0, type=0;
	int status=0;
//	int EndOfAnnex=ANNEX_ADDR_END; 
	int DeviceId=0,  PathId=0,  BandId=0, SubBandId=0;

	if(pointer) *pointer = ANNEX_ADDR_END; 
	if(pAnnexType) *pAnnexType = ANNEX_TYPE_RESERVED;
	if(pAnnexSize) *pAnnexSize = 0;

 // get Cal Main. if not programed, no annexes, if programmed, get last annex
	status = GetFirstAnnexInfo(deviceType, pObj, 
								&ptr, &sizeOfTbl, &type, 
									&DeviceId,  &PathId,  
									&BandId, &SubBandId);
 	if(ANNEX_ADDR_END != ptr && ANNEX_ADDR_BLANK != ptr)
	{
			if(ANNEX_TYPE_BT_CAL == type) 
			{
				if(pointer) *pointer = ptr; 
				if(pAnnexType) *pAnnexType = type;
				if(pAnnexSize) *pAnnexSize = sizeOfTbl;
//				break;
			}
//		EndOfAnnex = (ptr + sizeOfTbl);
		do
		{
			// get next pointer
			//status =Spi_DumpData(ptr+4, 1, (DWORD*)&address,  (DWORD*)&nextPtr);
			status =GetNextAnnexPtr(deviceType, pObj, ptr, &nextPtr); 
			if(status) return status;
			if(ANNEX_ADDR_END == nextPtr || ANNEX_ADDR_BLANK == nextPtr) break;
			GetAnnexInfo(deviceType, pObj, 
							nextPtr, &sizeOfTbl, &type, 
									&DeviceId,  &PathId,  
									&BandId, &SubBandId); 
			if(ANNEX_TYPE_BT_CAL == type) 
			{
				if(pointer) *pointer = nextPtr; 
				if(pAnnexType) *pAnnexType = type;
				if(pAnnexSize) *pAnnexSize = sizeOfTbl;
				break;
			}
//			EndOfAnnex = (nextPtr + sizeOfTbl);
			ptr = nextPtr;
//			DebugLogRite("BT nextPtr = %X \n",nextPtr);
		}while (1);
	}

	return status;
}

int DutSharedClssHeader AddBtAnnex(int deviceType, void *pObj, 
								   DWORD offsetInMemAnnex, int PurgeAfter)
{
	int status=0;
	int sizeOfTblInByte=0;
	DWORD NextOffsetInMemAnnex=0;

 	if(pAnnex55)  
	{ 
		ANNEX_TYPE_55_BT_CONFIGURE *pAnnex=NULL;
		
		pAnnex = (ANNEX_TYPE_55_BT_CONFIGURE*) (pAnnex55);



 		if(pAnnex49[0][0]) 
			pAnnex->RFXtal = 
			((ANNEX_TYPE_49_2G_POWERTABLE_VD *)pAnnex49[0][0])->CalXtal;
		else if (pAnnex50[0][0]) 
			pAnnex->RFXtal = 
			((ANNEX_TYPE_50_5G_POWERTABLE_VD *)pAnnex50[0][0])->CalXtal;
 
		sizeOfTblInByte = sizeof(ANNEX_TYPE_55_BT_CONFIGURE);
		NextOffsetInMemAnnex =offsetInMemAnnex;
		SetAnnexTblInfo(deviceType, pObj,
						(ANNEX_TYPE_HEADER_VC*)pAnnex,
						sizeOfTblInByte, 
						(int*)&NextOffsetInMemAnnex, 
						NULL, NULL, 
#ifdef _FLASH_
						false,
#endif //#ifdef _FLASH_
						NULL); 
	}

 	if(pAnnex56)  
	{ 
		ANNEX_TYPE_56_BT_SECURITY_DATA *pAnnex=NULL;
		int nextType =0;
		int nextAnnexOffset=0;
		
// is the next annex56? 
		if( ANNEX_ADDR_END != NextOffsetInMemAnnex && 
			ANNEX_ADDR_BLANK != NextOffsetInMemAnnex)  
			status = GetAnnexInfo(deviceType, pObj, 
							NextOffsetInMemAnnex, NULL, &nextType, 
						NULL, NULL, NULL, NULL); 
		else 
			nextType = ANNEX_TYPE_RESERVED; // pend annex 56

		if(status) return status;

		if(ANNEX_TYPE_NUM56 == nextType)
			status = GetNextAnnexPtr(deviceType, pObj, 
									NextOffsetInMemAnnex, &nextAnnexOffset); 

		if(ANNEX_TYPE_NUM56 == nextType || 
			ANNEX_TYPE_RESERVED_LAST == nextType ||
			ANNEX_TYPE_RESERVED == nextType)
		{
		pAnnex = (ANNEX_TYPE_56_BT_SECURITY_DATA*) (pAnnex56);

		
		sizeOfTblInByte = sizeof(ANNEX_TYPE_56_BT_SECURITY_DATA);
		SetAnnexTblInfo(deviceType, pObj,
						(ANNEX_TYPE_HEADER_VC	*)pAnnex,
						sizeOfTblInByte, 
						(int*)&offsetInMemAnnex, 
						NULL, NULL, 
#ifdef _FLASH_
						true,
#endif //#ifdef _FLASH_
						NULL);
		}
	}
	if(PurgeAfter) PurgeAnnexPointers();
 
	return status; 
}

int DutSharedClssHeader UpdateBtAnnex(int deviceType, void*pObj, 
									  DWORD offsetInMemAnnex, 
									  DWORD NextOffsetInMemAnnex, 
									  int PurgeAfter)
{
	int status=0;
	int sizeOfTblInByte=0;
 	
	if(pAnnex55)  
	{ 
		ANNEX_TYPE_55_BT_CONFIGURE *pAnnex=NULL;
		
		pAnnex = (ANNEX_TYPE_55_BT_CONFIGURE*) (pAnnex55);

 		if(pAnnex49[0][0]) 
			pAnnex->RFXtal = 
			((ANNEX_TYPE_49_2G_POWERTABLE_VD *)pAnnex49[0][0])->CalXtal;
		else if (pAnnex50[0][0]) 
			pAnnex->RFXtal = 
			((ANNEX_TYPE_50_5G_POWERTABLE_VD *)pAnnex50[0][0])->CalXtal;
 
		sizeOfTblInByte = sizeof(ANNEX_TYPE_55_BT_CONFIGURE);
		SetAnnexTblInfo(deviceType, pObj,
						(ANNEX_TYPE_HEADER_VC	*)pAnnex,
						sizeOfTblInByte, 
						(int*)&offsetInMemAnnex, 
						NULL, NULL, 
#ifdef _FLASH_
						false,
#endif //#ifdef _FLASH_
						NULL);
	}

	if(pAnnex56)  
	{ 
		ANNEX_TYPE_56_BT_SECURITY_DATA *pAnnex=NULL;
		
		pAnnex = (ANNEX_TYPE_56_BT_SECURITY_DATA*) (pAnnex56);

		sizeOfTblInByte = sizeof(ANNEX_TYPE_56_BT_SECURITY_DATA);
		SetAnnexTblInfo(deviceType, pObj,
						(ANNEX_TYPE_HEADER_VC	*)pAnnex,
						sizeOfTblInByte, 
						(int*)&offsetInMemAnnex, 
						NULL, NULL, 
#ifdef _FLASH_
						false,
#endif //#ifdef _FLASH_
						NULL);
 
	}
	if(PurgeAfter) PurgeAnnexPointers();
 
	return status; 
}

int DutSharedClssHeader UpdateAnnexHeader(int deviceType, void*pObj, 
										  DWORD offsetInMemLastAnnex, 
										  DWORD NewOffsetInMemAnnex)
{
	int status=0;
	ANNEX_TYPE_HEADER_EXTENDED_VC Header={0};
	BYTE newAddrSum=0, oldAddrSum=0;
	DWORD oldPtr=0;

	// use pointer to get header infor for the annex
	if(g_NoEeprom)
	{
		if(deviceType)
		{
			status = ReadCalDataFile(g_NoEepromBtFlexFileName, offsetInMemLastAnnex, 
								sizeof(ANNEX_TYPE_HEADER_EXTENDED_VC)/sizeof(DWORD), 
								(DWORD*)&Header);
		}
		else
		{
			status = ReadCalDataFile(g_NoEepromWlanFlexFileName, offsetInMemLastAnnex, 
								sizeof(ANNEX_TYPE_HEADER_EXTENDED_VC)/sizeof(DWORD), 
								(DWORD*)&Header);
		}
	}
	else
	{
		status = DumpE2PData(deviceType, pObj, 
						offsetInMemLastAnnex, 
						sizeof(ANNEX_TYPE_HEADER_EXTENDED_VC)/sizeof(DWORD), 
						NULL, (DWORD*)&Header);
	}

	if(status) return status;


	newAddrSum = (BYTE)((NewOffsetInMemAnnex    )&0xFF)+(BYTE)((NewOffsetInMemAnnex>>8 )&0xFF)
				+(BYTE)((NewOffsetInMemAnnex>>16)&0xFF)+(BYTE)((NewOffsetInMemAnnex>>24)&0xFF);
	
	oldPtr = Header.typeHdr.PtrNextAnnex;

	oldAddrSum = (BYTE)((oldPtr    )&0xFF)+(BYTE)((oldPtr>>8 )&0xFF)
				+(BYTE)((oldPtr>>16)&0xFF)+(BYTE)((oldPtr>>24)&0xFF);

//	Header.typeHdr.Cs -= (NewOffsetInMemAnnex - Header.typeHdr.PtrNextAnnex);
	Header.typeHdr.Cs -= (newAddrSum - oldAddrSum);
	Header.typeHdr.PtrNextAnnex = NewOffsetInMemAnnex;

	if(deviceType)
	{
 		status = WriteCalDataFile(g_NoEepromBtFlexFileName, offsetInMemLastAnnex, 
 				sizeof(ANNEX_TYPE_HEADER_EXTENDED_VC)/sizeof(DWORD),  (DWORD*)&Header);
 	}
 	else
	{
		status = WriteCalDataFile(g_NoEepromWlanFlexFileName, offsetInMemLastAnnex, 
 				sizeof(ANNEX_TYPE_HEADER_EXTENDED_VC)/sizeof(DWORD),  (DWORD*)&Header);
	}
	if(status) return status;

	if(0 == g_NoEeprom)
	{
		status = WriteLumpedData(deviceType, pObj, offsetInMemLastAnnex, 
						sizeof(ANNEX_TYPE_HEADER_EXTENDED_VC)/sizeof(DWORD),  
						(DWORD*)&Header);
	}
	return status; 
}

int DutSharedClssHeader GetBdAddressFromE2P(int deviceType, void*pObj, 
											BYTE BdAddress[SIZE_BDADDRESS_INBYTE])
{ // download powertables. lna settings. RF/BB/SOC regs

	int status = 0, CsC=0, CsCValue=0, i=0, DeviceId=0, PathId=0;
	BYTE RfAddress[MAX_SETTING_PAIRS]={0}, Value[MAX_SETTING_PAIRS]={0}; 
 	WORD BbuAddress[MAX_SETTING_PAIRS]={0}; 
	DWORD SocAddress[MAX_SETTING_PAIRS]={0}, SocValue[MAX_SETTING_PAIRS]={0}; 
	int UseBbuSetting=0; 
	BYTE BbuLnaHi_G[NUM_SUBBAND_G]={0}, BbuLnaLo_G[NUM_SUBBAND_G]={0};
	BYTE BbuLnaGainOffset_G[NUM_SUBBAND_G]={0};
	BYTE RfLnaGrbb_G[NUM_SUBBAND_G]={0}, RfLnaGrif_G[NUM_SUBBAND_G]={0};
	BYTE BbuLnaHi_A[NUM_SUBBAND_A]={0}, BbuLnaLo_A[NUM_SUBBAND_A]={0};
	BYTE BbuLnaGainOffset_A[NUM_SUBBAND_A]={0};
	BYTE RfLnaGrbb_A[NUM_SUBBAND_A]={0}, RfLnaGrif_A[NUM_SUBBAND_A]={0};
	int CalXtal=0, InitPCommon=0, ThermScale=0; 
 
#ifdef GOLDENAPMIMODLL_EXPORTS
	if(NULL == pMain)
#endif // #ifdef GOLDENAPMIMODLL_EXPORTS 
	{
		status = GetCalDataRevC(deviceType, pObj, 
								&CsC, &CsCValue, NULL);  
		if(status)	return status;
		if(!CsC)	return ERROR_MISMATCH;
	}
	
	return status;
}

int DutSharedClssHeader LoadCustomizedSettings(int deviceType, void *pObj)
{ // download powertables. lna settings. RF/BB/SOC regs

	int status = 0, CsC=0, CsCValue=0, i=0, DeviceId=0, PathId=0;
	BYTE RfAddress[MAX_SETTING_PAIRS]={0}, Value[MAX_SETTING_PAIRS]={0}; 
 	WORD BbuAddress[MAX_SETTING_PAIRS]={0}; 
	DWORD SocAddress[MAX_SETTING_PAIRS]={0}, SocValue[MAX_SETTING_PAIRS]={0}; 
	int UseBbuSetting=0; 
	BYTE BbuLnaHi_G[NUM_SUBBAND_G]={0}, BbuLnaLo_G[NUM_SUBBAND_G]={0};
	BYTE BbuLnaGainOffset_G[NUM_SUBBAND_G]={0};
	BYTE RfLnaGrbb_G[NUM_SUBBAND_G]={0}, RfLnaGrif_G[NUM_SUBBAND_G]={0};
	BYTE BbuLnaHi_A[NUM_SUBBAND_A]={0}, BbuLnaLo_A[NUM_SUBBAND_A]={0};
	BYTE BbuLnaGainOffset_A[NUM_SUBBAND_A]={0};
	BYTE RfLnaGrbb_A[NUM_SUBBAND_A]={0}, RfLnaGrif_A[NUM_SUBBAND_A]={0};
	int CalXtal=0, InitPCommon=0, ThermScale=0; 
 

#ifdef GOLDENAPMIMODLL_EXPORTS
	if(NULL == pMain)
#endif // #ifdef GOLDENAPMIMODLL_EXPORTS 
	{
 		status = GetCalDataRevC(deviceType, pObj, &CsC, &CsCValue, NULL);  
		if(status)	return status;
		if(!CsC)	return ERROR_MISMATCH;
	}

#if 0
#ifndef _LINUX_
#if defined (_MIMO_)
	// CckPwrScaler
 	if(deviceType == 0)
 		((class DutWlanApiClss*)pObj)->Dvc_SetCckFltScaler(((CardSpecDataRev0C*)pMain)->MiscFlag.CckPwrScaler);

#endif //#if defined (_MIMO_)

#else // #ifndef _LINUX_

#if defined (_MIMO_)
	// CckPwrScaler
	if(deviceType == 0)
		Dvc_SetCckFltScaler(((CardSpecDataRev0C*)pMain)->MiscFlag.CckPwrScaler);
	
#endif //#if defined (_MIMO_)
#endif // #ifndef _LINUX_
#endif // #if 0


	// XTAL and Thermal 
 	for (DeviceId=0; DeviceId<MAXNUM_MIMODEVICE; DeviceId++)
		for (PathId=0; PathId<MAXNUM_TXPATH; PathId++)
		{
			char TxIQmismatch_Amp_Bw20[NUM_SUBBAND_G]={0};
			char TxIQmismatch_Phase_Bw20[NUM_SUBBAND_G]={0};
			char TxIQmismatch_Amp_Bw40[NUM_SUBBAND_G]={0};
			char TxIQmismatch_Phase_Bw40[NUM_SUBBAND_G]={0};
			char TxIQmismatch_Amp_Bw10[NUM_SUBBAND_G]={0};
			char TxIQmismatch_Phase_Bw10[NUM_SUBBAND_G]={0};
			char TxIQmismatch_Amp_Bw5[NUM_SUBBAND_G]={0};
			char TxIQmismatch_Phase_Bw5[NUM_SUBBAND_G]={0};
		
#if defined(_CAL_REV_D_)
			//revisit
#else
			status = GetAnnexType14Data(
#if defined (_MIMO_)
				DeviceId, PathId, 
#endif //#if defined (_MIMO_)
								NULL, //CaledRng 
								NULL, //CalOption
								&CalXtal, 
#if defined (_MIMO_)
								&InitPCommon, 
#endif //#if defined (_MIMO_)
								&ThermScale, 
								NULL, NULL, NULL, NULL,  
								// NumOfSavedCh ChNumList LcTankCalFlag LcTankCalValueList
 								NULL, NULL, // CorrPDetTh_B CorrInitTrgt_B
								// TxIq
								TxIQmismatch_Amp_Bw20, TxIQmismatch_Phase_Bw20, 
								TxIQmismatch_Amp_Bw10, TxIQmismatch_Phase_Bw10, 
								TxIQmismatch_Amp_Bw5,  TxIQmismatch_Phase_Bw5, 
								TxIQmismatch_Amp_Bw40, TxIQmismatch_Phase_Bw40);
#endif	//#if !defined(_CAL_REV_D_)
 
			if(ERROR_DATANOTEXIST == status)  
				status =ERROR_NONE;
			else
			{
 				if(status)	return status;
 
#ifndef _LINUX_
#if defined (_MIMO_)  

				if(deviceType == 0)
				{
// TBD_KY 					status = ((class DutWlanApiClss*)pObj)->SetRfPwrInitPCommon(BAND_802_11G, InitPCommon, DeviceId); 
					if(status)	return status;

// TBD_KY 					status = ((class DutWlanApiClss*)pObj)->SetRfXTal(true, CalXtal, DeviceId); 
					if(status)	return status;
// TBD_KY 					status = ((class DutWlanApiClss*)pObj)->SetPaThermalScaler(ThermScale, PathId, DeviceId);
					if(status)	return status;

#if 0 
//			TxIq
				for (i=0; i<NUM_SUBBAND_G; i++)
				{
					status = ((class DutWlanApiClss*)pObj)->DownloadTxIqTbl(BAND_802_11G, i, 
								CHANNEL_BW_11N_20MHZ, 
								TxIQmismatch_Amp_Bw20[i], 
								TxIQmismatch_Phase_Bw20[i], 
								PathId, 
								DeviceId); 
					if(status)	return status;
					status = ((class DutWlanApiClss*)pObj)->DownloadTxIqTbl(BAND_802_11G, i, 
								CHANNEL_BW_11N_40MHZ, 
								TxIQmismatch_Amp_Bw40[i], 
								TxIQmismatch_Phase_Bw40[i], 
								PathId, 
								DeviceId); 
					if(status)	return status;
					status = ((class DutWlanApiClss*)pObj)->DownloadTxIqTbl(BAND_802_11G, i, 
								CHANNEL_BW_11N_10MHZ, 
								TxIQmismatch_Amp_Bw10[i], 
								TxIQmismatch_Phase_Bw10[i], 
								PathId, 
								DeviceId); 
					if(status)	return status;
					status = ((class DutWlanApiClss*)pObj)->DownloadTxIqTbl(BAND_802_11G, i, 
								CHANNEL_BW_11N_05MHZ, 
								TxIQmismatch_Amp_Bw5[i], 
								TxIQmismatch_Phase_Bw5[i], 
								PathId, 
								DeviceId); 
					if(status)	return status;
				}
#endif //#if 0 
				}
#endif //#if defined (_MIMO_)
#else // #ifndef _LINUX_
#if defined (_MIMO_)  

				if(deviceType == 0)
				{
//tbd jsz
#if 0	
					status = SetRfPwrInitPCommon(BAND_802_11G, InitPCommon, DeviceId); 
					if(status)	return status;

					status = SetRfXTal(true, CalXtal, DeviceId); 
					if(status)	return status;
					status = SetPaThermalScaler(ThermScale, PathId, DeviceId);
					if(status)	return status;

//			TxIq
				for (i=0; i<NUM_SUBBAND_G; i++)
				{
					status = DownloadTxIqTbl(BAND_802_11G, i, 
								CHANNEL_BW_11N_20MHZ, 
								TxIQmismatch_Amp_Bw20[i], 
								TxIQmismatch_Phase_Bw20[i], 
								PathId, 
								DeviceId); 
					if(status)	return status;
					status = DownloadTxIqTbl(BAND_802_11G, i, 
								CHANNEL_BW_11N_40MHZ, 
								TxIQmismatch_Amp_Bw40[i], 
								TxIQmismatch_Phase_Bw40[i], 
								PathId, 
								DeviceId); 
					if(status)	return status;
					status = DownloadTxIqTbl(BAND_802_11G, i, 
								CHANNEL_BW_11N_10MHZ, 
								TxIQmismatch_Amp_Bw10[i], 
								TxIQmismatch_Phase_Bw10[i], 
								PathId, 
								DeviceId); 
					if(status)	return status;
					status = DownloadTxIqTbl(BAND_802_11G, i, 
								CHANNEL_BW_11N_05MHZ, 
								TxIQmismatch_Amp_Bw5[i], 
								TxIQmismatch_Phase_Bw5[i], 
								PathId, 
								DeviceId); 
					if(status)	return status;
				}
#endif
				}
#endif //#if defined (_MIMO_)

#endif // #ifndef _LINUX_

			}
		}
 
// 	// XTAL and Thermal 
 	for (DeviceId=0; DeviceId<MAXNUM_MIMODEVICE; DeviceId++)
		for (PathId=0; PathId<MAXNUM_TXPATH; PathId++)
		{
			char TxIQmismatch_Amp_Bw20[NUM_SUBBAND_A]={0};
			char TxIQmismatch_Phase_Bw20[NUM_SUBBAND_A]={0};
			char TxIQmismatch_Amp_Bw40[NUM_SUBBAND_A]={0};
			char TxIQmismatch_Phase_Bw40[NUM_SUBBAND_A]={0};
			char TxIQmismatch_Amp_Bw10[NUM_SUBBAND_A]={0};
			char TxIQmismatch_Phase_Bw10[NUM_SUBBAND_A]={0};
			char TxIQmismatch_Amp_Bw5[NUM_SUBBAND_A]={0};
			char TxIQmismatch_Phase_Bw5[NUM_SUBBAND_A]={0};
		
#if !defined(_CAL_REV_D_)
			status = GetAnnexType15Data(
#if defined (_MIMO_)
				DeviceId, PathId, 
#endif //#if defined (_MIMO_)
								NULL,		
								NULL,		&CalXtal, 
#if defined (_MIMO_)
								&InitPCommon, 
#endif //#if defined (_MIMO_)
								&ThermScale, 
								NULL, NULL, NULL, NULL, 
								NULL,  
								// TxIq
								TxIQmismatch_Amp_Bw20, TxIQmismatch_Phase_Bw20, 
								TxIQmismatch_Amp_Bw40, TxIQmismatch_Phase_Bw40, 
								TxIQmismatch_Amp_Bw10, TxIQmismatch_Phase_Bw10, 
								TxIQmismatch_Amp_Bw5,  TxIQmismatch_Phase_Bw5);
#endif	//#if !defined(_CAL_REV_D_)

			if(ERROR_DATANOTEXIST == status)  
				status =ERROR_NONE;
			else
			{
 				if(status)	return status;
 
#if 0// TBD_KY defined (_MIMO_)  
				status = SetRfPwrInitPCommon(BAND_802_11A, InitPCommon, DeviceId); 
				if(status)	return status;

				if(NULL == pAnnex14[DeviceId][PathId])
				{
					status = SetRfXTal(true, CalXtal, DeviceId); 
					if(status)	return status;
					status = SetPaThermalScaler(ThermScale, PathId, DeviceId);
					if(status)	return status;
				}
//			TxIq
				for (i=0; i<NUM_SUBBAND_A; i++)
				{
					status = DownloadTxIqTbl(BAND_802_11A, i, 
								CHANNEL_BW_11N_20MHZ, 
								TxIQmismatch_Amp_Bw20[i], 
								TxIQmismatch_Phase_Bw20[i], 
								PathId, 
								DeviceId); 
					if(status)	return status;
					status = DownloadTxIqTbl(BAND_802_11A, i, 
								CHANNEL_BW_11N_40MHZ, 
								TxIQmismatch_Amp_Bw40[i], 
								TxIQmismatch_Phase_Bw40[i], 
								PathId, 
								DeviceId); 
					if(status)	return status;
					status = DownloadTxIqTbl(BAND_802_11A, i, 
								CHANNEL_BW_11N_10MHZ, 
								TxIQmismatch_Amp_Bw10[i], 
								TxIQmismatch_Phase_Bw10[i], 
								PathId, 
								DeviceId); 
					if(status)	return status;
					status = DownloadTxIqTbl(BAND_802_11A, i, 
								CHANNEL_BW_11N_05MHZ, 
								TxIQmismatch_Amp_Bw5[i], 
								TxIQmismatch_Phase_Bw5[i], 
								PathId, 
								DeviceId); 
					if(status)	return status;
				}
#endif //#if defined (_MIMO_)

			}
		}
 
#if 0// TBD_KY defined (_MIMO_)


  // RF/BB/SOC settings
 	status = getAnnexType22Data(RfAddress, Value);
	if(ERROR_DATANOTEXIST == status)  
		status =ERROR_NONE;
	else
	{
 		if(status)	return status;
		// write rf pairs
		for (i=0; i<MAX_SETTING_PAIRS; i++)
		if(0xFF!=RfAddress[i] && 0xFF!=Value[i])
		{
			WriteRfReg(RfAddress[i], Value[i],MIMODEVICE0);
		}
	}
 

  	status = getAnnexType24Data( BbuAddress, Value);
	if(ERROR_DATANOTEXIST == status)  
		status =ERROR_NONE;
	else
	{
 		if(status)	return status;
		// write bbu pairs
		for (i=0; i<MAX_SETTING_PAIRS; i++)
		if(0xFF!=BbuAddress[i] && 0xFF!=Value[i])
		{
			WriteBBReg(BbuAddress[i], Value[i],MIMODEVICE0);
		}
	}

  	status = getAnnexType25Data( SocAddress, SocValue);
	if(ERROR_DATANOTEXIST == status)  
		status =ERROR_NONE;
	else
	{
 		if(status)	return status;
		// write soc pairs
		for (i=0; i<MAX_SETTING_PAIRS; i++)
		if(0xFF!=SocAddress[i] && 0xFF!=SocValue[i])
		{
			WriteSocReg(SocAddress[i], SocValue[i], MIMODEVICE0);
		}
	}

// LNA settings
	for (DeviceId=0; DeviceId<MAXNUM_MIMODEVICE; DeviceId++)
		for (PathId=0; PathId<MAXNUM_RXPATH; PathId++)
		{
			status = GetAnnexType17Data(DeviceId, PathId, &UseBbuSetting,
										BbuLnaHi_G, BbuLnaLo_G,	
										BbuLnaGainOffset_G,
										RfLnaGrbb_G, RfLnaGrif_G,
										BbuLnaHi_A, BbuLnaLo_A,	
										BbuLnaGainOffset_A,
										RfLnaGrbb_A, RfLnaGrif_A);
			if(ERROR_DATANOTEXIST == status)  
				status =ERROR_NONE;
			else
			{
 				if(status)	return status;
				for (i=0; i<NUM_SUBBAND_G; i++)
				{
					if(RfLnaGrif_G[i]!=0 && RfLnaGrbb_G[i]!=0)	
						status = DownLoadStaticSettingsToFw(BAND_802_11G, i, 
											PathId, 
 											RfLnaGrif_G[i],
 											RfLnaGrbb_G[i], 
											DeviceId);
 					if(status)	return status;
				}
				for (i=0; i<NUM_SUBBAND_A; i++)
				{
					if(RfLnaGrif_A[i]!=0 && RfLnaGrbb_A[i]!=0)	
						status = DownLoadStaticSettingsToFw(BAND_802_11A, i, 
											PathId, 
 											RfLnaGrif_A[i],
 											RfLnaGrbb_A[i], 
											DeviceId);
 					if(status)	return status;
				}
			}
		}


 	for (DeviceId=0; DeviceId<MAXNUM_MIMODEVICE; DeviceId++)
	{
		ResetRxPathGain(DeviceId);
	}

	// Rx IQ settings
	for (DeviceId=0; DeviceId<MAXNUM_MIMODEVICE; DeviceId++)
	{
		char RxIQmismatch_Amp_Bw20[MAXNUM_RXPATH][NUM_SUBBAND_G]={0};
		char RxIQmismatch_Phase_Bw20[MAXNUM_RXPATH][NUM_SUBBAND_G]={0};
		char RxIQmismatch_Amp_Bw40[MAXNUM_RXPATH][NUM_SUBBAND_G]={0};
		char RxIQmismatch_Phase_Bw40[MAXNUM_RXPATH][NUM_SUBBAND_G]={0};
		char RxIQmismatch_Amp_Bw10[MAXNUM_RXPATH][NUM_SUBBAND_G]={0};
		char RxIQmismatch_Phase_Bw10[MAXNUM_RXPATH][NUM_SUBBAND_G]={0};
		char RxIQmismatch_Amp_Bw5[MAXNUM_RXPATH][NUM_SUBBAND_G]={0};
		char RxIQmismatch_Phase_Bw5[MAXNUM_RXPATH][NUM_SUBBAND_G]={0};
		char RxGainMisM[MAXNUM_RXPATH][NUM_SUBBAND_G]={0};

		status = getAnnexType19Data(DeviceId, 
				RxIQmismatch_Amp_Bw20, RxIQmismatch_Phase_Bw20,
				RxIQmismatch_Amp_Bw40, RxIQmismatch_Phase_Bw40,
				RxIQmismatch_Amp_Bw10, RxIQmismatch_Phase_Bw10,
				RxIQmismatch_Amp_Bw5,  RxIQmismatch_Phase_Bw5,
				RxGainMisM);
		if(ERROR_DATANOTEXIST == status)  
			status =ERROR_NONE;
		else //if(ERROR_DATANOTEXIST == status)  
		{
 			if(status)	return status;
			for (PathId=0; PathId<MAXNUM_RXPATH; PathId++)
				for (i=0; i<NUM_SUBBAND_G; i++)
				{
					status = DownloadRxIqTbl(BAND_802_11G, i, 
											CHANNEL_BW_11N_20MHZ, 
											RxIQmismatch_Amp_Bw20[PathId][i], 
											RxIQmismatch_Phase_Bw20[PathId][i],
											PathId,  
											DeviceId);
 					if(status)	return status;
					status = DownloadRxIqTbl(BAND_802_11G, i, 
											CHANNEL_BW_11N_40MHZ, 
											RxIQmismatch_Amp_Bw40[PathId][i], 
											RxIQmismatch_Phase_Bw40[PathId][i],
											PathId,  
											DeviceId);
 					if(status)	return status;
					status = DownloadRxIqTbl(BAND_802_11G, i, 
											CHANNEL_BW_11N_10MHZ, 
											RxIQmismatch_Amp_Bw10[PathId][i], 
											RxIQmismatch_Phase_Bw10[PathId][i],
											PathId,  
											DeviceId);
 					if(status)	return status;
					status = DownloadRxIqTbl(BAND_802_11G, i, 
											CHANNEL_BW_11N_05MHZ, 
											RxIQmismatch_Amp_Bw5[PathId][i], 
											RxIQmismatch_Phase_Bw5[PathId][i],
											PathId,  
											DeviceId);
 					if(status)	return status;
					status = DownloadRxGainTbl(BAND_802_11G, i,  
											RxGainMisM[PathId][i],  
											PathId,  
											DeviceId);
 					if(status)	return status;
				}
				 
		} //if(ERROR_DATANOTEXIST == status)  
	} //DeviceId

	for (DeviceId=0; DeviceId<MAXNUM_MIMODEVICE; DeviceId++)
	{
		char RxIQmismatch_Amp_Bw20[MAXNUM_RXPATH][NUM_SUBBAND_A]={0};
		char RxIQmismatch_Phase_Bw20[MAXNUM_RXPATH][NUM_SUBBAND_A]={0};
		char RxIQmismatch_Amp_Bw40[MAXNUM_RXPATH][NUM_SUBBAND_A]={0};
		char RxIQmismatch_Phase_Bw40[MAXNUM_RXPATH][NUM_SUBBAND_A]={0};
		char RxIQmismatch_Amp_Bw10[MAXNUM_RXPATH][NUM_SUBBAND_A]={0};
		char RxIQmismatch_Phase_Bw10[MAXNUM_RXPATH][NUM_SUBBAND_A]={0};
		char RxIQmismatch_Amp_Bw5[MAXNUM_RXPATH][NUM_SUBBAND_A]={0};
		char RxIQmismatch_Phase_Bw5[MAXNUM_RXPATH][NUM_SUBBAND_A]={0};
		char RxGainMisM[MAXNUM_RXPATH][NUM_SUBBAND_A]={0};

		status = GetAnnexType21Data(DeviceId, 
				RxIQmismatch_Amp_Bw20, RxIQmismatch_Phase_Bw20,
				RxIQmismatch_Amp_Bw40, RxIQmismatch_Phase_Bw40,
				RxIQmismatch_Amp_Bw10, RxIQmismatch_Phase_Bw10,
				RxIQmismatch_Amp_Bw5,  RxIQmismatch_Phase_Bw5,
				RxGainMisM);
		if(ERROR_DATANOTEXIST == status)  
			status =ERROR_NONE;
		else //if(ERROR_DATANOTEXIST == status)  
		{
 			if(status)	return status;
			for (PathId=0; PathId<MAXNUM_RXPATH; PathId++)
				for (i=0; i<NUM_SUBBAND_A; i++)
				{
					status = DownloadRxIqTbl(BAND_802_11A, i, 
											CHANNEL_BW_11N_20MHZ, 
											RxIQmismatch_Amp_Bw20[PathId][i], 
											RxIQmismatch_Phase_Bw20[PathId][i],
											PathId,  
											DeviceId);
 					if(status)	return status;
					status = DownloadRxIqTbl(BAND_802_11A, i, 
											CHANNEL_BW_11N_40MHZ, 
											RxIQmismatch_Amp_Bw40[PathId][i], 
											RxIQmismatch_Phase_Bw40[PathId][i],
											PathId,  
											DeviceId);
 					if(status)	return status;
					status = DownloadRxIqTbl(BAND_802_11A, i, 
											CHANNEL_BW_11N_10MHZ, 
											RxIQmismatch_Amp_Bw10[PathId][i], 
											RxIQmismatch_Phase_Bw10[PathId][i],
											PathId,  
											DeviceId);
 					if(status)	return status;
					status = DownloadRxIqTbl(BAND_802_11A, i, 
											CHANNEL_BW_11N_05MHZ, 
											RxIQmismatch_Amp_Bw5[PathId][i], 
											RxIQmismatch_Phase_Bw5[PathId][i],
											PathId,  
											DeviceId);
 					if(status)	return status;
					status = DownloadRxGainTbl(BAND_802_11A, i,  
											RxGainMisM[PathId][i],  
											PathId,  
											DeviceId);
 					if(status)	return status;
				}
				 
		} //if(ERROR_DATANOTEXIST == status)  
	} //DeviceId


// power table
	for (DeviceId=0; DeviceId<MAXNUM_MIMODEVICE; DeviceId++)
		for (PathId=0; PathId<MAXNUM_TXPATH; PathId++)
		{
			int BandId=0, SubBandId=0, j;
			int HighFreqBondary=0, LowFreqBondary=0;
			int SizeofPwrT=0, sizeOfPwrTblInByte=0; 
			int PpaGain1_Rng[RFPWRRANGE_NUM]={0}, PpaGain2_Rng[RFPWRRANGE_NUM]={0};
			int MaxPwr_Rng[RFPWRRANGE_NUM]={0}, MinPwr_Rng[RFPWRRANGE_NUM]={0}; 

			BYTE Pwrlvl[200]={0}; 
			FW_POWRTABLE PwrTbl={0};//, *ptr=NULL; 
			 
			for (j= 0; j<(NUM_SUBBAND_G+NUM_SUBBAND_A); j++)
			{
				BandId = (j<NUM_SUBBAND_G)?BAND_802_11G:BAND_802_11A;
				SubBandId = (j<NUM_SUBBAND_G)?j:j-NUM_SUBBAND_G;
				status = getAnnexType16Data(DeviceId, PathId,  
						   BandId, SubBandId,  
						   &HighFreqBondary,  &LowFreqBondary,
						   PpaGain1_Rng, PpaGain2_Rng,  MaxPwr_Rng, MinPwr_Rng,  
						   &sizeOfPwrTblInByte, Pwrlvl);		
				if(ERROR_DATANOTEXIST == status)  
				{
		//			if(MIMODEVICE0 == DeviceId)
					{
					status = UpLoadPwrTable(BandId, SubBandId,  
											&SizeofPwrT, (BYTE*)&PwrTbl,  	
											PathId,	DeviceId);
//					if (status) return status;

					PwrTbl.DeviceId		= DeviceId;
					PwrTbl.PathId		= PathId;
					PwrTbl.BandId		= BandId;
					PwrTbl.SubBandId	= SubBandId;

					PwrTbl.pwrTbl.HighFreqBound = 0;
					PwrTbl.pwrTbl.LowFreqBound = 0; 
					
					SizeofPwrT = sizeof(FW_POWRTABLE);

					status = DownLoadPwrTable(BandId, SubBandId,  
											&SizeofPwrT, (BYTE*)&PwrTbl, 1,		
											PathId,	DeviceId);
//					if (status) return status;

					}
					status =ERROR_NONE;
				}
				else
				{
 					if(status)	return status;
				//	ptr = (POWRTABLE*)&PwrTbl;
					PwrTbl.DeviceId		= DeviceId;
					PwrTbl.PathId		= PathId;
					PwrTbl.BandId		= BandId;
					PwrTbl.SubBandId	= SubBandId;

					PwrTbl.pwrTbl.HighFreqBound = HighFreqBondary;
					PwrTbl.pwrTbl.LowFreqBound = LowFreqBondary;
					for (i=0; i<RFPWRRANGE_NUM; i++)
					{
						PwrTbl.pwrTbl.rngTblLevel[i].PpaGain5_1	= PpaGain1_Rng[i];
						PwrTbl.pwrTbl.rngTblLevel[i].PpaGain5_2	= PpaGain2_Rng[i];
						PwrTbl.pwrTbl.rngTblLevel[i].TopPwr		= MaxPwr_Rng[i];
						PwrTbl.pwrTbl.rngTblLevel[i].BtmPwr		= MinPwr_Rng[i];
					}
					memcpy(PwrTbl.pwrTbl.pwrTblLevel, Pwrlvl, sizeOfPwrTblInByte);
					
					SizeofPwrT = sizeof(FW_POWRTABLE);

					status = DownLoadPwrTable(BandId, SubBandId,  
											&SizeofPwrT, (BYTE*)&PwrTbl, 1,		
											PathId,	DeviceId);
					if (status) return status;

				}
			}

		}

#if 0  // no LO
		//   Lo table

	for (DeviceId=0; DeviceId<MAXNUM_MIMODEVICE; DeviceId++)
		for (PathId=0; PathId<MAXNUM_RXPATH; PathId++)
		{
			BYTE LoSpusCalSetting0[NUM_SUBBAND_A]={0};
			BYTE LoSpusCalSetting1[NUM_SUBBAND_A]={0};

			status = getAnnexType30Data(DeviceId, PathId, BAND_802_11A,  
				LoSpusCalSetting0, LoSpusCalSetting1);
			if(ERROR_DATANOTEXIST == status)  
				status =ERROR_NONE;
			else
			{
 				if(status)	return status;
			 
				for (i=0; i<NUM_SUBBAND_A; i++)
				{
					status = DownloadLoTbl(BAND_802_11A, i, 
 											LoSpusCalSetting0[i],
 											LoSpusCalSetting1[i], 
											PathId, 
											DeviceId);
 					if(status)	return status;
				}
			}
		}
#endif // #if 0 no LO
#endif //#if defined (_MIMO_)
 
 
	return status;
}



int DutSharedClssHeader GetUsbAttributes(int deviceType, void *pObj, 
										 BYTE *pmAttribute, BYTE *pMaxPower)
{	
	int status=ERROR_NONE;
	DWORD AddressArray=0, DataArray=0;; 
	int index=0; 
 
	status = DumpE2PData(deviceType, pObj, 0x60, 1,
						&AddressArray, 
						&DataArray);
	if(pmAttribute) (*pmAttribute) = (BYTE)((DataArray&0xFF000000)>>24);

	if(pMaxPower) (*pMaxPower) = (BYTE)((DataArray&0xFF0000)>>16);

	if(status) return status;
 
	return  status;
}


int DutSharedClssHeader SetUsbAttributes(int deviceType, void *pObj, 
										 BYTE Attribute, BYTE MaxPower)
{	
	int status=ERROR_NONE;
	DWORD AddressArray=0, DataArray=0;; 
	int index=0; 
 
	status = DumpE2PData(deviceType, pObj, 0x60, 1,
						&AddressArray, 
						&DataArray);
	if(status) return status;

	if (0xFFFF != (DataArray & 0xFFFF)) 
		return ERROR_SPACENOTAVAILABLE;

//	DataArray &= 0xFFFF0000;
//	DataArray |= (0xFF & Attribute);
//	DataArray |= ((0xFF & MaxPower) <<8);
	DataArray &= 0x0000FFFF;
	DataArray |= ((0xFF & Attribute) <<24);
	DataArray |= ((0xFF & MaxPower) <<16);
	

	status = WriteLumpedData(deviceType, pObj, 
							0x60,  1, 
							&DataArray);

	return  status;
}

#endif // #if 0

int DutSharedClssHeader getCalMainDataRevD(
						BYTE *pStructRev,
						BYTE *pDesignType,
						BYTE *pLNAGain,
						BYTE *pAntDiversityInfo,
						BYTE *FEVersion,
						//LNA option
						bool *pPaOption_ExtLnaFlag_PathABC_G,
						bool *pPaOption_ExtLnaFlag_PathABC_A,
						//PA option
						bool *pPaOption_ExtPaPolar_PathABC_G,
						bool *pPaOption_ExtPaPolar_PathABC_A,

						BYTE *pSpiSizeInfo,
 						BYTE *pCC,
						
						BYTE *pExtXtalSource,		
						BYTE *pExtSleepClk,	
						BYTE *pWlanWakeUp,

						BYTE pAntTx[MAXNUM_MIMODEVICE],		//
						BYTE pAntRx[MAXNUM_MIMODEVICE],		//

						//Capabilities
						bool *pCapable_2G, //
						bool *pCapable_5G, //
						bool *pInternalBluetooth,	//
						bool *pAntennaDiversity,
						bool *pLNA_2G,	//
						bool *pLNA_5G,
						bool *pSameAnt_2G_BT,
						bool *pSameAnt_5G_BT,
						
						//Concurrency
						BYTE *Capabilities_2G,
						BYTE *Capabilities_5G,

						BYTE  pBTFrontEnd2G[9],
						BYTE  pBTFrontEnd5G[9],

						DWORD *pTestToolVer,	DWORD *pMfgTaskVersion,	
						DWORD *pDllVersion, 
						BYTE *pSocOrRev, BYTE *pRfuOrRev,
						BYTE *pMimoTxCnt, 	BYTE *pMimoRxCnt, 
						BYTE pMimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH])
{
	int status=0, i=0, j=0, ChCnt=0;
 	CardSpecDataRev0D *ptr=NULL;
	// parse the data and pass it back to app. 
 
	if(NULL == pMain) return ERROR_DATANOTEXIST; 

	ptr = (CardSpecDataRev0D *)pMain;

   	if(	(0xFF != ptr->StructureRevision) && 
		(0x00 != ptr->StructureRevision) && 
		(STRUCTUREREV != ptr->StructureRevision) )
	{
		return ERROR_MISMATCH;
	}

	if(pStructRev)			(*pStructRev) = ptr->StructureRevision; 
	if(pDesignType)			(*pDesignType) = ptr->DesignType; 
 
	if(pAntTx)	
		for (i=0; i<=(MAXNUM_MIMODEVICE-1); i++)
			(pAntTx[i]) = ptr->AntCfg[i].TxAntCfg ;

 	if(pAntRx)	
		for (i=0; i<=(MAXNUM_MIMODEVICE-1); i++)
			(pAntRx[i]) = ptr->AntCfg[i].RxAntCfg;

	if(pSpiSizeInfo)	(*pSpiSizeInfo) = ptr->SpiSize.size; 



	if(pCC)				(*pCC) = ptr->CountryCode ;

	//FEM Capabilities
	if(pCapable_2G)		(*pCapable_2G) =	ptr->Capabilities.Capable_2G;
	if(pCapable_5G)		(*pCapable_5G) =	ptr->Capabilities.Capable_5G;
    if(pInternalBluetooth) 	
		(*pInternalBluetooth)=ptr->Capabilities.InternalBluetooth;

	if(pAntennaDiversity) 	
		(*pAntennaDiversity)=ptr->Capabilities.Antenna_Diversity;

	if(pLNA_2G) 	
		(*pLNA_2G)=ptr->Capabilities.LNA_2G;

	if(pLNA_5G) 	
		(*pLNA_5G)=ptr->Capabilities.LNA_5G;

	if(pSameAnt_2G_BT) 	
		(*pSameAnt_2G_BT)=ptr->Capabilities.SameAnt_2G_BT;

	if(pSameAnt_5G_BT) 	
		(*pSameAnt_5G_BT)=ptr->Capabilities.SameAnt_5G_BT;

				

	//Concurrency
	if(Capabilities_2G)
		(*Capabilities_2G)=ptr->Concurrency.Capabilities_2G;

	if(Capabilities_5G)
		(*Capabilities_5G)=ptr->Concurrency.Capabilities_5G;

	if(pExtXtalSource)	(*pExtXtalSource) = ptr->MiscFlag.ExtXtalSource;
	if(pExtSleepClk)	(*pExtSleepClk) = ptr->MiscFlag.ExtSleepClk;
	if(pWlanWakeUp)		(*pWlanWakeUp) = ptr->MiscFlag.WlanWakeUp;
	if(pLNAGain)		(*pLNAGain) = ptr->LNAGain;
	if(pAntDiversityInfo) (*pAntDiversityInfo) = ptr->AntDiversityInfo;

	if(FEVersion)		(*FEVersion) = ptr->FEVersion;
	//PA option
	if(pPaOption_ExtPaPolar_PathABC_G) 
		(*pPaOption_ExtPaPolar_PathABC_G) = ptr->PaOption.PaOption_ExtPaPolar_PathABC_G;

	if(pPaOption_ExtPaPolar_PathABC_A) 
		(*pPaOption_ExtPaPolar_PathABC_A) = ptr->PaOption.PaOption_ExtPaPolar_PathABC_A;

	if(pBTFrontEnd2G)
	{
		for (int i=0; i<9; i++)
			  pBTFrontEnd2G[i] =ptr->BTFrontEnd2G[i];
	}
	if(pBTFrontEnd5G)
	{
		for (int i=0; i<9; i++)
			 pBTFrontEnd5G[i] = ptr->BTFrontEnd5G[i];
	}

    if(pSocOrRev)       (*pSocOrRev) = ptr->Soc_OR_Rev;
	if(pRfuOrRev)       (*pRfuOrRev) = ptr->Rfu_OR_Rev;

 	if(pTestToolVer)	(*pTestToolVer) = ptr->TestVersion;
	if(pMfgTaskVersion)	(*pMfgTaskVersion) = ptr->MfgTaskVersion;
	if(pDllVersion)		(*pDllVersion) = ptr->DllVersion;

 	if(pMimoTxCnt)	(*pMimoTxCnt) = ptr->MimoTxCnt;
  	if(pMimoRxCnt)	(*pMimoRxCnt) = ptr->MimoRxCnt;

 	return status;
}

int DutSharedClssHeader setCalMainDataRevD( 
						BYTE *pStructRev,
						BYTE *pDesignType,
						BYTE *pSpiSizeInfo,
						BYTE *pLNAGain,
						BYTE *pAntDiversityInfo,
						BYTE *pFEVersion,
					    BYTE *pCC,
						
						BYTE *pExtXtalSource,		
						BYTE *pExtSleepClk,	
						BYTE *pWlanWakeUp,
					

						BYTE pAntTx[MAXNUM_MIMODEVICE],		//
						BYTE pAntRx[MAXNUM_MIMODEVICE],		//

						//Capabilities
						bool *pCapable_2G, //
						bool *pCapable_5G, //
						bool *pInternalBluetooth,	//
						bool *pAntennaDiversity,
						bool *pLNA_2G,	//
						bool *pLNA_5G,
						bool *pSameAnt_2G_BT,
						bool *pSameAnt_5G_BT,

						
						//Concurrency
						BYTE *pCapabilities_2G,
						BYTE *pCapabilities_5G,

						BYTE  pBTFrontEnd2G[9],
						BYTE  pBTFrontEnd5G[9],	
					
						DWORD *pTestToolVer,	DWORD *pMfgTaskVersion,	
						DWORD *pDllVersion, 
						BYTE *pSocOrRev, BYTE *pRfuOrRev,
						BYTE *pMimoTxCnt, 	BYTE *pMimoRxCnt, 
						BYTE pMimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]
						)
{
	int status=0, i=0;
 	CardSpecDataRev0D *ptr=NULL;
 
	// alloc space to pointer, fill structure, other than next pointer and csc
	
 	if(NULL == pMain)
	{
		pMain = malloc(sizeof(CardSpecData));
		memset(pMain, 0, sizeof(CardSpecData)); 
	}

 	ptr = (CardSpecDataRev0D *)pMain;

	if(!pStructRev || (STRUCTUREREV_D != *pStructRev)) 
		return ERROR_MISMATCH;

	ptr->LengthInByte = sizeof(CardSpecDataRev0D);
	if(pStructRev)			ptr->StructureRevision = *pStructRev;
	if(pDesignType)			ptr->DesignType= (*pDesignType);


 	if(pAntTx)	
		for (i=0; i<=(MAXNUM_MIMODEVICE-1); i++)
			ptr->AntCfg[i].TxAntCfg =(pAntTx[i]);

	if(pAntRx)	
		for (i=0; i<=(MAXNUM_MIMODEVICE-1); i++)
			ptr->AntCfg[i].RxAntCfg =(pAntRx[i]);

	if(pSpiSizeInfo)	ptr->SpiSize.size = (*pSpiSizeInfo); 



	if(pCapable_2G)		ptr->Capabilities.Capable_2G = (*pCapable_2G);
	
	if(pCapable_5G)		ptr->Capabilities.Capable_5G = (*pCapable_5G);

	if(pInternalBluetooth)	ptr->Capabilities.InternalBluetooth = (*pInternalBluetooth);

	if(pAntennaDiversity)		ptr->Capabilities.Antenna_Diversity = (*pAntennaDiversity);

	if(pLNA_2G)		ptr->Capabilities.LNA_2G = (*pLNA_2G);

	if(pLNA_5G)		ptr->Capabilities.LNA_5G = (*pLNA_5G);

	if(pSameAnt_2G_BT)	ptr->Capabilities.SameAnt_2G_BT = (*pSameAnt_2G_BT);

	if(pSameAnt_5G_BT)	ptr->Capabilities.SameAnt_5G_BT = (*pSameAnt_5G_BT);

	if(pCapabilities_2G) ptr->Concurrency.Capabilities_2G = (*pCapabilities_2G);
						
	if(pCapabilities_5G) ptr->Concurrency.Capabilities_5G = (*pCapabilities_5G);	

	if(pAntDiversityInfo) ptr->AntDiversityInfo = (*pAntDiversityInfo);
	if(pFEVersion)		ptr->FEVersion = (*pFEVersion);

	if(pCC)				ptr->CountryCode = (*pCC) ;

	if(pExtXtalSource)	ptr->MiscFlag.ExtXtalSource = (*pExtXtalSource);
	if(pExtSleepClk)	ptr->MiscFlag.ExtSleepClk = (*pExtSleepClk);
	if(pWlanWakeUp)		ptr->MiscFlag.WlanWakeUp = (*pWlanWakeUp);
	if(pLNAGain)		ptr->LNAGain = (*pLNAGain);

	if(pBTFrontEnd2G)
	{
		for (int i=0; i<9; i++)
			ptr->BTFrontEnd2G[i]= pBTFrontEnd2G[i];
	}
	if(pBTFrontEnd5G)
	{
		for (int i=0; i<9; i++)
			ptr->BTFrontEnd5G[i]= pBTFrontEnd5G[i];
	}

    if(pSocOrRev)       ptr->Soc_OR_Rev = (*pSocOrRev);
	if(pRfuOrRev)       ptr->Rfu_OR_Rev = (*pRfuOrRev);

 	if(pTestToolVer)	ptr->TestVersion = (*pTestToolVer);
	if(pMfgTaskVersion)	ptr->MfgTaskVersion = (*pMfgTaskVersion);
	if(pDllVersion)		ptr->DllVersion = (*pDllVersion);

 	if(pMimoTxCnt)	ptr->MimoTxCnt =(*pMimoTxCnt);
  	if(pMimoRxCnt)	ptr->MimoRxCnt =(*pMimoRxCnt);

	return status;
}

#if !defined(_W8787_) && !defined(_W8790_) && !defined(_W8782_)
int DutSharedClssHeader getAnnexType24Data( 
								   WORD *BbuAddr, BYTE *BbuVal)
{
 	int status=0, i=0, j=0, ChCnt=0;
 	ANNEX_TYPE_24_BBUSETTING_VC *ptr=NULL;
	// parse the data and pass it back to app. 
	if(NULL == pAnnex24) return ERROR_DATANOTAVAILABLE; 

	ptr = (ANNEX_TYPE_24_BBUSETTING_VC *)pAnnex24;

	if( BbuAddr && BbuVal)
	{
			for (i=0; i <MAX_SETTING_PAIRS; i++)
			{
				BbuAddr[i]	= ptr->BbuValuePairs[i].address;
				BbuVal[i]	= ptr->BbuValuePairs[i].value;
			}
	}

 	return status;
}

int DutSharedClssHeader setAnnexType24Data(WORD *BbuAddr, BYTE *BbuVal)
{
  	int status=0, i=0;
 	ANNEX_TYPE_24_BBUSETTING_VC *ptr=NULL;
 
	// alloc space to pointer, fill structure, other than next pointer and csc
	
	if(NULL == pAnnex24)
		pAnnex24 = malloc(sizeof(ANNEX_TYPE_24_BBUSETTING_VC));

	ptr = (ANNEX_TYPE_24_BBUSETTING_VC *)pAnnex24;
	memset(ptr, 0, sizeof(ANNEX_TYPE_24_BBUSETTING_VC));
	setAnnexInfo(ptr, sizeof(ANNEX_TYPE_24_BBUSETTING_VC),ANNEX_TYPE_NUM24,
					0, 0, 0,0); 

	if( BbuAddr && BbuVal)
	{
		for (i=0; i <MAX_SETTING_PAIRS; i++)
		{
			ptr->BbuValuePairs[i].address = (BYTE)BbuAddr[i];
			ptr->BbuValuePairs[i].value = BbuVal[i];
		}
	}
 	return status;
}

int DutSharedClssHeader getAnnexType25Data( 
								   DWORD *SocAddr, DWORD *SocVal)
{
 	int status=0, i=0, j=0, ChCnt=0;
 	ANNEX_TYPE_25_SOCSETTING_VC *ptr=NULL;
	// parse the data and pass it back to app. 
	if(NULL == pAnnex25) return ERROR_DATANOTAVAILABLE; 

	ptr = (ANNEX_TYPE_25_SOCSETTING_VC *)pAnnex25;

	if( SocAddr && SocVal)
	{
			for (i=0; i <MAX_SETTING_PAIRS; i++)
			{
				SocAddr[i]	= ptr->SocValuePairs[i].address;
				SocVal[i]	= ptr->SocValuePairs[i].value;
			}
	}

 	return status;
}

int DutSharedClssHeader setAnnexType25Data(DWORD *SocAddr, DWORD *SocVal)
{
  	int status=0, i=0;
 	ANNEX_TYPE_25_SOCSETTING_VC *ptr=NULL;
 
	// alloc space to pointer, fill structure, other than next pointer and csc
	
	if(NULL == pAnnex25)
		pAnnex25 = malloc(sizeof(ANNEX_TYPE_25_SOCSETTING_VC));

	ptr = (ANNEX_TYPE_25_SOCSETTING_VC *)pAnnex25;
	memset(ptr, 0, sizeof(ANNEX_TYPE_25_SOCSETTING_VC));
	setAnnexInfo(ptr, sizeof(ANNEX_TYPE_25_SOCSETTING_VC),ANNEX_TYPE_NUM25,
					0, 0, 0,0); 

	if( SocAddr && SocVal)
	{
		for (i=0; i <MAX_SETTING_PAIRS; i++)
		{
			ptr->SocValuePairs[i].address = SocAddr[i];
			ptr->SocValuePairs[i].value = SocVal[i];
		}
	}
 	return status;
}
 
int DutSharedClssHeader getAnnexType27Data(
								   BYTE *LED)
{
 	int status=0, i=0, j=0, ChCnt=0;
 	ANNEX_TYPE_27_LED_VC *ptr=NULL;
	// parse the data and pass it back to app. 
	if(NULL == pAnnex27) return ERROR_DATANOTAVAILABLE; 

	ptr = (ANNEX_TYPE_27_LED_VC *)pAnnex27;

	if( LED)
	{
			for (i=0; i <MAX_LED; i++)
			{
				LED[i]	= ptr->LED[i]; 
			}
	}
 	return status;
}

int DutSharedClssHeader setAnnexType27Data(BYTE *LED)
{
  	int status=0, i=0;
 	ANNEX_TYPE_27_LED_VC *ptr=NULL;
 
	// alloc space to pointer, fill structure, other than next pointer and csc
	
	if(NULL == pAnnex27)
		pAnnex27 = malloc(sizeof(ANNEX_TYPE_27_LED_VC));

	ptr = (ANNEX_TYPE_27_LED_VC *)pAnnex27;
	memset(ptr, 0, sizeof(ANNEX_TYPE_27_LED_VC));
	setAnnexInfo(ptr, sizeof(ANNEX_TYPE_27_LED_VC),ANNEX_TYPE_NUM27,
					0, 0, 0,0); 

	if( LED)
	{
		for (i=0; i <MAX_LED; i++)
		{
			ptr->LED[i] = LED[i]; 
		}
	}
 	return status;
}
 
int DutSharedClssHeader getAnnexType28Data(BYTE *UsbString)
{
 	int status=0, i=0, j=0, ChCnt=0;
 	ANNEX_TYPE_28_USBSTRINGS_VC *ptr=NULL;
	// parse the data and pass it back to app. 
	if(NULL == pAnnex28) return ERROR_DATANOTAVAILABLE; 

	ptr = (ANNEX_TYPE_28_USBSTRINGS_VC *)pAnnex28;

	if(UsbString)
	{
		for (i=0; i <MAX_VPSTR_SIZE; i++)
		{
			UsbString[i]	= ptr->VendorProductString[i]; 
		}
	}
 	return status;
}

int DutSharedClssHeader setAnnexType28Data(BYTE *UsbString)
{
  	int status=0, i=0;
 	ANNEX_TYPE_28_USBSTRINGS_VC *ptr=NULL;
 
	// alloc space to pointer, fill structure, other than next pointer and csc
	
	if(NULL == pAnnex28)
		pAnnex28 = malloc(sizeof(ANNEX_TYPE_28_USBSTRINGS_VC));

	ptr = (ANNEX_TYPE_28_USBSTRINGS_VC *)pAnnex28;
	memset(ptr, 0, sizeof(ANNEX_TYPE_28_USBSTRINGS_VC));
	setAnnexInfo(ptr, sizeof(ANNEX_TYPE_28_USBSTRINGS_VC),ANNEX_TYPE_NUM28,
					0, 0, 0,0); 

	if( UsbString)
	{
		for (i=0; i <MAX_VPSTR_SIZE; i++)
		{
			ptr->VendorProductString[i] = UsbString[i]; 
		}
	}

 	return status;
}

int DutSharedClssHeader getAnnexType31Data( 
						int DeviceId,
						char Rssi_Nf[NUM_SUBBAND_G], 
						char Rssi_cck[NUM_SUBBAND_G], 
						char Rssi_ofdm[NUM_SUBBAND_G], 
						char Rssi_mcs[NUM_SUBBAND_G])
{
	int status=0, i=0, ChCnt=0;
 	ANNEX_TYPE_31_RSSI_G_VC *ptr=NULL;
	// parse the data and pass it back to app. 
//	if(BAND_802_11A == BandId) return ERROR_DATANOTAVAILABLE; 
	if(NULL == pAnnex31[DeviceId]) return ERROR_DATANOTAVAILABLE; 

	ptr = (ANNEX_TYPE_31_RSSI_G_VC *)pAnnex31[DeviceId];

	for (i=0; i<NUM_SUBBAND_G ; i++)
	{
 			if(Rssi_Nf)		Rssi_Nf[i]		= ptr->Rssi[i].NfOffset;	
			if(Rssi_cck)	Rssi_cck[i]		= ptr->Rssi[i].RssiOffset_CCK_11B;
			if(Rssi_ofdm)	Rssi_ofdm[i]	= ptr->Rssi[i].RssiOffset_OFDM_11G;	
			if(Rssi_mcs)	Rssi_mcs[i]		= ptr->Rssi[i].RssiOffset_MCS_11N;
	}
 	return status;
}						

int DutSharedClssHeader setAnnexType31Data(	int DeviceId, //int PathId, int BandId,
						char Rssi_Nf[NUM_SUBBAND_G], 
						char Rssi_cck[NUM_SUBBAND_G], 
						char Rssi_ofdm[NUM_SUBBAND_G], 
						char Rssi_mcs[NUM_SUBBAND_G]) 
{
	int status=0, i=0;
 	ANNEX_TYPE_31_RSSI_G_VC *ptr=NULL;
 
	// alloc space to pointer, fill structure, other than next pointer and csc
//	if(BAND_802_11A == BandId) return ERROR_INPUT_INVALID; 
	
	if(NULL == pAnnex31[DeviceId])
		pAnnex31[DeviceId] = malloc(sizeof(ANNEX_TYPE_31_RSSI_G_VC));

	ptr = (ANNEX_TYPE_31_RSSI_G_VC *)pAnnex31[DeviceId];
	memset(ptr, 0, sizeof(ANNEX_TYPE_31_RSSI_G_VC));
	setAnnexInfo(ptr, sizeof(ANNEX_TYPE_31_RSSI_G_VC),ANNEX_TYPE_NUM31,
					DeviceId, 0, BAND_802_11G,0); 

	for (i=0; i<NUM_SUBBAND_G ; i++)
	{
		ptr->Rssi[i].NfOffset				= Rssi_Nf[i];
		ptr->Rssi[i].RssiOffset_CCK_11B	= Rssi_cck[i];
		ptr->Rssi[i].RssiOffset_OFDM_11G	= Rssi_ofdm[i];
		ptr->Rssi[i].RssiOffset_MCS_11N	= Rssi_mcs[i];
	}
 
  	return status;
}

int DutSharedClssHeader getAnnexType32Data(
					   int DeviceId,
						char Rssi_Nf[NUM_SUBBAND_A], 
						char Rssi_cck[NUM_SUBBAND_A], 
						char Rssi_ofdm[NUM_SUBBAND_A], 
						char Rssi_mcs[NUM_SUBBAND_A]) 
{
	int status=0, i=0, ChCnt=0;
 	ANNEX_TYPE_32_RSSI_A_VC *ptr=NULL;
	// parse the data and pass it back to app. 
	if(NULL == pAnnex32[DeviceId]) return ERROR_DATANOTAVAILABLE; 

	ptr = (ANNEX_TYPE_32_RSSI_A_VC *)pAnnex32[DeviceId];

	for (i=0; i<NUM_SUBBAND_A ; i++)
	{
 			if(Rssi_Nf)		Rssi_Nf[i]		= ptr->Rssi[i].NfOffset;	
			if(Rssi_cck)	Rssi_cck[i]		= ptr->Rssi[i].RssiOffset_CCK_11B;
			if(Rssi_ofdm)	Rssi_ofdm[i]	= ptr->Rssi[i].RssiOffset_OFDM_11G;	
			if(Rssi_mcs)	Rssi_mcs[i]		= ptr->Rssi[i].RssiOffset_MCS_11N;
	}

 	return status;
}

int DutSharedClssHeader setAnnexType32Data(	int DeviceId, //int PathId, int BandId,
						char Rssi_Nf[NUM_SUBBAND_A], 
						char Rssi_cck[NUM_SUBBAND_A], 
						char Rssi_ofdm[NUM_SUBBAND_A], 
						char Rssi_mcs[NUM_SUBBAND_A]) 
{
  	int status=0, i=0;
 	ANNEX_TYPE_32_RSSI_A_VC *ptr=NULL;
 
	// alloc space to pointer, fill structure, other than next pointer and csc
	
	if(NULL == pAnnex32[DeviceId])
		pAnnex32[DeviceId] = malloc(sizeof(ANNEX_TYPE_32_RSSI_A_VC));

	ptr = (ANNEX_TYPE_32_RSSI_A_VC *)pAnnex32[DeviceId];
	memset(ptr, 0, sizeof(ANNEX_TYPE_32_RSSI_A_VC));
	setAnnexInfo(ptr, sizeof(ANNEX_TYPE_32_RSSI_A_VC),ANNEX_TYPE_NUM32,
					DeviceId, 0, BAND_802_11A,0); 

	for (i=0; i<NUM_SUBBAND_A ; i++)
	{
		ptr->Rssi[i].NfOffset				= Rssi_Nf[i];
		ptr->Rssi[i].RssiOffset_CCK_11B	= Rssi_cck[i];
		ptr->Rssi[i].RssiOffset_OFDM_11G	= Rssi_ofdm[i];
		ptr->Rssi[i].RssiOffset_MCS_11N	= Rssi_mcs[i];
	}
 
  	return status;
}
#endif	//#if !defined(_W8787_) || defined(_W8790_)

int DutSharedClssHeader getEndOfAnnex(int deviceType, void *pObj, DWORD* pointer)
{
	int ptr=0, nextPtr=0, sizeOfTbl=0, type=0;
	int status=0;
	int EndOfAnnex=ANNEX_ADDR_END; 
	int DeviceId=0,  PathId=0,  BandId=0, SubBandId=0;

 // get Cal Main. if not programed, no annexes, if programmed, get last annex
	status = GetFirstAnnexInfo(deviceType, pObj, &ptr, &sizeOfTbl, &type, 
									&DeviceId,  &PathId,  
									&BandId, &SubBandId);
 	if(ANNEX_ADDR_END != ptr && ANNEX_ADDR_BLANK != ptr)
	{
		EndOfAnnex = (ptr + sizeOfTbl);
		do
		{
			// get next pointer
			//status =Spi_DumpData(ptr+4, 1, (DWORD*)&address,  (DWORD*)&nextPtr);
//			status =getNextAnnexPtr(ptr, &nextPtr); 
			status =GetNextAnnexPtr(deviceType, pObj, ptr, &nextPtr); 
			if(status) return status;
			if(ANNEX_ADDR_END == nextPtr || ANNEX_ADDR_BLANK == nextPtr) break;
			getAnnexInfo(deviceType, pObj, nextPtr, &sizeOfTbl, &type, 
									&DeviceId,  &PathId,  
									&BandId, &SubBandId); 
			EndOfAnnex = (nextPtr + sizeOfTbl);
			ptr = nextPtr;
		}while (1);
	}

	*pointer = EndOfAnnex; 

	return status;
}

int DutSharedClssHeader getNextOpenSpace(int deviceType, void *pObj,  int PartMaxAddress, BOOL includeCal, int sizeOfTblInByte, int* pointer)
{
	DWORD HeaderTblData[VMR_HEADER_LENGTH/sizeof(DWORD)]={0};
	DWORD Version=0; 
	int CsC=0; 
	DWORD EndOfCIS=0, EndOfBoot=0, EndOfCal=0, EndOfAnnex=0;
	int OpenPtr=0, nextPtr=0, address=0, sizeOfTbl=0, type=0 ; 
	CardSpecData CalTable={0}; 
 	int len=sizeof(CardSpecData);  
	int 	ptr=0, status=0;
	// this function is needed with add a new annex, 
// Get open space: the open space is after CIS or Boot. 
// Get the pointer of the end of CIS/ boot/ CalMain from header. 
// the end of them. the end of the last annex. 
 	DebugLogRite("### Get Open Space ###\n");

	status=GetHeaderTbl(deviceType, pObj, HeaderTblData, &Version, &CsC);
	if( status) return status;
	// CIS length, pointer
	EndOfCIS = (HeaderTblData[5]&0xFFFF) + HeaderTblData[6]; //0x14, 0x18

	// Boot length pointer
	EndOfBoot = (HeaderTblData[1]&0xFFFF) + HeaderTblData[2]; //0x04, 0x08

// Cal length pointer
	EndOfCal = (HeaderTblData[19]&0xFFFF) + HeaderTblData[18]; //0x44/4, 0x48/4
// add annexes

//	if(STRUCTUREREV_A == Version) 
//		getEndOfAnnex(&EndOfAnnex); 
//	else
		EndOfAnnex =0;
  
	// the max value is the open space's location
	// the max of EndOfCIS, EndOfBoot, EndOfCal, EndOfAnnex; 
//;	OpenPtr= EndOfCIS;
//;	if(EndOfBoot> OpenPtr) OpenPtr = EndOfBoot;
//;	if(EndOfCal> OpenPtr) OpenPtr = EndOfCal;
		OpenPtr = EndOfCal;
	if(includeCal) 
	{
		if((int)EndOfAnnex > OpenPtr) 
			OpenPtr = EndOfAnnex;
	}

	if( (OpenPtr + sizeOfTblInByte) > (PartMaxAddress+1)) 
	{
		*pointer = 0;
		return ERROR_SPACENOTAVAILABLE;
	}

   *pointer = OpenPtr;
		
   return status;
}

int DutSharedClssHeader getAnnexInfo(int deviceType, void *pObj, int pointer,	int *sizeOfTblInByte, int* type,
							 int *DeviceId, int *PathId, 
							 int *BandId,	int *SubBandId) 
{    
	ANNEX_TYPE_HEADER_EXTENDED_VC address={0}, Header={0};
  	int status=0;
	// use pointer to get header infor for the annex
//	status =Spi_DumpData(pointer, 
	status =DumpE2PData(deviceType, pObj, pointer, 
		sizeof(ANNEX_TYPE_HEADER_EXTENDED_VC)/sizeof(DWORD), 
		(DWORD*)&address, (DWORD*)&Header);
	if(sizeOfTblInByte) *sizeOfTblInByte = Header.typeHdr.LengthInByte;
	if(type)		*type = Header.typeHdr.AnnexType;
	if(DeviceId)	*DeviceId = Header.DeviceId; 
	if(PathId)		*PathId = Header.PathId; 
	if(BandId)		*BandId = Header.BandId;
	if(SubBandId)	*SubBandId = Header.SubBandId;

	return 0;
}

int DutSharedClssHeader setAnnexInfo(void *pHeader, 
							 int sizeOfTblInByte, 
							 int type,
							 int DeviceId,	int PathId, 
							 int BandId,	int SubBandId) 
{    
 	// use pointer to get header infor for the annex
	((ANNEX_TYPE_HEADER_EXTENDED_VC*)pHeader)->typeHdr.AnnexType= type;
	((ANNEX_TYPE_HEADER_EXTENDED_VC*)pHeader)->typeHdr.LengthInByte = sizeOfTblInByte;
 	((ANNEX_TYPE_HEADER_EXTENDED_VC*)pHeader)->DeviceId = DeviceId;
 	((ANNEX_TYPE_HEADER_EXTENDED_VC*)pHeader)->PathId = PathId;
 	((ANNEX_TYPE_HEADER_EXTENDED_VC*)pHeader)->BandId = BandId;
 	((ANNEX_TYPE_HEADER_EXTENDED_VC*)pHeader)->SubBandId = SubBandId;

	return 0;
}

int DutSharedClssHeader setAnnexInfoRevD(void *pHeader, 
							 int sizeOfTblInByte, 
							 int type,
							 int DeviceId,	int PathId)
{    
 	// use pointer to get header infor for the annex
	((ANNEX_TYPE_HEADER_EXTENDED_VD*)pHeader)->typeHdr.AnnexType= type;
	((ANNEX_TYPE_HEADER_EXTENDED_VD*)pHeader)->typeHdr.LengthInByte = sizeOfTblInByte;
 	((ANNEX_TYPE_HEADER_EXTENDED_VD*)pHeader)->DeviceId = DeviceId;
 	((ANNEX_TYPE_HEADER_EXTENDED_VD*)pHeader)->PathId = PathId;
	return 0;
}

