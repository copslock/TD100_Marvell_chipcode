/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*
   Intel Confidential
   Copyright © [dates], Intel Corporation
 */
/***************************************/
//exception handler
/***************************************/





#if defined (OSA_WINCE)
#include <windows.h>
#include <ceddk.h>
#include <Regext.h>
#include <diag_api.h>
#include <Oal_memory.h>
#include <Winbase.h>
//#include "EEHandler_nvm.h"
//#define EE_DEFERRED_ACTIONS
#elif defined (OSA_LINUX)
#include <stdio.h>
#include <string.h>
#include "loadTable.h"
#include "pxa_dbg.h"
#else
#include <string.h>             // for memcpy() & strcpy()
#include "fstdio_wrap.h"        /*must be before <stdio.h> */
#include <stdio.h>
#include "bsp_hisr.h"
#include "mmap.h"
#include "asserts.h"
#include "watchdog.h"
#include "watchdog_apps4com.h"
#include "loadTable.h"
#include "hal_cfg.h"
#include "utils.h"
#include "lowtasks.h"

#include "bspUartManager.h"

#if !defined (_TAVOR_HARBELL_)
#include "manitoba_init.h"
#include "leds.h"
#include "mmu.h"
#else //_TAVOR_HARBELL_
extern void asm_Arm946e_flush_caches(void);
#define mmuFullDataCacheClean      asm_Arm946e_flush_caches
extern void Delay(unsigned long uSec);
#endif  //_TAVOR_HARBELL_
#endif  //OSA_WINCE

#include "osa.h"
#include "EEHandler.h"
#include "EE_Postmortem.h"
#include "EEHandler_nvm.h"
#include "EEHandler_int.h"


#if defined (OSA_WINCE)
//OSA encapsulations (finally to be present in the "osa.h")
#undef  OSATaskSleep
#define OSATaskSleep(tICK)    Sleep(5 * tICK)  /*WIN Sleep has resolution 1mS*/
#endif

#if !defined (OSA_WINCE) && !defined (OSA_LINUX)
PMU_LastResetStatus  PMULastReset(void);  //"pmu.h"

static void EE_fstdioConfigOnAssert(void);

#if defined(MIPS_TEST_RAM)
#include "mipsram_def.h"
#endif

#if defined (DATA_COLLECTOR_IMPL)
#include "ISPTExternalAPI.h"
static void ISPTSendDataEE(const char*                cond,
			   const char*                file,
			   int line,
			   EE_entry_t type,
			   const EE_RegInfo_Data_t*   regContext,
			   RTC_CalendarTime*          calendarTime);
#endif
#endif //OSA_WINCE



// Some Platforms (Architectures) have pair of APPS/COMM master/slave EE-handler sub-components.
// In this case master may assist to slave with collecting debug information and presentation to user.
//  (this requires a "Notification")

#if defined (_TAVOR_HARBELL_)
#define EEH_ASSISTED_CLIENT
#endif
#if defined (_TAVOR_BOERNE_)
#define EEH_ASSISTING_SERVER
#endif
//#else  EE_selfcontain

#if defined(NO_FDI) || !defined(INTEL_FDI)
#undef EEH_ASSISTED_CLIENT
#undef EEH_ASSISTING_SERVER
#endif


extern BOOL WDT_EXPIRED_withoutAssert;

#if defined(EEH_ASSISTING_SERVER)
extern UINT32 EE_AssistingMaster_ASSERT_found;
UINT32 SysEeHandlerEntryCntr;
#endif

#if defined(EEH_ASSISTED_CLIENT)
  #define EEH_DUMP_DTCM
#endif

#if !defined (_TAVOR_HARBELL_)
#define COPROCESSOR_FSR_REGISTER_USED
#endif

#if defined (_TAVOR_HARBELL_) || defined (_TAVOR_BOERNE_)
extern void PMdisablePMfeautures(void);
#else
static void PMdisablePMfeautures(void)
{
	;
}                                          //filler
#endif

// Extra exception types
#define FSR_DEBUG_EVENT 0x200

// Watchdog
#define EE_WATCHDOG_TIME_IMMEDIATE 5 //ms  -  minimal time

/*----------- Globals --------------------------------------------------------*/
EE_RegInfo_Data_t __Saved_Registers;     //this can be deleted and I can use the EEirambuffer directly

#define EE_HANDLER_ENABLE
#if defined(EE_HANDLER_ENABLE)

#if defined(EE_EXT_EXCEPTION_HANDLER)
#define EE_EXT_EXCEPTION_CATCH(type) \
	if (eeExtExceptionHandler(type, &__Saved_Registers) == eehActionIgnore) return EE_EXC_RETURN;
#else
#define EE_EXT_EXCEPTION_CATCH(type)
#endif

extern EE_LogBuffer __CurrentExceptionLog;

/*----------- Local defines --------------------------------------------------*/

/*----------- Local macro definitions ----------------------------------------*/

/*----------- Local macro definitions ----------------------------------------*/


/*----------- Local type definitions -----------------------------------------*/
// Optional actions that can be disabled or enabled based on configuration and scenario (risk)
typedef enum
{
	EE_MODE_NONE,
	EE_MODE_DEFERRED = 1,
	EE_MODE_INTERMEDIATE = 2,
	EE_MODE_LOG = 4,
	EE_MODE_ALLOW_EXT = 8,
	EE_MODE_LEDS = 0x10,
	EE_MODE_DEFERRED_INTERMEDIATE = 0x20
} EE_Mode_t;

#define EE_MODE_DEFAULT (EE_MODE_INTERMEDIATE | EE_MODE_LOG | EE_MODE_ALLOW_EXT | EE_MODE_LEDS)
/*----------- Local variable definitions -------------------------------------*/

#if (!defined (OSA_WINCE) && !defined (OSA_LINUX)) || defined (DATA_COLLECTOR_IMPL)
static EE_Entry_t           *PhysicalExcepBuffer_p;
static UINT16 _exceptionSRAMBufIndex = 0;
#endif

typedef struct
{
	FatalEventAction_fn* pFunc;
	UINT32 mode;
} EeIntermediateAction_ts;

#if !defined (OSA_WINCE) && !defined (OSA_LINUX)
static EeIntermediateAction_ts eeIntermediateActions[EE_MAX_INTERMEDIATE_ACTIONS];
static FatalEventAction_fn* eeFatalActionExt;
static UINT32 eeEntryCount;             // increments on every entry for deferred final action case support
#endif

#if defined(EE_DEFERRED_ACTIONS) && !defined (OSA_WINCE) && !defined (OSA_LINUX)
// Exception mode stack top addresses: saved on init, used to reset exception stacks
static UINT32 eeAbortStack, eeUndefStack;
#endif

#if defined (ENABLE_DIAG_LOGGER)
extern UINT8* pNextFreeSpace;
#endif // defined (ENABLE_DIAG_LOGGER)


//----- Serial port definitions ----------------
//#define SERIAL_NOT_ON_BOARD
#if defined (_TAVOR_BOERNE_)
#define USE_SERIAL_EE_AS_FINAL_ACTION   /*config this if WDT cannot be applied*/
					/* used with eeConfiguration.finalAction=EE_NON_WDT_SERIAL */
#endif

#if !defined (SERIAL_NOT_ON_BOARD)
extern FatalEventAction_fn finalActionSerialLoop;
#endif

#if defined (USE_SERIAL_EE_AS_FINAL_ACTION) && !defined (OSA_WINCE) && !defined (OSA_LINUX)
static void NonWdtFinalAction(BOOL force);
#endif

#if !defined (OSA_WINCE) && !defined (OSA_LINUX)
/*----------- Local functions -------------------------------------*/
static void exceptionIRAMsave(const char* desc, const char* file, int line, EE_entry_t type,
			      const EE_RegInfo_Data_t* regContext, const RTC_CalendarTime* calendarTime,
			      const UINT8* data, UINT32 dataLen);
static void exceptionIRAMMarkValid(int valid);
static int  exceptionIRAMCheckValid(void);
#endif
void eehHandleFatalEvent(const char* desc, const char* file, int line, EE_entry_t type, const UINT8* data, UINT32 dataLen);

#if !defined (OSA_WINCE) && !defined (OSA_LINUX)
static WATCHDOG_ReturnCode eeWatchdogActivate(void);

static void errFinalActionStall(void);
static void eeAdjustRegisters(EE_ExceptionType_t et, EE_RegInfo_Data_t* regContext);
#endif

#if defined(EE_DEFERRED_ACTIONS) && !defined(OSA_LINUX)
static BOOL eeIsRecoverable(void);
static void eeParkThread(EE_entry_t type, void (*callbackFunc)(void));
static void eeDeferredFinalActionRun(BOOL notify2masterRequired);
static void eeDeferredFinalActionLoad(FinalAct_t fAction);
static void eeDeferredIntermediateActionRun(void);



static WATCHDOG_ReturnCode eeWatchdogActivateExt(UINT32 timeMs, BOOL reset);

#if !defined(OSA_WINCE)
static void eePrintDeferredError(void);
#endif

#endif //EE_DEFERRED_ACTIONS

#if !defined (OSA_WINCE) && !defined (OSA_LINUX)
static void eeNotify2AssistMaster(void);
static void EeHandlerWarningKeepInfoInEEbuf(int ExtendMode);
#endif
void EeHandlerWarningINFO( void );




#if defined (EEH_ASSISTED_CLIENT) || defined (EEH_ASSISTING_SERVER)
#define EELOG_MAX_STREAMS         10
static UINT32 NumLogstreams;
#endif

#if defined (EEH_ASSISTED_CLIENT)
static voidPFuncVoid*  eeLogStream[EELOG_MAX_STREAMS];

static FILE*   EELOG_fopen(const char * fileName, const char *modeStr);
static int     EELOG_fclose(FILE* handl);
static size_t  EELOG_fwrite(const void *buf, size_t chunkSize, size_t numChunks, FILE* handl);
#endif //EEH_ASSISTED_CLIENT

#if defined (EEH_DUMP_DTCM)
static void    EELOG_streamOutDTCM(void);
#endif


#if !defined (OSA_WINCE) && !defined (OSA_LINUX)


/*******************************************************************************
* Function: BranchZeroIntHandler
********************************************************************************
* Description: BranchZeroIntHandler
   .
*
* Parameters:
*
* Return value:
*
* Notes:
*******************************************************************************/
UINT32        BranchZeroIntHandler  ( void )
{
	if (!eeConfiguration.ExcepHandlerOn)
		return EE_EXC_STOP;
#ifdef EE_USE_TRACE_BUFFER
	if (eeConfiguration.exceptionContextBufType == EE_CT_ExecTrace)
		ExceptionInChipTraceBufferCopy();
#endif
	eeAdjustRegisters(EEE_ReservedInt, &__Saved_Registers);
	EE_EXT_EXCEPTION_CATCH(EEE_ReservedInt);
	eehHandleFatalEvent("Exception - Branch to 0x0 detected", NULL, 0, EE_EXCEPTION, NULL, 0);
	return EE_EXC_STOP;
} //end of BranchZeroIntHandler


/*******************************************************************************
* Function: FatalErrorHandler
********************************************************************************
* Description: FatalErrorHandler
   .
*
* Parameters:
*
* Return value:
*
* Notes:
*******************************************************************************/
UINT32        FatalErrorHandler   ( void )
{
	if (!eeConfiguration.ExcepHandlerOn)
		return EE_EXC_STOP;
#ifdef EE_USE_TRACE_BUFFER
	if (eeConfiguration.exceptionContextBufType == EE_CT_ExecTrace)
		ExceptionInChipTraceBufferCopy();
#endif

	eeAdjustRegisters(EEE_FatalError, &__Saved_Registers);
	EE_EXT_EXCEPTION_CATCH(EEE_FatalError);
	eehHandleFatalEvent("Exception - FatalError", NULL, 0, EE_EXCEPTION, NULL, 0);
	return EE_EXC_STOP;
} //end of FatalErrorHandler
/*******************************************************************************
* Function: SWInterruptHandler
********************************************************************************
* Description: SWInterruptHandler
   .
*
* Parameters:
*
* Return value:
*
* Notes:
*******************************************************************************/
UINT32        SWInterruptHandler  ( void )
{
	if (!eeConfiguration.ExcepHandlerOn)
		return EE_EXC_STOP;
#ifdef EE_USE_TRACE_BUFFER
	if (eeConfiguration.exceptionContextBufType == EE_CT_ExecTrace)
		ExceptionInChipTraceBufferCopy();
#endif
	eeAdjustRegisters(EEE_SWInterrupt, &__Saved_Registers);
	EE_EXT_EXCEPTION_CATCH(EEE_SWInterrupt);
	eehHandleFatalEvent("Exception - SW interrupt", NULL, 0, EE_EXCEPTION, NULL, 0);
	return EE_EXC_STOP;
} //end of SWInterruptHandler

