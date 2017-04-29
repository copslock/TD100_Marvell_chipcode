/******************************************************************************
 * *(C) Copyright 2008 Marvell International Ltd.
 * * All Rights Reserved
 * ******************************************************************************/

#include "at_tok.h"
#include "atchannel.h"
#include "marvell-ril.h"
#include "ril-requestdatahandler.h"
#include "work-queue.h"


#undef WORKAROUND_FOR_DEMO

extern int is_ppp_enabled();
extern void disableInterface(int cid);
extern void disablePPPInterface(int cid);

void updateRadioState(void);

/** Query CP power on status, Returns 1 if on, 0 if off, and -1 on error */
int isRadioOn()
{
	ATResponse *p_response = NULL;
	int err;
	char *line;
	char ret = 0;

	err = at_send_command_singleline("AT+CFUN?", "+CFUN:", &p_response);
	if (err < 0 || p_response->success == 0)
	{
		// assume radio is off
		goto error;
	}

	line = p_response->p_intermediates->line;

	err = at_tok_start(&line);
	if (err < 0) goto error;

	err = at_tok_nextbool(&line, &ret);
	if (err < 0) goto error;

	at_response_free(p_response);
	return (int)ret;

 error:
	at_response_free(p_response);
	return -1;
}

struct request_radio_power {
	RIL_Token token;
	int onOff;
};

/* Process RIL_REQUEST_RADIO_POWER */
static void ril_request_radio_power(int request, void *data, size_t datalen, RIL_Token token)
{
	int onOff = ((int *)data)[0];
	int err, status, continue_flag;
	RIL_RadioState currentState;
	ATResponse *p_response = NULL;

#ifdef WORKAROUND_FOR_DEMO
	RIL_onRequestComplete(token, RIL_E_SUCCESS, NULL, 0);
	goto exit;
#endif

	currentState = getRadioState();

	if ((onOff == 0 || onOff == 4) && (currentState != RADIO_STATE_OFF))
	{
		if(onOff == 0)
			err = at_send_command("AT+CFUN=0", &p_response);
		else
			err = at_send_command("AT+CFUN=4", &p_response);
		
		if (err < 0 || p_response->success == 0) goto error;
		setRadioState(RADIO_STATE_OFF);
		resetLocalRegInfo();
		continue_flag = 2; // sync AT
	}

	else if ((onOff == 1) && (currentState == RADIO_STATE_OFF))
	{
		err = at_send_command("AT+CFUN=1", &p_response);
		if (err < 0 || p_response->success == 0)
		{
			/* Some stacks return an error when there is no SIM, but they really turn the RF portion on
			 * So, if we get an error, let's check to see if it turned on anyway
			 */
			if (isRadioOn() != 1) goto error;
		}
		updateRadioState();
		continue_flag = 2; // sync AT
	}

	else
	{
		LOGD("Already in current state, return directly");
		continue_flag = 0; // local implementation
	}

	RIL_onRequestComplete(token, RIL_E_SUCCESS, NULL, 0);
	goto exit;

 error:
	
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
 exit:
	at_response_free(p_response);
}

/* Process RIL_REQUEST_RESET_RADIO */
static void ril_request_reset_radio(int request, void *data, size_t datalen, RIL_Token token)
{
	int onOff, err;
	ATResponse *p_response = NULL;

#ifdef WORKAROUND_FOR_DEMO
	RIL_onRequestComplete(token, RIL_E_SUCCESS, NULL, 0);
	return;
#endif

	err = at_send_command("AT+CFUN=4", &p_response);
	if (err < 0 || p_response->success == 0) goto error;

	setRadioState(RADIO_STATE_OFF);
	resetLocalRegInfo();

	err = at_send_command("AT+CFUN=1", &p_response);
	if (err < 0 || p_response->success == 0)
	{
		/* Some stacks return an error when there is no SIM, but they really turn the RF portion on
		 * So, if we get an error, let's check to see if it turned on anyway
		 */
		if (isRadioOn() != 1) goto error;
	}

	updateRadioState();

	at_response_free(p_response);
	RIL_onRequestComplete(token, RIL_E_SUCCESS, NULL, 0);
	return;

 error:
	at_response_free(p_response);
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
	return;
}

