/******************************************************************************
 * *(C) Copyright 2008 Marvell International Ltd.
 * * All Rights Reserved
 * ******************************************************************************/

#include "at_tok.h"
#include "atchannel.h"
#include "marvell-ril.h"
#include "misc.h"
#include "ril-requestdatahandler.h"
#include "work-queue.h"

#define CI_SIM_MAX_CMD_DATA_SIZE  261

static int sSimStatus = SIM_NOT_READY;
static int stk_app_inited = 0;
static int stk_CP_inited = 0;

static RIL_AppType sAppType = RIL_APPTYPE_UNKNOWN;

/*add for STK SET_UP_MENU*/
static char stkSetUpMenuRawData[CI_SIM_MAX_CMD_DATA_SIZE * 2 + 1] = "\0";
static int stkSetUpMenuFlag = 0;

extern int AfterCPReset;

/* Enum value of STK cmd type and STK ind msg type, should be same as in sim_api.h */
typedef enum StkCmdTypeTag
{
	STK_CMD_ENABLE_SIMAT = 0,
	STK_CMD_DOWNLOAD_PROFILE = 1,
	STK_CMD_GET_CAP_INFO = 2,
	STK_CMD_GET_PROFILE = 3,
	STK_CMD_SEND_ENVELOPE = 4,

	STK_CMD_PROACTIVE  = 11,
	STK_CMD_SETUP_CALL = 12,
	STK_CMD_DISPLAY_INFO = 13,
	STK_CMD_END_SESSION = 14,
	STK_CMD_SETUP_CALL_STATUS = 15,
	STK_CMD_SETUP_CALL_RESULT = 16,

	STK_CMD_SEND_SM_STATUS = 18,
	STK_CMD_SEND_SM_RESULT = 19,
	STK_CMD_SEND_USSD_RESULT = 20,

	STK_TYPE_INVALID
} StkCmdType;

typedef enum {
	SIM_CARD_TYPE_SIM,
	SIM_CARD_TYPE_USIM,
	SIM_CARD_TYPE_TEST_SIM,
	SIM_CARD_TYPE_TEST_USIM,
} SIM_Type;

static void getAppType(void *param);

static void downloadProfile()
{
	//Ninth byte b7 is LAUNCH BROWSER
	const char SIM_PROFILE[]  = "FFFFFFFF7F11009F7F0000000000000000";
	const char USIM_PROFILE[] = "FFFFFFFF7F1100DFFF000000000000000000";
	getAppType(NULL);
	const char *profile = sAppType == RIL_APPTYPE_USIM ? USIM_PROFILE : SIM_PROFILE;
	char* cmd;
	asprintf(&cmd, "AT+MSTK=1,%s; +MSTK=2", profile);
#if !defined(DKB_WUKONG)//wukong don't support such command
		at_send_command_singleline(cmd, "+MSTK:", NULL);
#endif
	free(cmd);
}

static void requestSetUpMenu() {
	if (stkSetUpMenuRawData[0] != '\0') {
		stkSetUpMenuFlag = 1;
		RIL_onUnsolicitedResponse(RIL_UNSOL_STK_PROACTIVE_COMMAND,
				&stkSetUpMenuRawData, strlen(stkSetUpMenuRawData));
	}
}

static void libSaveSetUpMenu(char * data, int len) {
	char temp1, temp2;
	char *c = data;

	if ((*c++ == 'D') && (*c++ == '0') && len <= CI_SIM_MAX_CMD_DATA_SIZE * 2) {
		temp1 = *c++;
		temp2 = *c++;
		if ((int) (temp1 & 0xff) < 0x38) {
			if (c[0] == '0' && c[1] == '1' && c[2] == '0' && c[3] == '3'
					&& c[6] == '2' && c[7] == '5') {
				memcpy(&stkSetUpMenuRawData, data, len);
				LOGD("%s: command Detail is STK SET_UP_MENU, save SET_UP_MENU \n", __FUNCTION__);
			}
		} else if (temp1 == '8' && temp2 == '1') {
			if (c[2] == '0' && c[3] == '1' && c[4] == '0' && c[5] == '3'
					&& c[8] == '2' && c[9] == '5') {
				memcpy(&stkSetUpMenuRawData, data, len);
				LOGD("%s: command Detail is STK SET_UP_MENU, save SET_UP_MENU \n", __FUNCTION__);
			}
		} else {
			LOGE("%s: the length of Indication is not correct\n", __FUNCTION__);
		}
	} else {
		LOGD("%s: This is not proactive command \n", __FUNCTION__);
	}
}

