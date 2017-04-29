/*------------------------------------------------------------
(C) Copyright [2006-2011] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/


/*
 * mtelif_lib.c
 *
 * Use to provide telephony interface to application layer 
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> /* close */
#include <pthread.h>
#include <termios.h>
#include <fcntl.h>
#include <cutils/log.h>
#include <errno.h>

/* PLATFORM_SDK_VERSION >= 9 means Android version >=  2.3 */
#if PLATFORM_SDK_VERSION >= 9
#include <cutils/properties.h>
#endif

#include "at_tok.h"
#include "mtelif_lib.h"
#include "agps_mtil.h"
//#include "cpaclient.h"
//#include "netlib.h"

#define LOG_TAG						"mrvl_agps"

#define LOG_BRIEF					LOGD
extern int asprintf(char **__restrict __ptr, __const char *__restrict __fmt, ...);	// Copied from stdio.h
#define DEFAULT_CID 1
#define MAX_CID 7
static int s_cid = DEFAULT_CID;

static int ReceiveDataLoop = 1;
static onMTSmsPayload ni_s_sms_cb;

#define DATABITS				CS8
#define BAUD					B115200
#define STOPBITS				0
#define PARITYON				0
#define PARITY					0


#define BUFFSIZE				2048

#define SERIAL_PORT1_NAME        "/dev/citty9"
int ATcmd_fd;
int AT_wait_count = 0;

pthread_t InputThread1;

#define WDP_DEST_SMS						0x0b84
#define SMS_CONTROL_APP_ADDR_SCHEME_16_BIT	5
#define SMS_CONTROL_CONCATENATED_SM_8_BIT       0
#define WAP_PUSH_PDU						6

void notifySmsPayload(const char* sms_pdu, unsigned long pdu_length);
extern int CpaClient_RecvNetworkUplinkCnf(UINT16 ret_code);

#define AT_PREFIX_MAX_LEN 20
/* AT CMD index table */
enum {
	CIMI_INDEX,
	CREG_INDEX,
	COPS_INDEX,
	CGACT_INDEX,
	CGDCONT_INDEX,
	CGQREQ_INDEX,
	CGQMIN_INDEX,
	CGDATA_INDEX,
	CGEREP_INDEX,
	LPNWUL_INDEX,
	LPLOCVR_INDEX,
	CGMR_INDEX,
	EXT_CTZR_INDEX,
	MODEMTYPE_INDEX,
	AT_CMD_INUSE
};
/* AT CMD prefix table
     Note: when you add new AT CMD prefix in this table,
              pelease update AT CMD index table at the same time.
*/
const char AT_PREFIX_table[AT_CMD_INUSE][AT_PREFIX_MAX_LEN]= {
						"+CIMI:",
						"+CREG:",
						"+COPS:",
						"+CGACT:",
						"+CGDCONT:",
						"+CGQREQ:",
						"+CGQMIN:",
						"+CGDATA:",
						"+CGEREP:",
						"+LPNWUL:",
						"+LPLOCVR:",
						"+CGMR:",
						"*CTZR:",
						"*MODEMTYPE:"};

static pthread_mutex_t AT_RSP_mutex[AT_CMD_INUSE];// = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t AT_RSP_cond[AT_CMD_INUSE]; //= PTHREAD_COND_INITIALIZER;
char *rsp_buf[AT_CMD_INUSE];

#define OMA_CONTEXT_TYPE	"application/vnd.omaloc-supl-init"
#define OMA_APPLICATION_ID	"x-oma-application:ulp.ua"
#define OMA_APP_ID_INT		(0xaf)
#define OMA_SMS_PORT_NUM_LEN_16BIT (4)
#define OMA_SMS_DEST_PORT_NUM	(7275)
#define WAP_PDU_LENGTH_QUOTE    (31)

#define ATCI_MAX_SMS_TDPU_SIZE            (232)
#define MAX_SMSC_CONCATEN_PDU_SIZE        (1024)
#define MAX_NUM_CONCATEN_PDU		  (5)
#define DEBUG_SMS

static char *sms_buf = NULL;
static int sms_len = 0;
static int sms_dcs = -1;
static pthread_mutex_t sms_mutex;

#define TIMEZONE_PLUS  "+"
#define TD_NW	"TTD"
enum {
	UNKNOWN_MODEM,
	PXA910_MODEM,
	PXA920_MODEM,
};
int acc_tech_mode;
int isTDNW = 0;

int FindString(char *dst, char *src);

//
// Combine 2 characters representing semi-bytes into a byte
//
unsigned char SemiByteCharsToByte(const char chHigh, const char chLow)
{
	unsigned char bRet;

	if ('0' <= chHigh && '9' >= chHigh)
	{
		bRet = (chHigh - '0') << 4;
	}
	else
	{
		if (chHigh <= 'F' && chHigh >= 'A')
			bRet = (0x0a + chHigh - 'A') << 4;
		else
			// lower case
			bRet = (0x0a + chHigh - 'a') << 4;
	}

	if ('0' <= chLow && '9' >= chLow)
	{
		bRet |= (chLow - '0');
	}
	else
	{
		if (chLow<='F' && chLow>='A')
			bRet |= (0x0a + chLow - 'A');
		else
			// lower case
			bRet |= (0x0a + chLow - 'a');
	}

	return bRet;
}

int GSMHexToGSM(const char* sIn, const unsigned int cbIn, const char* sOut, const unsigned int cbOut)
{
	char* pchIn		= (char *) (sIn);
	char* pchInEnd	= (char *) (sIn + cbIn);
	char* pchOut	= (char *) (sOut);
	char* pchOutEnd	= (char *) (sOut + cbOut);
	int rcbUsed;

	while (pchIn < pchInEnd - 1 && pchOut < pchOutEnd)
	{
		*pchOut++ = SemiByteCharsToByte(*pchIn, *(pchIn + 1));
		pchIn += 2;
	}

	rcbUsed = pchOut - sOut;
	return rcbUsed;
}

void ni_notification_thread()
{
	ni_s_sms_cb(sms_buf, sms_dcs, sms_len);
	pthread_mutex_unlock(&sms_mutex);
}

void s_sms_cb(const unsigned char *sms_payload, int dcs, int length)
{
	pthread_t notithread;

	if (sms_payload == NULL || length == 0)
	{
		LOGE("[%s] payload is NULL", __FUNCTION__);
		return;
	}
	if (sms_buf == NULL)
	{
		LOGE("[%s] sms_buf is NULL", __FUNCTION__);
		return;
	}
	pthread_mutex_lock(&sms_mutex);

	sms_len = length;
	sms_dcs = dcs;
	memcpy(sms_buf, sms_payload, length);
	if( pthread_create( &notithread, NULL, (void *)ni_notification_thread , NULL) != 0 )
	{
		LOGE("[%s]pthread_create failed!", __FUNCTION__);
		pthread_mutex_unlock(&sms_mutex);
	}

}
#if PLATFORM_SDK_VERSION >= 9
void notifySmsPayload_GB(const char * sms_pdu,unsigned long pdu_length)
{
	unsigned char idx;
	unsigned char bin_sms_pdu[ATCI_MAX_SMS_TDPU_SIZE];
	unsigned char msgLen;
	unsigned char addrLen, payLoadLen;
	int hdrpresent = 0;
	int dcs;
	int hdrLen;
	char buf[512];
	char num_str[10];
	static int	count = 0;
	static int max_num;
	static int cur_num;
	//static int offset = 0;
	int i;
	int context_len;
	unsigned long sms_dest_port;

#if 1 // Debug print of the raw SMS payload

	sprintf(buf, "[%s]: SMS raw length = %ld, SMS raw contents = ", __FUNCTION__, pdu_length);

	for (idx = 0; idx < (unsigned char) pdu_length; idx++)
	{
		sprintf(num_str, "%02X ", sms_pdu[idx]);
		strcat(buf, num_str);
	}
	LOGD("%s", buf);
#endif

	if (sms_pdu != NULL)
	{
		msgLen = (unsigned char) pdu_length;

		if (msgLen > ATCI_MAX_SMS_TDPU_SIZE)
		{
			LOGE("[%s]: Error! msgLen too large - %d", __FUNCTION__, msgLen);
			return;
		}

		memcpy((char *) bin_sms_pdu, sms_pdu, msgLen);

		idx = 0;
		dcs = 0xf5;

		LOGD("[%s]: s_sms_cb option 1", __FUNCTION__);
		s_sms_cb(&(bin_sms_pdu[idx]), dcs, msgLen);

	} else {
		LOGD("[%s]: s_sms_cb option 4", __FUNCTION__);
		s_sms_cb(NULL, 0, 0);
	}
	LOGD("End of notifySmsPayload\n");
}

