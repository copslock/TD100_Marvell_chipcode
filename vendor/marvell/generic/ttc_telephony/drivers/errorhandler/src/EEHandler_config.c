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
* Filename: EEHandler_config.c
*
* Target, platform: Hermon
*
* Authors: Anton Eidelman
*
* Description: Implementation file for Error handler configuration and NVM
*
*
*
* Notes:
******************************************************************************/

/*----------- include files -----------------------------------------*/




#if defined (OSA_WINCE)

#include <windows.h>
#include <Regext.h>

#include <diag_api.h>
#include "xllp_intc.h"
#include "tavorP_reset.h"

#else

#include <string.h>  // for memcpy() & strcpy()
#include <stdio.h>
//#include "hal_cfg.h"

#endif //OSA_WINCE


#if defined (OSA_WINCE) || defined (OSA_LINUX)
#define EE_HANDLER_ENABLE
#define INTEL_FDI
#endif //OSA_WINCE

#if defined(EE_HANDLER_ENABLE)

#if !defined (OSA_WINCE)

#include "osa.h"
#include "EEHandler.h"
#include "EEHandler_int.h"

#if defined (OSA_LINUX)
#include "pxa_dbg.h"
#endif

//#include "csw_mem.h"
//#include "utils.h"
//#include "watchdog.h"

/*
 * File name
 */
const char* const eehLogFileName = "EEhandlerV3_Log.bin";

#endif  //!defined (OSA_WINCE)



#if defined (OSA_WINCE)

const char* const eehCfgFileName = "EEHandlerConfig_WinCe.nvm";
typedef  FILE*  FILE_ID;
#define MIN(x, y)  (( (x) < (y) ) ? (x) : (y))

#elif defined (OSA_LINUX)

const char* const eehCfgFileName = "EEHandlerConfig_Linux.nvm";

#else

const char* const eehCfgFileName = "EEHandlerConfig.nvm";
#endif

/*
 * EEHandler actual configuration parameters
 */
#define EEHANDLER_NVM_VERSION      "2.0"      /* Change from 1.0 to 2.0 */

#include "EEHandler_nvm_default.h"
#include "EEHandler_config.h"

#if !defined (OSA_WINCE)
#include "EEHandler.h"
#endif

#if !defined (OSA_WINCE)
#include "EEHandler_int.h"
#endif  //!defined (OSA_WINCE)


EE_Configuration_t eeConfiguration;

#if defined (INTEL_FDI)
/*
 * NVM access
 */

typedef enum
{
	nvhsOK,
	nvhsFileTooShort,
	nvhsStructNameMismatch,
	nvhsStructFormat
} NVM_Header_Status_te;
static BOOL eeWriteNvmHeader(FILE_ID fdiID);
static UINT32 nvmReadHeader(FILE_ID fdiID, const char* sName);

#endif //INTEL_FDI


