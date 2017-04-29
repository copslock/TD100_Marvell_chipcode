/******************************************************************************
*(C) Copyright [2010 - 2011] Marvell International Ltd.
* All Rights Reserved
******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "misc.h"
#include "CamOSALBmm.h"

#include "cam_utility.h"
#include "cam_log.h"

#include "cam_extisp_eng.h"
#include "cam_extisp_ppu.h"


/* functions declaration */

// entry functions
static CAM_Error _open( void **ppDeviceData );
static CAM_Error _close( void **ppDeviceData );
static CAM_Error _command( void *pDeviceData, CAM_Command cmd, CAM_Param param1, CAM_Param param2 );
static CAM_Error _threadsafe_open( void **ppDeviceData );
static CAM_Error _threadsafe_close( void **ppDeviceData );
static CAM_Error _threadsafe_command( void *pDeviceData, CAM_Command cmd, CAM_Param param1, CAM_Param param2 );

// internal utility functions
static CAM_Error _set_sensor_id( _CAM_ExtIspState *pCameraState, CAM_Int32s iSensorID );
static CAM_Error _get_camera_capability( _CAM_ExtIspState *pCameraState, CAM_Int32s iSensorID, CAM_CameraCapability *pCameraCap );
static CAM_Error _get_port_bufreq( _CAM_ExtIspState *pCameraState, CAM_Int32s iPortId, CAM_ImageBufferReq *pBufReq );
static CAM_Error _enqueue_buffer( _CAM_ExtIspState *pCameraState, CAM_Int32s iPortId, CAM_ImageBuffer *pBuf );
static CAM_Error _dequeue_buffer( _CAM_ExtIspState *pCameraState, CAM_Int32s *pPortId, CAM_ImageBuffer **ppBuf );
static CAM_Error _flush_buffer( _CAM_ExtIspState *pCameraState, CAM_Int32s iPortId );
static CAM_Error _set_camera_state( _CAM_ExtIspState *pCameraState, CAM_CaptureState eToState );
static CAM_Error _set_digital_zoom( _CAM_ExtIspState *pCameraState, CAM_Int32s iDigitalZoomQ16 );
static CAM_Error _set_port_size( _CAM_ExtIspState *pCameraState, CAM_Int32s iPortId, CAM_Size *pSize );
static CAM_Error _set_port_format( _CAM_ExtIspState *pCameraState, CAM_Int32s iPortId, CAM_ImageFormat eFormat );
static CAM_Error _set_port_rotation( _CAM_ExtIspState *pCameraState, CAM_Int32s iPortId, CAM_FlipRotate eRotation );
static CAM_Error _set_jpeg_param( _CAM_ExtIspState *pCameraState, CAM_JpegParam *pJpegParam ); 
static CAM_Error _get_shotmode_capability( CAM_Int32s iSensorId, CAM_ShotMode eShotMode, CAM_ShotModeCapability *pShotModeCap );
static CAM_Error _get_required_sensor_settings( _CAM_ExtIspState *pCameraState, CAM_CaptureState eCaptureState, _CAM_SmartSensorConfig *pSensorConfig );
static CAM_Error _dequeue_filled( _CAM_ExtIspState *pCameraState, CAM_Int32s *pPortId, CAM_ImageBuffer **ppBuf );
static CAM_Error _check_image_buffer( CAM_ImageBuffer *pBuf, _CAM_PortState *pPortState );
static CAM_Error _run_one_frame( _CAM_ExtIspState *pCameraState );
static CAM_Error _process_previewstate_buffer( _CAM_ExtIspState *pCameraState );
static CAM_Error _process_videostate_buffer( _CAM_ExtIspState *pCameraState );
static CAM_Error _process_stillstate_buffer( _CAM_ExtIspState *pCameraState );
static CAM_Error _calc_digital_zoom( _CAM_ExtIspState *pCameraState, CAM_Size *pSize, CAM_Int32s iDigitalZoomQ16, 
                                     CAM_Int32s *pSensorDigitalZoomQ16, CAM_Int32s *pPpuDigitalZoomQ16 );
static CAM_Error _convert_digital_zoom( _CAM_ExtIspState *pCameraState, CAM_Size *pSize, CAM_CaptureState eToState );
static CAM_Error _set_sensor_digital_zoom( void *hSensorHandle, CAM_Int32s iSensorDigitalZoomQ16 );
static CAM_Error _reset_camera( _CAM_ExtIspState *pCameraState, CAM_Int32s iResetType );

//state transtion function
static CAM_Error _standby2idle( _CAM_ExtIspState *pCameraState );
static CAM_Error _idle2null( _CAM_ExtIspState *pCameraState );
static CAM_Error _idle2standby( _CAM_ExtIspState *pCameraState );
static CAM_Error _idle2preview( _CAM_ExtIspState *pCameraState );
static CAM_Error _preview2idle( _CAM_ExtIspState *pCameraState );
static CAM_Error _preview2video( _CAM_ExtIspState *pCameraState );
static CAM_Error _preview2still( _CAM_ExtIspState *pCameraState );
static CAM_Error _video2preview( _CAM_ExtIspState *pCameraState );
static CAM_Error _still2preview( _CAM_ExtIspState *pCameraState );

static CAM_Error _request_ppu_srcimg( _CAM_ExtIspState *pCameraState, CAM_ImageBufferReq *pBufReq, CAM_ImageBuffer *pUsrImgBuf, CAM_ImageBuffer **ppImgBuf );
static CAM_Error _release_ppu_srcimg( _CAM_ExtIspState *pCameraState, CAM_ImageBuffer *pImgBuf );
static CAM_Error _flush_all_ppu_srcimg( _CAM_ExtIspState *pCameraState );

static CAM_Bool  _is_valid_port( _CAM_ExtIspState *pCameraState, CAM_Int32s iPortId );
static CAM_Bool  _is_configurable_port( _CAM_ExtIspState *pCameraState, CAM_Int32s iPortId );
static CAM_Error _is_need_ppu( _CAM_PortState *pPortState, _CAM_SmartSensorConfig *pSensorConfig, CAM_Int32s iPpuDigitalZoomQ16,
                               CAM_Bool *pbNeedPostProcessing, CAM_Bool *pbNeedPrivateBuffer );
static CAM_Error _negotiate_image_format( CAM_ImageFormat dstFmt, CAM_ImageFormat *pSrcFmt, CAM_ImageFormat *pFmtCap, CAM_Int32s iFmtCapCnt );
static CAM_Error _lock_port( _CAM_ExtIspState *pCameraState, CAM_Int32s iPortId );
static CAM_Error _unlock_port( _CAM_ExtIspState *pCameraState, CAM_Int32s iPortId );


/* MACRO definition */ 
#define SEND_EVENT( pState, eEventId, param )\
	do\
	{\
		if ( pState->fnEventHandler != NULL )\
		{\
			pState->fnEventHandler( eEventId, (void*)param, pState->pUserData );\
		}\
		else\
		{\
			TRACE( CAM_WARN, "Warning: can not notify events due to Camera Engine event handler is not set! \n" );\
		}\
	} while ( 0 )

#define SET_SHOTMODE_PARAM( pCameraState, TYPE, PARA_NAME, param, error )\
do\
{\
	_CAM_ShotParam stShotParam = stShotParamStatus;\
	stShotParam.PARA_NAME = (TYPE)param;\
	error = SmartSensor_SetShotParam( pCameraState->hSensorHandle, &stShotParam );\
} while ( 0 )


/* Global entities */
// ext-isp camera-engine entry
CAM_DriverEntry entry_extisp = 
{
	"camera_extisp",
	_threadsafe_open,
	_threadsafe_close,
	_threadsafe_command,
};


/* Function definitions */
#if !defined( BUILD_OPTION_DEBUG_DISABLE_PROCESS_THREAD )
static CAM_Int32s ProcessBufferThread( void *pParam )
{
	CAM_Int32s       ret           = 0;
	CAM_Error        error         = CAM_ERROR_NONE;
	_CAM_ExtIspState *pCameraState = (_CAM_ExtIspState*)pParam;


	for ( ; ; )
	{
		error = CAM_ERROR_NONE;

		ret = IPP_EventWait( pCameraState->stProcessBufferThread.hEventWakeup, INFINITE_WAIT, NULL );
		ASSERT( ret == 0 );

		if ( pCameraState->stProcessBufferThread.bExitThread )
		{
			ret = IPP_EventSet( pCameraState->stProcessBufferThread.hEventExitAck );
			ASSERT( ret == 0 );
			break;
		}

		_lock_port( pCameraState, CAM_PORT_PREVIEW );
		switch ( pCameraState->eCaptureState )
		{
		case CAM_CAPTURESTATE_NULL:
		case CAM_CAPTURESTATE_STANDBY:
		case CAM_CAPTURESTATE_IDLE:
			error = CAM_ERROR_PORTNOTACTIVE;
			break;

		case CAM_CAPTURESTATE_PREVIEW:
			error = _process_previewstate_buffer( pCameraState );
			break;

		case CAM_CAPTURESTATE_VIDEO:
			_lock_port( pCameraState, CAM_PORT_VIDEO );
			error = _process_videostate_buffer( pCameraState );
			_unlock_port( pCameraState, CAM_PORT_VIDEO );
			break;

		case CAM_CAPTURESTATE_STILL:
			_lock_port( pCameraState, CAM_PORT_STILL );
			error = _process_stillstate_buffer( pCameraState );
			_unlock_port( pCameraState, CAM_PORT_STILL );
			break;

		default:
			ASSERT( 0 );
			break;
		}
		_unlock_port( pCameraState, CAM_PORT_PREVIEW );

		if ( error == CAM_ERROR_NONE )
		{
			// set event because there's potential more work to do
			ret = IPP_EventSet( pCameraState->stProcessBufferThread.hEventWakeup ); 
			ASSERT( ret == 0 );
		}
		else
		{
			switch ( error )
			{
			case CAM_ERROR_NOTENOUGHBUFFERS:
				// This error happens when camera-engine client doesn't en-queue enough number of buffers
				// when it happens, "ProcessBufferThread" should sleep till camera-engine client send new commands
				break;

			case CAM_ERROR_PORTNOTACTIVE:
				// This error happens when camera-engine is in Null/ Idle state, or in still state while all the required
				// images have been generated.
				// when it happens, "ProcessBufferThread" should sleep till camera-engine client send new commands
				break;

			case CAM_ERROR_FATALERROR:
				// This error happens when camera device met a serious issue and can not automatically recover
				// the connection with sensor
				// when it happens, "ProcessBufferThread" should sleep till camera-engine client reset the camera device
				// by change Camera Engine state to Idle
				SEND_EVENT( pCameraState, CAM_EVENT_FATALERROR, 0 );
				break;

			default:
				TRACE( CAM_ERROR, "Error: ProcessBufferThread exits with error code[%d]( %s, %s, %d )\n", 
				       error, __FILE__, __FUNCTION__, __LINE__ );
				return -1;
				break;
			}
		}
		// prevent ProcessBufferThread locks the camera-engine port all the time, 
		// XXX: make assumption that the maximum can no faster than 120fps
		IPP_Sleep( 8333 );
	}

	IPP_ThreadExit( (void*)(0) );

	return 0;
}
#endif

static CAM_Int32s CheckFocusThread( _CAM_ExtIspState *pCameraState )
{
	CAM_Int32s      ret               = 0;
	CAM_Error       error             = CAM_ERROR_NONE;
	CAM_Bool        bFocusAutoStopped = CAM_FALSE;
	_CAM_FocusState eFocusState;

	CAM_Tick        t = -IPP_TimeGetTickCount();

	for ( ; ; )
	{
		if ( pCameraState->stFocusThread.bExitThread )
		{
			// be destroyed
			ret = IPP_EventSet( pCameraState->stFocusThread.hEventExitAck );
			ASSERT( ret == 0 );
			
			IPP_ThreadExit( (void*)(0) );
			return 0;
		}

		error = SmartSensor_CheckFocusState( pCameraState->hSensorHandle, &bFocusAutoStopped, &eFocusState );
		ASSERT_CAM_ERROR( error );

		if ( eFocusState == CAM_IN_FOCUS )
		{
			SEND_EVENT( pCameraState, CAM_EVENT_IN_FOCUS, -1 );
			if ( bFocusAutoStopped == CAM_TRUE )
			{
				SEND_EVENT( pCameraState, CAM_EVENT_FOCUS_AUTO_STOP, CAM_TRUE );
				CELOG( "FOCUS SUCCESS\n" );
				break;
			}
		}
		else if ( eFocusState == CAM_OUTOF_FOCUS )
		{
			SEND_EVENT( pCameraState, CAM_EVENT_OUTOF_FOCUS, -1 );
		}
		else if ( eFocusState == CAM_AF_FAIL )
		{
			if ( bFocusAutoStopped == CAM_FALSE )
			{
				error = SmartSensor_CancelFocus( pCameraState->hSensorHandle );
				ASSERT_CAM_ERROR( error );
			}
			SEND_EVENT( pCameraState, CAM_EVENT_FOCUS_AUTO_STOP, CAM_FALSE );
			CELOG( "FOCUS FAIL\n" );
			break;
		}

		IPP_Sleep( 8333 );
	}

	t += IPP_TimeGetTickCount();
	
	CELOG( "focus time: %.2f ms\n", t / 1000.0 );

	// self exit
	ret = IPP_EventDestroy( pCameraState->stFocusThread.hEventWakeup );
	ASSERT( ret == 0 );

	ret = IPP_EventDestroy( pCameraState->stFocusThread.hEventExitAck );
	ASSERT( ret == 0 );

	pCameraState->stFocusThread.iThreadID     = -1;
	pCameraState->stFocusThread.hEventWakeup  = NULL;
	pCameraState->stFocusThread.hEventExitAck = NULL;
	
	IPP_ThreadExit( (void*)(0) );

	return 0;
}


static CAM_Error _threadsafe_open( void **ppDeviceData )
{
	CAM_Int32s       ret           = 0;
	CAM_Error        error         = CAM_ERROR_NONE;
	_CAM_ExtIspState *pCameraState = NULL;

	error = _open( ppDeviceData );
	if ( error != CAM_ERROR_NONE )
	{
		return error;
	}
	pCameraState = (_CAM_ExtIspState*)*ppDeviceData;

#if !defined( BUILD_OPTION_DEBUG_DISABLE_PROCESS_THREAD )
	_CreateThread( &pCameraState->stProcessBufferThread, ProcessBufferThread, pCameraState );
#endif

	// create ce mutex
	ret = IPP_MutexCreate( &pCameraState->hCEMutex ); 
	ASSERT( ret == 0 );

	return CAM_ERROR_NONE;
}

static CAM_Error _threadsafe_close( void **ppDeviceData )
{
	_CAM_ExtIspState *pCameraState = NULL;
	CAM_Int32s       ret           = 0;
	CAM_Error        error         = CAM_ERROR_NONE;
	void             *hCEMutex     = NULL;

	if ( NULL == ppDeviceData || NULL == *ppDeviceData )
	{
		TRACE( CAM_ERROR, "Error: camera_extISP handle shouldn't be NULL( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_BADARGUMENT;
	}

	pCameraState = (_CAM_ExtIspState*)(*ppDeviceData);

	hCEMutex = pCameraState->hCEMutex;

	ret = IPP_MutexLock( hCEMutex, INFINITE_WAIT, NULL );
	ASSERT( ret == 0 );

#if !defined( BUILD_OPTION_DEBUG_DISABLE_PROCESS_THREAD )
	_DestroyThread( &(pCameraState->stProcessBufferThread) );
#endif

	error = _close( ppDeviceData );

	ret = IPP_MutexUnlock( hCEMutex );
	ASSERT( ret == 0 );

	ret =  IPP_MutexDestroy( hCEMutex ); 
	ASSERT( ret == 0 );

	return error;
}

static CAM_Error _threadsafe_command( void *pDeviceData,
                                      CAM_Command cmd,
                                      CAM_Param param1,
                                      CAM_Param param2 )
{
	CAM_Int32s       ret           = 0;
	CAM_Error        error         = CAM_ERROR_NONE;
	_CAM_ExtIspState *pCameraState = (_CAM_ExtIspState*)pDeviceData;

	if ( pCameraState == NULL )
	{
		TRACE( CAM_ERROR, "Error: camera_extISP handle shouldn't be NULL( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_BADARGUMENT;
	}

	ret = IPP_MutexLock( pCameraState->hCEMutex, INFINITE_WAIT, NULL );
	ASSERT( ret == 0 );

	error = _command( pDeviceData, cmd, param1, param2 );

	ret = IPP_MutexUnlock( pCameraState->hCEMutex );
	ASSERT( ret == 0 );

	return error;
}


static CAM_Error _open( void **ppDeviceData )
{
	CAM_Error        error           = CAM_ERROR_NONE;
	_CAM_ExtIspState *pCameraState   = NULL;
	CAM_Int32s       i               = 0;
	CAM_Int32s       ret             = 0;

	// allocate camera engine internal state structure
	pCameraState = (_CAM_ExtIspState*)malloc( sizeof( _CAM_ExtIspState ) );
	memset( pCameraState, 0, sizeof( _CAM_ExtIspState ) );

	// enum camera engine supported sensors
	error = SmartSensor_EnumSensors( &(pCameraState->iSensorCount), pCameraState->sSensorName );
	if ( error != CAM_ERROR_NONE )
	{
		free( pCameraState );
		pCameraState  = NULL;
		*ppDeviceData = NULL;

		TRACE( CAM_ERROR, "Camera Engine enum sensor failed( %s, %d )\n", __FILE__, __LINE__ );
		return error;
	}

	// initialize buffer queues of each port
	for ( i = CAM_PORT_PREVIEW; i <= CAM_PORT_STILL; i++ )
	{
		ret = _InitQueue( &pCameraState->stPortState[i].qEmpty, CAM_MAX_PORT_BUF_CNT );
		ASSERT( ret == 0 );
		ret = _InitQueue( &pCameraState->stPortState[i].qFilled, CAM_MAX_PORT_BUF_CNT );
		ASSERT( ret == 0 );

		ret = IPP_MutexCreate( &pCameraState->stPortState[i].hMutex ); 
		ASSERT( ret == 0 );
	}

	// create post-processing handle
	ret = _ppu_create_handle( &(pCameraState->hPpuHandle) );
	ASSERT( ret == 0 );

	// private buffer management
	pCameraState->iPpuSrcImgAllocateCnt = 0;

	// no sensor is chosen
	pCameraState->iSensorID = -1;
	
	// process buffer thread and check focus thread is not open
	pCameraState->stProcessBufferThread.iThreadID = -1;
	pCameraState->stFocusThread.iThreadID         = -1;

	*ppDeviceData = pCameraState;

	// camera now in NULL state
	pCameraState->eCaptureState = CAM_CAPTURESTATE_NULL;

	return error;
}

static CAM_Error _close( void **ppDeviceData )
{
	CAM_Error        error         = CAM_ERROR_NONE;
	_CAM_ExtIspState *pCameraState = NULL;
	CAM_Int32s       i             = 0;
	CAM_Int32s       ret           = 0;

	_CHECK_BAD_POINTER( ppDeviceData );
	_CHECK_BAD_POINTER( *ppDeviceData );

	pCameraState = (_CAM_ExtIspState*)(*ppDeviceData);

	ret = _ppu_destroy_handle( &pCameraState->hPpuHandle );
	ASSERT( ret == 0 );

	if ( pCameraState->hSensorHandle != NULL )
	{
		error = SmartSensor_Deinit( &pCameraState->hSensorHandle );
		ASSERT_CAM_ERROR( error );
	}

	for ( i = CAM_PORT_PREVIEW; i <= CAM_PORT_STILL; i++ )
	{
		ret = _DeinitQueue( &pCameraState->stPortState[i].qEmpty );
		ASSERT( ret == 0 );
		ret = _DeinitQueue( &pCameraState->stPortState[i].qFilled );
		ASSERT( ret == 0 );

		ret =  IPP_MutexDestroy( pCameraState->stPortState[i].hMutex ); 
		ASSERT( ret == 0 );
	}

	free( *ppDeviceData );
	*ppDeviceData = NULL;

	return CAM_ERROR_NONE;
}


static CAM_Error _command( void *pDeviceData,
                           CAM_Command cmd,
                           CAM_Param param1,
                           CAM_Param param2 )
{
	CAM_Int32s             ret               = 0;
	CAM_Error              error             = CAM_ERROR_NONE;
	_CAM_ExtIspState       *pCameraState     = (_CAM_ExtIspState*)pDeviceData;
	CAM_ShotModeCapability *pShotModeCap     = &pCameraState->stShotModeCap;
	_CAM_ShotParam         stShotParamStatus;

	memset( &stShotParamStatus, 0xff, sizeof( _CAM_ShotParam ) );

	_CHECK_BAD_POINTER( pCameraState );

	if ( pCameraState->hSensorHandle != NULL )
	{
		error = SmartSensor_GetShotParam( pCameraState->hSensorHandle, &stShotParamStatus );
		ASSERT_CAM_ERROR( error );
	}

	switch( cmd )
	{
	////////////////////////////////////////////////////////////////////////////////
	// initial settings
	////////////////////////////////////////////////////////////////////////////////
	case CAM_CMD_ENUM_SENSORS:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY );
		_CHECK_BAD_POINTER( param1 );
		_CHECK_BAD_POINTER( param2 );

		error = SmartSensor_EnumSensors( (CAM_Int32s*)param1, param2 );
		break;

	case CAM_CMD_QUERY_CAMERA_CAP:
		_CHECK_BAD_POINTER( param2 );

		error = _get_camera_capability( pCameraState, (CAM_Int32s)param1, (CAM_CameraCapability*)param2 );
		break;

	case CAM_CMD_SET_SENSOR_ID:
		_lock_port( pCameraState, CAM_PORT_ANY );
		error = _set_sensor_id( pCameraState, (CAM_Int32s)param1 );
		_unlock_port( pCameraState, CAM_PORT_ANY );
		break;

	case CAM_CMD_GET_SENSOR_ID:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );
		_CHECK_BAD_POINTER( param1 );

		*(CAM_Int32s*)param1 = pCameraState->iSensorID;
		break;

	case CAM_CMD_SET_EVENTHANDLER:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_NULL | CAM_CAPTURESTATE_IDLE );

		// NOTE: no need to check the pointer validity. If param1 == NULL, it means that
		// user want to remove the previously registered event handler
		pCameraState->fnEventHandler = (CAM_EventHandler)param1;
		pCameraState->pUserData = (void*)param2;
		
		if ( pCameraState->hSensorHandle != NULL )
		{
			error = SmartSensor_RegisterEventHandler( pCameraState->hSensorHandle, pCameraState->fnEventHandler, pCameraState->pUserData );
			ASSERT_CAM_ERROR( error );
		}
		break;

	case CAM_CMD_SET_SENSOR_ORIENTATION: 
		// XXX: for some smart sensor and driver restriction, sensor orientation can only be changed while stream-off
		//      but for usage-model perspective, we all know that it's unreasonable
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE );
		_CHECK_SUPPORT_ENUM( (CAM_FlipRotate)param1, pCameraState->stCamCap.eSupportedSensorRotate, pCameraState->stCamCap.iSupportedSensorRotateCnt );

		SET_SHOTMODE_PARAM( pCameraState, CAM_FlipRotate, eSensorRotation, param1, error );
		break;

	case CAM_CMD_GET_SENSOR_ORIENTATION:
		_CHECK_BAD_POINTER( param1 );
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );

		*(CAM_FlipRotate*)param1 = stShotParamStatus.eSensorRotation;
		break;

	////////////////////////////////////////////////////////////////////////////////
	// state machine control commands
	////////////////////////////////////////////////////////////////////////////////
	case CAM_CMD_SET_STATE:
		_lock_port( pCameraState, CAM_PORT_ANY );
		error = _set_camera_state( pCameraState, (CAM_CaptureState)param1 );
		_unlock_port( pCameraState, CAM_PORT_ANY );

