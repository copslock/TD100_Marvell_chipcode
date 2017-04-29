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

/******************************************************************************
*               MODULE IMPLEMENTATION FILE
*******************************************************************************
* Title: Error Handler
*
* Filename: EEHandler_int.h
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

#ifndef _EEHANDLER_INT_
#define _EEHANDLER_INT_

#if !defined(OSA_WINCE)
#include "global_types.h"
#if !defined (OSA_LINUX)
#include "hal_cfg.h"
#include "EEHandler_int.h"
#endif
#endif

#if defined (INTEL_FDI) && !defined(OSA_WINCE)
#if defined (OSA_LINUX)
#include "fdi_file.h"
#else
#include "fdi_ext.h"
#include "fdi_file.h"
#include "fdi_err.h"
#endif
#endif


#include "EEHandler_config.h"
#include "EEHandler_nvm.h"

/*----------- Global macro definitions ---------------------------------------*/

#define EE_HANDLER_DISABLE_INTERRUPTS(x)   x = disableInterrupts()
#define EE_HANDLER_ENABLE_INTERRUPTS(x)    restoreInterrupts(x)

#if defined (OSA_LINUX)
/* This is not important error, just print it */
#define EE_INTERNAL_ASSERT(x) do { \
		fprintf(stderr, "ASSERT FAIL AT FILE %s FUNC %s LINE %d", __FILE__, __FUNCTION__, __LINE__); \
} while (0)
#else
void eeAssert(int x);
#define EE_INTERNAL_ASSERT(x) eeAssert(x)
#endif

#if defined (_TAVOR_HARBELL_)
	#define WATCHDOG_ReturnCode WDT_ReturnCodeE
#endif

/*
 * File names
 */
extern const char* const eehLogFileName;

// INTERNAL PROTOTYPES
void exceptionHandlersBind(void);

void        exceptionHandlerPhase1Init( void );
void        exceptionHandlerPhase2Init( void );

//Only for the EE
void excepHandlerSaveExceptionToFile( void );
int EeHandlerPrintEntry(const EE_Entry_t* entry);
void errHandlerSaveEntryToFile(EE_Entry_t* entry);
void errHandlerPreFinalAction(FinalAct_t action);
void errHandlerFinalAction(FinalAct_t action);
EEHandlerAction eeExtExceptionHandler(EE_ExceptionType_t type, EE_RegInfo_Data_t* pContext);


void        ExceptionInChipTraceBufferCopy(void);

typedef struct
{
	UINT16 logSignature;
	UINT16 logChksum;
	EE_Entry_t log[EXCEPTION_HANDLER_BUF_SIZE];
} EE_LogBuffer;

#define EE_RAM_BUFFER_SIGNATURE 0x57E2     /*magic*/

/*----------- Extern definition ----------------------------------------------*/
/*extern*/
extern EE_RegInfo_Data_t __Saved_Registers;
extern void Final_Action_Reset(void);


#endif

