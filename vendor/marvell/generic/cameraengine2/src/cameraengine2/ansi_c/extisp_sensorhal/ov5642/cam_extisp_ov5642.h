/*******************************************************************************
//(C) Copyright [2010 - 2011] Marvell International Ltd.
//All Rights Reserved
*******************************************************************************/

#ifndef _CAM_OV5642_H_
#define _CAM_OV5642_H_

#ifdef __cplusplus
extern "C" {
#endif

/* shot mode default value strategy */
static _CAM_ShotParam _OV5642DefaultShotParams[3] = 
{  
  // auto mode
  { .eShotMode          = CAM_SHOTMODE_AUTO,
    .eSensorRotation    = CAM_FLIPROTATE_NORMAL,
    .eExpMode           = CAM_EXPOSUREMODE_AUTO,
    .eExpMeterMode      = CAM_EXPOSUREMETERMODE_AUTO,
    .eIsoMode           = CAM_ISO_AUTO,
    .eBandFilterMode    = CAM_BANDFILTER_50HZ,
#if defined( BUILD_OPTION_STRATEGY_ENABLE_FLASH )
    .eFlashMode         = CAM_FLASH_OFF,
#else
    .eFlashMode         = -1,
#endif
    .eWBMode            = CAM_WHITEBALANCEMODE_AUTO,
    .eFocusMode         = CAM_FOCUS_INFINITY,
    .eColorEffect       = CAM_COLOREFFECT_OFF,
    .iEvCompQ16         = 0 << 16,
    .iShutterSpeedQ16   = 0 << 16,
    .iFNumQ16           = (CAM_Int32s)(2.8 * 65536 + 0.5),
    .iDigZoomQ16        = 1 << 16,
    .iOptZoomQ16        = 1 << 16,
    .iSaturation        = 64,
    .iBrightness        = 0,
    .iContrast          = 32,
    .iSharpness         = 0,
    .bVideoStabilizer   = CAM_FALSE,
    .bVideoNoiseReducer = CAM_FALSE,
    .bZeroShutterLag    = CAM_FALSE,
    .bHighDynamicRange  = CAM_FALSE,
    .iBurstCnt          = 1,
  },
  // portrait mode
  {
    .eShotMode          = CAM_SHOTMODE_PORTRAIT,
    .eSensorRotation    = CAM_FLIPROTATE_NORMAL,
    .eExpMode           = CAM_EXPOSUREMODE_AUTO,
    .eExpMeterMode      = CAM_EXPOSUREMETERMODE_AUTO,
    .eIsoMode           = CAM_ISO_AUTO,
    .eBandFilterMode    = CAM_BANDFILTER_50HZ,
#if defined( BUILD_OPTION_STRATEGY_ENABLE_FLASH )
    .eFlashMode         = CAM_FLASH_OFF,
#else
    .eFlashMode         = -1,
#endif
    .eWBMode            = CAM_WHITEBALANCEMODE_AUTO,
    .eFocusMode         = CAM_FOCUS_AUTO_CONTINUOUS_FACE,
    .eColorEffect       = CAM_COLOREFFECT_OFF,
    .iEvCompQ16         = 0 << 16,
    .iShutterSpeedQ16   = 0 << 16,
    .iFNumQ16           = (CAM_Int32s)(2.8 * 65536 + 0.5),
    .iDigZoomQ16        = 1 << 16,
    .iOptZoomQ16        = 1 << 16,
    .iSaturation        = 64,
    .iBrightness        = 0,
    .iContrast          = 32,
    .iSharpness         = 0,
    .bVideoStabilizer   = CAM_FALSE,
    .bVideoNoiseReducer = CAM_FALSE,
    .bZeroShutterLag    = CAM_FALSE,
    .bHighDynamicRange  = CAM_FALSE,
    .iBurstCnt          = 1,
  },
  // night mode
  { .eShotMode          = CAM_SHOTMODE_NIGHTSCENE,
    .eSensorRotation    = CAM_FLIPROTATE_NORMAL,
    .eExpMode           = CAM_EXPOSUREMODE_AUTO,
    .eExpMeterMode      = CAM_EXPOSUREMETERMODE_AUTO,
    .eIsoMode           = CAM_ISO_AUTO,
    .eBandFilterMode    = CAM_BANDFILTER_50HZ,
#if defined( BUILD_OPTION_STRATEGY_ENABLE_FLASH )
    .eFlashMode         = CAM_FLASH_OFF,
#else
    .eFlashMode         = -1,
#endif
    .eWBMode            = CAM_WHITEBALANCEMODE_AUTO,
    .eFocusMode         = CAM_FOCUS_INFINITY,
    .eColorEffect       = CAM_COLOREFFECT_OFF,
    .iEvCompQ16         = 0 << 16,
    .iShutterSpeedQ16   = 0 << 16,
    .iFNumQ16           = (CAM_Int32s)(2.8 * 65536 + 0.5),
    .iDigZoomQ16        = 1 << 16,
    .iOptZoomQ16        = 1 << 16,
    .iSaturation        = 64,
    .iBrightness        = 0,
    .iContrast          = 32,
    .iSharpness         = 0,
    .bVideoStabilizer   = CAM_FALSE,
    .bVideoNoiseReducer = CAM_FALSE,
    .bZeroShutterLag    = CAM_FALSE,
    .bHighDynamicRange  = CAM_FALSE,
    .iBurstCnt          = 1,
  },
};

// for focus
#define OV5642_AFFIRMWARE_MAX_WIDTH     64
#define OV5642_AFFIRMWARE_MAX_HEIGH     48

#define OV5642_FOCUS_STA_FOCUSED        0x02
#define OV5642_FOCUS_STA_FOCUSING       0x01
#define OV5642_FOCUS_STA_STARTUP        0xFA
#define OV5642_FOCUS_STA_IDLE           0x00

#define OV5642_AF_MAX_TIMEOUT           1000000  // 1s

// for digital zoom
#define OV5642_SENSOR_MAX_WIDTH         2592
#define OV5642_SENSOR_MAX_HEIGHT        1944

#define OV5642_2XZOOM_MAX_WIDTH         640
#define OV5642_2XZOOM_MAX_HEIGHT        480

// for face detection
#define OV5642_FACEDETECTION_INTERVAL    5
#define OV5642_FACEDETECTION_PROCBUFLEN  3308936
#define OV5642_FACEDETECTION_INBUFWIDTH  160
#define OV5642_FACEDETECTION_INBUFHEIGHT 120
#define OV5642_FACEDETECTION_INBUFLEN   ( OV5642_FACEDETECTION_INBUFWIDTH * OV5642_FACEDETECTION_INBUFHEIGHT )

// OV5642 registers

// AE/AG controller
#define OV5642_REG_AECAGC                 0x3503,0,2,0
// gain
#define OV5642_REG_GAINHIGH               0x350a,0,0,0  /* no use in practice */
#define OV5642_REG_GAINLOW                0x350b,0,7,0
// exposure time
#define OV5642_REG_EXPOSUREHIGH           0x3500,0,3,0
#define OV5642_REG_EXPOSUREMID            0x3501,0,7,0
#define OV5642_REG_EXPOSURELOW            0x3502,0,7,0
// AEC VTS( Vertical Total Size )
#define OV5642_REG_AECVTSHIGH             0x350c,0,7,0
#define OV5642_REG_AECVTSLOW              0x350d,0,7,0

// AWB controller
#define OV5642_REG_AWBC_22                0x5196,0,7,0
// AWB  current value
#define OV5642_REG_CURRENT_AWBRHIGH       0x519f,0,3,0
#define OV5642_REG_CURRENT_AWBRLOW        0x51A0,0,7,0
#define OV5642_REG_CURRENT_AWBGHIGH       0x51A1,0,3,0
#define OV5642_REG_CURRENT_AWBGLOW        0x51A2,0,7,0
#define OV5642_REG_CURRENT_AWBBHIGH       0x51A3,0,3,0
#define OV5642_REG_CURRENT_AWBBLOW        0x51A4,0,7,0
// AWB set value
#define OV5642_REG_SET_AWBRHIGH           0x5198,0,3,0
#define OV5642_REG_SET_AWBRLOW            0x5199,0,7,0
#define OV5642_REG_SET_AWBGHIGH           0x519A,0,3,0
#define OV5642_REG_SET_AWBGLOW            0x519B,0,7,0
#define OV5642_REG_SET_AWBBHIGH           0x519C,0,3,0
#define OV5642_REG_SET_AWBBLOW            0x519D,0,7,0


/* Power Keep Domain */
// color effect setting
#define OV5642_REG_SDE_CTL                0x5001,7,7,7
#define OV5642_REG_SAT_CTL                0x5580,1,1,1
#define OV5642_REG_CST_CTL                0x5580,2,2,2
#define OV5642_REG_TONE_CTL               0x5580,3,4,3
#define OV5642_REG_GRAY_CTL               0x5580,5,5,5
#define OV5642_REG_NEG_CTL                0x5580,6,6,6
#define OV5642_REG_SOLARIZE_CTL           0x558a,7,7,7

#define OV5642_REG_U_FIX                  0x5585,0,7,0
#define OV5642_REG_V_FIX                  0x5586,0,7,0

// white balance setting
#define OV5642_REG_WB_CTL                 0x3406,0,0,0
#define OV5642_REG_WB_RHIGH               0x3400,0,7,0
#define OV5642_REG_WB_RLOW                0x3401,0,7,0
#define OV5642_REG_WB_GHIGH               0x3402,0,7,0
#define OV5642_REG_WB_GLOW                0x3403,0,7,0
#define OV5642_REG_WB_BHIGH               0x3404,0,7,0
#define OV5642_REG_WB_BLOW                0x3405,0,7,0

// metering windows weighting
#define OV5642_REG_WIN_XSTART_HIGH        0x5680,0,3,0
#define OV5642_REG_WIN_XSTART_LOW         0x5681,0,7,0
#define OV5642_REG_WIN_XEND_HIGH          0x5682,0,3,0
#define OV5642_REG_WIN_XEND_LOW           0x5683,0,7,0
#define OV5642_REG_WIN_YSTART_HIGH        0x5684,0,2,0
#define OV5642_REG_WIN_YSTART_LOW         0x5685,0,7,0
#define OV5642_REG_WIN_YEND_HIGH          0x5686,0,2,0
#define OV5642_REG_WIN_YEND_LOW           0x5687,0,7,0

#define OV5642_REG_EM_WIN0WEI             0x5688,0,3,0
#define OV5642_REG_EM_WIN1WEI             0x5688,4,7,4
#define OV5642_REG_EM_WIN2WEI             0x5689,0,3,0
#define OV5642_REG_EM_WIN3WEI             0x5689,4,7,4
#define OV5642_REG_EM_WIN4WEI             0x568a,0,3,0
#define OV5642_REG_EM_WIN5WEI             0x568a,4,7,4
#define OV5642_REG_EM_WIN6WEI             0x568b,0,3,0
#define OV5642_REG_EM_WIN7WEI             0x568b,4,7,4
#define OV5642_REG_EM_WIN8WEI             0x568c,0,3,0
#define OV5642_REG_EM_WIN9WEI             0x568c,4,7,4
#define OV5642_REG_EM_WIN10WEI            0x568d,0,3,0
#define OV5642_REG_EM_WIN11WEI            0x568d,4,7,4
#define OV5642_REG_EM_WIN12WEI            0x568e,0,3,0
#define OV5642_REG_EM_WIN13WEI            0x568e,4,7,4
#define OV5642_REG_EM_WIN14WEI            0x568f,0,3,0
#define OV5642_REG_EM_WIN15WEI            0x568f,4,7,4

// JPEG quality
#define OV5642_REG_JPEG_QSCTL             0x4407,0,5,0

// brightness settings
#define OV5642_REG_CB_SINSIGN             0x558a,0,0,0
#define OV5642_REG_BRIT_SIGN              0x558a,3,3,3               
#define OV5642_REG_BRIT_VAL               0x5589,0,7,0

// contrast settings
#define OV5642_REG_CST_SIGNCTL            0x558a,2,2,2
#define OV5642_REG_CST_OFFSET             0x5587,0,7,0
#define OV5642_REG_CST_VAL                0x5588,0,7,0

// saturation setting
#define OV5642_REG_SAT_U                  0x5583,0,7,0
#define OV5642_REG_SAT_V                  0x5584,0,7,0

// flash setting
#define OV5642_REG_FLASH_TYPE             0x3b00,0,1,0
#define OV5642_REG_FLASH_ONOFF            0x3b00,7,7,7
	
// focus setting
#define OV5642_REG_AF_MAIN                0x3024,0,7,0
#define OV5642_REG_AF_TAG                 0x3025,0,7,0
#define OV5642_REG_AF_PARA3               0x5082,0,7,0
#define OV5642_REG_AF_PARA2               0x5083,0,7,0
#define OV5642_REG_AF_PARA1               0x5084,0,7,0
#define OV5642_REG_AF_PARA0               0x5085,0,7,0
#define OV5642_REG_AF_ZONESTA             0x3026,0,7,0
#define OV5642_REG_AF_FOCUSSTA            0x3027,0,7,0

// binning registers
#define OV5642_REG_BIN_HORIZONTAL         0x3621,7,7,0
#define OV5642_REG_BIN_VERTICAL           0x370D,6,6,0

// pixels from sensor 
#define OV5642_REG_SENSORCROP_LEFT_HIGH   0x3824,0,7,0
#define OV5642_REG_SENSORCROP_LEFT_LOW    0x3825,0,7,0
#define OV5642_REG_SENSORCROP_TOP_HIGH    0x3826,0,7,0
#define OV5642_REG_SENSORCROP_TOP_LOW     0x3827,0,7,0

// dvp input
#define OV5642_REG_DVPIN_WIDTH_HIGH       0x3804,0,3,0
#define OV5642_REG_DVPIN_WIDTH_LOW        0x3805,0,7,0
#define OV5642_REG_DVPIN_HEIGHT_HIGH      0x3806,0,3,0
#define OV5642_REG_DVPIN_HEIGHT_LOW       0x3807,0,7,0

/* Power Down Domain */
// ISO setting
#define OV5642_REG_SET_ISOHIGH            0x3a18,0,0,0
#define OV5642_REG_SET_ISOLOW             0x3a19,0,7,0

// banding filter 
#define OV5642_REG_BF_CTL                 0x3a00,5,5,5

// EV compensation 
#define OV5642_REG_AEC_ENT_HIGH           0x3a0f,0,7,0
#define OV5642_REG_AEC_ENT_LOW            0x3a10,0,7,0
#define OV5642_REG_AEC_OUT_HIGH           0x3a1b,0,7,0
#define OV5642_REG_AEC_OUT_LOW            0x3a1e,0,7,0
#define OV5642_REG_AEC_VPT_HIGH           0x3a11,0,7,0
#define OV5642_REG_AEC_VPT_LOW            0x3a1f,0,7,0

// average lux
#define OV5642_REG_AVG_LUX                0x5690,0,7,0

// HTS and VTS
#define OV5642_REG_TIMINGHTSHIGH          0x380c,0,3,0
#define OV5642_REG_TIMINGHTSLOW           0x380d,0,7,0
#define OV5642_REG_TIMINGVTSHIGH          0x380e,0,3,0
#define OV5642_REG_TIMINGVTSLOW           0x380f,0,7,0


typedef struct 
{
	CAM_Int16u  gainQ4;   // means that the lowest 4 bit of gainQ4 are fractional part
	CAM_Int16u  exposure; // uint is t_row_interval
	CAM_Int16u  awbRgain;
	CAM_Int16u  awbGgain;
	CAM_Int16u  awbBgain;
	CAM_Int32u  maxLines;
	CAM_Int32s  fps100;
	CAM_Int32s  binFactor;
} _OV5642_3AState;

typedef struct 
{
	CAM_Bool            bIsFocusStop;
	CAM_Bool            bIsFocused;
	CAM_Tick            tFocusStart;
} _OV5642_FocusState;

typedef struct 
{
	_V4L2SensorState    stV4L2;
	_OV5642_3AState     st3Astat;
	_OV5642_FocusState  stFocusState;

	// for face detection
	CAM_Int32s          iFaceDetectFrameSkip;
	CAM_Int8u           *pUserBuf;
	CAM_Int8u           *pFaceDetectProcBuf;
	CAM_Int8u           *pFaceDetectInputBuf; 

	_CAM_Thread         stFaceDetectThread;
	// face detection result
	CAM_MultiROI        stFaceROI;

	// digital zoom reference ROI
	CAM_Rect            stRefROI;

} OV5642State;

typedef void (*OV5642_ShotModeCap)( CAM_ShotModeCapability *pShotModeCap );

#ifdef __cplusplus
}
#endif

#endif  // _CAM_OV5642_H_
