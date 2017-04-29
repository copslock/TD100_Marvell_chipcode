/*
 * (C) Copyright 2010 Marvell International Ltd.
 * All Rights Reserved
 */
#include <arch/linux-arm/AndroidConfig.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#define HAVE_SYS_UIO_H
#define LOG_TAG "AudioHardware"
#include <utils/Log.h>
#include <utils/String8.h>
#include <media/AudioSystem.h>
#include <hardware_legacy/power.h>

#include "AudioSetting.h"
#include "AudioHardware.h"
#include "AudioPath.h"
#ifdef WITH_HIFIEQ
#include "HifiEqCalibration.h"
#include "IPPEqWrapper.h"
#endif

namespace android {

#ifdef USE_PROPERTY
const char * get_audiopath_name_for_device(uint32_t devices)
{
	if(devices & DEVICE_OUT_EARPIECE)
		return "Earpiece";
	if(devices & DEVICE_OUT_SPEAKER)
		return "Speaker";
	if(devices & DEVICE_OUT_WIRED_HEADSET)
		return "Headset";

	return NULL;
}
#endif

#ifndef USE_PROPERTY

#define DOMAINSOCK_PATH "/data/misc/ippvp/ippvp_plugin"

struct ippvp_data
{
	uint32_t type;
	uint32_t data;
};

int domainsocket_send(uint32_t device)
{
	int fd, len;
	struct sockaddr_un un;
	struct ippvp_data id;
	int retry_count = 0;

	/* create a domain stream socket */
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		return -1;

	/* fill socket address with server's address */
	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	sprintf(un.sun_path, "%s", DOMAINSOCK_PATH);
	len = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);

	/* try to connect to ippvp server */
	while(connect(fd, (struct sockaddr *)&un, len) < 0)
	{
		retry_count ++;

		if(retry_count > 5) {
			LOGI("can not connect to server \n");
			close(fd);
			return -1;
		}

		usleep(10000);
	}

	id.type = 0x0;
	id.data = device;

