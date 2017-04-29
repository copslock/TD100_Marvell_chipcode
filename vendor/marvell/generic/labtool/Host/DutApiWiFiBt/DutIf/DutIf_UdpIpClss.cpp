/** @file DutIf_UdpIpClss.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

//;#include "stdAfx.h"
#ifdef _IF_UDP_
#ifndef _UDPIP_CLSS_CPP_
#define _UDPIP_CLSS_CPP_

#include "DutIf_UdpIpClss.h"
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 
#include "..\..\DutCommon\utilities.h"
#include "..\..\DutCommon\utility.h"
#include "..\DutBtApi878XDll\HciStruct.h"
#include "..\DutBtApi878XDll\hci_opcodes.h"
#include "..\DutBtApi878XDll\UartErrCode.h"


typedef struct Bridge_Command_Format_Header
{
	WORD    Type;
	WORD	SubType;
	WORD	Length;
	WORD	Status;
	DWORD	Reserved;
}Bridge_Command_Format_Header;


BOOL FWDOWNLOAD=1;
BOOL Protocol=0;
#define MAXSPIPAGE_BYTE 256
#define UDP 0
#define TCP 1
// #define TCPIF Protocol== TCP 
int SpyLogRite(const char *string, ... )
{
   
    va_list   va;
	int err=0;
	FILE *hFile=0; 

	hFile = fopen("IfSpy.txt","a+"); 

	va_start(va, string);
    err = vprintf(string, va);
	vfprintf(hFile, string, va);
    va_end(va);

	fclose(hFile); 

	return err;
}



 
DutIf_UdpIp::DutIf_UdpIp()
{

	
	char temp[_MAX_PATH]="";
	char DutAP_IP[255]="192.168.0.10:9930", DutHost_IP[255]="192.168.0.100:9930"; 
	int DutAP_IpPort=9930, DutHost_IpPort = 9931; 
 	char temp1[255]="", temp2[255]="";  
	int cnt=0;
	char ProtocolUsed[4];

	
	if(getcwd(temp, _MAX_PATH) != NULL)  
	strcat(temp, "\\setup.ini");
	else
	strcpy(temp, "setup.ini");


	if(getcwd(temp, _MAX_PATH) != NULL)  
	strcat(temp, "\\setup.ini");
	else
	strcpy(temp, "setup.ini");

	vg_dwSpiSleep =0;

	timeOut = GetPrivateProfileInt("DutInitSet","TimeOut",1000, temp); 

 	DebugLog	= GetPrivateProfileInt("DEBUG","debugLog", 
		0, temp); 

#ifndef _Golden 
	cnt = GetPrivateProfileString("DutIp","DutIpAddress",
			"192.168.0.11:9930", temp1, 255, temp); 
#else
	cnt = GetPrivateProfileString("GoldenIp","GoldenIpAddress",
			"192.168.0.11:9930", temp1, 255, temp); 
#endif
 	cnt = sscanf(temp1, "%[^:]:%d", DutAP_IP, &DutAP_IpPort);

#ifndef _Golden
	cnt = GetPrivateProfileString("DutIp","HostIpAddress",
			"192.168.0.100:9931", temp2, 255, temp); 
#else
	cnt = GetPrivateProfileString("GoldenIp","HostIpAddress",
			"192.168.0.100:9931", temp2, 255, temp); 
#endif


	cnt = sscanf(temp2, "%[^:]:%d", DutHost_IP, &DutHost_IpPort);
//Check UDP or TCP
	cnt = GetPrivateProfileString("DutIp", "Protocol", "UDP", temp2, 255, temp);
    if(!_strnicmp(temp2,"TCP", 3))
	{
		Protocol=TCP;
	}
	else
		Protocol=UDP;

	delay = GetPrivateProfileInt("DutInitSet","DelayTime",0, temp); 


	vg_IfSpy = GetPrivateProfileInt("DEBUG","IfSpy",0, temp); 

	FWDOWNLOAD = GetPrivateProfileInt("DEBUG","FWDOWNLOAD",1, temp); 




	if(warningLog) DebugLogRite("%s\n", temp);
#ifndef _Golden
  	DebugLogRite("Dut's IP %s:%d\n", DutAP_IP, DutAP_IpPort);
#else
	DebugLogRite("Golden's IP %s:%d\n", DutAP_IP, DutAP_IpPort);
#endif
  	DebugLogRite("Host's IP %s:%d\n", DutHost_IP, DutHost_IpPort);
	if(warningLog) DebugLogRite( "DutIf_UdpIp::delay %d\n", delay);
	if(warningLog) DebugLogRite( "DutIf_UdpIp::vg_IfSpy %d\n", vg_IfSpy);
 	
	hSock_Cl_Listen = INVALID_SOCKET;
	hSock_Cl_Write = INVALID_SOCKET; // Socket bound to the client (broadcast)
//	destination_sin;        // Server socket address

 
// Assign the socket IP address.

// Convert to network ordering.

    destination_sin.sin_family = AF_INET; 
	destination_sin.sin_port = 0; 
	destination_sin.sin_addr.s_addr = htonl(INADDR_ANY); 
	destination_sin.sin_zero[0]= 0; 
	destination_sin.sin_zero[1]= 0; 
	destination_sin.sin_zero[2]= 0; 
	destination_sin.sin_zero[3]= 0; 
	destination_sin.sin_zero[4]= 0; 
	destination_sin.sin_zero[5]= 0; 
	destination_sin.sin_zero[6]= 0; 
	destination_sin.sin_zero[7]= 0; 

	host_sin.sin_family = AF_INET; 
	host_sin.sin_port = 0; 
	host_sin.sin_addr.s_addr = htonl(INADDR_ANY); 
	host_sin.sin_zero[0]= 0; 
	host_sin.sin_zero[1]= 0; 
	host_sin.sin_zero[2]= 0; 
	host_sin.sin_zero[3]= 0; 
	host_sin.sin_zero[4]= 0; 
	host_sin.sin_zero[5]= 0; 
	host_sin.sin_zero[6]= 0; 
	host_sin.sin_zero[7]= 0; 
  

 SetPartnerAddress(DutAP_IP, DutAP_IpPort);

 SetHostAddress(DutHost_IP, DutHost_IpPort);



}

DutIf_UdpIp::~DutIf_UdpIp()
{

}




void DutIf_UdpIp::SetPartnerAddress(char *address, int port)
{
// Assign the socket IP address. 
	destination_sin.sin_addr.s_addr =inet_addr(address); 
 // Convert to network ordering.
    destination_sin.sin_port = htons(port);      

}

void DutIf_UdpIp::SetHostAddress(char *address, int port)
{
  	int status =0; 

    host_sin.sin_family = AF_INET; 
    host_sin.sin_addr.s_addr =  inet_addr(address);    
//	host_sin.sin_addr.s_addr = htonl(INADDR_ANY);
// Convert to network ordering.
    host_sin.sin_port =  htons(port);        
  
}

int DutIf_UdpIp::SocketOpen(int UDPPortNum)
{
int status=0;

//;	if(UDPPortNum>=9930)
//;	{
//;		destination_sin.sin_port =  htons(UDPPortNum);
//;		host_sin.sin_port =  htons(UDPPortNum+1);
//;		DebugLogRite("Dut's Port: %d\n", UDPPortNum);
//;  		DebugLogRite("Host's Port: %d\n", UDPPortNum+1);
//;
//;	}
    status = WSAStartup (MAKEWORD(2,2), &WSAData);
	if(status != 0) 
    {
        HandleSvrSocketError();
        return status;
    }
//    DebugLogRite("Socket startup\n");
if(Protocol==TCP)
{
	 if ((hSock_Cl_Listen = socket (AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
        HandleSvrSocketError();
        return INVALID_SOCKET;
    }

	DebugLogRite("TCP connecting...\n");
	 status = connect (hSock_Cl_Listen, 
		 (const struct sockaddr*)&destination_sin, 
//;		(const struct sockaddr*)&destination_sin, 
  		sizeof(destination_sin));
	 if(status == -1)
	{     
		HandleSvrSocketError();
		if(warningLog) DebugLogRite("Sockets binding failed\n");
		DebugLogRite("TCP connecting fail...\n");
	}

}
else
{
// Create a Raw socket that is bound to the server.
    if ((hSock_Cl_Listen = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET){
        HandleSvrSocketError();
        return INVALID_SOCKET;
    }

   if ((hSock_Cl_Write = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
    {
        HandleSvrSocketError();
        return INVALID_SOCKET;
    }

//setsockopt
	status = bind(hSock_Cl_Listen, 
//		(const struct sockaddr*)&destination_sin, 
		(const struct sockaddr*)&host_sin, 

  		sizeof(host_sin));

	if(status == -1)
	{     
		HandleSvrSocketError();
		if(warningLog) DebugLogRite("Sockets binding failed\n");
	}
}
//Load Driver
#ifdef _LOAD_DRIVER_
	{
	Bridge_Command_Format_Header HearderBuf = {0};
	char *TxBuf=NULL, *ptr=NULL,pInfo[10]={0};
	int TotalLength=0, len =0;
	len = sizeof(pInfo);
    TotalLength = len+ sizeof(Bridge_Command_Format_Header);
    TxBuf = (char *)malloc (len+ sizeof(Bridge_Command_Format_Header));
	memset(TxBuf, 0, TotalLength);
	HearderBuf.Type = 0x0001;
	HearderBuf.SubType =0x0001;
	HearderBuf.Length = len+ sizeof(Bridge_Command_Format_Header);
	memcpy(TxBuf, &HearderBuf, sizeof(Bridge_Command_Format_Header));
	memcpy(TxBuf+(sizeof(Bridge_Command_Format_Header)/sizeof(char)), pInfo, len);
	SocketSendTo((char*) TxBuf, TotalLength);
	}
#endif //#ifdef _LOAD_DRIVER_
//     DebugLogRite("Sockets opened\n");
	//CRH to start AP82 to MFG mode
/*	
	if(FWDOWNLOAD)
	{
		char msg[]="MARVELLAP82S";
		status = SocketSendTo(msg, sizeof(msg));
	}
	else
	{
		if (debugLog)DebugLogRite("MFG FW not load!\n");
	}
*/
      
	return status;
}


