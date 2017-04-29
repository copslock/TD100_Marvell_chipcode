/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *	INTEL CONFIDENTIAL
 *	Copyright 2006 Intel Corporation All Rights Reserved.
 *	The source code contained or described herein and all documents related to the source code (�Material? are owned
 *	by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
 *	its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
 *	Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
 *	treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
 *	transmitted, distributed, or disclosed in any way without Intel�s prior express written permission.
 *
 *	No license under any patent, copyright, trade secret or other intellectual property right is granted to or
 *	conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
 *	estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
 *	Intel in writing.
 *	-------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: telss.c
 *
 *  Authors:  Rovin Yu
 *
 *  Description: Process SS related AT commands
 *
 *  History:
 *   Jan 26, 2008 - Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/

/******************************************************************************
*   include files
******************************************************************************/

#include "telutl.h"
#include "telatparamdef.h"
#include "ci_ss.h"
#include "ci_sim.h"
#include "ci_api.h"
#include "telss.h"
#include "teldef.h"
#include "telatci.h"
#include "ss_api.h"
#include "sim_api.h"

#define CCFC_MAX_ADDR_LEN               20
#define CCWA_PRESENTATION_DISABLE    0
#define CCWA_PRESENTATION_ENABLE     1
#define CCWA_PRESENTATION_DEFAULT    255
#define CCWA_MODE_DISABLE            0
#define CCWA_MODE_ENABLE             1
#define CCWA_MODE_QUERY              2
#define CCWA_MODE_DEFAULT            255

extern AtciSSCntx gSSCntx;

/* Table for fac code lookup containing string, and password max length.... */
static const AtciFacilityDescriptor facDescTable[] =
{
	{ ATCI_SS_FAC_CS, "CS", CI_MAX_PASSWORD_LENGTH },
	{ ATCI_SS_FAC_PS, "PS", CI_MAX_PASSWORD_LENGTH },
	{ ATCI_SS_FAC_PF, "PF", CI_MAX_PASSWORD_LENGTH },
	{ ATCI_SS_FAC_SC, "SC", CI_MAX_PASSWORD_LENGTH },

	{ ATCI_SS_FAC_AO, "AO", CI_MAX_PASSWORD_LENGTH },
	{ ATCI_SS_FAC_OI, "OI", CI_MAX_PASSWORD_LENGTH },
	{ ATCI_SS_FAC_OX, "OX", CI_MAX_PASSWORD_LENGTH },
	{ ATCI_SS_FAC_AI, "AI", CI_MAX_PASSWORD_LENGTH },

	{ ATCI_SS_FAC_IR, "IR", CI_MAX_PASSWORD_LENGTH },
	{ ATCI_SS_FAC_AB, "AB", CI_MAX_PASSWORD_LENGTH },
	{ ATCI_SS_FAC_AG, "AG", CI_MAX_PASSWORD_LENGTH },
	{ ATCI_SS_FAC_AC, "AC", CI_MAX_PASSWORD_LENGTH },

	{ ATCI_SS_FAC_FD, "FD", CI_MAX_PASSWORD_LENGTH },

	{ ATCI_SS_FAC_NT, "NT", CI_MAX_PASSWORD_LENGTH },
	{ ATCI_SS_FAC_NM, "NM", CI_MAX_PASSWORD_LENGTH },
	{ ATCI_SS_FAC_NS, "NS", CI_MAX_PASSWORD_LENGTH },
	{ ATCI_SS_FAC_NA, "NA", CI_MAX_PASSWORD_LENGTH },

	{ ATCI_SS_FAC_PN, "PN", CI_MAX_PASSWORD_LENGTH },
	{ ATCI_SS_FAC_PU, "PU", CI_MAX_PASSWORD_LENGTH },
	{ ATCI_SS_FAC_PP, "PP", CI_MAX_PASSWORD_LENGTH },
	{ ATCI_SS_FAC_PC, "PC", CI_MAX_PASSWORD_LENGTH },

	{ ATCI_SS_FAC_P2, "P2", CI_MAX_PASSWORD_LENGTH }
};

extern void utlSetDefaultParameterValue(const utlAtParameterValue_P parameter_value_p,
					const utlAtDataType_T type,
					const utlAtParameterAccess_T access);
extern CiReturnCode PersonalizeME(UINT32 atHandle, AtciFacType ciFacType, INT32 mode, CHAR * facPwdStr, INT16 facPwdStrLen, BOOL is_ext);
extern CiReturnCode SS_GetCCWEMode(UINT32 atHandle);
extern CiReturnCode SS_SetCCWEMode(UINT32 atHandle, INT32 ccweMode);

const char * SS_CODE_STR[] =
{
	"",     //SS_CODE_NULL = 0,
	"002",  //SS_ALL_CF
	"004",  //SS_ALL_COND_CF
	"03",   //SS_CPWD  //added by Jiangang Jing
	"21",   //SS_CFU
	"30",   //SS_CLIP
	"31",   //SS_CLIR
	"33",   //SS_BAOC
	"35",   //SS_BAIC
	"43",   //SS_WAIT
	"60", //SS_USSD
	"61",   //SS_CF_NOREPLY
	"62",   //SS_CF_NOTREACHABLE
	"66",   //SS_CD
	"67",   //SS_CFB
	"76",   //SS_COLP
	"77",   //SS_COLR
	"96",   //SS_ECT
	"300",  //SS_CNAP
	"330",  //SS_ALL_BARRING
	"331",  //SS_BAOIC
	"332",  //SS_BAOIC_EXCHOME
	"333",  //SS_OUTG_BARRING
	"351",  //SS_BAIC_ROAMING
	"353",  //SS_INC_BARRING
	"360",  //SS_USS_ALL
	"361",  //SS_UUS1
	"362",  //SS_UUS2
	"363",  //SS_UUS3
	"3424", //SS_MRL_1
};


static SS_CODE_ENUM getSScode(const char *ssName)
{
	SS_CODE_ENUM ret = SS_CODE_NULL;
	UINT8 i;

	for (i = 1; i < sizeof(SS_CODE_STR) / sizeof(SS_CODE_STR[0]); i++)
	{
		if (strcmp(SS_CODE_STR[i], ssName) == 0)
		{
			ret = (SS_CODE_ENUM)i;
			break;
		}
	}
	return ret;
}

static CiSsCfReason getCcfcReason(SS_CODE_ENUM ss_code)
{
	CiSsCfReason ret = CICF_NUM_REASONS;

	switch (ss_code)
	{
	case SS_ALL_CF:
		ret = CICF_REASON_ALL;
		break;
	case SS_ALL_COND_CF:
		ret = CICF_REASON_ALL_CONDITIONAL;
		break;
	case SS_CFU:
		ret = CICF_REASON_CFU;
		break;
	case SS_CF_NOREPLY:
		ret = CICF_REASON_CFNRY;
		break;
	case SS_CF_NOTREACHABLE:
		ret = CICF_REASON_CFNRC;
		break;
	case SS_CFB:
		ret = CICF_REASON_CFB;
		break;
	default:
		break;
	}
	return ret;
}

static AtciSsCcfcMode getCcfcMode(SS_OP_ENUM ss_op)
{
	AtciSsCcfcMode ret = ATCI_SS_NUM_MODES;

	switch (ss_op)
	{
	case SS_OP_ACTIVE:
		ret = ATCI_SS_MODE_ENABLE;
		break;
	case SS_OP_DEACTIVE:
		ret = ATCI_SS_MODE_DISABLE;
		break;
	case SS_OP_INTERROGATE:
		ret = ATCI_SS_MODE_QUERY_STATUS;
		break;
	case SS_OP_REGISTER:
		ret = ATCI_SS_MODE_REGISTER;
		break;
	case SS_OP_ERASE:
		ret = ATCI_SS_MODE_ERASE;
		break;
	default:
		break;
	}
	return ret;
}

static UINT8 getCcfcClass(char *ss_si)
{
	UINT8 ret = CISS_BSMAP_NONE;
	UINT32 val = atoi(ss_si);

	switch (val)
	{
	case SS_MMI_TEL:
		ret = CISS_BSMAP_VOICE;
		break;
	case SS_MMI_ALL_TELSRV:
		ret = CISS_BSMAP_VOICE | CISS_BSMAP_DATA | CISS_BSMAP_FAX;
		break;
	case SS_MMI_ALL_DATASRV:
		ret = CISS_BSMAP_DATA | CISS_BSMAP_FAX | CISS_BSMAP_SMS;
		break;
	case SS_MMI_FAX:
		ret = CISS_BSMAP_FAX;
		break;
	case SS_MMI_SMS:
		ret = CISS_BSMAP_SMS;
		break;
	case SS_MMI_ALL_DATA_SYNC:
		ret = CISS_BSMAP_DATA_SYNC;
		break;
	case SS_MMI_ALL_SYNC:
		ret = CISS_BSMAP_DATA_SYNC | CISS_BSMAP_PACKET;
		break;
	case SS_MMI_ALL_ASYNC:
		ret = CISS_BSMAP_DATA_ASYNC |CISS_BSMAP_PAD;
		break;
	case SS_MMI_ALL_DATA_ASYNC:
		ret = CISS_BSMAP_DATA_ASYNC;
		break;
	case SS_MMI_ALL_BEARSRV:
		ret = CISS_BSMAP_DATA_SYNC | CISS_BSMAP_DATA_ASYNC;
		break;
	case SS_MMI_ALL_GPRS_BEARSRV:
		ret = CISS_BSMAP_PACKET;
		break;
	default:
		break;
	}
	return ret;
}

static INT32 getCcfcTime(char *ss_si)
{
	INT32 val = atoi(ss_si);

	return val;
}


/************************************************************************************
 * F@: checkValidFacility -
 *
 */
BOOL checkValidFacility( CHAR *facStr, INT16 facStrLen, AtciFacType *ciFacType )
{
	BOOL result = FALSE;
	INT16 i;


	for (i = 0; i < ATCI_SS_FAC_MAX; i++ )
	{
		if ( memcmp( facDescTable[i].facTypeStr, facStr, facStrLen ) == 0)
		{
			result = TRUE;
			break;
		}
	}

	if (( result == TRUE ) && ( ciFacType != NULL ))
	{
		/*
		**  Map the fac request....
		*/
		switch ( facDescTable[i].facTypeId )
		{
		case ATCI_SS_FAC_AO:
			ciFacType->facId = CISS_CB_LOCK_BAOC;
			ciFacType->facLogicGroup = ATCI_FAC_CB;
			break;
		case ATCI_SS_FAC_OI:
			ciFacType->facId = CISS_CB_LOCK_BOIC;
			ciFacType->facLogicGroup = ATCI_FAC_CB;
			break;
		case ATCI_SS_FAC_OX:
			ciFacType->facId = CISS_CB_LOCK_BOIC_EXHC;
			ciFacType->facLogicGroup = ATCI_FAC_CB;
			break;
		case ATCI_SS_FAC_AI:
			ciFacType->facId = CISS_CB_LOCK_BAIC;
			ciFacType->facLogicGroup = ATCI_FAC_CB;
			break;
		case ATCI_SS_FAC_IR:
			ciFacType->facId = CISS_CB_LOCK_BAIC_ROAMING;
			ciFacType->facLogicGroup = ATCI_FAC_CB;
			break;
		case ATCI_SS_FAC_AB:
			ciFacType->facId = CISS_CB_LOCK_ALL;
			ciFacType->facLogicGroup = ATCI_FAC_CB;
			break;
		case ATCI_SS_FAC_AG:
			ciFacType->facId = CISS_CB_LOCK_ALL_OUTGOING;
			ciFacType->facLogicGroup = ATCI_FAC_CB;
			break;
		case ATCI_SS_FAC_AC:
			ciFacType->facId = CISS_CB_LOCK_ALL_INCOMING;
			ciFacType->facLogicGroup = ATCI_FAC_CB;
			break;
		case ATCI_SS_FAC_PS:
			ciFacType->facId = CI_SIM_PERS_CAT_SIM;
			ciFacType->facLogicGroup = ATCI_FAC_MEP;
			break;
		case ATCI_SS_FAC_PF:
			ciFacType->facId = CI_SIM_FAC_CODE_PERS_FSIM;
			ciFacType->facLogicGroup = ATCI_FAC_OTHER;
			break;
		case ATCI_SS_FAC_SC:
			ciFacType->facId = CI_SIM_FAC_CODE_SIM;
			ciFacType->facLogicGroup = ATCI_FAC_OTHER;
			break;
		case ATCI_SS_FAC_FD:
			ciFacType->facId = CI_SIM_FAC_CODE_FDN;
			ciFacType->facLogicGroup = ATCI_FAC_OTHER;
			break;
		case ATCI_SS_FAC_PN:
			ciFacType->facId = CI_SIM_PERS_CAT_NETWORK;
			ciFacType->facLogicGroup = ATCI_FAC_MEP;
			break;
		case ATCI_SS_FAC_PU:
			ciFacType->facId = CI_SIM_PERS_CAT_NETWORKSUBSET;
			ciFacType->facLogicGroup = ATCI_FAC_MEP;
			break;
		case ATCI_SS_FAC_PP:
			ciFacType->facId = CI_SIM_PERS_CAT_SERVICEPROVIDER;
			ciFacType->facLogicGroup = ATCI_FAC_MEP;
			break;
		case ATCI_SS_FAC_PC:
			ciFacType->facId = CI_SIM_PERS_CAT_CORPORATE;
			ciFacType->facLogicGroup = ATCI_FAC_MEP;
			break;
		case ATCI_SS_FAC_P2:
			ciFacType->facId = CI_SIM_FAC_CODE_SIM;         //fix me:it's for SIM PIN2
			ciFacType->facLogicGroup = ATCI_FAC_OTHER;
			break;
		default:
			result = FALSE;
			break;
		}
	}  /*  if ( result == TRUE )  */

	return result;
}


/************************************************************************************
 *
 * SS related AT commands
 *
 *************************************************************************************/

/************************************************************************************
 * F@: ciCallForward - GLOBAL API for GCF AT+CCFC-command
 *
 */
RETURNCODE_T  ciCallForward(            const utlAtParameterOp_T op,
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
	BOOL cmdValid = FALSE;

	INT32 reasonParam;
	INT32 modeParam;
	INT32 fwdType = 0;
	INT32 callClass;
	INT32 subType = 0;
	INT32 noReplyTime;
	INT8 fwdToAddress[TEL_AT_CCFC_2_NUMBER_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];
	INT16 fwdToAddressLength;
	INT8 subAddress[TEL_AT_CCFC_5_SUBADDR_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];
	INT16 subAddressLength;

	CiSsCfRegisterInfo regInfo;

	/*
	 *  Put parser index into the variable
	 */
	CiRequestHandle atHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	atHandle = MAKE_AT_HANDLE( sAtpIndex );
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);


	switch (op)
	{
	case TEL_EXT_TEST_CMD:          /* AT+CCFC=? */
	{
		/* Return list of all supported 'n' values (defined in VgCCFCReasonType */
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CCFC: (0,1,2,3,4,5)");

		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+CCFC= */
	{
		if ( getExtValue(parameter_values_p, 0, &reasonParam, TEL_AT_CCFC_0_REASON_VAL_MIN, TEL_AT_CCFC_0_REASON_VAL_MAX, TEL_AT_CCFC_0_REASON_VAL_DEFAULT) == FALSE )
			break;

		DBGMSG("%s: reasonParam = %d.\n", __FUNCTION__, reasonParam);
		if ( getExtValue(parameter_values_p, 1, &modeParam, TEL_AT_CCFC_1_MODE_VAL_MIN, TEL_AT_CCFC_1_MODE_VAL_MAX, TEL_AT_CCFC_1_MODE_VAL_DEFAULT) == FALSE )
			break;

		/*
		**  Check mandatory parameters are in allowed range....
		*/
		DBGMSG("%s: modeParam = %d.\n", __FUNCTION__, modeParam);
		if ( (reasonParam < CICF_MAX_REASONS) && (modeParam <= ATCI_SS_MODE_ERASE) )
		{
			if ( modeParam == ATCI_SS_MODE_REGISTER )
			{
				/*
				**  Fetch parameters for registration, starting with forwarding string....
				*/
				if ( getExtString( parameter_values_p, 2, &fwdToAddress[0], TEL_AT_CCFC_2_NUMBER_STR_MAX_LEN, &fwdToAddressLength, TEL_AT_CCFC_2_NUMBER_STR_DEFAULT) == FALSE )
					break;

				/*
				**  Forwarding number type....
				*/
				DBGMSG("%s: fwdToAddress = %s.\n", __FUNCTION__, fwdToAddress);
				if ( getExtValue( parameter_values_p, 3, &fwdType, TEL_AT_CCFC_3_TYPE_VAL_MIN, TEL_AT_CCFC_3_TYPE_VAL_MAX, TEL_AT_CCFC_3_TYPE_VAL_DEFAULT ) == FALSE )
					break;

				/*
				**  If this number has an international prefix,
				**  switch to international type....
				*/
				if ( fwdToAddress[0] == '+' )
				{
					fwdType = ATCI_DIAL_NUMBER_INTERNATIONAL;
				}

				/*
				**  Call class....
				*/
				if ( getExtValue( parameter_values_p, 4, &callClass, TEL_AT_CCFC_4_CLASSX_VAL_MIN, TEL_AT_CCFC_4_CLASSX_VAL_MAX, TEL_AT_CCFC_4_CLASSX_VAL_DEFAULT  ) == FALSE )
					break;

				/*
				**  Forwarding subaddress....
				*/
				DBGMSG("%s: callClass = %d.\n", __FUNCTION__, callClass);
				if ( getExtString(parameter_values_p, 5, &subAddress[0], TEL_AT_CCFC_5_SUBADDR_STR_MAX_LEN, &subAddressLength, TEL_AT_CCFC_5_SUBADDR_STR_DEFAULT) == FALSE )
					break;

				/*
				**  Forwarding subaddress type....
				*/
				DBGMSG("%s: subAddress = %s.\n", __FUNCTION__, subAddress);
				if ( getExtValue( parameter_values_p, 6, &subType, TEL_AT_CCFC_6_SATYPE_VAL_MIN, TEL_AT_CCFC_6_SATYPE_VAL_MAX, TEL_AT_CCFC_6_SATYPE_VAL_DEFAULT ) == FALSE )
					break;

				/*
				**  If this number has an international prefix, switch to international type....
				*/
				DBGMSG("%s: subType = %d.\n", __FUNCTION__, subType);
				if ( subAddress[0] == '+' )
				{
					subType = ATCI_DIAL_NUMBER_INTERNATIONAL;
				}

				/*
				**  Time for delay....
				*/
				if ( getExtValue( parameter_values_p, 7, &noReplyTime, TEL_AT_CCFC_7_TIME_VAL_MIN, TEL_AT_CCFC_7_TIME_VAL_MAX, TEL_AT_CCFC_7_TIME_VAL_DEFAULT ) == FALSE )
					break;

				/*
				**  Check number type & class params are okay....
				*/
				DBGMSG("%s: noReplyTime = %d.\n", __FUNCTION__, noReplyTime);
				if ((( fwdType == ATCI_DIAL_NUMBER_INTERNATIONAL ) ||
				     ( fwdType == ATCI_DIAL_NUMBER_NATIONAL )      ||
				     ( fwdType == ATCI_DIAL_NUMBER_UNKNOWN )       ||
				     ( fwdType == ATCI_DIAL_NUMBER_NET_SPECIFIC )) &&
				    (( subType == ATCI_DIAL_NUMBER_RESTRICTED ) ||
				     ( subType == ATCI_DIAL_NUMBER_INTERNATIONAL ) ||
				     ( subType == ATCI_DIAL_NUMBER_NATIONAL )      ||
				     ( subType == ATCI_DIAL_NUMBER_UNKNOWN )       ||
				     ( subType == ATCI_DIAL_NUMBER_NET_SPECIFIC )) &&
				    ( isValidCiSsClass( (UINT16*)&callClass )))
				{
					/*
					**  Everything entered okay....
					*/
					cmdValid = TRUE;
				}
			}                  /* if( modeParam == ATCI_SS_MODE_REGISTER ) */
			else
			{
				/*
				**  Read but don't use the forwarding number....
				*/
				if ( getExtString( parameter_values_p, 2, &fwdToAddress[0], TEL_AT_CCFC_2_NUMBER_STR_MAX_LEN, &fwdToAddressLength, TEL_AT_CCFC_2_NUMBER_STR_DEFAULT ) == FALSE )
					break;

				/*
				**  Read, but don't use the forwarding number type....
				*/
				if ( getExtValue( parameter_values_p, 3, &fwdType, TEL_AT_CCFC_3_TYPE_VAL_MIN, TEL_AT_CCFC_3_TYPE_VAL_MAX, ATCI_DIAL_NUMBER_INTERNATIONAL ) == FALSE )
					break;

				/*
				**  Read Call class....
				*/
				if ( getExtValue( parameter_values_p, 4, &callClass, TEL_AT_CCFC_4_CLASSX_VAL_MIN, TEL_AT_CCFC_4_CLASSX_VAL_MAX, TEL_AT_CCFC_4_CLASSX_VAL_DEFAULT ) == FALSE )
					break;

				if ( isValidCiSsClass( (UINT16*)&callClass ) )
				{
					cmdValid = TRUE;
				}
			}                  /* if( modeParam == ATCI_SS_MODE_REGISTER ) */

			/*
			**  Check status of input data....
			*/
			if ( cmdValid == TRUE )
			{
				/*
				**  Now send the appropriate CI Request....
				*/
				switch (modeParam)
				{
				case ATCI_SS_MODE_DISABLE:
				case ATCI_SS_MODE_ENABLE:
				{
					ret = SS_SetCallForwardingStatus(atHandle, reasonParam, callClass, (BOOL)modeParam);
					break;
				}

				case ATCI_SS_MODE_QUERY_STATUS:
				{
					ret = SS_GetCallForwardingSettings(atHandle, reasonParam, callClass);
					break;
				}

				case ATCI_SS_MODE_REGISTER:
				{
					/*
					**  Fill in the CI request data for the forwarding number.
					*/
					regInfo.Reason        = (CiSsCfReason)reasonParam;
					regInfo.Classes       = (CiSsBasicServiceMap)callClass;

					regInfo.Number.Length = fwdToAddressLength;

					regInfo.Number.AddrType.NumType = DialNumTypeGSM2CI( fwdType );
					regInfo.Number.AddrType.NumPlan = CI_NUMPLAN_E164_E163;                                   //CI_NUMPLAN_UNKNOWN;

					memcpy( regInfo.Number.Digits, fwdToAddress, fwdToAddressLength );

					/*
					**  Fill in the sub-address.
					*/
					regInfo.OptSubaddr.Type = DialNumTypeGSM2CI( subType );
					if ( regInfo.OptSubaddr.Type != CI_NUMTYPE_UNKNOWN )
					{
						regInfo.OptSubaddr.Present = TRUE;
						regInfo.OptSubaddr.Length  = subAddressLength;

						memcpy( regInfo.OptSubaddr.Digits, subAddress, subAddressLength );
					}
					else
					{
						regInfo.OptSubaddr.Present = FALSE;
						regInfo.OptSubaddr.Length  = 0;
					}

					/*
					**  Set the No-Reply timeout.
					*/
					regInfo.CFNRyDuration = noReplyTime;

					ret = SS_AddCallForwarding(atHandle, &regInfo);

					break;
				}

				case ATCI_SS_MODE_ERASE:
				{
					/*
					**  Allocate memory for the erase info.
					*/
					ret = SS_RemoveCallForwarding(atHandle, reasonParam, callClass);
					break;
				}

				default:
					break;
				}                       /* switch (modeParam) */
			}                               /* if (cmdValid) */
		}                                       /* if( (reasonParam < CICF_MAX_REASONS) && (modeParam <= ATCI_SS_MODE_ERASE) ) */

		break;
	}

	case TEL_EXT_GET_CMD:                   /* AT+CCFC? */
	case TEL_EXT_ACTION_CMD:                /* AT+CCFC */
	default:
		break;
	}


	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 * F@: ciCallWaiting - GLOBAL API for GCF AT+CCWA -command
 *
 */

RETURNCODE_T  ciCallWaiting(            const utlAtParameterOp_T op,
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
	INT32 presentationStatus;
	INT32 cwMode;
	INT32 callClass;
	char cwBuf[200];


	/*
	 *  Put parser index into the variable
	 */
	CiRequestHandle atHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	atHandle = MAKE_AT_HANDLE( sAtpIndex );
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);


	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_GET_CMD:            /* AT+CCWA?  */
	{
		/*
		**  Send the request.
		*/
		/* NOTE:  this primitive is currently not supported by SAC */
		//ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_GET_CW_OPTION_REQ,
		//		atHandle, (void *)&getCwOptionReq);

		sprintf(cwBuf, "+CCWA: %d", gSSCntx.localCwOption);
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, cwBuf );
		break;
	}

	case TEL_EXT_TEST_CMD:            /* AT+CCWA=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char  *)"+CCWA: (0,1)");
		break;
	}
	case TEL_EXT_SET_CMD:              /* AT+CCWA= */
	{
		/*
		**  Fetch presentation status from user....
		*/
		if ( getExtValue( parameter_values_p, 0, &presentationStatus, TEL_AT_CCWA_0_N_VAL_MIN, TEL_AT_CCWA_0_N_VAL_MAX, TEL_AT_CCWA_0_N_VAL_DEFAULT ) == FALSE )
			break;

		/*
		**  Fetch mode parameter.
		*/
		if ( getExtValue( parameter_values_p, 1, &cwMode, TEL_AT_CCWA_1_MODE_VAL_MIN, TEL_AT_CCWA_1_MODE_VAL_MAX, TEL_AT_CCWA_1_MODE_VAL_DEFAULT ) == FALSE )
			break;

		/*
		**  Now get call class....
		*/
		if ( getExtValue( parameter_values_p, 2, &callClass, TEL_AT_CCWA_2_CLASSX_VAL_MIN, TEL_AT_CCWA_2_CLASSX_VAL_MAX, TEL_AT_CCWA_2_CLASSX_VAL_DEFAULT ) == FALSE )
			break;

		if ( isValidCiSsClass( (UINT16*)&callClass ) != TRUE )
			callClass = CISS_BSMAP_DEFAULT;
		if ( cwMode == CCWA_MODE_DEFAULT)                //it means it is +CCWA=0/1
		{
			//SAC not support it, do it on AP side
			//setCwOptionReq.Local = presentationStatus;
			//ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_SET_CW_OPTION_REQ, atHandle, (void *)&setCwOptionReq );
			gSSCntx.localCwOption = presentationStatus;
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL );
		}
		else if ( cwMode == CCWA_MODE_QUERY)                  // it means we just need to query currently network status for  CW service. do'nt need to process the first parameter presentationStatus
		{
			ret = SS_GetCallWaitingSettings(atHandle, (UINT32)callClass);
		}
		else                   //we need to process both CW indication of modem and CW request to network
		{
			// process CW indication of modem
			if (presentationStatus != 2)
				gSSCntx.localCwOption = presentationStatus;

			ret = SS_SetCallWaitingStatus(atHandle, (UINT32)callClass, (cwMode == CCWA_MODE_ENABLE ) ? TRUE : FALSE);
		}

		break;
	}

	case TEL_EXT_ACTION_CMD:           /* AT+CCWA */
	default:
		break;
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}


