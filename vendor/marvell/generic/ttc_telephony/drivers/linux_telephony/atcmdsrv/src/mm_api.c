/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: mm_api.c
 *
 *  Description: API interface implementation for Network service
 *
 *  History:
 *   Oct 14, 2008 - Qiang Xu, Creation of file
 *
 ******************************************************************************/
#include "ci_api_types.h"
#include "ci_api.h"
#include "telatci.h"
#include "teldef.h"
#include "telatparamdef.h"
#include "telutl.h"
#include "ci_mm.h"
#include "ci_sim.h"
#include "utlMalloc.h"
#include "telmm.h"

#define CI_MM_PRIM_GET_STAR_NITZ_REQ (CI_MM_PRIM_LAST_COMMON_PRIM + 1)
/************************************************************************************
 *
 * MM related const and global variables
 *
 *************************************************************************************/
extern CiServiceHandle gAtciSvgHandle[CI_SG_NUMIDS + 1];

/* Global variable to record current reg option , used in +CREG */
static int gCurrRegOption = CIMM_REGRESULT_STATUS;

/* Global variable to record total supported operator number and currently fetched supported operator index */
static int gTotalSuppOpeNum = 0, gCurrSuppOpeIndex = 0;

/* Global variable to record total preferred operator number and currently fetched preferred operator index */
static int gTotalPrefOpeNum = 0, gCurrPrefOpeIndex = 0;

/* Because the info of reply string of +COPS=? is obtained by several CI conf msg, we need a global variable to record it */
static char gAtReplyString[500] = { 0 };

/*Global variable to record signal quality, used in +CSQ */
static int gSignalRssi = 99, gSignalBer = 99;       //99 means not known or not detectable

/* These global variables may be used by PS module */
CiMmAccTechMode gAccTchMode = CI_MM_NUM_ACT;

static int gCurrRegStatus = CIMM_REGSTATUS_NOT_SEARCHING;

/* Network mode ind msg reporting setting */
static int gNwModeIndEnabled = 0;

/* add for CNUM */
static int gNumberOfSubscriber = 0;

/* AcT (Access Tech), obtained from CI_MM_PRIM_NETWORK_MODE_IND and report by +CREG or +CGREG ind */
int gActDetail = 0;

/*system mode, used in ^MODE indication and ^SYSINFO command*/
static int gSysMode = ATCI_SYSINFO_SYSTEMO_MODE_NO_SERVICE;

/*array to save reject cause history*/
#define MAX_REJECTCAUSE_HISTORY 10
CiMmPrimAirInterfaceRejectCauseInd rejectCauseHistory[MAX_REJECTCAUSE_HISTORY];
static int gRejectCauseNumber = 0;
static int gRejectCauseIndEnabled = 0;

extern UINT8 gCurrentPSRegStatus;
extern CiSimStatus gSimStatus;
extern CiSimPinState gSimPinState;

/************************************************************************************
 *
 * Internal library function
 *
 *************************************************************************************/
/****************************************************************************************
*  FUNCTION:  Lib function to set preferred operator list - libAddPreferenceOperation
*
****************************************************************************************/
static CiReturnCode libAddPreferenceOperation(INT32 index, INT32 format, char* operatorString, CiRequestHandle atHandle, INT32 Actval)
{
	CiMmPrimAddPreferredOperatorReq *addPrefOperReq;
	CiReturnCode ret = CIRC_FAIL;
	INT16 length = strlen((char *)operatorString);
	Plmn plmn;

	addPrefOperReq = utlCalloc(1, sizeof(CiMmPrimAddPreferredOperatorReq));
	if (addPrefOperReq == NULL)
		return ret;

	if (index == 0)
	{
		addPrefOperReq->addPrefOpType =  CI_MM_ADDPREFOP_FIRST_AVAILABLE;
	}
	else
	{
		addPrefOperReq->index = index;
		addPrefOperReq->addPrefOpType =  CI_MM_ADDPREFOP_INSERT_AT_INDEX;
	}
	addPrefOperReq->info.Format = format;

	switch (format)
	{
	case ATCI_OP_FORMAT_LONG:
	{
		addPrefOperReq->info.CiMmNetOpId.OperatorId.Length = length;
		strcpy(addPrefOperReq->info.CiMmNetOpId.OperatorId.Id, (char *)operatorString);
		break;
	}
	case ATCI_OP_FORMAT_SHORT:
	{
		addPrefOperReq->info.CiMmNetOpId.OperatorId.Length = length;
		strcpy(addPrefOperReq->info.CiMmNetOpId.OperatorId.Id, (char *)operatorString);
		break;
	}
	case ATCI_OP_FORMAT_NUM:
	{
		if (operStringToPlmn( (CHAR *)operatorString, &plmn, length ))
		{
			addPrefOperReq->info.CiMmNetOpId.NetworkId.CountryCode = plmn.mcc;
			addPrefOperReq->info.CiMmNetOpId.NetworkId.NetworkCode = plmn.mnc;
		}
		else
		{
			goto invalid;
		}
		break;
	}
	default:
		goto invalid;
		break;
	}

	switch(Actval)
	{
	case ATCI_ACT_GSM:
	case ATCI_ACT_GSM_COMPACT:
	case ATCI_ACT_UTRAN:
		addPrefOperReq->AccTchMode = Actval;
		break;
	default:
		goto invalid;
		break;
	}

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_ADD_PREFERRED_OPERATOR_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_ADD_PREFERRED_OPERATOR_REQ), (void *)addPrefOperReq );
	return ret;
invalid:
	utlFree(addPrefOperReq);
	return ret;
}

/****************************************************************************************
*  FUNCTION:  Lib function to set preferred operator list - libDeletePreferenceOperation
*
****************************************************************************************/
static CiReturnCode libDeletePreferenceOperation(INT32 index, CiRequestHandle atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMmPrimDeletePreferredOperatorReq *delPrefOperReq;

	delPrefOperReq = utlCalloc(1, sizeof(CiMmPrimDeletePreferredOperatorReq));
	if (delPrefOperReq == NULL)
		return ret;

	delPrefOperReq->Index = index;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_DELETE_PREFERRED_OPERATOR_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_DELETE_PREFERRED_OPERATOR_REQ), (void *)delPrefOperReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  Lib function to set preferred operator list - libChangeIDFormat
*
****************************************************************************************/
static CiReturnCode libChangeIDFormat(INT32 format, CiRequestHandle atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMmPrimSetIdFormatReq     *setFormatRequest;

	setFormatRequest = utlCalloc(1, sizeof(CiMmPrimSetIdFormatReq));
	if (setFormatRequest == NULL)
		return ret;

	setFormatRequest->Format = format;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_SET_ID_FORMAT_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_SET_ID_FORMAT_REQ), (void *)setFormatRequest );

	return ret;
}


/************************************************************************************
 *
 * Implementation of MM related AT commands by sending Request to CCI. Called by Telmm.c
 *
 *************************************************************************************/