#if !defined( BUILD_OPTION_DEBUG_DISABLE_PROCESS_THREAD )
		// wake-up process buffer thread since port may become active
		{
			CAM_Int32s ret = 0;
			ret = IPP_EventSet( pCameraState->stProcessBufferThread.hEventWakeup );
			ASSERT( ret == 0 );
		}
#endif
		break;

	case CAM_CMD_GET_STATE:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY );
		_CHECK_BAD_POINTER( param1 );

		*(CAM_CaptureState*)param1 = pCameraState->eCaptureState;
		break;

	////////////////////////////////////////////////////////////////////////////////
	// buffer management
	////////////////////////////////////////////////////////////////////////////////
	case CAM_CMD_PORT_GET_BUFREQ:
		_CHECK_BAD_SINGLE_PORT_ID( (CAM_Int32s)param1 );

		error = _get_port_bufreq( pCameraState, (CAM_Int32s)param1, (CAM_ImageBufferReq*)param2 );
		break;

	case CAM_CMD_PORT_ENQUEUE_BUF:
		_CHECK_BAD_SINGLE_PORT_ID( (CAM_Int32s)param1 );

		_lock_port( pCameraState, (CAM_Int32s)param1 );
		error = _enqueue_buffer( pCameraState, (CAM_Int32s)param1, (CAM_ImageBuffer*)param2 );
		_unlock_port( pCameraState, (CAM_Int32s)param1 );

#if !defined( BUILD_OPTION_DEBUG_DISABLE_PROCESS_THREAD )
		// wake-up process buffer thread since some buffer maybe can dequeue
		{
			CAM_Int32s ret = 0;
			ret = IPP_EventSet( pCameraState->stProcessBufferThread.hEventWakeup );
			ASSERT( ret == 0 );
		}
#endif
		break;

	case CAM_CMD_PORT_DEQUEUE_BUF:
		{
			CAM_Int32s iPortId;
			_CHECK_BAD_POINTER( param1 );
			iPortId = *(CAM_Int32s*)param1;
			_CHECK_BAD_ANY_PORT_ID( iPortId );

			_lock_port( pCameraState, iPortId );
			error = _dequeue_buffer( pCameraState, (CAM_Int32s*)param1, (CAM_ImageBuffer**)param2 );
			_unlock_port( pCameraState, iPortId );
		}
		break;

	case CAM_CMD_PORT_FLUSH_BUF:
		_CHECK_BAD_ANY_PORT_ID( (CAM_Int32s)param1 );

		_lock_port( pCameraState, (CAM_Int32s)param1 );
		error = _flush_buffer( pCameraState, (CAM_Int32s)param1 );
		_unlock_port( pCameraState, (CAM_Int32s)param1 );
		break;

	////////////////////////////////////////////////////////////////////////////////
	//  port configuration
	////////////////////////////////////////////////////////////////////////////////
	case CAM_CMD_PORT_SET_SIZE:
		_CHECK_BAD_SINGLE_PORT_ID( (CAM_Int32s)param1 );
		
		_lock_port( pCameraState, (CAM_Int32s)param1 );
		error = _set_port_size( pCameraState, (CAM_Int32s)param1, (CAM_Size*)param2 );
		_unlock_port( pCameraState, (CAM_Int32s)param1 );
		break;

	case CAM_CMD_PORT_GET_SIZE:
		_CHECK_BAD_POINTER( param2 );
		_CHECK_BAD_SINGLE_PORT_ID( (CAM_Int32s)param1 );
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_NULL );

		((CAM_Size*)param2)->iWidth  = pCameraState->stPortState[(CAM_Int32s)param1].iWidth;
		((CAM_Size*)param2)->iHeight = pCameraState->stPortState[(CAM_Int32s)param1].iHeight;
		break;

	case CAM_CMD_PORT_SET_FORMAT:
		_CHECK_BAD_SINGLE_PORT_ID( (CAM_Int32s)param1 );

		_lock_port( pCameraState, (CAM_Int32s)param1 );
		error = _set_port_format( pCameraState, (CAM_Int32s)param1, (CAM_ImageFormat)param2 );
		_unlock_port( pCameraState, (CAM_Int32s)param1 );
		break;

	case CAM_CMD_PORT_GET_FORMAT:
		_CHECK_BAD_SINGLE_PORT_ID( (CAM_Int32s)param1 );
		_CHECK_BAD_POINTER( param2 );
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_NULL );

		*(CAM_ImageFormat*)param2 = pCameraState->stPortState[(CAM_Int32s)param1].eFormat;
		break;

	case CAM_CMD_PORT_SET_ROTATION:
		_CHECK_BAD_SINGLE_PORT_ID( (CAM_Int32s)param1 );

		_lock_port( pCameraState, (CAM_Int32s)param1 );
		error = _set_port_rotation( pCameraState, (CAM_Int32s)param1, (CAM_FlipRotate)param2 );
		_unlock_port( pCameraState, (CAM_Int32s)param1 );
		break;

	case CAM_CMD_PORT_GET_ROTATION:
		_CHECK_BAD_SINGLE_PORT_ID( (CAM_Int32s)param1 );
		_CHECK_BAD_POINTER( param2 );
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );

		*(CAM_FlipRotate*)param2 = pCameraState->stPortState[(CAM_Int32s)param1].eRotation;
		break;

	////////////////////////////////////////////////////////////////////////////////
	// Preview Settings
	////////////////////////////////////////////////////////////////////////////////
	case CAM_CMD_SET_PREVIEW_FRAMERATE:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );

		pCameraState->iTargetPreviewFpsQ16 = (CAM_Int32u)param1;
		break;

	case CAM_CMD_GET_PREVIEW_FRAMERATE:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );
		_CHECK_BAD_POINTER( param1 );

		*(CAM_Int32u*)param1 = pCameraState->iActualPreviewFpsQ16;
		break;

	case CAM_CMD_SET_PREVIEW_RESIZEFAVOR:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE | CAM_CAPTURESTATE_PREVIEW );
		// XXX
		pCameraState->bPreviewFavorAspectRatio = (CAM_Bool)param1;
		break;

	case CAM_CMD_GET_PREVIEW_RESIZEFAVOR:
		_CHECK_BAD_POINTER( param1 );
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );

		*(CAM_Bool*)param1 = pCameraState->bPreviewFavorAspectRatio;
		break;

	////////////////////////////////////////////////////////////////////////////////
	// Video Settings
	////////////////////////////////////////////////////////////////////////////////

	case CAM_CMD_SET_VIDEO_FRAMERATE:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE | CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_STILL );

		pCameraState->iTargetVideoFpsQ16 = (CAM_Int32u)param1;
		break;

	case CAM_CMD_GET_VIDEO_FRAMERATE:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );
		_CHECK_BAD_POINTER( param1 );

		*(CAM_Int32u*)param1 = pCameraState->iActualVideoFpsQ16;
		break;

	case CAM_CMD_SET_VIDEO_STABLIZER:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );

		if ( pCameraState->stShotModeCap.bSupportVideoStabilizer )
		{
			SET_SHOTMODE_PARAM( pCameraState, CAM_Bool, bVideoStabilizer, param1, error );
		}
		else
		{
			error = CAM_ERROR_NOTSUPPORTEDCMD;
		}
		break;

	case CAM_CMD_GET_VIDEO_STABLIZER:
		_CHECK_BAD_POINTER( param2 );

		*(CAM_Bool*)param1 = stShotParamStatus.bVideoStabilizer;
		break;

	case CAM_CMD_SET_VIDEO_NR:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );

		if ( pCameraState->stShotModeCap.bSupportVideoNoiseReducer )
		{
			SET_SHOTMODE_PARAM( pCameraState, CAM_Bool, bVideoNoiseReducer, param1, error );
		}
		else
		{
			error = CAM_ERROR_NOTSUPPORTEDCMD;
		}
		break;

	case CAM_CMD_GET_VIDEO_NR:
		_CHECK_BAD_POINTER( param1 );

		*(CAM_Bool*)param1 = stShotParamStatus.bVideoNoiseReducer;
		break;

	////////////////////////////////////////////////////////////////////////////////
	// Still Settings
	////////////////////////////////////////////////////////////////////////////////
	case CAM_CMD_SET_JPEGPARAM:
		_CHECK_BAD_POINTER( param1 );
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE | CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_VIDEO );
		
		_lock_port( pCameraState, CAM_PORT_STILL );
		error = _set_jpeg_param( pCameraState, (CAM_JpegParam*)param1 );
		_unlock_port( pCameraState, CAM_PORT_STILL );
		break;

	case CAM_CMD_GET_JPEGPARAM:
		_CHECK_BAD_POINTER( param1 );

		*(CAM_JpegParam*)param1 = pCameraState->stJpegParam;
		break;

	case CAM_CMD_SET_SHUTTERSPEED:
		_CHECK_SUPPORT_RANGE( (CAM_Int32s)param1, pShotModeCap->iMinShutSpdQ16,pShotModeCap->iMaxShutSpdQ16 );
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_STILL & ~CAM_CAPTURESTATE_NULL );

		SET_SHOTMODE_PARAM( pCameraState, CAM_Int32s, iShutterSpeedQ16, param1, error );
		break;

	case CAM_CMD_GET_SHUTTERSPEED:
		_CHECK_BAD_POINTER( param1 );
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );

		*(CAM_Int32s*)param1 = stShotParamStatus.iShutterSpeedQ16;
		break;

	case CAM_CMD_SET_STILL_BURST:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_STILL & ~CAM_CAPTURESTATE_NULL );

		if ( pCameraState->stShotModeCap.bSupportBurstCapture )
		{
			_CHECK_SUPPORT_RANGE( (CAM_Int32s)param1, 1, pCameraState->stShotModeCap.iMaxBurstCnt );
			SET_SHOTMODE_PARAM( pCameraState, CAM_Int32s, iBurstCnt, param1, error );
		}
		else
		{
			error = CAM_ERROR_NOTSUPPORTEDCMD;
		}
		break;

	case CAM_CMD_GET_STILL_BURST:
		_CHECK_BAD_POINTER( param1 );
		*(CAM_Int32s*)param1 = stShotParamStatus.iBurstCnt;
		break;

	case CAM_CMD_SET_STILL_ZSL:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_STILL & ~CAM_CAPTURESTATE_NULL );

		if ( pCameraState->stShotModeCap.bSupportZeroShutterLag )
		{
			SET_SHOTMODE_PARAM( pCameraState, CAM_Bool, bZeroShutterLag, param1, error );
		}
		else
		{
			error = CAM_ERROR_NOTSUPPORTEDCMD;
		}
		break;

	case CAM_CMD_GET_STILL_ZSL:
		_CHECK_BAD_POINTER( param1 );

		*(CAM_Bool*)param1 = stShotParamStatus.bZeroShutterLag;
		break;

	case CAM_CMD_SET_STILL_HDR:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_STILL & ~CAM_CAPTURESTATE_NULL );
		if ( pCameraState->stShotModeCap.bSupportHighDynamicRange )
		{
			SET_SHOTMODE_PARAM( pCameraState, CAM_Bool, bHighDynamicRange, param1, error );
		}
		else
		{
			error = CAM_ERROR_NOTSUPPORTEDCMD;
		}
		break;

	case CAM_CMD_GET_STILL_HDR:
		_CHECK_BAD_POINTER( param1 );

		*(CAM_Bool*)param1 = stShotParamStatus.bHighDynamicRange;
		break;

	////////////////////////////////////////////////////////////////////////////////
	// shooting parameters
	////////////////////////////////////////////////////////////////////////////////

	// shot mode
	case CAM_CMD_QUERY_SHOTMODE_CAP:
		_CHECK_BAD_POINTER( param2 );
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_NULL );
		_CHECK_SUPPORT_ENUM( (CAM_ShotMode)param1, pCameraState->stCamCap.eSupportedShotMode, pCameraState->stCamCap.iSupportedShotModeCnt );

		error = _get_shotmode_capability ( pCameraState->iSensorID, (CAM_ShotMode)param1, (CAM_ShotModeCapability*)param2 );
		break;

	case CAM_CMD_SET_SHOTMODE:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE | CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_VIDEO );
		_CHECK_SUPPORT_ENUM( (CAM_ShotMode)param1, pCameraState->stCamCap.eSupportedShotMode, pCameraState->stCamCap.iSupportedShotModeCnt );

		SET_SHOTMODE_PARAM( pCameraState, CAM_ShotMode, eShotMode, param1, error );

		// update the shot mode capability
		error = _get_shotmode_capability( pCameraState->iSensorID, (CAM_ShotMode)param1, &pCameraState->stShotModeCap );
		ASSERT_CAM_ERROR( error );
		break;

	case CAM_CMD_GET_SHOTMODE:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );
		_CHECK_BAD_POINTER( param1 );

		*(CAM_ShotMode*)param1 = stShotParamStatus.eShotMode;
		break;

	// exposure metering mode
	case CAM_CMD_SET_EXPOSUREMETERMODE:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE | CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_VIDEO );
		_CHECK_SUPPORT_ENUM( (CAM_ExposureMeterMode)param1, pShotModeCap->eSupportedExpMeter, pShotModeCap->iSupportedExpMeterCnt );

		SET_SHOTMODE_PARAM( pCameraState, CAM_ExposureMeterMode, eExpMeterMode, param1, error );
		break;

	case CAM_CMD_GET_EXPOSUREMETERMODE:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );
		_CHECK_BAD_POINTER( param1 );

		*(CAM_ExposureMeterMode*)param1 = stShotParamStatus.eExpMeterMode;
		break;

	case CAM_CMD_SET_EVCOMPENSATION:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE | CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_VIDEO );
		_CHECK_SUPPORT_RANGE( (CAM_Int32s)param1, pShotModeCap->iMinEVCompQ16, pShotModeCap->iMaxEVCompQ16 );

		SET_SHOTMODE_PARAM( pCameraState, CAM_Int32s, iEvCompQ16, param1, error );
		break;

	case CAM_CMD_GET_EVCOMPENSATION:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );
		_CHECK_BAD_POINTER( param1 );

		*(CAM_Int32s*)param1 = stShotParamStatus.iEvCompQ16;
		break;

	case CAM_CMD_SET_ISO:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE | CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_VIDEO );
		_CHECK_SUPPORT_ENUM( (CAM_ISOMode)param1, pShotModeCap->eSupportedIsoMode, pShotModeCap->iSupportedIsoModeCnt );

		SET_SHOTMODE_PARAM( pCameraState, CAM_ISOMode, eIsoMode, param1, error );
		break;

	case CAM_CMD_GET_ISO:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );
		_CHECK_BAD_POINTER( param1 );

		*(CAM_ISOMode*)param1 = stShotParamStatus.eIsoMode;
		break;

	case CAM_CMD_SET_BANDFILTER:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE | CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_VIDEO );
		_CHECK_SUPPORT_ENUM( (CAM_BandFilterMode)param1, pShotModeCap->eSupportedBdFltMode, pShotModeCap->iSupportedBdFltModeCnt );

		SET_SHOTMODE_PARAM( pCameraState, CAM_BandFilterMode, eBandFilterMode, param1, error );
		break;

	case CAM_CMD_GET_BANDFILTER:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );
		_CHECK_BAD_POINTER( param1 );

		*(CAM_BandFilterMode*)param1 = stShotParamStatus.eBandFilterMode;
		break;

	case CAM_CMD_SET_FLASHMODE:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE | CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_VIDEO );
		_CHECK_SUPPORT_ENUM( (CAM_FlashMode)param1, pShotModeCap->eSupportedFlashMode, pShotModeCap->iSupportedFlashModeCnt );

		SET_SHOTMODE_PARAM( pCameraState, CAM_FlashMode, eFlashMode, param1, error );
		break;

	case CAM_CMD_GET_FLASHMODE:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );
		_CHECK_BAD_POINTER( param1 );
		*(CAM_FlashMode*)param1 = stShotParamStatus.eFlashMode;
		break;

	// white balance
	case CAM_CMD_SET_WHITEBALANCEMODE:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE | CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_VIDEO );
		_CHECK_SUPPORT_ENUM( (CAM_WhiteBalanceMode)param1, pShotModeCap->eSupportedWBMode, pShotModeCap->iSupportedWBModeCnt );

		SET_SHOTMODE_PARAM( pCameraState, CAM_WhiteBalanceMode, eWBMode, param1, error );
		break;

	case CAM_CMD_GET_WHITEBALANCEMODE:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );
		_CHECK_BAD_POINTER( param1 );

		*(CAM_WhiteBalanceMode*)param1 = stShotParamStatus.eWBMode;
		break;

	// focus
	case CAM_CMD_SET_FOCUSMODE:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE | CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_VIDEO );
		_CHECK_SUPPORT_ENUM( (CAM_FocusMode)param1, pShotModeCap->eSupportedFocusMode, pShotModeCap->iSupportedFocusModeCnt );

		SET_SHOTMODE_PARAM( pCameraState, CAM_FocusMode, eFocusMode, param1, error );
		break;

	case CAM_CMD_GET_FOCUSMODE:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );
		_CHECK_BAD_POINTER( param1 );

		*(CAM_FocusMode*)param1 = stShotParamStatus.eFocusMode;
		break;

	case CAM_CMD_START_FOCUS:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_VIDEO );
		if ( stShotParamStatus.eFocusMode != CAM_FOCUS_AUTO_ONESHOT_CENTER &&
		     stShotParamStatus.eFocusMode != CAM_FOCUS_AUTO_ONESHOT_TOUCH && 
		     stShotParamStatus.eFocusMode != CAM_FOCUS_MACRO )
		{
			error = CAM_ERROR_NOTSUPPORTEDCMD;
		}
		else if ( pCameraState->stFocusThread.iThreadID != -1 )
		{
			TRACE( CAM_ERROR, "Error: the last auto-focus is not finished yet( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
			error = CAM_ERROR_NOTSUPPORTEDCMD;
		}
		else if ( stShotParamStatus.eFocusMode == CAM_FOCUS_AUTO_ONESHOT_CENTER || stShotParamStatus.eFocusMode == CAM_FOCUS_AUTO_ONESHOT_TOUCH )
		{
			error = SmartSensor_StartFocus( pCameraState->hSensorHandle, param1 );
			if ( error == CAM_ERROR_NONE )
			{
				_CreateThread( &(pCameraState->stFocusThread), CheckFocusThread, pCameraState );
			}
		}
		else
		{
			SEND_EVENT( pCameraState, CAM_EVENT_FOCUS_AUTO_STOP, CAM_TRUE );
		}
		break;

	case CAM_CMD_CANCEL_FOCUS:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_VIDEO );

		if ( stShotParamStatus.eFocusMode != CAM_FOCUS_AUTO_ONESHOT_CENTER &&
		     stShotParamStatus.eFocusMode != CAM_FOCUS_AUTO_ONESHOT_TOUCH &&
		     stShotParamStatus.eFocusMode != CAM_FOCUS_MACRO )
		{
			error = CAM_ERROR_NOTSUPPORTEDCMD;
		}
		else if ( stShotParamStatus.eFocusMode == CAM_FOCUS_AUTO_ONESHOT_CENTER || stShotParamStatus.eFocusMode == CAM_FOCUS_AUTO_ONESHOT_TOUCH )
		{
			if ( pCameraState->stFocusThread.iThreadID != -1 )
			{
				_DestroyThread( &(pCameraState->stFocusThread) );
				error = SmartSensor_CancelFocus( pCameraState->hSensorHandle );
			}
			else
			{
				TRACE( CAM_WARN, "Warning: no focus need to be canceled( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
			}
		}
		break;

	// zoom
	case CAM_CMD_SET_DIGZOOM:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE | CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_VIDEO );
		_CHECK_SUPPORT_RANGE( (CAM_Int32s)param1, pCameraState->stShotModeCap.iMinDigZoomQ16, pCameraState->stShotModeCap.iMaxDigZoomQ16 );

		_lock_port( pCameraState, CAM_PORT_ANY );
		error = _set_digital_zoom( pCameraState, (CAM_Int32s)param1 );
		_unlock_port( pCameraState, CAM_PORT_ANY );
		break;

	case CAM_CMD_GET_DIGZOOM:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );
		_CHECK_BAD_POINTER( param1 );

		*(CAM_Int32s*)param1 = pCameraState->iDigitalZoomQ16;
		break;

	// digital special effect
	case CAM_CMD_SET_COLOREFFECT:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE | CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_VIDEO );
		_CHECK_SUPPORT_ENUM( (CAM_ColorEffect)param1, pShotModeCap->eSupportedColorEffect, pShotModeCap->iSupportedColorEffectCnt );

		// set sensor 
		SET_SHOTMODE_PARAM( pCameraState, CAM_ColorEffect, eColorEffect, param1, error );
		break;

	case CAM_CMD_GET_COLOREFFECT:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );
		_CHECK_BAD_POINTER( param1 );

		*(CAM_ColorEffect*)param1 = stShotParamStatus.eColorEffect;
		break;

	// vision effect
	case CAM_CMD_SET_BRIGHTNESS:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE | CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_VIDEO );
		_CHECK_SUPPORT_RANGE( (CAM_Int32s)param1, pShotModeCap->iMinBrightness, pShotModeCap->iMaxBrightness );

		SET_SHOTMODE_PARAM( pCameraState, CAM_Int32s, iBrightness, param1, error );
		break;

	case CAM_CMD_GET_BRIGHTNESS:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );
		_CHECK_BAD_POINTER( param1 );

		*(CAM_Int32s*)param1 = stShotParamStatus.iBrightness;
		break;

	case CAM_CMD_SET_CONTRAST:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE | CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_VIDEO );
		_CHECK_SUPPORT_RANGE( (CAM_Int32s)param1, pShotModeCap->iMinContrast, pShotModeCap->iMaxContrast );

		SET_SHOTMODE_PARAM( pCameraState, CAM_Int32s, iContrast, param1, error );
		break;

	case CAM_CMD_GET_CONTRAST:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );
		_CHECK_BAD_POINTER( param1 );

		*(CAM_Int32s*)param1 = stShotParamStatus.iContrast;
		break;

	case CAM_CMD_SET_SATURATION:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE | CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_VIDEO );
		_CHECK_SUPPORT_RANGE( (CAM_Int32s)param1, pShotModeCap->iMinSaturation, pShotModeCap->iMaxSaturation );

		SET_SHOTMODE_PARAM( pCameraState, CAM_Int32s, iSaturation, param1, error );
		break;

	case CAM_CMD_GET_SATURATION:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );
		_CHECK_BAD_POINTER( param1 );

		*(CAM_Int32s*)param1 = stShotParamStatus.iSaturation;
		break;

	case CAM_CMD_SET_SHARPNESS:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE | CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_VIDEO );
		_CHECK_SUPPORT_RANGE( (CAM_Int32s)param1, pShotModeCap->iMinSharpness, pShotModeCap->iMaxSharpness );

		SET_SHOTMODE_PARAM( pCameraState, CAM_Int32s, iSharpness, param1, error );
		break;

	case CAM_CMD_GET_SHARPNESS:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );
		_CHECK_BAD_POINTER( param1 );

		*(CAM_Int32s*)param1 = stShotParamStatus.iSharpness;
		break;

	case CAM_CMD_SET_OPTZOOM:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE | CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_VIDEO );
		_CHECK_SUPPORT_RANGE( (CAM_Int32s)param1, pShotModeCap->iMinOptZoomQ16, pShotModeCap->iMaxOptZoomQ16 );

		SET_SHOTMODE_PARAM( pCameraState, CAM_Int32s, iOptZoomQ16, param1, error );
		break;

	case CAM_CMD_GET_OPTZOOM:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );
		_CHECK_BAD_POINTER( param1 );

		*(CAM_Int32u*)param1 = stShotParamStatus.iOptZoomQ16;
		break;

	case CAM_CMD_SET_FNUM:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE | CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_VIDEO );
		_CHECK_SUPPORT_RANGE( (CAM_Int32s)param1, pShotModeCap->iMinFNumQ16, pShotModeCap->iMaxFNumQ16 );

		SET_SHOTMODE_PARAM( pCameraState, CAM_Int32s, iFNumQ16, param1, error );
		break;

	case CAM_CMD_GET_FNUM:
		_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );

		*(CAM_Int32s*)param1 = stShotParamStatus.iFNumQ16;
		break;

	case CAM_CMD_RESET_CAMERA:
		_CHECK_SUPPORT_RANGE( (CAM_Int32s)param1, CAM_RESET_FAST, CAM_RESET_COMPLETE );

		error = _reset_camera( pCameraState, (CAM_Int32s)param1 );
		break;

	default:
		TRACE( CAM_ERROR, "Error: Unrecognized command id[%d]( %s, %d )\n", cmd, __FILE__, __LINE__ );
		error = CAM_ERROR_NOTSUPPORTEDCMD;
		break;
	}

	return error;
}

