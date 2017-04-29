#include "StdAfx.h"
#include "tchar.h"

extern int WindowsVersion;
extern HANDLE hValidDevice;
extern NDIS_802_11_MAC_ADDRESS MacAddr;
extern TCHAR RegistryPath[ADAPTER_REGPATH_BUF_SIZE];
extern TCHAR AdapterName[ADAPTER_REGPATH_BUF_SIZE];
extern TCHAR RegNetTransPath[ADAPTER_REGPATH_BUF_SIZE];

//=====================================================================
//	Get Registry Path
//=====================================================================
int GetRegistryPath()
{
 	// Windows Version
	if (WindowsVersion == -1)
	{
		if ((WindowsVersion=GetWindowsVersion())==-1)
		{
  			return STATUS_FAILURE;
		}
	}

	if (WindowsVersion == Mrv8000xOSWindows) // win 98SE, ME
	{
		return GetRegistryPath98(); 
	}
	else // win NT, 2000, XP
	{
 		return GetRegistryPathNT5(); 
	}
}

//=====================================================================
//	Get Registry Path, for windows 98SE, ME
//=====================================================================
int GetRegistryPath98()
{
	HKEY	hKey, hSubKey;
	HKEY	hKey2, hSubKey2;
 	int		nIndex=0;
	DWORD	dwType, dwLen;
	TCHAR	szRegKey[100];
	TCHAR	szTmp[100];
	LPTSTR  pTmp;
	FILETIME LastWriteTime;

	// get hkey_dyn_data\config Manager\enum
	if (RegOpenKeyEx(
					HKEY_DYN_DATA,
					REGKEY_CFG_MGR_ENUM, 
					0,							// Options (Reserved)
					KEY_READ,				// Security Access Mask
					&hKey
					) != ERROR_SUCCESS )
	{
		return STATUS_FAILURE;
	}
	
	nIndex = 0;

	while (1)
	{
		dwLen = 100;

		// enum key
		if (RegEnumKeyEx(hKey, 
                     nIndex, 
                     szRegKey, 
                     &dwLen, 
                     NULL, 
                     NULL, 
                     NULL, 
                     &LastWriteTime)!=ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return STATUS_FAILURE;
		}
		
		// open enum subkey
		if (RegOpenKeyEx(hKey,						// Handle to key
						szRegKey,					// Value name
						0	,						// Reserved 
						KEY_READ,					// Security Access Mask
						&hSubKey					// Buffer size
						) != ERROR_SUCCESS)
		{
			nIndex++;
			continue;
		}

		// query key value "HardWareKey"
		dwLen = 100;
		
		if (RegQueryValueEx(	
						hSubKey,				  // Handle to key
						REGKEY_HW_KEY,			 // Value name
						0	,					  // Reserved 
						&dwType,				  // Value type
						(LPBYTE)szTmp,		  // Pointer to value data buffer
						&dwLen				      // Buffer size
						)!= ERROR_SUCCESS)
		{
			nIndex++;
			RegCloseKey(hSubKey);
			continue;
		}
		else
		{
			// if device id found in the key, get key
			_tcsupr(szTmp);
			if (_tcsstr(szTmp, MARVELL_WLAN_DEVICE)!=0 
				|| _tcsstr(szTmp, MARVELL_WLAN_DEVICE2)!=0
#ifdef CF83XX
				|| _tcsstr(szTmp, MARVELL_WLAN_DEVICE3)!=0 
				|| _tcsstr(szTmp, MARVELL_WLAN_DEVICE5)!=0
#endif
#ifdef IEEE_802_11a
				|| _tcsstr(szTmp, MARVELL_WLAN_DEVICE4)!=0
#endif
				)
			{
				if (RegOpenKeyEx(
					HKEY_LOCAL_MACHINE,
					REGKEY_ENUM, 
					0,							// Options (Reserved)
					KEY_READ,				// Security Access Mask
					&hKey2
					) == ERROR_SUCCESS )
				{
					if (RegOpenKeyEx(	
						hKey2,						// Handle to key
						szTmp,						// Value name
						0	,						// Reserved 
						KEY_READ,					// Security Access Mask
						&hSubKey2					// Buffer size
						) == ERROR_SUCCESS)
					{
						dwLen = 100;
						 
						if (RegQueryValueEx(	
								hSubKey2,				  // Handle to key
								REGKEY_DRIVER,			  // Value name
								0	,					  // Reserved 
								&dwType,				  // Value type
								(LPBYTE)szRegKey,		      // Pointer to value data buffer
								&dwLen				      // Buffer size
								)== ERROR_SUCCESS)
						{
							_tcscpy(RegistryPath, REGSTR_CLASS_NET_98);	
							_tcscat(RegistryPath, szRegKey);
							
							if (GetNetTransRegPath9x(szTmp, 100)==STATUS_SUCCESS)
							{
								_tcscpy(RegNetTransPath, REGSTR_CLASS_NET_98);
								_tcscat(RegNetTransPath, szTmp);
							}

							if ((pTmp=_tcsstr(szRegKey, _T("\\")))!=NULL)
							{
								_tcscpy(AdapterName, pTmp+1);
							}

							RegCloseKey(hSubKey2);
							RegCloseKey(hKey2);
							RegCloseKey(hSubKey);
							RegCloseKey(hKey);
							return STATUS_SUCCESS;
						}
						 
						RegCloseKey(hSubKey2);
					}

					RegCloseKey(hKey2);	
				}
			}
		}

		RegCloseKey(hSubKey);
		nIndex++;
	}

	RegCloseKey(hKey);
	
	return STATUS_FAILURE;
}

