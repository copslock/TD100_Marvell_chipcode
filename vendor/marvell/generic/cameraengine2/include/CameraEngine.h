/******************************************************************************
//(C) Copyright [2010 - 2011] Marvell International Ltd.
//All Rights Reserved
******************************************************************************/

#ifndef _CAM_ENGINE_H_
#define _CAM_ENGINE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Camera Engine Version Info */
#define CAM_ENGINE_VERSION_MAJOR  2
#define CAM_ENGINE_VERSION_MINOR  3

typedef char                CAM_Int8s;
typedef unsigned char       CAM_Int8u;
typedef short               CAM_Int16s;
typedef unsigned short      CAM_Int16u;
typedef int                 CAM_Int32s;
typedef unsigned int        CAM_Int32u;
typedef long long           CAM_Int64s;
typedef unsigned long long  CAM_Int64u;
typedef long long           CAM_Tick;
typedef int                 CAM_Bool;

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

#define CAM_TRUE                    (CAM_Bool)(1)
#define CAM_FALSE                   (CAM_Bool)(0)

#define CAM_STATISTIC_DISABLE       ((void*)(0xffffffff))

#define CAM_PORT_NONE               (-1)
#define CAM_PORT_PREVIEW            (0)
#define CAM_PORT_VIDEO              (1)
#define CAM_PORT_STILL              (2)
#define CAM_PORT_SNAPSHOT           (3)
#define CAM_PORT_ANY                (-2)

#define CAM_MAX_PORT_BUF_CNT             20
#define CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT 10
#define CAM_MAX_SUPPORT_IMAGE_SIZE_CNT   10

typedef enum 
{
	CAM_CAPTURESTATE_NULL            = 1,	// camera engine handle is created but no sensor is connected
	CAM_CAPTURESTATE_STANDBY         = 2,	// reserved for low power state
	CAM_CAPTURESTATE_IDLE            = 4,	// sensor is connected
	CAM_CAPTURESTATE_PREVIEW         = 8,
	CAM_CAPTURESTATE_VIDEO           = 16,
	CAM_CAPTURESTATE_STILL           = 32,
	CAM_CAPTURESTATE_ANY             = 0x3f,

	CAM_CAPTURESTATE_LIMIT	         = 0x7fffffff,
} CAM_CaptureState;


typedef struct 
{
	CAM_Int32s  iWidth;
	CAM_Int32s  iHeight;
} CAM_Size;

typedef struct 
{
	CAM_Int32s  iLeft;              // left boundary, the boundary itself is included in the rectangle
	CAM_Int32s  iTop;               // top boundary, the boundary itself is included in the rectangle
	CAM_Int32s  iWidth;             // width of the rect
	CAM_Int32s  iHeight;            // height of the rect
} CAM_Rect;

typedef struct 
{
	CAM_Rect    stRect;
	CAM_Int32s  iWeight;
}CAM_WeightedRect;

#define MAX_ROI_CNT   5
typedef struct 
{
	CAM_Int32s       iROICnt;
	CAM_WeightedRect stWeiRect[MAX_ROI_CNT];
}CAM_MultiROI;


typedef enum 
{
	CAM_ERROR_NONE                 = 0,

	/* warnings */
	CAM_WARN_STARTCODE             = 1000, // start code of warnings, only a indicator 
	
	CAM_WARN_DUPLICATEBUFFER       = 1001, // enqueue a same buffer

	/* errors */
	// user input error
	CAM_ERROR_STARTCODE            = 3000, // start code of errors, only a indicator 

	CAM_ERROR_BADPOINTER           = 3001, // the input pointer is bad( NULL )

	CAM_ERROR_BADARGUMENT          = 3002, // the input argument is bad

	CAM_ERROR_BADSENSORID          = 3003, // the input sensor id is invalid

	CAM_ERROR_BADPORTID            = 3004, // the input port id is bad

	CAM_ERROR_PORTNOTVALID         = 3005, // the port's parameters are not valid / consistent

	CAM_ERROR_PORTNOTCONFIGUREABLE = 3006, // the port is not configurable (due to not inactive or reside with buffers)

	CAM_ERROR_BADBUFFER            = 3007, // the buffer doesn't meet the buffer requirement of the port

	CAM_ERROR_BADSTATETRANSITION   = 3008, // the requested state transition is not allowed

	CAM_ERROR_STATELIMIT           = 3009, // operation not allowed due to the limitation of the current camera state

	CAM_ERROR_SHOTMODELIMIT        = 3010, // operation not allowed due to the limitation of the current photo mode

	// implementation limitation
	CAM_ERROR_NOTSUPPORTEDCMD      = 3011, // the command is not supported

	CAM_ERROR_NOTSUPPORTEDARG      = 3012, // the argument is not supported

	CAM_ERROR_NOTIMPLEMENTED       = 3013, // the feature is planned to be supported, but not yet implemented

	// execution error
	CAM_ERROR_NOTENOUGHBUFFERS     = 3014, // no enough number of buffers has been enqueued

	CAM_ERROR_BUFFERPREEMPTED      = 3015, // dequeue can't return a valid buffer because there's another dequeue in "FRAME_READY" event handler, 
	                                       // and it always preempt the buffer before first dequeue

	CAM_ERROR_PORTNOTACTIVE        = 3016, // dequeue from an inactive port

	CAM_ERROR_OUTOFMEMORY          = 3017, // memory allocation failed

	CAM_ERROR_OUTOFRESOURCE        = 3018, // usually caused by too much eneuque

	CAM_ERROR_TIMEOUT              = 3019, // usually happened in dequeue

	CAM_ERROR_DRIVEROPFAILED       = 3020, // open/close/ioctl return error

	CAM_ERROR_PPUFAILED            = 3021, // post-processing failed

	CAM_ERROR_FATALERROR           = 3022, // when fatal happened, camera engine must be set back to idle to clear this error. 
	                                       // Fatal error may be caused by an unrecoverable device failure.

	CAM_ERROR_LIMIT                = 0x7fffffff, 

} CAM_Error;

