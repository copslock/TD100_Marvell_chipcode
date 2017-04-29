/*******************************************************************************
//(C) Copyright [2010 - 2011] Marvell International Ltd.
//All Rights Reserved
*******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "cam_log.h"
#include "cam_utility.h"


#include "cam_extisp_sensorwrapper.h"
#include "cam_extisp_v4l2base.h"
#include "cam_extisp_ov5640.h"


/* Sensor Capability Table */

// NOTE: if you want to enable static resolution table to bypass sensor dynamically detect to save camera-off to viwerfinder-on latency, 
//       you can fill the following four tables according to your sensor's capability. And open macro 
//       BUILD_OPTION_STARTEGY_DISABLE_DYNAMIC_SENSOR_DETECT in cam_extisp_buildopt.h

// OV5640 supported video/preview resolution
CAM_Size _OV5640VideoResTable[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT] =
{
	{176,  144},	// QCIF
	{320,  240},	// QVGA
	{352,  288},	// CIF
	{640,  480},	// VGA
	{720,  480},
//	{720,  576},	// D1
};

// OV5640 supported still capture resolution 
CAM_Size _OV5640StillResTable[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT] = 
{
	{2592, 1944},	// 5M
};

// OV5640 supported video/preview capture image format
CAM_ImageFormat _OV5640VideoFormatTable[CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT] = 
{
	CAM_IMGFMT_YCbCr420P,
};

// OV5640 supported still image format
CAM_ImageFormat _OV5640StillFormatTable[CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT] =
{
	CAM_IMGFMT_JPEG,
};

// OV5640 supported sensor rotation/flip/mirror type
CAM_FlipRotate _OV5640RotationTable[] = 
{
	CAM_FLIPROTATE_NORMAL,
//	CAM_FLIPROTATE_HFLIP,
//	CAM_FLIPROTATE_VFLIP,
//	CAM_FLIPROTATE_180,
};

// shot mode
CAM_ShotMode _OV5640ShotModeTable[] = 
{
	CAM_SHOTMODE_AUTO,
	CAM_SHOTMODE_MANUAL,
};

// exposure metering mode
_CAM_RegEntry _OV5640ExpMeter_Mean[] = 
{
	{ OV5640_REG_EM_WIN0WEI, 0x01},
	{ OV5640_REG_EM_WIN1WEI, 0x10}, 
	{ OV5640_REG_EM_WIN2WEI, 0x01}, 
	{ OV5640_REG_EM_WIN3WEI, 0x10}, 
	{ OV5640_REG_EM_WIN10WEI,0x01},
	{ OV5640_REG_EM_WIN11WEI,0x10},
	{ OV5640_REG_EM_WIN12WEI,0x01},
	{ OV5640_REG_EM_WIN13WEI,0x10},
	{ OV5640_REG_EM_WIN20WEI,0x01},
	{ OV5640_REG_EM_WIN21WEI,0x10},
	{ OV5640_REG_EM_WIN22WEI,0x01},
	{ OV5640_REG_EM_WIN23WEI,0x10},
	{ OV5640_REG_EM_WIN30WEI,0x01},
	{ OV5640_REG_EM_WIN31WEI,0x10},
	{ OV5640_REG_EM_WIN32WEI,0x01},
	{ OV5640_REG_EM_WIN33WEI,0x10},

};

_CAM_RegEntry _OV5640ExpMeter_Spot[] = 
{
	{ OV5640_REG_EM_WIN0WEI, 0x00},
	{ OV5640_REG_EM_WIN1WEI, 0x00}, 
	{ OV5640_REG_EM_WIN2WEI, 0x00}, 
	{ OV5640_REG_EM_WIN3WEI, 0x00}, 
	{ OV5640_REG_EM_WIN10WEI,0x00},
	{ OV5640_REG_EM_WIN11WEI,0x10},
	{ OV5640_REG_EM_WIN12WEI,0x01},
	{ OV5640_REG_EM_WIN13WEI,0x00},
	{ OV5640_REG_EM_WIN20WEI,0x00},
	{ OV5640_REG_EM_WIN21WEI,0x10},
	{ OV5640_REG_EM_WIN22WEI,0x01},
	{ OV5640_REG_EM_WIN23WEI,0x00},
	{ OV5640_REG_EM_WIN30WEI,0x00},
	{ OV5640_REG_EM_WIN31WEI,0x00},
	{ OV5640_REG_EM_WIN32WEI,0x00},
	{ OV5640_REG_EM_WIN33WEI,0x00},

};

_CAM_RegEntry _OV5640ExpMeter_CW[] = 
{
	{ OV5640_REG_EM_WIN0WEI, 0x02},
	{ OV5640_REG_EM_WIN1WEI, 0x60}, 
	{ OV5640_REG_EM_WIN2WEI, 0x06}, 
	{ OV5640_REG_EM_WIN3WEI, 0x20}, 
	{ OV5640_REG_EM_WIN10WEI,0x06},
	{ OV5640_REG_EM_WIN11WEI,0xe0},
	{ OV5640_REG_EM_WIN12WEI,0x0e},
	{ OV5640_REG_EM_WIN13WEI,0x60},
	{ OV5640_REG_EM_WIN20WEI,0x0a},
	{ OV5640_REG_EM_WIN21WEI,0xe0},
	{ OV5640_REG_EM_WIN22WEI,0x0e},
	{ OV5640_REG_EM_WIN23WEI,0xa0},
	{ OV5640_REG_EM_WIN30WEI,0x06},
	{ OV5640_REG_EM_WIN31WEI,0xa0},
	{ OV5640_REG_EM_WIN32WEI,0x0a},
	{ OV5640_REG_EM_WIN33WEI,0x60},

};

_CAM_ParameterEntry _OV5640ExpMeterMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_EXPOSUREMETERMODE_MEAN, _OV5640ExpMeter_Mean),
	PARAM_ENTRY_USE_REGTABLE(CAM_EXPOSUREMETERMODE_SPOT, _OV5640ExpMeter_Spot),
	PARAM_ENTRY_USE_REGTABLE(CAM_EXPOSUREMETERMODE_CENTRALWEIGHTED, _OV5640ExpMeter_CW),
};

// ISO setting
_CAM_RegEntry _OV5640IsoMode_Auto[] = 
{
	{OV5640_REG_SET_ISOLOW, 0xfc},
};

_CAM_RegEntry _OV5640IsoMode_100[] =
{
	{OV5640_REG_SET_ISOLOW, 0x6c},
};

_CAM_RegEntry _OV5640IsoMode_200[] = 
{
	{OV5640_REG_SET_ISOHIGH, 0x00},
	{OV5640_REG_SET_ISOLOW, 0x8c},
};

_CAM_RegEntry _OV5640IsoMode_400[] = 
{
	{OV5640_REG_SET_ISOHIGH, 0x00},
	{OV5640_REG_SET_ISOLOW, 0xcc},
};

_CAM_RegEntry _OV5640IsoMode_800[] =
{
	{OV5640_REG_SET_ISOHIGH, 0x00},
	{OV5640_REG_SET_ISOLOW, 0xfc},
};

_CAM_RegEntry _OV5640IsoMode_1600[] =
{
	{OV5640_REG_SET_ISOHIGH, 0x01},
	{OV5640_REG_SET_ISOLOW, 0xfc},
};