//=====================================================================
//	Get Registry Path, for windows NT, 2K, XP
//=====================================================================
int GetRegistryPathNT5() 
{
	HKEY	hKey, hSubKey;
	HANDLE  hFile;
	int		nIndex=0;
	BOOL	bLoop = TRUE;
	DWORD	dwType, dwLen;
	TCHAR	szRegKey[ADAPTER_REGPATH_BUF_SIZE];
	TCHAR	szAdapterName[200], szTmp[100];
	FILETIME LastWriteTime;

	// Open Registry Key
	_tcscpy(szRegKey, REGSTR_CLASS_NET_NT5);	
//	_tcscpy(RegistryPath, REGSTR_CLASS_NET_NT5);	
	
	if (RegOpenKeyEx(
					HKEY_LOCAL_MACHINE,
					szRegKey, 
					0,							// Options (Reserved)
					KEY_READ,				// Security Access Mask
					&hKey
					) != ERROR_SUCCESS )
	{
		return STATUS_FAILURE;
	}
	
	// Net Class found, enumerates subkey to find the one for Marvell 802.11 NIC
	nIndex=0;

	while (1)
	{
		dwLen = 1024;

		if (RegEnumKeyEx(hKey, 
                     nIndex, 
                     szRegKey, 
                     &dwLen, 
                     NULL, 
                     NULL, 
                     NULL, 
                     &LastWriteTime)!=ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return STATUS_FAILURE;
		}
      
		
		if (RegOpenKeyEx(	
						hKey,						// Handle to key
						szRegKey,					// Value name
						0	,						// Reserved 
						KEY_READ,					// Security Access Mask
						&hSubKey					// Buffer size
						) != ERROR_SUCCESS)
		{
			nIndex++;
			continue;
		}

		dwLen = 100;
		
		if (RegQueryValueEx(	
						hSubKey,				  // Handle to key
						REGKEY_COMPONENT_ID,	  // Value name
						0	,					  // Reserved 
						&dwType,				  // Value type
						(LPBYTE)szTmp,		  // Pointer to value data buffer
						&dwLen				      // Buffer size
						)!= ERROR_SUCCESS)
		{
			nIndex++;
			RegCloseKey(hSubKey);
			continue;
		}
		else
		{
			_tcsupr(szTmp);
			if (_tcsstr(szTmp, MARVELL_WLAN_DEVICE)!=0 
				|| _tcsstr(szTmp, MARVELL_WLAN_DEVICE2)!=0
#ifdef CF83XX
				|| _tcsstr(szTmp, MARVELL_WLAN_DEVICE3)!=0 
				|| _tcsstr(szTmp, MARVELL_WLAN_DEVICE5)!=0
#endif
#ifdef IEEE_802_11a
				|| _tcsstr(szTmp, MARVELL_WLAN_DEVICE4)!=0
#endif
				)
			{
				// make sure it is the one
				dwLen = 100;
				if (RegQueryValueEx(	
						hSubKey,				  // Handle to key
						REGKEY_SERVICE,			  // Value name
						0	,					  // Reserved 
						&dwType,				  // Value type
						(LPBYTE)szTmp,			// Pointer to value data buffer
						&dwLen				     // Buffer size
						) == ERROR_SUCCESS)
				{
					_tcscpy(szAdapterName, _T(NAME_PREFIX));
					_tcscat(szAdapterName, szTmp);
							
					hFile=CreateFile(szAdapterName,
									0,      // Device Query Access
									0, //FILE_SHARE_READ,
									NULL,
									OPEN_EXISTING,
									0,
									NULL
									);
					
					if (hFile != INVALID_HANDLE_VALUE && hFile != NULL)
					{
						_tcscpy(RegistryPath, REGSTR_CLASS_NET_NT5);	
						_tcscat(RegistryPath, _T("\\"));
						_tcscat(RegistryPath, szRegKey);
						_tcscpy(AdapterName, szTmp);
						RegCloseKey(hSubKey);
						CloseHandle(hFile);
						break;
					}
				}				
			}
		
			RegCloseKey(hSubKey);
			nIndex++;
		}
	}

	RegCloseKey(hKey);
	return STATUS_SUCCESS;
}

