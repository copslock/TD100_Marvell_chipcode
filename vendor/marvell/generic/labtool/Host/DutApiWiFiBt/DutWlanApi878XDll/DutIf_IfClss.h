/** @file DutIf_IfClss.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#ifndef _DUTIF_IF_CLSS_
#define _DUTIF_IF_CLSS_

#include "../../DutCommon/Clss_os.h"

#ifndef _LINUX_
//#define DutWlanApiClssHeader DutWlanApiClss::

//#include "stdAfx.h"
#ifdef _IF_UDP_
	#include "../DutIf/DutIf_UdpIpClss.h" 
	class DutIf_If: protected DutIf_UdpIp
#endif 
#ifdef _IF_SDIO_
#include "../DutIf/DutIf_SdioAdapter.h"  
 	class DutIf_If:protected DutIf_SdioAdpt

#endif 
#ifdef _IF_USB_
	#include "../DutIf/DutIf_DvcIoCtrl.h" 
	class DutIf_If: protected DutIf_DvcIoCtrl
#endif 
#ifdef _NDIS_
	#include "../DutIf/DutIf_Ndis.h" 
	class DutIf_If: protected DutIf_Ndis
#endif	//#ifdef _NDIS_
#ifdef _IF_UART_
	#include "../DutIf/DutIf_UartComm.h" 
	class DutIf_If: protected DutIf_UartComm
#endif 
{

protected:
 
public: 
 
DutIf_If();
~DutIf_If();

#else	//#ifndef _LINUX_
//	#include "../DutIf/DutIf_UdpIpClss.h" 

#endif	//#ifndef _LINUX_

DWORD	WifiIf_GetErrorLast(void);  

BOOL WifiIf_query( ULONG OID, PUCHAR pInfo, UINT len);
BOOL WifiIf_set( ULONG OID, PUCHAR pInfo, UINT len);

int	WifiIf_OpenDevice(bool Gru);
int	WifiIf_CloseDevice( );

ULONG WifiIf_GetDebugLogFlag();
void  WifiIf_SetDebugLogFlag(BOOL Flag_Log);

#ifndef _LINUX_

};
#endif	//#ifndef _LINUX_
#endif

