/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************
*  Filename: telcc.c
*
*  Authors: Johnny He
*
*  Description: Process CC related AT commands
*
*  History:
*   May 12, 2008 - Creation of file

******************************************************************************/

#include "ci_api_types.h"
#include "ci_api.h"
#include "telatci.h"
#include "teldef.h"
#include "telconfig.h"
#include "telatparamdef.h"
#include "tel3gdef.h"
#include "telutl.h"
#include "telcc.h"
#include "cc_api.h"
#include "ss_api.h"

CiCcBasicCMode gCurrentDataMode = CICC_BASIC_CMODE_DATA;
UINT8 g_iCRCMode = TEL_AT_CRC_MODE_VAL_DEFAULT;
/*add for CMEE support*/
UINT8 g_uintErrorCodePresentationMode = TEL_AT_CMEE_MODE_VAL_DEFAULT;
/*add for CPUC*/
CiCcPrimSetPuctInfoReq gSetPuctInfoReq;

int    g_iCVHUMode = TEL_AT_CVHU_0_MODE_1; /* mode of AT+CVHU, default value 1 */

extern char gErrCauseBuf[];

#define DTMFCHAR "1234567890*#"

/************************************************************************************
 *
 * CC related AT commands functions
 *
 *************************************************************************************/
/* Added by Michal Bukai - AT+DS command*/
/************************************************************************************
 * F@: ciDS - GLOBAL API for AT+DS command
 *
 */
RETURNCODE_T  ciDS(            const utlAtParameterOp_T op,
			       const char                      *command_name_p,
			       const utlAtParameterValue_P2c parameter_values_p,
			       const size_t num_parameters,
			       const char                      *info_text_p,
			       unsigned int                    *xid_p,
			       void                            *arg_p)
{

	UNUSEDPARAM(command_name_p);
	UNUSEDPARAM(parameter_values_p);
	UNUSEDPARAM(num_parameters);
	UNUSEDPARAM(info_text_p);

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);
	int P0val, P1val, P2val, Nval;
	CiCcDataCompInfo dataCompInfo;

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT+DS=? */
	{
		ret = CC_GetSupportDS(atHandle);

		break;
	}

	case TEL_EXT_GET_CMD:         /* AT+DS? */
	{
		ret = CC_GetDS(atHandle);

		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+DS= */
	{
		if ( getExtValue( parameter_values_p, 0, (int*)&P0val, TEL_AT_DS_0_P0_VAL_MIN, TEL_AT_DS_0_P0_VAL_MAX, TEL_AT_DS_0_P0_VAL_DEFAULT ) == TRUE )
		{
			if ( getExtValue( parameter_values_p, 1, (int*)&Nval, TEL_AT_DS_1_N_VAL_MIN, TEL_AT_DS_1_N_VAL_MAX, TEL_AT_DS_1_N_VAL_DEFAULT ) == TRUE )
			{
				if ( getExtValue( parameter_values_p, 2, (int*)&P1val, TEL_AT_DS_2_P1_VAL_MIN, TEL_AT_DS_2_P1_VAL_MAX, TEL_AT_DS_2_P1_VAL_DEFAULT ) == TRUE )
				{
					if ( getExtValue( parameter_values_p, 3, (int*)&P2val, TEL_AT_DS_3_P2_VAL_MIN, TEL_AT_DS_3_P2_VAL_MAX, TEL_AT_DS_3_P2_VAL_DEFAULT ) == TRUE )
					{
						dataCompInfo.dir = (UINT8)P0val;

						if ( Nval == TEL_AT_DS_1_N_VAL_MAX)
							dataCompInfo.zNegRequired = TRUE;
						else
							dataCompInfo.zNegRequired = FALSE;

						dataCompInfo.maxDict = (UINT16)P1val;

						dataCompInfo.maxStrLen = (UINT8)P2val;

						ret = CC_SetDS(atHandle, &dataCompInfo);

					}
				}
			}
		}
		break;
	}
	case TEL_EXT_ACTION_CMD:           /* AT+DS */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}

/************************************************************************************
 * F@: ciReleaseCall - GLOBAL API for GCF ATD-command
 * Notes:  dail string end with ';' - voice call
 *          dail string end with '#' - packet domain request service or supplimentary service
 *          dail string end with number - data call
 */

RETURNCODE_T ciMakeCall(            const utlAtParameterOp_T op,
				    const char                      *command_name_p,
				    const utlAtParameterValue_P2c parameter_values_p,
				    const size_t num_parameters,
				    const char                      *info_text_p,
				    unsigned int                    *xid_p,
				    void                            *arg_p)
{
	UNUSEDPARAM(op)
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);
	CHAR dialDigits[ATCI_MAX_CMDSTR_LENGTH + ATCI_NULL_TERMINATOR_LENGTH];
	INT16 dialDigitsLen;
	UINT32 callType = gCurrentDataMode;
	CiCcCallOptions CallOption = CICC_CALLOPTIONS_NONE;

	memset(dialDigits, 0x0, sizeof(dialDigits));


	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	if ( getExtendedString((CHAR *)parameter_values_p[0].value.dial_string_p, &dialDigits[0], ATCI_MAX_CMDSTR_LENGTH, &dialDigitsLen) == FALSE )
		return rc;

	if (dialDigitsLen <= 0)
		return rc;

	if ( dialDigits[dialDigitsLen - 1 ] == ';' )
	{
		/* get rid of the semi collin */
		dialDigits[dialDigitsLen - 1 ] = 0;
		callType = CICC_BASIC_CMODE_VOICE;

		if ((dialDigitsLen > 1) && (dialDigits[dialDigitsLen - 2 ] == 'I'))
		{
			CallOption = CICC_CALLOPTIONS_CLIR_RESTRICT;
			dialDigits[dialDigitsLen - 2 ] = 0;
		}
		else if ((dialDigitsLen > 1) && (dialDigits[dialDigitsLen - 2 ] == 'i'))
		{
			CallOption = CICC_CALLOPTIONS_CLIR_ALLOW;
			dialDigits[dialDigitsLen - 2 ] = 0;
		}
	}

	ret = CC_Dial(atHandle, dialDigits, callType, CallOption);

	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}

/************************************************************************************
 * F@: ciHangupVT - GLOBAL API for ECHUPVT-command
 * Notes:  cause
 */

RETURNCODE_T ciHangupVT(            const utlAtParameterOp_T op,
				    const char                      *command_name_p,
				    const utlAtParameterValue_P2c parameter_values_p,
				    const size_t num_parameters,
				    const char                      *info_text_p,
				    unsigned int                    *xid_p,
				    void                            *arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	UINT32 cause;

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);
	*xid_p = atHandle;

	switch ( op )
	{
	case TEL_EXT_SET_CMD:
	if ( getExtValue(parameter_values_p, 0, (int *)&cause,TEL_AT_ECHUPVT_CAUSE_VAL_MIN,TEL_AT_ECHUPVT_CAUSE_VAL_MAX,TEL_AT_ECHUPVT_CAUSE_VAL_DEFAULT ) == TRUE )
	{
		ret = CC_HangVT(atHandle, cause);
	}
	break;

	default:
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);


}



