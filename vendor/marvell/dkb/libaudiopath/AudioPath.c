#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <hardware_legacy/power.h>
#include <utils/Log.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "AudioProtocol.h"
#include "MarvellAmixer.h"

#include "AudioPath.h"
#include "UICal_APIs.h"

uint32_t side; //AP or CP
uint32_t current_side;
int mDevice;
int mStandby;
unsigned char mVolume;
static int _fmFreq = 0;
static unsigned char _fmShadowVolume = 0xFF;
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif
static int _fmMuted = FALSE;
int mRadioFd;
int mNrec_on = 0;

static const int control_info_to_numid(int client, int method_type, int method_id, int device)
{
	int i;
        for (i = 2; i < MIXER_NUM; i++)
                if (gItemCtlInfo[i].client == client && gItemCtlInfo[i].methodType == method_type && \
                        gItemCtlInfo[i].methodId == method_id && gItemCtlInfo[i].device == device)
                        return i;

        return -1;
}

struct audio_path_t {
    uint32_t device;
    const char *alsaDevName;
    void (*enable)(unsigned char left, unsigned char right);
    void (*disable)();
    void (*set_volume)(unsigned char left, unsigned char right);
#ifdef WITH_ECPATH
    int (*ecenable)(int volume);
    int (*ecdisable)(int volume);
#endif
};

static void enable_earpiece(unsigned char left, unsigned char right)
{
	unsigned char nv = (left+right) * 10 / 51;

	if( side == CP_SIDE ) {
		LOGI("**************************enable_phone earpiece: %d %d - %d\n", left, right, nv);

		audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
				EARPIECE_DEVICE,((OUTPUT)<<16) | nv);
		audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
				EARPIECE_DEVICE,((OUTPUT|INPUT)<<16) | nv);
		current_side = CP_SIDE;
	} else {
		LOGI("**************************enable_hifi earpiece: %d %d - %d\n", left, right, nv);
		audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
				EARPIECE_DEVICE,((OUTPUT)<<16) | nv);
		current_side = AP_SIDE; 
	}
}

static void disable_earpiece()
{
	if(current_side == CP_SIDE) {
		LOGI("**************************disable_phone earpiece\n");
		audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
				EARPIECE_DEVICE,(OUTPUT|INPUT)<<16);
		audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
				EARPIECE_DEVICE,(OUTPUT)<<16);
	} else {
		LOGI("**************************disable_hifi earpiece\n");
		audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
				EARPIECE_DEVICE,(OUTPUT)<<16);
	}
}

static void set_earpiece_volume(unsigned char left, unsigned char right)
{
	unsigned char nv = (left+right) * 10 / 51;
	if(side == CP_SIDE) {
		LOGI("**************************set_phone earpiece_volume=%d %d - %d",left,right,nv);
		audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_VOLUME_SET, \
				EARPIECE_DEVICE,(OUTPUT << 16)| nv);
	} else {
		LOGI("**************************set_hifi earpiece_volume=%d %d - %d",left,right,nv);
		audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_VOLUME_SET, \
				EARPIECE_DEVICE,(OUTPUT << 16)| nv);
	}
}

static void enable_speaker(unsigned char left, unsigned char right)
{
	unsigned char nv = (left+right) * 10 / 51;

	if( side == CP_SIDE ) {
		LOGI("**************************enable_phone_speaker: %d %d - %d\n", left, right, nv);

	       audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
			  	SPEAKER_DEVICE,(OUTPUT<<16) | nv);
           audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
			  	SPEAKER_DEVICE,((OUTPUT|INPUT)<<16) | nv);
		current_side = CP_SIDE;
	}
	else {
                LOGI("**************************enable_hifi_speaker: %d %d - %d\n", left, right, nv);

               audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
			   	SPEAKER_DEVICE,(OUTPUT<<16) | nv);
		 current_side = AP_SIDE;
	}

}

static void disable_speaker()
{
       if( current_side == CP_SIDE ) { 
                LOGI("**************************disable_phone_speaker\n");

                audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
					SPEAKER_DEVICE,(OUTPUT|INPUT)<<16);
                audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
					SPEAKER_DEVICE,OUTPUT<<16);
       }
       else {
                LOGI("**************************disable_hifi_speaker\n");

                audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
					SPEAKER_DEVICE,OUTPUT<<16);

       }


}

