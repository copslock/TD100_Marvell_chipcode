/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: pb_api.c
 *
 *  Description: API interface implementation for PBK service
 *
 *  History:
 *   Oct 10, 2008 - Qiang Xu Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/
#include <sys/ioctl.h>
#include <fcntl.h>

#include "ci_api_types.h"
#include "ci_api.h"
#include "telatci.h"
#include "teldef.h"
#include "telatparamdef.h"
#include "telutl.h"
#include "ci_pb.h"
#include "utlMalloc.h"
#include "telpb.h"
#include "sim_api.h"

#include <pthread.h>
#include <stdlib.h>

/************************************************************************************
 *
 * PBK related const and global variables
 *
 *************************************************************************************/
extern CiServiceHandle gAtciSvgHandle[CI_SG_NUMIDS + 1];
extern unsigned short g_pbEntryStr[CI_MAX_NAME_LENGTH + ATCI_NULL_TERMINATOR_LENGTH];
extern AtciCharacterSet chset_type;
extern UINT16 PBdelindex;
/* Array of phonebook storage names */
const char* gPbStorageStr[] =
{
	"CU", /* "Current phonebook" Not a valid name in some cases */
	"DC",
	"EN",
	"FD",
	"LD",
	"MC",
	"ME",
	"MT",
	"ON",
	"RC",
	"SM",
	"AP",
	"MBDN",
	"MN"
};

/* enum to record current operation, used when processing same CI Cnf msg */
typedef enum
{
	AT_PBK_OPERATION_QUERY_STORAGE = 1,
	AT_PBK_OPERATION_GET_STORAGE,
	AT_PBK_OPERATION_SET_STORAGE,
	AT_PBK_OPERATION_QUERY_READ,
	AT_PBK_OPERATION_READ_ENTRY,
	AT_PBK_OPERATION_QUERY_WRITE,
	AT_PBK_OPERATION_WRITE_ENTRY,
	AT_PBK_OPERATION_FIND_ENTRY,
	AT_PBK_OPERATION_NULL,
} AT_MSG_OPERATION;


/* Global variable to record current read index and last index, used in +CPBR=<index1>,<index2> and +CPBF
  *gCurrReadIndex[0] and  gReadEndIndex[0] is for +CPBR
  *gCurrReadIndex[1] and  gReadEndIndex[1] is for +CPBF
  */
static int gCurrReadIndex[2] = {0}, gReadEndIndex[2] = {0};

/* Flag to mark whether CP PBK module is ready, according to CI Ind msg CI_PB_PRIM_PHONEBOOK_READY_IND */
static CiBoolean gModemPbkReady = FALSE;

#if 0
/* Global Mutex
 * NOTE: it is a recursive mutex
 */
static pthread_mutex_t gPbOperLock = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;

#define LOCK_PBCTX    do{ \
	pthread_mutex_lock(&gPbOperLock); \
	DBGMSG("[%s] Line(%d): gPbOperLock is locked\n", __FUNCTION__, __LINE__); \
}while(0)

#define UNLOCK_PBCTX  do{ \
	pthread_mutex_unlock(&gPbOperLock); \
	DBGMSG("[%s] Line(%d): gPbOperLock is unlocked\n", __FUNCTION__, __LINE__); \
}while(0)
#endif

/* Define some private primitives for PB service */
#define PRI_PB_PRIM_GET_STORAGE_REQ (CI_PB_PRIM_LAST_COMMON_PRIM + 1)
#define PRI_PB_PRIM_QUERY_READ_REQ (CI_PB_PRIM_LAST_COMMON_PRIM + 2)
#define PRI_PB_PRIM_READ_ENTRY_REQ (CI_PB_PRIM_LAST_COMMON_PRIM + 3)
#define PRI_PB_PRIM_QUERY_WRITE_REQ (CI_PB_PRIM_LAST_COMMON_PRIM + 4)
#define PRI_PB_PRIM_FIND_ENTRY_REQ (CI_PB_PRIM_LAST_COMMON_PRIM + 5)

#define READ_OPERATION 0
#define FIND_OPERATION 1

/************************************************************************************
 *
 * Internal library function
 *
 *************************************************************************************/

/************************************************************************************
 * F@: libPbStr2Enum - convert PB storage from string to enum value
 *
 */
static CiPhonebookId libPbStr2Enum(char * PbStr)
{
	INT32 i;

	for ( i = 1; i < CI_PHONEBOOK_NEXT; i++ )
	{
		if ( strcasecmp((char *)PbStr, gPbStorageStr[i]) == 0 )
		{
			break;
		}
	}

	return i;
}


/************************************************************************************
 *
 * Implementation of PBK related AT commands by sending Request to CCI. Called by Telpb.c
 *
 *************************************************************************************/

