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

/******************************************************************************
*               MODULE IMPLEMENTATION FILE
*******************************************************************************
* Title: Error Handler
*
* Filename: EEHandler_nvm.h
*
* Target, platform: Hermon
*
* Authors: Anton Eidelman
*
* Description: Implementation file: Error handler configuration and NVM definitions
*
*
*
* Notes:
******************************************************************************/


#ifndef _EEHANDLER_NVM_
#define _EEHANDLER_NVM_

//#include "global_types.h"

#ifdef __cplusplus
extern "C" {
#endif


#if !defined (OSA_WINCE) && !defined (OSA_LINUX)
#include "nvm_header.h"
#else
#include "EEHandler.h"
#include "EEHandler_config.h"
#endif

#if defined (_TAVOR_HARBELL_)
 #define NVM_ON_REMOTE  1
#else
 #define NVM_ON_REMOTE  0
#endif


#if defined (OSA_WINCE) || defined (OSA_LINUX)
//ICAT EXPORTED STRUCT
typedef struct NVM_Header
{
	unsigned long StructSize;       // the size of the user structure below
	unsigned long NumofStructs;     // >1 in case of array of structs (default is 1).
	char StructName[64];            // the name of the user structure below
	char Date[32];                  // date updated by the ICAT when the file is saved. Filled by calibration SW.
	char time[32];                  // time updated by the ICAT when the file is saved. Filled by calibration SW.
	char Version[64];               // user version - this field is updated by the SW eng. Every time they update the UserStruct.
	char HW_ID[32];                 // signifies the board number. Filled by calibration SW.
	char CalibVersion[32];          // signifies the calibration SW version. Filled by calibration SW.
} NVM_Header_ts;
#endif

//ICAT EXPORTED ENUM
typedef enum
{
	EE_RESET,
	EE_STALL,
	EE_RESERVED,
	EE_EXTERNAL,
	EE_RESET_START_BASIC,
	EE_RESET_COMM_SILENT
#if !defined(COMPILER_DOES_NOT_SUPPORT_SHORT_ENUMS) || defined(_PPC_)
}FinalAct_t;
#else
} FinalAct_t_;
typedef UINT8 FinalAct_t;
#endif

#define EE_RESET_COMM_SILENT    (EE_RESET_START_BASIC + 1)
#define EE_RESET_GPIO_RESET             (EE_RESET_COMM_SILENT + 1)
#define EE_RESET_FULL_UI                (EE_RESET_GPIO_RESET + 1)
#define EE_RAMDUMP                (EE_RESET_FULL_UI + 1)



//ICAT EXPORTED ENUM
typedef enum
{
	EE_OFF,
	EE_ON
#if !defined(COMPILER_DOES_NOT_SUPPORT_SHORT_ENUMS) || defined(_PPC_)
}EE_OnOff_t;
#else
} EE_OnOff_t_;
typedef UINT8 EE_OnOff_t;
#endif


//ICAT EXPORTED ENUM
typedef enum
{
	EE_WARN_OFF,
	EE_WARN_ASSERTCONTINUE,
	EE_WARN_ASSERT
#if !defined(COMPILER_DOES_NOT_SUPPORT_SHORT_ENUMS) || defined(_PPC_)
}EE_WarningOn_t;
#else
} EE_WarningOn_t_;
typedef UINT8 EE_WarningOn_t;
#endif



//ICAT EXPORTED ENUM
typedef enum
{
	EE_VER_3 = 3,
	EE_VER
#if !defined(COMPILER_DOES_NOT_SUPPORT_SHORT_ENUMS) || defined(_PPC_)
}EE_Version_t;
#else
} EE_Version_t_;
typedef UINT8 EE_Version_t;
#endif


//ICAT EXPORTED ENUM
typedef enum
{
	EE_HSL_OFF = 0,
	EE_HSL_1_8V = 1,
	EE_HSL_3V = 3
#if !defined(COMPILER_DOES_NOT_SUPPORT_SHORT_ENUMS) || defined(_PPC_)
}EE_HSL_V_t;
#else
} EE_HSL_V_t_;
typedef UINT8 EE_HSL_V_t;
#endif



