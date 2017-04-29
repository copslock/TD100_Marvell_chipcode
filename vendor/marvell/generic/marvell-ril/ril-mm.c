/******************************************************************************
 * *(C) Copyright 2008 Marvell International Ltd.
 * * All Rights Reserved
 * ******************************************************************************/

#include <cutils/properties.h>
#include <sys/atomics.h>
#include <sys/time.h>

#include "at_tok.h"
#include "atchannel.h"
#include "marvell-ril.h"
#include "misc.h"
#include "ril-requestdatahandler.h"
#include "work-queue.h"

typedef struct RegState_s
{
	int stat;
	int lac;
	int cid;
} RegState;

typedef struct OperInfo_s
{
	int mode;
	char operLongStr[20];
	char operShortStr[10];
	char operNumStr[10];
	int act;
} OperInfo;

static RegState sCregState = { -1, -1, -1 }, sCgregState = { -1, -1, -1 };
static OperInfo sOperInfo = { -1, "\0", "\0", "\0", -1 };
static int sScreenState = 0;   //default screen state = OFF, it will turn to ON after RIL is initialized
static int sCSQ[2] = { 99, 99 };
int AfterCPReset = 0;
#define MAX_NEIGHBORING_CELLS 6 //max neighboring cell number is set as 6 in defualt
static RIL_NeighboringCell sNeighboringCell[MAX_NEIGHBORING_CELLS]; 
static int sCellNumber = 0;
struct timeval lastNitzTimeReceived;
char lastNitzTime[18];

#define MM_NETOP_ID_FORMAT_ALPHA_LONG 0
#define MM_NETOP_ID_FORMAT_ALPHA_SHORT 1
/*
 * RIL request network type
 *
 * 0 for GSM/WCDMA (WCDMA preferred)
 * 1 for GSM only
 * 2 for WCDMA only
 * 3 for GSM/WCDMA (auto mode, according to PRL)
 * 4 for CDMA and EvDo (auto mode, according to PRL)
 * 5 for CDMA only
 * 6 for EvDo only
 * 7 for GSM/WCDMA, CDMA, and EvDo (auto mode, according to PRL)
 * 8 for Dual mode (GSM preferred)
 */

/*
 * AT*BAND network Mode
 *
 * 0 for GSM only
 * 1 for WCDMA only
 * 2 for GSM/WCDMA (auto mode)
 * 3 for GSM/WCDMA (GSM preferred)
 * 4 for GSM/WCDMA (WCDMA preferred)
 */


/* map ril request network type to AT*BAND network mode */
static const int RILNetworkTypeTostarBandNetworkType[9] =
{
	4,  /* GSM/WCDMA (WCDMA preferred) */
	0,  /* GSM only */
	1,  /* WCDMA only */
	2,  /* GSM/WCDMA (auto mode, according to PRL) */
	-1, /* CDMA and EvDo (auto mode, according to PRL) */
	-1, /* CDMA only */
	-1, /* EvDo only */
	-1,  /* GSM/WCDMA, CDMA, and EvDo (auto mode, according to PRL) */
	3   /* Dual mode (GSM preferred) */
};

/* map AT*BAND network mode to ril request network type */
static const int starBandNetworkTypeToRILNetworkType[5] =
{
	1,  /* GSM only */
	2,  /* WCDMA only */
	3,  /* GSM/WCDMA (auto mode) */
	8, /* GSM/WCDMA (GSM preferred) */
	0   /* GSM/WCDMA (WCDMA preferred) */
};


/* Parse  AT reply of +CREG or +CGREG
 * Output: responseInt[0] : <stat>
		  responseInt[1] : <lac>
		  responseInt[2] : <cid>
		  responseInt[3] : <AcT>
 */
int parseResponseWithMoreInt(ATResponse* response, int responseInt[], int *pNum)
{
	int err = 0, num;
	char *line, *p;

	if (response->success == 0 || response->p_intermediates == NULL)
	{
		goto error;
	}

	line = response->p_intermediates->line;
	err = at_tok_start(&line);
	if (err < 0) goto error;

	num = 0;
	while (at_tok_hasmore(&line))
	{
		if(num == 2 || num == 3) //for <lac>,<cid>
		{
			err = at_tok_nexthexint(&line, &(responseInt[num]));
		}
		else
		{
			err = at_tok_nextint(&line, &(responseInt[num]));
		}
		if (err < 0) goto error;
		num++;
	}

	/* AT Reply format: +CREG: <n>,<stat>[,<lac>,<ci>[,<AcT>]] (Take +CREG: as example in following comments)   */
	switch (num)
	{
	case 2: /* +CREG: <n>, <stat> */
	{
		/* responseInt[1] is stat, copy to responseInt[0]. <lac> and <ci> are unavailable, <AcT> is unknown  */
		responseInt[0] = responseInt[1];
		responseInt[1] = -1;
		responseInt[2] = -1;
		responseInt[3] = -1;
		break;
	}
	case 4: /* +CREG: <n>, <stat>, <lac>, <cid> */
	case 5: /* +CREG: <n>, <stat>, <lac>, <cid>, <AcT> */
	{
		/* Need to change the place */
		responseInt[0] = responseInt[1];
		responseInt[1] = responseInt[2];
		responseInt[2] = responseInt[3];
		if (num == 5)
		{
			responseInt[3] = responseInt[4];
		}
		else
		{
			responseInt[3] = -1;
		}
		break;
	}

	default:
		goto error;
	}

	*pNum = num;

	return 0;

 error:
	return err;
}

/* Report to upper layer about signal strength (unsol msg of  +CSQ: ) when screen state is ON */
void reportSignalStrength(void *param)
{
	if (sScreenState)
	{
		RIL_onUnsolicitedResponse(RIL_UNSOL_SIGNAL_STRENGTH, sCSQ, sizeof(sCSQ));
	}
}

/* Reset all local saved reginfo and operInfo to NULL, force to update by AT cmd */
void resetLocalRegInfo(void)
{
	sOperInfo.mode = -1;
	sOperInfo.operLongStr[0] = '\0';
	sOperInfo.operShortStr[0] = '\0';
	sOperInfo.operNumStr[0] = '\0';
	sOperInfo.act = -1;
	sCregState.stat = -1;
	sCgregState.stat = -1;
}

static inline void setScreenState(int state)
{
	__atomic_swap(state, &sScreenState);
}

/* Set flag whether permit to report CSQ or CREG/CGREG ind msg to RIL */
void setNetworkStateReportOption(int flag)
{
	setScreenState(flag);
}

