// (C) Copyright 2006 Marvell International Ltd.
// All Rights Reserved
//
#ifndef NVMIPCCLIENT_H
#define NVMIPCCLIENT_H

#include "nvm_shared.h"

#define NVM_CLIENT_NAME_LEN     16

#ifdef __cplusplus
extern "C" {
#endif

/* client */
BOOL NVMIPCClient_InitClient(char *name);
void NVMIPCClient_DeInitClient();
NVM_STATUS_T NVMIPCClient_FileFindClose(HANDLE hSearchHandle);
NVM_STATUS_T NVMIPCClient_FileFindFirst(LPCTSTR szFileName, NVM_FILE_INFO* pFindResults, LPHANDLE lpSearchHandle);
NVM_STATUS_T NVMIPCClient_FileFindNext(NVM_FILE_INFO* pFindResults, HANDLE hSearchHandle );
NVM_STATUS_T NVMIPCClient_GetFileSize(LPCTSTR szFileName, PDWORD pdwSizeHigh, PDWORD pdwSizeLow);
NVM_STATUS_T NVMIPCClient_FileOpen(LPCTSTR szFileName, LPCTSTR szAttrib, PUINT32 phFile);
NVM_STATUS_T NVMIPCClient_FileWrite(UINT32 hFile, LPVOID pBuffer, UINT32 dwBufferLen, short wItemSize, UINT32 dwCount, PUINT32 pdwActual);
NVM_STATUS_T NVMIPCClient_FileFlush(UINT32 hFile);
NVM_STATUS_T NVMIPCClient_FileSeek(UINT32 hFile, LONG dwOffset, INT32 dwOrigin);
NVM_STATUS_T NVMIPCClient_FileRead(UINT32 hFile, short wItemSize, UINT32 dwCount, PUINT32 pdwActual, LPVOID pBuffer);
NVM_STATUS_T NVMIPCClient_FileClose(UINT32 hFile);
NVM_STATUS_T NVMIPCClient_FileCloseAll();
NVM_STATUS_T NVMIPCClient_FileDelete(LPCTSTR szFileName);
NVM_STATUS_T NVMIPCClient_FileRename(LPCTSTR szOldFileName, LPCTSTR szNewFileName);
NVM_STATUS_T NVMIPCClient_GetFileStat(LPCTSTR szFileName, PDWORD pdwStat);
NVM_STATUS_T NVMIPCClient_FileChangeMode(LPCTSTR szFileName, DWORD dwNewMode);
NVM_STATUS_T NVMIPCClient_GetAvailSpace(PUINT32 pdwSize);
NVM_STATUS_T NVMIPCClient_GetTotalSpace(PUINT32 pdwSize);

#ifdef __cplusplus
}
#endif

#endif

