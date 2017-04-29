// (C) Copyright 2006 Marvell International Ltd.
// All Rights Reserved
//
/*
** INTEL CONFIDENTIAL
** Copyright 2000-2006 Intel Corporation All Rights Reserved.
**
** The source code contained or described herein and all documents
** related to the source code (Material) are owned by Intel Corporation
** or its suppliers or licensors.  Title to the Material remains with
** Intel Corporation or its suppliers and licensors. The Material contains
** trade secrets and proprietary and confidential information of Intel
** or its suppliers and licensors. The Material is protected by worldwide
** copyright and trade secret laws and treaty provisions. No part of the
** Material may be used, copied, reproduced, modified, published, uploaded,
** posted, transmitted, distributed, or disclosed in any way without Intel?s
** prior express written permission.
**
** No license under any patent, copyright, trade secret or other intellectual
** property right is granted to or conferred upon you by disclosure or
** delivery of the Materials, either expressly, by implication, inducement,
** estoppel or otherwise. Any license under such intellectual property rights
** must be express and approved by Intel in writing.
*/
#include <NVMServer_api.h>
#include <pxa_dbg.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>
#include <linux/rtc.h>
//#include <linux/reboot.h>

#include "diag.h"
#include "NVMDiag.h"
#include "diag_buff.h"
#include "EEHandler.h"

#define SANREMO_D4_CALI        "/sys/class/power_supply/battery/device/trim_d4"
#define SANREMO_D5D7_CALI      "/sys/class/power_supply/battery/device/trim_d5d7"
#define SANREMO_TRIM_RETURN    "/sys/class/power_supply/battery/device/trim_return"
#define SANREMO_TRIM_VOL       "/sys/class/power_supply/battery/device/trim_vol"
#define SANREMO_I_REMAIN       "/sys/class/power_supply/battery/device/trim_ibat_remain"
#define SANREMO_CALI           "/sys/class/power_supply/battery/device/calibration"
#define SANREMO_CALI_VBAT      "/sys/class/power_supply/battery/device/calibration_vbat"
#define SANREMO_CALI_IBAT      "/sys/class/power_supply/battery/device/calibration_ibat"

signed short ibat_offset =0;
unsigned int sanremo_trim_value;
unsigned short voltage[3];
unsigned short input_vol[3];

//ICAT EXPORTED STRUCT
typedef struct
{
	unsigned char cmd;
	short data;
}InputStruct;

//ICAT EXPORTED STRUCT
typedef struct
{
	unsigned char d4;
	unsigned char d5;
	unsigned char d7;
	signed short offset_ibat;
	signed short offset_vbat;
	unsigned short slope_low;
	unsigned short slope_high;
}CaliReturnStruct;

// Defines
#define MAX_READ_BUFFER_SIZE    1024
#define MAX_FILES_NAME_BUFFER   1024
#define MAX_FILE_NAME_LENGTH    256
#define MAX_FILE_MODE_LENGTH    256

#define FFS_WRAPPER_NUM_VOLUMES           6
#define FFS_WRAPPER_VOLUME_NAME_LENGTH    10
#define UNUSEDPARAM(param) (void)param;
//ICAT EXPORTED STRUCT
typedef struct
{
	char fileName[MAX_FILE_NAME_LENGTH];    // name of file located in Flash (or to be create)
	char mode[MAX_FILE_MODE_LENGTH];        // Opening mode (e.g. open for readonly)
}FOpenInputStruct;

//ICAT EXPORTED STRUCT
typedef struct
{
	UINT32 fileID;
	char fileName[MAX_FILE_NAME_LENGTH]; // name of file located in Flash memory.
} FOpenReturnStruct;

//ICAT EXPORTED STRUCT
typedef struct
{
	UINT32 fileID;                          // File ID of the file being handled.
	UINT16 PacketSize;                      // size of packet being transfered.
	char writeBuf[MAX_READ_BUFFER_SIZE];    // buffer of bytes to be written
}WriteInputStruct;

//ICAT EXPORTED STRUCT
typedef struct
{
	UINT32 fileID;
	UINT16 bufferSize;
}ReadInputStruct;

//ICAT EXPORTED STRUCT
typedef struct
{
	UINT16 PacketOK;                        // Contian the error code see FDI Doc (e.g. ERR_OK)
	UINT16 LastPacket;                      // equal to 1 if it's the last packet, 0 - otherwise.
	UINT16 PacketSize;                      // size in bytes of the buffer being read from file.
	char readBuf[MAX_READ_BUFFER_SIZE];     // buffer of bytes contains the Flash file data.
} ReadReturnStruct;

//ICAT EXPORTED STRUCT
typedef struct
{
	char fInfoList[MAX_FILES_NAME_BUFFER];                          // buffer contain file properties information (name, size, date etc.)
	UINT16 fListLen;                                                // Number of files describe in the buffer (fInfoList)
	UINT8 LastPacket;                                               // Flag which intecates whether this is the last structure contain file -
}FNamesReturnStruct;

//ICAT EXPORTED STRUCT
typedef struct
{
	char fOldName[MAX_FILE_NAME_LENGTH];
	char fNewName[MAX_FILE_NAME_LENGTH];
}RenameInputStruct;

//ICAT EXPORTED STRUCT
typedef struct
{
	char fileName[MAX_FILE_NAME_LENGTH];
	UINT32 newMode;
}ChangModeStruct;

//ICAT EXPORTED STRUCT
typedef struct
{
	UINT32 fileID;
	long offset;
	int whereFrom;
}FSeekStruct;

//ICAT EXPORTED STRUCT
typedef struct
{
	unsigned long long fdvSize;
}FdvSizeStruct;

//ICAT EXPORTED STRUCT
typedef struct
{
	UINT32 fdvAvailableSpace;
}FdvAvailableSpaceStruct;

//ICAT EXPORTED STRUCT
typedef struct
{
	char dirName[MAX_FILE_NAME_LENGTH];
}RmDirStruct;

//ICAT EXPORTED STRUCT
typedef struct
{
	char dirName[MAX_FILE_NAME_LENGTH];
	UINT16 mode;
}MkDirStruct;

//ICAT EXPORTED STRUCT
typedef struct
{
	UINT8 ffs_num_volumes;
	char volumes[FFS_WRAPPER_NUM_VOLUMES][FFS_WRAPPER_VOLUME_NAME_LENGTH];
}FdvVolumesStruct;

NVM_CLIENT_ID nvm_acat_client_id = NVM_CLIENT_ACAT;

//ICAT EXPORTED FUNCTION - Diag,Utils,OutMsgBodyLimit
UINT32 diagGetOutMsgBodyLimit(void)
{
#define DIAG_COMM_EXTIF_ETH_MAX_BUFF_SIZE 4096
	//YK change -100 to value of FOOTER+HEADER (sizeof(TxPDU)+TX_PDU_FOOTER_SIZE)
	UINT32 msgBodySize = (_diagInternalData.MsgLimits.diagMaxMsgOutLimit - 100);
/* deduct 100 bytes ~= message header overhead */
#if !defined (DIAGNEWHEADER)
	//MB - This should be set to the corrcet value depending on te header used.
	//At this point this is a work around for the worst case.(ethernet-l2/l4)
	msgBodySize = DIAG_COMM_EXTIF_ETH_MAX_BUFF_SIZE - 100;
#if defined (OSA_LINUX)
//Due to a problem found in Linux this number was reduced need further research - workaround
	msgBodySize = 0x400;
#endif
#endif
	DIAG_FILTER(Diag, Utils, MaxOutMsgBodySize, DIAG_INFORMATION)
	diagPrintf("diag(target) to ACAT: %ld", msgBodySize);

	return msgBodySize;
}


