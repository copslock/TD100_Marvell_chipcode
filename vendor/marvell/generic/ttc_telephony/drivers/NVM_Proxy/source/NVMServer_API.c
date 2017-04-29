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
#include <pxa_dbg.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>
#include <sys/sendfile.h>

/* "private" includes should be done after STDIO, STDLIB...*/
#include <NVMServer_api.h>
#include "NVMDiag.h"
#include "osa.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NVM_OBJECT_MAGIC        0x32414818
#define NVM_DUMMY_TOTAL_SIZE    (8 * 1024 * 1024)
#define NVM_DUMMY_AVAIL_SIZE    (4 * 1024 * 1024)

#define NVM_FILE_NAME_MAX_LENGTH_LONG  128

struct NVMClientObject {
	UINT32 magic;
	char*   client_name;
};

/*******************************************************************************
* Minimum 2 tasks are using this API
*  - NVM-Server task serving the COMM-side requests
*  - APPS ACAT Diag
* All APIs look like reentrant - no any globals used but local-stack only.
* But in a fact the collision problem found between APPS & COMM services.
* To avoid this the PREEMPTION_DISABLE/ENABLE macros used.
* ANDROID power/sleep implementation may require disable/enable;
*                         place them into the same macros here.
*
* PREEMPTION_SEMA_INIT called by ffs_pwrSafe_Init() called from the NVM_Init.cpp
********************************************************************************
*/
static OSSemaRef  NVM_sema;
#define PREEMPTION_SEMA_INIT()   if(NVM_sema == NULL) {                      \
           ASSERT(OS_SUCCESS == OSASemaphoreCreate(&NVM_sema, 1, OSA_FIFO)); \
                            ASSERT(NVM_sema != NULL);                        \
                                                      } /*endM*/
#define PREEMPTION_DISABLE()   { DEBUGMSG("[NVMS] Get NVM_sema\n"); \
									OSASemaphoreAcquire(NVM_sema, OSA_SUSPEND); }
#define PREEMPTION_ENABLE()    { DEBUGMSG("[NVMS] Release NVM_sema\n"); \
									OSASemaphoreRelease(NVM_sema);}

#define nvmCheckIgnoreFile "/marvell/tel/nvm_uncheck_file.cfg"

static NVM_STATUS_T NVMS_FileFindClose_local( NVM_CLIENT_ID ClientID, LPHANDLE lpSearchHandle );

//Reliable operations
FILE*  fopen_pwrSafe(char* fName, char* szAttrib);
int    fclose_pwrSafe(FILE* fd);
static char* NVM_get_fName_by_fd(FILE* fd);
static int nvmIoctl_write(UINT32* pBuffer, short wItemSize, UINT32 dwCount, UINT32 fd);
static void nvmsHFileKeep(NVM_CLIENT_ID ClientID, UINT32 hFile);
static void nvmsHFileFree(NVM_CLIENT_ID ClientID, UINT32 hFile);
static void nvmsHFileCloseFreeAll(NVM_CLIENT_ID ClientID);
static int  NVMS_checkNVM_file(char* fileName, int removeReq, int useTmp);
static int NVM_copyFile(char * source,char*dest);

void NVMS_checkNVM_dir(char* nvm_root_dir);

#define VerifyClientObject(CLIENT_id)  {ClientID = ClientID;} //just to avoid WARNING
/*static BOOL VerifyClientObject( NVM_CLIENT_ID ClientID)
{
	struct NVMClientObject *pClient;

	pClient = (struct NVMClientObject*)ClientID;
	if (pClient == NULL || pClient->magic != NVM_OBJECT_MAGIC || pClient->client_name == NULL)
		return FALSE;

	DEBUGMSG("[NVMS] Client [%s]\n", pClient->client_name);
	return TRUE;
}*/

NVM_STATUS_T NVMS_Register( LPCTSTR szClientName, NVM_CLIENT_ID *pClientID)
{
	if(strcmp((char*)szClientName, NVM_CLIENT_COMM_NAME) == 0)
	{
		*pClientID = (NVM_CLIENT_ID)NVM_CLIENT_COMM;
	}
	else if(strcmp((char*)szClientName, NVM_ACAT_CLIENT_NAME) == 0)
	{
		*pClientID = (NVM_CLIENT_ID)NVM_CLIENT_ACAT;
	}
	else
	{
		struct NVMClientObject *pClient;

		pClient = (struct NVMClientObject*)malloc(sizeof(struct NVMClientObject));
		ASSERT(pClient);
		pClient->magic = NVM_OBJECT_MAGIC;
		pClient->client_name = malloc(strlen(szClientName) + 4);
		ASSERT(pClient->client_name);
		strcpy(pClient->client_name, szClientName);

		*pClientID = (NVM_CLIENT_ID)pClient;
	}
	return NVM_STATUS_SUCCESS;
}

NVM_STATUS_T NVMS_FileDelete( NVM_CLIENT_ID ClientID, LPCTSTR szFileName)
{
	int ret;

	VerifyClientObject(ClientID);

	PREEMPTION_DISABLE();
	ret = unlink(szFileName);
	PREEMPTION_ENABLE();
	DEBUGMSG("[NVMS] %s(%s) returns %d\n", __func__, szFileName, ret);
	if (ret == 0)
		return NVM_STATUS_SUCCESS;
	else
		return NVM_STATUS_FAILURE;
}

struct FileFindObject {
	DIR*    dirstream;
	char*   pattern;
};

