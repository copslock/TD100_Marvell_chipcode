/*------------------------------------------------------------
(C) Copyright [2006-2011] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/********************************************************************
	Filename:	CpaClient.c
	Created:	2009/11/11
	
	Purpose:	This file contains the implementation of
				AGPS Control Plane Agent, Client side over MTIL.
*********************************************************************/

// Header files includes
//#include "stdafx.h"
#include <stdio.h>
#include <string.h>

#ifdef WIN32_PC_DEBUG
#include <windows.h>
#else /* WIN32_PC_DEBUG */
#ifdef BIONIC
#include <linux/in.h>
#include <sys/endian.h>
#else /* BIONIC */
#include <netinet/in.h>
#endif /* BIONIC */

#include <sys/select.h>
#include <sys/socket.h>
#endif /* WIN32_PC_DEBUG */

//#include "MtilCpaConfig.h"
//#include "cpaclient.h"
#include <cutils/log.h>
#define LOG_TAG						"mrvl_agps"

#include "agps_mtil.h"
#include "mtelif_lib.h"
// Macro definitions
#define EXTRA_DEBUG_TRACES					// Disable Extra Debug Traces before release

extern CP2LSM CP2LSM_env;

// Declarations of internal functions
static void		CpaClient_ErrorHandler(int ErrorCode, int ErrorParam);
       LSM_BOOL	CpaClient_SendNetworkUplinkReq(eLSM_OTA_TYPE ota_type, LSM_lplOtaParams *p_ota_params, LSM_UINT8* p_msg_data, LSM_UINT32 msg_size);
#ifdef WIN32_PC_DEBUG
static int		CpaClient_SendTestThread(void);
#endif /* WIN32_PC_DEBUG */

// Declaration of global/const variables
static eLSM_OTA_TYPE gOtaType = LSM_INVALID_OTA_TYPE;
static BOOL g_bSendTerminateSession = FALSE;

extern int at_send_command_async(char* ATcmd, char *prefix);

static void l_void(void) {}

CP2LSM CP2LSM_env =
{
	(CP_SendCellInfo_func)				l_void,
	(CP_SendSETIDInfo_func)				l_void,
	(CP_SendNetMessage_func)			l_void,
	(CP_SendMeasurementTerminate_func)	l_void,
	(CP_SetMessageListeners_func)		l_void,
	(CP_SendRRCStateEvent_func)			l_void,
	(CP_NotfiyE911Dialed_func)			l_void,
	(CP_Reset_func)						l_void,
	(CP_MtlrNotifyLocInd_func)			l_void,
	(logBrief_func)						l_void
};


CP2LSM *CP2LSMGetEnvPtr(void)
{
	return &CP2LSM_env;
}

/*******************************************************************************\
*	Function:		CpaClient_ErrorHandler
*	Description:	This function implements the error handler of this AGPS CPA MTIL client
*	Parameters:		See below
*	Returns:		None
\*******************************************************************************/
void CpaClient_ErrorHandler(int ErrorCode,
							int ErrorParam)
{
	LOGE("[%s]: Received error code %d with param %d", __FUNCTION__, ErrorCode, ErrorParam);
}