	if(send(fd, (unsigned char *) &id, sizeof(struct ippvp_data), 0) != sizeof(struct ippvp_data))
	{
		LOGI("can not send\n");
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}
#endif

//#define STREAM_DUMP_DEBUG

#ifdef STREAM_DUMP_DEBUG
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#define RECORDER_PCM_LOC "/data/record_debug.pcm"
#define PLAYBACK_PCM_LOC "/data/playback_debug.pcm"

static int gRecorderFd  = 0;
static int gPlaybackFd  = 0;
static int gPlaybackLen = 0;

static void Pcm_Data_Open(int *pfd, bool mIsPlayback)
{
	LOGE("Pcm_Data_Open_File");

	*pfd = (mIsPlayback) ? (open(PLAYBACK_PCM_LOC, O_WRONLY | O_CREAT | O_TRUNC,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) : (open(RECORDER_PCM_LOC, O_WRONLY | O_CREAT | O_TRUNC,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));

	if (*pfd < 0)
	{
		LOGE("Cannot open output file");
		return;
	}
}

static void Pcm_Data_Write(int pfd, const void* buffer, size_t bytes)
{
	if (pfd != 0)
	{
		write(pfd , buffer, bytes);
	}
}

static void Pcm_Data_Close(int *pfd)
{
	LOGE("Pcm_Data_Close_File");
	close(*pfd);
	*pfd = 0;
}
#endif

static uint32_t getChannelCount(uint32_t channels)
{
    /* According to AudioSystem's channel definition. */
    uint32_t cur = channels;
    uint32_t ret = 0;
    while(cur > 0)
    {
        if(cur & 1)
        {
            ret++;
        }
        cur >>= 1;
    }
    return ret;
}

// ----------------------------------------------------------------------------

AudioHardwareMarvell::AudioHardwareMarvell() : mMicMute(false), mOutput(0), mPhoneOutput(0), mHdmiOutput(0), mInput(0), mVoiceInput(0), mVTInput(0)
{
    sem_init(&mLock, 0, 1);
    mMode = AudioSystem::MODE_INVALID;
    mVoiceVolume = 0.9f;
#ifdef WITH_LOOPBACK
    mLoopbackDevice = 0;
#endif
#ifdef WITH_NREC
    mNrec_on = false;
#endif
#ifdef WITH_HIFIEQ
	/* register HIFI EQ monitor */
	const String8 hifieq_filename = String8(AUDIO_EQ_HIFI_TABLE_FILENAME);
	HifiEqCalibrationCallback * callback = new HifiEqCalibrationCallback(hifieq_filename);
	FileMonitorSystem::registerCallback(callback);
#endif
}

AudioHardwareMarvell::~AudioHardwareMarvell()
{
    if(mOutput)
        delete mOutput;
    if(mPhoneOutput)
        delete mPhoneOutput;
    if(mHdmiOutput)
        delete mHdmiOutput;
    size_t size = mInputs.size();
    for (size_t i = 0; i < size; i++) {
        delete mInputs.itemAt(i);
    }
    mInputs.clear();
    sem_destroy(&mLock);
}

#ifdef RECORD_OVER_PCM
status_t AudioHardwareMarvell::closeInputsmHandle(){
    size_t size = mInputs.size();
    for (size_t i = 0; i < size; i++) {
        // Close mHandle once it is hold by AP, in the case for voice recording
        // and MVT recording, the GSSP is hold by CP, so we mustn't release.
        if (mInputs.itemAt(i)->getmDevice() != AudioSystem::DEVICE_IN_VOICE_CALL
            && mInputs.itemAt(i)->getmDevice() != AudioSystem::DEVICE_IN_VT_MIC){
            mInputs.itemAt(i)->closemHandle();
        }
    }
    return NO_ERROR;
}
#endif

status_t AudioHardwareMarvell::initCheck()
{
    LOGI("*************************AudioHardwareMarvell::initCheck***************************\n");
    return audio_init_check();
}

AudioStreamOut* AudioHardwareMarvell::openOutputStream(
        uint32_t devices, int *format, uint32_t *channels, uint32_t *sampleRate, status_t *status)
{
#ifdef STREAM_DUMP_DEBUG
	LOGE("try to open pcm file \n");
    if (gPlaybackFd == 0)
		Pcm_Data_Open(&gPlaybackFd, true);
#endif
	/*
		Two output streams are allowed: default and phone
	*/
	if ((devices == DEVICE_OUT_ALL) && (mPhoneOutput)) {
        // phone output already exist
        if(status) {
            *status = UNKNOWN_ERROR;
        }
        return NULL;
    } else if((devices == DEVICE_OUT_HDMI) && (mHdmiOutput)) {
        // hdmi output already exist
        if(status) {
            *status = UNKNOWN_ERROR;
        }
        return NULL;    
    } else if (((devices != DEVICE_OUT_ALL) && (devices != DEVICE_OUT_HDMI) ) && (mOutput)) {
        // default output already exist
        if(status) {
            *status = UNKNOWN_ERROR;
        }
        return NULL;    
    }
    LOGI("*************************AudioHardwareMarvell::openOutputStream(0x%x)***************************\n", devices);
    // create new output stream
    AudioStreamOutMarvell* out = NULL;
    if(devices == DEVICE_OUT_ALL)
        out = new AudioStreamOutMarvell(AudioSystem::PCM_16_BIT, AudioSystem::CHANNEL_OUT_STEREO, PHONE_OUTPUT_SAMPLE_RATE, PHONE_OUTPUT_BUFFER_TIME, PHONE_OUTPUT_PERIOD_TIME);
    else if( devices == DEVICE_OUT_HDMI ) {
        out = new AudioStreamOutMarvell(*format, *channels, *sampleRate, DEFAULT_OUTPUT_BUFFER_TIME, DEFAULT_OUTPUT_PERIOD_TIME);
    } else {
        out = new AudioStreamOutMarvell(AudioSystem::PCM_16_BIT, AudioSystem::CHANNEL_OUT_STEREO, DEFAULT_OUTPUT_SAMPLE_RATE, DEFAULT_OUTPUT_BUFFER_TIME, DEFAULT_OUTPUT_PERIOD_TIME);
    }
    if(out != NULL) {
        if( devices == DEVICE_OUT_HDMI ) {
            // Open hdmi output, we need switch default output from sspa1 to sspa2
            // default output should always exist
            uint32_t olddev = mOutput->getDevice();
            if( mOutput->setDevice(get_alsa_dev_name_for_device(AudioSystem::DEVICE_OUT_SPEAKER_SSPA2),AudioSystem::DEVICE_OUT_SPEAKER_SSPA2) != NO_ERROR ) {
                *status = UNKNOWN_ERROR;
                delete out;
                return NULL;
            }
            if( out->set(this, get_alsa_dev_name_for_device(devices), devices, format, channels, sampleRate ) != NO_ERROR ) {
                // restore default output, it should be always success
                mOutput->setDevice( get_alsa_dev_name_for_device(olddev), olddev);
                *status = UNKNOWN_ERROR;
                delete out;
                return NULL;                
            }
        } else if(devices != DEVICE_OUT_ALL) {
            if (out->set(this, get_alsa_dev_name_for_device(devices), devices, format, channels, sampleRate) != NO_ERROR) {
                *status = UNKNOWN_ERROR;
                delete out;
                return NULL;
            }
        } else {
            if (out->set(this, "plug:phone", devices, format, channels, sampleRate) != NO_ERROR) {
                *status = UNKNOWN_ERROR;
                delete out;
                return NULL;
            }
        }
    } else {
        LOGI("Can not allocate Audio Stream Out \n");
        return NULL;
    }

    if(devices == DEVICE_OUT_ALL)
        mPhoneOutput = out;
    else if( devices == DEVICE_OUT_HDMI)
        mHdmiOutput = out;
    else
        mOutput = out;

    if(status) {
        *status = OK;
    }
    return out;
}

void AudioHardwareMarvell::closeOutputStream(AudioStreamOut* out) {
#ifdef STREAM_DUMP_DEBUG
    if (gPlaybackFd != 0) {
		Pcm_Data_Close(&gPlaybackFd);
		gPlaybackFd = 0;
	}
#endif

    if( mHdmiOutput && out == mHdmiOutput ) {
        delete mHdmiOutput;
        if( mOutput ) {
            uint32_t dev = mOutput->getPrevDevice();
            mOutput->setDevice(get_alsa_dev_name_for_device(dev), dev);
        }
        mHdmiOutput = 0;
    }

    if (mOutput && out == mOutput) {
        delete mOutput;
        mOutput = 0;
    }

	if (mPhoneOutput && out == mPhoneOutput) {
        delete mPhoneOutput;
        mPhoneOutput = 0;
    }

}

AudioStreamIn* AudioHardwareMarvell::openInputStream(
        uint32_t devices, int *format, uint32_t *channels, uint32_t *sampleRate,
        status_t *status, AudioSystem::audio_in_acoustics acoustics)
{
#ifdef STREAM_DUMP_DEBUG
	LOGE("try to open recorder pcm file \n");
    if (gRecorderFd == 0)
		Pcm_Data_Open(&gRecorderFd, false);
#endif

    LOGI("*************************AudioHardwareMarvell::openInputStream 0x%x***************************\n", devices);
    // create new input stream
    AudioStreamInMarvell* in = new AudioStreamInMarvell(this, devices, format, channels, sampleRate, acoustics);
    mInputs.add(in);

    if(status) {
        *status = OK;
    }

    return in;
}

void AudioHardwareMarvell::closeInputStream(AudioStreamIn* in) {
#ifdef STREAM_DUMP_DEBUG
    if (gRecorderFd != 0) {
		Pcm_Data_Close(&gRecorderFd);
		gRecorderFd = 0;
	}
#endif
    size_t size = mInputs.size();
    for (size_t i = 0; i < size; i++) {
	if (mInputs.itemAt(i) == in) {
            delete mInputs.itemAt(i);
            mInputs.removeAt(i);
	    break;
	}
    }
}
#ifdef BOARD_USES_SANREMO
static int      shadowFmFreq = 0;
#endif
static int gFmDeviceActive =0;

status_t AudioHardwareMarvell::setParameters(const String8& keyValuePairs)
{
    AudioParameter param = AudioParameter(keyValuePairs);
    String8 value;
    String8 key;
    int intvalue;
    const char BT_NREC_KEY[] = "bt_headset_nrec";
    const char BT_NAME_KEY[] = "bt_headset_name";
    const char BT_NREC_VALUE_ON[] = "on";
#ifdef WITH_LOOPBACK
    const char LoopBack[] = "Loopback";
#endif
#ifdef BOARD_USES_SANREMO
    int fmFreq;
#endif
    LOGI("AudioHardwareMarvell::setParameters() %s", keyValuePairs.string());

    if (keyValuePairs.length() == 0) return BAD_VALUE;

    key = String8(BT_NREC_KEY);
    if (param.get(key, value) == NO_ERROR) {
        if (value == BT_NREC_VALUE_ON) {
#ifdef WITH_NREC
	        mNrec_on = false;//The bt_headset does not support NREC.
                LOGI("Open board's NREC.The bt_headset does not support NREC!");
#endif
        } else {
#ifdef WITH_NREC
                mNrec_on = true;//The bt_headset supports NREC
                LOGI("Close board's NREC.The bt_headset supports NREC!");
#endif
        }
    }
    key = String8(BT_NAME_KEY);
    if (param.get(key, value) == NO_ERROR) {
    }
    
	if (param.getInt(String8(AudioParameter::keyRouting),intvalue) == NO_ERROR) {
		LOGI("All Routing operation should be synced with Mixer thread\n");
	}

#ifdef WITH_LOOPBACK
	//loopback
	key = String8(LoopBack);
	if (param.getInt(key, intvalue) == NO_ERROR) {
		LOGI("loopback %d \n", intvalue);
		switch(intvalue) {//disable
			case 0: {
					disable_loopback(mLoopbackDevice);
					mLoopbackDevice = 0;
					break;
				}
			case AudioSystem::DEVICE_OUT_EARPIECE: { //earpiece
					enable_loopback(AudioSystem::DEVICE_OUT_EARPIECE);
					mLoopbackDevice = AudioSystem::DEVICE_OUT_EARPIECE;
					break;
				}
			case AudioSystem::DEVICE_OUT_SPEAKER: { //speaker
					enable_loopback(AudioSystem::DEVICE_OUT_SPEAKER);
					mLoopbackDevice = AudioSystem::DEVICE_OUT_SPEAKER;
					break;
				}
			case AudioSystem::DEVICE_OUT_WIRED_HEADSET: { //headset
					enable_loopback(AudioSystem::DEVICE_OUT_WIRED_HEADSET);
					mLoopbackDevice = AudioSystem::DEVICE_OUT_WIRED_HEADSET;
					break;
				}
			default:
				LOGE("unknow loopback device\n");
		}
	}
#endif

#ifdef BOARD_USES_SANREMO
    if (param.getInt(String8("FM_CHANNEL"), fmFreq) == NO_ERROR)
    {
        if(shadowFmFreq != fmFreq)
        {
            shadowFmFreq = fmFreq;
            if(gFmDeviceActive == AudioSystem::DEVICE_OUT_FM_HEADPHONE){
                set_fm_frequency(fmFreq);
            }
        }
    }
#endif
    return NO_ERROR;
}

status_t AudioHardwareMarvell::setVoiceVolume(float volume)
{
    mVoiceVolume = volume;
	mOutput->setVolume(volume,volume);
    return NO_ERROR;
}

status_t AudioHardwareMarvell::setMasterVolume(float volume)
{
    // return error - software mixer will handle it
    return INVALID_OPERATION;
}

status_t AudioHardwareMarvell::setMicMute(bool state) 
{
    LOGI("*************************AudioHardwareMarvell::setMicMute(%d)***************************\n",state); 
    mMicMute = state;  
    set_mic_mute(mMicMute ? 1 : 0);
    return  NO_ERROR; 
}

status_t AudioHardwareMarvell::getMicMute(bool* state) 
{ 
    *state = mMicMute ; 
    return NO_ERROR; 
}

#ifdef WITH_NREC
bool AudioHardwareMarvell::getNrecOn()
{
    return mNrec_on;
}
#endif

status_t AudioHardwareMarvell::dumpInternals(int fd, const Vector<String16>& args)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;
    result.append("AudioHardwareMarvell::dumpInternals\n");
    snprintf(buffer, SIZE, "\tmMicMute: %s\n", mMicMute? "true": "false");
    result.append(buffer);
    ::write(fd, result.string(), result.size());
    return NO_ERROR;
}

status_t AudioHardwareMarvell::dump(int fd, const Vector<String16>& args)
{
    dumpInternals(fd, args);
    return NO_ERROR;
}

// ----------------------------------------------------------------------------
/*
 *   Underrun and suspend recovery
 */
static int xrun_recovery (snd_pcm_t * handle, int err)
{
    if (!handle){
        return err;
    }
    if (err == -ESTRPIPE) {
        while ((err = snd_pcm_resume (handle)) == -EAGAIN)
            usleep (100);           /* wait until the suspend flag is released */

        if (err < 0) {
            err = snd_pcm_prepare (handle);
            if (err < 0) {
                LOGE("Can't recovery from suspend, prepare failed: %s", snd_strerror (err));
                return err;
            }
        }
    } else {
        err = snd_pcm_prepare (handle);
        if (err < 0) {
            LOGE("Can't recovery from underrun, prepare failed: %s", snd_strerror (err));
            return err;
        }
    }

    return 0;
}

// ----------------------------------------------------------------------------

AudioStreamOutMarvell::AudioStreamOutMarvell(int format, int channel, int rate, unsigned int buffertime, unsigned int periodtime)
{
    sem_init(&mLock, 0, 1);
    mFormat = format;
    mSampleRate = rate;
    mChannel = channel;
    mChannelCount = getChannelCount(mChannel);
    mBufferTime = buffertime;
    mPeriodTime = periodtime;
    mBufferSize = (mBufferTime * mSampleRate) / 1000000;
    mPeriodSize = (mPeriodTime * mSampleRate) / 1000000;
    mAudioHardware = NULL;
    mHandle = NULL;
    mDevice = 0;
}

AudioStreamOutMarvell::~AudioStreamOutMarvell()
{
	if(mHandle) {
        snd_pcm_close(mHandle);
        mHandle = NULL;

		mAudioHardware->getLock();
		if(! mAudioHardware->isPhoneOutput(this))
			disable_audio_path_for_device(mDevice);
		mAudioHardware->releaseLock();
    }

    sem_destroy(&mLock);
}
status_t AudioStreamOutMarvell::setDevice(const char* plugName, uint32_t devices){
    LOGD("setDevice %s, [0x%x]", plugName, devices);
    if( devices == mDevice )
        return NO_ERROR;
    
    // switch output device
    sem_wait(&mLock);
    
    unsigned int requestedBufferTime = mBufferTime;
    unsigned int requestedPeriodTime = mPeriodTime;
    unsigned int requestSampleRate = mSampleRate;

    int ret = 0;
    snd_pcm_t *tmpHandle = NULL;
    ret = snd_pcm_open(&tmpHandle, plugName, SND_PCM_STREAM_PLAYBACK, 0);
    if (ret < 0) {
        LOGE("*******************setDevice : snd_pcm_open %s failed*******************************", plugName);
        sem_post(&mLock);    
        return BAD_VALUE;
    }
    snd_pcm_hw_params_t *params = NULL;
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(tmpHandle, params);

    /* Set the desired hardware parameters. */
    snd_pcm_hw_params_set_access(tmpHandle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(tmpHandle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(tmpHandle, params, mChannelCount);
    snd_pcm_hw_params_set_rate_near(tmpHandle, params, &requestSampleRate, NULL);
    
    if( snd_pcm_hw_params_set_period_time_near(tmpHandle,params, &requestedPeriodTime, 0)<0 ){
        LOGW("Unable to set period time to %u usec", mPeriodTime);
    }
    if( requestedPeriodTime != mPeriodTime ) {
        LOGE("!!! setDevice : period time eventually set as %d usec, should be %d usec. Pay attention, check with BSP!!!!", requestedPeriodTime, mPeriodSize);
    }

    if( snd_pcm_hw_params_set_buffer_time_near(tmpHandle, params, &requestedBufferTime, 0)<0 ){
        LOGW("Unable to set buffer time to %d usec", mBufferTime);
    }
    if( requestedBufferTime != mBufferTime ) {
        LOGE("setDevice : buffer time eventually set as %d usec, should be %d usec, Pay attention, check with BSP!!!", requestedBufferTime, mBufferTime);
    }

    assert((requestedBufferTime == mBufferTime) && (requestedPeriodTime == mPeriodTime));
    
    /* Write the parameters to the driver */
    ret = snd_pcm_hw_params(tmpHandle, params);
    if (ret < 0) {
        LOGE("************setDevice : unable to set hw parameters: %s**********************\n", snd_strerror(ret));
        snd_pcm_close(tmpHandle);
        sem_post(&mLock);
        return BAD_VALUE;
    }

    /* soft params */
    snd_pcm_sw_params_t *sw_params = NULL;
    snd_pcm_sw_params_alloca(&sw_params);

    // Get the current software parameters
    if(snd_pcm_sw_params_current(tmpHandle, sw_params)<0){
        LOGE("setDevice : failed to fetch alsa current sw params");
        snd_pcm_close(tmpHandle);
        sem_post(&mLock);
        return BAD_VALUE;
    }
    
    snd_pcm_uframes_t bufferSize = 0;
    snd_pcm_uframes_t periodSize = 0;
    snd_pcm_uframes_t startThreshold;

    snd_pcm_get_params(tmpHandle, &bufferSize, &periodSize);

    LOGI("bufferSize is %d, periodSize is %d", bufferSize, periodSize);
    mBufferSize = bufferSize;
    mPeriodSize = periodSize;

    startThreshold = bufferSize - periodSize;

    if( snd_pcm_sw_params_set_start_threshold(tmpHandle, sw_params, startThreshold)<0 ){
        LOGW("setDevice : Unable to set start threshold to %lu frames", startThreshold);
    }

    // Stop the transfer when the buffer is full.
    if( snd_pcm_sw_params_set_stop_threshold(tmpHandle, sw_params, bufferSize)<0 ){
        LOGW("setDevice : Unable to set stop threshold to %lu frames", bufferSize);
    }

    // Allow the transfer to start when at least periodSize samples can be
    // processed.
    if( snd_pcm_sw_params_set_avail_min(tmpHandle, sw_params, periodSize)<0 ){
        LOGW("setDevice : Unable to configure available minimum to %lu", periodSize);
    }

    if( snd_pcm_sw_params(tmpHandle, sw_params)<0 ){;
        LOGE("setDevice : Unable to configure software parameters");
        snd_pcm_close(tmpHandle);
        sem_post(&mLock);        
        return BAD_VALUE;
    }
    int err = snd_pcm_prepare (tmpHandle);
    if (err < 0) {
        LOGE("setDevice :  prepare failed: %s", snd_strerror (err));
        snd_pcm_close(tmpHandle);        
        sem_post(&mLock);
        return BAD_VALUE;
    }
    
    mPrevDevice = mDevice;
    mDevice = devices;
    mPeriodTime = requestedPeriodTime;
    mBufferTime = requestedBufferTime;
    if( snd_pcm_close(mHandle) != 0 ) {
        LOGE("close origin pcm handle failed on device 0x%x", mPrevDevice);
    }
    mHandle = tmpHandle;
    LOGD("setDevice mPreDevice = 0x%x", mPrevDevice);
    sem_post(&mLock);
    return NO_ERROR;
}


status_t AudioStreamOutMarvell::set(
        AudioHardwareMarvell *hw,
		const char * name,
        uint32_t devices,
        int *pFormat,
        uint32_t *pChannels,
        uint32_t *pRate)
{
    int lFormat = pFormat ? *pFormat : 0;
    uint32_t lChannels = pChannels ? *pChannels : 0;
    uint32_t lRate = pRate ? *pRate : 0;

    // fix up defaults
    if (lFormat == 0) lFormat = format();
    if (lChannels == 0) lChannels = channels();
    if (lRate == 0) lRate = sampleRate();

    // check values
    if ((lFormat != format()) ||
//            (lChannels != channels()) ||
            (lRate != sampleRate())) {
        if (pFormat) *pFormat = format();
//        if (pChannels) *pChannels = channels();
        if (pRate) *pRate = sampleRate();
        return BAD_VALUE;
    }

    LOGI("--------------AudioStreamOutMarvell::set(%d, %d, %d, devices = 0x%x)---------------------", lFormat, lChannels, lRate, devices);

	if(mHandle)
	{
		//close the current plugin
		snd_pcm_close(mHandle);
	}

    unsigned int requestedBufferTime = mBufferTime;
    unsigned int requestedPeriodTime = mPeriodTime;

	int ret = 0;

	ret = snd_pcm_open(&mHandle, name, SND_PCM_STREAM_PLAYBACK, 0);

    if (ret < 0) {
        LOGE("***********************snd_pcm_open failed*******************************\n");
        return BAD_VALUE;
    }

    mChannelCount = getChannelCount(lChannels);

    snd_pcm_hw_params_t *params = NULL;
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(mHandle, params);

    /* Set the desired hardware parameters. */
    snd_pcm_hw_params_set_access(mHandle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(mHandle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(mHandle, params, mChannelCount);
    snd_pcm_hw_params_set_rate_near(mHandle, params, &mSampleRate, NULL);
    
    //assert(mSampleRate == DEFAULT_OUTPUT_SAMPLE_RATE);

	/*
    if( snd_pcm_hw_params_set_rate_resample(mHandle, params, 0)<0 ){
        LOGW("unable to disable hw resampling");
    }
	*/
    if( snd_pcm_hw_params_set_period_time_near(mHandle,params, &mPeriodTime, 0)<0 ){
        LOGW("Unable to set period time to %u usec", requestedPeriodTime);
    }
    LOGI("period time eventually set as %d usec", mPeriodTime);

    if( snd_pcm_hw_params_set_buffer_time_near(mHandle, params, &mBufferTime, 0)<0 ){
        LOGW("Unable to set buffer time to %d usec", requestedBufferTime);
    }
    LOGI("buffer time eventually set as %d usec", mBufferTime);

    assert((requestedBufferTime == mBufferTime) && (requestedPeriodTime == mPeriodTime));

    /* Write the parameters to the driver */
    ret = snd_pcm_hw_params(mHandle, params);
    if (ret < 0) {
        LOGE("***************unable to set hw parameters: %s**********************\n", snd_strerror(ret));
        return BAD_VALUE;
    }

    /* soft params */
    snd_pcm_sw_params_t *sw_params = NULL;
    snd_pcm_sw_params_alloca(&sw_params);

    // Get the current software parameters
    if(snd_pcm_sw_params_current(mHandle, sw_params)<0){
        LOGE("failed to fetch alsa current sw params");
        return BAD_VALUE;
    }
    
    snd_pcm_uframes_t bufferSize = 0;
    snd_pcm_uframes_t periodSize = 0;
    snd_pcm_uframes_t startThreshold;

    snd_pcm_get_params(mHandle, &bufferSize, &periodSize);

    LOGI("bufferSize is %lu, periodSize is %lu", bufferSize, periodSize);
    mBufferSize = bufferSize;
    mPeriodSize = periodSize;

    startThreshold = bufferSize - periodSize;

    if( snd_pcm_sw_params_set_start_threshold(mHandle, sw_params, startThreshold)<0 ){
        LOGW("Unable to set start threshold to %lu frames", startThreshold);
    }

    // Stop the transfer when the buffer is full.
    if( snd_pcm_sw_params_set_stop_threshold(mHandle, sw_params, bufferSize)<0 ){
        LOGW("Unable to set stop threshold to %lu frames", bufferSize);
    }
   
    // Allow the transfer to start when at least periodSize samples can be
    // processed.
    if( snd_pcm_sw_params_set_avail_min(mHandle, sw_params, periodSize)<0 ){
        LOGW("Unable to configure available minimum to %lu", periodSize);
    }

    if( snd_pcm_sw_params(mHandle, sw_params)<0 ){;
        LOGE("Unable to configure software parameters");
        return BAD_VALUE;
    }

    if (pFormat) *pFormat = lFormat;
    if (pChannels) *pChannels = lChannels;
    if (pRate) *pRate = lRate;

    mAudioHardware = hw;
    mDevice = devices;
    return NO_ERROR;
}

status_t AudioStreamOutMarvell::standby()
{
	mAudioHardware->getLock();

    if( !mAudioHardware->isPhoneOutput(this) || isStandby() ) {
                set_Standby();
    }
   /*
      drop remaining frames before closing codec
      clock to make sure trigger stop is invoked
    */
    if (mHandle) {
        snd_pcm_drop(mHandle);
    }

	mAudioHardware->releaseLock();

#ifdef WITH_HIFIEQ
    EqReset();
#endif

    return NO_ERROR;
}

status_t AudioStreamOutMarvell::setVolume(float left, float right)
{
    unsigned char nl = left * 255;
    unsigned char nr = right * 255;

	if(mAudioHardware->isPhoneOutput(this))
		return NO_ERROR;

    set_volume_for_device(mDevice, nl, nr);
    return NO_ERROR;
}

status_t AudioStreamOutMarvell::setParameters(const String8& keyValuePairs)
{
    AudioParameter param = AudioParameter(keyValuePairs);
    String8 key = String8(AudioParameter::keyRouting);
    status_t status = NO_ERROR;
    int device;
    int phone_state;
    int phone_volume;
    int samplerate = 0;
    int fmStatus = 0;
    int hdmiStatus = 0;
    int fm_volume;
    int streamActive = 0;
    bool nrec_on = false;
    int phone_plugin_active = 0;
    int close_inputs = 0;

    LOGI("AudioStreamOutMarvell::setParameters() %s", keyValuePairs.string());

#ifdef RECORD_OVER_PCM
    if (param.getInt(String8(CLOSE_INPUTS), close_inputs) == NO_ERROR){
        LOGI("AudioStreamOutMarvell::setParameters() closeAllInputsHandle");
        /* Due to fact, AP and CP share GSSP to access codec,
         * release AP GSSP once in call */
        mAudioHardware->closeInputsmHandle();
    }
#endif

    if(mAudioHardware->isPhoneOutput(this))
    {
        if (param.getInt(String8(PHONE_PLUGIN_ACTIVE), phone_plugin_active) == NO_ERROR) {
            if(phone_plugin_active) {
                if(mHandle == NULL)
                    set(mAudioHardware, "plug:phone", 0, 0, 0, 0);
            } else {
                if (param.getInt(String8(STREAM_ACTIVE), streamActive) == NO_ERROR) {
                    if(!streamActive && mHandle != NULL) {
                        snd_pcm_close(mHandle);
                        mHandle = NULL;
                    }

					param.remove(String8(STREAM_ACTIVE));
                }
            }

            param.remove(String8(PHONE_PLUGIN_ACTIVE));
        }
        return NO_ERROR;
    }
    mAudioHardware->getLock();
    if (param.getInt(String8(FM_STATE), fmStatus) == NO_ERROR){
        switch(fmStatus)
        {
        case 0 /* Disable */:
            if (!gFmDeviceActive){
                break;
            }
            disable_audio_path_for_device(gFmDeviceActive);
            gFmDeviceActive = 0;
            break;

        case 1 /* Enable / Set Headset */:
            if (gFmDeviceActive == DEVICE_OUT_FM_HEADPHONE){
                break;
            }
            if (param.getInt(String8(FM_VOLUME), fm_volume) == NO_ERROR){
                enable_audio_path_for_device(DEVICE_OUT_FM_HEADPHONE, fm_volume, fm_volume);
#ifdef BOARD_USES_SANREMO
                set_fm_frequency(shadowFmFreq);
#endif
            }
            gFmDeviceActive = DEVICE_OUT_FM_HEADPHONE;
            break;

        case 2 /* Enable / Set Loud-Speaker */:
            if (gFmDeviceActive == DEVICE_OUT_FM_SPEAKER){
                break;
            }
            if (param.getInt(String8(FM_VOLUME), fm_volume) == NO_ERROR){
                enable_audio_path_for_device(DEVICE_OUT_FM_SPEAKER, fm_volume, fm_volume);
            }
            gFmDeviceActive = DEVICE_OUT_FM_SPEAKER;
            break;
        case 3 /* Volume / Set FM Volume */:
            if (param.getInt(String8(FM_VOLUME), fm_volume) == NO_ERROR){
                if (gFmDeviceActive){
                    set_volume_for_device(gFmDeviceActive, fm_volume, fm_volume);
                }
            }
            break;
        }
        param.remove(String8(FM_STATE));
        param.remove(String8(FM_VOLUME));
    }

    if (param.getInt(key, device) == NO_ERROR) {
		if (param.getInt(String8(PHONE_STATE), phone_state) == NO_ERROR) {
			if (param.getInt(String8(PHONE_VOLUME), phone_volume) == NO_ERROR) {
				/*
				   if VT call, Switch plugin and enable EC path
				 */
				if (param.getInt(String8(AudioParameter::keySamplingRate), samplerate) == NO_ERROR) {
					if(samplerate == DEFAULT_OUTPUT_SAMPLE_RATE) {
#ifdef WITH_SRSWITCH
						/*
						   enable 44.1k sample rate
						 */
						enable_44k1_sample();
#endif
						mFormat = AudioSystem::PCM_16_BIT;
						mSampleRate = DEFAULT_OUTPUT_SAMPLE_RATE;
						mChannel = AudioSystem::CHANNEL_OUT_STEREO;
						mChannelCount = getChannelCount(mChannel);
						mBufferTime = DEFAULT_OUTPUT_BUFFER_TIME;
						mPeriodTime = DEFAULT_OUTPUT_PERIOD_TIME;
						mBufferSize = (mBufferTime * mSampleRate) / 1000000;
						mPeriodSize = (mPeriodTime * mSampleRate) / 1000000;

						set(mAudioHardware, get_alsa_dev_name_for_device(device), device, &mFormat, &mChannel, &mSampleRate);
					} else { //VT call
#ifdef WITH_SRSWITCH
						/*
						   enable 8k sample rate
						 */
						enable_8k_sample();
#endif
						mFormat = AudioSystem::PCM_16_BIT;
						mSampleRate = VT_OUTPUT_SAMPLE_RATE;
						mChannel = AudioSystem::CHANNEL_OUT_STEREO;
						mChannelCount = getChannelCount(mChannel);
						mBufferTime = VT_OUTPUT_BUFFER_TIME;
						mPeriodTime = VT_OUTPUT_PERIOD_TIME;
						mBufferSize = (mBufferTime * mSampleRate) / 1000000;
						mPeriodSize = (mPeriodTime * mSampleRate) / 1000000;

#ifdef WITH_VTOVERBT
						set(mAudioHardware, "plug:phone", device, &mFormat, &mChannel, &mSampleRate);
#else
						set(mAudioHardware, "ippvp", device, &mFormat, &mChannel, &mSampleRate);
#endif
					}

					param.remove(String8(AudioParameter::keySamplingRate));
				}

				if(mSampleRate == VT_OUTPUT_SAMPLE_RATE)
				{
#ifdef USE_PROPERTY
					if(property_set("debug.audiopath", get_audiopath_name_for_device(device)) < 0)
						LOGI("Can not set the property for device: %s \n", get_audiopath_name_for_device(device));
#else
					if(domainsocket_send(device) < 0)
						LOGI("Can not send -  device: 0x%x \n", device);
#endif
				}

#ifdef WITH_NREC
                nrec_on = mAudioHardware->getNrecOn();
                mDevice = set_side(phone_state, device, (unsigned char)phone_volume, (nrec_on==true)?1:0);
#else
                mDevice = set_side(phone_state, device, (unsigned char)phone_volume);
#endif
				param.remove(key);
				param.remove(String8(PHONE_STATE));
				param.remove(String8(PHONE_VOLUME));
			}
		}

#ifdef AUDIO_WITH_HDMI
        else if (param.getInt(String8(HDMI_STATE), hdmiStatus) == NO_ERROR) {
            set(mAudioHardware, get_alsa_dev_name_for_device(device), device, NULL, NULL, NULL);
            switch (hdmiStatus) {
            case 0:
#ifdef WITH_NREC
                nrec_on = mAudioHardware->getNrecOn();
                mDevice = set_side(MODE_NORMAL, device, 230, (nrec_on==true)?1:0);
#else
                mDevice = set_side(MODE_NORMAL, device, 230);
#endif
                break;
            case 1:
#ifdef WITH_NREC
                nrec_on = mAudioHardware->getNrecOn();
                mDevice = set_side(MODE_NORMAL, device, 230, (nrec_on==true)?1:0);
#else
                mDevice = set_side(MODE_NORMAL, device, 230);

#endif
                break;
            default:
                LOGE("Invalid HDMI status");
                break;
            }
        }
#endif
    }

#ifdef VOLUME_CALIBRATION
    int iVolumeForceSpeaker = 0;
    int iVolumeForceHeadset = 0;
    if (param.getInt(String8(VOLUME_FORCED_SPEAKER_STR), iVolumeForceSpeaker) == NO_ERROR &&
        param.getInt(String8(VOLUME_FORCED_HEADSET_STR), iVolumeForceHeadset) == NO_ERROR) {
        LOGD("Organ set iVolumeForceSpeaker=%d iVolumeForceHeadset=%d", iVolumeForceSpeaker, iVolumeForceHeadset);
        set_volume_for_device(mDevice, iVolumeForceSpeaker, iVolumeForceHeadset);
    }
#endif

    mAudioHardware->releaseLock();

    if (param.size()) {
        status = BAD_VALUE;
    }
    return status;
}

String8 AudioStreamOutMarvell::getParameters(const String8& keys)
{
    AudioParameter param = AudioParameter(keys);
    String8 value;
    String8 key = String8(AudioParameter::keyRouting);

	if(! mAudioHardware->isPhoneOutput(this)) {
		if (param.get(key, value) == NO_ERROR) {
			param.addInt(key, (int)mDevice);
		}
	}

    LOGV("getParameters() %s", param.toString().string());
    return param.toString();
}

ssize_t AudioStreamOutMarvell::write(const void* buffer, size_t bytes)
{
#ifdef STREAM_DUMP_DEBUG
    if (gPlaybackFd == 0)
		Pcm_Data_Open(&gPlaybackFd, true);

	if(gPlaybackFd != 0)
		Pcm_Data_Write(gPlaybackFd, buffer, bytes);
#endif
	snd_pcm_t * handle = mHandle;

    sem_wait(&mLock);

    if(!handle)
    {
        sem_post(&mLock);
        //LOGE("---------------handle is null---------------\n");
        return (ssize_t)bytes;
    }

    mAudioHardware->getLock();
	check_Standby();
    mAudioHardware->releaseLock();

#ifdef WITH_HIFIEQ
    if((mDevice | AudioSystem::DEVICE_OUT_SPEAKER) == mDevice)
	{
		EqTransformPacket((void*)buffer, bytes, mChannelCount);
	}
#endif

    int frames = bytes / sizeof(int16_t) / mChannelCount;
    int ret;
    int16_t *ptr = (int16_t*)buffer;

    while(frames > 0)
    {
        ret = snd_pcm_writei(handle, ptr, frames);
        if(ret < 0) {   
            LOGE("---------------write error is %s---------------\n", snd_strerror(ret)); 
            if (ret == -EAGAIN) {
                continue;
            } else if(xrun_recovery (handle, ret) < 0) {
                sem_post(&mLock);
                usleep(20000); //sleep 20ms
                return bytes;
            }
            continue;
        }
        ptr += ret * mChannelCount;
        frames -= ret;
    }
    sem_post(&mLock);

    return (ssize_t)bytes;
}

status_t AudioStreamOutMarvell::dump(int fd, const Vector<String16>& args)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;
    snprintf(buffer, SIZE, "AudioStreamOutMarvell::dump\n");
    snprintf(buffer, SIZE, "\tsample rate: %d\n", sampleRate());
    snprintf(buffer, SIZE, "\tbuffer size: %d\n", bufferSize());
    snprintf(buffer, SIZE, "\tchannel count: %d\n", channels());
    snprintf(buffer, SIZE, "\tformat: %d\n", format());
    result.append(buffer);
    ::write(fd, result.string(), result.size());
    return NO_ERROR; 
}

// ----------------------------------------------------------------------------

AudioStreamInMarvell::AudioStreamInMarvell() 
{
    sem_init(&mLock, 0, 1);
    mFormat = AudioSystem::PCM_16_BIT;
    mSampleRate = DEFAULT_INPUT_SAMPLE_RATE;
    mChannel = AudioSystem::CHANNEL_IN_MONO;
    mChannelCount = getChannelCount(mChannel);
    mBufferTime = DEFAULT_INPUT_BUFFER_TIME;
    mPeriodTime = DEFAULT_INPUT_PERIOD_TIME;
    mBufferSize = (mBufferTime * mSampleRate) / 1000000;
    mPeriodSize = (mPeriodTime * mSampleRate) / 1000000;
    mAudioHardware = NULL;
    mHandle = NULL;
    mDevice = 0;
}

AudioStreamInMarvell::AudioStreamInMarvell(
                        AudioHardwareMarvell *hw,
                        uint32_t devices,
                        int *pFormat,
                        uint32_t *pChannels,
                        uint32_t *pRate,
                        AudioSystem::audio_in_acoustics acoustics)
{
    sem_init(&mLock, 0, 1);
    mFormat = *pFormat;
    mSampleRate = *pRate;
    mChannel = *pChannels;
    mChannelCount = getChannelCount(mChannel);
    mBufferTime = DEFAULT_INPUT_BUFFER_TIME;
    mPeriodTime = DEFAULT_INPUT_PERIOD_TIME;
    mBufferSize = (mBufferTime * mSampleRate) / 1000000;
    mPeriodSize = (mPeriodTime * mSampleRate) / 1000000;
    mAudioHardware = hw;
    mAcoustics = acoustics;
    mHandle = NULL;
    mDevice = devices;
}

AudioStreamInMarvell::~AudioStreamInMarvell()
{
    if(mHandle) {
        snd_pcm_close(mHandle);
        mHandle = NULL;
		if((mDevice & DEVICE_IN_VT_MIC) == 0) {
			mAudioHardware->getLock();
#ifdef WITH_ECPATH
			if(mAudioHardware->getMode() == AudioSystem::MODE_IN_VT_CALL)
				disable_audio_ecpath_for_device(mDevice, 125);
			else
				disable_audio_path_for_device(mDevice);
#else
			disable_audio_path_for_device(mDevice);
#endif
			mAudioHardware->releaseLock();
		}
    }
    sem_destroy(&mLock);
}

status_t AudioStreamInMarvell::set(
        AudioHardwareMarvell *hw,
        uint32_t devices,
        int *pFormat,
        uint32_t *pChannels,
        uint32_t *pRate,
        AudioSystem::audio_in_acoustics acoustics)
{
    if (pFormat == 0 || pChannels == 0 || pRate == 0) return BAD_VALUE;

    int ret = -1;
    bool mic_mute_state = false;

	switch(hw->getMode()){
		case AudioSystem::MODE_IN_VT_CALL:
			mSampleRate = VT_INPUT_SAMPLE_RATE;
			mBufferTime = VT_INPUT_BUFFER_TIME;
			mPeriodTime = VT_INPUT_PERIOD_TIME;
			mBufferSize = (mBufferTime * mSampleRate) / 1000000;
			mPeriodSize = (mPeriodTime * mSampleRate) / 1000000;

			if((devices & DEVICE_IN_VT_MIC) != 0)
			{
				ret = snd_pcm_open(&mHandle, "vtrec", SND_PCM_STREAM_CAPTURE, 0);
				break;
			}
#ifdef WITH_VTOVERBT
			ret = snd_pcm_open(&mHandle, "plug:phone", SND_PCM_STREAM_CAPTURE, 0);
#else
			ret = snd_pcm_open(&mHandle, "ippvp", SND_PCM_STREAM_CAPTURE, 0);
#endif
			break;
		case AudioSystem::MODE_IN_CALL:
		default:
			if(devices & DEVICE_IN_VOICE_CALL)
				ret = snd_pcm_open(&mHandle, "plug:phone", SND_PCM_STREAM_CAPTURE, 0);
			else
				ret = snd_pcm_open(&mHandle, get_alsa_dev_name_for_device(devices), SND_PCM_STREAM_CAPTURE, 0);
			break;
	}

    if (ret < 0) {
        LOGE("***********************snd_pcm_open failed*******************************\n");
        return BAD_VALUE;
    }

    snd_pcm_hw_params_t *params;
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(mHandle, params);

    /* Set the desired hardware parameters. */
    snd_pcm_hw_params_set_access(mHandle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(mHandle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(mHandle, params, getChannelCount(*pChannels));
    snd_pcm_hw_params_set_rate_near(mHandle, params, pRate, NULL);

    unsigned int requestedBufferTime = mBufferTime;
    unsigned int requestedPeriodTime = mPeriodTime;
    unsigned int maxBufferTime;
    unsigned int minBufferTime;

    snd_pcm_hw_params_get_buffer_time_max(params, &maxBufferTime, 0);
    snd_pcm_hw_params_get_buffer_time_min(params, &minBufferTime, 0);
    LOGI("The buffer_time_max is %d, buffer_time_min is %d, requested is %d", maxBufferTime, minBufferTime, requestedBufferTime);
    assert((mBufferTime <= maxBufferTime) && (mBufferTime >= minBufferTime));

    if( snd_pcm_hw_params_set_period_time_near(mHandle, params, &mPeriodTime, 0)<0 ){
        LOGE("Unable to set period time to %u usec", mPeriodTime);
    }
    LOGI("period time eventually set as %d usec", mPeriodTime);

    if( snd_pcm_hw_params_set_buffer_time_near(mHandle, params, &mBufferTime, 0)<0 ){
        LOGE("Unable to set buffer time to %d usec", mBufferTime);
    }
    LOGI("buffer time eventually set as %d usec", mBufferTime);

    assert((requestedBufferTime == mBufferTime) && (requestedPeriodTime == mPeriodTime));

    /* Write the parameters to the driver */
    ret = snd_pcm_hw_params(mHandle, params);
    if (ret < 0) {
        LOGE("************************unable to set hw parameters: %s***********************\n", snd_strerror(ret));
        return BAD_VALUE;
    }

    mSampleRate = *pRate;
    mChannel = *pChannels;
    mChannelCount = getChannelCount(*pChannels);

    /* soft params */
    snd_pcm_sw_params_t *sw_params = NULL;
    snd_pcm_sw_params_alloca(&sw_params);

    // Get the current software parameters
    if(snd_pcm_sw_params_current(mHandle, sw_params)<0){
        LOGE("failed to fetch alsa current sw params");
        return BAD_VALUE;
    }

    snd_pcm_uframes_t bufferSize = 0;
    snd_pcm_uframes_t periodSize = 0;
    snd_pcm_uframes_t startThreshold;

    snd_pcm_get_params(mHandle, &bufferSize, &periodSize);

    assert((bufferSize == mBufferSize) && (periodSize == mPeriodSize));
    LOGI("bufferSize is %lu, periodSize is %lu", bufferSize, periodSize);
    mBufferSize = bufferSize;
    mPeriodSize = periodSize;

    // set startThreshold as 1 so that PCM can be recoverd whenever read fail.
    startThreshold = 1;

    if( snd_pcm_sw_params_set_start_threshold(mHandle, sw_params, startThreshold)<0 ){
        LOGW("Unable to set start threshold to %lu frames", startThreshold);
    }

    // Stop the transfer when the buffer is full.
    if( snd_pcm_sw_params_set_stop_threshold(mHandle, sw_params, bufferSize)<0 ){
        LOGW("Unable to set stop threshold to %lu frames", bufferSize);
    }

    // Allow the transfer to start when at least periodSize samples can be
    // processed.
    if( snd_pcm_sw_params_set_avail_min(mHandle, sw_params, periodSize)<0 ){
        LOGW("Unable to configure available minimum to %lu", periodSize);
    }

    if( snd_pcm_sw_params(mHandle, sw_params)<0 ){;
        LOGE("Unable to configure software parameters");
        return BAD_VALUE;
    }

    mAudioHardware = hw;
    mDevice = devices;

    mAudioHardware->getLock();

#ifdef RECORD_OVER_PCM
    /* Due to fact, AP and CP share GSSP to access codec, no path enabled during Voice or VT call */
if((hw->getMode() != AudioSystem::MODE_IN_VT_CALL) && (hw->getMode() != AudioSystem::MODE_IN_CALL))
{
#endif
#ifdef WITH_ECPATH
	if(hw->getMode() == AudioSystem::MODE_IN_VT_CALL)
		enable_audio_ecpath_for_device(devices, 125);
	else
		enable_audio_path_for_device(devices, 125, 125);
#else
	enable_audio_path_for_device(devices, 125, 125);
#endif
#ifdef RECORD_OVER_PCM
} else {
	LOGW("can not enable MIC path during VT or voice call \n");
}
#endif

    //check mic mute status
    mAudioHardware->getMicMute(&mic_mute_state);
    if (mic_mute_state) mAudioHardware->setMicMute(mic_mute_state);

    mAudioHardware->releaseLock();
    return NO_ERROR;
}

status_t AudioStreamInMarvell::setGain(float gain)
{
    unsigned char nv = gain * 255;
	/*
		VT_MIC can not modify Gain
	*/
	if((mDevice & DEVICE_IN_VT_MIC) == 0)
		set_volume_for_device(mDevice, nv, nv);
	else
		return BAD_VALUE;

    return NO_ERROR; 
}

status_t AudioStreamInMarvell::standby()
{
    // TODO: audio hardware to standby mode
    return NO_ERROR;
}

#ifdef RECORD_OVER_PCM
status_t AudioStreamInMarvell::closemHandle()
{
    sem_wait(&mLock);
    if (mHandle){
        snd_pcm_close(mHandle);
        mHandle = NULL;
    }
    sem_post(&mLock);
    return NO_ERROR;
}
#endif

status_t AudioStreamInMarvell::setParameters(const String8& keyValuePairs)
{
    AudioParameter param = AudioParameter(keyValuePairs);
    String8 key = String8(AudioParameter::keyRouting);
    status_t status = NO_ERROR;
    int device;
    bool mic_mute_state = false;
    LOGV("setParameters() %s", keyValuePairs.string());
    int active = 0;
    if (param.getInt(key, device) == NO_ERROR) {
	       if(param.getInt(String8(STREAM_IN_ACTIVE),active) == NO_ERROR){
	           if(active){
                       int ret = set(mAudioHardware,device, &mFormat, &mChannel, &mSampleRate, mAcoustics);
                       if(ret < 0){
		           LOGE("can not open the alsa plugin.Error code: %d \n",ret);
			   status = ret;
		       }
		   }
		   else if(mHandle){
		       snd_pcm_close(mHandle);
		       mHandle = NULL;
		   }
		   param.remove(String8(STREAM_IN_ACTIVE));
                }
		if((mDevice & DEVICE_IN_VT_MIC) == 0)
		{
			if((unsigned int)device != mDevice)
			{

                               mAudioHardware->getLock();
#ifdef WITH_ECPATH
				if(mAudioHardware->getMode() == AudioSystem::MODE_IN_VT_CALL)
					disable_audio_ecpath_for_device(mDevice, 125);
				else
					disable_audio_path_for_device(mDevice);
#else
				disable_audio_path_for_device(mDevice);
#endif
#ifdef RECORD_OVER_PCM
                if (device != 0){
    /* Due to fact, AP and CP share GSSP to access codec, no path enabled during Voice or VT call */
if((mAudioHardware->getMode() != AudioSystem::MODE_IN_VT_CALL) && (mAudioHardware->getMode() != AudioSystem::MODE_IN_CALL))
{
#endif
#ifdef WITH_ECPATH
				if(mAudioHardware->getMode() == AudioSystem::MODE_IN_VT_CALL)
					enable_audio_ecpath_for_device(device, 125);
				else
					enable_audio_path_for_device(device, 125, 125);
#else
					enable_audio_path_for_device(device, 125, 125);
#endif
#ifdef RECORD_OVER_PCM
} else {
	LOGW("can not enable MIC path during VT or voice call \n");
}
                }
#endif

                                //check mic mute status
                                mAudioHardware->getMicMute(&mic_mute_state);
                                if (mic_mute_state) mAudioHardware->setMicMute(mic_mute_state);

                                mAudioHardware->releaseLock();
			}
			mDevice = device;
		}
        param.remove(key);
    }

    if (param.size()) {
        status = BAD_VALUE;
    }
    return status;
}

String8 AudioStreamInMarvell::getParameters(const String8& keys)
{
    AudioParameter param = AudioParameter(keys);
    String8 value;
    String8 key = String8(AudioParameter::keyRouting);

    if (param.get(key, value) == NO_ERROR) {
        param.addInt(key, (int)mDevice);
    }

    LOGV("getParameters() %s", param.toString().string());
    return param.toString();
}

ssize_t AudioStreamInMarvell::read(void* buffer, ssize_t bytes)
{
    sem_wait(&mLock);
    if(!mHandle)
    {
        sem_post(&mLock);
        LOGE("---------------handle is null---------------\n");
        memset((unsigned char *)buffer, 0, bytes);
        usleep(20000); //sleep 20ms
        return (ssize_t)bytes;
    }

    int frames = bytes / sizeof(int16_t) / mChannelCount;
    int ret;
    int16_t *ptr = (int16_t*)buffer;

    while(frames > 0)
    {
        ret = snd_pcm_readi(mHandle, ptr, frames);    
        if(ret < 0) {   
            LOGE("---------------read error is %s---------------\n", snd_strerror(ret)); 
            if (ret == -EAGAIN) {
                continue;
            } else if (xrun_recovery (mHandle, ret) < 0) {
                sem_post(&mLock);
                memset((unsigned char *)buffer, 0, bytes);
                usleep(20000); //sleep 20ms
                return bytes;
            }
            continue;
        }
        ptr += ret * mChannelCount;
        frames -= ret;
    }
    sem_post(&mLock);

#ifdef STREAM_DUMP_DEBUG
    if (gRecorderFd == 0)
        Pcm_Data_Open(&gRecorderFd, false);

    if(gRecorderFd != 0)
        Pcm_Data_Write(gRecorderFd, buffer, bytes);
#endif

    return (ssize_t)bytes;
}

status_t AudioStreamInMarvell::dump(int fd, const Vector<String16>& args)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;
    snprintf(buffer, SIZE, "AudioStreamInMarvell::dump\n");
    result.append(buffer);
    snprintf(buffer, SIZE, "\tsample rate: %d\n", sampleRate());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tbuffer size: %d\n", bufferSize());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tchannel count: %d\n", channels());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tformat: %d\n", format());
    result.append(buffer);
    ::write(fd, result.string(), result.size());
    return NO_ERROR;
}

// default implementation
size_t AudioHardwareMarvell::getInputBufferSize(uint32_t sampleRate, int format, int channelCount)
{

//    if ( (sampleRate != 8000) && (sampleRate != 16000) ) {
//        LOGW("getInputBufferSize bad sampling rate: %d", sampleRate);
//        return 0;
//    }

    if (format != AudioSystem::PCM_16_BIT) {
        LOGW("getInputBufferSize bad format: %d", format);
        return 0;
    }

    return ((DEFAULT_INPUT_PERIOD_TIME * sampleRate) / 1000000) * sizeof(uint16_t);
}

// ----------------------------------------------------------------------------

extern "C" AudioHardwareInterface* createAudioHardware(void)
{
    return new AudioHardwareMarvell();
}

}; // namespace android
