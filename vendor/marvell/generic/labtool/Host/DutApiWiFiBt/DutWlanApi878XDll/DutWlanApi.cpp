/** @file DutWlanApi.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include <stdio.h>
#include "../../DutCommon/Clss_os.h"
#include "../../DutCommon/utilities.h" 
#include "../../DutCommon/mathUtility.h"

#include "DutIf_IfClss.h"

#ifndef _LINUX_
#include DUTCLASS_SHARED_STDH_PATH	//"../DutShareApi878XDll/stdafx.h" 
#include DUTDLL_WLAN_HC_PATH	//"../DutWlanApi.hc"
#ifdef _WIN_NDIS_
#include "../../windis5/include/WINDIS.h"
#endif

#else	//_LINUX_
extern int query1(ULONG OID, PUCHAR pInfo, UINT len, UINT mfg_rsp);
extern int query(ULONG OID, PUCHAR pInfo, UINT len);
int DebugLog=0;
extern DWORD GetErrorLast(void);
#endif //_LINUX_

#include DUTCLASS_WLAN_H_PATH	//"DutWlanApiClss.h"
#include "../DutIf/DutNdis.h"
#include "../DutIf/ioctl.h"
#include "WlanMfgCmd.h"
 
static	int seqnum=0; 

#ifndef _LINUX_
HANDLE h;
int vg_retry_temp;
HANDLE     hmutex;
int	vg_maxWait;
#endif //#ifndef _LINUX_

void  Cf_StuffHeader(void* buf, unsigned short  int mfgCmd, unsigned short len);

#define LOCK_MUTEX		//WaitForSingleObject(hmutex, INFINITE);
#define RELEASE_MUTEX	//ReleaseMutex(hmutex);

#include "DutWlanIQCal.cpp"

////////////////////

int DutWlanApiClssHeader Cf_OpenCfDevice( )
{
 
 	if (WifiIf_GetDebugLogFlag()) 
		DebugLogRite(" Cf_OpenCfDevice \n"    );
#ifndef _LINUX_
	hmutex = CreateMutex (NULL, FALSE, NULL);
#endif //#ifndef _LINUX_

#ifdef	_GOLDEN_REFERENCE_UNIT_
 	return WifiIf_OpenDevice(1); 
#else //#ifdef	_GOLDEN_REFERENCE_UNIT_
 	return WifiIf_OpenDevice(0); 
#endif //#ifdef	_GOLDEN_REFERENCE_UNIT_
 	return 0;
}

int DutWlanApiClssHeader Cf_CloseCfDevice( )
{
 if (WifiIf_GetDebugLogFlag()) DebugLogRite(" Cf_CloseCfDevice \n");
	return WifiIf_CloseDevice( ); 
}

//////////////////////////
int DutWlanApiClssHeader Cf_SetModuleTypeConf(int ModuleType)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_SWT_MODULETYPE
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdModuleType_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
	TxBuf.action = THIS_ACTION; 
	TxBuf.ModuleType = ModuleType; 

	if (WifiIf_GetDebugLogFlag())
		DebugLogRite ("Cf_SetModuleTypeConf  %X\n", ModuleType);

		err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	
	return err; 
}

int DutWlanApiClssHeader Cf_GetModuleTypeConf(int *pModuleType)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_SWT_MODULETYPE
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdModuleType_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
	TxBuf.action = THIS_ACTION; 
 

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	if (WifiIf_GetDebugLogFlag())
		DebugLogRite ("Cf_GetModuleTypeConf  %X\n", TxBuf.ModuleType);

	if(pModuleType) (*pModuleType) =(short)TxBuf.ModuleType ;
	return err; 
}

//////////////////////////
int DutWlanApiClssHeader Cf_Poke(DWORD address, DWORD data, DWORD DeviceId)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_MAC_REG
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdMacReg_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)
	TxBuf.address = address;
   	TxBuf.data = data;

	if (WifiIf_GetDebugLogFlag())
		DebugLogRite ("Cf_Poke  %08X %08X\n", address, data);

 		err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	
	return err; 
}

int DutWlanApiClssHeader Cf_Peek(DWORD address, DWORD *data, DWORD DeviceId)   
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_MAC_REG
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdMacReg_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)
   	TxBuf.address = address;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	*data =TxBuf.data;

	if (WifiIf_GetDebugLogFlag())
		DebugLogRite ("Cf_Peek  %08X %08X\n", address, *data);

	return err; 
}
 
int DutWlanApiClssHeader Cf_BbpPoke(DWORD address, DWORD data, DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_BB_REG
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdBbReg_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

	TxBuf.address = address;
   	TxBuf.data = data;

	if (WifiIf_GetDebugLogFlag())
		DebugLogRite ("Cf_BbpPoke  %02X %02X\n", address, data);

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	return err; 
}

int DutWlanApiClssHeader Cf_BbpPeek(DWORD address, DWORD *data, DWORD DeviceId)  
{ 
     
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_BB_REG
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdBbReg_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

   	TxBuf.address = address;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	if (err)
	{
		if (WifiIf_GetDebugLogFlag())
			DebugLogRite ("Failed query of OID_MRVL_MFG_COMMAND; error 0x%08X  0x%08X\n", 
				err,  WifiIf_GetErrorLast());
	}

	
	*data =TxBuf.data;

	if (WifiIf_GetDebugLogFlag())
		DebugLogRite ("Cf_BbpPeek  %02X %02X\n", address, *data);

	return err; 
}
 
int DutWlanApiClssHeader Cf_RfPoke(DWORD address, DWORD data, DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RF_REG
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdRfReg_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

	TxBuf.address = address;
   	TxBuf.data = data;

	if (WifiIf_GetDebugLogFlag())
		DebugLogRite ("Cf_RfPeek  %02X %02X\n", address, data);

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	return err; 
}

int DutWlanApiClssHeader Cf_RfPeek(DWORD address, DWORD *data, DWORD DeviceId)  
{ 
     
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RF_REG
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdRfReg_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

   	TxBuf.address = address;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	*data =TxBuf.data;
	if (WifiIf_GetDebugLogFlag())
		DebugLogRite ("Cf_RfPeek  %08X %08X\n", address, *data);

	return err; 
}
 
int DutWlanApiClssHeader Cf_CAUPoke(DWORD address, DWORD data, DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_CAU_REG
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdRfReg_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

	TxBuf.address = address;
   	TxBuf.data = data;

	if (WifiIf_GetDebugLogFlag())
		DebugLogRite ("Cf_CAUPoke  %02X %02X\n", address, data);

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	return err; 
}

int DutWlanApiClssHeader Cf_CAUPeek(DWORD address, DWORD *data, DWORD DeviceId)  
{ 
     
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_CAU_REG
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdRfReg_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

   	TxBuf.address = address;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	*data =TxBuf.data;
	if (WifiIf_GetDebugLogFlag())
		DebugLogRite ("Cf_CAUPeek  %08X %08X\n", address, *data);

	return err; 
}

int DutWlanApiClssHeader Cf_LoadRFUFW(DWORD deviceId, DWORD offset, DWORD sizeBytes, BYTE *pSqBufAddr)
{ 
     
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RFU_FW
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
    int packetSize = sizeof(mfg_RFU_FW_t)+sizeBytes;

	mfg_RFU_FW_t	*pTxBuf = NULL;
    pTxBuf  = (mfg_RFU_FW_t*)malloc(packetSize); 
	memset (pTxBuf, 0, packetSize);
   
 	seqnum++; 
	Cf_StuffHeader(pTxBuf, THIS_CMD, packetSize); 
 	pTxBuf->action = THIS_ACTION; 

#if defined (_MIMO_)
   	pTxBuf->deviceId = deviceId;
#endif // #if defined (_MIMO_)

   	pTxBuf->offset = offset;
   	pTxBuf->seglen = sizeBytes;
	memcpy(pTxBuf->seg, pSqBufAddr, sizeBytes);

	if (WifiIf_GetDebugLogFlag())
		DebugLogRite("Cf_LoadRFUFW\n" );

	err = query1(OID_MRVL_MFG_COMMAND, (PUCHAR) pTxBuf, packetSize, THIS_RSP);

	if (pTxBuf) free(pTxBuf);

	return err; 
}

int DutWlanApiClssHeader Cf_UpdateRFUFW(DWORD deviceId)
{
     
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RFU_FW_UPDATE
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;

	mfg_CmdSoftResetHW_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(mfg_CmdSoftResetHW_t)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = deviceId;
#endif // #if defined (_MIMO_)

	if (WifiIf_GetDebugLogFlag())
		DebugLogRite("Cf_UpdateRFUFW\n" );

	err = query1(OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof(mfg_CmdSoftResetHW_t), THIS_RSP);

	return err; 
}

int DutWlanApiClssHeader Wifi_SpiWriteData(DWORD address, DWORD lenInDw, 
									 DWORD *data, BOOL SectorErase)
{  
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_SPI_EEPROM
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	spi_CmdEeprom_t *pTxBuf=NULL;
	int lenInByte=0; 
	lenInByte = sizeof(DWORD)*lenInDw;
	
    pTxBuf  = (spi_CmdEeprom_t*)malloc(sizeof(spi_CmdEeprom_t)+lenInByte); 

	memset (pTxBuf, 0, sizeof(spi_CmdEeprom_t) +lenInByte);

 	if (WifiIf_GetDebugLogFlag())
		DebugLogRite("SpiWriteData\n" );

	seqnum++; 
	Cf_StuffHeader(pTxBuf, THIS_CMD, sizeof(spi_CmdEeprom_t)+lenInByte); 
 	pTxBuf->action = THIS_ACTION; 
   	pTxBuf->signature = SIGNATURE_SPIEEPROM;
   	pTxBuf->sector_erase = SectorErase;

   	pTxBuf->address = address;
	pTxBuf->lenInByte = lenInByte;
	memcpy(pTxBuf->byte, data,lenInByte);

	EndianSwapByte((DWORD*)pTxBuf->byte, (3+lenInByte)/sizeof(DWORD));

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR)pTxBuf, sizeof(spi_CmdEeprom_t)+lenInByte, THIS_RSP);
	
	if (pTxBuf) free(pTxBuf);

	return err; 
}

int DutWlanApiClssHeader Wifi_SpiReadData(DWORD address, DWORD lenInDw, DWORD *data)   
{ 
       // Not Implemented 
 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_SPI_EEPROM
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	spi_CmdEeprom_t *pTxBuf=NULL;
	int lenInByte=0; 
	lenInByte = sizeof(DWORD)*lenInDw;
   
    pTxBuf  = (spi_CmdEeprom_t*)malloc(sizeof(spi_CmdEeprom_t)+lenInByte); 

	memset (pTxBuf, 0, sizeof(spi_CmdEeprom_t) +lenInByte);

	if (WifiIf_GetDebugLogFlag())
		DebugLogRite("SpiReadData\n" );

	seqnum++; 
	Cf_StuffHeader(pTxBuf, THIS_CMD, sizeof(spi_CmdEeprom_t)+lenInByte); 
 	pTxBuf->action = THIS_ACTION; 
   	pTxBuf->signature = SIGNATURE_SPIEEPROM;
   	pTxBuf->sector_erase = 0;

   	pTxBuf->address = address;
	pTxBuf->lenInByte = lenInByte;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR)pTxBuf, sizeof(spi_CmdEeprom_t)+lenInByte, THIS_RSP);

	memcpy(data, pTxBuf->byte, lenInByte);

	EndianSwapByte(data, (3+lenInByte)/sizeof(DWORD));

   	if (pTxBuf->Error  != ERROR_NONE)
	{
		err = pTxBuf->Error;
		
		if (WifiIf_GetDebugLogFlag())
			DebugLogRite ("Resp result  %08X\n", pTxBuf->Error);
	}
	
	if (pTxBuf) free(pTxBuf);
	return err; 
}

int DutWlanApiClssHeader Cf_FWVesion(BYTE *Version)   
{ 
      
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_FW_VERS
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdFwVers_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
 
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	sprintf((char*)Version, "FW Version:  %d.%d.%d.%02d\tMfg Version: %d.%d.%d.%d",
			((TxBuf.FwVersion&0xFF000000)>>24),
			((TxBuf.FwVersion&0x00FF0000)>>16),
			((TxBuf.FwVersion&0x0000FF00)>>8),
			((TxBuf.FwVersion&0x000000FF)>>0), 
			((TxBuf.MfgVersion&0xFF000000)>>24),
			((TxBuf.MfgVersion&0x00FF0000)>>16),
			((TxBuf.MfgVersion&0x0000FF00)>>8),
			((TxBuf.MfgVersion&0x000000FF)>>0)); 

	return err; 
}
 
int DutWlanApiClssHeader Cf_FWVerNumber(DWORD *FWVersion,DWORD *MFGFWVersion)   
{ 
      
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_FW_VERS
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdFwVers_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
 
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	if(FWVersion) *FWVersion = TxBuf.FwVersion;
	if(MFGFWVersion) *MFGFWVersion = TxBuf.MfgVersion;

	return err; 
}

int DutWlanApiClssHeader Cf_GetReleaseNote(char *ReleaseNote)   
{ 
      
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RELEASENOTE
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdReleaseNote_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
 
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	 
	strcpy(ReleaseNote, (char*)TxBuf.releaseNote); 
			
	if (WifiIf_GetDebugLogFlag())
		DebugLogRite ("ReleaseNote: %s\n", ReleaseNote);

	return err; 
}

int DutWlanApiClssHeader Cf_PokeRegWord(DWORD address, WORD data, DWORD DeviceId)  
{
	DWORD dataTemp=0;

	dataTemp = data;

	return Cf_PokeRegDword(address, dataTemp, DeviceId);
}

int DutWlanApiClssHeader Cf_PeekRegWord(DWORD address, WORD *data, DWORD DeviceId)
{
	DWORD dataTemp=0;
	int status=0;
	
	status = Cf_PeekRegDword(address, &dataTemp, DeviceId);
	if(ERROR_NONE != status)	return status;
	*data = (WORD)dataTemp; 
	return status;
}

int DutWlanApiClssHeader Cf_PokeRegByte(DWORD address, BYTE data, DWORD DeviceId)  
{
	DWORD dataTemp=0;

	dataTemp = data;

	return Cf_PokeRegDword(address, dataTemp, DeviceId);
}

int DutWlanApiClssHeader Cf_PeekRegByte(DWORD address, BYTE *data, DWORD DeviceId)  
{
	DWORD dataTemp=0;
	int status=0;
	
	status = Cf_PeekRegDword(address, &dataTemp, DeviceId);
	if(ERROR_NONE != status)	return status;
	*data = (BYTE)dataTemp; 
	return status;
}

int DutWlanApiClssHeader Cf_PokeRegDword(DWORD address, DWORD data, DWORD DeviceId)  
{	 
	return Cf_Poke(address, data, DeviceId);
}

int DutWlanApiClssHeader Cf_PeekRegDword(DWORD address, DWORD *data, DWORD DeviceId)  
{ 
	return Cf_Peek(address, data, DeviceId);
}

int DutWlanApiClssHeader Cf_SpiDownload(DWORD LenInDword, DWORD *DwordArray ) 
{ 
#define	EAGLE_SIZE_OF_CMD_BUFFER	1440
#define SEG_SZ						EAGLE_SIZE_OF_CMD_BUFFER - sizeof(mem_t)
#define DTCM						0x04000020//0x04000000// EagleLite;

#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_SPI_MEM
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	mem_t *pMemBuf=NULL;
    int dest=0,i=0;
    BOOL res=0;
	DWORD err=0;
    spi_CmdEepromPgm_t progBuf={0};
 
 
	BYTE *localArray=NULL;

    localArray = (BYTE*)malloc(LenInDword*sizeof(DWORD));

	memcpy(localArray, DwordArray , LenInDword*sizeof(DWORD));
   	EndianSwapByte((DWORD*)localArray,LenInDword); 

    // allocate segment structure
    pMemBuf = (mem_t *)malloc(EAGLE_SIZE_OF_CMD_BUFFER);
    //TODO check if NULL return

    // dest is used to save destination addr. because after calling query() 
    // mem_t struct pointed to by m is corrupted.
    dest = DTCM;
    for (i=0; i<(int)(LenInDword*sizeof(DWORD)); i += SEG_SZ)
    {
        WORD seglen;  // for saving m->seglen

 		pMemBuf->action = THIS_ACTION; 
		pMemBuf->dest = dest;
       		seglen = min(SEG_SZ, (LenInDword*sizeof(DWORD) - i));
		pMemBuf->seglen = seglen;
 		Cf_StuffHeader(pMemBuf, THIS_CMD, sizeof(mem_t)+seglen); 

        memcpy(pMemBuf->seg, localArray+i, pMemBuf->seglen);

		if (WifiIf_GetDebugLogFlag())
		{
			DebugLogRite("dest 0x%08X seglen %4d ", pMemBuf->dest, pMemBuf->seglen);
			DebugLogRite("%02x-%02x-%02x-%02x\n", pMemBuf->seg[0], pMemBuf->seg[1], 
							pMemBuf->seg[2], pMemBuf->seg[3]);
		}
		
		// send MI_MEM_SET command
        res = WifiIf_query(OID_MRVL_MFG_COMMAND, (PUCHAR)pMemBuf, sizeof(mem_t)+seglen);
        if ( res)
        {
			err = WifiIf_GetErrorLast();
			DebugLogRite("Failed MI_MEM_SET; error 0x%08X\n", err);
  			return (err);  
        }
#ifndef _LINUX_
        Sleep(seglen / 100);
#endif //#ifndef _LINUX_
        dest += seglen;
    }
 
    // prepare MFG_CMD_SPI_DL command
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
																	
 
#define THIS_CMD	MFG_CMD_SPI_DL
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET


	Cf_StuffHeader(&progBuf, THIS_CMD, sizeof(spi_CmdEepromPgm_t)); 
 	progBuf.action = THIS_ACTION; 
	progBuf.dest = 0x0; //0;
    progBuf.src  = DTCM;
    progBuf.len  = LenInDword*sizeof(DWORD);
    progBuf.sector_erase = TRUE;
    progBuf.signature = SIGNATURE_SPIEEPROM;
    DebugLogRite("\n  Writing Flash... D O   N O T   U N P L U G   A D A P T E R !\n\n");

    err = query1(OID_MRVL_MFG_COMMAND, (PUCHAR)&progBuf, sizeof(progBuf), THIS_RSP);
	  	 
    DebugLogRite("Successful Completion\n");

	if (localArray) free(localArray);
	if (pMemBuf) free(pMemBuf);
    return (err);
}  

//////////////////////////////////////////////////////////////
void  Cf_StuffHeader(void* buf, unsigned short  int mfgCmd, unsigned short len)
{
 	((PkHeader*)buf)->cmd= HostCmd_CMD_MFG_COMMAND;
	((PkHeader*)buf)->len= len;
	((PkHeader*)buf)->seq= seqnum;
	((PkHeader*)buf)->result= 0; 
	((PkHeader*)buf)->MfgCmd= mfgCmd; 

}

int DutWlanApiClssHeader Cf_ReadMacRegister(DWORD offset, DWORD* data) 
{ 
	int err=0;
	HostCmd_DS_MAC_REG_ACCESS	TxBuf={0};
  
 	seqnum++; 

 	TxBuf.Command = HostCmd_CMD_MAC_REG_ACCESS;
	TxBuf.Size =  sizeof(TxBuf);
	TxBuf.Action = HostCmd_ACT_GEN_READ; 
	TxBuf.Offset = (USHORT)offset;
	TxBuf.SeqNum = seqnum;
  
	err = query1(OID_MRVL_OEM_COMMAND, (PUCHAR)&TxBuf, sizeof(TxBuf), THIS_RSP);
 
	*data = TxBuf.Value; 
  
	return err; 
}

int DutWlanApiClssHeader Cf_WriteMacRegister(DWORD offset, DWORD  data) 
{ 
	int err=0;
	HostCmd_DS_MAC_REG_ACCESS	TxBuf={0};
  
 	seqnum++; 

 	TxBuf.Command = HostCmd_CMD_MAC_REG_ACCESS;
	TxBuf.Size =  sizeof(TxBuf);
	TxBuf.Action = HostCmd_ACT_GEN_WRITE; 
	TxBuf.Offset = (USHORT)offset;
	TxBuf.SeqNum = seqnum;
 	TxBuf.Value = data;
 
	err = query1(OID_MRVL_OEM_COMMAND, (PUCHAR)&TxBuf, sizeof(TxBuf), THIS_RSP);
	
	return err; 
}

int DutWlanApiClssHeader Cf_ReadBbpRegister(int offset, BYTE* data) 
{ 
  	int err=0;
	HostCmd_DS_BBP_REG_ACCESS	TxBuf={0};
  
 	seqnum++; 

 	TxBuf.Command = HostCmd_CMD_BBP_REG_ACCESS;
	TxBuf.Size =  sizeof(TxBuf);
	TxBuf.Action = HostCmd_ACT_GEN_READ; 
	TxBuf.Offset = offset;
	TxBuf.SeqNum = seqnum;
  
	err = query1(OID_MRVL_OEM_COMMAND, (PUCHAR)&TxBuf, sizeof(TxBuf), THIS_RSP);
    
	*data = TxBuf.Value; 
  
	return err; 
}

int DutWlanApiClssHeader Cf_WriteBbpRegister(int offset, BYTE  data) 
{ 
	int err=0;
	HostCmd_DS_BBP_REG_ACCESS	TxBuf={0};
  
 	seqnum++; 

 	TxBuf.Command = HostCmd_CMD_BBP_REG_ACCESS;
	TxBuf.Size =  sizeof(TxBuf);
	TxBuf.Action = HostCmd_ACT_GEN_WRITE; 
	TxBuf.Offset = offset;
	TxBuf.SeqNum = seqnum;
 	TxBuf.Value = data;
 
	err = query1(OID_MRVL_OEM_COMMAND, (PUCHAR)&TxBuf, sizeof(TxBuf), THIS_RSP);
 	
	return err; 
}

int DutWlanApiClssHeader Cf_ReadRfRegister(int offset, BYTE* data) 
{ 
 	int err=0;
	HostCmd_DS_RF_REG_ACCESS	TxBuf={0};
  
 	seqnum++; 

 	TxBuf.Command = HostCmd_CMD_RF_REG_ACCESS;
	TxBuf.Size =  sizeof(TxBuf);
	TxBuf.Action = HostCmd_ACT_GEN_READ; 
	TxBuf.Offset = (USHORT)offset;
	TxBuf.SeqNum = seqnum;
  
	err = query1(OID_MRVL_OEM_COMMAND, (PUCHAR)&TxBuf, sizeof(TxBuf), THIS_RSP);
	
	*data = TxBuf.Value; 
  
	return err; 
}

int DutWlanApiClssHeader Cf_WriteRfRegister(int offset, BYTE  data) 
{ 
	int err=0;
	HostCmd_DS_RF_REG_ACCESS	TxBuf={0};
  
 	seqnum++; 

 	TxBuf.Command = HostCmd_CMD_RF_REG_ACCESS;
	TxBuf.Size =  sizeof(TxBuf);
	TxBuf.Action = HostCmd_ACT_GEN_WRITE; 
	TxBuf.Offset = (USHORT)offset;
	TxBuf.SeqNum = seqnum;
 	TxBuf.Value = data;
 
	err = query1(OID_MRVL_OEM_COMMAND, (PUCHAR)&TxBuf, sizeof(TxBuf), THIS_RSP);
 	
	return err; 
}

int DutWlanApiClssHeader Cf_GetTxAnt(int *antSelection, DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_TX_ANT
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int				err;
 	mfg_CmdTxAnt_t	TxBuf = {0};

  
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

 	TxBuf.action = THIS_ACTION; 

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	*antSelection = TxBuf.antSelec;

	return err; 
}

int DutWlanApiClssHeader Cf_SetTxAnt(int antSelection, DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_TX_ANT
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int				err;
 	mfg_CmdTxAnt_t	TxBuf = {0};

  	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
  	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

	TxBuf.antSelec = antSelection;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
 	
	return err; 
}

int DutWlanApiClssHeader Cf_GetRxAnt(int *antSelection, DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RX_ANT
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdRxAnt_t	TxBuf = {0};

  
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)


	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	*antSelection = TxBuf.antSelec;

	return err; 
}

int DutWlanApiClssHeader Cf_SetRxAnt(int antSelection, DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RX_ANT
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
 	mfg_CmdRxAnt_t	TxBuf = {0};

  	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
  	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

	TxBuf.antSelec = antSelection;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
 	 
	return err; 
}

//////////////////////
int DutWlanApiClssHeader Cf_GetTxDataRate(int *DataRate, DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_TX_RATE
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdTxRate_t	TxBuf = {0};
	
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)


	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	*DataRate = TxBuf.txRate;

	return err; 
}

int DutWlanApiClssHeader Cf_SetTxDataRate(int DataRate, DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_TX_RATE
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdTxRate_t	TxBuf = {0};
	
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

 	TxBuf.txRate = DataRate; 

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	return err; 
}

int DutWlanApiClssHeader Cf_Enable22M(BOOL mode) 
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_22M_RATE
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_Cmd2272Rate_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
 	TxBuf.txRx2272Rate = mode; 

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	return err; 
}

int DutWlanApiClssHeader Cf_Enable72M(BOOL mode) 
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_72M_RATE
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_Cmd2272Rate_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
 	TxBuf.txRx2272Rate = mode; 

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	return err; 
}

int DutWlanApiClssHeader Cf_SetTxContMode(BOOL enable, BOOL CwMode, DWORD payloadPattern,
				DWORD		CSmode,
				DWORD		ActSubCh, 
				DWORD 		DeviceId) 
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_TX_CONT
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdTxCont_t	TxBuf = {0};
/*
	int DataRate=0;
	char Bssid[6]={0};

	if(enable)
	{
		Cf_GetTxDataRate(&DataRate);
		Cf_TxBrdCstPkts(DataRate, 0, 0x400, 1, 0, Bssid);
	}
*/
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

 	TxBuf.enableTx = enable; 
	TxBuf.cwMode = CwMode; 
	TxBuf.framePattern = payloadPattern;
	TxBuf.csMode = CSmode;