NVM_STATUS_T NVMS_FileFindFirst(NVM_CLIENT_ID ClientID,
				LPCTSTR szFileName,
				NVM_FILE_INFO* pFindResults,
				LPHANDLE lpSearchHandle )
{
	DIR* dirstream;
	struct FileFindObject* ffobj;
	struct dirent* ent;
	struct stat fs;
	int ret;
	int file_date, file_time;
	struct tm gm_time;
	//char wildcard[] = "*";
	char *pdir;
	char *pbase;
	char dir[NVM_FILE_NAME_MAX_LENGTH_LONG];
	char base[NVM_FILE_NAME_MAX_LENGTH_LONG];
	char path[NVM_FILE_NAME_MAX_LENGTH_LONG];
	int  nameOffs;
	BOOL listAllFiles;
	DEBUGMSG("[NVMS] Enter Find First\n");
	VerifyClientObject(ClientID);

	PREEMPTION_DISABLE();

	DBGMSG("[NVMS] Find First [%s]\n", szFileName);

	strncpy(dir, szFileName, NVM_FILE_NAME_MAX_LENGTH_LONG);
	strncpy(base, szFileName, NVM_FILE_NAME_MAX_LENGTH_LONG);
	pdir = dirname(dir);
	pbase = basename(base);

	strncpy(path, pdir, NVM_FILE_NAME_MAX_LENGTH_LONG);
	strncat(path, "/", 1);
	nameOffs = strlen(path);
	listAllFiles = 1;

	if(listAllFiles) {
		//Close OLD (from from prefious FindFirst & FindNext) and create new
		if(*lpSearchHandle != (HANDLE)NULL)
			NVMS_FileFindClose_local(ClientID, lpSearchHandle);
	}
	else
	{
		if(ClientID == NVM_CLIENT_COMM)
		{
			if(NVMS_checkNVM_file(szFileName, TRUE, 0) != 0)
			{
				PREEMPTION_ENABLE();
				DEBUGMSG("[NVMS] Find First [%s] - not found (on Check)\n",szFileName);
				return NVM_STATUS_FAILURE;
			}
		}
	}

	dirstream = opendir(pdir);
	if (dirstream == NULL)
	{
		PREEMPTION_ENABLE();
		DEBUGMSG("[NVMS] failed to open dir.\n");
		return NVM_STATUS_FAILURE;
	}

	do {
		ent = readdir(dirstream);
		if (NULL == ent) break;
		DEBUGMSG("[NVMS] readdir() return [%s]\n", ent->d_name);
		if (strcmp(ent->d_name, "..") == 0 || strcmp(ent->d_name, ".") == 0)
			continue;

		/* Found */
		if (fnmatch(pbase, ent->d_name, 0) == 0)
		{
			memset(pFindResults,0,sizeof(NVM_FILE_INFO));
			strcpy((char*)pFindResults->file_name, ent->d_name);
			path[nameOffs] = '\0';
			strncat(path+nameOffs, ent->d_name, NVM_FILE_NAME_MAX_LENGTH_LONG-nameOffs);
			ret= stat(path,&fs);
			if(!ret)
				break;
		}
	} while (ent != NULL);

	if (ent)
	{
		gmtime_r((time_t const *)&(fs.st_mtime), &gm_time);
		file_date = gm_time.tm_mday << (12 + 4);
		file_date += (gm_time.tm_mon + 1) << 12;
		file_date += gm_time.tm_year + 1900;
		file_time = gm_time.tm_hour << (6 + 6);
		file_time += gm_time.tm_min << 6;
		file_time += gm_time.tm_sec;
		pFindResults->size = fs.st_size;
		pFindResults->date = file_date;
		pFindResults->time = file_time;
		if(!listAllFiles)
		{
			closedir(dirstream);
		}
		else
		{

			ffobj = malloc(sizeof(struct FileFindObject));
			DEBUGMSG("[NVMS] FileFindFirst ffobj=0x%08x\n", ffobj);
			if (ffobj == NULL)
			{
				DEBUGMSG("[NVMS] failed to malloc ffobj for FindNext.\n");
				closedir(dirstream);
				*lpSearchHandle = 0;
				PREEMPTION_ENABLE();
				return NVM_STATUS_SUCCESS;
			}
			ffobj->pattern = malloc(strlen(szFileName) + 4);
			if (ffobj->pattern == NULL)
			{
				DEBUGMSG("[NVMS] failed to malloc ffobj->pattern for FindNext.\n");
				free(ffobj);
				closedir(dirstream);
				*lpSearchHandle = 0;
				PREEMPTION_ENABLE();
				return NVM_STATUS_SUCCESS;
			}
			ffobj->dirstream = dirstream;
			strcpy(ffobj->pattern, szFileName);

			*lpSearchHandle = (int)ffobj;
			pFindResults->dir_id = (INT32)ffobj;
		}
		PREEMPTION_ENABLE();
		if(ClientID != NVM_CLIENT_ACAT)
			DEBUGMSG("[NVMS] Find First [%s]\n",szFileName);
		else
			usleep(10000); //Prevents CPU starvation caused by ACAT APPS
		return NVM_STATUS_SUCCESS;
	}
	else
	{
		DEBUGMSG("[NVMS] failed to find a matching file.\n");
		closedir(dirstream);
		PREEMPTION_ENABLE();
		return NVM_STATUS_FAILURE;
	}
}

static NVM_STATUS_T NVMS_FileFindClose_local( NVM_CLIENT_ID ClientID, LPHANDLE lpSearchHandle )
{
	int error = 0;
	struct FileFindObject* ffobj;
	VerifyClientObject(ClientID);
	ffobj = (struct FileFindObject*)(*lpSearchHandle);

	if(ffobj != NULL)
	{
		if(ffobj->dirstream == NULL)	error |= 1;
		else
			closedir(ffobj->dirstream);
		if(ffobj->pattern == NULL)		error |= 2;
		else
			free(ffobj->pattern);
		free(ffobj);
	}
	*lpSearchHandle = (HANDLE)NULL;
	if(error != 0) {
		ERRMSG("[NVMS] ERR: Invalid FileFindClose, error=%x\n", error);
		return NVM_STATUS_FAILURE;
	}
	return NVM_STATUS_SUCCESS;
}

NVM_STATUS_T NVMS_FileFindClose( NVM_CLIENT_ID ClientID, LPHANDLE lpSearchHandle )
{
	//PREEMPTION_DISABLE();
	NVM_STATUS_T rc = NVMS_FileFindClose_local(ClientID, lpSearchHandle);
	//PREEMPTION_ENABLE();
	return rc;
}

//////////////////////////////////////////////////////////////////////////
NVM_STATUS_T NVMS_FileFindNext( NVM_CLIENT_ID ClientID,
				NVM_FILE_INFO* pFindResults,
				LPHANDLE lpSearchHandle )
{
	struct FileFindObject* ffobj;
	struct dirent* ent;
	struct stat fs;
	int ret;
	int file_date, file_time;
	struct tm gm_time;
	char *pdir;
	char *pbase;
	char dir[NVM_FILE_NAME_MAX_LENGTH_LONG];
	char base[NVM_FILE_NAME_MAX_LENGTH_LONG];
	char path[NVM_FILE_NAME_MAX_LENGTH_LONG];
	int  nameOffs;

	VerifyClientObject(ClientID);

	PREEMPTION_DISABLE();

	if(pFindResults->dir_id)
	{
		ffobj = (struct FileFindObject*)(pFindResults->dir_id);
		DEBUGMSG("[NVMS] FileFindNext from request ffobj=0x%08x\n", ffobj);
	}
	else
	{
		ffobj = (struct FileFindObject*)(*lpSearchHandle);
		DEBUGMSG("[NVMS] FileFindNext (WARNING) ffobj=0x%08x\n", ffobj);
	}
	if (ffobj == NULL ||  ffobj->pattern == NULL)
	{
		PREEMPTION_ENABLE();
		ERRMSG("[NVMS] Invalid ffobj\n");
		return NVM_STATUS_FAILURE;
	}

	strncpy(dir, ffobj->pattern, NVM_FILE_NAME_MAX_LENGTH_LONG);
	strncpy(base, ffobj->pattern, NVM_FILE_NAME_MAX_LENGTH_LONG);
	pdir = dirname(dir);
	pbase = basename(base);
	strncpy(path, pdir, NVM_FILE_NAME_MAX_LENGTH_LONG);
	strncat(path, "/", NVM_FILE_NAME_MAX_LENGTH_LONG);
	nameOffs = strlen(path);

	DEBUGMSG("[NVMS] Find Next [%s]\n", ffobj->pattern);
	do {
		ent = readdir(ffobj->dirstream);
		if (NULL == ent) break;
		DEBUGMSG("[NVMS] readdir() return [%s]\n", ent->d_name);
		if (strcmp(ent->d_name, "..") == 0 || strcmp(ent->d_name, ".") == 0)
			continue;

		/* Found */
		if (fnmatch(pbase, ent->d_name, 0) == 0)
		{
			memset(pFindResults,0,sizeof(NVM_FILE_INFO));
			strcpy((char*)pFindResults->file_name, ent->d_name);
			path[nameOffs] = '\0';
			strncat(path+nameOffs, ent->d_name, NVM_FILE_NAME_MAX_LENGTH_LONG-nameOffs);
			ret= stat(path,&fs);
			if(!ret)
				break; /* OK, entry is valid; otherwise continue as non-statable entries might exist, e.g. softlink with bad target */
		}
	} while (ent != NULL);

	if (ent)
	{
		gmtime_r((time_t const *)&(fs.st_mtime), &gm_time);
		file_date = gm_time.tm_mday << (12 + 4);
		file_date += (gm_time.tm_mon + 1) << 12;
		file_date += gm_time.tm_year + 1900;
		file_time = gm_time.tm_hour << (6 + 6);
		file_time += gm_time.tm_min << 6;
		file_time += gm_time.tm_sec;
		pFindResults->size = fs.st_size;
		pFindResults->date = file_date;
		pFindResults->time = file_time;
		pFindResults->dir_id = (INT32)ffobj;

		PREEMPTION_ENABLE();
		DEBUGMSG("[NVMS] Find Next [%s]\n",ffobj->pattern);
		return NVM_STATUS_SUCCESS;
	}
	else
	{
		NVMS_FileFindClose_local(ClientID, lpSearchHandle);
		PREEMPTION_ENABLE();
		ERRMSG("[NVMS] failed to find a matching file.\n");
		return NVM_STATUS_FAILURE;
	}
}

