/** @file DutWlanApiDll.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */
#ifndef _DUTAPI83XXPDLL_H_
#define _DUTAPI83XXPDLL_H_
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DUTAPICFDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DUTAPICFDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifndef _LINUX_  
#ifdef DUTAPI83XXPDLL_EXPORTS
#define DUT_API extern "C" __declspec(dllexport)
#else
#ifdef _DEBUG
#define DUT_API extern "C"
#else
#define DUT_API extern "C" __declspec(dllimport)
#endif
#endif
#ifndef STDCALL
#define STDCALL _stdcall
#endif
#else	//#ifndef _LINUX_
#define NULL 0
#define DUT_API
#define STDCALL
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned int BOOL;
typedef unsigned int BOOLEAN;
#endif	//#ifndef _LINUX_
#include "DutApi.hc"
#include "..\DutCommon\utilities.h"

//DUT_API int STDCALL DebugLogRite(const char *string, ... );
//DUT_API int STDCALL PrintOnScreen(BOOLEAN flag);
DUT_API int STDCALL SetGuiLogHandle(int h);
//DUT_API int STDCALL SetOutputFileName(char *pFileName);

DUT_API int STDCALL DutIf_PokeRegDword(DWORD address, DWORD data);
DUT_API int STDCALL DutIf_PeekRegDword(DWORD address, DWORD *data);
 
DUT_API int STDCALL DutIf_PokeRegWord(DWORD address, WORD data);
DUT_API int STDCALL DutIf_PeekRegWord(DWORD address, WORD *data);
DUT_API int STDCALL DutIf_PokeRegByte(DWORD address, BYTE data);
DUT_API int STDCALL DutIf_PeekRegByte(DWORD address, BYTE *data);
 
 
DUT_API int STDCALL DutIf_WriteRfReg(BYTE address, BYTE data); 
DUT_API int STDCALL DutIf_ReadRfReg(BYTE address, BYTE *data); 
DUT_API int STDCALL DutIf_WriteBBReg(int address, BYTE data); 
DUT_API int STDCALL DutIf_ReadBBReg(int address, BYTE *data); 
DUT_API int STDCALL DutIf_WriteMACReg(DWORD address, DWORD data);	 
DUT_API int STDCALL DutIf_ReadMACReg(DWORD address, DWORD *data);  
 
DUT_API int STDCALL DutIf_WriteSQmem(DWORD address, DWORD data);
DUT_API int STDCALL DutIf_ReadSQmem(DWORD address, DWORD *data);

DUT_API int STDCALL DutIf_DllVersion(void); 
DUT_API char* STDCALL DutIf_About(void);

DUT_API int STDCALL DutIf_InitConnection(void **theObj);		 
DUT_API int STDCALL DutIf_Disconnection(void);	 
DUT_API int STDCALL DutIf_FirmwareDownload(void);

 
DUT_API int STDCALL DutIf_SetBandAG(int band);
DUT_API int STDCALL DutIf_GetBandAG(int *band); 

// int set channel, set power, read RSSI, read .11status, 
DUT_API int STDCALL DutIf_SetRfChannel(int channel);
DUT_API int STDCALL DutIf_GetRfChannel(int *channel, int *pFreqInMHz=NULL);

//DUT_API int STDCALL DutIf_SetRfChannel_new(int channel);
//DUT_API int STDCALL DutIf_GetRfChannel_new(int *channel);

//DUT_API int STDCALL DutIf_SetRfChannel_div6(int channel);
//DUT_API int STDCALL DutIf_GetRfChannel_div6(int *channel);

DUT_API int STDCALL DutIf_SetRfPower(int *pPwr4Pa, 
										   int *pCorrOffset, 
										   int *pCorrOffsetInitP);
 
//DUT_API int STDCALL DutIf_GetRfPowerCorrOffset(int Pwr4Pa, 
//													 int *pCorrOffset, 
//													 int *pCorrOffsetInitP) ;
  
DUT_API int STDCALL DutIf_GetRfPowerSetting(BYTE *pPaConf1, BYTE *pPaConf2,
										BYTE *pPaConf3, 
										BYTE *pVt_Hi, BYTE *pVt_Lo,
										BYTE *pInitP, BYTE *pDacExtsn);
 