static int parseSimStatusString(char *line)
{
	int status, err;
	char *result;

	err = at_tok_start(&line);
	if (err < 0)
	{
		status = SIM_NOT_READY;
		goto done;
	}

	err = at_tok_nextstr(&line, &result);
	if (err < 0)
	{
		status = SIM_NOT_READY;
		goto done;
	}

	if (0 == strcmp(result, "SIM PIN"))
	{
		status = SIM_PIN;
		goto done;
	}
	else if (0 == strcmp(result, "SIM PUK"))
	{
		status = SIM_PUK;
		goto done;
	}
	else if (0 == strcmp(result, "PH-NET PIN"))
	{
		status = SIM_NETWORK_PERSONALIZATION;
		goto done;
	}
	else if (0 == strcmp(result, "REMOVED"))
	{
		status = SIM_ABSENT;
		goto done;
	}
	else if (0 == strcmp(result, "SIM PIN2"))
	{
		status = SIM_PIN2;
		goto done;
	}
	else if (0 == strcmp(result, "SIM PUK2"))
	{
		status = SIM_PUK2;
		goto done;
	}
	else if (0 == strcmp(result, "PH-NETSUB PIN"))
	{
		status = SIM_NETWORK_SUBSET_PERSONALIZATION;
		goto done;
	}
	else if (0 == strcmp(result, "PH-SP PIN"))
	{
		status = SIM_SERVICE_PROVIDER_PERSONALIZATION;
		goto done;
	}
	else if (0 == strcmp(result, "PH-CORP PIN"))
	{
		status = SIM_CORPORATE_PERSONALIZATION;
		goto done;
	}
	else if (0 == strcmp(result, "PH-SIMLOCK PIN"))
	{
		status = SIM_SIM_PERSONALIZATION;
		goto done;
	}
	else if (0 == strcmp(result, "PH-NET PUK"))
	{
		status = SIM_NETWORK_PERSONALIZATION_PUK;
		goto done;
	}
	else if (0 == strcmp(result, "PH-NETSUB PUK"))
	{
		status = SIM_NETWORK_SUBSET_PERSONALIZATION_PUK;
		goto done;
	}
	else if (0 == strcmp(result, "PH-SP PUK"))
	{
		status = SIM_SERVICE_PROVIDER_PERSONALIZATION_PUK;
		goto done;
	}
	else if (0 == strcmp(result, "PH-CORP PUK"))
	{
		status = SIM_CORPORATE_PERSONALIZATION_PUK;
		goto done;
	}
	else if (0 == strcmp(result, "PH-SIMLOCK PUK"))
	{
		status = SIM_SIM_PERSONALIZATION_PUK;
		goto done;
	}
	else if (0 != strcmp(result, "READY"))
	{
		/* we're treating unsupported lock types as "sim absent" */
		status = SIM_ABSENT;
		goto done;
	}
	else
	{
		status = SIM_READY;
	}

 done:
	sSimStatus = status;
	LOGI("[%s]: set sSimStatus=%d", __FUNCTION__, sSimStatus);

	return status;
}

static int parseSimStatus(ATResponse* response)
{
	int status, err;
	char *cpinLine, *cpinResult;

	switch (at_get_cme_error(response))
	{
	case CME_SUCCESS:
	{
		break;
	}
	case CME_SIM_NOT_INSERTED:
	{
		status = SIM_ABSENT;
		goto done;
	}
	case CME_SIM_UNKNOWN_ERROR:
	{
		/* some time, CP will return CME_SIM_UNKNOWN_ERROR if there is no SIM card in slot  */
		if (SIM_ABSENT == sSimStatus)
		{
			status = SIM_ABSENT;
			goto done;
		}
		else
		{
			status = SIM_NOT_READY;
			goto done;
		}
	}
	default:
	{
		status = SIM_NOT_READY;
		goto done;
	}
	}

	/* +CPIN? has succeeded, now look at the result */
	if(response->p_intermediates == NULL)
	{
		status = SIM_NOT_READY;
		goto done;
	}

	cpinLine = response->p_intermediates->line;
	status = parseSimStatusString(cpinLine);

 done:
	sSimStatus = status;
	LOGI("[%s]: set sSimStatus=%d", __FUNCTION__, sSimStatus);

	return status;
}

static void onPbInit(void* param)
{
	/* Select PBK location first for Marvell modem, otherwise other PBK related AT command may work abnormally */
	at_send_command("AT+CPBS=\"SM\"", NULL);
}

/* Do some init work after SIM is ready */
static void onSimInitReady(void* param)
{
	sSimStatus = SIM_READY;
	LOGI("[%s]: set sSimStatus=%d", __FUNCTION__, sSimStatus);

	int status = getRadioState();
	if ((status != RADIO_STATE_OFF) && (status != RADIO_STATE_UNAVAILABLE) && (status != RADIO_STATE_SIM_READY))
	{
		setRadioState(RADIO_STATE_SIM_READY);
	}
	if(stk_app_inited == 1) //only if STK service is ready and SIM is ready, download profile and set ready
	{
		/* Init STK and download ME's capability to SIM */
		downloadProfile();
	}

	stk_CP_inited = 1;
	/* Always send SMS messages directly to the TE, refer to onSmsInitReady() in ril-msg.c */
	at_send_command("AT+CNMI=1,2,2,1,1", NULL);
}

static int processStkUnsol(int cmdType, char *data)
{
	if (cmdType < 10)
	{
		LOGE("[%s]: error happens in +MSTK: %d, cmdType should be greater than 10", __FUNCTION__, cmdType);
		return -1;
	}

	/*SIM originated proactive command to ME */
	else if ((cmdType == STK_CMD_PROACTIVE) && (data != NULL) && (strlen(data) > 0))
	{
		RIL_onUnsolicitedResponse( RIL_UNSOL_STK_PROACTIVE_COMMAND, data, strlen(data));
		libSaveSetUpMenu(data, strlen(data));
	}

	/*SIM originated call setup request */
	else if ((cmdType == STK_CMD_SETUP_CALL) && (data != NULL) && (strlen(data) > 0))
	{
		RIL_onUnsolicitedResponse( RIL_UNSOL_STK_PROACTIVE_COMMAND, data, strlen(data));
	}

	/*SIM originated event notify, such as display sth in UI screen */
	else if ((cmdType == STK_CMD_DISPLAY_INFO) && (data != NULL) && (strlen(data) > 0))
	{
		RIL_onUnsolicitedResponse( RIL_UNSOL_STK_EVENT_NOTIFY, data, strlen(data));
	}

	/*SIM originated to notify ME that STK session is terminated by SIM */
	else if ((cmdType == STK_CMD_END_SESSION))
	{
		RIL_onUnsolicitedResponse( RIL_UNSOL_STK_SESSION_END, NULL, 0);
	}
#ifdef MARVELL_EXTENDED
	/* SIM originated call setup status */
	else if ((cmdType == STK_CMD_SETUP_CALL_STATUS) && (data != NULL) && (strlen(data) > 0))
	{
		RIL_onUnsolicitedResponse( RIL_UNSOL_STK_CALL_SETUP_STATUS, data, strlen(data));
	}

	/* CP originated call setup result */
	else if ((cmdType == STK_CMD_SETUP_CALL_RESULT) && (data != NULL) && (strlen(data) > 0))
	{
		RIL_onUnsolicitedResponse( RIL_UNSOL_STK_CALL_SETUP_RESULT, data, strlen(data));
	}

	/* SIM originated send sm status */
	else if ((cmdType == STK_CMD_SEND_SM_STATUS) && (data != NULL) && (strlen(data) > 0))
	{
		RIL_onUnsolicitedResponse( RIL_UNSOL_STK_SEND_SM_STATUS, data, strlen(data));
	}

	/* CP originated send sm result */
	else if ((cmdType == STK_CMD_SEND_SM_RESULT) && (data != NULL) && (strlen(data) > 0))
	{
		RIL_onUnsolicitedResponse( RIL_UNSOL_STK_SEND_SM_RESULT, data, strlen(data));
	}

	/* CP originated send ussd result */
	else if ((cmdType == STK_CMD_SEND_USSD_RESULT) && (data != NULL) && (strlen(data) > 0))
	{
		RIL_onUnsolicitedResponse( RIL_UNSOL_STK_SEND_USSD_RESULT, data, strlen(data));
	}
#endif
	else
	{
		LOGE("[%s]: error happens in +MSTK: %d, %s. ", __FUNCTION__, cmdType, data);
		return -1;
	}

	return 0;

}

