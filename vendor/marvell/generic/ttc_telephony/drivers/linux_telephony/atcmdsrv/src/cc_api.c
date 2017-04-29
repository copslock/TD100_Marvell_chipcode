/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: cc_api.c
 *
 *  Description: API interface implementation for call control service
 *
 *  History:
 *   August 28, 2008 - Rovin Yu Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/
#include <math.h>
#include "ci_api_types.h"
#include "ci_api.h"
#include "telatci.h"
#include "teldef.h"
#include "telatparamdef.h"
#include "tel3gdef.h"
#include "telutl.h"
#include "telconfig.h"
#include "ci_cc.h"
#include "ci_sim.h"
#include "utlMalloc.h"
#include "telsim.h"
#include "sim_api.h"
#include "cc_api.h"


extern UINT8 g_iCRCMode;

AtciCurrentCallsList gCurrentCallsList;
static AtciCurrentCallsList sBackupCurrentCallsList;

UINT16 DTMFDuration = 0;
CHAR currentSentDTMFStr[TEL_AT_VTS_0_DTMF_STR_MAX_LEN + 1];

static utlRelativeTime_T ringing_period = { TEL_RINGING_TIMER_PERIOD_IN_SEC, 0 };
static utlTimeOutCount_T ringing_num_cycles = TEL_RINGING_TIMER_NUM_CYCLES;
static utlTimerId_T ringing_timer_id = 0;

#define TEL_AUTO_ANSWER_DELAY_IN_SEC  1     /* ring once for each second */
#define TEL_AUTO_ANSWER_DELAY_CYCLES  0    /* set 0 to disable the auto answer */
static utlRelativeTime_T auto_answer_delay = { TEL_AUTO_ANSWER_DELAY_IN_SEC, 0 };
utlTimeOutCount_T auto_answer_delay_cycles = TEL_AUTO_ANSWER_DELAY_CYCLES;
static utlTimeOutCount_T auto_answer_remind_cycles = TEL_AUTO_ANSWER_DELAY_CYCLES;
static utlTimerId_T auto_answer_timer_id = 0;
static CiCcCallType IncomingCallType = 0;


extern AtciSSCntx gSSCntx;
extern CiServiceHandle gAtciSvgHandle[CI_SG_NUMIDS + 1];
extern INT32 gWaitToSetAcmMax;

/*add for AT+CPUC support*/
extern CiCcPrimSetPuctInfoReq gSetPuctInfoReq;

char gErrCauseBuf[256];

#ifdef AT_CUSTOMER_HTC
typedef enum
{
	AT_HTCDIS_CAUSE_CC_CALL_BARRING = 0xF0,
	AT_HTCDIS_CAUSE_CC_FDN_REJECT,
} AT_HTCDIS_CAUSE;
#endif

void PARSER_START_RINGING();
void PARSER_ANSWER_CALL();
void PARSER_RELEASE_CALL();
void PARSER_START_AUTO_ANSWER();
void resetCurrCall( UINT8 atpIdx );
void resetCallParam( CiRequestHandle reqHandle, CiCcCallId callId );


extern CiReturnCode ciMakeGPRSDataCallbyAtd(UINT32 atHandle, const char *command_name_p);
extern CiReturnCode ciMakeSS(UINT32 atHandle, const char *command_name_p);

static inline void resetCallInfo(AtCiCcCallInfo *pCallInfo)
{
	if(!pCallInfo) return;
	memset(pCallInfo, 0, sizeof(*pCallInfo));
	pCallInfo->CallId = 0;
	pCallInfo->IsEmergency = FALSE;
	pCallInfo->IsMPTY = FALSE;
	pCallInfo->State = CICC_NUM_CURRENT_CSTATES;
	pCallInfo->OldState = CICC_NUM_CURRENT_CSTATES;
	pCallInfo->Mode = CICC_NUM_CURRENT_CMODES;
	pCallInfo->OldMode = CICC_NUM_CURRENT_CMODES;
	/* default release direction to mobile terminated */
	pCallInfo->ReleaseDirection = CICC_MT_CALL;
	pCallInfo->reqHandle = INVALID_REQ_HANDLE;
}

/****************************************************************************************
*  FUNCTION:  CC_Dial
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process ATD
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode CC_Dial(UINT32 atHandle, CHAR* szAddress, UINT32 dwType, UINT32 dwOptions)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimMakeCallReq     *makeCallReq;

	if (dwType != CICC_BASIC_CMODE_VOICE)
	{
		/***** begin handle *99# */
		if (strlen((char*)szAddress) > 3 )
		{
			char *validAddress = (char*)szAddress;
			if (szAddress[0] == 'T' || szAddress[0] == 't')
				validAddress++;
			if ((validAddress[0] == '*') && (validAddress[1] == '9') && (validAddress[2] == '9'))
			{

				/*
				**  modify the timeout for the AT command processing
				*/
				{
					utlRelativeTime_T period;
					period.seconds     = 155;
					period.nanoseconds = 0;
					if ( utlModifyPendingAsyncResponse(&period, atHandle) != utlSUCCESS)
						return ret;
				}
				ret = ciMakeGPRSDataCallbyAtd(atHandle, (const char *)validAddress);
				return ret;
			}
		}
		/***** end handle *99# */

		/* Added by Johnny to support PIN/PIN2 UNBLOCK: **04*, **05*, **042*, **052* */
		if ( (strlen((char*)szAddress) > 4) && (isCHVOper((char*)szAddress) == TRUE))
		{

			ret = ciUnblockPin(atHandle, (const char*)szAddress);

			return ret;

		}

		/*
		 * Support activating call un-related SS by ATD command, such as ATD*#30#.
		 * So here we should judge it.
		 */
		if ( szAddress[0] == '*' || szAddress[0] == '#')
		{
			ret = ciMakeSS(atHandle, (const char *)szAddress);

			return ret;

		}

		DBGMSG("This is a %s call.\n", dwType == CICC_BASIC_CMODE_FAX ? "fax" : "data");

	}
	if(CI_MAX_ADDRESS_LENGTH - 1 < strlen((char *)szAddress))
	{
			ERRMSG("%s: the length of %s is too long, exceed %d\n", __FUNCTION__, (char *)szAddress, CI_MAX_ADDRESS_LENGTH - 1);
			return ret;
	}
	makeCallReq = utlCalloc(1, sizeof(*makeCallReq));
	if (makeCallReq == NULL)
		return CIRC_FAIL;

	makeCallReq->info.BasicCMode = dwType;
	strcpy((char*)makeCallReq->info.dialString, (char*)szAddress);
	makeCallReq->info.Options = dwOptions;

	DBGMSG("%s: BasicCMode = %d, dialString = %s\n", __FUNCTION__, makeCallReq->info.BasicCMode, makeCallReq->info.dialString);

	gCurrentCallsList.currCall[GET_ATP_INDEX(atHandle)].currBasicMode = dwType;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_MAKE_CALL_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_MAKE_CALL_REQ), (void *)makeCallReq );

	return ret;

}

/****************************************************************************************
*  FUNCTION:  CC_Answer
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process ATA
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode CC_Answer(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	INT32 i;

	CiCcPrimAnswerCallReq      *answerRequest;

	for ( i = 0; i < gCurrentCallsList.NumCalls; i++ )
	{
		if ( gCurrentCallsList.callInfo[i].State == CICC_CURRENT_CSTATE_INCOMING )
		{
			answerRequest = utlCalloc(1, sizeof(*answerRequest));
			if (answerRequest == NULL)
				return CIRC_FAIL;

			answerRequest->CallId = gCurrentCallsList.callInfo[i].CallId;

			ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_ANSWER_CALL_REQ,
					 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_ANSWER_CALL_REQ), (void *)answerRequest );

			/* stop ringing timer */
			PARSER_ANSWER_CALL();

			break; /* entry found - break out of the loop */
		}
	}
	return ret;

}

/****************************************************************************************
*  FUNCTION:  CC_Hangup
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process ATH
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_Hangup(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	INT32 i;

	CiCcPrimReleaseCallReq          *ccReleaseRequest;

	/* look for an active or in progress call */
	for ( i = 0; i < gCurrentCallsList.NumCalls; i++ )
	{
		if ( gCurrentCallsList.callInfo[i].State != CICC_NUM_CURRENT_CSTATES )
		{
			/* mark is is MO released */
			gCurrentCallsList.callInfo[i].ReleaseDirection = CICC_MO_CALL;
			ccReleaseRequest = utlCalloc(1, sizeof(*ccReleaseRequest));
			if (ccReleaseRequest == NULL)
				return CIRC_FAIL;

			ccReleaseRequest->CallId = gCurrentCallsList.callInfo[i].CallId;
			ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_RELEASE_CALL_REQ,
					 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_RELEASE_CALL_REQ), (void *)ccReleaseRequest );
		}
	}

	return ret;
}

/****************************************************************************************
*  FUNCTION:  CC_HangVT
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process ECHUPVT
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_HangVT(UINT32 atHandle, UINT32 causeValue)
{
	CiReturnCode ret = CIRC_FAIL;
	INT32 i;

	CiCcPrimReleaseCallReq          *ccReleaseRequest;
	CiCcPrimRefuseCallReq *ccRefuseRequest;

	/* look for an active or in progress call */
	for ( i = 0; i < gCurrentCallsList.NumCalls; i++ )
	{
		if ( gCurrentCallsList.callInfo[i].State != CICC_NUM_CURRENT_CSTATES
		     && gCurrentCallsList.callInfo[i].State != CICC_CURRENT_CSTATE_HELD )
		{
			if(gCurrentCallsList.callInfo[i].State == CICC_CURRENT_CSTATE_INCOMING
				|| gCurrentCallsList.callInfo[i].State == CICC_CURRENT_CSTATE_WAITING)
			{
				/* mark is MO released */
				gCurrentCallsList.callInfo[i].ReleaseDirection = CICC_MO_CALL;
				ccRefuseRequest = utlCalloc(1, sizeof(*ccRefuseRequest));
				if (ccRefuseRequest == NULL)
					return CIRC_FAIL;

				ccRefuseRequest->CallId = gCurrentCallsList.callInfo[i].CallId;
				ccRefuseRequest->Cause = causeValue;
				ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_REFUSE_CALL_REQ,
						 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_REFUSE_CALL_REQ), (void *)ccRefuseRequest );
			}
			else
			{
				/* mark is is MO released */
				gCurrentCallsList.callInfo[i].ReleaseDirection = CICC_MO_CALL;
				ccReleaseRequest = utlCalloc(1, sizeof(*ccReleaseRequest));
				if (ccReleaseRequest == NULL)
					return CIRC_FAIL;

				ccReleaseRequest->CallId = gCurrentCallsList.callInfo[i].CallId;
				ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_RELEASE_CALL_REQ,
						 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_RELEASE_CALL_REQ), (void *)ccReleaseRequest );
			}
		}
	}

	return ret;
}



/****************************************************************************************
*  FUNCTION:  CC_SendDTMF
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+VTS=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_SendDTMF(UINT32 atHandle, CHAR* szChars, UINT32 dwDuration)
{

	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimSetDtmfPacingReq  *setDTMFPacingReq;
	CiCcPrimSendDtmfStringReq *sendDTMFStrReq;

	memcpy(currentSentDTMFStr, szChars, sizeof(currentSentDTMFStr));
	if (dwDuration != DTMFDuration)
	{
		//will send  CI_CC_PRIM_SEND_DTMF_STRING_REQ after we receive Cnf
		DTMFDuration = dwDuration;
		setDTMFPacingReq = utlCalloc(1, sizeof(*setDTMFPacingReq));
		if (setDTMFPacingReq == NULL)
			return CIRC_FAIL;

		setDTMFPacingReq->Pacing.Duration = dwDuration;
		ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC],
				 CI_CC_PRIM_SET_DTMF_PACING_REQ,
				 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_SEND_DTMF_STRING_REQ),
				 (void*)setDTMFPacingReq);
	}
	else
	{
		sendDTMFStrReq = utlCalloc(1, sizeof(*sendDTMFStrReq));
		if (sendDTMFStrReq == NULL)
			return CIRC_FAIL;

		sendDTMFStrReq->CallId = 0;
		memcpy(sendDTMFStrReq->digits, currentSentDTMFStr, sizeof(sendDTMFStrReq->digits));
		ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC],
				 CI_CC_PRIM_SEND_DTMF_STRING_REQ,
				 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_SEND_DTMF_STRING_REQ),
				 (void*)sendDTMFStrReq);
	}
	return ret;
}

CiReturnCode CC_GetDTMFDuration(UINT32 atHandle){
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimGetDtmfPacingReq  *GetDTMFPacingReq = NULL;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC],
			CI_CC_PRIM_GET_DTMF_PACING_REQ,
			MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_GET_DTMF_PACING_REQ),
			(void*)GetDTMFPacingReq);
	return ret;
}

CiReturnCode CC_SetDTMFDuration(UINT32 atHandle, UINT32 duration){
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimSetDtmfPacingReq  *setDTMFPacingReq;

	DTMFDuration = duration;
	setDTMFPacingReq = utlCalloc(1, sizeof(*setDTMFPacingReq));
	if (setDTMFPacingReq == NULL)
		return CIRC_FAIL;

	setDTMFPacingReq->Pacing.Duration = duration;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC],
		CI_CC_PRIM_SET_DTMF_PACING_REQ,
		MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_SET_DTMF_PACING_REQ),
		(void*)setDTMFPacingReq);
	return ret;
}

/****************************************************************************************
*  FUNCTION:  CC_GetCallList
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+CLCC
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_GetCallList(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimListCurrentCallsReq    *getCallListReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_LIST_CURRENT_CALLS_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_LIST_CURRENT_CALLS_REQ), (void *)getCallListReq );
	return ret;

}

/****************************************************************************************
*  FUNCTION:  CC_ManageCalls
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+CHLD=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_ManageCalls(UINT32 atHandle, UINT32 dwCommand, UINT32 dwID)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimManipulateCallsReq                   *callOnHoldReq;
	CiCcPrimReleaseCallReq          *ccReleaseRequest;
	BOOL sendCHLDReq = FALSE;
	int i;
	UINT32 reqHandle;

	/*
	**  find the call to be put on hold.
	*/
	DBGMSG("%s: gCurrentCallsList.NumCalls is: %d, currCallId is: %d\n", __FUNCTION__, gCurrentCallsList.NumCalls, gCurrentCallsList.currCall[GET_ATP_INDEX(atHandle)].currCallId);
	for ( i = 0; i < gCurrentCallsList.NumCalls; i++ )
	{
		DBGMSG("%s: No. %d call state: %d, callId: %d\n", __FUNCTION__, i + 1, gCurrentCallsList.callInfo[i].State, gCurrentCallsList.callInfo[i].CallId);
		if ( dwID == 0 )
		{
			if (( gCurrentCallsList.callInfo[i].State == CICC_CURRENT_CSTATE_ACTIVE ) ||
			    ( gCurrentCallsList.callInfo[i].State == CICC_CURRENT_CSTATE_HELD ) ||
			    ( gCurrentCallsList.callInfo[i].State == CICC_CURRENT_CSTATE_WAITING ) ||
			    ( gCurrentCallsList.callInfo[i].State == CICC_CURRENT_CSTATE_INCOMING))
			{
				sendCHLDReq = TRUE;
				break; /* entry found - break out of the loop */
			}
		}
		else
		{
			if (( gCurrentCallsList.callInfo[i].CallId == dwID ) &&
			    (( gCurrentCallsList.callInfo[i].State == CICC_CURRENT_CSTATE_ACTIVE ) ||
			     ( gCurrentCallsList.callInfo[i].State == CICC_CURRENT_CSTATE_HELD ) ||
			     ( gCurrentCallsList.callInfo[i].State == CICC_CURRENT_CSTATE_WAITING ) ||
			     ( gCurrentCallsList.callInfo[i].State == CICC_CURRENT_CSTATE_DIALING) ||
			     ( gCurrentCallsList.callInfo[i].State == CICC_CURRENT_CSTATE_ALERTING)))

			{
				sendCHLDReq = TRUE;
				break; /* entry found - break out of the loop */
			}
		}
	}

	/*
	**  is it a valid request.
	*/
	if ( sendCHLDReq == TRUE )
	{

		if( dwCommand == CI_CC_MANOP_RLS_CALL && 
			(( gCurrentCallsList.callInfo[i].State == CICC_CURRENT_CSTATE_DIALING) ||
			     ( gCurrentCallsList.callInfo[i].State == CICC_CURRENT_CSTATE_ALERTING)))
		{
			//release MO call
			gCurrentCallsList.callInfo[i].ReleaseDirection = CICC_MO_CALL;
			ccReleaseRequest = utlCalloc(1, sizeof(*ccReleaseRequest));
			if (ccReleaseRequest == NULL)
				return CIRC_FAIL;

			ccReleaseRequest->CallId = gCurrentCallsList.callInfo[i].CallId;
			ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_RELEASE_CALL_REQ,
					 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_RELEASE_CALL_REQ), (void *)ccReleaseRequest );

			return ret;
		}

		if ( dwCommand == CI_CC_MANOP_RLS_ACT_ACCEPT_OTHER )
		{
			/* One workaround for SAC bug */
			for ( i = 0; i < gCurrentCallsList.NumCalls; i++ )
			{
				if (gCurrentCallsList.callInfo[i].State == CICC_CURRENT_CSTATE_WAITING ||
				    gCurrentCallsList.callInfo[i].State == CICC_CURRENT_CSTATE_HELD ||
				    gCurrentCallsList.callInfo[i].State == CICC_CURRENT_CSTATE_INCOMING)
					break;
			}

			/* If all the calls are active call, release them all */
			if ( i == gCurrentCallsList.NumCalls )
			{
				DBGMSG("%s: Hangup all the active calls!\n", __FUNCTION__);
				ret = CC_Hangup(atHandle);
				return ret;
			}
		}
		callOnHoldReq = utlCalloc(1, sizeof(*callOnHoldReq));
		if (callOnHoldReq == NULL)
			return CIRC_FAIL;

		callOnHoldReq->CallId = gCurrentCallsList.callInfo[i].CallId;
		callOnHoldReq->OpCode = dwCommand;
		DBGMSG("%s: Callid: %d OpCode: %d.\n", __FUNCTION__, callOnHoldReq->CallId, callOnHoldReq->OpCode);

		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_MANIPULATE_CALLS_REQ);

		gCurrentCallsList.callInfo[i].reqHandle = reqHandle;

		ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_MANIPULATE_CALLS_REQ,
				 reqHandle, (void *)callOnHoldReq );
	}

	return ret;
}

