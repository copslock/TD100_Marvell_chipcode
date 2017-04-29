/*******************************************************************************
//(C) Copyright [2010 - 2011] Marvell International Ltd.
//All Rights Reserved
*******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "cam_log.h"
#include "cam_utility.h"


#include "cam_extisp_sensorwrapper.h"
#include "cam_extisp_v4l2base.h"
#include "cam_extisp_basicsensor.h"


// NOTE: if you want to enable static resolution table to bypass sensor dynamically detect to save camera-off to viwerfinder-on latency, 
//       you can fill the following four tables according to your sensor's capability. And open macro 
//       BUILD_OPTION_STARTEGY_DISABLE_DYNAMIC_SENSOR_DETECT in cam_extisp_buildopt.h

/* Basic Sensor video/preview resolution table */
CAM_Size _BasicSensorVideoResTable[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT] = 
{
	{0, 0},
};

/* Basic Sensor still resolution table */
CAM_Size _BasicSensorStillResTable[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT] = 
{ 
	{0, 0},
};

/* Basic Sensor video/preview format table */
CAM_ImageFormat _BasicSensorVideoFormatTable[CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT] = 
{ 
	0, 
};

/* Basic Sensor still format table */
CAM_ImageFormat _BasicSensorStillFormatTable[CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT] = 
{
	0,
};


CAM_FlipRotate _BasicSensorRotationTable[] = 
{
	CAM_FLIPROTATE_NORMAL,
};


CAM_ShotMode _BasicSensorShotModeTable[] = 
{
	CAM_SHOTMODE_AUTO,
};

_CAM_RegEntry _BasicSensorExpMeter_Mean[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _BasicSensorExpMeter[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_EXPOSUREMETERMODE_MEAN, _BasicSensorExpMeter_Mean),
};

_CAM_RegEntry _BasicSensorIsoMode_Auto[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _BasicSensorIsoMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_ISO_AUTO, _BasicSensorIsoMode_Auto),
};

_CAM_RegEntry _BasicSensorBdFltMode_Auto[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _BasicSensorBdFltMode_Off[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _BasicSensorBdFltMode_50Hz[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _BasicSensorBdFltMode_60Hz[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _BasicSensorBdFltMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_BANDFILTER_50HZ, _BasicSensorBdFltMode_50Hz),
};

_CAM_RegEntry _BasicSensorFlashMode_Off[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _BasicSensorFlashMode_On[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _BasicSensorFlashMode_Auto[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _BasicSensorFlashMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_FLASH_OFF,	_BasicSensorFlashMode_Off),
};

_CAM_RegEntry _BasicSensorWBMode_Auto[] =
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _BasicSensorWBMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_WHITEBALANCEMODE_AUTO, _BasicSensorWBMode_Auto),
};

_CAM_RegEntry _BasicSensorFocusMode_Infinity[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _BasicSensorFocusMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_FOCUS_INFINITY,  _BasicSensorFocusMode_Infinity),
};

_CAM_RegEntry _BasicSensorColorEffectMode_Off[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _BasicSensorColorEffectMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_COLOREFFECT_OFF, _BasicSensorColorEffectMode_Off),
};


static CAM_Error _BasicSensorSaveAeAwb( const _CAM_SmartSensorConfig*, void* );
static CAM_Error _BasicSensorRestoreAeAwb( const _CAM_SmartSensorConfig*, void* );
static CAM_Error _BasicSensorStartFlash( void* );
static CAM_Error _BasicSensorStopFlash( void* );

static CAM_Error _BasicSensorFillFrameShotInfo( BasicSensorState*, CAM_ImageBuffer* );
static CAM_Error _BasicSensorSetJpegParam( BasicSensorState*, CAM_JpegParam* );

