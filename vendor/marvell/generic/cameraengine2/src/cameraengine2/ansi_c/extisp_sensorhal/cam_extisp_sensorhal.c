/*******************************************************************************
// (C) Copyright [2010 - 2011] Marvell International Ltd.
// All Rights Reserved
*******************************************************************************/

#include <string.h>
#include <stdlib.h>

#include "cam_utility.h"
#include "cam_log.h"
#include "cam_extisp_sensorwrapper.h"


typedef struct 
{
	CAM_Int8s               sSensorName[32];
	_SmartSensorFunc        *pFunc;
} _SmartSensorEntry;

typedef struct 
{
	CAM_Int32s              iSensorID;
	void                    *pDevice;
} _SmartSensorState;


#if defined( BUILD_OPTION_DEBUG_ENABLE_FAKE_SENSOR )

extern _SmartSensorFunc func_fakesensor;
static const _SmartSensorEntry gSmartSensorTable[] = 
{
	{"fakesensor", &func_fakesensor},
};

#else

// sensors Camera Engine has tuned in reference implementation, also an interface for Fast Validation Pass -- func_basicsensor
extern _SmartSensorFunc func_gt2005;
extern _SmartSensorFunc func_ov5642;
extern _SmartSensorFunc func_ov5640;
extern _SmartSensorFunc func_mt9v113;
extern _SmartSensorFunc func_basicsensor;

#if !defined( BUILD_OPTION_STARTEGY_DISABLE_DYNAMIC_SENSOR_DETECT )
static const _SmartSensorEntry gSmartSensorTable[] = 
{
#if ( PLATFORM_BOARD_VALUE == PLATFORM_BOARD_G50 )
	{"gt2005", &func_gt2005},
#else
	{"ov5642", &func_ov5642},
	{"ov5640", &func_ov5640},
	{"mt9v113", &func_mt9v113},
#endif
	{"basicsensor", &func_basicsensor},
};
#else
// XXX: add sensor's in your platforms here in BUILD_OPTION_STARTEGY_DISABLE_DYNAMIC_SENSOR_DETECT option, we will only detect only these sensors
static const _SmartSensorEntry gSmartSensorTable[] = 
{
	{"ov5642", &func_ov5642},
	{"mt9v113", &func_mt9v113},
};
#endif

#endif

assert_static( sizeof( gSmartSensorTable ) >= sizeof( _SmartSensorEntry ) );
static CAM_Int32s gSmartSensorCnt = _ARRAY_SIZE( gSmartSensorTable );

// sensors in current platform
static _SmartSensorAttri gCurrentSensorTable[4];
static CAM_Int32s        gCurrentSensorCnt  = 0;


CAM_Error SmartSensor_EnumSensors( CAM_Int32s *pSensorCount, CAM_Int8s sSensorName[4][32] )
{
	CAM_Int32s        i = 0, j = 0; 
	CAM_Error         error = CAM_ERROR_NONE;
	_SmartSensorAttri stDetectedSensors[4];

	// bad argument check
	if ( pSensorCount == NULL || sSensorName == NULL )
	{
		return CAM_ERROR_BADPOINTER;
	}

	if ( gCurrentSensorCnt == 0 )
	{
		for ( i = 0; i < gSmartSensorCnt; i++ )
		{
			memset( &stDetectedSensors, 0, sizeof( _SmartSensorAttri ) * 4 );
			error = gSmartSensorTable[i].pFunc->SelfDetect( stDetectedSensors );
			if( CAM_ERROR_NONE != error )
			{
				*pSensorCount = 0;
				TRACE( CAM_ERROR, "Error: cannot complete sensor self-detect, pls confirm your sensor/sensor driver/system are all ready( %s, %d )\n", 
				       __FILE__, __LINE__ );
				return error;
			}
			for ( j = 0; j < 4; j++ )
			{
				if ( stDetectedSensors[j].pFunc == NULL )
				{
					break;
				}
				strcpy( sSensorName[gCurrentSensorCnt], stDetectedSensors[j].sSensorName );
				gCurrentSensorTable[gCurrentSensorCnt] = stDetectedSensors[j];

				gCurrentSensorCnt++;

				if ( gCurrentSensorCnt >= 4 )
				{
					(*pSensorCount) = gCurrentSensorCnt;
					TRACE( CAM_ERROR, "Error: Camera Engine cannot support more than four sensors in one platform, so the potential more camera sensors will be ignored\
					       ( %s, %d )\n", __FILE__, __LINE__ );
					return CAM_ERROR_NONE;
				}
			}

		}

		*pSensorCount = gCurrentSensorCnt;
	}
	else
	{
		for ( i = 0; i < gCurrentSensorCnt; i++ )
		{
			strcpy( sSensorName[i], gCurrentSensorTable[i].sSensorName );
		}
		*pSensorCount = gCurrentSensorCnt;
	}

	return CAM_ERROR_NONE;
}

