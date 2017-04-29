/******************************************************************************
 * *(C) Copyright 2008 Marvell International Ltd.
 * * All Rights Reserved
 * ******************************************************************************/

#include <assert.h>
#include <cutils/properties.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>


#include "at_tok.h"
#include "atchannel.h"
#include "marvell-ril.h"
#include "misc.h"
#include "work-queue.h"

static pthread_key_t sKey;

/* Global varialble used in ril layer */
RIL_RadioState sState = RADIO_STATE_UNAVAILABLE;
int sFirstGet = 1;  //default sFirstGet = TRUE

pthread_mutex_t s_state_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t s_state_cond = PTHREAD_COND_INITIALIZER;

int s_closed = 0;  /* trigger change to this with s_state_cond */
static int s_newrilconnection = 1; /*used to identify whether it is new RIL connection between RILD and App telephony*/
extern int AfterCPReset;

void setNetworkStateReportOption(int flag);
int getSimStatus(void);
void updateRadioState(void);

/* static I/F which will be called by Android upper layer */
static void onRequest(int request, void *data, size_t datalen, RIL_Token token);
static RIL_RadioState onCurrentState();
static int onSupports(int requestCode);
static void onCancel(RIL_Token token);
static void onConfirm(int channelID, ATResponse* response, int cookie);
static const char *onGetVersion();

/* Internal function declaration */
static void onUnsolicited(const char *s, const char *smsPdu);

/*** Static Variables ***/
static const RIL_RadioFunctions s_callbacks = {
	RIL_VERSION,
	onRequest,
	onCurrentState,
	onSupports,
	onCancel,
	onGetVersion
};

#define DATABITS       CS8
#define BAUD          B115200
#define STOPBITS        0
#define PARITYON        0
#define PARITY            0

#ifdef DKB_CP   //FOR CP
struct channel_description descriptions[] = {
	[CHANNEL_CMD00] =    { "/dev/citty0", "RIL-CC", NULL },
	[CHANNEL_CMD01] =   {  "/dev/citty1", "RIL-DEV", NULL },
	[CHANNEL_CMD02] =    {  "/dev/citty2", "RIL-MM", NULL },
	[CHANNEL_CMD03] =   {  "/dev/citty3", "RIL-MSG", NULL },
	[CHANNEL_CMD04] =    {  "/dev/citty4", "RIL-PS", NULL },
	[CHANNEL_CMD05] =   {  "/dev/citty5", "RIL-SIM", NULL },
	[CHANNEL_CMD06] =    {  "/dev/citty6", "RIL-SS", NULL },
	[CHANNEL_CMD07] =    {  "/dev/citty7", NULL, NULL }
};

int service_channel_map[] = {
	[SERVICE_CC] = CHANNEL_CMD00,
	[SERVICE_DEV] = CHANNEL_CMD01,
	[SERVICE_MM] = CHANNEL_CMD02,
	[SERVICE_MSG] = CHANNEL_CMD03,
	[SERVICE_PS] = CHANNEL_CMD04,
	[SERVICE_SIM] = CHANNEL_CMD05,
	[SERVICE_SS]  = CHANNEL_CMD06,
	[SERVICE_UNSOL] = CHANNEL_CMD07,
};
struct psd_channel_decription ppp_channel =
{
	0,
	0,
	"/dev/cidatatty1",
	"/var/lock/LCK..cidatatty1"
};

#elif defined(BROWNSTONE_CP) //For brownstone
struct channel_description descriptions[] = {
	[CHANNEL_CMD00] = {  "/dev/ttyACM2", "RIL-WorkQueue", NULL },
};

int service_channel_map[] = {
	[SERVICE_CC] = CHANNEL_CMD00,
	[SERVICE_DEV] = CHANNEL_CMD00,
	[SERVICE_MM] = CHANNEL_CMD00,
	[SERVICE_MSG] = CHANNEL_CMD00,
	[SERVICE_PS] = CHANNEL_CMD00,
	[SERVICE_SIM] = CHANNEL_CMD00,
	[SERVICE_SS]  = CHANNEL_CMD00,
	[SERVICE_UNSOL] = CHANNEL_CMD00,
};

struct psd_channel_decription ppp_channel =
{
	0,
	0,
	"/dev/ttyACM0",
	"/var/lock/LCK..ttyACM0"
};

