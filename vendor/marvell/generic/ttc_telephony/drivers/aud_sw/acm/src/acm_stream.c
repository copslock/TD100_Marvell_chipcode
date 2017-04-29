/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (?Material? are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel?s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************
*               MODULE IMPLEMENTATION FILE
*******************************************************************************
* Title: ACM (Audio Component Manager)
*
* Filename: acm.c
*
* Target, platform: Common Platform, SW platform
*
* Authors: Isar Ariel
*
* Description: Controls the h/w access of all audio devices.
*
* Last Updated:
*
* Notes:
******************************************************************************/
/*----------- External include files -----------------------------------------*/

/*----------- Local include files --------------------------------------------*/

#include <stdlib.h>
#include "acm.h"
#include "acmplatform.h"
#include "acm_stub.h"
#include "audio_stub_api_ttc.h"
#include <pxa_dbg.h>

/*----------- Local defines --------------------------------------------------*/
#define TOTAL_NUM_OF_DB_ENTRIES       (16)

/*----------- Local type definitions -----------------------------------------*/

typedef enum
{
	ACM_FALSE = 0,
	ACM_TRUE = 1
} ACM_BOOL;

typedef struct
{
	ACM_PacketTransferFunc packetTransferFunc;
} ACM_StreamCallBackDB;

typedef struct
{
	ACM_StreamCallBackDB streamDB[TOTAL_NUM_OF_DB_ENTRIES];
} ACM_StreamDataBase;


typedef enum
{
	ACM_DATA_BASE_NO_CHANGE = 0,
	ACM_DATA_BASE_CHANGE
} ACM_DataBaseChange;



/*----------- Global constant definitions ------------------------------------*/
static ACM_StreamDataBase _streamCallBackDB;


/*----------- Global extrenal variable definition -------------------------------------*/
extern const ACM_ComponentHandle* componentHandles[];

/*******************************************************************************
* Function: deleteEntryFromAcmDB
********************************************************************************
* Description: Delets entry from the data base of the in/out table.
*               If the table changes, the update audio mode function should be
*               called.
*
* Parameters: ACM_StreamHandle    streamHandle
*             ACM_PacketTransferFunc packetTransferFunc
*
* Return value: ACM_DataBaseChange
*
* Notes:
*******************************************************************************/
static int deleteEntryFromAcmDB(ACM_PacketTransferFunc packetTransferFunc)
{
	int entry;
	int ret = -1;
	ACM_StreamDataBase streamCallBackDB;

	streamCallBackDB = _streamCallBackDB;


	for (entry = 0; entry < TOTAL_NUM_OF_DB_ENTRIES; entry++)
	{
		if (streamCallBackDB.streamDB[entry].packetTransferFunc == packetTransferFunc)
		{       /* The right entry to delete was found */

			streamCallBackDB.streamDB[entry].packetTransferFunc = 0;
			ret = entry;
			break;  /* Stop running over the table entries */
		}
	}

	if (entry < TOTAL_NUM_OF_DB_ENTRIES)
	{       /* Entry to delete was found */
		while ( entry < (TOTAL_NUM_OF_DB_ENTRIES - 1) )
		{
			streamCallBackDB.streamDB[entry].packetTransferFunc = streamCallBackDB.streamDB[entry + 1].packetTransferFunc;

			streamCallBackDB.streamDB[entry + 1].packetTransferFunc = NULL;

			entry++;
		}

		/* restore data-base */
		_streamCallBackDB = streamCallBackDB;

	}

	return ret;
} /* End of deleteEntryFromAcmDB */


/*******************************************************************************
* Function: addEntryToAcmDB
********************************************************************************
* Description: Adds entry to the data base of the in/out table. If the call-back
*               function exists, it's parameters will be changed.
*               If the table changes, the update audio mode function should be
*               called.
*
* Parameters: ACM_StreamDirection
*             ACM_PacketTransferFunc packetTransferFunc
*
* Return value: ACM_StreamHandle
*
* Notes:
*******************************************************************************/
static int addEntryToAcmDB(ACM_PacketTransferFunc packetTransferFunc)
{
	int entry;
	int ret = -1;
	ACM_StreamDataBase streamCallBackDB;

	streamCallBackDB = _streamCallBackDB;

	for (entry = 0; entry < TOTAL_NUM_OF_DB_ENTRIES; entry++)
	{
		if ( (streamCallBackDB.streamDB[entry].packetTransferFunc == NULL) )
		{
			_streamCallBackDB.streamDB[entry].packetTransferFunc = packetTransferFunc;
			ret = entry;
			break;
		}
	}
	return ret;
} /* End of addEntryToAcmDB */


/*******************************************************************************
* Function: searchEntryInAcmDB
********************************************************************************
* Description: Delets entry from the data base of the in/out table.
*               If the table changes, the update audio mode function should be
*               called.
*
* Parameters: ACM_StreamHandle    streamHandle
*             ACM_PacketTransferFunc packetTransferFunc
*
* Return value: ACM_DataBaseChange
*
* Notes:
*******************************************************************************/
static int searchEntryInAcmDB( ACM_PacketTransferFunc packetTransferFunc)
{
	int entry;
	ACM_StreamDataBase streamCallBackDB;

	streamCallBackDB = _streamCallBackDB;

	int ret = -1;

	for (entry = 0; entry < TOTAL_NUM_OF_DB_ENTRIES; entry++)
	{
		if (streamCallBackDB.streamDB[entry].packetTransferFunc == packetTransferFunc)
		{               /* Found The right entry */
			ret = entry;
			break;  /* Stop running over the table entries */
		}
	}

	return ret;
}


/*******************************************************************************
* Function: ACMStreamInit
********************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
*******************************************************************************/
#if 0
void streamCallback(AudioStream  *audioStream)
{
	int i;
	int entry;
	int handle_num = audioStream->handle_num;

	ACM_StreamDataBase streamCallBackDB;

	streamCallBackDB = _streamCallBackDB;

	if (handle_num < 1)
		return;

	for (i = 0; i < handle_num; i++ )
	{
		entry = searchEntryInAcmDB(audioStream->stream_indicate[i].streamHandle);
		if (entry >= 0)
			streamCallBackDB.streamDB[entry].packetTransferFunc(audioStream->stream_indicate[i].data);
	}
}
#endif

void  ACMStreamInit(void)
{
	int entry;
	ACM_StreamDataBase streamCallBackDB;


	for (entry = 0; entry < TOTAL_NUM_OF_DB_ENTRIES; entry++)
	{
		streamCallBackDB.streamDB[entry].packetTransferFunc = NULL;
	}


	/* restore data-base */
	_streamCallBackDB = streamCallBackDB;

//	RegisterCallback((void *) streamCallback);
}


/*******************************************************************************
* Function: ACMAudioStreamInStop
********************************************************************************
* Description: Stops the stream-in process. It updates the stream-in data base,
*               and sets both Up-Link and Down-Link modes.
*
* Parameters: ACM_PacketTransferFunc packetTransferFunc
*
* Return value: ACM_ReturnCode
*
* Notes:
*******************************************************************************/
AudioStreamInStopReq streamInStopReq;
AudioStreamInStopRsp streamInStopRsp;
ACM_ReturnCode ACMAudioStreamInStop(ACM_PacketTransferFunc packetTransferFunc)
{

	int entry;

	DPRINTF("ACM: ACMAudioStreamInStop\n");
	if (packetTransferFunc == NULL)
		return ACM_RC_BUFFER_GET_FUNC_INVALID;

	entry = deleteEntryFromAcmDB(packetTransferFunc);

	if (entry >= 0 )
	{
		streamInStopReq.streamHandle = packetTransferFunc;
		DisableInStream(&streamInStopReq, &streamInStopRsp);
	}
	else
	{
		return ACM_RC_STREAM_IN_TO_BE_STOPPED_NOT_ACTIVE;
	}

	return ACM_RC_OK;
} /* End of ACMAudioStreamInStop */


/*******************************************************************************
* Function: ACMAudioStreamOutStop
********************************************************************************
* Description: Stops the stream-out process. It updates the stream-out data base,
*               and sets both Up-Link and Down-Link modes.
*
* Parameters: ACM_PacketTransferFunc packetTransferFunc
*
* Return value: ACM_ReturnCode
*
* Notes:
*******************************************************************************/
AudioStreamOutStopReq streamOutStopReq;
AudioStreamOutStopRsp streamOutStopRsp;
ACM_ReturnCode ACMAudioStreamOutStop(ACM_PacketTransferFunc packetTransferFunc)
{

	int entry;

	DPRINTF("ACM: ACMAudioStreamOutStop\n");
	if (packetTransferFunc == NULL)
		return ACM_RC_BUFFER_GET_FUNC_INVALID;

	entry = deleteEntryFromAcmDB(packetTransferFunc);

	if (entry >= 0)
	{
		streamOutStopReq.streamHandle = packetTransferFunc;
		DisableOutStream(&streamOutStopReq, &streamOutStopRsp);
	}
	else
	{
		return ACM_RC_STREAM_OUT_TO_BE_STOPPED_NOT_ACTIVE;
	}

	return ACM_RC_OK;
} /* End of ACMAudioStreamOutStop */


/*******************************************************************************
* Function: ACMAudioStreamInStart
********************************************************************************
* Description: Starts the stream-in process. It updates the stream-in data base,
*               and sets both Up-Link and Down-Link modes.
*               Under 20m HISR, packetTransferFunc will be called with valid pointer
*               for a buffer, and the buffer will return full.
*
* Parameters: ACM_StreamType         streamType
*             ACM_SrcDst             srcDst
*             ACM_PacketTransferFunc packetTransferFunc
*
* Return value: ACM_ReturnCode
*
* Notes: packetTransferFunc is the index field in the table. If it is already
*         present, change the entry data of this entry.
*******************************************************************************/
AudioStreamInStartReq streamInStartReq;
AudioStreamInStartRsp streamInStartRsp;

ACM_ReturnCode ACMAudioStreamInStart(ACM_StreamType streamType,
				     ACM_SrcDst srcDst,
				     ACM_PacketTransferFunc packetTransferFunc)
{
	int entry;

	DPRINTF("ACM: ACMAudioStreamInStart\n");
	if (packetTransferFunc == NULL)
	{
		DPRINTF("ACMAudioStreamInStart ERROR; packetTransferFunc = NULL");
		return ACM_RC_BUFFER_GET_FUNC_INVALID;
	}

	entry = addEntryToAcmDB(packetTransferFunc);

	if (entry >= 0)
	{
		streamInStartReq.streamHandle  = packetTransferFunc;
		streamInStartReq.streamType  = streamType;
		streamInStartReq.srcDst  = srcDst;
		EnableInStream(&streamInStartReq, &streamInStartRsp);
	}
	else
	{
		return ACM_RC_STREAM_IN_NOT_PERFORMED;
	}

	return ACM_RC_OK;
} /* End of ACMAudioStreamInStart */


/*******************************************************************************
* Function: ACMAudioStreamOutStart
********************************************************************************
* Description: Starts the stream-out process. It updates the stream-out data base,
*               and sets both Up-Link and Down-Link modes.
*               Under 20m HISR, packetTransferFunc will be called with valid pointer
*               for a buffer, and the buffer should be filled by the client inside
*               the function packetTransferFunc.
*
* Parameters: ACM_StreamType         streamType
*             ACM_SrcDst             srcDst
*             ACM_CombWithCall       combWithCall
*             ACM_PacketTransferFunc packetTransferFunc
*
* Return value: ACM_ReturnCode
*
* Notes: packetTransferFunc is the index field in the table. If it is already
*         present, change the entry data of this entry.
*******************************************************************************/
AudioStreamOutStartReq streamOutStartReq;
AudioStreamOutStartRsp streamOutStartRsp;

ACM_ReturnCode ACMAudioStreamOutStart(ACM_StreamType streamType,
				      ACM_SrcDst srcDst,
				      ACM_CombWithCall combWithCall,
				      ACM_PacketTransferFunc packetTransferFunc)
{
	int entry;

	DPRINTF("ACM: ACMAudioStreamOutStart, srcDst %d, combCall %d, func %p\n", srcDst, combWithCall, packetTransferFunc);

	if (packetTransferFunc == NULL)
	{
		DPRINTF("ACMAudioStreamOutStart ERROR; packetTransferFunc = NULL");

		return ACM_RC_BUFFER_GET_FUNC_INVALID;
	}

	entry = addEntryToAcmDB(packetTransferFunc);

	if (entry >= 0)
	{
		streamOutStartReq.streamHandle  = packetTransferFunc;
		streamOutStartReq.streamType  = streamType;
		streamOutStartReq.srcDst  = srcDst;
		streamOutStartReq.combWithCall  = combWithCall;
		EnableOutStream(&streamOutStartReq, &streamOutStartRsp);
	}
	else
	{
		return ACM_RC_STREAM_OUT_NOT_PERFORMED;
	}

	return ACM_RC_OK;
} /* End of ACMAudioStreamOutStart */

int AudioGetUpLinkStream(ACM_PacketTransferFunc packetTransferFunc, unsigned int *data, unsigned int dataSize)
{
	UNUSEDPARAM(dataSize)
	ACM_StreamDataBase streamCallBackDB;

	streamCallBackDB = _streamCallBackDB;
	int entry;

	//DPRINTF("AudioGetUpLinkStream\n");
	entry = searchEntryInAcmDB(packetTransferFunc);
	if (entry >= 0)
	{
		//DPRINTF("find and exec the packetTransferFunc\n");
		streamCallBackDB.streamDB[entry].packetTransferFunc(data);
	}

	return 0;
}

int AudioPutDownLinkStream(ACM_PacketTransferFunc packetTransferFunc, unsigned int *data, unsigned int dataSize)
{
	ACM_StreamDataBase streamCallBackDB;

	streamCallBackDB = _streamCallBackDB;
	int entry;
	DOWNLINKSTREAM_REQUEST *downlinkReq;

	//DPRINTF("AudioPutDownLinkStream\n");
	entry = searchEntryInAcmDB(packetTransferFunc);
	if (entry >= 0)
	{
		//DPRINTF("find and exec the packetTransferFunc\n");
		streamCallBackDB.streamDB[entry].packetTransferFunc(data);
	}
	else
	{
		return -1;
	}


	downlinkReq = malloc(sizeof(DOWNLINKSTREAM_REQUEST) + dataSize);
	if (downlinkReq == NULL )
	{
		DPRINTF("Failed to malloc mem for downlinkReq\n");
		return -2;
	}

	downlinkReq->handle_num = 1;
	downlinkReq->streamInd[0].streamHandle = packetTransferFunc;
	downlinkReq->streamInd[0].data = (UINT16 *)data;
	downlinkReq->streamInd[0].dataSize = (int)dataSize;

	StreamIndResp(downlinkReq);
	free(downlinkReq);

	return 0;
}
