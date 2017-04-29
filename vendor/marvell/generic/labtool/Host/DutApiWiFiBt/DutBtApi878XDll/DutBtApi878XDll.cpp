/** @file DutBtApi878XDll.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */
 
#include "../../DutCommon/Clss_os.h"
#include "../../DutCommon/utilities.h" 
//#include DUTCLASS_SHARED_STDH_PATH	// "../DutShareApi878XDll/stdafx.h"

#include DUTDLL_BT_H_PATH		// "../DutBtApi878XDll.h"
#include DUTCLASS_BT_H_PATH		// "DutBtApiClss.h"
#include DUTCLASS_SHARED_H_PATH		// "../DutShareApi878XDll/DutSharedClss.h"

#ifdef _HCI_PROTOCAL_
#ifndef _LINUX_
#include GRUCLASS_BT_H_PATH		// "../GruBtApi86xxDll/GruBtApiClss.h"
#endif
#endif //#ifdef _HCI_PROTOCAL_

DUT_BT_API int STDCALL Dut_Bt_Version()
{
	return BT_OBJ Bt_Version();
}

DUT_BT_API char* STDCALL Dut_Bt_About()
{
	return BT_OBJ Bt_About();
}

#if defined (_BT_UART_)
DUT_BT_API int STDCALL Dut_Bt_UartFwDl()
{
	return  BT_OBJ UartFwDl();
}
#endif //#if defined (_BT_UART_)

DUT_BT_API int STDCALL Dut_Bt_OpenDevice(void **theObj)
{

#ifndef _LINUX_
#ifndef STATIC_OBJ
	if (BtObj == NULL)
	{
		if (*theObj)
		{
			BtObj = (DutBtApiClss*)(*theObj);
			BT_OBJ DutBtApiClss_Init();
			return ERROR_NONE;
		}
		else
		{
			BtObj = new (DutBtApiClss);
			*theObj = (void*)BtObj;
			return BT_OBJ Bt_OpenDevice();
		}
	}
#else	//#ifndef STATIC_OBJ
 	BtObj = &BtOnlyOne;  
	*theObj = (void*)BtObj;
#endif	//#ifndef STATIC_OBJ
#else	//#ifndef _LINUX_
	return Bt_OpenDevice();
#endif //#ifndef _LINUX_
		return ERROR_NONE;

/*	
	if(NULL == BtObj) 
	{
		if (*theObj)
		{
			BtObj = (DutBtApiClss*)*theObj;
		}
		else
		{
			BtObj = &BtOnlyOne;
		}
	}
	return BT_OBJ OpenDevice();


#ifndef STATIC_OBJ
	BtObj = new (DutBtApiClss);
#else
	if(NULL == BtObj) BtObj = &BtOnlyOne;
#endif //#ifdef STATIC_OBJ
 
	return BT_OBJ OpenDevice();
*/
}

DUT_BT_API int STDCALL Dut_Bt_CloseDevice(void **theObj)
{
	int err=0;

	err = BT_OBJ Bt_CloseDevice();
#ifndef _LINUX_  

#ifndef STATIC_OBJ
	delete BtObj;
	BtObj = NULL;
	*theObj = BtObj;
#endif
#endif	//#ifndef _LINUX_ 

#if !defined(_BT_SDIO_) 
	if(theObj)
	{
		*theObj = NULL;
	}
#endif

	return err;
	
}

//DUT_BT_API int STDCALL Dut_Bt_MfgPfwReloadCalfromFlash(void) 
//{
//	return  BT_OBJ MfgPfwReloadCalfromFlash();
//}

#ifdef _HCI_PROTOCAL_

DUT_BT_API int STDCALL Dut_Bt_GetBDAddress(BYTE *BdAddress)
{
	return  BT_OBJ GetBDAddress(BdAddress);
		
}

DUT_BT_API int STDCALL Dut_Bt_ResetBt(void)
{
	return  BT_OBJ ResetBt();
}

DUT_BT_API int STDCALL Dut_Bt_PeriodicInquery(int MaxPeriod, int MinPeriod, int InquryLength)
{
	return  BT_OBJ PeriodicInquery(MaxPeriod, MinPeriod, InquryLength);
}

DUT_BT_API int STDCALL Dut_Bt_ExitPeriodicInquery(void)
{
	return  BT_OBJ ExitPeriodicInquery();
}


DUT_BT_API int STDCALL Dut_Bt_EnableDeviceUnderTestMode(void)
{
	return  BT_OBJ EnableDeviceUnderTestMode();
}
#endif //#ifdef _HCI_PROTOCAL_

DUT_BT_API int STDCALL Dut_Bt_WriteMem(unsigned long address, unsigned long data)
{
	return BT_OBJ WriteMemDword(address, data);
}


DUT_BT_API int STDCALL Dut_Bt_ReadMem(unsigned long address, unsigned long *data)
{
	return BT_OBJ ReadMemDword(address, data);
}

DUT_BT_API int STDCALL Dut_Bt_WriteBtuRegister(unsigned long address, unsigned short data)
{
	return BT_OBJ WriteBtuReg(address, data);
}

DUT_BT_API int STDCALL Dut_Bt_ReadBtuRegister(unsigned long address, unsigned short *data)
{
	return BT_OBJ ReadBtuReg(address, data);
}

DUT_BT_API int STDCALL Dut_Bt_WriteBrfRegister(unsigned long address, unsigned short data)
{
	return BT_OBJ WriteBrfReg(address, data);
}

DUT_BT_API int STDCALL Dut_Bt_ReadBrfRegister(unsigned long address, unsigned short *data)
{
	return BT_OBJ ReadBrfReg(address, data);
}



DUT_BT_API int STDCALL Dut_Bt_GetHwVersion(int *pSocChipId,int *pSocChipVersion,
					int *pBtuChipId,int *pBtuChipVersion,
					int *pBrfChipId,int *pBrfChipVersion)
{

	return BT_OBJ Bt_GetHwVersion(pSocChipId, pSocChipVersion,
						pBtuChipId,	pBtuChipVersion,
						pBrfChipId,	pBrfChipVersion);

}