/************************************************************************************
 * F@: ciReleaseCall - GLOBAL API for GCF ATH-command
 *
 */
RETURNCODE_T ciReleaseCall(            const utlAtParameterOp_T op,
				       const char                      *command_name_p,
				       const utlAtParameterValue_P2c parameter_values_p,
				       const size_t num_parameters,
				       const char                      *info_text_p,
				       unsigned int                    *xid_p,
				       void                            *arg_p)
{
	UNUSEDPARAM(op)
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(parameter_values_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);
	*xid_p = atHandle;

	if (GET_ATP_INDEX(atHandle) != TEL_MODEM_AT_CMD_ATP || g_iCVHUMode != TEL_AT_CVHU_0_MODE_1)
		ret = CC_Hangup(atHandle);
	else
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);

	rc = HANDLE_RETURN_VALUE(ret);

	return(rc);
}

/************************************************************************************
 * F@: ciCHUP - GLOBAL API for GCF AT+CHUP-command
 *
 */
RETURNCODE_T ciCHUP(            const utlAtParameterOp_T op,
				       const char                      *command_name_p,
				       const utlAtParameterValue_P2c parameter_values_p,
				       const size_t num_parameters,
				       const char                      *info_text_p,
				       unsigned int                    *xid_p,
				       void                            *arg_p)
{
	UNUSEDPARAM(op)
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(parameter_values_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);
	*xid_p = atHandle;

	ret = CC_Hangup(atHandle);

	rc = HANDLE_RETURN_VALUE(ret);

	return(rc);
}


/************************************************************************************
 * F@: ciSend - GLOBAL API for GCF ATA-command
 *
 */
RETURNCODE_T ciAnswerCall(            const utlAtParameterOp_T op,
				      const char                      *command_name_p,
				      const utlAtParameterValue_P2c parameter_values_p,
				      const size_t num_parameters,
				      const char                      *info_text_p,
				      unsigned int                    *xid_p,
				      void                            *arg_p)
{
	UNUSEDPARAM(op)
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(parameter_values_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	*xid_p = atHandle;

	ret = CC_Answer(atHandle);

	rc = HANDLE_RETURN_VALUE(ret);

	return(rc);
}

/************************************************************************************
 * F@: ciCVHU - GLOBAL API for GCF AT+CVHU command (Voice Hangup Control)
 *
 * NOTE: CCI does not have a primitive for this command. For the purposes of
 * GCF automation, AT+CVHU=2 will return "OK", other parameters will return an error
 */
RETURNCODE_T ciCVHU(            const utlAtParameterOp_T op,
				const char                      *command_name_p,
				const utlAtParameterValue_P2c parameter_values_p,
				const size_t num_parameters,
				const char                      *info_text_p,
				unsigned int                    *xid_p,
				void                            *arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 mode;

	UINT32 reqHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	reqHandle = MAKE_AT_HANDLE( sAtpIndex );
	*xid_p = reqHandle;
	DBGMSG("%s: reqHandle = %d.\n", __FUNCTION__, reqHandle);

	switch (op)
	{
	case TEL_EXT_SET_CMD:           /* AT+CVHU= */
		if ( getExtValue(parameter_values_p, 0, (int *)&mode, TEL_AT_CVHU_0_MODE_MIN, TEL_AT_CVHU_0_MODE_MAX, TEL_AT_CVHU_0_MODE_DEFAULT ) == TRUE )
		{
			g_iCVHUMode = mode;
			ret = ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL );
		}
		else
		{
			ret = ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL );
		}
		break;

	case TEL_EXT_GET_CMD:           /* AT+CVHU? */
		{
			char resp[20];
			sprintf(resp, "+CVHU: %d\r\n", g_iCVHUMode);
			ret = ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, resp );
			break;
		}

	default:
		ret = ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}


/************************************************************************************
 * F@: ciAccumReq - GLOBAL API for GCF AT+CACM command
 *
 */
RETURNCODE_T ciAccumReq(            const utlAtParameterOp_T op,
				    const char                      *command_name_p,
				    const utlAtParameterValue_P2c parameter_values_p,
				    const size_t num_parameters,
				    const char                      *info_text_p,
				    unsigned int                    *xid_p,
				    void                            *arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;

	CHAR acmChvPswd[TEL_AT_CACM_0_PASSWD_STR_MAX_LEN];
	INT16 pswdLength;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch (op)
	{
	case TEL_EXT_GET_CMD:           /* AT+CACM? */
	{
		ret = CC_GetAcm(atHandle);
		break;
	}

	case TEL_EXT_TEST_CMD:               /* AT+CACM=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CACM= CI_CC_PRIM_RESET_ACM_VALUE_REQ*/
	{
		if ( getExtString(parameter_values_p, 0, acmChvPswd, TEL_AT_CACM_0_PASSWD_STR_MAX_LEN - ATCI_NULL_TERMINATOR_LENGTH, &pswdLength, TEL_AT_CACM_0_PASSWD_STR_DEFAULT) == FALSE )
			break;

		if ( pswdLength == 0 )
			ret = CC_SetAcm(atHandle, NULL);
		else
			ret = CC_SetAcm(atHandle, acmChvPswd);

		break;
	}

	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}

	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}


/************************************************************************************
 * F@: ciAccumMaxReq - GLOBAL API for GCF AT+CAMM command
 *
 */
RETURNCODE_T ciAccumMaxReq(            const utlAtParameterOp_T op,
				       const char                      *command_name_p,
				       const utlAtParameterValue_P2c parameter_values_p,
				       const size_t num_parameters,
				       const char                      *info_text_p,
				       unsigned int                    *xid_p,
				       void                            *arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	INT16 paramLength;
	INT32 newAccMaxVal;
	CHAR newAccMaxValStr[TEL_AT_CAMM_0_ACMMAX_STR_MAX_LEN];
	CHAR acmmChvPswd[TEL_AT_CACM_0_PASSWD_STR_MAX_LEN];

	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	*xid_p = atHandle;

	switch ( op )
	{
	case TEL_EXT_GET_CMD:           /* AT+CAMM? */
	{
		ret = CC_GetAmm(atHandle);
		break;
	}

	case TEL_EXT_TEST_CMD:               /* AT+CAMM=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	/* TS 27.007 - Set command sets the Advice of Charge related accumulated call meter maximum value.  */
	case TEL_EXT_SET_CMD:              /* AT+CAMM= */
	{
		/* get newAcmMax parameter and convert to integer */
		if (getExtString( parameter_values_p, 0, newAccMaxValStr, (UINT16)(TEL_AT_CAMM_0_ACMMAX_STR_MAX_LEN - ATCI_NULL_TERMINATOR_LENGTH), &paramLength, TEL_AT_CAMM_0_ACMMAX_STR_DEFAULT) == FALSE)
			break;

		if ( paramLength > 0 )
		{
			if (HexToBin( (char *)newAccMaxValStr, &newAccMaxVal, paramLength) == TRUE)
			{
				if ( getExtString(parameter_values_p, 1, acmmChvPswd, (UINT16)(TEL_AT_CAMM_1_PASSWD_STR_MAX_LEN - ATCI_NULL_TERMINATOR_LENGTH), &paramLength, TEL_AT_CAMM_1_PASSWD_STR_DEFAULT) == FALSE )
					break;

				if ( paramLength == 0 )
					ret = CC_SetAmm(atHandle, newAccMaxVal, NULL);
				else
					ret = CC_SetAmm(atHandle, newAccMaxVal, acmmChvPswd);
			}
		}
		else                    /* if (paramLength > 0) */
		{
			/* nothing to do */
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}

		break;
	}

	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}

	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}


