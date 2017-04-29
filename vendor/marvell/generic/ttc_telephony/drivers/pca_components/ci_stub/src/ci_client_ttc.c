#include "ci_api_types.h"
#include "ci_api.h"
#include "ci_stub_ttc.h"
#include "osa.h"
#include "ci_mem.h"
#include "ci_trace.h"
#include "ci_task.h"
#include "ci_trace.h"
#include "ci_msg.h"
#include "ci_pb.h"

#define  CI_DATA_DUMP

#ifdef CI_DATA_DUMP
#define CIPRINTF(...) eeh_log(LOG_DEBUG, __VA_ARGS__)
#define CIPRINTF_1(...) eeh_log(LOG_DEBUG, __VA_ARGS__)
void ciDumpMsgInfo(UINT8 srvGrpId, UINT16 primId, UINT32 reqHandle,  UINT8 *data, UINT32 dataSize);
#endif
#define EXT2_READ_OPER    CI_SIM_PRIM_LAST_COMMON_PRIM + 1
#define EXT2_UPDATE_OPER  CI_SIM_PRIM_LAST_COMMON_PRIM + 2
static CiShConfirm gCiShConfirmFuncPtr;
static CiShOpaqueHandle gCiShOpaqueHandle;
static CiConfirm gCiDefConfirmFuncPtr;
static CiIndicate gCiDefIndicateFuncPtr;
static CiConfirm gCiConfirmFuncPtr[CI_SG_ID_NEXTAVAIL];
static CiIndicate gCiIndicateFuncPtr[CI_SG_ID_NEXTAVAIL];
CiSgOpaqueHandle gOpSgCnfHandle[CI_SG_ID_NEXTAVAIL];
CiSgOpaqueHandle gOpSgIndHandle[CI_SG_ID_NEXTAVAIL];
static CiSgOpaqueHandle gCiSgOpaqueReqHandle[CI_SG_ID_NEXTAVAIL];
static CiSgOpaqueHandle gCiSgOpaqueRspHandle[CI_SG_ID_NEXTAVAIL];
static CiSgFreeReqMem gCiSgFreeReqMemFuncTable[CI_SG_ID_NEXTAVAIL];
static CiSgFreeReqMem gCiSgFreeRspMemFuncTable[CI_SG_ID_NEXTAVAIL];
static CiServiceHandle gCiServiceHandle[CI_SG_ID_NEXTAVAIL];

extern OSASemaRef semaRef;  /* for Semaphore */

CrsmCciAction 			*head = NULL;
CiServiceHandle 	  	ciATciSvgHandle[CI_SG_NUMIDS+1];
/*Michal Bukai*/
UINT8 gPBMaxAlphaIdLength = 14;

extern void ciSendMsgToServer(int procId, unsigned char *msg, int msglen);
extern UINT32 cimem_GetCiShReqDataSize(CiShOper oper);
extern UINT32 cimem_GetCiPrimDataSize(CiServiceHandle handle, CiPrimitiveID primId, void *paras);
extern CiServiceGroupID findCiSgId(CiServiceHandle handle);
extern void string2BCD(UINT8* pp, char *tempBuf, UINT8 length);
typedef enum PB_RECORD_STATUS {
  PB_RECORD_EMPTY = 0,
  PB_RECORD_TABLE,
  PB_RECORD_UPDATE,
} _ciPbRecordStatus;

typedef UINT8 ciPbRecordStatus;
typedef UINT16 ciPbRecordIndex;

typedef struct ciFDNEntryWithExt{
	ciPbRecordStatus	status;					//record update status
	ciPbRecordIndex		FDNIndex;				// FDN PB entry index
	ciPbRecordIndex		ExtIndexReq;			// EXT2 index requested by App 
} ciFDNEntryWithExt;

typedef struct ciEXT2Entry{
	ciPbRecordStatus	status;				//record update status
	ciPbRecordIndex		ExtIndexReq;		// EXT2 index requested by App 
	UINT8   			pRecord[13];			// EXT2 data
} ciEXT2Entry;

static ciEXT2Entry gEXT2Table[100];
static ciFDNEntryWithExt gFDNWithExtTable[100];

/*Michal bukai - more Vritual SIM handler functions - START*/
UINT8 ReadSemiOctetData (UINT8 octetPosition, UINT8 *numberList,
                   UINT8 maxDigits, UINT8 *basePtr, BOOL checkForPad)
{
    UINT8    i                   = 0;
    UINT8    octetPos            = octetPosition;
    BOOL     loSemiOctet         = TRUE;
    BOOL     moreDigitsToRead    = TRUE;

    while (moreDigitsToRead)
    {
        if (loSemiOctet)
        {
            /* process a semi octet in the low nybble position */
            numberList[i] = basePtr[octetPos] & 0x0F;
			loSemiOctet = FALSE;
        }
        else
        {
            /* process a semi octet in the high nybble position */
            numberList[i] = (basePtr[octetPos] & 0xF0) >> 4;
			loSemiOctet = TRUE;
            octetPos++;
        }

		if(numberList[i] < 0x0a)
		{
			numberList[i] += 0x30;
		}
		else
		{
			if(numberList[i] == 0x0a)/*bcd value 0xa will be translate to '*' charecter */
			{
				numberList[i] = 0x2a;
			}
			else if(numberList[i] == 0x0b)/*bcd value 0xb will be translate to '#' charecter */
			{
				numberList[i] = 0x23;
			}
			else if(numberList[i] == 0x0c)/*bcd value 0xc will be translate to 'p' charecter - DTMF */
			{
				numberList[i] = 0x70;
			}
			else if(numberList[i] == 0x0d)/*bcd value 0xd will be translate to '?' charecter - Wild */
			{
				numberList[i] = 0x3f;
			}
			else if(numberList[i] == 0x0F )
			{
				if(i + 1 == maxDigits)
                {
					numberList[i] = 0x00;
					return i;
				}
				else
					return 0;
			}
			else
				return 0;

		}

        if (((checkForPad == TRUE) && (numberList[i] == 0xF)) ||
             (i + 1 == maxDigits))
        {
            moreDigitsToRead = FALSE;
        }
        else
        {
            i++;
        }
    }

    /* return actual number of digits read */
    return (i+1);
}

UINT8 ReadAddressInfo (CiAddressInfo *addr, UINT8 *basePtr)
{
    UINT8   octetPos = 0;
    UINT8   ieLength;
    UINT8   octetsRead;
    UINT8   maxDigits;

	ieLength = basePtr[octetPos];
	octetPos++;

    addr->AddrType.NumType = (UINT8) ((basePtr[octetPos] >> 4) & 0x07);
  	addr->AddrType.NumPlan = (UINT8) (basePtr[octetPos] & 0x0f);
	octetPos++;

	maxDigits = (ieLength - 1) * 2;

	addr->Length = ReadSemiOctetData (octetPos,(UINT8*) &addr->Digits[0], maxDigits, basePtr, TRUE);
	if(addr->Length == 0)
	{
		return 0;
	}
	octetsRead = (ieLength + 1);

	return (octetsRead);
}

UINT8 ReadMoreAddressInfo (CiAddressInfo *addr, UINT8 *basePtr)
{
	UINT8   octetPos = 0, len = 0;
    UINT8   ieLength;
    UINT8   octetsRead;
    UINT8   maxDigits = 22;

	ieLength = basePtr[octetPos];
	if(ieLength < 12)
	{
		maxDigits = ieLength* 2;
	}
	if(addr->Length > CI_MAX_ADDRESS_LENGTH - maxDigits)
	{
		maxDigits = CI_MAX_ADDRESS_LENGTH - addr->Length;
	}
	octetPos++;

	len = ReadSemiOctetData (octetPos,(UINT8*) &addr->Digits[addr->Length], maxDigits, basePtr, TRUE);
	addr->Length += len;
	octetsRead = ieLength;
	return (octetsRead);
}

/*Michal bukai - more Vritual SIM handler functions - END*/
#ifndef CCI_SIM_CONTIGUOUS

	#define SET_SIM_CMD_LEN_AND_TERM(__CnfPtr,__Len)\
			(__CnfPtr)->pCnf->len=(__Len);\
			(__CnfPtr)->pCnf->pData[(__Len)-2]=144;\
			(__CnfPtr)->pCnf->pData[(__Len)-1]=0

#else

	#define SET_SIM_CMD_LEN_AND_TERM(__CnfPtr,__Len)\
			(__CnfPtr)->cnf.len=(__Len);\
			(__CnfPtr)->cnf.data[(__Len)-2]=144;\
			(__CnfPtr)->cnf.data[(__Len)-1]=0
#endif




void Bcd2String(UINT8 * bcd, UINT8* len)
{
	char *str;
	UINT8 cnt = 0;
	UINT8 value = 0;
	str = (char*)CI_MEM_ALLOC((*len)*2);
   if(bcd != NULL &&  (*len) > 0)
    {
        for ( cnt = 0; cnt < (*len) * 2; cnt++ )
        {
            if(cnt%2 == 0)
            {
                value = (bcd[cnt/2]&0xf0)>>4;
            }
            else
            {
                value = bcd[cnt/2]&0xf;
            }

            if(value < 10)
            {
			if(cnt%2 == 0)
				str[cnt+1] = 0x30 + value;
			else
				str[cnt-1] = 0x30 + value;
            }
            else if (value == 15 && cnt < (*len)*2-1)
            	{
            		continue;
            	}
	     else
             {
               	break;
             }
        }
    }
   memcpy((void*)bcd,(void*)str,cnt);
   *len = cnt;
   CI_MEM_FREE(str);
}

void string2Number(UINT8* srcNum,UINT8* len)
{
	char *temp;
	UINT8 i,j;
	temp = (char*)CI_MEM_ALLOC((*len)*2+1);
	for (i=0,j=0;i<(*len);i++,j++)
		{
			temp[j+1] = (srcNum[i]&0x0F)+ '0';
			temp[j] = ((srcNum[i]&0xF0)>>4)+'0';
			j++;
		}
	memcpy((void*)srcNum,(void*)temp,j+1);
	(*len) = j+1;
	CI_MEM_FREE(temp);
}

void getString(UINT8* dest,UINT8* src,UINT8* len)
{
	UINT8 i = 0;
	while(src[i]!=EMPTY)
	{
		dest[i] = src[i];
		i++;
	}
	*len = i;
}
void judgeOp(UINT8* pData,UINT8 *i,UINT8 j)
{
	UINT8 temp;
	temp=*i;
	while(temp<j)
	{
		if (pData[temp]==EMPTY)
		temp++;
		else
			break;
	}
	*i=temp;
}

void CiSetSvgGroupHandle(UINT8 svgId, UINT32 shcnfhandler)
{
	ciATciSvgHandle[svgId] = shcnfhandler;
}// it needs to notify Borqs.

int handleReturn(UINT16 ret)
{
	if ( ret == 0 )
	{
		return CIRC_SUCCESS;
	}
	else
	{
		/* When CI Request is falied */;
		return CIRC_FAIL;
	}
}

void clearCciActionlist(void)
{
	CrsmCciAction *ptemp = head;
	OSASemaphoreAcquire(semaRef, OSA_SUSPEND);
	while(head != NULL)
	{
		head = head->next;
		CI_MEM_FREE(ptemp->ci_request);
		CI_MEM_FREE(ptemp);
		ptemp = head;
	}
	OSASemaphoreRelease(semaRef);
}
void freeCciActionlist(CrsmCciAction *tem)
{
	CCI_ASSERT(tem != NULL);
	CCI_ASSERT(head != NULL);
	CrsmCciAction *ptemp = head;
	
	OSASemaphoreAcquire(semaRef, OSA_SUSPEND);

	if (head == tem)
		head =tem->next;
	else
	{
		while(ptemp!=NULL && ptemp->next!=tem)
			ptemp = ptemp ->next;
		if (ptemp!=NULL)
		ptemp->next = tem->next;
	}
	CI_MEM_FREE(tem->ci_request);
	CI_MEM_FREE(tem);
	
	OSASemaphoreRelease(semaRef);
}
void add2CciActionlist(CrsmCciAction *tem)
{
	CrsmCciAction *temp = head;

	// take semaphore,
	OSASemaphoreAcquire(semaRef, OSA_SUSPEND);

	
	tem->next = NULL;
	if (head == NULL)
	{
		head = tem;
	}
	else
	{
		while(temp->next!=NULL)
		{
			temp = temp->next;
		}
		temp->next = tem;
	}

	OSASemaphoreRelease(semaRef);
}
CiReturnCode GetRecordIndex(UINT8 p1, UINT8 p2)
{
	switch (p2)
	{
	case 3:
		if(0==p1)
			p1 = LAST_RECORD;
		if(FIRST_RECORD==p1)
			p1 = INVALID_INDEX;
		if (0!=p1&&FIRST_RECORD!=p1)
		break;
	case 2:
		if (0 == p1)
			p1 = 1;
		if (LAST_RECORD == p1)
			p1 = INVALID_INDEX;
		if (0!=p1&&LAST_RECORD!=p1)
		    p1 = p1+1;
		break;
	default:
		break;
	}
	return p1;
}

int GetSIMEfFileId(CiSimFilePath path)
{
	int iFileId = 0;
	int temp = 0;
	UINT8	tmpByte;
	int i;

#if 0
	if(path.len>0)
		{
		if (((path.val[path.len-1]& 0x0f)<=0x0f)&&((path.val[path.len-1]& 0x0f)>=0x00))
			{
				temp= (path.val[path.len-1]& 0x0f) +0;
				iFileId+=temp;
			}
		if(((path.val[path.len-1]>>4) < 0x0f) && ((path.val[path.len-1] >> 4) >= 0x00))
			{
				temp = (path.val[path.len-1] >> 4) + 0;
				temp = temp*16;
				iFileId += temp;
			}
		if (((path.val[path.len-2]& 0x0f)<=0x0f)&&((path.val[path.len-2]& 0x0f)>=0x00))
			{
				temp = (path.val[path.len-2]& 0x0f) +0;
				temp = temp*256;
				iFileId+=temp;
			}
		if(((path.val[path.len-2]>>4) < 0x0f) && ((path.val[path.len-2] >> 4) >= 0x00))
			{
				temp = (path.val[path.len-2] >> 4) + 0;
				temp = temp*4096;
				iFileId += temp;
			}
		}
#else
	for (i=0;i<2; i++) {
		if(path.len>i)
		{
#ifdef CCI_SIM_CONTIGUOUS
			tmpByte=path.valStr[path.len-1-i];
#else
			tmpByte=path.val[path.len-1-i];
#endif

			temp= (tmpByte& 0x0f) +0;
			if (i)
				temp=temp*256;
			iFileId+=temp;
			if((tmpByte>>4) < 0x0f)
			{
					temp = (tmpByte >> 4) + 0;
					temp = temp*16;
					if (i)
						temp=temp*256;
					iFileId += temp;
			}
		}
	}
#endif
	return iFileId;
}



CiReturnCode  ciShRegisterReq_1(CiShConfirm ciShConfirm,
				CiShOpaqueHandle opShHandle)

{
	//CiApiMessage msg;
	//ShmApiMsg *pShmArgs;
	CiShRegisterReqArgs *pProcArgs;
	unsigned char *pShmBuf;
	int shmBufLen;

	CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_API, "ciShRegisterReq_1()");

	if (gDlLinkDownFlag == TRUE)
	{
		CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_API, "ciShRegisterReq_1: CIRC_INTERLINK_FAIL");
		return CIRC_INTERLINK_FAIL;
	}

	/* register user defined confirm callback  Fn */
	gCiShConfirmFuncPtr = ciShConfirm;
	gCiShOpaqueHandle = opShHandle;


	/* Assemble Shm Buf Message */

	shmBufLen = SHM_HEADER_SIZE + sizeof(CiShRegisterReqArgs);
	pShmBuf = CI_MEM_ALLOC(shmBufLen);
	pProcArgs = (CiShRegisterReqArgs*)(pShmBuf + SHM_HEADER_SIZE);

//  pProcArgs->ciShConfirm = ciShConfirm;
//  pProcArgs->opShHandle = opShHandle;
//  pProcArgs->ciShFreeReqMem = ciShFreeReqMem;
	pProcArgs->ciShOpaqueHandle = opShHandle;

	ciSendMsgToServer(CiShRegisterReqProcId, pShmBuf,  sizeof(CiShRegisterReqArgs));

	CI_MEM_FREE(pShmBuf);

	return CIRC_SUCCESS;
}


CiReturnCode  ciShDeregisterReq_1 (CiShHandle handle, CiShOpaqueHandle opShHandle)
{
	CiShDeregisterReqArgs *pProcArgs;
	unsigned char *pShmBuf;
	int shmBufLen;

	CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_API, "ciShDeregisterReq()");
	if (gDlLinkDownFlag == TRUE)
	{
		CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_API, "ciShDeregisterReq_1: CIRC_INTERLINK_FAIL");
		return CIRC_INTERLINK_FAIL;
	}


	shmBufLen = SHM_HEADER_SIZE + sizeof(CiShDeregisterReqArgs);
	pShmBuf = CI_MEM_ALLOC(shmBufLen);
	pProcArgs = (CiShDeregisterReqArgs *)(pShmBuf + SHM_HEADER_SIZE);

	pProcArgs->handle = handle;
	pProcArgs->opShHandle = opShHandle;

	ciSendMsgToServer(CiShDeregisterReqProcId, pShmBuf, sizeof(CiShDeregisterReqArgs));

	CI_MEM_FREE(pShmBuf);

	return CIRC_SUCCESS;
}


/* service group Free memory call-back that is used by the CI Server task to service group to free memory allocated
   by the server service group for the indicate service primitive */
static void clientRegisterCiSgFreeFunction(CiServiceGroupID id,
					   CiSgOpaqueHandle reqHandle,
					   CiSgOpaqueHandle rspHandle,
					   CiSgFreeReqMem ciSgFreeReqMem,
					   CiSgFreeRspMem ciSgFreeRspMem)
{
	if ( id >= CI_SG_ID_NEXTAVAIL )
	{
		CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_ERROR, "Invalid service ID");
		CCI_ASSERT(FALSE);
	}

	// Can be Null If client uses a static memory */
	if ( ciSgFreeReqMem != NULL)
	{
		gCiSgOpaqueReqHandle[id] = reqHandle;
		gCiSgFreeReqMemFuncTable[id] = ciSgFreeReqMem;
	}

	if (ciSgFreeRspMem != NULL)
	{
		gCiSgOpaqueRspHandle[id] = rspHandle;
		// SCR TBD: 32, 33 */
		gCiSgFreeRspMemFuncTable[id] = ciSgFreeRspMem;
	}
}

CiReturnCode  ciShRequest_1 (CiShHandle handle,
			     CiShOper oper,
			     void* reqParas,
			     CiShRequestHandle opHandle)
{
	CiShRequestArgs *pProcArgs;
	UINT32 primDataSize;
	CiShOperRegisterDefaultSgReq *pCiShOperRegisterDefaultSgReq;
	CiShOperRegisterSgReq *pCiShOperRegisterSgReq;
	unsigned char *pShmBuf;
	int shmBufLen;
	unsigned char *bufptr;

	CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_API, "ciShRequest_1()");
	if (gDlLinkDownFlag == TRUE)
	{
		CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_API, "ciShRequest_1: CIRC_INTERLINK_FAIL");
		return CIRC_INTERLINK_FAIL;
	}

	if (oper == CI_SH_OPER_REGISTERDEFSG)
	{
		pCiShOperRegisterDefaultSgReq = (CiShOperRegisterDefaultSgReq *)reqParas;
		gCiDefConfirmFuncPtr = pCiShOperRegisterDefaultSgReq->ciCnfDef;
		gCiDefIndicateFuncPtr = pCiShOperRegisterDefaultSgReq->ciIndDef;
	}
	if (oper == CI_SH_OPER_DEREGISTERDEFSG)
	{
		gCiDefConfirmFuncPtr = NULL; /* de-register default confirmation and indication callback */
		gCiDefIndicateFuncPtr = NULL;
	}

	/*=========== PER service group =============*/
	if (oper == CI_SH_OPER_REGISTERSG)
	{
		pCiShOperRegisterSgReq = (CiShOperRegisterSgReq *)reqParas;
		gCiIndicateFuncPtr[pCiShOperRegisterSgReq->id] = pCiShOperRegisterSgReq->ciIndicate;
		gCiConfirmFuncPtr[pCiShOperRegisterSgReq->id] = pCiShOperRegisterSgReq->ciConfirm;
		gOpSgCnfHandle[pCiShOperRegisterSgReq->id] = pCiShOperRegisterSgReq->opSgCnfHandle;
		gOpSgIndHandle[pCiShOperRegisterSgReq->id] = pCiShOperRegisterSgReq->opSgIndHandle;
#if 1
		clientRegisterCiSgFreeFunction(pCiShOperRegisterSgReq->id,
					       pCiShOperRegisterSgReq->opSgFreeReqHandle,
					       pCiShOperRegisterSgReq->opSgFreeRspHandle,
					       pCiShOperRegisterSgReq->ciSgFreeReqMem,
					       pCiShOperRegisterSgReq->ciSgFreeRspMem);
#endif
	}

	primDataSize =  cimem_GetCiShReqDataSize(oper);

	shmBufLen = SHM_HEADER_SIZE + sizeof(CiShRequestArgs) + primDataSize;
	pShmBuf = CI_MEM_ALLOC(shmBufLen);
	pProcArgs = (CiShRequestArgs *)(pShmBuf + SHM_HEADER_SIZE);

	pProcArgs->handle = handle;
	pProcArgs->oper = oper;
	pProcArgs->opHandle = opHandle;
	bufptr =   pShmBuf + SHM_HEADER_SIZE + sizeof(CiShRequestArgs);
	memcpy(bufptr, reqParas, primDataSize);

	ciSendMsgToServer(CiShRequestProcId, pShmBuf, sizeof(CiShRequestArgs) + primDataSize);

	CI_MEM_FREE(pShmBuf);

	return CIRC_SUCCESS;

}

