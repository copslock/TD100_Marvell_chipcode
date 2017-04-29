/** @file DutIf_Ndis.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include "stdafx.h" 
#ifndef _NDIS_CLSS_CPP_
#define _NDIS_CLSS_CPP_
 
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <direct.h>


#include "../../windis5/include/WINDIS.h"
 
#include "_ntddndis.h"   // from DDK; modified not to conflict with WINDIS.h
#include "DutIf_IfClss.h"
#include "ioctl.h"
#include "utilities.h"
#include "DutIf_Ndis.h"

#include "DutApi.h"
 
 
static OVERLAPPED g_overlapped;

// prefix of adapter description
static char adapter_prefix[256] = "Marvell CF8385p ";
  
DutIf_Ndis::DutIf_Ndis(void)
{
 	char temp[_MAX_PATH]="";
  
 	if(getcwd(temp, _MAX_PATH) != NULL)  
 	strcat(temp, "\\setup.ini");
 	else
	strcpy(temp, "setup.ini");

 		
	debugLog	= GetPrivateProfileInt("DEBUG","debugLog", 
		0, temp); 

	vg_maxWait	= GetPrivateProfileInt("DutInitSet","maxWait",100, temp); 

	vg_dwSleep = GetPrivateProfileInt("DutInitSet","Delay",1, temp); 
    
 	vg_retry_temp = GetPrivateProfileInt("DutInitSet","retry",0, temp); 
  
  	GetPrivateProfileString("Driver","AdapterPrefix","Marvell",adapter_prefix, sizeof(adapter_prefix), temp); 
  
	vg_dwSpiSleep	= GetPrivateProfileInt("DutInitSet","SpiDelay", 0, temp); 
 	debugLog = GetPrivateProfileInt("DEBUG","debugLog", 0, temp); 

	vg_IfSpy = GetPrivateProfileInt("DEBUG","IfSpy", 0, temp); 


//	if(debugLog)
	{
		DebugLogRite( "debugLog %d\n", debugLog);
 		DebugLogRite("%s\n", temp);
		DebugLogRite( "adapter_prefix %s\n", adapter_prefix);
  		DebugLogRite( "Delay %X\n", vg_dwSleep);
		DebugLogRite( "maxWait %d\n", vg_maxWait);
		DebugLogRite( "Retry %X\n", vg_retry_temp);
		DebugLogRite( "SpiDelay %X\n", vg_dwSpiSleep);
		DebugLogRite( "IfSpy %d\n", vg_IfSpy);
	}
}

DutIf_Ndis::~DutIf_Ndis(void)
{
}

DWORD DutIf_Ndis::GetErrorLast(void)
{
	return W32N_GetLastError();
}


BOOL DutIf_Ndis::query( ULONG OID, PUCHAR pInfo, UINT len)
{
   W32N_REQUEST req;
	NDIS_STATUS status=NDIS_STATUS_SUCCESS;
int temp=0;

//  DebugLogRite("h 0x%X vg_IfSpy %d \n", h, vg_IfSpy);
	if (vg_IfSpy)
	{
		int index =0;
		DebugLogRite("Query:OID %08X\n", OID);

		for (index =0; index < len; index ++)
			DebugLogRite("%02X\t", *(pInfo+index));

		DebugLogRite("\n");
	}
   
   req.NdisRequest.RequestType = NdisRequestQueryInformation;
   req.NdisRequest.DATA.QUERY_INFORMATION.Oid = OID;
   req.NdisRequest.DATA.QUERY_INFORMATION.InformationBuffer = pInfo;
   req.NdisRequest.DATA.QUERY_INFORMATION.InformationBufferLength = len;
   req.NdisRequest.DATA.QUERY_INFORMATION.BytesWritten = 0;
   req.NdisRequest.DATA.QUERY_INFORMATION.BytesNeeded = 0;

   status = W32N_MakeNdisRequest(h, &req, &g_overlapped, TRUE); 
   if (status==NDIS_STATUS_SUCCESS)
   {   
//		DebugLogRite("q %d\n", vg_dwSleep);
 		Sleep(vg_dwSleep);


//		return (req.nRequestStatus == NDIS_STATUS_SUCCESS);
	if  (vg_IfSpy)
	{
		int index =0;
		DebugLogRite("Resps:OID %08X\n", OID);

		for (index =0; index < len; index ++)
			DebugLogRite("%02X\t", *(pInfo+index));
	
		DebugLogRite("\n");
	}

		return (req.nRequestStatus);
   }

	DebugLogRite("Failed to W32N_MakeNdisRequest RequestStatus:0x%08X  0x%08X\n", status, req.nRequestStatus);

	return status; 
//   return FALSE;
}
 
BOOL DutIf_Ndis::set( ULONG OID, PUCHAR pInfo, UINT len)
{
   W32N_REQUEST req;
	NDIS_STATUS status=NDIS_STATUS_SUCCESS;

	if (vg_IfSpy)
	{
		int index =0;
		DebugLogRite("Set:OID %08X\n", OID);

		for (index =0; index < len; index ++)
			DebugLogRite("%02X\t", *(pInfo+index));

		DebugLogRite("\n");
	}
	
	req.NdisRequest.RequestType = NdisRequestSetInformation;
   req.NdisRequest.DATA.SET_INFORMATION.Oid = OID;
   req.NdisRequest.DATA.SET_INFORMATION.InformationBuffer = pInfo;
   req.NdisRequest.DATA.SET_INFORMATION.InformationBufferLength = len;
   req.NdisRequest.DATA.SET_INFORMATION.BytesRead = 0;
   req.NdisRequest.DATA.SET_INFORMATION.BytesNeeded = 0;

   status = W32N_MakeNdisRequest(h, &req, &g_overlapped, TRUE); 
   if (status==NDIS_STATUS_SUCCESS)
   {   
		return (req.nRequestStatus);
   }


	DebugLogRite("Failed to W32N_MakeNdisRequest RequestStatus:0x%X  0x%X\n", status, req.nRequestStatus);

	return status; 
}
 
void DutIf_Ndis::init_dev_list(dev_t *dev)
{
   TCHAR *path;
   LONG  ret;
   HKEY  key;
   int   i;
   int   j;

   if (W32N_IsWindowsNT())
   {
      DWORD ver = W32N_OSGetPlatformVersion(VER_PLATFORM_WIN32_NT);

      if (ver < 0x0005)
         path = W32N_REGSTR_PATH_NETCARDS;
      else
         path = W32N_REGSTR_PATH_CLASS_NET_NT5;
   }
   else
      path = W32N_REGSTR_PATH_CLASS_NET;

   ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                      path,
                      0,                   // reserved options
                      KEY_READ,
                      &key);
   if (ret != ERROR_SUCCESS)
      return;

   // alloc
   for (i=0; i<NDEV; i++)
   {
      dev[i].AdapterDescription = (LPSTR)malloc(NDESCRIPTION * sizeof(TCHAR));
      dev[i].AdapterName = (LPSTR)malloc(NNAME * sizeof(TCHAR));

      // represent empty entry
      _tcscpy(dev[i].AdapterDescription, _T(""));
   }

   j = 0;
   for (i=0; j < NDEV && ret == ERROR_SUCCESS; i++)
   {
      TCHAR name[NDESCRIPTION];
      DWORD name_len;
      W32N_ADAPTER_INFO info;

      name_len = NDESCRIPTION;

      ret = RegEnumKeyEx(
                       key,
                       i,
                       name,
                       &name_len,
                       NULL,            // Reserved; must be NULL
                       NULL,            // class string of the enumerated subkey
                       NULL,            // NULL since the previous param is NULL
                       NULL);           // time last written
      // no more registry subkey to enumerate
      if (ret != ERROR_SUCCESS)
         break;

      ret = W32N_GetAdapterRegistryInfo(&info, name);

      if (ret == ERROR_SUCCESS)
      {
         if (_tcslen(info.cDescription) == 0)
            break;
         _tcsncpy(dev[j].AdapterDescription, info.cDescription,NDESCRIPTION);
         if (W32N_IsWindowsNT())
         {
            _tcscpy(dev[j].AdapterName, _T("\\Device\\"));
            _tcscat(dev[j].AdapterName, info.cServiceName);
         }
         else
            _tcscat(dev[j].AdapterName, name);
         j++;
      }
   }
   RegCloseKey(key);
}

 

HANDLE DutIf_Ndis::open_adapter(dev_t *dev)
{
	HANDLE h=0; 
   char     *AdapterName = NULL;
   DWORD    errorCode = 0;
   int i = 0;
   
   
	for (i=0; i<NDEV && dev[i].AdapterDescription; i++)
	{
		if (!strncmp(dev[i].AdapterDescription, 
            adapter_prefix, sizeof(adapter_prefix) - 1))
		{
			AdapterName = dev[i].AdapterName;
		 
			h = W32N_OpenAdapter(AdapterName);
 
			if (INVALID_HANDLE_VALUE != h )
			{
				DebugLogRite("Open %s\n", AdapterName);

				// Create The Global OVERLAPPED Event To Wait On
				//
				g_overlapped.hEvent = CreateEvent(
										NULL,      // Security Attributes
										FALSE,     // Auto-Reset
										FALSE,     // Initial State Signaled
										NULL       // Event-object Name
										);
				if (!g_overlapped.hEvent)
				{
					W32N_CloseAdapter(h);
					DebugLogRite("Failed to create event\n");
					return (h);

	//				exit (-1);
				}
				ResetEvent(g_overlapped.hEvent);

				return (h);
			}
			else
			{
				errorCode = W32N_GetLastError();
				DebugLogRite("Failed to open %s; error 0x%8.8X\n", AdapterName, errorCode);    
			}	
		}

	}

   if (i >= NDEV)
      DebugLogRite("WARNING: no %s adapter found\n", adapter_prefix);

   return (INVALID_HANDLE_VALUE);
}



int DutIf_Ndis::OpenDevice( )
{
	dev_t		dev[NDEV]={0};

	init_dev_list(dev);

//	a = select_default_adapter(dev);

	h=open_adapter(dev); //?

	if(INVALID_HANDLE_VALUE == h)
		return (ERROR_ERROR);

	return (ERROR_NONE); 
}

// Close The Adapter
int  DutIf_Ndis::CloseDevice( )
{
	int err=0; 

	if (h != INVALID_HANDLE_VALUE && h != NULL)
	{
		err = W32N_CloseAdapter(h);
		if(err) return err;
		h=NULL;

	}


   // Close The Global Event Handle
	if (g_overlapped.hEvent)
	{  
		err = CloseHandle(g_overlapped.hEvent);
		if(err) return err;
		g_overlapped.hEvent = NULL;
	}

	return err;
}

 

int DutIf_Ndis::bssid_list_scan()
{
   DWORD err;

   if (!query( OID_802_11_BSSID_LIST_SCAN, NULL, 0))
   {
      err = W32N_GetLastError();
      DebugLogRite("Failed query of OID_802_11_BSSID_LIST_SCAN; error 0x%x\n", err);
      return (ERROR_ERROR);
   }
   DebugLogRite("started BSSID list scan\n");
   return (ERROR_NONE);
}

#define SSID_SZ   32


//TODO from sta/src/hwi/usb/dsdef.h
#define EAGLE_SIZE_OF_CMD_BUFFER       0x5a0

// BSSID_LIST or BSSID_LIST_SCAN
int DutIf_Ndis::bssid_list( HANDLE h)
{
   unsigned char buf[EAGLE_SIZE_OF_CMD_BUFFER];   // 1072 1536
   DWORD err;
   PNDIS_802_11_BSSID_LIST pbl;
   PNDIS_WLAN_BSSID        pb;
   ULONG i;
   UCHAR ssidstr[SSID_SZ+1];

  
   if (!query( OID_802_11_BSSID_LIST, buf, EAGLE_SIZE_OF_CMD_BUFFER))
   {
      err = W32N_GetLastError();
      DebugLogRite("Failed query of OID_802_11_BSSID_LIST; error 0x%x\n", err);
      return (-1);
   }
   pbl = (PNDIS_802_11_BSSID_LIST)buf;
   pb  = pbl->Bssid;
   DebugLogRite("%2d networks:   SSID              Privacy RSSI NetworkType Mode\n", 
         pbl->NumberOfItems);
   for (i=0; i < pbl->NumberOfItems; i++)
   {
      ULONG len;

      len = min(pb->Ssid.SsidLength, SSID_SZ);
      memcpy(ssidstr, pb->Ssid.Ssid, len);
      ssidstr[len] = '\0';
      DebugLogRite("%32s    ", ssidstr);
      DebugLogRite("%d     ", pb->Privacy);
      DebugLogRite("%d     ", pb->Rssi);
      DebugLogRite("%s      ", pb->NetworkTypeInUse == Ndis802_11FH ? "FH" :
                                 pb->NetworkTypeInUse == Ndis802_11DS ? "DS" :
                                 "neither FH nor DS");
      DebugLogRite("%s\n", pb->InfrastructureMode == Ndis802_11IBSS ? "Ad-hoc" :
                          pb->InfrastructureMode == Ndis802_11Infrastructure ?
                          "Infrastructure" :
                          pb->InfrastructureMode == Ndis802_11AutoUnknown ?
                          "AutoUnknown" : "unknown");
      pb = (PNDIS_WLAN_BSSID)((UCHAR *)pb + pb->Length);
   }

   return (ERROR_NONE); 
}


int DutIf_Ndis::disassocate( HANDLE h)
{
	return !query( OID_802_11_DISASSOCIATE, NULL, 0);	
}

int	DutIf_Ndis::assocate( HANDLE h, char* szSSID, DWORD AuthenModeEnable)//  DWORD WepModeEnable) //fixed 
{
	int status=0;

//	RECT rtSize;
 	char  *szMsg;
	DWORD dwRegKey = FALSE;
 	PNDIS_802_11_WEP pWep;
	char  * szKeyVal, *szRegKey;
	ULONG   nTxKey;
	USHORT  nKeySize;
 

 	//=======================================================
	// The section of code above to be removed in the future
	//=======================================================
	
	NDIS_802_11_SSID  Ssid;

	Ssid.SsidLength = strlen(szSSID);
	_tcscpy((char*)Ssid.Ssid, (LPCTSTR)szSSID);
 
 	//====================================
	//   send set SSID command
	//====================================
	return !query( OID_802_11_SSID, (LPBYTE)&Ssid, sizeof (NDIS_802_11_SSID));
 
} 
 
#endif