/******************************************************************************
*\
*	Function:		CpaClient_SendNetworkUplinkReq
*	Description:	This function is called from LSM/LPL (callback) when uplink
*					message is sent to the NW. It fragments the message buffer
*					to segments because of CCI length limitation.
*	Parameters:		See below
*	Returns:		TRUE=Success, FALSE=Fail
\******************************************************************************
*/
#define CPA_PAYLOAD_MAX_SIZE (CI_DEV_MAX_APGS_MSG_SIZE*CI_DEV_MAX_NUM_NWDL_MSG_IND)
LSM_BOOL CpaClient_SendNetworkUplinkReq(eLSM_OTA_TYPE ota_type,				// over-the-air type (RRLP / RRC)
										LSM_lplOtaParams *p_ota_params,		// pointer to params of OTA
										LSM_UINT8* p_msg_data,				// pointer to payload
										LSM_UINT32 msg_size)				// payload length
{
	LSM_BOOL bRet = LSM_TRUE;
	int segment_size, kk, count, i;
	char ATcmd[CI_DEV_MAX_APGS_MSG_SIZE*2+50+2];
	char temp[50];
	CiDevLpBearerType bearer_type;
	int isFinal;

 	LOGD("[%s]: Entry point", __FUNCTION__);

	if (!p_msg_data)	// check pointer to data
	{
		LOGE("[%s]: Error! p_msg_data is NULL", __FUNCTION__);
		bRet = LSM_FALSE;
		goto ExitSendNetworkUplinkMsg;
	}

	if (msg_size > CPA_PAYLOAD_MAX_SIZE)	// check data size (in bytes)
	{
		LOGE("[%s]: Error! msg_size = %d", __FUNCTION__, (int) msg_size);
		bRet = LSM_FALSE;
		goto ExitSendNetworkUplinkMsg;
	}

	// Segmentation of the uplink message
	for (count = 0; count < CI_DEV_MAX_NUM_NWDL_MSG_IND; count++)
	{
		/* Format:
			+LPNWUL="msg_data in hex", msg_data_len, count, bearer_type, ifinal
		*/
		ATcmd[0] = '\0';
		sprintf(ATcmd, "AT+LPNWUL=");

		// calculate the segment size (in bytes). only real payload. padding isn't included.
		kk = (msg_size / CI_DEV_MAX_APGS_MSG_SIZE) - count;

		if (kk == 0) // segment is not full
		{
			segment_size = msg_size % CI_DEV_MAX_APGS_MSG_SIZE;
		}

		else if (kk > 0) // segment is full
		{
			segment_size = CI_DEV_MAX_APGS_MSG_SIZE;
		}

		else // segment is empty
		{
			segment_size = 0;
		}

		/* 1. Get msg_data in hex string format */
		if (segment_size > 0)
		{
			//memcpy(&pLpNwUlReq->msg_data[0], &p_msg_data[MTIL_MAX_APGS_MSG_SIZE * count], MTIL_MAX_APGS_MSG_SIZE);
			for (i = 0; i < segment_size; ++i)
			{
				temp[0] = '\0';
				sprintf(temp, "%02x", p_msg_data[CI_DEV_MAX_APGS_MSG_SIZE * count + i]);
				strcat(ATcmd, temp);
			}
		}

		strcat(ATcmd, ",");

		/* 2. Get msg_data_len */
		temp[0] = '\0';
		sprintf(temp, "%ld,", msg_size);
		strcat(ATcmd, temp);
		//pLpNwUlReq->msg_data_len = msg_size;
		// Note: the size is the total size (in bytes), even if this segment is padding

		/* 3. Get count */
		temp[0] = '\0';
		sprintf(temp, "%d,", count);
		strcat(ATcmd, temp);
		//pLpNwUlReq->count = count;

		// OTA / Bearer Type
		switch (ota_type)
		{
			case LSM_CP_RRLP:
				bearer_type = CI_DEV_LP_RRLP;
				break;

			case LSM_CP_RRC:
				if (LSM_RRC_MEAS_REPORT == p_ota_params->rrc_msg_type)
				{
					bearer_type = CI_DEV_LP_RRC;
				}

				else
				{
					if (p_ota_params->rrc_msg_type == LSM_RRC_MEAS_FAILURE)
					{
						LOGE("[%s]: Warning! This is an UL RRC Measurement Control Failure", __FUNCTION__);
					}

					else
					{
						LOGE("[%s]: Warning! This is an UL RRC Status", __FUNCTION__);
					}
					goto ExitSendNetworkUplinkMsg;
				}
				break;

			default:
				LOGE("[%s]: Error! Illegal value in ota_type = %d", __FUNCTION__, ota_type);
				bRet = LSM_FALSE;
				goto ExitSendNetworkUplinkMsg;
		}

		/* 4. get bearer_type */
		temp[0] = '\0';
		sprintf(temp, "%d,", bearer_type);
		strcat(ATcmd, temp);

		// Final Response
		isFinal = 0;
		if (p_ota_params->is_final_response == LSM_TRUE)
		{
			isFinal = TRUE;

			// in first segment only, close the LSM session
			if (count == 0)
			{
				g_bSendTerminateSession = TRUE;
			}
		}

		/* 5. get bearer_type */
		temp[0] = '\0';
		sprintf(temp, "%d", isFinal);
		strcat(ATcmd, temp);
		LOGD("%s", ATcmd);
		at_send_command_async(ATcmd, "+LPNWUL:");
		usleep(10000);
	}

	//////////////////////////////////////////////////////////////////////////
#ifdef EXTRA_DEBUG_TRACES
	{
		int i;
		unsigned char* pChar;
		char buf[200];
		char num_str[10];

		LOGD("[%s]: sizeof LSM_lplOtaParams = %d, sizeof meas_report = %d, sizeof is_final_response = %d", __FUNCTION__,
			sizeof(LSM_lplOtaParams),
			sizeof(p_ota_params->meas_report),
			sizeof(p_ota_params->is_final_response));

		LOGD("[%s]: rrc_msg_type = %d, measurement_identity = %d, measurement_or_event = %d", __FUNCTION__,
			(int) p_ota_params->rrc_msg_type,
			(int) p_ota_params->meas_report.measurement_identity,
			(int) p_ota_params->meas_report.measurement_or_event);

		pChar = (unsigned char*) p_ota_params;
		sprintf(buf, "[%s]: LSM_lplOtaParams =", __FUNCTION__);
		for(i = 0; i < (int) (sizeof(LSM_lplOtaParams)); i++)
		{
			sprintf(num_str, " 0x%x", pChar[i]);
			strcat(buf, num_str);
		}
		LOGD("%s", buf);
	}
#endif /* EXTRA_DEBUG_TRACES */
	//////////////////////////////////////////////////////////////////////////

	LOGD("[%s]: bearer_type = %d, is_final_response = %d, msg_data_len = %d", __FUNCTION__,
		(int) bearer_type,
		(int) isFinal,
		(int) msg_size);

ExitSendNetworkUplinkMsg:
 	LOGD("[%s]: Exit point. bRet = %d", __FUNCTION__, (int) bRet);
	return (bRet);
}
/******************************************************************************
*\
*	Function:		CpaClient_SendLcsVerificationResp
*	Description:	This function is called by GPS HAL when receiving the user's
*					Verification Response to C-Plane MT-LR Notification.
*	Parameters:		See below
*	Returns:		TRUE=Success, FALSE=Fail
\******************************************************************************
*/
LSM_BOOL CpaClient_SendLcsVerificationResp(unsigned long notifId,				// Notification ID (unique)
										   unsigned long present,				// Indicates if verification response is present
										   unsigned long verificationResponse)	// User's verification response value
{
	LSM_BOOL bRet = LSM_TRUE;
	char ATcmd[50];
	char temp[20];

	LOGD("[%s]: Entry point", __FUNCTION__);

	/*AT+LPLOCVR=taskid, invokeHandle, present, verificationResponse */
	// Taskid = 0
	sprintf(ATcmd, "AT+LPLOCVR=0,%lu,%ld", notifId, present);

	if (present)
	{
		sprintf(temp, ",%ld", verificationResponse);
		strcat(ATcmd, temp);
	}
	//pLpVerResp->invokeHandle = (MTIL_LCS_INVOKE_HANDLE) notifId;
	//pLpVerResp->present = present;
	//pLpVerResp->verificationResponse = verificationResponse;

	LOGD("[%s]: invokeHandle=0x%lx, present=%d, verificationResponse=%d", __FUNCTION__,
		 notifId,
		 present,
		 verificationResponse);

	at_send_command(ATcmd, "+LPLOCVR:", NULL);

//ExitSendLcsVerResp:
	LOGD("[%s]: Exit point. bRet = %d", __FUNCTION__, (int) bRet);
	return (bRet);
}

