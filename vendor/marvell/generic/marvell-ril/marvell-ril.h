/******************************************************************************
 * *(C) Copyright 2008 Marvell International Ltd.
 * * All Rights Reserved
 * ******************************************************************************/

#ifndef MARVELL_RIL_H_
#define MARVELL_RIL_H_

#include <telephony/ril.h>
#include <stdlib.h>

#define LOG_TAG getLogTag()
#include <utils/Log.h>

#define MAX_DATA_CALLS     8
#define MAX_AT_LENGTH     640   //512   //128

#define FREE(a)      if (a != NULL) { free(a); a = NULL; }

#ifdef RIL_SHLIB
const struct RIL_Env *s_rilenv;

#define RIL_onRequestComplete(t, e, response, responselen) s_rilenv->OnRequestComplete(t, e, response, responselen)
#define RIL_onUnsolicitedResponse(a, b, c) s_rilenv->OnUnsolicitedResponse(a, b, c)
#define RIL_requestTimedCallback(a, b, c) s_rilenv->RequestTimedCallback(a, b, c)
#endif

typedef enum {
	SIM_ABSENT = 0,
	SIM_NOT_READY = 1,
	SIM_READY = 2, /* SIM_READY means the radio state is RADIO_STATE_SIM_READY */
	SIM_PIN = 3,
	SIM_PUK = 4,
	SIM_NETWORK_PERSONALIZATION = 5,
	SIM_PIN2 = 6,
	SIM_PUK2 = 7,
	SIM_NETWORK_SUBSET_PERSONALIZATION = 8,
	SIM_SERVICE_PROVIDER_PERSONALIZATION = 9,
	SIM_CORPORATE_PERSONALIZATION = 10,
	SIM_SIM_PERSONALIZATION = 11,
	SIM_NETWORK_PERSONALIZATION_PUK = 12,
	SIM_NETWORK_SUBSET_PERSONALIZATION_PUK = 13,
	SIM_SERVICE_PROVIDER_PERSONALIZATION_PUK = 14,
	SIM_CORPORATE_PERSONALIZATION_PUK = 15,
	SIM_SIM_PERSONALIZATION_PUK = 16
} SIM_Status;

typedef enum {
	SERVICE_CC,
	SERVICE_DEV,
	SERVICE_MM,
	SERVICE_MSG,
	SERVICE_PS,
	SERVICE_SIM,
	SERVICE_SS,
	SERVICE_UNSOL,
	SERVICE_TOTAL
} ServiceType;
#define SERVICE_NULL  SERVICE_TOTAL

/*
enum channel_state {
	CHANNEL_IDLE,
	CHANNEL_BUSY,
};
*/
struct channel_description
{
	//int channelID;
	//int fd;
	const char* ttyName;
	const char* workQueueName;
	struct WorkQueue* workQueue;
	//enum channel_state state;
	//pthread_mutex_t mutex;
};


struct psd_channel_decription
{
	int channelID;
	int fd;
	char ttyName[128];
	char ttyLock[128];
};



typedef struct ThreadContext_s
{
	char threadname[16];
	int write_channel;
	long long channel_timeout_msec;
} ThreadContext;

/* I/F implemented in ril.cpp */
extern const char * requestToString(int request);


/*Extern  I/F implemented in marvell-ril.c */
const char* getLogTag();
void setThreadContext(ThreadContext* context);
ThreadContext* getThreadContext();

struct WorkQueue* getWorkQueue(int service);
void ril_handle_cmd_default_response(const char* cmd, RIL_Token token);
void ril_handle_cmd_sms_one_int(const char* cmd, const char* sms_pdu, const char* prefix, RIL_Token token);
inline static void ril_handle_cmd_one_int(const char* cmd, const char* prefix, RIL_Token token)
{
	ril_handle_cmd_sms_one_int(cmd, NULL, prefix, token);
}