int DutIf_UdpIp::SocketClose()
{
//Unload driver
#ifdef _LOAD_DRIVER_
	{
	Bridge_Command_Format_Header HearderBuf = {0};
	char *TxBuf=NULL, *ptr=NULL,pInfo[10]={0};
	int TotalLength=0, len =0;
	len = sizeof(pInfo);
    TotalLength = len+ sizeof(Bridge_Command_Format_Header);
    TxBuf = (int *)malloc (len+ sizeof(Bridge_Command_Format_Header));
	memset(TxBuf, 0, TotalLength);
	HearderBuf.Type = 0x0001;
	HearderBuf.SubType =0x0002;
	HearderBuf.Length = len+ sizeof(Bridge_Command_Format_Header);
	memcpy(TxBuf, &HearderBuf, sizeof(Bridge_Command_Format_Header));
	memcpy(TxBuf+(sizeof(Bridge_Command_Format_Header)/sizeof(char)), pInfo, len);
	SocketSendTo((char*) TxBuf, TotalLength);
	}
#endif //#ifdef _LOAD_DRIVER_
// Close the socket.
    if (closesocket (hSock_Cl_Listen) == SOCKET_ERROR)
    {
        HandleSvrSocketError();
        return SOCKET_ERROR;
    }

	if(Protocol!=TCP)
	{
	// Clean up socket
		 if (closesocket (hSock_Cl_Write) == SOCKET_ERROR)
		{
			HandleSvrSocketError();
			return SOCKET_ERROR;
		}
	}
// Clean up socket
   return WSACleanup ();
 
}


