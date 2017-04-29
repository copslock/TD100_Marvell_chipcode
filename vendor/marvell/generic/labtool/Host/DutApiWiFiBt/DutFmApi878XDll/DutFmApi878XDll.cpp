/** @file DutFmApi878XDll.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

/*
#include "../../DutCommon/Clss_os.h"
#include "../../DutCommon/utilities.h" 
//#include DUTCLASS_SHARED_STDH_PATH	// "../DutShareApi878XDll/stdafx.h"

//#include DUTDLL_BT_H_PATH		// "../DutBtApi878XDll.h"
#include DUTCLASS_BT_H_PATH		// "DutBtApiClss.h"
#include DUTCLASS_SHARED_H_PATH		// "../DutShareApi878XDll/DutSharedClss.h"

//#ifdef _HCI_PROTOCAL_
//#include GRUCLASS_BT_H_PATH		// "../GruBtApi86xxDll/GruBtApiClss.h"
//#endif //#ifdef _HCI_PROTOCAL_
#include "../DutBtApi878XDll/DutBtApiClss.h"
*/

#include "../DutFmApi878XDll.h"
//#include DUTDLL_BT_H_PATH	


DUT_FM_API int STDCALL Dut_Fm_Version()
{
	return Dut_Bt_Version();
}


DUT_FM_API char* STDCALL Dut_Fm_About()
{
	return Dut_Bt_About();
}


DUT_FM_API int STDCALL Dut_Fm_OpenDevice(void **theObj)
{
		return Dut_Bt_OpenDevice(theObj);
}

DUT_FM_API int STDCALL Dut_Fm_CloseDevice(void **theObj)
{
	return Dut_Bt_CloseDevice(theObj);
}

#if !defined (_NO_EEPROM_)
DUT_FM_API int STDCALL Dut_Fm_FlexSpiDL(bool erase, char *fileName)
{
   return Dut_Bt_FlexSpiDL(erase, fileName);
}

DUT_FM_API int STDCALL Dut_Fm_FlexSpiCompare(char *fileName)  
{
   return Dut_Bt_FlexSpiCompare(fileName); 
}

DUT_FM_API int STDCALL Dut_Fm_CompareE2PData(
					DWORD  *RefArray,int SizeInByte)
{
	return Dut_Bt_CompareE2PData(RefArray, SizeInByte);
}
#endif //#if !defined (_NO_EEPROM_)

 
#if  !defined (_NO_EFUSE_)
DUT_FM_API int STDCALL Dut_Fm_SetEfuseData(int Block, int LenInWord, 
										   WORD *pData)
{
	return Dut_Bt_SetEfuseData(Block, LenInWord, pData);
}
DUT_FM_API int STDCALL Dut_Fm_GetEfuseData(int Block, int LenInWord, 
										   WORD *pData)
{
	return Dut_Bt_GetEfuseData(Block, LenInWord, pData);
}
#endif // #if !defined (_NO_EFUSE_)

DUT_FM_API int STDCALL Dut_Fm_GetFwReleaseNote(char *pReleaseNote)
{
	return Dut_Bt_GetFwReleaseNote(pReleaseNote);
}

#if !defined (_NO_EEPROM_)
DUT_FM_API int STDCALL Dut_Fm_ForceE2PromType(DWORD		IfType, 
												DWORD		AddrWidth,
												DWORD		DeviceType)
{
	return Dut_Bt_ForceE2PromType(IfType,  AddrWidth, DeviceType);
}
DUT_FM_API int STDCALL Dut_Fm_GetE2PromType(	DWORD		*pIfType, 
						DWORD		*pAddrWidth,
						DWORD		*pDeviceType) 
{
	return Dut_Bt_GetE2PromType(pIfType, pAddrWidth, pDeviceType);
}

DUT_FM_API int STDCALL Dut_Fm_SpiWriteDword(DWORD Address, DWORD data)
{
	return Dut_Bt_SpiWriteDword(Address, data);
}

DUT_FM_API int STDCALL Dut_Fm_SpiReadDword(DWORD Address, DWORD *data)
{
	return Dut_Bt_SpiReadDword(Address, data);
}

