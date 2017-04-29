/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: ss_api.h
 *
 *  Description: API interface for supplementary service
 *
 *  History:
 *   Oct 15, 2008 - Rovin Yu Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/


#ifndef __SS_API_H__
#define __SS_API_H__

#include "ci_ss.h"

#define ATCI_SS_PRESENTATION_ENABLE  1
#define ATCI_SS_PRESENTATION_DISABLE 0
#define ATCI_SS_PRESENTATION_CANCEL  2

CiReturnCode SS_GetCdipStatus(UINT32 atHandle);
CiReturnCode SS_SetCdipOption(UINT32 atHandle, UINT32 dwStatus);
CiReturnCode SS_GetCallerIdSettings(UINT32 atHandle);
CiReturnCode SS_SetCallerIdStatus(UINT32 atHandle, UINT32 dwStatus);
CiReturnCode SS_GetHideIdSettings(UINT32 atHandle);
CiReturnCode SS_SetHideIdStatus(UINT32 atHandle, UINT32 dwStatus);
CiReturnCode SS_GetHideConnectedIdSettings(UINT32 atHandle);
CiReturnCode SS_GetDialedIdSettings(UINT32 atHandle);
CiReturnCode SS_SetDialedIdStatus(UINT32 atHandle, UINT32 dwStatus);
CiReturnCode SS_GetCallForwardingSettings(UINT32 atHandle, UINT32 dwReason, UINT32 dwInfoClasses);
CiReturnCode SS_AddCallForwarding(UINT32 atHandle, const CiSsCfRegisterInfo* lpSettings);
CiReturnCode SS_RemoveCallForwarding(UINT32 atHandle, UINT32 dwReason, UINT32 dwInfoClasses);
CiReturnCode SS_SetCallForwardingStatus(UINT32 atHandle, UINT32 dwReason, UINT32 dwInfoClasses, BOOL bStatus);
CiReturnCode SS_GetCallWaitingSettings(UINT32 atHandle, UINT32 dwInfoClasses);
CiReturnCode SS_SetCallWaitingStatus(UINT32 atHandle, UINT32 dwInfoClasses, BOOL bStatus);
CiReturnCode SS_SendSupServiceData(UINT32 atHandle, UINT32 charSet, const CHAR* lpbData, UINT32 dwSize);
CiReturnCode SS_CancelSupServiceDataSession(UINT32 atHandle);
CiReturnCode SS_GetNamePresentationSettings(UINT32 atHandle);
CiReturnCode SS_SetNamePresentationStatus(UINT32 atHandle, UINT32 dwStatus);
CiReturnCode SS_SetNotificationOptions(UINT32 atHandle, BOOL ssiEnable, BOOL ssuEnable);
CiReturnCode SS_GetNotificationOptions(UINT32 atHandle);
CiReturnCode SS_SetCallBarringStatus(UINT32 atHandle, UINT32 dwType, UINT32 dwInfoClass, CHAR* lpszPassword, BOOL bEnable);
CiReturnCode SS_GetCallBarringStatus(UINT32 atHandle, UINT32 dwType, UINT32 dwInfoClass, CHAR* lpszPassword);
CiReturnCode SS_ChangeCallBarringPassword(UINT32 atHandle, CHAR* lpszOldPassword, CHAR* lpszNewPassword, CHAR* lpszNewPasswordVerify);
CiReturnCode SS_SendMarvellPrivateCode(UINT32 atHandle, const CHAR* subCode, UINT32 dwSize);
CiReturnCode SS_LocationVerificationRsp(UINT32 atHandle, INT32 taskid, INT32 mInvokeHandle, INT32 respresent, INT32 veriresp);
CiReturnCode SS_SetCcmOption(UINT32 atHandle, UINT8 Enable);
CiReturnCode SS_GetCcmOption(UINT32 atHandle);


void ssCnf(CiSgOpaqueHandle opSgCnfHandle,
	   CiServiceGroupID svgId,
	   CiPrimitiveID primId,
	   CiRequestHandle reqHandle,
	   void*            paras);

void ssInd(CiSgOpaqueHandle opSgIndHandle,
	   CiServiceGroupID svgId,
	   CiPrimitiveID primId,
	   CiIndicationHandle indHandle,
	   void*               paras);

void resetSsParas(void);

#endif

