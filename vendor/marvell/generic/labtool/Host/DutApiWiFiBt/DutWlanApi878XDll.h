/** @file DutWlanApi878XDll.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#ifndef _DUTAPI878XDLL_H_
#define _DUTAPI878XDLL_H_
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DUTAPICFDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DUTAPICFDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

#include "DutApi878XDll.h" 

 
DUT_WLAN_API int STDCALL DutIf_PokeRegDword(DWORD address, DWORD data
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
DUT_WLAN_API int STDCALL DutIf_PeekRegDword(DWORD address, DWORD *data
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
 
DUT_WLAN_API int STDCALL DutIf_PokeRegWord(DWORD address, WORD data
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
DUT_WLAN_API int STDCALL DutIf_PeekRegWord(DWORD address, WORD *data
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
DUT_WLAN_API int STDCALL DutIf_PokeRegByte(DWORD address, BYTE data
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
DUT_WLAN_API int STDCALL DutIf_PeekRegByte(DWORD address, BYTE *data
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
 
 
DUT_WLAN_API int STDCALL DutIf_WriteRfReg(WORD address, BYTE data
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
DUT_WLAN_API int STDCALL DutIf_ReadRfReg(WORD address, BYTE *data
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
DUT_WLAN_API int STDCALL DutIf_WriteBBReg(WORD address, BYTE data
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
DUT_WLAN_API int STDCALL DutIf_ReadBBReg(WORD address, BYTE *data
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
DUT_WLAN_API int STDCALL DutIf_WriteMACReg(DWORD address, DWORD data
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
DUT_WLAN_API int STDCALL DutIf_ReadMACReg(DWORD address, DWORD *data
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );  
 
DUT_WLAN_API int STDCALL DutIf_WriteSQmem(DWORD address, DWORD data
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
DUT_WLAN_API int STDCALL DutIf_ReadSQmem(DWORD address, DWORD *data
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );

DUT_WLAN_API int STDCALL DutIf_DllVersion(void); 
DUT_WLAN_API char* STDCALL DutIf_About(void);

DUT_WLAN_API int STDCALL DutIf_InitConnection(void **theObj);		 
DUT_WLAN_API int STDCALL DutIf_Disconnection(void  **theObj);	  

 
DUT_WLAN_API int STDCALL DutIf_SetBandAG(int band
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );

DUT_WLAN_API int STDCALL DutIf_GetBandAG(int *band
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );

// int set channel, set power, read RSSI, read .11status, 
DUT_WLAN_API int STDCALL DutIf_SetRfChannel(int channel
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
DUT_WLAN_API int STDCALL DutIf_GetRfChannel(int *channel, double *pChFreqInMHz
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
DUT_WLAN_API int STDCALL DutIf_GetCurrentChannelFreq(double *pChFreqInMHz
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );

DUT_WLAN_API int STDCALL DutIf_SetChannelBw(int ChannelBw
#if defined (_MIMO_)
											   ,  
											 DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );

DUT_WLAN_API int STDCALL DutIf_GetChannelBw(int *ChannelBw
#if defined (_MIMO_)
											   ,  
											 DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
DUT_WLAN_API int STDCALL DutIf_GetSubBand(int *SubBandId, 
											 int *HighFreq=NULL, int *LowFreq=NULL
#if defined (_MIMO_)
											   ,  
											 DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );

DUT_WLAN_API int STDCALL DutIf_GetRfChannelFreq(int band, int channel, double *chFreqInMhz  
#if defined (_MIMO_)
											   ,  
											 DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
DUT_WLAN_API int STDCALL DutIf_SetRfChannelByFreq(double chFreqInMhz  
#if defined (_MIMO_)
											   ,  
											 DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );

DUT_WLAN_API int STDCALL DutIf_SetRfPower(int *pPwr4Pa, 
										   int *pCorrOffset, 
										   int *pCorr1A,  
										  DWORD PathId=TXPATH_PATHA, 
										  DWORD DeviceId=MIMODEVICE0); 

DUT_WLAN_API int STDCALL DutIf_GetRfPowerSetting(BYTE *pPaConf1, BYTE *pPaConf2, 
						 BYTE *pVt_Hi_G,	BYTE *pVt_Lo_G,	BYTE *pInitP_G, 
						 BYTE *pVt_Hi_B,	BYTE *pVt_Lo_B,	BYTE *pInitP_B);	// TBD_KY_PWR
 

DUT_WLAN_API int STDCALL DutIf_GetRfPower(	int *pPwr4Pa, 
												int	*pCorrInitP_G,
												int	*pCorrInitP_B,
												int	*pCorrTh_G,
												int	*pCorrTh_B);				// TBD_KY_PWR

DUT_WLAN_API int STDCALL DutIf_StepRfPower(int *pSteps 
#if defined (_MIMO_)
											   ,  
											  DWORD PathId=TXPATH_PATHA,
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
 

DUT_WLAN_API int STDCALL DutIf_SetRfVga(int extMode, int value
#if defined (_MIMO_)
											   ,  
											  DWORD PathId=TXPATH_PATHA,  
											  DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
DUT_WLAN_API int STDCALL DutIf_GetRfVga(int *pExtMode, int *pValue
#if defined (_MIMO_)
											   ,   
											  DWORD PathId=TXPATH_PATHA,  
											  DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
//DUT_WLAN_API int STDCALL DutIf_SetRfLcCap(int extMode, int value);
//DUT_WLAN_API int STDCALL DutIf_GetRfLcCap(int *pExtMode, int *pValue);
DUT_WLAN_API int STDCALL DutIf_SetRfXTal(int Extension, int Cal
#if defined (_MIMO_)
											   ,   
											  DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
DUT_WLAN_API int STDCALL DutIf_GetRfXTal(int *pExtension, int *pCal
#if defined (_MIMO_)
											   ,   
											  DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );

//DUT_WLAN_API int STDCALL DutIf_SetRfPowerRange(int pwrRange);

DUT_WLAN_API int STDCALL DutIf_GetRfPowerRange(int *pRangeIndex=NULL, int *pTopPwr=NULL, int *pBtmPwr=NULL 
#if defined (_MIMO_)					
												  ,
											  DWORD PathId=TXPATH_PATHA,  
											  DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											  ); 

DUT_WLAN_API int STDCALL DutIf_ClearRxPckts(DWORD DeviceId=MIMODEVICE0);
DUT_WLAN_API int STDCALL DutIf_GetRxPckts(	unsigned long *cntRx,
												unsigned long *cntOk,
												unsigned long *cntKo,  
												DWORD DeviceId=MIMODEVICE0); 

DUT_WLAN_API int STDCALL DutIf_SetRxAntenna(int antSelection,  
												DWORD DeviceId=MIMODEVICE0); 
DUT_WLAN_API int STDCALL DutIf_SetTxAntenna(int antSelection,  
												DWORD DeviceId=MIMODEVICE0); 

DUT_WLAN_API int STDCALL DutIf_GetRxAntenna(int *antSelection,  
												DWORD DeviceId=MIMODEVICE0); 
DUT_WLAN_API int STDCALL DutIf_GetTxAntenna(int *antSelection,  
												DWORD DeviceId=MIMODEVICE0); 

DUT_WLAN_API int STDCALL DutIf_SetTxDataRate(int rate,  
												DWORD DeviceId=MIMODEVICE0);
DUT_WLAN_API int STDCALL DutIf_GetTxDataRate(int *rate,  
												DWORD DeviceId=MIMODEVICE0);

DUT_WLAN_API int STDCALL DutIf_SetTxContMode(BOOL enable, BOOL CWmode, 
										DWORD payloadPattern=0,
										DWORD		CSmode=0,
										DWORD		ActSubCh=ACTSUBCH_40MHZ_BOTH, 
										DWORD 		DeviceId=MIMODEVICE0);  
DUT_WLAN_API int STDCALL DutIf_SetTxCarrierSuppression(BOOL enable, 
										DWORD 		DeviceId=MIMODEVICE0);  
DUT_WLAN_API int STDCALL DutIf_SetTxDutyCycle(BOOL enable, int dataRate=4,  
													int Percentage=50, 
													int payloadPattern=0, 
													int ShortPreamble=0							
#if defined (_MIMO_)
													,
													DWORD		ActSubCh=3, 
													DWORD		ShortGI=0, 
													DWORD		AdvCoding=0, 
													DWORD		TxBfOn=0,  
													DWORD		GFMode=0, 
													DWORD		STBC=0, 
													DWORD       DPDEnable=0,
													DWORD		PowerID=0,
													DWORD		SignalBw=-1,
													DWORD 		DeviceId=MIMODEVICE0
#endif // #if defined (_MIMO_)
													);   
/*
DUT_WLAN_API int STDCALL DutIf_SetTxDutyCycle2(BOOL enable, int dataRate=4,  
													int Percentage=50, 
													int PayloadLengthInKB=1, 
													int payloadPattern=0, 
													int ShortPreamble=0
#if defined (_MIMO_)
													,							
													DWORD		ActSubCh=3, 
													DWORD		ShortGI=0, 
													DWORD		AdvCoding=0, 
													DWORD		TxBfOn=0,  
													DWORD		GFMode=0, 
													DWORD		STBC=0, 
													DWORD       DPDEnable=0,
													DWORD		PowerID=0,
													DWORD 		DeviceId=MIMODEVICE0
#endif // #if defined (_MIMO_)
													);   
 
*/
//DUT_WLAN_API int STDCALL DutIf_SetPowerMode(int mode);		// 0 active, 1 standby mode, 3 power down mode
DUT_WLAN_API int STDCALL DutIf_SetPowerCycleMode(int cycle, 
													int sleepDur, 
													int awakeDur, 
													int stableXosc,  
													DWORD DeviceId=MIMODEVICE0);