typedef enum
{
	CAM_IMGFMT_RGGB8      = 1000,
	CAM_IMGFMT_BGGR8      = 1001,
	CAM_IMGFMT_GRBG8      = 1002,
	CAM_IMGFMT_GBRG8      = 1003,
	CAM_IMGFMT_RGGB10     = 1004,
	CAM_IMGFMT_BGGR10     = 1005,
	CAM_IMGFMT_GRBG10     = 1006,
	CAM_IMGFMT_GBRG10     = 1007,

	// low->high  R,G,B
	CAM_IMGFMT_RGB888     = 2000,
	CAM_IMGFMT_RGB444     = 2001,
	CAM_IMGFMT_RGB555     = 2002,
	CAM_IMGFMT_RGB565     = 2003,
	CAM_IMGFMT_RGB666     = 2004,
	CAM_IMGFMT_BGR888     = 2005,
	CAM_IMGFMT_BGR444     = 2006,
	CAM_IMGFMT_BGR555     = 2007,
	CAM_IMGFMT_BGR565     = 2008,
	CAM_IMGFMT_BGR666     = 2009,
	// with alpha channel
	CAM_IMGFMT_BGRA8888   = 2010,
	CAM_IMGFMT_RGBA8888   = 2011,

	CAM_IMGFMT_YCbCr444P  = 3000,
	CAM_IMGFMT_YCbCr444I  = 3001,
	CAM_IMGFMT_YCbCr422P  = 3002,
	CAM_IMGFMT_YCbYCr     = 3003,
	CAM_IMGFMT_CbYCrY     = 3004,
	CAM_IMGFMT_YCrYCb     = 3005,
	CAM_IMGFMT_CrYCbY     = 3006,
	CAM_IMGFMT_YCrCb420P  = 3007, // fourcc: YV12
	CAM_IMGFMT_YCbCr420P  = 3008, // fourcc: IYUV, I420
	CAM_IMGFMT_YCbCr420SP = 3009, // fourcc: NV12
	CAM_IMGFMT_YCrCb420SP = 3010, // fourcc: NV21

	CAM_IMGFMT_JPEG       = 4000,

	CAM_IMGFMT_LIMIT      = 0x7fffffff,
} CAM_ImageFormat;

// flip/rotate
typedef enum 
{
	CAM_FLIPROTATE_NORMAL  = 0,
	CAM_FLIPROTATE_90L,
	CAM_FLIPROTATE_90R,
	CAM_FLIPROTATE_180,
	CAM_FLIPROTATE_HFLIP,        // horizontal flip
	CAM_FLIPROTATE_VFLIP,        // vertical flip
	CAM_FLIPROTATE_DFLIP,        // diagonal flip
	CAM_FLIPROTATE_ADFLIP,       // anti-diagonal flip

	CAM_FLIPROTATE_NUM,

	CAM_FLIPROTATE_LIMIT  = 0x7fffffff,
} CAM_FlipRotate;

// shot mode
typedef enum 
{
	CAM_SHOTMODE_AUTO          = 0,
	CAM_SHOTMODE_MANUAL,

	CAM_SHOTMODE_PORTRAIT,
	CAM_SHOTMODE_LANDSCAPE,
	CAM_SHOTMODE_NIGHTPORTRAIT,
	CAM_SHOTMODE_NIGHTSCENE,
	CAM_SHOTMODE_CHILD,
	CAM_SHOTMODE_INDOOR,
	CAM_SHOTMODE_PLANTS,
	CAM_SHOTMODE_SNOW,
	CAM_SHOTMODE_BEACH,
	CAM_SHOTMODE_FIREWORKS,
	CAM_SHOTMODE_SUBMARINE,
	CAM_SHOTMODE_WHITEBOARD,
	CAM_SHOTMODE_SPORTS,

	CAM_SHOTMODE_NUM,

	CAM_SHOTMODE_LIMIT         = 0x7fffffff, 
} CAM_ShotMode;

// white balance mode
typedef enum 
{
	CAM_WHITEBALANCEMODE_AUTO                  = 0,
	CAM_WHITEBALANCEMODE_INCANDESCENT          = 1,
	CAM_WHITEBALANCEMODE_DAYLIGHT_FLUORESCENT  = 2,
	CAM_WHITEBALANCEMODE_DAYWHITE_FLUORESCENT  = 3,
	CAM_WHITEBALANCEMODE_COOLWHITE_FLUORESCENT = 4,
	CAM_WHITEBALANCEMODE_DAYLIGHT              = 5,
	CAM_WHITEBALANCEMODE_CLOUDY                = 6,
	CAM_WHITEBALANCEMODE_SHADOW                = 7,

	CAM_WHITEBALANCEMODE_NUM,

	CAM_WHITEBALANCEMODE_LIMIT                 = 0x7fffffff,
} CAM_WhiteBalanceMode;