/* Process AT reply of RIL_REQUEST_GET_IMEI */
static void ril_request_get_imei(int request, void *data, size_t datalen, RIL_Token token)
{
	ATResponse *response = NULL;
	int err = at_send_command_numeric("AT+CGSN", &response);

	if (err < 0 || response->success == 0 || !response->p_intermediates) 
		goto error;

	char *strImei = response->p_intermediates->line;
	RIL_onRequestComplete(token, RIL_E_SUCCESS, strImei, sizeof(strImei));
	goto exit;

error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);
}

static void ril_request_get_imeisv(int request, void *data, size_t datalen, RIL_Token token)
{
	ATResponse *response = NULL;
	char *line, *result;
	
	int err = at_send_command_singleline("AT*CGSN?", "*CGSN:", &response);
	if (err< 0 || response->success == 0 || !response->p_intermediates) goto error;

	line = response->p_intermediates->line;

	err = at_tok_start(&line);
	if (err < 0) goto error;

	err = at_tok_nextstr(&line, &result);
	if (err < 0) goto error;

	RIL_onRequestComplete(token, RIL_E_SUCCESS, result, strlen(result));
	goto exit;

error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);

}


/**
 * RIL_REQUEST_BASEBAND_VERSION
 *
 * Return string value indicating baseband version, eg
 * response from AT+CGMR.
 */
static void ril_request_baseband_version(int request, void *data, size_t datalen, RIL_Token token)
{
	ATResponse *response = NULL;
	int err;
	char *line, *result;
	
	err = at_send_command_singleline("AT+CGMR", "+CGMR:", &response);
	if (err< 0 || response->success == 0 || !response->p_intermediates) goto error;

	line = response->p_intermediates->line;

	err = at_tok_start(&line);
	if (err < 0) goto error;

	err = at_tok_nextstr(&line, &result);
	if (err < 0) goto error;

	RIL_onRequestComplete(token, RIL_E_SUCCESS, result, strlen(result));
	goto exit;

error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);

}


/**
 * RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE
 *
 * Query the list of band mode supported by RF.
 *
 * See also: RIL_REQUEST_SET_BAND_MODE
 */
static void ril_request_query_available_band_mode(int request, void *data, size_t datalen, RIL_Token token)
{
	ATResponse *response = NULL;
	int err;
	int gsmband, umtsband, result[20];
	char *line;
	char* mode;
	int count = 0;

	err = at_send_command_singleline("AT*BAND=?", "*BAND:", &response);
	if (err< 0 || response->success == 0 || response->p_intermediates == NULL) 
		goto error;

	line = response->p_intermediates->line;

	err = at_tok_start(&line);
	if (err < 0)  goto error;

	err = at_tok_nextstr(&line, &mode);
	if (err < 0)  goto error;

	err = at_tok_nextint(&line, &gsmband);
	if (err < 0)  goto error;

	err = at_tok_nextint(&line, &umtsband);
	if (err < 0)  goto error;


	if((gsmband & GSMBAND_PGSM_900) && (gsmband & GSMBAND_DCS_GSM_1800) && (umtsband & UMTSBAND_BAND_1))
	{
	     count++;
	     result[count] = 1; //EURO band(GSM-900 / DCS-1800 / WCDMA-IMT-2000)
	     
	}

	if((gsmband & GSMBAND_GSM_850) && (gsmband & GSMBAND_PCS_GSM_1900) && ( umtsband & UMTSBAND_BAND_5) && ( umtsband & UMTSBAND_BAND_2))
	{
	     count++;
	     result[count] = 2; //US band(GSM-850 / PCS-1900 / WCDMA-850 / WCDMA-PCS-1900)
	     
	}

	if((umtsband & UMTSBAND_BAND_1) && (umtsband & UMTSBAND_BAND_6))
	{
	     count++;
	     result[count]= 3; //JPN band(WCDMA-800 / WCDMA-IMT-2000)
	}

	if((gsmband & GSMBAND_PGSM_900) && (gsmband & GSMBAND_DCS_GSM_1800) &&( umtsband & UMTSBAND_BAND_5) && (umtsband & UMTSBAND_BAND_1))
	{
	     count++;
	     result[count]= 4; //AUS band (GSM-900 / DCS-1800 / WCDMA-850 / WCDMA-IMT-2000)
	     
	}

	result[0] = count;
	     
	RIL_onRequestComplete(token, RIL_E_SUCCESS, result, (count + 1)*sizeof(int));

	goto exit;

error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);

}

