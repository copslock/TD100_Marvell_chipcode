/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (“Material”) are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/*******************************************************************************
*               MODULE HEADER FILE
********************************************************************************
*
* Filename: EE_Postmortem.c
*
* The BOERNE could access the whole HARBELL's DDR for postmortem debugging
* Let's use this possibility for the fetching HARBELL-DDR dump into BIN-file.
* This is especially usable in case of exceptions.
*
* There are next utilities:
*  - SaveComMIPSbuf     keep the MIPS-RAM   buffer. The Address of buffer is FIXED-PreDefined
*  - SaveComEEbuf       keep the EE-handler buffer. The Address of buffer is FIXED-PreDefined
*  - SaveCom_Addr_Len   make dump of any COMM-DDR area started from with a given address and length.
*                                    The address may be given even in the COMM-addressing format (0xD0Xxxxxx)
*
*******************************************************************************/
#if defined(_TAVOR_BOERNE_) || defined(_TAVOR_HARBELL_)


#if defined (OSA_WINCE)
#include <windows.h>
#include <ceddk.h>
#include <Regext.h>
#include <diag_api.h>
#else
#include <string.h>  // for memcpy() & strcpy()
#include <stdio.h>
#if defined (OSA_LINUX)
#include "pxa_dbg.h"
#else
#include "asserts.h"
#endif
#endif

#include "EE_Postmortem.h"

#include "EEHandler.h"
#include "EEHandler_int.h"

#if !defined(_TAVOR_BOERNE_)
#include "EE_PostmortemApi.h"
#endif //!(_TAVOR_BOERNE_)

#define ENTRY_NOT_FOUND    ((EE_PostmortemDesc_Entry*)0xDEADDEAD)

//The EE_PostmortemDesc may be printed when ICAT postmortem command sent
//#define PRINT_POSTMORTEM_DESC

#if defined (OSA_WINCE) || defined (OSA_LINUX)
#define CacheCleanMemory(X, Y)      /* */
#else
extern void CacheCleanMemory( void *pMem, UINT32 size);
#endif

#if defined (_TAVOR_HARBELL_)
#include "mipsram_def.h"
extern EE_LogBuffer __CurrentExceptionLog;

EE_PostmortemDesc_Entry EE_PostmortemDesc[POSTMORTEM_NUMOF_ENTRIES] =
{
/*   Buf-Name[8]                   UINT8* pBuf                        UINT32 len    */
	POSTMORTEM_BUF_NAME_EEBUF,   (UINT8*)&__CurrentExceptionLog.log, sizeof(__CurrentExceptionLog.log), /*sizeof(EE_Entry_t)*/
#if defined(MIPS_TEST_RAM)
	POSTMORTEM_BUF_NAME_MIPSRAM, (UINT8*)mipsRamBuffer,		 sizeof(mipsRamBuffer),
#endif
	0,			     0,					 0,
	0,			     0,					 0 /*end must be zeroed*/
};
// PUB_ADDR_OF_PTR2POSTMORTEM_DESC <- &EE_PostmortemDesc[0]

#endif // _TAVOR_HARBELL_


UINT32 EE_AssistingMaster_ASSERT_found = 0;


BOOL EE_PostmortemRegistration(char* name, UINT8* bufAddr, UINT32 bufLen)
{
	UNUSEDPARAM(name)
	UNUSEDPARAM(bufAddr)
	UNUSEDPARAM(bufLen)

#if !defined (_TAVOR_HARBELL_)
	return TRUE;
#else
	char nameBuf[POSTMORTEM_BUF_NAME_SIZE];
	int i;
	UINT32 cpsrReg;

	//Check neme length and cut if needed
	memcpy(nameBuf, name, POSTMORTEM_BUF_NAME_SIZE);
	nameBuf[POSTMORTEM_BUF_NAME_SIZE - 1] = 0;

	//Check the name is already registered
	for (i = 0; i < (POSTMORTEM_NUMOF_ENTRIES - 1); i++)
		if (strcmp(EE_PostmortemDesc[i].name, nameBuf) == 0)
		{ //Already exists => update pBuf and size
			EE_PostmortemDesc[i].bufAddr = bufAddr;
			EE_PostmortemDesc[i].bufLen  = bufLen;
			CacheCleanMemory(&EE_PostmortemDesc[i], sizeof(EE_PostmortemDesc_Entry));
			return TRUE;
		}

	//Look for a free entry (Don't forget, the last should be zero)
	for (i = 0; i < (POSTMORTEM_NUMOF_ENTRIES - 1); i++)
	{
		cpsrReg = disableInterrupts(); //Protect from task-switch
		if (EE_PostmortemDesc[i].name[0] == 0)
		{
			//Add to Table with disableInterrupts
			memcpy(EE_PostmortemDesc[i].name, nameBuf, POSTMORTEM_BUF_NAME_SIZE);
			EE_PostmortemDesc[i].bufAddr = bufAddr;
			EE_PostmortemDesc[i].bufLen  = bufLen;
			CacheCleanMemory(&EE_PostmortemDesc[i], sizeof(EE_PostmortemDesc_Entry));
			restoreInterrupts(cpsrReg);
			return (TRUE);
		}
		restoreInterrupts(cpsrReg);
	}
	return(FALSE);
#endif  // _TAVOR_HARBELL_
}


