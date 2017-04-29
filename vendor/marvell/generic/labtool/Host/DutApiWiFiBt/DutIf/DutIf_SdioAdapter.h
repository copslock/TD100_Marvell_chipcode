/** @file DutIf_SdioAdapter.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#ifndef _DUTIF_SDIOADPT_CLSS_
#define _DUTIF_SDIOADPT_CLSS_ 


#include "../../DutCommon/utilities.h" 

class DutIf_SdioAdpt 
{
 HANDLE h;

 int vg_IfSpy;
 int vg_maxWait;
 bool	instanceExist;

BOOL ToasterDeviceInfo();
 
protected:
	int DebugLog; 
	int timeOut;
public: 
 
DutIf_SdioAdpt();
~DutIf_SdioAdpt();
 
DWORD	GetErrorLast(void);  

BOOL query( ULONG OID, PUCHAR pInfo, UINT len);
BOOL set( ULONG OID, PUCHAR pInfo, UINT len);
BOOL queryHci(ULONG OID, PUCHAR Buf, UINT BufSize, 
							 UINT ExpectedEvent=0, 
							 UINT *BufRetSize=NULL
							 );

int	OpenDevice( );
int	CloseDevice( );

ULONG GetDebugLogFlag();
void  SetDebugLogFlag(BOOL Flag_Log);

void GetTimeOut(int *pTimeOut);
void SetTimeOut(int TimeOut);
void ResetTimeOut(void);
void GetDataTimeOut(int *pTimeOut);
void SetDataTimeOut(int TimeOut);
void ResetDataTimeOut(void);


BOOL SendAclData(PUCHAR pData, ULONG DataLength);
BOOL RxData(PUCHAR pData,	   ULONG BufLength);


};

#endif