#else
void notifySmsPayload(const char* sms_pdu, unsigned long pdu_length)
{
	unsigned char idx;
	unsigned char bin_sms_pdu[ATCI_MAX_SMS_TDPU_SIZE];
	unsigned char msgLen;
	unsigned char addrLen, payLoadLen;
	int hdrpresent = 0;
	int dcs;
	int hdrLen;
	char buf[512];
	char num_str[10];
	//static char toalcatbuff[MAX_SMSC_CONCATEN_PDU_SIZE];
	char *totalcatbuff = NULL;
	static char catbuff[MAX_NUM_CONCATEN_PDU][ATCI_MAX_SMS_TDPU_SIZE];
	static int  index_catbf[MAX_NUM_CONCATEN_PDU];
	static int  count = 0;
	static int max_num;
	static int cur_num;
	//static int offset = 0;
	int i;
	int context_len;
	unsigned long sms_dest_port;

#if 1 // Debug print of the raw SMS payload

	sprintf(buf, "[%s]: SMS raw length = %ld, SMS raw contents = ", __FUNCTION__, pdu_length);

	for (idx = 0; idx < (unsigned char) pdu_length; idx++)
	{
		sprintf(num_str, "%02X ", sms_pdu[idx]);
		strcat(buf, num_str);
	}
	LOGD("%s", buf);
#endif

	if (sms_pdu != NULL)
	{
//		msgLen = UpaClient_GSMHexToGSM(sms_pdu, strlen(sms_pdu), (const char *) bin_sms_pdu, ATCI_MAX_SMS_TDPU_SIZE);
		msgLen = (unsigned char) pdu_length;

		if (msgLen > ATCI_MAX_SMS_TDPU_SIZE)
		{
			LOGE("[%s]: Error! msgLen too large - %d", __FUNCTION__, msgLen);
			return;
		}

//		sprintf(buf, "[%s]: converted sms_pdu length = %d, contents = ", __FUNCTION__, msgLen);
// 
// 		for (idx = 0; idx < msgLen; idx++)
// 		{
// 			sprintf(num_str, "%02x ", bin_sms_pdu[idx]);
// 			strcat(buf, num_str);
// 		}
// 		LOGD("%s", buf);

		memcpy((char *) bin_sms_pdu, sms_pdu, msgLen);

		idx = 0;

		//the 1st octet is PDU Type
		//it contains TP-MTI, TP-MMS, TP-SRI, TP-UDHI, TP-RP
		//check if TP-UDHI is set to 1
		hdrpresent = bin_sms_pdu[idx] & 0x40;
		LOGD("[%s]: hdrpresent = 0x%02x", __FUNCTION__, hdrpresent);
		idx++;

		// Pick up the Address Length (number of BCD digits)
		addrLen = bin_sms_pdu[idx++];
		LOGD("[%s]: addrLen = 0x%02x", __FUNCTION__, addrLen);
		idx++; // skip the numbering of plan
		idx += addrLen/2 + addrLen%2; // skipped the packed BCD address

		// the protocol ID: one octet
		LOGD("[%s]: TP-PID = 0x%02x", __FUNCTION__, bin_sms_pdu[idx]);
		idx++;

		// DCS: one octet
		dcs = bin_sms_pdu[idx];
		LOGD("[%s]: TP-DCS = 0x%02x", __FUNCTION__, dcs);
		idx++;

		// skip the timestamp : 7 octets according to 23.040 spec
		idx += 7;

		// Other fields are optional - option is set using CSDH - not implemented

		// user data length
		payLoadLen = bin_sms_pdu[idx++];
		LOGD("[%s]: TP UDL = %d bytes", __FUNCTION__, payLoadLen);

		if ((hdrpresent == 0x40) && ((dcs & 0x04) == 0x04))
		{
			// TP-UDLH exist and 8-bit data coding scheme
			hdrLen = bin_sms_pdu[idx++];
			LOGD("[%s]: TP WDP UDH len = %d", __FUNCTION__, hdrLen);
			// check if IEIA is 05 IEI (Application Port Schema Addressing, 16 bit)
			if (bin_sms_pdu[idx] == SMS_CONTROL_APP_ADDR_SCHEME_16_BIT)
			{
				idx += hdrLen;
				payLoadLen -= (hdrLen + 1);
				// check if it contains WAP PUSH PDU
				if (bin_sms_pdu[idx + 1] == WAP_PUSH_PDU)
				{
					idx++; // skip push ID
					idx++; // skip PDU type
					hdrLen = bin_sms_pdu[idx++];

					if (hdrLen < WAP_PDU_LENGTH_QUOTE) // value
					{
						if (bin_sms_pdu[idx] == 0x3 &&
						    bin_sms_pdu[idx + 1] == 0x2 &&
						    bin_sms_pdu[idx + 2] == 0x3 &&
						    bin_sms_pdu[idx + 3] == 0x12)
						{
							context_len = bin_sms_pdu[idx];
							if ((bin_sms_pdu[idx + context_len + 1] != OMA_APP_ID_INT) ||
							    (bin_sms_pdu[idx + context_len + 2] != 0x90))
							{
								LOGD("[%s] APP ID error\n", __FUNCTION__);
								return;
							}
						}
						else
						{
							LOGD("[%s] CONTEXT TYPE error\n", __FUNCTION__);
							return;
						}
					}
					else // string
					{
						// WSP: get "context type" and "Application ID"
						context_len = strlen((char *)&bin_sms_pdu[idx]);

						if (strcmp((char *)&bin_sms_pdu[idx], OMA_CONTEXT_TYPE))
						{
							LOGD("[%s]: wrong OMA_CONTEXT_TYPE:%s", __FUNCTION__, &bin_sms_pdu[idx]);
							return;
						}
						LOGD("[%s]: application id %d ", __FUNCTION__, bin_sms_pdu[idx + context_len + 1]);
						if ((bin_sms_pdu[idx + context_len + 1] != OMA_APP_ID_INT) ||
								(strcmp((char *)&bin_sms_pdu[idx + context_len + 2], OMA_APPLICATION_ID)))
						{
							LOGD("[%s]: wrong OMA_APPLICATION_ID:%s", __FUNCTION__,
									&bin_sms_pdu[idx + context_len + 2]);
							return;
						}
					}
					idx += hdrLen;
					LOGD("[%s]: TP WSP len = %d", __FUNCTION__, hdrLen);
					payLoadLen -= (hdrLen + 3);
					LOGD("[%s]: s_sms_cb option 1", __FUNCTION__);
					s_sms_cb(&(bin_sms_pdu[idx]), dcs, payLoadLen);
				}

				else
				{
					//if (bin_sms_pdu[idx - hdrLen + 1] == OMA_SMS_PORT_NUM_LEN_16BIT)
					{
						sms_dest_port = 0;
						// 16bit dest port number
						sms_dest_port = ((bin_sms_pdu[idx - hdrLen + 2] << 8) |
									bin_sms_pdu[idx - hdrLen + 3]);
						if (sms_dest_port != OMA_SMS_DEST_PORT_NUM)
						{
							LOGD("[%s]: ERROR: sms_dest_port %ld is not %d", __FUNCTION__,
										sms_dest_port, OMA_SMS_DEST_PORT_NUM);
							return;
						}
					}
					LOGD("[%s]: s_sms_cb option 2", __FUNCTION__);
					s_sms_cb(&(bin_sms_pdu[idx]), dcs, payLoadLen);
				}
			}
			else if (bin_sms_pdu[idx] == SMS_CONTROL_CONCATENATED_SM_8_BIT)
			{
				//UDL+UDHL+IEIa+IEIDLa+IEDa+...+IEDa+..
				//if (bin_sms_pdu[idx+1] == 3)
				{
					max_num = bin_sms_pdu[idx+3]; // MAX number of packages
					cur_num = bin_sms_pdu[idx+4]; // Current index
					LOGD("[%s] max = %d, cur = %d", __FUNCTION__, max_num, cur_num);
					if (max_num > MAX_NUM_CONCATEN_PDU)
					{
						LOGD("[%s] Error: number of concatenated short message is too large",
											__FUNCTION__);
						return;
					}
				}
                                idx += hdrLen;
                                payLoadLen -= (hdrLen + 1);

				if (cur_num && cur_num <= max_num)
				{
					LOGD("[%s] payLoadLen %d, idx %d, -> %2x, %2x ,%2x", __FUNCTION__, payLoadLen,
										idx, bin_sms_pdu[idx],bin_sms_pdu[idx+1],
										bin_sms_pdu[idx+2]);
					memcpy(catbuff[cur_num - 1], &bin_sms_pdu[idx], payLoadLen);
					index_catbf[cur_num - 1] = payLoadLen;
					count++;
					LOGD("done");
				}

				// pass to s_sms_cb
				if (count == max_num) // collect all the messages
				{
					int offset;

					if (totalcatbuff == NULL)
					{
						totalcatbuff = (char *)malloc(sizeof(char) * MAX_SMSC_CONCATEN_PDU_SIZE);
						if (totalcatbuff == NULL)
						{
							LOGD("[%s] not enough memory for totalcatbuff\n", __FUNCTION__);
							return;
						}
						LOGD("[%s] alloc successful", __FUNCTION__);
					}

					offset = 0;
					for (i = 0; i < count; i++)
					{
						memcpy(totalcatbuff+offset, catbuff[i], index_catbf[i]);
						offset += index_catbf[i];
					}
					idx = 0;
					payLoadLen = offset;
					LOGD("..payLoadLen = %d", payLoadLen);
					// check if it contains WAP PUSH PDU
					if (totalcatbuff[idx + 1] == WAP_PUSH_PDU)
					{
						idx++; // skip push ID
						idx++; // skip PDU type
						hdrLen = totalcatbuff[idx++];
						idx += hdrLen;
						LOGD("[%s]: TP WSP len = %d", __FUNCTION__, hdrLen);
						payLoadLen -= (hdrLen + 3);
						LOGD("[%s]: s_sms_cb option 1.1", __FUNCTION__);
						s_sms_cb((totalcatbuff + idx), dcs, payLoadLen);
					}
					else
					{
						LOGD("[%s]: s_sms_cb option 2.1", __FUNCTION__);
						s_sms_cb((totalcatbuff + idx), dcs, payLoadLen);
					}
					count = 0;
					if (totalcatbuff)
					{
						free(totalcatbuff);
						totalcatbuff = NULL;
					}
				}
				else
				{
					LOGD("[%s] in concating PDU\n", __FUNCTION__);
					return;
				}
			}
			else
			{
				LOGD("[%s] meet no s_sms_cb case!!!", __FUNCTION__);
			}
		}

		else
		{
			payLoadLen = msgLen - idx;
			LOGD("[%s]: s_sms_cb option 3", __FUNCTION__);
			//s_sms_cb(&(bin_sms_pdu[idx]), dcs, payLoadLen);
		}
	}

	else
	{
		LOGD("[%s]: s_sms_cb option 4", __FUNCTION__);
		s_sms_cb(NULL, 0, 0);
	}
	LOGD("End of notifySmsPayload\n");
}
#endif
int strStartsWith(const char *line, const char *prefix)
{
    for ( ; *line != '\0' && *prefix != '\0' ; line++, prefix++) {
        if (*line != *prefix) {
            return 0;
        }
    }

    return *prefix == '\0';
}

