// (C) Copyright 2006 Marvell International Ltd.
// All Rights Reserved
//
/******************************************************************************
**
** INTEL CONFIDENTIAL
** Copyright 2000-2005 Intel Corporation All Rights Reserved.
**
** The source code contained or described herein and all documents
** related to the source code (Material) are owned by Intel Corporation
** or its suppliers or licensors.  Title to the Material remains with
** Intel Corporation or its suppliers and licensors. The Material contains
** trade secrets and proprietary and confidential information of Intel
** or its suppliers and licensors. The Material is protected by worldwide
** copyright and trade secret laws and treaty provisions. No part of the
** Material may be used, copied, reproduced, modified, published, uploaded,
** posted, transmitted, distributed, or disclosed in any way without Intel’s
** prior express written permission.
**
** No license under any patent, copyright, trade secret or other intellectual
** property right is granted to or conferred upon you by disclosure or
** delivery of the Materials, either expressly, by implication, inducement,
** estoppel or otherwise. Any license under such intellectual property rights
** must be express and approved by Intel in writing.
**
*****************************************************************************/
#include "NVMServer_defs.h"
#include "pxa_dbg.h"
#include "string.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <poll.h>
#include "NVMIPCClient.h"

#define NVM_REQ_RES_MAX_LEN     (8192)

static unsigned char *buf = NULL;

/* common */
static BOOL NVMIPCClient_inited = FALSE;
static int NVMIPCClient_hIPC[2];
static char NVMIPCClient_clientName[NVM_CLIENT_NAME_LEN];

BOOL NVMIPCClient_InitClient(char *name)
{
	char clientPath[FILENAME_MAX];
	int ret;

	if (NVMIPCClient_inited == TRUE)
		return TRUE;

	NVMIPCClient_hIPC[0] = 0;
	NVMIPCClient_hIPC[1] = 0;
	NVMIPCClient_clientName[0] = '\0';

	buf = (unsigned char *)malloc(NVM_REQ_RES_MAX_LEN);
	if (buf == NULL)
	{
		DEBUGMSG("[NVM IPC] Could not allocate buffer\n");
		return FALSE;
	}

	strncpy(NVMIPCClient_clientName, name, NVM_CLIENT_NAME_LEN);
	NVMIPCClient_clientName[NVM_CLIENT_NAME_LEN - 1] = '\0';

	sprintf(clientPath, "/tmp/%s0", NVMIPCClient_clientName);
	ret = open(clientPath, O_RDWR);
	if (ret < 0)
	{
		DEBUGMSG("[NVM IPC] Could not open FIFO[%s] ,error %d\r\n", clientPath, ret);
		return FALSE;
	}
	NVMIPCClient_hIPC[1] = ret;
	DEBUGMSG("[NVM IPC] hIPC = %d\r\n", NVMIPCClient_hIPC[1]);

	sprintf(clientPath, "/tmp/%s1", NVMIPCClient_clientName);
	ret = open(clientPath, O_RDWR);
	if (ret < 0)
	{
		DEBUGMSG("[NVM IPC] Could not open FIFO[%s] ,error %d\r\n", clientPath, ret);
		return FALSE;
	}
	NVMIPCClient_hIPC[0] = ret;
	DEBUGMSG("[NVM IPC] hIPC = %d\r\n", NVMIPCClient_hIPC[0]);

	NVM_OP_CODE helloreq;
	NVM_STATUS_T hellores;

	/* clean the garbage if any */
	struct pollfd fds;
	fds.fd = NVMIPCClient_hIPC[0];
	fds.events = POLLIN;
	while (poll(&fds, 1, 0) > 0)
		read(NVMIPCClient_hIPC[0], &hellores, sizeof(NVM_STATUS_T));

	/* handshake */
	helloreq = NVM_OP_CODE_HANDSHAKE;
	if (write(NVMIPCClient_hIPC[1], &helloreq, sizeof(NVM_OP_CODE)) < (int)sizeof(NVM_OP_CODE))
		return FALSE;
	DEBUGMSG("[NVM IPC] sent shakehand request [%d]\n", helloreq);
	hellores = NVM_STATUS_FAILURE;
	read(NVMIPCClient_hIPC[0], &hellores, sizeof(NVM_STATUS_T));
	DEBUGMSG("[NVM IPC] received shakehand response [%d]\n", hellores);
	if (hellores != NVM_STATUS_SUCCESS)
		return FALSE;

	NVMIPCClient_inited = TRUE;

	return TRUE;
}

