/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
 *  INTEL CONFIDENTIAL
 *  Copyright 2006 Intel Corporation All Rights Reserved.
 *  The source code contained or described herein and all documents related to the source code (“Material? are owned
 *  by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
 *  its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
 *  Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
 *  treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual property right is granted to or
 *  conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
 *  estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
 *  Intel in writing.
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: teldef.h
 *
 *  Authors:  Vincent Yeung
 *
 *  Description: Macro used for telephony controller
 *
 *  History:
 *   May 19, 2006 - Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/

#ifndef TELDEF_H
#define TELDEF_H

#include "gbl_types.h"
#include "linux_types.h"
#include "stdio.h"
#include <utlAtParser.h>
#include <sys/time.h>
#include <signal.h>
#include "pxa_dbg.h"

#define RESULT_CODE_NULL utlFAILED
#define ResultCode_t utlReturnCode_T


/******************************************************************************
*   AT Parser Number
******************************************************************************/

typedef enum
{
	TEL_AT_CMD_ATP_0,
	TEL_AT_CMD_ATP_1,
	TEL_AT_CMD_ATP_2,
	TEL_AT_CMD_ATP_3,
	TEL_AT_CMD_ATP_4,
	TEL_AT_CMD_ATP_5,
	TEL_AT_CMD_ATP_6,
	TEL_AT_CMD_ATP_7,
	TEL_TEST_AT_CMD_ATP,    // used by serial_client
	TEL_AGPS_AT_CMD_ATP,    // used by A-GPS
	TEL_SIMAL_AT_CMD_ATP,	// used by simal
	TEL_ESP_AT_CMD_ATP,     //external serial port
	TEL_MODEM_AT_CMD_ATP,   // act as Modem when linked with PC
#ifdef AT_PRODUCTION_CMNDS
	TEL_PROD_AT_CMD_ATP,    // used for production mode, connect to UART device
#endif
	NUM_OF_TEL_ATP
}
TelAtParserID;

#define RIL_IND_MSG_CHANNEL TEL_AT_CMD_ATP_7

enum TelAtParserCmd
{
	TEL_ATP_CMD_EXIT_CHAN,
	TEL_ATP_CMD_TRIGGER_PARSER,
#ifdef AT_PRODUCTION_CMNDS
	TEL_ATP_CMD_EXIT_CHAN_AND_START_DIAG,
#endif
	TEL_ATP_CMD_NUM
};

typedef struct _TelAtpCtrl
{
	void *taskRef;
	int iFd;
	char *path;
	TelAtParserID index;
	BOOL bSmsDataEntryModeOn;
	BOOL bEnable;
	/* notify pipe, used to send command to AT parser channel */
	int iNotifyReadEnd; /* read end of notify pipe */
	int iNotifyWriteEnd; /* write end of notify pipe */
} TelAtpCtrl;

extern utlAtParser_P aParser_p[ NUM_OF_TEL_ATP ];
extern unsigned short gusXid;

extern char revisionId[128];

#define ATM_MANUFACTURER_ID "Marvell"
#define ATM_MODEL_ID "LINUX"
#define ATM_REVISION_ID "1.0"
#define ATM_SERIAL_NUMBER "12333"

#define ATM_ID "Marvell"
#define ATM_OBJ_ID      "OBJ ID"
#define ATM_COUNTRY_CODE        "(20,3C,3D,42,50,58,00,61,FE,A9,B4,B5)"


/******************************************************************************
*   OSA Task Priority
******************************************************************************/
#define ESP_TASK_PRIORITY 30
#define MODEM_TASK_PRIORITY 30
#define AT_TASK_PRIORITY 30
#define OSA_STACK_SIZE  1024

/******************************************************************************
*   Response (RESP) related Macro / inline functions
******************************************************************************/
extern int atRespStr(UINT32 reqHandle, UINT8 resultCode, UINT16 errCode, char *respString);

#define IND_REQ_HANDLE 1
#define INVALID_REQ_HANDLE 0xFFFFFFFF

#define ATRESP(REQHANDLE, RESULTCODE, ERRCODE, RESPSTRING) atRespStr( REQHANDLE, RESULTCODE, ERRCODE, RESPSTRING )

/******************************************************************************
*   AT Parser related Macro / inline functions
******************************************************************************/


/*--------- Making Compatible to Intel AT Parser--------____-----------------*/
#define RETURNCODE_T utlReturnCode_T

#define TEL_INVALID_EXT_CMD     utlAT_PARAMETER_OP_UNKNOWN
#define TEL_EXT_EXEC_CMD                utlAT_PARAMETER_OP_EXEC         //AT+CGMI
#define TEL_EXT_GET_CMD                 utlAT_PARAMETER_OP_GET          //?
#define TEL_EXT_SET_CMD                 utlAT_PARAMETER_OP_SET          //=
#define TEL_EXT_ACTION_CMD              utlAT_PARAMETER_OP_ACTION       //AT+CPAS
#define TEL_EXT_TEST_CMD                utlAT_PARAMETER_OP_SYNTAX       //=?


#define INITIAL_RETURN_CODE     utlFAILED

#define AT_RESV_HANDLE(sAtpIndex, xid) (UINT32)((sAtpIndex << 16) | xid)

// CiRequestHandle == UINT32
#define MAKE_CI_REQ_HANDLE( atHandle, reqId )				    \
	(UINT32)(														\
		(												      \
			( (UINT32)reqId << 20)   & 0x7FF00000		       \
		)												      \
		|												      \
		(												      \
			atHandle  & 0x000FFFFF					     \
		)												      \
		)

/* use highest bit to mark whether this request is origined from +CRSM cmd */
#define MAKE_CRSM_CI_REQ_HANDLE( atHandle, reqId )				\
	(UINT32)(														\
		(												      \
			( (UINT32)reqId << 20)   & 0x7FF00000		       \
		)												      \
		|  0x80000000 |										      \
		(												      \
			atHandle  & 0x000FFFFF					     \
		)												      \
		)

#define MAKE_AT_HANDLE( sAtpIndex )								\
	(UINT32)(														\
		(												      \
			( (UINT32)sAtpIndex << 16)   & 0x000F0000	       \
		)												      \
		|													      \
		(													      \
			( (gusXid == 0xffff)? gusXid=11 : ++gusXid)							\
		)													      \
		)


#define GET_ATP_INDEX( atHandle ) ( (atHandle >> 16 ) & 0xF )
#define GET_AT_HANDLE( reqHandle ) ( reqHandle & 0xFFFFF )
#define GET_REQ_ID( reqHandle ) ( (reqHandle >> 20 ) & 0x7FF )
#define GET_CRSM_FLAG( reqHandle ) ( (reqHandle >> 31) & 0x1 )

extern RETURNCODE_T HANDLE_RETURN_VALUE(UINT32 ret);

/******************************************************************************
*   Audio Related Macro
******************************************************************************/

extern void enable_voicecall_audio(void);
extern void disable_voicecall_audio(void);
extern int tel_set_voicecall_mute(INT32 mute);
extern INT32 tel_get_voicecall_mute(void);
extern void tel_init_voicecall_mute(void);

extern INT32 tel_set_loopback_state(INT32 method, INT32 device);
extern INT32 tel_get_loopback_state(INT32 * method, INT32 * device);

#define MACRO_START_VOICE_CALL() enable_voicecall_audio()

#define MACRO_END_VOICE_CALL() disable_voicecall_audio()

void initCCContext(void);
void initPSContext(void);

/* Some global variables */
extern BOOL bLocalTest;


extern const utlAtCommand_T * const pshell_commands;
extern const unsigned int shell_commands_num;
extern ATCMD_TimeOut atcmd_timeout_table[];
extern pthread_mutex_t gTimeoutTableLock;

#define MAX_TIMEOUT_NUM 5
extern int gTimeoutNum;
extern pthread_mutex_t gTimeoutLock;


#endif


/* END OF FILE */