CAM_Error SmartSensor_GetCapability( CAM_Int32s iSensorID, _CAM_SmartSensorCapability *pCapability )
{
	CAM_Error error = CAM_ERROR_NONE;

	if ( iSensorID < 0 || iSensorID >= gCurrentSensorCnt )
	{
		TRACE( CAM_ERROR, "Error: sensor ID[ %d ] out of range( %s, %d )\n", iSensorID, __FILE__, __LINE__ );
		return CAM_ERROR_BADSENSORID;
	}

	memset( pCapability, 0, sizeof( _CAM_SmartSensorCapability ) );

	error = gCurrentSensorTable[iSensorID].pFunc->GetCapability( pCapability );

	// This is the basic requirement to the sensor HAL implementations
	// If sensor HAL does not conform to this assumption, upper level may not work correctly
	ASSERT( pCapability->iSupportedVideoSizeCnt > 0 );
	ASSERT( pCapability->iSupportedVideoFormatCnt > 0 );
	ASSERT( pCapability->iSupportedStillSizeCnt > 0 );
	ASSERT( pCapability->iSupportedStillFormatCnt > 0 );

	return error;
}

CAM_Error SmartSensor_GetShotModeCapability( CAM_Int32s iSensorID, CAM_ShotMode eShotMode, CAM_ShotModeCapability *pShotModeCap )
{
	CAM_Error error = CAM_ERROR_NONE;

	if ( iSensorID < 0 || iSensorID >= gCurrentSensorCnt )
	{
		TRACE( CAM_ERROR, "Error: sensor ID[ %d ] out of range( %s, %d )\n", iSensorID, __FUNCTION__, __LINE__ );
		return CAM_ERROR_BADSENSORID;
	}

	error = gCurrentSensorTable[iSensorID].pFunc->GetShotModeCapability( eShotMode, pShotModeCap );
	return error;
}

CAM_Error SmartSensor_GetDigitalZoomCapability( CAM_Int32s iSensorID, CAM_Int32s iWidth, CAM_Int32s iHeight, CAM_Int32s *pSensorDigZoomQ16 )
{
	CAM_Error error = CAM_ERROR_NONE;

	if ( iSensorID < 0 || iSensorID >= gCurrentSensorCnt )
	{
		TRACE( CAM_ERROR, "Error: sensor ID[ %d ] out of range( %s, %d )\n", iSensorID, __FUNCTION__, __LINE__ );
		return CAM_ERROR_BADSENSORID;
	}

	error = gCurrentSensorTable[iSensorID].pFunc->GetDigitalZoomCapability( iWidth, iHeight, pSensorDigZoomQ16 );
	return error;
}

CAM_Error SmartSensor_Init( void **phDevice, int iSensorID )
{
	CAM_Error         error   = CAM_ERROR_NONE;
	_SmartSensorState *pState = (_SmartSensorState*)malloc( sizeof(_SmartSensorState) );
	if ( pState == NULL )
	{
		TRACE( CAM_ERROR, "Error: No memeory for sensor[%d]( %s, %d )\n", iSensorID, __FILE__, __LINE__ );
		return CAM_ERROR_OUTOFMEMORY;
	}

	error = gCurrentSensorTable[iSensorID].pFunc->Init( &pState->pDevice, (void*)(gCurrentSensorTable[iSensorID].cReserved) );
	
	if ( CAM_ERROR_NONE == error )
	{
		pState->iSensorID = iSensorID;
		*phDevice         = pState;
	}
	else
	{
		free( pState );
		*phDevice = NULL;
	}

	return error;
}

