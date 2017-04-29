/** @file DutIf_UartComm.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

//////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
#include "DutIf_UartComm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// prefix of adapter description
static char adapter_prefix[256] = "Marvell SD8686 ";

//CRH
char Revtempstr[256];
char ComStr[16]="\\\\.\\COM";
DWORD dwBaudRateBuf;
BYTE  byStopBitsBuf, byByteSizeBuf, byParityBuf;
#define FAIL -1
#define SUCCESS 0
//char Com_Port[10]={"COM1"};
#include "../../DutCommon/utilities.h" 
#include "..\DutBtApi878XDll\HciStruct.h"
#include "..\DutBtApi878XDll\hci_opcodes.h"
#include "..\DutBtApi878XDll\UartErrCode.h"


DWORD crc32_table[256];
#define CRC32_POLY 0x04c11db7 
#define SizeOfPattern	2
#define SizeOfPlayLenth	2
#define SizeOfFCS		4
#define SizeOfUARTCmdHead  (SizeOfPattern+SizeOfPlayLenth+SizeOfFCS)   

typedef struct Bridge_Command_Format_Header
{
	WORD    Type;
	WORD	SubType;
	WORD	Length;
	WORD	Status;
	DWORD	Reserved;
}Bridge_Command_Format_Header;

typedef struct UartBridge_Command_Format_Header
{	
	WORD	Signature;
	WORD	UartLength;
//	WORD    Type;
//	WORD	SubType;
//	WORD	Length;
//	WORD	Status;
//	DWORD	Reserved;
	Bridge_Command_Format_Header Header;
//	DWORD	FCS;
}UartBridge_Command_Format_Header;
#define UART_SIGNATURE	(0x5555)

void init_crc32()
{
	int i, j;
	DWORD c;
	for (i = 0; i < 256; ++i) 
	{
		for (c = i << 24, j = 8; j > 0; --j)
			c = c & 0x80000000 ? (c << 1) ^ CRC32_POLY : (c << 1);
		crc32_table[i] = c;
	}
}


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



DWORD crc32(BYTE *buf, int len)
{
BYTE *p;
DWORD  crc;
//;if (!crc32_table[1]) 
//;init_crc32();
crc = 0xffffffff; 
for (p = buf; len > 0; ++p, --len)
{
 crc = (crc << 8) ^ crc32_table[(crc >> 24) ^ *p];
 //;DebugLogRite("0x%x, 0x%x\n", crc, ~crc);
}
return ~crc;
}

 



DutIf_UartComm::DutIf_UartComm(void)
{
 	char temp[_MAX_PATH]="";
	int PortNo=1;
  
 	if(getcwd(temp, _MAX_PATH) != NULL)  
 		strcat(temp, "\\setup.ini");
 	else
		strcpy(temp, "setup.ini");
 		
	DebugLog	= GetPrivateProfileInt("DEBUG","debuglog", 	0, temp); 

	m_theCommPort.debugLog = DebugLog;

	vg_checkFW	= GetPrivateProfileInt("COMSET", "CHECKFW", 0, temp); 
	if (vg_checkFW)
	{
		vg_time2wait	= GetPrivateProfileInt("COMSET", "TIME2WAITINSEC", 5, temp); 
	}

	vg_maxWait	= GetPrivateProfileInt("COMSET","maxWait",1000, temp); 
	vg_uartdelay = GetPrivateProfileInt("COMSET","UartDelay", 0 ,temp);
	vg_dwSleep = GetPrivateProfileInt("DutInitSet","Delay",1, temp); 
 	vg_retry_temp = GetPrivateProfileInt("DutInitSet","retry",0, temp); 
  	GetPrivateProfileString("Driver","AdapterPrefix","Marvell",adapter_prefix, sizeof(adapter_prefix), temp); 
	vg_dwSpiSleep	= GetPrivateProfileInt("DutInitSet","SpiDelay", 0, temp); 
 	vg_IfSpy = GetPrivateProfileInt("DEBUG","IfSpy", 0, temp); 

	PortNo = GetPrivateProfileInt("COMSET","ComNo", 1, temp); 
    sprintf(ComStr, "\\\\.\\COM%d", PortNo);

	dwBaudRateBuf = GetPrivateProfileInt("COMSET","BaudRate", 38400, temp);
	byParityBuf = GetPrivateProfileInt("COMSET","byParity", 0, temp);
	byStopBitsBuf = GetPrivateProfileInt("COMSET","byStopBits", 1, temp);
	byByteSizeBuf = GetPrivateProfileInt("COMSET","byByteSize", 8, temp);

	if(DebugLog)
	{
//;		DebugLogRite( "DebugLog %d\n", DebugLog);
 		DebugLogRite("%s\n", temp);
		DebugLogRite( "adapter_prefix %s\n", adapter_prefix);
  		DebugLogRite( "Delay %X\n", vg_dwSleep);
		DebugLogRite( "maxWait %d\n", vg_maxWait);
		DebugLogRite( "Retry %X\n", vg_retry_temp);
		DebugLogRite( "SpiDelay %X\n", vg_dwSpiSleep);
		DebugLogRite( "IfSpy %d\n", vg_IfSpy);
		//;
		DebugLogRite( "HOST is %s\n", ComStr);
		DebugLogRite( "BaudRate is %d\n", dwBaudRateBuf);
		DebugLogRite( "ByParity is %d\n", byParityBuf);
		DebugLogRite( "ByStopBits is %d\n", byStopBitsBuf);
		DebugLogRite( "ByByteSizeBuf is %d\n", byByteSizeBuf);
	}
    //init CRC buffer
	init_crc32();
	return;
}

DutIf_UartComm::~DutIf_UartComm(void)
{
}


BOOL DutIf_UartComm::query( ULONG OID, PUCHAR pInfo, UINT len)
{
	int status=0;
	std::string szData ;
	std::string szDataEx;
	int temp_len;
	
	int i=0;
	int *TxBuf=NULL;
	WORD *ptr=NULL; 
	DWORD CheckSum=0, *CheckSumPtr=NULL;

    if (DebugLog)
		DebugLogRite("Payload len %d\n", len); 

	TxBuf = (int *)malloc (len+ (sizeof(BYTE)*SizeOfUARTCmdHead)); //Pattern(2 bytes) + Length(2) + FCS (4)
	memset(TxBuf, 0, len+ SizeOfUARTCmdHead);
	ptr = (WORD *)TxBuf;
	//Pattern
	*ptr= 0x5555;
	//Length
      ptr++;
	*ptr=len;
	//Payload
	ptr++;

	memcpy(ptr, pInfo, len);
	//FSC
	CheckSum= crc32(pInfo, len);
  
	CheckSumPtr =(DWORD *)ptr+(len/sizeof(DWORD));
	*CheckSumPtr = CheckSum;
	if (DebugLog)
		DebugLogRite("CRC:0x%08x\n", CheckSum);
    
//;	status = m_theCommPort.Write((const char* )pInfo, len);
	if (DebugLog)
	{
		DebugLogRite("Data Out:\n");
		for ( i=1; i< (len+ SizeOfUARTCmdHead) ; i++)
		{   
			BYTE *DebugPtr=NULL;
			DebugPtr =(BYTE *) TxBuf;
				DebugPtr +=i-1;
			DebugLogRite("%02x", *DebugPtr);

			if((! (i%16)) && (i!=0))
					DebugLogRite("\n");
		}
		DebugLogRite("\n");
	}
	if (DebugLog)
		DebugLogRite("UART data Out len:%d\n", len+ SizeOfUARTCmdHead);
	Sleep(vg_uartdelay);
	status = m_theCommPort.Write((const char* )TxBuf, len+ SizeOfUARTCmdHead);


	if (status) 
	{
		if(TxBuf) 
			free(TxBuf);
		return status;
	}
	while(1)
	{
		status = m_theCommPort.Read_N (szData,4, vg_maxWait);
		if (status) 	
		{
			if(TxBuf)
				free(TxBuf);
			return status;
		}
/*
		if(status)
		{
			printf("UART retry!!\n");
			status = m_theCommPort.Write((const char* )pInfo, len);
			if (status) return status;
			status = m_theCommPort.Read_N (szData,4, vg_maxWait);
			if (status) return status;
		}
*/
		temp_len = ((unsigned char)szData[3])*256 + ((unsigned char)szData[2]) + SizeOfUARTCmdHead;
		if(szData.at(1) == 0x70)
		{
			status = m_theCommPort.Read_N (szDataEx, (temp_len-4), vg_maxWait);
			DebugLogRite("DEBUG: %s\n", szDataEx.c_str());
			if (status) 
			{
				if(TxBuf)
				free(TxBuf);
				return status;
			}
		}
		else
		{
			break;
		}
    }
    
	status = m_theCommPort.Read_N (szDataEx, (temp_len-4), vg_maxWait);
	if (status) 
	{
		if(TxBuf)
			free(TxBuf);
		return status;
	}
	szData.append(szDataEx);
	if ( len > szData.length()) 
		len = szData.length();

	if (DebugLog)
	DebugLogRite("UART data In len:%d\n", temp_len);
  
	memcpy(TxBuf,(unsigned char *)szData.c_str(), len+SizeOfUARTCmdHead);
	if (DebugLog)
	{
		DebugLogRite("Data In :\n");
		for ( i=1; i< (len+ 8+1) ; i++)
		{   
			BYTE *DebugPtr=NULL;
			DebugPtr =(BYTE *) TxBuf;
				DebugPtr +=i-1;
			DebugLogRite("%02x", *DebugPtr);

			if((! (i%16)) && (i!=0))
					DebugLogRite("\n");
		}
	}

	ptr = (WORD *)TxBuf+((SizeOfPattern+SizeOfPlayLenth)/sizeof(WORD));  //Point to payload
	memcpy(pInfo,ptr, len);  //copy payload
	//Check FSC
	{
		CheckSum =0;
		CheckSum= crc32(pInfo, len); // 8 = 0x5555 + line(2bytes)+ CRC (4bytes)
		if (DebugLog)
			DebugLogRite("Local CRC:0x%08x\n", CheckSum);
		CheckSumPtr =(DWORD *)ptr+((len)/sizeof(DWORD));
		 if(CheckSum != *CheckSumPtr)
		 {
			 DebugLogRite("CRC error! Receive CRC:0x%08x\n", &CheckSumPtr);	
			 status =1;
		 }
	}
	if(TxBuf)
		free(TxBuf);
	return status;
}
 