NVM_STATUS_T NVMS_GetFileSize(  NVM_CLIENT_ID ClientID,
				LPCTSTR szFileName,
				PDWORD pdwSizeHigh,
				PDWORD pdwSizeLow)
{
	struct stat fs;
	int ret;

	VerifyClientObject(ClientID);
	ret = stat(szFileName, &fs);
	DPRINTF("[NVMS] %s(%s) returns %d\n", __func__, szFileName, ret);
	if (ret != 0)
	{
		ERRMSG("[NVMS] failed to open the file [%s].\n", szFileName);
		return NVM_STATUS_FAILURE;
	}
	*pdwSizeLow = fs.st_size;
	*pdwSizeHigh = 0;
	DEBUGMSG("[NVMS] size = %d\n", fs.st_size);
	return NVM_STATUS_SUCCESS;
}


NVM_STATUS_T NVMS_FileOpen( NVM_CLIENT_ID ClientID,
			    LPCTSTR szFileName,
			    LPCTSTR szAttrib,
			    PUINT32 phFile)
{
	FILE* fp;

	VerifyClientObject(ClientID);

	PREEMPTION_DISABLE();
	if(ClientID == NVM_CLIENT_COMM)
		fp = fopen_pwrSafe(szFileName, szAttrib);
	else
		fp = fopen(szFileName,szAttrib);
	DPRINTF("[NVMS] %s(%s with %s) returns %x\n", __func__, szFileName, szAttrib, fp);

	if (NULL == fp)
	{
		PREEMPTION_ENABLE();
		ERRMSG("Cannot open file [%s] with attrib [%s]\n", szFileName, szAttrib);
		return NVM_STATUS_FAILURE;
	}
	*phFile = (UINT32)fp;
	if(fp)
		nvmsHFileKeep(ClientID, (UINT32)fp);
	PREEMPTION_ENABLE();
	return NVM_STATUS_SUCCESS;
}

NVM_STATUS_T NVMS_FileWrite( NVM_CLIENT_ID ClientID,
			     UINT32 hFile, LPVOID pBuffer, UINT32 dwBufferLen,
			     short wItemSize, UINT32 dwCount, PUINT32 pdwActual)
{
	UNUSEDPARAM(dwBufferLen)

	VerifyClientObject(ClientID);
	*pdwActual = 0;
	if (hFile == 0)
	{
		ERRMSG("[NVMS] %s: Invalid file handler [%x].\n", __func__, hFile);
		return NVM_STATUS_FAILURE;
	}
	if((wItemSize == 0) || (dwCount == 0))
		return NVM_STATUS_FAILURE;

	DEBUGMSG("[NVMS] %s(%x, %d * %d)\n", __func__, hFile, wItemSize, dwCount);
	*pdwActual = nvmIoctl_write(pBuffer, wItemSize, dwCount, hFile);
	if(*pdwActual != 0)
		return NVM_STATUS_SUCCESS;


	PREEMPTION_DISABLE();
	*pdwActual = fwrite(pBuffer,wItemSize, dwCount, (FILE*)hFile);
           if(ferror((FILE*)hFile))
            {
                 ERRMSG("[NVMS] %s: File  fead error%d, %s.[%x].\n", __func__, *pdwActual, strerror(errno),hFile);
                 clearerr((FILE*)hFile);
            }
	if((*pdwActual == 0) && (wItemSize == 1))
	{
		usleep(50000);usleep(50000);usleep(50000);  //Suppose it fixed by garbage collection
		*pdwActual = fwrite(pBuffer,wItemSize, dwCount, (FILE*)hFile);
                if(ferror((FILE*)hFile))
                {
                     ERRMSG("[NVMS] %s: File  fead error%d, %s.[%x].\n", __func__, *pdwActual, strerror(errno),hFile);
                    clearerr((FILE*)hFile);
                 }
	}
	if(*pdwActual == 0)
	{
		NVM_get_fName_by_fd((FILE*)hFile); //remove hFile from the TMP list
	}
	else
	{
        /* According to fseek and fopen man pages rb+ requires sync after fwrite;
           Even though fseek is supposed to sync as well, still observed cases
           where fopen,fread,fseek(0,0),fwrite(size), fseek(size,0) failed in fseek */
        fflush((FILE*)hFile);
	}
	DPRINTF("[NVMS] %s(%x, %d * %d) returns %d\n", __func__, hFile, wItemSize, dwCount, *pdwActual);
	PREEMPTION_ENABLE();

	return NVM_STATUS_SUCCESS;
}

NVM_STATUS_T NVMS_FileFlush( NVM_CLIENT_ID ClientID, UINT32 hFile)
{
	VerifyClientObject(ClientID);
	if (hFile == 0)
	{
		ERRMSG("[NVMS] %s: Invalid file handler [%x].\n", __func__, hFile);
		return NVM_STATUS_FAILURE;
	}

	DEBUGMSG("[NVMS] %s(%x)\n", __func__, hFile);
	PREEMPTION_DISABLE();
	int ret = fflush((FILE*)hFile);
	PREEMPTION_ENABLE();
	DPRINTF("[NVMS] %s(%x) returns %d\n", __func__, hFile, ret);
	return NVM_STATUS_SUCCESS;
}

NVM_STATUS_T NVMS_FileSeek( NVM_CLIENT_ID ClientID, UINT32 hFile, LONG dwOffset, INT32 dwOrigin)
{
	VerifyClientObject(ClientID);
	if (hFile == 0)
	{
		ERRMSG("[NVMS] %s: Invalid file handler [%x].\n", __func__, hFile);
		return NVM_STATUS_FAILURE;
	}

	DEBUGMSG("[NVMS] %s(%x at %d from %d)\n", __func__, hFile, dwOffset, dwOrigin);
	PREEMPTION_DISABLE();
	int ret = fseek((FILE*)hFile, dwOffset, dwOrigin);
	PREEMPTION_ENABLE();
	DPRINTF("[NVMS] %s(%x at %d from %d) returns %d\n", __func__, hFile, dwOffset, dwOrigin, ret);
	if (ret == 0)
		return NVM_STATUS_SUCCESS;
	else
		return NVM_STATUS_FAILURE;
}

NVM_STATUS_T NVMS_FileRead( NVM_CLIENT_ID ClientID, UINT32 hFile,
			    short wItemSize, UINT32 dwCount, PUINT32 pdwActual, LPVOID pBuffer)
{
	VerifyClientObject(ClientID);
	if (hFile == 0)
	{
		ERRMSG("[NVMS] %s: Invalid file handler [%x].\n", __func__, hFile);
		return NVM_STATUS_FAILURE;
	}

	DEBUGMSG("[NVMS] %s(%x %d * %d)\n", __func__, hFile, wItemSize, dwCount);
	PREEMPTION_DISABLE();
        if(feof((FILE*)hFile))
            ERRMSG("[NVMS] %s: File pointer is at the end of the file!.[%x].\n", __func__, hFile);

        *pdwActual = fread(pBuffer, wItemSize, dwCount, (FILE*)hFile);

        if(ferror((FILE*)hFile))
        {
            ERRMSG("[NVMS] %s: File  fead error%d, %s.[%x].\n", __func__, *pdwActual, strerror(errno),hFile);
            clearerr((FILE*)hFile);
        }

         PREEMPTION_ENABLE();
	DPRINTF("[NVMS] %s(%x %d * %d) returns %d\n", __func__, hFile, wItemSize, dwCount, *pdwActual);

	return NVM_STATUS_SUCCESS;
}