DUT_BT_API int STDCALL Dut_Bt_GetFwVersion(int *pFwVersion, int *pMfgVersion, 
					char *pVersionString)
{

	return BT_OBJ Bt_GetFwVersion(pFwVersion, pMfgVersion, 
					pVersionString);

}
DUT_BT_API int STDCALL Dut_Bt_GetCurrentORVersion(int *pSocVersion, int *pRfVersion)
{

	return BT_OBJ Bt_GetCurrentORVersion(pSocVersion, pRfVersion);
}
DUT_BT_API int STDCALL Dut_Bt_GetCurrentAppMode(int *pAppMode)
{
	return BT_OBJ Bt_GetCurrentAppMode(pAppMode);
}



DUT_BT_API int STDCALL Dut_Bt_GetChipClassModeCapacity(int *pMode)
{
	return BT_OBJ GetChipClassModeCapacity(pMode);
}

#if defined(_W8780_) 
DUT_BT_API int STDCALL Dut_Bt_GetTrSwAntenna(int *pAntSelection)
{
	return BT_OBJ GetTrSwAntenna(pAntSelection);
}

DUT_BT_API int STDCALL Dut_Bt_SetTrSwAntenna(int AntSelection)
{
	return BT_OBJ SetTrSwAntenna(AntSelection);
}
#endif // #if defined(_W8780_)

DUT_BT_API int STDCALL Dut_Bt_SetPcmLoopBackMode(int enable, int useGpio12AsDin)
{
	return BT_OBJ SetPcmLoopBackMode(enable, useGpio12AsDin);
}

#if !defined (_NO_EEPROM_)
#if defined (_IF_USB_) 
DUT_BT_API int STDCALL Dut_Bt_SetUsbAttributes(BYTE Attribute, BYTE MaxPower) 
{
	return SHARED_OBJ SetUsbAttributes(DEVICETYPE_BT, BtObj, 
										 Attribute, MaxPower);
}

DUT_BT_API int STDCALL Dut_Bt_GetUsbAttributes(BYTE *pAttribute, BYTE *pMaxPower)
{
	return SHARED_OBJ GetUsbAttributes(DEVICETYPE_BT, BtObj, 
										 pAttribute, pMaxPower);
}
#endif // #if defined (_IF_USB_) 
#endif // #if !defined (_NO_EEPROM_)


DUT_BT_API int STDCALL Dut_Bt_SetBtXtal(BYTE ExtMode, BYTE XtalValue)		
// 0 internal mode 1: external mode
{
	return BT_OBJ SetBtXtal(ExtMode, XtalValue);		
}

DUT_BT_API int STDCALL Dut_Bt_GetBtXtal(BYTE *pExtMode, BYTE *pXtalValue)
{
	return BT_OBJ GetBtXtal(pExtMode, pXtalValue);
}

DUT_BT_API int STDCALL Dut_Bt_SetBtChannel(int channelNum, bool BT2)
{
	return BT_OBJ SetBtChannel(channelNum, BT2);
}

DUT_BT_API int STDCALL Dut_Bt_SetBtChannelByFreq(int ChannelFreq, bool BT2)
{
	return BT_OBJ SetBtChannelByFreq(ChannelFreq, BT2);
}

DUT_BT_API int STDCALL Dut_Bt_GetBtChannel(int *pChannelNum, int *pChannelFreq, 
										   bool BT2)
{
	//DebugLogRite(" JKL GetBtChannel Dut_Bt_GetBtChannel \n");
	return BT_OBJ GetBtChannel(pChannelNum, pChannelFreq, BT2);
}

DUT_BT_API int STDCALL Dut_Bt_SetBt2ChHopping(bool enable)
{
	return BT_OBJ SetBt2ChHopping(enable);
}

DUT_BT_API int STDCALL Dut_Bt_GetBt2ChHopping(bool *pEnable) 
{
	return BT_OBJ GetBt2ChHopping(pEnable);
}


//0:MRVL, 1:Class2, 2:Class1 3: Class2+XPA
DUT_BT_API int STDCALL Dut_Bt_SetBtPwrControlClass(DWORD option) 				
{
	return BT_OBJ SetBtPwrControlClass(option);			
}
DUT_BT_API int STDCALL Dut_Bt_GetBtPwrControlClass(DWORD *pOption)
{
	return BT_OBJ GetBtPwrControlClass(pOption);
}


DUT_BT_API int STDCALL Dut_Bt_SetBtBrfReset(void)
{
	return BT_OBJ SetBtBrfReset();
}

DUT_BT_API int STDCALL Dut_Bt_SetBtPwrGain(DWORD Value)
{
	return BT_OBJ SetBtPwrGain(Value);
}

DUT_BT_API int STDCALL Dut_Bt_GetBtPwrGain(DWORD *pValue)
{
	return BT_OBJ GetBtPwrGain(pValue);
}

/*
DUT_BT_API int STDCALL Dut_Bt_SetBtCloseLoop(bool CloseLoop)
{
	return BT_OBJ SetBtCloseLoop(CloseLoop);		
}

DUT_BT_API int STDCALL Dut_Bt_GetBtCloseLoop(bool *pCloseLoop)
{
	return BT_OBJ GetBtCloseLoop(pCloseLoop);		
}
*/

DUT_BT_API int STDCALL Dut_Bt_SetBtPwrLvlValue(double PwrLvlValueDB, DWORD IsEDR)
{
	return BT_OBJ SetBtPwrLvlValue(PwrLvlValueDB, IsEDR);
}
DUT_BT_API int STDCALL Dut_Bt_GetBtPwrLvlValue(double *pPwrLvlValueDB)
{
	return BT_OBJ GetBtPwrLvlValue(pPwrLvlValueDB);
}

DUT_BT_API int STDCALL Dut_Bt_StepPower(double stepDB)
{
	return BT_OBJ StepPower(stepDB);
}


// 0 internal mode 1: external mode
DUT_BT_API int STDCALL Dut_Bt_GetBtPwrMaxMin(double *pMaxValueDB,		
											 double *pMinValueDB)
{
	return BT_OBJ GetBtPwrMaxMin(pMaxValueDB, pMinValueDB);		
}