static CAM_Error _set_sensor_id( _CAM_ExtIspState *pCameraState, CAM_Int32s iSensorID )
{
	CAM_Error      error = CAM_ERROR_NONE;
	CAM_Int32s     i = 0, ret = -1;
	_CAM_ShotParam stDefaultShotParam;

	_CHECK_SUPPORT_RANGE( iSensorID, 0, pCameraState->iSensorCount - 1 );

	// sensor ID can only be changed when all ports are configurable
	if ( !_is_configurable_port( pCameraState, CAM_PORT_PREVIEW ) ||\
	     !_is_configurable_port( pCameraState, CAM_PORT_VIDEO ) ||\
	     !_is_configurable_port( pCameraState, CAM_PORT_STILL ) )
	{
		TRACE( CAM_ERROR, "Error: port is not configurable, pls check if there is buffer pushed into any port( %s, %s, %d )\n", 
		       __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_PORTNOTCONFIGUREABLE;
	}

#if 0

	// only in NULL state that sensor id can switch
	_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_NULL );
	ASSERT( pCameraState->iSensorID == -1 );

#else

	// only in NULL/IDLE state that sensor id can switch
	_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_NULL | CAM_CAPTURESTATE_IDLE );

	// if change sensor ID, first deinit obsolete sensor node
	if ( pCameraState->eCaptureState == CAM_CAPTURESTATE_IDLE )
	{
		ASSERT( pCameraState->iSensorID != -1 );

		if ( pCameraState->iSensorID == iSensorID )
		{
			return CAM_ERROR_NONE;
		}
		else
		{
			error = SmartSensor_Deinit( &pCameraState->hSensorHandle );
			ASSERT_CAM_ERROR( error );

			// here we will flush all buffers assigned to the old sensor in all ports, so client should aware that, he can
			// handle those buffers after switch sensor ID. A good practice is that client flush all ports themselves.
			for ( i = CAM_PORT_PREVIEW; i <= CAM_PORT_STILL; i++ )
			{
				ret = _FlushQueue( &pCameraState->stPortState[i].qEmpty );
				ASSERT( ret == 0 );
				ret = _FlushQueue( &pCameraState->stPortState[i].qFilled );
				ASSERT( ret == 0 );
			}

		}
	}
#endif

	// init the new sensor handle
	error = SmartSensor_Init( &(pCameraState->hSensorHandle), iSensorID );
	ASSERT_CAM_ERROR( error );
	
	// register event handler
	error = SmartSensor_RegisterEventHandler( pCameraState->hSensorHandle, pCameraState->fnEventHandler, pCameraState->pUserData );
	ASSERT_CAM_ERROR( error );
	
	pCameraState->iSensorID = iSensorID;

	// get camera capability under current sensor 
	error = _get_camera_capability( pCameraState, iSensorID, &pCameraState->stCamCap );
	ASSERT_CAM_ERROR( error );

	// set default port attribute
	for ( i = CAM_PORT_PREVIEW; i <= CAM_PORT_STILL; i++ )
	{
		pCameraState->stPortState[i].iWidth  = pCameraState->stCamCap.stPortCapability[i].stMin.iWidth;
		pCameraState->stPortState[i].iHeight = pCameraState->stCamCap.stPortCapability[i].stMin.iHeight;
		pCameraState->stPortState[i].eFormat = pCameraState->stCamCap.stPortCapability[i].eSupportedFormat[0];
	}

	// set default video attributes
	pCameraState->iTargetVideoFpsQ16   = ( 30 << 16 );
	pCameraState->iActualVideoFpsQ16   = ( 0 << 16 );

	// set default still attributes
	pCameraState->iStillRestCount      = 0;

	// set default preview attributes
	pCameraState->bPreviewFavorAspectRatio = CAM_FALSE;
	pCameraState->iTargetPreviewFpsQ16     = ( 30 << 16 );
	pCameraState->iActualPreviewFpsQ16     = ( 0 << 16 );	

	// set default JPEG params
	pCameraState->stJpegParam.iSampling      = 1; // 0 - 420, 1 - 422, 2 - 444
	pCameraState->stJpegParam.iQualityFactor = 80;
	pCameraState->stJpegParam.bEnableExif    = CAM_FALSE;
	pCameraState->stJpegParam.bEnableThumb   = CAM_FALSE;
	pCameraState->stJpegParam.iThumbWidth    = 0;
	pCameraState->stJpegParam.iThumbHeight   = 0;

	// by default, we will set shot mode as AUTO
	error = SmartSensor_GetShotParam( pCameraState->hSensorHandle, &stDefaultShotParam );

	stDefaultShotParam.eShotMode = CAM_SHOTMODE_AUTO;
	error = SmartSensor_SetShotParam( pCameraState->hSensorHandle, &stDefaultShotParam );
	ASSERT_CAM_ERROR( error );

	error = _get_shotmode_capability( pCameraState->iSensorID, stDefaultShotParam.eShotMode, &(pCameraState->stShotModeCap) );
	ASSERT_CAM_ERROR( error );

	// by default, focus not open
	pCameraState->bIsFocusOpen       = CAM_FALSE;

	// initialize digital zoom param
	pCameraState->iPpuDigitalZoomQ16 = ( 1 << 16 );
	pCameraState->iDigitalZoomQ16    = ( 1 << 16 );

	pCameraState->eCaptureState = CAM_CAPTURESTATE_IDLE;

	// init port buffer requirement
	for ( i = CAM_PORT_PREVIEW; i <= CAM_PORT_STILL; i++ )
	{
		error = _get_port_bufreq( pCameraState, i, &pCameraState->stPortState[i].stBufReq  );
		ASSERT_CAM_ERROR( error );
	}

	return error;
}

static CAM_Error _get_camera_capability( _CAM_ExtIspState *pCameraState, CAM_Int32s iSensorID, CAM_CameraCapability *pCameraCap )
{
	CAM_Int32s                 i, index;
	CAM_Error                  error = CAM_ERROR_NONE;
	CAM_ImageFormat            eFormat;
	_CAM_SmartSensorCapability stSensorCap;
	CAM_PortCapability         *pPortCap        = NULL;
	CAM_Int32s                 iPpuFmtCnt       = 0;
	CAM_Bool                   bIsArbitrarySize = CAM_FALSE; 
	CAM_ImageFormat            stPpuFmtCap[CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT] = {0};

	_CHECK_BAD_POINTER( pCameraState );
	_CHECK_BAD_POINTER( pCameraCap );

	memset( pCameraCap, 0, sizeof( CAM_CameraCapability ) );

	_CHECK_SUPPORT_RANGE( iSensorID, 0, pCameraState->iSensorCount - 1 );

	_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY );

	error = SmartSensor_GetCapability( iSensorID, &stSensorCap );
	ASSERT_CAM_ERROR( error );

	// supported shot mode
	for ( i = 0; i < stSensorCap.iSupportedShotModeCnt; i++ ) 
	{
		pCameraCap->eSupportedShotMode[i] = stSensorCap.eSupportedShotMode[i];
	}
	pCameraCap->iSupportedShotModeCnt = stSensorCap.iSupportedShotModeCnt;

	// all supported shot parameters
	pCameraCap->stSupportedShotParams = stSensorCap.stSupportedShotParams;
	// add on ppu digital zoom capability
	if ( stSensorCap.stSupportedShotParams.iMaxDigZoomQ16 == stSensorCap.stSupportedShotParams.iMinDigZoomQ16 )
	{
		pCameraCap->stSupportedShotParams.iDigZoomStepQ16 = (CAM_Int32s)(0.2 * 65536 + 0.5);
	}
	if ( stSensorCap.stSupportedShotParams.iMaxDigZoomQ16 < BUILD_OPTION_STRATEGY_MAX_DIGITALZOOM_Q16 )
	{
		pCameraCap->stSupportedShotParams.bSupportSmoothDigZoom = CAM_FALSE;
	}
	pCameraCap->stSupportedShotParams.iMinDigZoomQ16 = ( 1 << 16 );
	pCameraCap->stSupportedShotParams.iMaxDigZoomQ16 = BUILD_OPTION_STRATEGY_MAX_DIGITALZOOM_Q16;


	/* preview port capability */
	// size
	pPortCap = &pCameraCap->stPortCapability[CAM_PORT_PREVIEW];

	bIsArbitrarySize = stSensorCap.bArbitraryVideoSize;
	if ( bIsArbitrarySize )
	{
		// support arbitrary size by sensor
		pPortCap->stMin = stSensorCap.stMinVideoSize;
		pPortCap->stMax = stSensorCap.stMaxVideoSize;

		pPortCap->bArbitrarySize = bIsArbitrarySize;
	}
	else
	{
		CAM_Size stMinSize = { 0, 0 }, stMaxSize = { 0, 0 }; 
		
		error = _ppu_query_resizer_cap( &bIsArbitrarySize, &stMinSize, &stMaxSize );
		ASSERT_CAM_ERROR( error );

		pPortCap->bArbitrarySize = bIsArbitrarySize;

		if ( pPortCap->bArbitrarySize )
		{
			// if ppu support arbitrary size
			pPortCap->stMin = stMinSize;  
			pPortCap->stMax = stMaxSize;
		}
		else	
		{
			// if ppu do not support arbitrary size
			pPortCap->stMin = stSensorCap.stMinVideoSize;
			pPortCap->stMax = stSensorCap.stMaxVideoSize;
		}

		// natively supported sizes by sensor, suppose has better performance
		pPortCap->iSupportedSizeCnt = stSensorCap.iSupportedVideoSizeCnt;
		for ( i = 0; i < pPortCap->iSupportedSizeCnt; i++ ) 
		{
			pPortCap->stSupportedSize[i] = stSensorCap.stSupportedVideoSize[i];
		}
	}

	// format
	error = _ppu_query_csc_cap( stSensorCap.eSupportedVideoFormat, stSensorCap.iSupportedVideoFormatCnt, stPpuFmtCap, &iPpuFmtCnt );
	ASSERT_CAM_ERROR( error );

	for ( i = 0; i < stSensorCap.iSupportedVideoFormatCnt; i++ )
	{
		pPortCap->eSupportedFormat[i] = stSensorCap.eSupportedVideoFormat[i];
	}
	for ( i = stSensorCap.iSupportedVideoFormatCnt; i < _MIN( ( iPpuFmtCnt + stSensorCap.iSupportedVideoFormatCnt ), CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT ); i++ )
	{
		pPortCap->eSupportedFormat[i] = stPpuFmtCap[i - stSensorCap.iSupportedVideoFormatCnt];
	}
	pPortCap->iSupportedFormatCnt = _MIN( ( iPpuFmtCnt + stSensorCap.iSupportedVideoFormatCnt ), CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT );

	// rotation
	error = _ppu_query_rotator_cap( pPortCap->eSupportedRotate, &(pPortCap->iSupportedRotateCnt) );
	ASSERT_CAM_ERROR( error );

	/* video port capability */
	// FIXME: need to add ppu to video port
	pPortCap = &pCameraCap->stPortCapability[CAM_PORT_VIDEO];

	// size
	pPortCap->bArbitrarySize  = stSensorCap.bArbitraryVideoSize;
	pPortCap->stMin.iWidth    = stSensorCap.stMinVideoSize.iWidth;
	pPortCap->stMin.iHeight   = stSensorCap.stMinVideoSize.iHeight;
	pPortCap->stMax.iWidth    = stSensorCap.stMaxVideoSize.iWidth;
	pPortCap->stMax.iHeight   = stSensorCap.stMaxVideoSize.iHeight;

	if ( !pPortCap->bArbitrarySize )
	{
		pPortCap->iSupportedSizeCnt = stSensorCap.iSupportedVideoSizeCnt;
		for ( i = 0; i < pPortCap->iSupportedSizeCnt; i++ ) 
		{
			pPortCap->stSupportedSize[i] = stSensorCap.stSupportedVideoSize[i];
		}
	}

	// format 
	pPortCap->iSupportedFormatCnt = stSensorCap.iSupportedVideoFormatCnt;
	for ( i = 0; i < pPortCap->iSupportedFormatCnt; i++ ) 
	{
		pPortCap->eSupportedFormat[i] = stSensorCap.eSupportedVideoFormat[i];
	}

	// rotate
	pPortCap->iSupportedRotateCnt = 1;
	pPortCap->eSupportedRotate[0] = CAM_FLIPROTATE_NORMAL;


	/* still port capability */
	pPortCap = &pCameraCap->stPortCapability[CAM_PORT_STILL];

	// size
	pPortCap->bArbitrarySize  = stSensorCap.bArbitraryStillSize;
	pPortCap->stMin.iWidth    = stSensorCap.stMinStillSize.iWidth;
	pPortCap->stMin.iHeight   = stSensorCap.stMinStillSize.iHeight;
	pPortCap->stMax.iWidth    = stSensorCap.stMaxStillSize.iWidth;
	pPortCap->stMax.iHeight   = stSensorCap.stMaxStillSize.iHeight;

	if ( !pPortCap->bArbitrarySize )
	{
		pPortCap->iSupportedSizeCnt = stSensorCap.iSupportedStillSizeCnt;
		for ( i = 0; i < pPortCap->iSupportedSizeCnt; i++ ) 
		{
			pPortCap->stSupportedSize[i] = stSensorCap.stSupportedStillSize[i];
		}
	}

	// format
	if ( stSensorCap.stSupportedJPEGParams.bSupportJpeg )
	{
		// sensor originally support JPEG
		pPortCap->iSupportedFormatCnt = stSensorCap.iSupportedStillFormatCnt;
		for ( i = 0; i < pPortCap->iSupportedFormatCnt; i++ ) 
		{
			pPortCap->eSupportedFormat[i] = stSensorCap.eSupportedStillFormat[i];
		}

		pCameraCap->stSupportedJPEGParams = stSensorCap.stSupportedJPEGParams;
	}
	else
	{
		CAM_JpegCapability stJpegEncCap;

		error = _ppu_query_jpegenc_cap( stSensorCap.eSupportedStillFormat, stSensorCap.iSupportedStillFormatCnt, &stJpegEncCap );
		ASSERT_CAM_ERROR( error );

		if ( stJpegEncCap.bSupportJpeg )
		{
			pPortCap->iSupportedFormatCnt = 1;
			pPortCap->eSupportedFormat[0] = CAM_IMGFMT_JPEG;
			TRACE( CAM_INFO, "Info: sensor[%d] don't support JPEG format, will enable Camera Engine's embeded JPEG compressor...\n", iSensorID );
		}
		else
		{
			pPortCap->iSupportedFormatCnt = stSensorCap.iSupportedStillFormatCnt;
			for ( i = 0; i < pPortCap->iSupportedFormatCnt; i++ ) 
			{
				pPortCap->eSupportedFormat[i] = stSensorCap.eSupportedStillFormat[i];
			}
			TRACE( CAM_WARN, "Warning: No suitable JPEG compressor for sensor[%d]'s output( %s, %d ) \n", iSensorID, __FUNCTION__, __LINE__ );
		}

		pCameraCap->stSupportedJPEGParams = stJpegEncCap;
	}

	// rotation
	error = _ppu_query_rotator_cap( pPortCap->eSupportedRotate, &(pPortCap->iSupportedRotateCnt) );
	ASSERT_CAM_ERROR( error );

	// sensor rotation capability, generally speaking, sensor only support flip/mirror
	pCameraCap->iSupportedSensorRotateCnt = stSensorCap.iSupportedRotateCnt;
	for ( i = 0; i < pCameraCap->iSupportedSensorRotateCnt; i++ ) 
	{
		pCameraCap->eSupportedSensorRotate[i] = stSensorCap.eSupportedRotate[i];
	}

	return CAM_ERROR_NONE;
}


static CAM_Error _get_shotmode_capability( CAM_Int32s iSensorId, CAM_ShotMode eShotMode, CAM_ShotModeCapability *pShotModeCap )
{
	CAM_Error error = CAM_ERROR_NONE;

	error = SmartSensor_GetShotModeCapability( iSensorId, eShotMode, pShotModeCap );
	ASSERT_CAM_ERROR( error );

	// XXX
	if ( pShotModeCap->iMinDigZoomQ16 == pShotModeCap->iMaxDigZoomQ16 && pShotModeCap->iMaxDigZoomQ16 == (1 << 16) )
	{
		pShotModeCap->iDigZoomStepQ16 = (CAM_Int32s)(0.2 * 65536 + 0.5);
	}
	if ( pShotModeCap->iMaxDigZoomQ16 < BUILD_OPTION_STRATEGY_MAX_DIGITALZOOM_Q16 )
	{
		pShotModeCap->bSupportSmoothDigZoom = CAM_FALSE;
	}
	pShotModeCap->iMinDigZoomQ16 = ( 1 << 16 );
	pShotModeCap->iMaxDigZoomQ16 = BUILD_OPTION_STRATEGY_MAX_DIGITALZOOM_Q16;
	 
	return error;
}