/*******************************************************************************
* Function: UndefInstHandler
********************************************************************************
* Description: UndefInstHandler
   .
*
* Parameters:
*
* Return value:
*
* Notes:
*******************************************************************************/
UINT32        UndefInstHandler    ( void )
{
	if (!eeConfiguration.ExcepHandlerOn)
		return EE_EXC_STOP;
#ifdef EE_USE_TRACE_BUFFER
	if (eeConfiguration.exceptionContextBufType == EE_CT_ExecTrace)
		ExceptionInChipTraceBufferCopy();
#endif

	eeAdjustRegisters(EEE_UndefInst, &__Saved_Registers);
	EE_EXT_EXCEPTION_CATCH(EEE_UndefInst);
	eehHandleFatalEvent("Exception - Undefine instruction", NULL, 0, EE_EXCEPTION, NULL, 0);
	return EE_EXC_STOP;
} //end of UndefInstHandler

/*******************************************************************************
* Function: ReservedIntHandler
********************************************************************************
* Description: ReservedIntHandler
   .
*
* Parameters:
*
* Return value:
*
* Notes:
*******************************************************************************/
UINT32        ReservedIntHandler  ( void )
{
	if (!eeConfiguration.ExcepHandlerOn)
		return EE_EXC_STOP;
#ifdef EE_USE_TRACE_BUFFER
	if (eeConfiguration.exceptionContextBufType == EE_CT_ExecTrace)
		ExceptionInChipTraceBufferCopy();
#endif
	eeAdjustRegisters(EEE_ReservedInt, &__Saved_Registers);
	EE_EXT_EXCEPTION_CATCH(EEE_ReservedInt);
	eehHandleFatalEvent("Exception - Reserved interrupt", NULL, 0, EE_EXCEPTION, NULL, 0);
	return EE_EXC_STOP;
} //end of ReservedIntHandler

/*******************************************************************************
* Function: DataAbortHandler
********************************************************************************
* Description: Detect cause  for DATA ABORT
   .
*
* Parameters:
*
* Return value:
*
* Notes:
*******************************************************************************/
UINT32 DataAbortHandler( void )
{
	UINT8 i;
	EEHandlerAction eehAction = eehActionFatal;
	const char* descString = NULL;

	if (!eeConfiguration.ExcepHandlerOn)
		return EE_EXC_STOP;
#ifdef EE_USE_TRACE_BUFFER
	if (eeConfiguration.exceptionContextBufType == EE_CT_ExecTrace)
		ExceptionInChipTraceBufferCopy();
#endif

	eeAdjustRegisters(EEE_DataAbort, &__Saved_Registers);
	EE_EXT_EXCEPTION_CATCH(EEE_DataAbort);

#if !defined (COPROCESSOR_FSR_REGISTER_USED)
	i = 0;
	descString = "DataAbort";
#else
	// read FSR bits 0:3
	switch (i = (__Saved_Registers.FSR & 0x000F))
	{
	//MMU data abort  Alignment
	case ALIGNMENT_1:
	case ALIGNMENT_2:
		i = ALIGNMENT_1;
		descString = "DataAbort-ALIGNMENT";
		break;

	case EXTERNAL_ABORT_ON_TRANSLATION:
	case EXTERNAL_ABORT_ON_TRANSLATION_2:
		i = EXTERNAL_ABORT_ON_TRANSLATION;
		descString = "DataAbort-EXTERNAL_ABORT_ON_TRANSLATION";
		break;

	case TRANSLATION:
	case TRANSLATION_2:
		i = TRANSLATION;
		descString = "DataAbort-TRANSLATION";
		break;

	case EE_DOMAIN:
	case EE_DOMAIN_2:
		i = EE_DOMAIN;
		descString = "DataAbort-DOMAIN";
		break;

	case PERMISSION:
	case PERMISSION_2:
		i = PERMISSION;
		descString = "DataAbort-PERMISSION";
		break;
	//End of MMU data abort

	case LOCK_ABORT:
		i = PERMISSION;
		descString = "DataAbort-PERMISSION";
		break;

	case IMPRECISE_EXTERNAL_DATA_ABORT:
		descString = "DataAbort-IMPRECISE_EXTERNAL_DATA_ABORT";
		break;

	case DATA_CACHE_PARITY_ERROR:
		descString = "DataAbort-DATA_CACHE_PARITY_ERROR";
		break;
	default:
		i = 0;
		descString = "DataAbort-UNKNOWN";
	}

	if (!i)
	{
		// Extra exception types
		if (__Saved_Registers.FSR & FSR_DEBUG_EVENT) descString = "DATA BREAKPOINT";
	}
#endif //COPROCESSOR_FSR_REGISTER_USED

	eehAction = eehCallSecondaryHandler(i, &__Saved_Registers);
	if (eehAction == eehActionFatal) eehHandleFatalEvent(descString, NULL, 0, EE_EXCEPTION, NULL, 0);
	return EE_EXC_STOP;
} // End of DataAbortHandler()



/*******************************************************************************
* Function: PrefetchAbortHandler
********************************************************************************
* Description: Detect cause  for Prefetch ABORT
   .
*
* Parameters:
*
* Return value:
*
* Notes:
*******************************************************************************/
UINT32 PrefetchAbortHandler(void)
{
	UINT8 i;
	const char* descString = NULL;
	EEHandlerAction eehAction = eehActionFatal;

	if (!eeConfiguration.ExcepHandlerOn)
		return EE_EXC_STOP;
#ifdef EE_USE_TRACE_BUFFER
	if (eeConfiguration.exceptionContextBufType == EE_CT_ExecTrace)
		ExceptionInChipTraceBufferCopy();
#endif

	eeAdjustRegisters(EEE_PrefetchAbort, &__Saved_Registers);
	EE_EXT_EXCEPTION_CATCH(EEE_PrefetchAbort);

#if !defined (COPROCESSOR_FSR_REGISTER_USED)
	i = 0;
	descString = "PrefetchAbort";
#else
	// read FSR bits 3:0
	switch (i = (__Saved_Registers.FSR & 0x000F))
	{
	case INSTRUCTION_MMU:
		descString = "PrefetchAbort-INSTRUCTION_MMU";
		break;

	case EXTERNAL_INSTRUCTION_ERROR:
		descString = "PrefetchAbort-EXTERNAL_INSTRUCTION_ERROR";
		break;

	case INSTRUCTION_CACHE_PARITY_ERROR:
		descString = "PrefetchAbort-INSTRUCTION_CACHE_PARITY_ERROR";
		break;
	default:
		i = 0;
		descString = "PrefetchAbort-UNKNOWN";
	}

	if (!i)
	{
		// Extra exception types
		if (__Saved_Registers.FSR & FSR_DEBUG_EVENT) descString = "BREAKPOINT";
	}
#endif //COPROCESSOR_FSR_REGISTER_USED

	eehAction = eehCallSecondaryHandler(i, &__Saved_Registers);
	if (eehAction == eehActionFatal) eehHandleFatalEvent(descString, NULL, 0, EE_EXCEPTION, NULL, 0);
	return EE_EXC_STOP;
} //End of PrefetchAbortHandler()


/*******************************************************************************
* Function: exceptionHandlerPhase2Init
********************************************************************************
* Description: Exception handler phase 2 initiation
   .
*
* Parameters:
*
* Return value:
*
* Notes:
*******************************************************************************/
void exceptionHandlerPhase2Init( void )
{
	exceptionHandlersBind(); //secondary handlers
	PhysicalExcepBuffer_p = (EE_Entry_t*)CACHED_TO_NON_CACHED(&__CurrentExceptionLog.log);  // use non-cached space to ensure data written survives reset
#if defined(EE_DEFERRED_ACTIONS)
	eeAbortStack = ReadMode_R13(CPSR_MODE_ABT);
	eeUndefStack = ReadMode_R13(CPSR_MODE_UND);
//	EEHandlerFatalErrorActionBind(eePrintDeferredError, EE_ActionDefer|EE_ActionDeferredOnly);
#endif

#if defined (DATA_COLLECTOR_IMPL)
	//bind ISPT logger
	EEHandlerFatalErrorActionBind(ISPTSendDataEE, EE_ActionDefer | EE_ActionDeferredOnly);
#endif

#if !defined (SERIAL_NOT_ON_BOARD)
	EEHandlerExternalFinalActionBind(finalActionSerialLoop); //default external action
#endif

#if defined (EEH_DUMP_DTCM)
	EELOG_RegisterForLogstream(EELOG_streamOutDTCM);
#endif
#if defined(FULL_SYSTEM)
	EELOG_RegisterForLogstream(NULL /*RAMLOG*/);
#endif

} //End of exceptionHandlerPhase2Init()


/*******************************************************************************
* Function: eehHandleFatalEvent
********************************************************************************
* Description: Common handling for all fatal events
*
* Parameters:
*
* Return value:
*
* Notes:
*******************************************************************************/
void eehHandleFatalEvent(const char* desc, const char* file, int line, EE_entry_t type, const UINT8* data, UINT32 dataLen)
{
	RTC_CalendarTime calendarTime = { 0 };
	FinalAct_t fAction;
	UINT32 eeMode = EE_MODE_DEFAULT;
	BOOL watchdogExpired = WDT_EXPIRED_withoutAssert;
	int len;

	disableInterrupts(); //START OF NON-REENTRANT PART

#if defined (ENABLE_DIAG_LOGGER)
	mmuDataCacheInvalidateSafe(&pNextFreeSpace, 3 * sizeof(pNextFreeSpace));
#endif // defined (ENABLE_DIAG_LOGGER)
	// Examine the state and determine the behaviour
	if (eeEntryCount++)
	{
		// Not the first error
#if defined(EE_DEFERRED_ACTIONS)
		if (eeConfiguration.deferredCfg.enable)
		{
			if (!eeIsRecoverable() || (eeEntryCount >= eeConfiguration.deferredCfg.limitHits))
			{
				// Hit limit reached: trigger the final action
				eeDeferredFinalActionRun(watchdogExpired == FALSE);
			}
			else
			{
				// Sustain the state: silently terminate the faulty thread on subsequent entries
#if defined (USE_SERIAL_EE_AS_FINAL_ACTION)
				if (eeConfiguration.finalAction == EE_NON_WDT_SERIAL)
					NonWdtFinalAction(FALSE);
#endif
				eeParkThread(type, NULL);
			}
		}
#endif          //EE_DEFERRED_ACTIONS
       // Something went wrong while the previous error was handled. Apply the final action immediately.
		eeMode = 0;
	}
	else //if(eeEntryCount)
	{
		// The first error

		PMdisablePMfeautures();

#if defined(MIPS_TEST_RAM)
		mipsRamStop();
#endif

		EeHandlerWarningKeepInfoInEEbuf(0);

		EE_fstdioConfigOnAssert();

		if (!watchdogExpired)
			watchdogExpired = (watchdogRunStateGet() == WATCHDOG_EXPIRED);

#if defined(EE_DEFERRED_ACTIONS)
		if (eeIsRecoverable() && ((eeConfiguration.deferredCfg.enable & EE_DEFER_ASSERTS) && (type == EE_ASSERT) ||
					  (eeConfiguration.deferredCfg.enable & EE_DEFER_EXCEPTIONS) && (type == EE_EXCEPTION)))
		{
			if (!watchdogExpired)
			{
				eeMode |= EE_MODE_DEFERRED;
				if (eeConfiguration.deferredCfg.deferIntermediateActions)
				{
					//eeMode&=~EE_MODE_INTERMEDIATE;
					eeMode |= EE_MODE_DEFERRED_INTERMEDIATE;
				}
			}
		}
#endif          //EE_DEFERRED_ACTIONS
	} //endof  if(eeEntryCount)


	// Determine the proper final action
	fAction = eeConfiguration.finalAction;
	if ((fAction == EE_EXTERNAL) && ((eeFatalActionExt == 0) || ((eeMode & EE_MODE_ALLOW_EXT) == 0))) fAction = EE_RESET;
	if (fAction == EE_RESET_START_BASIC) fAction = EE_RESET;

	// Align the inputs
#if !defined (_TAVOR_HARBELL_)
	if (eeMode & EE_MODE_LEDS)
		ledSetOnComm(COMM_LED_RED, LED_ON_VAL);
#endif


	if (!file) file = "Unknown";
	if (!desc) desc = "Assert";

	if (eeMode & EE_MODE_LOG)
	{
		//save time
		RTCCurrentDateAndTimeGet(&calendarTime);

		// LOG this event
		len = strlen(desc);
		if (watchdogExpired && (len < (ERROR_HANDLER_MAX_DESC_SIZE - 12)) )
		{
			char descWDT[ERROR_HANDLER_MAX_DESC_SIZE] = "WDT + ";
			memcpy( descWDT + strlen(descWDT), desc, len + 1 );
			exceptionIRAMsave(descWDT, file, line, type, &__Saved_Registers, &calendarTime, data, dataLen);
		}
		else
			exceptionIRAMsave(desc, file, line, type, &__Saved_Registers, &calendarTime, data, dataLen);
	}

#if defined (USE_SERIAL_EE_AS_FINAL_ACTION)
	if ((type == EE_EXCEPTION) && (eeConfiguration.finalAction == EE_NON_WDT_SERIAL))
		NonWdtFinalAction(TRUE);
	else
#endif
	if (eeMode & EE_MODE_INTERMEDIATE)
	{
		// Intermediate actions
		BOOL aboutToReset = (fAction == EE_RESET) && ((eeMode & EE_MODE_DEFERRED) == 0);
		int i;
		for (i = 0; i < EE_MAX_INTERMEDIATE_ACTIONS; i++)
		{
			if (eeIntermediateActions[i].pFunc != 0)
			{
				UINT32 actMode = eeIntermediateActions[i].mode;
				if (!aboutToReset || ((actMode & EE_ActionSkipBeforeReset) == 0))
#if defined(EE_DEFERRED_ACTIONS)
					if ((((actMode & EE_ActionDefer) == 0) || ((eeMode & EE_MODE_DEFERRED_INTERMEDIATE) == 0)) && ((actMode & EE_ActionDeferredOnly) == 0))
#endif //EE_DEFERRED_ACTIONS
					eeIntermediateActions[i].pFunc(desc, file, line, type, &__Saved_Registers, &calendarTime);
			}
		}
	}

#if defined(MIPS_TEST_RAM)
	if (eeEntryCount == 1)
		mipsRamSaveEventToRam();
#endif

	//
	// Final action
	//
#if defined(EE_DEFERRED_ACTIONS)
	if ((eeMode & EE_MODE_DEFERRED) == 0)
	{                 //If WDT, don't notify master, it is already done
		eeDeferredFinalActionRun(watchdogExpired == FALSE);
	}
	else
	{
		eeDeferredFinalActionLoad(fAction);
		eeParkThread(type, (eeMode & EE_MODE_DEFERRED_INTERMEDIATE) ? eeDeferredIntermediateActionRun : eePrintDeferredError);
	}
#else
	{
#if defined(EE_FLUSH_DCACHE_BEFORE_FINAL_ACTION)
		if (fAction != EE_RESET) mmuFullDataCacheClean();  //synchronize the external memory to allow "post mortem" memory content upload
#endif
		errHandlerPreFinalAction(fAction);

		if ((fAction == EE_EXTERNAL) && eeFatalActionExt)
			eeFatalActionExt(desc, file, line, type, &__Saved_Registers, &calendarTime);

		errHandlerFinalAction(fAction);
	}
#endif  //EE_DEFERRED_ACTIONS

} //eehHandleFatalEvent


