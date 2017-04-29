/*
 * (C) Copyright 2010 Marvell International Ltd.
 * All Rights Reserved
 */
/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "AudioPolicyManager"
#define LOG_NDEBUG 0
#include <utils/Log.h>
#include "AudioPolicyManager.h"
#include <media/mediarecorder.h>

#include "AudioSetting.h"
#include "AudioPath.h"
#include <hardware_legacy/power.h>

namespace android {

// ----------------------------------------------------------------------------
// AudioPolicyInterface implementation
// ----------------------------------------------------------------------------
uint32_t AudioPolicyManager::mAvailableOutputDevices;
uint32_t AudioPolicyManager::mAvailableInputDevices;

#define ENABLE_FM(device, volInt)\
do{\
    int fm_status = (device == AudioSystem::DEVICE_OUT_FM_HEADPHONE) ? 1 : 2;\
    AudioParameter param = AudioParameter();\
    param.addInt(String8(FM_STATE), fm_status /* Enable */);\
    param.addInt(String8(FM_VOLUME), volInt /* volume num */);\
    mpClientInterface->setParameters(mHardwareOutput, param.toString());\
}while(0)

/* This will disable both FM_HEADPHONE and FM_SPEAKER */
#define DISABLE_FM()\
do{\
    AudioParameter param = AudioParameter();\
    param.addInt(String8(FM_STATE), 0 /* Disable */);\
    param.addInt(String8(FM_VOLUME), 0 /* Volume num */);\
    mpClientInterface->setParameters(mHardwareOutput, param.toString());\
}while(0)

#define SET_FM_VOLUME(volInt)\
do{\
    AudioParameter param = AudioParameter();\
    param.addInt(String8(FM_STATE), 3 /* Set Volume */);\
    param.addInt(String8(FM_VOLUME), volInt /* volume num */);\
    mpClientInterface->setParameters(mHardwareOutput, param.toString(), delayMs);\
}while(0)

status_t AudioPolicyManager::setDeviceConnectionState(AudioSystem::audio_devices device,
                                                  AudioSystem::device_connection_state state,
                                                  const char *device_address)
{
    LOGV("setDeviceConnectionState() device: %x, state %d, address %s", device, state, device_address);

    // connect/disconnect only 1 device at a time
    if (AudioSystem::popCount(device) != 1) return BAD_VALUE;

    if (strlen(device_address) >= MAX_DEVICE_ADDRESS_LEN) {
        LOGE("setDeviceConnectionState() invalid address: %s", device_address);
        return BAD_VALUE;
    }

    // handle output devices
    if (AudioSystem::isOutputDevice(device)) {	
#ifndef WITH_A2DP
        if (AudioSystem::isA2dpDevice(device)) {
            LOGE("setDeviceConnectionState() invalid device: %x", device);
            return BAD_VALUE;
        }
#endif

        switch (state)
        {
        // handle output device connection
        case AudioSystem::DEVICE_STATE_AVAILABLE:
            if (mAvailableOutputDevices & device) {
                LOGW("setDeviceConnectionState() device already connected: %x", device);
                return INVALID_OPERATION;
            }

            LOGV("setDeviceConnectionState() connecting device %x", device);

            // register new device as available
            mAvailableOutputDevices |= device;

            if((device == AudioSystem::DEVICE_OUT_FM_HEADPHONE) ||
            (device == AudioSystem::DEVICE_OUT_FM_SPEAKER))
            {
                if (mPhoneState == AudioSystem::MODE_IN_CALL || mPhoneState == AudioSystem::MODE_IN_VT_CALL ||
                    mOutputs.valueFor(mHardwareOutput)->isUsedByStrategy(STRATEGY_PHONE) ||
                    mOutputs.valueFor(mHardwareOutput)->isUsedByStrategy(STRATEGY_SONIFICATION) ||
                    mOutputs.valueFor(mHardwareOutput)->isUsedByStrategy(STRATEGY_MEDIA) ||
                    mOutputs.valueFor(mHardwareOutput)->isUsedByStrategy(STRATEGY_DTMF)) {
                    break;
                }
                LOGV("setDeviceConnectionState(); FM ENABLE");
                StreamDescriptor &streamDesc = mStreams[AudioSystem::FM];
                int volInt = (255 * (streamDesc.mIndexCur - streamDesc.mIndexMin))
                                    /(streamDesc.mIndexMax - streamDesc.mIndexMin);
                ENABLE_FM(device, volInt);
            }

#ifdef AUDIO_WITH_HDMI
            if(device == AudioSystem::DEVICE_OUT_HDMI)
            {
                LOGV("setDeviceConnectionState(); HDMI_ENABLE");
                AudioParameter param = AudioParameter();
                param.addInt(String8(AudioParameter::keyRouting), (int)device);
                param.addInt(String8(HDMI_STATE), 1);
                mpClientInterface->setParameters(mHardwareOutput, param.toString());
            }
#endif

#ifdef WITH_A2DP
            // handle A2DP device connection
            if (AudioSystem::isA2dpDevice(device)) {
                status_t status = handleA2dpConnection(device, device_address);
                if (status != NO_ERROR) {
                    mAvailableOutputDevices &= ~device;
                    return status;
                }
			}else
#endif
            // handle wired and SCO device connection (accessed via hardware output)
            {
                if (AudioSystem::isBluetoothScoDevice(device)) {
                    LOGV("setDeviceConnectionState() BT SCO  device, address %s", device_address);
                    // keep track of SCO device address
                    mScoDeviceAddress = String8(device_address, MAX_DEVICE_ADDRESS_LEN);
                }
                else if (device == AudioSystem::DEVICE_OUT_FM_TRANSMITTER) {
                    if (mPhoneState == AudioSystem::MODE_IN_CALL || mPhoneState == AudioSystem::MODE_IN_VT_CALL ||
                        mOutputs.valueFor(mHardwareOutput)->isUsedByStrategy(STRATEGY_PHONE) ||
                        mOutputs.valueFor(mHardwareOutput)->isUsedByStrategy(STRATEGY_SONIFICATION) ||
                        mOutputs.valueFor(mHardwareOutput)->isUsedByStrategy(STRATEGY_MEDIA) ||
                        mOutputs.valueFor(mHardwareOutput)->isUsedByStrategy(STRATEGY_DTMF)) {
                        break;
                    }
                }
            }
            break;
        // handle output device disconnection
        case AudioSystem::DEVICE_STATE_UNAVAILABLE: {
            if (!(mAvailableOutputDevices & device)) {
                LOGW("setDeviceConnectionState() device not connected: %x", device);
                return INVALID_OPERATION;
            }

            // remove device from available output devices
            mAvailableOutputDevices &= ~device;

            if((device == AudioSystem::DEVICE_OUT_FM_HEADPHONE) ||
            (device == AudioSystem::DEVICE_OUT_FM_SPEAKER))
            {
                LOGV("setDeviceConnectionState(); FM DISABLE");
                DISABLE_FM();

            }

#ifdef AUDIO_WITH_HDMI
            if(device == AudioSystem::DEVICE_OUT_HDMI)
            {
                LOGV("setDeviceConnectionState(); HDMI DISABLE");
                AudioParameter param = AudioParameter();
                param.addInt(String8(AudioParameter::keyRouting), (int)getDeviceForStrategy(STRATEGY_MEDIA));
                param.addInt(String8(HDMI_STATE), 0);
                mpClientInterface->setParameters(mHardwareOutput, param.toString());
            }
#endif

#ifdef WITH_A2DP
            // handle A2DP device disconnection
            if (AudioSystem::isA2dpDevice(device)) {
                status_t status = handleA2dpDisconnection(device, device_address);
                if (status != NO_ERROR) {
                    mAvailableOutputDevices |= device;
                    return status;
                }
			} else
#endif
            {
                if (AudioSystem::isBluetoothScoDevice(device)) {
                    mScoDeviceAddress = "";
                } else if (device == AudioSystem::DEVICE_OUT_FM_TRANSMITTER) {
                    if (mPhoneState == AudioSystem::MODE_IN_CALL || mPhoneState == AudioSystem::MODE_IN_VT_CALL ||
                        mOutputs.valueFor(mHardwareOutput)->isUsedByStrategy(STRATEGY_PHONE) ||
                        mOutputs.valueFor(mHardwareOutput)->isUsedByStrategy(STRATEGY_SONIFICATION) ||
                        mOutputs.valueFor(mHardwareOutput)->isUsedByStrategy(STRATEGY_DTMF)) {
                        break;
                    }
                }
            }

            // clear A2DP and SCO device address if necessary
#ifdef WITH_A2DP
            if (AudioSystem::isA2dpDevice(device)) {
                mA2dpDeviceAddress = "";
            }
#endif
            if (AudioSystem::isBluetoothScoDevice(device)) {
                mScoDeviceAddress = "";
            }
            } break;

        default:
            LOGE("setDeviceConnectionState() invalid state: %x", state);
            return BAD_VALUE;
        }

        // request routing change if necessary
        uint32_t newDevice = getNewDevice(mHardwareOutput, false);
#ifdef WITH_A2DP
        checkOutputForAllStrategies();
        // A2DP outputs must be closed after checkOutputForAllStrategies() is executed
        if (state == AudioSystem::DEVICE_STATE_UNAVAILABLE && AudioSystem::isA2dpDevice(device)) {
            closeA2dpOutputs();
        }
        checkA2dpSuspend();
#endif
        updateDeviceForStrategy();

#ifdef VOLUME_CALIBRATION
		if (device == AudioSystem::DEVICE_OUT_WIRED_HEADSET || device == AudioSystem::DEVICE_OUT_WIRED_HEADPHONE) {
			// notify AudioFlinger the audio device change before switching audio path
			AudioParameter param = AudioParameter();
			param.addInt(String8(DEVICE_CHANGED), newDevice);
			mpClientInterface->setParameters(mHardwareOutput, param.toString());
			// add 500ms delay to avoid noise in headset/headphone during volume change
			setOutputDevice(mHardwareOutput, newDevice, false, 500);
		} else {
#endif
			setOutputDevice(mHardwareOutput, newDevice);
#ifdef VOLUME_CALIBRATION
		}
#endif

        if (device == AudioSystem::DEVICE_OUT_WIRED_HEADSET) {
            device = AudioSystem::DEVICE_IN_WIRED_HEADSET;
        } else if (device == AudioSystem::DEVICE_OUT_BLUETOOTH_SCO ||
                   device == AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET ||
                   device == AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_CARKIT) {
            device = AudioSystem::DEVICE_IN_BLUETOOTH_SCO_HEADSET;
        } else {
            return NO_ERROR;
        }
    }
    // handle input devices
    if (AudioSystem::isInputDevice(device)) {

        switch (state)
        {
        // handle input device connection
        case AudioSystem::DEVICE_STATE_AVAILABLE: {
            if (mAvailableInputDevices & device) {
                LOGW("setDeviceConnectionState() device already connected: %d", device);
                return INVALID_OPERATION;
            }
            mAvailableInputDevices |= device;
            }
            break;

        // handle input device disconnection
        case AudioSystem::DEVICE_STATE_UNAVAILABLE: {
            if (!(mAvailableInputDevices & device)) {
                LOGW("setDeviceConnectionState() device not connected: %d", device);
                return INVALID_OPERATION;
            }
            mAvailableInputDevices &= ~device;
            } break;

        default:
            LOGE("setDeviceConnectionState() invalid state: %x", state);
            return BAD_VALUE;
        }

        audio_io_handle_t activeInput = getActiveInput();
        if (activeInput != 0) {
            AudioInputDescriptor *inputDesc = mInputs.valueFor(activeInput);
            uint32_t newDevice = getDeviceForInputSource(inputDesc->mInputSource);
            if (newDevice != inputDesc->mDevice) {
                LOGV("setDeviceConnectionState() changing device from %x to %x for input %d",
                        inputDesc->mDevice, newDevice, activeInput);
                inputDesc->mDevice = newDevice;
                AudioParameter param = AudioParameter();
                param.addInt(String8(AudioParameter::keyRouting), (int)newDevice);
                mpClientInterface->setParameters(activeInput, param.toString());
            }
        }

        return NO_ERROR;
    }

    LOGW("setDeviceConnectionState() invalid device: %x", device);
    return BAD_VALUE;
}

AudioSystem::device_connection_state AudioPolicyManager::getDeviceConnectionState(AudioSystem::audio_devices device,
                                                  const char *device_address)
{
    AudioSystem::device_connection_state state = AudioSystem::DEVICE_STATE_UNAVAILABLE;
    String8 address = String8(device_address);
    if (AudioSystem::isOutputDevice(device)) {
        if (device & mAvailableOutputDevices) {
#ifdef WITH_A2DP
            if (AudioSystem::isA2dpDevice(device) &&
                address != "" && mA2dpDeviceAddress != address) {
                return state;
            }
#endif
            if (AudioSystem::isBluetoothScoDevice(device) &&
                address != "" && mScoDeviceAddress != address) {
                return state;
            }
            state = AudioSystem::DEVICE_STATE_AVAILABLE;
        }
    } else if (AudioSystem::isInputDevice(device)) {
        if (device & mAvailableInputDevices) {
            state = AudioSystem::DEVICE_STATE_AVAILABLE;
        }
    }

    return state;
}

void AudioPolicyManager::setPhoneState(int state)
{
    LOGV("setPhoneState() state %d", state);
    uint32_t newDevice = 0;
    if (state < 0 || state >= AudioSystem::NUM_MODES) {
        LOGW("setPhoneState() invalid state %d", state);
        return;
    }

    if (state == mPhoneState ) {
        LOGW("setPhoneState() setting same state %d", state);
        return;
    }

#ifdef RECORD_OVER_PCM
    if(isStateInCall(state)){
        AudioParameter param = AudioParameter();
        param.addInt(String8(CLOSE_INPUTS), 1);
        mpClientInterface->setParameters(mHardwareOutput, param.toString());
    }
#endif

    if(isStateInCall(state) && (mPhoneOutput ==0)){
        //open Phone output - the DEVICE_OUT_ALL is used
	AudioOutputDescriptor * phoneOutputDesc = new AudioOutputDescriptor();
	phoneOutputDesc->mDevice = DEVICE_OUT_ALL;
	mPhoneOutput = mpClientInterface->openOutput(&phoneOutputDesc->mDevice,
                                        &phoneOutputDesc->mSamplingRate,
                                        &phoneOutputDesc->mFormat,
                                        &phoneOutputDesc->mChannels,
                                        &phoneOutputDesc->mLatency,
                                         phoneOutputDesc->mFlags);

	if (mPhoneOutput == 0) {
        LOGE("Failed to initialize phone output stream, samplingRate: %d, format %d, channels %d",
                phoneOutputDesc->mSamplingRate, phoneOutputDesc->mFormat, phoneOutputDesc->mChannels);
        } else {
        addOutput(mPhoneOutput, phoneOutputDesc);
        }
    }
    // if leaving call state, handle special case of active streams
    // pertaining to sonification strategy see handleIncallSonification()
    if (isStateInCall(mPhoneState)) {
        LOGV("setPhoneState() in call state management: new state is %d", state);
        for (int stream = 0; stream < AudioSystem::NUM_STREAM_TYPES; stream++) {
            handleIncallSonification(stream, false, true);
        }
    }

    if (mAvailableOutputDevices & (DEVICE_OUT_FM_HEADPHONE | DEVICE_OUT_FM_SPEAKER)){
        // Phone change state from Normal, disable FM.
        if (mPhoneState == AudioSystem::MODE_NORMAL){
            LOGV("setPhoneState() FM DISABLE.");
            DISABLE_FM();
        }
    }

    // store previous phone state for management of sonification strategy below
    int oldState = mPhoneState;
    mPhoneState = state;
    bool force = false;

    // are we entering or starting a call
    if (!isStateInCall(oldState) && isStateInCall(state)) {
        LOGV("  Entering call in setPhoneState()");
        // force routing command to audio hardware when starting a call
        // even if no device change is needed
        force = true;
    } else if (isStateInCall(oldState) && !isStateInCall(state)) {
        LOGV("  Exiting call in setPhoneState()");
        // force routing command to audio hardware when exiting a call
        // even if no device change is needed
        force = true;
    } else if (isStateInCall(state) && (state != oldState)) {
        LOGV("  Switching between telephony and VoIP in setPhoneState()");
        // force routing command to audio hardware when switching between telephony and VoIP
        // even if no device change is needed
        force = true;
    }

    // check for device and output changes triggered by new phone state
    newDevice = getNewDevice(mHardwareOutput, false);
#ifdef WITH_A2DP
    checkOutputForAllStrategies();
    checkA2dpSuspend();
#endif
    audio_io_handle_t dstOutput = 0;

    if((mA2dpOutput == 0) && (!isInCall()))
        dstOutput = mHardwareOutput;
    else
        dstOutput = mPhoneOutput;

    for (int i = 0; i < (int)AudioSystem::NUM_STREAM_TYPES; i++) {
        if ((getStrategy((AudioSystem::stream_type)i) == STRATEGY_PHONE)
                ||(getStrategy((AudioSystem::stream_type)i) == STRATEGY_DTMF)) {
                mpClientInterface->setStreamOutput((AudioSystem::stream_type)i, dstOutput);
        }
    }

    updateDeviceForStrategy();

    AudioOutputDescriptor *hwOutputDesc = mOutputs.valueFor(mHardwareOutput);

    // force routing command to audio hardware when ending call
    // even if no device change is needed
	if (isStateInCall(oldState) && newDevice == 0) {
        newDevice = hwOutputDesc->device();
    }

    // when changing from ring tone to in call mode, mute the ringing tone
    // immediately and delay the route change to avoid sending the ring tone
    // tail into the earpiece or headset.
    int delayMs = 0;
    if (isStateInCall(state) && oldState == AudioSystem::MODE_RINGTONE) {
        // delay the device change command by twice the output latency to have some margin
        // and be sure that audio buffers not yet affected by the mute are out when
        // we actually apply the route change
        delayMs = mOutputs.valueFor(mHardwareOutput)->mLatency*2;
        setStreamMute(AudioSystem::RING, true, mHardwareOutput);
    }

    nsecs_t duration =(systemTime() - mMusicStopTime)/1000000;
    int latency = mOutputs.valueFor(mHardwareOutput)->mLatency*2;
    if(isStateInCall(state) && (oldState == AudioSystem::MODE_NORMAL) && (duration < latency)) {
	delayMs = mOutputs.valueFor(mHardwareOutput)->mLatency*2;
    }

    // change routing is necessary
    setOutputDevice(mHardwareOutput, newDevice, force, delayMs);

    // if entering in call state, handle special case of active streams
    // pertaining to sonification strategy see handleIncallSonification()
    if (isStateInCall(state)) {
        LOGV("setPhoneState() in call state management: new state is %d", state);
        // unmute the ringing tone after a sufficient delay if it was muted before
        // setting output device above
        if (oldState == AudioSystem::MODE_RINGTONE) {
            setStreamMute(AudioSystem::RING, false, mHardwareOutput, MUTE_TIME_MS);
        }
        for (int stream = 0; stream < AudioSystem::NUM_STREAM_TYPES; stream++) {
            handleIncallSonification(stream, true, true);
        }
    }

    if (mAvailableOutputDevices & (DEVICE_OUT_FM_HEADPHONE | DEVICE_OUT_FM_SPEAKER)){
        // Phone change state to Normal, enable FM.
        if (state == AudioSystem::MODE_NORMAL){
            LOGV("setPhoneState() FM Enable.");
            StreamDescriptor &streamDesc = mStreams[AudioSystem::FM];
            int volInt = (255 * (streamDesc.mIndexCur - streamDesc.mIndexMin))
                                /(streamDesc.mIndexMax - streamDesc.mIndexMin);
            int FMDevice = mAvailableOutputDevices & (DEVICE_OUT_FM_HEADPHONE | DEVICE_OUT_FM_SPEAKER);
            ENABLE_FM(FMDevice, volInt);
        }
    }

    // Flag that ringtone volume must be limited to music volume until we exit MODE_RINGTONE
    if (state == AudioSystem::MODE_RINGTONE &&
        (hwOutputDesc->mRefCount[AudioSystem::MUSIC] ||
        (systemTime() - mMusicStopTime) < seconds(SONIFICATION_HEADSET_MUSIC_DELAY))) {
        mLimitRingtoneVolume = true;
    } else {
        mLimitRingtoneVolume = false;
    }
}

void AudioPolicyManager::setRingerMode(uint32_t mode, uint32_t mask)
{
    LOGV("setRingerMode() mode %x, mask %x", mode, mask);

    mRingerMode = mode;
}

void AudioPolicyManager::setForceUse(AudioSystem::force_use usage, AudioSystem::forced_config config)
{
    LOGV("setForceUse() usage %d, config %d, mPhoneState %d", usage, config, mPhoneState);
 
    bool forceVolumeReeval = false;
    switch(usage) {
    case AudioSystem::FOR_COMMUNICATION:
        if (config != AudioSystem::FORCE_SPEAKER && config != AudioSystem::FORCE_BT_SCO &&
            config != AudioSystem::FORCE_NONE) {
            LOGW("setForceUse() invalid config %d for FOR_COMMUNICATION", config);
            return;
        }
        mForceUse[usage] = config;
	    break;
    case AudioSystem::FOR_MEDIA:
        if (config != AudioSystem::FORCE_HEADPHONES && config != AudioSystem::FORCE_BT_A2DP &&
            config != AudioSystem::FORCE_WIRED_ACCESSORY && config != AudioSystem::FORCE_SPEAKER &&
            config != AudioSystem::FORCE_NONE) {
            LOGW("setForceUse() invalid config %d for FOR_MEDIA", config);
            return;
        }
        mForceUse[usage] = config;
        break;
    case AudioSystem::FOR_RECORD:
        if (config != AudioSystem::FORCE_BT_SCO && config != AudioSystem::FORCE_WIRED_ACCESSORY &&
            config != AudioSystem::FORCE_NONE) {
            LOGW("setForceUse() invalid config %d for FOR_RECORD", config);
            return;
        }
        mForceUse[usage] = config;
        break;
    case AudioSystem::FOR_DOCK:
        if (config != AudioSystem::FORCE_NONE && config != AudioSystem::FORCE_BT_CAR_DOCK &&
            config != AudioSystem::FORCE_BT_DESK_DOCK && config != AudioSystem::FORCE_WIRED_ACCESSORY) {
            LOGW("setForceUse() invalid config %d for FOR_DOCK", config);
        }
        forceVolumeReeval = true;
        mForceUse[usage] = config;
        break;
    default:
        LOGW("setForceUse() invalid usage %d", usage);
        break;
    }

	// check for device and output changes triggered by new phone state
    uint32_t newDevice = getNewDevice(mHardwareOutput, false);
#ifdef WITH_A2DP
    checkOutputForAllStrategies();
    checkA2dpSuspend();
#endif
    updateDeviceForStrategy();
    setOutputDevice(mHardwareOutput, newDevice);
    if (forceVolumeReeval) {
        applyStreamVolumes(mHardwareOutput, newDevice);
    }

    audio_io_handle_t activeInput = getActiveInput();
    if (activeInput != 0) {
        AudioInputDescriptor *inputDesc = mInputs.valueFor(activeInput);
        newDevice = getDeviceForInputSource(inputDesc->mInputSource);
        if (newDevice != inputDesc->mDevice) {
            LOGV("setForceUse() changing device from %x to %x for input %d",
                    inputDesc->mDevice, newDevice, activeInput);
            inputDesc->mDevice = newDevice;
            AudioParameter param = AudioParameter();
            param.addInt(String8(AudioParameter::keyRouting), (int)newDevice);
            mpClientInterface->setParameters(activeInput, param.toString());
        }
    }
}

AudioSystem::forced_config AudioPolicyManager::getForceUse(AudioSystem::force_use usage)
{
    return mForceUse[usage];
}

void AudioPolicyManager::setSystemProperty(const char* property, const char* value)
{
    LOGV("setSystemProperty() property %s, value %s", property, value);
    if (strcmp(property, "ro.camera.sound.forced") == 0) {
        if (atoi(value)) {
            LOGV("ENFORCED_AUDIBLE cannot be muted");
            mStreams[AudioSystem::ENFORCED_AUDIBLE].mCanBeMuted = false;
        } else {
            LOGV("ENFORCED_AUDIBLE can be muted");
            mStreams[AudioSystem::ENFORCED_AUDIBLE].mCanBeMuted = true;
        }
    }
}

audio_io_handle_t AudioPolicyManager::getOutput(AudioSystem::stream_type stream,
                                    uint32_t samplingRate,
                                    uint32_t format,
                                    uint32_t channels,
                                    AudioSystem::output_flags flags)
{
    audio_io_handle_t output = 0;
    uint32_t latency = 0;
    routing_strategy strategy = getStrategy((AudioSystem::stream_type)stream);
    uint32_t device = getDeviceForStrategy(strategy);
    LOGV("getOutput() stream %d,  device %d, samplingRate %d, format %d, channels %x, flags %x", stream, device, samplingRate, format, channels, flags);

    // open a direct output if:
    // 1 a direct output is explicitely requested
    // 2 the audio format is compressed
    if ((flags & AudioSystem::OUTPUT_FLAG_DIRECT) ||
         (format !=0 && !AudioSystem::isLinearPCM(format))) 
	{
        LOGV("getOutput() opening direct output device %x", device);
        AudioOutputDescriptor *outputDesc = new AudioOutputDescriptor();
        outputDesc->mDevice = device;
        outputDesc->mSamplingRate = samplingRate;
        outputDesc->mFormat = format;
        outputDesc->mChannels = channels;
        outputDesc->mLatency = 0;
        outputDesc->mFlags = (AudioSystem::output_flags)(flags); //| AudioSystem::OUTPUT_FLAG_DIRECT);
        outputDesc->mRefCount[stream] = 1;
        output = mpClientInterface->openOutput(&outputDesc->mDevice,
                                        &outputDesc->mSamplingRate,
                                        &outputDesc->mFormat,
                                        &outputDesc->mChannels,
                                        &outputDesc->mLatency,
                                        outputDesc->mFlags);

        // only accept an output with the requeted parameters
        if ((samplingRate != 0 && samplingRate != outputDesc->mSamplingRate) ||
            (format != 0 && format != outputDesc->mFormat) ||
            (channels != 0 && channels != outputDesc->mChannels)) {
            LOGV("getOutput() failed opening direct output: samplingRate %d, format %d, channels %d",
                    samplingRate, format, channels);
            mpClientInterface->closeOutput(output);
            delete outputDesc;
            return 0;
        }
        addOutput(output, outputDesc);
        return output;
    }

    if (channels != 0 && channels != AudioSystem::CHANNEL_OUT_MONO &&
        channels != AudioSystem::CHANNEL_OUT_STEREO) {
        return 0;
    }
    // open a non direct output

    // get which output is suitable for the specified stream. The actual routing change will happen
    // when startOutput() will be called
    uint32_t device2 = device & ~AudioSystem::DEVICE_OUT_SPEAKER;
    if (AudioSystem::popCount((AudioSystem::audio_devices)device) == 2) {
#ifdef WITH_A2DP
        if ((AudioSystem::isA2dpDevice((AudioSystem::audio_devices)device2)) && (mPhoneState != MODE_RINGTONE)) {
            // if playing on 2 devices among which one is A2DP, use duplicated output
            LOGV("getOutput() using duplicated output");
            LOGW_IF((mA2dpOutput == 0), "getOutput() A2DP device in multiple %x selected but A2DP output not opened", device);
            output = mDuplicatedOutput;
        } else
#endif
        {
            // if playing on 2 devices among which none is A2DP, use hardware output
            output = mHardwareOutput;
        }
        LOGV("getOutput() using output %d for 2 devices %x", output, device);
    } else {
#ifdef WITH_A2DP
        if ((AudioSystem::isA2dpDevice((AudioSystem::audio_devices)device2)) && (mPhoneState != MODE_RINGTONE)) {
            // if playing on A2DP device, use a2dp output
            LOGW_IF((mA2dpOutput == 0), "getOutput() A2DP device %x selected but A2DP output not opened", device);
            output = mA2dpOutput;
        } else
#endif
        {
		    output = mHardwareOutput;
            /*
            If strategy is STRATEGY_DTMF,  Use mPhoneOutput when mPhoneState is in real call state.
            If strategy is STRATEGY_PHONE, Use mPhoneOutput in these cases:
                1) in a real call(VT Call or Voice Call)
                2) in holding state(MODE_NORMAL) with bluetooth device during a voice call.
            */
            if (mPhoneState == MODE_IN_CALL || mPhoneState == MODE_IN_VT_CALL) {
                if (getStrategy(stream) == STRATEGY_PHONE || getStrategy(stream) == STRATEGY_DTMF) {
                    output = mPhoneOutput;
                }
            } else if (mA2dpOutput && (mPhoneState != MODE_IN_COMMUNICATION)) {
                if (getStrategy(stream) == STRATEGY_PHONE) {
                    output = mPhoneOutput;
                }
            }
        }
    }

    LOGW_IF((output ==0), "getOutput() could not find output for stream %d, samplingRate %d, format %d, channels %x, flags %x",
                stream, samplingRate, format, channels, flags);

    return output;
}

#if PLATFORM_SDK_VERSION >= 9
status_t AudioPolicyManager::startOutput(audio_io_handle_t output, AudioSystem::stream_type stream, int session)
#else
status_t AudioPolicyManager::startOutput(audio_io_handle_t output, AudioSystem::stream_type stream)
#endif
{
    LOGV("startOutput() output %d, stream %d", output, stream);

    ssize_t index = mOutputs.indexOfKey(output);
    if (index < 0) {
        LOGW("startOutput() unknow output %d", output);
        return BAD_VALUE;
    }

    AudioOutputDescriptor *outputDesc = mOutputs.valueAt(index);
    routing_strategy strategy = getStrategy((AudioSystem::stream_type)stream);

#ifdef WITH_A2DP
    if (mA2dpOutput != 0  && !a2dpUsedForSonification() && strategy == STRATEGY_SONIFICATION) {
        setStrategyMute(STRATEGY_MEDIA, true, mA2dpOutput);
    }
#endif

    if ((output == mPhoneOutput) && ((stream == AudioSystem::DTMF) || (stream == AudioSystem::BLUETOOTH_SCO) ||
        (stream == AudioSystem::VOICE_CALL))) {
        // open mHandle of phoneOutput
        outputDesc->changeRefCount(stream, 1);
        AudioParameter param = AudioParameter();
        param.addInt(String8(PHONE_PLUGIN_ACTIVE), 1);
        mpClientInterface->setParameters(output, param.toString());
        return NO_ERROR;
    }

    if ((output == mHardwareOutput ) &&
            (mAvailableOutputDevices & (DEVICE_OUT_FM_HEADPHONE | DEVICE_OUT_FM_SPEAKER))){
            LOGV("startOutput() FM DISABLE");
            DISABLE_FM();
    }

    // incremenent usage count for this stream on the requested output:
    // NOTE that the usage count is the same for duplicated output and hardware output which is
    // necassary for a correct control of hardware output routing by startOutput() and stopOutput()
    outputDesc->changeRefCount(stream, 1);
    if(stream == AudioSystem::MUSIC)
        if (acquire_wake_lock(PARTIAL_WAKE_LOCK, mWakeLock.string()) > 0 )
            LOGI("*********************************acquire_wake_lock ok %s",mWakeLock.string());


    setOutputDevice(output, getNewDevice(output));

    // handle special case for sonification while in call
    if (isInCall()) {
        handleIncallSonification(stream, true, false);
    }

    // apply volume rules for current stream and device if necessary
    checkAndSetVolume(stream, mStreams[stream].mIndexCur, output, outputDesc->device());

    return NO_ERROR;
}

#if PLATFORM_SDK_VERSION >=9 
status_t AudioPolicyManager::stopOutput(audio_io_handle_t output, AudioSystem::stream_type stream, int session)
#else
status_t AudioPolicyManager::stopOutput(audio_io_handle_t output, AudioSystem::stream_type stream)
#endif
{
    LOGV("stopOutput() output %d, stream %d", output, stream);
    ssize_t index = mOutputs.indexOfKey(output);
    if (index < 0) {
        LOGW("stopOutput() unknow output %d", output);
        return BAD_VALUE;
    }

    AudioOutputDescriptor *outputDesc = mOutputs.valueAt(index);
    routing_strategy strategy = getStrategy((AudioSystem::stream_type)stream);

	if(output == mPhoneOutput)
	{
		outputDesc->changeRefCount(stream, -1);
		return NO_ERROR;
	}

    // handle special case for sonification while in call
    if (isInCall()) {
        handleIncallSonification(stream, false, false);
    }

	if (outputDesc->mRefCount[stream] > 0) {
        // decrement usage count of this stream on the output
        outputDesc->changeRefCount(stream, -1);
        // store time at which the last music track was stopped - see computeVolume()
        if (stream == AudioSystem::MUSIC) {
            mMusicStopTime = systemTime();
            if(outputDesc->mRefCount[stream] == 0)
                release_wake_lock(mWakeLock.string());
        }

        setOutputDevice(output, getNewDevice(output));

#ifdef WITH_A2DP
        if (mA2dpOutput != 0 && !a2dpUsedForSonification() &&
                strategy == STRATEGY_SONIFICATION) {
            setStrategyMute(STRATEGY_MEDIA,
                            false,
                            mA2dpOutput,
                            mOutputs.valueFor(mHardwareOutput)->mLatency*2);
        }
#endif
        return NO_ERROR;
    } else {
        LOGW("stopOutput() refcount is already 0 for output %d", output);
        return INVALID_OPERATION;
    }
}

void AudioPolicyManager::releaseOutput(audio_io_handle_t output)
{
    LOGV("releaseOutput() %d", output);
    ssize_t index = mOutputs.indexOfKey(output);
    if (index < 0) {
        LOGW("releaseOutput() releasing unknown output %d", output);
        return;
    }
    if (output == mPhoneOutput) {
        AudioParameter param = AudioParameter();
        param.addInt(String8(PHONE_PLUGIN_ACTIVE), 0);

        // sum ref count of all stream types
        int refCounts = 0;
        for (ssize_t i = 0; i < AudioSystem::NUM_STREAM_TYPES; i++) {
            refCounts += mOutputs.valueAt(index)->mRefCount[i];
        }
        param.addInt(String8(STREAM_ACTIVE), refCounts);

        mpClientInterface->setParameters(output, param.toString());
    }

    if (mOutputs.valueAt(index)->mFlags & AudioSystem::OUTPUT_FLAG_DIRECT) {
        mpClientInterface->closeOutput(output);
        delete mOutputs.valueAt(index);
        mOutputs.removeItem(output);
    }
}

audio_io_handle_t AudioPolicyManager::getInput(int inputSource,
                                    uint32_t samplingRate,
                                    uint32_t format,
                                    uint32_t channels,
                                    AudioSystem::audio_in_acoustics acoustics)
{
    audio_io_handle_t input = 0;
    uint32_t device = getDeviceForInputSource(inputSource);
    int ret = 0;

    LOGV("getInput() inputSource %d, device %d,samplingRate %d, format %d, channels %x, acoustics %x", inputSource, device , samplingRate, format, channels, acoustics);

    if (device == 0) {
        return 0;
    }

 #ifdef RECORD_OVER_PCM
    /*
        refuse PCM recording during voice call since CP is using PCM port currently

        About VT, the "default" input source is used to obtain the near
        end voice, so mInput.size() don't equal 0.If the getInput is not 
        called by the VT recorder(AUDIO_SOURCE_VOICE_VT_CALL), refuse the PCM recording
	*/
    if ((mPhoneState == AudioSystem::MODE_IN_CALL && inputSource != AUDIO_SOURCE_VOICE_CALL) ||
        (mPhoneState == AudioSystem::MODE_IN_VT_CALL && mInputs.size() != 0 && inputSource != AUDIO_SOURCE_VOICE_VT_CALL))
    {
        LOGE("refuse PCM recording during voice call/VT: mPhoneState %d, inputSource %d", mPhoneState, inputSource);
        return 0;
    }

   // switch ADC/SSP sample rate
    ret = switch_sample_rate(1, samplingRate, samplingRate);
    if(ret < 0) {
    LOGI("getInput() can not siwtch to %d \n", samplingRate);
    return 0;
    }
#endif

   AudioInputDescriptor *inputDesc = new AudioInputDescriptor();

    inputDesc->mInputSource = inputSource;
    inputDesc->mDevice = device;
    inputDesc->mSamplingRate = samplingRate;
    inputDesc->mFormat = format;
    inputDesc->mChannels = channels;
    inputDesc->mAcoustics = acoustics;
    inputDesc->mRefCount = 0;
    input = mpClientInterface->openInput(&inputDesc->mDevice,
                                    &inputDesc->mSamplingRate,
                                    &inputDesc->mFormat,
                                    &inputDesc->mChannels,
                                    inputDesc->mAcoustics);

    // only accept input with the exact requested set of parameters
    if (input == 0 ||
		(samplingRate != inputDesc->mSamplingRate) ||
        (format != inputDesc->mFormat) ||
        (channels != inputDesc->mChannels)) {
        LOGV("getInput() failed opening input: samplingRate %d, format %d, channels %d",
                samplingRate, format, channels);
		if (input != 0) {
           mpClientInterface->closeInput(input);
		}
        delete inputDesc;
        return 0;
    }
    mInputs.add(input, inputDesc);
    return input;
}

status_t AudioPolicyManager::startInput(audio_io_handle_t input)
{
    LOGV("startInput() input %d", input);
    ssize_t index = mInputs.indexOfKey(input);
    if (index < 0) {
        LOGW("startInput() unknow input %d", input);
        return BAD_VALUE;
    }
    AudioInputDescriptor *inputDesc = mInputs.valueAt(index);

    // refuse 2 active AudioRecord clients at the same time
    if (getActiveInput() != 0) {
        if (mPhoneState != AudioSystem::MODE_IN_VT_CALL || inputDesc->mDevice != AudioSystem::DEVICE_IN_VT_MIC){
            LOGW("startInput() input %d failed: other input already started", input);
            return INVALID_OPERATION;
	}
    }

    AudioParameter param = AudioParameter();
    param.addInt(String8(AudioParameter::keyRouting), (int)inputDesc->mDevice);
    param.addInt(String8(STREAM_IN_ACTIVE), 1);
    mpClientInterface->setParameters(input, param.toString());

    inputDesc->mRefCount = 1;
    return NO_ERROR;
}

status_t AudioPolicyManager::stopInput(audio_io_handle_t input)
{
    LOGV("stopInput() input %d", input);
    ssize_t index = mInputs.indexOfKey(input);
    if (index < 0) {
        LOGW("stopInput() unknow input %d", input);
        return BAD_VALUE;
    }
    AudioInputDescriptor *inputDesc = mInputs.valueAt(index);

    if (inputDesc->mRefCount == 0) {
        LOGW("stopInput() input %d already stopped", input);
        return INVALID_OPERATION;
    } else {
        AudioParameter param = AudioParameter();
        param.addInt(String8(AudioParameter::keyRouting), 0);
        param.addInt(String8(STREAM_IN_ACTIVE), 0);
        mpClientInterface->setParameters(input, param.toString());
        inputDesc->mRefCount = 0;
        return NO_ERROR;
    }
}

void AudioPolicyManager::releaseInput(audio_io_handle_t input)
{
    LOGV("releaseInput() %d", input);
    ssize_t index = mInputs.indexOfKey(input);
    if (index < 0) {
        LOGW("releaseInput() releasing unknown input %d", input);
        return;
    }
    mpClientInterface->closeInput(input);
    delete mInputs.valueAt(index);
    mInputs.removeItem(input);
    LOGV("releaseInput() exit");
}



void AudioPolicyManager::initStreamVolume(AudioSystem::stream_type stream,
                                            int indexMin,
                                            int indexMax)
{
    LOGV("initStreamVolume() stream %d, min %d, max %d", stream , indexMin, indexMax);
    if (indexMin < 0 || indexMin >= indexMax) {
        LOGW("initStreamVolume() invalid index limits for stream %d, min %d, max %d", stream , indexMin, indexMax);
        return;
    }
    mStreams[stream].mIndexMin = indexMin;
    mStreams[stream].mIndexMax = indexMax;
}

status_t AudioPolicyManager::setStreamVolumeIndex(AudioSystem::stream_type stream, int index)
{
    if ((index < mStreams[stream].mIndexMin) || (index > mStreams[stream].mIndexMax)) {
        return BAD_VALUE;
    }

	// Force max volume if stream cannot be muted
    if (!mStreams[stream].mCanBeMuted) index = mStreams[stream].mIndexMax;

    LOGV("setStreamVolumeIndex() stream %d, index %d", stream, index);
    mStreams[stream].mIndexCur = index;

    // compute and apply stream volume on all outputs according to connected device
    status_t status = NO_ERROR;
    for (size_t i = 0; i < mOutputs.size(); i++) {
        status_t volStatus = checkAndSetVolume(stream, index, mOutputs.keyAt(i), mOutputs.valueAt(i)->device(), 0, true);
        if (volStatus != NO_ERROR) {
            status = volStatus;
        }
    }
    return status;
}

status_t AudioPolicyManager::getStreamVolumeIndex(AudioSystem::stream_type stream, int *index)
{
    if (index == 0) {
        return BAD_VALUE;
    }
    LOGV("getStreamVolumeIndex() stream %d", stream);
    *index =  mStreams[stream].mIndexCur;
    return NO_ERROR;
}

status_t AudioPolicyManager::dump(int fd)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;

