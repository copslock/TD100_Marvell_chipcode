/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: ss_api.c
 *
 *  Description: API interface implementation for supplementary service
 *
 *  History:
 *   Oct 15, 2008 - Rovin Yu Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/
#include "telutl.h"
#include "telatparamdef.h"
#include "ci_ss.h"
#include "ci_sim.h"
#include "ci_api.h"
#include "ci_dev.h"
#include "teldef.h"
#include "telatci.h"
#include "utlMalloc.h"
#include "ss_api.h"

extern CiServiceHandle gAtciSvgHandle[CI_SG_NUMIDS + 1];

AtciSSCntx gSSCntx = { 0, ATCI_SS_PRESENTATION_ENABLE, 0, FALSE};


//AT+CDIP?
CiReturnCode SS_GetCdipStatus(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimGetCdipStatusReq   *getCdipStatusReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_GET_CDIP_STATUS_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_GET_CDIP_STATUS_REQ), (void *)getCdipStatusReq );

	return ret;
}

//AT+CDIP=
CiReturnCode SS_SetCdipOption(UINT32 atHandle, UINT32 dwStatus)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimSetCdipOptionReq   *setCdipOptionReq;

	setCdipOptionReq = utlCalloc(1, sizeof(*setCdipOptionReq));
	if (setCdipOptionReq == NULL)
		return CIRC_FAIL;

	setCdipOptionReq->Local = (UINT8)dwStatus;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_SET_CDIP_OPTION_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_SET_CDIP_OPTION_REQ), (void *)setCdipOptionReq );

	return ret;
}


//AT+CLIP?
CiReturnCode SS_GetCallerIdSettings(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimGetClipStatusReq   *getClipStatusReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_GET_CLIP_STATUS_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_GET_CLIP_STATUS_REQ), (void *)getClipStatusReq );

	return ret;
}

//AT+CLIP=
CiReturnCode SS_SetCallerIdStatus(UINT32 atHandle, UINT32 dwStatus)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimSetClipOptionReq   *setClipOptionReq;

	setClipOptionReq = utlCalloc(1, sizeof(*setClipOptionReq));
	if (setClipOptionReq == NULL)
		return CIRC_FAIL;

	setClipOptionReq->Local = (UINT8)dwStatus;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_SET_CLIP_OPTION_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_SET_CLIP_OPTION_REQ), (void *)setClipOptionReq );

	return ret;
}

//AT+CLIR?
CiReturnCode SS_GetHideIdSettings(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimGetClirStatusReq      *getCLIRStatusReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_GET_CLIR_STATUS_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_GET_CLIR_STATUS_REQ), (void *)getCLIRStatusReq );

	return ret;
}

//AT+CLIR=
CiReturnCode SS_SetHideIdStatus(UINT32 atHandle, UINT32 dwStatus)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimSetClirOptionReq      *setCLIROptionReq;

	setCLIROptionReq = utlCalloc(1, sizeof(*setCLIROptionReq));
	if (setCLIROptionReq == NULL)
		return CIRC_FAIL;

	setCLIROptionReq->Local = (UINT8)dwStatus;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_SET_CLIR_OPTION_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_SET_CLIR_OPTION_REQ), (void *)setCLIROptionReq );

	return ret;
}

//AT+COLR?
CiReturnCode SS_GetHideConnectedIdSettings(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimGetColrStatusReq      *getCOLRStatusReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_GET_COLR_STATUS_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_GET_COLR_STATUS_REQ), (void *)getCOLRStatusReq );

	return ret;
}

//AT+COLP?
CiReturnCode SS_GetDialedIdSettings(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimGetColpStatusReq       *colpStatus = NULL;


	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_GET_COLP_STATUS_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_GET_COLP_STATUS_REQ), (void *)colpStatus );

	return ret;
}

//AT+COLP=
CiReturnCode SS_SetDialedIdStatus(UINT32 atHandle, UINT32 dwStatus)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimSetColpOptionReq       *colpOpReq;

	colpOpReq = utlCalloc(1, sizeof(*colpOpReq));
	if (colpOpReq == NULL)
		return CIRC_FAIL;

	colpOpReq->Local = (UINT32)dwStatus;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_SET_COLP_OPTION_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_SET_COLP_OPTION_REQ), (void *)colpOpReq );

	return ret;
}

//AT+CCFC= ,2
CiReturnCode SS_GetCallForwardingSettings(UINT32 atHandle, UINT32 dwReason, UINT32 dwInfoClasses)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimInterrogateCfInfoReq     *interrogateCfReq;

	interrogateCfReq = utlCalloc(1, sizeof(*interrogateCfReq));
	if (interrogateCfReq == NULL)
		return CIRC_FAIL;

	interrogateCfReq->Classes = (UINT8)dwInfoClasses;
	interrogateCfReq->Reason = (UINT8)dwReason;

	// NOTE: use dwInfoClasses as REQ ID to avoid global variable
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_INTERROGATE_CF_INFO_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, (UINT8)dwInfoClasses), (void *)interrogateCfReq );

	return ret;
}

//AT+CCFC= ,3
CiReturnCode SS_AddCallForwarding(UINT32 atHandle, const CiSsCfRegisterInfo* lpSettings)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimRegisterCfInfoReq        *registerCfReq;

	registerCfReq = utlCalloc(1, sizeof(*registerCfReq));
	if (registerCfReq == NULL)
		return CIRC_FAIL;

	memcpy(&registerCfReq->info, lpSettings, sizeof(CiSsCfRegisterInfo));

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_REGISTER_CF_INFO_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_REGISTER_CF_INFO_REQ), (void *)registerCfReq );

	return ret;

}

//AT+CCFC= ,4
CiReturnCode SS_RemoveCallForwarding(UINT32 atHandle, UINT32 dwReason, UINT32 dwInfoClasses)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimEraseCfInfoReq           *eraseCfInfoReq;

	eraseCfInfoReq = utlCalloc(1, sizeof(*eraseCfInfoReq));
	if (eraseCfInfoReq == NULL)
		return CIRC_FAIL;

	eraseCfInfoReq->info.Classes = (UINT8)dwInfoClasses;
	eraseCfInfoReq->info.Reason = (UINT8)dwReason;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_ERASE_CF_INFO_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_ERASE_CF_INFO_REQ), (void *)eraseCfInfoReq);

	return ret;
}

//AT+CCFC= ,0 for disable; AT+CCFC= ,1  for enable
CiReturnCode SS_SetCallForwardingStatus(UINT32 atHandle, UINT32 dwReason, UINT32 dwInfoClasses, BOOL bStatus)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimSetCfActivationReq       *setCfReq;

	setCfReq = utlCalloc(1, sizeof(*setCfReq));
	if (setCfReq == NULL)
		return CIRC_FAIL;

	setCfReq->Activate = bStatus;
	setCfReq->Classes = (UINT8)dwInfoClasses;
	setCfReq->Reason = (UINT8)dwReason;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_SET_CF_ACTIVATION_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_SET_CF_ACTIVATION_REQ), (void *)setCfReq );

	return ret;
}

