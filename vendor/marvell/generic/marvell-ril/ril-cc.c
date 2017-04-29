/******************************************************************************
 * *(C) Copyright 2008 Marvell International Ltd.
 * * All Rights Reserved
 * ******************************************************************************/
#include <sys/atomics.h>

#include "at_tok.h"
#include "atchannel.h"
#include "marvell-ril.h"
#include "misc.h"
#include "ril-requestdatahandler.h"
#include "work-queue.h"


static int suppress_dtmf = 0;

/* Save last +CLCC unsol msg string */
static char sLastClccUnsolBuf[100] = "";

static int convertClccStateToRILState(int state, RIL_CallState *p_state)
{
	switch (state)
	{
	case 0: *p_state = RIL_CALL_ACTIVE;   return 0;
	case 1: *p_state = RIL_CALL_HOLDING;  return 0;
	case 2: *p_state = RIL_CALL_DIALING;  return 0;
	case 3: *p_state = RIL_CALL_ALERTING; return 0;
	case 4: *p_state = RIL_CALL_INCOMING; return 0;
	case 5: *p_state = RIL_CALL_WAITING;  return 0;
	default: return -1;
	}
}

/**
 * Note: directly modified line and has *p_call point directly into
 * modified line. AT response format like:
 *  +CLCC: 1,0,2,0,0,\"+18005551212\",145,"Name"
 *        index,isMT,state,mode,isMpty(,number,TOA(,alpha))?
 */
static int getInfoFromClccLine(char *line, RIL_Call *p_call)
{
	int err, state, mode;

	err = at_tok_start(&line);
	if (err < 0) goto error;

	err = at_tok_nextint(&line, &(p_call->index));
	if (err < 0) goto error;

	err = at_tok_nextbool(&line, &(p_call->isMT));
	if (err < 0) goto error;

	err = at_tok_nextint(&line, &state);
	if (err < 0) goto error;

	err = convertClccStateToRILState(state, &(p_call->state));
	if (err < 0) goto error;

	err = at_tok_nextint(&line, &mode);
	if (err < 0) goto error;

	p_call->isVoice = (mode == 0);

	err = at_tok_nextbool(&line, &(p_call->isMpty));
	if (err < 0) goto error;

	LOGD("%s: index: %d, isMT: %d, state: %d, isVoice: %d, isMpty: %d\n",
		__FUNCTION__, p_call->index, p_call->isMT, p_call->state, p_call->isVoice, p_call->isMpty);

	if (at_tok_hasmore(&line))
	{
		err = at_tok_nextstr(&line, &(p_call->number));
		/* tolerate null here */
		if (err < 0) return 0;

		//[Jerry] why?? (inherited code)
		// Some lame implementations return strings
		// like "NOT AVAILABLE" in the CLCC line
		if ((p_call->number != NULL) && (0 == strspn(p_call->number, "+0123456789")))
		{
			p_call->number = NULL;
		}

		err = at_tok_nextint(&line, &p_call->toa);
		if ((err < 0) && (p_call->number != NULL)) goto error;

		LOGD("%s: number: %s, toa: %d\n", __FUNCTION__, 
			((p_call->number == NULL) ? "null" : p_call->number), p_call->toa);

		if (at_tok_hasmore(&line))
		{
			err = at_tok_nextstr(&line, &p_call->name);
			if (err < 0) goto error;
			LOGD("%s: name: %s\n", __FUNCTION__, ((p_call->name == NULL) ? "null" : p_call->name));
		}
	}

	return 0;

 error:
	LOGE("invalid CLCC line\n");
	return -1;
}


