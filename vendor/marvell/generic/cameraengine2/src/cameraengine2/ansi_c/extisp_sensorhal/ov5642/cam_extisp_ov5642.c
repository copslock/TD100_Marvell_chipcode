/*******************************************************************************
//(C) Copyright [2010 - 2011] Marvell International Ltd.
//All Rights Reserved
*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "misc.h"
#include "ippIP.h"

#include "cam_log.h"
#include "cam_utility.h"

#include "cam_extisp_sensorwrapper.h"
#include "cam_extisp_v4l2base.h"
#include "cam_extisp_ov5642.h"

// sensor vendor deliverable
#include "FaceTrack.h"

/* sensor capability tables */

// NOTE: if you want to enable static resolution table to bypass sensor dynamically detect to save camera-off to viwerfinder-on latency, 
//       you can fill the following four tables according to your sensor's capability. And open macro 
//       BUILD_OPTION_STARTEGY_DISABLE_DYNAMIC_SENSOR_DETECT in cam_extisp_buildopt.h

// video resolution
CAM_Size _OV5642VideoResTable[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT] = 
{
	{0, 0},
};

// still resolution
CAM_Size _OV5642StillResTable[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT] = 
{
	{0, 0},
};

// supported video formats
CAM_ImageFormat _OV5642VideoFormatTable[CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT] = 
{
	0,
};

// supported still formats
CAM_ImageFormat _OV5642StillFormatTable[CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT] =
{
	0,
};


// supported flip/mirror/rotate styles
_CAM_RegEntry _OV5642Flip_None[] = 
{
	{ 0x3818,5,6,5, 0x00 },
};

_CAM_RegEntry _OV5642Flip_Mirror[] = 
{
	{ 0x3818,5,6,5, 0x40 },
};

_CAM_RegEntry _OV5642Flip_Flip[] = 
{
	{ 0x3818,5,6,5, 0x20 },
};

_CAM_RegEntry _OV5642Flip_FM[] = 
{
	{ 0x3818,5,6,5, 0x60 },
};

_CAM_ParameterEntry _OV5642FlipRotation[] = 
{
	PARAM_ENTRY_USE_REGTABLE( CAM_FLIPROTATE_NORMAL, _OV5642Flip_None ),
	PARAM_ENTRY_USE_REGTABLE( CAM_FLIPROTATE_HFLIP,  _OV5642Flip_Mirror ),
	PARAM_ENTRY_USE_REGTABLE( CAM_FLIPROTATE_VFLIP,  _OV5642Flip_Flip ),
	PARAM_ENTRY_USE_REGTABLE( CAM_FLIPROTATE_180,    _OV5642Flip_FM ),
};

/* shooting parameters  */

// shot mode
_CAM_RegEntry _OV5642ShotMode_Auto[] = 
{
	{ 0x3a05,0,7,0, 0x30 }, 
	{ 0x3a14,0,7,0, 0x03 }, 
	{ 0x3a15,0,7,0, 0x75 },
	{ 0x3a00,0,7,0, 0x78 }, 
};

_CAM_RegEntry _OV5642ShotMode_Portrait[] = 
{
	{ 0x3a05,0,7,0, 0x30 }, 
	{ 0x3a14,0,7,0, 0x03 }, 
	{ 0x3a15,0,7,0, 0x75 },
	{ 0x3a00,0,7,0, 0x78 }, 
};

_CAM_RegEntry _OV5642ShotMode_Night[] = 
{
	{ 0x3a05,0,7,0, 0x30}, 
	{ 0x3a02,0,7,0, 0x00},
	{ 0x3a03,0,7,0, 0xbb},
	{ 0x3a04,0,7,0, 0x80},
	{ 0x3a14,0,7,0, 0x00}, 
	{ 0x3a15,0,7,0, 0xbb},
	{ 0x3a16,0,7,0, 0x80},
	{ 0x3a00,0,7,0, 0x7c},
};

_CAM_ParameterEntry _OV564ShotMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_SHOTMODE_AUTO,       _OV5642ShotMode_Auto),
//	PARAM_ENTRY_USE_REGTABLE(CAM_SHOTMODE_PORTRAIT,   _OV5642ShotMode_Portrait),
	PARAM_ENTRY_USE_REGTABLE(CAM_SHOTMODE_NIGHTSCENE, _OV5642ShotMode_Night),
};

// exposure metering mode
_CAM_RegEntry _OV5642ExpMeter_Mean[] = 
{
	{ OV5642_REG_EM_WIN0WEI, 0x01},
	{ OV5642_REG_EM_WIN1WEI, 0x10}, 
	{ OV5642_REG_EM_WIN2WEI, 0x01}, 
	{ OV5642_REG_EM_WIN3WEI, 0x10}, 
	{ OV5642_REG_EM_WIN4WEI, 0x01},
	{ OV5642_REG_EM_WIN5WEI, 0x10},
	{ OV5642_REG_EM_WIN6WEI, 0x01},
	{ OV5642_REG_EM_WIN7WEI, 0x10},
	{ OV5642_REG_EM_WIN8WEI, 0x01},
	{ OV5642_REG_EM_WIN9WEI, 0x10},
	{ OV5642_REG_EM_WIN10WEI,0x01},
	{ OV5642_REG_EM_WIN11WEI,0x10},
	{ OV5642_REG_EM_WIN12WEI,0x01},
	{ OV5642_REG_EM_WIN13WEI,0x10},
	{ OV5642_REG_EM_WIN14WEI,0x01},
	{ OV5642_REG_EM_WIN15WEI,0x10},
};

// TODO: this spot metering reference take 1920*1080 as the whole image window.
//       if you want smaller image window's spot metering, pls contact
//       with OVT
_CAM_RegEntry _OV5642ExpMeter_Spot[] = 
{
	{ OV5642_REG_WIN_XSTART_LOW, 0x08},  
	{ OV5642_REG_WIN_XSTART_HIGH, 0x02}, 
	{ OV5642_REG_WIN_XEND_LOW, 0xf8},    
	{ OV5642_REG_WIN_XEND_HIGH, 0x02},   
	{ OV5642_REG_WIN_YSTART_LOW,0x86},  
	{ OV5642_REG_WIN_YSTART_HIGH,0x01}, 
	{ OV5642_REG_WIN_YEND_LOW, 0x3a},    
	{ OV5642_REG_WIN_YEND_HIGH, 0x02},   
	{ OV5642_REG_EM_WIN0WEI, 0x01},
	{ OV5642_REG_EM_WIN1WEI, 0x10}, 
	{ OV5642_REG_EM_WIN2WEI, 0x01}, 
	{ OV5642_REG_EM_WIN3WEI, 0x10}, 
	{ OV5642_REG_EM_WIN4WEI,0x01},
	{ OV5642_REG_EM_WIN5WEI,0x10},
	{ OV5642_REG_EM_WIN6WEI,0x01},
	{ OV5642_REG_EM_WIN7WEI,0x10},
	{ OV5642_REG_EM_WIN8WEI,0x01},
	{ OV5642_REG_EM_WIN9WEI,0x10},
	{ OV5642_REG_EM_WIN10WEI,0x01},
	{ OV5642_REG_EM_WIN11WEI,0x10},
	{ OV5642_REG_EM_WIN12WEI,0x01},
	{ OV5642_REG_EM_WIN13WEI,0x10},
	{ OV5642_REG_EM_WIN14WEI,0x01},
	{ OV5642_REG_EM_WIN15WEI,0x10},

};

_CAM_RegEntry _OV5642ExpMeter_CW[] = 
{
	{ OV5642_REG_EM_WIN0WEI, 0x02},
	{ OV5642_REG_EM_WIN1WEI, 0x60}, 
	{ OV5642_REG_EM_WIN2WEI, 0x06}, 
	{ OV5642_REG_EM_WIN3WEI, 0x20}, 
	{ OV5642_REG_EM_WIN4WEI,0x06},
	{ OV5642_REG_EM_WIN5WEI,0xe0},
	{ OV5642_REG_EM_WIN6WEI,0x0e},
	{ OV5642_REG_EM_WIN7WEI,0x60},
	{ OV5642_REG_EM_WIN8WEI,0x0a},
	{ OV5642_REG_EM_WIN9WEI,0xe0},
	{ OV5642_REG_EM_WIN10WEI,0x0e},
	{ OV5642_REG_EM_WIN11WEI,0xa0},
	{ OV5642_REG_EM_WIN12WEI,0x06},
	{ OV5642_REG_EM_WIN13WEI,0xa0},
	{ OV5642_REG_EM_WIN14WEI,0x0a},
	{ OV5642_REG_EM_WIN15WEI,0x60},
};

_CAM_ParameterEntry _OV5642ExpMeterMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_EXPOSUREMETERMODE_AUTO, _OV5642ExpMeter_Mean),
	PARAM_ENTRY_USE_REGTABLE(CAM_EXPOSUREMETERMODE_MEAN, _OV5642ExpMeter_Mean),
	PARAM_ENTRY_USE_REGTABLE(CAM_EXPOSUREMETERMODE_SPOT, _OV5642ExpMeter_Spot),
	PARAM_ENTRY_USE_REGTABLE(CAM_EXPOSUREMETERMODE_CENTRALWEIGHTED, _OV5642ExpMeter_CW),
};

// ISO setting
_CAM_RegEntry _OV5642IsoMode_Auto[] = 
{
	{OV5642_REG_SET_ISOLOW, 0x7c},
};

_CAM_RegEntry _OV5642IsoMode_100[] = 
{
	{OV5642_REG_SET_ISOHIGH, 0x00},
	{OV5642_REG_SET_ISOLOW, 0x1f},
};

_CAM_RegEntry _OV5642IsoMode_200[] = 
{
	{OV5642_REG_SET_ISOHIGH, 0x00},
	{OV5642_REG_SET_ISOLOW, 0x3e},
};

_CAM_RegEntry _OV5642IsoMode_400[] = 
{
	{OV5642_REG_SET_ISOHIGH, 0x00},
	{OV5642_REG_SET_ISOLOW, 0x7c},
};

_CAM_RegEntry _OV5642IsoMode_800[] = 
{
	{OV5642_REG_SET_ISOHIGH, 0x00},
	{OV5642_REG_SET_ISOLOW, 0xf8},
};

_CAM_RegEntry _OV5642IsoMode_1600[] = 
{
	{OV5642_REG_SET_ISOHIGH, 0x01},
	{OV5642_REG_SET_ISOLOW, 0xf0},
};

_CAM_ParameterEntry _OV5642IsoMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_ISO_AUTO, _OV5642IsoMode_Auto),
	PARAM_ENTRY_USE_REGTABLE(CAM_ISO_100, _OV5642IsoMode_100),
	PARAM_ENTRY_USE_REGTABLE(CAM_ISO_200, _OV5642IsoMode_200),
	PARAM_ENTRY_USE_REGTABLE(CAM_ISO_400, _OV5642IsoMode_400),
	PARAM_ENTRY_USE_REGTABLE(CAM_ISO_800, _OV5642IsoMode_800),
	PARAM_ENTRY_USE_REGTABLE(CAM_ISO_1600,_OV5642IsoMode_1600),
};

// band filter setting, relative with sensor clock frequency
_CAM_RegEntry _OV5642BdFltMode_Auto[] = 
{
	{OV5642_REG_BF_CTL,0x20},
	{0x3623, 0, 7, 0, 0x01},
	{0x3630, 0, 7, 0, 0x24},
	{0x3633, 0, 7, 0, 0x00},
	{0x3c00, 0, 7, 0, 0x00},
	{0x3c01, 0, 7, 0, 0x34},
	{0x3c04, 0, 7, 0, 0x28},
	{0x3c05, 0, 7, 0, 0x98},
	{0x3c06, 0, 7, 0, 0x00},
	{0x3c07, 0, 7, 0, 0x07},
	{0x3c08, 0, 7, 0, 0x01},
	{0x3c09, 0, 7, 0, 0xc2},
	{0x300d, 0, 7, 0, 0x02},
	{0x3104, 0, 7, 0, 0x01},
	{0x3c0a, 0, 7, 0, 0x4e},
	{0x3c0b, 0, 7, 0, 0x1f},
};

_CAM_RegEntry _OV5642BdFltMode_Off[] = 
{
	{OV5642_REG_BF_CTL, 0x00},
};

_CAM_RegEntry _OV5642BdFltMode_50Hz[] = 
{
	{OV5642_REG_BF_CTL, 0x20},
	{0x3c01, 7, 7, 7, 0x80},
	{0x3c00, 0, 7, 0, 0x04},
};

_CAM_RegEntry _OV5642BdFltMode_60Hz[] = 
{
	{OV5642_REG_BF_CTL, 0x20},
	{0x3c01, 7, 7, 7, 0x80},
	{0x3c00, 0, 7, 0, 0x00},
};

_CAM_ParameterEntry _OV5642BdFltMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_BANDFILTER_AUTO, _OV5642BdFltMode_Auto),
	PARAM_ENTRY_USE_REGTABLE(CAM_BANDFILTER_OFF,  _OV5642BdFltMode_Off),
	PARAM_ENTRY_USE_REGTABLE(CAM_BANDFILTER_50HZ, _OV5642BdFltMode_50Hz),
	PARAM_ENTRY_USE_REGTABLE(CAM_BANDFILTER_60HZ, _OV5642BdFltMode_60Hz),
};


// white balance mode
_CAM_RegEntry _OV5642WBMode_Auto[] = 
{
	{OV5642_REG_WB_CTL, 0x00},
};

_CAM_RegEntry _OV5642WBMode_Incandescent[] = 
{
	{OV5642_REG_WB_CTL,   0x01},
	{OV5642_REG_WB_RHIGH, 0x04},
	{OV5642_REG_WB_RLOW,  0x88},
	{OV5642_REG_WB_GHIGH, 0x04},
	{OV5642_REG_WB_GLOW,  0x00},
	{OV5642_REG_WB_BHIGH, 0x08},
	{OV5642_REG_WB_BLOW,  0xb6},
};

_CAM_RegEntry _OV5642WBMode_CoolFluorescent[] = 
{
	{OV5642_REG_WB_CTL,0x01},
	{OV5642_REG_WB_RHIGH,0x06},
	{OV5642_REG_WB_RLOW,0x2a},
	{OV5642_REG_WB_GHIGH,0x04},
	{OV5642_REG_WB_GLOW,0x00},
	{OV5642_REG_WB_BHIGH,0x07},
	{OV5642_REG_WB_BLOW,0x24},

};

_CAM_RegEntry _OV5642WBMode_Daylight[] = 
{
	{OV5642_REG_WB_CTL,0x01},
	{OV5642_REG_WB_RHIGH,0x07},
	{OV5642_REG_WB_RLOW,0x02},
	{OV5642_REG_WB_GHIGH,0x04},
	{OV5642_REG_WB_GLOW,0x00},
	{OV5642_REG_WB_BHIGH,0x05},
	{OV5642_REG_WB_BLOW,0x15},
};

_CAM_RegEntry _OV5642WBMode_Cloudy[] = 
{
	{OV5642_REG_WB_CTL,0x01},
	{OV5642_REG_WB_RHIGH,0x07},
	{OV5642_REG_WB_RLOW,0x88},
	{OV5642_REG_WB_GHIGH,0x04},
	{OV5642_REG_WB_GLOW,0x00},
	{OV5642_REG_WB_BHIGH,0x05},
	{OV5642_REG_WB_BLOW,0x00},
};

_CAM_ParameterEntry _OV5642WBMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_WHITEBALANCEMODE_AUTO, _OV5642WBMode_Auto),
	PARAM_ENTRY_USE_REGTABLE(CAM_WHITEBALANCEMODE_INCANDESCENT, _OV5642WBMode_Incandescent),
	PARAM_ENTRY_USE_REGTABLE(CAM_WHITEBALANCEMODE_COOLWHITE_FLUORESCENT, _OV5642WBMode_CoolFluorescent),
	PARAM_ENTRY_USE_REGTABLE(CAM_WHITEBALANCEMODE_DAYLIGHT,	_OV5642WBMode_Daylight),
	PARAM_ENTRY_USE_REGTABLE(CAM_WHITEBALANCEMODE_CLOUDY,	_OV5642WBMode_Cloudy),
};


// color effect mode
_CAM_RegEntry _OV5642ColorEffectMode_Off[] = 
{
	{OV5642_REG_SDE_CTL, 0x00},
	{OV5642_REG_SAT_CTL,0x00},
	{OV5642_REG_TONE_CTL,0x00},
	{OV5642_REG_NEG_CTL, 0x00},
	{OV5642_REG_SOLARIZE_CTL, 0x00},
};

_CAM_RegEntry _OV5642ColorEffectMode_Vivid[] = 
{
	{OV5642_REG_SDE_CTL, 0x80},
	{OV5642_REG_SAT_CTL,0x02},
	{OV5642_REG_TONE_CTL,0x00},
	{OV5642_REG_GRAY_CTL, 0x00},
	{OV5642_REG_NEG_CTL, 0x00},
	{OV5642_REG_SOLARIZE_CTL, 0x00},
	{OV5642_REG_SAT_U,  0x70},
	{OV5642_REG_SAT_V,  0x70},
};

_CAM_RegEntry _OV5642ColorEffectMode_Sepia[] =
{
	{OV5642_REG_SDE_CTL, 0x80},
	{OV5642_REG_TONE_CTL,0x18},
	{OV5642_REG_NEG_CTL, 0x00},
	{OV5642_REG_SOLARIZE_CTL, 0x00},
	{OV5642_REG_U_FIX,0x40},
	{OV5642_REG_V_FIX,0xa0},
};

_CAM_RegEntry _OV5642ColorEffectMode_Grayscale[] = 
{
	{OV5642_REG_SDE_CTL, 0x80},
	{OV5642_REG_TONE_CTL,0x18},
	{OV5642_REG_NEG_CTL, 0x00},
	{OV5642_REG_SAT_CTL,0x00},
	{OV5642_REG_SOLARIZE_CTL, 0x00},
	{OV5642_REG_U_FIX,0x80},
	{OV5642_REG_V_FIX,0x80},
};


_CAM_RegEntry _OV5642ColorEffectMode_Negative[] = 
{
	{OV5642_REG_SDE_CTL, 0x80},
	{OV5642_REG_NEG_CTL, 0x40},
	{OV5642_REG_TONE_CTL,0x00},
	{OV5642_REG_SAT_CTL,0x00},
	{OV5642_REG_SOLARIZE_CTL, 0x00},
};

_CAM_RegEntry _OV5642ColorEffectMode_Solarize[] = 
{
	{OV5642_REG_SDE_CTL, 0x80},
	{OV5642_REG_NEG_CTL, 0x00},
	{OV5642_REG_TONE_CTL,0x00},
	{OV5642_REG_SAT_CTL,0x00},
	{OV5642_REG_SOLARIZE_CTL, 0x80},
};