#if defined (_MIMO_)
  	TxBuf.ActSubCh = ActSubCh;
#endif // #if defined (_MIMO_)

	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
//	if(0 == enable) 
//		Sleep(100);

	return err; 
}

int DutWlanApiClssHeader Cf_SetTxCarrierSuppression(BOOL enable,
				DWORD 		DeviceId) 
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_EN_CST
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdCst_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

 	TxBuf.enableCst = enable; 
 	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	return err; 
}

int DutWlanApiClssHeader SetTxIQCalMode(BOOL enable, DWORD DeviceId) 
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_TXIQCALMODE
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdCst_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)
	TxBuf.enableCst = enable; 
 	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	return err; 
}
/*
int DutWlanApiClssHeader Cf_SetPowerMode(int mode) 
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_PW_MODE
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdPwrMode_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
 	TxBuf.powerMode = mode; 
 	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	 
	return err; 
}
*/
int DutWlanApiClssHeader Cf_SetPowerCycleMode(	 
						int cycle, 
						int sleepDur, 
						int awakeDur, 
						int stableXosc, 
						DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_PSM_CYCLE
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdPsmCycle_t	TxBuf = {0};
    
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action	= THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

	TxBuf.numCycle	= cycle?cycle:1; 
	TxBuf.sleepDur	= sleepDur?sleepDur:1000000;
	TxBuf.rxDur		= awakeDur?awakeDur:1000000;
	TxBuf.stableDur = stableXosc?stableXosc:2000;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	return err; 
}