/* Process AT reply of RIL_REQUEST_GET_CURRENT_CALLS */
static void ril_request_get_current_calls(int request, void *data, size_t datalen, RIL_Token token)
{
	ATResponse *response = NULL;
	ATLine *p_cur;
	int countCalls;
	int countValidCalls;
	RIL_Call *p_calls;
	RIL_Call **pp_calls;
	int i, err;
	int needRepoll = 0;
	RIL_RadioState currentState = getRadioState();

	/* Android will call +CLCC to clean variable when RADIO_OFF, return radio_available directly */
	if (( currentState == RADIO_STATE_SIM_NOT_READY ) && ( strlen(sLastClccUnsolBuf) == 0 )) {
		RIL_onRequestComplete(token, RIL_E_RADIO_NOT_AVAILABLE, NULL, 0);
		goto exit;
	}

	err = at_send_command_multiline("AT+CLCC", "+CLCC:", &response);
	if (err < 0 || response->success == 0)
		goto error;

	/* count the calls */
	for (countCalls = 0, p_cur = response->p_intermediates; p_cur != NULL; p_cur = p_cur->p_next)
	{
		countCalls++;
	}

	/* yes, there's an array of pointers and then an array of structures */
	pp_calls = (RIL_Call **)alloca(countCalls * sizeof(RIL_Call *));
	p_calls = (RIL_Call *)alloca(countCalls * sizeof(RIL_Call));
	memset(p_calls, 0, countCalls * sizeof(RIL_Call));

	/* init the pointer array */
	for (i = 0; i < countCalls; i++)
	{
		pp_calls[i] = &(p_calls[i]);
	}

	/* Analyze AT response and report */
	for (countValidCalls = 0, p_cur = response->p_intermediates; p_cur != NULL; p_cur = p_cur->p_next)
	{
		err = getInfoFromClccLine(p_cur->line, p_calls + countValidCalls);
		if (err != 0) continue;

		countValidCalls++;
	}

	RIL_onRequestComplete(token, RIL_E_SUCCESS, pp_calls, countValidCalls * sizeof(RIL_Call *));
	goto exit;
error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);

}

static void ril_request_dial(int request, void *data, size_t datalen, RIL_Token token)
{
	ATResponse *response = NULL;
	RIL_Dial *p_dial = (RIL_Dial *)data;
	char cmd[128];
	
	if (request == RIL_REQUEST_DIAL) {
		const char *clir;
		switch (p_dial->clir)
		{
		case 1:
			clir = "I"; /*invocation*/
			break;
		case 2:
			clir = "i"; /*suppression*/
			break;
		case 0:
		default:
			clir = ""; /*subscription default*/
		}
		snprintf(cmd, sizeof(cmd), "ATD%s%s;", p_dial->address, clir);
	} 
#ifdef MARVELL_EXTENDED
	else if (request == RIL_REQUEST_DIAL_VT) {
		snprintf(cmd, sizeof(cmd), "ATD%s", p_dial->address);
	}
#endif
	else {
		goto error;
	}
		
	int err = at_send_command( cmd, &response);
	if (err < 0 || response->success == 0)
		goto error;
	RIL_onRequestComplete(token, RIL_E_SUCCESS, NULL, 0);
	goto exit;
error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);

}

#ifdef MARVELL_EXTENDED
static void ril_handle_cmd_GetCallCost(const char* cmd, const char* prefix, RIL_Token token, int request)
{
	ATResponse *response = NULL;
	int err;
	char *line;
	char responseStr[RIL_ACM_AMM_DIG_LENGTH + 1];
	char *temp;
	char *result[2];

	err = at_send_command_singleline(cmd, prefix, &response);
	if (err < 0 || response->success == 0 || !response->p_intermediates)
		goto error;
	
	line = response->p_intermediates->line;

	err = at_tok_start(&line);
	if (err < 0) goto error;

	switch(request)
	{
	case RIL_REQUEST_GET_ACM:
	{
		err = at_tok_nextstr(&line, &temp);
		strcpy(responseStr, temp);
		if (err < 0) goto error;
		RIL_onRequestComplete(token, RIL_E_SUCCESS, &responseStr, sizeof(responseStr));
		break;
	}
	case RIL_REQUEST_GET_AMM:
	{
		err = at_tok_nextstr(&line, &temp);
		strcpy(responseStr, temp);
		if (err < 0) goto error;
		RIL_onRequestComplete(token, RIL_E_SUCCESS, &responseStr, sizeof(responseStr));
		break;
	}
	case RIL_REQUEST_GET_CPUC:
	{

		err = at_tok_nextstr(&line, &result[0]);
		if (err < 0) goto error;

		err = at_tok_nextstr(&line, &result[1]);
		if (err < 0) goto error;

		RIL_onRequestComplete(token, RIL_E_SUCCESS, result, sizeof(result));
		break;
	}
	default:
		goto error;
	}

	goto exit;

error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);

}
#endif

