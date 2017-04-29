/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************
*  Filename: telmsg.c
*
*  Description: Process SMS related AT commands
*
*  History:
*   Jan 18, 2008 - Creation of file
*   Sept 12, 2008 - Optimization
*
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
#include "msg_api.h"

/************************************************************************************
 *
 * SMS related global variables and structure
 *
 *************************************************************************************/
/* Maintain value of AT+CMGF, and used in
 *       1) request msg for +CMGR, +CMGL, +CMGW, +CMGS, send sms text,
 *       2) confirm msg for +CMGR and +CMGL
 */
AtciSmsFormatMode gSmsFormatMode = ATCI_SMS_FORMAT_PDU_MODE;

extern TelAtpCtrl gAtpCtrl[];

typedef struct _msgStatCiMapType
{
	char              *statName;
	CiMsgMsgStatus ciMsgStat;
}  msgStatCiMapType;

/* Save AT handle of +CMGS and +CMGW */
static UINT32 gSendMsgReqHandle;

/************************************************************************************
 *
 * Lib Function used to Convert SMS data
 *
 *************************************************************************************/

/****************************************************************
 *  F@: convert SMS status string to enum status type
 *
 */
static BOOL libConvertStringToCiMsgStatType( CHAR *statStr, CiMsgMsgStatus *msgStatType )
{
	BOOL ret = FALSE;
	INT32 i = 0;

	#define NUM_OF_MSG_STAT 5

	msgStatCiMapType msgStatCnvTable[] =
	{
		{ "REC READ",	CI_MSG_MSG_STAT_READ	    },
		{ "REC UNREAD", CI_MSG_MSG_STAT_REC_UNREAD  },
		{ "STO SENT",	CI_MSG_MSG_STAT_STO_SENT    },
		{ "STO UNSENT", CI_MSG_MSG_STAT_STO_UNSENT  },
		{ "ALL",	CI_MSG_MSG_STAT_ALL	    }
	};

	while ( (ret == FALSE) && (i < NUM_OF_MSG_STAT ) )
	{
		if ( strcmp( (char *)msgStatCnvTable[i].statName, (char *)statStr ) == 0 )
		{
			*msgStatType = msgStatCnvTable[i].ciMsgStat;
			ret = TRUE;
		}
		i++;
	}

	return ret;
}


/****************************************************************
 *  F@: convert SMS status digit to enum status type
 *
 */
static BOOL libConvertIntegerToCiMsgStatType( INT32 statInt, CiMsgMsgStatus *msgStatType )
{
	BOOL ret = TRUE;

	DBGMSG("%s statInt = %d.\n", __FUNCTION__, statInt);

	switch ( statInt )
	{
	case 0:
		*msgStatType = CI_MSG_MSG_STAT_REC_UNREAD;
		break;
	case 1:
		*msgStatType = CI_MSG_MSG_STAT_READ;
		break;
	case 2:
		*msgStatType = CI_MSG_MSG_STAT_STO_UNSENT;
		break;
	case 3:
		*msgStatType = CI_MSG_MSG_STAT_STO_SENT;
		break;
	case 4:
		*msgStatType = CI_MSG_MSG_STAT_ALL;
		break;
	default:
		ret = FALSE;
		break;
	}
	return ret;
}

/************************************************************************************
 * F@: libGetSmsAddressInfo - convert GSM 7 bit address in CI API format, extract AT para like ...<da>[,<toda>]...
 *
 */
static BOOL  libGetSmsAddrInfo ( const utlAtParameterValue_P2c parameter_values_p, int index, CiAddressInfo *pSmsAddr )
{
	INT16 strIdx = 0,  addrLen = 0;
	int addrType = 0;
	CHAR addrStr[ TEL_SMS_MAX_ADDR_LEN + ATCI_NULL_TERMINATOR_LENGTH];

	/* init length */
	pSmsAddr->Length = 0;

	/* Extract the message destination address */
	if (( getExtString( parameter_values_p, index++, addrStr, TEL_SMS_MAX_ADDR_LEN, &addrLen, NULL ) == TRUE ))
	{
		/* if a new destination address is specified -> must convert it and send it */
		if ( addrLen != 0 )
		{
			/* currently SAC expects the address in 'IRA digits string' only */
			/* SAC will convert it to unpacket BCD format for the protocol stack */

			/* NPI: Numbering Plan Identification */
			pSmsAddr->AddrType.NumPlan = CI_NUMPLAN_E164_E163;

			/* TON: Type of Number */
			if ( addrStr[strIdx] == '+' )
			{
				pSmsAddr->AddrType.NumType = ATCI_DIAL_NUMBER_INTERNATIONAL;
				strIdx++;
			}
			else
			{
				pSmsAddr->AddrType.NumType = ATCI_DIAL_NUMBER_UNKNOWN;
				/* Check optional AT para for address type */
				if ( getExtValue(parameter_values_p, index, &addrType, 0, TEL_AT_SMS_TEXTMODE_ADDRTYPE_MAX, ATCI_DIAL_NUMBER_UNKNOWN) == TRUE )
				{
					pSmsAddr->AddrType.NumType = addrType;
				}
			}

			/* copy the digits */
			memcpy( pSmsAddr->Digits, addrStr + strIdx, (addrLen - strIdx) < CI_MAX_ADDRESS_LENGTH ? (addrLen - strIdx) : CI_MAX_ADDRESS_LENGTH );

			/* save final address length */
			pSmsAddr->Length = ((addrLen - strIdx) < CI_MAX_ADDRESS_LENGTH ? (addrLen - strIdx) : CI_MAX_ADDRESS_LENGTH);
		}
		return TRUE;
	}

	return FALSE;
}