/****************************************************************************************
*  FUNCTION:  PB_QueryStorage
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to query supported PBK storage, implementation of AT+CPBS=?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode PB_QueryStorage(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPbPrimGetSupportedPhonebooksReq   *getSuppPbReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PB], CI_PB_PRIM_GET_SUPPORTED_PHONEBOOKS_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_PB_PRIM_GET_SUPPORTED_PHONEBOOKS_REQ), (void *)getSuppPbReq );


	return ret;
}

/****************************************************************************************
*  FUNCTION:  PB_GetInfo
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get current PBK info, used by AT+CPBS?, +CPBR=?, +CPBW=?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode PB_GetInfo(UINT32 atHandle, CiPrimitiveID priPrimId)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPbPrimGetPhonebookInfoReq   *getPbInfoReq;

	getPbInfoReq = utlCalloc(1, sizeof(CiPbPrimGetPhonebookInfoReq));
	if (getPbInfoReq == NULL)
		return CIRC_FAIL;

	getPbInfoReq->PbId = CI_PHONEBOOK_CURRENT;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PB], CI_PB_PRIM_GET_PHONEBOOK_INFO_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, priPrimId), (void *)getPbInfoReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  PB_GetStorage
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get current PBK storage, implementation of AT+CPBS?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode PB_GetStorage(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;

	ret = PB_GetInfo(atHandle, PRI_PB_PRIM_GET_STORAGE_REQ);

	return ret;
}


/****************************************************************************************
*  FUNCTION:  PB_SetStorage
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to set current PBK storage, implementation of AT+CPBS=<storage>[,<password>]
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode PB_SetStorage(UINT32 atHandle, char *pbStorageStr, char *pbPasswordStr, int pbPasswordStrLen)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPbPrimSelectPhonebookReq   *selectPbReq;

	selectPbReq = utlCalloc(1, sizeof(CiPbPrimSelectPhonebookReq));
	if (selectPbReq == NULL)
		return CIRC_FAIL;

	if ( (selectPbReq->PbId = libPbStr2Enum(pbStorageStr)) < CI_PHONEBOOK_NEXT )
	{
		if ( (pbPasswordStrLen > 0) && (pbPasswordStr != NULL) )
		{
			if(CI_MAX_PASSWORD_LENGTH - 1 < pbPasswordStrLen)
			{
				ERRMSG("%s: the length of %s is too long, exceed %d\n", __FUNCTION__, pbPasswordStrLen, CI_MAX_PASSWORD_LENGTH - 1);
				utlFree(selectPbReq);
				return ret;
			}
			selectPbReq->password.len = pbPasswordStrLen;
			strcpy((char*)(selectPbReq->password.data), pbPasswordStr);
		}
		else
		{
			selectPbReq->password.len = 0;
		}

		ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PB], CI_PB_PRIM_SELECT_PHONEBOOK_REQ,
				 MAKE_CI_REQ_HANDLE(atHandle, CI_PB_PRIM_SELECT_PHONEBOOK_REQ), (void *)selectPbReq );
	}
	else
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_ALLOWED, NULL);
		utlFree(selectPbReq);
	}

	return ret;
}

/****************************************************************************************
*  FUNCTION:  PB_QueryRead
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to query supported PBK read parameters, implementation of AT+CPBR=?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode PB_QueryRead(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;

	ret = PB_GetInfo(atHandle, PRI_PB_PRIM_QUERY_READ_REQ);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  PB_ReadOneEntry
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to read one PBK entry of specified index, implementation of AT+CPBR=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
static CiReturnCode PB_ReadOneEntry_(UINT32 reqHandle, int index)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPbPrimReadPhonebookEntryReq   *getPbEntryReq;

	getPbEntryReq = utlCalloc(1, sizeof(CiPbPrimReadPhonebookEntryReq));
	if (getPbEntryReq == NULL) {
		return CIRC_FAIL;
	}

	getPbEntryReq->Index = (UINT16)index;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PB], CI_PB_PRIM_READ_PHONEBOOK_ENTRY_REQ, reqHandle, (void *)getPbEntryReq );


	return ret;
}

CiReturnCode PB_ReadOneEntry(UINT32 atHandle, int index)
{
	UINT32 reqHandle = MAKE_CI_REQ_HANDLE(atHandle, PRI_PB_PRIM_READ_ENTRY_REQ);

	return PB_ReadOneEntry_(reqHandle, index);
}

/****************************************************************************************
 * interface to read one PBK entry of specified index, implementation of AT+CRSM=178,28474,... */
CiReturnCode PB_CRSM_ReadOneEntry(UINT32 atHandle, int index)
{
	UINT32 reqHandle = MAKE_CRSM_CI_REQ_HANDLE(atHandle, PRI_PB_PRIM_READ_ENTRY_REQ);

	return PB_ReadOneEntry_(reqHandle, index);
}