DUT_API int STDCALL DutIf_GetRfPower(	int *pPwr4Pa, 
										int *pCorrOffset, 
										int *pCorrOffsetInitP);

DUT_API int STDCALL DutIf_SetRfVga(int extMode, int value);
DUT_API int STDCALL DutIf_GetRfVga(int *pExtMode, int *pValue);
DUT_API int STDCALL DutIf_SetRfLcCap(int extMode, int value);
DUT_API int STDCALL DutIf_GetRfLcCap(int *pExtMode, int *pValue);
DUT_API int STDCALL DutIf_SetRfXTal(int value);
DUT_API int STDCALL DutIf_GetRfXTal(int *pValue);

DUT_API int STDCALL DutIf_StepRfPower(int *pSteps);
 
DUT_API int STDCALL DutIf_SetRfPowerRange(int pwrRange);

DUT_API int STDCALL DutIf_GetRfPowerRange(int *pRangeIndex=NULL, int *pTopPwr=NULL, int *pBtmPwr=NULL); 

DUT_API int STDCALL DutIf_ClearRxPckts(void);
DUT_API int STDCALL DutIf_GetRxPckts(unsigned long *cntRx,
									 unsigned long *cntOk,
									 unsigned long *cntKo); 

DUT_API int STDCALL DutIf_SetRxAntenna(int antSelection); 
DUT_API int STDCALL DutIf_SetTxAntenna(int antSelection); 

DUT_API int STDCALL DutIf_GetRxAntenna(int *antSelection); 
DUT_API int STDCALL DutIf_GetTxAntenna(int *antSelection); 

DUT_API int STDCALL DutIf_SetTxDataRate(int rate);
DUT_API int STDCALL DutIf_GetTxDataRate(int *rate);

DUT_API int STDCALL DutIf_Enable22M(void);
DUT_API int STDCALL DutIf_Enable72M(void);

DUT_API int STDCALL DutIf_SetTxContMode(BOOL enable, BOOL CWmode, 
										DWORD payloadPattern=0);  
DUT_API int STDCALL DutIf_SetTxCarrierSuppression(BOOL enable);  
DUT_API int STDCALL DutIf_SetTxIQCalMode(BOOL enable);  
DUT_API int STDCALL DutIf_SetTxDutyCycle(BOOL enable, int dataRate=4,  
										 int Percentage=50, 
										 int payloadPattern=0, 
										 int ShortPreamble=0);   

DUT_API int STDCALL DutIf_SetPowerMode(int mode);		// 0 active, 1 standby mode, 3 power down mode
DUT_API int STDCALL DutIf_SetPowerCycleMode(int cycle, 
							 int sleepDur, 
							 int awakeDur, 
							 int stableXosc);

DUT_API int STDCALL DutIf_SetBeaconInterval(DWORD interval); //in ms
DUT_API int STDCALL DutIf_GetBeaconInterval(DWORD *interval);//in ms
DUT_API int STDCALL DutIf_SetBeaconEnable(BOOL enable);
DUT_API int STDCALL DutIf_GetBeaconEnable(BOOL *enable);

DUT_API int STDCALL DutIf_GetHWVersion(WORD *socId, BYTE *socVersion, 
									   BYTE *RfId, BYTE *RfVersion, 
									   BYTE *BbId, BYTE *BbVersion);
DUT_API int STDCALL DutIf_GetORVersion(BYTE OR_ID, BYTE *OR_Major, BYTE *OR_Minor, BYTE *OR_Cust);
DUT_API int STDCALL DutIf_GetFWVersion(BYTE *version);
DUT_API int STDCALL DutIf_GetReleaseNote(char *ReleaseNote);

DUT_API int STDCALL DutIf_MacWakeupRfBBP(void);
DUT_API int STDCALL DutIf_EnableBssidFilter(int mode=0, BYTE *Bssid=0, char *SSID=0);
DUT_API int STDCALL DutIf_TxBrdCstPkts(DWORD dataRate,	DWORD pattern, 
				 DWORD length,		DWORD count,
					int ShortPreamble=1, char *Bssid=0);
DUT_API int STDCALL DutIf_XoscCal(WORD *cal, int TU);

