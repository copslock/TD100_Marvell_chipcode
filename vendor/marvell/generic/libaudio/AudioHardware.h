/*
 * (C) Copyright 2010 Marvell International Ltd.
 * All Rights Reserved
 */

#ifndef ANDROID_AUDIO_HARDWARE_Marvell_H
#define ANDROID_AUDIO_HARDWARE_Marvell_H

#include <stdint.h>
#include <sys/types.h>
#include <semaphore.h>
#include <alsa/asoundlib.h>
#include <utils/Vector.h>
#include <hardware_legacy/AudioHardwareBase.h>


namespace android {

class AudioHardwareMarvell;

// ----------------------------------------------------------------------------

class AudioStreamOutMarvell : public AudioStreamOut {
public:
                        AudioStreamOutMarvell(int format, int channel, int rate, unsigned int buffertime, unsigned int periodtime);
    virtual             ~AudioStreamOutMarvell();

    status_t    set(
            AudioHardwareMarvell *hw,
			const char * plugName,
            uint32_t devices,
            int *pFormat,
            uint32_t *pChannels,
            uint32_t *pRate);
    status_t setDevice(const char* plugName, uint32_t devices);
    uint32_t getDevice(){return mDevice;}
    uint32_t getPrevDevice(){return mPrevDevice;}

    virtual uint32_t    sampleRate() const { return mSampleRate; }
    virtual size_t      bufferSize() const { return mPeriodSize * mChannelCount * sizeof(uint16_t); }
    virtual uint32_t    channels() const { return mChannel; }
    virtual int         format() const { return mFormat; }
#ifdef AUDIO_WITH_ABU
	/* 32k ABU is used */
    virtual uint32_t    latency() const { return (mBufferTime + mPeriodTime)/1000; }
#else
    virtual uint32_t    latency() const { return mBufferTime/1000; }
#endif
    virtual status_t    setVolume(float left, float right);
    virtual ssize_t     write(const void* buffer, size_t bytes);
    virtual status_t    dump(int fd, const Vector<String16>& args);
    virtual status_t    standby();
    virtual status_t    setParameters(const String8& keyValuePairs);
    virtual String8     getParameters(const String8& keys);
#if PLATFORM_SDK_VERSION >= 8
    // return the number of audio frames written by the audio dsp to DAC since
    // the output has exited standby
    virtual status_t    getRenderPosition(uint32_t *dspFrames) {*dspFrames = 0; return NO_ERROR;}
#endif

private:
    AudioHardwareMarvell  *mAudioHardware;
    int                    mFormat;
    int                    mChannelCount;
    uint32_t               mChannel;
    uint32_t               mSampleRate;
    sem_t                  mLock;
    snd_pcm_t             *mHandle;
    unsigned int           mBufferTime; /* macro second */
    unsigned int           mPeriodTime; /* macro second */
    unsigned int           mBufferSize; /* sample number */
    unsigned int           mPeriodSize; /* sample number */
    uint32_t               mDevice;
    // previous used device, we need to save it for switch between SSPA1/SSPA2 for default output
    uint32_t               mPrevDevice;

};

class AudioStreamInMarvell : public AudioStreamIn {
public:
                        AudioStreamInMarvell();
			AudioStreamInMarvell(
                            AudioHardwareMarvell *hw,
                            uint32_t devices,
                            int *pFormat,
                            uint32_t *pChannels,
                            uint32_t *pRate,
                            AudioSystem::audio_in_acoustics acoustics);
    virtual             ~AudioStreamInMarvell();

    status_t    set(
            AudioHardwareMarvell *hw,
            uint32_t devices,
            int *pFormat,
            uint32_t *pChannels,
            uint32_t *pRate,
            AudioSystem::audio_in_acoustics acoustics);