CiReturnCode ciRequest_Transfer(CiServiceHandle svchandle,
								CiPrimitiveID primId,
								CiRequestHandle crsm_reqHandle,
								void* paras)
{
	int	cmd = 0;
	CiReturnCode	ret = CIRC_FAIL;
	int	rc = -1;
	int iFileId = 0;
	UINT8 judge = 1;
	UINT16 iIndex = 0;  
	UINT16 iPBIndex = 0;
	BOOL deleteOp = FALSE;
	BOOL updateOp = FALSE;
      	CiSimPrimGenericCmdReq         *GenSimCmd_crsm;
	GenSimCmd_crsm = (CiSimPrimGenericCmdReq*)paras;
	iFileId=GetSIMEfFileId(GenSimCmd_crsm->path);
	cmd = GenSimCmd_crsm->instruction;
	if (EF_SMS == iFileId || EF_FDN == iFileId || EF_EXT2 == iFileId )
		{
		/*Michal Bukai - Extension2 support - update FDN PB with long numbers */
		if(iFileId == EF_EXT2)
		{
			if (READ_RECORD == cmd)
			{
				CrsmCciAction *pnode;
				if ((pnode = (CrsmCciAction *)CI_MEM_ALLOC(sizeof(CrsmCciAction)))!=NULL)
				{
					pnode->cci_confirm = EXT2_READ_OPER;
					pnode->crsm_reshandle = crsm_reqHandle;
					pnode->svshandle = svchandle;
					pnode->ci_request = NULL;
					pnode->GenSimCmd_crsm = *GenSimCmd_crsm;
					add2CciActionlist(pnode);
				}
				else
					return CIRC_FAIL;
			}
			if (UPDATE_RECORD == cmd)
			{
				CrsmCciAction *pnode;
				if ((pnode = (CrsmCciAction *)CI_MEM_ALLOC(sizeof(CrsmCciAction)))!=NULL)
				{
					UINT8 iLen = 13;
					BOOL bExtExist = FALSE;
					iPBIndex = GetRecordIndex(GenSimCmd_crsm->param1, GenSimCmd_crsm->param2);
					if(GenSimCmd_crsm->length < 14)
						iLen = GenSimCmd_crsm->length;

					//just update EXT2 table - record will be update when FDN will be update (via CI_PB) - if not update via FDN, this record will not be updatein EXT2 file
					//Record exists - Update record data
					for(iIndex = 0; iIndex<100; iIndex++)
					{
						if(gEXT2Table[iIndex].ExtIndexReq == iPBIndex)
						{
							memset(gEXT2Table[iIndex].pRecord,EMPTY,13);
#ifdef CCI_SIM_CONTIGUOUS
							memcpy(gEXT2Table[iIndex].pRecord, GenSimCmd_crsm->data, iLen);
#else
							memcpy(gEXT2Table[iIndex].pRecord, GenSimCmd_crsm->pData, iLen);
#endif
							gEXT2Table[iIndex].status = PB_RECORD_TABLE;
							bExtExist = TRUE;
							iIndex = 100;
						}
					}

					//Record doesn't exist - enter new 
					if(!bExtExist)
					{
						for(iIndex = 0; iIndex<100; iIndex++)
						{
							if(gEXT2Table[iIndex].status == PB_RECORD_EMPTY)
							{
								gEXT2Table[iIndex].ExtIndexReq = iPBIndex;
								gEXT2Table[iIndex].status = PB_RECORD_TABLE;
								memset(gEXT2Table[iIndex].pRecord,EMPTY,13);
#ifdef CCI_SIM_CONTIGUOUS
								memcpy(gEXT2Table[iIndex].pRecord,GenSimCmd_crsm->data, iLen);
#else
								memcpy(gEXT2Table[iIndex].pRecord,GenSimCmd_crsm->pData, iLen);
#endif
								iIndex = 100;
							}
						}
					}
					//send fake request

					GenSimCmd_crsm->instruction = CI_SIM_READ_RECORD;
					GenSimCmd_crsm->length = 0;
					GenSimCmd_crsm->responseExpected = TRUE;
					GenSimCmd_crsm->responseLength = 0x0d;

					pnode->cci_confirm = EXT2_UPDATE_OPER;
					pnode->crsm_reshandle = crsm_reqHandle;
					pnode->svshandle = svchandle;
					pnode->ci_request = NULL;
					add2CciActionlist(pnode);

				}
				else
					return CIRC_FAIL;
			}
			if(GET_RESPONSE == cmd)
			{
				GenSimCmd_crsm->instruction = CI_SIM_SELECT;
				GenSimCmd_crsm->param1 = 0;
				GenSimCmd_crsm->param2 = 0;
				GenSimCmd_crsm->length = 2;
#ifdef CCI_SIM_CONTIGUOUS
				GenSimCmd_crsm->data[0]= (iFileId>>8) & 0xFF;
				GenSimCmd_crsm->data[1]= (iFileId) & 0xFF;
#else
				GenSimCmd_crsm->pData[0]= (iFileId>>8) & 0xFF;
				GenSimCmd_crsm->pData[1]= (iFileId) & 0xFF;
#endif
				GenSimCmd_crsm->responseExpected = TRUE;
				GenSimCmd_crsm->responseLength = 255;
				if(GenSimCmd_crsm->path.len > 1)/* remove the file id the last 2 bytes*/
				{
#ifdef CCI_SIM_CONTIGUOUS
					GenSimCmd_crsm->path.valStr[GenSimCmd_crsm->path.len -1] = '\0';
					GenSimCmd_crsm->path.valStr[GenSimCmd_crsm->path.len -2] = '\0';
#else
					GenSimCmd_crsm->path.val[GenSimCmd_crsm->path.len -1] = '\0';
					GenSimCmd_crsm->path.val[GenSimCmd_crsm->path.len -2] = '\0';
#endif
					GenSimCmd_crsm->path.len -= 2;
				}
			}
			return(ciRequest_1(svchandle,primId,crsm_reqHandle,GenSimCmd_crsm));
		}
		if (iFileId == EF_SMS)
			{

			if (READ_RECORD == cmd)
				{
				//Michal Bukai - in case of Read SMS operetion, CRSM command will not Mapped to CI_MSG (CQ00056743)
				#if 0
				CrsmCciAction *pnode;
				if ((pnode = (CrsmCciAction *)CI_MEM_ALLOC(sizeof(CrsmCciAction)))!=NULL)
					{
					pnode->cci_confirm = CI_MSG_PRIM_READ_MESSAGE_CNF;
					pnode->crsm_reshandle = crsm_reqHandle;
					pnode->svshandle = svchandle;
					pnode->ci_request = (void*)CI_MEM_ALLOC(sizeof(CiMsgPrimSelectStorageReq));
					pnode->GenSimCmd_crsm = *GenSimCmd_crsm;
					if(pnode->ci_request !=NULL)
						{
							((CiMsgPrimSelectStorageReq*)(pnode->ci_request))->storage = CI_MSG_STORAGE_SM;
							((CiMsgPrimSelectStorageReq*)(pnode->ci_request))->type = CI_MSG_STRGOPERTYPE_READ_DELETE;
							add2CciActionlist(pnode);
						}
					else
						{
							CI_MEM_FREE(pnode);
							return CIRC_FAIL;
						}
					}
				ret = ciRequest( ciATciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_SELECT_STORAGE_REQ,
								crsm_reqHandle, pnode->ci_request );
				rc = handleReturn(ret);
					return rc;
				#endif
				return(ciRequest_1(svchandle,primId,crsm_reqHandle,paras));
				}
			if(UPDATE_RECORD == cmd)
				{
#ifdef CCI_SIM_CONTIGUOUS
				judgeOp(GenSimCmd_crsm->data,&judge,GenSimCmd_crsm->length);
#else
				judgeOp(GenSimCmd_crsm->pData,&judge,GenSimCmd_crsm->length);
#endif
				if(judge== GenSimCmd_crsm->length)
					{
						CrsmCciAction *pnode;
						if ((pnode = (CrsmCciAction *)CI_MEM_ALLOC(sizeof(CrsmCciAction)))!=NULL)
						{
							pnode->cci_confirm = CI_MSG_PRIM_DELETE_MESSAGE_CNF;
							pnode->crsm_reshandle = crsm_reqHandle;
							pnode->svshandle = svchandle;
							pnode->ci_request = (void*)CI_MEM_ALLOC(sizeof(CiMsgPrimSelectStorageReq));
							pnode->GenSimCmd_crsm = *GenSimCmd_crsm;
							if(pnode->ci_request !=NULL)
							{
								((CiMsgPrimSelectStorageReq*)(pnode->ci_request))->storage = CI_MSG_STORAGE_SM;
								((CiMsgPrimSelectStorageReq*)(pnode->ci_request))->type = CI_MSG_STRGOPERTYPE_READ_DELETE;
								add2CciActionlist(pnode);
							}
							else
							{
								CI_MEM_FREE(pnode);
								return CIRC_FAIL;
							}
						}
						else
							return CIRC_FAIL;
						ret = ciRequest_1(ciATciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_SELECT_STORAGE_REQ,
								crsm_reqHandle, pnode->ci_request );
						rc = handleReturn(ret);
						if(rc == CIRC_FAIL)
							freeCciActionlist(pnode);
						return rc;

					}
				if(judge< GenSimCmd_crsm->length)
					{
						CrsmCciAction *pnode;
						if ((pnode = (CrsmCciAction *)CI_MEM_ALLOC(sizeof(CrsmCciAction)))!=NULL)
						{
							pnode->cci_confirm = CI_MSG_PRIM_WRITE_MSG_WITH_INDEX_CNF;
							pnode->crsm_reshandle = crsm_reqHandle;
							pnode->svshandle = svchandle;
							pnode->ci_request = (void*)CI_MEM_ALLOC(sizeof(CiMsgPrimSelectStorageReq));
							pnode->GenSimCmd_crsm = *GenSimCmd_crsm;
							if(pnode->ci_request !=NULL)
							{
								((CiMsgPrimSelectStorageReq*)(pnode->ci_request))->storage = CI_MSG_STORAGE_SM;
								((CiMsgPrimSelectStorageReq*)(pnode->ci_request))->type = CI_MSG_STRGOPERTYPE_WRITE_SEND;
								add2CciActionlist(pnode);
							}
							else
							{
								CI_MEM_FREE(pnode);
								return CIRC_FAIL;
							}
						}
						else
							return CIRC_FAIL;
						ret = ciRequest_1( ciATciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_SELECT_STORAGE_REQ,
								crsm_reqHandle, pnode->ci_request );
						rc = handleReturn(ret);
						if(rc == CIRC_FAIL)
							freeCciActionlist(pnode);
						return rc;
					}
				}
				/* START: Michal Bukai - Convert Response command to Select command */
				if(GET_RESPONSE == cmd)
				{
					GenSimCmd_crsm->instruction = CI_SIM_SELECT;
					GenSimCmd_crsm->param1 = 0;
					GenSimCmd_crsm->param2 = 0;
					GenSimCmd_crsm->length = 2;
#ifdef CCI_SIM_CONTIGUOUS
					GenSimCmd_crsm->data[0]= (iFileId>>8) & 0xFF;
					GenSimCmd_crsm->data[1]= (iFileId) & 0xFF;
#else
					GenSimCmd_crsm->pData[0]= (iFileId>>8) & 0xFF;
					GenSimCmd_crsm->pData[1]= (iFileId) & 0xFF;
#endif
					GenSimCmd_crsm->responseExpected = TRUE;
					GenSimCmd_crsm->responseLength = 255;
					if(GenSimCmd_crsm->path.len > 1)/* remove the file id the last 2 bytes*/
					{
#ifdef CCI_SIM_CONTIGUOUS
						GenSimCmd_crsm->path.valStr[GenSimCmd_crsm->path.len -1] = '\0';
						GenSimCmd_crsm->path.valStr[GenSimCmd_crsm->path.len -2] = '\0';
#else
						GenSimCmd_crsm->path.val[GenSimCmd_crsm->path.len -1] = '\0';
						GenSimCmd_crsm->path.val[GenSimCmd_crsm->path.len -2] = '\0';
#endif
						GenSimCmd_crsm->path.len -= 2;
					}
					return(ciRequest_1(svchandle,primId,crsm_reqHandle,GenSimCmd_crsm));
				}
				/* END: Michal Bukai - Convert Response command to Select command */

			}
		if (iFileId == EF_FDN)
			{
				if (READ_RECORD == cmd)
				{
				CrsmCciAction *pnode;
				if ((pnode = (CrsmCciAction *)CI_MEM_ALLOC(sizeof(CrsmCciAction)))!=NULL)
					{
					pnode->cci_confirm = CI_PB_PRIM_READ_PHONEBOOK_ENTRY_CNF;
					pnode->crsm_reshandle = crsm_reqHandle;
					pnode->svshandle = svchandle;
					pnode->ci_request = (void*)CI_MEM_ALLOC(sizeof(CiPbPrimSelectPhonebookReq));
					pnode->GenSimCmd_crsm = *GenSimCmd_crsm;
					if(pnode->ci_request !=NULL)
						{
							((CiPbPrimSelectPhonebookReq*)(pnode->ci_request))->PbId = CI_PHONEBOOK_FD;
#ifdef CCI_SIM_CONTIGUOUS
							((CiPbPrimSelectPhonebookReq*)(pnode->ci_request))->password.len = 0;
							((CiPbPrimSelectPhonebookReq*)(pnode->ci_request))->password.data[0] = (UINT8)0;
#else
							((CiPbPrimSelectPhonebookReq*)(pnode->ci_request))->Password = NULL;
#endif
							add2CciActionlist(pnode);
						}
					else
						{
						CI_MEM_FREE(pnode);
						return CIRC_FAIL;
						}
					}
				else
					return CIRC_FAIL;
				ret = ciRequest_1( ciATciSvgHandle[CI_SG_ID_PB], CI_PB_PRIM_SELECT_PHONEBOOK_REQ,
								crsm_reqHandle, pnode->ci_request );
				rc = handleReturn(ret);
				if(rc == CIRC_FAIL)
					freeCciActionlist(pnode);
				return rc;
				/* handle the return value */
			}
				if(UPDATE_RECORD == cmd)
				{
					if(GenSimCmd_crsm->length < 14)
						return CIRC_FAIL; /*pdu dat must contain at least 14 bytes*/
					UINT8 AlphaIDLen = GenSimCmd_crsm->length - 14;
					judge = 0;
#ifdef CCI_SIM_CONTIGUOUS
					judgeOp(GenSimCmd_crsm->data,&judge,AlphaIDLen);
#else
					judgeOp(GenSimCmd_crsm->pData,&judge,AlphaIDLen);
#endif

					if(judge == AlphaIDLen)
						deleteOp = TRUE;
					else
						{
						judge = AlphaIDLen;
#ifdef CCI_SIM_CONTIGUOUS
						judgeOp(GenSimCmd_crsm->data,&judge,GenSimCmd_crsm->length);
#else
						judgeOp(GenSimCmd_crsm->pData,&judge,GenSimCmd_crsm->length);
#endif

						if(judge == GenSimCmd_crsm->length)
							deleteOp = TRUE;
						else
							updateOp = TRUE;
						}
					if(deleteOp == TRUE)
					{
						CrsmCciAction *pnode;
						if ((pnode = (CrsmCciAction *)CI_MEM_ALLOC(sizeof(CrsmCciAction)))!=NULL)
						{
							pnode->cci_confirm = CI_PB_PRIM_DELETE_PHONEBOOK_ENTRY_CNF;
							pnode->crsm_reshandle = crsm_reqHandle;
							pnode->svshandle = svchandle;
							pnode->ci_request = (void*)CI_MEM_ALLOC(sizeof(CiPbPrimSelectPhonebookReq));
							pnode->GenSimCmd_crsm = *GenSimCmd_crsm;
							if(pnode->ci_request !=NULL)
							{
								((CiPbPrimSelectPhonebookReq*)(pnode->ci_request))->PbId = CI_PHONEBOOK_FD;

#ifdef CCI_SIM_CONTIGUOUS
								((CiPbPrimSelectPhonebookReq*)(pnode->ci_request))->password.len = 0;
								((CiPbPrimSelectPhonebookReq*)(pnode->ci_request))->password.data[0] = (UINT8)0;
#else
								((CiPbPrimSelectPhonebookReq*)(pnode->ci_request))->Password = NULL;
#endif

								add2CciActionlist(pnode);
							}
							else
							{
							CI_MEM_FREE(pnode);
							return CIRC_FAIL;
							}
						}
						else
							return CIRC_FAIL;
						ret = ciRequest_1( ciATciSvgHandle[CI_SG_ID_PB], CI_PB_PRIM_SELECT_PHONEBOOK_REQ,
										crsm_reqHandle, pnode->ci_request );
						rc = handleReturn(ret);
						if(rc == CIRC_FAIL)
							freeCciActionlist(pnode);
						return rc;
					}
					if(updateOp == TRUE)
					{
						CrsmCciAction *pnode;
						if ((pnode = (CrsmCciAction *)CI_MEM_ALLOC(sizeof(CrsmCciAction)))!=NULL)
							{
							pnode->cci_confirm = CI_PB_PRIM_REPLACE_PHONEBOOK_ENTRY_CNF;
							pnode->crsm_reshandle = crsm_reqHandle;
							pnode->svshandle = svchandle;
							pnode->ci_request = (void*)CI_MEM_ALLOC(sizeof(CiPbPrimSelectPhonebookReq));
							pnode->GenSimCmd_crsm = *GenSimCmd_crsm;
							if(pnode->ci_request !=NULL)
							{
								((CiPbPrimSelectPhonebookReq*)(pnode->ci_request))->PbId = CI_PHONEBOOK_FD;
#ifdef CCI_SIM_CONTIGUOUS
								((CiPbPrimSelectPhonebookReq*)(pnode->ci_request))->password.len = 0;
								((CiPbPrimSelectPhonebookReq*)(pnode->ci_request))->password.data[0] = (UINT8)0;
#else
								((CiPbPrimSelectPhonebookReq*)(pnode->ci_request))->Password = NULL;
#endif

								add2CciActionlist(pnode);
							}
							else
							{
							CI_MEM_FREE(pnode);
							return CIRC_FAIL;
							}
							}
							else
								return CIRC_FAIL;
							ret = ciRequest_1( ciATciSvgHandle[CI_SG_ID_PB], CI_PB_PRIM_SELECT_PHONEBOOK_REQ,
										crsm_reqHandle, pnode->ci_request );
							rc = handleReturn(ret);
							if(rc == CIRC_FAIL)
								freeCciActionlist(pnode);
							return rc;
					}
				}
				/* START: Michal Bukai - Convert Response command to Select command */
				if(GET_RESPONSE == cmd)
				{
					GenSimCmd_crsm->instruction = CI_SIM_SELECT;
					GenSimCmd_crsm->param1 = 0;
					GenSimCmd_crsm->param2 = 0;
					GenSimCmd_crsm->length = 2;
#ifdef CCI_SIM_CONTIGUOUS
					GenSimCmd_crsm->data[0]= (iFileId>>8) & 0xFF;
					GenSimCmd_crsm->data[1]= (iFileId) & 0xFF;
#else
					GenSimCmd_crsm->pData[0]= (iFileId>>8) & 0xFF;
					GenSimCmd_crsm->pData[1]= (iFileId) & 0xFF;
#endif
					GenSimCmd_crsm->responseExpected = TRUE;
					GenSimCmd_crsm->responseLength = 255;
					if(GenSimCmd_crsm->path.len > 1)/* remove the file id the last 2 bytes*/
					{
#ifdef CCI_SIM_CONTIGUOUS
						GenSimCmd_crsm->path.valStr[GenSimCmd_crsm->path.len -1] = '\0';
						GenSimCmd_crsm->path.valStr[GenSimCmd_crsm->path.len -2] = '\0';
#else
						GenSimCmd_crsm->path.val[GenSimCmd_crsm->path.len -1] = '\0';
						GenSimCmd_crsm->path.val[GenSimCmd_crsm->path.len -2] = '\0';
#endif
						GenSimCmd_crsm->path.len -= 2;
					}
					return(ciRequest_1(svchandle,primId,crsm_reqHandle,GenSimCmd_crsm));
				}
				/* END: Michal Bukai - Convert Response command to Select command */
		}
	else
		{
		CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_ERROR," *****it is not support yet!****");
		}
}
else
{
	/* START: Michal Bukai - Convert Response command to Select command for all other file IDs*/
	if(GET_RESPONSE == cmd)
	{
    					GenSimCmd_crsm->instruction = CI_SIM_SELECT;
						GenSimCmd_crsm->param1 = 0;
						GenSimCmd_crsm->param2 = 0;
						GenSimCmd_crsm->length = 2;
#ifdef CCI_SIM_CONTIGUOUS
						GenSimCmd_crsm->data[0]= (iFileId>>8) & 0xFF;
						GenSimCmd_crsm->data[1]= (iFileId) & 0xFF;
#else
						GenSimCmd_crsm->pData[0]= (iFileId>>8) & 0xFF;
						GenSimCmd_crsm->pData[1]= (iFileId) & 0xFF;
#endif
						GenSimCmd_crsm->responseExpected = TRUE;
						GenSimCmd_crsm->responseLength = 255;
						if(GenSimCmd_crsm->path.len > 1)/* remove the file id the last 2 bytes*/
						{
#ifdef CCI_SIM_CONTIGUOUS
							GenSimCmd_crsm->path.valStr[GenSimCmd_crsm->path.len -1] = '\0';
							GenSimCmd_crsm->path.valStr[GenSimCmd_crsm->path.len -2] = '\0';
#else
							GenSimCmd_crsm->path.val[GenSimCmd_crsm->path.len -1] = '\0';
							GenSimCmd_crsm->path.val[GenSimCmd_crsm->path.len -2] = '\0';
#endif
							GenSimCmd_crsm->path.len -= 2;
						}
						return(ciRequest_1(svchandle,primId,crsm_reqHandle,GenSimCmd_crsm));
	}
	/* END: Michal Bukai - Convert Response command to Select command */
	else
	{
		return(ciRequest_1(svchandle,primId,crsm_reqHandle,paras));
	}
}
	return CIRC_FAIL;

}


