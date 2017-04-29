/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (“Material”) are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

#ifndef _EEHANDLER_CONFIG_H_
#define _EEHANDLER_CONFIG_H_

#include "diag_API.h"


/*----------- Global defines -------------------------------------------------*/
#define EXCEPTION_HANDLER_BUF_SIZE            1   /* only 1-entry is currently supported */
#define TRACE_BUF_SIZE                        5
#define EXCEPTION_FUN_ARRAY                   15
#define ERROR_HANDLER_MAX_DESC_SIZE           100
#define EE_MAX_INTERMEDIATE_ACTIONS           10

//#define EE_WARNINGS
#if 0
#define DIAG_FILTER(CAT1, CAT2, CAT3, reportLevel) do {} while (0);
#define diagPrintf(...)                    eeh_log(LOG_NOTICE, __VA_ARGS__)
#define diagTextPrintf(text)               do {} while (0)
#define diagStructPrintf(text, ptr, nbytes)  do {} while (0)
#endif

#define EE_INTERNAL_ASSERT_FATAL
//#define EE_INTERNAL_ASSERT_DIAG
#define EE_CREATE_DEFAULT_CONFIGURATION_FILE
#if !defined (_TAVOR_HARBELL_)
 #define EE_USE_TRACE_BUFFER
#endif
#define EE_TASK_INFO

// Enables the "Deferred actions" feature
#define EE_DEFERRED_ACTIONS

// Enable to run deferred intermediate actions (e.g. LCD printout) not under the thread callback,
// but under the low priority task (public service). This is recommended, because not all actions
// can always be run under the thread callback, which might be an HISR (LCD print impossible).
// Still, using the low priority task service might be not guaranteed under certain error conditions.
#if !defined (_TAVOR_HARBELL_)
	#define EE_DEFERRED_INTERMEDIATE_IN_LOW_EVENT
#endif

// Set OS current thread to a known task thread before running the deferred final action
// This allows the final action code to use OS calls, which are not allowed under HISR
// (relevant external final action).
// NOTE: these OS calls must not be blocking (de-facto), otherwise scheduling might be resumed causing unpredictable results.
#define EE_DEFERRED_FINAL_ACTION_FAKE_SAFE_TASK_THREAD

// Enables binding of an extended exception handler to catch all exceptions
// Extended handler allows the system to recognize and ignore certain exceptions
#define EE_EXT_EXCEPTION_HANDLER

// Synchronize the external memory to allow "post mortem" memory content upload: effective for non-reset final actions
#define EE_FLUSH_DCACHE_BEFORE_FINAL_ACTION


#endif  //_EEHANDLER_CONFIG_H_