int DutWlanApiClssHeader Cf_EnterSleepMode(void)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_PSM_CYCLE
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdPsmCycle_t	TxBuf = {0};
    
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	return err; 
}

int DutWlanApiClssHeader Cf_GetBeaconInterval(DWORD *interval, DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_BCN_INT 
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdBcnInt_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

  	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	*interval = TxBuf.beaconPeriod; 

	return err; 
}

int DutWlanApiClssHeader Cf_SetBeaconInterval(DWORD interval, DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_BCN_INT 
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdBcnInt_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

 	TxBuf.beaconPeriod = interval; 
 	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
		
	return err; 
}

int DutWlanApiClssHeader Cf_GetBeaconEnable(BOOL *enable, DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_CTRL_BCN
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdCtrlBcn_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

  	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	*enable = TxBuf.enableBcn;
 
	return err; 
}

int DutWlanApiClssHeader Cf_SetBeaconEnable(BOOL enable, DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_CTRL_BCN
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdCtrlBcn_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

 	TxBuf.enableBcn = enable; 
 	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	 
	return err; 
}

int DutWlanApiClssHeader Dev_GetBandAG(int *band, DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RF_BAND_AG
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdRfBand_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

  	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	*band = TxBuf.band_G0A1;
 
	return err; 
}

int DutWlanApiClssHeader Dev_SetBandAG(int band, DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RF_BAND_AG
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdRfBand_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

	TxBuf.band_G0A1 = band;

	err = query1(OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	return err; 
}

int DutWlanApiClssHeader Cf_GetRfChannel(int *channel, double *pFreqInMHz, DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RF_CHAN
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdRfChan_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

  	
	//DebugLogRite("OID_MRVL_MFG_COMMAND =%x \n",OID_MRVL_MFG_COMMAND);
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	if(channel) *channel = TxBuf.channel;
	if(pFreqInMHz) (*pFreqInMHz) = TxBuf.FreqInMHz;

	return err; 
}


int DutWlanApiClssHeader Cf_SetRfChannel(int channel, DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RF_CHAN
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdRfChan_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

	TxBuf.channel = channel;
  	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	  
	return err; 
}
 

int DutWlanApiClssHeader Dev_GetChannelBw(int *ChannelBw, DWORD DeviceId)
{
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_11N_CHANNELBW
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdChannelBw_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
  	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	*ChannelBw = TxBuf.BW20401005MHz;
 
	return err; 
} 

int DutWlanApiClssHeader Dev_SetChannelBw(int ChannelBw , DWORD DeviceId) 

{
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_11N_CHANNELBW
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdChannelBw_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
	TxBuf.BW20401005MHz = ChannelBw;
  	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
 
	return err; 
}

int DutWlanApiClssHeader Dev_GetSubBand(int *SubBandId, int *HighFreq, int *LowFreq,  
					DWORD DeviceId) 
{
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RF_FREQSUBBAND
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdRfPwrRange_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
  	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
 	if(SubBandId)	*SubBandId	= TxBuf.RangeIndex_PwrPnt;
	if(HighFreq)	*HighFreq	= TxBuf.Top;
	if(LowFreq)		*LowFreq	= TxBuf.Btm;


	return err; 
}



int DutWlanApiClssHeader Dev_GetRfChannelFreq(int band, int channel, double *chFreqInMhz,  
					DWORD DeviceId)
{
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RFCHANFREQ
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdRfBandChanFreq_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
    TxBuf.band          = band;
    TxBuf.channel       = channel;
  	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
 	
    if (chFreqInMhz)  
        *chFreqInMhz = ((double) TxBuf.FreqInMHz);
	
	return err; 
}


int DutWlanApiClssHeader Dev_SetRfChannelByFreq(double chFreqInMhz, DWORD DeviceId) 
{
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RFCHANFREQ
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdRfChan_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
    TxBuf.FreqInMHz       = (DWORD) floor(chFreqInMhz + 0.5);
  	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	return err; 
}



/*
int DutWlanApiClssHeader Cf_GetRfChannel_new(int *channel) 
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RF_CHAN_NEW
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdRfChan_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
  	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	*channel = TxBuf.channel;
 
	return err; 
}

int DutWlanApiClssHeader Cf_SetRfChannel_new(int channel) 
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RF_CHAN_NEW
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdRfChan_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
	TxBuf.channel = channel;
  	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	  
	return err; 
}

int DutWlanApiClssHeader Cf_GetRfChannel_div6(int *channel) 
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RF_CHAN_DIV6
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdRfChan_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
  	
	//DebugLogRite("OID_MRVL_MFG_COMMAND =%x \n",OID_MRVL_MFG_COMMAND);
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	*channel = TxBuf.channel;
 
	return err; 
}

int DutWlanApiClssHeader Cf_SetRfChannel_div6(int channel) 
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RF_CHAN_DIV6
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdRfChan_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
	TxBuf.channel = channel;
  	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	  
	return err; 
}

int DutWlanApiClssHeader Cf_SetRfBbFiler(int channel ) 
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_TUNED_BB
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdTunedRfBb_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
   	TxBuf.channel = channel;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	  
	return err; 
}
*/
int DutWlanApiClssHeader Cf_MacWakeupRfBBP() 
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_WAKEUP_RFBBP
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdWakeRfBB_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
   	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	  
	return err; 
}
 
int DutWlanApiClssHeader Cf_SetRfPowerRange(int pwr
#if defined (_MIMO_)					
												  ,
											  DWORD PathId,  
											  DWORD DeviceId
#endif //#if defined (_MIMO_)
											  )
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RFPWRRANGE
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdRfPwrRange_t	TxBuf = {0};
   
	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
  	TxBuf.pathId = PathId;
#endif // #if defined (_MIMO_)
  	TxBuf.RangeIndex_PwrPnt = pwr; 
  	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
  
	return err; 
}

int DutWlanApiClssHeader Cf_GetRfPowerRange(int *pRangeIndex, int *pTopPwr, int *pBtmPwr
#if defined (_MIMO_)					
												  ,
											  DWORD PathId,  
											  DWORD DeviceId
#endif //#if defined (_MIMO_)
											  )
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RFPWRRANGE
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdRfPwrRange_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
  	TxBuf.pathId = PathId;
#endif // #if defined (_MIMO_)
   	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
    if(pRangeIndex) *pRangeIndex = TxBuf.RangeIndex_PwrPnt; 
    if(pTopPwr) *pTopPwr = TxBuf.Top; 
    if(pBtmPwr) *pBtmPwr = TxBuf.Btm; 

	return err; 
}

int DutWlanApiClssHeader Cf_GetRfPowerSetting(
						 BYTE *pPaConf1,	BYTE *pPaConf2, 
						 BYTE *pVt_Hi_G,	BYTE *pVt_Lo_G,	BYTE *pInitP_G, 
						 BYTE *pVt_Hi_B,	BYTE *pVt_Lo_B,	BYTE *pInitP_B)
{ 
 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RFPWRSETTING 
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdRfPwrSetting_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
   	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	if(pPaConf1) *pPaConf1 = TxBuf.PaConf1;   
	if(pPaConf2) *pPaConf2 = TxBuf.PaConf2;  
 	if(pVt_Hi_G) *pVt_Hi_G = TxBuf.Vt_Hi_G; 
	if(pVt_Lo_G) *pVt_Lo_G = TxBuf.Vt_Lo_G; 
	if(pInitP_G) *pInitP_G = TxBuf.InitP_G; 
 
 	return err; 
}

int DutWlanApiClssHeader Cf_GetRfPower(
						int *pPwr4Pa, 
						int	*pCorrInitP_G,
						int	*pCorrInitP_B,
						int	*pCorrTh_G,
						int	*pCorrTh_B) 
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RF_POW 
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdRfPow_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
 
	err = query1 ( OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	if (WifiIf_GetDebugLogFlag())
	{
			DebugLogRite ("Cf_GetRfPower %d 0x%08X  0x%08X\n", 
				TxBuf.Pwr4Pa, 
				TxBuf.Corr1A, TxBuf.CorrOffset);
	}
	
   	if(pPwr4Pa) *pPwr4Pa		= TxBuf.Pwr4Pa;
	if(pCorrInitP_G)	(*pCorrInitP_G)	= TxBuf.Corr1A;
	if(pCorrTh_G)		(*pCorrTh_G)	= TxBuf.CorrOffset;

 	return err; 
}

#define vg_dwSpiSleep 100
int DutWlanApiClssHeader Cf_SpiPoke(int deviceType, void *pObj, DWORD address, DWORD *data, DWORD lenInByte, BOOL sec_erase)
{  
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_SPI_EEPROM
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	spi_CmdEeprom_t *pTxBuf=NULL;
   
    pTxBuf  = (spi_CmdEeprom_t*)malloc(sizeof(spi_CmdEeprom_t)+lenInByte); 

	memset (pTxBuf, 0, sizeof(spi_CmdEeprom_t) +lenInByte);

 	if (DebugLog)
		DebugLogRite("Cf_SpiPoke\n" );

	seqnum++; 
	Cf_StuffHeader(pTxBuf, THIS_CMD, sizeof(spi_CmdEeprom_t)+lenInByte); 
 	pTxBuf->action = THIS_ACTION; 
   	pTxBuf->signature = SIGNATURE_SPIEEPROM;
   	pTxBuf->sector_erase = sec_erase;

   	pTxBuf->address = address;
	pTxBuf->lenInByte = lenInByte;
	memcpy(pTxBuf->byte, data,lenInByte);

#ifdef BIG_ENDIAN
	EndianSwapeByte((DWORD*)pTxBuf->byte, (3+lenInByte)/sizeof(DWORD));
#endif //#ifdef BIG_ENDIAN

	Sleep(vg_dwSpiSleep);
	LOCK_MUTEX; //WaitForSingleObject(hmutex, INFINITE);

	err = query (OID_MRVL_MFG_COMMAND, (PUCHAR)pTxBuf, sizeof(spi_CmdEeprom_t)+lenInByte);
	if (err)
	{
		if (DebugLog)
			DebugLogRite ("Failed query of OID_MRVL_MFG_COMMAND; error 0x%08X  0x%08X\n", 
				err,  GetErrorLast());
	}

	RELEASE_MUTEX; //ReleaseMutex(hmutex);

	if (pTxBuf->header.MfgCmd != THIS_RSP)
	{
		err =ERROR_ERROR; 
		if (DebugLog)
			DebugLogRite(" --------- ERR cmd %08X; expected %08X\n", 
				pTxBuf->header.MfgCmd, THIS_RSP);
	}

 	if (pTxBuf->header.result  != ERROR_NONE)
	{
		if (DebugLog)
			DebugLogRite ("Resp result  %08X\n", pTxBuf->header.result);
	}

   	if (pTxBuf->Error  != ERROR_NONE)
	{
		err = pTxBuf->Error;
		
		if (DebugLog)
			DebugLogRite ("Resp result  %08X\n", pTxBuf->Error);
	}

 	free(pTxBuf);
	
	return err; 

}

