/** @file DutBtIf_HciUart.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include "../../DutCommon/Clss_os.h"
#include "../../DutCommon/utilities_os.h"
#include "../../DutCommon/utilities.h" 

#include "DutBtIf_HciUart.h"
#include DUTHCI_BT_LL_CTRL_H_PATH		// "HciCtrl.h"
#include DUTHCI_BT_LL_STRUCT_H_PATH		// "HciStruct.h"
#include DUTHCI_BT_LL_OPCODE_H_PATH		// "hci_opcodes.h"
#include DUTHCI_BT_LL_UARTCHECK_H_PATH //"../UartPortCheck.h"		// "xmodem.h"
 

DutBtLowLevelClssHeader DutBtIf_HciUart(void)
{
 	char temp[_MAX_PATH]="";
	char line[30]="";
   
 	if(getcwd(temp, _MAX_PATH) != NULL)  
 	strcat(temp, "/setup.ini");
 	else
	strcpy(temp, "setup.ini");

 	hPort =0;
	vg_BootSignalNum=5;

	debugLog	= GetPrivateProfileInt("DEBUG","debugLog", 0, temp); 

	vg_maxWait	= GetPrivateProfileInt("DutInitSet","maxWait", GetTimeoutTimer(hPort), temp); 
 	vg_dwSleep = GetPrivateProfileInt("DutInitSet","Delay",1, temp); 
    
// 	vg_retry_temp = GetPrivateProfileInt("DutInitSet","retry",0, temp); 
  
  	GetPrivateProfileString("Driver","UartPort","COM1",PortName, sizeof(PortName), temp); 
//  	GetPrivateProfileString("Driver","UartPort","test",PortName, sizeof(PortName), temp); 
 
 	vg_BaudRate	= GetPrivateProfileInt("Driver","UartBaudRate", CBR_115200, temp); 
	vg_RtsFlowControl	= GetPrivateProfileInt("Driver","UartRtsFlowControl", RTS_CONTROL_HANDSHAKE, temp); 
    vg_OverLapped 	= GetPrivateProfileInt("Driver","UartOverLapped", false, temp)?true:false; 

	vg_dwSpiSleep	= GetPrivateProfileInt("DutInitSet","SpiDelay", 0, temp); 
 	debugLog = GetPrivateProfileInt("DEBUG","debugLog", 0, temp); 

	vg_IfSpy = GetPrivateProfileInt("DEBUG","IfSpy", 0, temp); 
 	vg_SpiPageSizeInByte =0x100;
//	vg_SpiPageSizeInByte = GetPrivateProfileInt("DutInitSet", "SpiPageSizeInByte", 
//							MAXSPIPAGE_BYTE, temp);

	SetTimeoutTimer(hPort, vg_maxWait);
	vg_DataMaxWait = GetPortTimeoutTimer(hPort);

	vg_UartFwDl			= GetPrivateProfileInt("Driver","UartFwDl", 0, temp);
  	GetPrivateProfileString("Driver","FwImageName","fwimage.bin",vg_FwImageName, sizeof(vg_FwImageName), temp); 
 
	vg_BootBaudRate		= GetPrivateProfileInt("Driver","BootBaudRate", CBR_115200, temp); 
	vg_BootRtsFlowCtrl	= GetPrivateProfileInt("Driver","BootRtsFlowCtrl", RTS_CONTROL_DISABLE, temp); 
//	vg_BootSignal		= GetPrivateProfileInt("Driver","BootSignal", 0x1C, temp); 

	GetPrivateProfileString("Driver", "BootSignal","0xA5 0x10 0x00 0xFE 0xFF",line, sizeof(line), temp); 
	vg_BootSignalNum = sscanf(line, "%x %x %x %x %x", 
		&vg_BootSignal[0], &vg_BootSignal[1], &vg_BootSignal[2], 
		&vg_BootSignal[3], &vg_BootSignal[4]);
 	vg_BootSignalWait	= GetPrivateProfileInt("Driver","BootSignalWait", 5000, temp); 

	vg_HelperBaudRate		= GetPrivateProfileInt("Driver","HelperBaudRate", CBR_3000000, temp); 
	vg_HelperRtsFlowCtrl	= GetPrivateProfileInt("Driver","HelperRtsFlowCtrl", RTS_CONTROL_DISABLE, temp); 

 	//if(debugLog)
	{
// 		DebugLogRite("%s\n", temp);
//		DebugLogRite( "debugLog %d\n", debugLog);
 		DebugLogRite( "PortName %s\n", PortName);
 		DebugLogRite( "BaudRate %d\n", vg_BaudRate);
 		DebugLogRite( "RtsFlowControl %d\n", vg_RtsFlowControl);
  		DebugLogRite( "OverLapped %d\n", vg_OverLapped);
 		DebugLogRite( "Delay %X\n", vg_dwSleep);
		DebugLogRite( "maxWait %d\n", vg_maxWait);
//		DebugLogRite( "SpiPageSizeInByte %X\n", vg_SpiPageSizeInByte);
//		DebugLogRite( "SpiDelay %X\n", vg_dwSpiSleep);
		DebugLogRite( "UartSpy %d\n", vg_IfSpy);


		if(vg_UartFwDl)
		{
			DebugLogRite( "FwImageName %s\n",			vg_FwImageName);
			DebugLogRite( "Boot_BaudRate %d\n",			vg_BootBaudRate);
 			DebugLogRite( "Boot_RtsFlowControl %d\n",	vg_BootRtsFlowCtrl);
 			DebugLogRite( "HelperBaudRate %d\n",		vg_HelperBaudRate);
 			DebugLogRite( "HelperRtsFlowCtrl %d\n",		vg_HelperRtsFlowCtrl);

 			DebugLogRite( "Boot_Signal %X %X %X %X %X (%d)\n",		
				vg_BootSignal[0], vg_BootSignal[1], vg_BootSignal[2], 
				vg_BootSignal[3], vg_BootSignal[4], vg_BootSignalNum);
 			DebugLogRite( "Boot_SignalWait %d\n",	vg_BootSignalWait);
		}
	}
}

DutBtLowLevelClssHeader ~DutBtIf_HciUart(void)
{
}

char* DutBtLowLevelClssHeader GetUartPortName()
{
	return PortName;
}


void DutBtLowLevelClssHeader SetUartPortName(char* UartPortName)
{
	strcpy(PortName, UartPortName);
}
void DutBtLowLevelClssHeader SetUartParm(	int BaudRate,
					int RtsFlowControl,
					int OverLapped
					)
{
	vg_BaudRate = BaudRate;
	vg_RtsFlowControl = RtsFlowControl;
    vg_OverLapped = OverLapped?true:false;
}
void DutBtLowLevelClssHeader GetUartParm(	
				int *pBaudRate,
				int	*pRtsFlowControl,
				int	*pOverLapped
				)
{
	if(pBaudRate)		*pBaudRate		= vg_BaudRate;
	if(pRtsFlowControl)	*pRtsFlowControl= vg_RtsFlowControl;
	if(pOverLapped)		*pOverLapped	= vg_OverLapped;
}
/*
void DutBtLowLevelClssHeader SetFwDlParm(int UartFwDl,	char *pFwImageName,
										 int BootBaudRate,
					int BootRtsFlowControl,
					int *BootSignal, int BootSignalWait, 
					int HelperBaudRate,  int HelperRtsFlowControl
					)
{
	vg_UartFwDl			= UartFwDl;
	if(pFwImageName) strcpy(vg_FwImageName, pFwImageName);
	vg_BootBaudRate		= BootBaudRate;
	vg_BootRtsFlowCtrl	= BootRtsFlowControl;
	vg_HelperBaudRate		= HelperBaudRate;
	vg_HelperRtsFlowCtrl	= HelperRtsFlowControl;

	vg_BootSignal[0]		= BootSignal[0];
	vg_BootSignal[1]		= BootSignal[1];
	vg_BootSignal[2]		= BootSignal[2];
	vg_BootSignal[3]		= BootSignal[3];
	vg_BootSignal[4]		= BootSignal[4];
	vg_BootSignalWait	= BootSignalWait;
}
void DutBtLowLevelClssHeader GetFwDlParm(	
				int *pUartFwDl,	char *pFwImageName,
				int *pBootBaudRate,
					int *pBoot_RtsFlowControl,
					int *pBoot_Signal, int *pBootSignalWait, 
					int *pHelperBaudRate,  int *pHelperRtsFlowControl
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

	if(pHelperBaudRate) 
		(*pHelperBaudRate) = vg_HelperBaudRate;
	if(pHelperRtsFlowControl)	
		(*pHelperRtsFlowControl) = vg_HelperRtsFlowCtrl;

	if(pBoot_Signal)
	{
		(pBoot_Signal[0])		= vg_BootSignal[0];
		(pBoot_Signal[1])		= vg_BootSignal[1];
		(pBoot_Signal[2])		= vg_BootSignal[2];
		(pBoot_Signal[3])		= vg_BootSignal[3];
		(pBoot_Signal[4])		= vg_BootSignal[4];
	}
	if(pBootSignalWait)			
		(*pBootSignalWait)	= vg_BootSignalWait;
}
*/
ULONG DutBtLowLevelClssHeader GetUartSpyFlag()
{
	return vg_IfSpy;
}