#elif defined(DKB_WUKONG) //For WuKong
#if !defined(MUX_DISABLE) //USE MUX TTY
struct channel_description descriptions[] = {
	[CHANNEL_CMD00] = {  "/dev/mux1",  "RIL-CC", NULL},
	[CHANNEL_CMD01] = {  "/dev/mux2", "RIL-DEV-MM-SIM", NULL},
	[CHANNEL_CMD02] = {  "/dev/mux3", "RIL-MSG", NULL},
	[CHANNEL_CMD03] = {  "/dev/mux4", "RIL-PS", NULL},
	[CHANNEL_CMD04] = {  "/dev/mux5", "RIL-SS", NULL},
	[CHANNEL_CMD05] = {  "/dev/mux6", NULL, NULL}
};
int service_channel_map[]={
[SERVICE_CC] = CHANNEL_CMD00,
[SERVICE_DEV] = CHANNEL_CMD01,
[SERVICE_MM] = CHANNEL_CMD01,
[SERVICE_MSG] = CHANNEL_CMD02,
[SERVICE_PS] = CHANNEL_CMD03,
[SERVICE_SIM] = CHANNEL_CMD01,
[SERVICE_SS] = CHANNEL_CMD04,
[SERVICE_UNSOL] = CHANNEL_CMD05,
};
#else  //USE SINGLE TTY
struct channel_description descriptions[] = {
	[CHANNEL_CMD00] = { "/dev/ttySPI0", "RIL-CMD", NULL},
};
int service_channel_map[]={
[SERVICE_CC] = CHANNEL_CMD00,
[SERVICE_DEV] = CHANNEL_CMD00,
[SERVICE_MM] = CHANNEL_CMD00,
[SERVICE_MSG] = CHANNEL_CMD00,
[SERVICE_PS] = CHANNEL_CMD00,
[SERVICE_SIM] = CHANNEL_CMD00,
[SERVICE_SS] = CHANNEL_CMD00,
[SERVICE_UNSOL] = CHANNEL_CMD00,
};
#endif
struct psd_channel_decription ppp_channel =
{
	CHANNEL_PSD01,
	-1,
	"/dev/mux11",
	"/var/lock/LCK..mux11"
};
struct psd_channel_decription wukong_pdp_channel[] = {
	{ CHANNEL_PSD02, -1, "/dev/mux12",	"/var/lock/LCK..mux12"}, //never used,reserved fro modem
	{ CHANNEL_PSD03, -1, "/dev/mux13",	"/var/lock/LCK..mux13"}, //cid 1
	{ CHANNEL_PSD04, -1, "/dev/mux14",	"/var/lock/LCK..mux14"}, //cid 2
	{ CHANNEL_PSD05, -1, "/dev/mux15",	"/var/lock/LCK..mux15"}, //cid 3
	{ CHANNEL_PSD06, -1, "/dev/mux16",	"/var/lock/LCK..mux16"}, //cid 4
	{ CHANNEL_PSD07, -1, "/dev/mux17",	"/var/lock/LCK..mux17"}, //cid 5
	{ CHANNEL_PSD08, -1, "/dev/mux18",	"/var/lock/LCK..mux18"}, //cid 6
	{ CHANNEL_PSD09, -1, "/dev/mux19",	"/var/lock/LCK..mux19"}, //cid 7
	{ CHANNEL_PSD10, -1, "/dev/mux20",	"/var/lock/LCK..mux20"}, //cid 8
};
#endif


#ifdef DKB_CP //FOR CP
int modem_mode = NORMAL_MODE;
Modem_Type modemType = CP;
#elif defined(DKB_WUKONG) //FOR WUKONG
int modem_mode = MUX_MODE;
Modem_Type modemType = WUKONG;
#elif defined BROWNSTONE_CP //FOR BROWNSTONE
int modem_mode = NORMAL_MODE;
Modem_Type modemType = BROWNSTONE;
#endif

int get_channel_number(void)
{
	return (sizeof(descriptions)/sizeof(descriptions[0]));
}

static void createWorkQueues()
{
	int i;
	int n = get_channel_number();
	for (i = 0; i < n; i++) {
		if (descriptions[i].workQueueName)
			descriptions[i].workQueue = createWorkQueue(descriptions[i].workQueueName, i, 3* 60 * 1000);
	}
}

struct WorkQueue* getWorkQueue(int service)
{
	assert(service <= SERVICE_SS);
	return descriptions[service_channel_map[service]].workQueue;
}


void setThreadContext(ThreadContext* context)
{
	pthread_setspecific(sKey, context);
}
ThreadContext* getThreadContext()
{
	return (ThreadContext*)pthread_getspecific(sKey);
}

const char* getLogTag()
{
	ThreadContext* context = getThreadContext();
	return context ? context->threadname : "RIL";
}

/*************************************************************
 *  Lib called by ril-xxx.c
 **************************************************************/


void ril_handle_cmd_default_response(const char* cmd, RIL_Token token)
{
	ATResponse *response = NULL;

	int err = at_send_command(cmd, &response);
	if (err < 0 || response->success == 0)
		goto error;

	RIL_onRequestComplete(token, RIL_E_SUCCESS, NULL, 0);
	goto exit;
error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);

}

void ril_handle_cmd_sms_one_int(const char* cmd, const char* sms_pdu, const char* prefix, RIL_Token token)
{
	ATResponse *response = NULL;
	int result, err;
	char *line;

	err = at_send_command_sms(cmd, sms_pdu, prefix, &response);
	if (err < 0 || response->success == 0 || !response->p_intermediates)
		goto error;

	line = response->p_intermediates->line;

	err = at_tok_start(&line);
	if (err < 0) goto error;

	err = at_tok_nextint(&line, &result);
	if (err < 0) goto error;

	RIL_onRequestComplete(token, RIL_E_SUCCESS, &result, sizeof(result));
	return;

	goto exit;
error:
	RIL_onRequestComplete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
	at_response_free(response);

}

RIL_RadioState getRadioState()
{
	return sState;
}

void setRadioState(RIL_RadioState newState)
{
	RIL_RadioState oldState;

	pthread_mutex_lock(&s_state_mutex);

	oldState = sState;

	if (s_closed > 0)
	{
		/* If we're closed, the only reasonable state is RADIO_STATE_UNAVAILABLE
		 * This is here because things on the main thread may attempt to change the radio state
		 * after the closed event happened in another thread
		 */
		newState = RADIO_STATE_UNAVAILABLE;
		pthread_cond_broadcast(&s_state_cond);
	}

	sState = newState;

	pthread_mutex_unlock(&s_state_mutex);

	LOGI("setRadioState: sState=%d,oldState=%d", sState, oldState);

	/* Do these outside of the mutex */
	/* [Jerry] Send unsol msg to upper layer only when RILD is connected.
	 *  When RILD is connected, onNewCommandConnect() in RIL.cpp will be called, which calls RIL_onUnsolicitedResponse()
	 *  then onCurrentState() will be called, and sFirstGet will be changed
	 */
	if (!sFirstGet)
	{
		if (sState != oldState)
		{
			s_newrilconnection = 0;
			RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, NULL, 0);
			s_newrilconnection = 1;
		}
	}
}