CAM_Error SmartSensor_Deinit( void **phDevice )
{
	CAM_Error         error   = CAM_ERROR_NONE;
	_SmartSensorState *pState = NULL;

	if ( phDevice == NULL || *phDevice == NULL || ((_SmartSensorState*)(*phDevice))->pDevice == NULL )
	{
		TRACE( CAM_ERROR, "Error: invalid sensor handle( %s, %d )\n", __FILE__, __LINE__ );
		return CAM_ERROR_BADSENSORID;
	}

	pState = (_SmartSensorState*)(*phDevice);
	error = gCurrentSensorTable[pState->iSensorID].pFunc->Deinit( pState->pDevice );
	ASSERT( CAM_ERROR_NONE == error );

	free( pState );
	pState    = NULL;
	*phDevice = NULL;

	return error;
}

CAM_Error SmartSensor_RegisterEventHandler( void *hDevice, CAM_EventHandler fnEventHandler, void *pUserData )
{
	CAM_Error             error   = CAM_ERROR_NONE;
	_SmartSensorState *pState = (_SmartSensorState*)hDevice;

	if ( pState == NULL || pState->pDevice == NULL )
	{
		TRACE( CAM_ERROR, "Error: invalid sensor handle( %s, %d )\n", __FILE__, __LINE__ );
		return CAM_ERROR_BADSENSORID;
        }

	error = gCurrentSensorTable[pState->iSensorID].pFunc->RegisterEventHandler( pState->pDevice, fnEventHandler, pUserData );
	return error;
}

CAM_Error SmartSensor_Config( void *hDevice, _CAM_SmartSensorConfig *pSensorConfig )
{
	CAM_Error         error   = CAM_ERROR_NONE;
	_SmartSensorState *pState = (_SmartSensorState*)hDevice;

	if ( pState == NULL || pState->pDevice == NULL )
	{
		TRACE( CAM_ERROR, "Error: invalid sensor handle( %s, %d )\n", __FILE__, __LINE__ );
		return CAM_ERROR_BADSENSORID;
	}

	error = gCurrentSensorTable[pState->iSensorID].pFunc->Config( pState->pDevice, pSensorConfig );
	return error;
}

CAM_Error SmartSensor_GetBufReq( void *hDevice, _CAM_SmartSensorConfig *pSensorConfig, CAM_ImageBufferReq *pBufReq )
{
	CAM_Error         error   = CAM_ERROR_NONE;
	_SmartSensorState *pState = (_SmartSensorState*)hDevice;

	if ( pState == NULL || pState->pDevice == NULL ) 
	{
		TRACE( CAM_ERROR, "Error: invalid sensor handle( %s, %d )\n", __FILE__, __LINE__ );
		return CAM_ERROR_BADSENSORID;
	}

	error = gCurrentSensorTable[pState->iSensorID].pFunc->GetBufReq(pState->pDevice, pSensorConfig, pBufReq);
	return error;
}

CAM_Error SmartSensor_Enqueue( void *hDevice, CAM_ImageBuffer *pImgBuf )
{
	CAM_Error         error   = CAM_ERROR_NONE;
	_SmartSensorState *pState = (_SmartSensorState*)hDevice;

	if ( pState == NULL || pState->pDevice == NULL ) 
	{
		TRACE( CAM_ERROR, "Error: invalid sensor handle( %s, %d )\n", __FILE__, __LINE__ );
		return CAM_ERROR_BADSENSORID;
	}

	error = gCurrentSensorTable[pState->iSensorID].pFunc->Enqueue( pState->pDevice, pImgBuf );
	return error;
}

CAM_Error SmartSensor_Dequeue( void *hDevice, CAM_ImageBuffer **ppImgBuf )
{
	CAM_Error         error   = CAM_ERROR_NONE;
	_SmartSensorState *pState = (_SmartSensorState*)hDevice;

	if ( pState == NULL || pState->pDevice == NULL ) 
	{
		TRACE( CAM_ERROR, "Error: invalid sensor handle( %s, %d )\n", __FILE__, __LINE__ );
		return CAM_ERROR_BADSENSORID;
	}

	error = gCurrentSensorTable[pState->iSensorID].pFunc->Dequeue( pState->pDevice, ppImgBuf );
	return error;
}

