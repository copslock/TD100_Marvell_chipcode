/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (“Material<94>) are owned
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
#if defined (OSA_WINCE)
#include "EEHandler_nvm.h"
#endif


#ifndef EEHANDLER_NVM_DEFAULT
#define EEHANDLER_NVM_DEFAULT

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This file defines the default configuration set by EEHandler when no NVM file is found
// In order to override the settings copy the file into target\inc and modify
//
// Do not include this file from any module other than EEHandler_config.h
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(EEHANDLER_NVM_VERSION)
#define EEHANDLER_DEFAULT_LOGFILE_SIZE     (10 * sizeof(EE_Entry_t))

/* ASSERTS should not return */
const EE_Configuration_t eeDefaultConfiguration = {
	EE_ON,                                                  //  EE_OnOff_t       AssertHandlerOn;
	EE_ON,                                                  //  EE_OnOff_t       ExcepHandlerOn;
	EE_WARN_OFF,                                            //  EE_WarningOn_t    WarningHandlerOn;
	EE_OFF,                                                 //  EE_OnOff_t       powerUpLogOn;
	EE_ON,                                                  //  EE_OnOff_t       extHandlersOn;
	EE_ON,                                                  //  EE_OnOff_t       printRecentLogOnStartup;
	EE_STALL,                                               //  FinalAct_t       finalAction;
	EEHANDLER_DEFAULT_LOGFILE_SIZE,                         //  UINT32           EELogFileSize;
	0,                                                      //  UINT16           delayOnStartup; // Delay
	EE_CT_StackDump,                                        //  EE_ContextType_t assertContextBufType;
	EE_CT_StackDump,                                        //  EE_ContextType_t exceptionContextBufType;
	EE_CT_StackDump,                                        //  EE_ContextType_t warningContextBufType;
#if defined(EE_DEFERRED_ACTIONS)
	{ EE_DEFER_ALL, EE_ON, 500 /*0.5s*/, 20, 0 },           //EE_DeferredCfg_t deferredCfg;
#else
	0,                                                      //UINT8            reserved1[8];
#endif
#if defined(_TAVOR_HARBELL_)
	EE_WDT_TIME_10SEC                         //EE_WdtTimeCfg_t  wdtConfigTime;  //UINT16
#else
	EE_WDT_OFF
#endif
,	0,0,
        {0}
};

#else //EEHANDLER_NVM_VERSION
#error  EEHANDLER_NVM_VERSION   IS NOT SUPPORTED
#endif //EEHANDLER_NVM_VERSION

#endif