/*************************************************************
 * Power on process related
 **************************************************************/

/* Update latest correct state to upper layer, called by onCurrentState() */
static void reportLatestRadioState(void *param)
{
	if (sState != RADIO_STATE_OFF)
	{
		s_newrilconnection = 0;
		RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, NULL, 0);
		s_newrilconnection = 1;
	}
}

/**
 * Synchronous call from the RIL to us to return current radio state.
 * RADIO_STATE_UNAVAILABLE should be the initial state.
 */
static RIL_RadioState onCurrentState()
{
	RIL_RadioState state;
	const struct timeval TIMEVAL_DELAY_1s = { 1, 0 };

	/*
	 * We set init state as RADIO_OFF. If we set real state other that RADIO_OFF, upper layer will send AT+CFUN=0 to reset radio.
	 * Sometimes the Android applicatoin will crash and Andriod application will restart and the ril connection between rild and
	 * android application will be reestablished. onNewCommandConnect() -> RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, NULL ,0)
	 * ->onCurrentState() will be called.
	 * So we use one trick flag s_newrilconnection to identify it
	 */

	if(s_newrilconnection) sFirstGet = 1;

	if (sFirstGet)
	{
		state = RADIO_STATE_OFF;
#ifdef BROWNSTONE_CP
		char  value[PROPERTY_VALUE_MAX];
		property_get("ril.have.internal.modem", value, "0");
		if (atoi(value) == 0) state = RADIO_STATE_UNAVAILABLE;
#endif
		sFirstGet = 0;
		LOGI("%s: Return init RADIO_STATE_OFF and call reportLatestRadioState()", __FUNCTION__);

		/* The func will be called when rild is connected, refer to RIL_onUnsolicitedResponse() in RIL.cpp */
		if (sState != RADIO_STATE_OFF)
		{
			/* If RILD has been connected and RADIO_OFF has been sent to upper layer for first access,
			      we need to update latest correct state to upper layer now. To avoid potential blocking of messages
			      between RILJ/RILC, latest radio state is reported sometime later */
			RIL_requestTimedCallback(reportLatestRadioState, NULL, &TIMEVAL_DELAY_1s);

			/* Enable reporting network state to upper layer because RIL has finished initialization and rild is connected */
			setNetworkStateReportOption(1);
		}
	}
	else
	{
		state = sState;
	}

	LOGI("%s: Fetched current state=%d", __FUNCTION__, state);
	return state;


}
/**
 * Call from RIL to us to find out whether a specific request code
 * is supported by this implementation.
 *
 * Return 1 for "supported" and 0 for "unsupported"
 */

static int onSupports (int requestCode)
{
	//@@@ todo
	return 1;
}

static void onCancel (RIL_Token token)
{
	//@@@todo
	LOGI("onCancel is called, but not implemented yet\n");
}

static const char * onGetVersion(void)
{
	return "Marvell Ril 1.0";
}

static void initializeCallback_dev(void* arg)
{
	at_send_command("AT", NULL);
	if(AfterCPReset) {
		AfterCPReset = 0;
		at_send_command("AT+CFUN=1", NULL);
	}

	/* If radio is on, get correct SIM state */
	if (isRadioOn() > 0)
	{
		updateRadioState();
	}

}

static void initializeCallback_mm(void* arg)
{
	ATResponse *p_response = NULL;
	int err;

	at_send_command("AT", NULL);
	err = at_send_command("AT+CREG=2", &p_response);
	/* some handsets -- in tethered mode -- don't support CREG=2 */
	if (err < 0 || p_response->success == 0)
	{
		at_send_command("AT+CREG=1", NULL);
	}
	at_response_free(p_response);
	p_response = NULL;

	/*  GPRS registration events */
	//err = at_send_command_sync(CHANNEL_ID(MM), "AT+CGREG=2", NO_RESULT, NULL, NULL, &p_response);
	err = at_send_command("AT+CGREG=2", &p_response);
	if (err < 0 || p_response->success == 0)
	{
		at_send_command("AT+CGREG=1", NULL);
	}
	at_response_free(p_response);

	/*  Enable network mode indication msg */
	at_send_command("AT+CIND=1", NULL);

	/*  Enable CSD mode  */
	at_send_command("AT+CBST=134,1,0", NULL);
}

static void initializeCallback_cc(void* arg)
{
	at_send_command("AT", NULL);
	/*  No auto-answer */
	at_send_command("ATS0=0", NULL);
	/*  Disable +CCCM ind msg */
	at_send_command("AT+CAOC=1", NULL);
}

static void initializeCallback_msg(void* arg)
{
	at_send_command("AT", NULL);
	/*  SMS PDU mode */
	at_send_command("AT+CMGF=0", NULL);
	/*  MT msg saved in ME, enable the setting if SIM card already initialized */
	at_send_command("AT+CNMI=1,2,2,1,1",  NULL);
}

