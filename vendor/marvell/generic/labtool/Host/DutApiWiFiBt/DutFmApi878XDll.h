/** @file DutFmApi878XDll.h
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
/*
#ifndef _LINUX_FEDORA_
#ifdef DUTAPI8787DLL_EXPORTS
#define DUT_FM_API	extern "C" __declspec(dllexport)
#else // #ifdef DUTAPI8787DLL_EXPORTS
#ifdef _DEBUG
#define DUT_FM_API	extern "C"
#else // #ifdef _DEBUG
#define DUT_FM_API	extern "C" __declspec(dllimport)  
#endif // #ifdef _DEBUG
#endif // #ifdef DUTAPI8787DLL_EXPORTS
#else // #ifndef _LINUX_FEDORA_
#define DUT_FM_API	extern "C"
#endif // #ifndef _LINUX_FEDORA_
*/
#include "DutApi878XDll.h" 

#include "../DutCommon/Clss_os.h"

#include "DutBtApi.hc"
#include "DutWlanApi.hc"
  


DUT_FM_API int STDCALL Dut_Fm_Version();

DUT_FM_API char* STDCALL Dut_Fm_About();

DUT_FM_API int STDCALL Dut_Fm_OpenDevice(void **theObj);

DUT_FM_API int STDCALL Dut_Fm_CloseDevice(void **theObj);


DUT_FM_API int STDCALL Dut_Fm_WriteMem(DWORD address, DWORD data);

DUT_FM_API int STDCALL Dut_Fm_ReadMem(DWORD address, DWORD *data);

#if defined(_W8689_) || defined(_W8787_) || defined(_W8790_) || defined(_W8782_)
DUT_FM_API int STDCALL Dut_Fm_WriteFmuRegister(DWORD address, DWORD data); 
DUT_FM_API int STDCALL Dut_Fm_ReadFmuRegister(DWORD address, DWORD *data); 
#endif //#if defined(_W8689_) || defined(_W8787_) || defined(_W8790_)|| defined(_W8782_)


DUT_FM_API int STDCALL Dut_Fm_GetCurrentORVersion(int *pSocVersion, 
												  int *pRfVersion);
DUT_FM_API int STDCALL Dut_Fm_GetCurrentAppMode(int *pAppMode);
//DUT_FM_API int STDCALL Dut_Fm_GetChipClassModeCapacity(int *pMode);

DUT_FM_API int STDCALL Dut_Fm_SetXtal(BYTE ExtMode, BYTE XtalValue);	
DUT_FM_API int STDCALL Dut_Fm_GetXtal(BYTE *pExtMode, BYTE *pXtalValue);
 
#if !defined (_NO_EEPROM_)
DUT_FM_API int STDCALL Dut_Fm_ForceE2PromType(DWORD		IfType, 
										  DWORD		AddrWidth,
										  DWORD		DeviceType);
DUT_FM_API int STDCALL Dut_Fm_GetE2PromType(DWORD		*pIfType, 
											DWORD		*pAddrWidth,
											DWORD		*pDeviceType);
DUT_FM_API int STDCALL Dut_Fm_SpiWriteDword(DWORD Address, DWORD data);
DUT_FM_API int STDCALL Dut_Fm_SpiReadDword(DWORD Address, DWORD *data);
DUT_FM_API int STDCALL Dut_Fm_DumpE2PData(DWORD StartAddress, DWORD LenInDw,
									DWORD *pAddressArray, 
									DWORD *pDataArray);
DUT_FM_API int STDCALL Dut_Fm_WriteLumpedData(DWORD StartAddress, 
								   DWORD LenInDw, 
									DWORD *pDataArray);

DUT_FM_API int STDCALL Dut_Fm_FlexSpiDL(bool erase, char *fileName);
DUT_FM_API int STDCALL Dut_Fm_FlexSpiCompare(char *fileName);
#endif //#if !defined (_NO_EEPROM_)

#if !defined (_NO_EFUSE_)
DUT_FM_API int STDCALL Dut_Fm_SetEfuseData(int Block, int LenInWord, 
										   WORD *pData);
