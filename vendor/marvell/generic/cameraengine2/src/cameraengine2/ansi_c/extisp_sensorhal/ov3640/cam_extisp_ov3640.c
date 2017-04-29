/*******************************************************************************
//(C) Copyright [2009 - 2011] Marvell International Ltd.
//All Rights Reserved
*******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "cam_log.h"
#include "cam_utility.h"


#include "cam_extisp_sensorwrapper.h"
#include "cam_extisp_v4l2base.h"
#include "cam_extisp_ov3640.h"

// NOTE: if you want to enable static resolution table to bypass sensor dynamically detect to save camera-off to viwerfinder-on latency, 
//       you can fill the following four tables according to your sensor's capability. And open macro 
//       BUILD_OPTION_STARTEGY_DISABLE_DYNAMIC_SENSOR_DETECT in cam_extisp_buildopt.h

/* OV3640 preview/video resolution table */
CAM_Size _OV3640VideoResTable[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT] = 
{
	{176,  144},    // 15fps for VT
	{640,  480},	// 15fps in current driver implementation (TTC/TD)
	{720,  480},	// 30fps in current driver implementation (TTC/TD)
};

/* OV3640 still resolution table */
CAM_Size _OV3640StillResTable[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT] =
{
	{640, 480},
	{2048, 1536},	// 3M
};

CAM_ImageFormat _OV3640VideoFormatTable[CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT] = 
{
	CAM_IMGFMT_YCbCr420P,
};

CAM_ImageFormat _OV3640StillFormatTable[CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT] = 
{
	CAM_IMGFMT_JPEG,
};


CAM_FlipRotate _OV3640RotationTable[] =
{
	CAM_FLIPROTATE_NORMAL,
};


CAM_ShotMode _OV3640ShotModeTable[] = 
{
	CAM_SHOTMODE_AUTO,
};

_CAM_RegEntry _OV3640ExpMeter_Mean[] =
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _OV3640ExpMeter[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_EXPOSUREMETERMODE_MEAN, _OV3640ExpMeter_Mean),
};

_CAM_RegEntry _OV3640IsoMode_Auto[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _OV3640IsoMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_ISO_AUTO, _OV3640IsoMode_Auto),
};

_CAM_RegEntry _OV3640BdFltMode_Auto[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _OV3640BdFltMode_Off[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _OV3640BdFltMode_50Hz[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _OV3640BdFltMode_60Hz[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _OV3640BdFltMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_BANDFILTER_AUTO, _OV3640BdFltMode_Auto),
	PARAM_ENTRY_USE_REGTABLE(CAM_BANDFILTER_OFF,  _OV3640BdFltMode_Off),
	PARAM_ENTRY_USE_REGTABLE(CAM_BANDFILTER_50HZ, _OV3640BdFltMode_50Hz),
	PARAM_ENTRY_USE_REGTABLE(CAM_BANDFILTER_60HZ, _OV3640BdFltMode_60Hz),
};

_CAM_RegEntry _OV3640FlashMode_Off[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _OV3640FlashMode_On[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _OV3640FlashMode_Auto[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _OV3640FlashMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_FLASH_OFF,	_OV3640FlashMode_Off),
};

_CAM_RegEntry _OV3640WBMode_Auto[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _OV3640WBMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_WHITEBALANCEMODE_AUTO, _OV3640WBMode_Auto),
};

_CAM_RegEntry _OV3640FocusMode_Infinity[] =
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _OV3640FocusMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_FOCUS_INFINITY, _OV3640FocusMode_Infinity),

};

_CAM_RegEntry _OV3640ColorEffectMode_Off[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _OV3640ColorEffectMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_COLOREFFECT_OFF, _OV3640ColorEffectMode_Off),
};

static CAM_Error _OV3640SaveAeAwb( const _CAM_SmartSensorConfig*, void* );
static CAM_Error _OV3640RestoreAeAwb( const _CAM_SmartSensorConfig*, void* );
static CAM_Error _OV3640StartFlash( void* );
static CAM_Error _OV3640StopFlash( void* );

static CAM_Error _OV3640FillFrameShotInfo( OV3640State*, CAM_ImageBuffer* );
static CAM_Error _OV3640SetJpegParam( OV3640State*, CAM_JpegParam* );

// shot mode capability
static void _OV3640AutoModeCap( CAM_ShotModeCapability* );
// static void _OV3640ManualModeCap( CAM_ShotModeCapability* );
// static void _OV3640NightModeCap( CAM_ShotModeCapability* );

/* shot mode cap function table */
OV3640_ShotModeCap _OV3640ShotModeCap[CAM_SHOTMODE_NUM] = { _OV3640AutoModeCap, // CAM_SHOTMODE_AUTO = 0,
                                                            NULL              , // CAM_SHOTMODE_MANUAL,
                                                            NULL              , // CAM_SHOTMODE_PORTRAIT,
                                                            NULL              , // CAM_SHOTMODE_LANDSCAPE,
                                                            NULL              , // CAM_SHOTMODE_NIGHTPORTRAIT,
                                                            NULL              , // CAM_SHOTMODE_NIGHTSCENE,
                                                            NULL              , // CAM_SHOTMODE_CHILD,
                                                            NULL              , // CAM_SHOTMODE_INDOOR,
                                                            NULL              , // CAM_SHOTMODE_PLANTS,
                                                            NULL              , // CAM_SHOTMODE_SNOW,
                                                            NULL              , // CAM_SHOTMODE_BEACH,
                                                            NULL              , // CAM_SHOTMODE_FIREWORKS,
                                                            NULL              , // CAM_SHOTMODE_SUBMARINE, 
                                                            NULL              , // CAM_SHOTMODE_WHITEBOARD,
                                                            NULL              , // CAM_SHOTMODE_SPORTS
                                                           };  