//0 stands for fail. 1 successful.
int strEndWith(const char *line, const char *postfix)
{
    int line_len, post_len;
    int i;

    line_len = strlen(line);
    post_len = strlen(postfix);
    if (line_len < post_len)
	return 0;

    for (i = 0; (line_len - i) && (post_len - i) ; i++) {
        if (line[line_len - 1 - i] != postfix[post_len - 1 - i]) {
            return 0;
        }
    }

    return 1;
}

// 1 is for success, 0 for fail
int isWholeCMT(char *sms)
{
    int index = 0, count = 0;

    while ((index = FindString(sms+index, "\r\n")) != -1)
    {
        index += strlen("\r\n");
        count++;
    }

    if (count == 3)
       return 1;
    else
       return 0;
}

int handle_RecvNetworkUplinkCnf(char *pData)
{
	char *line, *buf;
	int err, response, index;

	LOGD("[%s] enter", __FUNCTION__);
	buf = pData;
        //Multi lines:
        while (buf && (*buf != '\0'))
        {
                index = FindString(buf, "+LPNWUL:");
                if (index < 0)
                        break;

                line = &buf[index];
                index = FindString(line, "OK\r\n");
                if (index >= 0)
                {
                        line[index] = '\0';
                        buf = line + index + 4;
                }
                else
                {
                        buf = NULL;
                }
		LOGD("%s", line);
		err = at_tok_start(&line);
		if (err < 0)
		{
			LOGD("[%s] +LPNWUL start error", __FUNCTION__);
			goto END_NWUL;
		}
		err = at_tok_nextint(&line, &response);
		if (err < 0)
		{
			LOGD("[%s] +LPNWUL get ret code error", __FUNCTION__);
			goto END_NWUL;
		}
		//CpaClient_RecvNetworkUplinkCnf(response);
	}
	CpaClient_RecvNetworkUplinkCnf(response);
	LOGD("[%s] exit", __FUNCTION__);
END_NWUL:
	return err;
}
/* +LPNWDL: bearer_type, "msg_data", msg_size, session type, RrcState, count */
int handle_RecvNetworkDownlinkInd(char *pData)
{
	char *line, *out, *buf;
	int err = -1, index;
	int bearer_type;
	UINT8 msg_data[CI_DEV_MAX_APGS_MSG_SIZE];
	int msg_size;
	int session_type, rrcstate, count;

	LOGD("Enter %s", __FUNCTION__);
	buf = pData;

	//Multi lines:
	while (buf && (*buf != '\0'))
	{
		index = FindString(buf, "+LPNWDL:");
		LOGD("buf[0]=%c buf[1]=%c index=%d", buf[0], buf[1], index);
		if (index < 0)
			break;

		line = &buf[index];
		index = FindString(line, "\r\n");
		if (index >= 0)
		{
			line[index] = '\0';
			buf = line + index + 2;
		}
		else
		{
			buf = NULL;
		}

		err = at_tok_start(&line);
		if (err < 0)
		{
			LOGD("[%s] +LPNWDL start error", __FUNCTION__);
			goto END_NWDL;
		}
		/* 1. get bearer_type */
		err = at_tok_nextint(&line, &bearer_type);
		if (err < 0)
		{
			LOGD("[%s] +LPNWDL get ret code error", __FUNCTION__);
			goto END_NWDL;
		}

		/* 2. get msg_data*/
		err = at_tok_nextstr(&line, &out);
		if (err < 0)
		{
			LOGD("[%s] +LPNWDL get string error", __FUNCTION__);
			goto END_NWDL;
		}
		GSMHexToGSM(out, strlen(out), (char *)msg_data, CI_DEV_MAX_APGS_MSG_SIZE);

		/* 3. msg_size */
		err = at_tok_nextint(&line, &msg_size);
		if (err < 0)
		{
			LOGD("[%s] +LPNWDL get msg size error", __FUNCTION__);
			goto END_NWDL;
		}

		/* 4. session type */
		err = at_tok_nextint(&line, &session_type);
		if (err < 0)
		{
			LOGD("[%s] +LPNWDL get session type error", __FUNCTION__);
			goto END_NWDL;
		}

		/* 5. RRC state */
		err = at_tok_nextint(&line, &rrcstate);
		if (err < 0)
		{
			LOGD("[%s] +LPNWDL get RRC state error", __FUNCTION__);
			goto END_NWDL;
		}

		/* 6. count */
		err = at_tok_nextint(&line, &count);
		if (err < 0)
		{
			LOGD("[%s] +LPNWDL get count error", __FUNCTION__);
			goto END_NWDL;
		}

		CpaClient_RecvNetworkDownlinkInd(bearer_type, msg_data,
									 msg_size, session_type,
									 rrcstate, count);
	}
END_NWDL:
	return err;
}

int hanlde_RecvRrcStateInd(char *pData)
{
	char *line;
	int err;
	int bearer_type, rrc_state;

	line = pData;
	err = at_tok_start(&line);
	if (err < 0)
	{
		LOGD("[%s] +LPSTATE start error", __FUNCTION__);
		goto END_STATE;
	}
	/* 1. get bearer typer */
	err = at_tok_nextint(&line, &bearer_type);
	if (err < 0)
	{
		LOGD("[%s] +LPSTATE get bearer type error", __FUNCTION__);
		goto END_STATE;
	}
	/* 2. get RRC state */
	err = at_tok_nextint(&line, &rrc_state);
	if (err < 0)
	{
		LOGD("[%s] +LPSTATE get RRC state error", __FUNCTION__);
		goto END_STATE;
	}

	CpaClient_RecvRrcStateInd(bearer_type, rrc_state);

END_STATE:
	return err;

}

int handle_RecvMeasTerminateInd(char *pData)
{
	char *line;
	int err;
	int bearer_type;

	LOGD("[%s] enter", __FUNCTION__);
	line = pData;
	err = at_tok_start(&line);
	if (err < 0)
	{
		LOGD("[%s] +LPMEAST start error", __FUNCTION__);
		goto END_MEAST;
	}
	LOGD("line:%s", line);
	err = at_tok_nextint(&line, &bearer_type);
	if (err < 0)
	{
		LOGD("[%s] +LPMEAST get bearer_type error", __FUNCTION__);
		goto END_MEAST;
	}
	CpaClient_RecvMeasTerminateInd(bearer_type);

END_MEAST:
	LOGD("[%s] exit", __FUNCTION__);
	return err;
}

