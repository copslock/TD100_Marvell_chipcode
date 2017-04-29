/** @file DutBtApiDll.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DUTBTAPI86XXDLL_UART_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DUTBTAPI86XXDLL_UART_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

#ifndef _LINUX_FEDORA_
#ifdef DUTBTAPI86XXDLL_EXPORTS
#define DUT_BT_API	extern "C" __declspec(dllexport)
#else
#ifdef _DEBUG
#define DUT_BT_API	extern "C"
#else
#define DUT_BT_API	extern "C" __declspec(dllimport)  
#endif //_DEBUG
#endif
#else
#define DUT_BT_API	extern "C"
#endif

#define DUT_FM_API DUT_BT_API

#include "DutBtApi.hc"
#include "DutApi.hc"
  
#include "../DutCommon/Clss_os.h"

DUT_BT_API int STDCALL DUT_BT_Version();

DUT_BT_API char* STDCALL DUT_BT_About();

DUT_BT_API int STDCALL DUT_BT_OpenDevice(void **theObj=NULL);

DUT_BT_API int STDCALL DUT_BT_CloseDevice();
DUT_BT_API int STDCALL DUT_BT_GetBDAddress(BYTE *BdAddress);
DUT_BT_API int STDCALL DUT_BT_ResetBt(void);
DUT_BT_API int STDCALL DUT_BT_EnableDeviceUnderTestMode(void);
DUT_BT_API int STDCALL DUT_BT_SetDeviceDiscoverable(void);
 

DUT_BT_API int STDCALL DUT_BT_WriteMem(DWORD address, DWORD data);

DUT_BT_API int STDCALL DUT_BT_ReadMem(DWORD address, DWORD *data);

DUT_BT_API int STDCALL DUT_BT_WriteBtuRegister(DWORD address, WORD data);

DUT_BT_API int STDCALL DUT_BT_ReadBtuRegister(DWORD address, WORD *data);

DUT_BT_API int STDCALL DUT_BT_WriteBrfRegister(DWORD address, WORD data);

DUT_BT_API int STDCALL DUT_BT_ReadBrfRegister(DWORD address, WORD *data);

#ifdef _W8689_
DUT_BT_API int STDCALL DUT_BT_WriteFmuRegister(DWORD address, WORD data); 
DUT_BT_API int STDCALL DUT_BT_ReadFmuRegister(DWORD address, WORD *data); 
#endif //#ifdef _W8689_

DUT_BT_API int STDCALL DUT_BT_GetHwVersion(int *pSocChipId,int *pSocChipVersion,
					int *pBtuChipId,int *pBtuChipVersion,
					int *pBrfChipId,int *pBrfChipVersion);
DUT_BT_API int STDCALL DUT_BT_GetFwVersion(int *pFwVersion, int *pMfgVersion, 
					char *pVersionString);
DUT_BT_API int STDCALL DUT_BT_GetCurrentORVersion(int *pSocVersion, 
												  int *pRfVersion);
DUT_BT_API int STDCALL DUT_BT_GetCurrentAppMode(int *pAppMode);
DUT_BT_API int STDCALL DUT_BT_GetChipClassModeCapacity(int *pMode);
#ifdef _W8780_
DUT_BT_API int STDCALL DUT_BT_GetTrSwAntenna(int *pAntSelection);
DUT_BT_API int STDCALL DUT_BT_SetTrSwAntenna(int AntSelection);
#endif // #ifdef _W8780_

DUT_BT_API int STDCALL DUT_BT_SetBtXtal(BYTE XtalValue);	
DUT_BT_API int STDCALL DUT_BT_GetBtXtal(BYTE *pXtalValue);

DUT_BT_API int STDCALL DUT_BT_SetBtChannel(int channelNum, bool BT2=0);
DUT_BT_API int STDCALL DUT_BT_SetBtChannelByFreq(int ChannelFreq, 
												 bool BT2=0);
DUT_BT_API int STDCALL DUT_BT_GetBtChannel(int *pChannelNum, 
										   int *pChannelFreq, bool BT2=0);
//DUT_BT_API int STDCALL DUT_BT_SetBtChBank(int BankNum, int channelNum=-1);
//DUT_BT_API int STDCALL DUT_BT_GetBtChBank(int BankNum, int *pChannelNum);


DUT_BT_API int STDCALL DUT_BT_SetBt2ChHopping(bool enable);
DUT_BT_API int STDCALL DUT_BT_GetBt2ChHopping(bool *pEnable);

// bake code
//DUT_BT_API int STDCALL DUT_BT_SetBtPseudoCode(DWORD option);
//DUT_BT_API int STDCALL DUT_BT_GetBtPseudoCode(DWORD *pOption);

//0:MRVL, 1:Class2, 2:Class1 3: Class2+XPA
DUT_BT_API int STDCALL DUT_BT_FE_BTOnly(bool bOn);				

DUT_BT_API int STDCALL DUT_BT_SetBtPwrControlClass(DWORD option);				
DUT_BT_API int STDCALL DUT_BT_GetBtPwrControlClass(DWORD *pOption);

DUT_BT_API int STDCALL DUT_BT_SetBtCloseLoop(bool CloseLoop);			
DUT_BT_API int STDCALL DUT_BT_GetBtCloseLoop(bool *pCloseLoop);		

DUT_BT_API int STDCALL DUT_BT_SetBtPwrPpaPgaInitValue(WORD PpaValue,		WORD PgaValue);		// 0 internal mode 1: external mode
DUT_BT_API int STDCALL DUT_BT_GetBtPwrPpaPgaInitValue(WORD *pPpaValue,	WORD *pPgaValue);
 
DUT_BT_API int STDCALL DUT_BT_StepPower(double step);

DUT_BT_API int STDCALL DUT_BT_SetBtPwrPpaMaxMin(bool enable,			
							   WORD MaxValue,		WORD MinValue);		// 0 internal mode 1: external mode
DUT_BT_API int STDCALL DUT_BT_GetBtPwrPpaMaxMin(bool *pEnable,		
							   WORD *pMaxValue,		WORD *pMinValue);		// 0 internal mode 1: external mode

DUT_BT_API int STDCALL DUT_BT_SetBtPwrPgaMaxMin(bool enable,			
							   WORD MaxValue,		WORD MinValue);		// 0 internal mode 1: external mode
DUT_BT_API int STDCALL DUT_BT_GetBtPwrPgaMaxMin(bool *pEnable,		
							   WORD *pMaxValue,		WORD *pMinValue);		// 0 internal mode 1: external mode


// 0 internal mode 1: external mode
DUT_BT_API int STDCALL DUT_BT_SetBtPwrPpaPgaExtMode(bool extMode,  
											 WORD PpaValue, 
											 WORD PgaValue);		
DUT_BT_API int STDCALL DUT_BT_GetBtPwrPpaPgaExtMode(bool *pExtMode,  
											 WORD *pPpaValue, 
											 WORD *pPgaValue);
 
DUT_BT_API int STDCALL DUT_BT_SetBtPwrThSetting(WORD ThHi, WORD ThLo);			 
DUT_BT_API int STDCALL DUT_BT_GetBtPwrThSetting(WORD *pThHi, WORD *pThLo);

// dataRate 0; 1M, 1:2M, 2: 3M
// payloadPattern 0: all 0, 1: all 1, 2: PN9, 3: 0xAA, 4: 0xF0
// payloadOnly: tx Payload only, no header (true, false)
DUT_BT_API int STDCALL DUT_BT_GetBtTxDetectedInChip(int *pTx); 

DUT_BT_API int STDCALL DUT_BT_TxBtCont( bool enable,			
									    int dataRate=0x11, 
										int payloadPattern=DEF_PAYLOADPATTERN,	
										bool payloadOnly=1);

// dataRate 0; 1M, 1:2M, 2: 3M
// payloadPattern 0: all 0, 1: all 1, 2: PN9, 3: 0xAA, 4: 0xF0
// payloadOnly: tx Payload only, no header (true, false)
// DutyWeight: On-Time percentage
DUT_BT_API int STDCALL DUT_BT_TxBtDutyCycle(bool enable,			
			int PacketType=0x11,  
			int payloadPattern=DEF_PAYLOADPATTERN,	
			int DutyWeight=50, 
			bool payloadOnly=0);

DUT_BT_API int STDCALL DUT_BT_TxBtMultiCastFrames(
			int FrameCount=100,	
			int PayloadLength=1024,  
			int PacketType=0x11, 
			int payloadPattern=DEF_PAYLOADPATTERN, 
			bool payloadOnly=0);

DUT_BT_API int STDCALL DUT_BT_TxBtDutyCycleHop(bool enable,			
										int PacketType=0x11, 
										int payloadPattern=DEF_PAYLOADPATTERN,	
										int PayloadLenghtInByte=PAYLOADLENGTH_MAXIMUN_POSSIBLE, 
										bool HopMode=0);

 
/*DUT_BT_API int STDCALL DUT_BT_TxBtRxTest(int FrameCount,	int PacketType, 
			 int payloadPattern=DEF_PAYLOADPATTERN,	
			 int PayloadLenghtInByte=PAYLOADLENGTH_MAXIMUN_POSSIBLE, 
			 BYTE TxBdAddress[SIZE_BDADDRESS_INBYTE]=NULL, int TxAmAddress=DEF_AMADDRESS, 
			 int FramSkip=0, 
			 int CorrWndw=DEF_CORRELATION_WINDOW, 
			 int CorrTh=DEF_CORRELATION_THRESHOLD, 
			 bool Whitening=0,BYTE PrbsPattern[LEN_PRBS_INBYTE]=NULL,
			 // report
				BYTE*		Report_Status_Complete0_Abort1=NULL, // status
				DWORD*		Report_TotalPcktCount=NULL,
				DWORD*		Report_HecMatchCount=NULL,
				DWORD*		Report_HecMatchCrcPckts=NULL,
				DWORD*		Report_NoRxCount=NULL,
				DWORD*		Report_CrcErrCount=NULL,
				DWORD*		Report_SuccessfulCorrelation=NULL,
				DWORD*		Report_TotalByteCount=NULL,
				DWORD*		Report_ErrorBits=NULL,
				DWORD*		Report_PER=NULL,
				DWORD*		Report_BER=NULL);
*/
// not supported by this chip DUT_BT_API int STDCALL DUT_BT_TxBtCw(DWORD enable);
DUT_BT_API int STDCALL DUT_BT_TxBtCst(bool enable, 
									  int FreqOffsetinKHz=DEF_IQSINEFREQ_INKHZ);