#define MAX_LINE_NUM_CHARS    10
/*******************************************************************************
* Function: exceptionIRAMsave
********************************************************************************
* Description: save the register into the buffer located in IRAM.
*
* Parameters:
*
* Return value:
*
* Notes:
*******************************************************************************/
static void exceptionIRAMsave( const char* DescString, const char* file, int line, EE_entry_t type,
			       const EE_RegInfo_Data_t* regContext, const RTC_CalendarTime* calendarTime, const UINT8* data, UINT32 dataLen)
{
	int i, maxStrLength, fileLineLength;
	char*  str;
	char lineNum[MAX_LINE_NUM_CHARS] = "";

  #define callerAddrFormat "[AT %.8X]"
	EE_Entry_t* pBuffer = &PhysicalExcepBuffer_p[_exceptionSRAMBufIndex];
	if (!PhysicalExcepBuffer_p) return;     //not initialized

	if (eeEntryCount > 1) return;           //Do not save secondary and further asserts

	pBuffer->type = type;

	pBuffer->dateAndTime = *calendarTime;
	// Format the event description string directly in the RAM buffer (no intermediate storage)
	str = pBuffer->desc;
	if (line) sprintf(lineNum, ",L:%d", line);
	fileLineLength = strlen(file) + strlen(lineNum);
	maxStrLength = sizeof(pBuffer->desc) - 1;
	if (type == EE_EXCEPTION) maxStrLength -= sizeof(callerAddrFormat) - 4 + 8;  //-4 format chars + 8 hex digits
	strncpy( pBuffer->desc, DescString, maxStrLength);
	if (type == EE_EXCEPTION) sprintf(pBuffer->desc + strlen(pBuffer->desc), callerAddrFormat, __Saved_Registers.PC);

	if (fileLineLength)
	{
		fileLineLength++;               // also ","
		i = maxStrLength - strlen(str); //space left
		if (i < fileLineLength)         //do file+line fit?
		{
			int fileLen = strlen(file);
			if ((i = fileLineLength - i) > fileLen) i = fileLen;
		}
		else i = 0;  //enough space for full file+line
		strcat(str, ",");
		strcat(str, file + i);
		strcat(str, lineNum);
	}
	//save registers
	//Registers_struct is a global array contains all registers saved in exception handler on init.s
	pBuffer->RegInfo  =   __Saved_Registers;

	// Fill context buffer depending on configuration
	if (dataLen)
	{
		// Store user data in the context buffer
		if (dataLen > sizeof(pBuffer->contextBuffer)) dataLen = sizeof(pBuffer->contextBuffer);
		pBuffer->contextBufferType = EE_CDT_UserDefined;
		memcpy(pBuffer->contextBuffer, (void*)data, dataLen);
		if (dataLen < sizeof(pBuffer->contextBuffer)) memset(((UINT8*)pBuffer->contextBuffer) + dataLen, 0, sizeof(pBuffer->contextBuffer) - dataLen);
	}
	else
#ifdef EE_USE_TRACE_BUFFER
	if (((type == EE_ASSERT)    && (eeConfiguration.assertContextBufType == EE_CT_ExecTrace)) ||
	    ((type == EE_EXCEPTION) && (eeConfiguration.exceptionContextBufType == EE_CT_ExecTrace)))
	{
		pBuffer->contextBufferType = EE_CDT_ExecTrace;
	}
	else
#endif
	if (((type == EE_ASSERT)    && (eeConfiguration.assertContextBufType == EE_CT_StackDump)) ||
	    ((type == EE_EXCEPTION) && (eeConfiguration.exceptionContextBufType == EE_CT_StackDump)))
	{
		UINT32 sp = (UINT32)__Saved_Registers.SP;
		//UINT32 sz=sp+sizeof(pBuffer->contextBuffer);
		pBuffer->contextBufferType = EE_CDT_StackDump;
		memcpy(pBuffer->contextBuffer, (void*)sp, sizeof(pBuffer->contextBuffer));
		pBuffer->CHKPT0 = pBuffer->CHKPT1 = 0;
	}
	else
	{
		pBuffer->contextBufferType = EE_CDT_None;
		memset(pBuffer->contextBuffer, 0, sizeof(pBuffer->contextBuffer));
		pBuffer->CHKPT0 = pBuffer->CHKPT1 = 0;
	}

#ifdef EE_TASK_INFO
	{
		UINT32 mode = regContext->cpsr & CPSR_MODE_BITS;
		pBuffer->taskStackStart = pBuffer->taskStackEnd = 0;
		if ((mode != CPSR_MODE_IRQ) && (mode != CPSR_MODE_FIQ) && !OS_Current_Interrupt_Count())
		{
			extern BOOL OS_Current_Task_Info(char** name, UINT32* stackStart, UINT32* stackEnd);
			char* name;
			OS_Current_Task_Info(&name, &pBuffer->taskStackStart, &pBuffer->taskStackEnd);
			if (name)
			{
				strncpy(pBuffer->taskName, name, sizeof(pBuffer->taskName));
				pBuffer->taskName[sizeof(pBuffer->taskName) - 1] = 0;
			}
			else
				strcpy(pBuffer->taskName, "Unknown");
		}
		else
			strcpy(pBuffer->taskName, "INTRRPT");
	}
#endif

#if !defined (INTEL_2CHIP_PLAT_BVD) && !defined(_TAVOR_BOERNE_) && !defined(_TAVOR_HARBELL_)
// only for HERMON
	if (type == EE_EXCEPTION)
	{
		// Save XSC Gasket registers (relevant for exception debug, mostly for external aborts)
		UINT32* pGSKT = (UINT32*)0x50000000;
		pBuffer->xscaleGasketRegs._PESR = pGSKT[4];     // offset 0x10
		pBuffer->xscaleGasketRegs._XESR = pGSKT[8];     // offset 0x20
		pBuffer->xscaleGasketRegs._PEAR = pGSKT[12];    // offset 0x30
		pBuffer->xscaleGasketRegs._FEAR = pGSKT[13];    // offset 0x34
		pBuffer->xscaleGasketRegs._SEAR = pGSKT[14];    // offset 0x38
		pBuffer->xscaleGasketRegs._GEAR = pGSKT[15];    // offset 0x3c
	}
	else
#endif // only for HERMON
	{
		memset((void*)&pBuffer->xscaleGasketRegs, 0, sizeof(pBuffer->xscaleGasketRegs));
	}
	//pBuffer->warningCntr;

	//Cyclic
	if (++_exceptionSRAMBufIndex >= EXCEPTION_HANDLER_BUF_SIZE)
	{
		_exceptionSRAMBufIndex = 0;
	}
	exceptionIRAMMarkValid(1);
} /* End of exceptionIRAMsave */


/*******************************************************************************
* Function: exceptionIRAMMarkValid
********************************************************************************
* Description: Marks the buffer in RAM as valid.
*
* Parameters:
*
* Return value:
*
* Notes:
*******************************************************************************/
static void exceptionIRAMMarkValid(int valid)
{
	EE_LogBuffer* pBuf = (EE_LogBuffer*)CACHED_TO_NON_CACHED(&__CurrentExceptionLog);  // use non-cached space to ensure data written survives reset

	if (valid)
	{
		UINT16* pw = (UINT16*)&pBuf->log;
		UINT16* pwEnd = (UINT16*)&pBuf->log[EXCEPTION_HANDLER_BUF_SIZE];
		UINT16 chksum;
		for (chksum = 0; pw < pwEnd; pw++) chksum += *pw;
		pBuf->logChksum = ~chksum;
		pBuf->logSignature = EE_RAM_BUFFER_SIGNATURE;
	}
	else
	{
		memset(pBuf, 0, sizeof(EE_LogBuffer));
	}
}

/*******************************************************************************
* Function: exceptionIRAMCheckValid
********************************************************************************
* Description: Verify the buffer in RAM is valid.
*
* Parameters:
*
* Return value:
*
* Notes:
*******************************************************************************/
static int exceptionIRAMCheckValid(void)
{
	EE_LogBuffer* pBuf = (EE_LogBuffer*)CACHED_TO_NON_CACHED(&__CurrentExceptionLog);  // use non-cached space to ensure data written survives reset

	if (pBuf->logSignature == EE_RAM_BUFFER_SIGNATURE)
	{
		UINT16* pw = (UINT16*)&pBuf->log;
		UINT16* pwEnd = (UINT16*)&pBuf->log[EXCEPTION_HANDLER_BUF_SIZE];
		UINT16 chksum;
		for (chksum = 0; pw < pwEnd; pw++) chksum += *pw;
		if (pBuf->logChksum == (UINT16) ~chksum) return 1;
	}
	return 0;
}


/*******************************************************************************
* Function: EEhandlerSaveToFile
********************************************************************************
* Description: save the buffer to FDI file .
*
* Parameters:
*
* Return value:
*
* Notes: - DIAGM_EEHANDLER,Tests,C3_EEhandlerSaveToFile
*******************************************************************************/
void  excepHandlerSaveExceptionToFile( void)
{
	//save the reset cause.
	//  POR  = 1,when the last system reset was power on reset
	//  EMR  = 2,when the last system reset was external master reset (POR == 0)
	//  WDTR = 3,when the last system reset was watchdog timer reset
	EE_PMU_t pmu_val = (EE_PMU_t)PMULastReset();


	if (eeConfiguration.powerUpLogOn)
	{
		// power on
		if (pmu_val & PMU_POR /*&& PhysicalExcepBuffer_p[_exceptionSRAMBufIndex].type == EE_SYS_RESET*/)
		{
			RTC_ReturnCode rtcReturnCode = RTC_RC_OK;
			RTC_CalendarTime calendarTime = { 0 };
			rtcReturnCode = RTCCurrentDateAndTimeGet(&calendarTime);
			ASSERT_LOW_LEVEL((rtcReturnCode == RTC_RC_OK), 0);

			//load the Iram buffer with the description and with the type
			exceptionIRAMsave((const char*)"EE_SYS_RESET", NULL, 0, EE_SYS_RESET, &__Saved_Registers, &calendarTime, NULL, 0);

			//all register info set = 0
			memset(&PhysicalExcepBuffer_p[_exceptionSRAMBufIndex].RegInfo, 0, sizeof(EE_RegInfo_Data_t));

			//save reset cause;
			PhysicalExcepBuffer_p[_exceptionSRAMBufIndex].RegInfo.PMU_reg = pmu_val;

			//save it to the log file and set the type to system_reset- if ASSERT/EXCEPTION then it will be change.
			errHandlerSaveEntryToFile(&PhysicalExcepBuffer_p[_exceptionSRAMBufIndex]);

			//for next time , not for the current
			//PhysicalExcepBuffer_p[_exceptionSRAMBufIndex].type = EE_SYS_RESET;
			return; //otherwise the following code would save it again
		}

	}

#if defined(EE_SAVE_RAMBUFFER_ONLY_ON_WDTRESET)
	if ((pmu_val & (PMU_WDTR | PMU_POR)) == PMU_WDTR) // Watchdog reset indication (valid only when no POR indication)
#else
	if (exceptionIRAMCheckValid())
#endif
	{
		// all other cases are relvent to the Assert or Exception the reset will be from a WDT.
		//save reset cause;
		PhysicalExcepBuffer_p[_exceptionSRAMBufIndex].RegInfo.PMU_reg = pmu_val;

		if (eeConfiguration.printRecentLogOnStartup)
		{
			OSATaskSleep(eeConfiguration.delayOnStartup); //make sure USB finished enumeration (2-3sec)
			DIAG_FILTER(EE_HANDLER, EE_LOG, RECENT_LOG, DIAG_INFORMATION)
			diagPrintf("EE recent error log: %s", PhysicalExcepBuffer_p[_exceptionSRAMBufIndex].desc);
			EeHandlerPrintEntry(&PhysicalExcepBuffer_p[_exceptionSRAMBufIndex]);
#if defined (ENABLE_DIAG_LOGGER)
			/* Save the Diag Log */
			DiagLoggerSaveLogToFile();
#endif // defined (ENABLE_DIAG_LOGGER)
		}

		//save it to the log file and set the type to system_reset- if ASSERT/EXCEPTION then it will be change.
		errHandlerSaveEntryToFile(&PhysicalExcepBuffer_p[_exceptionSRAMBufIndex]);

		exceptionIRAMMarkValid(0); //mark the buffer invalid

		if (eeConfiguration.finalAction == EE_RESET_START_BASIC)
		{
			// Stop system initialization right now
			while (1)
			{
				OSATaskSleep(20000);
			}
		}
	}
	exceptionIRAMMarkValid(0); //mark the buffer invalid UNCONDITIONALLY
}
/*******************************************************************************
* Function: eeHandlerSaveDescAssert
********************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
*******************************************************************************/
void eeHandlerSaveDescAssert (const char* desc)
{
	eeHandlerSaveDescAssertData(desc, NULL, 0);
}