/************************************************************************************
 * F@: ConvertDelFlag2Ci - Convert SMS delete flag from AT to CCI enum
 *
 */
static UINT8 libConvertDelFlag2Ci(UINT8 AtFlag)
{
	UINT8 CiFlag;

	switch (AtFlag)
	{
	case ATCI_SMS_DEL_INDEX:
		CiFlag = CI_MSG_MSG_DELFLAG_INDEX;
		break;

	case ATCI_SMS_DEL_RECV_READ:
		CiFlag = CI_MSG_MSG_DELFLAG_ALL_READ;
		break;

	case ATCI_SMS_DEL_RECV_READ_SENT:
		CiFlag = CI_MSG_MSG_DELFLAG_ALL_READ_OR_SENT;
		break;

	case ATCI_SMS_DEL_RECV_READ_MO:
		CiFlag = CI_MSG_MSG_DELFLAG_ALL_READ_OR_MO;
		break;

	case ATCI_SMS_DEL_ALL:
		/* CI does not have equivalent flag value... */
		CiFlag = CI_MSG_MSG_DELFLAG_INDEX;
		break;

	default:
		CiFlag = CI_MSG_MSG_DELFLAG_INDEX;
		break;
	}

	return CiFlag;
}

/************************************************************************************
 *
 * SMS related AT commands Processing Function
 *
 *************************************************************************************/

/************************************************************************************
 * F@: ciSelectSmsService - GLOBAL API for GCF AT+CSMS command
 *
 *      Select Message Service
 */