int handle_RecvResetPosInfoInd(char *pData)
{
	char *line;
	int err;
	int bearer_type;

	line = pData;
	err = at_tok_start(&line);
	if (err < 0)
	{
		LOGD("[%s] +LPRESET start error", __FUNCTION__);
		goto END_RESET;
	}
	err = at_tok_nextint(&line, &bearer_type);
	if (err < 0)
	{
		LOGD("[%s] +LPRESET get bearer_type error", __FUNCTION__);
		goto END_RESET;
	}
	CpaClient_RecvResetPosInfoInd(bearer_type);

END_RESET:
	return err;
}
char TBCDparse(char in)
{
	char out = 0;

	if (in == 'f' || in == 'F')
	{
		out = 0;
	}
	else if (in >= '0' && in <= '9')
	{
		out = in;// - '0';
	}
	else if ((in >= 'c' && in <= 'e') ||
		 (in >= 'C' && in <= 'E'))
	{
		out = in - 2; //'c'-> 'a','d'->'b','e'->'c'
	}
	else if ((in == 'a' || in == 'A'))
	{
		out = '*';
	}
	else if ((in == 'b' || in == 'B'))
	{
		out = '#';
	}

	return out;
}
static UINT16 DecodeGsm7BitData( UINT8 *pInBuf, UINT16 inLen, UINT8 *pOutBuf )
{
        UINT16 inIdx, outIdx, tempIdx;
        UINT8  bits;
        BOOL   firstRun = TRUE;

        inIdx   = 0;
        outIdx  = 0;
        bits    = 0;
        while( outIdx < inLen )
        {
                /* Clear the byte before first writing to it */
                if( bits == 0 )
                {
                        pOutBuf[outIdx] = 0;
                }

                if ( bits == 0 || !firstRun )
                {
                        pOutBuf[outIdx++] |= ((pInBuf[inIdx] << bits) & 0x7F);
                }
                pOutBuf[outIdx] = pInBuf[inIdx] >> (7-bits);
                firstRun = FALSE;
                bits = ++inIdx%7;
                //if( bits == 0 && inIdx < inLen)
                if( bits == 0  && outIdx<inLen )
                {
                        outIdx++;
                }
        }
        for( tempIdx = 0; tempIdx <= outIdx; tempIdx++ )
        {
                if( pOutBuf[tempIdx] == 0)//if '@' character (GSM7bit) convert it to 0 (ASCII)
                {
                        pOutBuf[tempIdx] = 0x40;
                }
        }
        return outIdx;
}
int handle_RecvLocationInd(char *pData)
{
	CPA_LCS_LOCATION_IND CpaLcsLocationInd;
	P_CPA_LCS_LOCATION_IND pCpaLcsLocationInd = &CpaLcsLocationInd;
	char *line, *out;
	int err;
	int mInvokeHandle, notifyType, locEstType, defLocTypePresent, defLocType;
	int optClientExtIDpresent, optClientAddpresent, optClientAddlen;
	int optClientNamepresent, optClientNamesch, optClientNamelen;
	int optcnformatindpresent, optcnformatind;
	int optRequstorIdpresent, optRequstorIdsch, optRequstorIdlen;
	int optformatindpresent, optformatind;
	char output[CPA_LCS_MAX_NAME_STRING_LENGTH];
	/* +LPLOC:mIvokeHandle, notifyType,locationType.locEstimateType,
			  locationType.deferredLocationTypePresent,[locationType.deferredLocationType],
			  lcsOptClientExternalId.present,[optExternalAddress.present,[len,strID]],
			  lcsOptClientName.present,[scheme, len , name,
									optFormatIndicator.present,[formatIndicator]],
			  lcsOptRequestorId.present,[scheme, len , name,
									optFormatIndicator.present,[formatIndicator]]
	*/


	memset(pCpaLcsLocationInd, 0, sizeof(CPA_LCS_LOCATION_IND));

	line = pData;
	err = at_tok_start(&line);
	if (err < 0)
	{
		LOGD("[%s] +LPLOC start error", __FUNCTION__);
		goto END_LOCATION;
	}
	/* get mInvokeHandle */
	err = at_tok_nextint(&line, &mInvokeHandle);
	if (err < 0)
	{
		LOGD("[%s] +LPLOC get mIvokeHandle error", __FUNCTION__);
		goto END_LOCATION;
	}
	pCpaLcsLocationInd->invokeHandle = mInvokeHandle;

	/* get notifyType */
	err = at_tok_nextint(&line, &notifyType);
	if (err < 0)
	{
		LOGD("[%s] +LPLOC get mIvokeHandle error", __FUNCTION__);
		goto END_LOCATION;
	}
	pCpaLcsLocationInd->notifyType = notifyType;

	/* get locEstType */
	err = at_tok_nextint(&line, &locEstType);
	if (err < 0)
	{
		LOGD("[%s] +LPLOC get locEstType error", __FUNCTION__);
		goto END_LOCATION;
	}
	pCpaLcsLocationInd->locationType.locEstimateType = locEstType;

	/* get defLocTypePresent */
	err = at_tok_nextint(&line, &defLocTypePresent);
	if (err < 0)
	{
		LOGD("[%s] +LPLOC get defLocTypePresent error", __FUNCTION__);
		goto END_LOCATION;
	}
	pCpaLcsLocationInd->locationType.deferredLocationTypePresent = defLocTypePresent;

	if (defLocTypePresent)
	{
		/* get defLocType */
		err = at_tok_nextint(&line, &defLocType);
		if (err < 0)
		{
			LOGD("[%s] +LPLOC get defLocType error", __FUNCTION__);
			goto END_LOCATION;
		}
		pCpaLcsLocationInd->locationType.deferredLocationType = defLocType;
	}
	/* get optClientExtIDpresent */
	err = at_tok_nextint(&line, &optClientExtIDpresent);
	if (err < 0)
	{
		LOGD("[%s] +LPLOC get optClientExtIDpresent error", __FUNCTION__);
		goto END_LOCATION;
	}
	pCpaLcsLocationInd->optClientExternalId.present = optClientExtIDpresent;

	if (optClientExtIDpresent)
	{
		/* get optClientAddpresent */
		err = at_tok_nextint(&line, &optClientAddpresent);
		if (err < 0)
		{
			LOGD("[%s] +LPLOC get optClientAddpresent error", __FUNCTION__);
			goto END_LOCATION;
		}
		pCpaLcsLocationInd->optClientExternalId.optExternalAddress.present = optClientAddpresent;

		if (optClientAddpresent)
		{
			/* get optClientAddlen */
			err = at_tok_nextint(&line, &optClientAddlen);
			if (err < 0)
			{
				LOGD("[%s] +LPLOC get optClientAddlen error", __FUNCTION__);
				goto END_LOCATION;
			}
			//pCpaLcsLocationInd->optClientExternalId.optExternalAddress.addressLength = optClientAddlen;

			/* get client address string */
			err = at_tok_nextstr(&line, &out);
			if (err < 0)
			{
				LOGD("[%s] +LPLOC get string error", __FUNCTION__);
				goto END_LOCATION;
			}
			//GSMHexToGSM(out, optClientAddlen*2,\
				(char *)pCpaLcsLocationInd->optClientExternalId.optExternalAddress.externalAddress,\
				CPA_LCS_MAX_EXTERNAL_ADDRESS_SIZE);
			/*ClientExtendId is in TBCD format. Translate TBCD to UTF-8 here.
			  If UTF-8 format: "0123456", its TBCD format:"91103254f6", "91" is Numbering Plan.
			  We don't care about Numbering Plan here.
			*/
			{
				int i, j = 0;
				char *extAddr, tmp;
				extAddr = pCpaLcsLocationInd->optClientExternalId.optExternalAddress.externalAddress;
				/* i = 2, skip Numbering Plan */
				for (i = 2; (i+1) < optClientAddlen*2; i += 2)
				{
					tmp = TBCDparse(out[i+1]);
					LOGD("tmp:%c, %c", tmp, out[i+1]);
					if (tmp == 0 || tmp == 'f' || tmp == 'F')
					{
						break;
					}
					else
					{
						extAddr[j++] = tmp;
					}

					tmp = TBCDparse(out[i]);
					LOGD("tmp:%c, %c", tmp, out[i]);
					if (tmp == 0 || tmp == 'f' || tmp == 'F')
					{
						break;
					}
					else
					{
						extAddr[j++] = tmp;
					}
				}
				pCpaLcsLocationInd->optClientExternalId.optExternalAddress.addressLength = j;
			}
		}
	}
	/* get optClientNamepresent */
	err = at_tok_nextint(&line, &optClientNamepresent);
	if (err < 0)
	{
		LOGD("[%s] +LPLOC get optClientNamepresent error", __FUNCTION__);
		goto END_LOCATION;
	}
	pCpaLcsLocationInd->optClientName.present = optClientNamepresent;

	if (optClientNamepresent)
	{
		/* get  optClientNamesch */
		err = at_tok_nextint(&line, &optClientNamesch);
		if (err < 0)
		{
			LOGD("[%s] +LPLOC get optClientNamesch error", __FUNCTION__);
			goto END_LOCATION;
		}
		pCpaLcsLocationInd->optClientName.dataCodingScheme = optClientNamesch;

		/* get optClientNamelen */
		err = at_tok_nextint(&line, &optClientNamelen);
		if (err < 0)
		{
			LOGD("[%s] +LPLOC get optClientNamelen error", __FUNCTION__);
			goto END_LOCATION;
		}
		pCpaLcsLocationInd->optClientName.clientNameString.length = optClientNamelen;

		/* get client address string */
		err = at_tok_nextstr(&line, &out);
		if (err < 0)
		{
			LOGD("[%s] +LPLOC get string error", __FUNCTION__);
			goto END_LOCATION;
		}
		//GSMHexToGSM(out, optClientNamelen*2, \
				(char *)pCpaLcsLocationInd->optClientName.clientNameString.name,\
				CPA_LCS_MAX_NAME_STRING_LENGTH);
		{
			GSMHexToGSM(out, optClientNamelen*2, output, CPA_LCS_MAX_NAME_STRING_LENGTH);
			LOGD("[%s]ClientLen:%d,ClientName:%s", __FUNCTION__,
						pCpaLcsLocationInd->optClientName.clientNameString.length,
						output);

			DecodeGsm7BitData(output,
					  pCpaLcsLocationInd->optClientName.clientNameString.length,
					  pCpaLcsLocationInd->optClientName.clientNameString.name);
			pCpaLcsLocationInd->optClientName.clientNameString.length = \
					strlen(pCpaLcsLocationInd->optClientName.clientNameString.name);

			LOGD("ClientLen new:%d,ClientName new:%s",
					pCpaLcsLocationInd->optClientName.clientNameString.length,
					(char *)pCpaLcsLocationInd->optClientName.clientNameString.name);

			//pCpaLcsLocationInd->optClientName.dataCodingScheme = GPS_ENC_SUPL_UTF8;
		}
		/* get optcnformatindpresent */
		err = at_tok_nextint(&line, &optcnformatindpresent);
		if (err < 0)
		{
			LOGD("[%s] +LPLOC get optcnformatindpresent error", __FUNCTION__);
			goto END_LOCATION;
		}
		pCpaLcsLocationInd->optClientName.optFormatIndicator.present = optcnformatindpresent;

		if (optcnformatindpresent)
		{
			/* get optformatind */
			err = at_tok_nextint(&line, &optcnformatind);
			if (err < 0)
			{
				LOGD("[%s] +LPLOC get optcnformatind error", __FUNCTION__);
				goto END_LOCATION;
			}
			pCpaLcsLocationInd->optClientName.optFormatIndicator.formatIndicator = optcnformatind;
		}

	}
	/* get optRequstorIdpresent */
	err = at_tok_nextint(&line, &optRequstorIdpresent);
	if (err < 0)
	{
		LOGD("[%s] +LPLOC get optRequstorIdpresent error", __FUNCTION__);
		goto END_LOCATION;
	}
	pCpaLcsLocationInd->optRequesterId.present = optRequstorIdpresent;

	if (optRequstorIdpresent)
	{
		/* get  optRequstorIdsch */
		err = at_tok_nextint(&line, &optRequstorIdsch);
		if (err < 0)
		{
			LOGD("[%s] +LPLOC get optRequstorIdsch error", __FUNCTION__);
			goto END_LOCATION;
		}
		pCpaLcsLocationInd->optRequesterId.dataCodingScheme = optRequstorIdsch;

		/* get optRequstorIdlen */
		err = at_tok_nextint(&line, &optRequstorIdlen);
		if (err < 0)
		{
			LOGD("[%s] +LPLOC get optRequstorIdlen error", __FUNCTION__);
			goto END_LOCATION;
		}
		pCpaLcsLocationInd->optRequesterId.requesterIdString.length = optRequstorIdlen;

		/* get requestor id string */
		err = at_tok_nextstr(&line, &out);
		if (err < 0)
		{
			LOGD("[%s] +LPLOC get string error", __FUNCTION__);
			goto END_LOCATION;
		}
		GSMHexToGSM(out, optRequstorIdlen*2,
				(char *)pCpaLcsLocationInd->optRequesterId.requesterIdString.name,
				CPA_LCS_MAX_NAME_STRING_LENGTH);

		/* get optformatindpresent */
		err = at_tok_nextint(&line, &optformatindpresent);
		if (err < 0)
		{
			LOGD("[%s] +LPLOC get optformatindpresent error", __FUNCTION__);
			goto END_LOCATION;
		}
		pCpaLcsLocationInd->optRequesterId.optFormatIndicator.present = optformatindpresent;

		if (optformatindpresent)
		{
			/* get optformatind */
			err = at_tok_nextint(&line, &optformatind);
			if (err < 0)
			{
				LOGD("[%s] +LPLOC get optformatind error", __FUNCTION__);
				goto END_LOCATION;
			}
			pCpaLcsLocationInd->optRequesterId.optFormatIndicator.formatIndicator = optformatind;
		}
	}

	CpaClient_RecvLocInd((char *)pCpaLcsLocationInd);

