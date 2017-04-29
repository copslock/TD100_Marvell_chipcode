/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: msg_api.h
 *
 *  Description: API interface for SMS service
 *
 *  History:
 *   September 22, 2008 - Qiang XU Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/


#ifndef __MSG_API_H__
#define __MSG_API_H__

#include "ci_msg.h"

CiReturnCode MSG_QuerySmsService(UINT32 atHandle);
CiReturnCode MSG_GetSmsService(UINT32 atHandle);
CiReturnCode MSG_SetSmsService(UINT32 atHandle, const UINT32 service);
CiReturnCode MSG_QuerySupportStorage(UINT32 atHandle);
CiReturnCode MSG_GetPrefStorage(UINT32 atHandle);
CiReturnCode MSG_SetPrefStorage(UINT32 atHandle, CHAR *mem1Str, CHAR *mem2Str, CHAR *mem3Str, INT16 mem1StrLen, INT16 mem2StrLen, INT16 mem3StrLen);
CiReturnCode MSG_GetSrvCenterAddr(UINT32 atHandle);
CiReturnCode MSG_SetSrvCenterAddr(UINT32 atHandle, const CHAR *addressStr, UINT16 addrStrLen, UINT32 type);
CiReturnCode MSG_GetMsgTextMode(UINT32 atHandle, UINT8 *fo, UINT8 *vp, UINT8 *pid, UINT8 *dcs);
CiReturnCode MSG_SetMsgTextMode(UINT32 atHandle, const UINT8 fo, const UINT8 vp, const UINT8 pid, const UINT8 dcs);
CiReturnCode MSG_GetCbMsgType(UINT32 atHandle);
CiReturnCode MSG_SetCbMsgType(UINT32 atHandle, INT32 mode, CHAR *mids, CHAR *dcss);
CiReturnCode MSG_GetNewMsgIndSetting(UINT32 atHandle, UINT32 *mode, UINT32 *mt, UINT32 *bm, UINT32 *ds, UINT32 *bfr);
CiReturnCode MSG_SetNewMsgIndSetting(UINT32 atHandle, const INT32 mode, const INT32 mt, const INT32 bm, const INT32 ds, const INT32 bfr);
CiReturnCode MSG_GetStorageInfo(UINT32 atHandle, CiPrimitiveID priPrimId);
CiReturnCode MSG_ListMsgStart(UINT32 atHandle, const UINT32 readSmsOption);
CiReturnCode MSG_ReadMsg(UINT32 atHandle, const UINT32 msgIndex);
CiReturnCode MSG_SetSentMsgInfo(UINT32 atHandle, const UINT8 smsFormatMode, const UINT8 maxMsgLen, const CiAddressInfo *pDestAddr);
CiReturnCode MSG_SendStoredMsg(UINT32 atHandle, const UINT32 msgIndex, const CiAddressInfo newDestAddr);
CiReturnCode MSG_SetStoredMsgInfo(UINT32 atHandle, const UINT8 smsFormatMode, const UINT8 maxMsgLen, const CiAddressInfo *pDestAddr, const UINT8 stat);
CiReturnCode MSG_QueryDelMsg(UINT32 atHandle);
CiReturnCode MSG_DeleteMsg(UINT32 atHandle, const UINT32 msgIndex, const UINT8 delFlag);
CiReturnCode MSG_DeleteAllMsgStart(UINT32 atHandle);
CiReturnCode MSG_GetModeMoreMsgToSend(UINT32 atHandle, UINT32 *mode);
CiReturnCode MSG_SetModeMoreMsgToSend(UINT32 atHandle, const UINT32 mode);
CiReturnCode MSG_QueryMoService(UINT32 atHandle);
CiReturnCode MSG_GetMoService(UINT32 atHandle);
CiReturnCode MSG_SetMoService(UINT32 atHandle, UINT32 service);
CiReturnCode MSG_SendMsg(UINT32 reqHandle, INT32 formatMode, const CHAR *pSmsData, const INT32 smsDataLength, const CHAR *pSmscData, const INT32 smscLength);
CiReturnCode MSG_WriteMsgWithIndex(UINT32 atHandle, INT32 index, const CiMsgPdu smsPdu);
CiReturnCode MSG_NewMsgAck(UINT32 atHandle, UINT32 reply);
CiReturnCode __MSG_NewMsgAck(UINT32 atHandle, UINT32 reply);

CiReturnCode MSG_CRSM_ReadMsg(UINT32 atHandle, const UINT32 msgIndex);
CiReturnCode MSG_CRSM_DeleteMsg(UINT32 atHandle, const UINT32 msgIndex, const UINT8 delFlag);
CiReturnCode MSG_CRSM_WriteMsgWithIndex(UINT32 atHandle, INT32 index, const CiMsgPdu *smsPdu);

//CiReturnCode MSG_RSTMemFull(UINT32 atHandle);
CiReturnCode MSG_CMEMFULL(UINT32 atHandle, BOOL bMemFull);

void msgCnf(CiSgOpaqueHandle opSgCnfHandle, CiServiceGroupID svgId, CiPrimitiveID primId, CiRequestHandle reqHandle, void* paras);
void msgInd(CiSgOpaqueHandle opSgIndHandle, CiServiceGroupID svgId, CiPrimitiveID primId, CiIndicationHandle indHandle, void* pParam);
void resetMsgParas(void);
void resetMsgOperFlag(void);
#endif

