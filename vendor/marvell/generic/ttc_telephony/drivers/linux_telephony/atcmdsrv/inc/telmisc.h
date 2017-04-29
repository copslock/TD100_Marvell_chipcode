/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************
*  Filename: telmisc.h
*
*  Authors: 
*
*  Description: Process WIFI/BT/FM related AT commands and other misc functions
*
*  History:

******************************************************************************/
#ifndef TELMISC_H
#define TELMISC_H

RETURNCODE_T ciMVWFTST( const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters,	const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T ciMVWFIDRW( const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters,	const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T ciMVBTTST( const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters,	const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T ciMVBTIDRW( const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters,	const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T ciMVFMTST( const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters,	const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T ciSWUPGRADE( const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters,	const char *info_text_p, unsigned int *xid_p, void *arg_p);

#endif