//ICAT EXPORTED ENUM
typedef enum
{
	EE_WDT_OFF        =  0,
	EE_WDT_TIME_6SEC  =  6000,      //in miliseconds; for max WCDMA/GSM DRX cycle
	EE_WDT_TIME_7SEC  =  7000,      //in miliseconds
	EE_WDT_TIME_8SEC  =  8000,      //in miliseconds
	EE_WDT_TIME_10SEC = 10000,      //in miliseconds
	EE_WDT_TIME_20SEC = 20000,      //in miliseconds
	EE_WDT_TIME_30SEC = 30000,      //in miliseconds
	EE_WDT_TIME_MAX   = 0xFFFF      //UINT16
#if !defined(COMPILER_DOES_NOT_SUPPORT_SHORT_ENUMS) || defined(_PPC_)
}EE_WdtTimeCfg_t;
#else
} EE_WdtTimeCfg_t_;
typedef UINT16 EE_WdtTimeCfg_t;
#endif



#define EE_DEFERRED_ACTIONS

#if defined(EE_DEFERRED_ACTIONS)

//ICAT EXPORTED ENUM
typedef enum
{
	EE_DEFER_NONE,
	EE_DEFER_ASSERTS,
	EE_DEFER_EXCEPTIONS,
	EE_DEFER_ALL
#if !defined(COMPILER_DOES_NOT_SUPPORT_SHORT_ENUMS) || defined(_PPC_)
}EE_DeferredMode_t;
#else
} EE_DeferredMode_t_;
typedef UINT8 EE_DeferredMode_t;
#endif


//ICAT EXPORTED STRUCT
typedef struct
{
	EE_DeferredMode_t enable;
	EE_OnOff_t deferIntermediateActions;
	UINT16 limitMs;
	UINT16 limitHits;
	UINT16 reserved2;
}EE_DeferredCfg_t;
#endif

//ICAT EXPORTED STRUCT
typedef struct
{
	EE_OnOff_t AssertHandlerOn;
	EE_OnOff_t ExcepHandlerOn;
	EE_WarningOn_t WarningHandlerOn;
	EE_OnOff_t powerUpLogOn;
	EE_OnOff_t extHandlersOn;
	EE_OnOff_t printRecentLogOnStartup;
	FinalAct_t finalAction;
	UINT32 EELogFileSize;
	UINT16 delayOnStartup;     // Delay for ICAT log coverage in 5ms units
	EE_ContextType_t assertContextBufType;
	EE_ContextType_t exceptionContextBufType;
	EE_ContextType_t warningContextBufType;
#if defined(EE_DEFERRED_ACTIONS)
	//-------- version 1+D ----------
	EE_DeferredCfg_t deferredCfg;
#else
	UINT8 reserved1[8];
#endif
	EE_WdtTimeCfg_t wdtConfigTime;  //UINT16
	UINT16 sysEeHandlerLimit;       //relevant for EE_ASSISTING_MASTER only; ZERO is no limits
	UINT32 dumpDdrSizeBytes;        //relevant for EE_ASSISTING_MASTER only
  UINT8            reserved[24];
}EE_Configuration_t;


typedef enum
{
	EE_NVMCONFIG_UNCODITIONAL,
	EE_NVMCONFIG_EARLY,     /* used for Non NVM_ON_REMOTE */
	EE_NVMCONFIG_LATEST     /* used for NVM_ON_REMOTE     */
}EE_NvmConfigMode_t;


//
// EEHandler Internal services
//

//
BOOL eeReadConfiguration(EE_Configuration_t* pCfgBuf);
void eeSaveConfiguration( void );
void  eeSetConfiguration(const EE_Configuration_t* pConfig, int len);
void eePrintConfiguration( void );
void eeInitDfltConfiguration(void);
BOOL eeInitNvmConfigSuccess(EE_NvmConfigMode_t mode);
//#ifdef __cplusplus
void eeInitConfiguration(void);
//#endif

//
// EEHandler Internal data
//

extern EE_Configuration_t eeConfiguration;

#ifdef __cplusplus
}
#endif

#endif

