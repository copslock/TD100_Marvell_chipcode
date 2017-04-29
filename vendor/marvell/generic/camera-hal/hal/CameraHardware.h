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

#ifndef ANDROID_HARDWARE_CAMERA_HARDWARE_H
#define ANDROID_HARDWARE_CAMERA_HARDWARE_H

#include <utils/threads.h>
#if PLATFORM_SDK_VERSION >= 8
#include <camera/CameraHardwareInterface.h>
#else
#include <ui/CameraHardwareInterface.h>
#endif
#include <binder/MemoryBase.h>
#include <binder/MemoryHeapBase.h>
#include <utils/threads.h>

#include <binder/MemoryHeapPmem.h>
#include <linux/android_pmem.h>
#include <cutils/properties.h>
#include "CameraCtrl.h"

namespace android {

    class CameraPreviewInterface;
    extern sp<CameraHardware> getInstance(wp<CameraHardwareInterface> camera);

    class CameraHardware : public CameraHardwareInterface {
        public:
            virtual sp<IMemoryHeap> getPreviewHeap() const;

            virtual sp<IMemoryHeap> getRawHeap() const;

            virtual void setCallbacks(notify_callback notify_cb,
                    data_callback data_cb,
                    data_callback_timestamp data_cb_timestamp,
                    void* user);

            virtual void        enableMsgType(int32_t msgType);

            virtual void        disableMsgType(int32_t msgType);

            virtual bool        msgTypeEnabled(int32_t msgType);

            virtual status_t    startPreview();
            virtual status_t    RegisterPreviewBuffers();

            virtual bool        useOverlay();

            virtual status_t    setOverlay(const sp<Overlay> &overlay);

            virtual void        stopPreview();
            void		__stopPreviewThread();

            virtual bool        previewEnabled();

            virtual status_t    startRecording();

            virtual void        stopRecording();

            virtual bool        recordingEnabled(){return mRecordingEnabled;}

            virtual void        releaseRecordingFrame(const sp<IMemory>& mem);

            virtual status_t    autoFocus();

            virtual status_t    cancelAutoFocus();

            virtual status_t    takePicture();

            virtual status_t    cancelPicture();

            virtual status_t    setParameters(const CameraParameters& params);

            virtual CameraParameters  getParameters() const;

            virtual void        release();

            virtual status_t    dump(int fd, const Vector<String16>& args) const;

            static sp<CameraHardwareInterface> createInstance();
            static sp<CameraHardwareInterface> createInstance(int cameraId);

            virtual void        releasePreviewBuffer(int index);

            enum{
                MAX_CAMERAS=2,	//this num should be no less than MAX_CAMERAS in cameraservice.h
            };
            //For baselayer workaround buffer for the format that surface unsupported, or for debug usage
            //TODO:put it private.
            sp<ImageBuf>    mPreviewImageBuf_base;

        private:
            CameraHardware(int cameraID=0);
            virtual             ~CameraHardware();

            static wp<CameraHardwareInterface> singleton;
            static wp<CameraHardwareInterface> multiInstance[MAX_CAMERAS];

            int kPreviewBufCnt;
            int kStillBufCnt;
            int kVideoBufCnt;
            int iExpectedPicNum;
            int iSkipFrame;
            int mDumpNum;
            int mMinDriverBufCnt;

            class PictureThread : public Thread {
                CameraHardware* mHardware;
                public:
                PictureThread(CameraHardware* hw):
                    Thread(false), mHardware(hw) { }
                virtual void onFirstRef() {
                    run("CameraPictureThread", PRIORITY_URGENT_DISPLAY);
                }
                virtual bool threadLoop() {
                    mHardware->pictureThread();
                    //once run.
                    return false;
                }
            };

            class PreviewThread : public Thread {
                CameraHardware* mHardware;
                public:
                PreviewThread(CameraHardware* hw)
                    : Thread(false), mHardware(hw) { }
                virtual void onFirstRef() {
                    run("CameraPreviewThread", PRIORITY_URGENT_DISPLAY);
                }
                virtual bool threadLoop() {
                    mHardware->previewThread();
                    // loop until we need to quit
                    return true;
                }
            };

            //void initHeapLocked();

            int previewThread();

            static int beginAutoFocusThread(void *cookie);
            status_t autoFocusThread();

            int pictureThread();

            void updateBufferQueue(int index);

            mutable Mutex       mLock;

            notify_callback     mNotifyCb;
            data_callback       mDataCb;
            data_callback_timestamp mDataCbTimestamp;
            void                *mCallbackCookie;
            int32_t             mMsgEnabled;

            /*
             * mRecordingLock ensures callbacks and hardware resources
             * won't be freed while a frame is still under processing.
             */
            bool                mRecordingEnabled;

            char PMEM_DEVICE[PROPERTY_VALUE_MAX];
            sp<CameraPreviewInterface>	mPreviewDevice;
            sp<Overlay>         mOverlay;
            bool                mUseOverlay;
            int                 mDisplayEnabled;

            virtual status_t    sendCommand(int32_t cmd, int32_t arg1, int32_t arg2)
            {return BAD_VALUE;}

            sp<CameraCtrl> mCamPtr;
            sp<ImageBuf>    mPreviewImageBuf;
            sp<IMemoryHeap> mPreviewHeap;

            sp<ImageBuf>    mStillImageBuf;

            sp<MemoryHeapBase>  mRawHeap;


            sp<PreviewThread>   mPreviewThread;
            sp<PictureThread>   mPictureThread;

            long long		mPreviewFrameCount;

            /*
             * When video encoding can't catch up with camera input, streaming
             * buffer queue will finally drain. Then, preview thread begins to
             * wait for empty buffer, and sets mBuffersEmpty to true.
             * After an empty buffer is returned through releaseRecordingFrame(),
             * preview thread will be signaled and mBuffersEmpty will be reset
             * to false.
             */
            Mutex               mBuffersLock;
            Condition           mBuffersCond;
            int			mState;
            enum{
                CAM_STATE_IDLE,
                CAM_STATE_PREVIEW,
                CAM_STATE_STILL,
                CAM_STATE_VIDEO,
            } CAMERA_STATE;
    };

}; // namespace android

#endif