DUT_FM_API int STDCALL Dut_Fm_GetEfuseData(int Block, int LenInWord, 
										   WORD *pData);
#endif // #if !defined (_NO_EFUSE_)

DUT_FM_API int STDCALL Dut_Fm_GetFwReleaseNote(char *pReleaseNote);
///////////////////////////////////


#if defined(_W8689_) || defined(_W8787_) || defined(_W8790_) || defined(_W8782_)
// TBRM DUT_FM_API int STDCALL Dut_Fm_FmReset(void);
DUT_FM_API int STDCALL Dut_Fm_FmInitialization(int XtalFreqInKHz); //, int FreqErrPpm);
DUT_FM_API int STDCALL Dut_Fm_FmPowerUpMode(int PowerMode);
DUT_FM_API int STDCALL Dut_Fm_GetFmCurrentRssi(DWORD *pCurrentRssi);
DUT_FM_API int STDCALL Dut_Fm_GetFmCurrentCmi(DWORD *pCurrentCmi);

DUT_FM_API int STDCALL Dut_Fm_SetFmChannel(DWORD *pFmChInKHz);
DUT_FM_API int STDCALL Dut_Fm_GetFmChannel(DWORD *pFmChInKHz);
DUT_FM_API int STDCALL Dut_Fm_SetFmAfChannel(DWORD *pFmAfChInKHz, BYTE AfMode=0);
//DUT_FM_API int STDCALL Dut_Fm_GetFmAfChannel(DWORD *pFmAfChInKHz);

DUT_FM_API int STDCALL Dut_Fm_SetFmSearchDir(BYTE FmSearchDir_F0B1);
DUT_FM_API int STDCALL Dut_Fm_GetFmSearchDir(DWORD *pFmSearchDir_F0B1);

DUT_FM_API int STDCALL Dut_Fm_SetFmAutoSearchMode(BYTE FmAutoSearchMode);
DUT_FM_API int STDCALL Dut_Fm_GetFmAutoSearchMode(DWORD *pFmAutoSearchMode);

DUT_FM_API int STDCALL Dut_Fm_SetFmRxForceStereoMode(BYTE FmForceStereoMode);

DUT_FM_API int STDCALL Dut_Fm_SetFmRxForceMonoMode(BYTE FmForceMonoMode);
DUT_FM_API int STDCALL Dut_Fm_GetFmRxForceMonoMode(DWORD *pFmForceMonoMode);
DUT_FM_API int STDCALL Dut_Fm_SetFmRssiThreshold(DWORD FmRssiThreshold);
DUT_FM_API int STDCALL Dut_Fm_GetFmRssiThreshold(DWORD *pFmRssiThreshold);
DUT_FM_API int STDCALL Dut_Fm_SetFmCmiThreshold(DWORD FmCmiThreshold);
DUT_FM_API int STDCALL Dut_Fm_GetFmCmiThreshold(DWORD *pFmCmiThreshold);

DUT_FM_API int STDCALL Dut_Fm_SetFmBand(BYTE FmBand);
// TBRM DUT_FM_API int STDCALL Dut_Fm_GetFmBand(DWORD *pFmBand);

DUT_FM_API int STDCALL Dut_Fm_SetFmChStepSizeKHz(DWORD StepSizeKHz);
// TBRM DUT_FM_API int STDCALL Dut_Fm_GetFmChStepSizeKHz(DWORD *pStepSizeKHz);

DUT_FM_API int STDCALL Dut_Fm_FmMoveChUp(DWORD *pFmChInKHz);
DUT_FM_API int STDCALL Dut_Fm_FmMoveChDown(DWORD *pFmChInKHz);

DUT_FM_API int STDCALL Dut_Fm_GetFmFwVersion(DWORD *pFwVersion);
DUT_FM_API int STDCALL Dut_Fm_GetFmFwVersionStr(char *pFwVersion);
DUT_FM_API int STDCALL Dut_Fm_GetFmHwVersion(DWORD *pHwVersion);
DUT_FM_API int STDCALL Dut_Fm_GetFmHwId(DWORD *pHwId);
DUT_FM_API int STDCALL Dut_Fm_GetFmManId(DWORD *pVId);




