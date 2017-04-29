/** @file DutBtIf_IfClss.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include "../../DutCommon/Clss_os.h"
#include "DutBtIf_IfClss.h"

#ifndef _LINUX_

#if defined (_BT_UART_)
  
DutBtIfClssHeader DutBtIf_If(void)
{ 
}
 
DutBtIfClssHeader ~DutBtIf_If(void)
{
}

DWORD DutBtIfClssHeader BtIf_GetErrorLast(void)
{
	return DutBtIf_HciUart::GetErrorLast();
}

int	DutBtIfClssHeader BtIf_GetDebugLogFlag(void)
{
	return 	 DutBtIf_HciUart::GetDebugLogFlag();
}

void	DutBtIfClssHeader BtIf_SetDebugLogFlag(BOOL Flag_Log)
{
	DutBtIf_HciUart::SetDebugLogFlag(Flag_Log);
	return;
}

BOOL DutBtIfClssHeader BtIf_query(ULONG OID, PUCHAR pInfo, UINT len) 
{
 	 
	return  DutBtIf_HciUart::query( OID,   pInfo,   len);
}

BOOL DutBtIfClssHeader BtIf_set(ULONG OID, PUCHAR pInfo, UINT len) 
{
	return DutBtIf_HciUart::query( OID,   pInfo,   len);
}
int DutBtIfClssHeader BtIf_OpenDevice(bool Gru)
{
	return 	 DutBtIf_HciUart::OpenDevice(); // TBD_KY
}
int	DutBtIfClssHeader BtIf_CloseDevice( )
{
	return 	 DutBtIf_HciUart::CloseDevice();
}
#elif defined _BT_USB_ 

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>

//#include "DutIf_IfClss.h"
#include "../../DutCommon/utilities.h"
 
//#include "DutIf_DvcIoCtrl.cpp"  

  
DutBtIfClssHeader DutBtIf_If(void)
{   
}
 
 
DutBtIfClssHeader ~DutBtIf_If(void)
{
}

DWORD DutBtIfClssHeader BtIf_GetErrorLast(void)
{
	return DutIf_DvcIoCtrl::GetErrorLast();
}

BOOL DutBtIfClssHeader BtIf_query(ULONG OID, PUCHAR pInfo, UINT len) 
{
 	 
	return  DutIf_DvcIoCtrl::query(OID, pInfo, len);
}

BOOL DutBtIfClssHeader BtIf_set(ULONG OID, PUCHAR pInfo, UINT len) 
{
	return DutIf_DvcIoCtrl::set( OID,   pInfo,   len);
}
int DutBtIfClssHeader BtIf_OpenDevice(bool Gru)
{
	return 	 DutIf_DvcIoCtrl::OpenDevice(Gru);
}
int	DutBtIfClssHeader BtIf_CloseDevice( )
{
	return 	 DutIf_DvcIoCtrl::CloseDevice();
}
 
#elif defined _BT_SDIO_ 

#include "../../DutCommon/utilities.h"

#ifndef _NO_CLASS_
DutBtIfClssHeader DutBtIf_If(void)
{   
}
 
 
DutBtIfClssHeader ~DutBtIf_If(void)
{
}
#endif // #ifndef _NO_CLASS_
DWORD DutBtIfClssHeader BtIf_GetErrorLast(void)
{
	return DutIf_SdioAdptHeader GetErrorLast();
}

BOOL DutBtIfClssHeader BtIf_query(ULONG OID, PUCHAR pInfo, UINT len) 
{
 	 
	return  DutIf_SdioAdptHeader query(OID, pInfo, len);
}

BOOL DutBtIfClssHeader BtIf_set(ULONG OID, PUCHAR pInfo, UINT len) 
{
	return DutIf_SdioAdptHeader set( OID,   pInfo,   len);
}
int DutBtIfClssHeader BtIf_OpenDevice(bool Gru)
{
	return 	 DutIf_SdioAdptHeader OpenDevice();
}
int	DutBtIfClssHeader BtIf_CloseDevice( )
{
	return 	 DutIf_SdioAdptHeader CloseDevice();
}
 

int	DutBtIfClssHeader BtIf_GetDebugLogFlag( )
{
	return 	 DutIf_SdioAdptHeader GetDebugLogFlag();
}
void	DutBtIfClssHeader BtIf_SetDebugLogFlag(BOOL Flag_Log)
{
	DutIf_SdioAdptHeader SetDebugLogFlag(Flag_Log);
	return;
}
#elif defined _BT_UDP_ 
#include "../../DutCommon/utilities.h"
 
#ifndef _NO_CLASS_
DutBtIfClssHeader DutBtIf_If(void)
{   
}
 
 
DutBtIfClssHeader ~DutBtIf_If(void)
{
}
#endif // #ifndef _NO_CLASS_
DWORD DutBtIfClssHeader BtIf_GetErrorLast(void)
{
	return DutIf_UDPAdptHeader GetErrorLast();
}

BOOL DutBtIfClssHeader BtIf_query(ULONG OID, PUCHAR pInfo, UINT len) 
{
	//;return 	 DutIf_UDPAdptHeader query
//	return 	 DutIf_UDPAdptHeader queryHci(OID, pInfo, len);
 	return 	 DutIf_UDPAdptHeader query(OID, pInfo, len);
}

BOOL DutBtIfClssHeader BtIf_set(ULONG OID, PUCHAR pInfo, UINT len) 
{
	SocketSendTo((char*)pInfo, len);
	 
	return  SocketRcvFrom((char*)pInfo, len, 500/1000);
}
int DutBtIfClssHeader BtIf_OpenDevice(bool Gru)
{
	return 	 DutIf_UDPAdptHeader SocketOpen();
}
int	DutBtIfClssHeader BtIf_CloseDevice( )
{
	return 	 DutIf_UDPAdptHeader SocketClose();
}
 
int	DutBtIfClssHeader BtIf_GetDebugLogFlag( )
{
	return 	 DebugLog;
}
void	DutBtIfClssHeader BtIf_SetDebugLogFlag(BOOL Flag_Log)
{
	DebugLog=Flag_Log;
	return;
}

#elif defined (_BT_REMOTE_UART_) 
//===========================================
#include "../../DutCommon/utilities.h"
 
#ifndef _NO_CLASS_
DutBtIfClssHeader DutBtIf_If(void)
{   
}
 
 
DutBtIfClssHeader ~DutBtIf_If(void)
{
}
#endif // #ifndef _NO_CLASS_
DWORD DutBtIfClssHeader BtIf_GetErrorLast(void)
{
//;	return DutIf_UDPAdptHeader GetErrorLast();
	 return 0;
}
 
BOOL DutBtIfClssHeader BtIf_query(ULONG OID, PUCHAR pInfo, UINT len) 
{
/*	int status =0;
	Bridge_Command_Format_Header HearderBuf = {0};
	char *TxBuf=NULL, *ptr=NULL;
	int TotalLength=0;
    TotalLength = len+ sizeof(Bridge_Command_Format_Header);
    TxBuf = (char *)malloc (len+ sizeof(Bridge_Command_Format_Header));
	memset(TxBuf, 0, TotalLength);
	HearderBuf.Type = 0x0003;
	HearderBuf.Length = len+ sizeof(Bridge_Command_Format_Header);
	memcpy(TxBuf, &HearderBuf, sizeof(Bridge_Command_Format_Header));
	memcpy(TxBuf+(sizeof(Bridge_Command_Format_Header)/sizeof(char)), pInfo, len);
	query(OID, (unsigned char*)TxBuf, TotalLength);
//;	status = SocketSendTo((char*) TxBuf, TotalLength);
//;	status = SocketRcvFrom((char*) TxBuf, TotalLength, 5000/1000);
	ptr = TxBuf+(sizeof(Bridge_Command_Format_Header)/sizeof(char));
	len =(TotalLength - sizeof(Bridge_Command_Format_Header));
	memcpy(pInfo, ptr, len);
	if(TxBuf)
		free(TxBuf); 
    return status;
	 return 0;
*/
 	return  Bt_query(OID, pInfo, len);

}

