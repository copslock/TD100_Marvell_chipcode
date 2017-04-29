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
*     File name:      diag_api .h                                                 *
*     Programmer:     Shiri Dolev                                                 *
*                     Ohad Peled, Itzik Yankilevich, Barak Witkowsky, Yaeli Karni *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*       Create Date:  November, 2003                                              *
*                                                                                 *
*       Description: DIAG Tx APIs - MACROs.									   *
*                                                                                 *
*       Notes:  There are 2 files diag_API_var.h and diag_API.h                   *
*               The diag_API_var.h - declaration without MACRO-re-define          *
*               The diag_API.h     - has part which is re-defined by MACROs       *
*                                    and treated differently by PrePassProcessor! *
*                                                                                 *
* July 2007 - fix macros DIAT_TRACEM... (IY)									   *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if !defined (DIAG_API_H)
#define     DIAG_API_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "global_types.h"

#if defined (OSA_WINCE)
// The below definition is needed for WinCE, as its compiler
// does not support ANSCI_C99 Macros with ... (unfortunately)
	#define NO_MACRO_ANSI_C99__VA_ARGS__
#endif

#if !defined(NODIAG)

#include "diag_types.h"
#include "diag_config.h"
#include "diag_pdu.h"
/////////#include "diag_tx.h"
#include "diag_osif.h"

#if defined (OSA_WINCE)
#ifdef  __cplusplus // - do WinCE only
#include "diagdb.h"
#endif //cplusplus
#endif //OSA_WinCE

#define DIAG_INVALID_MODULE_ID  (-1)            // module ID can be U16 only, so invalid value will be all 11's

#if defined (ENABLE_DIAG_LOGGER)
#include    "diaglogger.h"
DIAG_EXPORT DiagLoggerDefs diagLoggerConfig;
#endif // defined (ENABLE_DIAG_LOGGER)
// definition of Callbacks for Memory and DiagSig
typedef BOOL (*DiagPSisRunningFn)(void);
DIAG_EXPORT void SetDiagSigPScheckFn(DiagPSisRunningFn diagSigPsCheckFn);
// for memeory allocation
DIAG_EXPORT void SetTraceBeFiltered(void);
DIAG_EXPORT void SetTraceNotFiltered(void);
DIAG_EXPORT UINT32 isTraceFiltered(void);
DIAG_EXPORT UINT32 isDiagTracePermitted(void);
DIAG_EXPORT void setDiagTraceProhibited(UINT8 setProhibit);
DIAG_EXPORT void setDiagBlockPDU(void);
DIAG_EXPORT TxPDU * diagAlignAndHandleErr(UINT32 lendata, UINT32 msgID, UINT32 *numOfDecreasedBytes, UINT16 sap_type);
DIAG_EXPORT UINT32 isDiagBlocked(void);
DIAG_EXPORT UINT32 isDiagConnected(void);

#if !defined (OSA_WINCE)
void diagPhase1Init(void);
void diagPhase2Init(void);
#endif

#if defined (OSA_NUCLEUS)
// for internal usage only by diag!!
extern DiagBlockReasons diagBlocked;
#endif
// must be defined here - for compile phase (diagDB.h is empty at start)
#if defined DIAG_DB_C
DIAG_EXPORT const DiagTraceDescriptor*  diagTraceDesc;
// defined in diagDB.c - so all others are import..
DIAG_EXPORT UINT8 filterArray[];
DIAG_EXPORT UINT8 g_OverridePassFilter;
#else
DIAG_IMPORT const DiagTraceDescriptor*  diagTraceDesc;
// defined in diagDB.c - so all others are import..
DIAG_IMPORT UINT8 filterArray[];
DIAG_IMPORT UINT8 g_OverridePassFilter;
#endif
/************************
*		Typedefs		*
************************/
typedef enum {
	DIAGM_RC_OK,
	DIAGM_RC_POINTER_NOT_ALLIGNED
}DIAGM_ReturnCode;

typedef enum {
	SPLIT_FILTER_STATE_NONE,
	SPLIT_FILTER_STATE_REPORT_NONE,
	SPLIT_FILTER_STATE_REPORT_ENABLE_ALL,
	SPLIT_FILTER_STATE_REPORT_DISABLE_ALL,
	SPLIT_FILTER_STATE_DISABLE_ALL,
	SPLIT_FILTER_STATE_ENABLE_ALL,
	SPLIT_FILTER_STATE_ARRAY
}e_FilteringState;

