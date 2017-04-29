/** @file UartIf.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */
 
#include "UartIf.h"
#include "../Common/UartErrCode.h"
#include "../../../../DutCommon/Clss_os.h"
 
 
#ifdef _LINUX_
long GetTickCount()
{
	struct tms tm;
#ifndef _LINUX_FEDORA_
	return times(&tm)*1000/(CLK_TCK);
#else // #ifndef _LINUX_FEDORA_
	Sleep(1);
	return time(NULL)*1000;
#endif // #ifndef _LINUX_FEDORA_
}
#endif //#ifdef _LINUX_

static int set_speed(int fd, struct termios *ti, int speed);



int CheckInQ(HANDLE hPort)
{
#ifdef _LINUX_
	int Len_InQ=0;

	ioctl(hPort, FIONREAD, &Len_InQ);
//printf("CheckInQ %X %d\n",hPort, Len_InQ); 
	return Len_InQ;

#else //#ifdef _LINUX_
	COMSTAT ComStat={0};
//	LPDWORD lpErrors={0};
	if ((hPort < 0) || (!ClearCommError(hPort, NULL, &ComStat)))
		return 0;
	else
		return ComStat.cbInQue;
#endif //#ifdef _LINUX_
}
 
int ReadUartFile(HANDLE hPort, unsigned char *pBuf, int Len, unsigned long *pRd)
{
#ifdef _LINUX_
int rd=0;
	if(pRd) 
		*pRd = rd;

	if(CheckInQ(hPort) < Len) return rd;

	rd=read (hPort, pBuf, Len);

	if(pRd) 
		*pRd = rd;
	 
	return rd;

#else //#ifdef _LINUX_
 		return ReadFile(hPort, pBuf, Len, pRd, NULL);
#endif //#ifdef _LINUX_
}

int WriteUartFile(HANDLE hPort, unsigned char *pBuf, int Len, unsigned long *pRd)
{
#ifdef _LINUX_
	int wt=0;
	wt = write (hPort, pBuf, Len);
		
	if(pRd) 
		*pRd = wt;
		
//printf("WriteUartFile %d %d\n",Len, wt); 
//{
//	int i=0;
//	for (i=0; i<Len;i++)
//		printf("%02X ",pBuf[i]);
//	printf("\n"); 
//}	 
	return wt;

#else //#ifdef _LINUX_
	return WriteFile(hPort, pBuf, Len, pRd, NULL);
#endif //#ifdef _LINUX_
}


int ReadBytes(HANDLE hPort, unsigned char *pBuf, int Len, unsigned long *pRd)
{
	int	status =0, index=0, cnt=0;
	unsigned long Rd=0;
	ULONG StartTimer=0;

if(pRd) (*pRd)=0;

	StartTimer = GetTickCount();

	for(index=0; index <Len; index+=Rd)
	{
 		status=ReadUartFile(hPort, pBuf+index, 1, &Rd);
//		status=ReadFile(hPort, pBuf+index, 1, &Rd, NULL);
		
		if(pRd) (*pRd) +=Rd;

		if(Rd) 
		{
			StartTimer = GetTickCount();
			cnt=0;
		}
		else 
			cnt++;

//		if(Rd) printf("0x%02X ", *(pBuf+index));
		
		if(status==0)
		{
#ifdef _LINUX_
			status=errno;
#else //#ifdef _LINUX_
 			status = GetLastError();//ClearCommError(nPortID, NULL, NULL);
#endif //#ifdef _LINUX_
//			break;
		}

//		printf("diff Time %d %X %d\n", 
//			GetTickCount()-StartTimer, hPort, nPortTimer);
//		if(cnt>10) 
		if((GetTickCount()-StartTimer) > nPortTimer)
		{	
//		printf("diff Time %d (%ld %ld)\n", GetTickCount()-StartTimer, GetTickCount(), StartTimer);
			status = ERROR_GET_EVENT_TO;
			break;
		}
	}
//	printf("\n");
	return status;

}


