/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************
 *
 *  COPYRIGHT (C) 2005 Intel Corporation.
 *
 *  This software as well as the software described in it is furnished under
 *  license and may only be used or copied in accordance with the terms of the
 *  license. The information in this file is furnished for informational use
 *  only, is subject to change without notice, and should not be construed as
 *  a commitment by Intel Corporation. Intel Corporation assumes no
 *  responsibility or liability for any errors or inaccuracies that may appear
 *  in this document or any software that may be provided in association with
 *  this document.
 *  Except as permitted by such license, no part of this document may be
 *  reproduced, stored in a retrieval system, or transmitted in any form or by
 *  any means without the express written consent of Intel Corporation.
 *
 *  FILENAME:   data_channel.h
 *  PURPOSE:    data_channel API used by stub
 *
 *****************************************************************************/

#ifndef __DATA_CHANNEL_H__
#define __DATA_CHANNEL_H__

#include "linux_types.h"
#include "osa.h"
#include "msldl.h"
#include "msl_sal_dr_type.h"

typedef struct
{
	UINT32 ceUserContext;
	UINT32 cei;
	OSAMsgQRef msgQ;
}CIDATASTUB_CONN_CONF_PARM;

typedef struct
{
	UINT32 cei;
	OSAMsgQRef msgQ;
}CIDATASTUB_ACT_CONF_PARM;


/* TODO */
typedef struct
{
	/* Local connect point information */
	UINT32 localSapi;
	UINT32 localCei;

	/* Remote connect point information */
	UINT32 remoteSapi;

	/* Node information */
	UINT8 localNode;
	UINT8 remoteNode;
} DataChannelConnectPointInfo;

/* TODO */
typedef struct
{
	/*	Contains node, sapi & cei for local and remote point*/
	DataChannelConnectPointInfo connectPointInfo;
	/*	Channel configuration information */
	MslConnectionConfigure channelConf;
	/* Channel mode */
	UINT32 mode;
	/* Endian type */
	MslServiceFormat serverFormat;
} DataChannelInfo;
/**
 * @brief MslDataChanReadyCallbackFunc: type definition of callback function
 *  to indicate data channel is ready. (Stub will implement it)
 *
 * @param BOOL      The flag indicates whether data channel is ready. \n
 *        UINT32    The data channel cei.
 *
 * @return void
 *
 */
typedef void (*MslDataChanReadyCallbackFunc)(BOOL, UINT32);

/**
 * @ brief MslDataChanRxCallbackFunc: type definition of callback function to
 * response to data indication from a data channel, which is called by DL.
 *
 * @see MSLDL_DATA_CALLBACK
 *
 * @param UINT32    The primitive indicates the type of callback: send confirm or
 *                  receive indication.
 *        PMslCallbackContext   The parameters associated with the primitive.
 *
 * @return MSL_STATUS   The status of this operation.
 *
 */
typedef MSL_STATUS (*MslDataChanRxCallbackFunc)(UINT32, PMslCallbackContext);

/**
 * setupDataChannel
 *
 * @brief data channel setup api used by Client.
 *
 * Global function
 *
 * @param StbcConnectionInfo*   The connection information for this channel.\n
 *        MslDataChanRxCallbackFunc The data receiving indication callback
 *                                  function pointer.\n
 *        MslDataChanReadyCallbackFunc  The data channel ready callback
 *                                      function pointer.
 *
 * @return MSL_STATUS   The status of this operation.
 *
 */
MSL_STATUS setupDataChannel(
	DataChannelInfo  *p_connectInfo,
	MslDataChanRxCallbackFunc rxCallbackFunc,
	MslDataChanReadyCallbackFunc readyCallbackFunc);

/**
 * setupDataChannelResponse
 *
 * @brief data channel setup response api used by server after receiving data
 *          channel setup indication.
 *
 * Global function
 *
 * @param StbcConnectionInfo*   The connection information for this channel.\n
 *        MslDataChanRxCallbackFunc The data receiving indication callback
 *                                  function pointer.\n
 *        MslDataChanReadyCallbackFunc  The data channel ready callback
 *                                      function pointer.
 *
 * @return MSL_STATUS   The status of this operation.
 *
 */
MSL_STATUS setupDataChannelResponse(
	DataChannelInfo     *p_connectInfo,
	MslDataChanRxCallbackFunc rxCallbackFunc,
	MslDataChanReadyCallbackFunc readyCallbackFunc);

