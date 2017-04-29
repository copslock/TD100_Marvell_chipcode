/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: dev_api.c
 *
 *  Description: API interface implementation for dev service
 *
 *  History:
 *   Oct 17, 2008 - Qiang Xu, Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/
#include "ci_api_types.h"
#include "ci_api.h"
#include "telatci.h"
#include "teldef.h"
#include "telatparamdef.h"
#include "telutl.h"
#include "ci_dev.h"
#include "utlMalloc.h"
#include "teldev.h"

/************************************************************************************
 *
 * Dev related const and global variables
 *
 *************************************************************************************/
extern CiServiceHandle gAtciSvgHandle[CI_SG_NUMIDS + 1];

CiDevStatus gDeviceStatus = 0;

UINT8 g_UMTS_EM_IND_SEQ = 0;
EngModeInfo stEngModeInfo;
CiDevEngModeReportOption gEngModeRepOpt = CI_DEV_EM_OPTION_NONE;
static CiDevEngModeReportOption gStoreEngModeRepOpt = CI_DEV_EM_OPTION_NONE;
int gStoreTimeVal = -1;
int gCurrTimeVal= -1;

extern void initATCIContext(void);

#define PRI_DEV_PRIM_GET_CP_FUNC_REQ (CI_DEV_PRIM_LAST_COMMON_PRIM + 1)

typedef enum ATCI_HSDPA_MODE {
	ATCI_HSDPA_OFF_HSUPA_OFF = 0,
	ATCI_HSDPA_ON_HSUPA_ON,
	ATCI_HSDPA_ON_HSUPA_OFF,
} AtciHsdpaMode;

#define MAX_RESPBUFF_SIZE (2048 - 4)      //4 is the space for the '\r', '\n' added at the begin and end of respString in function atRespStr()
/* enum of wireless parameter type*/
typedef enum
{
	CI_DEV_WIRELSS_PARAM_TDD_TS_TSCP = 0x01,
	CI_DEV_WIRELSS_PARAM_TDD_DPCH_RSCP = 0x02,
	CI_DEV_WIRELSS_PARAM_TDD_TX_POWER = 0x08,
	CI_DEV_WIRELSS_PARAM_TDD_SCELL_PCCPH_RSCP = 0x40,
	CI_DEV_WIRELSS_PARAM_TDD_SCELL_ID = 0x42,
	CI_DEV_WIRELSS_PARAM_TDD_SCELL_PARAM_ID = 0x43,
	CI_DEV_WIRELSS_PARAM_TDD_SCELL_UARFCN = 0x44,
	CI_DEV_WIRELSS_PARAM_TDD_SCELL_UTRARSSI = 0x50,
	CI_DEV_WIRELSS_PARAM_TDD_NCELL_PARAM_LIST = 0x52,
	CI_DEV_WIRELSS_PARAM_TDD_DEDICATED_PHYCH_PARAM = 0x59,
	CI_DEV_WIRELSS_PARAM_TDD_HRNTI = 0x56,
	CI_DEV_WIRELSS_PARAM_TDD_ERNTI = 0x5B,
	CI_DEV_WIRELSS_PARAM_TDD_URNTI_CRNTI = 0x5D,
	CI_DEV_WIRELSS_PARAM_COMMON_UE_ID = 0x54,
	CI_DEV_WIRELSS_PARAM_COMMON_MM_INFO = 0x60,
	CI_DEV_WIRELSS_PARAM_GSM_SCELL_SYS_INFO = 0x80,
	CI_DEV_WIRELSS_PARAM_GSM_SCELL_RESELECTION_INFO = 0x81,
	CI_DEV_WIRELSS_PARAM_GSM_SCELL_RADIO_INFO = 0x82,
	CI_DEV_WIRELSS_PARAM_GSM_NCELL_INFO = 0x84,
	CI_DEV_WIRELSS_PARAM_GSM_DEDICATED_CHANNEL_INFO = 0x86,
	CI_DEV_WIRELSS_PARAM_GSM_DEDICATED_MEASURE_INFO = 0x88,
	CI_DEV_WIRELSS_PARAM_GSM_CI_INFO = 0x8E,
	CI_DEV_WIRELSS_PARAM_GSM_BEPCV_BEP_INFO = 0xB3,
	CI_DEV_WIRELSS_PARAM_GSM_MCS_CS_INFO = 0xB4,
	CI_DEV_WIRELSS_PARAM_GSM_TFI_TS_ALLOCATION = 0xB5,
} WIRELESS_PARAM_TYPE;
#define CI_DEV_WIRELSS_PARAM_TDD_SCELL_PCCPH_RSCP_VAL_MAX 91
#define CI_DEV_WIRELSS_PARAM_TDD_SCELL_ID_VAL_MAX 0x0FFFFFFF
#define CI_DEV_WIRELSS_PARAM_TDD_SCELL_PARAM_ID_VAL_MAX 127
#define CI_DEV_WIRELSS_PARAM_TDD_SCELL_UARFCN_VAL_MAX 16383
#define CI_DEV_WIRELSS_PARAM_TDD_SCELL_UTRARSSI_VAL_MAX 76
#define CI_DEV_WIRELSS_PARAM_GSM_SCELL_RESELECTION_INFO_VAL_MAX 2

#define CI_DEV_SIGNALING_PACKET_NON_PAYLOAD_LEN 17
#define BIT_NUM_ONE_OCTET 8
#define TEST_BIT(val, nr)	(((1UL << ((nr) & ((sizeof(val) * BIT_NUM_ONE_OCTET - 1)))) & (val)) != 0)

extern void libConvertNumToHex(char *inData, int inLen, char *outData);

static void DEV_SendIndication(char *respBuffer, UINT16 *pBufferLeft, BOOL bIsLastIndication)
{
	char *p;
	if(respBuffer == NULL || pBufferLeft == NULL)
		return;
	if((p = strchr(respBuffer, ',')) == NULL)
	{
		//Fix me: the max length of DBGMSG is 1024, the respbuffer will be truncated
		DBGMSG("not found first comma, buffer is %s\n", respBuffer);
		return;
	}
	p++;           //skip the comma
	if(bIsLastIndication && *p == '1')
	{
		*p = '0';
		ATRESP(IND_REQ_HANDLE, 0, 0, respBuffer);
		return;
	}
	ATRESP(IND_REQ_HANDLE, 0, 0, respBuffer);
	if(*p++ == '\0' || *p != ',')
	{
		DBGMSG("not found second comma, buffer is %s\n", respBuffer);
		return;
	}
	*p = '\0';
	*pBufferLeft = MAX_RESPBUFF_SIZE - strlen(respBuffer) - 1;
}
static void DEV_ProcessWirelessParam(UINT8 paramType, const UINT8 *pParam, UINT16 paramLen, char *respBuffer, UINT16 *pBufferLeft)
{
	UINT16 infoState;
	UINT16 storedParamLen = paramLen;
	char tempBuffer[10];                     //paramType(2) + infoState(4) + three comma(3) + '\0'(1)
	if(pParam == NULL || respBuffer == NULL || pBufferLeft == NULL)
		return;
	//paramType(2) + infoState(4) + three comma(3) + mininum parameter data(2)
	if(*pBufferLeft < 11)
	{
		DEV_SendIndication(respBuffer, pBufferLeft, FALSE);
	}
	ASSERT(*pBufferLeft >= 11);
	do{
		//paramType(2) + infoState(4) + three comma(3)
		if(paramLen * 2 <= *pBufferLeft - 9)
		{
			infoState = storedParamLen - paramLen;
			*pBufferLeft -= snprintf(tempBuffer, utlNumberOf(tempBuffer), ",%02X,%04X,", paramType, infoState);
			strcat(respBuffer, tempBuffer);
			*pBufferLeft -= paramLen * 2 ;
			libConvertNumToHex((char *)pParam, paramLen, respBuffer);
			paramLen = 0;
		}
		else
		{
			infoState = storedParamLen - paramLen;
			infoState = infoState | (1UL << (sizeof(infoState) * BIT_NUM_ONE_OCTET - 1));
			*pBufferLeft -= snprintf(tempBuffer, utlNumberOf(tempBuffer), ",%02X,%04X,", paramType, infoState);
			strcat(respBuffer, tempBuffer);
			paramLen -= *pBufferLeft / 2;
			libConvertNumToHex((char *)pParam, *pBufferLeft / 2, respBuffer);
			DEV_SendIndication(respBuffer, pBufferLeft, FALSE);
		}
	}while(paramLen > 0);
}
static BOOL DEV_GetTimeStamp(unsigned long *secAfterBoot)
{
	FILE *fp;
	unsigned long sec, usec;
	char line[1024];
	if(secAfterBoot == NULL)
		return FALSE;
	if((fp = fopen("/proc/uptime", "r")) == NULL)
		return FALSE;
	if((fgets(line, sizeof(line), fp)) == NULL)
	{
		fclose(fp);
		return FALSE;
	}
	sscanf(line, "%lu %lu", &sec, &usec);
	*secAfterBoot = sec;
	fclose(fp);
	return TRUE;
}
static BOOL DEV_GetSystemTick(UINT32 *systemTick)
{
	FILE *fp;
	char line[1024];
	char matchString[] = "jiffies:";
	int len = strlen(matchString);
	if(systemTick == NULL)
		return FALSE;
	if((fp = fopen("/proc/timer_list", "r")) == NULL)
		return FALSE;
	while(fgets(line, sizeof(line), fp) != NULL)
	{
		if(strncmp(line, matchString, len) == 0)
		{
			*systemTick = (UINT32)strtoul(line + len, NULL, 10);
			fclose(fp);
			return TRUE;
		}
	}
	fclose(fp);
	return FALSE;
}

/************************************************************************************
 *
 * Implementation of Dev related AT commands by sending Request to CCI.
 *
 *************************************************************************************/
/************************************************************************************
 * F@: ConvertHsdpaMode2Ci - Convert HSDPA Mode from AT to CCI enum
 *
 */
static BOOL libConvertHsdpaMode2Ci(UINT8 mode, CiDevHspaConfig *hspaCnf )
{
	BOOL ret = TRUE;
	DBGMSG("%s mode = %d.\n", __FUNCTION__, mode);

	switch ( mode )
	{
	case ATCI_HSDPA_OFF_HSUPA_OFF:
		*hspaCnf = CI_DEV_HSDPA_OFF_HSUPA_OFF;
		break;
	case ATCI_HSDPA_ON_HSUPA_ON:
		*hspaCnf = CI_DEV_HSDPA_ON_HSUPA_ON;
		break;
	case ATCI_HSDPA_ON_HSUPA_OFF:
		*hspaCnf = CI_DEV_HSDPA_ON_HSUPA_OFF;
		break;
	default:
		ret = FALSE;
		break;
	}
	return ret;
}
/************************************************************************************
 * F@: ConvertHsdpaMode2At - Convert HSDPA Mode from CCI enum to AT
 *
 */
static BOOL libConvertHsdpaMode2At(CiDevHspaConfig hspaCnf, UINT8 *mode)
{
	BOOL ret = TRUE;
	DBGMSG("%s hspaCnf = %d.\n", __FUNCTION__, hspaCnf);

	switch ( hspaCnf )
	{
	case CI_DEV_HSDPA_OFF_HSUPA_OFF:
		*mode = ATCI_HSDPA_OFF_HSUPA_OFF;
		break;
	case CI_DEV_HSDPA_ON_HSUPA_OFF:
		*mode = ATCI_HSDPA_ON_HSUPA_OFF;
		break;
	case CI_DEV_HSDPA_ON_HSUPA_ON:
		*mode = ATCI_HSDPA_ON_HSUPA_ON;
		break;
	default:
		ret = FALSE;
		break;
	}
	return ret;
}

/****************************************************************************************
*  FUNCTION:  DEV_GetCD
*
*  PARAMETERS:
*
*  DESCRIPTION: Caculate the CD value from 14 digital imel numbers
*
*  RETURNS: CHAR
*
****************************************************************************************/
CHAR DEV_GetCD(CHAR * imei)
{
	int num = 0;
	int sum = 0;
	int CD = 0;
	int digNum = 0;
	if(imei == NULL)
		return 0;

	while(*imei)
	{
		if( *imei >'9' || *imei <'0')
			return 0;
		digNum = *imei - '0';
		if(num % 2)
			digNum *= 2;
		if(digNum >= 10)
			digNum = digNum/10 + digNum%10;
		sum += digNum;
		num++;
		imei++;
	}
	if(sum % 10)
		CD = 10 - sum%10;
	else
		CD = 0;

	return '0'+CD;
}
/****************************************************************************************
*  FUNCTION:  DEV_GetFunc
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get current phone functionality status, implementation of AT+CFUN?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_GetFunc(UINT32 atHandle, BOOL bCpConfig)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimGetFuncReq   *getFuncReq = NULL;
	UINT32 reqHandle;
	if(bCpConfig)
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, PRI_DEV_PRIM_GET_CP_FUNC_REQ);
	else
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_GET_FUNC_REQ);
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_GET_FUNC_REQ,
			 reqHandle, (void *)getFuncReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  DEV_SetFunc
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to set phone functionality status, implementation of AT+CFUN=/AT*CFUN=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_SetFunc(UINT32 atHandle, int funcVal, int resetVal, BOOL isExt, INT32 IsCommFeatureConfig, INT32 CommFeatureConfig)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimSetFuncReq   *setFuncReq;

	setFuncReq = utlCalloc(1, sizeof(CiDevPrimSetFuncReq));
	if (setFuncReq == NULL)
		return CIRC_FAIL;

	if ((resetVal != 0) && (resetVal != 1))
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		utlFree(setFuncReq);
		return ret;
	}

	/* if resetVal = FALSE => power function is not actually executed right away by the protocol stack */
	/* if resetVal = TRUE => power function is executed right away by the prtotocol stack */
	if (resetVal == 0)
	{
		setFuncReq->reset = FALSE;
	}
	else if (resetVal == 1)
	{
		setFuncReq->reset = TRUE;
	}

	if(isExt)
	{
		if (IsCommFeatureConfig == 1)
			setFuncReq->IsCommFeatureConfig = TRUE;
		else
			setFuncReq->IsCommFeatureConfig = FALSE;

		setFuncReq->CommFeatureConfig = CommFeatureConfig;
	}

	/* take action based on requested power function */
	setFuncReq->func = funcVal;

	/* Send the CI Request. */
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_SET_FUNC_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, (UINT8)funcVal), (void *)setFuncReq );

	return ret;
}


/****************************************************************************************
*  FUNCTION:  DEV_GetDevStatus
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get current activity status, implementation of AT+CPAS
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_GetDevStatus(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_SUCCESS;
	char AtReplyBuf[100];

	sprintf(AtReplyBuf, "+CPAS: %d", gDeviceStatus);
	ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, AtReplyBuf);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  DEV_SetEngineModeReportOption
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to set engine mode report option, implementation of AT+CGED=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_SetEngineModeReportOption(UINT32 atHandle, int modeVal, int timeVal)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimSetEngmodeRepOptReq   *setEngmodeRepOptReq;

	setEngmodeRepOptReq = utlCalloc(1, sizeof(CiDevPrimSetEngmodeRepOptReq));
	if (setEngmodeRepOptReq == NULL)
		return CIRC_FAIL;

	if (modeVal == 0) //export the engineering information once
	{
		setEngmodeRepOptReq->type = CI_DEV_EM_OPTION_REQUEST;
	}
	else if (modeVal == 2) // stop the reporting the engineering information
	{
		setEngmodeRepOptReq->type = CI_DEV_EM_OPTION_NONE;
	}
	else // export the engineering information periodically
	{
		setEngmodeRepOptReq->type = CI_DEV_EM_OPTION_PERIODIC;
		setEngmodeRepOptReq->interval = timeVal;
	}

	/* Send the request */
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_SET_ENGMODE_REPORT_OPTION_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_SET_ENGMODE_REPORT_OPTION_REQ), (void *)setEngmodeRepOptReq );

	gEngModeRepOpt = setEngmodeRepOptReq->type;

	return ret;
}

/****************************************************************************************
*  FUNCTION:  DEV_GetSerialNumId
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get serial number ID, implementation of AT+CGSN, +CGSN?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_GetSerialNumId(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimGetSerialNumIdReq   *getCgsnReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_GET_SERIALNUM_ID_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_GET_SERIALNUM_ID_REQ), (void *)getCgsnReq);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  DEV_GetSupportedBandModeReq
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get supported band mode, implementation of AT*BAND=?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_GetSupportedBandModeReq(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_GET_SUPPORTED_BAND_MODE_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_GET_SUPPORTED_BAND_MODE_REQ), NULL);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  DEV_GetBandModeReq
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get band mode, implementation of AT*BAND?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_GetBandModeReq(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_GET_BAND_MODE_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_GET_BAND_MODE_REQ), NULL);

	return ret;
}


/****************************************************************************************
*  FUNCTION:  DEV_SetBandModeReq
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to set band mode, implementation of AT*BAND=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_SetBandModeReq(UINT32 atHandle, UINT8 networkMode, UINT8 preferredMode,  UINT32 gsmMode, UINT32 umtsMode, UINT8 roamingConfig, UINT8 srvDomain)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimSetBandModeReq                         *setBandModeReq;

	setBandModeReq = (CiDevPrimSetBandModeReq *)utlCalloc(1, sizeof(CiDevPrimSetBandModeReq));
	if (setBandModeReq == NULL)
		return CIRC_FAIL;

	setBandModeReq->networkMode = networkMode;
	setBandModeReq->preferredMode = preferredMode;
	setBandModeReq->GSMBandMode = gsmMode;
	setBandModeReq->UMTSBandMode = umtsMode;
	setBandModeReq->roamingConfig = roamingConfig;
	setBandModeReq->srvDomain = srvDomain;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_SET_BAND_MODE_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_SET_BAND_MODE_REQ), (void*)setBandModeReq);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  DEV_GetEngineModeInfo
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get engine mode info, implementation of AT+CGED=0  (mode type = CI_DEV_EM_OPTION_REQUEST)
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_GetEngineModeInfo(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimGetEngmodeInfoReq   *getEngmodeInfoReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_GET_ENGMODE_INFO_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_GET_ENGMODE_INFO_REQ), (void *)getEngmodeInfoReq);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  DEV_SetEngModeIndicatorOption
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to set engine mode report option, implementation of AT+EEMOPT=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode
DEV_SetEngModeIndicatorOption(UINT32 atHandle, int modeVal, int timeVal)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimSetEngmodeRepOptReq   *setEngmodeRepOptReq;

	setEngmodeRepOptReq = utlCalloc(1, sizeof(CiDevPrimSetEngmodeRepOptReq));
	if (setEngmodeRepOptReq == NULL)
		return CIRC_FAIL;

	switch (modeVal)
	{
	case 0: // turn off indicator
		setEngmodeRepOptReq->type = CI_DEV_EM_OPTION_NONE;
		break;
	case 1: // set to query mode
		setEngmodeRepOptReq->type = CI_DEV_EM_OPTION_REQUEST;
		break;
	case 2: // set to periodic mode
		setEngmodeRepOptReq->type = CI_DEV_EM_OPTION_PERIODIC;
		setEngmodeRepOptReq->interval = timeVal;
		gCurrTimeVal = timeVal;
		break;
	case 3: // snapshot mode, old <timeVal> is stored as snapshot
		// current type and value.
		setEngmodeRepOptReq->type = gEngModeRepOpt;
		setEngmodeRepOptReq->interval = gCurrTimeVal;

		// store current type and value.
		gStoreEngModeRepOpt = gEngModeRepOpt;
		gStoreTimeVal = gCurrTimeVal;
		break;
	case 4: // restore to snapshot <timeVal>
		setEngmodeRepOptReq->type = gStoreEngModeRepOpt;
		setEngmodeRepOptReq->interval = gStoreTimeVal;
		gCurrTimeVal = gStoreTimeVal;
		break;
	default:
		break;
	}

	gEngModeRepOpt = setEngmodeRepOptReq->type;

	/* Send the request */
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_SET_ENGMODE_REPORT_OPTION_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_SET_ENGMODE_REPORT_OPTION_REQ), (void *)setEngmodeRepOptReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  DEV_GetEngModeInfo
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get engine mode info, implementation of AT+EEMGINFO? 
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_GetEngModeInfo(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimGetEngmodeInfoReq   *getEngmodeInfoReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_GET_ENGMODE_INFO_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_GET_ENGMODE_INFO_REQ), (void *)getEngmodeInfoReq);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  DEV_setLPNWUL
*
*  PARAMETERS:
*
*  DESCRIPTION: Delivers measurements and positioning reports and/or status from A-GPS
*               client to PS/NW
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_setLPNWUL(UINT32 atHandle, UINT8 *msg_data, UINT32 msg_data_len, UINT8 count, UINT8 bearer_type, UINT8 isFinalRsp)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimLpNwulMsgReq *setNwulMsgReq;
	int len;
	extern int libConvertHexToNum(char *inData, int inLen, char *outData);
	setNwulMsgReq = utlCalloc(1, sizeof(CiDevPrimLpNwulMsgReq));
	if (setNwulMsgReq == NULL)
		return CIRC_FAIL;

	/* msg_data_len is the total bytes, even if this segment is padding */
	if (msg_data_len >= ((count + 1U) * CI_DEV_MAX_APGS_MSG_SIZE))
	{
		len = CI_DEV_MAX_APGS_MSG_SIZE;
	}
	else
	{
		len = msg_data_len % CI_DEV_MAX_APGS_MSG_SIZE;
	}
	libConvertHexToNum((char *)msg_data, len * 2, (char *)setNwulMsgReq->msg_data);
	setNwulMsgReq->msg_data_len = msg_data_len;
	setNwulMsgReq->count = count;
	setNwulMsgReq->bearer_type = bearer_type;
	setNwulMsgReq->isFinalResponse = isFinalRsp;