int DutIf_UdpIp::SocketSendTo(char * msg, int len)//, const struct sockaddr *RxrAddress)
{

		int status =0;
	unsigned long lenthIn=0;
 	char *ptempString=NULL;
	struct sockaddr_in sa_temp={0};
	int tempInt=0; 

	sa_temp.sin_family = AF_INET;
//	sa_temp.sin_addr.s_addr = htonl(INADDR_ANY);
//    sa_temp.sa_data =  destination_sin.sin_addr.s_addr;      


	// clean Rx buff  

	ioctlsocket (hSock_Cl_Listen, FIONREAD, &lenthIn);
	if( 0< lenthIn)
	{ 
		ptempString = (char*)malloc(lenthIn*sizeof(char));
		tempInt = lenthIn; 
		status = recvfrom (hSock_Cl_Listen, 
				ptempString, lenthIn, 0, 
				(sockaddr*)&sa_temp,  &tempInt);
		if(status == SOCKET_ERROR) 
		{
			HandleSvrSocketError();			
			free(ptempString);
				return status;
		}
		free(ptempString);
	}

	if(vg_IfSpy)	
	{
		int	index =0;
		SpyLogRite("Sendto: %X\n", destination_sin.sin_addr);
		for (index =0; index < len; index ++)
		SpyLogRite("0x%02X ",  0xFF&msg[index]); 
		SpyLogRite("\n");
	}

	if(Protocol==TCP)
		status = sendto (hSock_Cl_Listen, msg, len, 0, 
			(const struct sockaddr *)&destination_sin, sizeof(destination_sin));
	else 
		status = sendto (hSock_Cl_Write, msg, len, 0, 
			(const struct sockaddr *)&destination_sin, sizeof(destination_sin));
	if(status == SOCKET_ERROR) 
    {
        HandleSvrSocketError();
        return status;
    }

	return 0;
}

 
int DutIf_UdpIp::SocketRcvFrom(char * msg, 
											   int len, double timeout)
{
	int status =0;
	unsigned long lenthIn=0;
	time_t startTime={0}, nowTime={0};
	double timeElaped=0; 
	char *ptempString=NULL;
	struct sockaddr_in sa_temp={0};
	int tempInt=0; 

	sa_temp.sin_family = AF_INET;
//	sa_temp.sin_addr.s_addr = htonl(INADDR_ANY);
//    sa_temp.sa_data =  destination_sin.sin_addr.s_addr;      

 	time (&startTime);
	strcpy(msg,"");

	do
	{	
		time (&nowTime); 
		timeElaped =difftime(nowTime, startTime);

		ioctlsocket (hSock_Cl_Listen, FIONREAD, &lenthIn);
		if( 0< lenthIn)
		{ 
			ptempString = (char*)malloc((lenthIn)*sizeof(char));
			memset(ptempString, 0, (lenthIn)*sizeof(char));

			tempInt = lenthIn; 
			status = recvfrom (hSock_Cl_Listen, 
				ptempString, lenthIn, 0, 
				(sockaddr*)&sa_temp,  &tempInt);
			if(status == SOCKET_ERROR) 
			{
				HandleSvrSocketError();			
				free(ptempString);

				return status;
			}
			memcpy(msg, ptempString,lenthIn);
			free(ptempString);
			break;
		}

	}while (lenthIn == 0 && (timeout) > timeElaped);	
	if(timeout <= timeElaped) 
	if(warningLog) DebugLogRite("TIMEOUT ( %f[%f])\n", timeout, timeElaped); 
	else
	{	
		if(vg_IfSpy)	
		{
			int	index =0;
			SpyLogRite("RecvFrom: %X\n", destination_sin.sin_addr);
			for (index =0; index < len; index ++)
			SpyLogRite("0x%02X ",  0xFF&msg[index]); 
			SpyLogRite("\n");
		}
	
	}
	return !lenthIn;

}

void DutIf_UdpIp::StopSocketHard()
{
	if( closesocket(hSock_Cl_Listen) == SOCKET_ERROR )
	{
		if(warningLog) DebugLogRite("Error closing socket in StopSocketHard()");
	}
	
	if( closesocket(hSock_Cl_Write) == SOCKET_ERROR )
	{
		if(warningLog) DebugLogRite("Error closing socket in StopSocketHard()");
	}

	WSACleanup();
}