    snprintf(buffer, SIZE, "\nAudioPolicyManager Dump: %p\n", this);
    result.append(buffer);
    snprintf(buffer, SIZE, " Hardware Output: %d\n", mHardwareOutput);
    result.append(buffer);
#ifdef WITH_A2DP
    snprintf(buffer, SIZE, " A2DP Output: %d\n", mA2dpOutput);
    result.append(buffer);
    snprintf(buffer, SIZE, " Duplicated Output: %d\n", mDuplicatedOutput);
    result.append(buffer);
    snprintf(buffer, SIZE, " A2DP device address: %s\n", mA2dpDeviceAddress.string());
    result.append(buffer);
#endif
    snprintf(buffer, SIZE, " SCO device address: %s\n", mScoDeviceAddress.string());
    result.append(buffer);
    snprintf(buffer, SIZE, " Output devices: %08x\n", mAvailableOutputDevices);
    result.append(buffer);
    snprintf(buffer, SIZE, " Input devices: %08x\n", mAvailableInputDevices);
    result.append(buffer);
    snprintf(buffer, SIZE, " Phone state: %d\n", mPhoneState);
    result.append(buffer);
    snprintf(buffer, SIZE, " Ringer mode: %d\n", mRingerMode);
    result.append(buffer);
    snprintf(buffer, SIZE, " Force use for communications %d\n", mForceUse[AudioSystem::FOR_COMMUNICATION]);
    result.append(buffer);
    snprintf(buffer, SIZE, " Force use for media %d\n", mForceUse[AudioSystem::FOR_MEDIA]);
    result.append(buffer);
    snprintf(buffer, SIZE, " Force use for record %d\n", mForceUse[AudioSystem::FOR_RECORD]);
    result.append(buffer);
    snprintf(buffer, SIZE, " Force use for dock %d\n", mForceUse[AudioSystem::FOR_DOCK]);
    result.append(buffer);
    write(fd, result.string(), result.size());