/* Get registe state: return 1: registered, 0: unregistered */
int isRegistered(void)
{
	int regState;

	if ((sCregState.stat == 1) || (sCregState.stat == 5))
	{
		regState = 1;
	}
	else
	{
		regState = 0;
	}
	return regState;
}

/* Update Local Reg Info, if reg info changed, report network change unsol msg to upper layer  */
void updateLocalRegInfo(void *param)
{
	ATResponse *p_response = NULL;
	int responseInt[10], err, num;
	int oldRegState = sCregState.stat;
	static int query_times = 0;

	/* Update CREG info */
	err = at_send_command_singleline("AT+CREG?", "+CREG:", &p_response);
	if (err < 0 || p_response->success == 0) goto error;

	err = parseResponseWithMoreInt(p_response, responseInt, &num);
	if (err < 0) goto error;

	if (responseInt[0] > 5) responseInt[0] = 10; //Register state extention: 10 - Same as 0, but indicates that emergency calls are enabled

	/* Save latest reg status locally */
	sCregState.stat = responseInt[0];
	sCregState.lac  = responseInt[1];
	sCregState.cid  = responseInt[2];

	at_response_free(p_response);
	p_response = NULL;

	/* Update CGREG info */
	err = at_send_command_singleline("AT+CGREG?", "+CGREG:", &p_response);
	if (err < 0 || p_response->success == 0) goto error;

	err = parseResponseWithMoreInt(p_response, responseInt, &num);
	if (err < 0) goto error;

	/* Save latest gprs reg status locally */
	sCgregState.stat = responseInt[0];
	sCgregState.lac  = responseInt[1];
	sCgregState.cid  = responseInt[2];
	if (num == 5)
		sOperInfo.act = responseInt[3];

	if (oldRegState != sCregState.stat)
	{
		RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED, NULL, 0);
		query_times = 0;
	}

	else
	{
		query_times++;
		if ((query_times < 10) &&  (!isRegistered()))
		{
			const struct timeval TIMEVAL_15s = { 15, 0 };
			enqueDelayed(getWorkQueue(SERVICE_MM),updateLocalRegInfo, NULL, &TIMEVAL_15s);
		}
		else
		{
			query_times = 0;
		}
	}

	at_response_free(p_response);
	return;

 error:
	sCregState.stat = -1;
	sCgregState.stat = -1;
	LOGE("%s: Error in sending this AT response", __FUNCTION__);
	at_response_free(p_response);
	return;
}

/* Convert AcT value in AT cmd to GPRS reg value defined in ril.h
 *  Input para: AcT (3GPP spec definition):
 *      0	GSM
 *      1	GSM Compact
 *      2	UTRAN
 *      3	GSM w/EGPRS (see NOTE 1)
 *      4	UTRAN w/HSDPA (see NOTE 2)
 *      5	UTRAN w/HSUPA (see NOTE 2)
 *      6	UTRAN w/HSDPA and HSUPA (see NOTE 2)
 *  Output para: state (Refer to ril.h, RIL_REQUEST_GPRS_REGISTRATION_STATE)
  *      0 == unknown
 *      1 == GPRS only
 *      2 == EDGE
 *      3 == UMTS
 *      9 == HSDPA
 *      10 == HSUPA
 *      11 == HSPA
 */
static void libConvertActToRilState(int AcT, char *state)
{

	switch(AcT)
	{
		case 0:
		case 1:
			strcpy(state, "1"); //GPRS only
			break;
			
		case 2: 
			strcpy(state, "3"); // UMTS
			break;

		case 3:
			strcpy(state, "2"); //EDGE
			break;

		case 4:
			strcpy(state, "9"); //HSDPA
			break;

		case 5:
			strcpy(state, "10"); //HSUPA
			break;
			
		case 6:
			strcpy(state, "11"); //HSPA
			break;

		case -1:
		default:
			strcpy(state, "0"); //unknown
			break;
	}

	return;
}

static int getNeighboringCellId(RIL_Token token)
{
	int onOff, err;
	ATResponse *p_response = NULL;

	//if screen state is OFF, CP will not indicate CELL IDS
	if (!sScreenState)
	{
		RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
		return 0;
	}
			
	//save mode 
	err = at_send_command("AT+EEMOPT=3", &p_response);
	if (err < 0 || p_response->success == 0) goto error;

	at_response_free(p_response);

	//Set query mode
	err = at_send_command("AT+EEMOPT=1", &p_response);
	if (err < 0 || p_response->success == 0) goto error;

	at_response_free(p_response);
	return 1;

 error:
	at_response_free(p_response);
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
	return 0;
}

static void ril_request_screen_state(int request, void *data, size_t datalen, RIL_Token token)
{
	setScreenState(((int *)data)[0]);
	if (sScreenState)
		//notify CP that AP will wake
		at_send_command( "AT*POWERIND=0", NULL);
	else
		//notify CP that AP will sleep
		at_send_command("AT*POWERIND=1", NULL);

	RIL_onRequestComplete(token, RIL_E_SUCCESS, NULL, 0);
}

/**
 * RIL_REQUEST_REGISTRATION_STATE
 *
 * Request current registration state.
 */
static void ril_request_registration_state(int request, void *data, size_t datalen, RIL_Token token)
{
	if ((sCregState.stat != -1) && (sCregState.lac != -1) && (sOperInfo.act != -1))
	{
		char * responseStr[4];
		char AccessTech[3];
		asprintf(&responseStr[0], "%d", sCregState.stat);
		asprintf(&responseStr[1], "%x", sCregState.lac);
		asprintf(&responseStr[2], "%x", sCregState.cid);

		/* Convert AcT value of 3GPP spec to GPRS reg state defined in ril.h */
		libConvertActToRilState(sOperInfo.act, AccessTech);
		responseStr[3] = AccessTech;

		LOGD("%s: Return local saved registration state", __FUNCTION__);
		RIL_onRequestComplete(token, RIL_E_SUCCESS, responseStr, sizeof(responseStr));
		FREE(responseStr[0]);
		FREE(responseStr[1]);
		FREE(responseStr[2]);
		return;
	}

	ATResponse* response = NULL;
	int responseInt[10];
	char * responseStr[4],radiotech[3];
	const char *cmd;
	const char *prefix;
	int num, err;
	
	err = at_send_command_singleline("AT+CREG?", "+CREG:", &response);
	if (err < 0 || response->success == 0)
        	goto error;
        
	err = parseResponseWithMoreInt(response, responseInt, &num);
	if (err < 0) goto error;

	if (responseInt[0] > 5) responseInt[0] = 10; //Register state extention: 10 - Same as 0, but indicates that emergency calls are enabled

	asprintf(&responseStr[0], "%d", responseInt[0]);
	if(num > 2)
	{
	       asprintf(&responseStr[1], "%x", responseInt[1]);
	       asprintf(&responseStr[2], "%x", responseInt[2]);
	}
	else
	{
	       responseStr[1] = NULL;
	       responseStr[2] = NULL;
	}

	if(num == 5)
	{
	       libConvertActToRilState(responseInt[3], radiotech);
	       responseStr[3] = radiotech;
	}
	else
	       responseStr[3] = NULL;

	RIL_onRequestComplete(token, RIL_E_SUCCESS, responseStr, sizeof(responseStr));
	FREE(responseStr[0]);
	FREE(responseStr[1]);
	FREE(responseStr[2]);

	/* Save latest reg status locally */
	sCregState.stat = responseInt[0];
	sCregState.lac  = responseInt[1];
	sCregState.cid  = responseInt[2];
	if (num == 5)
	       sOperInfo.act = responseInt[3];

	goto exit;
error:
	LOGE("%s: Format error in this AT response", __FUNCTION__);
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);

}