//DUT_BT_API int STDCALL DUT_BT_BrfInit(void);
DUT_BT_API int STDCALL DUT_BT_SetBtDisableBtuPwrCtl(bool DisableBtuPwrCtl);
DUT_BT_API int STDCALL DUT_BT_GetBtDisableBtuPwrCtl(bool *pDisableBtuPwrCtl);

DUT_BT_API int STDCALL DUT_BT_GetTxIQ(DWORD *pExtMode, 
									  BYTE *pAlpha, BYTE *pBeta);

DUT_BT_API int STDCALL DUT_BT_SetTxIQ(DWORD extMode, 
									  BYTE Alpha, BYTE Beta);

DUT_BT_API int STDCALL DUT_BT_GetBtThermalSensorReading(DWORD *pValue);

DUT_BT_API int STDCALL DUT_BT_SpiWriteDword(DWORD Address, DWORD data);
DUT_BT_API int STDCALL DUT_BT_SpiReadDword(DWORD Address, DWORD *data);
/*
DUT_BT_API int STDCALL DUT_BT_SpiWriteByte(DWORD Address, BYTE data);
DUT_BT_API int STDCALL DUT_BT_SpiReadByte(DWORD Address, BYTE *data);
*/

DUT_BT_API int STDCALL DUT_BT_ResetAccum(void);
DUT_BT_API int STDCALL DUT_BT_GetAccum(int *pAccum);