/****************************************************************************************
*  FUNCTION:  PB_ReadEntryStart
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to start to read PBK entry (one entry or entry between specified range, implementation of AT+CPBR=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode PB_ReadEntryStart(UINT32 atHandle, int indexStart, int indexEnd)
{
	CiReturnCode ret = CIRC_FAIL;

	/* Record index info, which will be usd when receiving CI CNF msg.	*/
	if (indexEnd > 0)
	{
		gCurrReadIndex[READ_OPERATION] = indexStart;
		gReadEndIndex[READ_OPERATION] = indexEnd;
	}
	else
	{
		gReadEndIndex[READ_OPERATION] = 0;
	}

	ret = PB_ReadOneEntry(atHandle, indexStart);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  PB_QueryWrite
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to query supported PBK read parameters, implementation of AT+CPBR=?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode PB_QueryWrite(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;

	/* NOTE: CI_PB_PRIM_GET_PHONEBOOK_INFO_REQ does not return all the necessary
	     information for AT+CPBW=? ... Maybe this info needs to be added to SAC first */
	ret = PB_GetInfo(atHandle, PRI_PB_PRIM_QUERY_WRITE_REQ);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  PB_DeleteEntry
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to delete one PBK entry of specified index, implementation of AT+CPBW=   (case when some para are omitted)
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
static CiReturnCode PB_DeleteEntry_(UINT32 reqHandle, int index)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPbPrimDeletePhonebookEntryReq   *delPbEntryReq;

	delPbEntryReq = utlCalloc(1, sizeof(CiPbPrimDeletePhonebookEntryReq));
	if (delPbEntryReq == NULL) {
		return CIRC_FAIL;
	}

	delPbEntryReq->Index = index;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PB], CI_PB_PRIM_DELETE_PHONEBOOK_ENTRY_REQ, reqHandle, (void *)delPbEntryReq );

	return ret;
}

CiReturnCode PB_DeleteEntry(UINT32 atHandle, int index)
{
	UINT32 reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_PB_PRIM_DELETE_PHONEBOOK_ENTRY_REQ);

	return PB_DeleteEntry_(reqHandle, index);
}

/****************************************************************************************
 * interface to delete one PBK entry of specified index, implementation of AT+CRSM=220,28474,... */
CiReturnCode PB_CRSM_DeleteEntry(UINT32 atHandle, int index)
{
	UINT32 reqHandle = MAKE_CRSM_CI_REQ_HANDLE(atHandle, CI_PB_PRIM_DELETE_PHONEBOOK_ENTRY_REQ);

	return PB_DeleteEntry_(reqHandle, index);
}

/****************************************************************************************
*  FUNCTION:  PB_WriteEntry
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to add/overwrite one PBK entry of specified index, implementation of AT+CPBW= [<index>][,<number>[,<type>[,<text>[,<group>[,<adnumber>[,<adtype>[,<secondtext>[,<email>[,<sip_uri>[,<tel_uri>[,<hidden>]]]]]]]]]]]
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
static CiReturnCode PB_WriteEntry_(UINT32 atHandle, bool bIsCrsm, int index, char *number, int numDigits, int type, char *text, int txtLen)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPbPrimAddPhonebookEntryReq     *addPbEntryReq;
	CiPbPrimReplacePhonebookEntryReq *replacePbEntryReq;
	CiPhonebookEntry PbEntry;
	UINT32 reqHandle;
	extern int libConvertHexToNum(char *inData, int inLen, char *outData);
	memset(&PbEntry, 0, sizeof(CiPhonebookEntry));
	/* Set value of entry name */
	if(!bIsCrsm)
	{
		PbEntry.Name.Length = pb_encode_character(text, txtLen, chset_type, (char *)PbEntry.Name.Name, CI_MAX_NAME_LENGTH);
	}
	else // CRSM
	{
		if (txtLen < CI_MAX_NAME_LENGTH)
		{
			PbEntry.Name.Length = txtLen;
			memcpy((void*)PbEntry.Name.Name, (void*)text, txtLen);
		}
		else
		{
			PbEntry.Name.Length = CI_MAX_NAME_LENGTH;
			memcpy((void*)PbEntry.Name.Name, (void*)text, CI_MAX_NAME_LENGTH);
		}
	}

	/* Set value of entry number */
	PbEntry.Number.AddrType.NumType = DialNumTypeGSM2CI(type);
	PbEntry.Number.AddrType.NumPlan = CI_NUMPLAN_E164_E163; /* Windows Mobile assigns this num. plan */

	if (numDigits < CI_MAX_ADDRESS_LENGTH)
	{
		PbEntry.Number.Length = numDigits;
		strcpy((char *)(PbEntry.Number.Digits), number);
	}
	else
	{
		PbEntry.Number.Length = CI_MAX_ADDRESS_LENGTH - 1;
		memcpy((void*)PbEntry.Number.Digits, (void*)number, CI_MAX_ADDRESS_LENGTH - 1);
		PbEntry.Number.Digits[CI_MAX_ADDRESS_LENGTH - 1] = '\0';
	}

	/* Build atHandle first according to index and bIsCrsm */
	if (bIsCrsm)
	{
		/* If index is valid, replace the phonebook entry */
		if (index > ATCI_PB_INVALID_INDEX )
		{
			reqHandle = MAKE_CRSM_CI_REQ_HANDLE(atHandle, CI_PB_PRIM_REPLACE_PHONEBOOK_ENTRY_REQ);
		}
		/*If index is invalid, add phonebook entry to the first available position */
		else
		{
			reqHandle = MAKE_CRSM_CI_REQ_HANDLE(atHandle, CI_PB_PRIM_ADD_PHONEBOOK_ENTRY_REQ);
		}
	}
	else
	{
		if (index > ATCI_PB_INVALID_INDEX )
		{
			reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_PB_PRIM_REPLACE_PHONEBOOK_ENTRY_REQ);
		}
		else
		{
			reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_PB_PRIM_ADD_PHONEBOOK_ENTRY_REQ);
		}
	}

	/* If index is valid, replace the phonebook entry */
	if (index > ATCI_PB_INVALID_INDEX )
	{
		replacePbEntryReq = utlCalloc(1, sizeof(CiPbPrimReplacePhonebookEntryReq));
		if (replacePbEntryReq == NULL) {
			return CIRC_FAIL;
		}

		replacePbEntryReq->Index = index;
		replacePbEntryReq->entry = PbEntry;
		ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PB], CI_PB_PRIM_REPLACE_PHONEBOOK_ENTRY_REQ, reqHandle, (void *)replacePbEntryReq );
	}

	/*If index is invalid, add phonebook entry to the first available position */
	else
	{
		addPbEntryReq = utlCalloc(1, sizeof(CiPbPrimAddPhonebookEntryReq));
		if (addPbEntryReq == NULL) {
                        return CIRC_FAIL;
		}

		addPbEntryReq->entry = PbEntry;
		ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PB], CI_PB_PRIM_ADD_PHONEBOOK_ENTRY_REQ, reqHandle, (void *)addPbEntryReq );
	}

	return ret;
}