_CAM_ParameterEntry _OV5640IsoMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_ISO_AUTO, _OV5640IsoMode_Auto),
	PARAM_ENTRY_USE_REGTABLE(CAM_ISO_100, _OV5640IsoMode_100),
	PARAM_ENTRY_USE_REGTABLE(CAM_ISO_200, _OV5640IsoMode_200),
	PARAM_ENTRY_USE_REGTABLE(CAM_ISO_400, _OV5640IsoMode_400),
	PARAM_ENTRY_USE_REGTABLE(CAM_ISO_800, _OV5640IsoMode_800),
	PARAM_ENTRY_USE_REGTABLE(CAM_ISO_1600,_OV5640IsoMode_1600),
};

// band filter setting
_CAM_RegEntry _OV5640BdFltMode_Auto[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _OV5640BdFltMode_Off[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _OV5640BdFltMode_50Hz[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _OV5640BdFltMode_60Hz[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _OV5640BdFltMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_BANDFILTER_AUTO, _OV5640BdFltMode_Auto),
	PARAM_ENTRY_USE_REGTABLE(CAM_BANDFILTER_OFF,  _OV5640BdFltMode_Off),
	PARAM_ENTRY_USE_REGTABLE(CAM_BANDFILTER_50HZ, _OV5640BdFltMode_50Hz),
	PARAM_ENTRY_USE_REGTABLE(CAM_BANDFILTER_60HZ, _OV5640BdFltMode_60Hz),
};

// Flash Mode
_CAM_RegEntry _OV5640FlashMode_Off[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _OV5640FlashMode_On[] =
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _OV5640FlashMode_Auto[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _OV5640FlashMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_FLASH_OFF,	_OV5640FlashMode_Off),
};

// white balance mode
_CAM_RegEntry _OV5640WBMode_Auto[] = 
{
	{OV5640_REG_WB_CTL,0x00},
};

_CAM_RegEntry _OV5640WBMode_Incandescent[] = 
{
	{OV5640_REG_WB_CTL,0x01},
	{OV5640_REG_WB_RHIGH,0x04},
	{OV5640_REG_WB_RLOW,0x88},
	{OV5640_REG_WB_GHIGH,0x04},
	{OV5640_REG_WB_GLOW,0x00},
	{OV5640_REG_WB_BHIGH,0x08},
	{OV5640_REG_WB_BLOW,0xb6},
};

_CAM_RegEntry _OV5640WBMode_CoolFluorescent[] = 
{
	{OV5640_REG_WB_CTL,0x01},
	{OV5640_REG_WB_RHIGH,0x06},
	{OV5640_REG_WB_RLOW,0x2a},
	{OV5640_REG_WB_GHIGH,0x04},
	{OV5640_REG_WB_GLOW,0x00},
	{OV5640_REG_WB_BHIGH,0x07},
	{OV5640_REG_WB_BLOW,0x24},
};

_CAM_RegEntry _OV5640WBMode_Daylight[] = 
{
	{OV5640_REG_WB_CTL,0x01},
	{OV5640_REG_WB_RHIGH,0x07},
	{OV5640_REG_WB_RLOW,0x02},
	{OV5640_REG_WB_GHIGH,0x04},
	{OV5640_REG_WB_GLOW,0x00},
	{OV5640_REG_WB_BHIGH,0x05},
	{OV5640_REG_WB_BLOW,0x15},
};

_CAM_RegEntry _OV5640WBMode_Cloudy[] = 
{
	{OV5640_REG_WB_CTL,0x01},
	{OV5640_REG_WB_RHIGH,0x07},
	{OV5640_REG_WB_RLOW,0x88},
	{OV5640_REG_WB_GHIGH,0x04},
	{OV5640_REG_WB_GLOW,0x00},
	{OV5640_REG_WB_BHIGH,0x05},
	{OV5640_REG_WB_BLOW,0x00},
};

_CAM_ParameterEntry _OV5640WBMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_WHITEBALANCEMODE_AUTO, _OV5640WBMode_Auto),
	PARAM_ENTRY_USE_REGTABLE(CAM_WHITEBALANCEMODE_INCANDESCENT, _OV5640WBMode_Incandescent),
	PARAM_ENTRY_USE_REGTABLE(CAM_WHITEBALANCEMODE_COOLWHITE_FLUORESCENT, _OV5640WBMode_CoolFluorescent),
	PARAM_ENTRY_USE_REGTABLE(CAM_WHITEBALANCEMODE_DAYLIGHT,	_OV5640WBMode_Daylight),
	PARAM_ENTRY_USE_REGTABLE(CAM_WHITEBALANCEMODE_CLOUDY, _OV5640WBMode_Cloudy),
};


// focus mode
_CAM_RegEntry _OV5640FocusMode_Infinity[] =
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _OV5640FocusMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_FOCUS_INFINITY,  _OV5640FocusMode_Infinity),
};

// color effect mode
_CAM_RegEntry _OV5640ColorEffectMode_Off[] = 
{
	{OV5640_REG_SDE_CTL, 0x80},
	{OV5640_REG_TONE_CTL,0x00},
	{OV5640_REG_NEG_CTL, 0x00},
};

_CAM_RegEntry _OV5640ColorEffectMode_Vivid[] = 
{
	{OV5640_REG_SDE_CTL, 0x80},
	{OV5640_REG_SAT_CTL,0x02},
	{OV5640_REG_U_CTL,0x70},
	{OV5640_REG_V_CTL,0x70},
	{OV5640_REG_SDE_CTL2,0x40},
};

_CAM_RegEntry _OV5640ColorEffectMode_Sepia[] = 
{
	{OV5640_REG_SDE_CTL, 0x80},
	{OV5640_REG_TONE_CTL,0x18},
	{OV5640_REG_NEG_CTL, 0x00},
	{OV5640_REG_U_CTL,0x40},
	{OV5640_REG_V_CTL,0xa0},
};

_CAM_RegEntry _OV5640ColorEffectMode_Grayscale[] = 
{
	{OV5640_REG_SDE_CTL, 0x80},
	{OV5640_REG_TONE_CTL,0x18},
	{OV5640_REG_NEG_CTL, 0x00},
	{OV5640_REG_U_CTL,0x80},
	{OV5640_REG_V_CTL,0x80},
};


_CAM_RegEntry _OV5640ColorEffectMode_Negative[] =
{
	{OV5640_REG_SDE_CTL, 0x80},
	{OV5640_REG_NEG_CTL, 0x40},
};

_CAM_ParameterEntry _OV5640ColorEffectMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_COLOREFFECT_OFF,   _OV5640ColorEffectMode_Off),
	PARAM_ENTRY_USE_REGTABLE(CAM_COLOREFFECT_VIVID, _OV5640ColorEffectMode_Vivid),
	PARAM_ENTRY_USE_REGTABLE(CAM_COLOREFFECT_SEPIA, _OV5640ColorEffectMode_Sepia),
	PARAM_ENTRY_USE_REGTABLE(CAM_COLOREFFECT_GRAYSCALE, _OV5640ColorEffectMode_Grayscale),
	PARAM_ENTRY_USE_REGTABLE(CAM_COLOREFFECT_NEGATIVE, _OV5640ColorEffectMode_Negative),	
};

// JPEG quality factor table
_CAM_RegEntry _OV5640JpegQuality_Low[] =
{
	{OV5640_REG_JPEG_QSCTL, 0x03},
};

_CAM_RegEntry _OV5640JpegQuality_Medium[] = 
{
	{OV5640_REG_JPEG_QSCTL, 0x04},
};

_CAM_RegEntry _OV5640JpegQuality_High[] = 
{
	{OV5640_REG_JPEG_QSCTL, 0x08},
};

