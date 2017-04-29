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
#include "cam_extisp_gt2005.h"


// NOTE: if you want to enable static resolution table to bypass sensor dynamically detect to save camera-off to viwerfinder-on latency, 
//       you can fill the following four tables according to your sensor's capability. And open macro 
//       BUILD_OPTION_STARTEGY_DISABLE_DYNAMIC_SENSOR_DETECT in cam_extisp_buildopt.h

/* GT2005 video/preview resolution table */
CAM_Size _GT2005VideoResTable[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT] = 
{
	{0, 0},
};

/* GT2005 still resolution table */
CAM_Size _GT2005StillResTable[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT] = 
{ 
	{0, 0},
};

/* GT2005 video/preview format table */
CAM_ImageFormat _GT2005VideoFormatTable[CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT] = 
{ 
	0, 
};

/* GT2005 still format table */
CAM_ImageFormat _GT2005StillFormatTable[CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT] = 
{
	0,
};


CAM_FlipRotate _GT2005RotationTable[] = 
{
	CAM_FLIPROTATE_NORMAL,
};


CAM_ShotMode _GT2005ShotModeTable[] = 
{
	CAM_SHOTMODE_AUTO,
};

_CAM_RegEntry _GT2005ExpMeter_Mean[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _GT2005ExpMeter[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_EXPOSUREMETERMODE_MEAN, _GT2005ExpMeter_Mean),
};

_CAM_RegEntry _GT2005IsoMode_Auto[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _GT2005IsoMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_ISO_AUTO, _GT2005IsoMode_Auto),
};

_CAM_RegEntry _GT2005BdFltMode_Auto[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _GT2005BdFltMode_Off[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _GT2005BdFltMode_50Hz[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _GT2005BdFltMode_60Hz[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _GT2005BdFltMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_BANDFILTER_50HZ, _GT2005BdFltMode_50Hz),
};

_CAM_RegEntry _GT2005FlashMode_Off[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _GT2005FlashMode_On[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _GT2005FlashMode_Auto[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _GT2005FlashMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_FLASH_OFF,	_GT2005FlashMode_Off),
};

_CAM_RegEntry _GT2005WBMode_Auto[] =
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _GT2005WBMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_WHITEBALANCEMODE_AUTO, _GT2005WBMode_Auto),
};

_CAM_RegEntry _GT2005FocusMode_Infinity[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _GT2005FocusMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_FOCUS_INFINITY,  _GT2005FocusMode_Infinity),
};

_CAM_RegEntry _GT2005ColorEffectMode_Off[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _GT2005ColorEffectMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_COLOREFFECT_OFF, _GT2005ColorEffectMode_Off),
};


static CAM_Error _GT2005SaveAeAwb( const _CAM_SmartSensorConfig*, void* );
static CAM_Error _GT2005RestoreAeAwb( const _CAM_SmartSensorConfig*, void* );
static CAM_Error _GT2005StartFlash( void* );
static CAM_Error _GT2005StopFlash( void* );
static CAM_Error _GT2005FillFrameShotInfo( GT2005State*, CAM_ImageBuffer* );
static CAM_Error _GT2005SetJpegParam( GT2005State*, CAM_JpegParam* );