//ICAT EXPORTED FUNCTION - EE_HANDLER,EE,eePrintConfiguration
void eePrintConfiguration(void)
{
#if (0)
	DIAGM_TRACE_1(EE_HANDLER, EE, EE_CONFIG1, "AssertHandler(off/on)               = %b", eeConfiguration.AssertHandlerOn);
	DIAGM_TRACE_1(EE_HANDLER, EE, EE_CONFIG2, "ExceptionHandler(off/on)            = %b", eeConfiguration.ExcepHandlerOn);
#ifdef EE_WARNINGS
	DIAGM_TRACE_1(EE_HANDLER, EE, EE_CONFIG3, "WarningHandler(off/on)              = %b", eeConfiguration.WarningHandlerOn);
#endif
	DIAGM_TRACE_1(EE_HANDLER, EE, EE_CONFIG4, "powerUpLogOn trace(off/on)          = %b", eeConfiguration.powerUpLogOn);
	DIAGM_TRACE_1(EE_HANDLER, EE, EE_CONFIG5, "Ext.handlers (off/on)               = %b", eeConfiguration.extHandlersOn);
	DIAGM_TRACE_1(EE_HANDLER, EE, EE_CONFIG6, "printRecentLogOnStartup (off/on)    = %b", eeConfiguration.printRecentLogOnStartup);
	DIAGM_TRACE_1(EE_HANDLER, EE, EE_CONFIG7, "FinalAction(reset/stall/cont/ext)   = %b", eeConfiguration.finalAction   );
	DIAGM_TRACE_1(EE_HANDLER, EE, EE_CONFIG8, "LogFile size                        = %d", eeConfiguration.EELogFileSize);
	DIAGM_TRACE_1(EE_HANDLER, EE, EE_CONFIG9, "Delay on startup (x5ms)             = %d", eeConfiguration.delayOnStartup);
#else
#if defined(OSA_LINUX)
	DBGMSG("*********%s: begin*********** \n", __FUNCTION__);
	DBGMSG("eeConfiguration.AssertHandlerOn:%d\n", eeConfiguration.AssertHandlerOn);
	DBGMSG("eeConfiguration.ExcepHandlerOn:%d\n", eeConfiguration.ExcepHandlerOn);
	DBGMSG("eeConfiguration.WarningHandlerOn:%d\n", eeConfiguration.WarningHandlerOn);
	DBGMSG("eeConfiguration.powerUpLogOn:%d\n", eeConfiguration.powerUpLogOn);
	DBGMSG("eeConfiguration.extHandlersOn:%d\n", eeConfiguration.extHandlersOn);
	DBGMSG("eeConfiguration.printRecentLogOnStartup:%d\n", eeConfiguration.printRecentLogOnStartup);
	DBGMSG("eeConfiguration.finalAction:%d\n", eeConfiguration.finalAction);
	DBGMSG("eeConfiguration.EELogFileSize:%d, defaultFileSize: %d\n", eeConfiguration.EELogFileSize, EEHANDLER_DEFAULT_LOGFILE_SIZE);
	DBGMSG("eeConfiguration.delayOnStartup:%d\n", eeConfiguration.delayOnStartup);
	DBGMSG("*********%s: end ************ \n", __FUNCTION__);
#endif
	DIAG_FILTER(EE_HANDLER, EE, EE_CONFIG_PRINT, DIAG_INFORMATION)
	diagStructPrintf("EE CONFIGURATION: %S{EE_Configuration_t}", &eeConfiguration, sizeof(eeConfiguration));
#endif
}

/*******************************************************************************
* Function: eeInitXxxxConfiguration
*******************************************************************************
* Description:  Set default configuration
*  eeInitDfltConfiguration - Non blocking copy default into working
*  eeInitNvmConfigSuccess  - BLOCKING read NVM and copy into working
*  eeInitConfiguration     - both inside
*
*******************************************************************************/
//ICAT EXPORTED FUNCTION - EE_HANDLER,EE,eeInitConfiguration
void eeInitConfiguration(void)
{
	eeInitDfltConfiguration();
	eeInitNvmConfigSuccess(EE_NVMCONFIG_UNCODITIONAL);
}

//-------------------------------
void eeInitDfltConfiguration(void)
{

	eeConfiguration = eeDefaultConfiguration;
}

//-------------------------------
BOOL eeInitNvmConfigSuccess(EE_NvmConfigMode_t mode)
{
#if defined (INTEL_FDI)
	EE_Configuration_t tmpBuf;

	if ( ((mode == EE_NVMCONFIG_EARLY)  &&  NVM_ON_REMOTE)
	     || ((mode == EE_NVMCONFIG_LATEST) && !NVM_ON_REMOTE) )
	{
		return(FALSE); /*nothing required at this time*/
	}

	if (eeReadConfiguration(&tmpBuf))
	{
		//NVM file exist, and data now in the tmpBuf
#if defined (OSA_WINCE)
		cpsrReg = XllpIntcDisableInterrupts();
#elif !defined (OSA_LINUX)
		cpsrReg = disableInterrupts();
#endif
		eeConfiguration = tmpBuf;

		eePrintConfiguration();

#if defined (OSA_WINCE)
		XllpIntcRestoreInts(cpsrReg);
#elif !defined (OSA_LINUX)
		restoreInterrupts(cpsrReg);
#endif
	}
	else
	{
		//NVM file not exist, create it and continue with DFLT-configuration
    #if defined(EE_CREATE_DEFAULT_CONFIGURATION_FILE)
		eeSaveConfiguration();
#if !defined (OSA_WINCE) && !defined (OSA_LINUX)
		DIAGM_TRACE(EE_HANDLER, EE, EE_NOCONFIG, "No valid EE configuration file found. Created default file.");
#endif
    #endif
	}
	return(TRUE);
#else
	return(FALSE);
#endif  //FDI
}