//AT+CCWA?
CiReturnCode SS_GetCallWaitingSettings(UINT32 atHandle, UINT32 dwInfoClasses)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimGetActiveCwClassesReq    *getActiveCwClassesReq;

	getActiveCwClassesReq = utlCalloc(1, sizeof(*getActiveCwClassesReq));
	if (getActiveCwClassesReq == NULL)
		return CIRC_FAIL;

	getActiveCwClassesReq->Classes = (CiSsBasicServiceMap)dwInfoClasses;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_GET_ACTIVE_CW_CLASSES_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_GET_ACTIVE_CW_CLASSES_REQ), (void *)getActiveCwClassesReq );

	return ret;
}

//AT+CCWA=
CiReturnCode SS_SetCallWaitingStatus(UINT32 atHandle, UINT32 dwInfoClasses, BOOL bStatus)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimSetCwActivationReq       *cwActivationReq;

	cwActivationReq = utlCalloc(1, sizeof(*cwActivationReq));
	if (cwActivationReq == NULL)
		return CIRC_FAIL;

	cwActivationReq->Activate = bStatus;
	cwActivationReq->Classes = (CiSsBasicServiceMap)dwInfoClasses;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_SET_CW_ACTIVATION_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_SET_CW_ACTIVATION_REQ), (void *)cwActivationReq );

	return ret;
}

//AT+CUSD=0/1, str
CiReturnCode SS_SendSupServiceData(UINT32 atHandle, UINT32 charSet, const CHAR* lpbData, UINT32 dwSize)
{
	CiReturnCode ret = CIRC_FAIL;
	AtciUssdState oldState = gSSCntx.currentUssdState;
	CiSsPrimStartUssdSessionReq             *startUssdSessionReq;
	CiSsPrimReceivedUssdInfoRsp             ussdInfoRsp;

	if ( gSSCntx.currentUssdState == ATCI_USSD_IDLE )
	{
		/*
		**  Attempt to send command as phase 2....
		*/
		startUssdSessionReq = utlCalloc(1, sizeof(*startUssdSessionReq));
		if (startUssdSessionReq == NULL)
			return CIRC_FAIL;

		startUssdSessionReq->info.CharSet = (UINT8)charSet;
		startUssdSessionReq->info.DataLength = (UINT8)dwSize;
		memcpy(startUssdSessionReq->info.Data, lpbData, startUssdSessionReq->info.DataLength);

		gSSCntx.currentUssdState = ATCI_USSD_MO_WAIT_NW_RSP;

		ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_START_USSD_SESSION_REQ,
				 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_START_USSD_SESSION_REQ), (void *)startUssdSessionReq );
	}
	else if ( gSSCntx.currentUssdState == ATCI_USSD_MT_WAIT_TE_RSP )
	{
		/*
		**  It's a response to the network, so send ReSPonse....
		*/
		ussdInfoRsp.info.CharSet = (UINT8)charSet;
		ussdInfoRsp.info.DataLength = (UINT8)dwSize;
		memcpy(ussdInfoRsp.info.Data, lpbData, ussdInfoRsp.info.DataLength);

		gSSCntx.currentUssdState = ATCI_USSD_MT_TE_RSP_SENT;

		ret = ciRespond( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_RECEIVED_USSD_INFO_RSP,
				 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_RECEIVED_USSD_INFO_RSP), (void *)&ussdInfoRsp );

		if(ret == CIRC_SUCCESS)
			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
	}
	else
		return ret;
	
	/* If sending fails, recover the state */
	if (ret != CIRC_SUCCESS)
		gSSCntx.currentUssdState = oldState;

	return ret;

}

//AT+CUSD=2
CiReturnCode SS_CancelSupServiceDataSession(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimAbortUssdSessionReq     *abortUssdSessionReq = NULL;
	AtciUssdState oldState = gSSCntx.currentUssdState;

	gSSCntx.currentUssdState = ATCI_USSD_ABORT_SENT;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_ABORT_USSD_SESSION_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_ABORT_USSD_SESSION_REQ), (void *)abortUssdSessionReq );
	if (ret != CIRC_SUCCESS)
		gSSCntx.currentUssdState = oldState;

	return ret;
}

//AT+CNAP?
CiReturnCode SS_GetNamePresentationSettings(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimGetCnapStatusReq        *cnapStatus = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_GET_CNAP_STATUS_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_GET_CNAP_STATUS_REQ), (void *)cnapStatus );

	return ret;
}

//AT+CNAP=
CiReturnCode SS_SetNamePresentationStatus(UINT32 atHandle, UINT32 dwStatus)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimSetCnapOptionReq                *cnapOptionReq;

	cnapOptionReq = utlCalloc(1, sizeof(*cnapOptionReq));
	if (cnapOptionReq == NULL)
		return CIRC_FAIL;

	cnapOptionReq->Local = (UINT8)dwStatus;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_SET_CNAP_OPTION_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_SET_CNAP_OPTION_REQ), (void *)cnapOptionReq );

	return ret;
}

//AT+CSSN=
CiReturnCode SS_SetNotificationOptions(UINT32 atHandle, BOOL ssiEnable, BOOL ssuEnable)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimSetSsNotifyOptionsReq   *setSsNotifyOptReq;

	setSsNotifyOptReq = utlCalloc(1, sizeof(*setSsNotifyOptReq));
	if (setSsNotifyOptReq == NULL)
		return CIRC_FAIL;

	setSsNotifyOptReq->SsiEnable = ssiEnable;
	setSsNotifyOptReq->SsuEnable = ssuEnable;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_SET_SS_NOTIFY_OPTIONS_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_SET_SS_NOTIFY_OPTIONS_REQ), (void *)setSsNotifyOptReq );

	return ret;
}

//AT+CSSN?
CiReturnCode SS_GetNotificationOptions(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimGetSsNotifyOptionsReq   *getSsNotifyOptReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_GET_SS_NOTIFY_OPTIONS_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_GET_SS_NOTIFY_OPTIONS_REQ), (void *)getSsNotifyOptReq );

	return ret;
}

//AT+CLCK=fac, 0/1
CiReturnCode SS_SetCallBarringStatus(UINT32 atHandle, UINT32 dwType, UINT32 dwInfoClass, CHAR* lpszPassword, BOOL bEnable)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimSetCbActivationReq      *setCbActReq;

	setCbActReq = utlCalloc(1, sizeof(*setCbActReq));
	if (setCbActReq == NULL)
		return CIRC_FAIL;

	setCbActReq->Service = (CiSsCbServiceCode)dwType;
	setCbActReq->Activate = bEnable;
	setCbActReq->Classes = (CiSsBasicServiceMap)dwInfoClass;
	setCbActReq->password.len = (strlen((char*)lpszPassword) < CI_MAX_PASSWORD_LENGTH ? strlen((char*)lpszPassword) : CI_MAX_PASSWORD_LENGTH);
	memcpy(setCbActReq->password.data, lpszPassword, setCbActReq->password.len);

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_SET_CB_ACTIVATE_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_SET_CB_ACTIVATE_REQ), (void *)setCbActReq );

	return ret;
}

