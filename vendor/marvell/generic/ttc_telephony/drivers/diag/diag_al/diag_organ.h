#ifndef DIAG_ORGAN_H
#define DIAG_ORGAN_H

#define VOLUME_CALIBRATION_NVM_FILE "audio_hifi_volume.nvm"
#define ORGAN_DAEMON_SOCKET "/data/misc/organ/sockets/organ_daemon"
#define MAX_PATH 256
#define UNUSEPARAM(param) (void)param;

// Copied from AudioService.java, must keep exactly the same
int MAX_STREAM_VOLUME[] = {
    5,  // STREAM_VOICE_CALL
    7,  // STREAM_SYSTEM
    7,  // STREAM_RING
    15, // STREAM_MUSIC
    7,  // STREAM_ALARM
    7,  // STREAM_NOTIFICATION
    15, // STREAM_BLUETOOTH_SCO
    7,  // STREAM_SYSTEM_ENFORCED
    15, // STREAM_DTMF
    15, // STREAM_TTS
    // Organ specific definition
    15, // STREAM_FM_RADIO
    0,  // STREAM_VOICE_RECODER
    0,  // STREAM_CAMCODER
    0,  // STREAM_ASR
    5,  // STREAM_VOIP
    0   // STREAM_VOIP_RECORDER
};

int DEFAULT_DB_VALUE_15[] = {
    0, 27, 33, 40, 47, 53, 60, 67, 73, 80, 87, 93, 100, 107, 113, 120
};

int DEFAULT_DB_VALUE_7[] = {
    0, 34, 48, 63, 77, 91, 106, 120
};

int DEFAULT_DB_VALUE_5[] = {
    0, 40, 60, 80, 100, 120
};

int DEFAULT_DB_VALUE_1[] = {
    120
};

enum organ_command {
    GET_VOLUME,
    RELOAD_NVM,
    SET_VOLUME
};

enum organ_status {
    STATUS_OK,
    STATUS_ERROR
};

// Copied from OrganDefinition.h, must keep exactly the same
enum organ_type {
    HifiPlayback,
    HifiRecord
};

enum organ_device {
    DEVICERESERVED,
    SPEAKER_DEV,
    HEADSET_DEV,
    SPEAKER_FORCEDSPEAKER_DEV,
    HEADSET_FORCEDSPEAKER_DEV,
    BLUETOOTH_DEV,
    MIC1_DEV,
    MIC2_DEV,
    HEADSETMIC_DEV,
    EARPIECE_DEV
};

enum organ_stream {
    STREAMRESERVED,
    VOICE_CALL,
    SYSTEM,
    RING,
    MUSIC,
    ALARM,
    NOTIFICATION,
    BLUETOOTH_SCO,
    ENFORCED_AUDIBLE,
    DTMF,
    TTS,
    FM_RADIO,
    VOICE_RECORDER,
    CAMCORDER,
    ASR,
    VOIP,
    VOIP_RECORDER
};

enum organ_format {
    FORMATRESERVED,
    MP3_FAT,
    AAC_FAT,
    OGG_FAT,
    MIDI_FAT,
    AMR_FAT,
    WAV_FAT,
    TONE_FAT,
    AVI_FAT,
    MP4_FAT,
    RMVB_FAT,
    PCM_FAT,
    AMR_NB_FAT
};

//extended android streams
enum ex_audio_streams {
    EX_VOIP = 20
};

typedef struct HiFi_Organ_Mapping {
    unsigned short paramType;  //0-playback 1-record
    unsigned int   deviceType; //output or input device
    unsigned short streamType; //stream type for playback, application style for record
    unsigned short formatType; //file format for playback, encoding format for record
} hifiMapping;

