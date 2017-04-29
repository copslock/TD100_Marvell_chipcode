/** @file DutBtApiClss.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#ifndef _DUT_BT_API_H_
#define _DUT_BT_API_H_
#include "../../DutCommon/Clss_os.h" 
#include "DutBtIf_IfClss.h"
#include "../DutBtApi.hc"
 
//#if !defined(AFX_NDISIF_H__90DC2111_AEB5_435E_B719_6A54BEB076A6__INCLUDED_)
//#define AFX_NDISIF_H__90DC2111_AEB5_435E_B719_6A54BEB076A6__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

#include DUTDLL_WLAN_HC_PATH	//"../DutWlanApi.hc"
#include DUTCLASS_SHARED_H_PATH	//"../DutShareApi878XDll/DutSharedClss.h"
 

typedef struct	Bt_PacketTypeCmd
{
	DWORD SlotCount:4;
	DWORD DataRate:4;
	DWORD FecType:4;
	DWORD Reserved:20;
}Bt_PacketTypeCmd;


//#define DutBtApiClssHeader	DutBtApiClss::
#ifndef _NO_CLASS_
class DutBtApiClss :public DutBtIf_If
{ 
char adapter_prefix[256];
#endif // #ifndef _NO_CLASS_ 

#ifndef _NO_CLASS_ 
friend  class DutSharedClss; 

private:
#endif //#ifnndef _NO_CLASS_




 
int PokeBtRegDword(DWORD address, DWORD data);
int PeekBtRegDword(DWORD address, DWORD *data);


int Bt_GetBtModePdetFlag(int *pFlagThHiU,
						 int *pFlagThHiD,
						 int *pFlagThLoU,
						 int *pFlagThLoD);

 
 
//////////////////////////////////////

//////////////////////////////////////
int	 InvalidPacketType(int PacketType);

int  MfgBtCmdTemplete(void);
void StuffHeader(void *TxBuf, DWORD cmd, DWORD action,DWORD size=24);
int	 CheckRspHeader(void *TxBuf, DWORD Rsp);
 

int BrfTxBtDutyCycle0(bool enable,			int dataRate, 
			 int payloadPattern,	int DutyWeight, bool payloadOnly);

////////////////////////////////////////////////////
////////////////////////////////////////////////////
////////////////////////////////////////////////////
 
 
#ifndef _NO_CLASS_
public:

DutBtApiClss();
~DutBtApiClss();
DutBtApiClss_Init();
#endif //#ifndef _NO_CLASS_

int Bt_Version(void);
char* Bt_About(void);
int ClipPayLoadLengthInByte(int PacketType,  
							int *pPayloadLenghtInByte);

int Bt_OpenDevice( );

int Bt_CloseDevice( );

int MfgPfwReloadCalfromFlash(void);
#ifdef _HCI_PROTOCAL_
int GetBDAddress(BYTE *BdAddress);
int GetBufferSizeInfo(WORD *pMaxAclPktLen, WORD *pTotalNumAclPkt,
					BYTE *pMaxScoPktLen, WORD *pTotalNumScoPkt);


int ResetBt(void);
int EnableDeviceUnderTestMode(void);
int SetFlowControlEnable(bool enable);
int PeriodicInquery(int MaxPeriod, int MinPeriod, int InquryLength);
int ExitPeriodicInquery(void);

typedef unsigned short USHORT;
typedef unsigned char UCHAR;

int Inquiry(BYTE ExpectedBdAddress[SIZE_BDADDRESS_INBYTE], 
			int* NumResp, int *expectedFound, 
			BYTE *pPageScanRepMode,
			WORD *pClockOffset);//(ULONG LAP, PINQUIRY_RESP pResp, int& NumResp);
int SetPageTimeOut(USHORT TimeOut);
int SetScanEnable(BYTE enableFlags);//
int SetEventFilter(PUCHAR pFilter, UCHAR Length);
int SetEventMask(PUCHAR pMask, UCHAR Length);
int SetAuthenticationEnable(BYTE enableFlag);
int SetVoiceSetting(WORD setting);
int SetDeviceDiscoverable(void);

/*
int CreateAclConnect(BYTE PartnerBdAddress[SIZE_BDADDRESS_INBYTE], 
					BYTE PageScanRepMode, WORD ClockOffset,
					WORD* pHConnection);
int DisConnect(WORD hConnection, BYTE Reason = 0x13);
int SetConnPacketType(WORD hConnection, WORD PacketType);
int getPacketTypeSetting(int PacketType);


int WriteLinkPolice(WORD hConnection, 
				WORD RoleSwitchMode,
				WORD HoldMode,
				WORD SniffMode,
				WORD ParkMode);
int ReadLinkPolice(WORD hConnection, 
									  WORD *pRoleSwitchMode,
									 WORD *pHoldMode,
									 WORD *pSniffMode,
									 WORD *pParkMode);

int Hold(WORD hConnection, WORD HoldMaxInterval, WORD HoldMinInterval, bool wait4Exit=0);
int Park(WORD hConnection, WORD BeaconMaxInterval, WORD BeaconMinInterval);
int ExitPark(WORD hConnection);
int Sniff(	WORD hConnection, 
			WORD SniffMaxInterval,	WORD SniffMinInterval,
			WORD SniffAttempt,		WORD SniffTimeout);
int ExitSniff(WORD hConnection);
*/
#endif //#ifdef _HCI_PROTOCAL_

