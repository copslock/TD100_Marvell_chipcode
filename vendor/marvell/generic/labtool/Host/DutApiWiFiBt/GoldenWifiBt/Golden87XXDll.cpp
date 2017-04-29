// GoldenAPMimoDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "../../DutCommon/Clss_os.h"  



#include DUTCLASS_WLAN_H_PATH		//"../DutWlanApi878XDll/DutWlanApiClss.h"
#include DUTCLASS_BT_H_PATH			//"../DutBtApi878XDll/DutBtApiClss.h" 
#include DUTDLL_SHARED_H_PATH	 
#include "Golden87XXDll.h"
 

#ifndef _LINUX_
#define WLAN_OBJ 	WlanObj->
#define BT_OBJ 		BtObj->
#define SHARED_OBJ	SharedObj->

#ifdef GOLDEN87XXDLL_EXPORTS
	DutWlanApiClss 	*WlanObj=NULL;
	DutBtApiClss 	*BtObj=NULL;
	DutSharedClss 	*SharedObj=NULL;

//DutApiClss hEagle;
BOOL APIENTRY DllMain( HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
			if(NULL == SharedObj) 
				SharedObj = new (DutSharedClss);
			break;
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			if(SharedObj) 
			{
				delete(SharedObj);
				SharedObj = NULL;
			}
			break;
    }
    return TRUE;
}

#else // #ifdef GOLDEN87XXDLL_EXPORTS
	DutWlanApiClss 	*WlanObj=NULL;
	DutBtApiClss 	*BtObj=NULL;
	DutSharedApiClss 	*SharedObj=NULL;
#endif // #ifdef GOLDEN87XXDLL_EXPORTS


#else //#ifndef _LINUX_ 
#define WLAN_OBJ 	 
#define BT_OBJ 		 
#define SHARED_OBJ 	 
#endif	//#ifndef _LINUX_ 


// This is an example of an exported variable
GOLDEN87XXDLL_API int nGoldenAPMimoDll=0;