void DutBtLowLevelClssHeader SetUartSpyFlag(BOOL Flag_Spy)
{
	vg_IfSpy = Flag_Spy;
}


ULONG DutBtLowLevelClssHeader GetDebugLogFlag()
{
	return debugLog;
}
void DutBtLowLevelClssHeader SetDebugLogFlag(BOOL Flag_Log)
{
	debugLog = Flag_Log;
}



BOOL DutBtLowLevelClssHeader query(ULONG OID, PUCHAR Buf, UINT BufSize)
{
#define BUFSIZE	1024
#define EVENT_BUF_SIZE 400
	UCHAR EventData[EVENT_BUF_SIZE]={0}, Buffer[BUFSIZE]={0};
 	USHORT RetBufSize=0, FoundEvent=0;
	USHORT	CmdGrp=0;
	USHORT	CmdCode=0;
	PUCHAR pInBuf=NULL;
	UINT InBufSize=0;
	UINT i=0;
    int status=0;
	
     if(vg_IfSpy)
	 {
		 DebugLogRite("\nOID %08X\n", OID);
		 DebugLogRite("Command\n");

		 for (i=0; i<BufSize; i++)
			DebugLogRite("%02X ", *(Buf+i));

		 DebugLogRite("\n");
	 }

	CmdGrp = (WORD)(OID>>16);
	CmdCode = (WORD)(OID&0x3FF);
	 switch(CmdGrp)
	 {
		case HCI_CMD_GROUP_INFORMATIONAL:
			pInBuf=NULL;
			InBufSize=0; 
			break;
		case HCI_CMD_GROUP_HC_BASEBAND:
		case HCI_CMD_GROUP_VENDOR_SPECIFIC:
		case HCI_CMD_GROUP_LINK_CONTROL:
		case HCI_CMD_GROUP_LINK_POLICY:
		case HCI_CMD_GROUP_TESTING:
 			pInBuf=Buf;
			InBufSize=BufSize;
			break;
		default:
			return (-4);
			break;

	 }
	memset(EventData, 0, EVENT_BUF_SIZE);

	Sleep(vg_dwSleep);

 

	status = SendHciCommand(hPort,CmdGrp,  //
					CmdCode,
					HCI_EVENT_CMD_COMPLETE,
					pInBuf,
					InBufSize,
					&FoundEvent,
					EventData,
					(RetBufSize = EVENT_BUF_SIZE>BufSize?EVENT_BUF_SIZE:BufSize));
 	if(status)
	{
		return (status); 
	}

	if(vg_IfSpy)
	{
		 DebugLogRite("\nEventData\n");
		 for (i=0; i<RetBufSize; i++)
			DebugLogRite("%02X ", *(EventData+i));
	
		 DebugLogRite("\n");
	}


//	if (EventData[0]!=ERROR_SUCCESS)
//	{
//		 
//		return EventData[0];
//	}
//	else
	{
		memset(Buf, 0, BufSize);
		memcpy(Buf, &EventData[1], RetBufSize-1);

		if(vg_IfSpy)
		{
			 DebugLogRite("\nResponse\n");
			 for (i=0; i+1<RetBufSize; i++)
				DebugLogRite("%02X ", *(Buf+i));
		
			 DebugLogRite("\n");
		}
		return EventData[0];
	}
	
	return 0;
}