// shot mode capability
static void _BasicSensorAutoModeCap( CAM_ShotModeCapability* );
// static void _BasicSensorManualModeCap( CAM_ShotModeCapability* );
// static void _BasicSensorNightModeCap( CAM_ShotModeCapability* );
/* shot mode cap function table */
BasicSensor_ShotModeCap _BasicSensorShotModeCap[CAM_SHOTMODE_NUM] = { _BasicSensorAutoModeCap, // CAM_SHOTMODE_AUTO = 0,
                                                                      NULL                   , // CAM_SHOTMODE_MANUAL,
                                                                      NULL                   , // CAM_SHOTMODE_PORTRAIT,
                                                                      NULL                   , // CAM_SHOTMODE_LANDSCAPE,
                                                                      NULL                   , // CAM_SHOTMODE_NIGHTPORTRAIT,
                                                                      NULL                   , // CAM_SHOTMODE_NIGHTSCENE,
                                                                      NULL                   , // CAM_SHOTMODE_CHILD,
                                                                      NULL                   , // CAM_SHOTMODE_INDOOR,
                                                                      NULL                   , // CAM_SHOTMODE_PLANTS,
                                                                      NULL                   , // CAM_SHOTMODE_SNOW,
                                                                      NULL                   , // CAM_SHOTMODE_BEACH,
                                                                      NULL                   , // CAM_SHOTMODE_FIREWORKS,
                                                                      NULL                   , // CAM_SHOTMODE_SUBMARINE, 
                                                                      NULL                   , // CAM_SHOTMODE_WHITEBOARD,    
                                                                      NULL                   , // CAM_SHOTMODE_SPORTS,
                                                                     };  

extern _SmartSensorFunc func_basicsensor; 
CAM_Error BasicSensor_SelfDetect( _SmartSensorAttri *pSensorInfo )
{
	CAM_Error error = CAM_ERROR_NONE;
	
	// NOTE:  If you open macro BUILD_OPTION_STARTEGY_DISABLE_DYNAMIC_SENSOR_DETECT in cam_extisp_buildopt.h 
	//        to bypass sensor dynamically detect to save camera-off to viwerfinder-on latency, you should initilize
	//        _BasicSensorVideoResTable/_BasicSensorStillResTable/_BasicSensorVideoFormatTable/_BasicSensorStillFormatTable manually.

#if !defined( BUILD_OPTION_STARTEGY_DISABLE_DYNAMIC_SENSOR_DETECT )	
	error = V4L2_SelfDetect( pSensorInfo, "basicsensor", &func_basicsensor,
	                         _BasicSensorVideoResTable, _BasicSensorStillResTable,
	                         _BasicSensorVideoFormatTable, _BasicSensorStillFormatTable );

#else
	{
		_V4L2SensorEntry *pSensorEntry = (_V4L2SensorEntry*)( pSensorInfo->cReserved );
		strcpy( pSensorInfo->sSensorName, "BasicSensor-unknown" );
		pSensorInfo->pFunc = &func_basicsensor;

		// FIXME: the following is just an example in Marvell platform, you should change it according to your driver implementation
		strcpy( pSensorEntry->sDeviceName, "/dev/video0" );
		pSensorEntry->iV4L2SensorID = 0;
	}
#endif

	return error;
} 