// color effect
typedef enum 
{
	CAM_COLOREFFECT_OFF        = 0,
	CAM_COLOREFFECT_VIVID,
	CAM_COLOREFFECT_SEPIA,
	CAM_COLOREFFECT_GRAYSCALE,
	CAM_COLOREFFECT_NEGATIVE,
	CAM_COLOREFFECT_SOLARIZE, 
	CAM_COLOREFFECT_POSTERIZE,
	CAM_COLOREFFECT_AQUA,

	CAM_COLOREFFECT_NUM,

	CAM_COLOREFFECT_LIMIT      = 0x7fffffff,
} CAM_ColorEffect;

// exposure mode
typedef enum
{
	CAM_EXPOSUREMODE_AUTO              = 0,
	CAM_EXPOSUREMODE_APERTUREPRIOR,
	CAM_EXPOSUREMODE_SHUTTERPRIOR,
	CAM_EXPOSUREMODE_PROGRAM,
	CAM_EXPOSUREMODE_MANUAL,

	CAM_EXPOSUREMODE_NUM,

	CAM_EXPOSUREMODE_LIMIT             = 0x7fffffff,
}CAM_ExposureMode;

// exposure metering mode
typedef enum 
{
	CAM_EXPOSUREMETERMODE_AUTO            = 0,
	CAM_EXPOSUREMETERMODE_MEAN,
	CAM_EXPOSUREMETERMODE_CENTRALWEIGHTED,
	CAM_EXPOSUREMETERMODE_SPOT,
	CAM_EXPOSUREMETERMODE_MATRIX,

	CAM_EXPOSUREMETERMODE_NUM,

	CAM_EXPOSUREMETERMODE_LIMIT           = 0x7fffffff,
} CAM_ExposureMeterMode;

// ISO speed
typedef enum 
{
	CAM_ISO_AUTO  = 0,
	CAM_ISO_50,
	CAM_ISO_100,
	CAM_ISO_200,
	CAM_ISO_400,
	CAM_ISO_800,
	CAM_ISO_1600,
	CAM_ISO_3200,

	CAM_ISO_NUM,

	CAM_ISO_LIMIT = 0x7fffffff,
} CAM_ISOMode;

// shutter type
typedef enum 
{
	CAM_SHUTTERTYPE_ROLLING,
	CAM_SHUTTERTYPE_MECHANICAL,

	CAM_SHUTTERTYPE_NUM,

	CAM_SHUTTERTYPE_LIMIT       = 0x7fffffff,
} CAM_ShutterType;

// only electronic rolling shutter needs anti-flicker
typedef enum 
{
	CAM_BANDFILTER_AUTO  = 0,
	CAM_BANDFILTER_OFF,
	CAM_BANDFILTER_50HZ,
	CAM_BANDFILTER_60HZ,

	CAM_BANDFILTER_NUM,

	CAM_BANDFILTER_LIMIT = 0x7fffffff,
} CAM_BandFilterMode;

// actuator type
typedef enum 
{
	CAM_ACTUATOR_FIXED,
	CAM_ACTUATOR_VCM,
	CAM_ACTUATOR_PIEZO,
	CAM_ACTUATOR_LIQUID,
	CAM_ACTUATOR_EDOF,

	CAM_ACTUATOR_NUM,

	CAM_ACTUATOR_LIMIT = 0x7fffffff,
} CAM_ActuatorType;

// focus mode
typedef enum 
{
	CAM_FOCUS_AUTO_ONESHOT_CENTER    = 0x0,  // need CAM_CMD_START_FOCUS trigger focus
	CAM_FOCUS_AUTO_ONESHOT_TOUCH     = 0x1,  // need CAM_CMD_START_FOCUS trigger focus
	CAM_FOCUS_AUTO_CONTINUOUS_CENTER = 0x2,
	CAM_FOCUS_AUTO_CONTINUOUS_FACE   = 0x3,
	CAM_FOCUS_SUPERMACRO             = 0x4,
	CAM_FOCUS_MACRO                  = 0x5,  // need CAM_CMD_START_FOCUS trigger focus
	CAM_FOCUS_HYPERFOCAL             = 0x6,
	CAM_FOCUS_INFINITY               = 0x7,
	CAM_FOCUS_MANUAL                 = 0x8,

	CAM_FOCUS_NUM,

	CAM_FOCUS_LIMIT                  = 0x7fffffff,
} CAM_FocusMode;

// focus zone
typedef enum 
{ 
	CAM_FOCUSZONE_CENTER     = 0x0,
	CAM_FOCUSZONE_MULTIZONE1 = 0x1,
	CAM_FOCUSZONE_MULTIZONE2 = 0x2,
	CAM_FOCUSZONE_MULTIZONE3 = 0x3,
	CAM_FOCUSZONE_MANUAL     = 0x4,

	CAM_FOCUSZONE_NUM,

	CAM_FOCUSZONE_LIMIT      = 0x7fffffff,
} CAM_FocusZone;