/************************
*		Prototypes		*
************************/
#if defined (NO_MACRO_ANSI_C99__VA_ARGS__)
DIAG_EXPORT void diagStructPrintf_NoText(/*const char *fmt, */ void *data, UINT16 length);
DIAG_EXPORT void diagTextPrintf_NoText(/*const char *fmt, */ void);
DIAG_EXPORT void diagPrintf_Extend(const char *fmt, ...);
#else
DIAG_EXPORT void diagStructPrintf_NoText(const DiagTraceDescriptor* diagTraceDesc, void *data, UINT16 length);
DIAG_EXPORT void diagTextPrintf_NoText(const DiagTraceDescriptor* diagTraceDesc);
DIAG_EXPORT void diagPrintf_Extend(const DiagTraceDescriptor* diagTraceDesc, const char *fmt, ...);
#endif
// diagmPrintf - left for backword compatibility. Used in embedded SW in some places
DIAG_EXPORT void diagmPrintf( UINT16 moduleID, char *format, ... );    //Has NO pair DIAG_FILTER
DIAG_EXPORT void diagSendPDU(UINT8 type, UINT16 moduleID, UINT16 msgID, void *txPduBuffPtr, UINT16 dataLength, UINT32 numOfDecreasedBytes);

/************************
*		Defines			*
************************/

/****************************
*		Hermon MACROs		*
****************************/
//PPC - pre-pass Compiler
#if !defined (_CEPPC_) && !defined (_PPC_)

#if defined (NO_MACRO_ANSI_C99__VA_ARGS__)
/* Since the MACRO(__VA_ARGS__) is not supported, there is
 * - no FILTER/printf pair checking,
 * - the diag descriptor is passed as global variable,
 * - the global diag descriptor is mutual by interruptsDisable/Restore
 */
 #define diagPrintf                         diagPrintf_Extend
 #define diagTextPrintf(text)               diagTextPrintf_NoText()
 #define diagStructPrintf(text, ptr, nbytes)  diagStructPrintf_NoText(ptr, nbytes)
// diagStructPrintfPrealloc - not yet implemented!
//#define diagStructPrintfPrealloc(text,ptr,nbytes,ptrdata)  diagStructPrintf_NoText(ptr,nbytes)
#else // NO_MACRO_ANSI_C99__VA_ARGS__
 #if !defined (ENABLE_DIAG_LOGGER)
 #define diagPrintf(...)                    diagPrintf_Extend(diagTraceDesc, __VA_ARGS__); } }
 #define diagTextPrintf(text)               diagTextPrintf_NoText(diagTraceDesc); } }
 #define diagStructPrintf(text, ptr, nbytes)  diagStructPrintf_NoText(diagTraceDesc, ptr, nbytes); } }
// diagStructPrintfPrealloc - not yet implemented!
//#define diagStructPrintfPrealloc(text,ptr,nbytes,ptrdata)  diagStructPrintf_NoText(diagTraceDesc, ptr, nbytes); }}
 #else
 #define diagPrintf(...)                    diagPrintf_Extend(diagTraceDesc, __VA_ARGS__); } } }
 #define diagTextPrintf(text)               diagTextPrintf_NoText(diagTraceDesc); } } }
 #define diagStructPrintf(text, ptr, nbytes)  diagStructPrintf_NoText(diagTraceDesc, ptr, nbytes); } } }
// diagStructPrintfPrealloc - not yet implemented!
//#define diagStructPrintfPrealloc(text,ptr,nbytes,ptrdata)  diagStructPrintf_NoText(diagTraceDesc, ptr, nbytes); }}}
 #endif
#endif // NO_MACRO_ANSI_C99__VA_ARGS__


/*
	The macro IS_DIAG_BLOCKED translates to checking of global variable in Nucleus, to have
	less impact on runtime when ACAT is not connected (operational, production, field testing)
	In WinCE (and Linux?), the usage of global is a bit difficult due to the required import/export
	definition needed from different dll's. Currently this is left with a function call (more instructions
	are performed before finding out no trace should be sent)
 */
