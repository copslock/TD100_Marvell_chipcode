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

/*********************************************************************
 *                      M O D U L E     B O D Y                       *
 **********************************************************************
 * Title: Diag                                                        *
 *                                                                    *
 * Filename: diag_comm_EXTif.h                                        *
 *                                                                    *
 * Target, platform: Common Platform, SW platform                     *
 *                                                                    *
 * Authors:                                                           *
 *                                                                    *
 * Description: Handles the external interface (interaction with ACAT)*
 *                                                                    *
 * Notes:                                                             *
 *                                                                    *
 *                                                                    *
 *                                                                    *
 *                                                                    *
 ************** General include definitions ***************************/

#if !defined (_DIAG_COMM_EXT_IF_H_)
#define _DIAG_COMM_EXT_IF_H_

//#include "ICAT_config.h"
#include "osa.h"
#include "osa_types.h" //OS_Mutex_t
//#include "diag_comm.h"
//#include "diag_comm_if.h"

#if !defined (ASSERT_NO_DIAG)
#define ASSERT_NO_DIAG DIAG_ASSERT
#endif

#define DIAG_COM_USB_MAX_PACKET_BYTE_SIZE              4096

#if defined (OSA_WINCE)   // stack  is not used by OSA in WinCE - set to small value
#define   DIAG_EXT_IF_STACK_SIZE                256
#define   DIAG_EXT_IF_TASK_PRIORITY     6
#elif defined (OSA_LINUX)
#define   DIAG_EXT_IF_STACK_SIZE        1024
#define   DIAG_EXT_IF_TASK_PRIORITY     42   /* Low (non-RT) to collisions with prevent real-time tasks */
#else
#define   DIAG_EXT_IF_STACK_SIZE        1024
#define   DIAG_EXT_IF_TASK_PRIORITY     42
#endif

//Cached Write mechanism
typedef struct {
	UINT32 cachePtr;
	OS_Mutex_t diag_cached_write_mutex;
	OSTimerRef diag_cached_write_timer_ref;
	UINT8      *cachedWriteBuff;
} CachedWriteS;

// functions called from specific OS implementation
void diagExtIFStatusDiscNotify(void);
void diagExtIFstatusConnectNotify(void);

// functions implemetned by specific OS:
void diagCommExtIFosSpecificInit(void);
void diagCommExtIfConnInit(void);
UINT32 diag_os_TransmitToExtIfMulti(UINT16 nOfBuffers, UINT16 pos);
void diag_os_TransmitToExtIf(UINT16 pos);
void diagExtIfcon_discon(UINT32 disconnect_flag, UINT32 connect_flag);

#endif //_DIAG_COMM_EXT_IF_H_