END_LOCATION:
	return err;
}

void ReceiveDataFromChan1(void)
{
	/*
	 *  check if there is response
	 */
	int		offset = 0;
	int		nwdloff = -1;
	fd_set	fds;
	int		rc;
	char *buffer = NULL;
	char prefix[AT_PREFIX_MAX_LEN];
	int myprefix;
	int i, count = 0;
	printf("Enter %s\n", __FUNCTION__);
	LOGD("[%s] Enter, Loop flag = %d ", __FUNCTION__, ReceiveDataLoop);

	buffer = (char *)malloc(BUFFSIZE * sizeof(char));
	if (buffer == NULL)
	{
		LOGE("[%s]: buffer Mem alloc error", __FUNCTION__);
		goto EXIT_LOOP;
	}

	for (i = 0; i < AT_CMD_INUSE; ++i)
	{
		rsp_buf[i] = (char *)malloc(BUFFSIZE * sizeof(char));

		if (rsp_buf[i] == NULL)
			count++;
	}
	if (count)
	{
		LOGE("[%s]: rsp_buf Mem alloc error", __FUNCTION__);
		goto EXIT_LOOP;
	}

	while (ReceiveDataLoop) 
	{

		int bytes = 0;
		
		FD_ZERO(&fds);
		if (ATcmd_fd <= 0)
		{
			LOGD("[%s] fd is %d", __FUNCTION__, ATcmd_fd);
			usleep(10000);
			continue;
		}
		FD_SET(ATcmd_fd,&fds);

		// block until a byte is received
		rc=select(ATcmd_fd + 1, &fds, NULL, NULL, NULL);
		if (rc <= 0 || errno == EINTR)
		{
			LOGD("[%s] rc = %d\n", __FUNCTION__, rc);
			continue;
		}
		if (FD_ISSET(ATcmd_fd, &fds) == 0)
			continue;
		//LOGD("offset = %d", offset);
		bytes = read(ATcmd_fd, buffer+offset, (BUFFSIZE - offset - 1));

		if (bytes <= 0)
		{
			//LOGD("[%s] read bytes = %d\n", __FUNCTION__, bytes);
			continue;
		}

		offset += bytes;
		buffer[offset] = '\0';        /* Assure null terminated string */

		if (offset == 0)
			continue;

		//LOGD("[%s]: get %c(%x), total %d bytes", __FUNCTION__,  buffer[0],buffer[0], bytes);

		//LOGD("buffer: %s", buffer);
		// Find LPNWDL indicator from buffer
		nwdloff = FindString(buffer, "+LPNWDL:");
#if PLATFORM_SDK_VERSION < 9
		if (strStartsWith(buffer, "\r\n+CMT:") && strEndWith(buffer, "\r\n"))
		{
			int pdulen, index, i, len;
			char *line, *out, *tmp, *end;
			//Format: \r\n+CMT:,<pdu.len>\r\n<pdu.data %2x>\r\n
			//1. skip the head string "\r\n".
			LOGD("Enter SMS handler.....");
			line = buffer + strlen("\r\n+CMT:,");

			//2. get pdu len
			tmp = line;
			index = FindString(line,"\r\n");
			tmp[index] = '\0';
			LOGD("index = %d", index);
			pdulen = strtol(tmp, &end, 10);
			//pdulen = 100;
			LOGD("PDU len: %d", pdulen);

			//3. skip second \r\n in front of pdu.data
			line += index + 2;

			//4. /r/n should be filtered at the end of string.
			out = line;
			len = strlen(out) - 2;
			out[len] = '\0';
			LOGD("SMSC+PDU len: %d", len);

			for (i = 0; i < len; ++i)
			{
				if (out[i] >= '0' && out[i] <= '9')
				{
					out[i] -= '0';
				}
				else if (out[i] >= 'a' && out[i] <= 'f')
				{
					out[i] -= 'a';
					out[i] += 0xa;
				}
				else if (out[i] >= 'A' && out[i] <= 'F')
				{
					out[i] -= 'A';
					out[i] += 0xA;
				}
			}
			for (i = 0; i < len/2; i++ )
			{
				out[i] = (out[i*2] << 4 | out[i*2 + 1]);
			}
			notifySmsPayload(out+(len/2-pdulen), pdulen);
			offset = 0;
		error:
			offset = 0;
		}
		else if (strStartsWith(buffer, "\r\n+CSQ:"))
#else
		if (strStartsWith(buffer, "\r\n+CSQ:"))
#endif
		{
			//nothing.
			offset = 0;
			//LOGD("[%s] get CSQ", __FUNCTION__);
		}
		else if (strStartsWith(buffer, "\r\n+LPNWUL:"))
		{
			LOGD("Handle LPNWUL");
			handle_RecvNetworkUplinkCnf(&buffer[2]);
			offset = 0;
		}
		else if (strStartsWith(buffer, "\r\n+LPSTATE:"))
		{
			hanlde_RecvRrcStateInd(&buffer[2]);
			offset = 0;
		}
		else if (strStartsWith(buffer, "\r\n+LPMEAST:"))
		{
			handle_RecvMeasTerminateInd(&buffer[2]);
			offset = 0;
		}
		else if (strStartsWith(buffer, "\r\n+LPRESET:"))
		{
			handle_RecvResetPosInfoInd(&buffer[2]);
			offset = 0;
		}
		else if (strStartsWith(buffer, "\r\n+LPLOC:"))
		{
			handle_RecvLocationInd(&buffer[2]);
			offset = 0;
		}
		else if (AT_wait_count)
		{
			//printf("==>buf:%s\n",buffer+offset);
			//ERROR/OK in response..
			if ((FindString(buffer+offset-bytes,"ERROR") >= 0)
				|| (FindString(buffer+offset-bytes, "OK") >= 0))
			{
				int index;
				offset = 0;

				//LOGD("[%s] get AT CMD response", __FUNCTION__);
				prefix[AT_PREFIX_MAX_LEN - 1] = '\0';
				strncpy(prefix, &buffer[2], AT_PREFIX_MAX_LEN - 1);

				index = FindString(prefix, ":");
				if (index >= 0)
				{
					prefix[index + 1] = '\0';
					myprefix = getindexofPrefix(prefix);
				}
				else
				{
					myprefix = CIMI_INDEX; //default, There is no +CIMI: in CIMI rsp
				}
				if (myprefix < 0)
				{
					LOG_BRIEF("[%s] %s not in my AT list!!", __FUNCTION__, prefix);
				}
				else
				{
					pthread_mutex_lock(&AT_RSP_mutex[myprefix]);

					rsp_buf[myprefix][0] = '\0';
					strcpy(rsp_buf[myprefix], buffer);

					pthread_cond_signal(&AT_RSP_cond[myprefix]);
					pthread_mutex_unlock(&AT_RSP_mutex[myprefix]);
				}
			}
			else
			{
				//offset += bytes;
				//LOGD("[%s] get: %s", __FUNCTION__, buffer);
			}
			//usleep(1); //make the waiting thread schedulred.
		}
		else if (!AT_wait_count && nwdloff == -1) // We got part of indicator, which can't be identified as "CMT" or "CSQ" yet
		{
			// do clear offset, we should wait for the rest bytes.
			//LOGD("[%s] get part of message in %d bytes", __FUNCTION__, bytes);
#if PLATFORM_SDK_VERSION < 9
			if ((offset >= strlen("\r\n+CMT:")) && (strStartsWith(buffer, "\r\n+CMT:")))
			{
				continue;
			}
			else
#endif
			{
				offset = 0;
			}
		}
		//LOGD("go to next step.... nwdl:%d", nwdloff);
		if ( nwdloff != -1)//strStartsWith(buffer, "\r\n+LPNWDL:") && strEndWith(buffer, "\r\n"))
		{
			LOGD("Get LPNWDL:%s", &buffer[nwdloff]);
			handle_RecvNetworkDownlinkInd(&buffer[nwdloff]);
			LOGD("[%s]:%s", __FUNCTION__, &buffer[nwdloff]);
			offset = 0;
		}
	}
EXIT_LOOP:
	if (buffer)
	{
		free(buffer);
		buffer = NULL;
	}
	for (i = 0; i < AT_CMD_INUSE; ++i)
	{
		if (rsp_buf[i])
		{
			free(rsp_buf[i]);
			rsp_buf[i] = NULL;
		}
	}

	LOGD("[%s] exit\n", __FUNCTION__);
}
// int FindString(char *dst, char *src)
//return the index of the first src in dst
// dst = "abnmjmhddf" src="jm"
// return 4.
// if fails, return -1
int FindString(char *dst, char *src)
{
	int index = 0;
	
	if (!dst || !src)
		return -1;
	
	while (*dst)
	{
		if (!strncmp(dst,src,strlen(src)))
		{
			break;
		}
		dst++;
		index++;
	}
	
	if (*dst == '\0')
		return -1;
	else
		return index;
}

