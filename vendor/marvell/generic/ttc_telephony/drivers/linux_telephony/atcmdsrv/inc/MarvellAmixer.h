#ifndef __MARVELL_AMIXER_H__
#define __MARVELL_AMIXER_H__

#include <stdint.h>
#include "audio_protocol.h"

#define MIXER_OLD_GENERIC_NUM 2
#define MIXER_NEW_ANDROID_NUM 72
#define MIXER_UI_CALIBRATION_TEST 1

#define MIXER_NUM       (MIXER_OLD_GENERIC_NUM + MIXER_NEW_ANDROID_NUM + MIXER_UI_CALIBRATION_TEST)

#define MSA_DATA_LENGTH 128 //uinit: unsigned short

//--- List of Voice Command OpCodes:
#define     START_VOICE_PATH    0x0040
#define     END_VOICE_PATH      0x0041
#define     VOICE_HANDOVER      0x0042
#define     VOICE_RX_PACKET     0x0043
#define     VOICE_MODULE_DEBUG  0x0044
#define     SET_CODEC           0x0045
#define     VOICE_TEST_CONTROL  0x0046
#define     PCM_STREAM_CONTROL  0x0047
#define     SELF_INVOCATION     0x0049
#define     VOICE_DRIFT_CONTROL 0x004A
#define     AMR_NSYNC_UPDATE    0x004B
#define     CODEC_IF_CONTROL    0x004C

#define     VOCODER_CONTROL     0x0060
#define     VOICE_CONTROL       0x0061
#define     VOLUME_CONTROL      0x0062
#define     MUTE_CONTROL        0x0063
#define     NS_CONTROL          0x0064
#define     EC_CONTROL          0x0065
#define     EQ_CONTROL          0x0066
#define     AVC_CONTROL         0x0067
#define     DTMF_CONTROL        0x0068
#define     AUX_MODE_CONTROL    0x0069
#define     EC_TEST_CMD         0x006A
#define     NS_TEST_CMD         0x006B
#define     AVC_TEST_CMD        0x006C
#define     DITHER_GEN_CONTROL  0x006D
#define     CTM_CONTROL         0x006E