_CAM_ParameterEntry _OV5640JpegQuality[] = 
{
	PARAM_ENTRY_USE_REGTABLE(0, _OV5640JpegQuality_Low),
	PARAM_ENTRY_USE_REGTABLE(1, _OV5640JpegQuality_Medium),
	PARAM_ENTRY_USE_REGTABLE(2, _OV5640JpegQuality_High),
};


static CAM_Error _OV5640SaveAeAwb( const _CAM_SmartSensorConfig*, void* );
static CAM_Error _OV5640RestoreAeAwb( const _CAM_SmartSensorConfig*, void* );
static CAM_Error _OV5640StartFlash( void* );
static CAM_Error _OV5640StopFlash( void* );

static CAM_Error _OV5640FillFrameShotInfo( OV5640State*, CAM_ImageBuffer* );
static CAM_Error _OV5640SetJpegParam( OV5640State*, CAM_JpegParam* );

// shot mode capability
static void _OV5640AutoModeCap( CAM_ShotModeCapability* );
// static void _OV5640ManualModeCap( CAM_ShotModeCapability* );
// static void _OV5640NightModeCap( CAM_ShotModeCapability* );
/* shot mode cap function table */
OV5640_ShotModeCap _OV5640ShotModeCap[CAM_SHOTMODE_NUM] = { _OV5640AutoModeCap, // CAM_SHOTMODE_AUTO = 0,
                                                           NULL              ,  // CAM_SHOTMODE_MANUAL,
                                                           NULL              ,  // CAM_SHOTMODE_PORTRAIT,
                                                           NULL              ,  // CAM_SHOTMODE_LANDSCAPE,
                                                           NULL              ,  // CAM_SHOTMODE_NIGHTPORTRAIT,
                                                           NULL              ,  // CAM_SHOTMODE_NIGHTSCENE,
                                                           NULL              ,  // CAM_SHOTMODE_CHILD,
                                                           NULL              ,  // CAM_SHOTMODE_INDOOR,
                                                           NULL              ,  // CAM_SHOTMODE_PLANTS,
                                                           NULL              ,  // CAM_SHOTMODE_SNOW,
                                                           NULL              ,  // CAM_SHOTMODE_BEACH,
                                                           NULL              ,  // CAM_SHOTMODE_FIREWORKS,
                                                           NULL              ,  // CAM_SHOTMODE_SUBMARINE, 
                                                           NULL              ,  // CAM_SHOTMODE_WHITEBOARD,
                                                           NULL              ,  // CAM_SHOTMODE_SPORTS,
                                                          };  


/* OV5640 behavior interface */

extern _SmartSensorFunc func_ov5640; 
CAM_Error OV5640_SelfDetect( _SmartSensorAttri *pSensorInfo )
{
	CAM_Error error = CAM_ERROR_NONE;

	// NOTE:  If you open macro BUILD_OPTION_STARTEGY_DISABLE_DYNAMIC_SENSOR_DETECT in cam_extisp_buildopt.h 
	//        to bypass sensor dynamically detect to save camera-off to viwerfinder-on latency, you should initilize
	//        _OV5640VideoResTable/_OV5640StillResTable/_OV5640VideoFormatTable/_OV5640StillFormatTable manually.

#if !defined( BUILD_OPTION_STARTEGY_DISABLE_DYNAMIC_SENSOR_DETECT )
	error = V4L2_SelfDetect( pSensorInfo, "ov5640", &func_ov5640,
	                         _OV5640VideoResTable, _OV5640StillResTable,
	                         _OV5640VideoFormatTable, _OV5640StillFormatTable );
#else
	{
		_V4L2SensorEntry *pSensorEntry = (_V4L2SensorEntry*)( pSensorInfo->cReserved );
		strcpy( pSensorInfo->sSensorName, "ov5640" );
		pSensorInfo->pFunc = &func_ov5640;

		// FIXME: the following is just an example in Marvell platform, you should change it according to your driver implementation
		strcpy( pSensorEntry->sDeviceName, "/dev/video0" );
		pSensorEntry->iV4L2SensorID = 1;
	}
#endif

	return error;
} 

CAM_Error OV5640_GetCapability( _CAM_SmartSensorCapability *pCapability )
{
	CAM_Int32s i = 0;


	// preview/video supporting 
	// format
	pCapability->iSupportedVideoFormatCnt = 0;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT; i++ ) 
	{
		if ( _OV5640VideoFormatTable[i] == 0 )
		{
			break;
		}
		pCapability->eSupportedVideoFormat[pCapability->iSupportedVideoFormatCnt] = _OV5640VideoFormatTable[i];
		pCapability->iSupportedVideoFormatCnt++;
	}

	pCapability->bArbitraryVideoSize    = CAM_FALSE;
	pCapability->iSupportedVideoSizeCnt = 0;
	pCapability->stMinVideoSize.iWidth   = _OV5640VideoResTable[0].iWidth;
	pCapability->stMinVideoSize.iHeight  = _OV5640VideoResTable[0].iHeight;
	pCapability->stMaxVideoSize.iWidth   = _OV5640VideoResTable[0].iWidth;
	pCapability->stMaxVideoSize.iHeight  = _OV5640VideoResTable[0].iHeight;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_SIZE_CNT; i++ )
	{
		if ( _OV5640VideoResTable[i].iWidth == 0 || _OV5640VideoResTable[i].iHeight == 0)
		{
			break;
		}
		pCapability->stSupportedVideoSize[pCapability->iSupportedVideoSizeCnt] = _OV5640VideoResTable[i];
		pCapability->iSupportedVideoSizeCnt++;

		if ( ( pCapability->stMinVideoSize.iWidth > _OV5640VideoResTable[i].iWidth ) || 
		     ( ( pCapability->stMinVideoSize.iWidth == _OV5640VideoResTable[i].iWidth ) && ( pCapability->stMinVideoSize.iHeight > _OV5640VideoResTable[i].iHeight ) ) ) 
		{
			pCapability->stMinVideoSize.iWidth = _OV5640VideoResTable[i].iWidth;
			pCapability->stMinVideoSize.iHeight = _OV5640VideoResTable[i].iHeight;
		}
		if ( ( pCapability->stMaxVideoSize.iWidth < _OV5640VideoResTable[i].iWidth) ||
		     ( ( pCapability->stMaxVideoSize.iWidth == _OV5640VideoResTable[i].iWidth ) && ( pCapability->stMaxVideoSize.iHeight < _OV5640VideoResTable[i].iHeight ) ) )
		{
			pCapability->stMaxVideoSize.iWidth = _OV5640VideoResTable[i].iWidth;
			pCapability->stMaxVideoSize.iHeight = _OV5640VideoResTable[i].iHeight;
		}
	}

	// still capture supporting
	// format
	pCapability->iSupportedStillFormatCnt = 0;
	pCapability->stSupportedJPEGParams.bSupportJpeg = CAM_FALSE;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT; i++ ) 
	{
		if ( _OV5640StillFormatTable[i] == CAM_IMGFMT_JPEG )
		{
			// JPEG encoder functionalities
			pCapability->stSupportedJPEGParams.bSupportJpeg = CAM_TRUE;
			pCapability->stSupportedJPEGParams.bSupportExif = CAM_FALSE;
			pCapability->stSupportedJPEGParams.bSupportThumb = CAM_FALSE;
			pCapability->stSupportedJPEGParams.iMinQualityFactor = 20;//80
			pCapability->stSupportedJPEGParams.iMaxQualityFactor = 95;//80
		}
		if ( _OV5640StillFormatTable[i] == 0 )
		{
			break;
		}
		pCapability->eSupportedStillFormat[pCapability->iSupportedStillFormatCnt] = _OV5640StillFormatTable[i];
		pCapability->iSupportedStillFormatCnt++;
	}
	// resolution
	pCapability->bArbitraryStillSize = CAM_FALSE;
	pCapability->iSupportedStillSizeCnt = 0;
	pCapability->stMinStillSize.iWidth  = _OV5640StillResTable[0].iWidth;
	pCapability->stMinStillSize.iHeight = _OV5640StillResTable[0].iHeight;
	pCapability->stMaxStillSize.iWidth  = _OV5640StillResTable[0].iWidth;
	pCapability->stMaxStillSize.iHeight = _OV5640StillResTable[0].iHeight;
	for (i = 0; i < CAM_MAX_SUPPORT_IMAGE_SIZE_CNT; i++)
	{
		if ( _OV5640StillResTable[i].iWidth == 0 || _OV5640StillResTable[i] .iHeight == 0 )
		{
			break;
		}

		pCapability->stSupportedStillSize[pCapability->iSupportedStillSizeCnt] = _OV5640StillResTable[i];
		pCapability->iSupportedStillSizeCnt++;

		if ( ( pCapability->stMinStillSize.iWidth > _OV5640StillResTable[i].iWidth ) ||
		     ( ( pCapability->stMinStillSize.iWidth == _OV5640StillResTable[i].iWidth ) && ( pCapability->stMinStillSize.iHeight > _OV5640StillResTable[i].iHeight ) ) )
		{
			pCapability->stMinStillSize.iWidth  = _OV5640StillResTable[i].iWidth;
			pCapability->stMinStillSize.iHeight = _OV5640StillResTable[i].iHeight;
		}
		if ( ( pCapability->stMaxStillSize.iWidth < _OV5640StillResTable[i].iWidth ) || 
		     ( ( pCapability->stMaxStillSize.iWidth == _OV5640StillResTable[i].iWidth ) && ( pCapability->stMaxStillSize.iHeight < _OV5640StillResTable[i].iHeight ) ) )
		{
			pCapability->stMaxStillSize.iWidth = _OV5640StillResTable[i].iWidth;
			pCapability->stMaxStillSize.iHeight = _OV5640StillResTable[i].iHeight;
		}
	}

	// rotate
	pCapability->iSupportedRotateCnt = _ARRAY_SIZE( _OV5640RotationTable );
	for ( i = 0; i < pCapability->iSupportedRotateCnt; i++ )
	{
		pCapability->eSupportedRotate[i] = _OV5640RotationTable[i];
	}

	pCapability->iSupportedShotModeCnt = _ARRAY_SIZE( _OV5640ShotModeTable );
	for ( i = 0; i < pCapability->iSupportedShotModeCnt; i++ )
	{
		pCapability->eSupportedShotMode[i] = _OV5640ShotModeTable[i];
	}

	// FIXME: all supported shot parameters
	_OV5640AutoModeCap( &pCapability->stSupportedShotParams );

	return CAM_ERROR_NONE;

}

