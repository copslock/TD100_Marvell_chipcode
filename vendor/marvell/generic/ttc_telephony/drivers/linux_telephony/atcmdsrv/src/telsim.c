/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************
*  Filename: Telsim.c
*
*  Description: Process SIM related AT commands
*
*  History:
*   May 21, 2008 - Johnny He, Creation of file
*   Oct 16, 2008 - Qiang XU, Optimization

******************************************************************************/
/*#include "teldef.h"
 #include "telutl.h"
 #include "telmm.h"
 #include "telatci.h"
 #include "telatparamdef.h"
 #include "mm_api.h"
 */
#include "ci_api.h"
#include "telatci.h"
#include "tellinux.h"
#include "tel3gdef.h"
#include "telcontroller.h"
#include "teldef.h"
#include "telutl.h"
#include "telatparamdef.h"
#include "telmsg.h"
#include "ci_sim.h"
#include "sim_api.h"

#include <sys/ioctl.h>
#include <fcntl.h>
#include "utlMalloc.h"


extern CiServiceHandle gAtciSvgHandle[CI_SG_NUMIDS + 1];

CiSimStatus gSimStatus = CI_SIM_ST_NOT_READY;
CiSimPinState gSimPinState = CI_SIM_PIN_NUM_STATES;

extern AtSimCardType gSimCardType;
BOOL getSimTypeInd = FALSE;

extern void dbg_dump_string(char* name, char* buf, int len);
extern BOOL checkValidFacility( CHAR *facStr, INT16 facStrLen, AtciFacType *ciFacType );
extern CiReturnCode MEPReadCodes(UINT32 atHandle, AtciFacType ciFacType);
extern CiReturnCode PersonalizeME(UINT32 atHandle, AtciFacType ciFacType, INT32 mode, CHAR * facPwdStr, INT16 facPwdStrLen, BOOL is_ext);
/************************************************************************************
 *
 * SIM related AT commands
 *
 *************************************************************************************/

/************************************************************************************
 * F@: ciRestrictedAccessSIM - GLOBAL API to meet Borqs requirement AT+CRSM command
 *
 */