//AT+CLCK=fac, 2
CiReturnCode SS_GetCallBarringStatus(UINT32 atHandle, UINT32 dwType, UINT32 dwInfoClass, CHAR* lpszPassword)
{
	UNUSEDPARAM(lpszPassword)

	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimGetCbMapStatusReq          *getCbMapStatusReq;

	getCbMapStatusReq = utlCalloc(1, sizeof(*getCbMapStatusReq));
	if (getCbMapStatusReq == NULL)
		return CIRC_FAIL;

	getCbMapStatusReq->Service = (CiSsCbServiceCode)dwType;
	getCbMapStatusReq->Classes = (CiSsBasicServiceMap)dwInfoClass;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_GET_CB_MAP_STATUS_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_GET_CB_MAP_STATUS_REQ), (void *)getCbMapStatusReq );

	return ret;
}

//AT+CPWD=
CiReturnCode SS_ChangeCallBarringPassword(UINT32 atHandle, CHAR* lpszOldPassword, CHAR* lpszNewPassword, CHAR* lpszNewPasswordVerify)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimChangeCbPasswordReq     *changeCbPswdReq;

	changeCbPswdReq = utlCalloc(1, sizeof(*changeCbPswdReq));
	if (changeCbPswdReq == NULL)
		return CIRC_FAIL;

	changeCbPswdReq->oldPassword.len = (strlen((char*)lpszOldPassword) < CI_MAX_PASSWORD_LENGTH? strlen((char*)lpszOldPassword) : CI_MAX_PASSWORD_LENGTH);
	memcpy(changeCbPswdReq->oldPassword.data, lpszOldPassword, changeCbPswdReq->oldPassword.len);
	changeCbPswdReq->newPassword.len = (strlen((char*)lpszNewPassword) < CI_MAX_PASSWORD_LENGTH ? strlen((char*)lpszNewPassword) : CI_MAX_PASSWORD_LENGTH);
	memcpy(changeCbPswdReq->newPassword.data, lpszNewPassword, changeCbPswdReq->newPassword.len);

	if(lpszNewPasswordVerify)
	{
		changeCbPswdReq->newPasswdVerification.len = (strlen((char*)lpszNewPasswordVerify) < CI_MAX_PASSWORD_LENGTH ? strlen((char*)lpszNewPasswordVerify) : CI_MAX_PASSWORD_LENGTH);
		memcpy(changeCbPswdReq->newPasswdVerification.data, lpszNewPasswordVerify, changeCbPswdReq->newPasswdVerification.len);
	}
	else
		memcpy(&changeCbPswdReq->newPasswdVerification, &changeCbPswdReq->newPassword, sizeof(CiPassword));

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_CHANGE_CB_PASSWORD_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_CHANGE_CB_PASSWORD_REQ), (void *)changeCbPswdReq );

	return ret;
}

//ATD##3424*
CiReturnCode SS_SendMarvellPrivateCode(UINT32 atHandle, const CHAR* subCode, UINT32 dwSize)
{
	UNUSEDPARAM(subCode);
	UNUSEDPARAM(dwSize);
	CiReturnCode ret = CIRC_FAIL;

	ret = ciRequest(gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_COMM_ASSERT_REQ,
		MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_COMM_ASSERT_REQ), NULL);
	return ret;
}

/************************************************************************************
 *
 * SS CI confirmations
 *
 *************************************************************************************/

static void ssPrintResult ( CiRequestHandle reqHandle, CiSsResultCode result )
{
	if ( result == CIRC_SS_SUCCESS )
	{
		ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
	}
	else
	{
		switch (result)
		{
		case CIRC_SS_NOT_PROVISIONED:      /* Supplementary Service not provisioned  */
			ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_SERVICE_OP_NOT_SUBSCRIBED, NULL);
			break;
		case CIRC_SS_NOT_REGISTERED:       /* Supplementary Service not registered */
			ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
			break;
		case CIRC_SS_REJECTED:
			ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_SERVICE_OP_NOT_SUPPORTED, NULL);
			break;
		case CIRC_SS_INCORRECT_PWD:
			ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INCORRECT_PASSWD, NULL);
			break;
		default:
			ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		}
	}
}