static void ril_request_gprs_registration_state(int request, void *data, size_t datalen, RIL_Token token)
{
	if ((sCgregState.stat != -1) && (sCgregState.lac != -1) && (sOperInfo.act != -1))
	{
		char * responseStr[4];
		char gprsState[3];
		asprintf(&responseStr[0], "%d", sCgregState.stat);
		asprintf(&responseStr[1], "%x", sCgregState.lac);
		asprintf(&responseStr[2], "%x", sCgregState.cid);
		/* Convert AcT value of 3GPP spec to GPRS reg state defined in ril.h */
		libConvertActToRilState(sOperInfo.act, gprsState);
		responseStr[3] = gprsState;
	
		LOGD("%s: Return local saved GPRS registration state. GPRS state=%s", __FUNCTION__, responseStr[3]);
		RIL_onRequestComplete(token, RIL_E_SUCCESS, responseStr, sizeof(responseStr));
		FREE(responseStr[0]);
		FREE(responseStr[1]);
		FREE(responseStr[2]);
		return;
	}

	ATResponse* response = NULL;
	int responseInt[10];
	char *responseStr[4], gprsState[3];
	const char *cmd;
	const char *prefix;
	int num, err;

	err = at_send_command_singleline("AT+CGREG?", "+CGREG:", &response);
	if (err < 0 || response->success == 0)
        	goto error;
        
	err = parseResponseWithMoreInt(response, responseInt, &num);
	if (err < 0) goto error;

	asprintf(&responseStr[0], "%d", responseInt[0]);
	if(num > 2)
	{
	       asprintf(&responseStr[1], "%x", responseInt[1]);
	       asprintf(&responseStr[2], "%x", responseInt[2]);
	}
	else
	{
	       responseStr[1] = NULL;
	       responseStr[2] = NULL;
	}

	if(num == 5)
	{
	       /* Convert AcT value of 3GPP spec to GPRS reg state defined in ril.h */
	       libConvertActToRilState(responseInt[3], gprsState);
	       responseStr[3] = gprsState;
	}
	else
	       responseStr[3] = NULL;

	RIL_onRequestComplete(token, RIL_E_SUCCESS, responseStr, sizeof(responseStr));
	FREE(responseStr[0]);
	FREE(responseStr[1]);
	FREE(responseStr[2]);

	/* Save latest reg status locally */
	sCgregState.stat = responseInt[0];
	sCgregState.lac	= responseInt[1];
	sCgregState.cid	= responseInt[2];
	if (num == 5)
	       sOperInfo.act = responseInt[3];
	goto exit;

error:
	LOGE("%s: Format error in this AT response", __FUNCTION__);
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);
}

/**
 * RIL_REQUEST_OPERATOR
 *
 * Request current operator ONS or EONS.
 */
static void ril_request_operator(int request, void *data, size_t datalen, RIL_Token token)
{
	if (!isRegistered())
	{
		char *result[3];
		result[0] =  "";
		result[1] =  "";
		result[2] =  "";
	
		LOGD("%s: unregistered, return empty info", __FUNCTION__);
		RIL_onRequestComplete(token, RIL_E_SUCCESS, result, sizeof(result));
		return;
	}
	
	if ( (sOperInfo.operLongStr[0] != '\0') && (sOperInfo.operShortStr[0] != '\0') &&  (sOperInfo.operNumStr[0] != '\0') )
	{
		char *result[3];
		result[0] =  &(sOperInfo.operLongStr[0]);
		result[1] =  &(sOperInfo.operShortStr[0]);
		result[2] =  &(sOperInfo.operNumStr[0]);
		LOGD("%s: Return local saved operator info", __FUNCTION__);
		RIL_onRequestComplete(token, RIL_E_SUCCESS, result, sizeof(result));
		return;
	}
	
	ATResponse* response = NULL;
	int err;
	int i;
	int skip;
	ATLine *p_cur;
	char *result[3];

	err = at_send_command_multiline("AT+COPS=3,0;+COPS?;+COPS=3,1;+COPS?;+COPS=3,2;+COPS?", "+COPS:", &response);
	if (err < 0 || response->success == 0)
        	goto error;
        
	if (strStartsWith(response->finalResponse, "+CME ERROR:") || response->p_intermediates == NULL)
	{
	       goto error;
	}

	memset(result, 0, sizeof(result));

	for (i = 0, p_cur = response->p_intermediates; p_cur != NULL; p_cur = p_cur->p_next, i++ )
	{
	       char *line = p_cur->line;

	       err = at_tok_start(&line);
	       if (err < 0) goto error;

	       err = at_tok_nextint(&line, &(sOperInfo.mode));
	       if (err < 0) goto error;

	       // If we're unregistered, we may just get a "+COPS: 0" response
	       if (!at_tok_hasmore(&line))
	       {
		       goto unregistered;
	       }

	       err = at_tok_nextint(&line, &skip);
	       if (err < 0) goto error;

	       // a "+COPS: 0, n" response is also possible
	       if (!at_tok_hasmore(&line))
	       {
		       result[i] = NULL;
		       continue;
	       }

	       err = at_tok_nextstr(&line, &(result[i]));
	       if (err < 0) goto error;

	       if (at_tok_hasmore(&line))
	       {
		       err = at_tok_nextint(&line, &(sOperInfo.act));
		       if (err < 0) goto error;
	       }
	}

	/* expect 3 lines exactly */
	if (i != 3) goto error;

	/* Save operator info locally */
	strcpy(sOperInfo.operLongStr, result[0]);
	strcpy(sOperInfo.operShortStr, result[1]);
	strcpy(sOperInfo.operNumStr, result[2]);

	RIL_onRequestComplete(token, RIL_E_SUCCESS, result, sizeof(result));

	/* [Jerry, 2009/01/06] When operator name is available, the network should be registered.
	* If CP doesn't send indication msg, we need to query CP and update reg info
	*/
#if 0 
	if ((sCregState.stat != 1) && (sCregState.stat != 5))
	{
	       const struct timeval TIMEVAL_30s = { 30, 0 };
	       RIL_requestTimedCallback(updateLocalRegInfo, NULL, &TIMEVAL_30s);
	}
#endif
	goto exit ;

unregistered:
	LOGD("RIL_REQUEST_OPERATOR callback: network not registered");
	/* The reason to return RIL_E_SUCCESS instead of RADIO_NOT_AVAILABLE:
	* GsmServiceStateTracker handlePollStateResult() will cancelPollState if RADIO_NOT_AVAILABLE
	* is received, which will cause phone.notifyServiceStateChanged() in pollStateDone() never be called,
	* and it is root cause why after enabling airplane mode, the screen keeps waiting
	*/
	result[0] = NULL;
	result[1] = NULL;
	result[2] = NULL;
	RIL_onRequestComplete(token, RIL_E_SUCCESS, result, sizeof(result));
	goto exit;

error:
	LOGE("%s: Error in this AT response", __FUNCTION__);
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);
}

