/******************************************************************************
 * *(C) Copyright 2008 Marvell International Ltd.
 * * All Rights Reserved
 * ******************************************************************************/

#include <assert.h>

#include "at_tok.h"
#include "atchannel.h"
#include "marvell-ril.h"
#include "misc.h"
#include "ril-requestdatahandler.h"
#include "work-queue.h"

#define MAX_PDU_LENGTH    400   //128
#define MAX_GSM_BROAD_CAST_SMS_CONFIG_INFO (40 * 40)
#define MAX_GSM_BROAD_CAST_SMS_DATA_CODE_CONFIG_INFO 40
#define INVALID_BROAD_CAST_MESSAGE_IDENTIFY -1UL
#define INVALID_BROAD_CAST_DATA_CODE_SCHEME -1UL

static void ril_handle_cmd_SendSMS(const char* cmd, const char *smsPdu, const char* prefix, RIL_Token token)
{
	ATResponse *response = NULL;
	int err;
	RIL_SMS_Response result;
	char *line;

	err = at_send_command_sms(cmd, smsPdu, prefix, &response);
	if (err < 0 || response->success == 0 || !response->p_intermediates)
		goto error;
	
	line = response->p_intermediates->line;

	memset(&result, 0, sizeof(result));
	err = at_tok_start(&line);
	if (err < 0) goto error;
	err = at_tok_nextint(&line, &(result.messageRef));
	if (err < 0) goto error;

	if (at_tok_hasmore(&line))
	{
		err = at_tok_nextstr(&line, &(result.ackPDU));
		if (err < 0) goto error;
	}
	RIL_onRequestComplete(token, RIL_E_SUCCESS, &result, sizeof(result));
	goto exit;

error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);
}

static void ril_request_get_smsc_address(RIL_Token token)
{
	ATResponse *response = NULL;
	int result[1];
	char *line;
	char *addr;
	char *sca;
	int tosca;
	int err;

	err = at_send_command_singleline("AT+CSCA?", "+CSCA:",  &response);
	if (err < 0 || response->success == 0 || !response->p_intermediates)
		goto error;
	
	line = response->p_intermediates->line;

	err = at_tok_start(&line);
	if (err < 0) goto error;

	err = at_tok_nextstr(&line, &addr);
	if (err < 0) goto error;

	err = at_tok_nextint(&line, &tosca);
	if (err < 0) goto error;

	if (tosca == 145 && addr[0] != '+')
	{
		sca = alloca(sizeof(char) * (strlen(addr) + 1 + 1));
		sca[0] = '+';
		sca[1] = 0;
		strcat(sca, addr);
	}
	else
	{
		sca = addr;
	}

	LOGI("%s: sca: %s, tosca: %d\n", __FUNCTION__, sca, tosca);

	RIL_onRequestComplete(token, RIL_E_SUCCESS, sca, strlen(sca));

	goto exit;

 error:
	 RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
 exit:
	 at_response_free(response);

}

