/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: ps_api.c
 *
 *  Description: API interface implementation for packet service
 *
 *  History:
 *   Sep 18, 2008 - Rovin Yu Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "ci_api_types.h"
#include "ci_api.h"
#include "teldef.h"
#include "telutl.h"
#include "telatparamdef.h"
#include "ci_ps.h"
#include "telatci.h"
#include "utlMalloc.h"

AtciCurrentPsCntxList gCIDList;

//DIRECTIPCONFIGLIST *directIpAddressList = NULL;

extern CiServiceHandle gAtciSvgHandle[CI_SG_NUMIDS + 1];

extern int gActDetail;

static UINT8 gCurrentPSRegOption = CI_PS_NW_REG_IND_DISABLE;
UINT8 gCurrentPSRegStatus = CI_PS_NW_REG_STA_NOT_REGED;

char gPdpErrCauseBuf[256];

#define MAX_ADDR_PRESENTATION_BUF_LEN (sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")

/* Define some private primitives for PS service */
#define PRI_PS_PRIM_GET_SECOND_PDP_CTXS_RANGE_REQ (CI_PS_PRIM_LAST_COMMON_PRIM + 1)
#define PRI_PS_PRIM_GET_MIN_QOS_CAPS_REQ (CI_PS_PRIM_LAST_COMMON_PRIM + 2)
#define PRI_PS_PRIM_GET_NEG_QOS_CAPS_REQ (CI_PS_PRIM_LAST_COMMON_PRIM + 3)
#define PRI_PS_PRIM_GET_MIN_QOS_REQ (CI_PS_PRIM_LAST_COMMON_PRIM + 4)
#define PRI_PS_PRIM_GET_NEG_QOS_REQ (CI_PS_PRIM_LAST_COMMON_PRIM + 5)
#define PRI_PS_PRIM_SET_PDP_CTX_DEACT_STATE_REQ (CI_PS_PRIM_LAST_COMMON_PRIM + 6)
#define PRI_PS_PRIM_SET_DETACH_STATE_REQ (CI_PS_PRIM_LAST_COMMON_PRIM + 7)
#define PRI_PS_PRIM_GET_TFT_REQ (CI_PS_PRIM_LAST_COMMON_PRIM + 8)
#define PRI_PS_PRIM_GET_IP_REQ (CI_PS_PRIM_LAST_COMMON_PRIM + 9)


void resetCurrCntx(UINT8 atpIdx);
CiReturnCode   sendDefineDefaultPdpContext( CiRequestHandle reqHandle, INT8 cid );
//In order to reset PS paras in case of CP assertion
void resetPsParas(void);

#if 0

static void addToList(DIRECTIPCONFIGLIST *newdrvnode)
{
	DIRECTIPCONFIGLIST *pCurrNode;

	if (directIpAddressList == NULL)
	{
		directIpAddressList = newdrvnode;
	}
	else
	{
		pCurrNode = directIpAddressList;
		while (pCurrNode->next != NULL)
			pCurrNode = pCurrNode->next;

		pCurrNode->next = newdrvnode;

	}
	return;
}
BOOL  searchListByCid(unsigned char cid, DIRECTIPCONFIGLIST ** ppBuf)
{
	DIRECTIPCONFIGLIST *pCurrBuf;

	if (directIpAddressList == NULL)
		return FALSE;

	pCurrBuf = directIpAddressList;
	while ((pCurrBuf != NULL) && (pCurrBuf->directIpAddress.dwContextId != cid))
	{
		pCurrBuf = pCurrBuf->next;
	}

	if (pCurrBuf != NULL)
	{
		*ppBuf = pCurrBuf;
		return TRUE;
	}

	return FALSE;
}


static void removeList(void)
{
	DIRECTIPCONFIGLIST *pCurrNode;

	pCurrNode = directIpAddressList;
	while (pCurrNode != NULL)
	{
		directIpAddressList = directIpAddressList->next;
		free(pCurrNode);
		pCurrNode = directIpAddressList;
	}
	return;
}

static void  removeNodeByCid(unsigned char cid)
{
	DIRECTIPCONFIGLIST *pCurrNode, *pPrevNode;

	pPrevNode = pCurrNode = directIpAddressList;
	while (pCurrNode != NULL)
	{
		if (pCurrNode->directIpAddress.dwContextId == cid)
		{
			if (pCurrNode == directIpAddressList)
			{
				directIpAddressList = pCurrNode->next;
				free(pCurrNode);
				pPrevNode = pCurrNode = directIpAddressList;
			}
			else
			{
				pPrevNode->next = pCurrNode->next;
				free(pCurrNode);
				pCurrNode = pPrevNode->next;
			}
		}
		else
		{
			pPrevNode = pCurrNode;
			pCurrNode = pCurrNode->next;
		}
	}
}
#endif

//AT+CGREG=
CiReturnCode PS_SetGPRSRegStatus (UINT32 atHandle, UINT32 state)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimEnableNwRegIndReq    *setNwRegIndReq;

	setNwRegIndReq = utlCalloc(1, sizeof(*setNwRegIndReq));
	if (setNwRegIndReq == NULL)
		return CIRC_FAIL;

	setNwRegIndReq->flag = state;
	gCurrentPSRegOption = state;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_ENABLE_NW_REG_IND_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_ENABLE_NW_REG_IND_REQ), (void *)setNwRegIndReq);
	return ret;
}

//AT+CGREG?
CiReturnCode PS_GetGPRSRegStatus (UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimGetNwRegStatusReq    *getPsRegStatusReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_NW_REG_STATUS_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_GET_NW_REG_STATUS_REQ), (void *)getPsRegStatusReq );
	return ret;
}

//AT+CTFT?
CiReturnCode PS_GetTftList (UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimGetPdpCtxReq    *getPdpCtxReq;
	UINT32 reqHandle;

	getPdpCtxReq = utlCalloc(1, sizeof(*getPdpCtxReq));
	if (getPdpCtxReq == NULL)
		return CIRC_FAIL;

	getPdpCtxReq->cid = ATCI_FIRST_CID;
	gCIDList.currCntx[GET_ATP_INDEX(atHandle)].currCid = ATCI_FIRST_CID;
	reqHandle = MAKE_CI_REQ_HANDLE(atHandle, PRI_PS_PRIM_GET_TFT_REQ);

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_PDP_CTX_REQ,
			 reqHandle, (void *)getPdpCtxReq );

	return ret;
}

//AT+CTFT = cid, only cid is included
CiReturnCode PS_DeleteTft (UINT32 atHandle, UINT32 dwContextID)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimDeleteTftReq            *deleteTftReq;
	UINT32 reqHandle;

	deleteTftReq = utlCalloc(1, sizeof(*deleteTftReq));
	if (deleteTftReq == NULL)
		return CIRC_FAIL;


	deleteTftReq->cid = (UINT8)dwContextID;
	reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_DELETE_TFT_REQ);
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_DELETE_TFT_REQ,
			 reqHandle, (void *)deleteTftReq );

	return ret;
}

//AT+CTFT = cid, pfi, , ,
CiReturnCode PS_SetTftFilter (UINT32 atHandle, UINT32 dwContextID, const CiPsTftFilter *tftFilter)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimDefineTftFilterReq    *defineTftFilterReq;
	UINT32 reqHandle;

	defineTftFilterReq = utlCalloc(1, sizeof(*defineTftFilterReq));
	if (defineTftFilterReq == NULL)
		return CIRC_FAIL;


	defineTftFilterReq->cid = (UINT8)dwContextID;
	memcpy(&defineTftFilterReq->filter, tftFilter, sizeof(CiPsTftFilter));
	reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_DEFINE_TFT_FILTER_REQ);
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_DEFINE_TFT_FILTER_REQ,
			 reqHandle, (void *)defineTftFilterReq );

	return ret;
}

//AT+CGCMOD=?
CiReturnCode PS_GetGPRSActiveCidList (UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimGetPdpCtxsActStateReq    *pdpCtxsActStateReq = NULL;
	UINT32 reqHandle;

	reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_GET_PDP_CTXS_ACT_STATE_REQ);

	ret = ciRequest(gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_PDP_CTXS_ACT_STATE_REQ,
			reqHandle, (void *)pdpCtxsActStateReq);

	return ret;
}

//AT+CGCMOD=cid,
CiReturnCode PS_ModifyGPRSContext(UINT32 atHandle, UINT32 dwContextID, BOOL doAll)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimModifyPdpCtxReq            *modifyPdpCtxReq;
	UINT32 reqHandle;

	modifyPdpCtxReq = utlCalloc(1, sizeof(*modifyPdpCtxReq));
	if (modifyPdpCtxReq == NULL)
		return CIRC_FAIL;

	modifyPdpCtxReq->cid = (UINT32)dwContextID;
	modifyPdpCtxReq->doAll = doAll;
	reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_MODIFY_PDP_CTX_REQ);

	ret = ciRequest(gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_MODIFY_PDP_CTX_REQ,
			reqHandle, (void *)modifyPdpCtxReq);

	return ret;

}

//AT+CGDCONT?
CiReturnCode PS_GetGPRSContextList (UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimGetPdpCtxReq    *getPdpCtxReq;
	UINT32 reqHandle;

	getPdpCtxReq = utlCalloc(1, sizeof(*getPdpCtxReq));
	if (getPdpCtxReq == NULL)
		return CIRC_FAIL;

	getPdpCtxReq->cid = ATCI_FIRST_CID;
	gCIDList.currCntx[GET_ATP_INDEX(atHandle)].currCid = ATCI_FIRST_CID;
	reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_GET_PDP_CTX_REQ);

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_PDP_CTX_REQ,
			 reqHandle, (void *)getPdpCtxReq );

	return ret;
}

//AT+CGDCONT=cid, type, apn,
CiReturnCode PS_SetGPRSContext (UINT32 atHandle, const CiPsPdpCtx* lpGprsContext)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimDefinePdpCtxReq  *defPdpReq;

	//If the PDP context is already in active state, we should reject it
	if (gCIDList.cInfo[lpGprsContext->cid].reqHandle != INVALID_REQ_HANDLE)
	{
		ERRMSG("%s: the cid %d context is already active!\n", __FUNCTION__, lpGprsContext->cid);
		return ret;
	}

	defPdpReq = utlCalloc(1, sizeof(*defPdpReq));
	if (defPdpReq == NULL)
		return CIRC_FAIL;

	memcpy(&(defPdpReq->pdpCtx), lpGprsContext, sizeof(CiPsPdpCtx));

	gCIDList.currCntx[GET_ATP_INDEX(atHandle)].currCid = lpGprsContext->cid;
	gCIDList.cInfo[lpGprsContext->cid].pdpType = lpGprsContext->type;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_DEFINE_PDP_CTX_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_DEFINE_PDP_CTX_REQ), (void *)defPdpReq );

	return ret;
}

//AT+CGDCONT=cid only cid is included
CiReturnCode PS_DeleteGPRSContext (UINT32 atHandle, UINT32 dwContextID)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimDeletePdpCtxReq  *deletePdpCtxReq;

	/* Delete PDP context */
	deletePdpCtxReq = utlCalloc(1, sizeof(*deletePdpCtxReq));
	if (deletePdpCtxReq == NULL)
		return CIRC_FAIL;

	deletePdpCtxReq->cid = dwContextID;
	gCIDList.currCntx[GET_ATP_INDEX(atHandle)].currCid = dwContextID;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_DELETE_PDP_CTX_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_DELETE_PDP_CTX_REQ), (void *)deletePdpCtxReq );

	return ret;
}

//AT+CGQREQ?
CiReturnCode PS_GetQualityOfServiceList (UINT32 atHandle, BOOL isMin)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimGetQosReq       *getQosReq;
	UINT32 reqHandle;

	getQosReq = utlCalloc(1, sizeof(*getQosReq));
	if (getQosReq == NULL)
		return CIRC_FAIL;


	getQosReq->cid = ATCI_FIRST_CID;
	gCIDList.currCntx[GET_ATP_INDEX(atHandle)].currCid = ATCI_FIRST_CID;
	getQosReq->isMin = isMin;

	if (isMin)
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, PRI_PS_PRIM_GET_MIN_QOS_REQ);
	else
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_GET_QOS_REQ);

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_QOS_REQ,
			 reqHandle, (void *)getQosReq );

	return ret;
}

//AT+CGQREQ=
CiReturnCode PS_SetQualityOfService (UINT32 atHandle, UINT32 dwContextID, const CiPsQosProfile* lpGprsQosProfile, BOOL isMin)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimSetQosReq       *setQosReq = utlCalloc(1, sizeof(*setQosReq));

	if (setQosReq == NULL)
		return CIRC_FAIL;

	setQosReq->isMin = isMin;
	setQosReq->cid = dwContextID;
	memcpy(&setQosReq->qosProf, lpGprsQosProfile, sizeof(CiPsQosProfile));

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_SET_QOS_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_SET_QOS_REQ), (void *)setQosReq );

	return ret;
}

//AT+CGATT=
CiReturnCode PS_SetGPRSAttached (UINT32 atHandle, BOOL fAttached)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimSetAttachStateReq    *setAttStateReq;
	UINT32 reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_SET_ATTACH_STATE_REQ);

	setAttStateReq = utlCalloc(1, sizeof(*setAttStateReq));
	if (setAttStateReq == NULL)
		return CIRC_FAIL;

	setAttStateReq->state = fAttached;
	if (!fAttached)
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, PRI_PS_PRIM_SET_DETACH_STATE_REQ);
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_SET_ATTACH_STATE_REQ,
			 reqHandle, (void *)setAttStateReq );
	return ret;
}