DUT_FM_API int STDCALL Dut_Fm_DumpE2PData(DWORD StartAddress, DWORD LenInDw,
									DWORD *pAddressArray, 
									DWORD *pDataArray)
{
	return Dut_Bt_DumpE2PData(	StartAddress, LenInDw,
			  					pAddressArray, 
			 					pDataArray);
}

DUT_FM_API int STDCALL Dut_Fm_WriteLumpedData(DWORD StartAddress, 
								   DWORD LenInDw, 
									DWORD *pDataArray)
{
	return Dut_Bt_WriteLumpedData(	StartAddress, 
									LenInDw, 
									pDataArray);
}
#endif // #if !defined (_NO_EEPROM_)

DUT_FM_API int STDCALL Dut_Fm_GetCurrentORVersion(int *pSocVersion, int *pRfVersion)
{

	return Dut_Bt_GetCurrentORVersion(pSocVersion, pRfVersion);
}
DUT_FM_API int STDCALL Dut_Fm_GetCurrentAppMode(int *pAppMode)
{
	return Dut_Bt_GetCurrentAppMode(pAppMode);
}

DUT_FM_API int STDCALL Dut_Fm_SetXtal(BYTE ExtMode, BYTE XtalValue)		
// 0 internal mode 1: external mode
{
	return Dut_Bt_SetBtXtal(ExtMode, XtalValue);		
}

DUT_FM_API int STDCALL Dut_Fm_GetXtal(BYTE *pExtMode, BYTE *pXtalValue)
{
	return Dut_Bt_GetBtXtal(pExtMode, pXtalValue);
}



#if defined (_FM_)

DUT_FM_API int STDCALL Dut_Fm_WriteMem(unsigned long address, unsigned long data)
{
	return BT_OBJ WriteMemDword(address, data);
}


DUT_FM_API int STDCALL Dut_Fm_ReadMem(unsigned long address, unsigned long *data)
{
	return BT_OBJ ReadMemDword(address, data);
}


DUT_FM_API int STDCALL Dut_Fm_WriteFmuRegister(DWORD address, DWORD data) 
{
	return BT_OBJ WriteFmuReg( address,  data);
}
DUT_FM_API int STDCALL Dut_Fm_ReadFmuRegister(DWORD address, DWORD *data) 
{
	return BT_OBJ ReadFmuReg(address, data);
}

// TBRM DUT_FM_API int STDCALL Dut_Fm_FmReset(void)
// TBRM {
// TBRM 	return BT_OBJ FmReset();
// TBRM }

DUT_FM_API int STDCALL Dut_Fm_FmInitialization(int XtalFreqInKHz)//, int FreqErrPpm)
{
	return BT_OBJ FmInitialization(XtalFreqInKHz);//, FreqErrPpm);
}

DUT_FM_API int STDCALL Dut_Fm_FmPowerUpMode(int PowerMode)
{
	return BT_OBJ FmPowerUpMode(PowerMode);
}

DUT_FM_API int STDCALL Dut_Fm_GetFmCurrentRssi(DWORD *pCurrentRssi)
{
	return BT_OBJ GetFmCurrentRssi(pCurrentRssi);
}

DUT_FM_API int STDCALL Dut_Fm_GetFmCurrentCmi(DWORD *pCurrentCmi)
{
	return BT_OBJ GetFmCurrentCmi(pCurrentCmi);
}


DUT_FM_API int STDCALL Dut_Fm_SetFmChannel(DWORD *pFmChInKHz)
{
	return BT_OBJ SetFmChannel(pFmChInKHz);
}

DUT_FM_API int STDCALL Dut_Fm_GetFmChannel(DWORD *pFmChInKHz)
{
	return BT_OBJ GetFmChannel(pFmChInKHz);
}

DUT_FM_API int STDCALL Dut_Fm_SetFmAfChannel(DWORD *pFmAfChInKHz, BYTE AfMode)
{
	return BT_OBJ SetFmAfChannel(pFmAfChInKHz, AfMode);
}

//DUT_FM_API int STDCALL Dut_Fm_GetFmAfChannel(DWORD *pFmAfChInKHz)
//{
//	return BT_OBJ GetFmAfChannel(pFmAfChInKHz);
//}

