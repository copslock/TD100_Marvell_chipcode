/** @file DutWlanApiClss.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include <stdio.h>
#ifndef _LINUX_
#include "../DutShareApi878XDll/stdafx.h"
#include <stdlib.h>
#include <time.h>
#include <direct.h>
#include "../../DutCommon/mathUtility.h"
#else	//_LINUX_
#include <string.h>
#include "DutWlanApi.hc"
extern int GetPrivateProfileString(char*,char*,char*, char*,int,char*);
#define _MAX_PATH 256
extern int DebugLogRite(const char*,...);
//#define TRUE 1
#endif	//#ifndef _LINUX_

#include "DutWlanApiClss.h"
#include "DutWlanApiClss.hd"
#include "../../DutCommon/mathUtility.h"
#include "../../DutCommon/utility.h"
#include "../DutApi878XDll/DutDllVerNo.h"

#include "time.h"

int DutWlanApiClssHeader SetExtLnaNsettings(int mode, int LnaPolority,
                       BYTE Bbu_Hi, BYTE Bbu_Lo,
                       BYTE Bbu_GainOffset,
                       BYTE Rfu_GrBb, BYTE Rfu_GrLpf)
{
	return Dev_SetExtLnaNsettings(mode, LnaPolority,
                       Bbu_Hi, Bbu_Lo,
                       Bbu_GainOffset,
                       Rfu_GrBb, Rfu_GrLpf);
}

int DutWlanApiClssHeader GetExtLnaNsettings(int *pMode, int *pLnaPolority,
                       BYTE *pBbu_Hi, BYTE *pBbu_Lo,
                       BYTE *pBbu_GainOffset,
                       BYTE *pRfu_GrBb, BYTE *pRfu_GrLpf)
{
	return Dev_GetExtLnaNsettings(pMode, pLnaPolority,
                       pBbu_Hi, pBbu_Lo,
                       pBbu_GainOffset,
                       pRfu_GrBb, pRfu_GrLpf);
}

int DutWlanApiClssHeader WriteCAUReg(WORD address, BYTE data,  
											   DWORD DeviceId)
{
 	return  Cf_CAUPoke((address), data, DeviceId);
}

int DutWlanApiClssHeader ReadCAUReg(WORD address, BYTE *data,  
											   DWORD DeviceId)
{
  	int status =0;
	DWORD temp=0; 

	status =   Cf_CAUPeek((address), &temp, DeviceId);
	*data = (BYTE)temp;
	return status;
}

int DutWlanApiClssHeader UpdateRFUFW(DWORD deviceId)
{
	int status =0;

	status =   Cf_UpdateRFUFW(deviceId);

	return status;
}

int DutWlanApiClssHeader LoadRFUFW(DWORD deviceId, char *FileName)
{
#define PACKET_LEN 0x100
  	int status =0;
	FILE* hFile;
	BYTE temp[PACKET_LEN]="";
	int i =0xf0;
	int j=0;
	char temp1[4]="";
	int offset = 0;
    int len = 0;
    char filename[]="rfu_fw.mem";

	if( NULL != FileName && 0 != strlen(FileName))
		strcpy(filename, FileName);

    if (filelength(filename)==0) 
    {
		DebugLogRite("File \"%s\" zero length!\n",filename);
        return 1;
    }

	//read the cal data content from file
	hFile = fopen(filename,"rb"); 
	if (!hFile)
	{
		DebugLogRite("File \"%s\" cannot be found!\n",filename);
		return 1;
	}
	else
	{
		DebugLogRite("File \"%s\". \n",filename);
	}
	
    i = 0;
	while(!feof(hFile))
	{
		len = BlockReadFromTextFile(temp, PACKET_LEN, hFile);
        DebugLogRite("offset 0x%04x(%6d) \tlen (%6d)\n", offset,offset,len);
		status = Cf_LoadRFUFW(deviceId, offset, len, (BYTE*)temp);
        if(status) break;
		offset += len;
	}
	fclose (hFile);

	return status;
}

int DutWlanApiClssHeader WriteRfReg(WORD address, BYTE data,  
											   DWORD DeviceId)
{
 	return  Cf_RfPoke((address), data, DeviceId);
}

int DutWlanApiClssHeader ReadRfReg(WORD address, BYTE *data,  
											   DWORD DeviceId)
{
  	int status =0;
	DWORD temp=0; 

	status =   Cf_RfPeek((address), &temp, DeviceId);
	*data = (BYTE)temp;
	return status;
}

int DutWlanApiClssHeader WriteBBReg(WORD address, BYTE data,  
											   DWORD DeviceId)
{
	return  Cf_BbpPoke((address&0xFFFF), data, DeviceId);
}

int DutWlanApiClssHeader ReadBBReg(WORD address, BYTE *data,  
											   DWORD DeviceId)
{
 	int status =0;
	DWORD temp=0; 

	status =   Cf_BbpPeek((address&0xFFFF), &temp, DeviceId);
	*data = (BYTE)temp;

	return status;
}

int DutWlanApiClssHeader PokeRegWord(DWORD address, WORD data,  
											   DWORD DeviceId)
{
	DWORD dataTemp=0;

	dataTemp = data;

	return PokeRegDword(address, dataTemp, DeviceId);
}

int DutWlanApiClssHeader PeekRegWord(DWORD address, WORD *data,  
											   DWORD DeviceId)
{
	DWORD dataTemp=0;
	int status=0;
	
	status = PeekRegDword(address, &dataTemp, DeviceId);
	if(ERROR_NONE != status)	return status;
	*data = (WORD)dataTemp; 
	return status;
}

int DutWlanApiClssHeader PokeRegByte(DWORD address, BYTE data,  
											   DWORD DeviceId)
{
	DWORD dataTemp=0;

	dataTemp = data;

	return PokeRegDword(address, dataTemp, DeviceId);
}

int DutWlanApiClssHeader PeekRegByte(DWORD address, BYTE *data,  
											   DWORD DeviceId)
{
	DWORD dataTemp=0;
	int status=0;
	
	status = PeekRegDword(address, &dataTemp, DeviceId);
	if(ERROR_NONE != status)	return status;
	*data = (BYTE)dataTemp; 
	return status;
}

int DutWlanApiClssHeader PokeRegDword(DWORD address, DWORD data,  
											   DWORD DeviceId) 
{	 
 	return Cf_Poke(address, data, DeviceId);
}

int DutWlanApiClssHeader PeekRegDword(DWORD address, DWORD *data,  
											   DWORD DeviceId) 
{ 
	return Cf_Peek(address, data, DeviceId);
}

int DutWlanApiClssHeader WriteMACReg(DWORD address, DWORD data,  
											   DWORD DeviceId)
{
	 
 	return Cf_Poke((REGBASE_MAC + (address&0xFFF)), data, DeviceId);
}

int DutWlanApiClssHeader ReadMACReg(DWORD address, DWORD *data,  
											   DWORD DeviceId)
{ 
	return Cf_Peek((REGBASE_MAC + (address&0xFFF)), data, DeviceId);

}

int DutWlanApiClssHeader WriteSocReg(DWORD address, DWORD data,  
											   DWORD DeviceId)
{
	 
 	return Cf_Poke(address, data, DeviceId);
}

int DutWlanApiClssHeader ReadSocReg(DWORD address, DWORD *data,  
											   DWORD DeviceId)
{ 
	return Cf_Peek(address, data, DeviceId);

}

int DutWlanApiClssHeader WriteSQmem(DWORD address, DWORD data,  
											   DWORD DeviceId)
{
	int status=0;

	status=PokeRegDword((REGBASE_SQ + (address&0xFFFF)), data, DeviceId);

	return status;
}

int DutWlanApiClssHeader ReadSQmem(DWORD address, DWORD *data,  
											   DWORD DeviceId)
{
	int status=0;

	status=PeekRegDword((REGBASE_SQ + (address&0xFFFF)), data, DeviceId);
 
	return ERROR_NONE; 
} 
/* 
int DutWlanApiClssHeader SetRfBbFiler(int channel) 
{
	return Cf_SetRfBbFiler(channel);
}
*/
 
