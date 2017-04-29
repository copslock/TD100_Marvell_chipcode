/******************************************************************************
 * *(C) Copyright 2008 Marvell International Ltd.
 * * All Rights Reserved
 * ******************************************************************************/

#include <arpa/inet.h>
#include <cutils/properties.h>
#include "at_tok.h"
#include "atchannel.h"
#include "marvell-ril.h"
#include "misc.h"
#include "ril-requestdatahandler.h"
#include "work-queue.h"

extern int is_ppp_enabled();
extern void enableInterface(int cid);
extern void disableInterface(int cid);
extern int configureInterface(char* address, char* ifname);
extern void deconfigureInterface(char* ifname);
extern void setDNS(char* dns, char* ifname);
extern void closeCiNetDev();

extern int enablePPPInterface(int cid, const char *user, const char *passwd, char* ipaddress);
extern void disablePPPInterface(int cid);
extern int getInterfaceAddr(int af, const char* ifname, char* ipaddress);

extern struct psd_channel_decription ppp_channel;

#ifdef DKB_WUKONG
extern struct psd_channel_decription wukong_pdp_channel[];
#endif

typedef struct {
	int active;      // 0 disable, 1 enable
	char  cid[2];        /* predfined Context ID */
	char apn[64];
	char ifname[16]; /*save predifined interface name */
} Data_Call_Info;

static Data_Call_Info g_datacalls[MAX_DATA_CALLS] = {{0, "1", "", "ccinet0"},{0, "2", "", "ccinet1"},{0, "3", "", "ccinet2"},
			{0, "4", "", "ccinet3"},{0, "5", "", "ccinet4"},{0, "6", "", "ccinet5"},{0, "7", "", "ccinet6"},{0, "8", "", "ccinet7"}};

static char* getDNSList(const char* ifname)
{
	static char dnslist[PROPERTY_VALUE_MAX * 2];
	char dns1[PROPERTY_VALUE_MAX];
	char dns2[PROPERTY_VALUE_MAX];
	char* pname;

	asprintf(&pname, "net.%s.dns1", ifname);
	property_get(pname, dns1, "");
	free(pname);

	asprintf(&pname, "net.%s.dns2", ifname);
	property_get(pname, dns2, "");
	free(pname);

	sprintf(dnslist, "%s %s", dns1, dns2);
	return dnslist;
}

static char* getGateway(const char* ifname)
{
	static char gw[PROPERTY_VALUE_MAX];
	char* pname;

	asprintf(&pname, "net.%s.gw", ifname);
	property_get(pname, gw, "");
	free(pname);

	return gw;
}

/* Internal tool to parse PDP context list.
 * Sample: +CGDCONT: 1,"IP","cmnet","10.60.176.183",0,0,802110030100108106d38870328306d38814cb,
 * Input para: line (as above exsample)
 * Output para: pCid, type, apn, address, dns
 * Note: for the output para with type char *, the space is allocated in this func
 */
static int parsePDPContexstList(char *line, int *pCid, char **pType, char **pApn, char **pAddress, char **pDns)
{
	int err, ignore;
	char *out, *type, *apn, *address, *dns;

	type = apn = address = dns = NULL;

	err = at_tok_start(&line);
	if (err < 0) goto error;

	err = at_tok_nextint(&line, pCid);
	if (err < 0) goto error;

	err = at_tok_nextstr(&line, &out);
	if (err < 0) goto error;
	if (pType != NULL)
	{
		type = malloc(strlen(out) + 1);
		strcpy(type, out);
		*pType = type;
	}

	err = at_tok_nextstr(&line, &out);
	if (err < 0) goto error;
	if (pApn != NULL)
	{
		apn = malloc(strlen(out) + 1);
		strcpy(apn, out);
		*pApn = apn;
	}

	err = at_tok_nextstr(&line, &out);
	if (err < 0) goto error;
	if (pAddress != NULL)
	{
		address = malloc(strlen(out) + 1);
		strcpy(address, out);
		*pAddress = address;
	}

	err = at_tok_nextint(&line, &ignore);
	if (err < 0) goto error;

	err = at_tok_nextint(&line, &ignore);
	if (err < 0) goto error;

	err = at_tok_nextstr(&line, &out);
	if (err < 0) goto error;
	if (pDns != NULL)
	{
		dns = malloc(strlen(out) + 1);
		strcpy(dns, out);
		*pDns = dns;
	}

	return 0;

 error:
	if (type) free(type);
	if (apn) free(apn);
	if (address) free(address);
	if (pType) *pType = NULL;
	if (pApn) *pApn = NULL;
	if (pAddress) *pAddress = NULL;
	if (pDns) *pDns = NULL;
	return err;
}