void NVMIPCClient_DeInitClient()
{
	if (buf)
		free(buf);
	if (NVMIPCClient_hIPC[0] > 0)
		close(NVMIPCClient_hIPC[0]);
	if (NVMIPCClient_hIPC[1] > 0)
		close(NVMIPCClient_hIPC[1]);
	NVMIPCClient_inited = FALSE;
}

NVM_STATUS_T NVMIPCClient_FileFindClose(HANDLE hSearchHandle)
{
	UNUSEDPARAM(hSearchHandle)

	DEBUGMSG("%s() not supported yet.\n", __func__);
	return NVM_STATUS_FAILURE;
}

NVM_STATUS_T NVMIPCClient_FileFindFirst(LPCTSTR szFileName,     NVM_FILE_INFO* pFindResults, LPHANDLE lpSearchHandle)
{
	NVM_FILE_FIND_FIRST_REQUEST_STRUCT req;
	NVM_FILE_FIND_FIRST_RESPONSE_STRUCT *res;
	int len;

	DEBUGMSG("%s(%s)\n", __func__, szFileName);
	req.op_code = NVM_OP_CODE_FILE_FIND_FIRST;
	strcpy((LPCTSTR)(req.szFileName), szFileName);
	if (write(NVMIPCClient_hIPC[1], &req, sizeof(req)) < (int)sizeof(NVM_FILE_FIND_FIRST_REQUEST_STRUCT))
		return NVM_STATUS_FAILURE;
	len = read(NVMIPCClient_hIPC[0], buf, NVM_REQ_RES_MAX_LEN);
	if (len < (int)sizeof(NVM_FILE_FIND_FIRST_RESPONSE_STRUCT))
		return NVM_STATUS_FAILURE;
	res = (NVM_FILE_FIND_FIRST_RESPONSE_STRUCT *)buf;
	if (res->op_code != req.op_code)
		return NVM_STATUS_FAILURE;
	if (res->error_code != NVM_STATUS_SUCCESS)
		return res->error_code;

	memcpy(pFindResults, &(res->fileParms), sizeof(NVM_FILE_INFO));
	*lpSearchHandle = res->fileParms.dir_id;
	DEBUGMSG("%s() return handler=%d\n", __func__, *lpSearchHandle);
	return NVM_STATUS_SUCCESS;
}

NVM_STATUS_T NVMIPCClient_FileFindNext(NVM_FILE_INFO* pFindResults, HANDLE hSearchHandle )
{
	NVM_FILE_FIND_NEXT_REQUEST_STRUCT req;
	NVM_FILE_FIND_NEXT_RESPONSE_STRUCT *res;
	int len;

	DEBUGMSG("%s(handler=%d)\n", __func__, hSearchHandle);
	req.op_code = NVM_OP_CODE_FILE_FIND_NEXT;
	req.fileParms.dir_id = hSearchHandle;
	if (write(NVMIPCClient_hIPC[1], &req, sizeof(req)) < (int)sizeof(NVM_FILE_FIND_NEXT_REQUEST_STRUCT))
		return NVM_STATUS_FAILURE;
	len = read(NVMIPCClient_hIPC[0], buf, NVM_REQ_RES_MAX_LEN);
	if (len < (int)sizeof(NVM_FILE_FIND_NEXT_RESPONSE_STRUCT))
		return NVM_STATUS_FAILURE;
	res = (NVM_FILE_FIND_NEXT_RESPONSE_STRUCT *)buf;
	if (res->op_code != req.op_code)
		return NVM_STATUS_FAILURE;
	if (res->error_code != NVM_STATUS_SUCCESS)
		return res->error_code;

	memcpy(pFindResults, &(res->fileParms), sizeof(NVM_FILE_INFO));
	return NVM_STATUS_SUCCESS;
}