RETURNCODE_T  ciSelectSmsSrv( const utlAtParameterOp_T op,
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
	UINT32 service;

	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT+CSMS=? */
	{
		ret = MSG_QuerySmsService(atHandle);
		break;
	}

	case TEL_EXT_GET_CMD:         /* AT+CSMS? */
	{
		ret = MSG_GetSmsService(atHandle);
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+CSMS= */
	{
		if ( getExtValue( parameter_values_p, 0, (int *)&service, TEL_AT_CSMS_0_SERVICE_VAL_MIN, TEL_AT_CSMS_0_SERVICE_VAL_MAX, TEL_AT_CSMS_0_SERVICE_VAL_DEFAULT ) == TRUE )
		{
			ret = MSG_SetSmsService(atHandle, service);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
		}
		break;
	}

	default:         /* AT+CSMS */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}


/************************************************************************************
 * F@: ciPrefStorage - GLOBAL API for GCF AT+CPMS command
 *
 *      Preferred Message Storage
 */
RETURNCODE_T  ciPrefStorage( const utlAtParameterOp_T op,
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
	BOOL cmdValid = FALSE;
	CHAR mem1Str[ATCI_MSG_STORAGE_STR_LENGTH + ATCI_NULL_TERMINATOR_LENGTH],
	     mem2Str[ATCI_MSG_STORAGE_STR_LENGTH + ATCI_NULL_TERMINATOR_LENGTH],
	     mem3Str[ATCI_MSG_STORAGE_STR_LENGTH + ATCI_NULL_TERMINATOR_LENGTH];
	INT16 mem1StrLen, mem2StrLen, mem3StrLen;

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT+CPMS=? */
	{
		ret = MSG_QuerySupportStorage(atHandle);
		break;
	}

	case TEL_EXT_GET_CMD:         /* AT+CPMS? */
	{
		ret = MSG_GetPrefStorage(atHandle);
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+CPMS= */
	{
		if(getExtString(parameter_values_p, 0, mem1Str, ATCI_MSG_STORAGE_STR_LENGTH, &mem1StrLen, NULL) == TRUE)
		{
			if(getExtString(parameter_values_p, 1, mem2Str, ATCI_MSG_STORAGE_STR_LENGTH, &mem2StrLen, NULL) == TRUE)
			{
				if(getExtString(parameter_values_p, 2, mem3Str, ATCI_MSG_STORAGE_STR_LENGTH, &mem3StrLen, NULL) == TRUE)
				{
					ret = MSG_SetPrefStorage(atHandle, mem1Str, mem2Str, mem3Str, mem1StrLen, mem2StrLen, mem3StrLen);
					cmdValid = TRUE;
				}
			}
		}
		if(!cmdValid)
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
		}
		break;
	}

	default:         /* AT+CPMS */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}

/************************************************************************************
 * F@: ciMsgFormat - GLOBAL API for GCF AT+CMGF command
 *
 *      Select SMS message format
 *	 This value is maintained in global variable gSmsFormatMode and not sent to CI
 */
RETURNCODE_T  ciMsgFormat( const utlAtParameterOp_T op,
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

	INT32 smsFormat;
	CHAR AtRspBuf [300];

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	**  handle AT operation
	*/
	switch ( op)
	{
	case TEL_EXT_SET_CMD:             /* AT+CMGF=  */
	{
		/*
		**  Determine the extended parameter.
		*/
		if ( getExtValue(parameter_values_p, 0, &smsFormat, TEL_AT_CMGF_0_MODE_VAL_MIN, TEL_AT_CMGF_0_MODE_VAL_MAX, TEL_AT_CMGF_0_MODE_VAL_DEFAULT) == TRUE )
		{
			if ( smsFormat < ATCI_SMS_FORMAT_NUM_MODE)
			{
				/* nothing to be sent to CI; maintain this info in the ATCI
				 * as it's required for further processing of SMS commands.
				 */
				gSmsFormatMode = smsFormat;
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
			}
			else
			{
				/* invalid value */
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
			}
		}                        /* if ( getExtendedParamete */
		else
		{
			/* invalid value */
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
		}
		break;
	}

	case TEL_EXT_TEST_CMD:          /* AT+CMGF=? */
	{
		/* display "+CMGF: list of supported modes" */
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CMGF: (0,1)");
		break;
	}

	case TEL_EXT_GET_CMD:            /* AT+CMGF?  */
	{
		/* display "+CMGF: current mode" */
		sprintf((char *)AtRspBuf, "+CMGF: %d", gSmsFormatMode);
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf);
		break;
	}

	case TEL_EXT_ACTION_CMD:           /* AT+CMGF  */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}

/************************************************************************************
 * F@: ciCSCA - GLOBAL API for GCF AT+CSCA command
 *
 *	  Service Center Address
 */
RETURNCODE_T  ciSrvCenterAddr( const utlAtParameterOp_T op,
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

	BOOL cmdValid = FALSE;
	CHAR addressStr[ CI_MAX_ADDRESS_LENGTH + ATCI_NULL_TERMINATOR_LENGTH ];
	UINT16 addrStrLen;
	UINT32 type;

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT+CSCA=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	case TEL_EXT_GET_CMD:         /* AT+CSCA? */
	{
		ret = MSG_GetSrvCenterAddr(atHandle);
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+CSCA= */
	{
		if ( getExtString(parameter_values_p, 0, addressStr, CI_MAX_ADDRESS_LENGTH, (INT16 *)&addrStrLen, NULL) == TRUE )
		{
			if ( addrStrLen > 0 )
			{
				if ( getExtValue( parameter_values_p, 1, (int *)&type, 0, 255, ATCI_DIAL_NUMBER_UNKNOWN ) == TRUE )
				{
					ret = MSG_SetSrvCenterAddr(atHandle, addressStr, addrStrLen, type);
					cmdValid = TRUE;
				}
			}
		}

		if ( !cmdValid )
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
		}
		break;
	}

	default:         /* AT+CSCA */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}

/************************************************************************************
 * F@: ciTextParam - GLOBAL API for GCF AT+CSMP -command
 *
 *	Set Text Mode Parameters
 *	This value is maintained in global variable gAtSmsMessage and not sent to CI
 */
RETURNCODE_T  ciSetTextModeParam( const utlAtParameterOp_T op,
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

	BOOL cmdValid = FALSE;
	UINT32 newFO, newVP, newPID, newDCS;
	CHAR AtRspBuf [300];

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT+CSMP=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	case TEL_EXT_GET_CMD:         /* AT+CSMP? */
	{
		UINT8 fo, vp, pid, dcs;
		ret = MSG_GetMsgTextMode(atHandle, &fo, &vp, &pid, &dcs);
		sprintf( (char *)AtRspBuf, "+CSMP:%d,%d,%d,%d", fo, vp, pid, dcs );
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf );
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+CSMP= */
	{
		if ( getExtValue( parameter_values_p, 0, (int *)&newFO, TEL_AT_CSMP_0_FO_VAL_MIN, TEL_AT_CSMP_0_FO_VAL_MAX, TEL_AT_CSMP_0_FO_VAL_DEFAULT ) == TRUE )
		{
			if ( getExtValue( parameter_values_p, 1, (int *)&newVP, TEL_AT_CSMP_1_VP_VAL_MIN, TEL_AT_CSMP_1_VP_VAL_MAX, TEL_AT_CSMP_1_VP_VAL_DEFAULT ) == TRUE )
			{
				if ( getExtValue( parameter_values_p, 2, (int *)&newPID, TEL_AT_CSMP_2_PID_VAL_MIN, TEL_AT_CSMP_2_PID_VAL_MAX, TEL_AT_CSMP_2_PID_VAL_DEFAULT ) == TRUE )
				{
					if ( getExtValue( parameter_values_p, 3, (int *)&newDCS, TEL_AT_CSMP_3_DCS_VAL_MIN, TEL_AT_CSMP_3_DCS_VAL_MAX, TEL_AT_CSMP_3_DCS_VAL_DEFAULT ) == TRUE )
					{
						cmdValid = TRUE;

						/*
						 * Test for error conditions (as indicated in Voyager doc).
						 * NOTE: 27.005 does not require to check for this...
						 */

						if ( (newFO & ATCI_TP_MTI_MASK) != ATCI_SMS_SUBMIT_MTI )
						{
							/* Message Type Indicator must be SMS-SUBMIT */
							ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_INVALID_TEXT_MODE_PARA, NULL );
							break;
						}

						if ( ((newFO & ATCI_SMS_TP_VPF) != 0x10) && ((newVP & ATCI_SMS_TP_VPF) != 0x00) )
						{
							/* We support only relative Validity Period or no VP at all */
							ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_INVALID_TEXT_MODE_PARA, NULL );
							break;
						}

						/* NOTE: This command is supposed to write Validity Period to NVRAM */
						/* CCI DOES NOT SUPPORT THIS!!!!!!!                                 */

						ret = MSG_SetMsgTextMode(atHandle, newFO, newVP, newPID, newDCS);

						ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
					}
				}
			}
		}

		if ( !cmdValid )
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
		}
		break;
	}

	default:         /* AT+CSMP */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 * F@: ciMsCbCSCB - GLOBAL API for GCF AT+CSCB command
 *
 *	  Select Cell Broadcast Message Types
 */