#if defined(INTEL_FDI) && defined (_TAVOR_BOERNE_)

#if !defined (OSA_WINCE)
#if !defined (OSA_LINUX)
#include "mmap_phy.h"
#endif
#else
//////////////////////static volatile UINT32 _BOERNE_ADDR_EXEC_REGION_BASE=0;
#define NVM_SERVER_KEY                          TEXT("Drivers\\BuiltIn\\NVMServer\\")
#define PREFIX_KEY                                      TEXT("Prefix")
#endif //(OSA_WINCE)

#if defined(OSA_LINUX)
#define DDR_RW_SAVE_DIR "/data"
#endif

static void SaveToFile(char* fileName, UINT8* pBuf, UINT32 len)
{

#if !defined(OSA_WINCE)
	FILE_ID fdiID;
#else
	TCHAR path[MAX_PATH] = { 0 };
	char fullPath[MAX_PATH] = { 0 };
	FILE *fdiID;
#endif
	UINT32 count;
	char fNameBuf[MAX_PATH] = {0};
	UINT32 nameLen;

	//cut given name if too long
	nameLen = strlen(fileName);
	if (nameLen >= POSTMORTEM_BUF_NAME_SIZE)
		fileName[POSTMORTEM_BUF_NAME_SIZE - 1] = 0;

#if defined (OSA_WINCE)
	RegistryGetString(HKEY_LOCAL_MACHINE, L"\\Drivers\\BuiltIn\\NVMServer\\Clients\\Comm", L"ReadWritePath", path, MAX_PATH);
	wcstombs(fullPath, path + 2, sizeof(fullPath));
#endif

#if defined(OSA_LINUX)
	if(!strcmp(fileName, "DDR_RW") /* for DDR_RW, save it in DDR_RW_SAVE_DIR */
#if defined(TD_SUPPORT)
		|| !strcmp(fileName, "DIAGLOG") /* for DIAG LOG OFFLINE, save it in DDR_RW_SAVE_DIR */
#endif
		)
		sprintf(fNameBuf, "%s/com_%s.bin", DDR_RW_SAVE_DIR, fileName);
	else
#endif
		sprintf(fNameBuf, "com_%s.bin", fileName);
	//The FDI does NOT update the FILE-time-date attributes. So delete and create with new date
	//if(FStatusIsExist(fNameBuf))

#if !defined (OSA_WINCE)
	FDI_remove((char *)fNameBuf);
#endif  //!defined (OSA_WINCE)

	if (len == 0)
	{
		DIAG_FILTER(SW_PLAT, PERFORMANCE, SAVE2FILE0, DIAG_INFORMATION)
		diagPrintf("COMM MEM-DUMP File %s: nothing to save", fNameBuf);
	}
	else
	{
		//pBuf = ADDR_CONVERT(pBuf);
#if !defined(OSA_WINCE)
		if ((fdiID = FDI_fopen(fNameBuf, "wb")) != 0)
#else
		strcat(fullPath, "\\");
		strcat(fullPath, fNameBuf);
		if ((fdiID = fopen(fullPath, "wb")) != 0)
#endif
		{
#if !defined(OSA_WINCE)
			pBuf = (UINT8*)MAP_PHYSICAL_TO_VIRTUAL_ADDRESS(pBuf);
			count = FDI_fwrite(pBuf, sizeof(char), len, fdiID);
			FDI_fclose(fdiID);
#else

			pBuf = (UINT8*)MAP_PHYSICAL_TO_VIRTUAL_ADDRESS(pBuf);
			count = fwrite(pBuf, sizeof(char), len, fdiID);
			fclose(fdiID);
#endif
			DIAG_FILTER(SW_PLAT, PERFORMANCE, SAVE2FILE, DIAG_INFORMATION)
			diagPrintf("COMM MEM-DUMP File %s: %ld bytes saved", fNameBuf, count);
		}
	}
}