NVM_STATUS_T NVMS_FileClose( NVM_CLIENT_ID ClientID, UINT32 hFile)
{
	int  ret;
	FILE* fp;

	VerifyClientObject(ClientID);

	if (hFile == 0)
	{
		ERRMSG("[NVMS] %s: Invalid file handler [%x].\n", __func__, hFile);
		return NVM_STATUS_FAILURE;
	}

	PREEMPTION_DISABLE();
	fp = (FILE*)hFile;

	if(ClientID == NVM_CLIENT_COMM)
		 ret = fclose_pwrSafe(fp);
	else
		ret = fclose(fp);
	if(ret == 0)
		nvmsHFileFree(ClientID, (UINT32)fp);
	PREEMPTION_ENABLE();
	if(ClientID != NVM_CLIENT_ACAT)
	{
		if(ret == 0)
			ret = NVM_STATUS_SUCCESS;
		else
			ret = NVM_STATUS_FS_ERROR;
	}
	else
	{
		usleep(10000); //Prevents CPU starvation caused by ACAT APPS
		ret = NVM_STATUS_SUCCESS;
	}
	DPRINTF("[NVMS] %s(%x) returns %d\n", __func__, hFile, ret);
	return (NVM_STATUS_T)ret;
}

NVM_STATUS_T NVMS_FileCloseAll( NVM_CLIENT_ID ClientID)
{
	nvmsHFileCloseFreeAll(ClientID);
	return NVM_STATUS_SUCCESS;
}

NVM_STATUS_T NVMS_DeRegister( NVM_CLIENT_ID ClientID)
{
	struct NVMClientObject *pClient;

	VerifyClientObject(ClientID);

	PREEMPTION_DISABLE();
	pClient = (struct NVMClientObject*)ClientID;
	if (pClient == NULL || pClient->client_name == NULL)
	{
		ERRMSG("[NVMS] Invalid ClientID\n");
		return NVM_STATUS_FAILURE;
	}
	free(pClient->client_name);
	memset(pClient, 0, sizeof(struct NVMClientObject));
	free(pClient);
	PREEMPTION_ENABLE();
	return NVM_STATUS_SUCCESS;
}

NVM_STATUS_T NVMS_FileRename(NVM_CLIENT_ID ClientID,
			     LPCTSTR szOldFileName,
			     LPCTSTR szNewFileName)
{
	int ret;

	VerifyClientObject(ClientID);
	PREEMPTION_DISABLE();
	ret = rename(szOldFileName, szNewFileName);
	PREEMPTION_ENABLE();
	DEBUGMSG("[NVMS] %s(%s --> %s) returns %d\n", __func__, szOldFileName, szNewFileName, ret);
	if (ret == 0)
		return NVM_STATUS_SUCCESS;
	else
		return NVM_STATUS_FAILURE;
}

NVM_STATUS_T NVMS_GetFileStat(NVM_CLIENT_ID ClientID,
			      LPCTSTR szFileName,
			      PDWORD pdwStat)
{
	struct stat fs;
	int ret;

	VerifyClientObject(ClientID);
	PREEMPTION_DISABLE();
	ret = stat(szFileName, &fs);
	PREEMPTION_ENABLE();
	DBGMSG("[NVMS] %s(%s) returns %d\n", __func__, szFileName, ret);
	if (ret == 0)
	{
		*pdwStat = S_ISDIR(fs.st_mode)
			   ? NVM_DIR_MASK | NVM_FILE_MASK
			   : NVM_FILE_MASK;
		return NVM_STATUS_SUCCESS;
	}
	else
		return NVM_STATUS_FAILURE;
}

NVM_STATUS_T NVMS_FileChangeMode(NVM_CLIENT_ID ClientID,
				 LPCTSTR szFileName,
				 DWORD dwNewMode)
{
	mode_t fm = (mode_t)dwNewMode;
	int ret;

	VerifyClientObject(ClientID);

	fm = /*dwNewMode*/ 0x777;
	PREEMPTION_DISABLE();
	ret = chmod(szFileName, fm);
	PREEMPTION_ENABLE();
	DEBUGMSG("[NVMS] %s(%s, %d) returns %d\n", __func__, szFileName, dwNewMode, ret);
	if (ret == 0)
		return NVM_STATUS_SUCCESS;
	else
		return NVM_STATUS_FAILURE;
}

NVM_STATUS_T NVMS_GetAvailSpace( NVM_CLIENT_ID ClientID, LPCTSTR szVol, PUINT32 pdwSize)
{
	UNUSEDPARAM(szVol)

	VerifyClientObject(ClientID);

	/* dummy value */
	(*pdwSize) = NVM_DUMMY_AVAIL_SIZE;
	DEBUGMSG("[NVMS] %s() returns %d\n", __func__, *pdwSize);

	return NVM_STATUS_SUCCESS;
}

NVM_STATUS_T NVMS_GetTotalSpace( NVM_CLIENT_ID ClientID, PUINT32 pdwSize)
{
	VerifyClientObject(ClientID);

	/* dummy value */
	(*pdwSize) = NVM_DUMMY_TOTAL_SIZE;
	DEBUGMSG("[NVMS] %s() returns %d\n", __func__, *pdwSize);

	return NVM_STATUS_SUCCESS;
}

NVM_STATUS_T NVMS_MkDir(NVM_CLIENT_ID ClientID, LPCTSTR szDirName, DWORD dwMode)
{
	int ret;
	int mode = (int)dwMode;

	VerifyClientObject(ClientID);
	mode = 0x777;
	PREEMPTION_DISABLE();
	ret = mkdir(szDirName, mode);
	PREEMPTION_ENABLE();

	DEBUGMSG("[NVMS] %s(%s, %d) returns %d\n", __func__, szDirName, dwMode, ret);
	if (ret == 0)
		return NVM_STATUS_SUCCESS;
	else
		return NVM_STATUS_FAILURE;
}

NVM_STATUS_T NVMS_RmDir(NVM_CLIENT_ID ClientID, LPCTSTR szDirName)
{
	int ret;

	VerifyClientObject(ClientID);
	PREEMPTION_DISABLE();
	ret = rmdir(szDirName);
	PREEMPTION_ENABLE();
	DEBUGMSG("[NVMS] %s(%s) returns %d\n", __func__, szDirName, ret);
	if (ret == 0)
		return NVM_STATUS_SUCCESS;
	else
		return NVM_STATUS_FAILURE;
}



/******************************************************************************
*******************************************************************************/
// #include "nvm_shared.h"
#define NVM_IOCTL_FILENAME     "nvm_ioctl"

typedef enum
{
	NVM_MODE_DEFAULT = 0,
	NVM_MODE_RESTART ,
	NVM_MODE_NO_PARTIAL_SYNC,
	NVM_MODE_FORCE_SYNC  ,
	NVM_MODE_NORMAL_SYNC ,

	NVM_MODE_SPARE1 ,
	NVM_MODE_SPARE2 ,
	NVM_MODE_SPARE3 ,

	NVM_MODE_GODOWN  ,
	NVM_GET_MODE = 0x1FFFFFFF
} NVM_SERVER_MODE;