static const hifiMapping hifi_mapping[] = {
    //================================================
    {HifiPlayback, EARPIECE_DEV, VOICE_CALL, FORMATRESERVED},//playback, earpiece, VOICE_CALL, OTR
    {HifiPlayback, EARPIECE_DEV, VOIP,       FORMATRESERVED},//playback, earpiece, VOIP, OTR
    //------------------------------------------------
    {HifiPlayback, SPEAKER_DEV, VOICE_CALL, FORMATRESERVED},//playback, speaker, VOICE_CALL, OTR
    {HifiPlayback, SPEAKER_DEV, VOIP,       FORMATRESERVED},//playback, speaker, VOIP, OTR
    //------------------------------------------------
    {HifiPlayback, SPEAKER_DEV, SYSTEM, MP3_FAT}, //playback, speaker, SYSTEM, MP3
    {HifiPlayback, SPEAKER_DEV, SYSTEM, AAC_FAT}, //playback, speaker, SYSTEM, AAC
    {HifiPlayback, SPEAKER_DEV, SYSTEM, OGG_FAT}, //playback, speaker, SYSTEM, OGG
    {HifiPlayback, SPEAKER_DEV, SYSTEM, MIDI_FAT}, //playback, speaker, SYSTEM, MIDI
    {HifiPlayback, SPEAKER_DEV, SYSTEM, AMR_FAT}, //playback, speaker, SYSTEM, AMR
    {HifiPlayback, SPEAKER_DEV, SYSTEM, WAV_FAT}, //playback, speaker, SYSTEM, WAV
    {HifiPlayback, SPEAKER_DEV, SYSTEM, TONE_FAT}, //playback, speaker, SYSTEM, TONE
    //------------------------------------------------
    {HifiPlayback, SPEAKER_DEV, RING, MP3_FAT}, //playback, speaker, RING, MP3
    {HifiPlayback, SPEAKER_DEV, RING, AAC_FAT}, //playback, speaker, RING, AAC
    {HifiPlayback, SPEAKER_DEV, RING, OGG_FAT}, //playback, speaker, RING, OGG
    {HifiPlayback, SPEAKER_DEV, RING, MIDI_FAT}, //playback, speaker, RING, MIDI
    {HifiPlayback, SPEAKER_DEV, RING, AMR_FAT}, //playback, speaker, RING, AMR
    {HifiPlayback, SPEAKER_DEV, RING, WAV_FAT}, //playback, speaker, RING, WAV
    {HifiPlayback, SPEAKER_DEV, RING, TONE_FAT}, //playback, speaker, RING, TONE
    //------------------------------------------------
    {HifiPlayback, SPEAKER_DEV, MUSIC, MP3_FAT}, //playback, speaker, MUSIC, MP3
    {HifiPlayback, SPEAKER_DEV, MUSIC, AAC_FAT}, //playback, speaker, MUSIC, AAC
    {HifiPlayback, SPEAKER_DEV, MUSIC, OGG_FAT}, //playback, speaker, MUSIC, OGG
    {HifiPlayback, SPEAKER_DEV, MUSIC, MIDI_FAT}, //playback, speaker, MUSIC, MIDI
    {HifiPlayback, SPEAKER_DEV, MUSIC, AMR_FAT}, //playback, speaker, MUSIC, AMR
    {HifiPlayback, SPEAKER_DEV, MUSIC, WAV_FAT}, //playback, speaker, MUSIC, WAV
    {HifiPlayback, SPEAKER_DEV, MUSIC, AVI_FAT}, //playback, speaker, MUSIC, AVI
    {HifiPlayback, SPEAKER_DEV, MUSIC, MP4_FAT}, //playback, speaker, MUSIC, MP4
    {HifiPlayback, SPEAKER_DEV, MUSIC, RMVB_FAT}, //playback, speaker, MUSIC, RMVB
    {HifiPlayback, SPEAKER_DEV, MUSIC, TONE_FAT}, //playback, speaker, MUSIC, TONE
    //------------------------------------------------
    {HifiPlayback, SPEAKER_DEV, ALARM, MP3_FAT}, //playback, speaker, ALARM, MP3
    {HifiPlayback, SPEAKER_DEV, ALARM, AAC_FAT}, //playback, speaker, ALARM, AAC
    {HifiPlayback, SPEAKER_DEV, ALARM, OGG_FAT}, //playback, speaker, ALARM, OGG
    {HifiPlayback, SPEAKER_DEV, ALARM, MIDI_FAT}, //playback, speaker, ALARM, MIDI
    {HifiPlayback, SPEAKER_DEV, ALARM, AMR_FAT}, //playback, speaker, ALARM, AMR
    {HifiPlayback, SPEAKER_DEV, ALARM, WAV_FAT}, //playback, speaker, ALARM, WAV
    {HifiPlayback, SPEAKER_DEV, ALARM, TONE_FAT}, //playback, speaker, ALARM, TONE
    //------------------------------------------------
    {HifiPlayback, SPEAKER_DEV, NOTIFICATION, MP3_FAT}, //playback, speaker, NOTIFICATION, MP3
    {HifiPlayback, SPEAKER_DEV, NOTIFICATION, AAC_FAT}, //playback, speaker, NOTIFICATION, AAC
    {HifiPlayback, SPEAKER_DEV, NOTIFICATION, OGG_FAT}, //playback, speaker, NOTIFICATION, OGG
    {HifiPlayback, SPEAKER_DEV, NOTIFICATION, MIDI_FAT}, //playback, speaker, NOTIFICATION, MIDI
    {HifiPlayback, SPEAKER_DEV, NOTIFICATION, AMR_FAT}, //playback, speaker, NOTIFICATION, AMR
    {HifiPlayback, SPEAKER_DEV, NOTIFICATION, WAV_FAT}, //playback, speaker, NOTIFICATION, WAV
    {HifiPlayback, SPEAKER_DEV, NOTIFICATION, TONE_FAT}, //playback, speaker, NOTIFICATION, TONE
    //------------------------------------------------
    {HifiPlayback, SPEAKER_DEV, BLUETOOTH_SCO, FORMATRESERVED}, //playback, speaker, BLUETOOTH_SCO, OTR
    //------------------------------------------------
    {HifiPlayback, SPEAKER_DEV, ENFORCED_AUDIBLE, MP3_FAT}, //playback, speaker, ENFORCED_AUDIBLE, MP3
    {HifiPlayback, SPEAKER_DEV, ENFORCED_AUDIBLE, AAC_FAT}, //playback, speaker, ENFORCED_AUDIBLE, AAC
    {HifiPlayback, SPEAKER_DEV, ENFORCED_AUDIBLE, OGG_FAT}, //playback, speaker, ENFORCED_AUDIBLE, OGG
    {HifiPlayback, SPEAKER_DEV, ENFORCED_AUDIBLE, MIDI_FAT}, //playback, speaker, ENFORCED_AUDIBLE, MIDI
    {HifiPlayback, SPEAKER_DEV, ENFORCED_AUDIBLE, AMR_FAT}, //playback, speaker, ENFORCED_AUDIBLE, AMR
    {HifiPlayback, SPEAKER_DEV, ENFORCED_AUDIBLE, WAV_FAT}, //playback, speaker, ENFORCED_AUDIBLE, WAV
    {HifiPlayback, SPEAKER_DEV, ENFORCED_AUDIBLE, TONE_FAT}, //playback, speaker, ENFORCED_AUDIBLE, TONE
    //------------------------------------------------
    {HifiPlayback, SPEAKER_DEV, DTMF, FORMATRESERVED}, //playback, speaker, DTMF, OTR
    //------------------------------------------------
    {HifiPlayback, SPEAKER_DEV, TTS, FORMATRESERVED}, //playback, speaker, TTS, OTR
    //------------------------------------------------
    {HifiPlayback, SPEAKER_DEV, FM_RADIO, FORMATRESERVED}, //playback, speaker, FMRADIO, OTR

    //================================================
    {HifiPlayback, HEADSET_DEV, VOICE_CALL, FORMATRESERVED},//playback, headset, VOICE_CALL, OTR
    {HifiPlayback, HEADSET_DEV, VOIP,       FORMATRESERVED},//playback, headset, VOIP, OTR
    //------------------------------------------------
    {HifiPlayback, HEADSET_DEV, SYSTEM, MP3_FAT}, //playback, headset, SYSTEM, MP3
    {HifiPlayback, HEADSET_DEV, SYSTEM, AAC_FAT}, //playback, headset, SYSTEM, AAC
    {HifiPlayback, HEADSET_DEV, SYSTEM, OGG_FAT}, //playback, headset, SYSTEM, OGG
    {HifiPlayback, HEADSET_DEV, SYSTEM, MIDI_FAT}, //playback, headset, SYSTEM, MIDI
    {HifiPlayback, HEADSET_DEV, SYSTEM, AMR_FAT}, //playback, headset, SYSTEM, AMR
    {HifiPlayback, HEADSET_DEV, SYSTEM, WAV_FAT}, //playback, headset, SYSTEM, WAV
    {HifiPlayback, HEADSET_DEV, SYSTEM, TONE_FAT}, //playback, headset, SYSTEM, TONE
    //------------------------------------------------
    {HifiPlayback, HEADSET_DEV, RING, MP3_FAT}, //playback, headset, RING, MP3
    {HifiPlayback, HEADSET_DEV, RING, AAC_FAT}, //playback, headset, RING, AAC
    {HifiPlayback, HEADSET_DEV, RING, OGG_FAT}, //playback, headset, RING, OGG
    {HifiPlayback, HEADSET_DEV, RING, MIDI_FAT}, //playback, headset, RING, MIDI
    {HifiPlayback, HEADSET_DEV, RING, AMR_FAT}, //playback, headset, RING, AMR
    {HifiPlayback, HEADSET_DEV, RING, WAV_FAT}, //playback, headset, RING, WAV
    {HifiPlayback, HEADSET_DEV, RING, TONE_FAT}, //playback, headset, RING, TONE
    //------------------------------------------------
    {HifiPlayback, HEADSET_DEV, MUSIC, MP3_FAT}, //playback, headset, MUSIC, MP3
    {HifiPlayback, HEADSET_DEV, MUSIC, AAC_FAT}, //playback, headset, MUSIC, AAC
    {HifiPlayback, HEADSET_DEV, MUSIC, OGG_FAT}, //playback, headset, MUSIC, OGG
    {HifiPlayback, HEADSET_DEV, MUSIC, MIDI_FAT}, //playback, headset, MUSIC, MIDI
    {HifiPlayback, HEADSET_DEV, MUSIC, AMR_FAT}, //playback, headset, MUSIC, AMR
    {HifiPlayback, HEADSET_DEV, MUSIC, WAV_FAT}, //playback, headset, MUSIC, WAV
    {HifiPlayback, HEADSET_DEV, MUSIC, AVI_FAT}, //playback, headset, MUSIC, AVI
    {HifiPlayback, HEADSET_DEV, MUSIC, MP4_FAT}, //playback, headset, MUSIC, MP4
    {HifiPlayback, HEADSET_DEV, MUSIC, RMVB_FAT}, //playback, headset, MUSIC, RMVB
    {HifiPlayback, HEADSET_DEV, MUSIC, TONE_FAT}, //playback, headset, MUSIC, TONE
    //------------------------------------------------
    {HifiPlayback, HEADSET_DEV, ALARM, MP3_FAT}, //playback, headset, ALARM, MP3
    {HifiPlayback, HEADSET_DEV, ALARM, AAC_FAT}, //playback, headset, ALARM, AAC
    {HifiPlayback, HEADSET_DEV, ALARM, OGG_FAT}, //playback, headset, ALARM, OGG
    {HifiPlayback, HEADSET_DEV, ALARM, MIDI_FAT}, //playback, headset, ALARM, MIDI
    {HifiPlayback, HEADSET_DEV, ALARM, AMR_FAT}, //playback, headset, ALARM, AMR
    {HifiPlayback, HEADSET_DEV, ALARM, WAV_FAT}, //playback, headset, ALARM, WAV
    {HifiPlayback, HEADSET_DEV, ALARM, TONE_FAT}, //playback, headset, ALARM, TONE
    //------------------------------------------------
    {HifiPlayback, HEADSET_DEV, NOTIFICATION, MP3_FAT}, //playback, headset, NOTIFICATION, MP3
    {HifiPlayback, HEADSET_DEV, NOTIFICATION, AAC_FAT}, //playback, headset, NOTIFICATION, AAC
    {HifiPlayback, HEADSET_DEV, NOTIFICATION, OGG_FAT}, //playback, headset, NOTIFICATION, OGG
    {HifiPlayback, HEADSET_DEV, NOTIFICATION, MIDI_FAT}, //playback, headset, NOTIFICATION, MIDI
    {HifiPlayback, HEADSET_DEV, NOTIFICATION, AMR_FAT}, //playback, headset, NOTIFICATION, AMR
    {HifiPlayback, HEADSET_DEV, NOTIFICATION, WAV_FAT}, //playback, headset, NOTIFICATION, WAV
    {HifiPlayback, HEADSET_DEV, NOTIFICATION, TONE_FAT}, //playback, headset, NOTIFICATION, TONE
    //------------------------------------------------
    {HifiPlayback, HEADSET_DEV, BLUETOOTH_SCO, FORMATRESERVED}, //playback, headset, BLUETOOTH_SCO, OTR
    //------------------------------------------------
    {HifiPlayback, HEADSET_DEV, ENFORCED_AUDIBLE, MP3_FAT}, //playback, headset, ENFORCED_AUDIBLE, MP3
    {HifiPlayback, HEADSET_DEV, ENFORCED_AUDIBLE, AAC_FAT}, //playback, headset, ENFORCED_AUDIBLE, AAC
    {HifiPlayback, HEADSET_DEV, ENFORCED_AUDIBLE, OGG_FAT}, //playback, headset, ENFORCED_AUDIBLE, OGG
    {HifiPlayback, HEADSET_DEV, ENFORCED_AUDIBLE, MIDI_FAT}, //playback, headset, ENFORCED_AUDIBLE, MIDI
    {HifiPlayback, HEADSET_DEV, ENFORCED_AUDIBLE, AMR_FAT}, //playback, headset, ENFORCED_AUDIBLE, AMR
    {HifiPlayback, HEADSET_DEV, ENFORCED_AUDIBLE, WAV_FAT}, //playback, headset, ENFORCED_AUDIBLE, WAV
    {HifiPlayback, HEADSET_DEV, ENFORCED_AUDIBLE, TONE_FAT}, //playback, headset, ENFORCED_AUDIBLE, TONE
    //------------------------------------------------
    {HifiPlayback, HEADSET_DEV, DTMF, FORMATRESERVED},//playback, headset, DTMF, OTR
    //------------------------------------------------
    {HifiPlayback, HEADSET_DEV, TTS, FORMATRESERVED},//playback, headset, TTS, OTR
    //------------------------------------------------
    {HifiPlayback, HEADSET_DEV, FM_RADIO, FORMATRESERVED},//playback, headset, FMRADIO, OTR
    //------------------------------------------------
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, RING, MP3_FAT}, //playback, headset_forcedspeaker, RING, MP3
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, RING, AAC_FAT}, //playback, headset_forcedspeaker, RING, AAC
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, RING, OGG_FAT}, //playback, headset_forcedspeaker, RING, OGG
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, RING, MIDI_FAT},//playback, headset_forcedspeaker, RING, MIDI
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, RING, AMR_FAT}, //playback, headset_forcedspeaker, RING, AMR
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, RING, WAV_FAT}, //playback, headset_forcedspeaker, RING, WAV
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, RING, TONE_FAT}, //playback, headset_forcedspeaker, RING, TONE
    //------------------------------------------------
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, ALARM, MP3_FAT}, //playback, headset_forcedspeaker, ALARM, MP3
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, ALARM, AAC_FAT}, //playback, headset_forcedspeaker, ALARM, AAC
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, ALARM, OGG_FAT}, //playback, headset_forcedspeaker, ALARM, OGG
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, ALARM, MIDI_FAT},//playback, headset_forcedspeaker, ALARM, MIDI
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, ALARM, AMR_FAT}, //playback, headset_forcedspeaker, ALARM, AMR
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, ALARM, WAV_FAT}, //playback, headset_forcedspeaker, ALARM, WAV
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, ALARM, TONE_FAT}, //playback, headset_forcedspeaker, ALARM, TONE
    //------------------------------------------------
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, NOTIFICATION, MP3_FAT}, //playback, headset_forcedspeaker, NOTIFICATION, MP3
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, NOTIFICATION, AAC_FAT}, //playback, headset_forcedspeaker, NOTIFICATION, AAC
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, NOTIFICATION, OGG_FAT}, //playback, headset_forcedspeaker, NOTIFICATION, OGG
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, NOTIFICATION, MIDI_FAT},//playback, headset_forcedspeaker, NOTIFICATION, MIDI
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, NOTIFICATION, AMR_FAT}, //playback, headset_forcedspeaker, NOTIFICATION, AMR
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, NOTIFICATION, WAV_FAT}, //playback, headset_forcedspeaker, NOTIFICATION, WAV
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, NOTIFICATION, TONE_FAT}, //playback, headset_forcedspeaker, NOTIFICATION, TONE
    //------------------------------------------------
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, ENFORCED_AUDIBLE, MP3_FAT}, //playback, headset_forcedspeaker, ENFORCED_AUDIBLE, MP3
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, ENFORCED_AUDIBLE, AAC_FAT}, //playback, headset_forcedspeaker, ENFORCED_AUDIBLE, AAC
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, ENFORCED_AUDIBLE, OGG_FAT}, //playback, headset_forcedspeaker, ENFORCED_AUDIBLE, OGG
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, ENFORCED_AUDIBLE, MIDI_FAT},//playback, headset_forcedspeaker, ENFORCED_AUDIBLE, MIDI
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, ENFORCED_AUDIBLE, AMR_FAT}, //playback, headset_forcedspeaker, ENFORCED_AUDIBLE, AMR
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, ENFORCED_AUDIBLE, WAV_FAT}, //playback, headset_forcedspeaker, ENFORCED_AUDIBLE, WAV
    {HifiPlayback, HEADSET_FORCEDSPEAKER_DEV, ENFORCED_AUDIBLE, TONE_FAT}, //playback, headset_forcedspeaker, ENFORCED_AUDIBLE, TONE
    //------------------------------------------------
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, RING, MP3_FAT}, //playback, speaker_forcedspeaker, RING, MP3
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, RING, AAC_FAT}, //playback, speaker_forcedspeaker, RING, AAC
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, RING, OGG_FAT}, //playback, speaker_forcedspeaker, RING, OGG
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, RING, MIDI_FAT},//playback, speaker_forcedspeaker, RING, MIDI
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, RING, AMR_FAT}, //playback, speaker_forcedspeaker, RING, AMR
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, RING, WAV_FAT}, //playback, speaker_forcedspeaker, RING, WAV
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, RING, TONE_FAT}, //playback, speaker_forcedspeaker, RING, TONE
    //------------------------------------------------
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, ALARM, MP3_FAT}, //playback, speaker_forcedspeaker, ALARM, MP3
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, ALARM, AAC_FAT}, //playback, speaker_forcedspeaker, ALARM, AAC
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, ALARM, OGG_FAT}, //playback, speaker_forcedspeaker, ALARM, OGG
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, ALARM, MIDI_FAT},//playback, speaker_forcedspeaker, ALARM, MIDI
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, ALARM, AMR_FAT}, //playback, speaker_forcedspeaker, ALARM, AMR
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, ALARM, WAV_FAT}, //playback, speaker_forcedspeaker, ALARM, WAV
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, ALARM, TONE_FAT}, //playback, speaker_forcedspeaker, ALARM, TONE
    //------------------------------------------------
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, NOTIFICATION, MP3_FAT}, //playback, speaker_forcedspeaker, NOTIFICATION, MP3
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, NOTIFICATION, AAC_FAT}, //playback, speaker_forcedspeaker, NOTIFICATION, AAC
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, NOTIFICATION, OGG_FAT}, //playback, speaker_forcedspeaker, NOTIFICATION, OGG
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, NOTIFICATION, MIDI_FAT},//playback, speaker_forcedspeaker, NOTIFICATION, MIDI
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, NOTIFICATION, AMR_FAT}, //playback, speaker_forcedspeaker, NOTIFICATION, AMR
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, NOTIFICATION, WAV_FAT}, //playback, speaker_forcedspeaker, NOTIFICATION, WAV
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, NOTIFICATION, TONE_FAT}, //playback, speaker_forcedspeaker, NOTIFICATION, TONE
    //------------------------------------------------
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, ENFORCED_AUDIBLE, MP3_FAT}, //playback, speaker_forcedspeaker, ENFORCED_AUDIBLE, MP3
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, ENFORCED_AUDIBLE, AAC_FAT}, //playback, speaker_forcedspeaker, ENFORCED_AUDIBLE, AAC
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, ENFORCED_AUDIBLE, OGG_FAT}, //playback, speaker_forcedspeaker, ENFORCED_AUDIBLE, OGG
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, ENFORCED_AUDIBLE, MIDI_FAT},//playback, speaker_forcedspeaker, ENFORCED_AUDIBLE, MIDI
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, ENFORCED_AUDIBLE, AMR_FAT}, //playback, speaker_forcedspeaker, ENFORCED_AUDIBLE, AMR
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, ENFORCED_AUDIBLE, WAV_FAT}, //playback, speaker_forcedspeaker, ENFORCED_AUDIBLE, WAV
    {HifiPlayback, SPEAKER_FORCEDSPEAKER_DEV, ENFORCED_AUDIBLE, TONE_FAT}, //playback, speaker_forcedspeaker, ENFORCED_AUDIBLE, TONE

    //================================================
    {HifiPlayback, BLUETOOTH_DEV, MUSIC, MP3_FAT}, //playback, bluetooth, MUSIC, MP3
    {HifiPlayback, BLUETOOTH_DEV, MUSIC, AAC_FAT}, //playback, bluetooth, MUSIC, AAC
    {HifiPlayback, BLUETOOTH_DEV, MUSIC, OGG_FAT}, //playback, bluetooth, MUSIC, OGG
    {HifiPlayback, BLUETOOTH_DEV, MUSIC, MIDI_FAT}, //playback, bluetooth, MUSIC, MIDI
    {HifiPlayback, BLUETOOTH_DEV, MUSIC, AMR_FAT}, //playback, bluetooth, MUSIC, AMR
    {HifiPlayback, BLUETOOTH_DEV, MUSIC, WAV_FAT}, //playback, bluetooth, MUSIC, WAV
    {HifiPlayback, BLUETOOTH_DEV, MUSIC, AVI_FAT}, //playback, bluetooth, MUSIC, AVI
    {HifiPlayback, BLUETOOTH_DEV, MUSIC, MP4_FAT}, //playback, bluetooth, MUSIC, MP4
    {HifiPlayback, BLUETOOTH_DEV, MUSIC, RMVB_FAT}, //playback, bluetooth, MUSIC, RMVB
    {HifiPlayback, BLUETOOTH_DEV, MUSIC, TONE_FAT}, //playback, bluetooth, MUSIC, TONE

    //================================================
    {HifiRecord, MIC1_DEV, VOICE_RECORDER, PCM_FAT}, //record, built-in mic1, voice recorder, PCM
    {HifiRecord, MIC1_DEV, VOICE_RECORDER, AMR_NB_FAT}, //record, built-in mic1, voice recorder, AMR-NB
    {HifiRecord, MIC1_DEV, VOICE_RECORDER, AAC_FAT}, //record, built-in mic1, voice recorder, AAC
    {HifiRecord, MIC1_DEV, CAMCORDER, PCM_FAT}, //record, built-in mic1, camcorder, PCM
    {HifiRecord, MIC1_DEV, CAMCORDER, AMR_NB_FAT}, //record, built-in mic1, camcorder, AMR-NB
    {HifiRecord, MIC1_DEV, CAMCORDER, AAC_FAT}, //record, built-in mic1, camcorder, AAC
    {HifiRecord, MIC1_DEV, ASR, PCM_FAT}, //record, built-in mic1, ASR, PCM
    {HifiRecord, MIC1_DEV, ASR, AMR_NB_FAT}, //record, built-in mic1, ASR, AMR-NB
    {HifiRecord, MIC1_DEV, ASR, AAC_FAT}, //record, built-in mic1, ASR, AAC
    {HifiRecord, MIC1_DEV, VOIP_RECORDER, PCM_FAT},    //record, built-in mic1, VOIP_RECORDER, PCM
    {HifiRecord, MIC1_DEV, VOIP_RECORDER, AMR_NB_FAT}, //record, built-in mic1, VOIP_RECORDER, AMR-NB
    {HifiRecord, MIC1_DEV, VOIP_RECORDER, AAC_FAT},    //record, built-in mic1, VOIP_RECORDER, AAC
    //================================================
    {HifiRecord, MIC2_DEV, VOICE_RECORDER, PCM_FAT}, //record, built-in mic2, voice recorder, PCM
    {HifiRecord, MIC2_DEV, VOICE_RECORDER, AMR_NB_FAT}, //record, built-in mic2, voice recorder, AMR-NB
    {HifiRecord, MIC2_DEV, VOICE_RECORDER, AAC_FAT}, //record, built-in mic2, voice recorder, AAC
    {HifiRecord, MIC2_DEV, CAMCORDER, PCM_FAT}, //record, built-in mic2, camcorder, PCM
    {HifiRecord, MIC2_DEV, CAMCORDER, AMR_NB_FAT}, //record, built-in mic2, camcorder, AMR-NB
    {HifiRecord, MIC2_DEV, CAMCORDER, AAC_FAT}, //record, built-in mic2, camcorder, AAC
    {HifiRecord, MIC2_DEV, ASR, PCM_FAT}, //record, built-in mic2, ASR, PCM
    {HifiRecord, MIC2_DEV, ASR, AMR_NB_FAT}, //record, built-in mic2, ASR, AMR-NB
    {HifiRecord, MIC2_DEV, ASR, AAC_FAT}, //record, built-in mic2, ASR, AAC
    {HifiRecord, MIC2_DEV, VOIP_RECORDER, PCM_FAT},    //record, built-in mic2, VOIP_RECORDER, PCM
    {HifiRecord, MIC2_DEV, VOIP_RECORDER, AMR_NB_FAT}, //record, built-in mic2, VOIP_RECORDER, AMR-NB
    {HifiRecord, MIC2_DEV, VOIP_RECORDER, AAC_FAT},    //record, built-in mic2, VOIP_RECORDER, AAC
    //================================================
    {HifiRecord, HEADSETMIC_DEV, VOICE_RECORDER, PCM_FAT}, //record, headset mic, voice recorder, PCM
    {HifiRecord, HEADSETMIC_DEV, VOICE_RECORDER, AMR_NB_FAT}, //record, headset mic, voice recorder, AMR-NB
    {HifiRecord, HEADSETMIC_DEV, VOICE_RECORDER, AAC_FAT}, //record, headset mic, voice recorder, AAC
    {HifiRecord, HEADSETMIC_DEV, CAMCORDER, PCM_FAT}, //record, headset mic, camcorder, PCM
    {HifiRecord, HEADSETMIC_DEV, CAMCORDER, AMR_NB_FAT}, //record, headset mic, camcorder, AMR-NB
    {HifiRecord, HEADSETMIC_DEV, CAMCORDER, AAC_FAT}, //record, headset mic, camcorder, AAC
    {HifiRecord, HEADSETMIC_DEV, ASR, PCM_FAT}, //record, headset mic, ASR, PCM
    {HifiRecord, HEADSETMIC_DEV, ASR, AMR_NB_FAT}, //record, headset mic, ASR, AMR-NB
    {HifiRecord, HEADSETMIC_DEV, ASR, AAC_FAT}, //record, headset mic, ASR, AAC
    {HifiRecord, HEADSETMIC_DEV, VOIP_RECORDER, PCM_FAT},    //record, headset mic, VOIP_RECORDER, PCM
    {HifiRecord, HEADSETMIC_DEV, VOIP_RECORDER, AMR_NB_FAT}, //record, headset mic, VOIP_RECORDER, AMR-NB
    {HifiRecord, HEADSETMIC_DEV, VOIP_RECORDER, AAC_FAT},    //record, headset mic, VOIP_RECORDER, AAC
};