#if 0 // TEST_CPLANE
	{
		void processLpNwdlMsgInfoInd(const void *pParam);
		CiDevPrimLpNwdlMsgInd *nwdlMsgInd;

		nwdlMsgInd = utlCalloc(1, sizeof(CiDevPrimLpNwdlMsgInd));
		nwdlMsgInd->BearerType = bearer_type;
		nwdlMsgInd->count = count;
		memcpy(nwdlMsgInd->msg_data, setNwulMsgReq->msg_data, len);
		nwdlMsgInd->msg_size = len;
		nwdlMsgInd->RrcState = CI_DEV_LP_RRC_STATE_CELL_DCH;
		nwdlMsgInd->sessionType = 0;

		DPRINTF("%s, count %d, %d\r\n", __FUNCTION__, count, nwdlMsgInd->count);
		processLpNwdlMsgInfoInd(nwdlMsgInd);

		utlFree(nwdlMsgInd);
	}
#endif
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_LP_NWUL_MSG_REQ,
                         MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_LP_NWUL_MSG_REQ), (void *)setNwulMsgReq);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  DEV_GetSoftwareVersionNumber
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get SV of IMEISV, implementation of AT*CGSN?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_GetSoftwareVersionNumber(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimGetSvReq   *getSvReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_GET_SV_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_GET_SV_REQ), (void *)getSvReq);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  DEV_SetSoftwareVersionNumber
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to set SV of IMEISV, implementation of AT*CGSN=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_SetSoftwareVersionNumber(UINT32 atHandle, const char* sv)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimSetSvReq   *setSvReq;

	setSvReq = utlCalloc(1, sizeof(CiDevPrimSetSvReq));
	if (setSvReq == NULL)
		return CIRC_FAIL;

	setSvReq->SVDigits.len = strlen(sv);
	strcpy((char *)setSvReq->SVDigits.val, sv);

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_SET_SV_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_SET_SV_REQ), (void *)setSvReq);

	return ret;
}
/****************************************************************************************
*  FUNCTION:  DEV_SetHsdpaModeReq
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to enable hsdpa, implementation of AT*EHSDPA=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_SetHsdpaModeReq(UINT32 atHandle, UINT8 mode, UINT8 dl_category, UINT8 ul_category, UINT8 cpc_state)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimEnableHsdpaReq 	*enableHsdpaReq;
	CiDevHspaConfig hsdpaCnf;

	enableHsdpaReq = utlCalloc(1, sizeof(CiDevPrimEnableHsdpaReq));
	if (enableHsdpaReq == NULL)
		return CIRC_FAIL;

	if(libConvertHsdpaMode2Ci(mode, &hsdpaCnf))
	{
		enableHsdpaReq->hspaConfig = hsdpaCnf;
		enableHsdpaReq->dlCategory = dl_category;
		enableHsdpaReq->ulCategory = ul_category;
		enableHsdpaReq->cpcState = cpc_state;
		ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_ENABLE_HSDPA_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_ENABLE_HSDPA_REQ), (void *)enableHsdpaReq);
	}
	else
	{
		utlFree(enableHsdpaReq);
	}
	return ret;
}
/****************************************************************************************
*  FUNCTION:  DEV_GetHsdpaModeReq
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get hsdpa mode, implementation of AT*EHSDPA?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_GetHsdpaModeReq(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimGetHsdpaStatusReq 		*getHsdpaStatusReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_GET_HSDPA_STATUS_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_GET_HSDPA_STATUS_REQ), (void *)getHsdpaStatusReq);

	return ret;
}

#ifdef AT_PRODUCTION_CMNDS
/****************************************************************************************
*  FUNCTION:  DEV_SetTdModeTxRxReq
*
*  PARAMETERS:
*
*  DESCRIPTION: Request to set Tx or Rx on TD for radio testing
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_SetTdModeTxRxReq(UINT32 atHandle, CiDevTdTxRxOption option, INT16 txRxGain, UINT16 freq)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimSetTdModeTxRxReq                   *setTdModeTxRxReq;

	setTdModeTxRxReq = (CiDevPrimSetTdModeTxRxReq *)utlCalloc(1, sizeof(CiDevPrimSetTdModeTxRxReq));
	if (setTdModeTxRxReq == NULL)
		return CIRC_FAIL;

	setTdModeTxRxReq->option = option;
	setTdModeTxRxReq->txRxGain = txRxGain;
	setTdModeTxRxReq->freq = freq;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_SET_TD_MODE_TX_RX_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_SET_TD_MODE_TX_RX_REQ), (void*)setTdModeTxRxReq);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  DEV_SetTdModeLoopbackReq
*
*  PARAMETERS:
*
*  DESCRIPTION: Request to set loopback mode on TD for radio testing
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_SetTdModeLoopbackReq(UINT32 atHandle, UINT32 regValue)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimSetTdModeLoopbackReq               *setTdModeLoopbackReq;

	setTdModeLoopbackReq = (CiDevPrimSetTdModeLoopbackReq *)utlCalloc(1, sizeof(CiDevPrimSetTdModeLoopbackReq));
	if (setTdModeLoopbackReq == NULL)
		return CIRC_FAIL;

	setTdModeLoopbackReq->regValue = regValue;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_SET_TD_MODE_LOOPBACK_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_SET_TD_MODE_LOOPBACK_REQ), (void*)setTdModeLoopbackReq);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  DEV_SetGsmModeTxRxReq
*
*  PARAMETERS:
*
*  DESCRIPTION: Request to set Tx/Rx on GSM for radio tesing
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_SetGsmModeTxRxReq(UINT32 atHandle, CiDevGsmTxRxOption option, UINT8 gsmBandMode, UINT16 arfcn, UINT32 afcDac, UINT32 txRampScale, UINT32 rxGainCode)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimSetGsmModeTxRxReq                  *setGsmModeTxRxReq;

	setGsmModeTxRxReq = (CiDevPrimSetGsmModeTxRxReq *)utlCalloc(1, sizeof(CiDevPrimSetGsmModeTxRxReq));
	if (setGsmModeTxRxReq == NULL)
		return CIRC_FAIL;

	setGsmModeTxRxReq->option = option;
	setGsmModeTxRxReq->gsmBandMode = gsmBandMode;
	setGsmModeTxRxReq->arfcn = arfcn;
	setGsmModeTxRxReq->afcDac = afcDac;
	setGsmModeTxRxReq->txRampScale = txRampScale;
	setGsmModeTxRxReq->rxGainCode = rxGainCode;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_SET_GSM_MODE_TX_RX_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_SET_GSM_MODE_TX_RX_REQ), (void*)setGsmModeTxRxReq);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  DEV_SetGsmControlInterfaceReq
*
*  PARAMETERS:
*
*  DESCRIPTION: Request to set GSM for control interface
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_SetGsmControlInterfaceReq(UINT32 atHandle, CiDevGsmControlMode mode, UINT16 addrReg, UINT16 regValue)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimSetGsmControlInterfaceReq          *setGsmControlInterfaceReq;

	setGsmControlInterfaceReq = (CiDevPrimSetGsmControlInterfaceReq *)utlCalloc(1, sizeof(CiDevPrimSetGsmControlInterfaceReq));
	if (setGsmControlInterfaceReq == NULL)
		return CIRC_FAIL;

	setGsmControlInterfaceReq->mode = mode;
	setGsmControlInterfaceReq->addrReg = addrReg;
	setGsmControlInterfaceReq->regValue = regValue;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_SET_GSM_CONTROL_INTERFACE_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_SET_GSM_CONTROL_INTERFACE_REQ), (void*)setGsmControlInterfaceReq);

	return ret;
}

#endif
#ifdef AT_CUSTOMER_HTC
/****************************************************************************************
*  FUNCTION:  DEV_GetModeSwitchOptReq
*
*  PARAMETERS:
*
*  DESCRIPTION: Request to get mode switch option, implementation of AT+TPCN?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_GetModeSwitchOptReq(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimGetModeSwitchOptReq          *getModeSwitchOptReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_GET_MODE_SWITCH_OPTION_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_GET_MODE_SWITCH_OPTION_REQ), (void*)getModeSwitchOptReq);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  DEV_SetModeSwitchOptReq
*
*  PARAMETERS:
*
*  DESCRIPTION: Request to set mode switch option, implementation of AT+TPCN=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_SetModeSwitchOptReq(UINT32 atHandle, UINT8 option)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimSetModeSwitchOptReq          *setModeSwitchOptReq;

	setModeSwitchOptReq = utlCalloc(1, sizeof(CiDevPrimSetModeSwitchOptReq));
	if (setModeSwitchOptReq == NULL)
		return CIRC_FAIL;
	setModeSwitchOptReq->option = option;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_SET_MODE_SWITCH_OPTION_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_SET_MODE_SWITCH_OPTION_REQ), (void*)setModeSwitchOptReq);

	return ret;
}
#endif

/****************************************************************************************
*  FUNCTION:  DEV_SetNetWorkMonitorOptReq
*
*  PARAMETERS:
*
*  DESCRIPTION: Request to mode switch of the MT between normal mode and test mode, implementation of AT^DCTS=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_SetNetWorkMonitorOptReq(UINT32 atHandle, CiBoolean option, CiDevNwMonitorMode mode)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimSetNetworkMonitorOptReq          *setNetWorkMonitorOptReq;

	setNetWorkMonitorOptReq = (CiDevPrimSetNetworkMonitorOptReq  *)utlCalloc(1, sizeof(CiDevPrimSetNetworkMonitorOptReq));
	if (setNetWorkMonitorOptReq == NULL)
		return CIRC_FAIL;

	setNetWorkMonitorOptReq->Option= option;
	setNetWorkMonitorOptReq->Mode= mode;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_SET_NETWORK_MONITOR_OPTION_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_SET_NETWORK_MONITOR_OPTION_REQ), (void*)setNetWorkMonitorOptReq);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  DEV_GetNetWorkMonitorOptReq
*
*  PARAMETERS:
*
*  DESCRIPTION: Request to get mode of the MT, implementation of AT^DCTS?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_GetNetWorkMonitorOptReq(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimGetNetworkMonitorOptReq          *getNetWorkMonitorOptReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_GET_NETWORK_MONITOR_OPTION_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_GET_NETWORK_MONITOR_OPTION_REQ), (void*)getNetWorkMonitorOptReq);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  DEV_SetProtocolStatusConfigReq
*
*  PARAMETERS:
*
*  DESCRIPTION: Request to enable/disable the presentation of protocol status, implementation of AT^DEELS=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_SetProtocolStatusConfigReq(UINT32 atHandle, CiBoolean option)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimSetProtocolStatusConfigReq          *setProtocolStatusConfigReq;

	setProtocolStatusConfigReq = (CiDevPrimSetProtocolStatusConfigReq  *)utlCalloc(1, sizeof(CiDevPrimSetProtocolStatusConfigReq));
	if (setProtocolStatusConfigReq == NULL)
		return CIRC_FAIL;

	setProtocolStatusConfigReq->Option= option;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_SET_PROTOCOL_STATUS_CONFIG_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_SET_PROTOCOL_STATUS_CONFIG_REQ), (void*)setProtocolStatusConfigReq);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  DEV_GetProtocolStatusConfigReq
*
*  PARAMETERS:
*
*  DESCRIPTION: Request to get protocol status presentation configuration, implementation of AT^DEELS?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_GetProtocolStatusConfigReq(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimGetProtocolStatusConfigReq          *getProtocolStatusConfigReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_GET_PROTOCOL_STATUS_CONFIG_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_GET_PROTOCOL_STATUS_CONFIG_REQ), (void*)getProtocolStatusConfigReq);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  DEV_SetEventIndConfigReq
*
*  PARAMETERS:
*
*  DESCRIPTION: Request to enable/disable the presentation of events indication during the MMI cell test mode, implementation of AT^DEVEI=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_SetEventIndConfigReq(UINT32 atHandle, CiBoolean option)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimSetEventIndConfigReq          *setEventIndConfigReq;

	setEventIndConfigReq = (CiDevPrimSetEventIndConfigReq  *)utlCalloc(1, sizeof(CiDevPrimSetEventIndConfigReq));
	if (setEventIndConfigReq == NULL)
		return CIRC_FAIL;

	setEventIndConfigReq->Option= option;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_SET_EVENT_IND_CONFIG_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_SET_EVENT_IND_CONFIG_REQ), (void*)setEventIndConfigReq);

	return ret;
}
/****************************************************************************************
*  FUNCTION:  DEV_GetEventIndConfigReq
*
*  PARAMETERS:
*
*  DESCRIPTION: Request to get presentation of events indication configuration during the MMI cell test mode, implementation of AT^DEVEI?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_GetEventIndConfigReq(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimGetEventIndConfigReq          *getEventIndConfigReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_GET_EVENT_IND_CONFIG_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_GET_EVENT_IND_CONFIG_REQ), (void*)getEventIndConfigReq);

	return ret;
}
/****************************************************************************************
*  FUNCTION:  DEV_SetWirelessParamConfigReq
*
*  PARAMETERS:
*
*  DESCRIPTION: Request to enable/disable the presentation and report interval of wireless parameter indication, implementation of AT^DNPR=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_SetWirelessParamConfigReq(UINT32 atHandle, CiBoolean option, UINT16 interval)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimSetWirelessParamConfigReq          *setWirelessParamConfigReq;
	setWirelessParamConfigReq = (CiDevPrimSetWirelessParamConfigReq  *)utlCalloc(1, sizeof(CiDevPrimSetWirelessParamConfigReq));

	if (setWirelessParamConfigReq == NULL)
		return CIRC_FAIL;

	setWirelessParamConfigReq->Option= option;
	setWirelessParamConfigReq->Interval = interval;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_SET_WIRELESS_PARAM_CONFIG_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_SET_WIRELESS_PARAM_CONFIG_REQ), (void*)setWirelessParamConfigReq);

	return ret;
}
/****************************************************************************************
*  FUNCTION:  DEV_GetWirelessParamConfigReq
*
*  PARAMETERS:
*
*  DESCRIPTION: Request to get presentation of wireless parameter indication configuration, implementation of AT^DNPR?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_GetWirelessParamConfigReq(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimGetWirelessParamConfigReq          *getWirelessParamConfigReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_GET_WIRELESS_PARAM_CONFIG_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_GET_WIRELESS_PARAM_CONFIG_REQ), (void*)getWirelessParamConfigReq);

	return ret;
}
/****************************************************************************************
*  FUNCTION:  DEV_SetSignalingReportConfigReq
*
*  PARAMETERS:
*
*  DESCRIPTION: Request to enable/disable the presentation of Signaling indication, implementation of AT^DUSR=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_SetSignalingReportConfigReq(UINT32 atHandle, CiBoolean option, CiBoolean mode)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimSetSignalingReportConfigReq          *setSignalingReportConfigReq;

	setSignalingReportConfigReq = (CiDevPrimSetSignalingReportConfigReq  *)utlCalloc(1, sizeof(CiDevPrimSetSignalingReportConfigReq));
	if (setSignalingReportConfigReq == NULL)
		return CIRC_FAIL;

	setSignalingReportConfigReq->Option= option;
	setSignalingReportConfigReq->Mode= mode;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_SET_SIGNALING_REPORT_CONFIG_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_SET_SIGNALING_REPORT_CONFIG_REQ), (void*)setSignalingReportConfigReq);

	return ret;
}
/****************************************************************************************
*  FUNCTION:  DEV_GetSignalingReportConfigReq
*
*  PARAMETERS:
*
*  DESCRIPTION: Request to get presentation of Signaling indication configuration, implementation of AT^DUSR?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_GetSignalingReportConfigReq(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimGetSignalingReportConfigReq          *getSignalingReportConfigReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_GET_SIGNALING_REPORT_CONFIG_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_GET_SIGNALING_REPORT_CONFIG_REQ), (void*)getSignalingReportConfigReq);

	return ret;
}

/************************************************************************************
 *
 * Implementation of processing reply or indication of Dev related AT commands. Called by devCnf() or devInd
 *
 *************************************************************************************/
/****************************************************************
 *  F@: processGetSerialNumIdConf - Process CI cnf msg of AT+CGSN?
 */
static void processGetSerialNumIdConf(UINT32 atHandle, const void *paras)
{
	CiDevPrimGetSerialNumIdCnf *getCgsnCnf = (CiDevPrimGetSerialNumIdCnf *)paras;
	int len;
	CHAR imei[18];          /* 15(IMEI) + 2(SV) + 1(\0) */
	CHAR resImei[50];       /* large enough for IMEI response */

	switch (getCgsnCnf->rc)
	{
	case CIRC_DEV_SUCCESS:
		if (getCgsnCnf->serialNumStr.len > 17)
			len = 17;
		else
			len = getCgsnCnf->serialNumStr.len;
		memcpy(imei, getCgsnCnf->serialNumStr.valStr, len);
		imei[len] = '\0';               /* in case IMEI not end with \0 */
		if(len > 14)
			sprintf((char *)resImei, "%s", imei);
		else
			sprintf((char *)resImei, "%s%c", imei, DEV_GetCD(imei));
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)resImei);
		break;
	case CIRC_DEV_FAILURE:
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_PHONE_FAILURE, NULL);
		break;
	case CIRC_DEV_NO_CONNECTION:
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_NO_CONNECTION, NULL);
		break;
	case CIRC_DEV_UNKNOWN:
	default:
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		break;
	}


	return;
}

/****************************************************************
 *  F@: processGetFuncConf - Process CI cnf msg of AT+CFUN?
 */
static void processGetFuncConf(UINT32 reqHandle, const void *paras)
{
	char cfunBuf[50];
	CiDevPrimGetFuncCnf *getFuncCnf;

	getFuncCnf = (CiDevPrimGetFuncCnf *)paras;
	switch (getFuncCnf->rc)
	{
	case CIRC_DEV_SUCCESS:
		if ( GET_REQ_ID(reqHandle) == PRI_DEV_PRIM_GET_CP_FUNC_REQ)
			sprintf(cfunBuf, "*CFUN : %d, %d", getFuncCnf->func, getFuncCnf->CommFeatureConfig);
		else
		{
			if(getFuncCnf->func == ATCI_CFUN_OP_DISABLE_SIM)
				sprintf(cfunBuf, "+CFUN: %d", ATCI_PLUS_CFUN_OP_DISABLE_SIM);
			else
				sprintf(cfunBuf, "+CFUN: %d", getFuncCnf->func);
		}
		ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, cfunBuf);
		break;
	case CIRC_DEV_FAILURE:
		ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_PHONE_FAILURE, NULL);
		break;
	case CIRC_DEV_NO_CONNECTION:
		ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_NO_CONNECTION, NULL);
		break;
	case CIRC_DEV_UNKNOWN:
	default:
		ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		break;
	}
	return;
}