int DutWlanApiClssHeader Dvc_GetCckFltScaler( 
					int *pScalerMode,  
					int DeviceId)
{
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_CCKFLTRSCALE
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int				err;
 	mfg_CmdCckFltrScale_t	TxBuf = {0};

  
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
	TxBuf.deviceId =DeviceId; 
 
	LOCK_MUTEX; //WaitForSingleObject(hmutex, INFINITE);

	err = query (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf));
	if (err)
	{
		if (DebugLog)
			DebugLogRite ("Failed query of OID_MRVL_MFG_COMMAND; error 0x%08X  0x%08X\n", 
				err,  GetErrorLast());
	}

	RELEASE_MUTEX; //ReleaseMutex(hmutex);

	if (TxBuf.header.MfgCmd != THIS_RSP)
	{
		err =ERROR_ERROR; 
		if (DebugLog)
			DebugLogRite(" --------- ERR cmd %08X; expected %08X\n", 
				TxBuf.header.MfgCmd, THIS_RSP);
	}

 	if (TxBuf.header.result  != ERROR_NONE)
	{
		if (DebugLog)
			DebugLogRite ("Resp result  %08X\n", TxBuf.header.result);
	}

   	if (TxBuf.Error  != ERROR_NONE)
	{
		err = TxBuf.Error;
		
		if (DebugLog)
			DebugLogRite ("Resp result  %08X\n", TxBuf.Error);
	}
	
	if(pScalerMode)	*pScalerMode		= TxBuf.scaleMode;
 
	return err; 
}

int DutWlanApiClssHeader Dvc_SetCckFltScaler( 
					int ScalerMode,  
					int DeviceId)
{
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_CCKFLTRSCALE
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int				err;
 	mfg_CmdCckFltrScale_t	TxBuf = {0};

  
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
	TxBuf.deviceId =DeviceId; 
 	TxBuf.scaleMode =ScalerMode; 

	LOCK_MUTEX; //WaitForSingleObject(hmutex, INFINITE);

	err = query (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf));
	if (err)
	{
		if (DebugLog)
			DebugLogRite ("Failed query of OID_MRVL_MFG_COMMAND; error 0x%08X  0x%08X\n", 
				err,  GetErrorLast());
	}

	RELEASE_MUTEX; //ReleaseMutex(hmutex);

	if (TxBuf.header.MfgCmd != THIS_RSP)
	{
		err =ERROR_ERROR; 
		if (DebugLog)
			DebugLogRite(" --------- ERR cmd %08X; expected %08X\n", 
				TxBuf.header.MfgCmd, THIS_RSP);
	}

 	if (TxBuf.header.result  != ERROR_NONE)
	{
		if (DebugLog)
			DebugLogRite ("Resp result  %08X\n", TxBuf.header.result);
	}

   	if (TxBuf.Error  != ERROR_NONE)
	{
		err = TxBuf.Error;
		
		if (DebugLog)
			DebugLogRite ("Resp result  %08X\n", TxBuf.Error);
	}
	
 	return err; 
}					

int DutWlanApiClssHeader Cf_SetRfPower( 
				  int *pPwr4Pa, 
				  int *pCorrOffset, 
				  int *pCorr1A, 
				  DWORD PathId,  DWORD DeviceId)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RF_POW 
#define THIS_RSP	RSP_CODE(THIS_CMD) 
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdRfPow_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
	TxBuf.deviceId = DeviceId;
	TxBuf.pathId = PathId;
   	TxBuf.Pwr4Pa = pPwr4Pa? *pPwr4Pa:15;
 
   	TxBuf.CorrOffset = pCorrOffset?*pCorrOffset:0;
   	TxBuf.Corr1A = pCorr1A?*pCorr1A:0;

	LOCK_MUTEX; //WaitForSingleObject(hmutex, INFINITE);

	err = query (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf));
	if (err)
	{
		if (DebugLog)
			DebugLogRite ("Failed query of OID_MRVL_MFG_COMMAND; error 0x%08X  0x%08X\n", 
				err,  GetErrorLast());
	}

	RELEASE_MUTEX; //ReleaseMutex(hmutex);

	if (TxBuf.header.MfgCmd != THIS_RSP)
	{
		err =ERROR_ERROR; 
		if (DebugLog)
			DebugLogRite(" --------- ERR cmd %08X; expected %08X\n", 
				TxBuf.header.MfgCmd, THIS_RSP);
	}

 	if (TxBuf.header.result  != ERROR_NONE)
	{
		if (DebugLog)
			DebugLogRite ("Resp result  %08X\n", TxBuf.header.result);
	}

   	if (TxBuf.Error  != ERROR_NONE)
	{
		err = TxBuf.Error;
		
		if (DebugLog)
			DebugLogRite ("Resp result  %08X\n", TxBuf.Error);
	}
	
	if(pCorrOffset) *pCorrOffset = TxBuf.CorrOffset;
	if(pCorr1A)   *pCorr1A = TxBuf.Corr1A;

	return err; 
}

int DutWlanApiClssHeader Cf_StepRfPower(int *pSteps, 
										DWORD PathId, 
										DWORD DeviceId)
{ 
 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_STEP_RF_PWR
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdStepRfPwr_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
  	TxBuf.pathId = PathId;
#endif // #if defined (_MIMO_)

   	TxBuf.StepRfPwr = *pSteps;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
   	*pSteps = TxBuf.StepRfPwr;

	return err; 
}


int DutWlanApiClssHeader Cf_AdjustPcktSifs(
		DWORD Enable, 
		DWORD dataRate,	
		DWORD pattern, 
		DWORD length,
		int ShortPreamble, 
		char *Bssid
#if defined (_MIMO_)
		,
		DWORD		ActSubCh, 
		DWORD		ShortGI, 
		DWORD		AdvCoding, 
		DWORD		TxBfOn,    
		DWORD		GFMode,    
		DWORD		STBC,
		DWORD		DPDEnable,
		DWORD		PowerID,
		DWORD		SignalBw,
#if defined (_W8787_) || 	defined (_W8782_)						
	DWORD	AdjustTxBurstGap, 
	DWORD	BurstSifsInUs, 
#endif //#if defined (_W8787_) || 	defined (_W8782_)						
		DWORD 		DeviceId
#endif // #if defined (_MIMO_)
		) 

{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_TX_FRAME
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int					err;
 	mfg_CmdTxFrame2_t	TxBuf = {0};

//	if (length > 0x480) return ERROR_INPUT_INVALID; 
 
	if (DATARATE_1M == dataRate) 
		ShortPreamble = 0; 

 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
  	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

	
	TxBuf.DataRate = dataRate;
	TxBuf.FramePattern = pattern;
	TxBuf.FrameLength = length;
	TxBuf.Enable = Enable;
	TxBuf.ShortPreamble = ShortPreamble; 
	if (Bssid)
		memcpy (TxBuf.Bssid, Bssid, 6);
	else
		memset (TxBuf.Bssid, 0xFF, 6);

#if defined (_MIMO_)
	TxBuf.ActSubCh	= ActSubCh; 
	TxBuf.ShortGI	= ShortGI; 
	TxBuf.AdvCoding = AdvCoding; 
    TxBuf.TxBfOn	= TxBfOn;    
    TxBuf.GFMode	= GFMode;    
    TxBuf.STBC		= STBC;
	//DPD
	TxBuf.DPDEnable	= DPDEnable;
	TxBuf.PowerID	= PowerID;
	TxBuf.SignalBw	= SignalBw;

#if defined (_W8787_) || 	defined (_W8782_)						
	TxBuf.AdjustBurstSifs = AdjustTxBurstGap; 
	TxBuf.BurstSifsInUs =  BurstSifsInUs; 
#endif //#if defined (_W8787_) || 	defined (_W8782_)						
	
#endif // #if defined (_MIMO_)

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
 	
	return err; 
}

int DutWlanApiClssHeader Cf_TxBrdCstPkts (
		DWORD dataRate,	
		DWORD pattern, 
		DWORD length,
		DWORD count, 
		int ShortPreamble, 
		char *Bssid
#if defined (_MIMO_)
		,
		DWORD		ActSubCh, 
		DWORD		ShortGI, 
		DWORD		AdvCoding, 
		DWORD		TxBfOn,    
		DWORD		GFMode,    
		DWORD		STBC,
		DWORD		DPDEnable,
		DWORD		PowerID,
 		DWORD 		DeviceId
#endif // #if defined (_MIMO_)
		) 

{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_TX_FRAME
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int					err;
 	mfg_CmdTxFrame_t	TxBuf = {0};

//	if (length > 0x480) return ERROR_INPUT_INVALID; 
 
	if (DATARATE_1M == dataRate) 
		ShortPreamble = 0; 

 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
  	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

	
	TxBuf.dataRate = dataRate;
	TxBuf.framePattern = pattern;
	TxBuf.frameLength = length;
	TxBuf.frameCount = count;
	TxBuf.shortPreamble = ShortPreamble; 
	if (Bssid)
		memcpy (TxBuf.bssid, Bssid, 6);
	else
		memset (TxBuf.bssid, 0xFF, 6);

#if defined (_MIMO_)
	TxBuf.ActSubCh	= ActSubCh; 
	TxBuf.ShortGI	= ShortGI; 
	TxBuf.AdvCoding = AdvCoding; 
    TxBuf.TxBfOn	= TxBfOn;    
    TxBuf.GFMode	= GFMode;    
    TxBuf.STBC		= STBC;
	//DPD
	TxBuf.DPDEnable	= DPDEnable;
	TxBuf.PowerID	= PowerID;

#endif // #if defined (_MIMO_)

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
 	
	return err; 
}

int DutWlanApiClssHeader Cf_SetTxDutyCycle (
		int enable,
		int dataRate,	
		int percentage, 
		int PayloadLengthInKB,
		int payloadPattern,
		int ShortPreamble
#if defined (_MIMO_)
		,							
		DWORD		ActSubCh, 
		DWORD		ShortGI, 
		DWORD		AdvCoding, 
		DWORD		TxBfOn,  
		DWORD		GFMode, 
		DWORD		STBC, 
		DWORD       DPDEnable,
		DWORD		PowerID,
		DWORD		SignalBw,
		DWORD 		DeviceId
#endif // #if defined (_MIMO_)
		) 
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_DUTY_CYCLE
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET
	
	int					err;
 	mfg_CmdDutyCycle_t	TxBuf = {0};

 		seqnum++; 

		if(-1 != payloadPattern) 
			payloadPattern &=0xFF;
	
		Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
  		TxBuf.Action = THIS_ACTION; 
		TxBuf.Enable = enable;

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

		TxBuf.DataRate = dataRate;
		TxBuf.Percent = percentage; 
		TxBuf.PayloadPattern = payloadPattern;
		TxBuf.ShortPreamble = ShortPreamble;

#if defined (_MIMO_)
		TxBuf.PayloadLengthInKB = PayloadLengthInKB;
		TxBuf.ActSubCh	= ActSubCh; 
		TxBuf.ShortGI	= ShortGI; 
		TxBuf.AdvCoding = AdvCoding; 
		TxBuf.TxBfOn	= TxBfOn;    
		TxBuf.GFMode	= GFMode;    
		TxBuf.STBC		= STBC;
		//DPD
		TxBuf.DPDEnable	= DPDEnable;
		TxBuf.PowerID	= PowerID;
		TxBuf.SignalBw  = SignalBw;
#endif // #if defined (_MIMO_)


		err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	return err; 
}
 
int DutWlanApiClssHeader Cf_XoscCal(WORD *cal, DWORD TU, DWORD DeviceId)  
{ 
      
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_XOSCCAL
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdXoscCal_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)


	TxBuf.TU = (TU>0)? TU: 10;
 
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	*cal =(USHORT)TxBuf.xoscCal;

	return err; 
}
 