static void freePDPContext(RIL_Data_Call_Response *pdpResponses, int num)
{
	int i;
	char *type, *apn, *address;

	for (i = 0; i < num; i++)
	{
		type = pdpResponses[i].type;
		apn  = pdpResponses[i].apn;
		address = pdpResponses[i].address;

		FREE(type);
		FREE(apn);
		FREE(address);
	}
	FREE(pdpResponses);
}

/* Prossess RIL_REQUEST_SETUP_DATA_CALL in sync way*/
static void syncSetupDefaultPDPConnection(RIL_Token token, int profile, const char* apn, int auth_type, const char* user, const char* passwd, const char* protocol)
{
	ATResponse *p_response = NULL;
	int err = -1;
	int cid;
	char *result[5];
	char cmdString[MAX_AT_LENGTH];
	char ipaddress[64];

	int is_ppp = is_ppp_enabled();
	int af = strcasecmp(protocol, "IPv6") ? AF_INET: AF_INET6;
	if (is_ppp)// For PPP connection, alway use default one
	{
		profile = RIL_DATA_PROFILE_DEFAULT;
	}
	
	if(profile < RIL_DATA_PROFILE_DEFAULT || profile >= MAX_DATA_CALLS)
	{
		LOGW("RIL_REQUEST_SETUP_DATA_CALL: Profile %d unsupportted! \n", profile);
		goto error;
	}

	cid = atoi(g_datacalls[profile].cid);

	result[0] = g_datacalls[profile].cid;
	result[1] = is_ppp ? "ppp0" : g_datacalls[profile].ifname;
	err = getInterfaceAddr(af, result[1], ipaddress);

	//if same data profile is active and APN is same, return SUCCESS
	if (err == 0 && g_datacalls[profile].apn[0] && strcmp(apn, g_datacalls[profile].apn) == 0)
	{
		result[2] = ipaddress;
		LOGD("The PDP CID %s is already active: IP address %s for Inteface %s", g_datacalls[profile].cid, ipaddress, result[1]);
		result[3] = getDNSList(result[1]);
		result[4] = getGateway(result[1]);
		RIL_onRequestComplete(token, RIL_E_SUCCESS, result, sizeof(result));
		return;
	}
	else if(err == 0)// if the same data profile is active and APN is changed, deactive it
	{
		LOGD("Data profile %d change APN from %s to %s, deactive cid %s firstly", profile, g_datacalls[profile].apn, apn, g_datacalls[profile].cid);
		sprintf(cmdString, "AT+CGACT=0,%s", g_datacalls[profile].cid);
		at_send_command(cmdString, NULL);
		sleep(3); //Workaround: it seems CP need sometime to clear the previous PDP context before reactiving it
	}

	/* Step1: Define the CID */
	sprintf(cmdString, "AT+CGDCONT=%s,\"%s\",\"%s\"", g_datacalls[profile].cid, protocol, apn );
	err = at_send_command(cmdString, &p_response);

	if (err < 0 || p_response->success == 0)
	{
		LOGW("Fail to define the PDP context: %s", g_datacalls[profile].cid);
		sprintf(cmdString, "AT+CGACT=0,%s", g_datacalls[profile].cid);
		at_send_command(cmdString, NULL);
		goto error;
	}
	at_response_free(p_response);
	p_response = NULL;

	/* Step2: set PPP auth parameters for direct IP type*/
	if (!is_ppp)
	{
		if(auth_type == 3) //PAP /CHAP may be performed - baseband dependent.
			auth_type = 1; //use PAP as default

		sprintf(cmdString, "AT*AUTHReq=%s,%d,%s,%s", g_datacalls[profile].cid, auth_type, user ? user : "", passwd ? passwd : "");
		at_send_command(cmdString, NULL);
	}

	/* Step3: Active the PDP Context */
	sprintf(cmdString, "AT+CGDATA=\"%s\",%s", is_ppp ? "PPP" : "", g_datacalls[profile].cid);

#if defined(DKB_CP)
	err = at_send_command( cmdString, &p_response);
#elif defined(DKB_WUKONG)
	if(modem_mode == MUX_MODE)
	{
		if(is_ppp)
			err = at_switch_data_mode(&ppp_channel,cmdString,&p_response);
		else
			err = at_switch_data_mode(&wukong_pdp_channel[atoi(g_datacalls[profile].cid)],cmdString,&p_response);
	}
	else
		goto error;
#elif defined BROWNSTONE_CP
	err = at_switch_data_mode(&ppp_channel,cmdString,&p_response);
#endif

	if (err < 0 || p_response->success == 0)
	{
		LOGW("Fail to activate the PDP context: %s", g_datacalls[profile].cid);
		sprintf(cmdString, "AT+CGACT=0,%s", g_datacalls[profile].cid);
		at_send_command(cmdString, NULL);
		sleep(3); //Workaround: it seems CP need sometime to clear the previous PDP context before reactiving it
		goto error;
	}
	at_response_free(p_response);
	p_response = NULL;

	/* Step4: Enable the network interface */
	if (is_ppp)
	{
		//For PPP case
		int ret = enablePPPInterface(cid, user, passwd, ipaddress);
		if ( ret == -1)
		{
			/* deactive the CID if it is active, we don't need to care about the result */
			sprintf(cmdString, "AT+CGACT=0,%s", g_datacalls[profile].cid);
			at_send_command(cmdString, NULL);
			sleep(3); //Workaround: it seems CP need sometime to clear the previous PDP context before reactiving it
			goto error;
		}
	}
	else
	{
		//For Direct IP case
		char *line, *dns = NULL, *address = NULL;
		ATLine *p_cur;
		int found = 0;
		err = at_send_command_multiline("AT+CGDCONT?", "+CGDCONT:", &p_response);
		if (err < 0 || p_response->success == 0)
		{
			LOGW("Fail to query the PDP context");
			goto error;
		}

		p_cur = p_response->p_intermediates;
		while (p_cur)
		{
			line = p_cur->line;
			err = parsePDPContexstList(line, &cid, NULL, NULL, &address, &dns);
			if (err == 0 && cid == atoi(g_datacalls[profile].cid))
			{
				found = 1; break;
			}
			p_cur = p_cur->p_next;
		}

		if ( !found )
		{
			LOGW("Fail to find the IP address and dns for the cid: %s", g_datacalls[profile].cid);
			goto error;
		}
		setDNS(dns, g_datacalls[profile].ifname);
		FREE(dns);

		enableInterface(cid);
		int ret = configureInterface(address, g_datacalls[profile].ifname);
		FREE(address);

		if (ret == -1) goto error;
	}
	getInterfaceAddr(af, result[1], ipaddress);
	result[2] = ipaddress;
	result[3] = getDNSList(result[1]);
	result[4] = getGateway(result[1]);
	RIL_onRequestComplete(token, RIL_E_SUCCESS, result, sizeof(result));
	/* Save last time APN name */
	strcpy(g_datacalls[profile].apn, apn);

	goto exit;

 error:

	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(p_response);
	return;

}

