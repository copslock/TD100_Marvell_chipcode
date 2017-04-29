#ifdef _HCI_PROTOCAL_
//GruApiClss.h
#ifndef _GRU_API_H_
#define _GRU_API_H_

#include "../../DutCommon/Clss_os.h"


#include DUTCLASS_BT_H_PATH		//"../DutBtApi878XDll/DutBtApiClss.h"   
//#include "../DutBtApi.hc"
 
//#if !defined(AFX_NDISIF_H__90DC2111_AEB5_435E_B719_6A54BEB076A6__INCLUDED_)
//#define AFX_NDISIF_H__90DC2111_AEB5_435E_B719_6A54BEB076A6__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

//#define DutBtApiClssHeader	DutBtApiClss::
#ifndef _NO_CLASS_
class GruBtApiClss : public DutBtApiClss  
{
#endif //#ifndef _NO_CLASS_ 
//#ifdef _LINUX_
//typedef unsigned long DWORD;
//typedef unsigned char BYTE;
//typedef unsigned short WORD;
////#define NULL 0
//#endif	//#ifdef _LINUX_


#ifndef _NO_CLASS_
private:
#endif //#ifndef _NO_CLASS_
/*
int PokeRegDword(DWORD address, DWORD data);
int PeekRegDword(DWORD address, DWORD *data);
int PokeBtRegDword(DWORD address, DWORD data);
int PeekBtRegDword(DWORD address, DWORD *data);
int PokeBtRegWord(DWORD address, WORD data);
int PeekBtRegWord(DWORD address, WORD *data);
int PokeBtRegByte(DWORD address, BYTE data);
int PeekBtRegByte(DWORD address, BYTE *data);

   
int FormularNormal(DWORD RefLoIn100KHz, DWORD ChannelFreqInMHz, 
									  WORD *pSetting0, WORD *pSetting1);
int CalCh();
int GetRefLo(DWORD *pRefLo);
int CheckBrfChipReady(void);
int SetChannelSetting(WORD ChNum, WORD Setting0, WORD Setting1);
int SetChannel2Setting(WORD ChNum, WORD Setting0, WORD Setting1);

int BrfType(WORD *RadioType);
int wait4DbusDone();



int  MfgBtCmdTemplete(void);
void StuffHeader(void *TxBuf, DWORD cmd, DWORD action);
*/
#ifndef _NO_CLASS_
public:
 GruBtApiClss();
 ~GruBtApiClss();
#endif //#ifndef _NO_CLASS_

int Version(void);
char* About(void);

int OpenDevice( );
/*
int CloseDevice( );
int GetBDAddress(BYTE *BdAddress);
int ResetBt(void);
int Inquriy(void);//(ULONG LAP, PINQUIRY_RESP pResp, int& NumResp);
int SetPageTimeOut(USHORT TimeOut);
int SetEventFilter(PUCHAR pFilter, UCHAR Length);
int SetEventMask(PUCHAR pMask, UCHAR Length);

int WriteMemDword(DWORD address, DWORD data);
int ReadMemDword(DWORD address, DWORD *data);
int WriteBtuReg(DWORD address, WORD data);
int WriteBtuReg8(DWORD address, BYTE data);
int ReadBtuReg(DWORD address, WORD *data);
int WriteBrfReg(DWORD address, WORD data);
int ReadBrfReg(DWORD address, WORD *data);




int SetBtChannel(int channelNum, BOOL BT2=0);
int SetBtChannelByFreq(int ChannelFreq, BOOL BT2=0);
int GetBtChannel(int *pChannelNum, int *pChannelFreq=NULL, BOOL BT2=0);
int SetBtChBank(int BankNum, int channelNum=-1);
int GetBtChBank(int BankNum, int *pChannelNum);
int GetBtLowLoMode(BOOL *pLowLoMode);
int SetBtLowLoMode(BOOL LowLoMode);

int SetBt2ChHopping(BOOL enable);
int GetBt2ChHopping(BOOL *pEnable);

int SetBtPseudoCode(DWORD option);
int GetBtPseudoCode(DWORD *pOption);

int SetBtPwrControlClass(DWORD option);				//0:MRVL, 1:Class2, 2:Class1 3: Class2+XPA
int GetBtPwrControlClass(DWORD *pOption);

int SetBtPwrPpaPgaInitValue(WORD PpaValue,		WORD PgaValue);		// 0 internal mode 1: external mode
int GetBtPwrPpaPgaInitValue(WORD *pPpaValue,	WORD *pPgaValue);
 
int BrfInit(void);


/*
int SetBtPwrPpaPgaMaxMin(DWORD enablePpa,			DWORD enablePga,	
							   WORD PpaMaxValue,		WORD PpaMinValue,	
							   WORD PgaMaxValue,		WORD PgaMinValue);		// 0 internal mode 1: external mode
int GetBtPwrPpaPgaMaxMin(DWORD *pEnablePpa,		DWORD *pEnablePga,	
							   WORD *pPpaMaxValue,		WORD *pPpaMinValue,	
							   WORD *pPgaMaxValue,		WORD *pPgaMinValue);		// 0 internal mode 1: external mode
* /
int SetBtPwrPpaMaxMin(BOOL enable,	WORD MaxValue,		WORD MinValue );		// 0 internal mode 1: external mode
int GetBtPwrPpaMaxMin(BOOL *pEnable, WORD *pMaxValue,		WORD *pMinValue);		// 0 internal mode 1: external mode
int SetBtPwrPgaMaxMin(BOOL enable,	WORD MaxValue,		WORD MinValue );		// 0 internal mode 1: external mode
int GetBtPwrPgaMaxMin(BOOL *pEnable, WORD *pMaxValue,		WORD *pMinValue);		// 0 internal mode 1: external mode

int SetBtPwrPpaExtMode(BOOL extMode, WORD value);		// 0 internal mode 1: external mode
int GetBtPwrPpaExtMode(BOOL *pExtMode, WORD *pValue);
int SetBtPwrPgaExtMode(BOOL extMode, WORD value);		// 0 internal mode 1: external mode
int GetBtPwrPgaExtMode(BOOL *pExtMode, WORD *pValue);

int SetBtPwrSetting(WORD ThHi, WORD ThLo);			 
int GetBtPwrSetting(WORD *pThHi, WORD *pThLo);

int TxBtCont(BOOL enable,			int dataRate, 
			 int payloadPattern,	BOOL payloadOnly);
int TxBtDutyCycle(BOOL enable,			int PacketType, 
			 int payloadPattern,	int DutyWeight, BOOL payloadOnly);
int TxBtMultiCastFrames(int FrameCount,	int PayloadLength,
						int PacketType, 
			 int payloadPattern, BOOL payloadOnly);


int TxBtDutyCycleHop(BOOL enable,			int PacketType, 
			 int payloadPattern=DEF_PAYLOADPATTERN,	
			 int PayloadLenghtInByte=PAYLOADLENGTH_MAXIMUN_POSSIBLE, 
			 BOOL HopMode=0);
//int TxBtMultiCastFramesHop(int FrameCount,	int PayloadLength,
//						int PacketType, 
//						int payloadPattern, BOOL HopMode=1);

int TxBtRxTest(int FrameCount,	int PacketType, 
			 int payloadPattern=DEF_PAYLOADPATTERN,	
			 int PayloadLenghtInByte=PAYLOADLENGTH_MAXIMUN_POSSIBLE, 
			 BYTE TxBdAddress[SIZE_BDADDRESS_INBYTE]=NULL, int TxAmAddress=DEF_AMADDRESS, 
			 int FramSkip=0, 
			 int CorrWndw=DEF_CORRELATION_WINDOW, 
			 int CorrTh=DEF_CORRELATION_THRESHOLD, 
			 BOOL Whitening=0,BYTE PrbsPattern[LEN_PRBS_INBYTE]=NULL,
	// report
	BYTE*		Report_Status_Complete0_Abort1=NULL, // status
	DWORD*		Report_TotalPcktCount=NULL,
	DWORD*		Report_HecMatchCount=NULL,
	DWORD*		Report_HecMatchCrcPckts=NULL,
	DWORD*		Report_NoRxCount=NULL,
	DWORD*		Report_CrcErrCount=NULL,
	DWORD*		Report_SuccessfulCorrelation=NULL,
	DWORD*		Report_TotalByteCount=NULL,
	DWORD*		Report_ErrorBits=NULL,
	DWORD*		Report_PER=NULL,
	DWORD*		Report_BER=NULL
);



// not supported by this chip int TxBtCw(DWORD enable);
int TxBtCst(BOOL enable, int FreqOffsetinKHz=250);
*/
#ifndef _NO_CLASS_
};
#endif //#ifndef _NO_CLASS_
//#endif // !defined(AFX_NDISIF_H__90DC2111_AEB5_435E_B719_6A54BEB076A6__INCLUDED_)

 
#define HOST_BUFFER_COUNT	10