CAM_Error SmartSensor_Flush( void *hDevice )
{
	CAM_Error         error   = CAM_ERROR_NONE;
	_SmartSensorState *pState = (_SmartSensorState*)hDevice;

	if ( pState == NULL || pState->pDevice == NULL ) 
	{
		TRACE( CAM_ERROR, "Error: invalid sensor handle( %s, %d )\n", __FILE__, __LINE__ );
		return CAM_ERROR_BADSENSORID;
	}

	error = gCurrentSensorTable[pState->iSensorID].pFunc->Flush( pState->pDevice );
	return error;
}

CAM_Error SmartSensor_SetShotParam( void *hDevice, _CAM_ShotParam *pSettings )
{
	CAM_Error         error   = CAM_ERROR_NONE;
	_SmartSensorState *pState = (_SmartSensorState*)hDevice;

	if ( pState == NULL || pState->pDevice == NULL ) 
	{
		TRACE( CAM_ERROR, "Error: invalid sensor handle( %s, %d )\n", __FILE__, __LINE__ );
		return CAM_ERROR_BADSENSORID;
	}
	
	error = gCurrentSensorTable[pState->iSensorID].pFunc->SetShotParam( pState->pDevice, pSettings );
	return error;
}

CAM_Error SmartSensor_GetShotParam( void *hDevice, _CAM_ShotParam *pSettings )
{
	CAM_Error         error   = CAM_ERROR_NONE;
	_SmartSensorState *pState = (_SmartSensorState*)hDevice;

	memset( pSettings, 0, sizeof(_CAM_ShotParam) );

	if ( pState == NULL || pState->pDevice == NULL ) 
	{
		TRACE( CAM_ERROR, "Error: invalid sensor handle( %s, %d )\n", __FILE__, __LINE__ );
		return CAM_ERROR_BADSENSORID;
	}

	error = gCurrentSensorTable[pState->iSensorID].pFunc->GetShotParam( pState->pDevice, pSettings );
	return error;
}

CAM_Error SmartSensor_StartFocus( void *hDevice, void *pFocusROI )
{
	CAM_Error         error   = CAM_ERROR_NONE;
	_SmartSensorState *pState = (_SmartSensorState*)hDevice;

	if ( pState == NULL || pState->pDevice == NULL ) 
	{
		TRACE( CAM_ERROR, "Error: invalid sensor handle( %s, %d )\n", __FILE__, __LINE__ );
		return CAM_ERROR_BADSENSORID;
	}

	error = gCurrentSensorTable[pState->iSensorID].pFunc->StartFocus( pState->pDevice, pFocusROI );
	return error;
}

CAM_Error SmartSensor_CancelFocus( void *hDevice )
{
	CAM_Error         error   = CAM_ERROR_NONE;
	_SmartSensorState *pState = (_SmartSensorState*)hDevice;

	if ( pState == NULL || pState->pDevice == NULL ) 
	{
		TRACE( CAM_ERROR, "Error: invalid sensor handle( %s, %d )\n", __FILE__, __LINE__ );
		return CAM_ERROR_BADSENSORID;
	}
	
	error = gCurrentSensorTable[pState->iSensorID].pFunc->CancelFocus( pState->pDevice );
	return error;
}

CAM_Error SmartSensor_CheckFocusState( void *hDevice, CAM_Bool *pFocusAutoStopped, _CAM_FocusState *pFocusState )
{
	CAM_Error         error   = CAM_ERROR_NONE;
	_SmartSensorState *pState = (_SmartSensorState*)hDevice;

	if ( pState == NULL || pState->pDevice == NULL ) 
	{
		TRACE( CAM_ERROR, "Error: invalid sensor handle( %s, %d )\n", __FILE__, __LINE__ );
		return CAM_ERROR_BADSENSORID;
	}

	error = gCurrentSensorTable[pState->iSensorID].pFunc->CheckFocusState( pState->pDevice, pFocusAutoStopped, pFocusState );
	return error;
}
