/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************
*  Filename: teldev.c
*
*  Description: Process dev related AT commands
*
*  History:
*   Jan 17, 2008 - Creation of file
*   Oct 17, 2008 - Qiang XU, Optimization
*
******************************************************************************/

/******************************************************************************
*    Include files
******************************************************************************/
#include "teldev.h"
#include "teldef.h"
#include "ci_api_types.h"
#include "ci_api.h"
#include "ci_dev.h"
#include "telmsg.h"
#include "telatci.h"
#include "telconfig.h"
#include "telatparamdef.h"
#include "telutl.h"
#include "dev_api.h"
#include "utilities.h"
#include "eeh_assert_notify.h"

/************************************************************************************
 *
 * Dev related global variables and structure
 *
 *************************************************************************************/

/************************************************************************************
 *
 * DEV related AT commands
 *
 *************************************************************************************/

/************************************************************************************
 * F@: ciFuncSet - GLOBAL API for GCF AT+CFUN -command
 *
 */
RETURNCODE_T  ciFuncSet(            const utlAtParameterOp_T op,
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
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_GET_CMD:            /* AT+CFUN?  */
	{
		ret = DEV_GetFunc(atHandle, FALSE);
		break;
	}

	case TEL_EXT_TEST_CMD:            /* AT+CFUN=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CFUN: (0,1,4,5),(0-1)\r\n");
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CFUN= */
	{
		INT32 funcVal, resetVal;
		/* Extract the arguments starting with the functionality. */
		if ( (getExtValue(parameter_values_p, 0, &funcVal, TEL_AT_CFUN_0_FUN_VAL_MIN, TEL_AT_CFUN_0_FUN_VAL_MAX, TEL_AT_CFUN_0_FUN_VAL_DEFAULT) == TRUE)
				&& ((funcVal <= ATCI_CFUN_OP_FULL_FUNC) || (funcVal == ATCI_CFUN_OP_DISABLE_RF_RXTX) || (funcVal == ATCI_PLUS_CFUN_OP_DISABLE_SIM)))
		{
			if(funcVal == ATCI_PLUS_CFUN_OP_DISABLE_SIM)
				funcVal = ATCI_CFUN_OP_DISABLE_SIM;
			/* Get the reset flag; default value shall be 0 */
			if ( getExtValue( parameter_values_p, 1, &resetVal, TEL_AT_CFUN_1_RST_VAL_MIN, TEL_AT_CFUN_1_RST_VAL_MAX, TEL_AT_CFUN_1_RST_VAL_DEFAULT ) == TRUE )
			{
				ret = DEV_SetFunc(atHandle, funcVal, resetVal, FALSE, 0, 0);
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

	case TEL_EXT_ACTION_CMD:           /* AT+CFUN */
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
 * F@: ciFuncSet - GLOBAL API for GCF AT*CFUN -command
 *
 */
RETURNCODE_T  ciFuncCpConfig(            const utlAtParameterOp_T op,
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
	INT32 IsCommFeatureConfig;
	INT32 CommFeatureConfig;

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_GET_CMD:            /* AT*CFUN?  */
	{
		ret = DEV_GetFunc(atHandle, TRUE);
		break;
	}

	case TEL_EXT_TEST_CMD:            /* AT*CFUN=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"*CFUN: (0,1,4,5,6,9),(0-1),(0,1),(0-63)\r\n");
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT*CFUN= */
	{
		INT32 funcVal, resetVal;
		/* Extract the arguments starting with the functionality. */
		if ( getExtValue(parameter_values_p, 0, &funcVal, TEL_AT_CFUN_0_FUN_VAL_MIN, TEL_AT_CFUN_0_FUN_VAL_MAX, TEL_AT_CFUN_0_FUN_VAL_DEFAULT) == TRUE )
		{
			/* Get the reset flag; default value shall be 0 */
			if ( getExtValue( parameter_values_p, 1, &resetVal, TEL_AT_CFUN_1_RST_VAL_MIN, TEL_AT_CFUN_1_RST_VAL_MAX, TEL_AT_CFUN_1_RST_VAL_DEFAULT ) == TRUE )
			{
				if ( getExtValue( parameter_values_p, 2, &IsCommFeatureConfig, TEL_AT_CFUN_2_EN_CONFIG_VAL_MIN, TEL_AT_CFUN_2_EN_CONFIG_VAL_MAX, TEL_AT_CFUN_2_EN_CONFIG_VAL_DEFAULT ) == TRUE )
				{
					if ( getExtValue( parameter_values_p, 3, &CommFeatureConfig, TEL_AT_CFUN_3_CONFIG_VAL_MIN, TEL_AT_CFUN_3_CONFIG_VAL_MAX, TEL_AT_CFUN_3_CONFIG_VAL_DEFAULT ) == TRUE )
					{
						ret = DEV_SetFunc(atHandle, funcVal, resetVal, TRUE, IsCommFeatureConfig, CommFeatureConfig);
					}
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

	case TEL_EXT_ACTION_CMD:           /* AT*CFUN */
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
   / * F@: ciDevStatus - GLOBAL API for GCF AT+CPAS-command
 *
 */
RETURNCODE_T  ciDevStatus(            const utlAtParameterOp_T op,
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

	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_TEST_CMD:            /* AT+CPAS=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CPAS: (0,2,3,4)\r\n");
		break;
	}

	case TEL_EXT_ACTION_CMD:           /* AT+CPAS */
	{
		/*
		**  Retrieve the device status from the global variable.
		**  This variable is updated everytime the CI_DEV_PRIM_STATUS_IND
		**  is received.  The result originates from the last time
		**  the indication was received.
		*/
		ret = DEV_GetDevStatus(atHandle);
		break;
	}

	case TEL_EXT_GET_CMD:                   /* AT+CPAS?  */
	case TEL_EXT_SET_CMD:                   /* AT+CPAS= */
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
   / * F@: ciCGED - GLOBAL API for AT+CGED-command
 *
 */
RETURNCODE_T  ciCGED(            const utlAtParameterOp_T op,
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
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_TEST_CMD:            /* AT+CGED=? */
	{
		ATRESP( atHandle, 0, 0, (char *)"AT+CGED=0            :Export the EM information once for each request.");
		ATRESP( atHandle, 0, 0, (char *)"AT+CGED=1,<interval> :Export the EM information periodically in every interval seconds.");
		ATRESP( atHandle, 0, 0, (char *)"AT+CGED=2            :Disable Engineering mode, stop output Engineering mode information.");
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_SUPPRESS, 0, NULL);
		break;
	}

	case TEL_EXT_SET_CMD:           /* AT+CGED=*/
	{
		INT32 modeVal,  timeVal;
		if ( getExtValue(parameter_values_p, 0, &modeVal, TEL_AT_CGED_0_MODE_VAL_MIN, TEL_AT_CGED_0_MODE_VAL_MAX, TEL_AT_CGED_0_MODE_VAL_DEFAULT) == TRUE )
		{
			if(getExtValue( parameter_values_p, 1, &timeVal, TEL_AT_CGED_1_INTERVAL_VAL_MIN, TEL_AT_CGED_1_INTERVAL_VAL_MAX, TEL_AT_CGED_1_INTERVAL_VAL_DEFAULT) == TRUE)
				ret = DEV_SetEngineModeReportOption(atHandle, modeVal, timeVal);
		}
		break;
	}

	case TEL_EXT_GET_CMD:                   /* AT+CGED?  */
	case TEL_EXT_ACTION_CMD:                /* AT+CGED */
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


/* IMEI */
/************************************************************************************
 * F@: ciCGSN - GLOBAL API for AT+CGSN/GSN command
 *
 */

RETURNCODE_T  ciCGSN(
	const utlAtParameterOp_T op,
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

	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_GET_CMD:                   /* AT+CGSN?  */
	case TEL_EXT_ACTION_CMD:                /* AT+CGSN */
	{
		ret = DEV_GetSerialNumId(atHandle);
		break;
	}

	case TEL_EXT_TEST_CMD:            /* AT+CGSN=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CGSN:\r\n");
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CGSN= */
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

/* Added by Michal Bukai - AT*BAND command*/
/************************************************************************************
 * F@: ciBAND - GLOBAL API for AT*BAND command
 *
 */
RETURNCODE_T  ciBAND(            const utlAtParameterOp_T op,
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
	INT32 NWMode, band;
	int roamingConfig, srvDomain;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);


	/*
	 *  Put parser index into the variable
	 */
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT*BAND=? */
	{
		ret = DEV_GetSupportedBandModeReq(atHandle);
		break;
	}

	case TEL_EXT_GET_CMD:         /* AT*BAND? */
	{
		ret = DEV_GetBandModeReq(atHandle);
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT*BAND= */
	{
		if ( getExtValue( parameter_values_p, 0, &NWMode, TEL_AT_BAND_NW_MODE_VAL_MIN, TEL_AT_BAND_NW_MODE_VAL_MAX, TEL_AT_BAND_NW_MODE_VAL_DEFAULT ) == TRUE )
		{
			if ( getExtValue( parameter_values_p, 1, &band, TEL_AT_BAND_BAND_VAL_MIN, TEL_AT_BAND_BAND_VAL_MAX, TEL_AT_BAND_BAND_VAL_DEFAULT ) == TRUE )
			{
				if ( getExtValue( parameter_values_p, 2, &roamingConfig, TEL_AT_BAND_ROAMING_CONFIG_VAL_MIN, TEL_AT_BAND_ROAMING_CONFIG_VAL_MAX, TEL_AT_BAND_ROAMING_CONFIG_VAL_DEFAULT ) == TRUE )
				{
					if ( getExtValue( parameter_values_p, 3, &srvDomain, TEL_AT_BAND_SRV_DOMAIN_VAL_MIN, TEL_AT_BAND_SRV_DOMAIN_VAL_MAX, TEL_AT_BAND_SRV_DOMAIN_VAL_DEFAULT ) == TRUE )
					{
						switch (NWMode) {
						case 0:
							ret = DEV_SetBandModeReq(atHandle, NWMode, -1, band, 0, roamingConfig, srvDomain);
							break;

						case 1:
							ret = DEV_SetBandModeReq(atHandle, NWMode, -1, 0, band, roamingConfig, srvDomain);
							break;

						case 2:
							ret = DEV_SetBandModeReq(atHandle, 2, CI_DEV_NW_DUAL_MODE, 0, 0, roamingConfig, srvDomain);
							break;

						case 3:
							ret = DEV_SetBandModeReq(atHandle, 2, CI_DEV_NW_GSM, 0, 0, roamingConfig, srvDomain);
							break;

						case 4:
							ret = DEV_SetBandModeReq(atHandle, 2, CI_DEV_NW_UMTS, 0, 0, roamingConfig, srvDomain);
							break;
						}
					}
				}
			}
		}
		break;
	}
	case TEL_EXT_ACTION_CMD:           /* AT*BAND */
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
 * F@: ciPOWERIND - GLOBAL API for GCF AT*POWERIND command
 *
 */
RETURNCODE_T  ciPOWERIND(            const utlAtParameterOp_T op,
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

	INT32 powerInd;


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
	case TEL_EXT_TEST_CMD:         /* AT*POWERIND=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"*POWERIND: (0-1)\r\n");
		break;
	}

	case TEL_EXT_SET_CMD:             /* AT*POWERIND= */
	{
		if ( getExtValue(parameter_values_p, 0, &powerInd, TEL_AT_POWERIND_MODE_VAL_MIN, TEL_AT_POWERIND_MODE_VAL_MAX, TEL_AT_POWERIND_MODE_VAL_DEFAULT) == TRUE )
		{
			ret = DEV_SetPOWERIND(atHandle, powerInd);
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
		
/* Added by Michal Bukai */
/************************************************************************************
 * F@: ciSilentReset - GLOBAL API for GCF AT*CPRST command
 *
 */
#if 0

RETURNCODE_T ciSilentReset(            const utlAtParameterOp_T op,
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

	static CiDevPrimCommAssertReq commAssertReq;

	/*
	 *  Put parser index into the variable
	 */
	CiRequestHandle reqHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	reqHandle = RET_CI_REQ_HANDLE( sAtpIndex, CI_SG_ID_DEV );
	*xid_p = reqHandle;

	DBGMSG("%s: reqHandle = %d.\n", __FUNCTION__, reqHandle);
	RESP_DEBUG((CHAR *)"Enter ciSilentReset", TRUE, FALSE);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT*CPRST=? */
	{
		RESP( reqHandle, (char *)"OK\r\n", TRUE, FALSE );
		ret = CIRC_SUCCESS;
		break;
	}

	case TEL_EXT_ACTION_CMD:           /* AT*CPRST */
	{
		ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DEV], CI_DEV_PRIM_COMM_ASSERT_REQ,
				 reqHandle, (void *)&commAssertReq );
		break;
	}

	case TEL_EXT_GET_CMD:                   /* AT*CPRST? */
	case TEL_EXT_SET_CMD:                   /* AT*CPRST= */
	default:
	{
		ret = OPER_NOT_SUPPORTED;
		break;
	}
	}

	RESP_DEBUG((CHAR *)"Exit ciSilentReset", TRUE, TRUE);

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret, reqHandle);
	return(rc);
}

/* Added by Michal Bukai*/
/************************************************************************************
 * F@: ciCMER - GLOBAL API for GCF AT+CMER command
 *
 */
RETURNCODE_T  ciCMER(            const utlAtParameterOp_T op,
				 __attribute__((unused)) const char                      *command_name_p,
				 __attribute__((unused)) const utlAtParameterValue_P2c parameter_values_p,
				 __attribute__((unused)) const size_t num_parameters,
				 __attribute__((unused)) const char                      *info_text_p,
				 unsigned int                    *xid_p,
				 void                            *arg_p)
{
	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_SUCCESS;
	BOOL cmdValid = FALSE;

	INT32 dwMode = 0;
	INT32 dwKeyp = 0;
	INT32 dwDisp = 0;
	INT32 dwInd  = 0;
	INT32 dwBfr  = 0;

	CHAR AtRspBuf [50];

	/*
	 *  Put parser index into the variable
	 */
	CiRequestHandle reqHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;

	reqHandle = RET_CI_REQ_HANDLE( sAtpIndex, CI_SG_ID_DEV );
	*xid_p = reqHandle;

	DBGMSG("%s: reqHandle = %d.\n", __FUNCTION__, reqHandle);
	RESP_DEBUG((CHAR *)"Enter ciCMER", TRUE, FALSE);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT+CMER=? */
	{
		/* display "+CMER: list of supported modes - only parameter #4 (ind) is configurable" */
		RESP( reqHandle, (char *)"+CMER: (0), (0), (0), (0-2), (0)\r\nOK\r\n", TRUE, FALSE );
		break;
	}

	case TEL_EXT_GET_CMD:         /* AT+CMER? */
	{
		/* display "+CMER: current mode" */
		sprintf((char*)AtRspBuf, "+CMER: 0,0,0,%d,0\r\nOK\r\n", g_uintCMERIndMode);
		RESP( reqHandle, (char *)AtRspBuf, TRUE, FALSE);
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+CMER= */
	{
		if ( getExtValue(parameter_values_p, 0, &dwMode, TEL_AT_CMER_MODE_VAL_DEFAULT, TEL_AT_CMER_MODE_VAL_DEFAULT, TEL_AT_CMER_MODE_VAL_DEFAULT) == TRUE )
		{
			if ( getExtValue(parameter_values_p, 1, &dwKeyp, TEL_AT_CMER_KEY_VAL_DEFAULT, TEL_AT_CMER_KEY_VAL_DEFAULT, TEL_AT_CMER_KEY_VAL_DEFAULT) == TRUE )
			{
				if ( getExtValue(parameter_values_p, 2, &dwDisp, TEL_AT_CMER_DIS_VAL_DEFAULT, TEL_AT_CMER_DIS_VAL_DEFAULT, TEL_AT_CMER_DIS_VAL_DEFAULT) == TRUE )
				{
					if ( getExtValue(parameter_values_p, 3, &dwInd, TEL_AT_CMER_IND_VAL_MIN, TEL_AT_CMER_IND_VAL_MAX, TEL_AT_CMER_IND_VAL_DEFAULT) == TRUE )
					{
						if ( getExtValue(parameter_values_p, 4, &dwBfr, TEL_AT_CMER_BUFFER_VAL_DEFAULT, TEL_AT_CMER_BUFFER_VAL_DEFAULT, TEL_AT_CMER_BUFFER_VAL_DEFAULT) == TRUE )
						{
							g_uintCMERIndMode  = dwInd;
							cmdValid = TRUE;
							RESP( reqHandle, (char *)"OK", TRUE, TRUE );
						}
					}
				}
			}
		}
		if (cmdValid == FALSE)
			ret = INVALID_PARAM;
		break;
	}

	case TEL_EXT_ACTION_CMD:   /* AT+CMER */
	default:
		ret = OPER_NOT_SUPPORTED;
		break;
	}

	RESP_DEBUG((CHAR *)"Exit ciCMER", TRUE, TRUE);

	rc = HANDLE_RETURN_VALUE(ret, reqHandle);
	return(rc);
}
#endif

/************************************************************************************
   / * F@: ciEEMOPT - GLOBAL API for AT+EEMOPT-command
 *
 */
RETURNCODE_T  ciEEMOPT(            const utlAtParameterOp_T op,
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
	char AtRspBuf[50];
	extern CiDevEngModeReportOption gEngModeRepOpt;
	extern int gCurrTimeVal;

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_TEST_CMD:            /* AT+EEMOPT=? */
	{
		ATRESP( atHandle, 0, 0, (char *)"AT+EEMOPT=0            :Turn off indicator. This is default mode after ME bootup");
		ATRESP( atHandle, 0, 0, (char *)"AT+EEMOPT=1            :Set to query mode. User can use +EEMGINFO to query network parameter");
		ATRESP( atHandle, 0, 0, (char *)"AT+EEMOPT=2,<interval> :Set to periodic mode. Report EM info in <value> seconds");
		ATRESP( atHandle, 0, 0, (char *)"AT+EEMOPT=3            :The old <value> will be stored as snapshot");
		ATRESP( atHandle, 0, 0, (char *)"AT+EEMOPT=4            :Restore to snapshot <value>");
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_SUPPRESS, 0, NULL);
		break;
	}
	case TEL_EXT_SET_CMD:           /* AT+EEMOPT=*/
	{
		INT32 modeVal,  timeVal;
		if ( getExtValue(parameter_values_p, 0, &modeVal, TEL_AT_EEMOPT_0_MODE_VAL_MIN, TEL_AT_EEMOPT_0_MODE_VAL_MAX, TEL_AT_EEMOPT_0_MODE_VAL_DEFAULT) == TRUE )
		{
			if(getExtValue( parameter_values_p, 1, &timeVal, TEL_AT_EEMOPT_1_INTERVAL_VAL_MIN, TEL_AT_EEMOPT_1_INTERVAL_VAL_MAX, TEL_AT_EEMOPT_1_INTERVAL_VAL_DEFAULT) == TRUE)
				ret = DEV_SetEngModeIndicatorOption(atHandle, modeVal, timeVal);
		}
		break;
	}
	case TEL_EXT_GET_CMD:                   /* AT+EEMOPT?  */
	{
		if (gEngModeRepOpt == CI_DEV_EM_OPTION_PERIODIC)
			sprintf((char*)AtRspBuf, "+EEMOPT: %d,%d\r\n", gEngModeRepOpt, gCurrTimeVal);
		else
			sprintf((char*)AtRspBuf, "+EEMOPT: %d\r\n", gEngModeRepOpt);

		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, AtRspBuf);

		break;
	}
	case TEL_EXT_ACTION_CMD:                /* AT+EEMOPT */
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
   / * F@: ciEEMOPT - GLOBAL API for AT+EEMGINFO-command
 *
 */
RETURNCODE_T  ciEEMGINFO(            const utlAtParameterOp_T op,
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
	extern CiDevEngModeReportOption gEngModeRepOpt;

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_GET_CMD:           /* AT+EEMGINFO?  */
	{
		if (gEngModeRepOpt == CI_DEV_EM_OPTION_REQUEST)
			ret = DEV_GetEngModeInfo(atHandle);
		else
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_ALLOWED, NULL);
		break;
	}
	case TEL_EXT_SET_CMD:           /* AT+EEMGINFO=*/
	case TEL_EXT_TEST_CMD:          /* AT+EEMGINFO=? */
	case TEL_EXT_ACTION_CMD:        /* AT+EEMGINFO */
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

#define TEL_AT_LPNWUL_1_LEN_VAL_MIN 0
#define TEL_AT_LPNWUL_1_LEN_VAL_MAX 2000
#define TEL_AT_LPNWUL_1_LEN_VAL_DEFAULT 500
#define TEL_AT_LPNWUL_2_COUNT_VAL_MIN 0
#define TEL_AT_LPNWUL_2_COUNT_VAL_MAX 3
#define TEL_AT_LPNWUL_2_COUNT_VAL_DEFAULT 0
#define TEL_AT_LPNWUL_3_TYPE_VAL_MIN 0
#define TEL_AT_LPNWUL_3_TYPE_VAL_MAX 4
#define TEL_AT_LPNWUL_3_TYPE_VAL_DEFAULT 0
#define TEL_AT_LPNWUL_4_FINAL_VAL_MIN 0
#define TEL_AT_LPNWUL_4_FINAL_VAL_MAX 1
#define TEL_AT_LPNWUL_4_FINAL_VAL_DEFAULT 0

/************************************************************************************
   / * F@: ciLPNWUL - GLOBAL API for AT+LPNWUL-command
 *
 */
RETURNCODE_T  ciLPNWUL(            const utlAtParameterOp_T op,
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
	eeh_log(LOG_NOTICE, "Enter %s ", __FUNCTION__);

	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_SET_CMD:           /* AT+LPNWUL=*/
	{
		CHAR  msg_data[CI_DEV_MAX_APGS_MSG_SIZE*2 + 2];
		INT32 len, msg_data_len;
		INT32  count;
		INT32  bearer_type;
		INT32  isFinalRsp;

		if(getExtString(parameter_values_p, 0, msg_data, CI_DEV_MAX_APGS_MSG_SIZE*2 + 1, (INT16 *)&len, NULL) == TRUE)
		{
			if(getExtValue( parameter_values_p, 1, &msg_data_len, TEL_AT_LPNWUL_1_LEN_VAL_MIN, TEL_AT_LPNWUL_1_LEN_VAL_MAX, TEL_AT_LPNWUL_1_LEN_VAL_DEFAULT) == TRUE)
			{
				if(getExtValue( parameter_values_p, 2, &count, TEL_AT_LPNWUL_2_COUNT_VAL_MIN, TEL_AT_LPNWUL_2_COUNT_VAL_MAX, TEL_AT_LPNWUL_2_COUNT_VAL_DEFAULT) == TRUE)
				{
					if(getExtValue( parameter_values_p, 3, &bearer_type, TEL_AT_LPNWUL_3_TYPE_VAL_MIN, TEL_AT_LPNWUL_3_TYPE_VAL_MAX, TEL_AT_LPNWUL_3_TYPE_VAL_DEFAULT) == TRUE)
					{
						if(getExtValue( parameter_values_p, 4, &isFinalRsp, TEL_AT_LPNWUL_4_FINAL_VAL_MIN, TEL_AT_LPNWUL_4_FINAL_VAL_MAX, TEL_AT_LPNWUL_4_FINAL_VAL_DEFAULT) == TRUE)
						{
							/* set to CI_DEV_PRIM_LP_NWUL_MSG_REQ*/
							ret = DEV_setLPNWUL(atHandle, (UINT8 *)msg_data, msg_data_len, count, bearer_type, isFinalRsp);
						}
					}
				}
			}
		}
		break;
	}
	case TEL_EXT_TEST_CMD:			/* AT+LPNWUL=? */
	case TEL_EXT_GET_CMD:                   /* AT+LPNWUL?  */
	case TEL_EXT_ACTION_CMD:                /* AT+LPNWUL   */
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
 * F@: ciOFF - GLOBAL API for AT+OFF -command
 *
 */
RETURNCODE_T  ciOFF(            const utlAtParameterOp_T op,
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

	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_ACTION_CMD:                /* AT+OFF */
	{
		ret = DEV_SetFunc(atHandle, ATCI_CFUN_OP_MIN_FUNC_NO_IMSI_DETACH, 0, FALSE, 0, 0);
		break;
	}

	case TEL_EXT_TEST_CMD:            /* AT+OFF=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+OFF\r\n");
		break;
	}

	case TEL_EXT_GET_CMD:              /* AT+OFF?  */
	case TEL_EXT_SET_CMD:              /* AT+OFF= */
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

/* SV */
/************************************************************************************
 * F@: ciStarCGSN - GLOBAL API for AT*CGSN command
 *
 */

RETURNCODE_T  ciStarCGSN(
	const utlAtParameterOp_T op,
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

	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_GET_CMD:                   /* AT*CGSN?  */
	{
		ret = DEV_GetSoftwareVersionNumber(atHandle);
		break;
	}

	case TEL_EXT_TEST_CMD:            /* AT*CGSN=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"*CGSN: <sv>\r\n");
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT*CGSN= */
	{
		char sv[3];
		INT16 len;
		if(getExtString(parameter_values_p, 0, (CHAR *)sv, 2, &len, NULL) == TRUE)
		{
			if(len != 2)
			{
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
			}
			else
			{
				ret = DEV_SetSoftwareVersionNumber(atHandle, sv);
			}
		}

		break;
	}

	case TEL_EXT_ACTION_CMD:                /* AT*CGSN */
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
 * F@: ciStarMODEMTYPE - GLOBAL API for AT*MODEMTYPE -command
 *
 */
RETURNCODE_T  ciStarMODEMTYPE(            const utlAtParameterOp_T op,
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

	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_GET_CMD:			   /* AT*MODEMTYPE?  */
	{
		int modem_type;
		unsigned long chipid;
		char buf[100];

		if (get_chipid(&chipid) > 0)
		{
			DBGMSG("%s: chip id = 0x%x.\n", __FUNCTION__, chipid);
			switch (chipid & MODEM_FLAG_MASK)
			{
			case PXA910_MODEM_FLAG:
				modem_type = PXA910_MODEM;
				break;
			case PXA920_MODEM_FLAG:
				modem_type = PXA920_MODEM;
				break;
			default:
				modem_type = UNKNOWN_MODEM;
				break;
			}

			sprintf(buf, "*MODEMTYPE: %d\r\n", modem_type);
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, buf);
		}
		else
		{
			DBGMSG("%s: Can not get chip id.\n", __FUNCTION__);
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		}

		break;
	}
	case TEL_EXT_ACTION_CMD:          /* AT*MODEMTYPE */
	case TEL_EXT_TEST_CMD:            /* AT*MODEMTYPE=? */
	case TEL_EXT_SET_CMD:             /* AT*MODEMTYPE= */
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
 * F@: ciStarHTCCTO - GLOBAL API for AT*HTCCTO -command
 *
 */
RETURNCODE_T  ciStarHTCCTO(            const utlAtParameterOp_T op,
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

	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_SET_CMD:			   /* AT*HTCCTO=  */
	{
		CHAR commandName[utlAT_MAX_COMMAND_LENGTH_WITH_PREFIX];
		INT16 len;
		int mode, timeout;
		BOOL cmdValid = FALSE;
		if(getExtString(parameter_values_p, 0, commandName, utlAT_MAX_COMMAND_LENGTH_WITH_PREFIX - 1, (INT16 *)&len, NULL) == TRUE)
		{
			if(getExtValue( parameter_values_p, 1, &mode, TEL_AT_HTCCTO_2_MODE_VAL_MIN, TEL_AT_HTCCTO_2_MODE_VAL_MAX, TEL_AT_HTCCTO_2_MODE_VAL_DEFAULT) == TRUE)
			{
				if(getExtValue( parameter_values_p, 2, &timeout, 0, 65536, 0) == TRUE)
				{
					cmdValid = TRUE;
				}
			}
		}
		if(cmdValid == TRUE)
		{
			if(utlAtCommandTimeoutModify((char *)commandName, mode, timeout))
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
			else
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		}
		else
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
		break;
	}
	case TEL_EXT_ACTION_CMD:          /* AT*HTCCTO */
	case TEL_EXT_TEST_CMD:            /* AT*HTCCTO=? */
	case TEL_EXT_GET_CMD:             /* AT*HTCCTO? */
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
 * F@: ciStarMODEMRESET - GLOBAL API for AT*MODEMRESET -command
 *
 */
RETURNCODE_T  ciStarMODEMRESET(            const utlAtParameterOp_T op,
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

	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_SET_CMD:			   /* AT*MODEMRESET=  */
	{
		char buf[512];
		INT16 len;
		if(getExtString(parameter_values_p, 0, (CHAR *)buf, sizeof(buf) - 1, &len, NULL) == TRUE)
		{
			if(cp_silent_reset_on_req(buf) != 0)
			{
				DBGMSG("cp silent reset req error\n");
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
			}
			else
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
		else
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
		break;
	}
	case TEL_EXT_ACTION_CMD:          /* AT*MODEMRESET */
	case TEL_EXT_TEST_CMD:            /* AT*MODEMRESET=? */
	case TEL_EXT_GET_CMD:             /* AT*MODEMRESET?*/
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
 * F@: ciStarEHSDPA - GLOBAL API for AT*EHSDPA -command
 *
 */
RETURNCODE_T  ciStarEHSDPA(            const utlAtParameterOp_T op,
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

	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_SET_CMD:			   /* AT*EHSDPA=  */
	{
		int mode, dl_category, ul_category, cpc_state;
		BOOL cmdValid = FALSE;
		if(getExtValue( parameter_values_p, 0, &mode, TEL_AT_EHSDPA_0_MODE_VAL_MIN, TEL_AT_EHSDPA_0_MODE_VAL_MAX, TEL_AT_EHSDPA_0_MODE_VAL_DEFAULT) == TRUE)
		{
			if((getExtValue( parameter_values_p, 1, &dl_category, TEL_AT_EHSDPA_1_DL_CATEGORY_VAL_MIN, TEL_AT_EHSDPA_1_DL_CATEGORY_VAL_MAX, TEL_AT_EHSDPA_1_DL_CATEGORY_VAL_DEFAULT) == TRUE)
			#ifdef TD_SUPPORT
				&& (dl_category != TEL_AT_EHSDPA_1_DL_CATEGORY_VAL_EXCEPTION)
			#endif
				)
			{
				if(getExtValue( parameter_values_p, 2, &ul_category, TEL_AT_EHSDPA_2_UL_CATEGORY_VAL_MIN, TEL_AT_EHSDPA_2_UL_CATEGORY_VAL_MAX, TEL_AT_EHSDPA_2_UL_CATEGORY_VAL_DEFAULT) == TRUE)
				{
					if(getExtValue( parameter_values_p, 3, &cpc_state, TEL_AT_EHSDPA_3_CPC_STATE_VAL_MIN, TEL_AT_EHSDPA_3_CPC_STATE_VAL_MAX, TEL_AT_EHSDPA_3_CPC_STATE_VAL_DEFAULT) == TRUE)
					{
						cmdValid = TRUE;
					}
				}
			}
		}
		if(cmdValid)
		{
			ret = DEV_SetHsdpaModeReq(atHandle, (UINT8)mode, (UINT8)dl_category, (UINT8)ul_category, (UINT8)cpc_state);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
		}
		break;
	}
	case TEL_EXT_GET_CMD:             /* AT*EHSDPA? */
	{
		ret = DEV_GetHsdpaModeReq(atHandle);
		break;
	}
	case TEL_EXT_TEST_CMD:            /* AT*EHSDPA=? */
	case TEL_EXT_ACTION_CMD:          /* AT*EHSDPA*/
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

#ifdef AT_PRODUCTION_CMNDS

/************************************************************************************
 * F@: ciTDTR - GLOBAL API for AT*TDTR command
 *
 */
RETURNCODE_T  ciTDTR(            const utlAtParameterOp_T op,
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
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);


	/*
	 *  Put parser index into the variable
	 */
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT*TDTR=? */
	{
		ATRESP( atHandle, 0, 0, (char *)"*TDTR=<op>[,<gain>,<freq>]");
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}
	case TEL_EXT_SET_CMD:         /* AT*TDTR= */
	{
		int option;
		int txRxGain;
		char txRxGainStr[TEL_AT_TDTR_TX_RX_GAIN_STR_LEN_MAX + ATCI_NULL_TERMINATOR_LENGTH] = {0};
		int txRxGainLen = 0;
		int freq;
		BOOL cmdValid = FALSE;

		if ( getExtValue(parameter_values_p, 0, &option, TEL_AT_TDTR_OPTION_MIN, TEL_AT_TDTR_OPTION_MAX, TEL_AT_TDTR_OPTION_DEFAULT) == TRUE )
		{
			int val_tbl[TEL_AT_TDTR_OPTION_MAX - TEL_AT_TDTR_OPTION_MIN + 1][3] = {
				{TEL_AT_TDTR_TX_GAIN_MIN, TEL_AT_TDTR_TX_GAIN_MAX, TEL_AT_TDTR_TX_GAIN_DEFAULT},
				{TEL_AT_TDTR_RX_GAIN_MIN, TEL_AT_TDTR_RX_GAIN_MAX, TEL_AT_TDTR_RX_GAIN_DEFAULT},
				{INT_MIN, INT_MAX, 0}
			};

			if ( getExtString(parameter_values_p, 1, (CHAR *)txRxGainStr, TEL_AT_TDTR_TX_RX_GAIN_STR_LEN_MAX, (INT16 *)&txRxGainLen, NULL) == TRUE )
			{
				if(txRxGainLen)
					txRxGain = atoi(txRxGainStr);
				else
					txRxGain = val_tbl[option - TEL_AT_TDTR_OPTION_MIN][2];

				if(txRxGain >= val_tbl[option - TEL_AT_TDTR_OPTION_MIN][0] && txRxGain <= val_tbl[option - TEL_AT_TDTR_OPTION_MIN][1])
				{
					if ( getExtValue(parameter_values_p, 2, &freq, TEL_AT_TDTR_FREQ_RANGE1_MIN, TEL_AT_TDTR_FREQ_RANGE2_MAX, TEL_AT_TDTR_FREQ_DEFAULT) == TRUE )
					{
						if(freq > TEL_AT_TDTR_FREQ_RANGE1_MAX && freq < TEL_AT_TDTR_FREQ_RANGE2_MIN)
							cmdValid = FALSE;
						else
							cmdValid = TRUE;
					}
				}

			}
		}

		if (cmdValid == TRUE)
		{
			ret = DEV_SetTdModeTxRxReq(atHandle, (CiDevTdTxRxOption)option, (INT16)txRxGain, (UINT16)freq);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
		}

		break;
	}
	case TEL_EXT_GET_CMD:         /* AT*TDTR? */
	case TEL_EXT_ACTION_CMD:      /* AT*TDTR */
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

BOOL checkArfcnValue(int gsmBandMode, int arfcn)
{
	/* refer to 3GPP TS 45005 */
	BOOL ret = FALSE;
	if(gsmBandMode & (1 << CI_DEV_PGSM_900))
	{
		ret |= (arfcn >= 1 && arfcn <= 124);
	}
	if(gsmBandMode & (1 << CI_DEV_DCS_GSM_1800))
	{
		ret |= (arfcn >= 512 && arfcn < 885);
	}
	if(gsmBandMode & (1 << CI_DEV_PCS_GSM_1900))
	{
		ret |= (arfcn >= 512 && arfcn <= 810);
	}
	if(gsmBandMode & (1 << CI_DEV_EGSM_900))
	{
		ret |= ((arfcn >= 0 && arfcn <= 124) || (arfcn >= 975 && arfcn <= 1023));
	}
	if(gsmBandMode & (1 << CI_DEV_GSM_450))
	{
		ret |= (arfcn >= 259 && arfcn <= 293);
	}
	if(gsmBandMode & (1 << CI_DEV_GSM_480))
	{
		ret |= (arfcn >= 306 && arfcn <= 340);
	}
	if(gsmBandMode & (1 << CI_DEV_GSM_850))
	{
		ret |= (arfcn >= 128 && arfcn <= 251);
	}

	return ret;
}

/************************************************************************************
 * F@: ciGSMTR - GLOBAL API for AT*GSMTR command
 *
 */
RETURNCODE_T  ciGSMTR(            const utlAtParameterOp_T op,
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
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);


	/*
	 *  Put parser index into the variable
	 */
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT*GSMTR=? */
	{
		ATRESP( atHandle, 0, 0, (char *)"*GSMTR=<op>[,<band>,<arfcn>,<afcDac>[,<txRampScale>,<rxGainCode>]]");
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}
	case TEL_EXT_SET_CMD:         /* AT*GSMTR= */
	{
		int option;
		int gsmBandMode;
		int arfcn;
		int afcDac;
		int txRampScale;
		int rxGainCode;
		BOOL cmdValid = FALSE;

		if ( getExtValue(parameter_values_p, 0, &option, TEL_AT_GSMTR_OPTION_MIN, TEL_AT_GSMTR_OPTION_MAX, TEL_AT_GSMTR_OPTION_DEFAULT) == TRUE )
		{
			if ( getExtValue(parameter_values_p, 1, &gsmBandMode, TEL_AT_GSMTR_BAND_MODE_MIN, TEL_AT_GSMTR_BAND_MODE_MAX, TEL_AT_GSMTR_BAND_MODE_DEFAULT) == TRUE )
			{
				if ( getExtValue(parameter_values_p, 2, &arfcn, TEL_AT_GSMTR_ARFCN_MIN, TEL_AT_GSMTR_ARFCN_MAX, TEL_AT_GSMTR_ARFCN_DEFAULT) == TRUE )
				{
					if ( getExtValue(parameter_values_p, 3, &afcDac, TEL_AT_GSMTR_AFCDAC_MIN, TEL_AT_GSMTR_AFCDAC_MAX, TEL_AT_GSMTR_AFCDAC_DEFAULT) == TRUE )
					{
						if ( getExtValue(parameter_values_p, 4, &txRampScale, TEL_AT_GSMTR_TXRAMP_SCALE_MIN, TEL_AT_GSMTR_TXRAMP_SCALE_MAX, TEL_AT_GSMTR_TXRAMP_SCALE_DEFAULT) == TRUE )
						{
							if ( getExtValue(parameter_values_p, 5, &rxGainCode, TEL_AT_GSMTR_RXGAIN_CODE_MIN, TEL_AT_GSMTR_RXGAIN_CODE_MAX, TEL_AT_GSMTR_RXGAIN_CODE_DEFAULT) == TRUE )
							{
								if((option == CI_DEV_GSM_TX_RX_STOP) || checkArfcnValue(gsmBandMode, arfcn))
									cmdValid = TRUE;
							}
						}
					}
				}
			}
		}

		if (cmdValid == TRUE)
		{
			ret = DEV_SetGsmModeTxRxReq(atHandle, (CiDevGsmTxRxOption)option, (UINT8)gsmBandMode,
				(UINT16)arfcn, (UINT32)afcDac, (UINT32)txRampScale, (UINT32)rxGainCode);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
		}

		break;
	}
	case TEL_EXT_GET_CMD:         /* AT*GSMTR? */
	case TEL_EXT_ACTION_CMD:      /* AT*GSMTR */
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
 * F@: ciTGCTRL - GLOBAL API for AT*TGCTRL command
 *
 */
RETURNCODE_T  ciTGCTRL(            const utlAtParameterOp_T op,
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
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);


	/*
	 *  Put parser index into the variable
	 */
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT*TGCTRL=? */
	{
		ATRESP( atHandle, 0, 0, (char *)"*TGCTRL=<nw>,<op>,<value>[,<addr>]");
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}
	case TEL_EXT_SET_CMD:         /* AT*TGCTRL= */
	{
		int network = 0;
		int mode = 0;
		int regValue = 0;
		int addrReg = 0;
		BOOL cmdValid = FALSE;

		if ( getExtValue(parameter_values_p, 0, &network, TEL_AT_TGCTRL_NETWORK_MIN, TEL_AT_TGCTRL_NETWORK_MAX, TEL_AT_TGCTRL_NETWORK_DEFAULT) == TRUE )
		{
			int val_tbl[TEL_AT_TGCTRL_NETWORK_MAX-TEL_AT_TGCTRL_NETWORK_MIN+1][3] = {
				{TEL_AT_TGCTRL_TD_OPTION_MIN, TEL_AT_TGCTRL_TD_OPTION_MAX, TEL_AT_TGCTRL_TD_OPTION_DEFAULT},
				{TEL_AT_TGCTRL_GSM_OPTION_MIN, TEL_AT_TGCTRL_GSM_OPTION_MAX, TEL_AT_TGCTRL_GSM_OPTION_DEFAULT},
			};

			if ( getExtValue(parameter_values_p, 1, &mode, val_tbl[network-TEL_AT_TGCTRL_NETWORK_MIN][0], val_tbl[network-TEL_AT_TGCTRL_NETWORK_MIN][1], val_tbl[network-TEL_AT_TGCTRL_NETWORK_MIN][2]) == TRUE )
			{
				if ( getExtValue(parameter_values_p, 2, &regValue, INT_MIN, INT_MAX, 0) == TRUE )
				{
					if ( getExtValue(parameter_values_p, 3, &addrReg, TEL_AT_TGCTRL_GSM_REG_ADDR_MIN, TEL_AT_TGCTRL_GSM_REG_ADDR_MAX, TEL_AT_TGCTRL_GSM_REG_ADDR_DEFAULT) == TRUE )
					{
						cmdValid = TRUE;
					}
				}
			}
		}

		if (cmdValid == TRUE)
		{
			if(network == TEL_AT_TGCTRL_NETWORK_TD) /* TD network */
			{
				ret = DEV_SetTdModeLoopbackReq(atHandle, (UINT32)regValue);
			}
			else /* GSM network */
			{
				ret = DEV_SetGsmControlInterfaceReq(atHandle, (CiDevGsmControlMode)mode, (UINT16)addrReg, (UINT16)regValue);
			}
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
		}

		break;
	}
	case TEL_EXT_GET_CMD:         /* AT*TGCTRL? */
	case TEL_EXT_ACTION_CMD:      /* AT*TGCTRL */
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

#endif
#ifdef AT_CUSTOMER_HTC
/************************************************************************************
 * F@: ciTGCTRL - GLOBAL API for AT+TPCN command
 *
 */
RETURNCODE_T  ciTPCN(            const utlAtParameterOp_T op,
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
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);


	/*
	 *  Put parser index into the variable
	 */
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_GET_CMD:		  /* AT+TPCN? */
	{
		ret = DEV_GetModeSwitchOptReq(atHandle);
		break;
	}
	case TEL_EXT_SET_CMD:         /* AT+TPCN= */
	{
		int option;
		if ( getExtValue(parameter_values_p, 0, &option, TEL_AT_TPCN_0_OPTION_VAL_MIN, TEL_AT_TPCN_0_OPTION_VAL_MAX, TEL_AT_TPCN_0_OPTION_VAL_DEFAULT) == TRUE )
		{
			ret = DEV_SetModeSwitchOptReq(atHandle, (UINT8)option);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
		}
		break;
	}
	case TEL_EXT_ACTION_CMD:      /* AT*TPCN */
	case TEL_EXT_TEST_CMD:        /* AT*TPCN=?*/
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
#endif
/************************************************************************************
 * F@: ciTGCTRL - GLOBAL API for AT^HVER command
 *
 */
RETURNCODE_T  ciGetHVER(            const utlAtParameterOp_T op,
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
	UNUSEDPARAM(parameter_values_p);

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);


	/*
	 *  Put parser index into the variable
	 */
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_ACTION_CMD:		  /* AT^HVER */
	{
		char hardwareVerBuf[TEL_AT_HVER_MAX_HARDWARE_VERSION_LEN + ATCI_NULL_TERMINATOR_LENGTH];
		char buf[50];
		int getHVertRet;
		if ( (getHVertRet = get_hardwareVersion(hardwareVerBuf, sizeof(hardwareVerBuf))) > 0)
		{
			DBGMSG("%s: hardware version = %s\n", __FUNCTION__, hardwareVerBuf);
			sprintf(buf, "^HVER:\"%s\"\r\n", hardwareVerBuf);
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, buf);
		}
		else
		{
			DBGMSG("%s: Can not get hardware version, ret = %d\n", __FUNCTION__, getHVertRet);
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		}
		break;
	}
	case TEL_EXT_SET_CMD:		 /* AT^HVER= */
	case TEL_EXT_GET_CMD:		 /* AT^HVER? */
	case TEL_EXT_TEST_CMD:		 /* AT^HVER=?*/
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
 * F@: ciDCTS - GLOBAL API for AT^DCTS command
 *
 */
RETURNCODE_T  ciDCTS(            const utlAtParameterOp_T op,
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
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);


	/*
	 *  Put parser index into the variable
	 */
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT^DCTS=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"^DCTS: (0),(16,96)");
		break;
	}
	case TEL_EXT_SET_CMD:         /* AT^DCTS= */
	{
		int option = 0;
		int mode = 0;
		BOOL cmdValid = FALSE;

		if ( getExtValue(parameter_values_p, 0, &option, TEL_AT_DCTS_OPTION_MIN, TEL_AT_DCTS_OPTION_MAX, TEL_AT_DCTS_OPTION_DEFAULT) == TRUE )
		{
			if ( (getExtValue(parameter_values_p, 1, &mode, TEL_AT_DCTS_MODE_MIN, TEL_AT_DCTS_MODE_MAX, TEL_AT_DCTS_MODE_DEFAULT) == TRUE)
				&& ((mode == CI_DEV_NW_MONIOTR_NORMAL) || (mode == CI_DEV_NW_MONIOTR_DETECT)))
			{
				cmdValid = TRUE;
			}
		}

		if (cmdValid == TRUE)
		{
			ret = DEV_SetNetWorkMonitorOptReq(atHandle, (CiBoolean)option, (CiDevNwMonitorMode)mode);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
		}

		break;
	}
	case TEL_EXT_GET_CMD:         /* AT^DCTS? */
	{
		ret = DEV_GetNetWorkMonitorOptReq(atHandle);
		break;
	}
	case TEL_EXT_ACTION_CMD:      /* AT^DCTS */
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
 * F@: ciDCTS - GLOBAL API for AT^DEELS command
 *
 */
RETURNCODE_T  ciDEELS(            const utlAtParameterOp_T op,
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
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);


	/*
	 *  Put parser index into the variable
	 */
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT^DEELS=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"^DEELS: (0,1)");
		break;
	}
	case TEL_EXT_SET_CMD:         /* AT^DEELS= */
	{
		int option = 0;
		BOOL cmdValid = FALSE;

		if ( getExtValue(parameter_values_p, 0, &option, TEL_AT_DEELS_OPTION_MIN, TEL_AT_DEELS_OPTION_MAX, TEL_AT_DEELS_OPTION_DEFAULT) == TRUE )
		{
			cmdValid = TRUE;
		}

		if (cmdValid == TRUE)
		{
			ret = DEV_SetProtocolStatusConfigReq(atHandle, (CiBoolean)option);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
		}

		break;
	}
	case TEL_EXT_GET_CMD:         /* AT^DEELS? */
	{
		ret = DEV_GetProtocolStatusConfigReq(atHandle);
		break;
	}
	case TEL_EXT_ACTION_CMD:      /* AT^DEELS */
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
 * F@: ciDCTS - GLOBAL API for AT^DEVEI command
 *
 */
RETURNCODE_T  ciDEVEI(            const utlAtParameterOp_T op,
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
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);


	/*
	 *  Put parser index into the variable
	 */
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT^DEVEI=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"^DEVEI: (0,1)");
		break;
	}
	case TEL_EXT_SET_CMD:         /* AT^DEVEI= */
	{
		int option = 0;
		BOOL cmdValid = FALSE;

		if ( getExtValue(parameter_values_p, 0, &option, TEL_AT_DEVEI_OPTION_MIN, TEL_AT_DEVEI_OPTION_MAX, TEL_AT_DEVEI_OPTION_DEFAULT) == TRUE )
		{
			cmdValid = TRUE;
		}

		if (cmdValid == TRUE)
		{
			ret = DEV_SetEventIndConfigReq(atHandle, (CiBoolean)option);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
		}

		break;
	}
	case TEL_EXT_GET_CMD:         /* AT^DEVEI? */
	{
		ret = DEV_GetEventIndConfigReq(atHandle);
		break;
	}
	case TEL_EXT_ACTION_CMD:      /* AT^DEVEI */
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
 * F@: ciDCTS - GLOBAL API for AT^DNPR command
 *
 */
RETURNCODE_T  ciDNPR(            const utlAtParameterOp_T op,
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
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);


	/*
	 *  Put parser index into the variable
	 */
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT^DNPR=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"^DNPR: (0,1),(0,65535)");
		break;
	}
	case TEL_EXT_SET_CMD:         /* AT^DNPR= */
	{
		int option = 0;
		int interval = 0;
		BOOL cmdValid = FALSE;

		if ( getExtValue(parameter_values_p, 0, &option, TEL_AT_DNPR_OPTION_MIN, TEL_AT_DNPR_OPTION_MAX, TEL_AT_DNPR_OPTION_DEFAULT) == TRUE )
		{
			if ( getExtValue(parameter_values_p, 1, &interval, TEL_AT_DNPR_INTERVAL_MIN, TEL_AT_DNPR_INTERVAL_MAX, TEL_AT_DNPR_INTERVAL_DEFAULT) == TRUE )
			{
				cmdValid = TRUE;
			}
		}

		if (cmdValid == TRUE)
		{
			ret = DEV_SetWirelessParamConfigReq(atHandle, (CiBoolean)option, (UINT16)interval);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
		}

		break;
	}
	case TEL_EXT_GET_CMD:         /* AT^DNPR? */
	{
		ret = DEV_GetWirelessParamConfigReq(atHandle);
		break;
	}
	case TEL_EXT_ACTION_CMD:      /* AT^DNPR */
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
 * F@: ciDCTS - GLOBAL API for AT^DUSR command
 *
 */
RETURNCODE_T  ciDUSR(            const utlAtParameterOp_T op,
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
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);


	/*
	 *  Put parser index into the variable
	 */
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT^DUSR=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"^DUSR: (0,1),(0,1)");
		break;
	}
	case TEL_EXT_SET_CMD:         /* AT^DUSR= */
	{
		int option = 0;
		int mode = 0;
		BOOL cmdValid = FALSE;

		if ( getExtValue(parameter_values_p, 0, &option, TEL_AT_DUSR_OPTION_MIN, TEL_AT_DUSR_OPTION_MAX, TEL_AT_DUSR_OPTION_DEFAULT) == TRUE )
		{
			if ( getExtValue(parameter_values_p, 1, &mode, TEL_AT_DUSR_MODE_MIN, TEL_AT_DUSR_MODE_MAX, TEL_AT_DUSR_MODE_DEFAULT) == TRUE )
			{
				cmdValid = TRUE;
			}
		}

		if (cmdValid == TRUE)
		{
			ret = DEV_SetSignalingReportConfigReq(atHandle, (CiBoolean)option, (CiBoolean)mode);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
		}

		break;
	}
	case TEL_EXT_GET_CMD:         /* AT^DUSR? */
	{
		ret = DEV_GetSignalingReportConfigReq(atHandle);
		break;
	}
	case TEL_EXT_ACTION_CMD:      /* AT^DUSR */
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