CAM_Error OV5640_GetShotModeCapability( CAM_ShotMode eShotMode, CAM_ShotModeCapability *pShotModeCap )
{
	CAM_Int32s i;
	// BAC check
	for ( i = 0; i < _ARRAY_SIZE( _OV5640ShotModeTable ); i++ )
	{
		if ( _OV5640ShotModeTable[i] == eShotMode )
		{
			break;
		}
	}

	if ( i >= _ARRAY_SIZE(_OV5640ShotModeTable) || pShotModeCap ==NULL ) 
	{
		return CAM_ERROR_BADARGUMENT;
	}

    (void)(_OV5640ShotModeCap[eShotMode])( pShotModeCap );

	return CAM_ERROR_NONE;
}

CAM_Error OV5640_Init( void **ppDevice, void *pSensorEntry ) 
{
	CAM_Error  error      = CAM_ERROR_NONE;
	CAM_Int32s iSkipFrame = 0;
	_V4L2SensorAttribute  _OV5640Attri;
	_V4L2SensorEntry *pSensor = (_V4L2SensorEntry*)(pSensorEntry);
	OV5640State *pState = (OV5640State*)malloc( sizeof( OV5640State ) );

	memset( &_OV5640Attri, 0, sizeof(_V4L2SensorAttribute) );

	*ppDevice = pState;
	if ( *ppDevice == NULL )
	{
		return CAM_ERROR_OUTOFMEMORY;
	}

	_OV5640Attri.stV4L2SensorEntry.iV4L2SensorID = pSensor->iV4L2SensorID;
	strcpy( _OV5640Attri.stV4L2SensorEntry.sDeviceName, pSensor->sDeviceName );

#if defined( BUILD_OPTION_DEBUG_DISABLE_SAVE_RESTORE_3A )
	iSkipFrame                        = 20;
	_OV5640Attri.fnSaveAeAwb          = NULL;
	_OV5640Attri.fnRestoreAeAwb       = NULL;
	_OV5640Attri.pSaveRestoreUserData = NULL;
	_OV5640Attri.fnStartFlash         = NULL;
 	_OV5640Attri.fnStopFlash          = NULL;
#else
	iSkipFrame                        = 2;
	_OV5640Attri.fnSaveAeAwb          = _OV5640SaveAeAwb;
	_OV5640Attri.fnRestoreAeAwb       = _OV5640RestoreAeAwb;
	_OV5640Attri.pSaveRestoreUserData = (void*)pState;
 	_OV5640Attri.fnStartFlash         =  _OV5640StartFlash;
 	_OV5640Attri.fnStopFlash          =  _OV5640StopFlash;
#endif

	error = V4L2_Init( &(pState->stV4L2), &(_OV5640Attri), iSkipFrame );

	/* here we can get default shot params */
	pState->stV4L2.stShotParam.eShotMode        = CAM_SHOTMODE_AUTO;
	pState->stV4L2.stShotParam.eExpMode         = CAM_EXPOSUREMODE_AUTO;
	pState->stV4L2.stShotParam.eExpMeterMode    = CAM_EXPOSUREMETERMODE_AUTO;
	pState->stV4L2.stShotParam.iEvCompQ16       = 0;
	pState->stV4L2.stShotParam.eIsoMode         = CAM_ISO_AUTO;
	pState->stV4L2.stShotParam.iShutterSpeedQ16 = -1;
	pState->stV4L2.stShotParam.iFNumQ16         = 1;
	pState->stV4L2.stShotParam.eBandFilterMode  = CAM_BANDFILTER_50HZ;
	pState->stV4L2.stShotParam.eWBMode          = CAM_WHITEBALANCEMODE_AUTO;
	pState->stV4L2.stShotParam.eFocusMode       = CAM_FOCUS_INFINITY;
	pState->stV4L2.stShotParam.iDigZoomQ16      = 0;
	pState->stV4L2.stShotParam.eColorEffect     = CAM_COLOREFFECT_OFF;
	pState->stV4L2.stShotParam.iSaturation      = 64;
	pState->stV4L2.stShotParam.iBrightness      = 0;
	pState->stV4L2.stShotParam.iContrast        = 0;
	pState->stV4L2.stShotParam.iSharpness       = 0;
	pState->stV4L2.stShotParam.eFlashMode       = CAM_FLASH_OFF;
	pState->stV4L2.stShotParam.bVideoStabilizer = CAM_FALSE;
	pState->stV4L2.stShotParam.bVideoNoiseReducer = CAM_FALSE;
	pState->stV4L2.stShotParam.bZeroShutterLag    = CAM_FALSE;
	pState->stV4L2.stShotParam.bHighDynamicRange  = CAM_FALSE;

	pState->stV4L2.stShotParam.iBurstCnt          = 1;
	/* get default JPEG params */
	pState->stV4L2.stJpegParam.iSampling      = 1; // 0 - 420, 1 - 422, 2 - 444
	pState->stV4L2.stJpegParam.iQualityFactor = 80;//67
	pState->stV4L2.stJpegParam.bEnableExif    = CAM_FALSE;
	pState->stV4L2.stJpegParam.bEnableThumb   = CAM_FALSE;
	pState->stV4L2.stJpegParam.iThumbWidth    = 0;
	pState->stV4L2.stJpegParam.iThumbHeight   = 0;

	return error;
}