// flash type
typedef enum 
{
	CAM_FLASHTYPE_NONE  = 0x0,
	CAM_FLASHTYPE_LED   = 0x1,
	CAM_FLASHTYPE_XENON = 0x2,

	CAM_FLASHTYPE_NUM,

	CAM_FLASHTYPE_LIMIT = 0x7fffffff,
} CAM_FlashType;

// flash mode
typedef enum 
{
	CAM_FLASH_AUTO  = 0x0,
	CAM_FLASH_OFF   = 0x1,
	CAM_FLASH_ON    = 0x2,
	CAM_FLASH_TORCH = 0x3,

	CAM_FLASH_NUM,

	CAM_FLASH_LIMIT = 0x7fffffff,
} CAM_FlashMode;


// Events
typedef enum 
{
	CAM_EVENT_FRAME_DROPPED        = 0x0,	// param - port id

	CAM_EVENT_FRAME_READY          = 0x1,	// param - port id

	CAM_EVENT_IN_FOCUS             = 0x2,

	CAM_EVENT_OUTOF_FOCUS          = 0x3,

	CAM_EVENT_FOCUS_AUTO_STOP      = 0x4,	// param - pointer to focus status (CAM_Bool), which indicate if the auto focus succeeded

	CAM_EVENT_STILL_SHUTTERCLOSED  = 0x5,

	CAM_EVENT_STILL_ALLPROCESSED   = 0x6,

	CAM_EVENT_FACE_UPDATE          = 0x7,

	CAM_EVENT_FATALERROR           = 0x8,

	CAM_EVENT_NUM,

	CAM_EVENT_LIMIT                = 0x7fffffff,
} CAM_EventId;

// JPEG Parameter
typedef struct 
{
	CAM_Int32s iSampling;		// 0 - 420, 1 - 422, 2 - 444
	CAM_Int32s iQualityFactor;
	CAM_Bool   bEnableExif;
	CAM_Bool   bEnableThumb;
	CAM_Int32s iThumbWidth;
	CAM_Int32s iThumbHeight;
} CAM_JpegParam;

#define BUF_FLAG_PHYSICALCONTIGUOUS             0x01
#define BUF_FLAG_NONPHYSICALCONTIGUOUS	        0x02
#define BUF_FLAG_L1CACHEABLE                    0x04
#define BUF_FLAG_L1NONCACHEABLE                 0x08
#define BUF_FLAG_L2CACHEABLE                    0x10
#define BUF_FLAG_L2NONCACHEABLE	                0x20
#define BUF_FLAG_BUFFERABLE                     0x40
#define BUF_FLAG_UNBUFFERABLE                   0x80

#define BUF_FLAG_YUVPLANER                      0x100
#define BUF_FLAG_YUVBACKTOBACK                  0x200
#define BUF_FLAG_YVUBACKTOBACK                  0x400

#define BUF_FLAG_ALLOWPADDING                   0x800
#define BUF_FLAG_FORBIDPADDING                  0x1000

#define BUF_FLAG_ALL                            0x1fff

// shooting informations of frame, currently for EXIF 
typedef struct
{
	CAM_Int32s              iExposureTimeQ16;   // uint: s
	CAM_Int32s              iFNumQ16;
	CAM_ExposureMode        eExposureMode;
	CAM_ExposureMeterMode   eExpMeterMode;
	CAM_Int32s              iISOSpeed;
	CAM_Int32s              iSubjectDistQ16;   // unit: m
	CAM_Int32s              iFlashStatus;
	CAM_Int32s              iFocalLenQ16;
	CAM_Int32s              iFocalLen35mm; 
}CAM_ShotInfo;

// image buffer header
typedef struct
{
	CAM_ImageFormat         eFormat;
	CAM_Int32s              iWidth;
	CAM_Int32s              iHeight;
	CAM_Int32s              iStep[3];       // Given by external before enqueue
	CAM_Int32s              iAllocLen[3];   // Given by external before enqueue
	CAM_Int32s              iFilledLen[3];
	CAM_Int32s              iOffset[3];     // Given by external before enqueue
	CAM_Int8u               *pBuffer[3];    // Given by external before enqueue, virtual address
	CAM_Int32u              iPhyAddr[3];    // Given by external before enqueue, physical address of pBuffer[]
	CAM_Tick                iTick;
	CAM_Int32u              iFlag;          // Given by external before enqueue
	CAM_Int32s              iUserIndex;     // Given by external before enqueue
	void                    *pUserData;     // Given by external before enqueue
	CAM_Int32s              iPrivateIndex;
	void                    *pPrivateData;  

	CAM_Bool                bEnableShotInfo;
	CAM_ShotInfo            stShotInfo;
} CAM_ImageBuffer;

// buffer requirement
typedef struct 
{
	CAM_ImageFormat         eFormat;
	CAM_Int32s              iWidth;         // image width in pixels
	CAM_Int32s              iHeight;        // image height in pixels
	CAM_Int32s              iMinStep[3];    // image buffer's row stride in bytes
	CAM_Int32s              iMinBufLen[3];
	CAM_Int32s              iMinBufCount;
	CAM_Int32s              iAlignment[3];  // alignment of each buffer start address
	CAM_Int32s              iRowAlign[3];   // alignment of each row

	CAM_Int32u              iFlagOptimal;
	CAM_Int32u              iFlagAcceptable;
} CAM_ImageBufferReq;