void DutIf_UdpIp::HandleSvrSocketError()
{
    int errval;
	errval=WSAGetLastError();

	switch(errval){
		case WSAEACCES : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEACCES");
			StopSocketHard(); 
			break; 
		case WSAEADDRINUSE : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEADDRINUSE");
			StopSocketHard();
			break; 
		case WSAEADDRNOTAVAIL : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEADDRNOTAVAIL");
			StopSocketHard();
			break; 
		case WSAEAFNOSUPPORT : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEAFNOSUPPORT");
			StopSocketHard();
			break; 
		case WSAEALREADY : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEALREADY");
			StopSocketHard();
			break; 
 		case WSAECONNABORTED : 
			if(warningLog) DebugLogRite("General Socket Error:WSAECONNABORTED");
			StopSocketHard();
			break; 
		case WSAECONNREFUSED : 
			if(warningLog) DebugLogRite("General Socket Error:WSAECONNREFUSED");
			StopSocketHard();
			break; 
		case WSAECONNRESET : 
			if(warningLog) DebugLogRite("General Socket Error:WSAECONNRESET");
			StopSocketHard();
			break; 
		case WSAEDESTADDRREQ : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEDESTADDRREQ");
			StopSocketHard();
			break; 
		case WSAEFAULT : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEFAULT");
			StopSocketHard();
			break; 
		case WSAEHOSTDOWN : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEHOSTDOWN");
			StopSocketHard();
			break; 
		case WSAEHOSTUNREACH : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEHOSTUNREACH");
			StopSocketHard();
			break; 
		case WSAEINPROGRESS : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEINPROGRESS");
			StopSocketHard();
			break; 
		case WSAEINTR : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEINTR");
			StopSocketHard();
			break; 
		case WSAEINVAL : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEINVAL");
			StopSocketHard();
			break; 
		case WSAEISCONN : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEISCONN");
			StopSocketHard();
			break; 
		case WSAEMFILE : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEMFILE");
			StopSocketHard();
			break; 
		case WSAEMSGSIZE : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEMSGSIZE");
			StopSocketHard();
			break; 
		case WSAENETDOWN : 
			if(warningLog) DebugLogRite("General Socket Error:WSAENETDOWN");
			StopSocketHard();
			break; 
		case WSAENETRESET : 
			if(warningLog) DebugLogRite("General Socket Error:WSAENETRESET");
			StopSocketHard();
			break; 
		case WSAENETUNREACH : 
			if(warningLog) DebugLogRite("General Socket Error:WSAENETUNREACH");
			StopSocketHard();
		case WSAENOBUFS : 
			if(warningLog) DebugLogRite("General Socket Error:WSAENOBUFS");
			StopSocketHard();
			break; 
		case WSAENOPROTOOPT : 
			if(warningLog) DebugLogRite("General Socket Error:WSAENOPROTOOPT");
			StopSocketHard();
			break; 
		case WSAENOTCONN : 
			if(warningLog) DebugLogRite("General Socket Error:WSAENOTCONN");
			StopSocketHard();
			break; 
		case WSAENOTSOCK : 
			if(warningLog) DebugLogRite("General Socket Error:WSAENOTSOCK");
			StopSocketHard();
			break; 
		case WSAEOPNOTSUPP : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEOPNOTSUPP");
			StopSocketHard();
			break; 
		case WSAEPFNOSUPPORT : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEPFNOSUPPORT");
			StopSocketHard();
			break; 
		case WSAEPROCLIM : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEPROCLIM");
			StopSocketHard();
			break; 
		case WSAEPROTONOSUPPORT : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEPROTONOSUPPORT");
			StopSocketHard();
			break; 
		case WSAEPROTOTYPE : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEPROTOTYPE");
			StopSocketHard();
			break; 
		case WSAESHUTDOWN : 
			if(warningLog) DebugLogRite("General Socket Error:WSAESHUTDOWN");
			StopSocketHard();
			break; 
		case WSAESOCKTNOSUPPORT : 
			if(warningLog) DebugLogRite("General Socket Error:WSAESOCKTNOSUPPORT");
			StopSocketHard();
			break; 
		case WSAETIMEDOUT : 
			if(warningLog) DebugLogRite("General Socket Error:WSAETIMEDOUT");
			StopSocketHard();
			break; 
//;		case WSATYPE_NOT_FOUND : 
//;			if(warningLog) DebugLogRite("General Socket Error:WSATYPE_NOT_FOUND");
//;			StopSocketHard();
			break; 
		case WSAEWOULDBLOCK : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEWOULDBLOCK");
			StopSocketHard();
			break; 
		case WSAHOST_NOT_FOUND : 
			if(warningLog) DebugLogRite("General Socket Error:WSAHOST_NOT_FOUND");
			StopSocketHard();
			break; 
//;		case WSA_INVALID_HANDLE : 
//;			if(warningLog) DebugLogRite("General Socket Error:WSA_INVALID_HANDLE");
//;			StopSocketHard();
//;			break; 
//;		case WSA_INVALID_PARAMETER : 
//;			if(warningLog) DebugLogRite("General Socket Error:WSA_INVALID_PARAMETER");
//;			StopSocketHard();
//;			break; 
//;		case WSA_IO_INCOMPLETE : 
//;			if(warningLog) DebugLogRite("General Socket Error:WSA_IO_INCOMPLETE");
//;			StopSocketHard();
//;			break; 
//;		case WSA_IO_PENDING : 
//;			if(warningLog) DebugLogRite("General Socket Error:WSA_IO_PENDING");
//;			StopSocketHard();
//;			break; 
//;		case WSA_NOT_ENOUGH_MEMORY : 
//;			if(warningLog) DebugLogRite("General Socket Error:WSA_NOT_ENOUGH_MEMORY");
//;			StopSocketHard();
//;			break; 
		case WSANOTINITIALISED : 
			if(warningLog) DebugLogRite("General Socket Error:WSANOTINITIALISED");
			StopSocketHard();
			break; 
		case WSANO_DATA : 
			if(warningLog) DebugLogRite("General Socket Error:WSANO_DATA");
			StopSocketHard();
			break; 
		case WSANO_RECOVERY : 
			if(warningLog) DebugLogRite("General Socket Error:WSANO_RECOVERY");
			StopSocketHard();
			break; 
//;		case WSASYSCALLFAILURE : 
//;			if(warningLog) DebugLogRite("General Socket Error:WSASYSCALLFAILURE");
//;			StopSocketHard();
			break; 
		case WSASYSNOTREADY : 
			if(warningLog) DebugLogRite("General Socket Error:WSASYSNOTREADY");
			StopSocketHard();
			break; 
		case WSATRY_AGAIN : 
			if(warningLog) DebugLogRite("General Socket Error:WSATRY_AGAIN");
			StopSocketHard();
			break; 
		case WSAVERNOTSUPPORTED : 
			if(warningLog) DebugLogRite("General Socket Error:WSAVERNOTSUPPORTED");
			StopSocketHard();
			break; 
		case WSAEDISCON : 
			if(warningLog) DebugLogRite("General Socket Error:WSAEDISCON");
			StopSocketHard();
			break; 
//;		case WSA_OPERATION_ABORTED : 
//;			if(warningLog) DebugLogRite("General Socket Error:WSA_OPERATION_ABORTED");
//;			StopSocketHard();
//;			break; 
		default:
			if(warningLog) DebugLogRite("General Socket Error:UNKNOWN (poss. byte return mismatch)");
			StopSocketHard();
			break; 
	}
    return;
}
DWORD DutIf_UdpIp::GetErrorLast()
{
	return -1;//W32N_GetErrorLast();
}
void DutIf_UdpIp::SetWarningLog(int setting)  
{
	warningLog = setting;
} 

/*
BOOL DutIf_UdpIp::query(ULONG OID, PUCHAR pInfo, UINT len) 
{
	SocketSendTo((char*)pInfo, len);
	 
	return SocketRcvFrom((char*)pInfo, len, timeOut/1000);

}
*/
BOOL DutIf_UdpIp::GetDebugLogFlag(void)
{
	 return DebugLog;
}