DUT_BT_API int STDCALL DUT_BT_SetBtXPaPdetPolority(bool Polority_Neg);
DUT_BT_API int STDCALL DUT_BT_GetBtXPaPdetPolority(bool *pPolority_Neg);

DUT_BT_API int STDCALL DUT_BT_SetBtPpaPgaPriority(bool PgaFirst);
DUT_BT_API int STDCALL DUT_BT_GetBtPpaPgaPriority(bool *pPgaFirst);

DUT_BT_API int STDCALL DUT_BT_SetBtPaThermScaler(int ThermScaler);
DUT_BT_API int STDCALL DUT_BT_GetBtPaThermScaler(int *pThermScaler);


DUT_BT_API int STDCALL DUT_BT_SetBtPaBias(WORD PaBias1, WORD PaBias2, bool PerformanceMode);
DUT_BT_API int STDCALL DUT_BT_GetBtPaBias(WORD *pPaBias1, WORD *pPaBias2, bool PerformanceMode);

DUT_BT_API int STDCALL DUT_BT_StartBtRxTest(
				int RxChannel, int FrameCount,	int PacketType, 
				int payloadPattern=DEF_PAYLOADPATTERN,	
				int PayloadLenghtInByte=PAYLOADLENGTH_MAXIMUN_POSSIBLE, 
				BYTE TxBdAddress[SIZE_BDADDRESS_INBYTE]=NULL);

DUT_BT_API int STDCALL DUT_BT_QueryBtRxTestResult( 
				double*	pBER=NULL, 
				DWORD*	pTotalBitsRx=NULL);

///////////////////////////////////
DUT_BT_API int STDCALL DUT_BT_SetExtendedPwrCtrlMode(int Mode);
DUT_BT_API int STDCALL DUT_BT_GetExtendedPwrCtrlMode(int *pMode);
DUT_BT_API int STDCALL DUT_BT_SetBbGainInitValue(WORD Gain);
DUT_BT_API int STDCALL DUT_BT_GetBbGainInitValue(WORD *pGain);
DUT_BT_API int STDCALL DUT_BT_SetBtPwrBbGainMaxMin(bool enable,	
							WORD MaxValue,		WORD MinValue);		