// JPEG Encoder Capability
typedef struct 
{
	CAM_Bool   bSupportJpeg;
	CAM_Int32s iMinQualityFactor;
	CAM_Int32s iMaxQualityFactor;
	CAM_Bool   bSupportExif;
	CAM_Int8s  sExifVersion[8];
	CAM_Bool   bSupportThumb;
	CAM_Size   stMinThumbSize;
	CAM_Size   stMaxThumbSize;
} CAM_JpegCapability;

// port capability
typedef struct 
{
	CAM_Bool        bArbitrarySize;
	CAM_Size        stMin;
	CAM_Size        stMax;

	CAM_Int32s      iSupportedSizeCnt;
	CAM_Size        stSupportedSize[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT];

	CAM_Int32s      iSupportedFormatCnt;
	CAM_ImageFormat eSupportedFormat[CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT];

	CAM_Int32s      iSupportedRotateCnt;
	CAM_FlipRotate  eSupportedRotate[CAM_FLIPROTATE_NUM];
} CAM_PortCapability;

// camera capability
typedef struct 
{
	CAM_Int32s             iSupportedExpModeCnt;
	CAM_ExposureMode       eSupportedExpMode[CAM_EXPOSUREMODE_NUM];

	CAM_Int32s             iSupportedExpMeterCnt;
	CAM_ExposureMeterMode  eSupportedExpMeter[CAM_EXPOSUREMETERMODE_NUM];

	CAM_Int32s             iMinEVCompQ16;
	CAM_Int32s             iMaxEVCompQ16;
	CAM_Int32s             iEVCompStepQ16;	// if support continuous ev compensation, pls set "iEVCompStepQ16" to zero

	CAM_Int32s             iSupportedIsoModeCnt;
	CAM_ISOMode            eSupportedIsoMode[CAM_ISO_NUM];

	CAM_Int32s             iMinShutSpdQ16;
	CAM_Int32s             iMaxShutSpdQ16;

	CAM_Int32s             iMinFNumQ16;
	CAM_Int32s             iMaxFNumQ16;

	CAM_Int32s             iSupportedBdFltModeCnt;
	CAM_BandFilterMode     eSupportedBdFltMode[CAM_BANDFILTER_NUM];

	CAM_Int32s             iSupportedFlashModeCnt;
	CAM_FlashMode          eSupportedFlashMode[CAM_FLASH_NUM];

	CAM_Int32s             iSupportedWBModeCnt;
	CAM_WhiteBalanceMode   eSupportedWBMode[CAM_WHITEBALANCEMODE_NUM];

	CAM_Int32s             iSupportedFocusModeCnt;
	CAM_FocusMode          eSupportedFocusMode[CAM_FOCUS_NUM];

	CAM_Int32s             iMinOptZoomQ16;
	CAM_Int32s             iMaxOptZoomQ16;

	CAM_Int32s             iMinDigZoomQ16;
	CAM_Int32s             iMaxDigZoomQ16;
	CAM_Int32s             iDigZoomStepQ16;
	CAM_Int32s             bSupportSmoothDigZoom;

	CAM_Int32s             iMinFpsQ16;
	CAM_Int32s             iMaxFpsQ16;

	CAM_Int32s             iSupportedColorEffectCnt;
	CAM_ColorEffect        eSupportedColorEffect[CAM_COLOREFFECT_NUM];

	CAM_Int32s             iMinBrightness;
	CAM_Int32s             iMaxBrightness;

	CAM_Int32s             iMinContrast;
	CAM_Int32s             iMaxContrast;

	CAM_Int32s             iMinSaturation;
	CAM_Int32s             iMaxSaturation;

	CAM_Int32s             iMinSharpness;
	CAM_Int32s             iMaxSharpness;

	CAM_Bool               bSupportVideoStabilizer;
	CAM_Bool               bSupportVideoNoiseReducer;
	CAM_Bool               bSupportZeroShutterLag;
	CAM_Bool               bSupportHighDynamicRange;

	CAM_Bool               bSupportBurstCapture;
	CAM_Int32s             iMaxBurstCnt;

} CAM_ShotModeCapability;

typedef struct 
{
	CAM_PortCapability     stPortCapability[3];

	CAM_Int32s             iSupportedShotModeCnt;
	CAM_ShotMode           eSupportedShotMode[CAM_SHOTMODE_NUM];

	CAM_Int32s             iSupportedSensorRotateCnt;
	CAM_FlipRotate         eSupportedSensorRotate[CAM_FLIPROTATE_NUM];

	CAM_ShotModeCapability stSupportedShotParams;

	CAM_JpegCapability     stSupportedJPEGParams;

} CAM_CameraCapability;

// Event handler table
typedef void (*CAM_EventHandler)( CAM_EventId eEventId, void *pParam, void *pUserData );


/*------------------------------------------------------------------------------
--  Commands for Camera Engine (the application adaptation layer)
------------------------------------------------------------------------------*/