static int parseStkResponse(char *s, int *pCmdType, char **pData)
{
	char *line = s, *data;
	int err, cmdType;

	err = at_tok_start(&line);
	if (err < 0) goto error;
	err = at_tok_nextint(&line, &cmdType);
	if (err < 0) goto error;
	*pCmdType = cmdType;

	if (at_tok_hasmore(&line))
	{
		err = at_tok_nextstr(&line, &data);
		if (err < 0) goto error;
		*pData = data;
	}
	else
	{
		*pData = NULL;
	}

	return 0;

 error:
	LOGE("[%s]: error happens when parsing response msg %s", __FUNCTION__, s);
	*pData = NULL;
	return -1;
}


/* External func to query current SIM status */
int getSimStatus(void)
{
	ATResponse *p_response = NULL;
	int err, status;

	err = at_send_command_singleline("AT+CPIN?", "+CPIN:",  &p_response);
	if (err < 0)
	{
		status = SIM_NOT_READY;
	}
	else
	{
		status = parseSimStatus(p_response);
	}

	at_response_free(p_response);
	return status;
}

/* Update radio state according to SIM state */
void updateRadioState(void)
{
	int simStatus = getSimStatus();

	if ((simStatus == SIM_ABSENT) || (simStatus == SIM_PIN)
	    || (simStatus == SIM_PUK)  || (simStatus == SIM_NETWORK_PERSONALIZATION))
	{
		setRadioState(RADIO_STATE_SIM_LOCKED_OR_ABSENT);
	}
	else if (simStatus == SIM_READY)
	{
		setRadioState(RADIO_STATE_SIM_READY);
	}
	else if (simStatus == SIM_NOT_READY)
	{
		setRadioState(RADIO_STATE_SIM_NOT_READY);
	}
	else
	{
		LOGD("Unexpected branch in %s", __FUNCTION__);
	}
}
/* Sometimes when receiving +CPIN: READY, but network is not registered, to speed up registration,
 * RADIO_STATE_SIM_READY will be sent later after network is registered
 */
void notifyRegisteredToSim(void)
{
	int status = getRadioState();

	if ((sSimStatus == SIM_READY) && (status != RADIO_STATE_SIM_READY))
	{
		setRadioState(RADIO_STATE_SIM_READY);
	}
}

static void getAppType(void *param)
{
	if(sAppType == RIL_APPTYPE_UNKNOWN)
	{
		ATResponse *p_response = NULL;
		int err;

		err = at_send_command_singleline("AT*EUICC?", "*EUICC:", &p_response);
		if (err < 0 || p_response->success == 0 || p_response->p_intermediates == NULL)
		{
			sAppType = RIL_APPTYPE_UNKNOWN;
			goto done;
		}
		else
		{
			char *line = p_response->p_intermediates->line;
			int type;

			err = at_tok_start(&line);
			if (err < 0)
			{
				sAppType = RIL_APPTYPE_UNKNOWN;
				goto done;
			}

			err = at_tok_nextint(&line, &type);
			if (err < 0)
			{
				sAppType = RIL_APPTYPE_UNKNOWN;
				goto done;
			}

			if(type == SIM_CARD_TYPE_SIM || type == SIM_CARD_TYPE_TEST_SIM)
				sAppType = RIL_APPTYPE_SIM;
			else if(type == SIM_CARD_TYPE_USIM || type == SIM_CARD_TYPE_TEST_USIM)
				sAppType = RIL_APPTYPE_USIM;
			else
				sAppType = RIL_APPTYPE_UNKNOWN;
		}
done:
		at_response_free(p_response);
	}

	LOGD("%s: UICC type: %d\n", __FUNCTION__, sAppType);
}

