/*------------------------------------------------------------
(C) Copyright [2006-2011] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/


/* PLATFORM_SDK_VERSION >= 9 means Android version >=  2.3 */
#if PLATFORM_SDK_VERSION >= 9

#include <hardware/gps.h>
#include <cutils/log.h>
#include <errno.h>
#include "mtelif_lib.h"

/* --Define--Start-- */
#define LOG_TAG		"mrvl_agps"
//#define AGPS_RIL_REQUEST_SETID_IMSI    1
//#define AGPS_RIL_REQUEST_SETID_MSISDN  2
#define AGPS_RIL_REQUEST_CELLID        (1<<3L)
/* --Define--End-- */

#define  GPS_DEBUG  1

#if GPS_DEBUG == 2
#define  D(...)         LOGD(__VA_ARGS__)
#define  D_LOW(...)     LOGD(__VA_ARGS__)

#elif GPS_DEBUG == 1
#define  AGPSLOGD(...)  LOGD(__VA_ARGS__)
#define  D_LOW(...)     ((void)0)

#else
#define  D(...)         ((void)0)
#define  D_LOW(...)     ((void)0)
#endif

AGpsRilCallbacks mrilcallback;
static char rilbuf[100];
static int rilsize = -1;
pthread_t mrilthread = NULL;
volatile int agps_loop = 0;
static volatile int ril_action = -1;

static pthread_mutex_t ril_mutex;
enum {
	IMSI_INDEX,
	CI_INDEX,
	THREAD_INDEX,
	AGPS_RIL_MUTEX_NUM
};
static pthread_mutex_t agps_ril_mutex[AGPS_RIL_MUTEX_NUM];
static pthread_cond_t  agps_ril_cond[AGPS_RIL_MUTEX_NUM];
extern int acc_tech_mode;
extern int isTDNW;


/** Extended interface for AGPS_RIL support. */
void send_agps_ril_request(int action_flags);
int wait_agps_ril_response(int action_flags);

static void agps_ril_request(void *arg)
{
	AGPSLOGD("[%s] thread is created!!", __FUNCTION__);
	while(agps_loop)
	{
		pthread_mutex_lock(&agps_ril_mutex[THREAD_INDEX]);
		pthread_cond_timedwait(&agps_ril_cond[THREAD_INDEX], &agps_ril_mutex[THREAD_INDEX], NULL);
		pthread_mutex_unlock(&agps_ril_mutex[THREAD_INDEX]);
		switch(ril_action)
		{
		case AGPS_RIL_REQUEST_SETID_IMSI:
		case AGPS_RIL_REQUEST_CELLID:
			send_agps_ril_request(ril_action);
			break;
		default:
			AGPSLOGD("Action: %d Not support", ril_action);
			break;
		}
	}
	AGPSLOGD("[%s] thread is exit!!", __FUNCTION__);
}
 /* Opens the AGPS interface and provides the callback routines
 * to the implemenation of this interface.
 */
void  mrvl_ril_init( AGpsRilCallbacks* callbacks)
{
	int i;

	if (callbacks != NULL)
	{
		mrilcallback.request_setid    = callbacks->request_setid;
		mrilcallback.request_refloc   = callbacks->request_refloc;
		mrilcallback.create_thread_cb = callbacks->create_thread_cb;
	} else {
		LOGE("[%s] AGpsRilCallbacks is NULL!", __FUNCTION__);
	}

	if (mrilcallback.create_thread_cb && mrilthread == NULL)
	{
		agps_loop = 1;
		mrilthread = mrilcallback.create_thread_cb("agps_ril_request",
							agps_ril_request, NULL);
	}
	AGPSLOGD("[%s] complete successfully!", __FUNCTION__);
}

/**
 * Sets the reference location.
 */
void mrvl_ril_set_ref_location(const AGpsRefLocation *agps_reflocation, size_t sz_struct)
{
	AGPSLOGD("[%s] called", __FUNCTION__);
	/* initialize rilbuf, rilsize */
	rilsize = -1;
	rilbuf[0] = '\0';

	// In Android framework, sz_struct is always set to sizeof(AGpsRefLocation)
	if (sz_struct)
	{
		memcpy(rilbuf, (char *)agps_reflocation, sz_struct);
		rilsize = sz_struct;
	}

	pthread_mutex_lock(&agps_ril_mutex[CI_INDEX]);
	pthread_cond_signal(&agps_ril_cond[CI_INDEX]);
	pthread_mutex_unlock(&agps_ril_mutex[CI_INDEX]);

}
/**
 * Sets the set ID.
 */
