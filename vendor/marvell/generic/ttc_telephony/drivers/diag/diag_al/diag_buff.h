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
*     File name:      Diag_Buff .h                                                *
*     Programmer:     Shiri Dolev                                                 *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*       Create Date:  November, 2003                                              *
*                                                                                 *
*       Description: DIAG Buffering Definitions								   *
*                                                                                 *
*       Notes:                                                                    *
* Yaeli Karni June 2006	Add definitions for data collection of messages Tx/Rx  *
*                                       sizes. Target - to limit message size to 4000 bytes			   *
*                                                                                 *
* Yaeli Karni Jan 2008  Tx-Q dynamically allocated, size per Q. UPdate macros  *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if !defined (_DIAG_BUFF_H_)
#define _DIAG_BUFF_H_

#include "global_types.h"
////#include "diag_tx.h"
////#include "osa.h"

//For APPS+COMM platform the capability of APPS must be greater than COMM.
//So increase APPS-list-size vs COMM-list-size
#if defined (DIAG_APPS)
	#define DO_APPS_STATS
	#define DIAG_TX_MAX_ITEMS_ON_INTif                              (256)                   // internal interface (used mainly for commands - rather small)
	#define DIAG_TX_MAX_ITEMS_ON_CMIif                              (256)                   //???
	#define DIAG_MIN_FREE_ITEMS_ON_LIST                             20                      // on APPS with STATS we want to leave at least 20.

	#if defined (OSA_WINCE)                                                                 // DIAG APPS only - external Q should be big, internal queue small (only commands usually)
		#define DIAG_TX_MAX_ITEMS_ON_EXTif                      (256 * 4 * 6)           //10K000 items ( in SICAT (old RTR (diag) of WM) 8K)
	#elif defined (OSA_LINUX)
		#define DIAG_TX_MAX_ITEMS_ON_EXTif                      (256 * 4 * 6)           //external interface BIG,
	#else //RTOS
		#define DIAG_TX_MAX_ITEMS_ON_EXTif                      (256 * 4)               //external interface BIG,
	#endif
#else                                                                                           // DIAG COMM
	#define DO_COMM_STATS
	#define DIAG_MIN_FREE_ITEMS_ON_LIST                             3                       //COMM -  minimum of 3
	#define DIAG_TX_MAX_ITEMS_ON_EXTif                              (256)                   // external interface big
	#define DIAG_TX_MAX_ITEMS_ON_INTif                              (256 * 3)               // internal interface big
	#define DIAG_TX_QUICK_REPORTS_ON_INTif                  10
	#define INTIF_CYCLIC_BUFF
#endif //

#if (DIAG_TX_MAX_ITEMS_ON_EXTif > 0xFFF0)
	#error  Diag Queue size should not exceed 0xFFF0
#endif

#if (DIAG_TX_MAX_ITEMS_ON_INTif > 0xFFF0)
	#error  Diag Queue size should not exceed 0xFFF0
#endif

// suggested by Chen Reibach - March 2008 due to FT .. wait for 1/3 of the Queue to be empty before resuming..
#define DIAG_MIN_FREE_ITEMS_ON_LIST_AFTER_CONGESTION(dps)       (((dps)->diagNumOfItemsAllowedOnQ) / 3 > DIAG_MIN_FREE_ITEMS_ON_LIST  ? (((dps)->diagNumOfItemsAllowedOnQ) / 3 + 1) : DIAG_MIN_FREE_ITEMS_ON_LIST + 2)

#define LIST_EMPTY(dps) (((dps)->diagTotalNumOfItems) == 0)

#define DIAG_INC_LIST_ITEM(x, dps)   ((x) =							\
					      ((((x) + 1) < ((dps)->diagNumOfItemsAllowedOnQ)) ?     \
					       ((x) + 1) :							     \
					       ((x) + 1 - ((dps)->diagNumOfItemsAllowedOnQ)))	     \
					      )