/* Process AT reply of RIL_REQUEST_ENTER_SIM_XXX */
static void callback_RequestLeftPinRetry(ATResponse* response, RIL_Token token, int request, int is_code_fd)
{
	int err, pin1num, pin2num, puk1num, puk2num, retryLeft = -1;
	char *line;

	if (response->success == 0) goto error;

	line = response->p_intermediates->line;
	err = at_tok_start(&line);
	if (err < 0) goto error;
	err = at_tok_nextint(&line, &pin1num);
	if (err < 0) goto error;
	err = at_tok_nextint(&line, &pin2num);
	if (err < 0) goto error;
	err = at_tok_nextint(&line, &puk1num);
	if (err < 0) goto error;
	err = at_tok_nextint(&line, &puk2num);
	if (err < 0) goto error;

	switch (request)
	{
	case RIL_REQUEST_ENTER_SIM_PIN:
	case RIL_REQUEST_CHANGE_SIM_PIN:
	case RIL_REQUEST_SET_FACILITY_LOCK:
		retryLeft  = pin1num;
		break;

	case RIL_REQUEST_ENTER_SIM_PIN2:
	case RIL_REQUEST_CHANGE_SIM_PIN2:
		retryLeft  = pin2num;
		break;

	case RIL_REQUEST_ENTER_SIM_PUK:
		retryLeft  = puk1num;
		break;

	case RIL_REQUEST_ENTER_SIM_PUK2:
		retryLeft  = puk2num;
		break;
	}

	if((pin2num == 0) && (request == RIL_REQUEST_CHANGE_SIM_PIN2 || (request == RIL_REQUEST_SET_FACILITY_LOCK && is_code_fd)))
	{
		RIL_onRequestComplete(token, RIL_E_SIM_PUK2, NULL, 0);
		return;
	}

error:
	LOGI("[%s]: retryLeft =%d", __FUNCTION__, retryLeft);
	RIL_onRequestComplete(token, RIL_E_PASSWORD_INCORRECT, &retryLeft, sizeof(int));
}

/**
 * RIL_REQUEST_SIM_IO
 *
 * Request SIM I/O operation.
 * This is similar to the TS 27.007 "restricted SIM" operation
 * where it assumes all of the EF selection will be done by the
 * callee.
 */
static void ril_request_sim_io(int request, void *data, size_t datalen, RIL_Token token)
{
	RIL_SIM_IO *p_args = (RIL_SIM_IO *)data;
	ATResponse *response = NULL;
	int err;
	char* cmdString;
	if (p_args->data == NULL)
	{
		if(p_args->path == NULL)
		{
			asprintf(&cmdString, "AT+CRSM=%d,%d,%d,%d,%d",
				p_args->command, p_args->fileid, p_args->p1, p_args->p2, p_args->p3);
		}
		else
		{
			asprintf(&cmdString, "AT+CRSM=%d,%d,%d,%d,%d,,%s",
				p_args->command, p_args->fileid, p_args->p1, p_args->p2, p_args->p3, p_args->path);
		}
	}
	else
	{
		if(p_args->path == NULL)
		{
			asprintf(&cmdString, "AT+CRSM=%d,%d,%d,%d,%d,%s",
				p_args->command, p_args->fileid, p_args->p1, p_args->p2, p_args->p3, p_args->data);
		}
		else
		{
			asprintf(&cmdString, "AT+CRSM=%d,%d,%d,%d,%d,%s,%s",
				p_args->command, p_args->fileid, p_args->p1, p_args->p2, p_args->p3, p_args->data, p_args->path);
		}
	}
	err = at_send_command_singleline(cmdString, "+CRSM:", &response);
	free(cmdString);
	if (err< 0 || response->success == 0 || response->p_intermediates == NULL)
		goto error;

	RIL_SIM_IO_Response sr;
	char *line;

	memset(&sr, 0, sizeof(sr));

	line = response->p_intermediates->line;

	err = at_tok_start(&line);
	if (err < 0) goto error;

	err = at_tok_nextint(&line, &(sr.sw1));
	if (err < 0) goto error;

	err = at_tok_nextint(&line, &(sr.sw2));
	if (err < 0) goto error;

	if (at_tok_hasmore(&line))
	{
	       err = at_tok_nextstr(&line, &(sr.simResponse));
	       if (err < 0) goto error;
	}

	RIL_onRequestComplete(token, RIL_E_SUCCESS, &sr, sizeof(sr));
	goto exit;

error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);
}

/**
 * RIL_REQUEST_GET_SIM_STATUS
 *
 * Requests status of the SIM interface and the SIM card.
 *
 * Valid errors:
 *  Must never fail.
 */