//AT+CGATT?
CiReturnCode PS_GetGPRSAttached (UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimGetAttachStateReq    *getAttStateReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_ATTACH_STATE_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_GET_ATTACH_STATE_REQ), (void *)getAttStateReq );
	return ret;
}

//AT+CGACT?
CiReturnCode PS_GetGPRSContextActivatedList (UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimGetPdpCtxReq    *getPdpCtxReq;
	UINT32 reqHandle;

	getPdpCtxReq = utlCalloc(1, sizeof(*getPdpCtxReq));
	if (getPdpCtxReq == NULL)
		return CIRC_FAIL;

	getPdpCtxReq->cid = ATCI_FIRST_CID;
	gCIDList.currCntx[GET_ATP_INDEX(atHandle)].currCid = ATCI_FIRST_CID;
	reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_GET_ACTIVE_CID_LIST_REQ);

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_PDP_CTX_REQ,
			 reqHandle, (void *)getPdpCtxReq );

	return ret;

}

//AT+CGACT=
CiReturnCode PS_SetGPRSContextActivated (UINT32 atHandle, UINT32 dwContextID, BOOL fContextActivation, BOOL doAll)
{
	CiReturnCode ret = CIRC_FAIL;
	UINT32 reqHandle;

	CiPsL2P l2p = CI_PS_L2P_NONE;

	CiPsPrimSetPdpCtxActStateReq   *setActStateReq;

	/* Determine the l2p type */
	if ( fContextActivation == TRUE )
	{
		if ( gCIDList.cInfo[dwContextID].bDefined == TRUE )
		{
			switch ( gCIDList.cInfo[dwContextID].pdpType )
			{
			case CI_PS_PDP_TYPE_PPP:
				l2p = CI_PS_L2P_PPP;
				break;
			case CI_PS_PDP_TYPE_IP:
				l2p = CI_PS_L2P_NONE;
				break;
			default:
				l2p = CI_PS_L2P_NONE;
				break;
			}
		}
		else
		{
			/* If the context is not defined - we cannot activate it */
			ERRMSG("%s: the dwContextID:%d is not defined. Please define it first!\n", __FUNCTION__, dwContextID);
			return ret;
		}
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_SET_PDP_CTX_ACT_STATE_REQ);

	}
	else
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, PRI_PS_PRIM_SET_PDP_CTX_DEACT_STATE_REQ);

	gCIDList.currCntx[GET_ATP_INDEX(atHandle)].currCid = dwContextID;

	setActStateReq = (CiPsPrimSetPdpCtxActStateReq*)utlCalloc(1, sizeof(CiPsPrimSetPdpCtxActStateReq));
	if (setActStateReq == NULL)
		return CIRC_FAIL;

	setActStateReq->state = fContextActivation;
	setActStateReq->cid = dwContextID;
	setActStateReq->doAll = doAll;
	setActStateReq->l2p = l2p;

	/*
	**  Send the actual activate CI Request.
	*/
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_SET_PDP_CTX_ACT_STATE_REQ,
			 reqHandle, (void *)setActStateReq );

	return ret;

}
//AT*AUTHReq
CiReturnCode PS_SendAUTHReq(UINT32 atHandle, const CiPsPrimAuthenticateReq * lpAuthReq )
{
	CiReturnCode ret = CIRC_FAIL;
	UINT32 reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_AUTHENTICATE_REQ);
	UINT8 atpIndex = GET_ATP_INDEX(atHandle);
	CiPsPrimAuthenticateReq *pdpAuthReq = NULL;

	gCIDList.currCntx[atpIndex].currCid = lpAuthReq->cid;

	if ( gCIDList.cInfo[lpAuthReq->cid].bDefined == TRUE )
	{
		if (gCIDList.cInfo[lpAuthReq->cid].reqHandle != INVALID_REQ_HANDLE)
		{
			ERRMSG("%s: the dwContextID:%d has been actived. So reject to reactivate it!\n", __FUNCTION__, lpAuthReq->cid);
			return ret;
		}
		pdpAuthReq = utlCalloc(1, sizeof(*pdpAuthReq));
		if (pdpAuthReq == NULL)
			return CIRC_FAIL;

		memcpy((void *)pdpAuthReq, (void *)lpAuthReq, sizeof(*pdpAuthReq));

		DBGMSG("ciSendAUTHReq:cid=%d,Autytype=%d,Username-%s, PassWord-%s\n",
		       pdpAuthReq->cid, pdpAuthReq->AuthenticationType, pdpAuthReq->UserName.valStr, pdpAuthReq->Password.valStr);

		ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_AUTHENTICATE_REQ,
				 reqHandle, (void *)pdpAuthReq);
	}
	else
	{
		ERRMSG("%s: the dwContextID:%d is not defined. Please define it first!\n", __FUNCTION__, lpAuthReq->cid);
		return ret;
	}
	return ret;
}

//AT+GETIP=cid
CiReturnCode PS_GetGPRSContextIP (UINT32 atHandle, UINT32 dwContextID)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimGetPdpCtxReq    *getPdpCtxReq;
	UINT32 reqHandle;

	getPdpCtxReq = utlCalloc(1, sizeof(*getPdpCtxReq));
	if (getPdpCtxReq == NULL)
		return CIRC_FAIL;

	getPdpCtxReq->cid = dwContextID; //ATCI_FIRST_CID;
	//gCIDList.currCntx[GET_ATP_INDEX(atHandle)].currCid = ATCI_FIRST_CID;
	reqHandle = MAKE_CI_REQ_HANDLE(atHandle, PRI_PS_PRIM_GET_IP_REQ);

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_PDP_CTX_REQ,
			 reqHandle, (void *)getPdpCtxReq );

	return ret;

}

//AT+CGDATA=
CiReturnCode PS_EnterGPRSDataMode (UINT32 atHandle, UINT32 dwContextID, CiPsL2P l2p)
{
	CiReturnCode ret = CIRC_FAIL;
	UINT32 reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_ENTER_DATA_STATE_REQ);
	UINT8 atpIndex = GET_ATP_INDEX(atHandle);

	CiPsPrimEnterDataStateReq      *pdpEnterDataReq = NULL;

	gCIDList.currCntx[atpIndex].currCid = dwContextID;

	pdpEnterDataReq = utlCalloc(1, sizeof(*pdpEnterDataReq));
	if (pdpEnterDataReq == NULL)
		return CIRC_FAIL;

	pdpEnterDataReq->optimizedData = TRUE;
	pdpEnterDataReq->cid = dwContextID;
	pdpEnterDataReq->l2p = l2p;

	/* Mark the context as waiting to enter data state */
	if (l2p == CI_PS_L2P_PPP)
		gCIDList.cInfo[dwContextID].pdpType = CI_PS_PDP_TYPE_PPP;
	else
		gCIDList.cInfo[dwContextID].pdpType = CI_PS_PDP_TYPE_IP;

	if (GET_ATP_INDEX(atHandle) == TEL_MODEM_AT_CMD_ATP)
		gCIDList.cInfo[dwContextID].connectionType = ATCI_REMOTE;
	else
		gCIDList.cInfo[dwContextID].connectionType = ATCI_LOCAL;

	if ( gCIDList.cInfo[dwContextID].bDefined == TRUE )
	{
		/*Fixed me: shall we allow to reactivate one active PDP context*/
		if (gCIDList.cInfo[dwContextID].reqHandle != INVALID_REQ_HANDLE)
		{
			ERRMSG("%s: the dwContextID:%d has been actived. So reject to reactivate it!\n", __FUNCTION__, dwContextID);
			utlFree(pdpEnterDataReq);
			return ret;
		}

		DBGMSG("ciEnterData:cid=%d,l2p=%d,opt-%d\n",
		       pdpEnterDataReq->cid, pdpEnterDataReq->l2p, pdpEnterDataReq->optimizedData);

		/* Send the CI Request */
		ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_ENTER_DATA_STATE_REQ,
				 reqHandle, (void *)pdpEnterDataReq );

	}
	else
	{
#if 0
		/* Mark that we should enter data state as soon as the context is defined */
		if (gCIDList.currCntx[atpIndex].reqMsg != NULL)
		{
			/* First release it */
			utlFree(gCIDList.currCntx[atpIndex].reqMsg);
		}

		gCIDList.currCntx[atpIndex].reqMsg = pdpEnterDataReq;

		ret = sendDefineDefaultPdpContext( reqHandle, pdpEnterDataReq->cid );
#endif
		/* If the CID is not defined, we think it's no sense to use the default PDP to activate it 
		 * because we cannot assume the default PDP APN is fixed.
		 * Therefore, we just reject the request.
		 */
		ERRMSG("%s: the dwContextID:%d is not defined. Please define it first!\n", __FUNCTION__, dwContextID);
		utlFree(pdpEnterDataReq);
		return ret;
	}

	return ret;

}

//AT+CGEQREQ?
CiReturnCode PS_Get3GQualityOfServiceList (UINT32 atHandle, CiPs3GQosType type)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimGet3GQosReq      *get3GQosReq;
	UINT32 reqHandle;

	get3GQosReq = utlCalloc(1, sizeof(*get3GQosReq));
	if (get3GQosReq == NULL)
		return CIRC_FAIL;

	get3GQosReq->cid = ATCI_FIRST_CID;
	gCIDList.currCntx[GET_ATP_INDEX(atHandle)].currCid = ATCI_FIRST_CID;
	get3GQosReq->qosType = type;

	if (type == CI_PS_3G_QOSTYPE_MIN)
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, PRI_PS_PRIM_GET_MIN_QOS_REQ);
	else if (type == CI_PS_3G_QOSTYPE_NEG)
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, PRI_PS_PRIM_GET_NEG_QOS_REQ);
	else
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_GET_3G_QOS_REQ);

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_3G_QOS_REQ,
			 reqHandle, (void *)get3GQosReq );

	return ret;
}

//AT+CGEQREQ=
CiReturnCode PS_Set3GQualityOfService (UINT32 atHandle, UINT32 dwContextID, const CiPs3GQosProfile* lp3GQosProfile, CiPs3GQosType type)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimSet3GQosReq      *set3GQosReq;

	set3GQosReq = utlCalloc(1, sizeof(*set3GQosReq));
	if (set3GQosReq == NULL)
		return CIRC_FAIL;

	set3GQosReq->cid = dwContextID;
	set3GQosReq->qosType = type;
	memcpy(&set3GQosReq->qosProf, lp3GQosProfile, sizeof(CiPs3GQosProfile));

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_SET_3G_QOS_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_SET_3G_QOS_REQ), (void *)set3GQosReq );

	return ret;
}

//AT+CGEQREQ=?
CiReturnCode PS_Get3GCapsQos (UINT32 atHandle, CiPs3GQosType type)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimGet3GQosCapsReq  *get3GQosCapsReq;
	UINT32 reqHandle;

	get3GQosCapsReq = utlCalloc(1, sizeof(*get3GQosCapsReq));
	if (get3GQosCapsReq == NULL)
		return CIRC_FAIL;

	get3GQosCapsReq->qosType = type;
	if (type == CI_PS_3G_QOSTYPE_MIN)
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, PRI_PS_PRIM_GET_MIN_QOS_CAPS_REQ);
	else if (type == CI_PS_3G_QOSTYPE_NEG)
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, PRI_PS_PRIM_GET_NEG_QOS_CAPS_REQ);
	else
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_GET_3G_QOS_CAPS_REQ);

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_3G_QOS_CAPS_REQ,
			 reqHandle, (void *)get3GQosCapsReq );

	return ret;
}

//AT+CGQREQ=?
CiReturnCode PS_GetGprsCapsQos (UINT32 atHandle, BOOL isMin)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimGetQosCapsReq   *getQosCapsReq;
	UINT32 reqHandle;

	getQosCapsReq = utlCalloc(1, sizeof(*getQosCapsReq));
	if (getQosCapsReq == NULL)
		return CIRC_FAIL;

	getQosCapsReq->isMin = isMin;
	if (isMin)
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, PRI_PS_PRIM_GET_MIN_QOS_CAPS_REQ);
	else
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_GET_QOS_CAPS_REQ);
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_QOS_CAPS_REQ,
			 reqHandle, (void *)getQosCapsReq );

	return ret;
}

//AT+CGDSCONT?
CiReturnCode PS_GetSecondaryContextList (UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimGetSecPdpCtxReq        *getSecPdpReq;

	getSecPdpReq = utlCalloc(1, sizeof(*getSecPdpReq));
	if (getSecPdpReq == NULL)
		return CIRC_FAIL;

	getSecPdpReq->cid = ATCI_FIRST_CID;
	gCIDList.currCntx[GET_ATP_INDEX(atHandle)].currCid = ATCI_FIRST_CID;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_SEC_PDP_CTX_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_GET_SEC_PDP_CTX_REQ), (void *)getSecPdpReq );

	return ret;
}

//AT+CGDSCONT=
CiReturnCode PS_SetSecondaryPDPContext (UINT32 atHandle, const CiPsSecPdpCtx* lpSecondaryPDPContext)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimDefineSecPdpCtxReq     *defSecPdpReq;

	if ( gCIDList.cInfo[lpSecondaryPDPContext->p_cid].bDefined == TRUE )
	{
		gCIDList.currCntx[GET_ATP_INDEX(atHandle)].currCid = lpSecondaryPDPContext->cid;
		defSecPdpReq = utlCalloc(1, sizeof(*defSecPdpReq));
		if (defSecPdpReq == NULL)
			return CIRC_FAIL;

		memcpy(&defSecPdpReq->secPdpCtx, lpSecondaryPDPContext, sizeof(CiPsSecPdpCtx));
		ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_DEFINE_SEC_PDP_CTX_REQ,
				 MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_DEFINE_SEC_PDP_CTX_REQ), (void *)defSecPdpReq );
	}

	return ret;

}