extern _SmartSensorFunc func_ov3640; 
CAM_Error OV3640_SelfDetect( _SmartSensorAttri *pSensorInfo )
{
	CAM_Error error = CAM_ERROR_NONE;

	// NOTE:  If you open macro BUILD_OPTION_STARTEGY_DISABLE_DYNAMIC_SENSOR_DETECT in cam_extisp_buildopt.h 
	//        to bypass sensor dynamically detect to save camera-off to viwerfinder-on latency, you should initilize
	//        _OV3640VideoResTable/_OV3640StillResTable/_OV3640VideoFormatTable/_OV3640StillFormatTable manually.

#if !defined( BUILD_OPTION_STARTEGY_DISABLE_DYNAMIC_SENSOR_DETECT )	
	error = V4L2_SelfDetect( pSensorInfo, "ov3640", &func_ov3640,
	                         _OV3640VideoResTable, _OV3640StillResTable,
	                         _OV3640VideoFormatTable, _OV3640StillFormatTable );
#else
	{
		_V4L2SensorEntry *pSensorEntry = (_V4L2SensorEntry*)( pSensorInfo->cReserved );
		strcpy( pSensorInfo->sSensorName, "ov3640" );
		pSensorInfo->pFunc = &func_ov3640;

		// FIXME: the following is just an example in Marvell platform, you should change it according to your driver implementation
		strcpy( pSensorEntry->sDeviceName, "/dev/video0" );
		pSensorEntry->iV4L2SensorID = 1;
	}
#endif

	return error;
} 