/*******************************************************************************
* Function: eeHandlerSaveDescAssertData
********************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
*******************************************************************************/
void eeHandlerSaveDescAssertData(const char* desc, const UINT8* data, UINT32 dataLen)
{
	if (!eeConfiguration.AssertHandlerOn)
		return;
	ReadRegisterContext((UINT32*)&__Saved_Registers);
	eehHandleFatalEvent(desc, NULL, 0, EE_ASSERT, data, dataLen);
}

/*******************************************************************************
* Function: eeWarning
*
*   "Concatenation" of eeHandlerSaveDescFileLineAssert() and reduced eehHandleFatalEvent()
*******************************************************************************/
UINT32 eeWarningCntr;
typedef struct eeWarningInfo_TAG
{
	UINT32 cntr;
	UINT32 idx;
	char*  file;
	int line;
}eeWarningInfo_type;

eeWarningInfo_type eeWarningInfo;

void eeWarning (const char *file, int line, int warningMask)
{
	//Always Keep info even for (eeConfiguration.WarningHandlerOn == EE_WARN_OFF)
	if ( eeWarningInfo.idx == eeWarningInfo.cntr)
	{
		if (file == NULL) eeWarningInfo.file = "Unknown";
		else eeWarningInfo.file = (char*)file;
		eeWarningInfo.line = line;
	}
	eeWarningInfo.cntr++;

#if defined (EE_WARN_ASSERTCONTINUE__SUPPORTED)
	/* diagPrintf() is permitted in task context only but prohibited in LISR and HISR.
	 * So using of this option is dangerous and must be used for "deep debugging" only
	 */
	if (eeConfiguration.WarningHandlerOn == EE_WARN_ASSERTCONTINUE)
	{
		//eehHandleFatalEvent(desc,file,line,EE_ASSERT,NULL,0);
		RTC_CalendarTime calendarTime = { 0 };
		UINT32 eeMode = EE_MODE_DEFAULT;
		UINT32 cpsrReg;
		char *desc;

		cpsrReg = disableInterrupts(); //START OF NON-REENTRANT PART

		ReadRegisterContext((UINT32*)&__Saved_Registers);

    #if defined(MIPS_TEST_RAM)
		mipsRamStop();
    #endif

		mmuFullDataCacheClean(); //synchronize the external memory to allow "post mortem" memory content upload

		//EE_fstdioConfigOnAssert();

		desc = "WARNING";
		if (!file)
			file = "Unknown";

		if (eeMode & EE_MODE_LOG)
		{       // LOG this event
			RTCCurrentDateAndTimeGet(&calendarTime);
			exceptionIRAMsave(desc, file, line, EE_WARNING, &__Saved_Registers, &calendarTime, NULL, 0);
		}
		restoreInterrupts(cpsrReg);

		//eePrintDeferredError();
		DIAG_FILTER(EE_HANDLER, EE_LOG, WARN_ASSERTCONTINUE, DIAG_INFORMATION)
		diagPrintf("EE WARNING no_%d: %s", eeWarningInfo.cntr, PhysicalExcepBuffer_p[_exceptionSRAMBufIndex].desc);
		EeHandlerPrintEntry(&PhysicalExcepBuffer_p[_exceptionSRAMBufIndex]);

		// Intermediate actions
		// There are 2 kinds of problem with intermediate:
		//  - many DisableInterrupts without restoreInterrupts
		//  - save to file on the current thread
		// So currently do NOT use it...
		//for(i=0;i<EE_MAX_INTERMEDIATE_ACTIONS;i++)
		//{
		//    if(eeIntermediateActions[i].pFunc!=0)
		//       eeIntermediateActions[i].pFunc(desc,file,line, EE_WARNING, &__Saved_Registers,&calendarTime);
		//}
	}
	else
#endif //EE_WARN_ASSERTCONTINUE__SUPPORTED
	if (eeConfiguration.WarningHandlerOn == EE_WARN_ASSERT)
	{
		char *desc = "WARNING";
		eeHandlerSaveDescFileLineAssert(desc, file, line);
	}
	//else do nothing
} //eeWarning

static void EeHandlerWarningKeepInfoInEEbuf(int ExtendMode)
{
	PhysicalExcepBuffer_p->warningCntr = eeWarningInfo.cntr;
}


/*******************************************************************************
* Function: eeHandlerSaveDescFileLineAssert
*******************************************************************************/
void eeHandlerSaveDescFileLineAssert (const char* desc, const char *file, UINT16 line)
{
	if (!eeConfiguration.AssertHandlerOn)
	{
#if !defined(SERIAL_NOT_ON_BOARD) && defined (_TAVOR_HARBELL_)
		/* EE reads configuration from remote NVM over client/server.
		 * So NVM is initialized before the eeHandlerPhase2Init().
		 * BUT if NVM fails, we need EE-handler to be FORCEd even without Config-file!
		 * The direct UART loop is working better in this case */
		if (initState == BSP_INIT_PHASE1_PASSED)
			finalActionSerialLoop(/*desc*/ file, file, line, (EE_entry_t)0, 0, 0);
#endif
		return;
	}

	ReadRegisterContext((UINT32*)&__Saved_Registers);
	eehHandleFatalEvent(desc, file, line, EE_ASSERT, NULL, 0);
} //end of eeHandlerSaveDescFileLineAssert

void eeHandlerSaveDescFileLineAssertRegs(const char* desc, const char *file, UINT16 line, const EE_RegInfo_Data_t* regs)
{
	if (!eeConfiguration.AssertHandlerOn)
		return;
	memcpy(&__Saved_Registers, regs, sizeof(__Saved_Registers));
	eehHandleFatalEvent(desc, file, line, EE_ASSERT, NULL, 0);
}

#if defined (EE_USE_TRACE_BUFFER)
/*******************************************************************************
* Function: ExceptionInChipTraceBufferCopy
********************************************************************************
* Description:  read & copy the trace buffer from the chip buffer to the EE
*              ram buffer.
*
* Parameters:
*

*
* Return value:
* UINT32 ReadTBREG(void);
   UINT32 Read
   CHKPT1(void);
   UINT32 Read
   CHKPT0(void);
*
*******************************************************************************/
void  ExceptionInChipTraceBufferCopy  (void)
{
	UINT16 i;

	if (eeEntryCount) return;  //skip on subsequent entries

	//disable the trace - inchip trace.- InChipTrace.
	DisableTraceBuffer();

	for (i = 0; i < 256; i++)
	{
		PhysicalExcepBuffer_p->contextBuffer[i] = ReadTBREG();
	}
	PhysicalExcepBuffer_p->CHKPT0 = ReadCHKPT0();
	PhysicalExcepBuffer_p->CHKPT1 = ReadCHKPT1();


	//enable inChiptrace buffer.
	EnableTraceBuffer();
}
#endif //EE_USE_TRACE_BUFFER


//
// Register context adjustments
//

//
// Adjust PC,SP,CPSR,LR
// NOTE: PC is set from the LR value to point to the excepted instruction
// this can be modified by eeSkipExceptedInstruction() to NOT retry.
//
static void eeAdjustRegisters(EE_ExceptionType_t et, EE_RegInfo_Data_t* regContext)
{
	UINT32 rR14, rR13, spsr;

	regContext->PC = regContext->LR;
	spsr = Read_SPSR();
	regContext->cpsr = spsr; //excepted mode
	spsr &= CPSR_MODE_BITS;

	// Pick up the SP and LR values from the mode saved registers
	switch (spsr)
	{
	case CPSR_MODE_SVC:
	case CPSR_MODE_IRQ:
	case CPSR_MODE_FIQ:
		rR14 = ReadMode_R14(spsr);
		rR13 = ReadMode_R13(spsr);
		regContext->SP = rR13;
		regContext->LR = rR14;
		break;
	}

	// Adjust the fault PC value
	regContext->PC -= eeGetPcOffset(et, regContext);
}

//
// Returns the offset from excepted instruction to the PC saved on exception
//
int eeGetPcOffset(EE_ExceptionType_t et, const EE_RegInfo_Data_t* regContext)
{
	if (et == EEE_DataAbort)
	{
		if (regContext->cpsr & CPSR_T_BIT) return 6;
		else return 8;
	}
	else if (et == EEE_PrefetchAbort) return 4;
	return 0;
}

int eeGetPcStep(const EE_RegInfo_Data_t* regContext)
{
	if (regContext->cpsr & CPSR_T_BIT) return 2;
	return 4;
}

//
// Handler binding
//
BOOL EEHandlerFatalErrorActionBind(FatalEventAction_fn* pFunc, UINT32 mode)
{
	int i;

#if defined (EEH_ASSISTED_CLIENT)
	if (mode & EE_ActionDeferredOnly)
		return TRUE;                                    //do not exec. Everything required is done by Assist-Master
#endif
	if (!eeConfiguration.extHandlersOn) return TRUE;        //Block binding if disabled by config
	for (i = 0; i < EE_MAX_INTERMEDIATE_ACTIONS; i++)
	{
		if (eeIntermediateActions[i].pFunc == 0)
		{
			eeIntermediateActions[i].pFunc = pFunc;
			eeIntermediateActions[i].mode = mode;
			return TRUE;
		}
	}
	return FALSE;
}

FatalEventAction_fn* EEHandlerExternalFinalActionBind(FatalEventAction_fn* pFunc)
{
	FatalEventAction_fn* pOld = eeFatalActionExt;

	eeFatalActionExt = pFunc;
	return pOld;
}

/*******************************************************************************
* Function: errHandlerFinalAction
*******************************************************************************
* Description:  final action
*
* Parameters:
* Output Param:
*
* Return value: none
*
* Notes:        EXTERNAL option will trigger a reset: should be handled outside this function
*******************************************************************************/
void  errHandlerFinalAction(FinalAct_t action)
{
	bspSwResetReasonSet(SHD_SW_ERROR);
	switch (action)
	{
	case EE_EXTERNAL:
	case EE_RESET:
	case EE_RESET_START_BASIC:
	default:
		//disable the trace - inchip trace.- InChipTrace.
		DisableTraceBuffer();
		eeWatchdogActivate(); //set watchdog for 1 msec and stall
		errFinalActionStall();
		break;

	case EE_STALL:
		//disable the trace - inchip trace.- InChipTrace.
		DisableTraceBuffer();
		errFinalActionStall();
		break;
	}
}


/*******************************************************************************
* Function: errHandlerPreFinalAction
*******************************************************************************
* Description:  "final action" to be executed BEFORE external action and "final"
*               E.g. this is an extention for the errHandlerFinalAction()
*               Ordinary to be used for additional RAM-dumps if possible
* Parameters:
* Output Param:
*
* Return value: none
*
*******************************************************************************/
void  errHandlerPreFinalAction(FinalAct_t action)
{
#if defined(EEH_ASSISTED_CLIENT)
	UINT32 i;
	for (i = 0; i < EELOG_MAX_STREAMS; i++)
	{
		if (eeLogStream[i] != NULL)
			eeLogStream[i]();
	}
#endif
}


/******************************************************************************
* Function: eeWatchdogActivate
*******************************************************************************
* Description: Configures the match value and the mode of the watchdog timer
*
* Parameters: None
*
* Return value: WATCHDOG_RC_OK
*				WATCHDOG_RC_NULL_POINTER
*				WATCHDOG_RC_BAD_MATCH_VALUE
*				WATCHDOG_RC_NO_HANDLER_REGISTERED
*
* Notes:
******************************************************************************/
static WATCHDOG_ReturnCode eeWatchdogActivate(void)
{
	watchdogReset();
	return(WATCHDOG_RC_OK);
}