//AT+CGDSCONT=only CID
CiReturnCode PS_DeleteSecondaryPDPContext (UINT32 atHandle, UINT32 dwContextID)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimDeleteSecPdpCtxReq  *deletePdpCtxReq;

	/* Delete Second PDP context */
	deletePdpCtxReq = utlCalloc(1, sizeof(*deletePdpCtxReq));
	if (deletePdpCtxReq == NULL)
		return CIRC_FAIL;

	deletePdpCtxReq->cid = dwContextID;
	gCIDList.currCntx[GET_ATP_INDEX(atHandle)].currCid = dwContextID;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_DELETE_SEC_PDP_CTX_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_DELETE_SEC_PDP_CTX_REQ), (void *)deletePdpCtxReq );

	return ret;

}


//AT+CGDSCONT=?
CiReturnCode PS_GetSecondaryContextRange (UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimGetPdpCtxReq    *getPdpCtxReq;
	UINT32 reqHandle;

	getPdpCtxReq = utlCalloc(1, sizeof(*getPdpCtxReq));
	if (getPdpCtxReq == NULL)
		return CIRC_FAIL;

	getPdpCtxReq->cid = ATCI_FIRST_CID;
	gCIDList.currCntx[GET_ATP_INDEX(atHandle)].currCid = ATCI_FIRST_CID;
	reqHandle = MAKE_CI_REQ_HANDLE(atHandle, PRI_PS_PRIM_GET_SECOND_PDP_CTXS_RANGE_REQ);

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_PDP_CTX_REQ,
			 reqHandle, (void *)getPdpCtxReq );

	return ret;
}

//AT*FastDorm
CiReturnCode PS_FastDormancy(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	UINT32 reqHandle;
	reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_FAST_DORMANT_REQ);
	ret = ciRequest(gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_FAST_DORMANT_REQ, reqHandle, NULL);
	return ret;
}
#ifdef AT_CUSTOMER_HTC
//AT*FDY=
CiReturnCode PS_SetFDY(UINT32 atHandle, UINT8 type, UINT16 interval)
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimSetFastDormancyConfigReq *setFastDormancyConfigReq = NULL;
	setFastDormancyConfigReq = utlCalloc(1, sizeof(CiPsPrimSetFastDormancyConfigReq));
	if (setFastDormancyConfigReq == NULL)
		return CIRC_FAIL;
	setFastDormancyConfigReq->type = type;
	setFastDormancyConfigReq->interval = interval;

	ret = ciRequest(gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_SET_FAST_DORMANCY_CONFIG_REQ,
		MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_SET_FAST_DORMANCY_CONFIG_REQ), (void*)setFastDormancyConfigReq);
	return ret;
}
#endif
/************************************************************************************
 * F@: sendDefineDefaultPdpContext
 *
 */
CiReturnCode   sendDefineDefaultPdpContext( CiRequestHandle reqHandle, INT8 cid )
{
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimDefinePdpCtxReq        *enterDataDefPdpReq;

	/* initialize the request fields to the default values */
	enterDataDefPdpReq = utlCalloc(1, sizeof(*enterDataDefPdpReq));
	if (enterDataDefPdpReq == NULL)
		return CIRC_FAIL;

	enterDataDefPdpReq->pdpCtx.cid = cid;
	enterDataDefPdpReq->pdpCtx.type = CI_PS_PDP_TYPE_IP;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_DEFINE_PDP_CTX_REQ,
			 reqHandle, (void *)enterDataDefPdpReq );

	return ret;
}

/*******************************************************************
*  FUNCTION:  ciMakeGPRSDataCallbyAtd
*
*  DESCRIPTION: To support GPRS Data Call triggered by ATD, such as ATDT*99#
*
*
*  RETURNS: _CiReturnCode
*
*******************************************************************/
CiReturnCode ciMakeGPRSDataCallbyAtd (UINT32 atHandle, const char *command_name_p)
{
	CiReturnCode ret = CIRC_FAIL;
	UINT16 dialDigitsLen = strlen(command_name_p);
	UINT8 cid = 0;
	CiPsL2P l2p;

	/* this is gprs data call*/
	if (dialDigitsLen > strlen("*99***"))
		cid = atoi(&command_name_p[strlen("*99***")]) - 1;

	if (cid >= CI_PS_MAX_PDP_CTX_NUM)
	{
		ERRMSG("%s:Invalid cid:%d\n", __FUNCTION__, cid);
		return CIRC_FAIL;
	}

	l2p = CI_PS_L2P_PPP;

	ret = PS_EnterGPRSDataMode(atHandle, cid, l2p);

	return ret;

}

void resetCurrCntx(UINT8 atpIdx)
{
	if (atpIdx < NUM_OF_TEL_ATP)
	{
		if (gCIDList.currCntx[atpIdx].currCid != CI_PS_MAX_PDP_CTX_NUM)
		{
			gCIDList.cInfo[gCIDList.currCntx[atpIdx].currCid].reqHandle = INVALID_REQ_HANDLE;
			gCIDList.currCntx[atpIdx].currCid = CI_PS_MAX_PDP_CTX_NUM;

		}
		if (gCIDList.currCntx[atpIdx].reqMsg != NULL)
		{
			utlFree(gCIDList.currCntx[atpIdx].reqMsg);
			gCIDList.currCntx[atpIdx].reqMsg = NULL;
		}
	}
}

void resetCidList(void)
{
	int i;

	for ( i = 0; i < NUM_OF_TEL_ATP; i++ )
		resetCurrCntx(i);
	for ( i = 0; i < CI_PS_MAX_PDP_CTX_NUM; i++ )
		gCIDList.cInfo[i].bDefined = FALSE;
}
void resetPsParas(void)
{
	gCurrentPSRegOption = CI_PS_NW_REG_IND_DISABLE;
	gCurrentPSRegStatus = CI_PS_NW_REG_STA_NOT_REGED;
}

/************************************************************************************
 *
 * PS CI confirmations
 *
 *************************************************************************************/
static void getPdpTypeStr (CiPsPdpType pdpType, char  *typeStr)
{
	/* PDP type number */
	switch ( pdpType )
	{
	case CI_PS_PDP_TYPE_IP:      /* IPv4 */
	{
		sprintf((char *)typeStr, "\"IP\"");
		break;
	}
	case CI_PS_PDP_TYPE_IPV6:    /* IPv6 */
	{
		sprintf((char *)typeStr, "\"IPV6\"");
		break;
	}
	case CI_PS_PDP_TYPE_PPP:     /* PPP */
	{
		sprintf((char *)typeStr, "\"PPP\"");
		break;
	}
	default:
		break;
	}
}


/* -----------------------------------------------------------------
 * Function    : Decode3GBitRate
 * Description : Decodes 3G QoS Bit Rate parameter value.
 * Parameters  : codedValue - encoded 8-bit value
 * Returns     : 16-bit decoded value, in kbps
 * Notes       : See 3GPP TS 24.008/V3.11.0, Table 10.5.156 for
 *               the coding scheme.
 *               This function is used for the Maximum and Guaranteed
 *               Bit Rate (both Uplink and Downlink) parameters.
 * ----------------------------------------------------------------- */
static INT16 Decode3GBitRate( UINT8 codedValue )
{
	INT16 rawValue = 0;

	if ( codedValue >= 0x01 && codedValue <= 0x3f )
	{
		/* 0x01..0x3f -> 1kbps..63kbps, in 1kbps steps */
		rawValue = (INT16)codedValue;
	}
	else if ( codedValue >= 0x40 && codedValue <= 0x7f )
	{
		/* 0x40..0x7f -> 64kbps..568kbps, in 8kbps steps */
		rawValue = (INT16)( ( codedValue - 0x40 ) * 8 ) + 64;
	}
	else if ( codedValue >= 0x80 && codedValue <= 0xfe )
	{
		/* 0x80..0xfe -> 576kbps..8640kbps, in 64kbps steps */
		rawValue = (INT16)( ( codedValue - 0x80 ) * 64 ) + 576;
	}
	else if ( codedValue == 0xff )
	{
		/* 0xff -> 0kbps (explicitly) */
		rawValue = 0;
	}

	return( rawValue );
}

/* -----------------------------------------------------------------
 * Function    : Decode3GTransDelay
 * Description : Decodes 3G QoS Transfer Delay parameter value.
 * Parameters  : codedValue - encoded 8-bit value
 * Returns     : 16-bit decoded value, in ms
 * Notes       : See 3GPP TS 24.008/V3.11.0, Table 10.5.156 for
 *               the coding scheme.
 * ----------------------------------------------------------------- */
static INT16 Decode3GTransDelay( UINT8 codedValue )
{
	INT16 rawValue = 0;

	if ( codedValue >= 0x01 && codedValue <= 0x0f )
	{
		/* 0x01..0x0f -> 10ms..150ms, in 10ms steps */
		rawValue = (INT16)( codedValue * 10 );
	}
	else if ( codedValue >= 0x10 && codedValue <= 0x1f )
	{
		/* 0x10..0x1f -> 200ms..950ms, in 50ms steps */
		rawValue = (INT16)( ( codedValue - 0x10 ) * 50 ) + 200;
	}
	else if ( codedValue >= 0x20 && codedValue <= 0x3e )
	{
		/* 0x20..0x3e -> 1000ms..4000ms, in 100ms steps */
		rawValue = (INT16)( ( codedValue - 0x20 ) * 100 ) + 1000;
	}

	return( rawValue );
}

/* -----------------------------------------------------------------
 * Function    : Decode3GMaxSduSize
 * Description : Decodes 3G QoS Maximum SDU Size parameter value.
 * Parameters  : codedValue - encoded 8-bit value
 * Returns     : 16-bit decoded value, in octets
 * Notes       : See 3GPP TS 24.008/V3.11.0, Table 10.5.156 for
 *               the coding scheme.
 * ----------------------------------------------------------------- */
static INT16 Decode3GMaxSduSize( UINT8 codedValue )
{
	INT16 rawValue = 0;

	if ( codedValue >= 0x01 && codedValue <= 0x96 )
	{
		/* 0x01..0x96 -> 10..1500 octets, in 10-octet steps */
		rawValue = (INT16)( codedValue * 10 );
	}
	else if ( codedValue == 0x97 )
	{
		/* 0x97 -> 1502 octets */
		rawValue = 1502;
	}
	else if ( codedValue == 0x98 )
	{
		/* 0x98 -> 1510 octets */
		rawValue = 1510;
	}
	else if ( codedValue == 0x99 )
	{
		/* 0x99 -> 1520 octets */
		rawValue = 1520;
	}

	return( rawValue );
}

static void printSduErrRatio( char *outStr, CiPs3GSduErrorRatio SduErrRatio )
{
	switch ( SduErrRatio )
	{
	case CI_PS_3G_SDU_ERROR_RATIO_1EM2:
		sprintf( outStr, "1E2" );
		break;

	case CI_PS_3G_SDU_ERROR_RATIO_7EM3:
		sprintf( outStr, "7E3" );
		break;

	case CI_PS_3G_SDU_ERROR_RATIO_1EM3:
		sprintf( outStr, "1E3" );
		break;

	case CI_PS_3G_SDU_ERROR_RATIO_1EM4:
		sprintf( outStr, "1E4" );
		break;

	case CI_PS_3G_SDU_ERROR_RATIO_1EM5:
		sprintf( outStr, "1E5" );
		break;

	case CI_PS_3G_SDU_ERROR_RATIO_1EM6:
		sprintf( outStr, "1E6" );
		break;

	case CI_PS_3G_SDU_ERROR_RATIO_1EM1:
		sprintf( outStr, "1E1" );
		break;

	case  CI_PS_3G_SDU_ERROR_RATIO_SUBSCRIBED:
	default:
		sprintf( outStr, "0E0" );
		break;
	}
}

static void printResBER( char *outStr, CiPs3GResidualBer ResBer )
{
	switch ( ResBer )
	{
	case CI_PS_3G_RESIDUAL_BER_5EM2:
		sprintf( outStr, "5E2" );
		break;

	case CI_PS_3G_RESIDUAL_BER_1EM2:
		sprintf( outStr, "1E2" );
		break;

	case CI_PS_3G_RESIDUAL_BER_5EM3:
		sprintf( outStr, "5E3" );
		break;

	case CI_PS_3G_RESIDUAL_BER_4EM3:
		sprintf( outStr, "4E3" );
		break;

	case CI_PS_3G_RESIDUAL_BER_1EM3:
		sprintf( outStr, "1E3" );
		break;

	case CI_PS_3G_RESIDUAL_BER_1EM4:
		sprintf( outStr, "1E4" );
		break;

	case CI_PS_3G_RESIDUAL_BER_1EM5:
		sprintf( outStr, "1E5" );
		break;

	case CI_PS_3G_RESIDUAL_BER_1EM6:
		sprintf( outStr, "1E6" );
		break;

	case CI_PS_3G_RESIDUAL_BER_6EM8:
		sprintf( outStr, "6E8" );
		break;

	case  CI_PS_3G_RESIDUAL_BER_SUBSCRIBED:
	default:
		sprintf( outStr, "0E0" );
		break;
	}
}