/////////////////////////////////////////////////

DUT_FM_API int STDCALL Dut_Fm_GetFmRdsBLER(int *pSeqGood, int *pSeqNoSync, 
									int *pExpected, int *pSuccRate);
DUT_FM_API int STDCALL Dut_Fm_ResetFmRdsBLER(int SecPeriod);

//DUT_FM_API int STDCALL Dut_Fm_SetFmReferenceClk(int CalFreqInKHz);

DUT_FM_API int STDCALL Dut_Fm_SetFmReferenceClkError(int FreqErrPpm);

 // 0: 75Ohm, 1: TxLoop  
DUT_FM_API int STDCALL Dut_Fm_SelectFmRxAntennaType(int AntType);

DUT_FM_API int STDCALL Dut_Fm_SetFmIrsMask(int RssiLow,	int NewRdsData,	int RssiIndicator,	int BandLimit, 
				 int SyncLost,	int SearchStop, int BMatch,			int PiMatched,
				 int MonoStereoChanged,	int AudioPause, int CmiLow);

DUT_FM_API int STDCALL Dut_Fm_GetFmIrsMask(int *pRssiLow,		int *pNewRdsData,	int *pRssiIndicator,	int *pBandLimit, 
				 int *pSyncLost,	int *pSearchStop,	int *pBMatch,			int *pPiMatched,
				 int *pMonoStereoChanged,	int *pAudioPause, int *pCmiLow);

DUT_FM_API int STDCALL Dut_Fm_GetFmCurrentFlags(DWORD *pFlags);

DUT_FM_API int STDCALL Dut_Fm_FmPowerDown(void);


DUT_FM_API int STDCALL Dut_Fm_StopFmSearch(void);

//	LoInjectionSide
//	{
//	LOINJECTION_HIGHSIDE,	//Flo=Frx+Fif, 
//	LOINJECTION_LOWSIDE,	//Flo=Frx-Fif, 
//	LOINJECTION_AUTO 
//	}
//DUT_FM_API int STDCALL Dut_Fm_SetFmLoInjectionSide(int mode);

//	emphasis
//	{
//	EMPHASIS_NONE, EMPHASIS_50US, EMPHASIS_75US
//	}
DUT_FM_API int STDCALL Dut_Fm_SetFmAudioEmphasis(int Mode);

// TBRM DUT_FM_API int STDCALL Dut_Fm_GetFmRxAudioDeemphasis(int *pMode);


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
DUT_FM_API int STDCALL Dut_Fm_SetFmRxMuteMode(int MuteMode);

DUT_FM_API int STDCALL Dut_Fm_GetFmRxMuteMode(int *pMuteMode);



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
											 BYTE I2sMode);

DUT_FM_API int STDCALL Dut_Fm_GetRxAudioPath(BYTE *pAudioPath);

//	Sampling
//	{
//	SAMP_8K,	SAMP_11D025K,	SAMP_12K,	SAMP_16K,	SAMP_22D05K, 
//	SAMP_24K,	SAMP_32K,		SAMP_44D1K, SAMP_48K
//	}

//	BClk_LrClk
//	{
//	CLK_32X,	CLK_50X,	CLK_64X
//	}
DUT_FM_API int STDCALL Dut_Fm_SetFmRxAudioSamplingRate(int Sampling, 
													   int BClk_LrClk);
   
//	PauseMode
//	{
//	PAUSEMODE_OFF, PAUSEMODE_PAUSESTARTONLY, PAUSEMODE_PAUSESTOPONLY, PAUSEMODE_STARTSTOP
//	}
DUT_FM_API int STDCALL Dut_Fm_EnableFmAudioPauseMode(int PauseMode);

DUT_FM_API int STDCALL Dut_Fm_SetFmAudioPauseDuration(WORD PauseDurInMs);

//DUT_FM_API int STDCALL Dut_Fm_GetFmAudioPauseDuration(WORD *pPauseDurInMs);

