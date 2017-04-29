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


CAM_Bool   bCanReturnToPreview;		// used to decide if we can change back to preview state
extern int iSensorID;


static void eventhandler( CAM_EventId eEventId, void *param, void *pUserData )
{
	switch ( eEventId )
	{
	case CAM_EVENT_FRAME_DROPPED:
		// TRACE( "Warning: port %d frame dropped, please enqueue buffer in time!( %s, %s, %d )\n", (CAM_Int32s)param, __FILE__, __FUNCTION__, __LINE__ );
		break;

	case CAM_EVENT_FRAME_READY:
		// Ingore this notification, since in this application we uses blocking call method to get the buffer
		break;

	case CAM_EVENT_IN_FOCUS:
		TRACE( "Event: In Focus!\n" );
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
		bCanReturnToPreview = CAM_TRUE;
		break;

	default:
		TRACE( "Warning: not handled event[%d]!\n", eEventId );
		break;
	}
}


int _test_dv_dsc_actively_dequeue( CAM_DeviceHandle handle )
{
	CAM_CameraCapability stCameraCap;
	CAM_Error            error;

	// register event handler to Camera Engine
	error = CAM_SendCommand( handle, CAM_CMD_SET_EVENTHANDLER, (CAM_Param)eventhandler, (CAM_Param)handle );
	ASSERT_CAM_ERROR( error );

	error = CAM_SendCommand( handle, CAM_CMD_QUERY_CAMERA_CAP, (CAM_Param)iSensorID, (CAM_Param)&stCameraCap );
	ASSERT_CAM_ERROR( error );

	error = GetParametersAndDisplay( handle, &stCameraCap );
	ASSERT_CAM_ERROR( error );

	error = StartPreview( handle );	// enter the preview state
	ASSERT_CAM_ERROR( error );
	TRACE( "Start Preview\n" );

	for ( ; ; )
	{
		CAM_AppCmd       cmd;
		CameraParam      param;
		CAM_CaptureState eState;

		error = DequeueUserCmd( handle, &cmd, &param );
		ASSERT_CAM_ERROR( error );

		if ( cmd == CMD_EXIT )
		{
			break;
		}
		else if ( cmd != CMD_NULL )
		{
			switch ( cmd )
			{
			case CMD_PREPARECAPTURE:
				error = AutoFocus( handle, NULL );
				if ( error != CAM_ERROR_NONE )
				{
					TRACE( "do nothing for prepare capture" );
				}
				break;

			case CMD_STILLCAPTURE:
				bCanReturnToPreview = CAM_FALSE;
				error = TakePicture( handle );
				ASSERT_CAM_ERROR( error );
				break;

			case CMD_CANCELCAPTURE:
				error = CancelAutoFocus( handle );
				ASSERT_CAM_ERROR( error );
				break;

			case CMD_VIDEOONOFF:
				error = CAM_SendCommand( handle, CAM_CMD_GET_STATE, (CAM_Param)&eState, UNUSED_PARAM );
				ASSERT_CAM_ERROR( error );
				if ( eState == CAM_CAPTURESTATE_PREVIEW )
				{
					error = StartRecording( handle );
					ASSERT_CAM_ERROR( error );
				}
				else if ( eState == CAM_CAPTURESTATE_VIDEO )
				{
					error = StopRecording( handle );
					ASSERT_CAM_ERROR( error );
				}
				break;

			default:
				{
					error = SetParameters( handle, cmd, param );
					ASSERT_CAM_ERROR( error );
				}
				break;
			}

			error = GetParametersAndDisplay( handle, &stCameraCap );
			ASSERT_CAM_ERROR( error );
		}

		error = RunOneFrame( handle );
		if ( error == CAM_ERROR_NOTENOUGHBUFFERS )	// happen when video encoder didn't return the buffer to camera engine timely
		{
			;
		}
		else
		{
			ASSERT_CAM_ERROR( error );
		}

		if ( bCanReturnToPreview )
		{
			/* state change */
			error = CAM_SendCommand( handle, CAM_CMD_SET_STATE, (CAM_Param)CAM_CAPTURESTATE_PREVIEW, UNUSED_PARAM );
			ASSERT_CAM_ERROR( error );

			/* simulate Marvell Android camera-hal behavior */
			// error = CAM_SendCommand( handle, CAM_CMD_SET_STATE, (CAM_Param)CAM_CAPTURESTATE_IDLE, UNUSED_PARAM );
			// ASSERT_CAM_ERROR( error );
			// error = CAM_SendCommand( handle, CAM_CMD_SET_STATE, (CAM_Param)CAM_CAPTURESTATE_PREVIEW, UNUSED_PARAM );
			// ASSERT_CAM_ERROR( error );
			bCanReturnToPreview = CAM_FALSE;
		}
	}

	error = StopPreview( handle );
	ASSERT_CAM_ERROR( error );

	return 0;
}

int test_dv_dsc_actively_dequeue( CAM_DeviceHandle handle )
{
	int val;

	/* change STDIN settings to allow non-blocking read */
	if ( ( val = fcntl( STDIN_FILENO, F_GETFL, 0 ) ) < 0 )
	{
		TRACE( "get_fl failed to get FL\n" );
		return -1;
	}
	if ( fcntl( STDIN_FILENO, F_SETFL, val | O_NONBLOCK ) < 0 )
	{
		TRACE( "set_fl failed to set FL\n" );
		return -1;
	}

	/* start the camera process */
	val = _test_dv_dsc_actively_dequeue( handle );

	/* restore STDIN settings */
	if ( fcntl( STDIN_FILENO, F_SETFL, val ) < 0 )
	{
		TRACE( "set_fl failed to set FL\n" );
		return -1;
	}

	return val;
}