_CAM_ParameterEntry _OV5642ColorEffectMode[] =
{
	PARAM_ENTRY_USE_REGTABLE(CAM_COLOREFFECT_OFF, _OV5642ColorEffectMode_Off),
	PARAM_ENTRY_USE_REGTABLE(CAM_COLOREFFECT_VIVID, _OV5642ColorEffectMode_Vivid),
	PARAM_ENTRY_USE_REGTABLE(CAM_COLOREFFECT_SEPIA, _OV5642ColorEffectMode_Sepia),
	PARAM_ENTRY_USE_REGTABLE(CAM_COLOREFFECT_GRAYSCALE, _OV5642ColorEffectMode_Grayscale),
	PARAM_ENTRY_USE_REGTABLE(CAM_COLOREFFECT_NEGATIVE, _OV5642ColorEffectMode_Negative),	
	PARAM_ENTRY_USE_REGTABLE(CAM_COLOREFFECT_SOLARIZE, _OV5642ColorEffectMode_Solarize),	
};

// EV compensation
_CAM_RegEntry _OV5642EvComp_N12[] = 
{
	{OV5642_REG_AEC_ENT_HIGH, 0x18},
	{OV5642_REG_AEC_ENT_LOW,0x10},
	{OV5642_REG_AEC_OUT_HIGH,0x18},
	{OV5642_REG_AEC_OUT_LOW,0x10},
	{OV5642_REG_AEC_VPT_HIGH,0x30},
	{OV5642_REG_AEC_VPT_LOW,0x10},
};

_CAM_RegEntry _OV5642EvComp_N09[] = 
{
	{OV5642_REG_AEC_ENT_HIGH, 0x20},
	{OV5642_REG_AEC_ENT_LOW,0x18},
	{OV5642_REG_AEC_OUT_HIGH,0x20},
	{OV5642_REG_AEC_OUT_LOW,0x18},
	{OV5642_REG_AEC_VPT_HIGH,0x41},
	{OV5642_REG_AEC_VPT_LOW,0x10},
};

_CAM_RegEntry _OV5642EvComp_N06[] = 
{
	{OV5642_REG_AEC_ENT_HIGH, 0x28},
	{OV5642_REG_AEC_ENT_LOW,0x20},
	{OV5642_REG_AEC_OUT_HIGH,0x28},
	{OV5642_REG_AEC_OUT_LOW,0x20},
	{OV5642_REG_AEC_VPT_HIGH,0x51},
	{OV5642_REG_AEC_VPT_LOW,0x10},
};

_CAM_RegEntry _OV5642EvComp_N03[] = 
{
	{OV5642_REG_AEC_ENT_HIGH, 0x30},
	{OV5642_REG_AEC_ENT_LOW,0x28},
	{OV5642_REG_AEC_OUT_HIGH,0x30},
	{OV5642_REG_AEC_OUT_LOW,0x28},
	{OV5642_REG_AEC_VPT_HIGH,0x61},
	{OV5642_REG_AEC_VPT_LOW,0x10},
};

_CAM_RegEntry _OV5642EvComp_000[] = 
{
	{OV5642_REG_AEC_ENT_HIGH, 0x38},
	{OV5642_REG_AEC_ENT_LOW,0x30},
	{OV5642_REG_AEC_OUT_HIGH,0x38},
	{OV5642_REG_AEC_OUT_LOW,0x30},
	{OV5642_REG_AEC_VPT_HIGH,0x61},
	{OV5642_REG_AEC_VPT_LOW,0x10},
};

_CAM_RegEntry _OV5642EvComp_P03[] = 
{
	{OV5642_REG_AEC_ENT_HIGH, 0x40},
	{OV5642_REG_AEC_ENT_LOW,0x38},
	{OV5642_REG_AEC_OUT_HIGH,0x40},
	{OV5642_REG_AEC_OUT_LOW,0x38},
	{OV5642_REG_AEC_VPT_HIGH,0x71},
	{OV5642_REG_AEC_VPT_LOW,0x10},
};

_CAM_RegEntry _OV5642EvComp_P06[] = 
{
	{OV5642_REG_AEC_ENT_HIGH, 0x48},
	{OV5642_REG_AEC_ENT_LOW,0x40},
	{OV5642_REG_AEC_OUT_HIGH,0x48},
	{OV5642_REG_AEC_OUT_LOW,0x40},
	{OV5642_REG_AEC_VPT_HIGH,0x80},
	{OV5642_REG_AEC_VPT_LOW,0x20},
};

_CAM_RegEntry _OV5642EvComp_P09[] = 
{
	{OV5642_REG_AEC_ENT_HIGH, 0x50},
	{OV5642_REG_AEC_ENT_LOW,0x48},
	{OV5642_REG_AEC_OUT_HIGH,0x50},
	{OV5642_REG_AEC_OUT_LOW,0x48},
	{OV5642_REG_AEC_VPT_HIGH,0x90},
	{OV5642_REG_AEC_VPT_LOW,0x20},
};

_CAM_RegEntry _OV5642EvComp_P12[] = 
{
	{OV5642_REG_AEC_ENT_HIGH, 0x58},
	{OV5642_REG_AEC_ENT_LOW,0x50},
	{OV5642_REG_AEC_OUT_HIGH,0x58},
	{OV5642_REG_AEC_OUT_LOW,0x50},
	{OV5642_REG_AEC_VPT_HIGH,0x91},
	{OV5642_REG_AEC_VPT_LOW,0x20},
};

_CAM_ParameterEntry _OV5642EvCompMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(-78644,  _OV5642EvComp_N12),
	PARAM_ENTRY_USE_REGTABLE(-58983,  _OV5642EvComp_N09),
	PARAM_ENTRY_USE_REGTABLE(-39322,  _OV5642EvComp_N06),
	PARAM_ENTRY_USE_REGTABLE(-19661,  _OV5642EvComp_N03),
	PARAM_ENTRY_USE_REGTABLE(0,       _OV5642EvComp_000),
	PARAM_ENTRY_USE_REGTABLE(19661,   _OV5642EvComp_P03),
	PARAM_ENTRY_USE_REGTABLE(39322,	  _OV5642EvComp_P06),
	PARAM_ENTRY_USE_REGTABLE(58983,	  _OV5642EvComp_P09),
	PARAM_ENTRY_USE_REGTABLE(78644,	  _OV5642EvComp_P12),
};


// focus mode
_CAM_RegEntry _OV5642FocusMode_AutoOneShot[] =
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _OV5642FocusMode_AutoContinuous[] =
{
	{0, 0, 0, 0, 0},
};


_CAM_RegEntry _OV5642FocusMode_Macro[] =
{
	{OV5642_REG_AF_TAG,  0x04},
	{OV5642_REG_AF_MAIN, 0x05},
};

_CAM_RegEntry _OV5642FocusMode_Infinity[] =
{
	{OV5642_REG_AF_TAG,  0x03},
	{OV5642_REG_AF_MAIN, 0x05},
};

_CAM_ParameterEntry _OV5642FocusMode[] = 
{
	PARAM_ENTRY_USE_REGTABLE(CAM_FOCUS_INFINITY,            _OV5642FocusMode_Infinity),
#if defined( BUILD_OPTION_STRATEGY_ENABLE_AUTOFOCUS )
	PARAM_ENTRY_USE_REGTABLE(CAM_FOCUS_AUTO_ONESHOT_CENTER, _OV5642FocusMode_AutoOneShot),
	PARAM_ENTRY_USE_REGTABLE(CAM_FOCUS_AUTO_ONESHOT_TOUCH,  _OV5642FocusMode_AutoOneShot),
//	PARAM_ENTRY_USE_REGTABLE(CAM_FOCUS_AUTO_CONTINUOUS_FACE,_OV5642FocusMode_AutoContinuous),
	PARAM_ENTRY_USE_REGTABLE(CAM_FOCUS_MACRO,               _OV5642FocusMode_Macro),
#endif
};

// focus zone
_CAM_RegEntry _OV5642FocusZone_Center[] = 
{
	{OV5642_REG_AF_TAG,  0x01},
	{OV5642_REG_AF_MAIN, 0x10},
	{OV5642_REG_AF_MAIN, 0x03},
};

_CAM_RegEntry _OV5642FocusZone_Multizone1[] =
{
	// 5x zone
	{OV5642_REG_AF_TAG,  0x02},
	{OV5642_REG_AF_MAIN, 0x10},
	{OV5642_REG_AF_MAIN, 0x03},
};

_CAM_RegEntry _OV5642FocusZone_Multizone2[] =
{
	// 5+ zone
	{OV5642_REG_AF_TAG,  0x03},
	{OV5642_REG_AF_MAIN, 0x10},
	{OV5642_REG_AF_MAIN, 0x03},
};

_CAM_RegEntry _OV5642FocusZone_Manual[] =
{
	// manual zone
	{OV5642_REG_AF_TAG,  0x80},
	{OV5642_REG_AF_MAIN, 0x01},
//	{OV5642_REG_AF_MAIN, 0x03},
};


_CAM_ParameterEntry _OV5642FocusZone[] = 
{
#if defined( BUILD_OPTION_STRATEGY_ENABLE_AUTOFOCUS )
	PARAM_ENTRY_USE_REGTABLE(CAM_FOCUSZONE_CENTER,     _OV5642FocusZone_Center),
	PARAM_ENTRY_USE_REGTABLE(CAM_FOCUSZONE_MULTIZONE1, _OV5642FocusZone_Multizone1),
	PARAM_ENTRY_USE_REGTABLE(CAM_FOCUSZONE_MULTIZONE2, _OV5642FocusZone_Multizone2),
	PARAM_ENTRY_USE_REGTABLE(CAM_FOCUSZONE_MANUAL,     _OV5642FocusZone_Manual),
#endif
};


// flash Mode
_CAM_RegEntry _OV5642FlashMode_Off[] = 
{
	{OV5642_REG_FLASH_TYPE,  0x03},
	{OV5642_REG_FLASH_ONOFF, 0x00},
};

_CAM_RegEntry _OV5642FlashMode_On[] = 
{
	{OV5642_REG_FLASH_TYPE,  0x03},
	{OV5642_REG_FLASH_ONOFF, 0x80},
};

_CAM_RegEntry _OV5642FlashMode_Torch[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_RegEntry _OV5642FlashMode_Auto[] = 
{
	{0, 0, 0, 0, 0},
};

_CAM_ParameterEntry _OV5642FlashMode[] = 
{
#if defined( BUILD_OPTION_STRATEGY_ENABLE_FLASH )
	PARAM_ENTRY_USE_REGTABLE(CAM_FLASH_OFF,		_OV5642FlashMode_Off),
	PARAM_ENTRY_USE_REGTABLE(CAM_FLASH_ON,		_OV5642FlashMode_On),
	PARAM_ENTRY_USE_REGTABLE(CAM_FLASH_TORCH,	_OV5642FlashMode_Torch),
	PARAM_ENTRY_USE_REGTABLE(CAM_FLASH_AUTO,	_OV5642FlashMode_Auto),
#endif
};


// function declaration
static CAM_Error  _OV5642SaveAeAwb( const _CAM_SmartSensorConfig*, void* );
static CAM_Error  _OV5642RestoreAeAwb( const _CAM_SmartSensorConfig*, void* );
static CAM_Error  _OV5642StartFlash( void* );
static CAM_Error  _OV5642StopFlash( void* );

static void       _OV5642GetShotParamCap( CAM_ShotModeCapability* );
static CAM_Error  _OV5642SetDigitalZoom( OV5642State*, CAM_Int32s );
static CAM_Error  _OV5642FillFrameShotInfo( OV5642State*, CAM_ImageBuffer* );
static CAM_Error  _OV5642SetJpegParam( OV5642State*, CAM_JpegParam* );
static CAM_Error  _OV5642SetBandFilter( CAM_Int32s );
static CAM_Error  _OV5642InitSetting( OV5642State *pState );
// shot mode capability
static void _OV5642AutoModeCap( CAM_ShotModeCapability* );
static void _OV5642PortraitModeCap( CAM_ShotModeCapability* );
static void _OV5642NightModeCap( CAM_ShotModeCapability* );

// face detection
unsigned int      _OV5642FaceDetectCallback( unsigned int value );
static CAM_Int32s FaceDetectionThread( OV5642State *pState );
static CAM_Int32s _YCbCr422ToGrayRsz_8u_C2_C( const Ipp8u *pSrc, int srcStep,  IppiSize srcSize, Ipp8u *pDst, int dstStep, IppiSize dstSize );
static CAM_Error  _YUVToGrayRsz_8u( CAM_ImageBuffer *pImgBuf, CAM_Int8u *pDstBuf, CAM_Size stDstImgSize );

static CAM_Int32s _ov5642_set_paramsreg( CAM_Int32s, _CAM_ParameterEntry*, CAM_Int32s, CAM_Int32s );
static CAM_Int32s _ov5642_set_paramsreg_nearest( CAM_Int32s*, _CAM_ParameterEntry*, CAM_Int32s, CAM_Int32s );
static CAM_Int32s _ov5642_wait_afcmd_ready( CAM_Int32s );
static CAM_Error  _ov5642_set_new_exposure( CAM_Int32s iSensorFD,_OV5642_3AState *pNew3A );
static CAM_Error  _ov5642_save_settings( CAM_Int32s iSensorFD, _OV5642_3AState *pCurrent3A );
static CAM_Error  _ov5642_calc_new3A( CAM_Int32s iSensorFD, _OV5642_3AState *p3Astat );
static CAM_Error  _ov5642_calc_framerate( CAM_Int32s iSensorFD, CAM_Int32s *pFrameRate100 );
static CAM_Error  _ov5642_update_ref_roi( OV5642State *pState );

static inline CAM_Error _ov5642_calc_sysclk( CAM_Int32s iSensorFD, double *pSysClk );
static inline CAM_Error _ov5642_calc_hvts( CAM_Int32s iSensorFD, CAM_Int32s *pHts, CAM_Int32s *pVts );

// shot mode cap function table
OV5642_ShotModeCap _OV5642ShotModeCap[CAM_SHOTMODE_NUM] = { _OV5642AutoModeCap   ,   // CAM_SHOTMODE_AUTO = 0,
                                                           NULL                  ,   // CAM_SHOTMODE_MANUAL,
                                                           _OV5642PortraitModeCap,   // CAM_SHOTMODE_PORTRAIT,
                                                           NULL                  ,   // CAM_SHOTMODE_LANDSCAPE,
                                                           NULL                  ,   // CAM_SHOTMODE_NIGHTPORTRAIT,
                                                           _OV5642NightModeCap   ,   // CAM_SHOTMODE_NIGHTSCENE,
                                                           NULL                  ,   // CAM_SHOTMODE_CHILD,
                                                           NULL                  ,   // CAM_SHOTMODE_INDOOR,
                                                           NULL                  ,   // CAM_SHOTMODE_PLANTS,
                                                           NULL                  ,   // CAM_SHOTMODE_SNOW,
                                                           NULL                  ,   // CAM_SHOTMODE_BEACH,
                                                           NULL                  ,   // CAM_SHOTMODE_FIREWORKS,
                                                           NULL                  ,   // CAM_SHOTMODE_SUBMARINE, 
                                                           NULL                  ,   // CAM_SHOTMODE_WHITEBOARD,
                                                           NULL                  ,   // CAM_SHOTMODE_SPORTS
                                                          };

// NOTE: for OV face detection library, OV face detection lib defines many global variables, even face output is global, I think it's a *BAD* practice
static CAM_Int32s giSensorFD = -1;
/*--------------------------------------------------------------------------------------------------------------------
 * APIs 
 *-------------------------------------------------------------------------------------------------------------------*/
extern _SmartSensorFunc func_ov5642; 
CAM_Error OV5642_SelfDetect( _SmartSensorAttri *pSensorInfo )
{
	CAM_Error error = CAM_ERROR_NONE;
	
	// NOTE:  If you open macro BUILD_OPTION_STARTEGY_DISABLE_DYNAMIC_SENSOR_DETECT in cam_extisp_buildopt.h 
	//        to bypass sensor dynamically detect to save camera-off to viwerfinder-on latency, you should initilize
	//        _OV5642VideoResTable/_OV5642StillResTable/_OV5642VideoFormatTable/_OV5642StillFormatTable manually.

#if !defined( BUILD_OPTION_STARTEGY_DISABLE_DYNAMIC_SENSOR_DETECT )
	error = V4L2_SelfDetect( pSensorInfo, "ov5642", &func_ov5642,
	                         _OV5642VideoResTable, _OV5642StillResTable,
	                         _OV5642VideoFormatTable, _OV5642StillFormatTable );
#else
	{
		_V4L2SensorEntry *pSensorEntry = (_V4L2SensorEntry*)( pSensorInfo->cReserved );
		strcpy( pSensorInfo->sSensorName, "ov5642" );
		pSensorInfo->pFunc = &func_ov5642;

		// FIXME: the following is just an example in Marvell platform, you should change it according to your driver implementation
		strcpy( pSensorEntry->sDeviceName, "/dev/video0" );
		pSensorEntry->iV4L2SensorID = 1;
	}
#endif

	return error;
} 

CAM_Error OV5642_GetCapability( _CAM_SmartSensorCapability *pCapability )
{
	CAM_Int32s i = 0;

	// preview/video supporting 
	pCapability->iSupportedVideoFormatCnt = 0;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT; i++ ) 
	{
		if ( _OV5642VideoFormatTable[i] == 0 )
		{
			break;
		}
		pCapability->eSupportedVideoFormat[pCapability->iSupportedVideoFormatCnt] = _OV5642VideoFormatTable[i];
		pCapability->iSupportedVideoFormatCnt++;
	}

	pCapability->bArbitraryVideoSize     = CAM_FALSE;
	pCapability->iSupportedVideoSizeCnt  = 0;
	pCapability->stMinVideoSize.iWidth   = _OV5642VideoResTable[0].iWidth;
	pCapability->stMinVideoSize.iHeight  = _OV5642VideoResTable[0].iHeight;
	pCapability->stMaxVideoSize.iWidth   = _OV5642VideoResTable[0].iWidth;
	pCapability->stMaxVideoSize.iHeight  = _OV5642VideoResTable[0].iHeight;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_SIZE_CNT; i++ )
	{
		if ( _OV5642VideoResTable[i].iWidth == 0 || _OV5642VideoResTable[i].iHeight == 0 )
		{
			break;
		}
		pCapability->stSupportedVideoSize[pCapability->iSupportedVideoSizeCnt] = _OV5642VideoResTable[i];
		pCapability->iSupportedVideoSizeCnt++;

		if ( ( pCapability->stMinVideoSize.iWidth > _OV5642VideoResTable[i].iWidth ) || 
		     ( ( pCapability->stMinVideoSize.iWidth == _OV5642VideoResTable[i].iWidth ) &&
		       ( pCapability->stMinVideoSize.iHeight > _OV5642VideoResTable[i].iHeight ) 
		      ) 
		    ) 
		{
			pCapability->stMinVideoSize.iWidth = _OV5642VideoResTable[i].iWidth;
			pCapability->stMinVideoSize.iHeight = _OV5642VideoResTable[i].iHeight;
		}
		if ( ( pCapability->stMaxVideoSize.iWidth < _OV5642VideoResTable[i].iWidth) ||
		     ( ( pCapability->stMaxVideoSize.iWidth == _OV5642VideoResTable[i].iWidth ) && 
		       ( pCapability->stMaxVideoSize.iHeight < _OV5642VideoResTable[i].iHeight ) 
		      ) 
		    )
		{
			pCapability->stMaxVideoSize.iWidth = _OV5642VideoResTable[i].iWidth;
			pCapability->stMaxVideoSize.iHeight = _OV5642VideoResTable[i].iHeight;
		}
	}

	// still capture supporting
	pCapability->iSupportedStillFormatCnt           = 0;
	pCapability->stSupportedJPEGParams.bSupportJpeg = CAM_FALSE;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT; i++ ) 
	{
		if ( _OV5642StillFormatTable[i] == CAM_IMGFMT_JPEG )
		{
			// JPEG encoder functionalities
			pCapability->stSupportedJPEGParams.bSupportJpeg      = CAM_TRUE;
			pCapability->stSupportedJPEGParams.bSupportExif      = CAM_FALSE;
			pCapability->stSupportedJPEGParams.bSupportThumb     = CAM_FALSE;
			pCapability->stSupportedJPEGParams.iMinQualityFactor = 20;
			pCapability->stSupportedJPEGParams.iMaxQualityFactor = 95;
		}
		if ( _OV5642StillFormatTable[i] == 0 )
		{
			break;
		}
		pCapability->eSupportedStillFormat[pCapability->iSupportedStillFormatCnt] = _OV5642StillFormatTable[i];
		pCapability->iSupportedStillFormatCnt++;
	}
	pCapability->bArbitraryStillSize    = CAM_FALSE;
	pCapability->iSupportedStillSizeCnt = 0;
	pCapability->stMinStillSize.iWidth  = _OV5642StillResTable[0].iWidth;
	pCapability->stMinStillSize.iHeight = _OV5642StillResTable[0].iHeight;
	pCapability->stMaxStillSize.iWidth  = _OV5642StillResTable[0].iWidth;
	pCapability->stMaxStillSize.iHeight = _OV5642StillResTable[0].iHeight;
	for ( i = 0; i < CAM_MAX_SUPPORT_IMAGE_SIZE_CNT; i++ )
	{
		if ( _OV5642StillResTable[i].iWidth == 0 || _OV5642StillResTable[i] .iHeight == 0 )
		{
			break;
		}

		pCapability->stSupportedStillSize[pCapability->iSupportedStillSizeCnt] = _OV5642StillResTable[i];
		pCapability->iSupportedStillSizeCnt++;

		if ( ( pCapability->stMinStillSize.iWidth > _OV5642StillResTable[i].iWidth ) ||
		     ( ( pCapability->stMinStillSize.iWidth == _OV5642StillResTable[i].iWidth ) && 
		       ( pCapability->stMinStillSize.iHeight > _OV5642StillResTable[i].iHeight ) 
		     ) 
		   )
		{
			pCapability->stMinStillSize.iWidth  = _OV5642StillResTable[i].iWidth;
			pCapability->stMinStillSize.iHeight = _OV5642StillResTable[i].iHeight;
		}
		if ( ( pCapability->stMaxStillSize.iWidth < _OV5642StillResTable[i].iWidth ) || 
		     ( ( pCapability->stMaxStillSize.iWidth == _OV5642StillResTable[i].iWidth ) && 
		       ( pCapability->stMaxStillSize.iHeight < _OV5642StillResTable[i].iHeight )
		     ) 
		   )
		{
			pCapability->stMaxStillSize.iWidth  = _OV5642StillResTable[i].iWidth;
			pCapability->stMaxStillSize.iHeight = _OV5642StillResTable[i].iHeight;
		}
	}

	// flip/rotate
	pCapability->iSupportedRotateCnt = _ARRAY_SIZE(_OV5642FlipRotation);
	for ( i = 0; i < pCapability->iSupportedRotateCnt; i++ )
	{
		pCapability->eSupportedRotate[i] = (CAM_FlipRotate)_OV5642FlipRotation[i].iParameter;
	}

	// shot mode capability
	pCapability->iSupportedShotModeCnt = _ARRAY_SIZE(_OV564ShotMode);
	for ( i = 0; i < pCapability->iSupportedShotModeCnt; i++ )
	{
		pCapability->eSupportedShotMode[i] = (CAM_ShotMode)_OV564ShotMode[i].iParameter;
	}

	// all supported shot parameters
	_OV5642GetShotParamCap( &pCapability->stSupportedShotParams );

	return CAM_ERROR_NONE;
}