RETURNCODE_T  ciSelectCbMsgType( const utlAtParameterOp_T op,
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
	BOOL cmdValid = FALSE;

	INT32 mode;
	CHAR mids[ATCI_MAX_CMDSTR_LENGTH + ATCI_NULL_TERMINATOR_LENGTH];
	CHAR dcss[ATCI_MAX_CMDSTR_LENGTH + ATCI_NULL_TERMINATOR_LENGTH];
	UINT16 txtLen;

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT+CSCB=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CSCB: (0,1)" );
		break;
	}

	case TEL_EXT_GET_CMD:         /* AT+CSCB? */
	{
		ret = MSG_GetCbMsgType(atHandle);
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+CSCB= */
	{
		/* NOTE that documentation is not clear as what mode is default */
		if ( getExtValue( parameter_values_p, 0, &mode, TEL_AT_CSCB_0_MODE_VAL_MIN, TEL_AT_CSCB_0_MODE_VAL_MAX, TEL_AT_CSCB_0_MODE_VAL_DEFAULT ) == TRUE )
		{
			if ( getExtString(parameter_values_p, 1, mids,
					  ATCI_MAX_CMDSTR_LENGTH, (INT16 *)&txtLen, NULL) == TRUE )
			{
				if ( txtLen == 0 )
				{
					mids[0] = '\0';
				}
				if ( getExtString(parameter_values_p, 2, dcss,
						  ATCI_MAX_CMDSTR_LENGTH, (INT16 *)&txtLen, NULL) == TRUE )
				{
					if ( txtLen == 0 )
					{
						dcss[0] = '\0';
					}

					ret = MSG_SetCbMsgType(atHandle, (BOOL)mode, mids, dcss);

					cmdValid = (ret == CIRC_SUCCESS);
				}
			}
		}

		if ( !cmdValid )
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_INVALID_PDU_MODE_PARA, NULL );
		}

		break;
	}

	default:         /* AT+CSCB */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_INVALID_PDU_MODE_PARA, NULL );
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}


/************************************************************************************
 * F@: ciPfCNMI - GLOBAL API for GCF AT+CNMI command
 *
 *	  New Message Indications to TE
 */
RETURNCODE_T  ciNewMsgInd( const utlAtParameterOp_T op,
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
	BOOL cmdValid = FALSE;

	UINT32 mode, mt, bm, ds, bfr;
	CHAR AtRspBuf[300];

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT+CNMI=? */
	{
		/* It looks like CI request for this is not even defined */
		/* Print out hard-coded result */
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CNMI: (0-3),(0-3),(0-3),(0-1),(0-1)" );
		break;
	}

	case TEL_EXT_GET_CMD:         /* AT+CNMI? */
	{
		MSG_GetNewMsgIndSetting(atHandle, &mode, &mt, &bm, &ds, &bfr);
		/* Note: The mode and bfr are settings saved in local variables. Not used by CP   */
		sprintf( (char *)AtRspBuf, "+CNMI:%d,%d,%d,%d,%d", mode, mt, bm, ds, bfr);
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)AtRspBuf);
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+CNMI= */
	{
		if ( getExtValue( parameter_values_p, 0, (int *)&mode, TEL_AT_CNMI_0_MODE_VAL_MIN, TEL_AT_CNMI_0_MODE_VAL_MAX, TEL_AT_CNMI_0_MODE_VAL_DEFAULT ) == TRUE )
		{
			if ( getExtValue( parameter_values_p, 1, (int *)&mt, TEL_AT_CNMI_1_MT_VAL_MIN, TEL_AT_CNMI_1_MT_VAL_MAX, TEL_AT_CNMI_1_MT_VAL_DEFAULT ) == TRUE )
			{
				if ( getExtValue( parameter_values_p, 2, (int *)&bm, TEL_AT_CNMI_2_BM_VAL_MIN, TEL_AT_CNMI_2_BM_VAL_MAX, TEL_AT_CNMI_2_BM_VAL_DEFAULT ) == TRUE )
				{
					if ( getExtValue( parameter_values_p, 3, (int *)&ds, TEL_AT_CNMI_3_DS_VAL_MIN, TEL_AT_CNMI_3_DS_VAL_MAX, TEL_AT_CNMI_3_DS_VAL_DEFAULT ) == TRUE )
					{
						if ( getExtValue( parameter_values_p, 4, (int *)&bfr, TEL_AT_CNMI_4_BFR_VAL_MIN, TEL_AT_CNMI_4_BFR_VAL_MAX, TEL_AT_CNMI_4_BFR_VAL_DEFAULT ) == TRUE )
						{
							ret = MSG_SetNewMsgIndSetting(atHandle, mode, mt, bm, ds, bfr);
							cmdValid = TRUE;
						}
					}
				}
			}
		}
		if ( !cmdValid )
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
		}
		break;
	}

	default:         /* AT+CNMI */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}


/************************************************************************************
 * F@: ciMsgList - GLOBAL API for GCF AT+CMGL command
 *
 *      List Messages
 */