BOOL DutBtLowLevelClssHeader queryHci(ULONG OID, PUCHAR Buf, UINT BufSize, 
						UINT ExpectedEvent,	  
						UINT *BufRetSize)
{
#define BUFSIZE	1024
#define EVENT_BUF_SIZE 400
	UCHAR EventData[EVENT_BUF_SIZE]={0}, Buffer[BUFSIZE]={0};
 	USHORT RetBufSize=0, FoundEvent=0;
	USHORT	CmdGrp=0;
	USHORT	CmdCode=0;
	PUCHAR pInBuf=NULL;
	UINT InBufSize=0;
	UINT i=0;
    int status=0;
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


	CmdGrp = (WORD)(OID>>16);
	CmdCode = (WORD)(OID&0x3FF);
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
					&FoundEvent,
					EventData,
					(RetBufSize = EVENT_BUF_SIZE));
 	if(status)
	{
		return (status); 
	}

	if (EventData[0]!=ERROR_SUCCESS && FoundEvent != HCI_EVENT_VENDOR_SPECIFIC)
	{
		if( 0xFF == EventData[0] //&& 
			//(	(HCI_EVENT_VENDOR_SPECIFIC ==  (expectEvent&0x00FF)	) ||  
			//	(HCI_EVENT_VENDOR_SPECIFIC == ((expectEvent&0xFF00)>>8)) 
			//)
		  )
		{
			return (EventData[0] | (EventData[2]<<16));
		}
		else
		{
			return EventData[0];
		}
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
		}
		else
		{

			memcpy(Buf, &EventData[1], expectedReturnBuf);

			if(vg_IfSpy)
			{
				 DebugLogRite("\nResponse\n");
				 for (i=0; i<expectedReturnBuf; i++)
					DebugLogRite("%02X ", *(Buf+i));
			
				 DebugLogRite("\n");
			}
		}
			
		if(HCI_EVENT_VENDOR_SPECIFIC == FoundEvent && 
			(	(HCI_EVENT_CMD_COMPLETE ==  (expectEvent&0x00FF)	) ||  
				(HCI_EVENT_CMD_COMPLETE == ((expectEvent&0xFF00)>>8)) 
			)
		  )
		{
			RetBufSize = EVENT_BUF_SIZE; 
			GetEvent(hPort,  CmdGrp,  
					CmdCode, HCI_EVENT_CMD_COMPLETE, 
				 EventData, RetBufSize);

		}
		return EventData[0];

	}
	
	return 0;
}