BOOL DutBtIfClssHeader BtIf_set(ULONG OID, PUCHAR pInfo, UINT len) 
{
//;	SocketSendTo((char*)pInfo, len);
	 query(OID, (unsigned char*)pInfo, len);
//;	return  SocketRcvFrom((char*)pInfo, len, 500/1000);
	return 0;
}
int DutBtIfClssHeader BtIf_OpenDevice(bool Gru)
{
	return 	 OpenDevice();

}
int	DutBtIfClssHeader BtIf_CloseDevice( )
{
	return 	 CloseDevice();

}
 

int	DutBtIfClssHeader BtIf_GetDebugLogFlag( )
{
	return 	 DebugLog;
}
void	DutBtIfClssHeader BtIf_SetDebugLogFlag(BOOL Flag_Log)
{
	DebugLog=Flag_Log;
	return;
}
//===========================================
#endif //#if defined (_BT_REMOTE_UART_)

#else //#ifndef _LINUX_

#include "../../drvwrapper/drv_wrapper.h"
/* Default HCI interface name */
#define DEFAULT_HCI_INTF "hci0"

#include "hci_opcodes.h"

static char HCI_PORT[256]=DEFAULT_HCI_INTF;

#define SIOCIWFIRSTPRIV 0x8be0
#define SIOCCFMFG SIOCIWFIRSTPRIV+2

static int err;
static struct ifreq userdata;
int BtIf_debugLog;

DWORD BtIf_GetErrorLast(void)
{
	return 1;//DutIf_Ndis::GetErrorLast();
}

BOOL BtIf_query(ULONG OID, PUCHAR pInfo, UINT len)
{
    BOOL status = BtIf_query1(OID, pInfo, len, NULL);
    
    return status;
}