/* Process AT reply of RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG */
static void ril_request_gsm_get_broadcast_sms_config(RIL_Token token)
{
	ATResponse *response = NULL;
	RIL_GSM_BroadcastSmsConfigInfo *cbConfig[MAX_GSM_BROAD_CAST_SMS_CONFIG_INFO] = {NULL};
	RIL_GSM_BroadcastSmsConfigInfo *cbConfig_dcss[MAX_GSM_BROAD_CAST_SMS_DATA_CODE_CONFIG_INFO] = {NULL};
	char *line;
	char *rangestr;
	char *rangefrom, *rangeto;
	char *mids, *dcss;
	int err;
	char* end;
	int selected;
	int count = -1, i;
	int dcss_count = -1;
	int fromServiceId, toServiceId;
	
	err = at_send_command_singleline("AT+CSCB?", "+CSCB:", &response);
	if (err < 0 || response->success == 0 || !response->p_intermediates)
		goto error;
	
	line = response->p_intermediates->line;

	err = at_tok_start(&line);
	if (err < 0) goto error;

	err = at_tok_nextint(&line, &selected);
	if (err < 0) goto error;

	err = at_tok_nextstr(&line, &mids);
	if (err < 0) goto error;
	err = at_tok_nextstr(&line, &dcss);
	if (err < 0) goto error;
	//process the case <dcss>=""
	skipWhiteSpace(&dcss);
	if(*dcss == '\0')
		dcss = NULL;
	while(at_tok_nextstr(&dcss, &rangestr) == 0)
	{
		if(dcss_count < MAX_GSM_BROAD_CAST_SMS_DATA_CODE_CONFIG_INFO - 1)
		{
			cbConfig_dcss[++dcss_count] = malloc(sizeof(RIL_GSM_BroadcastSmsConfigInfo));
			assert(cbConfig_dcss[dcss_count] != NULL);
			cbConfig_dcss[dcss_count]->selected = !selected;
			rangefrom = strsep(&rangestr, "-");
			cbConfig_dcss[dcss_count]->fromCodeScheme = strtol(rangefrom, &end, 10);
			if(end == rangefrom) goto error;
			if(rangestr != NULL)
			{
				cbConfig_dcss[dcss_count]->toCodeScheme = strtol(rangestr, &end, 10);
				if(end == rangestr ) goto error;
			}
			else
			{
				cbConfig_dcss[dcss_count]->toCodeScheme = cbConfig_dcss[dcss_count]->fromCodeScheme;
			}
			cbConfig_dcss[dcss_count]->fromServiceId = cbConfig_dcss[dcss_count]->toServiceId = INVALID_BROAD_CAST_MESSAGE_IDENTIFY;
		}
		else
		{
			LOGE("%s: Max limit %d passed, can not send all ranges", __FUNCTION__, MAX_GSM_BROAD_CAST_SMS_DATA_CODE_CONFIG_INFO);
			RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
			goto exit;
		}
	}
	//process the case <mids>=""
	skipWhiteSpace(&mids);
	if(*mids == '\0')
	{
		for(i = 0; i <= dcss_count; i++)
			cbConfig[i] = cbConfig_dcss[i];
		count = dcss_count;
		dcss_count = -1;
		mids = NULL;
	}
	while(at_tok_nextstr(&mids, &rangestr) == 0)
	{
		if(count + dcss_count + 1 < MAX_GSM_BROAD_CAST_SMS_CONFIG_INFO - 1)
		{
			rangefrom = strsep(&rangestr, "-");
			fromServiceId = strtol(rangefrom, &end, 10);
			if(end == rangefrom) goto error;
			if(rangestr != NULL)
			{
				toServiceId = strtol(rangestr, &end, 10);
				if(end == rangestr ) goto error;
			}
			else
			{
				toServiceId = fromServiceId;
			}
			if(dcss_count <= -1)
			{
				cbConfig[++count] = malloc(sizeof(RIL_GSM_BroadcastSmsConfigInfo));
				assert(cbConfig[count] != NULL);
				cbConfig[count]->selected = !selected;
				cbConfig[count]->fromServiceId = fromServiceId;
				cbConfig[count]->toServiceId = toServiceId;
				cbConfig[count]->fromCodeScheme = cbConfig[count]->toCodeScheme = INVALID_BROAD_CAST_DATA_CODE_SCHEME;
			}
			else
			{
				for(i = count + 1; i <= count + dcss_count + 1; i++)
				{
					cbConfig[i] = malloc(sizeof(RIL_GSM_BroadcastSmsConfigInfo));
					assert(cbConfig[i] != NULL);
					memcpy(cbConfig[i], cbConfig_dcss[i - count - 1], sizeof(*cbConfig[i]));
					cbConfig[i]->fromServiceId = fromServiceId;
					cbConfig[i]->toServiceId = toServiceId;
				}
				count += dcss_count + 1;
			}
		}
		else
		{
			LOGE("%s: Max limit %d passed, can not send all ranges", __FUNCTION__, MAX_GSM_BROAD_CAST_SMS_CONFIG_INFO);
			RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
			goto exit;
		}
	}
	for(i = 0; i <= count; i++)
		LOGI("%s: fromServiceId: %d, toServiceId: %d, fromCodeScheme: %d, toCodeScheme: %d, selected: %d\n",
			__FUNCTION__, cbConfig[i]->fromServiceId, cbConfig[i]->toServiceId,
			cbConfig[i]->fromCodeScheme, cbConfig[i]->toCodeScheme, cbConfig[i]->selected );

	RIL_onRequestComplete(token, RIL_E_SUCCESS, &cbConfig, (count + 1) * sizeof(RIL_GSM_BroadcastSmsConfigInfo *));
	goto exit;

error:
	LOGE("%s: Error parameter in response: %s", __FUNCTION__, response->p_intermediates->line);
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	for(i = 0; i <= dcss_count; i++)
		free(cbConfig_dcss[i]);
	for(i = 0; i <= count; i++)
		free(cbConfig[i]);
	at_response_free(response);

}