void mrvl_ril_set_set_id(AGpsSetIDType type, const char* setid)
{
	AGPSLOGD("[%s] AGpsSetIDType: %d", __FUNCTION__, type);

	/* initialize rilbuf, rilsize */
	rilsize = -1;
	rilbuf[0] = '\0';

	rilsize = strlen(setid);
	strcpy(rilbuf, setid);

	pthread_mutex_lock(&agps_ril_mutex[IMSI_INDEX]);
	pthread_cond_signal(&agps_ril_cond[IMSI_INDEX]);
	pthread_mutex_unlock(&agps_ril_mutex[IMSI_INDEX]);
}

/**
 * Send network initiated message.
 */
void mrvl_ril_ni_message(uint8_t *msg, size_t len)
{
	// assume Android passes whole SMS pdu to us.
	notifySmsPayload_GB(msg, len);
}

/**
 * Notify GPS of network status changes.
 * These parameters match values in the android.net.NetworkInfo class.
 */
void mrvl_ril_update_network_state(int connected, int type, int roaming, const char* extra_info)
{
	AGPSLOGD("connected: %d, type: %d, roam:%d, %s", connected, type, roaming, extra_info);
	AGPSLOGD("[%s] not implemented!", __FUNCTION__);
}

const AGpsRilInterface mrvlAgpsRil = {
	sizeof(AGpsRilInterface),
	mrvl_ril_init,
	mrvl_ril_set_ref_location,
	mrvl_ril_set_set_id,
	mrvl_ril_ni_message,
	mrvl_ril_update_network_state
};

AGpsRilInterface * get_agpsril_interface(void)
{
	int i;
	AGpsRilInterface * agpsrilif = &mrvlAgpsRil;

	if (pthread_mutex_init(&ril_mutex, NULL) != 0)
	{
		LOGE("[%s] Mutex can't be created!", __FUNCTION__);
		agpsrilif = NULL;
	}
	for (i = 0;i < AGPS_RIL_MUTEX_NUM; ++i)
	{
		if (pthread_mutex_init(&agps_ril_mutex[i],NULL) != 0 ||
			pthread_cond_init(&agps_ril_cond[i],NULL) != 0)
		{
			LOGE("[%s]: pthread mutex or cond init error\n", __FUNCTION__);
			agpsrilif = NULL;
		}
	}

	return agpsrilif;
}

void send_agps_ril_request(int action_flags)
{
	AGPSLOGD("[%s] action_flags:%d", __FUNCTION__, action_flags);

	if (action_flags & AGPS_RIL_REQUEST_SETID_IMSI)
	{
		if (mrilcallback.request_setid)
		{
			mrilcallback.request_setid(action_flags);
		}
		else
		{
			LOGE("request_setid is NULL!!");
		}
	} else if (action_flags & AGPS_RIL_REQUEST_CELLID)
	{
		if (mrilcallback.request_refloc)
		{
			/* Android framework doesn't care about the parameter for  request_refloc() */
			mrilcallback.request_refloc(action_flags);
		}
		else
		{
			LOGE("request_refloc is NULL!!");
		}
	} else
	{
		LOGE("action flag isn't supported");
	}
}

int wait_agps_ril_response(int action_flags)
{
	int rc = -1;
	struct timeval now;
	struct timespec timeout;
	int mutex_index = -1;

	AGPSLOGD("[%s] action_flags: %d", __FUNCTION__, action_flags);

	if (action_flags & AGPS_RIL_REQUEST_SETID_IMSI)
	{
		mutex_index = IMSI_INDEX;
		ril_action = AGPS_RIL_REQUEST_SETID_IMSI;
	} else if (action_flags & AGPS_RIL_REQUEST_CELLID)
	{
		mutex_index = CI_INDEX;
		ril_action = AGPS_RIL_REQUEST_CELLID;
	}

	if (mutex_index < 0)
	{
		LOGE("[%s] index %s not in my mutex list!!", __FUNCTION__, mutex_index);
		return -1;
	}

	pthread_mutex_lock(&agps_ril_mutex[mutex_index]);

	pthread_mutex_lock(&agps_ril_mutex[THREAD_INDEX]);
	AGPSLOGD("ril_action: %d", ril_action);
	pthread_cond_signal(&agps_ril_cond[THREAD_INDEX]);
	pthread_mutex_unlock(&agps_ril_mutex[THREAD_INDEX]);

	gettimeofday(&now, NULL);
	timeout.tv_sec = now.tv_sec + 5; // wait for 5sec
	timeout.tv_nsec = now.tv_usec * 1000;

	rc = pthread_cond_timedwait(&agps_ril_cond[mutex_index], &agps_ril_mutex[mutex_index], &timeout);

	if (rc == ETIMEDOUT)
	{
		LOGE("send_agps_ril_request Time out: 5sec");
		rc = -1;
	}
	pthread_mutex_unlock(&agps_ril_mutex[mutex_index]);

	mutex_index = -1;

	return (rc<0?-1:0);
}
/*******************************************************************************\
*	Function:		GetIMSI
*	Description:	Get the International Mobile Subscriber Identity (IMSI) value
*	Parameters:		
*	Returns:		0=OK, <0=Error Code
\*******************************************************************************/
int GetIMSI(unsigned char* imsi_str)
{
	int ret = -1;

	AGPSLOGD("[%s] is called", __FUNCTION__);

	pthread_mutex_lock(&ril_mutex);

	//send_agps_ril_request(AGPS_RIL_REQUEST_SETID_IMSI);
	wait_agps_ril_response(AGPS_RIL_REQUEST_SETID_IMSI);

	if (rilsize >= 0 && imsi_str)
	{
		strcpy(imsi_str, rilbuf);
		ret = 0;
	}

	pthread_mutex_unlock(&ril_mutex);

	return ret;
}

