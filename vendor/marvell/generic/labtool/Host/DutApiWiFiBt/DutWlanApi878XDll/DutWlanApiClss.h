/** @file DutWlanApiClss.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#ifndef _DUT_WLAN_API_H_
#define _DUT_WLAN_API_H_
#include "../../DutCommon/Dut_error.hc"
#include "../../DutCommon/Clss_os.h"
#include "../../DutCommon/utilities_os.h"
#include "../../DutCommon/utilities.h"

#include "DutIf_IfClss.h"
#include DUTCLASS_SHARED_H_PATH	//"../DutShareApi878XDll/DutSharedClss.h"

#ifndef _LINUX_
class DutWlanApiClss :DutIf_If 
{
char adapter_prefix[256];
#endif	//#ifndef _LINUX_


#ifndef _NO_CLASS_
friend  class DutSharedClss;
#endif // #ifndef _NO_CLASS_  	
int Cf_OpenCfDevice( );
 
int Cf_CloseCfDevice( );

BYTE DoCs1(BYTE *TblData, DWORD length);
void DoCs1Check(BYTE *TblData, DWORD length, BOOL *CsC);

int Cf_SetModuleTypeConf(int ModuleType);
int Cf_GetModuleTypeConf(int *pModuleType);

int Dev_SetExtLnaNsettings(int mode, int LnaPolority,
                       BYTE Bbu_Hi, BYTE Bbu_Lo,
                       BYTE Bbu_GainOffset,
                       BYTE Rfu_GrBb, BYTE Rfu_GrLpf);

int Dev_GetExtLnaNsettings(int *pMode, int *pLnaPolority,
                       BYTE *pBbu_Hi, BYTE *pBbu_Lo,
                       BYTE *pBbu_GainOffset,
                       BYTE *pRfu_GrBb, BYTE *pRfu_GrLpf);

int Cf_Poke( DWORD address, DWORD data, DWORD DeviceId);
int Cf_Peek( DWORD address, DWORD *data, DWORD DeviceId);

int Wifi_SpiWriteData(DWORD address, DWORD lenInDw, DWORD *data, 
				 BOOL SectorErase);
int Wifi_SpiReadData(DWORD address, DWORD lenInDw, DWORD *data);   

int Cf_PokeRegDword(DWORD address, DWORD data, DWORD DeviceId);
int Cf_PeekRegDword(DWORD address, DWORD *data, DWORD DeviceId);
int Cf_BbpPeek(DWORD address, DWORD *data, DWORD DeviceId);   
int Cf_BbpPoke(DWORD address, DWORD data, DWORD DeviceId);   
int Cf_RfPeek(DWORD address, DWORD *data, DWORD DeviceId);   
int Cf_RfPoke(DWORD address, DWORD data, DWORD DeviceId);   

int Cf_CAUPeek(DWORD address, DWORD *data, DWORD DeviceId);   
int Cf_CAUPoke(DWORD address, DWORD data, DWORD DeviceId);   
int Cf_LoadRFUFW(DWORD deviceId, DWORD offset, DWORD sizeBytes, BYTE *pSqBufAddr);
int Cf_UpdateRFUFW(DWORD deviceId);

int Cf_PokeRegWord(DWORD address, WORD data, DWORD DeviceId);
int Cf_PeekRegWord(DWORD address, WORD *data, DWORD DeviceId);
int Cf_PokeRegByte(DWORD address, BYTE data, DWORD DeviceId);
int Cf_PeekRegByte(DWORD address, BYTE *data, DWORD DeviceId);

int Cf_FWVesion(BYTE *version);   
int Cf_FWVerNumber(DWORD *FWVersion,DWORD *MFGFWVersion);   
int Cf_GetReleaseNote(char *ReleaseNote);   

int Cf_ReadMacRegister(DWORD offset,	DWORD* data); 
int Cf_WriteMacRegister(DWORD offset,	DWORD  data); 

int Cf_ReadBbpRegister(int offset,	BYTE* data); 
int Cf_WriteBbpRegister(int offset, BYTE  data); 

int Cf_ReadRfRegister(int offset,	BYTE* data); 
int Cf_WriteRfRegister(int offset,	BYTE  data); 

int Cf_GetTxAnt(int *antSelection, DWORD DeviceId);
int Cf_SetTxAnt(int antSelection, DWORD DeviceId); 
int Cf_GetRxAnt(int *antSelection, DWORD DeviceId); 
int Cf_SetRxAnt(int antSelection, DWORD DeviceId); 

int Cf_GetTxDataRate(int *DataRate, DWORD DeviceId); 
int Cf_SetTxDataRate(int DataRate, DWORD DeviceId);

int Cf_Enable22M(BOOL mode ); 
int Cf_Enable72M(BOOL mode ); 

int Cf_SetTxContMode(	BOOL enable, BOOL CwMode,
						DWORD payloadPattern,
						DWORD		CSmode,
						DWORD		ActSubCh, 
						DWORD 		DeviceId);
int Cf_SetTxCarrierSuppression(BOOL enable,	DWORD 		DeviceId);
int SetIQCalMode(BOOL enable);
 
int Cf_SetTxDutyCycle (int enable,
		int dataRate,	
		int percentage, 
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
		) ;
int Cf_SetPowerCycleMode( 
						int cycle, 
						int sleepDur, 
						int awakeDur, 
						int stableXosc,  
						DWORD DeviceId);   
int Cf_EnterSleepMode(void);
int Cf_GetBeaconInterval( DWORD *interval, DWORD DeviceId);
int Cf_SetBeaconInterval( DWORD interval, DWORD DeviceId);

int Cf_GetBeaconEnable(BOOL *enable, DWORD DeviceId); 
int Cf_SetBeaconEnable(BOOL enable, DWORD DeviceId);

int Dev_SetBandAG(int band, DWORD DeviceId);
int Dev_GetBandAG(int *band, DWORD DeviceId);

int Cf_GetRfChannel(int *channel, double *pChFreqInMHz,DWORD DeviceId); 
int Cf_SetRfChannel(int channel, DWORD DeviceId); 

int Dev_SetChannelBw(int ChannelBw , DWORD DeviceId=MIMODEVICE0);

int Dev_GetChannelBw(int *ChannelBw,  DWORD DeviceId=MIMODEVICE0);
int Dev_GetSubBand(int *SubBandId, int *HighFreq=NULL, int *LowFreq=NULL,  
					DWORD DeviceId=MIMODEVICE0);

int Dev_GetRfChannelFreq(int band, int channel, double *chFreqInMhz,  
					DWORD DeviceId=MIMODEVICE0);
int Dev_SetRfChannelByFreq(double chFreqInMhz, DWORD DeviceId=MIMODEVICE0);

int Cf_SetRfPowerRange(int pwrPnt
#if defined (_MIMO_)					
					  ,
					  DWORD PathId=TXPATH_PATHA,  
					  DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
					  );

int Cf_GetRfPowerRange(int *pRangeIndex, int *pTopPwr, int *pBtmPwr
#if defined (_MIMO_)					
					  ,
					  DWORD PathId=TXPATH_PATHA,  
					  DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
					  );

int Cf_GetRfPowerSetting(BYTE *pPaConf1,	BYTE *pPaConf2,
						 BYTE *pVt_Hi_G,	BYTE *pVt_Lo_G,		BYTE *pInitP_G, 
						 BYTE *pVt_Hi_B,	BYTE *pVt_Lo_B,		BYTE *pInitP_B);

int Cf_SpiPoke(int deviceType, void *pObj, DWORD address, DWORD *data, DWORD len, BOOL sec_erase);
int Cf_SpiPeek(int deviceType, void *pObj, DWORD address, DWORD *data, DWORD len);   
int Dvc_GetCckFltScaler( 
					int *pScalerMode,  
					int DeviceId=MIMODEVICE0);

int Dvc_SetCckFltScaler( 
					int ScalerMode,  
					int DeviceId=MIMODEVICE0);

int Cf_SetRfPower(	int *pPwr4Pa, 
					int *pCorrOffset, 
					int *pCorr1A, 
					DWORD PathId,  
					DWORD DeviceId);

int Cf_StepRfPower(	int *pSteps, 
					DWORD PathId=TXPATH_PATHA,
					DWORD DeviceId=MIMODEVICE0);


int Cf_GetRfPower(	int *pPwr4Pa,  
					int	*pCorrInitP_G,
					int	*pCorrInitP_B,
					int	*pCorrTh_G,
					int	*pCorrTh_B);

int Cf_SetRfVga( int extMode, int value, 
				DWORD PathId, DWORD DeviceId);
int Cf_GetRfVga( int *pExtMode, int *pValue, 
				DWORD PathId, DWORD DeviceId);
int Cf_SetRfLcCap( int extMode, int value);
int Cf_GetRfLcCap( int *pExtMode, int *pValue);
int Cf_SetRfXTal(int Extension, int Cal, DWORD DeviceId);
int Cf_GetRfXTal(int *pExtension, int *pCal, DWORD DeviceId);

int Cf_ClearRxPckt(DWORD DeviceId);

int Cf_GetRxPckt(unsigned long *cntRx,
				 unsigned long *cntOk,
				 unsigned long *cntKo,
				 DWORD DeviceId); 
int Cf_GetHWVersion(WORD *socId, BYTE *socVersion, 
									   BYTE *RfId, BYTE *RfVersion, 
									   BYTE *BbId, BYTE *BbVersion,
									   DWORD DeviceId);
int Cf_GetORVersion(BYTE OR_ID, BYTE *OR_Major, BYTE *OR_Minor, BYTE *OR_Cust);


int Cf_MacWakeupRfBBP(); 

int Cf_AdjustPcktSifs(		DWORD Enable, 
							DWORD dataRate,	DWORD pattern, 
							DWORD length,		
							int ShortPreamble, char *Bssid	
#if defined (_MIMO_)
					,
							DWORD		ActSubCh=3, 
							DWORD		ShortGI=0, 
							DWORD		AdvCoding=0,  
							DWORD		TxBfOn=0,  
							DWORD		GFMode=0, 
							DWORD		STBC=0, 
							DWORD		DPDEnable=0,
							DWORD		PowerID=0,
							DWORD		SignalBw=0,
#if defined (_W8787_) || 	defined (_W8782_)						
							DWORD	AdjustTxBurstGap=false, 
							DWORD	BurstSifsInUs=0, 
#endif //#if defined (_W8787_) || 	defined (_W8782_)						
							DWORD 		DeviceId=MIMODEVICE0
#endif // #if defined (_MIMO_)
					);

int Cf_TxBrdCstPkts(DWORD dataRate,	DWORD pattern, 
							DWORD length,		DWORD count, 
							int ShortPreamble, char *Bssid	
#if defined (_MIMO_)
					,
							DWORD		ActSubCh=3, 
							DWORD		ShortGI=0, 
							DWORD		AdvCoding=0,  
							DWORD		TxBfOn=0,  
							DWORD		GFMode=0, 
							DWORD		STBC=0, 
							DWORD		DPDEnable=0,
							DWORD		PowerID=0,
							DWORD 		DeviceId=MIMODEVICE0
#endif // #if defined (_MIMO_)
					);
int Cf_XoscCal(WORD *cal, DWORD TU, DWORD DeviceId);   
int Cf_EnableBssidFilter(int mode, BYTE *Bssid, char *SSID, DWORD DeviceId);

int Cf_TxFastCal(BYTE *DataStruc=NULL, int len=0);
int Cf_TxFastVer(BYTE *DataStruc=NULL, int len=0);
int Cf_RxFastVer(BOOL enable, int channel, BYTE *Bssid=0, char *SSID=0);

int Dev_SoftReset(DWORD DeviceId);
int Dev_HwResetRf(DWORD DeviceId);
int Dev_SetRssiNf(DWORD DeviceId);
int Dev_GetRssiNf(int stop, 
				  int *count, int *rssi, 
				  int *SNR, int *NoiseFloor, 
				  DWORD DeviceId);

int Dev_ArmReadUlongArray(DWORD armAddr, 
						  DWORD *pBuf, 
						  DWORD sizeBytes, 
						  DWORD DeviceId);

//#if defined(_ENGINEERING_)
int Dev_RunBbpSqDiag(DWORD sizeBytes, DWORD *pSqBufAddr);
//#endif //#if defined(_ENGINEERING_)


int Dev_ThermalTest(BOOL enabled, 
					DWORD PTarget,
					DWORD tempRef, 
					DWORD SlopeNumerator, 
					DWORD SlopeDenominator,
					DWORD CalibrationIntervalInMS,					
					DWORD Temp, 
					DWORD SlopeNumerator1, 
					DWORD SlopeDenominator1);

int Dev_ThermalComp(	BOOL enabled, 
						DWORD tempRefAtCal,
						DWORD tempRefSlope,
						DWORD SlopeNumerator, 
						DWORD SlopeNumerator1, 
						DWORD SlopeDenominator,
						DWORD CalibrationIntervalInMS, 
						DWORD PTargetDelta, 
						DWORD PathId=TXPATH_PATHA,  
						DWORD DeviceId=MIMODEVICE0);

int Dev_GetThermalComp(	BOOL *pEnabled, 
						DWORD *pTempRefAtCal,
						DWORD *pTempRefSlope,
						DWORD *pSlopeNumerator, 
						DWORD *pSlopeNumerator1, 
						DWORD *pSlopeDenominator,
						DWORD *pCalibrationIntervalInMS, 
						DWORD *pPTargetDelta, 
						DWORD PathId=TXPATH_PATHA,  
						DWORD DeviceId=MIMODEVICE0);

int Dev_GetThermalTest(	BOOL *pEnabled, 
						DWORD *pPTarget,
						DWORD *pTempRef,
						DWORD *pSlopeNumerator, 
						DWORD *pSlopeDenominator,
						DWORD *pCalibrationIntervalInMS, 
						DWORD *pTemp,
						DWORD *pSlopeNumerator1, 
						DWORD *pSlopeDenominator1);

int Dev_GetThermalSensorReading(int *pReadBack);

#if defined (_SUPPORT_LDO_)
int Dev_SetLDOConfig(int LDOSrc);
int Dev_GetLDOConfig(int *LDOSrc);
#endif // #if defined (_SUPPORT_LDO_)
#if defined (_SUPPORT_PM_)
int Dev_WritePMReg(int addr, int value);
int Dev_ReadPMReg(int addr, int *value);
int Dev_SetPMRail(int railId, int mode, int volt, int powerDown);
#endif // #if defined (_SUPPORT_PM_)
int Dev_SetRfPowerUseCal(int *pPwr4Pa , int RateG=0, 
						BYTE *cal_data_ext=NULL, int len=0,
						DWORD PathId=TXPATH_PATHA,  
						DWORD DeviceId=MIMODEVICE0);

int Dev_SetRfControlMode(DWORD ThermalOnly, DWORD DeviceId=MIMODEVICE0);
int Dev_GetRfControlMode(DWORD *pThermalOnly, DWORD DeviceId=MIMODEVICE0);
int Dev_ForceE2PromType(DWORD IfType, 
						DWORD AddrWidth, 
						DWORD DeviceType);

int Dev_GetE2PromType(DWORD *pIfType, 
					  DWORD *pAddrWidth, 
					  DWORD *pDeviceType);

int Dev_SetFEM(DWORD FE_VER,
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

int Dev_ReloadCalData();

int Dev_SetRfPwrInitP(	BYTE InitP, 
						DWORD PathId=TXPATH_PATHA,	
						DWORD DeviceId=MIMODEVICE0);

int Dev_GetRfPwrVgaMax(	BYTE *pVgaMax, 
						DWORD PathId=TXPATH_PATHA,	
						DWORD DeviceId=MIMODEVICE0);
int Dev_UpLoadPwrTable(int bandId, int tableId,  
					   int *sizeOfTblInByte, BYTE* tbl,		DWORD PathId,
						DWORD DeviceId);
int Dev_DownLoadPwrTable(int bandId, int tableId,   
						 int *sizeOfTblInByte, BYTE* tbl,		DWORD PathId,
						DWORD DeviceId);

int Dev_GetRfPwrTbl(int band,	int SubBandId,	 
					int *sizeOfTble,	BYTE *tbl,		
					DWORD PathId,
					DWORD DeviceId); 

int Dev_SetRfPwrTbl(int band,	int SubBandId, 
					int sizeOfTble,		BYTE *tbl,		
					DWORD PathId,
					DWORD DeviceId); 

int Wifi_EraseSect(DWORD startAddress);
int Write3Add4Data(DWORD Address, DWORD data);
int Read3Add4Data(DWORD Address, DWORD *data);
int  Cf_SpiDownload(DWORD LenInDW, DWORD *DwordArray );

int SetRfBbFiler(int channel);
int Dev_LoadCustomizedSettings(void); 

int Dev_SetCustomizedSettings(BYTE* cal_data_ext, int len);
int Dev_GetCustomizedSettings(int *Setting);

int Dev_SetMACAddress(BYTE *ByteArray);
int Dev_GetMACAddress(BYTE* ByteArray);

//DPD API
#if defined(_W8782_)

int Dev_SetDPD(DWORD PowerID, DWORD Memory2Row, DWORD *pDPDTbl, DWORD DeviceId);
int Dev_GetDPD(DWORD PowerID, DWORD Memory2Row, DWORD *pDPDTbl, DWORD DeviceId);

#endif //#if defined(_W8782_)

#ifndef _LINUX_
public:
#endif //#ifndef _LINUX_

int SetExtLnaNsettings(int mode, int LnaPolority,
                       BYTE Bbu_Hi, BYTE Bbu_Lo,
                       BYTE Bbu_GainOffset,
                       BYTE Rfu_GrBb, BYTE Rfu_GrLpf);

int GetExtLnaNsettings(int *pMode, int *pLnaPolority,
                       BYTE *pBbu_Hi, BYTE *pBbu_Lo,
                       BYTE *pBbu_GainOffset,
                       BYTE *pRfu_GrBb, BYTE *pRfu_GrLpf);

int PokeRegDword(DWORD address, DWORD data, DWORD DeviceId);
int PeekRegDword(DWORD address, DWORD *data, DWORD DeviceId);

int PokeRegWord(DWORD address, WORD data, DWORD DeviceId);
int PeekRegWord(DWORD address, WORD *data, DWORD DeviceId);
int PokeRegByte(DWORD address, BYTE data, DWORD DeviceId);
int PeekRegByte(DWORD address, BYTE *data, DWORD DeviceId);

int WriteCAUReg(WORD address, BYTE data, DWORD DeviceId); 
int ReadCAUReg(WORD address, BYTE *data, DWORD DeviceId); 
int LoadRFUFW(DWORD deviceId, char *FileName);
int UpdateRFUFW(DWORD deviceId);

int WriteRfReg(WORD address, BYTE data, DWORD DeviceId); 
int ReadRfReg(WORD address, BYTE *data, DWORD DeviceId); 
int WriteBBReg(WORD address, BYTE data, DWORD DeviceId); 
int ReadBBReg(WORD address, BYTE *data, DWORD DeviceId); 
int WriteMACReg(DWORD address, DWORD data, DWORD DeviceId);	 
int ReadMACReg(DWORD address, DWORD *data, DWORD DeviceId);  
int WriteSocReg(DWORD address, DWORD data, DWORD DeviceId);	 
int ReadSocReg(DWORD address, DWORD *data, DWORD DeviceId);  
 
int WriteSQmem(DWORD address, DWORD data, DWORD DeviceId);
int ReadSQmem(DWORD address, DWORD *data, DWORD DeviceId);

#ifndef _LINUX_
DutWlanApiClss();
~DutWlanApiClss();
#endif //#ifndef _LINUX_
int  Version(void);
char* About(void);

int InitConnection(void);		 
int Disconnection(void);	  

int SetBandAG(int band, DWORD DeviceId);
int GetBandAG(int *band, DWORD DeviceId);
 
// int set channel, set power, read RSSI, read .11status, 
int SetRfChannel(int channel, DWORD DeviceId);
int GetRfChannel(int *channel, double *pChFreqInMHz, DWORD DeviceId);

int SetChannelBw(int ChannelBw , DWORD DeviceId=MIMODEVICE0);

int GetChannelBw(int *ChannelBw,  DWORD DeviceId=MIMODEVICE0);
int GetSubBand(int *SubBandId, int *HighFreq=NULL, int *LowFreq=NULL,  
					DWORD DeviceId=MIMODEVICE0);

int GetRfChannelFreq(int band, int channel, double *chFreqInMhz,  
					DWORD DeviceId=MIMODEVICE0);
int SetRfChannelByFreq(double chFreqInMhz, DWORD DeviceId=MIMODEVICE0);

int SetRfPowerRange(int pwrPnt=0
#if defined (_MIMO_)
					,
					DWORD PathId=TXPATH_PATHA,  
					DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
					);
int GetRfPowerRange(int *pRangeIndex=NULL, int *pTopPwr=NULL, int *pBtmPwr=NULL
#if defined (_MIMO_)
					,
											  DWORD PathId=TXPATH_PATHA,  
											  DWORD DeviceId=MIMODEVICE0
#endif //#if defined (_MIMO_)
											  );
int SetRfPower( int *pPwr4Pa, 
				int *pCorrOffset, 
				int *pCorr1A, 
				DWORD PathId,  
				DWORD DeviceId);

int SetRfPowerCal( 
				  int *pPwr4Pa, 
				  int RateG,
				  BYTE* cal_data_ext,
				  int len
				  );
int StepRfPower(int *pSteps , 
				DWORD PathId=TXPATH_PATHA,
				DWORD DeviceId=MIMODEVICE0);
int GetRfPowerSetting(BYTE *pPaConf1, BYTE *pPaConf2, 
						 BYTE *pVt_Hi_G,	 BYTE *pVt_Lo_G,		BYTE *pInitP_G, 
						 BYTE *pVt_Hi_B,	 BYTE *pVt_Lo_B,		BYTE *pInitP_B);

int GetRfPower(	int *pPwr4Pa,  
				int	*pCorrInitP_G,
				int	*pCorrInitP_B,
				int	*pCorrTh_G,
				int	*pCorrTh_B);

int SetRfVga(int extMode, int value, 
											  DWORD PathId,  
											  DWORD DeviceId);
int GetRfVga(int *pExtMode, int *pValue, 
											  DWORD PathId,  
											  DWORD DeviceId);
int SetRfXTal(int Extension, int Cal, DWORD DeviceId);
int GetRfXTal(int *pExtension, int *pCal, DWORD DeviceId);
 
int ClearRxPckt(DWORD DeviceId);
int GetRxPckt(	unsigned long *cntRx,
				unsigned long *cntOk,
				unsigned long *cntKo,
				DWORD DeviceId); 


int SetRxAntenna(int antSelection, DWORD DeviceId); 
int SetTxAntenna(int antSelection, DWORD DeviceId); 

int GetRxAntenna(int *antSelection, DWORD DeviceId); 
int GetTxAntenna(int *antSelection, DWORD DeviceId); 

int SetTxDataRate(int rate, DWORD DeviceId);
int GetTxDataRate(int *rate, DWORD DeviceId);

int SetTxContMode(BOOL enable, BOOL CWmode, 
				DWORD payloadPattern=0,
				DWORD		CSmode=0,
				DWORD		ActSubCh=ACTSUBCH_40MHZ_BOTH, 
				DWORD 		DeviceId=MIMODEVICE0);  
int SetTxCarrierSuppression(BOOL enable, DWORD 		DeviceId);  
int SetTxIQCalMode(BOOL enable, DWORD DeviceId=MIMODEVICE0);  
int GetTxIQValue(int *pExtMode  , DWORD *pAmp, DWORD *pPhase, 
				DWORD PathId=TXPATH_PATHA,  
				DWORD DeviceId=MIMODEVICE0);
int SetTxIQValue(int ExtMode  , DWORD Amp, DWORD Phase, 
				DWORD PathId=TXPATH_PATHA,  
				DWORD DeviceId=MIMODEVICE0);

int GetTxIQEstimatedValue(DWORD *pAmp, DWORD *pPhase);
int PerformTxIQCal(DWORD Avg, DWORD *pTxAmp, DWORD *pTxPhase );
int PerformTxIQCal_Host(DWORD Avg, DWORD *pTxAmp, DWORD *pTxPhase );

int GetRxIQ(int *pExtMode  , DWORD *pAmp, DWORD *pPhase, 
			DWORD PathId=TXPATH_PATHA,  
			DWORD DeviceId=MIMODEVICE0);
int SetRxIQ(int ExtMode  , DWORD Amp, DWORD Phase, 
			DWORD PathId=TXPATH_PATHA,  
			DWORD DeviceId=MIMODEVICE0);
int GetRxIQEstimated(DWORD *pAmp, DWORD *pPhase, 
						DWORD PathId=TXPATH_PATHA,  
						DWORD DeviceId=MIMODEVICE0);
int PerformTxRxIQCal(	DWORD Avg,
						DWORD *pTxAmp, DWORD *pTxPhase, 
						DWORD *pRxAmp, DWORD *pRxPhase);  
void InitB4IQCal();
int rfu_wait_for_signal(int addr, int sigWait);
int rfu_wait_for_intr(int intrWait);
int LO_program(int centerFreq, /* in MHz */
               int RefClockHKHz,   /* In 100 kHz */
               int isDiv2);