RETURNCODE_T  ciListMsg( const utlAtParameterOp_T op,
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
	CiMsgMsgStatus readSmsOption;

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	**  process operation.
	*/
	switch (op)
	{
	case TEL_EXT_TEST_CMD:               /* AT+CMGL=? */
	{
		if ( gSmsFormatMode == ATCI_SMS_FORMAT_TXT_MODE )
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CMGL: \"REC UNREAD\",\"REC READ\",\"STO UNSENT\",\"STO SENT\",\"ALL\"");
		}
		else                  /* PDU mode */
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CMGL: (0-4)");
		}
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CMGL=<stat> */
	{
		BOOL validParam = FALSE;
		CHAR txtModeStatus[ TEL_AT_SMS_TEXTMODE_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];
		INT32 txtModeStatusLength;
		INT32 pduModeStatus;
		CHAR pduModeStatusStr[10];

		/* check whether it is an action command */
		if(parameter_values_p[0].is_default == TRUE) /* AT+CMGL */
		{
			readSmsOption = CI_MSG_MSG_STAT_REC_UNREAD;
			ret = MSG_ListMsgStart(atHandle, readSmsOption);

			break;
		}

		/* Step1: Get readSmsOption from AT command parameter */
		if ( gSmsFormatMode == ATCI_SMS_FORMAT_TXT_MODE )
		{
			/* <stat> is string type in text mode */
			if ( getExtString(parameter_values_p, 0, txtModeStatus, TEL_AT_SMS_TEXTMODE_MAX_LEN, (INT16 *)&txtModeStatusLength, NULL ) == TRUE )
			{
				/* must convert string selection to an enum value */
				if ( libConvertStringToCiMsgStatType( txtModeStatus, &readSmsOption ) == TRUE )
				{
					validParam = TRUE;
				}                 /* get index */
			}
		}
		else                 /* PDU mode */
		{
			/* <stat> is integer type in PDU mode */
			snprintf((char *)pduModeStatusStr, sizeof(pduModeStatusStr)/sizeof(pduModeStatusStr[0]), "%s", parameter_values_p[0].value.string_p);
			pduModeStatus = atoi((char *)pduModeStatusStr);

			if ( pduModeStatus >= TEL_AT_CMGL_0_STATE_VAL_MIN && pduModeStatus <= TEL_AT_CMGL_0_STATE_VAL_MAX)
			{
				/* convert index to SmsSimAccessType */
				if ( libConvertIntegerToCiMsgStatType( pduModeStatus, &readSmsOption ) == TRUE )
				{
					validParam = TRUE;
				}
			}
		}

		/* Step2: If AT command parameter is valid, send CI request */
		if ( validParam == TRUE )
		{
			ret = MSG_ListMsgStart(atHandle, readSmsOption);
		}
		else
		{
			/* invalid msg status type */
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
		}

		break;
	}

	case TEL_EXT_ACTION_CMD:           /* AT+CMGL  */
	{
		readSmsOption = CI_MSG_MSG_STAT_REC_UNREAD;
		ret = MSG_ListMsgStart(atHandle, readSmsOption);

		break;
	}

	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
		break;
	}
	}


	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);

	return(rc);
}

/************************************************************************************
 * F@: ciReadSMS - GLOBAL API for GCF AT+CMGR command
 *
 *       Read Message
 */
RETURNCODE_T  ciReadMsg( const utlAtParameterOp_T op,
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
	case TEL_EXT_TEST_CMD:               /* AT+CMGR=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CMGR=<index> */
	{
		INT32 msgIndex;
		if ( getExtValue(parameter_values_p, 0, &msgIndex, TEL_AT_SMS_INDEX_VAL_MIN, TEL_AT_SMS_INDEX_VAL_MAX, TEL_AT_SMS_INDEX_VAL_DEFAULT) == TRUE)
		{
			if ( msgIndex > 0 )
			{
				ret = MSG_ReadMsg( atHandle, msgIndex );
			}
			else
			{
				/* invalid index */
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_INVALID_MEMORY_INDEX, NULL);
			}
		}
		else
		{
			/* invalid index */
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_INVALID_MEMORY_INDEX, NULL);
		}

		break;
	}

	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
		break;
	}
	}


	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 * F@: ciSendSMS - GLOBAL API for GCF AT+CMGS-command
 *
 *       Send Message
 *       Note: ">" will display and CI request will be sent after enter SMS text and press Ctrl+Z
 */
RETURNCODE_T  ciSendMsg( const utlAtParameterOp_T op,
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
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	**  process operation.
	*/
	switch (op)
	{
	case TEL_EXT_TEST_CMD:               /* AT+CMGS=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CMGS= */
	{
		/* For PDU mode, AT command like: +CMGS=<length><CR>...  */
		if ( gSmsFormatMode == ATCI_SMS_FORMAT_PDU_MODE )
		{
			INT32 msgLength;
			CHAR msgLengthStr[10];

			/* get the message length parameter */
			/* Because in Txt mode, first para is defined as string type, refer to plusCMGS_params */
			snprintf((char *)msgLengthStr, sizeof(msgLengthStr)/sizeof(msgLengthStr[0]), "%s", parameter_values_p[0].value.string_p);
			msgLength = atoi((char *)msgLengthStr);

			if (msgLength > 0)
			{
				/* save the pdu length (times 2 because it takes twice more characters to enter) */
				ret = MSG_SetSentMsgInfo(atHandle, ATCI_SMS_FORMAT_PDU_MODE, msgLength * 2, NULL);
				gSendMsgReqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_SEND_MESSAGE_REQ);
			}
			else
			{
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_INVALID_PDU_MODE_PARA, NULL );
				break;
			}
		}

		/* For Text mode, AT command like: +CMGS=<da>[,<toda>]<CR>...  */
		else
		{
			CiAddressInfo destAddr;
			/* Extract the message destination address:  +CMGS=<da>[,<toda>]<CR> ... */
			if ( libGetSmsAddrInfo( parameter_values_p, 0, &destAddr ) == TRUE )
			{
				if ( destAddr.Length > 0 )
				{
					ret = MSG_SetSentMsgInfo(atHandle, ATCI_SMS_FORMAT_TXT_MODE, ATCI_MAX_SMS_TDPU_SIZE, &destAddr);
					gSendMsgReqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_SEND_MESSAGE_REQ);
				}
				else
				{
					ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_INVALID_TEXT_MODE_PARA, NULL );
					break;
				}
			}
			else
			{
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_INVALID_TEXT_MODE_PARA, NULL );
				break;
			}
		}

		gAtpCtrl[sAtpIndex].bSmsDataEntryModeOn = TRUE;

		ret = ATRESP( atHandle, 0, 0, (char *)"> " );
		break;

	}                 //End Set

	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
		break;
	}
	}


	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);

	return(rc);

}