#ifndef _LINUX_
DutWlanApiClssHeader DutWlanApiClss()
{
 	char temp[_MAX_PATH]="";

 	if(getcwd(temp, _MAX_PATH) != NULL)  
 	strcat(temp, "\\setup.ini");
 	else
	strcpy(temp, "setup.ini");

 		
#ifdef	_GOLDEN_REFERENCE_UNIT_
	GetPrivateProfileString("GoldenUnit",	"AdapterPrefix", "Marvell", adapter_prefix, sizeof(adapter_prefix), temp); 
#else //#ifdef	_GOLDEN_REFERENCE_UNIT_
	GetPrivateProfileString("Driver",		"AdapterPrefix", "Marvell", adapter_prefix, sizeof(adapter_prefix), temp); 
#endif //#ifdef	_GOLDEN_REFERENCE_UNIT_

}

DutWlanApiClssHeader ~DutWlanApiClss()
{

}
#endif	//#ifndef _LINUX_

int DutWlanApiClssHeader Version(void)
{ 
 	return	MFG_VERSION(DUT_DLL_VERSION_MAJOR1, DUT_DLL_VERSION_MAJOR2,
						DUT_DLL_VERSION_MINOR1, DUT_DLL_VERSION_MINOR2);
}

char* DutWlanApiClssHeader About(void)
{
	char tempDate[50]="", tempTime[50]="";
	char AboutFormat[512]=
			"Name:\t\tDutApiClass\n"
#ifdef _UDP_
			"Interface:\t""UDP\n"
#elif defined(_LINUX_)
//			"GSPI\n"
#elif defined (_WIFI_SDIO_)
			"Interface:\t""SD\n"
#elif defined (_IF_USB_)
			"Interface:\t""USB\n"
#else
			"Interface:\t""CF\n"
#endif	
			"Version:\t%d.%d.%d.%02d\n" 
#ifndef _LINUX_
			"Date:\t\t%s (%s)\n\n" 			
#else
			"Date:\t\t%s\n\n"
#endif
			"Note:\t\t\n" 
#ifdef STATIC_OBJ
			"Static Object\n" 
#endif			
			;
#ifdef _LINUX
	time_t timer;
#endif

	static char AboutString[512]=""; 

	sprintf(AboutString, AboutFormat, 
			DUT_DLL_VERSION_MAJOR1,	
			DUT_DLL_VERSION_MAJOR2,
			DUT_DLL_VERSION_MINOR1, 
			DUT_DLL_VERSION_MINOR2,
			__DATE__, __TIME__);

	return AboutString; 
}

int DutWlanApiClssHeader SetBandAG(int band,
				  DWORD DeviceId)
{
	int status=ERROR_NONE;
	status = Dev_SetBandAG(band, DeviceId);  
	return status;
}

int DutWlanApiClssHeader GetBandAG(int *band,
				  DWORD DeviceId)
{
	return Dev_GetBandAG(band,DeviceId);
}

// int set channel, set power, read RSSI, read .11status, 

int DutWlanApiClssHeader SetRfChannel(int channel,
				  DWORD DeviceId)
{
	int status=ERROR_NONE;

	status = Cf_SetRfChannel(channel, DeviceId);  
  
	return status;
}

int DutWlanApiClssHeader GetRfChannel(int *channel, double *pChFreqInMHz,
				  DWORD DeviceId)
{
	return Cf_GetRfChannel(channel, pChFreqInMHz, DeviceId);
}

int DutWlanApiClssHeader SetChannelBw(int ChannelBw , DWORD DeviceId) 

{
	return Dev_SetChannelBw(ChannelBw, DeviceId);
}
int DutWlanApiClssHeader GetChannelBw(int *ChannelBw,  DWORD DeviceId) 
{
	return Dev_GetChannelBw(ChannelBw, DeviceId);
}
int DutWlanApiClssHeader GetSubBand(int *SubBandId, int *HighFreq, int *LowFreq,  
					DWORD DeviceId) 
{
	return Dev_GetSubBand( SubBandId, HighFreq, LowFreq, DeviceId);
}

int DutWlanApiClssHeader GetRfChannelFreq(int band, int channel, double *chFreqInMhz,  
					DWORD DeviceId) 
{
	return Dev_GetRfChannelFreq(band, channel, chFreqInMhz, DeviceId);
}


int DutWlanApiClssHeader SetRfChannelByFreq(double chFreqInMhz, DWORD DeviceId)
{
	return Dev_SetRfChannelByFreq(chFreqInMhz, DeviceId);
}

int DutWlanApiClssHeader SetRfPower( 
				  int *pPwr4Pa, 
				  int *pCorrOffset, 
				  int *pCorr1A, 
					DWORD PathId,  
					DWORD DeviceId)
{
 
	return Cf_SetRfPower(pPwr4Pa,  pCorrOffset,  pCorr1A,  PathId,  DeviceId);
}

int DutWlanApiClssHeader StepRfPower(int *pSteps , 
				DWORD PathId,  
				DWORD DeviceId)
{
 	return Cf_StepRfPower(	pSteps, 
							PathId,  
							DeviceId);
}

int DutWlanApiClssHeader GetRfPowerSetting(BYTE *pPaConf1, BYTE *pPaConf2, 
						 BYTE *pVt_Hi_G,	 BYTE *pVt_Lo_G,		BYTE *pInitP_G, 
						 BYTE *pVt_Hi_B,	 BYTE *pVt_Lo_B,		BYTE *pInitP_B)
{
 	return Cf_GetRfPowerSetting(pPaConf1, pPaConf2, 
						 pVt_Hi_G,	 pVt_Lo_G,	pInitP_G, 
						 pVt_Hi_B,	 pVt_Lo_B,	pInitP_B);
}

 
int DutWlanApiClssHeader GetRfPower(int *pPwr4Pa,  
						int	*pCorrInitP_G,
						int	*pCorrInitP_B,
						int	*pCorrTh_G,
						int	*pCorrTh_B)
{
 	return Cf_GetRfPower(pPwr4Pa,  
							pCorrInitP_G,
							pCorrInitP_B,
							pCorrTh_G,
							pCorrTh_B);
}

int DutWlanApiClssHeader SetRfVga(int extMode, int value, 
				DWORD PathId, DWORD DeviceId)
{
	return  Cf_SetRfVga( extMode, value, 
				PathId, DeviceId);
}

int DutWlanApiClssHeader GetRfVga(int *pExtMode, int *pValue, 
				DWORD PathId, DWORD DeviceId)
{
	return  Cf_GetRfVga( pExtMode, pValue, 
				PathId, DeviceId);
}

//int DutWlanApiClssHeader SetRfLcCap(int extMode, int value)
//{
//	return  Cf_SetRfLcCap( extMode, value); 
//}

//int DutWlanApiClssHeader GetRfLcCap(int *pExtMode, int *pValue)
//{
//	return Cf_GetRfLcCap( pExtMode, pValue); 
//}

int DutWlanApiClssHeader SetRfXTal(int Extension, int Cal, DWORD DeviceId)
{
	return  Cf_SetRfXTal(Extension, Cal, DeviceId);
}

int DutWlanApiClssHeader GetRfXTal(int *pExtension, int *pCal, DWORD DeviceId)
{
	return  Cf_GetRfXTal(pExtension, pCal, DeviceId);
}

int DutWlanApiClssHeader SetRfPowerRange(int pwrRange
#if defined (_MIMO_)					
												  ,
											  DWORD PathId,  
											  DWORD DeviceId
#endif //#if defined (_MIMO_)
											  )
{
 	return Cf_SetRfPowerRange(pwrRange
#if defined (_MIMO_)					
								,
								PathId,  
								DeviceId
#endif //#if defined (_MIMO_)
								);
}

int DutWlanApiClssHeader GetRfPowerRange(int *pRangeIndex, int *pTopPwr, int *pBtmPwr
#if defined (_MIMO_)					
												  ,
											  DWORD PathId,  
											  DWORD DeviceId
#endif //#if defined (_MIMO_)
											  )
{
 	return Cf_GetRfPowerRange(pRangeIndex, pTopPwr, pBtmPwr
#if defined (_MIMO_)					
								,
								PathId,  
								DeviceId
#endif //#if defined (_MIMO_)
											  );
}

int DutWlanApiClssHeader ClearRxPckt(DWORD DeviceId)
{ 
	return Cf_ClearRxPckt(DeviceId);
}