/*******************************************************************************
* Function: eeSetConfiguration
*******************************************************************************
* Description:  configure the EEhandler
*
* Parameters:
* Output Param:
*
* Return value:
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - EE_HANDLER,EE,eeSetConfiguration
void  eeSetConfiguration(const EE_Configuration_t* pConfig, int len)
{
	eeConfiguration = eeDefaultConfiguration;
#if defined (INTEL_FDI)
	if (pConfig && len > 0)
	{
		memcpy(&eeConfiguration, pConfig, MIN((size_t)len, sizeof(EE_Configuration_t)));
	}

	eePrintConfiguration();
#endif //FDI
}

/******************************************************************************
* Function: eeReadConfiguration
*******************************************************************************
* Description: read current configuration without DIAG TRACE for phase2.
* Parameters: None
*
* Return value:
*
* Notes:
******************************************************************************/
//ICAT EXPORTED FUNCTION - EE_HANDLER,EE,eeReadConfiguration
void eeReadConfiguration_ICAT( void )
{
	eeReadConfiguration(NULL);
}

BOOL eeReadConfiguration(EE_Configuration_t* pCfgBuf)
{
	BOOL valid = FALSE;

#if defined (INTEL_FDI)
	FILE_ID fdiID;
	UINT16 count;
	//ERR_CODE          ret_val;


#if defined (OSA_WINCE)
	TCHAR path[MAX_PATH] = { 0 };
	char fullPath[MAX_PATH] = { 0 };
#endif

	if (pCfgBuf == NULL)
		pCfgBuf = &eeConfiguration;


 #if defined (OSA_WINCE)
	RegistryGetString(HKEY_LOCAL_MACHINE, L"\\Drivers\\BuiltIn\\NVMServer\\Clients\\Comm", L"ReadWritePath", path, MAX_PATH);
	wcstombs(fullPath, path + 2, sizeof(fullPath));
	strcat(fullPath, "\\");
	strcat(fullPath, eehCfgFileName);
	fdiID = fopen(fullPath, "rb");

#else

	fdiID  =  FDI_fopen(eehCfgFileName, "rb");
 #endif

	if (fdiID) //success on open file . now read it
	{
		if (nvmReadHeader(fdiID, "EE_Configuration_t") >= sizeof(EE_Configuration_t))
		{
 #if defined (OSA_WINCE)
			count = fread(pCfgBuf, sizeof(EE_Configuration_t), 1, fdiID );
 #else
			count = FDI_fread(pCfgBuf, sizeof(EE_Configuration_t), 1, fdiID );
 #endif

			if (count == 1) valid = TRUE;

		}
 #if defined (OSA_WINCE)
		fclose(fdiID);
 #else
		/*ret_val = */ FDI_fclose(fdiID);
 #endif
	}
	if (valid)
	{

#if !defined (OSA_WINCE)
		DIAGM_TRACE(EE_HANDLER , EE , EE_CONFIG_OK, "EEHandler configuration file is valid");
#else
		RETAILMSG(1, ( _T("*** [ EEhandler] eeReadConfiguration - EEHandler configuration file is valid...***\r\n")));
#endif
	}
	else
		eeConfiguration = eeDefaultConfiguration;  //restore default
#endif //FDI
	return valid;
}