// NVM file header definition
typedef struct NVM_Header {
    unsigned long StructSize;       // the size of the user structure below
    unsigned long NumofStructs;     // >1 in case of array of structs (default is 1).
    char StructName[64];            // the name of the user structure below
    char Date[32];                  // date updated by the ICAT when the file is saved. Filled by calibration SW.
    char time[32];                  // time updated by the ICAT when the file is saved. Filled by calibration SW.
    char Version[64];               // user version - this field is updated by the SW eng. Every time they update the UserStruct.
    char HW_ID[32];                 // signifies the board number. Filled by calibration SW.
    char CalibVersion[32];          // signifies the calibration SW version. Filled by calibration SW.
} NVM_Header_ts;

//ICAT EXPORTED STRUCT
typedef struct HiFi_Organ_Params {
    unsigned short paramType; //0-playback 1-record
    unsigned short deviceType; //output or input device
    unsigned short streamType; //stream type for playback, application style for record
    unsigned short formatType; //file format for playback, encoding format for record
    unsigned short volumeIndex; //only valid for playback for record fixed to 0
    unsigned short dBIndex; //index to indicate the dB
} hifiParam;

//ICAT EXPORTED ENUM
typedef enum {
    MUSIC_MP3_SPK =0,
    MUSIC_AAC_SPK =1,
    MUSIC_OGG_SPK =2,
    MUSIC_MIDI_SPK =3,
    MUSIC_AMR_SPK =4,
    MUSIC_WAV_SPK =5,
    MUSIC_TONE_SPK =6,
    MUSIC_AVI_SPK =7,
    MUSIC_MP4_SPK =8,
    MUSIC_RMVB_SPK =9,
    MUSIC_MP3_HS =10,
    MUSIC_AAC_HS =11,
    MUSIC_OGG_HS =12,
    MUSIC_MIDI_HS =13,
    MUSIC_AMR_HS =14,
    MUSIC_WAV_HS =15,
    MUSIC_TONE_HS = 16,
    MUSIC_AVI_HS =17,
    MUSIC_MP4_HS =18,
    MUSIC_RMVB_HS =19,
    MUSIC_MP3_BT =20,
    MUSIC_AAC_BT =21,
    MUSIC_OGG_BT =22,
    MUSIC_MIDI_BT =23,
    MUSIC_AMR_BT =24,
    MUSIC_WAV_BT =25,
    MUSIC_TONE_BT =26,
    MUSIC_AVI_BT =27,
    MUSIC_MP4_BT =28,
    MUSIC_RMVB_BT =29,
    SYSTEM_MP3_SPK =30,
    SYSTEM_AAC_SPK =31,
    SYSTEM_OGG_SPK = 32,
    SYSTEM_MIDI_SPK =33,
    SYSTEM_AMR_SPK = 34,
    SYSTEM_WAV_SPK = 35,
    SYSTEM_TONE_SPK =36,
    SYSTEM_MP3_HS = 37,
    SYSTEM_AAC_HS = 38,
    SYSTEM_OGG_HS = 39,
    SYSTEM_MIDI_HS = 40,
    SYSTEM_AMR_HS = 41,
    SYSTEM_WAV_HS = 42,
    SYSTEM_TONE_HS = 43,
    RING_MP3_SPK = 44,
    RING_AAC_SPK = 45,
    RING_OGG_SPK = 46,
    RING_MIDI_SPK = 47,
    RING_AMR_SPK = 48,
    RING_WAV_SPK = 49,
    RING_TONE_SPK =50,
    RING_MP3_HS = 51,
    RING_AAC_HS = 52,
    RING_OGG_HS = 53,
    RING_MIDI_HS = 54,
    RING_AMR_HS = 55,
    RING_WAV_HS = 56,
    RING_TONE_HS = 57,
    RING_MP3_SPK_FSPK = 58,
    RING_AAC_SPK_FSPK = 59,
    RING_OGG_SPK_FSPK = 60,
    RING_MIDI_SPK_FSPK = 61,
    RING_AMR_SPK_FSPK = 62,
    RING_WAV_SPK_FSPK = 63,
    RING_TONE_SPK_FSPK =64,
    RING_MP3_HS_FSPK = 65,
    RING_AAC_HS_FSPK = 66,
    RING_OGG_HS_FSPK = 67,
    RING_MIDI_HS_FSPK = 68,
    RING_AMR_HS_FSPK =69,
    RING_WAV_HS_FSPK =70,
    RING_TONE_HS_FSPK =71,
    ALARM_MP3_SPK = 72,
    ALARM_AAC_SPK = 73,
    ALARM_OGG_SPK = 74,
    ALARM_MIDI_SPK =75,
    ALARM_AMR_SPK = 76,
    ALARM_WAV_SPK = 77,
    ALARM_TONE_SPK =78,
    ALARM_MP3_HS = 79,
    ALARM_AAC_HS = 80,
    ALARM_OGG_HS = 81,
    ALARM_MIDI_HS = 82,
    ALARM_AMR_HS = 83,
    ALARM_WAV_HS = 84,
    ALARM_TONE_HS= 85,
    ALARM_MP3_SPK_FSPK =86,
    ALARM_AAC_SPK_FSPK =87,
    ALARM_OGG_SPK_FSPK =88,
    ALARM_MIDI_SPK_FSPK =89,
    ALARM_AMR_SPK_FSPK = 90,
    ALARM_WAV_SPK_FSPK =91,
    ALARM_TONE_SPK_FSPK =92,
    ALARM_MP3_HS_FSPK = 93,
    ALARM_AAC_HS_FSPK = 94,
    ALARM_OGG_HS_FSPK = 95,
    ALARM_MIDI_HS_FSPK = 96,
    ALARM_AMR_HS_FSPK = 97,
    ALARM_WAV_HS_FSPK = 98,
    ALARM_TONE_HS_FSPK= 99,
    NOTIFICATION_MP3_SPK =100,
    NOTIFICATION_AAC_SPK =101,
    NOTIFICATION_OGG_SPK =102,
    NOTIFICATION_MIDI_SPK =103,
    NOTIFICATION_AMR_SPK =104,
    NOTIFICATION_WAV_SPK =105,
    NOTIFICATION_TONE_SPK =106,
    NOTIFICATION_MP3_HS = 107,
    NOTIFICATION_AAC_HS = 108,
    NOTIFICATION_OGG_HS = 109,
    NOTIFICATION_MIDI_HS = 110,
    NOTIFICATION_AMR_HS = 111,
    NOTIFICATION_WAV_HS = 112,
    NOTIFICATION_TONE_HS= 113,
    NOTIFICATION_MP3_SPK_FSPK =114,
    NOTIFICATION_AAC_SPK_FSPK =115,
    NOTIFICATION_OGG_SPK_FSPK =116,
    NOTIFICATION_MIDI_SPK_FSPK =117,
    NOTIFICATION_AMR_SPK_FSPK =118,
    NOTIFICATION_WAV_SPK_FSPK =119,
    NOTIFICATION_TONE_SPK_FSPK =120,
    NOTIFICATION_MP3_HS_FSPK =121,
    NOTIFICATION_AAC_HS_FSPK =122,
    NOTIFICATION_OGG_HS_FSPK =123,
    NOTIFICATION_MIDI_HS_FSPK =124,
    NOTIFICATION_AMR_HS_FSPK =125,
    NOTIFICATION_WAV_HS_FSPK =126,
    NOTIFICATION_TONE_HS_FSPK =127,
    ENFORCED_MP3_SPK =128,
    ENFORCED_AAC_SPK =129,
    ENFORCED_OGG_SPK =130,
    ENFORCED_MIDI_SPK =131,
    ENFORCED_AMR_SPK =132,
    ENFORCED_WAV_SPK =133,
    ENFORCED_TONE_SPK =134,
    ENFORCED_MP3_HS =135,
    ENFORCED_AAC_HS =136,
    ENFORCED_OGG_HS =137,
    ENFORCED_MIDI_HS =138,
    ENFORCED_AMR_HS =139,
    ENFORCED_WAV_HS =140,
    ENFORCED_TONE_HS= 141,
    ENFORCED_MP3_SPK_FSPK =142,
    ENFORCED_AAC_SPK_FSPK =143,
    ENFORCED_OGG_SPK_FSPK =144,
    ENFORCED_MIDI_SPK_FSPK =145,
    ENFORCED_AMR_SPK_FSPK =146,
    ENFORCED_WAV_SPK_FSPK =147,
    ENFORCED_TONE_SPK_FSPK= 148,
    ENFORCED_MP3_HS_FSPK=149,
    ENFORCED_AAC_HS_FSPK=150,
    ENFORCED_OGG_HS_FSPK=151,
    ENFORCED_MIDI_HS_FSPK=152,
    ENFORCED_AMR_HS_FSPK=153,
    ENFORCED_WAV_HS_FSPK=154,
    ENFORCED_TONE_HS_FSPK= 155,
    VOICE_CALL_SPK =156,
    VOICE_CALL_HS =157,
    BLUETOOTH_SCO_SPK =158,
    BLUETOOTH_SCO_HS =159,
    DTMF_SPK =160,
    DTMF_HS =161,
    TTS_SPK =162,
    TTS_HS =163,
    FM_RADIO_SPK =164,
    FM_RADIO_HS =165,
    RECORDER_PCM_MIC1 =166,
    RECORDER_AMR_NB_MIC1 =167,
    RECORDER_AAC_MIC1 =168,
    RECORDER_PCM_MIC2 =169,
    RECORDER_AMR_NB_MIC2 =170,
    RECORDER_AAC_MIC2 =171,
    RECORDER_PCM_HSMIC =172,
    RECORDER_AMR_NB_HSMIC =173,
    RECORDER_AAC_HSMIC =174,
    CAMCORDER_PCM_MIC1 =175,
    CAMCORDER_AMR_NB_MIC1 =176,
    CAMCORDER_AAC_MIC1 =177,
    CAMCORDER_PCM_MIC2 =178,
    CAMCORDER_AMR_NB_MIC2 =179,
    CAMCORDER_AAC_MIC2 =180,
    CAMCORDER_PCM_HSMIC =181,
    CAMCORDER_AMR_NB_HSMIC =182,
    CAMCORDER_AAC_HSMIC =183,
    ASR_PCM_MIC1 =184,
    ASR_AMR_NB_MIC1 =185,
    ASR_AAC_MIC1 =186,
    ASR_PCM_MIC2 =187,
    ASR_AMR_NB_MIC2 =188,
    ASR_AAC_MIC2 =189,
    ASR_PCM_HSMIC =190,
    ASR_AMR_NB_HSMIC =191,
    ASR_AAC_HSMIC =192,
    VOIP_SPK =193,
    VOIP_CALL_HS =194,
    VOIP_RECORDER_PCM_MIC1 =195,
    VOIP_RECORDER_AMR_NB_MIC1 =196,
    VOIP_RECORDER_AAC_MIC1 =197,
    VOIP_RECORDER_PCM_MIC2 =198,
    VOIP_RECORDER_AMR_NB_MIC2 =199,
    VOIP_RECORDER_AAC_MIC2 =200,
    VOIP_RECORDER_PCM_HSMIC =201,
    VOIP_RECORDER_AMR_NB_HSMIC =202,
    VOIP_RECORDER_AAC_HSMIC =203,
    VOICE_CALL_EPC =204,
    VOIP_EPC =205,
    ORGANPATHNUMB
} ORGAN_PATH;

#endif