DUT_WLAN_API int STDCALL DutIf_EnterSleepMode(void);

DUT_WLAN_API int STDCALL DutIf_SetBeaconInterval(DWORD interval, DWORD DeviceId=MIMODEVICE0); //in ms
DUT_WLAN_API int STDCALL DutIf_GetBeaconInterval(DWORD *interval, DWORD DeviceId=MIMODEVICE0);//in ms
DUT_WLAN_API int STDCALL DutIf_SetBeaconEnable(BOOL enable, DWORD DeviceId=MIMODEVICE0);
DUT_WLAN_API int STDCALL DutIf_GetBeaconEnable(BOOL *enable, DWORD DeviceId=MIMODEVICE0);

DUT_WLAN_API int STDCALL DutIf_GetHWVersion(WORD *socId, BYTE *socVersion, 
									   BYTE *RfId, BYTE *RfVersion, 
									   BYTE *BbId, BYTE *BbVersion,
									   DWORD DeviceId=MIMODEVICE0);
DUT_WLAN_API int STDCALL DutIf_GetORVersion(BYTE OR_ID, BYTE *OR_Major, BYTE *OR_Minor, BYTE *OR_Cust);
DUT_WLAN_API int STDCALL DutIf_GetFWVersion(BYTE *version);
DUT_WLAN_API int STDCALL DutIf_GetFWVerNumber(DWORD *FWVersion, DWORD * MFGFWVersion);
DUT_WLAN_API int STDCALL DutIf_GetReleaseNote(char *ReleaseNote);

