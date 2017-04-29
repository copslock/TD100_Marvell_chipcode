
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GOLDENAPMIMODLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GOLDEN87XXDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef GOLDEN87XXDLL_EXPORTS

#define GOLDEN87XXDLL_API __declspec(dllexport)
#else
#define GOLDEN87XXDLL_API __declspec(dllimport)
#endif
#include "Golden87XXDll.hc"
// This class is exported from the GoldenAPMimoDll.dll
//;class GOLDEN87XXDLL_API CGoldenAPMimoDll {
//;public:
//;	CGoldenAPMimoDll(void);
	// TODO: add your methods here.
	
	
//;}; 


//extern GOLDEN87XXDLL_API int nGoldenAPMimoDll;

GOLDEN87XXDLL_API int fnGoldenAPMimoDll(void);



GOLDEN87XXDLL_API int STDCALL Gru_Wlan_GoldenInit(void);

GOLDEN87XXDLL_API int STDCALL Gru_Wlan_GoldenClose(void);

GOLDEN87XXDLL_API int STDCALL Gru_Wlan_SwitchBand(int band, DWORD DeviceId=MIMODEVICE0);
GOLDEN87XXDLL_API int STDCALL Gru_Wlan_CurrentBand(int *band, DWORD DeviceId=MIMODEVICE0);

GOLDEN87XXDLL_API int STDCALL Gru_Wlan_SwitchChannelBw(int Bw);
GOLDEN87XXDLL_API int STDCALL Gru_Wlan_SwitchChannel(int channel);
GOLDEN87XXDLL_API int STDCALL Gru_Wlan_GetChannel(int *pChannel, int *pFreqInMHz);
GOLDEN87XXDLL_API int STDCALL Gru_Wlan_StartContinuousTx(int enable, int DataRate 
#ifdef MIMO
														  , int ActSub=0, int shortGI=0
#endif //#ifdef MIMO
														  );
GOLDEN87XXDLL_API int STDCALL Gru_Wlan_StartCWTx(int enable);
GOLDEN87XXDLL_API int STDCALL Gru_Wlan_SelectTxAntenna(int TxAnt);
GOLDEN87XXDLL_API int STDCALL Gru_Wlan_SelectRxAntenna(int RxAnt); 
GOLDEN87XXDLL_API int STDCALL Gru_Wlan_TxMulticastFrame( int dataRate, DWORD pattern,
														 int length, int cnt, int ShortPreamble, 
#ifdef MIMO
														 int ActSub, int shortGI, 
#endif //#ifdef MIMO
														 char Bssid[]
#ifdef MIMO
														 , DWORD AdvCoding=0
#endif //#ifdef MIMO
														 );
GOLDEN87XXDLL_API int STDCALL Gru_Wlan_ClearRxPcktCount(void);
GOLDEN87XXDLL_API int STDCALL Gru_Wlan_ReadRxPcktCount(DWORD *RxCount, DWORD *McCount, DWORD *ErrCount);
GOLDEN87XXDLL_API int STDCALL Gru_Wlan_BssidFilterMode(int Mode, char Bssid[]); 
GOLDEN87XXDLL_API int STDCALL Gru_Wlan_SetTxDutyCycle (BOOL enable, int dataRate, 
													   int percentage, int ShortPreamble
#ifdef MIMO
													   , 
													   int ActSub, int shortGI,int AdvCoding
#endif //#ifdef MIMO
													   );
GOLDEN87XXDLL_API int STDCALL Gru_Wlan_SetTxDataRate(int cmd);
GOLDEN87XXDLL_API int STDCALL Gru_Wlan_GetTxDataRate(int *cmd);
GOLDEN87XXDLL_API int STDCALL Gru_Wlan_CarrierSuppression(BOOL enable);

GOLDEN87XXDLL_API int STDCALL Gru_Wlan_TxToneForRxIqTest(BOOL enable);
GOLDEN87XXDLL_API int STDCALL Gru_Wlan_SoftReset(void);



GOLDEN87XXDLL_API int STDCALL Gru_Bt_GoldenInit(void);

GOLDEN87XXDLL_API int STDCALL Gru_Bt_GoldenClose(void);
GOLDEN87XXDLL_API int STDCALL Gru_Bt_SwitchChannel(int channel);
GOLDEN87XXDLL_API int STDCALL Gru_Bt_SwitchAntenna(int ant); 

GOLDEN87XXDLL_API int STDCALL Gru_Bt_StartContinuousTx(bool enable,			int dataRate, 
			 int payloadPattern,	bool payloadOnly);
GOLDEN87XXDLL_API int STDCALL Gru_Bt_SetTxDutyCycleHop (bool enable,	int PacketType, 
				  int PayLoadPattern, 
				  int PayloadLength,int RandowmHopping, int Interval);

GOLDEN87XXDLL_API int STDCALL Gru_Bt_StartRxTest(int RxChannel,
					int PacketNumber,
					int PacketType, 
					int PayLoadPattern, 
					int PayloadLength,	 
					char *pTxBdAddress);


GOLDEN87XXDLL_API int STDCALL Gru_Bt_StopRxTest( 
					int *pStatus_Complete0_Abort1,
/*
					int *pTotalPcktCount,
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
					int *pRssiAvg);