    snprintf(buffer, SIZE, "\nOutputs dump:\n");
    write(fd, buffer, strlen(buffer));
    for (size_t i = 0; i < mOutputs.size(); i++) {
        snprintf(buffer, SIZE, "- Output %d dump:\n", mOutputs.keyAt(i));
        write(fd, buffer, strlen(buffer));
        mOutputs.valueAt(i)->dump(fd);
    }

    snprintf(buffer, SIZE, "\nInputs dump:\n");
    write(fd, buffer, strlen(buffer));
    for (size_t i = 0; i < mInputs.size(); i++) {
        snprintf(buffer, SIZE, "- Input %d dump:\n", mInputs.keyAt(i));
        write(fd, buffer, strlen(buffer));
        mInputs.valueAt(i)->dump(fd);
    }

    snprintf(buffer, SIZE, "\nStreams dump:\n");
    write(fd, buffer, strlen(buffer));
    snprintf(buffer, SIZE, " Stream  Index Min  Index Max  Index Cur  Mute Count  Can be muted\n");
    write(fd, buffer, strlen(buffer));
    for (size_t i = 0; i < AudioSystem::NUM_STREAM_TYPES; i++) {
        snprintf(buffer, SIZE, " %02d", i);
        mStreams[i].dump(buffer + 3, SIZE-3);
        write(fd, buffer, strlen(buffer));
    }