/****************************************************************************************
*  FUNCTION:  MM_QueryRegOption
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to query register status report option, implementation of AT+CREG=?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_QueryRegOption(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMmPrimGetSupportedRegResultOptionsReq   *getSupportedRegOptReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_GET_SUPPORTED_REGRESULT_OPTIONS_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_GET_SUPPORTED_REGRESULT_OPTIONS_REQ), (void *)getSupportedRegOptReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MM_GetRegStatus
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get register status report option and current reg status info, implementation of AT+CREG?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_GetRegStatus(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMmPrimGetRegResultInfoReq   *getRegInfoReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_GET_REGRESULT_INFO_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_GET_REGRESULT_INFO_REQ), (void *)getRegInfoReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MM_SetRegOption
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to set register status report option, implementation of AT+CREG=[<n>]
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_SetRegOption(UINT32 atHandle, INT32 regOption)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMmPrimSetRegResultOptionReq   *setRegOptReq;

	setRegOptReq = utlCalloc(1, sizeof(CiMmPrimSetRegResultOptionReq));
	if (setRegOptReq == NULL)
		return CIRC_FAIL;

	setRegOptReq->Option = (CiMmRegResultOption)regOption;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_SET_REGRESULT_OPTION_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, setRegOptReq->Option), (void *)setRegOptReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MM_QuerySuppOperator
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to query supported operators, implementation of AT+COPS=?
*
*  Note: Send a CI Request to query the number of network operators.  The confirmation callback will then
*            send a get information request for every network operator.
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_QuerySuppOperator(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMmPrimGetNumPreferredOperatorsReq   *getNumOp = NULL;

	memset(gAtReplyString,'\0',sizeof(gAtReplyString));

	/* !!! IULIA - this should be changed to a new CI primitive that returns the operator list
	 * as specified in the spec. !!! */

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_GET_NUM_NETWORK_OPERATORS_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_GET_NUM_NETWORK_OPERATORS_REQ), (void *)getNumOp );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MM_GetCurrOperator
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get current operator info, implementation of AT+COPS?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_GetCurrOperator(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMmPrimGetCurrentOperatorInfoReq   *getCurrOperatorReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_GET_CURRENT_OPERATOR_INFO_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_GET_CURRENT_OPERATOR_INFO_REQ), (void *)getCurrOperatorReq );

	//gCurrMsgReq = CI_MM_PRIM_GET_CURRENT_OPERATOR_INFO_REQ;

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MM_SetCurrOperator
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to set current operator info, implementation of AT+COPS= [<mode>[,<format>[,<oper>[,< AcT>]]]]
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_SetCurrOperator(UINT32 atHandle, int modeVal, int formatVal, char *networkOperator, int opeStrLen, int accTchVal)
{
	CiReturnCode ret = CIRC_FAIL;

	switch ( (AtciCopsMode)modeVal )
	{
	case ATCI_MANUAL_DEREGISTER:
	{
		/* Send the CI request to deregister.  All other parameters can be ignored.	 */
		CiMmPrimDeregisterReq  *deregRequest = NULL;
		ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_DEREGISTER_REQ,
				 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_DEREGISTER_REQ), (void *)deregRequest );
		break;
	}

	case ATCI_AUTOMATIC_MODE:
	{
		/*  Send the CI request to automatically register with the network.  All other parameters are ignored. */
		CiMmPrimAutoRegisterReq  *autoregRequest = NULL;
		ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_AUTO_REGISTER_REQ,
				 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_AUTO_REGISTER_REQ), (void *)autoregRequest );
		break;
	}

	case ATCI_SET_FORMAT:
	{
		/* Set format only */
		if ( (AtciCopsModeFormat)formatVal < ATCI_OP_FORMAT_NUMS )
		{
			CiMmPrimSetIdFormatReq  *setFormatRequest;
			setFormatRequest = utlCalloc(1, sizeof(CiMmPrimSetIdFormatReq));
			if (setFormatRequest == NULL)
				return CIRC_FAIL;

			setFormatRequest->Format = formatVal;
			ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_SET_ID_FORMAT_REQ,
					 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_SET_ID_FORMAT_REQ), (void *)setFormatRequest );
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		}
		break;
	}

	case ATCI_MANUAL_THEN_AUTOMATIC:
	case ATCI_MANUAL_OPERATOR_SELECTION:
	{
		CiMmPrimManualRegisterReq   *manRegReq;
		manRegReq = utlCalloc(1, sizeof(CiMmPrimManualRegisterReq));
		if (manRegReq == NULL)
			return CIRC_FAIL;

		/* Init Parameters */
		if (modeVal == ATCI_MANUAL_THEN_AUTOMATIC)
		{
			/* convey the auto-fallback option request */
			manRegReq->AutoFallback = TRUE;
		}
		else if (modeVal == ATCI_MANUAL_OPERATOR_SELECTION)
		{
			manRegReq->AutoFallback = FALSE;
		}

		manRegReq->info.Present = FALSE;

		/*
		**  Determine the operator format.
		**  As per spec 27.007 and TTPCom document: If 'manual mode is chosen
		**  the format and the operator have to be specified.
		**  i.e. AT+COPS=1,2,"00101" (for numeric format)
		*/
		if ( (AtciCopsModeFormat)formatVal < ATCI_OP_FORMAT_NUMS)
		{
			if (opeStrLen > 0)
			{
				/* default access tech is GSM according to 27.007 */
				manRegReq->AccTchMode = CI_MM_ACT_GSM;
				if ( accTchVal < CI_MM_NUM_ACT )
				{
					manRegReq->AccTchMode = accTchVal;
				}
				else
				{
					/* invalid access technology */
					ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
					utlFree(manRegReq);
					break;
				}

				manRegReq->info.Present = TRUE;
				switch ( (AtciCopsModeFormat)formatVal )
				{
				case ATCI_OP_FORMAT_LONG:                   /* the full "operator alpha name", max 16 chars. */
				{
					manRegReq->info.Format = CIMM_NETOP_ID_FORMAT_ALPHA_LONG;
					manRegReq->info.CiMmNetOpId.OperatorId.Length = opeStrLen;
					strcpy( manRegReq->info.CiMmNetOpId.OperatorId.Id, (char *)networkOperator );
					break;
				}
				case ATCI_OP_FORMAT_SHORT:                  /* the abreviated "operator alpha name" */
				{
					manRegReq->info.Format = CIMM_NETOP_ID_FORMAT_ALPHA_SHORT;
					manRegReq->info.CiMmNetOpId.OperatorId.Length = opeStrLen;
					strcpy( manRegReq->info.CiMmNetOpId.OperatorId.Id, (char *)networkOperator );
					break;
				}
				case ATCI_OP_FORMAT_NUM:                    /* network ID in format "mncmcc" */
				{
					Plmn plmn;
					manRegReq->info.Format = CIMM_NETOP_ID_FORMAT_NETWORK;
					if(operStringToPlmn( (CHAR *)networkOperator, &plmn, opeStrLen ) == FALSE)
					{
						ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
						utlFree(manRegReq);
						return ret;
					}
					manRegReq->info.CiMmNetOpId.NetworkId.CountryCode = plmn.mcc;
					manRegReq->info.CiMmNetOpId.NetworkId.NetworkCode = plmn.mnc;
					break;
				}
				default:
				{
					/* invalid format */
					ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
					utlFree(manRegReq);
					return ret;
				}
				}         /* switch format */

				/* Send the manual register CI Request. */
				ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_MANUAL_REGISTER_REQ,
						 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_MANUAL_REGISTER_REQ), (void *)manRegReq );

			}
			else if (opeStrLen <= 0)
			{
				/* invalid format */
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
				utlFree(manRegReq);
				break;
			}
		}
		else if ( formatVal >= ATCI_OP_FORMAT_NUMS )
		{
			/* format must be specified as per Specs.  */
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
			utlFree(manRegReq);
			break;
		}

		break;
	}           //end of case(ATCI_MANUAL_OPERATOR_SELECTION)

	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}         /* switch mode */
	}

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MM_GetPrefOperator
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get preferred operator list from SIM, implementation of AT+CPOL?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_GetPrefOperator(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMmPrimGetNumPreferredOperatorsReq   *getNumPrefOperReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_GET_NUM_PREFERRED_OPERATORS_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_GET_NUM_PREFERRED_OPERATORS_REQ), (void *)getNumPrefOperReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MM_SetPrefOperator
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to set preferred operator list from SIM, implementation of AT+CPOL=[<index>][, <format>[,<oper>]]
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_SetPrefOperator(UINT32 atHandle, int indexVal, int formatVal, INT32 ActVal, char *networkOperator, int length)
{

	CiReturnCode ret = CIRC_FAIL;

	/* Currently we support 6 formats:
	   1. +CPOL=1,2,"40800"
	   2. +CPOL=,,"40800"
	   3. +CPOL=,2,"40800"
	   4. +CPOL=1,,"40800"    add
	   5. +CPOL=1     delete
	   6. +CPOL=,2      change display format
	 */

	if ( (indexVal != TEL_AT_CPOL_0_INDEX_VAL_DEFAULT) && (length > 0) ) // case 1 & 4
	{
		ret = libAddPreferenceOperation(indexVal, formatVal, networkOperator, atHandle, ActVal);
	}
	if ( (indexVal == TEL_AT_CPOL_0_INDEX_VAL_DEFAULT) && (length > 0) ) // case 2 & 3
	{
		ret = libAddPreferenceOperation(0, formatVal, networkOperator, atHandle, ActVal);
	}
	if ( (indexVal != TEL_AT_CPOL_0_INDEX_VAL_DEFAULT) && (length == 0) ) // case 5
	{
		ret = libDeletePreferenceOperation(indexVal, atHandle);
	}
	if ( (indexVal == TEL_AT_CPOL_0_INDEX_VAL_DEFAULT) && (length == 0) ) // case 6
	{
		ret = libChangeIDFormat(formatVal, atHandle);
	}
	return ret;
}

/****************************************************************************************
*  FUNCTION:  MM_GetTimeZoneReportOption
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get current time zone report option (on/off), implementation of AT+CTZR=?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_GetTimeZoneReportOption(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_GET_NITZ_INFO_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_GET_NITZ_INFO_REQ), NULL );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MM_GetstarTimeZoneReportOption
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get current time zone report option (on/off), implementation of AT*CTZR?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_GetstarTimeZoneReportOption(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_GET_NITZ_INFO_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_GET_STAR_NITZ_REQ), NULL );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MM_QuerySuppOpeInfo
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to query info of one supported operator specified by index, part of implementation of AT+COPS=?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_QuerySuppOpeInfo(UINT32 atHandle, int opeIndex)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMmPrimGetNetworkOperatorInfoReq  *getOpInfoReq;

	getOpInfoReq = utlCalloc(1, sizeof(CiMmPrimGetNumPreferredOperatorsReq));
	if (getOpInfoReq == NULL)
		return CIRC_FAIL;

	getOpInfoReq->Index = opeIndex;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_GET_NETWORK_OPERATOR_INFO_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_GET_NETWORK_OPERATOR_INFO_REQ), (void *)getOpInfoReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MM_GetPrefOpeInfo
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get info of one preferred operator specified by index, part of implementation of AT+CPOL?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_GetPrefOpeInfo(UINT32 atHandle, int opeIndex)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMmPrimGetPreferredOperatorInfoReq *getPrefOperInfoReq;

	getPrefOperInfoReq = utlCalloc(1, sizeof(CiMmPrimGetPreferredOperatorInfoReq));
	if (getPrefOperInfoReq == NULL)
		return CIRC_FAIL;

	getPrefOperInfoReq->Index = opeIndex;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_GET_PREFERRED_OPERATOR_INFO_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_GET_PREFERRED_OPERATOR_INFO_REQ), (void *)getPrefOperInfoReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MM_GetSignalQuality
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get signal quality saved in local variable, implementation of AT+CSQ
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_GetSignalQuality(UINT32 atHandle, int *rssi, int *ber)
{
	UNUSEDPARAM(atHandle)
	CiReturnCode ret = CIRC_SUCCESS;

	*rssi = gSignalRssi;
	*ber = gSignalBer;
	return ret;
}


/****************************************************************************************
*  FUNCTION:  MM_GetSubscriber
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get MSISDN, implementation of AT+CNUM
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_GetSubscriber(UINT32 atHandle, int index)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMmPrimGetSubscriberInfoReq *getSubcriberInfoReq;

	getSubcriberInfoReq = utlCalloc(1, sizeof(CiMmPrimGetSubscriberInfoReq));
	if (getSubcriberInfoReq == NULL)
		return CIRC_FAIL;

	getSubcriberInfoReq->Index = index+1;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_GET_SUBSCRIBER_INFO_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_GET_SUBSCRIBER_INFO_REQ), (void *)getSubcriberInfoReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MM_GetSubscriber
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get MSISDN, implementation of AT+CNUM
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_GetNumberOfEntries(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_SUCCESS;
	CiMmPrimGetNumSubscriberNumbersReq *getNumSubscriberNumbersReq = NULL;

	memset(gAtReplyString,'\0',sizeof(gAtReplyString));

	ret = ciRequest(gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_GET_NUM_SUBSCRIBER_NUMBERS_REQ,
				MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_GET_NUM_SUBSCRIBER_NUMBERS_REQ), (void *)getNumSubscriberNumbersReq);

	return ret;
}


/****************************************************************************************
*  FUNCTION:  MM_SetNwIndMode
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to set network mode ind msg setting and save in local variable, implementation of AT+CIND=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_SetNwModeIndSetting(UINT32 atHandle, int bNwModeInd)
{
	CiReturnCode ret = CIRC_SUCCESS;
	CiMmPrimEnableNetworkModeIndReq *setNwModeIndReq;

	setNwModeIndReq = utlCalloc(1, sizeof(CiMmPrimEnableNetworkModeIndReq));
	if (setNwModeIndReq == NULL)
		return CIRC_FAIL;

	setNwModeIndReq->enable = bNwModeInd;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_ENABLE_NETWORK_MODE_IND_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_ENABLE_NETWORK_MODE_IND_REQ), (void *)setNwModeIndReq );
	gNwModeIndEnabled = bNwModeInd;

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MM_SetNwIndMode
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get network mode ind msg setting saved in local variable, implementation of AT+CIND?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_GetNwModeIndSetting(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_SUCCESS;
	char mmRspBuf[20];

	sprintf(mmRspBuf, "+CIND:%d", gNwModeIndEnabled);
	ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, mmRspBuf);
	return ret;
}

CiReturnCode MM_SetCellLOCK(UINT32 atHandle, UINT32 mode, UINT32 arfcn, UINT32 scramblingCode)
{
	CiReturnCode ret = CIRC_SUCCESS;
	CiMmPrimCellLockReq *MmCLReq;
	MmCLReq = utlCalloc(1, sizeof(CiMmPrimCellLockReq));
	if(MmCLReq == NULL)
		return CIRC_FAIL;
	MmCLReq->mode= mode;
	MmCLReq->arfcn = arfcn;
	MmCLReq->cellParameterId = scramblingCode;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_CELL_LOCK_REQ, MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_CELL_LOCK_REQ), (void *)MmCLReq );
	return ret;
}
/****************************************************************************************
 *  FUNCTION:  MM_GetSysInfo
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION: interface to get system information saved in local variable, implementation of AT^SYSINFO
 *
 *  RETURNS: CiReturnCode
 *
 ****************************************************************************************/