//NOT used #define LIST_FULL(dps)	(((dps)->diagTotalNumOfItems) >= (((dps)->diagNumOfItemsAllowedOnQ) - 2))

#define LIST_FREEENTRIES(dps)   (((dps)->diagNumOfItemsAllowedOnQ) - ((dps)->diagTotalNumOfItems))

// Lost Diag messages definitions:
// Queu full (ext, INT-if MUST be same size of text !!
#define DIAG_ERR_TEXT_EXT_QUEUE_IS_FULL                 "Diag Error message: Diag messages are dropped as result of overflow in the DiagEXT queue."
#define DIAG_ERR_TEXT_INT_QUEUE_IS_FULL                 "Diag Error message: Diag messages are dropped as result of overflow in the DiagINT queue."
#define DIAG_ERR_TEXT_MEMORY_ALLOC_FAILED       "Diag Error message: Diag messages are dropped as result of memory allocation failure."
// save 10 places for number digits	- all messages same size
#define DIAG_ERR_TEXT_TOTAL_LOST_MSGS_MEM               "Diag Error message: Total number of lost Diag messages due to: MEM is:                  ."
#define DIAG_ERR_TEXT_TOTAL_LOST_MSGS_EXT               "Diag Error message: Total number of lost Diag messages due to: EXT is:                  ."
#define DIAG_ERR_TEXT_TOTAL_LOST_MSGS_INT               "Diag Error message: Total number of lost Diag messages due to: INT is:                  ."
// save 12 places for len and msgID
#define DIAG_ERR_TEXT_MSG_TOO_LONG                          "Diag Error message: Message TOO long (msgID-size):                  ."
#define DIAG_ERR_TEXT_MSG_WARN_LONG                         "Diag Warning message:   Message long (msgID-size):                  ."

// 13 digits and '\n'
#define DIAG_ERR_TEST_TOTAL_LOST_NUMBER_LOCATION (sizeof(DIAG_ERR_TEXT_TOTAL_LOST_MSGS_MEM) - 16)
// 13 digits and '\n'
#define DIAG_ERR_MSG_TOO_LONG_NUMBER_LOCATION (sizeof(DIAG_ERR_TEXT_MSG_TOO_LONG) - 16)

#define DIAG_ERR_TEXT_QUEUE_IS_FULL_LEN         (TX_PDU_HEADER_SIZE + sizeof(DIAG_ERR_TEXT_EXT_QUEUE_IS_FULL) + TX_PDU_FOOTER_SIZE)
#define DIAG_ERR_TEXT_MEMORY_ALLOC_FAILED_LEN   (TX_PDU_HEADER_SIZE + sizeof(DIAG_ERR_TEXT_MEMORY_ALLOC_FAILED) + TX_PDU_FOOTER_SIZE)
#define DIAG_ERR_TEXT_TOTAL_LOST_MSGS_LEN               (TX_PDU_HEADER_SIZE + sizeof(DIAG_ERR_TEXT_TOTAL_LOST_MSGS_MEM) + TX_PDU_FOOTER_SIZE)
#define DIAG_ERR_TEXT_MSG_LONG_LEN                              (TX_PDU_HEADER_SIZE + sizeof(DIAG_ERR_TEXT_MSG_TOO_LONG) +  TX_PDU_FOOTER_SIZE)

// Diag error messages memory needs to be static (not dynamic):
DIAG_EXPORT UINT8 diagErrMsgBufQueueOverflow [];
DIAG_EXPORT UINT8 diagErrMsgBufQueueINTOverflow [];
DIAG_EXPORT UINT8 diagErrMsgBufMemoryAlloc [];
DIAG_EXPORT UINT8 diagErrMsgTotalLostMsgsMEM [];
DIAG_EXPORT UINT8 diagErrMsgTotalLostMsgsEXT [];
DIAG_EXPORT UINT8 diagErrMsgTotalLostMsgsINT [];
DIAG_EXPORT UINT8 diagErrMsgTooLong [];
DIAG_EXPORT UINT8 diagErrMsgWarningLong [];
// type of error condition
//ICAT EXPORTED ENUM
typedef enum {
	diagErrNoMemory,
	diagErrMemoryOK,
	diagErrExtQueueCongest,
	diagErrINTQueueCongest,
	diagErrCMIQueueCongest,
	diagErrExtQueueOK,
	diagErrINTQueueOK,
	diagErrCMIQueueOK,
	diagErrMSGmaxLen,
	diagErrMsgwarningLen
} diagErrStatesE;