int DutWlanApiClssHeader Cf_EnableBssidFilter(int mode, BYTE *Bssid, char *SSID, DWORD DeviceId)  
{ 
      
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_ENB_SSID
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdEnableSsid_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)


 	TxBuf.mode = mode; 
	if (Bssid)
		memcpy(TxBuf.Bssid , Bssid, sizeof(TxBuf.Bssid));
	if (SSID)
		memcpy(TxBuf.Ssid , SSID, sizeof(TxBuf.Bssid));
 
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	return err; 
}

int DutWlanApiClssHeader Cf_ClearRxPckt(DWORD DeviceId)
{ 
     
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_CLR_RX_UP
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_ReadCounts_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)


	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	return err; 
}

int DutWlanApiClssHeader Cf_GetRxPckt(
				 unsigned long *cntRx,
				 unsigned long *cntOk,
				 unsigned long *cntKo, 
				 DWORD DeviceId)  
{ 
      
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_GET_RX_UP
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_ReadCounts_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)


	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	*cntRx = TxBuf.PcktsCount;
	*cntOk = TxBuf.OkCount;
	*cntKo = TxBuf.KoCount;
	
	return err; 
}

int DutWlanApiClssHeader query1(
							ULONG OID, 
							PUCHAR pInfo, 
							UINT len, 
							UINT mfg_rsp)
{
	int err=0;
	
	LOCK_MUTEX; //WaitForSingleObject(hmutex, INFINITE);
	err = WifiIf_query(OID, pInfo, len);
	
	if (err)
	{
		if (WifiIf_GetDebugLogFlag())
			DebugLogRite ("Failed query of OID_MRVL_MFG_COMMAND; error 0x%08X  0x%08X\n", 
				err,  WifiIf_GetErrorLast());
	}

	RELEASE_MUTEX; //ReleaseMutex(hmutex);

 	if (((mfg_Cmd_t*)pInfo)->header.MfgCmd != mfg_rsp)
	{
		err =ERROR_ERROR; 
		if (WifiIf_GetDebugLogFlag())
			DebugLogRite(" --------- ERR cmd %08X; expected %08X\n", 
				((mfg_Cmd_t*)pInfo)->header.MfgCmd, mfg_rsp);
	}

	if (((mfg_Cmd_t*)pInfo)->header.result  != ERROR_NONE)
	{
		if (WifiIf_GetDebugLogFlag())
			DebugLogRite ("Resp result  %08X\n", ((mfg_Cmd_t*)pInfo)->header.result);
	}

   	if (((mfg_Cmd_t*)pInfo)->Error  != ERROR_NONE)
	{
		err = ((mfg_Cmd_t*)pInfo)->Error;
		
		if (WifiIf_GetDebugLogFlag())
			DebugLogRite ("Resp result  %08X\n", err);
	}

 	return err;
}

int DutWlanApiClssHeader Cf_GetHWVersion(
					WORD *socId, BYTE *socVersion, 
					BYTE *RfId, BYTE *RfVersion, 
					BYTE *BbId, BYTE *BbVersion, 
					DWORD DeviceId)  
{ 
      
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_GET_HW_VERS 
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdHwVers_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)


	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	*socId = TxBuf.socId;
	*RfId = (BYTE)(TxBuf.rfId);
	*BbId = (BYTE)(TxBuf.bbId);

	*socVersion = (BYTE)TxBuf.socVersion;
	*RfVersion = (BYTE)TxBuf.rfVersion;
	*BbVersion = (BYTE)TxBuf.bbVersion;
	
	return err; 
}

int DutWlanApiClssHeader Cf_GetORVersion(BYTE OR_ID, BYTE *OR_Major, BYTE *OR_Minor, BYTE *OR_Cust)
{ 
      
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_OR_VERSION 
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdORVers_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
	TxBuf.OR_ID = OR_ID;
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	if(OR_Major) *OR_Major = TxBuf.majorVersion;
	if(OR_Minor) *OR_Minor = TxBuf.minorVersion;
	if(OR_Cust)	 *OR_Cust = TxBuf.customerId;

	return err; 
}

int DutWlanApiClssHeader Cf_SetRfVga( int extMode, int value, 
									DWORD PathId, 
									DWORD DeviceId) 
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_VGACTRL 
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdRfVgaCtrl_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action	= THIS_ACTION; 
  	
#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
  	TxBuf.pathId = PathId;
#endif // #if defined (_MIMO_)

	TxBuf.extMode	= extMode;
	TxBuf.value		= value;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	return err; 
}

int DutWlanApiClssHeader Cf_GetRfVga( int *extMode, int *value, 
										DWORD PathId, 
										DWORD DeviceId) 
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_VGACTRL 
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdRfVgaCtrl_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action	= THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
  	TxBuf.pathId = PathId;
#endif // #if defined (_MIMO_)
 
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	*extMode	= TxBuf.extMode;
	*value		= TxBuf.value;

   
	return err; 
}

int DutWlanApiClssHeader Cf_SetRfLcCap( int extMode, int value) 
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_LCCAPCTRL 
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdRfLcCapCtrl_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action	= THIS_ACTION; 
	TxBuf.extMode	= extMode;
	TxBuf.value		= value;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
  
	return err; 
}

int DutWlanApiClssHeader Cf_GetRfLcCap( int *extMode, int *value) 
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_LCCAPCTRL
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdRfLcCapCtrl_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action	= THIS_ACTION; 
 
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	*extMode	= TxBuf.extMode;
	*value		= TxBuf.value;

   
	return err; 
}

int DutWlanApiClssHeader Cf_SetRfXTal(int extension, int cal, DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RFXTAL_CTRL 
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdRfXTalCtrl_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action		= THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

	TxBuf.extension		= extension;
	TxBuf.XTAL_CAL		= cal;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	return err; 
}

int DutWlanApiClssHeader Cf_GetRfXTal(int *pExtension, int *pCal, DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RFXTAL_CTRL
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdRfXTalCtrl_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action	= THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)


	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	if(pExtension)	(*pExtension)		= TxBuf.extension;
	if(pCal)		(*pCal)				= TxBuf.XTAL_CAL;
   
	return err; 
}

int DutWlanApiClssHeader Dev_GetRssiNf(
				  int stop, 
				  int *count, int *rssi, int *SNR, int *NoiseFloor, 
				  DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RSSI_SN_NPWR
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdRssiSnNpwr_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

	TxBuf.stop = stop; 
   	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	 
	*count = TxBuf.sample_NPwr + TxBuf.sample_SNR;
	*rssi = TxBuf.RSSI_Calc;
	*SNR  = TxBuf.SNR;
	*NoiseFloor = TxBuf.NoiseFloor;

 	if (WifiIf_GetDebugLogFlag())
	{		
		DebugLogRite ("Recent One: SNR %d\t rssi %d\t NoiseFloor %d\n", 
			TxBuf.SNR_recent1, TxBuf.RSSI_recent1, TxBuf.NoiseFloor_recent1);
		DebugLogRite ("Average   : SNR %d\t rssi %d\t NoiseFloor %d\n", 
			TxBuf.SNR, TxBuf.RSSI_Calc, TxBuf.NoiseFloor);
	}

	return err; 
}  

int DutWlanApiClssHeader Dev_GetThermalSensorReading(int *pReadBack)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
//#define THIS_CMD	MFG_CMD_THERMALREADING
#define THIS_CMD	MFG_CMD_THERMALSENSOR_RB
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdPwrMode_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

	TxBuf.powerMode=*pReadBack;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	 

	if(pReadBack) 
		(*pReadBack) = TxBuf.powerMode;  

	if (WifiIf_GetDebugLogFlag())
		DebugLogRite ("Dev_GetThermalSensorReading: %02X\n", 
				TxBuf.powerMode);

	return err; 
}  


int DutWlanApiClssHeader Dev_ArmReadUlongArray(DWORD armAddr, 
						  DWORD *pBuf, 
						  DWORD sizeBytes, 
						  DWORD DeviceId)
{
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  

#define THIS_CMD	MFG_CMD_ARMREADULONGARRAY
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdArmReadUlongArray_t TxBuf = {0};

 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
	TxBuf.armAddr = armAddr;
#if defined (_MIMO_)
	TxBuf.DeviceId = DeviceId;
#endif //#if defined (_MIMO_)
	if( sizeBytes <= ARM_ULONG_ARRAY_SIZE )
	{
		TxBuf.sizeBytes = sizeBytes;
	}
	else
	{
		TxBuf.sizeBytes = ARM_ULONG_ARRAY_SIZE;
	}

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	memcpy(pBuf, TxBuf.buf, TxBuf.sizeBytes);

	return err; 

}

//#if defined(_ENGINEERING_)
int DutWlanApiClssHeader Dev_RunBbpSqDiag(DWORD sizeBytes, DWORD *pSqBufAddr)
{
 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_BBPSQDIAG
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET


	int err=0;
	mfg_CmdBbpSqDiag_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action	= THIS_ACTION; 
	TxBuf.sizeBytes = sizeBytes;
	TxBuf.trigTime = 0;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	*pSqBufAddr = TxBuf.sqStartAddr;

	return err; 

}
//#endif // #if defined(_ENGINEERING_)

int DutWlanApiClssHeader Dev_GetThermalComp(BOOL *pEnabled, 
											DWORD *pTempRefAtCal,
											DWORD *pTempRefSlope,
											DWORD *pSlopeNumerator, 
											DWORD *pSlopeNumerator1, 
											DWORD *pSlopeDenominator,
											DWORD *pCalibrationIntervalInMS, 
											DWORD *pPTargetDelta, 
											DWORD PathId,  
											DWORD DeviceId)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_THERMALCOMP
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdThermalComp_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
  	TxBuf.pathId = PathId;
#endif // #if defined (_MIMO_)

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	 

	if(pEnabled) 
		(*pEnabled) = TxBuf.enable; 
	if(pTempRefAtCal) 
		(*pTempRefAtCal) = TxBuf.tempRef;	//0x30;
	if(pTempRefSlope) 
		(*pTempRefSlope) = TxBuf.tempRefSlope;	//0x30;
	if(pCalibrationIntervalInMS) 
		(*pCalibrationIntervalInMS) = TxBuf.calInterval;	//500;
	if(pPTargetDelta) 
		(*pPTargetDelta) = TxBuf.PTargetDelta;	//1;
	if(pSlopeDenominator) 
		(*pSlopeDenominator) = TxBuf.slopeDenominator;	//5;
	if(pSlopeNumerator) 
		(*pSlopeNumerator) = TxBuf.slopeNumerator;	//27;
	if(pSlopeNumerator1) 
		(*pSlopeNumerator1) = TxBuf.slopeNumerator1;	//27;

	if (WifiIf_GetDebugLogFlag())
		DebugLogRite ("Dev_GetThermalComp: enable(%d)"
				" tempRefAtCal(0x%08X)  tempRefSlope(0x%08X) "
				" calInterval(%d) PTargetDelta(0x%08X)"
				" slopeNumerator(%d) slopeNumerator1(%d) slopeDenominator(%d)\n", 
				TxBuf.enable, TxBuf.tempRef, TxBuf.tempRefSlope, TxBuf.calInterval,  
				TxBuf.PTargetDelta, 
				TxBuf.slopeNumerator, TxBuf.slopeNumerator1, TxBuf.slopeDenominator);

	return err; 
}  

int DutWlanApiClssHeader Dev_GetThermalTest(BOOL *pEnabled, 
											DWORD *pPTarget,
											DWORD *pTempRef,
											DWORD *pSlopeNumerator, 
											DWORD *pSlopeDenominator,
											DWORD *pCalibrationIntervalInMS, 
											DWORD *pTemp,
											DWORD *pSlopeNumerator1, 
											DWORD *pSlopeDenominator1)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_THERMAL_TEST
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdThermalTest_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	 

	if(pEnabled) 
		(*pEnabled) = TxBuf.enable; 
	if(pTempRef) 
		(*pTempRef) = TxBuf.tempRef;	
	if(pPTarget) 
		(*pPTarget) = TxBuf.PTarget;	
	if(pCalibrationIntervalInMS) 
		(*pCalibrationIntervalInMS) = TxBuf.calInterval;	//500;
	if(pSlopeDenominator) 
		(*pSlopeDenominator) = TxBuf.slopeDenominator;	//5;
	if(pSlopeNumerator) 
		(*pSlopeNumerator) = TxBuf.slopeNumerator;	//27;
	if(pSlopeDenominator1) 
		(*pSlopeDenominator1) = TxBuf.slopeDenominator1;	//5;
	if(pSlopeNumerator1) 
		(*pSlopeNumerator1) = TxBuf.slopeNumerator1;	//27;
	if (pTemp)
		*pTemp = TxBuf.temp;

	if (WifiIf_GetDebugLogFlag())
		DebugLogRite ("Dev_GetThermalTest: enable(%d)"
				" PTarget(0x%08X)  tempRef(0x%08X)  calInterval(%d) "
				" slopeNumerator(%d) slopeDenominator(%d)\n"
				" temp(%d) "
				"slopeNumerator1(0x%08X)  slopeDenominator1(0x%08X)\n",
				TxBuf.enable, TxBuf.PTarget, TxBuf.tempRef, TxBuf.calInterval,  
				TxBuf.slopeNumerator, TxBuf.slopeDenominator,
				TxBuf.temp,
				TxBuf.slopeNumerator1, TxBuf.slopeDenominator1);

	return err; 
}  