NVM_STATUS_T NVMIPCClient_GetFileSize(LPCTSTR szFileName, PDWORD pdwSizeHigh, PDWORD pdwSizeLow)
{
	UNUSEDPARAM(szFileName)
	UNUSEDPARAM(pdwSizeHigh)
	UNUSEDPARAM(pdwSizeLow)

	DEBUGMSG("%s() not supported yet.\n", __func__);
	return NVM_STATUS_FAILURE;
}

NVM_STATUS_T NVMIPCClient_FileOpen(LPCTSTR szFileName, LPCTSTR szAttrib, PUINT32 phFile)
{
	NVM_FILE_OPEN_REQUEST_STRUCT req;
	NVM_FILE_OPEN_RESPONSE_STRUCT *res;
	int len;

	DEBUGMSG("%s(%s, %s)\n", __func__, szFileName, szAttrib);
	req.op_code = NVM_OP_CODE_FILE_OPEN;
	strcpy((LPCTSTR)(req.szFileName), szFileName);
	strcpy((LPCTSTR)(req.szFileAttributes), szAttrib);
	if (write(NVMIPCClient_hIPC[1], &req, sizeof(req)) < (int)sizeof(NVM_FILE_OPEN_REQUEST_STRUCT))
		return NVM_STATUS_FAILURE;
	len = read(NVMIPCClient_hIPC[0], buf, NVM_REQ_RES_MAX_LEN);
	if (len < (int)sizeof(NVM_FILE_OPEN_RESPONSE_STRUCT))
		return NVM_STATUS_FAILURE;
	res = (NVM_FILE_OPEN_RESPONSE_STRUCT *)buf;
	if (res->op_code != req.op_code)
		return NVM_STATUS_FAILURE;
	if (res->error_code != NVM_STATUS_SUCCESS)
		return res->error_code;

	*phFile = res->hFile;
	DEBUGMSG("%s() return handler=%d\n", __func__, *phFile);
	return NVM_STATUS_SUCCESS;
}

NVM_STATUS_T NVMIPCClient_FileWrite(UINT32 hFile, LPVOID pBuffer, UINT32 dwBufferLen, short wItemSize, UINT32 dwCount, PUINT32 pdwActual)
{
	UNUSEDPARAM(dwBufferLen)
	static NVM_FILE_WRITE_REQUEST_STRUCT * pReqBuf = NULL;
	static UINT32 nReqBufAllocSize = 0;
	UINT32 nBytesToWrite, nReqBufReqSize;
	NVM_FILE_WRITE_RESPONSE_STRUCT *res;
	int len;

	DEBUGMSG("%s(%d, %d*%d)\n", __func__, hFile, wItemSize, dwCount);
	nBytesToWrite = dwCount * wItemSize;
	nReqBufReqSize = (nBytesToWrite > WRITE_DATA_SIZE_IN_BYTES) ?
			 (sizeof(NVM_FILE_WRITE_REQUEST_STRUCT) + nBytesToWrite - WRITE_DATA_SIZE_IN_BYTES) :
			 sizeof(NVM_FILE_WRITE_REQUEST_STRUCT);
	/* if buffer was not allocated yet or its allocated size is smaller than requested now */
	if (!pReqBuf || (nReqBufAllocSize < nReqBufReqSize) )
	{
		if (pReqBuf)    /* means that allocated buffer size is smaller than read-size requested now */
			free(pReqBuf);

		pReqBuf = (NVM_FILE_WRITE_REQUEST_STRUCT*)(malloc(nReqBufReqSize));
	}

	if (pReqBuf)
	{
		nReqBufAllocSize = nReqBufReqSize;
		pReqBuf->op_code = NVM_OP_CODE_FILE_WRITE;
		pReqBuf->hFile = hFile;
		pReqBuf->nNumberOfItems = dwCount;
		pReqBuf->nItemSize = wItemSize;
		memcpy(pReqBuf->DataBuffer, pBuffer, wItemSize * dwCount);
		if (write(NVMIPCClient_hIPC[1], pReqBuf, nReqBufAllocSize) < (int)nReqBufAllocSize)
			return NVM_STATUS_FAILURE;
		len = read(NVMIPCClient_hIPC[0], buf, NVM_REQ_RES_MAX_LEN);
		if (len < (int)sizeof(NVM_FILE_WRITE_RESPONSE_STRUCT))
			return NVM_STATUS_FAILURE;
		res = (NVM_FILE_WRITE_RESPONSE_STRUCT *)buf;
		if (res->op_code != pReqBuf->op_code)
			return NVM_STATUS_FAILURE;
		if (res->error_code != NVM_STATUS_SUCCESS)
			return res->error_code;

		*pdwActual = res->nNumberActuallItemsWrite;
		DEBUGMSG("%s() return size=%d\n", __func__, *pdwActual);
	}
	else
	{
		nReqBufAllocSize = 0;
	}
	return NVM_STATUS_SUCCESS;
}