/****************************************************************************************
*  FUNCTION:  CC_HangupAllVoiceCall
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+CHLD=19
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_HangupAllVoiceCall(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimReleaseAllCallsReq    *ccReleaseAllRequest = utlCalloc(1, sizeof(*ccReleaseAllRequest));

	if (ccReleaseAllRequest == NULL)
		return CIRC_FAIL;

	ccReleaseAllRequest->WhichCalls = CICC_WHICHCALLS_ALL_CALLS;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC],
			 CI_CC_PRIM_RELEASE_ALL_CALLS_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_RELEASE_ALL_CALLS_REQ),
			 (void *)ccReleaseAllRequest );
	return ret;
}

/****************************************************************************************
*  FUNCTION:  CC_GetAcm
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+CACM?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_GetAcm(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimGetAcmValueReq       *getAcmValRequest = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_GET_ACM_VALUE_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_GET_ACM_VALUE_REQ), (void *)getAcmValRequest );
	return ret;

}

/****************************************************************************************
*  FUNCTION:  CC_SetAcm
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+CACM=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_SetAcm(UINT32 atHandle, CHAR *passwd)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimResetAcmValueReq    *resetAcmReq = NULL;
	UINT8 oper;
	if ( passwd == NULL )
	{

		/* if password is not specified => send 'reset ACM' CI request directly */
		ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_RESET_ACM_VALUE_REQ,
				 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_RESET_ACM_VALUE_REQ), (void *)resetAcmReq );
	}
	else
	{
		/* if password is specified => must perform PIN2 validation */
		oper = (((RESET_ACM & 0x0f) << 4) | (CI_SIM_CHV_VERIFY & 0x0f));
		ret = SIM_EnterPin(atHandle, CI_SIM_CHV_2, oper, passwd, NULL);
	}

	return ret;
}

/****************************************************************************************
*  FUNCTION:  CC_GetAmm
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+CAMM?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_GetAmm(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimGetAcmMaxValueReq    *getAcmmRequest = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_GET_ACMMAX_VALUE_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_GET_ACMMAX_VALUE_REQ), (void *)getAcmmRequest );
	return ret;

}

/****************************************************************************************
*  FUNCTION:  CC_SetAmm
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+CAMM=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_SetAmm(UINT32 atHandle, UINT32 acmMax, CHAR *passwd)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimSetAcmMaxValueReq    *setAcmmRequest;
	UINT8 oper;
	if ( passwd == NULL )
	{

		/* if password is not specified => send 'reset ACM' CI request directly */
		setAcmmRequest = utlCalloc(1, sizeof(*setAcmmRequest));
		if (setAcmmRequest == NULL)
			return CIRC_FAIL;

		setAcmmRequest->AcmMax = acmMax;
		ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_SET_ACMMAX_VALUE_REQ,
				 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_SET_ACMMAX_VALUE_REQ), (void *)setAcmmRequest );
	}
	else
	{
		/* if password is specified => must perform PIN2 validation */
		gWaitToSetAcmMax = acmMax;
		oper = (((SET_ACM_MAX & 0x0f) << 4) | (CI_SIM_CHV_VERIFY & 0x0f));
		ret = SIM_EnterPin(atHandle, CI_SIM_CHV_2, oper, passwd, NULL);
	}

	return ret;
}

/****************************************************************************************
*  FUNCTION:  CC_GetBst
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+CBST?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_GetBst(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimGetDataBsTypeReq           *getDataBsTypeReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_GET_DATA_BSTYPE_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_GET_DATA_BSTYPE_REQ), (void *)getDataBsTypeReq);
	return ret;

}

/****************************************************************************************
*  FUNCTION:  CC_GetSupportBst
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+CBST=?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_GetSupportBst(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimGetSupportedDataBsTypesReq     *getSupportedDataBsTypeReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_GET_SUPPORTED_DATA_BSTYPES_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_GET_SUPPORTED_DATA_BSTYPES_REQ), (void *)getSupportedDataBsTypeReq);
	return ret;

}


/************************************************************************************
 * FUNCTION: convertATbstSpeed2CI
 *
 * PARAMETERS:     ATbstSpeed - BST speed as defined in 3GPP 27.007
 *                 CIbstSpeed - BST speed as defined in CI
 *
 * DESCRIPTION:    Converts BST speed parameter from 27.007 into CI enum type
 *
 * RETURNS: TRUE if the value is valid, FALSE if not
 *************************************************************************************/
BOOL convertATbstSpeed2CI( INT32 ATbstSpeed, CiBsTypeSpeed *CIbstSpeed )
{
	BOOL isValid = TRUE;

	if ( ATbstSpeed <= 7 )
	{
		/* These values are the same - no conversion needed */
		*CIbstSpeed = (CiBsTypeSpeed)ATbstSpeed;
	}
	else
	{
		switch (ATbstSpeed)
		{
		case 12:
			*CIbstSpeed = CI_BSTYPE_SPEED_9600_V34;
			break;
		case 14:
			*CIbstSpeed = CI_BSTYPE_SPEED_14400_V34;
			break;
		case 15:
			*CIbstSpeed = CI_BSTYPE_SPEED_19200_V34;
			break;
		case 16:
			*CIbstSpeed = CI_BSTYPE_SPEED_28800_V34;
			break;
		case 17:
			*CIbstSpeed = CI_BSTYPE_SPEED_33600_V34;
			break;
		case 34:
			*CIbstSpeed = CI_BSTYPE_SPEED_1200_V120;
			break;
		case 36:
			*CIbstSpeed = CI_BSTYPE_SPEED_2400_V120;
			break;
		case 38:
			*CIbstSpeed = CI_BSTYPE_SPEED_4800_V120;
			break;
		case 39:
			*CIbstSpeed = CI_BSTYPE_SPEED_9600_V120;
			break;
		case 43:
			*CIbstSpeed = CI_BSTYPE_SPEED_14400_V120;
			break;
		case 47:
			*CIbstSpeed = CI_BSTYPE_SPEED_19200_V120;
			break;
		case 48:
			*CIbstSpeed = CI_BSTYPE_SPEED_28800_V120;
			break;
		case 49:
			*CIbstSpeed = CI_BSTYPE_SPEED_38400_V120;
			break;
		case 50:
			*CIbstSpeed = CI_BSTYPE_SPEED_48000_V120;
			break;
		case 51:
			*CIbstSpeed = CI_BSTYPE_SPEED_56000_V120;
			break;
		case 65:
			*CIbstSpeed = CI_BSTYPE_SPEED_300_V110;
			break;
		case 66:
			*CIbstSpeed = CI_BSTYPE_SPEED_1200_V110;
			break;
		case 68:
			*CIbstSpeed = CI_BSTYPE_SPEED_2400_V110_X31;
			break;
		case 70:
			*CIbstSpeed = CI_BSTYPE_SPEED_4800_V110_X31;
			break;
		case 71:
			*CIbstSpeed = CI_BSTYPE_SPEED_9600_V110_X31;
			break;
		case 75:
			*CIbstSpeed = CI_BSTYPE_SPEED_14400_V110_X31;
			break;
		case 79:
			*CIbstSpeed = CI_BSTYPE_SPEED_19200_V110_X31;
			break;
		case 80:
			*CIbstSpeed = CI_BSTYPE_SPEED_28800_V110_X31;
			break;
		case 81:
			*CIbstSpeed = CI_BSTYPE_SPEED_38400_V110_X31;
			break;
		case 82:
			*CIbstSpeed = CI_BSTYPE_SPEED_48000_V110_X31;
			break;
		case 83:
			*CIbstSpeed = CI_BSTYPE_SPEED_56000_V110_X31;
			break;
		case 84:
			*CIbstSpeed = CI_BSTYPE_SPEED_64000_X31;
			break;
		case 115:
			*CIbstSpeed = CI_BSTYPE_SPEED_56000_BTM;
			break;
		case 116:
			*CIbstSpeed = CI_BSTYPE_SPEED_64000_BTM;
			break;
		case 120:
			*CIbstSpeed = CI_BSTYPE_SPEED_32000_PIAFS;
			break;
		case 121:
			*CIbstSpeed = CI_BSTYPE_SPEED_64000_PIAFS;
			break;
		case 130:
			*CIbstSpeed = CI_BSTYPE_SPEED_28800_MM;
			break;
		case 131:
			*CIbstSpeed = CI_BSTYPE_SPEED_32000_MM;
			break;
		case 132:
			*CIbstSpeed = CI_BSTYPE_SPEED_33600_MM;
			break;
		case 133:
			*CIbstSpeed = CI_BSTYPE_SPEED_56000_MM;
			break;
		case 134:
			*CIbstSpeed = CI_BSTYPE_SPEED_64000_MM;
			break;

		default:
			isValid = FALSE;
			break;
		}
	}
	return isValid;
}

/************************************************************************************
 * FUNCTION: convertATbstName2CI
 *
 * PARAMETERS:     ATbstName - BST speed as defined in 3GPP 27.007
 *                 CIbstName - BST speed as defined in CI
 *
 * DESCRIPTION:    Converts BST name parameter from 27.007 into CI enum type
 *
 * RETURNS: TRUE if the value is valid, FALSE if not
 *************************************************************************************/
BOOL convertATbstName2CI( INT32 ATbstName, CiCcBsTypeName *CIbstName )
{
	BOOL isValid = TRUE;

	switch ( ATbstName )
	{
	case 0:
		*CIbstName = CICC_BSTYPE_NAME_DATA_ASYNC_UDI;
		break;
	case 1:
		*CIbstName = CICC_BSTYPE_NAME_DATA_SYNC_UDI;
		break;
	case 2:
		*CIbstName = CICC_BSTYPE_NAME_PAD_ASYNC_UDI;
		break;
	case 3:
		*CIbstName = CICC_BSTYPE_NAME_PACKET_SYNC_UDI;
		break;
	case 4:
		*CIbstName = CICC_BSTYPE_NAME_DATA_ASYNC_RDI;
		break;
	case 5:
		*CIbstName = CICC_BSTYPE_NAME_DATA_SYNC_RDI;
		break;
	case 6:
		*CIbstName = CICC_BSTYPE_NAME_PAD_ASYNC_RDI;
		break;
	case 7:
		*CIbstName = CICC_BSTYPE_NAME_PACKET_SYNC_RDI;
		break;

	default:
		isValid = FALSE;
		break;
	}

	return isValid;
}

/************************************************************************************
 * FUNCTION: convertATbstCe2CI
 *
 * PARAMETERS:     ATbstCe - BST speed as defined in 3GPP 27.007
 *                 CIbstCe - BST speed as defined in CI
 *
 * DESCRIPTION:    Converts BST name parameter from 27.007 into CI enum type
 *
 * RETURNS: TRUE if the value is valid, FALSE if not
 *************************************************************************************/
BOOL convertATbstCe2CI( INT32 ATbstCe, CiCcBsTypeCe *CIbstCe )
{
	BOOL isValid = TRUE;

	switch ( ATbstCe )
	{
	case 0:
		*CIbstCe = CICC_BSTYPE_CE_TRANSPARENT_ONLY;
		break;
	case 1:
		*CIbstCe = CICC_BSTYPE_CE_NONTRANSPARENT_ONLY;
		break;
	case 2:
		*CIbstCe = CICC_BSTYPE_CE_PREFER_TRANSPARENT;
		break;
	case 3:
		*CIbstCe = CICC_BSTYPE_CE_PREFER_NONTRANSPARENT;
		break;

	default:
		isValid = FALSE;
		break;
	}

	return isValid;
}

