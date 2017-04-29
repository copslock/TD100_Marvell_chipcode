/*******************************************************************************
//(C) Copyright [2010 - 2011] Marvell International Ltd.
//All Rights Reserved
*******************************************************************************/
#ifndef _CAM_V4L2BASE_H_
#define _CAM_V4L2BASE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/poll.h>
#include <linux/videodev2.h>

#include "cam_extisp_sensorwrapper.h"

typedef struct 
{
	struct v4l2_buffer          stBufNode;
	CAM_ImageBuffer             *pImage;
	CAM_Int32s                  iFlag;	// 0 - dequeued, 1 - enqueued
} _V4L2Buf;

typedef CAM_Error (*SaveAeAwb)( const _CAM_SmartSensorConfig *pOldConfig, void *pUserData );
typedef CAM_Error (*RestoreAeAwb)( const _CAM_SmartSensorConfig *pNewConfig, void *pUserData );
typedef CAM_Error (*StartFlash)( void *pV4L2State );
typedef CAM_Error (*StopFlash)( void *pV4L2State );

typedef struct 
{
	CAM_Int8s      sDeviceName[32];
	CAM_Int32s     iV4L2SensorID;
}_V4L2SensorEntry;


// used to configure sensor attribute while initializing
typedef struct 
{
	// sensor attribute: name/ID
	_V4L2SensorEntry               stV4L2SensorEntry;

	// AE/AWB save/restore to shorten shutter lag callback
	SaveAeAwb                      fnSaveAeAwb;
	RestoreAeAwb                   fnRestoreAeAwb;
	void                           *pSaveRestoreUserData;

	// flash on/off callback
	StartFlash                     fnStartFlash;
	StopFlash                      fnStopFlash;
} _V4L2SensorAttribute;

typedef struct 
{
	CAM_Int32s                     iSensorFD;

	// event handler
	CAM_EventHandler               fnEventHandler;
	void                           *pUserData;

	_CAM_SmartSensorConfig	       stConfig;
	CAM_ImageBufferReq             stBufReq;
	_CAM_ShotParam                 stShotParam;
	CAM_JpegParam                  stJpegParam;
	CAM_Int32u                     iBaseFpsQ16;

	CAM_Bool                       bIsStreamOn;                      // This flag shows whether the driver has been stream on
	CAM_Int32s                     iUnstableFrameCnt;
	CAM_Int32s                     iCurrentSkipCnt;
	CAM_Bool                       bIsFlashOn;

	_V4L2Buf                       stV4L2Buf[CAM_MAX_PORT_BUF_CNT]; // the array index is used to fill the V4L2 buffer's index
	CAM_Int32s                     iV4L2BufCnt;                     // indicate the number of frames that is currently used by V4L2
	CAM_Int32s                     iV4L2QueueBufCnt;                // indicate the number of frames that is currently queued to V4L2

	// sometimes, dequeue buffers may meet driver polling failed, this may lead to hardware failure, need to inform other threads of process. 
	CAM_Bool                       bIsDriverOK;

	_V4L2SensorAttribute           stAttribute;

	// frame rate control by drop frames
#if defined( BUILD_OPTION_WORKAROUND_V4L2_FRAMERATECONTROL_BY_DROPFRAME )
	// frame rate control by drop frames
	CAM_Int32s                     iValidFrameIndex;
	CAM_Tick                       tFramesTickStart; // unit: us
#endif
} _V4L2SensorState;


typedef struct 
{
	CAM_Int16u	reg;
	CAM_Int16u	lbit;
	CAM_Int16u	hbit;
	CAM_Int16u	val_lbit;
	CAM_Int16u	val;
} _CAM_RegEntry;

typedef struct 
{
	CAM_Int32s    iLength;
	_CAM_RegEntry *pEntries;
} _CAM_RegEntryTable;

typedef CAM_Error (*SmartSensorSetParameter)(CAM_Int32s *pParameter);

typedef union 
{
	_CAM_RegEntryTable      stRegTable;
	SmartSensorSetParameter	fnSet;
} _CAM_SetParam;