int DutWlanApiClssHeader Dev_ThermalComp(BOOL enabled, 
						DWORD tempRefAtCal,
						DWORD tempRefSlope,
						DWORD SlopeNumerator, 
						DWORD SlopeNumerator1, 
						DWORD SlopeDenominator,
						DWORD CalibrationIntervalInMS, 
						DWORD PTargetDelta, 
						DWORD PathId,  
						DWORD DeviceId)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_THERMALCOMP
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdThermalComp_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
  	TxBuf.pathId = PathId;
#endif // #if defined (_MIMO_)

 	TxBuf.enable = enabled; 
	TxBuf.tempRef = tempRefAtCal;	//0x30;
	TxBuf.tempRefSlope = tempRefSlope;	//0x30;
	TxBuf.calInterval = CalibrationIntervalInMS;	//500;
	TxBuf.PTargetDelta = PTargetDelta;	//0x20;
	TxBuf.slopeDenominator = SlopeDenominator;	//5;
	TxBuf.slopeNumerator =	SlopeNumerator;	//27;
	TxBuf.slopeNumerator1 =	SlopeNumerator1;	//27;
  
	if (WifiIf_GetDebugLogFlag())
		DebugLogRite ("Dev_ThermalComp: enable(%d)"
				" tempRefAtCal(0x%08X) tempRefSlope(0x%08X)  calInterval(%d) PTargetDelta(0x%08X)"
				" slopeNumerator(%d) slopeNumerator1(%d) slopeDenominator(%d)\n", 
				TxBuf.enable, TxBuf.tempRef, TxBuf.tempRefSlope, TxBuf.calInterval,  
				TxBuf.PTargetDelta, 
				TxBuf.slopeNumerator, TxBuf.slopeNumerator1, TxBuf.slopeDenominator);

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	 
	return err; 
}  

int DutWlanApiClssHeader Dev_ThermalTest(BOOL enabled, 
				DWORD PTarget,
				DWORD tempRef, 
				DWORD SlopeNumerator, 
				DWORD SlopeDenominator,
				DWORD CalibrationIntervalInMS,
				DWORD Temp,
				DWORD SlopeNumerator1, 
				DWORD SlopeDenominator1)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_THERMAL_TEST
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdThermalTest_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

 	TxBuf.enable = enabled; 
	TxBuf.PTarget = PTarget;
	TxBuf.tempRef = tempRef;	
	TxBuf.slopeDenominator = SlopeDenominator;	
	TxBuf.slopeNumerator =	SlopeNumerator;	
	TxBuf.calInterval = CalibrationIntervalInMS;	
 	TxBuf.temp = Temp;	
	TxBuf.slopeDenominator1 = SlopeDenominator1;	
	TxBuf.slopeNumerator1 =	SlopeNumerator1;	
 
	if (WifiIf_GetDebugLogFlag())
		DebugLogRite ("Dev_ThermalTest: enable(%d)"
				" tempRef(0x%08X)  calInterval(%d) "
				" slopeNumerator(%d) slopeDenominator(%d)"
				" temp(0x%08X)   "
				" slopeNumerator(%d) slopeDenominator(%d)\n", 
				TxBuf.enable, TxBuf.tempRef, TxBuf.calInterval,  
				TxBuf.slopeNumerator, TxBuf.slopeDenominator,
				TxBuf.temp,  
				TxBuf.slopeNumerator1, TxBuf.slopeDenominator1);

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	 
	return err; 
}  

#if defined (_SUPPORT_PM_)
int DutWlanApiClssHeader Dev_WritePMReg(int addr, int value)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_PM_REG
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	host_CmdRfReg_t TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
 	TxBuf.data  = value; 
	TxBuf.address = addr;
   	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	return err; 
}  

int DutWlanApiClssHeader Dev_ReadPMReg(int addr, int *value)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_PM_REG
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	host_CmdRfReg_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
	TxBuf.address = addr;
   	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

 	*value  = TxBuf.data; 

	return err; 
}  

int DutWlanApiClssHeader Dev_SetPMRail(int railId, int mode, int volt, int powerDown)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_PM_RAIL
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_PM_Cfg_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
	TxBuf.RailID = railId;
	TxBuf.RailMode = mode;
	TxBuf.Volt = volt;
	TxBuf.PowerDownMode = powerDown;
   	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	return err; 
}  
#endif // #if defined (_SUPPORT_PM_)

#if defined (_SUPPORT_LDO_)
int DutWlanApiClssHeader Dev_SetLDOConfig(int LDOSrc)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_LDO_CFG
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdLDOConfig_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
 	TxBuf.enable = 1; 
	TxBuf.LDOSrc = LDOSrc;
   	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	return err; 
}  

int DutWlanApiClssHeader Dev_GetLDOConfig(int* LDOSrc)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_LDO_CFG
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdLDOConfig_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
 	TxBuf.enable = 0; 
	TxBuf.LDOSrc = *LDOSrc;
   	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	*LDOSrc = TxBuf.LDOSrc;
	 
	return err; 
}  
#endif // #if defined (_SUPPORT_LDO_)

int DutWlanApiClssHeader Dev_SetRssiNf(DWORD DeviceId)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RSSI_SN_NPWR
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdRssiSnNpwr_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

   	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	 
	return err; 
}  

int DutWlanApiClssHeader Dev_SoftReset(DWORD DeviceId)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_SOFTRESET
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdSoftResetHW_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)

  	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	 
	return err; 
}  

int DutWlanApiClssHeader Dev_HwResetRf(DWORD DeviceId)
{ 
#if defined (_W8688_) || defined( _W8682_)

#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_HWRESETRF
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdSoftResetHW_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
   	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	 

	return err;
#else
	int err=0;
	ULONG temp=0;
	unsigned char temp1;
	BYTE chipRev=0;

	//change for A0 only
	PeekRegDword(0x80002018, &temp, 0);
	chipRev = (BYTE)temp&0xFF;
	if (chipRev >= CHIP_REV_A0)
	{
		//preserve RCal value
		err = ReadRfReg(0x65, &temp1, DeviceId);
		if (temp1 & 0x8)
		{
			//has alread been saved, do nothing.
		}
		else
		{
			err = ReadRfReg(0x55, &temp1, DeviceId);
			err = WriteRfReg(0x65, 0x8, DeviceId);
			err = WriteRfReg(0x67, (temp1<<4),DeviceId);
		}
	}

	err = Cf_RfPeek(0x2,&temp, DeviceId);
	temp &= ~(1<<2);
	if(err) return err;

	err = Cf_RfPoke(0x2,temp, DeviceId);
	temp &= ~1;
	if(err) return err;
	
	err = Cf_RfPoke(0x2,temp, DeviceId);
	temp |= 1;
	if(err) return err;
	
	err = Cf_RfPoke(0x2,temp, DeviceId);
	return err;

#endif
}  

/*
int DutWlanApiClssHeader Dev_SetRfPowerUseCal(int *pPwr4Pa , int RateG, 
						DWORD PathId,  
						DWORD DeviceId)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RFPWRCALED
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdRfPowCaled_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
   	TxBuf.Pwr4Pa = pPwr4Pa? *pPwr4Pa:15;
	TxBuf.RateG = RateG; 
#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
  	TxBuf.pathId = PathId;
#endif // #if defined (_MIMO_)
 	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	 
	return err; 
}  

*/

int DutWlanApiClssHeader Dev_SetRfPowerUseCal(int *pPwr4Pa , int RateG, 
										BYTE* cal_data_ext, int len,
										DWORD PathId,  
										DWORD DeviceId)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RFPWRCALED
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdRfPowCaled_t	*pTxBuf = NULL;
   
	if(NULL == cal_data_ext) len=0;

 	seqnum++; 
	pTxBuf = (mfg_CmdRfPowCaled_t*) malloc(sizeof(mfg_CmdRfPowCaled_t)+len);
	memset((void*)pTxBuf, 0, sizeof(mfg_CmdRfPowCaled_t)+len);

	Cf_StuffHeader(pTxBuf, THIS_CMD, sizeof(mfg_CmdRfPowCaled_t)+len); 
 	pTxBuf->action = THIS_ACTION; 
   	pTxBuf->Pwr4Pa = pPwr4Pa? *pPwr4Pa:15;
	pTxBuf->RateG = RateG; 
#if defined (_MIMO_)
   	pTxBuf->deviceId = DeviceId;
  	pTxBuf->pathId = PathId;
#endif // #if defined (_MIMO_)
	pTxBuf->calDataLen = len;
	memcpy(pTxBuf->calDataExt , cal_data_ext,len); 
 	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR)pTxBuf, sizeof(mfg_CmdRfPowCaled_t)+len, THIS_RSP);
	 
	return err; 
}  

int DutWlanApiClssHeader Dev_SetRfControlMode(DWORD ThermalOnly, DWORD DeviceId)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RF_POW_CTRL_LOOPMODE
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdRfPwrCloseLoopMode_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
 #if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)
  	TxBuf.ThermalControlOnly = ThermalOnly;
  	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	 
	return err; 
}  

int DutWlanApiClssHeader Dev_GetRfControlMode(DWORD *pThermalOnly, DWORD DeviceId)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RF_POW_CTRL_LOOPMODE
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdRfPwrCloseLoopMode_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
#endif // #if defined (_MIMO_)
   	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	*pThermalOnly = TxBuf.ThermalControlOnly;
 
	return err; 
}  


int DutWlanApiClssHeader Dev_ForceE2PromType(DWORD IfType, 
											DWORD AddrWidth, 
											DWORD DeviceType)
{
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  

#define THIS_CMD	MFG_CMD_E2P_TYPE
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int				err;
	mfg_CmdE2PType_t 	TxBuf = {0};

 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
	TxBuf.IfType = IfType;
	TxBuf.AddrWidth = AddrWidth;
	TxBuf.DeviceType = DeviceType;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	return err; 
}

int DutWlanApiClssHeader Dev_GetE2PromType(DWORD *pIfType, 
											DWORD *pAddrWidth,
											DWORD *pDeviceType)
{
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  

#define THIS_CMD	MFG_CMD_E2P_TYPE
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int				err;
	mfg_CmdE2PType_t 	TxBuf = {0};

 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	if(pIfType)
		*pIfType = TxBuf.IfType;
	if(pAddrWidth)
		*pAddrWidth = TxBuf.AddrWidth;
	if(pDeviceType)
		*pDeviceType = TxBuf.DeviceType;

	return err; 
}

int DutWlanApiClssHeader Dev_SetFEM(DWORD FE_VER,
								DWORD FEMCap2G,
								DWORD FEMCap5G,
								DWORD FEM_INTERNAL_BT,
								DWORD FEM_ANT_DIV,
								DWORD FEM_EXTERNAL_2G_LNA,
								DWORD FEM_EXTERNAL_5G_LNA,
								DWORD FEM_SAMEANT_2G_BT,
								DWORD FEM_SAMEANT_5G_BT,
								DWORD CONCURRENT_CAP_2G_BT,
								DWORD CONCURRENT_CAP_5G_BT,
								BYTE *Front_End_2G,
								BYTE *Front_End_5G,
								DWORD AntDiversityInfo)
{
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  

#define THIS_CMD	MFG_CMD_BCA_SYSTEM_CONFIG
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int				err, i=0;
	mfg_CmdFEM_t 	TxBuf = {0};
	Capabilities_VD *Capabilities;

	Capabilities = (Capabilities_VD*)&(TxBuf.Capability);
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

	TxBuf.FE_VER = FE_VER;
	Capabilities->Capable_2G=FEMCap2G;
	Capabilities->Capable_5G=FEMCap5G;
	Capabilities->InternalBluetooth=FEM_INTERNAL_BT;
	Capabilities->Antenna_Diversity=FEM_ANT_DIV;
	Capabilities->LNA_2G=FEM_EXTERNAL_2G_LNA;
	Capabilities->LNA_5G=FEM_EXTERNAL_5G_LNA;
	Capabilities->SameAnt_2G_BT=FEM_SAMEANT_2G_BT;
	Capabilities->SameAnt_5G_BT=FEM_SAMEANT_5G_BT;
	TxBuf.CONCURRENT_CAP_2G_BT=CONCURRENT_CAP_2G_BT;
	TxBuf.CONCURRENT_CAP_5G_BT=CONCURRENT_CAP_5G_BT;
	for (i=0; i<9; i++)
	{
		TxBuf.Front_End_2G[i]=(DWORD)Front_End_2G[i];
		TxBuf.Front_End_5G[i]=(DWORD)Front_End_5G[i];	
	}
	TxBuf.AntDiversityInfo = AntDiversityInfo;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	return err; 
}