static EE_PostmortemDesc_Entry* getEntryNext(int getFirst)
{
	void* whellKnownAddr = (void*)(MAP_PHYSICAL_TO_VIRTUAL_ADDRESS(PUB_ADDR_OF_PTR2POSTMORTEM_DESC));
	static EE_PostmortemDesc_Entry * pCurr;
	static int idx;

	if (getFirst)
	{
		DBGMSG("%s: PUB_ADDR_OF_PTR2POSTMORTEM_DESC:0x%x\n", __FUNCTION__, PUB_ADDR_OF_PTR2POSTMORTEM_DESC);
		DBGMSG("%s: wellKnownAddr:0x%x\n", __FUNCTION__, whellKnownAddr);
		pCurr = (EE_PostmortemDesc_Entry*)/*ADDR_CONVERT*/ (*(UINT32*)/*ADDR_CONVERT*/ (whellKnownAddr));
		idx   = 0;
	}
	else
	{
		pCurr++;
		idx++;

		pCurr =  (EE_PostmortemDesc_Entry *)MAP_PHYSICAL_TO_VIRTUAL_ADDRESS(pCurr);
		if ((((EE_PostmortemDesc_Entry*)/*ADDR_CONVERT*/ (pCurr))->name[0] == 0) || (idx >= POSTMORTEM_NUMOF_ENTRIES))
			return(ENTRY_NOT_FOUND);
	}
	DBGMSG("%s: pCurr:0x%x\n", __FUNCTION__, pCurr);
	return( (EE_PostmortemDesc_Entry*)/*ADDR_CONVERT*/ (pCurr) );
}


static EE_PostmortemDesc_Entry* getEntryByName(char* name)
{
	int i;

	EE_PostmortemDesc_Entry * retAddr =  ENTRY_NOT_FOUND;
	void* whellKnownAddr = (void*)MAP_PHYSICAL_TO_VIRTUAL_ADDRESS(PUB_ADDR_OF_PTR2POSTMORTEM_DESC);
	EE_PostmortemDesc_Entry * p = (EE_PostmortemDesc_Entry*)(*(UINT32*)(whellKnownAddr));

	p = (EE_PostmortemDesc_Entry*)MAP_PHYSICAL_TO_VIRTUAL_ADDRESS(p);

	for (i = 0; i < POSTMORTEM_NUMOF_ENTRIES; i++)
	{
		if (p->name[0] == 0)
			break;  //Last END-entry. Just exit
		else
		{
			if (strcmp(p->name, name) == 0)
			{
				retAddr = p;
				break; // found
			}
			p++;
		}
	}
	return( (EE_PostmortemDesc_Entry*)/*ADDR_CONVERT*/ (retAddr) );
}

static void printEEinfo(UINT8* pBuf)
{
	// The pBuf points onto __CurrentExceptionLog.log
	// Keep it in the pEElog after Comm/App address convertion.
	// Since we need also the buffer CheckSum and Signature
	// let's find also the buffer beginning pEE.

	EE_Entry_t  *  pEElog = ((EE_Entry_t*)/*ADDR_CONVERT*/ (pBuf));
	EE_LogBuffer*  pEE;
	UINT32 logOffset;

	logOffset  = (UINT32)&( ((EE_LogBuffer*)pEElog)->log );
	logOffset -= (UINT32)pEElog;

	pEE =  (EE_LogBuffer*)( (UINT32)pEElog - logOffset );

	EE_AssistingMaster_ASSERT_found = (pEE->logSignature == EE_RAM_BUFFER_SIGNATURE);
	if (EE_AssistingMaster_ASSERT_found)
	{
		DIAG_FILTER(Diag, comMem, EE_LOG_TXT, DIAG_INFORMATION)
		diagPrintf("COMM EE LOG: %s", pEElog->desc);
		DIAG_FILTER(Diag, comMem, EE_LOG, DIAG_INFORMATION)
		diagStructPrintf("COMM EE LOG:  %S{EE_Entry_t}", (UINT16 *)pEElog, sizeof(EE_Entry_t));
	}
	else
	{
		DIAG_FILTER(Diag, comMem, EE_LOG_NOERR, DIAG_INFORMATION)
		diagPrintf("COMM WDT expired (NOT caused by ASSERT)" );
	}
}

