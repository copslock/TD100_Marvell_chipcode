/* GStreamer
 * Copyright (C) <2009> Prajnashi S <prajnashi@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef ANDROID_CMMBPLAYENGINE_H
#define ANDROID_CMMBPLAYENGINE_H

#include <utils/Errors.h>
#include <utils/String8.h>
#include <media/MediaPlayerInterface.h>

#define CMMB_PLAYENGINE_CONFIG_FILE "/system/etc/cmmbplayengine.conf"

class CmmbPlayEnginePipeline;

enum PlayEngineIndication {
    UNKNOWN_INDICATION,
    REQUEST_SHAREDBUFFER_FD,
    GOT_SHAREDBUFFER_FD,
    IMAGE_CAPTURE_DONE,
    IMAGE_CAPTURE_FAIL
};
struct PipelineAndFd{
	CmmbPlayEnginePipeline *pipe;
	int fd;
};

namespace android {

//**********
class LogMask {
public:
    enum LogType{
        ERROR = 0,
        WARNING = 1,
        MAIN = 2,
        PLAYENGINE = 3,

        AIDL = 4,
        PIPELINE = 5,
        DUMP_AV = 6,
        SERVICE = 7,

        ENGINE = 8,
        AGENT = 9,
        RECORD = 10,
        SEMAPHORE = 11,

        STATISTICS = 12,
        SESSION = 13,
        CMD_PATH = 14,
        DATA_MFS = 15,

        DATA_NAL = 16,
        DATA_AAC = 17,
        SIGNAL = 18,
        SIM = 19,

        CMMBSTACK = 20,
        MBBMSSTACK = 21,
        FIRMWARE_ERROR = 22,
        FIRMWARE_DEBUG = 23,

        FIRMWARE_POWER = 24,
    };
    LogMask();
    virtual ~LogMask();
    static int readConfigure();
    static int writeConfigure();
    static char masks[32];
};

#define LOGM_LOAD_MASKS()  LogMask::readConfigure()
#define LOGM_IS_ENABLED(type)    LogMask::masks[type]

#define LOGM(type, format, ...) \
    if (LOGM_IS_ENABLED(type)) { \
        LOGD("[%s][%s():%d]"format, #type, __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
    }

//**********

class CmmbPlayEngine: public MediaPlayerInterface {
public:
    CmmbPlayEngine();
    virtual ~CmmbPlayEngine();

    virtual status_t initCheck();
    virtual status_t setAudioStreamType(int type);
#if PLATFORM_SDK_VERSION >=8
    virtual status_t setDataSource(
            const char *url,
            const KeyedVector<String8, String8> *headers = NULL);
#else
    virtual status_t setDataSource(const char *url);
#endif
    virtual status_t setDataSource(int fd, int64_t offset, int64_t length);
    virtual status_t setVideoSurface(const sp<ISurface>& surface);
    virtual status_t prepare();
    virtual status_t prepareAsync();
    virtual status_t start();
    virtual status_t stop();
    virtual status_t pause();
    virtual bool isPlaying();
    virtual status_t getVideoWidth(int *w);
    virtual status_t getVideoHeight(int *h);
    virtual status_t seekTo(int msec);
    virtual status_t getCurrentPosition(int *msec);
    virtual status_t getDuration(int *msec);
    virtual status_t reset();
    virtual status_t setLooping(int loop);
    virtual player_type playerType() {
        return GST_PLAYER;
    }
    int startCommandThread();
    int stopCommandThread();
    static void* recv_command_thread(void* data);

    // make available to CmmbPlayEnginePipeline
    void sendEvent(int msg, int ext1 = 0, int ext2 = 0) {
        MediaPlayerBase::sendEvent(msg, ext1, ext2);
    }

#if PLATFORM_SDK_VERSION >= 5
    // Invoke a generic method on the player by using opaque parcels
    // for the request and reply.
    //
    // @param request Parcel that is positioned at the start of the
    //                data sent by the java layer.
    // @param[out] reply Parcel to hold the reply data. Cannot be null.
    // @return OK if the call was successful.
    virtual status_t invoke(const Parcel& request, Parcel *reply);

    // The Client in the MetadataPlayerService calls this method on
    // the native player to retrieve all or a subset of metadata.
    //
    // @param ids SortedList of metadata ID to be fetch. If empty, all
    //            the known metadata should be returned.
    // @param[inout] records Parcel where the player appends its metadata.
    // @return OK if the call was successful.
    virtual status_t getMetadata(const media::Metadata::Filter& ids, Parcel *records);
#endif

#if PLATFORM_SDK_VERSION >=8
    virtual status_t suspend() {return INVALID_OPERATION;}
    virtual status_t resume() {return INVALID_OPERATION;}
#endif
private:

    static int onCommand_fd(int session, int fd);
    static int onCommand_capture(void* pipe,int fd);
    static int onCommand_flush();
    static int sendIndication(int fd, PlayEngineIndication indication, const char* cookie);
    CmmbPlayEnginePipeline* mCmmbPlayEnginePipeline;
    bool mIsDataSourceSet;
    int mSession;
    sp<ISurface> mSurface;
    status_t mInit;
    int mDuration;
    int mStreamType;
    static int mSharedFds[2];
    int mSocket;
//    pthread_t mThreadCommand;
    static bool mCmdThreadStarted;
    static CmmbPlayEnginePipeline *pipeline[2];
};

}
; // namespace android

#endif // ANDROID_CMMBPLAYENGINE_H