/************************************************************************************
 * F@: ciSendStored - GLOBAL API for GCF AT+CMSS-command
 *
 *        Send Message from Storage
 */
RETURNCODE_T  ciSendStoredMsg( const utlAtParameterOp_T op,
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
	case TEL_EXT_TEST_CMD:               /* AT+CMSS=? */
	{
		ret = ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CMSS=<index>[,<da>[,<toda>]] (for both PDP mode and Text mode)*/
	{
		INT32 msgIndex;
		CiAddressInfo newDestAddr;
		if ( getExtValue(parameter_values_p, 0, &msgIndex, TEL_AT_SMS_INDEX_VAL_MIN, TEL_AT_SMS_INDEX_VAL_MAX, TEL_AT_SMS_INDEX_VAL_DEFAULT) == TRUE)
		{
			if ( msgIndex > 0 )
			{
				/* Extract the message destination address */
				libGetSmsAddrInfo( parameter_values_p, 1, &newDestAddr );

				/* Send stored msg by sending CI request */
				ret = MSG_SendStoredMsg(atHandle, msgIndex, newDestAddr);
			}
			else
			{
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL );
				break;
			}
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL );
		}
		break;
	}

	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL );
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 * F@: ciStoreSMS - GLOBAL API for GCF AT+CMGW-command
 *
 *       Write Message to Memory
 *        Note: ">" will display and CI request will be sent after enter SMS text and press Ctrl+Z
 */
RETURNCODE_T  ciStoreMsg( const utlAtParameterOp_T op,
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
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	**  process operation.
	*/
	switch (op)
	{
	case TEL_EXT_TEST_CMD:               /* AT+CMGW=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CMGW= */
	{
		/* For PDU mode, AT command like: +CMGW=<length>[,<stat>]<CR>...  */
		if ( gSmsFormatMode == ATCI_SMS_FORMAT_PDU_MODE )
		{
			INT32 msgLength;
			CHAR msgLengthStr[10];
			INT32 stat;

			/* get the message length parameter */
			/* the first parameter is declared as string type, so we cannot use getExtValue directly */
			snprintf((char *)msgLengthStr, sizeof(msgLengthStr) / sizeof(msgLengthStr[0]), "%s", parameter_values_p[0].value.string_p);
			msgLength = atoi((char *)msgLengthStr);

			if (msgLength > 0 && msgLength <= TEL_AT_SMS_PDUMODE_MAX_LEN)
			{
				if(getExtValue(parameter_values_p, 1, &stat, TEL_AT_CMGW_1_STAT_VAL_MIN, TEL_AT_CMGW_1_STAT_VAL_MAX, TEL_AT_CMGW_1_STAT_VAL_DEFAULT) == TRUE)
				{
					/* save the pdu length (times 2 because it takes twice more characters to enter) */
					ret = MSG_SetStoredMsgInfo(atHandle, ATCI_SMS_FORMAT_PDU_MODE, msgLength * 2, NULL, stat);
					gSendMsgReqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_WRITE_MESSAGE_REQ);
				}
				else
				{
					ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_INVALID_PDU_MODE_PARA, NULL );
					break;
				}
			}
			else
			{
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_INVALID_PDU_MODE_PARA, NULL );
				break;
			}
		}

		/* For Text mode, AT command like: +CMGW[=<oa/da>[,<tooa/toda>[,<stat>]]]]<CR>...  */
		else
		{
			CiAddressInfo destAddr;
			CHAR txtModeStatus[ TEL_AT_SMS_TEXTMODE_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];
			INT16 txtModeStatusLength;
			UINT8 stat;
			/* Extract the message destination address */
			if ( libGetSmsAddrInfo( parameter_values_p, 0, &destAddr ) == TRUE )
			{
				if ( destAddr.Length > 0 )
				{
					if ( getExtString(parameter_values_p, 2, txtModeStatus, TEL_AT_SMS_TEXTMODE_MAX_LEN, &txtModeStatusLength, (CHAR *)TEL_AT_CMGW_1_STAT_STR_DEFAULT ) == TRUE )
					{
						/* must convert string selection to an enum value */
						if ( libConvertStringToCiMsgStatType( txtModeStatus, &stat ) == TRUE )
						{
							if(stat >= TEL_AT_CMGW_1_STAT_VAL_MIN && stat <= TEL_AT_CMGW_1_STAT_VAL_MAX)
							{
								ret = MSG_SetStoredMsgInfo(atHandle, ATCI_SMS_FORMAT_TXT_MODE, ATCI_MAX_SMS_TDPU_SIZE, &destAddr, stat);
								gSendMsgReqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_MSG_PRIM_WRITE_MESSAGE_REQ);
							}
							else
							{
								ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_INVALID_TEXT_MODE_PARA, NULL );
								break;
							}
						}
						else
						{
							ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_INVALID_TEXT_MODE_PARA, NULL );
							break;
						}
					}
					else
					{
						ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_INVALID_TEXT_MODE_PARA, NULL );
						break;
					}
				}
				else
				{
					ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_INVALID_TEXT_MODE_PARA, NULL );
					break;
				}
			}
			else
			{
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_INVALID_TEXT_MODE_PARA, NULL );
				break;
			}
		}

		gAtpCtrl[sAtpIndex].bSmsDataEntryModeOn = TRUE;

		ret = ATRESP( atHandle, 0, 0, (char *)"> " );
		break;
	}

	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL );
		break;
	}
	}


	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}