NVM_SERVER_MODE  nvmIoctlMode;
UINT32           nvmIoctlHandler;
UINT32           nvmIoctlWriteOpen;

static FILE* nvmIoctl_open(char* fName, char* szAttrib)
{
	char* pIoctlName = strstr(fName, NVM_IOCTL_FILENAME);
	if(pIoctlName == NULL)
		return NULL;
	if(strcmp(pIoctlName, NVM_IOCTL_FILENAME) != 0)
		return NULL;

	nvmIoctlWriteOpen = (szAttrib[0]=='w');

	//if((nvmIoctlHandler != NULL) && (nvmIoctlMode != NVM_MODE_DEFAULT))
	//	TBD: Handlingif for "COMM restarted. Close all opened file handlers"

	nvmIoctlHandler = (UINT32)&nvmIoctlMode;
	return((FILE*)nvmIoctlHandler);
}

static int nvmIoctl_write(UINT32* pBuffer, short wItemSize, UINT32 dwCount, UINT32 fd)
{
	if(fd == nvmIoctlHandler)
	{
		NVM_SERVER_MODE  mode = *(NVM_SERVER_MODE*)pBuffer;
		(void)wItemSize; //unused
		(void)dwCount;   //unused
		if((mode == NVM_MODE_DEFAULT) && (nvmIoctlMode == NVM_MODE_NO_PARTIAL_SYNC))
			sync();
		else
		if(mode == NVM_MODE_FORCE_SYNC)
			sync();

		DEBUGMSG("[NVMS] IOCTL mode %x -> %x\n", nvmIoctlMode, mode);
		nvmIoctlMode = mode;
		return (1);
	}
	return (0);
}

static int nvmIoctl_close(FILE* fd)
{
	if((UINT32)fd == nvmIoctlHandler)
	{
		nvmIoctlHandler = 0;
		if(nvmIoctlWriteOpen)
			nvmIoctlMode = NVM_MODE_DEFAULT;
		return (0);
	}
	return (1);
}


/******************************************************************************
*******************************************************************************
*        fopen_pwrSafe
*        fclose_pwrSafe
*
*   When the Power occasionally goes down in a middle of file writing
*   or while the file is still not sync to flash
*   or assert occured  in a middle of file writing
*   the file is "partialy" updated on the flash.
*   In a a difference with file downloading or logging
*    (where it is better to save even part of file)
*   for the modem this file is "corrupted" and may  bring to the problems.
*   To avoid this apply writing into temporary-copy of the original file and
*   switch files only upon "fclose" operation.
*
*   There are some threads under the same process which are using these procedures.
*   Since we have the internal fDesc table we need the protection.
*   The protection is done by OSASemaphore
*******************************************************************************
******************************************************************************/

#define  TMP_FNAME_POSTFIX        "_tmpf"
#define  TMP_FNAME_POSTFIX_SIZE   5
#define  FOPEN_MAX_WRITE         16 /* Less than FOPEN_MAX _SYS_OPEN*/
#define  TMP_NVM_FILE_NAME_MAX_LENGTH  (NVM_FILE_NAME_MAX_LENGTH_LONG + TMP_FNAME_POSTFIX_SIZE)

typedef struct
{
	FILE* fd;
	char* fName;
}NVM_fDesc_s;

// The rename() function behavior for case "rename to EXISTING file"
//     may be different for different OS or even versions
// Let's test this behaviour once and then work accordingly.
typedef enum
{
	RENAME_UNKNOWN  =0,             //Unknown, non-initialized
	RENAME_DOS_LIKE =1,            // delete-orig, rename
	RENAME_FORCE_MOVE_LIKE =2,     //              rename
	RENAME_FORCE_COPY_LIKE =3      //              rename, delete-tmpf
}NVM_rename_behavior_e;

static NVM_fDesc_s  NVM_fDesc[FOPEN_MAX_WRITE];
static NVM_rename_behavior_e     NVM_rename_behavior;
static char*                     NVM_root_dir;
static char                      NVM_root_dir_buf[80];

//=================================================
static BOOL NMVS_TmpUseRequired(char* fName, char* szAttrib)
{
	FILE *uncheck_fd = NULL;
	char uncheck_file[256];

	ASSERT(strlen(fName)<NVM_FILE_NAME_MAX_LENGTH_LONG);
	/*************
	*     Required only if WRITE:
	* "r"         open text file for reading
	* "w"         create text file for writing, or truncate to zero length
	* "a"         append; open text file or create for writing at eof
	* "rb"        open binary file for reading
	* "wb"        create binary file for writing, or truncate to zero length
	* "ab"        append; open binary file or create for writing at eof
	* "r+"        open text file for update (reading and writing)
	* "w+"        create text file for update, or truncate to zero length
	* "a+"        append; open text file or create for update, writing at eof
	* "r+b"/"rb+" open binary file for update (reading and writing)
	* "w+b"/"wb+" create binary file for update, or truncate to zero length
	* "a+b"/"ab+" append; open binary file or create for update, writing at eof
	*************/


		uncheck_fd = fopen(nvmCheckIgnoreFile, "r");
		if(uncheck_fd == NULL)
		ERRMSG("[NVMS] Can not find %s, all the files must be checked!\n",nvmCheckIgnoreFile);
		else
		{
			while(fgets(uncheck_file, 256,uncheck_fd))
			{
				int length = strlen(uncheck_file);
				uncheck_file[length-1] = '\0';  //remove '\n' at the end.
				//ERRMSG("[NVMS] file need not to check :%s, file:%s\n",uncheck_file, fileName);
				if(strstr(fName,uncheck_file) != NULL)
				{
					ERRMSG("[NVMS] file %s is in the ignore list!\n",fName);
					fclose(uncheck_fd);
					return 0;
				}
			}
			ERRMSG("[NVMS] file %s is not in the ignore list!\n",fName);
			fclose(uncheck_fd);
		}
	return(  (szAttrib[0]=='w') || (szAttrib[0]=='a') || (szAttrib[1]=='+') || (szAttrib[2]=='+') );
}

//=================================================
static void NVM_save_fName_fd(char* fName, FILE* fd)
{
	// Keep fName in DB for NAME-switch upon fclose()
	UINT32 i;
	char*  pName;

	i = strlen(fName)+1;
	pName = (char*)malloc(i); /* freed in fclose_pwrSafe*/
	ASSERT(pName != NULL);
	memcpy(pName, fName, i);

	for(i=0; i<FOPEN_MAX_WRITE ; i++)
	{
		if(NVM_fDesc[i].fd == NULL)
		{
			NVM_fDesc[i].fd    = fd;
			NVM_fDesc[i].fName = pName;
			break;
		}
	}
	ASSERT(i < FOPEN_MAX_WRITE);
}

//=================================================
static char* NVM_get_fName_by_fd(FILE* fd)
{
	//Looks for descriptor, deletes from DB, but keeps the name
	UINT32 i;
	char* fName = NULL;

	for(i=0; i<FOPEN_MAX_WRITE ; i++)
	{
		if(NVM_fDesc[i].fd == fd)
		{
			fName = NVM_fDesc[i].fName;
			NVM_fDesc[i].fd    = NULL;
			NVM_fDesc[i].fName = NULL;
			break;
		}
	}
	return(fName);
}