typedef enum 
{
	////////////////////////////////////////////////////////////////////////////////
	//  Command                                 Parameter
	//                                          Comments
	////////////////////////////////////////////////////////////////////////////////
	// Used for intialization
	CAM_CMD_ENUM_SENSORS,                // [OUT] int *pCameraCount
	                                     // [OUT] char strCameraName[4][32]

	CAM_CMD_QUERY_CAMERA_CAP,            // [IN] int iSensorID
	                                     // [OUT] CAM_CameraCapability *pCapability

	CAM_CMD_SET_EVENTHANDLER,            // [IN] CAM_EventHandler fnEventHandler
	                                     // [OUT] void *pUserData

	CAM_CMD_SET_SENSOR_ID,               // [IN] int iSensorID
	                                     // [UNUSED]

	CAM_CMD_GET_SENSOR_ID,               // [OUT] int *pSensorID
	                                     // [UNUSED]

	CAM_CMD_SET_SENSOR_ORIENTATION,      // [IN] CAM_FlipRotate eFlipRotate
	                                     // [UNUSED]

	CAM_CMD_GET_SENSOR_ORIENTATION,      // [OUT] CAM_FlipRotate *pFlipRotate
	                                     // [UNUSED]

 	// State Machine Management Commands
	CAM_CMD_SET_STATE,                   // [IN] CAM_CaptureState eState
	                                     // [UNUSED]

	CAM_CMD_GET_STATE,                   // [OUT] CAM_CaptureState *pState
	                                     // [UNUSED]

 	// Buffer Management Commands
	CAM_CMD_PORT_GET_BUFREQ,             // [OUT CAM_ImageBufferReq *pBufReq
	                                     // [UNUSED]
	                                     // get buffer requirement for streaming

	CAM_CMD_PORT_ENQUEUE_BUF,            // [IN] int iPort
	                                     // [IN] CAM_ImageBuffer *pBuf
	                                     // enqueue to empty buffer queue.
	                                     // and implicitly register the buffer if
	                                     // the buffer is enqueued for the first time

	CAM_CMD_PORT_DEQUEUE_BUF,           // [IN/OUT] int *pPortId
	                                    // [OUT] CAM_ImageBuffer **ppBuf
	                                    // dequeue from filled buffer queue.
	                                    // Note that this is a blocking call.

	CAM_CMD_PORT_FLUSH_BUF,             // [IN] int iPortId
	                                    // [IN] CAM_ImageBuffer *pBuf
	                                    // release currently holded buffers and
	                                    // implicitly unregister all buffers that has
	                                    // ever been enqueued to the port

	// Data Port Commands
	CAM_CMD_PORT_SET_SIZE,              // [IN] int iPortId
	                                    // [IN] CAM_Size *sPreviewSize

	CAM_CMD_PORT_GET_SIZE,              // [IN] int iPortId
	                                    // [OUT] CAM_Size *sPreviewSize

	CAM_CMD_PORT_SET_FORMAT,            // [IN] int iPortId
	                                    // [IN] CAM_ImageFormat eFormat

	CAM_CMD_PORT_GET_FORMAT,            // [IN] int iPortId
	                                    // [OUT] CAM_ImageFormat *pFormat
	
	CAM_CMD_PORT_SET_ROTATION,          // [IN] int iPortId
	                                    // [IN] CAM_FlipRotate eFlipRotate

	CAM_CMD_PORT_GET_ROTATION,          // [IN] int iPortId
	                                    // [OUT] CAM_FlipRotate *pFlipRotate

	// Preview Features
	CAM_CMD_SET_PREVIEW_FRAMERATE,      // [IN] int iFPSQ16
	                                    // [UNUSED]
						
	CAM_CMD_GET_PREVIEW_FRAMERATE,      // [IN] int *pFPSQ16
	                                    // [UNUSED]

	CAM_CMD_SET_PREVIEW_RESIZEFAVOR,    // [IN] CAM_Bool bAspectRatioOverFOV
	                               	    // [UNUSED]

	CAM_CMD_GET_PREVIEW_RESIZEFAVOR,    // [IN] CAM_Bool *pAspectRatioOverFOV
	                                    // [UNUSED]

	// Video Recording Features
	CAM_CMD_SET_VIDEO_FRAMERATE,        // [IN] int iFPSQ16
	                                    // [UNUSED]

	CAM_CMD_GET_VIDEO_FRAMERATE,        // [IN] int *pFPSQ16
	                                    // [UNUSED]

	CAM_CMD_SET_VIDEO_STABLIZER,        // [IN] CAM_Bool bEnable
	                                    // [UNUSED]

	CAM_CMD_GET_VIDEO_STABLIZER,        // [OUT] CAM_Bool *pEnable
	                                    // [UNUSED]

	CAM_CMD_SET_VIDEO_NR,               // [IN] CAM_Bool bEnable
	                                    // [UNUSED]

	CAM_CMD_GET_VIDEO_NR,               // [OUT] CAM_Bool *pEnable
	                                    // [UNUSED]

	// Still Capture Features
	CAM_CMD_SET_JPEGPARAM,              // [IN] CAM_JpegParam jpegparam
	                                    // [UNUSED]

	CAM_CMD_GET_JPEGPARAM,              // [OUT] CAM_JpegParam *pJpegParam
	                                    // [UNUSED]

	CAM_CMD_SET_STILL_ZSL,              // [IN] int iFrameOffset
	                                    // [UNUSED]

	CAM_CMD_GET_STILL_ZSL,              // [OUT] int *pFrameOffset
	                                    // [UNUSED]

	CAM_CMD_SET_STILL_BURST,            // [IN] int iBurstCount
	                                    // [UNUSED]

	CAM_CMD_GET_STILL_BURST,            // [OUT] int *pBurstCount
	                                    // [UNUSED]

	CAM_CMD_SET_STILL_HDR,              // [IN]  CAM_Bool bEnable
	                                    // [UNUSED]

	CAM_CMD_GET_STILL_HDR,              // [OUT] CAM_Bool *pEnable
	                                    // [UNUSED]

	// Shooting Parameter Commands
	// photo mode
	CAM_CMD_QUERY_SHOTMODE_CAP,         // [IN] CAM_ShotMode eShotMode
	                                    // [OUT] CAM_ShotModeCapability *pCapability

	CAM_CMD_SET_SHOTMODE,               // [IN] CAM_ShotMode eShotMode
	                                    // [UNUSED]

	CAM_CMD_GET_SHOTMODE,               // [OUT] CAM_ShotMode *pShotMode
	                                    // [UNUSED]

	// exposure (usually used in still capture related photo mode)
	CAM_CMD_SET_EXPOSUREMETERMODE,      // [IN] CAM_ExposureMeterMode eExposureMeter
	                                    // [UNUSED]

	CAM_CMD_GET_EXPOSUREMETERMODE,      // [OUT] CAM_ExposureMeterMode *pExposureMeter
	                                    // [UNUSED]

	CAM_CMD_SET_EVCOMPENSATION,         // [IN] CAM_Int32s iEvCompensationQ16
	                                    // [UNUSED]

	CAM_CMD_GET_EVCOMPENSATION,         // [OUT] CAM_Int32s *pEvCompensationQ16
	                                    // [UNUSED]

	CAM_CMD_SET_ISO,                    // [IN] CAM_ISOMode eISO
	                                    // [UNUSED]

	CAM_CMD_GET_ISO,                    // [OUT] CAM_ISOMode *pISO
	                                    // [UNUSED]

	CAM_CMD_SET_SHUTTERSPEED,           // [IN] int iShutterSpeedQ16
	                                    // [UNUSED]

	CAM_CMD_GET_SHUTTERSPEED,           // [OUT] int *pShutterSpeedQ16
	                                    // [UNUSED]

	CAM_CMD_SET_FNUM,                   // [IN] int iFNumQ16
	                                    // [UNUSED]

	CAM_CMD_GET_FNUM,                   // [OUT] int *pFNumQ16
	                                    // [UNUSED]

	CAM_CMD_SET_BANDFILTER,             // [IN] CAM_BandFilterMode eBandFilter
	                                    // [UNUSED]

	CAM_CMD_GET_BANDFILTER,             // [OUT] CAM_BandFilterMode *pBandFilter
	                                    // [UNUSED]

	CAM_CMD_SET_FLASHMODE,              // [IN] CAM_FlashMode eFlashMode
	                                    // [UNUSED]

	CAM_CMD_GET_FLASHMODE,              // [OUT] CAM_FlashMode *pFlashMode
	                                    // [UNUSED]

	// white balance
	CAM_CMD_SET_WHITEBALANCEMODE,       // [IN] CAM_WhiteBalanceMode eWhiteBalance
	                                    // [UNUSED]

	CAM_CMD_GET_WHITEBALANCEMODE,       // [OUT] CAM_WhiteBalanceMode *pWhiteBalance
	                                    // [UNUSED]

	// focus
	CAM_CMD_SET_FOCUSMODE,              // [IN] CAM_FocusMode eFocusMode
	                                    // [UNUSED]

	CAM_CMD_GET_FOCUSMODE,              // [OUT] CAM_FocusMode *pFocusMode
	                                    // [UNUSED]

	CAM_CMD_START_FOCUS,                // [IN]  CAM_MultiROI *pFocusROI

	CAM_CMD_CANCEL_FOCUS,               // [UNUSED]
	                                    // [UNUSED]

	// zoom
	CAM_CMD_SET_OPTZOOM,                // [IN] int iZoomFactorQ16
	                                    // [UNUSED]

	CAM_CMD_GET_OPTZOOM,                // [IN] int *pZoomFactorQ16
	                                    // [UNUSED]

	CAM_CMD_SET_DIGZOOM,                // [IN] int iZoomFactorQ16
	                                    // [UNUSED]

	CAM_CMD_GET_DIGZOOM,                // [IN] int *pZoomFactorQ16
	                                    // [UNUSED]

	// special effect
	CAM_CMD_SET_COLOREFFECT,            // [IN] CAM_ColorEffect eColorEffect
	                                    // [UNUSED]

	CAM_CMD_GET_COLOREFFECT,            // [OUT] CAM_ColorEffect *pColorEffect
	                                    // [UNUSED]

	// tone effect
	CAM_CMD_SET_BRIGHTNESS,             // [IN] int iBrightness
	                                    // [UNUSED]

	CAM_CMD_GET_BRIGHTNESS,             // [OUT] int *pBrightness
	                                    // [UNUSED]

	CAM_CMD_SET_CONTRAST,               // [IN] int iContrast
	                                    // [UNUSED]

	CAM_CMD_GET_CONTRAST,               // [OUT] int *pContrast
	                                    // [UNUSED]

 	CAM_CMD_SET_SATURATION,             // [IN] int iSaturation
	                                    // [UNUSED]

	CAM_CMD_GET_SATURATION,             // [OUT] int *pSaturation
	                                    // [UNUSED]

	CAM_CMD_SET_SHARPNESS,              // [IN] int iSharpness
	                                    // [UNUSED]

	CAM_CMD_GET_SHARPNESS,              // [OUT] int *pSharpness
	                                    // [UNUSED]

	// robustness interface
	CAM_CMD_RESET_CAMERA,               // [IN] CAM_Int32s iResetType: CAM_RESET_FAST, CAM_RESET_COMPLETE
	                                    // [UNUSED]

	CAM_CMD_LIMIT         = 0x7fffffff,
} CAM_Command;

