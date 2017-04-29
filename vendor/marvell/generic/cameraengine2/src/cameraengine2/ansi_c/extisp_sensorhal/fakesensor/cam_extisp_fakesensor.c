/*******************************************************************************
//(C) Copyright [2010 - 2011] Marvell International Ltd.
//All Rights Reserved
*******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "cam_log.h"
#include "cam_utility.h"
#include "misc.h"


#include "cam_extisp_sensorwrapper.h"
#include "cam_extisp_v4l2base.h"
#include "cam_extisp_fakesensor.h"


/* fake sensor video/preview resolution table */
CAM_Size _FakeSensorVideoResTable[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT] = 
{
	{176, 144},
	{320, 240},
	{640, 480},
	{720, 480},
	{0, 0},
};

/* fake sensor still resolution table */
CAM_Size _FakeSensorStillResTable[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT] = 
{
	{2592, 1944},
	{0, 0},
};

/* fake sensor video/preview format table */
CAM_ImageFormat _FakeSensorVideoFormatTable[CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT] = 
{ 
	CAM_IMGFMT_YCbCr420P,
	CAM_IMGFMT_CbYCrY,
	0,
};

/* fake sensor still format table */
CAM_ImageFormat _FakeSensorStillFormatTable[CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT] = 
{
	CAM_IMGFMT_YCbCr420P,
	0,
};


CAM_FlipRotate _FakeSensorRotationTable[] = 
{
	CAM_FLIPROTATE_NORMAL,
};


CAM_ShotMode _FakeSensorShotModeTable[] = 
{
	CAM_SHOTMODE_AUTO,
};

_CAM_RegEntry _FakeSensorExpMeter_Mean[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _FakeSensorExpMeter[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_EXPOSUREMETERMODE_MEAN, _FakeSensorExpMeter_Mean),
};

_CAM_RegEntry _FakeSensorIsoMode_Auto[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _FakeSensorIsoMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_ISO_AUTO, _FakeSensorIsoMode_Auto),
};

_CAM_RegEntry _FakeSensorBdFltMode_Auto[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _FakeSensorBdFltMode_Off[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _FakeSensorBdFltMode_50Hz[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _FakeSensorBdFltMode_60Hz[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _FakeSensorBdFltMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_BANDFILTER_AUTO, _FakeSensorBdFltMode_Auto),
};

_CAM_RegEntry _FakeSensorFlashMode_Off[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _FakeSensorFlashMode_On[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _FakeSensorFlashMode_Auto[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _FakeSensorFlashMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_FLASH_OFF,	_FakeSensorFlashMode_Off),
};

_CAM_RegEntry _FakeSensorWBMode_Auto[] =
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _FakeSensorWBMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_WHITEBALANCEMODE_AUTO, _FakeSensorWBMode_Auto),
};

_CAM_RegEntry _FakeSensorFocusMode_Infinity[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _FakeSensorFocusMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_FOCUS_INFINITY,  _FakeSensorFocusMode_Infinity),
};

_CAM_RegEntry _FakeSensorColorEffectMode_Off[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _FakeSensorColorEffectMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_COLOREFFECT_OFF, _FakeSensorColorEffectMode_Off),
};

static CAM_Error _FakeSensorFillFrameShotInfo( FakeSensorState*, CAM_ImageBuffer* );

// shot mode capability
static void _FakeSensorAutoModeCap( CAM_ShotModeCapability* );