/************************************************************************************
 * F@: ciAccumMaxReq - GLOBAL API for GCF AT+CHLD - command
 *
 */
RETURNCODE_T ciHoldCall(            const utlAtParameterOp_T op,
				    const char                      *command_name_p,
				    const utlAtParameterValue_P2c parameter_values_p,
				    const size_t num_parameters,
				    const char                      *info_text_p,
				    unsigned int                    *xid_p,
				    void                            *arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	CiCcCallManOp callManOp;
	INT32 nValue;
	INT32 atCallId = 0;


	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	**  handle AT operation
	*/
	switch ( op)
	{
	case TEL_EXT_TEST_CMD:          /* AT+CHLD=? */
	{
		/* display  [+CHLD: (list of supported <n>s)] */
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CHLD: (0,1,1x,2,2x,3)");
		break;
	}

	case TEL_EXT_SET_CMD:             /* AT+CHLD=  */
	{
		/*
		**  Determine the extended parameter.
		*/
		if ( getExtValue(parameter_values_p, 0, &nValue, TEL_AT_CHLD_0_N_VAL_MIN, TEL_AT_CHLD_0_N_VAL_MAX, TEL_AT_CHLD_0_N_VAL_DEFAULT) == FALSE )
			break;

		/*
		**  Manipulate the current call.
		*/
		switch ( nValue )
		{
		case ATCI_CHLD_MODE_0:
			callManOp = CI_CC_MANOP_RLS_HELD_OR_UDUB;
			break;
		case ATCI_CHLD_MODE_1:
			callManOp = CI_CC_MANOP_RLS_ACT_ACCEPT_OTHER;
			break;
		case ATCI_CHLD_MODE_2:
			callManOp = CI_CC_MANOP_HOLD_ACT_ACCEPT_OTHER;
			break;
		case ATCI_CHLD_MODE_3:
			callManOp = CI_CC_MANOP_ADD_HELD_TO_MPTY;
			break;
		case ATCI_CHLD_MODE_4:
			callManOp = CI_CC_MANOP_ECT;
			break;
		/* case ATCI_CHLD_MODE_1X: */
		/* case ATCI_CHLD_MODE_2X: */
		default:
		{
			if ( (nValue >= (ATCI_CHLD_MODE_1X + MIN_USER_CALL_ID)) &&
			     (nValue <= (ATCI_CHLD_MODE_1X + MAX_USER_CALL_ID)) )
			{
				callManOp = CI_CC_MANOP_RLS_CALL;
				atCallId = nValue - ATCI_CHLD_MODE_1X;
			}
			else if ( (nValue >= (ATCI_CHLD_MODE_2X + MIN_USER_CALL_ID)) &&
				  (nValue <= (ATCI_CHLD_MODE_2X + MAX_USER_CALL_ID)) )
			{
				callManOp = CI_CC_MANOP_HOLD_ALL_EXCEPT_ONE;
				atCallId = nValue - ATCI_CHLD_MODE_2X;
			}
			else if (nValue == 19)                                //release all calls--Johnny
			{
				ret = CC_HangupAllVoiceCall(atHandle);
				return (HANDLE_RETURN_VALUE(ret));

			}
			else
				return rc;

			break;
		}
		}                  /* switch */

		ret = CC_ManageCalls(atHandle, callManOp, atCallId);

		break;
	}

	case TEL_EXT_GET_CMD:                   /* AT+CHLD?  */
	case TEL_EXT_ACTION_CMD:                /* AT+CHLD   */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}

	rc = HANDLE_RETURN_VALUE(ret);

	return(rc);
}

/************************************************************************************
 * F@: ciSelectBST - GLOBAL API for GCF AT+CBST -command
 *
 */
RETURNCODE_T ciSelectBST(            const utlAtParameterOp_T op,
				     const char                      *command_name_p,
				     const utlAtParameterValue_P2c parameter_values_p,
				     const size_t num_parameters,
				     const char                      *info_text_p,
				     unsigned int                    *xid_p,
				     void                            *arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);
	INT32 bstSpeed;
	INT32 bstName;
	INT32 bstCe;

	/*
	 *  Put parser index into the variable
	 */
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch (op)
	{
	case TEL_EXT_GET_CMD:            /* AT+CBST?  */
	{
		ret = CC_GetBst(atHandle);
		break;
	}

	case TEL_EXT_TEST_CMD:            /* AT+CBST=? */
	{
		ret = CC_GetSupportBst(atHandle);
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CBST= */
	{
		/*
		**  Extract the parameters from the AT Command.
		*/
		if ( getExtValue( parameter_values_p, 0, &bstSpeed, TEL_AT_CBST_0_SPEED_VAL_MIN, TEL_AT_CBST_0_SPEED_VAL_MAX, TEL_AT_CBST_0_SPEED_VAL_DEFAULT ) == FALSE )
			break;
		if ( getExtValue( parameter_values_p, 1, &bstName, TEL_AT_CBST_1_NAME_VAL_MIN, TEL_AT_CBST_1_NAME_VAL_MAX, TEL_AT_CBST_1_NAME_VAL_DEFAULT ) == FALSE )
			break;
		if ( getExtValue( parameter_values_p, 2, &bstCe, TEL_AT_CBST_2_CE_VAL_MIN, TEL_AT_CBST_2_CE_VAL_MAX, TEL_AT_CBST_2_CE_VAL_DEFAULT ) == FALSE )
			break;

		ret = CC_SetBst(atHandle, bstSpeed, bstName, bstCe);
		break;
	}

	case TEL_EXT_ACTION_CMD:           /* AT+CBST */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}

	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}


/************************************************************************************
 * F@: ciFaxClass - GLOBAL API for GCF AT+FCLASS - command
 *
 */