int DutWlanApiClssHeader GetRxPckt(
				unsigned long *cntRx,
				unsigned long *cntOk,
				unsigned long *cntKo,
				DWORD DeviceId) 
{ 
	return Cf_GetRxPckt(cntRx, cntOk, cntKo, DeviceId);
}

int DutWlanApiClssHeader SetRxAntenna(int antSelection, DWORD DeviceId) 
{
 	return Cf_SetRxAnt(antSelection, DeviceId);
}

int DutWlanApiClssHeader SetTxAntenna(int antSelection, DWORD DeviceId) 
{
	return Cf_SetTxAnt(antSelection, DeviceId);
}

int DutWlanApiClssHeader GetRxAntenna(int *antSelection, DWORD DeviceId) 
{
	return Cf_GetRxAnt(antSelection, DeviceId);
}

int DutWlanApiClssHeader GetTxAntenna(int *antSelection, DWORD DeviceId) 
{
	return Cf_GetTxAnt(antSelection, DeviceId);
}

//rate in B DATARATE_1M, DATARATE_2M, DATARATE_5M, DATARATE_11M, DATARATE_22M
int DutWlanApiClssHeader SetTxDataRate(int rate, DWORD DeviceId)
{
	return Cf_SetTxDataRate(rate, DeviceId);
}

int DutWlanApiClssHeader GetTxDataRate(int *rate, DWORD DeviceId)
{
	return Cf_GetTxDataRate(rate, DeviceId);
}

//int DutWlanApiClssHeader Enable22M(void)
//{ 
//	return Cf_Enable22M(TRUE );
//}

//int DutWlanApiClssHeader Enable72M(void)
//{ 
//	return Cf_Enable72M(TRUE );
//}
 
int DutWlanApiClssHeader SetTxContMode(BOOL enable, BOOL CwMode, 
										DWORD payloadPattern,
										DWORD		CSmode,
										DWORD		ActSubCh, 
										DWORD 		DeviceId)
{
 	return Cf_SetTxContMode(enable,  CwMode,  payloadPattern, CSmode, ActSubCh, DeviceId);
}  

int DutWlanApiClssHeader SetTxCarrierSuppression(BOOL enable, 
										DWORD 		DeviceId)
{
 	return Cf_SetTxCarrierSuppression(enable, DeviceId);
}  

int DutWlanApiClssHeader SetTxDutyCycle(BOOL enable, int dataRate, int Percentage, int PayloadLengthInKB,
					int payloadPattern,	int ShortPreamble							
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
					DWORD 		DeviceId
#endif // #if defined (_MIMO_)
		)   
{
	return Cf_SetTxDutyCycle(enable, dataRate,  Percentage, PayloadLengthInKB, 
					payloadPattern, ShortPreamble							
#if defined (_MIMO_)
					,
					ActSubCh,		ShortGI, 
					AdvCoding, 		TxBfOn,  
					GFMode, 		STBC, 
					DPDEnable,		PowerID,
					SignalBw,
					DeviceId
#endif // #if defined (_MIMO_)
					);
}  
/*
int DutWlanApiClssHeader SetPowerMode(int mode)  // 0 active, 1 standby mode, 2 power down mode
{
 	return Cf_SetPowerMode(mode);
}
*/
int DutWlanApiClssHeader SetPowerCycleMode(int cycle, int sleepDur, int awakeDur, int stableXosc,  
													DWORD DeviceId)   
{
 	int status=ERROR_NONE;

 	return Cf_SetPowerCycleMode(cycle, sleepDur, awakeDur, stableXosc,  DeviceId);
}

int DutWlanApiClssHeader EnterSleepMode(void)
{
return Cf_EnterSleepMode();
}

int DutWlanApiClssHeader SetBeaconInterval(DWORD interval, DWORD DeviceId)  //in ms
{
 	return Cf_SetBeaconInterval(interval, DeviceId);
}

int DutWlanApiClssHeader GetBeaconInterval(DWORD *interval, DWORD DeviceId)  //in ms
{
	return Cf_GetBeaconInterval(interval, DeviceId);
}

int DutWlanApiClssHeader SetBeaconEnable(BOOL enable, DWORD DeviceId)
{
	return Cf_SetBeaconEnable(enable, DeviceId);
}


int DutWlanApiClssHeader GetBeaconEnable(BOOL *enable, DWORD DeviceId)
{
	return Cf_GetBeaconEnable(enable, DeviceId);
}

 int DutWlanApiClssHeader GetHWVersion(WORD *socId, BYTE *socVersion, 
									   BYTE *RfId, BYTE *RfVersion, 
									   BYTE *BbId, BYTE *BbVersion,
									   DWORD DeviceId)
{ 
	return  Cf_GetHWVersion(socId, socVersion, 
							RfId, RfVersion, 
							BbId, BbVersion,
							DeviceId);
}

int DutWlanApiClssHeader GetORVersion(BYTE OR_ID, BYTE *OR_Major, BYTE *OR_Minor, BYTE *OR_Cust)
{ 
	return  Cf_GetORVersion(OR_ID, OR_Major, OR_Minor, OR_Cust);
}

int DutWlanApiClssHeader GetFWVersion(BYTE *version)
{
	int status=ERROR_NONE;

	status = Cf_FWVesion (version);
	if(ERROR_NONE != status)	return status;
 
	return status;
}

int DutWlanApiClssHeader GetFWVerNumber(DWORD *FWVersion,DWORD *MFGFWVersion)
{
	int status=ERROR_NONE;

	status = Cf_FWVerNumber(FWVersion, MFGFWVersion);
	if(ERROR_NONE != status)	return status;
 
	return status;
}

int DutWlanApiClssHeader GetReleaseNote(char *ReleaseNote)
{
	int status=ERROR_NONE;

	status = Cf_GetReleaseNote (ReleaseNote);
	if(ERROR_NONE != status)	return status;
 
	return status;
} 

int DutWlanApiClssHeader MacWakeupRfBBP(void)
{	
	int status=ERROR_NONE;

	status = Cf_MacWakeupRfBBP();
 	return status;
}

int DutWlanApiClssHeader EnableBssidFilter(int mode, BYTE Bssid[], char SSID[],  
											   DWORD DeviceId)
{
   	return Cf_EnableBssidFilter(mode, Bssid, SSID, DeviceId);
}
int DutWlanApiClssHeader TxBrdCstPkts(DWORD dataRate,	DWORD pattern, 
							 DWORD length,		DWORD count, 
							 int ShortPreamble, char Bssid[]
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
  	return Cf_TxBrdCstPkts(dataRate,		pattern, 
							length,			count, 
							ShortPreamble,  Bssid
#if defined (_MIMO_)
							,
							ActSubCh, 		ShortGI, 
							AdvCoding, 		TxBfOn,    
							GFMode,    		STBC,
							DPDEnable,		PowerID,
					 		DeviceId
#endif // #if defined (_MIMO_)
							);
}
int DutWlanApiClssHeader AdjustPcktSifs(DWORD Enable, 
										DWORD dataRate,	DWORD pattern, 
							DWORD length,	 
							int ShortPreamble, char *Bssid	
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
return Cf_AdjustPcktSifs(Enable,	dataRate,	pattern, 
						length,	 
						ShortPreamble, Bssid	
#if defined (_MIMO_)
					,
						ActSubCh, 
						ShortGI, 
						AdvCoding,  
						TxBfOn,  
						GFMode, 
						STBC, 
						DPDEnable,
						PowerID,
						SignalBw,
#if defined (_W8787_) || 	defined (_W8782_)						
						AdjustTxBurstGap, 
						BurstSifsInUs, 
#endif //#if defined (_W8787_) || 	defined (_W8782_)						
						DeviceId
#endif // #if defined (_MIMO_)
					);
}