static void set_speaker_volume(unsigned char left, unsigned char right)
{
	unsigned char nv = (left+right) * 10 / 51;
	if( side == CP_SIDE ) {
                LOGI("**************************set_phone_speaker_volume = %d %d %d",left,right,nv);

          audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_VOLUME_SET, \
					SPEAKER_DEVICE,((OUTPUT|INPUT)<<16)|nv);
        }
        else {
                LOGI("**************************set_hifi_speaker_volume=%d %d %d",left,right,nv);

                audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_VOLUME_SET, \
					SPEAKER_DEVICE,(OUTPUT<<16)|nv);

        }

}

void enable_fm_speaker(unsigned char left, unsigned char right)
{
	unsigned char nv = (left+right) * 10 / 51;
    LOGI("**************************enable_fm_speaker: %d %d - %d \n", left, right, nv);

    audio_route_control(FMRADIO_CLINET, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
        SPEAKER_DEVICE, (OUTPUT<<16)|nv);

    if (nv == 0) {
        audio_route_control(FMRADIO_CLINET, PLUGIN_CTL_WRITE, METHOD_MUTE, \
            SPEAKER_DEVICE, (OUTPUT<<16)|1);
        _fmMuted = TRUE;
    } else {
        _fmMuted = FALSE;
    }

    _fmShadowVolume = nv;
}

void disable_fm_speaker()
{
    LOGI("**************************disable_fm_speaker");

    audio_route_control(FMRADIO_CLINET, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
        SPEAKER_DEVICE,(OUTPUT<<16));

}

static void set_fm_speaker_volume(unsigned char left, unsigned char right)
{
	unsigned char nv = (left+right) * 10 / 51;
    LOGI("**************************set_fm_speaker_volume=%d %d - %d",left,right,nv);

    if (nv == 0 && !_fmMuted){
        audio_route_control(FMRADIO_CLINET, PLUGIN_CTL_WRITE, METHOD_MUTE, \
            SPEAKER_DEVICE, (OUTPUT<<16)|1);
        _fmMuted = TRUE;
    } else if (nv != 0 && _fmMuted){
        audio_route_control(FMRADIO_CLINET, PLUGIN_CTL_WRITE, METHOD_MUTE, \
            SPEAKER_DEVICE, (OUTPUT<<16)|0);
        audio_route_control(FMRADIO_CLINET, PLUGIN_CTL_WRITE, METHOD_VOLUME_SET, \
            SPEAKER_DEVICE,(OUTPUT<<16)|nv);
        _fmMuted = FALSE;
    } else if (nv != 0 && !_fmMuted){
        audio_route_control(FMRADIO_CLINET, PLUGIN_CTL_WRITE, METHOD_VOLUME_SET, \
            SPEAKER_DEVICE,(OUTPUT<<16)|nv);
    }
    _fmShadowVolume = nv;
}

/*
** Limitation: FM Recording volume would be same as FM playback volume.
** i.e, changing FM playback volume would change FM Recording volume, too.
** vice verse.
** Reason: FM recording and FM playback share same PGA Sanremo register.
*/
static void enable_fm_record(unsigned char left, unsigned char right)
{
    LOGI("**************************enable_fm_record, nv = %d\n", _fmShadowVolume);

    audio_route_control(FMRADIO_CLINET, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
                           HEADSET_DEVICE, (INPUT<<16) | _fmShadowVolume);

    if (_fmShadowVolume == 0) {
        audio_route_control(FMRADIO_CLINET, PLUGIN_CTL_WRITE, METHOD_MUTE, \
            HEADSET_DEVICE, (INPUT<<16)|1);
    }
}

static void disable_fm_record()
{
    LOGI("**************************disable_fm_record\n");
    audio_route_control(FMRADIO_CLINET, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
                           HEADSET_DEVICE,INPUT<<16);
}

static void set_fm_record_volume(unsigned char left, unsigned char right)
{
    unsigned char nv = (left+right) * 10 / 51;
    LOGI("**************************set_fm_speaker_volume=%d %d - %d",left,right,nv);

    if (nv == 0 && !_fmMuted){
        audio_route_control(FMRADIO_CLINET, PLUGIN_CTL_WRITE, METHOD_MUTE, \
            HEADSET_DEVICE, (INPUT<<16)|1);
        _fmMuted = TRUE;
    } else if (nv != 0 && _fmMuted){
        audio_route_control(FMRADIO_CLINET, PLUGIN_CTL_WRITE, METHOD_MUTE, \
            HEADSET_DEVICE, (INPUT<<16)|0);
        audio_route_control(FMRADIO_CLINET, PLUGIN_CTL_WRITE, METHOD_VOLUME_SET, \
            HEADSET_DEVICE,(INPUT<<16)|nv);
        _fmMuted = FALSE;
    } else if (nv != 0 && !_fmMuted){
        audio_route_control(FMRADIO_CLINET, PLUGIN_CTL_WRITE, METHOD_VOLUME_SET, \
            HEADSET_DEVICE,(INPUT<<16)|nv);
    }
    _fmShadowVolume = nv;
}