typedef struct 
{
	CAM_Int32s      iParameter;
	CAM_Bool        bUseRegTable;
	_CAM_SetParam	stSetParam;
} _CAM_ParameterEntry;


#define PARAM_ENTRY_USE_REGTABLE(param, regtable)\
	{\
		.iParameter                              = param,\
		.bUseRegTable                            = CAM_TRUE,\
		.stSetParam                                = {\
			.stRegTable                            = {_ARRAY_SIZE(regtable), regtable},\
		                                             },\
	}

#define PARAM_ENTRY_USE_FUNCTION(param, func)\
	{\
		.iParameter                              = param,\
		.bUseRegTable                            = CAM_FALSE,\
		.stSetParam                              = {\
			.fnSet                                   = func,\
		                                           },\
	}

CAM_Error V4L2_SelfDetect( _SmartSensorAttri *pSensorInfo, CAM_Int8s *sSensorName, _SmartSensorFunc *pCallFunc,
                           CAM_Size stVideoResTable[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT], CAM_Size stStillResTable[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT],
                           CAM_ImageFormat eVideoFormatTable[CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT], CAM_ImageFormat eStillFormatTable[CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT] );
CAM_Error V4L2_Init( _V4L2SensorState *pSensorState, _V4L2SensorAttribute *pSensorAttri, CAM_Int32s iUnstableFrameCnt );
CAM_Error V4L2_Deinit( _V4L2SensorState *pSensorState );
CAM_Error V4L2_RegisterEventHandler( _V4L2SensorState *pSensorState, CAM_EventHandler fnEventHandler, void *pUserData );
CAM_Error V4L2_Enqueue( _V4L2SensorState *pSensorState, CAM_ImageBuffer *pImgBuf );
CAM_Error V4L2_Dequeue( _V4L2SensorState *pSensorState, CAM_ImageBuffer **ppImgBuf );
CAM_Error V4L2_Flush( _V4L2SensorState *pSensorState );
CAM_Error V4L2_Config( _V4L2SensorState *pSensorState, _CAM_SmartSensorConfig *pSensorConfig );
CAM_Error V4L2_GetBufReq( _V4L2SensorState *pSensorState, _CAM_SmartSensorConfig *pSensorConfig, CAM_ImageBufferReq *pBufReq );
CAM_Error V4L2_AFDownload( _V4L2SensorState *pSensorState );



// generic helper functions
extern CAM_Int32s _get_sensor_reg( CAM_Int32s iSensorFD, CAM_Int16u reg, CAM_Int32s lbit, CAM_Int32s hbit, CAM_Int32s val_lbit, CAM_Int16u *val );
extern CAM_Int32s _set_sensor_reg( CAM_Int32s iSensorFD, CAM_Int16u reg, CAM_Int32s lbit, CAM_Int32s hbit, CAM_Int32s val_lbit, CAM_Int16u val );
extern CAM_Int32s _set_reg_array( CAM_Int32s iSensorFD, const _CAM_RegEntry *p, CAM_Int32s size );


// camera driver access wrapper 
int dbg_ioctl( int device, int cmd, void* data,
               const char *str_device, const char *str_cmd, const char *str_data,
               const char *file, int line );
int dbg_open( const char *dev, int flag, const char *str_dev, const char *str_flag, const char *file, int line );
int dbg_close( int dev, const char *str_dev, const char *file, int line );
int dbg_poll( struct pollfd *ufds, unsigned int nfds, int timeout,
              const char *str_ufds, const char *str_nfds, const char *str_timeout, const char *file, int line );
#define cam_open(a, b)			dbg_open( a, b, a, #b, __FILE__, __LINE__ )
#define cam_close(a)			dbg_close( a, #a, __FILE__, __LINE__ )
#define cam_ioctl(a, b, c)		dbg_ioctl( a, b, c, #a, #b, #c, __FILE__, __LINE__ )
#define cam_poll(a, b, c)		dbg_poll( a, b, c, #a, #b, #c, __FILE__, __LINE__ )


#ifdef __cplusplus
}
#endif

#endif  // _CAM_V4L2BASE_H_