int WriteMemDword(DWORD address, DWORD data);
int ReadMemDword(DWORD address, DWORD *data);
int WriteBtuReg(DWORD address, WORD data);
int WriteBtuReg8(DWORD address, BYTE data);
int ReadBtuReg(DWORD address, WORD *data);
int WriteBrfReg(DWORD address, WORD data);
int ReadBrfReg(DWORD address, WORD *data);

#if defined (_FM_) 
int WriteFmuReg(DWORD address, DWORD data);
int ReadFmuReg(DWORD address, DWORD *data);

int FmReset(void);
int FmInitialization(int XtalFreqInKHz); //, int FreqErrPpm);
//int SetFmReferenceClk(int CalFreqInKHz);	// 
int SetFmReferenceClkError(int FreqErrPpm);	// 
int GetFmRdsBLER(int *pSeqGood, int *pSeqNoSync, 
									int *pExpected, int *pSuccRate);
int ResetFmRdsBLER(int SecPeriod);

int FmPowerUpMode(int PowerMode);				
int GetFmCurrentRssi(DWORD *pCurrentRssi);
int GetFmCurrentCmi(DWORD *pCurrentCmi);

int SelectFmRxAntennaType(int AntType);	// 0: 75Ohm, 1: TxLoop 
int SetFmIrsMask(int RssiLow,	int NewRdsData,	int RssiIndicator,	int BandLimit, 
				 int SyncLost,	int SearchStop, int BMatch,			int PiMatched,
				 int MonoStereoChanged,	int AudioPause, int CmiLow);	// 
int GetFmIrsMask(int *pRssiLow,		int *pNewRdsData,	int *pRssiIndicator,	int *pBandLimit, 
				 int *pSyncLost,	int *pSearchStop,	int *pBMatch,			int *pPiMatched,
				 int *pMonoStereoChanged,	int *pAudioPause, int *pCmiLow);	// 
int GetFmCurrentFlags(DWORD *pFlags);	// 
int FmPowerDown(void);	// 

int SetFmChannel(DWORD *pFmChInKHz);
int GetFmChannel(DWORD *pFmChInKHz);
int SetFmAfChannel(DWORD *pFmAfChInKHz, BYTE AfMode);
//int GetFmAfChannel(DWORD *pFmAfChInKHz);

int SetFmSearchDir(BYTE FmSearchDir_F0B1);
int GetFmSearchDir(DWORD *pFmSearchDir_F0B1);

int SetFmAutoSearchMode(BYTE FmAutoSearchMode);
int GetFmAutoSearchMode(DWORD *pFmAutoSearchMode);

int StopFmSearch(void);	// 

 
int SetFmRxForceStereoMode(BYTE FmForceStereoMode);


int SetFmRxForceMonoMode(BYTE FmForceMonoMode);
int GetFmRxForceMonoMode(DWORD *pFmForceMonoMode);
int SetFmRssiThreshold(DWORD FmRssiThreshold);
int GetFmRssiThreshold(DWORD *pFmRssiThreshold);
int SetFmCmiThreshold(DWORD FmCmiThreshold);
int GetFmCmiThreshold(DWORD *pFmCmiThreshold);
int SetFmBand(BYTE FmBand);
int SetFmChStepSizeKHz(DWORD StepSizeKHz);
int FmMoveChUp(DWORD *pFmChInKHz);
int FmMoveChDown(DWORD *pFmChInKHz);