/****************************************************************************************
*  FUNCTION:  CC_SetBst
*
* PARAMETERS:
*
*  DESCRIPTION: interface to process AT+CBST=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_SetBst(UINT32 atHandle, INT32 bstSpeed, INT32 bstName, INT32 bstCe)
{

	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimSetDataBsTypeReq        *setDataBsTypeReq;
	static CiCcDataBsTypeInfo bstProfile;

	if ( convertATbstSpeed2CI( bstSpeed, &bstProfile.Speed ) &&
	     convertATbstName2CI( bstName, &bstProfile.Name ) &&
	     convertATbstCe2CI( bstCe, &bstProfile.Ce ) )
	{
		setDataBsTypeReq = utlCalloc(1, sizeof(*setDataBsTypeReq));
		if (setDataBsTypeReq == NULL)
			return CIRC_FAIL;

		memcpy(&setDataBsTypeReq->info, &bstProfile, sizeof(bstProfile));
		ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_SET_DATA_BSTYPE_REQ,
				 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_SET_DATA_BSTYPE_REQ), (void *)setDataBsTypeReq );

	}

	return ret;
}

/****************************************************************************************
*  FUNCTION:  CC_GetMod
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+CMOD?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_GetMod(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimGetCallModeReq             *getCallModeReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_GET_CALLMODE_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_GET_CALLMODE_REQ), (void *)getCallModeReq);
	return ret;

}

/****************************************************************************************
*  FUNCTION:  CC_GetSupportMod
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+CMOD=?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_GetSupportMod(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimGetSupportedCallModesReq      *getSupportedCallModesReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_GET_SUPPORTED_CALLMODES_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_GET_SUPPORTED_CALLMODES_REQ), (void *)getSupportedCallModesReq);
	return ret;

}

/****************************************************************************************
*  FUNCTION:  CC_SetMod
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+CMOD=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_SetMod(UINT32 atHandle, INT32 callMode)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimSetCallModeReq             *setCallModeReq = utlCalloc(1, sizeof(*setCallModeReq));

	if (setCallModeReq == NULL)
		return CIRC_FAIL;

	setCallModeReq->Mode = (CiCcCallMode)callMode;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_SET_CALLMODE_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_SET_CALLMODE_REQ), (void *)setCallModeReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  CC_GetCcm
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+CAOC?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_GetCcm(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimGetCcmValueReq            *CCMvalueReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_GET_CCM_VALUE_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_GET_CCM_VALUE_REQ), (void *)CCMvalueReq );
	return ret;

}

/****************************************************************************************
*  FUNCTION:  CC_GetRlp
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+CRLP?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_GetRlp(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimGetRlpCfgReq              *GetRLPCfgReq = utlCalloc(1, sizeof(*GetRLPCfgReq));

	if (GetRLPCfgReq == NULL)
		return CIRC_FAIL;

	/* It looks like SAC does not even look at what version we are asking for */
	/* when it returns the confirmation for CI_CC_PRIM_GET_RLP_CFG_REQ          */
	GetRLPCfgReq->ver = 0;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_GET_RLP_CFG_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_GET_RLP_CFG_REQ), (void *)GetRLPCfgReq );
	return ret;

}

/****************************************************************************************
*  FUNCTION:  CC_GetSupportRlp
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+CRLP=?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_GetSupportRlp(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimGetRlpCapReq              *RLPCapReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_GET_RLP_CAP_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_GET_RLP_CAP_REQ), (void *)RLPCapReq );
	return ret;

}

/****************************************************************************************
*  FUNCTION:  CC_SetRlp
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+CRLP=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_SetRlp(UINT32 atHandle, CiCcRlpCfg *RLPCfg)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimSetRlpCfgReq              *SetRLPCfgReq = utlCalloc(1, sizeof(*SetRLPCfgReq));

	if (SetRLPCfgReq == NULL)
		return CIRC_FAIL;

	memcpy(&SetRLPCfgReq->cfg, RLPCfg, sizeof(*RLPCfg));

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_SET_RLP_CFG_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_SET_RLP_CFG_REQ), (void *)SetRLPCfgReq);

	return ret;
}


void restoreCallInfo(AtCiCcCallInfo *pInfo, CiCcCallId CallId)
{
	int i;
	if(!pInfo || CallId == CICC_CALLID_NONE) return;
	for (i = 0; i < sBackupCurrentCallsList.NumCalls; i++)
	{
		/* found, restore previous info */
		if(sBackupCurrentCallsList.callInfo[i].CallId == CallId)
		{
			memcpy(pInfo, &sBackupCurrentCallsList.callInfo[i], sizeof(*pInfo));
			break;
		}
	}

	/* not found, reset it to default value */
	if(i == sBackupCurrentCallsList.NumCalls)
		resetCallInfo(pInfo);
}

/************************************************************************************
 * FUNCTION: updateCallList
 *
 * PARAMETERS:
 *
 * DESCRIPTION:    updates and displays the valid entries of the CC calls list
 *
 * RETURNS
 *************************************************************************************/
static void updateCallList(CiRequestHandle reqHandle, INT32 numCalls, CiCcCallInfo *pCallInfo, BOOL ccCnf)
{
	UNUSEDPARAM(ccCnf)
	INT32 i, j;
	BOOL bFound = FALSE;
	char callListBuf[300];
	char callListTmpBuf[200];
	BOOL num_flag = FALSE;

	F_ENTER();
	/* backup current call list */
	sBackupCurrentCallsList = gCurrentCallsList;
	/* update the current calls list */
	gCurrentCallsList.NumCalls = numCalls;
	for ( i = 0; i < gCurrentCallsList.NumCalls; i++, pCallInfo++)
	{
		//if call id changed, try to restore previous content
		if(gCurrentCallsList.callInfo[i].CallId != pCallInfo->CallId)
		{
			restoreCallInfo(&gCurrentCallsList.callInfo[i], pCallInfo->CallId);
		}
		
		gCurrentCallsList.callInfo[i].CallId = pCallInfo->CallId;
		gCurrentCallsList.callInfo[i].IsEmergency = pCallInfo->IsEmergency;
		gCurrentCallsList.callInfo[i].IsMPTY = pCallInfo->IsMPTY;
		gCurrentCallsList.callInfo[i].CallDirection = pCallInfo->Direction;
		gCurrentCallsList.callInfo[i].OldState = gCurrentCallsList.callInfo[i].State;
		gCurrentCallsList.callInfo[i].State = pCallInfo->State;
		gCurrentCallsList.callInfo[i].OldMode = gCurrentCallsList.callInfo[i].Mode;
		gCurrentCallsList.callInfo[i].Mode = pCallInfo->Mode;

		num_flag = FALSE;

		sprintf( callListBuf, "+CLCC: %d, %d, %d, %d, %d", pCallInfo->CallId,
			 pCallInfo->Direction,
			 pCallInfo->State,
			 pCallInfo->Mode,
			 pCallInfo->IsMPTY );

		// do not overwrite if we have received SS indication before
		if (( gCurrentCallsList.callInfo[i].SsNumberInfoReceived == FALSE ) && 
			( pCallInfo->callerInfo.OptCallerNumber.Present == TRUE ) &&
			( pCallInfo->callerInfo.OptCallerNumber.AddressInfo.Length > 0 ))
		{
			memcpy( gCurrentCallsList.callInfo[i].Digits,
				pCallInfo->callerInfo.OptCallerNumber.AddressInfo.Digits,
				pCallInfo->callerInfo.OptCallerNumber.AddressInfo.Length );

			gCurrentCallsList.callInfo[i].Digits[pCallInfo->callerInfo.OptCallerNumber.AddressInfo.Length] = '\0';

			gCurrentCallsList.callInfo[i].AddrType = pCallInfo->callerInfo.OptCallerNumber.AddressInfo.AddrType;
		}

		if ( gCurrentCallsList.callInfo[i].Digits[0] != '\0' )
		{
			const char *plus = NULL;

			if ( gCurrentCallsList.callInfo[i].AddrType.NumType == CI_NUMTYPE_INTERNATIONAL )
				plus = "+";
			else
				plus = "";

			sprintf( callListTmpBuf, ", \"%s%s\"", plus, gCurrentCallsList.callInfo[i].Digits );
			strcat( callListBuf, callListTmpBuf );
			if ( gCurrentCallsList.callInfo[i].AddrType.NumType == CI_NUMTYPE_INTERNATIONAL )
			{
				strcat( callListBuf, ", 145" );
			}
			else
			{
				strcat( callListBuf, ", 129" );
			}
			num_flag = TRUE;
		}

		// do not overwrite if we have received SS indication before
		if (( gCurrentCallsList.callInfo[i].SsNameInfoReceived == FALSE ) && 
			( pCallInfo->callerInfo.OptCallerName.Present == TRUE ) &&
			( pCallInfo->callerInfo.OptCallerName.NameInfo.Length > 0 ))
		{
			gCurrentCallsList.callInfo[i].Name = pCallInfo->callerInfo.OptCallerName;
		}

		if ( ( gCurrentCallsList.callInfo[i].Name.Present == TRUE ) &&
		    ( gCurrentCallsList.callInfo[i].Name.NameInfo.Length > 0 ) )
		{
			char tmp_name[CI_MAX_NAME_LENGTH*4+1];
			int ret = libConvertIRAToCSCSString((const char *)gCurrentCallsList.callInfo[i].Name.NameInfo.Name, gCurrentCallsList.callInfo[i].Name.NameInfo.Length, tmp_name, sizeof(tmp_name) / sizeof(tmp_name[0]) - 1, chset_type);
			if(ret < 0)
			{
				ERRMSG("%s: convert string error\n", __FUNCTION__);
				ret = 0;
			}
			tmp_name[ret] = '\0';
			if(num_flag == FALSE)
				strcat( callListBuf, ", , " );
			sprintf( callListTmpBuf, ", \"%s\"", tmp_name );
			strcat( callListBuf, callListTmpBuf );
		}

		for (j = 0; (j < NUM_OF_TEL_ATP) && !bFound; j++)
		{
			if (gCurrentCallsList.callInfo[i].CallId == gCurrentCallsList.currCall[j].currCallId)
			{
				gCurrentCallsList.callInfo[i].reqHandle = gCurrentCallsList.currCall[j].currHandle;
				gCurrentCallsList.currCall[j].currCallId = 0;
				gCurrentCallsList.currCall[j].currHandle = INVALID_REQ_HANDLE;
				bFound = TRUE;
				break;
			}
		}

		/* this is result of call list request -> must display result */
//        if ( ccCnf == TRUE )
		{
			ATRESP( reqHandle, 0, 0, callListBuf);
		}

	}
	F_LEAVE();
}

static int callId2callIdx(CiCcCallId CallId)
{
	int i;
	for ( i = 0; i < gCurrentCallsList.NumCalls; i++ )
	{
		if ( gCurrentCallsList.callInfo[i].CallId == CallId )
		{
			return i;
		}
	}
	return -1;
}

/************************************************************************************
 * FUNCTION: updateCallerInfo
 *
 * PARAMETERS:
 *
 * DESCRIPTION:    updates caller informations in the CC calls list
 *
 * RETURNS
 *************************************************************************************/
static void updateCallerInfo(CiCcCallId CallId, CiCcCliValidity CliValidity, const CiCallerInfo *callerInfo)
{
	AtCiCcCallInfo *pCallInfo = NULL;
	int callIdx = -1;
	F_ENTER();

	callIdx = callId2callIdx(CallId);

	if(callIdx == -1)
	{
		DBGMSG("%s: wrong call id: %d\n", __FUNCTION__, CallId);
		F_LEAVE();
		return;
	}

	pCallInfo = &(gCurrentCallsList.callInfo[callIdx]);

	if (( CliValidity == CLI_VALIDITY_VALID ) &&
		( callerInfo->OptCallerNumber.Present == TRUE ) && ( callerInfo->OptCallerNumber.AddressInfo.Length > 0 ))
	{
		pCallInfo->AddrType = callerInfo->OptCallerNumber.AddressInfo.AddrType;

		memcpy( pCallInfo->Digits, callerInfo->OptCallerNumber.AddressInfo.Digits,
				callerInfo->OptCallerNumber.AddressInfo.Length );
		pCallInfo->Digits[callerInfo->OptCallerNumber.AddressInfo.Length] = 0;
		pCallInfo->SsNumberInfoReceived = TRUE;
	}

	if ( callerInfo->OptCallerName.Present == TRUE )
	{
		pCallInfo->Name = callerInfo->OptCallerName;
		pCallInfo->SsNameInfoReceived = TRUE;
	}

	F_LEAVE();
}


/************************************************************************************
 * FUNCTION: convertCIbstSpeed2AT
 *
 * PARAMETERS:     CIbstSpeed - BST speed as defined in CI
 *
 * DESCRIPTION:    Converts BST speed parameter from CI enum type into value from 27.007
 *
 * RETURNS: BST speed as defined in 3GPP 27.007
 *************************************************************************************/
static BOOL convertCIbstSpeed2AT( CiBsTypeSpeed CIbstSpeed, INT32 *ATbstSpeed )
{
	BOOL isValid = TRUE;

	if ( CIbstSpeed <= CI_BSTYPE_SPEED_9600_V32 )
	{
		/* These values are the same - no conversion needed */
		*ATbstSpeed = CIbstSpeed;
	}
	else
	{
		/* The rest of the numbers are very irregular, so do on case-by-case basis */
		switch (CIbstSpeed)
		{
		case CI_BSTYPE_SPEED_9600_V34:
			*ATbstSpeed = 12;
			break;
		case CI_BSTYPE_SPEED_14400_V34:
			*ATbstSpeed = 14;
			break;
		case CI_BSTYPE_SPEED_19200_V34:
			*ATbstSpeed = 15;
			break;
		case CI_BSTYPE_SPEED_28800_V34:
			*ATbstSpeed = 16;
			break;
		case CI_BSTYPE_SPEED_33600_V34:
			*ATbstSpeed = 17;
			break;
		case CI_BSTYPE_SPEED_1200_V120:
			*ATbstSpeed = 34;
			break;
		case CI_BSTYPE_SPEED_2400_V120:
			*ATbstSpeed = 36;
			break;
		case CI_BSTYPE_SPEED_4800_V120:
			*ATbstSpeed = 38;
			break;
		case CI_BSTYPE_SPEED_9600_V120:
			*ATbstSpeed = 39;
			break;
		case CI_BSTYPE_SPEED_14400_V120:
			*ATbstSpeed = 43;
			break;
		case CI_BSTYPE_SPEED_19200_V120:
			*ATbstSpeed = 47;
			break;
		case CI_BSTYPE_SPEED_28800_V120:
			*ATbstSpeed = 48;
			break;
		case CI_BSTYPE_SPEED_38400_V120:
			*ATbstSpeed = 49;
			break;
		case CI_BSTYPE_SPEED_48000_V120:
			*ATbstSpeed = 50;
			break;
		case CI_BSTYPE_SPEED_56000_V120:
			*ATbstSpeed = 51;
			break;
		case CI_BSTYPE_SPEED_300_V110:
			*ATbstSpeed = 65;
			break;
		case CI_BSTYPE_SPEED_1200_V110:
			*ATbstSpeed = 66;
			break;
		case CI_BSTYPE_SPEED_2400_V110_X31:
			*ATbstSpeed = 68;
			break;
		case CI_BSTYPE_SPEED_4800_V110_X31:
			*ATbstSpeed = 70;
			break;
		case CI_BSTYPE_SPEED_9600_V110_X31:
			*ATbstSpeed = 71;
			break;
		case CI_BSTYPE_SPEED_14400_V110_X31:
			*ATbstSpeed = 75;
			break;
		case CI_BSTYPE_SPEED_19200_V110_X31:
			*ATbstSpeed = 79;
			break;
		case CI_BSTYPE_SPEED_28800_V110_X31:
			*ATbstSpeed = 80;
			break;
		case CI_BSTYPE_SPEED_38400_V110_X31:
			*ATbstSpeed = 81;
			break;
		case CI_BSTYPE_SPEED_48000_V110_X31:
			*ATbstSpeed = 82;
			break;
		case CI_BSTYPE_SPEED_56000_V110_X31:
			*ATbstSpeed = 83;
			break;
		case CI_BSTYPE_SPEED_64000_X31:
			*ATbstSpeed = 84;
			break;
		case CI_BSTYPE_SPEED_56000_BTM:
			*ATbstSpeed = 115;
			break;
		case CI_BSTYPE_SPEED_64000_BTM:
			*ATbstSpeed = 116;
			break;
		case CI_BSTYPE_SPEED_32000_PIAFS:
			*ATbstSpeed = 120;
			break;
		case CI_BSTYPE_SPEED_64000_PIAFS:
			*ATbstSpeed = 121;
			break;
		case CI_BSTYPE_SPEED_28800_MM:
			*ATbstSpeed = 130;
			break;
		case CI_BSTYPE_SPEED_32000_MM:
			*ATbstSpeed = 131;
			break;
		case CI_BSTYPE_SPEED_33600_MM:
			*ATbstSpeed = 132;
			break;
		case CI_BSTYPE_SPEED_56000_MM:
			*ATbstSpeed = 133;
			break;
		case CI_BSTYPE_SPEED_64000_MM:
			*ATbstSpeed = 134;
			break;

		default:
			isValid = FALSE;
			break;
		}
	}
	return isValid;
}