/******************************************************************************
* Function	 : _Fopen
*******************************************************************************
*																			 *
* Description  : Open file for read/write									 *
*																			 *
* Input:		 FOpenInputStruct.fileName - file name to be opened.		  *
*				FOpenInputStruct.mode - instruct weather to open the file to.*
*				read / write / truncate (rb, wb, wb+ etc.) for more see FDI  *
*				Documentation.											   *																			 *
* Output:		Success: FOpenReturnStruct.fileID - opened file ID.		  *
*							FOpenReturnStruct.fileName - opened file name.   *
*							(to avoid several file open output mistakes).	*
*				Error:  Error message with the error code.				   *
******************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,Fopen
void _Fopen(void* fInOpenStruct)
{
	UINT32 hFileID;
	FOpenInputStruct* fOpenInSt = (FOpenInputStruct*)fInOpenStruct;

	if (NVMS_FileOpen(nvm_acat_client_id, fOpenInSt->fileName, fOpenInSt->mode,
			  &hFileID) == NVM_STATUS_SUCCESS)
	{
		FOpenReturnStruct retOpenStruct;
		retOpenStruct.fileID = hFileID;
		strncpy(retOpenStruct.fileName, fOpenInSt->fileName, MAX_FILE_NAME_LENGTH);
		DEBUGMSG("%s reports %d, %s\n", __func__, retOpenStruct.fileID, retOpenStruct.fileName);

		DIAG_FILTER(FDI, Transport, F_Open_Return_FileID, DIAG_INFORMATION)
		diagStructPrintf("%S{FOpenReturnStruct}", (void *)&retOpenStruct, sizeof(FOpenReturnStruct));
	}
	else
	{
		DIAG_FILTER(FDI, Transport, F_Open_Error, DIAG_INFORMATION)
		diagPrintf("fopen %s error", fOpenInSt->fileName);
	}
}

/******************************************************************************
* Function	 : _Fclose
*******************************************************************************
*																			 *
* Description  : close file												   *
*																			 *
* Input:		 fileIDStruct - ID of an open file.						   *
*																			 *
* Output:		Success: success message with file ID.					   *
*				Error:   Error message with error number.					*
******************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,Fclose
void _Fclose(void *fileID)
{
	UINT32 hFileID = *((UINT32 *)fileID);

	if (NVMS_FileClose(nvm_acat_client_id, hFileID) == NVM_STATUS_SUCCESS)
	{
		DIAG_FILTER(FDI, Transport, F_Close_Success, DIAG_INFORMATION)
		diagPrintf("File (ID: %lu) was succesfully closed", hFileID);
	}
	else
	{
		DIAG_FILTER(FDI, Transport, F_Close_Error, DIAG_INFORMATION)
		diagPrintf("File (ID: %lu) close error", hFileID);
	}
}

/************************************************************************************
* Function	 : _Fread
************************************************************************************
*																				  *
* Description  : read constant size buffer from an open flash file.				*
*																				  *
* Input:		 ReadInputStruct.fileID - ID of open file to be read			   *
*				ReadInputStruct.bufferSize - read buffer size (currenly not in use)
*																				  *
* Output:		Success:
*				  InReadReturnStruct.PacketOK -  no error occurred while reading  *
*				  ReadReturnStruct.LastPacket - last packet read form file		*
*				  eadReturnStruct.PacketSize - size of read packet/buffer		 *
*				  ReadReturnStruct.readBuf - buffer of bytes read form file	   *
*				Error: Error message
************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,Fread
void _Fread(void *pReadStruct)
{
	static ReadReturnStruct* pRetReadBuf = NULL;
	static UINT32 nReadBufAllocSize = 0;
	UINT32 numBytesToRead, numActualReadBytes, nReadBufReqSize;

	UINT32 hFileID = ((ReadInputStruct*)pReadStruct)->fileID;

	numBytesToRead = (UINT32)(((ReadInputStruct*)pReadStruct)->bufferSize);

	if ( numBytesToRead > diagGetOutMsgBodyLimit())
	{
		DIAG_FILTER(FDI, Transport, F_Read_Bad_Input, DIAG_INFORMATION)
		diagPrintf("Number of packets to read exceeded the maximal number (%d > %d)", numBytesToRead, diagGetOutMsgBodyLimit());
		return;
	}

	/*
	   for backward compatiblity the smallest size of buffer is sizeof(ReadReturnStruct)
	   which already contain 1024 bytes buffer for file content. the following assignment calculate the number of bytes
	   required in addition to this structure, with consideration of DIAG max OUT message body limitation
	 */
	nReadBufReqSize = (numBytesToRead > MAX_READ_BUFFER_SIZE) ?
			  (sizeof(ReadReturnStruct) + numBytesToRead - MAX_READ_BUFFER_SIZE) :
			  sizeof(ReadReturnStruct);
	/* if buffer was not allocated yet or its allocated size is smaller than requested now */
	if (!pRetReadBuf || (nReadBufAllocSize < nReadBufReqSize) )
	{
		if (pRetReadBuf)        /* means that allocated buffer size is smaller than read-size requested now */
			free(pRetReadBuf);

		pRetReadBuf = (ReadReturnStruct*)(malloc(nReadBufReqSize));
	}

	if (pRetReadBuf)
	{
		nReadBufAllocSize = nReadBufReqSize;
		if (NVMS_FileRead(nvm_acat_client_id, hFileID, sizeof(char), numBytesToRead, &numActualReadBytes,
				  pRetReadBuf->readBuf) == NVM_STATUS_SUCCESS)
		{
			pRetReadBuf->PacketSize = (UINT16)numActualReadBytes;
			if (numActualReadBytes == numBytesToRead)
			{
				pRetReadBuf->PacketOK = ERR_NONE;       // Update the return struct to contain any error information.
				pRetReadBuf->LastPacket = FALSE;
			}
			else   /*if(numActualReadBytes < numBytesToRead)*/
			{
				pRetReadBuf->PacketOK = ERR_EOF;
				pRetReadBuf->LastPacket = TRUE;
			}

			DIAG_FILTER(FDI, Transport, F_Read_Packet, DIAG_INFORMATION)
			diagStructPrintf("%S{ReadReturnStruct}", (void *)pRetReadBuf, nReadBufAllocSize);
		}
		else
		{
			DIAG_FILTER(FDI, Transport, F_Read_Error, DIAG_INFORMATION)
			diagPrintf("File read error (ID: %lu)", hFileID);
		}
	}
	else            //Unsuccessful allocation
	{
		nReadBufAllocSize = 0;
		DIAG_FILTER(FDI, Transport, F_Read_Malloc_Error, DIAG_INFORMATION)
		diagPrintf("Malloc in Fread failed");
	}
}