//-----------------------------------------------------------------------------
//ICAT EXPORTED FUNCTION - Diag,comMem,SaveComPostmortem
void SaveComPostmortem(void)
{
	EE_PostmortemDesc_Entry*  p; // = getEntryByName( POSTMORTEM_BUF_NAME_EEBUF );

	p = getEntryNext(1 /*getFirst*/);
	while (p != ENTRY_NOT_FOUND)
	{
     #ifdef PRINT_POSTMORTEM_DESC
		DIAG_FILTER(Diag, comMem, DESC, DIAG_INFORMATION)
		diagStructPrintf("%S{EE_PostmortemDesc_Entry}", (void*)p, sizeof(EE_PostmortemDesc_Entry));
     #endif
		p = (EE_PostmortemDesc_Entry*)MAP_PHYSICAL_TO_VIRTUAL_ADDRESS(p);
		DBGMSG("%s: p: 0x%x\n", __FUNCTION__, p);
		if (strcmp(p->name, POSTMORTEM_BUF_NAME_EEBUF) == 0)
		{
			printEEinfo((UINT8*)MAP_PHYSICAL_TO_VIRTUAL_ADDRESS(p->bufAddr));
		}

		SaveToFile( p->name, /*ADDR_CONVERT*/ (p->bufAddr), p->bufLen);
		p = getEntryNext(0 /*getFirst*/);
	}
}

//ICAT EXPORTED FUNCTION - Diag,comMem,SaveCom_Addr_Len
void SaveCom_Addr_Len(void* p)
{
	UINT32*           pVal = (UINT32*)MAP_PHYSICAL_TO_VIRTUAL_ADDRESS(p);
	UINT8*            pBuf = (UINT8*)pVal[0];
	UINT32 len  =          pVal[1];


	SaveToFile( POSTMORTEM_BUF_NAME_GENDUMP, pBuf, len);
}

//-----------------------------------------------------------------------------
void EE_SaveComDDR_RW(UINT32 sizeLimitBytes)
{
	LoadTableType  *p = (LoadTableType*)MAP_PHYSICAL_TO_VIRTUAL_ADDRESS(pLoadTable);
	UINT8*  pBuf;
	UINT32 len;

	if (p != (LoadTableType*)INVALID_ADDRESS)
	{
		pBuf = (UINT8*)(p->ramRWbegin);

		if (sizeLimitBytes == 0)        //do noting
			return;
		if (sizeLimitBytes <= 1024)     //do NOT limit but use default - full dump
			len  = p->ramRWend - p->ramRWbegin + 4;
		else
		{       //len = MIN(A,B)
			len  = ((p->ramRWend - p->ramRWbegin + 4) < sizeLimitBytes) ?
			       (p->ramRWend - p->ramRWbegin + 4) : sizeLimitBytes;
		}
		DIAG_FILTER(Diag, comMem, DDR_RW, DIAG_INFORMATION)
		diagPrintf("WAIT 1MIN! Be patient! DDR_RW Dumping 0x%lx bytes of DDR_RW", len);
		SaveToFile( "DDR_RW", pBuf, len); //"com_DDR_RW.bin"
	}
}
#if defined(TD_SUPPORT)
//-----------------------------------------------------------------------------
void EE_SaveComDDR_DiagOffline(UINT32 sizeLimitBytes)
{
	UINT8*  pBuf = (UINT8*)(COM_DIAG_OFFLINE_BEGIN);
	UINT32 len;

	if (pBuf != NULL)
	{
		if (sizeLimitBytes == 0)        //do noting
			return;
		if (sizeLimitBytes <= 1024)     //do NOT limit but use default - full dump
			len  = COM_DIAG_OFFLINE_END - COM_DIAG_OFFLINE_BEGIN + 4;
		else
		{       //len = MIN(A,B)
			len  = ((COM_DIAG_OFFLINE_END - COM_DIAG_OFFLINE_BEGIN + 4) < sizeLimitBytes) ?
			       (COM_DIAG_OFFLINE_END - COM_DIAG_OFFLINE_BEGIN + 4) : sizeLimitBytes;
		}
		DIAG_FILTER(Diag, comMem, DDR_DiagOffline, DIAG_INFORMATION)
		diagPrintf("WAIT 30 seconds! Be patient! DDR_DiagOffline Dumping 0x%lx bytes of DIAGLOG", len);
		SaveToFile( "DIAGLOG", pBuf, len); //"com_DIAGLOG.bin"
	}
}
#endif