CiReturnCode MM_GetSysInfo(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_SUCCESS;
	AtciSysinfoServiceStatus currSrvStatus = ATCI_SYSINFO_SERVICE_STATUS_NO_SERVICE;
	AtciSysinfoServiceDomain currSrvDomain = ATCI_SYSINFO_SERVICE_DOMAIN_NO_SERVICE;
	AtciSysinfoRoamStatus currRoamStatus = ATCI_SYSINFO_ROAM_STATUS_NO_ROAMING;
	AtciSysinfoSimStatus currSimState = ATCI_SYSINFO_SIM_STATUS_INVALID;
	AtciSysinfoSystemMode currSysMode;

	char mmRspBuf[100];

	currSysMode = gSysMode;

	if( gSimStatus == CI_SIM_ST_READY )
		currSimState = ATCI_SYSINFO_SIM_STATUS_VALID;
	else if( gSimStatus == CI_SIM_ST_NOT_READY )
		currSimState = ATCI_SYSINFO_SIM_STATUS_NO_INSERT_UNCHECKED_UNBLOCKED;
	else if( gSimPinState > CI_SIM_PIN_ST_READY && gSimPinState < CI_SIM_PIN_ST_NETWORK_REJECTED )
		currSimState = ATCI_SYSINFO_SIM_STATUS_NO_INSERT_UNCHECKED_UNBLOCKED;


	switch ( gCurrRegStatus )
	{
	case CIMM_REGSTATUS_NOT_SEARCHING:
	case CIMM_REGSTATUS_SEARCHING:
	case CIMM_REGSTATUS_UNKNOWN:
	       currSrvStatus = ATCI_SYSINFO_SERVICE_STATUS_NO_SERVICE;
	       currSrvDomain = ATCI_SYSINFO_SERVICE_DOMAIN_NO_SERVICE;
	       break;

	case CIMM_REGSTATUS_HOME:
	       currSrvStatus = ATCI_SYSINFO_SERVICE_STATUS_VALID_SERVICE;
	       currSrvDomain = ATCI_SYSINFO_SERVICE_DOMAIN_CS_ONLY;
	       break;

	case CIMM_REGSTATUS_DENIED:
	       currSrvStatus = ATCI_SYSINFO_SERVICE_STATUS_RESTRICTED_AREA_SERVICE;
	       currSrvDomain = ATCI_SYSINFO_SERVICE_DOMAIN_NO_SERVICE;
	       break;

	case CIMM_REGSTATUS_ROAMING:
	       currSrvStatus = ATCI_SYSINFO_SERVICE_STATUS_VALID_SERVICE;
	       currSrvDomain = ATCI_SYSINFO_SERVICE_DOMAIN_CS_ONLY;

	       currRoamStatus = ATCI_SYSINFO_ROAM_STATUS_ROAMING;
	       break;

	case CIMM_REGSTATUS_EMERGENCY_ONLY:
	       currSrvStatus = ATCI_SYSINFO_SERVICE_STATUS_RESTRICTED_SERVICE;
	       currSrvDomain = ATCI_SYSINFO_SERVICE_DOMAIN_NO_SERVICE;
	       break;

	default:
	       break;
	}

	switch ( gCurrentPSRegStatus )
	{
	case CI_PS_NW_REG_STA_NOT_REGED:
	case CI_PS_NW_REG_STA_TRYING:
	case CI_PS_NW_REG_STA_REG_DENIED:
	case CI_PS_NW_REG_STA_UNKNOWN:
	       break;

	case CI_PS_NW_REG_STA_REG_ROAMING:
	       currRoamStatus = ATCI_SYSINFO_ROAM_STATUS_ROAMING;
	case CI_PS_NW_REG_STA_REG_HPLMN:
	       if( currSrvDomain == ATCI_SYSINFO_SERVICE_DOMAIN_CS_ONLY)
			currSrvDomain = ATCI_SYSINFO_SERVICE_DOMAIN_CS_PS;
	       else
			currSrvDomain = ATCI_SYSINFO_SERVICE_DOMAIN_PS_ONLY;
	       break;

	 default:
	       break;
	 }

	sprintf(mmRspBuf, "^SYSINFO:%d,%d,%d,%d,%d\r\n", currSrvStatus, currSrvDomain, currRoamStatus, currSysMode, currSimState);
	ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, mmRspBuf);
	return ret;
}

/************************************************************************************
 *
 * Implementation of processing reply or indication of MM related AT commands. Called by msgCnf() or msgInd
 *
 *************************************************************************************/
static BOOL libCheckMMRet( CiRequestHandle atHandle, CiMmResultCode result )
{
	switch ( result )
	{
	case CIRC_MM_SUCCESS:
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	case CIRC_MM_FAIL:
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_PHONE_FAILURE, NULL);
		break;
	case CIRC_MM_INVALID_ADDRESS:
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_CHAR_IN_DIAL_STRING, NULL);
		break;
	case CIRC_MM_NO_SERVICE:
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_NO_NW_SERVICE, NULL);
		break;
	case CIRC_MM_TIMEOUT:
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_NW_TIMEOUT, NULL);
		break;
	case CIRC_MM_INCOMPLETE_INFO:
	case CIRC_MM_NOT_REGISTERED:
	case CIRC_MM_REJECTED:
	case CIRC_MM_UNAVAILABLE:
	case CIRC_MM_NO_MORE_ENTRIES:
	case CIRC_MM_NO_MORE_ROOM:
	default:
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		break;
	}

	return( FALSE );
}

/****************************************************************
 *  F@: processQueryRegOptionConf - Process CI cnf msg of AT+CREG=?
 */
static void processQueryRegOptionConf(UINT32 atHandle, const void *paras)
{
	char mmRspBuf [500], mmRspTmpBuf[300];
	int i;
	CiMmPrimGetSupportedRegResultOptionsCnf  *getSupportedRegOptCnf;

	/* NOTE: currently SAC doesn't support this primitive */
	getSupportedRegOptCnf = (CiMmPrimGetSupportedRegResultOptionsCnf *)paras;
	if ( getSupportedRegOptCnf->Result == CIRC_MM_SUCCESS )
	{
		if ( getSupportedRegOptCnf->NumOptions > 0 )
		{
			sprintf(mmRspBuf, "+CREG: (%d", getSupportedRegOptCnf->Option[0]);
			for ( i = 1; i < getSupportedRegOptCnf->NumOptions; i++ )
			{
				sprintf(mmRspTmpBuf, ",%d", getSupportedRegOptCnf->Option[i]);
				strcat(mmRspBuf, mmRspTmpBuf);
			}

			strcat(mmRspBuf, ")");
			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, mmRspBuf);
		}
		else
		{
			ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		}
	}
	else
	{
		libCheckMMRet(atHandle, getSupportedRegOptCnf->Result);
	}

	return;
}

/****************************************************************
 *  F@: processGetCurrRegOptionConf - Process CI cnf msg of AT+CREG? (not used now)
 */
static void processGetCurrRegOptionConf(UINT32 atHandle, const void *paras)
{
	CiMmPrimGetRegResultOptionCnf   *getRegOptCnf;

	/* NOTE: currently SAC doesn't support this primitive */
	getRegOptCnf = (CiMmPrimGetRegResultOptionCnf *)paras;
	if ( getRegOptCnf->Result == CIRC_MM_SUCCESS )
	{
		gCurrRegOption = getRegOptCnf->Option;
	}
	else
	{
		libCheckMMRet(atHandle, getRegOptCnf->Result);
	}
	return;
}

/****************************************************************
 *  F@: processGetCurrRegStatusConf - Process CI cnf msg of AT+CREG?
 */
static void processGetCurrRegStatusConf(UINT32 atHandle, const void *paras)
{
	char mmRspBuf [500], mmRspTmpBuf[300];
	CiMmPrimGetRegResultInfoCnf              *getRegInfoCnf;

	getRegInfoCnf = (CiMmPrimGetRegResultInfoCnf *)paras;
	if ( getRegInfoCnf->Result == CIRC_MM_SUCCESS )
	{
		gCurrRegStatus = getRegInfoCnf->RegStatus;
		//if ( regStatus == CIMM_REGSTATUS_EMERGENCY_ONLY )
		//	regStatus = CIMM_REGSTATUS_NOT_SEARCHING;
		/* this must be received for an AT+CREG? command */
		sprintf(mmRspBuf, "+CREG: %d,%d", gCurrRegOption, gCurrRegStatus);

		/* display the location info only if regOption is CIMM_REGRESULT_CELLINFO */
		if (( gCurrRegOption == CIMM_REGRESULT_CELLINFO ) &&
		    ( getRegInfoCnf->info.Present == TRUE ))
		{
			sprintf(mmRspTmpBuf, ",\"%04x\",\"%08x\",%d", getRegInfoCnf->info.LocArea, getRegInfoCnf->info.CellId, gActDetail);
			strcat(mmRspBuf, mmRspTmpBuf);
		}

		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, mmRspBuf);
	}
	else
	{
		libCheckMMRet(atHandle, getRegInfoCnf->Result);
	}

	return;
}

/****************************************************************
 *  F@: processQuerySuppOpeNumConf - Process CI cnf msg of AT+COPS=?
 */
static void processQuerySuppOpeNumConf(UINT32 atHandle, const void *paras)
{
	CiMmPrimGetNumNetworkOperatorsCnf  *getNumOpCnf = (CiMmPrimGetNumNetworkOperatorsCnf *)paras;

	if ( getNumOpCnf->Result == CIRC_MM_SUCCESS )
	{
		/* save the reported number of operators */
		gTotalSuppOpeNum = getNumOpCnf->NumOperators;

		if ( gTotalSuppOpeNum > 0 )
		{
			gCurrSuppOpeIndex = 1;
			MM_QuerySuppOpeInfo(atHandle, gCurrSuppOpeIndex);
			/* start the output text line */
			sprintf( gAtReplyString, "+COPS:" );
		}
		else
		{
			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+COPS:" );
		}
	}
	else
	{
		libCheckMMRet( atHandle, getNumOpCnf->Result );
	}

	return;
}

/****************************************************************
 *  F@: processQuerySuppOpeInfoConf - Process CI cnf msg of AT+COPS=?
 */
