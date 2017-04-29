/** @file DutBtApi878XDll.h
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
#include "DutApi878XDll.h" 


DUT_BT_API int STDCALL Dut_Bt_Version();

DUT_BT_API char* STDCALL Dut_Bt_About();

DUT_BT_API int STDCALL Dut_Bt_UartFwDl();

DUT_BT_API int STDCALL Dut_Bt_OpenDevice(void **theObj);

DUT_BT_API int STDCALL Dut_Bt_CloseDevice(void **theObj);

#ifdef _HCI_PROTOCAL_

DUT_BT_API int STDCALL Dut_Bt_GetBDAddress(BYTE *BdAddress);
DUT_BT_API int STDCALL Dut_Bt_ResetBt(void);
DUT_BT_API int STDCALL Dut_Bt_EnableDeviceUnderTestMode(void);
DUT_BT_API int STDCALL Dut_Bt_SetDeviceDiscoverable(void);
DUT_BT_API int STDCALL Dut_Bt_PeriodicInquery(int MaxPeriod, int MinPeriod, int InquryLength);
DUT_BT_API int STDCALL Dut_Bt_ExitPeriodicInquery(void);
#endif //#ifdef _HCI_PROTOCAL_
 

DUT_BT_API int STDCALL Dut_Bt_WriteMem(DWORD address, DWORD data);

DUT_BT_API int STDCALL Dut_Bt_ReadMem(DWORD address, DWORD *data);

DUT_BT_API int STDCALL Dut_Bt_WriteBtuRegister(DWORD address, WORD data);

DUT_BT_API int STDCALL Dut_Bt_ReadBtuRegister(DWORD address, WORD *data);

DUT_BT_API int STDCALL Dut_Bt_WriteBrfRegister(DWORD address, WORD data);

DUT_BT_API int STDCALL Dut_Bt_ReadBrfRegister(DWORD address, WORD *data);

DUT_BT_API int STDCALL Dut_Bt_SetPcmLoopBackMode(int enable, int useGpio12AsDin=0);

#if !defined (_NO_EEPROM_)
#if defined(_IF_USB_)
DUT_BT_API int STDCALL Dut_Bt_SetUsbAttributes(BYTE Attribute, BYTE MaxPower);
DUT_BT_API int STDCALL Dut_Bt_GetUsbAttributes(BYTE *pAttribute, BYTE *pMaxPower);
#endif // #if defined(_IF_USB_)
#endif // #if !defined (_NO_EEPROM_)


DUT_BT_API int STDCALL Dut_Bt_GetHwVersion(int *pSocChipId,int *pSocChipVersion,
					int *pBtuChipId,int *pBtuChipVersion,
					int *pBrfChipId,int *pBrfChipVersion);
DUT_BT_API int STDCALL Dut_Bt_GetFwVersion(int *pFwVersion, int *pMfgVersion, 
					char *pVersionString);
DUT_BT_API int STDCALL Dut_Bt_GetCurrentORVersion(int *pSocVersion, 
												  int *pRfVersion);
DUT_BT_API int STDCALL Dut_Bt_GetCurrentAppMode(int *pAppMode);
DUT_BT_API int STDCALL Dut_Bt_GetChipClassModeCapacity(int *pMode);

DUT_BT_API int STDCALL Dut_Bt_SetBtXtal(BYTE ExtMode, BYTE XtalValue);	
DUT_BT_API int STDCALL Dut_Bt_GetBtXtal(BYTE *pExtMode, BYTE *pXtalValue);

DUT_BT_API int STDCALL Dut_Bt_SetBtChannel(int channelNum, bool BT2=0);
DUT_BT_API int STDCALL Dut_Bt_SetBtChannelByFreq(int ChannelFreq, 
												 bool BT2=0);
DUT_BT_API int STDCALL Dut_Bt_GetBtChannel(int *pChannelNum, 
										   int *pChannelFreq, bool BT2=0);
//DUT_BT_API int STDCALL Dut_Bt_SetBtChBank(int BankNum, int channelNum=-1);
//DUT_BT_API int STDCALL Dut_Bt_GetBtChBank(int BankNum, int *pChannelNum);


DUT_BT_API int STDCALL Dut_Bt_SetBt2ChHopping(bool enable);
DUT_BT_API int STDCALL Dut_Bt_GetBt2ChHopping(bool *pEnable);

// bake code
//DUT_BT_API int STDCALL Dut_Bt_SetBtPseudoCode(DWORD option);
//DUT_BT_API int STDCALL Dut_Bt_GetBtPseudoCode(DWORD *pOption);

//0:MRVL, 1:Class2, 2:Class1 3: Class2+XPA
DUT_BT_API int STDCALL Dut_Bt_SetBtPwrControlClass(DWORD option);				
DUT_BT_API int STDCALL Dut_Bt_GetBtPwrControlClass(DWORD *pOption);

DUT_BT_API int STDCALL Dut_Bt_SetBtBrfReset(void);

DUT_BT_API int STDCALL Dut_Bt_SetBtPwrGain(DWORD Value);

DUT_BT_API int STDCALL Dut_Bt_GetBtPwrGain(DWORD *pValue);

DUT_BT_API int STDCALL Dut_Bt_SetBtPwrLvlValue(double PwrLvlValueDB, DWORD IsEDR);
DUT_BT_API int STDCALL Dut_Bt_GetBtPwrLvlValue(double *pPwrLvlValueDB);

//DUT_BT_API int STDCALL Dut_Bt_SetBtPwrMaxMin(double MaxValueDB,		
//											 double MinValueDB); 
DUT_BT_API int STDCALL Dut_Bt_GetBtPwrMaxMin(double *pMaxValueDB,		
											 double *pMinValueDB);
DUT_BT_API int STDCALL Dut_Bt_StepPower(double stepDB);
	
/*
DUT_BT_API int STDCALL Dut_Bt_SetBtCloseLoop(bool CloseLoop);			
DUT_BT_API int STDCALL Dut_Bt_GetBtCloseLoop(bool *pCloseLoop);		

DUT_BT_API int STDCALL Dut_Bt_SetBtPwrPpaPgaInitValue(WORD PpaValue,		WORD PgaValue);		// 0 internal mode 1: external mode
DUT_BT_API int STDCALL Dut_Bt_GetBtPwrPpaPgaInitValue(WORD *pPpaValue,	WORD *pPgaValue);
 

DUT_BT_API int STDCALL Dut_Bt_SetBtPwrPpaMaxMin(bool enable,			
							   WORD MaxValue,		WORD MinValue);		// 0 internal mode 1: external mode
DUT_BT_API int STDCALL Dut_Bt_GetBtPwrPpaMaxMin(bool *pEnable,		
							   WORD *pMaxValue,		WORD *pMinValue);		// 0 internal mode 1: external mode

DUT_BT_API int STDCALL Dut_Bt_SetBtPwrPgaMaxMin(bool enable,			
							   WORD MaxValue,		WORD MinValue);		// 0 internal mode 1: external mode
DUT_BT_API int STDCALL Dut_Bt_GetBtPwrPgaMaxMin(bool *pEnable,		
							   WORD *pMaxValue,		WORD *pMinValue);		// 0 internal mode 1: external mode


// 0 internal mode 1: external mode
DUT_BT_API int STDCALL Dut_Bt_SetBtPwrPpaPgaExtMode(bool extMode,  
											 WORD PpaValue, 
											 WORD PgaValue);		
DUT_BT_API int STDCALL Dut_Bt_GetBtPwrPpaPgaExtMode(bool *pExtMode,  
											 WORD *pPpaValue, 
											 WORD *pPgaValue);
 
DUT_BT_API int STDCALL Dut_Bt_SetBtPwrThSetting(WORD ThHi, WORD ThLo);			 
DUT_BT_API int STDCALL Dut_Bt_GetBtPwrThSetting(WORD *pThHi, WORD *pThLo);
*/
// dataRate 0; 1M, 1:2M, 2: 3M
// payloadPattern 0: all 0, 1: all 1, 2: PN9, 3: 0xAA, 4: 0xF0
// payloadOnly: tx Payload only, no header (true, false)
DUT_BT_API int STDCALL Dut_Bt_GetBtTxDetectedInChip(int *pTx); 