/****************************************************************
 *  F@: processSetFuncConf - Process CI cnf msg of AT+CFUN=
 */
static void processSetFuncConf(UINT32 reqHandle, const void *paras)
{
	CiDevPrimSetFuncCnf  *setFuncCnf = (CiDevPrimSetFuncCnf *)paras;
	UINT8 funcVal = GET_REQ_ID(reqHandle);

	DBGMSG("[%s]:line(%d), received CI_DEV_PRIM_SET_FUNC_CNF .\n", __FUNCTION__, __LINE__);
	DBGMSG("[%s]:line(%d), setFuncCnf->rc(%d).\n", __FUNCTION__, __LINE__, setFuncCnf->rc );
	switch ( setFuncCnf->rc )
	{
	case CIRC_DEV_SUCCESS:
		if ((funcVal == ATCI_CFUN_OP_MIN_FUNC) || (funcVal == ATCI_CFUN_OP_DISABLE_RF_RXTX)
			|| (funcVal == ATCI_CFUN_OP_MIN_FUNC_NO_IMSI_DETACH) || (funcVal == ATCI_CFUN_OP_DISABLE_SIM))
		{
			/* init the ATCI context variables */
			initATCIContext();
		}
		ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	case CIRC_DEV_FAILURE:
		ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_PHONE_FAILURE, NULL);
		break;
	case CIRC_DEV_NO_CONNECTION:
		ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_NO_CONNECTION, NULL);
		break;
	case CIRC_DEV_UNKNOWN:
	default:
		ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		break;
	}
	return;
}

#if 0
/****************************************************************
 *  F@: processSetEngineModeReportOptionConf - Process CI cnf msg of AT+CGED=
 */
static void processSetEngineModeReportOptionConf(UINT32 atHandle, const void *paras)
{
	CiDevPrimSetEngmodeRepOptCnf* setEngmodeRepOptCnf = (CiDevPrimSetEngmodeRepOptCnf *)paras;

	if (setEngmodeRepOptCnf->rc == CIRC_DEV_SUCCESS)
	{
		if (gEngModeRepOpt == CI_DEV_EM_OPTION_REQUEST)
		{
			DEV_GetEngineModeInfo(atHandle);
		}
		else
		{
			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
	}
	return;
}
#endif
/****************************************************************
 *	F@: processSetEngModeIndicatorOptionConf - Process CI cnf msg of AT+EEMOPT=
 */
static void processSetEngModeReportOptionConf(UINT32 atHandle, const void *paras)
{
	CiDevPrimSetEngmodeRepOptCnf* setEngmodeRepOptCnf = (CiDevPrimSetEngmodeRepOptCnf *)paras;

	if (setEngmodeRepOptCnf->rc == CIRC_DEV_SUCCESS)
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
	}
	return;
}
#if 0
/****************************************************************
 *  F@: processGetEngineModeInfoConf - Process CI cnf msg of AT+CGED=
 */
static void processGetEngineModeInfoConf(UINT32 atHandle, const void *paras)
{
	CiDevPrimGetEngmodeInfoCnf* getEngModeInfoCnf = (CiDevPrimGetEngmodeInfoCnf *)paras;

	DBGMSG("%s: CI_DEV_PRIM_GET_ENGMODE_INFO_CNF rc: %d, network: %d\n", getEngModeInfoCnf->rc, getEngModeInfoCnf->network);
	if (getEngModeInfoCnf->rc == CIRC_DEV_SUCCESS)
	{
		switch (getEngModeInfoCnf->network)
		{
		case CI_DEV_EM_NETWORK_GSM:
		{
			/*If the network type is specified to be GSM,
			      the engineering mode information is returned in the proceeding indication:
			      CI_DEV_PRIM_GMS_ENGMODE_INFO_IND. */
			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
		break;
		case CI_DEV_EM_NETWORK_UMTS:
			/* Since UMTS data are very large, they will not be returned through this config structure, while they will be returned later through 4 new indication below:
				 CI_DEV_PRIM_UMTS_ENGMODE_SVCCELL_INFO_IND,
				 CI_DEV_PRIM_UMTS_ENGMODE_INTRAFREQ_INFO_IND,
				 CI_DEV_PRIM_UMTS_ENGMODE_INTERFREQ_INFO_IND,
				 CI_DEV_PRIM_UMTS_ENGMODE_INTERRAT_INFO_IND,  */
			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
			break;
		default:
			ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
			break;
		}
	}
	else
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);

	return;
}
#endif
/****************************************************************
 *  F@: processSetSVConf - Process CI cnf msg of
 */
static void processSetSVConf(UINT32 atHandle, const void *paras)
{
	CiDevPrimSetSvCnf* setSVCnf = (CiDevPrimSetSvCnf *)paras;

	if (setSVCnf->rc == CIRC_DEV_SUCCESS)
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
	}
	else
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);

	return;
}

/****************************************************************
 *  F@: processGetSVConf - Process CI cnf msg of
 */
static void processGetSVConf(UINT32 atHandle, const void *paras)
{
	CHAR temp[4];
	CHAR resImei[50];       /* large enough for IMEI response */
	CiDevPrimGetSvCnf* getSVCnf = (CiDevPrimGetSvCnf *)paras;

	if (getSVCnf->rc == CIRC_DEV_SUCCESS)
	{
		resImei[0] = '\0';
		memset(temp, 0, sizeof(temp));
		if(getSVCnf->SVDigits.len > 2)
			memcpy(temp, getSVCnf->SVDigits.val, 2);
		else
			memcpy(temp, getSVCnf->SVDigits.val, getSVCnf->SVDigits.len);
		sprintf((char *)resImei, "*CGSN: %s", temp);

		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)resImei);
	}
	else
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);

	return;
}

/****************************************************************
 *  F@: processSetBandModeConf - Process CI cnf msg of AT*BAND=
 */
static void processSetBandModeConf(UINT32 atHandle, const void *paras)
{
	CiDevPrimSetBandModeCnf* setBandModeCnf = (CiDevPrimSetBandModeCnf *)paras;

	setBandModeCnf = (CiDevPrimSetBandModeCnf *)paras;
	if ( setBandModeCnf->result == CIRC_DEV_SUCCESS)
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, 0);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
	}

	return;
}

/****************************************************************
 *  F@: processGetBandModeConf - Process CI cnf msg of AT*BAND?
 */
static void processGetBandModeConf(UINT32 atHandle, const void *paras)
{
	CiDevPrimGetBandModeCnf* getBandModeCnf = (CiDevPrimGetBandModeCnf *)paras;
	char bandBuf[100];
	DBGMSG("net work mode:%d, preferred mode:%d\n", getBandModeCnf->networkMode, getBandModeCnf->preferredMode);
	if (getBandModeCnf->result == CIRC_DEV_SUCCESS)
	{
		if (getBandModeCnf->networkMode == CI_DEV_NW_GSM)
		{
			sprintf((char *)bandBuf, "*BAND: %d, %d, %d, %d\r\n", getBandModeCnf->networkMode, getBandModeCnf->GSMBandMode, getBandModeCnf->roamingConfig, getBandModeCnf->srvDomain);
			ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, bandBuf);
		}
		else if (getBandModeCnf->networkMode == CI_DEV_NW_UMTS)
		{
			sprintf((char *)bandBuf, "*BAND: %d, %d, %d, %d\r\n", getBandModeCnf->networkMode, getBandModeCnf->UMTSBandMode, getBandModeCnf->roamingConfig, getBandModeCnf->srvDomain);
			ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, bandBuf);
		}
		else if (getBandModeCnf->networkMode == CI_DEV_NW_DUAL_MODE)
		{
			CiDevNetworkMode networkMode;
			switch(getBandModeCnf->preferredMode)
			{
				case CI_DEV_NW_DUAL_MODE:
					networkMode = 2;
					break;
				case CI_DEV_NW_GSM:
					networkMode = 3;
					break;
				case CI_DEV_NW_UMTS:
					networkMode = 4;
					break;
				default :
					ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
					return;
			}

			sprintf((char *)bandBuf, "*BAND: %d, %d, %d, %d, %d\r\n", networkMode, getBandModeCnf->GSMBandMode, getBandModeCnf->UMTSBandMode, getBandModeCnf->roamingConfig, getBandModeCnf->srvDomain);
			ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, bandBuf);
		}
		else
			ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);

	}
	else
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);

	return;
}

/****************************************************************
 *  F@: processGetSupportedBandModeConf - Process CI cnf msg of AT*BAND=?
 */
static void processGetSupportedBandModeConf(UINT32 atHandle, const void *paras)
{
	CiDevPrimGetSupportedBandModeCnf* getSupportedBandModeCnf = (CiDevPrimGetSupportedBandModeCnf *)paras;
	char bandBuf[100];

	if (getSupportedBandModeCnf->result == CIRC_DEV_SUCCESS)
	{
		UINT32 bitsNWModeRange[2], bitsGSMBandRange[2], bitsUMTSBandRange[2];
		bitsNWModeRange[0] = -1, bitsGSMBandRange[0] = -1, bitsUMTSBandRange[0] = -1;
		int i = 0;
		for ( i = 0; i < (int)(8 * sizeof(UINT32)); i++ )
		{
			if ( ((int)bitsNWModeRange[0] == -1) && (getSupportedBandModeCnf->bitsNetworkMode & 1) )
			{
				bitsNWModeRange[0] = i;
				bitsNWModeRange[1] = i;
			}
			else if ( getSupportedBandModeCnf->bitsNetworkMode & 1 )
			{
				bitsNWModeRange[1] = i;
			}
			getSupportedBandModeCnf->bitsNetworkMode = getSupportedBandModeCnf->bitsNetworkMode >> 1;
		}
		sprintf( bandBuf, "*BAND:(%d-%d),%d,%d\r\n",
			 bitsNWModeRange[0], bitsNWModeRange[1],
			 getSupportedBandModeCnf->bitsGSMBandMode,
			 getSupportedBandModeCnf->bitsUMTSBandMode);
		ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, bandBuf);

	}
	else
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);

	return;
}


/****************************************************************************************
*  FUNCTION:  DEV_SetPOWERIND
*
*  PARAMETERS: 
*  		powerState: 1 means suspend; 0 means resume
*
*  DESCRIPTION: interface to set AP Power Ind, implementation of AT*POWERIND=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode DEV_SetPOWERIND(UINT32 atHandle, UINT32 powerState)
{
	CiReturnCode ret = CIRC_FAIL;
	CiDevPrimApPowerNotifyReq                         *apPowerNotifyReq;
	
	apPowerNotifyReq = (CiDevPrimApPowerNotifyReq *)utlCalloc(1, sizeof(CiDevPrimApPowerNotifyReq));
	if (apPowerNotifyReq == NULL)
		return CIRC_FAIL;

	apPowerNotifyReq->powerState = powerState;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_AP_POWER_NOTIFY_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_DEV_PRIM_AP_POWER_NOTIFY_REQ), (void*)apPowerNotifyReq);

	return ret;
}

/****************************************************************
 *  F@: processGetEngModeInfoConf - Process CI cnf msg of AT+EEMGINFO?
 */
static void processGetEngModeInfoConf(UINT32 atHandle, const void *paras)
{
	CiDevPrimGetEngmodeInfoCnf* getEngModeInfoCnf = (CiDevPrimGetEngmodeInfoCnf *)paras;
	char EngModeInfo[50];

	DBGMSG("%s: CI_DEV_PRIM_GET_ENGMODE_INFO_CNF rc: %d, network: %d\n", getEngModeInfoCnf->rc, getEngModeInfoCnf->network);
	if (getEngModeInfoCnf->rc == CIRC_DEV_SUCCESS)
	{
		sprintf((char *)EngModeInfo, "+EEMGINFO : %d, %d\r\n", getEngModeInfoCnf->mode, getEngModeInfoCnf->network);
		ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, EngModeInfo);
	}
	else
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);

	return;
}

/****************************************************************
 *  F@: processGetSVConf - Process CI cnf msg of
 */
static void processlpNwulConf(UINT32 atHandle, const void *paras)
{
	char rspBuf[50];       /* large enough for IMEI response */
	CiDevPrimLpNwulMsgCnf *lpNwulMsgCnf = (CiDevPrimLpNwulMsgCnf *)paras;

	rspBuf[0] = '\0';
	sprintf(rspBuf, "+LPNWUL:%d\r\n", lpNwulMsgCnf->result);

	ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)rspBuf);

	return;
}
/****************************************************************
 *  F@: processSetHsdpaModeConf - Process CI cnf msg of AT*EHSDPA=
 */
static void processSetHsdpaModeConf(UINT32 atHandle, const void *paras)
{
	CiDevPrimEnableHsdpaCnf *enableHsdpaCnf = (CiDevPrimEnableHsdpaCnf *)paras;

	if ( enableHsdpaCnf->rc == CIRC_DEV_SUCCESS)
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, 0);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
	}

	return;
}
/****************************************************************
 *  F@: processGetHsdpaModeConf - Process CI cnf msg of AT*EHSDPA?
 */
static void processGetHsdpaModeConf(UINT32 atHandle, const void *paras)
{
	char rspBuf[50];
	UINT8 mode;
	CiDevPrimGetHsdpaStatusCnf *getHsdpaStatusCnf = (CiDevPrimGetHsdpaStatusCnf *)paras;

	if ( (getHsdpaStatusCnf->rc == CIRC_DEV_SUCCESS) && libConvertHsdpaMode2At(getHsdpaStatusCnf->hspaConfig, &mode))
	{
		sprintf(rspBuf, "*EHSDPA:%d,%d,%d,%d\r\n", mode, getHsdpaStatusCnf->dlCategory, getHsdpaStatusCnf->ulCategory, getHsdpaStatusCnf->cpcState);
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, rspBuf);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
	}

	return;
}

#ifdef AT_PRODUCTION_CMNDS
/* table for mapping Dev return code to CME error code */
static UINT16 DevRc2CmeErrorTbl[CIRC_DEV_NUM_RESCODES] = {
	0xffff,            /* CIRC_DEV_SUCCESS */
	CME_PHONE_FAILURE, /* CIRC_DEV_FAILURE */
	CME_NO_CONNECTION, /* CIRC_DEV_NO_CONNECTION */
	CME_UNKNOWN        /* CIRC_DEV_UNKNOWN */
};
/****************************************************************
 *  F@: processSetTdModeTxRxConf - Process CI cnf msg of *TDTR
 */
static void processSetTdModeTxRxConf(UINT32 atHandle, const void *paras)
{
	CiDevPrimSetTdModeTxRxCnf *setTdModeTxRxCnf = (CiDevPrimSetTdModeTxRxCnf *)paras;

	if(setTdModeTxRxCnf->rc == CIRC_DEV_SUCCESS)
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, DevRc2CmeErrorTbl[setTdModeTxRxCnf->rc], NULL);
	}

	return;
}

/****************************************************************
 *  F@: processSetTdModeLoopbackConf - Process CI cnf msg of *TGCTRL
 */
static void processSetTdModeLoopbackConf(UINT32 atHandle, const void *paras)
{
	char rspBuf[50];
	CiDevPrimSetTdModeLoopbackCnf *setTdModeLoopbackCnf = (CiDevPrimSetTdModeLoopbackCnf *)paras;

	if(setTdModeLoopbackCnf->rc == CIRC_DEV_SUCCESS)
	{
		rspBuf[0] = '\0';
		sprintf(rspBuf, "*TGCTRL: %d\r\n", setTdModeLoopbackCnf->regValue);
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)rspBuf);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, DevRc2CmeErrorTbl[setTdModeLoopbackCnf->rc], NULL);
	}

	return;
}

/****************************************************************
 *  F@: processSetGsmModeTxRxConf - Process CI cnf msg of *GSMTR
 */
static void processSetGsmModeTxRxConf(UINT32 atHandle, const void *paras)
{
	char rspBuf[50];
	CiDevPrimSetGsmModeTxRxCnf *setGsmModeTxRxCnf = (CiDevPrimSetGsmModeTxRxCnf *)paras;

	if(setGsmModeTxRxCnf->rc == CIRC_DEV_SUCCESS)
	{
		rspBuf[0] = '\0';
		sprintf(rspBuf, "*GSMTR: %d\r\n", setGsmModeTxRxCnf->rssiDbmValue);
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)rspBuf);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, DevRc2CmeErrorTbl[setGsmModeTxRxCnf->rc], NULL);
	}

	return;
}

/****************************************************************
 *  F@: processSetGsmControlInterfaceConf - Process CI cnf msg of *TGCTRL
 */
static void processSetGsmControlInterfaceConf(UINT32 atHandle, const void *paras)
{
	char rspBuf[50];
	CiDevPrimSetGsmControlInterfaceCnf *setGsmControlInterfaceCnf = (CiDevPrimSetGsmControlInterfaceCnf *)paras;

	if(setGsmControlInterfaceCnf->rc == CIRC_DEV_SUCCESS)
	{
		rspBuf[0] = '\0';
		sprintf(rspBuf, "*TGCTRL: %d,%x\r\n", setGsmControlInterfaceCnf->regValue, setGsmControlInterfaceCnf->addrReg);
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)rspBuf);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, DevRc2CmeErrorTbl[setGsmControlInterfaceCnf->rc], NULL);
	}

	return;
}
#endif
#ifdef AT_CUSTOMER_HTC
/****************************************************************
 *  F@: processSetModeSwitchOptConf - Process CI cnf msg of +TPCN=
 */
static void processSetModeSwitchOptConf(UINT32 atHandle, const void *paras)
{
	CiDevPrimSetModeSwitchOptCnf *setModeSwitchOptCnf = (CiDevPrimSetModeSwitchOptCnf *)paras;

	if(setModeSwitchOptCnf->rc == CIRC_DEV_SUCCESS)
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
	}
	return;
}
/****************************************************************
 *  F@: processGetModeSwitchOptConf - Process CI cnf msg of +TPCN?
 */
static void processGetModeSwitchOptConf(UINT32 atHandle, const void *paras)
{
	char rspBuf[50];
	CiDevPrimGetModeSwitchOptCnf *getModeSwitchOptCnf = (CiDevPrimGetModeSwitchOptCnf *)paras;

	if(getModeSwitchOptCnf->rc == CIRC_DEV_SUCCESS)
	{
		sprintf(rspBuf, "+TPCN:%d", getModeSwitchOptCnf->option);
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, rspBuf);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
	}
	return;
}
#endif

/****************************************************************
 *  F@: processSetNetworkMonitorOptCnf - Process CI cnf msg of ^DCTS=
 */
static void processSetNetworkMonitorOptCnf(UINT32 atHandle, const void *paras)
{
	CiDevPrimSetNetworkMonitorOptCnf *setNetworkMonitorOptCnf = (CiDevPrimSetNetworkMonitorOptCnf *)paras;

	if(setNetworkMonitorOptCnf->rc == CIRC_DEV_SUCCESS)
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
	}
	return;
}

/****************************************************************
 *  F@: processGetNetworkMonitorOptCnf - Process CI cnf msg of ^DCTS?
 */
static void processGetNetworkMonitorOptCnf(UINT32 atHandle, const void *paras)
{
	char rspBuf[50];
	CiDevPrimGetNetworkMonitorOptCnf *getNetworkMonitorOptCnf = (CiDevPrimGetNetworkMonitorOptCnf *)paras;

	if(getNetworkMonitorOptCnf->rc == CIRC_DEV_SUCCESS)
	{
		snprintf(rspBuf, utlNumberOf(rspBuf), "^DCTS: %d,%d\r\n", getNetworkMonitorOptCnf->Option, getNetworkMonitorOptCnf->Mode);
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, rspBuf);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
	}

	return;
}
/****************************************************************
 *  F@: processSetProtocolStatusConfigCnf - Process CI cnf msg of ^DEELS=
 */