static void ril_request_get_sim_status(int request, void *data, size_t datalen, RIL_Token token)
{
	static RIL_AppStatus app_status_array[] = {
		/* SIM_ABSENT = 0 */
		{ RIL_APPTYPE_UNKNOWN, RIL_APPSTATE_UNKNOWN,		RIL_PERSOSUBSTATE_UNKNOWN,
		  NULL, NULL, 0, RIL_PINSTATE_UNKNOWN, RIL_PINSTATE_UNKNOWN },
		/* SIM_NOT_READY = 1 */
		{ RIL_APPTYPE_SIM,     RIL_APPSTATE_DETECTED,		RIL_PERSOSUBSTATE_UNKNOWN,
		  NULL, NULL, 0, RIL_PINSTATE_UNKNOWN, RIL_PINSTATE_UNKNOWN },
		/* SIM_READY = 2 */
		{ RIL_APPTYPE_SIM,     RIL_APPSTATE_READY,		RIL_PERSOSUBSTATE_READY,
		  NULL, NULL, 0, RIL_PINSTATE_UNKNOWN, RIL_PINSTATE_UNKNOWN },
		/* SIM_PIN = 3 */
		{ RIL_APPTYPE_SIM,     RIL_APPSTATE_PIN,		RIL_PERSOSUBSTATE_UNKNOWN,
		  NULL, NULL, 0, RIL_PINSTATE_ENABLED_NOT_VERIFIED, RIL_PINSTATE_UNKNOWN },
		/* SIM_PUK = 4 */
		{ RIL_APPTYPE_SIM,     RIL_APPSTATE_PUK,		RIL_PERSOSUBSTATE_UNKNOWN,
		  NULL, NULL, 0, RIL_PINSTATE_ENABLED_BLOCKED, RIL_PINSTATE_UNKNOWN },
		/* SIM_NETWORK_PERSONALIZATION = 5 */
		{ RIL_APPTYPE_SIM,     RIL_APPSTATE_SUBSCRIPTION_PERSO, RIL_PERSOSUBSTATE_SIM_NETWORK,
		  NULL, NULL, 0, RIL_PINSTATE_ENABLED_NOT_VERIFIED, RIL_PINSTATE_UNKNOWN },
		/* SIM_PIN2 = 6 */
		{ RIL_APPTYPE_SIM,	   RIL_APPSTATE_READY,		RIL_PERSOSUBSTATE_UNKNOWN,
		  NULL, NULL, 0, RIL_PINSTATE_UNKNOWN, RIL_PINSTATE_ENABLED_NOT_VERIFIED },
		/* SIM_PUK2 = 7 */
		{ RIL_APPTYPE_SIM,	   RIL_APPSTATE_READY,		RIL_PERSOSUBSTATE_UNKNOWN,
		  NULL, NULL, 0, RIL_PINSTATE_UNKNOWN, RIL_PINSTATE_ENABLED_BLOCKED },
		/* SIM_NETWORK_SUBSET_PERSONALIZATION = 8 */
		{ RIL_APPTYPE_SIM,	   RIL_APPSTATE_SUBSCRIPTION_PERSO, RIL_PERSOSUBSTATE_SIM_NETWORK_SUBSET,
		  NULL, NULL, 0, RIL_PINSTATE_ENABLED_NOT_VERIFIED, RIL_PINSTATE_UNKNOWN },
		/* SIM_SERVICE_PROVIDER_PERSONALIZATION = 9 */
		{ RIL_APPTYPE_SIM,	   RIL_APPSTATE_SUBSCRIPTION_PERSO, RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER,
		  NULL, NULL, 0, RIL_PINSTATE_ENABLED_NOT_VERIFIED, RIL_PINSTATE_UNKNOWN },
		/* SIM_CORPORATE_PERSONALIZATION = 10 */
		{ RIL_APPTYPE_SIM,	   RIL_APPSTATE_SUBSCRIPTION_PERSO, RIL_PERSOSUBSTATE_SIM_CORPORATE,
		  NULL, NULL, 0, RIL_PINSTATE_ENABLED_NOT_VERIFIED, RIL_PINSTATE_UNKNOWN },
		/* SIM_SIM_PERSONALIZATION = 11 */
		{ RIL_APPTYPE_SIM,	   RIL_APPSTATE_SUBSCRIPTION_PERSO, RIL_PERSOSUBSTATE_SIM_SIM,
		  NULL, NULL, 0, RIL_PINSTATE_ENABLED_NOT_VERIFIED, RIL_PINSTATE_UNKNOWN },
		/* SIM_NETWORK_PERSONALIZATION_PUK = 12 */
		{ RIL_APPTYPE_SIM,     RIL_APPSTATE_SUBSCRIPTION_PERSO, RIL_PERSOSUBSTATE_SIM_NETWORK_PUK,
		  NULL, NULL, 0, RIL_PINSTATE_ENABLED_BLOCKED, RIL_PINSTATE_UNKNOWN },
		/* SIM_NETWORK_SUBSET_PERSONALIZATION_PUK = 13 */
		{ RIL_APPTYPE_SIM,	   RIL_APPSTATE_SUBSCRIPTION_PERSO, RIL_PERSOSUBSTATE_SIM_NETWORK_SUBSET_PUK,
		  NULL, NULL, 0, RIL_PINSTATE_ENABLED_BLOCKED, RIL_PINSTATE_UNKNOWN },
		/* SIM_SERVICE_PROVIDER_PERSONALIZATION_PUK = 14 */
		{ RIL_APPTYPE_SIM,	   RIL_APPSTATE_SUBSCRIPTION_PERSO, RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER_PUK,
		  NULL, NULL, 0, RIL_PINSTATE_ENABLED_BLOCKED, RIL_PINSTATE_UNKNOWN },
		/* SIM_CORPORATE_PERSONALIZATION_PUK = 15 */
		{ RIL_APPTYPE_SIM,	   RIL_APPSTATE_SUBSCRIPTION_PERSO, RIL_PERSOSUBSTATE_SIM_CORPORATE_PUK,
		  NULL, NULL, 0, RIL_PINSTATE_ENABLED_BLOCKED, RIL_PINSTATE_UNKNOWN },
		/* SIM_SIM_PERSONALIZATION_PUK = 16 */
		{ RIL_APPTYPE_SIM,	   RIL_APPSTATE_SUBSCRIPTION_PERSO, RIL_PERSOSUBSTATE_SIM_SIM_PUK,
		  NULL, NULL, 0, RIL_PINSTATE_ENABLED_BLOCKED, RIL_PINSTATE_UNKNOWN }
	};

	ATResponse *response = NULL;
	int err;
	RIL_CardState card_state;
	int num_apps, i, sim_status;
	RIL_CardStatus *p_card_status;

	err = at_send_command_singleline("AT+CPIN?", "+CPIN:", &response);
	if (err< 0) goto error;

	sim_status = parseSimStatus(response);

	if (sim_status == SIM_ABSENT)
	{
		card_state = RIL_CARDSTATE_ABSENT;
		num_apps = 0;
	}
	else
	{
		card_state = RIL_CARDSTATE_PRESENT;
		num_apps = 1;
	}

	/* Allocate and initialize base card status. */
	p_card_status = malloc(sizeof(RIL_CardStatus));
	p_card_status->card_state = card_state;
	p_card_status->universal_pin_state = RIL_PINSTATE_UNKNOWN;
	p_card_status->gsm_umts_subscription_app_index = RIL_CARD_MAX_APPS;
	p_card_status->cdma_subscription_app_index = RIL_CARD_MAX_APPS;
	p_card_status->num_applications = num_apps;

	/* Initialize application status. */
	for (i = 0; i < RIL_CARD_MAX_APPS; i++)
	{
		p_card_status->applications[i] = app_status_array[SIM_ABSENT];
	}

	if(sim_status != SIM_ABSENT)
	{
		if(sAppType != RIL_APPTYPE_UNKNOWN)
		{
			for(i = SIM_ABSENT + 1; i < (int)sizeof(app_status_array) / (int)sizeof(app_status_array[0]); ++i)
			{
				app_status_array[i].app_type = sAppType;
			}
		}
	}

	/* Pickup the appropriate application status that reflects sim_status for gsm. */
	if (num_apps != 0)
	{
		/* Only support one app, gsm. */
		p_card_status->num_applications = 1;
		p_card_status->gsm_umts_subscription_app_index = 0;

		/* Get the correct app status. */
		p_card_status->applications[0] = app_status_array[sim_status];
	}

	RIL_onRequestComplete(token, RIL_E_SUCCESS, (char*)p_card_status, sizeof(RIL_CardStatus));

	free(p_card_status);
	goto exit;
error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);
}

