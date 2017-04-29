/*****************************************************************************
*(C) Copyright [2010 - 2011] Marvell International Ltd.
* All Rights Reserved
******************************************************************************/

#ifndef __CAM_ENGINE_EXTISP_H_
#define __CAM_ENGINE_EXTISP_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "CameraEngine.h"
#include "cam_extisp_sensorhal.h"
#include "cam_extisp_buildopt.h"
#include "cam_utility.h"

// @_CAM_PortState
typedef struct 
{
	// attribute information
	CAM_Int32s         iWidth;      // This is the width before ppu rotation
	CAM_Int32s         iHeight;     // This is the height before ppu rotation
	CAM_ImageFormat    eFormat;
	CAM_FlipRotate     eRotation;

	// buffer queue
	_CAM_Queue         qEmpty;
	_CAM_Queue         qFilled;

	// buffer requirement
	CAM_ImageBufferReq stBufReq;

	// port mutex
	void               *hMutex;
}_CAM_PortState;

// ext-isp camera-engine internal state structure
typedef struct 
{
	// camera-engine handle lock
	void                    *hCEMutex;

	// available sensors
	CAM_Int32s              iSensorCount;
	CAM_Int8s               sSensorName[4][32];

	// process buffer work thread management
#if !defined( BUILD_OPTION_DEBUG_DISABLE_PROCESS_THREAD )
	_CAM_Thread             stProcessBufferThread;
#endif
	// check-focus thread
	_CAM_Thread             stFocusThread;

	// event handler user registered to camera-engine
	CAM_EventHandler        fnEventHandler;
	void                    *pUserData;
    
	// post-processing unit handler
	void                    *hPpuHandle;
	CAM_Bool                bNeedPostProcessing;
	CAM_Bool                bUsePrivateBuffer;

	// buffer management for embeded post-processing unit
	CAM_ImageBuffer         stPpuSrcImg[CAM_MAX_PORT_BUF_CNT];    // the array index is used to fill the CAM_ImageBuffer::iUserIndex
	CAM_Bool                bPpuSrcImgUsed[CAM_MAX_PORT_BUF_CNT]; // whether the position on corresponding index is using 
	CAM_Int32s              iPpuSrcImgAllocateCnt;                // indicate how many image buffers in stPpuSrcImage are allocated by camera-enigne itself

	// current camera capability 
	CAM_CameraCapability	stCamCap;
	CAM_ShotModeCapability	stShotModeCap;

	// current sensor status
	CAM_Int32s              iSensorID;
	void                    *hSensorHandle;
	CAM_ImageBufferReq      stCurrentSensorBufReq;  // updated whenever the sensor is configured

	// camera state
	CAM_CaptureState        eCaptureState;
	_CAM_PortState          stPortState[3];

	// preview state settings
	CAM_Bool                bPreviewFavorAspectRatio;
	CAM_Int32u              iTargetPreviewFpsQ16;
	CAM_Int32u              iActualPreviewFpsQ16;

	// video state settings
	CAM_Int32u              iTargetVideoFpsQ16;
	CAM_Int32u              iActualVideoFpsQ16;

	// still state settings
	CAM_JpegParam           stJpegParam;
	CAM_Bool                bIsFocusOpen;
	CAM_Int32s              iStillRestCount;    // still image capture counter

	// digital zoom related
	CAM_Int32s              iDigitalZoomQ16;
	CAM_Int32s              iPpuDigitalZoomQ16;
} _CAM_ExtIspState;

#ifdef __cplusplus
}
#endif

#endif // __CAM_ENGINE_EXTISP_H_