CAM_Error OV3640_GetCapability( _CAM_SmartSensorCapability *pCapability )
{
	CAM_Int32s i = 0;

	// preview/video supporting 
	// format
	pCapability->iSupportedVideoFormatCnt = 0;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT; i++ ) 
	{
		if ( _OV3640VideoFormatTable[i] == 0 )
		{
			break;
		}
		pCapability->eSupportedVideoFormat[pCapability->iSupportedVideoFormatCnt] = _OV3640VideoFormatTable[i];
		pCapability->iSupportedVideoFormatCnt++;
	}

	pCapability->bArbitraryVideoSize    = CAM_FALSE;
	pCapability->iSupportedVideoSizeCnt = 0;
	pCapability->stMinVideoSize.iWidth   = _OV3640VideoResTable[0].iWidth;
	pCapability->stMinVideoSize.iHeight  = _OV3640VideoResTable[0].iHeight;
	pCapability->stMaxVideoSize.iWidth   = _OV3640VideoResTable[0].iWidth;
	pCapability->stMaxVideoSize.iHeight  = _OV3640VideoResTable[0].iHeight;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_SIZE_CNT; i++ )
	{
		if ( _OV3640VideoResTable[i].iWidth == 0 || _OV3640VideoResTable[i].iHeight == 0)
		{
			break;
		}
		pCapability->stSupportedVideoSize[pCapability->iSupportedVideoSizeCnt] = _OV3640VideoResTable[i];
		pCapability->iSupportedVideoSizeCnt++;

		if ( ( pCapability->stMinVideoSize.iWidth > _OV3640VideoResTable[i].iWidth ) || 
			( ( pCapability->stMinVideoSize.iWidth == _OV3640VideoResTable[i].iWidth ) && ( pCapability->stMinVideoSize.iHeight > _OV3640VideoResTable[i].iHeight ) ) ) 
		{
			pCapability->stMinVideoSize.iWidth = _OV3640VideoResTable[i].iWidth;
			pCapability->stMinVideoSize.iHeight = _OV3640VideoResTable[i].iHeight;
		}
		if ( ( pCapability->stMaxVideoSize.iWidth < _OV3640VideoResTable[i].iWidth) ||
			( ( pCapability->stMaxVideoSize.iWidth == _OV3640VideoResTable[i].iWidth ) && ( pCapability->stMaxVideoSize.iHeight < _OV3640VideoResTable[i].iHeight ) ) )
		{
			pCapability->stMaxVideoSize.iWidth = _OV3640VideoResTable[i].iWidth;
			pCapability->stMaxVideoSize.iHeight = _OV3640VideoResTable[i].iHeight;
		}
	}

	// still capture supporting
	// format
	pCapability->iSupportedStillFormatCnt = 0;
	pCapability->stSupportedJPEGParams.bSupportJpeg = CAM_FALSE;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT; i++ ) 
	{
		if ( _OV3640StillFormatTable[i] == CAM_IMGFMT_JPEG )
		{
			// JPEG encoder functionalities
            pCapability->stSupportedJPEGParams.bSupportJpeg = CAM_TRUE;
			pCapability->stSupportedJPEGParams.bSupportExif = CAM_FALSE;
			pCapability->stSupportedJPEGParams.bSupportThumb = CAM_FALSE;
			pCapability->stSupportedJPEGParams.iMinQualityFactor = 80;
			pCapability->stSupportedJPEGParams.iMaxQualityFactor = 80;
		}
		if ( _OV3640StillFormatTable[i] == 0 )
		{
			break;
		}
		pCapability->eSupportedStillFormat[pCapability->iSupportedStillFormatCnt] = _OV3640StillFormatTable[i];
		pCapability->iSupportedStillFormatCnt++;
	}
	// resolution
	pCapability->bArbitraryStillSize = CAM_FALSE;
	pCapability->iSupportedStillSizeCnt = 0;
	pCapability->stMinStillSize.iWidth  = _OV3640StillResTable[0].iWidth;
	pCapability->stMinStillSize.iHeight = _OV3640StillResTable[0].iHeight;
	pCapability->stMaxStillSize.iWidth  = _OV3640StillResTable[0].iWidth;
	pCapability->stMaxStillSize.iHeight = _OV3640StillResTable[0].iHeight;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_SIZE_CNT; i++ )
	{
		if ( _OV3640StillResTable[i].iWidth == 0 || _OV3640StillResTable[i] .iHeight == 0 )
		{
			break;
		}

		pCapability->stSupportedStillSize[pCapability->iSupportedStillSizeCnt] = _OV3640StillResTable[i];
		pCapability->iSupportedStillSizeCnt++;

		if ( ( pCapability->stMinStillSize.iWidth > _OV3640StillResTable[i].iWidth ) ||
			( ( pCapability->stMinStillSize.iWidth == _OV3640StillResTable[i].iWidth ) && ( pCapability->stMinStillSize.iHeight > _OV3640StillResTable[i].iHeight ) ) )
		{
			pCapability->stMinStillSize.iWidth  = _OV3640StillResTable[i].iWidth;
			pCapability->stMinStillSize.iHeight = _OV3640StillResTable[i].iHeight;
		}
		if ( ( pCapability->stMaxStillSize.iWidth < _OV3640StillResTable[i].iWidth ) || 
			( ( pCapability->stMaxStillSize.iWidth == _OV3640StillResTable[i].iWidth ) && ( pCapability->stMaxStillSize.iHeight < _OV3640StillResTable[i].iHeight ) ) )
		{
			pCapability->stMaxStillSize.iWidth = _OV3640StillResTable[i].iWidth;
			pCapability->stMaxStillSize.iHeight = _OV3640StillResTable[i].iHeight;
		}
	}

	// rotate
	pCapability->iSupportedRotateCnt = _ARRAY_SIZE(_OV3640RotationTable);
	for ( i = 0; i < pCapability->iSupportedRotateCnt; i++ )
	{
		pCapability->eSupportedRotate[i] = _OV3640RotationTable[i];
	}

	pCapability->iSupportedShotModeCnt = _ARRAY_SIZE(_OV3640ShotModeTable);
	for ( i = 0; i < pCapability->iSupportedShotModeCnt; i++ )
	{
		pCapability->eSupportedShotMode[i] = _OV3640ShotModeTable[i];
	}

	// FIXME: all supported shot parameters
	_OV3640AutoModeCap( &pCapability->stSupportedShotParams );

	return CAM_ERROR_NONE;

}

CAM_Error OV3640_GetShotModeCapability( CAM_ShotMode eShotMode, CAM_ShotModeCapability *pShotModeCap )
{
	CAM_Int32u i;

	// BAC check
	for ( i = 0; i < _ARRAY_SIZE(_OV3640ShotModeTable); i++ )
	{
		if ( _OV3640ShotModeTable[i] == eShotMode )
		{
			break;
		}
	}

	if ( i >= _ARRAY_SIZE(_OV3640ShotModeTable) || pShotModeCap ==NULL ) 
	{
		return CAM_ERROR_BADARGUMENT;
	}

	(void)(_OV3640ShotModeCap[eShotMode])( pShotModeCap );

	return CAM_ERROR_NONE;
}