/************************************************************************************
* Function	 : _Fwrite
************************************************************************************
*																				  *
* Description  : write file to flash.											  *
*																				  *
* Input:		 WriteInputStruct.fileID - open flash file ID.					 *
*				WriteInputStruct.PacketSize - size of the buffer to be write	  *
*				WriteInputStruct.writeBuf - bytes buffer to be write/append to open file *
*																				  *
* Output:		Success: Success message with the number of written bytes.		*
*				Error: Error message with error code.							 *
************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,Fwrite
void _Fwrite(void * pWriteStruct)
{
	WriteInputStruct *pws = (WriteInputStruct *)pWriteStruct;
	UINT32 numberOfBytesWritten = 0;
	UINT32 numBytesToWrite = (UINT32)(pws->PacketSize);

	if (NVMS_FileWrite(nvm_acat_client_id, pws->fileID, pws->writeBuf, numBytesToWrite,
			   sizeof(char), numBytesToWrite, &numberOfBytesWritten) == NVM_STATUS_SUCCESS
	    && numberOfBytesWritten == numBytesToWrite)
	{
		DIAG_FILTER(FDI, Transport, F_Write_Success, DIAG_INFORMATION)
		diagPrintf("The Packet was written successfully (packet size %d)", numBytesToWrite);
	}
	else
	{
		DIAG_FILTER(FDI, Transport, F_Write_Error, DIAG_INFORMATION)
		diagPrintf("File write error (ID: %lu)", pws->fileID);
	}
}

/************************************************************************************
* Function	 : _Format
************************************************************************************
*																				  *
* Description  : Format flash													  *
*																				  *
* Input:		 None															  *
*																				  *
* Output:		Success: Success message.										 *
*				Error: Error message.											 *
************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,Format_Flash
void _Format(void)
{
	DIAG_FILTER(FDI, Transport, Format_Error, DIAG_INFORMATION)
	diagPrintf("Flash format is NOT supported");
}

/************************************************************************************
* Function	 : _Remove
************************************************************************************
*																				  *
* Description  : Delete file from flash											*
*																				  *
* Input:		 file name														 *
*																				  *
* Output:		Success: Success message.										 *
*				Error: Error message including the error code.					*
************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,Remove_File
void _Remove(void* fileName)
{
	if (NVMS_FileDelete(nvm_acat_client_id, (char *)fileName) == NVM_STATUS_SUCCESS)
	{
		DIAG_FILTER(FDI, Transport, Remove_Success, DIAG_INFORMATION)
		diagPrintf("The file was successfully deleted");
	}
	else
	{
		DIAG_FILTER(FDI, Transport, Remove_Error, DIAG_INFORMATION)
		diagPrintf("An error occurred while deleting file");
	}
}

/************************************************************************************
* Function	 : _GetFileNameList
************************************************************************************
*																				  *
* Description  : Retrieve a list of flash file names and information			   *
*																				  *
* Input:		 wildcard characters,when placed within filename search criteria,  *
*				perform specific search functions								 *
*																				  *
* Output:		FNamesReturnStruct.fInfoList - list of file names and info separated by '@' character  *
*				FNamesReturnStruct.fListLen - the above list lenght.								   *
*				FNamesReturnStruct.LastPacket - bolean flag to indecate on last file info list		 *
************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,GetFileNameList
void _GetFileNameList(void* wildcard)
{
	NVM_FILE_INFO file_info;                                                                /* Holds the file information			   */
	char sFDI[NVM_FILE_NAME_MAX_LENGTH + 41];                                               // max num of digits recieved from DWORD/int (type of file attributes) = 10 + file name length + 1 (NULL terminator)
	char sDir[NVM_FILE_NAME_MAX_LENGTH];
	char sPath[NVM_FILE_NAME_MAX_LENGTH];
	char *psDir;
	NVM_STATUS_T ret_val;
	INT16 nFInfoListLen;
	FNamesReturnStruct retFNamesStruct;
	HANDLE hsearch = 0;
	UINT32 fStatus;

	strncpy(sDir, wildcard, NVM_FILE_NAME_MAX_LENGTH);

	psDir = dirname(sDir);
	memmove(sDir, psDir, strlen(psDir) + 1);
	strncat(sDir, "/", NVM_FILE_NAME_MAX_LENGTH);

	ret_val = NVMS_FileFindFirst(nvm_acat_client_id, (char*)wildcard, &file_info, &hsearch);
	if (ret_val != NVM_STATUS_SUCCESS)
	{
		DIAG_FILTER(FDI, Transport, FName_LIST_NOTEXISTS, DIAG_INFORMATION)
		diagPrintf("Currently there are no files on flash matching wildcard - %s", (char *)wildcard);
		return;
	}

	do {
		retFNamesStruct.LastPacket = 1;
		retFNamesStruct.fListLen = 0;                   /* Length meauered in number of files in the list */
		retFNamesStruct.fInfoList[0] = 0;
		nFInfoListLen = 0;
		do {
			fStatus = 0;
			strncpy(sPath, sDir, NVM_FILE_NAME_MAX_LENGTH);
			strncat(sPath, (char *)file_info.file_name, NVM_FILE_NAME_MAX_LENGTH);
			NVMS_GetFileStat(nvm_acat_client_id, sPath, &fStatus);
			nFInfoListLen += sprintf(sFDI, "%s@%d@%d@%u@%u@", (char*)file_info.file_name, file_info.time, file_info.date, file_info.size, fStatus);

			/* Check if file properties list will not exceed maximum list buffer length (+1 counts the '\n' at the end) */
			if ((nFInfoListLen + 1) > MAX_FILES_NAME_BUFFER)
			{
				retFNamesStruct.LastPacket = 0;                          /* Indicates on packet which DOES NOT contain all flash files information */
				break;
			}
			retFNamesStruct.fListLen++;                                       /* counts number of files */
			strncat(retFNamesStruct.fInfoList, sFDI, MAX_FILES_NAME_BUFFER);

			ret_val = NVMS_FileFindNext(nvm_acat_client_id, &file_info, &hsearch);
		} while (ret_val == NVM_STATUS_SUCCESS);                                                                   /* Loop until we don't find anymore files.  */

		DIAG_FILTER(FDI, Transport, F_NameList_Packet, DIAG_INFORMATION)
		diagStructPrintf("%S{FNamesReturnStruct}", (void *)&retFNamesStruct, sizeof(FNamesReturnStruct));
	} while (retFNamesStruct.LastPacket != (UINT8)1);                                         /* Loop until we don't find anymore files.  */

	NVMS_FileFindClose(nvm_acat_client_id, &hsearch);
}