static void processQuerySuppOpeInfoConf(UINT32 atHandle, const void *paras)
{
	char mmRspBuf [500];
	char operIdLongStr[CIMM_MAX_OPER_ID_LEN + ATCI_NULL_TERMINATOR_LENGTH] = { 0 };
	char operIdShortStr[CIMM_MAX_OPER_ID_LEN + ATCI_NULL_TERMINATOR_LENGTH] = { 0 };
	CiMmPrimGetNetworkOperatorInfoCnf        *nwOperatorInfoCnf;

	nwOperatorInfoCnf = (CiMmPrimGetNetworkOperatorInfoCnf *)paras;
	CiMmAccTechMode ActDetail = nwOperatorInfoCnf->opStatus.AccTchMode;

	/* check if operation is still to be processed */
	if (gCurrSuppOpeIndex <= 0 )
	{
		return;
	}

	if ( nwOperatorInfoCnf->Result == CIRC_MM_SUCCESS )
	{
		if ( nwOperatorInfoCnf->opStatus.LongAlphaId.Length > 0 )
		{
			memcpy( operIdLongStr, nwOperatorInfoCnf->opStatus.LongAlphaId.Id,
				nwOperatorInfoCnf->opStatus.LongAlphaId.Length );
			operIdLongStr[nwOperatorInfoCnf->opStatus.LongAlphaId.Length] = 0;
		}

		if ( nwOperatorInfoCnf->opStatus.ShortAlphaId.Length > 0 )
		{
			memcpy( operIdShortStr, nwOperatorInfoCnf->opStatus.ShortAlphaId.Id,
				nwOperatorInfoCnf->opStatus.ShortAlphaId.Length );
			operIdShortStr[nwOperatorInfoCnf->opStatus.ShortAlphaId.Length] = 0;
		}

		sprintf( mmRspBuf, "(%d, \"%s\", \"%s\", \"%03x%02x\", %d)", nwOperatorInfoCnf->opStatus.Status,
			 operIdLongStr,
			 operIdShortStr,
			 nwOperatorInfoCnf->opStatus.NetworkId.CountryCode,
			 nwOperatorInfoCnf->opStatus.NetworkId.NetworkCode,
			 ActDetail );
		strcat(gAtReplyString, mmRspBuf);

		if ( gCurrSuppOpeIndex < gTotalSuppOpeNum )
		{
			strcat(gAtReplyString, ", ");
			gCurrSuppOpeIndex++;

			MM_QuerySuppOpeInfo(atHandle, gCurrSuppOpeIndex);
		}
		else
		{
			/* added mode(s) and format(s)*/
			strcat(gAtReplyString, ",,(0,1,2,3,4), (0,1,2)");
			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, gAtReplyString);
			gTotalSuppOpeNum = 0;
			gCurrSuppOpeIndex = 0;
		}

	}  /* if (result) */
	else
	{
		libCheckMMRet( atHandle, nwOperatorInfoCnf->Result );
		gTotalSuppOpeNum = 0;
		gCurrSuppOpeIndex = 0;
	}

	return;
}

/****************************************************************
 *  F@: processGetCurrOpeInfoConf - Process CI cnf msg of AT+COPS?
 */
static void processGetCurrOpeInfoConf(UINT32 atHandle, const void *paras)
{
	char mmRspBuf [500];
	char operIdLongStr[CIMM_MAX_OPER_ID_LEN + 1] = { 0 };
	AtciRegMode curRegMode;
	CiMmPrimGetCurrentOperatorInfoCnf   *curOperatorInfoCnf;

	curOperatorInfoCnf = (CiMmPrimGetCurrentOperatorInfoCnf *)paras;
	CiMmAccTechMode ActDetail = curOperatorInfoCnf->AccTchMode;
	if ( curOperatorInfoCnf->Result == CIRC_MM_SUCCESS )
	{
		/* this was a request for initialization purpose => don't display anything */
		if ( gAccTchMode == CI_MM_NUM_ACT )
		{
			/* need to know the access technology for the data transfer */
			gAccTchMode = curOperatorInfoCnf->AccTchMode;
#if 0                                                   //inherit code
			if (gCurrMsgReq == 0)
				return;                 /* not elegant, but... simplifies the code */
			else
				gCurrMsgReq = 0;        //We need to return the result to TE
#endif
		}

		/* convert the registration mode */
		switch ( curOperatorInfoCnf->RegMode )
		{
		case CIMM_REGMODE_AUTOMATIC:
			curRegMode = ATCI_REGMODE_AUTOMATIC;
			break;
		case CIMM_REGMODE_MANUAL:
			curRegMode = ATCI_REGMODE_MANUAL;
			break;
		case CIMM_REGMODE_DEREGISTER:
			curRegMode = ATCI_REGMODE_DEREGISTER;
			break;
		case CIMM_REGMODE_MANUAL_AUTO:
			curRegMode = ATCI_REGMODE_MANUAL_AUTO;
			break;
		}

		if ( curOperatorInfoCnf->info.Present )
		{
			switch (curOperatorInfoCnf->info.Format)
			{
			case CIMM_NETOP_ID_FORMAT_ALPHA_LONG:
			case CIMM_NETOP_ID_FORMAT_ALPHA_SHORT:
			{
				if ( curOperatorInfoCnf->info.CiMmNetOpId.OperatorId.Length > 0 )
				{
					strncpy((char *)operIdLongStr, curOperatorInfoCnf->info.CiMmNetOpId.OperatorId.Id, curOperatorInfoCnf->info.CiMmNetOpId.OperatorId.Length);
				}
				break;
			}

			case CIMM_NETOP_ID_FORMAT_NETWORK:
			{
				sprintf( (char *)operIdLongStr, "%03x%02x",
					 curOperatorInfoCnf->info.CiMmNetOpId.NetworkId.CountryCode,
					 curOperatorInfoCnf->info.CiMmNetOpId.NetworkId.NetworkCode );
				break;
			}

			default:
				/* invalid format; only mode will be displayed */
				curOperatorInfoCnf->info.Present = FALSE;
				break;
			}
		}

		if ( !curOperatorInfoCnf->info.Present )
		{
			sprintf( mmRspBuf, "+COPS: %d", curRegMode );
		}
		else
		{
#ifdef AT_CUSTOMER_HTC
			sprintf( mmRspBuf, "+COPS: %d,%d,\"%s\",%d", curRegMode,
				 curOperatorInfoCnf->info.Format, operIdLongStr, gActDetail);
#else
			sprintf( mmRspBuf, "+COPS: %d,%d,\"%s\",%d", curRegMode,
				 curOperatorInfoCnf->info.Format, operIdLongStr, ActDetail );
#endif
		}

		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, mmRspBuf);

	}  /* if (result) */
	else
	{
		libCheckMMRet( atHandle, curOperatorInfoCnf->Result );
	}

	return;
}

/****************************************************************
 *  F@: processGetPrefOpeNumConf - Process CI cnf msg of AT+CPOL?
 */
static void processGetPrefOpeNumConf(UINT32 atHandle, const void *paras)
{
	CiMmPrimGetNumPreferredOperatorsCnf   *getNumPrefOperCnf = (CiMmPrimGetNumPreferredOperatorsCnf *)paras;

	if ( getNumPrefOperCnf->Result == CIRC_MM_SUCCESS )
	{
		/* save the reported number of pref operators */
		gTotalPrefOpeNum = getNumPrefOperCnf->NumPref;

		if ( gTotalPrefOpeNum > 0 )
		{
			gCurrPrefOpeIndex = 1;
			MM_GetPrefOpeInfo(atHandle, gCurrPrefOpeIndex);
		}
		else
		{
			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL );
		}
	}
	else
	{
		libCheckMMRet( atHandle, getNumPrefOperCnf->Result );
	}

	return;
}

/****************************************************************
 *  F@: processGetPrefOpeInfoConf - Process CI cnf msg of AT+CPOL?
 */
static void processGetPrefOpeInfoConf(UINT32 atHandle, const void *paras)
{
	char mmRspBuf [500];
	char operIdLongStr[CIMM_MAX_OPER_ID_LEN + ATCI_NULL_TERMINATOR_LENGTH] = { 0 };
	CiMmPrimGetPreferredOperatorInfoCnf *getPrefOperInfoCnf = (CiMmPrimGetPreferredOperatorInfoCnf *)paras;

	if ( getPrefOperInfoCnf->Result == CIRC_MM_SUCCESS )
	{
		if ( getPrefOperInfoCnf->info.Present )
		{
			switch (getPrefOperInfoCnf->info.Format)
			{
			case CIMM_NETOP_ID_FORMAT_ALPHA_LONG:
			case CIMM_NETOP_ID_FORMAT_ALPHA_SHORT:
			{
				if ( getPrefOperInfoCnf->info.CiMmNetOpId.OperatorId.Length > 0 )
				{
					memcpy(operIdLongStr, getPrefOperInfoCnf->info.CiMmNetOpId.OperatorId.Id, getPrefOperInfoCnf->info.CiMmNetOpId.OperatorId.Length);
					operIdLongStr[getPrefOperInfoCnf->info.CiMmNetOpId.OperatorId.Length] = '\0';
				}
				break;
			}

			case CIMM_NETOP_ID_FORMAT_NETWORK:
			{
				sprintf( (char *)operIdLongStr, "%03x%02x",
					 getPrefOperInfoCnf->info.CiMmNetOpId.NetworkId.CountryCode,
					 getPrefOperInfoCnf->info.CiMmNetOpId.NetworkId.NetworkCode );
				break;
			}

			default:
				/* invalid format; only mode will be displayed */
				break;
			}
		}

		sprintf( (char *)mmRspBuf, "+CPOL: %d, %d, \"%s\"", gCurrPrefOpeIndex,
			 getPrefOperInfoCnf->info.Format,  operIdLongStr);

		switch(getPrefOperInfoCnf->AccTchMode){
			case ATCI_ACT_GSM:
				strcat((char *)mmRspBuf, (char *)", 1, 0, 0");
				break;
			case ATCI_ACT_GSM_COMPACT:
				strcat((char *)mmRspBuf, (char *)", 0, 1, 0");
				break;
			case ATCI_ACT_UTRAN:
				strcat((char *)mmRspBuf, (char *)", 0, 0, 1");
				break;
			default:
				break;
		}

		ATRESP( atHandle, 0, 0, (char *)mmRspBuf);

		if (gCurrPrefOpeIndex < gTotalPrefOpeNum)
		{
			gCurrPrefOpeIndex++;
			MM_GetPrefOpeInfo(atHandle, gCurrPrefOpeIndex);
		}
		else
		{
			ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
			gTotalPrefOpeNum = 0;
			gCurrPrefOpeIndex = 0;
		}

	}  /* if (result) */
	else
	{
		libCheckMMRet( atHandle, getPrefOperInfoCnf->Result );
		gTotalPrefOpeNum = 0;
		gCurrPrefOpeIndex = 0;
	}

	return;
}

/****************************************************************
 *  F@: processGetNitzInfoConf - Process CI cnf msg of AT+CTZR?
 */
static void processGetNitzInfoConf(UINT32 atHandle, const void *paras)
{
	char mmRspBuf [500];
	CiMmPrimGetNitzInfoCnf        *getNitzInfoCnf;

	getNitzInfoCnf = (CiMmPrimGetNitzInfoCnf*)paras;

	if (getNitzInfoCnf->Result == CIRC_MM_SUCCESS)
	{
		sprintf(mmRspBuf, "+CTZR: 1");
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, mmRspBuf);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
	}

	return;
}

