/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: cc_api.h
 *
 *  Description: API interface for call control service
 *
 *  History:
 *   August 27, 2008 - Rovin Yu Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/


#ifndef __CC_API_H__
#define __CC_API_H__

#include "ci_cc.h"

CiReturnCode CC_Dial(UINT32 atHandle, CHAR* szAddress, UINT32 dwType, UINT32 dwOptions);
CiReturnCode CC_Answer(UINT32 atHandle);
CiReturnCode CC_Hangup(UINT32 atHandle);
CiReturnCode CC_HangVT(UINT32 atHandle, UINT32 causeValue);
CiReturnCode CC_SendDTMF(UINT32 atHandle, CHAR* szChars, UINT32 dwDuration);
CiReturnCode CC_GetCallList(UINT32 atHandle);
CiReturnCode CC_ManageCalls(UINT32 atHandle, UINT32 dwCommand, UINT32 dwID);
CiReturnCode CC_HangupAllVoiceCall(UINT32 atHandle);
CiReturnCode CC_GetAcm(UINT32 atHandle);
CiReturnCode CC_SetAcm(UINT32 atHandle, CHAR *passwd);
CiReturnCode CC_GetAmm(UINT32 atHandle);
CiReturnCode CC_SetAmm(UINT32 atHandle, UINT32 acmMax, CHAR *passwd);
CiReturnCode CC_GetBst(UINT32 atHandle);
CiReturnCode CC_GetSupportBst(UINT32 atHandle);
CiReturnCode CC_SetBst(UINT32 atHandle, INT32 bstSpeed, INT32 bstName, INT32 bstCe);
CiReturnCode CC_GetMod(UINT32 atHandle);
CiReturnCode CC_GetSupportMod(UINT32 atHandle);
CiReturnCode CC_SetMod(UINT32 atHandle, INT32 callMode);
CiReturnCode CC_GetCcm(UINT32 atHandle);
CiReturnCode CC_GetRlp(UINT32 atHandle);
CiReturnCode CC_GetSupportRlp(UINT32 atHandle);
CiReturnCode CC_SetRlp(UINT32 atHandle, CiCcRlpCfg *RLPCfg);
CiReturnCode CC_ResetAcmValue(UINT32 atHandle);
CiReturnCode CC_SetAcmMaxValue(UINT32 atHandle, int acmMaxValue);
CiReturnCode CC_GetDS(UINT32 atHandle);
CiReturnCode CC_GetSupportDS(UINT32 atHandle);
CiReturnCode CC_SetDS(UINT32 atHandle, CiCcDataCompInfo *pDataCompInfo);
CiReturnCode CC_SetCSTA(UINT32 atHandle, CiAddressType* pAddressType);
CiReturnCode CC_GetCSTA(UINT32 atHandle);
CiReturnCode CC_GetCPUC(UINT32 atHandle);
CiReturnCode CC_SetCPUC(UINT32 atHandle, BOOL isNoPasswd, INT16 param_2_Length, CHAR *cpucChvPswd);
CiReturnCode CC_GetDTMFDuration(UINT32 atHandle);
CiReturnCode CC_SetDTMFDuration(UINT32 atHandle, UINT32 duration);
CiReturnCode CC_SendLongDTMF(UINT32 atHandle, UINT8 DTMFChar, BOOL mode);
#ifdef AT_CUSTOMER_HTC
CiReturnCode CC_EditBlackList(UINT32 atHandle, UINT8 type, UINT16 number, const int *record_indexes, const CHAR *records, UINT8 maxRecordSize);
#endif

void ccCnf(CiSgOpaqueHandle opSgCnfHandle,
	   CiServiceGroupID svgId,
	   CiPrimitiveID primId,
	   CiRequestHandle reqHandle,
	   void*            paras);

void ccInd(CiSgOpaqueHandle opSgIndHandle,
	   CiServiceGroupID svgId,
	   CiPrimitiveID primId,
	   CiIndicationHandle indHandle,
	   void*               paras);

BOOL convertATbstSpeed2CI( INT32 ATbstSpeed, CiBsTypeSpeed *CIbstSpeed );
BOOL convertATbstName2CI( INT32 ATbstName, CiCcBsTypeName *CIbstName );
BOOL convertATbstCe2CI( INT32 ATbstCe, CiCcBsTypeCe *CIbstCe );

void resetAllCalls( void );

#endif