/******************************************************************************
* Function: eeNotify2AssistMaster
*******************************************************************************
* Description: #ifdef EE Assistance Master/Slave architecture used
*              and this is the Slave, notify Master that assert occured.
*              This is the "fast" non-deferred notification under disabled INT
*              so wait for "WDT notification is done" executted by polling.
*
*              Do NOTHING here if WDT is already running
*
* Parameters: None
* Return value: None
******************************************************************************/
static void eeNotify2AssistMaster(void)
{
#if defined (EEH_ASSISTED_CLIENT)
    #define MINICYCLES  0x800 /*Time: 0x1000.0000 = 135s (freq 208kHz)*/
	volatile UINT32 loopNoBusAccess;

	//Apply WDT (with protect against kicks)
	eeWatchdogActivateExt(/*timeout*/ 0, /*reset*/ FALSE);
	//We are under DISABLE-INT so make polling for WDT expiration.
	while (1)
	{
		for (loopNoBusAccess = 0; loopNoBusAccess < MINICYCLES; loopNoBusAccess--) ;

		if (watchdogRunStateGet() >= WATCHDOG_EXPIRED) //WATCHDOG_EXPIRED || WATCHDOG_EXPIRED_IN_DEBUG
			break;
	}
#endif
}

/*******************************************************************************
* Function: EeHandlerTestDataAbortExcep
*******************************************************************************
* Description:  for testing - data abort aligment
*
* Parameters:
* Output Param:
*
* Return value:
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - EE_HANDLER,EE,EE_Test_data_abort
void EeHandlerTestDataAbortExcep( void )
{
	int i = 2;

	*(volatile int*)(0x1807bbff) = i;
}

/*******************************************************************************
* Function: EeHandlerTestAssert
*******************************************************************************
* Description:  for testing - data abort aligment
*
* Parameters:
* Output Param:
*
* Return value:
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - EE_HANDLER,EE,EE_Test_Assert
void EeHandlerTestAssert( void )
{
	ASSERT(FALSE);
}

//ICAT EXPORTED FUNCTION - EE_HANDLER,EE,EE_Test_Warning
void EeHandlerTestWarning( void )
{
	WARNING(FALSE);
}

//ICAT EXPORTED FUNCTION - EE_HANDLER,EE,EE_WarningFirstINFO
void EeHandlerWarningINFO( void )
{
	if ( eeWarningInfo.idx >= eeWarningInfo.cntr)
	{
		DIAG_FILTER(EE_HANDLER, EE_LOG, WARN_NOINFO, DIAG_INFORMATION)
		diagPrintf("WARNING INFO: no new warnings");
	}
	else
	{
		DIAG_FILTER(EE_HANDLER, EE_LOG, WARN_INFO, DIAG_INFORMATION)
		diagPrintf("WARNING INFO for no_%d from total %d: %s, line_%ld",
			   (eeWarningInfo.idx + 1), eeWarningInfo.cntr, eeWarningInfo.file, eeWarningInfo.line);

		if (eeConfiguration.WarningHandlerOn == EE_WARN_ASSERTCONTINUE)
		{
			DIAG_FILTER(EE_HANDLER, EE_LOG, WARN_INFOEXT, DIAG_INFORMATION)
			diagPrintf("EE WARNING no_%d: %s", eeWarningInfo.cntr, PhysicalExcepBuffer_p[_exceptionSRAMBufIndex].desc);
			EeHandlerPrintEntry(&PhysicalExcepBuffer_p[_exceptionSRAMBufIndex]);
		}

		eeWarningInfo.idx = eeWarningInfo.cntr;
      #if defined(MIPS_TEST_RAM)
		mipsRamStart();
      #endif
	}
}

/*******************************************************************************
* Function: EeHandlerTestBranchZeroHandler
*******************************************************************************
* Description:  for testing - branches to zero
*
* Parameters:
* Output Param:
*
* Return value:
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - EE_HANDLER,EE,EE_Test_Branch_Zero
void EeHandlerTestBranchZeroHandler( void )
{
	void (*pFn)(void) = NULL;

	/* Branch to zero */
	pFn();
}



/*******************************************************************************
* Function: errFinalActionStall
*******************************************************************************
* Description:  Halt all activity, Kick WD, Flash LEDs
*
* Parameters:
* Output Param:
*
* Return value:
*
* Notes:
*******************************************************************************/
static void errFinalActionStall(void)
{
	disableInterrupts();
	while (1)
	{
		watchdogKick();
#if defined (USE_SERIAL_EE_AS_FINAL_ACTION)
		if (eeConfiguration.finalAction == EE_NON_WDT_SERIAL)
			NonWdtFinalAction(TRUE);
#endif
		Delay(10000); //10ms
	}
}

#if defined(EE_DEFERRED_ACTIONS)

/*******************************************************************************
* Function: eeIsRecoverable
*******************************************************************************
* Description:  Tell if the failed thread can be recovered (parked)
*
* Parameters:  type:
* Output Param:
*
* Return value:
*              TRUE if recoverable
* Notes:
*******************************************************************************/
static BOOL eeIsRecoverable(void)
{
	EE_RegInfo_Data_t* regContext = &__Saved_Registers;
	UINT32 mode = regContext->cpsr & CPSR_MODE_BITS;

	if ((mode == CPSR_MODE_SVC) && !OS_Current_Interrupt_Count()) return TRUE;
	return FALSE;
}

/*******************************************************************************
* Function: eeParkThread
*******************************************************************************
* Description:  Park the current thread and let the system run
*
* Parameters:  type: error event type (either exception or assert)
* Output Param:
*
* Return value:
*
* Notes:
*******************************************************************************/
static void eeParkThread(EE_entry_t type, void (*callbackFunc)(void))
{
	extern BOOL bspParkThread(EE_RegInfo_Data_t* regContext, void (*func)(void));
	EE_RegInfo_Data_t* regContext = &__Saved_Registers;
	UINT32 cMode = ReadCPSR() & CPSR_MODE_BITS;

	// Prepare the context changed required (depend on thread type, OS dependent)
	if (bspParkThread(regContext, callbackFunc))
	{
		if (cMode == CPSR_MODE_ABT) RestoreRegisterContextEx((UINT32*)regContext, eeAbortStack);
		else if (cMode == CPSR_MODE_UND) RestoreRegisterContextEx((UINT32*)regContext, eeUndefStack);
		else RestoreRegisterContext((UINT32*)regContext);
	}
	else
	{
		// Something is wrong: the thread was recoverable, but bspParkThread failed
		eeDeferredFinalActionRun(TRUE);
	}
}

/*******************************************************************************
* Function: eeDeferredIntermediateActionRun
*******************************************************************************
* Description:  Apply the final action immediately
*
* Parameters:  none
* Output Param:
*
* Return value:
*
* Notes:       This function is called from the watchdog interrupt handler
*******************************************************************************/

static void eeDeferredIntermediateActionRunInt(void)
{
	int i;

#if (EXCEPTION_HANDLER_BUF_SIZE != 1)
#error No support for multiple entries in the RAM buffer
#endif
	EE_Entry_t* pBuffer = &PhysicalExcepBuffer_p[0];

	for (i = 0; i < EE_MAX_INTERMEDIATE_ACTIONS; i++)
	{
		if (eeIntermediateActions[i].pFunc != 0)
		{
			UINT32 actMode = eeIntermediateActions[i].mode;
			if (actMode & (EE_ActionDefer | EE_ActionDeferredOnly))
				eeIntermediateActions[i].pFunc(pBuffer->desc, "", 0, pBuffer->type, &pBuffer->RegInfo, &pBuffer->dateAndTime);
		}
	}
}

static void eeDeferredIntermediateActionRun(void)
{
	// Print a trace to ICAT (always under thread callback)
	eePrintDeferredError();

#if !defined(EE_DEFERRED_INTERMEDIATE_IN_LOW_EVENT)
	eeDeferredIntermediateActionRunInt();
#else
	{
		lowTaskEventHandler_t ev = LowEventBind((LowEventFuncPtr) & eeDeferredIntermediateActionRunInt, 0);
		LowEventActivate(ev);
	}
#endif
}

/*******************************************************************************
* Function: eeDeferredFinalActionRun
*******************************************************************************
* Description:  Apply the final action immediately
*
* Parameters:  none
* Output Param:
*
* Return value:
*
* Notes:       This function is called from the watchdog interrupt handler
*******************************************************************************/
static void eeDeferredFinalActionRun(BOOL notify2masterRequired)
{
	FinalAct_t fAction = eeConfiguration.finalAction;

#if defined(EE_FLUSH_DCACHE_BEFORE_FINAL_ACTION)
	// We arrive here only when non-reset final action is taken
	mmuFullDataCacheClean(); //synchronize the external memory to allow "post mortem" memory content upload
#endif

	if ((fAction == EE_EXTERNAL) && eeFatalActionExt)
	{
		FatalEventAction_fn* pFun = eeFatalActionExt;

#if (EXCEPTION_HANDLER_BUF_SIZE != 1)
#error No support for multiple entries in the RAM buffer
#endif
		EE_Entry_t* pBuffer = &PhysicalExcepBuffer_p[0];
#if defined(EE_DEFERRED_FINAL_ACTION_FAKE_SAFE_TASK_THREAD)
		extern BOOL bspGuaranteeTaskThread(void);
		// Ensure the current thread is a task thread (fake it if needed).
		// However, do not cancel the external final action even if this fails
		bspGuaranteeTaskThread();
#endif
		// Ensure this external final action is not taken again in case it causes an error (though it's supposed to be safe)
		// NOTE: if such error happens, system will be reset (default final action is reset)
		eeFatalActionExt = 0;
		eeEntryCount = eeConfiguration.deferredCfg.limitHits;

		//Notify once only. If WDT expired, notification is not needed
		if (notify2masterRequired && !WDT_EXPIRED_withoutAssert)
			eeNotify2AssistMaster();

		errHandlerPreFinalAction(fAction);

		// ACTIVATE THE EXTERNAL FINAL ACTION
		pFun(pBuffer->desc, "", 0, pBuffer->type, &pBuffer->RegInfo, &pBuffer->dateAndTime);
	}
	errHandlerFinalAction(fAction);
}

/*******************************************************************************
* Function: eeDeferredFinalActionLoad
*******************************************************************************
* Description:  Configure the watchdog to trigger the final action sequence
*               later when the conditions are met
* Parameters:  none
* Output Param:
*
* Return value:
*
* Notes:       This function is called from the watchdog interrupt handler
*******************************************************************************/
static void eeDeferredFinalActionLoad(FinalAct_t fAction)
{
	BOOL reset = TRUE;
	UINT32 timeout = EE_WATCHDOG_TIME_IMMEDIATE;

	if ((fAction != EE_RESET) && (fAction != EE_RESET_START_BASIC)) reset = FALSE;
	if (eeConfiguration.deferredCfg.limitMs > EE_WATCHDOG_TIME_IMMEDIATE) timeout = eeConfiguration.deferredCfg.limitMs;
	eeWatchdogActivateExt(timeout, reset);
}

static void eeWatchdogInt(void)
{
	eeDeferredFinalActionRun(FALSE);
}

/******************************************************************************
* Function: eeWatchdogActivateExt
*******************************************************************************
* Description: Configures the match value and the mode of the watchdog timer
*
* Parameters:
*
* Return value: WATCHDOG_RC_OK
*				WATCHDOG_RC_NULL_POINTER
*				WATCHDOG_RC_BAD_MATCH_VALUE
*				WATCHDOG_RC_NO_HANDLER_REGISTERED
*
* Notes:
******************************************************************************/
static WATCHDOG_ReturnCode eeWatchdogActivateExt(UINT32 timeMs, BOOL reset)
{
#if defined (USE_SERIAL_EE_AS_FINAL_ACTION)
	if (eeConfiguration.finalAction == EE_NON_WDT_SERIAL)
		NonWdtFinalAction(FALSE);
#endif
	{
		WATCHDOG_Configuration wdgConfig;
		WATCHDOG_ReturnCode rc;

		watchdogProtect(FALSE);

		wdgConfig.matchValue = timeMs;

		if (reset)
		{
			/* RESET mode after "timeMs" */
			wdgConfig.mode = WATCHDOG_RESET_MODE;
		}
		else
		{
			/* Interrupt mode: */
			wdgConfig.mode = WATCHDOG_INTERRUPT_MODE;
			watchdogRegister(eeWatchdogInt);
		}
		rc = watchdogConfigure(&wdgConfig);

		if (rc == WATCHDOG_RC_OK)
		{
			rc = watchdogActivate();
			watchdogKick();
		}
		watchdogProtect(TRUE);
		return rc;
	}
} /* End of watchdogSet */


static void eePrintDeferredError(void)
{
	DIAG_FILTER(EE_HANDLER, EE_LOG, DEFERRED_LOG, DIAG_INFORMATION)
	diagPrintf("EE DEFERRED ERROR: %s", PhysicalExcepBuffer_p[_exceptionSRAMBufIndex].desc);
	EeHandlerPrintEntry(&PhysicalExcepBuffer_p[_exceptionSRAMBufIndex]);
	EeHandlerWarningINFO();
}


#endif //EE_DEFERRED_ACTIONS