//	emphasis
//	{
//	EMPHASIS_NONE, EMPHASIS_50US, EMPHASIS_75US
//	}
int SetFmAudioEmphasis(int Mode);		// 
// TBRM int GetFmRxAudioDeemphasis(int *pMode);	// 
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
int SetFmRxMuteMode(int MuteMode);	// 
int GetFmRxMuteMode(int *pMuteMode);	// 

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
int SetRxAudioPath(BYTE AudioPath,		BYTE I2sOperation,		BYTE I2sMode);	// 
int GetRxAudioPath(BYTE *pAudioPath);	// 

//	Sampling
//	{
//	SAMP_8K,	SAMP_11D025K,	SAMP_12K,	SAMP_16K,	SAMP_22D05K, 
//	SAMP_24K,	SAMP_32K,		SAMP_44D1K, SAMP_48K
//	}

//	BClk_LrClk
//	{
//	CLK_32X,	CLK_50X,	CLK_64X
//	}
int SetFmRxAudioSamplingRate(int Sampling, int BClk_LrClk);	// 

//	PauseMode
//	{
//	PAUSEMODE_OFF, PAUSEMODE_PAUSESTARTONLY, PAUSEMODE_PAUSESTOPONLY, PAUSEMODE_STARTSTOP
//	}
int EnableFmAudioPauseMode(int PauseMode);	// 
int SetFmAudioPauseDuration(WORD PauseDurInMs);	// 
// TBRM int GetFmAudioPauseDuration(WORD *pPauseDurInMs);	// 
int SetFmAudioPauseLevel(WORD PauseLevel);	// 
// TBRM int GetFmAudioPauseLevel(WORD *pPauseLevel);	// 

//	RdsDataPath
//	{
//	RDSDATAPATH_2NOWHERE, RDSDATAPATH_2HCI, RDSDATAPATH_2I2C, RDSDATAPATH_2HFP, RDSDATAPATH_2A2DP	
//	}
//int SetFmRxRdsDataPath(int Path);	// 
//int GetFmRxRdsDataPath(int *pPath);	// 
int GetFmRxRdsSyncStatus(BYTE *pSyncStatus);	// 
int FmRxRdsFlush(void);	// 
// int GetFmRdsData
int SetFmRxRdsMemDepth(BYTE Depth);	// 
int GetFmRxRdsMemDepth(BYTE *pDepth);	// 

int SetFmRxRdsBlockB(WORD BlockB);	// 
int GetFmRxRdsBlockB(WORD *pBlockB);	// 
int SetFmRxRdsBlockBMask(WORD BlockBMask);	// 
int GetFmRxRdsBlockBMask(WORD *pBlockBMask);	// 

int SetFmRxRdsPiCode(WORD PiCode);	// 
int GetFmRxRdsPiCode(WORD *pPiCode);	// 
int SetFmRxRdsPiCodeMask(WORD PiCodeMask);	// 
int GetFmRxRdsPiCodeMask(WORD *pPiCodeMask);	// 

//	RdsMode
//	{
//	RDSMODE_RDS_STD, RDSMODE_RBDSWOE
//	}
int SetFmRxRdsMode(int RdsMode);	// 
int GetFmRxRdsMode(int *pRdsMode);	// 

int GetFmFwVersion(DWORD *pFwVersion);
int GetFmHwVersion(DWORD *pHwVersion);
int GetFmHwId(DWORD *pHwId);
int GetFmManId(DWORD *pVId);


int GetFmTxOutputPwr(BYTE *pTxPwr);
int SetFmTxOutputPwr(BYTE TxPwr);	// 
// TBRM //	emphasis
// TBRM //	{
// TBRM //	EMPHASIS_NONE, EMPHASIS_50US, EMPHASIS_75US
// TBRM //	}
// TBRM int SetFmAudioPreemphasis(int Mode);	// 
// TBRM // int GetFmAudioPreemphasis(int *pMode);	// 

int SetFmFreqDeviation(DWORD FreqDeviaInHz);			// 
// TBRM int SearchFmTxFreeCh(DWORD StartChFreqInKhz);			// 
int SetFmTxMonoStereoMode(BYTE FmMonoStereoMode);		// 
//int GetFmTxMonoStereoMode(DWORD *pFmMonoStereoMode);	// 

//	MuteMode
// {
// OFF,ON
// }
int SetFmTxMuteMode(int MuteMode);	// 