typedef struct
{
	UINT8  *virtualReportPtr;
	UINT16 reportLength;
	//UINT16 /*Allocation_Type*/ allocType;	//Set for UINT16 to promise same assignement for all OS
	UINT8 allocType;
	UINT8 clientID; //TBDIY casted from COMDEV_NAME and valid only in CMI mode. For future extend to a general SendMsg function to all interfaces.
}DiagReportItem_S;

// =========================================================================================================================
// Manipulation of the queue variables (which are globals) MUST BE DONE under disable interrupts, since there are tightly
// connected.
// The enqueue (sendPDU()) action manipulates the NextFreeItem pointer (the tail of the queue which points to the
// next free entry in the queue) and the diagTotalNumOfItems (which is the total number of physical items in list, including
// those who were transmitted but not released yet).
// This action is called from many different tasks outside the Diag scope.
//
// The Tx task of each physical connection  use the FirstItemOnList and update the NextItemToSend (which points to the item which
// is the next one to be send on the physical connection) pointers,	and the diagNumOfTxedItems (which is the number of items
// that were already sent but were not released yet) as it dequues items and perform the actual send.
// As part of the removal of items from list, the FirstItemOnList, the diagTotalNumOfItems and diagNumOfTxedItemsis are
// updated for each item that was released.
// These actions must be synchronized with other actions that try to enqueu messages, or reset the queue toatlly,
// THUS all manipouation of the queue pointers (FirstItemOnList, NextFreeItem & NextItemToSend) and items' counters
// (the diagTotalNumOfItems and diagNumOfTxedItemsis) must be protected and atomic (when test and set actions are done).
// =========================================================================================================================
typedef struct
{
	// The head of the queue, from which items are dequeued for transmission and afterwards being removed.
	UINT16 diagFirstItemOnList;

	// The tail of the queue, to which new messages are enqueueud.
	UINT16 diagNextFreeItemOnList;

	// Points to the next message in the queue that needs to be dequeued and transmitted. This item will be equal to the
	// FirstItemOnList before transmission and will change as result of the transmission (while the FirstItemOnList will stiil point
	// the the first item that was transmitted but not released yet). Both indeces will be equal again after the remove of the
	// transmitted items.
	UINT16 NextItemToSend;

	// The number of items currently in the queue (including those who were transmitted but not released yet.
	UINT16 diagTotalNumOfItems;

	// The number of items that were transmitted in the last cycle of transmission, before the release.
	UINT16 diagNumOfTxedItems;
	// total number of items in queue - to allow different Queues with different numbers
	UINT16 diagNumOfItemsAllowedOnQ;
}DiagQPtrData_S;

// A structureand data info of transmitted data from COMM to APPS to ICAT.
// It is sent out any X bytes of traces are produced on the COMM
// It is intended to collect data on the traces path COMM-via-APPS-to-ICAT (not genral
// collection on all paths). Data collected as described in the structure items.
// the driving of data is the COMM collection when number of produced bytes-of-traces e
// exceeded the X (). The APPS collection is done when the message arrives at it over the
// internal interface (thus a letency of MSL/SHM transmit till 'APPS period' statistics are
// added.
// Collection is done on COMM, sent ot APP as C2C message (SAP: CORE_TO_CORE_MESSAGE, service: C2C_STATISTICS). At APPS, it will add
// its data, preper the message as COMM messge (trace, with message ID of the correct C123
// (sent from COMM in the AP_bytes_recieved_from_CP field).