/******************************************************************************
* Function: eeSaveConfiguration
*******************************************************************************
* Description: read current configuration without DIAG TRACE for phase2.
* Parameters: None
*
* Return value:
*
* Notes:
******************************************************************************/
//ICAT EXPORTED FUNCTION - EE_HANDLER,EE,eeSaveConfiguration
void eeSaveConfiguration( void )
{
#if defined (INTEL_FDI)
	FILE_ID fdiID;

 #if defined (OSA_WINCE)
	TCHAR path[MAX_PATH] = { 0 };
	char fullPath[MAX_PATH] = { 0 };
	RegistryGetString(HKEY_LOCAL_MACHINE, L"\\Drivers\\BuiltIn\\NVMServer\\Clients\\Comm", L"ReadWritePath", path, MAX_PATH);
	wcstombs(fullPath, path + 2, sizeof(fullPath));
	strcat(fullPath, "\\");
	strcat(fullPath, eehCfgFileName);
	fdiID = fopen(fullPath, "wb");

 #else
	fdiID  =  FDI_fopen(eehCfgFileName, "wb");
 #endif

	//UINT16            count;
	//ERR_CODE          ret_val;

	if (!fdiID)
#if !defined (OSA_WINCE)
		EE_INTERNAL_ASSERT(__LINE__);
#else
		RETAILMSG(1, ( _T("*** [ EEhandler] eeSaveConfiguration - cant open file to save...***\r\n")));


#endif
	else
	{
	if (nvmReadHeader(fdiID, "EE_Configuration_t") < sizeof(EE_Configuration_t))  //reserve existing header if valid
	{
		if (!eeWriteNvmHeader(fdiID))
#if !defined (OSA_WINCE)
			EE_INTERNAL_ASSERT(__LINE__);
#else
			RETAILMSG(1, ( _T("*** [ EEhandler] eeSaveConfiguration - cant write - eeWriteNvmHeader...***\r\n")));
#endif
	}

#if defined (OSA_WINCE)
	if ((/*count = */ fwrite(&eeConfiguration, sizeof(EE_Configuration_t), 1, fdiID )) != 1)
		RETAILMSG(1, ( _T("*** [ EEhandler] eeSaveConfiguration - cant write - fwrite...***\r\n")));
	/*ret_val = */ fclose(fdiID);
	//SystemResetType = eeConfiguration.finalAction; //update the final action from user to SW
	//RETAILMSG(1, (TEXT("[SHACHAR_DEBUG] **  eeSaveConfiguration SystemResetType =%d ** .\r\n"),SystemResetType));

#else
	if ((/*count = */ FDI_fwrite(&eeConfiguration, sizeof(EE_Configuration_t), 1, fdiID )) != 1)
		EE_INTERNAL_ASSERT(__LINE__);
	/*ret_val = */ FDI_fclose(fdiID);
 #endif
	}
#endif //FDI


}

//-----------------------------------------------------------
#if defined (INTEL_FDI)

 #define NVM_HEAD_SET_STRING(header, field, value) \
	strncpy(header.field, value, sizeof(header.field) - 1)
#endif //FDI

#if defined (INTEL_FDI)
static BOOL eeWriteNvmHeader(FILE_ID fdiID)
{
	NVM_Header_ts header;

 #if defined (OSA_WINCE)
	fseek(fdiID, 0, SEEK_SET);
 #else
	FDI_fseek(fdiID, 0, SEEK_SET);
 #endif

	header.StructSize = sizeof(EE_Configuration_t);
	header.NumofStructs = 1;
	NVM_HEAD_SET_STRING(header, StructName, "EE_Configuration_t");
	NVM_HEAD_SET_STRING(header, Date, "");
	NVM_HEAD_SET_STRING(header, time, "");
	NVM_HEAD_SET_STRING(header, Version,  EEHANDLER_NVM_VERSION);
	NVM_HEAD_SET_STRING(header, HW_ID, "");
	NVM_HEAD_SET_STRING(header, CalibVersion, "");

#if defined (OSA_LINUX)
	DBGMSG("%s: header.StructSize: %d, header.Version: %s\n", __FUNCTION__, header.StructSize, header.Version);
#endif

 #if defined (OSA_WINCE)
	if (fwrite(&header, sizeof(NVM_Header_ts), 1, fdiID ) != 1) return FALSE;

 #else
	if (FDI_fwrite(&header, sizeof(NVM_Header_ts), 1, fdiID ) != 1) return FALSE;
 #endif


	return TRUE;
}
#endif //FDI

#if defined (INTEL_FDI)