/**
 * RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE
 *
 * Query current network selectin mode.
 */
static void ril_request_query_network_selection_mode(int request, void *data, size_t datalen, RIL_Token token)
{
	ATResponse* response = NULL;
	int err;
	int result = 0;
	char *line;
	
	err = at_send_command_singleline("AT+COPS?", "+COPS:", &response);
	if (err < 0 || response->success == 0 || response->p_intermediates == NULL)
	       goto error;

	line = response->p_intermediates->line;

	err = at_tok_start(&line);

	if (err < 0)
	{
	       goto error;
	}

	err = at_tok_nextint(&line, &result);

	if (err < 0)
	{
	       goto error;
	}

	RIL_onRequestComplete(token, RIL_E_SUCCESS, &result, sizeof(int));

	sOperInfo.mode = result;
	goto exit;
error:
	LOGE("%s: Respond error, return default value 0: auto selection", __FUNCTION__);
	result = 0;
	RIL_onRequestComplete(token, RIL_E_SUCCESS, &result, sizeof(int));
exit:
	at_response_free(response);
}

/**
 * RIL_REQUEST_SIGNAL_STRENGTH
 *
 * Requests current signal strength and bit error rate.
 *
 * Must succeed if radio is on.
 */
static void ril_request_signal_strength(int request, void *data, size_t datalen, RIL_Token token)
{
    ATResponse *response = NULL;
    int err;
    int result[2];
    char *line;
       
    err = at_send_command_singleline("AT+CSQ", "+CSQ:", &response);
    if (err < 0 || response->success == 0)
        goto error;

    line = response->p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &result[0]);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &result[1]);
    if (err < 0)
        goto error;

    
    RIL_onRequestComplete(token, RIL_E_SUCCESS, result, sizeof(result));
    goto exit;

error:
    RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
    at_response_free(response);
}

/**
 * RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC
 *
 * Specify that the network should be selected automatically.
 */
static void ril_request_set_network_selection_automatic(int request, void *data, size_t datalen, RIL_Token token)
{
	ATResponse *response = NULL;
	int err;
	int result[2];
	char *line;
	
	resetLocalRegInfo();
	err = at_send_command("AT+COPS=0", &response);
	if (err < 0 || response->success == 0)
	    goto error;
	RIL_onRequestComplete(token, RIL_E_SUCCESS, NULL, 0);
	
	RIL_onUnsolicitedResponse( RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED, NULL, 0);
	goto exit;
error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);
	
}

/**
 * RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL
 *
 * Manually select a specified network.
 *
 * The radio baseband/RIL implementation will try to camp on the manually
 * selected network regardless of coverage, i.e. there is no fallback to
 * automatic network selection.
 */
static void ril_request_set_network_selection_manual(int request, void *data, size_t datalen, RIL_Token token)
{
	ATResponse *response = NULL;
	int err;
	int result[2];
	char *cmd, *line;
	
	resetLocalRegInfo();
	
	asprintf(&cmd, "AT+COPS=1,2,%s", (const char*)data);
	err = at_send_command(cmd, &response);
	free(cmd);
	if (err < 0 || response->success == 0)
	    goto error;
	RIL_onRequestComplete(token, RIL_E_SUCCESS, NULL, 0);
	
	RIL_onUnsolicitedResponse( RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED, NULL, 0);
	goto exit;
error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);
}

/**
 * RIL_REQUEST_QUERY_AVAILABLE_NETWORKS
 *
 * Scans for available networks.
 */
static void ril_request_query_available_networks(int request, void *data, size_t datalen, RIL_Token token)
{
	ATResponse* response = NULL;
	int err, availableOptNumber, lparen;
	char *line, *p;
	char** result;
	
	err = at_send_command_singleline("AT+COPS=?", "+COPS:", &response);
	if (err < 0 || response->success == 0 || response->p_intermediates == NULL)
		goto error;

	line = response->p_intermediates->line;

	err = at_tok_start(&line);
	if (err < 0) goto error;

	/* count number of lparen */
	lparen = 0;
	for (p = line; *p != '\0'; p++)
	{
		if (*p == '(') lparen++;
	}

	/*
	 * the response is +COPS:(op1),(op2),...(opn),,(0,1,2,3,4),(0-6)
	 * so available operator count should be num_of_left_parentheses - 2
	 */
	if(lparen > 1)
		availableOptNumber = lparen - 2;
	else
		availableOptNumber = 0;
	LOGV("%s: available operator number:%d", __FUNCTION__, availableOptNumber);
	result = alloca(availableOptNumber * 5 * sizeof(char*));

	int i = 0;
	while (i < availableOptNumber)
	{
		char* status, *longname, *shortname, *numbername, *act;
		at_tok_nextstr(&line, &status);
		LOGV("status:%s", status);
		switch (status[1])
		{
		case '1':
			result[i * 5 + 3] = "available";
			break;
		case '2':
			result[i * 5 + 3] = "current";
			break;
		case '3':
			result[i * 5 + 3] = "forbidden";
			break;
		default:
			result[i * 5 + 3] = "unknown";
		}
		LOGV("state:%s", result[i * 5 + 3]);
		at_tok_nextstr(&line, &result[i * 5]);
		LOGV("longname:%s", result[i * 5]);
		at_tok_nextstr(&line, &result[i * 5 + 1]);
		LOGV("shortname:%s", result[i * 5 + 1]);
		at_tok_nextstr(&line, &result[i * 5 + 2]);
		LOGV("numbername:%s", result[i * 5 + 2]);
		at_tok_nextstr(&line, &act);
		LOGV("act:%s", act);
		switch (act[0])
		{
		case '0':
			result[i * 5 + 4] = "GSM";
			break;
		case '1':
			result[i * 5 + 4] = "GSM_COMPACT";
			break;
		case '2':
			result[i * 5 + 4] = "UTRAN";
			break;
		default:
			result[i * 5 + 4] = "UNKNOWN";
		}
		LOGV("act:%s", result[i * 5 + 4]);
		i++;
	}

	RIL_onRequestComplete(token, RIL_E_SUCCESS, result, sizeof(char*) * availableOptNumber * 5);
	goto exit;

error:
	LOGE("%s: Format error in this AT response", __FUNCTION__);
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);

}

