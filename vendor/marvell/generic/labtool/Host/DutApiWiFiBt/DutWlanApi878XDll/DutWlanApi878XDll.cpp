/** @file DutWlanApi878XDll.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include "../../DutCommon/Clss_os.h"
#include DUTDLL_WLAN_H_PATH	//"../DutWlanApi878XDll.h"
#include DUTCLASS_WLAN_H_PATH	//"DutWlanApiClss.h" 
#include "DutXpartFileHandle.h"
 
//#undef	DUT_WLAN_API
//#define DUT_WLAN_API

DUT_WLAN_API int STDCALL DutIf_PokeRegDword(DWORD address, DWORD data
#if defined (_MIMO_)
											, DWORD DeviceId
#endif //#if defined (_MIMO_)
											   )
{
#if !defined (_MIMO_)
	DWORD DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ PokeRegDword(address, data, DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_PeekRegDword(DWORD address, DWORD *data  
#if defined (_MIMO_)
											 ,  DWORD DeviceId 
#endif //#if defined (_MIMO_)
											 )
{
#if !defined (_MIMO_)
	DWORD DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ PeekRegDword(address, data, DeviceId);
}
 
DUT_WLAN_API int STDCALL DutIf_PokeRegWord(DWORD address, WORD data  
#if defined (_MIMO_)
										   ,
											DWORD DeviceId
#endif //#if defined (_MIMO_)
											   )
{
#if !defined (_MIMO_)
	DWORD DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ PokeRegWord(address, data, DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_PeekRegWord(DWORD address, WORD *data
#if defined (_MIMO_)
										   ,
											DWORD DeviceId
#endif //#if defined (_MIMO_)
											)
{
#if !defined (_MIMO_)
	DWORD DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ PeekRegWord(address, data, DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_PokeRegByte(DWORD address, BYTE data
#if defined (_MIMO_)
										   ,
											DWORD DeviceId
#endif //#if defined (_MIMO_)
											)
{
#if !defined (_MIMO_)
	DWORD DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ PokeRegByte(address, data, DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_PeekRegByte(DWORD address, BYTE *data
#if defined (_MIMO_)
										   ,
											DWORD DeviceId
#endif //#if defined (_MIMO_)
											)
{
#if !defined (_MIMO_)
	DWORD DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ PeekRegByte(address, data, DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_WriteRfReg(WORD address, BYTE data
#if defined (_MIMO_)
										   , DWORD DeviceId
#endif //#if defined (_MIMO_)
											)
{
#if !defined (_MIMO_)
	DWORD DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ WriteRfReg(address, data, DeviceId);
} 

DUT_WLAN_API int STDCALL DutIf_ReadRfReg(WORD address, BYTE *data
#if defined (_MIMO_)
										   , DWORD DeviceId
#endif //#if defined (_MIMO_)
											)
{
#if !defined (_MIMO_)
	DWORD DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ ReadRfReg(address, data, DeviceId);
} 

DUT_WLAN_API int STDCALL DutIf_WriteBBReg(WORD address, BYTE data
#if defined (_MIMO_)
										   , DWORD DeviceId
#endif //#if defined (_MIMO_)
											)
{
#if !defined (_MIMO_)
	DWORD DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ WriteBBReg(address, data, DeviceId);
} 

DUT_WLAN_API int STDCALL DutIf_ReadBBReg(WORD address, BYTE *data
#if defined (_MIMO_)
										   , DWORD DeviceId
#endif //#if defined (_MIMO_)
											)
{
#if !defined (_MIMO_)
	DWORD DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ ReadBBReg(address, data, DeviceId);
} 

DUT_WLAN_API int STDCALL DutIf_WriteMACReg(DWORD address, DWORD data
#if defined (_MIMO_)
										   , DWORD DeviceId
#endif //#if defined (_MIMO_)
										   )
{
#if !defined (_MIMO_)
		int DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ WriteMACReg(address, data, DeviceId);
}	 

DUT_WLAN_API int STDCALL DutIf_ReadMACReg(DWORD address, DWORD *data
#if defined (_MIMO_)
										  , DWORD DeviceId
#endif //#if defined (_MIMO_)
										  )
{
#if !defined (_MIMO_)
		int DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ ReadMACReg(address, data, DeviceId);
}  
 
DUT_WLAN_API int STDCALL DutIf_WriteSQmem(DWORD address, DWORD data
#if defined (_MIMO_)
										  , DWORD DeviceId
#endif //#if defined (_MIMO_)
										  )
{
#if !defined (_MIMO_)
		int DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ WriteSQmem(address, data, DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_ReadSQmem(DWORD address, DWORD *data
#if defined (_MIMO_)
										  , DWORD DeviceId
#endif //#if defined (_MIMO_)
										 )
{
#if !defined (_MIMO_)
		int DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ ReadSQmem(address, data, DeviceId);
}

DUT_WLAN_API  int STDCALL DutIf_DllVersion(void) 
{ 
	return WLAN_OBJ Version();
}

DUT_WLAN_API char* STDCALL DutIf_About(void)
{
	return WLAN_OBJ About();
}

DUT_WLAN_API int STDCALL DutIf_InitConnection(void **theObj)
{
#ifndef _LINUX_
#ifndef STATIC_OBJ
	if (WlanObj == NULL)
	{
		if (*theObj)
		{
			WlanObj = (DutWlanApiClss*)(*theObj);
		}
		else
		{
			WlanObj = new (DutWlanApiClss);
			*theObj = (void*)WlanObj;
		}
	}
	
//	WlanObj = new (DutWlanApiClss);
#else	//#ifndef STATIC_OBJ
 	WlanObj = &WlanOnlyOne;  
	*theObj = (void*)WlanObj;
#endif	//#ifndef STATIC_OBJ
#endif //#ifndef _LINUX_
	return WLAN_OBJ InitConnection();
}
		 
DUT_WLAN_API int STDCALL DutIf_Disconnection(void  **theObj)
{
	int err=0;

	err = WLAN_OBJ Disconnection();
#ifndef _LINUX_  
#ifndef STATIC_OBJ
#if defined (_IF_SDIO_)
	delete WlanObj;
	WlanObj=NULL; 
	(*theObj) = WlanObj;
#else	// #if defined (_IF_SDIO_)
	(*theObj) = NULL;
#endif	// #if defined (_IF_SDIO_)
#endif	// #ifndef STATIC_OBJ
#endif	// #ifndef _LINUX_   
	return err;
}	 

DUT_WLAN_API int STDCALL DutIf_SetBandAG(int band
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId
#endif //#if defined (_MIMO_)
											   )
{ 
#if !defined (_MIMO_)
	DWORD DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ SetBandAG(band, DeviceId);
}	 
DUT_WLAN_API int STDCALL DutIf_GetBandAG(int *band
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId
#endif //#if defined (_MIMO_)
											   ) 
{ 
#if !defined (_MIMO_)
	DWORD DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ GetBandAG(band, DeviceId);
}	 
 
// int set channel, set power, read RSSI, read .11status, 
DUT_WLAN_API int STDCALL DutIf_SetRfChannel(int channel
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId
#endif //#if defined (_MIMO_)
											   )
{
#if !defined (_MIMO_)
	DWORD DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ SetRfChannel(channel, DeviceId);
}

 
DUT_WLAN_API int STDCALL DutIf_GetRfChannel(int *channel, double *pChFreqInMHz 
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId
#endif //#if defined (_MIMO_)
											   )
{
#if !defined (_MIMO_)
	DWORD DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ GetRfChannel(channel, pChFreqInMHz, DeviceId);
}
 
DUT_WLAN_API int STDCALL DutIf_GetCurrentChannelFreq(double *pChFreqInMHz 
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId
#endif //#if defined (_MIMO_)
											   )
{
#if !defined (_MIMO_)
		int DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ GetRfChannel(NULL, pChFreqInMHz, DeviceId);
}



DUT_WLAN_API int STDCALL DutIf_SetChannelBw(int ChannelBw
#if defined (_MIMO_)
											   ,  
											 DWORD DeviceId
#endif //#if defined (_MIMO_)
											   )
{ 
	return WLAN_OBJ SetChannelBw(ChannelBw
#if defined (_MIMO_)
											   ,  
								DeviceId
#endif //#if defined (_MIMO_)
											 );
}
 

DUT_WLAN_API int STDCALL DutIf_GetChannelBw(int *ChannelBw
#if defined (_MIMO_)
											   ,  
											 DWORD DeviceId
#endif //#if defined (_MIMO_)
											   ) 
{ 
	return WLAN_OBJ GetChannelBw(ChannelBw
#if defined (_MIMO_)
											   ,  
								DeviceId
#endif //#if defined (_MIMO_)
											 );
}
 

DUT_WLAN_API int STDCALL DutIf_GetRfChannelFreq(int band, 
												int channel, 
												double *chFreqInMhz  
#if defined (_MIMO_)
											   ,  
											 DWORD DeviceId
#endif //#if defined (_MIMO_)
											 )
{

	return WLAN_OBJ  GetRfChannelFreq(band, channel, chFreqInMhz  
#if defined (_MIMO_)
											   ,  
								DeviceId
#endif //#if defined (_MIMO_)
											   );
}


DUT_WLAN_API int STDCALL DutIf_SetRfChannelByFreq(double chFreqInMhz 
#if defined (_MIMO_)
											   ,  
											 DWORD DeviceId
#endif //#if defined (_MIMO_)
											   )
{
	return WLAN_OBJ  SetRfChannelByFreq(chFreqInMhz  
#if defined (_MIMO_)
											   ,  
									DeviceId
#endif //#if defined (_MIMO_)
											   );
}

DUT_WLAN_API int STDCALL DutIf_GetSubBand(int *SubBandId, 
											 int *HighFreq, int *LowFreq
#if defined (_MIMO_)
											   ,  
											 DWORD DeviceId
#endif //#if defined (_MIMO_)
											   )
{ 
	return WLAN_OBJ GetSubBand(SubBandId, HighFreq, LowFreq
#if defined (_MIMO_)
											   ,  
								DeviceId
#endif //#if defined (_MIMO_)
											 );
}

DUT_WLAN_API int STDCALL DutIf_SetRfPower(int *pPwr4Pa, 
										   int *pCorrOffset, 
										   int *pCorr1A, 
										   DWORD PathId,  
										   DWORD DeviceId)
{ 
	return WLAN_OBJ SetRfPower(pPwr4Pa, pCorrOffset, pCorr1A, PathId, DeviceId);
}
 
DUT_WLAN_API int STDCALL DutIf_GetRfPowerSetting(BYTE *pPaConf1, BYTE *pPaConf2, 
						 BYTE *pVt_Hi_G,	BYTE *pVt_Lo_G,	BYTE *pInitP_G, 
						 BYTE *pVt_Hi_B,	BYTE *pVt_Lo_B,	BYTE *pInitP_B)
{
	return WLAN_OBJ GetRfPowerSetting(pPaConf1,	pPaConf2,	 
							pVt_Hi_G,	pVt_Lo_G,	pInitP_G, 
							pVt_Hi_B,	pVt_Lo_B,	pInitP_B);
}

DUT_WLAN_API int STDCALL DutIf_GetRfPower(	int *pPwr4Pa,  
												int	*pCorrInitP_G,
												int	*pCorrInitP_B,
												int	*pCorrTh_G,
												int	*pCorrTh_B) 
{ 
	return WLAN_OBJ GetRfPower(pPwr4Pa,  
							pCorrInitP_G,
							pCorrInitP_B,
							pCorrTh_G,
							pCorrTh_B);
}

DUT_WLAN_API int STDCALL DutIf_SetRfVga(int extMode, int value 
#if defined (_MIMO_)
											   ,  
										DWORD PathId,  
										DWORD DeviceId
#endif //#if defined (_MIMO_) 
										)
{ 
#if !defined (_MIMO_)
	DWORD PathId=0,  DeviceId=0;
#endif //#if defined (_MIMO_)
	return WLAN_OBJ SetRfVga(extMode, value, PathId, DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_GetRfVga(int *pExtMode, int *pValue
#if defined (_MIMO_)
											   ,  
										DWORD PathId,  
										DWORD DeviceId
#endif //#if defined (_MIMO_) 
)
{ 
#if !defined (_MIMO_)
	DWORD PathId=0,  DeviceId=0;
#endif //#if defined (_MIMO_)
	return WLAN_OBJ GetRfVga(pExtMode, pValue, PathId, DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_SetRfXTal(int Extension, int Cal
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId
#endif //#if defined (_MIMO_)
											   )
{
#if !defined (_MIMO_)
	DWORD DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ SetRfXTal(Extension, Cal, DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_GetRfXTal(int *pExtension, int *pCal
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId
#endif //#if defined (_MIMO_)
											   )
{
#if !defined (_MIMO_)
	DWORD DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ GetRfXTal(pExtension, pCal, DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_StepRfPower(int *pSteps 
#if defined (_MIMO_)
											   ,  
											  DWORD PathId,
											   DWORD DeviceId
#endif //#if defined (_MIMO_)
)
{ 
	return WLAN_OBJ StepRfPower(pSteps
#if defined (_MIMO_)
								, PathId, DeviceId
#endif //#if defined (_MIMO_)
										);
}

DUT_WLAN_API int STDCALL DutIf_GetRfPowerRange(int *pRangeIndex, int *pTopPwr, int *pBtmPwr
#if defined (_MIMO_)					
												  ,
											  DWORD PathId,  
											  DWORD DeviceId
#endif //#if defined (_MIMO_)
											  )
{ 
	return WLAN_OBJ GetRfPowerRange( pRangeIndex,  pTopPwr,  pBtmPwr
#if defined (_MIMO_)					
												  ,
										PathId,  
										DeviceId
#endif //#if defined (_MIMO_)
										);
}

DUT_WLAN_API int STDCALL DutIf_ClearRxPckts(DWORD DeviceId)
{
	return WLAN_OBJ ClearRxPckt(DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_GetRxPckts(unsigned long *cntRx,
											 unsigned long *cntOk,
											 unsigned long *cntKo,
											 DWORD DeviceId) 
{
	return WLAN_OBJ GetRxPckt(cntRx, cntOk, cntKo, DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_SetRxAntenna(int antSelection,  
												DWORD DeviceId)
{
	return WLAN_OBJ SetRxAntenna(antSelection, DeviceId);
} 

DUT_WLAN_API int STDCALL DutIf_SetTxAntenna(int antSelection,  
												DWORD DeviceId)
{
	return WLAN_OBJ SetTxAntenna(antSelection, DeviceId);
} 

DUT_WLAN_API int STDCALL DutIf_GetRxAntenna(int *antSelection,  
												DWORD DeviceId)
{
	return WLAN_OBJ GetRxAntenna(antSelection, DeviceId);
} 

DUT_WLAN_API int STDCALL DutIf_GetTxAntenna(int *antSelection,  
												DWORD DeviceId)
{
	return WLAN_OBJ GetTxAntenna(antSelection, DeviceId);
} 

DUT_WLAN_API int STDCALL DutIf_SetTxDataRate(int rate,  
												DWORD DeviceId)
{
	return WLAN_OBJ SetTxDataRate(rate, DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_GetTxDataRate(int *rate,  
												DWORD DeviceId)
{
	return WLAN_OBJ GetTxDataRate(rate, DeviceId);
}


DUT_WLAN_API int STDCALL DutIf_SetTxContMode(BOOL enable, BOOL CWmode, 
										DWORD payloadPattern,
										DWORD CSmode,
										DWORD		ActSubCh, 
										DWORD 		DeviceId)
{
	 return WLAN_OBJ SetTxContMode(enable, CWmode, payloadPattern, CSmode, ActSubCh, DeviceId);
}
 
DUT_WLAN_API int STDCALL DutIf_SetTxCarrierSuppression(BOOL enable, 
										DWORD 		DeviceId)
{
	return WLAN_OBJ SetTxCarrierSuppression(enable, DeviceId);
} 
 
DUT_WLAN_API int STDCALL DutIf_SetTxIQCalMode(BOOL enable
#if defined (_MIMO_)
												   , 
												DWORD DeviceId
#endif // #if defined (_MIMO_)
											  )
{
	return WLAN_OBJ SetTxIQCalMode(enable
#if defined (_MIMO_)
									, 
									DeviceId
#endif // #if defined (_MIMO_)
									);
} 
 
DUT_WLAN_API int STDCALL DutIf_SetTxDutyCycle(BOOL enable, int dataRate,  
										int			Percentage, 
										int			payloadPattern, 
										int			ShortPreamble
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
	// Use data rate to determine payload size
	int PayLoadSize=0x600;
	
#if !defined(_W8787_)
	if(dataRate >= DATARATE_MCS0)
	{
		PayLoadSize=0xC00;
	}
#endif  //#if !defined(_W8787_)

	return WLAN_OBJ SetTxDutyCycle(enable, dataRate, Percentage, PayLoadSize,
								payloadPattern, 
								ShortPreamble
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
								DeviceId
#endif // #if defined (_MIMO_)
								);
}  
 
/*
DUT_WLAN_API int STDCALL DutIf_SetTxDutyCycle2(BOOL enable, int dataRate,  
										 int Percentage, 
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
	return WLAN_OBJ SetTxDutyCycle(enable, dataRate, Percentage, PayloadLengthInKB,
								payloadPattern, 
								ShortPreamble
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
								DeviceId
#endif // #if defined (_MIMO_)
								);
}  
*/