#define CP_TRACE_BYTES_FOR_STAT_PERIOD  500000
#define MULT_FOR_AVERAGE_TIME_CALC              32768           // 2^15 - a 'shiftable' value

//ICAT EXPORTED STRUCT
typedef struct {
	UINT32 CP_TimeStamp;                    // time stamp in COMM when X bytes (or more) were produced
	UINT32 CP_bytes_produced;               // number of bytes produced in traces in the last period
	UINT32 CP_bytes_dropped;                // number of bytes produced but discarded (due to congestion) in the last period
	UINT32 CP_bytes_sent;                   // number of bytes sent out on the internal channel in the last period
	UINT32 CP_bytes_sent_tot_time;          // total time to send all bytes in the period (each send has start/end TS, delta is added to this counter)
	UINT32 CP_max_time_byteOint;            // max time to send a byte over internal interface
	UINT32 AP_TimeStamp;                    // time stamp in APPS when the message arrived over internal interface and processed
	UINT32 AP_bytes_produced;               // number of bytes produced in traces (on APPS or coming from COMM) in the last period
	UINT32 AP_bytes_dropped;                // number of bytes produced but discarded (due to congestion) in the last period
	UINT32 AP_bytes_sent;                   // number of bytes sent out on the external channel in the last period
	UINT32 AP_bytes_sent_tot_time;          // total time to send all bytes in the period (each send has start/end TS, delta is added to this counter)
	UINT32 AP_max_time_byteOext;            // max time to send a byte over external interface
	UINT32 AP_bytes_recieved_from_CP;       // total time to send all bytes in the period (each send has start/end TS, delta is added to this counter)
	// Fields added after first diag release of stats (rel 04.28) - not exist in all versions!! must be checked for.
	UINT16 CP_struct_size;                  // size of the statistics structure on CP side (data allows for backward/forward compatibility)
	UINT16 AP_struct_size;                  // size of the statistics structure on AP side (data allows for backward/forward compatibility)
	UINT32 CP_bytes_added_INTif;            // bytes added for sending over INT if
	UINT32 AP_bytes_removed_INTif;          // bytes recieved from CP and removed (used for IntIF protocol only)
	UINT32 AP_bytes_added_ExtIf;            // bytes added on external interface
} DiagStats_CP_AP_S;

typedef struct {
	UINT32 CP_trace_bytes_for_statistics_period;
	UINT32 CP_trace_statistics;
	UINT32 Tx_Start_TS;
	UINT32 bytes_in_this_tx;
	UINT32 bytes_INTifOnly_in_this_tx;
} DiagStats_Internal_S;

extern DiagStats_CP_AP_S d_stat_CP_AP;
extern DiagStats_Internal_S d_stat_internal;

/* static declaraion...
   extern DiagReportItem_S	_diagExtIfReportsList[];
   extern DiagQPtrData_S	_diagExtIfQPtrData;     // additional pointers can be maintained by DiagComm
 #if defined (DIAG_INT_IF)
   extern DiagReportItem_S	_diagIntIfReportsList [];
   extern DiagQPtrData_S	_diagIntIfQPtrData;
 #endif
 */
// Q of Tx on interface is dyanmically allocated with different Q size per interface
extern DiagReportItem_S *_diagExtIfReportsList;
extern DiagQPtrData_S   *_diagExtIfQPtrData;     // additional pointers can be maintained by DiagComm

//CMI support
extern DiagReportItem_S * _diagCMIfReportsList;
extern DiagQPtrData_S   * _diagCMIfQPtrData;

//this is used for the internal interface and the data structure is allocated only if the internal interface is working
extern DiagReportItem_S *_diagIntIfReportsList;
extern DiagQPtrData_S   *_diagIntIfQPtrData;
#if defined (DO_COMM_STATS)     // only on COMM
// special quick send Q
extern DiagReportItem_S _diagIntIfQQReportsList[DIAG_TX_QUICK_REPORTS_ON_INTif];
extern DiagQPtrData_S _diagIntIfQQPtrData;
#define TRACE_STAT_SIZE (((sizeof(DiagStats_CP_AP_S) + TX_PDU_HEADER_SIZE + TX_PDU_FOOTER_SIZE + 3) / 4) * 4)
extern UINT8 _diagIntIfQQTraceBuffer[DIAG_TX_QUICK_REPORTS_ON_INTif][TRACE_STAT_SIZE];
#endif

