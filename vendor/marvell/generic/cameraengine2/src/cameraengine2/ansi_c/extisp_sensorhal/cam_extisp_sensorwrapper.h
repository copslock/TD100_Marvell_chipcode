/*******************************************************************************
//(C) Copyright [2010 - 2011] Marvell International Ltd.
//All Rights Reserved
*******************************************************************************/

#ifndef _CAM_SENSORHAL_WRAPPER_H_
#define _CAM_SENSORHAL_WRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "cam_extisp_sensorhal.h"

typedef struct 
{
	CAM_Error (*GetCapability)( _CAM_SmartSensorCapability *pCapability );
	CAM_Error (*GetShotModeCapability)( CAM_ShotMode eShotMode, CAM_ShotModeCapability *pShotModeCap );
	
	CAM_Error (*SelfDetect)( void *pSensorData ); // in this case, the parameters should be defined as _SmartSensorAttri*, to avoid complilation error,define it with void* as a trick
	CAM_Error (*Init)( void **ppDevice, void *pSensorEntry );
	CAM_Error (*Deinit)( void *pDevice );
	CAM_Error (*RegisterEventHandler)( void *pDevice, CAM_EventHandler fnEventHandler, void *pUserData );
	CAM_Error (*Config)( void *pDevice, _CAM_SmartSensorConfig *pSensorConfig );
	CAM_Error (*GetBufReq)( void *pDevice, _CAM_SmartSensorConfig *pSensorConfig, CAM_ImageBufferReq *pBufReq );
	CAM_Error (*Enqueue)( void *pDevice, CAM_ImageBuffer *pImgBuf );
	CAM_Error (*Dequeue)( void *pDevice, CAM_ImageBuffer **ppImgBuf );
	CAM_Error (*Flush)( void *pDevice );
	CAM_Error (*SetShotParam)( void *pDevice, _CAM_ShotParam *pSettings );
	CAM_Error (*GetShotParam)( void *pDevice, _CAM_ShotParam *pSettings );
	CAM_Error (*StartFocus)( void *pDevice, void *pFocusROI );
	CAM_Error (*CancelFocus)( void *pDevice );
	CAM_Error (*CheckFocusState)( void *pDevice, CAM_Bool *pFocusAutoStopped, _CAM_FocusState *pFocusState );	
	CAM_Error (*GetDigitalZoomCapability)( CAM_Int32s iWidth, CAM_Int32s iHeight, CAM_Int32s *pSensorDigZoomQ16 );
} _SmartSensorFunc;

typedef struct
{
	CAM_Int8s			sSensorName[32];
	_SmartSensorFunc	*pFunc;
	CAM_Int8s           cReserved[50]; // a field reverved for sensor entry in differnt platform	
}_SmartSensorAttri;

#ifdef __cplusplus
}
#endif

#endif  // _CAM_SENSORHAL_WRAPPER_H_