//	InputGain
//	{
//	INPUTGAIN_2D5,	INPUTGAIN_1D25, INPUTGAIN_0D833
//	}
//	ActiveAudioCtrl
//	{
//	ACTAUDIOCTRL_RONLY,	ACTAUDIOCTRL_LONLY,	ACTAUDIOCTRL_STEREO  
//	}
int ConfigFmAudioInput(int InputGain, int ActiveAudioCtrl);	// 

int EnableFmTxAudioAGC(int Mode_OffOn);		// 
int SetFmFreqDeviationExtended(	DWORD LprDev,  DWORD LmrDev, DWORD PilotDev, DWORD RdsDev);

// TBRM int GetFmTxPowerMode(int *pMode_HighLow);
int SetFmTxPowerMode(int Mode_HighLow);

//int ForceFmTxPilotGain(int Mode_OffOn, BYTE Gain);		// 
//int ForceFmTxRdsGain(int Mode_OffOn, BYTE Gain);		// 
int SetFmTxRdsPiCode(WORD PiCode);		// 
//	RdsGroup
//	{
//	RDSGROUP_PS0A, RDSGROUP_PS0B, RDSGROUP_RT2A, RDSGROUP_RT2B
//	}
int SetFmTxRdsGroup(int RdsGroup);		// 
int SetFmTxRdsPty(BYTE RdsPty);			// 
int SetFmTxRdsAfCode(BYTE AfCode);		// 
//	RdsTxMode
//	{
//	RDSTXMODE_PS_RT, RDSTXMODE_RAW
//	}
int SetFmTxRdsMode(int RdsTxMode);				// 
int SetFmTxRdsScrolling(int Mode_OffOn);		// 
int SetFmTxRdsScrollingRate(BYTE Rate_CharPerScroll);	// 
int SetFmTxRdsDisplayLength(BYTE Len_Chars);			// 
int SetFmTxRdsToggleAB(void);			// 
//	Repository
//	{
//	REPOSITORY_G0, REPOSITORY_G1,REPOSITORY_G2
//	}
int SetFmTxRdsRepository(int Repository);		// 

// Mute Threshold (0.37 dB/LSB) 
// Audio attenuation 
// "0 dB attenuation",   0x00
// "3 dB attenuation",   0x01
// "6 dB attenuation",   0x02
// "9 dB attenuation",   0x03
// "12 dB attenuation",  0x04
// "15 dB attenuation",  0x05
// "18 dB attenuation",  0x05
// "21 dB attenuation",  0x07
//{
//	MAX_ATTEN_0DB,	MAX_ATTEN_3DB,	MAX_ATTEN_6DB, 
//	MAX_ATTEN_9DB,	MAX_ATTEN_12DB,	MAX_ATTEN_15DB, 
//	MAX_ATTEN_18DB, MAX_ATTEN_21DB
//}
int GetPllLockStatus(int *pLocked);

int GetFmSoftMute(int *pEnable);
int SetFmSoftMute(int Enable, WORD MuteThreshold, BYTE AudioAttenuation);

// 0= swithcing blending, 1= soft blending
int GetFmStereoBlendingMode(int *pBlendingMode);
int SetFmStereoBlendingMode(int BlendingMode);

int SetFmStereoBlendingConfigration(int TimeConstant);
int GetFmStereoStatus(int *pStatus);
//	SamplingRate
//	{
//	SAMP_8K,	SAMP_11D025K,	SAMP_12K,	SAMP_16K,	SAMP_22D05K, 
//	SAMP_24K,	SAMP_32K,		SAMP_44D1K, SAMP_48K
//	}

//	BClk_LrClk
//	{
//	CLK_32X,	CLK_50X,	CLK_64X
//	}
int GetFmI2SParameter(int ChFreqInKHz, int *pSamplingRate, int *pBClk_LrClk);


int GetFmAudioVolume(int *pVolume);
int SetFmAudioVolume(int Volume);

#endif //#if defined (_FM_) 


int findStepdB(int Points, int GainPpa6dBStepMode,
	char *PwrIndBm,
	BYTE *GainPpa,
	BYTE *GainPga,
	BYTE *GainBb, 
	double targetPwr, 
	int *lowEndIndex, 
	double *stepdB); 

 
int Bt_GetHwVersion(int *pSocChipId=NULL,int *pSocChipVersion=NULL,
					int *pBtuChipId=NULL,int *pBtuChipVersion=NULL,
					int *pBrfChipId=NULL,int *pBrfChipVersion=NULL);