DUT_WLAN_API int STDCALL DutIf_MacWakeupRfBBP(void);
DUT_WLAN_API int STDCALL DutIf_EnableBssidFilter(int mode=0, BYTE *Bssid=0, char *SSID=0,  
											   DWORD DeviceId=MIMODEVICE0);
DUT_WLAN_API int STDCALL DutIf_TxBrdCstPkts(DWORD dataRate,	DWORD pattern, 
				 DWORD length,		DWORD count,
					int ShortPreamble=1, char *Bssid=0
#if defined (_MIMO_)
					,							
					DWORD		ActSubCh=3, 
					DWORD		ShortGI=0, 
					DWORD		AdvCoding=0,  
					DWORD		TxBfOn=0,  
					DWORD		GFMode=0, 
					DWORD		STBC=0, 
					DWORD       DPDEnable=0,
					DWORD		PowerID=0,
					DWORD 		DeviceId=MIMODEVICE0
#endif // #if defined (_MIMO_)
					);
DUT_WLAN_API int STDCALL DutIf_AdjustPcktSifs(DWORD Enable, DWORD dataRate,	DWORD pattern, 
									   DWORD length, int ShortPreamble, char *Bssid							
#if defined (_MIMO_)
										,							
										DWORD		ActSubCh=3, 
										DWORD		ShortGI=0, 
										DWORD		AdvCoding=0,  
										DWORD		TxBfOn=0,  
										DWORD		GFMode=0, 
										DWORD		STBC=0, 
										DWORD       DPDEnable=0,
										DWORD		PowerID=0,
										DWORD		SignalBw=0,
#if defined (_W8787_) || 	defined (_W8782_)						
							DWORD	AdjustTxBurstGap=0, 
							DWORD	BurstSifsInUs=0, 
#endif //#if defined (_W8787_) || 	defined (_W8782_)						
										DWORD 		DeviceId=MIMODEVICE0
#endif // #if defined (_MIMO_)
					);