RETURNCODE_T  ciRestrictedAccessSIM(            const utlAtParameterOp_T op,
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
	UNUSEDPARAM(parameter_values_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT+CRSM=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CRSM: (176,178,192,214,220,242),(12037-28599),(0-255),(0-255),(0-255),<data>,<pathid>" );
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+CRSM= */
	#if 0
	{
	    	int cmd = 0, fileId = 0, p1 = 0, p2 = 0, p3 = 0;
		INT16 dataLen = 0;
		CHAR data[CI_SIM_MAX_APDU_DATA_SIZE * 2];
		if ( ( getExtValue( parameter_values_p, 0, &cmd, 0, 255, 0 ) == TRUE) )
		{
			getExtValue( parameter_values_p, 1, &fileId, 0, 0x7fff, 0);

			if (( getExtValue( parameter_values_p, 2, &p1, 0, 255, 0 ) != TRUE )
			    || ( getExtValue( parameter_values_p, 3, &p2, 0, 255, 0 ) != TRUE )
			    || ( getExtValue( parameter_values_p, 4, &p3, 0, 255, 0 ) != TRUE ))
			{
				p1 = 0;
				p2 = 0;
				p3 = 0;
			}
			if ( getExtString(parameter_values_p, 5, data, CI_SIM_MAX_APDU_DATA_SIZE * 2, &dataLen, NULL) != TRUE )
			{
				dataLen = 0;
			}

			ret = SIM_RestrictedAccess(atHandle, cmd, fileId, p1, p2, p3, (char *)data, (int)dataLen);

		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		}
		break;
	}
	#endif
	{
		//change by linda add path parameter
		int cmd = 0, fileId = 0, p1 = 0, p2 = 0, p3 = 0;
		INT16 dataLen = 0, pathLen = 0;
		CHAR data[CI_SIM_MAX_APDU_DATA_SIZE * 2 + ATCI_NULL_TERMINATOR_LENGTH], valStr[CI_MAX_CI_STRING_LENGTH * 2 + ATCI_NULL_TERMINATOR_LENGTH];
		if ( ( getExtValue( parameter_values_p, 0, &cmd, 0, 255, 0 ) == TRUE) )
		{
			if( (getExtValue( parameter_values_p, 1, &fileId, 0, 0x7fff, 0) == TRUE))
			{
				if (( getExtValue( parameter_values_p, 2, &p1, 0, 255, 0 ) != TRUE )
				|| ( getExtValue( parameter_values_p, 3, &p2, 0, 255, 0 ) != TRUE )
				|| ( getExtValue( parameter_values_p, 4, &p3, 0, 255, 0 ) != TRUE ))
				{
					p1 = 0;
					p2 = 0;
					p3 = 0;
				}
				if ( getExtString(parameter_values_p, 5, data, CI_SIM_MAX_APDU_DATA_SIZE * 2, &dataLen, NULL) != TRUE )
				{
					dataLen = 0;
				}
				if (getExtString(parameter_values_p, 6, valStr, CI_MAX_CI_STRING_LENGTH * 2, &pathLen, NULL) != TRUE)
				{
					valStr[0] = '\0';
					pathLen = 0;
				}
				ret = SIM_RestrictedAccess(atHandle, cmd, fileId, (char *)valStr, p1, p2, p3, (char *)data, (int)dataLen);
			}
			else
			{
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
			}

		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		}
		break;
	}
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 * F@: ciAccessSIM - GLOBAL API for GCF AT+CSIM command
 *
 */
RETURNCODE_T  ciGenericAccessSim(            const utlAtParameterOp_T op,
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
	BOOL cmdValid     = FALSE;

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT+CSIM=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		cmdValid = TRUE;
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+CSIM= */
	{
		int cmdLen, strLen;
		CHAR cmdStr[CI_SIM_MAX_APDU_DATA_SIZE * 2 + ATCI_NULL_TERMINATOR_LENGTH];
		if ( getExtValue( parameter_values_p, 0, &cmdLen, 0, TEL_AT_CMDSTR_MAX_LEN, 0 ) == TRUE )
		{
			if ( getExtString(parameter_values_p, 1, cmdStr, CI_SIM_MAX_APDU_DATA_SIZE * 2, (INT16 *)&strLen, NULL) == TRUE )
			{
				if (strLen >= cmdLen)
				{
					ret = SIM_GenericAccess(atHandle, cmdLen, (char *)cmdStr);
					cmdValid = TRUE;
				}
			}
		}

		break;
	}

	default:         /* AT+CSIM, AT+CSIM? */
	{
		break;
	}
	}

	if ( !cmdValid )
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}

/************************************************************************************
 * F@: ciGetImsi - GLOBAL API for GCF AT+CIMI command
 *
 */
RETURNCODE_T  ciGetImsi(            const utlAtParameterOp_T op,
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
	BOOL cmdValid     = FALSE;

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT+CIMI=? */
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		cmdValid = TRUE;
		break;
	}

	case TEL_EXT_ACTION_CMD:         /* AT+CIMI */
	{
		ret = SIM_GetImsi(atHandle);
		cmdValid = TRUE;
		break;
	}

	default:         /* AT+CIMI?, AT+CIMI= */
		break;
	}

	if ( cmdValid == FALSE )
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}

/************************************************************************************
 * F@: ciEnterPin - GLOBAL API for GCF AT+CPIN command
 *
 */