DUT_FM_API int STDCALL Dut_Fm_SetFmSearchDir(BYTE FmSearchDir_F0B1)
{
	return BT_OBJ SetFmSearchDir(FmSearchDir_F0B1);
}

DUT_FM_API int STDCALL Dut_Fm_GetFmSearchDir(DWORD *pFmSearchDir_F0B1)
{
	return BT_OBJ GetFmSearchDir(pFmSearchDir_F0B1);
}

DUT_FM_API int STDCALL Dut_Fm_SetFmAutoSearchMode(BYTE FmAutoSearchMode)
{
	return BT_OBJ SetFmAutoSearchMode(FmAutoSearchMode);
}

DUT_FM_API int STDCALL Dut_Fm_GetFmAutoSearchMode(DWORD *pFmAutoSearchMode)
{
	return BT_OBJ GetFmAutoSearchMode(pFmAutoSearchMode);
}

DUT_FM_API int STDCALL Dut_Fm_SetFmRxForceStereoMode(BYTE FmForceStereoMode)
{
	return BT_OBJ SetFmRxForceStereoMode(FmForceStereoMode);
}

DUT_FM_API int STDCALL Dut_Fm_SetFmRxForceMonoMode(BYTE FmForceMonoMode) 
{
	return BT_OBJ SetFmRxForceMonoMode(FmForceMonoMode);
}

DUT_FM_API int STDCALL Dut_Fm_GetFmRxForceMonoMode(DWORD *pFmForceMonoMode)
{
	return BT_OBJ GetFmRxForceMonoMode(pFmForceMonoMode);
}

DUT_FM_API int STDCALL Dut_Fm_SetFmRssiThreshold(DWORD FmRssiThreshold)
{
	return BT_OBJ SetFmRssiThreshold(FmRssiThreshold);
}

DUT_FM_API int STDCALL Dut_Fm_GetFmRssiThreshold(DWORD *pFmRssiThreshold)
{
	return BT_OBJ GetFmRssiThreshold(pFmRssiThreshold);
}

DUT_FM_API int STDCALL Dut_Fm_SetFmCmiThreshold(DWORD FmCmiThreshold)
{
	return BT_OBJ SetFmCmiThreshold(FmCmiThreshold);
}

DUT_FM_API int STDCALL Dut_Fm_GetFmCmiThreshold(DWORD *pFmCmiThreshold)
{
	return BT_OBJ GetFmCmiThreshold(pFmCmiThreshold);
}

DUT_FM_API int STDCALL Dut_Fm_SetFmBand(BYTE FmBand)
{
	return BT_OBJ SetFmBand(FmBand);
}

// TBRM DUT_FM_API int STDCALL Dut_Fm_GetFmBand(DWORD *pFmBand)
// TBRM {
// TBRM 	return BT_OBJ GetFmBand(pFmBand);
// TBRM }

DUT_FM_API int STDCALL Dut_Fm_SetFmChStepSizeKHz(DWORD StepSizeKHz)
{
	return BT_OBJ SetFmChStepSizeKHz(StepSizeKHz);
}

// TBRM DUT_FM_API int STDCALL Dut_Fm_GetFmChStepSizeKHz(DWORD *pStepSizeKHz)
// TBRM {
// TBRM 	return BT_OBJ GetFmChStepSizeKHz(pStepSizeKHz);
// TBRM }

DUT_FM_API int STDCALL Dut_Fm_FmMoveChUp(DWORD *pFmChInKHz)
{
	return BT_OBJ FmMoveChUp(pFmChInKHz);
}

DUT_FM_API int STDCALL Dut_Fm_FmMoveChDown(DWORD *pFmChInKHz)
{
	return BT_OBJ FmMoveChDown(pFmChInKHz);
}

DUT_FM_API int STDCALL Dut_Fm_GetFmFwVersion(DWORD *pFwVersion)
{
	return BT_OBJ GetFmFwVersion(pFwVersion);
}
 
DUT_FM_API int STDCALL Dut_Fm_GetFmFwVersionStr(char *pFwVersion)
{
	return BT_OBJ Bt_GetFwVersion(NULL, NULL, pFwVersion);
}
 