static void initializeCallback_ss(void* arg)
{
	at_send_command("AT", NULL);
	/*  Call Waiting notifications */
	at_send_command("AT+CCWA=1", NULL);

	/*  +CSSU unsolicited supp service notifications */
	at_send_command("AT+CSSN=1,1", NULL);

	/*  no connected line identification */
	at_send_command("AT+COLP=0", NULL);

	/*  USSD unsolicited */
	at_send_command("AT+CUSD=1", NULL);
}

/**
 * Initialize everything that can be configured while we're still in
 * AT+CFUN=0
 */
 void enque_initializeCallback()
{
	enque(getWorkQueue(SERVICE_DEV), initializeCallback_dev, NULL);
	enque(getWorkQueue(SERVICE_MM), initializeCallback_mm, NULL);
	enque(getWorkQueue(SERVICE_CC), initializeCallback_cc, NULL);
	enque(getWorkQueue(SERVICE_MSG), initializeCallback_msg, NULL);
	enque(getWorkQueue(SERVICE_SS), initializeCallback_ss, NULL);
}

static void waitForClose()
{
	pthread_mutex_lock(&s_state_mutex);

	while (s_closed == 0)
	{
		pthread_cond_wait(&s_state_cond, &s_state_mutex);
	}

	pthread_mutex_unlock(&s_state_mutex);
}

/* Called on command or reader thread */
static void onATReaderClosed()
{
	disableAllInterfaces();

	// readerLoop has close all channels, just need to wake up mainLoop
	s_closed = 1;

	setRadioState(RADIO_STATE_UNAVAILABLE);
}

/* Called on command thread */
static void onATTimeout(int channelID)
{
	// just close the timeout channel, readerLoop would detect bad channel then close all channels and exit.
	at_channel_close(channelID);
}

static void usage(char *s)
{
#ifdef RIL_SHLIB
	fprintf(stderr, "reference-ril requires: -p <tcp port> or -d /dev/tty_device\n");
#else
	fprintf(stderr, "usage: %s [-p <tcp port>] [-d /dev/tty_device]\n", s);
	exit(-1);
#endif
}

#define SPI_NAME "/dev/ttySPI0"
static int spittyfd = -1;
static int enable_mux(char * dev_name)
{
	int fd;
	int displ_num = 7;
	char writeBuf[1024];
	int writeBytes;
	char readBuf[2048];
	int readBytes;
	int ret;

	fd = open(SPI_NAME, O_RDWR);
	if(fd < 0 )
	{
		LOGE("Error openning device: %s,%s:%d\n", strerror(errno),dev_name,fd);
		return 0;
	}
	sprintf(writeBuf,"%s","AT+CMUX=0\r\n");
	writeBytes = write(fd, writeBuf,strlen(writeBuf));

	if(writeBytes < 0)
			LOGE("Error write device: %s,%s:%d\n", strerror(errno),dev_name,fd);
	LOGI("Write %d bytes:%s !!\n", writeBytes,writeBuf);
	readBytes = read(fd, readBuf,512*1);
	if(readBytes < 0)
	{
		printf("Error write device: %s,%s:%d\n", strerror(errno),dev_name,fd);
		return -1;
	}
	if(strstr(readBuf,"OK")!= NULL)
	{
		LOGI("Changed into MUX mode\n");
		ret=ioctl(fd, TIOCSETD, &displ_num);
		modem_mode = MUX_MODE;
		spittyfd = fd;
		ret = 0;
	}else
	{
		LOGI("Changed into Normal mode\n");
		modem_mode = NORMAL_MODE;
		spittyfd = fd;
		close(fd);
		ret = 0;
	}
	return ret;
}
static void *mainLoop(void *param)
{
	pthread_setname_np(pthread_self(), "RIL-mainLoop");
	ThreadContext context = {"RIL-mainLoop", -1, 0};
	setThreadContext(&context);

	LOGD("entering mainLoop()");
	at_set_on_reader_closed(onATReaderClosed);
	at_set_on_timeout(onATTimeout);

	for (;; )
	{
		int i, n = get_channel_number();
		for (i = 0; i < n; i++)
		{
			//open TTY device, and attach it to channel
			int fd;
			struct termios newtio; //place for old and new port settings for serial port
			memset(&newtio, 0, sizeof(newtio));
			while ((fd = open(descriptions[i].ttyName, O_RDWR)) < 0) {
#ifdef BROWNSTONE_CP
				property_set("ril.have.internal.modem", "0");
#endif
				LOGE("AT channel [%d] open error, ttyName:%s, try again", i, descriptions[i].ttyName );
				sleep(1);
			}

#ifdef BROWNSTONE_CP
			property_set("ril.have.internal.modem", "1");
#endif
			//descriptions[i].fd = fd;
			LOGI("AT channel [%d] open successfully, ttyName:%s", i, descriptions[i].ttyName );


#if defined(DKB_CP) || defined(DKB_WUKONG)
			fcntl(fd, F_SETFL, 0);
			newtio.c_cflag = BAUD | CRTSCTS | DATABITS | STOPBITS | PARITYON | PARITY | CLOCAL | CREAD;
			newtio.c_iflag = IGNPAR;
			newtio.c_oflag = 0;
			newtio.c_lflag =  0;    /* disable ECHO, ICANON, etc... */

			newtio.c_cc[VINTR]    = 0;      /* Ctrl-c */
			newtio.c_cc[VQUIT]    = 0;      /* Ctrl-\ */
			newtio.c_cc[VERASE]   = 0x8;      /* del */
			newtio.c_cc[VKILL]    = 0;      /* @ */
			newtio.c_cc[VEOF]     = 4;      /* Ctrl-d */
			newtio.c_cc[VTIME]    = 0;      /* inter-character timer unused */
			newtio.c_cc[VMIN]     = 1;      /* blocking read until 1 character arrives */
			newtio.c_cc[VSWTC]    = 0;      /* '\0' */
			newtio.c_cc[VSTART]   = 0;      /* Ctrl-q */
			newtio.c_cc[VSTOP]    = 0;      /* Ctrl-s */
			newtio.c_cc[VSUSP]    = 0;      /* Ctrl-z */
			newtio.c_cc[VEOL]     = 0xD;      /* '\0' */
			newtio.c_cc[VREPRINT] = 0;      /* Ctrl-r */
			newtio.c_cc[VDISCARD] = 0;      /* Ctrl-u */
			newtio.c_cc[VWERASE]  = 0;      /* Ctrl-w */
			newtio.c_cc[VLNEXT]   = 0;      /* Ctrl-v */
			newtio.c_cc[VEOL2]    = 0;      /* '\0' */
#elif defined BROWNSTONE_CP
			tcgetattr(fd, &newtio);
			newtio.c_lflag = 0;
#endif
			tcflush(fd, TCIFLUSH);
			tcsetattr(fd, TCSANOW, &newtio);


			at_channel_open(/*descriptions[i].channelID*/ i, fd);
		}

		s_closed = 0;

		int ret = at_channel_init(onUnsolicited);
		if (ret < 0)
		{
			LOGE("AT init error %d \n", ret);
			return 0;
		}
		/* Init radio state to RADIO OFF  */
		setRadioState(RADIO_STATE_OFF);
		enque_initializeCallback();

		// Give initializeCallback a chance to dispatched, since
		// we don't presently have a cancellation mechanism
		sleep(1);

		waitForClose();

		LOGI("Re-opening after close");
	}
}

