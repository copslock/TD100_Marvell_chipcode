/*
 * (C) Copyright 2010 Marvell International Ltd.
 * All Rights Reserved
 */
#ifndef _AUDIO_PATH_H
#define _AUDIO_PATH_H

#ifdef __cplusplus
extern "C" {
#endif

const char PHONE_STATE[] = "phone_state";
const char PHONE_VOLUME[] = "phone_volume";
const char FM_STATE[] = "FM_STATUS";
const char FM_VOLUME[] = "FM_VOLUME";
const char HDMI_STATE[] = "HDMI_STATUS";
const char STREAM_ACTIVE[] = "STREAM_ACTIVE";
const char VOLUME_FORCED_SPEAKER_STR[] = "VOLUME_FORCED_SPEAKER";
const char VOLUME_FORCED_HEADSET_STR[] = "VOLUME_FORCED_HEADSET";
const char DEVICE_CHANGED[] = "DEVICE_CHANGED";
const char STREAM_IN_ACTIVE[] = "STREAM_IN_ACTIVE";
const char PHONE_PLUGIN_ACTIVE[] = "PHONE_PLUGIN_ACTIVE";
const char CLOSE_INPUTS[] = "CLOSE_INPUTS";

const char * get_alsa_dev_name_for_device(uint32_t devices);
void enable_audio_path_for_device(uint32_t devices, unsigned char left, unsigned char right);
void disable_audio_path_for_device(uint32_t devices);
#ifdef WITH_ECPATH
void enable_audio_ecpath_for_device(uint32_t devices, unsigned char volume);
void disable_audio_ecpath_for_device(uint32_t devices, unsigned char volume);
#endif
void set_volume_for_device(uint32_t devices, unsigned char left, unsigned char right);
uint32_t get_all_output_device();
uint32_t get_all_input_device();
int audio_init_check();
#ifdef WITH_NREC
int set_side(uint32_t mState , int device, unsigned char volume, int nrec_on);
#else
int set_side(uint32_t mState , int device, unsigned char volume);
#endif
int isStandby();
int is_side_changed();
int setRouting( int device , int mDevice, unsigned char volume);
int enable_48k_sample();
int enable_44k1_sample();
int enable_8k_sample();
void set_Standby();
void check_Standby();
void set_mic_mute(int on_off);
#ifdef BOARD_USES_SANREMO
void set_fm_frequency(int fmFreq);
#endif

#ifdef WITH_LOOPBACK
int enable_loopback(int device);
int disable_loopback(int device);
#endif

#ifdef RECORD_OVER_PCM
int switch_sample_rate(int direction, int srADC, int srReq);
#endif

    // Copied from AudioSystem.h, must keep exactly the same
    enum audio_devices {
        // output devices
        DEVICE_OUT_EARPIECE = 0x1,
        DEVICE_OUT_SPEAKER = 0x2,
        DEVICE_OUT_WIRED_HEADSET = 0x4,
        DEVICE_OUT_WIRED_HEADPHONE = 0x8,
        DEVICE_OUT_BLUETOOTH_SCO = 0x10,
        DEVICE_OUT_BLUETOOTH_SCO_HEADSET = 0x20,
        DEVICE_OUT_BLUETOOTH_SCO_CARKIT = 0x40,
        DEVICE_OUT_BLUETOOTH_A2DP = 0x80,
        DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES = 0x100,
        DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER = 0x200,
        DEVICE_OUT_AUX_DIGITAL = 0x400,
        DEVICE_OUT_FM_HEADPHONE = 0x800,
        DEVICE_OUT_FM_SPEAKER = 0x1000,
        DEVICE_OUT_SPEAKER_SSPA2 = 0x2000,
        DEVICE_OUT_HDMI= 0x4000,
        DEVICE_OUT_FM_TRANSMITTER = 0x8000,
        DEVICE_OUT_DEFAULT = 0x8000,
        DEVICE_OUT_ALL = (DEVICE_OUT_EARPIECE | DEVICE_OUT_SPEAKER | DEVICE_OUT_WIRED_HEADSET |
                DEVICE_OUT_WIRED_HEADPHONE | DEVICE_OUT_BLUETOOTH_SCO | DEVICE_OUT_BLUETOOTH_SCO_HEADSET |
                DEVICE_OUT_BLUETOOTH_SCO_CARKIT | DEVICE_OUT_BLUETOOTH_A2DP | DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES |
                DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER | DEVICE_OUT_AUX_DIGITAL | DEVICE_OUT_FM_HEADPHONE |
                DEVICE_OUT_FM_SPEAKER | DEVICE_OUT_SPEAKER_SSPA2 | DEVICE_OUT_HDMI | DEVICE_OUT_FM_TRANSMITTER),

        // input devices
        DEVICE_IN_COMMUNICATION = 0x10000,
        DEVICE_IN_AMBIENT = 0x20000,
        DEVICE_IN_BUILTIN_MIC = 0x40000,
        DEVICE_IN_BLUETOOTH_SCO_HEADSET = 0x80000,
        DEVICE_IN_WIRED_HEADSET = 0x100000,
        DEVICE_IN_AUX_DIGITAL = 0x200000,
        DEVICE_IN_VOICE_CALL = 0x400000,
        DEVICE_IN_BACK_MIC = 0x800000,
        DEVICE_IN_VT_MIC  = 0x1000000,
        DEVICE_IN_FMRADIO  = 0x2000000,
        DEVICE_IN_DEFAULT = 0x80000000,

        DEVICE_IN_ALL = (DEVICE_IN_COMMUNICATION | DEVICE_IN_AMBIENT | DEVICE_IN_BUILTIN_MIC |
                DEVICE_IN_BLUETOOTH_SCO_HEADSET | DEVICE_IN_WIRED_HEADSET | DEVICE_IN_AUX_DIGITAL |
                DEVICE_IN_VOICE_CALL | DEVICE_IN_BACK_MIC | DEVICE_IN_VT_MIC | DEVICE_IN_DEFAULT | 
                DEVICE_IN_FMRADIO)
    };

    enum audio_mode {
        MODE_INVALID = -2,
        MODE_CURRENT = -1,
        MODE_NORMAL = 0,
        MODE_RINGTONE,
        MODE_IN_CALL,
        MODE_IN_COMMUNICATION,
        MODE_IN_VT_CALL,
        NUM_MODES  // not a valid entry, denotes end-of-list 
    };                  

	#define CP_SIDE 0
	#define AP_SIDE 1
	
#ifdef __cplusplus
};
#endif

#endif