CAM_Error OV5642_GetShotModeCapability( CAM_ShotMode eShotMode, CAM_ShotModeCapability *pShotModeCap )
{
	CAM_Int32u i     = 0;
	CAM_Error  error = CAM_ERROR_NONE;

	// BAC check
	for ( i = 0; i < _ARRAY_SIZE( _OV564ShotMode ); i++ )
	{
		if ( (CAM_ShotMode)_OV564ShotMode[i].iParameter == eShotMode )
		{
			break;
		}
	}

	if ( i >= _ARRAY_SIZE( _OV564ShotMode ) || pShotModeCap == NULL || _OV5642ShotModeCap[eShotMode] == NULL ) 
	{
		return CAM_ERROR_BADARGUMENT;
	}

	(_OV5642ShotModeCap[eShotMode])( pShotModeCap );

	return error;
}

CAM_Error OV5642_Init( void **ppDevice, void *pSensorEntry )
{
	CAM_Error            error      = CAM_ERROR_NONE;
	CAM_Int32s           iSkipFrame = 0;
	OV5642State          *pState    = (OV5642State*)malloc( sizeof(OV5642State) );
	_V4L2SensorEntry     *pSensor   = (_V4L2SensorEntry*)(pSensorEntry);
	_V4L2SensorAttribute  _OV5642Attri;
 
	*ppDevice = pState;
	if ( *ppDevice == NULL )
	{
		return CAM_ERROR_OUTOFMEMORY;
	}

	memset( &_OV5642Attri, 0, sizeof( _V4L2SensorAttribute ) );
	memset( pState, -1, sizeof( OV5642State ) );

	_OV5642Attri.stV4L2SensorEntry.iV4L2SensorID = pSensor->iV4L2SensorID;
	strcpy( _OV5642Attri.stV4L2SensorEntry.sDeviceName, pSensor->sDeviceName );

#if defined( BUILD_OPTION_DEBUG_DISABLE_SAVE_RESTORE_3A )
	iSkipFrame                        = 20;
	_OV5642Attri.fnSaveAeAwb          = NULL;
	_OV5642Attri.fnRestoreAeAwb       = NULL;
	_OV5642Attri.pSaveRestoreUserData = NULL;
#else
	iSkipFrame                        = 2;
	_OV5642Attri.fnSaveAeAwb          = _OV5642SaveAeAwb;
	_OV5642Attri.fnRestoreAeAwb       = _OV5642RestoreAeAwb;
	_OV5642Attri.pSaveRestoreUserData = (void*)pState;
#endif

	_OV5642Attri.fnStartFlash         = _OV5642StartFlash;
 	_OV5642Attri.fnStopFlash          = _OV5642StopFlash;

	error = V4L2_Init( &(pState->stV4L2), &_OV5642Attri, iSkipFrame );
	if ( error != CAM_ERROR_NONE )
	{
		return error;
	}

	// init settings, including: jpeg and shot parameters
	error = _OV5642InitSetting( pState );
	if ( error != CAM_ERROR_NONE )
	{
		return error;
	}

	// af related
#if defined( BUILD_OPTION_STRATEGY_ENABLE_AUTOFOCUS )
	// download OV5642 AF firmware to sensor if needed
	{
#if defined( CAM_PERF )
		CAM_Tick t =  -IPP_TimeGetTickCount();
#endif
		error = V4L2_AFDownload( &pState->stV4L2 );
#if defined( CAM_PERF )
		t += IPP_TimeGetTickCount();
		CELOG( "Perf: AF firmware download cost: %.2f ms\n", t / 1000.0 );
#endif
		if ( error != CAM_ERROR_NONE )
		{
			return error;
		}
	}
#endif

	// face detection related
	giSensorFD                   = pState->stV4L2.iSensorFD;
	pState->iFaceDetectFrameSkip = 0;
	pState->pUserBuf             = NULL;
	pState->pFaceDetectProcBuf   = NULL;
	pState->pFaceDetectInputBuf  = NULL;
	
	pState->stFaceDetectThread.bExitThread   = CAM_FALSE;
	pState->stFaceDetectThread.hEventWakeup  = NULL;
	pState->stFaceDetectThread.hEventExitAck = NULL;
	pState->stFaceDetectThread.iThreadID     = -1;
	
	memset( &pState->stFaceROI, 0, sizeof( CAM_MultiROI ) );
	
	// init digital zoom ref ROI
	memset( &pState->stRefROI, 0, sizeof( CAM_Rect ) );
	error = _ov5642_update_ref_roi( pState );

	return error;
}

CAM_Error OV5642_Deinit( void *pDevice )
{
	CAM_Error   error   = CAM_ERROR_NONE;
	CAM_Int32s  ret     = 0;
	OV5642State *pState = (OV5642State*)pDevice;

	// V4L2 de-init
	error = V4L2_Deinit( &pState->stV4L2 );

	// face detection related
	giSensorFD = -1;
	if ( pState->stFaceDetectThread.iThreadID != -1 )
	{
		// reset the ack before wake-up
		ret = IPP_EventReset( pState->stFaceDetectThread.hEventExitAck );
		ASSERT( ret == 0 );

		// notify thread to exit
		pState->stFaceDetectThread.bExitThread = CAM_TRUE;
		ret = IPP_EventSet( pState->stFaceDetectThread.hEventWakeup );
		ASSERT( ret == 0 );

		// wait for ack for the thread exit
		ret = IPP_EventWait( pState->stFaceDetectThread.hEventExitAck, INFINITE_WAIT, NULL );
		ASSERT( ret == 0 );

		// destroy events safely
		ret = IPP_EventDestroy( pState->stFaceDetectThread.hEventWakeup );
		ASSERT( ret == 0 );

		ret = IPP_EventDestroy( pState->stFaceDetectThread.hEventExitAck );
		ASSERT( ret == 0 );
		
		FaceTrackClose();
		
		pState->stFaceDetectThread.iThreadID = -1;
	}
	if ( pState->pUserBuf != NULL )
	{
		free( pState->pUserBuf );
		pState->pUserBuf            = NULL;
		pState->pFaceDetectProcBuf  = NULL;
		pState->pFaceDetectInputBuf = NULL;
	}
	pState->iFaceDetectFrameSkip = 0;

	// free handle
	free( pState );

	return error;
}

CAM_Error OV5642_RegisterEventHandler( void *pDevice, CAM_EventHandler fnEventHandler, void *pUserData )
{
	OV5642State *pState = (OV5642State*)pDevice;
	CAM_Error   error   = CAM_ERROR_NONE;

	error = V4L2_RegisterEventHandler( &pState->stV4L2, fnEventHandler, pUserData );

	return error;
}

CAM_Error OV5642_Config( void *pDevice, _CAM_SmartSensorConfig *pSensorConfig )
{
	OV5642State *pState = (OV5642State*)pDevice;
	CAM_Error   error   = CAM_ERROR_NONE;
	CAM_Int32s  ret     = 0;

	// XXX: set digital zoom back to 1
	// _OV5642SetDigitalZoom( pState, 1 << 16 );

	error = V4L2_Config( &pState->stV4L2, pSensorConfig );
	if ( error != CAM_ERROR_NONE )
	{
		return error;
	}

	if ( pSensorConfig->eState != CAM_CAPTURESTATE_IDLE )
	{
		// update digital zoom reference ROI
		CELOG( "update digital zoom ref ROI(%s, %d)\n", __FILE__, __LINE__ );
		error = _ov5642_update_ref_roi( pState );
		if ( error != CAM_ERROR_NONE )
		{
			return error;
		}
		// NOTE: after each configure, zoom registers will be flushed, and need update
		pState->stV4L2.stShotParam.iDigZoomQ16 = ( 1 << 16 );

		// configure JPEG quality to sensor
		if ( pSensorConfig->eFormat == CAM_IMGFMT_JPEG )
		{
			error = _OV5642SetJpegParam( pState, &pSensorConfig->stJpegParam );
			if ( error != CAM_ERROR_NONE )
			{
				return error;
			}
		}

		// AGC/AEC restore to auto in preview state
		if ( pSensorConfig->eState == CAM_CAPTURESTATE_PREVIEW )
		{
			// _set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_GAINLOW, 0x003f );
			// _set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_EXPOSUREHIGH, 0x0000 );
			// _set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_EXPOSUREMID, 0x0000 );
			// _set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_EXPOSURELOW, 0x0000 );
			_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_AECAGC, 0x0000 );
		}
	}

	// face detection related
	if ( pSensorConfig->eState == CAM_CAPTURESTATE_PREVIEW )
	{
		if( pState->stV4L2.stShotParam.eShotMode == CAM_SHOTMODE_PORTRAIT )
		{
			pState->iFaceDetectFrameSkip = 0;
		}
	}

	pState->stV4L2.stConfig = *pSensorConfig;

	// XXX: since banding filter is highly related with stream, so we need update it after stream configure. It is better to do this is driver
	// _OV5642SetBandFilter( pState->stV4L2.iSensorFD );

	return CAM_ERROR_NONE;
}

CAM_Error OV5642_GetBufReq( void *pDevice, _CAM_SmartSensorConfig *pSensorConfig, CAM_ImageBufferReq *pBufReq )
{
	OV5642State *pState = (OV5642State*)pDevice;
	CAM_Error   error   = CAM_ERROR_NONE;

	error = V4L2_GetBufReq( &pState->stV4L2, pSensorConfig, pBufReq );

	return error;
}

CAM_Error OV5642_Enqueue( void *pDevice, CAM_ImageBuffer *pImgBuf )
{
	OV5642State *pState = (OV5642State*)pDevice;
	CAM_Error   error   = CAM_ERROR_NONE;

	error = V4L2_Enqueue( &pState->stV4L2, pImgBuf );

	return error;
}

CAM_Error OV5642_Dequeue( void *pDevice, CAM_ImageBuffer **ppImgBuf )
{
	OV5642State *pState = (OV5642State*)pDevice;
	CAM_Error   error   = CAM_ERROR_NONE;
	CAM_Int32s  ret     = 0;

	error = V4L2_Dequeue( &pState->stV4L2, ppImgBuf );

	// for face detection
	if ( pState->stV4L2.stShotParam.eShotMode == CAM_SHOTMODE_PORTRAIT &&
	     pState->stV4L2.stConfig.eState == CAM_CAPTURESTATE_PREVIEW )
	{
		CAM_Error error1 = CAM_ERROR_NONE;

		pState->iFaceDetectFrameSkip++;
		pState->iFaceDetectFrameSkip %= OV5642_FACEDETECTION_INTERVAL;
		if ( pState->iFaceDetectFrameSkip == 0 )
		{
			// add face detection gray-scale image generation here
			CAM_Size stDstSize = { OV5642_FACEDETECTION_INBUFWIDTH, OV5642_FACEDETECTION_INBUFHEIGHT };
			error1 = _YUVToGrayRsz_8u( *ppImgBuf, pState->pFaceDetectInputBuf, stDstSize );
			ASSERT_CAM_ERROR( error1 );

			// trigger FaceDetectionThread
			ret = IPP_EventSet( pState->stFaceDetectThread.hEventWakeup );
			ASSERT( ret == 0 );
		}
	}

	if ( error == CAM_ERROR_NONE && (*ppImgBuf)->bEnableShotInfo )
	{
		error = _OV5642FillFrameShotInfo( pState, *ppImgBuf );
	}

	return error;
}

CAM_Error OV5642_Flush( void *pDevice )
{
	OV5642State *pState = (OV5642State*)pDevice;
	CAM_Error   error   = CAM_ERROR_NONE;

	error = V4L2_Flush( &pState->stV4L2 );

	// XXX: after flush, zoom registers will be flushed, we need follow up this issue with driver
	pState->stV4L2.stShotParam.iDigZoomQ16 = ( 1 << 16 );
	
	return error;
}