BOOL DutIf_UdpIp::BT_Send(PUCHAR pInfo, UINT len) 
{
	Bridge_Command_Format_Header HearderBuf = {0};
	char *TxBuf=NULL, *ptr=NULL;
	int TotalLength=0;
	char RxBuf[100]={0};
	int Status=0;

     if(vg_IfSpy)
	 {
		 int i=0;

		 SpyLogRite("Hci Command\n");

		 for (i=0; i<len; i++)
			SpyLogRite("%02X ", *(pInfo+i));

		 SpyLogRite("\n");
	 }


    TotalLength = len+ sizeof(Bridge_Command_Format_Header);
    TxBuf = (char *)malloc (len+ sizeof(Bridge_Command_Format_Header));
	memset(TxBuf, 0, TotalLength);
	HearderBuf.Type = 0x0003;
	HearderBuf.Length = len+ sizeof(Bridge_Command_Format_Header);
	memcpy(TxBuf, &HearderBuf, sizeof(Bridge_Command_Format_Header));
	memcpy(TxBuf+(sizeof(Bridge_Command_Format_Header)/sizeof(char)), pInfo, len);
//;	return SocketSendTo((char*) TxBuf, TotalLength);

     if(vg_IfSpy)
	 {
		 int i=0;
		 SpyLogRite("IP Command\n");

		 for (i=0; i<TotalLength; i++)
			SpyLogRite("%02X ", (char)(*(TxBuf+i)));

		 SpyLogRite("\n");
	 }

	Status = SocketSendTo((char*)TxBuf, TotalLength);
/*
	Status = SocketRcvFrom((char*) RxBuf, 100, 5000/1000);

     if(vg_IfSpy)
	 {
 		 int i=0;
		 SpyLogRite("Response\n");

		 for (i=0; i<30; i++)
			SpyLogRite("%02X ", *(TxBuf+i));

		 SpyLogRite("\n");
	 }

	ptr = TxBuf+(sizeof(Bridge_Command_Format_Header)/sizeof(char));
	len =(TotalLength - sizeof(Bridge_Command_Format_Header));
 
    if(vg_IfSpy)
	 {
		 int i=0;
		 SpyLogRite("Event\n");

		 for (i=0; i<len; i++)
			SpyLogRite("%02X ", *(ptr+i));

		 SpyLogRite("\n");
	 }

	memcpy(pInfo, ptr, len);
*/
	if(TxBuf)
		free(TxBuf); 
    return 0;

	//; return DutIf_UdpIp::query(OID, pInfo, len);
}


BOOL DutIf_UdpIp::BT_Receive(PUCHAR pInfo, UINT *pLen) 
{
	Bridge_Command_Format_Header *pHearderBuf = NULL;
	char *TxBuf=NULL, *ptr=NULL;
	int Length=0;
	int Status=0;
	char tempChar[sizeof(HCI_EVENT_STRUCT)+sizeof(Bridge_Command_Format_Header)+200]={0};
  
	memset(pInfo, 0, *pLen);
/*

    TotalLength = len+ sizeof(Bridge_Command_Format_Header);
    TxBuf = (char *)malloc (len+ sizeof(Bridge_Command_Format_Header));
	memset(TxBuf, 0, TotalLength);
	HearderBuf.Type = 0x0003;
	HearderBuf.Length = len+ sizeof(Bridge_Command_Format_Header);
	memcpy(TxBuf, &HearderBuf, sizeof(Bridge_Command_Format_Header));
	memcpy(TxBuf+(sizeof(Bridge_Command_Format_Header)/sizeof(char)), pInfo, len);
//;	return SocketSendTo((char*) TxBuf, TotalLength);
*/
	pHearderBuf = (Bridge_Command_Format_Header*) tempChar;
 
// 	Status = SocketRcvFrom((char*) &HearderBuf, sizeof(Bridge_Command_Format_Header), 5000/1000);
 	Status = SocketRcvFrom((char*) tempChar, sizeof(tempChar), 5000/1000);
	if(0 != pHearderBuf->Length && 0x03 == pHearderBuf->Type)
	{
		Length = pHearderBuf->Length-sizeof(Bridge_Command_Format_Header);

//		TxBuf = (char *)malloc (Length);
//		memset(TxBuf, 0, Length);

//		Status = SocketRcvFrom((char*)TxBuf, Length, 5000/1000);

	TxBuf = tempChar + sizeof(Bridge_Command_Format_Header);

     if(vg_IfSpy)
	 {
 		 int i=0;
		 SpyLogRite("Response\n");

		 for (i=0; i<pHearderBuf->Length; i++)
			SpyLogRite("%02X ", 0xFF&(*(tempChar+i)));

		 SpyLogRite("\n");
	 }

	 if((*pLen) >= Length)
	 {
		(*pLen) = Length; 
		
		memcpy(pInfo, TxBuf, Length);

		if(vg_IfSpy)
		{
			 int i=0;
			 SpyLogRite("Event\n");

			 for (i=0; i<(*pLen); i++)
				SpyLogRite("%02X ", 0xFF&(*(pInfo+i)));

			 SpyLogRite("\n");
		}

	 }
	 else
	 {
		Status = ERROR_BUFFER_TOO_SMALL; 
	 }
 
	}

  // Fill out the event structure TBD_KY

    return Status;

	//; return DutIf_UdpIp::query(OID, pInfo, len);
}