DUT_API int STDCALL DutIf_SoftReset(void);
DUT_API int STDCALL DutIf_HwResetRf(void); 
DUT_API int STDCALL DutIf_SetRssiNf(void);
DUT_API int STDCALL DutIf_GetRssiNf(int stop, 
				  int *count, int *rssi, 
				  int *SNR, int *NoiseFloor);

DUT_API int STDCALL DutIf_SetCaledRfPower(int *pPwr4Pa, int RateG=0 );

DUT_API int STDCALL DutIf_SetRfPowerCal( int *pPwr4Pa, int RateG=0,
										BYTE *cal_data_ext=NULL, int len=0);

#ifndef NO_EEPROM
DUT_API int STDCALL DutIf_EraseEEPROM(int section);
#endif
DUT_API int STDCALL DutIf_GetMACAddress(BYTE *ByteArray, BOOL NOEEPROM=0);
DUT_API int STDCALL DutIf_SetMACAddress(BYTE *ByteArray, BOOL NOEEPROM=0);
 
DUT_API int STDCALL DutIf_SetPIDVID(DWORD E2pOffset,  WORD PID, WORD VID);
DUT_API int STDCALL DutIf_GetPIDVID(DWORD E2pOffset,  WORD *PID, WORD *VID);
DUT_API int STDCALL DutIf_SetClassId(DWORD E2pOffset,  DWORD Class);
DUT_API int STDCALL DutIf_GetClassId(DWORD E2pOffset,  DWORD *Class);

DUT_API int STDCALL DutIf_SetGPIOTCotrl(int GPIOPin, int Enable);

DUT_API int STDCALL DutIf_CheckCalDataSpace(int* pAvalable); 
DUT_API int STDCALL DutIf_GetHeaderPresent(int *pPresent, 
										   DWORD *pVersion=NULL, int *CsC=NULL); 

DUT_API int STDCALL DutIf_GetTxIQValue(int *pExtMode  , DWORD *pAmp, DWORD *pPhase);
DUT_API int STDCALL DutIf_SetTxIQValue(int ExtMode  , DWORD Amp, DWORD Phase);

DUT_API int STDCALL DutIf_SetCalMainData(
					BYTE dsgnVar,		BYTE dsgnType,
					BYTE dsgnMjrVer,	BYTE dsgnMinVer,
					BYTE pathId,		BYTE deviceId,
					BYTE externLNAG1,	BYTE externLNAA1,
					BYTE externLNAG0,	BYTE externLNAA0,

					BYTE externPaPolarG1,	BYTE externPaPolarA1,
					BYTE externPaPolarG0,	BYTE externPaPolarA0,
#if defined(_W8688_) || defined(_W8682_) || defined( _W8780_) 
					BYTE externLnaPolarG1,	BYTE externLnaPolarA1,
					BYTE externLnaPolarG0,	BYTE externLnaPolarA0,
#endif  //#if defined(_W8688_) || defined(_W8682_) || defined( _W8780_) 
                    BYTE txAnt1,		BYTE txAnt0,
					BYTE rxAnt1,		BYTE rxAnt0,
					BYTE WlanWakeup,	BYTE crystal,

					BYTE sleepClock,	BYTE regionCode,
					BYTE PA_VId_B1,		BYTE PA_VId_A1,
					BYTE PA_VId_B0,		BYTE PA_VId_A0,
					BYTE LNA_VId_C1,	BYTE LNA_VId_B1,	
					BYTE LNA_VId_A1,	BYTE LNA_VId_C0,
					BYTE LNA_VId_B0,	BYTE LNA_VId_A0,
					BYTE ANT_VId_C1,	BYTE ANT_VId_B1,	
					BYTE ANT_VId_A1,	BYTE ANT_VId_C0,
					BYTE ANT_VId_B0,	BYTE ANT_VId_A0,

					DWORD sn,			BYTE location,
					BYTE assemHouse,	BYTE spiUnit,
					BYTE spiSize,		BYTE SocMajorVer,
					BYTE SocMinorVer,	WORD socID,
					BYTE RFMajorVer,	BYTE RFMinorVer,
					
					BYTE RFId,			BYTE bbuMajorVer,
					BYTE bbuMinorVer,	BYTE bbuID,
					DWORD testVer,		DWORD mfgFWVer,
					DWORD dllVer,		BYTE btCoexist,
					int *nextPtr=NULL, 
					char *FileName=NULL, 
					int Band=0
					);

