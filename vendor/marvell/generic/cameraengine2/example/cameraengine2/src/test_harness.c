/******************************************************************************
*(C) Copyright [2010 - 2011] Marvell International Ltd.
* All Rights Reserved
******************************************************************************/

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_harness.h"
#include "test_harness_encoder.h"
#include "display.h"

#include "CameraEngine.h"
#include "misc.h"
#include "CamOSALBmm.h"

#include "ippdefs.h"
#include "ippIP.h"


#if defined( CAMERAENGINE_INTERNAL_PROCESSING_ENABLED )

// This section is for debug
#if defined( BONNELL )
CAM_FlipRotate  eSensorRotate          = CAM_FLIPROTATE_NORMAL;
CAM_ImageFormat eDefaultVideoFormat    = CAM_IMGFMT_CbYCrY;
CAM_ImageFormat eDefaultPreviewFormat  = CAM_IMGFMT_CbYCrY;
CAM_FlipRotate  eDefaultPreviewRotate  = CAM_FLIPROTATE_NORMAL;
CAM_Size        stDefaultPreviewSize   = {640, 480};
CAM_Int32s      iPreviewFpsQ16         = (30 << 16);
CAM_Int32s      iVideoFpsQ16           = (30 << 16);
#elif defined( BROWNSTONE ) || defined( G50 )
CAM_FlipRotate	eSensorRotate          = CAM_FLIPROTATE_NORMAL;
CAM_ImageFormat eDefaultVideoFormat    = CAM_IMGFMT_CbYCrY;
CAM_ImageFormat eDefaultPreviewFormat  = CAM_IMGFMT_BGR565;
CAM_FlipRotate	eDefaultPreviewRotate  = CAM_FLIPROTATE_NORMAL;
CAM_Size        stDefaultPreviewSize   = {SCREEN_WIDTH, SCREEN_HEIGHT};
CAM_Int32s      iPreviewFpsQ16         = (30 << 16);
CAM_Int32s      iVideoFpsQ16           = (30 << 16);
#elif defined ( ABILENE )
CAM_FlipRotate  eSensorRotate          = CAM_FLIPROTATE_NORMAL;
CAM_ImageFormat eDefaultVideoFormat    = CAM_IMGFMT_CbYCrY;
CAM_ImageFormat eDefaultPreviewFormat  = CAM_IMGFMT_BGR565;
CAM_FlipRotate  eDefaultPreviewRotate  = CAM_FLIPROTATE_NORMAL;
CAM_Size        stDefaultPreviewSize   = {SCREEN_WIDTH, SCREEN_HEIGHT};
CAM_Int32s      iPreviewFpsQ16         = (30 << 16);
CAM_Int32s      iVideoFpsQ16           = (30 << 16);
#elif defined( FLINT )
CAM_FlipRotate  eSensorRotate          = CAM_FLIPROTATE_NORMAL;
CAM_ImageFormat eDefaultVideoFormat    = CAM_IMGFMT_CbYCrY;
CAM_ImageFormat eDefaultPreviewFormat  = CAM_IMGFMT_CbYCrY;
CAM_FlipRotate  eDefaultPreviewRotate  = CAM_FLIPROTATE_NORMAL;
CAM_Size        stDefaultPreviewSize   = {SCREEN_WIDTH, SCREEN_HEIGHT};
CAM_Int32s      iPreviewFpsQ16         = (30 << 16);
CAM_Int32s      iVideoFpsQ16           = (30 << 16);
#elif defined( TTCDKB ) || defined( TDDKB )
CAM_FlipRotate  eSensorRotate          = CAM_FLIPROTATE_NORMAL;
CAM_ImageFormat eDefaultVideoFormat    = CAM_IMGFMT_YCbCr420P;
CAM_ImageFormat eDefaultPreviewFormat  = CAM_IMGFMT_YCbCr420P;
CAM_FlipRotate  eDefaultPreviewRotate  = CAM_FLIPROTATE_NORMAL;
CAM_Size        stDefaultPreviewSize   = {320, 240};
CAM_Int32s      iPreviewFpsQ16         = (30 << 16);
CAM_Int32s      iVideoFpsQ16           = (30 << 16);
#elif defined( PV2EVB )
CAM_FlipRotate  eSensorRotate          = CAM_FLIPROTATE_NORMAL;
CAM_ImageFormat eDefaultVideoFormat    = CAM_IMGFMT_YCbCr420P;
CAM_ImageFormat eDefaultPreviewFormat  = CAM_IMGFMT_YCbCr420P;
CAM_FlipRotate  eDefaultPreviewRotate  = CAM_FLIPROTATE_NORMAL;
CAM_Size        stDefaultPreviewSize   = {SCREEN_WIDTH, SCREEN_HEIGHT};
CAM_Int32s      iPreviewFpsQ16         = (30 << 16);
CAM_Int32s      iVideoFpsQ16           = (30 << 16);
#elif defined( MG1EVB )
CAM_FlipRotate  eSensorRotate          = CAM_FLIPROTATE_NORMAL;
CAM_ImageFormat eDefaultVideoFormat    = CAM_IMGFMT_CbYCrY;
CAM_ImageFormat eDefaultPreviewFormat  = CAM_IMGFMT_CbYCrY;
CAM_FlipRotate  eDefaultPreviewRotate  = CAM_FLIPROTATE_NORMAL;
CAM_Size        stDefaultPreviewSize   = {320, 240};
CAM_Int32s      iPreviewFpsQ16         = (30 << 16);
CAM_Int32s      iVideoFpsQ16           = (30 << 16);
#elif defined( PV2SAARB )
CAM_FlipRotate  eSensorRotate          = CAM_FLIPROTATE_NORMAL;
CAM_ImageFormat eDefaultVideoFormat    = CAM_IMGFMT_YCbCr420P;
CAM_ImageFormat eDefaultPreviewFormat  = CAM_IMGFMT_YCbCr420P;
CAM_FlipRotate  eDefaultPreviewRotate  = CAM_FLIPROTATE_NORMAL;
CAM_Size        stDefaultPreviewSize   = {480, 320};
CAM_Int32s      iPreviewFpsQ16         = (30 << 16);
CAM_Int32s      iVideoFpsQ16           = (30 << 16);
#elif defined( MG1SAARB )
CAM_FlipRotate  eSensorRotate          = CAM_FLIPROTATE_NORMAL;
CAM_ImageFormat eDefaultVideoFormat    = CAM_IMGFMT_CbYCrY;
CAM_ImageFormat eDefaultPreviewFormat  = CAM_IMGFMT_BGR565;
CAM_FlipRotate  eDefaultPreviewRotate  = CAM_FLIPROTATE_90L;// CAM_FLIPROTATE_NORMAL;
CAM_Size        stDefaultPreviewSize   = {320, 240};
CAM_Int32s      iPreviewFpsQ16         = (30 << 16);
CAM_Int32s      iVideoFpsQ16           = (30 << 16);
#else
#error no platform is defined!
#endif

#else


#if defined( BONNELL )
CAM_FlipRotate  eSensorRotate          = CAM_FLIPROTATE_NORMAL;
CAM_ImageFormat eDefaultVideoFormat    = CAM_IMGFMT_CbYCrY;
CAM_ImageFormat eDefaultPreviewFormat  = CAM_IMGFMT_CbYCrY;
CAM_FlipRotate  eDefaultPreviewRotate  = CAM_FLIPROTATE_NORMAL;
CAM_Size        stDefaultPreviewSize   = {640, 480};
CAM_Int32s      iPreviewFpsQ16         = (30 << 16);
CAM_Int32s      iVideoFpsQ16           = (30 << 16);
#elif defined( BROWNSTONE )
CAM_FlipRotate  eSensorRotate          = CAM_FLIPROTATE_NORMAL;
CAM_ImageFormat eDefaultVideoFormat    = CAM_IMGFMT_CbYCrY;
CAM_ImageFormat eDefaultPreviewFormat  = CAM_IMGFMT_CbYCrY;
CAM_FlipRotate  eDefaultPreviewRotate  = CAM_FLIPROTATE_NORMAL;
CAM_Size        stDefaultPreviewSize   = {640, 480};
CAM_Int32s      iPreviewFpsQ16         = (30 << 16);
CAM_Int32s      iVideoFpsQ16           = (30 << 16);
#elif defined ( ABILENE )
CAM_FlipRotate  eSensorRotate          = CAM_FLIPROTATE_NORMAL;
CAM_ImageFormat eDefaultVideoFormat    = CAM_IMGFMT_CbYCrY;
CAM_ImageFormat eDefaultPreviewFormat  = CAM_IMGFMT_CbYCrY;
CAM_FlipRotate  eDefaultPreviewRotate  = CAM_FLIPROTATE_NORMAL;
CAM_Size        stDefaultPreviewSize   = {640, 480};
CAM_Int32s      iPreviewFpsQ16         = (30 << 16);
CAM_Int32s      iVideoFpsQ16           = (30 << 16);
#elif defined( FLINT )
CAM_FlipRotate  eSensorRotate          = CAM_FLIPROTATE_NORMAL;
CAM_ImageFormat eDefaultVideoFormat    = CAM_IMGFMT_CbYCrY;
CAM_ImageFormat eDefaultPreviewFormat  = CAM_IMGFMT_CbYCrY;
CAM_ImageFormat eDefaultStillFormat    = CAM_IMGFMT_CbYCrY;
CAM_FlipRotate  eDefaultPreviewRotate  = CAM_FLIPROTATE_NORMAL;
CAM_Size        stDefaultPreviewSize   = {1920, 1080};
CAM_Size        stDefaultVideoSize     = {1920, 1080};
CAM_Size        stDefaultStillSize     = {3264, 2448}; // {2592, 1944};
CAM_Int32s      iPreviewFpsQ16         = (30 << 16);
CAM_Int32s      iVideoFpsQ16           = (30 << 16);
#elif defined( TTCDKB ) || defined( TDDKB )
CAM_FlipRotate  eSensorRotate          = CAM_FLIPROTATE_NORMAL;
CAM_ImageFormat eDefaultVideoFormat    = CAM_IMGFMT_YCbCr420P;
CAM_ImageFormat eDefaultPreviewFormat  = CAM_IMGFMT_YCbCr420P;
CAM_FlipRotate  eDefaultPreviewRotate  = CAM_FLIPROTATE_NORMAL;
CAM_Size        stDefaultPreviewSize   = {640, 480};
CAM_Int32s      iPreviewFpsQ16         = (30 << 16);
CAM_Int32s      iVideoFpsQ16           = (30 << 16);
#elif defined( PV2EVB )
CAM_FlipRotate  eSensorRotate          = CAM_FLIPROTATE_NORMAL;
CAM_ImageFormat eDefaultVideoFormat    = CAM_IMGFMT_CbYCrY;
CAM_ImageFormat eDefaultPreviewFormat  = CAM_IMGFMT_YCbCr420P;
CAM_FlipRotate  eDefaultPreviewRotate  = CAM_FLIPROTATE_NORMAL;
CAM_Size        stDefaultPreviewSize   = {SCREEN_WIDTH, SCREEN_HEIGHT};
CAM_Int32s      iPreviewFpsQ16         = (30 << 16);
CAM_Int32s      iVideoFpsQ16           = (30 << 16);
#elif defined( MG1EVB )
CAM_FlipRotate  eSensorRotate          = CAM_FLIPROTATE_NORMAL;
CAM_ImageFormat eDefaultVideoFormat    = CAM_IMGFMT_CbYCrY;
CAM_ImageFormat eDefaultPreviewFormat  = CAM_IMGFMT_CbYCrY;
CAM_FlipRotate  eDefaultPreviewRotate  = CAM_FLIPROTATE_NORMAL;
CAM_Size        stDefaultPreviewSize   = {320, 240};
CAM_Int32s      iPreviewFpsQ16         = (30 << 16);
CAM_Int32s      iVideoFpsQ16           = (30 << 16);
#elif defined( PV2SAARB )
CAM_FlipRotate  eSensorRotate          = CAM_FLIPROTATE_NORMAL;
CAM_ImageFormat eDefaultVideoFormat    = CAM_IMGFMT_CbYCrY;
CAM_ImageFormat eDefaultPreviewFormat  = CAM_IMGFMT_YCbCr420P;
CAM_FlipRotate  eDefaultPreviewRotate  = CAM_FLIPROTATE_NORMAL;
CAM_Size        stDefaultPreviewSize   = {480, 320};
CAM_Int32s      iPreviewFpsQ16         = (30 << 16);
CAM_Int32s      iVideoFpsQ16           = (30 << 16);
#elif defined( MG1SAARB )
CAM_FlipRotate  eSensorRotate          = CAM_FLIPROTATE_NORMAL;
CAM_ImageFormat eDefaultVideoFormat    = CAM_IMGFMT_CbYCrY;
CAM_ImageFormat eDefaultPreviewFormat  = CAM_IMGFMT_CbYCrY;
CAM_FlipRotate  eDefaultPreviewRotate  = CAM_FLIPROTATE_NORMAL;
CAM_Size        stDefaultPreviewSize   = {640, 480};
CAM_Int32s      iPreviewFpsQ16         = (30 << 16);
CAM_Int32s      iVideoFpsQ16           = (30 << 16);
#else
#error no platform is defined!
#endif