CAM_Error OV5640_Deinit( void *pDevice )
{
	OV5640State *pState = (OV5640State*)pDevice;

	(void)V4L2_Deinit( &pState->stV4L2 );

	free( pState );

	return CAM_ERROR_NONE;
}

CAM_Error OV5640_RegisterEventHandler( void *pDevice, CAM_EventHandler fnEventHandler, void *pUserData )
{
	OV5640State *pState = (OV5640State*)pDevice;
	CAM_Error   error   = CAM_ERROR_NONE;

	error = V4L2_RegisterEventHandler( &pState->stV4L2, fnEventHandler, pUserData );

	return error;
}

CAM_Error OV5640_Config( void *pDevice, _CAM_SmartSensorConfig *pSensorConfig )
{
	OV5640State *pState = (OV5640State*)pDevice;
	CAM_Error   error   = CAM_ERROR_NONE;

	error = V4L2_Config( &pState->stV4L2, pSensorConfig );
	if ( error != CAM_ERROR_NONE )
	{
		return error;
	}

	if ( pSensorConfig->eState != CAM_CAPTURESTATE_IDLE )
	{
		if ( pSensorConfig->eFormat == CAM_IMGFMT_JPEG ) //modify for liyong
		{
			error = _OV5640SetJpegParam( pDevice, &(pSensorConfig->stJpegParam) );
			if ( error != CAM_ERROR_NONE )
			{
				return error;
			}
		}
	}
	pState->stV4L2.stConfig = *pSensorConfig;

	return CAM_ERROR_NONE;
}

CAM_Error OV5640_GetBufReq( void *pDevice, _CAM_SmartSensorConfig *pSensorConfig, CAM_ImageBufferReq *pBufReq )
{
	OV5640State *pState = (OV5640State*)pDevice;
	CAM_Error   error   = CAM_ERROR_NONE;

	error = V4L2_GetBufReq( &pState->stV4L2, pSensorConfig, pBufReq );

	return error;
}

CAM_Error OV5640_Enqueue( void *pDevice, CAM_ImageBuffer *pImgBuf )
{
	OV5640State *pState = (OV5640State*)pDevice;
	CAM_Error   error   = CAM_ERROR_NONE;

	error = V4L2_Enqueue( &pState->stV4L2, pImgBuf );

	return error;
}

CAM_Error OV5640_Dequeue( void *pDevice, CAM_ImageBuffer **ppImgBuf )
{
	OV5640State *pState = (OV5640State*)pDevice;
	CAM_Error   error   = CAM_ERROR_NONE;

	error = V4L2_Dequeue( &pState->stV4L2, ppImgBuf );

	if ( error == CAM_ERROR_NONE && (*ppImgBuf)->bEnableShotInfo )
	{
		error = _OV5640FillFrameShotInfo( pState, *ppImgBuf );
	}

	return error;
}

CAM_Error OV5640_Flush( void *pDevice )
{
	OV5640State *pState = (OV5640State*)pDevice;
	CAM_Error   error   = CAM_ERROR_NONE;

	error = V4L2_Flush( &pState->stV4L2 );

	return error;
}

CAM_Error OV5640_SetShotParam( void *pDevice, _CAM_ShotParam *pShotParam )
{
	OV5640State *pState = (OV5640State*)pDevice;
	int         ret     = 0;

	// ISO setting
	if ( pShotParam->eIsoMode != pState->stV4L2.stShotParam.eIsoMode ) 
	{
		int i = 0;
		for ( i = 0; i < _ARRAY_SIZE(_OV5640IsoMode); i++ ) 
		{
			if ( pShotParam->eIsoMode == _OV5640IsoMode[i].iParameter ) 
			{
				break;
			}
		}

		if ( i >= _ARRAY_SIZE(_OV5640IsoMode) ) 
		{
			return CAM_ERROR_NOTSUPPORTEDARG;
		}

		ret = _set_reg_array( pState->stV4L2.iSensorFD, _OV5640IsoMode[i].stSetParam.stRegTable.pEntries,_OV5640IsoMode[i].stSetParam.stRegTable.iLength );

		pState->stV4L2.stShotParam.eIsoMode = pShotParam->eIsoMode;

	}

	// Color Effect Setting
	if ( pShotParam->eColorEffect != pState->stV4L2.stShotParam.eColorEffect ) 
	{

		int i = 0;
		for ( i = 0; i < _ARRAY_SIZE(_OV5640ColorEffectMode); i++ ) 
		{
			if ( pShotParam->eColorEffect == _OV5640ColorEffectMode[i].iParameter ) 
			{
				break;
			}
		}

		if ( i >= _ARRAY_SIZE(_OV5640ColorEffectMode) ) 
		{

			return CAM_ERROR_NOTSUPPORTEDARG;
		}

		ret = _set_reg_array( pState->stV4L2.iSensorFD, _OV5640ColorEffectMode[i].stSetParam.stRegTable.pEntries, _OV5640ColorEffectMode[i].stSetParam.stRegTable.iLength );

		pState->stV4L2.stShotParam.eColorEffect = pShotParam->eColorEffect;
	}

	// White Balance Setting
	if ( pShotParam->eWBMode != pState->stV4L2.stShotParam.eWBMode ) 
	{

		int i = 0;
		for ( i = 0; i < _ARRAY_SIZE(_OV5640WBMode); i++ ) 
		{
			if ( pShotParam->eWBMode == _OV5640WBMode[i].iParameter )
			{
				break;
			}
		}

		if ( i >= _ARRAY_SIZE(_OV5640WBMode) ) 
		{

			return CAM_ERROR_NOTSUPPORTEDARG;
		}

		ret = _set_reg_array( pState->stV4L2.iSensorFD, _OV5640WBMode[i].stSetParam.stRegTable.pEntries, _OV5640WBMode[i].stSetParam.stRegTable.iLength );

		pState->stV4L2.stShotParam.eWBMode = pShotParam->eWBMode;
	}


	// Exposure Metering mode
	if ( pShotParam->eExpMeterMode != pState->stV4L2.stShotParam.eExpMeterMode ) 
	{

		int i = 0;
		for ( i = 0; i < _ARRAY_SIZE(_OV5640ExpMeterMode); i++ ) 
		{
			if ( pShotParam->eExpMeterMode == _OV5640ExpMeterMode[i].iParameter ) 
			{
				break;
			}
		}

		if ( i >= _ARRAY_SIZE(_OV5640ExpMeterMode) ) 
		{

		return CAM_ERROR_NOTSUPPORTEDARG;
		}

		ret = _set_reg_array( pState->stV4L2.iSensorFD, _OV5640ExpMeterMode[i].stSetParam.stRegTable.pEntries, _OV5640ExpMeterMode[i].stSetParam.stRegTable.iLength);

		pState->stV4L2.stShotParam.eExpMeterMode = pShotParam->eExpMeterMode;
	}

	// Shutter Speed setting
	if ( pShotParam->iShutterSpeedQ16 != pState->stV4L2.stShotParam.iShutterSpeedQ16 )
	{
		;
	}

	return CAM_ERROR_NONE;
#if 0
	OV5640State *pState = (OV5640State*)pDevice;
// TODO: here you can add your code to set shot params you supported, just like examples in ov5642.c or you can give your own style
	pState->stV4L2.stShotParam = *pShotParam;

	return CAM_ERROR_NONE;
#endif
}

