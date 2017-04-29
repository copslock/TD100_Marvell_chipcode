/*******************************************************************************
//(C) Copyright [2010 - 2011] Marvell International Ltd.
//All Rights Reserved
*******************************************************************************/

#ifndef _CAM_MT9V113_H_
#define _CAM_MT9V113_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct 
{
	_V4L2SensorState	stV4L2;
} MT9V113State;

typedef void (*MT9V113_ShotModeCap)( CAM_ShotModeCapability *pShotModeCap );

#ifdef __cplusplus
}
#endif

#endif  // _CAM_MT9V113_H_