CAM_Error OV3640_Init( void **ppDevice, void *pSensorEntry )
{
	CAM_Error             error        = CAM_ERROR_NONE;
	CAM_Int32s            iSkipFrame   = 0;
	_V4L2SensorAttribute  _OV3640Attri;
	_V4L2SensorEntry      *pSensor     = (_V4L2SensorEntry*)pSensorEntry;
	OV3640State           *pState      = (OV3640State*)malloc( sizeof( OV3640State ) );

	memset( &_OV3640Attri, 0, sizeof( _V4L2SensorAttribute ) );

	*ppDevice = pState;
	if ( *ppDevice == NULL )
	{
		return CAM_ERROR_OUTOFMEMORY;
	}

	_OV3640Attri.stV4L2SensorEntry.iV4L2SensorID = pSensor->iV4L2SensorID;
	strcpy( _OV3640Attri.stV4L2SensorEntry.sDeviceName, pSensor->sDeviceName );

#if defined( BUILD_OPTION_DEBUG_DISABLE_SAVE_RESTORE_3A )
	iSkipFrame                        = 20;
	_OV3640Attri.fnSaveAeAwb          = NULL;
	_OV3640Attri.fnRestoreAeAwb       = NULL;
	_OV3640Attri.pSaveRestoreUserData = NULL;
	_OV3640Attri.fnStartFlash         = NULL;
	_OV3640Attri.fnStopFlash          = NULL;
#else
	iSkipFrame                        = 2;
	_OV3640Attri.fnSaveAeAwb          = _OV3640SaveAeAwb;
	_OV3640Attri.fnRestoreAeAwb       = _OV3640RestoreAeAwb;
	_OV3640Attri.pSaveRestoreUserData = (void*)pState;
	_OV3640Attri.fnStartFlash         = _OV3640StartFlash;
	_OV3640Attri.fnStopFlash          = _OV3640StopFlash;
#endif

	error = V4L2_Init( &(pState->stV4L2), &(_OV3640Attri), iSkipFrame );

	/* here we can get default shot params */
	pState->stV4L2.stShotParam.eShotMode        = CAM_SHOTMODE_AUTO;
	pState->stV4L2.stShotParam.eExpMode         = CAM_EXPOSUREMODE_AUTO;
	pState->stV4L2.stShotParam.eExpMeterMode    = CAM_EXPOSUREMETERMODE_AUTO;
	pState->stV4L2.stShotParam.iEvCompQ16       = 0;
	pState->stV4L2.stShotParam.eIsoMode         = CAM_ISO_AUTO;
	pState->stV4L2.stShotParam.iShutterSpeedQ16 = -1;
	pState->stV4L2.stShotParam.iFNumQ16         = 1;
	pState->stV4L2.stShotParam.eBandFilterMode  = CAM_BANDFILTER_50HZ;
	pState->stV4L2.stShotParam.eWBMode          = CAM_WHITEBALANCEMODE_AUTO;
	pState->stV4L2.stShotParam.eFocusMode       = CAM_FOCUS_INFINITY;
	pState->stV4L2.stShotParam.iDigZoomQ16      = 0;
	pState->stV4L2.stShotParam.eColorEffect     = CAM_COLOREFFECT_OFF;
	pState->stV4L2.stShotParam.iSaturation      = 64;
	pState->stV4L2.stShotParam.iBrightness      = 0;
	pState->stV4L2.stShotParam.iContrast        = 0;
	pState->stV4L2.stShotParam.iSharpness       = 0;
	pState->stV4L2.stShotParam.eFlashMode         = CAM_FLASH_OFF;
	pState->stV4L2.stShotParam.bVideoStabilizer   = CAM_FALSE;
	pState->stV4L2.stShotParam.bVideoNoiseReducer = CAM_FALSE;
	pState->stV4L2.stShotParam.bZeroShutterLag    = CAM_FALSE;
	pState->stV4L2.stShotParam.bHighDynamicRange  = CAM_FALSE;
	pState->stV4L2.stShotParam.iBurstCnt          = 1;
	/* get default JPEG params */
	pState->stV4L2.stJpegParam.iSampling      = 1; // 0 - 420, 1 - 422, 2 - 444
	pState->stV4L2.stJpegParam.iQualityFactor = 67;
	pState->stV4L2.stJpegParam.bEnableExif    = CAM_FALSE;
	pState->stV4L2.stJpegParam.bEnableThumb   = CAM_FALSE;
	pState->stV4L2.stJpegParam.iThumbWidth    = 0;
	pState->stV4L2.stJpegParam.iThumbHeight   = 0;


	return error;
}

CAM_Error OV3640_Deinit( void *pDevice )
{
	CAM_Error   error   = CAM_ERROR_NONE;
	OV3640State *pState = (OV3640State*)pDevice;

	error = V4L2_Deinit( &pState->stV4L2 );

	free( pDevice );

	return error;
}

CAM_Error OV3640_RegisterEventHandler( void *pDevice, CAM_EventHandler fnEventHandler, void *pUserData )
{
	OV3640State *pState = (OV3640State*)pDevice;
	CAM_Error   error   = CAM_ERROR_NONE;

	error = V4L2_RegisterEventHandler( &pState->stV4L2, fnEventHandler, pUserData );
	return error;
}

CAM_Error OV3640_Config( void *pDevice, _CAM_SmartSensorConfig *pSensorConfig )
{
	OV3640State *pState = (OV3640State*)pDevice;
	CAM_Error   error   = CAM_ERROR_NONE;

	error = V4L2_Config( &pState->stV4L2, pSensorConfig );
	if ( error != CAM_ERROR_NONE )
	{
		return error;
	}

	if ( pSensorConfig->eState != CAM_CAPTURESTATE_IDLE )
	{
		if ( pSensorConfig->eState == CAM_IMGFMT_JPEG )
		{
			error = _OV3640SetJpegParam( pDevice, &(pSensorConfig->stJpegParam) );
			if ( error != CAM_ERROR_NONE )
			{
				return error;
			}
		}
	}

	pState->stV4L2.stConfig = *pSensorConfig;
	return CAM_ERROR_NONE;
}