static void processSetProtocolStatusConfigCnf(UINT32 atHandle, const void *paras)
{
	CiDevPrimSetProtocolStatusConfigCnf *setProtocolStatusConfigCnf = (CiDevPrimSetProtocolStatusConfigCnf *)paras;

	if(setProtocolStatusConfigCnf->rc == CIRC_DEV_SUCCESS)
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
	}

	return;
}
/****************************************************************
 *  F@: processGetProtocolStatusConfigCnf - Process CI cnf msg of ^DEELS?
 */
static void processGetProtocolStatusConfigCnf(UINT32 atHandle, const void *paras)
{
	char rspBuf[50];
	CiDevPrimGetProtocolStatusConfigCnf *getProtocolStatusConfigCnf = (CiDevPrimGetProtocolStatusConfigCnf *)paras;

	if(getProtocolStatusConfigCnf->rc == CIRC_DEV_SUCCESS)
	{
		snprintf(rspBuf, utlNumberOf(rspBuf), "^DEELS: %d\r\n", getProtocolStatusConfigCnf->Option);
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, rspBuf);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
	}

	return;
}
/****************************************************************
 *  F@: processSetEventIndConfigCnf - Process CI cnf msg of ^DEVEI=
 */
static void processSetEventIndConfigCnf(UINT32 atHandle, const void *paras)
{
	CiDevPrimSetEventIndConfigCnf *setEventIndConfigCnf = (CiDevPrimSetEventIndConfigCnf *)paras;

	if(setEventIndConfigCnf->rc == CIRC_DEV_SUCCESS)
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
	}

	return;
}
/****************************************************************
 *  F@: processGetEventIndConfigCnf  - Process CI cnf msg of ^DEVEI?
 */
static void processGetEventIndConfigCnf(UINT32 atHandle, const void *paras)
{
	char rspBuf[50];
	CiDevPrimGetEventIndConfigCnf *getEventIndConfigCnf = (CiDevPrimGetEventIndConfigCnf *)paras;

	if(getEventIndConfigCnf->rc == CIRC_DEV_SUCCESS)
	{
		snprintf(rspBuf, utlNumberOf(rspBuf), "^DEVEI: %d\r\n", getEventIndConfigCnf->Option);
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, rspBuf);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
	}

	return;
}
/****************************************************************
 *  F@: processSetWirelessParamConfigCnf - Process CI cnf msg of ^DNPR=
 */
static void processSetWirelessParamConfigCnf(UINT32 atHandle, const void *paras)
{
	CiDevPrimSetWirelessParamConfigCnf *setWirelessParamConfigCnf = (CiDevPrimSetWirelessParamConfigCnf *)paras;

	if(setWirelessParamConfigCnf->rc == CIRC_DEV_SUCCESS)
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
	}

	return;
}
/****************************************************************
 *  F@: processGetWirelessParamConfigCnf  - Process CI cnf msg of ^DNPR?
 */
static void processGetWirelessParamConfigCnf(UINT32 atHandle, const void *paras)
{
	char rspBuf[50];
	CiDevPrimGetWirelessParamConfigCnf *getWirelessParamConfigCnf = (CiDevPrimGetWirelessParamConfigCnf *)paras;

	if(getWirelessParamConfigCnf->rc == CIRC_DEV_SUCCESS)
	{
		snprintf(rspBuf, utlNumberOf(rspBuf), "^DNPR: %d,%d\r\n", getWirelessParamConfigCnf->Option, getWirelessParamConfigCnf->Interval);
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, rspBuf);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
	}

	return;
}
/****************************************************************
 *  F@: processSetSignalingReportConfigCnf - Process CI cnf msg of ^DUSR=
 */
static void processSetSignalingReportConfigCnf(UINT32 atHandle, const void *paras)
{
	CiDevPrimSetSignalingReportConfigCnf *setSignalingReportConfigCnf = (CiDevPrimSetSignalingReportConfigCnf *)paras;

	if(setSignalingReportConfigCnf->rc == CIRC_DEV_SUCCESS)
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
	}

	return;
}
/****************************************************************
 *  F@: processGetSignalingReportConfigCnf  - Process CI cnf msg of ^DUSR?
 */
static void processGetSignalingReportConfigCnf(UINT32 atHandle, const void *paras)
{
	char rspBuf[50];
	CiDevPrimGetSignalingReportConfigCnf *getSignalingReportConfigCnf = (CiDevPrimGetSignalingReportConfigCnf *)paras;

	if(getSignalingReportConfigCnf->rc == CIRC_DEV_SUCCESS)
	{
		snprintf(rspBuf, utlNumberOf(rspBuf), "^DUSR: %d,%d\r\n", getSignalingReportConfigCnf->Option, getSignalingReportConfigCnf->Mode);
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, rspBuf);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
	}

	return;
}

/****************************************************************************************
*
*  DESCRIPTION: Callback function when receiving confirmation CI message related with Dev
*
****************************************************************************************/
void devCnf(CiSgOpaqueHandle opSgCnfHandle,
	    CiServiceGroupID svgId,
	    CiPrimitiveID primId,
	    CiRequestHandle reqHandle,
	    void*              paras)
{
	UNUSEDPARAM(opSgCnfHandle)

	UINT32 atHandle = GET_AT_HANDLE(reqHandle);

	/* Validate the para pointer */
	if ( (paras == NULL) && (primId < CI_DEV_PRIM_LAST_COMMON_PRIM) )
	{
		ERRMSG("para pointer of CI cnf msg is NULL!");
		return;
	}
	/*
	**  Determine the primitive being confirmed.
	*/
	switch (primId)
	{
	case CI_DEV_PRIM_GET_SERIALNUM_ID_CNF:
	{
		processGetSerialNumIdConf(atHandle, paras);
		break;
	}

	case CI_DEV_PRIM_GET_FUNC_CNF:
	{
		processGetFuncConf(reqHandle, paras);
		break;
	}

	case CI_DEV_PRIM_SET_FUNC_CNF:
	{
		processSetFuncConf(reqHandle, paras);
		break;
	}

	case CI_DEV_PRIM_SET_ENGMODE_REPORT_OPTION_CNF:
	{
		// disable CGED
		// processSetEngineModeReportOptionConf(atHandle, paras);

		// for EEMOPT
		processSetEngModeReportOptionConf(atHandle, paras);
		break;
	}

	case CI_DEV_PRIM_GET_ENGMODE_INFO_CNF:
	{
		// disable CGED
		//processGetEngineModeInfoConf(atHandle, paras);

		// for EEMOPT
		processGetEngModeInfoConf(atHandle, paras);

		break;
	}
	/*Michal Bukai - AT*BAND support - START*/
	case CI_DEV_PRIM_SET_BAND_MODE_CNF:
	{
		processSetBandModeConf(atHandle, paras);
		break;
	}
	case CI_DEV_PRIM_GET_BAND_MODE_CNF:
	{
		processGetBandModeConf(atHandle, paras);
		break;
	}
	case CI_DEV_PRIM_GET_SUPPORTED_BAND_MODE_CNF:
	{
		processGetSupportedBandModeConf(atHandle, paras);
		break;
	}
	/*Michal Bukai - AT*BAND support - END*/

	case CI_DEV_PRIM_SET_SV_CNF:
	{
		processSetSVConf(atHandle, paras);
		break;
	}

	case CI_DEV_PRIM_GET_SV_CNF:
	{
		processGetSVConf(atHandle, paras);
		break;
	}
	case CI_DEV_PRIM_AP_POWER_NOTIFY_CNF:
	{
		CiDevPrimApPowerNotifyCnf *apPowerNotifyCnf = (CiDevPrimApPowerNotifyCnf *)paras;
		if (apPowerNotifyCnf->rc == CIRC_DEV_SUCCESS)
			ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		else
			ATRESP(atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}
	case CI_DEV_PRIM_GET_REVISION_ID_CNF:
	{
		CiDevPrimGetRevisionIdCnf *revisionIdCnf = (CiDevPrimGetRevisionIdCnf *)paras;
		DPRINTF("CI_DEV_PRIM_GET_REVISION_ID_CNF: rc: %d!\n", revisionIdCnf->rc);
		if (revisionIdCnf->rc == CIRC_DEV_SUCCESS)
		{
			memcpy(revisionId, revisionIdCnf->revisionStr.valStr, revisionIdCnf->revisionStr.len);
			revisionId[revisionIdCnf->revisionStr.len] = '\0';
			DPRINTF("CI_DEV_PRIM_GET_REVISION_ID_CNF: revisionID: %s!\n", revisionId);
		}
		break;
	}
	case CI_DEV_PRIM_ENABLE_HSDPA_CNF:
	{
		processSetHsdpaModeConf(atHandle, paras);
		break;
	}
	case CI_DEV_PRIM_GET_HSDPA_STATUS_CNF:
	{
		processGetHsdpaModeConf(atHandle, paras);
		break;
	}
	case CI_ERR_PRIM_HASNOSUPPORT_CNF:
	{
		ATRESP(atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}
	case CI_ERR_PRIM_HASINVALIDPARAS_CNF:
	{
		ATRESP(atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
		break;
	}
	case CI_ERR_PRIM_ISINVALIDREQUEST_CNF:
	{
		ATRESP(atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		break;
	}
	case CI_DEV_PRIM_LP_NWUL_MSG_CNF:
	{
		processlpNwulConf(atHandle, paras);
		break;
	}
#ifdef AT_PRODUCTION_CMNDS
	case CI_DEV_PRIM_SET_TD_MODE_TX_RX_CNF:
	{
		processSetTdModeTxRxConf(atHandle, paras);
		break;
	}
	case CI_DEV_PRIM_SET_TD_MODE_LOOPBACK_CNF:
	{
		processSetTdModeLoopbackConf(atHandle, paras);
		break;
	}
	case CI_DEV_PRIM_SET_GSM_MODE_TX_RX_CNF:
	{
		processSetGsmModeTxRxConf(atHandle, paras);
		break;
	}
	case CI_DEV_PRIM_SET_GSM_CONTROL_INTERFACE_CNF:
	{
		processSetGsmControlInterfaceConf(atHandle, paras);
		break;
	}
#endif
#ifdef AT_CUSTOMER_HTC
	case CI_DEV_PRIM_SET_MODE_SWITCH_OPTION_CNF:
	{
		processSetModeSwitchOptConf(atHandle, paras);
		break;
	}
	case CI_DEV_PRIM_GET_MODE_SWITCH_OPTION_CNF:
	{
		processGetModeSwitchOptConf(atHandle, paras);
		break;
	}
#endif
	case CI_DEV_PRIM_SET_NETWORK_MONITOR_OPTION_CNF:
	{
		processSetNetworkMonitorOptCnf(atHandle, paras);
		break;
	}
	case CI_DEV_PRIM_GET_NETWORK_MONITOR_OPTION_CNF:
	{
		processGetNetworkMonitorOptCnf(atHandle, paras);
		break;
	}
	case CI_DEV_PRIM_SET_PROTOCOL_STATUS_CONFIG_CNF:
	{
		processSetProtocolStatusConfigCnf(atHandle, paras);
		break;
	}
	case CI_DEV_PRIM_GET_PROTOCOL_STATUS_CONFIG_CNF:
	{
		processGetProtocolStatusConfigCnf(atHandle, paras);
		break;
	}
	case CI_DEV_PRIM_SET_EVENT_IND_CONFIG_CNF:
	{
		processSetEventIndConfigCnf(atHandle, paras);
		break;
	}
	case CI_DEV_PRIM_GET_EVENT_IND_CONFIG_CNF:
	{
		processGetEventIndConfigCnf(atHandle, paras);
		break;
	}
	case CI_DEV_PRIM_SET_WIRELESS_PARAM_CONFIG_CNF:
	{
		processSetWirelessParamConfigCnf(atHandle, paras);
		break;
	}
	case CI_DEV_PRIM_GET_WIRELESS_PARAM_CONFIG_CNF:
	{
		processGetWirelessParamConfigCnf(atHandle, paras);
		break;
	}
	case CI_DEV_PRIM_SET_SIGNALING_REPORT_CONFIG_CNF:
	{
		processSetSignalingReportConfigCnf(atHandle, paras);
		break;
	}
	case CI_DEV_PRIM_GET_SIGNALING_REPORT_CONFIG_CNF:
	{
		processGetSignalingReportConfigCnf(atHandle, paras);
		break;
	}
	default:
	{
		break;
	}
	}

	/* free up the confirmation memory */
	atciSvgFreeCnfMem( svgId, primId, paras );

	return;
}

/****************************************************************************************
*
*  DESCRIPTION: Code to process indication message
*
****************************************************************************************/
#define UMTS_EM_IND_NUM_MAX 4 // totally 4 indication for UMTS.
#define EngmodeInfoEachMaxLen 100
#define EM_INFO_OUTPUT(s) ATRESP(IND_REQ_HANDLE, 0, 0, s)
#if 0
/****************************************************************
 *  F@: libEngmodeInfoOutput_GSM -  OUTPUT THE 2G ENGINEERING MODE INFORMATION
 */
static BOOL libEngmodeInfoOutput_GSM(EngModeGSMData *pbData, EngModeState mode) // engmode mode
{
	EngModeGSMData *pEngmodeInfo;
	UINT32 dwnbcellidex = 0;
	char EngmodeEachInfo[EngmodeInfoEachMaxLen];

	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	pEngmodeInfo = (EngModeGSMData *)pbData;

	EM_INFO_OUTPUT("Next to output the engineering mode information in details: ");
	EM_INFO_OUTPUT("/***************************************************/\n" );

	switch (mode)
	{
	case EM_IDLE:
		sprintf(EngmodeEachInfo, "current mode : IDLE " );
		break;
	case EM_DEDICATED:
		sprintf(EngmodeEachInfo, "current mode : DEDICATED " );
		break;
	default:
		sprintf(EngmodeEachInfo, "current mode : UNKNOWENED " );
		break;
	}
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "network type : GSM "  );
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	EM_INFO_OUTPUT( "Serving Cell information: " );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Receive Signal Level: 0x%x", pEngmodeInfo->svcCellInfo.rxSigLevel  );
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Receive Signal Level accessed over all TDMA frames: 0x%x", pEngmodeInfo->svcCellInfo.rxSigLevelFull  );
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Receive Signal Level accessed over subset of TDMA frames: 0x%x", pEngmodeInfo->svcCellInfo.rxSigLevelSub   );
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Receive Quality accessed over all TDMA frames: 0x%x", pEngmodeInfo->svcCellInfo.rxQualityFull  );
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Routing Area code: 0x%x", pEngmodeInfo->svcCellInfo.rac  );
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Base Transceiver Station Identity Code : 0x%x", pEngmodeInfo->svcCellInfo.bsic);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Network operation mode : 0x%x", pEngmodeInfo->svcCellInfo.nom);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Network Control Order : 0x%x", pEngmodeInfo->svcCellInfo.nco);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Number of mutiframes between paging messages sent : 0x%x", pEngmodeInfo->svcCellInfo.bs_pa_mfrms);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Mobile Country Code : 0x%x", pEngmodeInfo->svcCellInfo.mcc);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Mobile Network Code : 0x%x", pEngmodeInfo->svcCellInfo.mnc);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);


	sprintf(EngmodeEachInfo, "Location Area Code : 0x%x", pEngmodeInfo->svcCellInfo.lac);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Cell Identity : 0x%x", pEngmodeInfo->svcCellInfo.ci);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Absolute Radio Frequency Channel Number : 0x%x", pEngmodeInfo->svcCellInfo.arfcn);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Path loss criterion parameter #1 : 0x%x", pEngmodeInfo->svcCellInfo.C1);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Path loss criterion parameter #2 : 0x%x", pEngmodeInfo->svcCellInfo.C2);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "GPRS Signal Level Threshold Criterion Parameter : 0x%x", pEngmodeInfo->svcCellInfo.C31);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "GPRS Cell Ranking Criterion Parameter : 0x%x", pEngmodeInfo->svcCellInfo.C32);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Periodic LA Update Timer (T3212) in minutes : 0x%x", pEngmodeInfo->svcCellInfo.t3212);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Periodic RA Update Timer (T3312) in minutes : 0x%x", pEngmodeInfo->svcCellInfo.t3312);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	if (FALSE == pEngmodeInfo->svcCellInfo.pbcchSupport )
	{
		sprintf(EngmodeEachInfo, "Support of PBCCH : No ");
	}
	else
	{
		sprintf(EngmodeEachInfo, "Support of PBCCH : Yes ");
	}
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	EM_INFO_OUTPUT( "Neighboring Cell information: \n" );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Neighboring Cell Numbers: %d", pEngmodeInfo->numNCells);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);


	for (dwnbcellidex = 0; dwnbcellidex < pEngmodeInfo->numNCells; dwnbcellidex++)
	{
		sprintf(EngmodeEachInfo, "#%d Neighboring Cell information:", dwnbcellidex + 1 );
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Receive Signal Level : 0x%x", pEngmodeInfo->nbCellInfo[dwnbcellidex].rxSigLevel);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Base Transceiver Station Identity Code : 0x%x", pEngmodeInfo->nbCellInfo[dwnbcellidex].bsic);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Routing Area code : 0x%x", pEngmodeInfo->nbCellInfo[dwnbcellidex].rac);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Mobile Country Code : 0x%x", pEngmodeInfo->nbCellInfo[dwnbcellidex].mcc);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Mobile Network Code : 0x%x", pEngmodeInfo->nbCellInfo[dwnbcellidex].mnc);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Location Area Code : 0x%x", pEngmodeInfo->nbCellInfo[dwnbcellidex].lac);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Cell Identity : 0x%x", pEngmodeInfo->nbCellInfo[dwnbcellidex].ci);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Absolute Radio Frequency Channel Number : 0x%x", pEngmodeInfo->nbCellInfo[dwnbcellidex].arfcn);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Path loss criterion parameter #1: 0x%x", pEngmodeInfo->nbCellInfo[dwnbcellidex].C1);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Path loss criterion parameter #2: 0x%x", pEngmodeInfo->nbCellInfo[dwnbcellidex].C2);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " GPRS Signal Level Threshold Criterion Parameter: 0x%x", pEngmodeInfo->nbCellInfo[dwnbcellidex].C31);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " GPRS Cell Ranking Criterion Parameter: 0x%x", pEngmodeInfo->nbCellInfo[dwnbcellidex].C32);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);
	}

	EM_INFO_OUTPUT("  End of the output for GSM engineering mode information.   " );
	EM_INFO_OUTPUT("/***************************************************/" );
	EM_INFO_OUTPUT("\r\n");
	return TRUE;
}

/****************************************************************
 *  F@: libEngmodeInfoOutput_UMTS -  OUTPUT THE 3G UMTS ENGINEERING MODE INFORMATION
 */