/**
 * dataChannelRelease
 *
 * @brief data channel release api used by Client.
 *
 * Global function
 *
 * @param UINT32    The cei.\n
 *        OSAMsgQRef    The message queue reference of this data channel.
 *
 * @return MSL_STATUS   The status of this operation.
 *
 */
MSL_STATUS dataChannelRelease(
	UINT32 cei,
	OSAMsgQRef msgQ);

/**
 * dataChanConnect
 *
 * @brief Response to data channel connection confirmation
 *        or indication.
 *
 * Global function
 *
 * @param UINT32        The cei.\n
 *        UINT32        The user context \n
 *        OSAMsgQRef    The message queue reference of this data channel.
 *
 * @return MSL_STATUS   The status of this operation.
 *
 */
MSL_STATUS dataChanConnect(
	UINT32 cei,
	UINT32 userContext,
	OSAMsgQRef msgQ);

/**
 * dataChanActivate
 *
 * @brief Response to data channel activation confirmation
 *        or indication.
 *
 * Global function
 *
 * @param UINT32        The cei.\n
 *        OSAMsgQRef    The message queue reference of this data channel.
 *
 * @return MSL_STATUS   The status of this operation.
 *
 */
MSL_STATUS dataChanActivate(
	UINT32 cei,
	OSAMsgQRef msgQ);

/**
 * dataChanFindMsgQBySapi
 *
 * @brief Search the queue associated with given sapi in data channel message
 *        queue list.
 *
 * Global function.
 *
 * @param UINT32   The sapi to be seached.
 *
 * @return OSAMsgQRef The found message queue reference or NULL.
 *
 */
OSAMsgQRef dataChanFindMsgQBySapi(UINT32 sapi);

/**
 * dataChanFindMsgQByContext
 *
 * @brief Search the queue associated with given user context in data channel
 *        message queue list.
 *
 * Global function.
 *
 * @param UINT32   The user context to be seached.
 *
 * @return OSAMsgQRef The found message queue reference or NULL.
 *
 */
OSAMsgQRef dataChanFindMsgQByContext(UINT32 userContext);

/**
 * dataChanFindMsgQByCei
 *
 * @brief Search the queue associated with given cei in data channel message
 *        queue list.
 *
 * Global function.
 *
 * @param UINT32   The cei to be seached.
 *
 * @return OSAMsgQRef The found message queue reference or NULL.
 *
 */
OSAMsgQRef dataChanFindMsgQByCei(UINT32 cei);

/**
 * dataChanFindContextByCei
 *
 * @brief Search the context associated with given cei in data channel message
 *        queue list.
 *
 * Global function.
 *
 * @param UINT32   The cei to be seached.
 *
 * @return UINT32 The context or NULL.
 *
 */
UINT32 dataChanFindContextByCei(UINT32 cei);

/**
 * dataChanRemoveFromMsgQList
 *
 * @brief Remove the message queue from data channel message queue list.
 *
 * @param OSAMsgQRef   The message queue reference to be removed.
 *
 * @return MSL_STATUS   The status of this operation.
 *
 */
MSL_STATUS dataChanRemoveFromMsgQList(OSAMsgQRef msgQ);

/**
 * dataChanRemoveAllFromMsgQList
 *
 * @brief Remove all teh message queues from data channel message queue list.
 *
 * @param none.
 *
 * @return MSL_STATUS   The status of this operation.
 *
 */
MSL_STATUS dataChanRemoveAllFromMsgQList(void);

/**
 *  mdrDirect32Bits
 *
 *  @brief Do marshalling/un-marshalling of a 32-bit variable directly.
 *
 *  @param MslDrStream  The configration of this MDR opearation.
 *         UINT32*      The pointer to the data to be marshalled/un-marshalled
 *
 *  @return BOOL Indication whether this operation is successful.
 */
//BOOL mdrDirect32Bits(MslDrStream *MslDrs, UINT32 *p_data);

/**
 *  mdrDirect16Bits
 *
 *  @brief Do marshalling/un-marshalling of a 16-bit variable directly.
 *
 *  @param MslDrStream  The configration of this MDR opearation.
 *         UINT16*      The pointer to the data to be marshalled/un-marshalled
 *
 *  @return BOOL Indication whether this operation is successful.
 */
//BOOL mdrDirect16Bits(MslDrStream *MslDrs, UINT16 *p_data);

#endif /* __DATA_CHANNEL_H__ */