int DutWlanApiClssHeader Dev_ReloadCalData()
{
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RELOAD_CALDATA
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int				err;
 	mfg_Cmd_t	TxBuf = {0};

 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	  
	return err; 
}
 
int DutWlanApiClssHeader Dev_SetRfPwrInitP(	BYTE InitP, 
						DWORD PathId,	
						DWORD DeviceId)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RF_CALPWRFIELDS
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int				err;
 	mfg_CmdRfPwrFields_t	TxBuf = {0};

 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
	TxBuf.fieldType = FIELD_INIP_SIGNALB; 
	TxBuf.deviceId =DeviceId; 
	TxBuf.pathId = PathId; 
	TxBuf.value1 = InitP;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	  
	return err; 
}

int DutWlanApiClssHeader Dev_GetRfPwrVgaMax(BYTE *pVgaMax, 
											DWORD PathId,	
											DWORD DeviceId) 
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RF_CALPWRFIELDS
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int				err;
 	mfg_CmdRfPwrFields_t	TxBuf = {0};

  
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
	TxBuf.fieldType = FIELD_VGAMAX; 
	TxBuf.deviceId =DeviceId; 
	TxBuf.pathId = PathId; 

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	if(pVgaMax)	*pVgaMax	= (BYTE)TxBuf.value1; 
 
	return err; 
}	

int DutWlanApiClssHeader Dev_DownLoadStaticSettingsToFw( DWORD BandId, DWORD SubbandId, 
											   DWORD PathId, 
 									BYTE RfLnaGrif,
 									BYTE RfLnaGrBb,
							//		BYTE BbLnaExtHo,
 							//		BYTE BbLnaExtLi,
							//		BYTE BbLnaGainOffset, 
									DWORD DeviceId)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_LDTXRXRFSETTINGS 
#define THIS_RSP	RSP_CODE(THIS_CMD) 
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdRxLnaSettings_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION;  
 	TxBuf.deviceId =DeviceId; 
	TxBuf.pathId = PathId;
	TxBuf.BandId = BandId;
	TxBuf.SubBandId = SubbandId;
	TxBuf.Rf_RxGainBb = RfLnaGrBb;
	TxBuf.Rf_RxGainIf = RfLnaGrif;
 	TxBuf.Bbu_RxGainHi = 0; 
 	TxBuf.Bbu_RxGainLo = 0; 
 	TxBuf.Bbu_RxGainOffset = 0; 

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	 
	return err; 
}  


int DutWlanApiClssHeader Dev_UpLoadStaticSettingsFromFw( DWORD BandId, DWORD SubbandId, 
											   DWORD PathId, 
 									BYTE *pRfLnaGrif,
 									BYTE *pRfLnaGrBb,
							//		BYTE BbLnaExtHo,
 							//		BYTE BbLnaExtLi,
							//		BYTE BbLnaGainOffset, 
									DWORD DeviceId)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_LDTXRXRFSETTINGS 
#define THIS_RSP	RSP_CODE(THIS_CMD) 
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdRxLnaSettings_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
 	TxBuf.deviceId =DeviceId; 
	TxBuf.pathId = PathId;
	TxBuf.BandId = BandId;
	TxBuf.SubBandId = SubbandId;
 
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	 
 	if(pRfLnaGrBb) (*pRfLnaGrBb) = (BYTE)TxBuf.Rf_RxGainBb;
	if(pRfLnaGrif) (*pRfLnaGrif) = (BYTE)TxBuf.Rf_RxGainIf;
   	 
	return err; 
}  
 

int DutWlanApiClssHeader Dev_GetRfPwrTbl(int band, int SubBandId,   
								int *sizeOfTble, BYTE *tbl, 
									DWORD PathId,
									DWORD DeviceId)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_LD_PWRTBL
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdLoadPwrTbl_t	*pTxBuf = NULL;
   
    pTxBuf  = (mfg_CmdLoadPwrTbl_t*)malloc(sizeof(mfg_CmdLoadPwrTbl_t)+(*sizeOfTble)); 
	memset (pTxBuf, 0, sizeof(mfg_CmdLoadPwrTbl_t) + (*sizeOfTble));

	Cf_StuffHeader(pTxBuf, THIS_CMD, sizeof((*pTxBuf))+ (*sizeOfTble)); 
	pTxBuf->action = THIS_ACTION; 
  	pTxBuf->deviceId = DeviceId; 
  	pTxBuf->pathId = PathId;
  	pTxBuf->bandId = band;
	pTxBuf->subbandId = SubBandId;
 	pTxBuf->sizeOfTblInByte = *sizeOfTble;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR)(pTxBuf), sizeof ((*pTxBuf))+(*sizeOfTble), RSP_CODE(THIS_CMD));
	 
	if(*sizeOfTble < (BYTE)pTxBuf->sizeOfTblInByte) 
		err = ERROR_ERROR;
 	if(sizeOfTble) *sizeOfTble = pTxBuf->sizeOfTblInByte;
	if(tbl) 
		memcpy(tbl, pTxBuf->tbl,  *sizeOfTble);
	
	free(pTxBuf);

	return err; 
}  

int DutWlanApiClssHeader Dev_SetRfPwrTbl(int band, int SubBandId,  
								int sizeOfTble, BYTE *tbl, 
									DWORD PathId,
									DWORD DeviceId)  
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_LD_PWRTBL
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdLoadPwrTbl_t	*pTxBuf = {0};
   
    pTxBuf  = (mfg_CmdLoadPwrTbl_t*)malloc(sizeof(mfg_CmdLoadPwrTbl_t)+sizeOfTble); 
	memset ((char*)pTxBuf, 0, (sizeof(mfg_CmdLoadPwrTbl_t) +sizeOfTble));

	Cf_StuffHeader(pTxBuf, THIS_CMD, sizeof((*pTxBuf))+ (sizeOfTble)); 
 	pTxBuf->action = THIS_ACTION; 
  	pTxBuf->deviceId = DeviceId; 
  	pTxBuf->pathId = PathId;
  	pTxBuf->bandId = band;
	pTxBuf->subbandId = SubBandId;
	pTxBuf->maxPower = (BAND_802_11A == band)?POW_MAX_A:POW_MAX_DEF_G;
	pTxBuf->minPower = (BAND_802_11A == band)?POW_MIN_A:POW_MIN_DEF_G;;
	pTxBuf->sizeOfTblInByte = sizeOfTble;
	if(tbl) 
		memcpy(pTxBuf->tbl, tbl, pTxBuf->sizeOfTblInByte);
		
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR)pTxBuf, sizeof (*pTxBuf)+(sizeOfTble), THIS_RSP);
	 
	free(pTxBuf);

	return err; 
}  

int DutWlanApiClssHeader Dev_LoadCustomizedSettings(void) 
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_LOADSETS_EEPROM
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_Cmd_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action	= THIS_ACTION; 
 
 err = query1  ( OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	 
	return err; 
}

int DutWlanApiClssHeader Dev_SetCustomizedSettings(BYTE* cal_data_ext, int len)
{
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_CALDATA_EXT
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET
	int err=0;
	mfg_CmdCalDataExt_t	*pTxBuf = NULL;
   
	if(NULL == cal_data_ext) len=0;

 	seqnum++; 
	pTxBuf = (mfg_CmdCalDataExt_t*) malloc(sizeof(mfg_CmdCalDataExt_t)+len);
	memset((void*)pTxBuf, 0, sizeof(mfg_CmdCalDataExt_t)+len);

	Cf_StuffHeader(pTxBuf, THIS_CMD, sizeof(mfg_CmdCalDataExt_t)+len); 
 	pTxBuf->action = THIS_ACTION; 
	pTxBuf->calDataLen = len;
	memcpy(pTxBuf->calDataExt , cal_data_ext,len);  	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR)pTxBuf, sizeof(mfg_CmdCalDataExt_t)+len, THIS_RSP);
    free(pTxBuf);

	return err;

}
int DutWlanApiClssHeader Dev_GetCustomizedSettings(int *Setting)
{
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_CALDATA_EXT
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET
		int err=0;
	mfg_CmdCalDataExt_t	TxBuf;
   

 	seqnum++; 
	

	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(mfg_CmdCalDataExt_t)); 
 	TxBuf.action = THIS_ACTION;  	
	err = query1(OID_MRVL_MFG_COMMAND, (PUCHAR)&TxBuf, sizeof(mfg_CmdCalDataExt_t), THIS_RSP);
	*Setting =(int)TxBuf.caldatastatus;
	return err;
}
int DutWlanApiClssHeader Dev_SetMACAddress(BYTE* ByteArray)
{
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_MAC_ADD
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	int i = 0;
	mfg_CmdMacAdd_t	TxBuf = {0};
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
	for (i=0; i<6; i++)
	{
		TxBuf.macAddress[i] = ByteArray[i];
	}
   	
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	return err;
}

int DutWlanApiClssHeader Dev_GetMACAddress(BYTE* ByteArray)
{
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_MAC_ADD
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	int i = 0;
	mfg_CmdMacAdd_t	TxBuf = {0};
 	
	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	for (i=0; i<6; i++)
	{
		ByteArray[i] = TxBuf.macAddress[i];
	}
   	
	return err;
}

int DutWlanApiClssHeader Cf_TxFastCal(BYTE *DataStruc, int len)
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_TX_FASTCAL 
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
 	seqnum++;

	if(DataStruc)
	{
		mfg_CmdTxFastCal_t	*pTxBuf = NULL;
		pTxBuf  = (mfg_CmdTxFastCal_t*)malloc(sizeof(mfg_CmdTxFastCal_t) + len);
		memset ((char*)pTxBuf, 0,			 (sizeof(mfg_CmdTxFastCal_t) + len));

		Cf_StuffHeader(pTxBuf, THIS_CMD, sizeof(mfg_CmdTxFastCal_t)+len);
		pTxBuf->action = THIS_ACTION;
		pTxBuf->DataStrucLen = len;
		memcpy((char*)&(pTxBuf->DataStrucPtr), DataStruc, len);

		err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) pTxBuf, sizeof(mfg_CmdTxFastCal_t)+len, THIS_RSP);

		if (pTxBuf) free (pTxBuf);
	}
	else
	{
		err = 1;
	}

	return err;
}

int DutWlanApiClssHeader Cf_TxFastVer(BYTE *DataStruc, int len)
{ 
#undef	THIS_CMD
#undef	THIS_RSP
#undef	THIS_ACTION
 
#define THIS_CMD	MFG_CMD_TX_FASTVER
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
 	seqnum++;

	if(DataStruc)
	{
		mfg_CmdTxFastCal_t	*pTxBuf = NULL;
		pTxBuf  = (mfg_CmdTxFastCal_t*)malloc(sizeof(mfg_CmdTxFastCal_t) + len);
		memset ((char*)pTxBuf, 0,			 (sizeof(mfg_CmdTxFastCal_t) + len));

		Cf_StuffHeader(pTxBuf, THIS_CMD, sizeof(mfg_CmdTxFastCal_t)+len);
		pTxBuf->action = THIS_ACTION;
		pTxBuf->DataStrucLen = len;
		memcpy((char*)&(pTxBuf->DataStrucPtr), DataStruc, len);

		err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) pTxBuf, sizeof(mfg_CmdTxFastCal_t)+len, THIS_RSP);

		if (pTxBuf) free (pTxBuf);
	}
	else
	{
		err = 1;
	}

	return err;
}

int DutWlanApiClssHeader Cf_RxFastVer(BOOL enable, int channel, BYTE *Bssid, char *SSID)
{      
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RX_FASTVER
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdRxFastVer_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

 	TxBuf.enable = enable; 
	TxBuf.channel = channel;
	if (Bssid)
		memcpy(TxBuf.Bssid , Bssid, sizeof(TxBuf.Bssid));
	if (SSID)
		memcpy(TxBuf.Ssid ,  SSID,  sizeof(TxBuf.Ssid));
 
	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	
	return err;
}