    return NO_ERROR;
}

// ----------------------------------------------------------------------------
// AudioPolicyManager
// ----------------------------------------------------------------------------

// ---  class factory


extern "C" AudioPolicyInterface* createAudioPolicyManager(AudioPolicyClientInterface *clientInterface)
{
    return new AudioPolicyManager(clientInterface);
}

extern "C" void destroyAudioPolicyManager(AudioPolicyInterface *interface)
{
    delete interface;
}

AudioPolicyManager::AudioPolicyManager(AudioPolicyClientInterface *clientInterface)
: mPhoneState(AudioSystem::MODE_NORMAL), mRingerMode(0), mMusicStopTime(0), mLimitRingtoneVolume(false), mLastVoiceVolume(-1.0f), mA2dpSuspended(false)
{
    mpClientInterface = clientInterface;

    for (int i = 0; i < AudioSystem::NUM_FORCE_USE; i++) {
        mForceUse[i] = AudioSystem::FORCE_NONE;
    }

    mAvailableOutputDevices = get_all_output_device();
    mAvailableInputDevices = get_all_input_device();

#ifdef WITH_A2DP
    mA2dpOutput = 0;
    mDuplicatedOutput = 0;
    mA2dpDeviceAddress = String8("");
#endif
    mScoDeviceAddress = String8("");

    mWakeLock = String8("MusicWakeLock");
    // open hardware output
    AudioOutputDescriptor *outputDesc = new AudioOutputDescriptor();
    outputDesc->mDevice = (uint32_t)AudioSystem::DEVICE_OUT_SPEAKER;
    mHardwareOutput = mpClientInterface->openOutput(&outputDesc->mDevice,
                                    &outputDesc->mSamplingRate,
                                    &outputDesc->mFormat,
                                    &outputDesc->mChannels,
                                    &outputDesc->mLatency,
                                    outputDesc->mFlags);

    if (mHardwareOutput == 0) {
        LOGE("Failed to initialize hardware output stream, samplingRate: %d, format %d, channels %d",
                outputDesc->mSamplingRate, outputDesc->mFormat, outputDesc->mChannels);
    } else {
        addOutput(mHardwareOutput, outputDesc);
    }

    outputDesc->mDevice = 0;
    mPhoneOutput = 0;

    updateDeviceForStrategy();
}