static const struct
{
	int client;
	int methodType;
	int methodId;
	int device;
} gItemCtlInfo[MIXER_NUM] =
{
	/*dummy*/
	{ 0,		  0,		       0,		  0			     },
	{ 0,		  0,		       0,		  0			     },

	/*speaker control*/
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_ENABLE,	  SPEAKER_DEVICE	     },
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_DISABLE,	  SPEAKER_DEVICE	     },
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_VOLUME_SET, SPEAKER_DEVICE	     },
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_MUTE,	  SPEAKER_DEVICE	     },
	{ PHONE_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_ENABLE,	  SPEAKER_DEVICE	     },
	{ PHONE_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_DISABLE,	  SPEAKER_DEVICE	     },
	{ PHONE_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_VOLUME_SET, SPEAKER_DEVICE	     },
	{ PHONE_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_MUTE,	  SPEAKER_DEVICE	     },
	{ FMRADIO_CLINET, PLUGIN_CTL_WRITE,    METHOD_ENABLE,	  SPEAKER_DEVICE	     },
	{ FMRADIO_CLINET, PLUGIN_CTL_WRITE,    METHOD_DISABLE,	  SPEAKER_DEVICE	     },
	{ FMRADIO_CLINET, PLUGIN_CTL_WRITE,    METHOD_VOLUME_SET, SPEAKER_DEVICE	     },
	{ FMRADIO_CLINET, PLUGIN_CTL_WRITE,    METHOD_MUTE,	  SPEAKER_DEVICE	     },

	/*earpiece control*/
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_ENABLE,	  EARPIECE_DEVICE	     },
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_DISABLE,	  EARPIECE_DEVICE	     },
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_VOLUME_SET, EARPIECE_DEVICE	     },
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_MUTE,	  EARPIECE_DEVICE	     },
	{ PHONE_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_ENABLE,	  EARPIECE_DEVICE	     },
	{ PHONE_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_DISABLE,	  EARPIECE_DEVICE	     },
	{ PHONE_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_VOLUME_SET, EARPIECE_DEVICE	     },
	{ PHONE_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_MUTE,	  EARPIECE_DEVICE	     },
	{ FMRADIO_CLINET, PLUGIN_CTL_WRITE,    METHOD_ENABLE,	  EARPIECE_DEVICE	     },
	{ FMRADIO_CLINET, PLUGIN_CTL_WRITE,    METHOD_DISABLE,	  EARPIECE_DEVICE	     },
	{ FMRADIO_CLINET, PLUGIN_CTL_WRITE,    METHOD_VOLUME_SET, EARPIECE_DEVICE	     },
	{ FMRADIO_CLINET, PLUGIN_CTL_WRITE,    METHOD_MUTE,	  EARPIECE_DEVICE	     },

	/*headset control*/
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_ENABLE,	  HEADSET_DEVICE	     },
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_DISABLE,	  HEADSET_DEVICE	     },
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_VOLUME_SET, HEADSET_DEVICE	     },
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_MUTE,	  HEADSET_DEVICE	     },
	{ PHONE_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_ENABLE,	  HEADSET_DEVICE	     },
	{ PHONE_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_DISABLE,	  HEADSET_DEVICE	     },
	{ PHONE_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_VOLUME_SET, HEADSET_DEVICE	     },
	{ PHONE_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_MUTE,	  HEADSET_DEVICE	     },
	{ FMRADIO_CLINET, PLUGIN_CTL_WRITE,    METHOD_ENABLE,	  HEADSET_DEVICE	     },
	{ FMRADIO_CLINET, PLUGIN_CTL_WRITE,    METHOD_DISABLE,	  HEADSET_DEVICE	     },
	{ FMRADIO_CLINET, PLUGIN_CTL_WRITE,    METHOD_VOLUME_SET, HEADSET_DEVICE	     },
	{ FMRADIO_CLINET, PLUGIN_CTL_WRITE,    METHOD_MUTE,	  HEADSET_DEVICE	     },

	/*bluetooth control*/
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_ENABLE,	  BLUETOOTH_DEVICE	     },
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_DISABLE,	  BLUETOOTH_DEVICE	     },
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_VOLUME_SET, BLUETOOTH_DEVICE	     },
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_MUTE,	  BLUETOOTH_DEVICE	     },
	{ PHONE_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_ENABLE,	  BLUETOOTH_DEVICE	     },
	{ PHONE_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_DISABLE,	  BLUETOOTH_DEVICE	     },
	{ PHONE_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_VOLUME_SET, BLUETOOTH_DEVICE	     },
	{ PHONE_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_MUTE,	  BLUETOOTH_DEVICE	     },
	{ FMRADIO_CLINET, PLUGIN_CTL_WRITE,    METHOD_ENABLE,	  BLUETOOTH_DEVICE	     },
	{ FMRADIO_CLINET, PLUGIN_CTL_WRITE,    METHOD_DISABLE,	  BLUETOOTH_DEVICE	     },
	{ FMRADIO_CLINET, PLUGIN_CTL_WRITE,    METHOD_VOLUME_SET, BLUETOOTH_DEVICE	     },
	{ FMRADIO_CLINET, PLUGIN_CTL_WRITE,    METHOD_MUTE,	  BLUETOOTH_DEVICE	     },

	/*headphone control*/
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_ENABLE,	  HEADPHONE_DEVICE	     },
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_DISABLE,	  HEADPHONE_DEVICE	     },
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_VOLUME_SET, HEADPHONE_DEVICE	     },
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_MUTE,	  HEADPHONE_DEVICE	     },
	{ PHONE_CLIENT,   PLUGIN_CTL_WRITE,    METHOD_ENABLE,	  HEADPHONE_DEVICE	     },
	{ PHONE_CLIENT,   PLUGIN_CTL_WRITE,    METHOD_DISABLE,	  HEADPHONE_DEVICE	     },
	{ PHONE_CLIENT,   PLUGIN_CTL_WRITE,    METHOD_VOLUME_SET, HEADPHONE_DEVICE	     },
	{ PHONE_CLIENT,   PLUGIN_CTL_WRITE,    METHOD_MUTE,	  HEADPHONE_DEVICE	     },
	{ FMRADIO_CLINET, PLUGIN_CTL_WRITE,    METHOD_ENABLE,	  HEADPHONE_DEVICE	     },
	{ FMRADIO_CLINET, PLUGIN_CTL_WRITE,    METHOD_DISABLE,	  HEADPHONE_DEVICE	     },
	{ FMRADIO_CLINET, PLUGIN_CTL_WRITE,    METHOD_VOLUME_SET, HEADPHONE_DEVICE	     },
	{ FMRADIO_CLINET, PLUGIN_CTL_WRITE,    METHOD_MUTE,	  BLUETOOTH_DEVICE	     },

	/* for switching sample rate, using 8k in VT case, using 44.1k in HIFI case*/
	{HIFI_CLIENT, PLUGIN_CTL_WRITE, 0, 0},

	//wrapper enable path as diag command for loopback
	{ PHONE_CLIENT,	PLUGIN_CTL_WRITE, 0, 0},

	//wrapper enable path as diag command for loopback
	{ PHONE_CLIENT,	  PLUGIN_CTL_WRITE,    PATH_ENABLE,	  0},

	//wrapper disable path as diag command for loopback
	{ PHONE_CLIENT,	  PLUGIN_CTL_WRITE,    PATH_DISABLE,	  0},

	//BT NREC
	{ PHONE_CLIENT, PLUGIN_CTL_WRITE,    METHOD_ENABLE,	  BT_NREC_DEVICE	     },
	{ PHONE_CLIENT, PLUGIN_CTL_WRITE,    METHOD_DISABLE,	  BT_NREC_DEVICE	     },
	{ PHONE_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_VOLUME_SET, BT_NREC_DEVICE	     },
	{ PHONE_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_MUTE,	  BT_NREC_DEVICE	     },

	//HIFI Force speaker
	{ HIFI_CLIENT, PLUGIN_CTL_WRITE,    METHOD_ENABLE,	  FORCE_SPEAKER_DEVICE	     },
	{ HIFI_CLIENT, PLUGIN_CTL_WRITE,    METHOD_DISABLE,	  FORCE_SPEAKER_DEVICE	     },
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_VOLUME_SET, FORCE_SPEAKER_DEVICE	     },
	{ HIFI_CLIENT,	  PLUGIN_CTL_WRITE,    METHOD_MUTE,	  FORCE_SPEAKER_DEVICE	     },

	/* dummy for UI calibration */
	{0, 0, 0, 0},
};


/*******************************************************************************
* Function: audio_route_control
*******************************************************************************
* Description: Audio route control
*              Through ALSA control interface
*              Write item to ALSA phone control plug-in
*              Ctl plug-in send IPCs to audio server
*
* Parameters: int client (HIFI_CLIENT  VOICE_CLIENT  PHONE_CLIENT  FMRADIO_CLINET)
*             int method_type (PLUGIN_CTL_WRITE  PLUGIN_CTL_READ  PLUGIN_PCM_WRITE  PLUGIN_PCM_READ)
*             int method_id (METHOD_ENABLE  METHOD_DISABLE  METHOD_MUTE  METHOD_VOLUME_SET)
*             int device (EARPIECE_DEVICE  SPEAKER_DEVICE  BLUETOOTH_DEVICE  HEADSET_DEVICE)
*             int value (INPUT | OUTPUT | Volume)
*
* Return value: int (0: Success, -1: Fail)
*
*******************************************************************************/
int audio_route_control(int client, int method_type, int method_id, int device, int value);

/*calibration APIs*/
int UICal_send_MSA_setting(unsigned short *pData);
int UICal_send_MSA_request(unsigned short *pData);
/*loopback test*/
void start_loopback();
void stop_loopback();
#endif
