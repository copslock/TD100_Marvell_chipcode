/******************************************************************************
*(C) Copyright [2010 - 2011] Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
#ifndef _TEST_HARNESS_H_
#define _TEST_HARNESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CameraEngine.h"
#include "misc.h"

#define MIN(x, y) ( ((x) < (y)) ? (x) : (y) )

#define MAX(x, y) ( ((x) > (y)) ? (x) : (y) )

#define ALIGN_TO(x, iAlign) ( (((long long)(x)) + (iAlign) - 1) & (~((iAlign) - 1)) )

#if 0
#include <utils/Log.h>
#define LOG_TAG "MrvlCameraDemo"
#define TRACE LOGD
#else
#define TRACE(...)\
	do\
	{\
		printf( __VA_ARGS__ );\
		fflush( NULL );\
	} while( 0 )
#endif 

#define ASSERT(cond)\
	if ( !(cond) )\
	{\
		TRACE( "Assertion Failed (%s, %d): %s\n", __FILE__, __LINE__, #cond );\
		exit( -1 );\
	}



#define ERROR(...)\
	do\
	{\
		TRACE( __VA_ARGS__ );\
	} while ( 0 )

#define CAM_ECHO\
	do\
	{\
		TRACE( "%s, %d\n", __FILE__, __LINE__ );\
        /* usleep(100);*/\
	} while ( 0 );

