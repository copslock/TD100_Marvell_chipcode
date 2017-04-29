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
 *  Filename: telss.h
 *
 *  Authors:  Rovin Yu
 *
 *  Description: Macro definition for SS related AT commands
 *
 *  History:
 *   Jan 26, 2008 - Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/
#ifndef TELSS_H
#define TELSS_H

#include "teldef.h"
#include "ci_api_types.h"

typedef enum {
	SS_OP_NULL = 0,
	SS_OP_ACTIVE,
	SS_OP_DEACTIVE,
	SS_OP_INTERROGATE,
	SS_OP_REGISTER,
	SS_OP_ERASE
}SS_OP_ENUM;

typedef enum {
	SS_CODE_NULL = 0,
	SS_ALL_CF,
	SS_ALL_COND_CF,
	SS_CPWD, //added by Jiangang Jing
	SS_CFU,
	SS_CLIP,
	SS_CLIR,
	SS_BAOC,
	SS_BAIC,
	SS_WAIT,
	SS_USSD,
	SS_CF_NOREPLY,
	SS_CF_NOTREACHABLE,
	SS_CD,
	SS_CFB,
	SS_COLP,
	SS_COLR,
	SS_ECT,
	SS_CNAP,
	SS_ALL_BARRING,
	SS_BAOIC,
	SS_BAOIC_EXCHOME,
	SS_OUTG_BARRING,
	SS_BAIC_ROAMING,
	SS_INC_BARRING,
	SS_USS_ALL,
	SS_UUS1,
	SS_UUS2,
	SS_UUS3,
	SS_MRL_1,
}SS_CODE_ENUM;

typedef enum {
	SS_MMI_ALL_TELSRV = 10,
	SS_MMI_TEL      = 11,
	SS_MMI_ALL_DATASRV = 12,
	SS_MMI_FAX = 13,
	SS_MMI_SMS = 16,
	SS_MMI_VGCS = 17,
	SS_MMI_VBS = 18,
	SS_MMI_ALL_EXCEPT_SMS = 19,
	SS_MMI_ALL_BEARSRV = 20,
	SS_MMI_ALL_ASYNC = 21,
	SS_MMI_ALL_SYNC = 22,
	SS_MMI_ALL_DATA_SYNC = 24,
	SS_MMI_ALL_DATA_ASYNC = 25,
	SS_MMI_ALL_GPRS_BEARSRV = 99
}SS_MMI_SC_ENUM;

typedef struct {
	SS_OP_ENUM op;
	SS_CODE_ENUM sc;
	UINT8 si1Len;
	UINT8 si2Len;
	UINT8 si3Len;
	UINT8 si4Len;
	UINT8 si1Str[CI_MAX_ADDRESS_LENGTH];
	UINT8 si2Str[CI_MAX_ADDRESS_LENGTH];
	UINT8 si3Str[CI_MAX_ADDRESS_LENGTH];
	UINT8 si4Str[CI_MAX_ADDRESS_LENGTH];
}SS_STRUCT;

RETURNCODE_T  ciCallForward(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciCallWaiting(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciCOLP(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciCOLR(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciCSSN(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciCLIP(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciCLCK(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciPassword(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciCUSD(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciCallIDRes(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
CiReturnCode ciProcessSS(UINT32 atHandle, SS_STRUCT *ssStruct);
RETURNCODE_T  ciCNAP(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
/* Added by Michal Bukai*/
RETURNCODE_T  ciCDIP(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciCCWE(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciLPLOCVR(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);

#endif