DUT_API int STDCALL DutIf_SetCalAnnex14Data(
					  int currentPtr,
					BYTE rfXtal,		BYTE calRange1,
					BYTE calRange2,		BYTE calRange3,
					BYTE calRange4,		BYTE calBandB,
					BYTE calBandG,		BYTE calMethod,
					BYTE pathId,		BYTE deviceId,

					BYTE thermalScaler,	BYTE noChSaved,
					BYTE lcTank,		
					BYTE *chSaved=NULL,	BYTE *lcCal=NULL,
					BYTE *InitP_rng1=NULL,	BYTE *corrBG_rng1 = NULL,
					BYTE *InitP_rng2=NULL,	BYTE *corrBG_rng2 = NULL,
					BYTE *InitP_rng3=NULL,	BYTE *corrBG_rng3 = NULL,

					BYTE *InitP_rng4=NULL,	BYTE *corrBG_rng4 = NULL,
					BYTE IQmis20_ch14_phase=0,	BYTE IQMis20_14_amp=0,
					BYTE IQmis20_ch1_13_phase=0,	BYTE IQMis20_1_13_amp=0,
					BYTE IQmis10_ch14_phase=0,	BYTE IQMis10_14_amp=0,
					BYTE IQmis10_ch1_13_phase=0,	BYTE IQMis10_1_13_amp=0,

					BYTE IQmis5_ch14_phase=0,		BYTE IQMis5_14_amp=0,
					BYTE IQmis5_ch1_13_phase=0,	BYTE IQMis5_1_13_amp=0,
					BYTE IQmis40_ch14_phase=0,	BYTE IQMis40_14_amp=0,
					BYTE IQmis40_ch1_13_phase=0,	BYTE IQMis40_1_13_amp=0,
					int *nextPtr=NULL, char *FileName=NULL);

DUT_API int STDCALL DutIf_SetCalAnnex15Data(
                 	int currentPtr,
					BYTE subBand,
					BYTE rfXtal,		BYTE PAOption,

					BYTE calRange1,
					BYTE calRange2,		BYTE calRange3,
					BYTE calRange4,		BYTE calBandB,
					BYTE calBandG,		BYTE calMethod,
					BYTE pathId,		BYTE deviceId,
					BYTE thermalScaler,	BYTE noChSaved,
		            BYTE InitTrgt,

					BYTE  chSavedSubBand1,	BYTE chSavedSubBand2,
					BYTE  chSavedSubBand3,	BYTE  chSavedSubBand4,
					BYTE  chSavedSubBand5, 
					BYTE  ChannexIndexSaved[16],

					BYTE *InitP_rng1,	BYTE *corrA_rng1,
					BYTE *InitP_rng2,	BYTE *corrA_rng2,
					BYTE *InitP_rng3,	BYTE *corrA_rng3,
					BYTE *InitP_rng4,	BYTE *corrA_rng4,

					BYTE IQmis20_SubBand1_phase,	BYTE IQMis20_SubBand1_amp,
					BYTE IQmis20_SubBand2_phase,	BYTE IQMis20_SubBand2_amp,
					BYTE IQmis20_SubBand3_phase,	BYTE IQMis20_SubBand3_amp,
					BYTE IQmis20_SubBand4_phase,	BYTE IQMis20_SubBand4_amp,

					BYTE IQmis10_SubBand1_phase,	BYTE IQMis10_SubBand1_amp,
					BYTE IQmis10_SubBand2_phase,	BYTE IQMis10_SubBand2_amp,
					BYTE IQmis10_SubBand3_phase,	BYTE IQMis10_SubBand3_amp,
					BYTE IQmis10_SubBand4_phase,	BYTE IQMis10_SubBand4_amp,

					BYTE IQmis5_SubBand1_phase,		BYTE IQMis5_SubBand1_amp,
					BYTE IQmis5_SubBand2_phase,		BYTE IQMis5_SubBand2_amp,
					BYTE IQmis5_SubBand3_phase,		BYTE IQMis5_SubBand3_amp,
					BYTE IQmis5_SubBand4_phase,		BYTE IQMis5_SubBand4_amp,

					BYTE IQmis40_SubBand1_phase,	BYTE IQMis40_SubBand1_amp,
					BYTE IQmis40_SubBand2_phase,	BYTE IQMis40_SubBand2_amp,
					BYTE IQmis40_SubBand3_phase,	BYTE IQMis40_SubBand3_amp,
					BYTE IQmis40_SubBand4_phase,	BYTE IQMis40_SubBand4_amp,
					
					int *nextPtr=NULL, char *FileName=NULL);