/************************************************************************************
* Function	 : _RenameFile
************************************************************************************
*																				  *
* Description  : Delete file from flash											*
*																				  *
* Input:		 RenameInputStruct.fOldName - file name to be renamed			  *
*				RenameInputStruct.fNewName - new file name						*
* Output:		Success: Success message.										 *
*				Error: Error message including the error code.					*
************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,RenameFile
void _RenameFile(void* pRenameStruct)
{
	RenameInputStruct* pRenameSt = (RenameInputStruct*)pRenameStruct;

	if (NVMS_FileRename(nvm_acat_client_id, pRenameSt->fOldName, pRenameSt->fNewName) == NVM_STATUS_SUCCESS)
	{
		DIAG_FILTER(FDI, Transport, F_Rename_Succes, DIAG_INFORMATION)
		diagPrintf("The file was successfully renamed to %s", pRenameSt->fNewName);
	}
	else
	{
		DIAG_FILTER(FDI, Transport, F_Rename_Error, DIAG_INFORMATION)
		diagPrintf("Error occured while trying to change file name");
	}
}

/************************************************************************************
* Function	 : _FStatus
************************************************************************************
*																				  *
* Description  : change file statuse											   *
*																				  *
* Input:		 fileName - file name to query it status						   *
*																				  *
* Output:		Success: file status.											 *
*				Error: Error message including the error code.					*
************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,FStatus
void _FStatus(void* fileName)
{
	INT32 fStatus;

	if (NVMS_GetFileStat(nvm_acat_client_id, (char*)fileName, (PDWORD)&fStatus) == NVM_STATUS_SUCCESS)
	{
		DIAG_FILTER(FDI, Transport, F_Status_Success, DIAG_INFORMATION)
		diagPrintf("(%d)", fStatus);
	}
	else
	{
		DIAG_FILTER(FDI, Transport, F_Status_Error, DIAG_INFORMATION)
		diagPrintf("An error occurred while checking the file status");
	}
}

/************************************************************************************
* Function	 : _FChangeMode
************************************************************************************
*																				  *
* Description  : change file status											   *
*																				  *
* Input:		 ChangModeStruct.fileName - name of the file its mode should be changed.  *
*				ChangModeStruct.newMode - new mode to be set for the file mention above. *																 *
*																				  *
* Output:		Success: success message										  *
*				Error: Error message including file name.						 *
************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,FchangeMode
void _FChangeMode(void* stInChMode)
{
	char *fileName = (char*)(((ChangModeStruct*)stInChMode)->fileName);
	INT32 newMode = (INT32)(((ChangModeStruct*)stInChMode)->newMode);

	if (NVMS_FileChangeMode(nvm_acat_client_id, fileName, newMode) == NVM_STATUS_SUCCESS)
	{
		DIAG_FILTER(FDI, Transport, F_ChangeMode_Success, DIAG_INFORMATION)
		diagPrintf("mode of %s was successfully changed", fileName);
	}
	else
	{
		DIAG_FILTER(FDI, Transport, F_ChangeMode_Error, DIAG_INFORMATION)
		diagPrintf("An error occurred while changing file[%s] to mode %lu", fileName, newMode);
	}
}

/********************************************************************************************
 * Function	 : _Fseek
 ********************************************************************************************
 *																				                *
 * Description  : Seek within a file. Sets a new position to the file indicator .	                *
 *                                The new position, measured in bytes from the beginning of the file,	   *
 *                                is obtained by adding offset to the position specified by wherefrom	   *
 *																				                *
 * Input:		 FSeekStruct.fileID - Handle to open file							                *
 *				FSeekStruct.offset - Byte offset from position specified by wherefrom.          *
 *				FSeekStruct.whereFrom - indicating from what point in the file the offset should be measured.  *
 *																				                *
 * Output:		Success: success message										                *
 *				Error: Error message including file name.						                *
 *******************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,Fseek
void _Fseek(void* stFSeek)
{
	FSeekStruct* fdvSeek = (FSeekStruct*)(stFSeek);

	if (NVMS_FileSeek(nvm_acat_client_id, fdvSeek->fileID, fdvSeek->offset, fdvSeek->whereFrom) == NVM_STATUS_SUCCESS)
	{
		DIAG_FILTER(FDI, Transport, Fseek_Success, DIAG_INFORMATION)
		diagPrintf("File (ID: %lu) indicator was offset by %lu from %lu", fdvSeek->fileID, fdvSeek->offset, fdvSeek->whereFrom);
	}
	else
	{
		DIAG_FILTER(FDI, Transport, Fseek_Error, DIAG_INFORMATION)
		diagPrintf("Error occured while trying to seek within file (ID: %lu)", fdvSeek->fileID);
	}
}

/********************************************************************************************
 * Function	 : _GetMaxFileNameLength
 ********************************************************************************************
 *																				                *
 * Description  : Returns the number of maximum characters permited for file name in FDI FDV. *
 *																				                *
 * Input:		 None																		*
 * Output:		a number indicates on the maximum valid file name							*
 *																							*
 *******************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,GetMaxFileNameLen
void _GetMaxFileNameLength()
{
	DIAG_FILTER(FDI, Transport, MaxFileNameLen, DIAG_INFORMATION)
	diagPrintf("%d", MAX_FILE_NAME_LENGTH);
}

/********************************************************************************************
 * Function	 : _GetFdiFdvSize
 ********************************************************************************************
 *																				                *
 * Description  :  Returns the total space in byte of the Fdi Data Volume.					*
 *																				                *
 * Input:                  None																		*
 * Output:		 Buffer of 64 bytes contains the total space in byte of the Fdi Data Volume *
 *																							*
 *******************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,GetFdiFdvSize
void _GetFdiFdvSize()
{
	FdvSizeStruct fdvSize;

	if (NVMS_GetTotalSpace(nvm_acat_client_id, (UINT32 *)(&(fdvSize.fdvSize))) == NVM_STATUS_SUCCESS)
	{
		DIAG_FILTER(FDI, Transport, FdvSize, DIAG_INFORMATION)
		diagStructPrintf("%S{FdvSizeStruct}", (void *)&fdvSize, sizeof(FdvSizeStruct));
	}
	else
	{
		DIAG_FILTER(FDI, Transport, FdvSize_Error, DIAG_ERROR)
		diagPrintf("Error: cannot get NVM size");
	}
}

/*********************************************************************************************
 * Function	 : _GetFdiFdvAvailableSpace
 *********************************************************************************************
 *																				                 *
 * Description  :  Returns the total available space (in bytes) of the Fdi Data Volume (FDV).*
 *																				                 *
 * Input:                  None																		 *
 * Output:		 Get the space abailable in the flash data volume for new file/data objects*
 *																							 *
 *******************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,GetFdiFdvAvailableSpace
void _GetFdiFdvAvailableSpace(void *volName)
{
	FdvAvailableSpaceStruct fdvSize;

	if (NVMS_GetAvailSpace(nvm_acat_client_id, volName, &(fdvSize.fdvAvailableSpace)) == NVM_STATUS_SUCCESS)
	{
		DIAG_FILTER(FDI, Transport, FdvAvailableSize, DIAG_INFORMATION)
		diagStructPrintf("%S{FdvAvailableSpaceStruct}", (void *)&fdvSize, sizeof(FdvAvailableSpaceStruct));
	}
	else
	{
		DIAG_FILTER(FDI, Transport, FdvAvailableSize_Error, DIAG_ERROR)
		diagPrintf("Error: cannot get available NVM space");
	}
}

/********************************************************************************************
 * Function	 : _GetFdiVersion
 ********************************************************************************************
 *																				                *
 * Description  :  Returns the software version of the FDI.									*
 *																				                *
 * Input:                  None																		*
 * Output:		 Fdi Version                                                                                                                            *
 *******************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,GetFdiVersion
void _GetFdiVersion()
{
	DIAG_FILTER(FDI, Transport, FDI71, DIAG_INFORMATION)
	diagPrintf("FDI 71");
	DIAG_FILTER(FDI, Transport, FDI721, DIAG_INFORMATION)
	diagPrintf("Aug06:upgrade to FDI721");
}

/******************************************************************************
* Function     : _MkDir
*******************************************************************************
*                                                                             *
* Description  : Open direcrory for read/write                                *
*                                                                             *
* Input:         MkDirStruct.dirName - directory name to be opened.           *
*                MkDirStruct.mode - instruct weather to open the directory    *
*                for read / write / truncate (rb, wb, wb+ etc.)               *
*                for more see FDIDocumentation.                               *                                                                             *
* Output:        Success: Success message with success code.                  *
*                Error:  Error message with the error code.                   *
******************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,MkDir
void _MkDir(void* MakeDirStruct)
{
	MkDirStruct* MakeDirSt = (MkDirStruct*)MakeDirStruct;

	if (NVMS_MkDir(nvm_acat_client_id, MakeDirSt->dirName, MakeDirSt->mode) == NVM_STATUS_SUCCESS)
	{
		DIAG_FILTER(FDI, Transport, mkdir_Success, DIAG_INFORMATION)
		diagPrintf("Make Directory: %s", MakeDirSt->dirName);
	}
	else
	{
		DIAG_FILTER(FDI, Transport, mkdir_Error, DIAG_ERROR)
		diagPrintf("mkdir %s error", MakeDirSt->dirName);
	}
}

/******************************************************************************
* Function     : _RmDir
*******************************************************************************
*                                                                             *
* Description  : Remove direcrory                                             *
*                                                                             *
* Input:         RmDirStruct.dirName - directory name to be removed.          *
* Output:        Success: Success message with success code.                  *
*                Error:  Error message with the error code.                   *
******************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,RmDir
void _RmDir(void* RemoveDirStruct)
{
	RmDirStruct* RemoveDirSt = (RmDirStruct*)RemoveDirStruct;

	if (NVMS_RmDir(nvm_acat_client_id, RemoveDirSt->dirName) == NVM_STATUS_SUCCESS)
	{
		DIAG_FILTER(FDI, Transport, Rmdir_Success, DIAG_INFORMATION)
		diagPrintf("Remove Directory: %s", RemoveDirSt->dirName);
	}
	else
	{
		DIAG_FILTER(FDI, Transport, Rmdir_Error, DIAG_ERROR)
		diagPrintf("rmdir: %s error", RemoveDirSt->dirName);
	}
}

/********************************************************************************************
 * Function     : _GetFdiVolumes
 ********************************************************************************************
 *                                                                                              *
 * Description  :  Returns Volumes name and number  (in TCHAR format)                       *
 *                                                                                              *
 * Input:                  None																		*
 * Output:         retFdvVolumesStruct in 2 char format                                     *
 *																							*
 *******************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,GetFdiVolumes
void _GetFdiVolumes()
{
	int i = 0;
	FdvVolumesStruct retFdvVolumesStruct;

	memset(&retFdvVolumesStruct, 0, sizeof(FdvVolumesStruct));

	/* RootFS with absolute path */
	strncpy(retFdvVolumesStruct.volumes[i++], "/", FFS_WRAPPER_VOLUME_NAME_LENGTH);

	retFdvVolumesStruct.ffs_num_volumes = i;
	DIAG_FILTER(FDI, Transport, GetFdiVolumes_List, DIAG_INFORMATION)
	diagStructPrintf("%S{FdvVolumesStruct}", (void*)&retFdvVolumesStruct, sizeof(FdvVolumesStruct));
}