CAM_Error OV5640_GetShotParam( void *pDevice, _CAM_ShotParam *pShotParam )
{
	OV5640State *pState = (OV5640State*)pDevice;
    
	*pShotParam = pState->stV4L2.stShotParam;
	return CAM_ERROR_NONE;
}

CAM_Error OV5640_StartFocus( void *pDevice, void *pFocusROI )
{
	// TODO: add your start focus code here,an refrence is ov5642.c

	return CAM_ERROR_NONE;
}

CAM_Error OV5640_CancelFocus( void *pDevice )
{
	// TODO: add your cancel focus code here,an refrence is ov5642.c

	return CAM_ERROR_NONE;
}

CAM_Error OV5640_CheckFocusState( void *pDevice, CAM_Bool *pFocusAutoStopped, _CAM_FocusState *pFocusState )
{

	// TODO: add your check focus status code here,an refrence is ov5642.c
	*pFocusAutoStopped = CAM_TRUE;
	*pFocusState       = CAM_IN_FOCUS;

	return CAM_ERROR_NONE;
}

CAM_Error OV5640_GetDigitalZoomCapability( CAM_Int32s iWidth, CAM_Int32s iHeight, CAM_Int32s *pSensorDigZoomQ16 )
{
	// TODO: add your get zoom capability code here,an refrence is ov5642.c
	*pSensorDigZoomQ16 = ( 1 << 16 );

	return CAM_ERROR_NONE;
}


_SmartSensorFunc func_ov5640 = 
{
	OV5640_GetCapability,
	OV5640_GetShotModeCapability,

	OV5640_SelfDetect,
	OV5640_Init,
	OV5640_Deinit,
	OV5640_RegisterEventHandler,
	OV5640_Config,
	OV5640_GetBufReq,
	OV5640_Enqueue,
	OV5640_Dequeue,
	OV5640_Flush,
	OV5640_SetShotParam,
	OV5640_GetShotParam,

	OV5640_StartFocus,
	OV5640_CancelFocus,
	OV5640_CheckFocusState,
	OV5640_GetDigitalZoomCapability,
};

static CAM_Error _OV5640SetJpegParam( OV5640State *pState , CAM_JpegParam *pJpegParam )
{
	if ( pJpegParam->bEnableExif )
	{
		return CAM_ERROR_NOTSUPPORTEDARG;
	}

	if ( pJpegParam->bEnableThumb )
	{
		return CAM_ERROR_NOTSUPPORTEDARG;
	}
/*
	if ( pJpegParam->iQualityFactor != 80 )
	{CELOG( "------------555-----------\n" );
		return CAM_ERROR_NOTSUPPORTEDARG;
	}*/
	else
	{
		pState->stV4L2.stJpegParam.iQualityFactor = 80;
	}

	return CAM_ERROR_NONE;
}

/*-------------------------------------------------------------------------------------------------------------------------------------
 * OV5640 shotmode capability
 * TODO: if you enable new shot mode, pls add a correspoding modcap function here, and add it to OV5640_shotModeCap _OV5640ShotModeCap array
 *------------------------------------------------------------------------------------------------------------------------------------*/
static void _OV5640AutoModeCap( CAM_ShotModeCapability *pShotModeCap )
{
	// exposure mode
	pShotModeCap->iSupportedExpModeCnt  = 1;
	pShotModeCap->eSupportedExpMode[0]  = CAM_EXPOSUREMODE_AUTO;
	
	// exposure metering mode 
	pShotModeCap->iSupportedExpMeterCnt = 1;
	pShotModeCap->eSupportedExpMeter[0] = CAM_EXPOSUREMETERMODE_AUTO;

	// EV compensation 
	pShotModeCap->iEVCompStepQ16 = 0;
	pShotModeCap->iMinEVCompQ16  = 0;
	pShotModeCap->iMaxEVCompQ16  = 0;

	// ISO mode 
	pShotModeCap->iSupportedIsoModeCnt = 1;
	pShotModeCap->eSupportedIsoMode[0] = CAM_ISO_AUTO;

	// shutter speed
	pShotModeCap->iMinShutSpdQ16 = -1;
	pShotModeCap->iMaxShutSpdQ16 = -1;

	// F-number
	pShotModeCap->iMinFNumQ16 = (CAM_Int32s)(2.8 * 65536 + 0.5);
	pShotModeCap->iMaxFNumQ16 = (CAM_Int32s)(2.8 * 65536 + 0.5);

	// band filter
	pShotModeCap->iSupportedBdFltModeCnt = 1; 
	pShotModeCap->eSupportedBdFltMode[0] = CAM_BANDFILTER_50HZ;

	// flash mode
	pShotModeCap->iSupportedFlashModeCnt = 0;

	// white balance mode
	pShotModeCap->iSupportedWBModeCnt = 1;
	pShotModeCap->eSupportedWBMode[0] = CAM_WHITEBALANCEMODE_AUTO;

	// focus mode
	pShotModeCap->iSupportedFocusModeCnt = 1;
	pShotModeCap->eSupportedFocusMode[0] = CAM_FOCUS_INFINITY;

	// optical zoom mode
	pShotModeCap->iMinOptZoomQ16 = (CAM_Int32s)(1.0 * 65536 + 0.5);
	pShotModeCap->iMaxOptZoomQ16 = (CAM_Int32s)(1.0 * 65536 + 0.5);

	// digital zoom mode
	pShotModeCap->iMinDigZoomQ16 = (CAM_Int32s)(1.0 * 65536 + 0.5);
	pShotModeCap->iMaxDigZoomQ16 = (CAM_Int32s)(1.0 * 65536 + 0.5);
	pShotModeCap->bSupportSmoothDigZoom = CAM_FALSE;

	// fps
	pShotModeCap->iMinFpsQ16 = (CAM_Int32s)(1.0 * 65536 + 0.5);
	pShotModeCap->iMaxFpsQ16 = (CAM_Int32s)(40.0 * 65536 + 0.5);

	// color effect
	pShotModeCap->iSupportedColorEffectCnt = 1;
	pShotModeCap->eSupportedColorEffect[0] = CAM_COLOREFFECT_OFF; 

	// brightness
	pShotModeCap->iMinBrightness = 0;
	pShotModeCap->iMaxBrightness = 0;

	// contrast
	pShotModeCap->iMinContrast = 0;
	pShotModeCap->iMaxContrast = 0;

	// saturation
	pShotModeCap->iMinSaturation = 0;
	pShotModeCap->iMaxSaturation = 0;

	// sharpness
	pShotModeCap->iMinSharpness = 0;
	pShotModeCap->iMaxSharpness = 0;

	// advanced features
	pShotModeCap->bSupportVideoStabilizer   = CAM_FALSE;
	pShotModeCap->bSupportVideoNoiseReducer	= CAM_FALSE;
	pShotModeCap->bSupportZeroShutterLag    = CAM_FALSE;
	pShotModeCap->bSupportBurstCapture      = CAM_FALSE;
	pShotModeCap->bSupportHighDynamicRange	= CAM_FALSE;
	pShotModeCap->iMaxBurstCnt              = 1;

	return;
}


