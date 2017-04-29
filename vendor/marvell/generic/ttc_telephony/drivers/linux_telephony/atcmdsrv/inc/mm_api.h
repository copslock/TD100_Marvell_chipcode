/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: mm_api.h
 *
 *  Description: API interface for Network service
 *
 *  History:
 *   Oct 15, 2008 - Qiang XU Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/


#ifndef __MM_API_H__
#define __MM_API_H__

#include "ci_mm.h"

CiReturnCode MM_QueryRegOption(UINT32 atHandle);
CiReturnCode MM_GetRegStatus(UINT32 atHandle);
CiReturnCode MM_SetRegOption(UINT32 atHandle, INT32 regOption);
CiReturnCode MM_QuerySuppOperator(UINT32 atHandle);
CiReturnCode MM_GetCurrOperator(UINT32 atHandle);
CiReturnCode MM_SetCurrOperator(UINT32 atHandle, int modeVal, int formatVal, char *networkOperator, int opeStrLen, int accTchVal);
CiReturnCode MM_GetPrefOperator(UINT32 atHandle);
CiReturnCode MM_SetPrefOperator(UINT32 atHandle, int indexVal, int formatVal, INT32 ActVal, char *networkOperator, int length);
CiReturnCode MM_GetTimeZoneReportOption(UINT32 atHandle);
CiReturnCode MM_QuerySuppOpeInfo(UINT32 atHandle, int opeIndex);
CiReturnCode MM_GetPrefOpeInfo(UINT32 atHandle, int opeIndex);
CiReturnCode MM_GetSignalQuality(UINT32 atHandle, int *rssi, int *ber);
CiReturnCode MM_GetSubscriber(UINT32 atHandle);
CiReturnCode MM_GetNumberOfEntries(UINT32 atHandle);
CiReturnCode MM_SetNwModeIndSetting(UINT32 atHandle, int bNwModeInd);
CiReturnCode MM_GetNwModeIndSetting(UINT32 atHandle);
CiReturnCode MM_SelectCPLS(UINT32 atHandle, UINT8 listIndex);
CiReturnCode MM_GetCPLS(UINT32 atHandle);
CiReturnCode MM_GetBANDIND(UINT32 atHandle);
CiReturnCode MM_SetBANDIND(UINT32 atHandle, BOOL bEnableBandInd);
CiReturnCode MM_GetstarTimeZoneReportOption(UINT32 atHandle);
CiReturnCode MM_SetCellLOCK(UINT32 atHandle, UINT32 mode, UINT32 arfcn, UINT32 scramblingCode);
CiReturnCode MM_SetRejectCauseStatus(UINT32 atHandle, int bEnableRejectCauseInd);
CiReturnCode MM_GetRejectCauseStatus(UINT32 atHandle);
CiReturnCode MM_GetRejectCauseHistory(UINT32 atHandle);
CiReturnCode MM_GetSysInfo(UINT32 atHandle);

void mmCnf(CiSgOpaqueHandle opSgCnfHandle,      CiServiceGroupID svgId, CiPrimitiveID primId, CiRequestHandle reqHandle, void* paras);
void mmInd(CiSgOpaqueHandle opSgOpaqueHandle, CiServiceGroupID svgId, CiPrimitiveID primId, CiIndicationHandle indHandle, void* pParam);
void resetMmParas(void);

#endif

