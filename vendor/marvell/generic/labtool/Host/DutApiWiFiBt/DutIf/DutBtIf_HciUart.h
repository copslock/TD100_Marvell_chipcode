/** @file DutBtIf_HciUart.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#pragma pack(1) 
 
 


class DutBtIf_HciUart 
{
 HANDLE hPort; //h;

 int vg_IfSpy;
 int vg_dwSleep;
 int vg_maxWait;
 char PortName[255]; //="COM5";
// int vg_retry_temp;

	int vg_BaudRate;
	int vg_RtsFlowControl;
//	int vg_OverLapped;
	bool vg_OverLapped;

//	int timeOut;
	int vg_DataMaxWait;
	int vg_SpiPageSizeInByte;
	int vg_dwSpiSleep;
 
	int vg_UartFwDl;
	char vg_FwImageName[255];
	int vg_BootBaudRate;
	int vg_BootRtsFlowCtrl;
	unsigned char vg_BootSignal[5];
	int vg_BootSignalNum;
	int vg_BootSignalWait;

	int vg_HelperRtsFlowCtrl;
	int vg_HelperBaudRate;
//protected:
	int debugLog; 
public: 
 
DutBtIf_HciUart();
~DutBtIf_HciUart();
 
DWORD	GetErrorLast(void);  

BOOL query( ULONG OID, PUCHAR pInfo, UINT len);
//BOOL set( ULONG OID, PUCHAR pInfo, UINT len);
BOOL queryHci(ULONG OID, PUCHAR Buf, UINT BufSize, 
							 UINT ExpectedEvent=0, 
							 UINT *BufRetSize=NULL
							 );
int	UartFwDl();

int	OpenDevice( );
int	OpenDevice(char *FileName, long BootSingalWait=5000, char BootSingal=0x15);
int	CloseDevice( );

//int GetMAC_Address( HANDLE h, UCHAR* MACaddr);
ULONG GetUartSpyFlag();
void SetUartSpyFlag(BOOL Flag_Spy);

ULONG GetDebugLogFlag();
void SetDebugLogFlag(BOOL Flag_Log);


char *GetUartPortName(void);
void SetUartPortName(char* UartPortName);

void SetUartParm(	int BaudRate,
					int RtsFlowControl,
					int OverLapped
					);
void GetUartParm(	
				int *pBaudRate,
				int	*pRtsFlowControl,
				int	*pOverLapped
				);
void SetFwDlParm(int UartFwDl,	char *pFwImageName,
										 int BootBaudRate,
					int Boot_RtsFlowControl,
					int *pBoot_Signal, int BootSignalWait, 
					int HelperBaudRate,  int HelperRtsFlowControl
					);
void GetFwDlParm(	
				int *pUartFwDl,	char *pFwImageName,
				int *pBootBaudRate,
					int *pBoot_RtsFlowControl,
					int *pBoot_Signal, int *pBootSignalWait, 
					int *pHelperBaudRate,  int *pHelperRtsFlowControl
				);


void GetTimeOut(int *pTimeOut);
void SetTimeOut(int TimeOut);
void ResetTimeOut(void);
void GetDataTimeOut(int *pTimeOut);
void SetDataTimeOut(int TimeOut);
void ResetDataTimeOut(void);


BOOL SendAclData(PUCHAR pData, ULONG DataLength);
BOOL RxData(PUCHAR pData,	   ULONG BufLength);

}; 