//=================================================
static void NVM_rename_test_behavior(void)
{
	//Create Fsrc and Fdst files
	//Rename
	//Check Fsrc vs Fdst and presence
	//Set the "NVM_rename_behavior" accordingly
	//Remove Fsrc and Fdst
	FILE*  Fsrc;
	FILE*  Fdst;
	char*  nameFsrc = "Fsrc";
	char*  nameFdst = "Fdst";
	int    iFsrc = 0x12345678;
	int    iFdst = 0;
	int    ret;

	Fsrc = fopen(nameFsrc, "wb");    ASSERT(Fsrc!=NULL);
	Fdst = fopen(nameFdst, "wb");    ASSERT(Fdst!=NULL);
	fwrite(&iFsrc, 1, 4, Fsrc);
	fwrite(&iFdst, 1, 4, Fdst);
	fclose(Fsrc);
	fclose(Fdst);

	ret=rename(nameFsrc, nameFdst);

	if(ret != 0)
	NVM_rename_behavior = RENAME_DOS_LIKE;  // delete-orig required
	else
	{
	//Check the rename was really OK
		Fdst = fopen(nameFdst, "rb");    ASSERT(Fdst!=NULL);
		fread(&iFdst, 1, 4, Fdst);
		fclose(Fdst);
		if(iFdst != iFsrc)
		NVM_rename_behavior = RENAME_DOS_LIKE;
		else
		{ //Check the Original is not present
			Fsrc = fopen(nameFsrc, "rb");
			if(Fsrc != NULL)
			{
				fclose(Fsrc);
				NVM_rename_behavior = RENAME_FORCE_COPY_LIKE;
			}
			else
				NVM_rename_behavior = RENAME_FORCE_MOVE_LIKE;
		}
	}
	remove(nameFsrc);
	remove(nameFdst);
	//printf("\n\n =========> NVM_rename_behavior = %d\n\n\n", NVM_rename_behavior);
}

//=================================================
static int NVM_dir_fsync(char* dirName)
{
	int ret = -1;
	if(dirName != NULL)
	{
		int fid;
		FILE*  fd;
		fd = fopen(dirName, "rb");
		if(fd==NULL)
			ERRMSG("[NVMS] ERR: DIR fopen(%s) fail = %x", dirName, errno);
		else
		{
			fid = fileno(fd);
			ret = fsync(fid);
			if(ret!=0) ERRMSG("[NVMS] ERR: DIR fsync(%d) fail = %x", fid, errno);
			fclose(fd);
		}
	}
	return(ret);
}

//=================================================
static int NVM_move_tmp2orig(char* tmpName, char* fName)
{
	//RENAME_UNKNOWN,             //Unknown => call for test
	//RENAME_DOS_LIKE             // delete-orig, rename
	//RENAME_FORCE_MOVE_LIKE,     //              rename
	//RENAME_FORCE_COPY_LIKE,     //              rename, delete-tmpf
	int ret;

	if(NVM_rename_behavior == RENAME_UNKNOWN)
		NVM_rename_test_behavior();
	DEBUGMSG("[NVMS] Rename behavior:%d\n",NVM_rename_behavior);
	if(NVM_rename_behavior == RENAME_DOS_LIKE)
		remove(fName);
	if( (ret=rename(tmpName, fName)) != 0 )
	{
		ERRMSG("[NVMS] ERR: fclose: rename %s > %s failed\n", tmpName,fName);
	}
	else
	if(NVM_rename_behavior == RENAME_FORCE_COPY_LIKE)
		remove(tmpName);

	if(nvmIoctlMode != NVM_MODE_NO_PARTIAL_SYNC)
	{
		if((NVM_root_dir == NULL) || (NVM_root_dir[0] != '/'))
		{
			//NVM_root_dir is not initilized from Environment
			//But since we are here it must be "NVM" (/Linux/Marvell/NVM or its link)
			//Try to obtain it another way
			char* NVM_root_dir_buf = malloc(NVM_FILE_NAME_MAX_LENGTH_LONG);
			NVM_root_dir_buf[NVM_FILE_NAME_MAX_LENGTH_LONG-1] = 0;
			NVM_root_dir = getcwd(NVM_root_dir_buf, NVM_FILE_NAME_MAX_LENGTH_LONG-1);
			ASSERT(NVM_root_dir != NULL);
		}
		NVM_dir_fsync(NVM_root_dir);
	}
	return(ret);
}


//=================================================
//  GLOBAL APIs: fopen, fclose
//=================================================
FILE*  fopen_pwrSafe(char* fName, char* szAttrib)
{
	#define TMP_BUF_SIZE   1024
	FILE*  fpTmp;
	char   tmpName[TMP_NVM_FILE_NAME_MAX_LENGTH];
	int    fileFailure;
	int    retry = 0;
	const int maxRetries = 3;
	int    checkRc;

	fpTmp = nvmIoctl_open(fName, szAttrib);
	if(fpTmp != NULL)
		return(fpTmp);

	checkRc = NVMS_checkNVM_file(fName, TRUE, 0);

        if(checkRc != 0)
            ERRMSG("[NVMS] ERR: open file %s did not passed checked!It has been removed\n", fName);

	if( ! NMVS_TmpUseRequired(fName, szAttrib))
	{
		if(checkRc != 0)
			return NULL; //Not exist or removed
		return( fopen(fName,szAttrib) );
	}

	//Write has been required. Use "tmp reliable" mechanism
	strncpy(tmpName, fName, NVM_FILE_NAME_MAX_LENGTH_LONG);
	strncat(tmpName, TMP_FNAME_POSTFIX, TMP_FNAME_POSTFIX_SIZE);

	//If TMP exist the problem was on fclose
	//  DO NOT RECOVER from tmp but remove it

	if( remove(tmpName) == 0 )
	{
		ERRMSG("[NVMS] ERR: ================================================================\n");
		ERRMSG("[NVMS] ERR: Recent fwrite & fclose ERROR: %s found and removed\n", tmpName);
		ERRMSG("[NVMS] ERR: ================================================================\n");
	}

	// COPY original file into temporary
	copyOrig2tmp:
	fileFailure = NVM_copyFile(fName,tmpName);
         ERRMSG("[NVMS] ERR: Copy oring 2 tmp for %d time, fileFailure = %d\n", retry, fileFailure);
	if(fileFailure == 1)
	{
		if(retry < maxRetries)
		{
			retry++;
			usleep(50000);usleep(50000);usleep(50000);  //Suppose it fixed by garbage collection
			goto copyOrig2tmp;
		}
		else
			return NULL;
	}


	if(fileFailure == 0)
	{
		if(NVMS_checkNVM_file(tmpName, FALSE, TMP_FNAME_POSTFIX_SIZE) !=0)
		{
			ERRMSG("[NVMS] ERR:Copy to temp file is not passed check \n");
			if(retry < maxRetries)
			{
				retry++;
				usleep(50000);usleep(50000);usleep(50000);  //Suppose it fixed by garbage collection
				goto copyOrig2tmp;
			}
		}
	}

	//Now we could work with temporary as has been required by user
	fpTmp = fopen(tmpName, szAttrib);
	if(fpTmp != NULL)
		NVM_save_fName_fd(tmpName, fpTmp);

	return(fpTmp);
}