/*******************************************************************************\
*	Function:		CpaClient_RecvEmergencyNumberDialedInd
*	Description:	Handle Emergency Number Dialed notification. Not supported yet.
*	Parameters:		None
*	Returns:		0=Success, -1=Fail
\*******************************************************************************/
int CpaClient_RecvEmergencyNumberDialedInd(void)
{
	int rc = 0;
	LOGD("[%s]: Entry point", __FUNCTION__);

	// Notify the LSM/LPL
	CP2LSM_env.CP_NotfiyE911Dialed();

	LOGD("[%s]: Exit point. rc = %d", __FUNCTION__, rc);
	return rc;
}

/*******************************************************************************\
*	Function:		CpaClient_RecvNetworkDownlinkInd
*	Description:	This function is called when a NW Downlink message is received
*					from the Server. It assembles the message segments (due to CCI
*					length limitation) and calls the corresponding LSM API function.
*	Parameters:		See below
*	Returns:		0=Success, -1=Fail
\*******************************************************************************/
int CpaClient_RecvNetworkDownlinkInd( UINT8 bearer_type,
									  UINT8 *in_msg_data,
									  UINT32 in_msg_size,
									  UINT8 in_session_type,
									  UINT8 in_rrc_state,
									  UINT count)
{
	eLSM_OTA_TYPE				ota_type;
	eLSM_RRC_STATE				rrc_state;
	eLSM_SESSION_PRIORITY		session_type = NO_EMERGENCY;

	static unsigned int			msg_data_buf[CPA_PAYLOAD_MAX_SIZE / sizeof(unsigned int)];
	char*						msg_data = (char *) msg_data_buf;
	static int					msg_size;
	int							rc = 0;

	LOGD("[%s]: Entry point", __FUNCTION__);

	if (count == 0) // if first segment
	{
		msg_size = 0;
	}

	//msg_size += pLpNwDlInd->msg_data_len;
	msg_size += in_msg_size;

	memcpy(&msg_data[CI_DEV_MAX_APGS_MSG_SIZE * count], in_msg_data, in_msg_size);

	LOGD("[%s]: msg_size (of segment) = %d, count = %d", __FUNCTION__, in_msg_size, count);

	if (count == (CI_DEV_MAX_NUM_NWDL_MSG_IND - 1)) // if last segment
	{
		switch (bearer_type)
		{
			case CI_DEV_LP_RRLP:
				ota_type = LSM_CP_RRLP;
				gOtaType = LSM_CP_RRLP;
				break;

			case CI_DEV_LP_RRC:
				ota_type = LSM_CP_RRC;
				gOtaType = LSM_CP_RRC;
				break;

			default:
				//ASSERT(FALSE);
				LOGE("[%s] invalid OTA TYPE!!", __FUNCTION__);
				gOtaType = LSM_INVALID_OTA_TYPE;
				rc = -1;
				goto ExitNwDlMsg;
				break;
		}

		switch (in_rrc_state)
		{
			case CI_DEV_LP_RRC_STATE_CELL_DCH:
				rrc_state = LSM_RRC_STATE_CELL_DCH;
				break;

			case CI_DEV_LP_RRC_STATE_CELL_FACH:
				rrc_state = LSM_RRC_STATE_CELL_FACH;
				break;

			case CI_DEV_LP_RRC_STATE_CELL_PCH:
				rrc_state = LSM_RRC_STATE_CELL_PCH;
				break;

			case CI_DEV_LP_RRC_STATE_URA_PCH:
				rrc_state = LSM_RRC_STATE_URA_PCH;
				break;

			case CI_DEV_LP_RRC_STATE_IDLE:
				rrc_state = LSM_RRC_STATE_IDLE;
				break;

			default:
				//rrc_state = LSM_RRC_INVALID_STATE;
				//ASSERT(FALSE);
				LOGE("[%s] invalid RRC STATE!!", __FUNCTION__);
				rc = -1;
				goto ExitNwDlMsg;
				break;
		}
 
		if (in_session_type == CI_DEV_LP_EMERGENCY)
		{
			session_type = EMERGENCY;
		}

		LOGD("[%s]: ota_type = %d, session_type = %d, rrcState = %d, msg_size = %d", __FUNCTION__,
				ota_type,
				session_type,
				rrc_state,
				msg_size);

		// print the message payload
/*
		{
			int i;
			LOGD("[%s]: msg_data:", __FUNCTION__);
			for (i = 0; i < (int) msg_size; i++)
			{
				LOGD("[%s]: msg_data[%d] = 0x%x", __FUNCTION__, i, msg_data[i]);
			}
		}
*/

		if (!CP2LSM_env.CP_SendNetMessage(ota_type, rrc_state, session_type, (LSM_UINT8 *) msg_data, (LSM_UINT32) msg_size))
		{
			rc = -1;
			goto ExitNwDlMsg;
		}
	}

ExitNwDlMsg:
	LOGD("[%s]: Exit point. rc = %d", __FUNCTION__, rc);
	return rc;
}