/****************************************************************
 *  F@: processGetstarNitzInfoConf - Process CI cnf msg of AT*CTZR?
 */
static void processGetstarNitzInfoConf(UINT32 atHandle, const void *paras)
{
	char mmRspBuf [200];
	char temp[100];
	CiMmPrimGetNitzInfoCnf        *getNitzInfoCnf;
	UINT8 utYear, utMonth, utDay, utHour, utMinute, utSecond;
	INT8 locTimeZone, Timezone;

	getNitzInfoCnf = (CiMmPrimGetNitzInfoCnf*)paras;

	if (getNitzInfoCnf->Result == CIRC_MM_SUCCESS)
	{
		/* isTimePresent */
		sprintf(mmRspBuf, "*CTZR:%d", getNitzInfoCnf->info.uniTimePresent);

		if ( (TRUE == getNitzInfoCnf->info.uniTimePresent) )
		{
			utYear			= ( getNitzInfoCnf->info.uniTime.year & 0x0f ) * 10 + ( ( getNitzInfoCnf->info.uniTime.year & 0xf0 ) >> 4 );
			utMonth 		= ( getNitzInfoCnf->info.uniTime.month & 0x0f ) * 10 + ( ( getNitzInfoCnf->info.uniTime.month & 0xf0 ) >> 4 );
			utDay			= ( getNitzInfoCnf->info.uniTime.day & 0x0f ) * 10 + ( ( getNitzInfoCnf->info.uniTime.day & 0xf0 ) >> 4 );
			utHour			= ( getNitzInfoCnf->info.uniTime.hour & 0x0f ) * 10 + ( ( getNitzInfoCnf->info.uniTime.hour & 0xf0 ) >> 4 );
			utMinute		= ( getNitzInfoCnf->info.uniTime.minute & 0x0f ) * 10 + ( ( getNitzInfoCnf->info.uniTime.minute & 0xf0 ) >> 4 );
			utSecond		= ( getNitzInfoCnf->info.uniTime.second & 0x0f ) * 10 + ((  getNitzInfoCnf->info.uniTime.second & 0xf0 ) >> 4 );
			Timezone		= ( ( getNitzInfoCnf->info.uniTime.locTimeZone & 0x07 ) * 10 + ( ( getNitzInfoCnf->info.uniTime.locTimeZone & 0xf0 ) >> 4) );

			if ( getNitzInfoCnf->info.uniTime.locTimeZone & 0x08 )
			{
				locTimeZone = Timezone * (-1);
			}
			else
			{
				locTimeZone = Timezone;
			}

			if (TRUE == getNitzInfoCnf->info.locTimeZonePresent)
			{
				Timezone = ( ( getNitzInfoCnf->info.locTimeZone & 0x07 ) * 10 + ( ( getNitzInfoCnf->info.locTimeZone & 0xf0 ) >> 4 ) );
				if ( getNitzInfoCnf->info.locTimeZone & 0x08 )
				{
					locTimeZone = Timezone * (-1);
				}
				else
				{
					locTimeZone = Timezone;
				}
			}

			/* Year [ 00..99 ], Month [ 1..12 ] , Day [ 1..31 ] ,Hour [ 0..59 ],
			     Minute [ 0..59 ] , Second [ 0..59 ], +/-,Local time zone */
			sprintf(temp, ",%d,%d,%d,%d,%d,%d,%c,%d", utYear,
												   utMonth,
												   utDay,
												   utHour,
												   utMinute,
												   utSecond,
												   (locTimeZone >= 0? '+':'-'),
												   Timezone);

			strcat(mmRspBuf, temp);

		}

		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, mmRspBuf);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
	}

	return;
}

/****************************************************************
 *  F@: processGetBandIndInfoConf - Process CI cnf msg of AT*BANDIND?
 */
static void processGetBandIndInfoConf(UINT32 atHandle, const void *paras)
{
	CiMmPrimGetBandIndCnf *getBandIndCnf = (CiMmPrimGetBandIndCnf*)paras;
	char mmRspBuf[60];

	if ( getBandIndCnf->result == CIRC_MM_SUCCESS )
	{
		if (getBandIndCnf->currentBand.accessTechnology == CI_MM_ACT_GSM)
		{
			sprintf( mmRspBuf, "*BANDIND: %d, %d, %d\r\n", (UINT8)getBandIndCnf->enableBandInd,
				 getBandIndCnf->currentBand.currentBand.gsmBand,
				 getBandIndCnf->currentBand.accessTechnology);
		}
		else if (getBandIndCnf->currentBand.accessTechnology == CI_MM_ACT_GSM_COMPACT)
		{
			sprintf( mmRspBuf, "*BANDIND: %d, %d, %d\r\n", (UINT8)getBandIndCnf->enableBandInd, getBandIndCnf->currentBand.currentBand.gsmCompactBand, getBandIndCnf->currentBand.accessTechnology);
		}
		else if (getBandIndCnf->currentBand.accessTechnology == CI_MM_ACT_UTRAN)
		{
			sprintf( mmRspBuf, "*BANDIND: %d, %d, %d\r\n", (UINT8)getBandIndCnf->enableBandInd, getBandIndCnf->currentBand.currentBand.umtsBand, getBandIndCnf->currentBand.accessTechnology);
		}
		else
			sprintf( mmRspBuf, "*BANDIND: %d\r\n", (UINT8)getBandIndCnf->enableBandInd);

		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, mmRspBuf);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
	}

}

/****************************************************************
 *  F@: processGetSubscriberNumberConf - Process CI cnf msg of AT+CNUM,
 */
static void processGetSubscriberNumberConf(UINT32 atHandle, const void *paras)
{

	CiMmPrimGetNumSubscriberNumbersCnf		*getNumSubscriberNumbersCnf;
	getNumSubscriberNumbersCnf = (CiMmPrimGetNumSubscriberNumbersCnf *)paras;
	gNumberOfSubscriber = 0;

	if(getNumSubscriberNumbersCnf->Result == CIRC_MM_SUCCESS){
		CiReturnCode ret = CIRC_SUCCESS;
		int index = 0;
		if(getNumSubscriberNumbersCnf->NumMSISDN > 0){
			for(index = 0; index < getNumSubscriberNumbersCnf->NumMSISDN && ret == CIRC_SUCCESS; index++){
				gNumberOfSubscriber++;
				ret = MM_GetSubscriber(atHandle, index);
			}

			if(ret != CIRC_SUCCESS){
				ATRESP(atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
			}
		}else{
			ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CNUM:\r\n");
		}
	}else{
		ATRESP(atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
	}
	return;
}

/****************************************************************
 *  F@: processGetSubscriberInfoConf - Process CI cnf msg of AT+CNUM,
 *  Response: +CNUM: [<alpha1>],<number1>,<type1>[,<speed>,<service>[,<itc>]]
 */
static void processGetSubscriberInfoConf(UINT32 atHandle, const void *paras)
{
	char mmRspBuf [500], tempBuf[100];
	char name[CI_MAX_NAME_LENGTH * 4 + ATCI_NULL_TERMINATOR_LENGTH], number[CI_MAX_ADDRESS_LENGTH + ATCI_NULL_TERMINATOR_LENGTH + 1];
	int type, speed, service, itc;
	CiMmPrimGetSubscriberInfoCnf  *getSubscriberInfoCnf;

	getSubscriberInfoCnf = (CiMmPrimGetSubscriberInfoCnf*)paras;

	memset(name,'\0',sizeof(name));
	memset(number,'\0',sizeof(number));
	gNumberOfSubscriber--;
	
	if (getSubscriberInfoCnf->Result == CIRC_MM_SUCCESS)
	{
		if(getSubscriberInfoCnf->info.Number.AddrType.NumType == CI_NUMTYPE_INTERNATIONAL)
			strcpy(number, "+");
		else
			number[0] = '\0';

		strncat(number, (char *)(getSubscriberInfoCnf->info.Number.Digits),getSubscriberInfoCnf->info.Number.Length);
		type = DialNumTypeCI2GSM(getSubscriberInfoCnf->info.Number.AddrType.NumType);  // ?: what should be reported in At reply? NumType or NumPlan?
		if (getSubscriberInfoCnf->info.AlphaTag.Present && (getSubscriberInfoCnf->info.AlphaTag.NameInfo.Length > 0))
		{
			int ret = pb_decode_character((char *)(getSubscriberInfoCnf->info.AlphaTag.NameInfo.Name), getSubscriberInfoCnf->info.AlphaTag.NameInfo.Length, 
				name, sizeof(name) / sizeof(name[0]) - 1, chset_type);
			name[ret] = '\0';
		}
		else
		{
			name[0] = '\0';
		}

		sprintf(mmRspBuf, "+CNUM: \"%s\",\"%s\",%d", name, number, type);

		if (getSubscriberInfoCnf->info.SvcInfo.Present)
		{
			speed = getSubscriberInfoCnf->info.SvcInfo.Speed;
			service = getSubscriberInfoCnf->info.SvcInfo.SvcType;
			itc = getSubscriberInfoCnf->info.SvcInfo.Itc;
			sprintf(tempBuf, ",%d,%d,%d", speed, service, itc);
			strcat(mmRspBuf, tempBuf);
		}

		strcat(gAtReplyString,mmRspBuf);
		strcat(gAtReplyString,(char *)"\r\n");
	}

	if( gNumberOfSubscriber == 0 )
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, gAtReplyString);

	return;
}

/****************************************************************
 *  F@: processGetPlmnConf - Process CI cnf msg of AT+CPLS?,
 *  Response: +CPLS: <ListIndex>
 */
static void processGetPlmnConf(UINT32 atHandle, const void *paras)
{
	char mmRspBuf[50];
	CiMmPrimGetPreferredPlmnListCnf *pGetPlmnCnf;

	pGetPlmnCnf = (CiMmPrimGetPreferredPlmnListCnf *)paras;

	if (pGetPlmnCnf->Result == CIRC_MM_SUCCESS)
	{
		sprintf(mmRspBuf, "+CPLS:%d", pGetPlmnCnf->ListIndex);
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, mmRspBuf);
	}
	else
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
	}
}
static void processSetCellLockConf(UINT32 atHandle, const void *paras)
{
	CiMmPrimCellLockCnf * pCellLockCnf = (CiMmPrimCellLockCnf *)paras;
	libCheckMMRet(atHandle, pCellLockCnf->result);
}

/****************************************************************************************
*
*  DESCRIPTION: Callback function when receiving confirmation CI message related with MM
*
****************************************************************************************/

