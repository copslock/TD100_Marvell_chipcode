// (C) Copyright 2006 Marvell International Ltd.
// All Rights Reserved
//
#include "NVMProxy.h"
#include "NVMServer_defs.h"
#include "parser.h"
#include "pxa_dbg.h"
#include "osa.h"
#include "string.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "NVMIPCServer.h"

#define NVM_REQ_RES_MAX_LEN     (8192)
static void NVMIPCServerRun(void *arg) __attribute__((noreturn));
CNVMIPCServer::CNVMIPCServer()
{
	hIPC[0] = 0;
	hIPC[1] = 0;
	clientID = 0;
	taskRef = NULL;
	clientName[0] = '\0';
	buf = (unsigned char *)malloc(NVM_REQ_RES_MAX_LEN);
}

CNVMIPCServer::~CNVMIPCServer()
{
	free(buf);
}

NVM_PARSER_RESULT CNVMIPCServer::HandleIPCReqBufferAndResponse( NVM_CLIENT_ID ClientID,
								PUINT8 pRxBuffer, UINT32 dwBufferLen)
{
	UNUSEDPARAM(dwBufferLen)

	static HANDLE g_searchHandle = 0;

	UINT32 hFile = 0;
	NVM_OP_CODE NVM_OPCode;
	// Get first byte in buffer as OP CODE
	NVM_OPCode = ( NVM_OP_CODE)pRxBuffer[ 0];
	DEBUGMSG("[NVM Proxy] CNVMIPCServer::HandleIPCReqBufferAndResponse \r\n");
	switch ( NVM_OPCode)
	{
	case NVM_OP_CODE_HANDSHAKE:
		DEBUGMSG("[NVM Proxy] NVM_OP_CODE_HANDSHAKE + \r\n");
		NVM_STATUS_T NVMS_Handshake_Status;
		NVMS_Handshake_Status = NVM_STATUS_SUCCESS;
		write(hIPC[1], &NVMS_Handshake_Status, sizeof(NVM_STATUS_T));
		DEBUGMSG("[NVM Proxy] NVM_OP_CODE_HANDSHAKE - \r\n");
		break;
	case NVM_OP_CODE_FILE_OPEN:
		DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_OPEN + \r\n");
		NVM_FILE_OPEN_REQUEST_STRUCT *pFile_Open_Request;
		NVM_FILE_OPEN_RESPONSE_STRUCT File_Open_Response;
		//TCHAR szFileName[ NVM_FILE_NAME_MAX_LENGHT];
		//TCHAR szAttrib[ 12];
		NVM_STATUS_T NVMS_FileOpen_Status;

		// Parse receive buffer
		pFile_Open_Request = ( NVM_FILE_OPEN_REQUEST_STRUCT *)pRxBuffer;
		// FIXME
		// Multiple byte string format is not supported
		//mbstowcs( szFileName, pFile_Open_Request->szFileName,
		//	sizeof( szFileName));
		//mbstowcs( szAttrib, pFile_Open_Request->szFileAttributes,
		//	sizeof( szAttrib));
		DEBUGMSG("[NVM Proxy] File Name %s, Attrib - %s\r\n",
			 pFile_Open_Request->szFileName,
			 pFile_Open_Request->szFileAttributes);
		// Call NVM Server API
		NVMS_FileOpen_Status = NVMS_FileOpen( ClientID,
						      (char*)pFile_Open_Request->szFileName,
						      (char*)pFile_Open_Request->szFileAttributes, &hFile);
		if ( NVMS_FileOpen_Status != NVM_STATUS_SUCCESS)
			DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_OPEN request failed, error %d\n",
				 NVMS_FileOpen_Status);
		// Prepare Response buffer
		File_Open_Response.op_code = NVM_OPCode;
		File_Open_Response.error_code = (NVMS_FileOpen_Status);
		File_Open_Response.hFile = (hFile);
		// Response by callback function.
		// Callback function should transmit response data to GPC
		write(hIPC[1], ( PUINT8)&File_Open_Response, sizeof( File_Open_Response));
		DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_OPEN - \r\n");
		break;
	case NVM_OP_CODE_FILE_CLOSE:
	{
		DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_CLOSE + \r\n");
		NVM_FILE_CLOSE_REQUEST_STRUCT *pFile_Close_Request;
		NVM_FILE_CLOSE_RESPONSE_STRUCT File_Close_Response;
		NVM_STATUS_T NVMS_FileClose_Status;
		// Parse receive buffer
		pFile_Close_Request = ( NVM_FILE_CLOSE_REQUEST_STRUCT *)pRxBuffer;
		// Call NVM Server API
		NVMS_FileClose_Status = NVMS_FileClose( ClientID, pFile_Close_Request->hFile);
		// Prepare Response buffer
		File_Close_Response.op_code = NVM_OPCode;
		File_Close_Response.error_code = NVMS_FileClose_Status;
		// Response by callback function.
		// Callback function should transmit response data to GPC
		write(hIPC[1], ( PUINT8)&File_Close_Response, sizeof( File_Close_Response));
		DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_CLOSE  - \r\n");
	}
	break;
	case NVM_OP_CODE_FILE_SEEK:
	{
		NVM_FILE_SEEK_REQUEST_STRUCT *pFile_Seek_Request;
		NVM_FILE_SEEK_RESPONSE_STRUCT File_Seek_Response;
		NVM_STATUS_T NVMS_FileSeek_Status;

		DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_SEEK  + \r\n");
		// Parse receive buffer
		pFile_Seek_Request = ( NVM_FILE_SEEK_REQUEST_STRUCT *)pRxBuffer;

		// Call NVM Server API
		NVMS_FileSeek_Status = NVMS_FileSeek( ClientID, pFile_Seek_Request->hFile, pFile_Seek_Request->nOffset, pFile_Seek_Request->nOrigin);

		// Prepare Response buffer
		File_Seek_Response.op_code = NVM_OPCode;
		File_Seek_Response.error_code = NVMS_FileSeek_Status;

		// Response by callback function.
		// Callback function should transmit response data to GPC
		write(hIPC[1], ( PUINT8)&File_Seek_Response, sizeof( File_Seek_Response));
		DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_SEEK  - \r\n");
	}
	break;
	case NVM_OP_CODE_FILE_REMOVE:
	{
		NVM_FILE_REMOVE_REQUEST_STRUCT *pFile_Remove_Request;
		NVM_FILE_REMOVE_RESPONSE_STRUCT File_Remove_Response;
		//TCHAR szFileName[ NVM_FILE_NAME_MAX_LENGHT];
		NVM_STATUS_T NVMS_RemoveFile_Status;
		DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_REMOVE  +\r\n");

		// Parse receive buffer
		pFile_Remove_Request = ( NVM_FILE_REMOVE_REQUEST_STRUCT *)pRxBuffer;
		//mbstowcs( szFileName, pFile_Remove_Request->szFileName, sizeof( szFileName));

		// Call NVM Server API
		NVMS_RemoveFile_Status = NVMS_FileDelete( ClientID, (char*)pFile_Remove_Request->szFileName);

		// Prepare Response buffer
		File_Remove_Response.op_code = NVM_OPCode;
		File_Remove_Response.error_code = NVMS_RemoveFile_Status;


		// Response by callback function.
		// Callback function should transmit response data to GPC
		write(hIPC[1], ( PUINT8)&File_Remove_Response, sizeof( File_Remove_Response));
		DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_REMOVE  -\r\n");
	}
	break;
	case NVM_OP_CODE_FILE_FIND_FIRST:
	{
		DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_FIND_FIRST + \r\n");
		NVM_FILE_FIND_FIRST_REQUEST_STRUCT *pFile_FindFirst_Request;
		NVM_FILE_FIND_FIRST_RESPONSE_STRUCT File_FindFirst_Response;
		HANDLE SearchHandle = 0;;  //Ran
		NVM_FILE_INFO findData;
		NVM_STATUS_T NVMS_FileFindFirst_Status;

		memset(&File_FindFirst_Response, 0, sizeof(NVM_FILE_FIND_FIRST_RESPONSE_STRUCT));

		//Hezi - Check if ClientID is valid for FindClose operation.
		if (g_searchHandle != 0)
		{
			//Ran - why using  g_searchHandle ?
			NVMS_FileFindClose( ClientID, &g_searchHandle);
		}

		// Parse receive buffer
		pFile_FindFirst_Request = ( NVM_FILE_FIND_FIRST_REQUEST_STRUCT *)pRxBuffer;
		DEBUGMSG("[NVM Proxy] File Name %s\r\n", pFile_FindFirst_Request->szFileName);
		// Call NVM Server API
		//Ran - must give a ptr to handle,since FindFirst will allocate this handle
		NVMS_FileFindFirst_Status = NVMS_FileFindFirst( ClientID,
								(char*)pFile_FindFirst_Request->szFileName,
								&findData, &SearchHandle);
		if ( NVMS_FileFindFirst_Status != NVM_STATUS_SUCCESS)
		{
			DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_FINDFIRST request failed %d\n",
				 NVMS_FileFindFirst_Status);
			if (SearchHandle != 0)
			{
				NVMS_FileFindClose( ClientID, &SearchHandle);
				SearchHandle = 0;
			}
		}
		// Prepare Response buffer
		File_FindFirst_Response.op_code = NVM_OPCode;
		File_FindFirst_Response.error_code = NVMS_FileFindFirst_Status;
		memcpy(&File_FindFirst_Response.fileParms, &findData, sizeof(findData));
		File_FindFirst_Response.fileParms.dir_id = (INT32)SearchHandle;

		//Ran
		// the times returned are in FILETIME , we need first understand
		// the client requirments ( created \ access \ last write
		//1. convert FILETIME to SYSTEM time using  FileTimeToSystemTime API
		//2. pick the fields the client expects, each field is a word,
		// use MAKELONG Macro to create the output field.


		//Add other File Params
		// Response by callback function.
		// Callback function should transmit response data to GPC
		write(hIPC[1], ( PUINT8)&File_FindFirst_Response, sizeof( File_FindFirst_Response));
		g_searchHandle = SearchHandle;
		DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_FINDFIRST - \r\n");
		break;
	}
	case NVM_OP_CODE_FILE_FIND_NEXT:
	{
		DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_NEXT + \r\n");
		NVM_FILE_FIND_NEXT_REQUEST_STRUCT *pFile_FindNext_Request;
		NVM_FILE_FIND_NEXT_RESPONSE_STRUCT File_FindNext_Response; //Ran
		HANDLE SearchHandle;
		NVM_FILE_INFO findData;
		NVM_STATUS_T NVMS_FileFindNext_Status;

		memset(&File_FindNext_Response, 0, sizeof(NVM_FILE_FIND_NEXT_RESPONSE_STRUCT));

		// Parse receive buffer
		pFile_FindNext_Request = ( NVM_FILE_FIND_NEXT_REQUEST_STRUCT *)pRxBuffer;
		// Call NVM Server API
		//DON'T searchHandle=pFile_FindNext_Request->fileParms.dir_id;  done in NVMServer_API in its way
		SearchHandle = g_searchHandle;
		NVMS_FileFindNext_Status = NVMS_FileFindNext( ClientID, &findData, &SearchHandle);
		if ( NVMS_FileFindNext_Status != NVM_STATUS_SUCCESS)
		{
			DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_FINDNEXT request failed %d\n",
				 NVMS_FileFindNext_Status);
			if (SearchHandle != 0)
				NVMS_FileFindClose( ClientID, &SearchHandle);
			SearchHandle = 0;
		}
		// Prepare Response buffer
		File_FindNext_Response.op_code = NVM_OPCode;
		File_FindNext_Response.error_code = (NVMS_FileFindNext_Status);
		//Actually not really needed, since handle is the same as input value
		File_FindNext_Response.fileParms.dir_id = (INT32)SearchHandle;
		memcpy(&File_FindNext_Response.fileParms, &findData, sizeof(findData));

		//Ran - first understand what time is expected
		// the times returned are in FILETIME , we need
		//1. convert FILETIME to SYSTEM time using  FileTimeToSystemTime API
		//2. pick the fields the client expects, each field is a word,
		// use MAKELONG Macro to create the output field.

		// Response by callback function.
		// Callback function should transmit response data to GPC
		write(hIPC[1], ( PUINT8)&File_FindNext_Response, sizeof( File_FindNext_Response));
		g_searchHandle = SearchHandle;
		DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_FINDNEXT - \r\n");
		break;

	}
	case NVM_OP_CODE_FILE_WRITE:
	{

		NVM_FILE_WRITE_REQUEST_STRUCT *pFile_Write_Request;
		NVM_FILE_WRITE_RESPONSE_STRUCT File_Write_Response;
		UINT32 dwActual;
		NVM_STATUS_T NVMS_FileWrite_Status;
		UINT32 dwCurrentPacketSize = 0;
		DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_WRITE +\r\n");
		// Parse receive buffer
		pFile_Write_Request = ( NVM_FILE_WRITE_REQUEST_STRUCT*)pRxBuffer;
		dwCurrentPacketSize = pFile_Write_Request->nItemSize * pFile_Write_Request->nNumberOfItems;
		DEBUGMSG("[NVM_Parser] Number of itmems %d size of items %d\r\n", pFile_Write_Request->nNumberOfItems, pFile_Write_Request->nItemSize);
		NVMS_FileWrite_Status = NVMS_FileWrite( ClientID, pFile_Write_Request->hFile, ( LPVOID)pFile_Write_Request->DataBuffer, WRITE_DATA_SIZE_IN_BYTES, pFile_Write_Request->nItemSize, pFile_Write_Request->nNumberOfItems, &dwActual);
		// Prepare Response buffer
		File_Write_Response.op_code = NVM_OPCode;
		File_Write_Response.error_code = NVMS_FileWrite_Status;
		File_Write_Response.nNumberActuallItemsWrite = dwActual;

		// Response by callback function.
		// Callback function should transmit response data to GPC
		write(hIPC[1], ( PUINT8)&File_Write_Response, sizeof( File_Write_Response));
		DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_WRITE -\r\n");
	}
	break;
	case NVM_OP_CODE_FILE_READ:
	{
		static NVM_FILE_READ_RESPONSE_STRUCT *pResBuf = NULL;
		static UINT32 nReqBufAllocSize = 0;
		NVM_FILE_READ_REQUEST_STRUCT *pFile_Read_Request;
		UINT32 nBytesToRead, nReqBufReqSize;
		UINT32 dwActual = 0;
		NVM_STATUS_T NVMS_FileRead_Status;
		DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_READ +\r\n");
		// Parse receive buffer
		pFile_Read_Request = ( NVM_FILE_READ_REQUEST_STRUCT *)pRxBuffer;
		nBytesToRead = pFile_Read_Request->nItemSize * pFile_Read_Request->nNumberOfItems;

		nReqBufReqSize = (nBytesToRead > READ_DATA_RESPONSE_SIZE_IN_BYTES) ?
				 (sizeof(NVM_FILE_READ_RESPONSE_STRUCT) + nBytesToRead - READ_DATA_RESPONSE_SIZE_IN_BYTES) :
				 sizeof(NVM_FILE_READ_RESPONSE_STRUCT);
		/* if buffer was not allocated yet or its allocated size is smaller than requested now */
		if (!pResBuf || (nReqBufAllocSize < nReqBufReqSize) )
		{
			if (pResBuf)    /* means that allocated buffer size is smaller than read-size requested now */
				free(pResBuf);

			pResBuf = (NVM_FILE_READ_RESPONSE_STRUCT*)(malloc(nReqBufReqSize));
		}

		if (pResBuf)
		{
			nReqBufAllocSize = nReqBufReqSize;
			// Call NVM Server API
			NVMS_FileRead_Status = NVMS_FileRead( ClientID,
							      pFile_Read_Request->hFile,
							      pFile_Read_Request->nItemSize,
							      pFile_Read_Request->nNumberOfItems,
							      &dwActual, ( LPVOID)pResBuf->DataBuffer);
			// Prepare Response buffer
			pResBuf->op_code = NVM_OPCode;
			pResBuf->error_code = NVMS_FileRead_Status;
			pResBuf->nNumberActuallItemsRead = dwActual;
			if (dwActual)
				pResBuf->nDataBufferLength = pFile_Read_Request->nItemSize * pFile_Read_Request->nNumberOfItems;
			else
				pResBuf->nDataBufferLength = 0;

			// Response by callback function.
			// Callback function should transmit response data to GPC
			//we want to transmit only the header and actual bytes read
			write(hIPC[1], pResBuf, nReqBufAllocSize - (nBytesToRead - dwActual));
		}
		else
		{
			nReqBufAllocSize = 0;
		}
		DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_READ  -\r\n");

		break;
	}
	case NVM_OP_CODE_RTC_READ:
	{
		RETAILMSG("[NVM Proxy] NVM_OP_CODE_RTC_READ + \r\n");

		RTC_GET_RESPONSE_STRUCT responseData;
		//SYSTEMTIME	systemTime;
		time_t clock;
		struct tm* local;

		// GetSystemTime(&systemTime);
		time(&clock);
		local = localtime(&clock);
		ASSERT(local);


		responseData.op_code    = NVM_OPCode;
		responseData.error_code = NVM_STATUS_SUCCESS;

		responseData.second             = (UINT8)local->tm_sec;
		responseData.minute             = (UINT8)local->tm_min;
		responseData.hour               = (UINT8)local->tm_hour;
		responseData.day                = (UINT8)local->tm_mday;
		responseData.month              = (UINT8)local->tm_mon;
		responseData.year               = local->tm_year;

		write(hIPC[1], (PUINT8)&responseData, sizeof(RTC_GET_RESPONSE_STRUCT));

		RETAILMSG("[NVM Proxy] NVM_OP_CODE_RTC_READ  -\r\n");
	}
	break;
	default:
		DEBUGMSG("Error!! unknow OP CODE=%d", NVM_OPCode);
		return NVM_PARSER_RESULT_INVALID_OP_CODE;
	}
	return NVM_PARSER_RESULT_DONE;
}