//========================================================================
//	 Get Registry Path of the NetTrans bindings of this adapter (98SE/ME)
//========================================================================
int GetNetTransRegPath9x(LPTSTR szRegKey, ULONG nSize)
{
	TCHAR szTmp[200];
 	TCHAR szBuf[100];
	int nIndex;
	DWORD dwLen, dwType;
	HKEY hKey=0, hSubKey=0;
 
	if (WindowsVersion == Mrv8000xOSWindows) // win 98SE, ME
	{
	 	_tcscpy(szTmp, REGSTR_ENUM);
		_tcscat(szTmp, szRegKey);
		_tcscat(szTmp, REGKEY_BINDINGS);
 		
		if (RegOpenKeyEx(
					HKEY_LOCAL_MACHINE,
					szTmp, 
					0,							// Options (Reserved)
					KEY_READ,				// Security Access Mask
					&hKey
					) != ERROR_SUCCESS )
		{
 			return STATUS_FAILURE;
		}
	
		nIndex = 0;

		while (1)
		{
			dwLen = 100;

			// enum value
			if (RegEnumValue(
					hKey,        // handle to key to query
					nIndex,      // index of value to query
					szBuf,		 // value buffer
					&dwLen,		 // size of value buffer
					NULL,
					&dwType,
					NULL,         // data buffer
					NULL      // size of data buffer
				)!=ERROR_SUCCESS)
			{
				break;
			}
			
			RegCloseKey(hKey);
 
			if (_tcsstr(szBuf, REGKEY_MSTCP)!=NULL)
			{
				if (RegOpenKeyEx(
					HKEY_LOCAL_MACHINE,
					REGSTR_ENUM_NETWORK, 
					0,							// Options (Reserved)
					KEY_READ,				// Security Access Mask
					&hKey
					) != ERROR_SUCCESS )
				{
					break;
				}
	
				// get driver
				if (RegOpenKeyEx(
					hKey,
					szBuf, 
					0,							// Options (Reserved)
					KEY_READ,				// Security Access Mask
					&hSubKey
					) == ERROR_SUCCESS )
				{
					dwLen = nSize;
					
					if (RegQueryValueEx(	
								hSubKey,				  // Handle to key
								REGKEY_DRIVER,			  // Value name
								0	,					  // Reserved 
								&dwType,				  // Value type
								(LPBYTE)szRegKey,		      // Pointer to value data buffer
								&dwLen				      // Buffer size
								)== ERROR_SUCCESS)
					{
						RegCloseKey(hSubKey);
						RegCloseKey(hKey);
						return STATUS_SUCCESS;
					}
				}

				break;
			}
			else
			{
				_tcscpy(szRegKey, REGSTR_NETWORK);
				_tcscat(szRegKey, szBuf);
				
				if (GetNetTransRegPath9x(szRegKey, nSize)==STATUS_SUCCESS)
				{
					return STATUS_SUCCESS;
				}
			}

			nIndex++;
		}
	}

	if (hSubKey)
	{
		RegCloseKey(hSubKey);	
	}
	
	if (hKey)
	{
		RegCloseKey(hKey);	
	}
	
	return STATUS_FAILURE;
}