CiReturnCode PB_WriteEntry(UINT32 atHandle, int index, char *number, int numDigits, int type, char *text, int txtLen)
{
	return PB_WriteEntry_(atHandle, FALSE, index, number, numDigits, type, text, txtLen);
}

/****************************************************************************************
 * interface to add/overwrite one PBK entry of specified index, implementation of AT+220,28474,...  */
CiReturnCode PB_CRSM_WriteEntry(UINT32 atHandle, int index, char *number, int numDigits, int type, char *text, int txtLen)
{
	return PB_WriteEntry_(atHandle, TRUE, index, number, numDigits, type, text, txtLen);
}

static CiReturnCode processFindnextPBEntry(UINT32 atHandle, int index)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPbPrimReadPhonebookEntryReq *pReadPB;
	UINT32 reqHandle = MAKE_CI_REQ_HANDLE(atHandle, PRI_PB_PRIM_FIND_ENTRY_REQ);

	pReadPB = utlCalloc(1, sizeof(CiPbPrimReadPhonebookEntryReq));
	if (pReadPB == NULL) {
		return CIRC_FAIL;
	}

	pReadPB->Index = index;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PB], CI_PB_PRIM_READ_PHONEBOOK_ENTRY_REQ,
			 reqHandle, (void *)pReadPB );

	return ret;
}
CiReturnCode PB_FindPBEntry(UINT32 atHandle, char *findtext, int LenFindText)
{
	/*Currently, CP doesn't support CI_PB_PRIM_FIND_FIRST_PHONEBOOK_ENTRY_REQ*/
	UNUSEDPARAM(findtext);
	UNUSEDPARAM(LenFindText);
	CiReturnCode ret = CIRC_FAIL;

	ret = PB_GetInfo(atHandle, PRI_PB_PRIM_FIND_ENTRY_REQ);

	return ret;

}
/************************************************************************************
 *
 * Implementation of processing reply or indication of SMS related AT commands. Called by msgCnf() or msgInd
 *
 *************************************************************************************/

/****************************************************************
 *  F@: libCheckPBRet - common func to give AT reply according to conf msg result
 */
