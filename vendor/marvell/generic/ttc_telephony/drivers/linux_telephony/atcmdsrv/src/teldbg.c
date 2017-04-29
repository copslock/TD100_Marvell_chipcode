/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

#include "ci_api_types.h"
#include "ci_api.h"
#include "teldef.h"
#include "telconfig.h"
#include "telatci.h"
#include "telatparamdef.h"
#include "telutl.h"
#include <stdio.h>
#include <teldef.h>
#include "eeh_assert_notify.h"
#if defined (BIONIC)
#include <cutils/properties.h>
#endif

extern CiShHandle gAtciShellHandle;
extern CiShHandle gAtciShReqHandle;
CiServiceHandle gAtciSvgHandle[CI_SG_NUMIDS + 1];
extern int loglevel;
AtciCharacterSet chset_type = ATCI_CHSET_IRA;
extern void ciClientStubDeinit(void);

#define MAX_DUMP_LENGTH   160

void dbg_dump_buffer(char* name, char* buf, int len)
{
	int i;
	char dbgBuf[TEL_AT_CMDSTR_MAX_LEN], tempBuf[10];;

	sprintf(dbgBuf, "\nDump \"%s\" Buffer [len=%d]: \n", name, len);
	for (i = 0; i < len && i < MAX_DUMP_LENGTH; i++)
	{
		sprintf(tempBuf, "%02X", buf[i]);
		strcat(dbgBuf, tempBuf);
		if (((i + 1) % 32) == 0) strcat(dbgBuf, "\n");
	}
	strcat(dbgBuf, "\n");
	eeh_log(LOG_DEBUG, "%s", dbgBuf);
}

void dbg_dump_string(char* name, char* buf, int len)
{
	int i;
	char dbgBuf[TEL_AT_CMDSTR_MAX_LEN], tempBuf[10];;

	sprintf(dbgBuf, "Dump \"%s\" Buffer [len=%d]: \n", name, len);
	for (i = 0; i < len && i < MAX_DUMP_LENGTH; i++)
	{
		sprintf(tempBuf, "%c", buf[i]);
		strcat(dbgBuf, tempBuf);
		if (((i + 1) % 32) == 0) strcat(dbgBuf, "\n");
	}
	strcat(dbgBuf, "\n");
	eeh_log(LOG_NOTICE, "%s", dbgBuf);

}

/******************************************************************************
*    Debugging AT Command
******************************************************************************/


/*******************************************************************
*  FUNCTION: vHDL
*
*  DESCRIPTION: Get Handlle value for debugging
*
*  PARAMETERS:
*
*  RETURNS:
*
*******************************************************************/
utlReturnCode_T  vHDL( const utlAtParameterOp_T op,
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

	_CiServiceGroupId ciSvcGrpId;
	*xid_p = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	if ( op == utlAT_PARAMETER_OP_GET)
		DPRINTF("this is get.\n");
	else if ( op == utlAT_PARAMETER_OP_SET)
		DPRINTF("this is a set.\n");


	for ( ciSvcGrpId = CI_SG_ID_CC; ciSvcGrpId <= CI_SG_NUMIDS; ciSvcGrpId++ )
	{
		DPRINTF("ShellHandle[%d]: %d \r\n", ciSvcGrpId, gAtciSvgHandle[ciSvcGrpId]);
	}

	ATRESP(*xid_p, ATCI_RESULT_CODE_OK, 0, NULL);
	return utlSUCCESS;

}

/*******************************************************************
*  FUNCTION: vECHO
*
*  DESCRIPTION: echo back whatever type in the first arg
*
*  PARAMETERS:
*
*  RETURNS:
*
*******************************************************************/
utlReturnCode_T  vECHO( const utlAtParameterOp_T op,
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
	int i;
	char tmp[255];

	*xid_p = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	sprintf(tmp, "%s", parameter_values_p[0].value.string_p);

	/* convert '-' to ',' */
	for (i = 0; i < (int)strlen(tmp); i++)
	{
		if (tmp[i] == '-')
			tmp[i] = ',';
	}

	ATRESP( *xid_p, ATCI_RESULT_CODE_OK, 0, tmp);

	return utlSUCCESS;

}