int DutWlanApiClssHeader Dev_SetExtLnaNsettings(int mode, int LnaPolority, 
                                            BYTE Bbu_Hi, BYTE Bbu_Lo,
											BYTE Bbu_GainOffset,
											BYTE Rfu_GrBb, BYTE Rfu_GrLpf) 
{
#if 1
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD  	MFG_CMD_LNA_EXT_SETTING	        

#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	int i = 0;
	mfg_LnaModeSetting_t	TxBuf = {0};
 	
	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION;
    TxBuf.extMode = mode;
	//TxBuf.LnaPolority = LnaPolority;
	//TxBuf.Bbu_Hi = Bbu_Hi; 
    //TxBuf.Bbu_Lo = Bbu_Lo;
    //TxBuf.Bbu_GainOffset = Bbu_GainOffset;
    //TxBuf.Rfu_GrBb = Rfu_GrBb; 
    //TxBuf.Rfu_GrLpf = Rfu_GrLpf;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
    return err;
	
#else

	int err=0;
    DWORD tempDw=0;
    BYTE    tempB=0;

// mode soc 2044.31=mode
   err=PeekRegDword(0x80002044, &tempDw);
   tempDw &=(~(1<<31));
   tempDw |= ((mode&0x1)<<31);
   err=PokeRegDword(0x80002044, tempDw);

   // polority Bbu28.7
   err=ReadBBReg(0x28, &tempB);
   tempB &=(~(1<<7));
   tempB |= ((LnaPolority&0x1)<<7);
   err=WriteBBReg(0x28, tempB);

// Bbu/Rfu settings

   err=WriteBBReg(0xEB, Bbu_Hi);
   err=WriteBBReg(0xEC, Bbu_Lo);
   err=WriteBBReg(0xEF, Bbu_GainOffset);
    
   err=WriteRfReg(0x33, Rfu_GrBb);
   err=WriteRfReg(0x32, Rfu_GrLpf);
    return err;
#endif
}

int DutWlanApiClssHeader Dev_GetExtLnaNsettings(int *pMode, int *pLnaPolority, 
											BYTE *pBbu_Hi, BYTE *pBbu_Lo,
											BYTE *pBbu_GainOffset,
											BYTE *pRfu_GrBb, BYTE *pRfu_GrLpf)
{
#if 1
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD  	MFG_CMD_LNA_EXT_SETTING	        

#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	int i = 0;
	mfg_LnaModeSetting_t	TxBuf = {0};
 	
	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
    if(err) return err;

    if(pMode)   (*pMode) = TxBuf.extMode;
    if(pLnaPolority)    
        (*pLnaPolority) = 0;

    if(pBbu_Hi) (*pBbu_Hi) = 0; 
    if(pBbu_Lo) (*pBbu_Lo) = 0;
    if(pBbu_GainOffset) 
        (*pBbu_GainOffset) = 0;

    if(pRfu_GrBb)   (*pRfu_GrBb) = 0; 
    if(pRfu_GrLpf)  (*pRfu_GrLpf) = 0;
//    if(pLnaPolority)    
//        (*pLnaPolority) = TxBuf.LnaPolority;

//    if(pBbu_Hi) (*pBbu_Hi) = (BYTE) TxBuf.Bbu_Hi; 
//    if(pBbu_Lo) (*pBbu_Lo) = (BYTE) TxBuf.Bbu_Lo;
//    if(pBbu_GainOffset) 
//        (*pBbu_GainOffset) = (BYTE) TxBuf.Bbu_GainOffset;

//    if(pRfu_GrBb)   (*pRfu_GrBb) = (BYTE) TxBuf.Rfu_GrBb; 
//    if(pRfu_GrLpf)  (*pRfu_GrLpf) = (BYTE) TxBuf.Rfu_GrLpf;

    return err;
	
#else

    int err=0;
    DWORD tempDw=0;
    BYTE    tempB=0;

// mode soc 2044.31=mode
    if(pMode)
    {
        err=PeekRegDword(0x80002044, &tempDw);
        tempDw &=(1<<31);
        *pMode = tempDw>>31;
    }
   // polority Bbu28.7
    if(pLnaPolority) 
    {
        err=ReadBBReg(0x28, &tempB);
        tempB &=(1<<7);
        *pLnaPolority = tempB>>7;
    }
// Bbu/Rfu settings

   if(pBbu_Hi)  err=ReadBBReg(0xEB, pBbu_Hi);
   if(pBbu_Lo)  err=ReadBBReg(0xEC, pBbu_Lo);
   if(pBbu_GainOffset)  
       err=ReadBBReg(0xEF, pBbu_GainOffset);
    
   if(pRfu_GrBb)    err=ReadRfReg(0x33, pRfu_GrBb);
   if(pRfu_GrLpf)   err=ReadRfReg(0x32, pRfu_GrLpf);
    return err;
#endif
}
/*
int DutWlanApiClssHeader Dev_CloseLoopCal(
				   BYTE targetInitP, int maxThreshHold, int steps, int dataRate,
				   BYTE *initP, BYTE *threshHi, BYTE *threshLow) 
{
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_CLOSE_LOOP_CAL
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdCloseLoopCal	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION;
	TxBuf.targetInitP = targetInitP; 
	TxBuf.maxThreshHold = maxThreshHold; 
	TxBuf.steps = steps; 
	TxBuf.dataRate = dataRate; 
 

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

 	if (WifiIf_GetDebugLogFlag()) 
		DebugLogRite ("CloseLoopCal: \nInitP \t0x%x \n   HighThreshhold \t0x%x \n LowThreshold \t0x%x\n" , 
						TxBuf.initP, TxBuf.threshHi, TxBuf.threshLow);

	if(initP) *initP = (BYTE)TxBuf.initP;
	if(threshHi) *threshHi = (BYTE)TxBuf.threshHi;
	if(threshLow) *threshLow = (BYTE)TxBuf.threshLow;

	return err; 
}
*/ //jsz
int DutWlanApiClssHeader GetTxIQValue(int *pExtMode  , DWORD *pAmp, DWORD *pPhase, 
											  DWORD PathId,  
											  DWORD DeviceId)
{ 
      
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_TXIQSETTING 
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdTxRxIq_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
  	TxBuf.pathId = PathId;
#endif // #if defined (_MIMO_)

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	if(pExtMode) (*pExtMode) = TxBuf.extMode;
	if(pAmp)	(*pAmp) = TxBuf.Alpha;
	if(pPhase)  (*pPhase) = TxBuf.Beta;

	return err; 
}

int DutWlanApiClssHeader SetTxIQValue(int ExtMode  , DWORD Amp, DWORD Phase, 
											  DWORD PathId,  
											  DWORD DeviceId)
{ 
      
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_TXIQSETTING 
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdTxRxIq_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action	= THIS_ACTION; 
#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
  	TxBuf.pathId = PathId;
#endif // #if defined (_MIMO_)
	TxBuf.extMode	= ExtMode;
	TxBuf.Alpha		= Amp;
	TxBuf.Beta		= Phase;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	return err; 
}

int DutWlanApiClssHeader GetTxIQEstimatedValue(DWORD *pAmp, DWORD *pPhase)
{ 
      
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_TXIQESTVALUE 
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdTxRxIq_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	if(pAmp)	(*pAmp) = TxBuf.Alpha;
	if(pPhase)  (*pPhase) = TxBuf.Beta;

	return err; 
}

///////////
int DutWlanApiClssHeader GetRxIQ(int *pExtMode  , DWORD *pAmp, DWORD *pPhase, 
							DWORD PathId, DWORD DeviceId)
{ 
      
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RXIQSETTING 
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdTxRxIq_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
  	TxBuf.pathId = PathId;
#endif // #if defined (_MIMO_)

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	if(pExtMode) (*pExtMode) = TxBuf.extMode;
	if(pAmp)	(*pAmp) = TxBuf.Alpha;
	if(pPhase)  (*pPhase) = TxBuf.Beta;

	return err; 
}

int DutWlanApiClssHeader SetRxIQ(int ExtMode  , DWORD Amp, DWORD Phase, 
							DWORD PathId, DWORD DeviceId)
{ 
      
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RXIQSETTING 
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;
	mfg_CmdTxRxIq_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action	= THIS_ACTION; 
#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
  	TxBuf.pathId = PathId;
#endif // #if defined (_MIMO_)
	TxBuf.extMode	= ExtMode;
	TxBuf.Alpha		= Amp;
	TxBuf.Beta		= Phase;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	return err; 
}


int DutWlanApiClssHeader GetRxIQEstimated(DWORD *pAmp, DWORD *pPhase, 
							DWORD PathId, DWORD DeviceId)
{ 
      
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_RXIQESTVALUE 
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;
	mfg_CmdTxRxIq_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
#if defined (_MIMO_)
   	TxBuf.deviceId = DeviceId;
  	TxBuf.pathId = PathId;
#endif // #if defined (_MIMO_)

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	if(pAmp)	(*pAmp) = TxBuf.Alpha;
	if(pPhase)  (*pPhase) = TxBuf.Beta;

	return err; 
}

int DutWlanApiClssHeader PerformTxIQCal(DWORD Avg,
										DWORD *pTxAmp, DWORD *pTxPhase )
{ 
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_TXRXIQCALMODE 
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int err=0;

	mfg_CmdTxRxCal_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 
 
	TxBuf.Avg = Avg;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	if(pTxAmp)		(*pTxAmp) = TxBuf.TxIqAlpha;
	if(pTxPhase)	(*pTxPhase) = TxBuf.TxIqBeta; 

	return err; 
}

int DutWlanApiClssHeader PerformTxRxIQCal(DWORD Avg,
										DWORD *pTxAmp, DWORD *pTxPhase, 
													 DWORD *pRxAmp, DWORD *pRxPhase)
{ 


      
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_TXRXIQCALMODE 
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int err=0;

	mfg_CmdTxRxCal_t	TxBuf = {0};
   
 	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

	TxBuf.Avg = Avg;

 	if(pTxAmp)	TxBuf.TxIqAlpha = (*pTxAmp);
	if(pTxPhase)TxBuf.TxIqBeta = (*pTxPhase);

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);

	if(pTxAmp)		(*pTxAmp) = TxBuf.TxIqAlpha;
	if(pTxPhase)	(*pTxPhase) = TxBuf.TxIqBeta;

	if(pRxAmp)		(*pRxAmp) = TxBuf.RxIqAlpha;
	if(pRxPhase)	(*pRxPhase) = TxBuf.RxIqBeta;

	return err; 
}


//DPD API
#if defined(_W8782_)

int DutWlanApiClssHeader Dev_SetDPD(DWORD PowerID, DWORD Memory2Row, 
									DWORD *pDPDTbl, DWORD DeviceId)
{
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
 
#define THIS_CMD	MFG_CMD_DPD_INFO 
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_SET

	int i, err=0;

	mfg_CmdDPDInfo_t	TxBuf = {0};

	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

	TxBuf.PowerID		= PowerID;
	TxBuf.Memory2Row	= Memory2Row;
	TxBuf.deviceId		= DeviceId;
	if( pDPDTbl )
	{
		for(i=0; i<HAL_DPD_MAX_NUM_COEFF; i++)
		{
			TxBuf.pDPDTbl[i] = pDPDTbl[i];
		}
	}

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);


	return err; 
}

int DutWlanApiClssHeader Dev_GetDPD(DWORD PowerID, DWORD Memory2Row, 
									DWORD *pDPDTbl, DWORD DeviceId)
{
#undef	THIS_CMD	 
#undef	THIS_RSP	 
#undef	THIS_ACTION  
  
#define THIS_CMD	MFG_CMD_DPD_INFO 
#define THIS_RSP	RSP_CODE(THIS_CMD)
#define THIS_ACTION HostCmd_ACT_GEN_GET

	int i, err=0;

	mfg_CmdDPDInfo_t	TxBuf = {0};

	seqnum++; 
	Cf_StuffHeader(&TxBuf, THIS_CMD, sizeof(TxBuf)); 
 	TxBuf.action = THIS_ACTION; 

	TxBuf.PowerID		= PowerID;
	TxBuf.Memory2Row	= Memory2Row;
	TxBuf.deviceId		= DeviceId;

	err = query1 (OID_MRVL_MFG_COMMAND, (PUCHAR) &TxBuf, sizeof (TxBuf), THIS_RSP);
	if( pDPDTbl )
	{
		for(i=0; i<HAL_DPD_MAX_NUM_COEFF; i++)
		{
			pDPDTbl[i] = TxBuf.pDPDTbl[i];
		}
	}

	return err; 

}


#endif //#ifdef _W8782_