int	DutBtLowLevelClssHeader UartFwDl()
{
	int status=0;
	char cmd[255]="";
	
	if(vg_UartFwDl)
	{
 
		status = CloseDevice();
		// open port for bootcode
		 hPort=InitUARTPort(PortName, vg_BootBaudRate, vg_BootRtsFlowCtrl, vg_OverLapped);
 		if(hPort==INVALID_HANDLE_VALUE || hPort==0) 
			return (int)INVALID_HANDLE_VALUE;
			
		if( !pollUart(hPort, vg_BootSignalNum, vg_BootSignal, vg_BootSignalWait))
		{

		// if not found 
		status = SetBaudRateFlowControlMode(hPort, vg_BaudRate, vg_RtsFlowControl);
		if(status) return status;
		}
		else
		{
		// if found , then close this port, UploadXmodem(), reopen port if success.
			status = CloseDevice();
 			if(status) return status;
 				// dl helper 
#ifndef _LINUX_
			sprintf(cmd, "fw_loader_win.exe \\\\.\\%s %d 0 helper_uart_%d.bin", 
				 PortName,  vg_BootBaudRate, vg_HelperBaudRate);
#else // #ifndef _LINUX_
			sprintf(cmd, "./fw_loader_linux %s %d helper_uart_%d.bin", 
				 PortName,  vg_BootBaudRate, vg_HelperBaudRate);
#endif // #ifndef _LINUX_
			DebugLogRite("%s\n",cmd);
			system(cmd);
			// dl Fw
#ifndef _LINUX_
			sprintf(cmd, "fw_loader_win.exe \\\\.\\%s %d 1 %s", 
				 PortName,  vg_HelperBaudRate, vg_FwImageName);
#else // #ifndef _LINUX_
			sprintf(cmd, "fw_loader_linux %s %d %s", 
				 PortName,  vg_HelperBaudRate, vg_FwImageName);
#endif // #ifndef _LINUX_
			DebugLogRite("%s\n",cmd);
			system(cmd);
				if(status) return status;

			hPort=InitUARTPort(PortName, vg_BaudRate, vg_RtsFlowControl, vg_OverLapped);

		}
 
	}

	return status;

}

