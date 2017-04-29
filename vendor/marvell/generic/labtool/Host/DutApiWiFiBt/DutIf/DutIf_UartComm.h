/** @file DutIf_UartComm.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DUTIF_UARTCOMM_H__5137B7F2_60C5_46A9_AD6D_4BBA9C0D8C11__INCLUDED_)
#define AFX_DUTIF_UARTCOMM_H__5137B7F2_60C5_46A9_AD6D_4BBA9C0D8C11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SerialCommHelper.h"
#include <direct.h>

class DutIf_UartComm  
{
private:
 HANDLE h;

 int vg_IfSpy;
 int vg_dwSleep;
 int vg_maxWait;
 int vg_retry_temp;
 int vg_dwSpiSleep;
 int vg_checkFW;
 int vg_time2wait;
 int vg_uartdelay;
	CSerialCommHelper m_theCommPort;

protected:
 int DebugLog;
//	int debugLog;

	int timeOut;
	
///////////////////////////////////////	
// NEW 
BOOL BT_Send(PUCHAR pInfo, UINT len);
BOOL BT_Receive(PUCHAR pInfo, UINT *pLen);  
int RxHciDataEvent(PUCHAR pEvent, ULONG BufLength);

int	 WaitForEvent(USHORT CmdGroup, USHORT CmdCode, USHORT TargetEvent, 
				  UCHAR *pEvent, ULONG BufSize);
BOOL SendCommandToController(UCHAR CmdGroup, 
							 USHORT CmdCode, UCHAR *pParam, USHORT ParamLength);

BOOL SendHciCommand(USHORT CmdGroup, USHORT CmdCode, USHORT ExpEvent, 
							   PUCHAR pParam, USHORT ParamLength, 
								PUSHORT pFoundEvent,
							   PUCHAR pReturn, USHORT &RetBufSize);							 

///////////////////////////////////////	
	
	
	
	
	
	
	
public: 
	DutIf_UartComm();
	~DutIf_UartComm();

int	OpenDevice( );
int	CloseDevice( );

BOOL query( ULONG OID, PUCHAR pInfo, UINT len);

BOOL Bt_query(ULONG OID, PUCHAR Buf, UINT BufSize);
BOOL queryHci(ULONG OID, PUCHAR Buf, UINT BufSize, 
						UINT ExpectedEvent,	  
						UINT *BufRetSize);
BOOL set( ULONG OID, PUCHAR pInfo, UINT len);

DWORD	GetErrorLast(void);  
BOOL GetDebugLogFlag(void);

};

#endif // !defined(AFX_DUTIF_UARTCOMM_H__5137B7F2_60C5_46A9_AD6D_4BBA9C0D8C11__INCLUDED_)