/**
 * Enter SIM PIN, might be PIN, PIN2, PUK, PUK2, etc.
 *
 * Data can hold pointers to one or two strings, depending on what we
 * want to enter. (PUK requires new PIN, etc.).
 *
 */
static void request_handle_sim_pin_puk(int request, void *data, size_t datalen, RIL_Token token)
{

	ATResponse *response = NULL;
	int err;
	char* cmdString;
	int is_code_fd = 0;

	if (request == RIL_REQUEST_ENTER_SIM_PIN || request == RIL_REQUEST_ENTER_SIM_PIN2) {
		char * pin = ((char**)data)[0];
		asprintf(&cmdString, "AT+CPIN=%s", pin);
	} else if (request == RIL_REQUEST_ENTER_SIM_PUK || request == RIL_REQUEST_ENTER_SIM_PUK2){
		char * puk = ((char**)data)[0];
		char * newPin = ((char**)data)[1];
		char* fmt = request == RIL_REQUEST_ENTER_SIM_PUK2 ? "ATD**052*%s*%s*%s#" : "ATD**05*%s*%s*%s#";
		asprintf(&cmdString, fmt, puk, newPin, newPin);
	} else if (request == RIL_REQUEST_CHANGE_SIM_PIN || request == RIL_REQUEST_CHANGE_SIM_PIN2){
		char * oldPass = ((char**)data)[0];
		char * newPass = ((char**)data)[1];
		char* fmt = request == RIL_REQUEST_CHANGE_SIM_PIN ? "AT+CPIN=%s,%s" : "AT+CPWD=\"P2\",%s,%s";
		asprintf(&cmdString, fmt, oldPass, newPass);
	} else if (request == RIL_REQUEST_SET_FACILITY_LOCK) {
		char* code = ((char**)data)[0];
		char* action = ((char**)data)[1];
		char* password = ((char**)data)[2];
		char* clazz = ((char**)data)[3];
		asprintf(&cmdString, "AT+CLCK=%s,%s,%s,%s", code, action, password, clazz[0] != '0' ?  clazz : "7" );
		if (strcmp(code, "FD") == 0) is_code_fd = 1;
	} else if (request == RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION) {
		char* code = ((char**)data)[0];
		asprintf(&cmdString, "AT+CLCK=\"PN\",0,%s", code);
	} else {
		LOGE("Should not reach here: request = %d", request);
		goto error;
	}

	err = at_send_command(cmdString, &response);
	free(cmdString);
	if (err< 0) goto error;

	int retryLeft = -1;

	if (response->success != 0) {
		RIL_onRequestComplete(token, RIL_E_SUCCESS, &retryLeft, sizeof(int));
		goto exit;
	} else if (request == RIL_REQUEST_SET_FACILITY_LOCK && !is_code_fd) {
		if(response->finalResponse && strStartsWith(response->finalResponse, "+CME ERROR: 16"))
		{
			RIL_onRequestComplete(token, RIL_E_PASSWORD_INCORRECT, &retryLeft, sizeof(int));
		}
		else if(response->finalResponse && strStartsWith(response->finalResponse, "+CME ERROR: 17"))
		{
			RIL_onRequestComplete(token, RIL_E_SIM_PIN2, &retryLeft, sizeof(int));
		}
		else if(response->finalResponse && strStartsWith(response->finalResponse, "+CME ERROR: 18"))
		{
			RIL_onRequestComplete(token, RIL_E_SIM_PUK2, &retryLeft, sizeof(int));
		}
		else
		{
			RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, &retryLeft, sizeof(int));
		}

		goto exit;
	}

	at_response_free(response);
	response = NULL;
	err = at_send_command_singleline("AT+EPIN?", "+EPIN:", &response);
	if (err< 0 ) goto error;

	callback_RequestLeftPinRetry(response, token, request, is_code_fd);
	goto exit;
error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);
}

/**
 * RIL_REQUEST_GET_IMSI
 */
static void ril_request_get_imsi(int request, void *data, size_t datalen, RIL_Token token)
{
	ATResponse *response = NULL;
	int err;

	err = at_send_command_numeric("AT+CIMI", &response);

	if (err < 0 || response->success == 0 || !response->p_intermediates)
		goto error;
	RIL_onRequestComplete(token, RIL_E_SUCCESS, response->p_intermediates->line, sizeof(char *));
		goto exit;

error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);
}

/**
 * RIL_REQUEST_STK_GET_PROFILE
 * RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND
 *
 * Requests the profile of SIM tool kit.
 * The profile indicates the SAT/USAT features supported by ME.
 * The SAT/USAT features refer to 3GPP TS 11.14 and 3GPP TS 31.111.
 */