// shot mode capability
static void _GT2005AutoModeCap( CAM_ShotModeCapability* );
// static void _GT2005ManualModeCap( CAM_ShotModeCapability* );
// static void _GT2005NightModeCap( CAM_ShotModeCapability* );
/* shot mode cap function table */
GT2005_ShotModeCap _GT2005ShotModeCap[CAM_SHOTMODE_NUM] = { _GT2005AutoModeCap, // CAM_SHOTMODE_AUTO = 0,
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

extern _SmartSensorFunc func_gt2005; 
CAM_Error GT2005_SelfDetect( _SmartSensorAttri *pSensorInfo )
{
	CAM_Error error = CAM_ERROR_NONE;
	
	// NOTE:  If you open macro BUILD_OPTION_STARTEGY_DISABLE_DYNAMIC_SENSOR_DETECT in cam_extisp_buildopt.h 
	//        to bypass sensor dynamically detect to save camera-off to viwerfinder-on latency, you should initilize
	//        _GT2005VideoResTable/_GT2005StillResTable/_GT2005VideoFormatTable/_GT2005StillFormatTable manually.

#if !defined( BUILD_OPTION_STARTEGY_DISABLE_DYNAMIC_SENSOR_DETECT )	
	error = V4L2_SelfDetect( pSensorInfo, "gt2005", &func_gt2005,
	                         _GT2005VideoResTable, _GT2005StillResTable,
	                         _GT2005VideoFormatTable, _GT2005StillFormatTable );

#else
	{
		_V4L2SensorEntry *pSensorEntry = (_V4L2SensorEntry*)( pSensorInfo->cReserved );
		strcpy( pSensorInfo->sSensorName, "gt2005" );
		pSensorInfo->pFunc = &func_gt2005;

		// FIXME: the following is just an example in Marvell platform, you should change it according to your driver implementation
		strcpy( pSensorEntry->sDeviceName, "/dev/video0" );
		pSensorEntry->iV4L2SensorID = 0;
	}
#endif

	return error;
} 


CAM_Error GT2005_GetCapability( _CAM_SmartSensorCapability *pCapability )
{
	CAM_Int32s i = 0;

	// preview/video supporting 
	// format
	pCapability->iSupportedVideoFormatCnt = 0;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT; i++ ) 
	{
		if ( _GT2005VideoFormatTable[i] == 0 )
		{
			break;
		}
		pCapability->eSupportedVideoFormat[pCapability->iSupportedVideoFormatCnt] = _GT2005VideoFormatTable[i];
		pCapability->iSupportedVideoFormatCnt++;
	}

	pCapability->bArbitraryVideoSize     = CAM_FALSE;
	pCapability->iSupportedVideoSizeCnt  = 0;
	pCapability->stMinVideoSize.iWidth   = _GT2005VideoResTable[0].iWidth;
	pCapability->stMinVideoSize.iHeight  = _GT2005VideoResTable[0].iHeight;
	pCapability->stMaxVideoSize.iWidth   = _GT2005VideoResTable[0].iWidth;
	pCapability->stMaxVideoSize.iHeight  = _GT2005VideoResTable[0].iHeight;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_SIZE_CNT; i++ )
	{
		if ( _GT2005VideoResTable[i].iWidth == 0 || _GT2005VideoResTable[i].iHeight == 0)
		{
			break;
		}
		pCapability->stSupportedVideoSize[pCapability->iSupportedVideoSizeCnt] = _GT2005VideoResTable[i];
		pCapability->iSupportedVideoSizeCnt++;

		if ( ( pCapability->stMinVideoSize.iWidth > _GT2005VideoResTable[i].iWidth ) || 
		     ( ( pCapability->stMinVideoSize.iWidth == _GT2005VideoResTable[i].iWidth ) && ( pCapability->stMinVideoSize.iHeight > _GT2005VideoResTable[i].iHeight ) ) ) 
		{
			pCapability->stMinVideoSize.iWidth = _GT2005VideoResTable[i].iWidth;
			pCapability->stMinVideoSize.iHeight = _GT2005VideoResTable[i].iHeight;
		}
		if ( ( pCapability->stMaxVideoSize.iWidth < _GT2005VideoResTable[i].iWidth) ||
		     ( ( pCapability->stMaxVideoSize.iWidth == _GT2005VideoResTable[i].iWidth ) && ( pCapability->stMaxVideoSize.iHeight < _GT2005VideoResTable[i].iHeight ) ) )
		{
			pCapability->stMaxVideoSize.iWidth = _GT2005VideoResTable[i].iWidth;
			pCapability->stMaxVideoSize.iHeight = _GT2005VideoResTable[i].iHeight;
		}
	}

	// still capture supporting
	// format
	pCapability->iSupportedStillFormatCnt           = 0;
	pCapability->stSupportedJPEGParams.bSupportJpeg = CAM_FALSE;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT; i++ ) 
	{
		if ( _GT2005StillFormatTable[i] == CAM_IMGFMT_JPEG )
		{
			// JPEG encoder functionalities
			pCapability->stSupportedJPEGParams.bSupportJpeg = CAM_TRUE;
			pCapability->stSupportedJPEGParams.bSupportExif = CAM_FALSE;
			pCapability->stSupportedJPEGParams.bSupportThumb = CAM_FALSE;
			pCapability->stSupportedJPEGParams.iMinQualityFactor = 80;
			pCapability->stSupportedJPEGParams.iMaxQualityFactor = 80;
		}
		if ( _GT2005StillFormatTable[i] == 0 )
		{
			break;
		}
		pCapability->eSupportedStillFormat[pCapability->iSupportedStillFormatCnt] = _GT2005StillFormatTable[i];
		pCapability->iSupportedStillFormatCnt++;
	}
	// resolution
	pCapability->bArbitraryStillSize    = CAM_FALSE;
	pCapability->iSupportedStillSizeCnt = 0;
	pCapability->stMinStillSize.iWidth  = _GT2005StillResTable[0].iWidth;
	pCapability->stMinStillSize.iHeight = _GT2005StillResTable[0].iHeight;
	pCapability->stMaxStillSize.iWidth  = _GT2005StillResTable[0].iWidth;
	pCapability->stMaxStillSize.iHeight = _GT2005StillResTable[0].iHeight;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_SIZE_CNT; i++ )
	{
		if ( _GT2005StillResTable[i].iWidth == 0 || _GT2005StillResTable[i].iHeight == 0 )
		{
			break;
		}

		pCapability->stSupportedStillSize[pCapability->iSupportedStillSizeCnt] = _GT2005StillResTable[i];
		pCapability->iSupportedStillSizeCnt++;

		if ( ( pCapability->stMinStillSize.iWidth > _GT2005StillResTable[i].iWidth ) ||
		     ( ( pCapability->stMinStillSize.iWidth == _GT2005StillResTable[i].iWidth ) && ( pCapability->stMinStillSize.iHeight > _GT2005StillResTable[i].iHeight ) ) )
		{
			pCapability->stMinStillSize.iWidth  = _GT2005StillResTable[i].iWidth;
			pCapability->stMinStillSize.iHeight = _GT2005StillResTable[i].iHeight;
		}
		if ( ( pCapability->stMaxStillSize.iWidth < _GT2005StillResTable[i].iWidth ) || 
		     ( ( pCapability->stMaxStillSize.iWidth == _GT2005StillResTable[i].iWidth ) && ( pCapability->stMaxStillSize.iHeight < _GT2005StillResTable[i].iHeight ) ) )
		{
			pCapability->stMaxStillSize.iWidth = _GT2005StillResTable[i].iWidth;
			pCapability->stMaxStillSize.iHeight = _GT2005StillResTable[i].iHeight;
		}
	}

	// rotate
	pCapability->iSupportedRotateCnt = _ARRAY_SIZE( _GT2005RotationTable );
	for ( i = 0; i < pCapability->iSupportedRotateCnt; i++ )
	{
		pCapability->eSupportedRotate[i] = _GT2005RotationTable[i];
	}

	pCapability->iSupportedShotModeCnt = _ARRAY_SIZE(_GT2005ShotModeTable);
	for ( i = 0; i < pCapability->iSupportedShotModeCnt; i++ )
	{
		pCapability->eSupportedShotMode[i] = _GT2005ShotModeTable[i];
	}

	return CAM_ERROR_NONE;
}