/************************************************************************************
 * FUNCTION: checkCcRet
 *
 * PARAMETERS:
 *
 * DESCRIPTION:    prints appropriate message based on result code
 *
 * RETURNS:
 *************************************************************************************/
static void checkCcRet( CiRequestHandle reqHandle, CiCcResultCode result )
{
	DBGMSG("[%s] Line(%d): result(%d) \n", __FUNCTION__, __LINE__, result);
	switch ( result )
	{
	case CIRC_CC_SUCCESS:               /* Request completed successfully         */
		ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;

	case CIRC_CC_SIM_ACCESS_DENIED:     /* SIM access related error (CHV needed?) */
		ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_SIM_PIN_REQUIRED, NULL);
		break;

	case CIRC_CC_NO_SERVICE:            /* No Network service                     */
		ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_NO_NW_SERVICE, NULL);
		break;

	case CIRC_CC_TIMEOUT:              /* Request timed out                      */
		ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_NW_TIMEOUT, NULL);
		break;

	case CIRC_CC_EMERGENCY_ONLY:       /* Only Emergency Calls allowed           */
		ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_NW_NOT_ALLOWED, NULL);
		break;

	case CIRC_CC_BAD_DIALSTRING:       /* Invalid character(s) in Dial String    */
		ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_CHAR_IN_DIAL_STRING, NULL);
		break;

	case CIRC_CC_INVALID_CALLID:       /* Invalid Call Identifier                */
		ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_INDEX, NULL);
		break;

	case CIRC_CC_FDN_ONLY:                  /* Only Fixed Dialing Numbers allowed     */
#ifdef AT_CUSTOMER_HTC
	{
		char ccCnfBuf[15];
		sprintf(ccCnfBuf, "@HTCDIS:%02X", AT_HTCDIS_CAUSE_CC_FDN_REJECT);
		ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, ccCnfBuf);
		break;
	}
#endif
	case CIRC_CC_INCOMPLETE_INFO:           /* Incomplete information for request     */
	case CIRC_CC_INVALID_ADDRESS:           /* Invalid Address (Phone Number)         */
	case CIRC_CC_INVALID_MPTYID:            /* Invalid MPTY Identifier                */
	case CIRC_CC_CALL_BARRED:               /* Calls are barred                       */
	case CIRC_CC_NO_MORE_CALLS:             /* No more calls allowed                  */
	case CIRC_CC_NO_MORE_TIME:              /* No more Airtime left                   */
	case CIRC_CC_NOT_PROVISIONED:           /* Service not provisioned                */
	case CIRC_CC_CANNOT_SWITCH:             /* Call Mode cannot be switched           */
	case CIRC_CC_SWITCH_FAILED:             /* Failed to switch Call Mode             */
	case CIRC_CC_REJECTED:                  /* Request rejected by Network            */
	case CIRC_CC_FAIL:                      /* General Failure (catch-all)            */
	default:
		ATRESP(reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		break;
	}

	return;
}

/************************************************************************************
 * FUNCTION: getCcIndCauseString
 *
 * PARAMETERS:
 *
 * DESCRIPTION:    get the failure string for the cause in the indication message
 *
 * RETURNS:
 *************************************************************************************/
static void getCcIndCauseString( CiCcCause cause, char *sBuf )
{
	char *tBuf;

	switch (cause)
	{
	case CICC_CAUSE_UNKNOWN:                   /* Unknown cause (includes "None")  */
		tBuf = "0 Unknown";
		break;
	case CICC_CAUSE_UNASSIGNED_NUMBER:         /* Unassigned (unallocated) number  */
		tBuf = "1 Unassigned number";
		break;
	case CICC_CAUSE_NO_ROUTE_TO_DEST:          /* No route to Destination  */
		tBuf = "3 No route to destination";
		break;
	case CICC_CAUSE_CHAN_UNACCEPTABLE:         /* Channel unacceptable */
		tBuf = "6 Channel unacceptable";
		break;
	case CICC_CAUSE_OPERATOR_BARRING:          /* Operator determined barring  */
		tBuf = "8 Operator determined barring";
		break;
	case CICC_CAUSE_NORMAL_CLEARING:           /* Normal call clearing */
		tBuf = "16 Normal call clearing";
		break;
	case CICC_CAUSE_USER_BUSY:                 /* User busy  */
		tBuf = "17 User busy";
		break;
	case CICC_CAUSE_NO_USER_RESPONSE:          /* No user responding */
		tBuf = "18 No user responding";
		break;
	case CICC_CAUSE_ALERT_NO_ANSWER:           /* User alerting: no answer */
		tBuf = "19 User alerting: no answer";
		break;
	case CICC_CAUSE_CALL_REJECTED:             /* Call rejected  */
		tBuf = "21 Call rejected";
		break;
	case CICC_CAUSE_NUMBER_CHANGED:            /* Number changed */
		tBuf = "22 Number changed";
		break;
	case CICC_CAUSE_PREEMPTION:                /* Pre-emption  */
		tBuf = "25 Preemption";
		break;
	case CICC_CAUSE_NONSELECTED_USER_CLEAR:    /* Non selected user clearing */
		tBuf = "26 Non selected user clearing";
		break;
	case CICC_CAUSE_DEST_OUT_OF_ORDER:         /* Destination out of order */
		tBuf = "27 Destination out of order";
		break;
	case CICC_CAUSE_INVALID_NUMFORMAT:         /* Invalid number format (incomplete) */
		tBuf = "28 Invalid number format";
		break;
	case CICC_CAUSE_FACILITY_REJECT:           /* Facility rejected  */
		tBuf = "29 Facility rejected";
		break;
	case CICC_CAUSE_STATUSENQ_RESPONSE:        /* Response to STATUS ENQUIRY */
		tBuf = "30 Response to STATUS ENQUIRY";
		break;
	case CICC_CAUSE_NORMAL_UNSPECIFIED:        /* Normal: unspecified  */
		tBuf = "31 Normal: unspecified";
		break;
	case CICC_CAUSE_NO_CCT_AVAILABLE:          /* No circuit/channel available */
		tBuf = "34 No circuit/channel available";
		break;
	case CICC_CAUSE_NETWORK_OUT_OF_ORDER:      /* Network out of order */
		tBuf = "38 Network out of order";
		break;
	case CICC_CAUSE_TEMP_FAILURE:              /* Temporary failure  */
		tBuf = "41 Temporary failure";
		break;
	case CICC_CAUSE_CONGESTION:                /* Switching equipment congestion */
		tBuf = "42 Switching equipment congestion";
		break;
	case CICC_CAUSE_ACCESSINFO_DISCARDED:      /* Access information discarded */
		tBuf = "43 Access information discarded";
		break;
	case CICC_CAUSE_CIRCUIT_UNAVAILABLE:       /* Requested circuit/channel unavailable  */
		tBuf = "44 Requested circuit/channel unavailable";
		break;
	case CICC_CAUSE_RESOURCES_UNAVAILABLE:     /* Resources unavailable: unspecified */
		tBuf = "47 Resource unavailable";
		break;
	case CICC_CAUSE_QOS_UNAVAIL:               /* Quality of Service (QoS) unavailable */
		tBuf = "49 QoS unavailable";
		break;
	case CICC_CAUSE_FACILITY_NOTSUBSCRIBED:    /* Requested facility not subscribed  */
		tBuf = "50 Requested facility not subscribed";
		break;
	case CICC_CAUSE_MT_CALLBARRING_IN_CUG:     /* Incoming (MT) calls barred within CUG  */
		tBuf = "55 Incoming calls barred within CUG";
		break;
	case CICC_CAUSE_BEARERCAP_NOTAUTHORIZED:   /* Bearer capability not authorized */
		tBuf = "57 Bearer capability not authorized";
		break;
	case CICC_CAUSE_BEARERCAP_UNAVAILABLE:     /* Bearer capability not available  */
		tBuf = "58 Bearer capability not available";
		break;
	case CICC_CAUSE_SVC_UNAVAILABLE:           /* Service or option not available  */
		tBuf = "63 Service not available";
		break;
	case CICC_CAUSE_BEARERSVC_NOT_IMPLEMENTED: /* Bearer service not implemented */
		tBuf = "65 Bearer service not implemented";
		break;
	case CICC_CAUSE_ACMMAX_REACHED:            /* ACM equal to: or greater than: ACMmax  */
		tBuf = "68 ACM MAX reached";
		break;
	case CICC_CAUSE_FACILITY_NOT_IMPLEMENTED:  /* Requested facility not implemented */
		tBuf = "69 Facility not implemented";
		break;
	case CICC_CAUSE_BEARERCAP_RDI_ONLY:        /* Only RDI bearer capability is available  */
		tBuf = "70 Only RDI bearer capability available";
		break;
	case CICC_CAUSE_SVC_NOT_IMPLEMENTED:       /* Service or option not implemented  */
		tBuf = "79 Service not implemented";
		break;
	case CICC_CAUSE_INVALID_TRANSACTID:        /* Invalid transaction ID value */
		tBuf = "81 Invalid transaction ID";
		break;
	case CICC_CAUSE_NOT_CUG_MEMBER:            /* User not member of CUG */
		tBuf = "87 User not member of CUG";
		break;
	case CICC_CAUSE_DEST_INCOMPATIBLE:         /* Incompatible destination */
		tBuf = "88 Incompatible destination";
		break;
	case CICC_CAUSE_INCORRECT_MESSAGE:         /* Semantically incorrect message */
		tBuf = "95 Incorrect message";
		break;
	case CICC_CAUSE_TRANSIT_NETWORK_INVALID:   /* Invalid transit network selection  */
		tBuf = "91 Invalid transit network selection";
		break;
	case CICC_CAUSE_NO_SUCH_MSGTYPE:           /* Message type non-existent or not */
		tBuf = "97 Message type non-existent";
		break;
	/*   implemented  */
	case CICC_CAUSE_MSGTYPE_WRONG_STATE:       /* Message type incompatible with current */
		tBuf = "98 Message type wrong state";
		break;
	/*   protocol state */
	case CICC_CAUSE_NO_SUCH_IE:                /* Information element non-existent or not  */
		tBuf = "99 Information element not-existent";
		break;
	/*   implemented  */
	case CICC_CAUSE_CONDITIONAL_IE_ERROR:      /* Conditional IE error */
		tBuf = "100 Conditional IE error";
		break;
	case CICC_CAUSE_MSG_WRONG_STATE:           /* Message incompatible with current  */
		tBuf = "101 Message wrong state";
		break;
	/*   protocol state */
	case CICC_CAUSE_RECOVERY_AFTER_TIMEOUT:    /* Recovery after timer expiry  */
		tBuf = "102 Recovery after timer expiry";
		break;
	case CICC_CAUSE_PROTOCOL_ERROR:            /* Protocol error: unspecified  */
		tBuf = "111 Protocol error: unspecified";
		break;
	case CICC_CAUSE_ACCESS_CLASS_BARRED:       /*Cell barred: private cause, not defined in 24.008*/
		tBuf = "224 Call barring";
		break;
	case CICC_CAUSE_FDN_BLOCKED:
		tBuf = "241 FDN Blocked";
		break;
	default:              /* Interworking: unspecified  */
		tBuf = "127 Interworking: unspecified";
		break;
	}

	strcpy(sBuf, tBuf);
	DPRINTF("%s: %s!\n", __FUNCTION__, sBuf);

	return;
}



/************************************************************************************
 * FUNCTION: ccCnf
 *
 * PARAMETERS:
 *
 * DESCRIPTION:    process CC confirmations
 *
 * RETURNS:
 *************************************************************************************/