/**
 * RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE
 *
 * Requests to set the preferred network type for searching and registering
 * (CS/PS domain, RAT, and operation mode).
 */
static void ril_request_set_preferred_network_type(int request, void *data, size_t datalen, RIL_Token token)
{
	int networkType;
	int starBandNetworkType;
	networkType = ((int *)data)[0];
	
	if(networkType < 0 || networkType >= (int)sizeof(RILNetworkTypeTostarBandNetworkType)/(int)sizeof(RILNetworkTypeTostarBandNetworkType[0]))
		starBandNetworkType = -1;
	else
		starBandNetworkType = RILNetworkTypeTostarBandNetworkType[networkType];
	
	if(starBandNetworkType < 0)
	{
		RIL_onRequestComplete(token, RIL_E_MODE_NOT_SUPPORTED, NULL, 0);
		return;
	}
	ATResponse* response = NULL;
	char *cmd;
	asprintf(&cmd, "AT*BAND=%d", starBandNetworkType);
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
}

/**
 * RIL_REQUEST_NEIGHBORINGCELL_IDS
 */
static void ril_request_get_neighboring_cell_ids(int request, void *data, size_t datalen, RIL_Token token)
{
	int onOff, err;
	ATResponse *response = NULL;
	int mode, network;
	char *line;
	const struct timeval TIMEVAL_2s = { 1, 0 };

	//if screen state is OFF, CP will not indicate CELL IDS
	if (!sScreenState) goto error;
		       
	//save mode 
	err = at_send_command("AT+EEMOPT=3", &response);
	if (err < 0 || response->success == 0) goto error;
	at_response_free(response);

	//Set query mode
	err = at_send_command("AT+EEMOPT=1", &response);
	if (err < 0 || response->success == 0) goto error;
	at_response_free(response);
	
	// move forward or it would be late in multi-thread arc.
	//reset cell info counter
	sCellNumber = 0;

	err = at_send_command_singleline("AT+EEMGINFO?", "+EEMGINFO:", &response);
	if (err < 0 || response->success == 0) goto error;

	//check network type UMTS or GSM
	line = response->p_intermediates->line;

	err = at_tok_start(&line);
	if (err < 0) goto error;

	err = at_tok_nextint(&line, &mode);
	if (err < 0) goto error;
	LOGV("EngModeinfo mode:%d", mode);

	err = at_tok_nextint(&line, &network);
	if (err < 0) goto error;
	LOGV("EngModeinfo network:%d", network);

	//reset cell info counter
	//sCellNumber = 0;
	
	//wait for unsolicited thread to collect cell info, 
	sleep(1);
	
	RIL_NeighboringCell * result[MAX_NEIGHBORING_CELLS];
	int i = 0;

	LOGI("reportCellInfo sCellNumber: %d", sCellNumber);

	if(sCellNumber > 0 && sCellNumber <= MAX_NEIGHBORING_CELLS)
	{
		for(i = 0; i < sCellNumber; i++)
			result[i] = &sNeighboringCell[i];
		
		RIL_onRequestComplete(token, RIL_E_SUCCESS, result, sCellNumber* sizeof(RIL_NeighboringCell *));
		goto exit;
	}
	else goto error;


error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);
	//restore Engineering mode 
	at_send_command("AT+EEMOPT=4", NULL);
}

/**
 * RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE
 *
 * Query the preferred network type (CS/PS domain, RAT, and operation mode)
 * for searching and registering.
 */
static void ril_request_get_preferred_network_type(int request, void *data, size_t datalen, RIL_Token token)
{
	ATResponse *response = NULL;
	int result[1];
	int mode;
	int rilNetworkType;
	char *line;
	int err;
	
	err = at_send_command_singleline("AT*BAND?", "*BAND:", &response);
	if (err < 0 || response->success == 0)
		goto error;

	line = response->p_intermediates->line;

	err = at_tok_start(&line);
	if (err < 0) goto error;

	err = at_tok_nextint(&line, &mode);
	if (err < 0) goto error;

	if(mode < 0 || mode >= (int)sizeof(starBandNetworkTypeToRILNetworkType)/(int)sizeof(starBandNetworkTypeToRILNetworkType[0]))
		rilNetworkType = -1;
	else
		rilNetworkType = starBandNetworkTypeToRILNetworkType[mode];

	if(rilNetworkType < 0) goto error;

	LOGI("%s: Preferred Network Type: %d", __FUNCTION__, rilNetworkType);
	result[0] = rilNetworkType;

	RIL_onRequestComplete(token, RIL_E_SUCCESS, result, sizeof(result));

	goto exit;

error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);
}

/**
 * RIL_REQUEST_SET_LOCATION_UPDATES
 *
 * Enables/disables network state change notifications due to changes in
 * LAC and/or CID (basically, *EREG=2 vs. *EREG=1).
 *
 * Note:  The RIL implementation should default to "updates enabled"
 * when the screen is on and "updates disabled" when the screen is off.
 *
 * See also: RIL_REQUEST_SCREEN_STATE, RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED.
 */
static void ril_request_set_location_updates(int request, void *data, size_t datalen, RIL_Token token)
{
	ATResponse *response = NULL;
	char* cmd;
	asprintf(&cmd, "AT+CREG=%d", ((int *)data)[0]);
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
}