/*******************************************************************************\
*	Function:		CpaClient_RecvRrcStateInd
*	Description:	This function is called when an RRC State message is received
*					from the Server. It strips the message data and calls the
*					corresponding LSM API function.
*	Parameters:		See below
*	Returns:		0=Success, -1=Fail
\*******************************************************************************/
int CpaClient_RecvRrcStateInd(UINT8 bearer_type, UINT8 in_rrc_state)
{
	eLSM_RRC_STATE rrc_state;

	int rc = 0;

	LOGD("[%s]: Entry point", __FUNCTION__);

	switch (in_rrc_state)
	{
		case CI_DEV_LP_RRC_STATE_CELL_DCH:
			rrc_state = LSM_RRC_STATE_CELL_DCH;
			break;

		case CI_DEV_LP_RRC_STATE_CELL_FACH:
			rrc_state = LSM_RRC_STATE_CELL_FACH;
			break;

		case CI_DEV_LP_RRC_STATE_CELL_PCH:
			rrc_state = LSM_RRC_STATE_CELL_PCH;
			break;

		case CI_DEV_LP_RRC_STATE_URA_PCH:
			rrc_state = LSM_RRC_STATE_URA_PCH;
			break;

		case CI_DEV_LP_RRC_STATE_IDLE:
			rrc_state = LSM_RRC_STATE_IDLE;
			break;

		default:
			LOGE("[%s] invalid RRC STATE!!", __FUNCTION__);
			rrc_state = LSM_RRC_INVALID_STATE;
			rc = -1;
			goto ExitRrcStateMsg;
	}

	LOGD("[%s]: bearer_type = %d, rrc_state = %d", __FUNCTION__,
				bearer_type,
				in_rrc_state);

	if (!CP2LSM_env.CP_SendRRCStateEvent(rrc_state))
	{
		LOGE("[%s] CP_SendRRCStateEvent fail!!", __FUNCTION__);
		rc = -1;
		goto ExitRrcStateMsg;
	}

ExitRrcStateMsg:
	LOGD("[%s]: Exit point. rc = %d", __FUNCTION__, rc);
	return rc;
}