static void enable_headset(unsigned char left, unsigned char right)
{
	unsigned char nv = (left+right) * 10 / 51;

        if( side == CP_SIDE ) {
                LOGI("**************************enable_phone_headset: %d %d - %d\n", left, right, nv);

		  audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
					HEADSET_DEVICE,((OUTPUT)<<16) | nv);
          audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
					HEADSET_DEVICE,((OUTPUT|INPUT)<<16) | nv);
		  current_side = CP_SIDE;
        }
        else {
                LOGI("**************************enable_hifi_headset: %d %d - %d \n", left, right, nv);

                audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
					HEADSET_DEVICE,(OUTPUT<<16) | nv);
		  current_side = AP_SIDE;
        }

}

static void disable_headset()
{
        if( current_side == CP_SIDE ) {
                LOGI("**************************disable_phone_headset\n");

                audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
					HEADSET_DEVICE,(OUTPUT|INPUT)<<16);
		audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
					HEADSET_DEVICE,(OUTPUT)<<16);
        }
        else {
                LOGI("**************************disable_hifi_headset\n");

                audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
					HEADSET_DEVICE,OUTPUT<<16);
        }

}

static void set_headset_volume(unsigned char left, unsigned char right)
{
	unsigned char nv = (left+right) * 10/51;

	if( side == CP_SIDE ) {
                LOGI("**************************set_phone_headset_volume=%d %d %d",left,right,nv);

                audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_VOLUME_SET, \
					HEADSET_DEVICE, (OUTPUT<<16)|nv);
        }
        else {
                LOGI("**************************set_hifi_headset_volume=%d %d %d",left,right,nv);

                audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_VOLUME_SET, \
					HEADSET_DEVICE, (OUTPUT<<16)|nv);

        }
				
}

static void enable_headphone(unsigned char left, unsigned char right)
{
	unsigned char nv = (left+right) * 10 / 51;

        if( side == CP_SIDE ) {
                LOGI("**************************enable_phone_headphone: %d %d - %d\n", left, right, nv);

		  audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
					HEADPHONE_DEVICE,((OUTPUT)<<16) | nv);
          audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
					HEADPHONE_DEVICE,((OUTPUT|INPUT)<<16) | nv);
		  current_side = CP_SIDE;
        }
        else {
                LOGI("**************************enable_hifi_headphone: %d %d - %d \n", left, right, nv);

                audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
					HEADPHONE_DEVICE,(OUTPUT<<16) | nv);
		  current_side = AP_SIDE;
        }

}

static void disable_headphone()
{
        if( current_side == CP_SIDE ) {
                LOGI("**************************disable_phone_headphone\n");

		audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
					HEADPHONE_DEVICE,(OUTPUT|INPUT)<<16);
		audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
					HEADPHONE_DEVICE,(OUTPUT)<<16);
        }
        else {
                LOGI("**************************disable_hifi_headphone\n");

                audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
					HEADPHONE_DEVICE,OUTPUT<<16);
        }

}

static void set_headphone_volume(unsigned char left, unsigned char right)
{
	unsigned char nv = (left+right) * 10/51;

	if( side == CP_SIDE ) {
                LOGI("**************************set_phone_headphone_volume=%d %d %d",left,right,nv);

                audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_VOLUME_SET, \
					HEADPHONE_DEVICE, (OUTPUT<<16)|nv);
        }
        else {
                LOGI("**************************set_hifi_headphone_volume=%d %d %d",left,right,nv);

                audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_VOLUME_SET, \
					HEADPHONE_DEVICE, (OUTPUT<<16)|nv);

        }

}