#ifdef MARVELL_EXTENDED
static void ril_request_select_band(int request, void *data, size_t datalen, RIL_Token token)
{
	ATResponse *response = NULL;
	int req = ((int *)data)[0];
	int mode = -1;
	int ril_mode = -1;
	int band;
	char* cmd;
	
	/* linda use the High bytes for general band setting marvell-ril.h */
	if((req & 0xFFFF0000) != 0)
	{
		ril_mode = (req>>16) & 0x000000FF;
		
		if(ril_mode < 0 || ril_mode >= (int)sizeof(RILNetworkTypeTostarBandNetworkType)/(int)sizeof(RILNetworkTypeTostarBandNetworkType[0]))
		    mode = -1;
		else
		    mode = RILNetworkTypeTostarBandNetworkType[ril_mode];
		
		band = (req & 0x0000FFFF);
	}
	else /* LGE requirement */
	{
		switch(req)
		{
		case 1: /* GSM900 */
		{
			mode = MODE_GSM_ONLY;
			band = GSMBAND_PGSM_900;
			break;
		}

		case 2: /* GSM850 */
		{
			mode = MODE_GSM_ONLY;
			band = GSMBAND_GSM_850;
			break;
		}

		case 3: /* PCS */
		{
			mode = MODE_GSM_ONLY;
			band = GSMBAND_PCS_GSM_1900;
			break;
		}

		case 4: /* DCS */
		{
			mode = MODE_GSM_ONLY;
			band = GSMBAND_DCS_GSM_1800;
			break;
		}

		case 6: /* GSM ONLY */
		{
			mode = MODE_GSM_ONLY;
			band = 0;
			break;
		}

		case 7: /* WCDMA I */
		{
			mode = MODE_UMTS_ONLY;
			band = UMTSBAND_BAND_1;
			break;
		}

		case 8: /* WCDMA II */
		{
			mode = MODE_UMTS_ONLY;
			band = UMTSBAND_BAND_2;
			break;
		}

		case 9: /* WCDMA III */
		{
			mode = MODE_UMTS_ONLY;
			band = UMTSBAND_BAND_3;
			break;
		}

		case 10: /* WCDMA IV */
		{
			mode = MODE_UMTS_ONLY;
			band = UMTSBAND_BAND_4;
			break;
		}

		case 11: /* WCDMA V */
		{
			mode = MODE_UMTS_ONLY;
			band = UMTSBAND_BAND_5;
			break;
		}

		case 12: /* WCDMA VI */
		{
			mode = MODE_UMTS_ONLY;
			band = UMTSBAND_BAND_6;
			break;
		}

		case 13: /* WCDMA VII */
		{
			mode = MODE_UMTS_ONLY;
			band = UMTSBAND_BAND_7;
			break;
		}

		case 14: /* WCDMA VIII */
		{
			mode = MODE_UMTS_ONLY;
			band = UMTSBAND_BAND_8;
			break;
		}

		case 15: /* WCDMA VIIII */
		{
			mode = MODE_UMTS_ONLY;
			band = UMTSBAND_BAND_9;
			break;
		}

		case 16: /* WCDMA X */
		{
			/* not support */
			mode = -1;
			break;
		}

		case 17: /* WCDMA ONLY */
		{
			mode = MODE_UMTS_ONLY;
			band = 0;
			break;
		}

		case 18: /* Automatic */
		{
			mode = MODE_DUAL_MODE_AUTO;
			band = 0;
			break;
		}
		
		default:
		{
			mode = -1;
			break;
		}
		}
	}

	if(mode == -1)
	{
		LOGW("%s: band not support: %d\n", __FUNCTION__, req);
		goto error;
	}
	
	asprintf(&cmd, "AT*BAND=%d,%d", mode, band);
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
}

static void ril_request_get_band(int request, void *data, size_t datalen, RIL_Token token)
{
	ATResponse *response = NULL;
	int result[1];
	int mode;
	int band;
	int rilBand;
	char *line;

	int err = at_send_command_singleline("AT*BAND?", "*BAND:", &response);
	if (err < 0 || response->success == 0 || response->p_intermediates == NULL)
	       goto error;

	line = response->p_intermediates->line;

	err = at_tok_start(&line);
	if (err < 0) goto error;

	err = at_tok_nextint(&line, &mode);
	if (err < 0) goto error;
	
	err = at_tok_nextint(&line, &band);
	if (err < 0) goto error;

	if(band < 0)
        rilBand= -1;
	else
	    rilBand = band;

	LOGI("%s: get band value : %d", __FUNCTION__, rilBand);
	result[0] = rilBand;

	RIL_onRequestComplete(token, RIL_E_SUCCESS, result, sizeof(result));
	goto exit;
error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);

}

/**
 * RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL_EXT
 *
 * Manually select a specified network.
 *
 * The radio baseband/RIL implementation will try to camp on the manually
 * selected network regardless of coverage, i.e. there is no fallback to
 * automatic network selection.
 */
static void ril_request_set_network_selection_manual_ext(int request, void *data, size_t datalen, RIL_Token token)
{
	RIL_OperInfo * info = (RIL_OperInfo *)data;
	ATResponse *response = NULL;
	int err;
	char *cmd, *line;
	
	resetLocalRegInfo();
	
	asprintf(&cmd, "AT+COPS=1,2,%s,%d", info->operNumStr, info->act);
	err = at_send_command(cmd, &response);
	free(cmd);
	if (err < 0 || response->success == 0)
	    goto error;
	RIL_onRequestComplete(token, RIL_E_SUCCESS, NULL, 0);
	
	RIL_onUnsolicitedResponse( RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED, NULL, 0);
	goto exit;
error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);
}
#endif

static void mmProcessRequest(void* arg)
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
		case RIL_REQUEST_SCREEN_STATE:
		{
			ril_request_screen_state(r->request, r->data, r->datalen, r->token);
			break;
		}
		case RIL_REQUEST_REGISTRATION_STATE:
		{
			ril_request_registration_state(r->request, r->data, r->datalen, r->token);
			break;
		}

		case RIL_REQUEST_GPRS_REGISTRATION_STATE:
		{
			ril_request_gprs_registration_state(r->request, r->data, r->datalen, r->token);
			break;
		}

		case RIL_REQUEST_OPERATOR:
		{
			ril_request_operator(r->request, r->data, r->datalen, r->token);
			break;
		}

		case RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE:
		{
			ril_request_query_network_selection_mode(r->request, r->data, r->datalen, r->token);
			break;
		}

		case RIL_REQUEST_SIGNAL_STRENGTH:
		{
			ril_request_signal_strength(r->request, r->data, r->datalen, r->token);
			break;
		}

		case RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC:
		{
			
			ril_request_set_network_selection_automatic(r->request, r->data, r->datalen, r->token);
			break;
		}
		case RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL:
		{
			ril_request_set_network_selection_manual(r->request, r->data, r->datalen, r->token);
			break;
		}

		case RIL_REQUEST_QUERY_AVAILABLE_NETWORKS:
		{
			ril_request_query_available_networks(r->request, r->data, r->datalen, r->token);
			break;
		}

		case RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE:
		{
			ril_request_set_preferred_network_type(r->request, r->data, r->datalen, r->token);
			break;
		}

		case RIL_REQUEST_GET_NEIGHBORING_CELL_IDS:
		{
			ril_request_get_neighboring_cell_ids(r->request, r->data, r->datalen, r->token);
			break;
		}

		case RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE:
		{
			ril_request_get_preferred_network_type(r->request, r->data, r->datalen, r->token);
			break;
		}


		case RIL_REQUEST_SET_LOCATION_UPDATES:
		{
			ril_request_set_location_updates(r->request, r->data, r->datalen, r->token);
			break;
		}