static void requestStkCmdSingleLine(int request, void *data, size_t datalen, RIL_Token token)
{
	ATResponse *response = NULL;
	int err = 0;
	char *cmdString;
	int cmdType, cmdType_sent = STK_CMD_GET_PROFILE;
	char* pData;
	if (RIL_REQUEST_STK_GET_PROFILE == request) {
		cmdType_sent = STK_CMD_GET_PROFILE;
		asprintf(&cmdString, "AT+MSTK=%d", cmdType_sent);
	} else if (RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND == request) {
		cmdType_sent = STK_CMD_SEND_ENVELOPE;
		asprintf(&cmdString,  "AT+MSTK=%d,%s", cmdType_sent, (char *)data);
	}

	err = at_send_command_singleline(cmdString, "+MSTK:", &response);
	free(cmdString);
	if (err < 0 || response->success == 0 || response->p_intermediates == NULL)
		goto error;

	if (response->finalResponse)
	{
		RIL_onRequestComplete(token, RIL_E_SUCCESS, NULL, 0);
		goto exit;
	}

	err = parseStkResponse(response->p_intermediates->line, &cmdType, &pData);
	if (err < 0 || pData == NULL|| cmdType != cmdType_sent)
		goto error;

	RIL_onRequestComplete(token, RIL_E_SUCCESS, pData, sizeof(pData));
	goto exit;
error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

exit:
	at_response_free(response);
}



/**
 * RIL_REQUEST_STK_SET_PROFILE
 *
 * Download the STK terminal profile as part of SIM initialization
 * procedure.
 */
static void requestStkCmdNoResult(int request, void *data, size_t datalen, RIL_Token token)
{
    char *cmd = NULL;
    ATResponse *response = NULL;
    if (request == RIL_REQUEST_STK_GET_PROFILE) {
	    const char *profile = (const char *) data;
	    asprintf(&cmd, "AT+MSTK=%d,%s", STK_CMD_DOWNLOAD_PROFILE, profile);
    } else if (request == RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE) {
		if (stkSetUpMenuFlag == 1) {
			stkSetUpMenuFlag = 0;
		    RIL_onRequestComplete(token, RIL_E_SUCCESS, NULL, 0);
		    goto exit;
		}
		asprintf(&cmd, "AT+MSTK=%d,%s", STK_CMD_PROACTIVE, (char *) data);
	} else if (request == RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM){
	int acceptFlag = ((int *)data)[0];
	asprintf(&cmd, "AT+MSTK=%d,%d", STK_CMD_SETUP_CALL, acceptFlag);
    }


    int err = at_send_command(cmd, &response);
    free(cmd);
    if (err < 0 || response->success == 0)
        goto error;

    RIL_onRequestComplete(token, RIL_E_SUCCESS, NULL, 0);
    goto exit;

error:
    RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

exit:
    at_response_free(response);
    return;

}

#ifdef MARVELL_EXTENDED
/**
 * RIL_REQUEST_LOCK_INFO
 */
static void ril_request_lock_info(int request, void *data, size_t datalen, RIL_Token token)
{
	ATResponse *response = NULL;
	int err, pin1num, pin2num, puk1num, puk2num;
	int lockinfo[4];
	char *line;

	err = at_send_command_singleline("AT+EPIN?", "+EPIN:", &response);

	if (err < 0 || response->success == 0 || !response->p_intermediates)
		goto error;

	line = response->p_intermediates->line;

	err = at_tok_start(&line);
	if (err < 0) goto error;
	err = at_tok_nextint(&line, &pin1num);
	if (err < 0) goto error;
	err = at_tok_nextint(&line, &pin2num);
	if (err < 0) goto error;
	err = at_tok_nextint(&line, &puk1num);
	if (err < 0) goto error;
	err = at_tok_nextint(&line, &puk2num);
	if (err < 0) goto error;

	lockinfo[0] = pin1num;
	lockinfo[1] = pin2num;
	lockinfo[2] = puk1num;
	lockinfo[3] = puk2num;
	LOGI("ril-sim.c :: callback_RequestLockInfo() :pin1num : %d", pin1num);
	LOGI("ril-sim.c :: callback_RequestLockInfo() :pin2num : %d", pin2num);
	LOGI("ril-sim.c :: callback_RequestLockInfo() :puk1num : %d", puk1num);
	LOGI("ril-sim.c :: callback_RequestLockInfo() :puk2num : %d", puk2num);

	RIL_onRequestComplete(token, RIL_E_SUCCESS, lockinfo, sizeof(lockinfo));
	goto exit;

error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);
}
#endif