    virtual uint32_t    sampleRate() const { return mSampleRate; }
    virtual size_t      bufferSize() const { return mPeriodSize * mChannelCount * sizeof(uint16_t); }
    virtual uint32_t    channels() const { return mChannel; }
    virtual int         format() const { return mFormat; }
    virtual status_t    setGain(float gain);
    virtual ssize_t     read(void* buffer, ssize_t bytes);
    virtual status_t    dump(int fd, const Vector<String16>& args);
    virtual status_t    standby();
    virtual status_t    setParameters(const String8& keyValuePairs);
    virtual String8     getParameters(const String8& keys);
#ifdef RECORD_OVER_PCM
    virtual status_t    closemHandle();
    virtual int32_t     getmDevice() const { return mDevice; }
#endif
#if PLATFORM_SDK_VERSION >= 8
    // Return the amount of input frames lost in the audio driver since the last call of this function.
    // Audio driver is expected to reset the value to 0 and restart counting upon returning the current value by this function call.
    // Such loss typically occurs when the user space process is blocked longer than the capacity of audio driver buffers.
    // Unit: the number of input audio frames
    virtual unsigned int  getInputFramesLost() const {return 0;}
#endif

private:
    AudioHardwareMarvell  *mAudioHardware;
    int                    mFormat;
    int                    mChannelCount;
    uint32_t               mChannel;
    uint32_t               mSampleRate;
    sem_t              	   mLock;
    snd_pcm_t             *mHandle;
    uint32_t               mDevice;
    unsigned int           mBufferTime; /* macro second */
    unsigned int           mPeriodTime; /* macro second */
    unsigned int           mBufferSize; /* sample number */
    unsigned int           mPeriodSize; /* sample number */
    AudioSystem::audio_in_acoustics mAcoustics;
};

class AudioHardwareMarvell : public  AudioHardwareBase
{
public:
                        AudioHardwareMarvell();
    virtual             ~AudioHardwareMarvell();
    virtual status_t    initCheck();
    virtual status_t    setVoiceVolume(float volume);
    virtual status_t    setMasterVolume(float volume);

    // mic mute
    virtual status_t    setMicMute(bool state);
    virtual status_t    getMicMute(bool* state);
#ifdef WITH_NREC
    virtual bool        getNrecOn();
#endif
			int			getMode() const { return mMode; }

    virtual status_t    setParameters(const String8& keyValuePairs);

    virtual size_t 		getInputBufferSize(uint32_t sampleRate, int format, int channelCount);

    // create I/O streams
    virtual AudioStreamOut* openOutputStream(
				uint32_t devices,
                                int *format=0,
                                uint32_t *channelCount=0,
                                uint32_t *sampleRate=0,
                                status_t *status=0);
    virtual    void        closeOutputStream(AudioStreamOut* out);

    virtual AudioStreamIn* openInputStream(
				uint32_t devices,
                                int *format,
                                uint32_t *channels,
                                uint32_t *sampleRate,
                                status_t *status,
				AudioSystem::audio_in_acoustics);
    virtual    void        closeInputStream(AudioStreamIn* in);

    void                setVolume(int direction, float volume);
	void 				getLock() { sem_wait(&mLock); }
	void				releaseLock() { sem_post(&mLock); }
	bool				isPhoneOutput(AudioStreamOutMarvell * out) const { return (out == mPhoneOutput)? true:false; }
#ifdef RECORD_OVER_PCM
    virtual status_t    closeInputsmHandle();
#endif
protected:
    virtual status_t    dump(int fd, const Vector<String16>& args);

    bool                mMicMute;
private:
    status_t            dumpInternals(int fd, const Vector<String16>& args);

    AudioStreamOutMarvell   *mOutput;
    AudioStreamOutMarvell   *mPhoneOutput;
    AudioStreamOutMarvell   *mHdmiOutput;
    AudioStreamInMarvell    *mInput;
    AudioStreamInMarvell    *mVoiceInput; //record voice call
    AudioStreamInMarvell    *mVTInput; //record VT call
    sem_t                    mLock;
    float                    mVoiceVolume;
    Vector<AudioStreamInMarvell*> mInputs;
#ifdef WITH_NREC
    bool                     mNrec_on;
#endif
#ifdef WITH_LOOPBACK
    int                      mLoopbackDevice;
#endif
};

}; // namespace android

#endif // ANDROID_AUDIO_HARDWARE_Marvell_H