int ClearRxBuf(HANDLE hPort)
{
 	int Status=0;
//	ULONG StartTimer;
	UCHAR Buffer[2]={0};
	DWORD ByteRead=0;

	if (hPort==0 ||hPort == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

//	StartTimer = GetTickCount();
	ByteRead = CheckInQ( hPort);

	while (ByteRead>0) // do
	{
//		if (GetTickCount()-StartTimer >= nTimer)
//		{
//			return ERROR_GET_EVENT_TO;
//		}

//		Status=ReadFile(hPort, Buffer, 1, &ByteRead,0);
		Status=ReadBytes(hPort, Buffer, 1, &ByteRead); 
		if (Status==0)
		{
			return ERROR_ACCESS_COM_PORT;
		}
//		else
//		{
			ByteRead = CheckInQ( hPort);
//			continue;
//		}
	}//	while (ByteRead>0);


	LastIsTO=0;

	return 0;
}
 
#ifdef _LINUX_FEDORA_
int SetUartConfig(HANDLE hPort, DWORD BaudRate, DWORD flowControl)
{
 	static	struct termios config={0}; 
	int error=0;
 
 	tcflush(hPort, TCIOFLUSH);

	if (tcgetattr(hPort, &config) < 0) {
		perror("Can't get port settings");
		return ERROR_ERROR;
	}

	cfmakeraw(&config);

	config.c_cflag |= CLOCAL;
	if (flowControl)
		config.c_cflag |= CRTSCTS;
	else
		config.c_cflag &= ~CRTSCTS;

	if (tcsetattr(hPort, TCSANOW, &config) < 0) {
		perror("Can't set port settings");
		return ERROR_ERROR;
	}

	// printf("Set initial UART baud rate ...\r\n");
	if (set_speed(hPort, &config, BaudRate) < 0) {
		perror("Can't set initial baud rate");
		return ERROR_ERROR;
	}

// printf("Before tcflush...\r\n");
	tcflush(hPort, TCIOFLUSH);
// printf("After tcflush...\r\n");

return  error;
}
#else //#ifdef _LINUX_FEDORA_
int SetUartConfig(HANDLE hPort, DWORD BaudRate, DWORD flowControl)
{

#ifdef _LINUX_
	struct termios config={0}; 
	int error=0;

 	if(!hPort) return ERROR_INVALID_HANDLE;
	 
	error = tcgetattr(hPort,&config);
	if(error == -1) return error;

	config.c_ispeed = BaudRate; //CBR_115200;    // Specify buad rate of communicaiton.
	config.c_ospeed = BaudRate; //CBR_115200;    // Specify buad rate of communicaiton.

	config.c_cflag |= (CLOCAL | CREAD);    

	config.c_cflag &= (~(PARENB | CSTOPB | CSIZE));    
	config.c_cflag |= (CS8);    
 	
	config.c_cflag &= (~(CRTSCTS|CCTS_OFLOW|CRTSCTS|CRTS_IFLOW|MDMBUF));
//?	config.fDtrControl = flowControl?DTR_CONTROL_ENABLE: DTR_CONTROL_DISABLE;
	config.c_cflag |= flowControl?(CRTS_IFLOW):0;
 	config.c_cflag |= flowControl?(CRTSCTS|CCTS_OFLOW):0;

	error = tcsetattr(hPort,TCSANOW, &config);
	if(error == -1) return error;

 	return ERROR_SUCCESS;

#else //#ifdef _LINUX_

	DCB config={0}; 

 	if(!hPort) return ERROR_INVALID_HANDLE;
	 
	GetCommState(hPort,&config);
	config.BaudRate = BaudRate; //CBR_115200;    // Specify buad rate of communicaiton.
	config.StopBits = ONESTOPBIT;    // Specify stopbit of communication.
	config.Parity = NOPARITY;        // Specify parity of communication.
	config.ByteSize = 8;    // Specify  byte of size of communication.

	
//	config.fDtrControl = flowControl?DTR_CONTROL_ENABLE: DTR_CONTROL_DISABLE;
	config.fRtsControl = flowControl?RTS_CONTROL_HANDSHAKE:RTS_CONTROL_DISABLE;
//	config.fRtsControl = FlowContrlMode?RTS_CONTROL_ENABLE:RTS_CONTROL_DISABLE;
	config.fOutxCtsFlow = flowControl?TRUE:FALSE;
	if(!SetCommState(hPort,&config)) return GetLastError();
	else
		return ERROR_SUCCESS;
#endif //#ifdef _LINUX_

}
#endif //#ifdef _LINUX_FEDORA_



#ifdef _LINUX_FEDORA_
static int uart_speed(int s)
{
	switch (s) {
	case 9600:
		return B9600;
	case 19200:
		return B19200;
	case 38400:
		return B38400;
	case 57600:
		return B57600;
	case 115200:
		return B115200;
	case 230400:
		return B230400;
	case 460800:
		return B460800;
	case 500000:
		return B500000;
	case 576000:
		return B576000;
	case 921600:
		return B921600;
	case 1000000:
		return B1000000;
	case 1152000:
		return B1152000;
	case 1500000:
		return B1500000;
	default:
		return B57600;
	}
}

static int set_speed(int fd, struct termios *ti, int speed)
{
	cfsetospeed(ti, uart_speed(speed));
	return tcsetattr(fd, TCSANOW, ti);
}
 
HANDLE InitUartPort(char *szPort, DWORD BaudRate, DWORD flowControl, bool OverLapped)
{

	static struct termios ti;
	HANDLE fd;
	int error=0;

	// printf("Now initializing UART %s...\n", szPort);
	fd = open(szPort, O_RDWR | O_NOCTTY);
	if (fd < 0) {
		perror("Can't open serial port");
		return -1;
	}

	error = SetUartConfig(fd, BaudRate, flowControl);
	
	return fd;

}
#else //#ifdef _LINUX_FEDORA_
 
HANDLE InitUartPort(char *szPort, DWORD BaudRate, DWORD flowControl, bool OverLapped)
{ 

	HANDLE hPort=0;
	

#ifdef _LINUX_
	char WholePortName[256]=""; 
	int error=0;
 	struct termios config={0};  
 
	hPort = open(strcat(WholePortName, szPort),	// Specify port device: default "COM1"
			 O_RDWR|O_NOCTTY|O_NONBLOCK|O_NDELAY); //_A_NORMAL);//
       
	if (hPort == INVALID_HANDLE_VALUE || hPort==NULL)        // Was the device opened?
    {
  		hPort = NULL;
		return INVALID_HANDLE_VALUE;
    }

	error = ioctl(hPort, TIOCEXCL);
	if(error == -1) return  error;


 	error = fcntl(hPort, F_SETFL, 0);
	if(error == -1) return  error;

#else //#ifdef _LINUX_

	DCB config={0}; 

	char WholePortName[256]="\\\\.\\";
	COMMTIMEOUTS TimeOut={0};

 	hPort = CreateFile(strcat(WholePortName, szPort),	// Specify port device: default "COM1"
			 GENERIC_READ | GENERIC_WRITE,				// Specify mode that open device.
			 0,											// the devide isn't shared.
			 NULL,										// the object gets a default security.
			 OPEN_EXISTING,								// Specify which action to take on file. 
			 (OverLapped?FILE_FLAG_OVERLAPPED:0),		// default.
			 NULL);

    if (hPort == INVALID_HANDLE_VALUE || hPort==NULL)        // Was the device opened?
    {
  		hPort = NULL;
		return INVALID_HANDLE_VALUE;
    }

#endif //#ifdef _LINUX_

   
		// set configuration
		SetUartConfig(hPort, BaudRate, flowControl);

 		SetUartTimeoutTimer( hPort,  nPortTimer);
		ClearRxBuf(hPort);
  
 
	return hPort;

}
#endif //#ifdef _LINUX_FEDORA_
BOOL CloseUartPort(HANDLE hPort)
{
	int error=0;

#ifdef _LINUX_

	if(hPort)
	{
		error = close (hPort);
		hPort=(HANDLE)NULL;
	}
	return error;

#else //#ifdef _LINUX_

	if(hPort) 
	{
		PurgeComm(hPort, PURGE_RXCLEAR|PURGE_RXABORT|PURGE_TXABORT|PURGE_TXCLEAR);     // clean buffers
		error = !CloseHandle(hPort);
		hPort=NULL;
	}
#endif //#ifdef _LINUX_

	return error;

}



void SetUartTimeoutTimer(HANDLE hPort, ULONG newTimer)
{
#ifdef _LINUX_
	;
#else //#ifdef _LINUX_

	COMMTIMEOUTS TimeOut={0};

	nPortTimer = newTimer;
	if(hPort)
	{
		GetCommTimeouts(hPort, &TimeOut);
		TimeOut.ReadIntervalTimeout = 0;// MAXDWORD;
		TimeOut.ReadTotalTimeoutConstant = nPortTimer;
		TimeOut.WriteTotalTimeoutConstant = nPortTimer;
		SetCommTimeouts(hPort, &TimeOut);
	}

#endif //#ifdef _LINUX_

}

ULONG GetUartTimeoutTimer(HANDLE hPort)
{
#ifdef _LINUX_
	;
#else //#ifdef _LINUX_
 	COMMTIMEOUTS TimeOut={0};
	if(hPort)
	{
		GetCommTimeouts(hPort, &TimeOut);
 		nPortTimer = TimeOut.WriteTotalTimeoutConstant; 
	}
		return nPortTimer;
#endif //#ifdef _LINUX_

}