DUT_FM_API int STDCALL Dut_Fm_SetFmAudioPauseLevel(WORD PauseLevel);

//DUT_FM_API int STDCALL Dut_Fm_GetFmAudioPauseLevel(WORD *pPauseLevel);

//	RdsDataPath
//	{
//	RDSDATAPATH_2NOWHERE, RDSDATAPATH_2HCI, RDSDATAPATH_2I2C, RDSDATAPATH_2HFP, RDSDATAPATH_2A2DP	
//	}
//DUT_FM_API int STDCALL Dut_Fm_SetFmRxRdsDataPath(int Path); 
//
//DUT_FM_API int STDCALL Dut_Fm_GetFmRxRdsDataPath(int *pPath); 

DUT_FM_API int STDCALL Dut_Fm_GetFmRxRdsSyncStatus(BYTE *pSyncStatus); 

DUT_FM_API int STDCALL Dut_Fm_FmRxRdsFlush(void); 

DUT_FM_API int STDCALL Dut_Fm_SetFmRxRdsMemDepth(BYTE Depth); 

DUT_FM_API int STDCALL Dut_Fm_GetFmRxRdsMemDepth(BYTE *pDepth);

DUT_FM_API int STDCALL Dut_Fm_SetFmRxRdsBlockB(WORD BlockB);

DUT_FM_API int STDCALL Dut_Fm_GetFmRxRdsBlockB(WORD *pBlockB);

DUT_FM_API int STDCALL Dut_Fm_SetFmRxRdsBlockBMask(WORD BlockBMask);

DUT_FM_API int STDCALL Dut_Fm_GetFmRxRdsBlockBMask(WORD *pBlockBMask);

DUT_FM_API int STDCALL Dut_Fm_SetFmRxRdsPiCode(WORD PiCode);

DUT_FM_API int STDCALL Dut_Fm_GetFmRxRdsPiCode(WORD *pPiCode);

DUT_FM_API int STDCALL Dut_Fm_SetFmRxRdsPiCodeMask(WORD PiCodeMask);

DUT_FM_API int STDCALL Dut_Fm_GetFmRxRdsPiCodeMask(WORD *pPiCodeMask);

 
//	RdsMode
//	{
//	RDSMODE_RDS_STD, RDSMODE_RBDSWOE
//	}
DUT_FM_API int STDCALL Dut_Fm_SetFmRxRdsMode(int RdsMode);

DUT_FM_API int STDCALL Dut_Fm_GetFmRxRdsMode(int *pRdsMode);


DUT_FM_API int STDCALL Dut_Fm_GetFmTxOutputPwr(BYTE *pTxPwr);
DUT_FM_API int STDCALL Dut_Fm_SetFmTxOutputPwr(BYTE TxPwr);


// TBRM //	emphasis
// TBRM //	{
// TBRM //	EMPHASIS_NONE, EMPHASIS_50US, EMPHASIS_75US
// TBRM //	}
// TBRM DUT_FM_API int STDCALL Dut_Fm_SetFmAudioPreemphasis(int Mode);

DUT_FM_API int STDCALL Dut_Fm_SetFmFreqDeviation(DWORD FreqDeviaInHz);

// TBRM DUT_FM_API int STDCALL Dut_Fm_SearchFmTxFreeCh(DWORD StartChFreqInKhz);

DUT_FM_API int STDCALL Dut_Fm_SetFmTxMonoStereoMode(BYTE FmMonoStereoMode);



  
 
//	MuteMode
// {
//	OFF, ON
// }
DUT_FM_API int STDCALL Dut_Fm_SetFmTxMuteMode(int MuteMode);

//	InputGain
//	{
//	INPUTGAIN_2D5,	INPUTGAIN_1D25, INPUTGAIN_0D833
//	}
//	ActiveAudioCtrl
//	{
//	ACTAUDIOCTRL_RONLY,	ACTAUDIOCTRL_LONLY,	ACTAUDIOCTRL_STEREO  
//	}
DUT_FM_API int STDCALL Dut_Fm_ConfigFmAudioInput(int InputGain, 
												 int ActiveAudioCtrl);

