/******************************************************************************
*(C) Copyright [2010 - 2011] Marvell International Ltd.
* All Rights Reserved
******************************************************************************/

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CameraEngine.h"
#include "test_harness.h"

static void eventhandler( CAM_EventId eEventId, void *param, void *pUserData )
{
	switch ( eEventId )
	{
	case CAM_EVENT_FRAME_DROPPED:
		TRACE( "Warning: port %d frame dropped, please enqueue buffer in time!\n", (CAM_Int32s)param );
		break;

	case CAM_EVENT_FRAME_READY:
		// Ingore this notification, since in this application we uses blocking call method to get the buffer
		break;

	case CAM_EVENT_IN_FOCUS:
		TRACE( "Event: Focus Complete!\n" );
		break;

	case CAM_EVENT_STILL_SHUTTERCLOSED:
		break;

	case CAM_EVENT_STILL_ALLPROCESSED:
		break;

	default:
		TRACE( "Warning: not handled event!\n" );
		break;
	}
}

int test_dequeue_singleport( CAM_DeviceHandle handle )
{
	CAM_Error       error;
	int             i, port;
	CAM_ImageBuffer *pImgBuf;

	// register event handler to CameraEngine
	error = CAM_SendCommand( handle, CAM_CMD_SET_EVENTHANDLER, (CAM_Param)eventhandler, (CAM_Param)handle );
	ASSERT_CAM_ERROR( error );

	StartPreview( handle );	// enter the preview state
	StartRecording( handle );

	for ( i = 0; i < 20; i++ )
	{
		TRACE( "dequeue preview %d\n", i );
		port = CAM_PORT_PREVIEW;
		error = SafeDequeue( handle, &port, &pImgBuf );
		ASSERT_CAM_ERROR( error );

		TRACE( "queue preview %d\n", i );
		error = CAM_SendCommand( handle, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)port, (CAM_Param)pImgBuf );
		ASSERT_CAM_ERROR( error );
	}

	for ( i = 0; i < 20; i++ )
	{
		TRACE( "dequeue video %d\n", i );
		port = CAM_PORT_VIDEO;
		error = SafeDequeue( handle, &port, &pImgBuf );
		ASSERT_CAM_ERROR( error );

		TRACE( "queue video %d\n", i );
		error = CAM_SendCommand( handle, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)port, (CAM_Param)pImgBuf );
		ASSERT_CAM_ERROR( error );
	}

	StopRecording( handle );
	StopPreview( handle );

	TRACE( "============Report=================\n" );
	TRACE( "test:%s succeed\n", __FUNCTION__ );
	TRACE( "============EOR====================\n" );

	return 0;
}


int test_flush( CAM_DeviceHandle handle )
{
	CAM_Error error;
	int       i, j, port;

	// register event handler to CameraEngine
	error = CAM_SendCommand( handle, CAM_CMD_SET_EVENTHANDLER, (CAM_Param)eventhandler, (CAM_Param)handle );
	ASSERT_CAM_ERROR( error );

	StartPreview( handle );	// enter the preview state
	StartRecording( handle );

	for ( j = 0; j < 10; j++ )
	{
		error = CAM_SendCommand( handle, CAM_CMD_PORT_FLUSH_BUF, (CAM_Param)CAM_PORT_PREVIEW, UNUSED_PARAM );
		ASSERT_CAM_ERROR( error );

		for ( i = 0; i < stPreviewBufReq.iMinBufCount; i++ )
		{
			port = CAM_PORT_PREVIEW;
			error = CAM_SendCommand( handle, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)port, (CAM_Param)&pPreviewBuf[i] );
			ASSERT_CAM_ERROR( error );
		}

		error = CAM_SendCommand( handle, CAM_CMD_PORT_FLUSH_BUF, (CAM_Param)CAM_PORT_VIDEO, UNUSED_PARAM );
		ASSERT_CAM_ERROR( error );

		for ( i = 0; i < stVideoBufReq.iMinBufCount; i++ )
		{
			port = CAM_PORT_VIDEO;
			error = CAM_SendCommand( handle, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)port, (CAM_Param)&pVideoBuf[i] );
			ASSERT_CAM_ERROR( error );
		}
	}

	StopRecording( handle );
	StopPreview( handle );

	TRACE( "============Report=================\n" );
	TRACE( "test:%s succeed\n", __FUNCTION__ );
	TRACE( "============EOR====================\n" );
	
	return 0;
}


int test_state_transition( CAM_DeviceHandle handle )
{
	CAM_Error error;

	// register event handler to Camera Engine
	error = CAM_SendCommand( handle, CAM_CMD_SET_EVENTHANDLER, (CAM_Param)eventhandler, (CAM_Param)handle );
	ASSERT_CAM_ERROR( error );

	// idle -> preview
	StartPreview( handle );	// enter the preview state
	// preview->idle
	StopPreview( handle );

	// idle->preview
	StartPreview( handle );	// enter the preview state
	// preview->video
	StartRecording( handle );
	// video ->preview
	StopRecording( handle );

	// preview->video
	StartRecording( handle );
	// video ->preview
	StopRecording( handle );

	// preview->video
	StartRecording( handle );
	// video ->preview
	StopRecording( handle );

	StopPreview( handle );

	TRACE( "============Report=================\n" );
	TRACE( "test:%s succeed\n", __FUNCTION__ );
	TRACE( "============EOR====================\n" );
	
	return 0;
}