/* shot mode cap function table */
FakeSensor_ShotModeCap _FakeSensorShotModeCap[CAM_SHOTMODE_NUM] = { _FakeSensorAutoModeCap , // CAM_SHOTMODE_AUTO = 0,
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

extern _SmartSensorFunc func_fakesensor; 
CAM_Error FakeSensor_SelfDetect( _SmartSensorAttri *pSensorInfo )
{
	CAM_Error error = CAM_ERROR_NONE;

	_V4L2SensorEntry *pSensorEntry = (_V4L2SensorEntry*)( pSensorInfo->cReserved );
	strcpy( pSensorInfo->sSensorName, "BasicSensor-fake" );
	pSensorInfo->pFunc = &func_fakesensor;

	strcpy( pSensorEntry->sDeviceName, "\0" );
	pSensorEntry->iV4L2SensorID = 0;

	return error;
} 


CAM_Error FakeSensor_GetCapability( _CAM_SmartSensorCapability *pCapability )
{
	CAM_Int32s i = 0;

	// preview/video supporting 
	// format
	pCapability->iSupportedVideoFormatCnt = 0;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT; i++ ) 
	{
		if ( _FakeSensorVideoFormatTable[i] == 0 )
		{
			break;
		}
		pCapability->eSupportedVideoFormat[i] = _FakeSensorVideoFormatTable[i];
		pCapability->iSupportedVideoFormatCnt++;
	}

	pCapability->bArbitraryVideoSize     = CAM_FALSE;
	pCapability->iSupportedVideoSizeCnt  = 0;
	pCapability->stMinVideoSize.iWidth   = _FakeSensorVideoResTable[0].iWidth;
	pCapability->stMinVideoSize.iHeight  = _FakeSensorVideoResTable[0].iHeight;
	pCapability->stMaxVideoSize.iWidth   = _FakeSensorVideoResTable[0].iWidth;
	pCapability->stMaxVideoSize.iHeight  = _FakeSensorVideoResTable[0].iHeight;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_SIZE_CNT; i++ )
	{
		if ( _FakeSensorVideoResTable[i].iWidth == 0 || _FakeSensorVideoResTable[i].iHeight == 0 )
		{
			break;
		}
		pCapability->stSupportedVideoSize[pCapability->iSupportedVideoSizeCnt] = _FakeSensorVideoResTable[i];
		pCapability->iSupportedVideoSizeCnt++;

		if ( ( pCapability->stMinVideoSize.iWidth > _FakeSensorVideoResTable[i].iWidth ) || 
		     ( ( pCapability->stMinVideoSize.iWidth == _FakeSensorVideoResTable[i].iWidth ) && ( pCapability->stMinVideoSize.iHeight > _FakeSensorVideoResTable[i].iHeight ) ) ) 
		{
			pCapability->stMinVideoSize.iWidth  = _FakeSensorVideoResTable[i].iWidth;
			pCapability->stMinVideoSize.iHeight = _FakeSensorVideoResTable[i].iHeight;
		}
		if ( ( pCapability->stMaxVideoSize.iWidth < _FakeSensorVideoResTable[i].iWidth) ||
		     ( ( pCapability->stMaxVideoSize.iWidth == _FakeSensorVideoResTable[i].iWidth ) && ( pCapability->stMaxVideoSize.iHeight < _FakeSensorVideoResTable[i].iHeight ) ) )
		{
			pCapability->stMaxVideoSize.iWidth  = _FakeSensorVideoResTable[i].iWidth;
			pCapability->stMaxVideoSize.iHeight = _FakeSensorVideoResTable[i].iHeight;
		}
	}

	// still capture supporting
	// format
	pCapability->stSupportedJPEGParams.bSupportJpeg = CAM_FALSE;
	pCapability->iSupportedStillFormatCnt = 0;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT; i++ ) 
	{
		if ( _FakeSensorStillFormatTable[i] == 0 )
		{
			break;
		}
		pCapability->eSupportedStillFormat[pCapability->iSupportedStillFormatCnt] = _FakeSensorStillFormatTable[i];
		pCapability->iSupportedStillFormatCnt++;
	}
	// resolution
	pCapability->bArbitraryStillSize    = CAM_FALSE;
	pCapability->iSupportedStillSizeCnt = 0;
	pCapability->stMinStillSize.iWidth  = _FakeSensorStillResTable[0].iWidth;
	pCapability->stMinStillSize.iHeight = _FakeSensorStillResTable[0].iHeight;
	pCapability->stMaxStillSize.iWidth  = _FakeSensorStillResTable[0].iWidth;
	pCapability->stMaxStillSize.iHeight = _FakeSensorStillResTable[0].iHeight;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_SIZE_CNT; i++ )
	{
		if ( _FakeSensorStillResTable[i].iWidth == 0 || _FakeSensorStillResTable[i].iHeight == 0 )
		{
			break;
		}

		pCapability->stSupportedStillSize[pCapability->iSupportedStillSizeCnt] = _FakeSensorStillResTable[i];
		pCapability->iSupportedStillSizeCnt++;

		if ( ( pCapability->stMinStillSize.iWidth > _FakeSensorStillResTable[i].iWidth ) ||
		     ( ( pCapability->stMinStillSize.iWidth == _FakeSensorStillResTable[i].iWidth ) && ( pCapability->stMinStillSize.iHeight > _FakeSensorStillResTable[i].iHeight ) ) )
		{
			pCapability->stMinStillSize.iWidth  = _FakeSensorStillResTable[i].iWidth;
			pCapability->stMinStillSize.iHeight = _FakeSensorStillResTable[i].iHeight;
		}
		if ( ( pCapability->stMaxStillSize.iWidth < _FakeSensorStillResTable[i].iWidth ) || 
		     ( ( pCapability->stMaxStillSize.iWidth == _FakeSensorStillResTable[i].iWidth ) && ( pCapability->stMaxStillSize.iHeight < _FakeSensorStillResTable[i].iHeight ) ) )
		{
			pCapability->stMaxStillSize.iWidth = _FakeSensorStillResTable[i].iWidth;
			pCapability->stMaxStillSize.iHeight = _FakeSensorStillResTable[i].iHeight;
		}
	}

	// rotate
	pCapability->iSupportedRotateCnt = _ARRAY_SIZE( _FakeSensorRotationTable );
	for ( i = 0; i < pCapability->iSupportedRotateCnt; i++ )
	{
		pCapability->eSupportedRotate[i] = _FakeSensorRotationTable[i];
	}

	pCapability->iSupportedShotModeCnt = _ARRAY_SIZE(_FakeSensorShotModeTable);
	for ( i = 0; i < pCapability->iSupportedShotModeCnt; i++ )
	{
		pCapability->eSupportedShotMode[i] = _FakeSensorShotModeTable[i];
	}
	
	// FIXME: all supported shot parameters
	_FakeSensorAutoModeCap( &pCapability->stSupportedShotParams );

	return CAM_ERROR_NONE;
}

