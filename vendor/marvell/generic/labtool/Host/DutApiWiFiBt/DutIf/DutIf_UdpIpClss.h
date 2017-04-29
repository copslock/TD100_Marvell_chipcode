/** @file DutIf_UdpIpClss.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

//;#include <winsock2.h>
#include <winsock.h>
#ifndef _DUTIF_UDP_CLSS_
#define _DUTIF_UDP_CLSS_

 
//#define HOSTNAME "localhost"//IMPORTANT: SET ADDRESS OF CLIENT HERE!!
#define PORT_SERVER_RD	9930 // Server listen to it. Me write to it
#define PORT_SERVER_WT	9931 // Server write to it. Me listen to it

//;#include DUTHCI_BT_LL_OPCODE_H_PATH		//	"../DutBtApi878XDll/hci_opcodes.h"
//;#include DUTHCI_BT_LL_STRUCT_H_PATH


class DutIf_UdpIp
{
int warningLog; 

 
SOCKET hSock_Cl_Listen; // Socket client listen
SOCKET hSock_Cl_Write; // Socket client write

WSADATA WSAData;                    // Contains details of the 
void StopSocketHard();
void HandleSvrSocketError();

protected:
SOCKADDR_IN destination_sin;        // Server socket address
SOCKADDR_IN host_sin;				// Client socket address
unsigned long delay;
unsigned long vg_IfSpy;
int debugLog;
int DebugLog; 
int timeOut;
int vg_dwSpiSleep;

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

int SocketOpen(int UDPPortNum=9930);
int SocketClose();
int SocketSendTo(char * msg, int len);//, const struct sockaddr *RxrAddress);
int SocketRcvFrom(char *msg, int len, double timeout);// ,struct sockaddr  *);
public: 

DutIf_UdpIp();
~DutIf_UdpIp();

int	OpenDevice(HANDLE *h);
int	CloseDevice(HANDLE *h);

//;BOOL	query(HANDLE h, ULONG OID, PUCHAR pInfo, UINT len);
BOOL	query( ULONG OID, PUCHAR pInfo, UINT len);
BOOL set(ULONG OID, PUCHAR pInfo, UINT len);
BOOL queryHci(ULONG OID, PUCHAR Buf, UINT BufSize, 
							 UINT ExpectedEvent=0, 
							 UINT *BufRetSize=NULL
							 );
void SetDelayTime(unsigned long delayTime); 
void GetDelayTime(unsigned long *delayTime); 

void SetHostAddress(char *address, int port=PORT_SERVER_WT);
void SetPartnerAddress(char *address, int port=PORT_SERVER_RD);
void SetWarningLog(int setting); 
BOOL GetDebugLogFlag(void);
DWORD	GetErrorLast(void);  





};

#endif