/*******************************************************************************\
*	Function:		CpaClient_RecvMeasTerminateInd
*	Description:	This function is called when Measurement Terminate message is
*					received from the Server. It strips the message data and calls
*					the corresponding LSM API function.
*	Parameters:		See below
*	Returns:		0=Success, -1=Fail
\*******************************************************************************/
int CpaClient_RecvMeasTerminateInd(UINT8 bearer_type)
{
	eLSM_OTA_TYPE ota_type;
	int rc = 0;

	LOGD("[%s]: Entry point", __FUNCTION__);

	switch (bearer_type)
	{
		case CI_DEV_LP_RRLP:
			ota_type = LSM_CP_RRLP;
			break;

		case CI_DEV_LP_RRC:
			ota_type = LSM_CP_RRC;
			break;

		default:
			LOGE("[%s] Bearer type error!!", __FUNCTION__);
			rc = -1;
			goto ExitMeasTerminateMsg;
	}

	LOGD("[%s]: ota_type = %d", __FUNCTION__, ota_type);

	if (!CP2LSM_env.CP_SendMeasurementTerminate(ota_type))
	{
		LOGE("[%s] CP_SendMeasurementTerminate fail!!", __FUNCTION__);
		rc = -1;
		goto ExitMeasTerminateMsg;
	}

ExitMeasTerminateMsg:
	LOGD("[%s]: Exit point. rc = %d", __FUNCTION__, rc);
	return rc;
}