/* debugging function for linux */

/******************************************************************************
*    Command not implemented (faking for Qtopia Integration
******************************************************************************/

utlReturnCode_T  ciCSCS( const utlAtParameterOp_T op,
			 const char                      *command_name_p,
			 const utlAtParameterValue_P2c parameter_values_p,
			 const size_t num_parameters,
			 const char                      *info_text_p,
			 unsigned int                    *xid_p,
			 void                            *arg_p)
{
	RETURNCODE_T      rc = INITIAL_RETURN_CODE;
	CiReturnCode      ret = CIRC_FAIL;

	UNUSEDPARAM(op)
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(parameter_values_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)
	/*
	 *  Put parser index into the variable
	 */
	CiRequestHandle atHandle;
	TelAtParserID sAtpIndex;
	sAtpIndex = *(TelAtParserID *)arg_p;
	atHandle = MAKE_AT_HANDLE( sAtpIndex );
	*xid_p = atHandle;

	/*
	 * process operation
	 */
	switch(op)
	{
		case TEL_EXT_SET_CMD:      /* AT+CSCS= */
			{
				char chset[10];
				INT16 len;

				if( getExtString( parameter_values_p, 0, (CHAR *)chset, 9, &len, NULL ) == TRUE )
				{
					if(strcmp(chset, "IRA") == 0){
						chset_type = ATCI_CHSET_IRA;
						ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,NULL);
					}
					else if(strcmp(chset, "UCS2") == 0){
						chset_type = ATCI_CHSET_UCS2;
						ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,NULL);
					}
					else if(strcmp(chset, "HEX") == 0){
						chset_type = ATCI_CHSET_HEX;
						ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,NULL);
					}
					else
						ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
				}
				else
					ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);

				break;
			}

		case TEL_EXT_TEST_CMD:    /* AT+CSCS=? */
			{
				ret = ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CSCS: (\"IRA\",\"UCS2\",\"HEX\")\r\n");
				break;
			}
		case TEL_EXT_GET_CMD:      /* AT+CSCS?  */
			{
				if(chset_type == ATCI_CHSET_IRA)
					ret = ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CSCS: \"IRA\"\r\n");
				else if(chset_type == ATCI_CHSET_UCS2)
					ret = ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CSCS: \"UCS2\"\r\n");
				else if(chset_type == ATCI_CHSET_HEX)
					ret = ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CSCS: \"HEX\"\r\n");
				else
					ret = ATRESP( atHandle,ATCI_RESULT_CODE_ERROR,0,NULL);
				
				break;
			}

		default:
			ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_OPERATION_NOT_SUPPORTED,NULL);
			break;
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}

/************************************************************************************
 * F@: cciReg - GLOBAL API for GCF AT*CCIREG command
 *
 */