int rf_set_channel(int aMode, int chnl);
int lo_leak_cal(int *pLO_I, int *pLO_Q);
int coldpowerupcal();
int rfu_powerup_cal(int aMode, int chnl,
					int *TxAmp, int *TxPhase, 
					int *RxAmp, int *RxPhase);
int cal_init();
int txiq_rxiq1_cal(int *pTX_Alpha, int *pTX_Beta);
int rxiq1_cal(int *pTX_Alpha, int *pTX_Beta);
int rxiq2_cal(int *pRX_Alpha, int *pRX_Beta);
int IQCal_restore();

int PerformTxRxIQCal_Host(	DWORD Avg,
						DWORD *pTxAmp, DWORD *pTxPhase, 
						DWORD *pRxAmp, DWORD *pRxPhase);  

int SetTxDutyCycle(BOOL enable, int dataRate=4,  int Percentage=50, int PayloadLengthInKB=1,
					int payloadPattern=0,	int ShortPreamble=0							
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

int SetPowerCycleMode(int cycle, 
							 int sleepDur, 
							 int awakeDur, 
							 int stableXosc,  
							DWORD DeviceId);   
int EnterSleepMode(void);
int SetRssiPwrOption(int Ant, BOOL AbsolutePwr);	//ant : 0 ant-1, 1 ant-2, 2 dynamic/active,  
													//use absolute power
int GetRssiPwr(BYTE *reading);
int SetRxEDThrs(int Thrs=0);	//Thrs :  0 = 0 Low, 1 = 20H M, 2 = 3FH H

int SetBeaconInterval(DWORD interval, DWORD DeviceId); //in ms
int GetBeaconInterval(DWORD *interval, DWORD DeviceId);//in ms
int SetBeaconEnable(BOOL enable, DWORD DeviceId);
int GetBeaconEnable(BOOL *enable, DWORD DeviceId);

int GetHWVersion(WORD *socId, BYTE *socVersion, 
									   BYTE *RfId, BYTE *RfVersion, 
									   BYTE *BbId, BYTE *BbVersion,
									   DWORD DeviceId);
int GetORVersion(BYTE OR_ID, BYTE *OR_Major, BYTE *OR_Minor, BYTE *OR_Cust);
int GetFWVersion(BYTE *version);
int GetFWVerNumber(DWORD *FWVersion,DWORD *MFGFWVersion);
int GetReleaseNote(char *ReleaseNote);   

int MacWakeupRfBBP(void);
int EnableBssidFilter(int mode=0, BYTE *Bssid=0, char *SSID=0,  
						DWORD DeviceId=MIMODEVICE0);
int AdjustPcktSifs(			DWORD Enable, 
							DWORD dataRate,	DWORD pattern, 
							DWORD length,
							int ShortPreamble, char *Bssid	
#if defined (_MIMO_)
					,
							DWORD		ActSubCh=3, 
							DWORD		ShortGI=0, 
							DWORD		AdvCoding=0,  
							DWORD		TxBfOn=0,  
							DWORD		GFMode=0, 
							DWORD		STBC=0, 
							DWORD		DPDEnable=0,
							DWORD		PowerID=0,
							DWORD		SignalBw=0,
#if defined (_W8787_) || 	defined (_W8782_)						
							DWORD	AdjustTxBurstGap=false, 
							DWORD	BurstSifsInUs=0, 
#endif //#if defined (_W8787_) || 	defined (_W8782_)						
							DWORD 		DeviceId=MIMODEVICE0
#endif // #if defined (_MIMO_)
					);

int TxBrdCstPkts(DWORD dataRate,	DWORD pattern, 
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
int XoscCal(WORD *cal, int TU, DWORD DeviceId); 

int SoftReset(DWORD DeviceId);
int HwResetRf(DWORD DeviceId);
int SetRssiNf(DWORD DeviceId);
int GetRssiNf(int stop, 
				  int *count, int *rssi, 
				  int *SNR, int *NoiseFloor,
				  DWORD DeviceId);

int ArmReadUlongArray(DWORD armAddr, 
						  DWORD *pBuf, 
						  DWORD sizeBytes, 
						  DWORD deviceId);

//#if defined(_ENGINEERING_)
int RunBbpSqDiag(DWORD sizeBytes, DWORD *pSqBufAddr);
//#endif //#if defined(_ENGINEERING_)


int ThermalComp(BOOL enabled, 
				DWORD tempRefAtCal,
				DWORD tempRefSlope,
				DWORD SlopeNumerator, 
				DWORD SlopeNumerator1, 
				DWORD SlopeDenominator,
				DWORD CalibrationIntervalInMS, 
				DWORD PTargetDelta, 
				DWORD PathId=TXPATH_PATHA,  
				DWORD DeviceId=MIMODEVICE0);
int GetThermalComp(	BOOL *pEnabled, 
					DWORD *pTempRefAtCal,
					DWORD *pTempRefSlope,
					DWORD *pSlopeNumerator, 
					DWORD *pSlopeNumerator1, 
					DWORD *pSlopeDenominator,
					DWORD *pCalibrationIntervalInMS, 
					DWORD *pPTargetDelta, 
					DWORD PathId=TXPATH_PATHA,  
					DWORD DeviceId=MIMODEVICE0);
int GetThermalSensorReading(int *pReadBack);

int ThermalTest(BOOL enabled, 
				DWORD PTarget,
				DWORD tempRef, 
				DWORD SlopeNumerator, 
				DWORD SlopeDenominator,
				DWORD CalibrationIntervalInMS,
				DWORD Temp, 
				DWORD SlopeNumerator1, 
				DWORD SlopeDenominator1);

int GetThermalTest(	BOOL *pEnabled, 
					DWORD *pPTarget,
					DWORD *pTempRef,
					DWORD *pSlopeNumerator, 
					DWORD *pSlopeDenominator,
					DWORD *pCalibrationIntervalInMS, 
					DWORD *pTemp,
					DWORD *pSlopeNumerator1, 
					DWORD *pSlopeDenominator1);

#if defined (_SUPPORT_LDO_)
int SetLDOConfig(int LDOSrc);
int GetLDOConfig(int *LDOSrc);
#endif // #if defined (_SUPPORT_LDO_)

#if defined (_SUPPORT_PM_)
int WritePMReg(int addr, int value);
int ReadPMReg(int addr, int *value);
int SetPMRail(int railId, int mode, int volt, int powerDown);
#endif // #if defined (_SUPPORT_PM_)
int SetRfPowerUseCal(int *pPwr4Pa , int RateG=0, 
						BYTE *cal_data_ext=NULL, int len=0,
						DWORD PathId=TXPATH_PATHA,  
						DWORD DeviceId=MIMODEVICE0);

 
int Wifi_EraseEEPROM(int section);

int GetMACAddress(void  *pObj, BYTE *ByteArray);
int SetMACAddress(void  *pObj, BYTE *ByteArray);
int SetPIDVID(void  *pObj, DWORD E2pOffset,  WORD PID, WORD VID);
int GetPIDVID(void  *pObj, DWORD E2pOffset,  WORD *PID, WORD *VID);
int SetClassId(void  *pObj, DWORD E2pOffset,  DWORD Class);
int GetClassId(void  *pObj, DWORD E2pOffset,  DWORD *Class);

int SetRfControlMode(DWORD ThermalOnly, DWORD DeviceId=MIMODEVICE0);
int GetRfControlMode(DWORD *pThermalOnly, DWORD DeviceId=MIMODEVICE0);
int ForceE2PromType(DWORD IfType, DWORD AddrWidth, DWORD DeviceType);
int GetE2PromType(DWORD *pIfType, DWORD *pAddrWidth, DWORD *pDeviceType);

int SetFEM(	DWORD FE_VER,
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

int ReloadCalData();

int SetRfPwrInitP(	BYTE InitP, 
						DWORD PathId=TXPATH_PATHA,	
						DWORD DeviceId=MIMODEVICE0);

int GetRfPwrVgaMax(	BYTE *pVgaMax, 
						DWORD PathId=TXPATH_PATHA,	
						DWORD DeviceId=MIMODEVICE0);

int DownLoadStaticSettingsToFw(DWORD BandId, DWORD SubbandId, 
											   DWORD PathId, 
 									BYTE RfLnaGrif,
 									BYTE RfLnaGrBb,
									DWORD DeviceId);

int UpLoadStaticSettingsFromFw(DWORD BandId, DWORD SubbandId, 
											   DWORD PathId, 
 									BYTE *pRfLnaGrif,
 									BYTE *pRfLnaGrBb,
									DWORD DeviceId);

int Dev_DownLoadStaticSettingsToFw(DWORD BandId, DWORD SubbandId, 
											   DWORD PathId, 
 									BYTE RfLnaGrif,
 									BYTE RfLnaGrBb,
									DWORD DeviceId);

int Dev_UpLoadStaticSettingsFromFw(DWORD BandId, DWORD SubbandId, 
											   DWORD PathId, 
 									BYTE *pRfLnaGrif,
 									BYTE *pRfLnaGrBb,
									DWORD DeviceId);

int GetPwrTbleFileName(	int deviceId, 
						int pathId, int bandId, int subbandId,  
						int Upload, char* GivenFileName);
 
int LoadPwrTableFile(int deviceId, int pathId, int bandId, int subbandId,  
					   int *sizeOfTblInByte, BYTE* pwrTbl, char* GivenFileName=NULL); 

int UpLoadPwrTable  (int bandId, int tableId,   
					 int *sizeOfTblInByte, BYTE* tbl,		
					 DWORD PathId=TXPATH_PATHA,
					DWORD DeviceId=MIMODEVICE0);

int DownLoadPwrTable(int bandId, int tableId,  
					 int *sizeOfTblInByte, BYTE* tbl, int UsePassDownData=1,		
					 DWORD PathId=TXPATH_PATHA,
					DWORD DeviceId=MIMODEVICE0);

int LoadCustomizedSettings(void);

int SetCustomizedSettings(char *FileName=NULL);

int GetCustomizedSettings(int *Setting);
 
int SaveCalDataExt(char *pFileName, BYTE *DataArray, DWORD lenInByte, BOOL newFile=0, int totalLen=0); 
int query1(ULONG OID, PUCHAR pInfo, UINT len, UINT mfg_rsp);

int TxFastCal(BYTE *DataStruc, int len);
int TxFastVer(BYTE *DataStruc, int len);
int RxFastVer(BOOL enable, int channel, BYTE *Bssid=0, char *SSID=0);

int SetModuleTypeConf(int ModuleType);
int GetModuleTypeConf(int *pModuleType);

//RSSI cal
#if defined (_W8787_)
int WlanRSSI_CalOneCh_CW(int RSSIch, 
						 char *RSSIval,
						 char *NFval,
						 int Band, 	DWORD DeviceId);
						 
int WlanRSSI_CalOneCh_Pkt(int RSSI_ch, 
						 char *RSSI_val,
						 char *NF_val,
						 int Band, DWORD DeviceId);
#endif //#if defined (_W8787_)
//DPD API
#if defined(_W8782_)

int SetDPD(DWORD PowerID, DWORD Memory2Row, DWORD *pDPDTbl, DWORD DeviceId=MIMODEVICE0);
int GetDPD(DWORD PowerID, DWORD Memory2Row, DWORD *pDPDTbl, DWORD DeviceId=MIMODEVICE0);

#endif //#if defined(_W8782_)

#ifndef _NO_CLASS_
};  //class DutWlanApiClss :DutIf_If 
#endif //#ifndef _NO_CLASS_

#endif
