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
*     File name:      diag_header_handler.h                                       *
*     Programmer:     Barak Witkowski                                             *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*       Create Date:  November, 2006                                              *
*                                                                                 *
*       Description:                                                                                                                       *
*                                                                                 *
*       Notes:                                                                    *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#if !defined (_DIAG_HEADER_EXTERNAL_H)
#define _DIAG_HEADER_EXTERNAL_H

/* definitions of fields sizes in the new header */
#define SIZE_OF_SOT_PATTERN                     2
#define SIZE_OF_TOTAL_LENGTH            2
#define SIZE_OF_HEADER_LENGTH           2
#define MAX_SIZE_OF_HEADER_LENGTH 512
#define SIZE_OF_HEADER_VERSION          2
#define SIZE_OF_SRC                                     1
#define SIZE_OF_DEST                            1
#define SIZE_OF_SAP                                     2
#define SIZE_OF_CHECKSUM                        2
#define SIZE_OF_EOT_PATTERN                     2
#define SIZE_OF_OPTIONS                         4       //The options field is 4 bytes size
#define SIZE_OF_OPTIONS_ONE_FIELD       4               //Each option data field is 4 bytes size (indicated by a single bit in the options field)
#define NUM_OF_OPTIONS_FIELDS           4               //The number of option data fields: time stamp, frame number message counter etc.

#include "diag_header_handler.h"
typedef void VOID;
/****************************************
*   Definiiton of Core2Core messages    *
****************************************/
/*       YKstat
	C2C_STATISTICS ( March 2008) - used for statistics of COMM
		on internal, and apps on ext apps catches the message and adds its
		statistics. Sends out as trace from COMM (comm send traceID in the message)
 */
#define C2C_STATISTICS                  1

/****************************************
*                                      *
*                       Function Declaration		*
*                                      *
****************************************/

VOID    setTxDiagTraceHeader(UINT8 *pHeader, UINT16 *pduLen, UINT16 type, UINT16 moduleID, UINT16 msgID);
VOID    CreateDiagErrMsgHeader(UINT8 *pHeader, UINT16 pduLen);
VOID    InsertCounterAndCheckSumToTxHeader(UINT8 *pHeader, UINT32 MsgCounter);
VOID    CreateConnectedDeviceQueryTxHeader(UINT8 *pHeader, UINT16 packetLen);
UINT16  GetMsgIDFromTxHeader(UINT8 *pHeader);
VOID    InsertTimeStampToTxHeader(UINT8 *pHeader);
//VOID	ChangeMsgPointerAndLengthDueToDiffInHeaders(UINT8 connectedDev, UINT8 **msg, UINT16 *len, Allocation_Type allocType);
// The 3rd parameter is for 'old header' only  - as the header differes on different interfaces.
DIAG_EXPORT INT8 GetMessageCoreID(UINT8* pData, UINT32 nLen, UINT32 bIsUSBheader);
extern void     SetMessageOtherCoreID(UINT8* pData, UINT32 myCoreID);
INT8    GetMessageCoreIDForIntIf(UINT8* pData);
UINT16  GetRxMsgDiagSap(UINT8 *rxMsg);
UINT16  GetRxMsgServiceID(UINT8 *rxMsg);
UINT16  GetRxMsgModuleID(UINT8 *rxMsg);
UINT32  GetRxMsgCommID(UINT8 *rxMsg);
UINT32  GetRxMsgSourceID(UINT8 *rxMsg);
UINT8   *GetRxMsgFunctionParams(UINT8 *rxMsg);
void    SetRxMsgDiagSap(UINT8 *rxMsg, UINT8 newDiagSap);
void    SetSapAndServiceIDofCmdToOtherCore(UINT8 *rxMsg, UINT8 diagInternalServiceType);
UINT32  GetUsbMsgLen(void* pHeader);
BOOL    IsICATReadyNotify(UINT8* pData);
UINT8   FindSoTOffset(void* pHeader);
BOOL    IsC2CStats(UINT8 *pData);

#if !defined (DIAGNEWHEADER)
VOID    setTxDiagSigHeader(void *pMsg, UINT16 *pduLen);
#endif /* DIAGNEWHEADER */

#if defined (DIAGNEWHEADER)
UINT16  CalcCheckSum(UINT16 *data, UINT8 numOf2BytesCalculated);
UINT16 GetMsgDataLength(void *msg);
#endif /* DIAGNEWHEADER */

VOID    diagCalcUpdateTxL2L4MessageLength(TxL2L4PDUHeader *header, UINT16 *messageLength, UINT32 *padBytes);

/* This function sets a new a pointer to frame number, which will be inserted
 * in the header counter field (instead of the regular count up) - 2 lower bytes
 * (LSBytes 0 and LSByte 1 of FN) and
 * if Full-Frame-number requested (by ICAT command), then the high byte of
 * the module ID will be set with the 3rd byte (LSByte 3) of the frame number
 */
VOID    SetDiagTxMsgFramNumberPtr(const UINT32* const NewFNPtr);

#endif /* _DIAG_HEADER_EXTERNAL_H */