DUT_WLAN_API int STDCALL DutIf_XoscCal(WORD *cal, int TU, DWORD DeviceId=MIMODEVICE0);

DUT_WLAN_API int STDCALL DutIf_SoftReset(DWORD DeviceId=MIMODEVICE0);
DUT_WLAN_API int STDCALL DutIf_HwResetRf(DWORD DeviceId=MIMODEVICE0); 
DUT_WLAN_API int STDCALL DutIf_SetRssiNf(DWORD DeviceId=MIMODEVICE0);
DUT_WLAN_API int STDCALL DutIf_GetRssiNf(int stop, 
				  int *count, int *rssi, 
				  int *SNR, int *NoiseFloor, DWORD DeviceId=MIMODEVICE0);

DUT_WLAN_API int STDCALL DutIf_SetRfPowerUseCal(int *pPwr4Pa, int RateG=0
#if defined (_MIMO_)
												   , 
											  DWORD PathId=TXPATH_PATHA,  
											  DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											  );
DUT_WLAN_API int STDCALL DutIf_SetRfPowerUseCalExt( int *pPwr4Pa, int RateG=0,
										BYTE *cal_data_ext=NULL, int len=0
#if defined (_MIMO_)
												   , 
											  DWORD PathId=TXPATH_PATHA,  
											  DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											  );

#if defined(_FLASH_)
DUT_WLAN_API int STDCALL DutIf_EraseEEPROM(int section);
#endif // #if defined(_FLASH_)
DUT_WLAN_API int STDCALL DutIf_GetMACAddress(BYTE *ByteArray);
DUT_WLAN_API int STDCALL DutIf_SetMACAddress(BYTE *ByteArray);
DUT_WLAN_API int STDCALL DutIf_SetPIDVID(DWORD E2pOffset,  WORD PID, WORD VID);
DUT_WLAN_API int STDCALL DutIf_GetPIDVID(DWORD E2pOffset,  WORD *PID, WORD *VID);
DUT_WLAN_API int STDCALL DutIf_SetClassId(DWORD E2pOffset,  DWORD Class);
DUT_WLAN_API int STDCALL DutIf_GetClassId(DWORD E2pOffset,  DWORD *Class);

DUT_WLAN_API int STDCALL DutIf_CheckCalDataSpace(int* pAvalable); 
DUT_WLAN_API int STDCALL DutIf_GetHeaderPresent(int *pPresent, 
										   DWORD *pVersion=NULL, int *CsC=NULL); 

DUT_WLAN_API int STDCALL DutIf_SetTxIQCalMode(BOOL enable
#if defined (_MIMO_)
												   , 
												DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											  );  