//=================================================
int  fclose_pwrSafe(FILE* fd)
{
	int ret, fid;
	char* tmpName;
	char  fName[NVM_FILE_NAME_MAX_LENGTH_LONG];
	int    retry = 0;
	const int maxRetries = 2;

	if(nvmIoctl_close(fd) == 0)
		return(0);

	tmpName = NVM_get_fName_by_fd(fd);
	if(tmpName != NULL)
	{
		fid = fileno(fd);
		if(fid==-1)
			ERRMSG("[NVMS] ERR: fileno(0x%x) fail = %x. File on FLASH may be not SYNC\n", (unsigned int)fd, errno);
		else
		{
			if(nvmIoctlMode != NVM_MODE_NO_PARTIAL_SYNC)
			{
				ret = fsync(fid);
				if(ret!=0) ERRMSG("[NVMS] ERR: fsync(%d / %x) fail = %x\n", fid, (unsigned int)fd, errno);
			}
			}
	}

	ret = fclose(fd);

	if((tmpName != NULL) && (ret == 0))
	{
		int len = strlen(tmpName) - TMP_FNAME_POSTFIX_SIZE;

		if(1==NVMS_checkNVM_file(tmpName, FALSE, TMP_FNAME_POSTFIX_SIZE))
		{
			free(tmpName);
			return(0); //TMP file corrupted by incorrect WRITE; remove it
			//return( NVM_STATUS_FS_ERROR ); //exit with error and so COMM goes to ASSERT
		}

		memcpy(fName, tmpName, len);
		fName[len] = '\0';
		while(retry++ < maxRetries)
		{

			ret = NVM_move_tmp2orig(tmpName, fName);
			ERRMSG("[NVMS] ERR: move tmp 2 oring for %d time, fileFailure = %d\n", retry, ret);
			if(ret == 0)
				break;
		}
		free(tmpName);
	}
	return(ret);
}

//=================================================
void ffs_pwrSafe_Init(char* nvm_root_dir)
{
	static int passed=0;
	if(passed == 0)
	{
		PREEMPTION_SEMA_INIT();

		// The "nvm_root_dir" may be NULL at that point
		if((NVM_root_dir == NULL) && (nvm_root_dir != NULL) && (nvm_root_dir[0] == '/'))
		{
			strcpy(NVM_root_dir_buf,nvm_root_dir);
			NVM_root_dir = NVM_root_dir_buf;
			NVM_rename_test_behavior();
		}

		NVMS_checkNVM_dir(NVM_root_dir);
	}
	passed++;
}

/**************************************************************************
*            COMM Silent Reset
* When COMM assert occures there could be opened file(s)
* If the further COMM Silent Reset applied by APPS these files stay open
* and could make problems.
* To avoid this situation the fopen-fd is saved and removed on fclose
* This is done by  Keep / Free
***************************************************************************
*/
typedef struct
{
	struct CList	*pNext;
	UINT32			hFile;
}CList;

typedef struct
{
	CList *head;
	CList *tail;
}CListHeader;

CListHeader  nvmsHFileList[2];  //  APPS/ACAT=0, COMM=1

UINT32 nvmsHFileListCntr;

static void nvmsHFileKeep(NVM_CLIENT_ID ClientID, UINT32 hFile)
{
	int i = (ClientID == NVM_CLIENT_COMM);
	CList *listHead = nvmsHFileList[i].head;
	CList *listTail = nvmsHFileList[i].tail;
	CList *pNew;

	pNew = (CList*)malloc(sizeof(CList));
	ASSERT(pNew);
	DEBUGMSG("[NVMS] %s(%d) for %d files 1\n", __func__, i, nvmsHFileListCntr);

	pNew->pNext = NULL;
	pNew->hFile = hFile;

	// the first entry
	if (listHead == NULL)
		listHead = pNew;
	else
		listTail->pNext = (struct CList*)pNew;

	listTail = pNew;

	nvmsHFileList[i].head = listHead;
	nvmsHFileList[i].tail = listTail;
	nvmsHFileListCntr++;
}

static void nvmsHFileFree(NVM_CLIENT_ID ClientID, UINT32 hFile)
{
	int i = (ClientID == NVM_CLIENT_COMM);
	CList *listHead = nvmsHFileList[i].head;
	CList *listTail = nvmsHFileList[i].tail;
	CList *pCurrent	= listHead;
	CList *pPrev	= NULL;
	BOOL   found    = FALSE;

	if (pCurrent == NULL)
	{  // nothing kept, but try to free
		ERRMSG("[NVMS] ERR: %s(%d) nothing to free (cntr=%d)\n", __func__, i, nvmsHFileListCntr);
		return;
	}
	DEBUGMSG("[NVMS] %s(%d) for %d files\n", __func__, i, nvmsHFileListCntr);
	do
	{ //look for hF loop
		if (pCurrent->hFile == hFile)
		{
			found = TRUE;
			break;
		}
		pPrev = pCurrent;
		pCurrent = (CList*)pCurrent->pNext;

	} while (pCurrent != NULL);

	// delete "pCurrent" from list
	if (found)
	{
		if((pCurrent == listHead) && (pCurrent == listTail))
			listHead = listTail = NULL;
		else
		if (pCurrent == listTail)
		{
			listTail = pPrev;
			listTail->pNext = NULL;
		}
		else
		if (pCurrent == listHead)
			listHead = (CList*)pCurrent->pNext;
		else
			pPrev->pNext = pCurrent->pNext;

		nvmsHFileList[i].head = listHead;
		nvmsHFileList[i].tail = listTail;
		free(pCurrent);
		nvmsHFileListCntr--;
	}
	else
	{
		ERRMSG("[NVMS] ERR: %s(%d) hFile_%x not found (cntr=%d)\n", __func__, i, hFile, nvmsHFileListCntr);
	}
}

static void nvmsHFileCloseFreeAll(NVM_CLIENT_ID ClientID)
{
	int i = (ClientID == NVM_CLIENT_COMM);
	CList *pCurrent	= nvmsHFileList[i].head;
	CList *pTmp;

	DEBUGMSG("[NVMS] %s(%d) for %d files\n", __func__, i, nvmsHFileListCntr);

	while (pCurrent != NULL)
	{
		if(pCurrent->hFile)
		{
			if(pCurrent->hFile != (UINT32)&nvmIoctlMode)
				fclose((FILE*)pCurrent->hFile);
		}
		else
			{ ERRMSG("[NVMS] ERR: %s(%d)  hFile=0\n", __func__, i); }
		pTmp = pCurrent;
		pCurrent = (CList*)pCurrent->pNext;
		free(pTmp);
	}
	nvmsHFileList[i].head = NULL;
	nvmsHFileList[i].tail = NULL;
	nvmsHFileListCntr = 0;

	if(ClientID == NVM_CLIENT_COMM)
		memset(NVM_fDesc, 0, sizeof(NVM_fDesc[FOPEN_MAX_WRITE]));
}



//======================================================================
// Check  *.gki / *.gki_tmpf / *.nvm / *.nvm_tmpf files
//  for consistency of the NVM-format: nvmHeaderSize and StructSize
// The COMM compiler packs structure differently then APPS GCC does;
//  so the sizeof(NVM_Header_ts) may be different on APPS and COMM
//   ==> set "nvmHeaderSize" by define.
#define nvmHeaderSize   264
#define MAX_NVM_RECORD_STRUCTS	  1
#define MAX_GKI_RECORD_STRUCTS	  6

//#define KEEP_CORRUPTED_NVM_FILE
#if !defined(KEEP_CORRUPTED_NVM_FILE)
#define CHECKNVM_CORRUPT_FOUND   1
#define CHECKNVM_ABNORMAL       -1
#else
#define CHECKNVM_CORRUPT_FOUND   0  /*return OK instead error*/
#define CHECKNVM_ABNORMAL        0  /*return OK instead error*/
#endif