#else /*!EE_HANDLER_ENABLE*/
UINT32      FatalErrorHandler   ( void )
{
	return(0);
}
UINT32      SWInterruptHandler  ( void )
{
	return(0);
}
UINT32      UndefInstHandler    ( void )
{
	return(0);
}
UINT32      ReservedIntHandler  ( void )
{
	return(0);
}
UINT32      DataAbortHandler    ( void )
{
	return(0);
}
UINT32      PrefetchAbortHandler( void )
{
	return(0);
}

void eeHandlerSaveDescAssert(const char* desc)
{
	UNUSEDPARAM(desc)
}
void eeHandlerSaveDescAssertData(const char* desc, const UINT8* data, UINT32 dataLen)
{
	UNUSEDPARAM(desc)
	UNUSEDPARAM(data)
	UNUSEDPARAM(dataLen)
}
void eeHandlerSaveDescFileLineAssert(const char* desc, const char *file, UINT16 line)
{
	UNUSEDPARAM(desc)
	UNUSEDPARAM(file)
	UNUSEDPARAM(line)
}
void eeHandlerSaveDescFileLineAssertRegs(const char* desc, const char *file, UINT16 line, const EE_RegInfo_Data_t* regs)
{
	UNUSEDPARAM(desc)
	UNUSEDPARAM(file)
	UNUSEDPARAM(line)
	UNUSEDPARAM(regs)
}
int eeGetPcOffset(EE_ExceptionType_t et, const EE_RegInfo_Data_t* regContext)
{
	UNUSEDPARAM(et)
	UNUSEDPARAM(regContext)
	return 0;
}
int eeGetPcStep(const EE_RegInfo_Data_t* regContext)
{
	UNUSEDPARAM(regContext)
	return 0;
}

BOOL EEHandlerFatalErrorActionBind(FatalEventAction_fn* pFunc, UINT32 mode)
{
	UNUSEDPARAM(pFunc)
	UNUSEDPARAM(mode)
	return TRUE;
}

FatalEventAction_fn* EEHandlerExternalFinalActionBind(FatalEventAction_fn* pFunc)
{
	UNUSEDPARAM(pFunc)
	return NULL;
}

UINT32        BranchZeroIntHandler  ( void )
{
	return EE_EXC_STOP;
}


#endif /*EE_HANDLER_ENABLE*/

#if defined(EE_HANDLER_ENABLE) && defined(EE_EXT_EXCEPTION_HANDLER)
static ExceptionHendlerExt eeExtExcHandler;
ExceptionHendlerExt eeExtExceptionHandlerBind(ExceptionHendlerExt newHandler)
{
	ExceptionHendlerExt old = eeExtExcHandler;

	eeExtExcHandler = newHandler;
	return old;
}

EEHandlerAction eeExtExceptionHandler(EE_ExceptionType_t type, EE_RegInfo_Data_t* pContext)
{
	if (eeExtExcHandler)
		return eeExtExcHandler(type, pContext);
	return eehActionFatal;
}


#else
ExceptionHendlerExt eeExtExceptionHandlerBind(ExceptionHendlerExt newHandler)
{
	return NULL;
}
#endif



#if defined (USE_SERIAL_EE_AS_FINAL_ACTION)
/***********************************************************************************
		    USE_SERIAL_EE_AS_FINAL_ACTION
   In cases of "2CHIP diag-over-MSL" exceptions like "data abort" cannot be
   successfully transmitted over MSL and therefore cannot be seen.
   If we have UART serial, working in polling non-interrupt mode,
   we could send exceptions into UART to guaranty them to be seen.

   The sequence is next.
   1. eePrintDeferredError wich put EE-message into transmission queue.
   2. Interrupts have been previously disabled by EE-handler.
   Enable interrupts.
   Take delay_loop to permit EE-message transmission.
   The delay_loop is BLOCKING, do it in the LOW priority!
   Disable interrupts as it was.
   3. Set HIGHEST priority to stall all tasks.
   4. Call for seral printing loop, which does never return.

 ************************************************************************************/
// Blocking loop for delay (call it in LOW priority). 0x20000000 = ~13 seconds
#define DELAY_LOOP(COUNTER)    { volatile UINT32 delay = COUNTER; while (delay) delay--; }

#define PRIO_LOW         250
#define PRIO_HIGHEST      1

#if !defined (OSA_WINCE) && !defined (OSA_LINUX)
static UINT32 entryNumber = 0;
void NonWdtFinalAction(BOOL force)  //eePrintDeferredError eeDeferredIntermediateActionRun
{
	OSATaskRef taskRef = NULL;
	UINT8 oldPriority;

	if ( (!force) && (entryNumber == 0) )
	{
		if (OSATaskGetCurrentRef(&taskRef) == OS_SUCCESS)
		{
			eePrintDeferredError(); //do it for task but not for interrupt
			if (OSATaskChangePriority(taskRef, PRIO_LOW, &oldPriority) == OS_SUCCESS)
			{
				enableInterrupts();
				DELAY_LOOP(0x00300000);
				OSATaskChangePriority(taskRef, PRIO_HIGHEST, &oldPriority);
				disableInterrupts();
			}
		}
	}

	entryNumber++;

	mmuFullDataCacheClean(); //synchronize the external memory to allow "post mortem" memory content upload

#if defined (DATA_COLLECTOR_IMPL)
	ISPTSendDataEE(0, 0, 0, 0, 0, 0);
#endif

	finalActionSerialLoop(PhysicalExcepBuffer_p[_exceptionSRAMBufIndex].desc,       //mandatory, event description string
			      PhysicalExcepBuffer_p[_exceptionSRAMBufIndex].taskName,   //optional, may appear as NULL
			      entryNumber, (EE_entry_t)0, 0, 0);
	/* Never return */
}
#endif //USE_SERIAL_EE_AS_FINAL_ACTION


#endif //OSA_WINCE

#if defined (DATA_COLLECTOR_IMPL)
void ISPTSendDataEE(const char*                cond,
		    const char*                file,
		    int line,
		    EE_entry_t type,
		    const EE_RegInfo_Data_t*   regContext,
		    RTC_CalendarTime*          calendarTime)
{
	unsigned char chPacketID;
	unsigned char* pData;
	int iDataLen;

	chPacketID = 0; //EE-data brief text description
	pData      = (unsigned char*)PhysicalExcepBuffer_p[_exceptionSRAMBufIndex].desc;
	iDataLen   = strlen(PhysicalExcepBuffer_p[_exceptionSRAMBufIndex].desc);
	ISPTSendData(chPacketID, pData, iDataLen);

	chPacketID = 1; //EE-data buffer
	pData      = (unsigned char*)&__CurrentExceptionLog;
	iDataLen   = sizeof(EE_LogBuffer);
	ISPTSendData(chPacketID, pData, iDataLen);
}
#endif //DATA_COLLECTOR_IMPL



#endif //#if !defined (OSA_WINCE) shacharh 2




//======================  SYSTEM EXCEPTION HANDLER  =====================
//   Relevant only for system running on APP+COMM processors
//                    and only for APP side
#if defined (EEH_ASSISTING_SERVER)

#define SYSEE_TASK_PRIORITY   100
#define SYSEE_STACK_SIZE      2048

typedef enum
{
	SYSEE_TASK_ACTION_COMMWDT =    1, // 1<<0
	SYSEE_EVENT_MASK          = 0x0F
}
SysEeActionEnum;

UINT32 EELOG_shareReadFWrite(void);

#if !defined (OSA_WINCE) && !defined (OSA_LINUX)

static char SysEeTaskStack[SYSEE_STACK_SIZE];
static OSTaskRef SysEeTaskRef;
static void*            SysEeEventRef;

static NU_HISR SysEeHisr;


//-------------------------------------------
static void SysEeSendComWdtEvent(void)
{
	OSAFlagSet( (OSFlagRef)SysEeEventRef, SYSEE_TASK_ACTION_COMMWDT, OSA_FLAG_OR );
}

//-------------------------------------------
static void SysEeTask(void* arg)
{
	UINT32 flags;
	FinalAct_t fAction;
	FILE_ID fdiID;

	while (1)
	{
		OSAFlagWait( (OSFlagRef)SysEeEventRef, SYSEE_EVENT_MASK, OSA_FLAG_OR_CLEAR,
			     &flags, OSA_SUSPEND );
		switch (flags)
		{
		case SYSEE_TASK_ACTION_COMMWDT:
			fAction = eeConfiguration.finalAction;

			// save-to-files once only may be required
			if ( (eeConfiguration.sysEeHandlerLimit == 0) /*unlimited or below limit*/
			     || (eeConfiguration.sysEeHandlerLimit >= SysEeHandlerEntryCntr) )
			{
				EE_fstdioConfigOnAssert();
				OSATaskSleep(2);        //Helps in case of Comm WDT_EXPIRED_withoutAssert
				SaveComPostmortem();    //sets also the EE_AssistingMaster_ASSERT_found


				// Do LOG-Stream for ASSERT-found only, otherwise nobody sends EELOGs
				if ( EE_AssistingMaster_ASSERT_found ) NumLogstreams = 1;
				else NumLogstreams = 0;

				//if(fAction == EE_RESET_COMM_SILENT)
				//    NumLogstreams = 0;

				if (NumLogstreams > 0)
				{
					UINT32 streamNo;
					for (streamNo = 0; streamNo < NumLogstreams; streamNo++)
						NumLogstreams = EELOG_shareReadFWrite();
				}

				// DDR-RW dump takes a lot of time.
				// Do it for ASSERT-found only, but don't if "fast" silent reset required
				if ( EE_AssistingMaster_ASSERT_found && (fAction != EE_RESET_COMM_SILENT) )
					EE_SaveComDDR_RW(eeConfiguration.dumpDdrSizeBytes);
			}

			if (fAction == EE_RESET)
			{
				ASSERT(SYSEE_TASK_ACTION_COMMWDT == FALSE);
			}

			if (fAction == EE_RESET_COMM_SILENT)
			{
				extern void commSideReset(BOOL useWdt4reset);
				commSideReset(/*WDT=*/ TRUE);
			}

			DIAG_FILTER(EE_HANDLER, EE_LOG, COMMWDT_END, DIAG_INFORMATION)
			diagPrintf("EE COMM WDT handling is finished");
			break;

		default:
			break;
		}
	} //while
}

//-------------------------------------------
static void SysEeTaskCreate(void)
{
//  SysEeTaskStack = extMemDynMalloc( SYSEE_STACK_SIZE );
	SysEeEventRef = NULL;
	ASSERT(OS_SUCCESS == OSAFlagCreate((OSFlagRef*)&SysEeEventRef));
	ASSERT( SysEeEventRef != NULL );

	memset(SysEeTaskStack, 0xA5, SYSEE_STACK_SIZE);
	{
		OS_STATUS status;
		status = OSATaskCreate(&SysEeTaskRef, SysEeTaskStack, SYSEE_STACK_SIZE, SYSEE_TASK_PRIORITY, "SysEeTask", SysEeTask, NULL);
		ASSERT(status == OS_SUCCESS);
	}
}

//-------------------------------------------
static void SysEeHisrFunc(void)
{
	SysEeHandlerEntryCntr++;
	SysEeSendComWdtEvent(); //always report to task
}

//-------------------------------------------
static void SysExceptionIntHandler(void)
{
	OS_Activate_HISR(&SysEeHisr);
}

//-------------------------------------------
void sysExceptionHandlerPhase2Init(void)
{
	SysEeHandlerEntryCntr = 0;

	OS_Create_HISR(&SysEeHisr, "SysEE", SysEeHisrFunc, HISR_PRIORITY_2);

	SysEeTaskCreate();

	wdtApps4comInit();

	if (eeConfiguration.finalAction == EE_RESET_COMM_SILENT)
		wdtApps4comResetCfg(WDT_COM_RESET_KEEP_PENDING);

	wdtApps4comBindUserHander(SysExceptionIntHandler);
}
#endif //OSA_WINCE

#endif //EEH_ASSISTING_SERVER

/***********************************************************************************
 *
 *     Logging EE messsage stream into NVRAM file
 *
 ************************************************************************************/

#define SYNC_INFO(COND, PTR)     syncStateInfo(COND, PTR, __LINE__)

#define RECOD_SIZE           (1024 * 56)        /* 12k        */
#define EELOG_NUM_RECORDS    1                  /*  2 buffers */
#define STREAM_OPEN_MAGIC    (UINT32)(0x50495045)        /*0x50495045 mapping to multi-character character constant 'PIPE' */

/****    Stream-Name is 7characters+\0 (e.g. "1234567").
 * Resulting File-Name will be "com_1234567.bin". The "com_" and ".bin" added by server
 * Default names are:
 *      "com_DTCM.bin"
 *      "com_GB_RAM.bin"
 *      "com_RAMLOG.bin"
 ***/
#define STREAM_NAME_SIZE       (7 + 1)
#define FILESTREAM_NAME_SIZE   (15 + 1)

typedef enum
{
	EELOG_NONE  = 0x00000000L,
	EELOG_OPEN,
	EELOG_RECORD_READY,
	EELOG_CLOSE
}
EElogCmdState;