static void simProcessRequest (void* arg)
{
	RILRequest* r = (RILRequest*)arg;
	LOGD("%s: %s, token:%p", __FUNCTION__, requestToString(r->request), r->token);
	if (!needContinueProcess(r->request, r->data, r->datalen, r->token)) {
		freeRILRequest(r);
		LOGI("%s: do not need to continue to process, token:%p", __FUNCTION__, r->token);
		return;
	}
	switch (r->request)
	{

	case RIL_REQUEST_SIM_IO:
	{
		ril_request_sim_io(r->request, r->data, r->datalen, r->token);
		break;
	}

	case RIL_REQUEST_GET_SIM_STATUS:
	{
		ril_request_get_sim_status(r->request, r->data, r->datalen, r->token);
		break;
	}

	case RIL_REQUEST_SET_FACILITY_LOCK:
	case RIL_REQUEST_ENTER_SIM_PIN:
	case RIL_REQUEST_ENTER_SIM_PIN2:
	case RIL_REQUEST_ENTER_SIM_PUK:
	case RIL_REQUEST_ENTER_SIM_PUK2:
	case RIL_REQUEST_CHANGE_SIM_PIN:
	case RIL_REQUEST_CHANGE_SIM_PIN2:
	case RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION:
	{
		request_handle_sim_pin_puk(r->request, r->data, r->datalen, r->token);
		break;
	}

	case RIL_REQUEST_GET_IMSI:
	{
		ril_request_get_imsi(r->request, r->data, r->datalen, r->token);
		break;
	}


	case RIL_REQUEST_STK_GET_PROFILE:
	case RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND:
	{
		requestStkCmdSingleLine(r->request, r->data, r->datalen, r->token);
		break;
	}

	case RIL_REQUEST_STK_SET_PROFILE:
	case RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE:
	case RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM:
	{
		requestStkCmdNoResult(r->request, r->data, r->datalen, r->token);
		break;
	}

	case RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING:
	{

		if(stk_CP_inited == 1)//only if STK service is ready and SIM is ready, download profile and set ready
		{
			downloadProfile();
		}
		stk_app_inited = 1;
		requestSetUpMenu();
		RIL_onRequestComplete(r->token, RIL_E_SUCCESS, NULL, 0);
		break;
	}
#ifdef MARVELL_EXTENDED
	case RIL_REQUEST_LOCK_INFO:
	{
		ril_request_lock_info(r->request, r->data, r->datalen, r->token);
		break;
	}
#endif
	default:
	{
		LOGW("%s:invalid request:%d\n", __FUNCTION__, r->request);
		RIL_onRequestComplete(r->token, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
		break;
	}
	}

	/* Set AT cmd type and callback func */

	freeRILRequest(r);
}

void onRequest_sim (int request, void *data, size_t datalen, RIL_Token token)
{
	LOGV("%s entry: request = %d", __FUNCTION__, request);
	enque(getWorkQueue(SERVICE_SIM), simProcessRequest, (void*)newRILRequest(request, data, datalen, token));
}

void onUnsolicited_sim (const char *s)
{
	char *line = NULL;
	int err;
	char *linesave = NULL;

	/* Receive pin status ind msg */
	if (strStartsWith(s, "+CPIN:"))
	{
		int status;

		if(AfterCPReset)
		{
			enque_initializeCallback();
			LOGD("%s: begin to re-initialize RIL\n", __FUNCTION__);
			return;
		}

		linesave = strdup(s);
		status = parseSimStatusString(linesave);

		/* we must judge SIM_READY due to SIM Status can't be control by Radio*/
		if(status == SIM_READY)
		{
			/* when SIM Ready indication coming, we need to wait EUICC report SIM type
			 * onSimInitReady will do STK init, and according to the Radio state to change
			 * Radio state, so it must be check firstly */
			const struct timeval TIMEVAL_DELAY = { 1, 0 };
			enqueDelayed(getWorkQueue(SERVICE_SIM), onSimInitReady, NULL, &TIMEVAL_DELAY);
		}
		else if(getRadioState() == RADIO_STATE_OFF)
		{
			;//do nothing
		}
		else if (status == SIM_NOT_READY)
		{
			setRadioState(RADIO_STATE_SIM_NOT_READY);
		}
		else if ((status == SIM_PIN2) || (status == SIM_PUK2))
		{
			; // do nothing
		}
		else
		{
			setRadioState(RADIO_STATE_SIM_LOCKED_OR_ABSENT);
		}
	}

	/* PBK module in CP has finished initialization */
	else if (strStartsWith(s, "+MPBK: 1"))
	{
		enque(getWorkQueue(SERVICE_SIM),onPbInit, NULL);
	}

	else if (strStartsWith(s, "+MSTK:"))
	{
		int cmdType = 0;
		char *data;

		line = strdup(s);
		linesave = line;
		err = at_tok_start(&line);
		if (err < 0) goto error;
		err = at_tok_nextint(&line, &cmdType);
		if (err < 0) goto error;
		if (at_tok_hasmore(&line))
		{
			err = at_tok_nextstr(&line, &data);
			if (err < 0) goto error;
		}
		err = processStkUnsol(cmdType, data);
		if (err < 0) goto error;
	}
	else if (strStartsWith(s, "*EUICC:"))
	{
		int type = 0;

		line = strdup(s);
		linesave = line;
		err = at_tok_start(&line);
		if (err < 0) goto error;
		err = at_tok_nextint(&line, &type);
		if (err < 0) goto error;

		if(type == SIM_CARD_TYPE_SIM || type == SIM_CARD_TYPE_TEST_SIM)
			sAppType = RIL_APPTYPE_SIM;
		else if(type == SIM_CARD_TYPE_USIM || type == SIM_CARD_TYPE_TEST_USIM)
			sAppType = RIL_APPTYPE_USIM;
		else
			sAppType = RIL_APPTYPE_UNKNOWN;

		LOGD("%s: UICC type: %d\n", __FUNCTION__, sAppType);
	}
	else if (strStartsWith(s, "+REFRESH:"))
	{
		int refreshResult[2] = {1, 0};
		line = strdup(s);
		linesave = line;
		err = at_tok_start(&line);
		if(err < 0) goto error;
		err = at_tok_nextint(&line, &refreshResult[0]);
		if(err < 0) goto error;
		if (at_tok_hasmore(&line))
		{
			err = at_tok_nextint(&line, &refreshResult[1]);
			if (err < 0) goto error;
		}
		RIL_onUnsolicitedResponse( RIL_UNSOL_SIM_REFRESH, refreshResult, sizeof(refreshResult));
	}
	/* Free allocated memory and return */
	if (linesave != NULL) free(linesave);
	return;

 error:
	LOGE("[%s]: error happens when parsing ind msg %s", __FUNCTION__, s);
	if (linesave != NULL) free(linesave);
	return;
}