//ICAT EXPORTED FUNCTION - FDI,Transport,ExtendedMode
void _ExtendetMode()
{
	DIAG_FILTER(FDI, Transport, ExtendetMode, DIAG_INFORMATION)
	diagPrintf("FDI7 Extendet Mode is ON");
}
static time_t read_rtct(int utc)
{
	int rtc;
	struct tm tm;
	char *oldtz = 0;
	time_t t = 0;

	if (( rtc = open ( "/dev/rtc0", O_RDONLY )) < 0 ) {
		ERRMSG( "Could not access RTC" );
		return -1;
	}
	memset ( &tm, 0, sizeof( struct tm ));
	if ( ioctl ( rtc, RTC_RD_TIME, &tm ) < 0 )
		ERRMSG ( "Could not read time from RTC" );
	tm. tm_isdst = -1; // not known

	close ( rtc );

	if ( utc ) {
		oldtz = getenv ( "TZ" );
		setenv ( "TZ", "UTC 0", 1 );
		tzset ( );
	}

	t = mktime ( &tm );

	if ( utc ) {
		if ( oldtz )
			setenv ( "TZ", oldtz, 1 );
		else
			unsetenv ( "TZ" );
		tzset ( );
	}
	return t;
}

static int to_sys_clock(int utc)
{
	struct timeval tv = { 0, 0 };
	const struct timezone tz = { timezone/60 - 60*daylight, 0 };

	if((tv. tv_sec = read_rtct ( utc )) < 0)
	{
		ERRMSG(" read rtct error");
		return -1;
	}

	if ( settimeofday ( &tv, &tz ))
		ERRMSG( "settimeofday() failed" );

	return 0;
}

/* FIXME: Some Linux specific misc functions - put them here now */
int write_rtc(struct tm *tm)
{
	int rtcfd;
	int ret;

	rtcfd = open("/dev/rtc0", O_RDWR);

	if (rtcfd < 0)
	{
		ERRMSG("%s: Error to open /dev/rtc0\n", __FUNCTION__);
		return -1;
	}
	DEBUGMSG("%s:tm is: %s\n", __FUNCTION__, asctime(tm));
	if ( (ret = ioctl (rtcfd, RTC_SET_TIME, tm)) < 0)
		ERRMSG("%s: Error to write rtc\n", __FUNCTION__);
	close(rtcfd);
	return ret;
}



/***********************************************************************************
 * Function	 : _SetRTC
 ***********************************************************************************
 *
 * Description  : Initialize the RTC with input parameter RTC_CalendarTime structure.
 *																				  *
 * Input:		 RTC_CalendarTime structure.									   *
 *																				  *
 * Output:		Success: Success message.										 *
 *				Error: Error message.											 *
 * Notes:		 Not functional in wince, just a dummy one.
 ************************************************************************************/
//ICAT EXPORTED FUNCTION - Diag,Utils,SetRTC
void _SetRTC(void *pDateAmdTime)
{
	UNUSEDPARAM(pDateAmdTime)
	RTC_CalendarTime * Ctime = pDateAmdTime;
	struct tm curgmtime;
	time_t curtime;
	int utc = 0;

	curgmtime.tm_mday = Ctime->day;
	curgmtime.tm_mon =  Ctime->month-1;
	curgmtime.tm_year = Ctime->year-1900;
	curgmtime.tm_hour = Ctime->hour;
	curgmtime.tm_min = Ctime->minute;
	curgmtime.tm_sec = Ctime->second;
	curgmtime.tm_isdst = 0;
	utc = 1;
	DEBUGMSG("utc is %d\n",utc);
	curtime =mktime(&curgmtime);
	if(write_rtc(&curgmtime)<0)
	{
		DIAG_FILTER(Diag, Utils, SetRTC_Failure, DIAG_INFORMATION)
		diagPrintf("RTC set failed, check the input parameter validity.");

	}
	else
	{
		to_sys_clock(utc);
		DIAG_FILTER(Diag, Utils, SetRTC_Success, DIAG_INFORMATION)
		diagPrintf("RTC was successfully set.");
	}


}