#endif


CAM_ImageBuffer         *pImgBuf;
CAM_ImageBuffer         *pPreviewBuf;
CAM_Int32s              *pPreviewBufStatus; 
CAM_Int32s              iPrevBufNum;
CAM_ImageBuffer         *pVideoBuf;
CAM_ImageBuffer         *pStillBuf;
CAM_ImageBufferReq      stPreviewBufReq;
CAM_ImageBufferReq      stVideoBufReq;
CAM_ImageBufferReq      stStillBufReq;

extern CAM_Bool bDumpFrame; 

CAM_JpegParam stJpegParam = 
{
	1,	// 422
	80, // quality factor
	CAM_FALSE, // enable exif
	CAM_FALSE, // embed thumbnail
	0, 0,
};

CAM_Int32s iExpectedPicNum = 0;
CAM_Int32s iPreviewBufNum  = APP_PREVIEW_BUFCNT;
CAM_Bool   bSaveJPEG       = CAM_TRUE;

int  video_recording_cnt = 0;
void *hCamcorderHandle   = NULL;

FILE *fp_preview_dump  = NULL;
FILE *fp_snapshot_dump = NULL;

display_cfg disp;


CAM_Error RestartCameraEngine( CAM_DeviceHandle handle )
{
	CAM_Error        error = CAM_ERROR_NONE;
	CAM_CaptureState eOldState;

	TRACE( "Recovering from the CI FIFO overflow fatal error...\n" );

#if 0
	error = CAM_SendCommand( handle, CAM_CMD_GET_STATE, (CAM_Param)&eOldState, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	switch ( eOldState )
	{
	case CAM_CAPTURESTATE_PREVIEW:
		error = CAM_SendCommand( handle, CAM_CMD_SET_STATE, (CAM_Param)CAM_CAPTURESTATE_IDLE, UNUSED_PARAM );
		ASSERT_CAM_ERROR( error );
		error = CAM_SendCommand( handle, CAM_CMD_SET_STATE, (CAM_Param)CAM_CAPTURESTATE_PREVIEW, UNUSED_PARAM );
		ASSERT_CAM_ERROR( error );
		break;

	case CAM_CAPTURESTATE_VIDEO:
		error = CAM_SendCommand( handle, CAM_CMD_SET_STATE, (CAM_Param)CAM_CAPTURESTATE_PREVIEW, UNUSED_PARAM );
		ASSERT_CAM_ERROR( error );
		error = CAM_SendCommand( handle, CAM_CMD_SET_STATE, (CAM_Param)CAM_CAPTURESTATE_IDLE, UNUSED_PARAM );
		ASSERT_CAM_ERROR( error );
		error = CAM_SendCommand( handle, CAM_CMD_SET_STATE, (CAM_Param)CAM_CAPTURESTATE_PREVIEW, UNUSED_PARAM );
		ASSERT_CAM_ERROR( error );
		error = CAM_SendCommand( handle, CAM_CMD_SET_STATE, (CAM_Param)CAM_CAPTURESTATE_VIDEO, UNUSED_PARAM );
		ASSERT_CAM_ERROR( error );
		break;

	case CAM_CAPTURESTATE_STILL:
		error = CAM_SendCommand( handle, CAM_CMD_SET_STATE, (CAM_Param)CAM_CAPTURESTATE_PREVIEW, UNUSED_PARAM );
		ASSERT_CAM_ERROR( error );
		error = CAM_SendCommand( handle, CAM_CMD_SET_STATE, (CAM_Param)CAM_CAPTURESTATE_IDLE, UNUSED_PARAM );
		ASSERT_CAM_ERROR( error );
		error = CAM_SendCommand( handle, CAM_CMD_SET_STATE, (CAM_Param)CAM_CAPTURESTATE_PREVIEW, UNUSED_PARAM );
		ASSERT_CAM_ERROR( error );
		error = CAM_SendCommand( handle, CAM_CMD_SET_STATE, (CAM_Param)CAM_CAPTURESTATE_STILL, UNUSED_PARAM );
		ASSERT_CAM_ERROR( error );
		break;

	default:
		break;
	}

#else
	error = CAM_SendCommand( handle, CAM_CMD_RESET_CAMERA, (CAM_Param)CAM_RESET_FAST, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

#endif

	TRACE( "Done!\n" );

	return error;
}

CAM_Error SafeDequeue( CAM_DeviceHandle handle, int *pPort, CAM_ImageBuffer **ppImgBuf )
{
	CAM_Error error = CAM_ERROR_NONE;

	error = CAM_SendCommand( handle, CAM_CMD_PORT_DEQUEUE_BUF, (CAM_Param)pPort, (CAM_Param)ppImgBuf );
	while( CAM_ERROR_FATALERROR == error )
	{
		CAM_Error error1 = RestartCameraEngine( handle );
		ASSERT_CAM_ERROR( error1 );

		error = CAM_SendCommand( handle, CAM_CMD_PORT_DEQUEUE_BUF, (CAM_Param)pPort, (CAM_Param)ppImgBuf );
	}

	return error;
}

CAM_Error InitEngine( CAM_DeviceHandle *pHandle )
{
	CAM_Error            error = CAM_ERROR_NONE;
	int                  i, iSensorCount;
	char                 cSensorName[4][32];
	CAM_DeviceHandle     handle;
	CAM_CameraCapability stCameraCap;

	// get Camera Engine handle, currently MRVL Camera Engine supports two operation handles: entry_socisp, entry_extisp
#ifdef FLINT
	error = CAM_GetHandle( &entry_socisp, &handle );
#else
	error = CAM_GetHandle( &entry_extisp, &handle );
#endif
	ASSERT_CAM_ERROR( error );

	*pHandle = handle;

	// enumerate all available sensors
	error = CAM_SendCommand( handle, CAM_CMD_ENUM_SENSORS, (CAM_Param)&iSensorCount, (CAM_Param)cSensorName );
	ASSERT_CAM_ERROR( error );

	TRACE( "\n All sensors in this platform:\n" );
	for ( i = 0; i < iSensorCount; i++ )
	{
		error = CAM_SendCommand( handle, CAM_CMD_QUERY_CAMERA_CAP, (CAM_Param)i, (CAM_Param)&stCameraCap );
		ASSERT_CAM_ERROR( error );

		TRACE( "\t%d - %s\n", i, cSensorName[i] );
		// TODO: you can check camera engine's capability here
	}
	TRACE( "\n" );

	return error;
}

CAM_Error SetSensor( CAM_DeviceHandle handle, CAM_Int32s iSensorID )
{
	CAM_Error            error       = CAM_ERROR_NONE;
	CAM_CameraCapability stCameraCap;
	CAM_PortCapability   *pPortCap   = NULL;
	CAM_Size             stDefaultStillSize;

	// select sensor id
	error = CAM_SendCommand( handle, CAM_CMD_SET_SENSOR_ID, (CAM_Param)iSensorID, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	error = CAM_SendCommand( handle, CAM_CMD_QUERY_CAMERA_CAP, (CAM_Param)iSensorID, (CAM_Param)&stCameraCap );
	ASSERT_CAM_ERROR( error );

	error = CAM_SendCommand( handle, CAM_CMD_SET_SENSOR_ORIENTATION, (CAM_Param)eSensorRotate, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	// error = CAM_SendCommand( handle, CAM_CMD_SET_PREVIEW_RESIZEFAVOR, (CAM_Param)CAM_TRUE, UNUSED_PARAM );  
	// ASSERT_CAM_ERROR( error );

	error = CAM_SendCommand( handle, CAM_CMD_SET_PREVIEW_FRAMERATE, (CAM_Param)iPreviewFpsQ16, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	error = CAM_SendCommand( handle, CAM_CMD_PORT_SET_SIZE, (CAM_Param)CAM_PORT_PREVIEW, (CAM_Param)&stDefaultPreviewSize );
	ASSERT_CAM_ERROR( error );

	error = CAM_SendCommand( handle, CAM_CMD_PORT_SET_FORMAT, (CAM_Param)CAM_PORT_PREVIEW, (CAM_Param)eDefaultPreviewFormat );
	ASSERT_CAM_ERROR( error );

	error = CAM_SendCommand( handle, CAM_CMD_PORT_SET_SIZE, (CAM_Param)CAM_PORT_VIDEO,(CAM_Param)&stDefaultPreviewSize );
	ASSERT_CAM_ERROR( error );

	error = CAM_SendCommand( handle, CAM_CMD_PORT_SET_FORMAT, (CAM_Param)CAM_PORT_VIDEO,(CAM_Param)eDefaultVideoFormat );
	ASSERT_CAM_ERROR( error );

	error = CAM_SendCommand( handle, CAM_CMD_SET_VIDEO_FRAMERATE, (CAM_Param)iVideoFpsQ16, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );


	pPortCap = &stCameraCap.stPortCapability[CAM_PORT_STILL];
	stDefaultStillSize = pPortCap->stMax;
	error = CAM_SendCommand( handle, CAM_CMD_PORT_SET_SIZE, (CAM_Param)CAM_PORT_STILL,(CAM_Param)&stDefaultStillSize );
	ASSERT_CAM_ERROR( error );

	error = CAM_SendCommand( handle, CAM_CMD_PORT_SET_FORMAT, (CAM_Param)CAM_PORT_STILL, (CAM_Param)pPortCap->eSupportedFormat[0] );
	ASSERT_CAM_ERROR( error );

	error = CAM_SendCommand( handle, CAM_CMD_SET_JPEGPARAM, (CAM_Param)&stJpegParam, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	return error;
}


CAM_Error DeinitEngine( CAM_DeviceHandle handle )
{
	CAM_Error error = CAM_ERROR_NONE;

	error = CAM_SendCommand( handle, CAM_CMD_PORT_FLUSH_BUF, (CAM_Param)CAM_PORT_ANY, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	error = CAM_FreeHandle( &handle );
	ASSERT_CAM_ERROR( error );

	return error;
}

CAM_Error StartPreview( CAM_DeviceHandle handle )
{
	CAM_Error       error = CAM_ERROR_NONE;
	int             i, ret;
	CAM_Size        stPreviewSize;
	CAM_ImageFormat eFormat;

	error = CAM_SendCommand( handle, CAM_CMD_PORT_GET_SIZE, (CAM_Param)CAM_PORT_PREVIEW, (CAM_Param)&stPreviewSize );
	ASSERT_CAM_ERROR( error );
	
	error = CAM_SendCommand( handle, CAM_CMD_PORT_GET_FORMAT, (CAM_Param)CAM_PORT_PREVIEW, (CAM_Param)&eFormat );
	ASSERT_CAM_ERROR( error );
 
	ret = displaydemo_open( MAX( 0, ( SCREEN_WIDTH - stPreviewSize.iWidth ) / 2 ),
	                        MAX( 0, ( SCREEN_HEIGHT - stPreviewSize.iHeight ) / 2 ),
	                        stPreviewSize.iWidth, stPreviewSize.iHeight,
	                        stPreviewSize.iWidth, stPreviewSize.iHeight,
	                        eFormat, &disp );
	ASSERT( ret == 0 );

	error = CAM_SendCommand( handle, CAM_CMD_PORT_GET_BUFREQ, (CAM_Param)CAM_PORT_PREVIEW, (CAM_Param)&stPreviewBufReq );
	ASSERT_CAM_ERROR( error );

	/* buffer negotiation on PREVIEW port */
	if ( stPreviewBufReq.iMinBufCount < iPreviewBufNum ) 
	{
		stPreviewBufReq.iMinBufCount = iPreviewBufNum;
	}
	error = AllocateImages( &stPreviewBufReq, &pPreviewBuf );
	ASSERT_CAM_ERROR( error );

	pPreviewBufStatus = malloc( sizeof( CAM_Int32s ) * stPreviewBufReq.iMinBufCount );
	ASSERT( pPreviewBufStatus != NULL );

	memset( pPreviewBufStatus, 0, sizeof( CAM_Int32s ) * stPreviewBufReq.iMinBufCount );
	iPrevBufNum = stPreviewBufReq.iMinBufCount;

	/* enqueue buffers on PREVIEW port */
	for ( i = 0; i < stPreviewBufReq.iMinBufCount; i++ )
	{
		error = CAM_SendCommand( handle, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)CAM_PORT_PREVIEW, (CAM_Param)&pPreviewBuf[i] );
		ASSERT_CAM_ERROR(error);
	}

	/* start preview */
	error = CAM_SendCommand( handle, CAM_CMD_SET_STATE, (CAM_Param)CAM_CAPTURESTATE_PREVIEW, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	return error;
}

CAM_Error StopPreview( CAM_DeviceHandle handle )
{
	CAM_Error        error = CAM_ERROR_NONE;
	CAM_CaptureState eState;

	error = CAM_SendCommand( handle, CAM_CMD_GET_STATE, (CAM_Param)&eState, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	if ( eState == CAM_CAPTURESTATE_STILL )
	{
		error = CAM_SendCommand( handle, CAM_CMD_SET_STATE, (CAM_Param)CAM_CAPTURESTATE_PREVIEW, UNUSED_PARAM );
		ASSERT_CAM_ERROR( error );
	}
	else if ( eState == CAM_CAPTURESTATE_VIDEO )
	{
		error = StopRecording( handle );
		ASSERT_CAM_ERROR( error );
	}

	error = CAM_SendCommand( handle, CAM_CMD_SET_STATE, (CAM_Param)CAM_CAPTURESTATE_IDLE, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	error = CAM_SendCommand( handle, CAM_CMD_PORT_FLUSH_BUF, (CAM_Param)CAM_PORT_ANY, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	if ( pPreviewBuf != NULL )
	{
		FreeImages( &pPreviewBuf, stPreviewBufReq.iMinBufCount );
	}

	if ( pVideoBuf != NULL )
	{
		FreeImages( &pVideoBuf, stVideoBufReq.iMinBufCount );
	}

	if ( pStillBuf != NULL )
	{
		FreeImages( &pStillBuf, stStillBufReq.iMinBufCount );
	}

	error = displaydemo_close( &disp );
	ASSERT_CAM_ERROR( error );

	return error;
}


CAM_Error AutoFocus( CAM_DeviceHandle handle, CAM_MultiROI *pstFocusROI )
{
	CAM_Error        error = CAM_ERROR_NONE;
	CAM_CaptureState eState;

	error = CAM_SendCommand( handle, CAM_CMD_GET_STATE, (CAM_Param)&eState, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	if ( eState != CAM_CAPTURESTATE_PREVIEW )
	{
		TRACE( "Warning: only accept auto focus command at preview state!\n" );
		error = CAM_ERROR_STATELIMIT;
	}
	else
	{
		error = CAM_SendCommand( handle, CAM_CMD_START_FOCUS, (CAM_Param)pstFocusROI, UNUSED_PARAM );
		// error = CAM_SendCommand( handle, CAM_CMD_START_FOCUS, UNUSED_PARAM, UNUSED_PARAM );
		// ASSERT_CAM_ERROR( error );
	}

	return error;
}


CAM_Error CancelAutoFocus( CAM_DeviceHandle handle )
{	
	CAM_Error        error;
	CAM_CaptureState eState;
	
	error = CAM_SendCommand( handle, CAM_CMD_GET_STATE, (CAM_Param)&eState, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	error = CAM_SendCommand( handle, CAM_CMD_CANCEL_FOCUS, UNUSED_PARAM, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	return error;
}


CAM_Error TakePicture( CAM_DeviceHandle handle )
{
	CAM_CaptureState eState;
	CAM_Error        error;
	int              i;

	if ( iExpectedPicNum > 0 )
	{
		TRACE( "Previous image(s) are not yet dequeued!\n" );
	}

	error = CAM_SendCommand( handle, CAM_CMD_GET_STATE, (CAM_Param)&eState, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	if ( eState != CAM_CAPTURESTATE_PREVIEW )
	{
		TRACE( "Bad state transition!\n" );
		return CAM_ERROR_BADSTATETRANSITION;
	}

	// Allocate and enqueue still image buffers
	error = CAM_SendCommand( handle, CAM_CMD_PORT_GET_BUFREQ, (CAM_Param)CAM_PORT_STILL, (CAM_Param)&stStillBufReq );
	ASSERT_CAM_ERROR( error );

	if ( pStillBuf == NULL )
	{
		// Buffer negotiation on STILL port
		if ( stStillBufReq.iMinBufCount < APP_STILL_BUFCNT )
		{
			stStillBufReq.iMinBufCount = APP_STILL_BUFCNT;
		}

		error = AllocateImages( &stStillBufReq, &pStillBuf );
		ASSERT_CAM_ERROR( error );

		// Enqueue all still image buffers
		for ( i = 0; i < stStillBufReq.iMinBufCount; i++ )
		{
			error = CAM_SendCommand( handle, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)CAM_PORT_STILL, (CAM_Param)&pStillBuf[i] );
			ASSERT_CAM_ERROR( error );
		}
	}

	error = CAM_SendCommand( handle, CAM_CMD_GET_STILL_BURST, (CAM_Param)&iExpectedPicNum, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	// display as black
	clear_overlay( &disp );

	// start still state
	error = CAM_SendCommand( handle, CAM_CMD_SET_STATE, (CAM_Param)CAM_CAPTURESTATE_STILL, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );
	TRACE( "Start to capture %d image(s) ( %s, %s, %d )\n", iExpectedPicNum, __FILE__, __FUNCTION__, __LINE__ );

	return error;
}

CAM_Error StartRecording( CAM_DeviceHandle handle )
{
	CAM_CaptureState eState;
	CAM_Error        error;
	char             sFileName[256] = {0};
	int              i;
	char             ext[][8] = {"yuv", "mpeg4", "h263", "h264"};

#ifdef _ENABLE_VIDEO_ENCODER_
	int iCamcorderType = 3;	// h264
#else
	int iCamcorderType = 0;	// raw data
#endif

	if ( iCamcorderType >= 0 )
	{
#if defined( ANDROID )
		sprintf( sFileName, "/data/Video%d%d%d.%s", (video_recording_cnt / 100) % 10, (video_recording_cnt / 10) % 10, video_recording_cnt % 10, ext[iCamcorderType] );
#else
		sprintf( sFileName, "./Video%d%d%d.%s", (video_recording_cnt / 100) % 10, (video_recording_cnt / 10) % 10, video_recording_cnt % 10, ext[iCamcorderType] );
#endif
		TRACE( "Video recording --> %s\n", sFileName );
	}

	error = CAM_SendCommand( handle, CAM_CMD_GET_STATE, (CAM_Param)&eState, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	if ( eState != CAM_CAPTURESTATE_PREVIEW )
	{
		TRACE( "Bad state transition!\n" );
		return CAM_ERROR_BADSTATETRANSITION;
	}

	video_recording_cnt++;

	// allocate and enqueue video image buffers
	error = CAM_SendCommand( handle, CAM_CMD_PORT_GET_BUFREQ, (CAM_Param)CAM_PORT_VIDEO, (CAM_Param)&stVideoBufReq );
	ASSERT_CAM_ERROR( error );

	// negotiate buffer count on video port
	if ( stVideoBufReq.iMinBufCount < APP_VIDEO_BUFCNT )
	{
		stVideoBufReq.iMinBufCount = APP_VIDEO_BUFCNT;
	}

	error = AllocateImages( &stVideoBufReq, &pVideoBuf );
	ASSERT_CAM_ERROR( error );

	// enqueue all video image buffers
	for ( i = 0; i < stVideoBufReq.iMinBufCount; i++ )
	{
		error = CAM_SendCommand( handle, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)CAM_PORT_VIDEO, (CAM_Param)&pVideoBuf[i] );
		ASSERT_CAM_ERROR( error );
	}

	// initialize the video encoder
	hCamcorderHandle = VideoEncoder_Init( handle, pVideoBuf, stVideoBufReq.iMinBufCount, iCamcorderType, sFileName );
	if ( hCamcorderHandle == NULL )
	{
		TRACE( "Failed to initialize the video encoder!\n" );
		ASSERT( hCamcorderHandle != NULL );
	}

	// start recording
	error = CAM_SendCommand( handle, CAM_CMD_SET_STATE, (CAM_Param)CAM_CAPTURESTATE_VIDEO, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	return error;
}


CAM_Error StopRecording( CAM_DeviceHandle handle )
{
	CAM_CaptureState eState;
	CAM_Error        error;

	error = CAM_SendCommand( handle, CAM_CMD_GET_STATE, (CAM_Param)&eState, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );
	if ( eState != CAM_CAPTURESTATE_VIDEO )
	{
		TRACE( "Bad state transition!\n" );
		return CAM_ERROR_BADSTATETRANSITION;
	}

	VideoEncoder_SendEOS( hCamcorderHandle );
	do
	{
		RunOneFrame( handle );
	} while ( !VideoEncoder_EOSReceived( hCamcorderHandle ) );

	TRACE( "Closing video file......\n" );

	VideoEncoder_Deinit( hCamcorderHandle );
	hCamcorderHandle = NULL;

	error = CAM_SendCommand( handle, CAM_CMD_SET_STATE, (CAM_Param)CAM_CAPTURESTATE_PREVIEW, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	error = CAM_SendCommand( handle, CAM_CMD_PORT_FLUSH_BUF, (CAM_Param)CAM_PORT_VIDEO, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	TRACE( "Done\n" );

	FreeImages( &pVideoBuf, stVideoBufReq.iMinBufCount );
	
	return error;
}


CAM_Error RunOneFrame( CAM_DeviceHandle handle )
{
	CAM_Error        error    = CAM_ERROR_NONE;
	CAM_CaptureState eState;
	int              iPortId;
	CAM_ImageBuffer  *pImgBuf = NULL;

	static CAM_CaptureState eOldState = CAM_CAPTURESTATE_IDLE;

	error = CAM_SendCommand( handle, CAM_CMD_GET_STATE, (CAM_Param)&eState, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	if ( eOldState != eState )
	{
		switch ( eState )
		{
		case CAM_CAPTURESTATE_STANDBY:
			TRACE( "Standby State\n" );
			break;
		case CAM_CAPTURESTATE_IDLE:
			TRACE( "Idle State\n" );
			break;
		case CAM_CAPTURESTATE_PREVIEW:
			TRACE( "Preview State\n" );
			break;
		case CAM_CAPTURESTATE_VIDEO:
			TRACE( "Video State\n" );
			break;
		case CAM_CAPTURESTATE_STILL:
			TRACE( "Still State\n" );
			break;
		default:
			TRACE( "Invalid State\n" );
			break;
		}
		eOldState = eState;
	}

	iPortId = CAM_PORT_ANY;
	error = SafeDequeue( handle, &iPortId, &pImgBuf );
	if ( error == CAM_ERROR_NOTENOUGHBUFFERS )	// happen when video encoder didn't return the buffer to camera engine timely
	{
		return error;
	}
	if ( error == CAM_ERROR_BUFFERPREEMPTED )	// this happens if you have a dequeue in FRAME READY event handler
	{
		TRACE( "CAM_ERROR_BUFFERPREEMPTED!\n" );
		return error;
	}
	if ( eState == CAM_CAPTURESTATE_STILL && error == CAM_ERROR_NOTSUPPORTEDARG )
	{
		TRACE( "No snapshot!\n" );;
	}


	// TRACE( "port:%d\n", iPortId );
	if ( error == CAM_ERROR_NONE )
	{
		switch ( iPortId )
		{
		case CAM_PORT_PREVIEW:
			if ( eState == CAM_CAPTURESTATE_STILL )
			{
				TRACE( "got snapshot %s, %d\n", __FILE__, __LINE__ );
				error = DeliverSnapshotBuffer( handle, pImgBuf );	// snapshot images
				ASSERT_CAM_ERROR( error );
			}
			else
			{
				error = DeliverPreviewBuffer( handle, pImgBuf );
				ASSERT_CAM_ERROR( error );
			}
			break;

		case CAM_PORT_STILL:
			error = DeliverStillBuffer( handle, pImgBuf );
			ASSERT_CAM_ERROR( error );
			break;

		case CAM_PORT_VIDEO:
			error = DeliverVideoBuffer( handle, pImgBuf );
			ASSERT_CAM_ERROR( error );
			break;

		default:
			ASSERT( CAM_FALSE );
			break;
		}
	}
	return CAM_ERROR_NONE;
}


CAM_Error GetParametersAndDisplay( CAM_DeviceHandle handle, CAM_CameraCapability *pCameraCap )
{
	CAM_Error              error;
	int                    i;
	CAM_PortCapability     *pPortCap;
	CameraParam            stCameraParam;
	CAM_ShotModeCapability stShotModeCap;

	TRACE( "<q>:\texit this application\n" );

	error = CAM_SendCommand( handle, CAM_CMD_PORT_GET_SIZE, (CAM_Param)CAM_PORT_PREVIEW, (CAM_Param)&stCameraParam.size );
	ASSERT_CAM_ERROR( error );
	TRACE( "<ps: %d x %d>:\tset preview size\n", stCameraParam.size.iWidth, stCameraParam.size.iHeight );

	pPortCap = &pCameraCap->stPortCapability[CAM_PORT_PREVIEW];
	if ( pPortCap->bArbitrarySize )
	{
		TRACE( "\t%d x %d - %d x %d\n",
	                pPortCap->stMin.iWidth, pPortCap->stMin.iHeight,
	                pPortCap->stMax.iWidth, pPortCap->stMax.iHeight );
	}
	else
	{
		for ( i = 0; i < pPortCap->iSupportedSizeCnt; i++ )
		{
			if ( stCameraParam.size.iWidth == pPortCap->stSupportedSize[i].iWidth &&
				 stCameraParam.size.iHeight == pPortCap->stSupportedSize[i].iHeight )
			{
				TRACE( "\t[%d x %d]", pPortCap->stSupportedSize[i].iWidth, pPortCap->stSupportedSize[i].iHeight );
			}
			else
			{
				TRACE( "\t%d x %d", pPortCap->stSupportedSize[i].iWidth, pPortCap->stSupportedSize[i].iHeight );
			}
		}
		TRACE("\n");
	}

	error = CAM_SendCommand( handle, CAM_CMD_PORT_GET_SIZE, (CAM_Param)CAM_PORT_VIDEO, (CAM_Param)&stCameraParam.size );
	ASSERT_CAM_ERROR( error );
	TRACE( "<vs: %d x %d>:\tset video size\n", stCameraParam.size.iWidth, stCameraParam.size.iHeight );

	pPortCap = &pCameraCap->stPortCapability[CAM_PORT_VIDEO];
	if ( pPortCap->bArbitrarySize )
	{
		TRACE( "\t%d x %d - %d x %d\n", pPortCap->stMin.iWidth, pPortCap->stMin.iHeight, pPortCap->stMax.iWidth, pPortCap->stMax.iHeight );
	}
	else
	{
		for ( i = 0; i < pPortCap->iSupportedSizeCnt; i++ )
		{
			if ( stCameraParam.size.iWidth == pPortCap->stSupportedSize[i].iWidth && stCameraParam.size.iHeight == pPortCap->stSupportedSize[i].iHeight )
			{
				TRACE( "\t[%d x %d]", pPortCap->stSupportedSize[i].iWidth, pPortCap->stSupportedSize[i].iHeight );
			}
			else
			{
				TRACE( "\t%d x %d", pPortCap->stSupportedSize[i].iWidth, pPortCap->stSupportedSize[i].iHeight );
			}
		}
		TRACE( "\n" );
	}

	error = CAM_SendCommand( handle, CAM_CMD_PORT_GET_SIZE, (CAM_Param)CAM_PORT_STILL, (CAM_Param)&stCameraParam.size );
	ASSERT_CAM_ERROR( error );
	TRACE( "<ss: %d x %d>:\tset still size\n", stCameraParam.size.iWidth, stCameraParam.size.iHeight );

	pPortCap = &pCameraCap->stPortCapability[CAM_PORT_STILL];
	if ( pPortCap->bArbitrarySize )
	{
		TRACE( "\t%d x %d - %d x %d\n",
	               pPortCap->stMin.iWidth, pPortCap->stMin.iHeight,
	               pPortCap->stMax.iWidth, pPortCap->stMax.iHeight );
	}
	else
	{
		for ( i = 0; i < pPortCap->iSupportedSizeCnt; i++ )
		{
			if ( stCameraParam.size.iWidth == pPortCap->stSupportedSize[i].iWidth && stCameraParam.size.iHeight == pPortCap->stSupportedSize[i].iHeight )
			{
				TRACE( "\t[%d x %d]", pPortCap->stSupportedSize[i].iWidth, pPortCap->stSupportedSize[i].iHeight );
			}
			else
			{
				TRACE( "\t%d x %d", pPortCap->stSupportedSize[i].iWidth, pPortCap->stSupportedSize[i].iHeight );
			}
		}
		TRACE( "\n" );
	}

	TRACE(
	       "<r>:\tswitch on/off video recording\n"
	       "<hp>:\thalf-press to prepare taking picture\n"
	       "<rp>:\trelease-press to cancel capture\n"
	       "<p>:\tfull-press to take picture\n" );

	error = CAM_SendCommand( handle, CAM_CMD_GET_SHOTMODE, (CAM_Param)&stCameraParam.int32, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );
	TRACE( "<pm: %d>:\tset photo mode\n", stCameraParam.int32 );
	for ( i = 0; i < pCameraCap->iSupportedShotModeCnt; i++ )
	{
		if ( (CAM_ShotMode)stCameraParam.int32 == pCameraCap->eSupportedShotMode[i] )
		{
			TRACE( "\t[%d - ", pCameraCap->eSupportedShotMode[i] );
		}
		else
		{
			TRACE( "\t%d - ", pCameraCap->eSupportedShotMode[i] );
		}

		switch ( pCameraCap->eSupportedShotMode[i] )
		{
		case CAM_SHOTMODE_AUTO:
			TRACE( "Auto" );
			break;
		case CAM_SHOTMODE_MANUAL:
			TRACE( "Manual" );
			break;
		case CAM_SHOTMODE_PORTRAIT:
			TRACE( "Portrait" );
			break;
		case CAM_SHOTMODE_LANDSCAPE:
			TRACE( "Landscape" );
			break;
		case CAM_SHOTMODE_NIGHTPORTRAIT:
			TRACE( "Night portrait" );
			break;
		case CAM_SHOTMODE_NIGHTSCENE:
			TRACE( "Night" );
			break;
		case CAM_SHOTMODE_CHILD:
			TRACE( "Child" );
			break;
		case CAM_SHOTMODE_INDOOR:
			TRACE( "Indoor" );
			break;
		case CAM_SHOTMODE_PLANTS:
			TRACE( "Plant" );
			break;
		case CAM_SHOTMODE_SNOW:
			TRACE( "Snow" );
			break;
		case CAM_SHOTMODE_BEACH:
			TRACE( "Beach" );
			break;
		case CAM_SHOTMODE_FIREWORKS:
			TRACE( "Fire work" );
			break;
		case CAM_SHOTMODE_SUBMARINE:
			TRACE( "Submarine" );
			break;
		default:
			ASSERT( 0 );
			break;
		}

		if ( (CAM_ShotMode)stCameraParam.int32 == pCameraCap->eSupportedShotMode[i] )
		{
			TRACE( "]" );
		}
	}
	TRACE( "\n" );

	error = CAM_SendCommand( handle, CAM_CMD_QUERY_SHOTMODE_CAP, (CAM_Param)stCameraParam.int32, (CAM_Param)&stShotModeCap );
	ASSERT_CAM_ERROR( error );

	error = CAM_SendCommand( handle, CAM_CMD_GET_EXPOSUREMETERMODE, (CAM_Param)&stCameraParam.int32, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );
	TRACE( "<em: %d>:\tset exposure metering mode\n", stCameraParam.int32 );
	for ( i = 0; i < stShotModeCap.iSupportedExpMeterCnt; i++ )
	{
		if ( (CAM_ExposureMeterMode)stCameraParam.int32 == stShotModeCap.eSupportedExpMeter[i] )
		{
			TRACE( "\t[%d - ", stShotModeCap.eSupportedExpMeter[i] );
		}
		else
		{
			TRACE( "\t%d - ", stShotModeCap.eSupportedExpMeter[i] );
		}

		switch ( stShotModeCap.eSupportedExpMeter[i] )
		{
		case CAM_EXPOSUREMETERMODE_AUTO:
			TRACE( "Auto" );
			break;
		case CAM_EXPOSUREMETERMODE_MEAN:
			TRACE( "Mean" );
			break;
		case CAM_EXPOSUREMETERMODE_CENTRALWEIGHTED:
			TRACE( "Central Weighted" );
			break;
		case CAM_EXPOSUREMETERMODE_SPOT:
			TRACE( "Spot" );
			break;
		case CAM_EXPOSUREMETERMODE_MATRIX:
			TRACE( "Matrix (Evaluative)" );
			break;
		default:
			ASSERT( 0 );
			break;
		}

		if ( (CAM_ExposureMeterMode)stCameraParam.int32 == stShotModeCap.eSupportedExpMeter[i] )
		{
			TRACE( "]" );
		}
	}
	TRACE( "\n" );
	
	error = CAM_SendCommand( handle, CAM_CMD_GET_EVCOMPENSATION, (CAM_Param)&stCameraParam.int32, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );
	TRACE( "<ev: %d>:\tset EV (%d ev ~ %d ev, step: %d)\n",\
		    stCameraParam.int32,\
		    stShotModeCap.iMinEVCompQ16, stShotModeCap.iMaxEVCompQ16, stShotModeCap.iEVCompStepQ16 );

	error = CAM_SendCommand( handle, CAM_CMD_GET_ISO, (CAM_Param)&stCameraParam.int32, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );
	TRACE( "<iso: %d>:\tset ISO mode\n", stCameraParam.int32 );
	for ( i = 0; i < stShotModeCap.iSupportedIsoModeCnt; i++ )
	{
		if ( (CAM_ISOMode)stCameraParam.int32 == stShotModeCap.eSupportedIsoMode[i] )
		{
			TRACE( "\t[%d - ", stShotModeCap.eSupportedIsoMode[i] );
		}
		else
		{
			TRACE( "\t%d - ", stShotModeCap.eSupportedIsoMode[i] );
		}

		switch ( stShotModeCap.eSupportedIsoMode[i] )
		{
		case CAM_ISO_AUTO:
			TRACE( "Auto" );
			break;
		case CAM_ISO_50:
			TRACE( "50" );
			break;
		case CAM_ISO_100:
			TRACE( "100" );
			break;
		case CAM_ISO_200:
			TRACE( "200" );
			break;
		case CAM_ISO_400:
			TRACE( "400" );
			break;
		case CAM_ISO_800:
			TRACE( "800" );
			break;
		case CAM_ISO_1600:
			TRACE( "1600" );
			break;
		case CAM_ISO_3200:
			TRACE( "3200" );
			break;
		default:
			ASSERT( 0 );
			break;
		}

		if ( (CAM_ISOMode)stCameraParam.int32 == stShotModeCap.eSupportedIsoMode[i] )
		{
			TRACE( "]" );
		}
	}
	TRACE( "\n" );

	error = CAM_SendCommand( handle, CAM_CMD_GET_SHUTTERSPEED, (CAM_Param)&stCameraParam.int32, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );
	TRACE( "<st: %.3f>:\tset shutter time (%.3f sec - %.3f sec)\n", stCameraParam.int32 / 65536.0f,
	       stShotModeCap.iMinShutSpdQ16 / 65536.0f, stShotModeCap.iMaxShutSpdQ16 / 65536.0f );

	error = CAM_SendCommand( handle, CAM_CMD_GET_FNUM, (CAM_Param)&stCameraParam.int32, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );
	TRACE( "<fn: %.1f>:\tset f-number (%.1f - %.1f)\n",stCameraParam.int32 / 65536.0f,
	       stShotModeCap.iMinFNumQ16 / 65536.0f, stShotModeCap.iMaxFNumQ16 / 65536.0f );

	error = CAM_SendCommand( handle, CAM_CMD_GET_BANDFILTER, (CAM_Param)&stCameraParam.int32, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );
	TRACE( "<bf: %d>:\tset band filter\n", stCameraParam.int32 );
	for ( i = 0; i < stShotModeCap.iSupportedBdFltModeCnt; i++ )
	{
		if ( (CAM_BandFilterMode)stCameraParam.int32 == stShotModeCap.eSupportedBdFltMode[i] )
		{
			TRACE( "\t[%d - ", stShotModeCap.eSupportedBdFltMode[i] );
		}
		else
		{
			TRACE( "\t%d - ", stShotModeCap.eSupportedBdFltMode[i] );
		}
		switch ( stShotModeCap.eSupportedBdFltMode[i] )
		{
		case CAM_BANDFILTER_AUTO:
			TRACE( "Auto" );
			break;
		case CAM_BANDFILTER_OFF:
			TRACE( "Off" );
			break;
		case CAM_BANDFILTER_50HZ:
			TRACE( "50 Hz" );
			break;
		case CAM_BANDFILTER_60HZ:
			TRACE( "60 Hz" );
			break;
		default:
			ASSERT( 0 );
			break;
		}

		if ( (CAM_BandFilterMode)stCameraParam.int32 == stShotModeCap.eSupportedBdFltMode[i] )
		{
			TRACE( "]" );
		}
	}
	TRACE( "\n" );

	error = CAM_SendCommand( handle, CAM_CMD_GET_FLASHMODE, (CAM_Param)&stCameraParam.int32, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );
	TRACE( "<sm: %d>:\tset flash mode\n", stCameraParam.int32 );
	for ( i = 0; i < stShotModeCap.iSupportedFlashModeCnt; i++ )
	{
		if ( (CAM_FlashMode)stCameraParam.int32 == stShotModeCap.eSupportedFlashMode[i] )
		{
			TRACE( "\t[ %d - ", stShotModeCap.eSupportedFlashMode[i] );
		}
		else
		{
			TRACE( "\t  %d - ", stShotModeCap.eSupportedFlashMode[i] );
		}

		switch ( stShotModeCap.eSupportedFlashMode[i] )
		{
		case CAM_FLASH_AUTO:
			TRACE( "Auto" );
			break;
		case CAM_FLASH_OFF:
			TRACE( "Off" );
			break;
		case CAM_FLASH_ON:
			TRACE( "On" );
			break;
		case CAM_FLASH_TORCH:
			TRACE( "Torch" );
			break;
		default:
			ASSERT( 0 );
			break;
		}

		if ( (CAM_FlashMode)stCameraParam.int32 == stShotModeCap.eSupportedFlashMode[i] )
		{
			TRACE( "]" );
		}
	}
	TRACE( "\n" );

	error = CAM_SendCommand( handle, CAM_CMD_GET_WHITEBALANCEMODE, (CAM_Param)&stCameraParam.int32, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );
	TRACE( "<wb: %d>:\tset white balance mode\n", stCameraParam.int32 );
	for ( i = 0; i < stShotModeCap.iSupportedWBModeCnt; i++ )
	{
		if ( (CAM_WhiteBalanceMode)stCameraParam.int32 == stShotModeCap.eSupportedWBMode[i] )
		{
			TRACE( "\t[%d - ", stShotModeCap.eSupportedWBMode[i] );
		}
		else
		{
			TRACE( "\t%d - ", stShotModeCap.eSupportedWBMode[i] );
		}

		switch ( stShotModeCap.eSupportedWBMode[i] )
		{
		case CAM_WHITEBALANCEMODE_AUTO:
			TRACE( "Auto" );
			break;
		case CAM_WHITEBALANCEMODE_INCANDESCENT:
			TRACE( "Incandescent" );
			break;
		case CAM_WHITEBALANCEMODE_DAYLIGHT_FLUORESCENT:
			TRACE( "Daylight Fluorescent" );
			break;
		case CAM_WHITEBALANCEMODE_DAYWHITE_FLUORESCENT:
			TRACE( "Daywhite Fluorescent" );
			break;
		case CAM_WHITEBALANCEMODE_COOLWHITE_FLUORESCENT:
			TRACE( "Coolwhite Fluorescent" );
			break;
		case CAM_WHITEBALANCEMODE_DAYLIGHT:
			TRACE( "Daylight" );
			break;
		case CAM_WHITEBALANCEMODE_CLOUDY:
			TRACE( "Cloudy" );
			break;
		case CAM_WHITEBALANCEMODE_SHADOW:
			TRACE( "Shadow" );
			break;
		default:
			ASSERT( 0 );
			break;
		}

		if ( (CAM_WhiteBalanceMode)stCameraParam.int32 == stShotModeCap.eSupportedWBMode[i] )
		{
			TRACE( "]" );
		}
	}
	TRACE( "\n" );

	error = CAM_SendCommand( handle, CAM_CMD_GET_FOCUSMODE, (CAM_Param)&stCameraParam.int32, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );
	TRACE( "<fm: %d>:\tset focus mode\n", stCameraParam.int32 );
	for ( i = 0; i < stShotModeCap.iSupportedFocusModeCnt; i++ )
	{
		if ( (CAM_FocusMode)stCameraParam.int32 == stShotModeCap.eSupportedFocusMode[i] )
		{
			TRACE( "\t[%d - ", stShotModeCap.eSupportedFocusMode[i] );
		}
		else
		{
			TRACE( "\t%d - ", stShotModeCap.eSupportedFocusMode[i] );
		}

		switch ( stShotModeCap.eSupportedFocusMode[i] )
		{
		case CAM_FOCUS_AUTO_ONESHOT_CENTER:
			TRACE( "Auto OneShot Center" );
			break;
		case CAM_FOCUS_AUTO_ONESHOT_TOUCH:
			TRACE( "Auto OneShot Touch" );
			break;
		case CAM_FOCUS_AUTO_CONTINUOUS_CENTER:
			TRACE( "Auto Continuous Center" );
			break;
		case CAM_FOCUS_AUTO_CONTINUOUS_FACE:
			TRACE( "Auto Continuous Face" );
			break;
		case CAM_FOCUS_SUPERMACRO:
			TRACE( "Supper Macro" );
			break;
		case CAM_FOCUS_MACRO:
			TRACE( "Macro" );
			break;
		case CAM_FOCUS_HYPERFOCAL:
			TRACE( "Hyper Focal" );
			break;
		case CAM_FOCUS_INFINITY:
			TRACE( "Infinity" );
			break;
		case CAM_FOCUS_MANUAL:
			TRACE( "Manual" );
			break;
		default:
			ASSERT( 0 );
			break;
		}

		if ( (CAM_FocusMode)stCameraParam.int32 == stShotModeCap.eSupportedFocusMode[i] )
		{
			TRACE( "]" );
		}
	}
	TRACE( "\n" );

	error = CAM_SendCommand( handle, CAM_CMD_GET_DIGZOOM, (CAM_Param)&stCameraParam.int32, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );
	TRACE( "<z: %.2f>:\tset digital zoom (%.2f - %.2f)\n", stCameraParam.int32 / 65536.0f, stShotModeCap.iMinDigZoomQ16 / 65536.0f, stShotModeCap.iMaxDigZoomQ16 / 65536.0f );

	error = CAM_SendCommand( handle, CAM_CMD_GET_COLOREFFECT, (CAM_Param)&stCameraParam.int32, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );
	TRACE( "<ce: %d>:\tset color effect\n", stCameraParam.int32 );
	for ( i = 0; i < stShotModeCap.iSupportedColorEffectCnt; i++ )
	{
		if ( (CAM_ColorEffect)stCameraParam.int32 == stShotModeCap.eSupportedColorEffect[i] )
		{
			TRACE( "\t[%d - ", stShotModeCap.eSupportedColorEffect[i] );
		}
		else
		{
			TRACE( "\t%d - ", stShotModeCap.eSupportedColorEffect[i] );
		}

		switch ( stShotModeCap.eSupportedColorEffect[i] )
		{
		case CAM_COLOREFFECT_OFF:
			TRACE( "Off" );
			break;
		case CAM_COLOREFFECT_VIVID:
			TRACE( "Vivid" );
			break;
		case CAM_COLOREFFECT_SEPIA:
			TRACE( "Sepia" );
			break;
		case CAM_COLOREFFECT_GRAYSCALE:
			TRACE( "Gray-scale" );
			break;
		case CAM_COLOREFFECT_NEGATIVE:
			TRACE( "Negative" );
			break;
		case CAM_COLOREFFECT_SOLARIZE:
			TRACE( "Solarize" );
			break;
		default:
			ASSERT( 0 );
			break;
		}

		if ( (CAM_ColorEffect)stCameraParam.int32 == stShotModeCap.eSupportedColorEffect[i] )
		{
			TRACE( "]" );
		}
	}
	TRACE( "\n" );

	error = CAM_SendCommand( handle, CAM_CMD_GET_BRIGHTNESS, (CAM_Param)&stCameraParam.int32, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );
	TRACE( "<b: %d>:\tset brightness (%d ~ %d)\n", stCameraParam.int32, stShotModeCap.iMinBrightness, stShotModeCap.iMaxBrightness );

	error = CAM_SendCommand( handle, CAM_CMD_GET_CONTRAST, (CAM_Param)&stCameraParam.int32, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );
	TRACE( "<c: %d>:\tset contrast (%d ~ %d)\n", stCameraParam.int32, stShotModeCap.iMinContrast, stShotModeCap.iMaxContrast );

	error = CAM_SendCommand( handle, CAM_CMD_GET_SATURATION, (CAM_Param)&stCameraParam.int32, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );
	TRACE( "<s: %d>:\tset saturation (%d ~ %d)\n", stCameraParam.int32, stShotModeCap.iMinSaturation, stShotModeCap.iMaxSaturation );
	
	if ( stShotModeCap.bSupportBurstCapture == CAM_TRUE )
	{
		error = CAM_SendCommand( handle, CAM_CMD_GET_STILL_BURST, (CAM_Param)&stCameraParam.int32, UNUSED_PARAM );
		ASSERT_CAM_ERROR( error );
		TRACE( "<bst: %d>:\tset still capture burst cnt (%d ~ %d)\n", stCameraParam.int32, 1, stShotModeCap.iMaxBurstCnt );
	}
	
	if ( stShotModeCap.bSupportVideoNoiseReducer == CAM_TRUE )
	{
		error = CAM_SendCommand( handle, CAM_CMD_GET_VIDEO_NR, (CAM_Param)&stCameraParam.int32, UNUSED_PARAM );
		ASSERT_CAM_ERROR( error );
		TRACE( "<vnr: %d>:\tset video noise reduction\n", stCameraParam.int32 );
	}

	if ( stShotModeCap.bSupportVideoStabilizer == CAM_TRUE )
	{
		error = CAM_SendCommand( handle, CAM_CMD_GET_VIDEO_STABLIZER, (CAM_Param)&stCameraParam.int32, UNUSED_PARAM );
		ASSERT_CAM_ERROR( error );
		TRACE( "<stab: %d>:\tset video stablizer\n", stCameraParam.int32 );
	}

	return error;
}

CAM_Error DequeueUserCmd( CAM_DeviceHandle handle, CAM_AppCmd *pCmd, CameraParam *pParam )
{
	char                   buf[256] = {0};
	int                    len = 0;
	float                  temp;
	CAM_ShotModeCapability stShotModeCap;
	CAM_Error              error = CAM_ERROR_NONE;
	CAM_Int32s             iShotMode;

	error = CAM_SendCommand( handle, CAM_CMD_GET_SHOTMODE, (CAM_Param)&iShotMode, UNUSED_PARAM );
	ASSERT_CAM_ERROR( error );

	error = CAM_SendCommand( handle, CAM_CMD_QUERY_SHOTMODE_CAP, (CAM_Param)iShotMode, (CAM_Param)&stShotModeCap );
	ASSERT_CAM_ERROR( error );

	len = read( STDIN_FILENO, buf, 255 );
	if ( len > 0 ) 
	{
		buf[len-1] = '\0';
		// TRACE( "cmd: %s\n", buf );
	}
	else 
	{
		buf[0] = '\0'; // in case buf is empty
	}

	if ( strcmp(buf, "q") == 0 )
	{
		*pCmd = CMD_EXIT;
	}
	else if ( sscanf(buf, "ss: %d x %d", &pParam->size.iWidth, &pParam->size.iHeight) == 2 )
	{
		*pCmd = CMD_STILLSIZE;
	}
	else if ( sscanf(buf, "vs: %d x %d", &pParam->size.iWidth, &pParam->size.iHeight) == 2 )
	{
		*pCmd = CMD_VIDEOSIZE;
	}
	else if ( sscanf(buf, "ps: %d x %d", &pParam->size.iWidth, &pParam->size.iHeight) == 2 )
	{
		*pCmd = CMD_PREVIEWSIZE;
	}
	else if ( strcmp(buf, "hp") == 0 )
	{
		*pCmd = CMD_PREPARECAPTURE;
	}
	else if ( strcmp(buf, "p") == 0 )
	{
		*pCmd = CMD_STILLCAPTURE;
	}
	else if ( strcmp(buf, "rp") == 0 )
	{
		*pCmd = CMD_CANCELCAPTURE;
	}
	else if ( strcmp(buf, "r") == 0 )
	{
		*pCmd = CMD_VIDEOONOFF;
	}
	else if ( sscanf(buf, "pm: %d", &pParam->int32) == 1 )
	{
		*pCmd = CMD_SHOTMODE;
	}
	else if ( sscanf(buf, "em: %d", &pParam->int32) == 1 )
	{
		*pCmd = CMD_EXPMETER;
	}
	else if ( sscanf(buf, "ev: %d", &pParam->int32) == 1 )
	{
		*pCmd = CMD_EVCOMP;
	}
	else if ( sscanf(buf, "iso: %d", &pParam->int32) == 1 )
	{
		*pCmd = CMD_ISO;
	}
	else if ( sscanf(buf, "st: %d", &pParam->int32) == 1 )
	{
		*pCmd = CMD_SHUTTERTIME;
	}
	else if ( sscanf(buf, "fn: %d", &pParam->int32) == 1 )
	{
		*pCmd = CMD_FNUM;
	}
	else if ( sscanf(buf, "bf: %d", &pParam->int32) == 1 )
	{
		*pCmd = CMD_BANDFILTER;
	}
	else if ( sscanf(buf, "sm: %d", &pParam->int32) == 1 )
	{
		*pCmd = CMD_FLASHMODE;
	}
	else if ( sscanf(buf, "wb: %d", &pParam->int32) == 1 )
	{
		*pCmd = CMD_WB;
	}
	else if ( sscanf(buf, "fm: %d", &pParam->int32) == 1 )
	{
		*pCmd = CMD_FOCUSMODE;
	}
	else if ( sscanf(buf, "z: %f", &temp) == 1 )
	{
		pParam->int32 = (int)( temp * 65536.0f + 0.5f );
		*pCmd = CMD_DIGZOOM;
	}
	else if ( sscanf(buf, "ce: %d", &pParam->int32) == 1 )
	{
		*pCmd = CMD_COLOREFFECT;
	}
	else if ( sscanf(buf, "b: %d", &pParam->int32) == 1 )
	{
		*pCmd = CMD_BRIGHTNESS;
	}
	else if ( sscanf(buf, "c: %d", &pParam->int32) == 1 )
	{
		*pCmd = CMD_CONTRAST;
	}
	else if ( sscanf(buf, "s: %d", &pParam->int32) == 1 )
	{
		*pCmd = CMD_SATURATION;
	}
	else if ( sscanf(buf, "sf: %d", &pParam->int32) == 1 )
	{
		*pCmd = CMD_SENSORFLIP;
	}
	else if ( sscanf(buf, "vnr: %d", &pParam->int32) == 1 )
	{
		*pCmd = CMD_VNRONOFF;
	}
	else if ( sscanf(buf, "bst: %d", &pParam->int32) == 1 )
	{
		*pCmd = CMD_BURST;
	}
	else if ( sscanf(buf, "stab: %d", &pParam->int32) == 1 )
	{
		*pCmd = CMD_VSONOFF;
	}
	else
	{
		*pCmd = CMD_NULL;
	}

	return CAM_ERROR_NONE;
}

CAM_Error SetParameters( CAM_DeviceHandle handle, CAM_AppCmd cmd, CameraParam stCameraParam )
{
	CAM_Error        error = CAM_ERROR_NONE;
	CAM_CaptureState eState;

	switch ( cmd )
	{
	case CMD_DIGZOOM:
		error = CAM_SendCommand( handle, CAM_CMD_SET_DIGZOOM, (CAM_Param)stCameraParam.int32, UNUSED_PARAM );
		if ( CAM_ERROR_NONE == error ) 
		{
			TRACE( "digital zoom changed to %.2f\n", stCameraParam.int32 / 65536.0f );
		}
		else
		{
	       ASSERT_CAM_ERROR( error );
		}
		break;
	case CMD_STILLSIZE:
		error = CAM_SendCommand( handle, CAM_CMD_GET_STATE, (CAM_Param)&eState, UNUSED_PARAM );
		ASSERT_CAM_ERROR( error );
		if ( eState == CAM_CAPTURESTATE_STILL )
		{
			TRACE( "Pls switch to preview state for still port size setting\n" );
		}
		else
		{
			error = CAM_SendCommand( handle, CAM_CMD_PORT_SET_SIZE, (CAM_Param)CAM_PORT_STILL, (CAM_Param)&(stCameraParam.size) );
			ASSERT_CAM_ERROR( error );
		}
		break;


	case CMD_VIDEOSIZE:
		error = CAM_SendCommand( handle, CAM_CMD_GET_STATE, (CAM_Param)&eState, UNUSED_PARAM );
		ASSERT_CAM_ERROR( error );
		if ( eState == CAM_CAPTURESTATE_VIDEO )
		{
			TRACE( "Pls switch to preview state for video port size setting\n" );
		}
		else
		{
			error = CAM_SendCommand( handle, CAM_CMD_PORT_SET_SIZE, (CAM_Param)CAM_PORT_VIDEO, (CAM_Param)&(stCameraParam.size) );
			ASSERT_CAM_ERROR( error );
		}
		break;


	case CMD_PREVIEWSIZE:
		error = CAM_SendCommand( handle, CAM_CMD_GET_STATE, (CAM_Param)&eState, UNUSED_PARAM );
		ASSERT_CAM_ERROR( error );
		if ( eState == CAM_CAPTURESTATE_PREVIEW )
		{
			StopPreview( handle );
			error = CAM_SendCommand( handle, CAM_CMD_PORT_SET_SIZE, (CAM_Param)CAM_PORT_PREVIEW, (CAM_Param)&(stCameraParam.size) );
			ASSERT_CAM_ERROR( error );
			StartPreview( handle );
		}
		else if ( eState == CAM_CAPTURESTATE_VIDEO )
		{
			TRACE( "Pls switch to preview state for preview port size setting\n" );
		}
		else
		{
			error = CAM_SendCommand( handle, CAM_CMD_PORT_SET_SIZE, (CAM_Param)CAM_PORT_PREVIEW, (CAM_Param)&(stCameraParam.size) );
			ASSERT_CAM_ERROR( error );
		}
		break;

	case CMD_VSONOFF:	// turn on/off video stabilizer
		error = CAM_SendCommand( handle, CAM_CMD_SET_VIDEO_STABLIZER, (CAM_Param)stCameraParam.int32, UNUSED_PARAM );
		if ( CAM_ERROR_NONE == error ) 
		{
			TRACE( "Video stabilizer is turned on\n" );
		}
		else 
		{
			ASSERT_CAM_ERROR( error );
		}
		break;

	case CMD_VNRONOFF:	// turn on/off video noise reducer
		error = CAM_SendCommand( handle, CAM_CMD_SET_VIDEO_NR, (CAM_Param)stCameraParam.int32, UNUSED_PARAM );
		if ( CAM_ERROR_NONE == error ) 
		{
			TRACE( "Video noise reducer is turned on\n" );
		}
		else 
		{
			ASSERT_CAM_ERROR( error );
		}
		break;

	case CMD_ZSLONOFF:	// turn on/off zero shutter lag
		error = CAM_SendCommand( handle, CAM_CMD_SET_STILL_ZSL, (CAM_Param)CAM_TRUE, UNUSED_PARAM );
		if ( CAM_ERROR_NONE == error )
		{
			TRACE( "Zero Shutter Lag (ZSL) is turned on\n" );
		}
		else
		{
			ASSERT_CAM_ERROR( error );
		}
		break;
		
	case CMD_BURST:
		error = CAM_SendCommand( handle, CAM_CMD_SET_STILL_BURST, (CAM_Param)stCameraParam.int32, UNUSED_PARAM );
		if ( CAM_ERROR_NONE == error )
		{
			TRACE( "still burst changed to %d\n", stCameraParam.int32 );
		}
		else
		{
			ASSERT_CAM_ERROR( error );
		}
		break;

	case CMD_HDRONOFF:	// turn on/off high dynamic range capture
		error = CAM_SendCommand( handle, CAM_CMD_SET_STILL_HDR, (CAM_Param)CAM_TRUE, UNUSED_PARAM );
		if ( CAM_ERROR_NONE == error ) 
		{
			TRACE( "High Dynamic Range (HDR) capture is turned on\n" );
		}
		else 
		{
			ASSERT_CAM_ERROR( error );
		}
		break;

	case CMD_SHOTMODE:
		error = CAM_SendCommand( handle, CAM_CMD_SET_SHOTMODE, (CAM_Param)stCameraParam.int32, UNUSED_PARAM );
		if ( CAM_ERROR_NONE == error ) 
		{
			TRACE( "Photo mode is changed\n" );
		}
		else 
		{
			ASSERT_CAM_ERROR( error );
		}
		break;

	case CMD_SENSORFLIP:
		error = CAM_SendCommand( handle, CAM_CMD_SET_SENSOR_ORIENTATION, (CAM_Param)stCameraParam.int32, UNUSED_PARAM );
		if ( CAM_ERROR_NONE == error ) 
		{
			TRACE( "Sensor rotation is changed\n" );
		}
		else 
		{
			ASSERT_CAM_ERROR( error );
		}
		break;

	case CMD_FLASHMODE:
		error = CAM_SendCommand( handle, CAM_CMD_SET_FLASHMODE, (CAM_Param)stCameraParam.int32, UNUSED_PARAM );
		if ( CAM_ERROR_NONE == error ) 
		{
			TRACE( "Flash mode is changed\n" );
		}
		else 
		{
			ASSERT_CAM_ERROR( error );
		}
		break;

	case CMD_EVCOMP:
		error = CAM_SendCommand( handle, CAM_CMD_SET_EVCOMPENSATION, (CAM_Param)stCameraParam.int32, UNUSED_PARAM );
		if ( CAM_ERROR_SHOTMODELIMIT == error ) 
		{
			TRACE( "NOT allowed in current photo mode!\n" );
		}
		else if ( CAM_ERROR_NONE == error )
		{
			TRACE( "EV is changed\n" );
		}
		else 
		{
			ASSERT_CAM_ERROR( error );
		}
		break;

	case CMD_COLOREFFECT:
		error = CAM_SendCommand( handle, CAM_CMD_SET_COLOREFFECT, (CAM_Param)stCameraParam.int32, UNUSED_PARAM );
		if ( CAM_ERROR_NONE == error ) 
		{
			TRACE( "Color effect is changed\n" );
		}
		else 
		{
			ASSERT_CAM_ERROR( error );
		}
		break;

	case CMD_ISO:
		error = CAM_SendCommand( handle, CAM_CMD_SET_ISO, (CAM_Param)stCameraParam.int32, UNUSED_PARAM );
		if ( CAM_ERROR_NONE == error )
		{
			TRACE( "ISO is changed\n" );
		}
		else 
		{
			ASSERT_CAM_ERROR( error );
		}
		break;

	case CMD_SHUTTERTIME:
		error = CAM_SendCommand( handle, CAM_CMD_SET_SHUTTERSPEED, (CAM_Param)stCameraParam.int32, UNUSED_PARAM );
		if ( CAM_ERROR_NONE == error ) 
		{
			TRACE( "Shutter speed is changed\n" );
		}
		else 
		{
			ASSERT_CAM_ERROR( error );
		}
		break;

	case CMD_FNUM:
		error = CAM_SendCommand( handle, CAM_CMD_SET_FNUM, (CAM_Param)stCameraParam.int32, UNUSED_PARAM );
		if ( CAM_ERROR_NONE == error ) 
		{
			TRACE( "F-Number is changed\n" );
		}
		else 
		{
			ASSERT_CAM_ERROR( error );
		}
		break;

	case CMD_WB:
		error = CAM_SendCommand( handle, CAM_CMD_SET_WHITEBALANCEMODE, (CAM_Param)stCameraParam.int32, UNUSED_PARAM );
		if ( CAM_ERROR_NONE == error ) 
		{
			TRACE( "White balance mode is changed\n" );
		}
		else 
		{
			ASSERT_CAM_ERROR( error );
		}
		break;

	case CMD_SATURATION:
		error = CAM_SendCommand( handle, CAM_CMD_SET_SATURATION, (CAM_Param)stCameraParam.int32, UNUSED_PARAM );
		if ( CAM_ERROR_NONE == error ) 
		{
			TRACE( "Saturation is changed\n" );
		}
		else 
		{
			ASSERT_CAM_ERROR( error );
		}
		break;

	case CMD_BRIGHTNESS:
		error = CAM_SendCommand( handle,CAM_CMD_SET_BRIGHTNESS , (CAM_Param)stCameraParam.int32, UNUSED_PARAM );
		if ( CAM_ERROR_NONE == error ) 
		{
			TRACE( "Brightness is changed\n" );
		}
		else 
		{
			ASSERT_CAM_ERROR( error );
		}
		break;

	case CMD_CONTRAST:
		error = CAM_SendCommand( handle, CAM_CMD_SET_CONTRAST, (CAM_Param)stCameraParam.int32, UNUSED_PARAM );
		if ( CAM_ERROR_NONE == error ) 
		{
			TRACE( "Contrast is changed\n" );
		}
		else 
		{
			ASSERT_CAM_ERROR( error );
		}
		break;

	case CMD_EXPMETER:
		error = CAM_SendCommand( handle, CAM_CMD_SET_EXPOSUREMETERMODE, (CAM_Param)stCameraParam.int32, UNUSED_PARAM );
		if ( CAM_ERROR_NONE == error ) 
		{
			TRACE( "Exposure meter mode is changed\n" );
		}
		else 
		{
			ASSERT_CAM_ERROR( error );
		}
		break;

	case CMD_BANDFILTER:
		error = CAM_SendCommand( handle, CAM_CMD_SET_BANDFILTER, (CAM_Param)stCameraParam.int32, UNUSED_PARAM );
		if ( CAM_ERROR_NONE == error ) 
		{
			TRACE( "Band filter is changed\n" );
		}
		else 
		{
			ASSERT_CAM_ERROR( error );
		}
		break;

	case CMD_FOCUSMODE:
		{
			error = CAM_SendCommand( handle, CAM_CMD_SET_FOCUSMODE, (CAM_Param)(stCameraParam.int32), (CAM_Param)NULL );
			if ( CAM_ERROR_NONE == error ) 
			{
				TRACE( "Focus mode is changed\n" );
			}
			else 
			{
				ASSERT_CAM_ERROR( error );
			}
		}
		break;

	default:
		TRACE( "Invalid command\n" );
		break;
	}

	return error;
}


CAM_Error DeliverPreviewBuffer( CAM_DeviceHandle handle, CAM_ImageBuffer *pImgBuf )
{
	CAM_Error  error = CAM_ERROR_NONE;
	int        i;
	static int iPreviewFrameCnt = 0;

#ifdef PREVIEW_DUMP
	if ( fp_preview_dump == NULL )
	{
#if defined( ANDROID )
		fp_preview_dump = fopen( "/data/preview.yuv", "wb" );
#else
		fp_preview_dump = fopen( "preview.yuv", "wb" );
#endif
	}

	if ( iPreviewFrameCnt < PREVIEW_DUMP ) 
	{
		for( i = 0; i < 3; i++ ) 
		{
			if ( pImgBuf->iFilledLen[i] > 0 )
			{
				fwrite( pImgBuf->pBuffer[i], 1, pImgBuf->iFilledLen[i], fp_preview_dump );
			}
		}

	}

	if ( iPreviewFrameCnt == PREVIEW_DUMP )
	{
		fclose( fp_preview_dump );
	}
#endif

	// dump specified frame
	if ( bDumpFrame )
	{
#if defined( ANDROID )
		FILE *fp = fopen( "/data/preview_dump.yuv", "wb" );
#else
		FILE *fp = fopen( "preview_dump.yuv", "wb" );
#endif
		for( i = 0; i < 3; i++ ) 
		{
			if ( pImgBuf->iFilledLen[i] > 0 )
			{
				fwrite( pImgBuf->pBuffer[i], 1, pImgBuf->iFilledLen[i], fp );
			}
		}
		fclose( fp );
	}

	// display to overlay
	display_to_overlay( handle, &disp, pImgBuf );

	if ( iPreviewFrameCnt % PRINT_INTERVAL == 0 )
	{
		TRACE( "preview %d\n", iPreviewFrameCnt );
	}
	iPreviewFrameCnt++;

	return error;
}

CAM_Error DeliverSnapshotBuffer( CAM_DeviceHandle handle, CAM_ImageBuffer *pImgBuf )
{
	CAM_Error  error = CAM_ERROR_NONE;
	int        i;
	static int iSnapshotFrameCnt = 0;

#ifdef SNAPSHOT_DUMP
	if ( fp_snapshot_dump == NULL )
	{
#if defined( ANDROID )
		fp_snapshot_dump = fopen( "/data/snapshot.yuv", "wb" );
#else
		fp_snapshot_dump = fopen( "snapshot.yuv", "wb" );
#endif
	}

	if ( iSnapshotFrameCnt < SNAPSHOT_DUMP ) 
	{
		for ( i = 0; i< 3; i++ ) 
		{
			if ( pImgBuf->iFilledLen[i] > 0 )
			{
				fwrite( pImgBuf->pBuffer[i], 1, pImgBuf->iFilledLen[i], fp_snapshot_dump );
			}
		}
	}

	if ( iSnapshotFrameCnt == SNAPSHOT_DUMP )
	{
		fclose( fp_snapshot_dump );
	}
#endif
	
	TRACE( "snapshot %d\n", iSnapshotFrameCnt );

	display_to_overlay( handle, &disp, pImgBuf );

	iSnapshotFrameCnt++;

	return error;
}


CAM_Error ExtractThumbnailAndDisplay( CAM_ImageBuffer *pImgBuf )
{

	TRACE( "TODO: ExtractThumbnailAndDisplay() is NOT implemented yet!\n" );

	return CAM_ERROR_NONE;
}

CAM_Error DeliverStillBuffer( CAM_DeviceHandle handle, CAM_ImageBuffer *pImgBuf )
{
	CAM_Error  error = CAM_ERROR_NONE;
	int        i;
	static int cnt = 0;
    
	if ( bSaveJPEG )
	{
		char     fname[256] = {0};
		FILE     *fp        = NULL;

#if defined( ANDROID )
		sprintf( fname, "/data/IMG%d%d%d.jpg", (cnt / 100) % 10, ( cnt/10 ) % 10, cnt % 10 );
#else
		sprintf( fname, "IMG%d%d%d.jpg", (cnt / 100) % 10, ( cnt/10 ) % 10, cnt % 10 );
#endif
		TRACE( "Store image location: %s\n", fname );
		fp = fopen( fname, "wb" );
		if ( fp == NULL )
		{
			TRACE( "Can not open file: %s\n", fname );
			return CAM_ERROR_OUTOFRESOURCE;
		}

		for ( i = 0; i< 3; i++ )
		{
			if ( pImgBuf->iFilledLen[i] > 0 )
			{
				fwrite( pImgBuf->pBuffer[i], 1, pImgBuf->iFilledLen[i], fp );
			}
		}

		fflush( fp );
		fclose( fp );
	}

	iExpectedPicNum--;
	if ( iExpectedPicNum == 0 )
	{
		error = CAM_SendCommand( handle, CAM_CMD_PORT_FLUSH_BUF, (CAM_Param)CAM_PORT_STILL, UNUSED_PARAM );
		ASSERT_CAM_ERROR( error );

		error = FreeImages( &pStillBuf, stStillBufReq.iMinBufCount );
		ASSERT_CAM_ERROR( error );
	}
	else
	{
		error = CAM_SendCommand( handle, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)CAM_PORT_STILL, (CAM_Param)pImgBuf );
		ASSERT_CAM_ERROR( error );
	}
	cnt++;

	return error;
}

CAM_Error DeliverVideoBuffer( CAM_DeviceHandle handle, CAM_ImageBuffer *pImgBuf )
{
	CAM_Error  error          = CAM_ERROR_NONE;
	static int iVideoFrameCnt = 0;

	if ( iVideoFrameCnt % PRINT_INTERVAL == 0 )
	{
		TRACE( "video %d\n", iVideoFrameCnt );
	}

	VideoEncoder_DeliverInput( hCamcorderHandle, pImgBuf );
	iVideoFrameCnt++;

	return error;
}

CAM_Error AllocateImages( CAM_ImageBufferReq *pBufReq, CAM_ImageBuffer **ppImgBuf )
{
	int             i, total_len;
	CAM_ImageBuffer *pImgBuf;

	pImgBuf = (CAM_ImageBuffer*)malloc( sizeof( CAM_ImageBuffer ) * pBufReq->iMinBufCount );
	(*ppImgBuf) = pImgBuf;

	for ( i = 0; i < pBufReq->iMinBufCount; i++ )
	{
		pImgBuf[i].eFormat       = pBufReq->eFormat;
		pImgBuf[i].iOffset[0]    = 0;
		pImgBuf[i].iOffset[1]    = 0;
		pImgBuf[i].iOffset[2]    = 0;
		pImgBuf[i].iStep[0]      = pBufReq->iMinStep[0];
		pImgBuf[i].iStep[1]      = pBufReq->iMinStep[1];
		pImgBuf[i].iStep[2]      = pBufReq->iMinStep[2];
		pImgBuf[i].iAllocLen[0]  = pBufReq->iMinBufLen[0] + pImgBuf[i].iOffset[0];
		pImgBuf[i].iAllocLen[1]  = pBufReq->iMinBufLen[1] + pImgBuf[i].iOffset[1];
		pImgBuf[i].iAllocLen[2]  = pBufReq->iMinBufLen[2] + pImgBuf[i].iOffset[2];
		pImgBuf[i].iFilledLen[0] = 0;
		pImgBuf[i].iFilledLen[1] = 0;
		pImgBuf[i].iFilledLen[2] = 0;
		pImgBuf[i].iWidth        = pBufReq->iWidth;
		pImgBuf[i].iHeight       = pBufReq->iHeight;
		pImgBuf[i].iUserIndex    = i;
		total_len                = pImgBuf[i].iAllocLen[0] + pBufReq->iAlignment[0] +
		                           pImgBuf[i].iAllocLen[1] + pBufReq->iAlignment[1] +
		                           pImgBuf[i].iAllocLen[2] + pBufReq->iAlignment[2];
		pImgBuf[i].pUserData     = NULL; 
		pImgBuf[i].pUserData     = osalbmm_malloc( total_len + 200, OSALBMM_ATTR_NONCACHED );  
		pImgBuf[i].iPrivateIndex = 0;
		pImgBuf[i].pPrivateData  = NULL;
		pImgBuf[i].iTick         = 0;
		pImgBuf[i].pBuffer[0]    = (CAM_Int8u*)ALIGN_TO( pImgBuf[i].pUserData, pBufReq->iAlignment[0] );
		pImgBuf[i].pBuffer[1]    = (CAM_Int8u*)ALIGN_TO( pImgBuf[i].pBuffer[0] + pImgBuf[i].iAllocLen[0], pBufReq->iAlignment[1] );
		pImgBuf[i].pBuffer[2]    = (CAM_Int8u*)ALIGN_TO( pImgBuf[i].pBuffer[1] + pImgBuf[i].iAllocLen[1], pBufReq->iAlignment[2] );
		pImgBuf[i].iPhyAddr[0]   = osalbmm_get_paddr( pImgBuf[i].pBuffer[0] );
		pImgBuf[i].iPhyAddr[1]   = osalbmm_get_paddr( pImgBuf[i].pBuffer[1] );
		pImgBuf[i].iPhyAddr[2]   = osalbmm_get_paddr( pImgBuf[i].pBuffer[2] );
		pImgBuf[i].iFlag         = BUF_FLAG_PHYSICALCONTIGUOUS |
		                           BUF_FLAG_L1NONCACHEABLE | BUF_FLAG_L2NONCACHEABLE | BUF_FLAG_UNBUFFERABLE |
		                           BUF_FLAG_YUVBACKTOBACK | BUF_FLAG_FORBIDPADDING;

		if ( pImgBuf[i].pUserData == NULL )
		{
			TRACE( "failed to %d osalbmm_malloc(%d, ...)\n", i, total_len );
			for ( i = i - 1; i >= 0; i-- )
			{
				osalbmm_free( pImgBuf[i].pUserData );
			}
			free( pImgBuf );
			(*ppImgBuf) = NULL;
			return CAM_ERROR_OUTOFMEMORY;
		}

		 // invalidate osalbmm buffer's cache line
		osalbmm_flush_cache_range( pImgBuf[i].pUserData, total_len, OSALBMM_DMA_FROM_DEVICE );


		memset( pImgBuf[i].pBuffer[0], 0, pImgBuf[i].iAllocLen[0] );
		// memset( pImgBuf[i].pBuffer[1], 0, pImgBuf[i].iAllocLen[1] );
		// memset( pImgBuf[i].pBuffer[2], 0, pImgBuf[i].iAllocLen[2] );
	}

	return CAM_ERROR_NONE;
}

CAM_Error FreeImages( CAM_ImageBuffer **ppImgBuf, int iCount )
{
	int             i;
	CAM_ImageBuffer *pImgBuf;

	pImgBuf = (*ppImgBuf);

	for ( i = 0; i < iCount; i++ )
	{
		osalbmm_free( pImgBuf[i].pUserData );
	}

	free( pImgBuf );

	*ppImgBuf = NULL;

	return CAM_ERROR_NONE;
}

// Required by IPP lib
void* _CALLBACK ippiMalloc( int size )
{
	return malloc( size );
}

void  _CALLBACK ippiFree( void *pSrcBuf )
{
	free( pSrcBuf );
	return;
}
