/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************
*  Filename: telpb.c
*
*  Description: Process Phonebook related AT commands
*
*  History:
*   Jan 25, 2008 - Creation of file
*   Oct 10, 2008 - Optimization
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
#include "pb_api.h"
#include "telpb.h"

/************************************************************************************
 *
 * PBK related global variables and structure
 *
 *************************************************************************************/
unsigned short g_pbEntryStr[CI_MAX_NAME_LENGTH + ATCI_NULL_TERMINATOR_LENGTH];
UINT16 PBdelindex = 0 ;//Just work round!!
extern AtciCharacterSet chset_type;
/************************************************************************************
 *
 * PBK related AT commands Processing Function
 *
 *************************************************************************************/

/************************************************************************************
 * F@: ciSelectPBStorage - GLOBAL API for GCF AT+CPBS command
 *
 */
RETURNCODE_T  ciSelectPBStorage(   const utlAtParameterOp_T op,
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
	case TEL_EXT_TEST_CMD:         /* AT+CPBS=? */
	{
		ret = PB_QueryStorage(atHandle);
		break;
	}

	case TEL_EXT_GET_CMD:         /* AT+CPBS? */
	{
		ret = PB_GetStorage(atHandle);
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+CPBS=<storage>[,<password>] */
	{
		CHAR pbStorageStr[ATCI_PB_STORAGE_STR_LENGTH + ATCI_NULL_TERMINATOR_LENGTH];
		CHAR pbPasswordStr [ TEL_AT_CPBS_1_PASSWORD_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];
		int pbPasswordStrLen, pbStorageStrLen;

		if ( getExtString(parameter_values_p, 0, pbStorageStr,
				  ATCI_PB_STORAGE_STR_LENGTH, (INT16 *)&pbStorageStrLen, (CHAR *)TEL_AT_CPBS_0_STORAGE_STR_DEFAULT) == TRUE )
		{
			pbPasswordStrLen = 0;

			/* Password is required only in some cases */
			if ( (strcmp((char *)pbStorageStr, "FD") == 0) || (strcmp((char *)pbStorageStr, "ON") == 0))
			{
				if ( getExtString(parameter_values_p, 1, pbPasswordStr,
						  TEL_AT_CPBS_1_PASSWORD_STR_MAX_LEN, (INT16 *)&pbPasswordStrLen, TEL_AT_CPBS_1_PASSWORD_STR_DEFAULT) == FALSE)
				{
					ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_ALLOWED, NULL);
					break;
				}
			}

			ret = PB_SetStorage(atHandle, (char *)pbStorageStr, (char *)pbPasswordStr, pbPasswordStrLen);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_ALLOWED, NULL);
		}

		break;
	}

	default:         /* AT+CPBS */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_ALLOWED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 * F@: ciReadPB - GLOBAL API for GCF AT+CPBR command
 *
 */
RETURNCODE_T  ciReadPB(            const utlAtParameterOp_T op,
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
	case TEL_EXT_TEST_CMD:         /* AT+CPBR=? */
	{
		ret = PB_QueryRead(atHandle);
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+CPBR= */
	{
			INT32 index1, index2;
			if ( getExtValue( parameter_values_p, 0, &index1, TEL_AT_PB_INDEX_VAL_MIN, TEL_AT_PB_INDEX_VAL_MAX, TEL_AT_PB_INDEX_VAL_DEFAULT ) == TRUE )
			{
				/* If index2 is valid, record the arrange to display. And send next +CPBR after receive CI CNF msg */
				index2 = 0;
				if ((index1 != ATCI_PB_INVALID_INDEX) && (getExtValue( parameter_values_p, 1, &index2, TEL_AT_PB_INDEX_VAL_MIN, TEL_AT_PB_INDEX_VAL_MAX, ATCI_PB_INVALID_INDEX ) == TRUE) )
				{
					if ( (index2 == ATCI_PB_INVALID_INDEX) || (index2 <= index1) )
					{
						index2 = 0;
					}
					/* Send AT Command to read entry in index1 */
					ret = PB_ReadEntryStart(atHandle, index1, index2);
				}
				else
				{
					ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_ALLOWED, NULL);
				}

			}
			/* If Index1 is invalid */
			else
			{
				ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_ALLOWED, NULL);
			}
		break;
	}

	default:         /* AT+CPBR, AT+CPBR? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_ALLOWED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);

	return(rc);

}

/************************************************************************************
 * F@: ciWritePB - GLOBAL API for GCF AT+CPBW command
 *
 */