/***********************************************************************************
 * Function	 : _ReadRTC
 ***********************************************************************************
 *
 * Description  : Read the value of the RTC return its value.
 *																				  *
 * Input:		 None															  *
 *																				  *
 * Output:		Success: Target current time and data information.				*
 *				Error: Failure message.										   *
 ************************************************************************************/
//ICAT EXPORTED FUNCTION - Diag,Utils,ReadRTC
void _ReadRTC(void)
{
	time_t curtime;
	struct tm *curgmtime;
	RTC_CalendarTime Ctime;
	if (1)
	{
		char sCalanderTime[256];
		time(&curtime);
		curgmtime = gmtime(&curtime);
		sprintf(sCalanderTime, "The RTC Value is: %d/%d/%d, %d:%d:%d",
			curgmtime->tm_mday,
			curgmtime->tm_mon + 1,
			curgmtime->tm_year + 1900,
			curgmtime->tm_hour,
			curgmtime->tm_min,
			curgmtime->tm_sec);
		Ctime.day = curgmtime->tm_mday;
		Ctime.hour = curgmtime->tm_hour;
		Ctime.minute = curgmtime->tm_min;
		Ctime.month = curgmtime->tm_mon + 1;
		Ctime.second = curgmtime->tm_sec;
		Ctime.year = curgmtime->tm_year + 1900;
		DIAG_FILTER(Diag, Utils, ReadRTC_Calendar, DIAG_INFORMATION)
		diagStructPrintf("The CaledarTime is %S{RTC_CalendarTime}", (void *)&Ctime, sizeof(RTC_CalendarTime));

		DIAG_FILTER(Diag, Utils, ReadRTC_Success, DIAG_INFORMATION)
		diagPrintf("%s", sCalanderTime);
		return;
	}
	else
	{
		DIAG_FILTER(Diag, Utils, ReadRTC_Failure, DIAG_INFORMATION)
		diagPrintf("Failed to read RTC");
	}
}

/*******************************************************************************
* Function: EeHandlerTestDataAbortExcep
*******************************************************************************
* Description:  for testing - data abort aligment
*
* Parameters:
* Output Param:
*
* Return value:
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - EE_HANDLER,EE,EE_Test_data_abort
void EeHandlerTestDataAbortExcep( void )
{
	int i = 2;

	*(volatile int*)(0x1807bbff) = i;
}

/*******************************************************************************
* Function: EeHandlerTestAssert
*******************************************************************************
* Description:  for testing - Assert
*
* Parameters:
* Output Param:
*
* Return value:
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - EE_HANDLER,EE,EE_Test_Assert
void EeHandlerTestAssert( void )
{
	ASSERT(FALSE);
}


/***********************************************************************************
 * Function     : _GetVersionDiag
 ***********************************************************************************
 *
 * Description  : send the diag version
 *                                                                                  *
 * Input:         None                                                              *
 *                                                                                  *
 * Output:
 *
 ************************************************************************************/
//ICAT EXPORTED FUNCTION - Diag,Utils,GetVersionDiag
void _GetVersionDiag(void)
{
	DIAG_FILTER(Diag, Utils, SWVersion, DIAG_INFORMATION)
	diagPrintf("Tavor Linux V6.6");
}

static void ap_reboot()
{
	DIAG_FILTER(VALI_IF, MISC_IF, RESET_IF_Info, DIAG_INFORMATION)
	diagPrintf("Reset...");
	sync();
	//if(reboot(LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, LINUX_REBOOT_CMD_RESTART, NULL)) {
	if (system("/sbin/reboot"))
	{
		DIAG_FILTER(VALI_IF, MISC_IF, RESET_IF_Fail, DIAG_INFORMATION)
		diagPrintf("Reset failed.");
	}
	sleep(10000);
	DIAG_FILTER(VALI_IF, MISC_IF, RESET_IF_Timeout, DIAG_INFORMATION)
	diagPrintf("Reset timeout.");
}

/* disable reset_target command temporarily for Grait */
//ICAT EXPORTED FUNCTION - VALI_IF, MISC_IF, temp_RESET_IF
void ResetTarget()
{
	ap_reboot();
}

/* FIXME: ISPT functions - put them here temporarily */

#define ISPT_DEV                        "/dev/ispt"
#define MAX_DEVNAME_LEN         32
#define ISPT_IOC_MAGIC          'T'
#define ISPT_START                      _IOW(ISPT_IOC_MAGIC, 0, int)
#define ISPT_STOP                       _IOW(ISPT_IOC_MAGIC, 1, int)
#define ISPT_GET_DRV_NUM        _IOR(ISPT_IOC_MAGIC, 2, int *)
#define ISPT_GET_DRV_LIST       _IOR(ISPT_IOC_MAGIC, 3, void *)

/* This structure is used to dump device name list */
struct name_list {
	int id;
	char name[MAX_DEVNAME_LEN];
};

/*For CT_Panalyzer SW  on the PC side*/
#define VIF_MAX_IPM_CLIENTS 32

//ICAT EXPORTED STRUCT
typedef struct ouputIPM_DrvDetails_Tag
{
	UINT16 id;
	UINT16 name[32];
}ouputIPM_DrvDetails;

//ICAT EXPORTED STRUCT
typedef struct IPM_Drv_Export_struct_TYPE_Tag
{
	ouputIPM_DrvDetails IPMDrv_Params[VIF_MAX_IPM_CLIENTS];
}IPM_Drv_Export_struct_TYPE;

//ICAT EXPORTED FUNCTION - VALI_IF, CT_P_API, INIT_CT_Panalyzer
void INIT_CT_Panalyzer()
{
	int fd = open(ISPT_DEV, O_RDWR);

	if (fd <= 0)
		return;

	if (ioctl(fd, ISPT_START) == 0)
	{
		DIAG_FILTER(VALI_IF, CT_P_API, INIT_SUCCESS, DIAG_INFORMATION)
		diagPrintf("[CT_P] CT_Panalyzer INIT success");
	}

	close(fd);
}

//ICAT EXPORTED FUNCTION - VALI_IF, CT_P_API, TERM_CT_Panalyzer
void TERM_CT_Panalyzer()
{
	int fd = open(ISPT_DEV, O_RDWR);

	if (fd <= 0)
		return;

	if (ioctl(fd, ISPT_STOP) == 0)
	{
		DIAG_FILTER(VALI_IF, CT_P_API, TERM_SUCCESS, DIAG_INFORMATION)
		diagPrintf("[CT_P] CT_Panalyzer TERM success");
	}

	close(fd);
}