/* Record PDP activation fail cause, refer to 3GPP Spec TS 24.008  section 6.1.3.1.3 */
static void getPdpErrCauseString( CiPsRc cause, char *sBuf )
{
	char *tBuf;

	switch (cause)
	{
	case CIRC_PS_GPRS_SERVICES_NOT_ALLOWED:
	case CIRC_PS_PLMN_NOT_ALLOWED:
	case CIRC_PS_ROAMING_NOT_ALLOWED:
	case CIRC_PS_INVALID_MS_CLASS:
		tBuf = "8  Operator Determined Barring";
		break;

	case CIRC_PS_LA_NOT_ALLOWED:
		tBuf = "26  insufficient resources";
		break;

	case CIRC_PS_PDP_AUTHEN_FAILURE:
		tBuf = "29  user authentication failed";
		break;

	case CIRC_PS_SRVOPT_NOT_SUPPORTED:
		tBuf = "32  service option not supported";
		break;

	case CIRC_PS_SRVOPT_NOT_SUBSCRIBED:
		tBuf = "33  requested service option not subscribed";
		break;
		
	case CIRC_PS_UNSPECIFIED_ERROR:
		tBuf = "31	activation rejected, unspecified";
		break;

	case CIRC_PS_SRVOPT_TEMP_OUT_OF_ORDER:
	default:
		tBuf = "34  service option temporarily out of order";
		break;

	}

	strcpy(sBuf, tBuf);
	DPRINTF("%s: %s!\n", __FUNCTION__, sBuf);

	return;
}

//Added by Michal Bukai - support for CMEE command (present eroor code\string)
/************************************************************************************
 * Function: checkPSRet
 *
 * Parameters:
 *
 * Returned value:
 *
 * Description:    prints appropriate message based on result code
 */
void checkPSRet( CiRequestHandle reqHandle, CiPsRc result )
{
	switch ( result )
	{
	case CIRC_PS_SUCCESS:               /* Request completed successfully         */
		ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;

	default:
		ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
		break;
	}
	return;
}