void enable_fm_headset(unsigned char volume /* % */)
{
    LOGI("**************************enable_fm_headset volume = 0x%x, _fmShadowVolume = 0x%x", \
        volume, _fmShadowVolume);

    if( (volume != 0xFF) && (_fmShadowVolume != volume) )
    {  /* update _fmShadowVolume received from enable_fm() */
        _fmShadowVolume = volume;
    }

    if( /*(_fmFreq != 0) &&*/ (_fmShadowVolume != 0xFF) )
    {
        /*
        ** Frequency indication has already updated (TEMPORARY: allow with no valid frequency);
        ** also, valid volume already received, meaning "enable" request already received
        ** [15:0]   Volume
        ** [19:16]  OUTPUT/INPUT device
        ** [30:20]  Frequency (in 100 kHz units)
        ** [31] Reserved by AudioServer
        */
        if (_fmFreq >> 11){
            LOGE("enable_fm_headset: the fm_frequency 0x%x is invalid", _fmFreq);
            return;
        }
        audio_route_control(FMRADIO_CLINET, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
            HEADSET_DEVICE, (_fmFreq << 20) | _fmShadowVolume | (OUTPUT << 16));
        if (_fmShadowVolume == 0) {
            audio_route_control(FMRADIO_CLINET, PLUGIN_CTL_WRITE, METHOD_MUTE, \
                HEADSET_DEVICE, (OUTPUT<<16)|1);
            _fmMuted = TRUE;
        } else {
            _fmMuted = FALSE;
        }
    }
}

static void enable_fm(unsigned char left, unsigned char right)
{
    unsigned char volume = (left+right) * 10 / 51;
    LOGI("**************************enable_fm=%d %d - %d",left,right,volume);

    enable_fm_headset(volume);
}

void disable_fm()
{
    LOGI("**************************disable_fm\n");
    _fmFreq = 0;

    audio_route_control(FMRADIO_CLINET, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
        HEADSET_DEVICE,(OUTPUT<<16));

}

static void set_fm_volume(unsigned char left, unsigned char right)
{
	unsigned char nv = (left+right) * 10 / 51;
    LOGI("**************************set_fm_headset_volume=%d %d - %d",left,right,nv);

    if (nv == 0 && !_fmMuted){
        audio_route_control(FMRADIO_CLINET, PLUGIN_CTL_WRITE, METHOD_MUTE, \
            HEADSET_DEVICE, (OUTPUT<<16)|1);
        _fmMuted = TRUE;
    } else if (nv != 0 && _fmMuted){
        audio_route_control(FMRADIO_CLINET, PLUGIN_CTL_WRITE, METHOD_MUTE, \
            HEADSET_DEVICE, (OUTPUT<<16)|0);
        audio_route_control(FMRADIO_CLINET, PLUGIN_CTL_WRITE, METHOD_VOLUME_SET, \
            HEADSET_DEVICE,(OUTPUT<<16)|nv);
        _fmMuted = FALSE;
    } else if (nv != 0 && !_fmMuted){
        audio_route_control(FMRADIO_CLINET, PLUGIN_CTL_WRITE, METHOD_VOLUME_SET, \
            HEADSET_DEVICE,(OUTPUT<<16)|nv);
    }
    _fmShadowVolume = nv;
}
static void enable_bluetooth_sco_headset(unsigned char left, unsigned char right)
{
	unsigned char nv = (left+right) * 10/51;
       int device = 0;
       device = (mNrec_on == 1)? BT_NREC_DEVICE:BLUETOOTH_DEVICE;
	if( side == CP_SIDE ) {
		LOGI("**************************enable_phone_bluetooth sco headset: %d %d - %d\n", left, right, nv);
		audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
				EARPIECE_DEVICE,((OUTPUT)<<16) | nv);
		audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
				device,((OUTPUT)<<16) | nv);
		audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
				device,((OUTPUT|INPUT)<<16) | nv);
		current_side = CP_SIDE;
	}
	else {
		LOGI("**************************enable_hifi bluetooth sco_headset: %d %d - %d\n", left, right, nv);

		audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
				device,(OUTPUT<<16) | nv);
		current_side = AP_SIDE;
	}
}

static void disable_bluetooth_sco_headset()
{
        int device = 0;
	 device = (mNrec_on == 1)? BT_NREC_DEVICE:BLUETOOTH_DEVICE;
        if( current_side == CP_SIDE ) {
                LOGI("**************************disable_phone_bluetooth sco headset\n");

		 audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
				        EARPIECE_DEVICE,(OUTPUT)<<16);
		  audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
					device,(OUTPUT)<<16);
                audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
					device,(OUTPUT|INPUT)<<16);
        }
        else {
                LOGI("**************************disable_hifi_bluetooth sco headset\n");

                audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
					device,OUTPUT<<16);
        }

}

static void set_bluetooth_sco_headset_volume(unsigned char left, unsigned char right)
{
	unsigned char nv = (left+right) * 10/51;
	int device = 0;
       device = (mNrec_on == 1)? BT_NREC_DEVICE:BLUETOOTH_DEVICE;
	if( side == CP_SIDE ) {
                LOGI("**************************set_phone_bluetooth sco headset_volume=%d %d %d",left,right,nv);

                audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_VOLUME_SET, \
					device, (OUTPUT<<16)|nv);
        }
        else {
                LOGI("**************************set_hifi_bluetooth sco headset_volume=%d %d %d",left,right,nv);

                audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_VOLUME_SET, \
					device, (OUTPUT<<16)|nv);

        }
				
}