#define ASSERT_CAM_ERROR( err )\
	do {\
	switch ( err )\
	{\
	case CAM_ERROR_NONE:\
		break;\
	case CAM_WARN_DUPLICATEBUFFER:\
		TRACE( "Error Code: CAM_WARN_DUPLICATEBUFFER, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_BADPOINTER:\
		TRACE( "Error Code: CAM_ERROR_BADPOINTER, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_BADARGUMENT:\
		TRACE( "Error Code: CAM_ERROR_BADARGUMENT, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_BADSENSORID:\
		TRACE( "Error Code: CAM_ERROR_BADSENSORID, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_BADPORTID:\
		TRACE( "Error Code: CAM_ERROR_BADPORTID, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_PORTNOTVALID:\
		TRACE( "Error Code: CAM_ERROR_PORTNOTVALID, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_PORTNOTCONFIGUREABLE:\
		TRACE( "Error Code: CAM_ERROR_PORTNOTCONFIGUREABLE, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_BADBUFFER:\
		TRACE( "Error Code: CAM_ERROR_BADBUFFER, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_BADSTATETRANSITION:\
		TRACE( "Error Code: CAM_ERROR_BADSTATETRANSITION, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_STATELIMIT:\
		TRACE( "Error Code: CAM_ERROR_STATELIMIT, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_SHOTMODELIMIT:\
		TRACE( "Error Code: CAM_ERROR_SHOTMODELIMIT, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_NOTSUPPORTEDCMD:\
		TRACE( "Error Code: CAM_ERROR_NOTSUPPORTEDCMD, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_NOTSUPPORTEDARG:\
		TRACE( "Error Code: CAM_ERROR_NOTSUPPORTEDARG, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_NOTIMPLEMENTED:\
		TRACE( "Error Code: CAM_ERROR_NOTIMPLEMENTED, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_NOTENOUGHBUFFERS:\
		TRACE( "Error Code: CAM_ERROR_NOTENOUGHBUFFERS, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_BUFFERPREEMPTED:\
		TRACE( "Error Code: CAM_ERROR_BUFFERPREEMPTED, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_PORTNOTACTIVE:\
		TRACE( "Error Code: CAM_ERROR_PORTNOTACTIVE, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_OUTOFMEMORY:\
		TRACE( "Error Code: CAM_ERROR_OUTOFMEMORY, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_OUTOFRESOURCE:\
		TRACE( "Error Code: CAM_ERROR_OUTOFRESOURCE, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_TIMEOUT:\
		TRACE( "Error Code: CAM_ERROR_TIMEOUT, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_DRIVEROPFAILED:\
		TRACE( "Error Code: CAM_ERROR_DRIVEROPFAILED, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_PPUFAILED:\
		TRACE( "Error Code: CAM_ERROR_PPUFAILED, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_FATALERROR:\
		TRACE( "Error Code: CAM_ERROR_FATALERROR, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	default:\
		TRACE( "Unknown Error - %d, %s - %d\n", err, __FILE__, __LINE__ );\
		break;\
	}\
		if ( err > CAM_ERROR_STARTCODE ) {exit( -1 );}\
	} while ( 0 )



#define PREVIEW_DUMP	-1
#define SNAPSHOT_DUMP	-1
#define PRINT_INTERVAL	30

#define APP_PREVIEW_BUFCNT        7 // at least 4 buffers, or the preview fps will be compromised due to the camera interface behavior
#define APP_VIDEO_ENCODER_BUFCNT  2
#define APP_CAMERA_BUFCNT         2
#define APP_SWITCH_BUFCNT         5
#define APP_VIDEO_BUFCNT          7 // (APP_VIDEO_ENCODER_BUFCNT + APP_CAMERA_BUFCNT + APP_SWITCH_BUFCNT - 1)
#define APP_STILL_BUFCNT          3




typedef enum 
{
	CMD_NULL,
	CMD_EXIT,

	CMD_STILLSIZE,
	CMD_VIDEOSIZE,
	CMD_PREVIEWSIZE,

	CMD_PREPARECAPTURE,
	CMD_STILLCAPTURE,
	CMD_CANCELCAPTURE,
	CMD_VIDEOONOFF,

	CMD_VSONOFF,
	CMD_VNRONOFF,

	CMD_ZSLONOFF,
	CMD_BURST,
	CMD_HDRONOFF,

	CMD_SHOTMODE,
	CMD_EXPMETER,
	CMD_EVCOMP,
	CMD_ISO,
	CMD_SHUTTERTIME,
	CMD_FNUM,
	CMD_FLASHMODE,
	CMD_WB,
	CMD_FOCUSMODE,
	CMD_DIGZOOM,
	CMD_COLOREFFECT,
	CMD_BRIGHTNESS,
	CMD_SATURATION,
	CMD_CONTRAST,
	CMD_BANDFILTER,
	CMD_SENSORFLIP,
} CAM_AppCmd;

typedef union 
{
	int         int32;
	CAM_Size    size;
} CameraParam;

extern CAM_Error RestartCameraEngine(CAM_DeviceHandle handle);
extern CAM_Error SafeDequeue(CAM_DeviceHandle handle, int *pPort, CAM_ImageBuffer **ppImgBuf);

extern CAM_Error InitEngine( CAM_DeviceHandle *pHandle );
extern CAM_Error SetSensor( CAM_DeviceHandle handle, CAM_Int32s iSensorID );
extern CAM_Error DeinitEngine(CAM_DeviceHandle handle);

extern CAM_Error StartPreview( CAM_DeviceHandle handle );
extern CAM_Error StopPreview( CAM_DeviceHandle handle );

extern CAM_Error StartRecording( CAM_DeviceHandle handle );
extern CAM_Error StopRecording( CAM_DeviceHandle handle );

extern CAM_Error AutoFocus( CAM_DeviceHandle handle, CAM_MultiROI *pstFocusROI );
extern CAM_Error CancelAutoFocus( CAM_DeviceHandle handle );
extern CAM_Error TakePicture( CAM_DeviceHandle handle );

extern CAM_Error SetParameters( CAM_DeviceHandle handle, CAM_AppCmd cmd, CameraParam param );
extern CAM_Error GetParametersAndDisplay( CAM_DeviceHandle handle, CAM_CameraCapability *pCameraCap );

extern CAM_Error RunOneFrame( CAM_DeviceHandle handle );
extern CAM_Error DequeueUserCmd( CAM_DeviceHandle handle, CAM_AppCmd *pCmd, CameraParam *pParam) ;
extern CAM_Error DeliverPreviewBuffer( CAM_DeviceHandle handle, CAM_ImageBuffer *pImgBuf );
extern CAM_Error DeliverSnapshotBuffer( CAM_DeviceHandle handle, CAM_ImageBuffer *pImgBuf );
extern CAM_Error DeliverVideoBuffer( CAM_DeviceHandle handle, CAM_ImageBuffer *pImgBuf );
extern CAM_Error DeliverStillBuffer( CAM_DeviceHandle handle, CAM_ImageBuffer *pImgBuf );
extern CAM_Error ExtractThumbnailAndDisplay( CAM_ImageBuffer *pImgBuf );

extern CAM_Error AllocateImages( CAM_ImageBufferReq *pBufReq, CAM_ImageBuffer **ppImgBuf );
extern CAM_Error FreeImages( CAM_ImageBuffer **ppImgBuf, int iCount );

extern CAM_FlipRotate   eSensorRotate;
extern CAM_ImageFormat  eDefaultPreviewFormat;
extern CAM_FlipRotate   eDefaultPreviewRotate;
extern CAM_Size         sDefaultPreviewSize;

extern CAM_ImageBuffer    *pPreviewBuf;
extern CAM_ImageBuffer    *pVideoBuf;
extern CAM_ImageBuffer    *pStillBuf;
extern CAM_ImageBufferReq stPreviewBufReq;
extern CAM_ImageBufferReq stVideoBufReq;
extern CAM_ImageBufferReq stStillBufReq;

#ifdef __cplusplus
}
#endif

#endif // _TEST_HARNESS_H_