//=====================================================================
//	Set Registry Info
//=====================================================================
int SetRegistryInfo(HANDLE hAdapter, LPCTSTR lpKeyName, LPCTSTR lpValue, DWORD nSize)
{
	HKEY hKey;
 
	if (RegistryPath[0]==0)
	{
		if (GetRegistryPath()==STATUS_FAILURE)
		{
			return STATUS_FAILURE;
		}
	}

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,	// Handle to open key 
					(char*)RegistryPath,	// Subkey name
					0,						// reserved
					KEY_WRITE,				// Security access mask
					&hKey					// Handle to open key
					) != ERROR_SUCCESS)
		
	{ 
		return STATUS_FAILURE;
	}

	if (RegSetValueEx(hKey,					// Handle to key
					lpKeyName,				// Value name
					0,						// Reserved
					REG_SZ,					// Value type
					(LPBYTE)lpValue,		// Pointer to value data fuffer
					nSize					// Buffer size
					) != ERROR_SUCCESS)
	{
 		RegCloseKey(hKey);
		return STATUS_FAILURE;
	}		
	
	RegCloseKey(hKey);

	return STATUS_SUCCESS;
}

//=====================================================================
//	Set Registry Info
//=====================================================================
int SetRegistryInfo(HANDLE hAdapter, LPCTSTR lpKeyName, DWORD dwData, BOOL bIsHex)
{
	HKEY hKey;
	TCHAR szData[20];

	if (RegistryPath[0]==0)
	{
		if (GetRegistryPath()==STATUS_FAILURE)
		{
			return STATUS_FAILURE;
		}
	}
	
	ZeroMemory(szData, 20);
	
	if (bIsHex)
	{
		_stprintf(szData, "%X", dwData);
	}
	else
	{
		_stprintf(szData, "%d", dwData);
	}

	if (RegistryPath[0]==0)
	{
		if (GetRegistryPath()==STATUS_FAILURE)
		{
			return STATUS_FAILURE;
		}
	}

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,	// Handle to open key 
					(char*)RegistryPath,	// Subkey name
					0,						// reserved
					KEY_WRITE,				// Security access mask
					&hKey					// Handle to open key
					) != ERROR_SUCCESS)
		
	{ 
		return STATUS_FAILURE;
	}

	if (RegSetValueEx(hKey,					// Handle to key
					lpKeyName,				// Value name
					0,						// Reserved
					REG_SZ,				// Value type
					(LPBYTE)szData,			// Pointer to value data fuffer
					_tcslen(szData)			// Buffer size
					) != ERROR_SUCCESS)
	{
 		RegCloseKey(hKey);
		return STATUS_FAILURE;
	}		
	
	RegCloseKey(hKey);

	return STATUS_SUCCESS;
}