CiReturnCode  ciRequest_1 (CiServiceHandle handle,
			   CiPrimitiveID primId,
			   CiRequestHandle reqHandle,
			   void* paras)
{
	CiRequestArgs *pProcArgs;
	UINT32 primDataSize = 0;
	unsigned char *bufptr;
	unsigned char *pShmBuf;
	int shmBufLen;
	
	/* Check Link status flag first */
	if (gDlLinkDownFlag == TRUE)
	{
		CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_ERROR, "ciRequest_1: CIRC_INTERLINK_FAIL");
		return CIRC_INTERLINK_FAIL;
	}

	primDataSize = cimem_GetCiPrimDataSize(handle, primId, paras);

	shmBufLen = SHM_HEADER_SIZE + sizeof(CiRequestArgs) + primDataSize;
	pShmBuf = CI_MEM_ALLOC(shmBufLen);
	pProcArgs = (CiRequestArgs *)(pShmBuf + SHM_HEADER_SIZE);

	pProcArgs->handle = handle;
	pProcArgs->primId = primId;
	pProcArgs->reqHandle = reqHandle;
	bufptr =   pShmBuf + SHM_HEADER_SIZE + sizeof(CiRequestArgs);
	memcpy(bufptr, paras, primDataSize);

#ifdef CI_DATA_DUMP
	ciDumpMsgInfo( handle, primId, reqHandle, paras, primDataSize);
#endif
	ciSendMsgToServer(CiRequestProcId, pShmBuf, sizeof(CiRequestArgs) + primDataSize);

	CI_MEM_FREE(pShmBuf);

	CCI_TRACE3(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_INFO, "ciRequest_1:Svchandle 0x%x, primId %d, reqHandle 0x%x",handle,primId,reqHandle);
	return CIRC_SUCCESS;
}


CiReturnCode ciRespond_1 (CiServiceHandle handle,
			  CiPrimitiveID primId,
			  CiIndicationHandle indHandle,
			  void* paras)
{
	CiRespondArgs *pProcArgs;
	UINT32 primDataSize;
	unsigned char *pShmBuf;
	int shmBufLen;
	unsigned char *bufptr;


	CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_API, "ciRespond_1()");
	if (gDlLinkDownFlag == TRUE)
	{
		CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_API, "ciRespond_1: CIRC_INTERLINK_FAIL");
		return CIRC_INTERLINK_FAIL;
	}

	/* Check Link status flag first */

	primDataSize = cimem_GetCiPrimDataSize(handle, primId, paras);

	shmBufLen = SHM_HEADER_SIZE + sizeof(CiRespondArgs) + primDataSize;
	pShmBuf = CI_MEM_ALLOC(shmBufLen);
	pProcArgs = (CiRespondArgs *)(pShmBuf + SHM_HEADER_SIZE);

	pProcArgs->handle = handle;
	pProcArgs->primId = primId;
	pProcArgs->indHandle = indHandle;
	bufptr =   pShmBuf + SHM_HEADER_SIZE + sizeof(CiRespondArgs);
	memcpy(bufptr, paras, primDataSize);

#ifdef CI_DATA_DUMP
	ciDumpMsgInfo( handle, primId, indHandle, paras, primDataSize);
#endif
	ciSendMsgToServer(CiRespondProcId, pShmBuf, sizeof(CiRespondArgs) + primDataSize);

	CI_MEM_FREE(pShmBuf);

	return CIRC_SUCCESS;
}



void clientCiShConfirmCallback(CiShConfirmArgs* pArg,  void *paras)
{
	CiShOperDeregisterShCnf *pCiShOperDeregisterShCnf;
	CiShOperRegisterShCnf *pCiShOperRegisterShCnf;
	CiShOperRegisterDefaultSgCnf *pCiShOperRegisterDefaultSgCnf;
	CiShOperRegisterSgCnf *pCiShOperRegisterSgCnf;
	CiShOperDeregisterSgCnf *pCiShOperDeregisterSgCnf;


	CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_API, "clientCiShConfirmCallback()");


	/* Replace CI Client stub callback for free conf para */
	if (pArg->oper == CI_SH_OPER_REGISTERSH)
	{
		pCiShOperRegisterShCnf = (CiShOperRegisterShCnf *)paras;
		// pCiShOperRegisterShCnf->opShFreeHandle = gOpShFreeHandle;
		// pCiShOperRegisterShCnf->ciShFreeCnfMem = clientCiShFreeCnfMem;
	}
	if (pArg->oper == CI_SH_OPER_REGISTERDEFSG)
	{
		pCiShOperRegisterDefaultSgCnf = (CiShOperRegisterDefaultSgCnf *)paras;
		// pCiShOperRegisterDefaultSgCnf->opSgFreeDefCnfHandle = gOpShFreeHandle;
		// pCiShOperRegisterDefaultSgCnf->opSgFreeDefIndHandle = gOpShFreeHandle;
		// pCiShOperRegisterDefaultSgCnf->ciSgFreeDefCnfMem = clientCiSgFreeCnfMem;
		// pCiShOperRegisterDefaultSgCnf->ciSgFreeDefIndMem = clientCiSgFreeIndMem;
	}
	if (pArg->oper == CI_SH_OPER_REGISTERSG)
	{
		pCiShOperRegisterSgCnf = (CiShOperRegisterSgCnf *)paras;
		gCiServiceHandle[pCiShOperRegisterSgCnf->id] = pCiShOperRegisterSgCnf->handle;
		// pCiShOperRegisterSgCnf->opSgFreeCnfHandle = gOpShFreeHandle;
		// pCiShOperRegisterSgCnf->opSgFreeIndHandle = gOpShFreeHandle;
		// pCiShOperRegisterSgCnf->ciSgFreeCnfMem = clientCiSgFreeCnfMem;
		// pCiShOperRegisterSgCnf->ciSgFreeIndMem = clientCiSgFreeIndMem;
	}
	/* call real callback function that register by RIL */
	/*SCR 2000769*/
	if (pArg->oper == CI_SH_OPER_DEREGISTERSG)
	{
		pCiShOperDeregisterSgCnf = (CiShOperDeregisterSgCnf *)paras;
		gCiIndicateFuncPtr[pCiShOperDeregisterSgCnf->id] = NULL;
		gCiConfirmFuncPtr[pCiShOperDeregisterSgCnf->id] = NULL;
		gCiServiceHandle[pCiShOperDeregisterSgCnf->id] = 0;
	}

	//gCiShConfirmFuncPtr(pArg->opShHandle, pArg->oper,pArg->cnfParas,pArg->opHandle);
	gCiShConfirmFuncPtr(gCiShOpaqueHandle, pArg->oper, paras, pArg->opHandle);

	/* In case of CI_SH_OPER_DEREGISTERSH we need to deregister confirmationcallback */
	if (pArg->oper == CI_SH_OPER_DEREGISTERSH)
	{
		pCiShOperDeregisterShCnf = (CiShOperDeregisterShCnf *)paras;

		if (pCiShOperDeregisterShCnf->rc == CIRC_SUCCESS)
		{
			gCiShConfirmFuncPtr = NULL;
		}
	} //end if
}



void clientCiDefConfirmCallback(CiConfirmArgs* pArg, void *paras)
{
	CCI_TRACE3(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_INFO, "clientCiDefConfirmCallbac:svcId:%d,primId:%d,reqHandle:0x%x\n", pArg->id, pArg->primId, pArg->reqHandle);

	/* call real callback function that register by RIL */
	if (gCiDefConfirmFuncPtr != NULL)
	{
		gCiDefConfirmFuncPtr(pArg->opSgCnfHandle, pArg->id, pArg->primId, pArg->reqHandle, paras);
	}
	else
	{
		CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_ERROR, " CiDefConfirmCallback Function Pointer Error");
		CCI_ASSERT(FALSE);
	}
}

void clientCiConfirmCallback(CiConfirmArgs* pArg, void *paras)
{
	CrsmCciAction *tem = head;

	CCI_TRACE3(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_INFO, "clientCiConfirmCallback:svcId:%d,primId:%d,reqHandle:0x%x\n", pArg->id, pArg->primId, pArg->reqHandle);

	CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_API, "clientCiConfirmCallback()");

#ifdef CI_DATA_DUMP
	ciDumpMsgInfo( pArg->id, pArg->primId, pArg->reqHandle, paras, 0);
#endif

	if (head!=NULL)
	{
	  while (tem!=NULL&&tem->crsm_reshandle!=pArg->reqHandle)
	  {
		  tem = tem->next;
	  }
	}


	/* call real callback function that register by RIL */
	if (gCiConfirmFuncPtr[pArg->id] != NULL)
	{
  		if (tem!=NULL)
		{	
			clientCiConfirmCallback_transfer(pArg,paras,tem);
  		}
		else
		{
		    gCiConfirmFuncPtr[pArg->id](pArg->opSgCnfHandle, pArg->id, pArg->primId, pArg->reqHandle, paras);
		}
	}
	else
	{
		CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_ERROR, " CiConfirmCallback Function Pointer Error");
		CCI_ASSERT(FALSE);
	}
}


