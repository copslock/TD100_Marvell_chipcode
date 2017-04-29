/** @file DutIf_Msproio.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#ifndef _DUTIF_NDIS_CLSS_
#define _DUTIF_NDIS_CLSS_
#include "MsProIo.h"

class DutIf_MsProIo
{
 int lastErr;
 int vg_IfSpy;
// int vg_dwSleep;
// int vg_maxWait;
// int vg_retry_temp;
// int vg_dwSpiSleep;

// HANDLE  open_adapter(dev_t *dev);
// void  init_dev_list(dev_t *dev);

protected:
	int RamTestResult;
	int debugLog; 
	int timeOut;
public: 
 
DutIf_MsProIo();
~DutIf_MsProIo();
 
DWORD	GetErrorLast(void);  

BOOL query( ULONG Port, PUCHAR pInfo, UINT len);
BOOL set( ULONG Port, PUCHAR pInfo, UINT len);

int	OpenDevice( );
 
int	CloseDevice( );

void SetTimeout(int timeOut );
void GetTimeout(int *timeOut );

void GetRamTestResult(int *pRamTestResult);
  
};

#endif




 
