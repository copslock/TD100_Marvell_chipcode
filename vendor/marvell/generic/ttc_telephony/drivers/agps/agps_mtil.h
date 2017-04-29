/*------------------------------------------------------------
(C) Copyright [2006-2011] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/********************************************************************
	Filename:	agps_MTIL.h
	Created:	2010/05/15
	
	Purpose:	Constant definitions and function prototypes for
				agps_MTIL library
*********************************************************************/

#ifndef _agps_MTIL_h_
#define _agps_MTIL_h_

/* Header files includes */
#include "LSM_APIs.h"
#include "cpaclient.h"
/* Macro definitions */

typedef void     (*logBrief_func)(LSM_CHAR *pMsg);
typedef LSM_BOOL (*CP_SendCellInfo_func)(LSM_netCellID *pCellInfo, LSM_BOOL cellInfoValid);
typedef LSM_BOOL (*CP_SendSETIDInfo_func)(SETID_Info *pSETidInfo, LSM_BOOL isSETIDInfoValid);
typedef LSM_BOOL (*CP_SendNetMessage_func)(	eLSM_OTA_TYPE ota_type,
											eLSM_RRC_STATE rrc_state,
											eLSM_SESSION_PRIORITY sessionPriority,
											LSM_UINT8* p_msg_data,
											LSM_UINT32 msg_size);
typedef LSM_BOOL (*CP_SendMeasurementTerminate_func)(eLSM_OTA_TYPE otaType);
typedef LSM_BOOL (*CP_SetMessageListeners_func)(LSM_MessageListener listener);
typedef LSM_BOOL (*CP_SendRRCStateEvent_func)(eLSM_RRC_STATE rrc_state);
typedef LSM_BOOL (*CP_NotfiyE911Dialed_func)(tSIRF_VOID);
typedef LSM_BOOL (*CP_Reset_func)(tSIRF_VOID);
typedef LSM_BOOL (*CP_MtlrNotifyLocInd_func)(P_CPA_LCS_LOCATION_IND pCpaLcsLocationInd);

typedef struct CP2LSM_t
{
	CP_SendCellInfo_func 				CP_SendCellInfo;
	CP_SendSETIDInfo_func 				CP_SendSETIDInfo;
	CP_SendNetMessage_func 				CP_SendNetMessage;
	CP_SendMeasurementTerminate_func 	CP_SendMeasurementTerminate;
	CP_SetMessageListeners_func 		CP_SetMessageListeners;
	CP_SendRRCStateEvent_func 			CP_SendRRCStateEvent;
	CP_NotfiyE911Dialed_func 			CP_NotfiyE911Dialed;
	CP_Reset_func 						CP_Reset;
	CP_MtlrNotifyLocInd_func			CP_MtlrNotifyLocInd;
	logBrief_func 						logBrief;
} CP2LSM;

/* Declarations of external functions */
extern void AGPS_TRACE_LOGCAT(const char *fmt, ...);
extern void OM_logBrief(LSM_CHAR *pMsg);

#define UPDATE_LSMEnv \
{ \
	{ \
		extern CP2LSM *CP2LSMGetEnvPtr(void); \
		{ \
			CP2LSM *cp2lsmenv = CP2LSMGetEnvPtr(); \
			cp2lsmenv->CP_SendCellInfo 				= CP_SendCellInfo; \
			cp2lsmenv->CP_SendSETIDInfo 			= CP_SendSETIDInfo; \
			cp2lsmenv->CP_SendNetMessage			= CP_SendNetMessage; \
			cp2lsmenv->CP_SendMeasurementTerminate	= CP_SendMeasurementTerminate; \
			cp2lsmenv->CP_SetMessageListeners		= CP_SetMessageListeners; \
			cp2lsmenv->CP_SendRRCStateEvent			= CP_SendRRCStateEvent; \
			cp2lsmenv->CP_NotfiyE911Dialed			= CP_NotfiyE911Dialed; \
			cp2lsmenv->CP_Reset						= CP_Reset; \
			cp2lsmenv->CP_MtlrNotifyLocInd			= CP_MtlrNotifyLocInd; \
			cp2lsmenv->logBrief						= OM_logBrief; \
		} \
	} \
}

#endif /* _agps_MTIL_h_ */