#ifndef _NO_CLASS_
class BtLinkTestBtApiClss : public DutBtIf_If  
{
#endif //#ifndef _NO_CLASS_
 
//#ifdef _LINUX_
//typedef unsigned long DWORD;
//typedef unsigned char BYTE;
//typedef unsigned short WORD;
//#define NULL 0
//#endif	//#ifdef _LINUX_


#ifndef _NO_CLASS_
public ://private:	// public: //
#endif //#ifndef _NO_CLASS_

DutBtApiClss	*pTxer;
DutBtApiClss	*pRxer;
DutBtApiClss	*pMaster;
DutBtApiClss	*pSlave;
GruBtApiClss	*pGru;


WORD	hConnection;
BYTE	LinkType;
BYTE	EncryptMode;
WORD	TxMaxBuf;
int		AvailableBufCnt; 
#ifdef _LINUX_
pthread_t	hTxThread;
pthread_t	hRxThread;
pthread_t	hTxBufThread;
#else //#ifdef _LINUX_
HANDLE	hTxThread;
HANDLE	hRxThread;
HANDLE	hTxBufThread;
#endif //#ifdef _LINUX_ 

int TestPktCount;
int TestPacketType;
int TestPacketLengthInByte;
int TestPayloadPattern;
bool TestStress;

int TestResult_PER;
int TestResult_BER;
//int TestCntBitErr;
//int TestCntBitTotal;
//int TestCntPacketErr;

int flag_Started;
int flag_ReadyToEnd;
int flag_TestDone;
public:
int TxPortDataTo;
int RxPortDataTo;

int flag_Abort;
int flag_UserAbort;
//private:
BYTE	DataBuffer[HOST_BUFFER_COUNT][2048];
BYTE	RxDataBuffer[HOST_BUFFER_COUNT][2048];


long RxPacketCount;
long RxByteCount;
long RxBitCount;
long RxErrorPacketCount;
long RxErrorByteCount;
long RxErrorBitCount;

//int InvalidPacketType(int PacketType);
void CountError(BYTE *pTxData, BYTE *pRxData, int DataLenInByte, 
				int* pErrBit, int* pTotalBit, int* pErrByte);

int DoTest(void);
#ifndef _NO_CLASS_
public:
#endif //#ifndef _NO_CLASS_

//int getPacketTypeSetting(int PacketType);
//int getMaxPacketLengthInByte(int PacketType);
//int Clip2MaxPacketLengthInByte(int PacketType, int *pPayloadLengthInByte);

void StopTest(BOOL bWaitForThread=1);

 

/*
getFlagTestDone();
getFlagTestAbort();
setFlagTestAbort();
getFlagTestReadyToEnd();
setFlagTestReadyToEnd();

getTestPktCount();
getTestPktType();
getTestHandleConn();
getTestPacketLengthInByte();
getTestPayloadPattern();
getDataBufPtr();
getDataBufSize();
*/

BtLinkTestBtApiClss();
~BtLinkTestBtApiClss();
int Version(void);
char* About(void);

int StartTest(void* pMasterObj, void* pSlaveObj, 
			  int PacketType, int PacketCount, 
			  int PayloadLengthInByte, int PayloadPattern, 
			  bool masterAsTx, bool DutAsMaster, 
			  bool stress);
int GetTestResult(bool *pTestDone, 
				int *pRxPacket,
				int *pMissingPacket,
				double * pPer, double *pBer);
 



//int StartAclConnection(DutBtApiClss *pDut, 
//											WORD	*hDutConnection, 
//											 DutBtApiClss *pGru,  
//											 WORD	*hGruConnection );

//friend:
 
 
#ifdef _LINUX_
friend void*  AclTxThread(void* lpvParam);
friend void*  AclRxThread(void* lpvParam);
friend void*  AclTxBufferThread(void* lpvParam);
#else
friend DWORD WINAPI AclTxThread(LPVOID lpvParam);
friend DWORD WINAPI AclRxThread(LPVOID lpvParam);
friend DWORD WINAPI AclTxBufferThread(LPVOID lpvParam);
#endif 

#ifndef _NO_CLASS_
};
#endif //#ifndef _NO_CLASS_
//#endif // !defined(AFX_NDISIF_H__90DC2111_AEB5_435E_B719_6A54BEB076A6__INCLUDED_)

#endif // _GRU_API_H_


#endif //#ifdef _HCI_PROTOCAL_
