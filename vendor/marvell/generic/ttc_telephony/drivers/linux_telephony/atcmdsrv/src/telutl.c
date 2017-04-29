/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
 *  INTEL CONFIDENTIAL
 *  Copyright 2006 Intel Corporation All Rights Reserved.
 *  The source code contained or described herein and all documents related to the source code (“Material? are owned
 *  by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
 *  its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
 *  Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
 *  treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual property right is granted to or
 *  conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
 *  estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
 *  Intel in writing.
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: telutl.c
 *
 *  Description: utility functions for Telephony Controller
 *
 *  History:
 *   May 19, 2006 - Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/


/** include files **/

#include "telutl.h"
#include <stdlib.h>
#include <string.h>             //strlen()
#include <ci_ss.h>              //CISS_BSMAP_VOICE
#include "telatparamdef.h"      //TEL_AT_CMDSTR_MAX_LEN



/*******************************************************************
*  FUNCTION:  HexToBin
*
*  DESCRIPTION: convert a hexadecimal to integer
*
*  RETURNS: TRUE or FALSE
*
*******************************************************************/
BOOL HexToBin (const char * pcInput, int *piOutput, int maxLen)
{

	int i, iResult = 0;
	BOOL bSuccess = TRUE;
	unsigned char ucVal = 0;
	char character = 0;

	if (*pcInput == 0)
	{
		bSuccess = FALSE;
		return bSuccess;
	}
	else
	{
		for (i = 0; (i < maxLen) && (bSuccess == TRUE) && (*pcInput !=  '\0'); i++, pcInput++)
		{
			character = *pcInput;

			/* if it is lower case,  convert to upper case  first*/
			if ((character >= 'a') && (character <= 'f'))
			{
				character = character -  ('a' - 'A');
			}

			if ((character >= 'A') && (character <= 'F'))
			{
				ucVal = character - 'A' + 10;
				iResult = (iResult << 4 ) + ucVal;
			}
			else if ((character >= '0') && (character <= '9'))
			{
				ucVal = character - '0';
				iResult = (iResult << 4 ) + ucVal;
			}
			else
			{
				bSuccess = FALSE;
			}
		}

		if (*pcInput != 0)
		{
			bSuccess = FALSE;
		}
	}

	*piOutput = iResult;

	return (bSuccess);
}

/*******************************************************************
*  FUNCTION:  checkForNumericOnlyChars
*
*  DESCRIPTION: check the given string, validate it only contains numeric
*
*  RETURNS: TRUE or FALSE
*
*******************************************************************/
BOOL checkForNumericOnlyChars(const CHAR *password)
{
	INT16 passLen = 0;
	INT8 count = 0;
	BOOL result = TRUE;

	passLen = strlen((const char*)password);
	if (passLen > 0)
	{
		/* Chech each digit */
		while ((result == TRUE) && (count < passLen))
		{
			if ((password[count] < '0') || (password[count] > '9'))
			{
				result = FALSE;
			}

			count++;
		}
	}

	return (result);
}


BOOL getExtendedParameter (INT32 inValue,
			   INT32 *value_p,
			   INT32 DefaultValue)
{

	if ( inValue > DefaultValue )
		*value_p = DefaultValue;
	else
		*value_p = inValue;

	return ( TRUE );
}

/*******************************************************************
*  FUNCTION:  getExtValue
*
*  DESCRIPTION:
*
*  RETURNS: TRUE or FALSE
*
*******************************************************************/
BOOL getExtValue( const utlAtParameterValue_P2c param_value_p,
		  int index,
		  int *value_p,
		  int minValue,
		  int maxValue,
		  int DefaultValue)
{
	int inValue;

	/* check if param_value_p is okay*/
	//if (param_value_p[index] == NULL )
	//  return FALSE;

	/* check if it uses default values */
	if (param_value_p[index].is_default)
	{
		*value_p = DefaultValue;
		return TRUE;
	}

	inValue = param_value_p[index].value.decimal;

	/* check if it is within Range */
	if ( inValue > maxValue || inValue < minValue )
	{
		return FALSE;
	}
	else
	{
		*value_p = inValue;
	}

	return ( TRUE );
}

/*******************************************************************
*  FUNCTION:  getExtString
*
*  DESCRIPTION:
*
*  RETURNS: TRUE or FALSE
*
*******************************************************************/
BOOL getExtString ( const utlAtParameterValue_P2c param_value_p,
		    int index,
		    CHAR *outString,
		    INT16 maxStringLength,
		    INT16 *outStringLength,
		    CHAR *defaultString)
{
	CHAR inString[TEL_AT_CMDSTR_MAX_LEN];
	INT16 length = 0;

	*outString = (CHAR)0;
	*outStringLength = 0;

	/* check if it uses default values */
	if (param_value_p[index].is_default )
	{
		if ( defaultString != NULL )
		{

			strcpy((char *)outString, (char *)defaultString);

			*outStringLength = strlen((char *)defaultString);

		}
		else
		{
			*outStringLength = 0;
		}
		return TRUE;
	}

	strcpy((char *)inString, param_value_p[index].value.string_p);

	length = strlen( (char *)inString );

	if (length >     maxStringLength)
	{
		DPRINTF("Error In String: %s\n\n", inString);
		return FALSE;
	}
	strcpy((char *)outString, (char *)inString);
	*outStringLength = length;

	return ( TRUE );
}


BOOL getExtendedString ( CHAR *inString,
			 CHAR *outString,
			 INT16 maxStringLength,
			 INT16 *outStringLength)
{
	INT16 length = 0;

	*outString = (CHAR)0;
	*outStringLength = 0;

	length = strlen( (char *)inString );

	if (length >     maxStringLength)
	{
		DPRINTF("Error In String: %s\n\n", inString);
		return FALSE;
	}

	strcpy((char *)outString, (char *)inString);
	*outStringLength = length;

	return ( TRUE );
}


/*******************************************************************
*  FUNCTION:  atParamToCiEnum
*
*  DESCRIPTION: Convert Input parameter from AT parser to CI Enum value
*               according to the Lookup Table
*
*  RETURNS: TRUE or FALSE
*
*******************************************************************/
BOOL atParamToCiEnum (const utlAtParameterValue_P2c param_value_p,
		      int index,
		      int *param_p,
		      int DefaultValue,
		      EnumParamLookup *enumParamLookup_p)
{
	int i = 0;
	BOOL result = FALSE;
	int tmpVal;

	/* check if it uses default values */
	if (param_value_p[index].is_default)
	{
		tmpVal = DefaultValue;

	}
	else
	{
		tmpVal = param_value_p[index].value.decimal;
	}
	/* search for the value in lookup table.... */
	while ( (enumParamLookup_p[i].atParam != CI_PS_3G_ENUM_LOOKUP_END_ELEMENT) &&
		(result == FALSE))
	{
		if (enumParamLookup_p[i].atParam == tmpVal)
		{
			/* set the value to Ci Enum Value*/
			*param_p = enumParamLookup_p[i].bgParam;
			result = TRUE;
			break;
		}
		else
		{
			i++;
		}
	}

	return (result);
}


/*--------------------------------------------------------------------------
*
* Function:    getErrorRatioParam
*
* Parameters:  errRatioStr        - error Ratio String
*              errRatioLookup_p -  Enum lookup table of valid params
*              defaultValue           - Default value to use if no param
*              ratio_p                - Enum lookup value to use
*
* Returns:     TRUE if okay, FALSE otherwise
*
* Description: Converts error rate from mEe format to enum value.
*-------------------------------------------------------------------------*/
BOOL getErrorRatioParam ( const utlAtParameterValue_P2c param_value_p,
			  int index,
			  ErrorRatioLookup    *errorRatioLookup_p,
			  INT32 defaultValue,
			  INT32               *ratio_p)
{
	BOOL result            = TRUE;
	BOOL foundE            = FALSE;
	BOOL matchFound        = FALSE;
	INT16 mantissa          = 0;
	INT16 exponent          = 0;
	INT16 count             = 0;
	INT16 errorRatioStringLen;
	CHAR errorRatioString[ERROR_RATIO_STRING_LEN + ATCI_NULL_TERMINATOR_LENGTH];


	*ratio_p = SUBSCRIBED_VALUE;

	/* Fetch string from command line.... */
	if (getExtString(param_value_p, index++, errorRatioString, ERROR_RATIO_STRING_LEN, &errorRatioStringLen, NULL) == TRUE)
	{
		if (errorRatioStringLen > 0)
		{
			/* Split string into mantissa and exponent.... */
			while ((count < errorRatioStringLen) && (result == TRUE))
			{
				switch (errorRatioString[count])
				{
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				{
					/* Work out if we're processing mantissa or exponent.... */
					if (foundE == FALSE)
					{
						/* Mantissa.... */
						mantissa *= 10;
						mantissa += errorRatioString[count] - '0';
					}
					else
					{
						/* Exponent.... */
						exponent *= 10;
						exponent += errorRatioString[count] - '0';
					}
					break;
				}
				case 'E':
				{
					/* Check only a single 'E' in string.... */
					if (foundE == FALSE)
					{
						foundE = TRUE;
					}
					else
					{
						result = FALSE;
					}
					break;
				}
				default:
				{
					result = FALSE;
					break;
				}
				}
				count++;
			}

			/* If result okay, lookup correct enum value to return for supplied
			 * values....
			 */
			count = 0;
			while ((errorRatioLookup_p[count].mantissa != CI_PS_3G_ENUM_LOOKUP_END_ELEMENT) &&
			       (errorRatioLookup_p[count].exponent != CI_PS_3G_ENUM_LOOKUP_END_ELEMENT) &&
			       (matchFound == FALSE))
			{
				if ((errorRatioLookup_p[count].mantissa == mantissa) &&
				    (errorRatioLookup_p[count].exponent == exponent))
				{
					matchFound = TRUE;
					*ratio_p = errorRatioLookup_p[count].enumValue;
				}
				count++;
			}

			/* Did we find value? */
			if (matchFound == FALSE)
			{
				result = FALSE;
			}
		}
		else
		{
			result = TRUE;
			*ratio_p = defaultValue;
		}
	}
	else
	{
		result = FALSE;
	}

	return (result);
}


/********************************************************
 *  F@: operStringToPlmn - GLOBAL API for GCF AT-command
 *
 */
BOOL operStringToPlmn( CHAR *plmnString_p, Plmn *convertedPlmn, INT16 plmnStringLen )
{
	INT32 val;
	BOOL success = TRUE;
	INT16 len  = 0;
	CHAR plmnString[PLMN_NAME_FULL_LENGTH + 1] = {0};    /* Max possible length */
	if((plmnString_p == NULL) || (convertedPlmn == NULL))
		return FALSE;
	/* Initialise string */
	len = (plmnStringLen < (INT16)(PLMN_NAME_FULL_LENGTH*sizeof(CHAR)) ? plmnStringLen : (INT16)(PLMN_NAME_FULL_LENGTH*sizeof(CHAR)));
	memcpy(plmnString, plmnString_p, len);
	plmnString[len] = '\0';
	/* check length is valid */
	if ((len < (MCC_DIGIT_LENGTH + MNC_DIGIT_LENGTH_MIN)) ||
	    (len > (MCC_DIGIT_LENGTH + MNC_DIGIT_LENGTH_MAX)))
	{
		success = FALSE;
	}
	else
	{
		/* get MNC value */
		//pelease double check the max len inside HexToBin.
		if (HexToBin((char *)&plmnString[MCC_DIGIT_LENGTH], &val, MNC_DIGIT_LENGTH_MAX) == FALSE)
		{
			success = FALSE;
		}
		else
		{
			/* record MNC */
			convertedPlmn->mnc = (Mnc)val;

			/* null terminate string after MCC */
			plmnString[MCC_DIGIT_LENGTH] = NULL_CHAR;

			/* get MCC value */
			if (HexToBin((char *)plmnString, &val, MCC_DIGIT_LENGTH) == FALSE)
			{
				success = FALSE;
			}
			else
			{
				/* record MCC */
				convertedPlmn->mcc = (Mcc)val;
			}
		}
	}

	return (success);
}


/****************************************************************
 * isValidCiSsClass - as per spec. TS 27.007
 */
BOOL isValidCiSsClass( UINT16 *CiSsClass )
{
	BOOL result;

	switch (*CiSsClass)
	{
	case CISS_BSMAP_VOICE:
	case CISS_BSMAP_DATA:
	case CISS_BSMAP_FAX:
	/* Combined classes.... */
	case (CISS_BSMAP_SMS | CISS_BSMAP_FAX | CISS_BSMAP_VOICE):
	case (CISS_BSMAP_SMS | CISS_BSMAP_FAX):
	case (CISS_BSMAP_DATA_ASYNC | CISS_BSMAP_DATA_SYNC):
	case (CISS_BSMAP_DATA_SYNC | CISS_BSMAP_VOICE):
	case (CISS_BSMAP_VOICE | CISS_BSMAP_DATA):
	case (CISS_BSMAP_FAX | CISS_BSMAP_VOICE):
	case (CISS_BSMAP_FAX | CISS_BSMAP_DATA):
	case (CISS_BSMAP_PACKET | CISS_BSMAP_DATA_SYNC):
	case (CISS_BSMAP_PAD | CISS_BSMAP_DATA_ASYNC):
	case (CISS_BSMAP_VOICE | CISS_BSMAP_DATA | CISS_BSMAP_FAX | CISS_BSMAP_SMS):
	case CISS_BSMAP_DEFAULT:
	{
		/* Valid input */
		result = TRUE;
		break;
	}

	case CISS_BSMAP_SMS:
	case CISS_BSMAP_DATA_SYNC:
	case CISS_BSMAP_DATA_ASYNC:
	case CISS_BSMAP_PACKET:
	case CISS_BSMAP_PAD:
	{
		/* Valid input */
		result = TRUE;
		break;
	}

	case ATCI_SS_BS_EXT_ALL_SERVICES:
	{
		*CiSsClass = CISS_BSMAP_ALL;
		result = TRUE;
		break;
	}
	
	default:
	{
		/* Invalid input.... */
		result = FALSE;
		break;
	}
	}

	return (result);
}


/**************************************************
 * DialNumTypeGSM2CI - convert dial number type from
 *   3GPP type to CI enum type. (see 3GPP 24.08)
 */
CiAddrNumType DialNumTypeGSM2CI( INT32 GSMType )
{
	CiAddrNumType CiType;

	switch (GSMType)
	{
	case ATCI_DIAL_NUMBER_INTERNATIONAL:
		CiType = CI_NUMTYPE_INTERNATIONAL;
		break;
	case ATCI_DIAL_NUMBER_NATIONAL:
		CiType = CI_NUMTYPE_NATIONAL;
		break;
	case ATCI_DIAL_NUMBER_NET_SPECIFIC:
		CiType = CI_NUMTYPE_NETWORK;
		break;
	case ATCI_DIAL_NUMBER_UNKNOWN:
	default:
		CiType = CI_NUMTYPE_UNKNOWN;
		break;
	}

	return CiType;
}

/**************************************************
 * DialNumTypeCI2GSM - convert dial number type from
 *   CI enum type to 3GPP type  (see 3GPP 24.08).
 * Added by Rovin Yu
 */
INT32 DialNumTypeCI2GSM( CiAddrNumType CiType)
{
	INT32 GsmType;

	switch (CiType)
	{
	case CI_NUMTYPE_INTERNATIONAL:
		GsmType = ATCI_DIAL_NUMBER_INTERNATIONAL;
		break;
	case CI_NUMTYPE_NATIONAL:
		GsmType = ATCI_DIAL_NUMBER_NATIONAL;
		break;
	case CI_NUMTYPE_NETWORK:
		GsmType = ATCI_DIAL_NUMBER_NET_SPECIFIC;
		break;
	default:
		GsmType = ATCI_DIAL_NUMBER_UNKNOWN;
		break;
	}

	return GsmType;
}

/****************************************************************
 *  F@: string2BCD - GLOBAL API to convert string to BCD
 *
 */
void string2BCD(UINT8* pp, char *tempBuf, UINT8 length)
{
	int i;
	UINT8 lo_nibble, hi_nibble;

	for (i = 0; i < length; i += 2)
	{
		lo_nibble = tempBuf[i + 0];
		hi_nibble = tempBuf[i + 1];
		if ((i + 1) == length)
		{
			hi_nibble = 0x3f;
			lo_nibble = tempBuf[i + 0];
		}
		if ((lo_nibble >= 0x30) && (lo_nibble <= 0x39))
			lo_nibble -= 0x30;
		else lo_nibble = 0x0f;
		if ((hi_nibble >= 0x30) && (hi_nibble <= 0x39))
			hi_nibble -= 0x30;
		else hi_nibble = 0x0f;
		pp[i / 2] = (lo_nibble) | (hi_nibble << 4);
	}
}

/****************************************************************
 *  F@: hexToNum - GLOBAL API to convert hex to Num
 *
 */
INT8 hexToNum(CHAR ch)
{
	ch = toupper(ch);

	if ( isdigit(ch) )
	{
		ch -= '0';
	}
	else if ( isxdigit(ch) )
	{
		ch -= 'A';
		ch += 0x0A;
	}
	else
	{
		DPRINTF("%02x is not a hex digit", ch);
	}

	return ch;
}

/************************************************************************************
 * F@: str2NumericList - Convert a string containing numeric list to CiNumericList
 *
 */
BOOL str2NumericList( CHAR *str, CiNumericList *numList )
{
	CHAR * RemainingStr = str;
	UINT32 Number;

	/* Initialize the list */
	numList->hasIndvNums  = FALSE;
	numList->hasRange     = FALSE;
	numList->indvLstSize  = 0;
	numList->rangeLstSize = 0;

	/* Parse the string */
	while ( (RemainingStr - str) < (int)strlen((char *)str) )
	{
		Number = strtol( (char *)RemainingStr, (char **)&RemainingStr, 10 );

		if ( *RemainingStr == ' ' )
		{
			/* Skip any spaces */
			RemainingStr++;;
		}
		else if ( *(RemainingStr) == '-' )
		{
			/* we have a range */
			numList->hasRange = TRUE;
			numList->rangeLst[numList->rangeLstSize].min = Number;
			RemainingStr++;
			Number = strtol( (char *)RemainingStr, (char **)&RemainingStr, 10 );
			if ( *RemainingStr == '\0' || *(RemainingStr++) == ',' )
			{
				numList->rangeLst[numList->rangeLstSize++].max = Number;
			}
			else
			{
				/* ERROR */
				return FALSE;
			}

		}
		else if ( *RemainingStr == '\0' || *(RemainingStr++) == ',' )
		{
			/* just an individual number */
			numList->hasIndvNums = TRUE;
			numList->indvList[numList->indvLstSize++] = Number;
		}
		else
		{
			/* ERROR */
			return FALSE;
		}
	}

	return TRUE;
}

/*
 * Print variable of type CiNumericList into a string (append to the end)
 */
void PrintNumericList( const CiNumericList * pList, CHAR* targetStr )
{
	INT32 i;
	char TempBuf[400];
	BOOL hasRange = FALSE; /* if TRUE, put comma before individual list printout */

	strcat( (char *)targetStr, "\"" );

	/* Print range */
	if ( pList->hasRange == TRUE )
	{
		hasRange = TRUE;

		for ( i = 0; i < pList->rangeLstSize - 1; i++ )
		{
			sprintf( (char *)TempBuf, "%d-%d,", pList->rangeLst[i].min,
				 pList->rangeLst[i].max );
			strcat( (char *)targetStr, (char *)TempBuf );
		}
		sprintf( (char *)TempBuf, "%d-%d", pList->rangeLst[pList->rangeLstSize - 1].min,
			 pList->rangeLst[pList->rangeLstSize - 1].max );
		strcat( (char *)targetStr, (char *)TempBuf );

	}

	/* Print list */
	if ( pList->hasIndvNums == TRUE )
	{
		/* If we had a range, print comma before proceeding */
		if ( hasRange )
		{
			strcat( (char *)targetStr, "," );
		}

		for ( i = 0; i < pList->indvLstSize - 1; i++ )
		{
			sprintf( (char *)TempBuf, "%d,", pList->indvList[i] );
			strcat( (char *)targetStr, (char *)TempBuf );
		}

		sprintf((char *)TempBuf, "%d", pList->indvList[pList->indvLstSize - 1] );
		strcat( (char *)targetStr, (char *)TempBuf );
	}

	strcat( (char *)targetStr, "\"" );
	return;
}

struct character_pair
{
	unsigned short first;
	unsigned short second;
};

struct string_info
{
	int is_gsm;                /* gsm string or not*/
	int gsm_cnt;               /* gsm character count (exclude gsm extension) */
	int gsm_ext_cnt;           /* gsm extension character count */
	int non_gsm_cnt;           /* non-gsm character count */
	unsigned short highest;    /* highest non-gsm character */
	unsigned short lowest;     /* lowest non-gsm character */
};

#define GSM_ESCAPE_CHAR 0x1B
#define UNICODE_SPACE_CHAR 0x0020

/* GSM 7 bit default alphabet */
/* escape character is converted to 0xFFFF */
static const unsigned short def_gsm[] = 
{
	/* 0x00 ~ 0x0F */
	0x0040, 0x00A3, 0x0024, 0x00A5, 0x00E8, 0x00E9, 0x00F9, 0x00EC,
	0x00F2, 0x00C7, 0x000A, 0x00D8, 0x00F8, 0x000D, 0x00C5, 0x00E5,

	/* 0x10 ~ 0x1F */
	0x0394, 0x005F, 0x03A6, 0x0393, 0x039B, 0x03A9, 0x03A0, 0x03A8,
	0x03A3, 0x0398, 0x039E, 0xFFFF, 0x00C6, 0x00E6, 0x00DF, 0x00C9,

	/* 0x20 ~ 0x2F */
	0x0020, 0x0021, 0x0022, 0x0023, 0x00A4, 0x0025, 0x0026, 0x0027,
	0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,

	/* 0x30 ~ 0x3F */
	0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
	0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,

	/* 0x40 ~ 0x4F */
	0x00A1, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
	0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,

	/* 0x50 ~ 0x5F */
	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
	0x0058, 0x0059, 0x005A, 0x00C4, 0x00D6, 0x00D1, 0x00DC, 0x00A7,

	/* 0x60 ~ 0x6F */
	0x00BF, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
	0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,

	/* 0x70 ~ 0x7F */
	0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
	0x0078, 0x0079, 0x007A, 0x00E4, 0x00F6, 0x00F1, 0x00FC, 0x00E0
};

/* Unicode to GSM 7 bit default alphabet */
/* Sorted table */
static const struct character_pair def_unicode2gsm[] = 
{
	{0x000A, 0x0A}, {0x000D, 0x0D}, {0x0020, 0x20}, {0x0021, 0x21},
	{0x0022, 0x22}, {0x0023, 0x23}, {0x0024, 0x02}, {0x0025, 0x25},
	{0x0026, 0x26}, {0x0027, 0x27}, {0x0028, 0x28}, {0x0029, 0x29},
	{0x002A, 0x2A}, {0x002B, 0x2B}, {0x002C, 0x2C}, {0x002D, 0x2D},
	{0x002E, 0x2E}, {0x002F, 0x2F}, {0x0030, 0x30}, {0x0031, 0x31},
	{0x0032, 0x32}, {0x0033, 0x33}, {0x0034, 0x34}, {0x0035, 0x35},
	{0x0036, 0x36}, {0x0037, 0x37}, {0x0038, 0x38}, {0x0039, 0x39},
	{0x003A, 0x3A}, {0x003B, 0x3B}, {0x003C, 0x3C}, {0x003D, 0x3D},
	{0x003E, 0x3E}, {0x003F, 0x3F}, {0x0040, 0x00}, {0x0041, 0x41},
	{0x0042, 0x42}, {0x0043, 0x43}, {0x0044, 0x44}, {0x0045, 0x45},
	{0x0046, 0x46}, {0x0047, 0x47}, {0x0048, 0x48}, {0x0049, 0x49},
	{0x004A, 0x4A}, {0x004B, 0x4B}, {0x004C, 0x4C}, {0x004D, 0x4D},
	{0x004E, 0x4E}, {0x004F, 0x4F}, {0x0050, 0x50}, {0x0051, 0x51},
	{0x0052, 0x52}, {0x0053, 0x53}, {0x0054, 0x54}, {0x0055, 0x55},
	{0x0056, 0x56}, {0x0057, 0x57}, {0x0058, 0x58}, {0x0059, 0x59},
	{0x005A, 0x5A}, {0x005F, 0x11}, {0x0061, 0x61}, {0x0062, 0x62},
	{0x0063, 0x63}, {0x0064, 0x64}, {0x0065, 0x65}, {0x0066, 0x66},
	{0x0067, 0x67}, {0x0068, 0x68}, {0x0069, 0x69}, {0x006A, 0x6A},
	{0x006B, 0x6B}, {0x006C, 0x6C}, {0x006D, 0x6D}, {0x006E, 0x6E},
	{0x006F, 0x6F}, {0x0070, 0x70}, {0x0071, 0x71}, {0x0072, 0x72},
	{0x0073, 0x73}, {0x0074, 0x74}, {0x0075, 0x75}, {0x0076, 0x76},
	{0x0077, 0x77}, {0x0078, 0x78}, {0x0079, 0x79}, {0x007A, 0x7A},
	{0x00A1, 0x40}, {0x00A3, 0x01}, {0x00A4, 0x24}, {0x00A5, 0x03},
	{0x00A7, 0x5F}, {0x00BF, 0x60}, {0x00C4, 0x5B}, {0x00C5, 0x0E},
	{0x00C6, 0x1C}, {0x00C7, 0x09}, {0x00C9, 0x1F}, {0x00D1, 0x5D},
	{0x00D6, 0x5C}, {0x00D8, 0x0B}, {0x00DC, 0x5E}, {0x00DF, 0x1E},
	{0x00E0, 0x7F}, {0x00E4, 0x7B}, {0x00E5, 0x0F}, {0x00E6, 0x1D},
	{0x00E8, 0x04}, {0x00E9, 0x05}, {0x00EC, 0x07}, {0x00F1, 0x7D},
	{0x00F2, 0x08}, {0x00F6, 0x7C}, {0x00F8, 0x0C}, {0x00F9, 0x06},
	{0x00FC, 0x7E}, {0x0393, 0x13}, {0x0394, 0x10}, {0x0398, 0x19},
	{0x039B, 0x14}, {0x039E, 0x1A}, {0x03A0, 0x16}, {0x03A3, 0x18},
	{0x03A6, 0x12}, {0x03A8, 0x17}, {0x03A9, 0x15}, {0xFFFF, 0x1B}
};

/* GSM 7 bit default alphabet extension table */
/* Sorted table */
static const struct character_pair def_gsm_ext[] = {
	{0x0A, 0x000C},
	{0x14, 0x005E},
	{0x28, 0x007B},
	{0x29, 0x007D},
	{0x2F, 0x005C},
	{0x3C, 0x005B},
	{0x3D, 0x007E},
	{0x3E, 0x005D},
	{0x40, 0x007C},
	{0x65, 0x20AC}
};

/* Unicode to GSM 7 bit default alphabet extension table */
/* Sorted table */
static const struct character_pair def_unicode2gsm_ext[] = {
	{0x000C, 0x0A},
	{0x005B, 0x3C},
	{0x005C, 0x2F},
	{0x005D, 0x3E},
	{0x005E, 0x14},
	{0x007B, 0x28},
	{0x007C, 0x40},
	{0x007D, 0x29},
	{0x007E, 0x3D},
	{0x20AC, 0x65}
};

/*
 * compare two character_pair (just compare the member first)
 */
static int character_pair_cmp(const void *pa, const void *pb)
{
	const struct character_pair *pcpa = (const struct character_pair *)pa;
	const struct character_pair *pcpb = (const struct character_pair *)pb;

	return (pcpa->first > pcpb->first) - (pcpa->first < pcpb->first);
}

/*
 * convert unicode character to gsm 7 bit character
 *
 * @us: unicode character
 * @is_ext: out pointer to indicate whether is coded as extension character or not
 * @return: gsm 7 bit character, 0xFF means error or cannot convert to gsm 7 bit
 */
unsigned char unicode2gsm(unsigned short us, int *is_ext)
{
	struct character_pair cp;
	const struct character_pair *p;

	if(!is_ext)
		return 0xFF;

	cp.first = us;

	/* search in default GSM 7 bit table */
	p = bsearch(&cp, def_unicode2gsm, 
		sizeof(def_unicode2gsm) / sizeof(def_unicode2gsm[0]), 
		sizeof(def_unicode2gsm[0]), character_pair_cmp);

	if(p)
	{
		*is_ext = 0;
		return (unsigned char)p->second;
	}

	/* search in default GSM 7 bit extension table */
	p = bsearch(&cp, def_unicode2gsm_ext, 
		sizeof(def_unicode2gsm_ext) / sizeof(def_unicode2gsm_ext[0]), 
		sizeof(def_unicode2gsm_ext[0]), character_pair_cmp);

	if(p)
	{
		*is_ext = 1;
		return (unsigned char)p->second;
	}

	return 0xFF;
}

/*
 * convert gsm 7 bit character to unicode character
 *
 * @uc: gsm 7 bit character
 * @is_ext: indicate whether uc is extension or default character
 * @return: unicode character, 0xFFFF means error
 */
unsigned short gsm2unicode(unsigned char uc, int is_ext)
{
	if(uc >= sizeof(def_gsm) / sizeof(def_gsm[0]))
		return 0xFFFF;

	if(is_ext)
	{
		const struct character_pair *p;
		struct character_pair cp;

		cp.first = uc;

		/* search in default GSM 7bit extension table */
		p = bsearch(&cp, def_gsm_ext, 
			sizeof(def_gsm_ext) / sizeof(def_gsm_ext[0]), 
			sizeof(def_gsm_ext[0]), character_pair_cmp);

		if(p)
			return p->second;
		else
			return 0xFFFF;
	}
	else
	{
		return def_gsm[uc];
	}
}

/* iterate the string to check whether it is a gsm string, 
 * if not get some string information
 *
 * @str: given unicode string
 * @len: string length
 * @info: out pointer, string information
 * @return: success or not, -1 means error, 0 means success
 */
static int check_string(const unsigned short *str, int len, struct string_info *info)
{
	const unsigned short *p = NULL;
	const unsigned short *pend = NULL;

	if(!str || len < 0 || !info)
		return -1;

	info->is_gsm= 1; /* gsm string by default */
	info->highest = 0x0000;
	info->lowest = 0xFFFF;
	info->gsm_cnt = 0;
	info->gsm_ext_cnt = 0;
	info->non_gsm_cnt = 0;

	p = str;
	pend = str + len;

	while(p != pend)
	{
		int is_ext = 0;
		if(unicode2gsm(*p, &is_ext) == 0xFF)
		{
			if(*p > info->highest) info->highest = *p;
			if(*p < info->lowest) info->lowest = *p;
			info->is_gsm = 0;
			++info->non_gsm_cnt;
		}
		else if(is_ext)
		{
			++info->gsm_ext_cnt;
		}
		else
		{
			++info->gsm_cnt;
		}
		++p;
	}

	return 0;
}

/**
   * Encodes/Decodes a string field that's formatted like the EF[ADN] alpha
   * identifier
   *
   * From TS 51.011 10.5.1:
   *   Coding:
   *	   this alpha tagging shall use either
   *	  -    the SMS default 7 bit coded alphabet as defined in
   *		  TS 23.038 [12] with bit 8 set to 0. The alpha identifier
   *		  shall be left justified. Unused bytes shall be set to 'FF'; or
   *	  -    one of the UCS2 coded options as defined in annex B.
   *
   * Annex B from TS 11.11 V8.13.0:
   *	  1)  If the first octet in the alpha string is '80', then the
   *		  remaining octets are 16 bit UCS2 characters ...
   *	  2)  if the first octet in the alpha string is '81', then the
   *		  second octet contains a value indicating the number of
   *		  characters in the string, and the third octet contains an
   *		  8 bit number which defines bits 15 to 8 of a 16 bit
   *		  base pointer, where bit 16 is set to zero and bits 7 to 1
   *		  are also set to zero.  These sixteen bits constitute a
   *		  base pointer to a "half page" in the UCS2 code space, to be
   *		  used with some or all of the remaining octets in the string.
   *		  The fourth and subsequent octets contain codings as follows:
   *		  If bit 8 of the octet is set to zero, the remaining 7 bits
   *		  of the octet contain a GSM Default Alphabet character,
   *		  whereas if bit 8 of the octet is set to one, then the
   *		  remaining seven bits are an offset value added to the
   *		  16 bit base pointer defined earlier...
   *	  3)  If the first octet of the alpha string is set to '82', then
   *		  the second octet contains a value indicating the number of
   *		  characters in the string, and the third and fourth octets
   *		  contain a 16 bit number which defines the complete 16 bit
   *		  base pointer to a "half page" in the UCS2 code space...
   */

#define DUMP_ARRAY(arr, len, T) do \
{ \
	T *p = arr; \
	T *pend = arr + (len); \
	char *buf = NULL; \
	int buf_len = 0; \
	char fmt[10]; \
	buf = malloc(2 * sizeof(T) * (len) + 1); \
	if(!buf) break; \
	sprintf(fmt, "%%0%dX", sizeof(T) * 2); \
	while(p != pend) \
		buf_len += sprintf(buf+buf_len, fmt, *p++); \
	DBGMSG("%s", buf); \
	free(buf); \
} \
while(0)

/*
 * encode unicode string to ADN alpha tag
 * NOTE: this function will not try to add trailing '\0' to output string
 *
 * @in: unicode string
 * @in_len: length of unicode string
 * @out: output string to store encoded result
 * @out_len: length of output string (not include '\0' in the back)
 * @return: encoded length
 */
int pb_encode_alpha_tag(const unsigned short *in, int in_len, unsigned char *out, int out_len)
{
	unsigned short base = 0;
	int is_80alpha = 0;
	int length;
	struct string_info info = {0, 0, 0, 0, 0, 0};
	const unsigned short *pin = NULL;
	const unsigned short *pin_end = NULL;
	unsigned char        *pout = NULL;
	// const unsigned char  *pout_end = NULL;

	if(!in || !out || in_len <= 0 || out_len <= 0)
		return -1;

	DBGMSG("%s: dump of source string:\n", __FUNCTION__);
	DUMP_ARRAY(in, in_len, const unsigned short);

	if(check_string(in, in_len, &info) == -1)
	{
		DBGMSG("%s: check string error\n", __FUNCTION__);
		return -1;
	}

	pin = in;
	pin_end = in + in_len;
	pout = out;
	// pout_end = out + out_len;

	memset(out, 0, sizeof(*out) * out_len);

	/* prepare first few bytes and do length checking */
	if(!info.is_gsm)
	{
		/* 81 coding scheme
		 */
		if(((info.highest & 0x8000) == 0) && 
			((info.lowest & 0x8000) == 0) &&
			((info.highest >> 7) == (info.lowest >> 7)))
		{
			length = 3 + info.non_gsm_cnt + info.gsm_cnt + 2 * info.gsm_ext_cnt;
			if(out_len < length)
			{
				DBGMSG("%s: 81 coding scheme, need %d characters, out_len is only: %d\n", __FUNCTION__, length, out_len);
				return -1;
			}

			*pout++ = 0x81;
			*pout++ = in_len;
			*pout++ = info.lowest >> 7;
			base = info.lowest & (~0x007F);
		}
		/* 82 coding scheme
		 */
		else if((info.highest - info.lowest) < 0x80)
		{
			length = 4 + info.non_gsm_cnt + info.gsm_cnt + 2 * info.gsm_ext_cnt;
			if(out_len < length)
			{
				DBGMSG("%s: 82 coding scheme, need %d characters, out_len is only: %d\n", __FUNCTION__, length, out_len);
				return -1;
			}


			*pout++ = 0x82;
			*pout++ = in_len;
			*pout++ = info.lowest >> 8;
			*pout++ = info.lowest & 0x00FF;
			base = info.lowest;
		}
		/* 80 coding scheme
		 */
		else
		{
			length = 1 + in_len * 2;
			if(out_len < length)
			{
				DBGMSG("%s: 80 coding scheme, need %d characters, out_len is only: %d\n", __FUNCTION__, length, out_len);
				return -1;
			}

			*pout++ = 0x80;
			is_80alpha = 1;
		}
	}
	/* GSM 7 bit coding scheme
	 */
	else
	{
		length = info.gsm_cnt + 2 * info.gsm_ext_cnt;
		if(out_len < length)
		{
			DBGMSG("%s: GSM 7 bit coding scheme, need %d characters, out_len is only: %d\n", __FUNCTION__, length, out_len);
			return -1;
		}

	}

	if(is_80alpha)
	{
		while(pin != pin_end)
		{
			*pout++ = *pin >> 8;
			*pout++ = *pin & 0x00FF;
			++pin;
		}
	}
	else
	{
		while(pin != pin_end)
		{
			int is_ext = 0;
			unsigned char c = unicode2gsm(*pin, &is_ext);
			if(c == 0xFF) /* ucs2 */
			{
				*pout++ = (*pin - base) | 0x80;
			}
			else /* gsm */
			{
				if(is_ext)
					*pout++ = GSM_ESCAPE_CHAR;
				*pout++ = c;
			}
			++pin;
		}
	}

	DBGMSG("%s: dump of dest string:\n", __FUNCTION__);
	DUMP_ARRAY(out, pout - out, unsigned char);

	return pout - out;
}

/*
 * calculate the length of ADN alpha tag
 *
 * @in: ADN alpha tag string
 * @in_len: length of string
 * @return: length
 */
int calc_alpha_tag_length(const unsigned char *in, int in_len)
{
	const unsigned char   *pin = NULL;
	const unsigned char   *pin_end = NULL;
	int length = 0;
	int coded_length = 0;
	int is_80alpha = 0;

	if(!in || in_len <= 0)
		return 0;

	pin = in;
	pin_end = in + in_len;

	if(*pin == 0x80)
	{
		length = (in_len - 1) / 2;
		is_80alpha = 1;
	}
	else if(*pin == 0x81)
	{
		if(in_len < 3)
		{
			DBGMSG("%s: 81 coding scheme, in string is too short: %d\n", __FUNCTION__, in_len);
			return 0;
		}

		++pin;
		coded_length = *pin++;
		++pin;

	}
	else if(*pin == 0x82)
	{
		if(in_len < 4)
		{
			DBGMSG("%s: 82 coding scheme, in string is too short: %d\n", __FUNCTION__, in_len);
			return 0;
		}

		++pin;
		coded_length = *pin++;
		pin += 2;
	}
	else if(*pin < 0x80)
	{
		coded_length = in_len;
	}
	else
	{
		DBGMSG("%s: first byte is %02X, not a valid alpha tag string\n", __FUNCTION__, *pin);
		return 0;
	}

	if(!is_80alpha)
	{
		length = 0;
		while(pin != pin_end) /* go through the string to get the length */
		{
			if(*pin == GSM_ESCAPE_CHAR)
			{
				++pin;
				if(pin == pin_end)
					break;
			}
			++length;
			++pin;
		}

		length = (length > coded_length) ? coded_length : length;
	}

	return length;
}

/*
 * decode ADN alpha tag to unicode string
 * NOTE: this function will not try to add trailing 0x00 to output string
 *
 * @in: ADN alpha tag string
 * @in_len: length of ADN alpha tag string
 * @out: output unicode string to store encoded result
 * @out_len: length of output string (not include 0x00 in the back)
 * @return: decoded length
 */
int pb_decode_alpha_tag(const unsigned char *in, int in_len, unsigned short *out, int out_len)
{
	unsigned short base = 0;
	int is_80alpha = 0;
	int length;
	const unsigned char   *pin = NULL;
	const unsigned char   *pin_end = NULL;
	unsigned short        *pout = NULL;
	const unsigned short  *pout_end = NULL;

	if(!in || !out || in_len <= 0 || out_len <= 0)
		return -1;

	DBGMSG("%s: dump of source string:\n", __FUNCTION__);
	DUMP_ARRAY(in, in_len, const unsigned char);

	memset(out, 0, sizeof(*out) * out_len);

	length = calc_alpha_tag_length(in, in_len);

	if(length == 0)
	{
		DBGMSG("%s: decoded length is only 0\n", __FUNCTION__);
		return -1;
	}

	if(out_len < length)
	{
		DBGMSG("%s: need %d characters, out_len is only: %d\n", __FUNCTION__, length, out_len);
		return -1;
	}

	pin = in;
	pin_end = in + in_len;
	pout = out;
	pout_end = out + length;

	/* 80 coding scheme
	 */
	if(*pin == 0x80)
	{
		is_80alpha = 1;
		++pin;
	}
	/* 81 coding scheme
	 */
	else if(*pin == 0x81)
	{
		pin += 2;
		base = (*pin++) << 7;
	}
	/* 82 coding scheme
	 */
	else if(*pin == 0x82)
	{
		pin += 2;
		base = (*pin << 8) | (*(pin + 1));
		pin += 2;
	}
	/* GSM 7 bit coding scheme
	 */
	else if(*pin < 0x80)
	{
		// nothing need to do
	}
	/* Error string
	 */
	else
	{
		DBGMSG("%s: first byte is %02X, not a valid alpha tag string\n", __FUNCTION__, *pin);
		return -1;
	}

	if(is_80alpha)
	{
		while(pin != pin_end)
		{
			unsigned short c = *pin++;
			if(pin == pin_end)
				break;

			*pout++ = (c << 8) | *pin++;
		}
	}
	else
	{
		while(pin != pin_end && pout != pout_end)
		{
			if(*pin & 0x80) /* ucs2 */
			{
				*pout++ = base + (*pin & 0x7F);
			}
			else /* gsm */
			{
				int is_ext = 0;
				unsigned short c;
				if(*pin == GSM_ESCAPE_CHAR)
				{
					is_ext = 1;
					++pin;
					if(pin == pin_end)
						break;
				}
				c = gsm2unicode(*pin, is_ext);
				if(c == 0xFFFF)
					*pout++ = UNICODE_SPACE_CHAR;
				else
					*pout++ = c;
			}
			++pin;
		}
	}

	DBGMSG("%s: dump of dest string:\n", __FUNCTION__);
	DUMP_ARRAY(out, pout - out, unsigned short);

	return pout - out;
}

/*
 * get_from_cscs_xxx
 * get one character from CSCS string and increase the pointer
 *
 * @ppin: pointer to input pointer
 * @pin_end: input end pointer
 * @return: unicode character, -1 means reach the end
 */

static inline int get_from_cscs_ira(const char **ppin, const char *pin_end)
{
	(void)pin_end;
	return *(*ppin)++;
}

static inline int get_from_cscs_ucs2(const char **ppin, const char *pin_end)
{
	int c;
	if(*ppin + 3 >= pin_end) return -1;
	c = (hexToNum(**ppin) << 12) | (hexToNum(*(*ppin+1)) << 8) | (hexToNum(*(*ppin+2)) << 4) | hexToNum(*(*ppin+3));
	*ppin += 4;
	return c;
}

static inline int get_from_cscs_hex(const char **ppin, const char *pin_end)
{
	int c;
	if(*ppin + 1 >= pin_end) return -1;
	c = (hexToNum(**ppin) << 4) | hexToNum(*(*ppin+1));
	*ppin += 2;
	return c;
}

/*
 * put_to_cscs_xxx
 * put one character to CSCS string and increase the pointer
 *
 * @c: unicode character
 * @ppout: pointer to output pointer
 * @pout_end: output end pointer
 * @return: success or not, -1 means failure, 0 means success
 */

static inline int put_to_cscs_ira(int c, char **ppout, const char *pout_end)
{
	(void)pout_end;
	*(*ppout)++ = (char)c;
	return 0;
}

static const char digits[16] = "0123456789ABCDEF";

static inline int put_to_cscs_ucs2(int c, char **ppout, const char *pout_end)
{
	if(*ppout + 3 >= pout_end) return -1;
	*(*ppout)++ = digits[(c >> 12) & 0xF];
	*(*ppout)++ = digits[(c >> 8) & 0xF];
	*(*ppout)++ = digits[(c >> 4) & 0xF];
	*(*ppout)++ = digits[c & 0xF];
	return 0;
}

static inline int put_to_cscs_hex(int c, char **ppout, const char *pout_end)
{
	if(*ppout + 1 >= pout_end) return -1;
	*(*ppout)++ = digits[(c >> 4) & 0xF];
	*(*ppout)++ = digits[c & 0xF];
	return 0;
}

/*
 * get_from_bin_xxx
 * get one character from binary string and increase the pointer
 *
 * @ppin: pointer to input pointer
 * @pin_end: input end pointer
 * @return: unicode character, -1 means reach the end
 */

static inline int get_from_bin_ira(const void **ppin, const void *pin_end)
{
	const char *p = (const char *)*ppin;
	(void)pin_end;
	*ppin = (const void*)(p + 1);
	return *p;
}

static inline int get_from_bin_ucs2(const void **ppin, const void *pin_end)
{
	const unsigned short *p = (const unsigned short *)*ppin;
	(void)pin_end;
	*ppin = (const void*)(p + 1);
	return *p;
}

static inline int get_from_bin_hex(const void **ppin, const void *pin_end)
{
	const unsigned char *p = (const unsigned char *)*ppin;
	(void)pin_end;
	*ppin = (const void*)(p + 1);
	return *p;
}

static inline int get_from_bin_gsm(const void **ppin, const void *pin_end)
{
	const unsigned char *p = (const unsigned char *)*ppin;
	int c;
	int is_ext = 0;
	if(*p == GSM_ESCAPE_CHAR)
	{
		if(p + 1 >= (const unsigned char *)pin_end) return -1;
		is_ext = 1;
		++p;
	}
	c = gsm2unicode(*p, is_ext);
	*ppin = (const void*)(p + 1);
	return c;
}

/*
 * put_to_bin_xxx
 * put one character to binary string and increase the pointer
 *
 * @c: unicode character
 * @ppout: pointer to output pointer
 * @pout_end: output end pointer
 * @return: success or not, -1 means failure, 0 means success
 */

static inline int put_to_bin_ira(int c, void **ppout, const void *pout_end)
{
	char *p = (char *)*ppout;
	(void)pout_end;
	*ppout = (void*)(p + 1);
	*p = (char)c;
	return 0;
}

static inline int put_to_bin_ucs2(int c, void **ppout, const void *pout_end)
{
	unsigned short *p = (unsigned short *)*ppout;
	(void)pout_end;
	*ppout = (void*)(p + 1);
	*p = (unsigned short)c;
	return 0;
}

static inline int put_to_bin_hex(int c, void **ppout, const void *pout_end)
{
	unsigned char *p = (unsigned char *)*ppout;
	(void)pout_end;
	*ppout = (void*)(p + 1);
	*p = (unsigned char)c;
	return 0;
}

static inline int put_to_bin_gsm(int c, void **ppout, const void *pout_end)
{
	unsigned char *p = (unsigned char *)*ppout;
	int is_ext = 0;
	unsigned char gsm = unicode2gsm(c, &is_ext);
	if(gsm == 0xFF) /* ucs2 */
	{
		*p = (unsigned char)UNICODE_SPACE_CHAR;
	}
	else /* gsm */
	{
		if(is_ext)
		{
			if(p + 1 >= (const unsigned char *)pout_end) return -1;
			*p++ = (unsigned char)GSM_ESCAPE_CHAR;
		}
		*p = gsm;
	}
	*ppout = (void*)(p + 1);
	return 0;
}

/*
 * convert binary string to CSCS string
 * NOTE: will not add trailing '\0' in the end
 *
 * @in: binary string
 * @in_len: binary string length
 * @bin_type: binary type
 * @out: output string
 * @out_len: output string length(not include the trailing '\0')
 * @out_chset: character set of output string
 * @return: converted length
 */
int libConvertBinToCSCSString(const void *in, int in_len, enum string_binary_type bin_type, char *out, int out_len, AtciCharacterSet out_chset)
{
	const void   *pin = NULL;
	const void   *pin_end = NULL;
	char         *pout = NULL;
	const char   *pout_end = NULL;
	int (*put_fn)(int, char **, const char *);
	int (*get_fn)(const void **, const void *);

	if(!in || !out || in_len <= 0 || out_len <= 0)
		return -1;

	switch(out_chset)
	{
	case ATCI_CHSET_IRA:
		put_fn = put_to_cscs_ira;
		break;

	case ATCI_CHSET_UCS2:
		put_fn = put_to_cscs_ucs2;
		break;

	case ATCI_CHSET_HEX:
		put_fn = put_to_cscs_hex;
		break;
	default:
		ERRMSG("%s: unsupported chset %d\n", __FUNCTION__, out_chset);
		return -1;
	}

	pin = in;
	pout = out;
	pout_end = out + out_len;

	switch(bin_type)
	{
	case STR_BIN_TYPE_IRA:
		get_fn = get_from_bin_ira;
		pin_end = (const char *)in + in_len;
		break;

	case STR_BIN_TYPE_HEX:
		get_fn = get_from_bin_hex;
		pin_end = (const unsigned char *)in + in_len;
		break;

	case STR_BIN_TYPE_UCS2:
		get_fn = get_from_bin_ucs2;
		pin_end = (const unsigned short*)in + in_len;
		break;

	case STR_BIN_TYPE_GSM:
		get_fn = get_from_bin_gsm;
		pin_end = (const unsigned char*)in + in_len;
		break;

	default:
		ERRMSG("%s: error binary type: %d", __FUNCTION__, bin_type);
		return -1;
	}

	while(pin < pin_end && pout < pout_end)
	{
		int c= get_fn(&pin, pin_end);
		if(c < 0) break;
		c = put_fn(c, &pout, pout_end);
		if(c < 0) break;
	}

	return pout - out;
}

/*
 * convert CSCS string to binary string
 * NOTE: will not add trailing 0x00 in the end
 *
 * @in: cscs string
 * @in_len: cscs string length
 * @in_chset: character set of cscs string
 * @out: output string
 * @out_len: output string length(not include the trailing 0x00)
 * @bin_type: binary type
 * @return: converted length
 */
int libConvertCSCSStringToBin(const char *in, int in_len, AtciCharacterSet in_chset, void *out, int out_len, enum string_binary_type bin_type)
{
	const char   *pin = NULL;
	const char   *pin_end = NULL;
	void         *pout = NULL;
	const void   *pout_end = NULL;
	int (*put_fn)(int, void **, const void *);
	int (*get_fn)(const char **, const char *);

	if(!in || !out || in_len <= 0 || out_len <= 0)
		return -1;

	switch(in_chset)
	{
	case ATCI_CHSET_IRA:
		get_fn = get_from_cscs_ira;
		break;

	case ATCI_CHSET_UCS2:
		get_fn = get_from_cscs_ucs2;
		break;

	case ATCI_CHSET_HEX:
		get_fn = get_from_cscs_hex;
		break;
	default:
		ERRMSG("%s: unsupported chset %d\n", __FUNCTION__, in_chset);
		return -1;
	}

	pin = in;
	pin_end = in + in_len;
	pout = out;

	switch(bin_type)
	{
	case STR_BIN_TYPE_IRA:
		put_fn = put_to_bin_ira;
		pout_end = (char *)out + out_len;
		break;

	case STR_BIN_TYPE_HEX:
		put_fn = put_to_bin_hex;
		pout_end = (unsigned char *)out + out_len;
		break;

	case STR_BIN_TYPE_UCS2:
		put_fn = put_to_bin_ucs2;
		pout_end = (unsigned short *)out + out_len;
		break;

	case STR_BIN_TYPE_GSM:
		put_fn = put_to_bin_gsm;
		pout_end = (unsigned char *)out + out_len;
		break;

	default:
		ERRMSG("%s: error binary type: %d", __FUNCTION__, bin_type);
		return -1;
	}

	while(pin < pin_end && pout < pout_end)
	{
		int c= get_fn(&pin, pin_end);
		if(c < 0) break;
		c = put_fn(c, &pout, pout_end);
		if(c < 0) break;
	}

	if(bin_type == STR_BIN_TYPE_UCS2)
	{
		return (unsigned short*)pout - (unsigned short*)out;
	}
	else
	{
		return (char *)pout - (char *)out;
	}
}

/*
 * convert cscs string to ADN alpha tag strings
 * NOTE: will not add trailing '\0' in the end
 *
 * @in: input string
 * @in: input string length
 * @in_chset: character set of input string
 * @out: output string
 * @out_len: output string length(not include trailing '\0')
 */
int pb_encode_character(char *in, int in_len, AtciCharacterSet in_chset, char *out, int out_len)
{
	int length;
	unsigned short *mid = NULL;
	int mid_len = 0;
	int ret;

	if(in_chset == ATCI_CHSET_HEX)
	{
		ret = libConvertCSCSStringToHEX(in, in_len, in_chset, out, out_len);
		if(ret < 0)
		{
			ERRMSG("%s: covert cscs string to hex error\n", __FUNCTION__);
			ret = 0;
		}
		return ret;
	}

	if(in_chset == ATCI_CHSET_UCS2)
		mid_len = in_len / 4;
	else
		mid_len = in_len;

	mid = malloc(sizeof(unsigned short) * mid_len);
	if(!mid)
	{
		ERRMSG("%s: malloc fail\n", __FUNCTION__);
		return 0;
	}

	if((ret = libConvertCSCSStringToUCS2(in, in_len, in_chset, mid, mid_len)) < 0)
	{
		ERRMSG("%s: convert to ucs2 binary error", __FUNCTION__);
		free(mid);
		return 0;
	}

	length = pb_encode_alpha_tag(mid, ret, (unsigned char *)out, out_len);
	if(length == -1)
	{
		ERRMSG("%s: encode alpha tag error\n", __FUNCTION__);
		length = 0;
	}

	free(mid);

	return length;
}

/*
 * convert ADN alpha tag to cscs string
 * NOTE: will not add trailing '\0' in the end
 *
 * @in: ADN alpha tag string
 * @in: ADN alpha tag string string length
 * @out: output string
 * @out_len: output string length(not include trailing '\0')
 * @out_chset: character set of cscs string
 */
int pb_decode_character(char *in, int in_len, char *out, int out_len, AtciCharacterSet out_chset)
{
	int length;
	unsigned short *mid = NULL;
	int mid_len = 0;
	int ret;

	if(out_chset == ATCI_CHSET_HEX)
	{
		ret = libConvertHEXToCSCSString(in, in_len, out, out_len, out_chset);
		if(ret < 0)
		{
			ERRMSG("%s: covert hex to cscs string error\n", __FUNCTION__);
			ret = 0;
		}
		return ret;
	}

	mid_len = in_len;

	mid = malloc(sizeof(unsigned short) * mid_len);
	if(!mid)
	{
		ERRMSG("%s: malloc fail\n", __FUNCTION__);
		return 0;
	}

	length = pb_decode_alpha_tag((unsigned char *)in, in_len, mid, mid_len);
	if(length == -1)
	{
		ERRMSG("%s: decode alpha tag error\n", __FUNCTION__);
		*out = '\0';
		free(mid);
		return 0;
	}

	ret = libConvertUCS2ToCSCSString(mid, length, out, out_len, out_chset);
	if(ret < 0)
	{
		ERRMSG("%s: covert ucs2 to cscs string error\n", __FUNCTION__);
		ret = 0;
	}
	free(mid);
	return ret;
}