int Bt_GetFwVersion(int *pFwVersion=NULL, int *pMfgVersion=NULL, 
					char *pVersionString=NULL);
int Bt_GetCurrentORVersion(int *pSocVersion=NULL, int *pRfVersion=NULL);
int Bt_GetCurrentAppMode(int *pAppMode);

#if defined(_W8780_)
int GetTrSwAntenna(int *pAntSelection);
int SetTrSwAntenna(int AntSelection);
#endif // #if defined(_W8780_)

int SetPcmLoopBackMode(int enable, int useGpio12AsDin=0);

int SetBtChannel(int channelNum, bool BT2=0);
int SetBtChannelByFreq(int ChannelFreq, bool BT2=0);
int GetBtChannel(int *pChannelNum, int *pChannelFreq=NULL, bool BT2=0);


int SetBt2ChHopping(bool enable);
int GetBt2ChHopping(bool *pEnable);

int SetBtPseudoCode(DWORD option);
int GetBtPseudoCode(DWORD *pOption);

int SetBtPwrControlClass(DWORD option);				//0:MRVL, 1:Class2, 2:Class1 3: Class2+XPA
int GetBtPwrControlClass(DWORD *pOption);

int SetBtBrfReset(void);
int SetBtPwrGain(DWORD Value);
int GetBtPwrGain(DWORD *pValue);
 
int SetBtPwrLvlValue(double PwrLvlValueDB, DWORD IsEDR);
int GetBtPwrLvlValue(double *pPwrLvlValueDB );

int GetBtPwrMaxMin(double *pPwrMaxDB, double *pPwrMinDB);
 

int StepPower(double stepDB); 
int BrfInit(void);

int SetBtDisableBtuPwrCtl(bool DisableBtuPwrCtl);
int GetBtDisableBtuPwrCtl(bool *pDisableBtuPwrCtl); 
 

int SetBtXtal(BYTE ExtMode, BYTE XtalValue);		// 0 internal mode 1: external mode
int GetBtXtal(BYTE *pExtMode, BYTE *pXtalValue);

int GetBtTxDetectedInChip(int *pTx);

// not supported by this chip  
int BrfTxBtCont(bool enable,			int dataRate, 
			 int payloadPattern);
int BrfTxBtDutyCycle(bool enable,			int dataRate, 
			 int payloadPattern,	int DutyWeight, bool payloadOnly);
//int TxBtMultiCastFrames(int FrameCount,	int PayloadLength,
//						int PacketType, 
//			 int payloadPattern, bool payloadOnly);


#ifdef _HCI_PROTOCAL_
int TxBtDutyCycleHop(bool enable,			int PacketType, 
			 int payloadPattern=DEF_PAYLOADPATTERN,	
			 int PayloadLenghtInByte=PAYLOADLENGTH_MAXIMUN_POSSIBLE, 
			 bool HopMode=0, int interval=1,
			 int Whitening=0);
int TxBtDutyCycleHop2(bool enable,			int PacketType, 
			 int payloadPattern=DEF_PAYLOADPATTERN,	
			 int PayloadLenghtInByte=PAYLOADLENGTH_MAXIMUN_POSSIBLE, 
			 bool HopMode=0, int interval=1,
			 int Whitening=0, 
			 int TxCnt=0, char TxPwrLvl=0x7F);
//int TxBtMultiCastFramesHop(int FrameCount,	int PayloadLength,
//						int PacketType, 
//						int payloadPattern, bool HopMode=1);


int BtMrvlRxTest(	int RxChannel,
					int PacketNumber,	
					int PacketType, 
					int payloadPattern=DEF_PAYLOADPATTERN,	
					int PayloadLenghtInByte=PAYLOADLENGTH_MAXIMUN_POSSIBLE, 
					char TxBdAddress[SIZE_BDADDRESS_INBYTE]=NULL
		//	 , int TxAmAddress=DEF_AMADDRESS, 
		//	 int FramSkip=0, 
		//	 int CorrWndw=DEF_CORRELATION_WINDOW, 
		//	 int CorrTh=DEF_CORRELATION_THRESHOLD, 
		//	 bool Whitening=0,BYTE PrbsPattern[LEN_PRBS_INBYTE]=NULL
		);