DUT_WLAN_API int STDCALL DutIf_GetTxIQValue(int *pExtMode  , DWORD *pAmp, DWORD *pPhase
#if defined (_MIMO_)
												   , 
											  DWORD PathId=TXPATH_PATHA,  
											  DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
DUT_WLAN_API int STDCALL DutIf_SetTxIQValue(int ExtMode  , DWORD Amp, DWORD Phase
#if defined (_MIMO_)
												   , 
											  DWORD PathId=TXPATH_PATHA,  
											  DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );

DUT_WLAN_API int STDCALL DutIf_GetTxIQEstimatedValue(DWORD *pAmp, 
														DWORD *pPhase);

DUT_WLAN_API int STDCALL DutIf_PerformTxIQCal(	DWORD Avg,
													DWORD *pTxAmp, 
													DWORD *pTxPhase);

DUT_WLAN_API int STDCALL DutIf_PerformTxRxIQCal(	DWORD Avg,
													DWORD *pTxAmp, 
													DWORD *pTxPhase, 
													DWORD *pRxAmp, 
													DWORD *pRxPhase);

DUT_WLAN_API int STDCALL DutIf_GetRxIQ(int *pExtMode, 
											   DWORD *pAmp, 
											   DWORD *pPhase
#if defined (_MIMO_)
												   , 
											  DWORD PathId=TXPATH_PATHA,  
											  DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );

DUT_WLAN_API int STDCALL DutIf_SetRxIQ(int ExtMode, 
											   DWORD Amp, 
											   DWORD Phase
#if defined (_MIMO_)
												   , 
											  DWORD PathId=TXPATH_PATHA,  
											  DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );

DUT_WLAN_API int STDCALL DutIf_GetRxIQEstimated(DWORD *pAmp, 
														DWORD *pPhase
#if defined (_MIMO_)
												   , 
											  DWORD PathId=TXPATH_PATHA,  
											  DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );

DUT_WLAN_API int STDCALL DutIf_ArmReadUlongArray(DWORD armAddr,
                                               DWORD *pBuf,
                                               DWORD sizeBytes
#if defined (_MIMO_)
                                               ,
											   DWORD deviceId = MIMODEVICE0
#endif //#if defined (_MIMO_)
											  );

//#ifdef _ENGINEERING_
DUT_WLAN_API int STDCALL DutIf_RunBbpSqDiag(DWORD sizeBytes,
                                             DWORD *pSqBufAddr);

//#endif //#ifdef _ENGINEERING_

DUT_WLAN_API int STDCALL DutIf_SetCalDataRevC( 
									   int PurgeAfter, 
									   char *FlexFileNameNoE2prom=NULL); 

DUT_WLAN_API int STDCALL DutIf_GetCalDataRevC(int *CsC, int *CsCValue, 
										char *FlexFileNameNoE2prom=NULL);

DUT_WLAN_API int STDCALL DutIf_ForceE2PromType(DWORD IfType, 
											   DWORD AddrWidth, 
											   DWORD DeviceType);
DUT_WLAN_API int STDCALL DutIf_GetE2PromType(DWORD *pIfType, 
											 DWORD *pAddrWidth,	
											 DWORD *pDeviceType);

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
								DWORD AntDiversityInfo);

DUT_WLAN_API int STDCALL DutIf_FlexSpiDL(bool erase, char *fileName);
DUT_WLAN_API int STDCALL DutIf_FlexSpiCompare(char *fileName);
DUT_WLAN_API int STDCALL DutIf_CompareE2PData(DWORD  *RefArray,int SizeInByte);

 
DUT_WLAN_API int STDCALL DutIf_GetHeaderTbl( DWORD *HeaderTbl, DWORD *Version, BOOL *CsC);
DUT_WLAN_API int STDCALL DutIf_SetHeaderTbl( DWORD *HeaderTbl); 

DUT_WLAN_API int STDCALL DutIf_DumpE2PData( DWORD E2pOffset,  DWORD Len, 
					 DWORD *AddressArray, DWORD *DwordArray );
DUT_WLAN_API int STDCALL DutIf_WriteLumpedData( DWORD E2pOffset,  DWORD Len, DWORD *DwordArray );
 
DUT_WLAN_API int STDCALL DutIf_SetRfControlMode(DWORD ThermalOnly
#if defined (_MIMO_)
												   ,  
											  DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
												);
DUT_WLAN_API int STDCALL DutIf_GetRfControlMode(DWORD *pThermalOnly
#if defined (_MIMO_)
												   ,  
											  DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
												);
DUT_WLAN_API int STDCALL DutIf_WritePwrTableFile0( int DeviceId, int PathId, 
						int bandId, int tableId, //int *max, int *min, 
					  int *sizeOfTblInByte, void* tbl, char* GivenFileName=NULL); 
 
DUT_WLAN_API int STDCALL DutIf_SetRfPwrInitP(	BYTE InitP, 
						DWORD PathId=TXPATH_PATHA,	
						DWORD DeviceId=MIMODEVICE0);

DUT_WLAN_API int STDCALL DutIf_GetRfPwrVgaMax(	BYTE *pVgaMax, 
						DWORD PathId=TXPATH_PATHA,	
						DWORD DeviceId=MIMODEVICE0);

DUT_WLAN_API int STDCALL DutIf_DownLoadStaticSettingsToFw(DWORD BandId, DWORD SubbandId, 
											   DWORD PathId, 
 									BYTE RfLnaGrif,
 									BYTE RfLnaGrBb, 
									DWORD DeviceId=MIMODEVICE0);