CAM_Error OV3640_GetBufReq( void *pDevice, _CAM_SmartSensorConfig *pSensorConfig, CAM_ImageBufferReq *pBufReq ) 
{
	OV3640State *pState = (OV3640State*)pDevice;
	CAM_Error   error   = CAM_ERROR_NONE;

	error = V4L2_GetBufReq( &pState->stV4L2, pSensorConfig, pBufReq );

	return error;
}

CAM_Error OV3640_Enqueue( void *pDevice, CAM_ImageBuffer *pImgBuf )
{
	OV3640State *pState = (OV3640State*)pDevice;
	CAM_Error   error   = CAM_ERROR_NONE;

	error = V4L2_Enqueue( &pState->stV4L2, pImgBuf );

	return error;
}

CAM_Error OV3640_Dequeue( void *pDevice, CAM_ImageBuffer **ppImgBuf )
{
	OV3640State *pState = (OV3640State*)pDevice;
	CAM_Error   error   = CAM_ERROR_NONE;

	error = V4L2_Dequeue( &pState->stV4L2, ppImgBuf );

	if ( error == CAM_ERROR_NONE && (*ppImgBuf)->bEnableShotInfo )
	{
		error = _OV3640FillFrameShotInfo( pState, *ppImgBuf );
	}

	return error;
}

CAM_Error OV3640_Flush( void *pDevice )
{
	OV3640State *pState = (OV3640State*)pDevice;
	CAM_Error   error   = CAM_ERROR_NONE;

	error = V4L2_Flush( &pState->stV4L2 );

	return error;
}

CAM_Error OV3640_SetShotParam( void *pDevice, _CAM_ShotParam *pShotParam )
{
	// TODO: here you can add your code to set shot params you supported, just like examples in ov5642.c or you can give your own style

	return CAM_ERROR_NONE;
}

CAM_Error OV3640_GetShotParam( void *pDevice, _CAM_ShotParam *pShotParam )
{
	OV3640State *pState = (OV3640State*)pDevice;

	*pShotParam = pState->stV4L2.stShotParam;
	return CAM_ERROR_NONE;
}

CAM_Error OV3640_StartFocus( void *pDevice, void *pFocusROI )
{
	// TODO: add your start focus code here,an refrence is ov5642.c

	return CAM_ERROR_NONE;
}

CAM_Error OV3640_CancelFocus( void *pDevice )
{
	// TODO: add your cancel focus code here,an refrence is ov5642.c
	return CAM_ERROR_NONE;
}

CAM_Error OV3640_CheckFocusState( void *pDevice, CAM_Bool *pFocusAutoStopped, _CAM_FocusState *pFocusState )
{

	// TODO: add your check focus status code here,an refrence is ov5642.c
	*pFocusAutoStopped = CAM_TRUE;
	*pFocusState       = CAM_IN_FOCUS;

	return CAM_ERROR_NONE;
}



CAM_Error OV3640_GetDigitalZoomCapability( CAM_Int32s iWidth, CAM_Int32s iHeight, CAM_Int32s *pSensorDigZoomQ16 )
{
	// TODO: add your get zoom capability code here,an refrence is ov5642.c
	*pSensorDigZoomQ16 = ( 1 << 16 );

	return CAM_ERROR_NONE;
}
_SmartSensorFunc func_ov3640 = 
{
	OV3640_GetCapability,
	OV3640_GetShotModeCapability,

	OV3640_SelfDetect,
	OV3640_Init,
	OV3640_Deinit,
	OV3640_RegisterEventHandler,
	OV3640_Config,
	OV3640_GetBufReq,
	OV3640_Enqueue,
	OV3640_Dequeue,
	OV3640_Flush,
	OV3640_SetShotParam,
	OV3640_GetShotParam,

	OV3640_StartFocus,
	OV3640_CancelFocus,
	OV3640_CheckFocusState,
	OV3640_GetDigitalZoomCapability,
};

static CAM_Error _OV3640SetJpegParam( OV3640State *pState, CAM_JpegParam *pJpegParam )
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
* OV3640 shotmode capability
* TODO: if you enable new shot mode, pls add a correspoding modcap function here, and add it to OV3640_shotModeCap _OV3640ShotModeCap array
*------------------------------------------------------------------------------------------------------------------------------------*/
static void _OV3640AutoModeCap( CAM_ShotModeCapability *pShotModeCap )
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
	pShotModeCap->iMinDigZoomQ16 = (CAM_Int32s)(1.0 * 65536 + 0.5);
	pShotModeCap->iMaxDigZoomQ16 = (CAM_Int32s)(1.0 * 65536 + 0.5);
	pShotModeCap->bSupportSmoothDigZoom = CAM_FALSE;

	// fps
	pShotModeCap->iMinFpsQ16 = (CAM_Int32s)(1.0 * 65536 + 0.5);
	pShotModeCap->iMaxFpsQ16 = (CAM_Int32s)(40.0 * 65536 + 0.5);

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
	pShotModeCap->bSupportVideoStabilizer	= CAM_FALSE;
	pShotModeCap->bSupportVideoNoiseReducer	= CAM_FALSE;
	pShotModeCap->bSupportZeroShutterLag	= CAM_FALSE;
	pShotModeCap->bSupportBurstCapture      = CAM_FALSE;
	pShotModeCap->bSupportHighDynamicRange	= CAM_FALSE;
	pShotModeCap->iMaxBurstCnt              = 1;
	return;
}