DUT_FM_API int STDCALL Dut_Fm_GetFmHwVersion(DWORD *pHwVersion)
{
	return BT_OBJ GetFmHwVersion(pHwVersion);
}

DUT_FM_API int STDCALL Dut_Fm_GetFmHwId(DWORD *pHwId)
{
	return BT_OBJ GetFmHwId(pHwId);
}

DUT_FM_API int STDCALL Dut_Fm_GetFmManId(DWORD *pVId)
{
	return BT_OBJ GetFmManId(pVId);
}


/////////////////////////////////////////////////

DUT_FM_API int STDCALL Dut_Fm_GetFmRdsBLER(int *pSeqGood, int *pSeqNoSync, 
									int *pExpected, int *pSuccRate)
{
	return BT_OBJ GetFmRdsBLER(pSeqGood,pSeqNoSync, 
							pExpected, pSuccRate);
}
DUT_FM_API int STDCALL Dut_Fm_ResetFmRdsBLER(int SecPeriod)
{
	return BT_OBJ ResetFmRdsBLER(SecPeriod);
}

/*
DUT_FM_API int STDCALL Dut_Fm_SetFmReferenceClk(int CalFreqInKHz)
{
	return BT_OBJ SetFmReferenceClk(CalFreqInKHz);
}
*/

DUT_FM_API int STDCALL Dut_Fm_SetFmReferenceClkError(int FreqErrPpm)
{
	return BT_OBJ SetFmReferenceClkError(FreqErrPpm);
}
 
DUT_FM_API int STDCALL Dut_Fm_SelectFmRxAntennaType(int AntType)
{
	return BT_OBJ SelectFmRxAntennaType(AntType);	// 0: 75Ohm, 1: TxLoop  
}

DUT_FM_API int STDCALL Dut_Fm_SetFmIrsMask(int RssiLow,	int NewRdsData,	int RssiIndicator,	int BandLimit, 
				 int SyncLost,	int SearchStop, int BMatch,			int PiMatched,
				 int MonoStereoChanged,	int AudioPause, int CmiLow)
{
	return BT_OBJ SetFmIrsMask(RssiLow,	NewRdsData,	RssiIndicator,	BandLimit, 
				 SyncLost,	SearchStop, BMatch,			PiMatched,
				 MonoStereoChanged,	AudioPause, CmiLow);
}

DUT_FM_API int STDCALL Dut_Fm_GetFmIrsMask(int *pRssiLow,		int *pNewRdsData,	int *pRssiIndicator,	int *pBandLimit, 
				 int *pSyncLost,	int *pSearchStop,	int *pBMatch,			int *pPiMatched,
				 int *pMonoStereoChanged,	int *pAudioPause, int *pCmiLow)
{
	return BT_OBJ GetFmIrsMask(	pRssiLow,		pNewRdsData,	pRssiIndicator,	pBandLimit, 
								pSyncLost,		pSearchStop,	pBMatch,			pPiMatched,
								pMonoStereoChanged,	pAudioPause, pCmiLow);	
}

DUT_FM_API int STDCALL Dut_Fm_GetFmCurrentFlags(DWORD *pFlags)
{
	return BT_OBJ GetFmCurrentFlags(pFlags);	
}

DUT_FM_API int STDCALL Dut_Fm_FmPowerDown(void)
{
	return BT_OBJ FmPowerDown();
}

DUT_FM_API int STDCALL Dut_Fm_StopFmSearch(void)
{
	return BT_OBJ StopFmSearch();
}

/*
//	LoInjectionSide
//	{
//	LOINJECTION_HIGHSIDE,	//Flo=Frx+Fif, 
//	LOINJECTION_LOWSIDE,	//Flo=Frx-Fif, 
//	LOINJECTION_AUTO 
//	}

DUT_FM_API int STDCALL Dut_Fm_SetFmLoInjectionSide(int mode)
{
	return BT_OBJ SetFmLoInjectionSide(mode);
}
*/
//	emphasis
//	{
//	EMPHASIS_NONE, EMPHASIS_50US, EMPHASIS_75US
//	}
DUT_FM_API int STDCALL Dut_Fm_SetFmAudioEmphasis(int Mode)
{
	return BT_OBJ SetFmAudioEmphasis(Mode);	
}