//=====================================================================
//	Query Registry Info, Buffer
//=====================================================================
int QueryRegistryInfo(HANDLE hAdapter, LPCTSTR lpKeyName, LPBYTE lpData, DWORD &nSize)
{
	HKEY hKey;
	DWORD dwType, dwLen;
	TCHAR Buffer[100];
	
	if (RegistryPath[0]==0)
	{
		if (GetRegistryPath()==STATUS_FAILURE)
		{
			return STATUS_FAILURE;
		}
	}
		
	dwLen = 100;
	ZeroMemory(Buffer, 100*sizeof(TCHAR));

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,	// Handle to open key 
					RegistryPath,			// Subkey name
					0,					// reserved
					KEY_READ,			// Security access mask
					&hKey				// Handle to open key
					) != ERROR_SUCCESS)
		
	{
		return STATUS_FAILURE;
	}


	long nResult = RegQueryValueEx(	
						hKey,					  // Handle to key
						lpKeyName,				  // Value name
						0	,					  // Reserved 
						&dwType,				  // Value type
						(LPBYTE)Buffer,			  // Pointer to value data buffer
						&dwLen				      // Buffer size
						);
	
	if (nResult!= ERROR_SUCCESS || dwLen > nSize)
	{
 		RegCloseKey(hKey);
		return STATUS_FAILURE;
	}	
	
	CopyMemory(lpData, Buffer, dwLen); 
	nSize = dwLen;

	RegCloseKey(hKey);

	return STATUS_SUCCESS;

}

 
//=====================================================================
//	Query Registry Info, DWORD
//=====================================================================
int QueryRegistryInfo(HANDLE hAdapter, LPCTSTR lpKeyName, DWORD &dwData)
{
	HKEY hKey;
	DWORD dwType, dwLen;
	TCHAR Buffer[100];
	
	if (RegistryPath[0]==0)
	{
		if (GetRegistryPath()==STATUS_FAILURE)
		{
			return STATUS_FAILURE;
		}
	} 
		
	dwLen = 100;
	ZeroMemory(Buffer, 100*sizeof(TCHAR));

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,	// Handle to open key 
					RegistryPath,			// Subkey name
					0,					// reserved
					KEY_READ,			// Security access mask
					&hKey				// Handle to open key
					) != ERROR_SUCCESS)
		
	{
		return STATUS_FAILURE;
	}


	long nResult = RegQueryValueEx(	
						hKey,					  // Handle to key
						lpKeyName,				  // Value name
						0	,					  // Reserved 
						&dwType,				  // Value type
						(LPBYTE)Buffer,			  // Pointer to value data buffer
						&dwLen				      // Buffer size
						);
	
	if (nResult!= ERROR_SUCCESS)
	{
 		RegCloseKey(hKey);
		return STATUS_FAILURE;
	}	
	
	dwData = _ttol(Buffer);

	RegCloseKey(hKey);

	return STATUS_SUCCESS;

}


//=====================================================================
//	Get Windows version
//=====================================================================
int GetWindowsVersion()
{
	OSVERSIONINFOEX OsVer;
 
	OsVer.dwOSVersionInfoSize = sizeof (OSVERSIONINFOEX);

	if (!GetVersionEx((OSVERSIONINFO *) &OsVer))
	{
		OsVer.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

		if (!GetVersionEx((OSVERSIONINFO *) &OsVer))
		{
			return -1;
		}
	}
 
	switch (OsVer.dwMajorVersion)
	{
		case 5: return (OsVer.dwMinorVersion==1)?Mrv8000xOSWinXP:Mrv8000xOSWin2K; 
		case 4: return (OsVer.dwMinorVersion==0)?Mrv8000xOSWinNT:Mrv8000xOSWindows; 
		default:	return -1;
	}	
}


