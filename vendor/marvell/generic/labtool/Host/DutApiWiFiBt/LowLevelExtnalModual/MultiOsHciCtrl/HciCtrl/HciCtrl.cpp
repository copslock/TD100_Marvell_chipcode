/** @file HciCtrl.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include "UartIf.h"
#include "HciCtrlVer.h"
#include "../Common/HciCtrl.h"
#include "../Common/HciStruct.h"
#include "../Common/hci_opcodes.h"
#include "../Common/UartErrCode.h"
#include "../../../../DutCommon/utility.h"

 
ULONG nTimer=5000; 
ULONG nPortTimer=100; 
bool LastIsTO=0;
bool flowCtrl=0;

//=================================================================
// Internal Functions
//=================================================================
#ifndef _LINUX_
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
#endif //#ifdef _LINUX_



BOOL SendCommandToController(HANDLE hPort, UCHAR CmdGroup, 
							 USHORT CmdCode, UCHAR *pParam, USHORT ParamLength)
{
	PHCI_COMMAND_STRUCT pHciCmd=NULL;
	PUCHAR pBuffer=NULL;
	DWORD ByteSent=0;
	ULONG BufLen=0;

	
	if (hPort==0 ||hPort == INVALID_HANDLE_VALUE)
	{ 
		return FALSE;
	}

	BufLen = ParamLength+4;
	
	pBuffer = (PUCHAR)malloc(BufLen);
	memset((PUCHAR)pBuffer, 0, BufLen);
	
	pHciCmd = (PHCI_COMMAND_STRUCT)(pBuffer+1);

	*pBuffer=HCI_COMMAND_PACKET;
	
	pHciCmd->CmdCode=CmdCode;
	pHciCmd->CmdGroup=CmdGroup;// <<2;
	pHciCmd->ParamLength=(UCHAR)ParamLength;
	
	if (ParamLength>0)
	{
		memcpy(pHciCmd->Param, pParam, ParamLength);
	}

	if (//LastIsTO &&
		ClearRxBuf(hPort))
	{
		return FALSE;
	}
 
	if(flowCtrl)
	{
#ifdef _LINUX_
	if(-1 == tcflush(hPort,TCIOFLUSH))
	{
 		return FALSE;
	}
#else	
	if (FlushFileBuffers(hPort) == 0)      // clean buffer
    {
 	  return FALSE;
    }
#endif 
	}

 	if (WriteUartFile(hPort,			// handle to file to write to
				pBuffer,              // pointer to data to write to file
				BufLen,              // number of bytes to write
				&ByteSent) == 0)      // pointer to number of bytes written
    {
 	  return FALSE;
    }
 
	return TRUE;
}


 

int WaitForEvent(HANDLE hPort, USHORT CmdGroup, USHORT CmdCode, USHORT TargetEvent, PHCI_EVENT_STRUCT pEvent, ULONG BufSize)
{
 	UCHAR Target1=0, Target2=0;
	int Status=0;
	ULONG StartTimer=0;

	if (hPort==0 ||hPort == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	Target1 = (UCHAR)(TargetEvent>>8);
	Target2 = (UCHAR)(TargetEvent&0xFF);
	StartTimer = GetTickCount();

	while (1)
	{
		if (GetTickCount()-StartTimer >= nTimer)
		{
			return ERROR_GET_EVENT_TO;
		}

		if ((Status=RxHciDataEvent(hPort, (PUCHAR)pEvent, BufSize))!=ERROR_SUCCESS)
		{
			if (Status == ERROR_UNKNOWN_PACKET || 
				Status == ERROR_GET_EVENT_TO ||
				pEvent->IsDataPkt)
			{
				continue; 
			}
			
			return Status;
		}

 		if (pEvent->EventCode == Target1 || pEvent->EventCode == Target2)
		{
 			if (pEvent->EventCode == HCI_EVENT_CMD_COMPLETE)
			{
				if ( (pEvent->CmdComplete.OCF)== (CmdCode) && 
				     (pEvent->CmdComplete.OGF)== (CmdGroup))
				{
 					return ERROR_SUCCESS;
				}
			}
			else if (pEvent->EventCode == HCI_EVENT_CMD_STATUS) 
			{
				if (pEvent->CmdStatus.OCF==CmdCode && pEvent->CmdStatus.OGF==CmdGroup)
				{
 					return ERROR_SUCCESS;
				}
			}
			else
			{
 			return ERROR_SUCCESS;
			}
		}
		else if (pEvent->EventCode == HCI_EVENT_CMD_STATUS 
				&& pEvent->CmdStatus.OCF==CmdCode 
				&& pEvent->CmdStatus.OGF==CmdGroup 
				&& pEvent->CmdStatus.Status!=ERROR_SUCCESS)
		{
			return pEvent->CmdStatus.Status;
		}				
	}
}
	
//=================================================================
// Export Functions
//=================================================================
HCICTRL_API HANDLE InitUARTPort(char *szPort, DWORD BaudRate, DWORD flowControl, bool OverLapped)
{
	return InitUartPort(szPort, BaudRate, flowControl, OverLapped);
}


HCICTRL_API BOOL ClosePort(HANDLE hPort)
{
	return CloseUartPort(hPort);
}

 

HCICTRL_API int SendHciCommand(HANDLE hPort, USHORT CmdGroup, USHORT CmdCode, USHORT ExpEvent, 
							   PUCHAR pParam, USHORT ParamLength, 
								PUSHORT pFoundEvent,
							   PUCHAR pReturn, USHORT &RetBufSize)
{
	HCI_EVENT_STRUCT *pHciEvent=NULL;
 	int Status=0, BufSize=1024;
	char Buf[1024]={0};
 
	pHciEvent = (HCI_EVENT_STRUCT*)Buf;
	if(CmdGroup !=0 && CmdCode !=0)
	{
		if (!SendCommandToController(
				hPort,
				CmdGroup, 
				CmdCode, 
				pParam, 
				ParamLength)) // when clear all filter, only one parameter is used
		{
			return ERROR_HARDWARE_FAILURE; //0x03
		}
	}

	if ((Status=WaitForEvent(
			hPort,
			CmdGroup, 
			CmdCode, 
			ExpEvent,
			pHciEvent,
			BufSize //sizeof(HCI_EVENT_STRUCT)
			))==ERROR_SUCCESS)
	{
	 
		if (RetBufSize >= (*pHciEvent).ParamLength)
		{

			*pFoundEvent = (*pHciEvent).EventCode;

			if ((*pHciEvent).EventCode == HCI_EVENT_CMD_COMPLETE)
			{
				RetBufSize = (*pHciEvent).CmdComplete.ParamLength;
				memcpy(pReturn, (*pHciEvent).CmdComplete.pParam, 
							(*pHciEvent).CmdComplete.ParamLength);
			}
			else if ((*pHciEvent).EventCode == HCI_EVENT_VENDOR_SPECIFIC)
			{
				RetBufSize = (*pHciEvent).ParamLength;
				memcpy(pReturn, &(*pHciEvent).EventData[0], (*pHciEvent).ParamLength);
			}
			else 
			{
				RetBufSize = (*pHciEvent).ParamLength;
				memcpy(pReturn, (*pHciEvent).EventData, (*pHciEvent).ParamLength);
			}
	
			return ERROR_SUCCESS;
		}
		else
		{
			return ERROR_BUFFER_TOO_SMALL;
		}
	}
 	else
	{
		LastIsTO = (Status == ERROR_GET_EVENT_TO);
 		return Status;
	} 
}


HCICTRL_API int GetEvent(HANDLE hPort, 
						 USHORT CmdGroup, USHORT CmdCode, 
						 USHORT ExpEvent, 
						 PUCHAR pEvent, USHORT RetBufSize)
{
	HCI_EVENT_STRUCT *pHciEvent=NULL;
 	int Status=0, BufSize=1024;
	char Buf[1024]={0};
 
	pHciEvent = (HCI_EVENT_STRUCT*)Buf;

	if ((Status=WaitForEvent(
			hPort,
			CmdGroup, 
			CmdCode, 
			ExpEvent,
			pHciEvent,
			BufSize //sizeof(HCI_EVENT_STRUCT)
			))==ERROR_SUCCESS)
	{
	 
		if (RetBufSize >= (*pHciEvent).ParamLength)
		{
			if ((*pHciEvent).EventCode == HCI_EVENT_CMD_COMPLETE)
			{
				RetBufSize = (*pHciEvent).CmdComplete.ParamLength;
				memcpy(pEvent, (*pHciEvent).CmdComplete.pParam, 
							(*pHciEvent).CmdComplete.ParamLength);
			}
			else if ((*pHciEvent).EventCode == HCI_EVENT_VENDOR_SPECIFIC)
			{
				RetBufSize = (*pHciEvent).ParamLength;
				memcpy(pEvent, &(*pHciEvent).EventData[0], (*pHciEvent).ParamLength);
			}
			else 
			{
				RetBufSize = (*pHciEvent).ParamLength;
				memcpy(pEvent, (*pHciEvent).EventData, (*pHciEvent).ParamLength);
			}
	
			return ERROR_SUCCESS;
		}
		else
		{
			return ERROR_BUFFER_TOO_SMALL;
		}
	}
 	else
	{
		LastIsTO = (Status == ERROR_GET_EVENT_TO);
 		return Status;
	} 

}

HCICTRL_API int SendHciData(HANDLE hPort, BOOL IsAcl, PUCHAR pData, ULONG DataLength)
{
	HCI_EVENT_STRUCT Receive={0};
	DWORD ByteSent=0, ByteToSend=0; 
	ULONG TotalSent=0, BufLen=0;
 	PUCHAR pBuffer=NULL;
	BOOL bFirst = TRUE;
 
	if (hPort==0 ||hPort == INVALID_HANDLE_VALUE)
	{
		return ERROR_INVALID_HANDLE;
	}

	if (pData==0)
	{
		return ERROR_INVALID_DATA;
	}

	if(flowCtrl)
	{
#ifdef _LINUX_
	if(-1 == tcflush(hPort,TCIOFLUSH))
	{
 		return FALSE;
	}
#else
	if (FlushFileBuffers(hPort) == 0)      // clean buffer
    {
 	  return GetLastError();
    }
#endif //#ifndef _LINUX_
	}

	BufLen = DataLength+1;	
	pBuffer = (PUCHAR)malloc(BufLen);
	memset(pBuffer, 0, BufLen);

	pBuffer[0]=IsAcl?HCI_ACL_DATA_PACKET:HCI_SCO_DATA_PACKET;
	memcpy(pBuffer+1, pData, DataLength);
 
 	while (TotalSent < BufLen)
	{
 	 	ByteToSend = (BufLen-TotalSent);
 
		if (WriteUartFile(hPort, // handle to file to write to
				pBuffer+TotalSent,              // pointer to data to write to file
				ByteToSend, //BufLen,              // number of bytes to write
				&ByteSent) == 0)      // pointer to number of bytes written
		{
 			free(pBuffer);
			return ERROR_WRITE_FAULT;
		}
 
		TotalSent += ByteSent;
  
	 	while (bFirst && TotalSent<DataLength)
		{
  			RxHciDataEvent(hPort, (PUCHAR)&Receive, sizeof(HCI_EVENT_STRUCT));

			if (Receive.EventCode == HCI_EVENT_CMD_STATUS)
			{
				if (Receive.CmdStatus.Status!=ERROR_SUCCESS)
				{
 					free (pBuffer);
					return Receive.CmdStatus.Status;
				}			
				break;
			}
		}   
 
 	}
	
	free (pBuffer);
	return ERROR_NONE;
}


HCICTRL_API int RxHciDataEvent(HANDLE hPort, PUCHAR pData, ULONG BufLength)
{

	PHCI_EVENT_STRUCT pEvent=NULL;
	UCHAR Buffer[2048]={0};
	DWORD ByteRead=0;
	UINT i=0, Count=0;


 	if (hPort==0 ||hPort == INVALID_HANDLE_VALUE)
	{
 		return ERROR_INVALID_HANDLE;
	}
	
	memset(pData, 0, BufLength);
	pEvent = (PHCI_EVENT_STRUCT) pData;

 
	if (BufLength < sizeof(HCI_EVENT_STRUCT))
	{

 		return ERROR_BUFFER_TOO_SMALL;
	}

	{
		memset(Buffer, 2048,0);
		if (ReadBytes(hPort, Buffer, 1, &ByteRead)==0)
		{
 			return ERROR_ACCESS_COM_PORT;
		}
 		if( 0 == ByteRead) 
		{
 			return ERROR_GET_EVENT_TO;
		}

		if (Buffer[0]==HCI_EVENT_PACKET)
		{
 			pEvent->IsDataPkt = FALSE; 

			memset(Buffer, 2048,0);

  			if (ReadBytes(hPort, Buffer, 2, &ByteRead)!=0)
			{	if(ByteRead)
				pEvent->EventCode = Buffer[0];
				pEvent->ParamLength = Buffer[1];
		
 
				if (BufLength < (ULONG)(pEvent->ParamLength+2))
				{
 					return ERROR_BUFFER_TOO_SMALL;
				}
								
				Count=100;
				while (pEvent->ParamLength > CheckInQ(hPort) && (--Count)>0)
				{
					Sleep(1);
				}
 
				memset(Buffer, 2048,0);
				ReadBytes(hPort, Buffer, pEvent->ParamLength, &ByteRead);
 
 							
				switch (pEvent->EventCode)
				{
					case HCI_EVENT_CMD_COMPLETE:
							memcpy((PUCHAR)&pEvent->CmdComplete, Buffer, 3); // NumHciCmdPkt, OCF, OGF
							pEvent->CmdComplete.ParamLength = pEvent->ParamLength - 3;

							if (pEvent->CmdComplete.ParamLength>0)
							{
								pEvent->CmdComplete.pParam = (PUCHAR)malloc(pEvent->CmdComplete.ParamLength);
								memcpy((PUCHAR)pEvent->CmdComplete.pParam, Buffer+3, pEvent->CmdComplete.ParamLength);
							}
							else
							{
								pEvent->CmdComplete.pParam = 0;
							}
							break;
					case HCI_EVENT_INQUIRY_RESULT:
 	 						Count = pEvent->InquiryResult.NumResponses = Buffer[0];

							for (i=0; i<MAX_INQUIRY_RESPONSES && i<Count; i++)
							{
								memcpy(pEvent->InquiryResult.RespDev[i].BdAddress, Buffer+1+i*SIZE_OF_BD_ADDRESS, SIZE_OF_BD_ADDRESS);
								pEvent->InquiryResult.RespDev[i].PageScanRepMode = *(Buffer+1+Count*SIZE_OF_BD_ADDRESS+i);
								memcpy(pEvent->InquiryResult.RespDev[i].COD, Buffer+1+Count*(SIZE_OF_BD_ADDRESS+3)+3*i, 3);
								memcpy((PUCHAR)&(pEvent->InquiryResult.RespDev[i].ClockOffset), Buffer+1+Count*(SIZE_OF_BD_ADDRESS+6)+2*i, 2); // 2 bytes reserved
							}
							break;
					default:
							memcpy((PUCHAR)&pEvent->EventData, Buffer, pEvent->ParamLength);
							break;
				} // switch
			} // if 
		} 
		else if (Buffer[0] == HCI_ACL_DATA_PACKET)
		{
  			pEvent->IsDataPkt = TRUE; 
			pEvent->EventCode = Buffer[0];

 			if (ReadBytes(hPort, Buffer, 4, &ByteRead)!=0)
			{
				Count = *((USHORT*)(Buffer+2));
				
 				ReadBytes(hPort, Buffer+4, Count, &ByteRead);
				
				if (BufLength < (Count+4))
				{
 					return ERROR_BUFFER_TOO_SMALL;
				}
				else
				{
					memcpy((PUCHAR)&pEvent->AclData, Buffer, Count+4);
				}
			}
		}
		else if (Buffer[0] == HCI_SCO_DATA_PACKET)
		{
 			pEvent->IsDataPkt = TRUE; 
			pEvent->EventCode = Buffer[0];

 			if (ReadBytes(hPort, Buffer, 3, &ByteRead)!=0)
			{
				Count = (UCHAR)Buffer[2];
				
 				ReadBytes(hPort, Buffer+3, Count, &ByteRead);

				if (BufLength < (Count+3)) // buffer too small, read the whole data anyway
				{
 					return ERROR_BUFFER_TOO_SMALL;
				}
				else
				{
					memcpy((PUCHAR)&pEvent->ScoData, Buffer, Count+3);
				}
			}
		}
		else if (Buffer[0] == HCI_OUT_OF_SYNC)
		{
 			return ERROR_OUT_OF_SYNC;
		}
		else if (Buffer[0] == HCI_COMMAND_PACKET)
		{
 			return ERROR_COMMAND_PACKET;
		}
		else  
		{
 			return ERROR_UNKNOWN_PACKET;
		}
	}
	
 	return ERROR_SUCCESS;


}

HCICTRL_API ULONG GetTimeoutTimer(HANDLE hPort)
{
 
		return nTimer;
}
HCICTRL_API void SetTimeoutTimer(HANDLE hPort, ULONG newTimer)
{
	nTimer = newTimer;
 
}

HCICTRL_API ULONG GetPortTimeoutTimer(HANDLE hPort)
{
	return GetUartTimeoutTimer(hPort);
}

HCICTRL_API void SetPortTimeoutTimer(HANDLE hPort, ULONG newTimer)
{ 
	SetUartTimeoutTimer(hPort,newTimer);
	return;
}
 

 
 
HCICTRL_API int SetBaudRateFlowControlMode(HANDLE hPort, DWORD BaudRate, DWORD flowControl)
{
	flowCtrl=flowControl;

	return  SetUartConfig( hPort,  BaudRate,  flowControl);
}
 
 
HCICTRL_API int HciCtrlVersion()
{
	return MFG_VERSION(HCICTRL_VERSION_MAJOR1, HCICTRL_VERSION_MAJOR2, 
		HCICTRL_VERSION_MINOR1,HCICTRL_VERSION_MINOR2);

}


HCICTRL_API char* HciCtrlAbout()
{
	char AboutFormat[512]=
			"Name:\t\tHCI CTRL\n"
#ifdef _UDP_
			"Interface:\t""UART\n" 
#endif	
			"Version:\t%d.%d.%d.%02d\n" 
 			"Date:\t\t%s (%s)\n\n" 			
 			"Note:\t\t\n" 
			;
 
	static char AboutString[512]="";
 
	sprintf(AboutString, AboutFormat, 
			HCICTRL_VERSION_MAJOR1,	
			HCICTRL_VERSION_MAJOR2,
			HCICTRL_VERSION_MINOR1, 
			HCICTRL_VERSION_MINOR2,
			__DATE__, __TIME__);

	return AboutString; 
}