/*******************************************************************************\
*	Function:		CpaClient_RecvResetPosInfoInd
*	Description:	This function is called when Reset Position Stored Info message
*					is received from the Server. It strips the message data and
*					calls the corresponding LSM API function.
*	Parameters:		See below
*	Returns:		0=Success, -1=Fail
\*******************************************************************************/
int CpaClient_RecvResetPosInfoInd(UINT8 bearer_type)
{
	eLSM_OTA_TYPE ota_type;
	int rc = 0;

	LOGD("[%s]: Entry point", __FUNCTION__);

	switch (bearer_type)
	{
		case CI_DEV_LP_RRLP:
			ota_type = LSM_CP_RRLP;
			break;

		case CI_DEV_LP_RRC:
			ota_type = LSM_CP_RRC;
			break;

		default:
			LOGE("[%s] Bearer type error!!", __FUNCTION__);
			rc = -1;
			goto ExitResetPosInfoMsg;
	}

	LOGD("[%s]: ota_type = %d", __FUNCTION__, ota_type);

	if (!CP2LSM_env.CP_Reset())
	{
		LOGE("[%s] CP_Reset fail!!", __FUNCTION__);
		rc = -1;
		goto ExitResetPosInfoMsg;
	}

ExitResetPosInfoMsg:
	LOGD("[%s]: Exit point. rc = %d", __FUNCTION__, rc);
	return rc;
}
/******************************************************************************
*\
*	Function:		CpaClient_RecvLocInd
*	Description:	This function is called by MTIL Server when receiving the 
network
*					C-Plane MT-LR Notification.
*	Parameters:		See below
*	Returns:		0=Success, -1=Fail
\******************************************************************************
*/
int CpaClient_RecvLocInd(char* pData) // pointer to the message structure
{
	P_CPA_LCS_LOCATION_IND pLpLocInd = (P_CPA_LCS_LOCATION_IND) pData;
	CPA_LCS_LOCATION_IND CpaLcsLocationInd;
	P_CPA_LCS_LOCATION_IND pCpaLcsLocationInd = &CpaLcsLocationInd;
	int rc = 0;

	LOGD("[%s]: Entry point", __FUNCTION__);

	// If sizes are different, CPA struct (CpaClient.h) must follow MTIL struct (MtilAPI.h) changes
	//ASSERT(sizeof(CPA_LCS_LOCATION_IND) == sizeof(MTIL_LCS_LOCATION_IND));
	//if (sizeof(CPA_LCS_LOCATION_IND) != sizeof(MTIL_LCS_LOCATION_IND))
	//{
	//	LOGE("[%s]: Error! CPA struct CPA_LCS_LOCATION_IND must follow MTIL struct MTIL_LCS_LOCATION_IND changes", __FUNCTION__);
		//ASSERT(FALSE);
	//	rc = -1;
	//	goto ExitRecvLocInd;
	//}
#if 0
	LOGD("[%s]: notifyType=%d, invokeHandle=0x%lx, locEstimateType=%d, deferredLocationTypePresent=%d, deferredLocationType=0x%lx", __FUNCTION__,
		pLpLocInd->notifyType,
		pLpLocInd->invokeHandle,
		pLpLocInd->locationType.locEstimateType,
		pLpLocInd->locationType.deferredLocationTypePresent,
		pLpLocInd->locationType.deferredLocationType);

	LOGD("[%s]: optClientName: present=%d, dataCodingScheme=%d, length=%d, name=%s optFormatIndicator: present=%d, formatIndicator=%d", __FUNCTION__,
		pLpLocInd->optClientName.present,
		pLpLocInd->optClientName.dataCodingScheme,
		pLpLocInd->optClientName.clientNameString.length,
		pLpLocInd->optClientName.clientNameString.name,
		pLpLocInd->optClientName.optFormatIndicator.present,
		pLpLocInd->optClientName.optFormatIndicator.formatIndicator);

	LOGD("[%s]: optRequestorId: present=%d, dataCodingScheme=%d, length=%d, name=%s optFormatIndicator: present=%d, formatIndicator=%d", __FUNCTION__,
		pLpLocInd->optRequestorId.present,
		pLpLocInd->optRequestorId.dataCodingScheme,
		pLpLocInd->optRequestorId.requestorIdString.length,
		pLpLocInd->optRequestorId.requestorIdString.name,
		pLpLocInd->optRequestorId.optFormatIndicator.present,
		pLpLocInd->optRequestorId.optFormatIndicator.formatIndicator);
#endif
	// Reset the DST struct
	memset(pCpaLcsLocationInd, 0, sizeof(CPA_LCS_LOCATION_IND));

	// Copy the data
	memcpy(pCpaLcsLocationInd, pLpLocInd, sizeof(CPA_LCS_LOCATION_IND));

	if (!CP2LSM_env.CP_MtlrNotifyLocInd(pCpaLcsLocationInd))
	{
		//ASSERT(FALSE);
		rc = -1;
		goto ExitRecvLocInd;
	}

ExitRecvLocInd:
	LOGD("[%s]: Exit point. rc = %d", __FUNCTION__, rc);
	return rc;
}