static void onPDPContextListChanged(void *param)
{

	ATLine *p_cur;
	int err, i = 0, num = 0, cid;
	char *line;
	char *type, *apn, *address, *dns;
	ATResponse *p_response;
	RIL_Data_Call_Response *pdpResponses;
	int ActiveCount = 0;

	err = at_send_command_multiline("AT+CGDCONT?", "+CGDCONT:", &p_response);
	if (err != 0 || p_response->success == 0)
	{
		RIL_onUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED, NULL, 0);
		at_response_free(p_response);
		return;
	}

	for (p_cur = p_response->p_intermediates; p_cur != NULL; p_cur = p_cur->p_next)
		num++;
	pdpResponses = calloc(num, sizeof(RIL_Data_Call_Response));

	for (p_cur = p_response->p_intermediates; p_cur != NULL; p_cur = p_cur->p_next)
	{
		line = p_cur->line;

		err = parsePDPContexstList(line, &cid, &type, &apn, &address, NULL);
		if (err < 0) goto error;

		if (strlen(address) > 0)
		{
			pdpResponses[i].active = 2; /* 0=inactive, 1=active/physical link down, 2=active/physical link up */
			ActiveCount ++;
		}
		else
		{
			if (strcmp(type, "PPP") == 0)
				disablePPPInterface(cid);
			else
				disableInterface(cid);
		}
		pdpResponses[i].cid = cid;
		pdpResponses[i].type = type;
		pdpResponses[i].apn = apn;
		pdpResponses[i].address = address;
		i++;
	}

	if(ActiveCount == 0)//not any PDP context connected, clear cinetdev
		closeCiNetDev();

	RIL_onUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED, pdpResponses, num * sizeof(RIL_Data_Call_Response));
	freePDPContext(pdpResponses, num);
	at_response_free(p_response);
	return;

 error:
	LOGW("%s: Error parameter in response msg: %s", __FUNCTION__, line);
	RIL_onUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED, NULL, 0);
	freePDPContext(pdpResponses, num);
	at_response_free(p_response);
	return;
}