RETURNCODE_T  cciReg(            const utlAtParameterOp_T op,
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
	UNUSEDPARAM(xid_p)
	UNUSEDPARAM(arg_p)
	/*
	**  Set the result code to INITIAL_RETURN_CODE.  This allows
	**  the indications to display the correct return code after the
	**  AT Command is issued.
	*/
	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	BOOL cmdValid = FALSE;
	UINT32 regState;
	UINT8 sgId;
	static CiShOperDeregisterDefaultSgReq gDefDeregBuf;
	static CiShOperDeregisterSgReq gSvgDeregBuf;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	*xid_p = atHandle;

	/*
	 * process operation
	 */
	switch ( op )
	{
	case TEL_EXT_TEST_CMD:         /* AT*CCIREG=? */
	{
		/* No CI primitive for this: print hard-coded reply */
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"*CCIREG: 0" );
		cmdValid = TRUE;
		break;
	}
	/*
	case TEL_EXT_GET_CMD:         // AT*CCIREG? 
	{
		// return CCI REg status
		cmdValid = TRUE;
		break;
	}
	*/
	case TEL_EXT_SET_CMD:         /* AT*CCIREG= */
	{
		if ( getExtValue( parameter_values_p, 0, (int *)&regState, TEL_AT_CCIREG_0_N_VAL_MIN, TEL_AT_CCIREG_0_N_VAL_MAX, TEL_AT_CCIREG_0_N_VAL_DEFAULT ) == TRUE )
		{
			if (regState == 0)
			{
				DPRINTF("DeRegister the default service group\n");
				/*  DeRegister the default service group.  */
				gDefDeregBuf.defHandle = gAtciShellHandle;

				rc = ciShRequest( gAtciShellHandle,                             /* Shell Handle */
						  CI_SH_OPER_DEREGISTERDEFSG,                   /* Operation */
						  &gDefDeregBuf,                                /* parameters */
						  gAtciShReqHandle++ );                         /* Opaque handle */

				DPRINTF("DeRegister the service groups\n");
				// deregister CCI service groups
				for (sgId = CI_SG_ID_CC; sgId < CI_SG_NUMIDS; sgId++)
				{
					gSvgDeregBuf.handle = gAtciSvgHandle[sgId];
					rc = ciShRequest( gAtciShellHandle,                             /* Shell Handle */
							  CI_SH_OPER_DEREGISTERSG,                      /* Operation */
							  &gSvgDeregBuf,                                /* parameters */
							  gAtciShReqHandle++ );                         /* Opaque handle */
				}
				DPRINTF("DeRegister the shell\n");
				// deregister shell
				rc = ciShDeregisterReq(gAtciShellHandle, gAtciShReqHandle++);
				sleep(1000);
				DPRINTF("Deinit ci client\n");
				ciClientStubDeinit();
			}
			else
			{
				DPRINTF("to be implemented!!!\n");
			}
			cmdValid = TRUE;
		}
		break;
	}

	default:
	{
		break;
	}
	}

	if ( !cmdValid )
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
	}
	else
		rc = utlSUCCESS;



	return(rc);

}

/*******************************************************************
*  FUNCTION: vDump
*
*  DESCRIPTION: Used to dump the loglevel
*
*  PARAMETERS:
*
*  RETURNS:
*
*******************************************************************/
utlReturnCode_T  vDump( const utlAtParameterOp_T op,
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

	TelAtParserID sAtpIndex;
	sAtpIndex =  *(TelAtParserID *)arg_p;
	*xid_p = MAKE_AT_HANDLE(sAtpIndex);
	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	int level;
#if defined (BIONIC)
	char levelbuf[PROPERTY_VALUE_MAX] = {0};
#endif
	switch ( op )
	{
	case TEL_EXT_GET_CMD:         /* AT+VDUMP? */
	{
		char buf[50];
		sprintf(buf, "+VDUMP: %d", loglevel);
		rc = ATRESP(*xid_p, ATCI_RESULT_CODE_OK, 0, buf);
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+VDUMP= */
	{
		if ( getExtValue( parameter_values_p, 0, &level, TELEPHONY_MODULE_LOG_SILENT, TELEPHONY_MODULE_LOG_DEBUG, TELEPHONY_MODULE_LOG_NOTICE) == TRUE )
		{
		#if defined (BIONIC)
			levelbuf[0] = level + '0';
			property_set(TELEPHONY_MODULE_LOG_LEVEL_PROPERTY, levelbuf);
		#endif
			loglevel = level;
			rc = ATRESP(*xid_p, ATCI_RESULT_CODE_OK, 0, NULL);
		}
		break;
	}
	default:
		break;
	}

	return rc;
}

/* End Fake commands for Qtopia Integration*/