int DutIf_UdpIp::RxHciDataEvent(PUCHAR pData, ULONG BufLength)
{

	PHCI_EVENT_STRUCT pEvent=NULL;
	UCHAR RawBuf[2048]={0}, *ptr=NULL;
	DWORD ByteRead=0;
	UINT i=0, Count=0;
	int Status=0;

 
	memset(pData, 0, BufLength);
	pEvent = (PHCI_EVENT_STRUCT) pData;


	if (BufLength < sizeof(HCI_EVENT_STRUCT))
	{

 		return ERROR_BUFFER_TOO_SMALL;
	}
	ByteRead = BufLength;
	ptr= RawBuf;

	Status=BT_Receive((PUCHAR)RawBuf, (UINT*)&ByteRead);	
	{
/* 		if( 0 == Status) 
		{
 			return ERROR_GET_EVENT_TO;
		}
*/
		if ((*ptr) == HCI_EVENT_PACKET) // Buffer[0]
		{
 			pEvent->IsDataPkt = FALSE; 
			{// event packet. get event code and parmLength (2 bytes)
				ptr++;
				pEvent->EventCode = (*ptr);
				ptr++;
				pEvent->ParamLength = (*ptr);
		
				if (BufLength < (ULONG)(pEvent->ParamLength+2))
				{
 					return ERROR_BUFFER_TOO_SMALL;
				}
								
  
 			// check in the parms
				ptr++;
				switch (pEvent->EventCode)
				{
					case HCI_EVENT_CMD_COMPLETE:
							memcpy((PUCHAR)&pEvent->CmdComplete, ptr, 3); // NumHciCmdPkt, OCF, OGF
							pEvent->CmdComplete.ParamLength = pEvent->ParamLength - 3; 

							if (pEvent->CmdComplete.ParamLength>0)
							{
								pEvent->CmdComplete.pParam = (PUCHAR)malloc(pEvent->CmdComplete.ParamLength);
								memcpy((PUCHAR)pEvent->CmdComplete.pParam, ptr+3, pEvent->CmdComplete.ParamLength);
							}
							else
							{
								pEvent->CmdComplete.pParam = 0;
							}
							break;
					case HCI_EVENT_INQUIRY_RESULT:
 	 						Count = pEvent->InquiryResult.NumResponses = (*ptr);

							for (i=0; i<MAX_INQUIRY_RESPONSES && i<Count; i++)
							{
								memcpy(pEvent->InquiryResult.RespDev[i].BdAddress, ptr+1+i*SIZE_OF_BD_ADDRESS, SIZE_OF_BD_ADDRESS);
								pEvent->InquiryResult.RespDev[i].PageScanRepMode = *(ptr+1+Count*SIZE_OF_BD_ADDRESS+i);
								memcpy(pEvent->InquiryResult.RespDev[i].COD, ptr+1+Count*(SIZE_OF_BD_ADDRESS+3)+3*i, 3);
								memcpy((PUCHAR)&(pEvent->InquiryResult.RespDev[i].ClockOffset), ptr+1+Count*(SIZE_OF_BD_ADDRESS+6)+2*i, 2); // 2 bytes reserved
							}
							break;
					default:
							memcpy((PUCHAR)&pEvent->EventData, ptr, pEvent->ParamLength);
							break;
				} // switch
			} // if 
		} 
		else if ((*ptr) == HCI_ACL_DATA_PACKET)
		{
  			pEvent->IsDataPkt = TRUE; 
			pEvent->EventCode = (*ptr);
			ptr++;

			{ // check next 4 bytes to get the count
				Count = *((USHORT*)(ptr+2));
								
				if (BufLength < (Count+4))
				{
 					return ERROR_BUFFER_TOO_SMALL;
				}
				else
				{
					memcpy((PUCHAR)&pEvent->AclData, ptr, Count+4);
				}
			}
		}
		else if ((*ptr) == HCI_SCO_DATA_PACKET)
		{
 			pEvent->IsDataPkt = TRUE; 
			pEvent->EventCode = (*ptr);
			ptr++;

			{ // check next 3 byte
				Count = (UCHAR)(ptr+2);
				
 
				if (BufLength < (Count+3)) // buffer too small, read the whole data anyway
				{
 					return ERROR_BUFFER_TOO_SMALL;
				}
				else
				{
					memcpy((PUCHAR)&pEvent->ScoData, ptr, Count+3);
				}
			}
		}
		else if ((*ptr) == HCI_OUT_OF_SYNC)
		{
 			return ERROR_OUT_OF_SYNC;
		}
		else if ((*ptr) == HCI_COMMAND_PACKET)
		{
 			return ERROR_COMMAND_PACKET;
		}
		else  
		{
 			return ERROR_UNKNOWN_PACKET;
		}
	}
	
 	return ERROR_SUCCESS;


}
int	 DutIf_UdpIp::WaitForEvent(USHORT CmdGroup, USHORT CmdCode, USHORT TargetEvent, 
				  UCHAR *pEvent, ULONG BufSize)
{
 	UCHAR Target1=0, Target2=0;
	int Status=0;
	ULONG StartTimer=0;
	int nTimer=5000;

	HCI_EVENT_STRUCT *pLocalEvent=NULL;

	Target1 = (UCHAR)(TargetEvent>>8);
	Target2 = (UCHAR)(TargetEvent&0xFF);
	StartTimer = GetTickCount();

	pLocalEvent = (HCI_EVENT_STRUCT *) pEvent;
	while (1)
	{
		if (GetTickCount()-StartTimer >= nTimer)
		{
			return ERROR_GET_EVENT_TO;
		}

//		if ((Status=BT_Receive((PUCHAR)pEvent, (UINT*)&BufSize))!=ERROR_SUCCESS)
		if ((Status=RxHciDataEvent((PUCHAR)pEvent, (DWORD)BufSize))!=ERROR_SUCCESS)
		{
			if (Status == ERROR_UNKNOWN_PACKET || 
				Status == ERROR_GET_EVENT_TO ||
				pLocalEvent->IsDataPkt)
			{
				continue; 
			}
			
			return Status;
		}

 		if (pLocalEvent->EventCode == Target1 || pLocalEvent->EventCode == Target2)
		{
 			if (pLocalEvent->EventCode == HCI_EVENT_CMD_COMPLETE)
			{
				if ( (pLocalEvent->CmdComplete.OCF)== (CmdCode) && 
				     (pLocalEvent->CmdComplete.OGF)== (CmdGroup))
				{
 					return ERROR_SUCCESS;
				}
			}
			else if (pLocalEvent->EventCode == HCI_EVENT_CMD_STATUS) 
			{
				if (pLocalEvent->CmdStatus.OCF==CmdCode && pLocalEvent->CmdStatus.OGF==CmdGroup)
				{
 					return ERROR_SUCCESS;
				}
			}
			else
			{
 			return ERROR_SUCCESS;
			}
		}
		else if (pLocalEvent->EventCode == HCI_EVENT_CMD_STATUS 
				&& pLocalEvent->CmdStatus.OCF==CmdCode 
				&& pLocalEvent->CmdStatus.OGF==CmdGroup 
				&& pLocalEvent->CmdStatus.Status!=ERROR_SUCCESS)
		{
			return pLocalEvent->CmdStatus.Status;
		}				
	}

    return Status;
}