//ICAT EXPORTED STRUCT
typedef struct {
	UINT32 diagMaxMsgOutLimit;
	UINT32 diagMaxMsgOutWarning;
	UINT32 diagMaxMsgInLimit;
	UINT32 diagMaxMsgInWarning;
} diagMsgLimitSet_S;

typedef struct {
	UINT32 _diagNumberOfTracesExtQLost;
	UINT32 _diagNumberOfTracesIntQLost;
	UINT32 _diagNumberOfTracesCMIQLost;

	UINT32 _diagNumberOfTracesMEMLost;

	UINT32 diagNumberOfTracesExtQLostTotal;
	UINT32 diagNumberOfTracesExtQLostBytes;
	UINT32 diagNumberOfTracesIntQLostTotal;
	UINT32 diagNumberOfTracesIntQLostBytes;
	UINT32 diagNumberOfTracesCMIQLostTotal;
	UINT32 diagNumberOfTracesCMIQLostBytes;

	UINT32 diagNumberOfTracesMEMLostTotal;
	UINT32 diagNumberOfTracesMEMLostBytes;

	UINT32 diagNumberOfTraceRxTaskLostTotal;
	UINT32 diagNumberOfTraceRxTaskLostBytes;

	UINT32 diagMsgsLost_MemAlloc;

	UINT32 diagMsgsLost_ExtIfQueueIsFull;
	UINT32 diagMsgsLost_IntIfQueueIsFull;
	UINT32 diagMsgsLost_CMIifQueueIsFull;   // for CMI interface

	diagMsgLimitSet_S MsgLimits;
	/*
	   UINT32 diagMaxMsgOutLimit;
	   UINT32 diagMaxMsgOutWarning;
	   UINT32 diagMaxMsgInLimit;
	   UINT32 diagMaxMsgInWarning;
	 */
} _diagIntData_S;
DIAG_EXPORT _diagIntData_S _diagInternalData;

extern UINT32 flagKickExtTx;
extern UINT32 flagKickIntTx;
extern UINT32 flagKickCMITx; //TBDIY CMI support

// Public service invoked by all DiagTX layers
// These functions are re-entrable - Check for space in the Tx-Q (for Ext-if) and add final data (counter,checksum)
//extern void diagBufferPDUExtIf(UINT8 *sendData, UINT16 pduLength, Allocation_Type allocType, UINT32 traceFromLocalCore);
//extern void diagBufferPDUIntIf(UINT8 *sendData, UINT16 pduLength, Allocation_Type allocType, UINT32 traceFromLocalCore);
//extern void diagBufferPDUCMIf (UINT8 *sendData, UINT16 pduLength, Allocation_Type allocType, const UINT8 clientID /*TBDIY no traces via CMI clients - only commands, UINT32 traceFromLocalCore*/);

// This function sets a new Tx message counter pointer, which will not be
// counting up by the diag, and will be sent to ICAT instead of the usual counter.
void SetDiagTxMsgCounterPtr(UINT16* NewCounterPtr);
extern void diagSendErrMsg(UINT32 connectedDev, diagErrStatesE err_state, UINT32 num_lost /* zero - problem started*/);
extern void diagSendErrLenMsg(UINT32 connectedDev, diagErrStatesE err_state, UINT32 length, UINT32 msgID );

// Function to allocated buffer for message + handle error condition
// on allocation failure
//extern void DiagReleaseBufferMemory (Allocation_Type allocType, UINT8* pMem);

extern void updateStatsOnApps(UINT8 *bufData, UINT32 len);

#endif      // _DIAG_BUFF_H_