void * handle_eeh(char * value, int len,void * data)
{
/*	if(strcmp("ASSERT", value) == 0)
	{
		onReaderClosed();
		LOGI("Colse SPI TTY!\n");
		close(spittyfd);
	}
	exit(0);*/
	return NULL;
}

#ifdef RIL_SHLIB

pthread_t s_tid_mainloop;

const RIL_RadioFunctions *RIL_Init(const struct RIL_Env *env, int argc, char **argv)
{
	int ret;
	int fd = -1;
	int opt;
	pthread_attr_t attr;

	LOGI("Current CP: dkb version:%d",modemType);

	s_rilenv = env;

/* [Jerry] the arg "-d /dev/ttyS0" defined in /<nfsroot>/system/build.prop is not used now.
 *            dev name is defined in descriptions[i].ttyName, and opened in mainLoop()
 */
#if 0
	while ( -1 != (opt = getopt(argc, argv, "p:d:s:")))
	{
		switch (opt)
		{
		case 'p':
			s_port = atoi(optarg);
			if (s_port == 0)
			{
				usage(argv[0]);
				return NULL;
			}
			LOGI("Opening loopback port %d\n", s_port);
			break;

		case 'd':
			s_device_path = optarg;
			LOGI("Opening tty device %s\n", s_device_path);
			break;

		case 's':
			s_device_path   = optarg;
			s_device_socket = 1;
			LOGI("Opening socket %s\n", s_device_path);
			break;

		default:
			usage(argv[0]);
			return NULL;
		}
	}

	if (s_port < 0 && s_device_path == NULL)
	{
		usage(argv[0]);
		return NULL;
	}
#endif

#if 0
	switch(modemType)
	{
	case CP:
		descriptions = cp_descriptions;
		service_channel_map = cp_channel_map;
		break;
	case WUKONG:
	{
		switch(modem_mode)
		{
		case NORMAL_MODE:
			descriptions = wukong_spi_descriptions;
			service_channel_map = wukong_spi_channel_map;
			break;
		case MUX_MODE:
			descriptions = wukong_mux_descriptions;
			service_channel_map = wukong_mux_channel_map;
			break;
		}
		break;
	}
#ifdef BROWNSTONE_CP
	case BROWNSTONE:
		descriptions = brownstone_descriptions;
		service_channel_map = brownstone_channel_map;
		break;
#endif
	}
#endif
	init_all_channel_struct();
	pthread_key_create(&sKey, NULL);
	createWorkQueues();

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	ret = pthread_create(&s_tid_mainloop, &attr, mainLoop, NULL);

	return &s_callbacks;
}
#else /* RIL_SHLIB */
int main (int argc, char **argv)
{
	int ret;
	int fd = -1;
	int opt;

	while ( -1 != (opt = getopt(argc, argv, "p:d:")))
	{
		switch (opt)
		{
		case 'p':
			s_port = atoi(optarg);
			if (s_port == 0)
			{
				usage(argv[0]);
			}
			LOGI("Opening loopback port %d\n", s_port);
			break;

		case 'd':
			s_device_path = optarg;
			LOGI("Opening tty device %s\n", s_device_path);
			break;

		case 's':
			s_device_path   = optarg;
			s_device_socket = 1;
			LOGI("Opening socket %s\n", s_device_path);
			break;

		default:
			usage(argv[0]);
		}
	}

	if (s_port < 0 && s_device_path == NULL)
	{
		usage(argv[0]);
	}

	RIL_register(&s_callbacks);

	mainLoop(NULL);

	return 0;
}