DUT_WLAN_API int STDCALL DutIf_UpLoadStaticSettingsFromFw( DWORD BandId, DWORD SubbandId, 
											   DWORD PathId, 
 									BYTE *pRfLnaGrif,
 									BYTE *pRfLnaGrBb,
									DWORD DeviceId=MIMODEVICE0);	


//jsz DUT_WLAN_API int STDCALL DutIf_GetExtPaFileName(DWORD DeviceId, DWORD PathId, char *FileName); 
DUT_WLAN_API int STDCALL DutIf_GetFEMFileName(char *FileName); 

DUT_WLAN_API int STDCALL DutIf_GetBandSubBandTag(DWORD Band, DWORD SubBand, char *Tag);

DUT_WLAN_API int STDCALL DutIf_UpLoadPwrTable(	int BandId, int tableId,
												int *sizeOfTblInByte, 
												BYTE* tbl,		
												DWORD PathId=TXPATH_PATHA,
												DWORD DeviceId=MIMODEVICE0);

DUT_WLAN_API int STDCALL DutIf_DownLoadPwrTable(int BandId, int tableId, 
												int *sizeOfTblInByte, 
												BYTE* tbl, int UsePassDownData=1,		
												DWORD PathId=TXPATH_PATHA,
												DWORD DeviceId=MIMODEVICE0);
DUT_WLAN_API int STDCALL DutIf_LoadCustomizedSettings(void);
DUT_WLAN_API int STDCALL DutIf_SetCustomizedSettings(char *FileName=NULL);
DUT_WLAN_API int STDCALL DutIf_GetCustomizedSettings(int *Setting);
DUT_WLAN_API int STDCALL DutIf_ReloadCalData(void);

//jsz 
/*
DUT_WLAN_API int STDCALL DutIf_UpLoadSaveCalData(int *CsC, char* GivenFileName=NULL);
DUT_WLAN_API int STDCALL DutIf_LoadSaveCalData(char* GivenFileName=NULL);

DUT_WLAN_API int STDCALL DutIf_CloseLoopCal(BYTE targetInitP, int maxThreshold, int steps, int dataRate,
									   BYTE *initP, BYTE *threshHi, BYTE *threshLow);
*/ //jsz
DUT_WLAN_API int STDCALL DutIf_ThermalComp(	BOOL enabled, 
											DWORD tempRefAtCal,
											DWORD tempRefSlope,
											DWORD SlopeNumerator, 
											DWORD SlopeNumerator1, 
											DWORD SlopeDenominator,
											DWORD CalibrationIntervalInMS, 
											DWORD PTargetDeleta
#if defined (_MIMO_)
												   , 
											DWORD PathId=TXPATH_PATHA,  
											DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											  );

DUT_WLAN_API int STDCALL DutIf_GetThermalComp(	BOOL *pEnabled, 
												DWORD *pTempRefAtCal=NULL,
												DWORD *pTempRefAtSlope=NULL,
												DWORD *pSlopeNumerator=NULL, 
												DWORD *pSlopeNumerator1=NULL, 
												DWORD *pSlopeDenominator=NULL,
												DWORD *pCalibrationIntervalInMS=NULL, 
												DWORD *pPTargetDelta=NULL
#if defined (_MIMO_)
												   , 
												DWORD PathId=TXPATH_PATHA,  
												DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											  );

DUT_WLAN_API int STDCALL DutIf_ThermalTest(	BOOL enabled, DWORD PTarget, DWORD tempRef,
											DWORD SlopeNumerator, DWORD SlopeDenominator,
											DWORD CalibrationIntervalInMS,
											DWORD Temp,
											DWORD SlopeNumerator1, DWORD SlopeDenominator1);

DUT_WLAN_API int STDCALL DutIf_GetThermalTest(	BOOL *enabled, DWORD *PTarget, DWORD *tempRef,
												DWORD *SlopeNumerator, DWORD *SlopeDenominator,
												DWORD *CalibrationIntervalInMS,
												DWORD *Temp,
												DWORD *SlopeNumerator1, DWORD *SlopeDenominator1);

DUT_WLAN_API int STDCALL DutIf_GetThermalSensorReading(int *pReadBack);