DUT_API int STDCALL DutIf_SetCalAnnex17Data(
					    int currentPtr,
					   int UseBBLnaSettings, 
					    
					   BYTE BbuLna_hi_BandG[SUBBAND_NUM_G],			
                       BYTE BbuLna_Lo_BandG[SUBBAND_NUM_G],		
					   BYTE BbuLna_GainOffset_BandG[SUBBAND_NUM_G], 
					   BYTE RfLna_GrBb_BandG[SUBBAND_NUM_G],			
                       BYTE RfLna_GrRfIf_BandG[SUBBAND_NUM_G],  

					   BYTE BbuLna_hi_BandA[SUBBAND_NUM_A],			
                       BYTE BbuLna_Lo_BandA[SUBBAND_NUM_A],		
					   BYTE BbuLna_GainOffset_BandA[SUBBAND_NUM_A], 
					   BYTE RfLna_GrBb_BandA[SUBBAND_NUM_A],			
                       BYTE RfLna_GrRfIf_BandA[SUBBAND_NUM_A], 
					int *nextPtr=NULL,		char *pFileName=NULL );

DUT_API int STDCALL DutIf_SetCalAnnex37Data(
					  int currentPtr,
					BYTE rfXtal,		BYTE calRange1,
					BYTE calRange2,		BYTE calRange3,
					BYTE calRange4,		BYTE calBandB,
					BYTE calBandG,		BYTE calMethod,
					BYTE pathId,		BYTE deviceId,

					BYTE thermalScaler,	BYTE noChSaved,
					BYTE lcTank,		
					BYTE *chSaved=NULL,	BYTE *lcCal=NULL,
					BYTE *InitP_rng1=NULL,	BYTE *corrBG_rng1 = NULL,
					BYTE *InitP_rng2=NULL,	BYTE *corrBG_rng2 = NULL,
					BYTE *InitP_rng3=NULL,	BYTE *corrBG_rng3 = NULL,

					BYTE *InitP_rng4=NULL,	BYTE *corrBG_rng4 = NULL,
					BYTE IQmis20_ch14_phase=0,	BYTE IQMis20_14_amp=0,
					BYTE IQmis20_ch1_13_phase=0,	BYTE IQMis20_1_13_amp=0,
					BYTE IQmis10_ch14_phase=0,	BYTE IQMis10_14_amp=0,
					BYTE IQmis10_ch1_13_phase=0,	BYTE IQMis10_1_13_amp=0,

					BYTE IQmis5_ch14_phase=0,		BYTE IQMis5_14_amp=0,
					BYTE IQmis5_ch1_13_phase=0,	BYTE IQMis5_1_13_amp=0,
					BYTE IQmis40_ch14_phase=0,	BYTE IQMis40_14_amp=0,
					BYTE IQmis40_ch1_13_phase=0,	BYTE IQMis40_1_13_amp=0,
					int *nextPtr=NULL, char *FileName=NULL);

DUT_API int STDCALL DutIf_SetCalAnnex16Data(
					int currentPtr,
					BYTE subBand,	BYTE bandId,
					BYTE pathId,		BYTE deviceId,
					WORD freqLow,		WORD freqHigh,
					BYTE *pwrMin=NULL, BYTE *pwrMax=NULL,
					BYTE *ppa5_2=NULL,	BYTE *ppa5_1=NULL,
					BYTE *initp=NULL,	BYTE *threshLow=NULL,
					BYTE *threshHigh=NULL,
					int *nextPtr=NULL,
					BOOL lastTbl=0, char *FileName=NULL);

DUT_API int STDCALL DutIf_SetCalAnnex26Data(
					int currentPtr,
					BYTE noChSaved,
					BYTE pathId,		BYTE deviceId,
					BYTE *Initp_G_rng1=NULL, BYTE *Initp_G_rng2=NULL, 
					BYTE *Initp_G_rng3=NULL, BYTE *Initp_G_rng4=NULL, 
					int *nextPtr=NULL, char *FileName=NULL);

