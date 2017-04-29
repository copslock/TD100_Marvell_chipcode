/******************************************************************************
//(C) Copyright [2010 - 2011] Marvell International Ltd.
//All Rights Reserved
******************************************************************************/

#ifndef _CAM_EXTISP_SENSORHAL_H_
#define _CAM_EXTISP_SENSORHAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "CameraEngine.h"
#include "cam_extisp_buildopt.h"

typedef struct
{
	CAM_ShotMode                   eShotMode;

	CAM_FlipRotate                 eSensorRotation;

	CAM_ExposureMode               eExpMode;
	CAM_ExposureMeterMode          eExpMeterMode;
	CAM_ISOMode                    eIsoMode;
	CAM_BandFilterMode             eBandFilterMode;
	CAM_FlashMode                  eFlashMode;
	CAM_WhiteBalanceMode           eWBMode;
	CAM_FocusMode                  eFocusMode;
	CAM_ColorEffect                eColorEffect;

	CAM_Int32s                     iEvCompQ16;
	CAM_Int32s                     iShutterSpeedQ16;
	CAM_Int32s                     iFNumQ16;

	CAM_Int32s                     iDigZoomQ16;
	CAM_Int32s                     iOptZoomQ16;

	CAM_Int32s                     iSaturation;
	CAM_Int32s                     iBrightness;
	CAM_Int32s                     iContrast;
	CAM_Int32s                     iSharpness;

	CAM_Bool                       bVideoStabilizer;
	CAM_Bool                       bVideoNoiseReducer;
	CAM_Bool                       bZeroShutterLag;
	CAM_Bool                       bHighDynamicRange;

	CAM_Int32s                     iBurstCnt;
} _CAM_ShotParam;

typedef struct 
{
	CAM_Bool                       bArbitraryVideoSize;
	CAM_Size                       stMinVideoSize;
	CAM_Size                       stMaxVideoSize;

	CAM_Int32s                     iSupportedVideoSizeCnt;
	CAM_Size                       stSupportedVideoSize[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT];

	CAM_Int32s                     iSupportedVideoFormatCnt;
	CAM_ImageFormat                eSupportedVideoFormat[CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT];

	CAM_Bool                       bArbitraryStillSize;
	CAM_Size                       stMinStillSize;
	CAM_Size                       stMaxStillSize;

	CAM_Int32s                     iSupportedStillSizeCnt;
	CAM_Size                       stSupportedStillSize[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT];

	CAM_Int32s                     iSupportedStillFormatCnt;
	CAM_ImageFormat                eSupportedStillFormat[CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT];

	CAM_Int32s                     iSupportedRotateCnt;
	CAM_FlipRotate                 eSupportedRotate[CAM_FLIPROTATE_NUM];

	CAM_Int32s                     iSupportedShotModeCnt;
	CAM_ShotMode                   eSupportedShotMode[CAM_SHOTMODE_NUM];

	// the assembly of all supported shot paramters
	CAM_ShotModeCapability         stSupportedShotParams;

	CAM_JpegCapability             stSupportedJPEGParams;
} _CAM_SmartSensorCapability;


typedef struct
{
	CAM_CaptureState               eState;

	CAM_Int32s                     iWidth;
	CAM_Int32s                     iHeight;
	CAM_ImageFormat                eFormat;
	CAM_Int32u                     iTargetFpsQ16;
	CAM_JpegParam                  stJpegParam;

	// reset type
	CAM_Int32s                     iResetType;

	// Output
	CAM_Int32u                     iActualFpsQ16;
	CAM_Bool                       bFlushed;
} _CAM_SmartSensorConfig;

typedef enum
{
	CAM_KEEP_STATUS,
	CAM_IN_FOCUS,
	CAM_OUTOF_FOCUS,
	CAM_AF_FAIL,

	CAM_AFSTATE_LIMIT = 0x7fffffff,
}_CAM_FocusState;


CAM_Error SmartSensor_EnumSensors( CAM_Int32s *pSensorCount, CAM_Int8s sSensorName[4][32] );
CAM_Error SmartSensor_GetCapability( CAM_Int32s iSensorID, _CAM_SmartSensorCapability *pCapability );
CAM_Error SmartSensor_GetShotModeCapability( CAM_Int32s iSensorID, CAM_ShotMode eShotMode, CAM_ShotModeCapability *pShotModeCap );
CAM_Error SmartSensor_GetDigitalZoomCapability( CAM_Int32s iSensorID, CAM_Int32s iWidth, CAM_Int32s iHeight, CAM_Int32s *pSensorDigZoomQ16 );

CAM_Error SmartSensor_Init( void **phDevice, CAM_Int32s iSensorID );
CAM_Error SmartSensor_Deinit( void **phDevice );
CAM_Error SmartSensor_RegisterEventHandler( void *hDevice, CAM_EventHandler fnEventHandler, void *pUserData );
CAM_Error SmartSensor_Config( void *hDevice, _CAM_SmartSensorConfig *pSensorConfig );
CAM_Error SmartSensor_GetBufReq( void *hDevice, _CAM_SmartSensorConfig *pSensorConfig, CAM_ImageBufferReq *pBufReq );

CAM_Error SmartSensor_Enqueue( void *hDevice, CAM_ImageBuffer *pImgBuf );
CAM_Error SmartSensor_Dequeue( void *hDevice, CAM_ImageBuffer **ppImgBuf );
CAM_Error SmartSensor_Flush( void *hDevice );

CAM_Error SmartSensor_SetShotParam( void *hDevice, _CAM_ShotParam *pSettings );
CAM_Error SmartSensor_GetShotParam( void *hDevice, _CAM_ShotParam *pSettings );
CAM_Error SmartSensor_StartFocus( void *hDevice, void *pFocusROI );
CAM_Error SmartSensor_CancelFocus( void *hDevice );
CAM_Error SmartSensor_CheckFocusState( void *hDevice, CAM_Bool *pFocusAutoStopped, _CAM_FocusState *pFocusState );


#ifdef __cplusplus
}
#endif

#endif  // _CAM_EXTISP_SENSORHAL_H_