DUT_FM_API int STDCALL Dut_Fm_EnableFmTxAudioAGC(int Mode_OffOn);

DUT_FM_API int STDCALL Dut_Fm_SetFmFreqDeviationExtended(DWORD LprDev,  DWORD LmrDev, DWORD PilotDev, DWORD RdsDev);
// TBRM DUT_FM_API int STDCALL Dut_Fm_GetFmTxPowerMode(int *pMode_HighLow);
DUT_FM_API int STDCALL Dut_Fm_SetFmTxPowerMode(int Mode_HighLow);

//DUT_FM_API int STDCALL Dut_Fm_ForceFmTxPilotGain(int Mode_OffOn, BYTE Gain);

//DUT_FM_API int STDCALL Dut_Fm_ForceFmTxRdsGain(int Mode_OffOn, BYTE Gain);

DUT_FM_API int STDCALL Dut_Fm_SetFmTxRdsPiCode(WORD PiCode);
//	RdsGroup
//	{
//	RDSGROUP_PS0A, RDSGROUP_PS0B, RDSGROUP_RT2A, RDSGROUP_RT2B
//	}
DUT_FM_API int STDCALL Dut_Fm_SetFmTxRdsGroup(int RdsGroup);

DUT_FM_API int STDCALL Dut_Fm_SetFmTxRdsPty(BYTE RdsPty);

DUT_FM_API int STDCALL Dut_Fm_SetFmTxRdsAfCode(BYTE AfCode);

//	RdsTxMode
//	{
//	RDSTXMODE_PS_RT, RDSTXMODE_RAW
//	}
DUT_FM_API int STDCALL Dut_Fm_SetFmTxRdsMode(int RdsTxMode);

DUT_FM_API int STDCALL Dut_Fm_SetFmTxRdsScrolling(int Mode_OffOn);

DUT_FM_API int STDCALL Dut_Fm_SetFmTxRdsScrollingRate(BYTE Rate_CharPerScroll);

DUT_FM_API int STDCALL Dut_Fm_SetFmTxRdsDisplayLength(BYTE Len_Chars);

DUT_FM_API int STDCALL Dut_Fm_SetFmTxRdsToggleAB(void);

//	Repository
//	{
//	REPOSITORY_G0, REPOSITORY_G1,REPOSITORY_G2
//	}
DUT_FM_API int STDCALL Dut_Fm_SetFmTxRdsRepository(int Repository);
DUT_FM_API int STDCALL Dut_Fm_GetFmSoftMute(int *pEnable);
DUT_FM_API int STDCALL Dut_Fm_SetFmSoftMute(int Enable, WORD MuteThreshold, BYTE AudioAttenuation);
DUT_FM_API int STDCALL Dut_Fm_GetFmStereoBlendingMode(int *pBlendingMode);
DUT_FM_API int STDCALL Dut_Fm_SetFmStereoBlendingMode(int BlendingMode);

DUT_FM_API int STDCALL Dut_Fm_GetPllLockStatus(int *pLocked);
DUT_FM_API int STDCALL Dut_Fm_SetFmStereoBlendingConfigration(int TimeConstant);
DUT_FM_API int STDCALL Dut_Fm_GetFmStereoStatus(int *pStatus);
//	SamplingRate
//	{
//	SAMP_8K,	SAMP_11D025K,	SAMP_12K,	SAMP_16K,	SAMP_22D05K, 
//	SAMP_24K,	SAMP_32K,		SAMP_44D1K, SAMP_48K
//	}

//	BClk_LrClk
//	{
//	CLK_32X,	CLK_50X,	CLK_64X
//	}
DUT_FM_API int STDCALL Dut_Fm_GetFmI2SParameter(int ChFreqInKHz, int *pSamplingRate, int *pBClk_LrClk);

DUT_FM_API int STDCALL Dut_Fm_GetFmAudioVolume(int *pVolume);
DUT_FM_API int STDCALL Dut_Fm_SetFmAudioVolume(int Volume);

#endif //#if defined(_W8689_) || defined(_W8787_) || defined(_W8790_)|| defined(_W8782_)

 