//ICAT EXPORTED FUNCTION - Validation, Utils, Get_IPM_Drivers_List
void Get_IPM_Drivers_List()
{

	/* Coverity CID=238 */
	int fd = -1;
	int num;
	int i;
	IPM_Drv_Export_struct_TYPE retData;
	struct name_list *p = NULL;
	struct name_list *cur = NULL;

	do {
		p = (struct name_list *)malloc(/*PAGE_SIZE*/ 4096);
		if (p == NULL)
			break;

		fd = open(ISPT_DEV, O_RDWR);
		if (fd <= 0)
			break;

		if (ioctl(fd, ISPT_GET_DRV_NUM, &num) < 0)
			break;

		if (num <= 0 || num > VIF_MAX_IPM_CLIENTS)
			break;

		if (ioctl(fd, ISPT_GET_DRV_LIST, p) < 0)
			break;

		memset(&retData, 0, sizeof(IPM_Drv_Export_struct_TYPE));
		for (i = 0, cur = p; i < num; i++, cur++)
		{
			retData.IPMDrv_Params[i].id = cur->id;
			strncpy((char *)retData.IPMDrv_Params[i].name, cur->name, MAX_DEVNAME_LEN);
		}

		DIAG_FILTER(Validation, Utils, Export_IPMDrv_list, DIAG_INFORMATION)
		diagStructPrintf("VIF: Export_IPMDrv_list%S{IPM_Drv_Export_struct_TYPE}",
				 ((void*)&retData), sizeof(IPM_Drv_Export_struct_TYPE));
	} while (0);

	if (fd > 0)
		close(fd);
	if (p != NULL)
		free(p);
}
void print_ok(void)
{
	DIAG_FILTER(Diag, Utils, battery_cali_ok, DIAG_INFORMATION)
	diagPrintf("OK");
}
void print_error(void)
{
	DIAG_FILTER(Diag, Utils, battery_cali_error, DIAG_INFORMATION)
	diagPrintf("ERROR");
}

#define BATTERY_CALI_DBG_PREFIX "battery calibration: "

#define BATTERY_CALI_LOG_TO_ACAT

#define DBG_PRINT(fmt, args...) dbg_print(BATTERY_CALI_DBG_PREFIX fmt, ##args)

static void dbg_print(const char *fmt, ...)
{
	char buf[DIAG_MAX_PRINTF_DATA_SIZE];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);

#ifdef BATTERY_CALI_LOG_TO_ACAT
	// output to ACAT
	DIAG_FILTER(Diag, Utils, battery_cali_debug, DIAG_INFORMATION)
	diagPrintf("%s", buf);
#endif

	// output to radio log
	DBGMSG("%s", buf);

	va_end(ap);
}