BOOL DutIf_UartComm::set( ULONG OID, PUCHAR pInfo, UINT len)
{
//	std::string szData;
//	char  buff[2];
//	buff[0]=0;	
	m_theCommPort.Write((const char *)pInfo, len );
	return 0;
}

  
int DutIf_UartComm::OpenDevice( )
{
	int status =0;
	
	status = m_theCommPort.Init (ComStr,dwBaudRateBuf,0,1,8);   //sidd
	if (status) return status;
	status = m_theCommPort.Start ();
	if (status) return status;

	if (vg_checkFW)
	{
		std::string  buffer;
		BOOL notEmpty=false;
		clock_t StartTime={0}, NowTime={0};

		time(&StartTime);

		time(&NowTime);
/*
		//wait is in the unit of seconds
		while (((NowTime-StartTime) < vg_time2wait) && !notEmpty )
		{
			time(&NowTime);
			notEmpty = m_theCommPort.ReadAvailable(buffer);
			notEmpty = buffer.length();
			Sleep(300);
			DebugLogRite(".");
		}
*/
		DebugLogRite("\n");
#ifdef _DEBUG
		DebugLogRite("\nWaited for %d s\t (%d)\n", NowTime-StartTime, notEmpty); //in seconds
		time(&StartTime);
#endif

		if (buffer.c_str()[0] == 0x15)
		{
			//disconnect the come port
			CloseDevice();

			//download the FW
			{
				int iMyCounter = 0, iReturnVal = 0,SecondsToWait=20;
				DWORD dwExitCode;
				FILE *fileTemp;
				char cmdLine[256];
				char cwd[256];
				STARTUPINFO siStartupInfo;
				PROCESS_INFORMATION piProcessInfo;
				memset(&siStartupInfo, 0, sizeof(siStartupInfo));
				memset(&piProcessInfo, 0, sizeof(piProcessInfo));
				siStartupInfo.cb = sizeof(siStartupInfo);

				//download the FW
				getcwd(cwd, _MAX_PATH);	//current working directory

				//check whether helper_uart.code is there
				strcpy (cmdLine, cwd);
				strcat (cmdLine, "\\helper_uart.code ");	//executable
				fileTemp = fopen(cmdLine,"r");
				if(!fileTemp)
				{
					DebugLogRite("cannot access '%s'\n",cmdLine);
					return -1;
				}
				else
					fclose (fileTemp);
				//sd8686_mfg_uart.code
				strcpy (cmdLine, cwd);
				strcat (cmdLine, "\\sd8686_mfg_uart.code ");	//executable
				fileTemp = fopen(cmdLine,"r");
				if(!fileTemp)
				{
					DebugLogRite("cannot access '%s'\n",cmdLine);
					return -1;
				}
				else
					fclose (fileTemp);
				strcpy (cmdLine, cwd);
				strcat (cmdLine, "\\mfgdl.exe ");	//executable
				fileTemp = fopen(cmdLine,"r");
				if(!fileTemp)
				{
					DebugLogRite("cannot access '%s'\n",cmdLine);
					return -1;
				}
				else
					fclose (fileTemp);
				strcat (cmdLine, ComStr);			//comport
				strcat (cmdLine, " 38400 helper_uart.code");	//baud rate and fw file name
				status = CreateProcess(NULL, cmdLine, NULL, NULL, false, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &siStartupInfo, &piProcessInfo);
				if (status)
				{
					GetExitCodeProcess(piProcessInfo.hProcess, &dwExitCode);

					while (dwExitCode == STILL_ACTIVE && SecondsToWait != 0)
					{
						GetExitCodeProcess(piProcessInfo.hProcess, &dwExitCode);
						Sleep(500);
						iMyCounter += 500;

						if (iMyCounter > (SecondsToWait * 1000)) 
						{ 
							dwExitCode = 0;
							DebugLogRite("Timeout on Download Helper!\n");
							return 1;
						} 
					}
					CloseHandle(piProcessInfo.hProcess);
					CloseHandle(piProcessInfo.hThread);
				}
				else
				{
					DebugLogRite("Fail to download Helper!\n");
					return !status;
				}

				getcwd(cmdLine, _MAX_PATH);	//current working directory
				strcat (cmdLine, "\\mfgdl.exe ");	//executable
				strcat (cmdLine, ComStr);			//comport
				strcat (cmdLine, " 115200 sd8686_mfg_uart.code");	//baud rate and fw file name
				status = CreateProcess(NULL, cmdLine, NULL, NULL, false, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &siStartupInfo, &piProcessInfo);
				if (status)
				{
					iMyCounter = 0;
					iReturnVal = 0;
					GetExitCodeProcess(piProcessInfo.hProcess, &dwExitCode);

					while (dwExitCode == STILL_ACTIVE && SecondsToWait != 0)
					{
						GetExitCodeProcess(piProcessInfo.hProcess, &dwExitCode);
						Sleep(500);
						iMyCounter += 500;

						if (iMyCounter > (SecondsToWait * 1000)) 
						{ 
							dwExitCode = 0;
							DebugLogRite("Timeout on Download FW!\n");
							return 1;
						} 
					}
					CloseHandle(piProcessInfo.hProcess);
					CloseHandle(piProcessInfo.hThread);
#ifdef _DEBUG
					time(&NowTime);
					DebugLogRite("Download time:%d s\n", NowTime-StartTime); //in seconds
#endif
				}
				else
				{
					DebugLogRite("Failed to download FW!\n");
					return !status;
				}
			}

			//reconnect the com port
			status = m_theCommPort.Init (ComStr,dwBaudRateBuf,0,1,8);   //sidd
			if (status) return status;

			status = m_theCommPort.Start ();
		}
	}

	return status;
}

