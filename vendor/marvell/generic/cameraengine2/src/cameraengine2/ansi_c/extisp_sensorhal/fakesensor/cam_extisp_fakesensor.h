/*******************************************************************************
//(C) Copyright [2010 - 2011] Marvell International Ltd.
//All Rights Reserved
*******************************************************************************/

#ifndef _CAM_FAKESENSOR_H_
#define _CAM_FAKESENSOR_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct 
{
	_V4L2SensorState    stV4L2;
	CAM_ImageBuffer     *pImgBuf[CAM_MAX_PORT_BUF_CNT];
} FakeSensorState;

typedef void (*FakeSensor_ShotModeCap)( CAM_ShotModeCapability *pShotModeCap );

#ifdef __cplusplus
}
#endif

#endif  // _CAM_FAKESENSOR_H_