#if 0
int	DutBtLowLevelClssHeader OpenDevice( )
{
//	char WholePortName[256]="\\\\.\\";
//	int i=0;
//	i = strnicmp(PortName, "COM", 3);
//	int i = strlen(PortName);
//	strcat(WholePortName, PortName);

	if(strlen(PortName) <4 || strnicmp(PortName, "COM", 3))
		return 1;
	else
	{
		return ((hPort=InitUARTPort(PortName, vg_BaudRate, vg_RtsFlowControl, vg_OverLapped))==NULL);
	}
}
#else
int	DutBtLowLevelClssHeader OpenDevice()
{
	int status=0;
	long StartTime={0}, NowTime={0};
	char Buffer[1]={0};
	unsigned long ByteRead=0;

//	char WholePortName[256]="\\\\.\\";
//	int i=0;
//	i = strnicmp(PortName, "COM", 3);
//	int i = strlen(PortName);
//	strcat(WholePortName, PortName);

#ifndef _LINUX_
	if(strlen(PortName) <4 || strnicmp(PortName, "COM", 3))
//	if(strlen(PortName) <4 || strncasecmp(PortName, "COM", 3))
			return 1;
	else
#endif 	//#ifndef _LINUX_
	DebugLogRite("%s\n", HciCtrlAbout());

	{
		{
		hPort=InitUARTPort(PortName, vg_BaudRate, vg_RtsFlowControl, vg_OverLapped);
 	
	 	if(hPort==INVALID_HANDLE_VALUE || hPort==0) 
			return (int)INVALID_HANDLE_VALUE;
		else 
			return ERROR_NONE;
		}

	}
	return ERROR_NONE;
}
#endif 

int	DutBtLowLevelClssHeader CloseDevice( )
{
	int status=0; 

	if(hPort)
	{
		status= ClosePort(hPort); 
		if(status) return status;
		hPort =0;
	}
		return status;
	
}
  
DWORD	DutBtLowLevelClssHeader GetErrorLast(void)   
{
#ifndef _LINUX_
	return GetLastError(); 
#else //#ifndef _LINUX_
	return 0;
#endif //#ifndef _LINUX_

}


BOOL  DutBtLowLevelClssHeader SendAclData(PUCHAR pData, ULONG DataLength)
{
	return SendHciData(hPort, true, pData, DataLength);
}

BOOL  DutBtLowLevelClssHeader RxData(PUCHAR pData, 
							   ULONG BufLength)
{
	return RxHciDataEvent(hPort, pData, BufLength);
}


void DutBtLowLevelClssHeader GetTimeOut(int *pTimeOut)
{
		
	*pTimeOut=GetTimeoutTimer(hPort);
}
void DutBtLowLevelClssHeader SetTimeOut(int TimeOut)
{
//	vg_maxWait = TimeOut;
	SetTimeoutTimer(hPort, TimeOut);
}

void DutBtLowLevelClssHeader ResetTimeOut(void)
{
//	vg_maxWait = TimeOut;
	SetTimeoutTimer(hPort, vg_maxWait);
}

void DutBtLowLevelClssHeader GetDataTimeOut(int *pTimeOut)
{
		
	*pTimeOut=GetPortTimeoutTimer(hPort);
}

void DutBtLowLevelClssHeader SetDataTimeOut(int TimeOut)
{
//	vg_maxWait = TimeOut;
	SetPortTimeoutTimer(hPort, TimeOut);
}

void DutBtLowLevelClssHeader ResetDataTimeOut(void)
{
//	vg_maxWait = TimeOut;
	SetPortTimeoutTimer(hPort, vg_DataMaxWait);
}