/*******************************************************************************\
*	Function:		CpaClient_RecvNetworkUplinkCnf
*	Description:	This function is called when Network Uplink Confirm message
*					is received from the Server. It parses the confirmation
*					result and prints it (for debug only).
*	Parameters:		See below
*	Returns:		0=Success, -1=Fail
\*******************************************************************************/
int CpaClient_RecvNetworkUplinkCnf(UINT16 ret_code)
{
	BOOL bTerminateSession = FALSE;
	int rc = 0;

	LOGD("[%s]: Entry point", __FUNCTION__);

	if (CIRC_DEV_SUCCESS == ret_code)
	{
		LOGD("[%s]: Received CNF OK", __FUNCTION__);

		if (g_bSendTerminateSession) // check if GPS session should be terminated
		{
			g_bSendTerminateSession = FALSE;
			bTerminateSession = TRUE; // do it at function exit
			LOGD("[%s]: Terminate the LSM session ...", __FUNCTION__);
		}
	}
	else
	{
		LOGE("[%s]: Error! Received CNF Error. ret_code = %d", __FUNCTION__, ret_code);

		// just in case, terminate GPS session, to save power
		bTerminateSession = TRUE; // do it at function exit
		LOGD("[%s]: Terminate the LSM session ...", __FUNCTION__);

	}

	if (bTerminateSession)
	{
		if (!CP2LSM_env.CP_SendMeasurementTerminate(gOtaType))
		{
			LOGE("[%s] CP_SendMeasurementTerminate fail!!", __FUNCTION__);
			rc = -1;
			goto ExitRcvNwUlCnf;
		}
	}

ExitRcvNwUlCnf:
	LOGD("[%s]: Exit point. rc = %d", __FUNCTION__, rc);
	return rc;
}
#if 0
/*******************************************************************************\
*	Function:		CpaClient_RecvAtCiIndEnSetCnf
*	Description:	This function is called when CI Ind Enable Set Confirm message
*					is received from the Server. It parses the confirmation
*					result and prints it (for debug only).
*	Parameters:		See below
*	Returns:		0=Success, -1=Fail
\*******************************************************************************/
int CpaClient_RecvAtCiIndEnSetCnf(P_MTIL_BASE_MESSAGE pBaseMsg) // pointer to the message structure
{
	int rc = 0;
	P_MTIL_GEN_RC_MSG data = (P_MTIL_GEN_RC_MSG) pBaseMsg->data;

	LOGD("[%s]: Entry point", __FUNCTION__);

	if (data->ret_code == MTIL_CODE_AT_OK)
	{
		LOGD("[%s]: Received CNF OK", __FUNCTION__);
	}

	else
	{
		LOGE("[%s]: Error! Received CNF Error. ret_code = %ld", __FUNCTION__, data->ret_code);
		ASSERT(FALSE);
	}

	LOGD("[%s]: Exit point. rc = %d", __FUNCTION__, rc);
	return rc;
}