// camera reset type
#define CAM_RESET_FAST     1
#define CAM_RESET_COMPLETE 2

// Camera Engine entry API definitions
typedef void*   CAM_DeviceHandle;
typedef void*   CAM_Param;

#define UNUSED_PARAM	((CAM_Param)NULL)


typedef CAM_Error (*_CAM_OpenFunc)(void **ppDeviceData);

typedef CAM_Error (*_CAM_CloseFunc)(void **ppDeviceData);

typedef CAM_Error (*_CAM_CommandFunc)(void *pDeviceData, CAM_Command cmd, CAM_Param param1, CAM_Param param2);

typedef struct _CAM_DriverEntry 
{
	const char          *pName;
	_CAM_OpenFunc       pOpen;
	_CAM_CloseFunc      pClose;
	_CAM_CommandFunc    pCommand;
} CAM_DriverEntry;


CAM_Error CAM_GetHandle( const CAM_DriverEntry *pDriverEntry, CAM_DeviceHandle *pHandle );

CAM_Error CAM_FreeHandle( CAM_DeviceHandle *pHandle );

CAM_Error CAM_SendCommand( CAM_DeviceHandle handle, CAM_Command cmd, CAM_Param param1, CAM_Param param2 );


// Camera Engine Entries
extern CAM_DriverEntry entry_socisp;
extern CAM_DriverEntry entry_extisp;