/*******************************************************************************\
*	Function:		GetCurCellInfo
*	Description:	Get the current Cell Info value
*	Parameters:		
*	Returns:		0=OK, <0=Error Code
\*******************************************************************************/
int GetCurCellInfo(MCellInfo *pInfo)
{
	int ret = -1;

	AGpsRefLocation *agpsloc = NULL;

	AGPSLOGD("[%s] is called", __FUNCTION__);

	pthread_mutex_lock(&ril_mutex);

	//send_agps_ril_request(AGPS_RIL_REQUEST_CELLID);
	wait_agps_ril_response(AGPS_RIL_REQUEST_CELLID);

	if (rilsize == sizeof(AGpsRefLocation))
	{
		agpsloc = (AGpsRefLocation *)rilbuf;

		if (agpsloc && pInfo)
		{
			pInfo->CountryCode = agpsloc->u.cellID.mcc;
			pInfo->NetworkCode = agpsloc->u.cellID.mnc;
			pInfo->lac         = agpsloc->u.cellID.lac;
			pInfo->cellId      = agpsloc->u.cellID.cid;

			if (agpsloc->type == AGPS_REF_LOCATION_TYPE_GSM_CELLID)
			{
				acc_tech_mode = ACT_GSM;
			}
			else
			{
				acc_tech_mode = ACT_UTRAN;
			}

			if (acc_tech_mode == ACT_UTRAN && isTDNW)
			{
				acc_tech_mode = ACT_TDSCDMA;
			}

			AGPSLOGD("mcc:%d mnc:%d lac:%d cid:%d", pInfo->CountryCode,
												pInfo->NetworkCode,
												pInfo->lac,
												pInfo->cellId);

			ret = 0;
		}
	}
#if 0
	if (rilsize == sizeof (uint16_t) + sizeof (AGpsRefLocationCellID))
	{
		agpsCellid = (AGpsRefLocationCellID *)(rilbuf + sizeof (uint16_t));

		if (agpsCellid && pInfo)
		{
			pInfo->CountryCode = agpsCellid->mcc;
			pInfo->NetworkCode = agpsCellid->mnc;
			pInfo->lac         = agpsCellid->lac;
			pInfo->cellId      = agpsCellid->cid;

			AGPSLOGD("mcc:%d mnc:%d lac:%d cid:%d", pInfo->CountryCode,
												pInfo->NetworkCode,
												pInfo->lac,
												pInfo->cellId);

			ret = 0;
		}
	} else if (rilsize == sizeof (uint16_t) + sizeof (AGpsRefLocationMac))
	{
		agpsMac = (AGpsRefLocationMac *)(rilbuf + sizeof (uint16_t));

		AGPSLOGD("Temp, not support");

		if (pInfo)
		{
			memset(pInfo, 0, sizeof(MCellInfo));
		}
	}
#endif
	pthread_mutex_unlock(&ril_mutex);

	return ret;
}

 /* Clean the AGPS interface and provides the callback routines.
 */
void  mrvl_ril_deinit(void)
{
	int i;
	void *ret;

	mrilcallback.request_setid    = NULL;
	mrilcallback.request_refloc   = NULL;
	mrilcallback.create_thread_cb = NULL;

	if (mrilthread)
	{
		agps_loop = 0;
		pthread_mutex_lock(&agps_ril_mutex[THREAD_INDEX]);	
		pthread_cond_signal(&agps_ril_cond[THREAD_INDEX]);
		pthread_mutex_unlock(&agps_ril_mutex[THREAD_INDEX]);

		pthread_join(mrilthread, &ret);

		mrilthread = NULL;
	}
	for (i = 0; i < AGPS_RIL_MUTEX_NUM; ++i)
	{
		if (pthread_mutex_destroy(&agps_ril_mutex[i]) != 0 ||
			pthread_cond_destroy(&agps_ril_cond[i]) != 0)
		{
			AGPSLOGD("%s: ERROR: pthread_mutex_destroy or pthread_cond_destroy \
								failed", __FUNCTION__);
		}
	}

	AGPSLOGD("[%s] complete successfully!", __FUNCTION__);
}
#endif
