/** @file DutBtIf_IfClss.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#ifndef _DUTIF_BT_IF_CLSS_
#define _DUTIF_BT_IF_CLSS_

#include "../../DutCommon/Clss_os.h"



#ifndef _NO_CLASS_
#if defined (_BT_UART_)
	#include "../DutIf/DutBtIf_HciUart.h"
	
	class DutBtIf_If: public DutBtIf_HciUart //protected
#elif defined(_BT_USB_) 
	#include "../DutIf/DutIf_DvcIoCtrl.h" 
	class DutBtIf_If: public DutIf_DvcIoCtrl
	
#elif defined(_NDIS_) 
	#include "../DutIf/DutIf_Ndis.h" 
	class DutBtIf_If: public DutIf_Ndis
#elif defined(_BT_SDIO_) 
#include "../DutIf/DutIf_SdioAdapter.h"
	class DutBtIf_If: public DutIf_SdioAdpt
#elif defined(_BT_UDP_)
#include "../DutIf/DutIf_UdpIpClss.h" 
	class DutBtIf_If: public DutIf_UdpIp

#elif defined(_BT_REMOTE_UART_)
#include "../DutIf/DutIf_UartComm.h" 
	class DutBtIf_If: public DutIf_UartComm
#endif	//#if defined (_BT_UART_)
{

protected:
 
public: 
 
DutBtIf_If(); 
~DutBtIf_If();
#else //_NO_CLASS_
#ifdef _HCI_UART_
	#include "DutBtIf_HciUart.h"
#elif defined(_BT_USB_) 
	#include "../DutIf/DutIf_DvcIoCtrl.h"  
#elif defined(_NDIS_) 
	#include "../DutIf/DutIf_Ndis.h"
#elif defined(_BT_SDIO_) 
	#include "../DutIf/DutIf_SdioAdapter.h"
#endif	//#ifdef _HCI_UART_
#endif //_NO_CLASS_

int		BtIf_GetDebugLogFlag(void);
void	BtIf_SetDebugLogFlag(BOOL Flag_Log);
DWORD	BtIf_GetErrorLast(void);  

BOOL BtIf_query( ULONG OID, PUCHAR pInfo, UINT len);
BOOL BtIf_query1(ULONG OID, PUCHAR pInfo, UINT len, UINT *retBufSize);
BOOL BtIf_set( ULONG OID, PUCHAR pInfo, UINT len);



int	BtIf_OpenDevice(bool Gru);
int	BtIf_CloseDevice( );


#ifndef _NO_CLASS_

};
#endif	//#ifndef _NO_CLASS_
#endif