// RESERVED: sensor image statstics
/*
typedef enum 
{

	//Statistic Method                      // Param
	//                                      // Stat Result
	//                                      // Comments

	// IPP statistic method
	CAM_STATISTIC_ARBWINWEIGHTEDLUM,        // CAM_ArbWinWeightedParam *pParam
	// CAM_Int16u *pLumAvg
	// weighted luminance arverge

	CAM_STATISTIC_ARBWINWEIGHTEDRGBG,       // CAM_ArbWinWeightedParam *pParam
	// CAM_Int16u rg_bg_ratio[2]
	// weighted r/g, b/g ratios

	// Smart sensor statisic method
	CAM_STATISTIC_4X4WEIGHTEDLUMAVG,        // CAM_4X4WeightedLumParam *pParam
	// CAM_Int16u *pLumAvg
	// luminance average

	// ...
	CAM_STATISTIC_FRAMERGBHISTOGRAM,        // NULL
	// CAM_Int32s histogram[3][32]
	// 32 bins histograms for R, G, B
	// channels

	CAM_STATISTIC_FRAMELUMHISTOGRAM,        // NULL
	// CAM_Int32s histogram[32]
	// 32 bins histogram for luminance,
	// luminance can be defined by
	// statistic unit

	// Smart CI statistic method
	CAM_STATISTIC_8Z64P,                    // CAM_8Z64PParam
	// CAM_8Z64PResult *pResult
	// Statistic R/G/B average and Y
	// sharpness in 8 zones. Each zone
	// is composed of patches. The whole
	// frame are evenly divided into 8x8
	// totally 64 patches. Each patch
	// can be config to belong to one
	// zone. The statistic result is
	// updated every frame.
} CAM_StatisticMethod;

// Shutter limit
typedef struct 
{
	CAM_Int32s  iResIndex;
	CAM_Int32s  iFPS;
	CAM_Int32s  iMin;
	CAM_Int32s  iMax;
	CAM_Int32s  iMaxFPSAutoDeduce;
} CAM_EShutterLimit;

// Parameter structure for arbitrary window weighted brightness
typedef struct 
{
	int iWindowNum;
	CAM_Rect *pWindows;
	CAM_Int32s *pWeights;
} CAM_ArbWinWeightedParam;

// Parameter structure for window rgb/lum sum
typedef struct 
{
	CAM_Int32s iLeft;
	CAM_Int32s iTop;
	CAM_Int32s iWidth;
	CAM_Int32s iHeight;
	CAM_Int32s iWeight[16];
} CAM_4X4WeightedLumParam;

// Parameter structure for 8Z64P statistic method
typedef struct 
{
	CAM_Int8u   iRegionIndex[64];
	//  CAM_*****   ****************;           // filter for <r/g-b/g> color space
} CAM_8Z64PParam;

// Result structure for 8Z64P statistic method
typedef struct 
{
	CAM_Int32s  iSumR[8];
	CAM_Int32s  iSumG[8];
	CAM_Int32s  iSumB[8];
	CAM_Int32s  iCount[8];
} CAM_8Z64PResult;

// Parameter structure for SET_STATISTIC_PARAMETER and GET_STATISTIC_RESULT
typedef struct 
{
	CAM_StatisticMethod eMethod;
	void                *pData;     // content of pData depend on the eMethod
} CAM_StatisticData;
*/


#ifdef __cplusplus
}
#endif

#endif  // _CAM_ENGINE_H_