CAM_Error FakeSensor_GetShotModeCapability( CAM_ShotMode eShotMode, CAM_ShotModeCapability *pShotModeCap )
{
	CAM_Int32u i;

	// BAC check
	for ( i = 0; i < _ARRAY_SIZE( _FakeSensorShotModeTable ); i++ )
	{
		if ( _FakeSensorShotModeTable[i] == eShotMode )
		{
			break;
		}
	}

	if ( i >= _ARRAY_SIZE( _FakeSensorShotModeTable ) || pShotModeCap ==NULL ) 
	{
		return CAM_ERROR_BADARGUMENT;
	}

	(void)(_FakeSensorShotModeCap[eShotMode])( pShotModeCap );

	return CAM_ERROR_NONE;
}

CAM_Error FakeSensor_Init( void **ppDevice, void *pSensorEntry )
{
	CAM_Error        error    = CAM_ERROR_NONE;
	_V4L2SensorEntry *pSensor = (_V4L2SensorEntry*)pSensorEntry;
	FakeSensorState  *pState  = (FakeSensorState*)malloc( sizeof( FakeSensorState ) );
	
	*ppDevice = pState;
	if ( *ppDevice == NULL )
	{
		return CAM_ERROR_OUTOFMEMORY;
	}
	memset( pState, 0, sizeof( FakeSensorState ) );

	pState->stV4L2.stConfig.eState = CAM_CAPTURESTATE_IDLE;

	/* here we can get default shot params */
	pState->stV4L2.stShotParam.eShotMode          = CAM_SHOTMODE_AUTO;
	pState->stV4L2.stShotParam.eExpMode           = CAM_EXPOSUREMODE_AUTO;
	pState->stV4L2.stShotParam.eExpMeterMode      = CAM_EXPOSUREMETERMODE_AUTO;
	pState->stV4L2.stShotParam.iEvCompQ16         = 0;
	pState->stV4L2.stShotParam.eIsoMode           = CAM_ISO_AUTO;
	pState->stV4L2.stShotParam.iShutterSpeedQ16   = -1;
	pState->stV4L2.stShotParam.iFNumQ16           = 1;
	pState->stV4L2.stShotParam.eBandFilterMode    = CAM_BANDFILTER_50HZ;
	pState->stV4L2.stShotParam.eWBMode            = CAM_WHITEBALANCEMODE_AUTO;
	pState->stV4L2.stShotParam.eFocusMode         = CAM_FOCUS_INFINITY;
	pState->stV4L2.stShotParam.iDigZoomQ16        = 0;
	pState->stV4L2.stShotParam.eColorEffect       = CAM_COLOREFFECT_OFF;
	pState->stV4L2.stShotParam.iSaturation        = 64;
	pState->stV4L2.stShotParam.iBrightness        = 0;
	pState->stV4L2.stShotParam.iContrast          = 0;
	pState->stV4L2.stShotParam.iSharpness         = 0;
	pState->stV4L2.stShotParam.eFlashMode         = CAM_FLASH_OFF;
	pState->stV4L2.stShotParam.bVideoStabilizer   = CAM_FALSE;
	pState->stV4L2.stShotParam.bVideoNoiseReducer = CAM_FALSE;
	pState->stV4L2.stShotParam.bZeroShutterLag    = CAM_FALSE;
	pState->stV4L2.stShotParam.bHighDynamicRange  = CAM_FALSE;

	pState->stV4L2.stShotParam.iBurstCnt          = 1;


	return error;
}