DUT_BT_API int STDCALL Dut_Bt_BrfTxBtCont( bool enable,			
									    int dataRate=0x1, 
										int payloadPattern=DEF_PAYLOADPATTERN );

// dataRate 0; 1M, 1:2M, 2: 3M
// payloadPattern 0: all 0, 1: all 1, 2: PN9, 3: 0xAA, 4: 0xF0
// payloadOnly: tx Payload only, no header (true, false)
// DutyWeight: On-Time percentage
DUT_BT_API int STDCALL Dut_Bt_BrfTxBtDutyCycle(bool enable,			
			int dataRate=0x1,  
			int payloadPattern=DEF_PAYLOADPATTERN,	
			int DutyWeight=95, 
			bool payloadOnly=0);

/*
DUT_BT_API int STDCALL Dut_Bt_TxBtMultiCastFrames(
			int FrameCount=100,	
			int PayloadLength=1024,  
			int PacketType=0x11, 
			int payloadPattern=DEF_PAYLOADPATTERN, 
			bool payloadOnly=0);
*/

DUT_BT_API int STDCALL Dut_Bt_MfgMrvlRxTestResult1(
				BYTE*		Report_Status=NULL, // status
				DWORD*		Report_TotalPcktCount=NULL,
				DWORD*		Report_HecMatchCount=NULL,
				DWORD*		Report_HecMatchCrcPckts=NULL,
				DWORD*		Report_NoRxCount=NULL,
				DWORD*		Report_CrcErrCount=NULL,
				DWORD*		Report_SuccessfulCorrelation=NULL,
				DWORD*		Report_TotalByteCount=NULL,
				double*		Report_BER=NULL,
				DWORD*		Report_PER=NULL,
				DWORD*		Report_AvgRssi=NULL,
				DWORD*		Report_TotalBitsCountExpected=NULL,
				DWORD*		Report_TotalBitsCountError_LostDrop=NULL);

