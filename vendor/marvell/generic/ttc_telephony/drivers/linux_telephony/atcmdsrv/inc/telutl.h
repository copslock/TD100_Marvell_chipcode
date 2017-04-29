/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
 *  INTEL CONFIDENTIAL
 *  Copyright 2006 Intel Corporation All Rights Reserved.
 *  The source code contained or described herein and all documents related to the source code (“Material”) are owned
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
 *  Filename: telutl.h
 *
 *  Description: header file for utility functions for Telephony Controller
 *
 *  History:
 *   May 19, 2006 - Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/

#ifndef TELUTL_H
#define TELUTL_H

#include <linux_types.h>
#include <telatci.h>
#include <tel3gdef.h>
#include <teldef.h>
#include "utlAtParser.h"


#define NULL_CHAR            ('\0')


BOOL getErrorRatioParam( const utlAtParameterValue_P2c param_value_p,
			 int index,
			 ErrorRatioLookup    *errorRatioLookup_p,
			 INT32 defaultValue,
			 INT32               *ratio_p);

BOOL atParamToCiEnum(const utlAtParameterValue_P2c param_value_p,
		     int index,
		     int *param_p,
		     int DefaultValue,
		     EnumParamLookup *enumParamLookup_p);



BOOL operStringToPlmn( CHAR *plmnString_p, Plmn *convertedPlmn, INT16 plmnStringLen );

BOOL isValidCiSsClass( UINT16 *CiSsClass );

BOOL HexToBin(const char *input, int *output, int maxLen);

BOOL getExtendedParameter(INT32 inValue,
			  INT32 *value_p,
			  INT32 DefaultValue);

BOOL getExtValue( const utlAtParameterValue_P2c param_value_p,
		  int index,
		  int *value_p,
		  int minValue,
		  int maxValue,
		  int DefaultValue);

BOOL getExtendedString( CHAR *inString,
			CHAR *outString,
			INT16 maxStringLength,
			INT16 *outStringLength);

BOOL getExtString( const utlAtParameterValue_P2c param_value_p,
		   int index,
		   CHAR *outString,
		   INT16 maxStringLength,
		   INT16 *outStringLength,
		   CHAR *defaultString);

CiAddrNumType DialNumTypeGSM2CI( INT32 GSMType );
INT32 DialNumTypeCI2GSM( CiAddrNumType CiType);
BOOL checkForNumericOnlyChars(const CHAR *password);
CiAddrNumType DialNumTypeGSM2CI( INT32 GSMType );

INT8 hexToNum(CHAR ch);
void string2BCD(UINT8* pp, char *tempBuf, UINT8 length);
BOOL str2NumericList( CHAR *str, CiNumericList *numList );
void PrintNumericList( const CiNumericList * pList, CHAR* targetStr );

extern AtciCharacterSet chset_type;

int pb_encode_alpha_tag(const unsigned short *in, int in_len, unsigned char *out, int out_len);
int pb_decode_alpha_tag(const unsigned char *in, int in_len, unsigned short *out, int out_len);
int pb_encode_character(char *in, int in_len, AtciCharacterSet in_chset, char *out, int out_len);
int pb_decode_character(char *in, int in_len, char *out, int out_len, AtciCharacterSet out_chset);

enum string_binary_type
{
	STR_BIN_TYPE_IRA,
	STR_BIN_TYPE_HEX,
	STR_BIN_TYPE_UCS2,
	STR_BIN_TYPE_GSM
};

int libConvertBinToCSCSString(const void *in, int in_len, enum string_binary_type bin_type, char *out, int out_len, AtciCharacterSet out_chset);
int libConvertCSCSStringToBin(const char *in, int in_len, AtciCharacterSet in_chset, void *out, int out_len, enum string_binary_type bin_type);

/*
 * convert IRA strings to CSCS string according to out_chset
 */
#define libConvertIRAToCSCSString(in, in_len, out, out_len, out_chset) \
	libConvertBinToCSCSString(in, in_len, STR_BIN_TYPE_IRA, out, out_len, out_chset)

/*
 * convert in_chset coded CSCS string to IRA string
 */
#define libConvertCSCSStringToIRA(in, in_len, in_chset, out, out_len) \
	libConvertCSCSStringToBin(in, in_len, in_chset, out, out_len, STR_BIN_TYPE_IRA)

/*
 * convert UCS2 string to CSCS string according to out_chset
 */
#define libConvertUCS2ToCSCSString(in, in_len, out, out_len, out_chset) \
	libConvertBinToCSCSString(in, in_len, STR_BIN_TYPE_UCS2, out, out_len, out_chset)

/*
 * convert in_chset coded CSCS string to UCS2 string
 */
#define libConvertCSCSStringToUCS2(in, in_len, in_chset, out, out_len) \
	libConvertCSCSStringToBin(in, in_len, in_chset, out, out_len, STR_BIN_TYPE_UCS2)

/*
 * convert HEX strings to CSCS string according to out_chset
 */
#define libConvertHEXToCSCSString(in, in_len, out, out_len, out_chset) \
	libConvertBinToCSCSString(in, in_len, STR_BIN_TYPE_HEX, out, out_len, out_chset)

/*
 * convert in_chset coded CSCS string to HEX string
 */
#define libConvertCSCSStringToHEX(in, in_len, in_chset, out, out_len) \
	libConvertCSCSStringToBin(in, in_len, in_chset, out, out_len, STR_BIN_TYPE_HEX)

/*
 * convert GSM strings to CSCS string according to out_chset
 */
#define libConvertGSMToCSCSString(in, in_len, out, out_len, out_chset) \
	libConvertBinToCSCSString(in, in_len, STR_BIN_TYPE_GSM, out, out_len, out_chset)

/*
 * convert in_chset coded CSCS string to GSM string
 */
#define libConvertCSCSStringToGSM(in, in_len, in_chset, out, out_len) \
	libConvertCSCSStringToBin(in, in_len, in_chset, out, out_len, STR_BIN_TYPE_GSM)

#endif

/* END OF FILE */