int DutWlanApiClssHeader GetMACAddress(void  *pObj, 
									   BYTE ByteArray[])//, BOOL NOEEPROM)
{	
	int status=ERROR_NONE;

//	if (g_NoEeprom)
//		status = Dev_GetMACAddress(ByteArray);
//	else
	{
		DWORD HeaderTblData[VMR_HEADER_LENGTH/sizeof(DWORD)]={0};
		DWORD Version=0; 
		BOOL CsC=0; 

#ifndef _LINUX_   
		((DutSharedClssObj*)pObj)->SpiGetHeaderTbl(DEVICETYPE_WLAN,  this, HeaderTblData);//, &Version, &CsC);
		((DutSharedClssObj*)pObj)->CheckHeaderTbl(HeaderTblData, &Version, &CsC);
#else // #ifndef _LINUX_
		SpiGetHeaderTbl(DEVICETYPE_WLAN, pObj, HeaderTblData);//, &Version, &CsC);
		CheckHeaderTbl(HeaderTblData, &Version, (int*)&CsC);
#endif // #ifndef _LINUX_
		
		if(ByteArray) 
			memcpy(ByteArray, HeaderTblData+(VMR_MACADDR_LOW/sizeof(DWORD)), 6); 
		if(!CsC && Version == (0xFFFF & Version))
			return ERROR_MISMATCH; 
	}

 	return status;
}

int DutWlanApiClssHeader SetMACAddress(void  *pObj, 
									   BYTE ByteArray[])//, BOOL NOEEPROM, BOOL ReProg)
{	
	int status=ERROR_NONE;

//	if (g_NoEeprom)
//		status = Dev_SetMACAddress(ByteArray);
//	else
	{
		DWORD HeaderTblData[VMR_HEADER_LENGTH/sizeof(DWORD)]={0};
	//	DWORD Version=0; 
	//	BOOL CsC=0; 

#ifndef _LINUX_
		((DutSharedClssObj*)pObj)->SpiGetHeaderTbl(DEVICETYPE_WLAN, this, HeaderTblData); //, &Version, &CsC);
#else // #ifndef _LINUX_
		SpiGetHeaderTbl(DEVICETYPE_WLAN, pObj, HeaderTblData); //, &Version, &CsC);
#endif // #ifndef _LINUX_
		{
			memcpy(HeaderTblData+ (VMR_MACADDR_LOW/sizeof(DWORD)),ByteArray, 6); 
#ifndef _LINUX_
		  	((DutSharedClssObj*)pObj)->DoCs0((DWORD*)HeaderTblData, VMR_HEADER_LENGTH);
			((DutSharedClssObj*)pObj)->SpiSetHeaderTbl(DEVICETYPE_WLAN, this, HeaderTblData);
#else // #ifndef _LINUX_
		  	DoCs0((DWORD*)HeaderTblData, VMR_HEADER_LENGTH);
			SpiSetHeaderTbl(DEVICETYPE_WLAN, pObj, HeaderTblData);
#endif // #ifndef _LINUX_
		}
	}
	
	return status;
}
 
int DutWlanApiClssHeader Wifi_EraseEEPROM(int Sector)
{	
	int status=ERROR_NONE;
	DWORD data[1]={0xFFFFFFFF};
	
	return Wifi_SpiWriteData(Sector, 1, data, true);    
}

int DutWlanApiClssHeader SetPIDVID(void  *pObj, 
								   DWORD E2pOffset,  WORD PID, WORD VID)
{	
	int status=ERROR_NONE;
	DWORD data=0; 
	DWORD HeaderTblData[VMR_HEADER_LENGTH/sizeof(DWORD)]={0};
	DWORD Version=0; 
	int CsC=0; 


	data = PID;

	data = ((data<<16) & 0xFFFF0000) + (VID & 0xFFFF);

#ifndef _LINUX_
	((DutSharedClss*)pObj)->SpiGetHeaderTbl(DEVICETYPE_WLAN, this, HeaderTblData);//, &Version, &CsC);
	((DutSharedClss*)pObj)->CheckHeaderTbl(HeaderTblData, &Version, &CsC);
#else // #ifndef _LINUX_
	SpiGetHeaderTbl(DEVICETYPE_WLAN, pObj, HeaderTblData);//, &Version, &CsC);
	CheckHeaderTbl(HeaderTblData, &Version, &CsC);
#endif // #ifndef _LINUX_

	if(!CsC && (0x0000FFFF& Version) == Version) 
	{
		memcpy(HeaderTblData+ (E2pOffset/sizeof(DWORD)),&data, 4); 
#ifndef _LINUX_
	  	((DutSharedClss*)pObj)->DoCs0((DWORD*)HeaderTblData, VMR_HEADER_LENGTH);
		((DutSharedClss*)pObj)->SpiSetHeaderTbl(DEVICETYPE_WLAN, this, HeaderTblData);
#else // #ifndef _LINUX_
	  	DoCs0((DWORD*)HeaderTblData, VMR_HEADER_LENGTH);
		SpiSetHeaderTbl(DEVICETYPE_WLAN, pObj, HeaderTblData);
#endif // #ifndef _LINUX_
	}
	else
#ifndef _LINUX_
		((DutSharedClss*)pObj)->WriteLumpedData(DEVICETYPE_WLAN, this, E2pOffset, 1, &data);
#else // #ifndef _LINUX_
		WriteLumpedData(DEVICETYPE_WLAN, pObj, E2pOffset, 1, &data);
#endif // #ifndef _LINUX_

  	return status;
}
 
int DutWlanApiClssHeader GetPIDVID(void  *pObj, 
								   DWORD E2pOffset,  WORD *PID, WORD *VID)
{	
	int status=ERROR_NONE;
	DWORD data=0; 
	DWORD HeaderTblData[VMR_HEADER_LENGTH/sizeof(DWORD)]={0};
	DWORD Version=0; 
	int CsC=0; 

#ifndef _LINUX_
	((DutSharedClss*)pObj)->SpiGetHeaderTbl(DEVICETYPE_WLAN, this, HeaderTblData);//, &Version, &CsC);
	((DutSharedClss*)pObj)->CheckHeaderTbl(HeaderTblData, &Version, &CsC);
#else // #ifndef _LINUX_
	SpiGetHeaderTbl(DEVICETYPE_WLAN, pObj, HeaderTblData);//, &Version, &CsC);
	CheckHeaderTbl(HeaderTblData, &Version, &CsC);
#endif // #ifndef _LINUX_

	data = (*(HeaderTblData+ (E2pOffset/sizeof(DWORD))));

	*PID =  (WORD)((data & 0xFFFF0000)>>16);
	*VID =  (WORD)((data & 0x0000FFFF) );

	if(!CsC && Version == (0xFFFF & Version))
		return ERROR_MISMATCH; 

  	return status;
}

int DutWlanApiClssHeader SetClassId(void  *pObj, 
									DWORD E2pOffset,  DWORD Class)
{	
	int status=ERROR_NONE;
	DWORD data=0; 
	DWORD HeaderTblData[VMR_HEADER_LENGTH/sizeof(DWORD)]={0};
	DWORD Version=0; 
	int CsC=0; 

#ifndef _LINUX_
	((DutSharedClss*)pObj)->SpiGetHeaderTbl(DEVICETYPE_WLAN, this, HeaderTblData);//, &Version, &CsC);
	((DutSharedClss*)pObj)->CheckHeaderTbl(HeaderTblData, &Version, &CsC);
#else // #ifndef _LINUX_
	SpiGetHeaderTbl(DEVICETYPE_WLAN, pObj, HeaderTblData);//, &Version, &CsC);
	CheckHeaderTbl(HeaderTblData, &Version, &CsC);
#endif // #ifndef _LINUX_

	data = (*(HeaderTblData+E2pOffset/sizeof(DWORD)));
	data = data &0xFF;
	data |= (Class & 0xFFFFFF)<<8;

	if(!CsC && (0x0000FFFF& Version) == Version) 
	{
		memcpy(HeaderTblData+E2pOffset/sizeof(DWORD),&data, 4); 
#ifndef _LINUX_
	  	((DutSharedClss*)pObj)->DoCs0((DWORD*)HeaderTblData, VMR_HEADER_LENGTH);
		((DutSharedClss*)pObj)->SpiSetHeaderTbl(DEVICETYPE_WLAN, this, HeaderTblData);
#else // #ifndef _LINUX_
	  	DoCs0((DWORD*)HeaderTblData, VMR_HEADER_LENGTH);
		SpiSetHeaderTbl(DEVICETYPE_WLAN, pObj, HeaderTblData);
#endif // #ifndef _LINUX_
	}
	else
#ifndef _LINUX_
		((DutSharedClss*)pObj)->WriteLumpedData(DEVICETYPE_WLAN, this, E2pOffset, 1, &data);
#else // #ifndef _LINUX_
		WriteLumpedData(DEVICETYPE_WLAN, pObj, E2pOffset, 1, &data);
#endif // #ifndef _LINUX_

  	return status;
}