RETURNCODE_T ciFaxClass(            const utlAtParameterOp_T op,
				    const char                      *command_name_p,
				    const utlAtParameterValue_P2c parameter_values_p,
				    const size_t num_parameters,
				    const char                      *info_text_p,
				    unsigned int                    *xid_p,
				    void                            *arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	INT32 setFaxClass;
	char faxBuf[50];

	CiRequestHandle atHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	atHandle = MAKE_AT_HANDLE( sAtpIndex );
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);


	switch ( op)
	{
	case TEL_EXT_TEST_CMD:          /* AT+FCLASS=? */
	{
		/* only data & fax class 1 is supported */
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+FCLASS: 0,1\r\n");
		break;
	}

	case TEL_EXT_SET_CMD:             /* AT+FCLASS=  */
	{
		/*
		**  Extract the arguments starting with the functionality.
		*/
		if ( ( getExtValue(parameter_values_p, 0, &setFaxClass, TEL_AT_FCLASS_0_N_VAL_MIN, TEL_AT_FCLASS_0_N_VAL_MAX, TEL_AT_FCLASS_0_N_VAL_DEFAULT) == TRUE ) )
		{
			switch (setFaxClass)
			{
			case TEL_AT_FCLASS_FAX:
				gCurrentDataMode = CICC_BASIC_CMODE_FAX;
				break;
			case TEL_AT_FCLASS_DATA:
			default:
				gCurrentDataMode = CICC_BASIC_CMODE_DATA;
				break;
			}
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
		else
		{
			/*
			**  There was a problem extracting the functionality.
			*/
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		}

		break;
	}

	case TEL_EXT_GET_CMD:            /* AT+FCLASS?  */
	{
		sprintf( faxBuf, "+FCLASS: %d", (gCurrentDataMode == CICC_BASIC_CMODE_DATA) ? 0 : 1 );
		ret = ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, faxBuf);
		break;
	}
	case TEL_EXT_ACTION_CMD:           /* AT+FCLASS   */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 * F@: ciCallModeReq - GLOBAL API for GCF AT+CMOD -command
 *
 */
RETURNCODE_T  ciCallModeReq(            const utlAtParameterOp_T op,
					const char                      *command_name_p,
					const utlAtParameterValue_P2c parameter_values_p,
					const size_t num_parameters,
					const char                      *info_text_p,
					unsigned int                    *xid_p,
					void                            *arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	INT32 callMode;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);


	switch (op)
	{
	case TEL_EXT_SET_CMD:            /* AT+CMOD= */
	{
		if ( getExtValue(parameter_values_p, 0, &callMode, TEL_AT_CMOD_0_MODE_VAL_MIN, TEL_AT_CMOD_0_MODE_VAL_MAX, TEL_AT_CMOD_0_MODE_VAL_DEFAULT ) == FALSE)
			break;

		ret = CC_SetMod(atHandle, callMode);
		break;
	}

	case TEL_EXT_GET_CMD:            /* AT+CMOD?  */
	{
		ret = CC_GetMod(atHandle);
		break;
	}

	case TEL_EXT_TEST_CMD:            /* AT+CMOD=? */
	{
		ret = CC_GetSupportMod(atHandle);
		break;
	}

	case TEL_EXT_ACTION_CMD:            /* AT+CMOD */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}

	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}


/************************************************************************************
 * F@: ciCallList - GLOBAL API for GCF AT+CLCC  -command
 *
 */
RETURNCODE_T  ciCallList(            const utlAtParameterOp_T op,
				     const char                      *command_name_p,
				     const utlAtParameterValue_P2c parameter_values_p,
				     const size_t num_parameters,
				     const char                      *info_text_p,
				     unsigned int                    *xid_p,
				     void                            *arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(parameter_values_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch (op)
	{
	case TEL_EXT_ACTION_CMD:           /* AT+CLCC */
	{
		ret = CC_GetCallList(atHandle);
		break;
	}

	case TEL_EXT_TEST_CMD:            /* AT+CLCC=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	case TEL_EXT_SET_CMD:                   /* AT+CLCC= */
	case TEL_EXT_GET_CMD:                   /* AT+CLCC?  */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}

	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}


/************************************************************************************
 * F@: ciAdviceOFCharge - GLOBAL API for GCF AT+CAOC command
 *
 */
RETURNCODE_T  ciAdviceOfCharge(            const utlAtParameterOp_T op,
					   const char                      *command_name_p,
					   const utlAtParameterValue_P2c parameter_values_p,
					   const size_t num_parameters,
					   const char                      *info_text_p,
					   unsigned int                    *xid_p,
					   void                            *arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc           = INITIAL_RETURN_CODE;
	CiReturnCode ret          = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);
	BOOL cmdValid     = FALSE;
	AtciAocMode AOCmode;

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT+CAOC=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CAOC: (0-2)" );
		cmdValid = TRUE;
		break;
	}

	case TEL_EXT_GET_CMD:         /* AT+CAOC? */
	{
		ret = SS_GetCcmOption(atHandle);
		cmdValid = TRUE;
		break;
	}

	case TEL_EXT_ACTION_CMD:         /* AT+CAOC */
	{
		cmdValid = TRUE;
		AOCmode = ATCI_AOC_MODE_QUERY;
		/* Fall through */
	}

	case TEL_EXT_SET_CMD:         /* AT+CAOC= */
	{
		/* If it was an extended action command, skip some steps */
		if ( cmdValid == FALSE )
		{
			UINT32 mode;
			if ( getExtValue( parameter_values_p, 0, (int *)&mode, TEL_AT_CAOC_0_MODE_VAL_MIN, TEL_AT_CAOC_0_MODE_VAL_MAX, TEL_AT_CAOC_0_MODE_VAL_DEFAULT ) == TRUE )
			{
				AOCmode = mode;
				cmdValid = TRUE;
			}
		}

		if ( cmdValid == TRUE )
		{
			switch ( AOCmode )
			{
			case ATCI_AOC_MODE_QUERY:
			{
				ret = CC_GetCcm(atHandle);
				break;
			}

			case ATCI_AOC_MODE_DEACTIVATE:
			{
				ret = SS_SetCcmOption( atHandle, (UINT8)0 );
				break;
			}

			case ATCI_AOC_MODE_ACTIVATE:
			{
				ret = SS_SetCcmOption( atHandle, (UINT8)1 );
				break;
			}

			default:
				cmdValid = FALSE;
				break;
			}
		}

		break;
	}

	default:
		break;
	}

	if ( cmdValid == FALSE )
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
	}

	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 * F@: ciSelectRLProtocol - GLOBAL API for GCF AT+CRLP command
 *
 */
