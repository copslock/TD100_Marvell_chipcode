/** @file DutIf_DvcIoCtrl.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#ifndef _DUTIF_DVCIOCTRL_CLSS_
#define _DUTIF_DVCIOCTRL_CLSS_
#include "DutNdis.h"
//#include "winbase.h"
//#include "dsdef.h"

#pragma pack(1) 
class DutIf_DvcIoCtrl 
{

//HANDLE g_usb_handle; 
// HANDLE hPort; //h;
//char adapter_prefix[256];
#ifdef _UART_
  
 //int vg_UartSpy;
 char PortName[255]; //="COM5";

	int vg_BaudRate;
	int vg_RtsFlowControl;
	int vg_OverLapped;

//	int timeOut;
	int vg_DataMaxWait;
	int vg_SpiPageSizeInByte;
 
	int vg_UartFwDl;
	char vg_FwImageName[255];
	int vg_BootBaudRate;
	int vg_BootRtsFlowCtrl;
	int vg_BootSignal;
	int vg_BootSignalWait;

#endif // #ifdef _UART_

 int vg_dwSpiSleep;

 int vg_IfSpy;
 int vg_dwSleep;
 int vg_maxWait;
 int vg_retry_temp;
 

//HANDLE  open_adapter(dev_t *dev);
//void  init_dev_list(dev_t *dev);
 HANDLE hPort;
	int timeOut;

protected:
//	int debugLog; 
	
	DWORD GetMaxTimeOut(void);
	void SetMaxTimeOut(DWORD TimeOut);

public: 
 
DutIf_DvcIoCtrl();
~DutIf_DvcIoCtrl();
 
DWORD	GetErrorLast(void);  

BOOL query( ULONG OID, PUCHAR pInfo, UINT len);
BOOL set( ULONG OID, PUCHAR pInfo, UINT len);
BOOL queryHci(ULONG OID, PUCHAR Buf, UINT BufSize, 
							 UINT ExpectedEvent=0, 
							 UINT *BufRetSize=NULL
							 );

int	OpenDevice(bool Gru);
int	CloseDevice( );

public:
ULONG GetDebugLogFlag();
void SetDebugLogFlag(BOOL Flag_Log);
ULONG GetUartSpyFlag();
void SetUartSpyFlag(BOOL Flag_Spy);


#ifdef _UART_

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
					int Boot_Signal, int BootSignalWait
					);
void GetFwDlParm(	
				int *pUartFwDl,	char *pFwImageName,
				int *pBootBaudRate,
					int *pBoot_RtsFlowControl,
					int *pBoot_Signal, int *pBootSignalWait
				);
#endif // #ifdef _UART_


void GetTimeOut(int *pTimeOut);
void SetTimeOut(int TimeOut);
void ResetTimeOut(void);
void GetDataTimeOut(int *pTimeOut);
void SetDataTimeOut(int TimeOut);
void ResetDataTimeOut(void);


BOOL SendAclData(PUCHAR pData, ULONG DataLength);
BOOL RxData(PUCHAR pData,	   ULONG BufLength);

};

#endif




 
