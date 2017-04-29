/***********************************************************
*
*		Export Function: Set Information
*
***********************************************************/
#include "StdAfx.h"
extern HANDLE hValidDevice;
extern DWORD HasLockedPages;
 
//===================================================================
//				Register Event Handle
//===================================================================
 int Mrv8000xRegisterEvent(HANDLE hAdapter, LPBYTE lpBuf, ULONG nSize)
{
	ULONG nReturnedLength;
	int nStatus;

	if (hAdapter != hValidDevice || hAdapter == INVALID_HANDLE_VALUE || hAdapter == NULL)
	{
		return STATUS_FAILURE;
	}
	
	nStatus = (DeviceIoControl(hAdapter, IOCTL_MRVL_PROVIDE_EVENT_HANDLE,
                         (LPBYTE)lpBuf, nSize, NULL, 0,
                         &nReturnedLength, NULL)?STATUS_SUCCESS:STATUS_FAILURE);

	if (nStatus == STATUS_SUCCESS)
	{
		HasLockedPages = 1;
	}

	return nStatus;
}
 
 
 int Mrv8000xRegisterExitEvent(HANDLE hAdapter, LPBYTE lpBuf, ULONG nSize)
{
	ULONG nReturnedLength;
	int nStatus;

	if (hAdapter != hValidDevice || hAdapter == INVALID_HANDLE_VALUE || hAdapter == NULL)
	{
		return STATUS_FAILURE;
	}
	
	nStatus = (DeviceIoControl(hAdapter, IOCTL_MRVL_PROVIDE_EXIT_EVENT_HANDLE,
                         (LPBYTE)lpBuf, nSize, NULL, 0,
                         &nReturnedLength, NULL)?STATUS_SUCCESS:STATUS_FAILURE);

	if (nStatus == STATUS_SUCCESS)
	{
		HasLockedPages = 1;
	}

	return nStatus;
}
 
//===================================================================
//				Unlock Pages
//===================================================================
 int Mrv8000xUnlockPages(HANDLE hAdapter)
{
#ifndef REMOVE_LOCK_PAGE
	ULONG nReturnedLength;
	ULONG nBuffer;
	int nStatus;

	if (hAdapter != hValidDevice || hAdapter == INVALID_HANDLE_VALUE 
		|| hAdapter == NULL || HasLockedPages == 0)
	{
		return STATUS_FAILURE;
	}
	
	nStatus = (DeviceIoControl(hAdapter, IOCTL_MRVL_UNLOCK_PAGES,
                         &nBuffer, sizeof(ULONG), NULL, 0,
                         &nReturnedLength, NULL)?STATUS_SUCCESS:STATUS_FAILURE);

	if (nStatus == STATUS_SUCCESS)
	{
		HasLockedPages = 0;
	}

	return nStatus;
#else
	return STATUS_SUCCESS;
#endif
}
   