CAM_Error BasicSensor_GetCapability( _CAM_SmartSensorCapability *pCapability )
{
	CAM_Int32s i = 0;

	// preview/video supporting 
	// format
	pCapability->iSupportedVideoFormatCnt = 0;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT; i++ ) 
	{
		if ( _BasicSensorVideoFormatTable[i] == 0 )
		{
			break;
		}
		pCapability->eSupportedVideoFormat[pCapability->iSupportedVideoFormatCnt] = _BasicSensorVideoFormatTable[i];
		pCapability->iSupportedVideoFormatCnt++;
	}

	pCapability->bArbitraryVideoSize     = CAM_FALSE;
	pCapability->iSupportedVideoSizeCnt  = 0;
	pCapability->stMinVideoSize.iWidth   = _BasicSensorVideoResTable[0].iWidth;
	pCapability->stMinVideoSize.iHeight  = _BasicSensorVideoResTable[0].iHeight;
	pCapability->stMaxVideoSize.iWidth   = _BasicSensorVideoResTable[0].iWidth;
	pCapability->stMaxVideoSize.iHeight  = _BasicSensorVideoResTable[0].iHeight;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_SIZE_CNT; i++ )
	{
		if ( _BasicSensorVideoResTable[i].iWidth == 0 || _BasicSensorVideoResTable[i].iHeight == 0)
		{
			break;
		}
		pCapability->stSupportedVideoSize[pCapability->iSupportedVideoSizeCnt] = _BasicSensorVideoResTable[i];
		pCapability->iSupportedVideoSizeCnt++;

		if ( ( pCapability->stMinVideoSize.iWidth > _BasicSensorVideoResTable[i].iWidth ) || 
		     ( ( pCapability->stMinVideoSize.iWidth == _BasicSensorVideoResTable[i].iWidth ) && ( pCapability->stMinVideoSize.iHeight > _BasicSensorVideoResTable[i].iHeight ) ) ) 
		{
			pCapability->stMinVideoSize.iWidth = _BasicSensorVideoResTable[i].iWidth;
			pCapability->stMinVideoSize.iHeight = _BasicSensorVideoResTable[i].iHeight;
		}
		if ( ( pCapability->stMaxVideoSize.iWidth < _BasicSensorVideoResTable[i].iWidth) ||
		     ( ( pCapability->stMaxVideoSize.iWidth == _BasicSensorVideoResTable[i].iWidth ) && ( pCapability->stMaxVideoSize.iHeight < _BasicSensorVideoResTable[i].iHeight ) ) )
		{
			pCapability->stMaxVideoSize.iWidth = _BasicSensorVideoResTable[i].iWidth;
			pCapability->stMaxVideoSize.iHeight = _BasicSensorVideoResTable[i].iHeight;
		}
	}

	// still capture supporting
	// format
	pCapability->iSupportedStillFormatCnt           = 0;
	pCapability->stSupportedJPEGParams.bSupportJpeg = CAM_FALSE;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT; i++ ) 
	{
		if ( _BasicSensorStillFormatTable[i] == CAM_IMGFMT_JPEG )
		{
			// JPEG encoder functionalities
			pCapability->stSupportedJPEGParams.bSupportJpeg = CAM_TRUE;
			pCapability->stSupportedJPEGParams.bSupportExif = CAM_FALSE;
			pCapability->stSupportedJPEGParams.bSupportThumb = CAM_FALSE;
			pCapability->stSupportedJPEGParams.iMinQualityFactor = 80;
			pCapability->stSupportedJPEGParams.iMaxQualityFactor = 80;
		}
		if ( _BasicSensorStillFormatTable[i] == 0 )
		{
			break;
		}
		pCapability->eSupportedStillFormat[pCapability->iSupportedStillFormatCnt] = _BasicSensorStillFormatTable[i];
		pCapability->iSupportedStillFormatCnt++;
	}
	// resolution
	pCapability->bArbitraryStillSize    = CAM_FALSE;
	pCapability->iSupportedStillSizeCnt = 0;
	pCapability->stMinStillSize.iWidth  = _BasicSensorStillResTable[0].iWidth;
	pCapability->stMinStillSize.iHeight = _BasicSensorStillResTable[0].iHeight;
	pCapability->stMaxStillSize.iWidth  = _BasicSensorStillResTable[0].iWidth;
	pCapability->stMaxStillSize.iHeight = _BasicSensorStillResTable[0].iHeight;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_SIZE_CNT; i++ )
	{
		if ( _BasicSensorStillResTable[i].iWidth == 0 || _BasicSensorStillResTable[i].iHeight == 0 )
		{
			break;
		}

		pCapability->stSupportedStillSize[pCapability->iSupportedStillSizeCnt] = _BasicSensorStillResTable[i];
		pCapability->iSupportedStillSizeCnt++;

		if ( ( pCapability->stMinStillSize.iWidth > _BasicSensorStillResTable[i].iWidth ) ||
		     ( ( pCapability->stMinStillSize.iWidth == _BasicSensorStillResTable[i].iWidth ) && ( pCapability->stMinStillSize.iHeight > _BasicSensorStillResTable[i].iHeight ) ) )
		{
			pCapability->stMinStillSize.iWidth  = _BasicSensorStillResTable[i].iWidth;
			pCapability->stMinStillSize.iHeight = _BasicSensorStillResTable[i].iHeight;
		}
		if ( ( pCapability->stMaxStillSize.iWidth < _BasicSensorStillResTable[i].iWidth ) || 
		     ( ( pCapability->stMaxStillSize.iWidth == _BasicSensorStillResTable[i].iWidth ) && ( pCapability->stMaxStillSize.iHeight < _BasicSensorStillResTable[i].iHeight ) ) )
		{
			pCapability->stMaxStillSize.iWidth = _BasicSensorStillResTable[i].iWidth;
			pCapability->stMaxStillSize.iHeight = _BasicSensorStillResTable[i].iHeight;
		}
	}

	// rotate
	pCapability->iSupportedRotateCnt = _ARRAY_SIZE( _BasicSensorRotationTable );
	for ( i = 0; i < pCapability->iSupportedRotateCnt; i++ )
	{
		pCapability->eSupportedRotate[i] = _BasicSensorRotationTable[i];
	}

	pCapability->iSupportedShotModeCnt = _ARRAY_SIZE(_BasicSensorShotModeTable);
	for ( i = 0; i < pCapability->iSupportedShotModeCnt; i++ )
	{
		pCapability->eSupportedShotMode[i] = _BasicSensorShotModeTable[i];
	}

	// FIXME: all supported shot parameters
	_BasicSensorAutoModeCap( &pCapability->stSupportedShotParams );

	return CAM_ERROR_NONE;
}