/*
* OV5640 utility functions
*/

#define Capture_Framerate 375
#define Preview_FrameRate 1500

/******************************************************************************************************
//
// Name:         _ov5640_save_ae_awb
// Description:  save preview's AEC/AGC,AWB,and VTS(Vertical Total Size) values 
// Arguments:    pCurrent3A[OUTPUT]: pointer to the structure for save AE/AG/VTS values
// Return:       camera engine error code,refer to CameraEngine.h for more detail
// Notes:        
//     1) for more detail about OV5640 register, pls refer to its software application note
// Version Log:  version      Date          Author    Description
//                v0.1      07/14/2010    Matrix Yao   Create
//                v0.2      07/30/2010    Matrix Yao   rename function, add save AWB content
*******************************************************************************************************/

static CAM_Error _ov5640_save_ae_awb( CAM_Int32s iSensorFD, _OV5640_3AState *pCurrent3A )
{
	CAM_Int16u  reg      = 0; 

	CAM_Int16u exposure  = 0;
	CAM_Int16u gain      = 0;
	CAM_Int16u awbRgain  = 0;
	CAM_Int16u awbGgain  = 0;
	CAM_Int16u awbBgain  = 0;
	CAM_Int32u maxLines  = 0;

	// BAC (Bad Argument Check)
	_CHECK_BAD_POINTER( pCurrent3A );


	// check if AEC/AGC convergence
	// !!!FIXME
	if ( 1 )
	{
		;
	}


	// stop AEC/AGC
	reg = 0x0007;
	_set_sensor_reg( iSensorFD, OV5640_REG_AECAGC, reg );

	/* gain */
	reg = 0;
	_get_sensor_reg( iSensorFD, OV5640_REG_GAINLOW, &reg );
	gain = ( reg & 0x000f ) + 16;
	if ( reg & 0x0010 )
	{
		gain <<= 1;
	}
	if ( reg & 0x0020 )
	{
		gain <<= 1;
	}
	if ( reg & 0x0040 )
	{
		gain <<= 1;
	}
	if ( reg & 0x0080 )
	{
		gain <<= 1;
	}
	pCurrent3A->gainQ4 = gain;

	/* exposure */
	reg = 0;
	_get_sensor_reg( iSensorFD, OV5640_REG_EXPOSUREHIGH, &reg );
	exposure = ( reg & 0x000f );
	exposure <<= 12;

	reg = 0;
	_get_sensor_reg( iSensorFD, OV5640_REG_EXPOSUREMID, &reg );
	reg = ( reg & 0x00ff );
	exposure += ( reg << 4 );

	reg = 0;
	_get_sensor_reg( iSensorFD, OV5640_REG_EXPOSURELOW, &reg );
	reg = ( reg & 0x00f0 );
	exposure += ( reg >> 4 );

	pCurrent3A->exposure = exposure;

	/* VTS */
	reg = 0;
	_get_sensor_reg( iSensorFD, OV5640_REG_VTSHIGH, &reg );
	maxLines = ( reg  & 0x00ff );
	maxLines <<= 8;

	reg = 0;
	_get_sensor_reg( iSensorFD, OV5640_REG_VTSLOW, &reg );
	maxLines += ( reg & 0x00ff );

	pCurrent3A->maxLines = maxLines;

	/* AWB */

	reg = 0;
	_get_sensor_reg( iSensorFD, OV5640_REG_CURRENT_AWBRHIGH, &reg );
	awbRgain = ( reg & 0x000f );
	awbRgain <<= 8;
	reg = 0;
	_get_sensor_reg( iSensorFD, OV5640_REG_CURRENT_AWBRLOW, &reg );
	awbRgain |= reg;
	pCurrent3A->awbRgain = awbRgain;

	reg = 0;
	_get_sensor_reg( iSensorFD, OV5640_REG_CURRENT_AWBGHIGH, &reg );
	awbGgain = ( reg & 0x000f );
	awbGgain <<= 8;
	reg = 0;
	_get_sensor_reg( iSensorFD, OV5640_REG_CURRENT_AWBGLOW, &reg );
	awbGgain |= reg;
	pCurrent3A->awbGgain = awbGgain;

	reg = 0;
	_get_sensor_reg( iSensorFD, OV5640_REG_CURRENT_AWBBHIGH, &reg );
	awbBgain = ( reg & 0x000f );
	awbBgain <<= 8;
	reg = 0;
	_get_sensor_reg( iSensorFD, OV5640_REG_CURRENT_AWBBLOW, &reg );
	awbBgain |= reg;
	pCurrent3A->awbBgain = awbBgain;

	return  CAM_ERROR_NONE;

}

/******************************************************************************************************
//
// Name:         _ov5640_calc_new_3A
// Description:  calculate capture exposure from preview exposure
// Arguments:    p3Astat[IN/OUT] : preview's AEC paras(exposure\gain\maxlines)
// Return:       camera engine error code,refer to CameraEngine.h for more detail
// Notes:        
//     1) for more detail about OV5640 register, pls refer to its software application note
// Version Log:  version      Date          Author    Description
//                v0.1      07/14/1010    Matrix Yao   Create
//                v0.2      07/30/2010    Matrix Yao   rename function,etc
*******************************************************************************************************/

static CAM_Error _ov5640_calc_new_3A( CAM_Int32s iSensorFD, _OV5640_3AState *p3Astat )
{
	CAM_Int16u  reg = 0;

	// line_10ms is Banding Filter Value
	// here 10ms means the time unit is 10 mili-seconds
	CAM_Int32u lines_10ms        = 0;
	CAM_Int32u newMaxLines       = 0;

	CAM_Int64u newGainQ4         = 0;
	CAM_Int64u newExposure       = 0;
	CAM_Int64u newExposureGain   = 0;


	// stop AEC/AGC
	reg = 0x0007;
	_set_sensor_reg( iSensorFD, OV5640_REG_AECAGC, reg );

	// get capture VTS
	reg = 0;
	_get_sensor_reg( iSensorFD, OV5640_REG_VTSHIGH, &reg );
	newMaxLines = ( reg & 0x00ff );
	newMaxLines <<= 8;

	reg = 0;
	_get_sensor_reg( iSensorFD, OV5640_REG_VTSLOW, &reg );
	newMaxLines += ( reg & 0x00ff );

	// get capture's banding filter value, 50Hz flicker as example
	// !!!FIXME
	lines_10ms = Capture_Framerate * newMaxLines / 10000;


	if ( p3Astat->maxLines == 0 )
	{
		p3Astat->maxLines = 1;
	}

	if ( lines_10ms == 0 ) 
	{
		lines_10ms = 1;
	}

	/*
	if ( p3Astat->maxLines == 0 || lines_10ms == 0 )
	{
		return CAM_ERROR_NOTSUPPORTEDARG;
	}
	*/

	// calculate capture exposure, in uint of t_ROW_INTERVAL 
	newExposure = (p3Astat->exposure) * (Capture_Framerate) * newMaxLines / p3Astat->maxLines / Preview_FrameRate;

	// calculate exposure * gain for capture mode
	newExposureGain = newExposure * p3Astat->gainQ4;

	// get exposure for capture. 
	if ( newExposureGain < ((CAM_Int64u)newMaxLines << 4) ) 
	{
		newExposure = ( newExposureGain >> 4 );
		if ( newExposure > lines_10ms ) 
		{
			newExposure /= lines_10ms;
			newExposure *= lines_10ms;
		}
	}
	else 
	{
		newExposure = newMaxLines;
	}	

	if ( newExposure == 0 ) 
	{
		newExposure = 1;
	}

	// get gain for capture
	newGainQ4 = (newExposureGain * 2 / newExposure + 1) / 2;

	
	// update 3A status
	p3Astat->maxLines = newMaxLines;
	p3Astat->exposure = (CAM_Int16u)newExposure;
	p3Astat->gainQ4 = (CAM_Int16u)newGainQ4;

	// AWB no need to update
	return  CAM_ERROR_NONE;

}


