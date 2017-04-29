/** @file DutIf_Msproio.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include "stdafx.h" 
#ifndef _UDP_ 
#ifndef _NDIS_CLSS_CPP_
#define _NDIS_CLSS_CPP_
 
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>

#include "DutIf_IfClss.h"
#include "utilities.h"
 
#include "../DutApi.hc"
 
#include "MsProIo.h" 
 
// prefix of adapter description
//static char adapter_prefix[256] = "Marvell CF8385p ";
  
DutIf_MsProIo::DutIf_MsProIo(void)
{
 	char temp[_MAX_PATH]="";

	lastErr =0;

	if(GetCurrentDirectory(_MAX_PATH, temp) != 0)  
	strcat(temp, "\\setup.ini");
	else
	strcpy(temp, "setup.ini");
  
// 	GetPrivateProfileString("Driver","AdapterPrefix","Marvell",adapter_prefix, sizeof(adapter_prefix), temp); 
  
  	debugLog = GetPrivateProfileInt("DEBUG","debugLog", 0, temp); 

	vg_IfSpy = GetPrivateProfileInt("DEBUG","ifSpy", 0, temp); 


	if(debugLog)
	{
		DebugLogRite( "debugLog %d\n", debugLog);
// 		DebugLogRite( "adapter_prefix %s\n", adapter_prefix);
  		DebugLogRite( "IfSpy %d\n", vg_IfSpy);
	}
}

DutIf_MsProIo::~DutIf_MsProIo(void)
{
}

DWORD DutIf_MsProIo::GetErrorLast(void)
{
	return lastErr;
}


BOOL DutIf_MsProIo::query( ULONG Port, PUCHAR pInfo, UINT len)
{
	if (vg_IfSpy)
	{
		int index =0;
		DebugLogRite("Query:Port %08X\n", Port);

		for (index =0; index < (signed)len; index ++)
			DebugLogRite("%02X\t", *(pInfo+index));

		DebugLogRite("\n");
	}
   

    lastErr =  Query( Port, pInfo, len);
      
	if (vg_IfSpy)
	{
		int index =0;
		DebugLogRite("Query:Port %08X\n", Port);

		for (index =0; index < (signed)len; index ++)
			DebugLogRite("%02X\t", *(pInfo+index));

		DebugLogRite("\n");
	}
   
	return lastErr; 
}
 
BOOL DutIf_MsProIo::set( ULONG Port, PUCHAR pInfo, UINT len)
{
    lastErr =  Query( Port, pInfo, len);

	return lastErr; 
}
 
int DutIf_MsProIo::OpenDevice()
{ 
	lastErr = DeviceInit(&RamTestResult);		
	return lastErr; 

}

// Close The Adapter
int  DutIf_MsProIo::CloseDevice( )
{
  	lastErr = DeviceClose();
	return lastErr; 
}

  
void DutIf_MsProIo::SetTimeout(int timeOut )
{
 	setTimeout(timeOut);
		
}

void DutIf_MsProIo::GetTimeout(int *timeOut )
{
 	 getTimeout(timeOut);
		
}

void DutIf_MsProIo::GetRamTestResult(int *pRamTestResult)
{ 
	*pRamTestResult=RamTestResult;

}

#endif


#endif