BOOL DutIf_UartComm::BT_Send(PUCHAR pInfo, UINT len) 
{
	UartBridge_Command_Format_Header *pHearderBuf =NULL;
	char *TxBuf=NULL;
	DWORD *ptrD=NULL;
	int TotalLength=0;
	char RxBuf[100]={0};
	int Status=0;
	DWORD CheckSum=0;
 
     if(vg_IfSpy)
	 {
		 int i=0;

		 SpyLogRite("Hci Command\n");

		 for (i=0; i<len; i++)
			SpyLogRite("%02X ", *(pInfo+i));

		 SpyLogRite("\n");
	 }


    TotalLength = len+ sizeof(UartBridge_Command_Format_Header);
    TxBuf = (char *)malloc (TotalLength + SizeOfFCS);
	memset(TxBuf, 0, TotalLength + SizeOfFCS);
	memcpy(TxBuf+(sizeof(UartBridge_Command_Format_Header)/sizeof(char)), pInfo, len);

	pHearderBuf = (UartBridge_Command_Format_Header*)TxBuf;
	pHearderBuf->Header.Type = 0x0003;
	pHearderBuf->Header.Length = len+ sizeof(Bridge_Command_Format_Header);

	pHearderBuf->Signature=UART_SIGNATURE;
	pHearderBuf->UartLength = len+ sizeof(Bridge_Command_Format_Header);
		//FSC
//	CheckSum= crc32(pInfo, len);
	CheckSum= crc32((unsigned char*)&pHearderBuf->Header, len+ sizeof(Bridge_Command_Format_Header));
	
	ptrD = (DWORD*)(TxBuf+TotalLength);
	(*ptrD) = CheckSum;
//	pHearderBuf->FCS = CheckSum;
 	if (DebugLog)
		DebugLogRite("CRC:0x%08x\n", CheckSum);
//;	return SocketSendTo((char*) TxBuf, TotalLength);
//	memcpy(TxBuf, (char*)pHearderBuf, sizeof(UartBridge_Command_Format_Header));
	TotalLength += SizeOfFCS;
     if(vg_IfSpy)
	 {
		 int i=0;
		 SpyLogRite("UART Command\n");

		 for (i=0; i<TotalLength; i++)
			SpyLogRite("%02X ", (char)(0xFF & (*(TxBuf+i))));

		 SpyLogRite("\n");
	 }

	Status = m_theCommPort.Write((const char* )TxBuf, TotalLength);
//	Status = Write((const char* )TxBuf, TotalLength);
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
		 int i=0;$
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

	//; return DutIf_UartComm::query(OID, pInfo, len);
}