BOOL DutIf_UdpIp::SendCommandToController(UCHAR CmdGroup, 
							 USHORT CmdCode, UCHAR *pParam, USHORT ParamLength)
{
#if 1
	PHCI_COMMAND_STRUCT pHciCmd=NULL;
	PUCHAR pBuffer=NULL;
	DWORD ByteSent=0;
	ULONG BufLen=0;

	

	BufLen = ParamLength+4;
	
	pBuffer = (PUCHAR)malloc(BufLen);
	memset((PUCHAR)pBuffer, 0, BufLen);
	
	pHciCmd = (PHCI_COMMAND_STRUCT)(pBuffer+1);

	*pBuffer=HCI_COMMAND_PACKET;
	
	pHciCmd->CmdCode=CmdCode;
	pHciCmd->CmdGroup=CmdGroup;// <<2;
	pHciCmd->ParamLength=(UCHAR)ParamLength;
	
	if (ParamLength>0)
	{
		memcpy(pHciCmd->Param, pParam, ParamLength);
	}

//;	if (//LastIsTO &&
//;		ClearRxBuf(hPort))
//;	{
//;		return FALSE;
//;	}
/* 
	if(flowCtrl)
	{
#ifdef _LINUX_
	if(-1 == tcflush(hPort,TCIOFLUSH))
	{
 		return FALSE;
	}
#else	
	if (FlushFileBuffers(hPort) == 0)      // clean buffer
    {
 	  return FALSE;
    }
#endif 
	}
*/
/*
 	if (WriteUartFile(hPort,			// handle to file to write to
				pBuffer,              // pointer to data to write to file
				BufLen,              // number of bytes to write
				&ByteSent) == 0)      // pointer to number of bytes written
    {
 	  return FALSE;
    }
 */
	if(BT_Send(pBuffer, BufLen) )
	{
 	  return FALSE;
    }
#endif
//	memcpy(pParam, pBuffer, BufLen); 
	return TRUE;
}

BOOL DutIf_UdpIp::SendHciCommand(USHORT CmdGroup, USHORT CmdCode, USHORT ExpEvent, 
							   PUCHAR pParam, USHORT ParamLength, 
								PUSHORT pFoundEvent,
							   PUCHAR pReturn, USHORT &RetBufSize)
{
#if 1
	HCI_EVENT_STRUCT *pHciEvent=NULL;
 	int Status=0, BufSize=1024;
	char Buf[1024]={0};
 
	pHciEvent = (HCI_EVENT_STRUCT*)Buf;
	if(CmdGroup !=0 && CmdCode !=0)
	{
		if (!SendCommandToController(
				CmdGroup, 
				CmdCode, 
				pParam, 
				ParamLength)) // when clear all filter, only one parameter is used
		{
			return ERROR_HARDWARE_FAILURE; //0x03
//;			return 0x03;
		}
	}
 //   pHciEvent = (HCI_EVENT_STRUCT *)pParam;
	if ((Status=WaitForEvent(
			CmdGroup, 
			CmdCode, 
			ExpEvent,
			(UCHAR*)pHciEvent,
			BufSize //sizeof(HCI_EVENT_STRUCT)
			))==ERROR_SUCCESS)

//	if(1)
	{
	 
		if (RetBufSize >= (*pHciEvent).ParamLength)
		{

			*pFoundEvent = (*pHciEvent).EventCode;

			if ((*pHciEvent).EventCode == HCI_EVENT_CMD_COMPLETE)
			{
				RetBufSize = (*pHciEvent).CmdComplete.ParamLength;
				memcpy(pReturn, (*pHciEvent).CmdComplete.pParam, 
							RetBufSize);
			}
			else if ((*pHciEvent).EventCode == HCI_EVENT_VENDOR_SPECIFIC)
			{
				RetBufSize = (*pHciEvent).ParamLength;
				memcpy(pReturn, &(*pHciEvent).EventData[0], (*pHciEvent).ParamLength);
			}
			else 
			{
				RetBufSize = (*pHciEvent).ParamLength;
				memcpy(pReturn, (*pHciEvent).EventData, (*pHciEvent).ParamLength);
			}
	
			return ERROR_SUCCESS;
		}
		else
		{
//;			return ERROR_BUFFER_TOO_SMALL;
			return 1;
		}
	}
 	else
	{
//;		LastIsTO = (Status == ERROR_GET_EVENT_TO);
 		return Status;
	} 
#endif //#if 0
	return 0;
}



BOOL DutIf_UdpIp::query(ULONG OID, PUCHAR Buf, UINT BufSize)
{
#define BUFSIZE	1024
#define EVENT_BUF_SIZE 400
	UCHAR EventData[EVENT_BUF_SIZE]={0}, Buffer[BUFSIZE]={0};
 	USHORT RetBufSize=0, FoundEvent=0;
	USHORT	CmdGrp=0;
	USHORT	CmdCode=0;
	PUCHAR pInBuf=NULL;
	UINT InBufSize=0;
	UINT i=0;
    int status=0;
	
     if(vg_IfSpy)
	 {
		 DebugLogRite("\nOID %08X\n", OID);
		 DebugLogRite("Command\n");

		 for (i=0; i<BufSize; i++)
			DebugLogRite("%02X ", *(Buf+i));

		 DebugLogRite("\n");
	 }

	CmdGrp = (WORD)(OID>>16);
	CmdCode = (WORD)(OID&0x3FF);
	 switch(CmdGrp)
	 {
		case HCI_CMD_GROUP_INFORMATIONAL:
			pInBuf=NULL;
			InBufSize=0; 
			break;
		case HCI_CMD_GROUP_HC_BASEBAND:
		case HCI_CMD_GROUP_VENDOR_SPECIFIC:
		case HCI_CMD_GROUP_LINK_CONTROL:
		case HCI_CMD_GROUP_LINK_POLICY:
		case HCI_CMD_GROUP_TESTING:
 			pInBuf=Buf;
			InBufSize=BufSize;
			break;
		default:
			return (-4);
			break;

	 }
	memset(EventData, 0, EVENT_BUF_SIZE);
 

	status = SendHciCommand(CmdGrp,  //
					CmdCode,
					HCI_EVENT_CMD_COMPLETE,
					pInBuf,
					InBufSize,
					&FoundEvent,
					EventData,
					(RetBufSize = EVENT_BUF_SIZE>BufSize?EVENT_BUF_SIZE:BufSize));
 	if(status)
	{
		return (status); 
	}

	if(vg_IfSpy)
	{
		 DebugLogRite("\nEventData\n");
		 for (i=0; i<RetBufSize; i++)
			DebugLogRite("%02X ", *(EventData+i));
	
		 DebugLogRite("\n");
	}


//	if (EventData[0]!=ERROR_SUCCESS)
//	{
//		 
//		return EventData[0];
//	}
//	else
	{
		memset(Buf, 0, BufSize);
		memcpy(Buf, &EventData[1], RetBufSize-1);

		if(vg_IfSpy)
		{
			 DebugLogRite("\nResponse\n");
			 for (i=0; i+1<RetBufSize; i++)
				DebugLogRite("%02X ", *(Buf+i));
		
			 DebugLogRite("\n");
		}
		return EventData[0];
	}
	
	return 0;
}