// TBRM DUT_FM_API int STDCALL Dut_Fm_GetFmRxAudioDeemphasis(int *pMode)
// TBRM {
// TBRM 	return BT_OBJ GetFmRxAudioDeemphasis(pMode);
// TBRM }

//	MuteMode
// {
//	MUTEMODE_ALL_MUTE_OFF,		
//	MUTEMODE_AC_MUTE_ON_L_R,	 
//	MUTEMODE_HARD_MUTE_LEFT_ON,
//  MUTEMODE_HARD_MUTE_LEFT_AC_MUTE_RIGHT,	 
//	MUTEMODE_HARD_MUTE_RIGHT_ON,
//	MUTEMODE_HARD_MUTE_RIGHT_AC_MUTE_LEFT,			
//	MUTEMODE_HARD_MUTE_ON_L_R
// }
DUT_FM_API int STDCALL Dut_Fm_SetFmRxMuteMode(int MuteMode)
{
	return BT_OBJ SetFmRxMuteMode(MuteMode);
}

DUT_FM_API int STDCALL Dut_Fm_GetFmRxMuteMode(int *pMuteMode)
{
	return BT_OBJ GetFmRxMuteMode(pMuteMode);	
}

//	AudioPath
//	{
//	AUDIO_FM,  I2S_FM, I2S_AIU, SQU, SPDIF
//	}
//	I2sOperation
//	{
//  SLAVE, MASTER
//	}
//	I2sMode
//	{
//	I2S, MSB, LSB, PCM
//	}
DUT_FM_API int STDCALL Dut_Fm_SetRxAudioPath(BYTE AudioPath,		
											 BYTE I2sOperation,		
											 BYTE I2sMode)
{
	return BT_OBJ SetRxAudioPath(AudioPath,	I2sOperation, I2sMode);	
}

DUT_FM_API int STDCALL Dut_Fm_GetRxAudioPath(BYTE *pAudioPath)
{
	return BT_OBJ GetRxAudioPath(pAudioPath);	
}

//	Sampling
//	{
//	SAMP_8K,	SAMP_11D025K,	SAMP_12K,	SAMP_16K,	SAMP_22D05K, 
//	SAMP_24K,	SAMP_32K,		SAMP_44D1K, SAMP_48K
//	}

//	BClk_LrClk
//	{
//	CLK_32X,	CLK_50X,	CLK_64X
//	}
DUT_FM_API int STDCALL Dut_Fm_SetFmRxAudioSamplingRate(int Sampling, int BClk_LrClk)
{
	return BT_OBJ SetFmRxAudioSamplingRate(Sampling, BClk_LrClk);	
}
   
//	PauseMode
//	{
//	PAUSEMODE_OFF, PAUSEMODE_PAUSESTARTONLY, PAUSEMODE_PAUSESTOPONLY, PAUSEMODE_STARTSTOP
//	}
DUT_FM_API int STDCALL Dut_Fm_EnableFmAudioPauseMode(int PauseMode)
{
	return BT_OBJ EnableFmAudioPauseMode(PauseMode);	
}

DUT_FM_API int STDCALL Dut_Fm_SetFmAudioPauseDuration(WORD PauseDurInMs)
{
	return BT_OBJ SetFmAudioPauseDuration(PauseDurInMs);	
}

/* // TBRM
DUT_FM_API int STDCALL Dut_Fm_GetFmAudioPauseDuration(WORD *pPauseDurInMs)
{
	return BT_OBJ GetFmAudioPauseDuration(pPauseDurInMs);	
}
// TBRM */
DUT_FM_API int STDCALL Dut_Fm_SetFmAudioPauseLevel(WORD PauseLevel)
{
	return BT_OBJ SetFmAudioPauseLevel(PauseLevel);	
}
/* // TBRM
DUT_FM_API int STDCALL Dut_Fm_GetFmAudioPauseLevel(WORD *pPauseLevel)
{
	return BT_OBJ GetFmAudioPauseLevel(pPauseLevel);	
}
// TBRM */
/*
//	RdsDataPath
//	{
//	RDSDATAPATH_2NOWHERE, RDSDATAPATH_2HCI, RDSDATAPATH_2I2C, RDSDATAPATH_2HFP, RDSDATAPATH_2A2DP	
//	}
DUT_FM_API int STDCALL Dut_Fm_SetFmRxRdsDataPath(int Path)
{
	return BT_OBJ SetFmRxRdsDataPath(Path);	
}

DUT_FM_API int STDCALL Dut_Fm_GetFmRxRdsDataPath(int *pPath)
{
	return BT_OBJ GetFmRxRdsDataPath(pPath);	
}
*/