static void ril_request_gsm_set_broadcast_sms_config(int request, void *data, size_t datalen, RIL_Token token)
{
	char cmdString[MAX_AT_LENGTH];
	RIL_GSM_BroadcastSmsConfigInfo ** CbConfig = (RIL_GSM_BroadcastSmsConfigInfo **)data;
	int count = datalen/sizeof(RIL_GSM_BroadcastSmsConfigInfo *);
	int i;
	RIL_GSM_BroadcastSmsConfigInfo *configInfo = NULL;
	char *tmp;
	char tmpBuffer[15];
	char *mids[2] = {NULL, NULL};
	char *dcss[2] = {NULL, NULL};
	int valid = 0;
	for(i = 0; i < count; i++)
	{
		configInfo = CbConfig[i];
		if(configInfo->selected != 0 && configInfo->selected != 1)
			goto error;
		if(!((configInfo->fromServiceId >= 0x0) && (configInfo->fromServiceId <= 0xFFFF)
			&& (configInfo->toServiceId <= 0xFFFF) && (configInfo->fromServiceId <= configInfo->toServiceId)))
			goto next;
		if(configInfo->fromServiceId == configInfo->toServiceId)
			sprintf(tmpBuffer, "%d,", configInfo->fromServiceId);
		else
			sprintf(tmpBuffer, "%d-%d,", configInfo->fromServiceId, configInfo->toServiceId);
		if(mids[configInfo->selected] == NULL || strstr(mids[configInfo->selected], tmpBuffer) == NULL)
		{
			tmpBuffer[strlen(tmpBuffer) - 1] = '\0';
			tmp = mids[configInfo->selected];
			asprintf(&mids[configInfo->selected], "%s%s%s", (tmp ? tmp : ""), tmpBuffer,
				(i == (count - 1) ? "" : ","));
			free(tmp);
		}
	next:
		if(!((configInfo->fromCodeScheme >= 0x0) && (configInfo->fromCodeScheme <= 0xFF)
			&& (configInfo->toCodeScheme <= 0xFF) && (configInfo->fromCodeScheme <= configInfo->toCodeScheme)))
			continue;
		if(configInfo->fromCodeScheme == configInfo->toCodeScheme)
			sprintf(tmpBuffer, "%d,", configInfo->fromCodeScheme);
		else
			sprintf(tmpBuffer, "%d-%d,", configInfo->fromCodeScheme, configInfo->toCodeScheme);
		if(dcss[configInfo->selected] == NULL || strstr(dcss[configInfo->selected], tmpBuffer) == NULL)
		{
			tmpBuffer[strlen(tmpBuffer) - 1] = '\0';
			tmp = dcss[configInfo->selected];
			asprintf(&dcss[configInfo->selected], "%s%s%s", (tmp ? tmp : ""), tmpBuffer,
				(i == (count - 1) ? "" : ","));
			free(tmp);
		}
	}
	if(mids[0] != NULL || dcss[0] != NULL)
	{
		if(mids[0] != NULL && mids[0][strlen(mids[0]) - 1] == ',')
			mids[0][strlen(mids[0]) - 1] = '\0';
		if(dcss[0] != NULL && dcss[0][strlen(dcss[0]) - 1] == ',')
			dcss[0][strlen(dcss[0]) - 1] = '\0';
		/*selected 0 means message types specified in <fromServiceId, toServiceId>
		and <fromCodeScheme, toCodeScheme>are not accepted*/
		sprintf(cmdString, "AT+CSCB=%d,\"%s\",\"%s\"", 1, (mids[0] == NULL ? "" : mids[0]), (dcss[0] == NULL ? "" : dcss[0]));
		valid = 1;
	}
	if(mids[1] != NULL || dcss[1] != NULL)
	{
		if(mids[1] != NULL && mids[1][strlen(mids[1]) - 1] == ',')
			mids[1][strlen(mids[1]) - 1] = '\0';
		if(dcss[1] != NULL && dcss[1][strlen(dcss[1]) - 1] == ',')
			dcss[1][strlen(dcss[1]) - 1] = '\0';
		/*selected 1 means message types specified in <fromServiceId, toServiceId>
		and <fromCodeScheme, toCodeScheme>are accepted.*/
		if(valid)
			sprintf(cmdString, "%s;+CSCB=%d,\"%s\",\"%s\"", cmdString, 0, (mids[1] == NULL ? "" : mids[1]), (dcss[1] == NULL ? "" : dcss[1]));
		else
			sprintf(cmdString, "AT+CSCB=%d,\"%s\",\"%s\"", 0, (mids[1] == NULL ? "" : mids[1]), (dcss[1] == NULL ? "" : dcss[1]));
		valid = 1;
	}
	if(!valid)
		goto error;
	ril_handle_cmd_default_response(cmdString, token);
	goto exit;
error:
	LOGE("%s: parameter error in RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG", __FUNCTION__);
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	if(mids[0] != NULL) free(mids[0]);
	if(mids[1] != NULL) free(mids[1]);
	if(dcss[0] != NULL) free(dcss[0]);
	if(dcss[1] != NULL) free(dcss[1]);
}