int DutWlanApiClssHeader GetClassId(void  *pObj, 
									DWORD E2pOffset,  DWORD *Class)
{	
	int status=ERROR_NONE;
	DWORD data=0; 
	DWORD HeaderTblData[VMR_HEADER_LENGTH/sizeof(DWORD)]={0};
	DWORD Version=0; 
	int CsC=0; 

#ifndef _LINUX_
	((DutSharedClss*)pObj)->SpiGetHeaderTbl(DEVICETYPE_WLAN, this, HeaderTblData);//, &Version, &CsC);
	((DutSharedClss*)pObj)->CheckHeaderTbl(HeaderTblData, &Version, &CsC);
#else // #ifndef _LINUX_
	SpiGetHeaderTbl(DEVICETYPE_WLAN, pObj, HeaderTblData);//, &Version, &CsC);
	CheckHeaderTbl(HeaderTblData, &Version, &CsC);
#endif // #ifndef _LINUX_

	data = (*(HeaderTblData+E2pOffset/sizeof(DWORD)));

 	*Class  = (data >>8 ) & 0xFFFFFF ;

 	if(!CsC && Version == (0xFFFF & Version))
		return ERROR_MISMATCH; 

  	return status;
}

int DutWlanApiClssHeader XoscCal(WORD *cal, int TU, DWORD DeviceId) 
{
  	return  Cf_XoscCal(cal, TU, DeviceId);
}

int DutWlanApiClssHeader GetRssiNf(int stop, int *count, 
						  int *rssi, int *SNR, int *NoiseFloor, 
						  DWORD DeviceId)
{
	return Dev_GetRssiNf(stop, count, rssi, SNR, NoiseFloor, DeviceId); 
}

int DutWlanApiClssHeader ArmReadUlongArray(DWORD armAddr, DWORD *pBuf, DWORD sizeBytes,
                              DWORD deviceId) {
    return Dev_ArmReadUlongArray(armAddr, pBuf, sizeBytes, deviceId);
}


//#if defined(_ENGINEERING_)

int DutWlanApiClssHeader RunBbpSqDiag(DWORD sizeBytes, DWORD *pSqBufAddr)
{
	return Dev_RunBbpSqDiag(sizeBytes, pSqBufAddr);
}

//#endif // #if defined(_ENGINEERING_)


int DutWlanApiClssHeader ThermalTest(	BOOL enabled, 
										DWORD PTarget,
										DWORD tempRef, 
										DWORD SlopeNumerator, 
										DWORD SlopeDenominator,
										DWORD CalibrationIntervalInMS,
										DWORD Temp, 
										DWORD SlopeNumerator1, 
										DWORD SlopeDenominator1)
{
	return Dev_ThermalTest(	enabled, 
							PTarget,
							tempRef, 
							SlopeNumerator, 
							SlopeDenominator,
							CalibrationIntervalInMS,
							Temp, 
							SlopeNumerator1, 
							SlopeDenominator1);
}

int DutWlanApiClssHeader ThermalComp(BOOL enabled, 
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
	return Dev_ThermalComp(	enabled, tempRefAtCal, tempRefSlope,
							SlopeNumerator, SlopeNumerator1, SlopeDenominator,
							CalibrationIntervalInMS, 
							PTargetDelta, 
							PathId, DeviceId); 
}

int DutWlanApiClssHeader GetThermalComp(BOOL *pEnabled, 
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
	return Dev_GetThermalComp(pEnabled, pTempRefAtCal, pTempRefSlope,
							pSlopeNumerator, 
							pSlopeNumerator1, 
							pSlopeDenominator,
							pCalibrationIntervalInMS, 
							pPTargetDelta, 
							PathId, DeviceId);
}

int DutWlanApiClssHeader GetThermalTest(BOOL *pEnabled, 
						DWORD *pPTarget,
						DWORD *pTempRef,
						DWORD *pSlopeNumerator, 
						DWORD *pSlopeDenominator,
						DWORD *pCalibrationIntervalInMS, 
						DWORD *pTemp,
						DWORD *pSlopeNumerator1, 
						DWORD *pSlopeDenominator1)
{
	return Dev_GetThermalTest(	pEnabled, 
								pPTarget,
								pTempRef,
								pSlopeNumerator, 
								pSlopeDenominator,
								pCalibrationIntervalInMS, 
								pTemp,
								pSlopeNumerator1, 
								pSlopeDenominator1);
}

int DutWlanApiClssHeader GetThermalSensorReading(int *pReadBack)
{ 
	return Dev_GetThermalSensorReading(pReadBack);
}

#if defined (_SUPPORT_LDO_)
int DutWlanApiClssHeader SetLDOConfig(int LDOSrc)
{
	return Dev_SetLDOConfig(LDOSrc);
}

int DutWlanApiClssHeader GetLDOConfig(int* LDOSrc)
{
	return Dev_GetLDOConfig(LDOSrc);
}
#endif // #if defined (_SUPPORT_LDO_)

#if defined (_SUPPORT_PM_)
int DutWlanApiClssHeader WritePMReg(int addr, int value)
{
	return Dev_WritePMReg(addr, value);
}

int DutWlanApiClssHeader ReadPMReg(int addr, int *value)
{
	return Dev_ReadPMReg(addr, value);
}

int DutWlanApiClssHeader SetPMRail(int railId, int mode, int volt, int powerDown)
{
	return Dev_SetPMRail(railId, mode, volt, powerDown);
}
#endif // #if defined (_SUPPORT_PM_)

int DutWlanApiClssHeader SetRssiNf(DWORD DeviceId)
{
	return Dev_SetRssiNf(DeviceId); 
}

int DutWlanApiClssHeader SoftReset(DWORD DeviceId)
{
	return Dev_SoftReset(DeviceId); 
}

int DutWlanApiClssHeader HwResetRf(DWORD DeviceId)
{
	return Dev_HwResetRf(DeviceId); 
}

/*
int DutWlanApiClssHeader SetRfPowerUseCal(int *pPwr4Pa , int RateG, 
						DWORD PathId,  
						DWORD DeviceId)
{
	return Dev_SetRfPowerUseCal(pPwr4Pa,   RateG, PathId,  DeviceId);
}
*/
int DutWlanApiClssHeader SetRfPowerUseCal(int *pPwr4Pa , int RateG, 
										BYTE* cal_data_ext, int len,
						DWORD PathId,  
						DWORD DeviceId)
{
	return Dev_SetRfPowerUseCal(pPwr4Pa,   RateG, 
									cal_data_ext,	len,
									PathId,  DeviceId);
}


int DutWlanApiClssHeader SetRfControlMode(DWORD ThermalOnly,  
						DWORD DeviceId)
{
	return Dev_SetRfControlMode(ThermalOnly, DeviceId);
}

int DutWlanApiClssHeader GetRfControlMode(DWORD *pThermalOnly,  
						DWORD DeviceId)
{
	return Dev_GetRfControlMode(pThermalOnly, DeviceId);
}

int DutWlanApiClssHeader ForceE2PromType(DWORD IfType, 
										 DWORD AddrWidth, 
										 DWORD DeviceType)
{
	return Dev_ForceE2PromType(IfType, AddrWidth, DeviceType);
}

int DutWlanApiClssHeader GetE2PromType(DWORD *pIfType, 
									   DWORD *pAddrWidth, 
									   DWORD *pDeviceType)
{
	return Dev_GetE2PromType(pIfType, pAddrWidth, pDeviceType);
}

int DutWlanApiClssHeader SetFEM(DWORD FE_VER,
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
	return Dev_SetFEM(			FE_VER,
								FEMCap2G,
								FEMCap5G,
								FEM_INTERNAL_BT,
								FEM_ANT_DIV,
								FEM_EXTERNAL_2G_LNA,
								FEM_EXTERNAL_5G_LNA,
								FEM_SAMEANT_2G_BT,
								FEM_SAMEANT_5G_BT,
								CONCURRENT_CAP_2G_BT,
								CONCURRENT_CAP_5G_BT,
								Front_End_2G,
								Front_End_5G,
								AntDiversityInfo);
}

