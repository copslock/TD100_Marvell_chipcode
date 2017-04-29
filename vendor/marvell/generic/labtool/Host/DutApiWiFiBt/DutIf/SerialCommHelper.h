/** @file SerialCommHelper.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

//////////////////////////////////////////////////////////////////////

#pragma once
#include "time.h"
#include <string>
#include <tchar.h>

#define TIMEOUT_1S	1000

typedef enum tagSERIAL_STATE
{
	SS_Unknown,
	SS_UnInit,
	SS_Init,
	SS_Started ,
	SS_Stopped ,
	
} SERIAL_STATE;

class CSerialCommHelper  
{
private:
	SERIAL_STATE	m_eState;
	HANDLE	m_hCommPort;
	void	InvalidateHandle(HANDLE& hHandle );
	void	CloseAndCleanHandle(HANDLE& hHandle) ;
	
	CRITICAL_SECTION m_csLock;

public:
	CSerialCommHelper();
	virtual ~CSerialCommHelper();

	int debugLog;
	inline void		InitLock()			{InitializeCriticalSection (&m_csLock );}
	inline void		DelLock()				{DeleteCriticalSection (&m_csLock );}

	HRESULT			ComGetBuffer	(std::string& data, DWORD *alCountRead, long alCount, long alTimeOut);
	HRESULT			Read_N		(std::string& data, long alCount, long alTimeOut);
	HRESULT			ReadAvailable(std::string& data);
	HRESULT			Write (const char* data, DWORD dwSize);
	HRESULT			Init(std::string szPortName= _T("COM1"), DWORD dwBaudRate = 9600, BYTE byParity = 0, BYTE byStopBits = 1, BYTE byByteSize  = 8);
	HRESULT			Start();
	HRESULT			Stop();
	HRESULT			UnInit();

	//-- helper fn.
 	HRESULT  CanProcess();
};


