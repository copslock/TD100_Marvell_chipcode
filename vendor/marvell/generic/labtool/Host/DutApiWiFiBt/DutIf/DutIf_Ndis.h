/** @file DutIf_Ndis.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#ifndef _DUTIF_NDIS_CLSS_
#define _DUTIF_NDIS_CLSS_
#include "DutNdis.h"

class DutIf_Ndis 
{
 HANDLE h;

 int vg_IfSpy;
 int vg_dwSleep;
 int vg_maxWait;
 int vg_retry_temp;
 int vg_dwSpiSleep;

 HANDLE  open_adapter(dev_t *dev);
 void  init_dev_list(dev_t *dev);

protected:
	int debugLog; 
	int timeOut;
public: 
 
DutIf_Ndis();
~DutIf_Ndis();
 
DWORD	GetErrorLast(void);  

BOOL query( ULONG OID, PUCHAR pInfo, UINT len);
BOOL set( ULONG OID, PUCHAR pInfo, UINT len);

int	OpenDevice( );
int	CloseDevice( );

int bssid_list( HANDLE h);
int bssid_list_scan();
int	assocate( HANDLE h, char* szSSID, DWORD AuthenModeEnable);  
int disassocate( HANDLE h);



};

#endif




 