void ssCnf(CiSgOpaqueHandle opSgCnfHandle,
	   CiServiceGroupID svgId,
	   CiPrimitiveID primId,
	   CiRequestHandle reqHandle,
	   void*             paras)
{
	UNUSEDPARAM(opSgCnfHandle)
	INT8 i;
	INT8 j;
	CiSsCfRegisterInfo              *infoPtr;
	CiSsPrimRegisterCfInfoCnf       *regCfCnf;
	CiSsPrimEraseCfInfoCnf          *eraseCfInfoCnf;
	CiSsPrimSetCfActivationCnf      *activateCnf;
	CiSsPrimInterrogateCfInfoCnf    *interCfInfoCnf;
	CiSsPrimGetCwOptionCnf          *getCwOptionCnf;
	CiSsPrimSetCwOptionCnf          *setCwOptCnf;
	CiSsPrimGetCwActiveStatusCnf    *getCwActiveStatusCnf;
	CiSsPrimSetCwActivationCnf      *setCwActivCnf;
	CiSsPrimSetSsNotifyOptionsCnf   *setSsNotOptCnf;
	CiSsPrimGetSsNotifyOptionsCnf   *getSsNotOptCnf;
	CiSsPrimGetUssdEnableCnf        *getUssdEnableCnf;
	CiSsPrimSetUssdEnableCnf        *setUssdEnableCnf;
	CiSsPrimAbortUssdSessionCnf     *abortUssdSessionCnf;
	CiSsPrimStartUssdSessionCnf     *startUssdCnf;
	CiSsPrimSetColpOptionCnf        *setColpOptCnf;
	CiSsPrimGetColpStatusCnf        *getColpStatusCnf;
	CiSsPrimGetColrStatusCnf        *getColrStatusCnf;
	CiSsPrimSetClipOptionCnf        *setClipOptCnf;
	CiSsPrimGetClipStatusCnf        *getClipStatusCnf;
	/*Michal Bukai - CDIP support - Start*/
	CiSsPrimSetCdipOptionCnf        *setCdipOptCnf;
	CiSsPrimGetCdipStatusCnf        *getCdipStatusCnf;
	/*Michal Bukai - CDIP support - End*/
	CiSsPrimSetCbActivateCnf        *setCbActCnf;
	CiSsPrimGetCbMapStatusCnf       *getCbMapStatusCnf;
	CiSsPrimChangeCbPasswordCnf     *chgCbPswdCnf;
	CiCcPrimManipulateCallsCnf      *manipulateCallsCnf;
	CiSsPrimGetClirStatusCnf        *getClirStatusCnf;
	CiSsPrimSetClirOptionCnf        *setClirOptionCnf;
	CiSsPrimGetActiveCwClassesCnf   *getActiveCwClassesCnf;
	CiSsPrimGetCnapStatusCnf        *getCnapStatusCnf;
	CiSsPrimSetCnapOptionCnf        *setCnapOptionCnf;
	CiSsPrimGetAocWarningEnableCnf	*getCcweModeCnf;
	CiSsPrimSetAocWarningEnableCnf	*setCcweModeCnf;
	CiSsPrimGetCcmOptionCnf	        *getCcmOptionCnf;
	CiSsPrimSetCcmOptionCnf	        *setCcmOptionCnf;

	CHAR cwGetString[300] = { 0 };
	char ssRspBuf[500];
	char ssRspTmpBuf[400];

	/*
	**  Determine the primitive being confirmed.
	*/
	DBGMSG("%s: reqhandle = %d, primId = %d.\n", __FUNCTION__, reqHandle, primId);

	switch (primId)
	{
	case CI_SS_PRIM_REGISTER_CF_INFO_CNF:
	{
		regCfCnf = (CiSsPrimRegisterCfInfoCnf *)paras;
		ssPrintResult( reqHandle, regCfCnf->Result );
		break;
	}

	case CI_SS_PRIM_ERASE_CF_INFO_CNF:
	{
		eraseCfInfoCnf = (CiSsPrimEraseCfInfoCnf *)paras;
		ssPrintResult( reqHandle, eraseCfInfoCnf->Result );
		break;
	}

	case CI_SS_PRIM_SET_CF_ACTIVATION_CNF:
	{
		activateCnf = (CiSsPrimSetCfActivationCnf *)paras;
		ssPrintResult( reqHandle, activateCnf->Result );
		break;
	}

	case CI_SS_PRIM_INTERROGATE_CF_INFO_CNF:
	{
		interCfInfoCnf = (CiSsPrimInterrogateCfInfoCnf *)paras;

		if ( interCfInfoCnf->Result == CIRC_SS_SUCCESS )
		{
			infoPtr = interCfInfoCnf->info;

			if ( interCfInfoCnf->NumCfInfo == 0 )
			{
				// NOTE: we have saved classes as REQ ID in REQ HANDLE
				UINT8 classes = GET_REQ_ID(reqHandle);
				sprintf( ssRspBuf, "+CCFC: 0,%d", (int)classes );
				ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ssRspBuf );
			}
			else
			{
				char tmpBuf[100];

				for (i = 0; i < interCfInfoCnf->NumCfInfo; i++, infoPtr++)
				{
					const char *plus = NULL;
					if ( infoPtr->Number.AddrType.NumType == CI_NUMTYPE_INTERNATIONAL )
						plus = "+";
					else
						plus = "";

					/* Print address info */
					for ( j = 0; j < infoPtr->Number.Length; j++ )
					{
						ssRspTmpBuf[j] = '0' + infoPtr->Number.Digits[j];
					}

					ssRspTmpBuf[j] = '\0';
					sprintf( ssRspBuf, "+CCFC: %d, %d, \"%s%s\", %d, ", interCfInfoCnf->Status, infoPtr->Classes,
						 plus, ssRspTmpBuf, DialNumTypeCI2GSM(infoPtr->Number.AddrType.NumType) );

					if ( infoPtr->OptSubaddr.Type == CI_NUMTYPE_INTERNATIONAL )
						plus = "+";
					else
						plus = "";

					/* Print subaddress info */
					for ( j = 0; j < infoPtr->OptSubaddr.Length; j++ )
					{
						ssRspTmpBuf[j] = '0' + infoPtr->OptSubaddr.Digits[j];
					}

					ssRspTmpBuf[j] = '\0';
					sprintf( tmpBuf, "\"%s%s\", %d, %d\r\n", plus, ssRspTmpBuf, DialNumTypeCI2GSM(infoPtr->OptSubaddr.Type),
						 infoPtr->CFNRyDuration);
					strcat( ssRspBuf, tmpBuf );
					ATRESP( reqHandle, 0, 0, ssRspBuf );
				}
				ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
			}
		}
		else
		{
			ssPrintResult( reqHandle, interCfInfoCnf->Result );
		}

		break;
	}

	case CI_SS_PRIM_SET_CW_ACTIVATION_CNF:
	{
		setCwActivCnf = (CiSsPrimSetCwActivationCnf *)paras;
		ssPrintResult( reqHandle, setCwActivCnf->Result );
		break;
	}

	case CI_SS_PRIM_GET_CW_OPTION_CNF:
	{
		getCwOptionCnf = (CiSsPrimGetCwOptionCnf *)paras;
		if ( getCwOptionCnf->Result == CIRC_SS_SUCCESS )
		{
			sprintf(ssRspBuf, "+CCWA: %d", getCwOptionCnf->Local );
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ssRspBuf);
		}
		else
		{
			ssPrintResult( reqHandle, getCwOptionCnf->Result );
		}
		break;
	}

	case CI_SS_PRIM_SET_CW_OPTION_CNF:
	{
		setCwOptCnf = (CiSsPrimSetCwOptionCnf *)paras;
		ssPrintResult( reqHandle, setCwOptCnf->Result );

		break;
	}

	case CI_SS_PRIM_GET_CW_ACTIVE_STATUS_CNF:
	{
		/* NOTE:  this primitive is currently not supported by SAC */
		getCwActiveStatusCnf = (CiSsPrimGetCwActiveStatusCnf *)paras;
		if ( getCwActiveStatusCnf->Result == CIRC_SS_SUCCESS )
		{
			sprintf(ssRspBuf, "+CCWA: %d, %d\r\n", getCwActiveStatusCnf->Status, getCwActiveStatusCnf->Class);
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ssRspBuf);
		}
		else
		{
			ssPrintResult( reqHandle, getCwActiveStatusCnf->Result );
		}
		break;
	}
	case CI_SS_PRIM_GET_ACTIVE_CW_CLASSES_CNF:
	{
		getActiveCwClassesCnf = (CiSsPrimGetActiveCwClassesCnf*)paras;
		CiSsBasicServiceMap activeClasses = getActiveCwClassesCnf->ActClasses;
		CiSsBasicServiceMap reqClasses = getActiveCwClassesCnf->ReqClasses;
		CHAR tmp[20];
		if (getActiveCwClassesCnf->Result == CIRC_SS_SUCCESS)
		{
			if (reqClasses & CISS_BSMAP_VOICE)
			{
				if (activeClasses & CISS_BSMAP_VOICE)
				{
					sprintf((char *)tmp, "+CCWA: 1,1\r\n");
					strcat((char *)cwGetString, (char *)tmp);
				}
				else
				{
					sprintf((char *)tmp, "+CCWA: 0,1\r\n");
					strcat((char *)cwGetString, (char *)tmp);
				}
			}
			if (reqClasses & CISS_BSMAP_DATA)
			{
				if (activeClasses & CISS_BSMAP_DATA)
				{
					sprintf((char *)tmp, "+CCWA: 1,2\r\n");
					strcat((char *)cwGetString, (char *)tmp);
				}
				else
				{
					sprintf((char *)tmp, "+CCWA: 0,2\r\n");
					strcat((char *)cwGetString, (char *)tmp);
				}
			}
			if (reqClasses & CISS_BSMAP_FAX)
			{
				if (activeClasses & CISS_BSMAP_FAX)
				{
					sprintf((char *)tmp, "+CCWA: 1,4\r\n");
					strcat((char *)cwGetString, (char *)tmp);
				}
				else
				{
					sprintf((char *)tmp, "+CCWA: 0,4\r\n");
					strcat((char *)cwGetString, (char *)tmp);
				}
			}
			if (reqClasses & CISS_BSMAP_SMS)
			{
				if (activeClasses & CISS_BSMAP_SMS)
				{
					sprintf((char *)tmp, "+CCWA: 1,8\r\n");
					strcat((char *)cwGetString, (char *)tmp);
				}
				else
				{
					sprintf((char *)tmp, "+CCWA: 0,8\r\n");
					strcat((char *)cwGetString, (char *)tmp);
				}
			}
			if (reqClasses & CISS_BSMAP_DATA_SYNC)
			{
				if (activeClasses & CISS_BSMAP_DATA_SYNC)
				{
					sprintf((char *)tmp, "+CCWA: 1,16\r\n");
					strcat((char *)cwGetString, (char *)tmp);
				}
				else
				{
					sprintf((char *)tmp, "+CCWA: 0,16\r\n");
					strcat((char *)cwGetString, (char *)tmp);
				}
			}
			if (reqClasses & CISS_BSMAP_DATA_ASYNC)
			{
				if (activeClasses & CISS_BSMAP_DATA_ASYNC)
				{
					sprintf((char *)tmp, "+CCWA: 1,32\r\n");
					strcat((char *)cwGetString, (char *)tmp);
				}
				else
				{
					sprintf((char *)tmp, "+CCWA: 0,32\r\n");
					strcat((char *)cwGetString, (char *)tmp);
				}
			}
			if (reqClasses & CISS_BSMAP_PACKET)
			{
				if (activeClasses & CISS_BSMAP_PACKET)
				{
					sprintf((char *)tmp, "+CCWA: 1,64\r\n");
					strcat((char *)cwGetString, (char *)tmp);
				}
				else
				{
					sprintf((char *)tmp, "+CCWA: 0,64\r\n");
					strcat((char *)cwGetString, (char *)tmp);
				}
			}
			if (reqClasses & CISS_BSMAP_PAD)
			{
				if (activeClasses & CISS_BSMAP_PAD)
				{
					sprintf((char *)tmp, "+CCWA: 1,128\r\n");
					strcat((char *)cwGetString, (char *)tmp);
				}
				else
				{
					sprintf((char *)tmp, "+CCWA: 0,128\r\n");
					strcat((char *)cwGetString, (char *)tmp);
				}
			}
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, (char *)cwGetString);
		}
		else
			ssPrintResult( reqHandle, getActiveCwClassesCnf->Result );
		break;
	}
	case CI_SS_PRIM_GET_SS_NOTIFY_OPTIONS_CNF:
	{
		getSsNotOptCnf = (CiSsPrimGetSsNotifyOptionsCnf *)paras;
		if ( getSsNotOptCnf->Result == CIRC_SS_SUCCESS )
		{
			sprintf(ssRspBuf, "+CSSN: %d, %d\r\n", getSsNotOptCnf->SsiEnabled, getSsNotOptCnf->SsuEnabled);
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ssRspBuf);
		}
		else
		{
			ssPrintResult( reqHandle, getSsNotOptCnf->Result );
		}
		break;
	}

	case CI_SS_PRIM_SET_SS_NOTIFY_OPTIONS_CNF:
	{
		setSsNotOptCnf = (CiSsPrimSetSsNotifyOptionsCnf *)paras;
		ssPrintResult( reqHandle, setSsNotOptCnf->Result );
		break;
	}

	case CI_SS_PRIM_SET_USSD_ENABLE_CNF:
	{
		setUssdEnableCnf = (CiSsPrimSetUssdEnableCnf *)paras;
		ssPrintResult( reqHandle, setUssdEnableCnf->Result );
		break;
	}

	case CI_SS_PRIM_GET_USSD_ENABLE_CNF:
	{
		getUssdEnableCnf = (CiSsPrimGetUssdEnableCnf *)paras;
		if ( getUssdEnableCnf->Result == CIRC_SS_SUCCESS )
		{
			sprintf(ssRspBuf, "+CUSD: %d", getUssdEnableCnf->Enabled);
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ssRspBuf);
		}
		else
		{
			ssPrintResult( reqHandle, getUssdEnableCnf->Result );
		}
		break;
	}

	case CI_SS_PRIM_ABORT_USSD_SESSION_CNF:
	{
		abortUssdSessionCnf = (CiSsPrimAbortUssdSessionCnf *)paras;
		ssPrintResult( reqHandle, abortUssdSessionCnf->Result );
		gSSCntx.currentUssdState = ATCI_USSD_IDLE;
		break;
	}

	case CI_SS_PRIM_START_USSD_SESSION_CNF:
	{
		startUssdCnf = (CiSsPrimStartUssdSessionCnf *)paras;
		ssPrintResult( reqHandle, startUssdCnf->Result );
		break;
	}

	case CI_SS_PRIM_SET_COLP_OPTION_CNF:
	{
		setColpOptCnf = (CiSsPrimSetColpOptionCnf *)paras;
		ssPrintResult( reqHandle, setColpOptCnf->Result );
		break;
	}

	case CI_SS_PRIM_GET_COLP_STATUS_CNF:
	{
		getColpStatusCnf = (CiSsPrimGetColpStatusCnf *)paras;
		if ( getColpStatusCnf->Result == CIRC_SS_SUCCESS )
		{
			sprintf(ssRspBuf, "+COLP: %d, %d", getColpStatusCnf->Local, getColpStatusCnf->Provision);
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ssRspBuf);
		}
		else
		{
			ssPrintResult( reqHandle, getColpStatusCnf->Result );
		}
		break;
	}

	case CI_SS_PRIM_GET_COLR_STATUS_CNF:
	{
		getColrStatusCnf = (CiSsPrimGetColrStatusCnf*)paras;
		if ( getColrStatusCnf->Result == CIRC_SS_SUCCESS )
		{
			sprintf( ssRspBuf, "+COLR: %d, %d", getColrStatusCnf->Local, getColrStatusCnf->Provision );
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ssRspBuf );
		}
		else
		{
			ssPrintResult( reqHandle, getColrStatusCnf->Result );
		}
		break;
	}

	case CI_SS_PRIM_GET_CNAP_STATUS_CNF:
	{
		getCnapStatusCnf = (CiSsPrimGetCnapStatusCnf *)paras;
		if ( getCnapStatusCnf->Result == CIRC_SS_SUCCESS )
		{
			sprintf(ssRspBuf, "+CNAP: %d, %d", getCnapStatusCnf->Local, getCnapStatusCnf->Provision);
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ssRspBuf);
		}
		else
		{
			ssPrintResult( reqHandle, getCnapStatusCnf->Result );
		}
		break;
	}

	case CI_SS_PRIM_SET_CNAP_OPTION_CNF:
	{
		setCnapOptionCnf = (CiSsPrimSetCnapOptionCnf *)paras;
		ssPrintResult( reqHandle, setCnapOptionCnf->Result );
		break;
	}

	case CI_SS_PRIM_SET_CLIP_OPTION_CNF:
	{
		setClipOptCnf = (CiSsPrimSetClipOptionCnf *)paras;
		ssPrintResult( reqHandle, setClipOptCnf->Result );
		break;
	}

	case CI_SS_PRIM_GET_CLIP_STATUS_CNF:
	{
		getClipStatusCnf = (CiSsPrimGetClipStatusCnf *)paras;
		if ( getClipStatusCnf->Result == CIRC_SS_SUCCESS )
		{
			sprintf(ssRspBuf, "+CLIP: %d, %d", getClipStatusCnf->Local, getClipStatusCnf->Provision);
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ssRspBuf);
		}
		else
		{
			ssPrintResult( reqHandle, getClipStatusCnf->Result );
		}
		break;
	}
	/*Michal Bukai - CDIP support - Start*/
	case CI_SS_PRIM_SET_CDIP_OPTION_CNF:
	{
		setCdipOptCnf = (CiSsPrimSetCdipOptionCnf *)paras;
		ssPrintResult( reqHandle, setCdipOptCnf->Result );
		break;
	}

	case CI_SS_PRIM_GET_CDIP_STATUS_CNF:
	{
		getCdipStatusCnf = (CiSsPrimGetCdipStatusCnf *)paras;
		if ( getCdipStatusCnf->Result == CIRC_SS_SUCCESS )
		{
			sprintf(ssRspBuf, "+CDIP: %d, %d\r\n", getCdipStatusCnf->Local, getCdipStatusCnf->Provision);
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ssRspBuf);
		}
		else
		{
			ssPrintResult( reqHandle, getCdipStatusCnf->Result );
		}
		break;
	}
	/*Michal Bukai - CDIP support - End*/

	case CI_SS_PRIM_SET_CB_ACTIVATE_CNF:
	{
		setCbActCnf = (CiSsPrimSetCbActivateCnf *)paras;
		ssPrintResult( reqHandle, setCbActCnf->Result );
		break;
	}

	case CI_SS_PRIM_GET_CB_MAP_STATUS_CNF:
	{
		getCbMapStatusCnf = (CiSsPrimGetCbMapStatusCnf *)paras;
		if ( getCbMapStatusCnf->Result == CIRC_SS_SUCCESS )
		{
			if ( getCbMapStatusCnf->ActClasses== 0 )
			{
				ATRESP( reqHandle, 0, 0, (char *)"+CLCK: 0\r\n");
			}
			else
			{
				if ( getCbMapStatusCnf->ReqClasses & CISS_BSMAP_VOICE )
				{
					sprintf(ssRspBuf, "+CLCK: %d, 1\r\n", (getCbMapStatusCnf->ActClasses & CISS_BSMAP_VOICE) != 0 ? 1: 0);
					ATRESP( reqHandle, 0, 0, ssRspBuf);
				}
				if ( getCbMapStatusCnf->ReqClasses  & CISS_BSMAP_DATA )
				{
					sprintf(ssRspBuf, "+CLCK: %d, 2\r\n", (getCbMapStatusCnf->ActClasses & CISS_BSMAP_DATA) != 0 ? 1: 0);
					ATRESP( reqHandle, 0, 0, ssRspBuf);
				}
				if ( getCbMapStatusCnf->ReqClasses  & CISS_BSMAP_FAX )
				{
					sprintf(ssRspBuf, "+CLCK: %d, 4\r\n", (getCbMapStatusCnf->ActClasses & CISS_BSMAP_FAX) != 0 ? 1: 0);
					ATRESP( reqHandle, 0, 0, ssRspBuf);
				}
				if ( getCbMapStatusCnf->ReqClasses  & CISS_BSMAP_SMS )
				{
					sprintf(ssRspBuf, "+CLCK: %d, 8\r\n", (getCbMapStatusCnf->ActClasses & CISS_BSMAP_SMS) != 0 ? 1: 0 );
					ATRESP( reqHandle, 0, 0, ssRspBuf);
				}
				if ( getCbMapStatusCnf->ReqClasses  & CISS_BSMAP_DATA_SYNC )
				{
					sprintf(ssRspBuf, "+CLCK: %d, 16\r\n", (getCbMapStatusCnf->ActClasses & CISS_BSMAP_DATA_SYNC) != 0 ? 1: 0);
					ATRESP( reqHandle, 0, 0, ssRspBuf);
				}
				if ( getCbMapStatusCnf->ReqClasses  & CISS_BSMAP_DATA_ASYNC )
				{
					sprintf(ssRspBuf, "+CLCK: %d, 32\r\n", (getCbMapStatusCnf->ActClasses & CISS_BSMAP_DATA_ASYNC) != 0 ? 1: 0);
					ATRESP( reqHandle, 0, 0, ssRspBuf);
				}
				if ( getCbMapStatusCnf->ReqClasses  & CISS_BSMAP_PACKET )
				{
					sprintf(ssRspBuf, "+CLCK: %d, 64\r\n", (getCbMapStatusCnf->ActClasses & CISS_BSMAP_PACKET) != 0 ? 1: 0);
					ATRESP( reqHandle, 0, 0, ssRspBuf);
				}
				if ( getCbMapStatusCnf->ReqClasses  & CISS_BSMAP_PAD )
				{
					sprintf(ssRspBuf, "+CLCK: %d, 128\r\n", (getCbMapStatusCnf->ActClasses & CISS_BSMAP_PAD) != 0 ? 1: 0);
					ATRESP( reqHandle, 0, 0, ssRspBuf);
				}
			}

			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);

		}
		else
		{
			ssPrintResult( reqHandle, getCbMapStatusCnf->Result );
		}
		break;
	}

	case CI_SS_PRIM_CHANGE_CB_PASSWORD_CNF:
	{
		chgCbPswdCnf = (CiSsPrimChangeCbPasswordCnf *)paras;
		ssPrintResult( reqHandle, chgCbPswdCnf->Result );
		break;
	}

	case CI_CC_PRIM_MANIPULATE_CALLS_CNF:
	{
		manipulateCallsCnf = (CiCcPrimManipulateCallsCnf *)paras;
		ssPrintResult( reqHandle, manipulateCallsCnf->Result );
		break;
	}

	case CI_SS_PRIM_GET_CLIR_STATUS_CNF:
	{
		getClirStatusCnf = (CiSsPrimGetClirStatusCnf*)paras;
		if ( getClirStatusCnf->Result == CIRC_SS_SUCCESS )
		{
			sprintf( ssRspBuf, "+CLIR: %d, %d", getClirStatusCnf->Local, getClirStatusCnf->Provision );
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ssRspBuf );
		}
		else
		{
			ssPrintResult( reqHandle, getClirStatusCnf->Result );
		}
		break;
	}

	case CI_SS_PRIM_SET_CLIR_OPTION_CNF:
	{
		setClirOptionCnf = (CiSsPrimSetClirOptionCnf*)paras;
		ssPrintResult( reqHandle, setClirOptionCnf->Result );
		break;
	}

	case CI_ERR_PRIM_HASNOSUPPORT_CNF:
	case CI_ERR_PRIM_HASINVALIDPARAS_CNF:
	case CI_ERR_PRIM_ISINVALIDREQUEST_CNF:
	{
		/* Reply with an "Operation not supported" error (CME ERROR 4) */
		ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}

	case CI_SS_PRIM_GET_AOC_WARNING_ENABLE_CNF:
	{
		getCcweModeCnf = (CiSsPrimGetAocWarningEnableCnf*)paras;
		if ( getCcweModeCnf->Result == CIRC_SS_SUCCESS )
		{
			sprintf( ssRspBuf, "+CCWE: %d", getCcweModeCnf->Enabled);
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ssRspBuf );
		}
		else
		{
			ssPrintResult( reqHandle, getCcweModeCnf->Result );
		}
		break;
	}

	case CI_SS_PRIM_SET_AOC_WARNING_ENABLE_CNF:
	{
		setCcweModeCnf = (CiSsPrimSetAocWarningEnableCnf*)paras;
		ssPrintResult( reqHandle, setCcweModeCnf->Result );
		break;
	}

	case CI_SS_PRIM_GET_CCM_OPTION_CNF:
	{
		getCcmOptionCnf = (CiSsPrimGetCcmOptionCnf*)paras;

		if ( getCcmOptionCnf->Result == CIRC_SS_SUCCESS )
		{
			sprintf( ssRspBuf, "+CAOC: %d", getCcmOptionCnf->Enabled ? ATCI_AOC_MODE_ACTIVATE : ATCI_AOC_MODE_DEACTIVATE);
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ssRspBuf );
		}
		else
		{
			ssPrintResult( reqHandle, getCcmOptionCnf->Result );
		}

		break;
	}

	case CI_SS_PRIM_SET_CCM_OPTION_CNF:
	{
		setCcmOptionCnf = (CiSsPrimSetCcmOptionCnf*)paras;
		ssPrintResult( reqHandle, setCcmOptionCnf->Result );

		break;
	}

	default:
	{
		ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		break;
	}
	}

	/* free up the confirmation memory */
	atciSvgFreeCnfMem( svgId, primId, paras );

	return;
}