// This is an example of an exported function.
GOLDEN87XXDLL_API int fnGoldenAPMimoDll(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see GoldenAPMimoDll.h for the class definition

//;CGoldenAPMimoDll::CGoldenAPMimoDll()
//;{ 
//;	return; 
//;}



GOLDEN87XXDLL_API int STDCALL Gru_Wlan_GoldenInit(void)
{
	WlanObj = new (DutWlanApiClss);

	return WLAN_OBJ InitConnection();
}

GOLDEN87XXDLL_API int STDCALL Gru_Wlan_GoldenClose(void)
{
	return WLAN_OBJ Disconnection();
}

GOLDEN87XXDLL_API int STDCALL Gru_Wlan_SwitchBand(int band,DWORD DeviceId)
{
	return WLAN_OBJ SetBandAG(band);
}

GOLDEN87XXDLL_API int STDCALL  Gru_Wlan_CurrentBand(int *band,DWORD DeviceId)
{
	return WLAN_OBJ GetBandAG(band);
}

#ifdef MIMO
GOLDEN87XXDLL_API int STDCALL Gru_Wlan_SwitchChannelBw(int Bw)
{
	return WLAN_OBJ SetChannelBw(Bw);
}
#endif //#ifdef MIMO


GOLDEN87XXDLL_API int STDCALL Gru_Wlan_SwitchChannel(int channel)
{
   int status=ERROR_NONE;
   int pwr=12;


	status = WLAN_OBJ SetRfChannel(channel); 
 	DebugLogRite("SetRfChannel status 0x%X\n", status);
	if(status) return status; 

	status = WLAN_OBJ SetRfPowerCal(&pwr, 2, 0, 0); 
 	DebugLogRite("SetRfPowerCal status 0x%X\n", status);

	return status;
}
GOLDEN87XXDLL_API int STDCALL Gru_Wlan_GetChannel(int *pChannel, int *pFreqInMHz)
{
	int status=ERROR_NONE;

	status = WLAN_OBJ GetRfChannel(pChannel, pFreqInMHz); 
 	DebugLogRite("GetRfChannel status 0x%X\n", status);
	return status; 
}

GOLDEN87XXDLL_API int STDCALL Gru_Wlan_StartContinuousTx(int enable, int DataRate
#ifdef MIMO
														  , int ActSub, int shortGI
#endif //#ifdef MIMO
														  )
{
   int status=0;
   status = WLAN_OBJ SetTxDataRate(DataRate);
	if(status) return status; 
   status = WLAN_OBJ SetTxContMode(enable, 0, 0, 0
#ifdef MIMO
	   , ActSub
#endif //#ifdef MIMO
	   );
 	if(status) return status; 
	return status;			
}

GOLDEN87XXDLL_API int STDCALL Gru_Wlan_StartCWTx(int enable)
{
	return  WLAN_OBJ SetTxContMode(enable,1,0,0
#ifdef MIMO
		,ACTSUBCH_40MHZ_BOTH
#endif //#ifdef MIMO
		);
}

GOLDEN87XXDLL_API int STDCALL Gru_Wlan_SelectTxAntenna(int TxAnt)
{
	return  WLAN_OBJ SetTxAntenna(TxAnt);
}

GOLDEN87XXDLL_API int STDCALL Gru_Wlan_SelectRxAntenna(int RxAnt)
{
	return  WLAN_OBJ SetRxAntenna(RxAnt);
}

GOLDEN87XXDLL_API int STDCALL Gru_Wlan_SoftReset(void)
{
	return  WLAN_OBJ SoftReset();
}

GOLDEN87XXDLL_API int STDCALL Gru_Wlan_TxMulticastFrame(int dataRate, DWORD pattern, 
														 int length, int cnt, int ShortPreamble, 
#ifdef MIMO
														 int ActSub, int shortGI, 
#endif //#ifdef MIMO
														 char Bssid[]
#ifdef MIMO
														 , DWORD AdvCoding
#endif //#ifdef MIMO
														 )
{
  return WLAN_OBJ TxBrdCstPkts((DWORD) dataRate,	 pattern, 
							 (DWORD)length,		(DWORD) cnt, 
							 ShortPreamble, Bssid
#ifdef MIMO
							,
							(DWORD)ActSub, 
 							(DWORD)shortGI, 
							AdvCoding
#endif //#ifdef MIMO
							); 
}

GOLDEN87XXDLL_API int STDCALL Gru_Wlan_ClearRxPcktCount(void)
{
	return WLAN_OBJ ClearRxPckt();
}

GOLDEN87XXDLL_API int STDCALL Gru_Wlan_ReadRxPcktCount(DWORD *RxCount, DWORD *McCount, DWORD *ErrCount)
{
	return WLAN_OBJ GetRxPckt(RxCount, McCount, ErrCount);
}

GOLDEN87XXDLL_API int  STDCALL Gru_Wlan_BssidFilterMode(int Mode, char Bssid[])
{
	int status = ERROR_NONE;

	if(!Mode)
		status = WLAN_OBJ EnableBssidFilter(Mode);
	else
	{
		status = WLAN_OBJ EnableBssidFilter(Mode, (unsigned char*)Bssid); 
	}

 	return status;
}

GOLDEN87XXDLL_API int STDCALL Gru_Wlan_SetTxDutyCycle (BOOL enable, int dataRate, 
														int percentage, int ShortPreamble 
#ifdef MIMO
														,
														int ActSub, int shortGI, int AdvCoding
#endif //#ifdef MIMO
														)
{
	return WLAN_OBJ SetTxDutyCycle(enable, dataRate, percentage, 0x0,	ShortPreamble

#ifdef MIMO
							,
							(DWORD)ActSub,	(DWORD)shortGI, 
							(DWORD)	AdvCoding
#endif //#ifdef MIMO
							);   
}

GOLDEN87XXDLL_API int STDCALL Gru_Wlan_SetTxDataRate(int cmd)
{
	return WLAN_OBJ SetTxDataRate(cmd);
}

GOLDEN87XXDLL_API int STDCALL Gru_Wlan_GetTxDataRate(int* cmd)
{
	return WLAN_OBJ GetTxDataRate(cmd);
}

GOLDEN87XXDLL_API int STDCALL Gru_Wlan_CarrierSuppression(BOOL enable)
{
	return WLAN_OBJ SetTxCarrierSuppression(enable);
}

#ifdef MIMO
GOLDEN87XXDLL_API int STDCALL Gru_Wlan_TxToneForRxIqTest(BOOL enable)
{	
	int status =0, FreqInMHz=0;
	static int channel=0;

	status = WLAN_OBJ SetTxContMode(0,1,0,0,ACTSUBCH_40MHZ_BOTH);
	if(status) return status;

	if(enable)
	{
	status = WLAN_OBJ GetRfChannel(&channel, &FreqInMHz); 
	if(status) return status;

	FreqInMHz+=1;

	status = WLAN_OBJ SetRfChannelByFreq(FreqInMHz);
	if(status) return status;

	status = WLAN_OBJ SetTxContMode(enable,1,0,0
#ifdef MIMO
						,ACTSUBCH_40MHZ_BOTH
#endif //#ifdef MIMO
);
	}
	else
	{
 
	if(channel !=0) 
		status = WLAN_OBJ SetRfChannel(channel); 
	if(status) return status;

	}
	  return status;
}
#endif //#ifdef MIMO

//*/


GOLDEN87XXDLL_API int STDCALL Gru_Bt_GoldenInit(void)
{

	BtObj = new (DutBtApiClss);

	BT_OBJ OpenDevice();

	return  BT_OBJ SetRfPowerUseCal(SharedObj, 0, 0);
}

GOLDEN87XXDLL_API int STDCALL Gru_Bt_GoldenClose(void)
{
	return BT_OBJ CloseDevice();
}

GOLDEN87XXDLL_API int STDCALL Gru_Bt_SwitchAntenna(int ant) 
{
	return BT_OBJ SetTrSwAntenna(ant);
}

GOLDEN87XXDLL_API int STDCALL Gru_Bt_SwitchChannel(int channel) 
{
	return BT_OBJ SetBtChannel(channel);
}

GOLDEN87XXDLL_API int STDCALL Gru_Bt_StartContinuousTx(bool enable,			int dataRate, 
			 int payloadPattern,	bool payloadOnly) 
{
	return BT_OBJ TxBtCont(enable,			dataRate, 
			 payloadPattern,	payloadOnly);
}

GOLDEN87XXDLL_API int STDCALL Gru_Bt_SetTxDutyCycleHop (bool enable,	int PacketType, 
				  int PayLoadPattern, 
				  int PayloadLength,int RandowmHopping, int Interval)
{
	return BT_OBJ MfgMrvlTxTest(enable,	PacketType, 
				  PayLoadPattern, 
				  PayloadLength,	RandowmHopping, Interval);
}
 

GOLDEN87XXDLL_API int STDCALL Gru_Bt_StartRxTest(int RxChannel,
					int PacketNumber,
					int PacketType, 
					int PayLoadPattern, 
					int PayloadLength,	 
					char *pTxBdAddress)
{

	return BT_OBJ MfgMrvlRxTest(	RxChannel,
									PacketNumber,
									PacketType, 
									PayLoadPattern, 
									PayloadLength,	 
									pTxBdAddress);
}


GOLDEN87XXDLL_API int STDCALL Gru_Bt_StopRxTest( 
					int *pStatus_Complete0_Abort1,
/*					int *pTotalPcktCount,
					int *pSuccessfulCorrelation,
					int *pTotalRxedPkt,
					int *pHecMatchCount,
					int *pHecMatchCrcPckts,
					int *pHdrErrCount,
					int *pCrcErrCount,
					int *pNoErrPktCount,
					int *pTotalByteCount,		 
					int *pErrorBits,	 
					double *pPER,  
					double *pBER,
*/
					int *pRssiAvg)
{
	return BT_OBJ MfgMrvlRxTestResult(
					pStatus_Complete0_Abort1,
#if 0
					pTotalPcktCount,
					pSuccessfulCorrelation,
					pTotalRxedPkt,
					pHecMatchCount,
					pHecMatchCrcPckts,
					pHdrErrCount,
					pCrcErrCount,
					pNoErrPktCount,
					pTotalByteCount,		 
					pErrorBits,	 
					pPER,  
					pBER,
#else
					NULL, NULL, NULL, NULL, NULL, NULL, 
					NULL, NULL, NULL, NULL, NULL, NULL, 
#endif
					pRssiAvg);
}