#endif /* RIL_SHLIB */

/*************************************************************
 * Entrance of service related code, wiil call func in each ril-xxx.c
 **************************************************************/
static ServiceType judgeServiceOnRequest(int request)
{
	ServiceType service = SERVICE_NULL;

	switch (request)
	{
	case RIL_REQUEST_GET_SIM_STATUS:
	case RIL_REQUEST_SIM_IO:
	case RIL_REQUEST_ENTER_SIM_PIN:
	case RIL_REQUEST_ENTER_SIM_PUK:
	case RIL_REQUEST_ENTER_SIM_PIN2:
	case RIL_REQUEST_ENTER_SIM_PUK2:
	case RIL_REQUEST_CHANGE_SIM_PIN:
	case RIL_REQUEST_CHANGE_SIM_PIN2:
	case RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION:
	case RIL_REQUEST_GET_IMSI:
	case RIL_REQUEST_STK_GET_PROFILE:
	case RIL_REQUEST_STK_SET_PROFILE:
	case RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND:
	case RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE:
	case RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM:
	case RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING:
	case RIL_REQUEST_SET_FACILITY_LOCK:
#ifdef MARVELL_EXTENDED
	case RIL_REQUEST_LOCK_INFO:
#endif
		service = SERVICE_SIM;
		break;

	case RIL_REQUEST_SIGNAL_STRENGTH:
	case RIL_REQUEST_REGISTRATION_STATE:
	case RIL_REQUEST_GPRS_REGISTRATION_STATE:
	case RIL_REQUEST_OPERATOR:
	case RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE:
	case RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC:
	case RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL:
	case RIL_REQUEST_QUERY_AVAILABLE_NETWORKS:
	case RIL_REQUEST_SCREEN_STATE:
	case RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE:
	case RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE:
	case RIL_REQUEST_GET_NEIGHBORING_CELL_IDS:
	case RIL_REQUEST_SET_LOCATION_UPDATES:
#ifdef MARVELL_EXTENDED
	case RIL_REQUEST_SELECT_BAND:
	case RIL_REQUEST_GET_BAND:
	case RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL_EXT:
#endif
		service = SERVICE_MM;
		break;

	case RIL_REQUEST_GET_CURRENT_CALLS:
	case RIL_REQUEST_DIAL:
	case RIL_REQUEST_HANGUP:
	case RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND:
	case RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND:
	case RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE:
	case RIL_REQUEST_CONFERENCE:
	case RIL_REQUEST_UDUB:
	case RIL_REQUEST_LAST_CALL_FAIL_CAUSE:
	case RIL_REQUEST_DTMF:
	case RIL_REQUEST_ANSWER:
	case RIL_REQUEST_DTMF_START:
	case RIL_REQUEST_DTMF_STOP:
	case RIL_REQUEST_SEPARATE_CONNECTION:
	case RIL_REQUEST_EXPLICIT_CALL_TRANSFER:
	case RIL_REQUEST_SET_MUTE:
	case RIL_REQUEST_GET_MUTE:
#ifdef MARVELL_EXTENDED
	case RIL_REQUEST_DIAL_VT:
	case RIL_REQUEST_HANGUP_VT:
	case RIL_REQUEST_SET_ACM:
	case RIL_REQUEST_GET_ACM:
	case RIL_REQUEST_SET_AMM:
	case RIL_REQUEST_GET_AMM:
	case RIL_REQUEST_SET_CPUC:
	case RIL_REQUEST_GET_CPUC:
#endif
		service = SERVICE_CC;
		break;

	case RIL_REQUEST_RADIO_POWER:
	case RIL_REQUEST_GET_IMEI:
	case RIL_REQUEST_GET_IMEISV:
	case RIL_REQUEST_BASEBAND_VERSION:
	case RIL_REQUEST_RESET_RADIO:
	case RIL_REQUEST_OEM_HOOK_RAW:
	case RIL_REQUEST_OEM_HOOK_STRINGS:
	case RIL_REQUEST_SET_BAND_MODE:
	case RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE:
	case RIL_REQUEST_DEVICE_IDENTITY:
	case RIL_REQUEST_DEACTIVATE_DATA_CALL:
		service = SERVICE_DEV;
		break;

	case RIL_REQUEST_SEND_SMS:
	case RIL_REQUEST_SEND_SMS_EXPECT_MORE:
	case RIL_REQUEST_SMS_ACKNOWLEDGE:
	case RIL_REQUEST_WRITE_SMS_TO_SIM:
	case RIL_REQUEST_DELETE_SMS_ON_SIM:
	case RIL_REQUEST_GET_SMSC_ADDRESS:
	case RIL_REQUEST_SET_SMSC_ADDRESS:
	case RIL_REQUEST_REPORT_SMS_MEMORY_STATUS:
	case RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG:
	case RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG:
	case RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION:
		service = SERVICE_MSG;
		break;

	case RIL_REQUEST_SETUP_DATA_CALL:
	case RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE:
	case RIL_REQUEST_DATA_CALL_LIST:
#ifdef MARVELL_EXTENDED
	case RIL_REQUEST_FAST_DORMANCY:
#endif
		service = SERVICE_PS;
		break;

	case RIL_REQUEST_SEND_USSD:
	case RIL_REQUEST_CANCEL_USSD:
	case RIL_REQUEST_GET_CLIR:
	case RIL_REQUEST_SET_CLIR:
	case RIL_REQUEST_QUERY_CALL_FORWARD_STATUS:
	case RIL_REQUEST_SET_CALL_FORWARD:
	case RIL_REQUEST_QUERY_CALL_WAITING:
	case RIL_REQUEST_SET_CALL_WAITING:
	case RIL_REQUEST_QUERY_FACILITY_LOCK:
	case RIL_REQUEST_CHANGE_BARRING_PASSWORD:
	case RIL_REQUEST_QUERY_CLIP:
#ifdef MARVELL_EXTENDED
	case RIL_REQUEST_SET_CLIP:
	case RIL_REQUEST_QUERY_COLP:
	case RIL_REQUEST_SET_COLP:
	case RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION:
	case RIL_REQUEST_GET_CNAP:
	case RIL_REQUEST_SET_CNAP:
	case RIL_REQUEST_QUERY_COLR:
	case RIL_REQUEST_SET_COLR:
#endif
		service = SERVICE_SS;
		break;

	default:
		LOGW("invalid request:%d\n", request);
	}

	return service;
}