DUT_BT_API int STDCALL DUT_BT_GetBtPwrBbGainMaxMin(bool *pEnable, 
							WORD *pMaxValue,	WORD *pMinValue);	
DUT_BT_API int STDCALL DUT_BT_SetBtPwrBbGainExtMode(bool extMode, WORD BbGainValue);
DUT_BT_API int STDCALL DUT_BT_GetBtPwrBbGainExtMode(bool *pExtMode, WORD *pBbGainValue);
DUT_BT_API int STDCALL DUT_BT_SetPpaGainStepMode(int Mode);
DUT_BT_API int STDCALL DUT_BT_GetPpaGainStepMode(int *pMode);
DUT_BT_API int STDCALL DUT_BT_SetAutoZeroMode(int Mode, WORD Value);
DUT_BT_API int STDCALL DUT_BT_GetAutoZeroMode(int *pMode, WORD *pValue);
DUT_BT_API int STDCALL DUT_BT_SetTxPwrModeHighLow(int Mode);
DUT_BT_API int STDCALL DUT_BT_GetTxPwrModeHighLow(int *pMode);
/*
DUT_BT_API int STDCALL DUT_BT_SetEfuseData(int Block, int LenInWord, 
										   WORD *pData);
DUT_BT_API int STDCALL DUT_BT_GetEfuseData(int Block, int LenInWord, 
										   WORD *pData);
*/
DUT_BT_API int STDCALL DUT_BT_GetFwReleaseNote(char *pReleaseNote);
///////////////////////////////////
 
/*
DUT_BT_API int STDCALL DUT_BT_BtCloseLoopAdjustment(
					int CloseLoopOption,
					int PacketType, BYTE TargetInitBbGain,
					BYTE TargetInitPpa, BYTE TargetInitPga,  
					int TargetValueMargin_Looking4ThHi, 
					int *ValThHi,	int *ValThLo);
//DUT_BT_API int STDCALL DUT_BT_BtCloseLoopAdjustmentLc(
//					int CloseLoopOption,
//					int PacketType, 
//					BYTE TargetInitPpa, BYTE TargetInitPga,  
//					int TargetValueMargin_Looking4ThHi, 
//					int *ValThHi,	int *ValThLo);
*/
DUT_BT_API int STDCALL DUT_BT_DumpE2PData(DWORD StartAddress, DWORD LenInDw,
									DWORD *pAddressArray, 
									DWORD *pDataArray);
DUT_BT_API int STDCALL DUT_BT_WriteLumpedData(DWORD StartAddress, 
								   DWORD LenInDw, 
									DWORD *pDataArray);

DUT_BT_API int STDCALL DUT_BT_UpdateCalDataRevC_Bt(int PurgeAfter, 
												   char *FlexFileNameNoE2prom=NULL); 
DUT_BT_API int STDCALL DUT_BT_GetCalDataRevC_Bt(int *CsC, int *CsCValue, 
												char *FlexFileNameNoE2prom=NULL);
DUT_BT_API int STDCALL DUT_BT_GetCalDataRevC(int *CsC, int *CsCValue, 
												char *FlexFileNameNoE2prom=NULL);

DUT_BT_API int STDCALL DUT_BT_SetCalDataRevC_Wlan(int PurgeAfter, 
											 char *FlexFileNameNoE2prom=NULL);


DUT_BT_API int STDCALL DUT_BT_GetCalMainDataRevC(BYTE *StructRev, 	
						BYTE *PcbRev_DesignType,
						BYTE *PcbRev_MajorRev,
						BYTE *PcbRev_MinorRev,
						BYTE *PcbRev_Variation,

						bool pExtPaPolar_Neg_G[MAXNUM_MIMODEVICE], // 
						bool pExtPaPolar_Neg_A[MAXNUM_MIMODEVICE], // 
						bool pExtLna_G[MAXNUM_MIMODEVICE],	// 
						bool pExtLna_A[MAXNUM_MIMODEVICE],	// 
						BYTE pAntTx[MAXNUM_MIMODEVICE],		// 
						BYTE pAntRx[MAXNUM_MIMODEVICE],		// 
						BYTE *pAntBT,
						BYTE pFemConfId[MAXNUM_MIMODEVICE],  //
						BYTE pAnt_PartId[MAXNUM_MIMODEVICE][MAXNUM_RXPATH], //
						BYTE *pSpiSizeInfo,	BYTE *pSpiUnitInfo,		
						BYTE *pAssemblyHouse,	DWORD *pSN, 
 						BYTE *pCC,				 
						BYTE *pExtXtalSource,		
						BYTE *pExtSleepClk,	
						BYTE *pWlanWakeUp,	
						WORD *pSocId,			BYTE *pSocVer,			
						BYTE *pBbuId, 			BYTE *pBbuVers, 
						BYTE *pRfId, 			BYTE *pRfVer, 
						DWORD *pTestToolVer,	DWORD *pMfgTaskVersion,	
						DWORD *pDllVersion,
						BYTE *pAppModeFlag,	 
						BYTE *pSocOrRev, BYTE *pRfuOrRev,
						BYTE *pMimoTxCnt, 	BYTE *pMimoRxCnt, 
						BYTE pMimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]  
						);