// get buffer requirement of given port
static CAM_Error _get_port_bufreq( _CAM_ExtIspState *pCameraState, CAM_Int32s iPortId, CAM_ImageBufferReq *pBufReq )
{
	CAM_Error              error          = CAM_ERROR_NONE;
	_CAM_SmartSensorConfig stSensorConfig;

	// bad argument check
	_CHECK_BAD_POINTER( pCameraState );
	_CHECK_BAD_POINTER( pBufReq );
	
	_CHECK_BAD_SINGLE_PORT_ID( iPortId );

	_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );

	if ( !_is_valid_port( pCameraState, iPortId ) )
	{
		TRACE( CAM_ERROR, "Error: for sensor[%d], port[%d] is invalid( %s, %s, %d )\n", 
		       pCameraState->iSensorID, iPortId, __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_PORTNOTVALID;
	}

	// the acceptable and optimal port buffer property depend on if HW or SW is used to generate the data
	if ( iPortId == CAM_PORT_PREVIEW )
	{
		error = _get_required_sensor_settings( pCameraState, CAM_CAPTURESTATE_PREVIEW, &stSensorConfig );
		ASSERT_CAM_ERROR( error );
	}
	else if ( iPortId == CAM_PORT_VIDEO )
	{
		error = _get_required_sensor_settings( pCameraState, CAM_CAPTURESTATE_VIDEO, &stSensorConfig );
		ASSERT_CAM_ERROR( error );
	}
	else if ( iPortId == CAM_PORT_STILL )
	{
		error = _get_required_sensor_settings( pCameraState, CAM_CAPTURESTATE_STILL, &stSensorConfig );
		ASSERT_CAM_ERROR( error );
	}

	// don't use private buffer, get buffer requirement from sensor hal directly
	if ( stSensorConfig.iWidth == pCameraState->stPortState[iPortId].iWidth &&  stSensorConfig.iHeight == pCameraState->stPortState[iPortId].iHeight &&
	     stSensorConfig.eFormat == pCameraState->stPortState[iPortId].eFormat && pCameraState->stPortState[iPortId].eRotation == CAM_FLIPROTATE_NORMAL )
	{
		error = SmartSensor_GetBufReq( pCameraState->hSensorHandle, &stSensorConfig, pBufReq );
		ASSERT_CAM_ERROR( error );
	}
	else  // get buffer requirement from ppu
	{	
		_CAM_ImageInfo stImgInfo;

		stImgInfo.iWidth    = pCameraState->stPortState[iPortId].iWidth;
		stImgInfo.iHeight   = pCameraState->stPortState[iPortId].iHeight;
		stImgInfo.eFormat   = pCameraState->stPortState[iPortId].eFormat;
		stImgInfo.eRotation = pCameraState->stPortState[iPortId].eRotation;
		if ( stImgInfo.eFormat == CAM_IMGFMT_JPEG )
		{
			stImgInfo.pJpegParam = &pCameraState->stJpegParam;
		}
		else
		{
			stImgInfo.pJpegParam = NULL;
		}

		error = _ppu_get_bufreq( pCameraState->hPpuHandle, &stImgInfo, pBufReq );
		ASSERT_CAM_ERROR( error );
	}

	return error;
}