DUT_WLAN_API int STDCALL DutIf_TxFastCal(BYTE *TxCalDataStruc=NULL, int len=0);
DUT_WLAN_API int STDCALL DutIf_TxFastVer(BYTE *TxVerDataStruc=NULL, int len=0);
DUT_WLAN_API int STDCALL DutIf_RxFastVer(BOOL enable, int channel, BYTE *Bssid=0, char *SSID=0);

#if defined (_SUPPORT_LDO_)
DUT_WLAN_API int STDCALL DutIf_SetLDOConfig(int);
DUT_WLAN_API int STDCALL DutIf_GetLDOConfig(int*);
#endif // #if defined (_SUPPORT_LDO_)

#if defined (_SUPPORT_PM_)
DUT_WLAN_API int STDCALL DutIf_WritePMReg(int addr, int value);
DUT_WLAN_API int STDCALL DutIf_ReadPMReg(int addr, int *value);
DUT_WLAN_API int STDCALL DutIf_SetPMRail(int railId, int mode, int volt, int powerDown);
#endif // #if defined (_SUPPORT_PM_)

DUT_WLAN_API int STDCALL DutIf_SetModuleTypeConf(int ModuleType); 
DUT_WLAN_API int STDCALL DutIf_GetModuleTypeConf(int *pModuleType);


DUT_WLAN_API int STDCALL DutIf_SetExtLnaNsettings(int mode);

DUT_WLAN_API int STDCALL DutIf_GetExtLnaNsettings(int *pMode);

DUT_WLAN_API int STDCALL DutIf_WriteCAUReg(WORD address, BYTE data
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
DUT_WLAN_API int STDCALL DutIf_ReadCAUReg(WORD address, BYTE *data
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											   );
DUT_WLAN_API int STDCALL DutIf_LoadRFUFW(DWORD deviceId, char *FileName=NULL);
DUT_WLAN_API int STDCALL DutIf_UpdateRFUFW(DWORD deviceId);

#if defined(_IF_USB_) 
DUT_WLAN_API int STDCALL DutIf_SetUsbAttributes(BYTE Attribute, BYTE MaxPower);
DUT_WLAN_API int STDCALL DutIf_GetUsbAttributes(BYTE *pAttribute, BYTE *pMaxPower);
#endif// #if defined(_IF_USB_) 

#if defined(_FLASH_)  
DUT_WLAN_API int STDCALL DutIf_CheckMacAddressSpace(int* pAvalable);
#endif // #if defined(_FLASH_)

//RSSI cal
#if defined(_W8787_)
DUT_WLAN_API int STDCALL DutIf_WlanRSSI_CalOneCh_CW(int RSSI_ch, char *RSSI_val, char *NF_val, int Band, DWORD DeviceId);
DUT_WLAN_API int STDCALL DutIf_WlanRSSI_CalOneCh_Pkt(int RSSI_ch, 
						 char *RSSI_val,
						 char *NF_val,
						 int Band, DWORD DeviceId);

DUT_WLAN_API int STDCALL DutIf_GetCalculatedRSSI(char *RSSI_calc
#if defined (_MIMO_)
												 , 
												 DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
												 );

DUT_WLAN_API int STDCALL DutIf_GetCurrentRSSIThermalOffset(char *pRssi_Thermal
#if defined (_MIMO_)
													 , DWORD DeviceId=MIMODEVICE0
#endif  //#if defined (_MIMO_)
															);

DUT_WLAN_API int STDCALL DutIf_GetCurrentRSSICalOffset(char *pRssi_OFDM, 
													   char *pRssi_OFDM_Bypass,
													   char *pRssi_CCK,
													   char *pRssi_CCK_Bypass
#if defined (_MIMO_)
													   ,
													   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
														);

#endif //#if defined(_W8787_)
//DPD API
#if defined(_W8782_)
DUT_WLAN_API int STDCALL DutIf_SetCompensation(DWORD PowerID, DWORD Memory2Row, DWORD *pCompensationTbl
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
);

DUT_WLAN_API int STDCALL DutIf_GetCompensation(DWORD PowerID, DWORD Memory2Row, DWORD *pCompensationTbl
#if defined (_MIMO_)
											   ,  
											   DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
);
#endif //#if defined(_W8782_)


#endif //_DUTAPI878XDLL_H_