AudioPolicyManager::~AudioPolicyManager()
{
   for (size_t i = 0; i < mOutputs.size(); i++) {
        mpClientInterface->closeOutput(mOutputs.keyAt(i));
        delete mOutputs.valueAt(i);
   }
   mOutputs.clear();
   for (size_t i = 0; i < mInputs.size(); i++) {
        mpClientInterface->closeInput(mInputs.keyAt(i));
        delete mInputs.valueAt(i);
   }
   mInputs.clear();
}

void AudioPolicyManager::addOutput(audio_io_handle_t id, AudioOutputDescriptor *outputDesc)
{
    outputDesc->mId = id;
    mOutputs.add(id, outputDesc);
}

#ifdef WITH_A2DP
status_t AudioPolicyManager::handleA2dpConnection(AudioSystem::audio_devices device,
                                                 const char *device_address)
{
    // when an A2DP device is connected, open an A2DP and a duplicated output
    LOGV("opening A2DP output for device %s", device_address);
    AudioOutputDescriptor *outputDesc = new AudioOutputDescriptor();
    outputDesc->mDevice = device;
    mA2dpOutput = mpClientInterface->openOutput(&outputDesc->mDevice,
                                            &outputDesc->mSamplingRate,
                                            &outputDesc->mFormat,
                                            &outputDesc->mChannels,
                                            &outputDesc->mLatency,
                                            outputDesc->mFlags);
    if (mA2dpOutput) {
        // add A2DP output descriptor
        addOutput(mA2dpOutput, outputDesc);

        //TODO: configure audio effect output stage here

        // set initial stream volume for A2DP device
        applyStreamVolumes(mA2dpOutput, device);
        if (a2dpUsedForSonification()) {
            mDuplicatedOutput = mpClientInterface->openDuplicateOutput(mA2dpOutput, mHardwareOutput);
        }
        if (mDuplicatedOutput != 0 ||
            !a2dpUsedForSonification()) {
            // If both A2DP and duplicated outputs are open, send device address to A2DP hardware
            // interface
            AudioParameter param;
            param.add(String8("a2dp_sink_address"), String8(device_address));
            mpClientInterface->setParameters(mA2dpOutput, param.toString());
            mA2dpDeviceAddress = String8(device_address, MAX_DEVICE_ADDRESS_LEN);

            if (a2dpUsedForSonification()) {
                // add duplicated output descriptor
                AudioOutputDescriptor *dupOutputDesc = new AudioOutputDescriptor();
                dupOutputDesc->mOutput1 = mOutputs.valueFor(mHardwareOutput);
                dupOutputDesc->mOutput2 = mOutputs.valueFor(mA2dpOutput);
                dupOutputDesc->mSamplingRate = outputDesc->mSamplingRate;
                dupOutputDesc->mFormat = outputDesc->mFormat;
                dupOutputDesc->mChannels = outputDesc->mChannels;
                dupOutputDesc->mLatency = outputDesc->mLatency;
                addOutput(mDuplicatedOutput, dupOutputDesc);
                applyStreamVolumes(mDuplicatedOutput, device);
            }
        } else {
            LOGW("getOutput() could not open duplicated output for %d and %d",
                    mHardwareOutput, mA2dpOutput);
            mpClientInterface->closeOutput(mA2dpOutput);
            mOutputs.removeItem(mA2dpOutput);
            mA2dpOutput = 0;
            delete outputDesc;
            return NO_INIT;
        }
    } else {
        LOGW("setDeviceConnectionState() could not open A2DP output for device %x", device);
        delete outputDesc;
        return NO_INIT;
    }
    AudioOutputDescriptor *hwOutputDesc = mOutputs.valueFor(mHardwareOutput);

    if (!a2dpUsedForSonification()) {
        // mute music on A2DP output if a notification or ringtone is playing
        uint32_t refCount = hwOutputDesc->strategyRefCount(STRATEGY_SONIFICATION);
        for (uint32_t i = 0; i < refCount; i++) {
            setStrategyMute(STRATEGY_MEDIA, true, mA2dpOutput);
        }
    }

    mA2dpSuspended = false;

    return NO_ERROR;
}

status_t AudioPolicyManager::handleA2dpDisconnection(AudioSystem::audio_devices device,
                                                    const char *device_address)
{
    if (mA2dpOutput == 0) {
        LOGW("setDeviceConnectionState() disconnecting A2DP and no A2DP output!");
        return INVALID_OPERATION;
    }

    if (mA2dpDeviceAddress != device_address) {
        LOGW("setDeviceConnectionState() disconnecting unknow A2DP sink address %s", device_address);
        return INVALID_OPERATION;
    }

    // mute media strategy to avoid outputting sound on hardware output while music stream
    // is switched from A2DP output and before music is paused by music application
    setStrategyMute(STRATEGY_MEDIA, true, mHardwareOutput);
    setStrategyMute(STRATEGY_MEDIA, false, mHardwareOutput, MUTE_TIME_MS);

    if (!a2dpUsedForSonification()) {
        // unmute music on A2DP output if a notification or ringtone is playing
        uint32_t refCount = mOutputs.valueFor(mHardwareOutput)->strategyRefCount(STRATEGY_SONIFICATION);
        for (uint32_t i = 0; i < refCount; i++) {
            setStrategyMute(STRATEGY_MEDIA, false, mA2dpOutput);
        }
    }
    mA2dpDeviceAddress = "";
    mA2dpSuspended = false;
    return NO_ERROR;
}

void AudioPolicyManager::closeA2dpOutputs()
{
    LOGV("setDeviceConnectionState() closing A2DP and duplicated output!");

    if (mDuplicatedOutput != 0) {
        AudioOutputDescriptor *dupOutputDesc = mOutputs.valueFor(mDuplicatedOutput);
        AudioOutputDescriptor *hwOutputDesc = mOutputs.valueFor(mHardwareOutput);
        // As all active tracks on duplicated output will be deleted,
        // and as they were also referenced on hardware output, the reference
        // count for their stream type must be adjusted accordingly on
        // hardware output.
        for (int i = 0; i < (int)AudioSystem::NUM_STREAM_TYPES; i++) {
            int refCount = dupOutputDesc->mRefCount[i];
            hwOutputDesc->changeRefCount((AudioSystem::stream_type)i,-refCount);
        }

        mpClientInterface->closeOutput(mDuplicatedOutput);
        delete mOutputs.valueFor(mDuplicatedOutput);
        mOutputs.removeItem(mDuplicatedOutput);
        mDuplicatedOutput = 0;
    }
    if (mA2dpOutput != 0) {
        AudioParameter param;
        param.add(String8("closing"), String8("true"));
        mpClientInterface->setParameters(mA2dpOutput, param.toString());

        mpClientInterface->closeOutput(mA2dpOutput);
        delete mOutputs.valueFor(mA2dpOutput);
        mOutputs.removeItem(mA2dpOutput);
        mA2dpOutput = 0;
    }
}

void AudioPolicyManager::checkOutputForStrategy(routing_strategy strategy)
{
    uint32_t prevDevice = getDeviceForStrategy(strategy);
    uint32_t curDevice = getDeviceForStrategy(strategy, false);
    bool a2dpWasUsed = AudioSystem::isA2dpDevice((AudioSystem::audio_devices)(prevDevice & ~AudioSystem::DEVICE_OUT_SPEAKER));
    bool a2dpIsUsed = AudioSystem::isA2dpDevice((AudioSystem::audio_devices)(curDevice & ~AudioSystem::DEVICE_OUT_SPEAKER));
    audio_io_handle_t srcOutput = 0;
    audio_io_handle_t dstOutput = 0;

    if (a2dpWasUsed && !a2dpIsUsed) {
        bool dupUsed = a2dpUsedForSonification() && a2dpWasUsed && (AudioSystem::popCount(prevDevice) == 2);
        dstOutput = mHardwareOutput;
        if (dupUsed) {
            LOGV("checkOutputForStrategy() moving strategy %d from duplicated", strategy);
            srcOutput = mDuplicatedOutput;
        } else {
            LOGV("checkOutputForStrategy() moving strategy %d from a2dp", strategy);
            srcOutput = mA2dpOutput;
        }
    }
    if (a2dpIsUsed && !a2dpWasUsed) {
        bool dupUsed = a2dpUsedForSonification() && a2dpIsUsed && (AudioSystem::popCount(curDevice) == 2);
        srcOutput = mHardwareOutput;
        if (dupUsed) {
            LOGV("checkOutputForStrategy() moving strategy %d to duplicated", strategy);
            dstOutput = mDuplicatedOutput;
        } else {
            LOGV("checkOutputForStrategy() moving strategy %d to a2dp", strategy);
            dstOutput = mA2dpOutput;
        }
    }

    if (srcOutput != 0 && dstOutput != 0) {
        // Move effects associated to this strategy from previous output to new output
        for (size_t i = 0; i < mEffects.size(); i++) {
            EffectDescriptor *desc = mEffects.valueAt(i);
            if (desc->mSession != AudioSystem::SESSION_OUTPUT_STAGE &&
                    desc->mStrategy == strategy &&
                    desc->mOutput == srcOutput) {
                LOGV("checkOutputForStrategy() moving effect %d to output %d", mEffects.keyAt(i), dstOutput);
                mpClientInterface->moveEffects(desc->mSession, srcOutput, dstOutput);
                desc->mOutput = dstOutput;
            }
        }
        // Move tracks associated to this strategy from previous output to new output
        for (int i = 0; i < (int)AudioSystem::NUM_STREAM_TYPES; i++) {
            if (getStrategy((AudioSystem::stream_type)i) == strategy) {
                mpClientInterface->setStreamOutput((AudioSystem::stream_type)i, dstOutput);
            }
        }
    }
}