typedef enum {
	CP,
	BROWNSTONE,
	WUKONG
}Modem_Type;

enum {
	NORMAL_MODE,
	MUX_MODE
};

extern int modem_mode;
extern Modem_Type modemType;
extern struct channel_description descriptions[];
extern int service_channel_map[];
extern int get_channel_number(void);

typedef enum {
	CHANNEL_CMD00,
	CHANNEL_CMD01,
	CHANNEL_CMD02,
	CHANNEL_CMD03,
	CHANNEL_CMD04,
	CHANNEL_CMD05,
	CHANNEL_CMD06,
	CHANNEL_CMD07,
	CHANNEL_PSD01, //for ppp
	CHANNEL_PSD02,
	CHANNEL_PSD03,
	CHANNEL_PSD04,
	CHANNEL_PSD05,
	CHANNEL_PSD06,
	CHANNEL_PSD07,
	CHANNEL_PSD08,
	CHANNEL_PSD09,
	CHANNEL_PSD10,
} ChannelType;

#define CHANNEL_UNSOLICITED   service_channel_map[SERVICE_UNSOL]
#define CHANNEL_DATA   service_channel_map[SERVICE_PS]

void setRadioState(RIL_RadioState newState);
RIL_RadioState getRadioState();
int needContinueProcess(int request, void *data, size_t datalen, RIL_Token token);


/* I/F implemented in ril-xxx.c */
void onRequest_cc(int request, void *data, size_t datalen, RIL_Token token);
void onRequest_dev(int request, void *data, size_t datalen, RIL_Token token);
void onRequest_mm(int request, void *data, size_t datalen, RIL_Token token);
void onRequest_msg(int request, void *data, size_t datalen, RIL_Token token);
void onRequest_ps(int request, void *data, size_t datalen, RIL_Token token);
void onRequest_sim(int request, void *data, size_t datalen, RIL_Token token);
void onRequest_ss(int request, void *data, size_t datalen, RIL_Token token);

void onUnsolicited_cc(const char *s);
void onUnsolicited_dev(const char *s);
void onUnsolicited_mm(const char *s);
void onUnsolicited_msg(const char *s, const char *smsPdu);
void onUnsolicited_ps(const char *s);
void onUnsolicited_sim(const char *s);
void onUnsolicited_ss(const char *s);

// in dataapi.c
void disableAllInterfaces();

int isRadioOn(void);
//int isRegistered(void);
void resetLocalRegInfo(void);
void updateLocalRegInfo(void *param);
void enque_initializeCallback();


// AT*BAND related definition
//
// mode definition
#define MODE_GSM_ONLY 0
#define MODE_UMTS_ONLY 1
#define MODE_DUAL_MODE_AUTO 2
#define MODE_DUAL_MODE_GSM_PREFERRED 3
#define MODE_DUAL_MODE_UMTS_PREFERRED 4

//GSM band bit definition
#define GSMBAND_PGSM_900    0x01
#define GSMBAND_DCS_GSM_1800  0x02
#define GSMBAND_PCS_GSM_1900  0x04
#define GSMBAND_EGSM_900   0x08
#define GSMBAND_GSM_450  0x10
#define GSMBAND_GSM_480 0x20
#define GSMBAND_GSM_850 0x40

//UMTS band bit
#define UMTSBAND_BAND_1 0x01  //IMT-2100
#define UMTSBAND_BAND_2 0x02  //PCS-1900
#define UMTSBAND_BAND_3 0x04  //DCS-1800
#define UMTSBAND_BAND_4 0x08  //AWS-1700
#define UMTSBAND_BAND_5 0x10  //CLR-850
#define UMTSBAND_BAND_6 0x20  //800Mhz
#define UMTSBAND_BAND_7 0x40  //IMT-E 2600
#define UMTSBAND_BAND_8 0x80  //GSM-900
#define UMTSBAND_BAND_9 0x100 //not used

#endif // MARVELL_RIL_H_