DUT_BT_API int STDCALL DUT_BT_SetCalMainDataRevC(BYTE *StructRev=NULL, 	
						BYTE *PcbRev_DesignType=NULL,
						BYTE *PcbRev_MajorRev=NULL,
						BYTE *PcbRev_MinorRev=NULL,
						BYTE *PcbRev_Variation=NULL,

						bool pExtPaPolar_Neg_G[MAXNUM_MIMODEVICE]=NULL, //
						bool pExtPaPolar_Neg_A[MAXNUM_MIMODEVICE]=NULL, //
						bool pExtLna_G[MAXNUM_MIMODEVICE]=NULL,	//
						bool pExtLna_A[MAXNUM_MIMODEVICE]=NULL,	//
						BYTE pAntTx[MAXNUM_MIMODEVICE]=NULL,		//
						BYTE pAntRx[MAXNUM_MIMODEVICE]=NULL,		//
						BYTE *pAntBT=NULL,
						BYTE pFemConfId[MAXNUM_MIMODEVICE]=NULL,	//
 						BYTE pAnt_PartId[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]=NULL,		//
						BYTE *pSpiSizeInfo=NULL,		BYTE *pSpiUnitInfo=NULL,		
						BYTE *pAssemblyHouse=NULL,	DWORD *pSN=NULL, 
 						BYTE *pCC=NULL,				 
						BYTE *pExtXtalSource=NULL,		
						BYTE *pExtSleepClk=NULL,	
						BYTE *pWlanWakeUp=NULL,	
						WORD *pSocId=NULL,			BYTE *pSocVer=NULL,			
						BYTE *pBbuId=NULL, 			BYTE *pBbuVers=NULL, 
						BYTE *pRfId=NULL, 			BYTE *pRfVer=NULL, 
						DWORD *pTestToolVer=NULL,	DWORD *pMfgTaskVersion=NULL,	
						DWORD *pDllVersion=NULL,
						BYTE *pAppModeFlag=NULL,	 
						BYTE *pSocOrRev=NULL, BYTE *pRfuOrRev=NULL,
						BYTE *pMimoTxCnt=NULL, 	BYTE *pMimoRxCnt=NULL, 
						BYTE pMimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]=NULL
						);


DUT_BT_API int STDCALL DUT_BT_GetAnnexBtData(BOOL *pLimitPwrAsC2,						
					   BYTE *pGainPpa6dBStepMode,
					char *pPwrIndBm_C2,
						BYTE *pGainPpa_C2,
					   BYTE *pGainPga_C2,
					   BYTE *pGainBb_C2,
						char *pPwrIndBm_C1_5,
					   BYTE *pGainPpa_C1_5,
					   BYTE *pGainPga_C1_5,
					   BYTE *pGainBb_C1_5, 
					   int  *pPwrUpPwrLvl,
					   BYTE *pTempSlope_Gain,
					   BYTE *pTempRd_Ref,
					   BYTE *pFELoss,
					   BYTE *pXtal,
					   BYTE *pMaxPowerdBm_NoRssi,
					   BYTE *pRssiCorrection,
					   DWORD *pUartBaudRate,
					    BYTE *pLocalVersion,
						BYTE *pRemoteVersion,
					   BYTE *pDeepSleepState,
					   bool *pUseEncModeBrcst,
					   bool *pUseUnitKey,
					   bool *pOnChipProfileEnable, 
					   WORD *pDeviceJitter,
					   WORD *pDeviceDrift,
					   WORD *pEncKeyLmax,
					   WORD *pEncKeyLmin,
					   WORD *pMaxAcls,
					   WORD *pMaxScos,
					   WORD *pMaxAclPktLen,
					   WORD *pMaxAclPkts,
					   BYTE *pMaxScoPktLen,
					   WORD *pMaxScoPkts,
					   WORD *pConnRxWnd,
					   BYTE *pRssiGoldenRangeLow,
					   BYTE *pRssiGoldenRangeHigh,
					   BYTE *pCompatibleMode,
						WORD *pLMP_PwrReqMsgInterval,
						BYTE *pLMP_DecrPwrReqMsgIntervalRatio,
						BYTE *pNumOfPwrStep4Clss1p5,
						BYTE *pPwrStepSize4Clss1p5,
						BYTE *pPwrCtrlInClss2Device,
						WORD *pDynamicRssiLmpTineInterval,
						WORD *pDynamicRssiLmpTineIntervalThrhld,
					   WORD pLocalSupportedCmds[NUM_WORD_SUPPORTCOMMAND],
					   WORD pLocalSupportedFeatures[NUM_WORD_SUPPORTFEATURE],
					   BYTE	pBdAddress[SIZE_BDADDRESS_INBYTE], 
					   BYTE	pLocalDeviceName[MAXSTRLEN_LOCALDEVICENAME],
					   WORD *pMask,					   
					   WORD pBtAddrList[MAXPARTNER][SIZE_BDADDRESS_INBYTE],
					   WORD pBtKeyList[MAXPARTNER][SIZE_LINKKEY_INBYTE],
					   BYTE pSecurityKey[MAX_BYTE_SECURITYKEY]);