//======================================================================
int NVMS_checkNVM_file(char* fileName, int removeReq, int useTmp)
{
	char*   p;
	struct  stat fs;
	int     fileNameGKI, fileNameNVM;
	FILE*   fp;
	unsigned int  structSize;
	int     ret;
	UINT32  i, r, dataSize;
	char   *removeStr;
	int     err_track = 0;

#if defined(KEEP_CORRUPTED_NVM_FILE)
	removeReq = 0;
#endif

	//Files related to "audio" should not be checked, but ret-OK
	if(strstr(fileName, "audio") != NULL)
		return 0;

	if(useTmp == 0)
	{
		p = strstr(fileName, ".gki");
		fileNameGKI = (p != NULL) && (p[4] == 0);
	}
	else
	{
		p = strstr(fileName, ".gki" TMP_FNAME_POSTFIX);
		fileNameGKI = (p != NULL) && (p[4+TMP_FNAME_POSTFIX_SIZE] == 0);
	}
	fileNameNVM = 0;

	if(!fileNameGKI)
	{
		if(useTmp == 0)
		{
			p = strstr(fileName, ".nvm");
			fileNameNVM = (p != NULL) && (p[4] == 0);
		}
		else
		{
			p = strstr(fileName, ".nvm" TMP_FNAME_POSTFIX);
			fileNameNVM = (p != NULL) && (p[4+TMP_FNAME_POSTFIX_SIZE] == 0);
		}
	}

	if(fileNameGKI || fileNameNVM)
	{
		FILE *uncheck_fd = NULL;
		char uncheck_file[256];

		uncheck_fd = fopen(nvmCheckIgnoreFile, "r");
		if(uncheck_fd == NULL)
		ERRMSG("[NVMS] Can not find %s, all the files must be checked!\n",nvmCheckIgnoreFile);
		else
		{
			while(fgets(uncheck_file, 256,uncheck_fd))
			{
				int length = strlen(uncheck_file);
				uncheck_file[length-1] = '\0';  //remove '\n' at the end.
				//ERRMSG("[NVMS] file need not to check :%s, file:%s\n",uncheck_file, fileName);
				if(strstr(fileName,uncheck_file) != NULL)
				{
					ERRMSG("[NVMS] file %s should be ignored, no need to check!\n",fileName);
					fclose(uncheck_fd);
					return 0;
				}
			}
			ERRMSG("[NVMS] file %s should be checked!\n",fileName);
			fclose(uncheck_fd);
		}
		ret= stat(fileName,&fs);
		if(ret != 0)
			return 0; // *.nvm or *.gki has been requested but not exist - it's OK

		if(removeReq)    removeStr = "REMOVED";
		else             removeStr = " ";

		if(fs.st_size < nvmHeaderSize)
		{
			ERRMSG("[NVMS] ERR: file %s is too short %u. %s\n", fileName, (unsigned int)fs.st_size, removeStr);
			if(removeReq)
				remove(fileName);
			return(CHECKNVM_CORRUPT_FOUND); // Ups, HIT
		}

		fp = fopen(fileName,"rb");
		if(fp == NULL)
		{
			ERRMSG("[NVMS] ERR: cannot open file %s although it is in dir-list!\n", fileName);
			return(CHECKNVM_ABNORMAL);
		}
		structSize = 0;
		fread(&structSize,4,1, fp);
		fclose(fp);

		i = 0;
		r = 0;
		if(structSize == 0)
			err_track = 0;
		else
		{
			if( (UINT32)fs.st_size == (UINT32)(nvmHeaderSize) ) return 0; //this is possible OK

			dataSize = (UINT32)fs.st_size - nvmHeaderSize;
			i = dataSize / (UINT32)structSize;
			r = dataSize % (UINT32)structSize;
			if(fileNameNVM)
			{
				if((i <=MAX_NVM_RECORD_STRUCTS) && (r == 0))  return 0;
				err_track = 1;
			}
			else
			{ //This is GKI:
				if((i <=MAX_GKI_RECORD_STRUCTS) && (r == 0))  return 0;
				err_track = 2;
			}
		}
		ERRMSG("[NVMS] ERR: file %s corrupted - Size_%u != (%d + structSize_%u * %u) = %u. %s in %d\n",
			fileName, (unsigned int)fs.st_size, nvmHeaderSize, structSize, (unsigned int)i, (unsigned int)r,
			removeStr, err_track);
		if(removeReq)
			remove(fileName);

		return(CHECKNVM_CORRUPT_FOUND);  // Ups, HIT
	}//(fileNameGKI || fileNameNVM)
	return(0);
}

//=============================================================
void NVMS_checkNVM_dir(char* nvm_root_dir)
{
	DIR*    dirstream = NULL;
	struct  dirent* ent;
	int     badFileFound = 0;
	char    buf[100];

	if(nvm_root_dir == NULL)
	{
		ERRMSG("[NVMS] ERR: cannot find NVM directory\n");
		return;
	}

	//Use ERROR_MSG but not TRACE_MSG since ACAT may be unavailable...
	ERRMSG("\n[NVMS] Check %s/*.nvm and .gki consistency for nvmHeaderSize=%d. Bad files will be removed\n",
						nvm_root_dir, nvmHeaderSize);

	chdir(nvm_root_dir);

	do
	{
		badFileFound = 0;
		dirstream = opendir(nvm_root_dir);
		if(dirstream == NULL) {
			ERRMSG("[NVMS] ERR: cannot open NVM directory\n");
			return; //ASSERT
		}

		do{
			ent = readdir(dirstream);
			if(NULL==ent)break;
			if(strcmp(ent->d_name, "..") == 0 || strcmp(ent->d_name, ".") == 0)
				continue;

			badFileFound = NVMS_checkNVM_file(ent->d_name, FALSE, 0);

			if(badFileFound == 1)
			{
				strcpy(buf, ent->d_name);
				closedir(dirstream);
				ERRMSG("[NVMS] ERR: file %s/%s REMOVED\n", nvm_root_dir, buf);
				dirstream = NULL;
				remove(buf);
				break;
			}
		}while(ent!=NULL);
	}while(badFileFound == 1);

	if(dirstream != NULL)
		closedir(dirstream);
}

static int NVM_copyFile(char * source,char*dest)
{
	int ret = 0;
	struct  stat fs;
	int count =0;
         char*  buf;
         int    bRead, bWrite;
        #define TMP_BUF_SIZE   1024
          FILE*  fpOrig;
	FILE*  fpTmp;
	ERRMSG("[NVMS] CopyFile:  %s -> %s COPY\n", source,dest);
	{
		fpOrig = fopen(source, "rb");
		if(fpOrig != NULL)
		{
			ret = stat(source,&fs);
			if(ret !=0)
				ERRMSG("[NVMS] ERR: fopen: %s  Open failed:%s\n",source,strerror(errno));

			fpTmp = fopen(dest, "wb");
			if(fpTmp != NULL)
			{
				buf = malloc(TMP_BUF_SIZE);
				ASSERT(buf != NULL);
				do
				{
					bRead  = fread (buf, 1, TMP_BUF_SIZE, fpOrig);
					if(bRead == 0)
						break;
					bWrite = fwrite(buf, 1, bRead, fpTmp);
					count += bWrite;
					if(bWrite != bRead)
					{
						ERRMSG("[NVMS] ERR:wirte %d, != read %d",bWrite,  bRead);
						break;
					}
				} while(bRead == TMP_BUF_SIZE);
				free(buf);
				if(count != fs.st_size)
				{
					ERRMSG("[NVMS] ERR:  %s(%d) -> %s(%d) COPY failed:%s\n", source,fs.st_size,dest,count,strerror(errno));
					ret = 1;
				}
				fflush(fpTmp);
				fclose(fpTmp);
			}
			else
			{
				ERRMSG("[NVMS] ERR: fopen: %s  Open failed\n",dest);
				ret = 1;
			}
			fclose(fpOrig);
		}else
		{
			ERRMSG("[NVMS] ERR: fopen: %s  Open failed\n",source);
			ret = 2;
		}
	}

	return ret;
}

#ifdef __cplusplus
}
#endif