void psCnf(CiSgOpaqueHandle opSgCnfHandle,
	   CiServiceGroupID svgId,
	   CiPrimitiveID primId,
	   CiRequestHandle reqHandle,
	   void*            paras)
{
	UNUSEDPARAM(opSgCnfHandle)
	CiReturnCode ret = CIRC_SUCCESS;
	INT32 i;

	CiPsPrimEnableNwRegIndCnf        *nwRegIndCnf;
	CiPsPrimGetNwRegStatusCnf        *nwRegStatusCnf;
	CiPsPrimDefinePdpCtxCnf         *defPdpCnf;
	CiPsPrimGetAttachStateCnf       *getAttachCnf;
	CiPsPrimSetAttachStateCnf       *setAttachCnf;
	CiPsPrimSetPdpCtxActStateCnf    *pdpActCnf;
	CiPsPrimEnterDataStateCnf       *enterDataCnf;
	CiPsPrimSetQosCnf               *setQosCnf;
	CiPsPrimDefineSecPdpCtxCnf      *defPdpSecCnf;
	CiPsPrimGet3GQosCnf             *getQos3GCnf;
	CiPsPrimSet3GQosCnf             *setQos3GCnf;
	CiPsPrimGetPdpCtxCnf            *getPdpPrimCnf;
	CiPsPrimGetSecPdpCtxCnf         *getPdpSecCnf;
	CiPsPrimGetQosCnf               *getQosCnf;
	CiPsPrimGetQosCapsCnf           *getQosCapsCnf;
	CiPsPrimGet3GQosCapsCnf         *get3GQosCapsCnf;
	CiPsPrimGetPdpCtxReq     *getNxtPdpCtxReq;
	CiPsPrimGetQosReq        *getQosReq;
	CiPsPrimGet3GQosReq      *getQos3GReq;
	CiPsPrimGetPdpCtxReq  *getPdpCtxReq;
	CiPsPrimGetSecPdpCtxReq        *getSecPdpReq;
	CiPsPrimModifyPdpCtxCnf            *modifyPdpCtxCnf; //Added by Michal Buaki
	CiPsPrimGetPdpCtxsActStateCnf    *getCtxAtcListCnf; //Added by Michal Bukai
	/*Added by Michal Bukai - CGTFT support - START*/
	CiPsPrimDefineTftFilterCnf    *defineTftFilterCnf;
	CiPsPrimDeleteTftCnf         *deleteTftCnf;
	CiPsPrimGetTftCnf             *getTftCnf;
	CiPsPrimGetTftReq    *getTftReq;
	CiPsPrimFastDormantCnf *getFastDormancyCnf;
	CiPsPrimAuthenticateCnf *getAuthenticateCnf;

	/*Added by Michal Bukai - CGTFT support - END*/
	char AtRspBuf [500];
	char TempBuf [400];

	DBGMSG("psCnf: reqHandle(%d), primId(%d).\n", reqHandle, primId);


	/*
	**  Determine the primitive being confirmed.
	*/
	switch (primId)
	{
	case CI_PS_PRIM_ENABLE_NW_REG_IND_CNF:
	{
		nwRegIndCnf = (CiPsPrimEnableNwRegIndCnf *)paras;
		DBGMSG("%s: CI_PS_PRIM_ENABLE_NW_REG_IND_CNF reqHandle = %d.\n", __FUNCTION__, reqHandle);
		if ( nwRegIndCnf->rc == CIRC_PS_SUCCESS )
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, 0);
		else
			ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		break;
	}
	case CI_PS_PRIM_GET_NW_REG_STATUS_CNF:
	{
		nwRegStatusCnf = (CiPsPrimGetNwRegStatusCnf *)paras;
		if ( nwRegStatusCnf->rc == CIRC_PS_SUCCESS )
		{
			gCurrentPSRegStatus = nwRegStatusCnf->nwRegInfo.status;
			/* this must be received for an AT+CGREG? command */
			sprintf(AtRspBuf, "+CGREG: %d,%d", gCurrentPSRegOption, nwRegStatusCnf->nwRegInfo.status);

			/* display the location info only if regOption is CI_PS_NW_REG_IND_ENABLE_DETAIL */
			if (( gCurrentPSRegOption == CI_PS_NW_REG_IND_ENABLE_DETAIL ) &&
			    ( nwRegStatusCnf->nwRegInfo.present == TRUE))
			{
				sprintf(TempBuf, ",\"%04x\",\"%08x\",%d", nwRegStatusCnf->nwRegInfo.lac, nwRegStatusCnf->nwRegInfo.cellId, gActDetail);
				strcat(AtRspBuf, TempBuf);
			}

			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, AtRspBuf);
		}
		else
		{
			ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		}
		break;
	}
	case CI_PS_PRIM_GET_ATTACH_STATE_CNF:
	{
		getAttachCnf = (CiPsPrimGetAttachStateCnf *)paras;
		DBGMSG("%s: CI_PS_PRIM_GET_ATTACH_STATE_CNF reqHandle = %d.\n", __FUNCTION__, reqHandle);
		if ( getAttachCnf->rc == CIRC_PS_SUCCESS )
		{
			sprintf( (char *)AtRspBuf, "+CGATT: %d\r\n", getAttachCnf->state );
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf);
		}
		else
		{
			ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		}

		break;
	}

	case CI_PS_PRIM_SET_ATTACH_STATE_CNF:
	{
		setAttachCnf = (CiPsPrimSetAttachStateCnf *)paras;
		if ( setAttachCnf->rc == CIRC_PS_SUCCESS )
		{
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
			if (GET_REQ_ID(reqHandle) == PRI_PS_PRIM_SET_DETACH_STATE_REQ)
			{
				/*Report +CGEV */
				ATRESP( reqHandle, 0, 0, (char*)"+CGEV: ME DETACH");
				
				/*
				**  Flag all CIDs not in use
				*/
				resetCidList();
			}
		}
		else
		{
			ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
		}

		break;
	}

	case CI_PS_PRIM_DEFINE_PDP_CTX_CNF:
	{
		defPdpCnf = (CiPsPrimDefinePdpCtxCnf *)paras;
		UINT8 atpIdx = GET_ATP_INDEX(reqHandle);
		if(gCIDList.currCntx[atpIdx].currCid >= CI_PS_MAX_PDP_CTX_NUM)
		{
			resetCurrCntx(atpIdx);
			ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
			break;
		}

		if ( defPdpCnf->rc == CIRC_PS_SUCCESS )
		{
			/* this was sent when an enter data was requested with no PDP context defined */
			if ( GET_REQ_ID(reqHandle) == CI_PS_PRIM_ENTER_DATA_STATE_REQ )
			{
				/* mark connection waiting to enter the data mode */

				CiPsPrimEnterDataStateReq       *pdpEnterDataReq;
				pdpEnterDataReq = (CiPsPrimEnterDataStateReq *)gCIDList.currCntx[atpIdx].reqMsg;
				if (pdpEnterDataReq == NULL)
				{
					resetCurrCntx(atpIdx);
					ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, CME_UNKNOWN, NULL);
				}
				else
				{
					gCIDList.cInfo[gCIDList.currCntx[atpIdx].currCid].bDefined = TRUE;
					gCIDList.currCntx[atpIdx].reqMsg = NULL;

					/* make request to activate context and enter data mode */
					ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_ENTER_DATA_STATE_REQ,
						   reqHandle, (void *)pdpEnterDataReq );
				}
			}
			else
			{
				gCIDList.cInfo[gCIDList.currCntx[atpIdx].currCid].bDefined = TRUE;
				ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
			}
		}
		else
		{
			/* reset PS related info */
			resetCurrCntx(atpIdx);
			/* Save PDP activation fail cause in local variable */
			getPdpErrCauseString(defPdpCnf->rc, gPdpErrCauseBuf);

			ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);

		}

		break;
	}

	case CI_PS_PRIM_SET_PDP_CTX_ACT_STATE_CNF:
	{
		UINT8 currCid;
		pdpActCnf = (CiPsPrimSetPdpCtxActStateCnf *)paras;
		DBGMSG("[%s]:line(%d), CI_PS_PRIM_SET_PDP_CTX_ACT_STATE_CNF received.\n", __FUNCTION__, __LINE__ );
		DBGMSG("[%s]:line(%d), pdpActCnf->rc(%d).\n", __FUNCTION__, __LINE__, pdpActCnf->rc );


		currCid = gCIDList.currCntx[GET_ATP_INDEX(reqHandle)].currCid;

		if(currCid >= CI_PS_MAX_PDP_CTX_NUM)
		{
			resetCurrCntx(GET_ATP_INDEX(reqHandle));
			ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
			break;
		}

		if ( pdpActCnf->rc == CIRC_PS_SUCCESS )
		{

			DBGMSG("[%s]:line(%d), currCid(%d), reqHandle(%d).\n", __FUNCTION__, __LINE__, currCid, reqHandle);

			if (GET_REQ_ID(reqHandle) == PRI_PS_PRIM_SET_PDP_CTX_DEACT_STATE_REQ)
			{

				resetCurrCntx(GET_ATP_INDEX(reqHandle));

				//removeNodeByCid(currCid);

				/* Delete the CID Context */
				//        PS_DeleteGPRSContext(IND_REQ_HANDLE, currCid);

				ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL); //OK for AT+CGACT=0,*
		    #if 0
				/* if it was a deactivate request */
				if ( gAtciConnType == ATCI_WAIT_PS_DEACTIVATE )
				{

					//    sleep( WAIT_TIME_BEFORE_PDP_CTX_DELETE);
					/* Delete PDP context */
					deletePdpCtxReq.cid = gCurrentPdpSetCtx.cid;
					DBGMSG("[%s]:line(%d), gCurrentPdpSetCtx.cid(%d).\n", __FUNCTION__, __LINE__, gCurrentPdpSetCtx.cid );

					//    sleep(1);
					if (gCurrentPdpSetCtx.pdpType == CI_PS_PDP_TYPE_IP)
					{
						removeNodeByCid(gCurrentPdpSetCtx.cid);
						ioctl(cinetdevfd, CCINET_IP_DISABLE, gCurrentPdpSetCtx.cid);
					}
				}
		    #endif
			}
			else    /*Save the current Handle */
			{
				gCIDList.cInfo[currCid].reqHandle = reqHandle;
				ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
			}
		}
		else
		{
			resetCurrCntx(GET_ATP_INDEX(reqHandle));

			/* Save PDP activation fail cause in local variable */
			getPdpErrCauseString(pdpActCnf->rc, gPdpErrCauseBuf);
			/* Delete the CID Context */
			//        PS_DeleteGPRSContext(IND_REQ_HANDLE, currCid);

			ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		}

		break;
	}

	case CI_PS_PRIM_ENTER_DATA_STATE_CNF:
	{
		DBGMSG("[%s]:line(%d), CI_PS_PRIM_ENTER_DATA_STATE_CNF received.\n", __FUNCTION__, __LINE__ );
		enterDataCnf = (CiPsPrimEnterDataStateCnf *)paras;
		DBGMSG("[%s]:line(%d), enterDataCnf->rc(%d).\n", __FUNCTION__, __LINE__, enterDataCnf->rc );

		UINT8 cid = gCIDList.currCntx[GET_ATP_INDEX(reqHandle)].currCid;

		if(cid >= CI_PS_MAX_PDP_CTX_NUM)
		{
			resetCurrCntx(GET_ATP_INDEX(reqHandle));
			ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
			break;
		}

		if ( enterDataCnf->rc == CIRC_PS_SUCCESS )
		{
			DBGMSG("CI_PS_PRIM_ENTER_DATA_STATE_CNF, cid=%d, reqHandle=%d\n", cid, reqHandle);
			gCIDList.cInfo[cid].reqHandle = reqHandle;

			if (bLocalTest)
			{
				/* Try to obtain the IP address information for the activated CID*/
				getPdpCtxReq = utlCalloc(1, sizeof(*getPdpCtxReq));
				if (getPdpCtxReq == NULL) {
					ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
					break;
				}
				getPdpCtxReq->cid = cid;
				ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_PDP_CTX_REQ,
					   reqHandle, (void *)getPdpCtxReq );

				//Here we delay to transfer the CONNECT to TE until we receive the DATA_IND_OK from COMM
				//ATRESP( reqHandle,0, 0, (char *)"CONNECT\r\n");
			}
		    #if 0
			/* update the connection state */
			switch ( gCIDList[gCurrentPdpSetCtx.cid].connType )
			{
			case ATCI_WAIT_PS_ENTER_DATA:
			{
				/* mark waiting the ready for data transfer indication */
				gCIDList[gCurrentPdpSetCtx.cid].connType = ATCI_WAIT_PS_DATA_OK;
				gAtciConnType = gCIDList[gCurrentPdpSetCtx.cid].connType;

				getPdpCtxReq.cid = gCurrentPdpSetCtx.cid;
				if (gCurrentPdpSetCtx.pdpType == CI_PS_PDP_TYPE_IP)
				{
					ioctl(cinetdevfd, CCINET_IP_ENABLE, gCurrentPdpSetCtx.cid);
				}
				else
				{
					if ( gAtciPppState >= ATCI_PPP_REQUESTED)
					{

						//    ioctl(cctdatafd, TIOENABLE, gCurrentPdpSetCtx.cid);
					}
				}
				DBGMSG("CI_PS_PRIM_ENTER_DATA_STATE_CNF, cid=%d\n", getPdpCtxReq.cid);
				ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_PDP_CTX_REQ,
					   reqHandle, (void *)&getPdpCtxReq );

				DBGMSG("%s: at line %d with rehandle: %d\n", __FUNCTION__, __LINE__, reqHandle);
				ATRESP( reqHandle, 0, 0, (char *)"CONNECT\r\n");

				break;
			}

			default:
			{
				ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);

				/* Delete PDP context */
				deletePdpCtxReq.cid = gCurrentPdpSetCtx.cid;

				ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_DELETE_PDP_CTX_REQ,
					   reqHandle, (void *)&deletePdpCtxReq );

				break;
			}
			}
		    #endif
		}
		else
		{
			resetCurrCntx(GET_ATP_INDEX(reqHandle));
			/* Save PDP activation fail cause in local variable */
			getPdpErrCauseString(enterDataCnf->rc, gPdpErrCauseBuf);

			ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);

			/* Delete the CID Context */
			//    PS_DeleteGPRSContext(IND_REQ_HANDLE, cid);
		}

		break;
	}

	case CI_PS_PRIM_GET_QOS_CAPS_CNF:
	{
		getQosCapsCnf = (CiPsPrimGetQosCapsCnf *)paras;
		if ( getQosCapsCnf->rc == CIRC_PS_SUCCESS )
		{
			if (getQosCapsCnf->qosCapsPresent)
			{
				CiPsQosCap *pCtxCaps = getQosCapsCnf->qosCaps.caps;

				for ( i = 0; i < getQosCapsCnf->qosCaps.size; i++ )
				{
					/* get PDP type string */
					getPdpTypeStr( pCtxCaps->type, TempBuf );

					/* requested Qos is not defined for this CID */
					if (GET_REQ_ID(reqHandle) == PRI_PS_PRIM_GET_MIN_QOS_CAPS_REQ)
					{
						sprintf( (char *)AtRspBuf, "+CGQMIN: %s,(%d-%d),(%d-%d),", TempBuf,
							 pCtxCaps->precedenceCap.min,   /* Precedence class [0-4] */
							 pCtxCaps->precedenceCap.max,   /* Precedence class [0-4] */
							 pCtxCaps->delayCap.min,        /* Delay class [0-3] */
							 pCtxCaps->delayCap.max );      /* Delay class [0-3] */
						sprintf( (char *)TempBuf, "(%d-%d),(%d-%d),(",
							 pCtxCaps->reliabilityCap.min,  /* Reliability class [0-5] */
							 pCtxCaps->reliabilityCap.max,  /* Reliability class [0-5] */
							 pCtxCaps->peakCap.min,         /* Peak throughput [0-9] */
							 pCtxCaps->peakCap.max );       /* Peak throughput [0-9] */
						strcat( AtRspBuf, TempBuf );
					}
					else
					{
						sprintf( (char *)AtRspBuf, "+CGQREQ: %s,(%d-%d),(%d-%d),", TempBuf,
							 pCtxCaps->precedenceCap.min,   /* Precedence class [0-4] */
							 pCtxCaps->precedenceCap.max,   /* Precedence class [0-4] */
							 pCtxCaps->delayCap.min,        /* Delay class [0-3] */
							 pCtxCaps->delayCap.max );      /* Delay class [0-3] */
						sprintf( (char *)TempBuf, "(%d-%d),(%d-%d),(",
							 pCtxCaps->reliabilityCap.min,  /* Reliability class [0-5] */
							 pCtxCaps->reliabilityCap.max,  /* Reliability class [0-5] */
							 pCtxCaps->peakCap.min,         /* Peak throughput [0-9] */
							 pCtxCaps->peakCap.max );       /* Peak throughput [0-9] */
						strcat( AtRspBuf, TempBuf );
					}

					if ( pCtxCaps->meanCap.hasRange == TRUE )
					{
						sprintf( (char *)TempBuf, "%d-%d", pCtxCaps->meanCap.rangeLst[0].min,
							 pCtxCaps->meanCap.rangeLst[0].max );
						strcat( (char *)AtRspBuf, (char *)TempBuf );

						for (i = 1; i < pCtxCaps->meanCap.rangeLstSize; i++ )
						{
							sprintf( (char *)TempBuf, ",%d-%d", pCtxCaps->meanCap.rangeLst[i].min,
								 pCtxCaps->meanCap.rangeLst[i].max );
							strcat( (char *)AtRspBuf, (char *)TempBuf );
						}
					}

					if ( pCtxCaps->meanCap.hasIndvNums == TRUE )
					{
						for ( i = 0; i < pCtxCaps->meanCap.indvLstSize; i++ )
						{
							sprintf( (char *)TempBuf, ",%d", pCtxCaps->meanCap.indvList[i] );
							strcat( (char *)AtRspBuf, (char *)TempBuf );
						}
					}

					strcat( (char *)AtRspBuf, ")\r\n" );

					ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf);

					pCtxCaps++;
				}
			}
		}
		else
		{
			ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
		}

		break;
	}

	case CI_PS_PRIM_GET_QOS_CNF:
	{
		getQosCnf = (CiPsPrimGetQosCnf *)paras;
		UINT8 currCid = gCIDList.currCntx[GET_ATP_INDEX(reqHandle)].currCid;

		if(currCid >= CI_PS_MAX_PDP_CTX_NUM)
		{
			resetCurrCntx(GET_ATP_INDEX(reqHandle));
			ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
			break;
		}

		if (( getQosCnf->rc == CIRC_PS_SUCCESS ) && getQosCnf->qosProfPresent )
		{
			/* requested Qos is not defined for this CID */
			if (GET_REQ_ID(reqHandle) == PRI_PS_PRIM_GET_MIN_QOS_REQ)
			{
				sprintf( (char *)AtRspBuf, "CGQMIN: %d,%d,%d,%d,", currCid + 1,
					 getQosCnf->qosProf.precedence,
					 getQosCnf->qosProf.delay,
					 getQosCnf->qosProf.reliability);
				sprintf( TempBuf, "%d,%d\r\n",
					 getQosCnf->qosProf.peak,
					 getQosCnf->qosProf.mean );
				strcat( AtRspBuf, TempBuf );
				ATRESP( reqHandle, 0, 0, (char *)AtRspBuf);
			}
			else
			{
				sprintf( (char *)AtRspBuf, "CGQREQ: %d,%d,%d,%d,\r\n", currCid + 1,
					 getQosCnf->qosProf.precedence,
					 getQosCnf->qosProf.delay,
					 getQosCnf->qosProf.reliability);
				sprintf( TempBuf, "%d,%d\r\n",
					 getQosCnf->qosProf.peak,
					 getQosCnf->qosProf.mean );
				strcat( AtRspBuf, TempBuf );
				ATRESP( reqHandle, 0, 0, (char *)AtRspBuf);
			}
		}



		/* send query for next CID */
		currCid++;
		if ( currCid < CI_PS_MAX_PDP_CTX_NUM )
		{
			getQosReq = utlCalloc(1, sizeof(*getQosReq));
			if (getQosReq == NULL) {
				ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
				break;
			}

			gCIDList.currCntx[GET_ATP_INDEX(reqHandle)].currCid = currCid;
			getQosReq->cid = currCid;
			getQosReq->isMin = ( GET_REQ_ID(reqHandle) == PRI_PS_PRIM_GET_MIN_QOS_REQ ) ? TRUE : FALSE;
			ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_QOS_REQ,
					 reqHandle, (void *)getQosReq );
		}
		else
		{
			//resetCurrCntx(GET_ATP_INDEX(reqHandle));
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}

		break;
	}
	case CI_PS_PRIM_SET_QOS_CNF:
	{
		setQosCnf = (CiPsPrimSetQosCnf *)paras;
		if ( setQosCnf->rc == CIRC_PS_SUCCESS )
		{
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
		else
		{
			ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		}
		break;
	}

	case CI_PS_PRIM_DEFINE_SEC_PDP_CTX_CNF:
	{
		defPdpSecCnf = (CiPsPrimDefineSecPdpCtxCnf *)paras;
		if ( defPdpSecCnf->rc == CIRC_PS_SUCCESS )
		{
			UINT8 atpIdx = GET_ATP_INDEX(reqHandle);
			if(gCIDList.currCntx[atpIdx].currCid >= CI_PS_MAX_PDP_CTX_NUM)
			{
				resetCurrCntx(atpIdx);
				ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
				break;
			}
			gCIDList.cInfo[gCIDList.currCntx[atpIdx].currCid].bDefined = TRUE;
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
		else
		{
			resetCurrCntx(GET_ATP_INDEX(reqHandle));
			ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		}
		break;
	}

	case CI_PS_PRIM_GET_SEC_PDP_CTX_CNF:
	{

		/* query was for an AT+CGDSCONT */
		getPdpSecCnf = (CiPsPrimGetSecPdpCtxCnf *)paras;
		if (( getPdpSecCnf->rc == CIRC_PS_SUCCESS ) &&  getPdpSecCnf->ctxPresent )
		{
			sprintf( (char *)AtRspBuf, "+CGDSCONT: %d,%d,",
				 getPdpSecCnf->ctx.secPdpCtx.cid + 1,
				 getPdpSecCnf->ctx.secPdpCtx.p_cid + 1 );

			if ( getPdpSecCnf->ctx.secPdpCtx.dcompPresent )
			{
				sprintf((char *)TempBuf, "%d,", getPdpSecCnf->ctx.secPdpCtx.dcomp);
			}
			else
			{
				sprintf((char *)TempBuf, ",");
			}

			strcat((char *)AtRspBuf, (char *)TempBuf);

			if ( getPdpSecCnf->ctx.secPdpCtx.hcompPresent )
			{
				sprintf((char *)TempBuf, "%d", getPdpSecCnf->ctx.secPdpCtx.hcomp);
			}

			strcat((char *)AtRspBuf, (char *)TempBuf);
			strcat((char *)AtRspBuf, "\r\n");

			ATRESP( reqHandle, 0, 0, (char *)AtRspBuf);
		}

		UINT8 nextCid = gCIDList.currCntx[GET_ATP_INDEX(reqHandle)].currCid + 1;

		/* did we receive all confirmations ? */
		if ( nextCid < CI_PS_MAX_PDP_CTX_NUM )
		{
			/* Require the next CI */
			getSecPdpReq = utlCalloc(1, sizeof(*getSecPdpReq));
			if (getSecPdpReq == NULL) {
				ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
				break;
			}

			getSecPdpReq->cid = nextCid;

			gCIDList.currCntx[GET_ATP_INDEX(reqHandle)].currCid = nextCid;

			/*
			**  Send the CI Request.
			*/
			ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_SEC_PDP_CTX_REQ,
					 reqHandle, (void *)getSecPdpReq );
		}
		else
		{
			/* Received all the response */
			//resetCurrCntx(GET_ATP_INDEX(reqHandle));
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}

		break;
	}

	case CI_PS_PRIM_GET_PDP_CTX_CNF:
	{
		static INT32 activePrimPdp[CI_PS_MAX_PDP_CTX_NUM] = { 0, 0, 0, 0, 0, 0, 0, 0 };
		static INT32 idx = 0;
		static INT32 activeCounter = 0;
		UINT8 currCid;

		currCid = gCIDList.currCntx[GET_ATP_INDEX(reqHandle)].currCid;

		if(currCid >= CI_PS_MAX_PDP_CTX_NUM)
		{
			resetCurrCntx(GET_ATP_INDEX(reqHandle));
			ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
			break;
		}


		DBGMSG("got CI_PS_PRIM_GET_PDP_CTX_CNF\n");
		DBGMSG("gCurrentPdpSetCtx.cid=%d, .pdpType:%d\n", currCid, gCIDList.cInfo[currCid].pdpType);
#if 0
		/* Only for local test we need to preserve the IP address */
		if (gCIDList.cInfo[currCid].pdpType == CI_PS_PDP_TYPE_IP)
		{
			getPdpPrimCnf = (CiPsPrimGetPdpCtxCnf *)paras;
			DBGMSG("CI_PS_PRIM_GET_PDP_CTX_CNF,rc=%d\n", getPdpPrimCnf->rc);
			if (getPdpPrimCnf->ctxPresent)
			{
				pCtx = &getPdpPrimCnf->ctx;
				cid = pCtx->pdpCtx.cid;
				/* it is meaningful only when it gets PDP Address */
				if (pCtx->pdpCtx.addrPresent)
				{
					/* First we delete the cid if it exists */
					removeNodeByCid(cid);
					pNode = malloc(sizeof(DIRECTIPCONFIGLIST));
					ASSERT(pNode != NULL);
					pNode->next = NULL;

					pNode->directIpAddress.dwContextId = pCtx->pdpCtx.cid;
					if (pCtx->pdpCtx.type == CI_PS_PDP_TYPE_IP)
					{
						DBGMSG("type CI_PS_PDP_TYPE_IP\n");
						pNode->directIpAddress.dwProtocol = CI_PS_PDP_TYPE_IP;
						pNode->directIpAddress.ipv4.inSubnetMask = 0xFFFFFF00; //255.255.255.0

						//reconstruct IP addr in network order
						if (pCtx->pdpCtx.addrPresent)
						{
							UINT8 *ipaddr;
							pNode->directIpAddress.ipv4.inIPAddress = 0;
							ipaddr = pCtx->pdpCtx.addr.valData;
							pNode->directIpAddress.ipv4.inIPAddress |= (*ipaddr << 24);
							ipaddr++;
							pNode->directIpAddress.ipv4.inIPAddress |= (*ipaddr << 16);
							ipaddr++;
							pNode->directIpAddress.ipv4.inIPAddress |= (*ipaddr << 8);
							ipaddr++;
							pNode->directIpAddress.ipv4.inIPAddress |= *ipaddr;
							//SCR2113833: data_abort during 2 PDP contexts live testing in UK
							pNode->directIpAddress.ipv4.inDefaultGateway = pNode->directIpAddress.ipv4.inIPAddress ^ 0xFF;

							DPRINTF("inIPAddr=%x\n", pNode->directIpAddress.ipv4.inIPAddress);
							DPRINTF("inIPAddr=%d.%d.%d.%d\n", (pNode->directIpAddress.ipv4.inIPAddress & 0xff000000) >> 24,
								(pNode->directIpAddress.ipv4.inIPAddress & 0x00ff0000) >> 16, (pNode->directIpAddress.ipv4.inIPAddress & 0x0000ff00) >> 8,
								(pNode->directIpAddress.ipv4.inIPAddress & 0x0000ff));
							DPRINTF("inDefaultGateway=%x\n", pNode->directIpAddress.ipv4.inDefaultGateway);
							DPRINTF("inDefaultGateway=%d.%d.%d.%d\n", (pNode->directIpAddress.ipv4.inIPAddress & 0xff000000) >> 24,
								(pNode->directIpAddress.ipv4.inIPAddress & 0x00ff0000) >> 16, (pNode->directIpAddress.ipv4.inIPAddress & 0x0000ff00) >> 8,
								(pNode->directIpAddress.ipv4.inIPAddress ^ 0xFF) & 0xff);

							// calculate mask according to ip address
							oct = (UINT8)((pNode->directIpAddress.ipv4.inIPAddress & 0xFF000000) >> 24);
							if (oct >= 192)
							{
								pNode->directIpAddress.ipv4.inSubnetMask = 0xFFFFFF00; // 255.255.255.0
							}
							else if (oct >= 128)
							{
								pNode->directIpAddress.ipv4.inSubnetMask = 0xFFFF0000; // 255.255.0.0
							}
							else
							{
								pNode->directIpAddress.ipv4.inSubnetMask = 0xFF000000; // 255.0.0.0
							}
							DPRINTF("inSubnetMask=%x\n", pNode->directIpAddress.ipv4.inSubnetMask);


						}

						// parse the following info according to IPCP and PPP
						if (pCtx->pdpCtx.pdParasPresent)
						{
							UINT8 *buf, *packetEnd, *ipcpEnd;

							buf = (UINT8*)pCtx->pdpCtx.pdParas.valStr;
							packetEnd = buf + pCtx->pdpCtx.pdParas.len;
							while (buf < packetEnd)
							{

								unsigned short type = buf[0] << 8 | buf[1];
								UINT8 len = buf[2]; //  this length field includes only what follows it
								switch (type)
								{
								// we are currently interested only on one type
								// but will specify some more for fute
								case 0x23C0:
									// PAP - not used  - ignore
									buf += len + 3;
									break;
								case 0x8021:
									// IPCP option for IP configuration - we are looking for DNS parameters.
									// it seem that this option may appear more than once!
									ipcpEnd = buf + len + 3;
									buf += 3;
									// 3gpp TS29.061 conf-nak or conf-ack both can be here and has DNS infomation
									if (*buf == 0x02 || *buf == 0x03)
									{
										// Config-Nak/Ack found, advance to IPCP data start
										buf += 4;
										// parse any configuration
										while (buf < ipcpEnd)
										{
											if (*buf == 129)
											{
												// Primary DNS address
												buf += 2;
												DPRINTF("Primary DNS %d.%d.%d.%d\n", buf[0], buf[1], buf[2], buf[3]);
												pNode->directIpAddress.ipv4.inPrimaryDNS = ((buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3]);
												buf += 4;
												continue;
											}
											if (*buf == 131)
											{
												// Secondary DNS address
												buf += 2;
												DPRINTF("secondary DNS %d.%d.%d.%d\r\n", buf[0], buf[1], buf[2], buf[3]);
												pNode->directIpAddress.ipv4.inSecondaryDNS = ((buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3]);
												buf += 4;
												continue;
											}
											// buf[0] includes the ipcp type, buf[1] the length of this field includes the whole TLV
											buf += buf[1];
										}
									}
									else
									{
										buf += len;
									}
									break;
								default:
									buf += len + 3;
									break;
								}
							}
						}
						else
						{
							DBGMSG("no pPdParas!\n");
						}

					}
					else if (pCtx->pdpCtx.type == CI_PS_PDP_TYPE_IPV6)
					{
						pNode->directIpAddress.dwProtocol = CI_PS_PDP_TYPE_IPV6;
						ERRMSG("IPV6 not implemented yet\n");
					}
					addToList(pNode);
				}
			}
		}
#endif
		/* a query operation must be in progress */
		if (GET_REQ_ID(reqHandle) == PRI_PS_PRIM_GET_SECOND_PDP_CTXS_RANGE_REQ)
		{
			/* query was for an AT+CGDSCONT=? */
			getPdpPrimCnf = (CiPsPrimGetPdpCtxCnf *)paras;
			if (( getPdpPrimCnf->rc == CIRC_PS_SUCCESS ) && getPdpPrimCnf->ctxPresent )
			{
				/* check if active context */
				if ( getPdpPrimCnf->ctx.actState )
				{
					activePrimPdp[activeCounter++] = getPdpPrimCnf->ctx.pdpCtx.cid + 1;
				}
			}

			/* send request for next CID  */
			currCid++;
			if ( currCid < CI_PS_MAX_PDP_CTX_NUM )
			{
				/* request info about every primary context */
				getNxtPdpCtxReq = utlCalloc(1, sizeof(*getNxtPdpCtxReq));
				if (getNxtPdpCtxReq == NULL) {
					ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
					break;
				}
				getNxtPdpCtxReq->cid = currCid;

				gCIDList.currCntx[GET_ATP_INDEX(reqHandle)].currCid = currCid;

				/*
				**  Send the CI Request.
				*/
				ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_PDP_CTX_REQ,
						 reqHandle, (void *)getNxtPdpCtxReq );
			}
			else
			{
				/*  we received all confirmations ? */
				sprintf( (char *)AtRspBuf, "+CGDSCONT: (1-%d),", CI_PS_MAX_PDP_CTX_NUM);

				if ( activeCounter > 0 )
				{
					sprintf((char *)TempBuf, "(%d", activePrimPdp[0]);
					strcat((char *)AtRspBuf, (char *)TempBuf);

					for ( idx = 1; idx < activeCounter; idx++ )
					{
						sprintf((char *)TempBuf, ", %d", activePrimPdp[idx]);
						strcat((char *)AtRspBuf, (char *)TempBuf);
					}
				}
				else
				{
					strcat((char *)AtRspBuf, "(");
				}

				strcat((char *)AtRspBuf, "),(0,2),(0,1)");

				ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf);

				/* reinit all variables */
				//resetCurrCntx(GET_ATP_INDEX(reqHandle));
				activeCounter = 0;
				for ( idx = 0; idx < CI_PS_MAX_PDP_CTX_NUM; idx++ )
				{
					activePrimPdp[idx] = 0;
				}
			}
		}       /* if ( ==PRI_PS_PRIM_GET_SECOND_PDP_CTXS_RANGE_REQ  */

		else if (GET_REQ_ID(reqHandle) == CI_PS_PRIM_GET_ACTIVE_CID_LIST_REQ)
		{
			/* query was for an AT+CGACT */
			getPdpPrimCnf = (CiPsPrimGetPdpCtxCnf *)paras;

			/* act as per status */
			if (( getPdpPrimCnf->rc == CIRC_PS_SUCCESS ) &&  getPdpPrimCnf->ctxPresent )
			{
				sprintf( (char *)AtRspBuf, "+CGACT: %d,", getPdpPrimCnf->ctx.pdpCtx.cid + 1 );

				/* check if active context */
				if ( getPdpPrimCnf->ctx.actState )
				{
					strcat((char *)AtRspBuf, "1\r\n"); /* activated */
				}
				else
				{
					strcat((char *)AtRspBuf, "0\r\n"); /* deactivated */
				}

				ATRESP( reqHandle, 0, 0, (char *)AtRspBuf);
			}
			else
			{
				/* just ignore the confirmation - nothing to dislplay*/
			}

			/* send request for next CID  */
			currCid++;
			if ( currCid < CI_PS_MAX_PDP_CTX_NUM )
			{
				/* request info about every primary context */
				getNxtPdpCtxReq = utlCalloc(1, sizeof(*getNxtPdpCtxReq));
				if (getNxtPdpCtxReq == NULL) {
					ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
					break;
				}

				getNxtPdpCtxReq->cid = currCid;

				gCIDList.currCntx[GET_ATP_INDEX(reqHandle)].currCid = currCid;

				/*
				**  Send the CI Request.
				*/
				ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_PDP_CTX_REQ,
						 reqHandle, (void *)getNxtPdpCtxReq );
			}
			else
			{
				/* completed operation */
				ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
				//resetCurrCntx(GET_ATP_INDEX(reqHandle));
			}
		}       /* if ( ==CI_PS_PRIM_GET_ACTIVE_CID_LIST_REQ ) */
		   /*Added by Michal Bukai - CGTFT support*/
		else if (GET_REQ_ID(reqHandle) == PRI_PS_PRIM_GET_TFT_REQ)
		{
			getPdpPrimCnf = (CiPsPrimGetPdpCtxCnf *)paras;

			if (( getPdpPrimCnf->rc == CIRC_PS_SUCCESS ) &&  ( getPdpPrimCnf->ctxPresent))
			{
				getTftReq = utlCalloc(1, sizeof(*getTftReq));
				if (getTftReq == NULL) {
					ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
					break;
				}

				getTftReq->cid = getPdpPrimCnf->ctx.pdpCtx.cid;
				ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_TFT_REQ,
						 reqHandle, (void *)getTftReq );
			}
			else
			{
				/* just ignore the confirmation - nothing to dislplay*/
			}

			/* send request for next CID  */
			currCid++;
			if ( currCid < CI_PS_MAX_PDP_CTX_NUM )
			{
				/* request info about every primary context */
				getNxtPdpCtxReq = utlCalloc(1, sizeof(*getNxtPdpCtxReq));
				if (getNxtPdpCtxReq == NULL) {
					ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
					break;
				}

				getNxtPdpCtxReq->cid = currCid;

				gCIDList.currCntx[GET_ATP_INDEX(reqHandle)].currCid = currCid;

				/*
				**  Send the CI Request.
				*/
				ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_PDP_CTX_REQ,
						 reqHandle, (void *)getNxtPdpCtxReq );
			}
			else
			{
				/* completed operation */
				ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
				//resetCurrCntx(GET_ATP_INDEX(reqHandle));
			}
		}       /* if ( gAtCgTftQuery ) */

		else if (GET_REQ_ID(reqHandle) == CI_PS_PRIM_GET_PDP_CTX_REQ)
		{
			/* query was for an AT+CGDCONT? */
			getPdpPrimCnf = (CiPsPrimGetPdpCtxCnf *)paras;

			if (( getPdpPrimCnf->rc == CIRC_PS_SUCCESS ) &&  ( getPdpPrimCnf->ctxPresent))
			{
				sprintf( (char *)AtRspBuf, "+CGDCONT: %d,", getPdpPrimCnf->ctx.pdpCtx.cid + 1 );

				/* PDP type number */
				getPdpTypeStr( getPdpPrimCnf->ctx.pdpCtx.type, TempBuf );
				strcat((char *)AtRspBuf, (char *)TempBuf);
				strcat((char *)AtRspBuf, ",\"" );

				/* PDP access point */
				if ( getPdpPrimCnf->ctx.pdpCtx.apnPresent )
				{
					memcpy( TempBuf, getPdpPrimCnf->ctx.pdpCtx.apn.valStr, getPdpPrimCnf->ctx.pdpCtx.apn.len );
					TempBuf[getPdpPrimCnf->ctx.pdpCtx.apn.len] = '\0';
				}
				else
				{
					TempBuf[0] = '\0';
				}

				strcat((char *)AtRspBuf, (char *)TempBuf);
				strcat((char *)AtRspBuf, "\",\"" );

				/* PDP address */
				if ( getPdpPrimCnf->ctx.pdpCtx.addrPresent )
				{
					char ipp[MAX_ADDR_PRESENTATION_BUF_LEN];
					int af = getPdpPrimCnf->ctx.pdpCtx.addr.len == CI_PS_PDP_IP_V6_SIZE ? AF_INET6 : AF_INET;
					inet_ntop(af, getPdpPrimCnf->ctx.pdpCtx.addr.valData, ipp, sizeof(ipp));
					strcat((char *)AtRspBuf, (char *)ipp);
				}

				strcat((char *)AtRspBuf, "\",");

				/* PDP data compression */
				if ( getPdpPrimCnf->ctx.pdpCtx.dcompPresent )
				{
					sprintf( (char *)TempBuf, "%d", getPdpPrimCnf->ctx.pdpCtx.dcomp );
					strcat((char *)AtRspBuf, (char *)TempBuf);
				}

				strcat((char *)AtRspBuf, ",");

				/* PDP header compression */
				if ( getPdpPrimCnf->ctx.pdpCtx.hcompPresent )
				{
					sprintf( (char *)TempBuf, "%d", getPdpPrimCnf->ctx.pdpCtx.hcomp );
					strcat((char *)AtRspBuf, (char *)TempBuf);
				}

				strcat((char *)AtRspBuf, ",");

				/* string parameters whose meanings are specific to the PDP type */
				if ( getPdpPrimCnf->ctx.pdpCtx.pdParasPresent )
				{
					DBGMSG("CI_PS_PRIM_GET_PDP_CTX_CNF: ctx.pdpCtx.pdParas.len:%d!\n", getPdpPrimCnf->ctx.pdpCtx.pdParas.len);
					for ( i = 0; i < getPdpPrimCnf->ctx.pdpCtx.pdParas.len; i++ )
					{
						sprintf( (char *)TempBuf, "%02x", (UINT8)getPdpPrimCnf->ctx.pdpCtx.pdParas.valStr[i]);
						strcat( (char *)AtRspBuf, (char *)TempBuf );
					}
				}

				strcat((char *)AtRspBuf, ",\r\n");

				ATRESP( reqHandle, 0, 0, (char *)AtRspBuf);
			}       /* if (CIRC_PS_SUCCESS) */
			else
			{
				/* just ignore the confirmation - nothing to display*/
			}

			/* send request for next CID  */
			currCid++;
			if ( currCid < CI_PS_MAX_PDP_CTX_NUM )
			{
				/* request info about every primary context */
				getNxtPdpCtxReq = utlCalloc(1, sizeof(*getNxtPdpCtxReq));
				if (getNxtPdpCtxReq == NULL) {
					ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
					break;
				}

				getNxtPdpCtxReq->cid = currCid;

				gCIDList.currCntx[GET_ATP_INDEX(reqHandle)].currCid = currCid;

				/*
				**  Send the CI Request.
				*/
				ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_PDP_CTX_REQ,
						 reqHandle, (void *)getNxtPdpCtxReq );
			}
			else
			{
				/* completed operation */
				ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
				//resetCurrCntx(GET_ATP_INDEX(reqHandle));
			}
		}
		else if (GET_REQ_ID(reqHandle) == PRI_PS_PRIM_GET_IP_REQ)
		{
			getPdpPrimCnf = (CiPsPrimGetPdpCtxCnf *)paras;

			if (( getPdpPrimCnf->rc == CIRC_PS_SUCCESS ) &&  ( getPdpPrimCnf->ctxPresent) &&  getPdpPrimCnf->ctx.pdpCtx.addrPresent)
			{
				int len = getPdpPrimCnf->ctx.pdpCtx.addr.len;
				int af = len == CI_PS_PDP_IP_V6_SIZE ? AF_INET6 : AF_INET;
				const UINT8 *ip = getPdpPrimCnf->ctx.pdpCtx.addr.valData;
				UINT8 gw[CI_PS_PDP_IP_V6_SIZE];
				char ipp[MAX_ADDR_PRESENTATION_BUF_LEN];
				char gwp[MAX_ADDR_PRESENTATION_BUF_LEN];

				memcpy(gw, ip, len);
				gw[len - 1] ^= 0xFF;
				inet_ntop(af, ip, ipp, MAX_ADDR_PRESENTATION_BUF_LEN);
				inet_ntop(af, gw, gwp, MAX_ADDR_PRESENTATION_BUF_LEN);

				snprintf(AtRspBuf, sizeof(AtRspBuf), "+GETIP: %s, %s", ipp, gwp);
				ATRESP(reqHandle, ATCI_RESULT_CODE_OK, 0, AtRspBuf);
			}
			else
			{
				ATRESP(reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
			}

		}
		break;
	}

	case CI_PS_PRIM_GET_3G_QOS_CAPS_CNF:
	{
		get3GQosCapsCnf = (CiPsPrimGet3GQosCapsCnf *)paras;

		if ( get3GQosCapsCnf->rc == CIRC_PS_SUCCESS )
		{
			if (get3GQosCapsCnf->qosCapsPresent)
			{
				CiPs3GQosCap *pCtxCaps = get3GQosCapsCnf->qosCaps.caps;

				for ( i = 0; i < get3GQosCapsCnf->qosCaps.size; i++ )
				{
					/* get PDP type string */
					getPdpTypeStr( pCtxCaps->type, TempBuf );

					switch (GET_REQ_ID(reqHandle))
					{
					case PRI_PS_PRIM_GET_MIN_QOS_CAPS_REQ:
					{
						sprintf( (char *)AtRspBuf, "CGEQMIN: " );
						ATRESP( reqHandle, 0, 0, (char *)AtRspBuf);
						break;
					}
					case CI_PS_PRIM_GET_3G_QOS_CAPS_REQ:
					{
						sprintf( (char *)AtRspBuf, "CGEQREQ: " );
						ATRESP( reqHandle, 0, 0, (char *)AtRspBuf);
						break;
					}
					case PRI_PS_PRIM_GET_NEG_QOS_CAPS_REQ:
					{
						sprintf( (char *)AtRspBuf, "CGEQNEG: " );
						ATRESP( reqHandle, 0, 0, (char *)AtRspBuf);
						break;
					}
					default:
						ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
						return;
						break;
					}
					sprintf( (char *)AtRspBuf, "%s,(%d-%d),",
						 TempBuf,
						 pCtxCaps->trafficClass.min,
						 pCtxCaps->trafficClass.max);
					sprintf( (char *)TempBuf, "(%d-%d),(%d-%d),",
						 Decode3GBitRate(pCtxCaps->maxULRate.min),
						 Decode3GBitRate(pCtxCaps->maxULRate.max),
						 Decode3GBitRate(pCtxCaps->maxDLRate.min),
						 Decode3GBitRate(pCtxCaps->maxDLRate.max));
					strcat( (char *)AtRspBuf, (char *)TempBuf );
					sprintf( (char *)TempBuf, "(%d-%d),(%d-%d),",
						 Decode3GBitRate(pCtxCaps->guaranteedULRate.min),
						 Decode3GBitRate(pCtxCaps->guaranteedULRate.max),
						 Decode3GBitRate(pCtxCaps->guaranteedDLRate.min),
						 Decode3GBitRate(pCtxCaps->guaranteedDLRate.max));
					strcat( (char *)AtRspBuf, (char *)TempBuf );
					sprintf( (char *)TempBuf, "(%d-%d),(%d-%d),(\"",
						 pCtxCaps->deliveryOrder.min,
						 pCtxCaps->deliveryOrder.max,
						 Decode3GMaxSduSize(pCtxCaps->maxSduSize.min),
						 Decode3GMaxSduSize(pCtxCaps->maxSduSize.max));
					strcat( (char *)AtRspBuf, (char *)TempBuf );
					printSduErrRatio( (char *)TempBuf, pCtxCaps->errRatio.min );
					strcat( (char *)AtRspBuf, (char *)TempBuf );
					strcat( (char *)AtRspBuf, "\"-\"" );
					printSduErrRatio( (char *)TempBuf, pCtxCaps->errRatio.max );
					strcat( (char *)AtRspBuf, (char *)TempBuf );
					strcat( (char *)AtRspBuf, "\"),(\"" );
					printResBER( (char *)TempBuf, pCtxCaps->resBER.min );
					strcat( (char *)AtRspBuf, (char *)TempBuf );
					strcat( (char *)AtRspBuf, "\"-\"" );
					printResBER( (char *)TempBuf, pCtxCaps->resBER.max );
					strcat( (char *)AtRspBuf, (char *)TempBuf );
					strcat( (char *)AtRspBuf, "\")," );
					sprintf( (char *)TempBuf, "(%d-%d),(%d-%d),",
						 pCtxCaps->deliverErrSdu.min,
						 pCtxCaps->deliverErrSdu.max,
						 pCtxCaps->transDelay.min,
						 pCtxCaps->transDelay.max);
					strcat( (char *)AtRspBuf, (char *)TempBuf );
					sprintf( (char *)TempBuf, "(%d-%d)",
						 pCtxCaps->thPriority.min,
						 pCtxCaps->thPriority.max );
					strcat( (char *)AtRspBuf, (char *)TempBuf );
					ATRESP( reqHandle, 0, 0, (char *)AtRspBuf);

					pCtxCaps++;
				}
			}
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
		else
			ATRESP(reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);


		break;
	}

	case CI_PS_PRIM_GET_3G_QOS_CNF:
	{
		getQos3GCnf = (CiPsPrimGet3GQosCnf *)paras;
		CiPs3GQosType qosType;
		int ulRate, dlRate, maxSdu;
		UINT8 currCid = gCIDList.currCntx[GET_ATP_INDEX(reqHandle)].currCid;

		if(currCid >= CI_PS_MAX_PDP_CTX_NUM)
		{
			resetCurrCntx(GET_ATP_INDEX(reqHandle));
			ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
			break;
		}


		switch (GET_REQ_ID(reqHandle))
		{
		case PRI_PS_PRIM_GET_MIN_QOS_REQ:
		{
			sprintf( (char *)AtRspBuf, "%s", "+CGEQMIN: " );
			qosType = CI_PS_3G_QOSTYPE_MIN;
			break;
		}
		case CI_PS_PRIM_GET_3G_QOS_REQ:
		{
			sprintf( (char *)AtRspBuf, "%s", "+CGEQREQ: " );
			qosType = CI_PS_3G_QOSTYPE_REQ;
			break;
		}
		case PRI_PS_PRIM_GET_NEG_QOS_REQ:
		{
			sprintf( (char *)AtRspBuf, "%s", "+CGEQNEG: " );
			qosType = CI_PS_3G_QOSTYPE_NEG;
			break;
		}
		default:
		{
			ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
			return;
		}
		}

		if (( getQos3GCnf->rc == CIRC_PS_SUCCESS ) && getQos3GCnf->qosProfPresent )
		{
			/* Print the part that is common to REQ,MIN, and NEG */
			ulRate = Decode3GBitRate(getQos3GCnf->qosProf.maxULRate);
			dlRate = Decode3GBitRate(getQos3GCnf->qosProf.maxDLRate);
			sprintf( (char *)TempBuf, "%d,%d,%d,%d,",
				 currCid + 1, /* in SAC, PDP contexts start at 0 */
				 getQos3GCnf->qosProf.trafficClass,
				 ulRate, dlRate );
			strcat( (char *)AtRspBuf, (char *)TempBuf );
			ulRate = Decode3GBitRate(getQos3GCnf->qosProf.guaranteedULRate);
			dlRate = Decode3GBitRate(getQos3GCnf->qosProf.guaranteedDLRate);
			maxSdu = Decode3GMaxSduSize(getQos3GCnf->qosProf.maxSduSize);
			sprintf( (char *)TempBuf, "%d,%d,%d,%d,\"",
				 ulRate, dlRate,
				 getQos3GCnf->qosProf.deliveryOrder, maxSdu);
			strcat( (char *)AtRspBuf, (char *)TempBuf );
			printSduErrRatio( (char *)TempBuf, getQos3GCnf->qosProf.sduErrRatio );
			strcat( (char *)AtRspBuf, (char *)TempBuf );
			strcat( (char *)AtRspBuf, "\",\"" );
			printResBER( (char *)TempBuf, getQos3GCnf->qosProf.resBER );
			strcat( (char *)AtRspBuf, (char *)TempBuf );
			sprintf( (char *)TempBuf, "\",%d,%d,%d",
				 getQos3GCnf->qosProf.deliveryOfErrSdu,
				 Decode3GTransDelay(getQos3GCnf->qosProf.transDelay),
				 getQos3GCnf->qosProf.thPriority );
			strcat( (char *)AtRspBuf, (char *)TempBuf );
			ATRESP( reqHandle, 0, 0, (char *)AtRspBuf );

		}

		/* send query for next CID */
		currCid++;
		if ( currCid < CI_PS_MAX_PDP_CTX_NUM )
		{
			getQos3GReq = utlCalloc(1, sizeof(*getQos3GReq));
			if (getQos3GReq == NULL) {
				ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
				break;
			}

			getQos3GReq->cid = currCid;
			getQos3GReq->qosType = qosType;
			gCIDList.currCntx[GET_ATP_INDEX(reqHandle)].currCid = currCid;
			ret = ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_GET_3G_QOS_REQ,
					 reqHandle, (void *)getQos3GReq );
		}
		else
		{
			//resetCurrCntx(GET_ATP_INDEX(reqHandle));
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
		break;
	}

	case CI_PS_PRIM_SET_3G_QOS_CNF:
	{
		setQos3GCnf = (CiPsPrimSet3GQosCnf *)paras;
		if ( setQos3GCnf->rc == CIRC_PS_SUCCESS )
		{
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
		else
		{
			ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		}
		break;
	}

	case CI_PS_PRIM_DELETE_PDP_CTX_CNF:
	{
		DBGMSG("%s: at line %d.\n", __FUNCTION__, __LINE__);
		/*
		 * DELETE PDP CTX may be triggered by AT CMD Server internally,
		 *  in this case, we don't need to respond "OK"
		 */
		if (reqHandle != IND_REQ_HANDLE)
		{
			UINT32 atpIdx = GET_ATP_INDEX(reqHandle);
			if (gCIDList.currCntx[atpIdx].currCid != CI_PS_MAX_PDP_CTX_NUM)
				gCIDList.cInfo[gCIDList.currCntx[atpIdx].currCid].bDefined = FALSE;
			resetCurrCntx(atpIdx);
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
	}
	break;

	case CI_PS_PRIM_DELETE_SEC_PDP_CTX_CNF:
	{
		DBGMSG("CI_PS_PRIM_DELETE_SEC_PDP_CTX_CNF: at line %d.\n", __LINE__);
		resetCurrCntx(GET_ATP_INDEX(reqHandle));
		ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
	}
	break;

	case CI_ERR_PRIM_HASNOSUPPORT_CNF:
	case CI_ERR_PRIM_HASINVALIDPARAS_CNF:
	case CI_ERR_PRIM_ISINVALIDREQUEST_CNF:
	{
		/* Reply with an "Operation not supported" error (CME ERROR 4) */
		ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		//resetCurrCntx(GET_ATP_INDEX(reqHandle));
		break;
	}
	//Added by Michal Bukai - CGCMOD Set command support
	case CI_PS_PRIM_MODIFY_PDP_CTX_CNF:
	{
		modifyPdpCtxCnf = (CiPsPrimModifyPdpCtxCnf *)paras;
		checkPSRet(reqHandle, modifyPdpCtxCnf->rc);
		break;
	}
	//Added by Michal Bukai - CGCMOD Test command
	case CI_PS_PRIM_GET_PDP_CTXS_ACT_STATE_CNF:
	{
		getCtxAtcListCnf = (CiPsPrimGetPdpCtxsActStateCnf *)paras;
		if ( getCtxAtcListCnf->rc == CIRC_PS_SUCCESS )
		{
			CiPsPdpCtxActStateListPtr pCtxActLst = getCtxAtcListCnf->lst;
			char outputInfo[100];
			char tmp[10];
			memset(&outputInfo, 0, 100);
			memset(&tmp, 0, 10);

			sprintf(outputInfo, "+CGCMOD: ");
			UINT8 index = 0;

			for (index = 0; index < getCtxAtcListCnf->num && pCtxActLst; index++)
			{
				if (pCtxActLst->activated)
				{
					if (index != 0)
						strcat(outputInfo, ", ");

					sprintf(tmp, "%d", pCtxActLst->cid);
					strcat(outputInfo, tmp);
				}
				pCtxActLst++;
			}

			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, outputInfo);
		}
		else
		{
			checkPSRet(reqHandle, getCtxAtcListCnf->rc);
		}
		break;
	}
	/*Added by Michal Bukai - CGTFT support - START*/
	case CI_PS_PRIM_DEFINE_TFT_FILTER_CNF:
	{
		defineTftFilterCnf = (CiPsPrimDefineTftFilterCnf *)paras;
		checkPSRet(reqHandle, defineTftFilterCnf->rc);
		break;
	}
	case CI_PS_PRIM_DELETE_TFT_CNF:
	{
		deleteTftCnf = (CiPsPrimDeleteTftCnf *)paras;
		checkPSRet(reqHandle, deleteTftCnf->rc);
		break;
	}
	case CI_PS_PRIM_GET_TFT_CNF:
	{
		getTftCnf = (CiPsPrimGetTftCnf *)paras;
		// Here we should print the TFT to terminal. We will implement it later
		//        checkPSRet(reqHandle,getTftCnf->rc);
		break;
	}
	/*Added by Michal Bukai - CGTFT support - END*/
	case CI_PS_PRIM_FAST_DORMANT_CNF:
	{
		getFastDormancyCnf = (CiPsPrimFastDormantCnf *)paras;
		if ( getFastDormancyCnf->rc == CIRC_PS_SUCCESS )
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, 0);
		else
			ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		break;
	}
	case CI_PS_PRIM_AUTHENTICATE_CNF:
	{
		getAuthenticateCnf = (CiPsPrimAuthenticateCnf *)paras;
		if(getAuthenticateCnf->rc == CIRC_PS_SUCCESS)
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, 0);
		else
			ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		break;
	}