#if defined (NO_MACRO_ANSI_C99__VA_ARGS__)
#if !defined (ENABLE_DIAG_LOGGER)

#define DIAG_FILTER(CAT1, CAT2, CAT3, reportLevel)			    \
	{									    \
		int traceBlockCheck = DIAG_INVALID_MODULE_ID;			 \
		DIAG_FILTER_EXTERN(CAT1, CAT2, CAT3);					       \
		if (!IS_DIAG_BLOCKED) /* (!isDiagBlocked()))*/						\
		{								\
			traceBlockCheck    =   __trace_ ## CAT1 ## __ ## CAT2 ## __ ## CAT3(); \
			if (traceBlockCheck != DIAG_INVALID_MODULE_ID)										   \
				SetTraceNotFiltered();													 \
		}								\
	}									    \
	if ((!IS_DIAG_BLOCKED) && (!isTraceFiltered()) )    // warning here on __trace_...

#else // defined (ENABLE_DIAG_LOGGER)

#define DIAG_FILTER(CAT1, CAT2, CAT3, reportLevel)			    \
	{									    \
		int traceBlockCheck;						\
		DIAG_FILTER_EXTERN(CAT1, CAT2, CAT3);					   \
		traceBlockCheck    =   __trace_ ## CAT1 ## __ ## CAT2 ## __ ## CAT3();	      \
		if (traceBlockCheck != DIAG_INVALID_MODULE_ID)												       \
			SetTraceNotFiltered();												       \
		if (diagLoggerConfig.bEnabled && (!isTraceFiltered()))													       \
			DiagLoggerAddTraceToLog(diagTraceDesc->moduleId,			\
						diagTraceDesc->reportId,					      \
						(UINT32)GET_TIMESTAMP,			      \
						FALSE,           /*not opcode*/							\
						0,										      \
						0, NULL);																		  \
	}									    \
	if ((!isTraceFiltered() ) || (IS_DIAG_BLOCKED))													\
		diag_unlock_G();											 \
	else                                                                      /*M_END*/

#define DIAG_DSP_FILTER(CAT1, CAT2, CAT3, reportLevel, data, dataLen)	      \
	{									    \
		int traceBlockCheck;						\
		DIAG_FILTER_EXTERN(CAT1, CAT2, CAT3);					   \
		traceBlockCheck    =   __trace_ ## CAT1 ## __ ## CAT2 ## __ ## CAT3();	      \
		if (traceBlockCheck != DIAG_INVALID_MODULE_ID)												       \
			SetTraceNotFiltered();												       \
		if (diagLoggerConfig.bEnabled && (!isTraceFiltered()))													       \
			DiagLoggerAddTraceToLog(diagTraceDesc->moduleId,			\
						diagTraceDesc->reportId,					      \
						(UINT32)GET_TIMESTAMP,			      \
						TRUE, /* op code included*/							\
						/*OpCode */ *((UINT16*)data),				      \
						MIN(dataLen, diagLoggerConfig.nMaxDataPerTrace), \
						data);																		       \
	}									    \
	if ((!isTraceFiltered() ) || (IS_DIAG_BLOCKED))													\
		diag_unlock_G();											 \
	else                                                                      /*M_END*/

#endif // defined (ENABLE_DIAG_LOGGER)
      //defined NO_MACRO_ANSI_C99__VA_ARGS__
#else //
     // !defined NO_MACRO_ANSI_C99__VA_ARGS__