DUT_FM_API int STDCALL Dut_Fm_GetFmRxRdsSyncStatus(BYTE *pSyncStatus)
{
	return BT_OBJ GetFmRxRdsSyncStatus(pSyncStatus);	
}

DUT_FM_API int STDCALL Dut_Fm_FmRxRdsFlush(void)
{
	return BT_OBJ FmRxRdsFlush();	
}

DUT_FM_API int STDCALL Dut_Fm_SetFmRxRdsMemDepth(BYTE Depth)
{
	return BT_OBJ SetFmRxRdsMemDepth(Depth);	
}

DUT_FM_API int STDCALL Dut_Fm_GetFmRxRdsMemDepth(BYTE *pDepth)
{
	return BT_OBJ GetFmRxRdsMemDepth(pDepth);	
}


DUT_FM_API int STDCALL Dut_Fm_SetFmRxRdsBlockB(WORD BlockB)
{
	return BT_OBJ SetFmRxRdsBlockB(BlockB);	
}

DUT_FM_API int STDCALL Dut_Fm_GetFmRxRdsBlockB(WORD *pBlockB)
{
	return BT_OBJ GetFmRxRdsBlockB(pBlockB);	
}

DUT_FM_API int STDCALL Dut_Fm_SetFmRxRdsBlockBMask(WORD BlockBMask)
{
	return BT_OBJ SetFmRxRdsBlockBMask(BlockBMask);	
}

DUT_FM_API int STDCALL Dut_Fm_GetFmRxRdsBlockBMask(WORD *pBlockBMask)
{
	return BT_OBJ GetFmRxRdsBlockBMask(pBlockBMask);	
}

DUT_FM_API int STDCALL Dut_Fm_SetFmRxRdsPiCode(WORD PiCode)
{
	return BT_OBJ SetFmRxRdsPiCode(PiCode);	
}

DUT_FM_API int STDCALL Dut_Fm_GetFmRxRdsPiCode(WORD *pPiCode)
{
	return BT_OBJ GetFmRxRdsPiCode(pPiCode);	
}

DUT_FM_API int STDCALL Dut_Fm_SetFmRxRdsPiCodeMask(WORD PiCodeMask)
{
	return BT_OBJ SetFmRxRdsPiCodeMask(PiCodeMask);	
}

DUT_FM_API int STDCALL Dut_Fm_GetFmRxRdsPiCodeMask(WORD *pPiCodeMask)
{
	return BT_OBJ GetFmRxRdsPiCodeMask(pPiCodeMask);
}

//	RdsMode
//	{
//	RDSMODE_RDS_STD, RDSMODE_RBDSWOE
//	}
DUT_FM_API int STDCALL Dut_Fm_SetFmRxRdsMode(int RdsMode)
{
	return BT_OBJ SetFmRxRdsMode(RdsMode);	
}

DUT_FM_API int STDCALL Dut_Fm_GetFmRxRdsMode(int *pRdsMode)
{
	return BT_OBJ GetFmRxRdsMode(pRdsMode);	
}

DUT_FM_API int STDCALL Dut_Fm_GetFmTxOutputPwr(BYTE *pTxPwr)
{
	return BT_OBJ GetFmTxOutputPwr(pTxPwr);
}

DUT_FM_API int STDCALL Dut_Fm_SetFmTxOutputPwr(BYTE TxPwr)
{
	return BT_OBJ SetFmTxOutputPwr(TxPwr);	
}