#ifdef MARVELL_EXTENDED
		case RIL_REQUEST_SELECT_BAND:
		{
			ril_request_select_band(r->request, r->data, r->datalen, r->token);
			break;
		}

		case RIL_REQUEST_GET_BAND:
		{
			ril_request_get_band(r->request, r->data, r->datalen, r->token);
			break;
		}

		case RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL_EXT:
		{
			ril_request_set_network_selection_manual_ext(r->request, r->data, r->datalen, r->token);
			break;
		}
#endif
		default:
		{
			LOGW("%s:invalid request:%d\n", __FUNCTION__, r->request);
			break;
		}
	}
	freeRILRequest(r);
}

void onRequest_mm (int request, void *data, size_t datalen, RIL_Token token)
{
	LOGV("%s entry: request = %d", __FUNCTION__, request);
	enque(getWorkQueue(SERVICE_MM), mmProcessRequest, (void*)newRILRequest(request, data, datalen, token));
}

void onUnsolicited_mm (const char *s)
{
	char *line = NULL, *response, *plmn = NULL, *linesave = NULL;
	int err;

	/* Process ind msg of signal length */
	if (strStartsWith(s, "+CSQ:"))
	{
		int response[2];
		line = strdup(s);
		linesave = line;
		err = at_tok_start(&line);
		if (err < 0) goto error;

		err = at_tok_nextint(&line, &sCSQ[0]);
		if (err < 0) goto error;

		err = at_tok_nextint(&line, &sCSQ[1]);
		if (err < 0) goto error;

		/* CP is asserted or resetting, we need to reset our global variables */
		if (sCSQ[0] == 99 && sCSQ[1] == 99)
		{
			resetLocalRegInfo();
			AfterCPReset = 1;
			/* Init radio state to RADIO OFF  */
			setRadioState(RADIO_STATE_OFF);

			disableAllInterfaces();

			response[0] = 67;
			response[1] = 89;
			
			RIL_onUnsolicitedResponse(RIL_UNSOL_SIGNAL_STRENGTH, response, sizeof(response));

		}else
		{
			reportSignalStrength(NULL);
		}
	}

	/* Process ind msg of network registration status */
	else if (strStartsWith(s, "+CREG:") || strStartsWith(s, "+CGREG:"))
	{
		int responseInt[3], num;
		responseInt[1] = -1;
		responseInt[2] = -1;

		line = strdup(s);
		linesave = line;
		err = at_tok_start(&line);
		if (err < 0) goto error;

		err = at_tok_nextint(&line, &responseInt[0]);
		if (err < 0) goto error;

		if (at_tok_hasmore(&line))
		{
			err = at_tok_nexthexint(&line, &responseInt[1]);
			if (err < 0) goto error;

			if (at_tok_hasmore(&line))
			{
				err = at_tok_nexthexint(&line, &responseInt[2]);
				if (err < 0) goto error;

				if (at_tok_hasmore(&line))
				{
					err = at_tok_nextint(&line, &(sOperInfo.act));
					if (err < 0) goto error;
				}
			}
		}

		/* Save current reg state for query from upper layer */
		if (strStartsWith(s, "+CREG:"))
		{
			sCregState.stat = responseInt[0];
			sCregState.lac  = responseInt[1];
			sCregState.cid  = responseInt[2];
		}
		else if (strStartsWith(s, "+CGREG:"))
		{
			sCgregState.stat = responseInt[0];
			sCgregState.lac  = responseInt[1];
			sCgregState.cid  = responseInt[2];
		}

		/* Report to upper layer only when screen state is ON */
		if (sScreenState)
		{
			RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED, NULL, 0);
		}
	}

	/* Process ind msg of network time */
	else if (strStartsWith(s, "+NITZ:"))
	{
		char* dst, *tz, *uniTime;
		char nitztime[25];
		linesave = strdup(s);
		response = linesave;
		err = at_tok_start(&response);
		if (err < 0) goto error;

		err = at_tok_nextstr(&response, &dst);
		if (err < 0) goto error;
		if (at_tok_hasmore(&response))
		{
			err = at_tok_nextstr(&response, &tz);
			if (err < 0) goto error;

			if (at_tok_hasmore(&response))
			{
				// show the result at last. form required by Android: "yy/mm/dd,hh:mm:ss(+/-)tz,dt" in UTC
				sprintf(nitztime, "%s%s,%s", response, tz, dst);

				//save last Nitz recevied
				gettimeofday(&lastNitzTimeReceived, NULL);
				sprintf(lastNitzTime, "%s", response);

				RIL_onUnsolicitedResponse( RIL_UNSOL_NITZ_TIME_RECEIVED, nitztime, strlen(nitztime));
			}
			else
			{
				struct timeval currenttime;

				//sometimes GMM only contains tz and dst, if it comes not too later than last NITZ time,
				//we use last Nitz time to notify uper layer, it should be removed after android support
				//update TZ only
				gettimeofday(&currenttime, NULL);
				if(currenttime.tv_sec - lastNitzTimeReceived.tv_sec < 2)
				{
					// show the result at last. form required by Android: "yy/mm/dd,hh:mm:ss(+/-)tz,dt" in UTC
					sprintf(nitztime, "%s%s,%s", lastNitzTime, tz, dst);

					RIL_onUnsolicitedResponse( RIL_UNSOL_NITZ_TIME_RECEIVED, nitztime, strlen(nitztime));
				}
			}
		}

	}
	else if (strStartsWith(s, "*COPN:"))
	{
		int format;
		linesave = strdup(s);
		response = linesave;
		err = at_tok_start(&response);
		if (err < 0) goto error;

		err = at_tok_nextint(&response, &format);
		if (err < 0) goto error;
		//long format alphanumeric <oper>
		if(format == MM_NETOP_ID_FORMAT_ALPHA_LONG)
		{
			char *longname;
			err = at_tok_nextstr(&response, &longname);
			if (err < 0) goto error;
			strcpy(sOperInfo.operLongStr, longname);
		}
		//short format alphanumeric <oper>
		else if (format == MM_NETOP_ID_FORMAT_ALPHA_SHORT)
		{
			char *shortName;
			err = at_tok_nextstr(&response, &shortName);
			if (err < 0) goto error;
			strcpy(sOperInfo.operShortStr, shortName);
		}
		else
		{
			LOGI("onUnsolicited_mm: not supportted oper format: %d", format);
			goto error;
		}
	}
	else if(strStartsWith(s, "+EEMUMTSINTER:") || strStartsWith(s, "+EEMUMTSINTRA:"))
	{
		//UMTS neighboring cell info
		int loop, i;
		int psc; //Primary Scrambling Code (as described in TS 25.331) in 9 bits in UMTS FDD ,
		int cellparaId; // cellParameterId in UMTS TDD
		int rscp;//Level index of CPICH Received Signal Code Power in UMTS FDD, PCCPCH Received Signal Code Power in UMTS TDD
		
		//TDD case	
		//+EEMUMTSINTER: 0, -826, 0, -792, 1120, 0, 65534, 0, 10071, 71
		//+EEMUMTSINTER: index, pccpchRSCP, utraRssi, sRxLev,mcc, mnc, lac, ci, arfcn, cellParameterId

		//FDD case 
		//+EEMUMTSINTER: 0, -32768, 0, -32768, -144, -760, 65535, 65535, 65534, 0, 10663, 440
		//+EEMUMTSINTER: index, cpichRSCP, utraRssi, cpichEcN0, sQual, sRxLev,mcc, mnc, lac, ci, arfcn, psc

		if(sCellNumber < MAX_NEIGHBORING_CELLS)
		{
			line = strdup(s);
			linesave = line;
			err = at_tok_start(&line);
			if (err < 0) goto error;

			err = at_tok_nextint(&line,&loop);
			if (err < 0) goto error;

			err = at_tok_nextint(&line,&rscp);
			if (err < 0) goto error;

			for(i = 0; i < 7; i++ )
			{
				err = at_tok_nextint(&line,&loop);
				if (err < 0) goto error;
			}

			err = at_tok_nextint(&line,&cellparaId);
			if (err < 0) goto error;

			
			//FDD cases
			if(at_tok_hasmore(&line))
			{
				err = at_tok_nextint(&line,&loop);
				if (err < 0) goto error;

				err = at_tok_nextint(&line,&psc);
				if (err < 0) goto error;

				asprintf(&sNeighboringCell[sCellNumber].cid, "%x", psc);
				sNeighboringCell[sCellNumber].rssi = rscp;
				sCellNumber++;

				
				LOGI("onUnsolicited_mm new cell info cid:%s, rssi: %d", sNeighboringCell[sCellNumber-1].cid, rscp);

			}
			else
			{
				asprintf(&sNeighboringCell[sCellNumber].cid, "%x", cellparaId);
				sNeighboringCell[sCellNumber].rssi = rscp;
				sCellNumber++;

				LOGI("onUnsolicited_mm new cell info cid:%s, rssi: %d", sNeighboringCell[sCellNumber-1].cid, rscp);
			}
			
		}
		else
		{
			LOGV("onUnsolicited_mm ignor cell info ");
		}

	}
	else if(strStartsWith(s, "+EEMGINFONC:"))
	{
		//GSM neighboring cell info
		// +EEMGINFONC: 2, 0, 0, 6334, 0, 0,41, 55, 29, 29, 516, 0, 29
		//+EEMGINFONC: nc_num, mcc, mnc, lac, rac, ci,rx_lv, bsic, C1, C2, arfcn, C31, C32
		int loop, i;
		int lac, ci; //Upper 16 bits is LAC and lower 16 bits is CID (as described in TS 27.005), use lac and ci
		int rssi;//Received RSSI, use rx_lv

		if(sCellNumber < MAX_NEIGHBORING_CELLS)
		{
			line = strdup(s);
			linesave = line;
			err = at_tok_start(&line);
			if (err < 0) goto error;

			for(i = 0; i<3; i++)
			{
				err = at_tok_nextint(&line,&loop);
				if (err < 0) goto error;
			}

			err = at_tok_nextint(&line,&lac);
			if (err < 0) goto error;

			err = at_tok_nextint(&line,&loop);
			if (err < 0) goto error;

			err = at_tok_nextint(&line,&ci);
			if (err < 0) goto error;

			err = at_tok_nextint(&line,&rssi);
			if (err < 0) goto error;

			asprintf(&sNeighboringCell[sCellNumber].cid, "%04x%04x", lac, ci);
			sNeighboringCell[sCellNumber].rssi = rssi;
			sCellNumber++;

		    LOGI("onUnsolicited_mm new cell info cid:%s, rssi: %d", sNeighboringCell[sCellNumber-1].cid, rssi);
		}
		else
		{
			LOGI("onUnsolicited_mm ignor cell info ");
		}

		
	}
	else if(strStartsWith(s, "+EEMUMTSINTERRAT:"))
	{
		LOGI("onUnsolicited_mm: not support +EEMUMTSINTERRAT");
	}
	else if(strStartsWith(s, "++EEMGINFOBASIC:"))
	{
		LOGI("onUnsolicited_mm: not support ++EEMGINFOBASIC");
	}
	else if(strStartsWith(s, "+EEMGINFOSVC:"))
	{
		LOGI("onUnsolicited_mm: not support +EEMGINFOSVC");
	}
	else if(strStartsWith(s, "+EEMGINFOPS:"))
	{
		LOGI("onUnsolicited_mm: not support +EEMGINFOPS");
	}
	else if(strStartsWith(s, "+EEMGINBFTM:"))
	{
		LOGI("onUnsolicited_mm: not support +EEMGINBFTM");
	}
	/* Porcess ind msg of ServiceRestrictionsInd */
	else if (strStartsWith(s, "+MSRI:"))
	{
		// move to CPIN indication, nothing need to do here
		//if(AfterCPReset)
		//	enque_initializeCallback();
	}

	/* Free allocated memory and return */
	if (linesave != NULL) free(linesave);
	return;

 error:
	if (linesave != NULL) free(linesave);
	LOGE("%s: Error parameter in ind msg: %s", __FUNCTION__, s);
	return;
}


