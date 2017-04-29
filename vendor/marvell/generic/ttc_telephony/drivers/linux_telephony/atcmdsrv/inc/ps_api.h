/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: ps_api.h
 *
 *  Description: API interface for packet service
 *
 *  History:
 *   Sep 18, 2008 - Rovin Yu Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/


#ifndef __PS_API_H__
#define __PS_API_H__

#include "ci_ps.h"

CiReturnCode PS_GetTftList(UINT32 atHandle);
CiReturnCode PS_DeleteTft(UINT32 atHandle, UINT32 dwContextID);
CiReturnCode PS_SetTftFilter(UINT32 atHandle, UINT32 dwContextID, const CiPsTftFilter *tftFilter);
CiReturnCode PS_GetGPRSActiveCidList(UINT32 atHandle);
CiReturnCode PS_ModifyGPRSContext(UINT32 atHandle, UINT32 dwContextID, BOOL doAll);
CiReturnCode PS_SetGPRSRegStatus(UINT32 atHandle, UINT32 state);
CiReturnCode PS_GetGPRSRegStatus(UINT32 atHandle);
CiReturnCode PS_GetGPRSContextList(UINT32 atHandle);
CiReturnCode PS_SetGPRSContext(UINT32 atHandle, const CiPsPdpCtx* lpGprsContext);
CiReturnCode PS_DeleteGPRSContext(UINT32 atHandle, UINT32 dwContextID);
CiReturnCode PS_GetQualityOfServiceList(UINT32 atHandle, BOOL isMin);
CiReturnCode PS_SetQualityOfService(UINT32 atHandle, UINT32 dwContextID, const CiPsQosProfile* lpGprsQosProfile, BOOL isMin);
CiReturnCode PS_SetGPRSAttached(UINT32 atHandle, BOOL fAttached);
CiReturnCode PS_GetGPRSAttached(UINT32 atHandle);
CiReturnCode PS_GetGPRSContextActivatedList(UINT32 atHandle);
CiReturnCode PS_SetGPRSContextActivated(UINT32 atHandle, UINT32 dwContextID, BOOL fContextActivation, BOOL doAll);
CiReturnCode PS_EnterGPRSDataMode(UINT32 atHandle, UINT32 dwContextID, CiPsL2P l2p);
CiReturnCode PS_Get3GQualityOfServiceList(UINT32 atHandle, CiPs3GQosType type);
CiReturnCode PS_Set3GQualityOfService(UINT32 atHandle, UINT32 dwContextID, const CiPs3GQosProfile * lp3GQosProfile, CiPs3GQosType type);
CiReturnCode PS_GetGprsCapsQos(UINT32 atHandle, BOOL isMin);
CiReturnCode PS_Get3GCapsQos(UINT32 atHandle, CiPs3GQosType type);
CiReturnCode PS_GetSecondaryContextList(UINT32 atHandle);
CiReturnCode PS_SetSecondaryPDPContext(UINT32 atHandle, const CiPsSecPdpCtx * lpSecondaryPDPContext);
CiReturnCode PS_GetSecondaryContextRange(UINT32 atHandle);
CiReturnCode PS_DeleteSecondaryPDPContext(UINT32 atHandle, UINT32 dwContextID);
CiReturnCode PS_SendAUTHReq(UINT32 atHandle, const CiPsPrimAuthenticateReq * lpAuthReq );
CiReturnCode PS_GetGPRSContextIP (UINT32 atHandle, UINT32 dwContextID);
CiReturnCode PS_FastDormancy(UINT32 atHandle);
#ifdef AT_CUSTOMER_HTC
CiReturnCode PS_SetFDY(UINT32 atHandle, UINT8 type, UINT16 interval);
#endif

void psCnf(CiSgOpaqueHandle opSgCnfHandle,
	   CiServiceGroupID svgId,
	   CiPrimitiveID primId,
	   CiRequestHandle reqHandle,
	   void*            paras);

void psInd(CiSgOpaqueHandle opSgIndHandle,
	   CiServiceGroupID svgId,
	   CiPrimitiveID primId,
	   CiIndicationHandle indHandle,
	   void*               paras);

void resetCidList(void);
int deactive_ps_connection(UINT8 atpIdx);
void resetPsParas(void);

#endif