static BOOL libEngmodeInfoOutput_UMTS(EngModeUmtsData *pbData, EngModeState mode )  // engmode mode
{
	UNUSEDPARAM(mode)
	EngModeUmtsData * pEngmodeInfo;
	UINT32 dwnbcellidex = 0;
	char EngmodeEachInfo[EngmodeInfoEachMaxLen];

	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	pEngmodeInfo = (EngModeUmtsData *)pbData;

	EM_INFO_OUTPUT("Next to output the 3G UMTS engineering mode information in details: " );
	EM_INFO_OUTPUT("/***************************************************/\n" );

	/*========================================================================
	   mode
	   ========================================================================*/
	/*    switch(mode)
		{
		case EM_IDLE:
			sprintf(EngmodeEachInfo, "current mode : IDLE " );
			break;
		case EM_DEDICATED:
			sprintf(EngmodeEachInfo, "current mode : DEDICATED " );
			break;
		default:
			sprintf(EngmodeEachInfo, "current mode : UNKNOWENED " );
			break;
		} */

	sprintf(EngmodeEachInfo, "current mode : Not Applicable in UMTS " );

	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "network type : UMTS ");
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	EM_INFO_OUTPUT( "Serving Cell information: " );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	/* Serving Cell Information Ouptut */
	/*========================================================================
	   1. Measurements
	   ========================================================================*/

	if (pEngmodeInfo->svcCellInfo.sCellMeasPresent == TRUE)
	{
		sprintf(EngmodeEachInfo, "Cell Measurements information have been updated");
	}
	else
	{
		sprintf(EngmodeEachInfo, "Cell Measurements information have not been updated");
	}
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);


	sprintf(EngmodeEachInfo, "CPICH Received Signal Code Power: 0x%x", pEngmodeInfo->svcCellInfo.sCellMeas.cpichRSCP);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "UTRA Carrier RSSI: 0x%x", pEngmodeInfo->svcCellInfo.sCellMeas.utraRssi);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "CPICH Ec/N0: 0x%x", pEngmodeInfo->svcCellInfo.sCellMeas.cpichEcN0);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Cell Selection Quality (Squal): 0x%x", pEngmodeInfo->svcCellInfo.sCellMeas.sQual);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Cell Selection Rx Level (Srxlev): 0x%x", pEngmodeInfo->svcCellInfo.sCellMeas.sRxLev);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "UE Transmitted Power: 0x%x", pEngmodeInfo->svcCellInfo.sCellMeas.txPower);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	/*========================================================================
	   2. PLMN/Cell Parameters
	   ========================================================================*/
	if (pEngmodeInfo->svcCellInfo.sCellParamPresent == TRUE)
	{
		sprintf(EngmodeEachInfo, "PLMN/Cell Parameters information have been updated");
	}
	else
	{
		sprintf(EngmodeEachInfo, "PLMN/Cell Parameters information have not been updated");
	}
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Routing Area Code: 0x%x", pEngmodeInfo->svcCellInfo.sCellParam.rac);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Network Operation Mode: 0x%x", pEngmodeInfo->svcCellInfo.sCellParam.nom);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Mobile Country Code: 0x%x", pEngmodeInfo->svcCellInfo.sCellParam.mcc);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Mobile Network Code: 0x%x", pEngmodeInfo->svcCellInfo.sCellParam.mnc);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Location Area Code: 0x%x", pEngmodeInfo->svcCellInfo.sCellParam.lac);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Cell Identity: 0x%x", pEngmodeInfo->svcCellInfo.sCellParam.ci);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "URA Identity: 0x%x", pEngmodeInfo->svcCellInfo.sCellParam.uraId);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Primary Scrambling Code: 0x%x", pEngmodeInfo->svcCellInfo.sCellParam.psc);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Absolute Radio Frequency Channel Number: 0x%x", pEngmodeInfo->svcCellInfo.sCellParam.arfcn);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Periodic LA Update Timer (T3212) in minutes: 0x%x", pEngmodeInfo->svcCellInfo.sCellParam.t3212);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Periodic RA Update Timer (T3312) in minutes : 0x%x", pEngmodeInfo->svcCellInfo.sCellParam.t3312);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	if (FALSE == pEngmodeInfo->svcCellInfo.sCellParam.hcsUsed )
	{
		sprintf(EngmodeEachInfo, "Hierarchical Cell Structure used?: No ");
	}
	else
	{
		sprintf(EngmodeEachInfo, "Hierarchical Cell Structure used?: Yes ");
	}
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	if (FALSE == pEngmodeInfo->svcCellInfo.sCellParam.attDetAllowed )
	{
		sprintf(EngmodeEachInfo, "Attach-Detach allowed?: No ");
	}
	else
	{
		sprintf(EngmodeEachInfo, "Attach-Detach allowed?: Yes ");
	}
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	/*	if (FALSE == pEngmodeInfo->svcCellInfo.sCellParam.cipherStatus )
		{
			sprintf(EngmodeEachInfo, "Ciphering Status = On?: No ");
		}
		else
		{
			sprintf(EngmodeEachInfo, "Ciphering Status = On?: Yes ");
		}
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);  */

	sprintf(EngmodeEachInfo, "CS Domain DRX Cycle Length: 0x%x", pEngmodeInfo->svcCellInfo.sCellParam.csDrxCycleLen);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "PS Domain DRX Cycle Length: 0x%x", pEngmodeInfo->svcCellInfo.sCellParam.psDrxCycleLen);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "UTRAN DRX Cycle Length: 0x%x", pEngmodeInfo->svcCellInfo.sCellParam.utranDrxCycleLen);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	/*========================================================================
	   3. UE Operation Status
	   ========================================================================*/
	if (pEngmodeInfo->svcCellInfo.ueOpStatusPresent == TRUE)
	{
		sprintf(EngmodeEachInfo, "UE Operation Status have been updated");
	}
	else
	{
		sprintf(EngmodeEachInfo, "UE Operation Status have not been updated");
	}
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);


	sprintf(EngmodeEachInfo, "RRC State: 0x%x", pEngmodeInfo->svcCellInfo.ueOpStatus.rrcState);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "Number of Radio Links: 0x%x", pEngmodeInfo->svcCellInfo.ueOpStatus.numLinks);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "U-RNTI: SRNC Identifier: 0x%x", pEngmodeInfo->svcCellInfo.ueOpStatus.srncId);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	sprintf(EngmodeEachInfo, "U-RNTI: S-RNTI: 0x%x", pEngmodeInfo->svcCellInfo.ueOpStatus.sRnti);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);


	/* Neighboring Cell information */
	/* 1. Number of Intra-Frequency FDD Cells */
	sprintf(EngmodeEachInfo, "Number of Intra-Frequency FDD Cells: 0x%x", pEngmodeInfo->numIntraFreq);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	/* 2. Number of Inter-Frequency FDD Cells */
	sprintf(EngmodeEachInfo, "Number of Inter-Frequency FDD Cells: 0x%x", pEngmodeInfo->numInterFreq);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	/* 3. Number of Inter-RAT GSM Cells */
	sprintf(EngmodeEachInfo, "Number of Inter-RAT GSM Cells: 0x%x", pEngmodeInfo->numInterRAT);
	EM_INFO_OUTPUT( EngmodeEachInfo );
	memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	/*========================================================================
	   4. Engineering Mode 3G (UMTS) Intra-Frequency FDD Cell Information structure
	   ========================================================================*/

	for (dwnbcellidex = 0; dwnbcellidex < pEngmodeInfo->numIntraFreq; dwnbcellidex++)
	{
		sprintf(EngmodeEachInfo, "#0x%x 3G (UMTS) Intra-Frequency FDD Cell information **********:", dwnbcellidex + 1 );
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		/* Measurements */
		sprintf(EngmodeEachInfo, " CPICH Received Signal Code Power: 0x%x", pEngmodeInfo->intraFreq[dwnbcellidex].cpichRSCP);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " UTRA Carrier RSSI : 0x%x", pEngmodeInfo->intraFreq[dwnbcellidex].utraRssi);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " CPICH Ec/N0: 0x%x", pEngmodeInfo->intraFreq[dwnbcellidex].cpichEcN0);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Cell Selection Quality (Squal): 0x%x", pEngmodeInfo->intraFreq[dwnbcellidex].sQual);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Cell Selection Rx Level (Srxlev): 0x%x", pEngmodeInfo->intraFreq[dwnbcellidex].sRxLev);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		/* PLMN/Cell Parameters */
		sprintf(EngmodeEachInfo, " Mobile Country Code: 0x%x", pEngmodeInfo->intraFreq[dwnbcellidex].mcc);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Mobile Network Code: 0x%x", pEngmodeInfo->intraFreq[dwnbcellidex].mnc);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Location Area Code: 0x%x", pEngmodeInfo->intraFreq[dwnbcellidex].lac);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Cell Identity: 0x%x", pEngmodeInfo->intraFreq[dwnbcellidex].ci);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Absolute Radio Frequency Channel Number: 0x%x", pEngmodeInfo->intraFreq[dwnbcellidex].arfcn);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);
	}

	/*========================================================================
	   5. Engineering Mode 3G (UMTS) Inter-Frequency FDD Cell Information structure
	   ========================================================================*/

	for (dwnbcellidex = 0; dwnbcellidex < pEngmodeInfo->numInterFreq; dwnbcellidex++)
	{
		sprintf(EngmodeEachInfo, "#%d 3G (UMTS) Inter-Frequency FDD Cell information **********:", dwnbcellidex + 1 );
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		/* Measurements */
		sprintf(EngmodeEachInfo, " CPICH Received Signal Code Power: 0x%x", pEngmodeInfo->interFreq[dwnbcellidex].cpichRSCP);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " UTRA Carrier RSSI : 0x%x", pEngmodeInfo->interFreq[dwnbcellidex].utraRssi);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " CPICH Ec/N0: 0x%x", pEngmodeInfo->interFreq[dwnbcellidex].cpichEcN0);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Cell Selection Quality (Squal): 0x%x", pEngmodeInfo->interFreq[dwnbcellidex].sQual);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Cell Selection Rx Level (Srxlev): 0x%x", pEngmodeInfo->interFreq[dwnbcellidex].sRxLev);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		/* PLMN/Cell Parameters */
		sprintf(EngmodeEachInfo, " Mobile Country Code: 0x%x", pEngmodeInfo->interFreq[dwnbcellidex].mcc);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Mobile Network Code: 0x%x", pEngmodeInfo->interFreq[dwnbcellidex].mnc);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Location Area Code: 0x%x", pEngmodeInfo->interFreq[dwnbcellidex].lac);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Cell Identity: 0x%x", pEngmodeInfo->interFreq[dwnbcellidex].ci);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Absolute Radio Frequency Channel Number: 0x%x", pEngmodeInfo->interFreq[dwnbcellidex].arfcn);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);
	}


	/*========================================================================
	   6. Engineering Mode 3G (UMTS) Inter-RAT GSM Cell Information structure
	   ========================================================================*/

	for (dwnbcellidex = 0; dwnbcellidex < pEngmodeInfo->numInterRAT; dwnbcellidex++)
	{
		sprintf(EngmodeEachInfo, "#0x%x 3G (UMTS) Inter-RAT GSM Cell information **********:", dwnbcellidex + 1 );
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		/* Measurements */
		sprintf(EngmodeEachInfo, " GSM Carrier RSSI: 0x%x", pEngmodeInfo->interRAT[dwnbcellidex].gsmRssi);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Cell Selection Rx Level: 0x%x", pEngmodeInfo->interRAT[dwnbcellidex].rxLev);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Path loss criterion parameter #1: 0x%x", pEngmodeInfo->interRAT[dwnbcellidex].C1);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Path loss criterion parameter #2: 0x%x", pEngmodeInfo->interRAT[dwnbcellidex].C2);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		/* PLMN/Cell Parameters */
		sprintf(EngmodeEachInfo, " Mobile Country Code: 0x%x", pEngmodeInfo->interRAT[dwnbcellidex].mcc);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Mobile Network Code: 0x%x", pEngmodeInfo->interRAT[dwnbcellidex].mnc);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Location Area Code: 0x%x", pEngmodeInfo->interRAT[dwnbcellidex].lac);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Cell Identity: 0x%x", pEngmodeInfo->interRAT[dwnbcellidex].ci);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

		sprintf(EngmodeEachInfo, " Absolute Radio Frequency Channel Number: 0x%x", pEngmodeInfo->interRAT[dwnbcellidex].arfcn);
		EM_INFO_OUTPUT( EngmodeEachInfo );
		memset(&EngmodeEachInfo, 0, EngmodeInfoEachMaxLen);

	}

	EM_INFO_OUTPUT("  End of the output for 3G UMTS engineering mode information.   " );
	EM_INFO_OUTPUT("/***************************************************/" );
	EM_INFO_OUTPUT("\r\n");
	return TRUE;
}
/****************************************************************
 *  F@: libEngmodeInfoOutput -  OUTPUT ENGINEERING MODE INFORMATION
 */
static BOOL libEngmodeInfoOutput(EngModeInfo *pbData )
{

	switch ( pbData->network )
	{
	case  EM_NETWORK_GSM:
		libEngmodeInfoOutput_GSM( (EngModeGSMData *)&(pbData->data.gsmData ), pbData->mode);
		break;

	case EM_NETWORK_UMTS:
		libEngmodeInfoOutput_UMTS( (EngModeUmtsData *)&(pbData->data.umtsData ), pbData->mode);
		break;
	default:
		break;
	}
	return TRUE;

}
/****************************************************************
 *  F@: processEngModeInfoInd - Process engine mode info ind msg
 */
static void processEngModeInfoInd(const void *pParam)
{
	CiDevPrimGsmEngmodeInfoInd * pGsmEngmodeInfoInd;
	CiDevEngModeGsmData* pGSMEngModeInfo;

	pGsmEngmodeInfoInd = (CiDevPrimGsmEngmodeInfoInd *)pParam;
	pGSMEngModeInfo = &(pGsmEngmodeInfoInd->info);

	stEngModeInfo.mode    = pGsmEngmodeInfoInd->mode;
	stEngModeInfo.network = EM_NETWORK_GSM;
	/* Serving cell information */
	memcpy( &(stEngModeInfo.data.gsmData.svcCellInfo), (void *)&pGSMEngModeInfo->svcCellInfo, sizeof(GSMServingCellInfo) );

	/* Neighbors number */
	if (pGSMEngModeInfo->numNCells > MAX_GSM_NEIGHBORING_CELLS)
	{
		ERRMSG("pGSMEngModeInfo->numNCells  %d larger than MAX_GSM_NEIGHBORING_CELLS \r\n", pGSMEngModeInfo->numNCells    );
		return;
	}

	stEngModeInfo.data.gsmData.numNCells = pGSMEngModeInfo->numNCells;

	/* Neighboring cell information */
	memcpy(stEngModeInfo.data.gsmData.nbCellInfo,
	       (void *)&pGSMEngModeInfo->nbCellInfo,
	       sizeof(GSMNeighboringCellInfo) * (pGSMEngModeInfo->numNCells) );
	libEngmodeInfoOutput(&stEngModeInfo);
	g_UMTS_EM_IND_SEQ = 0;
	return;
}
#endif
/****************************************************************
 *  F@: processEEMOPTInfoInd - Process engine mode info ind msg
 */
static void processEEMOPTInfoInd(const void *pParam)
{
	CiDevPrimGsmEngmodeInfoInd * pGsmEngmodeInfoInd;
	CiDevEngModeGsmData* pGSMEngModeInfo;
	CiDevGsmServingCellInfo * pSVCCellInfo;
	CiDevGPRSPTMInfo * pGPRSPTMInfo;
	char resBuf[500];
	char tempBuf[50];
	UINT8 cellNUM;

	pGsmEngmodeInfoInd = (CiDevPrimGsmEngmodeInfoInd *)pParam;
	pGSMEngModeInfo = &(pGsmEngmodeInfoInd->info);
	pSVCCellInfo = &(pGSMEngModeInfo->svcCellInfo);
	pGPRSPTMInfo = &(pGSMEngModeInfo->GPRSPTMInfo);

	// Part I: +EEMGINFOBASIC
	sprintf(resBuf, "+EEMGINFOBASIC: %d\r\n", pGsmEngmodeInfoInd->mode);
	ATRESP(IND_REQ_HANDLE, 0, 0, resBuf);

	// Part II: +EEMGINFOSVC
	// -6- mcc, mnc, lac, ci, nom, nco
	sprintf(resBuf, "+EEMGINFOSVC: %d, %d, %d, %d, %d, %d,", pSVCCellInfo->mcc,
	                                                         pSVCCellInfo->mnc,
	                                                         pSVCCellInfo->lac,
	                                                         pSVCCellInfo->ci,
	                                                         pSVCCellInfo->nom,
	                                                         pSVCCellInfo->nco);

	// -5- bsic, C1, C2, TA, TxPwr
	sprintf(tempBuf, " %d, %d, %d, %d, %d, ", pSVCCellInfo->bsic,
	                                          pSVCCellInfo->C1,
	                                          pSVCCellInfo->C2,
	                                          pSVCCellInfo->timingAdv,
	                                          pSVCCellInfo->TxPowerLevel);
	strcat(resBuf, tempBuf);
	// -5- RxSig, RxSigFull, RxSigSub, RxQualFull, RxQualSub
	sprintf(tempBuf, " %d, %d, %d, %d, %d, ", pSVCCellInfo->rxSigLevel,
	                                          pSVCCellInfo->rxSigLevelFull,
	                                          pSVCCellInfo->rxSigLevelSub,
	                                          pSVCCellInfo->rxQualityFull,
	                                          pSVCCellInfo->rxQualitySub);
	strcat(resBuf, tempBuf);
	// -7- ARFCB_tch, hopping_chnl, chnl_type, TS, PacketIdle, rac, arfcn
	sprintf(tempBuf, " %d, %d, %d, %d, %d, %d, %d, ", pSVCCellInfo->arfcnTch,
	                                                  pSVCCellInfo->hoppingChannel,
	                                                  pSVCCellInfo->ChType,
	                                                  pSVCCellInfo->timeSlot,
	                                                  pSVCCellInfo->isInPacketIdle,
	                                                  pSVCCellInfo->rac,
	                                                  pSVCCellInfo->arfcn);
	strcat(resBuf, tempBuf);
	// -7- bs_pa_mfrms, C31, C32, t3212, t3312, pbcch_support, EDGE_support
	sprintf(tempBuf, " %d, %d, %d, %d, %d, %d, %d, ", pSVCCellInfo->bs_pa_mfrms,
	                                                  pSVCCellInfo->C31,
	                                                  pSVCCellInfo->C32,
	                                                  pSVCCellInfo->t3212,
	                                                  pSVCCellInfo->t3312,
	                                                  pSVCCellInfo->pbcchSupport,
	                                                  pSVCCellInfo->EGPRSSupport);
	strcat(resBuf, tempBuf);
	// -6- ncc_permitted, rl_timeout, ho_count, ho_succ, chnl_access_count, chnl_access_succ_count
	sprintf(tempBuf, " %d, %d, %d, %d, %d, %d\r\n", pSVCCellInfo->nccPermitted,
	                                                pSVCCellInfo->RadioLinkTimeout,
	                                                pSVCCellInfo->hoCount,
	                                                pSVCCellInfo->hoSuccessCount,
	                                                pSVCCellInfo->chanAssCount,
	                                                pSVCCellInfo->chanAssSuccessCount);
	strcat(resBuf, tempBuf);
	ATRESP(IND_REQ_HANDLE, 0, 0, resBuf);

	// Part III: +EEMGINFOPS
	// -5- PS_attached, attach_type, service_type, tx_power, c_value
	sprintf(resBuf, "+EEMGINFOPS: %d, %d, %d, %d, %d, ", pGPRSPTMInfo->GPRSAttached,
	                                                     pGPRSPTMInfo->GPRSAttachType,
	                                                     pGPRSPTMInfo->gprsServiceType,
	                                                     pGPRSPTMInfo->txPower,
	                                                     pGPRSPTMInfo->cValue);

	// -6- ul_ts, dl_ts, ul_cs, dl_cs, ul_modulation, dl_modulation
	sprintf(tempBuf, "%d, %d, %d, %d, %d, %d, ", pGPRSPTMInfo->ulTimeSlot,
                                                 pGPRSPTMInfo->dlTimeSlot,
                                                 pGPRSPTMInfo->CodingSchemeUL,
                                                 pGPRSPTMInfo->CodingSchemeDL,
                                                 pGPRSPTMInfo->ulMod,
                                                 pGPRSPTMInfo->dlMod);

	strcat(resBuf, tempBuf);
	// -6- gmsk_cv_bep, 8psk_cv_bep, gmsk_mean_bep, 8psk_mean_bep, EDGE_bep_period, single_gmm_rej_cause
	sprintf(tempBuf, "%d, %d, %d, %d, %d, %d, ", pGPRSPTMInfo->egprsBep.gmskCvBep,
                                                 pGPRSPTMInfo->egprsBep.eightPskCvBep,
                                                 pGPRSPTMInfo->egprsBep.gmskMeanBep,
                                                 pGPRSPTMInfo->egprsBep.eightPskMeanBep,
                                                 pGPRSPTMInfo->EGPRSBEPPeriod,
                                                 pGPRSPTMInfo->IsSingleGmmRejectCause);

	strcat(resBuf, tempBuf);
	// -6- pdp_active_num, mac_mode, network_control, network_mode, EDGE_slq_measurement_mode, edge_status
	sprintf(tempBuf, "%d, %d, %d, %d, %d, %d\r\n", pGPRSPTMInfo->NumActivePDPContext,
                                                   pGPRSPTMInfo->MacMode,
                                                   pGPRSPTMInfo->NetworkControl,
                                                   pGPRSPTMInfo->NetworkMode,
                                                   pGPRSPTMInfo->EGPRSLQMeasurementMode,
                                                   pGPRSPTMInfo->egprsBep.status);

	strcat(resBuf, tempBuf);
	ATRESP(IND_REQ_HANDLE, 0, 0, resBuf);

	// Part IV: +EEMGINFONC

	/* Neighbors number */
	if (pGSMEngModeInfo->numNCells > MAX_GSM_NEIGHBORING_CELLS)
	{
		ERRMSG("pGSMEngModeInfo->numNCells  %d larger than MAX_GSM_NEIGHBORING_CELLS \r\n", pGSMEngModeInfo->numNCells);
		return;
	}
	for (cellNUM = 0; cellNUM < pGSMEngModeInfo->numNCells; cellNUM++)
	{
		// -6- nc_num, mcc, mnc, lac, rac, ci
		sprintf(resBuf, "+EEMGINFONC: %d, %d, %d, %d, %d, %d,", cellNUM,
		                       pGSMEngModeInfo->nbCellInfo[cellNUM].mcc,
		                       pGSMEngModeInfo->nbCellInfo[cellNUM].mnc,
		                       pGSMEngModeInfo->nbCellInfo[cellNUM].lac,
		                       pGSMEngModeInfo->nbCellInfo[cellNUM].rac,
		                       pGSMEngModeInfo->nbCellInfo[cellNUM].ci);

		// -7- rx_lv, bsic, C1, C2, arfcn, C31, C32
		sprintf(tempBuf, "%d, %d, %d, %d, %d, %d, %d\r\n",
		                       pGSMEngModeInfo->nbCellInfo[cellNUM].rxSigLevel,
		                       pGSMEngModeInfo->nbCellInfo[cellNUM].bsic,
		                       pGSMEngModeInfo->nbCellInfo[cellNUM].C1,
		                       pGSMEngModeInfo->nbCellInfo[cellNUM].C2,
		                       pGSMEngModeInfo->nbCellInfo[cellNUM].arfcn,
		                       pGSMEngModeInfo->nbCellInfo[cellNUM].C31,
		                       pGSMEngModeInfo->nbCellInfo[cellNUM].C32);
		strcat(resBuf, tempBuf);
		ATRESP(IND_REQ_HANDLE, 0, 0, resBuf);
	}

	// Part V: +EEMGINBFTM
	// -8- EngMode, mcc, mnc, lac, ci, bsic, C1, C2
	sprintf(resBuf, "+EEMGINBFTM: %d, %d, %d, %d, %d, %d, %d, %d, ", gEngModeRepOpt,
	                                                                 pSVCCellInfo->mcc,
	                                                                 pSVCCellInfo->mnc,
	                                                                 pSVCCellInfo->lac,
	                                                                 pSVCCellInfo->ci,
	                                                                 pSVCCellInfo->bsic,
	                                                                 pSVCCellInfo->C1,
	                                                                 pSVCCellInfo->C2);
	// -5- Timing advace, TxPowerLevel, rxSigLevel, rxSigLevelFull, rxSigLevelSub
	sprintf(tempBuf, "%d, %d, %d, %d, %d,", pSVCCellInfo->timingAdv,
	                                        pSVCCellInfo->TxPowerLevel,
	                                        pSVCCellInfo->rxSigLevel,
	                                        pSVCCellInfo->rxSigLevelFull,
	                                        pSVCCellInfo->rxSigLevelSub);
	strcat(resBuf, tempBuf);

	// -6- rxQualityFull, rxQualitySub, arfcnTch, hopping status, channel type, server timeslot
	sprintf(tempBuf, "%d, %d, %d, %d, %d, %d\r\n", pSVCCellInfo->rxQualityFull,
	                                               pSVCCellInfo->rxQualitySub,
	                                               pSVCCellInfo->arfcnTch,
	                                               pSVCCellInfo->hoppingChannel,
	                                               pSVCCellInfo->ChType,
	                                               pSVCCellInfo->timeSlot);
	strcat(resBuf, tempBuf);
	ATRESP(IND_REQ_HANDLE, 0, 0, resBuf);

	return;
}
#if 0
/****************************************************************
 *  F@: processUmtsEngModeScellInfoInd - Process UMTS engine mode info
 */