// TBRM //	emphasis
// TBRM //	{
// TBRM //	EMPHASIS_NONE, EMPHASIS_50US, EMPHASIS_75US
// TBRM //	}
// TBRM DUT_FM_API int STDCALL Dut_Fm_SetFmAudioPreemphasis(int Mode)
// TBRM {
// TBRM 	return BT_OBJ SetFmAudioPreemphasis(Mode);
// TBRM }

DUT_FM_API int STDCALL Dut_Fm_SetFmFreqDeviation(DWORD FreqDeviaInHz)
{
	return BT_OBJ SetFmFreqDeviation(FreqDeviaInHz);
}

// TBRM DUT_FM_API int STDCALL Dut_Fm_SearchFmTxFreeCh(DWORD StartChFreqInKhz)
// TBRM {
// TBRM 	return BT_OBJ SearchFmTxFreeCh(StartChFreqInKhz);	
// TBRM }

DUT_FM_API int STDCALL Dut_Fm_SetFmTxMonoStereoMode(BYTE FmMonoStereoMode)
{
	return BT_OBJ SetFmTxMonoStereoMode(FmMonoStereoMode);		
}

 
//	MuteMode
// {
//  OFF, ON
// }
DUT_FM_API int STDCALL Dut_Fm_SetFmTxMuteMode(int MuteMode)
{
	return BT_OBJ SetFmTxMuteMode(MuteMode);
}

//	InputGain
//	{
//	INPUTGAIN_2D5,	INPUTGAIN_1D25, INPUTGAIN_0D833
//	}
//	ActiveAudioCtrl
//	{
//	ACTAUDIOCTRL_RONLY,	ACTAUDIOCTRL_LONLY,	ACTAUDIOCTRL_STEREO  
//	}
DUT_FM_API int STDCALL Dut_Fm_ConfigFmAudioInput(int InputGain, int ActiveAudioCtrl)
{
	return BT_OBJ ConfigFmAudioInput(InputGain, ActiveAudioCtrl);
}

DUT_FM_API int STDCALL Dut_Fm_EnableFmTxAudioAGC(int Mode_OffOn)
{
	return BT_OBJ EnableFmTxAudioAGC(Mode_OffOn);
}

DUT_FM_API int STDCALL Dut_Fm_SetFmFreqDeviationExtended(DWORD LprDev,  DWORD LmrDev, DWORD PilotDev, DWORD RdsDev)
{
	return BT_OBJ SetFmFreqDeviationExtended(LprDev, LmrDev, PilotDev, RdsDev);
}

// TBRM DUT_FM_API int STDCALL Dut_Fm_GetFmTxPowerMode(int *pMode_HighLow)
// TBRM {
// TBRM 	return BT_OBJ GetFmTxPowerMode(pMode_HighLow);
// TBRM }

DUT_FM_API int STDCALL Dut_Fm_SetFmTxPowerMode(int Mode_HighLow)
{
	return BT_OBJ SetFmTxPowerMode(Mode_HighLow);
}

/*
DUT_FM_API int STDCALL Dut_Fm_ForceFmTxPilotGain(int Mode_OffOn, BYTE Gain)
{
	return BT_OBJ ForceFmTxPilotGain(Mode_OffOn, Gain);
}

DUT_FM_API int STDCALL Dut_Fm_ForceFmTxRdsGain(int Mode_OffOn, BYTE Gain)
{
	return BT_OBJ ForceFmTxRdsGain(Mode_OffOn, Gain);
}
*/
DUT_FM_API int STDCALL Dut_Fm_SetFmTxRdsPiCode(WORD PiCode)
{
	return BT_OBJ SetFmTxRdsPiCode(PiCode);
}

//	RdsGroup
//	{
//	RDSGROUP_PS0A, RDSGROUP_PS0B, RDSGROUP_RT2A, RDSGROUP_RT2B
//	}
DUT_FM_API int STDCALL Dut_Fm_SetFmTxRdsGroup(int RdsGroup)
{
	return BT_OBJ SetFmTxRdsGroup(RdsGroup);
}

DUT_FM_API int STDCALL Dut_Fm_SetFmTxRdsPty(BYTE RdsPty)
{
	return BT_OBJ SetFmTxRdsPty(RdsPty);	
}