DUT_BT_API int STDCALL Dut_Bt_GetBtTxDetectedInChip(int *pTx)
{
	return BT_OBJ GetBtTxDetectedInChip(pTx);
}

DUT_BT_API int STDCALL Dut_Bt_BrfTxBtCont(bool enable,			int dataRate, 
			 int payloadPattern)
{
	return BT_OBJ BrfTxBtCont(enable, dataRate, payloadPattern);
}

DUT_BT_API int STDCALL Dut_Bt_BrfTxBtDutyCycle(bool enable,			
											int PacketType, 
			 int payloadPattern,	int DutyWeight, bool payloadOnly)
{
	return BT_OBJ BrfTxBtDutyCycle(enable, PacketType, 
			 payloadPattern,	DutyWeight, payloadOnly);
}


DUT_BT_API int STDCALL Dut_Bt_MfgResetBt(void) 
{
	return BT_OBJ MfgResetBt();
}

DUT_BT_API int STDCALL Dut_Bt_MfgGetFwBdAddress(unsigned char *pBdAddress)
{
	return BT_OBJ MfgGetFwBdAddress(pBdAddress);
}

DUT_BT_API int STDCALL Dut_Bt_MfgEnterTestMode(void)
{
	int status=0;
	
	status = BT_OBJ MfgResetBt();
	if(status) return status;

	Sleep(1500);

	return BT_OBJ MfgEnterTestMode();
}

DUT_BT_API int STDCALL Dut_Bt_MfgPeriodicInquery(int MaxPeriod, int MinPeriod, int InquryLength) 
{
	return BT_OBJ MfgPeriodicInquery(MaxPeriod, MinPeriod, InquryLength);
}

DUT_BT_API int STDCALL Dut_Bt_MfgExitPeriodicInquery(void) 
{
	return BT_OBJ MfgExitPeriodicInquery();
}


DUT_BT_API int STDCALL Dut_Bt_MfgMrvlTxTest( bool enable,	 
					int PacketType, 
					int PayLoadPattern, 
					int PayloadLength, 
					int RandomHopping,
					int Interval,
					int Whitening) 
{
	return BT_OBJ MfgMrvlTxTest(enable,PacketType, 
				   PayLoadPattern, PayloadLength, RandomHopping, Interval, Whitening);
}

DUT_BT_API int STDCALL Dut_Bt_MfgMrvlCountedTxTest(int TxCnt, char TxPwrLvl,  	 
					int PacketType, 
					int PayLoadPattern, 
					int PayloadLength, 
					int RandomHopping,
					int Interval,
					int Whitening) 
{
	return BT_OBJ MfgMrvlTxTest2(TxCnt==0?false:true,	
			PacketType, 
			PayLoadPattern,	PayloadLength, 
			RandomHopping, Interval,
			Whitening, TxCnt, TxPwrLvl);
}


DUT_BT_API int STDCALL Dut_Bt_MfgMrvlRxTest(
					int RxChannel,
					int PacketNumber,	
					int PacketType, 
					int PayLoadPattern, 
					int PayloadLength,
			//		char *pPRBS,	
					char *pTxBdAddress)
{
	return BT_OBJ MfgMrvlRxTest(
					RxChannel,
					PacketNumber,
					PacketType, 
					PayLoadPattern, 
					PayloadLength, 
		//			pPRBS,	
					pTxBdAddress);
}

