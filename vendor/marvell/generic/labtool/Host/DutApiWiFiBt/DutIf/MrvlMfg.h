/** @file MrvlMfg.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */
 
#ifdef MFGDLL_EXPORTS
#define MFGDLL_API __declspec(dllexport)
#else
#define MFGDLL_API __declspec(dllimport)
#endif
 
//------------------------------------------------------------------------------
MFGDLL_API int Mrv8000xOpenAdapter(HANDLE &hAdapter, bool Gru=0);
MFGDLL_API int Mrv8000xCloseAdapter(HANDLE &hAdapter);

MFGDLL_API int MrvlQueryInfo(HANDLE hAdapter, ULONG OidCode, LPBYTE lpData, ULONG nDataLen);

MFGDLL_API DWORD GetMaxtimeOut(void);
MFGDLL_API void SetMaxtimeOut(DWORD TimeOut);