int BtMrvlRxTestResult( 
			 // report
				BYTE*		Report_Status=NULL, // status
				DWORD*		Report_TotalPcktCount=NULL,
				DWORD*		Report_HecMatchCount=NULL,
				DWORD*		Report_HecMatchCrcPckts=NULL,
				DWORD*		Report_NoRxCount=NULL,
				DWORD*		Report_CrcErrCount=NULL,
				DWORD*		Report_SuccessfulCorrelation=NULL,
				DWORD*		Report_TotalByteCount=NULL,
				DWORD*		Report_BER=NULL,
				DWORD*		Report_PER=NULL,
				DWORD*		Report_AvgRssi=NULL,
				DWORD*		Report_TotalBitsCountExpected=NULL,
				DWORD*		Report_TotalBitsCountError_LostDrop=NULL);

 
#endif //#ifdef _HCI_PROTOCAL_


// not supported by this chip  
// int TxBtCst(bool enable, int FreqOffsetinKHz=250);
int TxBtCw(bool enable);


//int ResetAccum(void);
//int GetAccum(int *pAccum);

#if defined (_W8688_) || defined (_W8787_) || defined(_W8782_)
// not supported by this chip  
//int SetBcaMode(int Mode);
//int GetBcaMode(int *pMode);
int SetFE_BTOnly(bool bOn);
#endif //#if defined (_W8688_) || defined (_W8787_) || defined(_W8782_)


int SetExtendedPwrCtrlMode(int Mode);
int GetExtendedPwrCtrlMode(int *pMode);
int SetBbGainInitValue(WORD Gain);
int GetBbGainInitValue(WORD *pGain);
int SetBtPwrBbGainMaxMin(bool enable,	
							WORD MaxValue,		
							WORD MinValue);		
int GetBtPwrBbGainMaxMin(bool *pEnable, 
					WORD *pMaxValue,		
					WORD *pMinValue);	
int SetPpaGainStepMode(int Mode);
int GetPpaGainStepMode(int *pMode);
int SetAutoZeroMode(int Mode, WORD Value);
int GetAutoZeroMode(int *pMode, WORD *pValue);
/*
int SetTxPwrModeHighLow(int Mode);
int GetTxPwrModeHighLow(int *pMode);
*/
int SetEfuseData(int Block, int LenInWord, WORD *pData);
int GetEfuseData(int Block, int LenInWord, WORD *pData);

//int GetBtThermalSensorReading(DWORD *pValue);
int SetBtThermalCtrlMode(DWORD Enable);

#ifdef _ENGINEERING_
int GetBtDbgThermalCtrl(DWORD *pCtrlActive,
						DWORD *pTempAvg,
						DWORD *pTempCur,
						DWORD *pCorrDelta,
						DWORD *pTemp10Avg);
#endif // #ifdef _ENGINEERING_

int GetChipClassModeCapacity(int *pMode);

int SetMrvlTempCompModeHC(int		SwPwrCtrlMode,
											WORD	RefGain1,
 											WORD	RefGain2,
 											BYTE	RefTemp,
 											BYTE	TempGain);

int GetFwReleaseNote(char *pReleaseNote);
 
//int GetTxIQ(DWORD *pExtMode, BYTE *pAlpha, BYTE *pBeta);
//int SetTxIQ(DWORD extMode, BYTE Alpha, BYTE Beta);

int Bt_ForceE2PromType(	DWORD		IfType, 
						DWORD		AddrWidth,
						DWORD		DeviceType=-1);
int Bt_GetE2PromType(	DWORD		*pIfType, 
						DWORD		*pAddrWidth,
						DWORD		*pDeviceType);

int Bt_SpiReadData(DWORD StartAddress, 
									DWORD LenInDw,
 									DWORD *pDataArray);

int Bt_SpiWriteData(DWORD StartAddress, 
								   DWORD LenInDw, 
									DWORD *pDataArray, 
									bool SectorErase);

int Bt_EraseEEPROM(int section);



int MfgResetBt(void);
int MfgGetFwBdAddress(unsigned char *pBdAddress);

int MfgEnterTestMode(void);
int MfgPeriodicInquery(int MaxPeriod, int MinPeriod, int InquryLength);
int MfgExitPeriodicInquery(void);

//int MfgGetBdAddressFromFw(char *pBdAddress);
//int MfgSetBdAddressInFw(char *pBdAddress);
int MfgMrvlTxTest2(bool enable,	
									  int PacketType, 
									  int PayLoadPattern, 
									  int PayloadLength,
									  int RandomHopping,
                                      int Interval,
									  int Whitening, 
										int TxCnt=0, char TxPwrLvl=0x7F);