#ifdef AT_CUSTOMER_HTC
	case CI_PS_PRIM_SET_FAST_DORMANCY_CONFIG_CNF:
	{
		CiPsPrimSetFastDormancyConfigCnf *setFastDormancyConfigCnf = (CiPsPrimSetFastDormancyConfigCnf *)paras;
		if(setFastDormancyConfigCnf->rc == CIRC_PS_SUCCESS)
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, 0);
		else
			ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		break;
	}
#endif
	default:
		ERRMSG("Unknown primId:%d in psCnf \n", primId);

		break;
	}

	/* free up the confirmation memory */
	atciSvgFreeCnfMem( svgId, primId, paras );

	return;
}


/************************************************************************************
 *
 * PS CI notifications
 *
 *************************************************************************************/

void psInd(CiSgOpaqueHandle opSgOpaqueHandle,
	   CiServiceGroupID svgId,
	   CiPrimitiveID primId,
	   CiIndicationHandle indHandle,
	   void*               pParam)
{
	UNUSEDPARAM(opSgOpaqueHandle)
	UNUSEDPARAM(indHandle)
	F_ENTER();

	DBGMSG("psInd:primId=%d\n", primId);

	CiPsPrimPdpCtxDeactedInd  *pdpCtxDeactedInd;
	CiPsPrimNwRegInd    *nwRegInd;

	switch (primId)
	{
	case CI_PS_PRIM_PDP_CTX_DEACTED_IND:
	{
		char atBuf[200];
		char tmpBuf[50];
		UINT32 reqHandle = IND_REQ_HANDLE;
		int i;
		pdpCtxDeactedInd = (CiPsPrimPdpCtxDeactedInd *)pParam;

		/* PDP type number */
		getPdpTypeStr( pdpCtxDeactedInd->indedPdpCtx.type, tmpBuf );

		sprintf( atBuf, "+CGEV: NW DEACT \"%s\", ", tmpBuf );

		strcat( atBuf, "\"" );

		/* PDP address */
		if ( pdpCtxDeactedInd->indedPdpCtx.addrPresent )
		{
			for ( i = 0; i < pdpCtxDeactedInd->indedPdpCtx.addr.len; i++ )
			{
				if (i != 0 )
				{
					strcat( atBuf, "." );
				}
				sprintf( tmpBuf, "%d", pdpCtxDeactedInd->indedPdpCtx.addr.valData[i] );
				strcat( atBuf, tmpBuf );
			}
		}

		strcat( atBuf, "\",");

		sprintf( tmpBuf, "%d\r\n", pdpCtxDeactedInd->indedPdpCtx.cid + 1 );

		strcat( atBuf, tmpBuf );

		if (gCIDList.cInfo[pdpCtxDeactedInd->indedPdpCtx.cid].reqHandle != INVALID_REQ_HANDLE)
		{
			reqHandle = gCIDList.cInfo[pdpCtxDeactedInd->indedPdpCtx.cid].reqHandle;
		}

		//If the CID is already deleted, don't send +CGEV

		if (gCIDList.cInfo[pdpCtxDeactedInd->indedPdpCtx.cid].bDefined)
			ATRESP(reqHandle, 0, 0, atBuf);

	    #if 0
		deletePdpCtxReq.cid = gCurrentPdpSetCtx.cid;
		DBGMSG("[%s]:line(%d), gCurrentPdpSetCtx.cid(%d).\n", __FUNCTION__, __LINE__, gCurrentPdpSetCtx.cid );

		sleep( WAIT_TIME_BEFORE_PDP_CTX_DELETE );

		ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_DELETE_PDP_CTX_REQ,
			   gAtciSvgReqHandle[CI_SG_ID_PS]++, (void *)&deletePdpCtxReq );

		// vcy moved here.
		atciSvgFreeIndMem( svgId, primId, pParam );
	    #endif
		break;
	}
	case CI_PS_PRIM_NW_REG_IND:
	{
		char atBuf[100];
		nwRegInd = (CiPsPrimNwRegInd *)pParam;
		gCurrentPSRegStatus = nwRegInd->nwRegInfo.status;
		DBGMSG("[%s]:line(%d), CI_PS_PRIM_NW_REG_IND received, regstatus: %d!\n", __FUNCTION__, __LINE__, nwRegInd->nwRegInfo.status);

		/* show the registration status */
		switch ( gCurrentPSRegOption)
		{
		case CI_PS_NW_REG_IND_DISABLE:
			/* Nothing to show */
			break;

		case CI_PS_NW_REG_IND_ENABLE_STA_ONLY:
			sprintf(atBuf, "+CGREG: %d\r\n", nwRegInd->nwRegInfo.status);
			ATRESP( IND_REQ_HANDLE, 0, 0, atBuf);
			break;

		case CI_PS_NW_REG_IND_ENABLE_DETAIL:
			if (nwRegInd->nwRegInfo.present == TRUE)
			{
				sprintf( atBuf, "+CGREG: %d, \"%04x\", \"%08x\", %d\r\n", nwRegInd->nwRegInfo.status,
					 nwRegInd->nwRegInfo.lac,
					 nwRegInd->nwRegInfo.cellId,
					 gActDetail);
				ATRESP( IND_REQ_HANDLE, 0, 0, atBuf);
			}
			else
			{
				sprintf(atBuf, "+CGREG: %d\r\n", nwRegInd->nwRegInfo.status);
				ATRESP( IND_REQ_HANDLE, 0, 0, atBuf);
			}
			
			break;
		default:
			break;
		}
		break;
	}
	case CI_PS_PRIM_DETACHED_IND:
	{
		/* Report CGEV */
		ATRESP( IND_REQ_HANDLE, 0, 0, (char*)"+CGEV: NW DETACH\r\n");
		DBGMSG("[%s]:line(%d), CI_PS_PRIM_DETACHED_IND received \n", __FUNCTION__, __LINE__);
		break;
	}
#ifdef AT_CUSTOMER_HTC
	case CI_PS_PRIM_PDP_ACTIVATION_REJECT_CAUSE_IND:
	{
		char atBuf[100];
		CiPsPrimPdpActivationRejectCauseInd *psPrimPdpActivationRejectCauseInd;
		psPrimPdpActivationRejectCauseInd = (CiPsPrimPdpActivationRejectCauseInd *)pParam;
		sprintf(atBuf, "%s: %X,%X\r\n", SPEC_ERROR_CODE_INDICATION_PREFIX, ATCI_HTC_ERROR_CLASS_PDP_ACTIVATION_REJECT, psPrimPdpActivationRejectCauseInd->smCause);
		ATRESP( IND_REQ_HANDLE, 0, 0, atBuf);
		break;
	}
#endif
	default:
		break;
	}

	/* free up the indication memory */
	atciSvgFreeIndMem( svgId, primId, pParam );

	F_LEAVE();

	return;
}


