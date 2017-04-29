/******************************************************************************
*(C) Copyright [2010 - 2011] Marvell International Ltd.
* All Rights Reserved
******************************************************************************/

#include "CameraEngine.h"
#include "test_harness.h"


#define TOTAL_FRAME_COUNT  100


CAM_Size stPreviewSize[] = 
{
	{320, 240},  // QVGA
	{480, 320},  // HVGA
	{640, 480},  // VGA
	{720, 480},  // D1
	{800, 480},  // WVGA
	{1024, 768}, // XGA
};

extern int iSensorID;
extern int iPreviewBufNum;

static void eventhandler( CAM_EventId eEventId, void *param, void *pUserData )
{
	CAM_Error        error    = CAM_ERROR_NONE;
	CAM_DeviceHandle handle   = (CAM_DeviceHandle)pUserData;
	CAM_ImageBuffer  *pImgBuf = NULL;
	CAM_CaptureState eState;

	switch ( eEventId )
	{
	case CAM_EVENT_FRAME_DROPPED:
		break;

	case CAM_EVENT_FRAME_READY:
		break;

	case CAM_EVENT_IN_FOCUS:
		TRACE( "Event: In Focus!\n" );
		break;

	case CAM_EVENT_FOCUS_AUTO_STOP:
		break;

	case CAM_EVENT_STILL_SHUTTERCLOSED:
		break;

	case CAM_EVENT_STILL_ALLPROCESSED:
		break;

	case CAM_EVENT_FATALERROR:
		error = RestartCameraEngine( handle );
		ASSERT_CAM_ERROR( error );
		break;

	default:
		TRACE( "Warning: not handled event!\n" );
		break;
	}
}