void AudioPolicyManager::checkOutputForAllStrategies()
{
    checkOutputForStrategy(STRATEGY_PHONE);
    checkOutputForStrategy(STRATEGY_SONIFICATION);
    checkOutputForStrategy(STRATEGY_MEDIA);
    checkOutputForStrategy(STRATEGY_DTMF);
}

void AudioPolicyManager::checkA2dpSuspend()
{
    // suspend A2DP output if:
    //      (NOT already suspended) &&
    //      ((SCO device is connected &&
    //       (forced usage for communication || for record is SCO))) ||
    //      (phone state is ringing || in call)
    //
    // restore A2DP output if:
    //      (Already suspended) &&
    //      ((SCO device is NOT connected ||
    //       (forced usage NOT for communication && NOT for record is SCO))) &&
    //      (phone state is NOT ringing && NOT in call)
    //
    if (mA2dpOutput == 0) {
        return;
    }

    if (mA2dpSuspended) {
        if (((mScoDeviceAddress == "") ||
             ((mForceUse[AudioSystem::FOR_COMMUNICATION] != AudioSystem::FORCE_BT_SCO) &&
              (mForceUse[AudioSystem::FOR_RECORD] != AudioSystem::FORCE_BT_SCO))) &&
             ((mPhoneState != AudioSystem::MODE_IN_CALL) &&
              (mPhoneState != AudioSystem::MODE_RINGTONE) &&
			  (mPhoneState != AudioSystem::MODE_IN_VT_CALL))) {

            mpClientInterface->restoreOutput(mA2dpOutput);
            mA2dpSuspended = false;
        }
    } else {
        if (((mScoDeviceAddress != "") &&
             ((mForceUse[AudioSystem::FOR_COMMUNICATION] == AudioSystem::FORCE_BT_SCO) ||
              (mForceUse[AudioSystem::FOR_RECORD] == AudioSystem::FORCE_BT_SCO))) ||
             ((mPhoneState == AudioSystem::MODE_IN_CALL) ||
              (mPhoneState == AudioSystem::MODE_RINGTONE)||
			  (mPhoneState == AudioSystem::MODE_IN_VT_CALL))) {

            mpClientInterface->suspendOutput(mA2dpOutput);
            mA2dpSuspended = true;
        }
    }
}


#endif

// ---
uint32_t AudioPolicyManager::getNewDevice(audio_io_handle_t output, bool fromCache)
{
    uint32_t device = 0;

    AudioOutputDescriptor *outputDesc = mOutputs.valueFor(output);
    if(outputDesc == NULL) {
        LOGE("can not find output desc - %d \n", output);
        return 0;
    }

    // check the following by order of priority to request a routing change if necessary:
    // 1: we are in call or the strategy phone is active on the hardware output:
    //      use device for strategy phone
    // 2: the strategy sonification is active on the hardware output:
    //      use device for strategy sonification
    // 3: the strategy media is active on the hardware output:
    //      use device for strategy media
    // 4: the strategy DTMF is active on the hardware output:
    //      use device for strategy DTMF
    if (isInCall() ||
        outputDesc->isUsedByStrategy(STRATEGY_PHONE)) {
        device = getDeviceForStrategy(STRATEGY_PHONE, fromCache);
    } else if (outputDesc->isUsedByStrategy(STRATEGY_SONIFICATION)) {
        device = getDeviceForStrategy(STRATEGY_SONIFICATION, fromCache);
    } else if (outputDesc->isUsedByStrategy(STRATEGY_MEDIA)) {
        device = getDeviceForStrategy(STRATEGY_MEDIA, fromCache);
    } else if (outputDesc->isUsedByStrategy(STRATEGY_DTMF)) {
        device = getDeviceForStrategy(STRATEGY_DTMF, fromCache);
    } else if(outputDesc->isUsedByStrategy(STRATEGY_FMTRANSMITTER)){
        device = getDeviceForStrategy(STRATEGY_FMTRANSMITTER);
    }

    LOGV("getNewDevice() selected device %x", device);
    return device;
}

#if PLATFORM_SDK_VERSION >= 9
uint32_t AudioPolicyManager::getStrategyForStream(AudioSystem::stream_type stream) {
    return (uint32_t)getStrategy(stream);
}
#endif

AudioPolicyManager::routing_strategy AudioPolicyManager::getStrategy(AudioSystem::stream_type stream)
{
    // stream to strategy mapping
    switch (stream) {
    case AudioSystem::VOICE_CALL:
    case AudioSystem::BLUETOOTH_SCO:
        return STRATEGY_PHONE;
    case AudioSystem::RING:
    case AudioSystem::NOTIFICATION:
    case AudioSystem::ALARM:
    case AudioSystem::ENFORCED_AUDIBLE:
        return STRATEGY_SONIFICATION;
    case AudioSystem::DTMF:
        return STRATEGY_DTMF;
    case AudioSystem::HDMI:
        return STRATEGY_HDMI;
    default:
        LOGE("unknown stream type");
        return NUM_STRATEGIES;
    case AudioSystem::SYSTEM:
        // NOTE: SYSTEM stream uses MEDIA strategy because muting music and switching outputs
        // while key clicks are played produces a poor result
    case AudioSystem::TTS:
        return STRATEGY_MEDIA;
    case AudioSystem::MUSIC:
        if (mAvailableOutputDevices & AudioSystem::DEVICE_OUT_FM_TRANSMITTER)
            return STRATEGY_FMTRANSMITTER;
        return STRATEGY_MEDIA;
    }
}

uint32_t AudioPolicyManager::getDeviceForStrategy(routing_strategy strategy, bool fromCache)
{
    uint32_t device = 0;

	if (fromCache) {
        LOGV("getDeviceForStrategy() from cache strategy %d, device %x", strategy, mDeviceForStrategy[strategy]);
        return mDeviceForStrategy[strategy];
    }

    switch (strategy) {
    case STRATEGY_FMTRANSMITTER:
        device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_FM_TRANSMITTER;
        break;
    case STRATEGY_HDMI:
        device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_HDMI;
        break;
    case STRATEGY_DTMF:
        if (!isInCall()) {
            // when off call, DTMF strategy follows the same rules as MEDIA strategy
            device = getDeviceForStrategy(STRATEGY_MEDIA, false);
            break;
        }
        // when in call, DTMF and PHONE strategies follow the same rules
        // FALL THROUGH

    case STRATEGY_PHONE:
        // for phone strategy, we first consider the forced use and then the available devices by order
        // of priority
        switch (mForceUse[AudioSystem::FOR_COMMUNICATION]) {
        case AudioSystem::FORCE_BT_SCO:
            if (!isInCall() || strategy != STRATEGY_DTMF) {
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_CARKIT;
                if (device) break;
            }
            device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET;
            if (device) break;
            device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO;
            if (device) break;
            // if SCO device is requested but no SCO device is available, fall back to default case
            // FALL THROUGH

        default:    // FORCE_NONE
            device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_WIRED_HEADPHONE;
            if (device) break;
            device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_WIRED_HEADSET;
            if (device) break;
#ifdef WITH_A2DP
            // when not in a phone call, phone strategy should route STREAM_VOICE_CALL to A2DP
            if (!isInCall()) {
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP;
                if (device) break;
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES;
                if (device) break;
            }
#endif
            device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_EARPIECE;
            if (device == 0) {
                LOGE("getDeviceForStrategy() earpiece device not found");
            }
            break;

        case AudioSystem::FORCE_SPEAKER:
            if (!isInCall() || strategy != STRATEGY_DTMF) {
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_CARKIT;
                if (device) break;
            }
#ifdef WITH_A2DP
            // when not in a phone call, phone strategy should route STREAM_VOICE_CALL to
            // A2DP speaker when forcing to speaker output
            if (!isInCall()) {
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER;
                if (device) break;
            }
#endif
            device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_SPEAKER;
            if (device == 0) {
                LOGE("getDeviceForStrategy() speaker device not found");
            }
            break;
        }
    break;

    case STRATEGY_SONIFICATION:

        // If incall, just select the STRATEGY_PHONE device: The rest of the behavior is handled by
        // handleIncallSonification().
        if (isInCall()) {
            device = getDeviceForStrategy(STRATEGY_PHONE);
            break;
        }

	 device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_SPEAKER;
	 if (device == 0) {
		 LOGE("getDeviceForStrategy() speaker device not found");
	 }
        // The second device used for sonification is the same as the device used by media strategy
        // FALL THROUGH

    case STRATEGY_MEDIA: {
        uint32_t device2 = 0;
        if (mForceUse[AudioSystem::FOR_MEDIA] == AudioSystem::FORCE_SPEAKER){
            device2 = mAvailableOutputDevices & AudioSystem:: DEVICE_OUT_SPEAKER;
        }
        if (device2 == 0){
            device2 = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_AUX_DIGITAL;
        }
#ifdef AUDIO_WITH_HDMI
		if (device2 == 0)
            device2 = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_HDMI;
#endif
        if (device2 == 0) {
            device2 = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_WIRED_HEADPHONE;
        }
        if (device2 == 0) {
            device2 = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_WIRED_HEADSET;
        }
#ifdef WITH_A2DP
        if (mA2dpOutput != 0) {
            if (strategy == STRATEGY_SONIFICATION && !a2dpUsedForSonification()) {
                break;
            }
            if (device2 == 0) {
                device2 = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP;
            }
            if (device2 == 0) {
                device2 = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES;
            }
            if (device2 == 0) {
                device2 = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER;
            }
        }
#endif
        if (device2 == 0) {
            device2 = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_SPEAKER;
        }

        // device is DEVICE_OUT_SPEAKER if we come from case STRATEGY_SONIFICATION, 0 otherwise
        device |= device2;
        if (device == 0) {
            LOGE("getDeviceForStrategy() speaker device not found");
		}
        } break;
    default:
        LOGW("getDeviceForStrategy() unknown strategy: %d", strategy);
        break;
    }

    LOGV("getDeviceForStrategy() strategy %d, device %x", strategy, device);
    return device;
}