//Always use Long to be complient to WIN compiler. Pay attention for ENUM size
typedef struct
{
	UINT32 AcStreamOpenedValid;       // <--STREAM_OPEN_MAGIC
	EElogCmdState cmdState;
	UINT32 putIdx;
	UINT32 bytesReady   [EELOG_NUM_RECORDS];
	char*           recordBufPtr [EELOG_NUM_RECORDS];
	int blockSize;
	int lastBlockClosed;
	char streamName[STREAM_NAME_SIZE];
	UINT32 ackNo;
	//---------------------- for backward Client/Server compatibility add new fields belo this line
	UINT32 NumLogstreams;
}
EElogStreamHeader;
//  \\\_____Stream recordBuf-s are placed after EElogStreamHeader but not a part of the EElogStreamHeader

#if !defined(OSA_WINCE) && !defined(OSA_LINUX)
//---------------------------------------------------------------
// Function:  EE_fstdioConfigOnAssert
//    Always required for Client/Server or even None of them
void EE_fstdioConfigOnAssert(void)
{
#if defined (EEH_ASSISTED_CLIENT)
	StdioBind_t par;
	fstdio_bindGetCurr(&par);
	par.fopen  = (fopen_t)EELOG_fopen;
	par.fclose = (fclose_t)EELOG_fclose;
	par.fwrite = (fwrite_t)EELOG_fwrite;
	fstdio_bind(&par);
#else
  #if defined(NO_FDI) || !defined(INTEL_FDI)
	//There is no FDI. Do nothing
  #else
	StdioBind_t par;
	fstdio_bindGetCurr(&par);
	par.fopen  = (fopen_t)FDI_fopen;
	par.fclose = (fclose_t)FDI_fclose;
	par.fwrite = (fwrite_t)FDI_fwrite;
	fstdio_bind(&par);
  #endif
	//Do nothing
#endif
	bspUartUseForErrorIO();
}

#endif //OSA_WINCE

#if defined (EEH_ASSISTED_CLIENT) || defined (EEH_ASSISTING_SERVER)
//---------------------------------------------------------------


EElogStreamHeader*  pEeLogStream;

static void EELOG_CloseStream(BOOL isFileServer);

//---------------------------------------------------------------
#define EELOG_TIMEOUT_MS              6000
#define EELOG_TIMEOUT_BIG_MS         10000
#define EELOG_TIMEOUT_HUGE_MS       100000

#define EELOG_DELAY_MULT_FACTOR     12
#define RATE_64uS_1mS               4  //1mS/64uS = 16 = 2e4
#define DELAY_64uS_SEC              EELOG_Delay64uS(1, 0)


UINT32 EELOG_Delay64uS(UINT32 numCycles, UINT32 dummyZero)
{
	UINT32 i = (numCycles << EELOG_DELAY_MULT_FACTOR);

	while (i != dummyZero)
		i--;
	return(i);
}

//---------------------------------------------------------------
BOOL EELOG_WaitForEQ(UINT32* pV1, UINT32* pV2, UINT32 abortDelta, UINT32 timeoutMS)
{
	UINT32 retryCycles64uS = timeoutMS << RATE_64uS_1mS;

	while (1)
	{
		if (*pV1 == *pV2)
			return(TRUE);   //Condition hit
		if (abortDelta < (*pV1 - *pV2))
			break;          //Waiting aborted
		if (retryCycles64uS-- == 0)
			break;          //Time-out
		DELAY_64uS_SEC;
	}
	return(FALSE);
}

BOOL EELOG_WaitForAck(EElogStreamHeader* p, UINT32 timeoutMS)
{
	return
		EELOG_WaitForEQ(&(p->putIdx), &(p->ackNo), (EELOG_NUM_RECORDS + 1), timeoutMS);
}

#if defined(EEH_ASSISTING_SERVER)
//---------------------------------------------------------------
BOOL EELOG_WaitForState(EElogStreamHeader* p, EElogCmdState state, UINT32 timeoutMS)
{
	UINT32 retryCycles5mS = timeoutMS / 5;

	while (1)
	{
		if (p->cmdState == state)
			return(TRUE);  //Condition hit

		if (retryCycles5mS-- == 0)
			break;  //Time-out

		OSATaskSleep(1);
	}
	return(FALSE);
}

//---------------------------------------------------------------
BOOL EELOG_WaitForState2(EElogStreamHeader* p, EElogCmdState state, EElogCmdState state2, UINT32 timeoutMS)
{
	UINT32 retryCycles5mS = timeoutMS / 5;

	while (1)
	{
		if ((p->cmdState == state) || (p->cmdState == state2))
			return(TRUE);  //Condition hit

		if (retryCycles5mS-- == 0)
			break;  //Time-out

		OSATaskSleep(1);
	}
	return(FALSE);
}

//---------------------------------------------------------
// Every client should register for the EELOG service
// There is a counter of registered clients,
// and it is used by server to know
// how many times EELOG-stream protocol should be applied
static UINT32 EELOG_GetNumLogstreams(EElogStreamHeader* p)
{
	UINT32 l_NumLogstreams;

    #if !defined(OSA_LINUX)
	ASSERT(p != NULL);
    #else
	if (p == NULL)
		return 0;
    #endif
	l_NumLogstreams = p->NumLogstreams;
    #if !defined(OSA_LINUX)
	ASSERT(l_NumLogstreams <= EELOG_MAX_STREAMS);
    #else
	if (l_NumLogstreams > EELOG_MAX_STREAMS)
		l_NumLogstreams = EELOG_MAX_STREAMS;
    #endif
	return(l_NumLogstreams);
}
#endif //EEH_ASSISTING_SERVER

//---------------------------------------------------------------
static EElogStreamHeader* EELOG_OpenStream(BOOL isFileServer, char* nameBuf)
{
	UINT32 begin;
	UINT32 end;
	UINT32 i;

	getAppComShareMemAddr(&begin, &end);

	begin = (UINT32)ADDR_CONVERT(begin);
	end   = (UINT32)ADDR_CONVERT(end);   //to avoid compile warning only

	pEeLogStream = (EElogStreamHeader*)MAP_PHYSICAL_TO_VIRTUAL_ADDRESS(begin);

	if (isFileServer)
	{
		UINT32 retryCycles5mS = (EELOG_TIMEOUT_BIG_MS / 5);
		i = retryCycles5mS;
		while (  (pEeLogStream->AcStreamOpenedValid != STREAM_OPEN_MAGIC)
			 || (pEeLogStream->streamName[0] == 0)
			 || (pEeLogStream->cmdState != EELOG_NONE) )
		{
			if (i-- == 0) return(NULL);   //timeout
			OSATaskSleep(1);
		}


		if (pEeLogStream->streamName[STREAM_NAME_SIZE - 1] != 0)
			return(NULL);

		sprintf(nameBuf, "com_%s.bin", pEeLogStream->streamName);
		pEeLogStream->ackNo = 0;
	}
	else
	{
		memset(pEeLogStream, 0, sizeof(EElogStreamHeader));
		pEeLogStream->NumLogstreams = NumLogstreams;
		pEeLogStream->ackNo     = 0xFFFFFFFF;
		pEeLogStream->cmdState  = EELOG_NONE;
		pEeLogStream->blockSize = RECOD_SIZE;

		//Stream buffers are placed after EElogStreamHeader, so use (pEeLogStream + 1)
		for (i = 0; i < EELOG_NUM_RECORDS; i++)
			pEeLogStream->recordBufPtr[i] = (char*)ADDR_CONVERT((UINT32)(pEeLogStream + 1) + (pEeLogStream->blockSize * i));
		{
			char  *streamName;
			UINT32 nameLen;
			// Copy name but check consistency and skip :\  / and '.'
			nameLen = strlen(nameBuf);
			if ((nameLen == 0) || (nameLen > 14))
			{
				EELOG_CloseStream(0); return(0);
			}
			streamName = strchr(nameBuf, ':');
			if (streamName != NULL) nameBuf = streamName + 1;
			streamName = strchr(nameBuf, '\\');
			if (streamName != NULL) nameBuf = streamName + 1;
			streamName = strchr(nameBuf, '/');
			if (streamName != NULL) nameBuf = streamName + 1;
			streamName = strchr(nameBuf, '.');
			if (streamName != NULL) *streamName = 0;
			nameLen = strlen(nameBuf);
			if (nameLen == 0)
			{
				EELOG_CloseStream(0); return(0);
			}

			for (i = 0; i < (STREAM_NAME_SIZE - 1); i++)
				pEeLogStream->streamName[i] = nameBuf[i];
			pEeLogStream->streamName[i] = 0;
		}
		pEeLogStream->AcStreamOpenedValid = STREAM_OPEN_MAGIC;
	}

	return(pEeLogStream);
}

static void EELOG_CloseStream(BOOL isFileServer)
{
	if (isFileServer)
		pEeLogStream->ackNo = pEeLogStream->putIdx + (0xFFFFFFFF / 2);
	else
	{
		pEeLogStream->putIdx++;
		EELOG_WaitForAck(pEeLogStream, EELOG_TIMEOUT_MS);
		//0x53544F50 mapping to multi-character character constant 'STOP'
		pEeLogStream->AcStreamOpenedValid = (UINT32)(0x53544F50);
	}
}

#if defined (EEH_ASSISTED_CLIENT)

BOOL EELOG_RegisterForLogstream(voidPFuncVoid* pF)
{
	UINT32 i;

	if (pF == NULL)
	{
		NumLogstreams++;
		return(FALSE);
	}

	for (i = 0; i < EELOG_MAX_STREAMS; i++)
	{
		if (eeLogStream[i] == NULL)
		{
			eeLogStream[i] = pF;
			NumLogstreams++;
			return(TRUE);
		}
	}
	return(FALSE);
}

//---------------------------------------------------------------
FILE* EELOG_fopen(const char * fileName, const char *modeStr)
{
	EElogStreamHeader* p;

	p = EELOG_OpenStream(0 /*isFileServer*/, (char*)fileName);

	if ( (p == NULL) || (p->cmdState != EELOG_NONE) )
		return(NULL);

	if ( EELOG_WaitForAck(p, EELOG_TIMEOUT_HUGE_MS) == FALSE)
	{
		EELOG_CloseStream(0 /*isFileServer*/);
		return(NULL);
	}
	else
		p->cmdState = EELOG_OPEN;

	return ((FILE*)p);
}

//---------------------------------------------------------------
size_t  EELOG_fwrite(const void *buf, size_t chunkSize, size_t numChunks, FILE* handl)
{
	EElogStreamHeader* p  = (EElogStreamHeader*)handl;

	int len, i;
	UINT32 putIdx;
	char*   src;
	char*   dst;

	if (p == NULL)
		return(0);

	ASSERT_LOW_LEVEL( (p->cmdState == EELOG_OPEN) || (p->cmdState == EELOG_RECORD_READY), 0);

	len = chunkSize * numChunks;
	src = (char*)buf;

	putIdx = p->putIdx & (EELOG_NUM_RECORDS - 1);
	dst = (UINT8*)p->recordBufPtr[putIdx];

	i = p->bytesReady[putIdx];

	while (len)
	{
		dst[i] = *src;
		i++;
		src++;
		len--;

		if (i == p->blockSize)
		{
			p->cmdState = EELOG_RECORD_READY;
			p->bytesReady[putIdx] = i;
			p->putIdx++;
			i = 0;
			putIdx = p->putIdx & (EELOG_NUM_RECORDS - 1);
			dst = (UINT8*)p->recordBufPtr[putIdx];
			if ( EELOG_WaitForAck(p, EELOG_TIMEOUT_MS) == FALSE)
			{
				EELOG_CloseStream(0 /*isFileServer*/);
				return(0);
			}
		}
	}

	//Prepare to exit with keep information for next step(s)
	if (i == 0)
	{
		p->lastBlockClosed = 1;
		p->bytesReady[putIdx] = 0; //reset next block
	}
	else
	{
		p->lastBlockClosed = 0;         //Block not full
		p->bytesReady[putIdx] = i;      // Keep current position.
	}
	return (numChunks * chunkSize);
}

//---------------------------------------------------------------
int EELOG_fclose(FILE* handl)
{
	EElogStreamHeader* p  = (EElogStreamHeader*)handl;

	if (p == NULL)
		return(0);

	if (!p->lastBlockClosed)
	{
		p->putIdx++;
		p->cmdState = EELOG_RECORD_READY;
		EELOG_WaitForAck(p, EELOG_TIMEOUT_MS);
	}
	p->cmdState = EELOG_CLOSE;

	EELOG_CloseStream(0 /*isFileServer*/);

	return(0);
}
#endif //EEH_ASSISTED_CLIENT


#if defined(EEH_ASSISTING_SERVER)

