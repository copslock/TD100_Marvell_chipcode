/*
* (C) Copyright 2010 Marvell International Ltd.
* All Rights Reserved
*
* MARVELL CONFIDENTIAL
* Copyright 2008 ~ 2010 Marvell International Ltd All Rights Reserved.
* The source code contained or described herein and all documents related to
* the source code ("Material") are owned by Marvell International Ltd or its
* suppliers or licensors. Title to the Material remains with Marvell International Ltd
* or its suppliers and licensors. The Material contains trade secrets and
* proprietary and confidential information of Marvell or its suppliers and
* licensors. The Material is protected by worldwide copyright and trade secret
* laws and treaty provisions. No part of the Material may be used, copied,
* reproduced, modified, published, uploaded, posted, transmitted, distributed,
* or disclosed in any way without Marvell's prior express written permission.
*
* No license under any patent, copyright, trade secret or other intellectual
* property right is granted to or conferred upon you by disclosure or delivery
* of the Materials, either expressly, by implication, inducement, estoppel or
* otherwise. Any license under such intellectual property rights must be
* express and approved by Marvell in writing.
*
*/


#ifndef FMRADIO_H
#define FMRADIO_H

#include <utils/Log.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <utils/threads.h>
#include <linux/videodev2.h>
#include <binder/IPCThreadState.h>
#include <media/AudioTrack.h>
#include <media/stagefright/AudioPlayer.h>
#include <media/stagefright/MediaDebug.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/MediaSource.h>
#include <media/mediarecorder.h>
#include <media/AudioRecord.h>
#include <media/AudioSystem.h>
#include <media/stagefright/MetaData.h>
#include <linux/videodev2.h>
#include <linux/videodev.h>
#include <cutils/properties.h>
#include <utils/KeyedVector.h>
#ifndef FM_NOT_USES_RECORD //record mode
#include "utils/FMRingBuffer.h"
/* Standard IPP headers */
#include "ippdefs.h"
#include "codecAC.h"
#include "misc.h"
#include "ippLV.h"
#include <utils/threads.h>
#endif

#include <media/AudioSystem.h>
#include <signal.h>
#include <poll.h>
#include "IFMRadioService.h"
#include "IFMRadioCallback.h"

namespace android{
#define ANDROID_SET_AID_AND_CAP

class FMRadio : public BnFMRadioService, public IBinder::DeathRecipient {
public:
    enum mrv8787_cmd {    
        MRVL8787_CID_BASE = (V4L2_CID_PRIVATE_BASE),
        MRVL8787_SET_FM_BAND = 0x13,
        MRVL8787_SET_CHANNEL = 0x03,
        MRVL8787_GET_CHANNEL = 0x04,
        MRVL8787_GET_CURRENT_RSSI = 0x34,
        MRVL8787_SET_SEARCH_MODE = 0x09,
        MRVL8787_SET_SEARCH_DIRECTION = 0x07,
        MRVL8787_AUDIO_VOLUME = 0x65,
        MRVL8787_AUDIO_MUTE = 0x15,
        MRVL8787_STOP_SEARCH = 0x57,
        // FM Tx
        MRVL8787_SET_POWER_MODE = 0x5D,
        MRVL8787_SET_MUTE = 0x47,
        MRVL8787_SET_TX_MONO_STEREO = 0x46,
        // Mode setting
        MRVL8787_CID_MODE = 0x100
    };

#ifndef FM_NOT_USES_RECORD //record mode
    enum audio_buf_parameter {
        SAMPLE_RATE_RECORD = 48000,
        SAMPLE_RATE_PLAY = 44100,
        CHANNEL_COUNT = 2,
        CHUNK_SIZE = (1024)
    };
    static FMRingBuffer *mFMBuffer;
#endif

    // If change below values, do remember change that in FMManager.java
    enum recording_stop_reason {
        INTERNAL_ERROR = 1, //Media Recoder instance has error.
        MAX_FILESIZE_REACHED = 2, //Record file exceeded max lengh.
        UNKONW_ERROR = 3, //Unkown error
    };

    // exported APIs
    bool isFMTx();
    bool isFMRx();
    int powerOn();
    int powerOff();
    int enableTx();
    int disableTx();
    int enableRx();
    int disableRx();
    int suspend();
    int resume();

    int scan_all();
    int stop_scan();
    int set_channel(unsigned int freq);
    int get_channel();
    int get_rssi();
    float get_volume();
    int set_volume(int volume);
    bool set_mute(bool flag);
    int set_band(int band);
    int scan_next();
    int scan_prev();
    int setSpeakerOn(bool on);
    bool start_recording(int fd);
    bool stop_recording();
    int registerCallback(const sp<IFMRadioCallback>& callback);    
    int unregisterCallback(const sp<IFMRadioCallback>& callback);

    // FM Tx APIs
    bool set_tx_mute(bool flag);
    bool set_power_mode(bool flag);
    bool set_tx_mono_stereo(bool flag);

    // internal stactic functions
    static void signal_action(int signum, siginfo_t *info, void *p);
    static void instantiate();
    static FMRadio* getInstance();
    
    // Protect mCallbacks.
    mutable Mutex   mCallbackLock;      
    KeyedVector<wp<IBinder>, sp<IFMRadioCallback> >  mCallbacks;    
    virtual     void        binderDied(const wp<IBinder>& who);
   
private:
    FMRadio();
    ~FMRadio();

    enum thread_status{
        T_STATUS_SUSPEND,   // thread is waiting for commands "start" or "exit"
        T_STATUS_START,     // thread is polling events
        T_STATUS_EXIT,      // thread is down
    };

    bool isPowerOn;
    bool isTx;
    bool isRx;
    bool isScanning;
    bool isSpeaker;
    bool isDefferUnmuted;
    static int mfd;
    unsigned int mMinFrequency;
    unsigned int mMaxFrequency;
    // Protect all internal state.
    mutable Mutex mLock;
    // Only let one thread to stop scan
    mutable Mutex mLock_stopScan; 
    
    static Mutex mInstanceLock;
    static FMRadio* mInstance;
    static const char* radioDev;
 
    class EventThread;
    EventThread*  mEventThread;

#ifndef FM_NOT_USES_RECORD  //record mode
    class FMMediaRecordThread;
    FMMediaRecordThread *mFMMediaRecordThread;
    class FMRecordThread;
    FMRecordThread *mFMRecordThread;
    class FMPlayerThread;
    FMPlayerThread *mFMPlayerThread;
#else      //playback mode
    class RecordListener;
    sp<RecordListener> mRecordListener;
    sp<MediaRecorder> mRecorder; //FM recorder instance
    // 8787 already muted by user
    bool isMutedByUser;
#endif	
    
    int radio_set_ctrl(int ctrl_type, int value);
    int radio_get_ctrl(int ctrl_type);
#ifdef ANDROID_SET_AID_AND_CAP    
    void android_set_aid_and_cap();
    void print_process_info();
#endif
    bool stop_recording_locked();
    bool start_recording_locked(int fd);
    void get_record_format(char **format);
};
};//end android
#endif // FMRADIO_H

