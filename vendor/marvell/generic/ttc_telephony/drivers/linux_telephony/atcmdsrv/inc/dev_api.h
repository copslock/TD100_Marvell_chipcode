/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: dev_api.h
 *
 *  Description: API interface for Device service
 *
 *  History:
 *   Oct 17, 2008 - Qiang XU Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/

#ifndef __DEV_API_H__
#define __DEV_API_H__

#include "ci_dev.h"

CiReturnCode DEV_GetFunc(UINT32 atHandle, BOOL bCpConfig);
CiReturnCode DEV_SetFunc(UINT32 atHandle, int funcVal, int resetVal, BOOL isExt, INT32 IsCommFeatureConfig, INT32 CommFeatureConfig);
CiReturnCode DEV_GetDevStatus(UINT32 atHandle);
CiReturnCode DEV_SetEngineModeReportOption(UINT32 atHandle, int modeVal, int timeVal);
CiReturnCode DEV_GetSerialNumId(UINT32 atHandle);
CiReturnCode DEV_GetEngineModeInfo(UINT32 atHandle);
CiReturnCode DEV_GetSupportedBandModeReq(UINT32 atHandle);
CiReturnCode DEV_GetBandModeReq(UINT32 atHandle);
CiReturnCode DEV_SetBandModeReq(UINT32 atHandle, UINT8 networkMode, UINT8 preferredMode, UINT32 gsmMode, UINT32 umtsMode, UINT8 roamingConfig, UINT8 srvDomain);
CiReturnCode DEV_SetPOWERIND(UINT32 atHandle, UINT32 powerState);

CiReturnCode DEV_SetEngModeIndicatorOption(UINT32 atHandle, int modeVal, int timeVal);
CiReturnCode DEV_GetEngModeInfo(UINT32 atHandle);
CiReturnCode DEV_setLPNWUL(UINT32 atHandle, UINT8 *msg_data, UINT32 msg_data_len, UINT8 count, UINT8 bearer_type, UINT8 isFinalRsp);

CiReturnCode DEV_GetSoftwareVersionNumber(UINT32 atHandle);
CiReturnCode DEV_SetSoftwareVersionNumber(UINT32 atHandle, const char* sv);
CiReturnCode DEV_SetHsdpaModeReq(UINT32 atHandle, UINT8 mode, UINT8 dl_category, UINT8 ul_category, UINT8 cpc_state);
CiReturnCode DEV_GetHsdpaModeReq(UINT32 atHandle);

CiReturnCode DEV_SetNetWorkMonitorOptReq(UINT32 atHandle, CiBoolean option, CiDevNwMonitorMode mode);
CiReturnCode DEV_GetNetWorkMonitorOptReq(UINT32 atHandle);
CiReturnCode DEV_SetProtocolStatusConfigReq(UINT32 atHandle, CiBoolean option);
CiReturnCode DEV_GetProtocolStatusConfigReq(UINT32 atHandle);
CiReturnCode DEV_SetEventIndConfigReq(UINT32 atHandle, CiBoolean option);
CiReturnCode DEV_GetEventIndConfigReq(UINT32 atHandle);
CiReturnCode DEV_SetWirelessParamConfigReq(UINT32 atHandle, CiBoolean option, UINT16 interval);
CiReturnCode DEV_GetWirelessParamConfigReq(UINT32 atHandle);
CiReturnCode DEV_SetSignalingReportConfigReq(UINT32 atHandle, CiBoolean option, CiBoolean mode);
CiReturnCode DEV_GetSignalingReportConfigReq(UINT32 atHandle);

#ifdef AT_PRODUCTION_CMNDS
CiReturnCode DEV_SetTdModeTxRxReq(UINT32 atHandle, CiDevTdTxRxOption option, INT16 txRxGain, UINT16 freq);
CiReturnCode DEV_SetTdModeLoopbackReq(UINT32 atHandle, UINT32 regValue);
CiReturnCode DEV_SetGsmModeTxRxReq(UINT32 atHandle, CiDevGsmTxRxOption option, UINT8 gsmBandMode, UINT16 arfcn, UINT32 afcDac, UINT32 txRampScale, UINT32 rxGainCode);
CiReturnCode DEV_SetGsmControlInterfaceReq(UINT32 atHandle, CiDevGsmControlMode mode, UINT16 addrReg, UINT16 regValue);
#endif
#ifdef AT_CUSTOMER_HTC
CiReturnCode DEV_GetModeSwitchOptReq(UINT32 atHandle);
CiReturnCode DEV_SetModeSwitchOptReq(UINT32 atHandle, UINT8 option);
#endif
void devCnf(CiSgOpaqueHandle opSgCnfHandle,     CiServiceGroupID svgId, CiPrimitiveID primId, CiRequestHandle reqHandle, void* paras);
void devInd(CiSgOpaqueHandle opSgOpaqueHandle, CiServiceGroupID svgId, CiPrimitiveID primId, CiIndicationHandle indHandle, void* pParam);
void resetDevParas(void);
#endif