static void libCheckPBRet( CiRequestHandle atHandle, CiPbResultCode result )
{
	switch (result)
	{
	case CIRC_PB_SUCCESS:
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	case CIRC_PB_NO_PHONEBOOK:
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, "No Phonebook");
		break;
	}

	case CIRC_PB_BAD_PHONEBOOK_ID:
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, "Bad PBK ID");
		break;
	}

	case CIRC_PB_SIM_FAILURE:
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_SIM_FAILURE, "SIM Failure");
		break;
	}

	case CIRC_PB_NO_MORE_ENTRIES:
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_MEMORY_FULL, "No more entries");
		break;
	}

	case CIRC_PB_NO_MORE_ROOM:
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_MEMORY_FULL, "No more room");
		break;
	}

	case CIRC_PB_DATA_UNAVAILABLE:
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, "Data unavailable");
		break;
	}

	case CIRC_PB_INVALID_OPERATION:
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_ALLOWED, "Invalid operation");
		break;
	}

	case CIRC_PB_BAD_DIALSTRING:
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_CHAR_IN_DIAL_STRING, "Bad dial string");
		break;
	}

	case CIRC_PB_FDN_OPER_NOT_ALLOWED:
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_ALLOWED, "FDN operation not allowed");
		break;
	}

	case CIRC_PB_FAIL:
	default:
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		break;
	}
	}
}
static void libCheckPBWRet( CiRequestHandle atHandle, CiPbResultCode result, UINT16 index)
{
	char AtRspBuf [500];
	if(result == CIRC_PB_SUCCESS)
	{
		sprintf(AtRspBuf, "+CPBW:%d", index);
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, AtRspBuf);
	}
	else
	{
		libCheckPBRet( atHandle, result );
	}
}

/****************************************************************
 *  F@: processQuerySmsServiceConf - Process CI cnf msg of AT+CPBS=?
 */
static void processQueryStorageConf(UINT32 atHandle, const void *paras)
{
	char AtRspBuf [500], TempBuf[300];
	INT32 i;
	CiPbPrimGetSupportedPhonebooksCnf   *getSuppPbCnf = (CiPbPrimGetSupportedPhonebooksCnf*)paras;

	if ( getSuppPbCnf->Result == CIRC_PB_SUCCESS )
	{
		sprintf( (char *)AtRspBuf, "+CPBS:(");
		for ( i = 0; i < getSuppPbCnf->NumPb - 1; i++ )
		{
			sprintf( TempBuf, "\"%s\",", gPbStorageStr[getSuppPbCnf->SuppPb[i]] );
			strcat( AtRspBuf, TempBuf );
		}
		sprintf( TempBuf, "\"%s\")", gPbStorageStr[getSuppPbCnf->SuppPb[i]] );
		strcat( AtRspBuf, TempBuf );
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, AtRspBuf );
	}

	/* Conf Result is not CIRC_PB_SUCCESS */
	else
	{
		libCheckPBRet(atHandle, getSuppPbCnf->Result);
	}

	return;
}

/****************************************************************
 *  F@: processGetPbInfoConf - Process CI cnf msg of  AT+CPBS?, +CPBR=?, +CPBW=?
 */
static void processGetPbInfoConf(UINT32 atHandle, const void *paras, CiRequestHandle reqHandle)
{
	char AtRspBuf [500], TempBuf[300];
	CiPbPrimGetPhonebookInfoCnf  *getPbInfoCnf = (CiPbPrimGetPhonebookInfoCnf*)paras;

	if ( getPbInfoCnf->Result == CIRC_PB_SUCCESS )
	{
		/* Response of +CPBS? , print phonebook, used and total entries */
		if ( GET_REQ_ID(reqHandle) == PRI_PB_PRIM_GET_STORAGE_REQ)
		{
			sprintf( AtRspBuf, "+CPBS:\"%s\",%d,%d", gPbStorageStr[getPbInfoCnf->info.PbId], getPbInfoCnf->info.UsedEntries, getPbInfoCnf->info.TotalEntries);
			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, AtRspBuf );
		}

		/* Response of +CPBR=?, +CPBR: (list of supported <index>s),[<nlength>] */
		else if ( GET_REQ_ID(reqHandle) == PRI_PB_PRIM_QUERY_READ_REQ)
		{
			sprintf( AtRspBuf, "+CPBR:(" );
			if ( getPbInfoCnf->info.TotalEntries != 0 ) /* make sure we don't print "1" when no indices available */
			{
				sprintf( TempBuf, "1-%d", getPbInfoCnf->info.TotalEntries );
			}
			else
			{
				sprintf( TempBuf, "0" );
			}
			strcat( AtRspBuf, TempBuf );

			/* Print MAX dial string length - if available */
			sprintf( TempBuf, "),%d", CI_MAX_ADDRESS_LENGTH );
			strcat( AtRspBuf, TempBuf );

			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, AtRspBuf );
		}

		/* Response of +CPBW=?, +CPBW: (list of supported <index>s),[<nlength>],(list of supported <type>s) */
		else if ( GET_REQ_ID(reqHandle) == PRI_PB_PRIM_QUERY_WRITE_REQ)
		{
			sprintf( AtRspBuf, "+CPBW:(" );
			if ( getPbInfoCnf->info.TotalEntries != 0 ) /* make sure we don't print "1" when no indices available */
			{
				sprintf( TempBuf, "1-%d", getPbInfoCnf->info.TotalEntries );
			}
			else
			{
				sprintf( TempBuf, "0" );
			}
			strcat( AtRspBuf, TempBuf );

			/* Print MAX dial string length - if available */
			sprintf( TempBuf, "),%d,", CI_MAX_ADDRESS_LENGTH );
			strcat( AtRspBuf, TempBuf );

			/* Print list of supported phone types (taken from enum ATCI_DIAL_NUMBER_TYPE) */
			strcat( AtRspBuf, (char *)"(129,145)" );

			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, AtRspBuf );
		}
		else if (GET_REQ_ID(reqHandle) == PRI_PB_PRIM_FIND_ENTRY_REQ)
		{
			if (getPbInfoCnf->info.UsedEntries > 0)
			{
				ATRESP( atHandle, ATCI_RESULT_CODE_NULL, 0, NULL);
				gReadEndIndex[FIND_OPERATION] = getPbInfoCnf->info.TotalEntries;
				gCurrReadIndex[FIND_OPERATION] = 1;
				processFindnextPBEntry(atHandle, gCurrReadIndex[FIND_OPERATION]);
			}
			else
			{
				ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
			}
		}
		else
		{
			ATRESP( atHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
		}
	}
	/* Conf Result is not CIRC_PB_SUCCESS */
	else
	{
		libCheckPBRet(atHandle, getPbInfoCnf->Result);
	}

	return;
}

