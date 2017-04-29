/** @file DutIf_DvcIoCtrl.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include "../DutShareApi878XDll/stdafx.h" 
#ifndef _UDP_ 
#ifndef _NDIS_CLSS_CPP_
#define _NDIS_CLSS_CPP_
 
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <direct.h>
#include <string.h>

#include "dsdef.h"

#include "../../DutCommon/Clss_os.h" 
#include "../../DutCommon/utilities.h" 
//#include "../../windis5/include/WINDIS.h"
 
//#include "_ntddndis.h"   // from DDK; modified not to conflict with WINDIS.h
#include DUTIF_WLAN_H_PATH		//"../DutWlanApi878XDll/DutIf_IfClss.h" 
#include DUTDLL_WLAN_H_PATH		//"../DutWlanApi878XDll.h" 
#include "ioctl.h"
#include "DutIf_DvcIoCtrl.h"
#include "MrvlMfg.h" 


 
#define IOCTL_MRVL_QUERY_OID_VALUE \
	CTL_CODE(FILE_DEVICE_NETWORK, 0x100, METHOD_BUFFERED,FILE_ANY_ACCESS)

typedef struct _MRVL_DATA_BUFFER
{
	HANDLE Event;
	ULONG	BufferSize;
	PUCHAR	Buffer;
}MRVL_DATA_BUFFER;
//static OVERLAPPED g_overlapped;
MRVL_DATA_BUFFER Event;

typedef ULONG  NDIS_OID;
// prefix of adapter description
//static char adapter_prefix[256] = "Marvell CB55 ";
  
DutIf_DvcIoCtrl::DutIf_DvcIoCtrl(void)
{
 	char temp[_MAX_PATH]="";
  
 	if(getcwd(temp, _MAX_PATH) != NULL)  
 	strcat(temp, "\\setup.ini");
 	else
	strcpy(temp, "setup.ini");

	hPort=NULL;
 		
//  	GetPrivateProfileString("Driver","AdapterPrefix","Marvell",adapter_prefix, sizeof(adapter_prefix), temp); 
	debugLog	= GetPrivateProfileInt("DEBUG","debugLog", 
		0, temp); 

	vg_maxWait	= GetPrivateProfileInt("DutInitSet","maxWait",100, temp); 

	vg_dwSleep = GetPrivateProfileInt("DutInitSet","Delay",1, temp); 
    
 	vg_retry_temp = GetPrivateProfileInt("DutInitSet","retry",0, temp); 
  
  
	vg_dwSpiSleep	= GetPrivateProfileInt("DutInitSet","SpiDelay", 0, temp); 

 
	vg_IfSpy = GetPrivateProfileInt("DEBUG","IfSpy", 0, temp); 


	if(debugLog)
	{
		DebugLogRite( "debugLog %d\n", debugLog);
 		DebugLogRite("%s\n", temp);
//		DebugLogRite( "adapter_prefix %s\n", adapter_prefix);
  		DebugLogRite( "Delay %X\n", vg_dwSleep);
		DebugLogRite( "maxWait %d\n", vg_maxWait);
		DebugLogRite( "Retry %X\n", vg_retry_temp);
		DebugLogRite( "SpiDelay %X\n", vg_dwSpiSleep);
		DebugLogRite( "IfSpy %d\n", vg_IfSpy);
	}
}

DutIf_DvcIoCtrl::~DutIf_DvcIoCtrl(void)
{
}

DWORD DutIf_DvcIoCtrl::GetErrorLast(void)
{
	return 0;//W32N_GetLastError();
}


#ifdef _UART_
void DutIf_DvcIoCtrl:: SetFwDlParm(int UartFwDl,	char *pFwImageName,
										 int BootBaudRate,
					int Boot_RtsFlowControl,
					int Boot_Signal, int BootSignalWait
					)
{
	vg_UartFwDl			= UartFwDl;
	if(pFwImageName) strcpy(vg_FwImageName, pFwImageName);
	vg_BootBaudRate		= BootBaudRate;
	vg_BootRtsFlowCtrl	= Boot_RtsFlowControl;

	vg_BootSignal		= Boot_Signal;
	vg_BootSignalWait	= BootSignalWait;
}
void DutIf_DvcIoCtrl:: GetFwDlParm(	
				int *pUartFwDl,	char *pFwImageName,
				int *pBootBaudRate,
					int *pBoot_RtsFlowControl,
					int *pBoot_Signal, int *pBootSignalWait
				)
{
	if(pUartFwDl) 
		(*pUartFwDl) = vg_UartFwDl;
	if(pFwImageName) 
		strcpy(pFwImageName, vg_FwImageName);
	if(pBootBaudRate) 
		(*pBootBaudRate) = vg_BootBaudRate;
	if(pBoot_RtsFlowControl)	
		(*pBoot_RtsFlowControl) = vg_BootRtsFlowCtrl;

	if(pBoot_Signal)			
		(*pBoot_Signal)		= vg_BootSignal;
	if(pBootSignalWait)			
		(*pBootSignalWait)	= vg_BootSignalWait;
}
#endif // #ifdef _UART_

BOOL DutIf_DvcIoCtrl::query( ULONG OID, PUCHAR pInfo, UINT len)
{	
	int status=0; 

	if (vg_IfSpy)
	{
		int index =0;
		DebugLogRite("Resps:OID %08X\n", OID);

		for (index =0; index < (int)len; index ++)
			DebugLogRite("%02X\t", *(pInfo+index));
	
		DebugLogRite("\n");
	}

	status = MrvlQueryInfo(hPort, OID, pInfo, len);

	if (vg_IfSpy)
	{
		int index =0;
		DebugLogRite("Resps:OID %08X\n", OID);

		for (index =0; index < (int)len; index ++)
			DebugLogRite("%02X\t", *(pInfo+index));
	
		DebugLogRite("\n");
	}
	 
	 return status;
}
 
BOOL DutIf_DvcIoCtrl::set( ULONG OID, PUCHAR pInfo, UINT len)
{ 
	return query(OID, pInfo, len);
}
 
BOOL DutIf_DvcIoCtrl::queryHci(ULONG OID, PUCHAR Buf, UINT BufSize, 
						UINT ExpectedEvent,	  
						UINT *BufRetSize)
{
#ifdef _HCI_PROTOCAL_

#define BUFSIZE	1024
#define EVENT_BUF_SIZE 400
	UCHAR EventData[EVENT_BUF_SIZE]={0}, Buffer[BUFSIZE]={0};
 	USHORT RetBufSize;
	USHORT	CmdGrp=0;
	USHORT	CmdCode=0;
	PUCHAR pInBuf=NULL;
	UINT InBufSize=0;
	int i=0, status=0;
	UINT	expectEvent=HCI_EVENT_CMD_COMPLETE;
	UINT	expectedReturnBuf=EVENT_BUF_SIZE;
	
     if(vg_IfSpy)
	 {
		 DebugLogRite("\nOID %08X\n", OID);
		 DebugLogRite("Command\n");

		 for (i=0; i<BufSize; i++)
			DebugLogRite("%02X ", *(Buf+i));

		 DebugLogRite("\n");
	 }
	if(ExpectedEvent) 
		expectEvent = (0xFFFF & ExpectedEvent);
	else
		expectEvent = HCI_EVENT_CMD_COMPLETE;

	if(BufRetSize) 
		expectedReturnBuf = *BufRetSize;	
	else 
		expectedReturnBuf = BufSize;	


	CmdGrp = (OID>>16);
	CmdCode = (OID&0x3FF);
	 switch(CmdGrp)
	 {
		case HCI_CMD_GROUP_INFORMATIONAL:
			pInBuf=NULL; // for GetBDAddress
			InBufSize=0; 
			break;
		case HCI_CMD_GROUP_HC_BASEBAND:
		case HCI_CMD_GROUP_VENDOR_SPECIFIC:
		case HCI_CMD_GROUP_LINK_CONTROL:
		case HCI_CMD_GROUP_LINK_POLICY:
			pInBuf=Buf;
			InBufSize=BufSize;
			break;
		case 0:
			break;
		default:
			return (-4);
			break;

	 }
	memset(EventData, 0, EVENT_BUF_SIZE);

	Sleep(vg_dwSleep);

 

	status = SendHciCommand(hPort,CmdGrp,  //
					CmdCode,
					expectEvent,
					pInBuf,
					InBufSize,
					EventData,
					(RetBufSize = EVENT_BUF_SIZE));
 	if(status)
	{
		return (status); 
	}

	if (EventData[0]!=ERROR_SUCCESS)
	{
		 
		return EventData[0];
	}
	else
	{
		memset(Buf, 0, (BufSize>expectedReturnBuf)?BufSize:expectedReturnBuf);
 
		if(RetBufSize <= expectedReturnBuf) 
		{
			memcpy(Buf, &EventData[0], RetBufSize);

			if(vg_IfSpy)
			{
				 DebugLogRite("\nResponse\n");
				 for (i=0; i<RetBufSize; i++)
					DebugLogRite("%02X ", *(Buf+i));
			
				 DebugLogRite("\n");
			}
			return EventData[0];
		}
		else
		{

			memcpy(Buf, &EventData[1], expectedReturnBuf-1);

			if(vg_IfSpy)
			{
				 DebugLogRite("\nResponse\n");
				 for (i=0; i<expectedReturnBuf-1; i++)
					DebugLogRite("%02X ", *(Buf+i));
			
				 DebugLogRite("\n");
			}
			return EventData[0];
		}

	}
#endif //#ifdef _HCI_PROTOCAL_
	return 0;
}

int DutIf_DvcIoCtrl::OpenDevice(bool Gru)
{
	if(hPort!=NULL)
	{
		DebugLogRite("Open USB ioctl fail,Can not open twice \n");
		return -1;
	}
	int ret = Mrv8000xOpenAdapter(hPort, Gru); 
//	DebugLogRite(" Open USB BT/WIFI Mrv8000xOpenAdapter %X\n", hPort);
  	return ret;
}

// Close The Adapter
int  DutIf_DvcIoCtrl::CloseDevice( )
{
	int ret= 0;
	if(hPort)
	{
		ret = Mrv8000xCloseAdapter(hPort);
		hPort=NULL;
	}
// 	DebugLogRite(" Close USB BT/WIFI Mrv8000xCloseAdapter %X\n", hPort);
	return ret;
}


DWORD DutIf_DvcIoCtrl::GetMaxTimeOut(void)
{
	return GetMaxtimeOut();
}

void DutIf_DvcIoCtrl::SetMaxTimeOut(DWORD TimeOut) 
{
	SetMaxtimeOut(TimeOut); 
}

ULONG DutIf_DvcIoCtrl::GetDebugLogFlag()
{
	return debugLog;
}
void DutIf_DvcIoCtrl:: SetDebugLogFlag(BOOL Flag_Log)
{
	debugLog = Flag_Log;
}

void DutIf_DvcIoCtrl:: GetTimeOut(int *pTimeOut)
{
		
	//*pTimeOut=GetTimeoutTimer(hPort);
}
void DutIf_DvcIoCtrl:: SetTimeOut(int TimeOut)
{
//	vg_maxWait = TimeOut;
//	SetTimeoutTimer(hPort, TimeOut);
}

void DutIf_DvcIoCtrl:: ResetTimeOut(void)
{
//	vg_maxWait = TimeOut;
//	SetTimeoutTimer(hPort, vg_maxWait);
}

void DutIf_DvcIoCtrl:: GetDataTimeOut(int *pTimeOut)
{
		
//	*pTimeOut=GetPortTimeoutTimer(hPort);
}

void DutIf_DvcIoCtrl:: SetDataTimeOut(int TimeOut)
{
//	vg_maxWait = TimeOut;
//	SetPortTimeoutTimer(hPort, TimeOut);
}

void DutIf_DvcIoCtrl:: ResetDataTimeOut(void)
{
//	vg_maxWait = TimeOut;
//	SetPortTimeoutTimer(hPort, vg_DataMaxWait);
}
BOOL  DutIf_DvcIoCtrl:: SendAclData(PUCHAR pData, ULONG DataLength)
{
	return 1;//SendHciData(hPort, true, pData, DataLength);
}

BOOL  DutIf_DvcIoCtrl:: RxData(PUCHAR pData, 
							   ULONG BufLength)
{
	return 1;//RxHciDataEvent(hPort, pData, BufLength);
}

#ifdef _UART_
void DutIf_DvcIoCtrl:: SetUartParm(	int BaudRate,
					int RtsFlowControl,
					int OverLapped
					)
{
	vg_BaudRate = BaudRate;
	vg_RtsFlowControl = RtsFlowControl;
	vg_OverLapped = OverLapped;
}

void DutIf_DvcIoCtrl:: SetUartPortName(char* UartPortName)
{
	strcpy(PortName, UartPortName);
}
#endif // #ifdef _UART_

#endif


#endif