//ICAT EXPORTED FUNCTION - Diag,Utils,battery_calibration
void battery_calibration(void* CaliInputStruct)
{
	FILE *fd;
	char  value[20];
	unsigned char cmd;
	short data;

	signed short vbat_offset=0;
	unsigned short slope_low=1000, slope_high=1000;
	unsigned char d4=0,d5=0,d7=0;
	CaliReturnStruct TrimValue;

	int calibration_ibat = 0;
	int calibration_vbat = 0;
	char cmd_buf[128];

	int ret = 0;

	InputStruct* Input = (InputStruct*)CaliInputStruct;
	cmd = Input->cmd;
	data = Input->data;

	DBG_PRINT("acat call battery_calibration function ,cmd = %d, data = %d \n", cmd, data);
	switch (cmd)
	{
		case 0:
			DBG_PRINT("calibration 0 0 \n");
			system("cat "SANREMO_D4_CALI);
			print_ok();
			break;
		case 1:
			DBG_PRINT("calibration 1 xxx \n");
			fd=fopen(SANREMO_D4_CALI, "w");
			if(!fd ){
				DBG_PRINT("can't open d4 \n");
				exit(1);
			}
			DBG_PRINT("1 xxx, input value = %d \n",data);
			sprintf(value, "%d\n", data);
			fwrite(&value, strlen(value), 1, fd);
			print_ok();
			rewind(fd);
			fclose(fd);
			break;
		case 2:
			DBG_PRINT("calibration 2  0 \n");
			fd=fopen(SANREMO_D5D7_CALI,"r");
			if(!fd){
				DBG_PRINT("can't open d5d7 \n");
				exit(1);
			}
			fgets(value, sizeof(int), fd);
			//ibat_offset = atoi(value);
			DBG_PRINT("2 0, read trim value = %s\n", value);
			print_ok();
			rewind(fd);
			fclose(fd);
			break;
		case 3:
			DBG_PRINT("calibration 3  xxx \n");
			fd=fopen(SANREMO_D5D7_CALI, "w");
			if(!fd ){
				DBG_PRINT("can't open d5d7 \n");
				exit(1);
			}
			DBG_PRINT("3 xxx, input value = %d \n",data);
			sprintf(value, "%d\n", data);
			fwrite(&value, strlen(value), 1, fd);
			print_ok();
			rewind(fd);
			fclose(fd);
			break;


		case 4:
			DBG_PRINT("calibration 4  0 \n");
			fd=fopen(SANREMO_TRIM_RETURN,"r");
			if(!fd){
				DBG_PRINT("can't open trim return \n");
				exit(1);
			}
			fgets(value, sizeof(value), fd);
			sanremo_trim_value = atoi(value);
			print_ok();
			DBG_PRINT("4 0, read trim value = %d \n",sanremo_trim_value);
			rewind(fd);
			fclose(fd);
			break;


		case 5:
			DBG_PRINT("calibration 5  xxx \n");
			fd=fopen(SANREMO_TRIM_VOL,"w");
			if(!fd){
				DBG_PRINT("can't open trim vol \n");
				exit(1);
			}
			input_vol[0] = data;
			DBG_PRINT("5 xxx, input value = %d \n",data);
			sprintf(value, "%d\n", data);
			fwrite(&value, strlen(value), 1, fd);
			if(input_vol[0] >= 3195 && input_vol[0] <= 3205)
				print_ok();
			else
				print_error();
			rewind(fd);
			fclose(fd);
			break;
		case 6:
			DBG_PRINT("calibration 6  0 \n");
			fd=fopen(SANREMO_TRIM_VOL,"r");
			if(!fd){
				DBG_PRINT("can't open trim vol \n");
				exit(1);
			}
			fgets(value, sizeof(value), fd);
			voltage[0] = atoi(value);
			DBG_PRINT("input 3200, voltage = %d \n",voltage[0]);
			print_ok();
			rewind(fd);
			fclose(fd);
			break;

		case 7:
			DBG_PRINT("calibration 7  xxx \n");
			fd=fopen(SANREMO_TRIM_VOL,"w");
			if(!fd){
				DBG_PRINT("can't open trim vol \n");
				exit(1);
			}
			input_vol[1] = data;
			DBG_PRINT("7 xxx, input value = %d \n",data);
			sprintf(value, "%d\n", data);
			fwrite(&value, strlen(value), 1, fd);
			if(input_vol[1] >= 3695 && input_vol[1] <= 3705)
				print_ok();
			else
				print_error();
			rewind(fd);
			fclose(fd);
			break;
		case 8:
			DBG_PRINT("calibration 8  0 \n");
			fd=fopen(SANREMO_TRIM_VOL,"r");
			if(!fd){
				DBG_PRINT("can't open trim vol \n");
				exit(1);
			}
			fgets(value, sizeof(value), fd);
			voltage[1] = atoi(value);
			DBG_PRINT("input 3700, voltage = %d \n",voltage[1]);
			print_ok();
			rewind(fd);
			fclose(fd);
			break;

		case 9:
			DBG_PRINT("calibration 9  xxx \n");
			fd=fopen(SANREMO_TRIM_VOL,"w");
			if(!fd){
				DBG_PRINT("can't open trim vol \n");
				exit(1);
			}
			input_vol[2] = data;
			DBG_PRINT("9 xxx, input value = %d \n",data);
			sprintf(value, "%d\n", data);
			fwrite(&value, strlen(value), 1, fd);
			if(input_vol[2] >= 4195 && input_vol[2] <= 4205)
				print_ok();
			else
				print_error();
			rewind(fd);
			fclose(fd);
			break;
		case 10:
			DBG_PRINT("calibration 10  0 \n");
			fd=fopen(SANREMO_TRIM_VOL,"r");
			if(!fd){
				DBG_PRINT("can't open trim vol \n");
				exit(1);
			}
			fgets(value, sizeof(value), fd);
			voltage[2] = atoi(value);
			DBG_PRINT("input 4200, voltage = %d \n",voltage[2]);
			print_ok();
			rewind(fd);
			fclose(fd);
			break;
		case 11:
			DBG_PRINT("calibration 11  0 \n");
			fd=fopen(SANREMO_I_REMAIN,"r");
			if(!fd){
				DBG_PRINT("can't open trim return \n");
				exit(1);
			}
			fgets(value, sizeof(value), fd);
			ibat_offset = atoi(value);
			DBG_PRINT("ibat_offset = %d \n",ibat_offset);
			print_ok();
			rewind(fd);
			fclose(fd);
			break;
		case 12:
			DBG_PRINT("calibration 12  0 \n");
			vbat_offset = input_vol[1] - voltage[1];
			slope_low = 1000 *(input_vol[1] - input_vol[0])/(voltage[1] - voltage[0]);
			slope_high = 1000*(input_vol[2] - input_vol[1])/(voltage[2] -voltage[1]);

			d4  = sanremo_trim_value&0xff;		//sanremo register 0xD4 trim value
			d5  = (sanremo_trim_value>>8)&0xff;	//sanremo register 0xD5 trim value
			d7 = (sanremo_trim_value>>16)&0xff;	//sanremo register 0xD7 trim value
			DBG_PRINT("input_vol0 = %d,input_vol1 = %d,input_vol2 = %d \n",input_vol[0], input_vol[1], input_vol[2]);
			DBG_PRINT("voltage0 = %d,voltage1 = %d,voltage2 = %d \n",voltage[0], voltage[1], voltage[2]);
			DBG_PRINT("d4=%x,d5=%x,d7=%x \n", d4, d5, d7);
			DBG_PRINT("vbat_offset=%d,ibat_offset=%d \n", vbat_offset, ibat_offset);
			DBG_PRINT("slope_low=%d,slope_high=%d \n", slope_low, slope_high);

			TrimValue.d4 = d4;
			TrimValue.d5 = d5;
			TrimValue.d7 = d7;
			TrimValue.offset_ibat = ibat_offset;
			TrimValue.offset_vbat = vbat_offset;
			TrimValue.slope_low = slope_low;
			TrimValue.slope_high = slope_high;
			DIAG_FILTER(Diag, Utils, battery_calibration, DIAG_INFORMATION)
			diagStructPrintf("DIAG: calibration_return%S{CaliReturnStruct}",
						 ((void*)&TrimValue), sizeof(CaliReturnStruct));

			break;
		case 13:
			DBG_PRINT("calibration 13  0 \n");
			vbat_offset = input_vol[1] - voltage[1];
			slope_low = 1000 *(input_vol[1] - input_vol[0])/(voltage[1] - voltage[0]);
			slope_high = 1000*(input_vol[2] - input_vol[1])/(voltage[2] -voltage[1]);

			d4  = sanremo_trim_value&0xff;		//sanremo register 0xD4 trim value
			d5  = (sanremo_trim_value>>8)&0xff;	//sanremo register 0xD5 trim value
			d7 = (sanremo_trim_value>>16)&0xff;	//sanremo register 0xD7 trim value

			cmd_buf[0] = 0;
			sprintf(cmd_buf, "echo d4value %d > %s", d4, SANREMO_CALI);
			ret = system(cmd_buf);
			DBG_PRINT("cmd %s ret %d\n", cmd_buf, ret);
			cmd_buf[0] = 0;
			sprintf(cmd_buf, "echo d5value %d > %s", d5, SANREMO_CALI);
			ret = system(cmd_buf);
			DBG_PRINT("cmd %s ret %d\n", cmd_buf, ret);
			cmd_buf[0] = 0;
			sprintf(cmd_buf, "echo d7value %d > %s", d7, SANREMO_CALI);
			ret = system(cmd_buf);
			DBG_PRINT("cmd %s ret %d\n", cmd_buf, ret);
			cmd_buf[0] = 0;
			sprintf(cmd_buf, "echo ibat_offset %d > %s", ibat_offset, SANREMO_CALI);
			ret = system(cmd_buf);
			DBG_PRINT("cmd %s ret %d\n", cmd_buf, ret);
			cmd_buf[0] = 0;
			sprintf(cmd_buf, "echo vbat_offset %d > %s", vbat_offset, SANREMO_CALI);
			ret = system(cmd_buf);
			DBG_PRINT("cmd %s ret %d\n", cmd_buf, ret);
			cmd_buf[0] = 0;
			sprintf(cmd_buf, "echo vbat_slope_low %d > %s", slope_low, SANREMO_CALI);
			ret = system(cmd_buf);
			DBG_PRINT("cmd %s ret %d\n", cmd_buf, ret);
			cmd_buf[0] = 0;
			sprintf(cmd_buf, "echo vbat_slope_high %d > %s", slope_high, SANREMO_CALI);
			ret = system(cmd_buf);
			DBG_PRINT("cmd %s ret %d\n", cmd_buf, ret);
			print_ok();
			break;
		case 14:
			DBG_PRINT("calibration 14  0 \n");
			fd=fopen(SANREMO_CALI_VBAT,"r");
			if(!fd){
				DBG_PRINT("can't open calibration vbat \n");
				exit(1);
			}
			fgets(value, sizeof(value), fd);
			calibration_vbat = atoi(value);
			rewind(fd);
			fclose(fd);
			DIAG_FILTER(Diag, Utils, bettery_calibration_vbat, DIAG_INFORMATION)
			diagPrintf("DIAG: battery calibration_vbat: %d", calibration_vbat);
			break;
		case 15:
			DBG_PRINT("calibration 15  0 \n");
			fd=fopen(SANREMO_CALI_IBAT,"r");
			if(!fd){
				DBG_PRINT("can't open calibration ibat \n");
				exit(1);
			}
			fgets(value, sizeof(value), fd);
			calibration_ibat = atoi(value);
			rewind(fd);
			fclose(fd);
			DIAG_FILTER(Diag, Utils, battery_calibration_ibat, DIAG_INFORMATION)
			diagPrintf("DIAG: battery calibration_ibat: %d", calibration_ibat);
			break;
		}
}