CAM_Error OV5642_SetShotParam( void *pDevice, _CAM_ShotParam *pShotParam )
{
	CAM_Int32s     ret            = 0;
	CAM_Error      error          = CAM_ERROR_NONE;
	OV5642State    *pState        = (OV5642State*)pDevice;
	_CAM_ShotParam stSetShotParam = *pShotParam;

/*
	// sensor orientation
	if ( stSetShotParam.eSensorRotation != pState->stV4L2.stShotParam.eSensorRotation )
	{
		ret = _ov5642_set_paramsreg( stSetShotParam.eSensorRotation, _OV5642FlipRotation, _ARRAY_SIZE(_OV5642FlipRotation), pState->stV4L2.iSensorFD );
		if ( ret == -2 ) 
		{
			return CAM_ERROR_NOTSUPPORTEDARG;
		}
		pState->stV4L2.stShotParam.eSensorRotation = stSetShotParam.eSensorRotation;
	}
*/

	// shot mode setting
	if ( stSetShotParam.eShotMode != pState->stV4L2.stShotParam.eShotMode )
	{
		CAM_Int32s i = 0;

		CELOG( "shot mode\n" );
		ret = _ov5642_set_paramsreg( stSetShotParam.eShotMode, _OV564ShotMode, _ARRAY_SIZE( _OV564ShotMode ), pState->stV4L2.iSensorFD );
		if ( ret == -2 ) 
		{
			return CAM_ERROR_NOTSUPPORTEDARG;
		}

		for ( i = 0; i < 3; i++ )
		{
			if ( stSetShotParam.eShotMode == _OV5642DefaultShotParams[i].eShotMode )
			{
				stSetShotParam = _OV5642DefaultShotParams[i];
				break;
			}
		}
		// to portrait mode
		if ( stSetShotParam.eShotMode == CAM_SHOTMODE_PORTRAIT )
		{
			// init face detection lib
			ASSERT( pState->pUserBuf == NULL );
			pState->pUserBuf = (CAM_Int8u*)malloc( OV5642_FACEDETECTION_PROCBUFLEN + OV5642_FACEDETECTION_INBUFLEN + 16 );
			if ( pState->pUserBuf == NULL )
			{
				TRACE( CAM_ERROR, "Error: no enough memory for face detection( %s, %d )\n", __FILE__, __LINE__ );
				return CAM_ERROR_OUTOFMEMORY;
			}

			pState->pFaceDetectInputBuf = _ALIGN_TO( pState->pUserBuf, 8 );
			pState->pFaceDetectProcBuf  = pState->pFaceDetectInputBuf + OV5642_FACEDETECTION_INBUFLEN;

			ret = FaceTrackOpen( pState->pFaceDetectProcBuf, _OV5642FaceDetectCallback, 0x05 ); // NOTE: refer to FaceTrack.h for API information 
			ASSERT( ret == 0 );

			pState->iFaceDetectFrameSkip = 0;

			// create face-detection thread for better efficiency
			ASSERT( pState->stFaceDetectThread.iThreadID == -1 );
			pState->stFaceDetectThread.bExitThread = CAM_FALSE;

			ret = IPP_EventCreate( &pState->stFaceDetectThread.hEventWakeup );
			ASSERT( ret == 0 );
			ret = IPP_EventReset( pState->stFaceDetectThread.hEventWakeup );
			ASSERT( ret == 0 );

			ret = IPP_EventCreate( &pState->stFaceDetectThread.hEventExitAck );
			ASSERT( ret == 0 );
			ret = IPP_EventReset( pState->stFaceDetectThread.hEventExitAck );
			ASSERT( ret == 0 );

			ret = IPP_ThreadCreate( &pState->stFaceDetectThread.iThreadID, 0, FaceDetectionThread, pState );
			ASSERT( ret == 0 );
		}
		// from portrait mode
		else if ( pState->stV4L2.stShotParam.eShotMode == CAM_SHOTMODE_PORTRAIT )
		{
			// reset the ack before wake-up
			ret = IPP_EventReset( pState->stFaceDetectThread.hEventExitAck );
			ASSERT( ret == 0 );

			// notify thread to exit
			pState->stFaceDetectThread.bExitThread = CAM_TRUE;
			ret = IPP_EventSet( pState->stFaceDetectThread.hEventWakeup );
			ASSERT( ret == 0 );

			// wait for ack for the thread exit
			ret = IPP_EventWait( pState->stFaceDetectThread.hEventExitAck, INFINITE_WAIT, NULL );
			ASSERT( ret == 0 );

			// destroy events safely
			ret = IPP_EventDestroy( pState->stFaceDetectThread.hEventWakeup );
			ASSERT( ret == 0 );

			ret = IPP_EventDestroy( pState->stFaceDetectThread.hEventExitAck );
			ASSERT( ret == 0 );
		
			pState->stFaceDetectThread.iThreadID = -1;
			
			FaceTrackClose();
						
			free( pState->pUserBuf );
			pState->pUserBuf             = NULL;
			pState->pFaceDetectProcBuf   = NULL;
			pState->pFaceDetectInputBuf  = NULL;

			pState->iFaceDetectFrameSkip = 0;
		}

		pState->stV4L2.stShotParam.eShotMode = stSetShotParam.eShotMode;
	}

	// ISO setting
	if ( stSetShotParam.eIsoMode != pState->stV4L2.stShotParam.eIsoMode ) 
	{
		CELOG( "ISO\n" );
		ret = _ov5642_set_paramsreg( stSetShotParam.eIsoMode, _OV5642IsoMode, _ARRAY_SIZE(_OV5642IsoMode), pState->stV4L2.iSensorFD );
		if ( ret == -2 ) 
		{
			return CAM_ERROR_NOTSUPPORTEDARG;
		}

		pState->stV4L2.stShotParam.eIsoMode = stSetShotParam.eIsoMode;

	}

	// color effect setting
	if ( stSetShotParam.eColorEffect != pState->stV4L2.stShotParam.eColorEffect ) 
	{
		CELOG( "Color Effect\n" );
		ret = _ov5642_set_paramsreg( stSetShotParam.eColorEffect, _OV5642ColorEffectMode, _ARRAY_SIZE(_OV5642ColorEffectMode), pState->stV4L2.iSensorFD );
		
		if ( ret == -2 ) 
		{
			return CAM_ERROR_NOTSUPPORTEDARG;
		}

		pState->stV4L2.stShotParam.eColorEffect = stSetShotParam.eColorEffect;
	}

	// white balance setting
	if ( stSetShotParam.eWBMode != pState->stV4L2.stShotParam.eWBMode ) 
	{
		CELOG( "White Balance\n" );
		ret = _ov5642_set_paramsreg( stSetShotParam.eWBMode, _OV5642WBMode, _ARRAY_SIZE(_OV5642WBMode), pState->stV4L2.iSensorFD );
		
		if ( ret == -2 ) 
		{
			return CAM_ERROR_NOTSUPPORTEDARG;
		}

		pState->stV4L2.stShotParam.eWBMode = stSetShotParam.eWBMode;
	}

	// exposure metering mode
	if ( stSetShotParam.eExpMeterMode != pState->stV4L2.stShotParam.eExpMeterMode ) 
	{
		CELOG( "Exposure Metering mode\n" );
		ret = _ov5642_set_paramsreg( stSetShotParam.eExpMeterMode, _OV5642ExpMeterMode, _ARRAY_SIZE(_OV5642ExpMeterMode), pState->stV4L2.iSensorFD );
		
		if ( ret == -2 ) 
		{
			return CAM_ERROR_NOTSUPPORTEDARG;
		}

		pState->stV4L2.stShotParam.eExpMeterMode = stSetShotParam.eExpMeterMode;
	}
	
	// banding filter mode
	if ( stSetShotParam.eBandFilterMode != pState->stV4L2.stShotParam.eBandFilterMode ) 
	{
		CELOG( "Banding Filter mode\n" );
		// ret = _ov5642_set_paramsreg( stSetShotParam.eBandFilterMode, _OV5642BdFltMode, _ARRAY_SIZE(_OV5642BdFltMode), pState->stV4L2.iSensorFD );
		ret = 0;		

		if ( ret == -2 ) 
		{
			return CAM_ERROR_NOTSUPPORTEDARG;
		}

		pState->stV4L2.stShotParam.eBandFilterMode = stSetShotParam.eBandFilterMode;
	}

	// flash mode
	if ( stSetShotParam.eFlashMode != pState->stV4L2.stShotParam.eFlashMode ) 
	{
		CELOG( "flash mode\n" );
		_set_sensor_reg( pState->stV4L2.iSensorFD, 0x3000, 3, 3, 3, 0x00 );
		_set_sensor_reg( pState->stV4L2.iSensorFD, 0x3004, 3, 3, 3, 0xff );
		_set_sensor_reg( pState->stV4L2.iSensorFD, 0x3016, 1, 1, 1, 0x02 );
		_set_sensor_reg( pState->stV4L2.iSensorFD, 0x3b07, 0, 1, 0, 0x0a );
		if ( stSetShotParam.eFlashMode == CAM_FLASH_TORCH )
		{
			ret = _ov5642_set_paramsreg( CAM_FLASH_ON,_OV5642FlashMode, _ARRAY_SIZE(_OV5642FlashMode), pState->stV4L2.iSensorFD );
			if ( ret == -2 ) 
			{
			  return CAM_ERROR_NOTSUPPORTEDARG;
			}
			pState->stV4L2.bIsFlashOn = CAM_TRUE;
		}
		else if ( pState->stV4L2.stShotParam.eFlashMode == CAM_FLASH_TORCH )
		{
			ret = _ov5642_set_paramsreg( CAM_FLASH_OFF,_OV5642FlashMode, _ARRAY_SIZE(_OV5642FlashMode), pState->stV4L2.iSensorFD );
			if ( ret == -2 ) 
			{
			  return CAM_ERROR_NOTSUPPORTEDARG;
			}
			pState->stV4L2.bIsFlashOn = CAM_FALSE;
		}

		pState->stV4L2.stShotParam.eFlashMode = stSetShotParam.eFlashMode;
	}

	// EV compensation setting
	if ( stSetShotParam.iEvCompQ16 != pState->stV4L2.stShotParam.iEvCompQ16 ) 
	{
		CELOG( "EV compensation\n" );
		ret = _ov5642_set_paramsreg_nearest( &stSetShotParam.iEvCompQ16, _OV5642EvCompMode, _ARRAY_SIZE(_OV5642EvCompMode), pState->stV4L2.iSensorFD );
		
		if ( ret == -2 ) 
		{
			return CAM_ERROR_NOTSUPPORTEDARG;
		}

		pState->stV4L2.stShotParam.iEvCompQ16 = stSetShotParam.iEvCompQ16;
	}

	// focus mode setting
	if ( stSetShotParam.eFocusMode != pState->stV4L2.stShotParam.eFocusMode ) 
	{
		CELOG( "focus mode\n" );

		// reset sensor MCU
		_set_sensor_reg( pState->stV4L2.iSensorFD, 0x3000, 5, 5, 5, 0x20 );
		IPP_Sleep( 8000 );
		_set_sensor_reg( pState->stV4L2.iSensorFD, 0x3000, 5, 5, 5, 0x00 );

		ret = _ov5642_set_paramsreg( stSetShotParam.eFocusMode, _OV5642FocusMode, _ARRAY_SIZE( _OV5642FocusMode ), pState->stV4L2.iSensorFD );

		if ( ret == -2 ) 
		{
			return CAM_ERROR_NOTSUPPORTEDARG;
		}

		pState->stV4L2.stShotParam.eFocusMode = stSetShotParam.eFocusMode;
	}


/*--------------------------------------function style---------------------------------------------*/

	// digital zoom
	if ( stSetShotParam.iDigZoomQ16 != pState->stV4L2.stShotParam.iDigZoomQ16 ) 
	{
		error = _OV5642SetDigitalZoom( pState, stSetShotParam.iDigZoomQ16 );
		ASSERT_CAM_ERROR( error );
	}

	// brightness setting
	if ( stSetShotParam.iBrightness != pState->stV4L2.stShotParam.iBrightness ) 
	{
		CELOG( "Brightness\n" );
		CAM_Int16u  brt = 0;
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SDE_CTL, 0x80 );
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_CST_CTL, 0x04 );
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SAT_CTL, 0x02 );

		if ( stSetShotParam.iBrightness <  0) 
		{
			brt = (CAM_Int16u )( -stSetShotParam.iBrightness );
		    _set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_BRIT_SIGN, 0x08 );
		}
		else 
		{
			brt = (CAM_Int16u )( stSetShotParam.iBrightness );
		    _set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_BRIT_SIGN, 0x00 );
		}
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_BRIT_VAL, brt ); 
		pState->stV4L2.stShotParam.iBrightness = stSetShotParam.iBrightness;

	}

	// contrast setting  
	if ( stSetShotParam.iContrast != pState->stV4L2.stShotParam.iContrast ) 
	{
		CELOG( "Contrast\n" );
		CAM_Int16u  crt = 0;
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SDE_CTL, 0x80 );
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_CST_CTL, 0x04 );
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SAT_CTL, 0x02 );
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_CST_SIGNCTL, 0x00 );
		
		crt = (CAM_Int16u )(stSetShotParam.iContrast);
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_CST_OFFSET, crt );
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_CST_VAL, crt ); 

		pState->stV4L2.stShotParam.iContrast = stSetShotParam.iContrast;
	}

	// saturation setting
	if ( stSetShotParam.iSaturation != pState->stV4L2.stShotParam.iSaturation ) 
	{
		CELOG( "Saturation\n" );
		CAM_Int16u  sat = 0;
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SDE_CTL, 0x80 );
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SAT_CTL, 0x02 );
		
		sat = (CAM_Int16u)( stSetShotParam.iSaturation );
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SAT_U, sat );
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SAT_V, sat ); 

		pState->stV4L2.stShotParam.iSaturation = stSetShotParam.iSaturation;
	}
    
	// burst count setting
	if ( stSetShotParam.iBurstCnt != pState->stV4L2.stShotParam.iBurstCnt )
	{
		CELOG( "Burst Count\n" );
		// TODO: if your sensor/ISP originally support burst count, set ISP sepecific params here
		pState->stV4L2.stShotParam.iBurstCnt = stSetShotParam.iBurstCnt;
	}

	// TODO: other settings: shutter speed, HDR, image stabilizer if supported

	return CAM_ERROR_NONE;
}

CAM_Error OV5642_GetShotParam( void *pDevice, _CAM_ShotParam *pShotParam )
{
	OV5642State *pState = (OV5642State*)pDevice;
    
	*pShotParam = pState->stV4L2.stShotParam;

	return CAM_ERROR_NONE;
}

CAM_Error OV5642_StartFocus( void *pDevice, void *pFocusROI )
{
	OV5642State *pState = (OV5642State*)pDevice;
	CAM_Int32s  ret     = 0;
	CAM_Int32s  cnt     = 3000;
	CAM_Int16u  reg     = 0;
	CAM_Tick    t       = 0;

	// reset sensor MCU to ensure AF work 
	_set_sensor_reg( pState->stV4L2.iSensorFD, 0x3000, 5, 5, 5, 0x20 );
	IPP_Sleep( 5000 );
	_set_sensor_reg( pState->stV4L2.iSensorFD, 0x3000, 5, 5, 5, 0x00 );

	// check focus status
	while ( cnt )
	{
		_get_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_AF_FOCUSSTA, &reg );
		if( reg == OV5642_FOCUS_STA_IDLE || reg == OV5642_FOCUS_STA_FOCUSED )
		{
			break;
		}
		else if ( reg ==  OV5642_FOCUS_STA_STARTUP )
		{
			IPP_Sleep( 1000 );
			cnt--;
		}
		else
		{
			TRACE( CAM_ERROR, "Error: AF firmware fatal error[%d] ( %s, %d )\n", reg, __FILE__, __LINE__ );
			return CAM_ERROR_DRIVEROPFAILED;
		}
	}
	if ( cnt < 0 )
	{
		TRACE( CAM_ERROR, "Error: AF firmware start too long ( %s, %d )\n", __FILE__, __LINE__ );
		return CAM_ERROR_DRIVEROPFAILED;
	}

	// set af firmware to idle
#if defined( CAM_PERF )
	t = -IPP_TimeGetTickCount();
#endif
	if ( reg == OV5642_FOCUS_STA_FOCUSED )
	{
		TRACE( CAM_INFO, "Info: Set AF firmware to idle ( %s, %d )\n", __FILE__, __LINE__ );
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_AF_MAIN, 0x08 );
		ret = _ov5642_wait_afcmd_ready( pState->stV4L2.iSensorFD );
		if ( ret != 0 )
		{
			TRACE( CAM_ERROR, "Error: AF cmd send failure ( %s, %d )\n", __FILE__, __LINE__ );
			return CAM_ERROR_DRIVEROPFAILED;
		}
	}
#if defined( CAM_PERF )
	t += IPP_TimeGetTickCount();
	CELOG( "Perf: AF firmware initialization cost %.2f ms, cnt = %d \n", t / 1000.0, 3000 - cnt );
#endif

	// update zone
	_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_AF_TAG, 0x03 );
	_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_AF_MAIN, 0x12 );
	ret = _ov5642_wait_afcmd_ready( pState->stV4L2.iSensorFD );
	if ( ret != 0 )
	{
		TRACE( CAM_ERROR, "Error: AF cmd update zone failure( %s, %d )\n", __FILE__, __LINE__ );
		return CAM_ERROR_DRIVEROPFAILED;
	}
	TRACE( CAM_INFO, "Info: AF cmd update zone success( %s, %d )\n", __FILE__, __LINE__ );

	if ( pState->stV4L2.stShotParam.eFocusMode == CAM_FOCUS_AUTO_ONESHOT_CENTER )
	{
		ret = _ov5642_set_paramsreg( CAM_FOCUSZONE_CENTER, _OV5642FocusZone, _ARRAY_SIZE( _OV5642FocusZone ), pState->stV4L2.iSensorFD );
		if ( ret == -2 ) 
		{
			return CAM_ERROR_NOTSUPPORTEDARG;
		}
	}
	else
	{
		CAM_MultiROI *pROI= (CAM_MultiROI*)pFocusROI;
		if ( pROI->iROICnt == 1 && pROI->stWeiRect[0].stRect.iWidth == 1 && pROI->stWeiRect[0].stRect.iHeight == 1 )
		{
			// touch mode( spot )
			CAM_Int16u xc = pROI->stWeiRect[0].stRect.iLeft * OV5642_AFFIRMWARE_MAX_WIDTH / ( pState->stV4L2.stConfig.iWidth );
			CAM_Int16u yc = pROI->stWeiRect[0].stRect.iTop * OV5642_AFFIRMWARE_MAX_HEIGH / ( pState->stV4L2.stConfig.iHeight );
			_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_AF_TAG, 0x11 );
			_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_AF_PARA1, xc );
			_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_AF_PARA0, yc );
			_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_AF_MAIN, 0x10 );
			_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_AF_MAIN, 0x03 );
		}
		else
		{
			// TODO: add your ROI-based AF code here
			return CAM_ERROR_NOTSUPPORTEDARG;
		}
	}

	// wait until settings take effect
	ret = _ov5642_wait_afcmd_ready( pState->stV4L2.iSensorFD );
	if ( ret != 0 )
	{
		TRACE( CAM_ERROR, "Error: AF cmd send failure( %s, %d )\n", __FILE__, __LINE__ );
		return CAM_ERROR_DRIVEROPFAILED;
	}

	pState->stFocusState.bIsFocusStop = CAM_FALSE;
	pState->stFocusState.bIsFocused   = CAM_FALSE;
	pState->stFocusState.tFocusStart  = IPP_TimeGetTickCount();

	return CAM_ERROR_NONE;
}

CAM_Error OV5642_CancelFocus( void *pDevice )
{
	OV5642State *pState = (OV5642State*)pDevice;
	CAM_Int32s  ret     = 0;

	if ( pState->stFocusState.bIsFocusStop == CAM_FALSE )
	{
		// reset sensor MCU to ensure AF work 
		_set_sensor_reg( pState->stV4L2.iSensorFD, 0x3000, 5, 5, 5, 0x20 );
		IPP_Sleep( 5000 );
		_set_sensor_reg( pState->stV4L2.iSensorFD, 0x3000, 5, 5, 5, 0x00 );

		// set sensor to return idle
		TRACE( CAM_INFO, "Info: Set AF firmware to idle ( %s, %d )\n", __FILE__, __LINE__ );
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_AF_MAIN, 0x08 );
		ret = _ov5642_wait_afcmd_ready( pState->stV4L2.iSensorFD );
		if ( ret != 0 )
		{
			TRACE( CAM_ERROR, "Error: AF cmd send failure ( %s, %d )\n", __FILE__, __LINE__ );
			return CAM_ERROR_DRIVEROPFAILED;
		}

		pState->stFocusState.bIsFocusStop = CAM_TRUE;
		pState->stFocusState.bIsFocused   = CAM_FALSE;
	}

	return CAM_ERROR_NONE;
}