static CAM_Error _OV3640SaveAeAwb( const _CAM_SmartSensorConfig *pOldConfig, void *pUserData )
{
	OV3640State *pState   = (OV3640State*)pUserData;
	int         iSensorFD = pState->stV4L2.iSensorFD;

	CAM_Int16u reg=0, reg1=0, shutter=0, extra=0, gain16=0;
	CAM_Int16u dummy_line=0, dummy_pix=0; 
	CAM_Int16u preview_exposure=0;
	CAM_Int32u still_exposure16=0;
	CAM_Int16u preview_width=0, still_width=0;
	CAM_Int16u still_bandfilter_line=0, still_shutter_max=0;
	CAM_Int16u exposure_lines=0;
	CAM_Int16u wbGain=0;

	TRACE( CAM_INFO, "Info: %s in\n", __FUNCTION__ );
	// remove green frame
	reg = 0x02;
	_set_sensor_reg( iSensorFD, 0x3f00, 0, 7, 0, reg );
	reg = 0x13;
	_set_sensor_reg( iSensorFD, 0x3300, 0, 7, 0, reg );
	reg = 0x04;
	_set_sensor_reg( iSensorFD, 0x3014, 0, 7, 0, reg );

	// stop AEC/AGC
	_get_sensor_reg( iSensorFD, 0x3013, 0, 7, 0, &reg );
	reg &= 0xf8;
	_set_sensor_reg( iSensorFD, 0x3013, 0, 7, 0, reg );

	// preview shutter line
	_get_sensor_reg( iSensorFD, 0x3002, 0, 7, 0, &reg );
	_get_sensor_reg( iSensorFD, 0x3003, 0, 7, 0, &shutter );
	TRACE( CAM_INFO, "Info: preview shutter line: [0x3002]=0x%x, [0x3003]=0x%x\n", reg, shutter );
	shutter += reg << 8;

	// preview extra line
	_get_sensor_reg( iSensorFD, 0x302d, 0, 7, 0, &reg );
	_get_sensor_reg( iSensorFD, 0x302e, 0, 7, 0, &extra );
	TRACE( CAM_INFO, "Info: preview extra line: [0x302d]=0x%x, [0x302e]=0x%x\n", reg, extra );
	extra += reg << 8;

	preview_exposure = shutter + extra;

	// Preview gain
	_get_sensor_reg( iSensorFD, 0x3001, 0, 7, 0, &reg );
	gain16 = ( ((reg&0xf0) >> 4 ) + 1) * ( 16 + (reg & 0x0f) );
	TRACE( CAM_INFO, "Info: preview gain: %d ([0x3001]=0x%x)\n", gain16, reg );    

	// preview dummy line
	_get_sensor_reg( iSensorFD, 0x302a, 0, 7, 0, &reg );
	_get_sensor_reg( iSensorFD, 0x302b, 0, 7, 0, &reg1 );
	dummy_line = ( ( reg - DEFAULT_XGA_REG_0x302a ) << 8 ) + ( reg1 - DEFAULT_XGA_REG_0x302b );
	TRACE( CAM_INFO, "Info: preview dummy line: %d ([0x302a]=0x%x, [0x302b]=0x%x)\n", dummy_line, reg, reg1 );

	// preview dummy pixel
	_get_sensor_reg( iSensorFD, 0x3028, 0, 7, 0, &reg );
	_get_sensor_reg( iSensorFD, 0x3029, 0, 7, 0, &reg1 );
	dummy_pix = ( ( reg - DEFAULT_REG_0X3028 ) << 8 ) + ( reg1 - DEFAULT_REG_0X3029 );
	TRACE( CAM_INFO, "Info: preview dummy pixel: %d ([0x3028]=0x%x, [0x3029]=0x%x)\n", dummy_pix, reg, reg1 );

	// any better setting?
	pState->iStillDummyLine = 0;
	pState->iStillDummyPix  = 0;

	// sensor fps and width of preview and still
	preview_width = DEFAULT_XGA_LINE_WIDTH + dummy_pix;
	still_width = DEFAULT_QXGA_LINE_WIDTH + pState->iStillDummyPix;
	TRACE( CAM_INFO, "Info: preview: PCLK %dMHz  width %d\nstill: PCLK %dMHz  width %d\n",
	       PREVIEW_PIXEL_CLK, preview_width,
	       CAPTURE_PIXEL_CLK, still_width );

	// assume 50Hz AC banding filter
	still_bandfilter_line = CAPTURE_PIXEL_CLK * 1000000 / 100 / (DEFAULT_QXGA_LINE_WIDTH*2);

	still_shutter_max = (OV3640_SHUTTER_MAX_QXGA + pState->iStillDummyLine) / still_bandfilter_line;
	still_shutter_max *= still_bandfilter_line;

	// calculate still capture exposure
	still_exposure16 = gain16 * 3 / 2 * preview_exposure *
	                   CAPTURE_PIXEL_CLK / PREVIEW_PIXEL_CLK * preview_width / still_width;
	TRACE( CAM_INFO, "Info: preview_exposure: %d;  band filter: %d;  shutter max: %d\ntarget still exposure x 16: %d\n", 
	       preview_exposure, still_bandfilter_line, still_shutter_max, still_exposure16 );

	// distribute shuttet/extra/gain for still
	if ( still_exposure16 < (CAM_Int32u)still_bandfilter_line * 16 )
	{
		// exposure time < 1/100s
		if ( still_exposure16 < 16 )
		{
			still_exposure16 *= 4;
			exposure_lines = 1;
			pState->iStillGain16 = _DIV_ROUND(still_exposure16, exposure_lines) / 4;
		}
		else
		{
			exposure_lines = still_exposure16 / 16;
			pState->iStillGain16 = _DIV_ROUND(still_exposure16, exposure_lines);
		}
	}
	else if ( still_exposure16 > (CAM_Int32u)still_shutter_max * 16 )
	{
		// exposure time > maximum shutter line
		// make sure shutter = n/100s
		exposure_lines = still_shutter_max;
		pState->iStillGain16 = _DIV_ROUND( still_exposure16, exposure_lines );
		if ( pState->iStillGain16 >= CAPTURE_GAIN16_MAX )
		{
			CAM_Int16s temp_exp16;
			// extra lines are needed
			temp_exp16 = still_exposure16*11/10/CAPTURE_GAIN16_MAX;
			TRACE( CAM_INFO, "Info: temp_exp16 = %d\n",temp_exp16 );
			// make sure shutter = n/100s
			exposure_lines = temp_exp16 / still_bandfilter_line * still_bandfilter_line;
			TRACE( CAM_INFO, "Info: exposure_lines=%d\n",exposure_lines );
			pState->iStillGain16 = _DIV_ROUND(still_exposure16, exposure_lines);
		}
	}    
	else
	{
		// 1/100s < exposure time < maximum shutter line
		// make sure shutter = n/100s
		exposure_lines = (still_exposure16 / 16 / still_bandfilter_line) * still_bandfilter_line;
		pState->iStillGain16 = _DIV_ROUND(still_exposure16, exposure_lines);
	}

	pState->iStillExtra = (exposure_lines > still_shutter_max) ? (exposure_lines - still_shutter_max) : 0;
	pState->iStillShutter = exposure_lines - pState->iStillExtra;

	//save  AWB value
	reg = 0x00;
	_set_sensor_reg(iSensorFD, 0x33a0, 0, 7, 0, reg);
	_get_sensor_reg(iSensorFD, 0x33ca, 0, 7, 0, &wbGain);
	pState->iWBRgain = (wbGain & 0xff);

	reg = 0x01;
	_set_sensor_reg(iSensorFD, 0x33a0, 0, 7, 0, reg);
	_get_sensor_reg(iSensorFD, 0x33ca, 0, 7, 0, &wbGain);
	pState->iWBGgain = (wbGain & 0xff);

	reg = 0x02;
	_set_sensor_reg(iSensorFD, 0x33a0, 0, 7, 0, reg);
	_get_sensor_reg(iSensorFD, 0x33ca, 0, 7, 0, &wbGain);
	pState->iWBBgain = (wbGain & 0xff);

	TRACE( CAM_INFO, "Info: actual still exposure x 16: %d\n", (pState->iStillShutter + pState->iStillExtra) * pState->iStillGain16 );
	TRACE( CAM_INFO, "Info: actual still settings:\nshutter: %d;  extra: %d;  gain16: %d;  dummyLn: %d;  dummyPix: %d; rGain: %d; gGain: %d; bGain: %d;\n",
	       pState->iStillShutter, pState->iStillExtra, pState->iStillGain16, pState->iStillDummyLine, 
	       pState->iStillDummyPix, pState->iWBRgain, pState->iWBGgain, pState->iWBBgain );

	TRACE( CAM_INFO, "Info: %s out\n", __FUNCTION__ );

	return CAM_ERROR_NONE;
}