static void ril_request_gsm_sms_broadcast_activation(int request, void *data, size_t datalen, RIL_Token token)
{
	ATResponse *response = NULL;
	int mode, mt, bm, ds, bfr, skip;
	int activation = *((int *)data);
	char cmdString[MAX_AT_LENGTH];
	char *line;
	int err;

	err = at_send_command_singleline("AT+CNMI?", "+CNMI:",  &response);
	if (err < 0 || response->success == 0 || !response->p_intermediates)
		goto error;

	line = response->p_intermediates->line;
	err = at_tok_start(&line);
	if (err < 0)
		goto error;
	err = at_tok_nextint(&line, &mode);
	if (err < 0)
		goto error;
	err = at_tok_nextint(&line, &mt);
	if (err < 0)
		goto error;
	err = at_tok_nextint(&line, &skip);
	if (err < 0)
		goto error;
	err = at_tok_nextint(&line, &ds);
	if (err < 0)
		goto error;
	err = at_tok_nextint(&line, &bfr);
	if (err < 0)
		goto error;
	if (activation == 0)
		bm = 2;
	else
		bm = 0;
	sprintf(cmdString, "AT+CNMI=%d,%d,%d,%d,%d", mode, mt, bm, ds, bfr);
	ril_handle_cmd_default_response(cmdString, token);
	goto exit;
error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);

}

static void msgProcessRequest(void* arg)
{
	RILRequest* r = (RILRequest*)arg;
	int request = r->request;
	void* data = r->data;
	size_t datalen = r->datalen;
	RIL_Token token = r->token;
	LOGD("%s: %s, token:%p", __FUNCTION__, requestToString(r->request), r->token);
	char cmdString[MAX_AT_LENGTH];
	char smsPdu[MAX_PDU_LENGTH];
	if (!needContinueProcess(r->request, r->data, r->datalen, r->token)) {
		freeRILRequest(r);
		LOGI("%s: do not need to continue to process, token:%p", __FUNCTION__, r->token);
		return;
	}
	switch (request)
	{
	case RIL_REQUEST_SEND_SMS:
	case RIL_REQUEST_SEND_SMS_EXPECT_MORE:
	{
		const char *smsc, *tpdu;
		int tpLayerLength;
		char temp[20];

		/* Send +CMMS=<n> to set local variable in AT cmd server */
		//set 1, prevent the premature release of RRC by CP.
		strcpy(cmdString, "AT+CMMS=1;");

		smsc = ((const char **)data)[0];
		tpdu = ((const char **)data)[1];
		tpLayerLength = strlen(tpdu) / 2;
		/* NULL for default SMSC */
		if (smsc == NULL) smsc = "00";

		sprintf(temp, "+CMGS=%d", tpLayerLength);
		strcat(cmdString, temp);
		sprintf(smsPdu, "%s%s", smsc, tpdu);
		ril_handle_cmd_SendSMS(cmdString, smsPdu, "+CMGS:", token);
		break;
	}

	case RIL_REQUEST_SMS_ACKNOWLEDGE:
	{
		int ackSuccess;
		int memFull;
		ackSuccess = ((int *)data)[0];
		memFull = ((int*)data)[1];

		if (ackSuccess == 1)
		{
			strcpy(cmdString, "AT*CNMA=0");
		}
		else
		{
			if(memFull == 0xd3)
				strcpy(cmdString, "AT*CNMA=1");
			else
				strcpy(cmdString, "AT*CNMA=2");
		}
		ril_handle_cmd_default_response(cmdString, r->token);
		break;
	}

	case RIL_REQUEST_WRITE_SMS_TO_SIM:
	{
		RIL_SMS_WriteArgs *p_args;
		const char *smsc;
		int length, err;

		p_args = (RIL_SMS_WriteArgs *)data;
		length = strlen(p_args->pdu) / 2;
		sprintf(cmdString, "AT+CMGW=%d,%d", length, p_args->status);

		smsc = p_args->smsc;
		/* NULL for default SMSC */
		if (smsc == NULL) smsc = "00";
		sprintf(smsPdu, "%s%s", smsc, p_args->pdu);
		ril_handle_cmd_sms_one_int(cmdString, smsPdu, "+CMGW:", r->token);
		break;
	}

	case RIL_REQUEST_DELETE_SMS_ON_SIM:
	{
		sprintf(cmdString, "AT+CMGD=%d", ((int *)data)[0]);
		ril_handle_cmd_default_response(cmdString, r->token);
		break;
	}

	case RIL_REQUEST_REPORT_SMS_MEMORY_STATUS:
	{
		int memAvail = ((int *)data)[0];
		sprintf(cmdString, "AT+CMEMFULL=%d", !memAvail);
		ril_handle_cmd_default_response(cmdString, r->token);
		break;
	}

	case RIL_REQUEST_SET_SMSC_ADDRESS:
	{
		char *sca = (char *)data;
		int tosca;

		if(sca[0] == '+')
			tosca = 145;
		else
			tosca = 129;

		sprintf(cmdString, "AT+CSCA=\"%s\",%d", sca, tosca);
		ril_handle_cmd_default_response(cmdString, r->token);
		break;
	}

	case RIL_REQUEST_GET_SMSC_ADDRESS:
	{
		ril_request_get_smsc_address(token);
		break;
	}

	case RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG:
	{
		ril_request_gsm_get_broadcast_sms_config(token);
		break;
	}

	case RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG:
	{
		ril_request_gsm_set_broadcast_sms_config(request, data, datalen, token);
		break;
	}
	case RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION:
	{
		ril_request_gsm_sms_broadcast_activation(request, data, datalen, token);
		break;
	}

	default:
	{
		LOGW("%s:invalid request:%d\n", __FUNCTION__, request);
	}
	}

	freeRILRequest(r);
}