static void ril_request_setup_data_call(int request, void *data, size_t datalen, RIL_Token token)
{
	const char* radio_technology = ((const char **)data)[0];
	const char* profile_type = ((const char **)data)[1];
	const char* apn = ((const char **)data)[2];
	const char* user = ((const char **)data)[3];
	const char* passwd = ((const char **)data)[4];
	const char* auth_type_str = ((const char **)data)[5];
	const char* protocol = ((const char **)data)[6];

	int profile = atoi(profile_type);
	int auth_type = atoi(auth_type_str);


	LOGD("%s: profile_type=%d, apn=%s, user=%s,passwd=%s,auth_type=%d, protocol=%s",
		__FUNCTION__, profile, apn, user ? user : "NULL", passwd ? passwd : "NULL", auth_type, protocol);

	syncSetupDefaultPDPConnection(token, profile, apn, auth_type, user, passwd, protocol);

}

static void ril_request_last_data_call_fail_cause(int request, void *data, size_t datalen, RIL_Token token)
{
	ATResponse *response = NULL;
	int err = at_send_command_singleline("AT+PEER", "+PEER:", &response);

	if (err < 0 || response->success == 0 || !response->p_intermediates)
		goto error;

	char *line = response->p_intermediates->line;

	err = at_tok_start(&line);
	if (err < 0) goto error;

	int result;
	err = at_tok_nextint(&line, &result);
	if (err < 0) goto error;

	RIL_onRequestComplete(token, RIL_E_SUCCESS, &result, sizeof(result));
	goto exit;

error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);
}

static void ril_request_data_call_list(int request, void *data, size_t datalen, RIL_Token token)
{
	RIL_Data_Call_Response *pdpResponses = NULL;
	ATLine *p_cur;
	int i = 0, num = 0;

	LOGV("%s entry", __FUNCTION__);

	ATResponse *response = NULL;
	int err = at_send_command_multiline("AT+CGDCONT?", "+CGDCONT:", &response);

	if (err < 0 || response->success == 0)
		goto error;

	for (p_cur = response->p_intermediates; p_cur != NULL; p_cur = p_cur->p_next)
		num++;

	pdpResponses = (RIL_Data_Call_Response *)calloc(num, sizeof(RIL_Data_Call_Response));
	memset(pdpResponses, 0, num * sizeof(RIL_Data_Call_Response));

	for (i = 0, p_cur = response->p_intermediates; p_cur != NULL; i++, p_cur = p_cur->p_next)
	{
		char *line = p_cur->line;
		char *type, *apn, *address, *dns;
		int cid;
		err = parsePDPContexstList(line, &cid, &type, &apn, &address, NULL);
		if (err < 0) goto error;

		pdpResponses[i].active = strlen(address) > 0 ? 2 : 0; /* 0=inactive, 1=active/physical link down, 2=active/physical link up */
		pdpResponses[i].cid = cid;
		pdpResponses[i].type = type;
		pdpResponses[i].apn = apn;
		pdpResponses[i].address = address;
	}

	RIL_onRequestComplete(token, RIL_E_SUCCESS, pdpResponses, num * sizeof(RIL_Data_Call_Response));
	goto exit;

error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	freePDPContext(pdpResponses, num);
	at_response_free(response);
	LOGV("%s exit", __FUNCTION__);
}

static void psProcessRequest (void* arg)
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
		case RIL_REQUEST_SETUP_DATA_CALL:
		{
			ril_request_setup_data_call(r->request, r->data, r->datalen, r->token);
			break;
		}

		case RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE:
		{
			ril_handle_cmd_one_int("AT+PEER", "+PEER:", r->token);
			break;
		}

		case RIL_REQUEST_DATA_CALL_LIST:
		{
			ril_request_data_call_list(r->request, r->data, r->datalen, r->token);
			break;
		}
#ifdef MARVELL_EXTENDED
		case RIL_REQUEST_FAST_DORMANCY:
		{
		        ril_handle_cmd_default_response("AT*FASTDORM", r->token);
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

void onRequest_ps (int request, void *data, size_t datalen, RIL_Token token)
{
	LOGV("%s entry: request = %d", __FUNCTION__, request);
	enque(getWorkQueue(SERVICE_PS), psProcessRequest, (void*)newRILRequest(request, data, datalen, token));
}

void onUnsolicited_ps (const char *s)
{
	/* Really, we can ignore NW CLASS and ME CLASS events here,
	 * but right now we don't since extranous
	 * RIL_UNSOL_DATA_CALL_LIST_CHANGED calls are tolerated
	 */
	/* can't issue AT commands here -- call on ps queue thread */
	if (strStartsWith(s, "+CGEV:"))
	{
		enque(getWorkQueue(SERVICE_PS), onPDPContextListChanged, NULL);
	}
}


