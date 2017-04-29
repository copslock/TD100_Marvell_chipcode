#ifndef ACM_DEVICE_H
#define ACM_DEVICE_H

#include "acm.h"
#include "acmtypes.h"

typedef enum
{
	ACM_PATH_IN = 0,
	ACM_PATH_OUT,

	/* Must be at the end */
	ACM_PATH_NOT_CONNECTED,
	ACM_NUM_OF_PATHS = ACM_PATH_NOT_CONNECTED,

	ACM_PATH_DIRECTION_ENUM_32_BIT                  = 0x7FFFFFFF //32bit enum compiling enforcement
} ACM_PathDirection;

typedef enum
{
	ACM_DEVICE_DISABLE = 0,
	ACM_DEVICE_ENABLE,

	ACM_DEVICE_ENABLE_DISABLE_ENUM_32_BIT           = 0x7FFFFFFF //32bit enum compiling enforcement
}ACM_DeviceEnableDisable;

typedef enum
{
	ACM_MUTE_IN_MSA = 0,
	ACM_MUTE_IN_COMPONENT,
	ACM_MUTE_IN_COMPONENT_AND_MSA,

	ACM_MUTE_MODE_ENUM_32_BIT                           = 0x7FFFFFFF //32bit enum compiling enforcement
}ACM_MuteMode;

typedef enum
{
	ACM_SPEAKER_PHONE = 0,
	ACM_HANDSET,
	ACM_HEADSET,
	ACM_BT_HEADSET,
	ACM_CAR_KIT,
	ACM_BT_SPEAKER_PHONE,
	ACM_BT_CAR_KIT,
	ACM_BT_HANDSET,

	ACM_APPLIANCE_ENUM_32_BIT                           = 0x7FFFFFFF //32bit enum compiling enforcement
} ACM_Appliance;


typedef struct
{
	ACM_AudioDevice device;
	ACM_AudioFormat format;
	ACM_Component component;
	unsigned long path;
	ACM_PathDirection pathDirection;                      //ACM_PATH_NOT_CONNECTED - for not using DSP digital volume
	//AP audio doesn't need below part
	//ACM_MuteMode muteMode;
	//ACM_Appliance appliance;
} ACM_DeviceHandle;

typedef struct
{
	ACM_DeviceHandle deviceHandle;
	ACM_AudioMute deviceMute;
	ACM_DeviceEnableDisable deviceEnableDisable;
	ACM_DigitalGain deviceDigitalGain;
	ACM_AnalogGain deviceAnalogGain;
	ACM_AudioVolume deviceVolume;
}ACM_DeviceHandleLocal;



#endif