DUT_BT_API int STDCALL DUT_BT_SetAnnexBtData(BOOL LimitPwrAsC2,						
					   BYTE GainPpa6dBStepMode,
					char *pPwrIndBm_C2,
						BYTE *pGainPpa_C2,
					   BYTE *pGainPga_C2,
					   BYTE *pGainBb_C2,
						char *pPwrIndBm_C1_5,
					   BYTE *pGainPpa_C1_5,
					   BYTE *pGainPga_C1_5,
					   BYTE *pGainBb_C1_5, 
					   int  PwrUpPwrLvl,
					   BYTE TempSlope_Gain,
					   BYTE TempRd_Ref,
					   BYTE FELoss,
					   BYTE Xtal,
					   BYTE MaxPowerdBm_NoRssi,
					   BYTE RssiCorrection,
					   DWORD UartBaudRate,
					    BYTE LocalVersion,
						BYTE RemoteVersion,
					   BYTE DeepSleepState,
					   bool UseEncModeBrcst,
					   bool UseUnitKey,
					   bool OnChipProfileEnable, 
					   WORD DeviceJitter,
					   WORD DeviceDrift,
					   WORD EncKeyLmax,
					   WORD EncKeyLmin,
					   WORD MaxAcls,
					   WORD MaxScos,
					   WORD MaxAclPktLen,
					   WORD MaxAclPkts,
					   BYTE MaxScoPktLen,
					   WORD MaxScoPkts,
					   WORD ConnRxWnd,
					   BYTE RssiGoldenRangeLow,
					   BYTE RssiGoldenRangeHigh,
					   BYTE CompatibleMode,
						WORD LMP_PwrReqMsgInterval,
						BYTE LMP_DecrPwrReqMsgIntervalRatio,
						BYTE NumOfPwrStep4Clss1p5,
						BYTE PwrStepSize4Clss1p5,
						BYTE PwrCtrlInClss2Device,
						WORD DynamicRssiLmpTineInterval,
						WORD DynamicRssiLmpTineIntervalThrhld,
					   WORD LocalSupportedCmds[NUM_WORD_SUPPORTCOMMAND],
					   WORD LocalSupportedFeatures[NUM_WORD_SUPPORTFEATURE],
					   BYTE	pBdAddress[SIZE_BDADDRESS_INBYTE], 
					   BYTE	pLocalDeviceName[MAXSTRLEN_LOCALDEVICENAME],
					   WORD Mask,
					   WORD pBtAddrList[MAXPARTNER][SIZE_BDADDRESS_INBYTE],
					   WORD pBtKeyList[MAXPARTNER][SIZE_LINKKEY_INBYTE],
					   BYTE pSecurityKey[MAX_BYTE_SECURITYKEY]);
 
DUT_BT_API int STDCALL DUT_BT_GetAnnexType45Data(  
					   bool *pFlag_GpioCtrlXosc,
					   BYTE *pXoscSettling,
					   DWORD *pPinOut_DrvLow,
					   DWORD *pPinIn_PullDiable
						);

DUT_BT_API int STDCALL DUT_BT_SetAnnexType45Data( 
					   bool Flag_GpioCtrlXosc,
					   BYTE XoscSettling,
					   DWORD *pPinOut_DrvLow,
					   DWORD *pPinIn_PullDiable
					   );
DUT_BT_API int STDCALL DUT_BT_SetRfPowerUseCal(double PwrdBm, int ClassMode);

DUT_BT_API int STDCALL DUT_BT_LoadCustomizedSettings(void);
DUT_BT_API int STDCALL DUT_BT_GetBdAddressFromE2P(BYTE BdAddress[SIZE_BDADDRESS_INBYTE]);