CAM_Error OV5642_CheckFocusState( void *pDevice, CAM_Bool *pFocusAutoStopped, _CAM_FocusState *pFocusState )
{
	OV5642State *pState = (OV5642State*)pDevice;
	CAM_Error   error   = CAM_ERROR_NONE;
	CAM_Tick    t       = 0;
	CAM_Int16u  reg     = 0;
	CAM_Int32s  cnt     = 1;

	if ( pState->stV4L2.stShotParam.eFocusMode == CAM_FOCUS_AUTO_ONESHOT_CENTER ||
	     pState->stV4L2.stShotParam.eFocusMode ==CAM_FOCUS_AUTO_ONESHOT_TOUCH )
	{
		while ( cnt-- )
		{
			_get_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_AF_FOCUSSTA, &reg );
			if ( reg == OV5642_FOCUS_STA_FOCUSED && pState->stFocusState.bIsFocused == CAM_FALSE )
			{
				pState->stFocusState.bIsFocusStop = CAM_TRUE;
				pState->stFocusState.bIsFocused   = CAM_TRUE; 	
				*pFocusState                      = CAM_IN_FOCUS;
				*pFocusAutoStopped                = pState->stFocusState.bIsFocusStop;
				break;
			}	
		}
		if ( cnt < 0 )
		{
			t = IPP_TimeGetTickCount();
			if ( ( t - pState->stFocusState.tFocusStart ) >= OV5642_AF_MAX_TIMEOUT )
			{
				pState->stFocusState.bIsFocusStop = CAM_FALSE;
				pState->stFocusState.bIsFocused   = CAM_FALSE; 	
				*pFocusAutoStopped                = CAM_FALSE;
				*pFocusState                      = CAM_AF_FAIL;
			}
			else
			{
				*pFocusAutoStopped = CAM_FALSE;
				*pFocusState       = CAM_KEEP_STATUS;
			}
		}
		error = CAM_ERROR_NONE;
	}
	else
	{
		TRACE( CAM_ERROR, "Error: check AF status should only be called while focus mode set to AUTO OneShot ( %s, %d )\n", __FILE__, __LINE__ );
		error = CAM_ERROR_NOTSUPPORTEDARG;
	}

	return error;	
}


CAM_Error OV5642_GetDigitalZoomCapability( CAM_Int32s iWidth, CAM_Int32s iHeight, CAM_Int32s *pSensorDigZoomQ16 )
{
	if ( iWidth <= 0 || iHeight <= 0 || iWidth > OV5642_SENSOR_MAX_WIDTH || iHeight > OV5642_SENSOR_MAX_HEIGHT )
	{
		return CAM_ERROR_BADARGUMENT;
	}

	if ( ( iWidth <= OV5642_2XZOOM_MAX_WIDTH ) && ( iHeight <= OV5642_2XZOOM_MAX_HEIGHT ) )
	{
		*pSensorDigZoomQ16 =  2 << 16;
	}
	else
	{
		*pSensorDigZoomQ16 =  1 << 16;
	}

	return CAM_ERROR_NONE;
}

_SmartSensorFunc func_ov5642 = 
{
	OV5642_GetCapability,
	OV5642_GetShotModeCapability,

	OV5642_SelfDetect,
	OV5642_Init,
	OV5642_Deinit,
	OV5642_RegisterEventHandler,
	OV5642_Config,
	OV5642_GetBufReq,
	OV5642_Enqueue,
	OV5642_Dequeue,
	OV5642_Flush,
	OV5642_SetShotParam,
	OV5642_GetShotParam,

	OV5642_StartFocus,
	OV5642_CancelFocus,
	OV5642_CheckFocusState,
	OV5642_GetDigitalZoomCapability,
};

static CAM_Error _OV5642SetBandFilter( CAM_Int32s iSensorFD )
{
	CELOG( "%s\n", __FUNCTION__ );
	double     sys_clk;
	CAM_Int16u reg;
	CAM_Int32s hts, vts;
	CAM_Int32s band_value, max_bands;


	_ov5642_calc_sysclk( iSensorFD, &sys_clk );
	_ov5642_calc_hvts( iSensorFD, &hts, &vts );

	// 60Hz banding filter settings
	band_value = sys_clk * 16 * 1000000 / 120 / hts;
	max_bands  = vts / ( band_value / 16 ) - 1;
	CELOG( "band_value: 0x%x, max_bands: 0x%x\n", band_value, max_bands );
	reg = band_value & 0xff ;
	_set_sensor_reg( iSensorFD, 0x3a0b, 0, 7, 0, reg );
	reg = ( band_value >> 8 ) & 0x3f;
	_set_sensor_reg( iSensorFD, 0x3a0a, 0, 5, 0, reg );
	reg = max_bands & 0x3f;
	_set_sensor_reg( iSensorFD, 0x3a0d, 0, 5, 0, reg );


	// 50Hz banding filter settings
	band_value = sys_clk * 16 * 1000000 / 100 / hts;
	max_bands  = vts / ( band_value / 16 ) - 1;
	CELOG( "band_value: 0x%x, max_bands: 0x%x\n", band_value, max_bands );
	reg = band_value & 0xff ;
	_set_sensor_reg( iSensorFD, 0x3a09, 0, 7, 0, 0x4a );
	reg = ( band_value >> 8 ) & 0x3f;
	_set_sensor_reg( iSensorFD, 0x3a08, 0, 5, 0, 0x14 );
	reg = max_bands & 0x3f;
	_set_sensor_reg( iSensorFD, 0x3a0e, 0, 5, 0, 0x2 );

	return CAM_ERROR_NONE;
}

static CAM_Error _OV5642SetJpegParam( OV5642State *pState, CAM_JpegParam *pJpegParam )
{
	CAM_Int16u  usQuantStep = 0;
	CAM_Int32s  ret         = 0; 

	if ( pJpegParam->iQualityFactor != pState->stV4L2.stJpegParam.iQualityFactor ) 
	{
		
		if ( pJpegParam->iQualityFactor < 20 || pJpegParam->iQualityFactor > 95 ) 
		{
			return CAM_ERROR_NOTSUPPORTEDARG;
		}
        
		// compute quantization scale from quality factor
		if ( pJpegParam->iQualityFactor < 50 ) 
		{
			usQuantStep = 800 / pJpegParam->iQualityFactor;
		}
		else 
		{
			usQuantStep = 32 - ((pJpegParam->iQualityFactor) << 4) / 50;
		}

		ret = _set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_JPEG_QSCTL, usQuantStep ); 
        
		if ( ret != 0 ) 
		{
			return CAM_ERROR_DRIVEROPFAILED;
		}

		pState->stV4L2.stJpegParam.iQualityFactor = pJpegParam->iQualityFactor;
	}

	return CAM_ERROR_NONE;
}

static CAM_Error  _OV5642InitSetting( OV5642State *pState )
{
	CAM_Int32s     ret = 0;
	_CAM_ShotParam stShotParam;
	CAM_Int32s     i = 0;

	for ( i = 0; i < 3; i++ )
	{
		if ( CAM_SHOTMODE_AUTO == _OV5642DefaultShotParams[i].eShotMode )
		{
			stShotParam = _OV5642DefaultShotParams[i];
			break;
		}
	}
	
	// shooting parameters

	// shot mode
	ret = _ov5642_set_paramsreg( stShotParam.eShotMode, _OV564ShotMode, _ARRAY_SIZE(_OV564ShotMode), pState->stV4L2.iSensorFD );

	// ISO
	ret = _ov5642_set_paramsreg( stShotParam.eIsoMode, _OV5642IsoMode, _ARRAY_SIZE(_OV5642IsoMode), pState->stV4L2.iSensorFD );

	// color effect
	ret = _ov5642_set_paramsreg( stShotParam.eColorEffect, _OV5642ColorEffectMode, _ARRAY_SIZE(_OV5642ColorEffectMode), pState->stV4L2.iSensorFD );

	// white balance
	ret = _ov5642_set_paramsreg( stShotParam.eWBMode, _OV5642WBMode, _ARRAY_SIZE(_OV5642WBMode), pState->stV4L2.iSensorFD );
		
	// exposure metering mode
	ret = _ov5642_set_paramsreg( stShotParam.eExpMeterMode, _OV5642ExpMeterMode, _ARRAY_SIZE(_OV5642ExpMeterMode), pState->stV4L2.iSensorFD );

	// banding filter mode
	// ret = _ov5642_set_paramsreg( stShotParam.eBandFilterMode, _OV5642BdFltMode, _ARRAY_SIZE(_OV5642BdFltMode), pState->stV4L2.iSensorFD );

	// flash mode: off

	// EV compensation setting
	ret = _ov5642_set_paramsreg( stShotParam.iEvCompQ16, _OV5642EvCompMode, _ARRAY_SIZE(_OV5642EvCompMode), pState->stV4L2.iSensorFD );
		
	// focus mode: off

	// digital zoom: 1 << 16

	// brightness
	{
		CAM_Int16u  brt = 0;
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SDE_CTL, 0x80 );
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_CST_CTL, 0x04 );
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SAT_CTL, 0x02 );

		if ( stShotParam.iBrightness <  0 ) 
		{
			brt = (CAM_Int16u )( -stShotParam.iBrightness );
			_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_BRIT_SIGN, 0x08 );
		}
		else 
		{
			brt = (CAM_Int16u )( stShotParam.iBrightness );
			_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_BRIT_SIGN, 0x00 );
		}
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_BRIT_VAL, brt ); 

	}

	// contrast
	{
		CAM_Int16u  crt = 0;
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SDE_CTL, 0x80 );
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_CST_CTL, 0x04 );
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SAT_CTL, 0x02 );
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_CST_SIGNCTL, 0x00 );
		
		crt = (CAM_Int16u )(stShotParam.iContrast);
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_CST_OFFSET, crt );
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_CST_VAL, crt ); 
	}

	// saturation
	{
		CAM_Int16u  sat = 0;
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SDE_CTL, 0x80 );
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SAT_CTL, 0x02 );
		
		sat = (CAM_Int16u)( stShotParam.iSaturation );
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SAT_U, sat );
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SAT_V, sat ); 
	}
    
	// burst count: 1

	pState->stV4L2.stShotParam = stShotParam;

	/* JPEG parameters */
	pState->stV4L2.stJpegParam.iSampling      = 1; // 0 - 420, 1 - 422, 2 - 444
	pState->stV4L2.stJpegParam.iQualityFactor = 67;
	pState->stV4L2.stJpegParam.bEnableExif    = CAM_FALSE;
	pState->stV4L2.stJpegParam.bEnableThumb   = CAM_FALSE;
	pState->stV4L2.stJpegParam.iThumbWidth    = 0;
	pState->stV4L2.stJpegParam.iThumbHeight   = 0;

	return CAM_ERROR_NONE;
}

static CAM_Error _OV5642SaveAeAwb( const _CAM_SmartSensorConfig *pOldCfg, void *pUserData )
{
	CAM_Error   error   = CAM_ERROR_NONE;
	OV5642State *pState = (OV5642State*)pUserData;

	TRACE( CAM_INFO, "Info: %s in\n", __FUNCTION__ );

	if ( pOldCfg->eState != CAM_CAPTURESTATE_PREVIEW )
	{
		TRACE( CAM_ERROR, "Error: save 3A should happen in preview state, current state[%d] ( %s, %d )\n", pOldCfg->eState, __FILE__, __LINE__ );
		return CAM_ERROR_NONE;
	}

	error = _ov5642_save_settings( pState->stV4L2.iSensorFD, &pState->st3Astat );
	TRACE( CAM_INFO, "Info: %s out\n", __FUNCTION__ );

	return error;
}

static CAM_Error _OV5642RestoreAeAwb( const _CAM_SmartSensorConfig *pNewCfg, void *pUserData )
{
	CAM_Error   error   = CAM_ERROR_NONE;
	OV5642State *pState = (OV5642State*)pUserData;

	TRACE( CAM_INFO, "Info: %s in\n", __FUNCTION__ );

	if ( pNewCfg->eState != CAM_CAPTURESTATE_STILL )
	{
		TRACE( CAM_ERROR, "Error: restore 3A should happen in still state, current state:%d\n", pNewCfg->eState );
		return CAM_ERROR_NONE;
	}

	error = _ov5642_calc_new3A( pState->stV4L2.iSensorFD, &pState->st3Astat );
	if ( error != CAM_ERROR_NONE )
	{
		return error;
	}

	error = _ov5642_set_new_exposure( pState->stV4L2.iSensorFD, &pState->st3Astat );

	TRACE( CAM_INFO, "Info: %s out\n", __FUNCTION__ );
	return error;
}

static CAM_Error _OV5642StartFlash( void *pData )
{
	CAM_Int32s       ret         = 0;
	CAM_Bool         bOpenFlash  = CAM_FALSE;
	_V4L2SensorState *pV4l2State = (_V4L2SensorState*)pData; 

	if ( pV4l2State->stShotParam.eFlashMode == CAM_FLASH_AUTO )
	{
		CAM_Int16u reg = 0;
		_get_sensor_reg( pV4l2State->iSensorFD, OV5642_REG_AVG_LUX, &reg );

		// TODO: just reference, you can modify it according to your requirement
		if ( reg < 85 && !pV4l2State->bIsFlashOn )
		{
			bOpenFlash = CAM_TRUE;
		}
	}
	else if ( pV4l2State->stShotParam.eFlashMode == CAM_FLASH_ON ) 
	{
		if ( !pV4l2State->bIsFlashOn )
		{
			bOpenFlash = CAM_TRUE;
		}
	}

	// start flash
	if ( bOpenFlash == CAM_TRUE )
	{
		ret = _ov5642_set_paramsreg( CAM_FLASH_ON, _OV5642FlashMode, _ARRAY_SIZE( _OV5642FlashMode ), pV4l2State->iSensorFD );
		if ( ret == -2 ) 
		{
			return CAM_ERROR_NOTSUPPORTEDARG;
		}
		pV4l2State->bIsFlashOn = CAM_TRUE;
	}

	return CAM_ERROR_NONE;
}

static CAM_Error _OV5642StopFlash( void *pData )
{
	CAM_Int32s       ret         = 0;
	_V4L2SensorState *pV4l2State = (_V4L2SensorState*)pData; 

	if ( pV4l2State->stShotParam.eFlashMode != CAM_FLASH_TORCH )
	{
		if ( pV4l2State->bIsFlashOn )
		{
			ret = _ov5642_set_paramsreg( CAM_FLASH_OFF, _OV5642FlashMode, _ARRAY_SIZE( _OV5642FlashMode ), pV4l2State->iSensorFD );
			if ( ret == -2 ) 
			{
				return CAM_ERROR_NOTSUPPORTEDARG;
			}
		}
		pV4l2State->bIsFlashOn = CAM_FALSE;
	}

	return CAM_ERROR_NONE;
}

// set digital zoom
static CAM_Error _OV5642SetDigitalZoom( OV5642State *pState, CAM_Int32s iDigitalZoomQ16 )
{
#if ( PLATFORM_PROCESSOR_VALUE == PLATFORM_PROCESSOR_MG1 )
	struct v4l2_crop stCrop;
	CAM_Int32s	 ret = 0;
#endif	
	
	CAM_Rect         stRefROI, stNewROI;
	CAM_Int32s       iCenterX, iCenterY;
	CAM_Int16u       reg   = 0;
	CAM_Error        error = CAM_ERROR_NONE;

	_CHECK_BAD_POINTER( pState );

	stRefROI = pState->stRefROI;
	iCenterX = stRefROI.iLeft + stRefROI.iWidth  / 2;
	iCenterY = stRefROI.iTop  + stRefROI.iHeight / 2;

	/* calc new zoom settings */
	stNewROI.iWidth  = _ALIGN_TO( stRefROI.iWidth  * ( 1 << 16 ) / iDigitalZoomQ16, 4 );
	stNewROI.iHeight = _ALIGN_TO( stRefROI.iHeight * ( 1 << 16 ) / iDigitalZoomQ16, 4 );

	stNewROI.iLeft = _ALIGN_TO( iCenterX - stNewROI.iWidth / 2, 2 );
	stNewROI.iTop  = _ALIGN_TO( iCenterY - stNewROI.iHeight / 2, 2 );

	/* XXX: for MG familiy chip, SCI is sensitive and seems un-recoverable, so we need use S_CROP 
	 *      ioctl to do digital zoom, and driver can hide a SCI reset operation in the calling
	 */
#if ( PLATFORM_PROCESSOR_VALUE != PLATFORM_PROCESSOR_MG1 )
	// enable group 0
	// _set_sensor_reg( pState->stV4L2.iSensorFD, 0x3212, 0, 7, 0, 0x00 );

	/* set new digital zoom to sensor */
	reg = ( stNewROI.iLeft >> 8 ) & 0xff;
	_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SENSORCROP_LEFT_HIGH, reg );
	reg = stNewROI.iLeft & 0xff;
	_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SENSORCROP_LEFT_LOW, reg );
	
	reg = ( stNewROI.iTop >> 8 ) & 0xff;
	_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SENSORCROP_TOP_HIGH, reg );
	reg = stNewROI.iTop & 0xff;
	_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SENSORCROP_TOP_LOW, reg );

	reg = ( stNewROI.iWidth >> 8 ) & 0x0f;
	_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_DVPIN_WIDTH_HIGH, reg );
	reg = stNewROI.iWidth & 0xff;
	_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_DVPIN_WIDTH_LOW, reg );
	reg = ( stNewROI.iHeight >> 8 ) & 0x0f;
	_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_DVPIN_HEIGHT_HIGH, reg );
	reg = stNewROI.iHeight & 0xff;
	_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_DVPIN_HEIGHT_LOW, reg );

	// end group0
	// _set_sensor_reg( pState->stV4L2.iSensorFD, 0x3212, 0, 7, 0, 0x10 );

	// launch group0
	// _set_sensor_reg( pState->stV4L2.iSensorFD, 0x3212, 0, 7, 0, 0xa0 );

	// NOTE: skip 2 frames after digital zoom settings since sensor is unstable in the first 2 frames
	pState->stV4L2.iCurrentSkipCnt = 2;
#else	
	stCrop.type     = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	stCrop.c.width  = stNewROI.iWidth;
	stCrop.c.height = stNewROI.iHeight;
	stCrop.c.top    = stNewROI.iTop;
	stCrop.c.left   = stNewROI.iLeft;
	ret = ioctl( pState->stV4L2.iSensorFD, VIDIOC_S_CROP, &stCrop );
	ASSERT( ret == 0 );
#endif

	CELOG( "digital zoom ROI: left:%d, top:%d, width:%d, height:%d, zoom:%.2f\n", stNewROI.iLeft, stNewROI.iTop,
	       stNewROI.iWidth, stNewROI.iHeight, iDigitalZoomQ16 / 65536.0 );

	/* update AEC windows if not spot exposure metering mode */
	if ( pState->stV4L2.stShotParam.eExpMeterMode != CAM_EXPOSUREMETERMODE_SPOT )
	{
		reg = ( stNewROI.iWidth >> 8 ) & 0x0f;
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_WIN_XEND_HIGH, reg );
		reg = stNewROI.iWidth & 0xff;
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_WIN_XEND_LOW, reg );
		reg = ( stNewROI.iHeight >> 8 ) & 0x0f;
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_WIN_YEND_HIGH, reg );
		reg = stNewROI.iHeight & 0xff;
		_set_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_WIN_YEND_LOW, reg );
	}

	/* update digital zoom */
	pState->stV4L2.stShotParam.iDigZoomQ16 = iDigitalZoomQ16;

	return CAM_ERROR_NONE;
}