NVM_STATUS_T NVMIPCClient_FileFlush(UINT32 hFile)
{
	UNUSEDPARAM(hFile)

	DEBUGMSG("%s() not supported yet.\n", __func__);
	return NVM_STATUS_FAILURE;
}

NVM_STATUS_T NVMIPCClient_FileSeek(UINT32 hFile, LONG dwOffset, INT32 dwOrigin)
{
	NVM_FILE_SEEK_REQUEST_STRUCT req;
	NVM_FILE_SEEK_RESPONSE_STRUCT *res;
	int len;

	DEBUGMSG("%s(%d, %d from %d)\n", __func__, hFile, dwOffset, dwOrigin);
	req.op_code = NVM_OP_CODE_FILE_SEEK;
	req.hFile = hFile;
	req.nOffset = dwOffset;
	req.nOrigin = (NVM_SEEK_ORIGIN)dwOrigin;
	if (write(NVMIPCClient_hIPC[1], &req, sizeof(req)) < (int)sizeof(NVM_FILE_SEEK_REQUEST_STRUCT))
		return NVM_STATUS_FAILURE;
	len = read(NVMIPCClient_hIPC[0], buf, NVM_REQ_RES_MAX_LEN);
	if (len < (int)sizeof(NVM_FILE_SEEK_RESPONSE_STRUCT))
		return NVM_STATUS_FAILURE;
	res = (NVM_FILE_SEEK_RESPONSE_STRUCT *)buf;
	if (res->op_code != req.op_code)
		return NVM_STATUS_FAILURE;
	if (res->error_code != NVM_STATUS_SUCCESS)
		return res->error_code;

	return NVM_STATUS_SUCCESS;
}

NVM_STATUS_T NVMIPCClient_FileRead(UINT32 hFile, short wItemSize, UINT32 dwCount, PUINT32 pdwActual, LPVOID pBuffer)
{
	NVM_FILE_READ_REQUEST_STRUCT req;
	NVM_FILE_READ_RESPONSE_STRUCT *res;
	int len;

	DEBUGMSG("%s(%d, %d*%d)\n", __func__, hFile, wItemSize, dwCount);
	req.op_code = NVM_OP_CODE_FILE_READ;
	req.hFile = hFile;
	req.nNumberOfItems = dwCount;
	req.nItemSize = wItemSize;
	if (write(NVMIPCClient_hIPC[1], &req, sizeof(req)) < (int)sizeof(NVM_FILE_READ_REQUEST_STRUCT))
		return NVM_STATUS_FAILURE;
	len = read(NVMIPCClient_hIPC[0], buf, NVM_REQ_RES_MAX_LEN);
#if 0   /* Don't check this because NVM server may return a shorter msg */
	if (len < sizeof(NVM_FILE_READ_RESPONSE_STRUCT))
		return NVM_STATUS_FAILURE;
#endif
	res = (NVM_FILE_READ_RESPONSE_STRUCT *)buf;
	if (res->op_code != req.op_code)
		return NVM_STATUS_FAILURE;
	if (res->error_code != NVM_STATUS_SUCCESS)
		return res->error_code;

	memcpy(pBuffer, res->DataBuffer, wItemSize * res->nNumberActuallItemsRead);
	*pdwActual = res->nNumberActuallItemsRead;
	DEBUGMSG("%s() return size=%d\n", __func__, *pdwActual);
	return NVM_STATUS_SUCCESS;
}