DUT_BT_API int STDCALL DUT_BT_SupportNoE2PROM(void);

DUT_BT_API int STDCALL DUT_BT_GetHeaderPresent(int *pPresent, DWORD *pVersion=NULL, int *CsC=NULL);

/*
DUT_BT_API int STDCALL DUT_BT_AclLinkTest(int PacketType, int PacketCount, 
							int PayloadLengthInByte, int PayloadPattern, 
							bool MasterAsTx, bool DutAsMaster, 
							bool stress, 
						int *pRxPacket,
						int *pMissingPacket,
						double *pPer, double*pBer); 
*/

DUT_BT_API int STDCALL DUT_BT_ClearRxCnts(int LinkIndex);
DUT_BT_API int STDCALL DUT_BT_GetRxInfo(	int LinkIndex, 
				int	*pRssiQual,
				int	*pRssiValue,
				int	*pReceivedCrcCnt, 
				int *pHecMatchCnt,
				int *pCrcErrorCnt,
				int *pFECCorrectCnt);

DUT_BT_API int STDCALL DUT_BT_SetBcaMode(int Mode);
DUT_BT_API int STDCALL DUT_BT_GetBcaMode(int *pMode);

/*
DUT_BT_API int STDCALL DUT_BT_Test_Hold(int modeOnOff, 
							  WORD HoldMaxInterval, 
							  WORD HoldMinInterval,
							  bool PartnerAutoCtrl=0);

DUT_BT_API int STDCALL DUT_BT_Test_Park(int modeOnOff, 
							  WORD BeaconMaxInterval, 
							  WORD BeaconMinInterval,
							  bool partnerAutoCtrl=0);

DUT_BT_API int STDCALL DUT_BT_Test_Sniff(int modeOnOff, 
								WORD SniffMaxInterval,	
								WORD SniffMinInterval,
								WORD SniffAttempt,		
								WORD SniffTimeout,
								bool partnerAutoCtrl=0);
*/

DUT_BT_API int STDCALL DUT_BT_Hold(WORD hConnection, 
									WORD HoldMaxInterval, 
									WORD HoldMinInterval);

#if 1//def _ENGINEERING_ 

DUT_BT_API int STDCALL DUT_BT_GetAnnexType40Data( 
						WORD *pVid,	WORD *pPid,	
						BYTE *pChipId,	BYTE *pChipVersion,					   
						BYTE *pFlag_SettingPrev, 
						BYTE *pNumOfRailAvaiableInChip,
						BYTE pPowerDownMode[MAX_PM_RAIL_PW886],

						BYTE pNormalMode[MAX_PM_RAIL_PW886], 
						BYTE pSleepMode[MAX_PM_RAIL_PW886]);

DUT_BT_API int STDCALL DUT_BT_SetAnnexType40Data(
						WORD Vid,	WORD Pid,	
						BYTE ChipId,	BYTE ChipVersion,					   
						BYTE Flag_SettingPrev, 
						BYTE NumOfRailAvaiableInChip,
						BYTE pPowerDownMode[MAX_PM_RAIL_PW886],

						BYTE pNormalMode[MAX_PM_RAIL_PW886], 
						BYTE pSleepMode[MAX_PM_RAIL_PW886]);

#endif // #ifdef _ENGINEERING_ 

DUT_BT_API int STDCALL DUT_BT_SetMrvlTempCompModeHC(int		SwPwrCtrlMode,
													WORD	RefGain1,
 													WORD	RefGain2,
 													BYTE	RefTemp,
 													BYTE	TempGain);


DUT_BT_API int STDCALL DUT_BT_SetRfDynamicClkCtrl(int		DynClkTimer,
						int		MultiSlotProtect,
						int		AutoDetect,
						int		DynClkValue,
						int		DynClkEnable,
						int		DynClkStartTime,
						int		DynClkFinishTime,
						int		DynClkTimedEnable);

DUT_BT_API int STDCALL DUT_BT_SetBtSleepMode(int PowerMode);

DUT_BT_API int STDCALL DUT_BT_SetWakeUpMethod(int		HostWakeUpMethod,
					int		HostWakeUpGPIO,
					int		DeviceWakeUpMethod,
					int		DeviceWakeUpGPIO);


DUT_BT_API int STDCALL DUT_BT_SetMrvlHiuModuleConfig(int ModuleShutDownBringUp, 
						   int HostIfActive, 
						   int HostIfType);