RETURNCODE_T  ciEnterPin(            const utlAtParameterOp_T op,
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

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	**  process operation.
	*/
	switch (op)
	{
	case TEL_EXT_TEST_CMD:               /* AT+CPIN=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	case TEL_EXT_GET_CMD:          /* AT+CPIN? */
	{
		ret = SIM_GetPinState(atHandle, CMD_TYPE_CPIN);
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+CPIN= */
	{
		CHAR oldPinStr[CI_MAX_PASSWORD_LENGTH + ATCI_NULL_TERMINATOR_LENGTH];
		CHAR newPinStr[CI_MAX_PASSWORD_LENGTH + ATCI_NULL_TERMINATOR_LENGTH];
		INT16 oldPinLen,  newPinLen;
		/* check SIM status & state */
		if (gSimPinState != CI_SIM_PIN_NUM_STATES )
		{
			/* retrieve the first password */
			if ( getExtString(parameter_values_p, 0, &oldPinStr[0], CI_MAX_PASSWORD_LENGTH, &oldPinLen, NULL) == TRUE )
			{
				/* Check entered string.... */
				if ( oldPinLen >= CI_MIN_PASSWORD_LENGTH )
				{
					/* Extract the new password. */
					if ( getExtString(parameter_values_p, 1, &newPinStr[0], CI_MAX_PASSWORD_LENGTH, &newPinLen, NULL) == TRUE )
					{
						/* Check minimum new PIN length as per GSM 11.11 - 9.3
						**  and it contains no alpha characters. NOTE: checkForNumericOnlyChars is a Voyager function
						*/
						if (newPinLen == 0)
						{                 //no second parameter
							switch (gSimPinState)
							{
							case CI_SIM_PIN_ST_CHV1_REQUIRED:
								ret = SIM_EnterPin(atHandle, CI_SIM_CHV_1, CI_SIM_CHV_VERIFY, oldPinStr, NULL);
								break;
							case CI_SIM_PIN_ST_CHV2_REQUIRED:
								ret = SIM_EnterPin(atHandle, CI_SIM_CHV_2, CI_SIM_CHV_VERIFY, oldPinStr, NULL);
								break;
							default:
								ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
							}
						}
						else if (( newPinLen >= CI_MIN_PASSWORD_LENGTH ) && ( checkForNumericOnlyChars( newPinStr ) ))
						{
							/* !!! IULIA - may need checkForAdditionalPassword functionality */
							/* i.e. if no PIN expected => send error; if double check => display PIN again! */

							switch (gSimPinState)
							{
							case CI_SIM_PIN_ST_UNBLOCK_CHV1_REQUIRED:
								ret = SIM_EnterPin(atHandle, CI_SIM_CHV_1, CI_SIM_CHV_UNBLOCK, oldPinStr, newPinStr);
								break;
							case CI_SIM_PIN_ST_UNBLOCK_CHV2_REQUIRED:
								ret = SIM_EnterPin(atHandle, CI_SIM_CHV_2, CI_SIM_CHV_UNBLOCK, oldPinStr, newPinStr);
								break;
							/* If PIN1/PIN2 is not required, it is operation to change PIN1 */
							default:
								ret = SIM_EnterPin(atHandle, CI_SIM_CHV_1, CI_SIM_CHV_CHANGE, oldPinStr, newPinStr);
								break;
							}
						}
						else
						{
							/* Invalid PIN length. */
							ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
						}
					}
					else
					{
						/* invalid pin param */
						ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
					}
				}
				else                      //oldPin> MIN_LENGTH
				{                       /* Invalid PIN length. */
					ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
				}
			}
			else                      //retrive first para error
			{
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
			}
		}
		else                    /* invalide pin state */
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		}

		break;
	}

	case TEL_EXT_ACTION_CMD:         /* AT+CPIN */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}


/************************************************************************************
 * F@: ciEnterPin2 - GLOBAL API for GCF AT+CPIN2 command
 * used for verify PIN2.
 *
 */
RETURNCODE_T  ciEnterPin2(            const utlAtParameterOp_T op,
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

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	**  process operation.
	*/
	switch (op)
	{
	case TEL_EXT_TEST_CMD:               /* AT+CPIN2=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	case TEL_EXT_GET_CMD:          /* AT+CPIN2? */
	{
		ret = SIM_GetPinState(atHandle, CMD_TYPE_CPIN);
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+CPIN2= */
	{
		CHAR PinStr[CI_MAX_PASSWORD_LENGTH + ATCI_NULL_TERMINATOR_LENGTH];
		INT16 PinLen;
		/* check SIM status & state */
		if (( gSimStatus != CI_SIM_ST_NOT_READY ) && ( gSimPinState != CI_SIM_PIN_NUM_STATES ))
		{
			/* retrieve the first password */
			if ( getExtString(parameter_values_p, 0, &PinStr[0], CI_MAX_PASSWORD_LENGTH, &PinLen, NULL) == TRUE )
			{
				/* Check entered string.... */
				if ( PinLen >= CI_MIN_PASSWORD_LENGTH )
				{
					ret = SIM_EnterPin(atHandle, CI_SIM_CHV_2, CI_SIM_CHV_VERIFY, PinStr, NULL);
				}
				else
				{                       /* Invalid PIN length. */
					ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
				}
			}
			else                      //retrive first para error
			{
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
			}
		}
		else                    /* invalide pin state */
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		}

		break;
	}

	case TEL_EXT_ACTION_CMD:         /* AT+CPIN2 */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}
/************************************************************************************
 * F@: ciGetPinRetryTimes - GLOBAL API for  AT*SIMDETEC command
 *
 */
RETURNCODE_T  ciDetectSIM(            const utlAtParameterOp_T op,
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
	UNUSEDPARAM(parameter_values_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	**  process operation.
	*/
	switch (op)
	{
	case TEL_EXT_TEST_CMD:               /* AT*SIMDETEC=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT*SIMDETEC= */
	{
		int simSlot = 0;
		if ( ( getExtValue( parameter_values_p, 0, &simSlot, 1, 2, 1 ) == TRUE) )
		{
			if(simSlot == 2) //not support Slave SIM at this time
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
			else
				ret = SIM_GetSIMInserted(atHandle, simSlot);
		}
		else                    /* invalide pin state */
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		}

		
		break;
	}
	
	case TEL_EXT_GET_CMD:          /* AT*SIMDETEC? */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}


/************************************************************************************
 * F@: ciGetPinRetryTimes - GLOBAL API for GCF AT+EPIN command
 *
 */
RETURNCODE_T  ciGetPinRetryTimes(            const utlAtParameterOp_T op,
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
	UNUSEDPARAM(parameter_values_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	**  process operation.
	*/
	switch (op)
	{
	case TEL_EXT_TEST_CMD:               /* AT+EPIN=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	case TEL_EXT_GET_CMD:          /* AT+EPIN? */
	{
		ret = SIM_GetPinState(atHandle, CMD_TYPE_EPIN);
		break;
	}

	case TEL_EXT_ACTION_CMD:         /* AT+CPIN */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 *
 * SIM Function called by external modules other than AT parser
 *
 *************************************************************************************/

/************************************************************************************
 * F@: isCHVOper - API to check whether it is SIM operation
 *
 */
BOOL isCHVOper(char* dialDigits)
{
	if (dialDigits[0] != '*')
		return FALSE;
	if (dialDigits[1] != '*')
		return FALSE;
	if (strncmp(&dialDigits[2], "04*", 3) == 0)
		return TRUE;
	if (strncmp(&dialDigits[2], "05*", 3) == 0)
		return TRUE;
	if (strncmp(&dialDigits[2], "042*", 4) == 0)
		return TRUE;
	if (strncmp(&dialDigits[2], "052*", 4) == 0)
		return TRUE;
	return FALSE;

}

/************************************************************************************
 * F@: ciUnblockPin - API to unblock pin
 *
 */
CiReturnCode ciUnblockPin(UINT32 atHandle, const char *command_name_p)
{
	UINT32 len = strlen(command_name_p);
	CiReturnCode ret = CIRC_FAIL;
	char ptr[100];
	char* tmpStr;
	char operStr[5];

	char passwordStr[TEL_AT_CPWD_1_OLDPW_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];
	char newPasswordStr[TEL_AT_CPWD_2_NEWPW_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];
	char confirmPasswordStr[TEL_AT_CPWD_3_NEWPWVER_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];


	strcpy(ptr, command_name_p);
	if (ptr[len - 1] != '#')
		return ret;

	/* parse command string begin */
	tmpStr = strtok(&ptr[2], "*");
	if (strlen(tmpStr) > 3)
		return ret;
	strcpy(operStr, tmpStr);

	tmpStr = strtok(NULL, "*");
	if (strlen(tmpStr) > CI_MAX_PASSWORD_LENGTH || strlen(tmpStr) < CI_MIN_PASSWORD_LENGTH)
		return ret;
	strcpy(passwordStr, tmpStr);

	tmpStr = strtok(NULL, "*");
	if (strlen(tmpStr) > CI_MAX_PASSWORD_LENGTH || strlen(tmpStr) < CI_MIN_PASSWORD_LENGTH)
		return ret;
	strcpy(newPasswordStr, tmpStr);

	tmpStr = strtok(NULL, "*");
	if (strlen(tmpStr) > CI_MAX_PASSWORD_LENGTH || strlen(tmpStr) < CI_MIN_PASSWORD_LENGTH)
		return ret;
	strcpy(confirmPasswordStr, tmpStr);
	confirmPasswordStr[strlen(confirmPasswordStr) - 1] = '\0'; //replace the last '#'

	if (strcmp(newPasswordStr, confirmPasswordStr))
		return ret;
	/* parse command string end */

	/*process command string*/
	if (strcmp(operStr, "04") == 0) // change PIN
	{
		//ret = ciChvOper(CI_SIM_CHV_1,CI_SIM_CHV_CHANGE,passwordStr, newPasswordStr,MAKE_CI_REQ_HANDLE(atHandle,CI_SIM_CHV_CHANGE));
		ret = SIM_EnterPin(atHandle, CI_SIM_CHV_1, CI_SIM_CHV_CHANGE, (CHAR *)passwordStr, (CHAR *)newPasswordStr);

	}
	else if (strcmp(operStr, "042") == 0) // change PIN2
	{
		//ret = ciChvOper(CI_SIM_CHV_2,CI_SIM_CHV_CHANGE,passwordStr, newPasswordStr,MAKE_CI_REQ_HANDLE(atHandle,CI_SIM_CHV_CHANGE));
		ret = SIM_EnterPin(atHandle, CI_SIM_CHV_2, CI_SIM_CHV_CHANGE, (CHAR *)passwordStr, (CHAR *)newPasswordStr);
	}

	else if (strcmp(operStr, "05") == 0) // unblock PIN
	{
		//ret = ciChvOper(CI_SIM_CHV_1,CI_SIM_CHV_UNBLOCK,passwordStr, newPasswordStr,MAKE_CI_REQ_HANDLE(atHandle,CI_SIM_CHV_UNBLOCK));
		ret = SIM_EnterPin(atHandle, CI_SIM_CHV_1, CI_SIM_CHV_UNBLOCK, (CHAR *)passwordStr, (CHAR *)newPasswordStr);
	}
	else if (strcmp(operStr, "052") == 0) //unblock PIN2
	{
		//ret = ciChvOper(CI_SIM_CHV_2,CI_SIM_CHV_UNBLOCK,passwordStr, newPasswordStr,MAKE_CI_REQ_HANDLE(atHandle,CI_SIM_CHV_UNBLOCK));
		ret = SIM_EnterPin(atHandle, CI_SIM_CHV_2, CI_SIM_CHV_UNBLOCK, (CHAR *)passwordStr, (CHAR *)newPasswordStr);
	}

	return(ret);
}

/************************************************************************************
 *
 * STK related Marvell internal AT commands
 *
 *************************************************************************************/

/************************************************************************************
 * F@: ciSTK - GLOBAL API to process STK request of AT+MSTK command
 *
 */
RETURNCODE_T  ciSTK(            const utlAtParameterOp_T op,
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

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT+MSTK=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+MSTK: (0-3),<data>" );
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+MSTK= */
	{
		int cmd = 0;
		INT16 dataLen = 0;
		CHAR data[CI_SIM_MAX_CMD_DATA_SIZE * 2 + ATCI_NULL_TERMINATOR_LENGTH];
		if ( ( getExtValue( parameter_values_p, 0, &cmd, 0, STK_TYPE_INVALID, 0 ) == TRUE) )
		{
			if(getExtString(parameter_values_p, 1, data, CI_SIM_MAX_APDU_DATA_SIZE * 2, &dataLen, NULL) == FALSE)
			{
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
				break;
			}

			/* Enable/Disable Proactive command Indication. Refer to <<CI_SIM_Service_Group.pdf>> 3.5.3~3.5.4 */
			if (( cmd == STK_CMD_ENABLE_SIMAT) && dataLen)
			{
				UINT8 enable = TRUE;
				if (strcmp((char *)data, "1") == 0)
				{
					enable = TRUE;
				}
				else if (strcmp((char *)data, "0") == 0)
				{
					enable = FALSE;
				}
				else
				{
					ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
					break;
				}

				ret = STK_EnableProactiveCmdIndConfig(atHandle, enable);
			}

			/* Get SIM card profile */
			else if ( cmd == STK_CMD_GET_PROFILE)
			{
				ret = STK_GetSimProfile(atHandle);
			}

			/* Download ME capability profile */
			else if (( cmd == STK_CMD_DOWNLOAD_PROFILE) && dataLen)
			{
				ret = STK_DownloadMeProfile(atHandle, (char *)data, (int)dataLen);
			}

			/* Send envelope command */
			else if (( cmd == STK_CMD_SEND_ENVELOPE) && dataLen)
			{
				ret = STK_SendEnvelopeCmd(atHandle, (char *)data, (int)dataLen);
			}

			/* Respond to proactive command */
			else if (( cmd == STK_CMD_PROACTIVE) && dataLen)
			{
				dbg_dump_string("STK proactive resp (buffer)", (char *)data, dataLen);
				ret = STK_RespProactiveCmd(atHandle, (char *)data, (int)dataLen);
			}

			/* Respond to setup call request from STK app */
			else if (( cmd == STK_CMD_SETUP_CALL) && dataLen)
			{
				UINT8 accept = TRUE;
				if (strcmp((char *)data, "1") == 0)
				{
					accept = TRUE;
				}
				else if (strcmp((char *)data, "0") == 0)
				{
					accept = FALSE;
				}
				else
				{
					ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
					break;
				}
				ret = STK_RespSetupCall(atHandle, accept);
			}

			/* Requests SIMAT Notification Capability info */
			else if ( cmd == STK_CMD_GET_CAP_INFO)
			{
				ret = STK_GetSimCapInfo(atHandle);
			}

			else
			{
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
			}

		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		}
		break;
	}

	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/*Michal Bukai - Support AT*EnVsim*/
/******************************************************************************
 ******
 * F@: ciMEPCLCK - GLOBAL API for GCF AT*ENVSIM -command
 *
 */
RETURNCODE_T  ciEnVsim(            const utlAtParameterOp_T op,
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
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_TEST_CMD:            /* AT*ENVSIM=?  */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char*)"*ENVSIM: OK");
		break;
	}
	case TEL_EXT_GET_CMD:            /* AT*ENVSIM? */
	{
		ret = VSIM_GetVSim(atHandle);
		break;
	}
	case TEL_EXT_ACTION_CMD:           /* AT*ENVSIM*/
	{
		ret = VSIM_SetVSim(atHandle);
		break;
	}
	case TEL_EXT_SET_CMD:              /* AT*ENVSIM= */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 * F@: ciMEPCG - GLOBAL API for GCF AT*MEPCG -command
 *
 */
RETURNCODE_T  ciMEPCG(            const utlAtParameterOp_T        op,
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
	RETURNCODE_T                         rc = INITIAL_RETURN_CODE;
	CiReturnCode                         ret = CIRC_SUCCESS;
	INT16                                facStrLen = 0;
	AtciFacType                   		 ciFacType;
	CHAR                          		 facStr[TEL_AT_CLCK_0_FAC_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];

	/*
	 *  Put parser index into the variable
	 */
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 **  Check the format of the request.
	 */
	switch(op)
	{
		case TEL_EXT_TEST_CMD: /* AT*MEPCG=?  */
			{
				ATRESP( atHandle,  ATCI_RESULT_CODE_OK, 0, (char*)"*MEPCG: (\"PS\",\"PN\",\"PU\",\"PP\",\"PC\")\r\n" );
				break;
			}
		case TEL_EXT_SET_CMD: /* AT*MEPCG= */
			{
				/*
				 **  Get fac string and mode (mandatory)....
				 */
				if (( getExtString( parameter_values_p, 0, &facStr[0], TEL_AT_MEP_CLCK_0_FAC_STR_MAX_LEN, &facStrLen, TEL_AT_MEP_CLCK_0_FAC_STR_DEFAULT) == TRUE ))
				{
					/*
					 **  If fac code valid, process request....
					 */
					if (( checkValidFacility( &facStr[0], facStrLen, &ciFacType ) == TRUE ) && (ciFacType.facLogicGroup == ATCI_FAC_MEP))
					{
						MEPReadCodes(atHandle, ciFacType);
					}/* MEP Valid Facility */
					else
					{
						/* Invalid fac or mode */
						ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
					}
				} /*  get Facility */
				else
				{
					/* Invalid fac */
					ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
				}

				break;
			}
		case TEL_EXT_GET_CMD:    /* AT*MEPCG? */
		case TEL_EXT_ACTION_CMD:   /* AT*MEPCG */
		default:
			{
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
				break;
			}
	}
	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 * F@: ciMEPCLCK - GLOBAL API for GCF AT*CLCK -command
 *
 */
RETURNCODE_T  ciMEPCLCK(            const utlAtParameterOp_T        op,
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
	RETURNCODE_T                         rc = INITIAL_RETURN_CODE;
	CiReturnCode                         ret = CIRC_SUCCESS;
	INT16                                facStrLen = 0;
	INT16                                facPwdStrLen = 0;
	INT32                                mode;
	AtciFacType   		                 ciFacType;
	CHAR         		                 facStr[TEL_AT_CLCK_0_FAC_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];
	CHAR        		                 facPwdStr[TEL_AT_CLCK_2_PASSWD_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];

	/*
	 *  Put parser index into the variable
	 */
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 **  Check the format of the request.
	 */
	switch(op)
	{
		case TEL_EXT_TEST_CMD:    /* AT*CLCK=?  */
			{
				ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char*)"*CLCK: (\"PS\",\"PN\",\"PU\",\"PP\",\"PC\"), (0-2), <password>\r\n" );
				break;
			}
		case TEL_EXT_SET_CMD:      /* AT*CLCK= */
			{
				/*
				 **  Get fac string and mode (mandatory)....
				 */
				if (( getExtString( parameter_values_p, 0, &facStr[0], TEL_AT_MEP_CLCK_0_FAC_STR_MAX_LEN, &facStrLen, TEL_AT_MEP_CLCK_0_FAC_STR_DEFAULT) == TRUE ) &&
						( getExtValue(parameter_values_p, 1, &mode, TEL_AT_MEP_CLCK_1_MODE_VAL_MIN, TEL_AT_MEP_CLCK_1_MODE_VAL_MAX, TEL_AT_MEP_CLCK_1_MODE_VAL_DEFAULT) == TRUE ))
				{
					/*
					 **  If fac code valid, process request....
					 */
					if (( checkValidFacility( &facStr[0], facStrLen, &ciFacType ) == TRUE ) && (ciFacType.facLogicGroup == ATCI_FAC_MEP) &&
							( mode <= ATCI_SS_MODE_QUERY_STATUS ) )
					{
						/* retrieve the password */
						if( getExtString(parameter_values_p, 2, &facPwdStr[0], TEL_AT_MEP_CLCK_2_PASSWD_STR_MAX_LEN, &facPwdStrLen, TEL_AT_MEP_CLCK_2_PASSWD_STR_DEFAULT) == TRUE )
						{

							PersonalizeME(atHandle,ciFacType,mode,facPwdStr,facPwdStrLen, TRUE);

						}/*password*/
						else
						{
							/* Invalid password */
							ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
						}
					}  /* Valid Facility and mode */
					else
					{
						/* Invalid fac or mode */
						ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
					}
				} /*  get Facility and mode */
				else
				{
					/* Invalid fac or mode */
					ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
				}

				break;
			}
		case TEL_EXT_GET_CMD:    /* AT*CLCK? */
		case TEL_EXT_ACTION_CMD:   /* AT*CLCK */
		default:
			{
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
				break;
			}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
   / * F@: ciEUICC - GLOBAL API for AT*EUICC-command
 *
 */
RETURNCODE_T  ciEUICC(            const utlAtParameterOp_T op,
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
	CHAR euiccRspBuf[64];

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_GET_CMD:           /* AT*EUICC?  */
	{
		if (getSimTypeInd == TRUE){
		sprintf((char *)euiccRspBuf, "*EUICC: %d\r\n", gSimCardType);
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)euiccRspBuf);
		}
		else
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_NO_SIM, NULL);
		break;
	}
	case TEL_EXT_SET_CMD:           /* AT*EUICC=*/
	case TEL_EXT_TEST_CMD:          /* AT*EUICC=? */
	case TEL_EXT_ACTION_CMD:        /* AT*EUICC */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}

/************************************************************************************
 * F@: ciRunGSMAlgorithm - GLOBAL API to implement  AT+ERGA command
 *
 */
RETURNCODE_T  ciRunGSMAlgorithm(			const utlAtParameterOp_T op,
						const char						*command_name_p,
						const utlAtParameterValue_P2c parameter_values_p,
						const size_t num_parameters,
						const char						*info_text_p,
						unsigned int					*xid_p,
						void							*arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch ( op )
	{
	case TEL_EXT_SET_CMD:		  /* AT+ERGA= */
	{
		INT16 dataLen = 0;
		CHAR indata[RGA_RAND_LEN * 2 + 2];

		if ( getExtString(parameter_values_p, 0, indata, RGA_RAND_LEN * 2, &dataLen, NULL) == TRUE )
		{
			if (dataLen == (RGA_RAND_LEN * 2))
			{
				ret = SIM_RunGSMAlgorithm(atHandle, (char *)indata, (int)dataLen);
			}
			else
			{
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
			}
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		}

		break;
	}
	case TEL_EXT_TEST_CMD:		   /* AT+ERGA=? */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 * F@: ciRunContextAuth - GLOBAL API to implement AT+ERTCA command
 *
 */
RETURNCODE_T  ciRunTContextAuth(			const utlAtParameterOp_T op,
						const char						*command_name_p,
						const utlAtParameterValue_P2c parameter_values_p,
						const size_t num_parameters,
						const char						*info_text_p,
						unsigned int					*xid_p,
						void							*arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch ( op )
	{
	case TEL_EXT_SET_CMD:		  /* AT+ERTCA= */
	{
		INT16 randLen = 0, autnLen = 0;
		CHAR rand[RGA_RAND_LEN * 2 + 2];
		CHAR autn[RGA_AUTN_LEN * 2 + 2];

		if (getExtString(parameter_values_p, 0, rand, RGA_RAND_LEN * 2, &randLen, NULL) == TRUE)
		{
			if (getExtString(parameter_values_p, 1, autn, RGA_AUTN_LEN * 2, &autnLen, NULL) == TRUE)
			{
				if ((randLen == (RGA_RAND_LEN * 2)) &&
					(autnLen == (RGA_AUTN_LEN * 2)))
				{
					ret = SIM_RunTContextAuth(atHandle, (char *)rand, (int)randLen, (char *)autn, (int)autnLen);
				}
				else
				{
					ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
				}
			}
			else
			{
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
			}
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		}

		break;
	}
	case TEL_EXT_TEST_CMD:		   /* AT+ERTCA=? */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