NVM_STATUS_T NVMIPCClient_FileClose(UINT32 hFile)
{
	NVM_FILE_CLOSE_REQUEST_STRUCT req;
	NVM_FILE_CLOSE_RESPONSE_STRUCT *res;
	int len;

	DEBUGMSG("%s(%d)\n", __func__, hFile);
	req.op_code = NVM_OP_CODE_FILE_CLOSE;
	req.hFile = hFile;
	if (write(NVMIPCClient_hIPC[1], &req, sizeof(req)) < (int)sizeof(NVM_FILE_CLOSE_REQUEST_STRUCT))
		return NVM_STATUS_FAILURE;
	len = read(NVMIPCClient_hIPC[0], buf, NVM_REQ_RES_MAX_LEN);
	if (len < (int)sizeof(NVM_FILE_CLOSE_RESPONSE_STRUCT))
		return NVM_STATUS_FAILURE;
	res = (NVM_FILE_CLOSE_RESPONSE_STRUCT *)buf;
	if (res->op_code != req.op_code)
		return NVM_STATUS_FAILURE;
	if (res->error_code != NVM_STATUS_SUCCESS)
		return res->error_code;

	return NVM_STATUS_SUCCESS;
}

NVM_STATUS_T NVMIPCClient_FileCloseAll()
{
	DEBUGMSG("%s() not supported yet.\n", __func__);
	return NVM_STATUS_FAILURE;
}

NVM_STATUS_T NVMIPCClient_FileDelete(LPCTSTR szFileName)
{
	NVM_FILE_REMOVE_REQUEST_STRUCT req;
	NVM_FILE_REMOVE_RESPONSE_STRUCT *res;
	int len;

	DEBUGMSG("%s(%s)\n", __func__, szFileName);
	req.op_code = NVM_OP_CODE_FILE_REMOVE;
	strcpy((LPCTSTR)(req.szFileName), szFileName);
	if (write(NVMIPCClient_hIPC[1], &req, sizeof(req)) < (int)sizeof(NVM_FILE_REMOVE_REQUEST_STRUCT))
		return NVM_STATUS_FAILURE;
	len = read(NVMIPCClient_hIPC[0], buf, NVM_REQ_RES_MAX_LEN);
	if (len < (int)sizeof(NVM_FILE_REMOVE_RESPONSE_STRUCT))
		return NVM_STATUS_FAILURE;
	res = (NVM_FILE_REMOVE_RESPONSE_STRUCT *)buf;
	if (res->op_code != req.op_code)
		return NVM_STATUS_FAILURE;
	if (res->error_code != NVM_STATUS_SUCCESS)
		return res->error_code;

	return NVM_STATUS_SUCCESS;
}

NVM_STATUS_T NVMIPCClient_FileRename(LPCTSTR szOldFileName, LPCTSTR szNewFileName)
{
	UNUSEDPARAM(szOldFileName)
	UNUSEDPARAM(szNewFileName)
	DEBUGMSG("%s() not supported yet.\n", __func__);
	return NVM_STATUS_FAILURE;
}

NVM_STATUS_T NVMIPCClient_GetFileStat(LPCTSTR szFileName, PDWORD pdwStat)
{
	UNUSEDPARAM(szFileName)
	UNUSEDPARAM(pdwStat)

	DEBUGMSG("%s() not supported yet.\n", __func__);
	return NVM_STATUS_FAILURE;
}

NVM_STATUS_T NVMIPCClient_FileChangeMode(LPCTSTR szFileName, DWORD dwNewMode)
{
	UNUSEDPARAM(szFileName)
	UNUSEDPARAM(dwNewMode)

	DEBUGMSG("%s() not supported yet.\n", __func__);
	return NVM_STATUS_FAILURE;
}

NVM_STATUS_T NVMIPCClient_GetAvailSpace(PUINT32 pdwSize)
{
	UNUSEDPARAM(pdwSize)

	DEBUGMSG("%s() not supported yet.\n", __func__);
	return NVM_STATUS_FAILURE;
}

NVM_STATUS_T NVMIPCClient_GetTotalSpace(PUINT32 pdwSize)
{
	UNUSEDPARAM(pdwSize)

	DEBUGMSG("%s() not supported yet.\n", __func__);
	return NVM_STATUS_FAILURE;
}

