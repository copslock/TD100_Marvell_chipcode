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

extern int      iSensorID;
extern CAM_Bool bSaveJPEG;

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

	case CAM_EVENT_FOCUS_AUTO_STOP:
		if ( (CAM_Int32s)param == 1 )
		{
			TRACE( "Event: Focus Success!\n" );
		}
		else
		{
			TRACE( "Event: Focus Fail!\n" );
		}
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


int _test_res_switch_stress( CAM_DeviceHandle handle )
{
	CAM_Error error;
	int i = 0, j = 0, m = 0, port = 0, loop = 10, k = 0;
	CAM_CameraCapability stCameraCap;
	int cnt = 1;

	TRACE( "Pls set the port you want to test:\n 0 - preview,\n 1 - video,\n 2 - still,\n -1 - all \n" );
	scanf( "%d", &port );
	if ( port >= 3 || port < -1 )
	{
		TRACE( "error port number\n" );
		return -1;
	}

	TRACE( "Pls set loop number( >= 5 ): \n" );
	scanf( "%d", &loop );
	if ( loop < 5 )
	{
		TRACE( "error loop number\n" );
		return -1;
	}

	error = CAM_SendCommand( handle, CAM_CMD_QUERY_CAMERA_CAP, (CAM_Param)iSensorID, (CAM_Param)&stCameraCap );
	ASSERT_CAM_ERROR( error );


	if ( port == -1 )
	{
		cnt = 3;
	}
	
	for ( k = 0; k < cnt; k++ )
	{
		if ( cnt == 3 )
		{
			port++;
		}
		if ( port == CAM_PORT_STILL )
		{
			StartPreview( handle );
			error = CAM_SendCommand( handle, CAM_CMD_PORT_SET_FORMAT, (CAM_Param)CAM_PORT_STILL, (CAM_Param)stCameraCap.stPortCapability[port].eSupportedFormat[0] );
			ASSERT_CAM_ERROR( error );
			bSaveJPEG = CAM_FALSE;
		}
		for( i = 0; i < loop; i++ )
		{

			for( j = 0; j < stCameraCap.stPortCapability[port].iSupportedSizeCnt; j++ )
			{
				int frame_num = 0;
				if ( stCameraCap.stPortCapability[port].stSupportedSize[j].iWidth == 2048 )
				{
					continue;
				}
				if ( (i + 1) % 5 == 0 )
				{
					TRACE( "====================================================================================\n" );
					TRACE(" \nTest on Loop %d...\n",i + 1 );
					TRACE(" \n Test format %d, Resolution= [%d X %d] on Port [%d] Loop [%d]\n",stCameraCap.stPortCapability[port].eSupportedFormat[0],
						     stCameraCap.stPortCapability[port].stSupportedSize[j].iWidth,
						     stCameraCap.stPortCapability[port].stSupportedSize[j].iHeight,port,i + 1 );
					TRACE( "====================================================================================\n" );
				}
				switch ( port )
				{
				case CAM_PORT_PREVIEW:
					error = CAM_SendCommand( handle, CAM_CMD_PORT_SET_SIZE, (CAM_Param)CAM_PORT_PREVIEW, (CAM_Param)&stCameraCap.stPortCapability[port].stSupportedSize[j] );
					ASSERT_CAM_ERROR( error );
					StartPreview( handle ); // enter the preview state
					frame_num = 5;
					break;
				case CAM_PORT_VIDEO:
					error = CAM_SendCommand( handle, CAM_CMD_PORT_SET_SIZE, (CAM_Param)CAM_PORT_PREVIEW, (CAM_Param)&stCameraCap.stPortCapability[port].stSupportedSize[j] );
					ASSERT_CAM_ERROR( error );
					StartPreview( handle );
					error = CAM_SendCommand( handle, CAM_CMD_PORT_SET_SIZE, (CAM_Param)CAM_PORT_VIDEO, (CAM_Param)&stCameraCap.stPortCapability[port].stSupportedSize[j] );
					ASSERT_CAM_ERROR( error );
					StartRecording( handle );
					frame_num = 5;
					break;
				case CAM_PORT_STILL: 
					error = CAM_SendCommand( handle, CAM_CMD_PORT_SET_SIZE, (CAM_Param)CAM_PORT_STILL, (CAM_Param)&stCameraCap.stPortCapability[port].stSupportedSize[j] );
					ASSERT_CAM_ERROR( error );
					TakePicture( handle );
					frame_num = 1;
					break;
				}
				for ( m = 0; m < frame_num; m++ )
				{
					error = RunOneFrame( handle );
					ASSERT_CAM_ERROR( error );
				}
				switch( port )
				{
				case CAM_PORT_PREVIEW:
					StopPreview( handle );
					break;
				case CAM_PORT_VIDEO:
					StopRecording( handle );
					StopPreview( handle );
					break;
				case CAM_PORT_STILL:
					error = CAM_SendCommand( handle, CAM_CMD_SET_STATE, (CAM_Param)CAM_CAPTURESTATE_PREVIEW, UNUSED_PARAM );
					ASSERT_CAM_ERROR( error );
					for ( m = 0; m < 30; m++ )
					{
						error = RunOneFrame( handle );
						ASSERT_CAM_ERROR( error );
					}
					break;
				}
			}
		}
	}
	TRACE( "**************************************************************\n" );
	TRACE( "Resolution switch test finished success\n" );
	TRACE( "Test Intensity:%d times\n", loop );
	TRACE( "**************************************************************\n" );
	bSaveJPEG = CAM_TRUE;

	return 0;
}


int test_res_switch_stress( CAM_DeviceHandle handle )
{
	int val;

	
	/* change STDIN settings to allow non-blocking read */
	/*if ( (val = fcntl( STDIN_FILENO, F_GETFL, 0 )) < 0 )
	{
		TRACE( "get_fl failed to get FL\n" );
		return -1;
	}
	if ( fcntl( STDIN_FILENO, F_SETFL, val | O_NONBLOCK ) < 0 )
	{
		TRACE( "set_fl failed to set FL\n" );
		return -1;
	}
	*/

	val = _test_res_switch_stress( handle );

	
	/* restore STDIN settings */
	/*if ( fcntl( STDIN_FILENO, F_SETFL, val ) < 0 )
	{
		TRACE( "set_fl failed to set FL\n" );
		return -1;
	}
	*/

	return val;
}