/************************************************************************************
 *
 * SS CI notifications
 *
 *************************************************************************************/
/************************************************************************************
 * F@: convertAtDcs2CiDcs
 *
 */
static INT8 convertCiDcs2AtDcs ( CiUssdCharSet ciCharSet )
{
	INT8 ussdDcs = 0;

	switch (ciCharSet)
	{
	case CISS_USSD_CHARSET_HEX:                        /* Hexadecimal Digit Character String */
		ussdDcs = 0x04;
		break;

	/* Character Set codes for MO-USSD */
	case CISS_USSD_CHARSET_MO_GSM7BIT:                              /* IRA Text -> GSM 7-bit */
		ussdDcs = 0x10;                                         /* or 0x40 !! */
		break;

	case CISS_USSD_CHARSET_MO_UCS2:                                 /* Hex Digit String -> UCS2 */
		ussdDcs = 0x11;                                         /* or 0x08 !! */
		break;

	case CISS_USSD_CHARSET_USER:                            /* User-Specified Representation */
	case CISS_USSD_CHARSET_IRA:                             /* IRA "ASCII" Text String */
	case CISS_USSD_UNKNOWN:                                 /* Unknown Representation */
	default:
		break;
	}

	return ussdDcs;
}

static void convertCiStr2AtStr(char* data, UINT8 dataLength, char* outBuf)
{

	char tmp[3];
	UINT8 i = 0;

	outBuf[0] = '\0';
	while (i < dataLength)
	{
		sprintf(tmp, "%02x", data[i++]);
		strcat(outBuf, tmp);
	}
}

