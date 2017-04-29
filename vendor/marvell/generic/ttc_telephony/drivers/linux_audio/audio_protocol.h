/*
 *
 *  Audio server for tavor
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AUDIO_PROTOCOL_H__
#define __AUDIO_PROTOCOL_H__

#include <stdint.h>

typedef struct {
	int client_type;
	int method_type;
} SOCKETDATA;

#define INVALIDSOCKETDATA { -1, -1 }

typedef struct {
	int client_type;
	int method_type;
} AUDIOIPCHEADER;

#define INVALIDIPCHEADER { -1, -1 }

// Different types of client plugin for the daemon
#define INVALID_CLIENT_TYPE     0xFFFFFFFF
#define HIFI_CLIENT             0x00000001
#define VOICE_CLIENT            0x00000002
#define PHONE_CLIENT            0x00000003
#define FMRADIO_CLINET          0x00000004
#define CALIBRATION_CLIENT      0x00000005

#define PLUGIN_CTL_WRITE  0x00000001
#define PLUGIN_CTL_READ   0x00000002
#define PLUGIN_PCM_WRITE  0x00000003
#define PLUGIN_PCM_READ   0x00000004
#define PLUGIN_CALIBRATE_MSA    0x00000005

// Different methods
#define INVALID_METHOD_ID     0xFFFFFFFF
#define METHOD_ENABLE 0x00000001
#define METHOD_DISABLE 0x00000002
#define METHOD_MUTE 0x00000003
#define METHOD_VOLUME_SET 0x00000004
#define PATH_ENABLE 0x00000005
#define PATH_DISABLE 0x00000006

#define INVALID_DEVICE     0xFFFFFFFF
#define EARPIECE_DEVICE 0x00000001
#define SPEAKER_DEVICE 0x00000002
#define BLUETOOTH_DEVICE 0x00000003
#define HEADSET_DEVICE 0x00000004
#define HEADPHONE_DEVICE 0x00000008
#define BT_NREC_DEVICE   0x00000009
#define FORCE_SPEAKER_DEVICE 0x0000000a

/*low 2 bytes are used for volume control, below are high 2 bytes. When using them, we will shift them to left 16 bits, like "(INPUT << 16)"*/
#define INPUT 0x00000001
#define OUTPUT 0x00000002
/*enable voice call playback & capture*/
#define PCMSTREAMPLAYBACK 0x00000004
#define PCMSTREAMCAPTURE 0x00000008
/*voice call playback & capture to near end/far end, not define means both end*/
#define NEAREND 0x00000010
#define FAREND 0x00000020
#define COMBWITHCALL 0x00000040
/*with EC path enabled*/
#define ECENABLE 0x00000080
/* with sample rate switch enabled */
#define SRSWITCH 0X00000200
/* with calibration state switch enabled */
#define CALSTATESWITCH 0X00000400

/*loopback test*/
#define LOOPBACKTEST 0x00000100
/*test mode, in this mode audio server will drop all the msg from Android, atcmdsrv will control the audio server*/
#define TESTMODE 0x00000800
/*FIXME, notice that in ctl_phone.c, *imax = INT_MAX;*/
/*It means the flags we can use is up to bit 31, the highest 1 bit cannot be used.*/
/*Since in ctl phone plug-in, the int is signed, so we cannot use bit 31 (highest bit) as the bit flag */

#define HIFI_PLUGIN_CTL_WRITE  0x00000001
#define HIFI_PLUGIN_CTL_READ   0x00000002
#define HIFI_PLUGIN_PCM_WRITE  0x00000003
#define HIFI_PLUGIN_PCM_READ   0x00000004

#define VOICE_PLUGIN_CTL_WRITE  0x00000005
#define VOICE_PLUGIN_CTL_READ   0x00000006
#define VOICE_PLUGIN_PCM_WRITE  0x00000007
#define VOICE_PLUGIN_PCM_READ   0x00000008

#define PHONE_PLUGIN_CTL_WRITE  0x00000009
#define PHONE_PLUGIN_CTL_READ   0x0000000a
#define PHONE_PLUGIN_PCM_WRITE  0x0000000b
#define PHONE_PLUGIN_PCM_READ   0x0000000c


typedef struct {
	int device_type;
	int method_id;
	int param;
} __attribute__ ((packed)) AUDIOIPCDATA;

#define INVALIDIPCDATA { -1, -1, -1 }

typedef struct {
	int volume;
	int status;
} __attribute__ ((packed)) AUDIOCONTROLINFOS;


#define INVALIDCONTROLINFOS   { 0, 0, 0 }


typedef struct {
	unsigned int format;
	unsigned short rate;
	unsigned char channels;
} __attribute__ ((packed)) AUDIOSTREAMINFOS;


#define INVALIDAUDIOSTREAMINFOS   { 0, 0, 0 }


// parameters used to describe device state
typedef struct {
	int volume_phone;
	int status_phone;
	int volume_voice;
	int status_voice;
} __attribute__ ((packed)) AUDIOMIXERDATA;

#define INVALIDAUDIOMIXERDATA   { -1, -1, -1, -1 }


/* Phone Status */
#define PHONE_STATUS_NOACTION                     0x00000000
#define PHONE_VOICECALL_DISABLE                   0x00000001
#define PHONE_VOICECALL_ENABLE                    0x00000002
#define PHONE_PCMSTREAM_PLAYBACK_DISABLE          0x00000003
#define PHONE_PCMSTREAM_PLAYBACK_ENABLE           0x00000004
#define PHONE_PCMSTREAM_CAPTURE_DISABLE           0x00000005
#define PHONE_PCMSTREAM_CAPTURE_ENABLE            0x00000006

/* Voice Status */
#define VOICE_STATUS_NOACTION     0x00000000
#define VOICE_PLAYBACK_DISABLE    0x00000001
#define VOICE_PLAYBACK_ENABLE     0x00000002
#define VOICE_CAPTURE_DISABLE     0x00000003
#define VOICE_CAPTURE_ENABLE      0x00000004

#endif