/******************************************************************************************************
//
// Name:         _ov5640_set_new_exposure
// Description:  set capture exposure(exposure\gain\maxlines) manually
// Arguments:    pNew3A[INPUT] : capture's AEC paras(exposure\gain\maxlines)
// Return:       camera engine error code,refer to CameraEngine.h for more detail
// Notes:        
//     1) for more detail about OV5640 register, pls refer to its software application note;
//     2) pls update current AEC state once this function return CAM_ERROR_NONE;
// Version Log:  version      Date          Author    Description
//                v0.1      07/14/1010    Matrix Yao   Create
//                v0.2      07/30/2010    Matrix Yao   rename function,etc 
*******************************************************************************************************/

static CAM_Error _ov5640_set_new_exposure( CAM_Int32s iSensorFD,_OV5640_3AState *pNew3A )
{
	CAM_Int8u  exposureLow  = 0;
	CAM_Int8u  exposureMid  = 0;
	CAM_Int8u  exposureHigh = 0;

	CAM_Int8u  gain         = 0;

	CAM_Int16u gainQ4       = 0;  
	CAM_Int16u reg          = 0;

	CAM_Int32s  ret         = 0;            
	// calculate exposure value for OV5640 register
	exposureLow  = (CAM_Int8u)(((pNew3A->exposure) & 0x000f) << 4);
	exposureMid  = (CAM_Int8u)(((pNew3A->exposure) & 0x0ff0) >> 4);
	exposureHigh = (CAM_Int8u)(((pNew3A->exposure) & 0xf000) >> 12);

	// calculate gain value for OV5640 register
	gainQ4 = pNew3A->gainQ4;

	if ( gainQ4 > 31 )
	{
		gain   |= 0x10;
		gainQ4 >>= 1; 
	}

	if ( gainQ4 > 31 )
	{
		gain   |= 0x20;
		gainQ4 >>= 1; 
	}

	if ( gainQ4 > 31 )
	{
		gain   |= 0x40;
		gainQ4 >>= 1; 
	}

	if ( gainQ4 > 31 )
	{
		gain   |= 0x80;
		gainQ4 >>= 1; 
	}

	if ( gainQ4 > 16 )
	{
		gain |= ( ( gainQ4 -16 ) & 0x0f );
	}  

	// I still don't know the real meaning of the following statement 
	if ( gain == 0x10 )
	{
		gain = 0x11;
	}  

	// write gain and exposure to OV5640 registers

	/* gain */
	reg = (CAM_Int16u)gain;
	ret = _set_sensor_reg( iSensorFD, OV5640_REG_GAINLOW, reg );
	if ( ret )
	{
		return CAM_ERROR_DRIVEROPFAILED;
	}

	/* exposure */
	reg = (CAM_Int16u)exposureLow;
	ret = _set_sensor_reg( iSensorFD, OV5640_REG_EXPOSURELOW, reg );

	reg = (CAM_Int16u)exposureMid;
	ret = _set_sensor_reg( iSensorFD, OV5640_REG_EXPOSUREMID, reg );

	reg = (CAM_Int16u)exposureHigh;
	ret = _set_sensor_reg( iSensorFD, OV5640_REG_EXPOSUREHIGH, reg );

	/* AWB */

	reg = 0x0083;
	ret = _set_sensor_reg( iSensorFD, OV5640_REG_AWBC_22, reg );

	reg = ( pNew3A->awbRgain & 0x0f00 );
	reg = (reg >> 8);
	ret = _set_sensor_reg( iSensorFD, OV5640_REG_SET_AWBRHIGH, reg );
	reg = (pNew3A->awbRgain & 0x00ff);
	ret = _set_sensor_reg( iSensorFD, OV5640_REG_SET_AWBRLOW, reg );

	reg = (pNew3A->awbGgain & 0x0f00);
	reg = (reg >> 8);
	ret = _set_sensor_reg( iSensorFD, OV5640_REG_SET_AWBGHIGH, reg );
	reg = (pNew3A->awbGgain & 0x00ff);
	ret = _set_sensor_reg( iSensorFD, OV5640_REG_SET_AWBGLOW, reg );

	reg = (pNew3A->awbBgain & 0x0f00);
	reg = (reg >> 8);
	ret = _set_sensor_reg( iSensorFD, OV5640_REG_SET_AWBBHIGH, reg );
	reg = (pNew3A->awbBgain & 0x00ff);
	ret = _set_sensor_reg( iSensorFD, OV5640_REG_SET_AWBBLOW, reg );


	return  CAM_ERROR_NONE;
}

static CAM_Error _OV5640SaveAeAwb( const _CAM_SmartSensorConfig *pOldCfg, void *pUserData )
{
	CAM_Error   error   = CAM_ERROR_NONE;
	OV5640State *pState = (OV5640State*)pUserData;

	if ( pOldCfg->eState != CAM_CAPTURESTATE_PREVIEW )
	{
		return CAM_ERROR_NONE;
	}

	//error = _ov5640_save_ae_awb( pState->stV4L2.iSensorFD, &pState->st3Astat );

	return error;
}

static CAM_Error _OV5640RestoreAeAwb( const _CAM_SmartSensorConfig *pNewCfg, void *pUserData )
{
	CAM_Error   error   = CAM_ERROR_NONE;
	OV5640State *pState = (OV5640State*)pUserData;

	if ( pNewCfg->eState != CAM_CAPTURESTATE_STILL )
	{
		return CAM_ERROR_NONE;
	}

	//error = _ov5640_calc_new_3A( pState->stV4L2.iSensorFD, &pState->st3Astat );
	if ( error != CAM_ERROR_NONE )
	{
		return error;
	}

	//error = _ov5640_set_new_exposure( pState->stV4L2.iSensorFD, &pState->st3Astat );
	return error;
}

static CAM_Error _OV5640StartFlash( void *pV4l2State )
{
	//TODO: add your sensor specific start flash function here
	return CAM_ERROR_NONE;
}

static CAM_Error _OV5640StopFlash( void *pV4l2State )
{
	//TODO: add your sensor specific stop flash function here
	return CAM_ERROR_NONE;
}

// get shot info
static CAM_Error _OV5640FillFrameShotInfo( OV5640State *pState, CAM_ImageBuffer *pImgBuf )
{
	// TODO: add real value of these parameters here

	CAM_Error    error      = CAM_ERROR_NONE;
	CAM_ShotInfo *pShotInfo = &(pImgBuf->stShotInfo);

	pShotInfo->iExposureTimeQ16    = (1 << 16) / 30;
	pShotInfo->iFNumQ16            = (int)( 2.8 * 65536 + 0.5 );
	pShotInfo->eExposureMode       = CAM_EXPOSUREMODE_AUTO;
	pShotInfo->eExpMeterMode       = CAM_EXPOSUREMETERMODE_MEAN;
	pShotInfo->iISOSpeed           = 100;
	pShotInfo->iSubjectDistQ16     = 0;
	pShotInfo->iFlashStatus        = 0x0010;
	pShotInfo->iFocalLenQ16        = (int)( 3.79 * 65536 + 0.5 );
	pShotInfo->iFocalLen35mm       = 0;       

	return error;
}