DUT_BT_API int STDCALL DUT_BT_SetMrvlRadioPerformanceMode(	int		MrvlRxPowerMode,	 
 									int		PerfModeBdrRssiTh,		
									int		PerfModeEdrRssiTh,		 
									int		NormModeBdrRssiTh,	 
									int		NormModeEdrRssiTh,	 

									int		PerfModeBdrCrcTh,	 
									int		PerfModeEdrCrcTh,	 
									int		NormModeBdrCrcTh,		 
									int		NormModeEdrCrcTh,		 
									int		TxModeInTestMode,	 
									int		RxModeInTestMode,	 
									int		RxModeInOperationMode,	 
									int		RssiCrcSampleInterval, 
									int		LowPowerScanMode);

DUT_BT_API int STDCALL DUT_BT_SetMrvlArmDynamicClkCtrl(int		SetNcoMode,	 
 							int		DynamicClockChange 	 );


DUT_BT_API int STDCALL DUT_BT_SetBtXCompModeConfig(int		BTCoexistenceMode,	 
 									int		PriorityMode_Wlan,
									int PriorityMode_UpdateDefInPta,
									int PriorityDelay,
									int HighPriority_BitMap);
DUT_BT_API int STDCALL DUT_BT_SetBtMrvlModeConfig(int		BTCoexistenceMode,	 
 									int PriorityMode_Wlan,
									int PriorityMode_UpdateDefInPta,
									int PriorityDelay,
									int HighPriority_BitMap, 
									int MediumPriority_BitMap,
									int BtEndTimer,
									int BtFreqLowBand,
									int BtFreqHighBand );
DUT_BT_API int STDCALL DUT_BT_SetBtMrvlMode2Config(int		BTCoexistenceMode,	 
 									int	NonPeriodicMode_Enable, 
									int NonPeriodicMode_BtTimerTlead,
									int NonPeriodicMode_BtTimerTtail,
									int PeriodicMode_BtSyncPeriod,
									int PeriodicMode_BtSyncPulseWidth,
									int PeriodicMode_BtTsyncLead,
									int PeriodicMode_BtSyncMode);


#ifdef _W8689_ 
DUT_FM_API int STDCALL DUT_FM_FmReset(void);
DUT_FM_API int STDCALL DUT_FM_FmInitialization(int XtalFrequency);
DUT_FM_API int STDCALL DUT_FM_FmRxMode(int PowerMode);
DUT_FM_API int STDCALL DUT_FM_GetFmCurrentRssi(DWORD *pCurrentRssi);

DUT_FM_API int STDCALL DUT_FM_SetFmChannel(DWORD FmChInKHz);
DUT_FM_API int STDCALL DUT_FM_GetFmChannel(DWORD *pFmChInKHz);
DUT_FM_API int STDCALL DUT_FM_SetFmAfChannel(DWORD FmAfChInKHz);
DUT_FM_API int STDCALL DUT_FM_GetFmAfChannel(DWORD *pFmAfChInKHz);

DUT_FM_API int STDCALL DUT_FM_SetFmSearchDir(DWORD FmSearchDir_F0B1);
DUT_FM_API int STDCALL DUT_FM_GetFmSearchDir(DWORD *pFmSearchDir_F0B1);

DUT_FM_API int STDCALL DUT_FM_SetFmAutoSearchMode(DWORD FmAutoSearchMode);
DUT_FM_API int STDCALL DUT_FM_GetFmAutoSearchMode(DWORD *pFmAutoSearchMode);
DUT_FM_API int STDCALL DUT_FM_SetFmMonoStereoMode(DWORD FmMonoStereoMode);
DUT_FM_API int STDCALL DUT_FM_GetFmMonoStereoMode(DWORD *pFmMonoStereoMode);
DUT_FM_API int STDCALL DUT_FM_SetFmRssiThreshold(DWORD FmRssiThreshold);
DUT_FM_API int STDCALL DUT_FM_GetFmRssiThreshold(DWORD *pFmRssiThreshold);

DUT_FM_API int STDCALL DUT_FM_SetFmBand(DWORD FmBand);
DUT_FM_API int STDCALL DUT_FM_GetFmBand(DWORD *pFmBand);

DUT_FM_API int STDCALL DUT_FM_SetFmChStepSizeKHz(DWORD StepSizeKHz);
DUT_FM_API int STDCALL DUT_FM_GetFmChStepSizeKHz(DWORD *pStepSizeKHz);

DUT_FM_API int STDCALL DUT_FM_FmMoveChUp(void);
DUT_FM_API int STDCALL DUT_FM_FmMoveChDown(void);

DUT_FM_API int STDCALL DUT_FM_GetFmFwVersion(DWORD *pFwVersion);
DUT_FM_API int STDCALL DUT_FM_GetFmHwVersion(DWORD *pHwVersion);
DUT_FM_API int STDCALL DUT_FM_GetFmHwId(DWORD *pHwId);
DUT_FM_API int STDCALL DUT_FM_GetFmManId(DWORD *pVId);
#endif //#ifdef _W8689_
 