static CAM_Error _OV3640RestoreAeAwb( const _CAM_SmartSensorConfig *pNewConfig, void *pUserData )
{


	OV3640State *pState   = (OV3640State*)pUserData;
	int         iSensorFD = pState->stV4L2.iSensorFD;

	CAM_Int16u iStillShutter    = pState->iStillShutter;
	CAM_Int16u iStillExtra      = pState->iStillExtra;
	CAM_Int16u iStillGain16     = pState->iStillGain16;
	CAM_Int16u iStillDummyLine  = pState->iStillDummyLine;
	CAM_Int16u iStillDummyPix   = pState->iStillDummyPix;

	CAM_Int16u reg = 0, gain = 0;    

	TRACE( CAM_INFO, "Info: %s in\n", __FUNCTION__ );
	// make sure AE/AG has been stopped
	_get_sensor_reg(iSensorFD, 0x3013, 0, 7, 0, &reg);
	reg &= 0xfa;
	_set_sensor_reg(iSensorFD, 0x3013, 0, 7, 0, reg);

	// still shutter line
	_set_sensor_reg(iSensorFD, 0x3002, 0, 7, 0, iStillShutter>>8);
	_set_sensor_reg(iSensorFD, 0x3003, 0, 7, 0, iStillShutter&0x00ff);

	// still extra line
	_set_sensor_reg(iSensorFD, 0x302d, 0, 7, 0, iStillExtra>>8);
	_set_sensor_reg(iSensorFD, 0x302e, 0, 7, 0, iStillExtra&0x00ff);

	// still dummy line
	reg = (iStillDummyLine>>8) + DEFAULT_QXGA_REG_0x302a;
	_set_sensor_reg(iSensorFD, 0x302a, 0, 7, 0, reg);
	reg = (iStillDummyLine&0x00ff) + DEFAULT_QXGA_REG_0x302b;
	_set_sensor_reg(iSensorFD, 0x302b, 0, 7, 0, reg);

	// still dummy pixel
	reg = (iStillDummyPix>>8) + DEFAULT_REG_0X3028;
	_set_sensor_reg(iSensorFD, 0x3028, 0, 7, 0, reg);
	reg = (iStillDummyPix&0x00ff) + DEFAULT_REG_0X3029;
	_set_sensor_reg(iSensorFD, 0x3029, 0, 7, 0, reg);

	// still gain
	if ( iStillGain16 > 31 )
	{
		iStillGain16 >>= 1;
		gain = 0x10;
	}
	if ( iStillGain16 > 31 )
	{
		iStillGain16 >>= 1;
		gain |= 0x20;
	}
	if ( iStillGain16 > 31 )
	{
		iStillGain16 >>= 1;
		gain |= 0x40;
	}
	if ( iStillGain16 > 31 )
	{
		iStillGain16 >>= 1;
		gain |= 0x80;
	}
	if ( iStillGain16 > 16 )
	{
		gain |= (iStillGain16 - 16);
	}
	_set_sensor_reg(iSensorFD, 0x3001, 0, 7, 0, gain);

	//restore AWB
	reg = 0x08;
	_set_sensor_reg(iSensorFD, 0x332b, 3, 3, 3, reg);

	reg = pState->iWBRgain;
	_set_sensor_reg(iSensorFD, 0x33a7, 0, 7, 0, reg);

	reg = pState->iWBGgain;
	_set_sensor_reg(iSensorFD, 0x33a8, 0, 7, 0, reg);

	reg = pState->iWBBgain;
	_set_sensor_reg(iSensorFD, 0x33a9, 0, 7, 0, reg);

	{
		CAM_Int16u reg = 0, shutter = 0, extra = 0, gain16 = 0;
		CAM_Int16u dummy_line = 0, dummy_pix = 0; 

		// still shutter line
		_get_sensor_reg(iSensorFD, 0x3002, 0, 7, 0, &reg);
		_get_sensor_reg(iSensorFD, 0x3003, 0, 7, 0, &shutter);
		TRACE( CAM_INFO, "Info: still shutter line: [0x3002]=0x%x, [0x3003]=0x%x\n", reg, shutter );

		// still extra line
		_get_sensor_reg(iSensorFD, 0x302d, 0, 7, 0, &reg);
		_get_sensor_reg(iSensorFD, 0x302e, 0, 7, 0, &extra);
		TRACE( CAM_INFO, "Info: still extra line: [0x302d]=0x%x, [0x302e]=0x%x\n", reg, extra );

		// still gain
		_get_sensor_reg(iSensorFD, 0x3001, 0, 7, 0, &reg);
		gain16 = (((reg&0xf0)>>4) + 1) * (16 + (reg&0x0f));
		TRACE( CAM_INFO, "still gain: %d ([0x3001]=0x%x)\n", gain16, reg );    

		// still dummy line
		_get_sensor_reg(iSensorFD, 0x302a, 0, 7, 0, &reg);
		_get_sensor_reg(iSensorFD, 0x302b, 0, 7, 0, &dummy_line);
		TRACE( CAM_INFO, "Info: still dummy line: [0x302a]=0x%x, [0x302b]=0x%x\n", reg, dummy_line );

		// still dummy pixel
		_get_sensor_reg(iSensorFD, 0x3028, 0, 7, 0, &reg);
		_get_sensor_reg(iSensorFD, 0x3029, 0, 7, 0, &dummy_pix);
		TRACE( CAM_INFO, "Info: still dummy pixel: [0x3028]=0x%x, [0x3029]=0x%x\n", reg, dummy_pix );
	}

	TRACE( CAM_INFO, "Info: %s out\n", __FUNCTION__ );
	return CAM_ERROR_NONE;  
}

static CAM_Error _OV3640StartFlash( void *pV4l2State )
{
	//TODO: add your sensor specific start flash function here
	return CAM_ERROR_NONE;
}

static CAM_Error _OV3640StopFlash( void *pV4l2State )
{
	//TODO: add your sensor specific stop flash function here
	return CAM_ERROR_NONE;
}

// get shot info
static CAM_Error _OV3640FillFrameShotInfo( OV3640State *pState, CAM_ImageBuffer *pImgBuf )
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