CAM_Error BasicSensor_GetShotModeCapability( CAM_ShotMode eShotMode, CAM_ShotModeCapability *pShotModeCap )
{
	CAM_Int32u i;

	// BAC check
	for ( i = 0; i < _ARRAY_SIZE( _BasicSensorShotModeTable ); i++ )
	{
		if ( _BasicSensorShotModeTable[i] == eShotMode )
		{
			break;
		}
	}

	if ( i >= _ARRAY_SIZE( _BasicSensorShotModeTable ) || pShotModeCap ==NULL ) 
	{
		return CAM_ERROR_BADARGUMENT;
	}

	(void)(_BasicSensorShotModeCap[eShotMode])( pShotModeCap );

	return CAM_ERROR_NONE;
}

CAM_Error BasicSensor_Init( void **ppDevice, void *pSensorEntry )
{
	CAM_Error             error      = CAM_ERROR_NONE;
	CAM_Int32s            iSkipFrame = 0;
	_V4L2SensorAttribute  _BasicSensorAttri;
	_V4L2SensorEntry      *pSensor = (_V4L2SensorEntry*)pSensorEntry;
	BasicSensorState      *pState  = (BasicSensorState*)malloc( sizeof( BasicSensorState ) );

	memset( &_BasicSensorAttri, 0, sizeof( _V4L2SensorAttribute ) );

	*ppDevice = pState;
	if ( *ppDevice == NULL )
	{
		return CAM_ERROR_OUTOFMEMORY;
	}

	_BasicSensorAttri.stV4L2SensorEntry.iV4L2SensorID = pSensor->iV4L2SensorID;
	strcpy( _BasicSensorAttri.stV4L2SensorEntry.sDeviceName, pSensor->sDeviceName );

	/**************************************************************************************
	 * defaultly, we will skip 30 frames in FAST VALIDATION PASS to avoid potential black
	 * frame if 3A is not convergence while resolution switch. If this is not need to your 
	 * sensor, just modify iSkipFrame, if you sensor need do save/restore 3A, pls refer to
	 * your sensor vendor, and implement these functions/
	***************************************************************************************/
#if defined( BUILD_OPTION_DEBUG_DISABLE_SAVE_RESTORE_3A )
	iSkipFrame                             = 30;
	_BasicSensorAttri.fnSaveAeAwb          = NULL;
	_BasicSensorAttri.fnRestoreAeAwb       = NULL;
	_BasicSensorAttri.pSaveRestoreUserData = NULL;
	_BasicSensorAttri.fnStartFlash         = NULL;
 	_BasicSensorAttri.fnStopFlash          = NULL;
#else
	iSkipFrame                             = 2;
	_BasicSensorAttri.fnSaveAeAwb          = _BasicSensorSaveAeAwb;
	_BasicSensorAttri.fnRestoreAeAwb       = _BasicSensorRestoreAeAwb;
	_BasicSensorAttri.pSaveRestoreUserData = (void*)pState;
 	_BasicSensorAttri.fnStartFlash         =  _BasicSensorStartFlash;
 	_BasicSensorAttri.fnStopFlash          =  _BasicSensorStopFlash;
#endif

	error = V4L2_Init( &(pState->stV4L2), &(_BasicSensorAttri), iSkipFrame );

	/* here we can get default shot params */
	pState->stV4L2.stShotParam.eShotMode            = CAM_SHOTMODE_AUTO;
	pState->stV4L2.stShotParam.eExpMode             = CAM_EXPOSUREMODE_AUTO;
	pState->stV4L2.stShotParam.eExpMeterMode        = CAM_EXPOSUREMETERMODE_AUTO;
	pState->stV4L2.stShotParam.iEvCompQ16           = 0;
	pState->stV4L2.stShotParam.eIsoMode             = CAM_ISO_AUTO;
	pState->stV4L2.stShotParam.iShutterSpeedQ16     = -1;
	pState->stV4L2.stShotParam.iFNumQ16             = 1;
	pState->stV4L2.stShotParam.eBandFilterMode      = CAM_BANDFILTER_50HZ;
	pState->stV4L2.stShotParam.eWBMode              = CAM_WHITEBALANCEMODE_AUTO;
	pState->stV4L2.stShotParam.eFocusMode           = CAM_FOCUS_INFINITY;
	pState->stV4L2.stShotParam.iDigZoomQ16          = 0;
	pState->stV4L2.stShotParam.eColorEffect         = CAM_COLOREFFECT_OFF;
	pState->stV4L2.stShotParam.iSaturation          = 64;
	pState->stV4L2.stShotParam.iBrightness          = 0;
	pState->stV4L2.stShotParam.iContrast            = 0;
	pState->stV4L2.stShotParam.iSharpness           = 0;
	pState->stV4L2.stShotParam.eFlashMode           = CAM_FLASH_OFF;
	pState->stV4L2.stShotParam.bVideoStabilizer     = CAM_FALSE;
	pState->stV4L2.stShotParam.bVideoNoiseReducer   = CAM_FALSE;
	pState->stV4L2.stShotParam.bZeroShutterLag      = CAM_FALSE;
	pState->stV4L2.stShotParam.bHighDynamicRange	= CAM_FALSE;

	pState->stV4L2.stShotParam.iBurstCnt            = 1;
	/* get default JPEG params */
	pState->stV4L2.stJpegParam.iSampling      = -1; // 0 - 420, 1 - 422, 2 - 444
	pState->stV4L2.stJpegParam.iQualityFactor = 80;
	pState->stV4L2.stJpegParam.bEnableExif    = CAM_FALSE;
	pState->stV4L2.stJpegParam.bEnableThumb   = CAM_FALSE;
	pState->stV4L2.stJpegParam.iThumbWidth    = 0;
	pState->stV4L2.stJpegParam.iThumbHeight   = 0;


	return error;
}