void onRequest_msg (int request, void *data, size_t datalen, RIL_Token token)
{
	LOGV("%s entry: request = %d", __FUNCTION__, request);
	enque(getWorkQueue(SERVICE_MSG), msgProcessRequest, (void*)newRILRequest(request, data, datalen, token));
}


void onUnsolicited_msg (const char *s, const char *smsPdu)
{
	char *line = NULL, *response;
	int err;
	char* linesave = NULL;

	/* New SMS is reported directly to ME */
	if (strStartsWith(s, "+CMT:"))
	{
		RIL_onUnsolicitedResponse( RIL_UNSOL_RESPONSE_NEW_SMS, smsPdu, strlen(smsPdu));
	}

	/* New SMS is saved in SIM and the index is reported to ME */
	else if (strStartsWith(s, "+CMTI:"))
	{
		int index;
		line = strdup(s);
		linesave = line;
		err = at_tok_start(&line);
		if (err < 0) goto error;

		err = at_tok_nextstr(&line, &response);
		if (err < 0) goto error;

		err = at_tok_nextint(&line, &index);
		if (err < 0) goto error;

		RIL_onUnsolicitedResponse( RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM, &index, sizeof(index));
	}

	/* New SMS status report reported directly to ME */
	else if (strStartsWith(s, "+CDS:"))
	{
		RIL_onUnsolicitedResponse( RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT, smsPdu, strlen(smsPdu));
	}

	/* New CBM is reported directly to ME */
	else if (strStartsWith(s, "+CBM:"))
	{
	        RIL_onUnsolicitedResponse( RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS, smsPdu, strlen(smsPdu));
	}

	/* Marvell extended AT cmd to indicate memory of SIM is full  */
	else if (strStartsWith(s, "+MMSG:"))
	{
		int bSmsReady = 0, bSmsFull = 0;
		line = strdup(s);
		linesave = line;
		err = at_tok_start(&line);
		if (err < 0) goto error;

		err = at_tok_nextint(&line, &bSmsReady);
		if (err < 0) goto error;

		err = at_tok_nextint(&line, &bSmsFull);
		if (err < 0) goto error;

		if (bSmsFull == 1)
		{
			RIL_onUnsolicitedResponse(RIL_UNSOL_SIM_SMS_STORAGE_FULL, NULL, 0);
		}
	}

	/* Free allocated memory and return */
	if (linesave != NULL) free(linesave);
	return;

 error:
	if (linesave != NULL) free(linesave);
	LOGW("%s: Error parameter in ind msg: %s", __FUNCTION__, s);
	return;

}