static void enable_mic(unsigned char left, unsigned char right)
{
	LOGI("**************************enable_hifi_mic\n");

	audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
			EARPIECE_DEVICE,(INPUT<<16) | 90);
}

static void disable_mic()
{
	LOGI("**************************disable_hifi_mic\n");

	audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
			EARPIECE_DEVICE,INPUT<<16);
}

static void set_mic_volume(unsigned char left, unsigned char right)
{
	unsigned char nv = (left+right) * 10 / 51;

	LOGI("**************************set_hifi_mic_volume\n");

	audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_VOLUME_SET, \
			EARPIECE_DEVICE,(INPUT<<16)|nv);
}

static void enable_mic_headset(unsigned char left, unsigned char right)
{
	LOGI("**************************enable_hifi_mic_headset\n");

	audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
			HEADSET_DEVICE,(INPUT<<16) | 90);
}

static void disable_mic_headset()
{
	LOGI("**************************disable_hifi_mic_headset\n");

	audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
			HEADSET_DEVICE,INPUT<<16);
}

static void set_mic_headset_volume(unsigned char left, unsigned char right)
{
	unsigned char nv = (left+right) * 10 / 51;
	LOGI("**************************set_hifi_mic_headset_volume\n");

	audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_VOLUME_SET, \
			HEADSET_DEVICE,(INPUT<<16)|nv );
}

static void enable_mic_bluetooth_sco_headset(unsigned char left, unsigned char right)
{
	LOGI("**************************enable_hifi_mic bluetooth sco_headset\n");
       int device = 0;
       device = (mNrec_on == 1)? BT_NREC_DEVICE:BLUETOOTH_DEVICE;
	audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
			device,(INPUT<<16) | 90);
}

static void disable_mic_bluetooth_sco_headset()
{
	LOGI("**************************disable_hifi_mic_bluetooth sco headset\n");
       int device = 0;
       device = (mNrec_on == 1)? BT_NREC_DEVICE:BLUETOOTH_DEVICE;
	audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
			device,INPUT<<16);
}

static void set_mic_bluetooth_sco_headset_volume(unsigned char left, unsigned char right)
{
	unsigned char nv = (left+right) * 10 / 51;
       int device = 0;
       device = (mNrec_on == 1)? BT_NREC_DEVICE:BLUETOOTH_DEVICE;
	LOGI("**************************set_hifi_mic_bluetooth sco headset_volume\n");

	audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_VOLUME_SET, \
			device,(INPUT<<16)|nv );
}

static void enable_fm_transmitter(unsigned char left, unsigned char right)
{
    LOGI("**************************enable_fm_transmitter\n");

    mRadioFd = open("/dev/radio0", O_RDWR);
    if (mRadioFd < 0) {
        LOGE("**************************failed to enable fm transmitter!\n");
    }
}

static void disable_fm_transmitter()
{
    LOGI("**************************disable_fm_transmitter\n");

    if (mRadioFd > 0) {
        close(mRadioFd);
        mRadioFd = -1;
    }
}

static void set_fm_transmitter_volume(unsigned char left, unsigned char right)
{
    LOGI("**************************set_fm_transmitter_volume\n");
}

static void enable_force_speaker(unsigned char left, unsigned char right)
{
	unsigned char nv = (left+right) * 10 / 51;

    LOGI("**************************enable_force_speaker: %d %d - %d\n", left, right, nv);
    audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
		FORCE_SPEAKER_DEVICE,(OUTPUT<<16) | nv);
}

static void disable_force_speaker()
{
    LOGI("**************************disable_force_speaker\n");

    audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
		FORCE_SPEAKER_DEVICE,OUTPUT<<16);
}

static void set_force_speaker_volume(unsigned char left, unsigned char right)
{
	unsigned short nv = ((left << 8) | right);

    if (left > 0x3f || right > 0x3f) {
        LOGE("**************************set_force_speaker_volume failed: volume(%x/%x) out of range!", left, right);
        return;
    }

    LOGI("**************************set_force_speaker_volume=%d %d %x",left,right,nv);
    audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_VOLUME_SET, \
	    FORCE_SPEAKER_DEVICE, (OUTPUT<<16)|nv);
}

/*
 * the following is the list of all supported audio path
 */