int MRIL_Client_Init(void)
{
	int i;
	/*
	 *  Open the Device
	 */
	struct termios oldtio, newtio; 

	if (ATcmd_fd)
		return 0;

	ATcmd_fd = open(SERIAL_PORT1_NAME, O_RDWR   ); //| O_NOCTTY

	if (ATcmd_fd <= 0) 
	{
		LOG_BRIEF("Error opening device %s\n", SERIAL_PORT1_NAME);
		return -1;
	}
	
	/*	 Changes:
		0.3 -> 0.4: support launch multi at commands.
						 support SUPL NI SMS on GB
	*/
	LOG_BRIEF("Mrvl_tel version: 0.4");

	// Make the file descriptor asynchronous (the manual page says only
	// O_APPEND and O_NONBLOCK, will work with F_SETFL...)    

	fcntl(ATcmd_fd, F_SETFL, 0);

	tcgetattr(ATcmd_fd,&oldtio); // save current port settings

	// set new port settings for canonical input processing
	newtio.c_cflag = BAUD | CRTSCTS | DATABITS | STOPBITS | PARITYON | PARITY | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	///newtio.c_lflag =  ECHOE | ECHO | ICANON;       //ICANON;
	newtio.c_lflag =  0;       //ICANON;

	newtio.c_cc[VMIN]=1;
	newtio.c_cc[VTIME]=0;
#if 1
	newtio.c_cc[VERASE] = 0x8;
	newtio.c_cc[VEOL] = 0xD;

	newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */
	newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
	newtio.c_cc[VERASE]   = 0;     /* del */
	newtio.c_cc[VKILL]    = 0;     /* @ */
	newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
	newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
	newtio.c_cc[VSWTC]    = 0;     /* '\0' */
	newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */
	newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
	newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
	newtio.c_cc[VEOL]     = 0;     /* '\0' */
	newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
	newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
	newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
	newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
	newtio.c_cc[VEOL2]    = 0;     /* '\0' */

	newtio.c_cc[VMIN]=1;
	newtio.c_cc[VTIME]=0;
	newtio.c_cc[VERASE] = 0x8;
	newtio.c_cc[VEOL] = 0xD;
#endif
	tcflush(ATcmd_fd, TCIFLUSH);
	tcsetattr(ATcmd_fd,TCSANOW,&newtio);

	for (i=0;i<AT_CMD_INUSE;++i)
	{
		if (pthread_mutex_init(&AT_RSP_mutex[i],NULL) != 0 ||
			pthread_cond_init(&AT_RSP_cond[i],NULL) != 0)
		{
			LOG_BRIEF("pthread_mutex_init or pthread_cond_init error\n");
			return -1;
		}
	}

	if (pthread_mutex_init(&sms_mutex, NULL) != 0)
	{
		LOG_BRIEF("pthread_mutex_init or pthread_cond_init error\n");
		return -1;
	}

	sms_buf = (char *)malloc(MAX_SMSC_CONCATEN_PDU_SIZE * sizeof(char));
	if (sms_buf == NULL)
	{
		LOG_BRIEF("sms_buf: Mem alloc Error\n");
		return -1;
	}
	/*
	 *  Create a thread to recieve
	 */
	 ReceiveDataLoop = 1;

	if( pthread_create( &InputThread1, NULL, (void *)ReceiveDataFromChan1 , NULL) != 0 )
	{
		LOG_BRIEF("pthread_create failed!\n");
		return -1;
	}
#if PLATFORM_SDK_VERSION >= 9
	GetModemType();
#endif
	LOG_BRIEF("[%s] complete successful!!!", __FUNCTION__);
	
	return 0;
}

int MRIL_Client_DeInit(void)
{
	void *ret;
	int i;

	LOG_BRIEF("[%s] called", __FUNCTION__);

	if (ATcmd_fd <= 0)
	{
		LOG_BRIEF("[%s] nothing to do, return", __FUNCTION__);
		return 0;
	}
	ReceiveDataLoop = 0;
	// Send some AT command like "AT+CREG?" to wake up waiting thread.
	// We don't care about which AT command sent here.
	at_send_command("AT+CREG?","+CREG:", NULL);

	if (ATcmd_fd)
	{
		close(ATcmd_fd);
		ATcmd_fd = 0;
	}

	pthread_join(InputThread1, &ret);

	for (i=0;i<AT_CMD_INUSE;++i)
	{
		if (pthread_mutex_destroy(&AT_RSP_mutex[i]) != 0 ||
			pthread_cond_destroy(&AT_RSP_cond[i]) != 0)
		{
			LOG_BRIEF("%s: ERROR: pthread_mutex_destroy or pthread_cond_destroy \
								failed", __FUNCTION__);
		}
	}

	if ((pthread_mutex_destroy(&sms_mutex) != 0))
	{
	   LOG_BRIEF("%s: ERROR: pthread_mutex_destroy sms_mutex failed", __FUNCTION__);
	}

	if (sms_buf)
	{
		free(sms_buf);
		sms_buf = NULL;
	}
#if PLATFORM_SDK_VERSION >= 9
	{
		extern void  mrvl_ril_deinit(void);
		mrvl_ril_deinit();
	}
#endif
	LOG_BRIEF("[%s] exit", __FUNCTION__);
	return 0;
}

int getindexofPrefix(char *prefix)
{
	int tab_len,i;

	tab_len = sizeof(AT_PREFIX_table)/sizeof(char[AT_PREFIX_MAX_LEN]);

	for (i = 0; i < tab_len; i++)
	{
		if (strcmp(AT_PREFIX_table[i],prefix) == 0)
			break;
	}

	if (i < tab_len)
		return i;
	else
		return -1;

}

int at_send_command(char* ATcmd, char *prefix, char *ATRsp)
{
	int rc = -1;
	int received;
	struct timeval now;
	struct timespec timeout;
	int myprefix;
	
	if (NULL == ATcmd)
		return rc;

	myprefix = getindexofPrefix(prefix);
	if (myprefix < 0)
	{
		LOG_BRIEF("[%s] %s not in my AT list!!", __FUNCTION__, ATcmd);
		return -1;
	}

	pthread_mutex_lock(&AT_RSP_mutex[myprefix]);

	received = strlen(ATcmd);

	/* when sending one AT cmd, reference count increases 1 */
	AT_wait_count++;

	rc = write(ATcmd_fd, ATcmd, received);

	rc = write(ATcmd_fd, "\r", 1);
	
	if(rc<0) 
	{
		LOG_BRIEF("cannot send AT command");
		perror("cannot send data ");	
	}

	gettimeofday(&now, NULL);
	timeout.tv_sec = now.tv_sec + 5; // wait for 5sec
	timeout.tv_nsec = now.tv_usec * 1000;

	rc = pthread_cond_timedwait(&AT_RSP_cond[myprefix], &AT_RSP_mutex[myprefix], &timeout);

	if (rc == ETIMEDOUT)
	{
		LOG_BRIEF("AT command(%s) Time out: 5sec", ATcmd);
		rc = -1;
	} else {
		if (ATRsp)
		{
			if (strStartsWith(rsp_buf[myprefix], "\r\n"))
				strcpy(ATRsp,&rsp_buf[myprefix][2]);
			else
				strcpy(ATRsp,rsp_buf[myprefix]);
		}

		//ERROR in response..
		if (FindString(ATRsp,"ERROR") >= 0)
		{
			printf("ERROR in response..\n");
			LOG_BRIEF("ERROR in response..\n");
			rc = -1;
		}
	}
	/* when getting response for this AT cmd, reference count decreases 1 */
	AT_wait_count--;

	pthread_mutex_unlock(&AT_RSP_mutex[myprefix]);

	
	return (rc<0?-1:0);
}
int RegisterMTSmsPayloadNotification(onMTSmsPayload smsCb)
{

	LOG_BRIEF("[%s] Enter!!!",__FUNCTION__);
	
	ni_s_sms_cb = smsCb;

	LOG_BRIEF("[%s] complete successful!!",__FUNCTION__);

	return 1;
}

#if PLATFORM_SDK_VERSION < 9
/*******************************************************************************\
*	Function:		GetIMSI
*	Description:	Get the International Mobile Subscriber Identity (IMSI) value
*	Parameters:		
*	Returns:		0=OK, <0=Error Code
\*******************************************************************************/
int GetIMSI(unsigned char* imsi_str)
{
// +CIMI CI_SIM_PRIM_GET_SUBSCRIBER_ID_REQ
	int err;

	err = at_send_command("AT+CIMI","+CIMI:",(char *)imsi_str);
	
	//printf("In GetIMSI: err=%d\n",err);
	//LOG_BRIEF("[%s]: err=%d, IMSI:%s", __FUNCTION__, err, imsi_str);
	return err;
}