/*******************************************************************************\
*	Function:		CpaClient_RecvIndHandler
*	Description:	Handle incoming messages from Server over socket.
*					This function handles a single message that was received over
*					the socket from the Server. After handling some common fields
*					of the headers (MTIL_BASE_MESSAGE_HEADER and MTIL_GENERIC_CI_HEADER),
*					it dispatches the specific function that handles the message.
*	Parameters:		See below
*	Returns:		None
\*******************************************************************************/
void CpaClient_RecvIndHandler(P_MTIL_BASE_MESSAGE pBaseMsg) // pointer to the message structure
{
	P_MTIL_TVEND_SPEC_SET_MSG pTvendSpecSetMsg;

	LOGD("[%s]: Entry point", __FUNCTION__);

	// Handle Indication messages
	pTvendSpecSetMsg = (P_MTIL_TVEND_SPEC_SET_MSG) pBaseMsg->data;

	if (!pTvendSpecSetMsg)
	{
		LOGE("[%s]: Error! Null pointer in pTvendSpecSetMsg", __FUNCTION__);
		ASSERT(FALSE);
		goto ExitRecvInMsg;
	}

	switch (pTvendSpecSetMsg->vendor_code)
	{
/*
		case MTIL_VENDOR_SPEC_AGPS_NOTIFY_EMERGENCYNUMDIALED:
		CpaClient_RecvEmergencyNumberDialedInd();
		break;
*/

	case MTIL_VENDOR_SPEC_AGPS_NWDL_IND_TYPE:
		CpaClient_RecvNetworkDownlinkInd(pTvendSpecSetMsg->data);
		break;

	case MTIL_VENDOR_SPEC_AGPS_RRC_STATE_IND_TYPE:
		CpaClient_RecvRrcStateInd(pTvendSpecSetMsg->data);
		break;

	case MTIL_VENDOR_SPEC_AGPS_MEAS_TERMINATE_IND_TYPE:
		CpaClient_RecvMeasTerminateInd(pTvendSpecSetMsg->data);
		break;

	case MTIL_VENDOR_SPEC_AGPS_RESET_UE_POS_IND_TYPE:
		CpaClient_RecvResetPosInfoInd(pTvendSpecSetMsg->data);
		break;

	default:
		break;
	}

	LOGD("[%s]: Exit point", __FUNCTION__);

ExitRecvInMsg:
	return;
}
#endif
/******************************************************************************
*\
*	Function:		CpaClient_Init
*	Description:	This function is used to initialize the Client’s Receive thread
*					and other resources of the Client. It is called externally by
*					the AGPS main function at Init stage.
*	Parameters: 	None
*	Returns:		0=Success, -1=Fail
\******************************************************************************
*/
int CpaClient_Init(void)
{
	int rc = 0;

	LOGD("[%s]: Entry point", __FUNCTION__);

	rc = MRIL_Client_Init();

	if (rc < 0)
	{
		LOGE("[%s]: Error! MRIL_Client_Init failed", __FUNCTION__);
		return rc;
	}

	LOGD("[%s]: Starting CPA Client over MTIL ...", __FUNCTION__);

	LSM_MessageListener listener;
	listener.reportMessageCb = CpaClient_SendNetworkUplinkReq;

	if (!CP2LSM_env.CP_SetMessageListeners(listener))
	{
		LOGE("[%s]: Error! CP_SetMessageListeners failed", __FUNCTION__);
		//ASSERT(FALSE);
		rc = -1;
	}
#if 0 // TEST_CPLANE
{
	eLSM_OTA_TYPE ota_type = LSM_CP_RRLP; 		// over-the-air type (RRLP / RRC)
	LSM_lplOtaParams p_ota_params;	// pointer to params of OTA
	LSM_UINT8 p_msg_data[1509];			// pointer to payload
	LSM_UINT32 msg_size = 1509;
	p_ota_params.is_final_response = LSM_TRUE;

	memset(p_msg_data, 0, sizeof(LSM_UINT8[1509]));
	CpaClient_SendNetworkUplinkReq(ota_type,			// over-the-air type (RRLP / RRC)
								   &p_ota_params,		// pointer to params of OTA
									p_msg_data,				// pointer to payload
									msg_size);


}
#endif
	LOGD("[%s]: Exit point. rc = %d", __FUNCTION__, rc);
	return rc;
}


/*******************************************************************************\
*	Function:		CpaClient_Deinit
*	Description:	This function is used to de-initialize the Client’s Receive thread
*					and other resources of the Client. It is called externally
*					by the AGPS main function at Exit stage.
*	Parameters:		None
*	Returns:		0=Success, -1=Fail
\*******************************************************************************/
int CpaClient_Deinit(void)
{
	int rc = 0;
	LOGD("[%s]: Entry point", __FUNCTION__);

	MRIL_Client_DeInit();

	LOGD("[%s]: Exit point. rc = %d", __FUNCTION__, rc);
	return rc;
}