void ccCnf(CiSgOpaqueHandle opSgCnfHandle,
	   CiServiceGroupID svgId,
	   CiPrimitiveID primId,
	   CiRequestHandle reqHandle,
	   void*            paras)
{
	UNUSEDPARAM(opSgCnfHandle)
	INT8 i;
	INT32 atBbstVal;
	CiCcPrimMakeCallCnf                   *makeCallCnf;
	CiCcPrimReleaseCallCnf                *releaseCallCnf;
	CiCcPrimRefuseCallCnf                 *refuseCallCnf;
	CiCcPrimReleaseAllCallsCnf               *releaseAllCallsCnf;
	CiCcPrimAnswerCallCnf                 *answerCallCnf;
	CiCcPrimGetAcmValueCnf                *acmValCnf;
	CiCcPrimGetAcmMaxValueCnf             *acmMaxCnf;
	CiCcPrimHoldCallCnf                   *holdCallCnf;
	CiCcPrimRetrieveCallCnf               *retrieveCallCnf;
	CiCcPrimGetSupportedDataBsTypesCnf    *getSupportedBsTypeCnf;
	CiCcPrimGetDataBsTypeCnf              *getBsTypeCnf;
	CiCcPrimSetDataBsTypeCnf              *setBsTypeCnf;
	CiCcPrimManipulateCallsCnf            *manipCallCnf;
	CiCcPrimListCurrentCallsCnf           *listCurCallsCnf;
	CiCcPrimResetAcmValueCnf              *resetAcmValCnf;
	CiCcPrimSetAcmMaxValueCnf             *setAcmmCnf;
	CiCcPrimGetCallModeCnf                *getCallModeCnf;
	CiCcPrimSetCallModeCnf                *setCallModeCnf;
	CiCcPrimGetSupportedCallModesCnf      *getCallSuppModesCnf;
	CiCcPrimGetCcmValueCnf                *getCcmValueCnf;
	CiCcPrimSetDtmfPacingCnf           *setDTMFPacingCnf;
	CiCcPrimGetDtmfPacingCnf               *getDTMFPacingCnf;
	CiCcPrimSendDtmfStringCnf          *sendDTMFStrCnf;
	CiCcPrimGetRlpCapCnf                  *rlpCapCnf;
	CiCcPrimGetRlpCfgCnf                  *getRlpCfgCnf;
	CiCcPrimGetDataCompCapCnf             *getDataCompCapCnf;
	CiCcPrimGetDataCompCnf                *getDataCompCnf;
	CiCcPrimSetDataCompCnf                *setDataCompCnf;
	CiCcPrimGetNumberTypeCnf              *getNumberTypeCnf;
	CiCcPrimSetNumberTypeCnf              *setNumberTypeCnf;
	CiCcPrimGetPuctInfoCnf					   *getPuctInfoCnf;
	CiCcPrimSetPuctInfoCnf							  *setPuctInfoCnf;

	INT32 rlpVer[2];
	char ccRspBuf[300];
	char ccRspTmpBuf[300];

	TelAtParserID sAtpIndex;
	sAtpIndex = GET_ATP_INDEX(reqHandle);
	DBGMSG("%s: reqhandle = %d, primId(%d).\n", __FUNCTION__, reqHandle, primId);

	/*
	**  Determine the primitive being confirmed.
	*/
	switch (primId)
	{
	case CI_CC_PRIM_RESET_ACM_VALUE_CNF:
	{
		resetAcmValCnf = (CiCcPrimResetAcmValueCnf *)paras;
		checkCcRet( reqHandle, resetAcmValCnf->Result );
		break;
	}

	case CI_CC_PRIM_SET_ACMMAX_VALUE_CNF:
	{
		setAcmmCnf = (CiCcPrimSetAcmMaxValueCnf *)paras;
		checkCcRet( reqHandle, setAcmmCnf->Result );
		break;
	}

	case CI_CC_PRIM_MAKE_CALL_CNF:
	{
		makeCallCnf = (CiCcPrimMakeCallCnf *)paras;
		DBGMSG("[%s] Line(%d): makeCallCnf->Result(%d) \n", __FUNCTION__, __LINE__, makeCallCnf->Result);

		if ( makeCallCnf->Result == CIRC_CC_SUCCESS )
		{
			/* vcy OK is printed here, not on connected indication */
			DBGMSG("Received CI_CC_PRIM_MAKE_CALL_CNF: Call ID = %d\r\n", makeCallCnf->CallId);

			gCurrentCallsList.currCall[GET_ATP_INDEX(reqHandle)].currCallId = makeCallCnf->CallId;
			gCurrentCallsList.currCall[GET_ATP_INDEX(reqHandle)].currHandle = reqHandle;
			// currBasicMode is a unsigned char here, no need to compare with 0
			if (gCurrentCallsList.currCall[GET_ATP_INDEX(reqHandle)].currBasicMode < CICC_NUM_BASIC_CMODES)
			{
				// Johnny- AT command server response earlier, so that it can handle next AT command
				ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL );
				/* When starting a new call, the MUTE SETTING should be off */
				tel_init_voicecall_mute();
				if (bLocalTest)
				{
					MACRO_START_VOICE_CALL();
				}
			}
		}
		else
		{
			// need to update Cause Value if MO fail, APP need to know it in some cases
			if(makeCallCnf->Result == CIRC_CC_FDN_ONLY)
				getCcIndCauseString(CICC_CAUSE_FDN_BLOCKED, gErrCauseBuf);
			else
				getCcIndCauseString(CICC_CAUSE_UNKNOWN, gErrCauseBuf);
			
			checkCcRet( reqHandle, makeCallCnf->Result );
			
		}
		break;
	}

	case CI_CC_PRIM_RELEASE_CALL_CNF:
	{

		releaseCallCnf = (CiCcPrimReleaseCallCnf *)paras;
		/* vcy OK is printed here, not on disconnect indication */
		if ( releaseCallCnf->Result != CIRC_CC_SUCCESS )
		{
			checkCcRet( reqHandle, releaseCallCnf->Result );
		}
		else
		{
			//gCurCallReleaseReqHandle = reqHandle;

			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);

		}
		resetCurrCall(GET_ATP_INDEX(reqHandle));
		break;
	}
	case CI_CC_PRIM_REFUSE_CALL_CNF:
	{

		refuseCallCnf = (CiCcPrimRefuseCallCnf *)paras;
		/* vcy OK is printed here, not on disconnect indication */
		if ( refuseCallCnf->Result != CIRC_CC_SUCCESS )
		{
			checkCcRet( reqHandle, refuseCallCnf->Result );
		}
		else
		{
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);

		}
		resetCurrCall(GET_ATP_INDEX(reqHandle));
		break;
	}
	case CI_CC_PRIM_RELEASE_ALL_CALLS_CNF:
	{

		releaseAllCallsCnf = (CiCcPrimReleaseAllCallsCnf *)paras;
		/* vcy OK is printed here, not on disconnect indication */
		if ( releaseAllCallsCnf->Result != CIRC_CC_SUCCESS )
		{
			checkCcRet( reqHandle, releaseAllCallsCnf->Result );
		}
		else
		{

			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL );

		}
		initCCContext();
		break;
	}


	case CI_CC_PRIM_ANSWER_CALL_CNF:
	{

		answerCallCnf = (CiCcPrimAnswerCallCnf *)paras;
		if ( answerCallCnf->Result != CIRC_CC_SUCCESS )
		{
			checkCcRet( reqHandle, answerCallCnf->Result );
		}
		else
		{
			//handle in CONNECT_IND
			gCurrentCallsList.currCall[GET_ATP_INDEX(reqHandle)].currCallId = answerCallCnf->CallId;
			gCurrentCallsList.currCall[GET_ATP_INDEX(reqHandle)].currHandle = reqHandle;
			// Johnny- AT command server response earlier, so that it can handle next AT command
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL );
			if (bLocalTest)
			{
				DBGMSG("Enable audio path\n");
				MACRO_START_VOICE_CALL();
			}
		}
		break;
	}

	case CI_CC_PRIM_GET_ACM_VALUE_CNF:
	{
		acmValCnf = (CiCcPrimGetAcmValueCnf *)paras;
		if ( acmValCnf->Result == CIRC_CC_SUCCESS )
		{
			/*
			**  Display the accumulator value. TS 27.007 ">: string type; three bytes of the current
			**  call meter value in hexadecimal format (e.g. "00001E" indicates decimal value 30);"
			*/
			sprintf(ccRspBuf, "+CACM: \"%06x\"", acmValCnf->Acm);
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ccRspBuf);
		}
		else
		{
			checkCcRet( reqHandle, acmValCnf->Result );
		}

		break;
	}

	case CI_CC_PRIM_GET_ACMMAX_VALUE_CNF:
	{
		acmMaxCnf = (CiCcPrimGetAcmMaxValueCnf *)paras;
		if ( acmMaxCnf->Result == CIRC_CC_SUCCESS )
		{
			/*
			**  Display the accumulator value.
			*/
			sprintf(ccRspBuf, "+CAMM: \"%06x\"", acmMaxCnf->AcmMax);
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ccRspBuf);
		}
		else
		{
			checkCcRet( reqHandle, acmMaxCnf->Result );
		}
		break;
	}

	case CI_CC_PRIM_HOLD_CALL_CNF:
	{
		holdCallCnf = (CiCcPrimHoldCallCnf *)paras;
		checkCcRet( reqHandle, holdCallCnf->Result );
		break;
	}

	case CI_CC_PRIM_RETRIEVE_CALL_CNF:
	{
		retrieveCallCnf = (CiCcPrimRetrieveCallCnf *)paras;
		checkCcRet( reqHandle, retrieveCallCnf->Result );
		break;
	}

	case CI_CC_PRIM_GET_DATA_BSTYPE_CNF:
	{
		getBsTypeCnf = (CiCcPrimGetDataBsTypeCnf *)paras;
		if ( getBsTypeCnf->Result == CIRC_CC_SUCCESS )
		{
			convertCIbstSpeed2AT( getBsTypeCnf->info.Speed, &atBbstVal);
			/*
			**  Display the accumulator value.
			*/
			sprintf( ccRspBuf, "+CBST: %d,%d,%d", atBbstVal,
				 getBsTypeCnf->info.Name,
				 getBsTypeCnf->info.Ce );
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ccRspBuf);
		}
		else
		{
			checkCcRet( reqHandle, getBsTypeCnf->Result );
		}
		break;
	}

	case CI_CC_PRIM_GET_SUPPORTED_DATA_BSTYPES_CNF:
	{
		getSupportedBsTypeCnf = (CiCcPrimGetSupportedDataBsTypesCnf *)paras;
		if ( getSupportedBsTypeCnf->Result == CIRC_CC_SUCCESS )
		{
			/* NOTE: the CI - SAC doesn't report an actual list of suppported types, coded as per spec. */
			/* for now use the values as defined by TTPCom */
			sprintf( ccRspBuf, "+CBST: (" );

			for ( i = 0; i < CI_NUM_BSTYPE_SPEEDS - 1; i++ )
			{
				convertCIbstSpeed2AT( i, &atBbstVal);
				sprintf( ccRspTmpBuf, "%d,", atBbstVal );
				strcat( ccRspBuf, ccRspTmpBuf );
			}

			/* last entry */
			convertCIbstSpeed2AT( i, &atBbstVal );
			sprintf( ccRspTmpBuf, "%d),(0-7),(0-3)", atBbstVal );
			strcat(ccRspBuf, ccRspTmpBuf);
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ccRspBuf);
		}
		else
		{
			checkCcRet( reqHandle, getSupportedBsTypeCnf->Result );
		}
		break;
	}

	case CI_CC_PRIM_SET_DATA_BSTYPE_CNF:
	{
		setBsTypeCnf = (CiCcPrimSetDataBsTypeCnf *)paras;
		checkCcRet( reqHandle, setBsTypeCnf->Result );
		break;
	}

	case CI_CC_PRIM_GET_CALLMODE_CNF:
	{
		getCallModeCnf = (CiCcPrimGetCallModeCnf *)paras;
		if ( getCallModeCnf->Result == CIRC_CC_SUCCESS )
		{
			/*
			**  Display the accumulator value.
			*/
			sprintf( ccRspBuf, "+CMOD: %d", getCallModeCnf->Mode );
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ccRspBuf);
		}
		else
		{
			checkCcRet( reqHandle, getCallModeCnf->Result );
		}
		break;
	}

	case CI_CC_PRIM_GET_SUPPORTED_CALLMODES_CNF:
	{
		getCallSuppModesCnf = (CiCcPrimGetSupportedCallModesCnf *)paras;
		if ( getCallSuppModesCnf->Result == CIRC_CC_SUCCESS )
		{
			/*
			**  Display the accumulator value.
			*/
			sprintf( ccRspBuf, "+CMOD: %d", getCallSuppModesCnf->Modes[0] );
			ATRESP( reqHandle, 0, 0, ccRspBuf);

			for (i = 1; i < getCallSuppModesCnf->NumModes; i++ )
			{
				sprintf( ccRspBuf, ",%d", getCallSuppModesCnf->Modes[i] );
				ATRESP( reqHandle, 0, 0, ccRspBuf);
			}
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
		else
		{
			checkCcRet( reqHandle, getCallSuppModesCnf->Result );
		}
		break;
	}

	case CI_CC_PRIM_SET_CALLMODE_CNF:
	{
		setCallModeCnf = (CiCcPrimSetCallModeCnf *)paras;
		checkCcRet( reqHandle, setCallModeCnf->Result );
		break;
	}

	case CI_CC_PRIM_MANIPULATE_CALLS_CNF:
	{
		manipCallCnf = (CiCcPrimManipulateCallsCnf *)paras;
		checkCcRet( reqHandle, manipCallCnf->Result );
		break;
	}

	case CI_CC_PRIM_LIST_CURRENT_CALLS_CNF:
	{
		listCurCallsCnf = (CiCcPrimListCurrentCallsCnf *)paras;
		if ( listCurCallsCnf->Result == CIRC_CC_SUCCESS )
		{
			/* update the current calls list */
			updateCallList( reqHandle, listCurCallsCnf->NumCalls, listCurCallsCnf->callInfo, TRUE);

			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
		else
		{
			checkCcRet( reqHandle, listCurCallsCnf->Result );
		}

		break;
	}

	case CI_CC_PRIM_GET_CCM_VALUE_CNF:
	{
		getCcmValueCnf = (CiCcPrimGetCcmValueCnf *)paras;
		if ( getCcmValueCnf->Result == CIRC_CC_SUCCESS )
		{
			sprintf( ccRspBuf, "+CAOC:\"%06X\"", getCcmValueCnf->Ccm );
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ccRspBuf);
		}
		else
		{
			checkCcRet( reqHandle, getCcmValueCnf->Result );
		}
		break;
	}

	case CI_CC_PRIM_GET_DTMF_PACING_CNF:
	{
		getDTMFPacingCnf = (CiCcPrimGetDtmfPacingCnf*)paras;
		if ( getDTMFPacingCnf->Result == CIRC_CC_SUCCESS )
		{
			sprintf( ccRspBuf, "+VTD: %d", getDTMFPacingCnf->Pacing.Duration);
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ccRspBuf);
		}
		else
		{
			checkCcRet( reqHandle, getDTMFPacingCnf->Result );
		}
		break;
	}

	case CI_CC_PRIM_SET_DTMF_PACING_CNF:
		setDTMFPacingCnf = (CiCcPrimSetDtmfPacingCnf*)paras;
		if ( setDTMFPacingCnf->Result == CIRC_CC_SUCCESS )
		{
			if(GET_REQ_ID(reqHandle)== CI_CC_PRIM_SEND_DTMF_STRING_REQ){
				CiCcPrimSendDtmfStringReq *sendDTMFStrReq = utlCalloc(1, sizeof(*sendDTMFStrReq));
				if (sendDTMFStrReq == NULL) {
					ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
					break;
				}
				sendDTMFStrReq->CallId = 0;
				memcpy(sendDTMFStrReq->digits, currentSentDTMFStr, sizeof(sendDTMFStrReq->digits));
				int ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC],
					     CI_CC_PRIM_SEND_DTMF_STRING_REQ, reqHandle, (void*)sendDTMFStrReq);

				if (ret != CIRC_SUCCESS)
				{
					ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
				}
			}
			else ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
		else
		{
			checkCcRet( reqHandle, setDTMFPacingCnf->Result );
		}
		break;

	case CI_CC_PRIM_SEND_DTMF_STRING_CNF:
		sendDTMFStrCnf = (CiCcPrimSendDtmfStringCnf *)paras;
		checkCcRet( reqHandle, sendDTMFStrCnf->Result );
		break;

	case CI_CC_PRIM_GET_RLP_CAP_CNF:
	{
		rlpCapCnf = (CiCcPrimGetRlpCapCnf*)paras;
		if ( rlpCapCnf->Result == CIRC_CC_SUCCESS )
		{
			/* Find version range first */
			rlpVer[0] = -1;
			for ( i = 0; i < (INT8)(8 * sizeof(UINT32)); i++ )
			{
				if ( (rlpVer[0] == -1) && (rlpCapCnf->cap.bitsVer & 1) )
				{
					rlpVer[0] = i;
					rlpVer[1] = i;
				}
				else if ( rlpCapCnf->cap.bitsVer & 1 )
				{
					rlpVer[1] = i;
				}
				rlpCapCnf->cap.bitsVer = rlpCapCnf->cap.bitsVer >> 1;
			}

			sprintf( ccRspBuf, "+CRLP:(%d-%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d)",
				 rlpCapCnf->cap.winIWSRange.min,       rlpCapCnf->cap.winIWSRange.max,
				 rlpCapCnf->cap.winMWSRange.min,       rlpCapCnf->cap.winMWSRange.max,
				 rlpCapCnf->cap.ackTimerRange.min,     rlpCapCnf->cap.ackTimerRange.max,
				 rlpCapCnf->cap.reTxAttemptsRange.min, rlpCapCnf->cap.reTxAttemptsRange.max,
				 rlpVer[0],                              rlpVer[1],
				 rlpCapCnf->cap.reSeqPeriodRange.min,  rlpCapCnf->cap.reSeqPeriodRange.max );
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ccRspBuf);
		}
		else
		{
			checkCcRet( reqHandle, rlpCapCnf->Result );
		}
		break;
	}

	case CI_CC_PRIM_GET_RLP_CFG_CNF:
	{
		getRlpCfgCnf = (CiCcPrimGetRlpCfgCnf*)paras;
		if ( getRlpCfgCnf->Result == CIRC_CC_SUCCESS )
		{
			sprintf( ccRspBuf, "+CRLP:%d,%d,%d,%d,%d,%d",
				 getRlpCfgCnf->cfg.winIWS,   getRlpCfgCnf->cfg.winMWS,
				 getRlpCfgCnf->cfg.ackTimer, getRlpCfgCnf->cfg.reTxAttempts,
				 getRlpCfgCnf->cfg.ver,      getRlpCfgCnf->cfg.reSeqPeriod );
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, ccRspBuf);
		}
		else
		{
			checkCcRet( reqHandle, getRlpCfgCnf->Result );
		}
		break;
	}

	case CI_CC_PRIM_SET_RLP_CFG_CNF:
	{
		checkCcRet( reqHandle, ((CiCcPrimSetRlpCfgCnf*)paras)->Result );
		break;
	}


	case CI_ERR_PRIM_HASNOSUPPORT_CNF:
	case CI_ERR_PRIM_HASINVALIDPARAS_CNF:
	case CI_ERR_PRIM_ISINVALIDREQUEST_CNF:
	{
		/* Print "Operation not suported" error (CME error 4) */
		ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}
	case CI_CC_PRIM_GET_DATACOMP_CAP_CNF:
	{
		getDataCompCapCnf = (CiCcPrimGetDataCompCapCnf *)paras;

		if (getDataCompCapCnf->Result == CIRC_CC_SUCCESS)
		{
			UINT8 i;
			UINT32 bitsDirRange[2], bitsNegCompRange[2];

			bitsDirRange[0] = bitsNegCompRange[0] = -1;

			for (i = 0; getDataCompCapCnf->cap.bitsDir; i++)
			{
				if (getDataCompCapCnf->cap.bitsDir & 1)
				{
					if ((int)bitsDirRange[0] == -1)
						bitsDirRange[0] = i;

					bitsDirRange[1] = i;
				}

				getDataCompCapCnf->cap.bitsDir >>= 1;
			}

			for (i = 0; getDataCompCapCnf->cap.bitsNegComp; i++)
			{
				if (getDataCompCapCnf->cap.bitsNegComp & 1)
				{
					if ((int)bitsNegCompRange[0] == -1)
						bitsNegCompRange[0] = i;

					bitsNegCompRange[1] = i;
				}

				getDataCompCapCnf->cap.bitsNegComp >>= 1;
			}

			sprintf(ccRspBuf,
				"+DS:(%d-%d),(%d-%d),(%d-%d),(%d-%d)", bitsDirRange[0], bitsDirRange[1],
				bitsNegCompRange[0], bitsNegCompRange[1],
				getDataCompCapCnf->cap.maxDictRange.min,
				getDataCompCapCnf->cap.maxDictRange.max,
				getDataCompCapCnf->cap.maxStrLenRange.min,
				getDataCompCapCnf->cap.maxStrLenRange.max);
			ATRESP(reqHandle, ATCI_RESULT_CODE_OK, 0, ccRspBuf);
		}
		else
		{
			checkCcRet( reqHandle, getDataCompCapCnf->Result);
		}
		break;
	}

	case CI_CC_PRIM_GET_DATACOMP_CNF:
	{
		getDataCompCnf = (CiCcPrimGetDataCompCnf *)paras;

		if (getDataCompCnf->Result == CIRC_CC_SUCCESS)
		{
			sprintf(ccRspBuf, "%d,%d,%d,%d", getDataCompCnf->info.dir,
				getDataCompCnf->info.zNegRequired,
				getDataCompCnf->info.maxDict,
				getDataCompCnf->info.maxStrLen);
			ATRESP(reqHandle, ATCI_RESULT_CODE_OK, 0, ccRspBuf);
		}
		else
		{
			checkCcRet( reqHandle, getDataCompCnf->Result);
		}

		break;
	}
	case CI_CC_PRIM_SET_DATACOMP_CNF:
	{
		setDataCompCnf = (CiCcPrimSetDataCompCnf *)paras;

		checkCcRet( reqHandle, setDataCompCnf->Result);

		break;
	}
	case CI_CC_PRIM_GET_NUMBERTYPE_CNF: // AT+CSTA?
	{
		INT32 numbType;

		getNumberTypeCnf = (CiCcPrimGetNumberTypeCnf *)paras;

		if (getNumberTypeCnf->Result == CIRC_CC_SUCCESS)
		{
			numbType = DialNumTypeCI2GSM(getNumberTypeCnf->numType.NumType);
			sprintf(ccRspBuf, "+CSTA:%d", numbType);
			ATRESP(reqHandle, ATCI_RESULT_CODE_OK, 0, ccRspBuf);
		}
		else
		{
			checkCcRet( reqHandle, getNumberTypeCnf->Result);
		}
		break;
	}
	case CI_CC_PRIM_SET_NUMBERTYPE_CNF:  //AT+CSTA=
	{
		setNumberTypeCnf = (CiCcPrimSetNumberTypeCnf *)paras;

		checkCcRet( reqHandle, setNumberTypeCnf->Result);

		break;
	}

	case CI_CC_PRIM_GET_PUCT_INFO_CNF: //AT+CPUC?
	{
		char cpucCurrency[CICC_MAX_CURR_LENGTH * 4 + ATCI_NULL_TERMINATOR_LENGTH];
		getPuctInfoCnf = (CiCcPrimGetPuctInfoCnf*)paras;
		if( getPuctInfoCnf->Result == CIRC_CC_SUCCESS ){
			/* check the PUCT information is available or not */
			if( (getPuctInfoCnf->info.curr[0] == '\0') && (getPuctInfoCnf->info.eppu == 0) ){
				sprintf(ccRspBuf, "+CPUC: \"\",\"%f\"\r\n", (double)0);
				ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0,(char *)ccRspBuf );
			}
			else
			{
				double dPPU = 0;
				int ret;
				enum string_binary_type bin_type;
				if(getPuctInfoCnf->info.negExp){
					double dExp = (getPuctInfoCnf->info.exp)*(-1);
					dPPU =	(getPuctInfoCnf->info.eppu)*exp(dExp);
				}
				else
					dPPU =	(getPuctInfoCnf->info.eppu)*exp(getPuctInfoCnf->info.exp);

				if(chset_type == ATCI_CHSET_HEX)
					bin_type = STR_BIN_TYPE_HEX;
				else
					bin_type = STR_BIN_TYPE_GSM;

				if((ret = libConvertBinToCSCSString((const char *)getPuctInfoCnf->info.curr, CICC_MAX_CURR_LENGTH, bin_type, cpucCurrency, sizeof(cpucCurrency) / sizeof(cpucCurrency[0]) - 1, chset_type)) < 0)
				{
					ERRMSG("%s: convert string error\n", __FUNCTION__);
					cpucCurrency[0] = '\0';
					ret = 0;
				}

				cpucCurrency[ret] = '\0';
				sprintf(ccRspBuf, "+CPUC: \"%s\",\"%f\"\r\n", cpucCurrency, dPPU);

				ATRESP( reqHandle,		ATCI_RESULT_CODE_OK, 0, (char *)ccRspBuf );
								}
			}
		else
		{
			checkCcRet( reqHandle, getPuctInfoCnf->Result );
		}
		break;
	}
	case CI_CC_PRIM_SET_PUCT_INFO_CNF: //AT+CPUC=
	{
		setPuctInfoCnf = (CiCcPrimSetPuctInfoCnf*)paras;
		checkCcRet( reqHandle, setPuctInfoCnf->Result );
		break;
	}
	case CI_CC_PRIM_START_DTMF_CNF: //AT$VTS=<char>,1
	{
		CiCcPrimStartDtmfCnf *startDtmfCnf;
		startDtmfCnf = (CiCcPrimStartDtmfCnf*)paras;
		checkCcRet( reqHandle, startDtmfCnf->Result );
		break;
	}
	case CI_CC_PRIM_STOP_DTMF_CNF: //AT$VTS=<char>,0
	{
		CiCcPrimStopDtmfCnf *stopDtmfCnf;
		stopDtmfCnf = (CiCcPrimStopDtmfCnf*)paras;
		checkCcRet( reqHandle, stopDtmfCnf->Result );
		break;
	}