DUT_BT_API int STDCALL Dut_Bt_MfgMrvlRxTestResult1(
				BYTE*		Report_Status,
				DWORD*		Report_TotalPcktCount,
				DWORD*		Report_HecMatchCount,
				DWORD*		Report_HecMatchCrcPckts,
				DWORD*		Report_NoRxCount,
				DWORD*		Report_CrcErrCount,
				DWORD*		Report_SuccessfulCorrelation,
				DWORD*		Report_TotalByteCount,
				double*		Report_BER,
				DWORD*		Report_PER,
				DWORD*		Report_AvgRssi,
				DWORD*		Report_TotalBitsCountExpected,
				DWORD*		Report_TotalBitsCountError_LostDrop)
{
	DWORD BER=0;

	int status = 
#ifdef _HCI_PROTOCAL_
	BT_OBJ BtMrvlRxTestResult(
#else
	BT_OBJ MfgMrvlRxTestResult(
#endif	//#ifdef _HCI_PROTOCAL_
				Report_Status,
				Report_TotalPcktCount,
				Report_HecMatchCount,
				Report_HecMatchCrcPckts,
				Report_NoRxCount,
				Report_CrcErrCount,
				Report_SuccessfulCorrelation,
				Report_TotalByteCount,
				&BER,
				Report_PER,
				Report_AvgRssi,
				Report_TotalBitsCountExpected,
				Report_TotalBitsCountError_LostDrop);

	if(*Report_TotalBitsCountExpected)
		*Report_BER = 100.0*(*Report_TotalBitsCountError_LostDrop)/(*Report_TotalBitsCountExpected);

	return status;
}

DUT_BT_API int STDCALL Dut_Bt_MfgMrvlRxTestResult(
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
				DWORD*		Report_TotalBitsCountError_LostDrop)
{
	return BT_OBJ MfgMrvlRxTestResult(
				Report_Status,
				Report_TotalPcktCount,
				Report_HecMatchCount,
				Report_HecMatchCrcPckts,
				Report_NoRxCount,
				Report_CrcErrCount,
				Report_SuccessfulCorrelation,
				Report_TotalByteCount,
				Report_BER,
				Report_PER,
				Report_AvgRssi,
				Report_TotalBitsCountExpected,
				Report_TotalBitsCountError_LostDrop);
}

DUT_BT_API int STDCALL Dut_Bt_MfgMrvlRssiClear(int LinkId) 
{
	return BT_OBJ MfgMrvlRssiClear(LinkId);
}

DUT_BT_API int STDCALL Dut_Bt_MfgMrvlRssiRpt(int LinkId,
											 int *pRssiAvg, 
											 int *pRssiLast) 
{
	return BT_OBJ MfgMrvlRssiRpt(LinkId, pRssiAvg, pRssiLast);
}

#if defined (_HCI_PROTOCAL_)

DUT_BT_API int STDCALL Dut_Bt_TxBtDutyCycleHop(bool enable,			
										int PacketType, 
				int payloadPattern,	
				int PayloadLenghtInByte, 
				bool HopMode, int interval,
				int Whitening)
{
	return BT_OBJ TxBtDutyCycleHop(enable,	PacketType, 
			payloadPattern,	PayloadLenghtInByte, 
			HopMode, interval,
			Whitening);
}


DUT_BT_API int STDCALL Dut_Bt_TxBtTxCountedCycleHop(int TxCnt, char TxPwrLvl,  
										int PacketType, 
				int payloadPattern,	
				int PayloadLenghtInByte, 
				bool HopMode, int interval,
				int Whitening)
{
	return BT_OBJ TxBtDutyCycleHop2(TxCnt==0?false:true,	
			PacketType, 
			payloadPattern,	PayloadLenghtInByte, 
			HopMode, interval,
			Whitening, TxCnt, TxPwrLvl);
}




DUT_BT_API int STDCALL Dut_Bt_MrvlRxTest(
					int RxChannel, 
					int PacketNumber,	
					int PacketType, 
					int payloadPattern,	
					int PayloadLenghtInByte, 
					char TxBdAddress[SIZE_BDADDRESS_INBYTE] ) 
{
	return BT_OBJ BtMrvlRxTest(RxChannel, PacketNumber,	PacketType, 
								payloadPattern, PayloadLenghtInByte, 
								TxBdAddress);
}

DUT_BT_API int STDCALL Dut_Bt_MrvlRxTestResult( 
			 // report
				BYTE*		Report_Status, // status
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
				DWORD*		Report_TotalBitsCountError_LostDrop) 
{
	return BT_OBJ BtMrvlRxTestResult( 
			 // report
						Report_Status, // status
						Report_TotalPcktCount,
						Report_HecMatchCount,
						Report_HecMatchCrcPckts,
						Report_NoRxCount,
						Report_CrcErrCount,
						Report_SuccessfulCorrelation,
						Report_TotalByteCount,
						Report_BER,
						Report_PER,
						Report_AvgRssi,
						Report_TotalBitsCountExpected,
						Report_TotalBitsCountError_LostDrop) ;
}

/*
DUT_BT_API int STDCALL Dut_Bt_TxBtRxTest(
				int FrameCount,	int PacketType, 
				int payloadPattern,	
				int PayloadLenghtInByte, 
			 BYTE TxBdAddress[SIZE_BDADDRESS_INBYTE], int TxAmAddress, 
			 int FramSkip, int CorrWndw, int CorrTh, 
			 bool Whitening,BYTE PrbsPattern[LEN_PRBS_INBYTE],
			 			 // report
	BYTE*		Report_Status_Complete0_Abort1, // status
	DWORD*		Report_TotalPcktCount,
	DWORD*		Report_HecMatchCount,
	DWORD*		Report_HecMatchCrcPckts,
	DWORD*		Report_NoRxCount,
	DWORD*		Report_CrcErrCount,
	DWORD*		Report_SuccessfulCorrelation,
	DWORD*		Report_TotalByteCount,
	DWORD*		Report_ErrorBits,
	DWORD*		Report_PER,
	DWORD*		Report_BER)
{
	return BT_OBJ TxBtRxTest(FrameCount, PacketType, 
				 payloadPattern,	
				 PayloadLenghtInByte, 
			 TxBdAddress, TxAmAddress, 
			 FramSkip, CorrWndw, CorrTh, 
			 Whitening,PrbsPattern,
			 			 // report
			Report_Status_Complete0_Abort1, 
			Report_TotalPcktCount,
			Report_HecMatchCount,
			Report_HecMatchCrcPckts,
			Report_NoRxCount,
			Report_CrcErrCount,
			Report_SuccessfulCorrelation,
			Report_TotalByteCount,
			Report_ErrorBits,
			Report_PER,
			Report_BER);
}

*/
#endif //#if defined (_HCI_PROTOCAL_)


/*
DUT_BT_API int STDCALL Dut_Bt_SetExtendedPwrCtrlMode(int Mode) 
{
	return BT_OBJ SetExtendedPwrCtrlMode(Mode);
}
DUT_BT_API int STDCALL Dut_Bt_GetExtendedPwrCtrlMode(int *pMode)
{
	return BT_OBJ GetExtendedPwrCtrlMode(pMode);
}

DUT_BT_API int STDCALL Dut_Bt_SetBbGainInitValue(WORD Gain) 
{
	return BT_OBJ SetBbGainInitValue(Gain);
}
DUT_BT_API int STDCALL Dut_Bt_GetBbGainInitValue(WORD *pGain)
{
	return BT_OBJ GetBbGainInitValue(pGain);
}

DUT_BT_API int STDCALL Dut_Bt_SetBtPwrBbGainMaxMin(bool enable,	
							WORD MaxValue,		
							WORD MinValue)	
{
	return BT_OBJ SetBtPwrBbGainMaxMin(enable,	
							MaxValue,		
							MinValue);		
}

DUT_BT_API int STDCALL Dut_Bt_GetBtPwrBbGainMaxMin(bool *pEnable, 
					WORD *pMaxValue,		
					WORD *pMinValue)					
{
	return BT_OBJ GetBtPwrBbGainMaxMin(pEnable, 
					pMaxValue,		
					pMinValue);	
}


DUT_BT_API int STDCALL Dut_Bt_SetPpaGainStepMode(int Mode)
{
	return BT_OBJ SetPpaGainStepMode(Mode);
}
DUT_BT_API int STDCALL Dut_Bt_GetPpaGainStepMode(int *pMode)
{
	return BT_OBJ GetPpaGainStepMode(pMode);
}

DUT_BT_API int STDCALL Dut_Bt_SetAutoZeroMode(int Mode, WORD Value)
{
	return BT_OBJ SetAutoZeroMode(Mode, Value);
}

DUT_BT_API int STDCALL Dut_Bt_GetAutoZeroMode(int *pMode, WORD *pValue)
{
	return BT_OBJ GetAutoZeroMode(pMode, pValue);
}

DUT_BT_API int STDCALL Dut_Bt_SetTxPwrModeHighLow(int Mode)
{
	return BT_OBJ SetTxPwrModeHighLow(Mode);
}
DUT_BT_API int STDCALL Dut_Bt_GetTxPwrModeHighLow(int *pMode)
{
	return BT_OBJ GetTxPwrModeHighLow(pMode);
}
*/

#if !defined (_NO_EFUSE_)
DUT_BT_API int STDCALL Dut_Bt_SetEfuseData(int Block, int LenInWord, 
										   WORD *pData)
{
	return BT_OBJ SetEfuseData(Block, LenInWord, pData);
}
DUT_BT_API int STDCALL Dut_Bt_GetEfuseData(int Block, int LenInWord, 
										   WORD *pData)
{
	return BT_OBJ GetEfuseData(Block, LenInWord, pData);
}
#endif // #if !defined (_NO_EFUSE_)

DUT_BT_API int STDCALL Dut_Bt_GetFwReleaseNote(char *pReleaseNote)
{
	return BT_OBJ GetFwReleaseNote(pReleaseNote);
}

 

// not supported by this chip  
//DUT_BT_API int STDCALL Dut_Bt_TxBtCst(bool enable, int FreqOffsetinKHz)
//{
//	return BT_OBJ TxBtCst(enable, FreqOffsetinKHz);
//}

DUT_BT_API int STDCALL Dut_Bt_TxBtCw(bool enable) 
{
	return BT_OBJ TxBtCw(enable);
}

DUT_BT_API int STDCALL Dut_Bt_SetBtDisableBtuPwrCtl(bool DisableBtuPwrCtl)
{
	return BT_OBJ SetBtDisableBtuPwrCtl(DisableBtuPwrCtl);
}

DUT_BT_API int STDCALL Dut_Bt_GetBtDisableBtuPwrCtl(bool *pDisableBtuPwrCtl)
{
	return BT_OBJ GetBtDisableBtuPwrCtl(pDisableBtuPwrCtl);
}

#if !defined (_NO_EEPROM_)

DUT_BT_API int STDCALL Dut_Bt_ForceE2PromType(	DWORD		IfType, 
												DWORD		AddrWidth,
												DWORD		DeviceType)
{
	return BT_OBJ Bt_ForceE2PromType(	IfType, 
										AddrWidth,
										DeviceType);
}

DUT_BT_API int STDCALL Dut_Bt_GetE2PromType(	DWORD		*pIfType, 
						DWORD		*pAddrWidth,
						DWORD		*pDeviceType) 
{
	return BT_OBJ Bt_GetE2PromType(pIfType, pAddrWidth, pDeviceType);
}

DUT_BT_API int STDCALL Dut_Bt_SpiWriteDword(DWORD Address, DWORD data)
{
	return BT_OBJ Bt_SpiWriteData(Address, 1, &data, false);
}

DUT_BT_API int STDCALL Dut_Bt_SpiReadDword(DWORD Address, DWORD *data)
{
	return BT_OBJ Bt_SpiReadData(Address, 1, data);
}

DUT_BT_API int STDCALL Dut_Bt_DumpE2PData(DWORD StartAddress, DWORD LenInDw,
									DWORD *pAddressArray, 
									DWORD *pDataArray)
{
	return SHARED_OBJ DumpE2PData(DEVICETYPE_BT, BtObj,
							StartAddress, LenInDw,
			  				pAddressArray, 
			 				pDataArray);
}
DUT_BT_API int STDCALL Dut_Bt_WriteLumpedData(DWORD StartAddress, 
								   DWORD LenInDw, 
									DWORD *pDataArray)
{
	return SHARED_OBJ WriteLumpedData(	DEVICETYPE_BT, BtObj,
										StartAddress, 
										LenInDw, 
										pDataArray);
}

DUT_BT_API int STDCALL Dut_Bt_SpiGetBoot2Version(DWORD *pBoot2Version)
{
	return SHARED_OBJ SpiGetBoot2Version(DEVICETYPE_BT, BtObj, pBoot2Version);
}


DUT_BT_API int STDCALL Dut_Bt_EraseEEPROM(int section)
{
	return BT_OBJ Bt_EraseEEPROM(section);
}

#endif // #if !defined (_NO_EEPROM_)

/*
DUT_BT_API int STDCALL Dut_Bt_ResetAccum(void) 
{
	return BT_OBJ ResetAccum();
}

DUT_BT_API int STDCALL Dut_Bt_GetAccum(int *pAccum) 
{
	return BT_OBJ GetAccum(pAccum);
}
*/
// not supported by this chip  
/*
DUT_BT_API int STDCALL Dut_Bt_SetBtXPaPdetPolority(bool Polority_Neg)
{
	return BT_OBJ SetBtXPaPdetPolority(Polority_Neg);
}

DUT_BT_API int STDCALL Dut_Bt_GetBtXPaPdetPolority(bool *pPolority_Neg)
{
	return BT_OBJ GetBtXPaPdetPolority(pPolority_Neg);
}

DUT_BT_API int STDCALL Dut_Bt_SetBtPpaPgaPriority(bool PgaFirst)
{
	return BT_OBJ SetBtPpaPgaPriority(PgaFirst);
}

DUT_BT_API int STDCALL Dut_Bt_GetBtPpaPgaPriority(bool *pPgaFirst)
{
	return BT_OBJ GetBtPpaPgaPriority(pPgaFirst);
}

DUT_BT_API int STDCALL Dut_Bt_SetBtPaThermScaler(int ThermScaler)
{
	return BT_OBJ SetBtPaThermScaler(ThermScaler);
}

DUT_BT_API int STDCALL Dut_Bt_GetBtPaThermScaler(int *pThermScaler)
{
	return BT_OBJ GetBtPaThermScaler(pThermScaler);
}
 
DUT_BT_API int STDCALL Dut_Bt_SetBtPaBias(WORD PaBias1,	WORD PaBias2, bool PerformanceMod)
{
	return BT_OBJ SetBtPaBias(PaBias1,	PaBias2, PerformanceMod);
}

DUT_BT_API int STDCALL Dut_Bt_GetBtPaBias(WORD *pPaBias1, WORD *pPaBias2, bool PerformanceMode)
{
	return BT_OBJ GetBtPaBias(pPaBias1, pPaBias2, PerformanceMode);
}
// not supported by this chip  

DUT_BT_API int STDCALL Dut_Bt_GetTxIQ(DWORD *pExtMode, 
									  BYTE *pAlpha, BYTE *pBeta)
{
	return BT_OBJ GetTxIQ(pExtMode, pAlpha, pBeta);
}

DUT_BT_API int STDCALL Dut_Bt_SetTxIQ(DWORD extMode, 
									  BYTE Alpha, BYTE Beta)
{
	return BT_OBJ SetTxIQ(extMode, Alpha, Beta);
}
*/
// not supported by this chip  
//DUT_BT_API int STDCALL Dut_Bt_GetBtThermalSensorReading(DWORD *pValue)
//{
//	return BT_OBJ GetBtThermalSensorReading(pValue);
//}

DUT_BT_API int STDCALL Dut_Bt_SetBtThermalCtrlMode(DWORD Enable)
{
	return BT_OBJ SetBtThermalCtrlMode(Enable);
}

#ifdef _ENGINEERING_
DUT_BT_API int STDCALL Dut_Bt_GetBtDbgThermalCtrl(DWORD *pCtrlActive,
						DWORD *pTempAvg,
						DWORD *pTempCur,
						DWORD *pCorrDelta,
						DWORD *pTemp10Avg)
{
	return BT_OBJ GetBtDbgThermalCtrl(pCtrlActive,
						pTempAvg,
						pTempCur,
						pCorrDelta,
						pTemp10Avg);
}
#endif // #ifdef _ENGINEERING_

#if !defined (_NO_EEPROM_)
DUT_BT_API int STDCALL Dut_Bt_UpdateCalDataRevC_Bt(int PurgeAfter, char *FlexFileNameNoE2prom) 
{
	return SHARED_OBJ UpdateCalDataRevC_Bt(DEVICETYPE_BT, BtObj, PurgeAfter, FlexFileNameNoE2prom);
} 

DUT_BT_API int STDCALL Dut_Bt_GetCalDataRevC(int *CsC, int *CsCValue, char *FlexFileNameNoE2prom)
{
	return SHARED_OBJ GetCalDataRevC(DEVICETYPE_BT, BtObj, CsC, CsCValue, FlexFileNameNoE2prom);
} 
  
DUT_BT_API int STDCALL Dut_Bt_SetCalDataRevC(int PurgeAfter, 
											 char *FlexFileNameNoE2prom)
{
	return SHARED_OBJ SetCalDataRevC(DEVICETYPE_BT, BtObj, PurgeAfter, FlexFileNameNoE2prom);
}
 
/*
DUT_BT_API int STDCALL Dut_Bt_GetBdAddressFromE2P(BYTE BdAddress[SIZE_BDADDRESS_INBYTE])
{
	return SHARED_OBJ GetBdAddressFromE2P(DEVICETYPE_BT, BtObj, BdAddress);
}


DUT_BT_API int STDCALL Dut_Bt_SetRfPowerUseCal(double PwrdBm, int ClassMode, int useFeLoss)
{
	return BT_OBJ SetRfPowerUseCal(SharedObj, PwrdBm, ClassMode, useFeLoss);
}
 */
#if !defined(_FLASH_)
DUT_BT_API int STDCALL Dut_Bt_GetHeaderPresent(int *pPresent, DWORD *pVersion, int *CsC)
{
	return SHARED_OBJ GetHeaderPresent(DEVICETYPE_BT, BtObj, pPresent, pVersion, CsC);
}
#endif	//#if !defined(_FLASH_)

DUT_BT_API int STDCALL Dut_Bt_CheckCalDataSpace(int* pAvalable)  
{
	return SHARED_OBJ CheckCalDataSpace(DEVICETYPE_BT, BtObj, pAvalable); 
}

DUT_BT_API int STDCALL Dut_Bt_LoadCustomizedSettings(void)
{
	return SHARED_OBJ LoadCustomizedSettings(DEVICETYPE_BT, BtObj);
}

DUT_BT_API int STDCALL Dut_Bt_FlexSpiDL(bool erase, char *fileName)
{
   return SHARED_OBJ FlexSpiDL(DEVICETYPE_BT, BtObj, erase, fileName);
}

DUT_BT_API int STDCALL Dut_Bt_FlexSpiCompare(char *fileName)  
{
   return SHARED_OBJ FlexSpiCompare(DEVICETYPE_BT, BtObj, fileName); 
}

DUT_BT_API int STDCALL Dut_Bt_CompareE2PData(
					DWORD  *RefArray,int SizeInByte)
{
	return SHARED_OBJ CompareE2PData(DEVICETYPE_BT, BtObj, 
					 RefArray, SizeInByte);
}

#endif // #if !defined (_NO_EEPROM_)




#if defined (_HCI_PROTOCAL_)
/*
DUT_BT_API int STDCALL Dut_Bt_AclLinkTest(int PacketType, int PacketCount, 
					  int PayloadLengthInByte, int PayloadPattern, 
					  bool MasterAsTx, bool DutAsMaster, 
					  bool stress, 
						int *pRxPacket,
						int *pMissingPacket,
						double *pPer, double*pBer)
{
 	
	return Test_AclLinkTest(BtObj,			
						PacketType,				PacketCount, 
						PayloadLengthInByte,	PayloadPattern, 
						MasterAsTx,				DutAsMaster, 
						stress, 
						pRxPacket,
						pMissingPacket,
						pPer, pBer); 
 						
}
*/
/*
// not supported by this chip  

DUT_BT_API int STDCALL Dut_Bt_ClearRxCnts(int LinkIndex)
{
	return BT_OBJ BtClearRxCnts( LinkIndex);
}
DUT_BT_API int STDCALL Dut_Bt_GetRxInfo(	int LinkIndex, 
				int	*pRssiQual,
				int	*pRssiValue,
				int	*pReceivedCrcCnt, 
				int *pHecMatchCnt,
				int *pCrcErrorCnt,
				int *pFECCorrectCnt)
{
	return BT_OBJ BtGetRxInfo(LinkIndex, 
				pRssiQual,
				pRssiValue,
				pReceivedCrcCnt, 
				pHecMatchCnt,
				pCrcErrorCnt,
				pFECCorrectCnt);

}
*/ 
#endif // #if defined (_HCI_PROTOCAL_)


#if defined (_W8688_) || defined (_W8682_) || defined (_W8787_) || defined(_W8782_)
/*
DUT_BT_API int STDCALL Dut_Bt_SetBcaMode(int Mode)
{
	
	return BT_OBJ SetBcaMode(Mode);
}

DUT_BT_API int STDCALL Dut_Bt_GetBcaMode(int *pMode)
{
	
	return BT_OBJ GetBcaMode(pMode);
}
*/

DUT_BT_API int STDCALL Dut_Bt_SetFE_BTOnly(bool bOn)
{
	return BT_OBJ SetFE_BTOnly(bOn);		
}

#endif //#if defined (_W8688_) || defined (_W8682_) || defined (_W8787_) 



#if defined (_HCI_PROTOCAL_)

DUT_BT_API int STDCALL Dut_Bt_SetDeviceDiscoverable(void)
{
	
	return BT_OBJ SetDeviceDiscoverable();
}

/*
DUT_BT_API int STDCALL Dut_Bt_Hold(WORD hConnection, 
									WORD HoldMaxInterval, 
									WORD HoldMinInterval)
{
	int status=0;
	WORD MaxInt=0, MinInt=0;


	status= BT_OBJ Hold(hConnection, HoldMaxInterval, HoldMinInterval);
 	DebugLogRite(" Hold: %08X\n",status);


	status= BT_OBJ Park(hConnection, HoldMaxInterval, HoldMinInterval);
 	DebugLogRite(" Park: %08X\n",status);

	status= BT_OBJ ExitPark(hConnection);
 	DebugLogRite(" ExitPark: %08X\n",status);

	status= BT_OBJ Sniff(hConnection, 
			HoldMaxInterval, HoldMinInterval,
			1,		1000);
 	DebugLogRite(" Sniff: %08X\n",status);

	status= BT_OBJ ExitSniff( hConnection);
 	DebugLogRite(" ExitSniff: %08X\n",status);

return status;
}




DUT_BT_API int STDCALL Dut_Bt_Test_Hold(int modeOnOff, 
							  WORD HoldMaxInterval, 
							  WORD HoldMinInterval,
							  bool PartnerAutoCtrl)
{
	return BT_OBJ Test_Hold(modeOnOff, 
							  HoldMaxInterval, 
							  HoldMinInterval,
							  PartnerAutoCtrl);
}


DUT_BT_API int STDCALL Dut_Bt_Test_Park(int modeOnOff, 
							  WORD BeaconMaxInterval, 
							  WORD BeaconMinInterval,
							  bool partnerAutoCtrl)
{
	return BT_OBJ Test_Park(modeOnOff, 
						BeaconMaxInterval, 
						BeaconMinInterval,
						partnerAutoCtrl);
}

DUT_BT_API int STDCALL Dut_Bt_Test_Sniff(int modeOnOff, 
								WORD SniffMaxInterval,	
								WORD SniffMinInterval,
								WORD SniffAttempt,		
								WORD SniffTimeout,
								bool partnerAutoCtrl)
{
	return BT_OBJ Test_Sniff(modeOnOff, 
						SniffMaxInterval,	
						SniffMinInterval,
						SniffAttempt,		
						SniffTimeout,
						partnerAutoCtrl);
}
*/
#endif // #if defined (_HCI_PROTOCAL_)





 

#if 1 //def _ENGINEERING_ 
/*
DUT_BT_API int STDCALL Dut_Bt_GetAnnexType40Data( 
					   WORD *pVid,	WORD *pPid,	
					   BYTE *pChipId,	BYTE *pChipVersion,					   
					   BYTE *pFlag_SettingPrev, 
					   BYTE *pNumOfRailAvaiableInChip,
						BYTE pPowerDownMode[MAX_PM_RAIL_PW886],

					   BYTE pNormalMode[MAX_PM_RAIL_PW886], 
					   BYTE pSleepMode[MAX_PM_RAIL_PW886],
					   BYTE *pDpSlp)
{
	return SHARED_OBJ GetAnnexType40Data( 
						pVid,			pPid,	
						pChipId,		pChipVersion,					   
						pFlag_SettingPrev, 
						pNumOfRailAvaiableInChip,
						pPowerDownMode,

						pNormalMode, 
						pSleepMode,
						pDpSlp);
}
DUT_BT_API int STDCALL Dut_Bt_SetAnnexType40Data(WORD *pVid,		WORD *pPid,	
						BYTE *pChipId,	BYTE *pChipVersion,					   
						BYTE *pFlag_SettingPrev, 
						BYTE *pNumOfRailAvaiableInChip,
						BYTE pPowerDownMode[MAX_PM_RAIL_PW886],

					   BYTE pNormalMode[MAX_PM_RAIL_PW886], 
					   BYTE pSleepMode[MAX_PM_RAIL_PW886],
					   BYTE *pDpSlp)
{
	return SHARED_OBJ SetAnnexType40Data(pVid,	pPid,	
					pChipId,	pChipVersion,					   
					pFlag_SettingPrev, 
					pNumOfRailAvaiableInChip,
					pPowerDownMode,
					pNormalMode, 
					pSleepMode,
					pDpSlp);
}
*/

 


#if defined (_HCI_PROTOCAL_)
DUT_BT_API int STDCALL Dut_Bt_SetMrvlTempCompModeHC(int		SwPwrCtrlMode,
													WORD	RefGain1,
 													WORD	RefGain2,
 													BYTE	RefTemp,
 													BYTE	TempGain)
{
	return BT_OBJ SetMrvlTempCompModeHC(SwPwrCtrlMode,
									RefGain1,
 									RefGain2,
 									RefTemp,
 									TempGain);
}
#endif // #if defined (_HCI_PROTOCAL_)


#if defined (_HCI_PROTOCAL_)
DUT_BT_API int STDCALL Dut_Bt_SetRfDynamicClkCtrl(int DynClkTimer,
						int	MultiSlotProtect,
						int	AutoDetect,
						int	DynClkValue,
						int	DynClkEnable,
						int	DynClkStartTime,
						int	DynClkFinishTime,
						int	DynClkTimedEnable)
{
	return BT_OBJ SetRfDynamicClkCtrl(DynClkTimer,
						MultiSlotProtect,
						AutoDetect,
						DynClkValue,
						DynClkEnable,
						DynClkStartTime,
						DynClkFinishTime,
						DynClkTimedEnable);
}


DUT_BT_API int STDCALL Dut_Bt_SetBtSleepMode(int PowerMode)
{
	return BT_OBJ SetBtSleepMode(PowerMode);
}


DUT_BT_API int STDCALL Dut_Bt_SetWakeUpMethod(int HostWakeUpMethod,
					int	HostWakeUpGPIO,
					int	DeviceWakeUpMethod,
					int	DeviceWakeUpGPIO)
{
	return BT_OBJ SetWakeUpMethod(HostWakeUpMethod,
					HostWakeUpGPIO,
					DeviceWakeUpMethod,
					DeviceWakeUpGPIO);
}


DUT_BT_API int STDCALL Dut_Bt_SetMrvlHiuModuleConfig(int ModuleShutDownBringUp, 
						   int HostIfActive, 
						   int HostIfType)
{
	return BT_OBJ SetMrvlHiuModuleConfig(ModuleShutDownBringUp, 
						   HostIfActive, 
						   HostIfType);
}

DUT_BT_API int STDCALL Dut_Bt_SetMrvlRadioPerformanceMode(	int	MrvlRxPowerMode,	 
 									int	PerfModeBdrRssiTh,		
									int	PerfModeEdrRssiTh,		 
									int	NormModeBdrRssiTh,	 
									int	NormModeEdrRssiTh,	 

									int	PerfModeBdrCrcTh,	 
									int	PerfModeEdrCrcTh,	 
									int	NormModeBdrCrcTh,		 
									int	NormModeEdrCrcTh,		 
									int	TxModeInTestMode,	 
									int	RxModeInTestMode,	 
									int	RxModeInOperationMode,	 
									int	RssiCrcSampleInterval, 
									int	LowPowerScanMode)
{
	return BT_OBJ SetMrvlRadioPerformanceMode(	MrvlRxPowerMode,	 
 									PerfModeBdrRssiTh,		
									PerfModeEdrRssiTh,		 
									NormModeBdrRssiTh,	 
									NormModeEdrRssiTh,	 

									PerfModeBdrCrcTh,	 
									PerfModeEdrCrcTh,	 
									NormModeBdrCrcTh,		 
									NormModeEdrCrcTh,		 
									TxModeInTestMode,	 
									RxModeInTestMode,	 
									RxModeInOperationMode,	 
									RssiCrcSampleInterval, 
									LowPowerScanMode);
}


DUT_BT_API int STDCALL Dut_Bt_SetMrvlArmDynamicClkCtrl(int SetNcoMode,	 
 							int DynamicClockChange 	 )
{
	return BT_OBJ SetMrvlArmDynamicClkCtrl(SetNcoMode,	 
 							DynamicClockChange 	 );
}

DUT_BT_API int STDCALL Dut_Bt_SetBtXCompModeConfig(int BTCoexistenceMode,	 
 									int	PriorityMode_Wlan,
									int PriorityMode_UpdateDefInPta,
									int PriorityDelay,
									int HighPriority_BitMap)
{
	return BT_OBJ SetBtXCompModeConfig(BTCoexistenceMode,	 
 									PriorityMode_Wlan,
									PriorityMode_UpdateDefInPta,
									PriorityDelay,
									HighPriority_BitMap);
}
DUT_BT_API int STDCALL Dut_Bt_SetBtMrvlModeConfig(int BTCoexistenceMode,	 
 									int PriorityMode_Wlan,
									int PriorityMode_UpdateDefInPta,
									int PriorityDelay,
									int HighPriority_BitMap, 
									int MediumPriority_BitMap,
									int BtEndTimer,
									int BtFreqLowBand,
									int BtFreqHighBand )
{
	return BT_OBJ SetBtMrvlModeConfig(BTCoexistenceMode,	 
 									PriorityMode_Wlan,
									PriorityMode_UpdateDefInPta,
									PriorityDelay,
									HighPriority_BitMap, 
									MediumPriority_BitMap,
									BtEndTimer,
									BtFreqLowBand,
									BtFreqHighBand );
}

DUT_BT_API int STDCALL Dut_Bt_SetBtMrvlMode2Config(int BTCoexistenceMode,	 
 									int	NonPeriodicMode_Enable, 
									int NonPeriodicMode_BtTimerTlead,
									int NonPeriodicMode_BtTimerTtail,
									int PeriodicMode_BtSyncPeriod,
									int PeriodicMode_BtSyncPulseWidth,
									int PeriodicMode_BtTsyncLead,
									int PeriodicMode_BtSyncMode)
{
	return BT_OBJ SetBtMrvlMode2Config(BTCoexistenceMode,	 
 									NonPeriodicMode_Enable, 
									NonPeriodicMode_BtTimerTlead,
									NonPeriodicMode_BtTimerTtail,
									PeriodicMode_BtSyncPeriod,
									PeriodicMode_BtSyncPulseWidth,
									PeriodicMode_BtTsyncLead,
									PeriodicMode_BtSyncMode);
}

#endif // #if defined (_HCI_PROTOCAL_)

#endif // #ifdef _ENGINEERING_ 