RETURNCODE_T  ciSelRLP(            const utlAtParameterOp_T op,
				   const char                      *command_name_p,
				   const utlAtParameterValue_P2c parameter_values_p,
				   const size_t num_parameters,
				   const char                      *info_text_p,
				   unsigned int                    *xid_p,
				   void                            *arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	UINT32 iws;
	UINT32 mws;
	UINT32 T1;
	UINT32 N2;
	UINT32 ver;
	UINT32 T4;

	CiCcRlpCfg RLPCfg;

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT+CRLP=? */
	{
		ret = CC_GetSupportRlp(atHandle);
		break;
	}

	case TEL_EXT_GET_CMD:         /* AT+CRLP? */
	{
		ret = CC_GetRlp(atHandle);
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+CRLP= */
	{
		if ( getExtValue( parameter_values_p, 0, (int *)&iws, TEL_AT_CRLP_0_IWS_VAL_MIN, TEL_AT_CRLP_0_IWS_VAL_MAX, TEL_AT_CRLP_0_IWS_VAL_DEFAULT ) == FALSE )
			break;
		if ( getExtValue( parameter_values_p, 1, (int *)&mws, TEL_AT_CRLP_1_MWS_VAL_MIN, TEL_AT_CRLP_1_MWS_VAL_MAX, TEL_AT_CRLP_1_MWS_VAL_DEFAULT ) == FALSE )
			break;
		if ( getExtValue( parameter_values_p, 2, (int *)&T1, TEL_AT_CRLP_2_T1_VAL_MIN, TEL_AT_CRLP_2_T1_VAL_MAX, TEL_AT_CRLP_2_T1_VAL_DEFAULT ) == FALSE )
			break;
		if ( getExtValue( parameter_values_p, 3, (int *)&N2, TEL_AT_CRLP_3_N2_VAL_MIN, TEL_AT_CRLP_3_N2_VAL_MAX, TEL_AT_CRLP_3_N2_VAL_DEFAULT ) == FALSE )
			break;
		if ( getExtValue( parameter_values_p, 4, (int *)&ver, TEL_AT_CRLP_4_VER_VAL_MIN, TEL_AT_CRLP_4_VER_VAL_MAX, TEL_AT_CRLP_4_VER_VAL_DEFAULT ) == FALSE )
			break;
		if ( getExtValue( parameter_values_p, 5, (int *)&T4, TEL_AT_CRLP_5_T4_VAL_MIN, TEL_AT_CRLP_5_T4_VAL_MAX, TEL_AT_CRLP_5_T4_VAL_DEFAULT ) == FALSE )
			break;

		RLPCfg.winIWS = iws;
		RLPCfg.winMWS = mws;
		RLPCfg.ackTimer = T1;
		RLPCfg.reTxAttempts = N2;
		RLPCfg.ver = ver;
		RLPCfg.reSeqPeriod = T4;
		RLPCfg.initialT1 = 0;
		RLPCfg.initialN2 = 0;
		ret = CC_SetRlp(atHandle, &RLPCfg);
		break;
	}

	default:         /* AT+CRLP */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}

	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

RETURNCODE_T  ciSendDTMF(            const utlAtParameterOp_T op,
				     const char                      *command_name_p,
				     const utlAtParameterValue_P2c parameter_values_p,
				     const size_t num_parameters,
				     const char                      *info_text_p,
				     unsigned int                    *xid_p,
				     void                            *arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc           = INITIAL_RETURN_CODE;
	CiReturnCode ret          = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	UINT32 duration;
	CHAR DTMFStr[TEL_AT_VTS_0_DTMF_STR_MAX_LEN + 1];
	INT16 DTMFStrLength;

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch ( op )
	{
	case TEL_EXT_SET_CMD:         /* AT+VTS= */
	{

		if ( getExtString(parameter_values_p, 0, DTMFStr, TEL_AT_VTS_0_DTMF_STR_MAX_LEN, &DTMFStrLength, TEL_AT_VTS_0_DTMF_STR_DEFAULT) == FALSE )
			break;
		DBGMSG("%s: DTMFStr: %s\n", __FUNCTION__, DTMFStr);
		if ( getExtValue(parameter_values_p, 1, (int *)&duration, TEL_AT_VTS_1_DURATION_VAL_MIN, TEL_AT_VTS_1_DURATION_VAL_MAX, TEL_AT_VTS_1_DURATION_VAL_DEFAULT ) == FALSE )
			break;
		DBGMSG("%s: duration: %d\n", __FUNCTION__, duration);

		ret = CC_SendDTMF(atHandle, DTMFStr, duration);

		break;
	}
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}

	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}

/************************************************************************************
 * F@: ciMute - GLOBAL API for  AT+CMUT command used by Android RIL
 *
 */
RETURNCODE_T  ciMute(            const utlAtParameterOp_T op,
				 const char                      *command_name_p,
				 const utlAtParameterValue_P2c parameter_values_p,
				 const size_t num_parameters,
				 const char                      *info_text_p,
				 unsigned int                    *xid_p,
				 void                            *arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);
	INT32 mute;


	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT+CMUT=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CMUT: 0,1" );
		break;
	}

	case TEL_EXT_GET_CMD:         /* AT+CMUT? */
	{
		char mutBuf[50];
		mute = tel_get_voicecall_mute();

		sprintf( mutBuf, "+CMUT: %d\r\n", mute);
		ret = ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, mutBuf);
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+CMUT= */
	{
		if ( getExtValue( parameter_values_p, 0, (int *)&mute, TEL_AT_CMUT_0_MODE_VAL_MIN, TEL_AT_CMUT_0_MODE_VAL_MAX, TEL_AT_CMUT_0_MODE_VAL_DEFAULT) == FALSE )
			break;
		DBGMSG("%s: mute:%d\n", __FUNCTION__, mute);
		ret = tel_set_voicecall_mute(mute);
		if (ret == 0)
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	default:         /* AT+CMUT */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}

	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 * F@: ciErrorReport - GLOBAL API for AT+CEER  -command
 *
 */