#ifdef AT_CUSTOMER_HTC
	case CI_CC_PRIM_SET_FW_LIST_CNF: //AT+FWDB
	{
		CiCcPrimSetFwListCnf *setFwListCnf;
		setFwListCnf = (CiCcPrimSetFwListCnf*)paras;
		checkCcRet( reqHandle, setFwListCnf->Result );
	}
#endif
	default:
		break;
	}

	/* free up the confirmation memory */
	atciSvgFreeCnfMem( svgId, primId, paras );

	return;
}


/************************************************************************************
 * FUNCTION: resetCurrCall
 *
 * PARAMETERS:
 *
 * DESCRIPTION:
 *
 * RETURNS:
 *************************************************************************************/

void resetCurrCall( UINT8 atpIdx )
{
	if (atpIdx < NUM_OF_TEL_ATP)
	{
		gCurrentCallsList.currCall[atpIdx].currBasicMode = CICC_NUM_BASIC_CMODES;
		gCurrentCallsList.currCall[atpIdx].currCallId = 0;
		gCurrentCallsList.currCall[atpIdx].currHandle = INVALID_REQ_HANDLE;
	}
}


/************************************************************************************
 * FUNCTION: resetAllCalls
 *
 * PARAMETERS:
 *
 * DESCRIPTION:
 *
 * RETURNS:
 *************************************************************************************/
void resetAllCalls( void )
{
	int i;

	for ( i = 0; i < gCurrentCallsList.NumCalls; i++ )
	{
		resetCallParam(gCurrentCallsList.callInfo[i].reqHandle, gCurrentCallsList.callInfo[i].CallId);
	}
	DTMFDuration = 0;
	g_iCRCMode = TEL_AT_CRC_MODE_VAL_DEFAULT;
}

/************************************************************************************
 * FUNCTION: resetCallParam
 *
 * PARAMETERS:
 *
 * DESCRIPTION:
 *
 * RETURNS:
 *************************************************************************************/

void resetCallParam( CiRequestHandle reqHandle, CiCcCallId callId )
{
	UNUSEDPARAM(callId)

	INT32 i;

	/* NOTE: the updated call list arrives before the disconnect indication ->
	 * NumCalls is 0 and the call will not be found; but the OldState is maintained
	 * so look for the call ID anyway */

	/* look for the call in the current list */
	//      for ( i = 0; i < gCurrentCallsList.NumCalls; i++ )
	DPRINTF("resetCallParm: reqHandle: %x, callId: %d\n", reqHandle, callId);
	for ( i = 0; i < CICC_MAX_CURRENT_CALLS; i++ )
	{
		if ( gCurrentCallsList.callInfo[i].CallId == callId )
		{
			DPRINTF("gCurrentCallsList.callInfo[i].reqHandle: %x\n", gCurrentCallsList.callInfo[i].reqHandle);
			DPRINTF("gCurrentCallsList.callInfo[i].OldMode: %d\n", gCurrentCallsList.callInfo[i].OldMode);
			DPRINTF("gCurrentCallsList.callInfo[i].OldState : %d\n", gCurrentCallsList.callInfo[i].OldState);
			if ( gCurrentCallsList.callInfo[i].OldMode == CICC_CURRENT_CMODE_VOICE )
			{
				if ( gCurrentCallsList.NumCalls == 0 )
				{
					//it may release voice path several times.
					if (bLocalTest)
						MACRO_END_VOICE_CALL();
				}

				ATRESP( reqHandle, ATCI_RESULT_CODE_SUPPRESS, 0, (char *)"NO CARRIER\r\n");
			}
			else if ( gCurrentCallsList.callInfo[i].OldMode == CICC_CURRENT_CMODE_DATA )
			{
				ATRESP( reqHandle, ATCI_RESULT_CODE_SUPPRESS, 0, (char *)"NO CARRIER\r\n");
			}

			if ( gCurrentCallsList.callInfo[i].OldState == CICC_CURRENT_CSTATE_INCOMING )
			{
				/* stop ringing timer */
				PARSER_RELEASE_CALL();
			}
			resetCallInfo(&gCurrentCallsList.callInfo[i]);

			break; /* entry found - break out of the loop */
		}
	}

}

