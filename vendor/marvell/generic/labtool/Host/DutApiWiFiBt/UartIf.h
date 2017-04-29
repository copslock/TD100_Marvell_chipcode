/** @file UartIf.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#ifndef _UART_IF_H_
#define _UART_IF_H_

#include "../../../../DutCommon/Clss_os.h" 
#include "../../../../DutCommon/utilities_os.h" 

#ifdef _LINUX_
#include <sys/ioctl.h>   
#include <termios.h>
#include <limits.h>
#include <sys/times.h>  
#endif //#ifdef _LINUX_


extern ULONG nTimer; 
extern ULONG nPortTimer;//100; 
//int nflowctrl=1;
extern bool LastIsTO;

#ifdef _LINUX_
long GetTickCount();
#endif //#ifdef _LINUX_
//off_t filelength(const char *filename);
int CheckInQ(HANDLE hPort);
int ClearRxBuf(HANDLE hPort);
int SetUartConfig(HANDLE hPort, DWORD BaudRate, DWORD flowControl);
void SetUartTimeoutTimer(HANDLE hPort, ULONG newTimer);
ULONG GetUartTimeoutTimer(HANDLE hPort); 


HANDLE InitUartPort(char *szPort, DWORD BaudRate, DWORD flowControl, bool OverLapped);
BOOL CloseUartPort(HANDLE hPort);

int ReadUartFile(HANDLE hPort, 
				 unsigned char *pBuf, int Len, 
				 unsigned long *pRd);
int WriteUartFile(HANDLE hPort, 
				  unsigned char *pBuf, int Len, 
				  unsigned long *pRd);

int ReadBytes(HANDLE hPort, 
			  unsigned char *pBuf, int Len, 
			  unsigned long *pRd);


#endif //#ifndef _UART_IF_H_