static struct audio_path_t audio_path_list[] = {
// composite devices must be put in the top!!!
{
.device = DEVICE_OUT_SPEAKER | DEVICE_OUT_WIRED_HEADSET,
.alsaDevName = "default",
.enable = enable_force_speaker,
.disable = disable_force_speaker,
.set_volume = set_force_speaker_volume,
#ifdef WITH_ECPATH
.ecenable = NULL,
.ecdisable= NULL,
#endif
},

{
.device = DEVICE_OUT_SPEAKER | DEVICE_OUT_WIRED_HEADPHONE,
.alsaDevName = "default",
.enable = enable_force_speaker,
.disable = disable_force_speaker,
.set_volume = set_force_speaker_volume,
#ifdef WITH_ECPATH
.ecenable = NULL,
.ecdisable= NULL,
#endif
},

{
.device = DEVICE_OUT_EARPIECE,
.alsaDevName = "default",
.enable = enable_earpiece,
.disable = disable_earpiece,
.set_volume = set_earpiece_volume,
#ifdef WITH_ECPATH
.ecenable = NULL,
.ecdisable= NULL,
#endif
},

{
.device = DEVICE_OUT_SPEAKER,
.alsaDevName = "default",
.enable = enable_speaker,
.disable = disable_speaker,
.set_volume = set_speaker_volume,
#ifdef WITH_ECPATH
.ecenable = NULL,
.ecdisable= NULL,
#endif
},

{
.device = DEVICE_OUT_WIRED_HEADSET,
.alsaDevName = "default",
.enable = enable_headset,
.disable = disable_headset,
.set_volume = set_headset_volume,
#ifdef WITH_ECPATH
.ecenable = NULL,
.ecdisable= NULL,
#endif
},

{
.device = DEVICE_OUT_WIRED_HEADPHONE,
.alsaDevName = "default",
.enable = enable_headphone,
.disable = disable_headphone,
.set_volume = set_headphone_volume,
#ifdef WITH_ECPATH
.ecenable = NULL,
.ecdisable= NULL,
#endif
},

{
.device = DEVICE_OUT_BLUETOOTH_SCO,
.alsaDevName = "default",
.enable = enable_bluetooth_sco_headset,
.disable = disable_bluetooth_sco_headset,
.set_volume = set_bluetooth_sco_headset_volume,
#ifdef WITH_ECPATH
.ecenable = NULL,
.ecdisable= NULL,
#endif
},

{
.device = DEVICE_OUT_BLUETOOTH_SCO_HEADSET,
.alsaDevName = "default",
.enable = enable_bluetooth_sco_headset,
.disable = disable_bluetooth_sco_headset,
.set_volume = set_bluetooth_sco_headset_volume,
#ifdef WITH_ECPATH
.ecenable = NULL,
.ecdisable= NULL,
#endif
},
{
.device = DEVICE_OUT_FM_SPEAKER,
.alsaDevName = "default",
.enable = enable_fm_speaker,
.disable = disable_fm_speaker,
.set_volume = set_fm_speaker_volume,
#ifdef WITH_ECPATH
.ecenable = NULL,
.ecdisable= NULL,
#endif
},
{
.device = DEVICE_OUT_FM_HEADPHONE,
.alsaDevName = "default",
.enable = enable_fm,
.disable = disable_fm,
.set_volume = set_fm_volume,
#ifdef WITH_ECPATH
.ecenable = NULL,
.ecdisable= NULL,
#endif
},
{
.device = DEVICE_IN_BUILTIN_MIC,
.alsaDevName = "plughw:0,1",
.enable = enable_mic,
.disable = disable_mic,
.set_volume = set_mic_volume,
#ifdef WITH_ECPATH
.ecenable = mic_ec_path_enable,
.ecdisable= mic_ec_path_disable,
#endif
},

{
.device = DEVICE_IN_WIRED_HEADSET,
.alsaDevName = "plughw:0,1",
.enable = enable_mic_headset,
.disable = disable_mic_headset,
.set_volume = set_mic_headset_volume,
#ifdef WITH_ECPATH
.ecenable = headset_mic_ec_path_enable,
.ecdisable= headset_mic_ec_path_disable,
#endif
},

{
.device = DEVICE_IN_BLUETOOTH_SCO_HEADSET,
.alsaDevName = "default",
.enable = enable_mic_bluetooth_sco_headset,
.disable = disable_mic_bluetooth_sco_headset,
.set_volume = set_mic_bluetooth_sco_headset_volume,
#ifdef WITH_ECPATH
.ecenable = NULL,
.ecdisable= NULL,
#endif
},

{
.device = DEVICE_IN_FMRADIO,
.alsaDevName = "plughw:0,1",
.enable = enable_fm_record,
.disable = disable_fm_record,
.set_volume = set_fm_record_volume,
#ifdef WITH_ECPATH
.ecenable = NULL,
.ecdisable= NULL,
#endif
},

{
.device = DEVICE_OUT_FM_TRANSMITTER,
.alsaDevName = "default",
.enable = enable_fm_transmitter,
.disable = disable_fm_transmitter,
.set_volume = set_fm_transmitter_volume,
#ifdef WITH_ECPATH
.ecenable = NULL,
.ecdisable= NULL,
#endif
},

};



