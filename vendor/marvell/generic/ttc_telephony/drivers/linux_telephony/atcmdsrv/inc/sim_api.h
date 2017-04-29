/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: sim_api.h
 *
 *  Description: API interface for SIM service
 *
 *  History:
 *   Oct 16, 2008 - Qiang XU Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/


#ifndef __SIM_API_H__
#define __SIM_API_H__

#include "ci_sim.h"

/* Enum value of STK cmd type and STK ind msg type */
typedef enum StkCmdTypeTag
{
	STK_CMD_ENABLE_SIMAT = 0,
	STK_CMD_DOWNLOAD_PROFILE = 1,
	STK_CMD_GET_CAP_INFO = 2,
	STK_CMD_GET_PROFILE = 3,
	STK_CMD_SEND_ENVELOPE = 4,

	STK_CMD_PROACTIVE = 11,
	STK_CMD_SETUP_CALL = 12,
	STK_CMD_DISPLAY_INFO = 13,
	STK_CMD_END_SESSION = 14,
	STK_CMD_SETUP_CALL_REQ_STATUS,    //  ----- CI_SIM_PRIM_SIMAT_CC_STATUS_IND
	STK_CMD_SETUP_CALL_RESULT,   // CI_SIM_PRIM_SIMAT_SEND_CALL_SETUP_RSP_IND
	STK_CMD_PROACTIVE_REFRESH,
	STK_CMD_PROACTIVE_SEND_SM_REQ_STATUS,  //  ----- CI_SIM_PRIM_SIMAT_SM_STATUS_IND
	STK_CMD_PROACTIVE_SEND_SM_RESULT,  //  ----- CI_SIM_PRIM_SIMAT_SEND_SM_RSP_IND
	STK_CMD_PROACTIVE_SEND_USSD_RESULT,  //  ----- CI_SIM_PRIM_SIMAT_SEND_SS_USSD_RSP_IND
	STK_TYPE_INVALID,
	STK_TYPE_MAX,
	STK_TYPE_MAX_LAST= 0xFFFFFFFF
} StkCmdType;

CiReturnCode SIM_SetCommand(UINT32 atHandle, int cmdLen, CHAR *cmdStr);
CiReturnCode SIM_GetImsi(UINT32 atHandle);
CiReturnCode SIM_GetPinState(UINT32 atHandle, int cpinCmdType);
CiReturnCode SIM_EnterPin(UINT32 atHandle, UINT8 no, UINT8 oper, CHAR* oldPasswd, CHAR* newPasswd);
CiReturnCode SIM_GetSIMInserted(UINT32 atHandle, UINT32 simSlot);
CiReturnCode SIM_SetLockFacility(UINT32 atHandle, int mode, int facType, CHAR* passwd, int passwdStrLen);
//change by linda add path parameter
//CiReturnCode SIM_RestrictedAccess(UINT32 atHandle, int cmd, int iFileId, int p1, int p2, int p3, char *data, int dataLen);
CiReturnCode SIM_RestrictedAccess(UINT32 atHandle, int cmd, int iFileId, char *valStr,  int p1, int p2, int p3, char *data, int dataLen);
CiReturnCode SIM_GenericAccess(UINT32 atHandle, int cmdLen, char *cmdStr);
CiBoolean SIM_ConvertCrsmAtReply(UINT32 atHandle, CiPrimitiveID primId, const void *paras);

CiReturnCode STK_EnableProactiveCmdIndConfig(UINT32 atHandle, UINT8 enable);
CiReturnCode STK_GetSimProfile(UINT32 atHandle);
CiReturnCode STK_DownloadMeProfile(UINT32 atHandle, char *data, int dataLen);
CiReturnCode STK_SendEnvelopeCmd(UINT32 atHandle, char *data, int dataLen);
CiReturnCode STK_RespProactiveCmd(UINT32 atHandle, char *data, int dataLen);
CiReturnCode STK_RespSetupCall(UINT32 atHandle, UINT8 accept);
CiReturnCode STK_GetSimCapInfo(UINT32 atHandle);
CiReturnCode VSIM_GetVSim(UINT32 atHandle);
CiReturnCode VSIM_SetVSim(UINT32 atHandle);

CiReturnCode SIM_RunGSMAlgorithm(UINT32 atHandle, char *data, int dataLen);
CiReturnCode SIM_RunTContextAuth(UINT32 atHandle, char *rand, int randLen, char *autn, int autnLen);

void simCnf(CiSgOpaqueHandle opSgCnfHandle,     CiServiceGroupID svgId, CiPrimitiveID primId, CiRequestHandle reqHandle, void* paras);
void simInd(CiSgOpaqueHandle opSgOpaqueHandle, CiServiceGroupID svgId, CiPrimitiveID primId, CiIndicationHandle indHandle, void* pParam);
void resetSimParas(void);
enum
{
	CMD_TYPE_CPIN,
	CMD_TYPE_EPIN,
	CMD_TYPE_DETECT,
};

/* Run GSM Algorithm, 3gpp 51.011*/
#define RGA_RAND_LEN       16
#define RGA_AUTN_LEN       16
#define RGA_SRES_LEN       4
#define RGA_CK_LEN         8

typedef enum OPER_NEEDCISIMCHV_TAG {
	RESET_ACM = 1,
	SET_ACM_MAX,
	SET_CPUC,
} _NeedSimChvOper;

#define SIM_CARD_TYPE_USIM_BIT 0
#define SIM_CARD_TYPE_TEST_BIT 1
typedef enum AT_SIM_CARD_TYPE
{
	AT_SIM_CARD_TYPE_SIM = 0,
	AT_SIM_CARD_TYPE_USIM,
	AT_SIM_CARD_TYPE_TEST_SIM,
	AT_SIM_CARD_TYPE_TEST_USIM,
	AT_SIM_CARD_TYPE_UNKNOWN,
}AtSimCardType;

#endif