void ATRespByCallType(CiCcCallType CallType)
{
	/* Michal Buaki - AT+CRC support */
	if (g_iCRCMode == 1)
	{
		switch (CallType)
		{
		case CICC_CALLTYPE_ASYNC:
			ATRESP( IND_REQ_HANDLE, 0, 0, (char*)"+CRING: ASYNC\r\n");
			break;
		case CICC_CALLTYPE_SYNC:
			ATRESP( IND_REQ_HANDLE, 0, 0, (char *)"+CRING: SYNC\r\n");
			break;
		case CICC_CALLTYPE_REL_ASYNC:
			ATRESP( IND_REQ_HANDLE, 0, 0, (char *)"+CRING: REL ASYNC\r\n");
			break;
		case CICC_CALLTYPE_REL_SYNC:
			ATRESP( IND_REQ_HANDLE, 0, 0, (char *)"+CRING: REL SYNC\r\n");
			break;
		case CICC_CALLTYPE_FAX:
			ATRESP( IND_REQ_HANDLE, 0, 0, (char *)"+CRING: FAX\r\n");
			break;
		case CICC_CALLTYPE_VOICE:
			ATRESP( IND_REQ_HANDLE, 0, 0, (char *)"+CRING: VOICE\r\n");
			break;
		case CICC_CALLTYPE_VOICE_THEN_ASYNC:
			ATRESP( IND_REQ_HANDLE, 0, 0, (char *)"+CRING: VOICE/ASYNC\r\n");
			break;
		case CICC_CALLTYPE_VOICE_THEN_SYNC:
			ATRESP( IND_REQ_HANDLE, 0, 0, (char *)"+CRING: VOICE/SYNC\r\n");
			break;
		case CICC_CALLTYPE_VOICE_THEN_REL_ASYNC:
			ATRESP( IND_REQ_HANDLE, 0, 0, (char *)"+CRING: VOICE/REL ASYNC\r\n");
			break;
		case CICC_CALLTYPE_VOICE_THEN_REL_SYNC:
			ATRESP( IND_REQ_HANDLE, 0, 0, (char *)"+CRING: VOICE/REL SYNC\r\n");
			break;
		case CICC_CALLTYPE_ALT_VOICE_ASYNC:
			ATRESP( IND_REQ_HANDLE, 0, 0, (char *)"+CRING: ALT VOICE/ASYNC\r\n");
			break;
		case CICC_CALLTYPE_ALT_VOICE_SYNC:
			ATRESP( IND_REQ_HANDLE, 0, 0, (char *)"+CRING: ALT VOICE/SYNC\r\n");
			break;
		/*case CICC_CALLTYPE_ALT_VOCIE_REL_ASYNC:
					 ATRESP( IND_REQ_HANDLE, 0, 0, (char *)"+CRING: ALT VOICE/REL ASYNC\r\n");
		    break;
		   case CICC_CALLTYPE_ALT_VOCIE_REL_SYNC:
					 ATRESP( IND_REQ_HANDLE, 0, 0,  (char *)"+CRING: ALT VOICE/REL SYNC\r\n");
		    break;*/
		case CICC_CALLTYPE_ALT_ASYNC_VOICE:
			ATRESP( IND_REQ_HANDLE, 0, 0, (char *)"+CRING: ALT ASYNC/VOICE\r\n");
			break;
		case CICC_CALLTYPE_ALT_SYNC_VOICE:
			ATRESP( IND_REQ_HANDLE, 0, 0, (char *)"+CRING: ALT SYNC/VOICE\r\n");
			break;
		case CICC_CALLTYPE_ALT_REL_ASYNC_VOICE:
			ATRESP( IND_REQ_HANDLE, 0, 0, (char *)"+CRING: ALT REL ASYNC/VOICE\r\n");
			break;
		case CICC_CALLTYPE_ALT_REL_SYNC_VOICE:
			ATRESP( IND_REQ_HANDLE, 0, 0, (char *)"+CRING: ALT REL SYNC/VOICE\r\n");
			break;
		case CICC_CALLTYPE_ALT_VOICE_FAX:
			ATRESP( IND_REQ_HANDLE, 0, 0, (char *)"+CRING: ALT VOICE/FAX\r\n");
			break;
		case CICC_CALLTYPE_ALT_FAX_VOICE:
			ATRESP( IND_REQ_HANDLE, 0, 0, (char *)"+CRING: ALT FAX/VOICE\r\n");
			break;
		default:
			ATRESP( IND_REQ_HANDLE, 0, 0, (char *)"RING\r\n");
			break;
		}
	}
	else
		ATRESP( IND_REQ_HANDLE, 0, 0, (char *)"RING\r\n");

	return;
}


/************************************************************************************
 * FUNCTION: ccInd
 *
 * PARAMETERS:
 *
 * DESCRIPTION:   Process CC indication
 *
 * RETURNS:
 *************************************************************************************/


void ccInd(CiSgOpaqueHandle opSgOpaqueHandle,
	   CiServiceGroupID svgId,
	   CiPrimitiveID primId,
	   CiIndicationHandle indHandle,
	   void*               pParam)
{
	UNUSEDPARAM(opSgOpaqueHandle)
	UNUSEDPARAM(indHandle)
	CiCcPrimConnectInd            * connectInd;
	CiCcPrimDisconnectInd         *disconnectInd;
	CiCcPrimMoCallFailedInd       *moCallInd;
	CiCcPrimSsiNotifyInd          *ssiNotifyInd;
	CiCcPrimSsuNotifyInd          *ssuNotifyInd;
	CiCcPrimHeldCallInd           *heldCall;
	CiCcPrimCallWaitingInd        *callWaitInd;
	CiCcPrimClipInfoInd           *clipInfoInd;
	CiCcPrimColpInfoInd           *colpInfoInd;
	CiCcPrimCcmUpdateInd          *ccmUpdateInd;
	CiCcPrimListCurrentCallsInd   *ccPrimListCurrentCallsInd;
	CiCcPrimIncomingCallInd       *incomingCallInd;
	char ccIndBuf[300];
	char ccIndTmpBuf[300];


	INT32 i;
	DBGMSG("[%s] Line(%d): primId(%d) \n", __FUNCTION__, __LINE__, primId);
	switch (primId)
	{
	case CI_CC_PRIM_INCOMING_CALL_IND:
	{

		incomingCallInd = (CiCcPrimIncomingCallInd *)pParam;
		for ( i = 0; i < gCurrentCallsList.NumCalls; i++ )
		{
			if ( gCurrentCallsList.callInfo[i].CallId == incomingCallInd->CallId )
			{
				DBGMSG("CI_CC_PRIM_INCOMING_CALL_IND: CallId:%d,CallState:%d\n", incomingCallInd->CallId, gCurrentCallsList.callInfo[i].State);
				gCurrentCallsList.callInfo[i].State = CICC_CURRENT_CSTATE_INCOMING;
				break;
			}
		}

		IncomingCallType = incomingCallInd->callType.CallType;
		ATRespByCallType(IncomingCallType);

		/* start the ring - answer timer */
		// PARSER_START_RINGING();

		/* Auto Answer support */
		PARSER_START_AUTO_ANSWER();

		break;
	}

	case CI_CC_PRIM_CONNECT_IND:
	{
		DBGMSG("[%s] Line(%d): Received CI_CC_PRIM_CONNECT_IND \n", __FUNCTION__, __LINE__);
		connectInd = (CiCcPrimConnectInd *)pParam;

		/* look for the call in the current list */
		for ( i = 0; i < gCurrentCallsList.NumCalls; i++ )
		{
			if ( gCurrentCallsList.callInfo[i].CallId == connectInd->CallId )
			{
				if ( gCurrentCallsList.callInfo[i].Mode == CICC_CURRENT_CMODE_VOICE )
				{
					/* We don't need to return the second OK */
				}
				else    /* is a data call */
				{
					/*Here we delay to respond "CONNECT" until we receive DATA_OK_IND from COMM */
//                            ATRESP( gCurrentCallsList.callInfo[i].reqHandle, 0, 0, (char *)"CONNECT\r\n");
				}

				break;  /* entry found - break out of the loop */
			}
		}

		break;
	}

	case CI_CC_PRIM_CALL_PROCEEDING_IND:
	{
		break;
	}

	case CI_CC_PRIM_LIST_CURRENT_CALLS_IND:
	{
		ccPrimListCurrentCallsInd = (CiCcPrimListCurrentCallsInd *)pParam;

		/* update the current calls list */
		updateCallList( IND_REQ_HANDLE, ccPrimListCurrentCallsInd->NumCalls, ccPrimListCurrentCallsInd->callInfo, FALSE);

		break;
	}

	case CI_CC_PRIM_DISCONNECT_IND:
	{

		disconnectInd = (CiCcPrimDisconnectInd *)pParam;

		//workaround CP error that send wrong DISC_IND for CSD connection
		if(disconnectInd->CallId == NEVER_USED_USER_CALL_ID)
			break;
		
		getCcIndCauseString(disconnectInd->Cause, gErrCauseBuf);
#ifdef AT_CUSTOMER_HTC
		sprintf(ccIndBuf, "@HTCDIS:%02X\r\n", (UINT8)atoi(gErrCauseBuf));
		ATRESP( IND_REQ_HANDLE, 0, 0, ccIndBuf);
#endif
		resetCallParam( IND_REQ_HANDLE, disconnectInd->CallId );

		break;
	}

	case CI_CC_PRIM_MO_CALL_FAILED_IND:
	{

		moCallInd = (CiCcPrimMoCallFailedInd *)pParam;

		if(moCallInd->CallId == NEVER_USED_USER_CALL_ID)
			break;

		getCcIndCauseString(moCallInd->Cause, gErrCauseBuf);
#ifdef AT_CUSTOMER_HTC
		sprintf(ccIndBuf, "@HTCDIS:%02X\r\n", (UINT8)atoi(gErrCauseBuf));
		ATRESP( IND_REQ_HANDLE, 0, 0, ccIndBuf);
#endif

		/* reset call associated info variables */
		resetCallParam( IND_REQ_HANDLE, moCallInd->CallId );

		break;
	}

	case CI_CC_PRIM_SSI_NOTIFY_IND:
	{

		ssiNotifyInd = (CiCcPrimSsiNotifyInd *)pParam;
		sprintf(ccIndBuf, "+CSSI: %d", ssiNotifyInd->info.Status);
		if(ssiNotifyInd->info.Status == SSI_CS_CUG_CALL)
		{
			sprintf(ccIndTmpBuf, ", %d", ssiNotifyInd->info.CugIndex);
			strcat(ccIndBuf, ccIndTmpBuf);
		}
		strcat(ccIndBuf, "\r\n");
		ATRESP( IND_REQ_HANDLE, 0, 0, ccIndBuf);
#ifdef AT_CUSTOMER_HTC
		if(ssiNotifyInd->info.Status == SSI_CS_MO_BARRED)
		{
			sprintf(ccIndBuf, "@HTCDIS:%02X\r\n", AT_HTCDIS_CAUSE_CC_CALL_BARRING);
			ATRESP( IND_REQ_HANDLE, 0, 0, ccIndBuf);
		}
#endif
		break;
	}

	case CI_CC_PRIM_SSU_NOTIFY_IND:
	{
		const char *plus = NULL;
		char digitsBuf[CI_MAX_ADDRESS_LENGTH + ATCI_NULL_TERMINATOR_LENGTH];
		ssuNotifyInd = (CiCcPrimSsuNotifyInd *)pParam;
		{
			BOOL index_provided = FALSE;
			BOOL callerinfo_provided = FALSE;

			sprintf(ccIndBuf, "+CSSU: %d", ssuNotifyInd->info.Status);

			if(ssuNotifyInd->info.Status == SSU_CS_CUG_CALL)
			{
				sprintf(ccIndTmpBuf, ", %d", ssuNotifyInd->info.CugIndex);
				strcat(ccIndBuf, ccIndTmpBuf);
				index_provided = TRUE;
			}

			if(ssuNotifyInd->info.CallerInfo.OptCallerNumber.Present)
			{
				if(index_provided == FALSE)
				{
					strcat(ccIndBuf, ", ");
					index_provided = TRUE;
				}

				if(ssuNotifyInd->info.CallerInfo.OptCallerNumber.AddressInfo.AddrType.NumType == CI_NUMTYPE_INTERNATIONAL)
					plus = "+";
				else
					plus = "";
				memcpy( digitsBuf, ssuNotifyInd->info.CallerInfo.OptCallerNumber.AddressInfo.Digits,
						ssuNotifyInd->info.CallerInfo.OptCallerNumber.AddressInfo.Length );
				digitsBuf[ssuNotifyInd->info.CallerInfo.OptCallerNumber.AddressInfo.Length] = '\0';
				sprintf(ccIndTmpBuf, ", \"%s%s\", %d", plus, digitsBuf,
					DialNumTypeCI2GSM(ssuNotifyInd->info.CallerInfo.OptCallerNumber.AddressInfo.AddrType.NumType));
				strcat(ccIndBuf, ccIndTmpBuf);
				callerinfo_provided = TRUE;
			}

			if(ssuNotifyInd->info.CallerInfo.OptCallerSubaddr.Present)
			{
				if(index_provided == FALSE)
				{
					strcat(ccIndBuf, ", ");
					index_provided = TRUE;
				}

				if(callerinfo_provided == FALSE)
				{
					strcat(ccIndBuf, ", ,");
					callerinfo_provided = TRUE;
				}

				if(ssuNotifyInd->info.CallerInfo.OptCallerSubaddr.Type == CI_NUMTYPE_INTERNATIONAL)
					plus = "+";
				else
					plus = "";

				memcpy( digitsBuf, ssuNotifyInd->info.CallerInfo.OptCallerSubaddr.Digits,
						ssuNotifyInd->info.CallerInfo.OptCallerSubaddr.Length );
				digitsBuf[ssuNotifyInd->info.CallerInfo.OptCallerSubaddr.Length] = '\0';
				sprintf(ccIndTmpBuf, " , \"%s%s\", %d", plus, digitsBuf,
					DialNumTypeCI2GSM(ssuNotifyInd->info.CallerInfo.OptCallerSubaddr.Type));
				strcat(ccIndBuf, ccIndTmpBuf);
			}

			strcat(ccIndBuf, "\r\n");
			ATRESP( IND_REQ_HANDLE, 0, 0, ccIndBuf);
		}
		break;
	}

	case CI_CC_PRIM_HELD_CALL_IND:
	{
		heldCall = (CiCcPrimHeldCallInd *)pParam;

		/*
		**  Make sure this failure indication is for the current call.
		*/
		for ( i = 0; i < gCurrentCallsList.NumCalls; i++ )
		{
			if ( gCurrentCallsList.callInfo[i].CallId == heldCall->CallId )
			{
				sprintf(ccIndBuf, "+CHLD: %d\r\n", heldCall->CallId);
				ATRESP( gCurrentCallsList.callInfo[i].reqHandle, 0, 0, ccIndBuf);

				break;  /* entry found - break out of the loop */
			}
		}
		break;
	}

	case CI_CC_PRIM_CALL_WAITING_IND:
	{

		char digitsBuf[CI_MAX_ADDRESS_LENGTH + ATCI_NULL_TERMINATOR_LENGTH];
		callWaitInd = (CiCcPrimCallWaitingInd *)pParam;
		if (gSSCntx.localCwOption == 1)
		{
			const char *plus = NULL;
			if(callWaitInd->info.callerInfo.OptCallerNumber.AddressInfo.AddrType.NumType == CI_NUMTYPE_INTERNATIONAL)
				plus = "+";
			else
				plus = "";
			memcpy( digitsBuf, callWaitInd->info.callerInfo.OptCallerNumber.AddressInfo.Digits,
						callWaitInd->info.callerInfo.OptCallerNumber.AddressInfo.Length );
			digitsBuf[callWaitInd->info.callerInfo.OptCallerNumber.AddressInfo.Length] = '\0';
			sprintf(ccIndBuf, "+CCWA: \"%s%s\", %d\r\n", plus, digitsBuf,
				DialNumTypeCI2GSM(callWaitInd->info.callerInfo.OptCallerNumber.AddressInfo.AddrType.NumType));
			ATRESP( IND_REQ_HANDLE, 0, 0, ccIndBuf);
		}
		break;
	}

	case CI_CC_PRIM_CLIP_INFO_IND:
	{

		clipInfoInd = (CiCcPrimClipInfoInd *)pParam;
		updateCallerInfo(clipInfoInd->CallId, clipInfoInd->info.CliValidity, &clipInfoInd->info.callerInfo);
		if (( clipInfoInd->info.CliValidity == CLI_VALIDITY_VALID ) &&
		    ( clipInfoInd->info.callerInfo.OptCallerNumber.Present == TRUE ))
		{
			const char *plus = NULL;
			if ( clipInfoInd->info.callerInfo.OptCallerNumber.AddressInfo.AddrType.NumType == CI_NUMTYPE_INTERNATIONAL )
				plus = "+";
			else
				plus = "";

			memcpy( ccIndTmpBuf, clipInfoInd->info.callerInfo.OptCallerNumber.AddressInfo.Digits,
				clipInfoInd->info.callerInfo.OptCallerNumber.AddressInfo.Length );
			ccIndTmpBuf[clipInfoInd->info.callerInfo.OptCallerNumber.AddressInfo.Length] = 0;
			sprintf( ccIndBuf, "+CLIP: \"%s%s\", %d\r\n", plus, ccIndTmpBuf,
				     DialNumTypeCI2GSM(clipInfoInd->info.callerInfo.OptCallerNumber.AddressInfo.AddrType.NumType));

			ATRESP( IND_REQ_HANDLE, 0, 0, ccIndBuf);

		}

		if ( clipInfoInd->info.callerInfo.OptCallerName.Present == TRUE )
		{
			memcpy( ccIndTmpBuf, clipInfoInd->info.callerInfo.OptCallerName.NameInfo.Name,
				clipInfoInd->info.callerInfo.OptCallerName.NameInfo.Length );
			ccIndTmpBuf[clipInfoInd->info.callerInfo.OptCallerName.NameInfo.Length] = 0;

			sprintf( ccIndBuf, "+CNAP: %s\r\n", ccIndTmpBuf);
			ATRESP( IND_REQ_HANDLE, 0, 0, ccIndBuf);
		}

		break;
	}

	case CI_CC_PRIM_COLP_INFO_IND:
	{

		colpInfoInd = (CiCcPrimColpInfoInd *)pParam;
		if (( colpInfoInd->info.CliValidity == CLI_VALIDITY_VALID ) &&
		    ( colpInfoInd->info.callerInfo.OptCallerNumber.Present == TRUE ))
		{
			const char *plus = NULL;
			if ( colpInfoInd->info.callerInfo.OptCallerNumber.AddressInfo.AddrType.NumType == CI_NUMTYPE_INTERNATIONAL )
				plus = "+";
			else
				plus = "";

			memcpy( ccIndTmpBuf, colpInfoInd->info.callerInfo.OptCallerNumber.AddressInfo.Digits,
				colpInfoInd->info.callerInfo.OptCallerNumber.AddressInfo.Length );
			ccIndTmpBuf[colpInfoInd->info.callerInfo.OptCallerNumber.AddressInfo.Length] = 0;
			sprintf( ccIndBuf, "+COLP: \"%s%s\", %d\r\n", plus, ccIndTmpBuf,
				     DialNumTypeCI2GSM( colpInfoInd->info.callerInfo.OptCallerNumber.AddressInfo.AddrType.NumType));

			ATRESP( IND_REQ_HANDLE, 0, 0, ccIndBuf);
		}
		break;
	}

	case CI_CC_PRIM_CCM_UPDATE_IND:
	{
		ccmUpdateInd = (CiCcPrimCcmUpdateInd*)pParam;
		sprintf( ccIndBuf, "+CCCM:\"%06X\"\r\n", (UINT32)ccmUpdateInd->Ccm );
		ATRESP( IND_REQ_HANDLE, 0, 0, ccIndBuf);
		break;
	}

	case CI_CC_PRIM_AOC_WARNING_IND:
	{
		break;
	}

	default:
		break;
	}

	/* free up the indication memory */
	atciSvgFreeIndMem( svgId, primId, pParam );

	return;
}