DUT_WLAN_API int STDCALL DutIf_SetPowerCycleMode(int cycle, 
													int sleepDur, 
													int awakeDur, 
													int stableXosc,  
													DWORD DeviceId)
{
	return WLAN_OBJ SetPowerCycleMode(  cycle, 
							   sleepDur, 
							   awakeDur, 
							   stableXosc, 
							   DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_EnterSleepMode(void) 
{
	return WLAN_OBJ EnterSleepMode();
}


DUT_WLAN_API int STDCALL DutIf_SetBeaconInterval(DWORD interval, DWORD DeviceId)//in ms
{
	return WLAN_OBJ SetBeaconInterval(interval, DeviceId);
} 

DUT_WLAN_API int STDCALL DutIf_GetBeaconInterval(DWORD *interval, DWORD DeviceId)//in ms
{
	return WLAN_OBJ GetBeaconInterval(interval, DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_SetBeaconEnable(BOOL enable, DWORD DeviceId)
{
	return WLAN_OBJ SetBeaconEnable(enable, DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_GetBeaconEnable(BOOL *enable, DWORD DeviceId)
{
	return WLAN_OBJ GetBeaconEnable(enable, DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_GetHWVersion(WORD *socId, BYTE *socVersion, 
									   BYTE *RfId, BYTE *RfVersion, 
									   BYTE *BbId, BYTE *BbVersion,
									   DWORD DeviceId)
{
	return WLAN_OBJ GetHWVersion(socId, socVersion, 
							RfId, RfVersion, 
							BbId, BbVersion,
							DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_GetORVersion(BYTE OR_ID, BYTE *OR_Major, BYTE *OR_Minor, BYTE *OR_Cust)
{
	return WLAN_OBJ GetORVersion(OR_ID, OR_Major, OR_Minor, OR_Cust);
}

DUT_WLAN_API int STDCALL DutIf_GetFWVersion(BYTE *version)
{
	return WLAN_OBJ GetFWVersion(version);
}

DUT_WLAN_API int STDCALL DutIf_GetFWVerNumber(DWORD *FWVersion, DWORD * MFGFWVersion)
{
	return WLAN_OBJ GetFWVerNumber(FWVersion, MFGFWVersion);
}

DUT_WLAN_API int STDCALL DutIf_GetReleaseNote(char *ReleaseNote)
{
	return WLAN_OBJ GetReleaseNote(ReleaseNote);
}

DUT_WLAN_API int STDCALL DutIf_MacWakeupRfBBP(void)
{
	return WLAN_OBJ MacWakeupRfBBP();
}

DUT_WLAN_API int STDCALL DutIf_EnableBssidFilter(int mode, BYTE *Bssid, char *SSID,  
											   DWORD DeviceId)
{
	return WLAN_OBJ EnableBssidFilter(mode, Bssid, SSID, DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_TxBrdCstPkts(DWORD dataRate,	DWORD pattern, 
									   DWORD length,		DWORD count,
									   int ShortPreamble, char *Bssid							
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
										DWORD 		DeviceId
#endif // #if defined (_MIMO_)
					)
{
	return WLAN_OBJ TxBrdCstPkts(dataRate,		pattern, 
								length, count,
								ShortPreamble,	Bssid							
#if defined (_MIMO_)
								,
								ActSubCh,		ShortGI, 
								AdvCoding, 		TxBfOn,  
								GFMode, 		STBC, 
								DPDEnable,		PowerID,
						 		DeviceId
#endif // #if defined (_MIMO_)
								);
}

DUT_WLAN_API int STDCALL DutIf_AdjustPcktSifs(DWORD Enable, 
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
										DWORD       DPDEnable,
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
	return WLAN_OBJ AdjustPcktSifs(	Enable, dataRate,		pattern, 
									length, ShortPreamble,	Bssid							
#if defined (_MIMO_)
								,
								ActSubCh,		ShortGI, 
								AdvCoding, 		TxBfOn,  
								GFMode, 		STBC, 
								DPDEnable,		PowerID,
								SignalBw,
#if defined (_W8787_) || 	defined (_W8782_)						
							AdjustTxBurstGap, BurstSifsInUs, 
#endif //#if defined (_W8787_) || 	defined (_W8782_)						
						 		DeviceId
#endif // #if defined (_MIMO_)
								);
}
DUT_WLAN_API int STDCALL DutIf_XoscCal(WORD *cal, int TU, DWORD DeviceId)
{ 
	return WLAN_OBJ XoscCal(cal, TU, DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_SoftReset(DWORD DeviceId)
{ 
	return WLAN_OBJ SoftReset(DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_HwResetRf(DWORD DeviceId)
{ 
	return WLAN_OBJ HwResetRf(DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_SetRssiNf(DWORD DeviceId)
{ 
	return WLAN_OBJ SetRssiNf(DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_GetRssiNf(int stop, 
				  int *count, int *rssi, 
				  int *SNR, int *NoiseFloor, DWORD DeviceId)
{ 
	return WLAN_OBJ GetRssiNf(stop, 
						count,	rssi, 
						SNR,		NoiseFloor, DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_ThermalComp(	BOOL enabled, 
											DWORD tempRefAtCal,
											DWORD tempRefSlope,
											DWORD SlopeNumerator, 
											DWORD SlopeNumerator1, 
											DWORD SlopeDenominator,
											DWORD CalibrationIntervalInMS, 
											DWORD PTargetDelta
#if defined (_MIMO_)
												   , 
											DWORD PathId,  
											DWORD DeviceId
#endif //#if defined (_MIMO_)
											  )
{ 
	return WLAN_OBJ ThermalComp(	enabled, tempRefAtCal, tempRefSlope,
									SlopeNumerator, SlopeNumerator1, SlopeDenominator,
									CalibrationIntervalInMS, 
									PTargetDelta
#if defined (_MIMO_)
									, 
									PathId,  
									DeviceId
#endif //#if defined (_MIMO_)
											  );
}

DUT_WLAN_API int STDCALL DutIf_GetThermalComp(	BOOL *pEnabled, 
												DWORD *pTempRefAtCal,
												DWORD *pTempRefSlope,
												DWORD *pSlopeNumerator, 
												DWORD *pSlopeNumerator1, 
												DWORD *pSlopeDenominator,
												DWORD *pCalibrationIntervalInMS, 
												DWORD *pPTargetDelta
#if defined (_MIMO_)
												   , 
												DWORD PathId,  
												DWORD DeviceId
#endif //#if defined (_MIMO_)
											  )
{
	return WLAN_OBJ GetThermalComp(pEnabled, pTempRefAtCal, pTempRefSlope,
									pSlopeNumerator, 
									pSlopeNumerator1, 
									pSlopeDenominator,
									pCalibrationIntervalInMS, 
									pPTargetDelta
#if defined (_MIMO_)
												   , 
									PathId,  
									DeviceId
#endif //#if defined (_MIMO_)
											  );
}

DUT_WLAN_API int STDCALL DutIf_ThermalTest(	BOOL enabled, 
											DWORD PTarget,
											DWORD tempRef, 
											DWORD SlopeNumerator, 
											DWORD SlopeDenominator,
											DWORD CalibrationIntervalInMS,
											DWORD Temp, 
											DWORD SlopeNumerator1, 
											DWORD SlopeDenominator1)
{ 
	return WLAN_OBJ ThermalTest(	enabled, 
									PTarget,
									tempRef, 
									SlopeNumerator, 
									SlopeDenominator,
									CalibrationIntervalInMS,
									Temp, 
									SlopeNumerator1, 
									SlopeDenominator1);
}

DUT_WLAN_API int STDCALL DutIf_GetThermalTest(	BOOL *pEnabled, 
												DWORD *pPTarget,
												DWORD *pTempRef,
												DWORD *pSlopeNumerator, 
												DWORD *pSlopeDenominator,
												DWORD *pCalibrationIntervalInMS, 
												DWORD *pTemp,
												DWORD *pSlopeNumerator1, 
												DWORD *pSlopeDenominator1)
{
	return WLAN_OBJ GetThermalTest(	pEnabled, 
									pPTarget,
									pTempRef,
									pSlopeNumerator, 
									pSlopeDenominator,
									pCalibrationIntervalInMS, 
									pTemp,
									pSlopeNumerator1, 
									pSlopeDenominator1);
}

DUT_WLAN_API int STDCALL DutIf_GetThermalSensorReading(int *pReadBack)
{
	return WLAN_OBJ GetThermalSensorReading(pReadBack);
}

#if defined (_SUPPORT_LDO_)
DUT_WLAN_API int STDCALL DutIf_SetLDOConfig(int LDOSrc)
{ 
	return WLAN_OBJ SetLDOConfig(LDOSrc);
}

DUT_WLAN_API int STDCALL DutIf_GetLDOConfig(int* LDOSrc)
{ 
	return WLAN_OBJ GetLDOConfig(LDOSrc);
}
#endif // #if defined (_SUPPORT_LDO_)

#if defined (_SUPPORT_PM_)
DUT_WLAN_API int STDCALL DutIf_WritePMReg(int addr, int value)
{ 
	return WLAN_OBJ WritePMReg(addr, value);
}

DUT_WLAN_API int STDCALL DutIf_ReadPMReg(int addr, int *value)
{ 
	return WLAN_OBJ ReadPMReg(addr, value);
}

DUT_WLAN_API int STDCALL DutIf_SetPMRail(int railId, int mode, int volt, int powerDown)
{ 
	return WLAN_OBJ SetPMRail(railId, mode, volt, powerDown);
}
#endif // #if defined (_SUPPORT_PM_)
 

DUT_WLAN_API int STDCALL DutIf_ArmReadUlongArray(DWORD armAddr,
												DWORD *pBuf,
												DWORD sizeBytes
#if defined (_MIMO_)
										   , DWORD DeviceId
#endif //#if defined (_MIMO_)
											) 
{
#if !defined (_MIMO_)  
				DWORD DeviceId=0;
#endif //#if !defined (_MIMO_)
    return WLAN_OBJ ArmReadUlongArray(armAddr, pBuf, sizeBytes, DeviceId);
}

//#if defined(_ENGINEERING_)
DUT_WLAN_API int STDCALL DutIf_RunBbpSqDiag(DWORD sizeBytes,
                                             DWORD *pSqBufAddr)
{
	return WLAN_OBJ RunBbpSqDiag(sizeBytes, pSqBufAddr);
}
//#endif // #if defined(_ENGINEERING_)

DUT_WLAN_API int STDCALL DutIf_SetRfPowerUseCal(int *pPwr4Pa, int RateG
#if defined (_MIMO_)
												   , 
											  DWORD PathId,  
											  DWORD DeviceId
#endif //#if defined (_MIMO_)
											  )
{
	return WLAN_OBJ SetRfPowerUseCal(pPwr4Pa,  RateG, NULL, 0
#if defined (_MIMO_)
									, 
									PathId,  
									DeviceId
#endif //#if defined (_MIMO_)
);
}

DUT_WLAN_API int STDCALL DutIf_SetRfPowerUseCalExt(int *pPwr4Pa, int RateG,
										BYTE* cal_data_ext, int len
#if defined (_MIMO_)
												   , 
										DWORD PathId,  
										DWORD DeviceId
#endif //#if defined (_MIMO_)
)
{
	return WLAN_OBJ SetRfPowerUseCal(pPwr4Pa,  RateG, cal_data_ext, len
#if defined (_MIMO_)
									, 
									PathId,  
									DeviceId
#endif //#if defined (_MIMO_)
);
}

DUT_WLAN_API int STDCALL DutIf_LoadHwData(BOOL *CsC,				BYTE *pStructRev, 
				BYTE *PcbRev, 
				BOOL *pExtPaPolar_neg,	BOOL *pPa_External, 
				BYTE *pExtPA_PartId,	 	 
				BYTE *pAnt,				BYTE *pAntBT,	 	 
				BOOL *pCalTableOption,   
				BYTE *pCalRateB,		BYTE *pCalRateG,
				BYTE *pCalRng1,			BYTE *pCalRng2,
				BYTE *pCalRng3,			BYTE *pCalRng4,
				BYTE *pCC,				 
 				BYTE *pAssemblyHouse,	DWORD *pSN, 
				BYTE *pSocVer,			BYTE *pRfVer, 
				BYTE *pSpiSizeInfo,		BYTE *pSpiUnitInfo,		
				BYTE *pAntPartInfo, 
				WORD *pTestToolVer,		WORD *pDllVer)
{
	return SHARED_OBJ LoadHwData(CsC, pStructRev, 
					PcbRev, 
					pExtPaPolar_neg,	pPa_External, 
					pExtPA_PartId,	 	 
					pAnt,				pAntBT,	 	 
					pCalTableOption,   
					pCalRateB,			pCalRateG,
					pCalRng1,			pCalRng2,
					pCalRng3,			pCalRng4,
					pCC,				 
 					pAssemblyHouse,		pSN, 
					pSocVer,			pRfVer, 
					pSpiSizeInfo,		pSpiUnitInfo,		
					pAntPartInfo, 
					pTestToolVer,		pDllVer);  
} 

DUT_WLAN_API int STDCALL DutIf_EraseEEPROM(int section)
{
	return WLAN_OBJ Wifi_EraseEEPROM(section);
}

DUT_WLAN_API int STDCALL DutIf_GetMACAddress(BYTE *ByteArray)
{
	return WLAN_OBJ GetMACAddress(SharedObj,
		 					ByteArray);
}

DUT_WLAN_API int STDCALL DutIf_SetMACAddress(BYTE *ByteArray)
{
	return WLAN_OBJ SetMACAddress(SharedObj, 
							ByteArray);
}

DUT_WLAN_API int STDCALL DutIf_SetPIDVID(DWORD E2pOffset,  WORD PID, WORD VID)
{
	return WLAN_OBJ SetPIDVID(SharedObj, E2pOffset, PID, VID);
}

DUT_WLAN_API int STDCALL DutIf_GetPIDVID(DWORD E2pOffset,  WORD *PID, WORD *VID)
{
	return WLAN_OBJ GetPIDVID(SharedObj, E2pOffset, PID, VID);
}

DUT_WLAN_API int STDCALL DutIf_SetClassId(DWORD E2pOffset,  DWORD Class)
{
	return WLAN_OBJ SetClassId(SharedObj, E2pOffset, Class);
}

DUT_WLAN_API int STDCALL DutIf_GetClassId(DWORD E2pOffset,  DWORD *Class)
{
	return WLAN_OBJ GetClassId(SharedObj, E2pOffset, Class);
}

DUT_WLAN_API int STDCALL DutIf_DumpE2PData( DWORD E2pOffset,  DWORD Len, 
					 DWORD *AddressArray, DWORD *DwordArray )
{
	return SHARED_OBJ DumpE2PData(DEVICETYPE_WLAN, WlanObj, 
								E2pOffset, Len, AddressArray, DwordArray );
}

 
DUT_WLAN_API int STDCALL DutIf_WriteLumpedData( DWORD E2pOffset,  DWORD Len, DWORD *DwordArray )
{
	return SHARED_OBJ WriteLumpedData(DEVICETYPE_WLAN, WlanObj, 
								E2pOffset, Len, DwordArray );
}

DUT_WLAN_API int STDCALL DutIf_SetRfControlMode(DWORD ThermalOnly
#if defined (_MIMO_)
												   ,  
											  DWORD DeviceId
#endif //#if defined (_MIMO_)
											  )
{
	return WLAN_OBJ SetRfControlMode(ThermalOnly
#if defined (_MIMO_)
												   ,  
											  DeviceId
#endif //#if defined (_MIMO_)
											  );
}

DUT_WLAN_API int STDCALL DutIf_GetRfControlMode(DWORD *pThermalOnly
#if defined (_MIMO_)
												   ,  
											  DWORD DeviceId
#endif //#if defined (_MIMO_)
											  )
{
	return WLAN_OBJ GetRfControlMode(pThermalOnly
#if defined (_MIMO_)
												   ,  
											  DeviceId
#endif //#if defined (_MIMO_)
											  );
}

//jsz
/*DUT_WLAN_API int STDCALL DutIf_CloseLoopCal(
				 BYTE targetInitP, int maxThreshHold, int steps, int dataRate,
				 BYTE *initP, BYTE *threshHi, BYTE *threshLow)
{
	return WLAN_OBJ CloseLoopCal(targetInitP, maxThreshHold, steps, dataRate,
				 initP, threshHi, threshLow);
}


DUT_WLAN_API int STDCALL DutIf_SetPaThermalScaler(int scaler
#if defined (_MIMO_)
												   , 
											  DWORD PathId,  
											  DWORD DeviceId
#endif //#if defined (_MIMO_)
											  )
{
	return WLAN_OBJ SetPaThermalScaler(scaler
#if defined (_MIMO_)
												   , 
											  PathId,  
											  DeviceId
#endif //#if defined (_MIMO_)
											  );
}

DUT_WLAN_API int STDCALL DutIf_GetPaThermalScaler(int *scaler
#if defined (_MIMO_)
												   , 
											  DWORD PathId,  
											  DWORD DeviceId
#endif //#if defined (_MIMO_)
											  )
{
	return WLAN_OBJ GetPaThermalScaler(scaler
#if defined (_MIMO_)
											, 
										PathId,  
										DeviceId
#endif //#if defined (_MIMO_)
											  );
}
*/ //jsz

DUT_WLAN_API int STDCALL DutIf_WritePwrTableFile0( int DeviceId, int PathId, 
						int bandId, int tableId,  
					  int *sizeOfTblInByte, void* tbl, char* GivenFileName) 

{
		return SHARED_OBJ WritePwrTableFile( 
#if defined (_MIMO_)
			DeviceId, PathId, 
#endif // #if defined (_MIMO_)
			bandId, tableId, 
					   sizeOfTblInByte, (unsigned char*)tbl,  GivenFileName);
}

DUT_WLAN_API int STDCALL DutIf_SetRfPwrInitP(	BYTE InitP, 
						DWORD PathId,	
						DWORD DeviceId)
{ 
	return WLAN_OBJ SetRfPwrInitP(InitP, 
						PathId,	
						DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_GetRfPwrVgaMax(	BYTE *pVgaMax, 
						DWORD PathId,	
						DWORD DeviceId)
{ 
	return WLAN_OBJ GetRfPwrVgaMax(pVgaMax, 
						PathId,	
						DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_GetBandSubBandTag(DWORD Band, DWORD SubBand, char *Tag)
{
	return GetBandSubBandTag(Band, SubBand, Tag);
}

DUT_WLAN_API int STDCALL DutIf_DownLoadStaticSettingsToFw(  
									DWORD BandId, DWORD SubbandId, 
									DWORD PathId, 
 									BYTE RfLnaGrif,
 									BYTE RfLnaGrBb,
									DWORD DeviceId)
{
	return  WLAN_OBJ DownLoadStaticSettingsToFw(
									BandId, SubbandId, 
									PathId, 
 									RfLnaGrif,
 									RfLnaGrBb,
									DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_UpLoadStaticSettingsFromFw( 
									DWORD BandId, DWORD SubbandId, 
									DWORD PathId, 
 									BYTE *pRfLnaGrif,
 									BYTE *pRfLnaGrBb,
									DWORD DeviceId)
{	
	return  WLAN_OBJ UpLoadStaticSettingsFromFw(BandId, SubbandId, 
									PathId, 
 									pRfLnaGrif,
 									pRfLnaGrBb,
									DeviceId); 
}

//jsz DUT_WLAN_API int STDCALL DutIf_GetExtPaFileName(DWORD DeviceId, DWORD PathId, char *FileName) 
//jsz {
//jsz 	return GetExtPaFileName(DeviceId, PathId, FileName);
//jsz }

DUT_WLAN_API int STDCALL DutIf_GetFEMFileName(char *FileName) 
{
	return GetFEMFileName(FileName);
}

DUT_WLAN_API int STDCALL DutIf_UpLoadPwrTable(	int BandId, int tableId,  
 												int *sizeOfTblInByte, 
 												BYTE* tbl,		
												DWORD PathId,	
												DWORD DeviceId)
{
 	return WLAN_OBJ UpLoadPwrTable(	BandId, tableId,   sizeOfTblInByte, tbl,   
									PathId, DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_DownLoadPwrTable(int BandId, int tableId,  
 											 int *sizeOfTblInByte, 
 											 BYTE* tbl, int UsePassDownData,
											 DWORD PathId,	DWORD DeviceId)
{
 	return WLAN_OBJ DownLoadPwrTable(BandId, tableId, sizeOfTblInByte, tbl, 
		UsePassDownData, PathId, DeviceId);
}
 
DUT_WLAN_API int STDCALL DutIf_LoadCustomizedSettings(void)
{
 	return SHARED_OBJ LoadCustomizedSettings(DEVICETYPE_WLAN, WlanObj);
}

DUT_WLAN_API int STDCALL DutIf_SetCustomizedSettings(char *FileName)
{
 	return WLAN_OBJ SetCustomizedSettings(FileName);
}

DUT_WLAN_API int STDCALL DutIf_GetCustomizedSettings(int *Setting)
{
 	return WLAN_OBJ GetCustomizedSettings(Setting);
}
 
DUT_WLAN_API int STDCALL DutIf_ReloadCalData(void)
{
 	return WLAN_OBJ ReloadCalData();
}
 
DUT_WLAN_API int STDCALL DutIf_CheckCalDataSpace(int* pAvalable) 
{
 	return  SHARED_OBJ CheckCalDataSpace(DEVICETYPE_WLAN, WlanObj, pAvalable);
}

DUT_WLAN_API int STDCALL DutIf_GetHeaderPresent(int *pPresent, 
										   DWORD *pVersion, int *CsC) 
{
 	return SHARED_OBJ GetHeaderPresent(DEVICETYPE_WLAN, WlanObj, 
										pPresent, pVersion, CsC);
}

DUT_WLAN_API int STDCALL DutIf_GetTxIQValue(int *pExtMode  , DWORD *pAmp, DWORD *pPhase
#if defined (_MIMO_)
												   , 
											  DWORD PathId,  
											  DWORD DeviceId
#endif //#if defined (_MIMO_)
)
{
 	return WLAN_OBJ GetTxIQValue(pExtMode, pAmp, pPhase
#if defined (_MIMO_)
												   , 
											  PathId,  
											  DeviceId
#endif //#if defined (_MIMO_)
											);
}

DUT_WLAN_API int STDCALL DutIf_SetTxIQValue(int ExtMode  , DWORD Amp, DWORD Phase
#if defined (_MIMO_)
												   , 
											  DWORD PathId,  
											  DWORD DeviceId
#endif //#if defined (_MIMO_)
)
{
 	return WLAN_OBJ SetTxIQValue(ExtMode , Amp, Phase
#if defined (_MIMO_)
												   , 
								PathId,  
								DeviceId
#endif //#if defined (_MIMO_)
);
}

DUT_WLAN_API int STDCALL DutIf_GetTxIQEstimatedValue(DWORD *pAmp, 
														DWORD *pPhase) 
{
 	return WLAN_OBJ GetTxIQEstimatedValue(pAmp, pPhase);
}

DUT_WLAN_API int STDCALL DutIf_PerformTxIQCal(DWORD Avg,
												 DWORD *pTxAmp, 
												   DWORD *pTxPhase)   
{
#if 1
 	return WLAN_OBJ PerformTxIQCal_Host(Avg, pTxAmp, pTxPhase ) ;
#else
 	return WLAN_OBJ PerformTxIQCal(Avg, pTxAmp, pTxPhase ) ;
#endif
}


DUT_WLAN_API int STDCALL DutIf_PerformTxRxIQCal(	DWORD Avg,
													DWORD *pTxAmp, 
													DWORD *pTxPhase, 
													DWORD *pRxAmp, 
													DWORD *pRxPhase)   
{
#if defined(_W8782_)
 	return WLAN_OBJ PerformTxRxIQCal_Host(Avg, pTxAmp, pTxPhase, 
						 pRxAmp, pRxPhase) ;
#else
 	return WLAN_OBJ PerformTxRxIQCal(Avg, pTxAmp, pTxPhase, 
						 pRxAmp, pRxPhase) ;
#endif //#if defined(_W8782_)
}

DUT_WLAN_API int STDCALL DutIf_GetRxIQ(int *pExtMode, 
											   DWORD *pAmp, 
											   DWORD *pPhase
#if defined (_MIMO_)
												   , 
											  DWORD PathId,  
											  DWORD DeviceId
#endif //#if defined (_MIMO_)
											   ) 
{
 	return WLAN_OBJ GetRxIQ(pExtMode, pAmp, pPhase
#if defined (_MIMO_)
							, 
							PathId,  
							DeviceId
#endif //#if defined (_MIMO_)
											   );
}

DUT_WLAN_API int STDCALL DutIf_SetRxIQ(int ExtMode, 
											   DWORD Amp, 
											   DWORD Phase
#if defined (_MIMO_)
												   , 
											  DWORD PathId,  
											  DWORD DeviceId
#endif //#if defined (_MIMO_)
											   )
{
 	return WLAN_OBJ SetRxIQ(ExtMode, Amp, Phase
#if defined (_MIMO_)
								, 
								PathId,  
								DeviceId
#endif //#if defined (_MIMO_)
											   );
}

DUT_WLAN_API int STDCALL DutIf_GetRxIQEstimated(DWORD *pAmp, 
														DWORD *pPhase
#if defined (_MIMO_)
												   , 
											  DWORD PathId,  
											  DWORD DeviceId
#endif //#if defined (_MIMO_)
											   )
{
 	return WLAN_OBJ GetRxIQEstimated(pAmp, pPhase
#if defined (_MIMO_)
								, 
								PathId,  
								DeviceId
#endif //#if defined (_MIMO_)
											   );
}

DUT_WLAN_API int STDCALL DutIf_SetCalDataRevC( 
									   int PurgeAfter, 
									   char *FlexFileNameNoE2prom) 
{
 	return SHARED_OBJ SetCalDataRevC(DEVICETYPE_WLAN, WlanObj, 
									PurgeAfter, 							  
									FlexFileNameNoE2prom);
}

DUT_WLAN_API int STDCALL DutIf_GetCalDataRevC( 
									   int *CsC, int *CsCValue, 
									   char *FlexFileNameNoE2prom)  
{
 	return SHARED_OBJ GetCalDataRevC(DEVICETYPE_WLAN, WlanObj, 
									CsC, CsCValue, 							  
									FlexFileNameNoE2prom);
}

DUT_WLAN_API int STDCALL DutIf_ForceE2PromType(DWORD IfType, 
											   DWORD AddrWidth, 
											   DWORD DeviceType)
{
	return WLAN_OBJ ForceE2PromType(IfType, AddrWidth, DeviceType);
}

DUT_WLAN_API int STDCALL DutIf_GetE2PromType(DWORD *pIfType, 
											 DWORD *pAddrWidth,	
											 DWORD *pDeviceType)
{
	return WLAN_OBJ GetE2PromType(pIfType, pAddrWidth, pDeviceType);
}

DUT_WLAN_API int STDCALL DutIf_SetFEM(DWORD FE_VER,
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
	return WLAN_OBJ SetFEM(	FE_VER,
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

DUT_WLAN_API int STDCALL DutIf_FlexSpiDL(bool erase, char *fileName)
{
 	return SHARED_OBJ FlexSpiDL(DEVICETYPE_WLAN, WlanObj, erase, fileName);
}

DUT_WLAN_API int STDCALL DutIf_FlexSpiCompare(char *fileName)  
{
	return SHARED_OBJ FlexSpiCompare(DEVICETYPE_WLAN, WlanObj, fileName); 
} 

DUT_WLAN_API int STDCALL DutIf_CompareE2PData(DWORD  *RefArray,int SizeInByte)
{
	return SHARED_OBJ CompareE2PData(DEVICETYPE_WLAN, WlanObj, 
					 RefArray, SizeInByte);
}

DUT_WLAN_API int STDCALL DutIf_TxFastCal(BYTE *DataStruc, int len)
{  
	return WLAN_OBJ TxFastCal(DataStruc, len);
}

DUT_WLAN_API int STDCALL DutIf_TxFastVer(BYTE *DataStruc, int len)
{  
	return WLAN_OBJ TxFastVer(DataStruc, len);
}

DUT_WLAN_API int STDCALL DutIf_RxFastVer(BOOL enable, int channel, BYTE *Bssid, char *SSID)
{  
	return WLAN_OBJ RxFastVer(enable, channel, Bssid, SSID);
}

DUT_WLAN_API int STDCALL DutIf_SetModuleTypeConf(int ModuleType) 
{  
	return WLAN_OBJ SetModuleTypeConf(ModuleType);
}

DUT_WLAN_API int STDCALL DutIf_GetModuleTypeConf(int *pModuleType)
{  
	return WLAN_OBJ GetModuleTypeConf(pModuleType);
}


DUT_WLAN_API int STDCALL DutIf_SetExtLnaNsettings(int mode)
{  
#if defined(_W8787_)
	return WLAN_OBJ SetExtLnaNsettings(mode, 0, 0, 0, 0, 0, 0);
#else
	return 0;
#endif //defined(_W8787_)
}

DUT_WLAN_API int STDCALL DutIf_GetExtLnaNsettings(int *pMode)
{  
#if defined(_W8787_)
	return WLAN_OBJ GetExtLnaNsettings(pMode, NULL, NULL, NULL, NULL, NULL, NULL);
#else
	return 0;
#endif //defined(_W8787_)
}

DUT_WLAN_API int STDCALL DutIf_WriteCAUReg(WORD address, BYTE data
#if defined (_MIMO_)
										   , DWORD DeviceId
#endif //#if defined (_MIMO_)
										   )
{
#if !defined (_MIMO_)
		int DeviceId=0;
#endif //#if !defined (_MIMO_)

	return WLAN_OBJ WriteCAUReg(address, data, DeviceId);
} 

DUT_WLAN_API int STDCALL DutIf_ReadCAUReg(WORD address, BYTE *data
#if defined (_MIMO_)
										  , DWORD DeviceId
#endif //#if defined (_MIMO_)
										  )
{
#if !defined (_MIMO_)
		int DeviceId=0;
#endif //#if !defined (_MIMO_)
	return WLAN_OBJ ReadCAUReg(address, data, DeviceId);
} 

DUT_WLAN_API int STDCALL DutIf_LoadRFUFW(DWORD deviceId, char *FileName)
{
	return WLAN_OBJ LoadRFUFW(deviceId, FileName);
} 

DUT_WLAN_API int STDCALL DutIf_UpdateRFUFW(DWORD deviceId)
{
	return WLAN_OBJ UpdateRFUFW(deviceId);
} 

#ifndef _LINUX_
#if defined(_FLASH_) 
DUT_WLAN_API int STDCALL DutIf_CheckMacAddressSpace(int* pAvalable) 
{
	return WLAN_OBJ CheckMacAddressSpace(SharedObj,  pAvalable);
}
#endif // #if defined(_FLASH_) 
#endif 	//#ifndef _LINUX_

#if defined(_IF_USB_) 
DUT_WLAN_API int STDCALL DutIf_SetUsbAttributes(BYTE Attribute, BYTE MaxPower) 
{
	return SHARED_OBJ SetUsbAttributes(DEVICETYPE_WLAN, WlanObj, 
										 Attribute, MaxPower);
}

DUT_WLAN_API int STDCALL DutIf_GetUsbAttributes(BYTE *pAttribute, BYTE *pMaxPower)
{
	return SHARED_OBJ GetUsbAttributes(DEVICETYPE_WLAN, WlanObj, 
										 pAttribute, pMaxPower);
}

#endif //#if defined(_IF_USB_) 

//RSSI cal
#if defined (_W8787_)
DUT_WLAN_API int STDCALL DutIf_WlanRSSI_CalOneCh_CW(int RSSI_ch, char *RSSI_val, char *NF_val, int Band, DWORD DeviceId)
{
	return WLAN_OBJ WlanRSSI_CalOneCh_CW(RSSI_ch, RSSI_val, NF_val, Band, DeviceId);
}
DUT_WLAN_API int STDCALL DutIf_WlanRSSI_CalOneCh_Pkt(int RSSI_ch, 
						 char *RSSI_val,
						 char *NF_val,
						 int Band, DWORD DeviceId)
{
	return WLAN_OBJ WlanRSSI_CalOneCh_Pkt(RSSI_ch, 
											RSSI_val,
											NF_val,
											Band,
											DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_GetCalculatedRSSI(char *RSSI_calc
#if defined (_MIMO_)
        , DWORD DeviceId
#endif  //#if defined (_MIMO_)
        )
{
int status=0;
char RSSI_val=0;
char NF_val=0;
char RSSI_ofdm=0;
char Rssi_Thermal=0;
int band=0;
int channel;
double ChFreqInMHz;
THERMALCALPARMS_VD	ThermalCalParam[5];
	
	*RSSI_calc = 0x00;
	// Get current band
	status = DutIf_GetBandAG(&band, DeviceId);
	if(status)
	{
		return status;
	}
	status = DutIf_GetRfChannel(&channel, &ChFreqInMHz, DeviceId);
	if(status)
	{
		return status;
	}
	status = DutIf_WlanRSSI_CalOneCh_Pkt(channel, &RSSI_val, &NF_val, band, DeviceId);
	if(status)
	{
		return status;
	}
	status = DutIf_GetCurrentRSSICalOffset(&RSSI_ofdm, NULL, NULL,NULL, DeviceId);
	if(status)
	{
		return status;
	}

	status = DutIf_GetCurrentRSSIThermalOffset(&Rssi_Thermal, DeviceId);

	*RSSI_calc = 0 - (RSSI_val - RSSI_ofdm - Rssi_Thermal);

	return status;
}

DUT_WLAN_API int STDCALL DutIf_GetCurrentRSSIThermalOffset(char *pRssi_Thermal
#if defined (_MIMO_)
													 , DWORD DeviceId
#endif  //#if defined (_MIMO_)
															)
{
	int CsC=0;
    int CsCValue=0;
    int status=0;
	int band=0;
	int subBand=0;
	int channel;
	int sensorReading=0, tempint;
	double ChFreqInMHz;
	THERMALCALPARMS_VD ThermalCalParam[5];
	BYTE NoOfEntry;
	char RSSICalNum_normal, TempCalTime;

	// Get current band
	status = DutIf_GetBandAG(&band, DeviceId);
	// Get current channel, freq
	status = DutIf_GetRfChannel(&channel, &ChFreqInMHz, DeviceId);

	// if 5G, get current sub-band
	if(band == 1)
	{
		subBand=-1;
		if((ChFreqInMHz >= 4900) && (ChFreqInMHz <= 5100))
		{
			subBand=0;
		}
		if((ChFreqInMHz >= 5150) && (ChFreqInMHz <= 5350))
		{
			subBand=1;
		}
		if((ChFreqInMHz >= 5400) && (ChFreqInMHz <= 5720))
		{
			subBand=2;
		}
		if((ChFreqInMHz >= 5730) && (ChFreqInMHz <= 5850))
		{
			subBand=3;
		}

		if(subBand == -1)
		{
			return ERROR_DATANOTAVAILABLE;
		}
	}

	NoOfEntry = 5;
	status = Dut_Shared_GetAnnexType42Data_VD(&NoOfEntry, ThermalCalParam);
	RSSICalNum_normal = ThermalCalParam[band+subBand].RSSICalNum_normal;
	TempCalTime = ThermalCalParam[band+subBand].TempCalTime;

	status = DutIf_GetThermalSensorReading(&sensorReading);

	// calculate RSSI thermal offset
	tempint = ((sensorReading - (int)TempCalTime) * (int)RSSICalNum_normal)/RSSICAL_DENUM;
	if(pRssi_Thermal)
	{
		*pRssi_Thermal = (char)tempint;
	}

	return 0;
}

DUT_WLAN_API int STDCALL DutIf_GetCurrentRSSICalOffset(char *pRssi_OFDM, 
													   char *pRssi_OFDM_Bypass,
													   char *pRssi_CCK,
													   char *pRssi_CCK_Bypass
#if defined (_MIMO_)
													 , DWORD DeviceId
#endif  //#if defined (_MIMO_)
													 )
{
    int CsC=0;
    int CsCValue=0;
    int status=0;
    int band=0; 
    int subBand=0;
    int channel;
    double ChFreqInMHz;

    char pRssi_Nf[NUM_SUBBAND_A];
    char pRssi_cck[NUM_SUBBAND_A];
    char pRssi_ofdm[NUM_SUBBAND_A];
    char pRssi_mcs[NUM_SUBBAND_A];
    char Rssi_OFDM, Rssi_OFDM_Bypass, Rssi_CCK, Rssi_CCK_Bypass;

	// Get current band
	status = DutIf_GetBandAG(&band, DeviceId);
	// Get current channel, freq
	status = DutIf_GetRfChannel(&channel, &ChFreqInMHz, DeviceId);

	// if 5G, get current sub-band
	if(band == 1)
	{
		subBand=-1;
		if((ChFreqInMHz >= 4900) && (ChFreqInMHz <= 5100))
		{
			subBand=0;
		}
		if((ChFreqInMHz >= 5150) && (ChFreqInMHz <= 5350))
		{
			subBand=1;
		}
		if((ChFreqInMHz >= 5400) && (ChFreqInMHz <= 5720))
		{
			subBand=2;
		}
		if((ChFreqInMHz >= 5730) && (ChFreqInMHz <= 5850))
		{
			subBand=3;
		}

		if(subBand == -1)
		{
			return ERROR_DATANOTAVAILABLE;
		}
	}
	
	// if current band is 2.4G
	if(band == 0)
	{
		status = Dut_Shared_GetAnnexType31Data(	DeviceId, 
												pRssi_Nf, 
												pRssi_cck, 
												pRssi_ofdm,
												pRssi_mcs);

		if(status == ERROR_DATANOTAVAILABLE)
		{
			status = DutIf_GetCalDataRevC(&CsC, &CsCValue, NULL);
			status = Dut_Shared_GetAnnexType31Data(	DeviceId, 
													pRssi_Nf, 
													pRssi_cck, 
													pRssi_ofdm,
													pRssi_mcs);
		}
		if(status != ERROR_DATANOTAVAILABLE)
		{
			Rssi_OFDM			= pRssi_mcs[0];
			Rssi_OFDM_Bypass	= pRssi_ofdm[0];
			Rssi_CCK			= pRssi_cck[0];
			Rssi_CCK_Bypass		= pRssi_Nf[0];
		}
	}
	else // if current band is 5G
	{
		status = Dut_Shared_GetAnnexType32Data(	DeviceId, 
												pRssi_Nf, 
												pRssi_cck, 
												pRssi_ofdm,
												pRssi_mcs);

		if(status == ERROR_DATANOTAVAILABLE)
		{
			status = DutIf_GetCalDataRevC(&CsC, &CsCValue, NULL);
			status = Dut_Shared_GetAnnexType32Data(	DeviceId, 
													pRssi_Nf, 
													pRssi_cck, 
													pRssi_ofdm,
													pRssi_mcs);
		}
		if(status != ERROR_DATANOTAVAILABLE)
		{
			Rssi_OFDM			= pRssi_mcs[subBand];
			Rssi_OFDM_Bypass	= pRssi_ofdm[subBand];
			Rssi_CCK			= pRssi_cck[subBand];
			Rssi_CCK_Bypass		= pRssi_Nf[subBand];
		}
	}

	if(status == ERROR_DATANOTAVAILABLE)
	{
			// No RSSI cal data available 
			return ERROR_DATANOTAVAILABLE;
	}
	else
	{
		if(pRssi_OFDM)
		{
			*pRssi_OFDM = Rssi_OFDM;
		}
		if(pRssi_OFDM_Bypass)
		{
			*pRssi_OFDM_Bypass = Rssi_OFDM_Bypass;
		}
		if(pRssi_CCK)
		{
			*pRssi_CCK = Rssi_CCK;
		}
		if(pRssi_CCK_Bypass)
		{
			*pRssi_CCK_Bypass = Rssi_CCK_Bypass;
		}
	}
	
	return 0;
}


#endif //#if defined (_W8787_)
//DPD API
#ifdef _W8782_

DUT_WLAN_API int STDCALL DutIf_SetCompensation(DWORD PowerID, DWORD Memory2Row, DWORD *pCompensationTbl
#if defined (_MIMO_)
										  , DWORD DeviceId
#endif //#if defined (_MIMO_)
)
{
#if !defined (_MIMO_)
		int DeviceId=0;
#endif //#if !defined (_MIMO_)

	return WLAN_OBJ SetDPD(PowerID, Memory2Row, pCompensationTbl, DeviceId);
}

DUT_WLAN_API int STDCALL DutIf_GetCompensation(DWORD PowerID, DWORD Memory2Row, DWORD *pCompensationTbl
#if defined (_MIMO_)
										  , DWORD DeviceId
#endif //#if defined (_MIMO_)
)
{
#if !defined (_MIMO_)
		int DeviceId=0;
#endif //#if !defined (_MIMO_)

	return WLAN_OBJ GetDPD(PowerID, Memory2Row, pCompensationTbl, DeviceId);
}

#endif //#ifdef _W8782_