void AudioPolicyManager::updateDeviceForStrategy()
{
    for (int i = 0; i < NUM_STRATEGIES; i++) {
        mDeviceForStrategy[i] = getDeviceForStrategy((routing_strategy)i, false);
    }
}

void AudioPolicyManager::setOutputDevice(audio_io_handle_t output, uint32_t device, bool force, int delayMs)
{
    LOGV("setOutputDevice() output %d device %x delayMs %d", output, device, delayMs);
    if (mOutputs.indexOfKey(output) < 0) {
        LOGW("setOutputDevice() unknown output %d", output);
        return;
    }

    AudioOutputDescriptor *outputDesc = mOutputs.valueFor(output);

    if (outputDesc->isDuplicated()) {
        setOutputDevice(outputDesc->mOutput1->mId, device, force, delayMs);
        setOutputDevice(outputDesc->mOutput2->mId, device, force, delayMs);
        return;
    }
#ifdef WITH_A2DP
    // filter devices according to output selected
    if (output == mA2dpOutput) {
        device &= AudioSystem::DEVICE_OUT_ALL_A2DP;
    } else {
        device &= ~AudioSystem::DEVICE_OUT_ALL_A2DP;
    }
#endif

	uint32_t prevDevice = (uint32_t)outputDesc->device();
    // Do not change the routing if:
    //  - the requestede device is 0
    //  - the requested device is the same as current device and force is not specified.
    // Doing this check here allows the caller to call setOutputDevice() without conditions
    if ((device == 0 || device == prevDevice) && !force) {
        LOGV("setOutputDevice() setting same device %x or null device for output %d", device, output);
        return;
    }

    outputDesc->mDevice = device;
    // mute media streams if both speaker and headset are selected
    if (output == mHardwareOutput && AudioSystem::popCount(device) == 2) {
        setStrategyMute(STRATEGY_MEDIA, true, output);
        // wait for the PCM output buffers to empty before proceeding with the rest of the command
        usleep(mOutputs.valueFor(output)->mLatency*2*1000);
    }

    // do the routing
    AudioParameter param = AudioParameter();
    param.addInt(String8(AudioParameter::keyRouting), (int)device);
    param.addInt(String8(PHONE_STATE), (int)mPhoneState);
	if((mPhoneState == AudioSystem::MODE_IN_CALL) || (mPhoneState == AudioSystem::MODE_IN_VT_CALL))
	{
		if(device & DEVICE_OUT_BLUETOOTH_SCO_HEADSET)
			param.addInt(String8(PHONE_VOLUME), 255);
		else
			param.addInt(String8(PHONE_VOLUME), (int)((float)mStreams[0].mIndexCur/(float)mStreams[0].mIndexMax * 255));
	} else {
		//normal condition - 230 : (230+230)*10/51 = 90
		param.addInt(String8(PHONE_VOLUME), 230);
	}

 #ifdef WITH_SRSWITCH
	// change the sample rate
	if(mPhoneState == AudioSystem::MODE_IN_VT_CALL) {
		if(mOutputs.valueFor(mHardwareOutput)->mSamplingRate != VT_OUTPUT_SAMPLE_RATE) {
			param.addInt(String8(AudioParameter::keySamplingRate), VT_OUTPUT_SAMPLE_RATE);
		}
	} else if (mOutputs.valueFor(mHardwareOutput)->mSamplingRate != DEFAULT_OUTPUT_SAMPLE_RATE) {
			param.addInt(String8(AudioParameter::keySamplingRate), DEFAULT_OUTPUT_SAMPLE_RATE);
	}
#endif

	mpClientInterface->setParameters(output, param.toString(), delayMs);

#ifdef WITH_SRSWITCH
	//update the sample rate
	if((mPhoneState == AudioSystem::MODE_IN_VT_CALL)) {
		if(mOutputs.valueFor(mHardwareOutput)->mSamplingRate != VT_OUTPUT_SAMPLE_RATE) {
		mOutputs.valueFor(mHardwareOutput)->mSamplingRate = VT_OUTPUT_SAMPLE_RATE;
		mOutputs.valueFor(mHardwareOutput)->mChannels = AudioSystem::CHANNEL_OUT_STEREO;}
	} else if (mOutputs.valueFor(mHardwareOutput)->mSamplingRate != DEFAULT_OUTPUT_SAMPLE_RATE) {
		mOutputs.valueFor(mHardwareOutput)->mSamplingRate = DEFAULT_OUTPUT_SAMPLE_RATE;
		mOutputs.valueFor(mHardwareOutput)->mChannels = AudioSystem::CHANNEL_OUT_STEREO;
	}
#endif
	// update stream volumes according to new device
    applyStreamVolumes(output, device, delayMs);

	// if changing from a combined headset + speaker route, unmute media streams
    if (output == mHardwareOutput && AudioSystem::popCount(prevDevice) == 2) {
        setStrategyMute(STRATEGY_MEDIA, false, output, delayMs);
    }
}

uint32_t AudioPolicyManager::getDeviceForInputSource(int inputSource)
{
    uint32_t device;

    switch(inputSource) {
    case AUDIO_SOURCE_DEFAULT:
    case AUDIO_SOURCE_MIC:
    case AUDIO_SOURCE_VOICE_RECOGNITION:
    case AUDIO_SOURCE_VOICE_COMMUNICATION:
        if (mForceUse[AudioSystem::FOR_RECORD] == AudioSystem::FORCE_BT_SCO &&
            mAvailableInputDevices & AudioSystem::DEVICE_IN_BLUETOOTH_SCO_HEADSET) {
            device = AudioSystem::DEVICE_IN_BLUETOOTH_SCO_HEADSET;
        } else if (mAvailableInputDevices & AudioSystem::DEVICE_IN_WIRED_HEADSET) {
            device = AudioSystem::DEVICE_IN_WIRED_HEADSET;
        } else {
            device = AudioSystem::DEVICE_IN_BUILTIN_MIC;
        }
        break;
    case AUDIO_SOURCE_CAMCORDER:
        if (mAvailableInputDevices & AudioSystem::DEVICE_IN_WIRED_HEADSET) {
            device = AudioSystem::DEVICE_IN_WIRED_HEADSET;
        } else if (hasBackMicrophone()) {
            device = AudioSystem::DEVICE_IN_BACK_MIC;
		} else {
            device = AudioSystem::DEVICE_IN_BUILTIN_MIC;
		}
        break;
    case AUDIO_SOURCE_VOICE_UPLINK:
    case AUDIO_SOURCE_VOICE_DOWNLINK:
    case AUDIO_SOURCE_VOICE_CALL:
        device = AudioSystem::DEVICE_IN_VOICE_CALL;
        break;
    case AUDIO_SOURCE_FMRADIO:
        device = AudioSystem::DEVICE_IN_FMRADIO;
    break;
	case AUDIO_SOURCE_VOICE_VT_CALL:
		device = AudioSystem::DEVICE_IN_VT_MIC;
		break;
    default:
        LOGW("getInput() invalid input source %d", inputSource);
        device = 0;
        break;
    }
    return device;
}

audio_io_handle_t AudioPolicyManager::getActiveInput()
{
    for (size_t i = 0; i < mInputs.size(); i++) {
        if (mInputs.valueAt(i)->mRefCount > 0) {
            return mInputs.keyAt(i);
        }
    }
    return 0;
}

float AudioPolicyManager::computeVolume(int stream, int index, audio_io_handle_t output, uint32_t device)
{
    float volume = 1.0;
    AudioOutputDescriptor *outputDesc = mOutputs.valueFor(output);
    StreamDescriptor &streamDesc = mStreams[stream];

    // Force max volume if stream cannot be muted
    if (!streamDesc.mCanBeMuted) index = streamDesc.mIndexMax;

    if (device == 0) {
        device = outputDesc->device();
    }

    int volInt = (100 * (index - streamDesc.mIndexMin)) / (streamDesc.mIndexMax - streamDesc.mIndexMin);
    volume = AudioSystem::linearToLog(volInt);

    // if a heaset is connected, apply the following rules to ring tones and notifications
    // to avoid sound level bursts in user's ears:
    // - always attenuate ring tones and notifications volume by 6dB
    // - if music is playing, always limit the volume to current music volume,
    // with a minimum threshold at -36dB so that notification is always perceived.
    if ((device &
        (AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP |
        AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES |
        AudioSystem::DEVICE_OUT_WIRED_HEADSET |
        AudioSystem::DEVICE_OUT_WIRED_HEADPHONE)) &&
        (getStrategy((AudioSystem::stream_type)stream) == STRATEGY_SONIFICATION)) {
        volume *= SONIFICATION_HEADSET_VOLUME_FACTOR;
        // when the phone is ringing we must consider that music could have been paused just before
        // by the music application and behave as if music was active if the last music track was
        // just stopped
        if (outputDesc->mRefCount[AudioSystem::MUSIC] || mLimitRingtoneVolume) {
            float musicVol = computeVolume(AudioSystem::MUSIC, mStreams[AudioSystem::MUSIC].mIndexCur, output, device);
            float minVol = (musicVol > SONIFICATION_HEADSET_VOLUME_MIN) ? musicVol : SONIFICATION_HEADSET_VOLUME_MIN;
            if (volume > minVol) {
                volume = minVol;
                LOGV("computeVolume limiting volume to %f musicVol %f", minVol, musicVol);
            }
        }
    }

    return volume;
}

status_t AudioPolicyManager::checkAndSetVolume(int stream, int index, audio_io_handle_t output, uint32_t device, int delayMs, bool force)
{
    // do not change actual stream volume if the stream is muted
    if (mOutputs.valueFor(output)->mMuteCount[stream] != 0) {
        LOGV("checkAndSetVolume() stream %d muted count %d", stream, mOutputs.valueFor(output)->mMuteCount[stream]);
        return NO_ERROR;
    }

    if (stream == AudioSystem::FM){
        StreamDescriptor &streamDesc = mStreams[stream];
        int volInt = (255 * (index - streamDesc.mIndexMin)) / (streamDesc.mIndexMax - streamDesc.mIndexMin);
        LOGV("checkAndSetVolume() FM Set volume %d", volInt);
        SET_FM_VOLUME(volInt);
    }

    // do not change in call volume if bluetooth is connected and vice versa
    if ((stream == AudioSystem::VOICE_CALL && mForceUse[AudioSystem::FOR_COMMUNICATION] == AudioSystem::FORCE_BT_SCO) ||
        (stream == AudioSystem::BLUETOOTH_SCO && mForceUse[AudioSystem::FOR_COMMUNICATION] != AudioSystem::FORCE_BT_SCO)) {
        LOGV("checkAndSetVolume() cannot set stream %d volume with force use = %d for comm",
             stream, mForceUse[AudioSystem::FOR_COMMUNICATION]);
        return INVALID_OPERATION;
    }

    float volume = computeVolume(stream, index, output, device);
    // We actually change the volume if:
    // - the float value returned by computeVolume() changed
    // - the force flag is set
    if (volume != mOutputs.valueFor(output)->mCurVolume[stream] ||
            force) {
        mOutputs.valueFor(output)->mCurVolume[stream] = volume;
        LOGV("setStreamVolume() for output %d stream %d, volume %f, delay %d", output, stream, volume, delayMs);
        if (stream == AudioSystem::VOICE_CALL ||
            stream == AudioSystem::DTMF ||
            stream == AudioSystem::BLUETOOTH_SCO) {
            // offset value to reflect actual hardware volume that never reaches 0
            // 1% corresponds roughly to first step in VOICE_CALL stream volume setting (see AudioService.java)
            volume = 0.01 + 0.99 * volume;
        }
#ifndef VOLUME_CALIBRATION
        mpClientInterface->setStreamVolume((AudioSystem::stream_type)stream, volume, output, delayMs);
#else
        mpClientInterface->setStreamVolume((AudioSystem::stream_type)stream, index, output, delayMs);
#endif
    }

    if (stream == AudioSystem::VOICE_CALL ||
        stream == AudioSystem::BLUETOOTH_SCO) {
        float voiceVolume;
        // Force voice volume to max for bluetooth SCO as volume is managed by the headset
        if ((stream == AudioSystem::VOICE_CALL) || isInVTCall()) {
            voiceVolume = (float)index/(float)mStreams[stream].mIndexMax;
        } else {
            voiceVolume = 1.0;
        }
        if (voiceVolume != mLastVoiceVolume && output == mHardwareOutput) {
            mpClientInterface->setVoiceVolume(voiceVolume, delayMs);
            mLastVoiceVolume = voiceVolume;
        }
    }
    return NO_ERROR;
}

