
#include "StdAfx.h" 
#ifdef SIMPLEX_API
#include "WPASupp.h"
#endif //#ifdef SIMPLEX_API
 
extern MRVL_DATA_BUFFER EventBuffer;
extern UCHAR EventQueryBuf[EVENT_QUERY_BUF_SIZE];
extern HANDLE hRegEvent;
extern HANDLE hValidDevice;
extern HANDLE hTerminateEvent;
extern HANDLE hExitEvent;
extern BOOL bSwitchAP;
 
#ifdef SIMPLEX_API
//WPA
extern HODSTK_CONTEXT	m_hWPAContext;
extern HODSTK_LINK		m_hWPALink;
#endif

extern USHORT nPktCmplSeqNum;

 

#ifdef SIMPLEX_API
extern ULONG g_nWPALinkStatus;
extern ULONG g_nWPAAuthStatus;
extern ULONG g_nWPASuppCallbackStatus;

// Counter of retry for Supplicant
int g_nWPAPSKKeyWaitCount = 0;
#endif //#ifdef SIMPLEX_API



////////////////
  int Mrv8000xGetEventCode(HANDLE hAdapter, LPBYTE lpBuf, ULONG nSize);

  BOOL Mrv8000xGetConnectStatus(HANDLE hAdapter);
  int Mrv8000xSetDisassociate(HANDLE hAdapter);
 

//====================================================
//	Thread: Handle event code from driver
//====================================================
DWORD WINAPI EventHandler(LPVOID lpvParam)
{	
	PMRVL_EVENT_BUFFER_LIST pEventList;
	PMRVL_EVENT_BUFFER pEvent;
	HANDLE EventArray[3];
	ULONG	nCmdCode;
	DWORD dwObject;
	ULONG i;

	EventArray[0] = hTerminateEvent;
	EventArray[1] = hExitEvent;
	EventArray[2] = EventBuffer.Event;

	while (1)
	{
 		dwObject = WaitForMultipleObjects(3, EventArray, FALSE, INFINITE);

		if (dwObject == WAIT_OBJECT_0)
		{
			break;
		}
		else if (dwObject == (WAIT_OBJECT_0+1))
		{
			ResetEvent(hExitEvent);
			ResetEvent(EventBuffer.Event);
			Mrv8000xCloseAdapter(hValidDevice); 
 			continue;
		}	
 
	#ifdef REMOVE_LOCK_PAGE
		if (Mrv8000xGetEventCode(hValidDevice, (LPBYTE)EventBuffer.Buffer, EventBuffer.BufferSize)!=STATUS_SUCCESS)
		{
			ResetEvent(EventBuffer.Event);
			continue;
		}
	#endif
 
		ResetEvent(EventBuffer.Event);

	#ifdef REMOVE_LOCK_PAGE
		pEventList = (PMRVL_EVENT_BUFFER_LIST)EventBuffer.Buffer;
		for (i=0; i<pEventList->NumOfEvents; i++)
		{

			if (i==0)
			{
				pEvent = (PMRVL_EVENT_BUFFER)(pEventList->Events); 
			}
			else
			{
				pEvent = (PMRVL_EVENT_BUFFER)((LPBYTE)pEvent + pEvent->Length);	 
			}

			// make sure the pointer is valid
			if(pEvent==(PMRVL_EVENT_BUFFER)0xffffffff || pEvent==(PMRVL_EVENT_BUFFER)0) 
			{
				continue;
			}
			
			nCmdCode = pEvent->EventCode;
	#else
		nCmdCode = *((ULONG*)(EventBuffer.Buffer));
	#endif
			if (nCmdCode == COMMAND_EVENT_EXIT)
			{
				Sleep(500); //delay to make sure driver finish what it should do
				Mrv8000xCloseAdapter(hValidDevice); 
			}
			else if (nCmdCode == COMMAND_EVENT_QUERY_DONE)
			{
			#ifndef REMOVE_LOCK_PAGE
				CopyMemory((LPBYTE)EventQueryBuf, (LPBYTE)(CmdBuffer+sizeof(DWORD)), EVENT_QUERY_BUF_SIZE);
			#else
				CopyMemory((LPBYTE)EventQueryBuf, (LPBYTE)(pEvent->Buffer), EVENT_QUERY_BUF_SIZE);
			#endif				
				SetEvent(hRegEvent);
				continue;
			}
		 	 
	#ifdef REMOVE_LOCK_PAGE
		}
	#endif
	}

	return 0;
}
 