RETURNCODE_T  ciErrorReport(            const utlAtParameterOp_T op,
					const char                      *command_name_p,
					const utlAtParameterValue_P2c parameter_values_p,
					const size_t num_parameters,
					const char                      *info_text_p,
					unsigned int                    *xid_p,
					void                            *arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(parameter_values_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch (op)
	{
	case TEL_EXT_ACTION_CMD:           /* AT+CEER */
	{
		char tmpBuf[300];
		sprintf(tmpBuf, "+CEER: \"%s\"\r\n", gErrCauseBuf);
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, tmpBuf);
		break;
	}

	case TEL_EXT_TEST_CMD:            /* AT+CEER=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	case TEL_EXT_SET_CMD:                   /* AT+CEER= */
	case TEL_EXT_GET_CMD:                   /* AT+CEER?  */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}

	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/* Added by Michal Bukai */
/************************************************************************************
 * F@: cciCPUC - GLOBAL API for GCF AT+CPUC command
 *
 */
RETURNCODE_T ciCPUC(            const utlAtParameterOp_T op,
				const char                      *command_name_p,
				const utlAtParameterValue_P2c parameter_values_p,
				const size_t num_parameters,
				const char                      *info_text_p,
				unsigned int                    *xid_p,
				void                            *arg_p)
{
	UNUSEDPARAM(command_name_p);
	UNUSEDPARAM(num_parameters);
	UNUSEDPARAM(info_text_p);

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	INT16 param_0_Length, param_1_Length, param_2_Length, index;
	CHAR cpucCurrency[TEL_AT_CPUC_0_CURRENCY_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH] = {0};
	CHAR text[TEL_AT_CPUC_0_CURRENCY_STR_MAX_LEN * 4 + ATCI_NULL_TERMINATOR_LENGTH] = {0};
	CHAR cpucPpu[TEL_AT_CPUC_1_PPU_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];
	CHAR cpucChvPswd[TEL_AT_CPUC_2_PASSWD_STR_MAX_LEN];
	BOOL isNoPasswd = false;
	int txtLen = 0;

	/*
	 *  Put parser index into the variable
	 */
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch ( op )
	{
	case TEL_EXT_GET_CMD:           /* AT+CPUC? */
	{
		ret = CC_GetCPUC(atHandle);
		break;
	}

	case TEL_EXT_TEST_CMD:               /* AT+CPUC=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CPUC= */
	{
		if(chset_type == ATCI_CHSET_UCS2)
			txtLen = TEL_AT_CPUC_0_CURRENCY_STR_MAX_LEN * 4;
		else if(chset_type == ATCI_CHSET_HEX)
			txtLen = TEL_AT_CPUC_0_CURRENCY_STR_MAX_LEN * 2;
		else
			txtLen = TEL_AT_CPUC_0_CURRENCY_STR_MAX_LEN;

		if (getExtString( parameter_values_p, 0, text, (UINT16)txtLen, &param_0_Length, TEL_AT_CPUC_0_CURRENCY_STR_DEFAULT) == TRUE)
		{
			if ( getExtString(parameter_values_p, 1, cpucPpu, (UINT16)TEL_AT_CPUC_1_PPU_STR_MAX_LEN, &param_1_Length, TEL_AT_CPUC_1_PPU_STR_DEFAULT) == TRUE )
			{
				if ( getExtString(parameter_values_p, 2, cpucChvPswd, (UINT16)(TEL_AT_CPUC_2_PASSWD_STR_MAX_LEN - ATCI_NULL_TERMINATOR_LENGTH), &param_2_Length, TEL_AT_CPUC_2_PASSWD_STR_DEFAULT) == TRUE )
				{
					/* init ci set cpuc struct */
					CHAR eppu[ TEL_AT_CPUC_1_PPU_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH ];
					BOOL bExp = FALSE;
					BOOL bFormat = TRUE;
					enum string_binary_type bin_type;
					if(chset_type == ATCI_CHSET_HEX)
						bin_type = STR_BIN_TYPE_HEX;
					else
						bin_type = STR_BIN_TYPE_GSM;

					if((param_0_Length = libConvertCSCSStringToBin((char *)text, param_0_Length, chset_type, (char *)cpucCurrency, sizeof(cpucCurrency) / sizeof(cpucCurrency[0]) - 1, bin_type)) < 0)
					{
						ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL );
						break;
					}

					cpucCurrency[param_0_Length] = '\0';
					for ( index = 0; index < param_0_Length; index++)
					{
						gSetPuctInfoReq.info.curr[index] = cpucCurrency[index];
					}

					for (index = 0; index < param_1_Length; index++)
					{
						if ( cpucPpu [index] == 'E' || cpucPpu [index] == 'e')
						{
							bExp = TRUE;
							memcpy(eppu, cpucPpu, index);
							if ((index + 1 < param_1_Length) && (cpucPpu [index + 1] == '-'))
							{
								gSetPuctInfoReq.info.negExp = TRUE;
								if(index + 2 < param_1_Length)
									gSetPuctInfoReq.info.exp = atoi((char *)&cpucPpu [index + 2]);
								else
									bFormat = FALSE;
							}
							else if(index + 1 < param_1_Length)
							{
								gSetPuctInfoReq.info.negExp = FALSE;
								gSetPuctInfoReq.info.exp = atoi((char *)&cpucPpu [index + 1]);
							}
							else
								bFormat = FALSE;
							index = param_1_Length;                 /* exit for loop*/
						}
					}
					if (!bFormat)
					{
						ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL );
						break;
					}
					if (bExp)
					{
						gSetPuctInfoReq.info.eppu = atoi((char *)eppu);
					}
					else
					{
						gSetPuctInfoReq.info.eppu = atoi((char *)cpucPpu);
					}

					isNoPasswd = (param_2_Length == 0 || parameter_values_p[2].is_default == TRUE);
					ret = CC_SetCPUC(atHandle, isNoPasswd, param_2_Length, cpucChvPswd);

				}
				else
				{
					ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL );
				}
			}
			else
			{
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL );
			}
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL );
		}
		break;
	}

	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}
/* Added by Michal Bukai */
/************************************************************************************
 * F@: ciCSTA - GLOBAL API for GCF AT+CSTA command
 *
 */
RETURNCODE_T ciCSTA(            const utlAtParameterOp_T op,
				const char                      *command_name_p,
				const utlAtParameterValue_P2c parameter_values_p,
				const size_t num_parameters,
				const char                      *info_text_p,
				unsigned int                    *xid_p,
				void                            *arg_p)
{
	UNUSEDPARAM(command_name_p);
	UNUSEDPARAM(num_parameters);
	UNUSEDPARAM(info_text_p);

	CiAddressType addressType;


	/*
	**  Set the result code to INITIAL_RETURN_CODE.  This allows
	**  the indications to display the correct return code after the
	**  AT Command is issued.
	*/
	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	INT32 iAddType = ATCI_DIAL_NUMBER_UNKNOWN;                                                /* Default - Unknown */

	/*
	 *  Put parser index into the variable
	 */
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);


	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);


	switch ( op )
	{
	case TEL_EXT_GET_CMD:           /* AT+CSTA? */
	{
		ret = CC_GetCSTA(atHandle);
		break;
	}

	case TEL_EXT_TEST_CMD:               /* AT+CSTA=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CSTA: (129,145,161,177)\r\n");
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CSTA= */
	{
		if ( (getExtValue(parameter_values_p, 0, (int *)&iAddType, TEL_AT_CSTA_ADDRESS_TYPE_VAL_MIN, TEL_AT_CSTA_ADDRESS_TYPE_VAL_MAX, TEL_AT_CSTA_ADDRESS_TYPE_VAL_DEFAULT ) == TRUE)
			 && ((iAddType == ATCI_DIAL_NUMBER_UNKNOWN) || (iAddType == ATCI_DIAL_NUMBER_INTERNATIONAL) || (iAddType == ATCI_DIAL_NUMBER_NATIONAL) || (iAddType == ATCI_DIAL_NUMBER_NET_SPECIFIC)))
		{
			addressType.NumPlan = CI_NUMPLAN_E164_E163;
			addressType.NumType = DialNumTypeGSM2CI(iAddType);
			ret = CC_SetCSTA(atHandle, &addressType);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL );
		}
		break;
	}

	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}


	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

//Added by Michal Buaki
/************************************************************************************
 * F@: ciCRC - GLOBAL API for GCF AT+CRC - command
 *
 */
RETURNCODE_T ciCRC(            const utlAtParameterOp_T op,
			       const char                      *command_name_p,
			       const utlAtParameterValue_P2c parameter_values_p,
			       const size_t num_parameters,
			       const char                      *info_text_p,
			       unsigned int                    *xid_p,
			       void                            *arg_p)
{
	UNUSEDPARAM(command_name_p);
	UNUSEDPARAM(num_parameters);
	UNUSEDPARAM(info_text_p);
	/*
	**  Set the result code to INITIAL_RETURN_CODE.  This allows
	**  the indications to display the correct return code after the
	**  AT Command is issued.
	*/
	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	INT32 iCRCmode;
	CHAR AtRspBuf [50];

	/*
	 *  Put parser index into the variable
	 */
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);


	switch ( op)
	{
	case TEL_EXT_TEST_CMD:          /* AT+CRC=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CRC: (0,1)\r\n");
		break;
	}

	case TEL_EXT_SET_CMD:             /* AT+CRC=  */
	{
		if ( ( getExtValue(parameter_values_p, 0, &iCRCmode, TEL_AT_CRC_MODE_VAL_MIN, TEL_AT_CRC_MODE_VAL_MAX, TEL_AT_CRC_MODE_VAL_DEFAULT) == TRUE ) )
		{
			g_iCRCMode = iCRCmode;
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL );
		}
		break;
	}

	case TEL_EXT_GET_CMD:            /* AT+CRC?  */
	{
		/* display "+CRC: current mode" */
		sprintf((char *)AtRspBuf, "+CRC: %d\r\n", g_iCRCMode);
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf);
		break;
	}
	case TEL_EXT_ACTION_CMD:           /* AT+CRC   */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}

	rc = HANDLE_RETURN_VALUE(ret);

	return(rc);

}