static utlReturnCode_T RINGING_TIMER_CALLBACK(const utlTimerId_T id,
					      const unsigned long timeout_count,
					      void                *arg_p,
					      const utlAbsoluteTime_P2c curr_time_p)
{
	UNUSEDPARAM(id)
	UNUSEDPARAM(timeout_count)
	UNUSEDPARAM(arg_p)
	UNUSEDPARAM(curr_time_p)
	F_ENTER();

	TelAtParserID sAtpIndex = TEL_AT_CMD_ATP_0;
	UINT32 atHandle = MAKE_AT_HANDLE(sAtpIndex);

	INT32 i;
	CiCcPrimReleaseCallReq          *ccReleaseRequest;

	/* look for an active or in progress call */
	for ( i = 0; i < gCurrentCallsList.NumCalls; i++ )
	{
		if ( gCurrentCallsList.callInfo[i].State == CICC_CURRENT_CSTATE_INCOMING )
		{
			/* mark is is MO released */
			gCurrentCallsList.callInfo[i].ReleaseDirection = CICC_MO_CALL;
			ccReleaseRequest = utlCalloc(1, sizeof(*ccReleaseRequest));
			if (ccReleaseRequest == NULL) {
				ERRMSG("RINGING TIMER CALLBACK calloc error! \n");
				break;
			}
			ccReleaseRequest->CallId = gCurrentCallsList.callInfo[i].CallId;
			ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_RELEASE_CALL_REQ,
				   MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_RELEASE_CALL_REQ), (void *)ccReleaseRequest );
			break;
		}
	}

	utlStopTimer(ringing_timer_id);

	F_LEAVE();

	/* Always return SUCCESS */
	return(utlSUCCESS);
}


static utlReturnCode_T AUTO_ANSWER_TIMER_CALLBACK(const utlTimerId_T id,
					      const unsigned long timeout_count,
					      void                *arg_p,
					      const utlAbsoluteTime_P2c curr_time_p)
{
	UNUSEDPARAM(id)
	UNUSEDPARAM(timeout_count)
	UNUSEDPARAM(arg_p)
	UNUSEDPARAM(curr_time_p)
	F_ENTER();

	/* FIXME just use the default ATP */
	TelAtParserID sAtpIndex = TEL_AT_CMD_ATP_0;
	UINT32 atHandle = MAKE_AT_HANDLE(sAtpIndex);

	INT32 i;
	CiCcPrimAnswerCallReq          *ccAnswerRequest;

	if (auto_answer_remind_cycles) {
		ATRespByCallType(IncomingCallType);
		auto_answer_remind_cycles--;
		DBGMSG ("RINGING: cycles = %d \r\n", auto_answer_remind_cycles);
	        F_LEAVE();
		return(utlSUCCESS);
	}

	/* look for an active or in progress call */
	for ( i = 0; i < gCurrentCallsList.NumCalls; i++ )
	{
		if ( gCurrentCallsList.callInfo[i].State == CICC_CURRENT_CSTATE_INCOMING )
		{
			/* mark it is MT answered */
			gCurrentCallsList.callInfo[i].CallDirection = CICC_MT_CALL;
			ccAnswerRequest = utlCalloc(1, sizeof(*ccAnswerRequest));
			if (ccAnswerRequest == NULL) {
				ERRMSG("AUTO ANSWER TIMER CALLBACK Calloc error! \n");
				break;
			}
			ccAnswerRequest->CallId = gCurrentCallsList.callInfo[i].CallId;
			ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_ANSWER_CALL_REQ,
				   MAKE_CI_REQ_HANDLE(atHandle, CI_CC_PRIM_ANSWER_CALL_REQ), (void *)ccAnswerRequest );
			break;
		}
	}

	utlStopTimer(auto_answer_timer_id);

	F_LEAVE();

	/* Always return SUCCESS */
	return(utlSUCCESS);
}


void PARSER_START_AUTO_ANSWER()
{
	F_ENTER();


	if (auto_answer_delay_cycles == 0)
		return;

	if ((auto_answer_timer_id = utlStartTimer(&auto_answer_delay, auto_answer_delay_cycles, &AUTO_ANSWER_TIMER_CALLBACK, NULL)) == (utlTimerId_T)utlFAILED)
	{
		ERRMSG("Error: utlStartTimer(1.0)\n");
	}
	auto_answer_remind_cycles = auto_answer_delay_cycles - 1;


	F_LEAVE();
}

void PARSER_START_RINGING()
{
	F_ENTER();

	if ((ringing_timer_id = utlStartTimer(&ringing_period, ringing_num_cycles, &RINGING_TIMER_CALLBACK, NULL)) == (utlTimerId_T)utlFAILED)
	{
		ERRMSG("Error: utlStartTimer(1.0)\n");
	}

	F_LEAVE();
}

void PARSER_ANSWER_CALL()
{
	F_ENTER();

	if ( utlStopTimer(auto_answer_timer_id) == utlFAILED )
	{
		ERRMSG("Error: utlStopTimer(1.0)\n");
	}

	F_LEAVE();
}

void PARSER_RELEASE_CALL()
{
	F_ENTER();

	if ( utlStopTimer(auto_answer_timer_id) == utlFAILED )
	{
		ERRMSG("Error: utlStopTimer(1.0)\n");
	}

	F_LEAVE();
}

void initCCContext( void )
{
	UINT16 i;

	/* update the current calls list */
	gCurrentCallsList.NumCalls = 0;
	for ( i = 0; i < CICC_MAX_CURRENT_CALLS; i++ )
	{
		resetCallInfo(&gCurrentCallsList.callInfo[i]);
	}
	for ( i = 0; i < NUM_OF_TEL_ATP; i++ )
	{
		gCurrentCallsList.currCall[i].currBasicMode = CICC_NUM_BASIC_CMODES;
		gCurrentCallsList.currCall[i].currCallId = 0;
		gCurrentCallsList.currCall[i].currHandle = INVALID_REQ_HANDLE;
	}

	/* Initialize error cause */
	strcpy(gErrCauseBuf, "0 Unknown");
}

/****************************************************************************************
*  FUNCTION:  CC_GetDS
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+DS?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_GetDS(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimGetDataCompReq            *pGetDataCompReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_GET_DATACOMP_REQ,
			 atHandle, (void *)pGetDataCompReq );

	return ret;

}
/****************************************************************************************
*  FUNCTION:  CC_GetSupportDS
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+DS=?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_GetSupportDS(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimGetDataCompCapReq        *pGetDataCompCapReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_GET_DATACOMP_CAP_REQ,
			 atHandle, (void *)pGetDataCompCapReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  CC_SetDS
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+DS=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_SetDS(UINT32 atHandle, CiCcDataCompInfo *pDataCompInfo)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimSetDataCompReq            *pSetDataCompReq = utlCalloc(1, sizeof(*pSetDataCompReq));

	if (pSetDataCompReq == NULL)
		return CIRC_FAIL;

	memcpy(&pSetDataCompReq->info, pDataCompInfo, sizeof(*pDataCompInfo));

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_SET_DATACOMP_REQ,
			 atHandle, (void *)pSetDataCompReq );
	return ret;
}

/****************************************************************************************
*  FUNCTION:  CC_GetCSTA
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+CSTA?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_GetCSTA(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimGetNumberTypeReq           *pGetNumberTypeReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_GET_NUMBERTYPE_REQ,
			 atHandle, (void *)pGetNumberTypeReq );
	return ret;
}

/****************************************************************************************
*  FUNCTION:  CC_SetCSTA
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+CSTA=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_SetCSTA(UINT32 atHandle, CiAddressType* pAddressType)
{
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimSetNumberTypeReq        *pSetNumberTypeReq = utlCalloc(1, sizeof(CiCcPrimSetNumberTypeReq));

	if (pSetNumberTypeReq == NULL)
		return CIRC_FAIL;

	memcpy(&pSetNumberTypeReq->numType, pAddressType, sizeof(*pAddressType));

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_SET_NUMBERTYPE_REQ,
			 atHandle, (void *)pSetNumberTypeReq );
	return ret;
}

/****************************************************************************************
*  FUNCTION:  CC_GetCPUC
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+CPUC?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode CC_GetCPUC(UINT32 atHandle){
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimGetPuctInfoReq  *getPuctInfoReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_GET_PUCT_INFO_REQ,
			atHandle, (void *)getPuctInfoReq );
	return ret;
}

/****************************************************************************************
*  FUNCTION:  CC_SetCPUC
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+CPUC=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_SetCPUC(UINT32 atHandle, BOOL isNoPasswd, INT16 param_2_Length, CHAR *cpucChvPswd){
	UNUSEDPARAM(param_2_Length)
	CiReturnCode ret = CIRC_FAIL;
	CiCcPrimSetPuctInfoReq *setPuctInfoReq;
	UINT8 oper;
	if ( isNoPasswd ){
		setPuctInfoReq = utlCalloc(1, sizeof(CiCcPrimSetPuctInfoReq));
		if (setPuctInfoReq == NULL)
			return CIRC_FAIL;

		memcpy(setPuctInfoReq, &gSetPuctInfoReq, sizeof(CiCcPrimSetPuctInfoReq));
		/* if password is not specified => send 'set PUCT' CI request directly */
		ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_SET_PUCT_INFO_REQ,
				atHandle, (void *)setPuctInfoReq );
		}
	else    /* if password is specified => must perform PIN2 validation */
	{
		oper = (((SET_CPUC & 0x0f) << 4) | (CI_SIM_CHV_VERIFY & 0x0f));
		ret = SIM_EnterPin(atHandle, CI_SIM_CHV_2, oper, cpucChvPswd, NULL);
	}

	return ret;
}

/****************************************************************************************
*  FUNCTION:  CC_SendLongDTMF
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT$VTS=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_SendLongDTMF(UINT32 atHandle, UINT8 DTMFChar, BOOL mode){
       CiReturnCode ret = CIRC_FAIL;
       CiCcPrimStartDtmfReq *startDtmfReq;
       CiCcPrimStopDtmfReq *stopDtmfReq;
       if(mode)
       {
               startDtmfReq = utlCalloc(1, sizeof(CiCcPrimStartDtmfReq));
               if (startDtmfReq == NULL)
                       return CIRC_FAIL;

               startDtmfReq->CallId = 0;
               startDtmfReq->Digit = DTMFChar;
               ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_START_DTMF_REQ,
                               atHandle, (void *)startDtmfReq);
       }
       else
       {
               stopDtmfReq = utlCalloc(1, sizeof(CiCcPrimStopDtmfReq));
               if (stopDtmfReq == NULL)
                       return CIRC_FAIL;

               stopDtmfReq->CallId = 0;
               ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_STOP_DTMF_REQ,
                               atHandle, (void *)stopDtmfReq);
       }
       return ret;
}
#ifdef AT_CUSTOMER_HTC
/****************************************************************************************
*  FUNCTION:  CC_EditBlackList
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to process AT+FWDB=1,....
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode CC_EditBlackList(UINT32 atHandle, UINT8 type, UINT16 number, const int *record_indexes, const CHAR *records, UINT8 maxRecordSize){
       CiReturnCode ret = CIRC_FAIL;
       CiCcPrimSetFwListReq *setFwListReq;
       int i;
       setFwListReq = utlCalloc(1, sizeof(CiCcPrimSetFwListReq));
       if (setFwListReq == NULL)
            return CIRC_FAIL;
       setFwListReq->Type = type;
       setFwListReq->NumRecords = number;
       for(i = 0; i < number && i < CI_MAX_FW_NUM_LIST && record_indexes != NULL && records != NULL; i++)
       {
	    setFwListReq->ListInfo[i].Index = *record_indexes++;
	    setFwListReq->ListInfo[i].Length = strlen((char *)records);
	    if(setFwListReq->ListInfo[i].Length >= CI_MAX_FW_STRING_LENGTH)
	    {
		utlFree(setFwListReq);
		return ret;
	    }
	    strncpy((char *)setFwListReq->ListInfo[i].PhoneNumString, (char *)records, setFwListReq->ListInfo[i].Length);
	    records += maxRecordSize;
       }
       if(i == number)
            ret= ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_SET_FW_LIST_REQ,
                               atHandle, (void *)setFwListReq);
       else
            utlFree(setFwListReq);
       return ret;
}
#endif