void ssInd(CiSgOpaqueHandle opSgOpaqueHandle,
	   CiServiceGroupID svgId,
	   CiPrimitiveID primId,
	   CiIndicationHandle indHandle,
	   void*               pParam)
{
	UNUSEDPARAM(opSgOpaqueHandle)
	UNUSEDPARAM(indHandle)
	CiSsPrimReceivedUssdInfoInd    * ussdInfoInd;
	CiSsPrimLocationInd            * locationInd;
	INT8 ussdDcs;
	DBGMSG("[%s] Line(%d): primId(%d) \n", __FUNCTION__, __LINE__, primId);
	char ssIndBuf[500];
	char ssIndTmpBuf[500];
	switch (primId)
	{
	case CI_SS_PRIM_RECEIVED_USSD_INFO_IND:
	{
		ussdInfoInd = (CiSsPrimReceivedUssdInfoInd *)pParam;
		DBGMSG("[%s] Line(%d): ussdInfoInd->Status(%d) \n", __FUNCTION__, __LINE__, ussdInfoInd->Status);
		if ( ussdInfoInd->Status == CISS_USSD_STATUS_MORE_INFO_NEEDED )
		{
			gSSCntx.currentUssdState = ATCI_USSD_MT_WAIT_TE_RSP;
		}
		else
		{
			gSSCntx.currentUssdState = ATCI_USSD_IDLE;
		}
		DBGMSG("[%s] Line(%d): ussdInfoInd->info.DataLength(%d),ussdInfoInd->Info.data(%.*s) \n", __FUNCTION__, __LINE__, ussdInfoInd->info.DataLength, ussdInfoInd->info.DataLength, ussdInfoInd->info.Data);
		/* only for the following two cases: the USSD information is forwarded to TE */
		if (gSSCntx.currentUssdState == ATCI_USSD_MT_WAIT_TE_RSP || gSSCntx.ussdMode == ATCI_SS_PRESENTATION_ENABLE)
		{
			if ( ussdInfoInd->info.DataLength > 0 )
			{
				ussdDcs = convertCiDcs2AtDcs( ussdInfoInd->info.CharSet );
				convertCiStr2AtStr((char*)ussdInfoInd->info.Data, ussdInfoInd->info.DataLength, ssIndTmpBuf);

				sprintf(ssIndBuf, "+CUSD: %d,\"%s\", %d\r\n", ussdInfoInd->Status, ssIndTmpBuf, ussdDcs );
			}
			else
			{
				sprintf(ssIndBuf, "+CUSD: %d\r\n", ussdInfoInd->Status);
			}

			ATRESP( IND_REQ_HANDLE, 0, 0, ssIndBuf);
		}

		break;
	}
	case CI_SS_PRIM_LOCATION_IND:
	{
		int i, len;
		INT8 *buf;
		locationInd = (CiSsPrimLocationInd *)pParam;

		/* +LPLOC:mIvokeHandle, notifyType,locationType.locEstimateType,
				  locationType.deferredLocationTypePresent,[locationType.deferredLocationType],
				  lcsOptClientExternalId.present,[optExternalAddress.present,[len,strID]],
				  lcsOptClientName.present,[scheme, len , name,
				  						optFormatIndicator.present,[formatIndicator]],
				  lcsOptRequestorId.present,[scheme, len , name,
				  						optFormatIndicator.present,[formatIndicator]]
		*/

		/* +LPLOC: mIvokeHandle, notifyType, locationtype.locEstimateType */
		sprintf(ssIndBuf, "+LPLOC:%d,%d,%d,", locationInd->mtInvokeHandle,
												 locationInd->ssLcsLocNotifyInfo.notifyType,
												 locationInd->ssLcsLocNotifyInfo.locationType.locEstimateType);

		/* */
		sprintf(ssIndTmpBuf, "%d,", locationInd->ssLcsLocNotifyInfo.locationType.deferredLocationTypePresent);
		strcat(ssIndBuf, ssIndTmpBuf);

		if (locationInd->ssLcsLocNotifyInfo.locationType.deferredLocationTypePresent)
		{
			sprintf(ssIndTmpBuf, "%d,",
				locationInd->ssLcsLocNotifyInfo.locationType.deferredLocationType);
			strcat(ssIndBuf, ssIndTmpBuf);
		}

		/* lcsOptClientExternalId */
		sprintf(ssIndTmpBuf, "%d,", locationInd->ssLcsLocNotifyInfo.lcsOptClientExternalId.present);
		strcat(ssIndBuf, ssIndTmpBuf);

		if (locationInd->ssLcsLocNotifyInfo.lcsOptClientExternalId.present)
		{
			sprintf(ssIndTmpBuf, "%d,",
				locationInd->ssLcsLocNotifyInfo.lcsOptClientExternalId.optExternalAddress.present);
			strcat(ssIndBuf, ssIndTmpBuf);

			if (locationInd->ssLcsLocNotifyInfo.lcsOptClientExternalId.optExternalAddress.present)
			{
				sprintf(ssIndTmpBuf, "%d,",
					locationInd->ssLcsLocNotifyInfo.lcsOptClientExternalId.optExternalAddress.addressLength);
				strcat(ssIndBuf, ssIndTmpBuf);

				len = locationInd->ssLcsLocNotifyInfo.lcsOptClientExternalId.optExternalAddress.addressLength;
				buf = locationInd->ssLcsLocNotifyInfo.lcsOptClientExternalId.optExternalAddress.externalAddress;
				for (i = 0; i < len; ++i)
				{
					sprintf(ssIndTmpBuf, "%02x", buf[i]);
					strcat(ssIndBuf, ssIndTmpBuf);
				}
				strcat(ssIndBuf, ",");
			}

		}

		/* lcsOptClientName */
		sprintf(ssIndTmpBuf, "%d,", locationInd->ssLcsLocNotifyInfo.lcsOptClientName.present);
		strcat(ssIndBuf, ssIndTmpBuf);

		if (locationInd->ssLcsLocNotifyInfo.lcsOptClientName.present)
		{
			sprintf(ssIndTmpBuf, "%d,%d,",
				locationInd->ssLcsLocNotifyInfo.lcsOptClientName.dataCodingScheme.scheme,
				locationInd->ssLcsLocNotifyInfo.lcsOptClientName.nameString.length);
			strcat(ssIndBuf, ssIndTmpBuf);

			len = locationInd->ssLcsLocNotifyInfo.lcsOptClientName.nameString.length;
			buf = locationInd->ssLcsLocNotifyInfo.lcsOptClientName.nameString.name;
			for (i = 0; i < len; ++i)
			{
				sprintf(ssIndTmpBuf, "%02x", buf[i]);
				strcat(ssIndBuf, ssIndTmpBuf);
			}

			sprintf(ssIndTmpBuf, ",%d,", locationInd->ssLcsLocNotifyInfo.lcsOptClientName.optFormatIndicator.present);
			strcat(ssIndBuf, ssIndTmpBuf);

			if (locationInd->ssLcsLocNotifyInfo.lcsOptClientName.optFormatIndicator.present)
			{
				sprintf(ssIndTmpBuf, "%d,",
					locationInd->ssLcsLocNotifyInfo.lcsOptClientName.optFormatIndicator.formatIndicator);
				strcat(ssIndBuf, ssIndTmpBuf);
			}
		}

		/* lcsOptRequestorId */
		sprintf(ssIndTmpBuf, "%d", locationInd->ssLcsLocNotifyInfo.lcsOptRequestorId.present);
		strcat(ssIndBuf, ssIndTmpBuf);

		if (locationInd->ssLcsLocNotifyInfo.lcsOptRequestorId.present)
		{
			sprintf(ssIndTmpBuf, ",%d,%d,",
				locationInd->ssLcsLocNotifyInfo.lcsOptRequestorId.dataCodingScheme.scheme,
				locationInd->ssLcsLocNotifyInfo.lcsOptRequestorId.requestorIdString.length);
			strcat(ssIndBuf, ssIndTmpBuf);

			len = locationInd->ssLcsLocNotifyInfo.lcsOptRequestorId.requestorIdString.length;
			buf = locationInd->ssLcsLocNotifyInfo.lcsOptRequestorId.requestorIdString.name;
			for (i = 0; i < len; ++i)
			{
				sprintf(ssIndTmpBuf, "%02x", buf[i]);
				strcat(ssIndBuf, ssIndTmpBuf);
			}

			sprintf(ssIndTmpBuf, ",%d", locationInd->ssLcsLocNotifyInfo.lcsOptRequestorId.optFormatIndicator.present);
			strcat(ssIndBuf, ssIndTmpBuf);

			if (locationInd->ssLcsLocNotifyInfo.lcsOptRequestorId.optFormatIndicator.present)
			{
				sprintf(ssIndTmpBuf, ",%d",
					locationInd->ssLcsLocNotifyInfo.lcsOptRequestorId.optFormatIndicator.formatIndicator);
				strcat(ssIndBuf, ssIndTmpBuf);
			}
		}
		ATRESP( IND_REQ_HANDLE, 0, 0, ssIndBuf);
		break;
	}
	default:
		break;
	}

	/* free up the indication memory */
	atciSvgFreeIndMem( svgId, primId, pParam );

	return;
}