void clientCiConfirmCallback_transfer(CiConfirmArgs* pArg, void *paras,CrsmCciAction *tem)
{
	CiReturnCode ret = CIRC_FAIL;
	int rc = -1;
	CiConfirmArgs cnf_args;
	CiSimPrimGenericCmdCnf * genericCmdCnf;
	CiSimPrimGenericCmdCnf CmdCnf_str;
	genericCmdCnf = &CmdCnf_str; //(CiSimPrimGenericCmdCnf *)CI_MEM_ALLOC(sizeof(CiSimPrimGenericCmdCnf));
	BOOL sendCiReq = FALSE; /*Michal Bukai - Memory and other clean up in case CI reqest was not sen due to wrong data or memory allocation problems */
    UINT16 iIndex = 0;  
	UINT16 iPBIndex = 0;
	CCI_ASSERT(tem != NULL);
	CCI_TRACE3(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_INFO, "clientCiConfirmCallback_transfer: primId:%d;tem->reqHandle:0x%x;tem->cci_confirm:%d\n",
			pArg->primId, tem->crsm_reshandle, tem->cci_confirm);

	if (genericCmdCnf != NULL)
	{
		memset(genericCmdCnf,0,sizeof(CiSimPrimGenericCmdCnf));
#ifndef CCI_SIM_CONTIGUOUS
		genericCmdCnf->pCnf = (CiSimCmdRsp*)CI_MEM_ALLOC(sizeof(CiSimCmdRsp));
		memset(genericCmdCnf->pCnf,0,sizeof(CiSimCmdRsp));
		genericCmdCnf->pCnf->pData = (UINT8*)CI_MEM_ALLOC(CI_SIM_MAX_CMD_DATA_SIZE);
		memset(genericCmdCnf->pCnf->pData,0,CI_SIM_MAX_CMD_DATA_SIZE);
#endif

	switch(pArg->primId)
	{
	case CI_ERR_PRIM_SIMNOTREADY_CNF:
	{
		genericCmdCnf->rc = CIRC_SIM_NOT_INSERTED;
		if (tem!=NULL)
		{
		cnf_args.id = CI_SG_ID_SIM;
		cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
		cnf_args.paras = genericCmdCnf;
		cnf_args.opSgCnfHandle = tem->svshandle;
		cnf_args.reqHandle = tem->crsm_reshandle;
		freeCciActionlist(tem);
		clientCiConfirmCallback(&cnf_args,genericCmdCnf);
		}
		break;
	}
	case CI_MSG_PRIM_SELECT_STORAGE_CNF:
	{
		CiMsgPrimSelectStorageCnf*	selectStorageCnf;
		selectStorageCnf = (CiMsgPrimSelectStorageCnf*)(paras);
		sendCiReq = FALSE;
		if ( selectStorageCnf->rc == CIRC_MSG_SUCCESS )
		{
			if(tem->cci_confirm == CI_MSG_PRIM_DELETE_MESSAGE_CNF)
			{
				CI_MEM_FREE(tem->ci_request);
				tem->ci_request = (void*) CI_MEM_ALLOC(sizeof(CiMsgPrimDeleteMessageReq));
				if (tem->ci_request!=NULL)
				{
					((CiMsgPrimDeleteMessageReq*)(tem->ci_request))->index =
						GetRecordIndex(tem->GenSimCmd_crsm.param1, tem->GenSimCmd_crsm.param2);
					if((UINT16)INVALID_INDEX!= ((CiMsgPrimDeleteMessageReq*)(tem->ci_request))->index)
					{
						ret = ciRequest_1( ciATciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_DELETE_MESSAGE_REQ,
						tem->crsm_reshandle,  (tem->ci_request));
						rc = handleReturn(ret);
						sendCiReq = TRUE;
					}
				}
			}
/*Michal Bukai - Map and validate CRSM data to status CS address and data fields of WriteMsgWithIndex struct*/
			if(tem->cci_confirm == CI_MSG_PRIM_WRITE_MSG_WITH_INDEX_CNF)
			{
				UINT8 pduIndex = 0;
                //UINT16 dataLen = 0;
				CI_MEM_FREE(tem->ci_request);
				tem->ci_request = (void*) CI_MEM_ALLOC(sizeof(CiMsgPrimWriteMsgWithIndexReq));
				if (tem->ci_request!=NULL)
				{
					((CiMsgPrimWriteMsgWithIndexReq*)(tem->ci_request))->index = GetRecordIndex(tem->GenSimCmd_crsm.param1, tem->GenSimCmd_crsm.param2);
					if((UINT16)INVALID_INDEX != ((CiMsgPrimWriteMsgWithIndexReq*)(tem->ci_request))->index)
					{
#ifndef CCI_SIM_CONTIGUOUS
						/*Status Byte*/
						((CiMsgPrimWriteMsgWithIndexReq*)(tem->ci_request))->status = tem->GenSimCmd_crsm.pData[pduIndex];
						pduIndex++;
						/*Service Center Address*/
						if(tem->GenSimCmd_crsm.pData[pduIndex] == 0x00)
						{/*Service Center Address = NULL */
							((CiMsgPrimWriteMsgWithIndexReq*)(tem->ci_request))->optSca.Present = FALSE;
							pduIndex +=1;
							sendCiReq = TRUE;
						}
						else if((tem->GenSimCmd_crsm.pData[pduIndex] == 0x01)&&(tem->GenSimCmd_crsm.pData[pduIndex + 1] == 0x80))
						{/*Service Center Address = NULL */
							((CiMsgPrimWriteMsgWithIndexReq*)(tem->ci_request))->optSca.Present = FALSE;
							pduIndex +=2;
							sendCiReq = TRUE;
						}
						else
						{
							/*check if service center address length is valid - if not set service center address to null*/
							if((tem->GenSimCmd_crsm.pData[pduIndex] > 1) && (tem->GenSimCmd_crsm.pData[pduIndex] < 12))
							{
								((CiMsgPrimWriteMsgWithIndexReq*)(tem->ci_request))->optSca.Present = TRUE;
								pduIndex += ReadAddressInfo((CiAddressInfo*) &(((CiMsgPrimWriteMsgWithIndexReq*)(tem->ci_request))->optSca.AddressInfo),(UINT8*)&(tem->GenSimCmd_crsm.pData[1]));
								if(pduIndex == 1)
									sendCiReq = FALSE;
								else
									sendCiReq = TRUE;
							}
							else
							{
								sendCiReq = FALSE;
							}
						}

						if(sendCiReq)
						{
                            UINT16 dataLen;

							((CiMsgPrimWriteMsgWithIndexReq*)(tem->ci_request))->pPdu = (CiMsgPdu*)CI_MEM_ALLOC(sizeof(CiMsgPdu));
							((CiMsgPrimWriteMsgWithIndexReq*)(tem->ci_request))->pPdu->pData = (UINT8*)CI_MEM_ALLOC(CI_MAX_CI_MSG_PDU_SIZE);

							dataLen = tem->GenSimCmd_crsm.length - pduIndex;
							if(dataLen > CI_MAX_CI_MSG_PDU_SIZE)
							{
								dataLen = CI_MAX_CI_MSG_PDU_SIZE;
							}

							((CiMsgPrimWriteMsgWithIndexReq*)(tem->ci_request))->pPdu->len = dataLen;
							 memcpy(((CiMsgPrimWriteMsgWithIndexReq*)(tem->ci_request))->pPdu->pData,&(tem->GenSimCmd_crsm.pData[pduIndex]),dataLen);
						}
#else
						/*Status Byte*/
						((CiMsgPrimWriteMsgWithIndexReq*)(tem->ci_request))->status = tem->GenSimCmd_crsm.data[pduIndex];
						pduIndex++;
						/*Service Center Address*/
						if(tem->GenSimCmd_crsm.data[pduIndex] == 0x00)
						{/*Service Center Address = NULL */
							((CiMsgPrimWriteMsgWithIndexReq*)(tem->ci_request))->optSca.Present = FALSE;
							pduIndex +=1;
							sendCiReq = TRUE;
						}
						else if((tem->GenSimCmd_crsm.data[pduIndex] == 0x01)&&(tem->GenSimCmd_crsm.data[pduIndex + 1] == 0x80))
						{/*Service Center Address = NULL */
							((CiMsgPrimWriteMsgWithIndexReq*)(tem->ci_request))->optSca.Present = FALSE;
							pduIndex +=2;
							sendCiReq = TRUE;
						}
						else
						{
							/*check if service center address length is valid - if not set service center address to null*/
							if((tem->GenSimCmd_crsm.data[pduIndex] > 1) && (tem->GenSimCmd_crsm.data[pduIndex] < 12))
							{
								((CiMsgPrimWriteMsgWithIndexReq*)(tem->ci_request))->optSca.Present = TRUE;
								pduIndex += ReadAddressInfo((CiAddressInfo*) &(((CiMsgPrimWriteMsgWithIndexReq*)(tem->ci_request))->optSca.AddressInfo),(UINT8*)&(tem->GenSimCmd_crsm.data[1]));
								if(pduIndex == 1)
									sendCiReq = FALSE;
								else
									sendCiReq = TRUE;
							}
							else
							{
								sendCiReq = FALSE;
							}
						}

						if(sendCiReq)
						{
                            UINT16 dataLen;

							dataLen = tem->GenSimCmd_crsm.length - pduIndex;
							if(dataLen > CI_MAX_CI_MSG_PDU_SIZE)
							{
								dataLen = CI_MAX_CI_MSG_PDU_SIZE;
							}

							((CiMsgPrimWriteMsgWithIndexReq*)(tem->ci_request))->pdu.len = dataLen;
							 memcpy(((CiMsgPrimWriteMsgWithIndexReq*)(tem->ci_request))->pdu.data,&(tem->GenSimCmd_crsm.data[pduIndex]),dataLen);
						}

#endif
						if(sendCiReq)
						{
							ret = ciRequest_1( ciATciSvgHandle[CI_SG_ID_MSG], CI_MSG_PRIM_WRITE_MSG_WITH_INDEX_REQ,
							tem->crsm_reshandle, (tem->ci_request));
							rc = handleReturn(ret);
						}
					}
				}
			}
			/*Michal Bukai - Memory and other clean up in case CI reqest was not sen due to wrong data or memory allocation problems */
			if(!sendCiReq)
			{
				genericCmdCnf->rc = CIRC_SIM_FAILURE;
				if (tem!=NULL)
				{
				cnf_args.id = CI_SG_ID_SIM;
				cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
				cnf_args.paras = genericCmdCnf;
				cnf_args.opSgCnfHandle = tem->svshandle;
				cnf_args.reqHandle = tem->crsm_reshandle;
				freeCciActionlist(tem);
				clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
			}
		}
		else
			{
				genericCmdCnf->rc = CIRC_SIM_MEM_PROBLEM;
				if (tem!=NULL)
				{
					cnf_args.id = CI_SG_ID_SIM;
					cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
					cnf_args.paras = genericCmdCnf;
					cnf_args.opSgCnfHandle = tem->svshandle;
					cnf_args.reqHandle = tem->crsm_reshandle;
					freeCciActionlist(tem);
					clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
			}
		break;
	}
#if 1
	case CI_PB_PRIM_SELECT_PHONEBOOK_CNF:
	{
		CiPbPrimSelectPhonebookCnf* selectPBCnf;
		selectPBCnf = (CiPbPrimSelectPhonebookCnf*)(paras);
		sendCiReq = FALSE;
		if ( selectPBCnf->Result == CIRC_PB_SUCCESS)
		{
			if(tem->cci_confirm == CI_PB_PRIM_READ_PHONEBOOK_ENTRY_CNF)
			{
				CI_MEM_FREE(tem->ci_request);
				tem->ci_request = (void*) CI_MEM_ALLOC(sizeof(CiPbPrimGetPhonebookInfoReq));
				if (tem->ci_request!=NULL)
				{
					((CiPbPrimGetPhonebookInfoReq*)(tem->ci_request))->PbId = CI_PHONEBOOK_FD;
					ret = ciRequest_1( ciATciSvgHandle[CI_SG_ID_PB], CI_PB_PRIM_GET_PHONEBOOK_INFO_REQ,
					tem->crsm_reshandle, (tem->ci_request));
					rc = handleReturn(ret);
					sendCiReq = TRUE;

				}
			}
			if(tem->cci_confirm == CI_PB_PRIM_DELETE_PHONEBOOK_ENTRY_CNF)
			{
				CI_MEM_FREE(tem->ci_request);
				tem->ci_request = (void*) CI_MEM_ALLOC(sizeof(CiPbPrimDeletePhonebookEntryReq));
				if (tem->ci_request!=NULL)
				{
				((CiPbPrimDeletePhonebookEntryReq*)(tem->ci_request))->Index =
						GetRecordIndex(tem->GenSimCmd_crsm.param1, tem->GenSimCmd_crsm.param2);
					if((UINT16)INVALID_INDEX!= ((CiPbPrimDeletePhonebookEntryReq*)(tem->ci_request))->Index)
					{
						ret = ciRequest_1( ciATciSvgHandle[CI_SG_ID_PB], CI_PB_PRIM_DELETE_PHONEBOOK_ENTRY_REQ,
						tem->crsm_reshandle, (tem->ci_request));
						rc = handleReturn(ret);
						sendCiReq = TRUE;
					}
				}
			}
			if(tem->cci_confirm == CI_PB_PRIM_REPLACE_PHONEBOOK_ENTRY_CNF)
			{
				CI_MEM_FREE(tem->ci_request);
				tem->ci_request = (void*) CI_MEM_ALLOC(sizeof(CiPbPrimReplacePhonebookEntryReq));
				UINT8 AlphaIDLen = tem->GenSimCmd_crsm.length - 14;/*we checked taht the pdu data contain at least 14 bytes, befor the SELECT command*/
#ifndef CCI_SIM_CONTIGUOUS
				if (tem->ci_request!=NULL)
				{
					((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->Entry = (CiPhonebookEntry*)CI_MEM_ALLOC(sizeof(CiPhonebookEntry));
					getString(((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->Entry->Name.Name,
							tem->GenSimCmd_crsm.pData,
							&(((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->Entry->Name.Length));

					((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->Index =	GetRecordIndex(tem->GenSimCmd_crsm.param1, tem->GenSimCmd_crsm.param2);

					if((tem->GenSimCmd_crsm.pData[AlphaIDLen] > 1) && (tem->GenSimCmd_crsm.pData[AlphaIDLen] < 12))
					{
						if(ReadAddressInfo((CiAddressInfo*) &(((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->Entry->Number),(UINT8*)&(tem->GenSimCmd_crsm.pData[AlphaIDLen])) == 0)
							sendCiReq = FALSE;
						else
                        {
							UINT16 iIndex = 0, iExtIndex = 0;
							BOOL bFDNExist = FALSE;
							iExtIndex = tem->GenSimCmd_crsm.pData[tem->GenSimCmd_crsm.length - 1];
							
							//Update FDN table in case this is record with extention
							if( iExtIndex != 0xFF)
							{
								for(iIndex = 0; iIndex<100; iIndex++)
								{
									if(gFDNWithExtTable[iIndex].FDNIndex == ((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->Index)
									{
										gFDNWithExtTable[iIndex].ExtIndexReq = iExtIndex;
										bFDNExist = TRUE;
										iIndex = 100;
									}
								}

								if(!bFDNExist)
								{
									for(iIndex = 0; iIndex<100; iIndex++)
									{
										if(gFDNWithExtTable[iIndex].status == PB_RECORD_EMPTY)
										{
											gFDNWithExtTable[iIndex].status = PB_RECORD_UPDATE;
											gFDNWithExtTable[iIndex].FDNIndex = ((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->Index;
											gFDNWithExtTable[iIndex].ExtIndexReq = iExtIndex;
											iIndex = 100;
										}
									}
								}

								//Add extention informaition to the address 
								for(iIndex = 0; iIndex<100; iIndex++)
								{
									if(gEXT2Table[iIndex].ExtIndexReq == iExtIndex && gEXT2Table[iIndex].status > PB_RECORD_EMPTY && gEXT2Table[iIndex].pRecord[0] == 2 )
									{
										gEXT2Table[iIndex].status = PB_RECORD_UPDATE;
										ReadMoreAddressInfo((CiAddressInfo*) &(((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->Entry->Number),(UINT8*)&(gEXT2Table[iIndex].pRecord[1]));
										iExtIndex = gEXT2Table[iIndex].pRecord[12];
										iIndex = 200;
										while(iIndex == 200 && iExtIndex != 0XFF && ((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->Entry->Number.Length < CI_MAX_ADDRESS_LENGTH)
										{
											for(iIndex = 0; iIndex<100; iIndex++)
											{
												if(gEXT2Table[iIndex].ExtIndexReq == iExtIndex && gEXT2Table[iIndex].status > PB_RECORD_EMPTY && gEXT2Table[iIndex].pRecord[0] == 2 )
												{
													gEXT2Table[iIndex].status = PB_RECORD_UPDATE;
													ReadMoreAddressInfo((CiAddressInfo*) &(((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->Entry->Number),(UINT8*)&(gEXT2Table[iIndex].pRecord[1]));
													iExtIndex = gEXT2Table[iIndex].pRecord[12];
													iIndex = 200;
												}
											}
										}
									}
								}
							}
							sendCiReq = TRUE;
					}
					}
					else
					{
						sendCiReq = FALSE;
					}
#else
				if (tem->ci_request!=NULL)
				{
					getString(((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->entry.Name.Name,
							tem->GenSimCmd_crsm.data,
							&(((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->entry.Name.Length));

					((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->Index =	GetRecordIndex(tem->GenSimCmd_crsm.param1, tem->GenSimCmd_crsm.param2);

					if((tem->GenSimCmd_crsm.data[AlphaIDLen] > 1) && (tem->GenSimCmd_crsm.data[AlphaIDLen] < 12))
					{
						if(ReadAddressInfo((CiAddressInfo*) &(((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->entry.Number),(UINT8*)&(tem->GenSimCmd_crsm.data[AlphaIDLen])) == 0)
							sendCiReq = FALSE;
						else
                        {
							UINT16 iIndex = 0, iExtIndex = 0;
							BOOL bFDNExist = FALSE;
							iExtIndex = tem->GenSimCmd_crsm.data[tem->GenSimCmd_crsm.length - 1];
							
							//Update FDN table in case this is record with extention
							if( iExtIndex != 0xFF)
							{
								for(iIndex = 0; iIndex<100; iIndex++)
								{
									if(gFDNWithExtTable[iIndex].FDNIndex == ((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->Index)
									{
										gFDNWithExtTable[iIndex].ExtIndexReq = iExtIndex;
										bFDNExist = TRUE;
										iIndex = 100;
									}
								}

								if(!bFDNExist)
								{
									for(iIndex = 0; iIndex<100; iIndex++)
									{
										if(gFDNWithExtTable[iIndex].status == PB_RECORD_EMPTY)
										{
											gFDNWithExtTable[iIndex].status = PB_RECORD_UPDATE;
											gFDNWithExtTable[iIndex].FDNIndex = ((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->Index;
											gFDNWithExtTable[iIndex].ExtIndexReq = iExtIndex;
											iIndex = 100;
										}
									}
								}

								//Add extention informaition to the address 
								for(iIndex = 0; iIndex<100; iIndex++)
								{
									if(gEXT2Table[iIndex].ExtIndexReq == iExtIndex && gEXT2Table[iIndex].status > PB_RECORD_EMPTY && gEXT2Table[iIndex].pRecord[0] == 2 )
									{
										gEXT2Table[iIndex].status = PB_RECORD_UPDATE;
										ReadMoreAddressInfo((CiAddressInfo*) &(((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->entry.Number),(UINT8*)&(gEXT2Table[iIndex].pRecord[1]));
										iExtIndex = gEXT2Table[iIndex].pRecord[12];
										while(iExtIndex != 0XFF && ((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->entry.Number.Length < CI_MAX_ADDRESS_LENGTH)
										{
											for(iIndex = 0; iIndex<100; iIndex++)
											{
												if(gEXT2Table[iIndex].ExtIndexReq == iExtIndex && gEXT2Table[iIndex].status > PB_RECORD_EMPTY && gEXT2Table[iIndex].pRecord[0] == 2 )
												{
													gEXT2Table[iIndex].status = PB_RECORD_UPDATE;
													ReadMoreAddressInfo((CiAddressInfo*) &(((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->entry.Number),(UINT8*)&(gEXT2Table[iIndex].pRecord[1]));
													iExtIndex = gEXT2Table[iIndex].pRecord[12];
													iIndex = 100;
												}
											}
										}
										iIndex = 100;
									}
								}
							}
							sendCiReq = TRUE;
						}
					}
					else
					{
						sendCiReq = FALSE;
					}
#endif
					if(sendCiReq == TRUE && (UINT16)INVALID_INDEX!= ((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->Index)
					{
						ret = ciRequest_1( ciATciSvgHandle[CI_SG_ID_PB], CI_PB_PRIM_REPLACE_PHONEBOOK_ENTRY_REQ,
						tem->crsm_reshandle, (tem->ci_request));
						rc = handleReturn(ret);
					}
#ifndef CCI_SIM_CONTIGUOUS
					else
						CI_MEM_FREE(((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->Entry);
#endif
				}
			}
			/*Michal Bukai - Memory and other clean up in case CI reqest was not sen due to wrong data or memory allocation problems */
			if(!sendCiReq)
			{
				genericCmdCnf->rc = CIRC_SIM_FAILURE;
				if (tem!=NULL)
				{
				cnf_args.id = CI_SG_ID_SIM;
				cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
				cnf_args.paras = genericCmdCnf;
				cnf_args.opSgCnfHandle = tem->svshandle;
				cnf_args.reqHandle = tem->crsm_reshandle;
				freeCciActionlist(tem);
				clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
			}
		}
		else
			{
				genericCmdCnf->rc = CIRC_SIM_MEM_PROBLEM;
				if (tem!=NULL)
				{
				cnf_args.id = CI_SG_ID_SIM;
				cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
				cnf_args.paras = genericCmdCnf;
				cnf_args.opSgCnfHandle = tem->svshandle;
				cnf_args.reqHandle = tem->crsm_reshandle;
				freeCciActionlist(tem);
				clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
			}
		break;
	}
#endif
//Michal Bukai - in case of Read SMS operetion, CRSM command will not Mapped to CI_MSG (CQ00056743)
#if 0
	case CI_MSG_PRIM_READ_MESSAGE_CNF:
	{
		CiMsgPrimReadMessageCnf * readPduCnf;
		readPduCnf = (CiMsgPrimReadMessageCnf *)(paras);
		switch ( readPduCnf->rc )
		{
		case CIRC_MSG_SUCCESS:
			{
					genericCmdCnf->rc = CIRC_SIM_OK;

#ifndef CCI_SIM_CONTIGUOUS
					SET_SIM_CMD_LEN_AND_TERM(genericCmdCnf,readPduCnf->pPdu->len+4);  //SW1+SW2
/* Michal Bukai - add patch which map status parameter from CMGR to CRSM  */
#if 0
					genericCmdCnf->pCnf->pData[0] = readPduCnf->status;
#else
					switch(readPduCnf->status)
						{
					case 0:
						genericCmdCnf->pCnf->pData[0] = 3;
						break;
					case 1:
						genericCmdCnf->pCnf->pData[0] = 1;
						break;
					case 2:
						genericCmdCnf->pCnf->pData[0] = 7;
						break;
					case 3:
						genericCmdCnf->pCnf->pData[0] = 5;
						break;
					case 4:
					case 5:
						genericCmdCnf->pCnf->pData[0] = 0;
						break;
					default:
						genericCmdCnf->pCnf->pData[0] = 0xff;
						break;
						}
#endif
					genericCmdCnf->pCnf->pData[1] = 0x00;
                    memcpy(&(genericCmdCnf->pCnf->pData[2]),readPduCnf->pPdu->pData,readPduCnf->pPdu->len);
#else
					if (readPduCnf->pduPresent) {
						SET_SIM_CMD_LEN_AND_TERM(genericCmdCnf,readPduCnf->pdu.len+4);  //SW1+SW2
/* Michal Bukai - add patch which map status parameter from CMGR to CRSM  */
#if 0
					genericCmdCnf->cnf.data[0] = readPduCnf->status;
#else
					switch(readPduCnf->status)
						{
					case 0:
						genericCmdCnf->cnf.data[0] = 3;
						break;
					case 1:
						genericCmdCnf->cnf.data[0] = 1;
						break;
					case 2:
						genericCmdCnf->cnf.data[0] = 7;
						break;
					case 3:
						genericCmdCnf->cnf.data[0] = 5;
						break;
					case 4:
					case 5:
						genericCmdCnf->cnf.data[0] = 0;
						break;
					default:
						genericCmdCnf->cnf.data[0] = 0xff;
						break;
						}
#endif
						genericCmdCnf->cnf.data[1] = 0x00;
						memcpy(&(genericCmdCnf->cnf.data[2]),readPduCnf->pdu.data,readPduCnf->pdu.len);
					} else {
						SET_SIM_CMD_LEN_AND_TERM(genericCmdCnf,2);  //SW1+SW2
					}
#endif

					if (tem!=NULL)
						{
						cnf_args.id = CI_SG_ID_SIM;
						cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
						cnf_args.paras = genericCmdCnf;
						cnf_args.opSgCnfHandle = tem->svshandle;
						cnf_args.reqHandle = tem->crsm_reshandle;
						freeCciActionlist(tem);
						clientCiConfirmCallback(&cnf_args, genericCmdCnf);
						}
					break;
				}

		case CIRC_MSG_INVALID_MEM_INDEX:
			{

				genericCmdCnf->rc = CIRC_SIM_OK;

				SET_SIM_CMD_LEN_AND_TERM(genericCmdCnf,176+2);  //SW1+SW2
#ifndef CCI_SIM_CONTIGUOUS
				genericCmdCnf->pCnf->pData[0]=0x00;
                memset(&(genericCmdCnf->pCnf->pData[1]),EMPTY,genericCmdCnf->pCnf->len-3);
#else
				genericCmdCnf->cnf.data[0]=0x00;
				memset(&(genericCmdCnf->cnf.data[1]),EMPTY,genericCmdCnf->cnf.len-3);
#endif
				if (tem!=NULL)
				{
					cnf_args.id = CI_SG_ID_SIM;
					cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
					cnf_args.paras = genericCmdCnf;
					cnf_args.opSgCnfHandle = tem->svshandle;
					cnf_args.reqHandle = tem->crsm_reshandle;
					freeCciActionlist(tem);
					clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
				break;
			}

		default:
			{
				genericCmdCnf->rc = CIRC_SIM_FAILURE;
				if (tem!=NULL)
					{
					cnf_args.id = CI_SG_ID_SIM;
					cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
					cnf_args.paras = genericCmdCnf;
					cnf_args.opSgCnfHandle = tem->svshandle;
					cnf_args.reqHandle = tem->crsm_reshandle;
					freeCciActionlist(tem);
					clientCiConfirmCallback(&cnf_args, genericCmdCnf);
					}
				break;
				}
			}
		break;
	}
#endif
	case CI_MSG_PRIM_DELETE_MESSAGE_CNF:
	{
		CiMsgPrimDeleteMessageCnf * deleteMsgCnf;
		deleteMsgCnf = (CiMsgPrimDeleteMessageCnf *)(paras);
		if( deleteMsgCnf->rc == CIRC_MSG_SUCCESS )
		{

			genericCmdCnf->rc = CIRC_SIM_OK;
			SET_SIM_CMD_LEN_AND_TERM(genericCmdCnf,2);  //SW1+SW2

			if (tem!=NULL)
				{
				cnf_args.id = CI_SG_ID_SIM;
				cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
				cnf_args.paras = genericCmdCnf;
				cnf_args.opSgCnfHandle = tem->svshandle;
				cnf_args.reqHandle = tem->crsm_reshandle;
				freeCciActionlist(tem);
				clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
			}
		else
		{
			genericCmdCnf->rc = CIRC_SIM_FAILURE;
			if (tem!=NULL)
				{
				cnf_args.id = CI_SG_ID_SIM;
				cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
				cnf_args.paras = genericCmdCnf;
				cnf_args.opSgCnfHandle = tem->svshandle;
				cnf_args.reqHandle = tem->crsm_reshandle;
				freeCciActionlist(tem);
				clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
		}
		break;
	}
	case CI_MSG_PRIM_WRITE_MSG_WITH_INDEX_CNF:
	{
		CiMsgPrimWriteMsgWithIndexCnf* writeMsgWithIndexCnf;
		writeMsgWithIndexCnf = (CiMsgPrimWriteMsgWithIndexCnf*)(paras);
#ifndef CCI_SIM_CONTIGUOUS
		CI_MEM_FREE(((CiMsgPrimWriteMsgWithIndexReq*)(tem->ci_request))->pPdu->pData);
		CI_MEM_FREE(((CiMsgPrimWriteMsgWithIndexReq*)(tem->ci_request))->pPdu);
#endif
		if (writeMsgWithIndexCnf->rc == CIRC_MSG_SUCCESS )
		{
			genericCmdCnf->rc = CIRC_SIM_OK;

			SET_SIM_CMD_LEN_AND_TERM(genericCmdCnf,2);  //SW1+SW2

			if (tem!=NULL)
				{
				cnf_args.id = CI_SG_ID_SIM;
				cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
				cnf_args.paras = genericCmdCnf;
				cnf_args.opSgCnfHandle = tem->svshandle;
				cnf_args.reqHandle = tem->crsm_reshandle;
				freeCciActionlist(tem);
				clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
		}
		else
		{
			genericCmdCnf->rc = CIRC_SIM_FAILURE;
			if (tem!=NULL)
				{
				cnf_args.id = CI_SG_ID_SIM;
				cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
				cnf_args.paras = genericCmdCnf;
				cnf_args.opSgCnfHandle = tem->svshandle;
				cnf_args.reqHandle = tem->crsm_reshandle;
				freeCciActionlist(tem);
				clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
		}
		break;
	}

	case CI_PB_PRIM_READ_PHONEBOOK_ENTRY_CNF:
	{
	CiPbPrimReadPhonebookEntryCnf* readEntryCnf;
	readEntryCnf = (CiPbPrimReadPhonebookEntryCnf*)(paras);
	switch(readEntryCnf->Result)
		{
		case CIRC_PB_SUCCESS:
		{
			if (tem!=NULL)
				{
				UINT8 fdnNumLen;
				BOOL bFDNRecordExistInTable = FALSE;
				UINT16 iIndex, iPBIndex;
#ifndef CCI_SIM_CONTIGUOUS
    			memset(genericCmdCnf->pCnf->pData,EMPTY,gPBMaxAlphaIdLength + 14);
				fdnNumLen = readEntryCnf->Entry->Number.Length;
#else
				memset(genericCmdCnf->cnf.data,EMPTY,gPBMaxAlphaIdLength + 14);
				fdnNumLen = readEntryCnf->entry.Number.Length;
#endif
				if(fdnNumLen > 20)
                {
					for(iIndex = 0; iIndex<100; iIndex++)
					{
						iPBIndex = GetRecordIndex(tem->GenSimCmd_crsm.param1, tem->GenSimCmd_crsm.param2);
						if(gFDNWithExtTable[iIndex].FDNIndex == iPBIndex && gFDNWithExtTable[iIndex].status > PB_RECORD_EMPTY)
						{
#ifndef CCI_SIM_CONTIGUOUS
							genericCmdCnf->pCnf->pData[gPBMaxAlphaIdLength + 13] = (UINT8)gFDNWithExtTable[iIndex].ExtIndexReq;
#else
							genericCmdCnf->cnf.data[gPBMaxAlphaIdLength + 13] = (UINT8)gFDNWithExtTable[iIndex].ExtIndexReq;
#endif
							bFDNRecordExistInTable = TRUE;
							iIndex = 100;
						}
					}

					if(bFDNRecordExistInTable)
                    {    
						fdnNumLen = 20;
					}
					else
					{
						ret = ciRequest_1(tem->svshandle,CI_SIM_PRIM_GENERIC_CMD_REQ,tem->crsm_reshandle,&(tem->GenSimCmd_crsm));
						rc = handleReturn(ret);
					}
				}

				if(fdnNumLen <= 20)
                {
#ifndef CCI_SIM_CONTIGUOUS
					string2BCD(&(genericCmdCnf->pCnf->pData[gPBMaxAlphaIdLength + 2]),readEntryCnf->Entry->Number.Digits,fdnNumLen);
					
					if (fdnNumLen%2 == 0)
						fdnNumLen = fdnNumLen/2 +1;
					else
						fdnNumLen = fdnNumLen/2 +2;

				genericCmdCnf->pCnf->pData[gPBMaxAlphaIdLength]=fdnNumLen;
	
				if(readEntryCnf->Entry->Number.AddrType.NumType == 0x01)
					genericCmdCnf->pCnf->pData[gPBMaxAlphaIdLength + 1]=0x91;
				else
					genericCmdCnf->pCnf->pData[gPBMaxAlphaIdLength + 1]=0x81;
				memcpy(genericCmdCnf->pCnf->pData, readEntryCnf->Entry->Name.Name, readEntryCnf->Entry->Name.Length );
#else
				string2BCD(&(genericCmdCnf->cnf.data[gPBMaxAlphaIdLength + 2]), (char *)readEntryCnf->entry.Number.Digits, fdnNumLen);
				
				if (fdnNumLen%2 == 0)
					fdnNumLen = fdnNumLen/2 +1;
				else
					fdnNumLen = fdnNumLen/2 +2;
				
				genericCmdCnf->cnf.data[gPBMaxAlphaIdLength]= fdnNumLen;
	
				if(readEntryCnf->entry.Number.AddrType.NumType == 0x01)
					genericCmdCnf->cnf.data[gPBMaxAlphaIdLength + 1]=0x91;
				else
					genericCmdCnf->cnf.data[gPBMaxAlphaIdLength + 1]=0x81;
				memcpy(genericCmdCnf->cnf.data, readEntryCnf->entry.Name.Name, readEntryCnf->entry.Name.Length );

#endif

				genericCmdCnf->rc = CIRC_SIM_OK;
				SET_SIM_CMD_LEN_AND_TERM(genericCmdCnf,gPBMaxAlphaIdLength+16);  //SW1+SW2

				cnf_args.id = CI_SG_ID_SIM;
				cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
				cnf_args.paras = genericCmdCnf;
				cnf_args.opSgCnfHandle = tem->svshandle;
				cnf_args.reqHandle = tem->crsm_reshandle;
				freeCciActionlist(tem);
				clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
			}
			break;
		}
		case CIRC_PB_DATA_UNAVAILABLE:
		{
			if (tem!=NULL)
			{
				genericCmdCnf->rc = CIRC_SIM_OK;

				SET_SIM_CMD_LEN_AND_TERM(genericCmdCnf,gPBMaxAlphaIdLength+16);  //SW1+SW2
#ifndef CCI_SIM_CONTIGUOUS
				memset(genericCmdCnf->pCnf->pData,EMPTY,genericCmdCnf->pCnf->len-2);
#else
				memset(genericCmdCnf->cnf.data,EMPTY,genericCmdCnf->cnf.len-2);
#endif

				cnf_args.id = CI_SG_ID_SIM;
				cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
				cnf_args.paras = genericCmdCnf;
				cnf_args.opSgCnfHandle = tem->svshandle;
				cnf_args.reqHandle = tem->crsm_reshandle;
				freeCciActionlist(tem);
				clientCiConfirmCallback(&cnf_args, genericCmdCnf);
			}
			break;
		}
		default:
		{
			genericCmdCnf->rc = CIRC_SIM_FAILURE;
			if (tem!=NULL)
				{
				cnf_args.id = CI_SG_ID_SIM;
				cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
				cnf_args.paras = genericCmdCnf;
				cnf_args.opSgCnfHandle = tem->svshandle;
				cnf_args.reqHandle = tem->crsm_reshandle;
				freeCciActionlist(tem);
				clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
			break;
		}
		
		}
		break;
	}

	case CI_PB_PRIM_DELETE_PHONEBOOK_ENTRY_CNF:
	{
		CiPbPrimDeletePhonebookEntryCnf* deleteEntryCnf;
		deleteEntryCnf = (CiPbPrimDeletePhonebookEntryCnf*)(paras);
		if ( deleteEntryCnf->Result == CIRC_PB_SUCCESS)
		{
			if (tem!=NULL)
				{
				UINT16 iIndex, iPBIndex;
				iPBIndex = GetRecordIndex(tem->GenSimCmd_crsm.param1, tem->GenSimCmd_crsm.param2);
				//update FDN and EXT2 table if necessary:
				for(iIndex = 0; iIndex<100; iIndex++)
				{
					if(gFDNWithExtTable[iIndex].FDNIndex == iPBIndex)
					{
						iPBIndex = gFDNWithExtTable[iIndex].ExtIndexReq;
						gFDNWithExtTable[iIndex].status = PB_RECORD_EMPTY;
						gFDNWithExtTable[iIndex].FDNIndex = 0;
						iPBIndex = gFDNWithExtTable[iIndex].ExtIndexReq;
						gFDNWithExtTable[iIndex].ExtIndexReq = 0;

						for(iIndex = 0; iIndex<100 && iPBIndex > 0 && iPBIndex < 0xff ; iIndex++)
						{
							if(gEXT2Table[iIndex].ExtIndexReq == iPBIndex)
							{
								iPBIndex = gEXT2Table[iIndex].pRecord[12];
								memset(gEXT2Table[iIndex].pRecord,EMPTY,13);
								iIndex = 200;
								while(iPBIndex > 0 && iPBIndex < 0xff && iIndex == 200)
								{
									for(iIndex = 0; iIndex<100 ; iIndex++)
									{
										if(gEXT2Table[iIndex].ExtIndexReq == iPBIndex)
										{
											iPBIndex = gEXT2Table[iIndex].pRecord[12];
											memset(gEXT2Table[iIndex].pRecord,EMPTY,13);
											iIndex = 200;
										}
									}
								}
							}
						}
						iIndex = 100;
					}
				}

				genericCmdCnf->rc = CIRC_SIM_OK;

				SET_SIM_CMD_LEN_AND_TERM(genericCmdCnf,2);  //SW1+SW2
				
				cnf_args.id = CI_SG_ID_SIM;
				cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
				cnf_args.paras = genericCmdCnf;
				cnf_args.opSgCnfHandle = tem->svshandle;
				cnf_args.reqHandle = tem->crsm_reshandle;
				freeCciActionlist(tem);
				clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
		}
		/* Michal Bukai - added in order to support PB failur in case PIN2 required*/
		else if ( deleteEntryCnf->Result == CIRC_PB_FDN_OPER_NOT_ALLOWED)
		{
			if (tem!=NULL)
			{
				CI_MEM_FREE(tem->ci_request);
				tem->ci_request = (void*) CI_MEM_ALLOC(sizeof(CiSimPrimDeviceStatusReq));
				if (tem->ci_request!=NULL)
				{
					ret = ciRequest_1( ciATciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_DEVICE_STATUS_REQ,
						tem->crsm_reshandle, (tem->ci_request));
					rc = handleReturn(ret);
				}
				else
				{	
					genericCmdCnf->rc = CIRC_SIM_FAILURE;
					if (tem!=NULL)
						{
						cnf_args.id = CI_SG_ID_SIM;
						cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
						cnf_args.paras = genericCmdCnf;
						cnf_args.opSgCnfHandle = tem->svshandle;
						cnf_args.reqHandle = tem->crsm_reshandle;
						freeCciActionlist(tem);
						clientCiConfirmCallback(&cnf_args, genericCmdCnf);
						}
				}		
			}
		}
		else
		{	
			genericCmdCnf->rc = CIRC_SIM_FAILURE;
			if (tem!=NULL)
				{
				cnf_args.id = CI_SG_ID_SIM;
				cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
				cnf_args.paras = genericCmdCnf;
				cnf_args.opSgCnfHandle = tem->svshandle;
				cnf_args.reqHandle = tem->crsm_reshandle;
				freeCciActionlist(tem);
				clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
		}		
		break;
	}


	case CI_PB_PRIM_REPLACE_PHONEBOOK_ENTRY_CNF:
	{
		CiPbPrimReplacePhonebookEntryCnf* replaceEntryCnf;
		replaceEntryCnf = (CiPbPrimReplacePhonebookEntryCnf*)(paras);
#ifndef CCI_SIM_CONTIGUOUS
		CI_MEM_FREE(((CiPbPrimReplacePhonebookEntryReq*)(tem->ci_request))->Entry);
#endif
		if ( replaceEntryCnf->Result == CIRC_PB_SUCCESS)
		{
			if (tem!=NULL)
				{
				genericCmdCnf->rc = CIRC_SIM_OK;

				SET_SIM_CMD_LEN_AND_TERM(genericCmdCnf,2);
				
				cnf_args.id = CI_SG_ID_SIM;
				cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
				cnf_args.paras = genericCmdCnf;
				cnf_args.opSgCnfHandle = tem->svshandle;
				cnf_args.reqHandle = tem->crsm_reshandle;
				freeCciActionlist(tem);
				clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
		}
		/* Michal Bukai - added in order to support PB failur in case PIN2 required*/
		else if ( replaceEntryCnf->Result == CIRC_PB_FDN_OPER_NOT_ALLOWED)
		{
			if (tem!=NULL)
			{
				CI_MEM_FREE(tem->ci_request);
				tem->ci_request = (void*) CI_MEM_ALLOC(sizeof(CiSimPrimDeviceStatusReq));
				if (tem->ci_request!=NULL)
				{
					ret = ciRequest_1( ciATciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_DEVICE_STATUS_REQ,
						tem->crsm_reshandle, (tem->ci_request));
					rc = handleReturn(ret);
				}
				else
				{	
					genericCmdCnf->rc = CIRC_SIM_FAILURE;
					if (tem!=NULL)
						{
						cnf_args.id = CI_SG_ID_SIM;
						cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
						cnf_args.paras = genericCmdCnf;
						cnf_args.opSgCnfHandle = tem->svshandle;
						cnf_args.reqHandle = tem->crsm_reshandle;
						freeCciActionlist(tem);
						clientCiConfirmCallback(&cnf_args, genericCmdCnf);
						}
				}		
			}
		}
		else
		{	
			genericCmdCnf->rc = CIRC_SIM_FAILURE;
			if (tem!=NULL)
				{
				cnf_args.id = CI_SG_ID_SIM;
				cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
				cnf_args.paras = genericCmdCnf;
				cnf_args.opSgCnfHandle = tem->svshandle;
				cnf_args.reqHandle = tem->crsm_reshandle;
				freeCciActionlist(tem);
				clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
		}
		break;
	}
	case CI_SIM_PRIM_DEVICE_STATUS_CNF:
	{
		CiSimPrimDeviceStatusCnf* deviceStatusCnf;
		deviceStatusCnf = (CiSimPrimDeviceStatusCnf*)(paras);

		if ( deviceStatusCnf->rc == CIRC_SIM_OK )
		{
			//Map PIN State to SIM Result Code 
			switch(deviceStatusCnf->pinState)
			{
			case CI_SIM_PIN_ST_CHV1_REQUIRED:
			case CI_SIM_PIN_ST_UNIVERSALPIN_REQUIRED:
				{
					genericCmdCnf->rc = CIRC_SIM_PIN_REQUIRED;
					break;
				}
			case CI_SIM_PIN_ST_CHV2_REQUIRED:
				{
					genericCmdCnf->rc = CIRC_SIM_PIN2_REQUIRED;
					break;
				}
			case CI_SIM_PIN_ST_UNBLOCK_CHV1_REQUIRED:
			case CI_SIM_PIN_ST_UNBLOCK_UNIVERSALPIN_REQUIRED:
				{
					genericCmdCnf->rc = CIRC_SIM_PUK_REQUIRED;
					break;
				}
			case CI_SIM_PIN_ST_UNBLOCK_CHV2_REQUIRED:
				{
					genericCmdCnf->rc = CIRC_SIM_PUK2_REQUIRED;
					break;
				}
			case CI_SIM_PIN_ST_READY:/*should not happen - we get to this confirmation CI_SIM_PRIM_DEVICE_STATUS_CNF - after replace/delete PB operation fail due to CIRC_PB_FDN_OPER_NOT_ALLOWED*/
				{
					genericCmdCnf->rc = CIRC_SIM_OK;
					break;
				}
                
			default:
				{
					genericCmdCnf->rc = CIRC_SIM_UNKNOWN;;
					break;
				}
			}

			if (tem!=NULL)
			{
				cnf_args.id = CI_SG_ID_SIM;
				cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
				cnf_args.paras = genericCmdCnf;
				cnf_args.opSgCnfHandle = tem->svshandle;
				cnf_args.reqHandle = tem->crsm_reshandle;
				freeCciActionlist(tem);
				clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
		}
		else
		{	
			genericCmdCnf->rc = CIRC_SIM_FAILURE;
			if (tem!=NULL)
				{
				cnf_args.id = CI_SG_ID_SIM;
				cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
				cnf_args.paras = genericCmdCnf;
				cnf_args.opSgCnfHandle = tem->svshandle;
				cnf_args.reqHandle = tem->crsm_reshandle;
				freeCciActionlist(tem);
				clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
		}
		break;
	}

	case CI_PB_PRIM_GET_PHONEBOOK_INFO_CNF:
	{
		CiPbPrimGetPhonebookInfoCnf* pbInfo;
		pbInfo = (CiPbPrimGetPhonebookInfoCnf*)(paras);

		if ( pbInfo->Result == CIRC_PB_SUCCESS )
		{
			sendCiReq = FALSE;
#ifndef CCI_SIM_CONTIGUOUS
			gPBMaxAlphaIdLength = pbInfo->Info->MaxAlphaIdLength;
#else
			gPBMaxAlphaIdLength = pbInfo->info.MaxAlphaIdLength;
#endif
			CI_MEM_FREE(tem->ci_request);
			tem->ci_request = (void*) CI_MEM_ALLOC(sizeof(CiPbPrimReadPhonebookEntryReq));
			if (tem->ci_request!=NULL)
			{
				((CiPbPrimReadPhonebookEntryReq*)(tem->ci_request))->Index =
					GetRecordIndex(tem->GenSimCmd_crsm.param1, tem->GenSimCmd_crsm.param2);
				if((UINT16)INVALID_INDEX!=((CiPbPrimReadPhonebookEntryReq*)(tem->ci_request))->Index)
				{
					ret = ciRequest_1( ciATciSvgHandle[CI_SG_ID_PB], CI_PB_PRIM_READ_PHONEBOOK_ENTRY_REQ,
					tem->crsm_reshandle, (tem->ci_request));
					rc = handleReturn(ret);
					sendCiReq = TRUE;
				}
			}
			if(!sendCiReq) 
			{ 
				genericCmdCnf->rc = CIRC_SIM_FAILURE;
				if (tem!=NULL)
				{
				cnf_args.id = CI_SG_ID_SIM;
				cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
				cnf_args.paras = genericCmdCnf;
				cnf_args.opSgCnfHandle = tem->svshandle;
				cnf_args.reqHandle = tem->crsm_reshandle;
				freeCciActionlist(tem);
				clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
			} 
		}
		else
		{	
			genericCmdCnf->rc = CIRC_SIM_FAILURE;
			if (tem!=NULL)
				{
				cnf_args.id = CI_SG_ID_SIM;
				cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
				cnf_args.paras = genericCmdCnf;
				cnf_args.opSgCnfHandle = tem->svshandle;
				cnf_args.reqHandle = tem->crsm_reshandle;
				freeCciActionlist(tem);
				clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
		}
		break;
	}

	case CI_SIM_PRIM_GENERIC_CMD_CNF:
	{
		CiSimPrimGenericCmdCnf * tmpGenericCmdCnf;
		tmpGenericCmdCnf = (CiSimPrimGenericCmdCnf*)(paras);
		if ( tmpGenericCmdCnf->rc == CIRC_SIM_OK)
		{
			genericCmdCnf->rc = CIRC_SIM_OK;
			if (tem!=NULL)
			{
				if(tem->cci_confirm == CI_PB_PRIM_READ_PHONEBOOK_ENTRY_CNF)
				{
#ifndef CCI_SIM_CONTIGUOUS
					memcpy(genericCmdCnf->pCnf->pData, tmpGenericCmdCnf->pCnf->pData,tmpGenericCmdCnf->pCnf->len );
					genericCmdCnf->pCnf->len = tmpGenericCmdCnf->pCnf->len;
#else
					memcpy(genericCmdCnf->cnf.data, tmpGenericCmdCnf->cnf.data, tmpGenericCmdCnf->cnf.len );
					genericCmdCnf->cnf.len = tmpGenericCmdCnf->cnf.len;
#endif
					cnf_args.id = CI_SG_ID_SIM;
					cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
					cnf_args.paras = genericCmdCnf;
					cnf_args.opSgCnfHandle = tem->svshandle;
					cnf_args.reqHandle = tem->crsm_reshandle;
					freeCciActionlist(tem);
				    clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
				else if(tem->cci_confirm == EXT2_READ_OPER)
				{
					iPBIndex = GetRecordIndex(tem->GenSimCmd_crsm.param1, tem->GenSimCmd_crsm.param2);

#ifndef CCI_SIM_CONTIGUOUS
					memcpy(genericCmdCnf->pCnf->pData, tmpGenericCmdCnf->pCnf->pData,tmpGenericCmdCnf->pCnf->len );
					genericCmdCnf->pCnf->len = tmpGenericCmdCnf->pCnf->len;
#else
					memcpy(genericCmdCnf->cnf.data, tmpGenericCmdCnf->cnf.data, tmpGenericCmdCnf->cnf.len );
					genericCmdCnf->cnf.len = tmpGenericCmdCnf->cnf.len;
#endif
					
					for(iIndex = 0; iIndex<100; iIndex++)
					{
						if(gEXT2Table[iIndex].status > PB_RECORD_EMPTY && gEXT2Table[iIndex].ExtIndexReq == iPBIndex)
						{
#ifndef CCI_SIM_CONTIGUOUS
							memset(genericCmdCnf->pCnf->pData,EMPTY,13);
							memcpy(genericCmdCnf->pCnf->pData, gEXT2Table[iIndex].pRecord, 13 );
#else
							memset(genericCmdCnf->cnf.data,EMPTY,13);
							memcpy(genericCmdCnf->cnf.data, gEXT2Table[iIndex].pRecord, 13 );
#endif
							
							SET_SIM_CMD_LEN_AND_TERM(genericCmdCnf,15);  //SW1+SW2
							iIndex = 100;
						}
					}
					cnf_args.id = CI_SG_ID_SIM;
					cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
					cnf_args.paras = genericCmdCnf;
					cnf_args.opSgCnfHandle = tem->svshandle;
					cnf_args.reqHandle = tem->crsm_reshandle;
					freeCciActionlist(tem);
					clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
				else if(tem->cci_confirm == EXT2_UPDATE_OPER)
				{
					SET_SIM_CMD_LEN_AND_TERM(genericCmdCnf,2);  //SW1+SW2
					cnf_args.id = CI_SG_ID_SIM;
					cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
					cnf_args.paras = genericCmdCnf;
					cnf_args.opSgCnfHandle = tem->svshandle;
					cnf_args.reqHandle = tem->crsm_reshandle;
					freeCciActionlist(tem);
					clientCiConfirmCallback(&cnf_args, genericCmdCnf);
				}
			}
		}
		else
        {
			genericCmdCnf->rc = CIRC_SIM_FAILURE;
			if (tem!=NULL)
			{
				cnf_args.id = CI_SG_ID_SIM;
				cnf_args.primId = CI_SIM_PRIM_GENERIC_CMD_CNF;
				cnf_args.paras = genericCmdCnf;
				cnf_args.opSgCnfHandle = tem->svshandle;
				cnf_args.reqHandle = tem->crsm_reshandle;
				freeCciActionlist(tem);
				clientCiConfirmCallback(&cnf_args, genericCmdCnf);
			}
		}
		break;
	}

}
}

}



void clientCiIndicateCallback(CiIndicateArgs* pArg, void *paras)
{
	CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_API, "clientCiIndicateCallback()");

#ifdef CI_DATA_DUMP
	ciDumpMsgInfo( pArg->id, pArg->primId, pArg->indHandle, paras, 0);
#endif
	/* call real callback function that register by RIL */
	if ( gCiIndicateFuncPtr[pArg->id] != NULL)
	{
		gCiIndicateFuncPtr[pArg->id](pArg->opSgIndHandle, pArg->id, pArg->primId, pArg->indHandle, paras);

	}
	else
	{
		CCI_TRACE1(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_ERROR, " CiInicateCallback Function Pointer Error, id=%d", pArg->id);
		CCI_ASSERT(FALSE);
	}
}


void clientCiDefIndicateCallback(CiIndicateArgs* pArg, void *paras)
{
	CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_API, "clientCiDefIndicateCallback()");

	/* call real callback function that register by RIL */
	if (gCiDefIndicateFuncPtr != NULL)
	{
		gCiDefIndicateFuncPtr(pArg->opSgIndHandle, pArg->id, pArg->primId, pArg->indHandle, paras);
	}
	else
	{
		CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_INFO, "No CiDefInicateCallback Function Pointer defined");

		/* Just free the allocated memory */
		//cimem_CiSgFreeMem(pArg->opSgIndHandle, pArg->id, pArg->primId, pArg->paras);
	}
}


CiServiceGroupID findCiSgId(CiServiceHandle handle)
{
	UINT8 i;

	for (i = 0; i < CI_SG_ID_NEXTAVAIL; i++)
	{
		if (handle == gCiServiceHandle[i])
		{
			return i;
		}
	}

	return 0;
}

/* static stub call-back that is going to be called by the CI stub task for free CI Svc Group request parameter */
void clientCiSgFreeReqMem(CiRequestArgs *pCiRequestArgs)
{
  CiServiceGroupID id;

    id = findCiSgId(pCiRequestArgs->handle);
    if (id == 0)
    {
       CCI_TRACE(CI_CLIENT_STUB, CI_CLIENT, __LINE__, CCI_TRACE_ERROR," Can't Find Ci Service group FreeReqMem Function ");
       return;
    }
    if (gCiSgFreeReqMemFuncTable[id]) {
        gCiSgFreeReqMemFuncTable[id](gCiSgOpaqueReqHandle[id],
                                     id,
                                     pCiRequestArgs->primId,
                                     pCiRequestArgs->paras);
    }

}


#ifdef CI_DATA_DUMP

#define CI_DUMP_DATA_SIZE_MAX   0x80
/**
 *ciDumpMsgInfo
 *@brief this function is used to dump the message send or receive over ACI interface added by Rovin Yu.
 *@para
 *  srvGrpId: service group Id
 *  primId: primitive Id
 *  reqHandle: unique handle to identify session (can be reqHandle, indHandle, ...)
 *  data:  the pointer to the data
 *  datasize: the size of data
 *@return void
 */
void ciDumpMsgInfo(UINT8 srvGrpId, UINT16 primId, UINT32 reqHandle,  UINT8 *data, UINT32 dataSize)
{
	static char *sSrvGroupName[] = {
		"",
		"CI_SG_ID_CC",
		"CI_SG_ID_SS",
		"CI_SG_ID_MM",
		"CI_SG_ID_PB",
		"CI_SG_ID_SIM",
		"CI_SG_ID_MSG",
		"CI_SG_ID_PS",
		"CI_SG_ID_DAT",
		"CI_SG_ID_DEV",
		"CI_SG_ID_HSCSD",
		"CI_SG_ID_DEB",
		"CI_SG_ID_ATPI",
		"CI_SG_ID_PL",
		"CI_SG_ID_OAM",
		"CI_SG_ID_NEXTAVAIL"
	};

	static char *sCsMsgName[] = {
		"",
		"CI_CC_PRIM_GET_NUMBERTYPE_REQ",
		"CI_CC_PRIM_GET_NUMBERTYPE_CNF",
		"CI_CC_PRIM_SET_NUMBERTYPE_REQ",
		"CI_CC_PRIM_SET_NUMBERTYPE_CNF",
		"CI_CC_PRIM_GET_SUPPORTED_CALLMODES_REQ",
		"CI_CC_PRIM_GET_SUPPORTED_CALLMODES_CNF",
		"CI_CC_PRIM_GET_CALLMODE_REQ",
		"CI_CC_PRIM_GET_CALLMODE_CNF",
		"CI_CC_PRIM_SET_CALLMODE_REQ",
		"CI_CC_PRIM_SET_CALLMODE_CNF",
		"CI_CC_PRIM_GET_SUPPORTED_DATA_BSTYPES_REQ",
		"CI_CC_PRIM_GET_SUPPORTED_DATA_BSTYPES_CNF",
		"CI_CC_PRIM_GET_DATA_BSTYPE_REQ",
		"CI_CC_PRIM_GET_DATA_BSTYPE_CNF",
		"CI_CC_PRIM_SET_DATA_BSTYPE_REQ",
		"CI_CC_PRIM_SET_DATA_BSTYPE_CNF",
		"CI_CC_PRIM_GET_AUTOANSWER_ACTIVE_REQ",
		"CI_CC_PRIM_GET_AUTOANSWER_ACTIVE_CNF",
		"CI_CC_PRIM_SET_AUTOANSWER_ACTIVE_REQ",
		"CI_CC_PRIM_SET_AUTOANSWER_ACTIVE_CNF",
		"CI_CC_PRIM_LIST_CURRENT_CALLS_REQ",
		"CI_CC_PRIM_LIST_CURRENT_CALLS_CNF",
		"CI_CC_PRIM_GET_CALLINFO_REQ",
		"CI_CC_PRIM_GET_CALLINFO_CNF",
		"CI_CC_PRIM_MAKE_CALL_REQ",
		"CI_CC_PRIM_MAKE_CALL_CNF",
		"CI_CC_PRIM_CALL_PROCEEDING_IND",
		"CI_CC_PRIM_MO_CALL_FAILED_IND",
		"CI_CC_PRIM_ALERTING_IND",
		"CI_CC_PRIM_CONNECT_IND",
		"CI_CC_PRIM_DISCONNECT_IND",
		"CI_CC_PRIM_INCOMING_CALL_IND",
		"CI_CC_PRIM_CALL_WAITING_IND",
		"CI_CC_PRIM_HELD_CALL_IND",
		"CI_CC_PRIM_ANSWER_CALL_REQ",
		"CI_CC_PRIM_ANSWER_CALL_CNF",
		"CI_CC_PRIM_REFUSE_CALL_REQ",
		"CI_CC_PRIM_REFUSE_CALL_CNF",
		"CI_CC_PRIM_MT_CALL_FAILED_IND",
		"CI_CC_PRIM_HOLD_CALL_REQ",
		"CI_CC_PRIM_HOLD_CALL_CNF",
		"CI_CC_PRIM_RETRIEVE_CALL_REQ",
		"CI_CC_PRIM_RETRIEVE_CALL_CNF",
		"CI_CC_PRIM_SWITCH_ACTIVE_HELD_REQ",
		"CI_CC_PRIM_SWITCH_ACTIVE_HELD_CNF",
		"CI_CC_PRIM_CALL_DEFLECT_REQ",
		"CI_CC_PRIM_CALL_DEFLECT_CNF",
		"CI_CC_PRIM_EXPLICIT_CALL_TRANSFER_REQ",
		"CI_CC_PRIM_EXPLICIT_CALL_TRANSFER_CNF",
		"CI_CC_PRIM_RELEASE_CALL_REQ",
		"CI_CC_PRIM_RELEASE_CALL_CNF",
		"CI_CC_PRIM_RELEASE_ALL_CALLS_REQ",
		"CI_CC_PRIM_RELEASE_ALL_CALLS_CNF",
		"CI_CC_PRIM_SWITCH_CALLMODE_REQ",
		"CI_CC_PRIM_SWITCH_CALLMODE_CNF",
		"CI_CC_PRIM_ESTABLISH_MPTY_REQ",
		"CI_CC_PRIM_ESTABLISH_MPTY_CNF",
		"CI_CC_PRIM_ADD_TO_MPTY_REQ",
		"CI_CC_PRIM_ADD_TO_MPTY_CNF",
		"CI_CC_PRIM_HOLD_MPTY_REQ",
		"CI_CC_PRIM_HOLD_MPTY_CNF",
		"CI_CC_PRIM_RETRIEVE_MPTY_REQ",
		"CI_CC_PRIM_RETRIEVE_MPTY_CNF",
		"CI_CC_PRIM_SPLIT_FROM_MPTY_REQ",
		"CI_CC_PRIM_SPLIT_FROM_MPTY_CNF",
		"CI_CC_PRIM_SHUTTLE_MPTY_REQ",
		"CI_CC_PRIM_SHUTTLE_MPTY_CNF",
		"CI_CC_PRIM_RELEASE_MPTY_REQ",
		"CI_CC_PRIM_RELEASE_MPTY_CNF",
		"CI_CC_PRIM_START_DTMF_REQ",
		"CI_CC_PRIM_START_DTMF_CNF",
		"CI_CC_PRIM_STOP_DTMF_REQ",
		"CI_CC_PRIM_STOP_DTMF_CNF",
		"CI_CC_PRIM_GET_DTMF_PACING_REQ",
		"CI_CC_PRIM_GET_DTMF_PACING_CNF",
		"CI_CC_PRIM_SET_DTMF_PACING_REQ",
		"CI_CC_PRIM_SET_DTMF_PACING_CNF",
		"CI_CC_PRIM_SEND_DTMF_STRING_REQ",
		"CI_CC_PRIM_SEND_DTMF_STRING_CNF",
		"CI_CC_PRIM_CLIP_INFO_IND",
		"CI_CC_PRIM_COLP_INFO_IND",
		"CI_CC_PRIM_CCM_UPDATE_IND",
		"CI_CC_PRIM_GET_CCM_VALUE_REQ",
		"CI_CC_PRIM_GET_CCM_VALUE_CNF",
		"CI_CC_PRIM_AOC_WARNING_IND",
		"CI_CC_PRIM_SSI_NOTIFY_IND",
		"CI_CC_PRIM_SSU_NOTIFY_IND",
		"CI_CC_PRIM_LOCALCB_NOTIFY_IND",
		"CI_CC_PRIM_GET_ACM_VALUE_REQ",
		"CI_CC_PRIM_GET_ACM_VALUE_CNF",
		"CI_CC_PRIM_RESET_ACM_VALUE_REQ",
		"CI_CC_PRIM_RESET_ACM_VALUE_CNF",
		"CI_CC_PRIM_GET_ACMMAX_VALUE_REQ",
		"CI_CC_PRIM_GET_ACMMAX_VALUE_CNF",
		"CI_CC_PRIM_SET_ACMMAX_VALUE_REQ",
		"CI_CC_PRIM_SET_ACMMAX_VALUE_CNF",
		"CI_CC_PRIM_GET_PUCT_INFO_REQ",
		"CI_CC_PRIM_GET_PUCT_INFO_CNF",
		"CI_CC_PRIM_SET_PUCT_INFO_REQ",
		"CI_CC_PRIM_SET_PUCT_INFO_CNF",
		"CI_CC_PRIM_GET_BASIC_CALLMODES_REQ",
		"CI_CC_PRIM_GET_BASIC_CALLMODES_CNF",
		"CI_CC_PRIM_GET_CALLOPTIONS_REQ",
		"CI_CC_PRIM_GET_CALLOPTIONS_CNF",
		"CI_CC_PRIM_GET_DATACOMP_CAP_REQ",
		"CI_CC_PRIM_GET_DATACOMP_CAP_CNF",
		"CI_CC_PRIM_GET_DATACOMP_REQ",
		"CI_CC_PRIM_GET_DATACOMP_CNF",
		"CI_CC_PRIM_SET_DATACOMP_REQ",
		"CI_CC_PRIM_SET_DATACOMP_CNF",
		"CI_CC_PRIM_GET_RLP_CAP_REQ",
		"CI_CC_PRIM_GET_RLP_CAP_CNF",
		"CI_CC_PRIM_GET_RLP_CFG_REQ",
		"CI_CC_PRIM_GET_RLP_CFG_CNF",
		"CI_CC_PRIM_SET_RLP_CFG_REQ",
		"CI_CC_PRIM_SET_RLP_CFG_CNF",
		"CI_CC_PRIM_DATA_SERVICENEG_IND",
		"CI_CC_PRIM_ENABLE_DATA_SERVICENEG_IND_REQ",
		"CI_CC_PRIM_ENABLE_DATA_SERVICENEG_IND_CNF",
		"CI_CC_PRIM_SET_UDUB_REQ",
		"CI_CC_PRIM_SET_UDUB_CNF",
		"CI_CC_PRIM_GET_SUPPORTED_CALLMAN_OPS_REQ",
		"CI_CC_PRIM_GET_SUPPORTED_CALLMAN_OPS_CNF",
		"CI_CC_PRIM_MANIPULATE_CALLS_REQ",
		"CI_CC_PRIM_MANIPULATE_CALLS_CNF",
		"CI_CC_PRIM_LIST_CURRENT_CALLS_IND",

		"CI_CC_PRIM_CALL_DIAGNOSTIC_IND",
		"CI_CC_PRIM_DTMF_EVENT_IND",
		"CI_CC_PRIM_CLEAR_BLACK_LIST_REQ",
		"CI_CC_PRIM_CLEAR_BLACK_LIST_CNF",
		"CI_CC_PRIM_SET_CTM_STATUS_REQ",
		"CI_CC_PRIM_SET_CTM_STATUS_CNF",

		"CI_CC_PRIM_CTM_NEG_REPORT_IND",

		"CI_CC_PRIM_CDIP_INFO_IND",
#ifdef AT_CUSTOMER_HTC
		"CI_CC_PRIM_SET_FW_LIST_REQ",
		"CI_CC_PRIM_SET_FW_LIST_CNF",
#endif
		"CI_MSG_PRIM_LOCK_SMS_STATUS_REQ",
		"CI_MSG_PRIM_LOCK_SMS_STATUS_CNF",
		/* ADD NEW COMMON PRIMITIVES HERE, BEFORE 'CI_CC_PRIM_LAST_COMMON_PRIM' */

		/* END OF COMMON PRIMITIVES LIST */
		"CI_CC_PRIM_LAST_COMMON_PRIM"

	};

	static char *sPsMsgName[] = {
		"",
		"CI_PS_PRIM_SET_ATTACH_STATE_REQ",
		"CI_PS_PRIM_SET_ATTACH_STATE_CNF",
		"CI_PS_PRIM_GET_ATTACH_STATE_REQ",
		"CI_PS_PRIM_GET_ATTACH_STATE_CNF",
		"CI_PS_PRIM_DEFINE_PDP_CTX_REQ",
		"CI_PS_PRIM_DEFINE_PDP_CTX_CNF",
		"CI_PS_PRIM_DELETE_PDP_CTX_REQ",
		"CI_PS_PRIM_DELETE_PDP_CTX_CNF",
		"CI_PS_PRIM_GET_PDP_CTX_REQ",
		"CI_PS_PRIM_GET_PDP_CTX_CNF",
		"CI_PS_PRIM_GET_PDP_CTX_CAPS_REQ",
		"CI_PS_PRIM_GET_PDP_CTX_CAPS_CNF",
		"CI_PS_PRIM_SET_PDP_CTX_ACT_STATE_REQ",
		"CI_PS_PRIM_SET_PDP_CTX_ACT_STATE_CNF",
		"CI_PS_PRIM_GET_PDP_CTXS_ACT_STATE_REQ",
		"CI_PS_PRIM_GET_PDP_CTXS_ACT_STATE_CNF",
		"CI_PS_PRIM_ENTER_DATA_STATE_REQ",
		"CI_PS_PRIM_ENTER_DATA_STATE_CNF",
		"CI_PS_PRIM_MT_PDP_CTX_ACT_MODIFY_IND",
		"CI_PS_PRIM_MT_PDP_CTX_ACT_MODIFY_RSP",
		"CI_PS_PRIM_MT_PDP_CTX_ACTED_IND",
		"CI_PS_PRIM_SET_GSMGPRS_CLASS_REQ",
		"CI_PS_PRIM_SET_GSMGPRS_CLASS_CNF",
		"CI_PS_PRIM_GET_GSMGPRS_CLASS_REQ",
		"CI_PS_PRIM_GET_GSMGPRS_CLASS_CNF",
		"CI_PS_PRIM_GET_GSMGPRS_CLASSES_REQ",
		"CI_PS_PRIM_GET_GSMGPRS_CLASSES_CNF",
		"CI_PS_PRIM_ENABLE_NW_REG_IND_REQ",
		"CI_PS_PRIM_ENABLE_NW_REG_IND_CNF",
		"CI_PS_PRIM_NW_REG_IND",
		"CI_PS_PRIM_SET_QOS_REQ",
		"CI_PS_PRIM_SET_QOS_CNF",
		"CI_PS_PRIM_DEL_QOS_REQ",
		"CI_PS_PRIM_DEL_QOS_CNF",
		"CI_PS_PRIM_GET_QOS_REQ",
		"CI_PS_PRIM_GET_QOS_CNF",
		"CI_PS_PRIM_ENABLE_POWERON_AUTO_ATTACH_REQ",
		"CI_PS_PRIM_ENABLE_POWERON_AUTO_ATTACH_CNF",
		"CI_PS_PRIM_MT_PDP_CTX_REJECTED_IND",
		"CI_PS_PRIM_PDP_CTX_DEACTED_IND",
		"CI_PS_PRIM_PDP_CTX_REACTED_IND",
		"CI_PS_PRIM_DETACHED_IND",
		"CI_PS_PRIM_GPRS_CLASS_CHANGED_IND",
		"CI_PS_PRIM_GET_DEFINED_CID_LIST_REQ",
		"CI_PS_PRIM_GET_DEFINED_CID_LIST_CNF",
		"CI_PS_PRIM_GET_NW_REG_STATUS_REQ",
		"CI_PS_PRIM_GET_NW_REG_STATUS_CNF",
		"CI_PS_PRIM_GET_QOS_CAPS_REQ",
		"CI_PS_PRIM_GET_QOS_CAPS_CNF",
		"CI_PS_PRIM_ENABLE_EVENTS_REPORTING_REQ",
		"CI_PS_PRIM_ENABLE_EVENTS_REPORTING_CNF",

		/* SCR #1401348: 3G Quality of Service (QoS) primitives */
		"CI_PS_PRIM_GET_3G_QOS_REQ",
		"CI_PS_PRIM_GET_3G_QOS_CNF",
		"CI_PS_PRIM_SET_3G_QOS_REQ",
		"CI_PS_PRIM_SET_3G_QOS_CNF",
		"CI_PS_PRIM_DEL_3G_QOS_REQ",
		"CI_PS_PRIM_DEL_3G_QOS_CNF",
		"CI_PS_PRIM_GET_3G_QOS_CAPS_REQ",
		"CI_PS_PRIM_GET_3G_QOS_CAPS_CNF",

		/* SCR #1438547: Secondary PDP Context primitives */
		"CI_PS_PRIM_DEFINE_SEC_PDP_CTX_REQ",
		"CI_PS_PRIM_DEFINE_SEC_PDP_CTX_CNF",
		"CI_PS_PRIM_DELETE_SEC_PDP_CTX_REQ",
		"CI_PS_PRIM_DELETE_SEC_PDP_CTX_CNF",
		"CI_PS_PRIM_GET_SEC_PDP_CTX_REQ",
		"CI_PS_PRIM_GET_SEC_PDP_CTX_CNF",

		/* SCR #1438547: Traffic Flow Template (TFT) primitives */
		"CI_PS_PRIM_DEFINE_TFT_FILTER_REQ",
		"CI_PS_PRIM_DEFINE_TFT_FILTER_CNF",
		"CI_PS_PRIM_DELETE_TFT_REQ",
		"CI_PS_PRIM_DELETE_TFT_CNF",
		"CI_PS_PRIM_GET_TFT_REQ",
		"CI_PS_PRIM_GET_TFT_CNF",

		/* SCR TBD: PDP Context Modify primitives */
		"CI_PS_PRIM_MODIFY_PDP_CTX_REQ",
		"CI_PS_PRIM_MODIFY_PDP_CTX_CNF",
		"CI_PS_PRIM_GET_ACTIVE_CID_LIST_REQ",
		"CI_PS_PRIM_GET_ACTIVE_CID_LIST_CNF",

		"CI_PS_PRIM_REPORT_COUNTER_REQ",
		"CI_PS_PRIM_REPORT_COUNTER_CNF",
		"CI_PS_PRIM_RESET_COUNTER_REQ",
		"CI_PS_PRIM_RESET_COUNTER_CNF",
		"CI_PS_PRIM_COUNTER_IND",

		"CI_PS_PRIM_SEND_DATA_REQ",
		"CI_PS_PRIM_SEND_DATA_CNF",

		"CI_PS_PRIM_SET_ACL_SERVICE_REQ",
		"CI_PS_PRIM_SET_ACL_SERVICE_CNF",
		"CI_PS_PRIM_GET_ACL_SIZE_REQ",
		"CI_PS_PRIM_GET_ACL_SIZE_CNF",
		"CI_PS_PRIM_READ_ACL_ENTRY_REQ",
		"CI_PS_PRIM_READ_ACL_ENTRY_CNF",
		"CI_PS_PRIM_EDIT_ACL_ENTRY_REQ",
		"CI_PS_PRIM_EDIT_ACL_ENTRY_CNF",
		"CI_PS_PRIM_AUTHENTICATE_REQ",
		"CI_PS_PRIM_AUTHENTICATE_CNF",
		"CI_PS_PRIM_FAST_DORMANT_REQ",
		"CI_PS_PRIM_FAST_DORMANT_CNF",

		"CI_PS_PRIM_GET_CURRENT_JOB_REQ",
		"CI_PS_PRIM_GET_CURRENT_JOB_CNF",
#ifdef AT_CUSTOMER_HTC
		"CI_PS_PRIM_SET_FAST_DORMANCY_CONFIG_REQ",
		"CI_PS_PRIM_SET_FAST_DORMANCY_CONFIG_CNF",

		"CI_PS_PRIM_PDP_ACTIVATION_REJECT_CAUSE_IND",
#endif
		/* ADD NEW COMMON PRIMITIVES HERE, BEFORE 'CI_PS_PRIM_LAST_COMMON_PRIM' */
		/* END OF COMMON PRIMITIVES LIST */
		"CI_PS_PRIM_LAST_COMMON_PRIM"
	};

	static char *sSmsMsgName[] = {
		"",
		"CI_MSG_PRIM_GET_SUPPORTED_SERVICES_REQ",
		"CI_MSG_PRIM_GET_SUPPORTED_SERVICES_CNF",
		"CI_MSG_PRIM_SELECT_SERVICE_REQ",
		"CI_MSG_PRIM_SELECT_SERVICE_CNF",
		"CI_MSG_PRIM_GET_CURRENT_SERVICE_INFO_REQ",
		"CI_MSG_PRIM_GET_CURRENT_SERVICE_INFO_CNF",
		"CI_MSG_PRIM_GET_SUPPORTED_STORAGES_REQ",
		"CI_MSG_PRIM_GET_SUPPORTED_STORAGES_CNF",
		"CI_MSG_PRIM_SELECT_STORAGE_REQ",
		"CI_MSG_PRIM_SELECT_STORAGE_CNF",
		"CI_MSG_PRIM_GET_CURRENT_STORAGE_INFO_REQ",
		"CI_MSG_PRIM_GET_CURRENT_STORAGE_INFO_CNF",
		"CI_MSG_PRIM_READ_MESSAGE_REQ",
		"CI_MSG_PRIM_READ_MESSAGE_CNF",
		"CI_MSG_PRIM_DELETE_MESSAGE_REQ",
		"CI_MSG_PRIM_DELETE_MESSAGE_CNF",
		"CI_MSG_PRIM_SEND_MESSAGE_REQ",
		"CI_MSG_PRIM_SEND_MESSAGE_CNF",
		"CI_MSG_PRIM_WRITE_MESSAGE_REQ",
		"CI_MSG_PRIM_WRITE_MESSAGE_CNF",
		"CI_MSG_PRIM_SEND_COMMAND_REQ",
		"CI_MSG_PRIM_SEND_COMMAND_CNF",
		"CI_MSG_PRIM_SEND_STORED_MESSAGE_REQ",
		"CI_MSG_PRIM_SEND_STORED_MESSAGE_CNF",
		"CI_MSG_PRIM_CONFIG_MSG_IND_REQ",
		"CI_MSG_PRIM_CONFIG_MSG_IND_CNF",
		"CI_MSG_PRIM_NEWMSG_INDEX_IND",
		"CI_MSG_PRIM_NEWMSG_IND",
		"CI_MSG_PRIM_NEWMSG_RSP",
		"CI_MSG_PRIM_GET_SMSC_ADDR_REQ",
		"CI_MSG_PRIM_GET_SMSC_ADDR_CNF",
		"CI_MSG_PRIM_SET_SMSC_ADDR_REQ",
		"CI_MSG_PRIM_SET_SMSC_ADDR_CNF",
		"CI_MSG_PRIM_GET_MOSMS_SERVICE_CAP_REQ",
		"CI_MSG_PRIM_GET_MOSMS_SERVICE_CAP_CNF",
		"CI_MSG_PRIM_GET_MOSMS_SERVICE_REQ",
		"CI_MSG_PRIM_GET_MOSMS_SERVICE_CNF",
		"CI_MSG_PRIM_SET_MOSMS_SERVICE_REQ",
		"CI_MSG_PRIM_SET_MOSMS_SERVICE_CNF",
		"CI_MSG_PRIM_GET_CBM_TYPES_CAP_REQ",
		"CI_MSG_PRIM_GET_CBM_TYPES_CAP_CNF",
		"CI_MSG_PRIM_GET_CBM_TYPES_REQ",
		"CI_MSG_PRIM_GET_CBM_TYPES_CNF",
		"CI_MSG_PRIM_SET_CBM_TYPES_REQ",
		"CI_MSG_PRIM_SET_CBM_TYPES_CNF",
		"CI_MSG_PRIM_SELECT_STORAGES_REQ",
		"CI_MSG_PRIM_SELECT_STORAGES_CNF",

		"CI_MSG_PRIM_WRITE_MSG_WITH_INDEX_REQ",
		"CI_MSG_PRIM_WRITE_MSG_WITH_INDEX_CNF",

		"CI_MSG_PRIM_MESSAGE_WAITING_IND",
		"CI_MSG_PRIM_STORAGE_STATUS_IND",

		/*Michal Bukai - SMS Memory Full Feature - start*/
		"CI_MSG_PRIM_RESET_MEMCAP_FULL_REQ",
		"CI_MSG_PRIM_RESET_MEMCAP_FULL_CNF",
		/*Michal Bukai - SMS Memory Full Feature - end*/

		"CI_MSG_PRIM_SET_RECORD_STATUS_REQ",
		"CI_MSG_PRIM_SET_RECORD_STATUS_CNF",
#ifdef AT_CUSTOMER_HTC
		"CI_MSG_PRIM_SMS_REJECT_CAUSE_IND",
#endif
		/* ADD NEW COMMON PRIMITIVES HERE, BEFORE 'CI_MSG_PRIM_LAST_COMMON_PRIM' */

		/* END OF COMMON PRIMITIVES LIST */
		"CI_MSG_PRIM_LAST_COMMON_PRIM"
	};

	static char *sPbMsgName[] = {
		"",
		"CI_PB_PRIM_GET_SUPPORTED_PHONEBOOKS_REQ",
		"CI_PB_PRIM_GET_SUPPORTED_PHONEBOOKS_CNF",
		"CI_PB_PRIM_GET_PHONEBOOK_INFO_REQ",
		"CI_PB_PRIM_GET_PHONEBOOK_INFO_CNF",
		"CI_PB_PRIM_SELECT_PHONEBOOK_REQ",
		"CI_PB_PRIM_SELECT_PHONEBOOK_CNF",
		"CI_PB_PRIM_READ_FIRST_PHONEBOOK_ENTRY_REQ",
		"CI_PB_PRIM_READ_FIRST_PHONEBOOK_ENTRY_CNF",
		"CI_PB_PRIM_READ_NEXT_PHONEBOOK_ENTRY_REQ",
		"CI_PB_PRIM_READ_NEXT_PHONEBOOK_ENTRY_CNF",
		"CI_PB_PRIM_FIND_FIRST_PHONEBOOK_ENTRY_REQ",
		"CI_PB_PRIM_FIND_FIRST_PHONEBOOK_ENTRY_CNF",
		"CI_PB_PRIM_FIND_NEXT_PHONEBOOK_ENTRY_REQ",
		"CI_PB_PRIM_FIND_NEXT_PHONEBOOK_ENTRY_CNF",
		"CI_PB_PRIM_ADD_PHONEBOOK_ENTRY_REQ",
		"CI_PB_PRIM_ADD_PHONEBOOK_ENTRY_CNF",
		"CI_PB_PRIM_DELETE_PHONEBOOK_ENTRY_REQ",
		"CI_PB_PRIM_DELETE_PHONEBOOK_ENTRY_CNF",
		"CI_PB_PRIM_REPLACE_PHONEBOOK_ENTRY_REQ",
		"CI_PB_PRIM_REPLACE_PHONEBOOK_ENTRY_CNF",
		"CI_PB_PRIM_READ_PHONEBOOK_ENTRY_REQ",
		"CI_PB_PRIM_READ_PHONEBOOK_ENTRY_CNF",
		"CI_PB_PRIM_GET_IDENTITY_REQ",
		"CI_PB_PRIM_GET_IDENTITY_CNF",
		"CI_PB_PRIM_PHONEBOOK_READY_IND",
		"CI_PB_PRIM_GET_ALPHA_NAME_REQ",
		"CI_PB_PRIM_GET_ALPHA_NAME_CNF",

/* Michal Bukai & Boris Tsatkin  AT&T Smart Card support - Start */
/*AT&T- Smart Card  <CDR-SMCD-911> PB	   ENHANCED_PHONEBOOK_ENTRY -BT1*/
		"CI_PB_PRIM_READ_ENH_PBK_ENTRY_REQ",
		"CI_PB_PRIM_READ_ENH_PBK_ENTRY_CNF",
		"CI_PB_PRIM_EDIT_ENH_PBK_ENTRY_REQ",
		"CI_PB_PRIM_EDIT_ENH_PBK_ENTRY_CNF",

/*AT&T- Smart Card	<CDR-SMCD-941> PB	  CI_PB CategoryName- EFAas -BT2
 */
		"CI_PB_PRIM_READ_CATEGORY_NAME_REQ",
		"CI_PB_PRIM_READ_CATEGORY_NAME_CNF",
		"CI_PB_PRIM_EDIT_CATEGORY_NAME_REQ",
		"CI_PB_PRIM_EDIT_CATEGORY_NAME_CNF",

/*AT&T- Smart Card	<CDR-SMCD-941> PB	  CI_PB CategoryName- EFAas -BT3 */
		"CI_PB_PRIM_READ_GROUP_NAME_REQ",
		"CI_PB_PRIM_READ_GROUP_NAME_CNF",
		"CI_PB_PRIM_EDIT_GROUP_NAME_REQ",
		"CI_PB_PRIM_EDIT_GROUP_NAME_CNF",
/* Michal Bukai & Boris Tsatkin  AT&T Smart Card support - End*/


		/* ADD NEW COMMON PRIMITIVES HERE, BEFORE 'CI_PB_PRIM_LAST_COMMON_PRIM' */

		/* END OF COMMON PRIMITIVES LIST */
		"CI_PB_PRIM_LAST_COMMON_PRIM"
	};

	static char *sSimMsgName[] = {
		"",
		"CI_SIM_PRIM_EXECCMD_REQ",
		"CI_SIM_PRIM_EXECCMD_CNF",
		"CI_SIM_PRIM_DEVICE_IND",
		"CI_SIM_PRIM_PERSONALIZEME_REQ",
		"CI_SIM_PRIM_PERSONALIZEME_CNF",
		"CI_SIM_PRIM_OPERCHV_REQ",
		"CI_SIM_PRIM_OPERCHV_CNF",
		"CI_SIM_PRIM_DOWNLOADPROFILE_REQ",
		"CI_SIM_PRIM_DOWNLOADPROFILE_CNF",
		"CI_SIM_PRIM_ENDATSESSION_IND",
		"CI_SIM_PRIM_PROACTIVE_CMD_IND",
		"CI_SIM_PRIM_PROACTIVE_CMD_RSP",
		"CI_SIM_PRIM_ENVELOPE_CMD_REQ",
		"CI_SIM_PRIM_ENVELOPE_CMD_CNF",
		"CI_SIM_PRIM_GET_SUBSCRIBER_ID_REQ",
		"CI_SIM_PRIM_GET_SUBSCRIBER_ID_CNF",
		"CI_SIM_PRIM_GET_PIN_STATE_REQ",
		"CI_SIM_PRIM_GET_PIN_STATE_CNF",
		"CI_SIM_PRIM_GET_TERMINALPROFILE_REQ",
		"CI_SIM_PRIM_GET_TERMINALPROFILE_CNF",
		"CI_SIM_PRIM_ENABLE_SIMAT_INDS_REQ",
		"CI_SIM_PRIM_ENABLE_SIMAT_INDS_CNF",
		"CI_SIM_PRIM_LOCK_FACILITY_REQ",
		"CI_SIM_PRIM_LOCK_FACILITY_CNF",
		"CI_SIM_PRIM_GET_FACILITY_CAP_REQ",
		"CI_SIM_PRIM_GET_FACILITY_CAP_CNF",
		"CI_SIM_PRIM_GET_SIMAT_NOTIFY_CAP_REQ",
		"CI_SIM_PRIM_GET_SIMAT_NOTIFY_CAP_CNF",
		"CI_SIM_PRIM_GET_CALL_SETUP_ACK_IND",
		"CI_SIM_PRIM_GET_CALL_SETUP_ACK_RSP",

		/* Service Provider Name */
		"CI_SIM_PRIM_GET_SERVICE_PROVIDER_NAME_REQ",
		"CI_SIM_PRIM_GET_SERVICE_PROVIDER_NAME_CNF",

		/* Message Waiting Information */
		"CI_SIM_PRIM_GET_MESSAGE_WAITING_INFO_REQ",
		"CI_SIM_PRIM_GET_MESSAGE_WAITING_INFO_CNF",
		"CI_SIM_PRIM_SET_MESSAGE_WAITING_INFO_REQ",
		"CI_SIM_PRIM_SET_MESSAGE_WAITING_INFO_CNF",

		/* SIM Service Table */
		"CI_SIM_PRIM_GET_SIM_SERVICE_TABLE_REQ",
		"CI_SIM_PRIM_GET_SIM_SERVICE_TABLE_CNF",

		/* CPHS Customer Service Profile */
		"CI_SIM_PRIM_GET_CUSTOMER_SERVICE_PROFILE_REQ",
		"CI_SIM_PRIM_GET_CUSTOMER_SERVICE_PROFILE_CNF",

		/* Display Alpha and Icon Identifiers */
		"CI_SIM_PRIM_SIMAT_DISPLAY_INFO_IND",

		/* Default Language */
		"CI_SIM_PRIM_GET_DEFAULT_LANGUAGE_REQ",
		"CI_SIM_PRIM_GET_DEFAULT_LANGUAGE_CNF",

		/* Generic SIM commands */
		"CI_SIM_PRIM_GENERIC_CMD_REQ",
		"CI_SIM_PRIM_GENERIC_CMD_CNF",

		/* Indication of card type", status and PIN state */
		"CI_SIM_PRIM_CARD_IND",

		"CI_SIM_PRIM_IS_EMERGENCY_NUMBER_REQ",
		"CI_SIM_PRIM_IS_EMERGENCY_NUMBER_CNF",

		"CI_SIM_PRIM_SIM_OWNED_IND",
		"CI_SIM_PRIM_SIM_CHANGED_IND",

		"CI_SIM_PRIM_SET_VSIM_REQ",
		"CI_SIM_PRIM_SET_VSIM_CNF",
		"CI_SIM_PRIM_GET_VSIM_REQ",
		"CI_SIM_PRIM_GET_VSIM_CNF",

/*Michal Bukai - OTA support for AT&T - START*/
		"CI_SIM_PRIM_CHECK_MMI_STATE_IND",
		"CI_SIM_PRIM_CHECK_MMI_STATE_RSP",
/*Michal Bukai - OTA support for AT&T - END*/
/*Michal Bukai - additional SIMAT primitives - START*/
/*new add five IND */
		"CI_SIM_PRIM_SIMAT_CC_STATUS_IND",
		"CI_SIM_PRIM_SIMAT_SEND_CALL_SETUP_RSP_IND",
		"CI_SIM_PRIM_SIMAT_SEND_SS_USSD_RSP_IND",
		"CI_SIM_PRIM_SIMAT_SM_CONTROL_STATUS_IND",
		"CI_SIM_PRIM_SIMAT_SEND_SM_RSP_IND",
/*Michal Bukai - additional SIMAT primitives - END*/


		/* ADD NEW COMMON PRIMITIVES HERE, BEFORE 'CI_SIM_PRIM_LAST_COMMON_PRIM' */

		/* END OF COMMON PRIMITIVES LIST */
		"CI_SIM_PRIM_LAST_COMMON_PRIM"
	};

	static char *sSsMsgName[] = {
		"",
		"CI_SS_PRIM_GET_CLIP_STATUS_REQ",
		"CI_SS_PRIM_GET_CLIP_STATUS_CNF",
		"CI_SS_PRIM_SET_CLIP_OPTION_REQ",
		"CI_SS_PRIM_SET_CLIP_OPTION_CNF",
		"CI_SS_PRIM_GET_CLIR_STATUS_REQ",
		"CI_SS_PRIM_GET_CLIR_STATUS_CNF",
		"CI_SS_PRIM_SET_CLIR_OPTION_REQ",
		"CI_SS_PRIM_SET_CLIR_OPTION_CNF",
		"CI_SS_PRIM_GET_COLP_STATUS_REQ",
		"CI_SS_PRIM_GET_COLP_STATUS_CNF",
		"CI_SS_PRIM_SET_COLP_OPTION_REQ",
		"CI_SS_PRIM_SET_COLP_OPTION_CNF",
		"CI_SS_PRIM_GET_CUG_CONFIG_REQ",
		"CI_SS_PRIM_GET_CUG_CONFIG_CNF",
		"CI_SS_PRIM_SET_CUG_CONFIG_REQ",
		"CI_SS_PRIM_SET_CUG_CONFIG_CNF",
		"CI_SS_PRIM_GET_CNAP_STATUS_REQ",               //Michal Bukai - CNAP support
		"CI_SS_PRIM_GET_CNAP_STATUS_CNF",               //Michal Bukai - CNAP support
		"CI_SS_PRIM_SET_CNAP_OPTION_REQ",               //Michal Bukai - CNAP support
		"CI_SS_PRIM_SET_CNAP_OPTION_CNF",               //Michal Bukai - CNAP support
		"CI_SS_PRIM_REGISTER_CF_INFO_REQ",
		"CI_SS_PRIM_REGISTER_CF_INFO_CNF",
		"CI_SS_PRIM_ERASE_CF_INFO_REQ",
		"CI_SS_PRIM_ERASE_CF_INFO_CNF",
		"CI_SS_PRIM_GET_CF_REASONS_REQ",
		"CI_SS_PRIM_GET_CF_REASONS_CNF",
		"CI_SS_PRIM_SET_CF_ACTIVATION_REQ",
		"CI_SS_PRIM_SET_CF_ACTIVATION_CNF",
		"CI_SS_PRIM_INTERROGATE_CF_INFO_REQ",
		"CI_SS_PRIM_INTERROGATE_CF_INFO_CNF",
		"CI_SS_PRIM_SET_CW_ACTIVATION_REQ",
		"CI_SS_PRIM_SET_CW_ACTIVATION_CNF",
		"CI_SS_PRIM_GET_CW_OPTION_REQ",
		"CI_SS_PRIM_GET_CW_OPTION_CNF",
		"CI_SS_PRIM_SET_CW_OPTION_REQ",
		"CI_SS_PRIM_SET_CW_OPTION_CNF",
		"CI_SS_PRIM_GET_CW_ACTIVE_STATUS_REQ",
		"CI_SS_PRIM_GET_CW_ACTIVE_STATUS_CNF",
		"CI_SS_PRIM_GET_USSD_ENABLE_REQ",
		"CI_SS_PRIM_GET_USSD_ENABLE_CNF",
		"CI_SS_PRIM_SET_USSD_ENABLE_REQ",
		"CI_SS_PRIM_SET_USSD_ENABLE_CNF",
		"CI_SS_PRIM_RECEIVED_USSD_INFO_IND",
		"CI_SS_PRIM_RECEIVED_USSD_INFO_RSP",
		"CI_SS_PRIM_START_USSD_SESSION_REQ",
		"CI_SS_PRIM_START_USSD_SESSION_CNF",
		"CI_SS_PRIM_ABORT_USSD_SESSION_REQ",
		"CI_SS_PRIM_ABORT_USSD_SESSION_CNF",
		"CI_SS_PRIM_GET_CCM_OPTION_REQ",
		"CI_SS_PRIM_GET_CCM_OPTION_CNF",
		"CI_SS_PRIM_SET_CCM_OPTION_REQ",
		"CI_SS_PRIM_SET_CCM_OPTION_CNF",
		"CI_SS_PRIM_GET_AOC_WARNING_ENABLE_REQ",
		"CI_SS_PRIM_GET_AOC_WARNING_ENABLE_CNF",
		"CI_SS_PRIM_SET_AOC_WARNING_ENABLE_REQ",
		"CI_SS_PRIM_SET_AOC_WARNING_ENABLE_CNF",
		"CI_SS_PRIM_GET_SS_NOTIFY_OPTIONS_REQ",
		"CI_SS_PRIM_GET_SS_NOTIFY_OPTIONS_CNF",
		"CI_SS_PRIM_SET_SS_NOTIFY_OPTIONS_REQ",
		"CI_SS_PRIM_SET_SS_NOTIFY_OPTIONS_CNF",
		"CI_SS_PRIM_GET_LOCALCB_LOCKS_REQ",
		"CI_SS_PRIM_GET_LOCALCB_LOCKS_CNF",
		"CI_SS_PRIM_GET_LOCALCB_LOCK_ACTIVE_REQ",
		"CI_SS_PRIM_GET_LOCALCB_LOCK_ACTIVE_CNF",
		"CI_SS_PRIM_SET_LOCALCB_LOCK_ACTIVE_REQ",
		"CI_SS_PRIM_SET_LOCALCB_LOCK_ACTIVE_CNF",
		"CI_SS_PRIM_SET_LOCALCB_NOTIFY_OPTION_REQ",
		"CI_SS_PRIM_SET_LOCALCB_NOTIFY_OPTION_CNF",
		"CI_SS_PRIM_CHANGE_CB_PASSWORD_REQ",
		"CI_SS_PRIM_CHANGE_CB_PASSWORD_CNF",
		"CI_SS_PRIM_GET_CB_STATUS_REQ",
		"CI_SS_PRIM_GET_CB_STATUS_CNF",
		"CI_SS_PRIM_SET_CB_ACTIVATE_REQ",
		"CI_SS_PRIM_SET_CB_ACTIVATE_CNF",
		"CI_SS_PRIM_GET_CB_TYPES_REQ",
		"CI_SS_PRIM_GET_CB_TYPES_CNF",
		"CI_SS_PRIM_GET_BASIC_SVC_CLASSES_REQ",
		"CI_SS_PRIM_GET_BASIC_SVC_CLASSES_CNF",
		"CI_SS_PRIM_GET_ACTIVE_CW_CLASSES_REQ",
		"CI_SS_PRIM_GET_ACTIVE_CW_CLASSES_CNF",

		"CI_SS_PRIM_GET_CB_MAP_STATUS_REQ",
		"CI_SS_PRIM_GET_CB_MAP_STATUS_CNF",

#if defined(CCI_POSITION_LOCATION)
		"CI_SS_PRIM_PRIVACY_CTRL_REG_REQ",
		"CI_SS_PRIM_PRIVACY_CTRL_REG_CNF",
		"CI_SS_PRIM_LOCATION_IND",
		"CI_SS_PRIM_LOCATION_VERIFY_RSP",
		"CI_SS_PRIM_GET_LOCATION_REQ",
		"CI_SS_PRIM_GET_LOCATION_CNF",
		"CI_SS_PRIM_GET_LCS_NWSTATE_REQ",
		"CI_SS_PRIM_GET_LCS_NWSTATE_CNF",
		"CI_SS_PRIM_LCS_NWSTATE_CFG_IND_REQ",
		"CI_SS_PRIM_LCS_NWSTATE_CFG_IND_CNF",
		"CI_SS_PRIM_LCS_NWSTATE_IND",
#endif          /* CCI_POSITION_LOCATION */

		"CI_SS_PRIM_SERVICE_REQUEST_COMPLETE_IND",

		"CI_SS_PRIM_GET_COLR_STATUS_REQ",
		"CI_SS_PRIM_GET_COLR_STATUS_CNF",
		"CI_SS_PRIM_GET_CDIP_STATUS_REQ",
		"CI_SS_PRIM_GET_CDIP_STATUS_CNF",
		"CI_SS_PRIM_SET_CDIP_OPTION_REQ",
		"CI_SS_PRIM_SET_CDIP_OPTION_CNF",


		/* ADD NEW COMMON PRIMITIVES HERE, BEFORE 'CI_SS_PRIM_LAST_COMMON_PRIM' */

		/* END OF COMMON PRIMITIVES LIST */
		"CI_SS_PRIM_LAST_COMMON_PRIM"
	};

	static char *sMmMsgName[] = {
		"",
		"CI_MM_PRIM_GET_NUM_SUBSCRIBER_NUMBERS_REQ",
		"CI_MM_PRIM_GET_NUM_SUBSCRIBER_NUMBERS_CNF",
		"CI_MM_PRIM_GET_SUBSCRIBER_INFO_REQ",
		"CI_MM_PRIM_GET_SUBSCRIBER_INFO_CNF",
		"CI_MM_PRIM_GET_SUPPORTED_REGRESULT_OPTIONS_REQ",
		"CI_MM_PRIM_GET_SUPPORTED_REGRESULT_OPTIONS_CNF",
		"CI_MM_PRIM_GET_REGRESULT_OPTION_REQ",
		"CI_MM_PRIM_GET_REGRESULT_OPTION_CNF",
		"CI_MM_PRIM_SET_REGRESULT_OPTION_REQ",
		"CI_MM_PRIM_SET_REGRESULT_OPTION_CNF",
		"CI_MM_PRIM_REGRESULT_IND",
		"CI_MM_PRIM_GET_REGRESULT_INFO_REQ",
		"CI_MM_PRIM_GET_REGRESULT_INFO_CNF",
		"CI_MM_PRIM_GET_SUPPORTED_ID_FORMATS_REQ",
		"CI_MM_PRIM_GET_SUPPORTED_ID_FORMATS_CNF",
		"CI_MM_PRIM_GET_ID_FORMAT_REQ",
		"CI_MM_PRIM_GET_ID_FORMAT_CNF",
		"CI_MM_PRIM_SET_ID_FORMAT_REQ",
		"CI_MM_PRIM_SET_ID_FORMAT_CNF",
		"CI_MM_PRIM_GET_NUM_NETWORK_OPERATORS_REQ",
		"CI_MM_PRIM_GET_NUM_NETWORK_OPERATORS_CNF",
		"CI_MM_PRIM_GET_NETWORK_OPERATOR_INFO_REQ",
		"CI_MM_PRIM_GET_NETWORK_OPERATOR_INFO_CNF",
		"CI_MM_PRIM_GET_NUM_PREFERRED_OPERATORS_REQ",
		"CI_MM_PRIM_GET_NUM_PREFERRED_OPERATORS_CNF",
		"CI_MM_PRIM_GET_PREFERRED_OPERATOR_INFO_REQ",
		"CI_MM_PRIM_GET_PREFERRED_OPERATOR_INFO_CNF",
		"CI_MM_PRIM_ADD_PREFERRED_OPERATOR_REQ",
		"CI_MM_PRIM_ADD_PREFERRED_OPERATOR_CNF",
		"CI_MM_PRIM_DELETE_PREFERRED_OPERATOR_REQ",
		"CI_MM_PRIM_DELETE_PREFERRED_OPERATOR_CNF",
		"CI_MM_PRIM_GET_CURRENT_OPERATOR_INFO_REQ",
		"CI_MM_PRIM_GET_CURRENT_OPERATOR_INFO_CNF",
		"CI_MM_PRIM_AUTO_REGISTER_REQ",
		"CI_MM_PRIM_AUTO_REGISTER_CNF",
		"CI_MM_PRIM_MANUAL_REGISTER_REQ",
		"CI_MM_PRIM_MANUAL_REGISTER_CNF",
		"CI_MM_PRIM_DEREGISTER_REQ",
		"CI_MM_PRIM_DEREGISTER_CNF",
		"CI_MM_PRIM_GET_SIGQUALITY_IND_CONFIG_REQ",
		"CI_MM_PRIM_GET_SIGQUALITY_IND_CONFIG_CNF",
		"CI_MM_PRIM_SET_SIGQUALITY_IND_CONFIG_REQ",
		"CI_MM_PRIM_SET_SIGQUALITY_IND_CONFIG_CNF",
		"CI_MM_PRIM_SIGQUALITY_INFO_IND",

// SCR #1401348
		"CI_MM_PRIM_ENABLE_NETWORK_MODE_IND_REQ",
		"CI_MM_PRIM_ENABLE_NETWORK_MODE_IND_CNF",
		"CI_MM_PRIM_NETWORK_MODE_IND",
		"CI_MM_PRIM_GET_NITZ_INFO_REQ",
		"CI_MM_PRIM_GET_NITZ_INFO_CNF",
		"CI_MM_PRIM_NITZ_INFO_IND",

		"CI_MM_PRIM_CIPHERING_STATUS_IND",
		"CI_MM_PRIM_AIR_INTERFACE_REJECT_CAUSE_IND",

		/* Michal Bukai - Selection of preferred PLMN list +CPLS - START */
		"CI_MM_PRIM_SELECT_PREFERRED_PLMN_LIST_REQ",
		"CI_MM_PRIM_SELECT_PREFERRED_PLMN_LIST_CNF",
		"CI_MM_PRIM_GET_PREFERRED_PLMN_LIST_REQ",
		"CI_MM_PRIM_GET_PREFERRED_PLMN_LIST_CNF",
		/* Michal Bukai - Selection of preferred PLMN list +CPLS - END */
		"CI_MM_PRIM_BANDIND_IND",
		"CI_MM_PRIM_SET_BANDIND_REQ",
		"CI_MM_PRIM_SET_BANDIND_CNF",
		"CI_MM_PRIM_GET_BANDIND_REQ",
		"CI_MM_PRIM_GET_BANDIND_CNF",

		/* ADD NEW COMMON PRIMITIVES HERE, BEFORE 'CI_MM_PRIM_LAST_COMMON_PRIM' */
		"CI_MM_PRIM_SERVICE_RESTRICTIONS_IND",

		"CI_MM_PRIM_CELL_LOCK_REQ",
		"CI_MM_PRIM_CELL_LOCK_CNF",
		"CI_MM_PRIM_CELL_LOCK_IND",

		/* END OF COMMON PRIMITIVES LIST */
		"CI_MM_PRIM_LAST_COMMON_PRIM"
	};

	static char *sDevMsgName[] = {
		"",
		"CI_DEV_PRIM_STATUS_IND",
		"CI_DEV_PRIM_GET_MANU_ID_REQ",
		"CI_DEV_PRIM_GET_MANU_ID_CNF",
		"CI_DEV_PRIM_GET_MODEL_ID_REQ",
		"CI_DEV_PRIM_GET_MODEL_ID_CNF",
		"CI_DEV_PRIM_GET_REVISION_ID_REQ",
		"CI_DEV_PRIM_GET_REVISION_ID_CNF",
		"CI_DEV_PRIM_GET_SERIALNUM_ID_REQ",
		"CI_DEV_PRIM_GET_SERIALNUM_ID_CNF",
		"CI_DEV_PRIM_SET_FUNC_REQ",
		"CI_DEV_PRIM_SET_FUNC_CNF",
		"CI_DEV_PRIM_GET_FUNC_REQ",
		"CI_DEV_PRIM_GET_FUNC_CNF",
		"CI_DEV_PRIM_GET_FUNC_CAP_REQ",
		"CI_DEV_PRIM_GET_FUNC_CAP_CNF",
		"CI_DEV_PRIM_SET_GSM_POWER_CLASS_REQ",
		"CI_DEV_PRIM_SET_GSM_POWER_CLASS_CNF",
		"CI_DEV_PRIM_GET_GSM_POWER_CLASS_REQ",
		"CI_DEV_PRIM_GET_GSM_POWER_CLASS_CNF",
		"CI_DEV_PRIM_GET_GSM_POWER_CLASS_CAP_REQ",
		"CI_DEV_PRIM_GET_GSM_POWER_CLASS_CAP_CNF",
		"CI_DEV_PRIM_PM_POWER_DOWN_REQ",
		"CI_DEV_PRIM_PM_POWER_DOWN_CNF",
		"CI_DEV_PRIM_SET_ENGMODE_REPORT_OPTION_REQ",
		"CI_DEV_PRIM_SET_ENGMODE_REPORT_OPTION_CNF",
		"CI_DEV_PRIM_GET_ENGMODE_INFO_REQ",
		"CI_DEV_PRIM_GET_ENGMODE_INFO_CNF",
		"CI_DEV_PRIM_ENGMODE_INFO_IND",
		"CI_DEV_PRIM_GSM_ENGMODE_INFO_IND",
		"CI_DEV_PRIM_UMTS_ENGMODE_SVCCELL_INFO_IND",
		"CI_DEV_PRIM_UMTS_ENGMODE_INTRAFREQ_INFO_IND",
		"CI_DEV_PRIM_UMTS_ENGMODE_INTERFREQ_INFO_IND",
		"CI_DEV_PRIM_UMTS_ENGMODE_INTERRAT_INFO_IND",
		"CI_DEV_PRIM_DO_SELF_TEST_REQ",
		"CI_DEV_PRIM_DO_SELF_TEST_CNF",
		"CI_DEV_PRIM_DO_SELF_TEST_IND",
		"CI_DEV_PRIM_SET_RFS_REQ",
		"CI_DEV_PRIM_SET_RFS_CNF",
		"CI_DEV_PRIM_GET_RFS_REQ",
		"CI_DEV_PRIM_GET_RFS_CNF",
		"CI_DEV_PRIM_UMTS_ENGMODE_ACTIVE_SET_INFO_IND",
		"CI_DEV_PRIM_ACTIVE_PDP_CONTEXT_ENGMODE_IND",
		"CI_DEV_PRIM_NETWORK_MONITOR_INFO_IND",
		"CI_DEV_PRIM_LP_NWUL_MSG_REQ",
		"CI_DEV_PRIM_LP_NWUL_MSG_CNF",
		"CI_DEV_PRIM_LP_NWDL_MSG_IND",
		"CI_DEV_PRIM_LP_RRC_STATE_IND",
		"CI_DEV_PRIM_LP_MEAS_TERMINATE_IND",
		"CI_DEV_PRIM_LP_RESET_STORED_UE_POS_IND",
		"CI_DEV_PRIM_COMM_ASSERT_REQ",

		"CI_DEV_PRIM_SET_BAND_MODE_REQ",
		"CI_DEV_PRIM_SET_BAND_MODE_CNF",
		"CI_DEV_PRIM_GET_BAND_MODE_REQ",
		"CI_DEV_PRIM_GET_BAND_MODE_CNF",
		"CI_DEV_PRIM_GET_SUPPORTED_BAND_MODE_REQ",
		"CI_DEV_PRIM_GET_SUPPORTED_BAND_MODE_CNF",
		"CI_DEV_PRIM_SET_SV_REQ",
		"CI_DEV_PRIM_SET_SV_CNF",
		"CI_DEV_PRIM_GET_SV_REQ",
		"CI_DEV_PRIM_GET_SV_CNF",
		"CI_DEV_PRIM_AP_POWER_NOTIFY_REQ",
	 	"CI_DEV_PRIM_AP_POWER_NOTIFY_CNF",
		"CI_DEV_PRIM_SET_TD_MODE_TX_RX_REQ",
		"CI_DEV_PRIM_SET_TD_MODE_TX_RX_CNF",
		"CI_DEV_PRIM_SET_TD_MODE_LOOPBACK_REQ",	
		"CI_DEV_PRIM_SET_TD_MODE_LOOPBACK_CNF",	

		"CI_DEV_PRIM_SET_GSM_MODE_TX_RX_REQ",	
		"CI_DEV_PRIM_SET_GSM_MODE_TX_RX_CNF",	
		"CI_DEV_PRIM_SET_GSM_CONTROL_INTERFACE_REQ",	
		"CI_DEV_PRIM_SET_GSM_CONTROL_INTERFACE_CNF",	
		"CI_DEV_PRIM_ENABLE_HSDPA_REQ",
		"CI_DEV_PRIM_ENABLE_HSDPA_CNF",
		"CI_DEV_PRIM_GET_HSDPA_STATUS_REQ",
		"CI_DEV_PRIM_GET_HSDPA_STATUS_CNF",

		"CI_DEV_PRIM_READ_RF_TEMPERATURE_REQ",
		"CI_DEV_PRIM_READ_RF_TEMPERATURE_CNF",
#ifdef AT_CUSTOMER_HTC
		"CI_DEV_PRIM_SET_MODE_SWITCH_OPTION_REQ",
		"CI_DEV_PRIM_SET_MODE_SWITCH_OPTION_CNF",
		"CI_DEV_PRIM_GET_MODE_SWITCH_OPTION_REQ",
		"CI_DEV_PRIM_GET_MODE_SWITCH_OPTION_CNF",

		"CI_DEV_PRIM_RADIO_LINK_FAILURE_IND",
#endif

		"CI_DEV_PRIM_SET_NETWORK_MONITOR_OPTION_REQ",
		"CI_DEV_PRIM_SET_NETWORK_MONITOR_OPTION_CNF",
		"CI_DEV_PRIM_GET_NETWORK_MONITOR_OPTION_REQ",
		"CI_DEV_PRIM_GET_NETWORK_MONITOR_OPTION_CNF",

		"CI_DEV_PRIM_SET_PROTOCOL_STATUS_CONFIG_REQ",
		"CI_DEV_PRIM_SET_PROTOCOL_STATUS_CONFIG_CNF",
		"CI_DEV_PRIM_GET_PROTOCOL_STATUS_CONFIG_REQ",
		"CI_DEV_PRIM_GET_PROTOCOL_STATUS_CONFIG_CNF",
		"CI_DEV_PRIM_PROTOCOL_STATUS_CHANGED_IND",

		"CI_DEV_PRIM_SET_EVENT_IND_CONFIG_REQ",
		"CI_DEV_PRIM_SET_EVENT_IND_CONFIG_CNF",
		"CI_DEV_PRIM_GET_EVENT_IND_CONFIG_REQ",
		"CI_DEV_PRIM_GET_EVENT_IND_CONFIG_CNF",
		"CI_DEV_PRIM_EVENT_REPORT_IND",

		"CI_DEV_PRIM_SET_WIRELESS_PARAM_CONFIG_REQ",
		"CI_DEV_PRIM_SET_WIRELESS_PARAM_CONFIG_CNF",
		"CI_DEV_PRIM_GET_WIRELESS_PARAM_CONFIG_REQ",
		"CI_DEV_PRIM_GET_WIRELESS_PARAM_CONFIG_CNF",
		"CI_DEV_PRIM_WIRELESS_PARAM_IND",

		"CI_DEV_PRIM_SET_SIGNALING_REPORT_CONFIG_REQ",
		"CI_DEV_PRIM_SET_SIGNALING_REPORT_CONFIG_CNF",
		"CI_DEV_PRIM_GET_SIGNALING_REPORT_CONFIG_REQ",
		"CI_DEV_PRIM_GET_SIGNALING_REPORT_CONFIG_CNF",
		"CI_DEV_PRIM_SIGNALING_REPORT_IND",

		/* This should always be the last enum entry */
		"CI_DEV_PRIM_LAST_COMMON_PRIM"
	};
	static char *sDatMsgName[] = {
		"",
		"CI_DAT_PRIM_GET_MAX_PDU_SIZE_REQ",
		"CI_DAT_PRIM_GET_MAX_PDU_SIZE_CNF",
		"CI_DAT_PRIM_SEND_REQ",
		"CI_DAT_PRIM_SEND_CNF",
		"CI_DAT_PRIM_RECV_IND",
		"CI_DAT_PRIM_RECV_RSP",
		"CI_DAT_PRIM_OK_IND",
		"CI_DAT_PRIM_NOK_IND",
		"CI_DAT_PRIM_SEND_DATA_OPT_REQ",
		"CI_DAT_PRIM_SEND_DATA_OPT_CNF",
		"CI_DAT_PRIM_RECV_DATA_OPT_IND",
		"CI_DAT_PRIM_RECV_DATA_OPT_RSP",
		"CI_DAT_PRIM_SET_UL_TP_IND_REQ",
		"CI_DAT_PRIM_SET_UL_TP_IND_CNF",
		"CI_DAT_PRIM_UL_TP_IND",

		/* This should always be the last enum entry */
		"CI_DAT_PRIM_LAST_COMMON_PRIM"
	};

	static char *sErrPrim[] = {
		"CI_ERR_PRIM_HASNOSUPPORT_CNF",
		"CI_ERR_PRIM_HASINVALIDPARAS_CNF",
		"CI_ERR_PRIM_ISINVALIDREQUEST_CNF",
		"CI_ERR_PRIM_SIMNOTREADY_CNF",
		"CI_ERR_PRIM_ACCESSDENIED_CNF",
		"CI_ERR_PRIM_INTERLINKDOWN_IND",
		"CI_ERR_PRIM_INTERLINKDOWN_RSP",
		"CI_ERR_PRIM_INTERLINKUP_IND",

		/* This should always be the last enum entry */
		"CI_ERR_PRIM_NEXTAVAIL"
	};

	UINT8 *pCurr = data;
	UINT32 i, serviceHandle;

	if (srvGrpId == 0 || srvGrpId >= CI_SG_ID_NEXTAVAIL)
	{
		return;
	}

	CIPRINTF_1("********CI DUMP MSG BEGIN**********\n");
	CIPRINTF("ServiceGroup: %s(%d)\n", sSrvGroupName[srvGrpId], srvGrpId);

	if (primId >= CI_ERR_PRIM_NEXTAVAIL)
	{
		CIPRINTF("Invalid Primitive: 0x%x\n", primId);
		CIPRINTF_1("********CI DUMP MSG END**********\n");
		return;
	}
	else if (primId >= CI_ERR_PRIM_HASNOSUPPORT_CNF && primId < CI_ERR_PRIM_NEXTAVAIL)
	{
		CIPRINTF("Primitive: %s(0x%x)\n", sErrPrim[primId - CI_ERR_PRIM_HASNOSUPPORT_CNF], primId);
	}
	else
	{
		switch (srvGrpId)
		{
		case CI_SG_ID_CC:
			if(primId >= (sizeof(sCsMsgName)/sizeof(sCsMsgName[0]) - 1))
			{
				CIPRINTF("CI_SG_ID_CC Invalid Primitive: 0x%x\n", primId);
				CIPRINTF_1("********CI DUMP MSG END**********\n");
				return;
			}
			CIPRINTF("Primitive: %s(%d)\n", sCsMsgName[primId], primId);
			break;
		case CI_SG_ID_SS:
			if(primId >= (sizeof(sSsMsgName)/sizeof(sSsMsgName[0]) - 1))
			{
				CIPRINTF("CI_SG_ID_SS Invalid Primitive: 0x%x\n", primId);
				CIPRINTF_1("********CI DUMP MSG END**********\n");
				return;
			}
			CIPRINTF("Primitive: %s(%d)\n", sSsMsgName[primId], primId);
			break;
		case CI_SG_ID_SIM:
			if(primId >= (sizeof(sSimMsgName)/sizeof(sSimMsgName[0]) - 1))
			{
				CIPRINTF("CI_SG_ID_SIM Invalid Primitive: 0x%x\n", primId);
				CIPRINTF_1("********CI DUMP MSG END**********\n");
				return;
			}
			CIPRINTF("Primitive: %s(%d)\n", sSimMsgName[primId], primId);
			break;
		case CI_SG_ID_MSG:
			if(primId >= (sizeof(sSmsMsgName)/sizeof(sSmsMsgName[0]) - 1))
			{
				CIPRINTF("CI_SG_ID_MSG Invalid Primitive: 0x%x\n", primId);
				CIPRINTF_1("********CI DUMP MSG END**********\n");
				return;
			}
			CIPRINTF("Primitive: %s(%d)\n", sSmsMsgName[primId], primId);
			break;
		case CI_SG_ID_PS:
			if(primId >= (sizeof(sPsMsgName)/sizeof(sPsMsgName[0]) - 1))
			{
				CIPRINTF("CI_SG_ID_PS Invalid Primitive: 0x%x\n", primId);
				CIPRINTF_1("********CI DUMP MSG END**********\n");
				return;
			}
			CIPRINTF("Primitive: %s(%d)\n", sPsMsgName[primId], primId);
			break;
		case CI_SG_ID_PB:
			if(primId >= (sizeof(sPbMsgName)/sizeof(sPbMsgName[0]) - 1))
			{
				CIPRINTF("CI_SG_ID_PB Invalid Primitive: 0x%x\n", primId);
				CIPRINTF_1("********CI DUMP MSG END**********\n");
				return;
			}
			CIPRINTF("Primitive: %s(%d)\n", sPbMsgName[primId], primId);
			break;
		case CI_SG_ID_MM:
			if(primId >= (sizeof(sMmMsgName)/sizeof(sMmMsgName[0]) - 1))
			{
				CIPRINTF("CI_SG_ID_MM Invalid Primitive: 0x%x\n", primId);
				CIPRINTF_1("********CI DUMP MSG END**********\n");
				return;
			}
			CIPRINTF("Primitive: %s(%d)\n", sMmMsgName[primId], primId);
			break;
		case CI_SG_ID_DEV:
			if(primId >= (sizeof(sDevMsgName)/sizeof(sDevMsgName[0]) - 1))
			{
				CIPRINTF("CI_SG_ID_DEV Invalid Primitive: 0x%x\n", primId);
				CIPRINTF_1("********CI DUMP MSG END**********\n");
				return;
			}
			CIPRINTF("Primitive: %s(%d)\n", sDevMsgName[primId], primId);
			break;
		case CI_SG_ID_DAT:
			if(primId >= (sizeof(sDatMsgName)/sizeof(sDatMsgName[0]) - 1))
			{
				CIPRINTF("CI_SG_ID_DAT Invalid Primitive: 0x%x\n", primId);
				CIPRINTF_1("********CI DUMP MSG END**********\n");
				return;
			}
			CIPRINTF("Primitive: %s(%d)\n", sDatMsgName[primId], primId);
			break;
		default:
			CIPRINTF("Primitive: (%d)\n", primId);
		}
	}

	CIPRINTF("RequestHandle: 0x%x\n", reqHandle);

	if (dataSize == 0)
	{
		serviceHandle = gCiServiceHandle[srvGrpId];
		dataSize = cimem_GetCiPrimDataSize(serviceHandle, primId, data);
	}
	int length;
	UINT32 dumpDataSize;
	if (dataSize > CI_DUMP_DATA_SIZE_MAX)
		dumpDataSize = CI_DUMP_DATA_SIZE_MAX;
	else
		dumpDataSize = dataSize;

	char * ptempBuf = malloc(30+2*dumpDataSize);

	//CIPRINTF("Data[size=%d]: ", dataSize);
	length = sprintf(ptempBuf,"Data[size=%d]:", dataSize);

	if (dataSize > 0)
	{
		char *num_buf;
		num_buf = ptempBuf + length;
		for (i = 0; i < dumpDataSize; i++)
		{
			//CIPRINTF("%02x", *pCurr++);
			length = sprintf(num_buf, "%02x", *pCurr++);
			num_buf += length;
		}
		CIPRINTF(ptempBuf);
	}
	free(ptempBuf);
	CIPRINTF_1("********CI DUMP MSG END  **********\n");

}
#endif