/****************************************************************
 *  F@: processReadEntryConf - Process CI cnf msg of AT+CPBR: <index1>,<number>,<type>,<text>
 */
static void processReadEntryConf(UINT32 atHandle, const void *paras, CiRequestHandle reqHandle)
{
	char AtRspBuf [500], TempBuf[300];
	CiPbPrimReadPhonebookEntryCnf   *readEntryCnf = (CiPbPrimReadPhonebookEntryCnf*)paras;
	int isFindReq;
	if (GET_REQ_ID(reqHandle) != PRI_PB_PRIM_FIND_ENTRY_REQ)
		isFindReq = READ_OPERATION;
	else
		isFindReq = FIND_OPERATION;
	if ( readEntryCnf->Result == CIRC_PB_SUCCESS )
	{
		const char *plus = NULL;
		if ( readEntryCnf->entry.Number.AddrType.NumType == CI_NUMTYPE_INTERNATIONAL )
			plus = "+";
		else
			plus = "";

		memcpy( TempBuf, readEntryCnf->entry.Number.Digits, readEntryCnf->entry.Number.Length );
		TempBuf[ readEntryCnf->entry.Number.Length ] = '\0';

		if (isFindReq != FIND_OPERATION)
		{
			sprintf( AtRspBuf, "+CPBR:%d,\"%s%s\",", readEntryCnf->Index, plus, TempBuf );
		}
		else
		{
			sprintf( AtRspBuf, "+CPBF:%d,\"%s%s\",", readEntryCnf->Index, plus, TempBuf );
		}
		//	DBGMSG("Digits len: %d, str: %s.\n", readEntryCnf->entry.Number.Length, readEntryCnf->entry.Number.Digits);
		//    DBGMSG("readEntryCnf->entry.Number.AddrType.NumType = %d.\n", readEntryCnf->entry.Number.AddrType.NumType);
		switch ( readEntryCnf->entry.Number.AddrType.NumType )
		{
		case CI_NUMTYPE_INTERNATIONAL:
			strcat( AtRspBuf, "145,\"" );
			break;

		case CI_NUMTYPE_NATIONAL:
		case CI_NUMTYPE_NETWORK:
		case CI_NUMTYPE_DEDICATED:
		case CI_NUMTYPE_EXTENSION:
		case CI_NUMTYPE_UNKNOWN:
			strcat( AtRspBuf, "129,\"" );
			break;

		default:
			/* Will implement other types as needed */
			ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
			return;
		}
		
		TempBuf[0] = '\0';
		int ret = pb_decode_character((char *)readEntryCnf->entry.Name.Name, readEntryCnf->entry.Name.Length, TempBuf, sizeof(TempBuf) / sizeof(TempBuf[0]) - 1, chset_type);
		TempBuf[ret] = '\0';
		strcat( AtRspBuf, TempBuf );

		strcat( AtRspBuf, "\"" );
		if (isFindReq != FIND_OPERATION)
		{
			ATRESP( atHandle, ATCI_RESULT_CODE_NULL, 0, AtRspBuf );
		}
		else
		{
			unsigned short NameBuf[CI_MAX_NAME_LENGTH + 1];
			int len = 0;
			pb_decode_alpha_tag(readEntryCnf->entry.Name.Name, readEntryCnf->entry.Name.Length, NameBuf, sizeof(NameBuf) / sizeof(NameBuf[0]) - 1);
			while(g_pbEntryStr[len]) ++len;
			if(memcmp(g_pbEntryStr, NameBuf, len * sizeof(unsigned short)) == 0)
				ATRESP( atHandle, ATCI_RESULT_CODE_NULL, 0, AtRspBuf );
			else
				ATRESP( atHandle, ATCI_RESULT_CODE_NULL, 0, NULL );
		}
	}
	/* Case when Conf Result is not CIRC_PB_SUCCESS */
	else
	{
		/* Report to upper layer when +CPBR=<n> (read only one entry). If read multi-entry, skip this failed entry */
		if (gReadEndIndex[isFindReq] == 0)
			libCheckPBRet(atHandle, readEntryCnf->Result);
	}
	/* Consider next step: Check for global flag whether we need to read the next entry */
	/* +CPBR=<n>,  finish after reading one entry */
	if ( gReadEndIndex[isFindReq] == 0 )
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
	}
	/* +CPBR=<n1>,<n2>, try to read next entry */
	else
	{
		/* Send the next request */
		if ( gCurrReadIndex[isFindReq] < gReadEndIndex[isFindReq] )
		{
			gCurrReadIndex[isFindReq]++;
			if (isFindReq != FIND_OPERATION)
				PB_ReadOneEntry(atHandle, gCurrReadIndex[isFindReq]);
			else
				processFindnextPBEntry(atHandle, gCurrReadIndex[isFindReq]);
		}
		/* Range read finished */
		else
		{
			gCurrReadIndex[isFindReq] = 0;
			gReadEndIndex[isFindReq] = 0;
			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
	}
	return;
}

