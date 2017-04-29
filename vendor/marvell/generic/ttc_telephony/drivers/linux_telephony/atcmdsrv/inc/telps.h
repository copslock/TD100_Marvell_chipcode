/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *	INTEL CONFIDENTIAL
 *	Copyright 2006 Intel Corporation All Rights Reserved.
 *	The source code contained or described herein and all documents related to the source code (“Material? are owned
 *	by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
 *	its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
 *	Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
 *	treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
 *	transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.
 *
 *	No license under any patent, copyright, trade secret or other intellectual property right is granted to or
 *	conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
 *	estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
 *	Intel in writing.
 *	-------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: telps.h
 *
 *  Authors:  Rovin Yu
 *
 *  Description: Macro definition for packet switched service related AT commands
 *
 *  History:
 *   Feb 18, 2008 - Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/
#ifndef TELPS_H
#define TELPS_H

RETURNCODE_T  ciPSRegStatus(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciSetContext(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciPSAttach(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciPDPActivate(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciEnterData(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciSetMinQOS(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciSetReqQOS(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciSetSContext(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciSet3GQOS(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciSet3GminQOS(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciAUTHReq(const utlAtParameterOp_T op,const char *command_name_p,const utlAtParameterValue_P2c parameter_values_p,const size_t num_parameters,const char *info_text_p, unsigned int *xid_p,void *arg_p);//Michal Bukai - PAP_CHUP support (PDP Authentication)
RETURNCODE_T  ciReturnIp(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciPdpErrorReport(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciModifyContext(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciCGTFT(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciFastDormancy(const utlAtParameterOp_T op, const char * command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char * info_text_p, unsigned int * xid_p, void * arg_p);
#ifdef AT_CUSTOMER_HTC
RETURNCODE_T  ciStarFDY(const utlAtParameterOp_T op, const char * command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char * info_text_p, unsigned int * xid_p, void * arg_p);
#endif
#endif