#if !defined (ENABLE_DIAG_LOGGER)
#define DIAG_FILTER(CAT1, CAT2, CAT3, reportLevel)			   \
	if (!IS_DIAG_BLOCKED)						      \
	{								       \
		DIAG_FILTER_EXTERN(CAT1, CAT2, CAT3);					       \
		if (__trace_ ## CAT1 ## __ ## CAT2 ## __ ## CAT3() == 1)		      \
		{       /*------ MACRO END PART1---------------------------------M_END--*/
/*---PART2:  diagPrintf() or diagTextPrintf() or diagStructPrintf();       \
	}                                                                  \
    } ----------- 2 brackets----------------------------------------M_END--*/

#else // defined (ENABLE_DIAG_LOGGER)

#define DIAG_FILTER(CAT1, CAT2, CAT3, reportLevel)			    \
	if (diagLoggerConfig.bEnabled || !IS_DIAG_BLOCKED)			\
	{									\
		DIAG_FILTER_EXTERN(CAT1, CAT2, CAT3);						\
		if (__trace_ ## CAT1 ## __ ## CAT2 ## __ ## CAT3() == 1)		      \
		{								    \
			if ( diagLoggerConfig.bEnabled )				\
			{								\
				DiagLoggerAddTraceToLog(diagTraceDesc->moduleId,	    \
							diagTraceDesc->reportId,						    \
							(UINT32)GET_TIMESTAMP,					    \
							FALSE,   /*not opcode*/							\
							0,										      \
							0, NULL);										 \
			}								\
			if ( !IS_DIAG_BLOCKED )						\
			{       /*------ MACRO END PART1-------------------------------M_END-*/
/*---PART2:     diagPrintf() or diagTextPrintf() or diagStructPrintf();     \
	    }                                                               \
	}                                                                   \
    } ----------- 3 brackets------------------------------------------M_END-*/


#define DIAG_DSP_FILTER(CAT1, CAT2, CAT3, reportLevel, data, dataLen)	      \
	if (diagLoggerConfig.bEnabled || !IS_DIAG_BLOCKED)			\
	{									\
		DIAG_FILTER_EXTERN(CAT1, CAT2, CAT3);							    \
		if (__trace_ ## CAT1 ## __ ## CAT2 ## __ ## CAT3() == 1)		     \
		{								    \
			if ( diagLoggerConfig.bEnabled )				\
			{								\
				DiagLoggerAddTraceToLog(diagTraceDesc->moduleId,	    \
							diagTraceDesc->reportId,						    \
							(UINT32)GET_TIMESTAMP,					    \
							TRUE, /* op code included*/							\
							/*OpCode */ *((UINT16*)data),			     \
							MIN(dataLen, diagLoggerConfig.nMaxDataPerTrace),  \
							data );						    \
			}								\
			if (!IS_DIAG_BLOCKED)						\
			{       /*------ MACRO END PART1-------------------------------M_END-*/
/*---PART2:     diagPrintf() or diagTextPrintf() or diagStructPrintf();     \
	    }                                                               \
	}                                                                   \
    } ----------- 3 brackets------------------------------------------M_END-*/

#endif // defined (ENABLE_DIAG_LOGGER)

#endif // NO_MACRO_ANSI_C99__VA_ARGS__



#define DIAG_SET_FILTER( C1, C2, C3, isSet)	    \
	{										\
		DIAG_FILTER_EXTERN(C1, C2, C3);			\
		UINT32 cpsr;				    \
		diag_lock(&cpsr);					    \
		g_OverridePassFilter    =   1;		    \
		/* we are going to change filter, dont care about current filter etc */	\
		/* calling the trace, just to get the modID reportID */					\
		(__trace_ ## C1 ## __ ## C2 ## __ ## C3());			  \
		{					    \
			UINT16 index;							    \
			UINT16 id = diagTraceDesc->reportId;	  \
			/* the trace API is always working (lock & set of diagTraceDesc) - since g_OverridePassFilter is set to 1. We must unlock now */	\
			diag_unlock_G();						\
			index = PASS_FILTER_BYTE(id);		    \
			if (isSet == TRUE) {			 \
				filterArray[index] = filterArray[index] & (~(0x1 << ((UINT8)(PASS_FILTER_BIT(id))))); \
			}					\
			else {					\
				filterArray[index] = filterArray[index] | (0x1 << ((UINT8)(PASS_FILTER_BIT(id)))); \
			}					\
		}					    \
		g_OverridePassFilter    =   0;		    \
		diag_unlock(cpsr);			   \
	}
/*-----------------12/26/2005 14:26-----------------
 * Monitor supporting macros
 * --------------------------------------------------*/

#define MONITOR_CONNECT(Type, UniqueName, Address, IsModifiable, FuncPtr, SubCat)    \
	{										\
		DIAG_EXPORT int __MON_ ## UniqueName(void);				      \
		int paramID;								    \
		paramID = __MON_ ## UniqueName();					      \
		MonitorConnect(paramID, Address, IsModifiable, FuncPtr);		    \
	}

#define MONITOR_DISCONNECT(UniqueName)	     \
	{					     \
		DIAG_EXPORT int __MON_ ## UniqueName(void); \
		int paramID;				 \
		paramID =   __MON_ ## UniqueName();	   \
		MonitorDisconnect(paramID);		 \
	}

/************************
*		MACROs			*
************************/

#if !defined (DIAGNEWHEADER)
#define TRACE_SAP TRACE
#else
#define TRACE_SAP SAP_TRACE_RESPONSE
#endif /* DIAGNEWHEADER */
#define DIAGM_TRACE( CAT1, CAT2, CAT3, TEXT)						\
	{																		\
		if ( !IS_DIAG_BLOCKED)						    \
		{								    \
			DIAG_FILTER_EXTERN(CAT1, CAT2, CAT3);					    \
			ModuleMessageID modMsgID;									       \
			TxPDU *buf;														\
			UINT32 numOfDecreasedBytes;										\
			if (__trace_ ## CAT1 ## __ ## CAT2 ## __ ## CAT3() == 1)		   \
			{																\
				modMsgID.moduleID = diagTraceDesc->moduleId;				\
				modMsgID.messageID = diagTraceDesc->reportId;				\
				diag_unlock_G();											\
				buf = diagAlignAndHandleErr(0, modMsgID.messageID, &numOfDecreasedBytes, TRACE_SAP);	\
				if (buf)															\
				{																	\
					diagSendPDU(TRACE_SAP, modMsgID.moduleID, modMsgID.messageID, buf, 0, numOfDecreasedBytes );	  \
				}																\
			}																	\
		}																		\
	}


#define DIAGM_TRACE_1(CAT1, CAT2, CAT3, FORMAT, P1)									  \
	{																			\
		if ( !IS_DIAG_BLOCKED)						    \
		{								    \
			DIAG_FILTER_EXTERN(CAT1, CAT2, CAT3);					    \
			ModuleMessageID modMsgID;									       \
			TxPDU *buf;														\
			UINT32 numOfDecreasedBytes;										\
			if (__trace_ ## CAT1 ## __ ## CAT2 ## __ ## CAT3() == 1)		   \
			{																\
				modMsgID.moduleID = diagTraceDesc->moduleId;				\
				modMsgID.messageID = diagTraceDesc->reportId;				\
				diag_unlock_G();											\
				buf = diagAlignAndHandleErr(sizeof(UINT16), modMsgID.messageID, &numOfDecreasedBytes, TRACE_SAP);	\
				if (buf)															\
				{																	\
					UINT8 *p = buf->data - numOfDecreasedBytes;						  \
					*(p++)   = (UINT8)(  (P1) & 0x00FF     );						  \
					*p       = (UINT8)( ((P1) & 0xFF00) >> 8 );						    \
					diagSendPDU(TRACE_SAP, modMsgID.moduleID, modMsgID.messageID, buf, sizeof(UINT16), numOfDecreasedBytes); \
				}																	\
			}																	\
		}																		\
	}


#define DIAGM_TRACE_2(CAT1, CAT2, CAT3, FORMAT, P1, P2)									   \
	{																			\
		if ( !IS_DIAG_BLOCKED)						    \
		{								    \
			DIAG_FILTER_EXTERN(CAT1, CAT2, CAT3);					    \
			ModuleMessageID modMsgID;									       \
			TxPDU *buf;														\
			UINT32 numOfDecreasedBytes;										\
			if (__trace_ ## CAT1 ## __ ## CAT2 ## __ ## CAT3() == 1)		   \
			{																\
				modMsgID.moduleID = diagTraceDesc->moduleId;				\
				modMsgID.messageID = diagTraceDesc->reportId;				\
				diag_unlock_G();											\
				buf = diagAlignAndHandleErr(sizeof(UINT16) << 1, modMsgID.messageID, &numOfDecreasedBytes, TRACE_SAP);	   \
				if (buf)															\
				{																	\
					UINT8 *p = buf->data - numOfDecreasedBytes;						  \
					*(p++)   = (UINT8)(  (P1) & 0x00FF     );						  \
					*(p++)   = (UINT8)( ((P1) & 0xFF00) >> 8 );						    \
					*(p++)   = (UINT8)(  (P2) & 0x00FF     );						  \
					*p       = (UINT8)( ((P2) & 0xFF00) >> 8 );						    \
					diagSendPDU(TRACE_SAP, modMsgID.moduleID, modMsgID.messageID, buf, sizeof(UINT16) << 1, numOfDecreasedBytes); \
				}																	\
			}																	\
		}																		\
	}

#define DIAGM_TRACE_3(CAT1, CAT2, CAT3, FORMAT, P1, P2, P3)								   \
	{																			\
		if ( !IS_DIAG_BLOCKED)						    \
		{								    \
			DIAG_FILTER_EXTERN(CAT1, CAT2, CAT3);					    \
			ModuleMessageID modMsgID;									       \
			TxPDU *buf;														\
			UINT32 numOfDecreasedBytes;										\
			if (__trace_ ## CAT1 ## __ ## CAT2 ## __ ## CAT3() == 1)		   \
			{																\
				modMsgID.moduleID = diagTraceDesc->moduleId;				\
				modMsgID.messageID = diagTraceDesc->reportId;				\
				diag_unlock_G();											\
				buf = diagAlignAndHandleErr(3 * sizeof(UINT16), modMsgID.messageID, &numOfDecreasedBytes, TRACE_SAP);	   \
				if (buf)														\
				{																\
					UINT8 *p = buf->data - numOfDecreasedBytes;					  \
					*(p++)   = (UINT8)(  (P1) & 0x00FF     );					  \
					*(p++)   = (UINT8)( ((P1) & 0xFF00) >> 8 );					    \
					*(p++)   = (UINT8)(  (P2) & 0x00FF     );					  \
					*(p++)   = (UINT8)( ((P2) & 0xFF00) >> 8 );					    \
					*(p++)   = (UINT8)(  (P3) & 0x00FF     );					  \
					*p       = (UINT8)( ((P3) & 0xFF00) >> 8 );					    \
					diagSendPDU(TRACE_SAP, modMsgID.moduleID, modMsgID.messageID, buf, 3 * sizeof(UINT16), numOfDecreasedBytes);  \
				}																\
			}																	\
		}																		\
	}


#define DIAGM_TRACE_1S(CAT1, CAT2, CAT3, FORMAT, S1)												  \
	{																						\
		if ( !IS_DIAG_BLOCKED)						    \
		{								    \
			DIAG_FILTER_EXTERN(CAT1, CAT2, CAT3);					    \
			ModuleMessageID modMsgID;									       \
			TxPDU *buf;														\
			UINT32 numOfDecreasedBytes;										\
			if (__trace_ ## CAT1 ## __ ## CAT2 ## __ ## CAT3() == 1)		   \
			{																\
				modMsgID.moduleID = diagTraceDesc->moduleId;				\
				modMsgID.messageID = diagTraceDesc->reportId;				\
				diag_unlock_G();											\
				buf = diagAlignAndHandleErr((strlen(S1) + 1), modMsgID.messageID, &numOfDecreasedBytes, TRACE_SAP);			  \
				if (buf)																\
				{																		\
					strcpy((char *)(buf->data - numOfDecreasedBytes), (char *)(S1));	   \
					diagSendPDU(TRACE_SAP, modMsgID.moduleID, modMsgID.messageID, buf, (strlen(S1) + 1), numOfDecreasedBytes);   \
				}																		\
			}																			\
		}																				\
	}


#define DIAGM_TRACE_BUF(CAT1, CAT2, CAT3, FORMAT, B, LENGTH)									\
	{																						\
		if ( !IS_DIAG_BLOCKED)						    \
		{								    \
			DIAG_FILTER_EXTERN(CAT1, CAT2, CAT3);					    \
			ModuleMessageID modMsgID;									       \
			TxPDU *buf;														\
			UINT32 numOfDecreasedBytes;										\
			if (__trace_ ## CAT1 ## __ ## CAT2 ## __ ## CAT3() == 1)		   \
			{																\
				modMsgID.moduleID = diagTraceDesc->moduleId;				\
				modMsgID.messageID = diagTraceDesc->reportId;				\
				diag_unlock_G();											\
				buf = diagAlignAndHandleErr((LENGTH)+2, modMsgID.messageID, &numOfDecreasedBytes, TRACE_SAP);		\
				if (buf)																	\
				{																			\
					*((UINT8 *)(&(buf->data[0]) - numOfDecreasedBytes)) = (UINT8)(  (LENGTH) & 0x00FF     ); \
					*((UINT8 *)(&(buf->data[1]) - numOfDecreasedBytes)) = (UINT8)( ((LENGTH) & 0xFF00) >> 8 ); \
					memcpy(&(buf->data[2]) - numOfDecreasedBytes, (UINT8 *)(B), LENGTH);		  \
					diagSendPDU(TRACE_SAP, modMsgID.moduleID, modMsgID.messageID, buf, (LENGTH)+2, numOfDecreasedBytes);	\
				}																			\
			}																				\
		}																					\
	}

#define DIAGM_TRACE_STRUCT(CAT1, CAT2, CAT3, FORMAT, S)						\
	{									    \
		if ( !IS_DIAG_BLOCKED)						    \
		{								    \
			DIAG_FILTER_EXTERN(CAT1, CAT2, CAT3);					    \
			ModuleMessageID modMsgID;									       \
			TxPDU *buf;														\
			UINT32 numOfDecreasedBytes;										\
			if (__trace_ ## CAT1 ## __ ## CAT2 ## __ ## CAT3() == 1)		   \
			{																\
				modMsgID.moduleID = diagTraceDesc->moduleId;				\
				modMsgID.messageID = diagTraceDesc->reportId;				\
				diag_unlock_G();											\
				buf = diagAlignAndHandleErr( sizeof(S), modMsgID.messageID, &numOfDecreasedBytes, TRACE_SAP);			\
				if (buf)													\
				{							    \
					memcpy(&(buf->data[0]) - numOfDecreasedBytes, &(S), sizeof(S) );								\
					diagSendPDU(TRACE_SAP, modMsgID.moduleID, modMsgID.messageID, buf, sizeof(S), numOfDecreasedBytes);	  \
				}							     \
			}								 \
		}																	 \
	}

//DIAGM_PRINT_TEXT - left for backword compatibility. Used in embedded SW in some places
#define     DIAGM_PRINT_TEXT(S1) diagmPrintf( 0, S1 )


#endif  //#if !defined (_CEPPC_) && !defined (_PPC_)

#else  /* NODIAG */
// EMPTY IMPLEMENTATION FOR NODIAG VARIANT
#include "diag_types.h"
//#include "diag_config.h"
//#include "diag_pdu.h"
//#include "diag_tx.h"
#include "diag_osif.h"

#if !defined (NO_MACRO_ANSI_C99__VA_ARGS__)
 #define diagPrintf(...)
#else
void diagPrintf(const char *fmt, ...);
#endif
 #define diagTextPrintf(text)
 #define diagStructPrintf(text, ptr, nbytes)
 #define diagStructPrintfPrealloc(text, ptr, nbytes, ptrdata)
 #define DIAG_FILTER(CAT1, CAT2, CAT3, reportLevel)
 #define DIAG_DSP_FILTER(CAT1, CAT2, CAT3, reportLevel, data, dataLen)
 #define DIAG_SET_FILTER( C1, C2, C3, isSet)
 #define MONITOR_CONNECT(Type, UniqueName, Address, IsModifiable, FuncPtr, SubCat) {}
 #define MONITOR_DISCONNECT(UniqueName)                                       {}
 #define DIAGM_TRACE(C1, C2, C3, TEXT)
 #define DIAGM_TRACE_1(C1, C2, C3, FORMAT, P1)
 #define DIAGM_TRACE_2(C1, C2, C3, FORMAT, P1, P2)
 #define DIAGM_TRACE_3(C1, C2, C3, FORMAT, P1, P2, P3)
 #define DIAGM_TRACE_1S(C1, C2, C3, FORMAT, S1)
 #define DIAGM_TRACE_BUF(C1, C2, C3, FORMAT, B, LENGTH)
 #define DIAGM_PRINT_TEXT(S1) diagmPrintf( 0, S1 )
 #define DIAGM_TRACE_STRUCT(C1, C2, C3, FORMAT, S1)
 #define DIAGM_TRACE_STRUCT(C1, C2, C3, FORMAT, S1)
 #define DIAG_MAX_PRINTF_DATA_SIZE       2048 /* Not reduced: used in SAC as MAX_SAC_TRACE_STR_LEN with certain min value asumptions */
 #define isDiagBlocked() 0
#endif /* NODIAG */

#endif  //DIAG_API_H