//Get CP Error Description Information
BOOL GetComEEDescbuf(char *pDesc, int len)
{
	BOOL res = FALSE;
	EE_PostmortemDesc_Entry*  p = getEntryByName( POSTMORTEM_BUF_NAME_EEBUF );

	if (p != ENTRY_NOT_FOUND)
	{
		EE_Entry_t  *  pEElog = ((EE_Entry_t*)MAP_PHYSICAL_TO_VIRTUAL_ADDRESS(p->bufAddr));
		EE_LogBuffer*  pEE;
		UINT32 logOffset;

		logOffset  = (UINT32)&( ((EE_LogBuffer*)pEElog)->log );
		logOffset -= (UINT32)pEElog;

		pEE =  (EE_LogBuffer*)( (UINT32)pEElog - logOffset );

		EE_AssistingMaster_ASSERT_found = (pEE->logSignature == EE_RAM_BUFFER_SIGNATURE);
		if (EE_AssistingMaster_ASSERT_found)
		{
			snprintf(pDesc, len, "COMM EE LOG: %s", pEElog->desc);
			res = TRUE;
		}
		else
			snprintf(pDesc, len, "COMM WDT expired (NOT caused by ASSERT)");
	}
	else
		snprintf(pDesc, len, "COMM WDT expired (NOT caused by ASSERT)");
	return res;
}


#if defined (EE_POSTMORTEM_EXTEND_ICAT_EXPORT)
//-----------------------------------------------------------------------------
//ICAT EXPORTED FUNCTION - Diag,comMem,SaveComDDR_RW
void EE_SaveComDDR_RW_ACAT(UINT32 *pSizeLimitBytes)
{
	UINT32 sizeLimitBytes = *pSizeLimitBytes;

	EE_SaveComDDR_RW(sizeLimitBytes);
#if defined (TD_SUPPORT)
	EE_SaveComDDR_DiagOffline(sizeLimitBytes);
#endif
}


//-----------------------------------------------------------------------------
//ICAT EXPORTED FUNCTION - Diag,comMem,SaveComMIPSbuf
void SaveComMIPSbuf(void)
{
	EE_PostmortemDesc_Entry*  p = getEntryByName( POSTMORTEM_BUF_NAME_MIPSRAM );

	if (p != ENTRY_NOT_FOUND)
	{
		DIAG_FILTER(Diag, comMem, MIPS, DIAG_INFORMATION)
		diagStructPrintf("%S{EE_PostmortemDesc_Entry}", (VOID*)p, sizeof(EE_PostmortemDesc_Entry));
		SaveToFile( p->name, /*ADDR_CONVERT*/ (p->bufAddr), p->bufLen);
	}
}

//ICAT EXPORTED FUNCTION - Diag,comMem,SaveComEEbuf
void SaveComEEbuf(void)
{
	EE_PostmortemDesc_Entry*  p = getEntryByName( POSTMORTEM_BUF_NAME_EEBUF );

	if (p != ENTRY_NOT_FOUND)
	{
		printEEinfo(p->bufAddr);
		DIAG_FILTER(Diag, comMem, EE, DIAG_INFORMATION)
		diagStructPrintf("%S{EE_PostmortemDesc_Entry}", (VOID*)p, sizeof(EE_PostmortemDesc_Entry));
		SaveToFile( p->name, /*ADDR_CONVERT*/ (p->bufAddr), p->bufLen);
	}
}
#endif //EE_POSTMORTEM_EXTEND_ICAT_EXPORT

#endif //INTEL_FDI && _TAVOR_BOERNE_


#endif //_TAVOR_BOERNE_  _TAVOR_HARBELL_