static void processUmtsEngModeScellInfoInd(const void *pParam)
{
	CiDevPrimUmtsEngmodeScellInfoInd * pUmtsEngmodeScellInfoInd;
	CiDevUmtsServingCellInfo *pUmtsServingCellInfo;

	pUmtsEngmodeScellInfoInd = (CiDevPrimUmtsEngmodeScellInfoInd *)pParam;
	pUmtsServingCellInfo = &(pUmtsEngmodeScellInfoInd->info);

	stEngModeInfo.mode    = pUmtsEngmodeScellInfoInd->mode;
	stEngModeInfo.network = EM_NETWORK_UMTS;

	memcpy( &(stEngModeInfo.data.umtsData.svcCellInfo),  pUmtsServingCellInfo, sizeof(UMTSServingCellInfo) );

	g_UMTS_EM_IND_SEQ++;
	if ( g_UMTS_EM_IND_SEQ == UMTS_EM_IND_NUM_MAX)
	{
		libEngmodeInfoOutput(&stEngModeInfo);
		DBGMSG("Output EM info after receiving UMTS CI_DEV_PRIM_UMTS_ENGMODE_SVCCELL_INFO_IND once \r\n");
		g_UMTS_EM_IND_SEQ = 0;
	}
	return;
}

/****************************************************************
 *  F@: processUmtsEngModeIntraFreqInfoInd - Process UMTS engine mode info
 */
static void processUmtsEngModeIntraFreqInfoInd(const void *pParam)
{
	CiDevPrimUmtsEngmodeIntraFreqInfoInd * pUmtsEngmodeIntraFreqInfoInd;

	pUmtsEngmodeIntraFreqInfoInd = (CiDevPrimUmtsEngmodeIntraFreqInfoInd *)pParam;

	/* Neighboring Cell information */
	if (pUmtsEngmodeIntraFreqInfoInd->numIntraFreq > MAX_UMTS_NEIGHBORING_CELLS)
	{
		ERRMSG("pUmtsEngmodeIntraFreqInfoInd->numIntraFreq %d larger than MAX_UMTS_NEIGHBORING_CELLS \r\n", pUmtsEngmodeIntraFreqInfoInd->numIntraFreq   );
		return;
	}

	stEngModeInfo.data.umtsData.numIntraFreq = pUmtsEngmodeIntraFreqInfoInd->numIntraFreq; /* Number of Intra-Frequency FDD Cells */

	/* Engineering Mode 3G (UMTS) Intra-Frequency/Inter-Frequency FDD Cell Information structure */
	memcpy(stEngModeInfo.data.umtsData.intraFreq,
	       pUmtsEngmodeIntraFreqInfoInd->intraFreq,
	       sizeof(UMTSFddNeighborInfo) * (pUmtsEngmodeIntraFreqInfoInd->numIntraFreq)  );

	g_UMTS_EM_IND_SEQ++;
	if ( g_UMTS_EM_IND_SEQ == UMTS_EM_IND_NUM_MAX)
	{
		libEngmodeInfoOutput(&stEngModeInfo);
		DBGMSG("Output EM info after receiving UMTS CI_DEV_PRIM_UMTS_ENGMODE_INTRAFREQ_INFO_IND once \r\n");
		g_UMTS_EM_IND_SEQ = 0;
	}

	return;
}

/****************************************************************
 *  F@: processUmtsEngModeInterFreqInfoInd - Process UMTS engine mode info
 */
static void processUmtsEngModeInterFreqInfoInd(const void *pParam)
{
	CiDevPrimUmtsEngmodeInterFreqInfoInd * pUmtsEngmodeInterFreqInfoInd;

	pUmtsEngmodeInterFreqInfoInd = (CiDevPrimUmtsEngmodeInterFreqInfoInd *)pParam;

	/* Number of Inter-Frequency FDD Cells */
	if (pUmtsEngmodeInterFreqInfoInd->numInterFreq > MAX_UMTS_NEIGHBORING_CELLS)
	{
		ERRMSG("pUmtsEngmodeInterFreqInfoInd->numInterFreq  %d larger than MAX_UMTS_NEIGHBORING_CELLS \r\n", pUmtsEngmodeInterFreqInfoInd->numInterFreq );
		return;
	}
	stEngModeInfo.data.umtsData.numInterFreq = pUmtsEngmodeInterFreqInfoInd->numInterFreq; /* Number of Intra-Frequency FDD Cells */

	memcpy(stEngModeInfo.data.umtsData.intraFreq,
	       pUmtsEngmodeInterFreqInfoInd->interFreq,
	       sizeof(UMTSFddNeighborInfo) * (pUmtsEngmodeInterFreqInfoInd->numInterFreq)  );

	g_UMTS_EM_IND_SEQ++;
	if ( g_UMTS_EM_IND_SEQ == UMTS_EM_IND_NUM_MAX)
	{
		libEngmodeInfoOutput(&stEngModeInfo);
		DBGMSG("Output EM info after receiving UMTS CI_DEV_PRIM_UMTS_ENGMODE_INTERFREQ_INFO_IND once \r\n");

		g_UMTS_EM_IND_SEQ = 0;
	}

	return;
}

/****************************************************************
 *  F@: processUmtsEngModeInterRatInfoInd - Process UMTS engine mode info
 */
static void processUmtsEngModeInterRatInfoInd(const void *pParam)
{
	CiDevPrimUmtsEngmodeInterRatInfoInd * pUmtsEngmodeInterRatInfoInd;

	pUmtsEngmodeInterRatInfoInd = (CiDevPrimUmtsEngmodeInterRatInfoInd *)pParam;

	/* Number of Inter-Frequency FDD Cells */
	if (pUmtsEngmodeInterRatInfoInd->numInterRAT > MAX_UMTS_NEIGHBORING_CELLS)
	{
		ERRMSG("pUmtsEngmodeInterRatInfoInd->numInterRAT  %d larger than MAX_UMTS_NEIGHBORING_CELLS \r\n", pUmtsEngmodeInterRatInfoInd->numInterRAT );
		return;
	}
	stEngModeInfo.data.umtsData.numInterRAT = pUmtsEngmodeInterRatInfoInd->numInterRAT; /* Number of Inter-RAT GSM Cells */

	memcpy(stEngModeInfo.data.umtsData.interRAT,
	       pUmtsEngmodeInterRatInfoInd->interRAT,
	       sizeof(UMTSGsmNeighborInfo) * (pUmtsEngmodeInterRatInfoInd->numInterRAT)  );

	g_UMTS_EM_IND_SEQ++;
	if ( g_UMTS_EM_IND_SEQ == UMTS_EM_IND_NUM_MAX)
	{
		libEngmodeInfoOutput(&stEngModeInfo);
		DBGMSG("Output EM info after receiving UMTS CI_DEV_PRIM_UMTS_ENGMODE_INTERRAT_INFO_IND once \r\n");
		g_UMTS_EM_IND_SEQ = 0;
	}
	return;
}
#endif
/****************************************************************
 *  F@: processEMumtsINFOSVCInfoInd - Process UMTS engine mode info
 */
static void processEMumtsINFOSVCInfoInd(const void *pParam)
{
	CiDevPrimUmtsEngmodeScellInfoInd * pUmtsEngmodeScellInfoInd;
	CiDevUmtsServingCellInfo *pUmtsServingCellInfo;
	char resBuf[500];
	char tempBuf[80];

	pUmtsEngmodeScellInfoInd = (CiDevPrimUmtsEngmodeScellInfoInd *)pParam;
	pUmtsServingCellInfo = &(pUmtsEngmodeScellInfoInd->info);

	g_UMTS_EM_IND_SEQ++;
	//if ( g_UMTS_EM_IND_SEQ == UMTS_EM_IND_NUM_MAX)
	{
		DBGMSG("Output EM info after receiving UMTS CI_DEV_PRIM_UMTS_ENGMODE_SVCCELL_INFO_IND once \r\n");
		g_UMTS_EM_IND_SEQ = 0;

		// +EEMUMTSSVC

		sprintf(resBuf, "+EEMUMTSSVC:");

		// -4- Mode, sCMeasPresent, sCParamPresent, ueOpStatusPresent
		sprintf(tempBuf, "%d, %d, %d, %d,", pUmtsEngmodeScellInfoInd->mode,
		                                    pUmtsServingCellInfo->sCellMeasPresent,
		                                    pUmtsServingCellInfo->sCellParamPresent,
		                                    pUmtsServingCellInfo->ueOpStatusPresent);
		strcat(resBuf, tempBuf);

		if (pUmtsServingCellInfo->sCellMeasPresent)
		{
#ifndef TD_SUPPORT
			// -6- cpichRSCP, utraRssi, cpichEcN0, sQual, sRxLev, txPower
			sprintf(tempBuf, "%d, %d, %d, %d, %d, %d, ", pUmtsServingCellInfo->sCellMeas.cpichRSCP,
			                                             pUmtsServingCellInfo->sCellMeas.utraRssi,
			                                             pUmtsServingCellInfo->sCellMeas.cpichEcN0,
			                                             pUmtsServingCellInfo->sCellMeas.sQual,
			                                             pUmtsServingCellInfo->sCellMeas.sRxLev,
			                                             pUmtsServingCellInfo->sCellMeas.txPower);
#else
			// -4- pccpchRSCP, utraRssi, sRxLev, txPower
			sprintf(tempBuf, "%d, %d, %d, %d, ", pUmtsServingCellInfo->sCellMeas.pccpchRSCP,
			                                     pUmtsServingCellInfo->sCellMeas.utraRssi,
			                                     pUmtsServingCellInfo->sCellMeas.sRxLev,
			                                     pUmtsServingCellInfo->sCellMeas.txPower);

#endif
			strcat(resBuf, tempBuf);
		}
		if (pUmtsServingCellInfo->sCellParamPresent)
		{
			// -6- rac, nom, mcc, mnc, lac, ci
			sprintf(tempBuf, "%d, %d, %d, %d, %d, %d, ", pUmtsServingCellInfo->sCellParam.rac,
			                                             pUmtsServingCellInfo->sCellParam.nom,
			                                             pUmtsServingCellInfo->sCellParam.mcc,
			                                             pUmtsServingCellInfo->sCellParam.mnc,
			                                             pUmtsServingCellInfo->sCellParam.lac,
			                                             pUmtsServingCellInfo->sCellParam.ci);
			strcat(resBuf, tempBuf);
#ifndef TD_SUPPORT

			// -7- uraId, psc, arfcn, t3212, t3312, hcsUsed, attDetAllowed
			sprintf(tempBuf, "%d, %d, %d, %d, %d, %d, %d, ", pUmtsServingCellInfo->sCellParam.uraId,
			                                             pUmtsServingCellInfo->sCellParam.psc,
			                                             pUmtsServingCellInfo->sCellParam.arfcn,
			                                             pUmtsServingCellInfo->sCellParam.t3212,
			                                             pUmtsServingCellInfo->sCellParam.t3312,
			                                             pUmtsServingCellInfo->sCellParam.hcsUsed,
			                                             pUmtsServingCellInfo->sCellParam.attDetAllowed);
			strcat(resBuf, tempBuf);
#else
			// -7- uraId, cellParameterId, arfcn, t3212, t3312, hcsUsed, attDetAllowed
			sprintf(tempBuf, "%d, %d, %d, %d, %d, %d, %d, ", pUmtsServingCellInfo->sCellParam.uraId,
			                                             pUmtsServingCellInfo->sCellParam.cellParameterId,
			                                             pUmtsServingCellInfo->sCellParam.arfcn,
			                                             pUmtsServingCellInfo->sCellParam.t3212,
			                                             pUmtsServingCellInfo->sCellParam.t3312,
			                                             pUmtsServingCellInfo->sCellParam.hcsUsed,
			                                             pUmtsServingCellInfo->sCellParam.attDetAllowed);
			strcat(resBuf, tempBuf);
#endif

			// -5- csDrxCycleLen, psDrxCycleLen, utranDrxCycleLen, HSDPASupport, HSUPASupport
			sprintf(tempBuf, "%d, %d, %d, %d, %d, ", pUmtsServingCellInfo->sCellParam.csDrxCycleLen,
			                                             pUmtsServingCellInfo->sCellParam.psDrxCycleLen,
			                                             pUmtsServingCellInfo->sCellParam.utranDrxCycleLen,
			                                             pUmtsServingCellInfo->sCellParam.HSDPASupport,
			                                             pUmtsServingCellInfo->sCellParam.HSUPASupport);
			strcat(resBuf, tempBuf);
		}
		if (pUmtsServingCellInfo->ueOpStatusPresent)
		{
			// -4- rrcState, numLinks, srncId, sRnti
			sprintf(tempBuf, "%d, %d, %d, %d, ", pUmtsServingCellInfo->ueOpStatus.rrcState,
			                                             pUmtsServingCellInfo->ueOpStatus.numLinks,
			                                             pUmtsServingCellInfo->ueOpStatus.srncId,
			                                             pUmtsServingCellInfo->ueOpStatus.sRnti);
			strcat(resBuf, tempBuf);

			// -6- algPresent, cipherAlg, cipherOn, algPresent, cipherAlg, cipherOn
			sprintf(tempBuf, "%d, %d, %d, %d, %d, %d,", pUmtsServingCellInfo->ueOpStatus.csCipherInfo.algPresent,
			                                             pUmtsServingCellInfo->ueOpStatus.csCipherInfo.cipherAlg,
			                                             pUmtsServingCellInfo->ueOpStatus.csCipherInfo.cipherOn,
			                                             pUmtsServingCellInfo->ueOpStatus.psCipherInfo.algPresent,
			                                             pUmtsServingCellInfo->ueOpStatus.psCipherInfo.cipherAlg,
			                                             pUmtsServingCellInfo->ueOpStatus.psCipherInfo.cipherOn);
			strcat(resBuf, tempBuf);

			// -8- HSDPAActive, HSUPAActive, MccLastRegisteredNetwork, MncLastRegisteredNetwork
			//     TMSI, PTMSI, IsSingleMmRejectCause, IsSingleGmmRejectCause
			sprintf(tempBuf, "%d, %d, %d, %d, %d, %d, %d, %d,", pUmtsServingCellInfo->ueOpStatus.HSDPAActive,
			                                             pUmtsServingCellInfo->ueOpStatus.HSUPAActive,
			                                             pUmtsServingCellInfo->ueOpStatus.MccLastRegisteredNetwork,
			                                             pUmtsServingCellInfo->ueOpStatus.MncLastRegisteredNetwork,
			                                             pUmtsServingCellInfo->ueOpStatus.TMSI,
			                                             pUmtsServingCellInfo->ueOpStatus.PTMSI,
			                                             pUmtsServingCellInfo->ueOpStatus.IsSingleMmRejectCause,
			                                             pUmtsServingCellInfo->ueOpStatus.IsSingleGmmRejectCause);
			strcat(resBuf, tempBuf);
			// -9- MMRejectCause, GMMRejectCause, mmState, gmmState, gprsReadyState
			//     readyTimerValueInSecs, NumActivePDPContext, ULThroughput, DLThroughput
			sprintf(tempBuf, "%d, %d, %d, %d, %d, %d, %d, %d, %d", pUmtsServingCellInfo->ueOpStatus.MMRejectCause,
			                                             pUmtsServingCellInfo->ueOpStatus.GMMRejectCause,
			                                             pUmtsServingCellInfo->ueOpStatus.mmState,
			                                             pUmtsServingCellInfo->ueOpStatus.gmmState,
			                                             pUmtsServingCellInfo->ueOpStatus.gprsReadyState,
			                                             pUmtsServingCellInfo->ueOpStatus.readyTimerValueInSecs,
			                                             pUmtsServingCellInfo->ueOpStatus.NumActivePDPContext,
			                                             pUmtsServingCellInfo->ueOpStatus.ULThroughput,
			                                             pUmtsServingCellInfo->ueOpStatus.DLThroughput);
			strcat(resBuf, tempBuf);
		}

		strcat(resBuf, "\r\n");

		ATRESP(IND_REQ_HANDLE, 0, 0, resBuf);
	}

	return;
}