// get shot info
static CAM_Error _OV5642FillFrameShotInfo( OV5642State *pState, CAM_ImageBuffer *pImgBuf )
{
	// TODO: will contact sensor/module vendor to get the real value of these parameters

	CAM_Error    error      = CAM_ERROR_NONE;
	CAM_ShotInfo *pShotInfo = &(pImgBuf->stShotInfo);

	pShotInfo->iExposureTimeQ16    = (1 << 16) / 30;
	pShotInfo->iFNumQ16            = (int)( 2.8 * 65536 + 0.5 );
	pShotInfo->eExposureMode       = pState->stV4L2.stShotParam.eExpMode;
	pShotInfo->eExpMeterMode       = CAM_EXPOSUREMETERMODE_MEAN;
	pShotInfo->iISOSpeed           = 100;
	pShotInfo->iSubjectDistQ16     = 0;
	pShotInfo->iFlashStatus        = 0x0010;
	pShotInfo->iFocalLenQ16        = (int)( 3.79 * 65536 + 0.5 );
	pShotInfo->iFocalLen35mm       = 0;       

	return error;
}

static void _OV5642GetShotParamCap( CAM_ShotModeCapability *pShotModeCap )
{
	CAM_Int32s i = 0;

	// exposure mode
	pShotModeCap->iSupportedExpModeCnt  = 1;
	pShotModeCap->eSupportedExpMode[0]  = CAM_EXPOSUREMODE_AUTO;

	// exposure metering mode 
	pShotModeCap->iSupportedExpMeterCnt = _ARRAY_SIZE(_OV5642ExpMeterMode);
	for ( i = 0; i < pShotModeCap->iSupportedExpMeterCnt; i++ ) 
	{
		pShotModeCap->eSupportedExpMeter[i] = (CAM_ExposureMeterMode)_OV5642ExpMeterMode[i].iParameter;
	}

	// EV compensation 
	pShotModeCap->iMinEVCompQ16  = -78644;
	pShotModeCap->iMaxEVCompQ16  = 78644;
	pShotModeCap->iEVCompStepQ16 = 19661;

	// ISO mode 
	pShotModeCap->iSupportedIsoModeCnt = _ARRAY_SIZE(_OV5642IsoMode);
	for ( i = 0; i < pShotModeCap->iSupportedIsoModeCnt; i++ ) 
	{
		pShotModeCap->eSupportedIsoMode[i] = (CAM_ISOMode)_OV5642IsoMode[i].iParameter;
	}

	// shutter speed
	pShotModeCap->iMinShutSpdQ16 = -1;
	pShotModeCap->iMaxShutSpdQ16 = -1;

	// F-number
	pShotModeCap->iMinFNumQ16 = (CAM_Int32s)(2.8 * 65536 + 0.5);
	pShotModeCap->iMaxFNumQ16 = (CAM_Int32s)(2.8 * 65536 + 0.5);

	// band filter
	pShotModeCap->iSupportedBdFltModeCnt = _ARRAY_SIZE(_OV5642BdFltMode);
	for ( i = 0; i < pShotModeCap->iSupportedBdFltModeCnt; i++ ) 
	{
		pShotModeCap->eSupportedBdFltMode[i] = (CAM_BandFilterMode)_OV5642BdFltMode[i].iParameter;
	}

	// flash mode
	pShotModeCap->iSupportedFlashModeCnt = _ARRAY_SIZE(_OV5642FlashMode);
	for ( i = 0; i < pShotModeCap->iSupportedFlashModeCnt; i++ )
	{
		pShotModeCap->eSupportedFlashMode[i] = (CAM_FlashMode)_OV5642FlashMode[i].iParameter;
	}

	// white balance mode
	pShotModeCap->iSupportedWBModeCnt = _ARRAY_SIZE(_OV5642WBMode);
	for ( i = 0; i < pShotModeCap->iSupportedWBModeCnt; i++ )
	{
		pShotModeCap->eSupportedWBMode[i] = (CAM_ISOMode)_OV5642WBMode[i].iParameter;
	}

	// focus mode
	pShotModeCap->iSupportedFocusModeCnt = _ARRAY_SIZE( _OV5642FocusMode );
	for ( i = 0; i < _ARRAY_SIZE(_OV5642FocusMode); i++ )
	{
		pShotModeCap->eSupportedFocusMode[i] = (CAM_FocusMode)_OV5642FocusMode[i].iParameter;
	}

	// color effect
	pShotModeCap->iSupportedColorEffectCnt = _ARRAY_SIZE( _OV5642ColorEffectMode );
	for ( i = 0; i < pShotModeCap->iSupportedColorEffectCnt; i++ ) 
	{
		pShotModeCap->eSupportedColorEffect[i] = (CAM_ISOMode)_OV5642ColorEffectMode[i].iParameter;
	}

	// optical zoom mode
	pShotModeCap->iMinOptZoomQ16 = (CAM_Int32s)(1.0 * 65536 + 0.5);
	pShotModeCap->iMaxOptZoomQ16 = (CAM_Int32s)(1.0 * 65536 + 0.5);

	// digital zoom mode
	pShotModeCap->iMinDigZoomQ16        = (CAM_Int32s)(1.0 * 65536 + 0.5);
	pShotModeCap->iMaxDigZoomQ16        = (CAM_Int32s)(2.0 * 65536 + 0.5);
	pShotModeCap->iDigZoomStepQ16       = (CAM_Int32s)(0.2 * 65536 + 0.5);
	pShotModeCap->bSupportSmoothDigZoom = CAM_FALSE;

	// fps
	pShotModeCap->iMinFpsQ16 = 1 << 16;
	pShotModeCap->iMaxFpsQ16 = 40 << 16;

	// brightness
	pShotModeCap->iMinBrightness = -255;
	pShotModeCap->iMaxBrightness = 255;

	// contrast
	pShotModeCap->iMinContrast = 0;
	pShotModeCap->iMaxContrast = 255;

	// saturation
	pShotModeCap->iMinSaturation = 0;
	pShotModeCap->iMaxSaturation = 255;

	// sharpness
	pShotModeCap->iMinSharpness = 0;
	pShotModeCap->iMaxSharpness = 0;

	// advanced features
	pShotModeCap->bSupportVideoStabilizer   = CAM_FALSE;
	pShotModeCap->bSupportVideoNoiseReducer = CAM_FALSE;
	pShotModeCap->bSupportZeroShutterLag    = CAM_FALSE;
	pShotModeCap->bSupportHighDynamicRange  = CAM_FALSE;

	// take 6 shots burst capture as example: you can detemine
	// the maximum burst number by referencing sensor's fps capability
	pShotModeCap->bSupportBurstCapture      = CAM_TRUE;
	pShotModeCap->iMaxBurstCnt              = 6;

	return;
}

/*--------------------------------------------------------------------------------------------------------------------------------
 * OV5642 shotmode capability 
 *--------------------------------------------------------------------------------------------------------------------------------*/
static void _OV5642AutoModeCap( CAM_ShotModeCapability *pShotModeCap )
{
	CAM_Int32s i      = 0;
	CAM_Int32s iIndex = 0;

	// exposure mode
	pShotModeCap->iSupportedExpModeCnt  = 1;
	pShotModeCap->eSupportedExpMode[0]  = CAM_EXPOSUREMODE_AUTO;

	// exposure metering mode 
	pShotModeCap->iSupportedExpMeterCnt = _ARRAY_SIZE(_OV5642ExpMeterMode);
	for ( i = 0; i < pShotModeCap->iSupportedExpMeterCnt; i++ ) 
	{
		pShotModeCap->eSupportedExpMeter[i] = (CAM_ExposureMeterMode)_OV5642ExpMeterMode[i].iParameter;
	}

	// EV compensation 
	pShotModeCap->iMinEVCompQ16  = -78644;
	pShotModeCap->iMaxEVCompQ16  = 78644;
	pShotModeCap->iEVCompStepQ16 = 19661;

	// ISO mode 
	pShotModeCap->iSupportedIsoModeCnt = _ARRAY_SIZE(_OV5642IsoMode);
	for ( i = 0; i < pShotModeCap->iSupportedIsoModeCnt; i++ ) 
	{
		pShotModeCap->eSupportedIsoMode[i] = (CAM_ISOMode)_OV5642IsoMode[i].iParameter;
	}

	// shutter speed
	pShotModeCap->iMinShutSpdQ16 = -1;
	pShotModeCap->iMaxShutSpdQ16 = -1;

	// F-number
	pShotModeCap->iMinFNumQ16 = (CAM_Int32s)(2.8 * 65536 + 0.5);
	pShotModeCap->iMaxFNumQ16 = (CAM_Int32s)(2.8 * 65536 + 0.5);

	// band filter
	pShotModeCap->iSupportedBdFltModeCnt = _ARRAY_SIZE(_OV5642BdFltMode);
	for ( i = 0; i < pShotModeCap->iSupportedBdFltModeCnt; i++ ) 
	{
		pShotModeCap->eSupportedBdFltMode[i] = (CAM_BandFilterMode)_OV5642BdFltMode[i].iParameter;
	}

	// flash mode
	pShotModeCap->iSupportedFlashModeCnt = _ARRAY_SIZE(_OV5642FlashMode);
	for ( i = 0; i < pShotModeCap->iSupportedFlashModeCnt; i++ )
	{
		pShotModeCap->eSupportedFlashMode[i] = (CAM_FlashMode)_OV5642FlashMode[i].iParameter;
	}

	// white balance mode
	pShotModeCap->iSupportedWBModeCnt = _ARRAY_SIZE(_OV5642WBMode);
	for ( i = 0; i < pShotModeCap->iSupportedWBModeCnt; i++ )
	{
		pShotModeCap->eSupportedWBMode[i] = (CAM_ISOMode)_OV5642WBMode[i].iParameter;
	}

	// focus mode
	pShotModeCap->iSupportedFocusModeCnt = _ARRAY_SIZE(_OV5642FocusMode);
	iIndex = 0;
	for ( i = 0; i < _ARRAY_SIZE(_OV5642FocusMode); i++ )
	{
		if ( CAM_FOCUS_AUTO_CONTINUOUS_FACE == (CAM_FocusMode)_OV5642FocusMode[i].iParameter )
		{
			pShotModeCap->iSupportedFocusModeCnt--;
			continue;
		}
		else
		{
			pShotModeCap->eSupportedFocusMode[iIndex] = (CAM_FocusMode)_OV5642FocusMode[i].iParameter;
			iIndex++;
		}
	}

	// optical zoom mode
	pShotModeCap->iMinOptZoomQ16 = (CAM_Int32s)(1.0 * 65536 + 0.5);
	pShotModeCap->iMaxOptZoomQ16 = (CAM_Int32s)(1.0 * 65536 + 0.5);

	// digital zoom mode
	pShotModeCap->iMinDigZoomQ16        = (CAM_Int32s)(1.0 * 65536 + 0.5);
	pShotModeCap->iMaxDigZoomQ16        = (CAM_Int32s)(2.0 * 65536 + 0.5);
	pShotModeCap->iDigZoomStepQ16       = (CAM_Int32s)(0.2 * 65536 + 0.5);
	pShotModeCap->bSupportSmoothDigZoom = CAM_FALSE;

	// fps
	pShotModeCap->iMinFpsQ16 = 1 << 16;
	pShotModeCap->iMaxFpsQ16 = 40 << 16;

	// color effect
	pShotModeCap->iSupportedColorEffectCnt = _ARRAY_SIZE( _OV5642ColorEffectMode );
	for ( i = 0; i < pShotModeCap->iSupportedColorEffectCnt; i++ ) 
	{
		pShotModeCap->eSupportedColorEffect[i] = (CAM_ISOMode)_OV5642ColorEffectMode[i].iParameter;
	}

	// brightness
	pShotModeCap->iMinBrightness = -255;
	pShotModeCap->iMaxBrightness = 255;

	// contrast
	pShotModeCap->iMinContrast = 0;
	pShotModeCap->iMaxContrast = 255;

	// saturation
	pShotModeCap->iMinSaturation = 0;
	pShotModeCap->iMaxSaturation = 255;

	// sharpness
	pShotModeCap->iMinSharpness = 0;
	pShotModeCap->iMaxSharpness = 0;

	// advanced features
	pShotModeCap->bSupportVideoStabilizer   = CAM_FALSE;
	pShotModeCap->bSupportVideoNoiseReducer = CAM_FALSE;
	pShotModeCap->bSupportZeroShutterLag    = CAM_FALSE;
	pShotModeCap->bSupportHighDynamicRange  = CAM_FALSE;

	// take 6 shots burst capture as example: you can detemine
	// the maximum burst number by referencing sensor's fps capability
	pShotModeCap->bSupportBurstCapture      = CAM_TRUE;
	pShotModeCap->iMaxBurstCnt              = 6;

	return;
}

static void _OV5642PortraitModeCap( CAM_ShotModeCapability *pShotModeCap )
{
	CAM_Int32s i = 0;

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
	pShotModeCap->iSupportedFlashModeCnt = _ARRAY_SIZE(_OV5642FlashMode);
	for ( i = 0; i < pShotModeCap->iSupportedFlashModeCnt; i++ )
	{
		pShotModeCap->eSupportedFlashMode[i] = (CAM_FlashMode)_OV5642FlashMode[i].iParameter;
	}

	// white balance mode
	pShotModeCap->iSupportedWBModeCnt = 1;
	pShotModeCap->eSupportedWBMode[0] = CAM_WHITEBALANCEMODE_AUTO;

	// focus mode
	pShotModeCap->iSupportedFocusModeCnt = 1;
	pShotModeCap->eSupportedFocusMode[0] = CAM_FOCUS_AUTO_CONTINUOUS_FACE;

	// optical zoom mode
	pShotModeCap->iMinOptZoomQ16 = (CAM_Int32s)(1.0 * 65536 + 0.5);
	pShotModeCap->iMaxOptZoomQ16 = (CAM_Int32s)(1.0 * 65536 + 0.5);

	// digital zoom mode
	pShotModeCap->iMinDigZoomQ16        = (CAM_Int32s)(1.0 * 65536 + 0.5);
	pShotModeCap->iMaxDigZoomQ16        = (CAM_Int32s)(2.0 * 65536 + 0.5);
	pShotModeCap->iDigZoomStepQ16       = (CAM_Int32s)(0.2 * 65536 + 0.5);
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
	pShotModeCap->iMinContrast = 32;
	pShotModeCap->iMaxContrast = 32;

	// saturation
	pShotModeCap->iMinSaturation = 64;
	pShotModeCap->iMaxSaturation = 64;

	// sharpness
	pShotModeCap->iMinSharpness = 0;
	pShotModeCap->iMaxSharpness = 0;

	// advanced features
	pShotModeCap->bSupportVideoStabilizer	= CAM_FALSE;
	pShotModeCap->bSupportVideoNoiseReducer	= CAM_FALSE;
	pShotModeCap->bSupportZeroShutterLag	= CAM_FALSE;
	pShotModeCap->bSupportHighDynamicRange	= CAM_FALSE;

	// take 6 shots burst capture as example: you can detemine
	// the maximum burst number by referencing sensor's fps capability
	pShotModeCap->bSupportBurstCapture      = CAM_TRUE;
	pShotModeCap->iMaxBurstCnt              = 6;

	return;
}

static void _OV5642NightModeCap( CAM_ShotModeCapability *pShotModeCap )
{
	CAM_Int32s i      = 0;
	CAM_Int32s iIndex = 0;

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
	pShotModeCap->iSupportedIsoModeCnt = _ARRAY_SIZE(_OV5642IsoMode);
	for ( i = 0; i < pShotModeCap->iSupportedIsoModeCnt; i++ ) 
	{
		pShotModeCap->eSupportedIsoMode[i] = (CAM_ISOMode)_OV5642IsoMode[i].iParameter;
	}

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
	pShotModeCap->iSupportedFlashModeCnt = _ARRAY_SIZE(_OV5642FlashMode);
	for ( i = 0; i < pShotModeCap->iSupportedFlashModeCnt; i++ )
	{
		pShotModeCap->eSupportedFlashMode[i] = (CAM_FlashMode)_OV5642FlashMode[i].iParameter;
	}

	// white balance mode
	pShotModeCap->iSupportedWBModeCnt = 1;
	pShotModeCap->eSupportedWBMode[0] = CAM_WHITEBALANCEMODE_AUTO;

	// focus mode
	pShotModeCap->iSupportedFocusModeCnt = _ARRAY_SIZE(_OV5642FocusMode);
	iIndex = 0;
	for ( i = 0; i < _ARRAY_SIZE(_OV5642FocusMode); i++ )
	{
		if ( CAM_FOCUS_AUTO_CONTINUOUS_FACE == (CAM_FocusMode)_OV5642FocusMode[i].iParameter )
		{
			pShotModeCap->iSupportedFocusModeCnt--;
			continue;
		}
		else
		{
			pShotModeCap->eSupportedFocusMode[iIndex] = (CAM_FocusMode)_OV5642FocusMode[i].iParameter;
			iIndex++;
		}
	}

	// optical zoom mode
	pShotModeCap->iMinOptZoomQ16 = (CAM_Int32s)(1.0 * 65536 + 0.5);
	pShotModeCap->iMaxOptZoomQ16 = (CAM_Int32s)(1.0 * 65536 + 0.5);

	// digital zoom mode
	pShotModeCap->iMinDigZoomQ16        = (CAM_Int32s)(1.0 * 65536 + 0.5);
	pShotModeCap->iMaxDigZoomQ16        = (CAM_Int32s)(2.0 * 65536 + 0.5);
	pShotModeCap->iDigZoomStepQ16       = (CAM_Int32s)(0.2 * 65536 + 0.5);
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
	pShotModeCap->iMinContrast = 32;
	pShotModeCap->iMaxContrast = 32;

	// saturation
	pShotModeCap->iMinSaturation = 64;
	pShotModeCap->iMaxSaturation = 64;

	// sharpness
	pShotModeCap->iMinSharpness = 0;
	pShotModeCap->iMaxSharpness = 0;

	// advanced features
	pShotModeCap->bSupportVideoStabilizer   = CAM_FALSE;
	pShotModeCap->bSupportVideoNoiseReducer = CAM_FALSE;
	pShotModeCap->bSupportZeroShutterLag    = CAM_FALSE;
	pShotModeCap->bSupportHighDynamicRange  = CAM_FALSE;

	// take 3 shots burst capture as example: you can detemine
	// the maximum burst number by referencing sensor's fps capability
	pShotModeCap->bSupportBurstCapture      = CAM_TRUE;
	pShotModeCap->iMaxBurstCnt              = 3;

	return;
}

/*--------------------------------------------------------------------------------------------------------------------------------
 * OV5642 utility functions part
 *-------------------------------------------------------------------------------------------------------------------------------*/