static ServiceType judgeServiceOnUnsolicited(const char *s)
{
	int index, total, service;

	struct {
		char *prefix;
		ServiceType service;
	} matchTable[] = {
		{ "+CSQ:",	SERVICE_MM	       },
		{ "+CREG:",	SERVICE_MM	       },
		{ "+CGREG:",	SERVICE_MM	       },
		{ "+EEMGINFONC:", SERVICE_MM    },
		{ "+EEMUMTSINTER:", SERVICE_MM    },
		{ "+EEMUMTSINTRA:", SERVICE_MM    },
		{ "+EEMUMTSINTERRAT:", SERVICE_MM	  },
		{ "+EEMUMTSSVC:", SERVICE_MM	  },
		{ "+EEMGINFOBASIC", SERVICE_MM},
		{ "+EEMGINFOSVC", SERVICE_MM},
		{ "+EEMGINFOPS", SERVICE_MM},
		{ "+EEMGINBFTM", SERVICE_MM},
		{ "+NITZ:",	SERVICE_MM	       },
		{ "+MSRI:",	SERVICE_MM		},
		{ "*COPN:",	SERVICE_MM		},
		{ "+CRING:",	SERVICE_CC	       },
		{ "RING",	SERVICE_CC	       },
		{ "NO CARRIER", SERVICE_CC	       },
		{ "CONNECT", SERVICE_CC		   },
		{ "+CCWA:",	SERVICE_CC	       },
		{ "+CCCM:",	SERVICE_CC	       },
		{ "+CLCC:",	SERVICE_CC	       },
		{ "+CLIP:",	SERVICE_CC	       },
		{ "+CNAP:",	SERVICE_CC	       },
		{ "+CGEV:",	SERVICE_PS	       },
		{ "+CMT:",	SERVICE_MSG	       },
		{ "+CMTI:",	SERVICE_MSG	       },
		{ "+CDS:",	SERVICE_MSG	       },
		{ "+MMSG:",	SERVICE_MSG	       },
		{ "+CBM:",      SERVICE_MSG            },
		{ "+CPIN:",	SERVICE_SIM	       },
		{ "+MPBK:",	SERVICE_SIM	       },
		{ "+MSTK:",	SERVICE_SIM	       },
		{ "*EUICC:",	SERVICE_SIM		},
		{ "+REFRESH:",	SERVICE_SIM		},
		{ "+CSSI:",	SERVICE_SS	       },
		{ "+CSSU:",	SERVICE_SS	       },
		{ "+CUSD:",	SERVICE_SS	       },
	};

	total  = sizeof(matchTable) / sizeof(matchTable[0]);
	for (index = 0; index < total; index++)
	{
		if (strStartsWith(s, matchTable[index].prefix))
		{
			service = matchTable[index].service;
			return service;
		}
	}

	return SERVICE_NULL;
}