static void syncStateInfo(BOOL cond, EElogStreamHeader* p, int line)
{
	if (cond == TRUE)
	{
		switch (p->cmdState)
		{
		case EELOG_OPEN:
			DIAG_FILTER(EE_HANDLER, EE_LOG, logOpen, DIAG_INFORMATION)
			diagPrintf("Logging %s OPEN\n", p->streamName);
			break;
		case EELOG_RECORD_READY:
			DIAG_FILTER(EE_HANDLER, EE_LOG, logBlock, DIAG_INFORMATION)
			diagPrintf("Logging into %s block %d\n", p->streamName, p->putIdx);
			break;
		case EELOG_CLOSE:
			DIAG_FILTER(EE_HANDLER, EE_LOG, logClose, DIAG_INFORMATION)
			diagPrintf("Logging %s CLOSE\n", p->streamName);
			break;
		default:
			break;
		}
	}
	else
	{
		switch (p->cmdState)
		{
		case EELOG_NONE:
			DIAG_FILTER(EE_HANDLER, EE_LOG, logDbg0, DIAG_INFORMATION)
			diagPrintf("Line_%d: cmdState_NONE, %d\n", line, p->putIdx);
			break;
		case EELOG_OPEN:
			DIAG_FILTER(EE_HANDLER, EE_LOG, logDbg1, DIAG_INFORMATION)
			diagPrintf("Line_%d: cmdState_OPEN, %d\n", line, p->putIdx);
			break;
		case EELOG_RECORD_READY:
			DIAG_FILTER(EE_HANDLER, EE_LOG, logDbg2, DIAG_INFORMATION)
			diagPrintf("Line_%d: cmdState_RECO, %d\n", line, p->putIdx);
			break;
		case EELOG_CLOSE:
			DIAG_FILTER(EE_HANDLER, EE_LOG, logDbg3, DIAG_INFORMATION)
			diagPrintf("Line_%d: cmdState_CLOSE, %d\n", line, p->putIdx);
			break;
		default:
			DIAG_FILTER(EE_HANDLER, EE_LOG, logDbg4, DIAG_INFORMATION)
			diagPrintf("Line_%d: cmdState_UNKN, %d\n", line, p->putIdx);
			break;
		}
	}
}

// This is the SERVER.
// All addresses are in "client" format and should be converted before using !!!
//


UINT32 EELOG_shareReadFWrite(void)
{
	EElogStreamHeader* p;
	UINT32 getIdx, idx;
	int bytesOk;
	UINT8* src;
	FILE*  fHandl;
	char fileName[FILESTREAM_NAME_SIZE];

#if defined (OSA_WINCE)
	TCHAR path[MAX_PATH] = { 0 };
	char fullPath[MAX_PATH] = { 0 };
#endif

	UINT32 retryCycles5mS;
	UINT32 l_NumLogstreams = 0;

	p = EELOG_OpenStream(1 /*isFileServer*/, fileName);

	if (p == NULL)
		return(0);

	l_NumLogstreams = EELOG_GetNumLogstreams(p);

	if (l_NumLogstreams == 0)
	{
	#if defined (OSA_LINUX)
		ERRMSG("EElogStreamHeader 1: p->AcStreamOpenedValid = %d, p->cmdState = %d, p->putIdx = %d\n",
		       p->AcStreamOpenedValid, p->cmdState, p->putIdx);
		ERRMSG("EElogStreamHeader 1: p->blockSize = %d, p->lastBlockClosed = %d, p->streamName = %s\n",
		       p->blockSize, p->lastBlockClosed, p->streamName);
		ERRMSG("EElogStreamHeader 1: p->ackNo = %d, p->NumLogstreams = %d, p->bytesReady = %d\n",
		       p->ackNo, p->NumLogstreams, p->bytesReady[0]);
		#endif
		return(0);
	}

	// Wait for NONE->to->OPEN

	//SYNC_INFO(p->cmdState == EELOG_NONE, p);

	if ( EELOG_WaitForState2(p, EELOG_NONE, EELOG_OPEN, EELOG_TIMEOUT_MS) == FALSE)
	{
	#if defined (OSA_LINUX)
		ERRMSG("EElogStreamHeader 2: p->AcStreamOpenedValid = %d, p->cmdState = %d, p->putIdx = %d\n",
		       p->AcStreamOpenedValid, p->cmdState, p->putIdx);
		ERRMSG("EElogStreamHeader 2: p->blockSize = %d, p->lastBlockClosed = %d, p->streamName = %s\n",
		       p->blockSize, p->lastBlockClosed, p->streamName);
		ERRMSG("EElogStreamHeader 2: p->ackNo = %d, p->NumLogstreams = %d, p->bytesReady = %d\n",
		       p->ackNo, p->NumLogstreams, p->bytesReady[0]);
		#endif

		DIAG_FILTER(EE_HANDLER, EE_LOG, NoLog, DIAG_INFORMATION)
		diagPrintf("%s file NOT requested\n", fileName);
		return(0);
	}

#if !defined (OSA_WINCE)
	//The FDI does NOT update the FILE-time-date attributes. So delete and create with new date
	//if(FStatusIsExist(fileName))
	FDI_remove((char *)fileName);
	fHandl = FDI_fopen(fileName, "wb");
#else
	RegistryGetString(HKEY_LOCAL_MACHINE, L"\\Drivers\\BuiltIn\\NVMServer\\Clients\\Comm", L"ReadWritePath", path, MAX_PATH);
	wcstombs(fullPath, path + 2, sizeof(fullPath));
	strcat(fullPath, "\\");
	strcat(fullPath, fileName);
	fHandl = fopen(fullPath, "wb");
#endif

	if (fHandl == 0)
	{
		EELOG_CloseStream(1 /*isFileServer*/);
		return(0);
	}

	SYNC_INFO((p->cmdState == EELOG_OPEN) || (p->cmdState == EELOG_RECORD_READY), p);

	// Wait for first Record Ready
	if ( EELOG_WaitForState(p, EELOG_RECORD_READY, EELOG_TIMEOUT_HUGE_MS) == FALSE)
	{
	#if defined (OSA_LINUX)
		ERRMSG("EElogStreamHeader 3: p->AcStreamOpenedValid = %d, p->cmdState = %d, p->putIdx = %d\n",
		       p->AcStreamOpenedValid, p->cmdState, p->putIdx);
		ERRMSG("EElogStreamHeader 3: p->blockSize = %d, p->lastBlockClosed = %d, p->streamName = %s\n",
		       p->blockSize, p->lastBlockClosed, p->streamName);
		ERRMSG("EElogStreamHeader 3: p->ackNo = %d, p->NumLogstreams = %d, p->bytesReady = %d\n",
		       p->ackNo, p->NumLogstreams, p->bytesReady[0]);
		#endif
		DIAG_FILTER(EE_HANDLER, EE_LOG, ABNORMAL, DIAG_INFORMATION)
		diagPrintf("%s: STATE ABNORMAL: opened, but streaming never started\n", p->streamName);
		l_NumLogstreams = 0;
		goto END_CLOSE_FILE_AND_STREAM;
	}
	SYNC_INFO( TRUE, p);
	getIdx = 0;

	// READ from Stream and
	//   WRITE to File
	retryCycles5mS = EELOG_TIMEOUT_HUGE_MS / 5;

	while (p->cmdState == EELOG_RECORD_READY)
	{
		if (getIdx == p->putIdx)
		{
			if (retryCycles5mS--)
				OSATaskSleep(1);
			else
			{
		#if defined (OSA_LINUX)
				ERRMSG("EElogStreamHeader 4: p->AcStreamOpenedValid = %d, p->cmdState = %d, p->putIdx = %d\n",
				       p->AcStreamOpenedValid, p->cmdState, p->putIdx);
				ERRMSG("EElogStreamHeader 4: p->blockSize = %d, p->lastBlockClosed = %d, p->streamName = %s\n",
				       p->blockSize, p->lastBlockClosed, p->streamName);
				ERRMSG("EElogStreamHeader 4: p->ackNo = %d, p->NumLogstreams = %d, p->bytesReady = %d\n",
				       p->ackNo, p->NumLogstreams, p->bytesReady[0]);
				#endif
				DIAG_FILTER(EE_HANDLER, EE_LOG, STALL, DIAG_INFORMATION)
				diagPrintf("%s: RX streaming stalled. ABORT\n", p->streamName);
				l_NumLogstreams = 0;
				break;
			}
		}
		else
		{
			SYNC_INFO( (p->putIdx - getIdx) <= EELOG_NUM_RECORDS, p);


			idx = getIdx & (EELOG_NUM_RECORDS - 1);

			src = (UINT8*)MAP_PHYSICAL_TO_VIRTUAL_ADDRESS(ADDR_CONVERT(p->recordBufPtr[idx]));

			bytesOk = FDI_fwrite(src, 1, p->bytesReady[idx], fHandl);

	    #if defined (OSA_LINUX)
			DPRINTF("Stream: %s: bytes %d are written!\n", p->streamName, bytesOk);
	    #endif

#if defined (OSA_WINCE) || defined (OSA_LINUX)
#else
			ASSERT(bytesOk == p->bytesReady[idx]);
#endif
			getIdx++;
			p->ackNo = getIdx;
			retryCycles5mS = EELOG_TIMEOUT_HUGE_MS / 5;
		}
	}

 END_CLOSE_FILE_AND_STREAM:

#if defined (OSA_LINUX)
	fsync((int)fHandl);
#endif

	FDI_fclose(fHandl);
	SYNC_INFO((p->cmdState == EELOG_CLOSE), p);
    #if defined (OSA_WINCE)
	RETAILMSG(1, (TEXT("[ERROR Handler] *2.EELOG_shareReadFWrite-END_CLOSE_FILE_AND_STREAM* p=%x * .\r\n"), p));
	#endif
	#if defined (OSA_LINUX)
	DBGMSG("EELOG_shareReadFWrite: END_CLOSE_FILE_AND_STREAM *p = %p, l_NumLogstreams = %d\n", p, l_NumLogstreams);
    #endif
	EELOG_CloseStream(1 /*isFileServer*/);
	return(l_NumLogstreams);
}



#if defined (OSA_WINCE)
void WinCE_EELOG_shareReadFWrite()
{

	// Do LOG-Stream for ASSERT-found only, otherwise nobody sends EELOGs
	if ( EE_AssistingMaster_ASSERT_found ) NumLogstreams = 1;
	else NumLogstreams = 0;

	if (NumLogstreams > 0)
	{
		UINT32 streamNo;
		for (streamNo = 0; streamNo < NumLogstreams; streamNo++)
			NumLogstreams = EELOG_shareReadFWrite();
	}

	// DDR-RW dump takes a lot of time.
	// Do it for ASSERT-found only, but don't if "fast" silent reset required
	if ( EE_AssistingMaster_ASSERT_found ) //&& (fAction != EE_RESET_COMM_SILENT) )
		EE_SaveComDDR_RW(eeConfiguration.dumpDdrSizeBytes);

}
#endif //OSA_WINCE

#if defined (OSA_LINUX)
void Linux_EELOG_shareReadFWrite()
{

	// Do LOG-Stream for ASSERT-found only, otherwise nobody sends EELOGs
	if ( EE_AssistingMaster_ASSERT_found ) NumLogstreams = 1;
	else NumLogstreams = 0;

	if (NumLogstreams > 0)
	{
		UINT32 streamNo;
		for (streamNo = 0; streamNo < NumLogstreams; streamNo++)
			NumLogstreams = EELOG_shareReadFWrite();
	}
}

#endif


#endif //EEH_ASSISTING_SERVER

#if defined (EEH_DUMP_DTCM)
void EELOG_streamOutDTCM(void)
{
	extern UINT32 Image$$DDR_DTCM$$Base;
	UINT8*      pBuf = (UINT8*)(&Image$$DDR_DTCM$$Base);
	UINT32 len  =  0x40000;
	FILE*       p;

	p = EELOG_fopen("DTCM", "wb");
	if (p != NULL)
	{
		EELOG_fwrite(pBuf, 1, len, p);
		EELOG_fclose(p);
	}
}

#endif //EEH_DUMP_DTCM


#if defined (PLATFORM_ONLY)
#if defined (EEH_ASSISTED_CLIENT)
//ICAT EXPORTED FUNCTION - EE_HANDLER,EE,EELOG_TestDump
void EELOG_TestDump(void* par)
{
	UINT32*     pVal = (UINT32*)par;

	if (pVal[1] == 0)
	{
		DIAG_FILTER(EE_HANDLER, EE, EELOG_Test, DIAG_INFORMATION)
		diagPrintf("EELOG_Test(ADDR, LEN)");
	}
	else
	{
		UINT8*      pBuf = (UINT8*)pVal[0];
		UINT32 len  =          pVal[1];
		FILE*       p;
		UINT32 delay = 1;
		extern void watchdogConfigINT_ICAT(void *param);
		//eeNotify2AssistMaster();
		watchdogProtect(FALSE);
		watchdogConfigINT_ICAT(&delay);
		watchdogActivate();
		watchdogProtect(TRUE);

		p = EELOG_fopen("DTCM_1", "wb");
		EELOG_fwrite(pBuf, 1, len, p);
		EELOG_fclose(p);

		p = EELOG_fopen("DTCM_2", "wb");
		EELOG_fwrite(pBuf, 1, len, p);
		EELOG_fclose(p);

		p = EELOG_fopen("DTCM_3", "wb");
		EELOG_fwrite(pBuf, 1, len, p);
		EELOG_fclose(p);

		DIAG_FILTER(EE_HANDLER, EE, EELOG_TestEND, DIAG_INFORMATION)
		diagPrintf("EELOG_Test END");
	}
}
#endif //EEH_ASSISTED_CLIENT
#endif //PLATFORM_ONLY

#endif //defined (EEH_ASSISTED_CLIENT) || defined (EEH_ASSISTING_SERVER)