#ifndef _HCI_PROTOCAL_

DUT_BT_API int STDCALL Dut_Bt_MfgResetBt(void);
DUT_BT_API int STDCALL Dut_Bt_MfgGetFwBdAddress(unsigned char *pBdAddress);

DUT_BT_API int STDCALL Dut_Bt_MfgEnterTestMode(void);

DUT_BT_API int STDCALL Dut_Bt_MfgPeriodicInquery(int MaxPeriod=3, 
												 int MinPeriod=2, 
												 int InquryLength=4);

DUT_BT_API int STDCALL Dut_Bt_MfgExitPeriodicInquery(void);

DUT_BT_API int STDCALL Dut_Bt_MfgMrvlTxTest( bool enable,	 
					int PacketType, 
					int PayLoadPattern, 
					int PayloadLength, 
					int RandomHopping,
                    int Interval, 
					int Whitening=0);

DUT_BT_API int STDCALL Dut_Bt_MfgMrvlCountedTxTest(int TxCnt, char TxPwrLvl,  	 
					int PacketType, 
					int PayLoadPattern, 
					int PayloadLength, 
					int RandomHopping,
					int Interval,
					int Whitening); 

DUT_BT_API int STDCALL Dut_Bt_MfgMrvlRxTest(
					int RxChannel,
					int PacketNumber,
					int PacketType, 
					int PayLoadPattern, 
					int PayloadLength,	
				//	char *pPRBS,	
					char *pTxBdAddress);

