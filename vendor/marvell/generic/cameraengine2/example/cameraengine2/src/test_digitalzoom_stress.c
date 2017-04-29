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

extern int iSensorID;

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

int _test_digitalzoom_stress( CAM_DeviceHandle handle )
{
	CAM_Error              error;
	CAM_Size               stPreviewSize;
	CAM_CameraCapability   stCameraCap;
	CAM_ShotModeCapability stShotModeCap;
	CAM_ShotMode           eShotMode;
	
	int i = 0, j = 0, m = 0;
	int loop = 10, frame_num = 0;

	TRACE( "Pls set the preview size you wanna do stress in style as example( eg. 640 x 480 ): \n" );
	scanf( "%d x %d", &stPreviewSize.iWidth, &stPreviewSize.iHeight );
	
	TRACE( "Pls set loop number( >= 5 ): \n" );
	scanf( "%d", &loop );
	if ( loop < 5 )
	{
		TRACE( "error loop number\n" );
		return -1;
	}

	TRACE( "Pls set frames displayed between each digital zoom interval: \n" );
	scanf( "%d", &frame_num );
	if ( frame_num < 1 )
	{
		TRACE( "error loop number, must >= 1\n" );
		return -1;
	}

	// register event handler to Camera Engine
	error = CAM_SendCommand( handle, CAM_CMD_SET_EVENTHANDLER, (CAM_Param)eventhandler, (CAM_Param)handle );
	ASSERT_CAM_ERROR( error );

	error = CAM_SendCommand( handle, CAM_CMD_QUERY_CAMERA_CAP, (CAM_Param)iSensorID, (CAM_Param)&stCameraCap );
	ASSERT_CAM_ERROR( error );

	error = CAM_SendCommand( handle, CAM_CMD_PORT_SET_SIZE, (CAM_Param)CAM_PORT_PREVIEW, (CAM_Param)&stPreviewSize );
	ASSERT_CAM_ERROR( error );
	
	StartPreview( handle );

	error = CAM_SendCommand( handle, CAM_CMD_GET_SHOTMODE, (CAM_Param)&eShotMode, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	error = CAM_SendCommand( handle, CAM_CMD_QUERY_SHOTMODE_CAP, (CAM_Param)eShotMode, (CAM_Param)&stShotModeCap );
	ASSERT_CAM_ERROR( error );

	for ( i = 0; i < loop; i++ )
	{
		TRACE( "**********round: %d****************\n", i );
		for ( j = 0; j <= ( stShotModeCap.iMaxDigZoomQ16 - stShotModeCap.iMinDigZoomQ16 ) / stShotModeCap.iDigZoomStepQ16; j++ )
		{
			CAM_Int32s iDigitalZoomQ16 = 0;

			for ( m = 0; m < frame_num; m++ )
			{
				error = RunOneFrame( handle );
				ASSERT_CAM_ERROR( error );
			}

			iDigitalZoomQ16 = MAX( stShotModeCap.iMinDigZoomQ16 + stShotModeCap.iDigZoomStepQ16 * j, stShotModeCap.iMaxDigZoomQ16 );

			error = CAM_SendCommand( handle, CAM_CMD_SET_DIGZOOM, (CAM_Param)iDigitalZoomQ16, UNUSED_PARAM );
			ASSERT_CAM_ERROR( error );

			TRACE( "current digital zoom: %.2f\n", iDigitalZoomQ16 / 65536.0f );
		}
	}

	TRACE( "**************************************************************\n" );
	TRACE( "digital zoom stress test finished success\n" );
	TRACE( "test intensity: %d times\n", loop );
	TRACE( "test resolution: %d x %d \n", stPreviewSize.iWidth, stPreviewSize.iHeight );
	TRACE( "digital zoom range( %.2f - %.2f ), step: %.2f \n", stShotModeCap.iMinDigZoomQ16 / 65536.0f, stShotModeCap.iMaxDigZoomQ16 / 65536.0f, stShotModeCap.iDigZoomStepQ16 / 65536.0f );
	TRACE( "**************************************************************\n" );

	return 0;
}

int test_digitalzoom_stress( CAM_DeviceHandle handle )
{
	int val;

	
	/* change STDIN settings to allow non-blocking read */
	/*if ((val=fcntl(STDIN_FILENO, F_GETFL, 0)) < 0)
	{
		TRACE("get_fl failed to get FL\n");
		return -1;
	}
	if (fcntl(STDIN_FILENO, F_SETFL, val | O_NONBLOCK) < 0)
	{
		TRACE("set_fl failed to set FL\n");
		return -1;
	}
	*/

	val = _test_digitalzoom_stress( handle );

	
	/* restore STDIN settings */
	/*if (fcntl(STDIN_FILENO, F_SETFL, val) < 0)
	{
		TRACE("set_fl failed to set FL\n");
		return -1;
	}
	*/

	return val;
}