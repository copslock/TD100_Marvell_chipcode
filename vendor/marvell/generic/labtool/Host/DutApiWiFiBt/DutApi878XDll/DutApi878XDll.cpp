/** @file DutApi878XDll.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

// DutApiCfDll.cpp : Defines the entry point for the DLL application.
//

#include "../../DutCommon/Clss_os.h"

#include DUTDLL_SHARED_H_PATH  // "../DutApi878XDll.h" //
//#include  DUTCLASS_BT_H_PATH		//"../DutBtApi878XDll/DutBtApiClss.h"
//#include  DUTCLASS_WLAN_H_PATH	//"../DutWlanApi878XDll/DutWlanApiClss.h"
//#include DUTCLASS_SHARED_H_PATH		//"../DutShareApi878XDll/DutSharedClss.h" 
#include DUTCLASS_SHARED_H_PATH		// "../DutShareApi878XDll/DutSharedClss.h"  //

#ifndef _LINUX_
#define WLAN_OBJ 	WlanObj->
#define BT_OBJ 		BtObj->
#define SHARED_OBJ 	SharedObj->

#ifdef DUTAPI8787DLL_EXPORTS
	DutWlanApiClss 	*WlanObj=NULL;
	DutBtApiClss 	*BtObj=NULL;
	DutSharedClss 	*SharedObj=NULL;
#ifdef STATIC_OBJ
    DutWlanApiClss 	WlanOnlyOne; 
	DutBtApiClss 	BtOnlyOne;
	DutSharedClss 	SharedOnlyOne;
#endif //#ifdef STATIC_OBJ

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 ) 
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
//		case DLL_THREAD_ATTACH:
#ifndef STATIC_OBJ
			SharedObj=new (DutSharedClss);  
#else	// #ifndef STATIC_OBJ
 			SharedObj = &SharedOnlyOne;  
#endif	// #ifndef STATIC_OBJ
			break;
//		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
#ifndef STATIC_OBJ
			delete SharedObj;
			SharedObj=NULL;
#endif	// #ifndef STATIC_OBJ
	 
			break;
    }
    return TRUE;
}

#else // #ifdef DUTAPI8787DLL_EXPORTS
	DutWlanApiClss 	*WlanObj=NULL;
#if defined(_BT_)
	DutBtApiClss 	*BtObj=NULL;
#endif	//#if defined(_BT_)
	DutSharedClss 	*SharedObj=new (DutSharedClss);
#endif // #ifdef DUTAPI8787DLL_EXPORTS

#else	// #ifndef _LINUX_ 
#define WLAN_OBJ 	 
#define BT_OBJ 		 
#define SHARED_OBJ 
void *WlanObj=NULL;
void *BtObj=NULL;
void *SharedObj=NULL; //new (DutSharedClss);
	 
#endif	// #ifndef _LINUX_ 

#if defined(_WLAN_)
#include "../DutWlanApi878XDll/DutWlanApi878XDll.cpp" 
#endif // #if defined(_WLAN_)

#if defined(_BT_)
#include "../DutBtApi878XDll/DutBtApi878XDll.cpp"  
#endif	//#if defined(_BT_)

#if defined(_FM_)
#include "../DutFmApi878XDll/DutFmApi878XDll.cpp"  
#endif	//#if defined(_FM_)

DUT_SHARED_API int STDCALL Dut_Shared_SupportNoE2PROM(void)
{
	return  SHARED_OBJ SupportNoE2PROM();
}

DUT_SHARED_API int STDCALL Dut_Shared_GetWriteTemplateFlag(void)
{
	return SHARED_OBJ GetWriteTemplateFlag();
}

DUT_SHARED_API void STDCALL Dut_Shared_SetWriteTemplateFlag(int Flag)
{	
 	SHARED_OBJ SetWriteTemplateFlag(Flag); 
	return;
}

DUT_SHARED_API int STDCALL Dut_Shared_ReadMainDataFromFile_RevD(char *FileName,
						BYTE *pStructRev,
						BYTE *pDesignType,
						BYTE *pLNAGain,
						BYTE *pAntDiversityInfo,
						BYTE *pFEVersion,

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
						BYTE *pCapabilities_2G_BT,
						BYTE *pCapabilities_5G_BT,

						BYTE  pBTFrontEnd2G[9],
						BYTE  pBTFrontEnd5G[9],	
					
						DWORD *pTestToolVer,	DWORD *pMfgTaskVersion,	
						DWORD *pDllVersion, 
						BYTE *pSocOrRev, BYTE *pRfuOrRev,
						BYTE *pMimoTxCnt, 	BYTE *pMimoRxCnt, 
						BYTE pMimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH])
{
	return SHARED_OBJ ReadMainDataFromFile_RevD(FileName,
						pStructRev,
						pDesignType,
						pLNAGain,
						pAntDiversityInfo,
						pFEVersion,

						//PA option
						pPaOption_ExtPaPolar_PathABC_G,
						pPaOption_ExtPaPolar_PathABC_A,

						pSpiSizeInfo,
					    pCC,
						
						pExtXtalSource,		
						pExtSleepClk,	
						pWlanWakeUp,
						
						pAntTx,		//
						pAntRx,		//
		
						//Capabilities
						pCapable_2G, //
						pCapable_5G, //
						pInternalBluetooth,	//
						pAntennaDiversity,
						pLNA_2G,	//
						pLNA_5G,
						pSameAnt_2G_BT,
						pSameAnt_5G_BT,
						
						//Concurrency
						pCapabilities_2G_BT,
						pCapabilities_5G_BT,

						pBTFrontEnd2G,
						pBTFrontEnd5G,	
					
						pTestToolVer,	pMfgTaskVersion,	
						pDllVersion, 
						pSocOrRev,		pRfuOrRev,
						pMimoTxCnt, 	pMimoRxCnt, 
						pMimoMap);
}

DUT_SHARED_API int STDCALL Dut_Shared_WriteMainDataToFile_RevD(char *FileName,
						BYTE pStructRev,
						BYTE pDesignType,
						BYTE pLNAGain,
						BYTE AntDiversityInfo,
						BYTE pFEVersion,

						bool pPaOption_ExtLnaFlag_PathABC_G,
						bool pPaOption_ExtLnaFlag_PathABC_A,
						//PA option
						bool pPaOption_ExtPaPolar_PathABC_G,
						bool pPaOption_ExtPaPolar_PathABC_A,

						BYTE pSpiSizeInfo,
					    BYTE pCC,
						
						BYTE pExtXtalSource,		
						BYTE pExtSleepClk,	
						BYTE pWlanWakeUp,
						
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
						BYTE Capabilities_2G_BT,
						BYTE Capabilities_5G_BT,

						BYTE  BTFrontEnd2G[9],
						BYTE  BTFrontEnd5G[9],	
					
						DWORD pTestToolVer,	DWORD pMfgTaskVersion,	
						DWORD pDllVersion, 
						BYTE  pSocOrRev,	BYTE pRfuOrRev,
						BYTE  pMimoTxCnt, 	BYTE pMimoRxCnt, 
						BYTE pMimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH])
{
	return SHARED_OBJ WriteMainDataToFile_RevD(FileName,
						pStructRev,
						pDesignType,
						pLNAGain,
						AntDiversityInfo,
						pFEVersion,
						//LNA option
						pPaOption_ExtLnaFlag_PathABC_G,
						pPaOption_ExtLnaFlag_PathABC_A,
						//PA option
						pPaOption_ExtPaPolar_PathABC_G,
						pPaOption_ExtPaPolar_PathABC_A,

						pSpiSizeInfo,
					    pCC,
						
						pExtXtalSource,		
						pExtSleepClk,	
						pWlanWakeUp,
						
						pAntTx,		//
						pAntRx,		//

						//Capabilities
						pCapable_2G, //
						pCapable_5G, //
						pInternalBluetooth,	//
						pAntennaDiversity,
						pLNA_2G,	//
						pLNA_5G,
						pSameAnt_2G_BT,
						pSameAnt_5G_BT,
						
						//Concurrency
						Capabilities_2G_BT,
						Capabilities_5G_BT,

						BTFrontEnd2G,
						BTFrontEnd5G,	
					
						pTestToolVer,	pMfgTaskVersion,	
						pDllVersion, 
						pSocOrRev, pRfuOrRev,
						pMimoTxCnt,pMimoRxCnt, 
						pMimoMap);
}


DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex27DataFromFile(char *FileName,BYTE *pLED)
{
	return SHARED_OBJ ReadAnnex27DataFromFile(FileName, pLED);
}

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex27DataToFile(char *FileName,BYTE *pLED)
{
	return SHARED_OBJ WriteAnnex27DataToFile(FileName, pLED);
}

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex28DataFromFile(char *FileName, 
								   BYTE *pUsbVendorString, 
								   BYTE *pUsbProductString)
{
	return SHARED_OBJ ReadAnnex28DataFromFile(FileName, 
								   pUsbVendorString, 
								   pUsbProductString);
}

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex28DataToFile(char *FileName, 
								   BYTE *pUsbVendorString, 
								   BYTE *pUsbProductString)
{
	return SHARED_OBJ WriteAnnex28DataToFile(FileName, 
								   pUsbVendorString, 
								   pUsbProductString);
}

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex31DataFromFile(char *FileName,
#if defined (_MIMO_)
						   int DeviceId, //int PathId, int BandId,
#endif	// #if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G]) 
{
	return SHARED_OBJ ReadAnnex31DataFromFile(FileName,
#if defined (_MIMO_)
						   DeviceId, //PathId, BandId,
#endif	// #if defined (_MIMO_)
						pRssi_Nf, 
						pRssi_cck, 
						pRssi_ofdm, 
						pRssi_mcs);
}

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex31DataToFile(char *FileName,
#if defined (_MIMO_)
						   int DeviceId, //int PathId, int BandId,
#endif	// #if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G])
{
	return SHARED_OBJ WriteAnnex31DataToFile(FileName,
#if defined (_MIMO_)
						   DeviceId, //PathId, BandId,
#endif	// #if defined (_MIMO_)
						pRssi_Nf, 
						pRssi_cck, 
						pRssi_ofdm, 
						pRssi_mcs);
}


DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex32DataFromFile(char *FileName,
#if defined (_MIMO_)
						   int DeviceId, //int PathId, int BandId,
#endif	// #if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A])
{
	return SHARED_OBJ ReadAnnex32DataFromFile(FileName,
#if defined (_MIMO_)
						   DeviceId, //PathId, BandId,
#endif	// #if defined (_MIMO_)
						pRssi_Nf, 
						pRssi_cck, 
						pRssi_ofdm, 
						pRssi_mcs);
}


DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex32DataToFile(char *FileName,
#if defined (_MIMO_)
						   int DeviceId, //int PathId, int BandId,
#endif	// #if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A])
{
	return SHARED_OBJ WriteAnnex32DataToFile(FileName,
#if defined (_MIMO_)
						   DeviceId, //PathId, BandId,
#endif	// #if defined (_MIMO_)
						pRssi_Nf, 
						pRssi_cck, 
						pRssi_ofdm, 
						pRssi_mcs);
}

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex40DataFromFile(char *FileName,
						WORD *pVid,		WORD *pPid,	
						BYTE *pChipId,	BYTE *pChipVersion,					   
						BYTE *pFlag_SettingPrev, 
						BYTE *pNumOfRailAvaiableInChip,
						BYTE pPowerDownMode[MAX_PM_RAIL_PW886],
						BYTE pNormalMode[MAX_PM_RAIL_PW886], 
						BYTE pSleepModeFlag[MAX_PM_RAIL_PW886],
						BYTE pSleepModeSetting[MAX_PM_RAIL_PW886],
						BYTE *pDpSlp)
{
	return SHARED_OBJ ReadAnnex40DataFromFile(FileName,
						pVid,		pPid,	
						pChipId,	pChipVersion,					   
						pFlag_SettingPrev, 
						pNumOfRailAvaiableInChip,
						pPowerDownMode,
						pNormalMode, 
						pSleepModeFlag,	pSleepModeSetting,
						pDpSlp);

}

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex40DataToFile(char *FileName,
						WORD *pVid,		WORD *pPid,	
						BYTE *pChipId,	BYTE *pChipVersion,					   
						BYTE *pFlag_SettingPrev, 
						BYTE *pNumOfRailAvaiableInChip,
						BYTE pPowerDownMode[MAX_PM_RAIL_PW886],
						BYTE pNormalMode[MAX_PM_RAIL_PW886], 
						BYTE pSleepModeFlag[MAX_PM_RAIL_PW886],
						BYTE pSleepModeSetting[MAX_PM_RAIL_PW886],
						BYTE *pDpSlp) 
{
	return SHARED_OBJ WriteAnnex40DataToFile(FileName,
						pVid,		pPid,	
						pChipId,	pChipVersion,					   
						pFlag_SettingPrev, 
						pNumOfRailAvaiableInChip,
						pPowerDownMode,
						pNormalMode, 
						pSleepModeFlag,	pSleepModeSetting,
						pDpSlp);

}

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex42DataFromFile(char *FileName,
					BYTE *pNoOfSubBand,
					BYTE *pBandList,
					BYTE *pSubBandList,
					BYTE *pTempRefMinus40CList,
					BYTE *pTempRef0CList,
					BYTE *pTempRef65CList,
					BYTE *pTempRef85CList,
					BYTE *pPPACoeffList,
					BYTE *pPACoeffList,
					BYTE *pNumeratorList,
					BYTE *pNumerator2List,
					BYTE *pDenomList,
					BYTE *pPTargetTempList)
{
	return SHARED_OBJ ReadAnnex42DataFromFile(	FileName,
					pNoOfSubBand,
					pBandList,
					pSubBandList,
					pTempRefMinus40CList,
					pTempRef0CList,
					pTempRef65CList,
					pTempRef85CList,
					pPPACoeffList,
					pPACoeffList,
					pNumeratorList,
					pNumerator2List,
					pDenomList,
					pPTargetTempList);
}

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex42DataToFile(char *FileName,
					BYTE *pNoOfSubBand,
					BYTE *pBandList,
					BYTE *pSubBandList,
					BYTE *pTempRefMinus40CList,
					BYTE *pTempRef0CList,
					BYTE *pTempRef65CList,
					BYTE *pTempRef85CList,
					BYTE *pPPACoeffList,
					BYTE *pPACoeffList,
					BYTE *pNumeratorList,
					BYTE *pNumerator2List,
					BYTE *pDenomList,
					BYTE *pPTargetTempList)
{
	return SHARED_OBJ WriteAnnex42DataToFile(	FileName, 
					pNoOfSubBand,
					pBandList,
					pSubBandList,
					pTempRefMinus40CList,
					pTempRef0CList,
					pTempRef65CList,
					pTempRef85CList,
					pPPACoeffList,
					pPACoeffList,
					pNumeratorList,
					pNumerator2List,
					pDenomList,
					pPTargetTempList);
}

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnexBtDataFromFile(char *FileName, 
						   BOOL *pLimitPwrAsC2, 
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
						WORD *pDynamicRssiLmpTimeInterval,
						WORD *pDynamicRssiLmpTimeIntervalThrhld,
					   WORD pLocalSupportedCmds[NUM_WORD_SUPPORTCOMMAND],
					   WORD pLocalSupportedFeatures[NUM_WORD_SUPPORTFEATURE],
					   BYTE	pBdAddress[SIZE_BDADDRESS_INBYTE], 
					   BYTE	pLocalDeviceName[MAXSTRLEN_LOCALDEVICENAME],
					   WORD *pMask,					   
					   WORD pBtAddrList[MAXPARTNER][SIZE_BDADDRESS_INBYTE],
					   WORD pBtKeyList[MAXPARTNER][SIZE_LINKKEY_INBYTE],
					   BYTE pSecurityKey[MAX_BYTE_SECURITYKEY]
						)
{
	return SHARED_OBJ ReadAnnexBtDataFromFile(	FileName, 
						pLimitPwrAsC2, 
						pGainPpa6dBStepMode,
						pPwrIndBm_C2,
						pGainPpa_C2,
						pGainPga_C2,
						pGainBb_C2,
						pPwrIndBm_C1_5,
						pGainPpa_C1_5,
						pGainPga_C1_5,
						pGainBb_C1_5, 
						pPwrUpPwrLvl,
						pTempSlope_Gain,
						pTempRd_Ref,
						pFELoss,
						pXtal,

						pMaxPowerdBm_NoRssi,
						pRssiCorrection,
						pUartBaudRate,
						pLocalVersion,
						pRemoteVersion,
						pDeepSleepState,
						pUseEncModeBrcst,
						pUseUnitKey,
						pOnChipProfileEnable, 
						pDeviceJitter,
						pDeviceDrift,
						pEncKeyLmax,
						pEncKeyLmin,
						pMaxAcls,
						pMaxScos,
						pMaxAclPktLen,
						pMaxAclPkts,
						pMaxScoPktLen,
						pMaxScoPkts,
						pConnRxWnd,
						pRssiGoldenRangeLow,
						pRssiGoldenRangeHigh,
						pCompatibleMode,
						pLMP_PwrReqMsgInterval,
						pLMP_DecrPwrReqMsgIntervalRatio,
						pNumOfPwrStep4Clss1p5,
						pPwrStepSize4Clss1p5,
						pPwrCtrlInClss2Device,
						pDynamicRssiLmpTimeInterval,
						pDynamicRssiLmpTimeIntervalThrhld,
						pLocalSupportedCmds,
						pLocalSupportedFeatures,
						pBdAddress, 
						pLocalDeviceName,
						pMask,					   
						pBtAddrList,
						pBtKeyList,
						pSecurityKey
						);
}

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnexBtDataToFile(char *FileName, 
						   BOOL *pLimitPwrAsC2, 
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
						WORD *pDynamicRssiLmpTimeInterval,
						WORD *pDynamicRssiLmpTimeIntervalThrhld,
					   WORD pLocalSupportedCmds[NUM_WORD_SUPPORTCOMMAND],
					   WORD pLocalSupportedFeatures[NUM_WORD_SUPPORTFEATURE],
					   BYTE	pBdAddress[SIZE_BDADDRESS_INBYTE], 
					   BYTE	pLocalDeviceName[MAXSTRLEN_LOCALDEVICENAME],
					   WORD *pMask,					   
					   WORD pBtAddrList[MAXPARTNER][SIZE_BDADDRESS_INBYTE],
					   WORD pBtKeyList[MAXPARTNER][SIZE_LINKKEY_INBYTE],
					   BYTE pSecurityKey[MAX_BYTE_SECURITYKEY]
						)
{
	return SHARED_OBJ WriteAnnexBtDataToFile(	FileName, 
						pLimitPwrAsC2, 
						pGainPpa6dBStepMode,
						pPwrIndBm_C2,
						pGainPpa_C2,
						pGainPga_C2,
						pGainBb_C2,
						pPwrIndBm_C1_5,
						pGainPpa_C1_5,
						pGainPga_C1_5,
						pGainBb_C1_5, 
						pPwrUpPwrLvl,
						pTempSlope_Gain,
						pTempRd_Ref,
						pFELoss,
						pXtal,

						pMaxPowerdBm_NoRssi,
						pRssiCorrection,
						pUartBaudRate,
						pLocalVersion,
						pRemoteVersion,
						pDeepSleepState,
						pUseEncModeBrcst,
						pUseUnitKey,
						pOnChipProfileEnable, 
						pDeviceJitter,
						pDeviceDrift,
						pEncKeyLmax,
						pEncKeyLmin,
						pMaxAcls,
						pMaxScos,
						pMaxAclPktLen,
						pMaxAclPkts,
						pMaxScoPktLen,
						pMaxScoPkts,
						pConnRxWnd,
						pRssiGoldenRangeLow,
						pRssiGoldenRangeHigh,
						pCompatibleMode,
						pLMP_PwrReqMsgInterval,
						pLMP_DecrPwrReqMsgIntervalRatio,
						pNumOfPwrStep4Clss1p5,
						pPwrStepSize4Clss1p5,
						pPwrCtrlInClss2Device,
						pDynamicRssiLmpTimeInterval,
						pDynamicRssiLmpTimeIntervalThrhld,
						pLocalSupportedCmds,
						pLocalSupportedFeatures,
						pBdAddress, 
						pLocalDeviceName,
						pMask,					   
						pBtAddrList,
						pBtKeyList,
						pSecurityKey
						);
}

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex45DataFromFile(char *FileName,  
											bool *pFlag_GpioCtrlXosc,
											BYTE *pXoscSettling,
											DWORD *pPinOut_DrvLow,
											DWORD *pPinIn_PullDiable)
{
	return SHARED_OBJ ReadAnnex45DataFromFile(FileName,  
						pFlag_GpioCtrlXosc,
						pXoscSettling,
						pPinOut_DrvLow,
						pPinIn_PullDiable);
}

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex45DataToFile(char *FileName,  
						   bool *pFlag_GpioCtrlXosc,
						BYTE *pXoscSettling,
						DWORD *pPinOut_DrvLow,
						DWORD *pPinIn_PullDiable)
{
	return SHARED_OBJ WriteAnnex45DataToFile(	FileName,  
						pFlag_GpioCtrlXosc,
						pXoscSettling,
						pPinOut_DrvLow,
						pPinIn_PullDiable);
}

#if defined (_MIMO_)
DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex47DataFromFile(char *FileName,
 						   int DeviceId, 
						   int PathId, //int BandId,
 						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G]) 
{
	return SHARED_OBJ ReadAnnex47DataFromFile(FileName,
 							DeviceId, 
							PathId,  
 							pRssi_Nf, 
							pRssi_cck, 
							pRssi_ofdm, 
							pRssi_mcs);
}
DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex47DataToFile(char *FileName,
 						   int DeviceId, 
						   int PathId, //int BandId,
 						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G])
{
	return SHARED_OBJ WriteAnnex47DataToFile(FileName,
 							DeviceId, 
							PathId,  
 							pRssi_Nf, 
							pRssi_cck, 
							pRssi_ofdm, 
							pRssi_mcs);
}


DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex48DataFromFile(char *FileName,
						   int DeviceId, 
						   int PathId,  //int BandId,
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A])
{
	return SHARED_OBJ ReadAnnex48DataFromFile(FileName,
 							DeviceId, 
							PathId,  
 							pRssi_Nf, 
							pRssi_cck, 
							pRssi_ofdm, 
							pRssi_mcs);
}						

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex48DataToFile(char *FileName,
 						   int DeviceId, 
						   int PathId, //int BandId,
 						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A])
{
	return SHARED_OBJ WriteAnnex48DataToFile(FileName,
 							DeviceId, 
							PathId,  
 							pRssi_Nf, 
							pRssi_cck, 
							pRssi_ofdm, 
							pRssi_mcs);
}						
#endif //#if defined (_MIMO_)

//Rev D
//Annex 42
DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex42DataToFile_VD(char *FileName,
					BYTE *pNoOfEntry,
					THERMALCALPARMS_VD *ThermalCalParam)
{

	return SHARED_OBJ WriteAnnex42DataToFile_VD(FileName,
					pNoOfEntry,
					ThermalCalParam);

}

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex42DataFromFile_VD(char *FileName,
					BYTE *pNoOfEntry,
					THERMALCALPARMS_VD *ThermalCalParam)
{
	return SHARED_OBJ ReadAnnex42DataFromFile_VD(FileName,
					pNoOfEntry,
					ThermalCalParam);
}

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex49DataToFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					BYTE *pNumOfTxPowerLevelStore,
					BYTE *pNumOfSavedCh,
					bool *pCalMethod,
					bool *pCloseTXPWDetection,
					bool *pExtPAPolarize,
					BYTE *pCalXtal,
					BYTE *pPdetHiLoGap,
					BYTE *pMCS20MPdetOffset,
					BYTE *pMCS20MInitPOffset,
					BYTE *pMCS40MPdetOffset,
					BYTE *pMCS40MInitPOffset,
					int  pChNumList[MAXSAVEDCH_G_REVD],
					char pCorrInitTrgt_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
					char pCorrPDetTh_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD],

					char pCorrInitTrgt_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
					char pCorrPDetTh_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD],
					BYTE* pwrTbl)
{

	return SHARED_OBJ WriteAnnex49DataToFile(FileName,
#if defined (_MIMO_)
					DeviceId,	 PathId,		
#endif // #if defined (_MIMO_)
					pNumOfTxPowerLevelStore,
					pNumOfSavedCh,
					pCalMethod,
					pCloseTXPWDetection,
					pExtPAPolarize,
					pCalXtal,
					pPdetHiLoGap,
					pMCS20MPdetOffset,
					pMCS20MInitPOffset,
					pMCS40MPdetOffset,
					pMCS40MInitPOffset,
					pChNumList,
					pCorrInitTrgt_B, 
					pCorrPDetTh_B,
					pCorrInitTrgt_G, 
					pCorrPDetTh_G,
					pwrTbl);
return 0;
}

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex49DataFromFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					BYTE *pNumOfTxPowerLevelStore,
					BYTE *pNumOfSavedCh,
					bool *pCalMethod,
					bool *pCloseTXPWDetection,
					bool *pExtPAPolarize,
					BYTE *pCalXtal,
					BYTE *pPdetHiLoGap,
					BYTE *pMCS20MPdetOffset,
					BYTE *pMCS20MInitPOffset,
					BYTE *pMCS40MPdetOffset,
					BYTE *pMCS40MInitPOffset,
					int  pChNumList[MAXSAVEDCH_G_REVD],
					char pCorrInitTrgt_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
					char pCorrPDetTh_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD],
					char pCorrInitTrgt_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
					char pCorrPDetTh_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD])
{
					return SHARED_OBJ ReadAnnex49DataFromFile(FileName,
#if defined (_MIMO_)
					DeviceId,		PathId,		
#endif // #if defined (_MIMO_)
					pNumOfTxPowerLevelStore,
					pNumOfSavedCh,
					pCalMethod,
					pCloseTXPWDetection,
					pExtPAPolarize,
					pCalXtal,
					pPdetHiLoGap,
					pMCS20MPdetOffset,
					pMCS20MInitPOffset,
					pMCS40MPdetOffset,
					pMCS40MInitPOffset,
					pChNumList,
					pCorrInitTrgt_B, 
					pCorrPDetTh_B,
					pCorrInitTrgt_G, 
					pCorrPDetTh_G
					);


}

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex50DataToFile(char * FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
	int		SubBandId, 
	BYTE	*NumOfSavedChInSubBand,
	BYTE	*NumOfTxPowerLevelStore,
	BYTE	*NumOfSavedCh,
	bool	*CalMethod,
	bool 	*CloseTXPWDetection,
	bool    *ExtPAPolarize,
	BYTE	*CalXtal,	
	BYTE	*PdetHiLoGap,
	BYTE	*MCS20MPdetOffset,
	BYTE	*MCS20MInitPOffset,
	BYTE	*MCS40MPdetOffset,
	BYTE	*MCS40MInitPOffset,
	BYTE	SavedChannelIndex[MAXSAVEDCH_G_REVD],
	WORD	*FreqBound_Lo,
	WORD	*FreqBound_Hi,
	char	CorrInitTrgt[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
	char	CorrPDetTh[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD])
{
	return SHARED_OBJ WriteAnnex50DataToFile(FileName,
#if defined (_MIMO_)
			DeviceId,		PathId,		
#endif // #if defined (_MIMO_)
	SubBandId, 
	NumOfSavedChInSubBand,
	NumOfTxPowerLevelStore,
	NumOfSavedCh,
	CalMethod,
	CloseTXPWDetection,
	ExtPAPolarize,
	CalXtal,	
	PdetHiLoGap,
	MCS20MPdetOffset,
	MCS20MInitPOffset,
	MCS40MPdetOffset,
	MCS40MInitPOffset,
	SavedChannelIndex,
	FreqBound_Lo,
	FreqBound_Hi,
	CorrInitTrgt, 
	CorrPDetTh);
}

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex50DataFromFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
	int		SubBandId, 
	BYTE	*NumOfSavedChInSubBand,
	BYTE	*NumOfTxPowerLevelStore,
	BYTE	*NumOfSavedCh,
	bool	*CalMethod,
	bool 	*CloseTXPWDetection,
	bool    *ExtPAPolarize,
	BYTE	*CalXtal,	
	BYTE	*PdetHiLoGap,
	BYTE	*MCS20MPdetOffset,
	BYTE	*MCS20MInitPOffset,
	BYTE	*MCS40MPdetOffset,
	BYTE	*MCS40MInitPOffset,
	BYTE	SavedChannelIndex[MAXSAVEDCH_G_REVD],
	WORD	*FreqBound_Lo,
	WORD	*FreqBound_Hi,
	char	CorrInitTrgt[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
	char	CorrPDetTh[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD])	
{
	return SHARED_OBJ ReadAnnex50DataFromFile( FileName,
#if defined (_MIMO_)
			DeviceId,		PathId,		
#endif // #if defined (_MIMO_)
	SubBandId, 
	NumOfSavedChInSubBand,
	NumOfTxPowerLevelStore,
	NumOfSavedCh,
	CalMethod,
	CloseTXPWDetection,
	ExtPAPolarize,
	CalXtal,	
	PdetHiLoGap,
	MCS20MPdetOffset,
	MCS20MInitPOffset,
	MCS40MPdetOffset,
	MCS40MInitPOffset,
	SavedChannelIndex,
	FreqBound_Lo,
	FreqBound_Hi,
	CorrInitTrgt, 
	CorrPDetTh);
}

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex52DataFromFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					bool *Cal2G,
					bool *Cal5G,
					BYTE *LoLeakageCal2G_I,
					BYTE *LoLeakageCal2G_Q,
					char LoLeakageCal5G_I[NUM_SUBBAND_A_REVD],
					char LoLeakageCal5G_Q[NUM_SUBBAND_A_REVD])
{
	return SHARED_OBJ ReadAnnex52DataFromFile(FileName,
#if defined (_MIMO_)
					DeviceId,		PathId,		
#endif // #if defined (_MIMO_)
					Cal2G,
					Cal5G,
					LoLeakageCal2G_I,
					LoLeakageCal2G_Q,
					LoLeakageCal5G_I,
					LoLeakageCal5G_Q);
					
}

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex52DataToFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					bool Cal2G,
					bool Cal5G,
					BYTE LoLeakageCal2G_I,
					BYTE LoLeakageCal2G_Q,
					char LoLeakageCal5G_I[NUM_SUBBAND_A],
					char LoLeakageCal5G_Q[NUM_SUBBAND_A])
{
	return SHARED_OBJ WriteAnnex52DataToFile(FileName,
#if defined (_MIMO_)
					DeviceId,		PathId,
#endif // #if defined (_MIMO_)
					Cal2G,
					Cal5G,
					LoLeakageCal2G_I,
					LoLeakageCal2G_Q,
					LoLeakageCal5G_I,
					LoLeakageCal5G_Q);
}

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex53DataToFile(char *FileName,
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
						bool Cal2G,
						bool Cal5G,
						char TxIQmismatch_Amp_2G,	
						char TxIQmismatch_Phase_2G,
						char TxIQmismatch_Amp_5G[NUM_SUBBAND_A],	
						char TxIQmismatch_Phase_5G[NUM_SUBBAND_A])
{
	return SHARED_OBJ WriteAnnex53DataToFile(FileName,
#if defined (_MIMO_)
					   DeviceId,		PathId,   
#endif //#if defined (_MIMO_)
						Cal2G,
						Cal5G,
						TxIQmismatch_Amp_2G,TxIQmismatch_Phase_2G,
						TxIQmismatch_Amp_5G,TxIQmismatch_Phase_5G);
}

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex53DataFromFile(char *FileName,
#if defined (_MIMO_)
					   int DeviceId,		int PathId, 
#endif //#if defined (_MIMO_)
						bool *Cal2G,
						bool *Cal5G,
						char *TxIQmismatch_Amp_2G,	char *TxIQmismatch_Phase_2G,
						char TxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	char TxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD])

{
	return SHARED_OBJ ReadAnnex53DataFromFile(FileName,
#if defined (_MIMO_)
					   DeviceId,		PathId,  
#endif //#if defined (_MIMO_)
						Cal2G,
						Cal5G,
						TxIQmismatch_Amp_2G,TxIQmismatch_Phase_2G,
						TxIQmismatch_Amp_5G,TxIQmismatch_Phase_5G);
}

//Annex54
DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex54DataToFile(char *FileName,
#if defined (_MIMO_)
					   int DeviceId,		int PathId, 
#endif //#if defined (_MIMO_)
						bool Cal2G,
						bool Cal5G,
						char RxIQmismatch_Amp_2G,	
						char RxIQmismatch_Phase_2G,
						char RxIQmismatch_Amp_5G[NUM_SUBBAND_A],	
						char RxIQmismatch_Phase_5G[NUM_SUBBAND_A])
{
	return SHARED_OBJ WriteAnnex54DataToFile(FileName,
#if defined (_MIMO_)
					   DeviceId,		PathId, 
#endif //#if defined (_MIMO_)
						Cal2G,
						Cal5G,
						RxIQmismatch_Amp_2G,RxIQmismatch_Phase_2G,
						RxIQmismatch_Amp_5G,RxIQmismatch_Phase_5G);
}

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex54DataFromFile(char *FileName,
#if defined (_MIMO_)
					   int DeviceId,		int PathId, 
#endif //#if defined (_MIMO_)
						bool *Cal2G,
						bool *Cal5G,
						char *RxIQmismatch_Amp_2G,	
						char *RxIQmismatch_Phase_2G,
						char RxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	
						char RxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD])

{
	return SHARED_OBJ ReadAnnex54DataFromFile(FileName,
#if defined (_MIMO_)
					   DeviceId,		PathId, 
#endif //#if defined (_MIMO_)
						Cal2G,
						Cal5G,
						RxIQmismatch_Amp_2G,RxIQmismatch_Phase_2G,
						RxIQmismatch_Amp_5G,RxIQmismatch_Phase_5G);
}

//Annex 55
DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex55DataFromFile(char *FileName,
 					BYTE *Version,
					BYTE *RFXtal,
					BYTE *InitPwr,
					BYTE *FELoss,

					bool *ForceClass2Op,
					bool *DisablePwrControl,
					bool *MiscFlag,
					bool *UsedInternalSleepClock,
					BYTE *Rssi_Golden_Lo,
					BYTE *Rssi_Golden_Hi,
					DWORD *BTBAUDRate,
					BYTE BDAddr[6],
					BYTE *Encr_Key_Len_Max,
					BYTE *Encr_Key_Len_Min)
{
		return SHARED_OBJ ReadAnnex55DataFromFile(FileName,
					Version,
					RFXtal,
					InitPwr,
					FELoss,

					ForceClass2Op,
					DisablePwrControl,
					MiscFlag,
					UsedInternalSleepClock,
					Rssi_Golden_Lo,
					Rssi_Golden_Hi,
					BTBAUDRate,
					BDAddr,
					Encr_Key_Len_Max,
					Encr_Key_Len_Min);
}

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex55DataToFile(char *FileName,
					BYTE Version,
					BYTE RFXtal,
					BYTE InitPwr,
					BYTE FELoss,

					bool ForceClass2Op,
					bool DisablePwrControl,
					bool MiscFlag,
					bool UsedInternalSleepClock,
					BYTE Rssi_Golden_Lo,
					BYTE Rssi_Golden_Hi,
					DWORD BTBAUDRate,
					BYTE BDAddr[6],
					BYTE Encr_Key_Len_Max,
					BYTE Encr_Key_Len_Min)
{
		return SHARED_OBJ WriteAnnex55DataToFile(FileName,
					Version,
					RFXtal,
					InitPwr,
					FELoss,

					ForceClass2Op,
					DisablePwrControl,
					MiscFlag,
					UsedInternalSleepClock,
					Rssi_Golden_Lo,
					Rssi_Golden_Hi,
					BTBAUDRate,
					BDAddr,
					Encr_Key_Len_Max,
					Encr_Key_Len_Min);
}

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex56DataFromFile(char *FileName,
 					BYTE Lincense_Key[16],
	                WORD *Link_Key_Mask,
	                BYTE Key_Value[3][22])
{
		return SHARED_OBJ ReadAnnex56DataFromFile(FileName,
  					Lincense_Key,
					Link_Key_Mask,
					Key_Value);
}

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex56DataToFile(char *FileName,
 					BYTE Lincense_Key[16],
	                WORD Link_Key_Mask,
	                BYTE Key_Value[3][22])
{
	return SHARED_OBJ WriteAnnex56DataToFile(FileName,
 					Lincense_Key,
					Link_Key_Mask,
					Key_Value);
}

#if defined (_WLAN_)
DUT_SHARED_API int STDCALL Dut_Shared_GetPwrTbleFileName(
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
						int BandId, int SubBandId,   
						int Upload, char* pGivenFileName)
{
	return SHARED_OBJ GetPwrTbleFileName(
#if defined (_MIMO_)
		DeviceId,	PathId,		
#endif // #if defined (_MIMO_)
		BandId,		SubBandId,   
		Upload,		pGivenFileName); 

}

DUT_SHARED_API int STDCALL Dut_Shared_ReadPwrTableFile(
#if defined (_MIMO_)
						int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
						int BandId, int SubBandId,  
						int *pHighFreqBondary, int *pLowFreqBondary,
						int pPpaGain1_Rng[RFPWRRANGE_NUM], 
						int pPpaGain2_Rng[RFPWRRANGE_NUM],  
						int pMaxPwr_Rng[RFPWRRANGE_NUM], 
						int pMinPwr_Rng[RFPWRRANGE_NUM], 
						int *pSizeOfPwrTblInByte, BYTE* pPwrTbl, 
						char* pGivenFileName)
{
	return SHARED_OBJ ReadPwrTableFile(
#if defined (_MIMO_)
						DeviceId,		PathId,		
#endif // #if defined (_MIMO_)
						BandId, SubBandId,  
						pHighFreqBondary, 
						pLowFreqBondary,
						pPpaGain1_Rng, 
						pPpaGain2_Rng,  
						pMaxPwr_Rng, 
						pMinPwr_Rng, 
						pSizeOfPwrTblInByte, 
						pPwrTbl, 
						pGivenFileName); 
}


DUT_SHARED_API int STDCALL Dut_Shared_WritePwrTableFile(
#if defined (_MIMO_)
						int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
						int BandId, int SubBandId,   
						int HighFreqBondary, int LowFreqBondary,
						int pPpaGain1_Rng[RFPWRRANGE_NUM], 
					   int pPpaGain2_Rng[RFPWRRANGE_NUM],  
					   int pMaxPwr_Rng[RFPWRRANGE_NUM], 
					   int pMinPwr_Rng[RFPWRRANGE_NUM], 
					   int *pSizeOfPwrTblInByte, BYTE* pPwrTbl, 
					   char* pGivenFileName)
{
	return SHARED_OBJ WritePwrTableFile(
#if defined (_MIMO_)
						DeviceId,		PathId,		
#endif // #if defined (_MIMO_)
						BandId, SubBandId,   
						HighFreqBondary, LowFreqBondary,
						pPpaGain1_Rng, 
					   pPpaGain2_Rng,  
					   pMaxPwr_Rng, 
					   pMinPwr_Rng, 
					   pSizeOfPwrTblInByte,  pPwrTbl, 
					    pGivenFileName); 
}

//////////////////////////////////////////
DUT_SHARED_API int STDCALL Dut_Shared_ReadPwrTableFile_RevD(
#if defined (_MIMO_)
						int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
						int BandId, int SubBandId,  
						int *pHighFreqBondary, int *pLowFreqBondary,
						int *pSizeOfPwrTblInByte, BYTE* pPwrTbl, 
						char* pGivenFileName)
{
	return SHARED_OBJ ReadPwrTableFile_RevD(
#if defined (_MIMO_)
						DeviceId,		PathId,		
#endif // #if defined (_MIMO_)
						BandId, SubBandId,  
						pHighFreqBondary, 
						pLowFreqBondary,
						pSizeOfPwrTblInByte, 
						pPwrTbl, 
						pGivenFileName); 
}

DUT_SHARED_API int STDCALL Dut_Shared_WritePwrTableFile_RevD(
#if defined (_MIMO_)
						int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
						int BandId, int SubBandId, 
						int HighFreqBondary, int LowFreqBondary,
						int *pSizeOfPwrTblInByte, BYTE* pPwrTbl, 
					    char* pGivenFileName)
{
	return SHARED_OBJ WritePwrTableFile_RevD(
#if defined (_MIMO_)
						DeviceId,		PathId,		
#endif // #if defined (_MIMO_)
						BandId, SubBandId,   
						HighFreqBondary, LowFreqBondary,
					    pSizeOfPwrTblInByte,  pPwrTbl, 
					    pGivenFileName); 
}
#endif //#if defined (_WLAN_)

DUT_SHARED_API int STDCALL Dut_Shared_GetCalMainDataRevD(
						BYTE *pStructRev,
						BYTE *pDesignType,
						BYTE *pLNAGain,
						BYTE *pAntDiversityInfo,
						BYTE *pFEVersion,

						bool *pPaOption_ExtLnaFlag_PathABC_G,
						bool *pPaOption_ExtLnaFlag_PathABC_A,
	
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
						BYTE *pCapabilities_2G,
						BYTE *pCapabilities_5G,

						BYTE  pBTFrontEnd2G[9],
						BYTE  pBTFrontEnd5G[9],	
					
						DWORD *pTestToolVer,	DWORD *pMfgTaskVersion,	
						DWORD *pDllVersion, 
						BYTE *pSocOrRev, BYTE *pRfuOrRev,
						BYTE *pMimoTxCnt, 	BYTE *pMimoRxCnt, 
						BYTE pMimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH])
{
	return SHARED_OBJ  getCalMainDataRevD(
						pStructRev,
						pDesignType,
						pLNAGain,
						pAntDiversityInfo,
						pFEVersion,

						pPaOption_ExtLnaFlag_PathABC_G,
						pPaOption_ExtLnaFlag_PathABC_A,
	
						pPaOption_ExtPaPolar_PathABC_G,
						pPaOption_ExtPaPolar_PathABC_A,

						pSpiSizeInfo,
 						pCC,
						
						pExtXtalSource,		
						pExtSleepClk,	
						pWlanWakeUp,
						
						pAntTx,		//
						pAntRx,		//

						//Capabilities
						pCapable_2G, //
						pCapable_5G, //
						pInternalBluetooth,	//
						pAntennaDiversity,
						pLNA_2G,	//
						pLNA_5G,
						pSameAnt_2G_BT,
						pSameAnt_5G_BT,

						//Concurrency
						pCapabilities_2G,
						pCapabilities_5G,
					
						pBTFrontEnd2G,
						pBTFrontEnd5G,

						pTestToolVer,	pMfgTaskVersion,	
						pDllVersion, 
						pSocOrRev, pRfuOrRev,
						pMimoTxCnt, pMimoRxCnt, 
						pMimoMap);
}

DUT_SHARED_API int STDCALL Dut_Shared_SetCalMainDataRevD( 
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
						bool *pSameAnt_5G_BT,	//

						//Concurrency
						BYTE *Capabilities_2G,
						BYTE *Capabilities_5G,

						BYTE  BTFrontEnd2G[9],
						BYTE  BTFrontEnd5G[9],	
					
						DWORD *pTestToolVer,	DWORD *pMfgTaskVersion,	
						DWORD *pDllVersion, 
						BYTE *pSocOrRev, BYTE *pRfuOrRev,
						BYTE *pMimoTxCnt, 	BYTE *pMimoRxCnt, 
						BYTE pMimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH])
{
	return SHARED_OBJ  setCalMainDataRevD(
						pStructRev,
						pDesignType,
						pSpiSizeInfo,
						pLNAGain,
						pAntDiversityInfo,
						pFEVersion,
 						pCC,
						
						pExtXtalSource,		
						pExtSleepClk,	
						pWlanWakeUp,
						
						pAntTx,		//
						pAntRx,		//

						//Capabilities
						pCapable_2G, //
						pCapable_5G, //
						pInternalBluetooth,	//
						pAntennaDiversity,
						pLNA_2G,	//
						pLNA_5G,
						pSameAnt_2G_BT,
						pSameAnt_5G_BT,
						
						//Concurrency
						Capabilities_2G,
						Capabilities_5G,

						BTFrontEnd2G,
						BTFrontEnd5G,

						pTestToolVer,	pMfgTaskVersion,	
						pDllVersion, 
						pSocOrRev, pRfuOrRev,
						pMimoTxCnt, pMimoRxCnt, 
						pMimoMap);
}


DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType27Data(BYTE *pLED)
{
	return SHARED_OBJ GetAnnexType27Data(pLED);
}

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType27Data(BYTE *pLED) 
{
	return SHARED_OBJ SetAnnexType27Data(pLED);
}

DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType28Data(	BYTE *pUsbVendorString, 
						BYTE *pUsbProductString)
{
	return SHARED_OBJ GetAnnexType28Data(pUsbVendorString, pUsbProductString);
}

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType28Data(BYTE *pUsbVendorString, 
						BYTE *pUsbProductString)
{
	return SHARED_OBJ SetAnnexType28Data(pUsbVendorString, 
						pUsbProductString);
}

DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType31Data(
#if defined (_MIMO_)
 						   int DeviceId, 
#endif //#if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G])
{
	return SHARED_OBJ GetAnnexType31Data(
#if defined (_MIMO_)
 						   DeviceId, 
#endif //#if defined (_MIMO_) 
						pRssi_Nf, 
						pRssi_cck, 
						pRssi_ofdm, 
						pRssi_mcs);
}

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType31Data(	
#if defined (_MIMO_)
 						   int DeviceId, 
#endif //#if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G])
{
	return SHARED_OBJ SetAnnexType31Data(
#if defined (_MIMO_)
 						   DeviceId, 
#endif //#if defined (_MIMO_) 
						pRssi_Nf, 
						pRssi_cck, 
						pRssi_ofdm, 
						pRssi_mcs);
}

DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType32Data(
#if defined (_MIMO_)
 						   int DeviceId, 
#endif //#if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A])
{
	return SHARED_OBJ  GetAnnexType32Data(
#if defined (_MIMO_)
 						   DeviceId, 
#endif //#if defined (_MIMO_)
						pRssi_Nf, 
						pRssi_cck, 
						pRssi_ofdm, 
						pRssi_mcs); 
}

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType32Data(	
#if defined (_MIMO_)
 						   int DeviceId, 
#endif //#if defined (_MIMO_) 
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A])
{
	return SHARED_OBJ  SetAnnexType32Data(
#if defined (_MIMO_)
 						   DeviceId, 
#endif //#if defined (_MIMO_)
						pRssi_Nf, 
						pRssi_cck, 
						pRssi_ofdm, 
						pRssi_mcs); 
}

DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType40Data( WORD *pVid,		WORD *pPid,	
						BYTE *pChipId,	BYTE *pChipVersion,					   
						BYTE *pFlag_SettingPrev, 
						BYTE *pNumOfRailAvaiableInChip,
						BYTE pPowerDownMode[MAX_PM_RAIL_PW886],
						BYTE pNormalMode[MAX_PM_RAIL_PW886], 
						BYTE pSleepModeFlag[MAX_PM_RAIL_PW886],
						BYTE pSleepModeSetting[MAX_PM_RAIL_PW886],
						BYTE *pDpSlp)
{
	return SHARED_OBJ GetAnnexType40Data( pVid,		pPid,	
						pChipId,	pChipVersion,					   
						pFlag_SettingPrev, 
						pNumOfRailAvaiableInChip,
						pPowerDownMode,
						pNormalMode, 
						pSleepModeFlag, pSleepModeSetting,
						pDpSlp);
}

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType40Data(	WORD *pVid,	WORD *pPid,	
						BYTE *pChipId,	BYTE *pChipVersion,					   
						BYTE *pFlag_SettingPrev, BYTE *pNumOfRailAvaiableInChip,
						BYTE pPowerDownMode[MAX_PM_RAIL_PW886],
						BYTE pNormalMode[MAX_PM_RAIL_PW886], 
						BYTE pSleepModeFlag[MAX_PM_RAIL_PW886],
						BYTE pSleepModeSetting[MAX_PM_RAIL_PW886],
						BYTE *pDpSlp)
{
	return SHARED_OBJ SetAnnexType40Data( pVid,		pPid,	
						pChipId,	pChipVersion,					   
						pFlag_SettingPrev, 
						pNumOfRailAvaiableInChip,
						pPowerDownMode,
						pNormalMode, 
						pSleepModeFlag, pSleepModeSetting,
						pDpSlp);
}

DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType45Data(	bool *pFlag_GpioCtrlXosc,
						BYTE *pXoscSettling,
						DWORD *pPinOut_DrvLow,
						DWORD *pPinIn_PullDiable)
{	
	return SHARED_OBJ GetAnnexType45Data(	
						pFlag_GpioCtrlXosc,
						pXoscSettling,
						pPinOut_DrvLow,
						pPinIn_PullDiable);
}

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType45Data(
						bool Flag_GpioCtrlXosc,
					   BYTE XoscSettling,
					   DWORD *pPinOut_DrvLow,
					   DWORD *pPinIn_PullDiable
						)
{	
	return SHARED_OBJ SetAnnexType45Data(	
						Flag_GpioCtrlXosc,
						XoscSettling,
						pPinOut_DrvLow,
						pPinIn_PullDiable);
}
#if defined (_MIMO_)

DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType47Data( int DeviceId, 
						int PathId, 
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G]) 
{
	return SHARED_OBJ GetAnnexType47Data(DeviceId, 
						PathId,  
						pRssi_Nf, 
						pRssi_cck, 
						pRssi_ofdm, 
						pRssi_mcs);
}

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType47Data(	
						int DeviceId, 
						int PathId,
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G])
{
	return SHARED_OBJ SetAnnexType47Data(DeviceId, 
						PathId,  
						pRssi_Nf, 
						pRssi_cck, 
						pRssi_ofdm, 
						pRssi_mcs);
}
						
DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType48Data(
						int DeviceId, 
						int PathId, 
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A])
{
	return SHARED_OBJ GetAnnexType48Data(DeviceId, 
						PathId,  
						pRssi_Nf, 
						pRssi_cck, 
						pRssi_ofdm, 
						pRssi_mcs);
}						

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType48Data(	
 						int DeviceId, 
						int PathId,
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A])
{
	return SHARED_OBJ SetAnnexType48Data(DeviceId, 
						PathId,  
						pRssi_Nf, 
						pRssi_cck, 
						pRssi_ofdm, 
						pRssi_mcs);
}					
#endif //#if defined (_MIMO_)

//Rev D
DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType42Data_VD( 
					BYTE *pNoOfEntry,
					THERMALCALPARMS_VD *ThermalCalParam)
{

		return SHARED_OBJ SetAnnexType42Data_VD( 
					pNoOfEntry,
					ThermalCalParam);
}

DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType42Data_VD( 
					BYTE *pNoOfEntry,
					THERMALCALPARMS_VD *ThermalCalParam)
{

	return SHARED_OBJ GetAnnexType42Data_VD( 
					pNoOfEntry,
					ThermalCalParam);
}

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType49Data(
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					BYTE *pNumOfTxPowerLevelStore,
					BYTE *pNumOfSavedCh,
					bool *pCalMethod,
					bool *pCloseTXPWDetection,
					bool *pExtPAPolarize,

					BYTE *pCalXtal,
					BYTE *pPdetHiLoGap,
					BYTE *pMCS20MPdetOffset,
					BYTE *pMCS20MInitPOffset,
					BYTE *pMCS40MPdetOffset,
					BYTE *pMCS40MInitPOffset,

					int  pChNumList[MAXSAVEDCH_G_REVD],

					char pCorrInitTrgt_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
					char pCorrPDetTh_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 

					char pCorrInitTrgt_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
					char pCorrPDetTh_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
	
					BYTE* pwrTbl)
{
	return SHARED_OBJ SetAnnexType49Data(
#if defined (_MIMO_)
					DeviceId,		PathId,		
#endif // #if defined (_MIMO_)
					pNumOfTxPowerLevelStore,
					pNumOfSavedCh,
					pCalMethod,
					pCloseTXPWDetection,
					pExtPAPolarize,

					pCalXtal,

					pPdetHiLoGap,
					pMCS20MPdetOffset,
					pMCS20MInitPOffset,
					pMCS40MPdetOffset,
					pMCS40MInitPOffset,

					pChNumList,
	
					pCorrInitTrgt_B, 
					pCorrPDetTh_B,
					
					pCorrInitTrgt_G, 
					pCorrPDetTh_G, 

					pwrTbl);
}

DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType49Data(
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					BYTE *pNumOfTxPowerLevelStore,
					BYTE *pNumOfSavedCh,
					bool *pCalMethod,
					bool *pCloseTXPWDetection,
					bool *pExtPAPolarize,
					BYTE *pCalXtal,

					BYTE *pPdetHiLoGap,
					BYTE *pMCS20MPdetOffset,
					BYTE *pMCS20MInitPOffset,
					BYTE *pMCS40MPdetOffset,
					BYTE *pMCS40MInitPOffset,

					int  pChNumList[MAXSAVEDCH_G_REVD],

					char pCorrInitTrgt_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
					char pCorrPDetTh_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD],
					
					char pCorrInitTrgt_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
					char pCorrPDetTh_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
	
					BYTE* pPwrTbl)
{
	return SHARED_OBJ GetAnnexType49Data(
#if defined (_MIMO_)
					DeviceId,		PathId,		
#endif // #if defined (_MIMO_)
					pNumOfTxPowerLevelStore,
					pNumOfSavedCh,
					pCalMethod,
					pCloseTXPWDetection,
					pExtPAPolarize,

					pCalXtal,

					pPdetHiLoGap,
					pMCS20MPdetOffset,
					pMCS20MInitPOffset,
					pMCS40MPdetOffset,
					pMCS40MInitPOffset,

					pChNumList,
					pCorrInitTrgt_B, 
					pCorrPDetTh_B,

					pCorrInitTrgt_G, 
					pCorrPDetTh_G,
					pPwrTbl);
}

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType50Data(
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
	int		SubBandId, 
	BYTE	*pNumOfSavedChInSubBand,
	BYTE	*pNumOfTxPowerLevelStore,
	BYTE	*pNumOfSavedCh,
	bool	*pCal_Method,
	bool 	*pClose_TX_PW_Detection,
	bool    *pExtPAPolarize,
	BYTE	*pCalXtal,
	
	BYTE	*pPdetHiLoGap,
	BYTE	*pMCS20MPdetOffset,
	BYTE	*pMCS20MInitPOffset,
	BYTE	*pMCS40MPdetOffset,
	BYTE	*pMCS40MInitPOffset,

	BYTE	pSavedChannelIndex[MAXSAVEDCH_G_REVD],

	WORD	*pFreqBound_Lo,
	WORD	*pFreqBound_Hi,

	char	pCorrInitTrgt[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
	char	pCorrPDetTh[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD],

	BYTE* pPwrTbl)
{
	return SHARED_OBJ SetAnnexType50Data(
#if defined (_MIMO_)
	DeviceId,		PathId,		
#endif // #if defined (_MIMO_)
	SubBandId, 
	pNumOfSavedChInSubBand,
	pNumOfTxPowerLevelStore,
	pNumOfSavedCh,
	pCal_Method,
	pClose_TX_PW_Detection,
	pExtPAPolarize,
	pCalXtal,
	
	pPdetHiLoGap,
	pMCS20MPdetOffset,
	pMCS20MInitPOffset,
	pMCS40MPdetOffset,
	pMCS40MInitPOffset,

	pSavedChannelIndex,

	pFreqBound_Lo,
	pFreqBound_Hi,

	pCorrInitTrgt, 
	pCorrPDetTh,

	pPwrTbl);
}
	
DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType50Data(
#if defined (_MIMO_)
	int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
	int		SubBandId, 
	BYTE	*pNumOfSavedChInSubBand,
	BYTE	*pNumOfTxPowerLevelStore,
	
	bool	*pCal_Method,
	bool 	*pClose_TX_PW_Detection,
    bool    *pExtPAPolarize,
	BYTE	*pCalXtal,

	BYTE	*pPdetHiLoGap,
	BYTE	*pMCS20MPdetOffset,
	BYTE	*pMCS20MInitPOffset,
	BYTE	*pMCS40MPdetOffset,
	BYTE	*pMCS40MInitPOffset,
	
	BYTE	pSavedChannelIndex[MAXSAVEDCH_G_REVD],	
	
	WORD	*pFreqBound_Lo,
	WORD	*pFreqBound_Hi,

	char	pCorrInitTrgt[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
	char	pCorrPDetTh[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD],
	
	BYTE	*pPwrTbl)
{
	return SHARED_OBJ GetAnnexType50Data(
#if defined (_MIMO_)
	DeviceId,		PathId,		
#endif // #if defined (_MIMO_)
	SubBandId, 
	pNumOfSavedChInSubBand,
	pNumOfTxPowerLevelStore,
	
	pCal_Method,
	pClose_TX_PW_Detection,
	pExtPAPolarize,
	pCalXtal,

	pPdetHiLoGap,
	pMCS20MPdetOffset,
	pMCS20MInitPOffset,
	pMCS40MPdetOffset,
	pMCS40MInitPOffset,
	
	pSavedChannelIndex,	
	
	pFreqBound_Lo,
	pFreqBound_Hi,

	pCorrInitTrgt, 
	pCorrPDetTh, 
	
	pPwrTbl);
}

DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType51Data(
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)	
					BYTE	*pOFDM_TH_LO_Delta_40M,
					BYTE	*pOFDM_InitP_Delta_40M,
					BYTE	*pOFDM_TH_LO_Delta_20M,
					BYTE	*pOFDM_InitP_Delta_20M,
	
					BYTE	*pMCS_TH_LO_Delta_40M,
					BYTE	*pMCS_InitP_Delta_40M,
					BYTE	*pMCS_TH_LO_Delta_20M,
					BYTE	*pMCS_InitP_Delta_20M)
{
return SHARED_OBJ GetAnnexType51Data(
#if defined (_MIMO_)
					DeviceId,		PathId,		
#endif // #if defined (_MIMO_)	
					pOFDM_TH_LO_Delta_40M,
					pOFDM_InitP_Delta_40M,
					pOFDM_TH_LO_Delta_20M,
					pOFDM_InitP_Delta_20M,
	
					pMCS_TH_LO_Delta_40M,
					pMCS_InitP_Delta_40M,
					pMCS_TH_LO_Delta_20M,
					pMCS_InitP_Delta_20M);

}
 

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType51Data(
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)	
					
					BYTE	*pOFDM_TH_LO_Delta_40M,
					BYTE	*pOFDM_InitP_Delta_40M,
					BYTE	*pOFDM_TH_LO_Delta_20M,
					BYTE	*pOFDM_InitP_Delta_20M,
	
					BYTE	*pMCS_TH_LO_Delta_40M,
					BYTE	*pMCS_InitP_Delta_40M,
					BYTE	*pMCS_TH_LO_Delta_20M,
					BYTE	*pMCS_InitP_Delta_20M
)
{

return SHARED_OBJ SetAnnexType51Data(
#if defined (_MIMO_)
					DeviceId,		PathId,		
#endif // #if defined (_MIMO_)	
					pOFDM_TH_LO_Delta_40M,
					pOFDM_InitP_Delta_40M,
					pOFDM_TH_LO_Delta_20M,
					pOFDM_InitP_Delta_20M,
	
					pMCS_TH_LO_Delta_40M,
					pMCS_InitP_Delta_40M,
					pMCS_TH_LO_Delta_20M,
					pMCS_InitP_Delta_20M);
	return 0;
 }

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType52Data(
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
					    bool Cal2G,
						bool Cal5G,
						BYTE LoLeakageCal2G_I,
						BYTE LoLeakageCal2G_Q,
						BYTE LoLeakageCal5G_I[NUM_SUBBAND_A_REVD],
						BYTE LoLeakageCal5G_Q[NUM_SUBBAND_A_REVD])
{
	return SHARED_OBJ SetAnnexType52Data(
#if defined (_MIMO_)
					   DeviceId,		PathId,  
#endif //#if defined (_MIMO_)
						Cal2G,
						Cal5G,
						LoLeakageCal2G_I,
						LoLeakageCal2G_Q,
						LoLeakageCal5G_I,
						LoLeakageCal5G_Q);
}


DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType52Data( 
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
					    bool *Cal2G,
						bool *Cal5G,
						BYTE *LoLeakageCal2G_I,
						BYTE *LoLeakageCal2G_Q,
						BYTE LoLeakageCal5G_I[NUM_SUBBAND_A_REVD],
						BYTE LoLeakageCal5G_Q[NUM_SUBBAND_A_REVD]
						) 
{
	return  SHARED_OBJ GetAnnexType52Data(
#if defined (_MIMO_)
					   DeviceId,		PathId,  
#endif //#if defined (_MIMO_)
					    Cal2G,
						Cal5G,
						LoLeakageCal2G_I,
						LoLeakageCal2G_Q,
						LoLeakageCal5G_I,
						LoLeakageCal5G_Q);
}

DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType53Data(
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
						bool  *Cal_2G,
						bool  *Cal_5G,
						char *TxIQmismatch_Amp_2G,	
						char *TxIQmismatch_Phase_2G,
						char TxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	
						char TxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD])
{
		return  SHARED_OBJ GetAnnexType53Data( 
#if defined (_MIMO_)
					   DeviceId,		PathId,   
#endif //#if defined (_MIMO_)
						Cal_2G,
						Cal_5G,
						TxIQmismatch_Amp_2G,	
						TxIQmismatch_Phase_2G,
						TxIQmismatch_Amp_5G,	
						TxIQmismatch_Phase_5G);

}

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType53Data( 
#if defined (_MIMO_)
					   int DeviceId,		int PathId,   
#endif //#if defined (_MIMO_)
						bool  Cal_2G,
						bool  Cal_5G,
						char TxIQmismatch_Amp_2G,	
						char TxIQmismatch_Phase_2G,
						char TxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	
						char TxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD])
{
	return  SHARED_OBJ SetAnnexType53Data( 
#if defined (_MIMO_)
						DeviceId,		PathId,  
#endif //#if defined (_MIMO_)
						Cal_2G,
						Cal_5G,
						TxIQmismatch_Amp_2G,	
						TxIQmismatch_Phase_2G,
						TxIQmismatch_Amp_5G,	
						TxIQmismatch_Phase_5G);
}

DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType54Data( 
#if defined (_MIMO_)
					   int DeviceId,		int PathId,   
#endif //#if defined (_MIMO_)
						bool  *Cal_2G,
						bool  *Cal_5G,
						char *RxIQmismatch_Amp_2G,	
						char *RxIQmismatch_Phase_2G,
						char RxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	
						char RxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD])
{
		return  SHARED_OBJ GetAnnexType54Data( 
#if defined (_MIMO_)
					   DeviceId,		PathId,  
#endif //#if defined (_MIMO_)
						Cal_2G,
						Cal_5G,
						RxIQmismatch_Amp_2G,	
						RxIQmismatch_Phase_2G,
						RxIQmismatch_Amp_5G,	
						RxIQmismatch_Phase_5G);

}

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType54Data(
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
						bool  Cal_2G,
						bool  Cal_5G,
						char RxIQmismatch_Amp_2G,	
						char RxIQmismatch_Phase_2G,
						char RxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	
						char RxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD])
{
		return  SHARED_OBJ SetAnnexType54Data( 
#if defined (_MIMO_)
					   DeviceId,		PathId,   
#endif //#if defined (_MIMO_)
						Cal_2G,
						Cal_5G,
						RxIQmismatch_Amp_2G,	RxIQmismatch_Phase_2G,
						RxIQmismatch_Amp_5G,	RxIQmismatch_Phase_5G);

}

DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType55Data(
					BYTE *Version,
					BYTE *RFXtal,
					BYTE *InitPwr,
					BYTE *FELoss,

					bool *ForceClass2Op,
					bool *DisablePwrControl,
					bool *MiscFlag,
					bool *UsedInternalSleepClock,
					BYTE *Rssi_Golden_Lo,
					BYTE *Rssi_Golden_Hi,
					DWORD *BTBAUDRate,
					BYTE BDAddr[SIZE_BDADDRESS_INBYTE],
					BYTE *Encr_Key_Len_Max,
					BYTE *Encr_Key_Len_Min
					)
{
	return  SHARED_OBJ GetAnnexType55Data(
					Version,
					RFXtal,
					InitPwr,
					FELoss,

					ForceClass2Op,
					DisablePwrControl,
					MiscFlag,
					UsedInternalSleepClock,
					Rssi_Golden_Lo,
					Rssi_Golden_Hi,
					BTBAUDRate,
					BDAddr,
					Encr_Key_Len_Max,
					Encr_Key_Len_Min);
}

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType55Data(
					BYTE Version,
					BYTE RFXtal,
					BYTE InitPwr,
					BYTE FELoss,

					bool ForceClass2Op,
					bool DisablePwrControl,
					bool MiscFlag,
					bool UsedInternalSleepClock,

					BYTE Rssi_Golden_Lo,
					BYTE Rssi_Golden_Hi,
					DWORD BTBAUDRate,
					BYTE BDAddr[SIZE_BDADDRESS_INBYTE],
					BYTE Encr_Key_Len_Max,
					BYTE Encr_Key_Len_Min
					)
{
	return  SHARED_OBJ SetAnnexType55Data(
					Version,
					RFXtal,
					InitPwr,
					FELoss,

					ForceClass2Op,
					DisablePwrControl,
					MiscFlag,
					UsedInternalSleepClock,
					Rssi_Golden_Lo,
					Rssi_Golden_Hi,
					BTBAUDRate,
					BDAddr,
					Encr_Key_Len_Max,
					Encr_Key_Len_Min);

}

DUT_SHARED_API int STDCALL  Dut_Shared_GetAnnexType56Data(
					BYTE Lincense_Key[16],
	                WORD *Link_Key_Mask,
	                BYTE Key_Value[3][22])
{
	return  SHARED_OBJ GetAnnexType56Data(
					Lincense_Key,
	                Link_Key_Mask,
	                Key_Value);
}

DUT_SHARED_API int STDCALL  Dut_Shared_SetAnnexType56Data(
					BYTE Lincense_Key[16],
	                WORD Link_Key_Mask,
	                BYTE Key_Value[3][22])
{
	return  SHARED_OBJ SetAnnexType56Data( 
					Lincense_Key,
	                Link_Key_Mask,
	                Key_Value);
}


//Annex57
DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex57DataFromFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					BYTE Band,
					BYTE SubBand,
					BYTE *MEM_ROW,
					BYTE *NumOfEnterirs,
					DPD_ConeffcicentData *Coneffcicent_Data
					)
{
	return SHARED_OBJ ReadAnnex57DataFromFile(FileName,
#if defined (_MIMO_)
					DeviceId,		PathId,		
#endif // #if defined (_MIMO_)
					Band,
					SubBand,
					MEM_ROW,
					NumOfEnterirs,
					Coneffcicent_Data);
					
}
DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex57DataToFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					BYTE Band,
					BYTE SubBand,
					BYTE MEM_ROW,
					BYTE NumOfEnterirs,
					DPD_ConeffcicentData *Coneffcicent_Data
					)
{
	return SHARED_OBJ WriteAnnex57DataToFile(FileName,
#if defined (_MIMO_)
					DeviceId,		PathId,		
#endif // #if defined (_MIMO_)
					Band,
					SubBand,
					MEM_ROW,
					NumOfEnterirs,
					Coneffcicent_Data);
					
}
DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType57Data(
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
						BYTE Band,
						BYTE SubBand,
						BYTE MEM_ROW,
						BYTE NumOfEnterirs,
						DPD_ConeffcicentData *Coneffcicent_Data)
{
	return SHARED_OBJ SetAnnexType57Data(
#if defined (_MIMO_)
					   DeviceId,		PathId,  
#endif //#if defined (_MIMO_)
						Band,
						SubBand,
						MEM_ROW,
						NumOfEnterirs,
						Coneffcicent_Data);
}

DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType57Data(
#if defined (_MIMO_)
						int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
						BYTE BandId,
						BYTE SubBandId,
						BYTE *MEM_ROW,
						BYTE *NumOfEnterirs,
						DPD_ConeffcicentData *Coneffcicent_Data)
{
	return SHARED_OBJ GetAnnexType57Data(
#if defined (_MIMO_)
					   DeviceId,		PathId,  
#endif //#if defined (_MIMO_)
						BandId,
						SubBandId,
						MEM_ROW,
						NumOfEnterirs,
						Coneffcicent_Data);
}


DUT_SHARED_API int STDCALL Dut_Shared_PurgeCalPointers(void)
{
	return SHARED_OBJ PurgeAnnexPointers();
}