CAM_Error FakeSensor_Deinit( void *pDevice )
{
	CAM_Error       error   = CAM_ERROR_NONE;
	FakeSensorState *pState = (FakeSensorState*)pDevice;

	free( pDevice );

	return error;
}

CAM_Error FakeSensor_RegisterEventHandler( void *pDevice, CAM_EventHandler fnEventHandler, void *pUserData )
{
	FakeSensorState  *pState = (FakeSensorState*)pDevice;
	CAM_Error        error   = CAM_ERROR_NONE;

	error = V4L2_RegisterEventHandler( &pState->stV4L2, fnEventHandler, pUserData );
	
	return error;
}

CAM_Error FakeSensor_Config( void *pDevice, _CAM_SmartSensorConfig *pSensorConfig )
{
	CAM_Error       error   = CAM_ERROR_NONE;
	FakeSensorState *pState = (FakeSensorState*)pDevice;

	pState->stV4L2.stConfig = *pSensorConfig;

	return error;
}

CAM_Error FakeSensor_GetBufReq( void *pDevice, _CAM_SmartSensorConfig *pSensorConfig, CAM_ImageBufferReq *pBufReq )
{
	CAM_Error       error   = CAM_ERROR_NONE;
	FakeSensorState *pState = (FakeSensorState*)pDevice;

	error = V4L2_GetBufReq( &pState->stV4L2, pSensorConfig, pBufReq );

	return error;
}

