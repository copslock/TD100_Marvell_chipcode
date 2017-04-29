/** @file xmodem.h
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
#ifdef _XMODEM_EXPORTS
#define XMODEM_API __declspec(dllexport)
#else
#define XMODEM_API __declspec(dllimport)
#endif
#else // //#ifndef _LINUX_
#define XMODEM_API
#endif 

XMODEM_API BOOL UploadXModem(char *szPortname, DWORD szBaudrate, 
							 char *szFilename, bool Wait4Nack=0);

XMODEM_API int pollUart(HANDLE hPort, unsigned char targetByte, int TimeOut);

XMODEM_API int XModemVersion(void);

XMODEM_API char* XModemAbout(void);


