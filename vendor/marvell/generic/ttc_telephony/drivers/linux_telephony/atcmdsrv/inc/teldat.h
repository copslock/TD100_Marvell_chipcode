/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: teldat.h
 *
 *  Description: Telephony Definitions for AT Command for CI_SG_ID_DEV.
 *
 *  History:
 *   Jan 17, 2008 - Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/
#ifndef TELDAT_H
#define TELDAT_H

#include "ci_dat.h"
#include "ci_api_types.h"
#include "teldef.h"

RETURNCODE_T  ciGpCGSink(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciGpTGSINK(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciCGSend(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciCsDataSink(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  atdb(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  vPDUS(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);

void datCnf(CiSgOpaqueHandle opSgCnfHandle,
	    CiServiceGroupID svgId,
	    CiPrimitiveID primId,
	    CiRequestHandle reqHandle,
	    void*            paras);

void datInd(CiSgOpaqueHandle opSgIndHandle,
	    CiServiceGroupID svgId,
	    CiPrimitiveID primId,
	    CiIndicationHandle indHandle,
	    void*               pParam);

#endif