void mmCnf(CiSgOpaqueHandle opSgCnfHandle,
	   CiServiceGroupID svgId,
	   CiPrimitiveID primId,
	   CiRequestHandle reqHandle,
	   void*             paras)
{
	UNUSEDPARAM(opSgCnfHandle)

	UINT32 atHandle = GET_AT_HANDLE(reqHandle);

	DBGMSG("%s: svgId: %d, primId: %d, reqHanlde: %d\n", __FUNCTION__, svgId, primId, reqHandle);

	/* Validate the para pointer */
	if ( (paras == NULL) && (primId < CI_MM_PRIM_LAST_COMMON_PRIM) )
	{
		ERRMSG("para pointer of CI cnf msg is NULL!");
		return;
	}

	switch (primId)
	{
	/* Conf msg of +CREG */
	case CI_MM_PRIM_SET_REGRESULT_OPTION_CNF:
	{
		CiMmPrimSetRegResultOptionCnf  *setRegOptCnf;
		setRegOptCnf = (CiMmPrimSetRegResultOptionCnf *)paras;
		if ( setRegOptCnf->Result == CIRC_MM_SUCCESS )
		{
			gCurrRegOption = GET_REQ_ID(reqHandle);
		}
		libCheckMMRet( atHandle, setRegOptCnf->Result );
		break;
	}

	case CI_MM_PRIM_GET_SUPPORTED_REGRESULT_OPTIONS_CNF:
	{
		processQueryRegOptionConf(atHandle, paras);
		break;
	}

	case CI_MM_PRIM_GET_REGRESULT_OPTION_CNF:
	{
		processGetCurrRegOptionConf(atHandle, paras);
		break;
	}

	case CI_MM_PRIM_GET_REGRESULT_INFO_CNF:
	{
		processGetCurrRegStatusConf(atHandle, paras);
		break;
	}

	/* Conf msg of +COPS */
	case CI_MM_PRIM_GET_NUM_NETWORK_OPERATORS_CNF:
	{
		processQuerySuppOpeNumConf(atHandle, paras);
		break;
	}

	case CI_MM_PRIM_GET_NETWORK_OPERATOR_INFO_CNF:
	{
		processQuerySuppOpeInfoConf(atHandle, paras);
		break;
	}

	case CI_MM_PRIM_GET_CURRENT_OPERATOR_INFO_CNF:
	{
		processGetCurrOpeInfoConf(atHandle, paras);
		break;
	}

	case CI_MM_PRIM_DEREGISTER_CNF:
	{
		CiMmPrimDeregisterCnf   *deregCnf;
		deregCnf = (CiMmPrimDeregisterCnf *)paras;
		libCheckMMRet( atHandle, deregCnf->Result );
		break;
	}

	case CI_MM_PRIM_AUTO_REGISTER_CNF:
	{
		CiMmPrimAutoRegisterCnf   *autoRegCnf;
		autoRegCnf = (CiMmPrimAutoRegisterCnf *)paras;
		libCheckMMRet( atHandle, autoRegCnf->Result );
		break;
	}

	case CI_MM_PRIM_MANUAL_REGISTER_CNF:
	{
		CiMmPrimManualRegisterCnf  *manRegCnf;
		manRegCnf = (CiMmPrimManualRegisterCnf *)paras;
		libCheckMMRet( atHandle, manRegCnf->Result );
		break;
	}

	/* Conf msg of +CPOL */
	case CI_MM_PRIM_GET_NUM_PREFERRED_OPERATORS_CNF:
	{
		processGetPrefOpeNumConf(atHandle, paras);
		break;
	}

	case CI_MM_PRIM_GET_PREFERRED_OPERATOR_INFO_CNF:
	{
		processGetPrefOpeInfoConf(atHandle, paras);

		break;
	}

	case CI_MM_PRIM_ADD_PREFERRED_OPERATOR_CNF:
	{
		CiMmPrimAddPreferredOperatorCnf  *addPrefOperCnf;
		addPrefOperCnf = (CiMmPrimAddPreferredOperatorCnf*)paras;
		libCheckMMRet( atHandle, addPrefOperCnf->Result );
		break;
	}

	case CI_MM_PRIM_DELETE_PREFERRED_OPERATOR_CNF:
	{
		CiMmPrimDeletePreferredOperatorCnf *delPrefOperCnf;
		delPrefOperCnf = (CiMmPrimDeletePreferredOperatorCnf*)paras;
		libCheckMMRet( atHandle, delPrefOperCnf->Result );
		break;
	}

	/* Conf msg of +CTZR */
	case CI_MM_PRIM_GET_NITZ_INFO_CNF:
	{
		if (GET_REQ_ID(reqHandle) == CI_MM_PRIM_GET_STAR_NITZ_REQ)
		{
			/* conf msg of *CTZR */
			processGetstarNitzInfoConf(atHandle, paras);
		}
		else
		{
			processGetNitzInfoConf(atHandle, paras);
		}
		break;
	}

	case CI_MM_PRIM_SET_ID_FORMAT_CNF:
	{
		CiMmPrimSetIdFormatCnf  *setFormatCnf;
		setFormatCnf = (CiMmPrimSetIdFormatCnf *)paras;
		libCheckMMRet( atHandle, setFormatCnf->Result );
		break;
	}

	case CI_MM_PRIM_GET_NUM_SUBSCRIBER_NUMBERS_CNF:
	{
		processGetSubscriberNumberConf(atHandle, paras);
		break;
	}

	case CI_MM_PRIM_GET_SUBSCRIBER_INFO_CNF:
	{
		processGetSubscriberInfoConf(atHandle, paras);
		break;
	}

	/* Conf msg of +CIND */
	case CI_MM_PRIM_ENABLE_NETWORK_MODE_IND_CNF:
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, 0);
		break;
	}
	case CI_MM_PRIM_GET_PREFERRED_PLMN_LIST_CNF:         //Conf for AT+CPLS?
	{
		processGetPlmnConf(atHandle, paras);
		break;
	}
	case CI_MM_PRIM_SELECT_PREFERRED_PLMN_LIST_CNF:        //Conf for AT+CPLS=
	{
		CiMmPrimSelectPreferredPlmnListCnf *pSelectPlmnCnf;
		pSelectPlmnCnf = (CiMmPrimSelectPreferredPlmnListCnf *)paras;
		libCheckMMRet(atHandle, pSelectPlmnCnf->Result);
		break;
	}
	case CI_MM_PRIM_SET_BANDIND_CNF:
	{
		CiMmPrimSetBandIndCnf *pSetBandIndCnf;
		pSetBandIndCnf = (CiMmPrimSetBandIndCnf*)paras;

		libCheckMMRet(atHandle, pSetBandIndCnf->result);
		break;
	}
	case CI_MM_PRIM_GET_BANDIND_CNF:
	{
		processGetBandIndInfoConf(atHandle, paras);
		break;
	}
	case CI_MM_PRIM_CELL_LOCK_CNF:
	{
		processSetCellLockConf(atHandle, paras);
		break;
	}
	default:
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		break;
	}

	/* free up the confirmation memory */
	atciSvgFreeCnfMem( svgId, primId, paras );

	return;
}

/****************************************************************
 *  F@: processRegStatusInd - Process CI ind msg of +CREG:
 */
static void processRegStatusInd(const void *pParam)
{
	char mmIndBuf[300];

	CiMmPrimRegResultInd    *regResultInd;

	regResultInd = (CiMmPrimRegResultInd *)pParam;
	gCurrRegStatus = regResultInd->RegStatus;
	/* show the registration status */
	switch ( gCurrRegOption )
	{
	case CIMM_REGRESULT_DISABLE:
		/* The indication msg should be received since it is disabled */
		break;

	case CIMM_REGRESULT_STATUS:
		//if ( currRegStatus == CIMM_REGSTATUS_EMERGENCY_ONLY )
		//	currRegStatus = CIMM_REGSTATUS_NOT_SEARCHING;
		sprintf(mmIndBuf, "+CREG: %d\r\n", gCurrRegStatus);
		ATRESP( IND_REQ_HANDLE, 0, 0, mmIndBuf);
		break;

	case CIMM_REGRESULT_CELLINFO:
		//if ( currRegStatus == CIMM_REGSTATUS_EMERGENCY_ONLY )
		//	currRegStatus = CIMM_REGSTATUS_NOT_SEARCHING;
		if ( regResultInd->info.Present == TRUE )
		{
			sprintf( mmIndBuf, "+CREG: %d,\"%04x\",\"%08x\",%d\r\n", gCurrRegStatus,
				 regResultInd->info.LocArea, regResultInd->info.CellId, gActDetail);
			ATRESP( IND_REQ_HANDLE, 0, 0, mmIndBuf);
		}
		else
		{
			sprintf(mmIndBuf, "+CREG: %d\r\n", gCurrRegStatus);
			ATRESP( IND_REQ_HANDLE, 0, 0, mmIndBuf);
		}
		break;

	default:
		break;
	}
	if( regResultInd->RegStatus == CIMM_REGSTATUS_UNKNOWN)
       {
	        gSysMode = ATCI_SYSINFO_SYSTEMO_MODE_NO_SERVICE;
                sprintf(mmIndBuf, "^MODE:%d\r\n", gSysMode);
                ATRESP( IND_REQ_HANDLE,0,0,mmIndBuf);
	}
	return;
}


/****************************************************************
 *  F@: processNwModeInd - Process CI ind msg of network mode
 *<AcT>: access technology of the registered network (defined in 3GPP spec 27.007 +CREG/CGREG)
 * 0	GSM
 * 1	GSM Compact
 * 2	UTRAN
 * 3	GSM w/EGPRS (see NOTE 1)
 * 4	UTRAN w/HSDPA (see NOTE 2)
 * 5	UTRAN w/HSUPA (see NOTE 2)
 * 6	UTRAN w/HSDPA and HSUPA (see NOTE 2)
 */