/************************************************************************************
 * F@: ciMsgDelete - GLOBAL API for GCF AT+CMGD command
 *
 *       Delete Message
 */
RETURNCODE_T  ciDeleteMsg( const utlAtParameterOp_T op,
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
	case TEL_EXT_TEST_CMD:               /* AT+CMGD=? */
	{
		ret = MSG_QueryDelMsg(atHandle);
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CMGD=<index>[,<delflag>] */
	{
		INT32 delFlag, msgIndex;
		if ( getExtValue(parameter_values_p, 0, &msgIndex, TEL_AT_SMS_INDEX_VAL_MIN, TEL_AT_SMS_INDEX_VAL_MAX, TEL_AT_SMS_INDEX_VAL_DEFAULT) == TRUE)
		{
			if ( getExtValue(parameter_values_p, 1, &delFlag, TEL_AT_CMGD_1_DEFFLAG_VAL_MIN, TEL_AT_CMGD_1_DEFFLAG_VAL_MAX, TEL_AT_CMGD_1_DEFFLAG_VAL_DEFAULT) == TRUE)
			{
				if ( delFlag >= ATCI_SMS_DEL_MODE_DEFAULT && delFlag < ATCI_SMS_NUM_DEL_MODES )
				{
					if (delFlag < 4 )                 /* Delete One specified msg or some kind of all msg */
					{
						if ( msgIndex > 0 )
						{
							ret = MSG_DeleteMsg(atHandle, msgIndex, libConvertDelFlag2Ci(delFlag));
						}
						else
						{
							/* invalid msg index */
							ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_INVALID_MEMORY_INDEX, NULL);
						}
					}
					else if ( delFlag == 4 )                   /* Delete all msg, due to lack of corresponding CI operation, we have to delete all msg one by one */
					{
						ret = MSG_DeleteAllMsgStart(atHandle);
					}
				}
				else
				{
					/* invalid delFlag */
					ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
				}
			}
			else                    /* default delflag=0, to delete one msg specified by index */
			{
				ret = MSG_DeleteMsg(atHandle, msgIndex, CI_MSG_MSG_DELFLAG_INDEX);
			}

		}                 /* get index */
		else
		{
			/* invalid msg index */
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_INVALID_MEMORY_INDEX, NULL);
		}

		break;
	}
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
		break;
	}

	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}


/************************************************************************************
 * F@: ciSetMoreMsgToSend - GLOBAL API for GCF AT+CMMS -command
 *
 *      Set Option of More Message to Send
 */
RETURNCODE_T  ciSetMoreMsgToSend(  const utlAtParameterOp_T op,
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
	int mode = 0;

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	**   process operation.
	*/
	switch (op)
	{
	case TEL_EXT_TEST_CMD:            /* AT+CMMS=?  */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CMMS:(0,1,2)");
		break;
	}

	case TEL_EXT_GET_CMD:              /* AT+CMMS? */
	{
		char buffer[200];
		ret = MSG_GetModeMoreMsgToSend(atHandle, (UINT32 *)(&mode));
		sprintf(buffer, "+CMMS:%d", mode);
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)buffer);
		break;
	}

	case TEL_EXT_SET_CMD:            /* AT+CMMS= */
	{
		if ( getExtValue( parameter_values_p, 0, &mode, TEL_AT_CMMS_MODE_VAL_MIN, TEL_AT_CMMS_MODE_VAL_MAX, TEL_AT_CMMS_MODE_VAL_DEFAULT ) == TRUE )
		{
			ret = MSG_SetModeMoreMsgToSend( atHandle, (UINT32)mode);
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_SUPPORTED, NULL );
		}
		break;
	}
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}


/************************************************************************************
 * F@: ciSetMoService - GLOBAL API for GCF AT+CGSMS command
 *
 *       Set service type for MO SMS
 */