static void NVMIPCServerRun(void *arg)
{
	CNVMIPCServer *clientIPC = (CNVMIPCServer *)arg;

	clientIPC->RunServer();
}

void CNVMIPCServer::RunServer()
{
	int len;
	NVM_PARSER_RESULT NVMP_result;

	while (TRUE)
	{
		DEBUGMSG("%s() reading...\r\n", __func__);
		len = read(hIPC[0], buf, NVM_REQ_RES_MAX_LEN);
		DEBUGMSG("%s() read hIPC[fd=%d] %d bytes\r\n", __func__, hIPC[0], len);
		NVMP_result = HandleIPCReqBufferAndResponse(clientID, buf, len);
	}
}

BOOL CNVMIPCServer::InitServer(char *name)
{
	char clientPath[FILENAME_MAX];
	NVM_STATUS_T res;
	int ret;
	OSA_STATUS status;

	strncpy(clientName, name, NVM_CLIENT_NAME_LEN);
	clientName[NVM_CLIENT_NAME_LEN - 1] = '\0';

	res = NVMS_Register(clientName, &clientID);
	if (res != NVM_STATUS_SUCCESS)
	{
		DEBUGMSG("[NVM Proxy] Could not register IPC[%s] with NVM server,error %d\r\n", clientName, res);
		return FALSE;
	}

	DEBUGMSG("[NVM Proxy] IPC registerd with NVM server\r\n");
	sprintf(clientPath, "/tmp/%s0", clientName);
	unlink(clientPath);
	ret = mkfifo(clientPath, S_IRWXU);
	if (ret < 0)
	{
		DEBUGMSG("[NVM Proxy] Could not make FIFO[%s] ,error %d\r\n", clientPath, ret);
		return FALSE;
	}

	ret = open(clientPath, O_RDWR);
	if (ret < 0)
	{
		DEBUGMSG("[NVM Proxy] Could not open FIFO[%s] ,error %d\r\n", clientPath, ret);
		return FALSE;
	}
	hIPC[0] = ret;
	DEBUGMSG("[NVM Proxy] hIPC = %d\r\n", hIPC[0]);

	sprintf(clientPath, "/tmp/%s1", clientName);
	unlink(clientPath);
	ret = mkfifo(clientPath, S_IRWXU);
	if (ret < 0)
	{
		DEBUGMSG("[NVM Proxy] Could not make FIFO[%s] ,error %d\r\n", clientPath, ret);
		return FALSE;
	}

	ret = open(clientPath, O_RDWR);
	if (ret < 0)
	{
		DEBUGMSG("[NVM Proxy] Could not open FIFO[%s] ,error %d\r\n", clientPath, ret);
		return FALSE;
	}
	hIPC[1] = ret;
	DEBUGMSG("[NVM Proxy] hIPC = %d\r\n", hIPC[1]);

	status = OSATaskCreate(&taskRef, &taskStack, NVM_STACK_SIZE,
			       NVM_PRIORITY, (CHAR *)clientName, NVMIPCServerRun, this);

	if (status != OS_SUCCESS)
	{
		DEBUGMSG("[NVM Proxy] Could not start task [%s] ,error %d\r\n", clientName, status);
		return FALSE;
	}

	return TRUE;
}

void CNVMIPCServer::DeInitServer()
{
	if (hIPC[0] > 0)
		close(hIPC[0]);
	if (hIPC[1] > 0)
		close(hIPC[1]);
	if (taskRef > 0)
		OSATaskDelete(taskRef);
}

