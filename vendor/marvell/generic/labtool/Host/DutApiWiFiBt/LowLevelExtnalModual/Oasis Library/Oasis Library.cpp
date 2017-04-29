// Oasis Library.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h" 
#include "Tchar.h" 

DWORD vg_maxWait= 5000; //ms 
extern  int Mrv8000xRegisterEvent(HANDLE hAdapter, LPBYTE lpBuf, ULONG nSize);
extern  int Mrv8000xRegisterExitEvent(HANDLE hAdapter, LPBYTE lpBuf, ULONG nSize);
extern  int Mrv8000xUnlockPages(HANDLE hAdapter);
 
//====================================
//		Public Variable
//====================================
int WindowsVersion = -1;
HANDLE hValidDevice = 0;
DWORD ChipVersion=0;
DWORD FmDrvVersion=0;
DWORD HasLockedPages=0;
//DWORD AssocStatus=0;

ULONG  nCCXConfiguration = 0;
USHORT nPktTxSeqNum = 0;
USHORT nPktCmplSeqNum = 0;

TCHAR RegistryPath[ADAPTER_REGPATH_BUF_SIZE];
TCHAR RegNetTransPath[ADAPTER_REGPATH_BUF_SIZE];
TCHAR AdapterName[ADAPTER_REGPATH_BUF_SIZE];

MRVL_DATA_BUFFER EventBuffer;
UCHAR  CmdBuffer[EVENT_BUFFER_SIZE];
UCHAR EventQueryBuf[EVENT_QUERY_BUF_SIZE];
HANDLE hEventHandler;
HANDLE hExitEvent;  
HANDLE hRegEvent;
HANDLE hTerminateEvent;



//====================================
//		DLL Entry
//====================================
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
   DWORD dwThreadId;
   
   switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH: 
			ZeroMemory(RegistryPath, ADAPTER_REGPATH_BUF_SIZE*sizeof(TCHAR));
			ZeroMemory(RegNetTransPath, ADAPTER_REGPATH_BUF_SIZE*sizeof(TCHAR));
			ZeroMemory(AdapterName, ADAPTER_REGPATH_BUF_SIZE*sizeof(TCHAR));
		
			EventBuffer.BufferSize = EVENT_BUFFER_SIZE;
			EventBuffer.Buffer = CmdBuffer;	

 			if ((EventBuffer.Event = CreateEvent(NULL, TRUE, FALSE, NULL))==NULL)
			{
				return FALSE;
			}

			if ((hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL))==NULL)
			{
				return FALSE;
			}
						
			if ((hRegEvent = CreateEvent(NULL, TRUE, FALSE, NULL))==NULL)
			{
				return FALSE;
			}
			if ((hTerminateEvent = CreateEvent(NULL, FALSE, FALSE, NULL))==NULL)
			{
				return FALSE;
			}
			if ((hEventHandler=CreateThread(NULL, 0, EventHandler, 0, 0, &dwThreadId))==NULL)
			{
				return FALSE;
			}
		 
		#ifdef CCX2
			nCCXConfiguration =	FSW_CCX_CONFIGURATION_ENABLE_ROGUE_AP |
							FSW_CCX_CONFIGURATION_ENABLE_CCKM;

		#else
			nCCXConfiguration =	FSW_CCX_CONFIGURATION_ENABLE_ROGUE_AP;
		#endif
 			break;
		case DLL_THREAD_ATTACH: break;
		case DLL_THREAD_DETACH: break;
		case DLL_PROCESS_DETACH: 
 
				SetEvent(hTerminateEvent);
				WaitForSingleObject(hEventHandler, THREAD_EXIT_TIMEOUT);
				CloseHandle(hEventHandler);
				CloseHandle(EventBuffer.Event);
				CloseHandle(hExitEvent);	
 				CloseHandle(hRegEvent);
 				break;
		default: break;
    }
  
	return TRUE;
}

//========================================================
//		Export Functions : Open and Close Device
//========================================================
MFGDLL_API int Mrv8000xOpenAdapter(HANDLE &hAdapter, bool Gru)
{
	char     szDriverName[ _MAX_PATH ]={0};

	if(Gru)
		sprintf( szDriverName, "%s", MARVELL_8000x_GRU0);
	else
		sprintf( szDriverName, "%s", MARVELL_8000x_DEV0);

	hAdapter = CreateFile(
                    szDriverName,	// MARVELL_8000x_DEV0,           // Open the Device "file"
                    0, //GENERIC_READ | GENERIC_WRITE,
                    0, //FILE_SHARE_READ | FILE_SHARE_WRITE ,
                    NULL,
                    OPEN_EXISTING,
                    0,
                    NULL);

    if (hAdapter == INVALID_HANDLE_VALUE || hAdapter==NULL)        // Was the device opened?
    {
  		hAdapter = NULL;
		return STATUS_FAILURE;
    }
	else
	{
		hValidDevice = hAdapter;
	#ifndef REMOVE_LOCK_PAGE
		Mrv8000xRegisterEvent(hAdapter, (LPBYTE)&EventBuffer, sizeof(MRVL_DATA_BUFFER));
	#else
		Mrv8000xRegisterEvent(hAdapter, (LPBYTE)&(EventBuffer.Event), sizeof(HANDLE));
		Mrv8000xRegisterExitEvent(hAdapter, (LPBYTE)&(hExitEvent), sizeof(HANDLE));
	#endif
		ZeroMemory(RegistryPath, ADAPTER_REGPATH_BUF_SIZE*sizeof(TCHAR));
		ZeroMemory(RegNetTransPath, ADAPTER_REGPATH_BUF_SIZE*sizeof(TCHAR));
		ZeroMemory(AdapterName, ADAPTER_REGPATH_BUF_SIZE*sizeof(TCHAR));
 
		return STATUS_SUCCESS;
	}
}

