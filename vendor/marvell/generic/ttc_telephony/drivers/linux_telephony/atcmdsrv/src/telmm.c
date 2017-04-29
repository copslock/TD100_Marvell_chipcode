/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************
*  Filename: telmm.c
*
*  Description: Process Network related AT commands
*
*  History:
*   May 12, 2008 - Johnny He, Creation of file
*   Oct 14, 2008 - Qiang XU, Optimization
*
******************************************************************************/
#include "ci_api.h"
#include "telatci.h"
#include "tellinux.h"
#include "tel3gdef.h"
#include "telcontroller.h"
#include "teldef.h"
#include "telutl.h"
#include "telatparamdef.h"
#include "ci_mm.h"
#include "mm_api.h"

#include <sys/ioctl.h>
#include <fcntl.h>

/************************************************************************************
 *
 * MM related global variables and structure
 *
 *************************************************************************************/
/************************************************************************************
 *
 * MM related AT commands Processing Function
 *
 *************************************************************************************/
/* Added by Michal Bukai */
/************************************************************************************
 * F@: ciBANDIND - GLOBAL API for GCF AT*BANDIND command
 *
 */
RETURNCODE_T  ciBANDIND(            const utlAtParameterOp_T op,
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

	INT32 bandInd;
	BOOL bEnableBandInd;


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
	case TEL_EXT_TEST_CMD:         /* AT*BANDIND=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"*BANDIND: (0-1)\r\n");
		break;
	}

	case TEL_EXT_GET_CMD:         /* AT*BANDIND? */
	{
		ret = MM_GetBANDIND(atHandle);
		break;
	}

	case TEL_EXT_SET_CMD:             /* AT*BANDIND= */
	{
		if ( getExtValue(parameter_values_p, 0, &bandInd, TEL_AT_BANDIND_MODE_VAL_MIN, TEL_AT_BANDIND_MODE_VAL_MAX, TEL_AT_BANDIND_MODE_VAL_DEFAULT) == TRUE )
		{
			bEnableBandInd = (bandInd == 0) ? FALSE : TRUE;
			ret = MM_SetBANDIND(atHandle, bEnableBandInd);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
		}
		break;

	}
	case TEL_EXT_ACTION_CMD:         /*AT*BANDIND */
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
 * F@: ciCPLS - GLOBAL API for GCF AT+CPLS command
 *
 */
RETURNCODE_T  ciCPLS(            const utlAtParameterOp_T op,
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
	INT32 cplsList;


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
	case TEL_EXT_TEST_CMD: /* AT+CPLS=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CPLS: (0-2)\r\nOK\r\n");
		break;
	}

	case TEL_EXT_GET_CMD: /* AT+CPLS? */
	{
		ret = MM_GetCPLS(atHandle);
		break;
	}

	case TEL_EXT_SET_CMD:     /* AT+CPLS= */
	{
		if ( getExtValue(parameter_values_p, 0, &cplsList, TEL_AT_CPLS_0_LIST_VAL_MIN, TEL_AT_CPLS_0_LIST_VAL_MAX, TEL_AT_CPLS_0_LIST_VAL_DEFAULT) == TRUE )
		{
			ret = MM_SelectCPLS(atHandle, (UINT8)cplsList);
		}
		else
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);

		break;

	}
	case TEL_EXT_ACTION_CMD: /*AT+CPLS */
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
/*#if 0*/
/* Added by Michal Bukai */
/************************************************************************************
 * F@: ciCNUM - GLOBAL API for GCF AT+CNUM command
 *
 */
RETURNCODE_T  ciCNUM(            const utlAtParameterOp_T op,
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

	/*
	 *  Put parser index into the variable
	 */
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);
	*xid_p = atHandle;

	DBGMSG("%s: reqHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT+CNUM=? */
	{
		ret = ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"OK\r\n");
		break;
	}

	case TEL_EXT_ACTION_CMD:           /* AT+CNUM */
	{
		ret = MM_GetNumberOfEntries(atHandle);
		break;
	}

	case TEL_EXT_GET_CMD:                   /* AT+CNUM? */
	case TEL_EXT_SET_CMD:                   /* AT+CNUM= */
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
/*#endif*/

/************************************************************************************
 * F@: ciRegStatus - GLOBAL API for GCF AT+CREG  -command
 *
 */
RETURNCODE_T  ciRegStatus(            const utlAtParameterOp_T op,
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
	**  Check the operation type.
	*/
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:              /* AT+CREG=? */
	{
		ret = MM_QueryRegOption(atHandle);
		break;
	}

	case TEL_EXT_GET_CMD:              /* AT+CREG? */
	{
		ret = MM_GetRegStatus(atHandle);
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CREG= */
	{
		INT32 regParam;
		if ( getExtValue( parameter_values_p, 0, &regParam, CIMM_REGRESULT_DISABLE, CIMM_REGRESULT_CELLINFO, CIMM_REGRESULT_DISABLE ) == TRUE )
		{
			if (( regParam == CIMM_REGRESULT_DISABLE ) ||                                   /* Disable reporting */
			    ( regParam == CIMM_REGRESULT_STATUS ) ||                                    /* Report Registration Status only */
			    ( regParam == CIMM_REGRESULT_CELLINFO ))                                    /* Report Status + Current Cell Info */
			{
				ret = MM_SetRegOption(atHandle, regParam);
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

	case TEL_EXT_ACTION_CMD:              /* AT+CREG */
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
 * F@: ciOperSel - GLOBAL API for GCF AT+COPS -command
 *
 */
RETURNCODE_T  ciOperSel(            const utlAtParameterOp_T op,
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
	**  Determine the type of request.
	*/
	switch ( op )
	{
	/* TS 27.007 - command shall return a list of quadruplets, each representing...
	 * ... an operator present in the network. The list of operators shall be in order: ...
	 * ... home network, networks referenced in SIM/UICC, and other networks. */
	case TEL_EXT_TEST_CMD:         /* AT+COPS=? */
	{
		ret = MM_QuerySuppOperator(atHandle);
		break;
	}

	case TEL_EXT_GET_CMD:         /* AT+COPS? */
	{
		ret = MM_GetCurrOperator(atHandle);
		break;
	}

	/* TS 27.007 - Set command forces an attempt to select and register the GSM/UMTS network operator.
	 * The selected mode affects to all further network registration (e.g. after <mode>=2, MT shall be
	 * unregistered until <mode>=0 or 1 is selected). */
	case TEL_EXT_SET_CMD:          /* AT+COPS= [<mode>[,<format>[,<oper>[,< AcT>]]]] */
	{
		INT32 modeVal, formatVal, accTchVal;
		CHAR networkOperator[TEL_AT_COPS_2_OPER_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH] = { 0 };
		INT16 length;

		/* Get operator mode. */
		if ( getExtValue(parameter_values_p, 0, &modeVal, TEL_AT_COPS_0_MODE_VAL_MIN, TEL_AT_COPS_0_MODE_VAL_MAX, TEL_AT_COPS_0_MODE_VAL_DEFAULT) == TRUE )
		{
			/* Determine the operator format. */
			if ( getExtValue(parameter_values_p, 1, &formatVal, TEL_AT_COPS_1_FORMAT_VAL_MIN, TEL_AT_COPS_1_FORMAT_VAL_MAX, TEL_AT_COPS_1_FORMAT_VAL_DEFAULT) == TRUE )
			{
				/* Get the operator details. */
				if ( getExtString(parameter_values_p, 2, networkOperator, TEL_AT_COPS_2_OPER_STR_MAX_LEN, &length, TEL_AT_COPS_2_OPER_STR_DEFAULT) == TRUE )
				{
					/* default access tech is GSM according to 27.007 */
					if ( getExtValue(parameter_values_p, 3, &accTchVal, TEL_AT_COPS_3_ACT_VAL_MIN, TEL_AT_COPS_3_ACT_VAL_MAX, TEL_AT_COPS_3_ACT_VAL_DEFAULT ) == TRUE)
					{
						ret = MM_SetCurrOperator(atHandle, modeVal, formatVal, (char *)networkOperator, length, accTchVal);
					}
				}
			}
		}
		else
		{
			/* invalid mode */
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
 * F@: ciPrefOper - GLOBAL API for GCF AT+CPOL -command
 *
 */
RETURNCODE_T  ciPrefOper(            const utlAtParameterOp_T op,
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
	**  Determine the type of request.
	*/
	switch ( op )
	{
	case TEL_EXT_GET_CMD:         /* AT+CPOL? */
	{
		ret = MM_GetPrefOperator(atHandle);
		break;
	}
	case TEL_EXT_SET_CMD:      /* AT+CPOL=[<index>][, <format>[,<oper>]] */
	{
		INT32 indexVal, formatVal, GsmACTVal, GsmCompactACTVal, UtranACTVal, ActVal;
		CHAR networkOperator[TEL_AT_CPOL_2_OPER_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH] = { 0 };
		INT16 length;
		if ( getExtValue(parameter_values_p, 0, &indexVal, TEL_AT_CPOL_0_INDEX_VAL_MIN,
				 TEL_AT_CPOL_0_INDEX_VAL_MAX, TEL_AT_CPOL_0_INDEX_VAL_DEFAULT) == TRUE )
		{
			if ( getExtValue(parameter_values_p, 1, &formatVal, TEL_AT_CPOL_1_FORMAT_VAL_MIN,
					 TEL_AT_CPOL_1_FORMAT_VAL_MAX, TEL_AT_CPOL_1_FORMAT_VAL_DEFAULT) == TRUE )
			{
				if ( getExtString(parameter_values_p, 2, networkOperator,
						  TEL_AT_CPOL_2_OPER_STR_MAX_LEN, &length, TEL_AT_CPOL_2_OPER_STR_DEFAULT) == TRUE )
					if( getExtValue(parameter_values_p, 3, &GsmACTVal, TEL_AT_CPOL_3_GSM_ACT_VAL_MIN,
									TEL_AT_CPOL_3_GSM_ACT_VAL_MAX, TEL_AT_CPOL_3_GSM_ACT_VAL_DEFAULT) == TRUE )
					{
						if( getExtValue(parameter_values_p, 4, &GsmCompactACTVal, TEL_AT_CPOL_4_GSM_COMPACT_ACT_VAL_MIN,
										TEL_AT_CPOL_4_GSM_COMPACT_ACT_VAL_MAX, TEL_AT_CPOL_4_GSM_COMPACT_ACT_VAL_DEFAULT) == TRUE )
						{
							if( getExtValue(parameter_values_p, 5, &UtranACTVal, TEL_AT_CPOL_5_UTRAN_ACT_VAL_MIN,
											TEL_AT_CPOL_5_UTRAN_ACT_VAL_MAX, TEL_AT_CPOL_5_UTRAN_ACT_VAL_DEFAULT) == TRUE )
							{
								if((GsmCompactACTVal == 0) && (UtranACTVal == 0))
								{
									ActVal = ATCI_ACT_GSM;
								}
								else if((GsmACTVal == 0) && (GsmCompactACTVal == 1) && (UtranACTVal == 0))
								{
									ActVal = ATCI_ACT_GSM_COMPACT;
								}
								else if((GsmACTVal == 0) && (GsmCompactACTVal == 0) && (UtranACTVal == 1))
								{
									ActVal = ATCI_ACT_UTRAN;
								}
								ret = MM_SetPrefOperator(atHandle, indexVal, formatVal, ActVal, (char *)networkOperator, length);

							}
						}
					}
			}
		}
		break;
	}

	case TEL_EXT_TEST_CMD:     /* AT+CPOL=? */
	{
		/* Test cmd not supported now */
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		break;
	}

	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return rc;
}

/************************************************************************************
 * F@: ciSignalQuality - GLOBAL API for GCF AT+CSQ -command
 *
 */
RETURNCODE_T  ciSignalQuality(            const utlAtParameterOp_T op,
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
	**  Determine the type of request.
	*/
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT+CSQ=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, "+CSQ:(0-31),(0-7)");
		break;
	}
	case TEL_EXT_ACTION_CMD:          /* AT+CSQ */
	{
		int rssi, ber;
		char atRspBuf[100];
		ret = MM_GetSignalQuality(atHandle, &rssi, &ber);
		sprintf(atRspBuf, "+CSQ:%d,%d", rssi, ber);
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, atRspBuf);
		break;
	}
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}

/************************************************************************************
 * F@: ciGetSubscriber - GLOBAL API for GCF AT+CNUM command
 *
 */
RETURNCODE_T  ciGetSubscriber(     const utlAtParameterOp_T op,
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
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT+CNUM=? */
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	case TEL_EXT_ACTION_CMD:         /* AT+CNUM */
	{
		ret = MM_GetSubscriber(atHandle);
		break;
	}

	default:         /* AT+CNUM?, AT+CNUM= */
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}

/************************************************************************************
 * F@: ciCTZR - GLOBAL API for GCF AT+CTZR command
 *
 */

RETURNCODE_T  ciCTZR(            const utlAtParameterOp_T op,
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
	case TEL_EXT_GET_CMD:          /* AT+CTZR? */
	{
		ret = MM_GetTimeZoneReportOption(atHandle);
		break;
	}

	/*	case TEL_EXT_SET_CMD: //AT+CTZR=
			{

				// enables and disables the time zone change event reporting,  seems not supported by CP. Can't find corresponding CI msg

				if (getExtValue( parameter_values_p, 0, &colpFlag, TEL_AT_CTZR_0_N_VAL_MIN, TEL_AT_CTZR_0_N_VAL_MAX, TEL_AT_CTZR_0_N_VAL_DEFAULT ) == TRUE)
				{
					ctzrOpReq.Local = ctzrFlag;
					ret = ciRequest( gAtciSvgHandle[CI_SG_ID_MM], CI_SS_PRIM_SET_CTZR_OPTION_REQ,
							reqHandle, (void *)&ctzrOpReq );
				}
				else
				{
					ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_OPERATION_NOT_SUPPORTED,NULL);
				}

				break;
			}
	 */
	case TEL_EXT_ACTION_CMD:         /* AT+CTZR */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}
	break;
	}


	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 * F@: ciCTZU - GLOBAL API for GCF AT+CTZU command
 *
 */

RETURNCODE_T  ciCTZU(            const utlAtParameterOp_T op,
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
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch (op){
	case TEL_EXT_GET_CMD:          /* AT+CTZU? */
	{
		ret = ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CTZU: 1");
		break;
	}
	case TEL_EXT_ACTION_CMD:         /* AT+CTZU */
	default:
	{
		ret = ATRESP(atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}
	break;
	}


	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 * F@: ciConfigNwModeInd - GLOBAL API for GCF AT+CIND command
 *
 */

RETURNCODE_T  ciNwModeInd(            const utlAtParameterOp_T op,
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
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch ( op)
	{
	case TEL_EXT_TEST_CMD:          /* AT+CIND=? */
	{
		/* display "+CIND: list of supported modes" */
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CIND: (\"service\",(0-1))");
		break;
	}

	case TEL_EXT_SET_CMD:             /* AT+CIND=  */
	{
		INT32 nwIndMode;
		if ( getExtValue(parameter_values_p, 0, &nwIndMode, 0, 1, 0) == TRUE )
		{
			ret = MM_SetNwModeIndSetting(atHandle, nwIndMode);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_ALLOWED, NULL);
		}
		break;
	}

	case TEL_EXT_GET_CMD:             /* AT+CIND?  */
	{
		ret = MM_GetNwModeIndSetting(atHandle);
		break;
	}
	default:
		break;
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

RETURNCODE_T  ciCellLock(            const utlAtParameterOp_T op,
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
	INT32                       mode, /*act, band,*/ arfcn, scramblingCode;
	BOOL						cmdValid = FALSE;
	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);
	switch (op){
	case TEL_EXT_SET_CMD:	/*AT*CellLock= */
	{
		if ( getExtValue(parameter_values_p, 0, &mode, TEL_AT_CELLLOCK_0_MODE_VAL_MIN, TEL_AT_CELLLOCK_0_MODE_VAL_MAX, TEL_AT_CELLLOCK_0_MODE_VAL_DEFAULT) == TRUE )
		{
			if(mode == CIMM_CELL_LOCK_MODE_NONE)
				cmdValid = TRUE;
			if ( getExtValue(parameter_values_p, 1, &arfcn, TEL_AT_CELLLOCK_3_ARFCN_VAL_MIN, TEL_AT_CELLLOCK_3_ARFCN_VAL_MAX, TEL_AT_CELLLOCK_3_ARFCN_VAL_DEFAULT) == TRUE )
			{
				if(mode == CIMM_CELL_LOCK_MODE_LOCKFREQ)
					cmdValid = TRUE;
				if ( getExtValue(parameter_values_p, 2, &scramblingCode, TEL_AT_CELLLOCK_4_SC_VAL_MIN, TEL_AT_CELLLOCK_4_SC_VAL_MAX, TEL_AT_CELLLOCK_4_SC_VAL_DEFAULT) == TRUE )
				{
					cmdValid = TRUE;
				#if 0
					if(act == 0)
					{
						if(band <= CI_GSM_850_BAND || band == 0xFE /*CI_UMTS_BAND*/)
						{
							cmdValid = TRUE;
						}
					}
					else if (act == 2)
					{
						if(band <= CI_UMTS_BAND_9 ||band == 9 /*CI_BAND_GSM*/)
						{
							cmdValid = TRUE;
						}
					}
				#endif

				}
			}
			if(mode == CIMM_CELL_LOCK_MODE_NONE)
			{
				arfcn = 0xFFFF;
				scramblingCode = 0xFF;
			}else if(mode == CIMM_CELL_LOCK_MODE_LOCKFREQ)
			{
				scramblingCode = 0xFF;
			}
			if(cmdValid)
			{
				ret = MM_SetCellLOCK(atHandle, mode, arfcn, scramblingCode);
			}
		}
		if(!cmdValid)
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL );
		}
		break;
	}
	case TEL_EXT_TEST_CMD:		/*AT*CellLock=? */
	{
			ret = ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"*CellLock: <mode>,<arfcn>,<scrambling_code>");
			break;
	}
	case TEL_EXT_GET_CMD:          /* AT*CellLock? */
	case TEL_EXT_ACTION_CMD:         /* AT*CellLock */
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
 * F@: cistarCTZR - GLOBAL API for GCF AT*CTZR command
 *
 */

RETURNCODE_T  cistarCTZR(            const utlAtParameterOp_T op,
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
	case TEL_EXT_GET_CMD:          /* AT*CTZR? */
	{
		ret = MM_GetstarTimeZoneReportOption(atHandle);
		break;
	}
	case TEL_EXT_ACTION_CMD:         /* AT*CTZR */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}
	break;
	}


	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 * F@: ciRegStatus - GLOBAL API for AT*REJCAUSE  -command
 *
 */
RETURNCODE_T  cistarREJCAUSE(            const utlAtParameterOp_T op,
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
	**  Check the operation type.
	*/
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:              /* AT*REJCAUSE=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"*REJCAUSE: (0-2))");
		break;
	}

	case TEL_EXT_GET_CMD:              /* AT*REJCAUSE? */
	{
		ret = MM_GetRejectCauseStatus(atHandle);
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT*REJCAUSE= */
	{
		INT32 rejParam;
		if ( getExtValue( parameter_values_p, 0, &rejParam, 0, 2, 0 ) == TRUE )
		{
			if(rejParam == 2)
				ret = MM_GetRejectCauseHistory(atHandle);
			else
				ret = MM_SetRejectCauseStatus(atHandle, rejParam);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		}

		break;
	}

	case TEL_EXT_ACTION_CMD:              /* AT*REJCAUSE */
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
 * F@: ciSysInfo - GLOBAL API for CMCC AT^SYSINFO command
 *
 */
RETURNCODE_T  ciSysInfo(            const utlAtParameterOp_T        op,
		const char                      *command_name_p,
		const utlAtParameterValue_P2c   parameter_values_p,
		const size_t                    num_parameters,
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
	**  Check the operation type.
	*/

	switch(op)
	{
	case TEL_EXT_ACTION_CMD:   /* AT^SYSINFO */
	{
		ret = MM_GetSysInfo(atHandle);
		break;
	}
	case TEL_EXT_TEST_CMD:    /* AT^SYSINFO=? */
	case TEL_EXT_SET_CMD:      /* AT^SYSINFO= */
	case TEL_EXT_GET_CMD:       /* AT^SYSINFO?  */
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


