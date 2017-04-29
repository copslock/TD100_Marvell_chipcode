/** @file SerialCommHelper.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

//////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
#include "SerialCommHelper.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#include "../../DutCommon/utilities.h" 
extern int DebugLogRite(const char*,...);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern int DebugLogRite(const char*,...);

void CSerialCommHelper::InvalidateHandle(HANDLE& hHandle )
{
	hHandle = INVALID_HANDLE_VALUE;
}

void CSerialCommHelper::CloseAndCleanHandle(HANDLE& hHandle)
{
	BOOL abRet  = CloseHandle( hHandle ) ;
	if ( !abRet )
	{
		DebugLogRite ( "CSerialCommHelper : Failed to open Close Handle %d :Last Error: %d\n",hHandle,GetLastError());
	}
	InvalidateHandle ( hHandle );
}

CSerialCommHelper::CSerialCommHelper()
{
	InvalidateHandle( m_hCommPort );
 	
	InitLock();
	m_eState = SS_UnInit;
}

CSerialCommHelper::~CSerialCommHelper()
{
	m_eState = SS_Unknown;
	DelLock();
}

HRESULT CSerialCommHelper::Init(std::string szPortName, DWORD dwBaudRate, BYTE byParity, BYTE byStopBits, BYTE byByteSize)
{
	HRESULT hr = S_OK;
	try
	{
		//open the COM Port
		m_hCommPort = ::CreateFile(szPortName.c_str (),
									GENERIC_READ|GENERIC_WRITE,//access ( read and write)
									0,	//(share) 0:cannot share the COM port						
									0,	//security  (None)				
									OPEN_EXISTING,// creation : open_existing
    								0,  //FILE_FLAG_OVERLAPPED,// we want overlapped operation
									0// no templates file for COM port...
									);
		if ( m_hCommPort == INVALID_HANDLE_VALUE )
		{
			DebugLogRite( "CSerialCommHelper : Failed to open COM Port \tReason: %d\n",GetLastError());
			return E_FAIL;
		}

		if (debugLog)
			DebugLogRite( "CSerialCommHelper : COM port opened successfully\n" );
		
		//now start to read but first we need to set the COM port settings and the timeouts
		if (! ::SetCommMask(m_hCommPort,EV_RXCHAR|EV_TXEMPTY) )
		{
			DebugLogRite( "CSerialCommHelper : Failed to Set Comm Mask Reason: %d\n",GetLastError());
			return E_FAIL;
		}
		if (debugLog)
			DebugLogRite( "CSerialCommHelper : SetCommMask() success\n");
		
		//now we need to set baud rate etc,
		DCB dcb = {0};
		
		dcb.DCBlength = sizeof(DCB);
		
		if (!::GetCommState (m_hCommPort,&dcb))
		{
			DebugLogRite( "CSerialCommHelper : Failed to Get Comm State Reason: %d\n",GetLastError());
			return E_FAIL;
		}
		
		dcb.BaudRate	= dwBaudRate;
		dcb.ByteSize	= byByteSize;
		dcb.Parity		= byParity;
		if ( byStopBits == 1 )
			dcb.StopBits	= ONESTOPBIT;
		else if (byStopBits == 2 ) 
			dcb.StopBits	= TWOSTOPBITS;
		else 
			dcb.StopBits	= ONE5STOPBITS;

		dcb.fDsrSensitivity = 0;
		//;dcb.fDtrControl = DTR_CONTROL_ENABLE;   //;sidd
		//;dcb.fRtsControl = RTS_CONTROL_ENABLE;   //;sidd
		dcb.fDtrControl = DTR_CONTROL_DISABLE;
		dcb.fOutxDsrFlow = 0;

		if (!::SetCommState (m_hCommPort,&dcb))
		{
			DebugLogRite( "CSerialCommHelper : Failed to Set Comm State Reason: %d\n",GetLastError());
			return E_FAIL;
		}
		if (debugLog)
			DebugLogRite( "CSerialCommHelper : Current Settings, (Baud Rate %d; Parity %d; Byte Size %d; Stop Bits %d\n", dcb.BaudRate, 
								dcb.Parity,dcb.ByteSize,dcb.StopBits);
		
		//now set the timeouts ( we control the timeout overselves using WaitForXXX()
		COMMTIMEOUTS timeouts;
		
		timeouts.ReadIntervalTimeout			= MAXDWORD; 
		timeouts.ReadTotalTimeoutMultiplier		= 0;
		timeouts.ReadTotalTimeoutConstant		= 0;
		timeouts.WriteTotalTimeoutMultiplier	= 0;
		timeouts.WriteTotalTimeoutConstant		= 0;
		
		if (!SetCommTimeouts(m_hCommPort, &timeouts))
		{
			DebugLogRite( "CSerialCommHelper :  Error setting time-outs. %d\n",GetLastError());
			return E_FAIL;
		}
	}
	catch(...)
	{
		hr = E_FAIL;
	}
	
	if ( SUCCEEDED(hr) ) 
	{
		m_eState = SS_Init;
	}
 	
	return hr;
}	
 
HRESULT CSerialCommHelper::Start()
{
	m_eState = SS_Started;
	return S_OK;
}

HRESULT CSerialCommHelper::Stop()
{
	m_eState = SS_Stopped;
	return S_OK;
}

HRESULT CSerialCommHelper::UnInit()
{
	HRESULT hr = S_OK;
	try
	{
		CloseAndCleanHandle( m_hCommPort );
	}
	catch(...)
	{
		hr = E_FAIL;
	}
	if ( SUCCEEDED(hr)) 
		m_eState = SS_UnInit;
		
	return hr;
}

HRESULT  CSerialCommHelper::ComGetBuffer(std::string& data, DWORD* alCountRead, long alCount, long alTimeOut)
{
    HRESULT hr=S_OK;
    clock_t StartTime={0}, NowTime={0};
    DWORD comError;
    COMSTAT comStat;
    int iRet=0;
	char* buffer;
    
    //read buffer till empty or time out
    StartTime = clock();
    do
    {
        iRet = ClearCommError(m_hCommPort, &comError, &comStat);
        if (!iRet)
            return GetLastError();
        NowTime = clock();
    }  while ((NowTime-StartTime) < alTimeOut && comStat.cbInQue < alCount);
    
    //buffer empty
    if (!comStat.cbInQue)
        return S_FALSE;
     
    //Get buffer
	buffer = (char*) malloc(alCount);
	if (!buffer)
		return S_FALSE;

    iRet = ReadFile(m_hCommPort, buffer, alCount, alCountRead, NULL);
    if(!iRet)
        return GetLastError();
        
	data.erase();
	data.append(buffer, (int)*alCountRead);

	if(buffer)
		free (buffer);

    return S_OK;
}

HRESULT  CSerialCommHelper::CanProcess ()
{
    switch ( m_eState  ) 
    {
	    case SS_Unknown	://ASSERT(0);
		    return E_FAIL;
	    case SS_UnInit	:
	        return E_FAIL;
	    case SS_Started :
	        return S_OK;
	    case SS_Init		:
	    case SS_Stopped :
		    return E_FAIL;
	    default:;	

    }	
    
	return E_FAIL;
}

HRESULT CSerialCommHelper::Write(const char* data, DWORD dwSize)
{
	HRESULT hr = CanProcess();
	if ( FAILED(hr)) return hr;
	int iRet = 0 ;
	DWORD dwBytesWritten = 0;
	COMSTAT comStat;
	DWORD comError;
	clock_t StartTime={0}, NowTime={0};

	//purge the comm port before send data
	iRet = PurgeComm(m_hCommPort, PURGE_RXCLEAR);
	if (!iRet)
		return GetLastError();

	iRet = PurgeComm(m_hCommPort, PURGE_TXCLEAR);
	if (!iRet)
		return GetLastError();

	//send data
	iRet = WriteFile (m_hCommPort, data, dwSize, &dwBytesWritten, NULL);
	if ( !iRet )
		return GetLastError();
    
    //check whether the data has been sent
	StartTime = clock();
    do
    {
        iRet = ClearCommError(m_hCommPort, &comError, &comStat);
        if (!iRet)
            return GetLastError();

		NowTime = clock();
    } while ((NowTime - StartTime) < TIMEOUT_1S && comStat.cbOutQue > 0); //in milliseconds
    
	std::string szData(data);
	if(debugLog)
		DebugLogRite("RCSerial:Writing:{%s} len:{%d}\n",(szData).c_str(),szData.size());
	
	if (comStat.cbOutQue)
	    return S_FALSE;
	    
	return S_OK;
}

HRESULT CSerialCommHelper::Read_N(std::string& data, long alCount, long alTimeOut)
{
	HRESULT hr = CanProcess();
	
	if ( FAILED(hr)) 
	{
		return hr;
	}
	
	if (debugLog)
		DebugLogRite("CSerialCommHelper : CSerialCommHelper : Read_N called for %d bytes\n",alCount);
	try
	{
		std::string szTmp ;
		szTmp.erase();

		if (debugLog)
			DebugLogRite("CSerialCommHelper : CSerialCommHelper : Read_N (%d) locking the queue\n",alCount);
		
		DWORD alLocal=0;
		hr = ComGetBuffer(szTmp, &alLocal, alCount, alTimeOut);
		if (hr) return hr;

		data = szTmp;
	}
	catch(...)
	{
		DebugLogRite("CSerialCommHelper Unhandled exception\n");
	}
	
	return hr;
}

/*-----------------------------------------------------------------------
	-- Reads all the data that is available in the local buffer.. 
	does NOT make any blocking calls in case the local buffer is empty
-----------------------------------------------------------------------*/
HRESULT CSerialCommHelper::ReadAvailable(std::string& data)
{
	HRESULT hr = CanProcess();
	if ( FAILED(hr)) return hr;
	
	try
	{
		DWORD alCount=256, alCountRead=0;
		std::string szTemp;
		hr = ComGetBuffer(szTemp, &alCountRead, alCount, 0);

		data = szTemp;
	}
	catch(...)
	{
		DebugLogRite("CSerialCommHelper Unhandled exception in ReadAvailable()\n");
		hr = E_FAIL;
	}

	return hr;
}