CAM_Error BasicSensor_Deinit( void *pDevice )
{
	CAM_Error        error   = CAM_ERROR_NONE;
	BasicSensorState *pState = (BasicSensorState*)pDevice;

	error = V4L2_Deinit( &pState->stV4L2 );

	free( pDevice );

	return error;
}

CAM_Error BasicSensor_RegisterEventHandler( void *pDevice, CAM_EventHandler fnEventHandler, void *pUserData )
{
	BasicSensorState *pState = (BasicSensorState*)pDevice;
	CAM_Error        error   = CAM_ERROR_NONE;

	error = V4L2_RegisterEventHandler( &pState->stV4L2, fnEventHandler, pUserData );
	return error;
}

CAM_Error BasicSensor_Config( void *pDevice, _CAM_SmartSensorConfig *pSensorConfig )
{
	CAM_Error        error   = CAM_ERROR_NONE;
	BasicSensorState *pState = (BasicSensorState*)pDevice;

	error = V4L2_Config( &pState->stV4L2, pSensorConfig );
	if ( error != CAM_ERROR_NONE )
	{
		return error;
	}

	if ( pSensorConfig->eState != CAM_CAPTURESTATE_IDLE )
	{
		if ( pSensorConfig->eState == CAM_IMGFMT_JPEG )
		{
			error = _BasicSensorSetJpegParam( pDevice, &(pSensorConfig->stJpegParam) );
			if ( error != CAM_ERROR_NONE )
			{
				return error;
			}
		}
	}

	pState->stV4L2.stConfig = *pSensorConfig;

	return CAM_ERROR_NONE;
}

