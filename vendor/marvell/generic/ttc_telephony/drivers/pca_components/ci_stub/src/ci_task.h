/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************
**
**
**  FILENAME:       ci_task.h
**
**  PURPOSE:

   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.

   Unless otherwise agreed by Intel in writing, you may not remove or alter this notice or any other notice embedded
   in Materials by Intel or Intel’s suppliers or licensors in any way.
******************************************************************************/
#ifndef _CI_TASK_H_
#define _CI_TASK_H_

#include "ci_config.h"
#include "osa.h"
#include "ci_trace.h"
//#include "stbc_api.h"

#define CI_STUB_MESSAGE_Q_SIZE          16      /* sizeof(MslMessage) */
#define CI_STUB_MESSAGE_Q_MAX           256     // 32 //16 Changed for Data_indication
#define CI_STUB_STACK_SIZE                      2048

#ifdef CI_STUB_SERVER_INCLUDE
extern OSAMsgQRef ciServerStubMsgQ;
void ciServerStubTask(void *input);
#endif

#ifdef CI_STUB_CLIENT_INCLUDE
extern BOOL gDlLinkDownFlag;
extern OSAMsgQRef ciClientStubMsgQ;
void ciClientStubTask(void *input) __attribute__((noreturn));
#endif

#endif // end header


