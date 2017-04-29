/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*     File name:      Diag_config .h                                              *
*     Programmer:     Shiri Dolev                                                 *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*       Create Date:  November, 2003                                              *
*                                                                                 *
*       Description: DIAG Configuration File.                                                                      *
*                                                                                 *
*       Notes:                                                                    *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if !defined (_DIAG_CONFIG_H_)
#define _DIAG_CONFIG_H_

///#include "osa.h"
#include "diag_osif.h"
/************************************************************************\
* Global definitions
\************************************************************************/

/* definition of new header version */
#define DIAG_NEW_HEADER_VERSION   0x02

/* definition of USERS_ID */
#define DIAG_ICAT_USER  0x01
#define DIAG_APPS_USER  0x02
#define DIAG_COM_USER   0x03

#define MAX_DIAG_USER   DIAG_COM_USER

/************************************************************************\
* Definitions of SAP values
\************************************************************************/

#if defined (DIAGNEWHEADER)

/* definition of SAPs of new header */

/* SAPs of requests */
#define SAP_HANDSHAKE                                           0x00
#define SAP_INTERNAL_SERVICE_REQUEST            0x01
#define SAP_COMMAND_SERVICE_REQUEST                     0x02
#define SAP_FIXUP_COMMAND_SERVICE_REQUEST       0x03
#define SAP_SIGNAL_SERVICE_REQUEST                      0x04
#define SAP_SIGNAL_OR_FILTER_REQUEST            0x05
// 8,9,10 CMI SAPS
#define SAP_C2C_REQUEST                                         0x0b            //???

/* SAPs of responses/reports */
#define SAP_INTERNAL_SERVICE_RESPONSE           0x80
#define SAP_TRACE_NOT_ALIGNED_RESPONSE          0x81
#define SAP_TRACE_RESPONSE                                      0x82
#define SAP_MI_SIGNAL_RESPONSE                          0x83
#define SAP_LOGGED_SIGNAL_RESPONSE                      0x84
#define SAP_LOGGED_SPECIALSIGNAL_RESPONSE       0x85
#define SAP_ERROR                                                       0x86
// 8,9,10 CMI SAPS
#define SAP_C2C_RESPONSE                                        0x8b

#define MIN_REQUEST_SAP                                         SAP_HANDSHAKE
#define MAX_REQUEST_SAP                                         SAP_C2C_REQUEST
#define MIN_RESPONSE_SAP                                        SAP_INTERNAL_SERVICE_RESPONSE
#define MAX_RESPONSE_SAP                                        SAP_C2C_RESPONSE

// SAPs which do not have 'sap related data'
#define IS_SAP_HAS_RELATED_DATA(sap_type)			\
	((sap_type != SAP_SIGNAL_SERVICE_REQUEST) &&	\
	 (sap_type != SAP_LOGGED_SIGNAL_RESPONSE) &&	\
	 (sap_type != SAP_SIGNAL_OR_FILTER_REQUEST) &&	\
	 (sap_type != SAP_LOGGED_SPECIALSIGNAL_RESPONSE))

#else
/* definition of SAPs of old header */

/* RX DiagSAP's */
#define INTERNAL_SERVICE                        0
#define COMMAND_SERVER                          1
#define FIXUPS_COMMAND_SERVER           3
#define GKI_CMD_SERVER              5
#define SIGNAL_SERVER                           6
// 8,9,10 CMI SAPS
#define CLIENT_TO_MASTER_CMM		9
#define CORE_TO_CORE_MESSAGE_RX         11      // must be aligned with the tx...

/* TX DiagSAP's */
#define APPLICATION_REPORT                      0
#define RETURN_SERVICE                          1
#define TRACE                                           4
#define LOGGED_MI                                       5
#define LOGGED_SIGNAL_REALLOC       7
// 8,9,10 CMI SAPS
#define CORE_TO_CORE_MESSAGE_TX         11

// SAPs which do not have 'sap related data'
#define IS_SAP_HAS_RELATED_DATA(sap_type)			\
	((sap_type != LOGGED_MI) &&	\
	 (sap_type != GKI_CMD_SERVER) && \
	 (sap_type != SIGNAL_SERVER) &&	 \
	 (sap_type != LOGGED_SIGNAL_REALLOC))

#endif /* DIAGNEWHEADER */

#if !defined(DIAG_PRIORITY)
#if defined (OSA_WINCE)
#define DIAG_PRIORITY                   6
#else
#define DIAG_PRIORITY                   DIAG_DEFUALT_PRIORITY
#endif
#endif

#if !defined(DIAG_DEFAULT_STACK_SIZE)
#define DIAG_DEFAULT_STACK_SIZE                 0x2000
#endif

#define DIAG_RX_MAX_NUM_OF_MSG                  0x0a

#define DIAG_MAX_RX_MSG_SIZE                    4096     //the max size of TTP Pool

#if !defined (DIAG_STACK_SIZE)
#define DIAG_STACK_SIZE                                 DIAG_DEFAULT_STACK_SIZE
#endif

#define DIAG_MAX_PRINTF_DATA_SIZE       2048

#endif /*_DIAG_CONFIG_H_*/