/****************************************************************
 *  F@: processEMUmtsIntraFreqInfoInd - Process UMTS engine mode info
 */
static void processEMUmtsIntraFreqInfoInd(const void *pParam)
{
	CiDevPrimUmtsEngmodeIntraFreqInfoInd * pUmtsEngmodeIntraFreqInfoInd;
	UINT8 index;
	char resBuf[200];
	char tempBuf[50];

	pUmtsEngmodeIntraFreqInfoInd = (CiDevPrimUmtsEngmodeIntraFreqInfoInd *)pParam;

	/* Neighboring Cell information */
	if (pUmtsEngmodeIntraFreqInfoInd->numIntraFreq > MAX_UMTS_NEIGHBORING_CELLS)
	{
		ERRMSG("pUmtsEngmodeIntraFreqInfoInd->numIntraFreq %d larger than MAX_UMTS_NEIGHBORING_CELLS \r\n", pUmtsEngmodeIntraFreqInfoInd->numIntraFreq   );
		return;
	}

	g_UMTS_EM_IND_SEQ++;
	//if ( g_UMTS_EM_IND_SEQ == UMTS_EM_IND_NUM_MAX)
	{
		DBGMSG("Output EM info after receiving UMTS CI_DEV_PRIM_UMTS_ENGMODE_INTRAFREQ_INFO_IND once \r\n");
		g_UMTS_EM_IND_SEQ = 0;

		// +EEMUMTSINTRA

		for (index = 0; index < pUmtsEngmodeIntraFreqInfoInd->numIntraFreq; index++)
		{
			sprintf(resBuf, "+EEMUMTSINTRA: %d, ", index);

#ifndef TD_SUPPORT
			// -5- cpichRSCP, utraRssi, cpichEcN0, sQual, sRxLev
			sprintf(tempBuf, "%d, %d, %d, %d, %d, ", pUmtsEngmodeIntraFreqInfoInd->intraFreq[index].cpichRSCP,
			                                  pUmtsEngmodeIntraFreqInfoInd->intraFreq[index].utraRssi,
			                                  pUmtsEngmodeIntraFreqInfoInd->intraFreq[index].cpichEcN0,
			                                  pUmtsEngmodeIntraFreqInfoInd->intraFreq[index].sQual,
			                                  pUmtsEngmodeIntraFreqInfoInd->intraFreq[index].sRxLev);
			strcat(resBuf, tempBuf);

			// -6- mcc, mnc, lac, ci, arfcn, psc
			sprintf(tempBuf, "%d, %d, %d, %d, %d, %d", pUmtsEngmodeIntraFreqInfoInd->intraFreq[index].mcc,
			                                  pUmtsEngmodeIntraFreqInfoInd->intraFreq[index].mnc,
			                                  pUmtsEngmodeIntraFreqInfoInd->intraFreq[index].lac,
			                                  pUmtsEngmodeIntraFreqInfoInd->intraFreq[index].ci,
			                                  pUmtsEngmodeIntraFreqInfoInd->intraFreq[index].arfcn,
			                                  pUmtsEngmodeIntraFreqInfoInd->intraFreq[index].psc);
			strcat(resBuf, tempBuf);
#else
			// -3- pccpchRSCP, utraRssi, sRxLev
			sprintf(tempBuf, "%d, %d, %d, ", pUmtsEngmodeIntraFreqInfoInd->intraFreq[index].pccpchRSCP,
			                                  pUmtsEngmodeIntraFreqInfoInd->intraFreq[index].utraRssi,
			                                  pUmtsEngmodeIntraFreqInfoInd->intraFreq[index].sRxLev);
			strcat(resBuf, tempBuf);
			// -6- mcc, mnc, lac, ci, arfcn, cellParameterId
			sprintf(tempBuf, "%d, %d, %d, %d, %d, %d", pUmtsEngmodeIntraFreqInfoInd->intraFreq[index].mcc,
											  pUmtsEngmodeIntraFreqInfoInd->intraFreq[index].mnc,
											  pUmtsEngmodeIntraFreqInfoInd->intraFreq[index].lac,
											  pUmtsEngmodeIntraFreqInfoInd->intraFreq[index].ci,
											  pUmtsEngmodeIntraFreqInfoInd->intraFreq[index].arfcn,
											  pUmtsEngmodeIntraFreqInfoInd->intraFreq[index].cellParameterId);
			strcat(resBuf, tempBuf);
#endif
			strcat(resBuf, "\r\n");

			ATRESP(IND_REQ_HANDLE, 0, 0, resBuf);

		}
	}

	return;
}

/****************************************************************
 *  F@: processEMUmtsInterFreqInfoInd - Process UMTS engine mode info
 */
static void processEMUmtsInterFreqInfoInd(const void *pParam)
{
	CiDevPrimUmtsEngmodeInterFreqInfoInd * pUmtsEngmodeInterFreqInfoInd;
	UINT8 index;
	char resBuf[200];
	char tempBuf[50];

	pUmtsEngmodeInterFreqInfoInd = (CiDevPrimUmtsEngmodeInterFreqInfoInd *)pParam;

	/* Number of Inter-Frequency FDD Cells */
	if (pUmtsEngmodeInterFreqInfoInd->numInterFreq > MAX_UMTS_NEIGHBORING_CELLS)
	{
		ERRMSG("pUmtsEngmodeInterFreqInfoInd->numInterFreq  %d larger than MAX_UMTS_NEIGHBORING_CELLS \r\n", pUmtsEngmodeInterFreqInfoInd->numInterFreq );
		return;
	}
	g_UMTS_EM_IND_SEQ++;
	//if ( g_UMTS_EM_IND_SEQ == UMTS_EM_IND_NUM_MAX)
	{
		DBGMSG("Output EM info after receiving UMTS CI_DEV_PRIM_UMTS_ENGMODE_INTERFREQ_INFO_IND once \r\n");
		g_UMTS_EM_IND_SEQ = 0;

		// +EEMUMTSINTER
		for (index = 0; index < pUmtsEngmodeInterFreqInfoInd->numInterFreq; index++)
		{
			sprintf(resBuf, "+EEMUMTSINTER: %d, ", index);

#ifndef TD_SUPPORT
			// -5- cpichRSCP, utraRssi, cpichEcN0, sQual, sRxLev
			sprintf(tempBuf, "%d, %d, %d, %d, %d, ", pUmtsEngmodeInterFreqInfoInd->interFreq[index].cpichRSCP,
			                                  pUmtsEngmodeInterFreqInfoInd->interFreq[index].utraRssi,
			                                  pUmtsEngmodeInterFreqInfoInd->interFreq[index].cpichEcN0,
			                                  pUmtsEngmodeInterFreqInfoInd->interFreq[index].sQual,
			                                  pUmtsEngmodeInterFreqInfoInd->interFreq[index].sRxLev);
			strcat(resBuf, tempBuf);

			// -6- mcc, mnc, lac, ci, arfcn, psc
			sprintf(tempBuf, "%d, %d, %d, %d, %d, %d", pUmtsEngmodeInterFreqInfoInd->interFreq[index].mcc,
			                                  pUmtsEngmodeInterFreqInfoInd->interFreq[index].mnc,
			                                  pUmtsEngmodeInterFreqInfoInd->interFreq[index].lac,
			                                  pUmtsEngmodeInterFreqInfoInd->interFreq[index].ci,
			                                  pUmtsEngmodeInterFreqInfoInd->interFreq[index].arfcn,
			                                  pUmtsEngmodeInterFreqInfoInd->interFreq[index].psc);
			strcat(resBuf, tempBuf);
#else
			// -3- pccpchRSCP, utraRssi, sRxLev
			sprintf(tempBuf, "%d, %d, %d, ", pUmtsEngmodeInterFreqInfoInd->interFreq[index].pccpchRSCP,
			                                  pUmtsEngmodeInterFreqInfoInd->interFreq[index].utraRssi,
			                                  pUmtsEngmodeInterFreqInfoInd->interFreq[index].sRxLev);
			strcat(resBuf, tempBuf);
			// -6- mcc, mnc, lac, ci, arfcn, cellParameterId
			sprintf(tempBuf, "%d, %d, %d, %d, %d, %d", pUmtsEngmodeInterFreqInfoInd->interFreq[index].mcc,
											  pUmtsEngmodeInterFreqInfoInd->interFreq[index].mnc,
											  pUmtsEngmodeInterFreqInfoInd->interFreq[index].lac,
											  pUmtsEngmodeInterFreqInfoInd->interFreq[index].ci,
											  pUmtsEngmodeInterFreqInfoInd->interFreq[index].arfcn,
											  pUmtsEngmodeInterFreqInfoInd->interFreq[index].cellParameterId);
			strcat(resBuf, tempBuf);
#endif
			strcat(resBuf, "\r\n");

			ATRESP(IND_REQ_HANDLE, 0, 0, resBuf);
		}

	}

	return;
}

/****************************************************************
 *  F@: processEMUmtsInterRatInfoInd - Process UMTS engine mode info
 */
static void processEMUmtsInterRatInfoInd(const void *pParam)
{
	CiDevPrimUmtsEngmodeInterRatInfoInd * pUmtsEngmodeInterRatInfoInd;
	UINT8 index;
	char resBuf[200];
	char tempBuf[50];

	pUmtsEngmodeInterRatInfoInd = (CiDevPrimUmtsEngmodeInterRatInfoInd *)pParam;

	/* Number of Inter-Frequency FDD Cells */
	if (pUmtsEngmodeInterRatInfoInd->numInterRAT > MAX_UMTS_NEIGHBORING_CELLS)
	{
		ERRMSG("pUmtsEngmodeInterRatInfoInd->numInterRAT  %d larger than MAX_UMTS_NEIGHBORING_CELLS \r\n", pUmtsEngmodeInterRatInfoInd->numInterRAT );
		return;
	}

	g_UMTS_EM_IND_SEQ++;
	//if ( g_UMTS_EM_IND_SEQ == UMTS_EM_IND_NUM_MAX)
	{
		DBGMSG("Output EM info after receiving UMTS CI_DEV_PRIM_UMTS_ENGMODE_INTERRAT_INFO_IND once \r\n");
		g_UMTS_EM_IND_SEQ = 0;

		// +EEMUMTSINTERRAT:
		for (index = 0; index < pUmtsEngmodeInterRatInfoInd->numInterRAT; index++)
		{
			sprintf(resBuf, "+EEMUMTSINTERRAT: %d, ", index);

			// -4- gsmRssi, rxLev, C1, C2
			sprintf(tempBuf, "%d, %d, %d, %d, ", pUmtsEngmodeInterRatInfoInd->interRAT[index].gsmRssi,
			                                  pUmtsEngmodeInterRatInfoInd->interRAT[index].rxLev,
			                                  pUmtsEngmodeInterRatInfoInd->interRAT[index].C1,
			                                  pUmtsEngmodeInterRatInfoInd->interRAT[index].C2);
			strcat(resBuf, tempBuf);

			// -6- mcc, mnc, lac, ci, arfcn, bsic
			sprintf(tempBuf, "%d, %d, %d, %d, %d, %d\r\n", pUmtsEngmodeInterRatInfoInd->interRAT[index].mcc,
			                                  pUmtsEngmodeInterRatInfoInd->interRAT[index].mnc,
			                                  pUmtsEngmodeInterRatInfoInd->interRAT[index].lac,
			                                  pUmtsEngmodeInterRatInfoInd->interRAT[index].ci,
			                                  pUmtsEngmodeInterRatInfoInd->interRAT[index].arfcn,
			                                  pUmtsEngmodeInterRatInfoInd->interRAT[index].bsic);
			strcat(resBuf, tempBuf);

			ATRESP(IND_REQ_HANDLE, 0, 0, resBuf);
		}
	}
	return;
}

/****************************************************************
 *  F@: processNwMonitorInfoInd - Process network monitor info
 */
static void processNwMonitorInfoInd(const void *pParam)
{
	char respBuffer[255];
	CiDevPrimNetworkMonitorInfoInd *pCiDevPrimNetworkMonitorInfoInd;

	pCiDevPrimNetworkMonitorInfoInd = (CiDevPrimNetworkMonitorInfoInd*)pParam;

	sprintf(respBuffer, "+SNETIND:%d\r\n", pCiDevPrimNetworkMonitorInfoInd->info.mode);
	ATRESP(IND_REQ_HANDLE, 0, 0, respBuffer);
	return;
}

/****************************************************************
 *  F@: processLpNwdlMsgInfoInd - Process RRC/RRLP positioning-related
 *      message
 */
void processLpNwdlMsgInfoInd(const void *pParam)
{
	char *respBuffer;//[CI_DEV_MAX_APGS_MSG_SIZE*2+50+2];
	char temp[50];
	CiDevPrimLpNwdlMsgInd *pCiDevPrimLpNwdlMsgInd;
	extern void libConvertNumToHex(char *inData, int inLen, char *outData);

	pCiDevPrimLpNwdlMsgInd = (CiDevPrimLpNwdlMsgInd *)pParam;

	/* Get msg_data in HEX format */
	respBuffer = (char *)malloc(sizeof(char) * (CI_DEV_MAX_APGS_MSG_SIZE*2+50+2));
	if (respBuffer == NULL)
		return;

	sprintf(respBuffer, "+LPNWDL:%d,", pCiDevPrimLpNwdlMsgInd->BearerType);

	libConvertNumToHex((char *)pCiDevPrimLpNwdlMsgInd->msg_data,
					   pCiDevPrimLpNwdlMsgInd->msg_size, respBuffer);

	sprintf(temp, ",%d,%d,%d,%d\r\n",  pCiDevPrimLpNwdlMsgInd->msg_size,
										pCiDevPrimLpNwdlMsgInd->sessionType,
										pCiDevPrimLpNwdlMsgInd->RrcState,
										pCiDevPrimLpNwdlMsgInd->count);

	strcat(respBuffer, temp);
	ATRESP(IND_REQ_HANDLE, 0, 0, respBuffer);
	free(respBuffer);

	return;
}

/****************************************************************
 *	F@: processLpRRCStateInd - Process RRC/RRLP state change
 */
static void processLpRRCStateInd(const void *pParam)
{
	char respBuffer[50];
	CiDevPrimLpRrcStateInd *pCiDevPrimLpRrcStateInd;

	pCiDevPrimLpRrcStateInd = (CiDevPrimLpRrcStateInd *)pParam;

	sprintf(respBuffer, "+LPSTATE:%d, %d\r\n",
										pCiDevPrimLpRrcStateInd->bearer_type,
										pCiDevPrimLpRrcStateInd->rrc_state);

	ATRESP(IND_REQ_HANDLE, 0, 0, respBuffer);
	return;
}

/****************************************************************
 *	F@: processLpMeasTerminateInd - Erase all assistance data
 */
static void processLpMeasTerminateInd(const void *pParam)
{
	char respBuffer[50];
	CiDevPrimLpResetStoreUePosInd *pCiDevPrimLpResetStoreUePosInd;

	pCiDevPrimLpResetStoreUePosInd = (CiDevPrimLpResetStoreUePosInd *)pParam;

	sprintf(respBuffer, "+LPMEAST:%d\r\n",
										pCiDevPrimLpResetStoreUePosInd->bearer_type);

	ATRESP(IND_REQ_HANDLE, 0, 0, respBuffer);
	return;
}

/****************************************************************
 *	F@: processLpResetUEPosInd - Process measurement termination
 */
static void processLpResetUEPosInd(const void *pParam)
{
	char respBuffer[50];
	CiDevPrimLpMeasTerminateInd *pCiDevPrimLpMeasTerminateInd;

	pCiDevPrimLpMeasTerminateInd = (CiDevPrimLpMeasTerminateInd *)pParam;

	sprintf(respBuffer, "+LPRESET:%d\r\n",
										pCiDevPrimLpMeasTerminateInd->bearer_type);

	ATRESP(IND_REQ_HANDLE, 0, 0, respBuffer);
	return;
}

#ifdef AT_CUSTOMER_HTC
/****************************************************************
 *	F@: processRadioLinkFailureInd - Process radio link failure
 */
static void processRadioLinkFailureInd(const void *pParam)
{
	char respBuffer[50];
	CiDevPrimRadioLinkFailureInd *pCiDevPrimRadioLinkFailureInd;

	pCiDevPrimRadioLinkFailureInd = (CiDevPrimRadioLinkFailureInd *)pParam;

	sprintf(respBuffer, "%s: %X\r\n", SPEC_ERROR_CODE_INDICATION_PREFIX, ATCI_HTC_ERROR_CLASS_RADIO_LINK_FAILURE);

	ATRESP(IND_REQ_HANDLE, 0, 0, respBuffer);
	return;
}
#endif

/****************************************************************
 *	F@: processProtocolStatusChangedInd - Process protocol status change
 */
static void processProtocolStatusChangedInd(const void *pParam)
{
	char respBuffer[50];
	CiDevPrimProtocolStatusChangedInd *pCiDevPrimProtocolStatusChangedInd;

	pCiDevPrimProtocolStatusChangedInd = (CiDevPrimProtocolStatusChangedInd *)pParam;

	snprintf(respBuffer, utlNumberOf(respBuffer), "^DEELS:%d\r\n", pCiDevPrimProtocolStatusChangedInd->status);

	ATRESP(IND_REQ_HANDLE, 0, 0, respBuffer);
	return;
}
/****************************************************************
 *	F@: processEventReportInd - Process event report
 */