//
// Annex 40 - External PM support
//
DUT_API int STDCALL DutIf_SetCalAnnex31Data(
					int currentPtr,
					BYTE cal_flag,
					BYTE band,
					BYTE pathId,		BYTE deviceId,
					BYTE *rssi_MCS=NULL, BYTE *rssi_OFDM=NULL, 
					BYTE *rssi_CCK=NULL, BYTE *nf_offset=NULL, 
					int *nextPtr=NULL, char *FileName=NULL);

//
// Annex 40 - External PM support
//
DUT_API int STDCALL DutIf_SetCalAnnex32Data(
					int currentPtr,
					BYTE cal_flag,
					BYTE band,
					BYTE pathId,		BYTE deviceId,
					BYTE *rssi_MCS=NULL, BYTE *rssi_OFDM=NULL, 
					BYTE *rssi_CCK=NULL, BYTE *nf_offset=NULL, 
					int *nextPtr=NULL, char *FileName=NULL);

//
// Annex 40 - External PM support
//
DUT_API int STDCALL DutIf_SetCalAnnex40Data(
					int currentPtr,
					WORD VID,
					WORD PID,
					BYTE chipID,
					BYTE chipVersion,
					BYTE numOfRail,
					BYTE powerDownModeMap,
					BYTE normalRail0,
					BYTE normalRail1,
					BYTE normalRail2,
					BYTE normalRail3,
					BYTE normalRail4,
					BYTE normalRail5,
					BYTE normalRail6,
					BYTE normalRail7,
					BYTE sleepRail0,
					BYTE sleepRail1,
					BYTE sleepRail2,
					BYTE sleepRail3,
					BYTE sleepRail4,
					BYTE sleepRail5,
					BYTE sleepRail6,
					BYTE sleepRail7,
					int *nextPtr=NULL, char *FileName=NULL);

//
// Annex 42 - Thermal Calibration Parameters
//
DUT_API int STDCALL DutIf_SetCalAnnex42Data(
					int currentPtr,
					BYTE noBand,
					BYTE Band1,
					BYTE TempRefMinus40C1,
					BYTE TempRef0C1,
					BYTE TempRef65C1,
					BYTE TempRef85C1,
					BYTE PPACoeff1,
					BYTE PACoeff1,
					BYTE Denom1,
					BYTE Numerator1,
					BYTE Band2,
					BYTE TempRefMinus40C2,
					BYTE TempRef0C2,
					BYTE TempRef65C2,
					BYTE TempRef85C2,
					BYTE PPACoeff2,
					BYTE PACoeff2,
					BYTE Numerator2,
					BYTE Denom2,
					int *nextPtr=NULL, char *FileName=NULL);

DUT_API int STDCALL DutIf_GetCalTbl(DWORD *TblData, DWORD *length,  BOOL *CsC);
DUT_API int STDCALL DutIf_GetCalMainData(
					int *CsC,			BYTE *StructRev,
					BYTE *dsgnVar,		BYTE *dsgnType,
					BYTE *dsgnMjrVer,	BYTE *dsgnMinVer,
					BYTE *pathId,		BYTE *deviceId,
					BYTE *externLNAG1,	BYTE *externLNAA1,
					BYTE *externLNAG0,	BYTE *externLNAA0,

					BYTE *externPolarG1,BYTE *externPolarA1,
					BYTE *externPolarG0,BYTE *externPolarA0,
					BYTE *txAnt1,		BYTE *txAnt0,
					BYTE *rxAnt1,		BYTE *rxAnt0,
					BYTE *WlanWakeup,	BYTE *crystal,

					BYTE *sleepClock,	BYTE *regionCode,
					BYTE *PA_VId_B1,	BYTE *PA_VId_A1,
					BYTE *PA_VId_B0,	BYTE *PA_VId_A0,
					BYTE *LNA_VId_B1,	BYTE *LNA_VId_A1,
					BYTE *LNA_VId_B0,	BYTE *LNA_VId_A0,

					DWORD *sn,			BYTE *location,
					BYTE *assemHouse,	BYTE *spiUnit,
					BYTE *spiSize,		BYTE *SocMajorVer,
					BYTE *SocMinorVer,	BYTE *socID,
					BYTE *RFMajorVer,	BYTE *RFMinorVer,
					
					BYTE *RFId,			BYTE *bbuMajorVer,
					BYTE *bbuMinorVer,	BYTE *bbuID,
					BYTE *testVer,		BYTE *mfgFWVer,
					BYTE *dllVer,		BYTE *btCoexist
					);
