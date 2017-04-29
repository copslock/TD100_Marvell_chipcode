/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************
*  Filename: Telpb.h
*
*  Description: Macro definition for phonebook related AT commands
*
*  History:
*   Jan 25, 2008 - Rovin YU, Creation of file
*   Sep 22, 2008 - Qiang XU, Modification for optimization of AT SMS module
*
*  Notes:
*
******************************************************************************/

#ifndef TELPB_H
#define TELPB_H

#include "ci_api_types.h"
#include "ci_pb.h"

enum {
	OP_NULL = 0,
	OP_CPBS,
	OP_CPBW,
	OP_CPBR,
	OP_CPBF //Added By Michal Bukai
};

RETURNCODE_T  ciReadPB(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciSelectPBStorage(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciWritePB(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciFindPBEntries(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p); /* Added by Michal Bukai*/

#endif