static void processEventReportInd(const void *pParam)
{
	char respBuffer[50];
	CiDevPrimEventReportInd *pCiDevPrimEventReportInd;
	UINT32 systemTick;
	if(DEV_GetSystemTick(&systemTick) == FALSE)
	{
		DBGMSG("get system tick error\n");
		return;
	}
	pCiDevPrimEventReportInd = (CiDevPrimEventReportInd *)pParam;
	snprintf(respBuffer, utlNumberOf(respBuffer), "^DEVEI:%u,%u,%u\r\n", systemTick, pCiDevPrimEventReportInd->operationType, pCiDevPrimEventReportInd->eventId);

	ATRESP(IND_REQ_HANDLE, 0, 0, respBuffer);
	return;
}
static void processTdWirelseeParam(const CiDevTddData *pCiDevTddData, char *respBuffer, UINT16 *pbufferLeft)
{
	if(pCiDevTddData == NULL || respBuffer == NULL || pbufferLeft == NULL)
		return;
	//process Timeslot ISCP
	if(pCiDevTddData->iscp.num != 0)
	{
		DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_TDD_TS_TSCP, (UINT8*)&pCiDevTddData->iscp, sizeof(pCiDevTddData->iscp), respBuffer, pbufferLeft);
	}
	//process DPCH RSCP
	if(pCiDevTddData->rscp.num != 0)
	{
		DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_TDD_DPCH_RSCP, (UINT8*)&pCiDevTddData->rscp, sizeof(pCiDevTddData->rscp), respBuffer, pbufferLeft);
	}
	//process UE Timeslot Transmitted Power
	if(pCiDevTddData->txPower.num != 0)
	{
		DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_TDD_TX_POWER, (UINT8*)&pCiDevTddData->txPower, sizeof(pCiDevTddData->txPower), respBuffer, pbufferLeft);
	}
	//process Scell P-CCPCH RSCP
	if(pCiDevTddData->sCellPccphRscp <= CI_DEV_WIRELSS_PARAM_TDD_SCELL_PCCPH_RSCP_VAL_MAX)
	{
		DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_TDD_SCELL_PCCPH_RSCP, (UINT8*)&pCiDevTddData->sCellPccphRscp, sizeof(pCiDevTddData->sCellPccphRscp), respBuffer, pbufferLeft);
	}
	//process Scell Cell identifier
	if(pCiDevTddData->sCellId <= CI_DEV_WIRELSS_PARAM_TDD_SCELL_ID_VAL_MAX)
	{
		DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_TDD_SCELL_ID, (UINT8*)&pCiDevTddData->sCellId, sizeof(pCiDevTddData->sCellId), respBuffer, pbufferLeft);
	}
	//process Scell Cell parameter id
	if(pCiDevTddData->sCellParamId <= CI_DEV_WIRELSS_PARAM_TDD_SCELL_PARAM_ID_VAL_MAX)
	{
		DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_TDD_SCELL_PARAM_ID, (UINT8*)&pCiDevTddData->sCellParamId, sizeof(pCiDevTddData->sCellParamId), respBuffer, pbufferLeft);
	}
	//process Scell UARFCN
	if(pCiDevTddData->sCellUarfcn <= CI_DEV_WIRELSS_PARAM_TDD_SCELL_UARFCN_VAL_MAX)
	{
		DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_TDD_SCELL_UARFCN, (UINT8*)&pCiDevTddData->sCellUarfcn, sizeof(pCiDevTddData->sCellUarfcn), respBuffer, pbufferLeft);
	}
	//process Carrier RSSI
	if(pCiDevTddData->sCellUtraRssi <= CI_DEV_WIRELSS_PARAM_TDD_SCELL_UTRARSSI_VAL_MAX)
	{
		DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_TDD_SCELL_UTRARSSI, (UINT8*)&pCiDevTddData->sCellUtraRssi, sizeof(pCiDevTddData->sCellUtraRssi), respBuffer, pbufferLeft);
	}
	//process Neighbor cell parameter list
	if(pCiDevTddData->nbCellInfo.numNcellTdd != 0 || pCiDevTddData->nbCellInfo.numNcellGsm != 0)
	{
		DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_TDD_NCELL_PARAM_LIST, (UINT8*)&pCiDevTddData->nbCellInfo, sizeof(pCiDevTddData->nbCellInfo), respBuffer, pbufferLeft);
	}
	//process H-RNTI
	if(pCiDevTddData->hRnti.flag == TRUE)
	{
		DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_TDD_HRNTI, (UINT8*)&pCiDevTddData->hRnti, sizeof(pCiDevTddData->hRnti), respBuffer, pbufferLeft);
	}
	//process Delicated Phych parameter
	if(pCiDevTddData->dedicatedInfo.numPhych != 0)
	{
		DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_TDD_DEDICATED_PHYCH_PARAM, (UINT8*)&pCiDevTddData->dedicatedInfo, sizeof(pCiDevTddData->dedicatedInfo), respBuffer, pbufferLeft);
	}
	//process E-RNTI
	if(pCiDevTddData->eRnti.flag == TRUE)
	{
		DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_TDD_ERNTI, (UINT8*)&pCiDevTddData->eRnti, sizeof(pCiDevTddData->eRnti), respBuffer, pbufferLeft);
	}
	//process U-RNTI/ C-RNTI
	if(TEST_BIT(pCiDevTddData->rnti.flag, 0) || TEST_BIT(pCiDevTddData->rnti.flag, 1))
	{
		DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_TDD_URNTI_CRNTI, (UINT8*)&pCiDevTddData->rnti, sizeof(pCiDevTddData->rnti), respBuffer, pbufferLeft);
	}
}
static void processCommonParam(const CiDevCommonData *pCiDevCommonData, char *respBuffer, UINT16 *pbufferLeft)
{
	if(pCiDevCommonData == NULL || respBuffer == NULL || pbufferLeft == NULL)
		return;
	//process UE ID
	if(pCiDevCommonData->ueId.imei.digitSize != 0 || pCiDevCommonData->ueId.imsi.digitSize != 0 || pCiDevCommonData->ueId.tmsi.digitSize != 0 || pCiDevCommonData->ueId.ptmsi.digitSize != 0)
	{
		DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_COMMON_UE_ID, (UINT8*)&pCiDevCommonData->ueId, sizeof(pCiDevCommonData->ueId), respBuffer, pbufferLeft);
	}

	//process MM information
	DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_COMMON_MM_INFO, (UINT8*)&pCiDevCommonData->mmInfo, sizeof(pCiDevCommonData->mmInfo), respBuffer, pbufferLeft);
}
static void processGsmWirelseeParam(const CiDevGsmData *pCiDevGsmData, char *respBuffer, UINT16 *pbufferLeft)
{
	if(pCiDevGsmData == NULL || respBuffer == NULL || pbufferLeft == NULL)
		return;
	//process Serving cell system information
	DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_GSM_SCELL_SYS_INFO, (UINT8*)&pCiDevGsmData->sCellSysInfo, sizeof(pCiDevGsmData->sCellSysInfo), respBuffer, pbufferLeft);
	//process Serving cell reselection system information
	if(pCiDevGsmData->sCellSysInfoDtxInd <= CI_DEV_WIRELSS_PARAM_GSM_SCELL_RESELECTION_INFO_VAL_MAX)
	{
		DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_GSM_SCELL_RESELECTION_INFO, (UINT8*)&pCiDevGsmData->sCellSysInfoDtxInd, sizeof(pCiDevGsmData->sCellSysInfoDtxInd), respBuffer, pbufferLeft);
	}
	//process Serving cell radio information
	DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_GSM_SCELL_RADIO_INFO, (UINT8*)&pCiDevGsmData->sCellRadioInfo, sizeof(pCiDevGsmData->sCellRadioInfo), respBuffer, pbufferLeft);
	//process Neighbor cell information
	if(pCiDevGsmData->nCellInfo.numNcellGsm != 0 || pCiDevGsmData->nCellInfo.numNcellTdd != 0)
	{
		DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_GSM_NCELL_INFO, (UINT8*)&pCiDevGsmData->nCellInfo, sizeof(pCiDevGsmData->nCellInfo), respBuffer, pbufferLeft);
	}
	//process Dedicated channel information
	DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_GSM_DEDICATED_CHANNEL_INFO, (UINT8*)&pCiDevGsmData->dedChannelInfo, sizeof(pCiDevGsmData->dedChannelInfo), respBuffer, pbufferLeft);
	//process Dedicated measure information
	DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_GSM_DEDICATED_MEASURE_INFO, (UINT8*)&pCiDevGsmData->dedMeasInfo, sizeof(pCiDevGsmData->dedMeasInfo), respBuffer, pbufferLeft);
	//process C/I
	if(pCiDevGsmData->ciInfo.repArfcnNum != 0)
	{
		DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_GSM_CI_INFO, (UINT8*)&pCiDevGsmData->ciInfo, sizeof(pCiDevGsmData->ciInfo), respBuffer, pbufferLeft);
	}
	//process BEP/CV BEP
	if(pCiDevGsmData->bepCvBepinfo.isGmskValid == TRUE || pCiDevGsmData->bepCvBepinfo.is8pskValid == TRUE)
	{
		DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_GSM_BEPCV_BEP_INFO, (UINT8*)&pCiDevGsmData->bepCvBepinfo, sizeof(pCiDevGsmData->bepCvBepinfo), respBuffer, pbufferLeft);
	}
	//process MCS/CS (up/down)
	if(pCiDevGsmData->mscCsInfo.ulLength != 0 || pCiDevGsmData->mscCsInfo.dlLength != 0)
	{
		DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_GSM_MCS_CS_INFO, (UINT8*)&pCiDevGsmData->mscCsInfo, sizeof(pCiDevGsmData->mscCsInfo), respBuffer, pbufferLeft);
	}
	//process TFI (up/down)/ Timeslot Allocation (up/down)
	if(pCiDevGsmData->tfiTimeSlotInfo.isUlTfiValid == TRUE || pCiDevGsmData->tfiTimeSlotInfo.isDlTfiValid == TRUE)
	{
		DEV_ProcessWirelessParam(CI_DEV_WIRELSS_PARAM_GSM_TFI_TS_ALLOCATION, (UINT8*)&pCiDevGsmData->tfiTimeSlotInfo, sizeof(pCiDevGsmData->tfiTimeSlotInfo), respBuffer, pbufferLeft);
	}
}

/****************************************************************
 *	F@: processWirelessParamInd - Process wireless parameter report
 */
static void processWirelessParamInd(const void *pParam)
{
	char respBuffer[MAX_RESPBUFF_SIZE];
	static UINT8 gsmData[sizeof(CiDevGsmData)];
	static UINT16 gsmDataSize = 0;
	CiDevPrimWirelessParamInd *pCiDevPrimWirelessParamInd;
	UINT8 networkMode;
	UINT32 systemTick;
	UINT16 bufferLeft = utlNumberOf(respBuffer) - 1;           //left one byte for terminator '\0'
	pCiDevPrimWirelessParamInd = (CiDevPrimWirelessParamInd *)pParam;
	networkMode = pCiDevPrimWirelessParamInd->network;
	switch(networkMode)
	{
	case CI_DEV_EM_NETWORK_GSM:
	{
		if((pCiDevPrimWirelessParamInd->count == 1) && (pCiDevPrimWirelessParamInd->data_size < sizeof(gsmData)))
		{
			memcpy(gsmData, pCiDevPrimWirelessParamInd->tddGsmData, pCiDevPrimWirelessParamInd->data_size);
			gsmDataSize = pCiDevPrimWirelessParamInd->data_size;
		}
		else if((pCiDevPrimWirelessParamInd->count == 2) && (gsmDataSize != 0) && (pCiDevPrimWirelessParamInd->data_size + gsmDataSize == sizeof(gsmData)))
		{
			memcpy(gsmData + gsmDataSize, pCiDevPrimWirelessParamInd->tddGsmData, pCiDevPrimWirelessParamInd->data_size);
			gsmDataSize = 0;
			if(DEV_GetSystemTick(&systemTick) == FALSE)
			{
				DBGMSG("get system tick error\n");
				return;
			}
			bufferLeft -= snprintf(respBuffer, utlNumberOf(respBuffer), "^DNPR: %u,%d", systemTick, TRUE);
			processGsmWirelseeParam((CiDevGsmData *)(gsmData), respBuffer, &bufferLeft);
			processCommonParam(&(pCiDevPrimWirelessParamInd->commonInfo), respBuffer, &bufferLeft);
			DEV_SendIndication(respBuffer, &bufferLeft, TRUE);
		}
		else
		{
			gsmDataSize = 0;
			DBGMSG("GSM : invalid param, count = %d, data size = %d\n", pCiDevPrimWirelessParamInd->count, pCiDevPrimWirelessParamInd->data_size);
			return;
		}
		break;
	}
	case CI_DEV_EM_NETWORK_UMTS:
	{
		if(DEV_GetSystemTick(&systemTick) == FALSE)
		{
			DBGMSG("get system tick error\n");
			return;
		}
		bufferLeft -= snprintf(respBuffer, utlNumberOf(respBuffer), "^DNPR: %u,%d", systemTick, TRUE);
		processTdWirelseeParam((CiDevTddData *)(pCiDevPrimWirelessParamInd->tddGsmData), respBuffer, &bufferLeft);
		processCommonParam(&(pCiDevPrimWirelessParamInd->commonInfo), respBuffer, &bufferLeft);
		DEV_SendIndication(respBuffer, &bufferLeft, TRUE);
		break;
	}
	default:
	{
		DBGMSG("not supported network mode\n");
		return;
	}
	}
	return;
}
/****************************************************************
 *	F@: processSignalingReportInd - Process signaling report
 */
static void processSignalingReportInd(const void *pParam)
{
	char respBuffer[MAX_RESPBUFF_SIZE];
	char tempBuffer[50];
	UINT16 totalMsglen, paramLen, peerMsgLen;
	CiDevPrimSignalingReportInd *pCiDevPrimSignalingReportInd;
	unsigned long secAfterBoot;
	pCiDevPrimSignalingReportInd = (CiDevPrimSignalingReportInd *)pParam;
	if(DEV_GetTimeStamp(&secAfterBoot) == FALSE)
	{
		DBGMSG("get timestamp error\n");
		return;
	}
	tempBuffer[0] = '\0';
	switch(pCiDevPrimSignalingReportInd->msgId)
	{
		case CI_DEV_SIGNALING_BCH_MSG_IND:
			paramLen = sizeof(CiDevSystemInfoType);
			libConvertNumToHex((char*)&pCiDevPrimSignalingReportInd->paramData.bchParam, paramLen, tempBuffer);
			break;
		case CI_DEV_SIGNALING_BCCH_FACH_MSG_IND:
			paramLen = sizeof(CiDevBcchFachParam);
			libConvertNumToHex((char*)&pCiDevPrimSignalingReportInd->paramData.bcchFachParam, paramLen, tempBuffer);
			break;
		case CI_DEV_SIGNALING_PCCH_PCH_MSG_IND:
			paramLen = sizeof(CiDevPcchPchParam);
			libConvertNumToHex((char*)&pCiDevPrimSignalingReportInd->paramData.pcchPchParam, paramLen, tempBuffer);
			break;
		case CI_DEV_SIGNALING_UL_CCH_MSG_IND:
			paramLen = sizeof(CiDevDlCchParam);
			libConvertNumToHex((char*)&pCiDevPrimSignalingReportInd->paramData.dlCchParam, paramLen, tempBuffer);
			break;
		case CI_DEV_SIGNALING_DL_CCH_MSG_IND:
			paramLen = sizeof(CiDevUlCchParam);
			libConvertNumToHex((char*)&pCiDevPrimSignalingReportInd->paramData.ulCchParam, paramLen, tempBuffer);
			break;
		case CI_DEV_SIGNALING_GSM_MSG_IND:
			paramLen = sizeof(CiDevGsmParam);
			libConvertNumToHex((char*)&pCiDevPrimSignalingReportInd->paramData.gsmParam, paramLen, tempBuffer);
			break;
		default:
			ERRMSG("Signaling Report message identifier: %08X is not identified\n", pCiDevPrimSignalingReportInd->msgId);
			return;
	}
	peerMsgLen = min(pCiDevPrimSignalingReportInd->peerMsgLen, CI_DEV_MAX_PEER_MSG_LENGTH);
	totalMsglen = paramLen + peerMsgLen + CI_DEV_SIGNALING_PACKET_NON_PAYLOAD_LEN;
	snprintf(respBuffer, utlNumberOf(respBuffer), "^DUSR: %04X%02X%08X%08X%04X%s%04X%04X", totalMsglen, pCiDevPrimSignalingReportInd->frameType, pCiDevPrimSignalingReportInd->msgId,
		(unsigned int)secAfterBoot, paramLen, tempBuffer, peerMsgLen, pCiDevPrimSignalingReportInd->freeHeaderSpaceLen);
	libConvertNumToHex((char *)(pCiDevPrimSignalingReportInd->peerMsgData), peerMsgLen, respBuffer);
	ATRESP(IND_REQ_HANDLE, 0, 0, respBuffer);
	return;
}

/****************************************************************************************
*
*  DESCRIPTION: Callback function when receiving indication CI message related with Dev
*
****************************************************************************************/
void devInd(CiSgOpaqueHandle opSgOpaqueHandle,
	    CiServiceGroupID svgId,
	    CiPrimitiveID primId,
	    CiIndicationHandle indHandle,
	    void*               pParam)
{
	UNUSEDPARAM(opSgOpaqueHandle)
	UNUSEDPARAM(indHandle)

	DBGMSG("%s:primId=%d\n", __FUNCTION__, primId);

	switch (primId)
	{
	case CI_DEV_PRIM_STATUS_IND:
	{
		CiDevPrimStatusInd  *devStatusInd;
		devStatusInd = (CiDevPrimStatusInd *)pParam;
		gDeviceStatus = devStatusInd->status;
		break;
	}
	case CI_DEV_PRIM_GSM_ENGMODE_INFO_IND:         //CiDevPrimGsmEngmodeInfoInd
	{
		// Disable CGED
		//processEngModeInfoInd(pParam);

		// for EEMOPT
		processEEMOPTInfoInd(pParam);

		break;
	}
	/* 1. UMTS */
	case CI_DEV_PRIM_UMTS_ENGMODE_SVCCELL_INFO_IND:         //CiDevPrimUmtsEngmodeScellInfoInd
	{
		// Disable CGED
		//processUmtsEngModeScellInfoInd(pParam);

		// for EEMOPT
		processEMumtsINFOSVCInfoInd(pParam);

		break;
	}
	/* 2. UMTS */
	case CI_DEV_PRIM_UMTS_ENGMODE_INTRAFREQ_INFO_IND:         //CiDevPrimUmtsEngmodeIntraFreqInfoInd
	{
		// Disable CGED
		//processUmtsEngModeIntraFreqInfoInd(pParam);

		// for EEMOPT
		processEMUmtsIntraFreqInfoInd(pParam);

		break;
	}
	/* 3. UMTS */
	case CI_DEV_PRIM_UMTS_ENGMODE_INTERFREQ_INFO_IND:         //CiDevPrimUmtsEngmodeInterFreqInfoInd
	{
		// Disable CGED
		//processUmtsEngModeInterFreqInfoInd(pParam);

		// 1 for EEMOPT
		processEMUmtsInterFreqInfoInd(pParam);

		break;
	}
	/* 4. UMTS */
	case CI_DEV_PRIM_UMTS_ENGMODE_INTERRAT_INFO_IND:        //CiDevPrimUmtsEngmodeInterRatInfoInd
	{
		// Disable CGED
		//processUmtsEngModeInterRatInfoInd(pParam);

		// for EEMOPT
		processEMUmtsInterRatInfoInd(pParam);

		break;
	}
	case CI_DEV_PRIM_NETWORK_MONITOR_INFO_IND:        //CiDevPrimNetworkMonitorInfoInd
	{
		processNwMonitorInfoInd(pParam);
		break;
	}
	case CI_DEV_PRIM_LP_NWDL_MSG_IND:
	{
		processLpNwdlMsgInfoInd(pParam);
		break;
	}
	case CI_DEV_PRIM_LP_RRC_STATE_IND:
	{
		processLpRRCStateInd(pParam);
		break;
	}
	case CI_DEV_PRIM_LP_MEAS_TERMINATE_IND:
	{
		processLpMeasTerminateInd(pParam);
		break;
	}
	case CI_DEV_PRIM_LP_RESET_STORED_UE_POS_IND:
	{
		processLpResetUEPosInd(pParam);
		break;
	}
#ifdef AT_CUSTOMER_HTC
	case CI_DEV_PRIM_RADIO_LINK_FAILURE_IND:
	{
		processRadioLinkFailureInd(pParam);
		break;
	}
#endif
	case CI_DEV_PRIM_PROTOCOL_STATUS_CHANGED_IND:
	{
		processProtocolStatusChangedInd(pParam);
		break;
	}
	case CI_DEV_PRIM_EVENT_REPORT_IND:
	{
		processEventReportInd(pParam);
		break;
	}
	case CI_DEV_PRIM_WIRELESS_PARAM_IND:
	{
		processWirelessParamInd(pParam);
		break;
	}
	case CI_DEV_PRIM_SIGNALING_REPORT_IND:
	{
		processSignalingReportInd(pParam);
		break;
	}
	default:
		break;
	}

	/* free up the indication memory */
	atciSvgFreeIndMem( svgId, primId, pParam );

	return;
}

void resetDevParas(void)
{
	gDeviceStatus = 0;
	g_UMTS_EM_IND_SEQ = 0;
	gEngModeRepOpt = CI_DEV_EM_OPTION_NONE;
	gStoreEngModeRepOpt = CI_DEV_EM_OPTION_NONE;
	gStoreTimeVal = -1;
	gCurrTimeVal= -1;
}