static void processNwModeInd(const void *pParam)
{
	CiMmPrimNetworkModeInd  *nwModeInd = (CiMmPrimNetworkModeInd *)pParam;
	char mmIndBuf[20];
	AtciSysinfoSystemMode sysMode;
	/* First we should judge according to the network mode */
	switch (nwModeInd->mode)
	{
	case CI_MM_NETWORK_MODE_REPORT_GSM:
	case CI_MM_NETWORK_MODE_REPORT_GSM_GPRS:
		gActDetail = 0;
		break;
	case CI_MM_NETWORK_MODE_REPORT_UMTS:
		gActDetail = 2;
		break;
#ifdef AT_CUSTOMER_HTC
	case CI_MM_NETWORK_MODE_REPORT_UMTS_HSDPA:
	case CI_MM_NETWORK_MODE_REPORT_UMTS_HSUPA:
	case CI_MM_NETWORK_MODE_REPORT_UMTS_HSDPA_HSUPA:
		gActDetail = 4;
		break;
#else
	case CI_MM_NETWORK_MODE_REPORT_UMTS_HSDPA:
		gActDetail = 4;
		break;
	case CI_MM_NETWORK_MODE_REPORT_UMTS_HSUPA:
		gActDetail = 5;
		break;
	case CI_MM_NETWORK_MODE_REPORT_UMTS_HSDPA_HSUPA:
		gActDetail = 6;
		break;
#endif
	case CI_MM_NETWORK_MODE_REPORT_GSM_EGPRS:
		gActDetail = 3;
		break;
	default:
		gActDetail = 1;
	}
	switch(nwModeInd->mode)
	{
	case CI_MM_NETWORK_MODE_REPORT_UMTS:
	case CI_MM_NETWORK_MODE_REPORT_UMTS_HSDPA:
	case CI_MM_NETWORK_MODE_REPORT_UMTS_HSUPA:
	case CI_MM_NETWORK_MODE_REPORT_UMTS_HSDPA_HSUPA:
	#ifdef TD_SUPPORT
		sysMode = ATCI_SYSINFO_SYSTEMO_MODE_TDSCDMA;
	#else
		sysMode = ATCI_SYSINFO_SYSTEMO_MODE_WCDMA;
	#endif
		if( gSysMode != sysMode)
	        {
			gSysMode = sysMode;
			sprintf(mmIndBuf, "^MODE:%d\r\n", gSysMode);
			ATRESP( IND_REQ_HANDLE,0,0,mmIndBuf);
                }
                break;

	case CI_MM_NETWORK_MODE_REPORT_GSM:
        case CI_MM_NETWORK_MODE_REPORT_GSM_EGPRS:
	case CI_MM_NETWORK_MODE_REPORT_GSM_GPRS:
		if( gSysMode != ATCI_SYSINFO_SYSTEMO_MODE_GSM_GPRS)
	        {
			gSysMode = ATCI_SYSINFO_SYSTEMO_MODE_GSM_GPRS;
			sprintf(mmIndBuf, "^MODE:%d\r\n", gSysMode);
			ATRESP( IND_REQ_HANDLE,0,0,mmIndBuf);
		}
		break;

	default:
		DBGMSG("%s:unknown network mode : %d\n", __FUNCTION__, nwModeInd->mode);
		break;
	}
#if 0
	/* Then we can adjust the mode dynamically according to current usage */
	/* Get AcT from network mode ind para */
	if ( (nwModeInd->hsupaActive) && (nwModeInd->hsdpaActive))
	{
		gActDetail = 6;
	}
	else if ((nwModeInd->hsupaActive) && !(nwModeInd->hsdpaActive))
	{
		gActDetail = 5;
	}
	else if ( (nwModeInd->hsdpaActive) && !(nwModeInd->hsupaActive))
	{
		gActDetail = 4;
	}
	else if (nwModeInd->egprsActive)
	{
		gActDetail = 3;
	}
	else if (nwModeInd->gprsActive)
	{
		gActDetail = 0;
	}
#endif
	return;
}

/****************************************************************
 *  F@: processSignalQualityInfoInd - Process CI ind msg of +CSQ:
 */
static void processSignalQualityInfoInd(const void *pParam)
{
	char mmIndBuf[300];
	CiMmPrimSigQualityInfoInd  *signalQualityInd = (CiMmPrimSigQualityInfoInd *)pParam;

	if (signalQualityInd->info.Rssi <= -113)
	{
		gSignalRssi = 0;
	}
	else if ((signalQualityInd->info.Rssi > -113) && (signalQualityInd->info.Rssi <= -111))
	{
		gSignalRssi = 1;
	}
	/* -109 ~ -53dBm =>  2..30 */
	else if ((signalQualityInd->info.Rssi > -111) && (signalQualityInd->info.Rssi <= -53))
	{
		gSignalRssi = 0.5 * signalQualityInd->info.Rssi + 56.5;
	}
	else
	{
		gSignalRssi = 31;
	}

	gSignalBer = signalQualityInd->info.BER;

	sprintf(mmIndBuf, "+CSQ:%d,%d\r\n", gSignalRssi, gSignalBer);
	ATRESP( IND_REQ_HANDLE, 0, 0, mmIndBuf);
	return;
}

/****************************************************************
 *  F@: processNitzInfoInd - Process CI ind msg of +NITZ:
 */
static void processNitzInfoInd(const void *pParam)
{
	char mmIndBuf[300];
	char mmTmpBuf[300];
	UINT8 utYear, utMonth, utDay, utHour, utMinute, utSecond;
	INT8 utTimeZone, timeZoneInd;
	CiMmPrimNitzInfoInd *nitzInfoInd;

	nitzInfoInd = (CiMmPrimNitzInfoInd *)pParam;

	if (TRUE == nitzInfoInd->info.longAlphaIdPresent && nitzInfoInd->info.longAlphaId.Length != 0)
	{
		memcpy( mmTmpBuf,  nitzInfoInd->info.longAlphaId.Id, nitzInfoInd->info.longAlphaId.Length );
		mmTmpBuf[nitzInfoInd->info.longAlphaId.Length] = 0;
		sprintf( mmIndBuf, "*COPN:%d,%s\r\n", CIMM_NETOP_ID_FORMAT_ALPHA_LONG, mmTmpBuf );
		ATRESP( IND_REQ_HANDLE, 0, 0, mmIndBuf);
	}

	if (TRUE == nitzInfoInd->info.shortAlphaIdPresent && nitzInfoInd->info.shortAlphaId.Length != 0)
	{
		memcpy( mmTmpBuf,  nitzInfoInd->info.shortAlphaId.Id, nitzInfoInd->info.shortAlphaId.Length );
		mmTmpBuf[nitzInfoInd->info.shortAlphaId.Length] = 0;
		sprintf( mmIndBuf, "*COPN:%d,%s\r\n", CIMM_NETOP_ID_FORMAT_ALPHA_SHORT, mmTmpBuf );
		ATRESP( IND_REQ_HANDLE, 0, 0, mmIndBuf);
	}


	if(TRUE == nitzInfoInd->info.dstIndPresent)
	{
		sprintf( mmIndBuf, "+NITZ: %d", nitzInfoInd->info.dstInd);
	}
	else
	{
		sprintf( mmIndBuf, "+NITZ: 0");
	}


	if(TRUE == nitzInfoInd->info.uniTimePresent)
	{
		utYear          = ( nitzInfoInd->info.uniTime.year & 0x0f ) * 10 + ( ( nitzInfoInd->info.uniTime.year & 0xf0 ) >> 4 );
		utMonth         = ( nitzInfoInd->info.uniTime.month & 0x0f ) * 10 + ( ( nitzInfoInd->info.uniTime.month & 0xf0 ) >> 4 );
		utDay           = ( nitzInfoInd->info.uniTime.day & 0x0f ) * 10 + ( ( nitzInfoInd->info.uniTime.day & 0xf0 ) >> 4 );
		utHour          = ( nitzInfoInd->info.uniTime.hour & 0x0f ) * 10 + ( ( nitzInfoInd->info.uniTime.hour & 0xf0 ) >> 4 );
		utMinute        = ( nitzInfoInd->info.uniTime.minute & 0x0f ) * 10 + ( ( nitzInfoInd->info.uniTime.minute & 0xf0 ) >> 4 );
		utSecond        = ( nitzInfoInd->info.uniTime.second & 0x0f ) * 10 + ((  nitzInfoInd->info.uniTime.second & 0xf0 ) >> 4 );
		utTimeZone      = ( ( nitzInfoInd->info.uniTime.locTimeZone & 0x07 ) * 10 + ( ( nitzInfoInd->info.uniTime.locTimeZone & 0xf0 ) >> 4) );
		if ( nitzInfoInd->info.uniTime.locTimeZone & 0x08 )
		{
			utTimeZone = utTimeZone * (-1);
		}

		if (TRUE == nitzInfoInd->info.locTimeZonePresent)
		{
			timeZoneInd = ( ( nitzInfoInd->info.locTimeZone & 0x07 ) * 10 + ( ( nitzInfoInd->info.locTimeZone & 0xf0 ) >> 4 ) );
			if ( nitzInfoInd->info.locTimeZone & 0x08 )
			{
				timeZoneInd = timeZoneInd * (-1);
			}
		}
		else
		{
			timeZoneInd = utTimeZone;
		}

		// show the result at last. form required by Android: "yy/mm/dd,hh:mm:ss(+/-)tz,dt" in UTC
		sprintf( mmTmpBuf, "%02d/%02d/%02d,%02d:%02d:%02d", utYear, utMonth, utDay, utHour, utMinute, utSecond );

		if (timeZoneInd >= 0)
		{
			sprintf( mmIndBuf, "%s,+%d,%s\r\n", mmIndBuf, timeZoneInd, mmTmpBuf );
		}
		else
		{
			sprintf( mmIndBuf, "%s,-%d,%s\r\n", mmIndBuf, -timeZoneInd, mmTmpBuf );
		}

	}
	else if(TRUE == nitzInfoInd->info.locTimeZonePresent)
    {
		timeZoneInd = ( ( nitzInfoInd->info.locTimeZone & 0x07 ) * 10 + ( ( nitzInfoInd->info.locTimeZone & 0xf0 ) >> 4 ) );
		if ( nitzInfoInd->info.locTimeZone & 0x08 )
		{
			sprintf( mmIndBuf, "%s,-%d\r\n", mmIndBuf, timeZoneInd);
		}
		else
		{
			sprintf( mmIndBuf, "%s,+%d\r\n", mmIndBuf, timeZoneInd);
		}
    }
	else
	{
		sprintf( mmIndBuf, "%s\r\n", mmIndBuf);
	}
		
	
	ATRESP( IND_REQ_HANDLE, 0, 0, mmIndBuf);

	return;
}

/****************************************************************
 *  F@: processBandIndInfoInd - Process CI ind msg of *BandInd:
 */
static void processBandIndInfoInd(const void *pParam)
{
	CiMmPrimBandIndInd *bandIndInd = (CiMmPrimBandIndInd *)pParam;
	char mmIndBuf[60];

	if (bandIndInd->currentBand.accessTechnology == CI_MM_ACT_GSM)
	{
		sprintf( mmIndBuf, "*BANDIND: %d, %d\r\n", bandIndInd->currentBand.currentBand.gsmBand, bandIndInd->currentBand.accessTechnology);
	}
	else if (bandIndInd->currentBand.accessTechnology == CI_MM_ACT_GSM_COMPACT)
	{
		sprintf( mmIndBuf, "*BANDIND: %d, %d\r\n", bandIndInd->currentBand.currentBand.gsmCompactBand, bandIndInd->currentBand.accessTechnology);
	}
	else if (bandIndInd->currentBand.accessTechnology == CI_MM_ACT_UTRAN)
	{
		sprintf( mmIndBuf, "*BANDIND: %d, %d\r\n",  bandIndInd->currentBand.currentBand.umtsBand, bandIndInd->currentBand.accessTechnology);
	}
	else
		sprintf( mmIndBuf, "*BANDIND:\r\n");

	ATRESP(IND_REQ_HANDLE, 0, 0, mmIndBuf);

}

/****************************************************************
 *  F@: processServiceRestrictionsInd - Process CI ind msg of ServiceRestrictionsInd:
 */
static void processServiceRestrictionsInd(const void *pParam)
{
	CiMmPrimServiceRestrictionsInd *ServiceRestrictionsInd = (CiMmPrimServiceRestrictionsInd *)pParam;
	char mmIndBuf[60];

	sprintf( mmIndBuf, "+MSRI: %d\r\n",
		 ServiceRestrictionsInd->manualPlmnSelectionAllowed);

	ATRESP(IND_REQ_HANDLE, 0, 0, mmIndBuf);

}