// called from work queue process
// return value: 1 continue, 0 not continue
int needContinueProcess(int request, void *data, size_t datalen, RIL_Token token)
{
	if(AfterCPReset)
	{
		//if CP assert, should regard POWER ON as radio unavailable, POWER OFF as RADIO_OFF
		if(request ==  RIL_REQUEST_RADIO_POWER)
		{
			int onOff;
			assert(datalen >= sizeof(int *));
			onOff = ((int *)data)[0];

			if ((onOff == 0) && (sState != RADIO_STATE_OFF))
				setRadioState(RADIO_STATE_OFF);
			else if((onOff > 0) && (sState == RADIO_STATE_OFF))
				setRadioState(RADIO_STATE_UNAVAILABLE);

			RIL_onRequestComplete(token, RIL_E_SUCCESS, NULL, 0);

			return 0;
		}
	}


	/* RIL_REQUEST_SCREEN_STATE is supported in any radio state */
	if (request == RIL_REQUEST_SCREEN_STATE)
	{
		//onRequest_mm(request, data, datalen, token);
		return 1;
	}

	/* RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING is supported in any radio state */
	if (request == RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING)
	{
		// onRequest_sim(request, data, datalen, token);
		return 1;
	}

	/* Handle RIL request when radio state is UNAVAILABLE */
	if (sState == RADIO_STATE_UNAVAILABLE)
	{
		if (request == RIL_REQUEST_GET_SIM_STATUS)
		{
			// SIM NOT READY
			RIL_AppStatus app_status = { RIL_APPTYPE_SIM, RIL_APPSTATE_DETECTED,
				RIL_PERSOSUBSTATE_UNKNOWN, NULL, NULL, 0, RIL_PINSTATE_UNKNOWN, RIL_PINSTATE_UNKNOWN };

			RIL_CardStatus *p_card_status;

			/* Allocate and initialize base card status. */
			p_card_status = malloc(sizeof(RIL_CardStatus));
			p_card_status->card_state = RIL_CARDSTATE_PRESENT;
			p_card_status->universal_pin_state = RIL_PINSTATE_UNKNOWN;
			p_card_status->gsm_umts_subscription_app_index = 0;
			p_card_status->cdma_subscription_app_index = RIL_CARD_MAX_APPS;
			p_card_status->num_applications = 1;
			p_card_status->applications[0] = app_status;

			RIL_onRequestComplete(token, RIL_E_SUCCESS,
				(char*)p_card_status, sizeof(RIL_CardStatus));

			free(p_card_status);
			return 0;
		}

		else
		{
			RIL_onRequestComplete(token, RIL_E_RADIO_NOT_AVAILABLE, NULL, 0);
			return 0;
		}
	}

	/* Handle RIL request when radio state is OFF */
	else if (sState == RADIO_STATE_OFF)
	{
		if (request == RIL_REQUEST_GET_SIM_STATUS)
		{
			// SIM NOT READY
			RIL_AppStatus app_status = { RIL_APPTYPE_SIM, RIL_APPSTATE_DETECTED,
				RIL_PERSOSUBSTATE_UNKNOWN, NULL, NULL, 0, RIL_PINSTATE_UNKNOWN, RIL_PINSTATE_UNKNOWN };

			RIL_CardStatus *p_card_status;

			/* Allocate and initialize base card status. */
			p_card_status = malloc(sizeof(RIL_CardStatus));
			p_card_status->card_state = RIL_CARDSTATE_PRESENT;
			p_card_status->universal_pin_state = RIL_PINSTATE_UNKNOWN;
			p_card_status->gsm_umts_subscription_app_index = 0;
			p_card_status->cdma_subscription_app_index = RIL_CARD_MAX_APPS;
			p_card_status->num_applications = 1;
			p_card_status->applications[0] = app_status;

			RIL_onRequestComplete(token, RIL_E_SUCCESS,
				(char*)p_card_status, sizeof(RIL_CardStatus));

			free(p_card_status);
			return 0;
		}

		else if (request ==  RIL_REQUEST_RADIO_POWER)
		{
			//onRequest_dev(request, data, datalen, token);
			return 1;
		}

		else
		{
			RIL_onRequestComplete(token, RIL_E_RADIO_NOT_AVAILABLE, NULL, 0);
			return 0;
		}
	}
	return 1;
}

/**
 * Call from RIL to us to make a RIL_REQUEST
 *
 * Must be completed with a call to RIL_onRequestComplete()
 *
 * RIL_onRequestComplete() may be called from any thread, before or after
 * this function returns.
 *
 * Will always be called from the same thread, so returning here implies
 * that the radio is ready to process another command (whether or not
 * the previous command has completed).
 */

static void onRequest (int request, void *data, size_t datalen, RIL_Token token)
{
	LOGD("%s: %s, token:%p", __FUNCTION__, requestToString(request), token);

	ServiceType service = judgeServiceOnRequest(request);
	switch (service)
	{
	case SERVICE_CC:
		onRequest_cc(request, data, datalen, token);
		break;
	case SERVICE_DEV:
		onRequest_dev(request, data, datalen, token);
		break;
	case SERVICE_MM:
		onRequest_mm(request, data, datalen, token);
		break;
	case SERVICE_MSG:
		onRequest_msg(request, data, datalen, token);
		break;
	case SERVICE_PS:
		onRequest_ps(request, data, datalen, token);
		break;
	case SERVICE_SIM:
		onRequest_sim(request, data, datalen, token);
		break;
	case SERVICE_SS:
		onRequest_ss(request, data, datalen, token);
		break;
	default:
		LOGW("%s: invalid service ID:%d\n", __FUNCTION__, service);
		RIL_onRequestComplete(token, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
		return;
	}
}

/**
 * Called by atchannel when an unsolicited line appears
 * This is called on atchannel's reader thread. AT commands may
 * not be issued here
 */
static void onUnsolicited (const char *s, const char *smsPdu)
{
	LOGI("onUnsolicited:%s", s);
	/* Ignore unsolicited responses until we're initialized.
	 * This is OK because the RIL library will poll for initial state
	 */
	if (sState == RADIO_STATE_UNAVAILABLE)
	{
		if ((!strStartsWith(s, "+CPIN:")|| !AfterCPReset )&&!strStartsWith(s, "+CSQ:99,99"))
			return;
	}

	ServiceType service = judgeServiceOnUnsolicited(s); //one channel for one service
	switch (service)
	{
	case SERVICE_CC:
		onUnsolicited_cc(s);
		break;
	case SERVICE_DEV:
		onUnsolicited_dev(s);
		break;
	case SERVICE_MM:
		onUnsolicited_mm(s);
		break;
	case SERVICE_MSG:
		onUnsolicited_msg(s, smsPdu);
		break;
	case SERVICE_PS:
		onUnsolicited_ps(s);
		break;
	case SERVICE_SIM:
		onUnsolicited_sim(s);
		break;
	case SERVICE_SS:
		onUnsolicited_ss(s);
		break;
	default:
		LOGW("%s: Unexpected service type:%d", __FUNCTION__, service);
	}
}



