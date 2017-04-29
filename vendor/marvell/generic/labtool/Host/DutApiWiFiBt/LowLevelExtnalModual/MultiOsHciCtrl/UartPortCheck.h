/** @file UartPortCheck.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#define PADCHAR  0x1a
#define SOH      1
#define EOT      4
#define ACK      6
#define NAK      0x15
#define CAN      0x18
#define CRC      'C'

#ifndef _LINUX_
#ifdef _UARTPORTCHECK_EXPORTS
#define UARTPORTCHECK_API __declspec(dllexport)
#else
#define UARTPORTCHECK_API __declspec(dllimport)
#endif
#else // //#ifndef _LINUX_
#define UARTPORTCHECK_API
#endif 
/*
XMODEM_API BOOL UploadXModem(char *szPortname, DWORD szBaudrate, 
							 char *szFilename, bool Wait4Nack=0);
*/
UARTPORTCHECK_API int pollUart(HANDLE hPort, int NumOfByte, unsigned char *pTtargetByte, int TimeOut);

UARTPORTCHECK_API int UartPowrtCheckVersion(void);

UARTPORTCHECK_API char* UartPowrtCheckAbout(void);