// enqueue buffer to given port
static CAM_Error _enqueue_buffer( _CAM_ExtIspState *pCameraState, CAM_Int32s iPortId, CAM_ImageBuffer *pBuf )
{
	CAM_Int32s         ret         = 0;
	CAM_Error          error       = CAM_ERROR_NONE;
	_CAM_PortState     *pPortState = NULL;

	_CHECK_BAD_POINTER( pCameraState );
	_CHECK_BAD_POINTER( pBuf );
	
	_CHECK_BAD_SINGLE_PORT_ID( iPortId );

	_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );

	if ( !_is_valid_port( pCameraState, iPortId ) )
	{
		TRACE( CAM_ERROR, "Error: port[%d] is invalid( %s, %s, %d )\n", iPortId, __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_PORTNOTVALID;
	}

	pPortState = &( pCameraState->stPortState[iPortId] );
	
	error = _check_image_buffer( pBuf, pPortState );
	if ( error != CAM_ERROR_NONE )
	{
		if ( error == CAM_WARN_DUPLICATEBUFFER )
		{
			TRACE( CAM_WARN, "Warning: enqueue a same buffer to port[%d], this enqueue will be taken as invalid to camera-engine, pls check\
			       ( %s, %s, %d )\n", iPortId, __FILE__, __FUNCTION__, __LINE__ );
		}
		return error;
	}

	/* enqueue buffer to the corresponding port
	 * whether to enqueue buffers to sensor depends on the port id and the current state
	 * sensor enqueue is triggered by preview port when the current state is preview
	 * sensor enqueue is triggered by video port when the current state is video
	 * sensor enqueue is triggered by still port when the current state is still
	 */
	ret = _EnQueue( &pCameraState->stPortState[iPortId].qEmpty, (void*)pBuf );
	if ( ret != 0 )
	{
		TRACE( CAM_ERROR, "Error: port[%d] is too full to accept a new buffer ( %s, %s, %d )\n", iPortId, __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_OUTOFRESOURCE;
	}

	if ( pCameraState->eCaptureState == CAM_CAPTURESTATE_PREVIEW && iPortId == CAM_PORT_PREVIEW )
	{
		if ( pCameraState->bNeedPostProcessing )
		{
			CAM_ImageBuffer *pTmpImg = NULL;
			error = _request_ppu_srcimg( pCameraState, &pCameraState->stCurrentSensorBufReq, pBuf, &pTmpImg );
			if ( error != CAM_ERROR_NONE )
			{
				TRACE( CAM_ERROR, "Error: no memory available to afford private buffer ( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );

				ret = _RemoveQueue( &pCameraState->stPortState[iPortId].qEmpty, (void*)pBuf );
				ASSERT( ret == 0 );
			}
			else
			{
				error = SmartSensor_Enqueue( pCameraState->hSensorHandle, pTmpImg );

				if ( error != CAM_ERROR_NONE )
				{
					TRACE( CAM_ERROR, "Error: sensor hal is too full to accept a new buffer ( %s, %s, %d )\n", 
					       __FILE__, __FUNCTION__, __LINE__ );

					ret = _RemoveQueue( &pCameraState->stPortState[iPortId].qEmpty, (void*)pBuf );
					ASSERT( ret == 0 );

					error = _release_ppu_srcimg( pCameraState, pTmpImg );
					ASSERT_CAM_ERROR( error );
				}
			}
		}
		else 
		{
			error = SmartSensor_Enqueue( pCameraState->hSensorHandle, pBuf );

			if ( error != CAM_ERROR_NONE ) 
			{
				ret = _RemoveQueue( &pCameraState->stPortState[iPortId].qEmpty, (void*)pBuf );
				ASSERT( ret == 0 );
			}
		}
	}
	else if ( pCameraState->eCaptureState == CAM_CAPTURESTATE_VIDEO && iPortId == CAM_PORT_VIDEO )
	{
		error = SmartSensor_Enqueue( pCameraState->hSensorHandle, pBuf );
		ASSERT_CAM_ERROR( error );
	}
	else if ( pCameraState->eCaptureState == CAM_CAPTURESTATE_STILL && iPortId == CAM_PORT_STILL )
	{
		// in still state, enqueue to driver only when we need more images
		if ( pCameraState->iStillRestCount > 0 )	
		{
			if ( pCameraState->bNeedPostProcessing )
			{
				CAM_ImageBuffer *pTmpImg = NULL;
				error = _request_ppu_srcimg( pCameraState, &pCameraState->stCurrentSensorBufReq, pBuf, &pTmpImg );
				if ( error != CAM_ERROR_NONE )
				{
					TRACE( CAM_ERROR, "Error: no memory available to afford private buffer ( %s, %s, %d )\n", 
					       __FILE__, __FUNCTION__, __LINE__ );

					ret = _RemoveQueue( &pCameraState->stPortState[iPortId].qEmpty, (void*)pBuf );
					ASSERT( ret == 0 );
				}
				else
				{
					error = SmartSensor_Enqueue( pCameraState->hSensorHandle, pTmpImg );

					if ( error != CAM_ERROR_NONE )
					{
						CAM_Error err;
						TRACE( CAM_ERROR, "Error: sensor hal is too full to accept a new buffer ( %s, %s, %d )\n", 
						       __FILE__, __FUNCTION__, __LINE__ );

						ret = _RemoveQueue( &pCameraState->stPortState[iPortId].qEmpty, (void*)pBuf );
						ASSERT( ret == 0 );

						err = _release_ppu_srcimg( pCameraState, pTmpImg );
						ASSERT_CAM_ERROR( err );
					}
				}
			}
			else
			{
				error = SmartSensor_Enqueue( pCameraState->hSensorHandle, pBuf );
				if ( error != CAM_ERROR_NONE )
				{
					TRACE( CAM_ERROR, "Error: sensor hal is too full to accept a new buffer ( %s, %s, %d )\n", 
					       __FILE__, __FUNCTION__, __LINE__ );

					ret = _RemoveQueue( &pCameraState->stPortState[iPortId].qEmpty, (void*)pBuf );
					ASSERT( ret == 0 );
				}
			}
		}	
	}

	return error;
}

static CAM_Error _dequeue_buffer( _CAM_ExtIspState *pCameraState, CAM_Int32s *pPortId, CAM_ImageBuffer **ppBuf )
{
	CAM_Error  error   = CAM_ERROR_NONE;
	CAM_Int32s iPortId = *pPortId;

	_CHECK_BAD_POINTER( pCameraState );
	_CHECK_BAD_POINTER( pPortId );
	_CHECK_BAD_POINTER( ppBuf );

	_CHECK_BAD_ANY_PORT_ID( *pPortId );

	// allow dequeue at IDLE state to get the images generated in previous dequeue
	_CHECK_STATE_LIMIT( pCameraState->eCaptureState, ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );	

	*ppBuf = NULL;

	error = _dequeue_filled( pCameraState, &iPortId, ppBuf );
    
	// if buffer is not ready, but port is active, so we need to run one frame from driver directly
	if ( error == CAM_ERROR_NONE && *ppBuf == NULL )
	{
		error = _run_one_frame( pCameraState );
		if ( error != CAM_ERROR_NONE )
		{
			return error;
		}
        
		iPortId = *pPortId ;
		error = _dequeue_filled( pCameraState, &iPortId, ppBuf );
		ASSERT( error != CAM_ERROR_NONE || *ppBuf != NULL );
	}

	 *pPortId = iPortId;

	return error;
}

static CAM_Error _flush_buffer( _CAM_ExtIspState *pCameraState, CAM_Int32s iPortId )
{
	CAM_Error  error = CAM_ERROR_NONE;

	_CHECK_BAD_POINTER( pCameraState );
	_CHECK_BAD_ANY_PORT_ID( iPortId );

	_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );

	if ( iPortId == CAM_PORT_ANY )
	{
		CAM_Int32s i = 0;
		if ( pCameraState->eCaptureState != CAM_CAPTURESTATE_IDLE && pCameraState->eCaptureState != CAM_CAPTURESTATE_NULL )
		{
			error = SmartSensor_Flush( pCameraState->hSensorHandle );
			ASSERT_CAM_ERROR( error );

			if ( pCameraState->bNeedPostProcessing )
			{
				error = _flush_all_ppu_srcimg( pCameraState );
				ASSERT_CAM_ERROR( error );
			}
		}
		for ( i = CAM_PORT_PREVIEW; i <= CAM_PORT_STILL; i++ )
		{
			_FlushQueue( &pCameraState->stPortState[i].qEmpty );
			_FlushQueue( &pCameraState->stPortState[i].qFilled );
		}
	}
	else 
	{
		if ( ( pCameraState->eCaptureState == CAM_CAPTURESTATE_PREVIEW && iPortId == CAM_PORT_PREVIEW ) ||
		     ( pCameraState->eCaptureState == CAM_CAPTURESTATE_VIDEO   && iPortId == CAM_PORT_VIDEO ) ||
		     ( pCameraState->eCaptureState == CAM_CAPTURESTATE_STILL   && iPortId == CAM_PORT_STILL ) )
		{
			error = SmartSensor_Flush( pCameraState->hSensorHandle );
			ASSERT_CAM_ERROR( error );

			if ( pCameraState->bNeedPostProcessing )
			{
				error = _flush_all_ppu_srcimg( pCameraState );
				ASSERT_CAM_ERROR( error );
			}
		}

		_FlushQueue( &pCameraState->stPortState[iPortId].qEmpty );
		_FlushQueue( &pCameraState->stPortState[iPortId].qFilled );
	}

	return error;
}

static CAM_Error _set_port_size( _CAM_ExtIspState *pCameraState, CAM_Int32s iPortId, CAM_Size *pSize )
{
	CAM_PortCapability *pPortCap = NULL;
	CAM_Error          error     = CAM_ERROR_NONE;

	_CHECK_BAD_POINTER( pCameraState );
	_CHECK_BAD_POINTER( pSize );

	_CHECK_BAD_SINGLE_PORT_ID( iPortId );

	_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );

	if ( !_is_configurable_port( pCameraState, iPortId ) )
	{
		TRACE( CAM_ERROR, "Error: port[%d] is not configurable( %s, %s, %d )\n", iPortId, __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_PORTNOTCONFIGUREABLE;
	}

	pPortCap = &pCameraState->stCamCap.stPortCapability[iPortId];

	// check if this size is acceptable
	if ( pPortCap->bArbitrarySize )
	{
		if ( !( pSize->iWidth >= pPortCap->stMin.iWidth && pSize->iHeight >= pPortCap->stMin.iHeight &&
		        pSize->iWidth <= pPortCap->stMax.iWidth && pSize->iHeight <= pPortCap->stMax.iHeight )
		   )
		{
			TRACE( CAM_ERROR, "Error: image size(%d, %d) for port[%d] is not supported (%d, %d) - (%d, %d)\n",
			       pSize->iWidth, pSize->iHeight, iPortId,
			       pPortCap->stMin.iWidth, pPortCap->stMin.iHeight,
			       pPortCap->stMax.iWidth, pPortCap->stMax.iHeight );
			error = CAM_ERROR_NOTSUPPORTEDARG;
		}
	}
	else
	{
		CAM_Int32s i = 0;

		for ( i = 0; i < pPortCap->iSupportedSizeCnt; i++ )
		{
			if ( pSize->iWidth  == pPortCap->stSupportedSize[i].iWidth &&
			     pSize->iHeight == pPortCap->stSupportedSize[i].iHeight )
			{
				break;
			}
		}
		if ( i < pPortCap->iSupportedSizeCnt )
		{
			// not arbitrary size, video size should follow preview size 
			if ( iPortId == CAM_PORT_VIDEO && 
			     !( pCameraState->stCamCap.stPortCapability[CAM_PORT_PREVIEW].bArbitrarySize ) &&
			     ( pSize->iWidth  != pCameraState->stPortState[CAM_PORT_PREVIEW].iWidth ||
			       pSize->iHeight != pCameraState->stPortState[CAM_PORT_PREVIEW].iHeight ) )
			{
				TRACE( CAM_ERROR, "Error: image size(%d, %d) for port[%d] is not supported (%d options)\n", 
				       pSize->iWidth, pSize->iHeight, iPortId, pPortCap->iSupportedSizeCnt );
				error = CAM_ERROR_NOTSUPPORTEDARG;

			}
		}
		else
		{
			TRACE( CAM_ERROR, "Error: image size(%d, %d) is not supported (%d options)\n", 
			       pSize->iWidth, pSize->iHeight, pPortCap->iSupportedSizeCnt );
			error = CAM_ERROR_NOTSUPPORTEDARG;
		}
	}

	if ( error != CAM_ERROR_NONE )
	{
		return error;
	}
	pCameraState->stPortState[iPortId].iWidth  = pSize->iWidth;
	pCameraState->stPortState[iPortId].iHeight = pSize->iHeight;

	error = _get_port_bufreq( pCameraState, iPortId, &pCameraState->stPortState[iPortId].stBufReq );
	ASSERT_CAM_ERROR( error );

	return error;
}

static CAM_Error _set_port_format( _CAM_ExtIspState *pCameraState, CAM_Int32s iPortId, CAM_ImageFormat eFormat )
{
	CAM_PortCapability *pPortCap = NULL;
	CAM_Error          error     = CAM_ERROR_NONE;

	_CHECK_BAD_POINTER( pCameraState );
	_CHECK_BAD_SINGLE_PORT_ID( iPortId );

	_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );
	
	// check if supported
	pPortCap = &pCameraState->stCamCap.stPortCapability[iPortId];
	_CHECK_SUPPORT_ENUM( eFormat, pPortCap->eSupportedFormat, pPortCap->iSupportedFormatCnt );

	if ( !_is_configurable_port( pCameraState, iPortId ) )
	{
		TRACE( CAM_ERROR, "Error: port[%d] is not configurable( %s, %s, %d )\n", iPortId, __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_PORTNOTCONFIGUREABLE;
	}

	pCameraState->stPortState[iPortId].eFormat = eFormat;

	// update the port buffer requirement
	error = _get_port_bufreq( pCameraState, iPortId, &pCameraState->stPortState[iPortId].stBufReq );
	ASSERT_CAM_ERROR( error );

	return error;
}

static CAM_Error _set_port_rotation( _CAM_ExtIspState *pCameraState, CAM_Int32s iPortId, CAM_FlipRotate eRotation )
{
	CAM_PortCapability *pPortCap = NULL;
	CAM_Error          error     = CAM_ERROR_NONE;

	_CHECK_BAD_POINTER( pCameraState );
	_CHECK_BAD_SINGLE_PORT_ID( iPortId );

	_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY & ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_NULL );
	
	// check if supported
	pPortCap = &pCameraState->stCamCap.stPortCapability[iPortId];
	_CHECK_SUPPORT_ENUM( eRotation, pPortCap->eSupportedRotate, pPortCap->iSupportedRotateCnt );

	if ( !_is_configurable_port( pCameraState, iPortId ) )
	{
		TRACE( CAM_ERROR, "Error: port[%d] is not configurable( %s, %s, %d )\n", iPortId, __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_PORTNOTCONFIGUREABLE;
	}

	pCameraState->stPortState[iPortId].eRotation = eRotation;

	// update the port buffer requirement
	error = _get_port_bufreq( pCameraState, iPortId, &pCameraState->stPortState[iPortId].stBufReq );
	ASSERT_CAM_ERROR( error );

	return error;
}

static CAM_Error _set_jpeg_param( _CAM_ExtIspState *pCameraState, CAM_JpegParam *pJpegParam )
{
	CAM_Error error = CAM_ERROR_NONE;

	// bad argument check
	_CHECK_BAD_POINTER( pCameraState );
	_CHECK_BAD_POINTER( pJpegParam );

	_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE | CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_VIDEO );

	if ( !_is_configurable_port( pCameraState, CAM_PORT_STILL ) )
	{
		TRACE( CAM_ERROR, "Error: still port is not configurable( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_PORTNOTCONFIGUREABLE;
	}

	if ( pJpegParam->bEnableExif && pCameraState->stCamCap.stSupportedJPEGParams.bSupportExif == CAM_FALSE )
	{
		TRACE( CAM_ERROR, "Error: EXIF is not supported by current sensor( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
		error = CAM_ERROR_NOTSUPPORTEDARG;
	}
	else if ( pJpegParam->bEnableThumb && pCameraState->stCamCap.stSupportedJPEGParams.bSupportThumb == CAM_FALSE )
	{
		TRACE( CAM_ERROR, "Error: JPEG thumbnail is not supported by current sensor( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
		error = CAM_ERROR_NOTSUPPORTEDARG;
	}
	else if ( pJpegParam->iQualityFactor < pCameraState->stCamCap.stSupportedJPEGParams.iMinQualityFactor|| 
		      pJpegParam->iQualityFactor > pCameraState->stCamCap.stSupportedJPEGParams.iMaxQualityFactor ) 
	{
		TRACE( CAM_ERROR, "Error: JPEG quality[%d] must between %d ~ %d\n", pJpegParam->iQualityFactor, 
		       pCameraState->stCamCap.stSupportedJPEGParams.iMinQualityFactor,
		       pCameraState->stCamCap.stSupportedJPEGParams.iMaxQualityFactor );
		error = CAM_ERROR_NOTSUPPORTEDARG;
	}
	else
	{
		pCameraState->stJpegParam = *pJpegParam;

		// update the port buffer requirement
		error = _get_port_bufreq( pCameraState, (CAM_Int32s)CAM_PORT_STILL, &pCameraState->stPortState[CAM_PORT_STILL].stBufReq );
		ASSERT_CAM_ERROR( error );
	}

	return error;
}

static CAM_Error _set_digital_zoom( _CAM_ExtIspState *pCameraState, CAM_Int32s iDigitalZoomQ16 )
{
	CAM_Error              error   = CAM_ERROR_NONE;
	CAM_Int32s             iPortId = -1;
	CAM_Int32s             i       = 0;
	CAM_Int32s             iSensorDigitalZoomQ16, iPpuDigitalZoomQ16;
	CAM_Bool               bNeedPostProcessing, bUsePrivateBuffer;
	CAM_Bool               bNeedReplaceSensorBuf;
	_CAM_SmartSensorConfig stSensorConfig;
	CAM_Size               stSize = { 0, 0 };

	_CHECK_BAD_POINTER( pCameraState );
	_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_IDLE | CAM_CAPTURESTATE_PREVIEW | CAM_CAPTURESTATE_VIDEO );

	switch( pCameraState->eCaptureState )
	{
	case CAM_CAPTURESTATE_IDLE:
		pCameraState->iDigitalZoomQ16 = iDigitalZoomQ16;
		break;

	case CAM_CAPTURESTATE_PREVIEW:
	case CAM_CAPTURESTATE_VIDEO:
		if ( pCameraState->eCaptureState == CAM_CAPTURESTATE_PREVIEW )
		{
			iPortId = CAM_PORT_PREVIEW;
		}
		else
		{
			iPortId = CAM_PORT_VIDEO;
		}
		
		error = _get_required_sensor_settings( pCameraState, pCameraState->eCaptureState, &stSensorConfig );
		ASSERT_CAM_ERROR( error );

		stSize.iWidth  = stSensorConfig.iWidth;
		stSize.iHeight = stSensorConfig.iHeight;
		
		error = _calc_digital_zoom( pCameraState, &stSize, iDigitalZoomQ16, &iSensorDigitalZoomQ16, &iPpuDigitalZoomQ16 );
		ASSERT_CAM_ERROR( error );
		
		error = _is_need_ppu( &(pCameraState->stPortState[iPortId]), &stSensorConfig, iPpuDigitalZoomQ16,
		                      &bNeedPostProcessing, &bUsePrivateBuffer );
		ASSERT_CAM_ERROR( error );
		
		bNeedReplaceSensorBuf = ( ( pCameraState->bNeedPostProcessing == CAM_FALSE && bNeedPostProcessing == CAM_TRUE ) ||
		                          ( pCameraState->bNeedPostProcessing == CAM_TRUE && bNeedPostProcessing == CAM_FALSE ) ||
		                          ( pCameraState->bNeedPostProcessing == CAM_TRUE && bNeedPostProcessing == CAM_TRUE && pCameraState->bUsePrivateBuffer != bUsePrivateBuffer )
		                        );
		
		if ( bNeedReplaceSensorBuf )
		{
			error = SmartSensor_Flush( pCameraState->hSensorHandle );
			ASSERT_CAM_ERROR( error );

			if ( pCameraState->bNeedPostProcessing )
			{
				error = _flush_all_ppu_srcimg( pCameraState );
				ASSERT_CAM_ERROR( error );
			}
			
			// update bNeedPostProcessing, bUsePrivateBuffer
			pCameraState->bNeedPostProcessing = bNeedPostProcessing;
			pCameraState->bUsePrivateBuffer   = bUsePrivateBuffer;
			
			for ( i = 0; i < pCameraState->stPortState[iPortId].qEmpty.iQueueCount; i++ )
			{
				CAM_Int32s      ret         = 0;
				CAM_ImageBuffer *pUsrBuf    = NULL;
				CAM_ImageBuffer *pSensorBuf = NULL;

				ret = _GetQueue( &pCameraState->stPortState[iPortId].qEmpty, i, (void**)&pUsrBuf );
				ASSERT( ret == 0 );

				if ( pCameraState->bNeedPostProcessing )
				{
					TRACE( CAM_INFO, "Info: Post Processing is enabled in preview state!\n" );
					error = _request_ppu_srcimg( pCameraState, &pCameraState->stCurrentSensorBufReq, pUsrBuf, &pSensorBuf );
					ASSERT_CAM_ERROR( error );
				}
				else
				{
					pSensorBuf = pUsrBuf;
					TRACE( CAM_INFO, "Info: Post Processing is NOT enabled in preview state!\n" );
				}

				error = SmartSensor_Enqueue( pCameraState->hSensorHandle, pSensorBuf );
				ASSERT_CAM_ERROR( error );
			}
		}

	   error = _set_sensor_digital_zoom( pCameraState->hSensorHandle, iSensorDigitalZoomQ16 );
	   ASSERT_CAM_ERROR( error );

	   pCameraState->iDigitalZoomQ16    = iDigitalZoomQ16;
	   pCameraState->iPpuDigitalZoomQ16 = iPpuDigitalZoomQ16;
       break;
	 
	default:
	   ASSERT( 0 );
	   break;
   }

	return error;
}

static CAM_Error _convert_digital_zoom( _CAM_ExtIspState *pCameraState, CAM_Size *pSize, CAM_CaptureState eToState )
{
	CAM_Error  error  = CAM_ERROR_NONE;
	CAM_Int32s iSensorDigitalZoomQ16, iPpuDigitalZoomQ16;

	_CHECK_BAD_POINTER( pCameraState );
	_CHECK_BAD_POINTER( pSize );

	error = _calc_digital_zoom( pCameraState, pSize, pCameraState->iDigitalZoomQ16, &iSensorDigitalZoomQ16, &iPpuDigitalZoomQ16 );
	ASSERT_CAM_ERROR( error );

#if 0
	if ( ( eToState == CAM_CAPTURESTATE_PREVIEW || eToState == CAM_CAPTURESTATE_VIDEO ) &&
	      iPpuDigitalZoomQ16 > ( 1 << 16 ) )
	{
		TRACE( CAM_ERROR, "Error: Currently we do not support PPU digital zoom in preview/video state for performance consideration!\
		      ( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_NOTSUPPORTEDARG;
	}
#endif

	error = _set_sensor_digital_zoom( pCameraState->hSensorHandle, iSensorDigitalZoomQ16 );
	ASSERT_CAM_ERROR( error );

	pCameraState->iPpuDigitalZoomQ16 = iPpuDigitalZoomQ16;

	return error;
}

static CAM_Error _reset_camera( _CAM_ExtIspState *pCameraState, CAM_Int32s iResetType )
{
	CAM_Int32s             i = 0;
	CAM_Size               stSize = { 0, 0 };
	CAM_Error              error  = CAM_ERROR_NONE;
	_CAM_SmartSensorConfig stSensorConfig;
	CAM_Int32s             iPortId;

	// current port
	switch ( pCameraState->eCaptureState )
	{
	case CAM_CAPTURESTATE_IDLE:
	case CAM_CAPTURESTATE_STANDBY:
	case CAM_CAPTURESTATE_NULL:
		TRACE( CAM_ERROR, "Error: cannot reset sensor in state[%d]( %s, %s, %d )", pCameraState->eCaptureState, __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_STATELIMIT;
		break;

	case CAM_CAPTURESTATE_PREVIEW:
		iPortId = CAM_PORT_PREVIEW;
		break;
	
	case CAM_CAPTURESTATE_STILL:
		iPortId = CAM_PORT_STILL;
		break;
	
	case CAM_CAPTURESTATE_VIDEO:
		iPortId = CAM_PORT_VIDEO;
		break;
	default:
		ASSERT( 0 );
		break;
	}

	_lock_port( pCameraState, CAM_PORT_ANY );

	// get sensor settings
	error = _get_required_sensor_settings( pCameraState, pCameraState->eCaptureState, &stSensorConfig );
	ASSERT_CAM_ERROR( error );

	// send config to sensor
	stSensorConfig.iResetType = iResetType;
	error = SmartSensor_Config( pCameraState->hSensorHandle, &stSensorConfig );
	ASSERT_CAM_ERROR( error );

	// convert digital zoom
	stSize.iWidth  = stSensorConfig.iWidth;
	stSize.iHeight = stSensorConfig.iHeight;
	error = _convert_digital_zoom( pCameraState, &stSize, pCameraState->eCaptureState ); 
	ASSERT_CAM_ERROR( error );

	// tag all ppu src buffers as unusing
	if ( pCameraState->bNeedPostProcessing )
	{
		for( i = 0; i < CAM_MAX_PORT_BUF_CNT; i++ )
		{
			pCameraState->bPpuSrcImgUsed[i] = CAM_FALSE;
		}
	}

	for ( i = 0; i < pCameraState->stPortState[iPortId].qEmpty.iQueueCount; i++ )
	{
		CAM_Int32s      ret         = 0;
		CAM_ImageBuffer *pUsrBuf    = NULL;
		CAM_ImageBuffer *pSensorBuf = NULL;

		ret = _GetQueue( &pCameraState->stPortState[iPortId].qEmpty, i, (void**)&pUsrBuf );
		ASSERT( ret == 0 );

		if ( pCameraState->bNeedPostProcessing )
		{
			error = _request_ppu_srcimg( pCameraState, &pCameraState->stCurrentSensorBufReq, pUsrBuf, &pSensorBuf );
			ASSERT_CAM_ERROR( error );
		}
		else
		{
			pSensorBuf = pUsrBuf;
		}

		error = SmartSensor_Enqueue( pCameraState->hSensorHandle, pSensorBuf );
		ASSERT_CAM_ERROR( error );
	}
	
	_unlock_port( pCameraState, CAM_PORT_ANY );
	
	return CAM_ERROR_NONE;
}

static CAM_Error _set_camera_state( _CAM_ExtIspState *pCameraState, CAM_CaptureState eToState )
{
	CAM_Error error = CAM_ERROR_NONE;

	// BAC check
	_CHECK_BAD_POINTER( pCameraState );

	_CHECK_STATE_LIMIT( pCameraState->eCaptureState, CAM_CAPTURESTATE_ANY );

	switch ( eToState )
	{
	case CAM_CAPTURESTATE_NULL:
		if ( pCameraState->eCaptureState == CAM_CAPTURESTATE_IDLE )
		{
			CELOG( "Idle --> Null ...\n" );
			error = _idle2null( pCameraState );
		}
		else
		{
			error = CAM_ERROR_BADSTATETRANSITION;
		}
		break;

	case CAM_CAPTURESTATE_STANDBY:
		if ( pCameraState->eCaptureState == CAM_CAPTURESTATE_IDLE )
		{
			error = CAM_ERROR_NOTIMPLEMENTED;
		}
		else
		{
			error = CAM_ERROR_BADSTATETRANSITION;
		}
		break;

	case CAM_CAPTURESTATE_IDLE:
		if ( pCameraState->eCaptureState == CAM_CAPTURESTATE_PREVIEW )
		{
			CELOG( "Preview --> Idle ...\n" );
			error = _preview2idle( pCameraState );
		}
		else
		{
			error = CAM_ERROR_BADSTATETRANSITION;
		}
		break;

	case CAM_CAPTURESTATE_PREVIEW:
		if ( pCameraState->eCaptureState == CAM_CAPTURESTATE_IDLE )
		{
			CELOG( "Idle --> Preview ...\n" );
			error = _idle2preview( pCameraState );
		}
		else if ( pCameraState->eCaptureState == CAM_CAPTURESTATE_STILL )
		{
			CELOG( "Still --> Preview ...\n" );
			error = _still2preview( pCameraState );
		}
		else if ( pCameraState->eCaptureState == CAM_CAPTURESTATE_VIDEO )
		{
			CELOG( "Video --> Preview ...\n" );
			error = _video2preview( pCameraState );
		}
		else
		{
			error = CAM_ERROR_BADSTATETRANSITION;
		}
		break;

	case CAM_CAPTURESTATE_STILL:
		if ( pCameraState->eCaptureState == CAM_CAPTURESTATE_PREVIEW )
		{
			CELOG( "Preview --> Still ...\n" );
			error = _preview2still( pCameraState );
		}
		else
		{
			error = CAM_ERROR_BADSTATETRANSITION;
		}
		break;

	case CAM_CAPTURESTATE_VIDEO:
		if ( pCameraState->eCaptureState == CAM_CAPTURESTATE_PREVIEW )
		{
			CELOG( "Preview --> Video ...\n" );
			error = _preview2video( pCameraState );
		}
		else
		{
			error = CAM_ERROR_BADSTATETRANSITION;
		}
		break;

	default:
		TRACE( CAM_ERROR, "Error: transition to invalid state[%d]( %s, %s, %d )\n", eToState, __FILE__, __FUNCTION__, __LINE__ );
		error = CAM_ERROR_BADARGUMENT;
		break;
	}

	if ( error == CAM_ERROR_NONE )
	{
		CELOG( "State Transition Done\n" );
	}
	else if ( error == CAM_ERROR_BADSTATETRANSITION )
	{
		TRACE( CAM_ERROR, "Error: Unsupported State Transition( %d -> %d )\n", pCameraState->eCaptureState, eToState );
	}
	else
	{
		TRACE( CAM_ERROR, "Error: State Transition Failed, error code[%d]\n", error );
	}

	return error;
}



static CAM_Bool _is_valid_port( _CAM_ExtIspState *pCameraState, CAM_Int32s iPortId )
{
	// TODO: check the port consistency, here

	return CAM_TRUE;
}


static CAM_Bool _is_configurable_port( _CAM_ExtIspState *pCameraState, CAM_Int32s iPortId )
{
	// configurable port must be valid and must has no buffer

	CAM_Bool bCfg = CAM_TRUE;

	bCfg = ( pCameraState->stPortState[iPortId].qEmpty.iQueueCount + pCameraState->stPortState[iPortId].qFilled.iQueueCount == 0 );
	
	return bCfg;
}


/******************************************************************************************************
//
// Name:         _dequeue_filled
// Description:  dequeue an existing filled image according to the given port id
// Arguments:    pCameraState[IN] : camera state
//               pPortId [IN/OUT] : port id
//               ppBuf  [OUT]     : pointer to the dequeued buffer
// Return:       None
// Notes:        Three possible results:
//               1. The given port is not active and has no filled buffer, return NULL with "port not active" error code;
//               2. The given port has buffer, return the buffer with no error code, regardless if the port is active or not;
//               3. The given port is active and has no filled buffer, return NULL with no error.
// Version Log:  version      Date          Author     Description
 *******************************************************************************************************/
static CAM_Error _dequeue_filled( _CAM_ExtIspState *pCameraState, CAM_Int32s *pPortId, CAM_ImageBuffer **ppBuf )
{
	CAM_Error  error = CAM_ERROR_NONE;

	_CHECK_BAD_POINTER( pCameraState );
	_CHECK_BAD_POINTER( pPortId );
	_CHECK_BAD_POINTER( ppBuf );
	_CHECK_BAD_ANY_PORT_ID( *pPortId );

	_CHECK_STATE_LIMIT( pCameraState->eCaptureState, ~CAM_CAPTURESTATE_STANDBY & ~CAM_CAPTURESTATE_IDLE & ~CAM_CAPTURESTATE_NULL );

	*ppBuf = NULL;

	if ( *pPortId == CAM_PORT_PREVIEW || *pPortId == CAM_PORT_VIDEO || *pPortId == CAM_PORT_STILL )
	{
		if ( 0 == _DeQueue( &pCameraState->stPortState[*pPortId].qFilled, (void**)ppBuf ) )
		{
			// got a buffer, and return no error;
			error = CAM_ERROR_NONE;	
		}
		else if ( ( pCameraState->eCaptureState == CAM_CAPTURESTATE_PREVIEW && *pPortId != CAM_PORT_PREVIEW ) ||
			      ( pCameraState->eCaptureState == CAM_CAPTURESTATE_VIDEO && *pPortId == CAM_PORT_STILL ) ||
			      ( pCameraState->eCaptureState == CAM_CAPTURESTATE_STILL && *pPortId == CAM_PORT_VIDEO ) ||
			      ( pCameraState->eCaptureState == CAM_CAPTURESTATE_STILL && *pPortId != CAM_PORT_VIDEO && pCameraState->iStillRestCount <= 0 ) )
		{
			// got no buffer, and return port not active error code
			TRACE( CAM_ERROR, "Error: port[%d] is not active in state[%d] ( %s, %s, %d )\n", *pPortId, pCameraState->eCaptureState, 
			       __FILE__, __FUNCTION__, __LINE__ );
			error = CAM_ERROR_PORTNOTACTIVE;	
		}
		else
		{	
			// got no buffer, and return no error;
			error = CAM_ERROR_NONE;
		}
	}
	else if ( *pPortId == CAM_PORT_ANY )
	{
		if ( 0 == _DeQueue( &pCameraState->stPortState[CAM_PORT_STILL].qFilled, (void**)ppBuf ) )
		{
			*pPortId = CAM_PORT_STILL;
			error = CAM_ERROR_NONE;
		}
		else if ( 0 == _DeQueue( &pCameraState->stPortState[CAM_PORT_VIDEO].qFilled, (void**)ppBuf ) )
		{
			*pPortId = CAM_PORT_VIDEO;
			error = CAM_ERROR_NONE;
		}
		else if ( 0 == _DeQueue( &pCameraState->stPortState[CAM_PORT_PREVIEW].qFilled, (void**)ppBuf ) )
		{
			*pPortId = CAM_PORT_PREVIEW;
			error = CAM_ERROR_NONE;
		}
		else
		{
			*ppBuf = NULL;
			error = CAM_ERROR_NONE;
		}
	}

	return error;
}

static CAM_Error _run_one_frame( _CAM_ExtIspState *pCameraState )
{
	CAM_Error error = CAM_ERROR_NONE;

	ASSERT( pCameraState != NULL );

	switch ( pCameraState->eCaptureState )
	{
	case CAM_CAPTURESTATE_NULL:
	case CAM_CAPTURESTATE_STANDBY:
	case CAM_CAPTURESTATE_IDLE:
		error = CAM_ERROR_PORTNOTACTIVE;
		break;

	case CAM_CAPTURESTATE_PREVIEW:
		error = _process_previewstate_buffer( pCameraState );
		break;
	
	case CAM_CAPTURESTATE_VIDEO:
		error = _process_videostate_buffer( pCameraState );
		break;

	case CAM_CAPTURESTATE_STILL:
		error = _process_stillstate_buffer( pCameraState );
		break;

	default:
		ASSERT( 0 );
		break;
	}

	return error;
}

static CAM_Error _process_previewstate_buffer( _CAM_ExtIspState *pCameraState )
{
	CAM_Int32s      ret         = 0;
	CAM_Error       error       = CAM_ERROR_NONE;
	CAM_ImageBuffer *pSensorImg = NULL;
	CAM_ImageBuffer *pPortImg   = NULL;

	_CAM_PortState  *pPreviewPort = &pCameraState->stPortState[CAM_PORT_PREVIEW];

	// overwrite filled image when there's no enough number of empty buffers
	// we must have enough empty buffers on preview port to start the processing
	while( pPreviewPort->qEmpty.iQueueCount < pPreviewPort->stBufReq.iMinBufCount )
	{
		CAM_ImageBuffer *pTmpImg = NULL;

		// if there's no empty buffer, we try to overwrite a filled buffer
		ret = _DeQueue( &pPreviewPort->qFilled, (void**)&pTmpImg );
		if( ret != 0 )
		{
			TRACE( CAM_ERROR, "Error: there is no enough buffers for preview port( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
			return CAM_ERROR_NOTENOUGHBUFFERS;
		}

		// inform about the frame loss
		SEND_EVENT( pCameraState, CAM_EVENT_FRAME_DROPPED, CAM_PORT_PREVIEW );

		// queue as empty buffer
		error = _enqueue_buffer( pCameraState, CAM_PORT_PREVIEW, pTmpImg );
		ASSERT_CAM_ERROR( error );
	}

	// get image buffer from sensor
	error = SmartSensor_Dequeue( pCameraState->hSensorHandle, &pSensorImg );
	if ( error != CAM_ERROR_NONE ) 
	{
		return error;
	}

	if ( pCameraState->bNeedPostProcessing )
	{
		_CAM_PostProcessParam stPostProcessParam;

		stPostProcessParam.bFavorAspectRatio  = CAM_FALSE;
		stPostProcessParam.iPpuDigitalZoomQ16 = pCameraState->iPpuDigitalZoomQ16;
		stPostProcessParam.eRotation          = pPreviewPort->eRotation;
		stPostProcessParam.pJpegParam         = NULL;
		stPostProcessParam.pUsrData           = NULL;
		// whether in-place or not
		stPostProcessParam.bIsInPlace         = ( ( pCameraState->bUsePrivateBuffer == CAM_FALSE ) ? CAM_TRUE : CAM_FALSE );

		// get the preview buffer
		if ( stPostProcessParam.bIsInPlace )
		{
			ret = _RemoveQueue( &pPreviewPort->qEmpty, pSensorImg->pUserData );
			ASSERT( ret == 0 );

			pPortImg = (CAM_ImageBuffer*)( pSensorImg->pUserData );
		}
		else
		{
			ret = _DeQueue( &pPreviewPort->qEmpty, (void**)&pPortImg );
			ASSERT( ret == 0 && pPortImg != NULL );
		}

#if defined( CAM_PERF )
		{
		  CAM_Tick t = -IPP_TimeGetTickCount();
#endif
		  // image resizing & rotate to generate the preview buffer
		  error = _ppu_image_process( pCameraState->hPpuHandle, pSensorImg, pPortImg, &stPostProcessParam );
		  ASSERT_CAM_ERROR( error );

#if defined( CAM_PERF )
		  t += IPP_TimeGetTickCount();
		  CELOG( "Perf: format conversion[%d -> %d], resize[(%d * %d) -> (%d * %d)], rotation[%d] cost %.2f ms\n", 
		          pSensorImg->eFormat,  pPortImg->eFormat,
		          pSensorImg->iWidth,   pSensorImg->iHeight,
		          pPreviewPort->iWidth, pPreviewPort->iHeight,
		          pPreviewPort->eRotation,
		          t / 1000.0 );
		}
#endif
		if ( pPortImg->bEnableShotInfo )
		{
			pPortImg->stShotInfo = pSensorImg->stShotInfo;
		}

		pPortImg->iTick = pSensorImg->iTick;

		// release the ppu src image( _request_ppu_srcimg called by _enqueue_buffer )
		error = _release_ppu_srcimg( pCameraState, pSensorImg );
		ASSERT_CAM_ERROR( error );

		ret = _EnQueue( &pPreviewPort->qFilled, pPortImg );
		ASSERT( ret == 0 );
	}
	else
	{
		// remove from the empty queue
		ret = _RemoveQueue( &pPreviewPort->qEmpty, pSensorImg );
		ASSERT( ret == 0 );

		// put to the filled queue
		ret = _EnQueue( &pPreviewPort->qFilled, pSensorImg );
		ASSERT( ret == 0 );
	}

	SEND_EVENT( pCameraState, CAM_EVENT_FRAME_READY, CAM_PORT_PREVIEW );

	return error;
}


static CAM_Error _process_videostate_buffer( _CAM_ExtIspState *pCameraState )
{
	CAM_Int32s      ret         = 0;
	CAM_Error       error       = CAM_ERROR_NONE;
	CAM_ImageBuffer *pSensorImg = NULL;
	CAM_ImageBuffer *pPortImg   = NULL;

	_CAM_PortState  *pPreviewPort = &pCameraState->stPortState[CAM_PORT_PREVIEW];
	_CAM_PortState  *pVideoPort   = &pCameraState->stPortState[CAM_PORT_VIDEO];

	// we must have enough empty buffers on video port to start the processing
	while ( pVideoPort->qEmpty.iQueueCount < pVideoPort->stBufReq.iMinBufCount )
	{
		CAM_ImageBuffer *pTmpImg = NULL;

		// if there's no empty buffer, we try to overwrite a filled buffer
		ret = _DeQueue( &pVideoPort->qFilled, (void**)&pTmpImg );
		if ( ret != 0 )
		{
			TRACE( CAM_ERROR, "Error: there is no enough buffers for video port ( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
			return CAM_ERROR_NOTENOUGHBUFFERS;
		}

		SEND_EVENT( pCameraState, CAM_EVENT_FRAME_DROPPED, CAM_PORT_VIDEO );

		error = _enqueue_buffer( pCameraState, CAM_PORT_VIDEO, pTmpImg );
		ASSERT_CAM_ERROR( error );
	}

	// get sensor buffer
	error = SmartSensor_Dequeue( pCameraState->hSensorHandle, &pSensorImg );
	if ( error != CAM_ERROR_NONE ) 
	{
		return error;
	}

	// try to process the preview image
	if ( _DeQueue( &pPreviewPort->qEmpty, (void**)&pPortImg ) == -1 )
	{
		// anyway, one frame will drop form preview port
		SEND_EVENT( pCameraState, CAM_EVENT_FRAME_DROPPED, CAM_PORT_PREVIEW );
		if ( _DeQueue( &pPreviewPort->qFilled, (void**)&pPortImg ) == -1 )
		{
			pPortImg = NULL;
		}
	}

	if ( pPortImg != NULL )
	{
#if defined( CAM_PERF )
		{
		  CAM_Tick t = -IPP_TimeGetTickCount();
#endif
		  _CAM_PostProcessParam stPostProcessParam;

		  stPostProcessParam.bFavorAspectRatio  = pCameraState->bPreviewFavorAspectRatio;
		  stPostProcessParam.iPpuDigitalZoomQ16 = pCameraState->iPpuDigitalZoomQ16;
		  stPostProcessParam.eRotation          = pPreviewPort->eRotation;
		  stPostProcessParam.pJpegParam         = NULL;
		  stPostProcessParam.pUsrData           = NULL;
		  stPostProcessParam.bIsInPlace         = CAM_FALSE;

		 // image resizing & rotate to generate the preview buffer
		  error = _ppu_image_process( pCameraState->hPpuHandle, pSensorImg, pPortImg, &stPostProcessParam );
		  ASSERT_CAM_ERROR( error );

#if defined( CAM_PERF )
		  t += IPP_TimeGetTickCount();
		  CELOG( "Perf: format conversion[%d -> %d], resize[(%d * %d) -> (%d * %d)], rotation[%d] cost %.2f ms\n", 
		          pSensorImg->eFormat,  pPortImg->eFormat,
		          pSensorImg->iWidth,   pSensorImg->iHeight,
		          pPreviewPort->iWidth, pPreviewPort->iHeight,
		          pPreviewPort->eRotation,
		          t / 1000.0 );
		}
#endif
		if ( pPortImg->bEnableShotInfo )
		{
			pPortImg->stShotInfo = pSensorImg->stShotInfo;
		}

		pPortImg->iTick = pSensorImg->iTick;

		ret = _EnQueue( &pPreviewPort->qFilled, (void*)pPortImg );
		ASSERT( ret == 0 );

		SEND_EVENT( pCameraState, CAM_EVENT_FRAME_READY, CAM_PORT_PREVIEW );
	}

	// remove from the input queue and put to the output queue
	ret = _RemoveQueue( &pVideoPort->qEmpty, (void*)pSensorImg );
	ASSERT( ret == 0 );

	ret = _EnQueue( &pVideoPort->qFilled, (void*)pSensorImg );
	ASSERT( ret == 0 );

	SEND_EVENT( pCameraState, CAM_EVENT_FRAME_READY, CAM_PORT_VIDEO );

	return error;
}

static CAM_Error _process_stillstate_buffer( _CAM_ExtIspState *pCameraState )
{
	CAM_Int32s      ret         = 0;
	CAM_Error       error       = CAM_ERROR_NONE;
	CAM_ImageBuffer *pSensorImg = NULL;
	CAM_ImageBuffer *pPortImg   = NULL;

	_CAM_PortState  *pPreviewPort = &pCameraState->stPortState[CAM_PORT_PREVIEW];
	_CAM_PortState  *pStillPort   = &pCameraState->stPortState[CAM_PORT_STILL];

	if ( pCameraState->iStillRestCount <= 0 )
	{	
		return CAM_ERROR_PORTNOTACTIVE;
	}

	// we don't overwrite filled still image data if there's no enough empty buffer

	// get sensor buffer
	error = SmartSensor_Dequeue( pCameraState->hSensorHandle, &pSensorImg );
	if ( error != CAM_ERROR_NONE ) 
	{
		return error;
	}
	SEND_EVENT( pCameraState, CAM_EVENT_STILL_SHUTTERCLOSED, 0 );
	
	// try to process the snapshot image if feature opened
#if defined( BUILD_OPTION_STRATEGY_ENABLE_SNAPSHOT )
	if ( 0 == _DeQueue( &pPreviewPort->qEmpty, (void**)&pPortImg ) )
	{
#if defined( CAM_PERF )
		CAM_Tick t = -IPP_TimeGetTickCount(); 
#endif

		_CAM_PostProcessParam stPostProcessParam;

		stPostProcessParam.bFavorAspectRatio  = pCameraState->bPreviewFavorAspectRatio;
		stPostProcessParam.iPpuDigitalZoomQ16 = pCameraState->iPpuDigitalZoomQ16;
		stPostProcessParam.eRotation          = pPreviewPort->eRotation;
		stPostProcessParam.pJpegParam         = NULL;
		stPostProcessParam.pUsrData           = NULL;
		stPostProcessParam.bIsInPlace         = CAM_FALSE;

		// image resizing & rotate to generate the preview buffer
		error = _ppu_image_process( pCameraState->hPpuHandle, pSensorImg, pPortImg, &stPostProcessParam );
		if ( error == CAM_ERROR_NONE )
		{
#if defined( CAM_PERF )
		  t += IPP_TimeGetTickCount();
		  CELOG( "Perf: format conversion[%d -> %d], resize[(%d * %d) -> (%d * %d)], rotation[%d] cost %.2f ms\n", 
		         pSensorImg->eFormat, pPortImg->eFormat,
		         pSensorImg->iWidth, pSensorImg->iHeight,
		         pPreviewPort->iWidth, pPreviewPort->iHeight,
		         pPreviewPort->eRotation,
		         t / 1000.0 );
#endif
		  if ( pPortImg->bEnableShotInfo )
		  {
			pPortImg->stShotInfo = pSensorImg->stShotInfo;
		  }

		  pPortImg->iTick = pSensorImg->iTick;

		  ret = _EnQueue( &pPreviewPort->qFilled, (void*)pPortImg );
		  ASSERT( ret == 0 );

		  // notify that the snapshot is ready on the port
		  SEND_EVENT( pCameraState, CAM_EVENT_FRAME_READY, CAM_PORT_PREVIEW );
		}
		else
		{
			ret = _EnQueue( &pPreviewPort->qEmpty, (void*)pPortImg );
			ASSERT( ret == 0 );
			// notify the snapshot frame loss
			SEND_EVENT( pCameraState, CAM_EVENT_FRAME_DROPPED, CAM_PORT_PREVIEW );
		}

	}
	else
#endif
	{
		// notify the snapshot frame loss
		SEND_EVENT( pCameraState, CAM_EVENT_FRAME_DROPPED, CAM_PORT_PREVIEW );
	}
	
	// try to compress the image if JPEG compressor opened
	error = CAM_ERROR_NONE;
	if ( pCameraState->bNeedPostProcessing )
	{
		_CAM_PostProcessParam stPostProcessParam;

		stPostProcessParam.bFavorAspectRatio  = pCameraState->bPreviewFavorAspectRatio;
		stPostProcessParam.iPpuDigitalZoomQ16 = pCameraState->iPpuDigitalZoomQ16;
		stPostProcessParam.eRotation          = pStillPort->eRotation;
		stPostProcessParam.pJpegParam         = &pCameraState->stJpegParam;
		stPostProcessParam.pUsrData           = NULL;
		stPostProcessParam.bIsInPlace         = ( ( pCameraState->bUsePrivateBuffer == CAM_FALSE ) ? CAM_TRUE : CAM_FALSE );

		// get the still port buffer
		if ( stPostProcessParam.bIsInPlace )
		{
			ret = _RemoveQueue( &pStillPort->qEmpty, pSensorImg->pUserData );
			ASSERT( ret == 0 );

			pPortImg = (CAM_ImageBuffer*)( pSensorImg->pUserData );
		}
		else
		{
			ret = _DeQueue( &pStillPort->qEmpty, (void**)&pPortImg );
			ASSERT( ret == 0 && pPortImg != NULL );
		}

		// JPEG encoding
#if defined( CAM_PERF )
		{
		  CAM_Tick t = -IPP_TimeGetTickCount();
#endif
		  error = _ppu_image_process( pCameraState->hPpuHandle, pSensorImg, pPortImg, &stPostProcessParam );
		  ASSERT_CAM_ERROR( error );

#if defined( CAM_PERF )
		  t += IPP_TimeGetTickCount();
		  CELOG( "Perf: format conversion[%d -> %d], resize[(%d * %d) -> (%d * %d)], rotation[%d] cost %.2f ms\n", 
		          pSensorImg->eFormat, pPortImg->eFormat,
		          pSensorImg->iWidth, pSensorImg->iHeight,
		          pStillPort->iWidth, pStillPort->iHeight,
		          pStillPort->eRotation,
		          t / 1000.0 );
		}
#endif
		if ( pPortImg->bEnableShotInfo )
		{
			pPortImg->stShotInfo = pSensorImg->stShotInfo;
		}

		pPortImg->iTick = pSensorImg->iTick;

		error = _release_ppu_srcimg( pCameraState, pSensorImg );
		ASSERT_CAM_ERROR( error );

		ret = _EnQueue( &pStillPort->qFilled, pPortImg );
		ASSERT( ret == 0 );
	}
	else
	{
		// remove from the empty queue
		ret = _RemoveQueue( &pStillPort->qEmpty, pSensorImg );
		ASSERT( ret == 0 );

		// put to the filled queue
		ret = _EnQueue( &pStillPort->qFilled, pSensorImg );
		ASSERT( ret == 0 );
	}

	pCameraState->iStillRestCount--;
	SEND_EVENT( pCameraState, CAM_EVENT_FRAME_READY, CAM_PORT_STILL );

	// notify that all the images are processed
	if ( pCameraState->iStillRestCount <= 0 )
	{
		SEND_EVENT( pCameraState, CAM_EVENT_STILL_ALLPROCESSED, 0 );
	}

	return error;
}

static CAM_Error _negotiate_image_format( CAM_ImageFormat eDstFmt, CAM_ImageFormat *pSrcFmt, CAM_ImageFormat *pFmtCap, CAM_Int32s iFmtCapCnt )
{
	CAM_Int32s i     = 0;
	CAM_Error  error = CAM_ERROR_NONE;

	for ( i = 0; i < iFmtCapCnt; i++ )
	{
		if ( pFmtCap[i] == eDstFmt )
		{
			break;
		}
	}

	if ( i < iFmtCapCnt )
	{
		*pSrcFmt = pFmtCap[i];
		return CAM_ERROR_NONE;
	}

	error = _ppu_negotiate_format( eDstFmt, pSrcFmt, pFmtCap, iFmtCapCnt );
	return error;
}

/* negotiate image size with sensor( i.e. find the closest sensor resolution )
 * ideal size is exactly the same size as the target,
 * if can't find such a size, find out the max size that is no larger than the target size
 */
static CAM_Error _negotiate_image_size( CAM_Size *pDstSize, CAM_Size *pSrcSize, CAM_Size *pSizeCap, CAM_Int32s iSizeCapCnt )
{
	CAM_Int32s i;
	CAM_Int32s iNearestMatch = -1, iExactMatch = -1, iBestMatch = -1;
	CAM_Int32s iNearestPixNum, iDstPixNum;

	iDstPixNum     = pDstSize->iWidth * pDstSize->iHeight;
	iNearestPixNum = -1;

	for ( i = 0; i < iSizeCapCnt; i++ )
	{
		CAM_Int32s iPixNum = pSizeCap[i].iWidth * pSizeCap[i].iHeight;
		
		if ( ( iNearestPixNum == -1 ) || ( ( _ABSDIFF( iPixNum, iDstPixNum ) < _ABSDIFF( iNearestPixNum, iDstPixNum ) ) && 
		     ( pDstSize->iWidth / pDstSize->iHeight == pSizeCap[i].iWidth / pSizeCap[i].iHeight ) ) )
		{
			iNearestMatch  = i;
			iNearestPixNum = iPixNum;
		}
		if ( pSizeCap[i].iWidth == pDstSize->iWidth && pSizeCap[i].iHeight == pDstSize->iHeight )
		{
			iExactMatch = i;
		}
	}

	if ( iExactMatch >= 0 )
	{
		iBestMatch = iExactMatch;
	}
	else
	{
		iBestMatch = iNearestMatch;
	}

	pSrcSize->iWidth  = pSizeCap[iBestMatch].iWidth;
	pSrcSize->iHeight = pSizeCap[iBestMatch].iHeight;

	return CAM_ERROR_NONE;
}

static CAM_Error _get_required_sensor_settings( _CAM_ExtIspState *pCameraState, CAM_CaptureState eCaptureState, _CAM_SmartSensorConfig *pSensorConfig )
{
	CAM_Error                  error        = CAM_ERROR_NONE;
	_CAM_PortState             *pPortState  = NULL;
	CAM_ImageFormat            eFormat;
	CAM_Int32s                 iCurrentPort = -1;
	_CAM_SmartSensorCapability stSensorCap;

	memset( pSensorConfig, 0, sizeof( _CAM_SmartSensorConfig ) );
	
	if ( CAM_CAPTURESTATE_IDLE == eCaptureState || CAM_CAPTURESTATE_STANDBY == eCaptureState )
	{
		pSensorConfig->eState = CAM_CAPTURESTATE_IDLE;	
		return CAM_ERROR_NONE;
	}
	else if ( CAM_CAPTURESTATE_PREVIEW == eCaptureState )
	{
		iCurrentPort = CAM_PORT_PREVIEW;
		pPortState = &( pCameraState->stPortState[iCurrentPort] );
		pSensorConfig->iTargetFpsQ16 = pCameraState->iTargetPreviewFpsQ16;
		pSensorConfig->eState = eCaptureState;
	}
	else if ( CAM_CAPTURESTATE_STILL == eCaptureState )
	{
		iCurrentPort = CAM_PORT_STILL;
		pPortState = &( pCameraState->stPortState[iCurrentPort] );
		pSensorConfig->iTargetFpsQ16 = 0;
		pSensorConfig->eState        = eCaptureState;
		pSensorConfig->stJpegParam   = pCameraState->stJpegParam;
	}
	else if ( CAM_CAPTURESTATE_VIDEO == eCaptureState )
	{
		iCurrentPort = CAM_PORT_VIDEO;
		pPortState = &(pCameraState->stPortState[iCurrentPort]);
		pSensorConfig->iTargetFpsQ16 = pCameraState->iTargetVideoFpsQ16;
		pSensorConfig->eState = eCaptureState;
	}

	error = SmartSensor_GetCapability( pCameraState->iSensorID, &stSensorCap );
	ASSERT_CAM_ERROR( error );

	if ( CAM_PORT_VIDEO == iCurrentPort )
	{
		pSensorConfig->iWidth  = pPortState->iWidth;
		pSensorConfig->iHeight = pPortState->iHeight;
		pSensorConfig->eFormat = pPortState->eFormat;
	}
	else if ( CAM_PORT_STILL == iCurrentPort )
	{
		pSensorConfig->iWidth  = pPortState->iWidth;
		pSensorConfig->iHeight = pPortState->iHeight;
		error = _negotiate_image_format( pPortState->eFormat, &eFormat, stSensorCap.eSupportedStillFormat, stSensorCap.iSupportedStillFormatCnt );
		if ( error != CAM_ERROR_NONE )
		{
			TRACE( CAM_ERROR, "Error: unsupported format[%d] for still port( %s, %d )\n", pPortState->eFormat, __FILE__, __LINE__ );
			return error;
		}
		else
		{
			pSensorConfig->eFormat = eFormat;
		}
	}
	else if ( CAM_PORT_PREVIEW == iCurrentPort )
	{
		// negotiate image size with sensor
		CAM_Size stNegSize, stDstSize;
		stDstSize.iWidth  = pPortState->iWidth;
		stDstSize.iHeight = pPortState->iHeight;

		error = _negotiate_image_size( &stDstSize, &stNegSize, stSensorCap.stSupportedVideoSize, stSensorCap.iSupportedVideoSizeCnt );
		ASSERT_CAM_ERROR( error );

		pSensorConfig->iWidth  = stNegSize.iWidth;
		pSensorConfig->iHeight = stNegSize.iHeight;

		TRACE( CAM_INFO, "Info: Port[%d] Required size: %d x %d\n", iCurrentPort, pPortState->iWidth, pPortState->iHeight );
		TRACE( CAM_INFO, "Info: Port[%d] Selected size: %d x %d\n", iCurrentPort, pSensorConfig->iWidth, pSensorConfig->iHeight );

		// negotiate image format
		error = _negotiate_image_format( pPortState->eFormat, &eFormat, stSensorCap.eSupportedVideoFormat, stSensorCap.iSupportedVideoFormatCnt );
		if ( error != CAM_ERROR_NONE )
		{
			TRACE( CAM_ERROR, "Error: unsupported format[%d] for preview port( %s, %d )\n", pPortState->eFormat, __FILE__, __LINE__ );
			return error;
		}
		else
		{
			pSensorConfig->eFormat = eFormat;
		}
	}
	pSensorConfig->iActualFpsQ16 = 0;

	return CAM_ERROR_NONE;
}

static CAM_Error _calc_digital_zoom( _CAM_ExtIspState *pCameraState, CAM_Size *pSize, CAM_Int32s iDigitalZoomQ16, CAM_Int32s *pSensorDigitalZoomQ16, CAM_Int32s *pPpuDigitalZoomQ16 )
{
	CAM_Error  error                = CAM_ERROR_NONE;
	CAM_Int32s iSensorDigZoomCapQ16 = 0;


	_CHECK_BAD_POINTER( pCameraState );
	_CHECK_BAD_POINTER( pSize );
	_CHECK_BAD_POINTER( pSensorDigitalZoomQ16 );
	_CHECK_BAD_POINTER( pPpuDigitalZoomQ16 );


	error = SmartSensor_GetDigitalZoomCapability( pCameraState->iSensorID, pSize->iWidth, pSize->iHeight, &iSensorDigZoomCapQ16 ); 
	ASSERT_CAM_ERROR( error );

	// TODO: here we can do some optimization to avoid very small PPU resize
	if ( iDigitalZoomQ16 > iSensorDigZoomCapQ16 )
	{
		 *pPpuDigitalZoomQ16    = ( ( (CAM_Int64u)iDigitalZoomQ16 ) << 16 ) / iSensorDigZoomCapQ16;
		 *pSensorDigitalZoomQ16 = iSensorDigZoomCapQ16;
	}
	else
	{
		*pPpuDigitalZoomQ16    = 1 << 16;
		*pSensorDigitalZoomQ16 = iDigitalZoomQ16;
	}
	
	return error;
}

static CAM_Error _set_sensor_digital_zoom( void *hSensorHandle, CAM_Int32s iSensorDigitalZoomQ16 )
{
	CAM_Error      error             = CAM_ERROR_NONE;
	_CAM_ShotParam stShotParamStatus;

   	_CHECK_BAD_POINTER( hSensorHandle );

	error = SmartSensor_GetShotParam( hSensorHandle, &stShotParamStatus );
	ASSERT_CAM_ERROR( error );

	stShotParamStatus.iDigZoomQ16 = iSensorDigitalZoomQ16;
	error = SmartSensor_SetShotParam( hSensorHandle, &stShotParamStatus );
	ASSERT_CAM_ERROR( error );

	return error;
}



/*----------------------------------------------------------------------------------------------
*  Camera Engine's State Machine 
*               null
*                |
*               idle -- [ standby ]
*                |
*     still -- preview -- video
*---------------------------------------------------------------------------------------------*/
/****************************************************************************************************
* State Transition: standby->idle
* Notes:
****************************************************************************************************/
static CAM_Error _standby2idle( _CAM_ExtIspState *pCameraState )
{
	return CAM_ERROR_NOTIMPLEMENTED;
}

/****************************************************************************************************
* State Transition: idle->standby
* Notes:
****************************************************************************************************/
static CAM_Error _idle2standby( _CAM_ExtIspState *pCameraState )
{
	return CAM_ERROR_NOTIMPLEMENTED;
}

/****************************************************************************************************
* State Transition: idle->null
* Notes:
****************************************************************************************************/
static CAM_Error _idle2null( _CAM_ExtIspState *pCameraState )
{
	CAM_Error  error = CAM_ERROR_NONE;
	CAM_Int32s i     = 0;
	
	ASSERT( pCameraState->iSensorID != -1 );

	error = SmartSensor_Deinit( &pCameraState->hSensorHandle );
	if ( error != CAM_ERROR_NONE )
	{
		return error;
	}

	// NOTE: here we will flush all buffers assigned to the old sensor in all ports, so APP should aware that, he can
	// handle those buffers after switch sensor ID. A good practice is that APP flush all ports themselves.
	for ( i = CAM_PORT_PREVIEW; i <= CAM_PORT_STILL; i++ )
	{
		_FlushQueue( &pCameraState->stPortState[i].qEmpty );
		_FlushQueue( &pCameraState->stPortState[i].qFilled );
	}

	pCameraState->iSensorID = -1;

	pCameraState->eCaptureState = CAM_CAPTURESTATE_NULL;

	return CAM_ERROR_NONE;
}

/****************************************************************************************************
* State Transition: idle->preview
* Notes: If Client's requirement is not met by sensor driver, Camera Engine will require a 
*       nearest size from sensor( _get_required_sensor_settings ), and do resize/rotae/color space 
*       convertion in Camera Engine to meet this requirement. In this case, Camera Engine will allocate 
*       private buffers to sensor.
****************************************************************************************************/
static CAM_Error _idle2preview( _CAM_ExtIspState *pCameraState )
{
	CAM_Int32s             i = 0, ret = 0;
	CAM_Size               stSize = { 0, 0 };
	CAM_Error              error = CAM_ERROR_NONE;
	_CAM_SmartSensorConfig stSensorConfig;

	// check preview port
	if ( !_is_valid_port( pCameraState, CAM_PORT_PREVIEW ) )
	{
		return CAM_ERROR_PORTNOTVALID;
	}

	// get sensor settings( resolution/format etc. )
	error = _get_required_sensor_settings( pCameraState, CAM_CAPTURESTATE_PREVIEW, &stSensorConfig );
	ASSERT_CAM_ERROR( error );

	// send config to sensor
	error = SmartSensor_Config( pCameraState->hSensorHandle, &stSensorConfig );
	ASSERT_CAM_ERROR( error );

	pCameraState->iActualPreviewFpsQ16 = stSensorConfig.iActualFpsQ16;

	// convert digital zoom
	stSize.iWidth  = stSensorConfig.iWidth;
	stSize.iHeight = stSensorConfig.iHeight;
	error = _convert_digital_zoom( pCameraState, &stSize, CAM_CAPTURESTATE_PREVIEW ); 
	ASSERT_CAM_ERROR( error );

	// get smart sensor buffer requirement
	error = SmartSensor_GetBufReq( pCameraState->hSensorHandle, &stSensorConfig, &pCameraState->stCurrentSensorBufReq );
	ASSERT_CAM_ERROR( error );

	error = _is_need_ppu( &(pCameraState->stPortState[CAM_PORT_PREVIEW]), &stSensorConfig, pCameraState->iPpuDigitalZoomQ16,
	                      &(pCameraState->bNeedPostProcessing), &(pCameraState->bUsePrivateBuffer) );
	ASSERT_CAM_ERROR( error );

	for ( i = 0; i < pCameraState->stPortState[CAM_PORT_PREVIEW].qEmpty.iQueueCount; i++ )
	{
		CAM_ImageBuffer *pUsrBuf    = NULL;
		CAM_ImageBuffer *pSensorBuf = NULL;

		ret = _GetQueue( &pCameraState->stPortState[CAM_PORT_PREVIEW].qEmpty, i, (void**)&pUsrBuf );
		ASSERT( ret == 0 );

		if ( pCameraState->bNeedPostProcessing )
		{
			TRACE( CAM_INFO, "Info: Post Processing is enabled in preview state!\n" );
			error = _request_ppu_srcimg( pCameraState, &pCameraState->stCurrentSensorBufReq, pUsrBuf, &pSensorBuf );
			ASSERT_CAM_ERROR( error );
		}
		else
		{
			pSensorBuf = pUsrBuf;
			TRACE( CAM_INFO, "Info: Post Processing is NOT enabled in preview state!\n" );
		}

		error = SmartSensor_Enqueue( pCameraState->hSensorHandle, pSensorBuf );
		ASSERT_CAM_ERROR( error );
	}
	

	pCameraState->eCaptureState = CAM_CAPTURESTATE_PREVIEW;
	
	return CAM_ERROR_NONE;
}

/****************************************************************************************************
* State Transition: preview->idle
* Notes: 
****************************************************************************************************/
static CAM_Error _preview2idle( _CAM_ExtIspState *pCameraState )
{
	CAM_Error              error = CAM_ERROR_NONE;
	_CAM_SmartSensorConfig stSensorConfig;

	// cancel check focus thread if it exist
	if ( pCameraState->stFocusThread.iThreadID != -1 )
	{
		_DestroyThread( &(pCameraState->stFocusThread) );
		error = SmartSensor_CancelFocus( pCameraState->hSensorHandle );
	}
	
	error = _get_required_sensor_settings( pCameraState, CAM_CAPTURESTATE_IDLE, &stSensorConfig );
	ASSERT_CAM_ERROR( error );

	// sensor state switch to idle, buffers will be returned to Camera Engine 
	error = SmartSensor_Config( pCameraState->hSensorHandle, &stSensorConfig );
	ASSERT_CAM_ERROR( error );
	
	pCameraState->iActualPreviewFpsQ16 = 0;

	// make sure the buffers enqueued to sensor has been reclaimed
	if ( !stSensorConfig.bFlushed )
	{
		error = SmartSensor_Flush( pCameraState->hSensorHandle );
		ASSERT_CAM_ERROR( error );
	}

	// flush ppu src images
	if ( pCameraState->bNeedPostProcessing )
	{
		error = _flush_all_ppu_srcimg( pCameraState );
		ASSERT_CAM_ERROR( error );
	}

	// XXX: Pls be informed that here buffers in filled queue did not be moved to empty queue, this may lead a mixup of 
	//      two time-adjacent preview frames. If you think this is important, just add this operation.      

	pCameraState->eCaptureState = CAM_CAPTURESTATE_IDLE;

	return CAM_ERROR_NONE;
}

/****************************************************************************************************
* State Transition: preview->video
* Notes: video port don't support resize/rotate/color space conversion. 
****************************************************************************************************/
static CAM_Error _preview2video( _CAM_ExtIspState *pCameraState )
{
	CAM_Int32s             i      = 0;
	CAM_Size               stSize = { 0, 0 };
	CAM_Error              error  = CAM_ERROR_NONE;
	_CAM_SmartSensorConfig stSensorConfig;

	if ( !_is_valid_port( pCameraState, CAM_PORT_VIDEO ) )
	{
		return CAM_ERROR_PORTNOTVALID;
	}
	
	// cancel check focus thread if it exist
	if ( pCameraState->stFocusThread.iThreadID != -1 )
	{
		_DestroyThread( &(pCameraState->stFocusThread) );
		error = SmartSensor_CancelFocus( pCameraState->hSensorHandle );
	}

	error = _get_required_sensor_settings( pCameraState, CAM_CAPTURESTATE_VIDEO, &stSensorConfig );
	ASSERT_CAM_ERROR( error );

	error = SmartSensor_Config( pCameraState->hSensorHandle, &stSensorConfig );
	ASSERT_CAM_ERROR( error );
	
	pCameraState->iActualPreviewFpsQ16 = stSensorConfig.iActualFpsQ16;
	pCameraState->iActualVideoFpsQ16   = stSensorConfig.iActualFpsQ16;

	// convert digital zoom
	stSize.iWidth  = stSensorConfig.iWidth;
	stSize.iHeight = stSensorConfig.iHeight;
	error = _convert_digital_zoom( pCameraState, &stSize, CAM_CAPTURESTATE_VIDEO ); 
	ASSERT_CAM_ERROR( error );

	// make sure the buffers enqueued to sensor has been reclaimed
	if ( !stSensorConfig.bFlushed )
	{
		error = SmartSensor_Flush( pCameraState->hSensorHandle );
		ASSERT_CAM_ERROR( error );
	}

	// flush ppu src buffers
	if ( pCameraState->bNeedPostProcessing )
	{
		error = _flush_all_ppu_srcimg( pCameraState );
		ASSERT_CAM_ERROR( error );
	}

	error = SmartSensor_GetBufReq( pCameraState->hSensorHandle, &stSensorConfig, &pCameraState->stCurrentSensorBufReq );
	ASSERT_CAM_ERROR( error );

	// XXX: ppu is never used in video state
	pCameraState->bNeedPostProcessing = CAM_FALSE;
	pCameraState->bUsePrivateBuffer   = CAM_FALSE; 

	// enqueue video port buffers
	for ( i = 0; i < pCameraState->stPortState[CAM_PORT_VIDEO].qEmpty.iQueueCount; i++ )
	{
		CAM_ImageBuffer *pTmpImg = NULL;

		(void)_GetQueue( &pCameraState->stPortState[CAM_PORT_VIDEO].qEmpty, i, (void**)&pTmpImg );
		ASSERT( pTmpImg != NULL );

		error = SmartSensor_Enqueue( pCameraState->hSensorHandle, pTmpImg );
		ASSERT_CAM_ERROR( error );

	}

	pCameraState->eCaptureState = CAM_CAPTURESTATE_VIDEO;

	return CAM_ERROR_NONE;
}

/****************************************************************************************************
* State Transition: preview->still
* Notes: 
****************************************************************************************************/
static CAM_Error _preview2still( _CAM_ExtIspState *pCameraState )
{
	CAM_Int32s             i      = 0;
	CAM_Int32s             ret    = 0;
	CAM_Size               stSize = { 0, 0 };
	CAM_ImageBuffer        *pTemp = NULL;
	CAM_Error              error  = CAM_ERROR_NONE;
	_CAM_ShotParam         stShotParamStatus;
	_CAM_SmartSensorConfig stSensorConfig;

	// preview & still port must be valid
	if ( !_is_valid_port( pCameraState, CAM_PORT_PREVIEW ) || !_is_valid_port( pCameraState, CAM_PORT_STILL ) )
	{
		return CAM_ERROR_PORTNOTVALID;
	}
	
	// cancel check focus thread if it exist
	if ( pCameraState->stFocusThread.iThreadID != -1 )
	{
		_DestroyThread( &(pCameraState->stFocusThread) );
		error = SmartSensor_CancelFocus( pCameraState->hSensorHandle );
	}

	error = _get_required_sensor_settings( pCameraState, CAM_CAPTURESTATE_STILL, &stSensorConfig );
	ASSERT_CAM_ERROR( error );

	error = SmartSensor_Config( pCameraState->hSensorHandle, &stSensorConfig );
	ASSERT_CAM_ERROR( error );
	
	pCameraState->iActualPreviewFpsQ16 = 0;

	// convert digital zoom
	stSize.iWidth  = stSensorConfig.iWidth;
	stSize.iHeight = stSensorConfig.iHeight;
	error = _convert_digital_zoom( pCameraState, &stSize, CAM_CAPTURESTATE_STILL ); 
	ASSERT_CAM_ERROR( error );

	// make sure the buffers enqueued to sensor has been reclaimed
	if ( !stSensorConfig.bFlushed )
	{
		error = SmartSensor_Flush( pCameraState->hSensorHandle );
		ASSERT_CAM_ERROR( error );
	}

	// flush all ppu src images
	if ( pCameraState->bNeedPostProcessing )
	{
		error = _flush_all_ppu_srcimg( pCameraState );
		ASSERT_CAM_ERROR( error );
	}

	error = SmartSensor_GetBufReq( pCameraState->hSensorHandle, &stSensorConfig, &pCameraState->stCurrentSensorBufReq );
	ASSERT_CAM_ERROR( error );

	error = _is_need_ppu( &(pCameraState->stPortState[CAM_PORT_STILL]), &stSensorConfig, pCameraState->iPpuDigitalZoomQ16,
	                      &(pCameraState->bNeedPostProcessing), &(pCameraState->bUsePrivateBuffer) );
	ASSERT_CAM_ERROR( error );

	for ( i = 0; i < pCameraState->stPortState[CAM_PORT_STILL].qEmpty.iQueueCount; i++ )
	{
		CAM_ImageBuffer *pUsrBuf    = NULL;
		CAM_ImageBuffer *pSensorBuf = NULL;

		ret = _GetQueue( &pCameraState->stPortState[CAM_PORT_STILL].qEmpty, i, (void**)&pUsrBuf );
		ASSERT( ret == 0 );

		if ( pCameraState->bNeedPostProcessing )
		{
			TRACE( CAM_INFO, "Info: post processing unit is used in still state to enable JPEG compressor!\n" );
	
			error = _request_ppu_srcimg( pCameraState, &pCameraState->stCurrentSensorBufReq, pUsrBuf, &pSensorBuf );
			ASSERT_CAM_ERROR( error );
		}
		else
		{
			pSensorBuf = pUsrBuf;
		}
		error = SmartSensor_Enqueue( pCameraState->hSensorHandle, pSensorBuf );
		ASSERT_CAM_ERROR( error );
	}

	// clear the preview buffers so that it won't be mixed up with the snapshot images
	while ( _DeQueue( &pCameraState->stPortState[CAM_PORT_PREVIEW].qFilled, (void**)&pTemp ) == 0 )
	{
		ret = _EnQueue( &pCameraState->stPortState[CAM_PORT_PREVIEW].qEmpty, pTemp );
		ASSERT( ret == 0 );
	}

	// In still image capture state, we only capture the given number of images (iStillBurstCount)
	// while one frame is captured and processed, event CAM_EVENT_FRAME_READY is sent;
	// while all capture is done, we'll sent out an event CAM_EVENT_STILL_SHUTTERCLOSED;
	// while all images been processed, CAM_EVENT_STILL_ALLPROCESSED will be sent.
	error = SmartSensor_GetShotParam( pCameraState->hSensorHandle, &stShotParamStatus );
	ASSERT_CAM_ERROR( error );

	pCameraState->iStillRestCount += stShotParamStatus.iBurstCnt;

	pCameraState->eCaptureState   = CAM_CAPTURESTATE_STILL;

	return CAM_ERROR_NONE;
}

/****************************************************************************************************
* State Transition: video->preview
* Notes: 
****************************************************************************************************/
static CAM_Error _video2preview( _CAM_ExtIspState *pCameraState )
{
	CAM_Int32s             i      = 0;
	CAM_Int32s             ret    = 0;
	CAM_Size               stSize = { 0, 0 };
	CAM_Error              error  = CAM_ERROR_NONE;
	_CAM_SmartSensorConfig stSensorConfig;

	error = _get_required_sensor_settings( pCameraState, CAM_CAPTURESTATE_PREVIEW, &stSensorConfig );
	ASSERT_CAM_ERROR( error );

	error = SmartSensor_Config( pCameraState->hSensorHandle, &stSensorConfig );
	ASSERT_CAM_ERROR( error );
	
	pCameraState->iActualVideoFpsQ16   = 0;
	pCameraState->iActualPreviewFpsQ16 = stSensorConfig.iActualFpsQ16;

	// convert digital zoom
	stSize.iWidth  = stSensorConfig.iWidth;
	stSize.iHeight = stSensorConfig.iHeight;
	error = _convert_digital_zoom( pCameraState, &stSize, CAM_CAPTURESTATE_PREVIEW ); 
	ASSERT_CAM_ERROR( error );

	// make sure the buffers enqueued to sensor has been reclaimed
	if ( !stSensorConfig.bFlushed )
	{
		error = SmartSensor_Flush( pCameraState->hSensorHandle );
		ASSERT_CAM_ERROR( error );
	}

	error = SmartSensor_GetBufReq( pCameraState->hSensorHandle, &stSensorConfig, &pCameraState->stCurrentSensorBufReq );
	ASSERT_CAM_ERROR( error );

	error = _is_need_ppu( &(pCameraState->stPortState[CAM_PORT_PREVIEW]), &stSensorConfig, pCameraState->iPpuDigitalZoomQ16,
	                      &(pCameraState->bNeedPostProcessing), &(pCameraState->bUsePrivateBuffer) );
	ASSERT_CAM_ERROR( error );

	for ( i = 0; i < pCameraState->stPortState[CAM_PORT_PREVIEW].qEmpty.iQueueCount; i++ )
	{
		CAM_ImageBuffer *pUsrBuf    = NULL;
		CAM_ImageBuffer *pSensorBuf = NULL;

		ret = _GetQueue( &pCameraState->stPortState[CAM_PORT_PREVIEW].qEmpty, i, (void**)&pUsrBuf );
		ASSERT( ret == 0 );

		if ( pCameraState->bNeedPostProcessing )
		{
			TRACE( CAM_INFO, "Info: Post Processing is enabled in preview state!\n" );
			error = _request_ppu_srcimg( pCameraState, &pCameraState->stCurrentSensorBufReq, pUsrBuf, &pSensorBuf );
			ASSERT_CAM_ERROR( error );
		}
		else
		{
			pSensorBuf = pUsrBuf;
			TRACE( CAM_INFO, "Info: Post Processing is NOT enabled in preview state!\n" );
		}

		error = SmartSensor_Enqueue( pCameraState->hSensorHandle, pSensorBuf );
		ASSERT_CAM_ERROR( error );
	}


	pCameraState->eCaptureState = CAM_CAPTURESTATE_PREVIEW;

	return CAM_ERROR_NONE;
}

/****************************************************************************************************
* State Transition: still->preview
* Notes: 
****************************************************************************************************/
static CAM_Error _still2preview( _CAM_ExtIspState *pCameraState )
{
	CAM_Int32s             i      = 0;
	CAM_Int32s             ret    = 0;
	CAM_Size               stSize = { 0, 0 };
	CAM_Error              error  = CAM_ERROR_NONE;
	_CAM_SmartSensorConfig stSensorConfig;

	// preview port must be valid
	if ( !_is_valid_port( pCameraState, CAM_PORT_PREVIEW ) )
	{
		return CAM_ERROR_PORTNOTVALID;
	}

	error = _get_required_sensor_settings( pCameraState, CAM_CAPTURESTATE_PREVIEW, &stSensorConfig );
	ASSERT_CAM_ERROR( error );

	error = SmartSensor_Config( pCameraState->hSensorHandle, &stSensorConfig );
	ASSERT_CAM_ERROR( error );
	
	pCameraState->iActualPreviewFpsQ16 = stSensorConfig.iActualFpsQ16;

	// convert digital zoom
	stSize.iWidth  = stSensorConfig.iWidth;
	stSize.iHeight = stSensorConfig.iHeight;
	error = _convert_digital_zoom( pCameraState, &stSize, CAM_CAPTURESTATE_PREVIEW ); 
	ASSERT_CAM_ERROR( error );

	// make sure the buffers enqueued to sensor has been reclaimed
	if ( !stSensorConfig.bFlushed )
	{
		error = SmartSensor_Flush( pCameraState->hSensorHandle );
		ASSERT_CAM_ERROR( error );
	}

	// flush all ppu src images
	if ( pCameraState->bNeedPostProcessing )
	{
		error = _flush_all_ppu_srcimg( pCameraState );
		ASSERT_CAM_ERROR( error );
	}

	error = SmartSensor_GetBufReq( pCameraState->hSensorHandle, &stSensorConfig, &pCameraState->stCurrentSensorBufReq );
	ASSERT_CAM_ERROR( error );

	error = _is_need_ppu( &(pCameraState->stPortState[CAM_PORT_PREVIEW]), &stSensorConfig, pCameraState->iPpuDigitalZoomQ16,
	                      &(pCameraState->bNeedPostProcessing), &(pCameraState->bUsePrivateBuffer) );
	ASSERT_CAM_ERROR( error );

	for ( i = 0; i < pCameraState->stPortState[CAM_PORT_PREVIEW].qEmpty.iQueueCount; i++ )
	{
		CAM_ImageBuffer *pUsrBuf    = NULL;
		CAM_ImageBuffer *pSensorBuf = NULL;

		ret = _GetQueue( &pCameraState->stPortState[CAM_PORT_PREVIEW].qEmpty, i, (void**)&pUsrBuf );
		ASSERT( ret == 0 );

		if ( pCameraState->bNeedPostProcessing )
		{
			TRACE( CAM_INFO, "Info: Post Processing is enabled in preview state!\n" );
			error = _request_ppu_srcimg( pCameraState, &pCameraState->stCurrentSensorBufReq, pUsrBuf, &pSensorBuf );
			ASSERT_CAM_ERROR( error );
		}
		else
		{
			pSensorBuf = pUsrBuf;
			TRACE( CAM_INFO, "Info: Post Processing is NOT enabled in preview state!\n" );
		}

		error = SmartSensor_Enqueue( pCameraState->hSensorHandle, pSensorBuf );
		ASSERT_CAM_ERROR( error );
	}

	pCameraState->eCaptureState = CAM_CAPTURESTATE_PREVIEW;

	return CAM_ERROR_NONE;
}

/* port protection */
static CAM_Error _lock_port( _CAM_ExtIspState *pCameraState, CAM_Int32s iPortId )
{
	CAM_Int32s ret = 0;

	_CHECK_BAD_ANY_PORT_ID( iPortId );

	if ( iPortId == CAM_PORT_ANY )
	{
		CAM_Int32s i = 0;

		for ( i = CAM_PORT_PREVIEW; i <= CAM_PORT_STILL; i++ )
		{
			ret = IPP_MutexLock( pCameraState->stPortState[i].hMutex, INFINITE_WAIT, NULL );
			ASSERT( ret == 0 );
		}
	}
	else
	{
		ret = IPP_MutexLock( pCameraState->stPortState[iPortId].hMutex, INFINITE_WAIT, NULL );
		ASSERT( ret == 0 );
	}

	return CAM_ERROR_NONE;

}

static CAM_Error _unlock_port( _CAM_ExtIspState *pCameraState, CAM_Int32s iPortId )
{
	CAM_Int32s ret = 0;

	_CHECK_BAD_ANY_PORT_ID( iPortId );

	if ( iPortId == CAM_PORT_ANY )
	{
		CAM_Int32s i   = 0;

		for ( i = CAM_PORT_STILL; i >= CAM_PORT_PREVIEW; i-- )
		{
			ret = IPP_MutexUnlock( pCameraState->stPortState[i].hMutex );
			ASSERT( ret == 0 );
		}	

	}
	else
	{

		ret = IPP_MutexUnlock( pCameraState->stPortState[iPortId].hMutex );
		ASSERT( ret == 0 );	
	}

	return CAM_ERROR_NONE;

}


/* input buffer check */
static CAM_Error _check_image_buffer( CAM_ImageBuffer *pBuf, _CAM_PortState *pPortState )
{
	CAM_Error          error    = CAM_ERROR_NONE;
	CAM_Int32s         i        = 0;
	CAM_Int32s         iIndex   = -1;
	CAM_Int32s         ret      = 0;
	CAM_ImageBufferReq *pBufReq = NULL;

	_CHECK_BAD_POINTER( pBuf );
	_CHECK_BAD_POINTER( pPortState );

	// first check duplicate enqueue
	ret = _RetrieveQueue( &(pPortState->qEmpty), pBuf, &iIndex );
	ASSERT( ret == 0 );
	if ( iIndex != -1 )
	{
		return CAM_WARN_DUPLICATEBUFFER;
	}

	ret = _RetrieveQueue( &(pPortState->qFilled), pBuf, &iIndex );
	ASSERT( ret == 0 );
	if ( iIndex != -1 )
	{
		return CAM_WARN_DUPLICATEBUFFER;
	}

	pBufReq = &(pPortState->stBufReq);

	if ( ( (pBuf->iFlag & BUF_FLAG_PHYSICALCONTIGUOUS) && (pBuf->iFlag & BUF_FLAG_NONPHYSICALCONTIGUOUS) ) ||
	     ( !(pBuf->iFlag & BUF_FLAG_PHYSICALCONTIGUOUS) && !(pBuf->iFlag & BUF_FLAG_NONPHYSICALCONTIGUOUS) ) )
	{
		TRACE( CAM_ERROR, "Error: bad buffer( BUF_FLAG_PHYSICALCONTIGUOUS and BUF_FLAG_NONPHYSICALCONTIGUOUS must be set exclusively )!\n" );
		return CAM_ERROR_BADBUFFER;
	}

	if ( ( (pBuf->iFlag & BUF_FLAG_L1CACHEABLE) && (pBuf->iFlag & BUF_FLAG_L1NONCACHEABLE) ) ||
	     ( !(pBuf->iFlag & BUF_FLAG_L1CACHEABLE) && !(pBuf->iFlag & BUF_FLAG_L1NONCACHEABLE) ) )
	{
		TRACE( CAM_ERROR, "Error: bad buffer( BUF_FLAG_L1CACHEABLE and BUF_FLAG_L1NONCACHEABLE must be set exclusively )!\n" );
		return CAM_ERROR_BADBUFFER;
	}

	if ( ( (pBuf->iFlag & BUF_FLAG_L2CACHEABLE) &&  (pBuf->iFlag & BUF_FLAG_L2NONCACHEABLE) ) ||
	     ( !(pBuf->iFlag & BUF_FLAG_L2CACHEABLE) && !(pBuf->iFlag & BUF_FLAG_L2NONCACHEABLE) ) )
	{
		TRACE( CAM_ERROR, "Error: bad buffer( BUF_FLAG_L2CACHEABLE and BUF_FLAG_L2NONCACHEABLE must be set exclusively )!\n" );
		return CAM_ERROR_BADBUFFER;
	}

	if ( ( (pBuf->iFlag & BUF_FLAG_BUFFERABLE) &&  (pBuf->iFlag & BUF_FLAG_UNBUFFERABLE) ) ||
	     ( !(pBuf->iFlag & BUF_FLAG_BUFFERABLE) && !(pBuf->iFlag & BUF_FLAG_UNBUFFERABLE) ) )
	{
		TRACE( CAM_ERROR, "Error: bad buffer( BUF_FLAG_BUFFERABLE and BUF_FLAG_UNBUFFERABLE must be set exclusively )!\n" );
		return CAM_ERROR_BADBUFFER;
	}

	if ( !(
	       ( (pBuf->iFlag & BUF_FLAG_YUVPLANER) && !(pBuf->iFlag & BUF_FLAG_YUVBACKTOBACK) && !(pBuf->iFlag & BUF_FLAG_YVUBACKTOBACK) ) ||
	       (!(pBuf->iFlag & BUF_FLAG_YUVPLANER) &&  (pBuf->iFlag & BUF_FLAG_YUVBACKTOBACK) && !(pBuf->iFlag & BUF_FLAG_YVUBACKTOBACK) ) ||
	       (!(pBuf->iFlag & BUF_FLAG_YUVPLANER) && !(pBuf->iFlag & BUF_FLAG_YUVBACKTOBACK) &&  (pBuf->iFlag & BUF_FLAG_YVUBACKTOBACK) )
	      ) )
	{
		TRACE( CAM_ERROR, "Error: bad buffer( BUF_FLAG_YUVPLANER and BUF_FLAG_YUVBACKTOBACK and BUF_FLAG_YVUBACKTOBACK must be set exclusively )!\n" );
		return CAM_ERROR_BADBUFFER;
	}

	if ( ( (pBuf->iFlag & BUF_FLAG_ALLOWPADDING) &&  (pBuf->iFlag & BUF_FLAG_FORBIDPADDING) ) ||
	     ( !(pBuf->iFlag & BUF_FLAG_ALLOWPADDING) && !(pBuf->iFlag & BUF_FLAG_FORBIDPADDING) ) )
	{
		TRACE( CAM_ERROR, "Error: bad buffer( BUF_FLAG_ALLOWPADDING and BUF_FLAG_FORBIDPADDING must be set exclusively )!\n" );
		return CAM_ERROR_BADBUFFER;
	}

	if ( pBuf->iFlag & ~pBufReq->iFlagAcceptable )
	{
		TRACE( CAM_ERROR, "Error: bad buffer( iFlag (= 0x%x) is not acceptable (iFlagAcceptable = 0x%x) )!\n", pBuf->iFlag, pBufReq->iFlagAcceptable );
		return CAM_ERROR_BADBUFFER;
	}

	for( i = 0; i < 3; i++ )
	{
		if ( pBufReq->iMinBufLen[i] <= 0 )
		{
			continue;
		}

		// check iStep[i]
		if ( pBufReq->iFlagAcceptable & BUF_FLAG_FORBIDPADDING )
		{
			if ( pBuf->iStep[i] != pBufReq->iMinStep[i] )
			{
				TRACE( CAM_ERROR, "Error: BUF_FLAG_FORBIDPADDING flag is set, hence CAM_ImageBuffer::iStep[%d] (%d) should be equal to \
				       CAM_ImageBufferReq::iMinStep[%d] (%d)!\n", i, pBuf->iStep[i], i, pBufReq->iMinStep[i] );
				return CAM_ERROR_BADBUFFER;
			}
		}
		else
		{
			if ( pBuf->iStep[i] < pBufReq->iMinStep[i] )
			{
				TRACE( CAM_ERROR, "Error: BUF_FLAG_FORBIDPADDING flag is clear, hence CAM_ImageBuffer::iStep[%d] (%d) should be no less \
				       than CAM_ImageBufferReq::iMinStep[%d] (%d)!\n", i, pBuf->iStep[i], i, pBufReq->iMinStep[i] );
				return CAM_ERROR_BADBUFFER;
			}
		}

		// check row align
		if ( pBuf->iStep[i] & (pBufReq->iRowAlign[i] - 1) )
		{
			TRACE( CAM_ERROR, "Error: CAM_ImageBuffer::iStep[%d] (%d) should align to CAM_ImageBufferReq::iRowAlign[%d] (%d)!\n",
			       i, pBuf->iStep[i], i, pBufReq->iRowAlign[i] );
			return CAM_ERROR_BADBUFFER;
		}

		// check allocate len
		if ( pBuf->iAllocLen[i] < pBufReq->iMinBufLen[i] + pBuf->iOffset[i] )
		{
			TRACE( CAM_ERROR, "Error: CAM_ImageBuffer::iAllocLen[%d] (%d) should be no less than \
			       CAM_ImageBufferReq::iMinBufLen[%d] (%d) + CAM_ImageBuffer::iOffset[%d] (%d)!\n", \
			       i, pBuf->iAllocLen[i], i, pBufReq->iMinBufLen[i], i, pBuf->iOffset[i] );
			return CAM_ERROR_BADBUFFER;
		}

		// check pointer
		if ( pBufReq->iMinBufLen[i] > 0 && pBuf->pBuffer[i] == NULL )
		{
			TRACE( CAM_ERROR, "Error: CAM_ImageBuffer::pBuffer[%d] (%p) should point to valid address!\n", i, pBuf->pBuffer[i] );
			return CAM_ERROR_BADBUFFER;
		}

		if ( (CAM_Int32s)(pBuf->pBuffer[i] + pBuf->iOffset[i]) & (pBufReq->iAlignment[i] - 1) )
		{
			TRACE( CAM_ERROR, "Error: CAM_ImageBuffer::pBuffer[%d] (%p) + CAM_ImageBuffer::iOffset[%d] (%d) should align to \
			       CAM_ImageBufferReq::iAlignment[%d] (%d) byte!\n", i, pBuf->pBuffer[i], i, pBuf->iOffset[i], i, pBufReq->iAlignment[i] );
			return CAM_ERROR_BADBUFFER;
		}
	}

	if ( !(pBufReq->iFlagAcceptable & BUF_FLAG_YUVPLANER) )
	{
		// all three buffers must be back-to-back
		if ( pBufReq->iFlagAcceptable & BUF_FLAG_YUVBACKTOBACK )
		{
			// back to back layout in YUV order
			if ( pBufReq->iMinBufLen[1] > 0 &&
			     pBuf->pBuffer[0] + pBuf->iOffset[0] + pBufReq->iMinBufLen[0] != pBuf->pBuffer[1] + pBuf->iOffset[1] )
			{
				TRACE( CAM_ERROR, "Error: The input buffer must in back-to-back layout!\n" );
				return CAM_ERROR_NOTSUPPORTEDARG;
			}

			if ( pBufReq->iMinBufLen[2] > 0 &&
			     pBuf->pBuffer[1] + pBuf->iOffset[1] + pBufReq->iMinBufLen[1] != pBuf->pBuffer[2] + pBuf->iOffset[2] )
			{
				TRACE( CAM_ERROR, "Error: The input buffer must in back-to-back layout!\n" );
				return CAM_ERROR_NOTSUPPORTEDARG;
			}
		}
		else if ( pBufReq->iFlagAcceptable & BUF_FLAG_YVUBACKTOBACK )
		{
			// back to back layout in YVU order
			if ( pBufReq->iMinBufLen[2] > 0 &&
			     pBuf->pBuffer[0] + pBuf->iOffset[0] + pBufReq->iMinBufLen[0] != pBuf->pBuffer[2] + pBuf->iOffset[2] )
			{
				TRACE( CAM_ERROR, "Error: The input buffer must in back-to-back layout!\n" );
				return CAM_ERROR_NOTSUPPORTEDARG;
			}

			if ( pBufReq->iMinBufLen[1] > 0 &&
			     pBuf->pBuffer[2] + pBuf->iOffset[2] + pBufReq->iMinBufLen[2] != pBuf->pBuffer[1] + pBuf->iOffset[1] )
			{
				TRACE( CAM_ERROR, "Error: The input buffer must in back-to-back layout!\n" );
				return CAM_ERROR_NOTSUPPORTEDARG;
			}
		}
	}

	return error;
}

/* check whether ppu in needed in current usage case */
static CAM_Error _is_need_ppu( _CAM_PortState *pPortState, _CAM_SmartSensorConfig *pSensorConfig, CAM_Int32s iPpuDigitalZoomQ16,
                               CAM_Bool *pbNeedPostProcessing, CAM_Bool *pbNeedPrivateBuffer )
{
	*pbNeedPostProcessing = ( pSensorConfig->iWidth  != pPortState->iWidth  || pSensorConfig->iHeight != pPortState->iHeight   ||
	                          pSensorConfig->eFormat != pPortState->eFormat || pPortState->eRotation  != CAM_FLIPROTATE_NORMAL ||
	                          iPpuDigitalZoomQ16 > ( 1 << 16 ) );

	if ( *pbNeedPostProcessing == CAM_TRUE )
	{
		if ( pSensorConfig->eFormat == CAM_IMGFMT_YCbCr420P && ( pPortState->eFormat == CAM_IMGFMT_YCbCr420SP || pPortState->eFormat == CAM_IMGFMT_YCrCb420SP ) )
		{
			*pbNeedPrivateBuffer = CAM_FALSE;
		}
		else
		{
			*pbNeedPrivateBuffer = CAM_TRUE;
		}
	}
	else
	{
		*pbNeedPrivateBuffer = CAM_FALSE;
	}

	return CAM_ERROR_NONE;
}


/* ppu buffer management */
static CAM_Error _request_ppu_srcimg( _CAM_ExtIspState *pCameraState, CAM_ImageBufferReq *pBufReq, CAM_ImageBuffer *pUsrImgBuf, CAM_ImageBuffer **ppImgBuf )
{
	CAM_Int32s      i         = 0;
	CAM_Int32s      iTotalLen = 0;
	CAM_ImageBuffer *pTmpImg  = NULL;

	ASSERT( pCameraState->bNeedPostProcessing == CAM_TRUE );

	if ( !pCameraState->bUsePrivateBuffer )
	{
		// just reuse the user image buffer
		for ( i = 0; i < CAM_MAX_PORT_BUF_CNT; i++ )
		{
			if ( !pCameraState->bPpuSrcImgUsed[i] && pCameraState->stPpuSrcImg[i].pUserData == pUsrImgBuf )
			{
				break;
			}
		}

		if ( i < CAM_MAX_PORT_BUF_CNT )
		{
			pCameraState->bPpuSrcImgUsed[i] = CAM_TRUE;
			*ppImgBuf                       = &pCameraState->stPpuSrcImg[i];
			
			return CAM_ERROR_NONE;
		}

		for ( i = 0; i < CAM_MAX_PORT_BUF_CNT; i++ )
		{
			if ( !pCameraState->bPpuSrcImgUsed[i] && pCameraState->stPpuSrcImg[i].pUserData == NULL )
			{
				break;
			}
		}
		// we need to check if we have enough structures to hold the buffer
		if ( i >= CAM_MAX_PORT_BUF_CNT )
		{
			TRACE( CAM_ERROR, "Error: too many buffers to be handled by camera-engine( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
			return CAM_ERROR_OUTOFRESOURCE;
		}

		pTmpImg = &pCameraState->stPpuSrcImg[i];

		pTmpImg->eFormat       = pBufReq->eFormat;
		pTmpImg->iOffset[0]    = 0;
		pTmpImg->iOffset[1]    = 0;
		pTmpImg->iOffset[2]    = 0;
		pTmpImg->iStep[0]      = pBufReq->iMinStep[0];
		pTmpImg->iStep[1]      = pBufReq->iMinStep[1];
		pTmpImg->iStep[2]      = pBufReq->iMinStep[2];
		pTmpImg->iWidth        = pBufReq->iWidth;
		pTmpImg->iHeight       = pBufReq->iHeight;
		pTmpImg->iAllocLen[0]  = pBufReq->iMinBufLen[0];
		pTmpImg->iAllocLen[1]  = pBufReq->iMinBufLen[1];
		pTmpImg->iAllocLen[2]  = pBufReq->iMinBufLen[2];
		pTmpImg->iFilledLen[0] = 0;
		pTmpImg->iFilledLen[1] = 0;
		pTmpImg->iFilledLen[2] = 0;
		pTmpImg->iFlag         = pUsrImgBuf->iFlag;
		pTmpImg->iUserIndex    = i;
		pTmpImg->pBuffer[0]    = pUsrImgBuf->pBuffer[0];
		pTmpImg->pBuffer[1]    = (CAM_Int8u*)_ALIGN_TO( pTmpImg->pBuffer[0] + pTmpImg->iAllocLen[0], pBufReq->iAlignment[1] );
		pTmpImg->pBuffer[2]    = (CAM_Int8u*)_ALIGN_TO( pTmpImg->pBuffer[1] + pTmpImg->iAllocLen[1], pBufReq->iAlignment[2] );
		pTmpImg->iPhyAddr[0]   = 0;
		pTmpImg->iPhyAddr[1]   = 0;
		pTmpImg->iPhyAddr[2]   = 0;
		pTmpImg->pUserData     = (void*)pUsrImgBuf;

		pTmpImg->bEnableShotInfo = pUsrImgBuf->bEnableShotInfo;

		pCameraState->bPpuSrcImgUsed[i] = CAM_TRUE;

		*ppImgBuf = pTmpImg;

	}
	else
	{
		// search all allocated privated images and try to find one that is not in use
		for ( i = 0; i < pCameraState->iPpuSrcImgAllocateCnt; i++ )
		{
			if ( !pCameraState->bPpuSrcImgUsed[i] )
			{
				break;
			}
		}

		if ( i < pCameraState->iPpuSrcImgAllocateCnt )
		{	
			// found! then use it
			*ppImgBuf                       = &pCameraState->stPpuSrcImg[i];
			pCameraState->bPpuSrcImgUsed[i] = CAM_TRUE;

			return CAM_ERROR_NONE;
		}

		// not found, so we need to allocate a new one. and before that, we need to check if we have enough structures to hold the buffer
		if ( i >= CAM_MAX_PORT_BUF_CNT )
		{
			TRACE( CAM_ERROR, "Error: too many buffers to be handled by camera-engine( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
			return CAM_ERROR_OUTOFRESOURCE;
		}

		// start to allocate a new priviate image
		pTmpImg = &pCameraState->stPpuSrcImg[i];

		pTmpImg->eFormat = pBufReq->eFormat;
		pTmpImg->iOffset[0] = 0;
		pTmpImg->iOffset[1] = 0;
		pTmpImg->iOffset[2] = 0;
		pTmpImg->iStep[0] = pBufReq->iMinStep[0];
		pTmpImg->iStep[1] = pBufReq->iMinStep[1];
		pTmpImg->iStep[2] = pBufReq->iMinStep[2];
		pTmpImg->iWidth   = pBufReq->iWidth;
		pTmpImg->iHeight  = pBufReq->iHeight;
		pTmpImg->iAllocLen[0] = pBufReq->iMinBufLen[0];
		pTmpImg->iAllocLen[1] = pBufReq->iMinBufLen[1];
		pTmpImg->iAllocLen[2] = pBufReq->iMinBufLen[2];
		pTmpImg->iFilledLen[0] = 0;
		pTmpImg->iFilledLen[1] = 0;
		pTmpImg->iFilledLen[2] = 0;
		pTmpImg->iUserIndex    = i;
		iTotalLen = pTmpImg->iAllocLen[0] + pBufReq->iAlignment[0] +
		            pTmpImg->iAllocLen[1] + pBufReq->iAlignment[1] +
		            pTmpImg->iAllocLen[2] + pBufReq->iAlignment[2];
		pTmpImg->pUserData  = osalbmm_malloc( iTotalLen, OSALBMM_ATTR_DEFAULT ); // cachable & bufferable
		if ( pTmpImg->pUserData == NULL )
		{
			TRACE( CAM_ERROR, "Error: failed to allocate %d bytes memory ( %s, %s, %d )\n", iTotalLen, __FILE__, __FUNCTION__, __LINE__ );
			return CAM_ERROR_OUTOFMEMORY;
		}

		pTmpImg->pBuffer[0] = (CAM_Int8u*)_ALIGN_TO( pTmpImg->pUserData, pBufReq->iAlignment[0] );
		pTmpImg->pBuffer[1] = (CAM_Int8u*)_ALIGN_TO( pTmpImg->pBuffer[0] + pTmpImg->iAllocLen[0], pBufReq->iAlignment[1] );
		pTmpImg->pBuffer[2] = (CAM_Int8u*)_ALIGN_TO( pTmpImg->pBuffer[1] + pTmpImg->iAllocLen[1], pBufReq->iAlignment[2] );

		pTmpImg->iPhyAddr[0] = (CAM_Int32u)osalbmm_get_paddr( pTmpImg->pBuffer[0] );
		pTmpImg->iPhyAddr[1] = (CAM_Int32u)osalbmm_get_paddr( pTmpImg->pBuffer[1] );
		pTmpImg->iPhyAddr[2] = (CAM_Int32u)osalbmm_get_paddr( pTmpImg->pBuffer[2] );

		pTmpImg->iFlag = BUF_FLAG_PHYSICALCONTIGUOUS |
		                 BUF_FLAG_L1CACHEABLE | BUF_FLAG_L2CACHEABLE | BUF_FLAG_BUFFERABLE |
		                 BUF_FLAG_YUVBACKTOBACK | BUF_FLAG_FORBIDPADDING;

		pTmpImg->bEnableShotInfo = pUsrImgBuf->bEnableShotInfo;

		// invalidate bmm buffer's cache line
		osalbmm_flush_cache_range( pTmpImg->pUserData, iTotalLen, OSALBMM_DMA_FROM_DEVICE );

		pCameraState->iPpuSrcImgAllocateCnt++;
		pCameraState->bPpuSrcImgUsed[i] = CAM_TRUE;

		*ppImgBuf = pTmpImg;
	}

	return CAM_ERROR_NONE;
}

static CAM_Error _release_ppu_srcimg( _CAM_ExtIspState *pCameraState, CAM_ImageBuffer *pImgBuf )
{

	CAM_Int32s iBufIndex = pImgBuf->iUserIndex;

	ASSERT( pCameraState->bNeedPostProcessing == CAM_TRUE );

	ASSERT( pCameraState->bPpuSrcImgUsed[iBufIndex] == CAM_TRUE );
	ASSERT( &pCameraState->stPpuSrcImg[iBufIndex] == pImgBuf );

	if ( !pCameraState->bUsePrivateBuffer )
	{
		ASSERT( iBufIndex >= 0 && iBufIndex < CAM_MAX_PORT_BUF_CNT );
	}
	else
	{
		ASSERT( iBufIndex >= 0 && iBufIndex < pCameraState->iPpuSrcImgAllocateCnt );
	}

	pCameraState->bPpuSrcImgUsed[iBufIndex] = CAM_FALSE;

	return CAM_ERROR_NONE;
}

static CAM_Error _flush_all_ppu_srcimg( _CAM_ExtIspState *pCameraState )
{
	CAM_Int32s i = 0;

	if ( pCameraState->bUsePrivateBuffer && ( pCameraState->iPpuSrcImgAllocateCnt > 0 ) )
	{
		for( i = pCameraState->iPpuSrcImgAllocateCnt - 1; i >= 0; i-- )
		{
			osalbmm_free( pCameraState->stPpuSrcImg[i].pUserData );
		}
	}

	pCameraState->iPpuSrcImgAllocateCnt = 0;
	
	for( i = 0; i < CAM_MAX_PORT_BUF_CNT; i++ )
	{
		memset( &pCameraState->stPpuSrcImg[i], 0, sizeof( CAM_ImageBuffer ) );
		pCameraState->bPpuSrcImgUsed[i] = CAM_FALSE;
	}

	return CAM_ERROR_NONE;
}

