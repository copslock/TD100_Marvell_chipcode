/** @file UartPortCheck.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include "../../../../DutCommon/utility.h"
#include "UartIf.h" 
#include "UartPortCheck.h"
#include "UartPortCheckVer.h"
 
 
#ifndef _LINUX_FEDORA_
ULONG nTimer=100; 
ULONG nPortTimer=5000; 
bool LastIsTO=0;
#endif // #ifndef _LINUX_FEDORA_

#define COM_BUF_SIZE		8192
#define PAYLOAD_LEN			128
#define PACKET_LEN			PAYLOAD_LEN+4		// 3 byte header and 1 byte checksum
#define RETRIES_FOR_X  12
#define RETRIES  12
#define CRCTRIES 2
 
// Local data
static int gcTries;               // retry counter
static HANDLE gnPortID;              // ID of the current comm. port
static unsigned char *WholeBinBuf;

// Prototypes:
WORD CalculateCRC(char *pchBuffer, int nLen);
static int CharsWaitingToBeRead(HANDLE nPortID);
static int ComReadChar(HANDLE nPortID);
static BOOL ComWriteChar(HANDLE nPortID, unsigned char nChar);

/*
// Wait x seconds for char in the com. port, return -1 if none, char otherwise
static int ComReadCharTimeOut(HANDLE nPortID, int nSeconds)
	{
	int rdBck=0, flagRd=0;
	DWORD dwEnd = GetTickCount() + nSeconds * 1000;
//	gbTimeOut = FALSE;
	while (dwEnd > GetTickCount())
		{
		while (CharsWaitingToBeRead(nPortID))
		{
			rdBck = ComReadChar(nPortID);
			flagRd=1;
		}

		if(flagRd) 
			return (rdBck&0xFF);
//		if (CharsWaitingToBeRead(nPortID))
//			return ComReadChar(nPortID);
		
		//		DoEvents();
		Sleep(1);
		}
//	gbTimeOut = TRUE;
	return -1;
	}


WORD CalculateCRC(char *pchBuffer, int nLen)
	{
	int i;
	DWORD dwCRC = 0;

	while (nLen--)
		{
		dwCRC |= (*pchBuffer++) & 255;
		for (i = 0; i < 8; i++) 
			{
			dwCRC <<= 1;
			if (dwCRC & 0x1000000L)
				dwCRC ^= 0x102100L;
			}
		}
	return (WORD) (dwCRC >> 8);
	}


// Returns the number of characters waiting in the input queue
int CharsWaitingToBeRead(HANDLE nPortID)
{return CheckInQ(nPortID);}
// Read a character from the port specified by nPortID
// returns -1 if no character available
int ComReadChar(HANDLE nPortID)
{
unsigned char Buf[2]="";
unsigned long Rd=0;
ReadBytes(nPortID, Buf, 1, &Rd);
return Buf[0];
}  
// Read a character from the port specified by nPortID
// returns the number of characters read or -1 if an error occurs.
int ComReadChars(HANDLE nPortID, char *pchBuffer, int cbBuffer)
{
unsigned long Rd=0;
ReadBytes(nPortID, (unsigned char*)pchBuffer, cbBuffer, &Rd);
return (cbBuffer == Rd)?0:-1;
}

// Write a character to the port specified by nPortID
// returns TRUE if success, FALSE if failure
BOOL ComWriteChar(HANDLE nPortID, unsigned char nChar)
	{
	unsigned long buf;
	if (nPortID < 0)
		return FALSE;
	if (!WriteUartFile(nPortID, &nChar, 1, &buf))
		{
#ifndef _LINUX_
		ClearCommError(nPortID, NULL, NULL);
#endif
		return FALSE;
		}
	return TRUE;
	}

 
 

// Upload a file using the XModem protocol return TRUE if success
// hParent:    the handle of the window that should be the parent of
//             the transfer status dialog box (shouldn't be NULL).
// gnPortID:   the port ID of the port which is used for the transfer
// szFilename: the name of the file to be transfered
//BOOL UploadXModem(int gnPortID, char *szFilename)
XMODEM_API BOOL UploadXModem(char *szPortname, DWORD szBaudrate, 
							 char *szFilename, bool Wait4Nack)
	{
	int i=0, nCheckSum=0, chAnswer = 0, chCRCOut = 0;
	long LenBinFile=0;
	unsigned char *pBlock=NULL;
	int NumOfPacket=0, SizeLastPkt=0, CntTxPkt=0;
//	int nCounter=0;
	WORD wCRC=0;
//	char nBlock = 1;
	BOOL error=0;
//	int hFile=0;
	FILE* hFile=NULL;	
	unsigned long wt=0;
	
	 
//	hFile = _lopen(szFilename, OF_READ);
	LenBinFile = filelength(szFilename);
 	if(0> LenBinFile) 
	{
	printf( "File NOT found %s\n", szFilename);
		return 1;       // Port is not open or file not found		
	} 

 //	gnPortID = Init(szPortname,szBaudrate,0,1,8); 
	gnPortID = InitUartPort(szPortname,szBaudrate,0,0);
	if (gnPortID < 0)
	{
		printf( "Port Error\n");
		return  1;       // Port is not open or file not found
	}
	else
	{
		printf( "Xmodem with %s\n", szFilename);
	}
		
	NumOfPacket = LenBinFile/PAYLOAD_LEN;
	SizeLastPkt = LenBinFile%PAYLOAD_LEN;
	if (SizeLastPkt>0)
		{NumOfPacket++;}
 
	WholeBinBuf =(unsigned char*) malloc(sizeof(char)*NumOfPacket*PAYLOAD_LEN);
	memset(WholeBinBuf, 0, sizeof(char)*NumOfPacket*PAYLOAD_LEN);
	
	hFile = fopen(szFilename, "rb");
	if ((gnPortID < 0) || (hFile < 0))
		return 1;       // Port is not open or file not found

		fread(WholeBinBuf, sizeof(char), LenBinFile, hFile);
		pBlock = WholeBinBuf;
 
		
	gcTries = 0;
	if(Wait4Nack)
	{			
		while (gcTries++ < RETRIES_FOR_X && chCRCOut != NAK && chCRCOut != CRC)
			chCRCOut = ComReadCharTimeOut(gnPortID, 6);
		
		if (gcTries==RETRIES_FOR_X)
			return FALSE;
		
		gcTries = 0;
		if (chCRCOut != NAK)   // Time out or tried 10 times without succes
			gcTries = RETRIES;
	}	
	else
	{
		chCRCOut = NAK; //ComReadCharTimeOut(gnPortID, 6);
	}

	while (gcTries < RETRIES && (CntTxPkt < NumOfPacket) && chAnswer != CAN)
		{
		// read the next data block
		pBlock = WholeBinBuf + CntTxPkt * PAYLOAD_LEN;
		
 		
		if(!ComWriteChar(gnPortID, SOH))        // SOH
			return FALSE;
		if(!ComWriteChar(gnPortID, (CntTxPkt+1)))     // block number
			return FALSE;
		if(!ComWriteChar(gnPortID, (~(CntTxPkt+1))))    // 1s complement
			return FALSE;
		nCheckSum = 0;
		// send the data block
		if (!WriteUartFile(gnPortID, pBlock, PAYLOAD_LEN, &wt))
		{
#ifdef _LINUX_ 
			while(errno!=0);
#else
			ClearCommError(gnPortID, NULL, NULL);
#endif
			return FALSE;
		}
		for (i = 0; i < 128; i++)
		{
 
			nCheckSum += pBlock[i];     // checksum calculation
		}
		// Send error-correcting value (nCheckSum or crc)
		if (chCRCOut == NAK)
		{
			if(!ComWriteChar(gnPortID, (nCheckSum & 255)))
				return FALSE;
		}
		else
		{
			wCRC = CalculateCRC((char*)pBlock, 130);
			if(!ComWriteChar(gnPortID, ((wCRC >> 8) & 255)))
				return FALSE;
			if(!ComWriteChar(gnPortID, (wCRC & 255)))
				return FALSE;
		}
		// read ACK, NAK, or CAN from receiver
		chAnswer = ComReadCharTimeOut(gnPortID, 10);
		if (chAnswer == ACK)
			{
			CntTxPkt++;
			
 			gcTries = 0;
 			printf("\rBlocks downloaded: %8d", CntTxPkt+1);
			fflush(stdout);
			}
	 
		}
	if (CntTxPkt >= NumOfPacket) //(bEOF)
		{
		if(!ComWriteChar(gnPortID, EOT))        // send the EOT
			return ERROR_WRITE_FAULT;
		chAnswer = ComReadCharTimeOut(gnPortID, 10);   // wait for an ACK
		if (chAnswer == ACK) error = NO_ERROR;
		else error = ERROR_GEN_FAILURE;  
		}
	else
		error = ERROR_WRITE_FAULT;                    // transfer aborted
	fclose(hFile);

 	printf("\n");

	CloseUartPort(gnPortID);
 	return error;                         // success
	}
*/
UARTPORTCHECK_API int pollUart(HANDLE hPort, int NumOfByte, 
							   unsigned char *pTargetByte, int TimeOut)
 {
 	long StartTime={0}, NowTime={0};
	unsigned char Buffer[20]={0};
	unsigned long ByteRead=0;
	int found=1, i=0;

 			StartTime= GetTickCount();
 
		do
			{
			//	time(&NowTime);
  				ReadBytes(hPort, Buffer, NumOfByte, &ByteRead);
				NowTime = GetTickCount();
				
//				printf("Polling for %X (%d:%d) %d  %X\n",pTargetByte[0], TimeOut, (NowTime-StartTime), ByteRead, Buffer[0]);
				
//				if(NumOfByte == ByteRead && Buffer[0] == pTargetByte[0]
//					&& Buffer[1] == pTargetByte[1] && Buffer[2] == pTargetByte[2]
//					&& Buffer[3] == pTargetByte[3] && Buffer[4] == pTargetByte[4])
//					{found=1; break;}

				for (i=0; i<NumOfByte; i++)
				{
//					printf("Comparing for %X == %X ? \n",pTargetByte[i], Buffer[i]);
					if(Buffer[i] != pTargetByte[i] ) 
					{found=0; break;}
				}

				if(found)
				{
//					printf("found\n");
					break;
				}
 
			}while( (NowTime-StartTime) < TimeOut);
 
			
			return found;
 }


UARTPORTCHECK_API int UartPowrtCheckVersion()
{
	return MFG_VERSION(UARTCHECKCTRL_VERSION_MAJOR1, UARTCHECKCTRL_VERSION_MAJOR2, 
		UARTCHECKCTRL_VERSION_MINOR1,UARTCHECKCTRL_VERSION_MINOR2);

}

UARTPORTCHECK_API char* UartPowrtCheckAbout()
{
	char AboutFormat[512]=
			"Name:\t\tXMODEM\n"
#ifdef _UDP_
			"Interface:\t""UART\n" 
#endif	
			"Version:\t%d.%d.%d.%02d\n" 
 			"Date:\t\t%s (%s)\n\n" 			
 			"Note:\t\t\n" 
			;
 
	static char AboutString[512]="";
 
	sprintf(AboutString, AboutFormat, 
			UARTCHECKCTRL_VERSION_MAJOR1,	
			UARTCHECKCTRL_VERSION_MAJOR2,
			UARTCHECKCTRL_VERSION_MINOR1, 
			UARTCHECKCTRL_VERSION_MINOR2,
			__DATE__, __TIME__);

	return AboutString; 
}


