/** @file DutApi878XDll.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */
 
#include "../DutCommon/Clss_os.h"
  
#ifdef _LINUX_FEDORA_

#define DUT_SHARED_API	extern "C"
//#define NULL 0
//#define DUT_WLAN_API
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned int BOOL;
typedef unsigned int BOOLEAN;

#else   //#ifdef _LINUX_FEDORA_
#if defined DUTAPI8787DLL_EXPORTS || defined DUTAPI8XXXDLL_EXPORTS
#define DUT_SHARED_API	extern "C" __declspec(dllexport)
#else // #if defined DUTAPI8787DLL_EXPORTS || defined DUTAPI8XXXDLL_EXPORTS
#ifdef _DEBUG
#define DUT_SHARED_API	extern "C"
#else // #ifdef _DEBUG
#define DUT_SHARED_API	extern "C" __declspec(dllimport)  
#endif // #ifdef _DEBUG
#endif // #ifdef DUTAPI8787DLL_EXPORTS || defined DUTAPI8XXXDLL_EXPORTS

#endif // #ifdef _LINUX_FEDORA_

//#if defined (_WLAN_)
#define DUT_WLAN_API DUT_SHARED_API
//#else // #if defined (_WLAN_)
//#define DUT_WLAN_API
//#endif //#if defined (_WLAN_)


#if defined (_BT_)
#define DUT_BT_API DUT_SHARED_API
#else // #if defined (_BT_)
#define DUT_BT_API
#endif //#if defined (_BT_)


#if defined (_FM_)
#define DUT_FM_API DUT_SHARED_API
#else // #if defined (_FM_)
#define DUT_FM_API
#endif //#if defined (_FM_)


#include "DutWlanApi.hc"
#include "DutBtApi.hc"

DUT_SHARED_API int STDCALL Dut_Shared_SupportNoE2PROM(void);
DUT_SHARED_API int STDCALL Dut_Shared_GetWriteTemplateFlag(void);
DUT_SHARED_API void STDCALL Dut_Shared_SetWriteTemplateFlag(int Flag);

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
						BYTE pMimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]);

DUT_SHARED_API int STDCALL Dut_Shared_WriteMainDataToFile_RevD(char *FileName,
						BYTE pStructRev,
						BYTE pDesignType,
						BYTE pLNAGain,
						BYTE AntDiversityInfo,
						BYTE pFEMVersion,
						
						bool PaOption_ExtLnaFlag_PathABC_G,
						bool PaOption_ExtLnaFlag_PathABC_A,
	
						bool PaOption_ExtPaPolar_PathABC_G,
						bool PaOption_ExtPaPolar_PathABC_A,


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
						BYTE pSocOrRev, BYTE pRfuOrRev,
						BYTE pMimoTxCnt, 	BYTE pMimoRxCnt, 
						BYTE pMimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]);

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex27DataFromFile(char *FileName,
															  BYTE *pLED);

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex27DataToFile(char *FileName,
															 BYTE *pLED);

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex28DataFromFile(char *FileName, 
								   BYTE *pUsbVendorString, 
								   BYTE *pUsbProductString);

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex28DataToFile(char *FileName, 
								   BYTE *pUsbVendorString, 
								   BYTE *pUsbProductString);

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex31DataFromFile(char *FileName,
#if defined (_MIMO_)
						   int DeviceId, //int PathId, int BandId,
#endif	// #if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G]);

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex31DataToFile(char *FileName,
#if defined (_MIMO_)
						   int DeviceId, //int PathId, int BandId,
#endif	// #if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G]);


DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex32DataFromFile(char *FileName,
#if defined (_MIMO_)
						   int DeviceId, //int PathId, int BandId,
#endif	// #if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A]);


DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex32DataToFile(char *FileName,
#if defined (_MIMO_)
						   int DeviceId, //int PathId, int BandId,
#endif	// #if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A]);
 
DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex40DataFromFile(char *FileName,
						WORD *pVid,		WORD *pPid,	
						BYTE *pChipId,	BYTE *pChipVersion,					   
						BYTE *pFlag_SettingPrev, 
						BYTE *pNumOfRailAvaiableInChip,
						BYTE *pPowerDownMode,
						BYTE pNormalMode[MAX_PM_RAIL_PW886], 
						BYTE pSleepModeFlag[MAX_PM_RAIL_PW886],
						BYTE pSleepModeSetting[MAX_PM_RAIL_PW886],
						BYTE *pDpSlp);

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex40DataToFile(char *FileName,
						WORD *pVid,		WORD *pPid,	
						BYTE *pChipId,	BYTE *pChipVersion,					   
						BYTE *pFlag_SettingPrev, 
						BYTE *pNumOfRailAvaiableInChip,
						BYTE *pPowerDownMode,
						BYTE pNormalMode[MAX_PM_RAIL_PW886], 
						BYTE pSleepModeFlag[MAX_PM_RAIL_PW886],
						BYTE pSleepModeSetting[MAX_PM_RAIL_PW886],
						BYTE *pDpSlp);

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
					   BYTE pSecurityKey[MAX_BYTE_SECURITYKEY]);

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
					   BYTE pSecurityKey[MAX_BYTE_SECURITYKEY]);

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex45DataFromFile(char *FileName,  
											bool *pFlag_GpioCtrlXosc,
											BYTE *pXoscSettling,
											DWORD *pPinOut_DrvLow,
											DWORD *pPinIn_PullDiable);

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex45DataToFile(char *FileName,  
						   bool *pFlag_GpioCtrlXosc,
						BYTE *pXoscSettling,
						DWORD *pPinOut_DrvLow,
						DWORD *pPinIn_PullDiable);

#if defined (_MIMO_)
DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex47DataFromFile(char *FileName,
 						   int DeviceId, 
						   int PathId, //int BandId,
 						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G]);

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex47DataToFile(char *FileName,
 						   int DeviceId, 
						   int PathId, //int BandId,
 						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G]);


DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex48DataFromFile(char *FileName,
						   int DeviceId, 
						   int PathId,  //int BandId,
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A]);

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex48DataToFile(char *FileName,
 						   int DeviceId, 
						   int PathId, //int BandId,
 						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A]);
#endif //#if defined (_MIMO_)

//Rev D
DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType42Data_VD(BYTE *pNoOfEntry,
					THERMALCALPARMS_VD *ThermalCalParam);

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType42Data_VD(BYTE *pNoOfEntry,
					THERMALCALPARMS_VD *ThermalCalParam);

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex42DataToFile_VD(char *FileName,
					BYTE *pNoOfEntry,
					THERMALCALPARMS_VD *ThermalCalParam);

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex42DataFromFile_VD(char *FileName,
					BYTE *pNoOfEntry,
					THERMALCALPARMS_VD *ThermalCalParam);

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex49DataToFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					BYTE *pNumOfTxPowerLevelStore,
					BYTE *pNumOfSavedCh,
					bool *pCal_Method,
					bool *pClose_TX_PW_Detection,
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
					BYTE* pwrTbl);

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex49DataFromFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					BYTE *pNumOfTxPowerLevelStore,
					BYTE *pNumOfSavedCh,
					bool *pCal_Method,
					bool *pClose_TX_PW_Detection,
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
					char pCorrPDetTh_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD]
					);

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
					
					BYTE* pPwrTbl);

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType49Data(
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					BYTE *pNumOfTxPowerLevelStore,
					BYTE *pNumOfSavedCh,
					bool *pCalMethod,
					bool *pCloseTXPWDetection,
					bool *ExtPAPolarize,
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

					BYTE* pPwrTbl);

DUT_SHARED_API int STDCALL Dut_Shared_GetPwrTbleFileName(
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
						int BandId, int SubBandId,   
						int Upload, char* pGivenFileName);

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
						char* pGivenFileName);

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
					   char* pGivenFileName);

DUT_SHARED_API int STDCALL Dut_Shared_ReadPwrTableFile_RevD(
#if defined (_MIMO_)
						int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
						int BandId, int SubBandId,
						int *HighFreqBondary, int *LowFreqBondary,
					    int *pSizeOfPwrTblInByte, BYTE* pPwrTbl, 
					    char* pGivenFileName);

DUT_SHARED_API int STDCALL Dut_Shared_WritePwrTableFile_RevD(
#if defined (_MIMO_)
						int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
						int BandId, int SubBandId,
						int HighFreqBondary, int LowFreqBondary,
					    int *pSizeOfPwrTblInByte, BYTE* pPwrTbl, 
					    char* pGivenFileName);

/////////////////////////////////////////////
DUT_SHARED_API int STDCALL Dut_Shared_GetCalMainDataRevD(
						BYTE *pStructRev,
						BYTE *pDesignType,
						BYTE *pLNAGain,
						BYTE *pAntDiversityInfo,
                        BYTE *FEVersion,

						bool *PaOption_ExtLnaFlag_PathABC_G,
						bool *PaOption_ExtLnaFlag_PathABC_A,
	
						bool *PaOption_ExtPaPolar_PathABC_G,
						bool *PaOption_ExtPaPolar_PathABC_A,

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
						BYTE pMimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]);

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
						BYTE pMimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]);
DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType27Data(BYTE *pLED);
DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType27Data(BYTE *pLED);

DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType28Data(	BYTE *pUsbVendorString, 
						BYTE *pUsbProductString);
DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType28Data(BYTE *pUsbVendorString, 
						BYTE *pUsbProductString);

DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType31Data(
#if defined (_MIMO_)
 						   int DeviceId, 
#endif //#if defined (_MIMO_)
						//int PathId, int BandId,
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G]);

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType31Data(	
#if defined (_MIMO_)
 						   int DeviceId, 
#endif //#if defined (_MIMO_)
					   //int PathId, int BandId,
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G]);


DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType32Data(
#if defined (_MIMO_)
 						   int DeviceId, 
#endif //#if defined (_MIMO_)
					   //int PathId, int BandId,
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A]);

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType32Data(	
#if defined (_MIMO_)
 						   int DeviceId, 
#endif //#if defined (_MIMO_) 
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A]);

DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType40Data( WORD *pVid,		WORD *pPid,	
						BYTE *pChipId,	BYTE *pChipVersion,					   
						BYTE *pFlag_SettingPrev, 
						BYTE *pNumOfRailAvaiableInChip,
						BYTE *pPowerDownMode,
						BYTE pNormalMode[MAX_PM_RAIL_PW886], 
						BYTE pSleepModeFlag[MAX_PM_RAIL_PW886],
						BYTE pSleepModeSetting[MAX_PM_RAIL_PW886],
						BYTE *pDpSlp);

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType40Data(	WORD *pVid,	WORD *pPid,	
						BYTE *pChipId,	BYTE *pChipVersion,					   
						BYTE *pFlag_SettingPrev, BYTE *pNumOfRailAvaiableInChip,
						BYTE *pPowerDownMode,
						BYTE pNormalMode[MAX_PM_RAIL_PW886], 
						BYTE pSleepModeFlag[MAX_PM_RAIL_PW886],
						BYTE pSleepModeSetting[MAX_PM_RAIL_PW886],
						BYTE *pDpSlp);

DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType42Data( 
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
					BYTE *pPTargetTempList);


DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType42Data(
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
					BYTE *pPTargetTempList);
 
DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType45Data(	bool *pFlag_GpioCtrlXosc,
						BYTE *pXoscSettling,
						DWORD *pPinOut_DrvLow,
						DWORD *pPinIn_PullDiable);

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType45Data(
						bool Flag_GpioCtrlXosc,
					   BYTE XoscSettling,
					   DWORD *pPinOut_DrvLow,
					   DWORD *pPinIn_PullDiable);


#if defined (_MIMO_)

DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType47Data( int DeviceId, 
						int PathId, //int BandId,
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G]) ;

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType47Data(	int DeviceId, 
						int PathId, //int BandId,
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G]);
						
DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType48Data( int DeviceId, 
						int PathId, //int BandId,
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A]);			

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType48Data(	
 						int DeviceId, 
						int PathId, //int BandId,
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A]);
#endif //#if defined (_MIMO_)

//Rev D
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
	char	pCorrInitTrgt_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
	char	pCorrPDetTh_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 	
	BYTE* pPwrTbl);

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
	BYTE* pPwrTbl);

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
	char	CorrPDetTh[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD]
	);
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
	bool	*pExtPAPolarize,
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
	char	CorrPDetTh[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD]
	);	

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex52DataFromFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					bool *Cal2G,
					bool *Cal5G,
					BYTE *LoLeakageCal2G_I,
					BYTE *LoLeakageCal2G_Q,
					char LoLeakageCal5G_I[NUM_SUBBAND_A_REVD],
					char LoLeakageCal5G_Q[NUM_SUBBAND_A_REVD]);

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex52DataToFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					bool Cal2G,
					bool Cal5G,
					BYTE LoLeakageCal2G_I,
					BYTE LoLeakageCal2G_Q,
					char LoLeakageCal5G_I[NUM_SUBBAND_A_REVD],
					char LoLeakageCal5G_Q[NUM_SUBBAND_A_REVD]);



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
						) ;
DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType52Data(
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
					    bool Cal2G,
						bool Cal5G,
						BYTE LoLeakageCal2G_I,
						BYTE LoLeakageCal2G_Q,
						BYTE LoLeakageCal5G_I[NUM_SUBBAND_A_REVD],
						BYTE LoLeakageCal5G_Q[NUM_SUBBAND_A_REVD]);

//Annex 53

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex53DataFromFile(char *FileName,
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
						bool *Cal2G,
						bool *Cal5G,
						char *TxIQmismatch_Amp_2G,	char *TxIQmismatch_Phase_2G,
						char TxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	char TxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD]);

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex53DataToFile(char *FileName,
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
						bool Cal2G,
						bool Cal5G,
						char TxIQmismatch_Amp_2G,	char TxIQmismatch_Phase_2G,
						char TxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	char TxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD]);

DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType53Data( 
#if defined (_MIMO_)
					   int DeviceId,		int PathId,   
#endif //#if defined (_MIMO_)
						bool  *Cal_2G,
						bool  *Cal_5G,
						char *TxIQmismatch_Amp_2G,	
						char *TxIQmismatch_Phase_2G,
						char TxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	
						char TxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD]);

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType53Data( 
#if defined (_MIMO_)
					   int DeviceId,		int PathId,   
#endif //#if defined (_MIMO_)
						bool  Cal_2G,
						bool  Cal_5G,
						char TxIQmismatch_Amp_2G,	
						char TxIQmismatch_Phase_2G,
						char TxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	
						char TxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD]);
//Annex 54
DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex54DataFromFile(char *FileName,
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
						bool *Cal2G,
						bool *Cal5G,
						char *RxIQmismatch_Amp_2G,	char *RxIQmismatch_Phase_2G,
						char RxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	char RxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD]);

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex54DataToFile(char *FileName,
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
						bool Cal2G,
						bool Cal5G,
						char RxIQmismatch_Amp_2G,	char RxIQmismatch_Phase_2G,
						char RxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	char RxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD]);

DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType54Data( 
#if defined (_MIMO_)
					   int DeviceId,		int PathId,   
#endif //#if defined (_MIMO_)
						bool  *Cal_2G,
						bool  *Cal_5G,
						char *RxIQmismatch_Amp_2G,	
						char *RxIQmismatch_Phase_2G,
						char RxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	
						char RxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD]);

DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType54Data( 
#if defined (_MIMO_)
					   int DeviceId,		int PathId,   
#endif //#if defined (_MIMO_)
						bool  Cal_2G,
						bool  Cal_5G,
						char RxIQmismatch_Amp_2G,	
						char RxIQmismatch_Phase_2G,
						char RxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	
						char RxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD]);

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
					BYTE *Encr_Key_Len_Min);

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
					BYTE Encr_Key_Len_Min);

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
					BYTE BDAddr[6],
					BYTE *Encr_Key_Len_Max,
					BYTE *Encr_Key_Len_Min
					);
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
					DWORD		BTBAUDRate,
					BYTE BDAddr[6],
					BYTE Encr_Key_Len_Max,
					BYTE Encr_Key_Len_Min
					);

//Annex 56
DUT_SHARED_API int STDCALL  Dut_Shared_GetAnnexType56Data(
 					BYTE Lincense_Key[16],
	                WORD *Link_Key_Mask,
	                BYTE Key_Value[3][22]);

DUT_SHARED_API int STDCALL  Dut_Shared_SetAnnexType56Data(
 					BYTE Lincense_Key[16],
	                WORD Link_Key_Mask,
	                BYTE Key_Value[3][22]);

DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex56DataFromFile(char *FileName,
 					BYTE Lincense_Key[16],
	                WORD *Link_Key_Mask,
	                BYTE Key_Value[3][22]);

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex56DataToFile(char *FileName,
 					BYTE Lincense_Key[16],
	                WORD Link_Key_Mask,
	                BYTE Key_Value[3][22]);

//Annex57
DUT_SHARED_API int STDCALL Dut_Shared_ReadAnnex57DataFromFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					BYTE Band,
					BYTE SubBand,
					BYTE *MEM_ROW,
					BYTE *NumOfEnterirs,
					DPD_ConeffcicentData *Coneffcicent_Data);

DUT_SHARED_API int STDCALL Dut_Shared_WriteAnnex57DataToFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					BYTE Band,
					BYTE SubBand,
					BYTE MEM_ROW,
					BYTE NumOfEnterirs,
					DPD_ConeffcicentData *Coneffcicent_Data);

DUT_SHARED_API int STDCALL Dut_Shared_GetAnnexType57Data( 
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
					BYTE Band,
					BYTE SubBand,
					BYTE *MEM_ROW,
					BYTE *NumOfEnterirs,
					DPD_ConeffcicentData *Coneffcicent_Data);


DUT_SHARED_API int STDCALL Dut_Shared_SetAnnexType57Data(
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
					BYTE Band,
					BYTE SubBand,
					BYTE MEM_ROW,
					BYTE NumOfEnterirs,
					DPD_ConeffcicentData *Coneffcicent_Data);



DUT_SHARED_API int STDCALL Dut_Shared_PurgeCalPointers(void);

DUT_SHARED_API int STDCALL DutIf_GetHeaderTbl( DWORD HeaderTbl[], 
											   DWORD *Version, 
											   BOOL *CsC);