/* RIL_REQUEST_DEVICE_IDENTITY
 *
 * Request the device ESN / MEID / IMEI / IMEISV.
 *
 */
static void ril_request_device_identity(int request, void *data, size_t datalen, RIL_Token token)
{
	ATResponse *response = NULL;
	int err;
	char *line = NULL;
	char *sv = NULL;
	char *imei = NULL;
	char *result[4] = {0};
	char DeviceIdentity_sv[3] = {0}; // SVN of IMEISV, 2 digits + 1 '\0'
	
	err = at_send_command_singleline("AT*CGSN?", "*CGSN:", &response);
	if (err< 0 || response->success == 0 || response->p_intermediates == NULL) 
		goto error;

	line = response->p_intermediates->line;

	err = at_tok_start(&line);
	if (err < 0)  goto error;

	err = at_tok_nextstr(&line, &sv);
	if (err < 0)  goto error;
	
	strncpy(DeviceIdentity_sv, sv, 2);
	DeviceIdentity_sv[2] = '\0';

	at_response_free(response); 
	response = NULL;

	err = at_send_command_numeric("AT+CGSN", &response);
	if (err< 0 || response->success == 0 || response->p_intermediates == NULL) 
		goto error;

	imei = response->p_intermediates->line;

	result[0] = imei;
	result[1] = DeviceIdentity_sv;
	result[2] = NULL;
	result[3] = NULL;

	LOGI("%s: imei: %s, sv: %s\n", __FUNCTION__, imei, DeviceIdentity_sv);
	RIL_onRequestComplete(token, RIL_E_SUCCESS, result, sizeof(result));
	
	goto exit;
error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);
}

static void ril_request_deactivate_data_call(int request, void *data, size_t datalen, RIL_Token token)
{
	int cid = atoi(((char **)data)[0]);
	
	if (is_ppp_enabled()) disablePPPInterface(cid);
	else disableInterface(cid);

	char cmd[32];
	snprintf(cmd, sizeof(cmd), "AT+CGACT=0,%d", cid);
	at_send_command(cmd, NULL);
	sleep(1);
	RIL_onRequestComplete(token, RIL_E_SUCCESS, NULL, 0);
}

static void devProcessRequest(void* arg)
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
		case RIL_REQUEST_RADIO_POWER:
		{
			ril_request_radio_power(r->request, r->data, r->datalen, r->token);
			break;
		}

		case RIL_REQUEST_RESET_RADIO:
		{
			ril_request_reset_radio(r->request, r->data, r->datalen, r->token);
			break;
		}

		case RIL_REQUEST_GET_IMEI:
		{
			ril_request_get_imei(r->request, r->data, r->datalen, r->token);
			break;
		}

		case RIL_REQUEST_GET_IMEISV:
		{
			ril_request_get_imeisv(r->request, r->data, r->datalen, r->token);
			break;
		}

		case RIL_REQUEST_BASEBAND_VERSION:
		{
			ril_request_baseband_version(r->request, r->data, r->datalen, r->token);
			break;
		}

		case RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE:
		{
			ril_request_query_available_band_mode(r->request, r->data, r->datalen, r->token);
			break;
		}

		case RIL_REQUEST_DEVICE_IDENTITY:
		{
			ril_request_device_identity(r->request, r->data, r->datalen, r->token);
			break;
		}

		case RIL_REQUEST_OEM_HOOK_RAW:
		case RIL_REQUEST_OEM_HOOK_STRINGS:
		case RIL_REQUEST_SET_BAND_MODE:
		{
			//todo
			RIL_onRequestComplete(r->token, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
			break;
		}
		case RIL_REQUEST_DEACTIVATE_DATA_CALL:
		{
			ril_request_deactivate_data_call(r->request, r->data, r->datalen, r->token);
			break;
		}
		default:
		{
			LOGW("%s:invalid request:%d\n", __FUNCTION__, r->request);
			break;
		}
	}
	freeRILRequest(r);
}

void onRequest_dev (int request, void *data, size_t datalen, RIL_Token token)
{
	LOGV("%s entry: request = %d", __FUNCTION__, request);
	enque(getWorkQueue(SERVICE_DEV), devProcessRequest, (void*)newRILRequest(request, data, datalen, token));
}

void onUnsolicited_dev (const char *s)
{
	return;
}