CAM_Error BasicSensor_GetBufReq( void *pDevice, _CAM_SmartSensorConfig *pSensorConfig, CAM_ImageBufferReq *pBufReq )
{
	CAM_Error        error   = CAM_ERROR_NONE;
	BasicSensorState *pState = (BasicSensorState*)pDevice;

	error = V4L2_GetBufReq( &pState->stV4L2, pSensorConfig, pBufReq );

	return error;
}

CAM_Error BasicSensor_Enqueue( void *pDevice, CAM_ImageBuffer *pImgBuf )
{
	CAM_Error        error   = CAM_ERROR_NONE;
	BasicSensorState *pState = (BasicSensorState*)pDevice;

	error = V4L2_Enqueue( &pState->stV4L2, pImgBuf );

	return error;
}

CAM_Error BasicSensor_Dequeue( void *pDevice, CAM_ImageBuffer **ppImgBuf )
{
	CAM_Error        error   = CAM_ERROR_NONE;
	BasicSensorState *pState = (BasicSensorState*)pDevice;

	error = V4L2_Dequeue( &pState->stV4L2, ppImgBuf );

	if ( error == CAM_ERROR_NONE && (*ppImgBuf)->bEnableShotInfo )
	{
		error = _BasicSensorFillFrameShotInfo( pState, *ppImgBuf );
	}

	return error;
}

CAM_Error BasicSensor_Flush( void *pDevice )
{
	CAM_Error        error   = CAM_ERROR_NONE;
	BasicSensorState *pState = (BasicSensorState*)pDevice;

	error = V4L2_Flush( &pState->stV4L2 );

	return error;
}

CAM_Error BasicSensor_SetShotParam( void *pDevice, _CAM_ShotParam *pShotParam )
{
	// TODO: here you can add your code to set shot params you supported, just like examples in ov5642.c or you can give your own style
	
	return CAM_ERROR_NONE;
}

CAM_Error BasicSensor_GetShotParam( void *pDevice, _CAM_ShotParam *pShotParam )
{
	BasicSensorState *pState = (BasicSensorState*)pDevice;
    
	*pShotParam = pState->stV4L2.stShotParam;

	return CAM_ERROR_NONE;
}

CAM_Error BasicSensor_StartFocus( void *pDevice, void *pFocusROI )
{
	// TODO: add your start focus code here,an refrence is ov5642.c

	return CAM_ERROR_NONE;
}

CAM_Error BasicSensor_CancelFocus( void *pDevice )
{
	// TODO: add yourcancel focus code here,an refrence is ov5642.c
	return CAM_ERROR_NONE;
}

CAM_Error BasicSensor_CheckFocusState( void *pDevice, CAM_Bool *pFocusAutoStopped, _CAM_FocusState *pFocusState )
{

	// TODO: add your check focus status code here,an refrence is ov5642.c
	*pFocusAutoStopped = CAM_TRUE;
	*pFocusState       = CAM_IN_FOCUS;

	return CAM_ERROR_NONE;
}