// ----------Abstract layer for controlling different Audio path-------------
const char * get_alsa_dev_name_for_device(uint32_t devices)
{
    int supported = sizeof(audio_path_list)/sizeof(audio_path_list[0]);
    int i;

//    LOGI("**********************************get_alsa_dev_name_for_device****************************\n");
    for(i=0;i<supported;i++) {
        if(devices & audio_path_list[i].device) {
            return audio_path_list[i].alsaDevName;
        }
    }
    return NULL;
}

void enable_audio_path_for_device(uint32_t devices, unsigned char left, unsigned char right)
{
    //	LOGI("**********************************enable_audio_path_for_device****************************\n");
    int supported = sizeof(audio_path_list)/sizeof(audio_path_list[0]);
    int i;
    for(i=0;i<supported;i++) {
        if((devices & audio_path_list[i].device) == audio_path_list[i].device) {
            if(audio_path_list[i].enable)
                audio_path_list[i].enable(left,right);
            else
                LOGI("Enable Device is not supported now - 0x%x \n", audio_path_list[i].device);
            devices &= (~audio_path_list[i].device);
        }
    }
}

void disable_audio_path_for_device(uint32_t devices)
{
    int supported = sizeof(audio_path_list)/sizeof(audio_path_list[0]);
    int i;

    //	LOGI("**********************************disable_audio_path_for_device****************************\n");
    for(i=0;i<supported;i++) {
        if((devices & audio_path_list[i].device) == audio_path_list[i].device) {
            if(audio_path_list[i].disable)
                audio_path_list[i].disable();
            else
                LOGI("Disable Device is not supported now - 0x%x \n", audio_path_list[i].device);
            devices &= (~audio_path_list[i].device);
        }
    }
}

#ifdef WITH_ECPATH
void enable_audio_ecpath_for_device(uint32_t devices, unsigned char volume)
{
    //	LOGI("**********************************enable_audio_path_for_device****************************\n");
    int supported = sizeof(audio_path_list)/sizeof(audio_path_list[0]);
	int nv = volume * 20 / 51;
    int i;
    for(i=0;i<supported;i++) {
        if(devices & audio_path_list[i].device) {
            audio_path_list[i].ecenable(nv);
            break;
        }
    }
}

void disable_audio_ecpath_for_device(uint32_t devices, unsigned char volume)
{
    //	LOGI("**********************************disable_audio_path_for_device****************************\n");
    int supported = sizeof(audio_path_list)/sizeof(audio_path_list[0]);
	int nv = volume * 20 / 51;
    int i;
    for(i=0;i<supported;i++) {
        if(devices & audio_path_list[i].device) {
            audio_path_list[i].ecdisable(nv);
            break;
        }
    }
}
#endif

void set_volume_for_device(uint32_t devices, unsigned char left, unsigned char right)
{

    int supported = sizeof(audio_path_list)/sizeof(audio_path_list[0]);
    int i;
    LOGI("**********************************set_volume_for_device****************************\n");

    for(i=0;i<supported;i++) {
        if((devices & audio_path_list[i].device) == audio_path_list[i].device) {
            audio_path_list[i].set_volume(left,right);
            devices &= (~audio_path_list[i].device);
        }
    }
}

uint32_t get_all_output_device()
{
	side = AP_SIDE;
	current_side = AP_SIDE;
	mDevice = 0;
	mStandby = 0;
	mVolume = 0;

	return DEVICE_OUT_EARPIECE | DEVICE_OUT_SPEAKER ;
}

uint32_t get_all_input_device()
{
    return DEVICE_IN_BUILTIN_MIC;
}