static UINT32 nvmReadHeader(FILE_ID fdiID, const char* sName)
{
	NVM_Header_ts header;
	UINT16 count;
	UINT32 ret;

  #if defined (OSA_WINCE)
	count = fread(&header, sizeof(NVM_Header_ts), 1, fdiID );
  #else
	count = FDI_fread(&header, sizeof(NVM_Header_ts), 1, fdiID );
  #endif


	if (count != 1) {
		ret = nvhsFileTooShort;
		goto err;
	}
	if (header.NumofStructs != 1) {
		ret = nvhsStructFormat;
		goto err;
	}
	if (strcmp(sName, header.StructName)) {
		ret = nvhsStructNameMismatch;
		goto err;
	}
	if (strcmp(EEHANDLER_NVM_VERSION, header.Version)) {
		ret = nvhsStructNameMismatch;
		goto err;
	}
	if (header.StructSize != sizeof(EE_Configuration_t)) {
		ret = nvhsStructFormat;
		goto err;
	}
  #if defined (OSA_LINUX)
	DBGMSG("%s: count: %d, sName:%s header.StructName:%s\n", __FUNCTION__, count, sName, header.StructName);
	DBGMSG("%s: header.NumofStructs:%d, header.Version:%s, header.StructSize:%d\n",
	       __FUNCTION__, header.NumofStructs, header.Version, header.StructSize);
  #endif
	return header.StructSize;

err:
  #if defined (OSA_LINUX)
	DBGMSG("%s: an invalid nvm header.\n", __FUNCTION__);
  #endif
	return ret;
}
#endif //FDI


/******************************************************************************
* Function: eeHandlerWatchdogSet
* Function: eeHandlerWatchdogUpdate
*******************************************************************************
* Description:
*  Configures the REGULAR behaviour of the watchdog timer (WDT) with
*                                      time match value and the INT/RST mode.
*  Time match value is obtained from config-NVM; if it is ZERO, the WDT is disabled.
*  Recommended value is 6 seconds, so the maximal WCDMA/GSM DRX cycle will be supported.
*
*  Called on startup.
*
*  May and should be re-configured for Flight-mode and deep power saving mode by
*                                           eeHandlerWatchdogUpdate()
*
*  If ASSERT or EXCEPTION is occured WDT is reconfigured by EEhandler_fatal.
*
*  Refer also << hal_config.h::WATCHDOG_DISABLE >>
*
******************************************************************************/
  #if !defined(OSA_WINCE) && !defined (OSA_LINUX)

typedef void (*void_pF_UINT32)(UINT32 tmrId);
static OSATimerRef wdtOsTimerRef = NULL;
static void eeWatchdogInterruptRoutine(void)
{
	WARNING(0);
}                                                             //normally should NEVER be achieved

static void eeWatchdogKickOsThreadStart(UINT32 timeMSec)
{
	UINT32 ticks = timeMSec / 5;

	if (wdtOsTimerRef == NULL)
		OSATimerCreate(&wdtOsTimerRef);
	else
		OSATimerStop(wdtOsTimerRef);
	OSATimerStart(wdtOsTimerRef, ticks, ticks, (void_pF_UINT32)watchdogKick, 0);
}

void eeHandlerWatchdogUpdate(UINT16 wdtConfigTimeMSec)
{
	if (wdtConfigTimeMSec != EE_WDT_OFF)
	{
		WATCHDOG_Configuration wdtConfig;
		WATCHDOG_ReturnCode rc;

		watchdogRegister(eeWatchdogInterruptRoutine);
		wdtConfig.matchValue = (((UINT32)wdtConfigTimeMSec) * 13) / 10; // plus 30% guard spare time

	  #if defined (_TAVOR_HARBELL_)
		wdtConfig.mode = WATCHDOG_INTERRUPT_MODE;
	  #else
		wdtConfig.mode = WATCHDOG_RESET_MODE;
	  #endif

		rc = watchdogConfigure(&wdtConfig);
		ASSERT(rc == WATCHDOG_RC_OK);

		//Starting the OS-timer which calls for kick.
		eeWatchdogKickOsThreadStart((UINT32)wdtConfigTimeMSec);

		rc = watchdogActivate();
		ASSERT(rc == WATCHDOG_RC_OK);

		DIAG_FILTER(EE_HANDLER, EE_LOG, WDT, DIAG_INFORMATION)
		diagPrintf("EE-WATCHDOG started: osKick=%ld, wdt=%ld mSec", wdtConfigTimeMSec, wdtConfig.matchValue);
	}
}

void eeHandlerWatchdogSet(void)
{
  #if defined (_TAVOR_HARBELL_)
	//Currently supported for HARBELL only
	eeHandlerWatchdogUpdate(eeConfiguration.wdtConfigTime);
  #endif
}


  #endif //!OSA_WINCE


#endif //EE_HANDLER_ENABLE