//AT+CCWE?
CiReturnCode SS_GetCCWEMode(UINT32 atHandle){
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimGetAocWarningEnableReq   *getCcweModeReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_GET_AOC_WARNING_ENABLE_REQ,
			MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_GET_AOC_WARNING_ENABLE_REQ), (void *)getCcweModeReq );

	return ret;
}

//AT+CCWE=
CiReturnCode SS_SetCCWEMode(UINT32 atHandle, INT32 ccweMode){
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimSetAocWarningEnableReq  *setCcweModeReq = utlCalloc(1, sizeof(CiSsPrimSetAocWarningEnableReq));

	if (setCcweModeReq == NULL)
		return CIRC_FAIL;

	setCcweModeReq->Enable = (CiBoolean)ccweMode;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_SET_AOC_WARNING_ENABLE_REQ,
			MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_SET_AOC_WARNING_ENABLE_REQ), (void *)setCcweModeReq );
	return ret;
}
//AT+LPLOCVR=
CiReturnCode SS_LocationVerificationRsp(UINT32 atHandle, INT32 taskid, INT32 mInvokeHandle, INT32 respresent, INT32 veriresp)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimLocationVerifyRsp locationVerifyRsp;

	locationVerifyRsp.taskId = taskid;
	locationVerifyRsp.mtInvokeHandle = mInvokeHandle;
	locationVerifyRsp.response.present = respresent;
	locationVerifyRsp.response.verificationResponse = veriresp;

	ret = ciRespond( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_LOCATION_VERIFY_RSP,
			MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_LOCATION_VERIFY_RSP), (void *)&locationVerifyRsp );
	return ret;

}

//AT+CAOC=
CiReturnCode SS_SetCcmOption(UINT32 atHandle, UINT8 Enable)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimSetCcmOptionReq *setCcmOptionReq = utlCalloc(1, sizeof(CiSsPrimSetCcmOptionReq));

	if (setCcmOptionReq == NULL)
		return CIRC_FAIL;

	setCcmOptionReq->Enable = Enable;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_SET_CCM_OPTION_REQ,
			MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_SET_CCM_OPTION_REQ), (void *)setCcmOptionReq);
	return ret;
}

//AT+CAOC?
CiReturnCode SS_GetCcmOption(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSsPrimGetCcmOptionReq *getCcmOptionReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_GET_CCM_OPTION_REQ,
			MAKE_CI_REQ_HANDLE(atHandle, CI_SS_PRIM_GET_CCM_OPTION_REQ), (void *)getCcmOptionReq);
	return ret;
}

void resetSsParas(void)
{
	gSSCntx. queryCallClassStatus = 0;
	gSSCntx.ussdMode = ATCI_SS_PRESENTATION_ENABLE;
	gSSCntx.currentUssdState = 0;
	gSSCntx.localCwOption = FALSE;
}