DUT_FM_API int STDCALL Dut_Fm_SetFmTxRdsAfCode(BYTE AfCode)
{
	return BT_OBJ SetFmTxRdsAfCode(AfCode);	
}

//	RdsTxMode
//	{
//	RDSTXMODE_PS_RT, RDSTXMODE_RAW
//	}
DUT_FM_API int STDCALL Dut_Fm_SetFmTxRdsMode(int RdsTxMode)
{
	return BT_OBJ SetFmTxRdsMode(RdsTxMode);
}

DUT_FM_API int STDCALL Dut_Fm_SetFmTxRdsScrolling(int Mode_OffOn)
{
	return BT_OBJ SetFmTxRdsScrolling(Mode_OffOn);
}

DUT_FM_API int STDCALL Dut_Fm_SetFmTxRdsScrollingRate(BYTE Rate_CharPerScroll)
{
	return BT_OBJ SetFmTxRdsScrollingRate(Rate_CharPerScroll);
}

DUT_FM_API int STDCALL Dut_Fm_SetFmTxRdsDisplayLength(BYTE Len_Chars)
{
	return BT_OBJ SetFmTxRdsDisplayLength(Len_Chars);
}

DUT_FM_API int STDCALL Dut_Fm_SetFmTxRdsToggleAB(void)
{
	return BT_OBJ SetFmTxRdsToggleAB();	
}

//	Repository
//	{
//	REPOSITORY_G0, REPOSITORY_G1,REPOSITORY_G2
//	}
DUT_FM_API int STDCALL Dut_Fm_SetFmTxRdsRepository(int Repository)
{
	return BT_OBJ SetFmTxRdsRepository(Repository);
}

DUT_FM_API int STDCALL Dut_Fm_GetFmSoftMute(int *pEnable)
{
	return BT_OBJ GetFmSoftMute(pEnable);
}

DUT_FM_API int STDCALL Dut_Fm_SetFmSoftMute(int Enable, WORD MuteThreshold, BYTE AudioAttenuation)
{
	return BT_OBJ SetFmSoftMute(Enable, MuteThreshold, AudioAttenuation);
}

DUT_FM_API int STDCALL Dut_Fm_GetFmStereoBlendingMode(int *pBlendingMode)
{
	return BT_OBJ GetFmStereoBlendingMode(pBlendingMode);
}
DUT_FM_API int STDCALL Dut_Fm_SetFmStereoBlendingMode(int BlendingMode)
{
	return BT_OBJ SetFmStereoBlendingMode(BlendingMode);
}

DUT_FM_API int STDCALL Dut_Fm_GetPllLockStatus(int *pLocked)
{
	return BT_OBJ GetPllLockStatus(pLocked);
}


DUT_FM_API int STDCALL Dut_Fm_SetFmStereoBlendingConfigration(int TimeConstant)
{
	return BT_OBJ SetFmStereoBlendingConfigration(TimeConstant);
}

DUT_FM_API int STDCALL Dut_Fm_GetFmStereoStatus(int *pStatus)
{
	return BT_OBJ GetFmStereoStatus(pStatus);
}
//	SamplingRate
//	{
//	SAMP_8K,	SAMP_11D025K,	SAMP_12K,	SAMP_16K,	SAMP_22D05K, 
//	SAMP_24K,	SAMP_32K,		SAMP_44D1K, SAMP_48K
//	}

//	BClk_LrClk
//	{
//	CLK_32X,	CLK_50X,	CLK_64X
//	}
DUT_FM_API int STDCALL Dut_Fm_GetFmI2SParameter(int ChFreqInKHz, int *pSamplingRate, int *pBClk_LrClk)
{
	return BT_OBJ GetFmI2SParameter(ChFreqInKHz, pSamplingRate, pBClk_LrClk);
}

DUT_FM_API int STDCALL Dut_Fm_GetFmAudioVolume(int *pVolume)
{
	return BT_OBJ GetFmAudioVolume(pVolume);
}

DUT_FM_API int STDCALL Dut_Fm_SetFmAudioVolume(int Volume)
{
	return BT_OBJ SetFmAudioVolume(Volume);
}
#endif // #if defined (_FM_)