/************************************************************************************
 * F@: ciCOLP - GLOBAL API for GCF AT+COLP command
 *
 */

RETURNCODE_T  ciCOLP(            const utlAtParameterOp_T op,
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
	INT32 colpFlag;

	/*
	 *  Put parser index into the variable
	 */
	CiRequestHandle atHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	atHandle = MAKE_AT_HANDLE( sAtpIndex );
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);


	switch (op)
	{
	case TEL_EXT_GET_CMD:          /* AT+COLP? */
	{
		/*
		**  Determine COLP status.
		*/
		ret = SS_GetDialedIdSettings(atHandle);

		break;
	}

	case TEL_EXT_TEST_CMD:          /* AT+COLP=? */
	{
		/* Return list of all supported 'n' values (defined in VgSsPresentationType
		 * enumeration) which are valid for assignment to command....
		 * DISABLE, ENABLE
		 */
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+COLP: (0,1)\r\n");
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+COLP= */
	{
		/*
		**  Enables the transmission or suppression of our caller ID to the
		**  called party when we originate a call....
		*/
		if (getExtValue( parameter_values_p, 0, &colpFlag, TEL_AT_COLP_0_N_VAL_MIN, TEL_AT_COLP_0_N_VAL_MAX, TEL_AT_COLP_0_N_VAL_DEFAULT ) == TRUE)
		{
			ret = SS_SetDialedIdStatus(atHandle, (UINT32)colpFlag);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		}

		break;
	}

	case TEL_EXT_ACTION_CMD:         /* AT+COLP */
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
 * F@: ciCLIP - GLOBAL API for GCF AT+CLIP command
 *
 */
RETURNCODE_T  ciCLIP(            const utlAtParameterOp_T op,
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
	INT32 clipFlag;

	/*
	 *  Put parser index into the variable
	 */
	UINT32 atHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	atHandle = MAKE_AT_HANDLE( sAtpIndex );
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch (op)
	{
	case TEL_EXT_GET_CMD:            /* AT+CLIP?  */
	{
		ret = SS_GetCallerIdSettings(atHandle);
		break;
	}

	case TEL_EXT_TEST_CMD:            /* AT+CLIP=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CLIP: (0,1)");
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CLIP= */
	{
		if (getExtValue( parameter_values_p, 0, &clipFlag, TEL_AT_CLIP_0_N_VAL_MIN, TEL_AT_CLIP_0_N_VAL_MAX, TEL_AT_CLIP_0_N_VAL_DEFAULT ) == TRUE)
		{
			ret = SS_SetCallerIdStatus(atHandle, (UINT32)clipFlag);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		}

		break;
	}

	case TEL_EXT_ACTION_CMD:           /* AT+CLIP */
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

/* Added by Michal Bukai*/
/************************************************************************************
 * F@: ciCDIP - GLOBAL API for GCF AT+CDIP command
 *
 */
RETURNCODE_T  ciCDIP(            const utlAtParameterOp_T op,
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
	/*
	**  Set the result code to INITIAL_RETURN_CODE.  This allows
	**  the indications to display the correct return code after the
	**  AT Command is issued.
	*/
	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	INT32 cdipFlag;

	UINT32 atHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	atHandle = MAKE_AT_HANDLE( sAtpIndex );
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_GET_CMD:            /* AT+CDIP?  */
	{
		ret = SS_GetCdipStatus(atHandle);
		break;
	}

	case TEL_EXT_TEST_CMD:            /* AT+CDIP=? */
	{

		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CDIP: (0,1)");
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CDIP= */
	{
		if (getExtValue( parameter_values_p, 0, &cdipFlag, TEL_AT_CDIP_0_N_VAL_MIN, TEL_AT_CDIP_0_N_VAL_MAX, TEL_AT_CDIP_0_N_VAL_DEFAULT ) == TRUE)
		{
			ret = SS_SetCdipOption(atHandle, (UINT32)cdipFlag);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
		}

		break;
	}

	case TEL_EXT_ACTION_CMD:           /* AT+CDIP */
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
 * F@: ciCNAP - GLOBAL API for GCF AT+CNAP command
 *
 */

RETURNCODE_T  ciCNAP(            const utlAtParameterOp_T op,
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
	INT32 cnapFlag;

	/*
	 *  Put parser index into the variable
	 */
	CiRequestHandle atHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	atHandle = MAKE_AT_HANDLE( sAtpIndex );
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);


	switch (op)
	{
	case TEL_EXT_GET_CMD:          /* AT+CNAP? */
	{
		/*
		**  Determine CNAP status.
		*/
		ret = SS_GetNamePresentationSettings(atHandle);
		break;
	}

	case TEL_EXT_TEST_CMD:          /* AT+CNAP=? */
	{
		/* Return list of all supported 'n' values (defined in VgSsPresentationType
		 * enumeration) which are valid for assignment to command....
		 * DISABLE, ENABLE
		 */
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CNAP: (0,1)\r\n");
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CNAP= */
	{
		if (getExtValue( parameter_values_p, 0, &cnapFlag, TEL_AT_CNAP_0_N_VAL_MIN, TEL_AT_CNAP_0_N_VAL_MAX, TEL_AT_CNAP_0_N_VAL_DEFAULT ) == TRUE)
		{
			ret = SS_SetNamePresentationStatus(atHandle, (UINT32)cnapFlag);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		}

		break;
	}

	default:
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}
	break;
	}


	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}


/************************************************************************************
 * F@: ciCallIDRes - GLOBAL API for GCF AT+CLIR command
 *
 */
RETURNCODE_T  ciCallIDRes(            const utlAtParameterOp_T op,
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
	UINT32 CLIRpresentation;

	/*
	 *  Put parser index into the variable
	 */
	CiRequestHandle atHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	atHandle = MAKE_AT_HANDLE( sAtpIndex );
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);


	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT+CLIR=? */
	{
		/* No CI primitive for this: print hard-coded reply */
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CLIR: (0-2)" );
		break;
	}

	case TEL_EXT_GET_CMD:         /* AT+CLIR? */
	{
		ret = SS_GetHideIdSettings(atHandle);
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+CLIR= */
	{
		if ( getExtValue( parameter_values_p, 0, (int *)&CLIRpresentation, TEL_AT_CLIR_0_N_VAL_MIN, TEL_AT_CLIR_0_N_VAL_MAX, TEL_AT_CLIR_0_N_VAL_DEFAULT ) == TRUE )
		{
			ret = SS_SetHideIdStatus(atHandle, (UINT32)CLIRpresentation);
		}
		break;
	}

	default:         /* AT+CLIR */
	{
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 * F@: ciCOLR - GLOBAL API for GCF AT+COLR command
 * NOTE: This command is not in 27.007, but it is required for GCF TC.31.1.4.1.1
 *
 */
RETURNCODE_T  ciCOLR(            const utlAtParameterOp_T op,
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

	/*
	 *  Put parser index into the variable
	 */
	CiRequestHandle atHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	atHandle = MAKE_AT_HANDLE( sAtpIndex );
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);


	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_GET_CMD:         /* AT+COLR? */
	{
		ret = SS_GetHideConnectedIdSettings(atHandle);
		break;
	}

	default:         /* AT+COLR,  AT+COLR=?, AT+COLR= */
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
 * F@: ciCSSN - GLOBAL API for GCF AT+CSSN  -command
 *
 */

RETURNCODE_T  ciCSSN(            const utlAtParameterOp_T op,
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

	INT32 nValue;
	INT32 mValue;

	/*
	 *  Put parser index into the variable
	 */
	CiRequestHandle atHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	atHandle = MAKE_AT_HANDLE( sAtpIndex );
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);


	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_GET_CMD:            /* AT+CSSN?  */
	{
		ret = SS_GetNotificationOptions(atHandle);
		break;
	}

	case TEL_EXT_TEST_CMD:            /* AT+CSSN=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CSSN: (0,1),(0,1)");
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CSSN= */
	{
		/*
		**  Extract n and m parameters. Default is disabled (i.e. 0)
		*/
		if ( getExtValue( parameter_values_p, 0, &nValue, TEL_AT_CSSN_0_N_VAL_MIN, TEL_AT_CSSN_0_N_VAL_MAX, TEL_AT_CSSN_0_N_VAL_DEFAULT) == TRUE )
		{
			if ( getExtValue( parameter_values_p, 1, &mValue, TEL_AT_CSSN_1_M_VAL_MIN, TEL_AT_CSSN_1_M_VAL_MAX, TEL_AT_CSSN_1_M_VAL_DEFAULT) == TRUE )
			{
				ret = SS_SetNotificationOptions(atHandle, (BOOL)nValue, (BOOL)mValue);
			}
		}

		break;
	}

	case TEL_EXT_ACTION_CMD:           /* AT+CSSN */
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
 * F@: convertAtDcs2CiDcs
 *
 */
BOOL convertAtDcs2CiDcs ( INT8 dcs, CiUssdCharSet *ciCharSet )
{
	BOOL result = TRUE;

	/* default to unknown */
	*ciCharSet = CISS_USSD_UNKNOWN;

	/*Process coding group.... */
	switch (dcs & 0xC0)
	{
	case 0x00:                              /* 00xx */
	{
		/* All encoded in default except for..... */
		if (dcs == 0x11)
		{
			/* Coded as UCS2.... */
			*ciCharSet = CISS_USSD_CHARSET_MO_UCS2;
		}
		else if ((dcs == 0x10) || (dcs == 0xf) )
		{
			/* Coded as GSM.... */
			*ciCharSet = CISS_USSD_CHARSET_MO_GSM7BIT;
		}
		break;
	}

	case 0x40:                              /* 01xx */
	{
		switch (dcs & 0x0C)
		{
		case 0x00:                                      /* GSM */
		{
			/* Coded as GSM.... */
			*ciCharSet = CISS_USSD_CHARSET_MO_GSM7BIT;
			break;
		}

		case 0x04:                                      /* HEX */
		{
			/* Coded as HEX.... */
			*ciCharSet = CISS_USSD_CHARSET_HEX;
			break;
		}

		case 0x08:                                      /* UCS2 */
		{
			/* Coded as UCS2.... */
			*ciCharSet = CISS_USSD_CHARSET_MO_UCS2;
			break;
		}
		}

		break;
	}

	case 0xC0:                                              /* 11xx */
	{
		if ((dcs & 0xF0) == 0xF0)                       /* 1111 */
		{
			/* Check for 8-bit data.... */
			if (dcs & 0x04)
			{
				/* Coded as HEX.... */
				*ciCharSet = CISS_USSD_CHARSET_HEX;
			}
			else
			{
				/* Coded as GSM.... */
				*ciCharSet = CISS_USSD_CHARSET_MO_GSM7BIT;
			}
		}

		else if ((dcs & 0xE0) == 0xE0)
		{
			/* WAP encoding.... */
			result = FALSE;
		}
		break;
	}

	case 0x80:                              /* 10xx: Reserved */
	default:
	{
		/* Reserved */
		break;
	}
	}
	// we just support two kind of char set so far----Johnny
	if ( (*ciCharSet != CISS_USSD_CHARSET_MO_GSM7BIT) && (*ciCharSet != CISS_USSD_CHARSET_MO_UCS2))
		result = FALSE;

	return result;
}

/***********************************
 * Use to transfer user input "UCS2" string to real UCS2 code
	if ussdInString is "1A2B3C4D"
	ussdOutString lenght is 4, the content in each byte is: 0x1A 0x2B 0x3C 0x4D
 */

BOOL convertAtStr2CiStr(CHAR* ussdInString, INT16 ussdInLen, INT8* ussdOutString, UINT8* ussdOutLen, CiUssdCharSet charSet)
{
	volatile INT16 i = 0;
	volatile UINT8 num = 0;
	char tmp[5];
	char buf[CI_MAX_USSD_LENGTH];
	int tra;

	if ( (charSet != CISS_USSD_CHARSET_MO_GSM7BIT) && (charSet != CISS_USSD_CHARSET_MO_UCS2) )
		return FALSE;

	if ( charSet == CISS_USSD_CHARSET_MO_GSM7BIT) //keep unchanged
	{
		memcpy(ussdOutString, ussdInString, ussdInLen);
		*ussdOutLen = ussdInLen;
		return TRUE;
	}

	// it is UCS2 string
	while ( i < ussdInLen)
	{
		memcpy(tmp, &ussdInString[i], 2);
		tmp[2] = '\0';
		sscanf(tmp, "%x", &tra);
		buf[num] = (UINT8)tra;
		i = i + 2;
		num++;

	}
	*ussdOutLen = num;
	memcpy(ussdOutString, buf, num);
	if ( i == ussdInLen)
		return TRUE;
	else
		return FALSE;
}
/************************************************************************************
 * F@: ciCUSD - GLOBAL API for GCF AT+CUSD   -command
 *
 */
RETURNCODE_T  ciCUSD(            const utlAtParameterOp_T op,
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

	INT32 nParam;
	CHAR ussdString[TEL_AT_CUSD_1_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];
	INT16 ussdStringLength = 0;
	INT32 dcsParam;
	CiUssdCharSet charSet;
	INT8 data[ CI_MAX_USSD_LENGTH + ATCI_NULL_TERMINATOR_LENGTH];              /* USS Data */
	UINT8 dataLen;
	CHAR tmpBuf[50];

	/*
	 *  Put parser index into the variable
	 */
	CiRequestHandle atHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	atHandle = MAKE_AT_HANDLE( sAtpIndex );
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch (op)
	{
	case TEL_EXT_GET_CMD:            /* AT+CUSD?  */
	{
		//SAC not support it-- Johnny
		//ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SS], CI_SS_PRIM_GET_USSD_ENABLE_REQ,
		//		atHandle, (void *)&getUssdEnableReq );
		sprintf((char *)tmpBuf, "+CUSD: %d", gSSCntx.ussdMode);
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)tmpBuf);
		break;
	}

	case TEL_EXT_TEST_CMD:            /* AT+CUSD=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CUSD: (0,1,2)");
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CUSD= */
	{
		if ( getExtValue(parameter_values_p, 0, &nParam, TEL_AT_CUSD_0_N_VAL_MIN, TEL_AT_CUSD_0_N_VAL_MAX, TEL_AT_CUSD_0_N_VAL_DEFAULT) == TRUE )
		{
			/*
			**  Check validity of 'n' value....
			*/
			switch (nParam)
			{
			case ATCI_SS_PRESENTATION_DISABLE:
			case ATCI_SS_PRESENTATION_ENABLE:
			{
				gSSCntx.ussdMode = nParam;

				/*
				**  Determine if anymore arguments are in the command.
				*/
				if ( getExtString(parameter_values_p, 1, &ussdString[0], TEL_AT_CUSD_1_STR_MAX_LEN, &ussdStringLength, TEL_AT_CUSD_1_STR_DEFAULT) == TRUE )
				{
					if ( ussdStringLength > 0 )
					{
						/* ##3424*9# is a Marvell special code used for assert CP. */
						if (strcmp ((char *)ussdString, "##3424*9#") == 0) {
							ret = SS_SendMarvellPrivateCode(atHandle, (const CHAR*)ussdString, sizeof(ussdString));
							/* Return OK to avoid block the channel. */
							if (ret == CIRC_FAIL)
								ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
							else
								ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);

							break;
						}
						/*
						**  There is a string to send the network.
						*/
						//                  if (( getExtValue( parameter_values_p, X, &dcsParam, CISS_USSD_UNKNOWN ) == TRUE ) &&
						//                      ( convertAtDcs2CiDcs ((INT8)dcsParam, &ussdInfo.CharSet ) == TRUE ))
						if ( (getExtValue( parameter_values_p, 2, &dcsParam, TEL_AT_CUSD_2_DCS_VAL_MIN, TEL_AT_CUSD_2_DCS_VAL_MAX, TEL_AT_CUSD_2_DCS_VAL_DEFAULT ) == TRUE) &&
						     (convertAtDcs2CiDcs((INT8)dcsParam, &charSet) == TRUE) && (convertAtStr2CiStr(ussdString, ussdStringLength, data, &dataLen, charSet) == TRUE) )
							ret = SS_SendSupServiceData(atHandle, charSet, data, dataLen);
						else
							ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
					}
					else                                 // just have the first parameter-Johnny
					{
						ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
					}                                       /* if ( ussdStringLength */

				}                                               /* if (( gCurrentUssdState */

				break;
			}
			case ATCI_SS_PRESENTATION_CANCEL:
			{
				/*
				**  Cancel current USSD and send the CI Request.
				*/
				ret = SS_CancelSupServiceDataSession(atHandle);
				break;
			}
			default:
			{
				break;
			}
			}
		}
		break;
	}

	case TEL_EXT_ACTION_CMD:           /* AT+CUSD */
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
 * F@: ciCLCK - GLOBAL API for GCF AT+CLCK -command
 *
 */
RETURNCODE_T  ciCLCK(            const utlAtParameterOp_T op,
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
	INT16 facStrLen = 0;
	INT16 facPwdStrLen = 0;
	INT32 mode;
	INT32 callClass;
	INT16 i;

	AtciFacType ciFacType;
	CHAR facStr[TEL_AT_CLCK_0_FAC_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];

	CHAR facPwdStr[TEL_AT_CLCK_2_PASSWD_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];
	char clckBuf[500];

	/*
	 *  Put parser index into the variable
	 */
	CiRequestHandle atHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	atHandle = MAKE_AT_HANDLE(sAtpIndex);
	*xid_p = atHandle;

	switch (op)
	{
	case TEL_EXT_TEST_CMD:            /* AT+CLCK=?  */
	{
		/*
		**  Display the list as defined in CI; this is not an actual CI request to SAC!!
		*/
		sprintf( clckBuf, "+CLCK: (\"%s\"", facDescTable[0].facTypeStr );
		ATRESP( atHandle, 0, 0, clckBuf);

		for (i = 1; i < ATCI_SS_FAC_MAX; i++ )
		{
			sprintf( clckBuf, ", \"%s\"", facDescTable[i].facTypeStr );
			ATRESP( atHandle, 0, 0, clckBuf);
		}

		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)")");
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CLCK= */
	{
		/*
		**  Get fac string and mode (mandatory)....
		*/
		if (( getExtString( parameter_values_p, 0, &facStr[0], TEL_AT_CLCK_0_FAC_STR_MAX_LEN, &facStrLen, TEL_AT_CLCK_0_FAC_STR_DEFAULT) == TRUE ) &&
		    ( getExtValue(parameter_values_p, 1, &mode, TEL_AT_CLCK_1_MODE_VAL_MIN, TEL_AT_CLCK_1_MODE_VAL_MAX, TEL_AT_CLCK_1_MODE_VAL_DEFAULT) == TRUE ))
		{
			/*
			**  If fac code valid, process request....
			*/
			if (( checkValidFacility( &facStr[0], facStrLen, &ciFacType ) == TRUE ) &&
			    ( mode <= ATCI_SS_MODE_QUERY_STATUS ))
			{
				/* retrieve the password */
				if ( getExtString(parameter_values_p, 2, &facPwdStr[0], TEL_AT_CLCK_2_PASSWD_STR_MAX_LEN, &facPwdStrLen, TEL_AT_CLCK_2_PASSWD_STR_DEFAULT) == TRUE )
				{
					if ( ciFacType.facLogicGroup == ATCI_FAC_CB )
					{
						/*
						**  modify the timeout for the AT command processing
						*/
						{
							utlRelativeTime_T period;
							period.seconds     = 40;
							period.nanoseconds = 0;
							if ( utlModifyPendingAsyncResponse(&period, *xid_p) != utlSUCCESS)
								break;
						}
						if (( getExtValue(parameter_values_p, 3, &callClass, TEL_AT_CLCK_3_CLASSX_VAL_MIN, TEL_AT_CLCK_3_CLASSX_VAL_MAX, TEL_AT_CLCK_3_CLASSX_VAL_DEFAULT) == TRUE ) )
						{
						/* check if a call barring or SIM related service */

							if ( isValidCiSsClass( (UINT16*)&callClass ) == TRUE )
							{
								switch (mode)
								{
								case ATCI_SS_MODE_DISABLE:
								case ATCI_SS_MODE_ENABLE:
								{
									ret = SS_SetCallBarringStatus(atHandle, ciFacType.facId, callClass, facPwdStr, (BOOL)mode);
									break;
								}

								case ATCI_SS_MODE_QUERY_STATUS:
								{
									ret = SS_GetCallBarringStatus(atHandle, ciFacType.facId, callClass, NULL);
									break;
								}
								}
							}
						}                  /* if( ciFacType.cbType == TRUE ) */
                       /* if( ciFacType.cbType == TRUE ) */
					}                               /* if (( getExtValue(parameter_values_p, X, &callClass, */
					else if (ciFacType.facLogicGroup == ATCI_FAC_MEP)
					{
						/*
						**  modify the timeout for the AT command processing
						*/
						{
							utlRelativeTime_T period;
							period.seconds     = 20;
							period.nanoseconds = 0;
							if ( utlModifyPendingAsyncResponse(&period, *xid_p) != utlSUCCESS)
								break;
						}
						if(mode == 0) mode = 3;
						else if(mode == 1) mode = 4;
						ret = PersonalizeME(atHandle, ciFacType, mode, facPwdStr, facPwdStrLen, FALSE);
					}
					else
					{
						/*
						**  modify the timeout for the AT command processing
						*/
						{
							utlRelativeTime_T period;
							period.seconds     = 20;
							period.nanoseconds = 0;
							if ( utlModifyPendingAsyncResponse(&period, *xid_p) != utlSUCCESS)
								break;
						}
						ret  = SIM_SetLockFacility(atHandle, mode, ciFacType.facId, facPwdStr, facPwdStrLen);
					}
				}                                       /*if( getExtString(parameter_values_p, X, &facPwdStr[0],  */
			}
		}

		break;
	}

	case TEL_EXT_GET_CMD:                   /* AT+CLCK? */
	case TEL_EXT_ACTION_CMD:                /* AT+CLCK */
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
 * F@: ciCPWD - GLOBAL API for GCF AT+CPWD  -command
 *
 */
RETURNCODE_T  ciPassword(            const utlAtParameterOp_T op,
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

	CHAR oldCbPinStr[TEL_AT_CPWD_1_OLDPW_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];
	CHAR newCbPinStr[TEL_AT_CPWD_2_NEWPW_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];
	CHAR newCbPinVerStr[TEL_AT_CPWD_3_NEWPWVER_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];

	CHAR facStr[TEL_AT_CPWD_0_FAC_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];
	INT16 facStrLen = 0;
	INT16 oldCbPinLen;
	INT16 newCbPinLen;
	INT16 newCbPinVerLen;
	INT16 i;
	char passwdBuf[500];

	/*
	 *  Put parser index into the variable
	 */
	CiRequestHandle atHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	atHandle = MAKE_AT_HANDLE( sAtpIndex );
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);


	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_TEST_CMD:            /* AT+CPWD=? */
	{
		/*
		**  Display the list as defined in CI; this is not an actual CI request to SAC!!
		*/
		ATRESP( atHandle, 0, 0, (char *)"+CPWD: ");
		for (i = 0; i < ATCI_SS_FAC_MAX; i++ )
		{
			sprintf( passwdBuf, "(\"%s\", %d) ", facDescTable[i].facTypeStr, facDescTable[i].pswdLen );
			ATRESP( atHandle, 0, 0, passwdBuf);
		}
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CPWD= */
	{
		/*
		**  Get fac string, old passwd and new password....
		*/
		if ( getExtString( parameter_values_p, 0, &facStr[0], TEL_AT_CPWD_0_FAC_STR_MAX_LEN, &facStrLen, TEL_AT_CPWD_0_FAC_STR_DEFAULT) == TRUE )
		{
			/*
			**  If fac code valid, process request....
			*/
			if ( checkValidFacility( facStr, facStrLen, NULL ) == TRUE )
			{
				/* retrieve the old password */
				if ( getExtString(parameter_values_p, 1, &oldCbPinStr[0], TEL_AT_CPWD_1_OLDPW_STR_MAX_LEN, &oldCbPinLen, TEL_AT_CPWD_1_OLDPW_STR_DEFAULT) == TRUE )
				{
					/* retrieve the new password */
					if ( getExtString(parameter_values_p, 2, &newCbPinStr[0], TEL_AT_CPWD_2_NEWPW_STR_MAX_LEN, &newCbPinLen, TEL_AT_CPWD_2_NEWPW_STR_DEFAULT) == TRUE )
					{

						if ( getExtString(parameter_values_p, 3, &newCbPinVerStr[0], TEL_AT_CPWD_3_NEWPWVER_STR_MAX_LEN, &newCbPinVerLen, TEL_AT_CPWD_3_NEWPWVER_STR_DEFAULT) == TRUE )
						{

							/* Check entered string.... */

							if (strcmp((char *)facStr, "SC") == 0)
							{
								/* Change CHV1 value */
								ret = SIM_EnterPin(atHandle, CI_SIM_CHV_1, CI_SIM_CHV_CHANGE, oldCbPinStr, newCbPinStr);

							}
							else
							if (strcmp((char *)facStr, "P2") == 0)
							{
								/* Change CHV2 value */
								ret = SIM_EnterPin(atHandle, CI_SIM_CHV_2, CI_SIM_CHV_CHANGE, oldCbPinStr, newCbPinStr);
							}
							else
							{
								ret = SS_ChangeCallBarringPassword(atHandle, oldCbPinStr, newCbPinStr, NULL);
							}
						}
					}
				}
			}
		}
		break;
	}

	case TEL_EXT_GET_CMD:                   /* AT+CPWD?  */
	case TEL_EXT_ACTION_CMD:                /* AT+CPWD */
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

/*******************************************************************
*  FUNCTION:  ciMakeSS
*
*  DESCRIPTION: To support call unrelated SS triggered by ATD, such as ATD*#30#
*
*
*  RETURNS: CiReturnCode
*
*******************************************************************/
CiReturnCode ciMakeSS (UINT32 atHandle, const char *command_name_p)
{
	SS_STRUCT ssStruct;
	UINT32 len = strlen(command_name_p);
	char *ptr = (char *)command_name_p;
	UINT32 index = 0;
	char tmpStr[50];
	UINT32 i = 0;
	SS_CODE_ENUM sc;

	DBGMSG("%s:command_name:%s\n", __FUNCTION__, command_name_p);
	/* Invalid SS string: the normal SS string should end with '#' */
	if (command_name_p[len - 1] != '#')
		return CIRC_FAIL;

	memset(&ssStruct, 0, sizeof(ssStruct));
	if (command_name_p[0] == '*')
	{
		ptr++;
		if (command_name_p[1] == '*')
		{
			ptr++;
			ssStruct.op = SS_OP_REGISTER;
		}
		else if (command_name_p[1] == '#')
		{
			ptr++;
			ssStruct.op = SS_OP_INTERROGATE;
		}
		else
			ssStruct.op = SS_OP_ACTIVE;
	}
	else if (command_name_p[0] == '#')
	{
		ptr++;
		if (command_name_p[1] == '#')
		{
			ptr++;
			ssStruct.op = SS_OP_ERASE;
		}
		else
			ssStruct.op = SS_OP_DEACTIVE;
	}

	/* Get the service code  */
	while (*ptr && *ptr != '*' && *ptr != '#' )
	{
		tmpStr[i++] = *ptr++;
	}
	tmpStr[i] = '\0';

	sc = getSScode(tmpStr);

	/* Invalid SS CODE */
	if (sc == SS_CODE_NULL)
		return CIRC_FAIL;
	if (sc == SS_USSD)               //modified by Jiangang Jing
	{
		ssStruct.si1Len = strlen(command_name_p);
		memcpy( ssStruct.si1Str, command_name_p, ssStruct.si1Len);
		*ptr = '\0';
	}
	ssStruct.sc = sc;

	/* Get the SI */
	while (*ptr)
	{
		/* In normal case: the SS string should end with '#' */
		if (*ptr == '#')
		{
			ptr++;
			break;
		}
		ptr++;  //filter the '*'
		index++;
		if (index == 1)
		{
			while (*ptr && *ptr != '*' && *ptr != '#' )
				ssStruct.si1Str[ssStruct.si1Len++] = *ptr++;
		}
		else if (index == 2)
		{
			while (*ptr && *ptr != '*' && *ptr != '#' )
				ssStruct.si2Str[ssStruct.si2Len++] = *ptr++;
		}
		else if (index == 3)
		{
			while (*ptr && *ptr != '*' && *ptr != '#' )
				ssStruct.si3Str[ssStruct.si3Len++] = *ptr++;
		}
		else if (index == 4)
		{
			while (*ptr && *ptr != '*' && *ptr != '#' )
				ssStruct.si4Str[ssStruct.si4Len++] = *ptr++;
		}
		else
			break;
	}

	/* Invalid SS string */
	if (*ptr != '\0')
		return CIRC_FAIL;

	/*
	 * If SS string starts with a single '*', however, the SI1 is not NULL, we will
	 * consider it as REGISTER instead of ACTIVATE
	 */
	if (ssStruct.op == SS_OP_ACTIVE && ssStruct.si1Len != 0)
		ssStruct.op = SS_OP_REGISTER;


	return ciProcessSS(atHandle, &ssStruct);

}


/*******************************************************************
*  FUNCTION:  ciProcessSS
*
*  DESCRIPTION: To process call unrelated SS triggered by ATD, such as ATD*#30#
*
*
*  RETURNS: CiReturnCode
*
*******************************************************************/
CiReturnCode ciProcessSS (UINT32 atHandle, SS_STRUCT *ssStruct)
{
	CiReturnCode ret = CIRC_FAIL;

	DBGMSG("%s: ssStruct->sc = %d, op = %d, si1Len = %d, si2Len = %d, si3Len = %d, si4Len = %d.\n",
	       __FUNCTION__, ssStruct->sc, ssStruct->op, ssStruct->si1Len,
	       ssStruct->si2Len, ssStruct->si3Len, ssStruct->si4Len);

	switch (ssStruct->sc)
	{
	case SS_ALL_CF:
	case SS_ALL_COND_CF:
	case SS_CFU:
	case SS_CF_NOREPLY:
	case SS_CF_NOTREACHABLE:
	case SS_CFB:
	{
		CiSsCfRegisterInfo regInfo;
		CiSsCfReason reason = getCcfcReason(ssStruct->sc);
		AtciSsCcfcMode mode = getCcfcMode(ssStruct->op);
		UINT8 type = ATCI_DIAL_NUMBER_UNKNOWN;
		UINT8 callClass = CISS_BSMAP_NONE;
		UINT8 duration = 0;

		/* phone number type */
		if (ssStruct->si1Str[0] == '+')
			type = ATCI_DIAL_NUMBER_INTERNATIONAL;

		if (ssStruct->si2Len > 0)
			callClass = getCcfcClass((char *)ssStruct->si2Str);

		if (ssStruct->si3Len > 0)
			duration = getCcfcTime((char *)ssStruct->si3Str);

		switch (mode)
		{
		case ATCI_SS_MODE_DISABLE:
		case ATCI_SS_MODE_ENABLE:
		{
			ret = SS_SetCallForwardingStatus(atHandle, reason, callClass, (BOOL)mode);
			break;
		}

		case ATCI_SS_MODE_QUERY_STATUS:
		{
			ret = SS_GetCallForwardingSettings(atHandle, reason, callClass);
			break;
		}

		case ATCI_SS_MODE_REGISTER:
		{
			/*
			**  Fill in the CI request data for the forwarding number.
			*/
			regInfo.Reason        = (CiSsCfReason)reason;
			regInfo.Classes       = (CiSsBasicServiceMap)callClass;

			regInfo.Number.Length = ssStruct->si1Len;

			regInfo.Number.AddrType.NumType = DialNumTypeGSM2CI( type );
			regInfo.Number.AddrType.NumPlan = CI_NUMPLAN_E164_E163;                           //CI_NUMPLAN_UNKNOWN;

			memcpy( regInfo.Number.Digits, ssStruct->si1Str, ssStruct->si1Len );

			regInfo.OptSubaddr.Present = FALSE;
			regInfo.OptSubaddr.Length  = 0;

			/*
			**  Set the No-Reply timeout.
			*/
			regInfo.CFNRyDuration = duration;

			ret = SS_AddCallForwarding(atHandle, &regInfo);

			break;
		}

		case ATCI_SS_MODE_ERASE:
		{
			/*
			**  Allocate memory for the erase info.
			*/
			ret = SS_RemoveCallForwarding(atHandle, reason, callClass);
			break;
		}

		default:
			break;
		}          /* switch (modeParam) */

		break;
	}
	case SS_WAIT:
	{
		SS_OP_ENUM op;
		if(ssStruct->op == SS_OP_REGISTER)
			op = SS_OP_ACTIVE;
		else
			op = ssStruct->op;

		UINT8 mode = getCcfcMode(op);
		UINT8 callClass = CISS_BSMAP_NONE;

		if (ssStruct->si1Len != 0)
			callClass = getCcfcClass((char *)ssStruct->si1Str);

		if ( mode == CCWA_MODE_QUERY)
		{
			ret = SS_GetCallWaitingSettings(atHandle, (UINT32)callClass);
		}
		else if (mode == CCWA_MODE_DISABLE || mode == CCWA_MODE_ENABLE)
		{
			// process CW indication of modem
			gSSCntx.localCwOption = mode;

			ret = SS_SetCallWaitingStatus(atHandle, (UINT32)callClass, (mode == CCWA_MODE_ENABLE ) ? TRUE : FALSE);
		}

		break;
	}
	case SS_COLP:
		if (ssStruct->op == SS_OP_INTERROGATE)
			ret = SS_GetDialedIdSettings(atHandle);
		break;

	case SS_CLIR:
		if (ssStruct->op == SS_OP_INTERROGATE)
			ret = SS_GetHideIdSettings(atHandle);
		break;

	case SS_COLR:
		if (ssStruct->op == SS_OP_INTERROGATE)
			ret = SS_GetHideConnectedIdSettings(atHandle);
		break;
	case SS_CLIP:
		if (ssStruct->op == SS_OP_INTERROGATE)
			ret = SS_GetCallerIdSettings(atHandle);
		break;

	case SS_CNAP:
		if (ssStruct->op == SS_OP_INTERROGATE)
			ret = SS_GetNamePresentationSettings(atHandle);
		break;
		
	case SS_CPWD:
	{
		SS_CODE_ENUM sc;
		sc = getSScode((char *)ssStruct->si1Str);
		switch (sc)
		{
		case SS_BAIC:
		case SS_BAOC:
		case SS_BAOIC:
		case SS_BAOIC_EXCHOME:
		case SS_OUTG_BARRING:
		case SS_INC_BARRING:
		case SS_BAIC_ROAMING:
		case SS_ALL_BARRING:
			ret = SS_ChangeCallBarringPassword(atHandle, (CHAR*)ssStruct->si2Str, (CHAR*)ssStruct->si3Str,(CHAR*)ssStruct->si4Str);
			break;
		default:
			break;
		}
		break;
	}
	case SS_BAIC:                           //added by Jiangang Jing (35)
	case SS_BAOC:
	case SS_BAOIC:                          //added by Jiangang Jing(331)
	case SS_BAOIC_EXCHOME:                  //added by Jiangang Jing(332)
	case SS_OUTG_BARRING:                   //added by Jiangang Jing(333)
	case SS_BAIC_ROAMING:                   //added by Jiangang Jing(351)
	case SS_INC_BARRING:
	case SS_ALL_BARRING:                    //added by Jiangang Jing
	{
		UINT8 ciTypeId;
		UINT8 callClass = CISS_BSMAP_NONE;

		switch (ssStruct->sc)
		{
		case SS_BAIC:
			ciTypeId = CISS_CB_LOCK_BAIC;
			break;
		case SS_BAOC:
			ciTypeId = CISS_CB_LOCK_BAOC;
			break;
		case SS_BAOIC:
			ciTypeId = CISS_CB_LOCK_BOIC;
			break;
		case SS_BAOIC_EXCHOME:
			ciTypeId = CISS_CB_LOCK_BOIC_EXHC;
			break;
		case SS_OUTG_BARRING:
			ciTypeId = CISS_CB_LOCK_ALL_OUTGOING;
			break;
		case SS_INC_BARRING:
			ciTypeId = CISS_CB_LOCK_ALL_INCOMING;
			break;
		case SS_BAIC_ROAMING:
			ciTypeId = CISS_CB_LOCK_BAIC_ROAMING;
			break;
		default:
			ciTypeId = CISS_CB_LOCK_ALL;
			break;
		}

		if (ssStruct->si2Len > 0)
			callClass = getCcfcClass((char *)ssStruct->si2Str);

		if (ssStruct->op == SS_OP_INTERROGATE)
		{
			ret = SS_GetCallBarringStatus(atHandle, ciTypeId, callClass, NULL);
		}
		else if (ssStruct->op == SS_OP_ACTIVE || ssStruct->op == SS_OP_DEACTIVE || ssStruct->op == SS_OP_REGISTER)
		{
			int active = 0;
			if(ssStruct->op == SS_OP_DEACTIVE)
				active = 0;
			else
				active = 1;
			ret = SS_SetCallBarringStatus(atHandle, ciTypeId, callClass, (CHAR*)ssStruct->si1Str, active);
		}

		break;
	}
	case SS_USSD:
		if (ssStruct->op == SS_OP_REGISTER)
			ret = SS_SendSupServiceData(atHandle, CISS_USSD_CHARSET_MO_GSM7BIT, (const CHAR*)ssStruct->si1Str, ssStruct->si1Len);

		break;
	case SS_MRL_1:   /* This is a Marvell Private SS code*/
		ret = SS_SendMarvellPrivateCode(atHandle, (const CHAR*)ssStruct->si1Str, ssStruct->si1Len);
		/* Return OK to avoid block the channel. */
		if (ret == CIRC_FAIL)
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		else
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;

	default:
		DBGMSG("%s: the SC:%d is not supported now!\n", __FUNCTION__, ssStruct->sc);

		break;
	}

	return ret;
}

/************************************************************************************
 * F@: ciCLCK - GLOBAL API for AT+CCWE -command
 *
 */
RETURNCODE_T  ciCCWE(            const utlAtParameterOp_T op,
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
	INT32 ccweMode;

	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch ( op )
	{
	case TEL_EXT_GET_CMD:           /* AT+CCWE? */
	{
		ret = SS_GetCCWEMode(atHandle);
		break;
	}

	case TEL_EXT_TEST_CMD:               /* AT+CCWE=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CCWE= */
	{
		if ( getExtValue(parameter_values_p, 0, &ccweMode, TEL_AT_CCWE_0_CLASSX_VAL_MIN, TEL_AT_CCWE_0_CLASSX_VAL_MAX, TEL_AT_CCWE_0_CLASSX_VAL_DEFAULT ) == FALSE)
			break;
			ret = SS_SetCCWEMode(atHandle, ccweMode);
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
 * F@: ciLPLOCVR - GLOBAL API for AT+LPLOCVR -command
 *
 */
RETURNCODE_T  ciLPLOCVR(			 const utlAtParameterOp_T op,
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
	case TEL_EXT_SET_CMD:			   /* AT+LPLOCVR= */
	{
		INT32 taskid, mInvokeHandle, respresent, veriresp = 0;
		if ( getExtValue(parameter_values_p, 0, &taskid, 0, 256, 0 ) == FALSE)
			break;
		if ( getExtValue(parameter_values_p, 1, &mInvokeHandle, -65535, 65535, 0 ) == FALSE)
			break;
		if ( getExtValue(parameter_values_p, 2, &respresent, 0, 1, 0 ) == FALSE)
			break;
		if (respresent)
		{
			if ( getExtValue(parameter_values_p, 3, &veriresp, 0, 256, 0 ) == FALSE)
				break;
		}
		DPRINTF("[%s] taskid:%d, Invoke:%d, present:%d, veriresp:%d \r\n", 
								__FUNCTION__, taskid, mInvokeHandle, respresent, veriresp);
		SS_LocationVerificationRsp(atHandle, taskid, mInvokeHandle, respresent, veriresp);
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}
	case TEL_EXT_GET_CMD:			/* AT+LPLOCVR? */
	case TEL_EXT_TEST_CMD:			/* AT+LPLOCVR=? */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}

	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}