static CAM_Int32s _ov5642_wait_afcmd_ready( CAM_Int32s iSensorFD )
{
	CAM_Int16u reg = 0;
	CAM_Int32s cnt = 3000;
	while ( cnt-- )
	{
		_get_sensor_reg( iSensorFD, OV5642_REG_AF_TAG, &reg );
		if ( reg == 0x00 )
		{
			// TRACE( CAM_INFO, "Info: AF cmd convergence( %s, %d )\n", __FILE__, __LINE__ );
			return 0;
		}
		(void)IPP_Sleep( 1000 );
	}

	return -1;
}


static CAM_Int32s _ov5642_set_paramsreg( CAM_Int32s param, _CAM_ParameterEntry *pOptionArray, CAM_Int32s iOptionSize, CAM_Int32s iSensorFD )
{
	CAM_Int32s i   = 0;
	CAM_Int32s ret = 0;

	for ( i = 0; i < iOptionSize; i++ ) 
	{
		if ( param == pOptionArray[i].iParameter ) 
		{
			break;
		}
	}
	
	if ( i >= iOptionSize ) 
	{
		return -2;
	}

	if ( pOptionArray[i].stSetParam.stRegTable.pEntries[0].reg == 0x0000 )
	{
		// NOTE: in this case, no sensor registers need to be set
		ret = 0;
	}
	else
	{
		ret = _set_reg_array( iSensorFD, pOptionArray[i].stSetParam.stRegTable.pEntries, pOptionArray[i].stSetParam.stRegTable.iLength );
	}

	return ret;

}


static CAM_Int32s _ov5642_set_paramsreg_nearest( CAM_Int32s *pParam, _CAM_ParameterEntry *pOptionArray, CAM_Int32s iOptionSize, CAM_Int32s iSensorFD )
{
	CAM_Int32s i = 0, index;
	CAM_Int32s ret = 0;
	CAM_Int32s diff, min_diff;

	index = 0;
	min_diff = _ABSDIFF( pOptionArray[0].iParameter, *pParam );

	for ( i = 0; i < iOptionSize; i++ ) 
	{
		diff = _ABSDIFF( pOptionArray[i].iParameter, *pParam );
		if ( diff < min_diff ) 
		{
			min_diff = diff;
			index = i;
		}
	}

	ret = _set_reg_array( iSensorFD, pOptionArray[index].stSetParam.stRegTable.pEntries, pOptionArray[index].stSetParam.stRegTable.iLength );

	*pParam = pOptionArray[index].iParameter;

	return ret;

}

/*
*@ OV5642 clac system clock 
*/
static inline CAM_Error _ov5642_calc_sysclk( CAM_Int32s iSensorFD, double *pSysClk )
{

	double pre_div_map[8]   = { 1, 1.5, 2, 2.5, 3, 4, 6, 8 };
	double div1to2p5_map[4] = { 1, 1, 2, 2.5 };
	int    m1_div_map[2]    = { 1, 2 };
	int    seld_map[4]      = { 1, 1, 4, 5 }; 
	int    divs_map[8]      = { 1, 2, 3, 4, 5, 6, 7, 8 };

	double     vco_freq;
	double     pre_div, div1to2p5;
	int        m1_div, divp, divs, seld;
	CAM_Int16u reg;

	// vco frequency
	reg = 0;   
	_get_sensor_reg( iSensorFD, 0x3012, 0, 2, 0, &reg );
	pre_div = pre_div_map[reg];

	reg = 0;
	_get_sensor_reg( iSensorFD, 0x3011, 0, 5, 0, &reg );
	divp = reg;

	reg = 0;
	_get_sensor_reg( iSensorFD, 0x300f, 0, 1, 0, &reg );
	seld = seld_map[reg];

	vco_freq = PLATFORM_CCIC_SENSOR_MCLK / pre_div * divp * seld;
	// TRACE( CAM_INFO, "vco_freq: %.2f\n", vco_freq );
	CELOG( "vco_freq: %.2f\n", vco_freq );

	// system clk: the clk before ISP
	reg = 0;
	_get_sensor_reg( iSensorFD, 0x3010, 4, 7, 0, &reg );
	divs = divs_map[reg];

	reg = 0;
	_get_sensor_reg( iSensorFD, 0x300f, 0, 1, 0, &reg );
	div1to2p5 = div1to2p5_map[reg];

	reg = 0;
	_get_sensor_reg( iSensorFD, 0x3029, 0, 0, 0, &reg );
	m1_div = m1_div_map[reg];

	*pSysClk = vco_freq / divs / div1to2p5 / m1_div / 4;
	// TRACE( CAM_INFO, "sys_clk: %.2f\n", *pSysClk );
	CELOG( "sys_clk: %.2f\n", *pSysClk );

	return CAM_ERROR_NONE;

}

/*
*@ OV5642 hts & vts 
*/
static inline CAM_Error _ov5642_calc_hvts( CAM_Int32s iSensorFD, CAM_Int32s *pHts, CAM_Int32s *pVts )
{
	CAM_Int16u reg;

	// vts
	reg = 0;
	_get_sensor_reg( iSensorFD, OV5642_REG_TIMINGVTSHIGH, &reg );
	*pVts = (reg & 0x000f);
	*pVts <<= 8;

	reg = 0;
	_get_sensor_reg( iSensorFD, OV5642_REG_TIMINGVTSLOW, &reg );
	*pVts += (reg & 0x00ff);
	CELOG( "vts: %d\n", *pVts );

	// hts
	reg = 0;
	_get_sensor_reg( iSensorFD, OV5642_REG_TIMINGHTSHIGH, &reg );
	*pHts = (reg & 0x000f);
	*pHts <<= 8;

	reg = 0;
	_get_sensor_reg( iSensorFD, OV5642_REG_TIMINGHTSLOW, &reg );
	*pHts += (reg & 0x00ff);
	CELOG( "hts: %d\n", *pHts );

	return CAM_ERROR_NONE;
}

/*
*@ OV5642 frame rate calc, reference formula:
*frame_rate = sys_clk * 1000000 / ( HTS * VTS );
*sys_clk = vco_freq / divs / div1to2p5 / m1_div / 4;
*vco_freq = ref_clk / pre_div * divp * seld5
*/
static CAM_Error _ov5642_calc_framerate( CAM_Int32s iSensorFD, CAM_Int32s *pFrameRate100 )
{
	int    hts, vts;
	double sys_clk;
	double frame_rate;

	// get sensor system clk
	_ov5642_calc_sysclk( iSensorFD, &sys_clk );

	// get sensor hts & vts
	_ov5642_calc_hvts( iSensorFD, &hts, &vts );

	frame_rate = sys_clk * 1000000 / ( hts * vts );  
	// TRACE( CAM_INFO, "frame_rate: %.2f\n", frame_rate );
	CELOG( "frame_rate: %.2f\n", frame_rate );

	*pFrameRate100 = (CAM_Int32s)(frame_rate * 100);

	return CAM_ERROR_NONE;

}

/******************************************************************************************************
//
// Name:         _ov5642_save_settings
// Description:  save preview's AEC/AGC,AWB,and VTS(Vertical Total Size) values 
// Arguments:    pCurrent3A[OUTPUT]: pointer to the structure for save AE/AG/VTS values
// Return:       camera engine error code,refer to CameraEngine.h for more detail
// Notes:        
//     1) for more detail about OV5642 register, pls refer to its software application note
// Version Log:  version      Date          Author    Description
//                v0.1      07/14/2010    Matrix Yao   Create
//                v0.2      07/30/2010    Matrix Yao   rename function, add save AWB content
*******************************************************************************************************/

static CAM_Error _ov5642_save_settings( CAM_Int32s iSensorFD, _OV5642_3AState *pCurrent3A )
{
	CAM_Int16u  reg      = 0; 

	CAM_Int16u exposure  = 0;
	CAM_Int16u gain      = 0;
	CAM_Int32u maxLines  = 0;

	// CAM_Int16u awbRgain      = 0;
	// CAM_Int16u awbGgain      = 0;
	// CAM_Int16u awbBgain      = 0;

	// BAC check
	_CHECK_BAD_POINTER( pCurrent3A );


	// T0DO: check if AEC/AGC convergence
	if ( 0 )
	{
		;
	}

	/* frame rate */
	_ov5642_calc_framerate( iSensorFD, &(pCurrent3A->fps100) );

	// stop AEC/AGC
	reg = 0x0007;
	_set_sensor_reg( iSensorFD, OV5642_REG_AECAGC, reg );

	/* gain */
	reg = 0;
	_get_sensor_reg( iSensorFD, OV5642_REG_GAINLOW, &reg );
	gain = (reg & 0x000f) + 16;
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
	_get_sensor_reg( iSensorFD, OV5642_REG_EXPOSUREHIGH, &reg );
	exposure = (reg & 0x000f);
	exposure <<= 12;

	reg = 0;
	_get_sensor_reg( iSensorFD, OV5642_REG_EXPOSUREMID, &reg );
	reg = (reg & 0x00ff);
	exposure += (reg << 4);

	reg = 0;
	_get_sensor_reg( iSensorFD, OV5642_REG_EXPOSURELOW, &reg );
	reg = (reg & 0x00f0);
	exposure += (reg >> 4);

	pCurrent3A->exposure = exposure;

	/* exposure VTS */
	reg = 0;
	_get_sensor_reg( iSensorFD, OV5642_REG_TIMINGVTSHIGH, &reg );
	maxLines = (reg & 0x00ff);
	maxLines <<= 8;

	reg = 0;
	_get_sensor_reg( iSensorFD, OV5642_REG_TIMINGVTSLOW, &reg );
	maxLines += ( reg & 0x00ff);

	pCurrent3A->maxLines = maxLines;
	
	/* white balance 
	 * OV declares that restore white balance is unnecessary
	 */
	/*
	reg = 0;
	_get_sensor_reg( iSensorFD, OV5642_REG_CURRENT_AWBRHIGH, &reg );
	awbRgain = ( reg & 0x000f );
	awbRgain <<= 8;
	reg = 0;
	_get_sensor_reg( iSensorFD, OV5642_REG_CURRENT_AWBRLOW, &reg );
	awbRgain |= reg;
	pCurrent3A->awbRgain = awbRgain;

	reg = 0;
	_get_sensor_reg( iSensorFD, OV5642_REG_CURRENT_AWBGHIGH, &reg );
	awbGgain = ( reg & 0x000f );
	awbGgain <<= 8;
	reg = 0;
	_get_sensor_reg( iSensorFD, OV5642_REG_CURRENT_AWBGLOW, &reg );
	awbGgain |= reg;
	pCurrent3A->awbGgain = awbGgain;

	reg = 0;
	_get_sensor_reg( iSensorFD, OV5642_REG_CURRENT_AWBBHIGH, &reg );
	awbBgain = ( reg & 0x000f );
	awbBgain <<= 8;
	reg = 0;
	_get_sensor_reg( iSensorFD, OV5642_REG_CURRENT_AWBBLOW, &reg );
	awbBgain |= reg;
	pCurrent3A->awbBgain = awbBgain;
	*/

	/* binning factor */
	reg = 0;
	_get_sensor_reg( iSensorFD, OV5642_REG_BIN_HORIZONTAL, &reg );
	pCurrent3A->binFactor = ( 1 << reg );

	reg = 0;
	_get_sensor_reg( iSensorFD, OV5642_REG_BIN_VERTICAL, &reg );
	pCurrent3A->binFactor <<= reg;
	TRACE( CAM_INFO, "bin factor:%d\n ", pCurrent3A->binFactor );

	return  CAM_ERROR_NONE;
}

/******************************************************************************************************
//
// Name:         _ov5642_calc_new3A
// Description:  calculate capture exposure from preview exposure
// Arguments:    p3Astat[INPUT/OUTPUT] : preview's AEC paras(exposure\gain\maxlines)
// Return:       camera engine error code,refer to CameraEngine.h for more detail
// Notes:        
//     1) for more detail about OV5642 register, pls refer to its software application note
// Version Log:  version      Date          Author    Description
//                v0.1      07/14/1010    Matrix Yao   Create
//                v0.2      07/30/2010    Matrix Yao   rename function,etc
*******************************************************************************************************/