int DutWlanApiClssHeader ReloadCalData()
{
	int status=ERROR_NONE;

	status = Dev_ReloadCalData();

	return status;

}

int DutWlanApiClssHeader SetRfPwrInitP(	BYTE InitP, 
						DWORD PathId,	
						DWORD DeviceId)
{

    
	return Dev_SetRfPwrInitP(InitP, 
						PathId,	
						DeviceId);
}

int DutWlanApiClssHeader GetRfPwrVgaMax(	BYTE *pVgaMax, 
						DWORD PathId,	
						DWORD DeviceId)
{
	return Dev_GetRfPwrVgaMax(pVgaMax, 
							PathId,	
							DeviceId);
}

int DutWlanApiClssHeader DownLoadStaticSettingsToFw(DWORD BandId, DWORD SubbandId, 
											   DWORD PathId, 
 									BYTE RfLnaGrif,
 									BYTE RfLnaGrBb,
									DWORD DeviceId)
{
	return  Dev_DownLoadStaticSettingsToFw(BandId, SubbandId, 
									PathId, 
 									RfLnaGrif,
 									RfLnaGrBb,
									DeviceId);
}

int DutWlanApiClssHeader UpLoadStaticSettingsFromFw(DWORD BandId, DWORD SubbandId, 
											   DWORD PathId, 
 									BYTE *pRfLnaGrif,
 									BYTE *pRfLnaGrBb,
									DWORD DeviceId)
{	
	return  Dev_UpLoadStaticSettingsFromFw(BandId, SubbandId, 
										PathId, 
 									pRfLnaGrif,
 									pRfLnaGrBb,
									DeviceId);
}

int DutWlanApiClssHeader Dev_UpLoadPwrTable(int bandId, int SubBandId,  
								   int *sizeOfTblInByte, BYTE* tbl,
								   DWORD PathId,	DWORD DeviceId )
{  
	return Dev_GetRfPwrTbl(bandId, SubBandId,  
							sizeOfTblInByte, tbl,  
							PathId, DeviceId);
}

int DutWlanApiClssHeader Dev_DownLoadPwrTable(int bandId, int SubBandId,   
									 int *sizeOfTblInByte, BYTE* tbl, 
									DWORD PathId,	DWORD DeviceId)
{  
	return Dev_SetRfPwrTbl(bandId, SubBandId,  
							*sizeOfTblInByte, tbl,  
							PathId,	DeviceId);
}

int DutWlanApiClssHeader GetPwrTbleFileName(int deviceId, int pathId, int bandId, int subbandId,  
								   int Upload, char* FileName)
{
 	char	line[255]="";
	char	BandChar[NUM_BAND]={'G', 'A'}, PathChar[MAXNUM_TXPATH]={'A'};//, 'B'}; //, 'C'};
	char	*pUpLoad[3]={"", "_Upload", "_Swap"};
	WORD	socId;
	BYTE	socVersion;
	BYTE	RfId, RfVersion; 
	BYTE	BbId, BbVersion;

	char temp[_MAX_PATH]="";
	
	if(getcwd(temp, _MAX_PATH) != NULL)  
		strcat(temp, "\\setup.ini");
	else
		strcpy(temp, "setup.ini");

 	GetHWVersion(	&socId, &socVersion, &RfId, &RfVersion, 
 					&BbId,	&BbVersion, deviceId);

 
	if(CHIPID_8060 == RfId )
	{		
 		sprintf(line, "W8060_PwrTbl_Dev%d_Path%c_%c_%d%s", 
			deviceId, PathChar[pathId], BandChar[bandId], subbandId, pUpLoad[Upload]);
		strcpy( FileName,line); 
		strcat(FileName, ".txt");
  
		GetPrivateProfileString("SHPwrTbl", line, FileName, FileName,
				255, temp); 
	}
	if(0x09 == RfId )
	{		
 		sprintf(line, "W878x_PwrTbl_Dev%d_Path%c_%c_%d%s", 
			deviceId, PathChar[pathId], BandChar[bandId], subbandId, pUpLoad[Upload]);
		strcpy( FileName,line); 
		strcat(FileName, ".txt");
  
		GetPrivateProfileString("SHPwrTbl", line, FileName, FileName,
				255, temp); 
	}	

	return 0;
}

int DutWlanApiClssHeader LoadPwrTableFile(int deviceId, int pathId, int bandId, int subbandId, 
					   int *sizeOfTblInByte, BYTE* pwrTbl, char* GivenFileName) 
{
 
	int		status=ERROR_NONE;
	FILE	*hFile=NULL;
	int		cnt = 0, cntLine=0; 
	int		tempInt[5]={0};
	char	FileName[255]={"W878x_PwrTbl.txt"};
	char	line[255]="";
	FW_POWRTABLE *pTbl=NULL;
	int localPpa5_1[RFPWRRANGE_NUM]={0},	localPpa5_2[RFPWRRANGE_NUM]={0};
	int localTop[RFPWRRANGE_NUM]={0},		localBot[RFPWRRANGE_NUM]={0};

	int localPwrLvl[POWTBL_LEVEL]={0},		localValInitTrgt[POWTBL_LEVEL]={0};
	int localValPdetThHi[POWTBL_LEVEL]={0},	localValPdetThLo[POWTBL_LEVEL]={0};
	int localValDacX[POWTBL_LEVEL]={0};
	
	if(NULL== GivenFileName) 
		GetPwrTbleFileName(deviceId, pathId, bandId, subbandId,  0, FileName);
	else
		strcpy(FileName,GivenFileName);  

	pTbl =(FW_POWRTABLE*)pwrTbl; 
	
	hFile = fopen(FileName, "r");
	if (NULL == hFile)
	{
		DebugLogRite("Cannot Find File \"%s\"\n", FileName);
		return ERROR_ERROR;
	}
	DebugLogRite("LoadPwrTableFile from File \"%s\"\n", FileName);
  
 
 
 	*sizeOfTblInByte =0;  
	cntLine=0;

// first line
	do
	{
		if(NULL != fgets(line, 255, hFile) )
		{
			if(strlen(line)>0 && line[0]!=';')
			{	//  HigestFreq, lowestFreq 
				cnt = sscanf(line, "%d %d", 
					&tempInt[0], &tempInt[1] );  
			 
 				if ( cnt != 2) 
						DebugLogRite("Format error on Line: %s\n", line);
				else
				{	*sizeOfTblInByte += 4;
					pTbl->pwrTbl.HighFreqBound	= tempInt[0];
 					pTbl->pwrTbl.LowFreqBound		= tempInt[1]; 
					break;
				}
			}

		}
	}while(1);

	while ((cntLine<RFPWRRANGE_NUM) && (NULL != fgets(line, 255, hFile)) )
	{
		if(strlen(line)>0 && line[0]!=';')
		{
			cnt = sscanf(line, "%x %x %d %d", 
				&tempInt[0], &tempInt[1], &tempInt[2], &tempInt[3],  &tempInt[4]);  
 			if (cnt != 4)
			{
 				DebugLogRite("Format error on Line: %s\n", line);
			}
			else
			{
				*sizeOfTblInByte += 4;
				localPpa5_1[cntLine] = tempInt[0];
				localPpa5_2[cntLine] = tempInt[1];
				localTop[cntLine] = tempInt[2];
				localBot[cntLine] = tempInt[3];

				cntLine++; 
			}
		}
	}


		for (cnt=0; cnt < cntLine; cnt++)
		{
 				pTbl->pwrTbl.rngTblLevel[cnt].PpaGain5_1 = localPpa5_1[cnt];
 				pTbl->pwrTbl.rngTblLevel[cnt].PpaGain5_2 = localPpa5_2[cnt];
 				pTbl->pwrTbl.rngTblLevel[cnt].TopPwr = localTop[cnt];
 				pTbl->pwrTbl.rngTblLevel[cnt].BtmPwr = localBot[cnt];  
		}

	cntLine=0;
	//fgets(line, 255, hFile)
	while ((cntLine<POWTBL_LEVEL) && (NULL != fgets(line, 255, hFile)) )
	{
		if(strlen(line)>0 && line[0]!=';')
		{
			cnt = sscanf(line, "%d %x %x %x %x", 
				&tempInt[0], &tempInt[1], &tempInt[2], &tempInt[3],  &tempInt[4]);  

 			if (cnt != 5)
			{
 				DebugLogRite("Format error on Line: %s\n", line);
			}
			else
			{
				*sizeOfTblInByte += 4;
				
 				localPwrLvl[cntLine] = tempInt[0]; 
 				
 				localValInitTrgt[cntLine] = tempInt[1]; 
				
 				localValPdetThHi[cntLine] = tempInt[2]; 

				localValPdetThLo[cntLine] = tempInt[3]; 

 				localValDacX[cntLine] = tempInt[4]; 
 				cntLine++;
				
			}
		}
	}
	fclose(hFile);
 
	// sort them 
	Sort(cntLine, localPwrLvl, 
				   localValInitTrgt, localValPdetThHi, 
				   localValPdetThLo, localValDacX);

	// save them into structure
	for (cnt=0; cnt < cntLine; cnt++)
	{
		pTbl->pwrTbl.pwrTblLevel[cnt].powerLevel	= localPwrLvl[cnt];
		pTbl->pwrTbl.pwrTblLevel[cnt].ValInitTgrt = localValInitTrgt[cnt];
		pTbl->pwrTbl.pwrTblLevel[cnt].ValPDetThHi = localValPdetThHi[cnt];
		pTbl->pwrTbl.pwrTblLevel[cnt].ValPDetThLo = localValPdetThLo[cnt];
		pTbl->pwrTbl.pwrTblLevel[cnt].Reserve = 0;
	}

	*sizeOfTblInByte += 4;
	pTbl->DeviceId = deviceId;
	pTbl->PathId = pathId;
	pTbl->BandId = bandId;
	pTbl->SubBandId = subbandId;
 
	return status;
}