void initPSContext( void )
{
	int i;

	for ( i = 0; i < NUM_OF_TEL_ATP; i++ )
	{
		gCIDList.currCntx[i].currCid = CI_PS_MAX_PDP_CTX_NUM;
		gCIDList.currCntx[i].reqMsg = NULL;
	}
	for ( i = 0; i < CI_PS_MAX_PDP_CTX_NUM; i++ )
	{
		gCIDList.cInfo[i].bDefined = FALSE;
		gCIDList.cInfo[i].connectionType = ATCI_LOCAL;
		gCIDList.cInfo[i].pdpAddress[0] = 0;
		gCIDList.cInfo[i].pdpType = CI_PS_PDP_NUM_TYPES;
		gCIDList.cInfo[i].reqHandle = INVALID_REQ_HANDLE;
	}
}

/* deactive_ps_connection:
    deactive ps connection
    return value:
    0 for successful.  -1 for failure.
*/
int deactive_ps_connection(UINT8 atpIdx)
{
	int ret = 0;
	unsigned char cid;
	_AtciConnectionType conntype;
	UINT32 atHandle;

	if (atpIdx == TEL_MODEM_AT_CMD_ATP)
		conntype = ATCI_REMOTE;
	else
		conntype = ATCI_LOCAL;

	for (cid = 0; cid < CI_PS_MAX_PDP_CTX_NUM; cid++)
	{
		if ((gCIDList.cInfo[cid].reqHandle != INVALID_REQ_HANDLE)
			&&(gCIDList.cInfo[cid].connectionType == conntype))
		{
			atHandle = GET_AT_HANDLE(gCIDList.cInfo[cid].reqHandle);
			if (PS_SetGPRSContextActivated (atHandle, cid, FALSE, FALSE) == CIRC_SUCCESS)
			{
				DBGMSG("[%s]: deactivated cid %d successful!\n", __FUNCTION__, cid);
			}
			else
			{
				DBGMSG("[%s]: deactivated cid %d failed!\n", __FUNCTION__, cid);
				ret = -1;
				break;
			}
		}
	}

	return ret;
}