/************************************************************************************
 * F@: ciCRC - GLOBAL API for GCF AT+CMEE - command
 *
 */
RETURNCODE_T ciCMEE(            const utlAtParameterOp_T        op,
                               const char                      *command_name_p,
                               const utlAtParameterValue_P2c   parameter_values_p,
                               const size_t                    num_parameters,
                               const char                      *info_text_p,
                               unsigned int                    *xid_p,
                               void                            *arg_p)
{
       UNUSEDPARAM(command_name_p);
    UNUSEDPARAM(num_parameters);
    UNUSEDPARAM(info_text_p);
    /*
       **  Set the result code to INITIAL_RETURN_CODE.  This allows
       **  the indications to display the correct return code after the
       **  AT Command is issued.
       */
       RETURNCODE_T                        rc = INITIAL_RETURN_CODE;
       CiReturnCode                        ret = CIRC_SUCCESS;
       int                                                                iErrorCodePresentation = 0;
       CHAR                                                       AtRspBuf [500];

       /*
       *  Put parser index into the variable
       */
       UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);
       *xid_p = atHandle;
       DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

       switch ( op)
       {
               case TEL_EXT_TEST_CMD:  /* AT+CMEE=? */
               {
                       ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CMEE: (0-2)\r\n");
                       break;
               }
               case TEL_EXT_SET_CMD:     /* AT+CMEE=  or AT+CMEE (default 0)*/
               {
                       if ( ( getExtValue(parameter_values_p, 0, &iErrorCodePresentation, TEL_AT_CMEE_MODE_VAL_MIN, TEL_AT_CMEE_MODE_VAL_MAX, TEL_AT_CMEE_MODE_VAL_DEFAULT) == TRUE ) )
                       {
                               switch(iErrorCodePresentation)
                               {
                                       case 0:
                                               g_uintErrorCodePresentationMode = 0;
						break;
                                       case 1:
                                               g_uintErrorCodePresentationMode = 1;
                                               break;
                                       case 2:
                                               g_uintErrorCodePresentationMode = 2;
                                               break;
                                       default:
                                               g_uintErrorCodePresentationMode = 1;
                                               break;
                               }

                               ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL );
                       }
                       else
                       {
                               ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL );
                       }
                       break;
                       }

                       case TEL_EXT_GET_CMD:    /* AT+CMEE?  */
                       {
                               /* display "+CMEE: current mode" */
                               sprintf((char *)AtRspBuf, "+CMEE: %d\r\n", g_uintErrorCodePresentationMode);
                               ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf);
                               break;
                       }

                       default:
                       {
                               ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
                               break;
                       }
               }

               rc = HANDLE_RETURN_VALUE(ret);
               return(rc);
}

/************************************************************************************
 * F@: ciCRC - GLOBAL API for AT+VTD - command
 *
 */
RETURNCODE_T ciVTD(            const utlAtParameterOp_T        op,
					const char                      *command_name_p,
					const utlAtParameterValue_P2c   parameter_values_p,
					const size_t                    num_parameters,
					const char                      *info_text_p,
					unsigned int                    *xid_p,
					void                            *arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc           = INITIAL_RETURN_CODE;
	CiReturnCode ret          = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	UINT32 duration;

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch ( op )
	{
		case TEL_EXT_TEST_CMD:  /* AT+VTD=? */
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
			break;
		}
		case TEL_EXT_GET_CMD:    /* AT+VTD?  */
		{
			ret = CC_GetDTMFDuration(atHandle);
			break;
		}
		case TEL_EXT_SET_CMD:         /* AT+VTD= */
		{
			if ( getExtValue(parameter_values_p, 0, (int *)&duration, TEL_AT_VTS_1_DURATION_VAL_MIN, TEL_AT_VTS_1_DURATION_VAL_MAX, TEL_AT_VTS_1_DURATION_VAL_DEFAULT ) == FALSE )
			break;
			ret = CC_SetDTMFDuration(atHandle,duration);
		break;
		}
		default:
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
			break;
		}
	}

	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}

#if 0
//Added by Michal Buaki
/************************************************************************************
 * F@: ciCRC - GLOBAL API for GCF AT+CR - command
 *
 */
RETURNCODE_T ciCR(            const utlAtParameterOp_T op,
			      const char                      *command_name_p,
			      const utlAtParameterValue_P2c parameter_values_p,
			      const size_t num_parameters,
			      const char                      *info_text_p,
			      unsigned int                    *xid_p,
			      void                            *arg_p)
{
	UNUSEDPARAM(command_name_p);
	UNUSEDPARAM(num_parameters);
	UNUSEDPARAM(info_text_p);
	/*
	**  Set the result code to INITIAL_RETURN_CODE.  This allows
	**  the indications to display the correct return code after the
	**  AT Command is issued.
	*/
	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_SUCCESS;
	INT32 iCRmode;
	CHAR AtRspBuf [500];
	static CiCcPrimEnableDataServiceNegIndReq enableDataServiceNegIndReq;

	/*
	 *  Put parser index into the variable
	 */
	CiRequestHandle reqHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	reqHandle = RET_CI_REQ_HANDLE( sAtpIndex, CI_SG_ID_CC );
	*xid_p = reqHandle;
	DBGMSG("%s: reqHandle = %d.\n", __FUNCTION__, reqHandle);

	RESP_DEBUG((CHAR *)"Enter ciCR\r\n", TRUE, FALSE);


	switch ( op)
	{
	case TEL_EXT_TEST_CMD:          /* AT+CR=? */
	{
		RESP( reqHandle, (char *)"+CR: (0,1)\r\nOK\r\n", TRUE, FALSE);
		break;
	}

	case TEL_EXT_SET_CMD:             /* AT+CR=  */
	{
		if ( ( getExtValue(parameter_values_p, 0, &iCRmode, TEL_AT_CR_MODE_VAL_MIN, TEL_AT_CR_MODE_VAL_MAX, TEL_AT_CR_MODE_VAL_DEFAULT) == TRUE ) )
		{
			g_iCRMode = iCRmode;

			if (g_iCRMode == 1)
				enableDataServiceNegIndReq.enable = TRUE;
			else
				enableDataServiceNegIndReq.enable = FALSE;

			ret = ciRequest( gAtciSvgHandle[CI_SG_ID_CC], CI_CC_PRIM_ENABLE_DATA_SERVICENEG_IND_REQ,
					 reqHandle, (void *)&enableDataServiceNegIndReq );
		}
		else
		{
			ret = INVALID_PARAM;
		}
		break;
	}

	case TEL_EXT_GET_CMD:            /* AT+CR?  */
	{
		/* display "+CR: current mode" */
		sprintf((char *)AtRspBuf, "+CR: %d\r\nOK\r\n", g_iCRMode);
		RESP( reqHandle, (char *)AtRspBuf, TRUE, FALSE);
		break;
	}
	case TEL_EXT_ACTION_CMD:           /* AT+CR   */
	default:
	{
		ret = OPER_NOT_SUPPORTED;
		break;
	}
	}

	RESP_DEBUG((CHAR *)"Exit ciCR\r\n", TRUE, FALSE);

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret, reqHandle);
	return(rc);

}
#endif
/************************************************************************************
 * F@: ciCRC - GLOBAL API for AT$VTS - command
 *
 */