DUT_API int STDCALL DutIf_GetHeaderTbl( DWORD *HeaderTbl, DWORD *Version, BOOL *CsC);
DUT_API int STDCALL DutIf_SetHeaderTbl( DWORD *HeaderTbl); 

DUT_API int STDCALL DutIf_DumpE2PData( DWORD E2pOffset,  DWORD Len, 
					 DWORD *AddressArray, DWORD *DwordArray );
DUT_API int STDCALL DutIf_WriteLumpedData( DWORD E2pOffset,  DWORD Len, DWORD *DwordArray );
 
DUT_API int STDCALL DutIf_SpiReload(DWORD LenInDword, DWORD *DwordArray );
DUT_API int STDCALL DutIf_SetRfControlMode(int ThermalOnly);
DUT_API int STDCALL DutIf_GetRfControlMode(int *pThermalOnly);
DUT_API int STDCALL DutIf_SetPaThermalScaler(int scaler);
DUT_API int STDCALL DutIf_GetPaThermalScaler(int *scaler);

DUT_API int STDCALL DutIf_UpLoadPwrTable(int tableId, int *max, int *min, int *sizeOfTblInByte, BYTE* tbl);
DUT_API int STDCALL DutIf_DownLoadPwrTable(int tableId, int max, int min, int sizeOfTblInByte, BYTE* tbl, int UsePassDownData=1);
DUT_API int STDCALL DutIf_UpLoadRangeTable(int tableId, int *sizeOfTblInByte, DWORD* tbl);
DUT_API int STDCALL DutIf_DownLoadRangeTable(int tableId, int *sizeOfTblInByte, DWORD* tbl);
DUT_API int STDCALL DutIf_LoadRangeTableFile(int tableId, int *sizeOfTblInByte, DWORD* tbl);  
 
DUT_API int STDCALL DutIf_LoadCustomizedSettings(void);

DUT_API int STDCALL DutIf_WriteRangeTableFile(int tableId, int *sizeOfTblInByte, int* tbl, char* GivenFileName=NULL); 
DUT_API int STDCALL DutIf_WritePwrTableFile(int tableId, int *max, int *min, 
					  int *sizeOfTblInByte, void* tbl, char* GivenFileName=NULL); 

//jsz DUT_API int STDCALL DutIf_UpLoadSaveCalData(int *CsC);
//jsz DUT_API int STDCALL DutIf_LoadSaveCalData(BYTE band);

DUT_API int STDCALL DutIf_CloseLoopCal(BYTE targetInitP, int maxThreshold, int steps, int dataRate,
									   BYTE *initP, BYTE *threshHi, BYTE *threshLow, BOOLEAN bHost = true);

DUT_API int STDCALL DutIf_ThermalComp(int enabled, BYTE tempRefAtMinus40C,
									  BYTE SlopeNumerator, 
									  BYTE SlopeDenominator,
									  BYTE CalibrationIntervalInMS, 
									  BYTE PPACoeff, BYTE PACoeff);

DUT_API int STDCALL DutIf_TxFastCal(BYTE *TxCalDataStruc=NULL, int len=0);
DUT_API int STDCALL DutIf_TxFastVer(BYTE *TxVerDataStruc=NULL, int len=0);
DUT_API int STDCALL DutIf_RxFastVer(BOOL enable, int channel, BYTE *Bssid=0, char *SSID=0);

DUT_API int STDCALL DutIf_SetLDOConfig(int);
DUT_API int STDCALL DutIf_GetLDOConfig(int*);

DUT_API int STDCALL DutIf_WritePMReg(int addr, int value);
DUT_API int STDCALL DutIf_ReadPMReg(int addr, int *value);
DUT_API int STDCALL DutIf_SetPMRail(int railId, int mode, int volt, int powerDown);

#ifdef _INSTRUMENT_
DUT_API int STDCALL DutIf_RXFER(int, int, int,int);
#endif	//_INSTRUMENT_

#endif //_DUTAPI83XXPDLL_H_