/*******************************************************************************\
*	Function:		GetCurCellInfo
*	Description:	Get the current Cell Info value
*	Parameters:		
*	Returns:		0=OK, <0=Error Code
\*******************************************************************************/
int GetCurCellInfo(MCellInfo *pInfo)
{
// +CREG=2 CiMmPrimSetRegResultOptionReq
// +CREG? CiMmPrimGetRegResultInfoReq
// +COPS? CI_MM_PRIM_GET_CURRENT_OPERATOR_INFO_REQ
	int err;
	int response[3];
	const char *cmd;
	const char *prefix;
	char *line, *p;
	int commas;
	int skip;
	char *oper_str;
	//char LastRsp[20];
	char buf[BUFFSIZE];


	/*  Network registration events */
	//err = at_send_command("AT+CREG=2","+CREG:",NULL);

	err = at_send_command("AT+CREG?","+CREG:",buf);

	if (err != 0) goto error;

	printf("==>In %s:%s\n",__FUNCTION__,buf);
	
	line = buf;

	err = at_tok_start(&line);
	if (err < 0) goto error;

	/* Ok you have to be careful here
	 * The solicited version of the CREG response is
	 * +CREG: n, stat, [lac, cid]
	 * and the unsolicited version is
	 * +CREG: stat, [lac, cid]
	 * The <n> parameter is basically "is unsolicited creg on?"
	 * which it should always be
	 *
	 * Now we should normally get the solicited version here,
	 * but the unsolicited version could have snuck in
	 * so we have to handle both
	 *
	 * Also since the LAC and CID are only reported when registered,
	 * we can have 1, 2, 3, or 4 arguments here
	 */

	/* count number of commas */
	commas = 0;
	for (p = line ; *p != '\0' ;p++)
	{
		if (*p == ',') commas++;
	}
	printf("==>in %s:%d,%s\n",__FUNCTION__,commas,line);
	switch (commas)
	{
		case 0: /* +CREG: <stat> */
			err = at_tok_nextint(&line, &response[0]);
			if (err < 0) goto error;
			response[1] = -1;
			response[2] = -1;
		break;

		case 1: /* +CREG: <n>, <stat> */
			err = at_tok_nextint(&line, &skip);
			if (err < 0) goto error;
			err = at_tok_nextint(&line, &response[0]);
			if (err < 0) goto error;
			response[1] = -1;
			response[2] = -1;
			if (err < 0) goto error;
		break;

		case 2: /* +CREG: <stat>, <lac>, <cid> */
			err = at_tok_nextint(&line, &response[0]);
			if (err < 0) goto error;
			err = at_tok_nexthexint(&line, &response[1]);
			if (err < 0) goto error;
			err = at_tok_nexthexint(&line, &response[2]);
			if (err < 0) goto error;
		break;

		case 3: /* +CREG: <n>, <stat>, <lac>, <cid> */
			err = at_tok_nextint(&line, &skip);
			if (err < 0) goto error;
			err = at_tok_nextint(&line, &response[0]);
			if (err < 0) goto error;
			err = /*at_tok_nexthexint*/at_tok_nexthexint(&line, (int *) &pInfo->lac); //&response[1]);
			if (err < 0) goto error;
			err = /*at_tok_nexthexint*/at_tok_nexthexint(&line, (int *) &pInfo->cellId); //&response[2]);
			if (err < 0) goto error;
		break;
		case 4: /* +CREG: <n>, <stat>, <lac>, <cid>, <act>*/
			err = at_tok_nextint(&line, &skip);
			printf("==> err:%d,skip:%d\n",err,skip);
			if (err < 0) goto error;
			err = at_tok_nextint(&line, &response[0]);
			printf("==> err:%d,response:%d\n",err,response[0]);
			if (err < 0) goto error;
			err = /*at_tok_nexthexint*/at_tok_nexthexint(&line, (int *) &pInfo->lac); //&response[1]);
			printf("==> err:%d,lac:%d\n",err,pInfo->lac);
			if (err < 0) goto error;
			err = /*at_tok_nexthexint*/at_tok_nexthexint(&line, (int *) &pInfo->cellId); //&response[2]);
			printf("==> err:%d,cellId:%d\n",err,pInfo->cellId);
			if (err < 0) goto error;
		break;

		default:
			goto error;
	}

	//pInfo->lac = response[1];
	//pInfo->cellId = response[2];

	err = at_send_command("AT+COPS?","+COPS:",buf);

	if (err < 0 )
	{
		goto error;
	}
	
	line = buf;

	err = at_tok_start(&line);

	if (err < 0)
	{
		goto error;
	}
	
	//+COPS: <mode>[,<format>,<oper>[,< AcT>]]
	err = at_tok_nextint(&line, &skip);
	if (err < 0) goto error;
	err = at_tok_nextint(&line, &response[0]);
	if (err < 0) goto error;
	err = at_tok_nextstr(&line, &oper_str);
	if (err < 0) goto error;
	err = at_tok_nexthexint(&line, &skip);
	if (err < 0) goto error;

	printf("oper_str %s, len %d\n", oper_str, strlen(oper_str));
	pInfo->CountryCode = ((oper_str[0]-'0')*100)+((oper_str[1]-'0')*10)+(oper_str[2]-'0');

	if (strlen(oper_str) > 5)
	{
		pInfo->NetworkCode = ((oper_str[3]-'0')*100)+((oper_str[4]-'0')*10)+(oper_str[5]-'0');
	}

	else
	{
		pInfo->NetworkCode = ((oper_str[3]-'0')*10)+(oper_str[4]-'0');
	}

	printf("CountryCode %d, NetworkCode %d\n", pInfo->CountryCode, pInfo->NetworkCode);
	printf("LAC %d, Cell ID %d\n", pInfo->lac, pInfo->cellId);

error:
	return err;
}

/*******************************************************************************\
*	Function:		GetAccTechMode
*	Description:	Get the Access Technology Mode value
*	Parameters:		
*	Returns:		0=OK, <0=Error Code
\*******************************************************************************/
int GetAccTechMode(AccTechMode* p_acc_tech_mode)
{
	char *line;
	char *skip_str;
	char *version_str;
	int skip;
	int err;
	char LastRsp[20];
	char buf[BUFFSIZE];

	// Get CP image version to identify if TDSCDMA or WCDMA
	err = at_send_command("AT+CGMR","+CGMR:",buf);
	if (err < 0)
	{
		goto error;
	}
	line = buf;
	err = at_tok_start(&line);
	if (err < 0)
	{
		goto error;
	}
	err = at_tok_nextstr(&line, &version_str);
	if (err < 0) goto error;
	LOG_BRIEF("CP version: %s", version_str);
	if (!strncmp(version_str, TD_NW, strlen(TD_NW)))
	{
		isTDNW = 1;
	}
	LOG_BRIEF("TD NetWork:%s", isTDNW?"Yes":"No");

	// "AT+COPS?" to get acct
	err = at_send_command("AT+COPS?","+COPS:",buf);

	if (err < 0 )
	{
		goto error;
	}

	line = buf;

	err = at_tok_start(&line);

	if (err < 0)
	{
		goto error;
	}

	//+COPS: <mode>[,<format>,<oper>[,< AcT>]]
	err = at_tok_nextint(&line, &skip);
	if (err < 0) goto error;
	err = at_tok_nextint(&line, &skip);
	if (err < 0) goto error;
	err = at_tok_nextstr(&line, &skip_str);
	if (err < 0) goto error;
	err = at_tok_nexthexint(&line, &acc_tech_mode);
	if (err < 0) goto error;

	if (acc_tech_mode >= NUM_ACT)
	{
		acc_tech_mode = ACT_GSM;
	}
	if (acc_tech_mode == ACT_UTRAN && isTDNW)
	{
		acc_tech_mode = ACT_TDSCDMA;
	}

	*p_acc_tech_mode = acc_tech_mode;

	LOG_BRIEF("AcT %d\n", acc_tech_mode);

error:
	return err;
}

#else

/*******************************************************************************\
*	Function:		GetAccTechMode
*	Description:	Get the Access Technology Mode value
*	Parameters:
*	Returns:		0=OK, <0=Error Code
\*******************************************************************************/
int GetAccTechMode(AccTechMode* p_acc_tech_mode)
{
	*p_acc_tech_mode = acc_tech_mode;
	return 0;
}

/*******************************************************************************\
*	Function:		isTDPlatform
*	Description:	if current Platform is TD, return true.
\*******************************************************************************/
int isTDPlatform(void)
{
	return isTDNW;
}

int GetModemType()
{
	char *line;
	int err = 0;
	char buf[BUFFSIZE];
	int modemtype;

	// Get modem type to identify if TDSCDMA or WCDMA
	err = at_send_command("AT*MODEMTYPE?","*MODEMTYPE:",buf);
	if (err < 0)
	{
		goto error;
	}
	line = buf;
	err = at_tok_start(&line);
	if (err < 0)
	{
		goto error;
	}
	err = at_tok_nextint(&line, &modemtype);
	if (err < 0) goto error;

	if (modemtype == PXA920_MODEM)
	{
		isTDNW = 1;
	}
	LOG_BRIEF("TD Modem: %s", isTDNW?"Yes":"No");

error:
	return err;
}


#endif

int IsPSconnectionEstablish()
{
	char *cmd;
	int err = 0;
	int cid, active = 0;
	int retry_cnt = 0;
	int found = 0;
	char *out;
	char* skip;
	char *line;
	char LastRsp[20];
	char MYBUF[BUFFSIZE];
	char *buf = MYBUF;
	int index;


	// check if there is active GPRS connection, if yes just use it
	//s_cid = 0;

	err = at_send_command("AT+CGACT?","+CGACT:",buf);

	if (err < 0)
		goto error;
	
	//MultiLines: get single line from multilines strings.
	if (err == 0)
	{		
		while(*buf)
		{
			//char *line = p_cur->line;
			index = FindString(buf, "\r\n");
			line = buf;
			line[index] = '\0';
			buf += index + 2;

			printf("line:%s\n",line);
			printf("buf:%s\n",buf);
			
			if (strcmp(line,"OK") == 0)
				break;


			err = at_tok_start(&line);
			if (err < 0)
				goto error;

			err = at_tok_nextint(&line, &cid);
			if (err < 0)
				goto error;

			err = at_tok_nextint(&line, &active);
			if (err < 0)
				goto error;

			// found the active PDP context, use it 
			if (active == 1)
			{
				//s_cid = cid;
				found = 1;
				LOGI("found the active PDP context %d, use it\n", cid);
				break;
			}
		}
	}
	
	return (err?0:active);

error:
	LOGE("error in at_send_command %d\n", err);

	return 0;
}