void AudioPolicyManager::applyStreamVolumes(audio_io_handle_t output, uint32_t device, int delayMs)
{
    LOGV("applyStreamVolumes() for output %d and device %x", output, device);

    for (int stream = 0; stream < AudioSystem::NUM_STREAM_TYPES; stream++) {
        checkAndSetVolume(stream, mStreams[stream].mIndexCur, output, device, delayMs);
    }
}

void AudioPolicyManager::setStrategyMute(routing_strategy strategy, bool on, audio_io_handle_t output, int delayMs)
{
    LOGV("setStrategyMute() strategy %d, mute %d, output %d", strategy, on, output);
    for (int stream = 0; stream < AudioSystem::NUM_STREAM_TYPES; stream++) {
        if (getStrategy((AudioSystem::stream_type)stream) == strategy) {
            setStreamMute(stream, on, output, delayMs);
        }
    }
}

void AudioPolicyManager::setStreamMute(int stream, bool on, audio_io_handle_t output, int delayMs)
{
    StreamDescriptor &streamDesc = mStreams[stream];
    AudioOutputDescriptor *outputDesc = mOutputs.valueFor(output);

    LOGV("setStreamMute() stream %d, mute %d, output %d, mMuteCount %d", stream, on, output, outputDesc->mMuteCount[stream]);

    if (on) {
        if (outputDesc->mMuteCount[stream] == 0) {
            if (streamDesc.mCanBeMuted) {
                checkAndSetVolume(stream, 0, output, outputDesc->device(), delayMs);
            }
        }
        // increment mMuteCount after calling checkAndSetVolume() so that volume change is not ignored
        outputDesc->mMuteCount[stream]++;
    } else {
        if (outputDesc->mMuteCount[stream] == 0) {
            LOGW("setStreamMute() unmuting non muted stream!");
            return;
        }
        if (--outputDesc->mMuteCount[stream] == 0) {
            checkAndSetVolume(stream, streamDesc.mIndexCur, output, outputDesc->device(), delayMs);
        }
    }
}



void AudioPolicyManager::handleIncallSonification(int stream, bool starting, bool stateChange)
{
    // if the stream pertains to sonification strategy and we are in call we must
    // mute the stream if it is low visibility. If it is high visibility, we must play a tone
    // in the device used for phone strategy and play the tone if the selected device does not
    // interfere with the device used for phone strategy
    // if stateChange is true, we are called from setPhoneState() and we must mute or unmute as
    // many times as there are active tracks on the output

    if (getStrategy((AudioSystem::stream_type)stream) == STRATEGY_SONIFICATION) {
        AudioOutputDescriptor *outputDesc = mOutputs.valueFor(mHardwareOutput);
        LOGV("handleIncallSonification() stream %d starting %d device %x stateChange %d",
                stream, starting, outputDesc->mDevice, stateChange);
        if (outputDesc->mRefCount[stream]) {
            int muteCount = 1;
            if (stateChange) {
                muteCount = outputDesc->mRefCount[stream];
            }
            if (AudioSystem::isLowVisibility((AudioSystem::stream_type)stream)) {
                LOGV("handleIncallSonification() low visibility, muteCount %d", muteCount);
                for (int i = 0; i < muteCount; i++) {
                    setStreamMute(stream, starting, mHardwareOutput);
                }
            } else {
                LOGV("handleIncallSonification() high visibility ");
                if (outputDesc->mDevice & getDeviceForStrategy(STRATEGY_PHONE)) {
                    LOGV("handleIncallSonification() high visibility muted, muteCount %d", muteCount);
                    for (int i = 0; i < muteCount; i++) {
                        setStreamMute(stream, starting, mHardwareOutput);
                    }
                }
                if (starting) {
                    mpClientInterface->startTone(ToneGenerator::TONE_SUP_CALL_WAITING, AudioSystem::VOICE_CALL);
                } else {
                    mpClientInterface->stopTone();
                }
            }
        }
    }
}

bool AudioPolicyManager::isInCall()
{
    return isStateInCall(mPhoneState);
}

bool AudioPolicyManager::isInVTCall()
{
    return isStateInVTCall(mPhoneState);
}

bool AudioPolicyManager::isStateInCall(int state)
{
    return ((state == AudioSystem::MODE_IN_CALL) ||
            (state == AudioSystem::MODE_IN_COMMUNICATION) ||
	        (state == AudioSystem::MODE_IN_VT_CALL));
}

bool AudioPolicyManager::isStateInVTCall(int state)
{
    return (state == AudioSystem::MODE_IN_VT_CALL);
}

bool AudioPolicyManager::needsDirectOuput(AudioSystem::stream_type stream,
                                    uint32_t samplingRate,
                                    uint32_t format,
                                    uint32_t channels,
                                    AudioSystem::output_flags flags,
                                    uint32_t device)
{
   return ((flags & AudioSystem::OUTPUT_FLAG_DIRECT) ||
          (format !=0 && !AudioSystem::isLinearPCM(format)));
}

// --- AudioOutputDescriptor class implementation

AudioPolicyManager::AudioOutputDescriptor::AudioOutputDescriptor()
    : mSamplingRate(0), mFormat(0), mChannels(0), mLatency(0),
    mFlags((AudioSystem::output_flags)0), mDevice(0), mOutput1(0), mOutput2(0)
{
    // clear usage count for all stream types
    for (int i = 0; i < AudioSystem::NUM_STREAM_TYPES; i++) {
        mRefCount[i] = 0;
        mCurVolume[i] = -1.0;
        mMuteCount[i] = 0;
    }
}

uint32_t AudioPolicyManager::AudioOutputDescriptor::device()
{
    uint32_t device = 0;
    if (isDuplicated()) {
        device = mOutput1->mDevice | mOutput2->mDevice;
    } else {
        device = mDevice;
    }
    return device;
}

void AudioPolicyManager::AudioOutputDescriptor::changeRefCount(AudioSystem::stream_type stream, int delta)
{
    // forward usage count change to attached outputs
    if (isDuplicated()) {
        mOutput1->changeRefCount(stream, delta);
        mOutput2->changeRefCount(stream, delta);
    }
    if ((delta + (int)mRefCount[stream]) < 0) {
        LOGW("changeRefCount() invalid delta %d for stream %d, refCount %d", delta, stream, mRefCount[stream]);
        mRefCount[stream] = 0;
        return;
    }
    mRefCount[stream] += delta;
    LOGV("changeRefCount() stream %d, count %d", stream, mRefCount[stream]);
}

uint32_t AudioPolicyManager::AudioOutputDescriptor::refCount()
{
    uint32_t refcount = 0;
    for (int i = 0; i < (int)AudioSystem::NUM_STREAM_TYPES; i++) {
        refcount += mRefCount[i];
    }
    return refcount;
}

uint32_t AudioPolicyManager::AudioOutputDescriptor::strategyRefCount(routing_strategy strategy)
{
    uint32_t refCount = 0;
    for (int i = 0; i < (int)AudioSystem::NUM_STREAM_TYPES; i++) {
        if (getStrategy((AudioSystem::stream_type)i) == strategy) {
            refCount += mRefCount[i];
        }
    }
    return refCount;
}


status_t AudioPolicyManager::AudioOutputDescriptor::dump(int fd)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;

    snprintf(buffer, SIZE, " Sampling rate: %d\n", mSamplingRate);
    result.append(buffer);
    snprintf(buffer, SIZE, " Format: %d\n", mFormat);
    result.append(buffer);
    snprintf(buffer, SIZE, " Channels: %08x\n", mChannels);
    result.append(buffer);
    snprintf(buffer, SIZE, " Latency: %d\n", mLatency);
    result.append(buffer);
    snprintf(buffer, SIZE, " Flags %08x\n", mFlags);
    result.append(buffer);
    snprintf(buffer, SIZE, " Devices %08x\n", mDevice);
    result.append(buffer);
    snprintf(buffer, SIZE, " Stream volume refCount muteCount\n");
    result.append(buffer);
    for (int i = 0; i < AudioSystem::NUM_STREAM_TYPES; i++) {
        snprintf(buffer, SIZE, " %02d     %.03f     %02d       %02d\n", i, mCurVolume[i], mRefCount[i], mMuteCount[i]);
        result.append(buffer);
    }
    write(fd, result.string(), result.size());

    return NO_ERROR;
}

// --- AudioInputDescriptor class implementation

AudioPolicyManager::AudioInputDescriptor::AudioInputDescriptor()
    : mSamplingRate(0), mFormat(0), mChannels(0),
     mAcoustics((AudioSystem::audio_in_acoustics)0), mDevice(0), mRefCount(0), mInputSource(0)
{
}

status_t AudioPolicyManager::AudioInputDescriptor::dump(int fd)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;

    snprintf(buffer, SIZE, " Sampling rate: %d\n", mSamplingRate);
    result.append(buffer);
    snprintf(buffer, SIZE, " Format: %d\n", mFormat);
    result.append(buffer);
    snprintf(buffer, SIZE, " Channels: %08x\n", mChannels);
    result.append(buffer);
    snprintf(buffer, SIZE, " Acoustics %08x\n", mAcoustics);
    result.append(buffer);
    snprintf(buffer, SIZE, " Devices %08x\n", mDevice);
    result.append(buffer);
    snprintf(buffer, SIZE, " Ref Count %d\n", mRefCount);
    result.append(buffer);
    write(fd, result.string(), result.size());

    return NO_ERROR;
}

// --- StreamDescriptor class implementation

void AudioPolicyManager::StreamDescriptor::dump(char* buffer, size_t size)
{
    snprintf(buffer, size, "      %02d         %02d         %02d          %d\n",
            mIndexMin,
            mIndexMax,
            mIndexCur,
            mCanBeMuted);
}


}; // namespace android