char *id="AudioHolder";
int set_side( uint32_t mState , int device, unsigned char volume, int nrec_on)
{
	if(device == DEVICE_OUT_BLUETOOTH_SCO_HEADSET)
	{
		mNrec_on = nrec_on;
	}
#ifdef WITH_VTOVERBT
	if ((mState == MODE_IN_CALL) || (mState == MODE_IN_VT_CALL))
#else
	if (mState == MODE_IN_CALL)
#endif
	{
		side = CP_SIDE; //CP
		if (acquire_wake_lock(PARTIAL_WAKE_LOCK, id) > 0 )
			LOGI("*********************************acquire_wake_lock ok %s",id);
		else
			LOGI("*********************************acquire_wake_lock failed %s",id);
	}
	else
	{
		side = AP_SIDE;
		release_wake_lock(id);
	}

	mDevice = setRouting(device , mDevice, volume);

	return mDevice;
}

void set_Standby()
{
	/*
		If mode is phone call, the audio path will be closed when the
		mode is changed to normal.
	*/
	if(side == AP_SIDE)
	{
		disable_audio_path_for_device(mDevice);
		mStandby = 1;
	}
}

void check_Standby()
{
	if(mStandby)
	{
		enable_audio_path_for_device(mDevice, mVolume, mVolume);
		mStandby = 0;
	}
}

int isStandby()
{
    return mStandby;
}

int is_side_changed()
{
	if (side!=current_side )
		return 1;
	else
		return 0;
}

int audio_init_check()
{
	int i;
	for(i = 0; i < 200; i++)
	{
		FILE *fd = fopen("/tmp/audioserver_ok", "r");
		if (fd)
		{
			fclose(fd);
			return 0;
		}
		usleep(100000);
	}

	LOGI("\n\n+++++++++ AudioHardwareMarvell::initCheck wait audio server failed ++++++\n\n");

	return -1;
}

int setRouting( int device , int oDevice, unsigned char volume)
{
	LOGI("routing: old: %d, new: %d, volume: %d \n", oDevice, device, volume);
	mVolume=volume;

	if (( device != oDevice ) || (is_side_changed()))
	{
		mDevice = device;

		/*
			If in Phone mode, the new device should be switched as soon as
			possible.
		*/
		if((! mStandby) || (side == CP_SIDE))
		{
			if ( oDevice != 0)
			{
				disable_audio_path_for_device( oDevice);
			}

			enable_audio_path_for_device( mDevice , volume, volume);

			if(mStandby) mStandby = 0;
		}
	}

	return mDevice;
}

/* sample rate switch */
int enable_8k_sample()
{
	int ret = 0;

	LOGE("enable 8K sample rate \n");

	if(mDevice != 0)
		disable_audio_path_for_device(mDevice);

	ret = switch_sample_rate(0, 8000, 0);

	if(mDevice != 0)
		enable_audio_path_for_device(mDevice,
				mVolume, mVolume);

	return ret;
}

int enable_44k1_sample()
{
	int ret = 0;

	LOGE("enable 44.1K sample rate \n");

	if(mDevice != 0)
		disable_audio_path_for_device(mDevice);

	ret = switch_sample_rate(0, 44100, 0);

	if(mDevice != 0)
		enable_audio_path_for_device(mDevice,
				mVolume, mVolume);

	return ret;
}

int enable_48k_sample()
{
	int ret = 0;

	LOGE("enable 48K sample rate \n");

	if(mDevice != 0)
		disable_audio_path_for_device(mDevice);

	ret = switch_sample_rate(0, 48000, 0);

	if(mDevice != 0)
		enable_audio_path_for_device(mDevice,
				mVolume, mVolume);

	return ret;
}

/*      LOOPBACK      */
int enable_loopback(int device)
{
    audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
		device, ((OUTPUT|INPUT|LOOPBACKTEST)<<16)|90);

    start_loopback();
    return 0;
}

int disable_loopback(int device)
{
    audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
		device, ((OUTPUT|INPUT|LOOPBACKTEST)<<16));

    stop_loopback();
    return 0;
}
void set_fm_frequency(int fmFreq)
{
    _fmFreq = fmFreq / 100;  /* Change the frequency to 100 kHz units */
    LOGI("set_fm_frequency; fmFreq= %d, _fmFreq= %d\n", fmFreq, _fmFreq);

    enable_fm_headset(0xFF /* invalid volume */);
}

int switch_sample_rate(int direction, int srADC, int srReq)
{
    return amixer_switch_sample_rate(direction, srADC, srReq);
}

// Mute all mic devices, no matter which device is used currently
void set_mic_mute(int on_off)
{
    audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_MUTE, \
                EARPIECE_DEVICE, (INPUT<<16)|on_off);
    audio_route_control(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_MUTE, \
                HEADSET_DEVICE, (INPUT<<16)|on_off);
}