RETURNCODE_T  ciSetMoService( const utlAtParameterOp_T op,
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
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT+CGSMS=? */
	{
		ret = MSG_QueryMoService(atHandle);
		break;
	}

	case TEL_EXT_GET_CMD:         /* AT+CGSMS? */
	{
		ret = MSG_GetMoService(atHandle);
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+CGSMS= */
	{
		INT32 service;
		if ( getExtValue( parameter_values_p, 0, &service, TEL_AT_CGSMS_0_SERVICE_VAL_MIN, TEL_AT_CGSMS_0_SERVICE_VAL_MAX, TEL_AT_CGSMS_0_SERVICE_VAL_DEFAULT ) == TRUE )
		{
			ret = MSG_SetMoService(atHandle, service);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
		}
		break;
	}

	default:         /* AT+CGSMS */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}


/****************************************************************
 *  F@: ciSendMsgAfterCtrlZ - Send the SMS once user has finished entering it (i.e. pressed CTRL+Z)
 *
 */
void ciSendMsgAfterCtrlZ ( TelAtParserID sAtpIndex, CHAR *pSmsData, INT32 smsDataLength )
{
	UNUSEDPARAM(sAtpIndex)
	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 reqHandle = gSendMsgReqHandle;

	/* validate data pointer and data length */
	if ( (pSmsData == NULL ) || (smsDataLength < 0) || ( smsDataLength > CI_MAX_CI_MSG_PDU_SIZE * 2 ))
	{
		ret = ATRESP( reqHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
		return;
	}

	if (gSmsFormatMode == ATCI_SMS_FORMAT_PDU_MODE)
	{
		int scaLength;  //, *pInt=(UINT8 *)pSmsData;
		//scaLength = 2*(*pInt) + 2;  //<sca_len>=2 bytes, first byte is half of length of <type_addr> plus <number>
		scaLength = (hexToNum(pSmsData[0]) << 4) + hexToNum(pSmsData[1]);
		scaLength = 2 * scaLength + 2;

		if(scaLength >= smsDataLength)
		{
			ret = ATRESP( reqHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
			return;
		}

		DBGMSG("%s: reqHandle = %d. scaLength = %d.\n", __FUNCTION__, reqHandle, scaLength);
		ret = MSG_SendMsg(reqHandle, gSmsFormatMode, pSmsData + scaLength, smsDataLength - scaLength, pSmsData + 2, scaLength - 2);
	}

	/* For Txt mode, the PDU of CI msg is encoded in msg_api.c */
	else if (gSmsFormatMode == ATCI_SMS_FORMAT_TXT_MODE)
	{
		ret = MSG_SendMsg(reqHandle, gSmsFormatMode, pSmsData, smsDataLength, NULL, 0);
	}
	else
	{
		ret = ATRESP( reqHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_UNKNOWN_ERROR, NULL);
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return;
}

/****************************************************************
 *  F@: ciSendMsgAfterEsc - Cancel sending SMS after receiving ESC key
 *
 */
void ciSendMsgAfterEsc ( TelAtParserID sAtpIndex )
{
	UNUSEDPARAM(sAtpIndex)
	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 reqHandle = gSendMsgReqHandle;

	/* Call MSG api to clear g*/
	ret = MSG_SendMsg(reqHandle, gSmsFormatMode, NULL, 0, NULL, 0);

	/* Return OK to stop AT parser timer to finish this AT command session */
	ret = ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return;
}

RETURNCODE_T  ciNewMsgAck( const utlAtParameterOp_T op,
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
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_SET_CMD:
	{
		INT32 reply;
		/* for text mode,  AT+CNMA */
		if ( gSmsFormatMode == ATCI_SMS_FORMAT_TXT_MODE )
		{
			ret = __MSG_NewMsgAck(atHandle, 0);
		}

		/* for PDU mode,  AT+CNMA=<n> */
		else if ( gSmsFormatMode == ATCI_SMS_FORMAT_PDU_MODE )
		{
			if ( getExtValue( parameter_values_p, 0, &reply, 0, 2, 0 ) == TRUE )
			{
				ret = __MSG_NewMsgAck(atHandle, reply);
			}
			else
			{
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
			}
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
		}
		break;
	}

	case TEL_EXT_TEST_CMD:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}
#if 0
/******************************************************************************
 ******
 * F@: ciRSTMemFull - GLOBAL API for GCF AT*RSTMemFull command
 *
 */

RETURNCODE_T  ciRSTMemFull(            const utlAtParameterOp_T op,
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
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);


	switch ( op)
	{
	case TEL_EXT_ACTION_CMD:           /* AT*RSTMEMFULL */
	{
		ret = MSG_RSTMemFull(atHandle);
		break;
	}

	case TEL_EXT_TEST_CMD:          /* AT*RSTMEMFULL=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"*RSTMEMFULL:\r\nOK");
		break;
	}

	case TEL_EXT_GET_CMD:                   /* AT*RSTMEMFULL?  */
	case TEL_EXT_SET_CMD:                   /* AT*RSTMEMFULL=  */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}


	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}
#endif
/******************************************************************************
 ******
 * F@: ciCMEMFULL - GLOBAL API for GCF AT+CMEMFULL command
 *
 */

RETURNCODE_T  ciCMEMFULL(            const utlAtParameterOp_T op,
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
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);


	switch ( op)
	{
	case TEL_EXT_SET_CMD:          /* AT+CMEMFULL= */
	{
		int memFull;
		if ( getExtValue( parameter_values_p, 0, &memFull, 0, 1, 0 ) == TRUE )
		{
			ret = MSG_CMEMFULL(atHandle, (BOOL)memFull);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_ALLOWED, NULL);
		}
		break;
	}
	case TEL_EXT_GET_CMD:                   /* AT+CMEMFULL?  */
	case TEL_EXT_ACTION_CMD:                /* AT+CMEMFULL */
	case TEL_EXT_TEST_CMD: 	                /* AT+CMEMFULL=? */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}


	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