int test_video_fps( CAM_DeviceHandle handle )
{
	CAM_Error          error = CAM_ERROR_NONE;
	int                i, size_index, j, port;
	CAM_PortCapability *pPortCap = NULL;
	CAM_ImageBuffer    *pImgBuf = NULL;
	volatile CAM_Tick  t = 0, t1 = 0;
	float              fDigZoom = 1.0f, fTargetFps;

	CAM_CameraCapability stCameraCap;

	double fps_p_p[10] = {0};
	double fps_v_p[10][10];
	double fps_v_v[10] = {0};

	TRACE( "Please input the buffer number you want to enqueue into preview port( >= 3 && <= 10 ):\n" );
	scanf( "%d", &iPreviewBufNum );
	if ( iPreviewBufNum < 3 )
	{
	    TRACE( "error preview number\n" );
	    return -1;
	}

	TRACE( "Please input the digital zoom you want to set:\n" );
	scanf( "%f", &fDigZoom );
	if ( fDigZoom < 1.0f )
	{
		TRACE( "error zoom ratio\n" );
		return -1;
	}

	TRACE( "Please input the fps ( >= 10 && <= 30 ) you want to set:\n" );
	scanf( "%f", &fTargetFps );
	if ( fTargetFps < 10.0f || fTargetFps > 30.0f )
	{
		TRACE( "error fps\n" );
		return -1;
	}

	// register event handler to Camera Engine
	error = CAM_SendCommand( handle, CAM_CMD_SET_EVENTHANDLER, (CAM_Param)eventhandler, (CAM_Param)handle );
	ASSERT_CAM_ERROR( error );

	error = CAM_SendCommand( handle, CAM_CMD_QUERY_CAMERA_CAP, (CAM_Param)iSensorID, (CAM_Param)&stCameraCap );
	ASSERT_CAM_ERROR( error );
	
	pPortCap = &stCameraCap.stPortCapability[CAM_PORT_VIDEO];

	// Preview state preview port fps
	for ( size_index = 0; size_index < pPortCap->iSupportedSizeCnt; size_index++ )
	{
		if ( pPortCap->stSupportedSize[size_index].iWidth == 2048 )
		{
			fps_p_p[size_index] = -1.0;
			continue;
		}

		// try preview size
		error = CAM_SendCommand( handle, CAM_CMD_PORT_SET_SIZE, (CAM_Param)CAM_PORT_PREVIEW, (CAM_Param)&pPortCap->stSupportedSize[size_index] );
		if ( error == CAM_ERROR_NOTSUPPORTEDARG )
		{
			fps_p_p[size_index] = -1.0;
			TRACE( "Target preview size (%d x %d) is not supported!\n", pPortCap->stSupportedSize[size_index].iWidth, pPortCap->stSupportedSize[size_index].iHeight );
			continue;
		}
		ASSERT_CAM_ERROR( error );

		error = CAM_SendCommand( handle, CAM_CMD_SET_DIGZOOM, (int)( fDigZoom * 65536 + 0.5f ), NULL );
		ASSERT_CAM_ERROR( error );

		error = CAM_SendCommand( handle, CAM_CMD_SET_PREVIEW_FRAMERATE, (int)( fTargetFps *65536 + 0.5f ), NULL );
		ASSERT_CAM_ERROR( error );

		StartPreview( handle );	// enter the preview state
		for ( i = 0; i < 10; i++ )
		{
			port = CAM_PORT_PREVIEW;
			error = SafeDequeue( handle, &port, &pImgBuf );
			ASSERT_CAM_ERROR( error );

			error = CAM_SendCommand( handle, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)port, (CAM_Param)pImgBuf );
			ASSERT_CAM_ERROR( error );
		}

		t = -IPP_TimeGetTickCount();

		for ( i = 0; i < TOTAL_FRAME_COUNT; i++ )
		{
#if 0
			// CAM_Tick t1,t2,t3;
			// t1 = IPP_TimeGetTickCount();
			port = CAM_PORT_PREVIEW;
			error = SafeDequeue( handle, &port, &pImgBuf );
			ASSERT_CAM_ERROR( error );
            
			// t2 = IPP_TimeGetTickCount();
			error = CAM_SendCommand( handle, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)port, (CAM_Param)pImgBuf );
			ASSERT_CAM_ERROR( error );
			
			// t3 = IPP_TimeGetTickCount();

			// TRACE( "preview dequeue time:%.2f ms, enqueue time:%.2f ms\n", (double)(t2-t1) / 1000, (double)(t3-t2) / 1000 );
#else
			// include display in fps test
			error = RunOneFrame( handle );
			ASSERT_CAM_ERROR(error);
#endif

		}

		t += IPP_TimeGetTickCount();

		fps_p_p[size_index] = TOTAL_FRAME_COUNT * 1000000.0 / (unsigned int)t;

		// TRACE( "Resolution: %d * %d, fps: %f \n",pCap->stSupportedSize[size_index].iWidth, pCap->stSupportedSize[size_index].iHeight,fps_p_p[size_index] );

		StopPreview( handle );
	}

	TRACE( "Preview state preview port fps( buffer number: %d ):\n",iPreviewBufNum );
	for ( size_index = 0; size_index < pPortCap->iSupportedSizeCnt; size_index++ )
	{
		TRACE( "\n\tPreview %d x %d:\t%.2f\n",pPortCap->stSupportedSize[size_index].iWidth, pPortCap->stSupportedSize[size_index].iHeight,fps_p_p[size_index] );
	}


	// Video state preview port fps
	for ( size_index = 0; size_index < pPortCap->iSupportedSizeCnt; size_index++ )
	{

		for ( j = 0; j < pPortCap->iSupportedSizeCnt; j++ )
		{

			if ( pPortCap->stSupportedSize[size_index].iWidth == 2048 || pPortCap->stSupportedSize[j].iWidth == 2048 )
			{
				fps_v_p[size_index][j] = -1.0;
				continue;
			}

			// try preview size
			error = CAM_SendCommand( handle, CAM_CMD_PORT_SET_SIZE, (CAM_Param)CAM_PORT_PREVIEW, (CAM_Param)&pPortCap->stSupportedSize[j] );
			if ( error == CAM_ERROR_NOTSUPPORTEDARG )
			{
				fps_v_p[size_index][j] = -1.0;
				continue;
			}
			ASSERT_CAM_ERROR( error );

			// try video size
			error = CAM_SendCommand( handle, CAM_CMD_PORT_SET_SIZE, (CAM_Param)CAM_PORT_VIDEO,(CAM_Param)&pPortCap->stSupportedSize[size_index] );
			if ( error == CAM_ERROR_NOTSUPPORTEDARG )
			{
				fps_v_p[size_index][j] = -1.0;
				//  TRACE( "Target video size (%d x %d) is not supported!\n", pCap->stSupportedSize[size_index].iWidth, pCap->stSupportedSize[size_index].iHeight );
				continue;
			}
			ASSERT_CAM_ERROR( error );

			StartPreview( handle );	// enter the preview state

			StartRecording( handle );

			for ( i = 0; i < 10; i++ )
			{
				port = CAM_PORT_PREVIEW;
				error = SafeDequeue( handle, &port, &pImgBuf );
				ASSERT_CAM_ERROR( error );

				error = CAM_SendCommand( handle, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)port, (CAM_Param)pImgBuf );
				ASSERT_CAM_ERROR( error );

				port = CAM_PORT_VIDEO;
				error = SafeDequeue( handle, &port, &pImgBuf );
				ASSERT_CAM_ERROR( error );

				error = CAM_SendCommand( handle, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)port, (CAM_Param)pImgBuf );
				ASSERT_CAM_ERROR( error );
			}

			t = -IPP_TimeGetTickCount();

			for ( i = 0; i < TOTAL_FRAME_COUNT; i++ )
			{
				port = CAM_PORT_PREVIEW;
				error = SafeDequeue( handle, &port, &pImgBuf );
				ASSERT_CAM_ERROR( error );

				error = CAM_SendCommand( handle, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)port, (CAM_Param)pImgBuf );
				ASSERT_CAM_ERROR( error );

				port = CAM_PORT_VIDEO;
				error = SafeDequeue( handle, &port, &pImgBuf );
				ASSERT_CAM_ERROR( error );

				error = CAM_SendCommand( handle, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)port, (CAM_Param)pImgBuf );
				ASSERT_CAM_ERROR( error );
			}

			t += IPP_TimeGetTickCount();

			fps_v_p[size_index][j] = TOTAL_FRAME_COUNT * 1000000.0 / (int)t;

			StopRecording( handle );

			StopPreview( handle );
		}
	}

	// Video state video port fps
	for ( size_index = 0; size_index < pPortCap->iSupportedSizeCnt; size_index++ )
	{

		if ( pPortCap->stSupportedSize[size_index].iWidth == 2048 )
		{
			fps_v_v[size_index] = -1.0;
			continue;
		}

		error = CAM_SendCommand( handle, CAM_CMD_PORT_SET_SIZE, (CAM_Param)CAM_PORT_PREVIEW, (CAM_Param)&pPortCap->stSupportedSize[size_index] );

		// start preview
		StartPreview( handle );	// enter the preview state

		error = CAM_SendCommand( handle, CAM_CMD_PORT_SET_SIZE, (CAM_Param)CAM_PORT_VIDEO,(CAM_Param)&pPortCap->stSupportedSize[size_index] );
		ASSERT_CAM_ERROR( error );

		StartRecording( handle );

		error = CAM_SendCommand( handle, CAM_CMD_PORT_FLUSH_BUF, (CAM_Param)CAM_PORT_PREVIEW, UNUSED_PARAM );
		ASSERT_CAM_ERROR( error );

		for ( i = 0; i < 10; i++ )
		{
			port = CAM_PORT_VIDEO;
			error = SafeDequeue( handle, &port, &pImgBuf );
			ASSERT_CAM_ERROR( error );

			error = CAM_SendCommand( handle, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)port, (CAM_Param)pImgBuf );
			ASSERT_CAM_ERROR( error );
		}

		t = -IPP_TimeGetTickCount();

		for ( i = 0; i < TOTAL_FRAME_COUNT; i++ )
		{
			CAM_Tick t1,t2,t3;
			t1 = IPP_TimeGetTickCount();
			port = CAM_PORT_VIDEO;
			error = SafeDequeue( handle, &port, &pImgBuf );
			ASSERT_CAM_ERROR(error);
			t2 = IPP_TimeGetTickCount();
			error = CAM_SendCommand( handle, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)port, (CAM_Param)pImgBuf );
			ASSERT_CAM_ERROR( error );
			t3 = IPP_TimeGetTickCount();
 			// TRACE( "video dequeue time:%.2f ms, enqueue time:%.2f ms\n", (double)(t2-t1) / 1000, (double)(t3-t2) / 1000 );
		}

		t += IPP_TimeGetTickCount();

		fps_v_v[size_index] = TOTAL_FRAME_COUNT * 1000000.0 / (int)t;

		StopRecording( handle );

		StopPreview( handle );
	}

	TRACE( "Preview state preview port fps( buffer number: %d ):\n",iPreviewBufNum );
	for ( size_index = 0; size_index < pPortCap->iSupportedSizeCnt; size_index++ )
	{
		TRACE( "\n\tPreview %d x %d:\t%.2f\n", pPortCap->stSupportedSize[size_index].iWidth, pPortCap->stSupportedSize[size_index].iHeight, fps_p_p[size_index] );
	}

	TRACE( "Video state preview port fps:\n" );
	for ( size_index = 0; size_index < pPortCap->iSupportedSizeCnt; size_index++ )
	{
		//j = size_index;
		for ( j = 0; j < pPortCap->iSupportedSizeCnt; j++ )
		{
			if ( fps_v_p[size_index][j] != -1 )
			{
				TRACE( "\n\tVideo (%d x %d), Preview (%d x %d): \t%.2f\n",
				       pPortCap->stSupportedSize[size_index].iWidth,
				       pPortCap->stSupportedSize[size_index].iHeight,
				       pPortCap->stSupportedSize[j].iWidth,
				       pPortCap->stSupportedSize[j].iHeight,
				       fps_v_p[size_index][j] );
			}
		}
	}

	TRACE( "Video state video port fps:\n" );
	for  ( size_index = 0; size_index < pPortCap->iSupportedSizeCnt; size_index++ )
	{
		TRACE( "\n\tVideo: %d x %d\t%.2f\n",
			    pPortCap->stSupportedSize[size_index].iWidth,
			    pPortCap->stSupportedSize[size_index].iHeight,
			    fps_v_v[size_index] );
	}

	return 0;
}