DUT_BT_API int STDCALL Dut_Bt_MfgMrvlRxTestResult(
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

DUT_BT_API int STDCALL Dut_Bt_MfgMrvlRssiClear(int LinkId);
DUT_BT_API int STDCALL Dut_Bt_MfgMrvlRssiRpt(int LinkId, 
											 int *pRssiAvg, 
											 int *pRssiLast); 


#endif //#ifndef _HCI_PROTOCAL_

#if defined (_HCI_PROTOCAL_)
DUT_BT_API int STDCALL Dut_Bt_TxBtDutyCycleHop(bool enable,			
										int PacketType=0x11, 
										int payloadPattern=DEF_PAYLOADPATTERN,	
										int PayloadLenghtInByte=PAYLOADLENGTH_MAXIMUN_POSSIBLE, 
										bool HopMode=0, int interval=1,
										int Whitening=0);

DUT_BT_API int STDCALL Dut_Bt_TxBtTxCountedCycleHop(int TxCnt=0, 
										char TxPwrLvl=0x7F,
										int PacketType=0x11, 
										int payloadPattern=DEF_PAYLOADPATTERN,	
										int PayloadLenghtInByte=PAYLOADLENGTH_MAXIMUN_POSSIBLE, 
										bool HopMode=0, int interval=1,
										int Whitening=0
										); 
 
DUT_BT_API int STDCALL Dut_Bt_MrvlRxTest(
					int RxChannel, 
					int PacketNumber,	
					int PacketType, 
					int payloadPattern=DEF_PAYLOADPATTERN,	
					int PayloadLenghtInByte=PAYLOADLENGTH_MAXIMUN_POSSIBLE, 
					char TxBdAddress[SIZE_BDADDRESS_INBYTE]=NULL  
		);

DUT_BT_API int STDCALL Dut_Bt_MrvlRxTestResult( 
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

#endif //#if defined (_HCI_PROTOCAL_)


// not supported by this chip   
//DUT_BT_API int STDCALL Dut_Bt_TxBtCst(bool enable, 
//									  int FreqOffsetinKHz=DEF_IQSINEFREQ_INKHZ);
DUT_BT_API int STDCALL Dut_Bt_TxBtCw(bool enable); 

//DUT_BT_API int STDCALL Dut_Bt_BrfInit(void);
DUT_BT_API int STDCALL Dut_Bt_SetBtDisableBtuPwrCtl(bool DisableBtuPwrCtl);
DUT_BT_API int STDCALL Dut_Bt_GetBtDisableBtuPwrCtl(bool *pDisableBtuPwrCtl);

//DUT_BT_API int STDCALL Dut_Bt_GetTxIQ(DWORD *pExtMode, 
//									  BYTE *pAlpha, BYTE *pBeta);

//DUT_BT_API int STDCALL Dut_Bt_SetTxIQ(DWORD extMode, 
//									  BYTE Alpha, BYTE Beta);

// not supported by this chip  
//DUT_BT_API int STDCALL Dut_Bt_GetBtThermalSensorReading(DWORD *pValue);

DUT_BT_API int STDCALL Dut_Bt_SetBtThermalCtrlMode(DWORD Enable);
DUT_BT_API int STDCALL Dut_Bt_GetBtDbgThermalCtrl(DWORD *pCtrlActive,
						DWORD *pTempAvg,
						DWORD *pTempCur,
						DWORD *pCorrDelta,
						DWORD *pTemp10Avg);

#if !defined (_NO_EEPROM_)

DUT_BT_API int STDCALL Dut_Bt_ForceE2PromType(DWORD		IfType, 
										  DWORD		AddrWidth,
										  DWORD		DeviceType);
DUT_BT_API int STDCALL Dut_Bt_GetE2PromType(DWORD		*pIfType, 
											DWORD		*pAddrWidth,
											DWORD		*pDeviceType);
DUT_BT_API int STDCALL Dut_Bt_SpiWriteDword(DWORD Address, DWORD data);
DUT_BT_API int STDCALL Dut_Bt_SpiReadDword(DWORD Address, DWORD *data);
#endif // #if !defined (_NO_EEPROM_)

//DUT_BT_API int STDCALL Dut_Bt_ResetAccum(void);
//DUT_BT_API int STDCALL Dut_Bt_GetAccum(int *pAccum);

DUT_BT_API int STDCALL Dut_Bt_SetBtXPaPdetPolority(bool Polority_Neg);
DUT_BT_API int STDCALL Dut_Bt_GetBtXPaPdetPolority(bool *pPolority_Neg);

DUT_BT_API int STDCALL Dut_Bt_SetBtPpaPgaPriority(bool PgaFirst);
DUT_BT_API int STDCALL Dut_Bt_GetBtPpaPgaPriority(bool *pPgaFirst);

DUT_BT_API int STDCALL Dut_Bt_SetBtPaThermScaler(int ThermScaler);
DUT_BT_API int STDCALL Dut_Bt_GetBtPaThermScaler(int *pThermScaler);


DUT_BT_API int STDCALL Dut_Bt_SetBtPaBias(WORD PaBias1, WORD PaBias2, bool PerformanceMode);
DUT_BT_API int STDCALL Dut_Bt_GetBtPaBias(WORD *pPaBias1, WORD *pPaBias2, bool PerformanceMode);
///////////////////////////////////
DUT_BT_API int STDCALL Dut_Bt_SetExtendedPwrCtrlMode(int Mode);
DUT_BT_API int STDCALL Dut_Bt_GetExtendedPwrCtrlMode(int *pMode);
DUT_BT_API int STDCALL Dut_Bt_SetBbGainInitValue(WORD Gain);
DUT_BT_API int STDCALL Dut_Bt_GetBbGainInitValue(WORD *pGain);
DUT_BT_API int STDCALL Dut_Bt_SetBtPwrBbGainMaxMin(bool enable,	
							WORD MaxValue,		WORD MinValue);		
DUT_BT_API int STDCALL Dut_Bt_GetBtPwrBbGainMaxMin(bool *pEnable, 
							WORD *pMaxValue,	WORD *pMinValue);	
DUT_BT_API int STDCALL Dut_Bt_SetBtPwrBbGainExtMode(bool extMode, WORD BbGainValue);
DUT_BT_API int STDCALL Dut_Bt_GetBtPwrBbGainExtMode(bool *pExtMode, WORD *pBbGainValue);
DUT_BT_API int STDCALL Dut_Bt_SetPpaGainStepMode(int Mode);
DUT_BT_API int STDCALL Dut_Bt_GetPpaGainStepMode(int *pMode);
DUT_BT_API int STDCALL Dut_Bt_SetAutoZeroMode(int Mode, WORD Value);
DUT_BT_API int STDCALL Dut_Bt_GetAutoZeroMode(int *pMode, WORD *pValue);
DUT_BT_API int STDCALL Dut_Bt_SetTxPwrModeHighLow(int Mode);
DUT_BT_API int STDCALL Dut_Bt_GetTxPwrModeHighLow(int *pMode);
#if !defined (_NO_EFUSE_)
DUT_BT_API int STDCALL Dut_Bt_SetEfuseData(int Block, int LenInWord, 
										   WORD *pData);
DUT_BT_API int STDCALL Dut_Bt_GetEfuseData(int Block, int LenInWord, 
										   WORD *pData);

#endif // #if !defined (_NO_EFUSE_)
DUT_BT_API int STDCALL Dut_Bt_GetFwReleaseNote(char *pReleaseNote);
///////////////////////////////////
 
#if !defined (_NO_EEPROM_)
DUT_BT_API int STDCALL Dut_Bt_DumpE2PData(DWORD StartAddress, DWORD LenInDw,
									DWORD *pAddressArray, 
									DWORD *pDataArray);
DUT_BT_API int STDCALL Dut_Bt_WriteLumpedData(DWORD StartAddress, 
								   DWORD LenInDw, 
									DWORD *pDataArray);
DUT_BT_API int STDCALL Dut_Bt_SpiGetBoot2Version(DWORD *pBoot2Version);

DUT_BT_API int STDCALL Dut_Bt_UpdateCalDataRevC_Bt(int PurgeAfter, 
												   char *FlexFileNameNoE2prom=NULL); 
DUT_BT_API int STDCALL Dut_Bt_GetCalDataRevC(int *CsC, int *CsCValue, 
												char *FlexFileNameNoE2prom=NULL);
 
DUT_BT_API int STDCALL Dut_Bt_SetCalDataRevC(int PurgeAfter, 
											 char *FlexFileNameNoE2prom=NULL);

DUT_BT_API int STDCALL Dut_Bt_FlexSpiDL(bool erase, char *fileName);
DUT_BT_API int STDCALL Dut_Bt_FlexSpiCompare(char *fileName);
DUT_BT_API int STDCALL Dut_Bt_CompareE2PData(
					DWORD  *RefArray,int SizeInByte);

DUT_BT_API int STDCALL Dut_Bt_EraseEEPROM(int section);
DUT_BT_API int STDCALL Dut_Bt_SetRfPowerUseCal(double PwrdBm, int ClassMode, int useFeLoss=0);
DUT_BT_API int STDCALL Dut_Bt_SupportNoE2PROM(void);

DUT_BT_API int STDCALL Dut_Bt_CheckCalDataSpace(int* pAvalable); 
DUT_BT_API int STDCALL Dut_Bt_GetHeaderPresent(int *pPresent, DWORD *pVersion=NULL, int *CsC=NULL);
#endif // #if !defined (_NO_EEPROM_)



DUT_BT_API int STDCALL Dut_Bt_LoadCustomizedSettings(void);
//DUT_BT_API int STDCALL Dut_Bt_GetBdAddressFromE2P(BYTE BdAddress[SIZE_BDADDRESS_INBYTE]);


 
DUT_BT_API int STDCALL Dut_Bt_AclLinkTest(int PacketType, int PacketCount, 
							int PayloadLengthInByte, int PayloadPattern, 
							bool MasterAsTx, bool DutAsMaster, 
							bool stress, 
						int *pRxPacket,
						int *pMissingPacket,
						double *pPer, double*pBer); 

// not supported by this chip  
/*
DUT_BT_API int STDCALL Dut_Bt_ClearRxCnts(int LinkIndex);
DUT_BT_API int STDCALL Dut_Bt_GetRxInfo(	int LinkIndex, 
				int	*pRssiQual,
				int	*pRssiValue,
				int	*pReceivedCrcCnt, 
				int *pHecMatchCnt,
				int *pCrcErrorCnt,
				int *pFECCorrectCnt);
*/
#if defined (_W8688_) || defined (_W8787_) || defined(_W8782_)
DUT_BT_API int STDCALL Dut_Bt_SetBcaMode(int Mode);
DUT_BT_API int STDCALL Dut_Bt_GetBcaMode(int *pMode);
#endif // #if defined (_W8688_) || defined (_W8787_)


#if defined (_W8688_) 
DUT_BT_API int STDCALL Dut_Bt_SetFE_BTOnly(bool bOn);
#endif // #if defined (_W8688_)

DUT_BT_API int STDCALL Dut_Bt_Test_Hold(int modeOnOff, 
							  WORD HoldMaxInterval, 
							  WORD HoldMinInterval,
							  bool PartnerAutoCtrl=0);

DUT_BT_API int STDCALL Dut_Bt_Test_Park(int modeOnOff, 
							  WORD BeaconMaxInterval, 
							  WORD BeaconMinInterval,
							  bool partnerAutoCtrl=0);

DUT_BT_API int STDCALL Dut_Bt_Test_Sniff(int modeOnOff, 
								WORD SniffMaxInterval,	
								WORD SniffMinInterval,
								WORD SniffAttempt,		
								WORD SniffTimeout,
								bool partnerAutoCtrl=0);


DUT_BT_API int STDCALL Dut_Bt_Hold(WORD hConnection, 
									WORD HoldMaxInterval, 
									WORD HoldMinInterval);


 
 
DUT_BT_API int STDCALL Dut_Bt_SetMrvlTempCompModeHC(int		SwPwrCtrlMode,
													WORD	RefGain1,
 													WORD	RefGain2,
 													BYTE	RefTemp,
 													BYTE	TempGain);


DUT_BT_API int STDCALL Dut_Bt_SetRfDynamicClkCtrl(int		DynClkTimer,
						int		MultiSlotProtect,
						int		AutoDetect,
						int		DynClkValue,
						int		DynClkEnable,
						int		DynClkStartTime,
						int		DynClkFinishTime,
						int		DynClkTimedEnable);

DUT_BT_API int STDCALL Dut_Bt_SetBtSleepMode(int PowerMode);

DUT_BT_API int STDCALL Dut_Bt_SetWakeUpMethod(int		HostWakeUpMethod,
					int		HostWakeUpGPIO,
					int		DeviceWakeUpMethod,
					int		DeviceWakeUpGPIO);


DUT_BT_API int STDCALL Dut_Bt_SetMrvlHiuModuleConfig(int ModuleShutDownBringUp, 
						   int HostIfActive, 
						   int HostIfType);


DUT_BT_API int STDCALL Dut_Bt_SetMrvlRadioPerformanceMode(	int		MrvlRxPowerMode,	 
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

DUT_BT_API int STDCALL Dut_Bt_SetMrvlArmDynamicClkCtrl(int		SetNcoMode,	 
 							int		DynamicClockChange 	 );


DUT_BT_API int STDCALL Dut_Bt_SetBtXCompModeConfig(int		BTCoexistenceMode,	 
 									int		PriorityMode_Wlan,
									int PriorityMode_UpdateDefInPta,
									int PriorityDelay,
									int HighPriority_BitMap);
DUT_BT_API int STDCALL Dut_Bt_SetBtMrvlModeConfig(int		BTCoexistenceMode,	 
 									int PriorityMode_Wlan,
									int PriorityMode_UpdateDefInPta,
									int PriorityDelay,
									int HighPriority_BitMap, 
									int MediumPriority_BitMap,
									int BtEndTimer,
									int BtFreqLowBand,
									int BtFreqHighBand );
DUT_BT_API int STDCALL Dut_Bt_SetBtMrvlMode2Config(int		BTCoexistenceMode,	 
 									int	NonPeriodicMode_Enable, 
									int NonPeriodicMode_BtTimerTlead,
									int NonPeriodicMode_BtTimerTtail,
									int PeriodicMode_BtSyncPeriod,
									int PeriodicMode_BtSyncPulseWidth,
									int PeriodicMode_BtTsyncLead,
									int PeriodicMode_BtSyncMode);