BOOL DutIf_UartComm::BT_Receive(PUCHAR pInfo, UINT *pLen) 
{
	Bridge_Command_Format_Header *pHearderBuf = NULL;
	char *TxBuf=NULL, *ptr=NULL;
	int Length=0;
	int Status=0;
	char tempChar[sizeof(HCI_EVENT_STRUCT)+255]={0};
 	std::string szData ;
	std::string szDataEx;
 
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
 
//		HRESULT			ReadAvailable(std::string& data);
//	Length =m_theCommPort.ReadAvailable((std::string)tempChar);
	Status =m_theCommPort.Read_N(szData, 4, vg_maxWait);
		if (Status) 	
	{
 		return Status;
	}

/*		printf("%s (%d)\n"
		"%02X %02X %02X %02X %02X %02X\n"
			, szData.c_str(), szData.length(),
		szData.c_str()[0], szData.c_str()[1],szData.c_str()[2],
		szData.c_str()[3],szData.c_str()[4],szData.c_str()[5]
		);
*/
	if(( 4 == szData.length()) && (0x55 == szData.c_str()[0]) && (0x55 == szData.c_str()[1]) ) 
	{
		Length =  ((szData.c_str()[2])&0xFF)+((szData.c_str()[3]&0xFF)>>8);
//		memset(tempChar, 0, sizeof(tempChar));
		Status =m_theCommPort.Read_N(szDataEx, Length, vg_maxWait);
// 	Status = SocketRcvFrom((char*) &HearderBuf, sizeof(UartBridge_Command_Format_Header), 5000/1000);
// 	Status = SocketRcvFrom((char*) tempChar, sizeof(HCI_EVENT_STRUCT), 5000/1000);
	memcpy(tempChar, (szDataEx.c_str()), Length);
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
	}
  // Fill out the event structure  

    return Status;

	//; return DutIf_UartComm::query(OID, pInfo, len);
}

int DutIf_UartComm::RxHciDataEvent(PUCHAR pData, ULONG BufLength)
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
int	 DutIf_UartComm::WaitForEvent(USHORT CmdGroup, USHORT CmdCode, USHORT TargetEvent, 
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

BOOL DutIf_UartComm::SendCommandToController(UCHAR CmdGroup, 
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

BOOL DutIf_UartComm::SendHciCommand(USHORT CmdGroup, USHORT CmdCode, USHORT ExpEvent, 
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



BOOL DutIf_UartComm::Bt_query(ULONG OID, PUCHAR Buf, UINT BufSize)
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


BOOL DutIf_UartComm::queryHci(ULONG OID, PUCHAR Buf, UINT BufSize, 
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
DWORD DutIf_UartComm::GetErrorLast(void)
{
	return -1; // TBD
}
// Close The Adapter
int  DutIf_UartComm::CloseDevice( )
{
	m_theCommPort.Stop();
	m_theCommPort.UnInit();	
    return 0;	
}

BOOL DutIf_UartComm::GetDebugLogFlag(void)
{
	 return DebugLog;
}
