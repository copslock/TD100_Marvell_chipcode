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
 *  Filename: telcontroller.h
 *
 *  Authors:  Vincent Yeung
 *
 *  Description: Telephony Controller Implementation for AT Command Server
 *
 *  History:
 *   May 19, 2006 - Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/

#ifndef TELCONTROLLER_H
#define TELCONTROLLER_H

#include "telatci.h"
#include "utlAtParser.h"

BOOL tcInit( char * ext_serial_path, char* modem_serial_path);
int tcWriteParser(TelAtParserID sAtpIndex, char * string, int len);


utlReturnCode_T atParserShellSetFunction(const utlAtParameterOp_T op,
					 const char                      *command_name_p,
					 const utlAtParameterValue_P2c parameter_values_p,
					 const size_t num_parameters,
					 const char                      *info_text_p,
					 unsigned int                    *xid_p,
					 void                            *arg_p);

utlReturnCode_T atParserShellGetFunction(const utlAtParameterOp_T op,
					 const char                      *command_name_p,
					 const utlAtParameterValue_P2c parameter_values_p,
					 const size_t num_parameters,
					 const char                      *info_text_p,
					 unsigned int                    *xid_p,
					 void                            *arg_p);

utlReturnCode_T ciAT(            const utlAtParameterOp_T op,
				 const char                      *command_name_p,
				 const utlAtParameterValue_P2c parameter_values_p,
				 const size_t num_parameters,
				 const char                      *info_text_p,
				 unsigned int                    *xid_p,
				 void                            *arg_p);

int getFd(TelAtParserID sAtpIndex);
int notifyAtParser(TelAtParserID sAtpIndex, unsigned char cmd);
int triggerAtParser(TelAtParserID sAtpIndex);
int tcWriteParser(TelAtParserID sAtpIndex, char * string, int len);

#endif

/* END OF FILE */