CAM_Error GT2005_GetShotModeCapability( CAM_ShotMode eShotMode, CAM_ShotModeCapability *pShotModeCap )
{
	CAM_Int32u i;
	// BAC check
	for ( i = 0; i < _ARRAY_SIZE( _GT2005ShotModeTable ); i++ )
	{
		if ( _GT2005ShotModeTable[i] == eShotMode )
		{
			break;
		}
	}

	if ( i >= _ARRAY_SIZE( _GT2005ShotModeTable ) || pShotModeCap == NULL ) 
	{
		return CAM_ERROR_BADARGUMENT;
	}

	(void)(_GT2005ShotModeCap[eShotMode])( pShotModeCap );

	return CAM_ERROR_NONE;
}

CAM_Error GT2005_Init( void **ppDevice, void *pSensorEntry )
{
	CAM_Error             error      = CAM_ERROR_NONE;
	CAM_Int32s            iSkipFrame = 0;
	_V4L2SensorAttribute  _GT2005Attri;
	_V4L2SensorEntry      *pSensor = (_V4L2SensorEntry*)(pSensorEntry);
	GT2005State           *pState  = (GT2005State*)malloc( sizeof(GT2005State) );

	memset( &_GT2005Attri, 0, sizeof(_V4L2SensorAttribute) );

	*ppDevice = pState;
	if ( *ppDevice == NULL )
	{
		return CAM_ERROR_OUTOFMEMORY;
	}

	_GT2005Attri.stV4L2SensorEntry.iV4L2SensorID = pSensor->iV4L2SensorID;
	strcpy( _GT2005Attri.stV4L2SensorEntry.sDeviceName, pSensor->sDeviceName );

	/**************************************************************************************
     * defaultly, we will skip 30 frames in FAST VALIDATION PASS to avoid potential black
	 * frame if 3A is not convergence while resolution switch. If this is not need to your 
	 * sensor, just modify iSkipFrame, if you sensor need do save/restore 3A, pls refer to
	 * your sensor vendor, and implement these functions/
	***************************************************************************************/
#if defined( BUILD_OPTION_DEBUG_DISABLE_SAVE_RESTORE_3A )
	iSkipFrame                        = 30;
	_GT2005Attri.fnSaveAeAwb          = NULL;
	_GT2005Attri.fnRestoreAeAwb       = NULL;
	_GT2005Attri.pSaveRestoreUserData = NULL;
	_GT2005Attri.fnStartFlash         = NULL;
 	_GT2005Attri.fnStopFlash          = NULL;
#else
	iSkipFrame                        = 2;
	_GT2005Attri.fnSaveAeAwb          = _GT2005SaveAeAwb;
	_GT2005Attri.fnRestoreAeAwb       = _GT2005RestoreAeAwb;
	_GT2005Attri.pSaveRestoreUserData = (void*)pState;
 	_GT2005Attri.fnStartFlash         =  _GT2005StartFlash;
 	_GT2005Attri.fnStopFlash          =  _GT2005StopFlash;
#endif

	error = V4L2_Init( &(pState->stV4L2), &(_GT2005Attri), iSkipFrame );

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

CAM_Error GT2005_Deinit( void *pDevice )
{
	CAM_Error   error = CAM_ERROR_NONE;
	GT2005State *pState = (GT2005State*)pDevice;

	error = V4L2_Deinit( &pState->stV4L2 );

	free( pDevice );

	return error;
}
CAM_Error GT2005_RegisterEventHandler( void *pDevice, CAM_EventHandler fnEventHandler, void *pUserData )
{
	GT2005State *pState = (GT2005State*)pDevice;
	CAM_Error   error   = CAM_ERROR_NONE;

	error = V4L2_RegisterEventHandler( &pState->stV4L2, fnEventHandler, pUserData );
	return error;
}

CAM_Error GT2005_Config( void *pDevice, _CAM_SmartSensorConfig *pSensorConfig )
{
	CAM_Error   error   = CAM_ERROR_NONE;
	GT2005State *pState = (GT2005State*)pDevice;

	error = V4L2_Config( &pState->stV4L2, pSensorConfig );
	if ( error != CAM_ERROR_NONE )
	{
		return error;
	}

	if ( pSensorConfig->eState != CAM_CAPTURESTATE_IDLE )
	{
		if ( pSensorConfig->eState == CAM_IMGFMT_JPEG )
		{
			error = _GT2005SetJpegParam( pDevice, &(pSensorConfig->stJpegParam) );
			if ( error != CAM_ERROR_NONE )
			{
				return error;
			}
		}
	}

	pState->stV4L2.stConfig = *pSensorConfig;
	return CAM_ERROR_NONE;
}

CAM_Error GT2005_GetBufReq( void *pDevice, _CAM_SmartSensorConfig *pSensorConfig, CAM_ImageBufferReq *pBufReq )
{
	CAM_Error   error   = CAM_ERROR_NONE;
	GT2005State *pState = (GT2005State*)pDevice;

	error = V4L2_GetBufReq( &pState->stV4L2, pSensorConfig, pBufReq );

	return error;
}

CAM_Error GT2005_Enqueue( void *pDevice, CAM_ImageBuffer *pImgBuf )
{
	CAM_Error   error   = CAM_ERROR_NONE;
	GT2005State *pState = (GT2005State*)pDevice;

	error = V4L2_Enqueue( &pState->stV4L2, pImgBuf );

	return error;
}

CAM_Error GT2005_Dequeue( void *pDevice, CAM_ImageBuffer **ppImgBuf )
{
	CAM_Error   error   = CAM_ERROR_NONE;
	GT2005State *pState = (GT2005State*)pDevice;

	error = V4L2_Dequeue( &pState->stV4L2, ppImgBuf );

	if ( error == CAM_ERROR_NONE && (*ppImgBuf)->bEnableShotInfo )
	{
		error = _GT2005FillFrameShotInfo( pState, *ppImgBuf );
	}

	return error;
}

CAM_Error GT2005_Flush( void *pDevice )
{
	CAM_Error   error   = CAM_ERROR_NONE;
	GT2005State *pState = (GT2005State*)pDevice;

	error = V4L2_Flush( &pState->stV4L2 );

	return error;
}

CAM_Error GT2005_SetShotParam( void *pDevice, _CAM_ShotParam *pShotParam )
{
	GT2005State *pState = (GT2005State*)pDevice;

	// TODO: here you can add your code to set shot params you supported, just like examples in ov5642.c or you can give your own style	
	pState->stV4L2.stShotParam = *pShotParam;

	return CAM_ERROR_NONE;
}

CAM_Error GT2005_GetShotParam( void *pDevice, _CAM_ShotParam *pShotParam )
{
	GT2005State *pState = (GT2005State*)pDevice;
    
	*pShotParam = pState->stV4L2.stShotParam;

	return CAM_ERROR_NONE;
}

CAM_Error GT2005_StartFocus( void *pDevice, void *pFocusROI )
{
	// TODO: add your start focus code here,an refrence is ov5642.c

	return CAM_ERROR_NONE;
}

CAM_Error GT2005_CancelFocus( void *pDevice )
{
	// TODO: add yourcancel focus code here,an refrence is ov5642.c
	return CAM_ERROR_NONE;
}

CAM_Error GT2005_CheckFocusState( void *pDevice, CAM_Bool *pFocusAutoStopped, _CAM_FocusState *pFocusState )
{

	// TODO: add your check focus status code here,an refrence is ov5642.c
	*pFocusAutoStopped = CAM_TRUE;
	*pFocusState       = CAM_IN_FOCUS;

	return CAM_ERROR_NONE;
}


CAM_Error GT2005_GetDigitalZoomCapability( CAM_Int32s iWidth, CAM_Int32s iHeight, CAM_Int32s *pSensorDigZoomQ16 )
{
	// TODO: add your get zoom capability code here,an refrence is ov5642.c
	*pSensorDigZoomQ16 = ( 1 << 16 );

	return CAM_ERROR_NONE;
}

_SmartSensorFunc func_gt2005 = 
{
	GT2005_GetCapability,
	GT2005_GetShotModeCapability,

	GT2005_SelfDetect,
	GT2005_Init,
	GT2005_Deinit,
	GT2005_RegisterEventHandler,
	GT2005_Config,
	GT2005_GetBufReq,
	GT2005_Enqueue,
	GT2005_Dequeue,
	GT2005_Flush,
	GT2005_SetShotParam,
	GT2005_GetShotParam,

	GT2005_StartFocus,
	GT2005_CancelFocus,
	GT2005_CheckFocusState,
	GT2005_GetDigitalZoomCapability,
};

static CAM_Error _GT2005SetJpegParam( GT2005State *pState, CAM_JpegParam *pJpegParam )
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
 * GT2005 shotmode capability
 * TODO: if you enable new shot mode, pls add a correspoding modcap function here, and add it to GT2005_shotModeCap _GT2005ShotModeCap array
 *------------------------------------------------------------------------------------------------------------------------------------*/
static void _GT2005AutoModeCap( CAM_ShotModeCapability *pShotModeCap )
{
	// exposure mode
	pShotModeCap->iSupportedExpModeCnt  = 1;
	pShotModeCap->eSupportedExpMode[0]  = CAM_EXPOSUREMODE_AUTO;
	
	// exposure metering mode 
	pShotModeCap->iSupportedExpMeterCnt = 1;
	pShotModeCap->eSupportedExpMeter[0] = CAM_EXPOSUREMETERMODE_AUTO;

	// EV compensation 
	pShotModeCap->iMinEVCompQ16  = -78644;
	pShotModeCap->iMaxEVCompQ16  = 78644;
	pShotModeCap->iEVCompStepQ16 = 19661;

	// ISO mode 
	pShotModeCap->iSupportedIsoModeCnt = 1;
	pShotModeCap->eSupportedIsoMode[0] = CAM_ISO_AUTO;

	// shutter speed
	pShotModeCap->iMinShutSpdQ16 = -1;
	pShotModeCap->iMaxShutSpdQ16 = -1;

	// F-number
	pShotModeCap->iMinFNumQ16 = (CAM_Int32s)(2.8 * 65536 + 0.5);
	pShotModeCap->iMaxFNumQ16 = (CAM_Int32s)(2.8 * 65536 + 0.5);

	// Band filter
	pShotModeCap->iSupportedBdFltModeCnt = 1; 
	pShotModeCap->eSupportedBdFltMode[0] = CAM_BANDFILTER_50HZ;

	// Flash mode
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


static CAM_Error _GT2005SaveAeAwb( const _CAM_SmartSensorConfig *pOldConfig, void *pUserData )
{
	// TODO: add your sensor specific save 3A function here
	return CAM_ERROR_NONE;
}

static CAM_Error _GT2005RestoreAeAwb( const _CAM_SmartSensorConfig *pNewConfig, void *pUserData )
{
	// TODO: add your sensor specific restore 3A function here
	return CAM_ERROR_NONE;  
}

static CAM_Error _GT2005StartFlash( void *pSensorState )
{
	// TODO: add your sensor specific start flash function here
	return CAM_ERROR_NONE;
}

static CAM_Error _GT2005StopFlash( void *pSensorState )
{
	// TODO: add your sensor specific stop flash function here
	return CAM_ERROR_NONE;
}

// get shot info
static CAM_Error _GT2005FillFrameShotInfo( GT2005State *pState, CAM_ImageBuffer *pImgBuf )
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