static void processCellLockInd(const void *pParam)
{
	CiMmPrimCellLockInd * CellLockInd = (CiMmPrimCellLockInd *)pParam;
	char mmIndBuf[60];
	sprintf(mmIndBuf, "*CellLock:%d,%d,%d\r\n",CellLockInd->mode,CellLockInd->arfcn,CellLockInd->cellParameterId);
	ATRESP(IND_REQ_HANDLE, 0, 0, mmIndBuf);
}

static void processAirRejectCause(const void *pParam)
{
	CiMmPrimAirInterfaceRejectCauseInd * RejectcauseInd = (CiMmPrimAirInterfaceRejectCauseInd *)pParam;
	char mmIndBuf[60];


	DBGMSG("%s: gRejectCauseNumber: %d\n", __FUNCTION__, gRejectCauseNumber);

	//save last 10 history, if already reject 65535 times, all history is cleared and restart to save
	rejectCauseHistory[gRejectCauseNumber%MAX_REJECTCAUSE_HISTORY].gmmCause = RejectcauseInd->gmmCause;
	rejectCauseHistory[gRejectCauseNumber%MAX_REJECTCAUSE_HISTORY].mmCause = RejectcauseInd->mmCause;
	gRejectCauseNumber++;

#ifdef AT_CUSTOMER_HTC
	if(RejectcauseInd->mmRejectType < CI_MM_NUM_REJECT_TYPE)
	{
		switch(RejectcauseInd->mmRejectType)
		{
		case CI_MM_LUP_REJECT:
			sprintf(mmIndBuf, "%s: %X,%X\r\n", SPEC_ERROR_CODE_INDICATION_PREFIX, ATCI_HTC_ERROR_CLASS_LUP_REJECT, RejectcauseInd->mmCause);
			break;
		case CI_MM_AUTH_FAIL:
			sprintf(mmIndBuf, "%s: %X\r\n", SPEC_ERROR_CODE_INDICATION_PREFIX, ATCI_HTC_ERROR_CLASS_MM_AUTH_FAILURE);
			break;
		case CI_MM_CS_AUTH_RJECT:
		default:
			sprintf(mmIndBuf, "%s: %X\r\n", SPEC_ERROR_CODE_INDICATION_PREFIX, ATCI_HTC_ERROR_CLASS_NW_CS_AUTH_REJECT);
			break;
		}
		ATRESP(IND_REQ_HANDLE, 0, 0, mmIndBuf);
	}
	if(RejectcauseInd->gmmRejectType < CI_GMM_NUM_REJECT_TYPE)
	{
		switch(RejectcauseInd->gmmRejectType)
		{
		case CI_GMM_GPRS_ATTACH_REJECT:
			sprintf(mmIndBuf, "%s: %X,%X\r\n", SPEC_ERROR_CODE_INDICATION_PREFIX, ATCI_HTC_ERROR_CLASS_GPRS_ATTACH_REJECT, RejectcauseInd->gmmCause);
			break;
		case CI_GMM_RAU_REJECT:
			sprintf(mmIndBuf, "%s: %X,%X\r\n", SPEC_ERROR_CODE_INDICATION_PREFIX, ATCI_HTC_ERROR_CLASS_RAU_REJECT, RejectcauseInd->gmmCause);
			break;
		case CI_GMM_PS_SECURITY_FAIL:
			sprintf(mmIndBuf, "%s: %X\r\n", SPEC_ERROR_CODE_INDICATION_PREFIX, ATCI_HTC_ERROR_CLASS_PS_SECURITY_FAILURE);
			break;
		case CI_GMM_PS_AUTH_RJECT:
		default:
			sprintf(mmIndBuf, "%s: %X\r\n", SPEC_ERROR_CODE_INDICATION_PREFIX, ATCI_HTC_ERROR_CLASS_NW_PS_AUTH_REJECT);
			break;
		}
		ATRESP(IND_REQ_HANDLE, 0, 0, mmIndBuf);
	}
#else
	//report reject casue if AT*REJCAUSE=1
	if(gRejectCauseIndEnabled)
	{
		sprintf(mmIndBuf, "*REJCAUSE:%d,%d\r\n", RejectcauseInd->gmmCause, RejectcauseInd->mmCause);
		ATRESP(IND_REQ_HANDLE, 0, 0, mmIndBuf);
	}
#endif
}


/****************************************************************************************
*
*  DESCRIPTION: Callback function when receiving indication CI message related with MM
*
****************************************************************************************/

void mmInd(CiSgOpaqueHandle opSgOpaqueHandle,
	   CiServiceGroupID svgId,
	   CiPrimitiveID primId,
	   CiIndicationHandle indHandle,
	   void*               pParam)
{
	UNUSEDPARAM(opSgOpaqueHandle)
	UNUSEDPARAM(indHandle)

	DBGMSG("%s: svgId: %d, primId: %d\n", __FUNCTION__, svgId, primId);

	switch (primId)
	{
	case CI_MM_PRIM_REGRESULT_IND:
	{
		processRegStatusInd(pParam);
		break;
	}
	case CI_MM_PRIM_NITZ_INFO_IND:
	{
		processNitzInfoInd(pParam);
		break;
	}
	case CI_MM_PRIM_SIGQUALITY_INFO_IND:
	{
		processSignalQualityInfoInd(pParam);
		break;
	}
	case CI_MM_PRIM_NETWORK_MODE_IND:
	{
		processNwModeInd(pParam);
		break;
	}
	case CI_MM_PRIM_BANDIND_IND:
	{
		processBandIndInfoInd(pParam);
		break;
	}
	case CI_MM_PRIM_SERVICE_RESTRICTIONS_IND:
	{
		processServiceRestrictionsInd(pParam);
		break;
	}
	case CI_MM_PRIM_CELL_LOCK_IND:
	{
		 processCellLockInd(pParam);
		 break;
	}
	case CI_MM_PRIM_AIR_INTERFACE_REJECT_CAUSE_IND:
	{
		processAirRejectCause(pParam);
		break;
	}
	default:
		break;
	}

	/* free up the indication memory */
	atciSvgFreeIndMem( svgId, primId, pParam );

	return;
}

/****************************************************************************************
*  FUNCTION:  MM_GetCPLS
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get preferred PLMN list , implementation of AT+CPLS?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_GetCPLS(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMmPrimGetPreferredPlmnListReq          *pGetPreferredPlmnListReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_GET_PREFERRED_PLMN_LIST_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_GET_PREFERRED_PLMN_LIST_REQ), (void *)pGetPreferredPlmnListReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MM_SelectCPLS
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to Select Preferred PLMN list , implementation of AT+CPLS=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_SelectCPLS(UINT32 atHandle, UINT8 listIndex)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMmPrimSelectPreferredPlmnListReq       *pSelectPreferredPlmnListReq;


	pSelectPreferredPlmnListReq = utlCalloc(1, sizeof(CiMmPrimSelectPreferredPlmnListReq));
	if (pSelectPreferredPlmnListReq == NULL)
		return CIRC_FAIL;

	pSelectPreferredPlmnListReq->ListIndex = listIndex;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_SELECT_PREFERRED_PLMN_LIST_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_SELECT_PREFERRED_PLMN_LIST_REQ), (void *)pSelectPreferredPlmnListReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MM_GetBANDIND
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get BANDIND, implementation of AT*BANDIND?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_GetBANDIND(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMmPrimGetBandIndReq                *pGetBandIndReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_GET_BANDIND_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_GET_BANDIND_REQ), (void *)pGetBandIndReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MM_SetBANDIND
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to Set BANDIND, implementation of AT*BANDIND=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_SetBANDIND(UINT32 atHandle, BOOL bEnableBandInd)
{
	CiReturnCode ret = CIRC_FAIL;
	CiMmPrimSetBandIndReq                *pSetBandIndReq;

	pSetBandIndReq = utlCalloc(1, sizeof(CiMmPrimSetBandIndReq));
	if (pSetBandIndReq == NULL)
		return CIRC_FAIL;

	pSetBandIndReq->enableBandInd = bEnableBandInd;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_MM_PRIM_SET_BANDIND_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_MM_PRIM_SET_BANDIND_REQ), (void *)pSetBandIndReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MM_SetRejectCauseStatus
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to Set REJCAUSE, implementation of AT*REJCAUSE=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_SetRejectCauseStatus(UINT32 atHandle, int bEnableRejectCauseInd)
{
	CiReturnCode ret = CIRC_FAIL;

	gRejectCauseIndEnabled = bEnableRejectCauseInd;

	ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MM_GetRejectCauseStatus
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to Set REJCAUSE, implementation of AT*REJCAUSE=?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_GetRejectCauseStatus(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	char mmRspBuf [50];

	sprintf(mmRspBuf, "*REJCAUSE:%d\r\n", gRejectCauseIndEnabled);
	ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, mmRspBuf);

	return ret;
}

/****************************************************************************************
*  FUNCTION:  MM_GetRejectCauseHistory
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get REJCAUSE history, implementation of AT*REJCAUSE
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MM_GetRejectCauseHistory(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	char mmRspBuf [50];
	int i;

	DBGMSG("%s: gRejectCauseNumber: %d\n", __FUNCTION__, gRejectCauseNumber);


	for(i=0; i< gRejectCauseNumber && i<MAX_REJECTCAUSE_HISTORY; i++)
	{
		sprintf(mmRspBuf, "*REJCAUSE:%d,%d\r\n", rejectCauseHistory[(gRejectCauseNumber-i-1)%MAX_REJECTCAUSE_HISTORY].gmmCause, 
			rejectCauseHistory[(gRejectCauseNumber-i-1)%MAX_REJECTCAUSE_HISTORY].mmCause);
		ATRESP(atHandle, 0, 0, mmRspBuf);
	}

	ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);

	return ret;
}
//In order to reset MM paras in case of CP assertion
void resetMmParas(void)
{
	char mmIndBuf[100];
	gCurrRegStatus = CIMM_REGSTATUS_NOT_SEARCHING;
	gCurrRegOption = CIMM_REGRESULT_STATUS;
	gTotalSuppOpeNum = 0;
	gCurrSuppOpeIndex = 0;
	gTotalPrefOpeNum = 0;
	gCurrPrefOpeIndex = 0;
	memset(gAtReplyString,'\0',500);
	gAccTchMode = CI_MM_NUM_ACT;
	gNwModeIndEnabled = 0;
	gNumberOfSubscriber = 0;
	gActDetail = 0;
	gSysMode = ATCI_SYSINFO_SYSTEMO_MODE_NO_SERVICE;

	gSignalRssi = 99;
	gSignalBer = 99;
	sprintf(mmIndBuf, "+CSQ:%d,%d\r\n", gSignalRssi, gSignalBer);
	ATRESP( IND_REQ_HANDLE, 0, 0, mmIndBuf);
}