// return context id
int EstablishPSconnection(unsigned char* apn, unsigned char* userid, unsigned char* password, int usePPP)
{
	char *cmd;
	int err = 0;
	int cid, active = 0;
	int retry_cnt = 0;
	int found = 0;
	char *out;
	char* skip;
	char *line;
	char LastRsp[20];
	char MYBUF[BUFFSIZE];
	char *buf = MYBUF;
	int index;

	// not sure how to handle userid and password, ignore them for now
	(void) userid; // dummy parameter usage
	(void) password; // dummy parameter usage

	// check if there is active GPRS connection, if yes just use it
	s_cid = 0;

	err = at_send_command("AT+CGACT?","+CGACT:",buf);

	if (err < 0)
		goto error;
	
	//MultiLines: get single line from multilines strings.
	if (err == 0)
	{		
		while(*buf)
		{
			//char *line = p_cur->line;
			index = FindString(buf, "\r\n");
			line = buf;
			line[index] = '\0';
			buf += index + 2;

			printf("line:%s\n",line);
			printf("buf:%s\n",buf);
			
			if (strcmp(line,"OK") == 0)
				break;


			err = at_tok_start(&line);
			if (err < 0)
				goto error;

			err = at_tok_nextint(&line, &cid);
			if (err < 0)
				goto error;

			err = at_tok_nextint(&line, &active);
			if (err < 0)
				goto error;

			// found the active PDP context, use it 
			if (active == 1)
			{
				s_cid = cid;
				found = 1;
				LOGI("found the active PDP context %d, use it\n", cid);
				break;
			}
		}
	}

	if (s_cid == 0)
	{
		// no active PDP context exist, set up one
		for (s_cid = DEFAULT_CID; s_cid <= MAX_CID; s_cid++)
		{
			printf("\n==> AT+CGDCONT=%d,\"IP\",\"%s\",,0,0\n\n", s_cid, apn);
			asprintf(&cmd, "AT+CGDCONT=%d,\"IP\",\"%s\",,0,0", s_cid, apn);
			//FIXME check for error here
			err = at_send_command(cmd,"+CGDCONT:",NULL);
			free(cmd);

#if 0
			// Set required QoS params to default
			err = at_send_command("AT+CGQREQ=1", NULL);

			// Set minimum QoS params to default
			err = at_send_command("AT+CGQMIN=1", NULL);

			// packet-domain event reporting
			err = at_send_command("AT+CGEREP=1,0", NULL);

			// Hangup anything that's happening there now
			err = at_send_command("AT+CGACT=1,0", NULL);
#endif

			// Start data on PDP context
			if (usePPP)
				asprintf(&cmd, "AT+CGDATA=\"ppp\",%d", s_cid);
			else
				asprintf(&cmd, "AT+CGDATA=\"\",%d", s_cid);

			err = at_send_command(cmd,"+CGDATA:",buf);
			free(cmd);

			if (err < 0)
			{
				err = -1;
				goto error;
			}
			
			line = buf;

			while(*line)
			{
				if (strncmp(line, "CONNECT",7) == 0)
				{
					break;
				}
			}

			if (*line == '\0') // Not found "CONNECT" in response
				continue;

			// wait for address to be assigned
			for (retry_cnt = 0; retry_cnt < 5; retry_cnt++)
			{
				
				err = at_send_command("AT+CGDCONT?","+CGDCONT:",buf);
				if (err < 0)
					goto error;

				if (err == 0)
				{
					//MultiLines: get single line from multilines strings.
					
					//line = strtok(buf,"\r\n");
					while(*buf)
					{
						//char *line = p_cur->line;
						int cid;
// 						char *type;		// unused variable
// 						char *apn;		// unused variable
// 						char *address;	// unused variable

						index = FindString(buf, "\r\n");
						line = buf;
						line[index] = '\0';
						buf += index + 2;

						if (strcmp(line,"OK") == 0)
							break;
						
						printf("line:%s\n",line);
						printf("buf:%s\n",buf);

						err = at_tok_start(&line);
						if (err < 0)
							goto error;

						err = at_tok_nextint(&line, &cid);
						if (err < 0)
							goto error;

						err = at_tok_nextstr(&line, &skip);
						if (err < 0)
							goto error;

						//responses[i].type = alloca(strlen(out) + 1);
						//strcpy(responses[i].type, out);

						// apn
						err = at_tok_nextstr(&line, &skip);
						if (err < 0)
							goto error;

						//responses[i].apn = alloca(strlen(out) + 1);
						//strcpy(responses[i].apn, out);

						//address
						err = at_tok_nextstr(&line, &out);
						if (err < 0)
							goto error;

						//responses[i].address = alloca(strlen(out) + 1);
						//strcpy(responses[i].address, out);
						if (cid == s_cid && out != NULL && strlen(out) > 7 && strcmp(out, "10.0.0.1") != 0)
						{
							// address assigned
							int cinetdevfd = -1;
							char setif_str[100];

							found = 1;
							
							if (usePPP == 0)
							{
								printf("\n===> ready to open /dev/ccichar\n");
								
								if(cinetdevfd <= 0)
									cinetdevfd = open("/dev/ccichar", O_RDWR);
								
								if(cinetdevfd<0)
								{
									printf("Error open /dev/ccichar\n");
									return (-1);
								}
#define CCINET_MAJOR 241
#define CCINET_IOC_MAGIC CCINET_MAJOR
#define CCINET_IP_ENABLE  _IOW(CCINET_IOC_MAGIC, 1, int)

								ioctl(cinetdevfd, CCINET_IP_ENABLE, cid);
								printf("\n ==> ioctl is done\n");

								sprintf(setif_str, "ifconfig ccinet%1d %s",cid, out);
								printf("set up interface: %s\n", setif_str);
								system(setif_str);
								sprintf(setif_str, "route add default gw %s", out);
								printf("set up route: %s\n", setif_str);
								system(setif_str);

								close(cinetdevfd);
							}
							break;
						}
						//line = strtok(buf,"\r\n");
					}
				}

				if (found==1)
				{
					break;
				}
				else
					err = 1;
				sleep(2);
			}

			if (found == 1 || usePPP == 1)
			{
				break;
			}
		}
	}
	return (err?0:1);

error:
	LOGE("error in at_send_command %d\n", err);

	return (err?0:1);
}

int DisconnectGPRSconnection()
{
	char *cmd;
	int err=0;

	asprintf(&cmd, "AT+CGACT=0");
	
	//FIXME check for error here
	err = at_send_command(cmd,"+CGACT:",NULL);
	free(cmd);
	
	if (err < 0 )
	{
		goto error;
	}

error:
	return err;
}

int at_send_command_async(char* ATcmd, char *prefix)
{
	int rc = -1;
	int received;
	struct timeval now;
	struct timespec timeout;
	char buf[20];
	int myprefix;

	if (NULL == ATcmd)
		return rc;
	//strcpy(Myprefix,prefix);
	myprefix = getindexofPrefix(prefix);
	if (myprefix < 0)
		return -1;

	pthread_mutex_lock(&AT_RSP_mutex[myprefix]);

	received = strlen(ATcmd);

	//printf("==>Send: %s\n len:%d\n",ATcmd,received);
	//LOG_BRIEF("==>Send: %s len:%d\n", ATcmd, received);

	rc = write(ATcmd_fd, ATcmd, received);

	//LOG_BRIEF("[%s] rc = %d", __FUNCTION__, rc);
	rc = write(ATcmd_fd, "\r", 1);

	if(rc < 0)
	{
		LOG_BRIEF("cannot send AT command");
		perror("cannot send data ");
	}

	pthread_mutex_unlock(&AT_RSP_mutex[myprefix]);

	return (rc<0?-1:0);
}
#if PLATFORM_SDK_VERSION >= 9
void MRIL_get_gprs_interface(char *gprs_buf)
{
	// check APN name here.
	static char gprs_ppp0[] = "ppp0";
	static char gprs_ccinet0[] = "ccinet0";
	static char *gprs_interface = NULL;
	char value[PROPERTY_VALUE_MAX];
	//char ipaddress[64];
	//int err = -1;
	int cid;

	if (gprs_buf == NULL)
	{
		LOGE("[%s] get NULL gps_interface!!", __FUNCTION__);
		return;
	}

	cid = atoi("1");
	property_get("marvell.ril.ppp.enabled", value, "1");
	if (atoi(value))
	{
		LOGD("PPP is used");
		//Sometimes, gps_init is called before ppp is ready.

		//err = getInterfaceAddr(cid, gprs_ppp0, ipaddress);
		//if (err == 0)
		{
			gprs_interface = gprs_ppp0;
		}
	}
	else
	{
		LOGD("Direct IP is used");
		//err = getInterfaceAddr(cid, gprs_ccinet0, ipaddress);
		//if (err == 0)
		{
			gprs_interface = gprs_ccinet0;
		}
	}
	LOGD("set gprs:%s to gps module", gprs_interface);
	strcpy(gprs_buf, gprs_interface);
}
#endif

/*******************************************************************************\
*	Function:		UpaClient_GetNitzInfo
*	Description:	Get the NITZ (Network Informed Time Zone) data
*					from the telephony server.
*	Parameters:		See below
*	Returns:		0=OK, <0=Error Code
\*******************************************************************************/
int UpaClient_GetNitzInfo(AGPS_NITZ_STATUS_MSG* pNitzStatus)
{
	char *line;
	int err = 0;
	char buf[BUFFSIZE];
	char *sign_str;
	int bpresent, year, month, day, hour, minute, second, timezone;

	err = at_send_command("AT*CTZR?","*CTZR:",buf);

	if (err < 0)
	{
		goto error;
	}
	line = buf;
	err = at_tok_start(&line);
	if (err < 0)
	{
		goto error;
	}

	/* *CTZR: bPresent[,year,month,day,hour,minute,second,sign,timezone]*/
	err = at_tok_nextint(&line, &bpresent);
	if (err < 0) goto error;

	// Reset the DST struct
	memset(pNitzStatus, 0, sizeof(AGPS_NITZ_STATUS_MSG));

	if (bpresent == 1)
	{
		int sign;

		err = at_tok_nextint(&line, &year);
		if (err < 0) goto error;
		err = at_tok_nextint(&line, &month);
		if (err < 0) goto error;
		err = at_tok_nextint(&line, &day);
		if (err < 0) goto error;
		err = at_tok_nextint(&line, &hour);
		if (err < 0) goto error;
		err = at_tok_nextint(&line, &minute);
		if (err < 0) goto error;
		err = at_tok_nextint(&line, &second);
		if (err < 0) goto error;
		err = at_tok_nextstr(&line, &sign_str);
		if (!strncmp(sign_str, TIMEZONE_PLUS, strlen(TIMEZONE_PLUS)))
		{
			sign = 1; //'+';
		}
		else
		{
			sign = -1; //'-';
		}

		if (err < 0) goto error;
		err = at_tok_nextint(&line, &timezone);
		if (err < 0) goto error;


		// Copy the data
		pNitzStatus->year = year;
		pNitzStatus->month = month;
		pNitzStatus->day = day;
		pNitzStatus->hour = hour;
		pNitzStatus->minute = minute;
		pNitzStatus->sec = second;
		pNitzStatus->time_zone = sign * timezone;
		pNitzStatus->uncertainty = 0; // TBD

		LOGD("[%s]: %04d/%02d/%02d %02d:%02d:%02d UTC. Local TZ=UTC%c%02d",
			__FUNCTION__,
			(int) (pNitzStatus->year + 2000),
			(int) pNitzStatus->month,
			(int) pNitzStatus->day,
			(int) pNitzStatus->hour,
			(int) pNitzStatus->minute,
			(int) pNitzStatus->sec,
			(sign > 0? '+':'-'),
			(int) (pNitzStatus->time_zone / 4));

		pNitzStatus->ret_code = 0; // OK
	}
	else
	{
		LOGI("[%s]: NITZ info not available", __FUNCTION__);
		pNitzStatus->ret_code = 1; // Not OK
	}

error:
	LOGD("[%s]: Exit point. rc = %d", __FUNCTION__, err);
	return (err);
}