RETURNCODE_T  ciWritePB(            const utlAtParameterOp_T op,
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
	case TEL_EXT_TEST_CMD:         /* AT+CPBW=? */
	{
		ret = PB_QueryWrite(atHandle);
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+CPBW=[<index>][,<number>[,<type>[,<text>[,<group>[,<adnumber>[,<adtype>[,<secondtext>[,<email>[,<sip_uri>[,<tel_uri>[,<hidden>]]]]]]]]]]] */
	{
		BOOL deleteEntry = FALSE;
		int index, type;
		INT16 numDigits, txtLen;
		CHAR number[CI_MAX_ADDRESS_LENGTH + ATCI_NULL_TERMINATOR_LENGTH];
		CHAR text[CI_MAX_NAME_LENGTH * 2 + ATCI_NULL_TERMINATOR_LENGTH];
		BOOL cmdValid = FALSE;
		int name_max_len = 0;
		/* Get the index to be written. If index is omitted, the record will be written to first available location  */
		index = ATCI_PB_INVALID_INDEX;
		if ( getExtValue( parameter_values_p, 0, &index, TEL_AT_PB_INDEX_VAL_MIN, TEL_AT_PB_INDEX_VAL_MAX, TEL_AT_PB_INDEX_VAL_DEFAULT ) == TRUE )
		{
			/* Get number, if no number inputted, this entry will be removed from PBK */
			if ( getExtString(parameter_values_p, 1, number,
					  CI_MAX_ADDRESS_LENGTH, &numDigits, NULL) == TRUE )
			{
				if ( numDigits == 0 )
				{
					deleteEntry = TRUE;
				}

				/* Get type of number */
				if ( getExtValue( parameter_values_p, 2, &type, ATCI_DIAL_NUMBER_UNKNOWN, ATCI_DIAL_NUMBER_INVALID, ATCI_DIAL_NUMBER_UNKNOWN ) == TRUE )
				{
					if ( type == ATCI_DIAL_NUMBER_INVALID )
					{
						if ( number[0] == CI_INTERNATIONAL_PREFIX )
						{
							type = ATCI_DIAL_NUMBER_INTERNATIONAL;
						}
						else
						{
							type = ATCI_DIAL_NUMBER_UNKNOWN;
						}
					}

					if(chset_type == ATCI_CHSET_UCS2)
					{
						name_max_len = CI_MAX_NAME_LENGTH * 2;
					}
					else
					{
						name_max_len = CI_MAX_NAME_LENGTH;
					}


					/* Get text string  associated with the numbe */
					if ( getExtString(parameter_values_p, 3, text,
							  name_max_len, &txtLen, NULL) == TRUE )
					{
						if ( txtLen == 0 )
						{
							deleteEntry = TRUE;
						}
						cmdValid = TRUE;
					}
				}
			}
		}
		if(!cmdValid)
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_ALLOWED, NULL);
			break;
		}
		/* If mandatory arguments are not supplied - delete entry */
		if ( deleteEntry )
		{
			if ( index != ATCI_PB_INVALID_INDEX )
			{
				PBdelindex = index;
				ret = PB_DeleteEntry(atHandle, index);
			}
		}

		/* Write record to PBK */
		else
		{
			ret = PB_WriteEntry(atHandle, index, (char *)number, numDigits, type, (char *)text, txtLen);
		}
		break;
	}

	default:         /* AT+CPBW, AT+CPBW? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_ALLOWED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}
/************************** Added by Michal Bukai ***********************************/
/************************************************************************************
 * F@: ciFindPBEntries - GLOBAL API for GCF AT+CPBF command
 *
 */
RETURNCODE_T  ciFindPBEntries(            const utlAtParameterOp_T op,
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
	INT16 tmpLen;

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
	case TEL_EXT_TEST_CMD:         /* AT+CPBF=? */
	{
		ret = ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}
	case TEL_EXT_SET_CMD:         /* AT+CPBF= */
	{
		int max_len = 0;
		char text[CI_MAX_NAME_LENGTH * 2 + 1];
		if(chset_type == ATCI_CHSET_UCS2 || chset_type == ATCI_CHSET_HEX)
		{
			max_len = CI_MAX_NAME_LENGTH * 2;
		}
		else
		{
			max_len = CI_MAX_NAME_LENGTH;
		}

		if ( getExtString(parameter_values_p, 0, (CHAR *)text, max_len, &tmpLen, NULL) == TRUE )
		{
			int len;
			if(chset_type == ATCI_CHSET_HEX)
			{
				char hex[CI_MAX_NAME_LENGTH + 1] = {0};
				len = libConvertCSCSStringToHEX(text, tmpLen, chset_type, hex, sizeof(hex) / sizeof(hex[0]) - 1);
				memset(g_pbEntryStr, 0, sizeof(g_pbEntryStr));
				if(len > 0)
				{
					pb_decode_alpha_tag((unsigned char *)hex, len, g_pbEntryStr, sizeof(g_pbEntryStr) / sizeof(g_pbEntryStr[0]) - 1);
				}
				else
				{
					ret = ATRESP(atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_ALLOWED, NULL);
					break;
				}
			}
			else
			{
				memset(g_pbEntryStr, 0, sizeof(g_pbEntryStr));
				len = libConvertCSCSStringToUCS2(text, tmpLen, chset_type, g_pbEntryStr, sizeof(g_pbEntryStr) / sizeof(g_pbEntryStr[0]) - 1);
				if(len <= 0)
				{
					ret = ATRESP(atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_ALLOWED, NULL);
					break;
				}
			}

			/* currently last two parameters are not used, so just pass NULL and 0 */
			ret = PB_FindPBEntry(atHandle, NULL, 0);
		}
		else
			ret = ATRESP(atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_ALLOWED, NULL);
		break;
	}
	case TEL_EXT_GET_CMD:           /* AT+CPBF? */
	default:                        /* AT+CPBF */
	{
		ret = ATRESP(atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_ALLOWED, NULL);
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);

	return(rc);
}