int DutWlanApiClssHeader UpLoadPwrTable(int bandId, int SubBandId,  
							   int *sizeOfTblInByte, BYTE* tbl,
								DWORD DeviceId,		DWORD PathId)
{
	return Dev_UpLoadPwrTable(bandId, SubBandId, sizeOfTblInByte, tbl,
								DeviceId,	 PathId);
}

int DutWlanApiClssHeader DownLoadPwrTable(int bandId, int SubBandId,  
								 int *sizeOfTblInByte, BYTE* tbl, 
								 int UsePassDownData,
								DWORD DeviceId,		DWORD PathId)
{
 	int err=0;

 
	if(UsePassDownData)
	{
		return Dev_DownLoadPwrTable(bandId, SubBandId, 
									sizeOfTblInByte, tbl,
									DeviceId, PathId); 
	}else
	{
		if(ERROR_NONE == Dev_UpLoadPwrTable(bandId, SubBandId, 
						  sizeOfTblInByte, tbl, DeviceId, PathId))
		{
				err = LoadPwrTableFile( DeviceId, PathId, bandId, SubBandId, 
					 sizeOfTblInByte, tbl  				
									);
			if( ERROR_NONE == err)	
			{  
 				return Dev_DownLoadPwrTable(bandId, SubBandId,  
									sizeOfTblInByte, tbl,
									DeviceId,		PathId); 
			}
		}
	}
		return ERROR_ERROR;
}

int DutWlanApiClssHeader InitConnection(void)
{
	int status=0;

 
 	status = Cf_OpenCfDevice( );

 	return status;
}

int DutWlanApiClssHeader Disconnection(void)
{
	int status=0;
	Cf_CloseCfDevice( );
 	return status;
}

int DutWlanApiClssHeader TxFastCal(BYTE *DataStruc, int len) 
{
  	return Cf_TxFastCal(DataStruc, len);
}

int DutWlanApiClssHeader TxFastVer(BYTE *DataStruc, int len) 
{
  	return Cf_TxFastVer(DataStruc, len);
}

int DutWlanApiClssHeader RxFastVer(BOOL enable, int channel, BYTE *Bssid, char *SSID)
{
  	return Cf_RxFastVer(enable, channel, Bssid, SSID);
}
 
int DutWlanApiClssHeader SetModuleTypeConf(int ModuleType) 
{
	return Cf_SetModuleTypeConf(ModuleType);
}

int DutWlanApiClssHeader GetModuleTypeConf(int *pModuleType)
{
	return Cf_GetModuleTypeConf(pModuleType);

}

int DutWlanApiClssHeader SetCustomizedSettings(char *FileName)
{ 
	int status =0;
	FILE* hFile;
	BYTE cal_data_ext[1500];
	char temp[MAX_LENGTH]="";
	char DefCalFile[MAX_LENGTH]="WlanCalData_ext.conf";
	int i =0xf0;
	DWORD j=0;
	int cnt=0;
	char temp1[4]="";
	int totalLen=0;
    if (FileName !=NULL)
		hFile = fopen(FileName,"rb");
	else
		hFile = fopen(DefCalFile,"rb");
	if (!hFile)
	{
		DebugLogRite("File %s cannot be found!\n", FileName);
		fclose (hFile);
			return 1;
	}
	i = 0;
	fgets(temp, MAX_LENGTH, hFile);
	while(!feof(hFile))
	{
		cnt = strlen(temp);
		
		for (j=0; j<cnt; j++)
		{
			if (isxdigit(temp[j]))
			{
				cal_data_ext[i++] = ctoi(temp[j])*16+ctoi(temp[j+1]);
				j++;
			}
		}
		fgets(temp, MAX_LENGTH, hFile);

	}
	fclose (hFile);
	status = Dev_SetCustomizedSettings(&cal_data_ext[6], i-6);
	 return status;
}

int DutWlanApiClssHeader GetCustomizedSettings(int* Setting)
{
	int status =0;
	status = Dev_GetCustomizedSettings(Setting);
	return status;
}	

//DPD API
#if defined(_W8782_)

int DutWlanApiClssHeader SetDPD(DWORD PowerID, DWORD Memory2Row, DWORD *pDPDTbl, DWORD DeviceId)
{
	int status =0;
	status = Dev_SetDPD(PowerID, Memory2Row, pDPDTbl, DeviceId);
	return status;

}

int DutWlanApiClssHeader GetDPD(DWORD PowerID, DWORD Memory2Row, DWORD *pDPDTbl, DWORD DeviceId)
{
	int status =0;
	status = Dev_GetDPD(PowerID, Memory2Row, pDPDTbl, DeviceId);
	return status;
}

#endif //#ifdef _W8782_
#if defined (_W8787_)
#define MaxRead 8
#define CCA_REG4  0x05E
#define CCA_REG11 0x065
#define CCA_REG15 0x069
#define CCA_REG13 0x067
#define MacReg_Rx_WR_PTR_Data	0x8000a390
#define MacReg_Rx_ign_ownership	0x8000a3c0
#define MacReg_RxSize_data		0x8000a3f0
#define MacReg_block_io			0x8000a46c
#define MacReg_RxBase_data		0x8000a3f4
#define MacReg_RxMode			0x8000a300
#define AGC_REG20				0x0057