/****************************************************************************************
*
*  DESCRIPTION: Callback function when receiving confirmation CI message related with PBK
*
****************************************************************************************/

void pbCnf(CiSgOpaqueHandle opSgCnfHandle,
	   CiServiceGroupID svgId,
	   CiPrimitiveID primId,
	   CiRequestHandle reqHandle,
	   void*            paras)
{
	UNUSEDPARAM(opSgCnfHandle)

	/* By default, the gCurrOperation will be set as AT_MSG_OPERATION_NULL unless this flag is set as FALSE manually */
	BOOL bProcessed = FALSE;
	UINT32 atHandle = GET_AT_HANDLE(reqHandle);

	/* Validate the para pointer */
	if ( (paras == NULL) && (primId < CI_PB_PRIM_LAST_COMMON_PRIM) )
	{
		ERRMSG("para pointer of CI cnf msg is NULL!");
		return;
	}

#if 0
	{
		int i;
		char *p = paras;
		printf("\n#####[Enter %s][Prim=%d,gCurrOperation=%d] Data: ", __FUNCTION__, primId, gCurrOperation);
		for (i = 0; i < 30; i++)
			printf("%02x", *p++);
		printf("\n");
	}
#endif

	if (GET_CRSM_FLAG(reqHandle) == TRUE)
	{
		bProcessed = SIM_ConvertCrsmAtReply(atHandle, primId, paras);
	}

	if (!bProcessed)
	{
		switch (primId)
		{
		case CI_PB_PRIM_GET_SUPPORTED_PHONEBOOKS_CNF:
		{
			processQueryStorageConf(atHandle, paras);
			break;
		}

		case CI_PB_PRIM_GET_PHONEBOOK_INFO_CNF:
		{
			processGetPbInfoConf(atHandle, paras, reqHandle);
			break;
		}

		case CI_PB_PRIM_SELECT_PHONEBOOK_CNF:
		{
			CiPbPrimSelectPhonebookCnf          *selectPBCnf;
			selectPBCnf = (CiPbPrimSelectPhonebookCnf*)paras;
			libCheckPBRet( atHandle, selectPBCnf->Result );
			break;
		}

		case CI_PB_PRIM_REPLACE_PHONEBOOK_ENTRY_CNF:
		{
			CiPbPrimReplacePhonebookEntryCnf    *replaceEntryCnf;
			replaceEntryCnf = (CiPbPrimReplacePhonebookEntryCnf*)paras;
			libCheckPBWRet( atHandle, replaceEntryCnf->Result, replaceEntryCnf->Index);
			break;
		}

		case CI_PB_PRIM_ADD_PHONEBOOK_ENTRY_CNF:
		{
			CiPbPrimAddPhonebookEntryCnf        *addEntryCnf;
			addEntryCnf = (CiPbPrimAddPhonebookEntryCnf*)paras;
			libCheckPBWRet( atHandle, addEntryCnf->Result, addEntryCnf->Index);
			break;
		}

		case CI_PB_PRIM_DELETE_PHONEBOOK_ENTRY_CNF:
		{
			CiPbPrimDeletePhonebookEntryCnf     *deleteEntryCnf;
			deleteEntryCnf = (CiPbPrimDeletePhonebookEntryCnf*)paras;
			libCheckPBWRet( atHandle, deleteEntryCnf->Result, PBdelindex);
			break;
		}

		case CI_PB_PRIM_READ_PHONEBOOK_ENTRY_CNF:
		{
			processReadEntryConf(atHandle, paras, reqHandle);
			break;
		}

		case CI_ERR_PRIM_HASNOSUPPORT_CNF:
		case CI_ERR_PRIM_HASINVALIDPARAS_CNF:
		case CI_ERR_PRIM_ISINVALIDREQUEST_CNF:
		{
			/* Print "Operation not suported" error (CME error 4) */
			ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
			break;
		}
		case CI_PB_PRIM_FIND_FIRST_PHONEBOOK_ENTRY_CNF:
		{
			CiPbPrimFindFirstPhonebookEntryCnf *pFind1stPBEntryCnf;
			pFind1stPBEntryCnf = (CiPbPrimFindFirstPhonebookEntryCnf*)paras;
			char atRsfBuf[100];
			char nameBuf[CI_MAX_NAME_LENGTH + ATCI_NULL_TERMINATOR_LENGTH];
			char numberBuf[CI_MAX_ADDRESS_LENGTH + ATCI_NULL_TERMINATOR_LENGTH];
			if (pFind1stPBEntryCnf->Result == CIRC_PB_SUCCESS)
			{
				memcpy( nameBuf, pFind1stPBEntryCnf->entry.Name.Name, pFind1stPBEntryCnf->entry.Name.Length );
				nameBuf[ pFind1stPBEntryCnf->entry.Name.Length ] = '\0';
				memcpy( numberBuf, pFind1stPBEntryCnf->entry.Number.Digits, pFind1stPBEntryCnf->entry.Number.Length );
				numberBuf[ pFind1stPBEntryCnf->entry.Number.Length ] = '\0';
				sprintf(atRsfBuf, "+CPBF:%d,\"%s\",\"%s\"", pFind1stPBEntryCnf->Index,
					nameBuf,
					numberBuf);
				ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, (char *)atRsfBuf);
			}
			else
			{
				ATRESP(atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_NOT_FOUND, NULL);
			}
			break;
		}
		case CI_PB_PRIM_FIND_NEXT_PHONEBOOK_ENTRY_CNF:
		{
			break;
		}
		case CI_PB_PRIM_READ_ENH_PBK_ENTRY_CNF:
		case CI_PB_PRIM_EDIT_ENH_PBK_ENTRY_CNF:
		case CI_PB_PRIM_READ_CATEGORY_NAME_CNF:
		case CI_PB_PRIM_EDIT_CATEGORY_NAME_CNF:
		case CI_PB_PRIM_READ_GROUP_NAME_CNF:
		case CI_PB_PRIM_EDIT_GROUP_NAME_CNF:
			break;

		default:
			break;
		}
	}

	/* free up the confirmation memory */
	atciSvgFreeCnfMem( svgId, primId, paras );

	return;
}