static CAM_Error _ov5642_calc_new3A( CAM_Int32s iSensorFD, _OV5642_3AState *p3Astat )
{
	CAM_Int16u  reg = 0;

	// line_10ms is Banding Filter Value
	// here 10ms means the time unit is 10 mili-seconds
	CAM_Int32u lines_10ms        = 0;
	CAM_Int32u newMaxLines       = 0;
	CAM_Int32s newFrameRate100;
	CAM_Int32s newBinFactor;

	CAM_Int16u newGainQ4         = 0;
	CAM_Int64u newExposure       = 0;
	CAM_Int64u newExposureGain   = 0;

	// get capture fps
	_ov5642_calc_framerate( iSensorFD, &newFrameRate100 );

	// stop AEC/AGC
	reg = 0x0007;
	_set_sensor_reg( iSensorFD, OV5642_REG_AECAGC, reg );

	// get capture exposure VTS
	reg = 0;
	_get_sensor_reg( iSensorFD, OV5642_REG_TIMINGVTSHIGH, &reg );
	newMaxLines = (reg & 0x00ff);
	newMaxLines <<= 8;

	reg = 0;
	_get_sensor_reg( iSensorFD, OV5642_REG_TIMINGVTSLOW, &reg );
	newMaxLines += (reg & 0x00ff);
	

	// get capture's banding filter value
	// TODO: 50Hz flicker as example
	lines_10ms = newFrameRate100 * newMaxLines / 10000;


	if ( p3Astat->maxLines == 0 )
	{
		p3Astat->maxLines = 1;
	}

	if ( lines_10ms == 0 ) 
	{
		lines_10ms = 1;
	}

	// get capture bin factor
	reg = 0;
	_get_sensor_reg( iSensorFD, OV5642_REG_BIN_HORIZONTAL, &reg );
	newBinFactor = ( 1 << reg );

	reg = 0;
	_get_sensor_reg( iSensorFD, OV5642_REG_BIN_VERTICAL, &reg );
	newBinFactor <<= reg;

	TRACE( CAM_INFO, "new bin factor:%d\n ", newBinFactor );

	// calculate capture exposure, in uint of t_ROW_INTERVAL
	newExposure = (CAM_Int64u)p3Astat->exposure * (CAM_Int64u)newMaxLines * (CAM_Int64u)newFrameRate100 * (CAM_Int64u) p3Astat->binFactor / (CAM_Int64u)p3Astat->maxLines / (CAM_Int64u)p3Astat->fps100 / (CAM_Int64u)newBinFactor;

	// calculate exposure * gain for capture mode
	newExposureGain = newExposure * (CAM_Int64u)p3Astat->gainQ4;

	// get exposure for capture. 
	if ( newExposureGain < ((CAM_Int64u)newMaxLines << 4) )
	{
		newExposure = (newExposureGain >> 4);
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
	newGainQ4 = (CAM_Int16u)( (newExposureGain * 2 / newExposure + 1) / 2 );

	// update exposure and gain
	p3Astat->maxLines = newMaxLines;

	p3Astat->exposure = (CAM_Int16u)newExposure;

	p3Astat->gainQ4 = (CAM_Int16u)newGainQ4;

	// update fps
	p3Astat->fps100 = newFrameRate100;

	// update bin factor
	p3Astat->binFactor = newBinFactor;

	// white balance no need to re-calc

	return  CAM_ERROR_NONE;
}


/******************************************************************************************************
//
// Name:         _ov5642_set_new_exposure
// Description:  set capture exposure(exposure\gain\maxlines) manually
// Arguments:    pNew3A[INPUT]: capture's AEC paras(exposure\gain\maxlines)
// Return:       camera engine error code,refer to CameraEngine.h for more detail
// Notes:        
//     1) for more detail about OV5642 register, pls refer to its software application note;
//     2) pls update current AEC state once this function return CAM_ERROR_NONE;
// Version Log:  version      Date          Author    Description
//                v0.1      07/14/1010    Matrix Yao   Create
//                v0.2      07/30/2010    Matrix Yao   rename function,etc 
*******************************************************************************************************/

static CAM_Error _ov5642_set_new_exposure( CAM_Int32s iSensorFD, _OV5642_3AState *pNew3A )
{
	CAM_Int8u  exposureLow  = 0;
	CAM_Int8u  exposureMid  = 0;
	CAM_Int8u  exposureHigh = 0;

	CAM_Int16u gain         = 0;

	CAM_Int16u gainQ4       = 0;
	CAM_Int16u reg          = 0;
	CAM_Int16u base         = 0;

	CAM_Int32s  ret         = 0;

	// calculate exposure value for OV5642 register
	exposureLow  = (CAM_Int8u)(((pNew3A->exposure) & 0x000f) << 4);
	exposureMid  = (CAM_Int8u)(((pNew3A->exposure) & 0x0ff0) >> 4);
	exposureHigh = (CAM_Int8u)(((pNew3A->exposure) & 0xf000) >> 12);

	// calculate gain value for OV5642 register
	gainQ4 = pNew3A->gainQ4;
	base = 0x10;
	while ( gainQ4 > 31 )
	{
		gain   |=  base;
		gainQ4 >>= 1;
		base   <<= 1;
	}

	if ( gainQ4 > 16 )
	{
		gain |= ((gainQ4 -16) & 0x0f);
	}  

	// I still don't know the real meaning of the following statement 
	if ( gain == 0x10 )
	{
		gain = 0x11;
	}  

	// write gain and exposure to OV5642 registers

	/* gain */
	reg = (CAM_Int16u)(gain & 0xff);
	ret = _set_sensor_reg( iSensorFD, OV5642_REG_GAINLOW, reg );

	reg = (CAM_Int16u)((gain >> 8) & 0x1);
	ret = _set_sensor_reg( iSensorFD, OV5642_REG_GAINHIGH, reg );

	/* exposure */
	reg = (CAM_Int16u)exposureLow;
	ret = _set_sensor_reg( iSensorFD, OV5642_REG_EXPOSURELOW, reg );

	reg = (CAM_Int16u)exposureMid;
	ret = _set_sensor_reg( iSensorFD, OV5642_REG_EXPOSUREMID, reg );

	reg = (CAM_Int16u)exposureHigh;
	ret = _set_sensor_reg( iSensorFD, OV5642_REG_EXPOSUREHIGH, reg );

	/* white balance 
	 * OV declares that restore white balance is unnecessary
	 */
	/*
	reg = 0x0083;
	ret = _set_sensor_reg( iSensorFD, OV5642_REG_AWBC_22, reg );

	reg = (pNew3A->awbRgain & 0x0f00);
	reg = (reg >> 8);
	ret = _set_sensor_reg( iSensorFD, OV5642_REG_SET_AWBRHIGH, reg );
	reg = (pNew3A->awbRgain & 0x00ff);
	ret = _set_sensor_reg( iSensorFD, OV5642_REG_SET_AWBRLOW, reg );

	reg = (pNew3A->awbGgain & 0x0f00);
	reg = (reg >> 8);
	ret = _set_sensor_reg( iSensorFD, OV5642_REG_SET_AWBGHIGH, reg );
	reg = (pNew3A->awbGgain & 0x00ff);
	ret = _set_sensor_reg( iSensorFD, OV5642_REG_SET_AWBGLOW, reg );

	reg = (pNew3A->awbBgain & 0x0f00);
	reg = (reg >> 8);
	ret = _set_sensor_reg( iSensorFD, OV5642_REG_SET_AWBBHIGH, reg );
	reg = (pNew3A->awbBgain & 0x00ff);
	ret = _set_sensor_reg( iSensorFD, OV5642_REG_SET_AWBBLOW, reg );
	*/

	return  CAM_ERROR_NONE;
}


// for face detection

// grayscale image generation
static CAM_Int32s _YCbCr422ToGrayRsz_8u_C2_C( const Ipp8u *pSrc, int srcStep,  IppiSize srcSize, 
                                              Ipp8u *pDst, int dstStep, IppiSize dstSize )
{
	int    x, y;
	Ipp32s ay, fx, fx1, ay1;
	int    dstWidth, dstHeight;
	Ipp32s xLimit, yLimit;
	Ipp8u  *pDstY; 
	Ipp8u  *pSrcY1, *pSrcY2;
	Ipp8u  *pDstY1, *pDstY2, *pDstY3, *pDstY4;
	int    stepX, stepY;
	Ipp32s yLT, yRT, yLB, yRB, yT, yB, yR;
	Ipp8u  Y1, Y2, Y3, Y4;
	int    col, dcol, drow;
	int    xNum, yNum;
	int    rcpRatiox, rcpRatioy;

	ASSERT( pSrc && pDst );

	dstWidth  = dstSize.width;
	dstHeight = dstSize.height;

	rcpRatiox = ( ( ( srcSize.width & ~1 ) - 1 ) << 16 ) / ( ( dstSize.width & ~1 ) - 1 );
	rcpRatioy = ( ( ( srcSize.height & ~1 ) - 1 ) << 16 ) / ( ( dstSize.height & ~1 ) - 1 );

	xLimit = (srcSize.width - 1) << 16;
	yLimit = (srcSize.height - 1) << 16;
	xNum   = dstSize.width;
	yNum   = dstSize.height;

	ay      = 0;
	ay1     = rcpRatioy;
	pSrcY1  = (Ipp8u*)(pSrc + 1);
	pSrcY2  = (Ipp8u*)(pSrcY1 + (ay1 >> 16) * srcStep);
	pDstY   = (Ipp8u*)pDst;

	// ippCameraRotateDisable
	pDstY1 = pDstY;
	pDstY2 = pDstY + 1;
	pDstY3 = pDstY + dstStep;
	pDstY4 = pDstY3 + 1;
	stepX  = 2;
	stepY  = 2 * dstStep - dstSize.width;

	for( y = 0; y < yNum; y += 2 ) 
	{
		fx = 0;
		fx1 = rcpRatiox;
		for( x = 0; x < xNum; x += 2 ) 
		{
			col = fx >> 16;
			col = col * 2;
			dcol = fx & 0xffff;
			drow = ay & 0xffff;
			yLT = pSrcY1[col];
			yRT = pSrcY1[col + 2];
			yLB = pSrcY1[col + srcStep];
			yRB = pSrcY1[col + srcStep + 2];

			yT = ((dcol * (yRT - yLT)) >> 16) + yLT;
			yB = ((dcol * (yRB - yLB)) >> 16) + yLB;
			yR = ((drow * (yB - yT)) >> 16) + yT;
			Y1 = (Ipp8u)yR;

			col = fx1 >> 16;
			col = col * 2;
			dcol = fx1 & 0xffff;
			drow = ay & 0xffff;
			yLT = pSrcY1[col];
			yLB = pSrcY1[col + srcStep];

			if ( fx1 >= xLimit )
			{
				yRT = pSrcY1[col];
				yRB = pSrcY1[col + srcStep];
			}
			else
			{
				yRT = pSrcY1[col + 2];
				yRB = pSrcY1[col + srcStep + 2];
			}

			yT = ((dcol * (yRT - yLT)) >> 16) + yLT;
			yB = ((dcol * (yRB - yLB)) >> 16) + yLB;
			yR = ((drow * (yB - yT)) >> 16) + yT;
			Y2 = (Ipp8u)yR;

			col = fx >> 16;
			col = col * 2;
			dcol = fx & 0xffff;
			drow = ay1 & 0xffff;
			yLT = pSrcY2[col];
			yRT = pSrcY2[col + 2];
			yLB = pSrcY2[col + srcStep];
			yRB = pSrcY2[col + srcStep + 2];
			yT = ((dcol * (yRT - yLT)) >> 16) + yLT;
			yB = ((dcol * (yRB - yLB)) >> 16) + yLB;
			yR = ((drow * (yB - yT)) >> 16) + yT;
			Y3 = (Ipp8u)yR;

			col = fx1 >> 16;
			col = col * 2;
			dcol = fx1 & 0xffff;
			drow = ay1 & 0xffff;
			yLT = pSrcY2[col];
			yLB = pSrcY2[col + srcStep];

			if ( fx1 >= xLimit )
			{
				yRT = pSrcY2[col];
				yRB = pSrcY2[col + srcStep];
			}
			else
			{
				yRT = pSrcY2[col + 2];
				yRB = pSrcY2[col + srcStep + 2];
			}

			yT = ((dcol * (yRT - yLT)) >> 16) + yLT;
			yB = ((dcol * (yRB - yLB)) >> 16) + yLB;
			yR = ((drow * (yB - yT)) >> 16) + yT;
			Y4 = (Ipp8u)yR;

			fx += rcpRatiox << 1;
			fx1 += rcpRatiox << 1;

			/* 2.store Y */
			*pDstY1 = Y1;
			pDstY1 += stepX;
			*pDstY2 = Y2;
			pDstY2 += stepX;
			*pDstY3 = Y3;
			pDstY3 += stepX;
			*pDstY4 = Y4;
			pDstY4 += stepX;
		}
		pDstY1 += stepY;
		pDstY2 += stepY;
		pDstY3 += stepY;
		pDstY4 += stepY;

		ay  += 2 * rcpRatioy;
		ay1 += 2 * rcpRatioy;
		if( ay1 >= yLimit )
		{
			ay1 = yLimit - 1;
		}

		pSrcY1 = (Ipp8u*)(pSrc + 1 + (ay >> 16) * srcStep);
		pSrcY2 = (Ipp8u*)(pSrc + 1 + (ay1 >> 16) * srcStep);
	}

	return 0;
}

/*
// pImgBuf: src image, only CAM_IMGFMT_YCC420P( planar format ) and CAM_IMGFMT_CbYCrY( packet ) supported.
// pDstBuf: pointer of Y data. 8 byte align requirment.
*/
static CAM_Error _YUVToGrayRsz_8u( CAM_ImageBuffer *pImgBuf, CAM_Int8u *pDstBuf, CAM_Size stDstImgSize )
{
	IppiSize           stSrcSize, stDstSize;
	CAM_Int32s         ret = 0;

	if ( pImgBuf == NULL || pDstBuf == NULL || stDstImgSize.iWidth <= 0 || stDstImgSize.iHeight <= 0 )
	{
		return CAM_ERROR_BADARGUMENT;
	}

	stSrcSize.width     = pImgBuf->iWidth;
	stSrcSize.height    = pImgBuf->iHeight;
	stDstSize.width     = stDstImgSize.iWidth;
	stDstSize.height    = stDstImgSize.iHeight;

	if ( pImgBuf->eFormat == CAM_IMGFMT_YCbCr420P )
	{
		CAM_Int8u  *pHeap   = NULL;
		CAM_Int8u  *pBuf[3] = {NULL};
		CAM_Int32s iStep[3] = {0};
		CAM_Int32s rcpRatioxQ16, rcpRatioyQ16;
		IppStatus  eIppRet  = ippStsNoErr;
		
		pHeap = (CAM_Int8u*)malloc( stDstSize.width * ( stDstSize.height >> 1 ) + 128 );
		if ( pHeap == NULL )
		{
			TRACE( CAM_ERROR, "Error: no enough memory for face detection( %s, %d )\n", __FILE__, __LINE__ );
			return CAM_ERROR_OUTOFMEMORY;
		}
		
		iStep[0] = stDstSize.width;
		iStep[1] = stDstSize.width >> 1;
		iStep[2] = stDstSize.width >> 1;
		pBuf[0]  = pDstBuf;
		pBuf[1]  = (CAM_Int8u*)_ALIGN_TO( pHeap, 8 );
		pBuf[2]  = (CAM_Int8u*)_ALIGN_TO( pBuf[1] + iStep[1] * ( stDstSize.height >> 1 ), 8 );

		rcpRatioxQ16 = ( ( ( stSrcSize.width & ~1 ) - 1 ) << 16 ) / ( ( stDstSize.width & ~1 ) - 1 );
		rcpRatioyQ16 = ( ( ( stSrcSize.height & ~1 ) - 1 ) << 16 ) / ( ( stDstSize.height & ~1 ) - 1 );
		
		if ( stSrcSize.width == stDstSize.width && stSrcSize.height == stDstSize.height )
		{
			// just copy Y here
			memmove( pBuf[0], pImgBuf->pBuffer[0], pImgBuf->iFilledLen[0] );
		}
		else
		{
			eIppRet = ippiYCbCr420RszRot_8u_P3R( (const Ipp8u**)pImgBuf->pBuffer, pImgBuf->iStep, stSrcSize,
			                                     pBuf, iStep, stDstSize,
		                                         ippCameraInterpBilinear, ippCameraRotateDisable, rcpRatioxQ16, rcpRatioyQ16 );
		}
		
		ASSERT( eIppRet == ippStsNoErr );
		free( pHeap );
	}
	else if ( pImgBuf->eFormat == CAM_IMGFMT_CbYCrY )
	{
		/*
		CELOG( "src buffer: %p, src buffer step: %d, src size: %d * %d, dst buffer: %p, dst step: %d, dst size: %d * %d\n", 
		       pImgBuf->pBuffer[0], pImgBuf->iStep[0], stSrcSize.width, stSrcSize.height,
		       pDstBuf, stDstSize.width, stDstSize.width, stDstSize.height );
		*/
		ret = _YCbCr422ToGrayRsz_8u_C2_C( (const Ipp8u*)pImgBuf->pBuffer[0], pImgBuf->iStep[0], stSrcSize,
		                                   pDstBuf, stDstSize.width, stDstSize );
		ASSERT( ret == 0 );
	}
	else
	{
		TRACE( CAM_ERROR, "Error: unsupported format[%d] ( %s, %d )\n", pImgBuf->eFormat, __FILE__, __LINE__ );
		return CAM_ERROR_NOTIMPLEMENTED;
	}

#if 0
         {
		FILE   *pSrcFile = NULL;
		FILE   *pDstFile = NULL;
		pSrcFile = fopen ( "/data/preview.yuv","w" );
		pDstFile = fopen ( "/data/face.y", "w" );

		fwrite ( pImgBuf->pBuffer[0], 1, 2 * stSrcSize.width * stSrcSize.height, pSrcFile );
		fwrite ( pDstBuf, 1, stDstSize.width * stDstSize.height, pDstFile );
		fclose( pSrcFile );
		fclose( pDstFile );
	 }
#endif
	return CAM_ERROR_NONE;
}

// face detection lib callback function
unsigned int _OV5642FaceDetectCallback( unsigned int value )
{
	CAM_Int16u  reg;
	CAM_Int8u   ucIn[4];
	CAM_Int32u  tmp  = 0;
	CAM_Int32u  ret  = 0;
	CAM_Int32s  cnt  = 1000;
		
	ucIn[0] = value & 0xff;
	ucIn[1] = ( value >> 8 ) & 0xff;
	ucIn[2] = ( value >> 16 ) & 0xff;
	ucIn[3] = ( value >> 24 ) & 0xff;

	_set_sensor_reg( giSensorFD, OV5642_REG_AF_TAG,   0x01 );
	reg = ucIn[3] & 0xff;
	_set_sensor_reg( giSensorFD, OV5642_REG_AF_PARA3, reg );
	reg = ucIn[2] & 0xff;
	_set_sensor_reg( giSensorFD, OV5642_REG_AF_PARA2, reg );
	reg = ucIn[1] & 0xff;
	_set_sensor_reg( giSensorFD, OV5642_REG_AF_PARA1, reg );
	reg = ucIn[0] & 0xff;
	_set_sensor_reg( giSensorFD, OV5642_REG_AF_PARA0, reg );
	
	_set_sensor_reg( giSensorFD, OV5642_REG_AF_MAIN,  0xec );

	reg = 1;
	while( cnt-- )
	{
		_get_sensor_reg( giSensorFD,  OV5642_REG_AF_MAIN, &reg );
		if( reg == 0 )
		{
			break;
		}
		IPP_Sleep( 1000 );
	}
	ASSERT( cnt >= 0 );

	reg = 0;
	_get_sensor_reg( giSensorFD,  OV5642_REG_AF_PARA3, &reg );
	tmp = reg;
	ret = (tmp << 24);
	
	reg = 0;
	_get_sensor_reg( giSensorFD,  OV5642_REG_AF_PARA2, &reg );
	tmp = reg;
	ret += (tmp << 16);
	
	reg = 0;
	_get_sensor_reg( giSensorFD,  OV5642_REG_AF_PARA1, &reg );
	tmp = reg;
	ret += (tmp << 8);
	
	reg = 0;
	_get_sensor_reg( giSensorFD,  OV5642_REG_AF_PARA0, &reg );
	tmp = reg;
	ret += tmp;
	
	return ret;
}


// face detection process thread
static CAM_Int32s FaceDetectionThread( OV5642State *pState )
{
	CAM_Int32s ret = 0;
	CAM_Int32s i   = 0;

	for ( ; ; )
	{
		ret = IPP_EventWait( pState->stFaceDetectThread.hEventWakeup, INFINITE_WAIT, NULL );
		ASSERT( ret == 0 );

		if ( pState->stFaceDetectThread.bExitThread )
		{
			ret = IPP_EventSet( pState->stFaceDetectThread.hEventExitAck );
			ASSERT( ret == 0 );
			break;
		}
		
		ret = TrackInFrame( pState->pFaceDetectInputBuf, OV5642_FACEDETECTION_INBUFWIDTH, OV5642_FACEDETECTION_INBUFHEIGHT );
		ASSERT( ret == StatusSuccess || ret == StatusNoFace );

		pState->stFaceROI.iROICnt = 0;
		if ( ret == StatusSuccess )
		{
			pState->stFaceROI.iROICnt = iFaceNum;
			for ( i = 0; i < pState->stFaceROI.iROICnt; i++ )
			{
				pState->stFaceROI.stWeiRect[i].iWeight        = 1;
				pState->stFaceROI.stWeiRect[i].stRect.iLeft   = iFaceArea[i].iTl.iX * pState->stV4L2.stConfig.iWidth / OV5642_FACEDETECTION_INBUFWIDTH;
				pState->stFaceROI.stWeiRect[i].stRect.iTop    = iFaceArea[i].iTl.iY * pState->stV4L2.stConfig.iHeight / OV5642_FACEDETECTION_INBUFHEIGHT;
				pState->stFaceROI.stWeiRect[i].stRect.iWidth  = (iFaceArea[i].iBr.iX - iFaceArea[i].iTl.iX + 1) * pState->stV4L2.stConfig.iWidth / OV5642_FACEDETECTION_INBUFWIDTH;
				pState->stFaceROI.stWeiRect[i].stRect.iHeight = (iFaceArea[i].iBr.iY - iFaceArea[i].iTl.iY + 1) * pState->stV4L2.stConfig.iHeight / OV5642_FACEDETECTION_INBUFHEIGHT;
			}
			CELOG( "face number: %d\n", pState->stFaceROI.iROICnt );
			
			// send update face event
			if ( pState->stV4L2.fnEventHandler != NULL )
			{
				pState->stV4L2.fnEventHandler( CAM_EVENT_FACE_UPDATE, &(pState->stFaceROI), pState->stV4L2.pUserData );
			}

			// TODO: trigger face-region AF/exposure metering here

		}
	}

	IPP_ThreadExit( (void*)(0) );
	return 0;
}

static CAM_Error _ov5642_update_ref_roi( OV5642State *pState )
{
#if ( PLATFORM_PROCESSOR_VALUE != PLATFORM_PROCESSOR_MG1 )
	CAM_Int16u       reg;
#else
	struct v4l2_crop stCrop;
	CAM_Int32s       ret = 0;
#endif
	CAM_Rect         stRefROI;
	CAM_Error        error = CAM_ERROR_NONE;

	_CHECK_BAD_POINTER( pState );

#if ( PLATFORM_PROCESSOR_VALUE != PLATFORM_PROCESSOR_MG1 )
	// width
	reg = 0;
	_get_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_DVPIN_WIDTH_HIGH, &reg );
	stRefROI.iWidth = reg;
	reg = 0;
	_get_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_DVPIN_WIDTH_LOW, &reg );
	stRefROI.iWidth = ( stRefROI.iWidth << 8 ) | reg;

	// height
	reg = 0;
	_get_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_DVPIN_HEIGHT_HIGH, &reg );
	stRefROI.iHeight = reg;
	reg = 0;
	_get_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_DVPIN_HEIGHT_LOW, &reg );
	stRefROI.iHeight = ( stRefROI.iHeight << 8 ) | reg;

	// left corner
	reg = 0;
	_get_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SENSORCROP_LEFT_HIGH, &reg );
	stRefROI.iLeft = reg;
	reg = 0;
	_get_sensor_reg( pState->stV4L2.iSensorFD,  OV5642_REG_SENSORCROP_LEFT_LOW, &reg );
	stRefROI.iLeft = ( stRefROI.iLeft << 8 ) | reg;

	// top corner
	reg = 0;
	_get_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SENSORCROP_TOP_HIGH, &reg );
	stRefROI.iTop = reg;
	reg = 0;
	_get_sensor_reg( pState->stV4L2.iSensorFD, OV5642_REG_SENSORCROP_TOP_LOW, &reg );
	stRefROI.iTop = ( stRefROI.iTop << 8 ) | reg;
#else
	stCrop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        
	ret = ioctl( pState->stV4L2.iSensorFD, VIDIOC_G_CROP, &stCrop );
	ASSERT( ret == 0 );
	
	stRefROI.iTop    = stCrop.c.top;
	stRefROI.iLeft   = stCrop.c.left;
	stRefROI.iHeight = stCrop.c.height;
	stRefROI.iWidth  = stCrop.c.width;
#endif

	pState->stRefROI.iWidth  = stRefROI.iWidth;
	pState->stRefROI.iHeight = stRefROI.iHeight;
	pState->stRefROI.iLeft   = stRefROI.iLeft;
	pState->stRefROI.iTop    = stRefROI.iTop;

	CELOG( "digital zoom reference ROI: left:%d, top:%d, width:%d, height:%d\n", pState->stRefROI.iLeft, pState->stRefROI.iTop,
	       pState->stRefROI.iWidth, pState->stRefROI.iHeight );

	return error;
}