BOOL BtIf_query1(ULONG OID, PUCHAR pInfo, UINT len, UINT *retBufSize) 
{
#define EVENT_BUF_SIZE 400
	UCHAR EventData[EVENT_BUF_SIZE]={0};
	USHORT CmdGrp=0, CmdCode=0;
	int RetBufSize=0;
	PUCHAR pInBuf=NULL;
	UINT InBufSize=0, i=0;
    int status=0, vg_UartSpy = 0;

    if(vg_UartSpy)
	{
		 printf("\nOID %08X\n", OID);
		 printf("Command\n");

		if(pInfo)
		{
		    for (i=0; i<len; i++)
			    printf("%02X ", *(pInfo+i));

		    printf("\n");
		 }
	}

	CmdGrp = (WORD)(OID>>16);
	CmdCode = (WORD)(OID&0x3FF);
	switch(CmdGrp)
	{
		case HCI_CMD_GROUP_INFORMATIONAL:
			InBufSize=4; 
			break;
		case HCI_CMD_GROUP_HC_BASEBAND:
		case HCI_CMD_GROUP_VENDOR_SPECIFIC:
		case HCI_CMD_GROUP_LINK_CONTROL:
		case HCI_CMD_GROUP_LINK_POLICY:
		case HCI_CMD_GROUP_TESTING:
			InBufSize=len + 4;
			break;
		default:
			return (-4);
			break;

	}

typedef struct 
{
	USHORT	CmdCode:10;
	USHORT	CmdGroup:6;
	UCHAR   ParamLength; // size of buffer Param
	UCHAR   Param[1];
} PACK HCI_COMMAND_STRUCT, *PHCI_COMMAND_STRUCT;

	PHCI_COMMAND_STRUCT pHciCmd=NULL;
	
	if (InBufSize)
    	pInBuf = (PUCHAR)malloc(InBufSize);
    	
	memset((PUCHAR)pInBuf, 0, InBufSize);
	
	pHciCmd = (PHCI_COMMAND_STRUCT)(pInBuf+1);

	*pInBuf=0x1;	//HCI_COMMAND_PACKET;
	
	pHciCmd->CmdCode=CmdCode;
	pHciCmd->CmdGroup=CmdGrp;// <<2;
	pHciCmd->ParamLength=(UCHAR)InBufSize-4;
	
	if (len>0)
	{
		memcpy(pHciCmd->Param, pInfo, len);
	}

	memset(EventData, 0, EVENT_BUF_SIZE);
    status = drv_wrapper_send_hci_command(CmdGrp, CmdCode, pInBuf, InBufSize, EventData, &RetBufSize);
 	if(pInBuf)
	{
		free(pInBuf);
		pInBuf=NULL;
	}

 	if(status)
	{
		return (status); 
	}

	if(vg_UartSpy)
	{
		 printf("\nEventData\n");
		 for (i=0; i<RetBufSize; i++)
			printf("%02X ", EventData[i]);
	
		 printf("\n");
	}

#define HCI_EVENT_HDR_SIZE 2
#define HCI_CMD_RESP_HDR_SIZE 4
#define HCI_HDR 7   //1+HCI_EVENT_HDR_SIZE+HCI_CMD_RESP_HDR_SIZE

	if(pInfo)
	{
		UINT hdr_size=0;
		if(retBufSize)
		{
			if (EventData[1]==0xff)
			{
				hdr_size=4;
			}
			else
				hdr_size=6;

			memset(pInfo, 0, RetBufSize-hdr_size);
			memcpy(pInfo, &EventData[hdr_size], RetBufSize-hdr_size);
		}
		else
		{
			memset(pInfo, 0, RetBufSize-HCI_HDR);	//len);
			memcpy(pInfo, &EventData[HCI_HDR], RetBufSize-HCI_HDR);
		}

    	if(vg_UartSpy)
	    {
		    printf("\nResponse\n");
    		for (i=0; (i+HCI_HDR) < RetBufSize; i++)
			    printf("%02X ", pInfo[i]);
		
		    printf("\n");
        }
    	if (retBufSize) *retBufSize = RetBufSize-HCI_HDR;
	}
	
	return 0;
}

BOOL BtIf_set(ULONG OID, PUCHAR pInfo, UINT len) 
{
	return BtIf_query(OID, pInfo, len);
}

int BtIf_OpenDevice(bool Gru)
{
    //setup.ini
	GetPrivateProfileString("DutInitSet", "HCI_PORT", DEFAULT_HCI_INTF, HCI_PORT, 255, "/etc/labtool/SetUp.ini");
	return drv_wrapper_init_hci(HCI_PORT);
}

int	BtIf_CloseDevice( )
{
	drv_wrapper_deinit_hci();
	return 	 0;
}


int	DutBtIfClssHeader BtIf_GetDebugLogFlag(void)
{
	return 	 BtIf_debugLog;
}

void	DutBtIfClssHeader BtIf_SetDebugLogFlag(BOOL Flag_Log)
{
	BtIf_debugLog = Flag_Log;
	return;
}

BOOL 	DutBtIfClssHeader queryHci(ULONG OID, PUCHAR Buf, UINT BufSize,
		UINT ExpectedEvent, UINT *BufRetSize)
{
	BOOL status=0;
	status =  BtIf_query1(OID, Buf, BufSize, BufRetSize);

	return status;
}

 


#endif //#ifndef _LINUX_
