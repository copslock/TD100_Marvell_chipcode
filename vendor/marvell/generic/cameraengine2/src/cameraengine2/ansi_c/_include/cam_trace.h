/******************************************************************************
//(C) Copyright [2010 - 2011] Marvell International Ltd.
//All Rights Reserved
******************************************************************************/


#ifndef _CAM_TRACE_H_
#define _CAM_TRACE_H_


#include "CameraEngine.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TRACE info data type */
typedef struct 
{
	CAM_Command eCmd;
	const char  *pInfo;
} CAM_CmdInfo;

typedef struct 
{
	CAM_FlipRotate eRotate;
	const char     *pInfo;
} CAM_FlipRotInfo;

typedef struct 
{
	CAM_CaptureState eState;
	const char       *pInfo;
} CAM_StateInfo;

typedef struct 
{
	int          iPortId;
	const char   *pInfo;
} CAM_PortInfo;

typedef struct 
{
	CAM_Error   eErrorNo;
	const char  *pInfo;
} CAM_ResInfo;


/*trace function combo*/
int  _cam_inittrace();
void _cam_devtrace( void *pCameraState );
void _cam_cmdtrace( CAM_Int32s iSensorId, CAM_Command cmd, void *arg1, void *arg2, CAM_Tick *pTick );
void _cam_rettrace( CAM_Command cmd, void *arg1, void *arg2, CAM_Error eErrorNo, CAM_Tick tTick );
void _cam_closetrace();


#ifdef __cplusplus
}
#endif

#endif  // _CAM_TRACE_H_