int DutWlanApiClssHeader WlanRSSI_CalOneCh_CW(int RSSI_ch, char *RSSI_val, char *NF_val, int Band, DWORD DeviceId)
{
	int status = 0;
	BYTE rssi_reading = 0x04;
	BYTE BB_REG_Reading, BB_REG_Writing;
	double ChFreq;

	if (status = GetRfChannelFreq(Band, RSSI_ch, &ChFreq))
	return status;

	if(Band)
	{
		if(status = SetRfChannelByFreq(ChFreq,(unsigned long)DeviceId))
			return status;
	}
	else
	{
		if(status = SetRfChannel(RSSI_ch,(unsigned long)DeviceId))
			return status; 
	}
	//Instantaneous
	if(status = ReadBBReg(CCA_REG15, &BB_REG_Reading, DeviceId))
		return status;
	BB_REG_Writing = (BB_REG_Reading & 0xED);//69_4 = 0, 69_2 = 0
	if(status = WriteBBReg(CCA_REG15, BB_REG_Writing, DeviceId))
		return status;
//	DebugLogRite ("  CCA_REG15: Reading 0x%02X, Writing 0x%02X\n",
//		BB_REG_Reading, BB_REG_Writing);

// RSSI or SNR, Select RSSI
	if(status = ReadBBReg(CCA_REG4, &BB_REG_Reading, DeviceId))
		return status;
	BB_REG_Writing = BB_REG_Reading | 0x80;//5e_7  = 1 RSSI
	if(status = WriteBBReg(CCA_REG4, BB_REG_Writing, DeviceId))
		return status;
//	DebugLogRite ("  CCA_REG4: Reading 0x%02X, Writing 0x%02X\n",
//		BB_REG_Reading, BB_REG_Writing);

	if(status = ReadBBReg(AGC_REG20, &BB_REG_Reading, DeviceId))
		return status;
	BB_REG_Writing = BB_REG_Reading;
//	DebugLogRite ("  CCA_REG20: Reading 0x%02X, Writing 0x%02X\n",
//		BB_REG_Reading, BB_REG_Writing);

// RSSI or SNR, Select SNR
//	if(status = DutIf_ReadBBReg(CCA_REG4, &BB_REG_Reading, DeviceId))
//		return status;
//	BB_REG99999999999999999999999999999999999999999999999999999999/-_Writing = BB_REG_Reading & 0x7F;//5e_7  = 1 RSSI
//	DebugLogRite ("  CCA_REG4: Reading 0x%02X, Writing 0x%02X\n",
//		BB_REG_Reading, BB_REG_Writing);
//	if(status = DutIf_WriteBBReg(CCA_REG4, BB_REG_Writing, DeviceId))
//		return status;

	if(status = ReadBBReg(CCA_REG13, &BB_REG_Reading, DeviceId))
		return status;
	DebugLogRite ("   CCA_REG13 SNR: Reading 0x%02X, %d (Dec)\n",
		BB_REG_Reading, BB_REG_Reading);
	*NF_val = BB_REG_Reading;

	if(status = ReadBBReg(CCA_REG11, &BB_REG_Reading, DeviceId))
		return status;
	DebugLogRite ("   CCA_REG11 RSSI: Reading 0x%02X, %d (Dec)\n",
		BB_REG_Reading, BB_REG_Reading);
	*RSSI_val = BB_REG_Reading;

	//Confirmation
/*
	if(status = DutIf_ReadBBReg(CCA_REG11, &BB_REG_Reading, DeviceId))
		return status;
	if(BB_REG_Reading)
	{
		DebugLogRite ("  CCA_REG11: Reading 0x%02X, Writing 0x%02X\n",
		BB_REG_Reading, BB_REG_Writing);

	}
	else
	{
		DebugLogRite ("  CCA_REG4: Reading 0x%02X, Writing 0x%02X\n",
			BB_REG_Reading, BB_REG_Writing);
	}
*/
	return 0;
}
int DutWlanApiClssHeader WlanRSSI_CalOneCh_Pkt(int RSSI_ch, 
						 char *RSSI_val,
						 char *NF_val,
						 int Band,
						 DWORD DeviceId)
{

	int status = 0, i, Valid_rssi_counter = 0;
	int DataSize = 1, count;
	double ChFreq, AccRxInfo_rssi_reading = 0.0;
	DWORD addr_for_RxDataQueue, addr_for_RxDataheader;
	DWORD addr_for_RxInfo;
	DWORD RxInfo_rssi_reading;
	DWORD Rx_WR_PTR_Data, QueueSize = 0x08;

	if (status = GetRfChannelFreq(Band, RSSI_ch, &ChFreq))
	return status;

	if(Band)
	{
		if(status = SetRfChannelByFreq(ChFreq,(DWORD)DeviceId))
			return status;
	}
	else
	{
		if(status = SetRfChannel(RSSI_ch,(DWORD)DeviceId))
			return status; 
	}
	if (status = WriteMACReg(MacReg_block_io, 0, (DWORD)DeviceId))
		return status;							

#ifdef _DEBUG
	DebugLogRite ("   WriteMACReg: 0x%02X, set 0 to Clear the bitmap to process every packet\n",
			MacReg_block_io);
#endif
	
	if (status = WriteMACReg(MacReg_Rx_ign_ownership, 0x0FFF, (DWORD)DeviceId))
		return status;
#ifdef _DEBUG
		DebugLogRite ("   WriteMACReg: 0x%02X, set 0xFFF allowing buffer to be overwriten\n",
			MacReg_Rx_ign_ownership);
#endif
		if (status = WriteMACReg(MacReg_RxSize_data, QueueSize, (DWORD)DeviceId))
		return status;

#ifdef _DEBUG
		DebugLogRite ("   WriteMACReg: 0x%02X, set 0x%02X for the queue size\n",
			MacReg_RxSize_data, QueueSize);
#endif

	if (status = ReadMACReg(MacReg_RxMode, &addr_for_RxInfo, (DWORD)DeviceId))
	return status;
#ifdef _DEBUG
		DebugLogRite ("   RxMode_RegReading 0x%02X, Contant: 0x%02X\n",
			MacReg_RxMode, addr_for_RxInfo);
#endif
		addr_for_RxInfo = addr_for_RxInfo | 0x00000021;
#ifdef _DEBUG
		DebugLogRite ("   RxMode_RegWriting 0x%02X, PromiscuousModeEn: 0x%08X\n",
			MacReg_RxMode, addr_for_RxInfo);
#endif
		if (status = WriteMACReg(MacReg_RxMode, (WORD)addr_for_RxInfo, (DWORD)DeviceId))
		return status;

//	for(i = 0; i < Counter; i++)

	if (status = ReadMACReg(MacReg_RxBase_data, &addr_for_RxDataQueue, (DWORD)DeviceId))
		return status;
#ifdef _DEBUG
		DebugLogRite ("   ReadMACReg 0x%02X: 0x%02X\n",
			MacReg_RxBase_data, addr_for_RxDataQueue);//C00174300
#endif
	addr_for_RxDataheader = addr_for_RxDataQueue + 4;
	if (status = PeekRegDword(addr_for_RxDataheader, &addr_for_RxInfo, (DWORD)DeviceId))
		return status;
#ifdef _DEBUG
		DebugLogRite ("   The addr contains RxInfo Queue: 0x%08X, Contant: 0x%08X\n", 
			addr_for_RxDataheader, addr_for_RxInfo);//pt to first RxInfo header c0018500
#endif
	count = 0;
	addr_for_RxDataheader = addr_for_RxInfo;

	do
	{
		count++;
		if (status = PeekRegDword(addr_for_RxDataheader, &RxInfo_rssi_reading, (DWORD)DeviceId))
			return status;

#ifdef _DEBUG
			DebugLogRite ("   addr_for_RxInfo: 0x%08X, RxStructureHeader: 0x%08X\n",
				addr_for_RxDataheader, RxInfo_rssi_reading);//pt to first RxInfo header
#endif
			RxInfo_rssi_reading = RxInfo_rssi_reading & 0xFFFF0000;
		if(RxInfo_rssi_reading == 0xBEEF0000)
		{
			addr_for_RxInfo = addr_for_RxDataheader + 8;
			if (status = PeekRegDword(addr_for_RxInfo, &RxInfo_rssi_reading, (DWORD)DeviceId))
				return status;
#ifdef _DEBUG
				DebugLogRite ("   addr_for_RxInfo: 0x%08X, Contant: 0x%08X\n",
				addr_for_RxInfo, RxInfo_rssi_reading);
#endif
//			Valid_rssi_counter++;
			if(RxInfo_rssi_reading != 0)
			{
				RxInfo_rssi_reading = (RxInfo_rssi_reading >> 24);
//				AccRxInfo_rssi_reading = AccRxInfo_rssi_reading + (double)RxInfo_rssi_reading;
				break;
			}
		};
//		Reset Rx info signature
		PokeRegDword(addr_for_RxDataheader, 0x0, (DWORD)DeviceId);
//		if(Valid_rssi_counter == Valid_rssi_req) break;
		addr_for_RxDataheader = addr_for_RxDataheader + 0x00000100;
	}while (!( count >= MaxRead));

	*RSSI_val = (char)RxInfo_rssi_reading;

	return 0;
}
#endif  //#if defined (_W8787_)