int MfgMrvlTxTest(bool enable,	int PacketType, 
				  int PayLoadPattern, 
				  int PayloadLength,int RandomHopping,int Interval, int Whitening);

int MfgMrvlRxTest(	int RxChannel,
					int PacketNumber,
					int PacketType, 
					int PayLoadPattern, 
					int PayloadLength,						
			//		char *pPRBS,	
					char *pTxBdAddress);
int MfgMrvlRxTestResult(
				BYTE*		Report_Status, 
				DWORD*		Report_TotalPcktCount,
				DWORD*		Report_HecMatchCount,
				DWORD*		Report_HecMatchCrcPckts,
				DWORD*		Report_NoRxCount,
				DWORD*		Report_CrcErrCount,
				DWORD*		Report_SuccessfulCorrelation,
				DWORD*		Report_TotalByteCount,
				DWORD*		Report_BER,
				DWORD*		Report_PER,
				DWORD*		Report_AvgRssi,
				DWORD*		Report_TotalBitsCountExpected,
				DWORD*		Report_TotalBitsCountError_LostDrop);
int MfgMrvlRssiClear(int LinkId);
int MfgMrvlRssiRpt(int LinkId, int *pRssiAvg, int *pRssiLast);
 

bool SupportNoE2PROM();

#ifdef _HCI_PROTOCAL_
int StartAclConnection( WORD	*hDutConnection, 
											 void *pGru,  
											 WORD	*hGruConnection );
int StopAclConnection(WORD	*hMasterConnection);

int Test_Hold(int modeOnOff, 
							  WORD HoldMaxInterval, 
							  WORD HoldMinInterval, bool partnerAutoCtrl=0);
 
int Test_Park(int modeOnOff, 
							  WORD BeaconMaxInterval, 
							  WORD BeaconMinInterval,
							  bool partnerAutoCtrl=0);

int Test_Sniff(int modeOnOff, 
								WORD SniffMaxInterval,	
								WORD SniffMinInterval,
								WORD SniffAttempt,		
								WORD SniffTimeout,
								bool partnerAutoCtrl=0);


int SetRfDynamicClkCtrl(int		DynClkTimer,
						int		MultiSlotProtect,
						int		AutoDetect,
						int		DynClkValue,
						int		DynClkEnable,
						int		DynClkStartTime,
						int		DynClkFinishTime,
						int		DynClkTimedEnable);

int SetBtSleepMode(int PowerMode);

int SetWakeUpMethod(int		HostWakeUpMethod,
					int		HostWakeUpGPIO,
					int		DeviceWakeUpMethod,
					int		DeviceWakeUpGPIO);


int SetMrvlHiuModuleConfig(int ModuleShutDownBringUp, 
						   int HostIfActive, 
						   int HostIfType);


int SetMrvlRadioPerformanceMode(	int		MrvlRxPowerMode,	 
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

int SetMrvlArmDynamicClkCtrl(int		SetNcoMode,	 
 							int		DynamicClockChange 	 );


int SetBtXCompModeConfig(int		BTCoexistenceMode,	 
 									int		PriorityMode_Wlan,
									int PriorityMode_UpdateDefInPta,
									int PriorityDelay,
									int HighPriority_BitMap);
int SetBtMrvlModeConfig(int		BTCoexistenceMode,	 
 									int PriorityMode_Wlan,
									int PriorityMode_UpdateDefInPta,
									int PriorityDelay,
									int HighPriority_BitMap, 
									int MediumPriority_BitMap,
									int BtEndTimer,
									int BtFreqLowBand,
									int BtFreqHighBand );

int SetBtMrvlMode2Config(int		BTCoexistenceMode,	 
 									int	NonPeriodicMode_Enable, 
									int NonPeriodicMode_BtTimerTlead,
									int NonPeriodicMode_BtTimerTtail,
									int PeriodicMode_BtSyncPeriod,
									int PeriodicMode_BtSyncPulseWidth,
									int PeriodicMode_BtTsyncLead,
									int PeriodicMode_BtSyncMode);

#endif //#ifdef _HCI_PROTOCAL_
 
#ifndef _NO_CLASS_



};
#endif //#ifndef _NO_CLASS_
//#endif // !defined(AFX_NDISIF_H__90DC2111_AEB5_435E_B719_6A54BEB076A6__INCLUDED_)

#endif