CAM_Error FakeSensor_Enqueue( void *pDevice, CAM_ImageBuffer *pImgBuf )
{
	FakeSensorState *pState = (FakeSensorState*)pDevice;
	CAM_Int32s      i       = 0;

	// BAC check
	_CHECK_BAD_POINTER( pDevice );
	_CHECK_BAD_POINTER( pImgBuf );

	for ( i = 0; i < CAM_MAX_PORT_BUF_CNT; i++ )
	{
		if ( pState->pImgBuf[i] == NULL )
		{
			break;
		}
	}

	// no position for new buffer
	if ( i >= CAM_MAX_PORT_BUF_CNT )
	{
		TRACE( CAM_ERROR, "Error: current buffer number beyond CAM_MAX_PORT_BUF_CNT( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__  );
		return CAM_ERROR_OUTOFRESOURCE;
	}

	pState->pImgBuf[i] = pImgBuf;

	return CAM_ERROR_NONE;
}

CAM_Error FakeSensor_Dequeue(void *pDevice, CAM_ImageBuffer **ppImgBuf)
{
	FakeSensorState *pState = (FakeSensorState*)pDevice;
	CAM_Int32s      i       = 0;

	for ( i = 0; i < CAM_MAX_PORT_BUF_CNT; i++ )
	{
		if ( pState->pImgBuf[i] != NULL )
		{
			break;
		}
	}

	ASSERT( i < CAM_MAX_PORT_BUF_CNT );

	// fill with red color
	memset( pState->pImgBuf[i]->pBuffer[0], 0x4c, pState->pImgBuf[i]->iAllocLen[0] );
	memset( pState->pImgBuf[i]->pBuffer[1], 0x55, pState->pImgBuf[i]->iAllocLen[1] );
	memset( pState->pImgBuf[i]->pBuffer[2], 0xff, pState->pImgBuf[i]->iAllocLen[2] );

	(void)_calcimglen( pState->pImgBuf[i] );
	
	*ppImgBuf = pState->pImgBuf[i];

	// add image info
	_FakeSensorFillFrameShotInfo( pState, *ppImgBuf );

	pState->pImgBuf[i] = NULL;

	return CAM_ERROR_NONE;
}

CAM_Error FakeSensor_Flush(void *pDevice)
{
	FakeSensorState *pState = (FakeSensorState*)pDevice;
	CAM_Int32s      i       = 0;

	for ( i = 0; i < CAM_MAX_PORT_BUF_CNT; i++ )
	{
		pState->pImgBuf[i] = NULL;
	}

	return CAM_ERROR_NONE;
}

CAM_Error FakeSensor_SetShotParam( void *pDevice, _CAM_ShotParam *pShotParam )
{
	FakeSensorState *pState = (FakeSensorState*)pDevice;

	return CAM_ERROR_NONE;
}

CAM_Error FakeSensor_GetShotParam( void *pDevice, _CAM_ShotParam *pShotParam )
{
	FakeSensorState *pState = (FakeSensorState*)pDevice;
    
	*pShotParam = pState->stV4L2.stShotParam;

	return CAM_ERROR_NONE;
}

CAM_Error FakeSensor_StartFocus( void *pDevice, void *pFocusROI )
{
	return CAM_ERROR_NONE;
}

CAM_Error FakeSensor_CancelFocus( void *pDevice )
{
	return CAM_ERROR_NONE;
}

CAM_Error FakeSensor_CheckFocusState( void *pDevice, CAM_Bool *pFocusAutoStopped, _CAM_FocusState *pFocusState )
{
	*pFocusAutoStopped = CAM_TRUE;
	*pFocusState       = CAM_IN_FOCUS;

	return CAM_ERROR_NONE;
}


CAM_Error FakeSensor_GetDigitalZoomCapability( CAM_Int32s iWidth, CAM_Int32s iHeight, CAM_Int32s *pSensorDigZoomQ16 )
{
	*pSensorDigZoomQ16 = ( 1 << 16 );

	return CAM_ERROR_NONE;
}

_SmartSensorFunc func_fakesensor = 
{
	FakeSensor_GetCapability,
	FakeSensor_GetShotModeCapability,

	FakeSensor_SelfDetect,
	FakeSensor_Init,
	FakeSensor_Deinit,
	FakeSensor_RegisterEventHandler,
	FakeSensor_Config,
	FakeSensor_GetBufReq,
	FakeSensor_Enqueue,
	FakeSensor_Dequeue,
	FakeSensor_Flush,
	FakeSensor_SetShotParam,
	FakeSensor_GetShotParam,

	FakeSensor_StartFocus,
	FakeSensor_CancelFocus,
	FakeSensor_CheckFocusState,
	FakeSensor_GetDigitalZoomCapability,

};

/*-------------------------------------------------------------------------------------------------------------------------------------
 * FakeSensor shotmode capability
 * TODO: if you enable new shot mode, pls add a correspoding modcap function here, and add it to FakeSensor_shotModeCap _FakeSensorShotModeCap array
 *------------------------------------------------------------------------------------------------------------------------------------*/
static void _FakeSensorAutoModeCap( CAM_ShotModeCapability *pShotModeCap )
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

// get shot info
static CAM_Error _FakeSensorFillFrameShotInfo( FakeSensorState *pState, CAM_ImageBuffer *pImgBuf )
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