/****************************************************************
 *  F@: processPbkReadyInd - Process CI ind msg of CI_PB_PRIM_PHONEBOOK_READY_IND
 */
static void processPbkReadyInd(const void *pParam)
{
	char AtIndBuf [500];
	CiPbPrimPhoneBookReadyInd  *pbReadyInd;

	pbReadyInd = (CiPbPrimPhoneBookReadyInd *)pParam;
	gModemPbkReady = pbReadyInd->PbReady;

	sprintf(AtIndBuf, "+MPBK: %d", gModemPbkReady == TRUE ? 1 : 0);

	ATRESP( IND_REQ_HANDLE, 0, 0, (char *)AtIndBuf );

	return;
}

/****************************************************************************************
*
*  DESCRIPTION: Callback function when receiving indication CI message related with PBK
*
****************************************************************************************/
void pbInd(CiSgOpaqueHandle opSgOpaqueHandle,
	   CiServiceGroupID svgId,
	   CiPrimitiveID primId,
	   CiIndicationHandle indHandle,
	   void*               pParam)
{
	UNUSEDPARAM(opSgOpaqueHandle)
	UNUSEDPARAM(svgId)
	UNUSEDPARAM(indHandle)
	UNUSEDPARAM(pParam)

	if (primId == CI_PB_PRIM_PHONEBOOK_READY_IND)
	{
		processPbkReadyInd(pParam);
	}
	else
	{
		ERRMSG("Invalid PB indication:%d\r\n", primId);
	}

	/* Free up the indication memory */
	atciSvgFreeIndMem( svgId, primId, pParam );

	return;
}

void resetPbParas(void)
{
	int i;
	for(i = 0; i < 2; i++)
	{
		gCurrReadIndex[i] = 0;
		gReadEndIndex[i] = 0;
	}
	gModemPbkReady = FALSE;
}