BOOL DutIf_UdpIp::queryHci(ULONG OID, PUCHAR Buf, UINT BufSize, 
						UINT ExpectedEvent,	  
						UINT *BufRetSize)
{
#if 1
#ifdef _HCI_PROTOCAL_

#define BUFSIZE	1024
#define EVENT_BUF_SIZE 400
	UCHAR EventData[EVENT_BUF_SIZE]={0}, Buffer[BUFSIZE]={0};
 	USHORT RetBufSize, FoundEvent=0;
	USHORT	CmdGrp=0;
	USHORT	CmdCode=0;
	PUCHAR pInBuf=NULL;
	UINT InBufSize=0;
	int i=0, status=0;
	UINT	expectEvent=HCI_EVENT_CMD_COMPLETE;
	UINT	expectedReturnBuf=EVENT_BUF_SIZE;
	
     if(vg_IfSpy)
	 {
		 SpyLogRite("\nOID %08X\n", OID);
		 SpyLogRite("Command\n");

		 for (i=0; i<BufSize; i++)
			SpyLogRite("%02X ", *(Buf+i));

		 SpyLogRite("\n");
	 }
	if(ExpectedEvent) 
		expectEvent = (0xFFFF & ExpectedEvent);
	else
		expectEvent = HCI_EVENT_CMD_COMPLETE;

	if(BufRetSize) 
		expectedReturnBuf = *BufRetSize;	
	else 
		expectedReturnBuf = BufSize;	


	CmdGrp = (OID>>16);
	CmdCode = (OID&0x3FF);
	 switch(CmdGrp)
	 {
		case HCI_CMD_GROUP_INFORMATIONAL:
			pInBuf=NULL; // for GetBDAddress
			InBufSize=0; 
			break;
		case HCI_CMD_GROUP_HC_BASEBAND:
		case HCI_CMD_GROUP_VENDOR_SPECIFIC:
		case HCI_CMD_GROUP_LINK_CONTROL:
		case HCI_CMD_GROUP_LINK_POLICY:
			pInBuf=Buf;
			InBufSize=BufSize;
			break;
		case 0:
			break;
		default:
			return (-4);
			break;

	 }
	memset(EventData, 0, EVENT_BUF_SIZE);

//;	Sleep(vg_dwSleep);

 


		status = SendHciCommand(CmdGrp,  //
					CmdCode,
					expectEvent,
					pInBuf,
					InBufSize,
					&FoundEvent,
					EventData,
					(RetBufSize = EVENT_BUF_SIZE));

 	if(status)
	{
		return (status); 
	}

	if (EventData[0]!=ERROR_SUCCESS && FoundEvent != HCI_EVENT_VENDOR_SPECIFIC)
	{
		if( 0xFF == EventData[0] //&& 
			//(	(HCI_EVENT_VENDOR_SPECIFIC ==  (expectEvent&0x00FF)	) ||  
			//	(HCI_EVENT_VENDOR_SPECIFIC == ((expectEvent&0xFF00)>>8)) 
			//)
		  )
		{
			return (EventData[0] | (EventData[2]<<16));
		}
		else
		{
			return EventData[0];
		}
	}
	else
	{
		memset(Buf, 0, (BufSize>expectedReturnBuf)?BufSize:expectedReturnBuf);
 
		if(RetBufSize <= expectedReturnBuf) 
		{
			memcpy(Buf, &EventData[0], RetBufSize);

			if(vg_IfSpy)
			{
				 SpyLogRite("\nResponse\n");
				 for (i=0; i<RetBufSize; i++)
					SpyLogRite("%02X ", *(Buf+i));
			
				 SpyLogRite("\n");
			}
			return EventData[0];
		}
		else
		{

			memcpy(Buf, &EventData[1], expectedReturnBuf);

			if(vg_IfSpy)
			{
				 SpyLogRite("\nResponse\n");
				 for (i=0; i<expectedReturnBuf; i++)
					SpyLogRite("%02X ", *(Buf+i));
			
				 SpyLogRite("\n");
			}

#if 0 // when the bridge support multiple events back
			if(HCI_EVENT_VENDOR_SPECIFIC == FoundEvent && 
			(	(HCI_EVENT_CMD_COMPLETE ==  (expectEvent&0x00FF)	) ||  
				(HCI_EVENT_CMD_COMPLETE == ((expectEvent&0xFF00)>>8)) 
			)
		  )
		{
			RetBufSize = EVENT_BUF_SIZE; 
			WaitForEvent( CmdGrp,  
						CmdCode, HCI_EVENT_CMD_COMPLETE, 
						EventData, RetBufSize);

		}
			return EventData[0];
#else //#if 0 // when the bridge support multiple events back
			if(HCI_EVENT_VENDOR_SPECIFIC == FoundEvent)
				return ERROR_SUCCESS;
			else
				return EventData[0];
#endif //#if 0 // when the bridge support multiple event back

		}

	}
#endif //#ifdef _HCI_PROTOCAL_
#endif //#if 1
	return 0;
}
 
#endif

#endif
