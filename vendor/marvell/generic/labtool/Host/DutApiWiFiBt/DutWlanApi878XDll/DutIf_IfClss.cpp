/** @file DutIf_IfClss.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#ifndef _LINUX_
////#include <stdio.h>
//#include <stdlib.h>
//#include <tchar.h>
#include "../../DutCommon/Dut_error.hc"
//#include "../../DutCommon/utilities_os.h"
#include "../../DutCommon/utilities.h"
//#include "../DutShareApi878XDll/stdafx.h"
#include "DutIf_IfClss.h"
  
typedef struct Bridge_Command_Format_Header
{
	WORD    Type;
	WORD	SubType;
	WORD	Length;
	WORD	Status;
	DWORD	Reserved;
}Bridge_Command_Format_Header;


#if defined (_IF_UDP_)
#include "../DutIf/DutIf_UdpIpClss.h" 
  
DutIf_If::DutIf_If(void)
{ 
}
 
DutIf_If::~DutIf_If(void)
{
}

DWORD DutIf_If::WifiIf_GetErrorLast(void)
{
	return -1;
}

BOOL DutIf_If::WifiIf_query(ULONG OID, PUCHAR pInfo, UINT len) 
{
	Bridge_Command_Format_Header HearderBuf = {0};
	int *TxBuf=NULL, *ptr=NULL;
	int TotalLength=0;
    TotalLength = len+ sizeof(Bridge_Command_Format_Header);
    TxBuf = (int *)malloc (len+ sizeof(Bridge_Command_Format_Header));
	memset(TxBuf, 0, TotalLength);
	HearderBuf.Type = 0x0002;
	HearderBuf.Length = len+ sizeof(Bridge_Command_Format_Header);
	memcpy(TxBuf, &HearderBuf, sizeof(Bridge_Command_Format_Header));
	memcpy(TxBuf+(sizeof(Bridge_Command_Format_Header)/sizeof(int)), pInfo, len);
	SocketSendTo((char*) TxBuf, TotalLength);
//;	SocketSendTo((char*)pInfo, len);
//	SocketRcvFrom((char*) TxBuf, TotalLength, 5000/1000);
	SocketRcvFrom((char*) TxBuf, TotalLength, timeOut/1000.0);
	ptr = TxBuf+(sizeof(Bridge_Command_Format_Header)/sizeof(int));
	len =(TotalLength - sizeof(Bridge_Command_Format_Header));
	memcpy(pInfo, ptr, len);
	if(TxBuf)
		free(TxBuf); 
    return 0; 

	//; return DutIf_UdpIp::query(OID, pInfo, len);
}

BOOL DutIf_If::WifiIf_set(ULONG OID, PUCHAR pInfo, UINT len) 
{

	return DutIf_UdpIp::query(OID, pInfo, len);    
}
int DutIf_If::WifiIf_OpenDevice(bool Gru)
{
	return 	 DutIf_UdpIp::SocketOpen();
}
int	DutIf_If::WifiIf_CloseDevice( )
{
	return 	 DutIf_UdpIp::SocketClose();
}
ULONG	DutIf_If::WifiIf_GetDebugLogFlag(void)
{
	return 	 DutIf_UdpIp::GetDebugLogFlag();
}

#elif defined(_NDIS_) //NDIS
#include <windows.h>
//#include "DutIf_Ndis.h"  

DutIf_If::DutIf_If(void)
{   
}
 
 
DutIf_If::~DutIf_If(void)
{
}

DWORD DutIf_If::GetErrorLast(void)
{
	return DutIf_Ndis::GetErrorLast();
}

BOOL DutIf_If::query(ULONG OID, PUCHAR pInfo, UINT len) 
{
 	 
	return  DutIf_Ndis::query(OID, pInfo, len);
}

BOOL DutIf_If::set(ULONG OID, PUCHAR pInfo, UINT len) 
{
	return DutIf_Ndis::set( OID,   pInfo,   len);
}
int DutIf_If::OpenDevice( )
{
	return 	 DutIf_Ndis::OpenDevice();
}
int	DutIf_If::CloseDevice( )
{
	return 	 DutIf_Ndis::CloseDevice();
}

#elif defined (_IF_UART_)
#include "../DutIf/DutIf_UartComm.h" 
  
DutIf_If::DutIf_If(void)
{ 
}
 
DutIf_If::~DutIf_If(void)
{
}

DWORD DutIf_If::WifiIf_GetErrorLast(void)
{
	return -1;
}

BOOL DutIf_If::WifiIf_query(ULONG OID, PUCHAR pInfo, UINT len) 
{
		Bridge_Command_Format_Header HearderBuf = {0};
	int *TxBuf=NULL, *ptr=NULL;
	int TotalLength=0;
    TotalLength = len+ sizeof(Bridge_Command_Format_Header);
    TxBuf = (int *)malloc (len+ sizeof(Bridge_Command_Format_Header));
	memset(TxBuf, 0, TotalLength);
	HearderBuf.Type = 0x0002;
	HearderBuf.Length = len+ sizeof(Bridge_Command_Format_Header);
	memcpy(TxBuf, &HearderBuf, sizeof(Bridge_Command_Format_Header));
	memcpy(TxBuf+(sizeof(Bridge_Command_Format_Header)/sizeof(int)), pInfo, len);
	query(OID, (unsigned char*)TxBuf, TotalLength);
//;	SocketSendTo((char*) TxBuf, TotalLength);
//;	SocketSendTo((char*)pInfo, len);
//;	SocketRcvFrom((char*) TxBuf, TotalLength, 5000/1000);
	ptr = TxBuf+(sizeof(Bridge_Command_Format_Header)/sizeof(int));
	len =(TotalLength - sizeof(Bridge_Command_Format_Header));
	memcpy(pInfo, ptr, len);
	if(TxBuf)
		free(TxBuf); 
    return 0;

	//; return DutIf_UdpIp::query(OID, pInfo, len);
}

BOOL DutIf_If::WifiIf_set(ULONG OID, PUCHAR pInfo, UINT len) 
{
	return DutIf_UartComm::set(OID, pInfo, len);
}
int DutIf_If::WifiIf_OpenDevice(bool Gru)
{
	return 	 DutIf_UartComm::OpenDevice();
}
int	DutIf_If::WifiIf_CloseDevice( )
{
	return 	 DutIf_UartComm::CloseDevice();
}
ULONG	DutIf_If::WifiIf_GetDebugLogFlag(void)
{
	return 	 DutIf_UartComm::GetDebugLogFlag();
}



#elif defined _IF_USB_ 

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>

#include "DutIf_IfClss.h"
#include "../../DutCommon/utilities.h"
 
//#include "DutIf_DvcIoCtrl.cpp"  

  
DutIf_If::DutIf_If(void)
{   
}
 
 
DutIf_If::~DutIf_If(void)
{
}

DWORD DutIf_If::GetErrorLast(void)
{
	return DutIf_DvcIoCtrl::GetErrorLast();
}

BOOL DutIf_If::query(ULONG OID, PUCHAR pInfo, UINT len) 
{
 	 
	return  DutIf_DvcIoCtrl::query(OID, pInfo, len);
}

BOOL DutIf_If::set(ULONG OID, PUCHAR pInfo, UINT len) 
{
	return DutIf_DvcIoCtrl::set( OID,   pInfo,   len);
}
int DutIf_If::OpenDevice(bool Gru)
{
	return 	 DutIf_DvcIoCtrl::OpenDevice(Gru);
}
int	DutIf_If::CloseDevice( )
{
	return 	 DutIf_DvcIoCtrl::CloseDevice();
}

#elif defined(_IF_SDIO_)
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>
//#include "DutIf_SdioAdapter.cpp"  
#include "../DutIf/DutIf_SdioAdapter.h"  

DutIf_If::DutIf_If(void)
{ 
}
 
DutIf_If::~DutIf_If(void)
{
}

DWORD DutIf_If::WifiIf_GetErrorLast(void)
{
return 0;
//	return  GetLastError();
}

BOOL DutIf_If::WifiIf_query(ULONG OID, PUCHAR pInfo, UINT len) 
{
 	 
	return  DutIf_SdioAdpt::query(OID, pInfo, len);
}

BOOL DutIf_If::WifiIf_set(ULONG OID, PUCHAR pInfo, UINT len) 
{
	return DutIf_SdioAdpt::set( OID,   pInfo,   len);
}
int DutIf_If::WifiIf_OpenDevice(bool Gru )
{
	return 	 DutIf_SdioAdpt::OpenDevice();
}
int	DutIf_If::WifiIf_CloseDevice( )
{
	return 	 DutIf_SdioAdpt::CloseDevice();
}

ULONG	DutIf_If::WifiIf_GetDebugLogFlag(void)
{
	return 	 DutIf_SdioAdpt::GetDebugLogFlag();
}

void DutIf_If::WifiIf_SetDebugLogFlag(BOOL Flag_Log)
{
	DutIf_SdioAdpt::SetDebugLogFlag(Flag_Log);
	return;
}
#endif 
#else	// _LINUX_
#include "../../DutCommon/Clss_os.h"
#include "../../DutCommon/utilities_os.h"
#include "../DutIf/ioctl.h"
#include "../../drvwrapper/drv_wrapper.h"
// Default interface name 
#define DEFAULT_INTF "mlan0"

int debugLog=0;
static char ETH_PORT[256] = DEFAULT_INTF;

#define SIOCIWFIRSTPRIV 0x8be0
#define SIOCCFMFG SIOCIWFIRSTPRIV+4

static int err;
static struct ifreq userdata;
// OID to Mdriver IOCTL map table for WLAN
#ifndef _LINUX_
static OID_IOCTL WlanOidIoctlTable[] = {
	{OID_MRVL_MFG_COMMAND, SIOCCFMFG},
};
#endif	//#ifndef _LINUX_

DWORD WifiIf_GetErrorLast(void)
{
	return 1;//DutIf_Ndis::GetErrorLast();
}

BOOL WifiIf_query(ULONG OID, PUCHAR pInfo, UINT len) 
{
	int sockfd;
	int status = -1, ioctl_val;
	int dummy;
 
	status = drv_wrapper_ioctl(ETH_PORT,pInfo,&dummy);

	return  status;//DutIf_Ndis::query(OID, pInfo, len);
}

BOOL WifiIf_set(ULONG OID, PUCHAR pInfo, UINT len) 
{
	return WifiIf_query(OID, pInfo, len);
}

int WifiIf_OpenDevice(bool Gru)
{
    //setup.ini
    printf("WifiIf_OpenDevice\n");
	GetPrivateProfileString("DutInitSet", "ETH_PORT", DEFAULT_INTF, ETH_PORT, 255, "/etc/labtool/SetUp.ini");
	return drv_wrapper_init(ETH_PORT);
}

int	WifiIf_CloseDevice( )
{
    printf("WifiIf_CloseDevice\n");
	drv_wrapper_deinit();
	return 	 0;
}

ULONG	WifiIf_GetDebugLogFlag(void)
{
	return 	 debugLog;
}

void WifiIf_SetDebugLogFlag(BOOL Flag_Log)
{
	debugLog = Flag_Log;
	return;
}

BOOL query(ULONG OID, PUCHAR pInfo, UINT len) 
{
	return WifiIf_query(OID, pInfo, len);
}
DWORD GetErrorLast(void)
{
	return 1;//DutIf_Ndis::GetErrorLast();
}

#endif