static void ccProcessRequest (void* arg)
{
	RILRequest* r = (RILRequest*)arg;
	RIL_Token token = r->token;
	int request = r->request;
	LOGD("%s: %s, token:%p", __FUNCTION__, requestToString(r->request), r->token);
	char cmdString[64];
	if (!needContinueProcess(r->request, r->data, r->datalen, r->token)) {
		LOGI("%s: do not need to continue to process, token:%p", __FUNCTION__, r->token);
		goto bail_out;
	}
	switch (r->request) {
	case RIL_REQUEST_GET_CURRENT_CALLS:
		ril_request_get_current_calls(r->request, r->data, r->datalen, r->token);
		break;

	case RIL_REQUEST_LAST_CALL_FAIL_CAUSE:
	{
		ril_handle_cmd_one_int("AT+CEER", "+CEER:", r->token);
		break;
	}

	case RIL_REQUEST_DIAL:
	{
		ril_request_dial(r->request, r->data, r->datalen, r->token);
		break;
	}

	case RIL_REQUEST_ANSWER:
	{
		ril_handle_cmd_default_response("ATA", r->token);
		break;
	}

	case RIL_REQUEST_HANGUP:
	{
		sprintf(cmdString, "AT+CHLD=1%d", *(int *)r->data);
		ril_handle_cmd_default_response(cmdString, r->token);
		break;
	}

	case RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND:
	{
		ril_handle_cmd_default_response("AT+CHLD=0", r->token);
		break;
	}

	case RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND:
	{
		ril_handle_cmd_default_response("AT+CHLD=1", r->token);
		break;
	}

	case RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE:
	{
		ril_handle_cmd_default_response("AT+CHLD=2", r->token);
		break;
	}

	case RIL_REQUEST_CONFERENCE:
	{
		ril_handle_cmd_default_response("AT+CHLD=3", r->token);
		break;
	}

	case RIL_REQUEST_UDUB:
	{
		ril_handle_cmd_default_response("AT+CHLD=0", r->token);
		break;
	}

	case RIL_REQUEST_DTMF:
		if (suppress_dtmf)
			RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
		else {
			char c = ((char *)r->data)[0];
			char duration = ((char*)r->data)[1];
			LOGV("RIL_REQUEST_DTMF:%c,%c", c, duration);
			sprintf(cmdString, "AT+VTS=%c", (int)c);
			ril_handle_cmd_default_response(cmdString, r->token);
		}
		break;

	case RIL_REQUEST_DTMF_START:
		if (suppress_dtmf)
			RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
		else {
			char c = ((char *)r->data)[0];
			LOGV("RIL_REQUEST_DTMF_START:%c", c);
			sprintf(cmdString, "AT+VTS=%c", (int)c);
			ril_handle_cmd_default_response(cmdString, r->token);
		}
		break;

	case RIL_REQUEST_DTMF_STOP:
		if (suppress_dtmf)
			RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
		else 
			ril_handle_cmd_default_response("AT", r->token);
		break;

	case RIL_REQUEST_SET_MUTE:
	{
		int mute = ((int*)r->data)[0];
		sprintf(cmdString, "AT+CMUT=%d", mute);
		ril_handle_cmd_default_response(cmdString, r->token);
		break;
	}

	case RIL_REQUEST_GET_MUTE:
	{
		ril_handle_cmd_one_int("AT+CMUT?", "+CMUT:", r->token);
		break;
	}

	case RIL_REQUEST_SEPARATE_CONNECTION:
	{
		int party = ((int*)r->data)[0];
		// Make sure that party is in a valid range.
		// (Note: The Telephony middle layer imposes a range of 1 to 7.
		// It's sufficient for us to just make sure it's single digit.)
		if (party <= 0 || party >= 10)
			party = 1;
		sprintf(cmdString, "AT+CHLD=2%d", party);
		ril_handle_cmd_default_response(cmdString, r->token);
		break;
	}

	case RIL_REQUEST_EXPLICIT_CALL_TRANSFER:
	{
		ril_handle_cmd_default_response("AT+CHLD=4", r->token);
		break;
	}
#ifdef MARVELL_EXTENDED
	case RIL_REQUEST_DIAL_VT:
	{
		ril_request_dial(r->request, r->data, r->datalen, r->token);
		break;
	}

	case RIL_REQUEST_HANGUP_VT:
	{
		sprintf(cmdString, "AT+ECHUPVT=%d", *(int *)r->data);
		ril_handle_cmd_default_response(cmdString, r->token);
		break;
	}

	case RIL_REQUEST_SET_ACM:
	{
		const char** strings = (const char**)r->data;
		switch (r->datalen/sizeof(char *))  {
			case 1:
				sprintf(cmdString, "AT+CACM=%s", strings[0]);
				break;
			default:
				sprintf(cmdString, "AT+CACM=");
				break;
		}
		ril_handle_cmd_default_response(cmdString, r->token);
		break;
	}

	case RIL_REQUEST_GET_ACM:
	{
		ril_handle_cmd_GetCallCost("AT+CACM", "+CACM:", r->token, r->request);
		break;
	}

	case RIL_REQUEST_SET_AMM:
	{
		const char** strings = (const char**)r->data;
		switch (r->datalen/sizeof(char *))  {
			case 1:
				sprintf(cmdString, "AT+CAMM=%s", strings[0]);
				break;
			case 2:
				sprintf(cmdString, "AT+CAMM=%s, %s", strings[0], strings[1] );
				break;
			default:
				sprintf(cmdString, "AT+CAMM=");
				break;
		}
		ril_handle_cmd_default_response(cmdString, r->token);
		break;
	}
	case RIL_REQUEST_GET_AMM:
	{
		ril_handle_cmd_GetCallCost("AT+CAMM", "+CAMM:", r->token, r->request);
		break;
	}

	case RIL_REQUEST_SET_CPUC:
	{
		const char** strings = (const char**)r->data;
		switch (r->datalen/sizeof(char *)) {
			case 2:
				sprintf(cmdString, "AT+CPUC=%s,%s", strings[0], strings[1]);
				break;
			case 3:
				sprintf(cmdString, "AT+CPUC=%s,%s,%s", strings[0], strings[1], strings[2]);
				break;
			default:
				sprintf(cmdString, "AT+CPUC=");
				break;
		}
		ril_handle_cmd_default_response(cmdString, r->token);
		break;
	}

	case RIL_REQUEST_GET_CPUC:
	{
		ril_handle_cmd_GetCallCost("AT+CPUC", "+CPUC:", r->token, r->request);
		break;
	}
#endif
	default:
	{
		LOGW("%s:invalid request:%d\n", __FUNCTION__, r->request);
		break;
	}
	}
	
bail_out:
	if (request == RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND || 
	    request == RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE || 
	    request ==RIL_REQUEST_CONFERENCE)
		__atomic_dec(&suppress_dtmf);

	freeRILRequest(r);
}

void onRequest_cc (int request, void *data, size_t datalen, RIL_Token token)
{
	LOGV("%s entry: request = %d", __FUNCTION__, request);
	if (request == RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND || 
	    request == RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE || 
	    request ==RIL_REQUEST_CONFERENCE)
		__atomic_inc(&suppress_dtmf);
	enque(getWorkQueue(SERVICE_CC), ccProcessRequest, (void*)newRILRequest(request, data, datalen, token));
}


void onUnsolicited_cc (const char *s)
{
	if (strStartsWith(s, "+CRING:") || strStartsWith(s, "RING")
	    || strStartsWith(s, "NO CARRIER") || strStartsWith(s, "+CCWA:")
	    || strStartsWith(s, "+CLIP:") || strStartsWith(s, "+CNAP:"))
	{
		RIL_onUnsolicitedResponse( RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED, NULL, 0);
	}

	else if (strStartsWith(s, "+CLCC:") )
	{
		RIL_onUnsolicitedResponse( RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED, NULL, 0);
	}
}