RETURNCODE_T ciSendLongDTMF(           const utlAtParameterOp_T        op,
			                const char                      *command_name_p,
			                const utlAtParameterValue_P2c   parameter_values_p,
			                const size_t                    num_parameters,
			                const char                      *info_text_p,
			                unsigned int                    *xid_p,
			                void                            *arg_p)

{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc           = INITIAL_RETURN_CODE;
	CiReturnCode ret          = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	int mode;
	INT16 DTMFStrLength;
	CHAR DTMFStr[2];
	BOOL cmdValid = FALSE;
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch ( op )
	{
	case TEL_EXT_SET_CMD:         /* AT$VTS= */
	{
		if (getExtString(parameter_values_p, 0, DTMFStr, 1, &DTMFStrLength, NULL) == TRUE)
		{
			if((DTMFStrLength != 0) && (strchr(DTMFCHAR, (int)DTMFStr[0]) != NULL))
			{
				if ( getExtValue(parameter_values_p, 1, &mode, 0, 1, 0) == TRUE)
				{
					cmdValid = TRUE;
				}
			}
		}
		if(cmdValid)
		{
			ret = CC_SendLongDTMF(atHandle, (UINT8)DTMFStr[0], (BOOL)mode);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL );
		}
		break;
		}
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}

	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}

/************************************************************************************
    * F@: ciLOOPTEST - GLOBAL API for AT+LOOPTEST command
    *
    */
RETURNCODE_T ciLOOPTEST(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);
	INT32 method;  //1 indicate enable, 0 indicate disable
	INT32 device;  //0 indicate earpiece, 1 indicate speaker, 2 indicate headset

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT+LOOPTEST=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+LOOPTEST: (0-1),(0-2)");
		break;
	}

	case TEL_EXT_GET_CMD:         /* AT+LOOPTEST? */
	{
		char Buf[50];
		ret = tel_get_loopback_state(&method, &device);

		sprintf( Buf, "+LOOPTEST: %d,%d\r\n", method, device);
		ret = ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, Buf);
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+LOOPTEST= */
	{
		if ( getExtValue( parameter_values_p, 0, (int *)&method, 0, 1, 0) == FALSE )
			break;
		if ( getExtValue( parameter_values_p, 1, (int *)&device, 0, 2, 0) == FALSE ) //input min/max/default
			break;

		DBGMSG("%s: method:%d, device:%d\n", __FUNCTION__, method, device);
		ret = tel_set_loopback_state(method, device);
		if (ret == 0)
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}
	default:         /* AT+LOOPTEST */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}

	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}

#ifdef AT_CUSTOMER_HTC
/************************************************************************************
 * F@: ciFirewallDB - GLOBAL API for AT+FWDB command
 *
 */
RETURNCODE_T  ciFirewallDB(            const utlAtParameterOp_T op,
					  const char                      *command_name_p,
					  const utlAtParameterValue_P2c parameter_values_p,
					  const size_t num_parameters,
					  const char                      *info_text_p,
					  unsigned int                    *xid_p,
					  void                            *arg_p)
{

	UNUSEDPARAM(command_name_p);
	UNUSEDPARAM(num_parameters);
	UNUSEDPARAM(info_text_p);

	/*
	**  Set the result code to INITIAL_RETURN_CODE.  This allows
	**  the indications to display the correct return code after the
	**  AT Command is issued.
	*/
	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;

	/*
	 *  Put parser index into the variable
	 */
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);


	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_SET_CMD:         /* AT+FWDB= */
	{
		int type, number;
		int record_indexes[TEL_AT_FWDB_1_NUMBER_VAL_MAX];
		CHAR records[TEL_AT_FWDB_1_NUMBER_VAL_MAX][TEL_AT_FWDB_PHONE_NUMBER_MAX_LENGTH + ATCI_NULL_TERMINATOR_LENGTH];
		int record_index = 0;
		int paramter_index = 0;
		INT16 tmpLen;
		BOOL recordValid;
		if ( getExtValue( parameter_values_p, paramter_index++, &type, TEL_AT_FWDB_0_TYPE_VAL_MIN, TEL_AT_FWDB_0_TYPE_VAL_MAX, TEL_AT_FWDB_0_TYPE_VAL_DEFAULT ) == TRUE )
		{
			if ( getExtValue( parameter_values_p, paramter_index++, &number, TEL_AT_FWDB_1_NUMBER_VAL_MIN, TEL_AT_FWDB_1_NUMBER_VAL_MAX, TEL_AT_FWDB_1_NUMBER_VAL_DEFAULT ) == TRUE )
			{
				for(record_index = 0; record_index < number; record_index++)
				{
					recordValid = FALSE;
					if ( (getExtValue( parameter_values_p, paramter_index++, &record_indexes[record_index], TEL_AT_FWDB_INDEX_VAL_MIN, TEL_AT_FWDB_INDEX_VAL_MAX, TEL_AT_FWDB_INDEX_VAL_DEFAULT ) == TRUE)
						&& (record_indexes[record_index] >= 0))
					{
						if((getExtString(parameter_values_p, paramter_index++, records[record_index], TEL_AT_FWDB_PHONE_NUMBER_MAX_LENGTH, &tmpLen, NULL) == TRUE) &&
							(strspn((char *)records[record_index], TEL_AT_FWDB_PHONE_NUMBER_VALID_CHARACTERS) == (UINT16)tmpLen))
						{
							recordValid = TRUE;
						}
					}
					if(!recordValid)
						break;
				}
			}
		}
		if(record_index == number)
		{
			ret = CC_EditBlackList(atHandle, (UINT8)type, (UINT16)number, record_indexes, (const CHAR *)records, utlNumberOf(records[0]));
		}
		else
		{
			ret = ATRESP(atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
		}
		break;
	}
	case TEL_EXT_TEST_CMD:       /*AT+FWDB=?*/
	case TEL_EXT_GET_CMD:        /* AT+FWDB? */
	case TEL_EXT_ACTION_CMD:      /* AT+FWDB*/
	default:
	{
		ret = ATRESP(atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_ALLOWED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);

	return(rc);
}
#endif