CAM_Error BasicSensor_GetDigitalZoomCapability( CAM_Int32s iWidth, CAM_Int32s iHeight, CAM_Int32s *pSensorDigZoomQ16 )
{
	// TODO: add your get zoom capability code here,an refrence is ov5642.c
	*pSensorDigZoomQ16 = ( 1 << 16 );

	return CAM_ERROR_NONE;
}

_SmartSensorFunc func_basicsensor = 
{
	BasicSensor_GetCapability,
	BasicSensor_GetShotModeCapability,

	BasicSensor_SelfDetect,
	BasicSensor_Init,
	BasicSensor_Deinit,
	BasicSensor_RegisterEventHandler,
	BasicSensor_Config,
	BasicSensor_GetBufReq,
	BasicSensor_Enqueue,
	BasicSensor_Dequeue,
	BasicSensor_Flush,
	BasicSensor_SetShotParam,
	BasicSensor_GetShotParam,

	BasicSensor_StartFocus,
	BasicSensor_CancelFocus,
	BasicSensor_CheckFocusState,
	BasicSensor_GetDigitalZoomCapability,

};

static CAM_Error _BasicSensorSetJpegParam( BasicSensorState *pState, CAM_JpegParam *pJpegParam )
{
	if ( pJpegParam->bEnableExif )
	{
		return CAM_ERROR_NOTSUPPORTEDARG;
	}

	if ( pJpegParam->bEnableThumb )
	{
		return CAM_ERROR_NOTSUPPORTEDARG;
	}

	if ( pJpegParam->iQualityFactor != 80 )
	{
		return CAM_ERROR_NOTSUPPORTEDARG;
	}
	else
	{
		pState->stV4L2.stJpegParam.iQualityFactor = 80;
	}

	return CAM_ERROR_NONE;
}

/*-------------------------------------------------------------------------------------------------------------------------------------
 * BasicSensor shotmode capability
 * TODO: if you enable new shot mode, pls add a correspoding modcap function here, and add it to BasicSensor_shotModeCap _BasicSensorShotModeCap array
 *------------------------------------------------------------------------------------------------------------------------------------*/