MFGDLL_API int Mrv8000xCloseAdapter(HANDLE &hAdapter)
{
	int nStatus;
	
	if (hAdapter != hValidDevice || hAdapter == INVALID_HANDLE_VALUE || hAdapter == NULL)
	{
		if (hValidDevice==NULL)
		{
			hAdapter = NULL;
			return STATUS_SUCCESS;
		}
		
		return STATUS_FAILURE;
	}
	
	if (hValidDevice && HasLockedPages)
	{
		 Mrv8000xUnlockPages(hValidDevice);
 	}	
	
	nStatus = CloseHandle(hAdapter)?STATUS_SUCCESS:STATUS_FAILURE;
	
	hAdapter = NULL;
	hValidDevice = NULL;
	ChipVersion = 0;
	FmDrvVersion = 0;
	ZeroMemory(RegistryPath, ADAPTER_REGPATH_BUF_SIZE*sizeof(TCHAR));
	ZeroMemory(AdapterName, ADAPTER_REGPATH_BUF_SIZE*sizeof(TCHAR));
 
	return nStatus;
}
 

MFGDLL_API DWORD GetMaxtimeOut(void)
{
return vg_maxWait;
}

MFGDLL_API void SetMaxtimeOut(DWORD TimeOut)
{
  vg_maxWait = TimeOut;
}
//-------------------------------Internal Use----------------------------------------
//
//========================================================	  
//		Query OID
//========================================================
MFGDLL_API int MrvlQueryInfo(HANDLE hAdapter, ULONG OidCode, LPBYTE lpData, ULONG nDataLen)
{//	MRV8000X_REG_ACCESS Buffer;
	int nStatus;
		
//	Buffer.Index = Index;
	ZeroMemory((LPBYTE)&EventQueryBuf, EVENT_QUERY_BUF_SIZE);
	ResetEvent(hRegEvent);
	
	nStatus=Mrv8000xQueryInfo(hAdapter, OidCode, lpData, nDataLen );
 
	switch (nStatus)
	{
		case STATUS_SUCCESS:
	//		Value = Buffer.Index;  
			break;
		case NDIS_STATUS_PENDING:
			if (WaitForSingleObject(hRegEvent, vg_maxWait)!=WAIT_TIMEOUT)
			{
				memcpy(lpData, (EventQueryBuf+sizeof(ULONG)), nDataLen) ; 
				break;
			}			
		default : 
		//	Value = INVALID_REG_VALUE;
			break;
	}

	return STATUS_SUCCESS;	
}
   

 int Mrv8000xQueryInfo(HANDLE hAdapter, ULONG OidCode, LPBYTE lpData, ULONG nDataLen)
{
	int  nStatus;
	LPBYTE lpQueryBuf;
	ULONG nReturnedLength=0, nBufSize;

	if (hAdapter != hValidDevice || hAdapter == INVALID_HANDLE_VALUE || hAdapter == NULL)
	{
		return STATUS_FAILURE;
	}
	
	//=======================
	// Prepare Data Buffer
	//=======================
	nBufSize = sizeof(NDIS_OID)+nDataLen;
	nBufSize += (4-(nBufSize % 4)); // make it DWORD alignment

	lpQueryBuf = (LPBYTE) malloc(nBufSize);

	ZeroMemory(lpQueryBuf, nBufSize);

	*((NDIS_OID*)lpQueryBuf) = (NDIS_OID)OidCode;
	memcpy(lpQueryBuf+sizeof(NDIS_OID), lpData, nDataLen);


	if (DeviceIoControl(
					hAdapter,					// Handle to device
                    IOCTL_MRVL_QUERY_OID_VALUE, // IO Control code for Write
                    lpQueryBuf,					// Buffer to driver.  Holds port & data.
                    nBufSize,					// Length of buffer in bytes.
                    lpQueryBuf,					// Buffer from driver.   Not used.
                    nBufSize,					// Length of buffer in bytes.
                    &nReturnedLength,			// Bytes placed in outbuf.  Should be 0.
                    NULL					// NULL means wait till I/O completes.
                    ))
	 {
		nStatus = STATUS_SUCCESS;
		memcpy(lpData, lpQueryBuf+sizeof(ULONG), nDataLen);
	 }
	 else
	 {
		nStatus = *((ULONG*)lpQueryBuf);

		if (nReturnedLength>sizeof(ULONG))
		{
			memcpy(lpData, lpQueryBuf+sizeof(ULONG), nDataLen);
			nStatus = STATUS_SUCCESS;
		}

		if (nStatus==NDIS_STATUS_FAILURE)
		{
			nStatus = STATUS_FAILURE;
		} 
	 }

	 free (lpQueryBuf);
	return nStatus;

}
 

int Mrv8000xGetEventCode(HANDLE hAdapter, LPBYTE lpBuf, ULONG nSize)
{
	if (IsBadWritePtr(lpBuf, nSize))
	{
		return STATUS_INVALID_BUFFER;
	}

	ZeroMemory(lpBuf, nSize);

	return Mrv8000xQueryInfo(hAdapter, OID_MRVL_EVENT_CODE, lpBuf, nSize);
} 
