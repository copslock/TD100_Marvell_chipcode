/*******************************************************************************
//(C) Copyright [2010 - 2011] Marvell International Ltd.
//All Rights Reserved
*******************************************************************************/

#ifndef _CAM_GT2005_H_
#define _CAM_GT2005_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct 
{
	_V4L2SensorState	stV4L2;
} GT2005State;

typedef void (*GT2005_ShotModeCap)( CAM_ShotModeCapability *pShotModeCap );

#ifdef __cplusplus
}
#endif

#endif  // _CAM_GT2005_H_