static void _BasicSensorAutoModeCap( CAM_ShotModeCapability *pShotModeCap )
{
	// exposure mode
	pShotModeCap->iSupportedExpModeCnt  = 1;
	pShotModeCap->eSupportedExpMode[0]  = CAM_EXPOSUREMODE_AUTO;
	
	// exposure metering mode 
	pShotModeCap->iSupportedExpMeterCnt = 1;
	pShotModeCap->eSupportedExpMeter[0] = CAM_EXPOSUREMETERMODE_AUTO;

	// EV compensation 
	pShotModeCap->iEVCompStepQ16 = 0;
	pShotModeCap->iMinEVCompQ16  = 0;
	pShotModeCap->iMaxEVCompQ16  = 0;

	// ISO mode 
	pShotModeCap->iSupportedIsoModeCnt = 1;
	pShotModeCap->eSupportedIsoMode[0] = CAM_ISO_AUTO;

	// shutter speed
	pShotModeCap->iMinShutSpdQ16 = -1;
	pShotModeCap->iMaxShutSpdQ16 = -1;

	// F-number
	pShotModeCap->iMinFNumQ16 = (CAM_Int32s)(2.8 * 65536 + 0.5);
	pShotModeCap->iMaxFNumQ16 = (CAM_Int32s)(2.8 * 65536 + 0.5);

	// band filter
	pShotModeCap->iSupportedBdFltModeCnt = 1; 
	pShotModeCap->eSupportedBdFltMode[0] = CAM_BANDFILTER_50HZ;

	// flash mode
	pShotModeCap->iSupportedFlashModeCnt = 0;

	// white balance mode
	pShotModeCap->iSupportedWBModeCnt = 1;
	pShotModeCap->eSupportedWBMode[0] = CAM_WHITEBALANCEMODE_AUTO;

	// focus mode
	pShotModeCap->iSupportedFocusModeCnt = 1;
	pShotModeCap->eSupportedFocusMode[0] = CAM_FOCUS_INFINITY;

	// optical zoom mode
	pShotModeCap->iMinOptZoomQ16 = (CAM_Int32s)(1.0 * 65536 + 0.5);
	pShotModeCap->iMaxOptZoomQ16 = (CAM_Int32s)(1.0 * 65536 + 0.5);

	// digital zoom mode
	pShotModeCap->iMinDigZoomQ16        = (CAM_Int32s)(1.0 * 65536 + 0.5);
	pShotModeCap->iMaxDigZoomQ16        = (CAM_Int32s)(1.0 * 65536 + 0.5);
	pShotModeCap->bSupportSmoothDigZoom = CAM_FALSE;

	// frame rate
	pShotModeCap->iMinFpsQ16     = 1 << 16;
	pShotModeCap->iMaxFpsQ16     = 40 << 16;

	// color effect
	pShotModeCap->iSupportedColorEffectCnt = 1;
	pShotModeCap->eSupportedColorEffect[0] = CAM_COLOREFFECT_OFF; 

	// brightness
	pShotModeCap->iMinBrightness = 0;
	pShotModeCap->iMaxBrightness = 0;

	// contrast
	pShotModeCap->iMinContrast = 0;
	pShotModeCap->iMaxContrast = 0;

	// saturation
	pShotModeCap->iMinSaturation = 0;
	pShotModeCap->iMaxSaturation = 0;

	// sharpness
	pShotModeCap->iMinSharpness = 0;
	pShotModeCap->iMaxSharpness = 0;

	// advanced features
	pShotModeCap->bSupportVideoStabilizer   = CAM_FALSE;
	pShotModeCap->bSupportVideoNoiseReducer	= CAM_FALSE;
	pShotModeCap->bSupportZeroShutterLag    = CAM_FALSE;
	pShotModeCap->bSupportBurstCapture      = CAM_FALSE;
	pShotModeCap->bSupportHighDynamicRange  = CAM_FALSE;
	pShotModeCap->iMaxBurstCnt              = 1;

	return;
}


static CAM_Error _BasicSensorSaveAeAwb( const _CAM_SmartSensorConfig *pOldConfig, void *pUserData )
{
	// TODO: add your sensor specific save 3A function here
	return CAM_ERROR_NONE;
}

static CAM_Error _BasicSensorRestoreAeAwb( const _CAM_SmartSensorConfig *pNewConfig, void *pUserData )
{
	// TODO: add your sensor specific restore 3A function here
	return CAM_ERROR_NONE;  
}

static CAM_Error _BasicSensorStartFlash( void *pV4l2State )
{
	// TODO: add your sensor specific start flash function here
	return CAM_ERROR_NONE;
}

static CAM_Error _BasicSensorStopFlash( void *pV4l2State )
{
	// TODO: add your sensor specific stop flash function here
	return CAM_ERROR_NONE;
}

// get shot info
static CAM_Error _BasicSensorFillFrameShotInfo( BasicSensorState *pState, CAM_ImageBuffer *pImgBuf )
{
	// TODO: add real value of these parameters here

	CAM_Error    error      = CAM_ERROR_NONE;
	CAM_ShotInfo *pShotInfo = &(pImgBuf->stShotInfo);

	pShotInfo->iExposureTimeQ16    = (1 << 16) / 30;
	pShotInfo->iFNumQ16            = (int)( 2.8 * 65536 + 0.5 );
	pShotInfo->eExposureMode       = CAM_EXPOSUREMODE_AUTO;
	pShotInfo->eExpMeterMode       = CAM_EXPOSUREMETERMODE_MEAN;
	pShotInfo->iISOSpeed           = 100;
	pShotInfo->iSubjectDistQ16     = 0;
	pShotInfo->iFlashStatus        = 0x0010;
	pShotInfo->iFocalLenQ16        = (int)( 3.79 * 65536 + 0.5 );
	pShotInfo->iFocalLen35mm       = 0;       

	return error;
}
