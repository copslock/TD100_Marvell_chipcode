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
#include "parser.h"
#include "NVMServer_defs.h"
#include "pxa_dbg.h"
#include "time.h"
#include "stdlib.h"
#include "string.h"
#ifdef DEBUG_NVM_PROXY
#include <sys/time.h>
#endif
#define INVALID_HANDLE_VALUE    0
HANDLE g_searchHandle = INVALID_HANDLE_VALUE;
#define UNUSEDPARAM(param) (void)param;
extern void PrintBuffer(const PUINT8 pBuffer, UINT bufferLength, LPCTSTR pHeader);

int HandleReqBufferAndResponse( NVM_CLIENT_ID ClientID,
				PUINT8 pRxBuffer, UINT32 dwBufferLen,
				PUINT8 pRspBuffer)
{
	UNUSEDPARAM(dwBufferLen)

	UINT32 hFile = 0;
	NVM_OP_CODE NVM_OPCode;

	// Get first byte in buffer as OP CODE
	NVM_OPCode = ( NVM_OP_CODE)pRxBuffer[ 0];
	DEBUGMSG("[NVM Proxy] HandleIPCReqBufferAndResponse \r\n");
	switch ( NVM_OPCode)
	{
	case NVM_OP_CODE_FILE_OPEN:
	{
		DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_OPEN + \r\n");
		NVM_FILE_OPEN_REQUEST_STRUCT *pFile_Open_Request;
		NVM_FILE_OPEN_RESPONSE_STRUCT *pFile_Open_Response = (NVM_FILE_OPEN_RESPONSE_STRUCT *)pRspBuffer;
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
		DEBUGMSG("[NVM Proxy] File Name [%s], Attrib - %s\r\n",
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
		pFile_Open_Response->op_code = (NVM_OP_CODE_FILE_OPEN);
		pFile_Open_Response->error_code = (NVMS_FileOpen_Status);
		pFile_Open_Response->hFile = (hFile);
		/*	*(UINT32*)pRspBuffer = sizeof( File_Open_Response);*/// fill in the length
		DEBUGMSG("[NVM Proxy]NVM_OP_CODE_FILE_OPEN - \r\n");
		//	break;
		return sizeof( NVM_FILE_OPEN_RESPONSE_STRUCT);
	}
	case NVM_OP_CODE_FILE_CLOSE:
	{
	#ifdef DEBUG_NVM_PROXY
		struct timeval start;
		struct timeval end;
		float timeuse;
	#endif
		DEBUGMSG("[NVM Proxy]NVM_OP_CODE_FILE_CLOSE + \r\n");
		NVM_FILE_CLOSE_REQUEST_STRUCT *pFile_Close_Request;
		NVM_FILE_CLOSE_RESPONSE_STRUCT *pFile_Close_Response =(NVM_FILE_CLOSE_RESPONSE_STRUCT *)pRspBuffer;
		NVM_STATUS_T NVMS_FileClose_Status;
		// Parse receive buffer
		pFile_Close_Request = ( NVM_FILE_CLOSE_REQUEST_STRUCT *)pRxBuffer;
		// Call NVM Server API
	#ifdef DEBUG_NVM_PROXY
		gettimeofday(&start, NULL);
	#endif
		NVMS_FileClose_Status = NVMS_FileClose( ClientID, pFile_Close_Request->hFile);
	#ifdef DEBUG_NVM_PROXY
		gettimeofday(&end, NULL);
		timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
		timeuse /= 1000000;
		DEBUGMSG("file close time %f s\r\n", timeuse);
	#endif
		// Prepare Response buffer
		pFile_Close_Response->op_code = NVM_OP_CODE_FILE_CLOSE;
		pFile_Close_Response->error_code = NVMS_FileClose_Status;
		/*	*(UINT32*)pRspBuffer = sizeof( File_Close_Response);*/// fill in the length
		DEBUGMSG("[NVM Proxy]NVM_OP_CODE_FILE_CLOSE  - \r\n");
		return sizeof( NVM_FILE_CLOSE_RESPONSE_STRUCT);
	}

	case NVM_OP_CODE_FILE_SEEK:
	{
		NVM_FILE_SEEK_REQUEST_STRUCT *pFile_Seek_Request;
		NVM_FILE_SEEK_RESPONSE_STRUCT *pFile_Seek_Response = (NVM_FILE_SEEK_RESPONSE_STRUCT *)pRspBuffer;
		NVM_STATUS_T NVMS_FileSeek_Status;

		DEBUGMSG("[NVM Proxy]NVM_OP_CODE_FILE_SEEK  + \r\n");
		// Parse receive buffer
		pFile_Seek_Request = ( NVM_FILE_SEEK_REQUEST_STRUCT *)pRxBuffer;

		// Call NVM Server API
		NVMS_FileSeek_Status = NVMS_FileSeek( ClientID, pFile_Seek_Request->hFile, pFile_Seek_Request->nOffset, pFile_Seek_Request->nOrigin);

		// Prepare Response buffer
		pFile_Seek_Response->op_code = NVM_OP_CODE_FILE_SEEK;
		pFile_Seek_Response->error_code = NVMS_FileSeek_Status;

		/*	*(UINT32*)pRspBuffer = sizeof( File_Seek_Response); */// fill in the length
		DEBUGMSG("[NVM Proxy]NVM_OP_CODE_FILE_SEEK  - \r\n");
		return sizeof( NVM_FILE_SEEK_RESPONSE_STRUCT);
	}

	case NVM_OP_CODE_FILE_REMOVE:
	{
		NVM_FILE_REMOVE_REQUEST_STRUCT *pFile_Remove_Request;
		NVM_FILE_REMOVE_RESPONSE_STRUCT *pFile_Remove_Response = (NVM_FILE_REMOVE_RESPONSE_STRUCT *)pRspBuffer;
		//TCHAR szFileName[ NVM_FILE_NAME_MAX_LENGHT];
		NVM_STATUS_T NVMS_RemoveFile_Status;
		DEBUGMSG("[NVM Proxy]NVM_OP_CODE_FILE_REMOVE  +\r\n");

		// Parse receive buffer
		pFile_Remove_Request = ( NVM_FILE_REMOVE_REQUEST_STRUCT *)pRxBuffer;
		//mbstowcs( szFileName, pFile_Remove_Request->szFileName, sizeof( szFileName));

		// Call NVM Server API
		NVMS_RemoveFile_Status = NVMS_FileDelete( ClientID, (char*)pFile_Remove_Request->szFileName);

		// Prepare Response buffer
		pFile_Remove_Response->op_code = NVM_OP_CODE_FILE_REMOVE;
		pFile_Remove_Response->error_code = NVMS_RemoveFile_Status;

		/**(UINT32*)pRspBuffer = sizeof( File_Remove_Response);*/ // fill in the length
		DEBUGMSG("[NVM Proxy]NVM_OP_CODE_FILE_REMOVE  -\r\n");
		return sizeof( NVM_FILE_REMOVE_RESPONSE_STRUCT);
	}

	case NVM_OP_CODE_FILE_FIND_FIRST:
	{
		DEBUGMSG("[NVM Proxy]NVM_OP_CODE_FILE_FIND_FIRST + \r\n");
		NVM_FILE_FIND_FIRST_REQUEST_STRUCT *pFile_FindFirst_Request;
		NVM_FILE_FIND_FIRST_RESPONSE_STRUCT *pFile_FindFirst_Response = (NVM_FILE_FIND_FIRST_RESPONSE_STRUCT *)pRspBuffer;
		HANDLE SearchHandle = INVALID_HANDLE_VALUE;;  //Ran
		NVM_STATUS_T NVMS_FileFindFirst_Status;

		memset(pFile_FindFirst_Response, 0, sizeof(NVM_FILE_FIND_FIRST_RESPONSE_STRUCT));

		//Hezi - Check if ClientID is valid for FindClose operation.
		if (g_searchHandle != INVALID_HANDLE_VALUE)
		{
			//Ran - why using  g_searchHandle ?
			NVMS_FileFindClose( ClientID, &g_searchHandle);
		}

		// Parse receive buffer
		pFile_FindFirst_Request = ( NVM_FILE_FIND_FIRST_REQUEST_STRUCT *)pRxBuffer;
		DEBUGMSG("[NVM Proxy] File Name [%s]\r\n", pFile_FindFirst_Request->szFileName);
		// Call NVM Server API
		//Ran - must give a ptr to handle,since FindFirst will allocate this handle
		NVMS_FileFindFirst_Status = NVMS_FileFindFirst( ClientID,
								(char*)pFile_FindFirst_Request->szFileName,
								&pFile_FindFirst_Response->fileParms, &SearchHandle);
		if ( NVMS_FileFindFirst_Status != NVM_STATUS_SUCCESS)
		{
			DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_FINDFIRST request failed %d\n",
				 NVMS_FileFindFirst_Status);
			if (SearchHandle != INVALID_HANDLE_VALUE)
			{
				NVMS_FileFindClose( ClientID, &SearchHandle);
				SearchHandle = INVALID_HANDLE_VALUE;
			}
		}
		// Prepare Response buffer
		pFile_FindFirst_Response->op_code = (NVM_OP_CODE_FILE_FIND_FIRST);
		pFile_FindFirst_Response->error_code = NVMS_FileFindFirst_Status;
		pFile_FindFirst_Response->fileParms.dir_id = (INT32)SearchHandle;

		/*	*(UINT32*)pRspBuffer = sizeof( File_FindFirst_Response); */// fill in the length
		g_searchHandle = SearchHandle;
		DEBUGMSG("[NVM Proxy]NVM_OP_CODE_FILE_FIND_FIRST - \r\n");
		return sizeof( NVM_FILE_FIND_FIRST_RESPONSE_STRUCT);
	}
	case NVM_OP_CODE_FILE_FIND_NEXT:
	{
		DEBUGMSG("[NVM Proxy]NVM_OP_CODE_FILE_FIND_NEXT + \r\n");
		NVM_FILE_FIND_NEXT_REQUEST_STRUCT *pFile_FindNext_Request;
		NVM_FILE_FIND_NEXT_RESPONSE_STRUCT *pFile_FindNext_Response = (NVM_FILE_FIND_NEXT_RESPONSE_STRUCT *)pRspBuffer; //Ran
		HANDLE SearchHandle;
		NVM_STATUS_T NVMS_FileFindNext_Status = NVM_STATUS_SUCCESS;

		memset(pFile_FindNext_Response, 0, sizeof(NVM_FILE_FIND_NEXT_RESPONSE_STRUCT));
		// Parse receive buffer
		pFile_FindNext_Request = ( NVM_FILE_FIND_NEXT_REQUEST_STRUCT *)pRxBuffer;
		// Call NVM Server API
		SearchHandle  = (HANDLE)pFile_FindNext_Request->fileParms.dir_id; //Ran

		// fix the SearchHandle if ACAT does _NOT_ send the correct value!
		if (SearchHandle == 0)
			SearchHandle = g_searchHandle;
		if(SearchHandle != g_searchHandle)
		{
			/* This search has been cancelled by another FindFirst request:
			FindFirst-1 (*)  - due to Flash Explorer open in COMM mode in ICAT
			FindFirst-2 (SystemControl.nvm) - COMM initialization (ps_init.c)
			FindNext-1
			Parser.cpp:HandleReqBufferAndResponse() on NVM_OP_CODE_FILE_FIND_FIRST:
			NVMS_FileFindClose( ClientID,&g_searchHandle); -> deallocate g_searchHandle structures.
			...
			g_searchHandle = SearchHandle;
			Therefore second FindFirst request deallocates the previous FindFirst searchHandle.
			Following COMM FindNext request on the deallocated searchHandle crushes.
			Solution: add a check on FindNext  searchHandle - must be same as g_searchHandle.
			Otherwise return error AND DO NOT DISRUPT THE RECENT SEARCH HANDLE DATA.
			As FindNext is ONLY used by ICAT the collision will only result in incomplete file list in ICAT FlashExplorer.
			*/
		  NVMS_FileFindNext_Status=NVM_STATUS_FAILURE;
		  DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_FINDNEXT handle 0x%08x invalidated by recent FindFirst request\n", SearchHandle);
		}

		if(NVMS_FileFindNext_Status == NVM_STATUS_SUCCESS)
		{
			NVMS_FileFindNext_Status = NVMS_FileFindNext( ClientID, &pFile_FindNext_Response->fileParms, &SearchHandle);
			if ( NVMS_FileFindNext_Status != NVM_STATUS_SUCCESS)
			{
				DEBUGMSG("[NVM Proxy] NVM_OP_CODE_FILE_FINDNEXT request failed %d\n",
				 NVMS_FileFindNext_Status);
				if (SearchHandle != INVALID_HANDLE_VALUE)
					NVMS_FileFindClose( ClientID, &SearchHandle);
				SearchHandle = INVALID_HANDLE_VALUE;
			}
			g_searchHandle = SearchHandle;
		}
		// Prepare Response buffer
		pFile_FindNext_Response->op_code = (NVM_OP_CODE_FILE_FIND_NEXT);
		pFile_FindNext_Response->error_code = (NVMS_FileFindNext_Status);
		//Actually not really needed, since handle is the same as input value
		pFile_FindNext_Response->fileParms.dir_id = (INT32)SearchHandle;

		/*	*(UINT32*)pRspBuffer = sizeof( File_FindNext_Response); */// fill in the length

		DEBUGMSG("[NVM Proxy]NVM_OP_CODE_FILE_FIND_NEXT - \r\n");
		return sizeof( NVM_FILE_FIND_NEXT_RESPONSE_STRUCT);

	}
	case NVM_OP_CODE_FILE_WRITE:
	{

		NVM_FILE_WRITE_REQUEST_STRUCT *pFile_Write_Request;
		NVM_FILE_WRITE_RESPONSE_STRUCT *pFile_Write_Response = (NVM_FILE_WRITE_RESPONSE_STRUCT *)pRspBuffer;
		UINT32 dwActual;
		NVM_STATUS_T NVMS_FileWrite_Status;
		UINT32 dwCurrentPacketSize = 0;
	#ifdef DEBUG_NVM_PROXY
		struct timeval start;
		struct timeval end;
		float timeuse;
	#endif
		DEBUGMSG("[NVM Proxy]NVM_OP_CODE_FILE_WRITE +\r\n");
		// Parse receive buffer
		pFile_Write_Request = ( NVM_FILE_WRITE_REQUEST_STRUCT*)pRxBuffer;
		dwCurrentPacketSize = pFile_Write_Request->nItemSize * pFile_Write_Request->nNumberOfItems;
		DEBUGMSG("[NVM_Parser] Number of itmems %d size of items %d", pFile_Write_Request->nNumberOfItems, pFile_Write_Request->nItemSize);
		if ( dwCurrentPacketSize > sizeof( pFile_Write_Request->DataBuffer))
		{
			DEBUGMSG("[NVM_Parser] dwCurrentPacketSize %d EXCEED LIMIT of %d", dwCurrentPacketSize, sizeof( pFile_Write_Request->DataBuffer));
			return -1;
		}
	#ifdef DEBUG_NVM_PROXY
		gettimeofday(&start, NULL);
	#endif
		NVMS_FileWrite_Status = NVMS_FileWrite( ClientID, pFile_Write_Request->hFile, ( LPVOID)pFile_Write_Request->DataBuffer, WRITE_DATA_SIZE_IN_BYTES, pFile_Write_Request->nItemSize, pFile_Write_Request->nNumberOfItems, &dwActual);
		// Prepare Response buffer
	#ifdef DEBUG_NVM_PROXY
		gettimeofday(&end, NULL);
		timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
		timeuse /= 1000000;
		DEBUGMSG("file write time %f s \r\n", timeuse);
	#endif
		pFile_Write_Response->op_code = NVM_OP_CODE_FILE_WRITE;
		pFile_Write_Response->error_code = NVMS_FileWrite_Status;
		pFile_Write_Response->nNumberActuallItemsWrite = dwActual;

/*		*(UINT32*)pRspBuffer = sizeof( File_Write_Response); */// fill in the length
		DEBUGMSG("[NVM Proxy]NVM_OP_CODE_FILE_WRITE -\r\n");
		return sizeof(NVM_FILE_WRITE_RESPONSE_STRUCT);
	}

	case NVM_OP_CODE_FILE_READ:
	{
		NVM_FILE_READ_REQUEST_STRUCT *pFile_Read_Request;
		NVM_FILE_READ_RESPONSE_STRUCT *pFile_Read_Response = (NVM_FILE_READ_RESPONSE_STRUCT *)pRspBuffer;
		//ZeroMemory(&File_Read_Response,sizeof(File_Read_Response));
		memset(pFile_Read_Response, 0, sizeof(NVM_FILE_READ_RESPONSE_STRUCT));
		UINT32 dwHeaderSize = 0;
		UINT32 dwActual = 0;
		UINT32 dwCurrentPacketSize = 0;
		NVM_STATUS_T NVMS_FileRead_Status;
		DEBUGMSG("[NVM Proxy]NVM_OP_CODE_FILE_READ +\r\n");
		// Parse receive buffer
		pFile_Read_Request = ( NVM_FILE_READ_REQUEST_STRUCT *)pRxBuffer;
		dwCurrentPacketSize = pFile_Read_Request->nItemSize * pFile_Read_Request->nNumberOfItems;
		if ( dwCurrentPacketSize > sizeof(pFile_Read_Response->DataBuffer))
		{
			DEBUGMSG("[NVM_Parser] dwCurrentPacketSize %d EXCEED LIMIT of %d",
				 dwCurrentPacketSize, sizeof( pFile_Read_Response->DataBuffer));
			return -1;
		}

		{
			// Call NVM Server API
			NVMS_FileRead_Status = NVMS_FileRead( ClientID,
							      pFile_Read_Request->hFile,
							      pFile_Read_Request->nItemSize,
							      pFile_Read_Request->nNumberOfItems,
							      &dwActual, ( LPVOID)pFile_Read_Response->DataBuffer);
			// Prepare Response buffer
			pFile_Read_Response->op_code = NVM_OP_CODE_FILE_READ;
			pFile_Read_Response->error_code = NVMS_FileRead_Status;
			pFile_Read_Response->nNumberActuallItemsRead = dwActual;
			if (dwActual)
			{
				pFile_Read_Response->nDataBufferLength = pFile_Read_Request->nItemSize * pFile_Read_Request->nNumberOfItems;
			}
			else
				pFile_Read_Response->nDataBufferLength = 0;

			//copy yhr actual buffer to the response buffer
			PrintBuffer(pFile_Read_Response->DataBuffer, dwActual, (char *)"Read from File ");
			dwHeaderSize = (sizeof( NVM_FILE_READ_RESPONSE_STRUCT) - sizeof( pFile_Read_Response->DataBuffer));


			/*	*(UINT32*)pRspBuffer = sizeof( File_Read_Response); */// fill in the length
			// free buffer
		}
		DEBUGMSG("[NVM Proxy]NVM_OP_CODE_FILE_READ  -\r\n");
		return sizeof( NVM_FILE_READ_RESPONSE_STRUCT);
	}
	case NVM_OP_CODE_RTC_READ:
	{
		RETAILMSG("[NVM Proxy]NVM_OP_CODE_RTC_READ + \r\n");

		RTC_GET_RESPONSE_STRUCT *presponseData = (RTC_GET_RESPONSE_STRUCT *)pRspBuffer;
		//SYSTEMTIME	systemTime;
		time_t clock;
		struct tm* local;

		// GetSystemTime(&systemTime);
		time(&clock);
		local = localtime(&clock);
		ASSERT(local);


		presponseData->op_code    = NVM_OP_CODE_RTC_READ;
		presponseData->error_code = NVM_STATUS_SUCCESS;

		presponseData->second             = (UINT8)local->tm_sec;
		presponseData->minute             = (UINT8)local->tm_min;
		presponseData->hour               = (UINT8)local->tm_hour;
		presponseData->day                = (UINT8)local->tm_mday;
		presponseData->month              = (UINT8)local->tm_mon;
		presponseData->year               = local->tm_year;

		/*	*(UINT32*)pRspBuffer = sizeof( RTC_GET_RESPONSE_STRUCT); */// fill in the length

		DEBUGMSG("[NVM Proxy]NVM_OP_CODE_RTC_READ  -\r\n");
		return sizeof( RTC_GET_RESPONSE_STRUCT);

	}
	default:
		DEBUGMSG("Error!! unknow OP CODE=%d", NVM_OPCode);
		return -1;
	}

//	return NVM_PARSER_RESULT_DONE;
}

