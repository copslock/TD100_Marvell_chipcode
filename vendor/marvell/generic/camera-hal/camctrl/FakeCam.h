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

#ifndef ANDROID_HARDWARE_CAMERA_FAKECAM_H
#define ANDROID_HARDWARE_CAMERA_FAKECAM_H

#include <binder/MemoryBase.h>
#include <binder/MemoryHeapBase.h>
#include <utils/threads.h>
#include <binder/MemoryHeapPmem.h>
#include <linux/android_pmem.h>

#include "CameraSetting.h"

namespace android {

    class FakeCam: public CameraCtrl{
        public:
            FakeCam(int cameraID=0);
            virtual ~FakeCam();

            void setCallbacks(notify_callback notify_cb=NULL,void* user=NULL);
            void enableMsgType(int32_t msgType);
            void disableMsgType(int32_t msgType);

            status_t startPreview();
            status_t stopPreview();

            status_t startCapture();
            status_t stopCapture();

            status_t startVideo();
            status_t stopVideo();

            status_t setParameters(CameraParameters param);
            CameraParameters getParameters();

            status_t registerPreviewBuffers(sp<ImageBuf> imagebuf);
            status_t unregisterPreviewBuffers();

            status_t enqPreviewBuffer(sp<ImageBuf> imagebuf, int index){return NO_ERROR;};
            status_t enqPreviewBuffer(int index);
            status_t deqPreviewBuffer(int* index);

            status_t registerStillBuffers(sp<ImageBuf> imagebuf);
            status_t unregisterStillBuffers();

            status_t enqStillBuffer(sp<ImageBuf> imagebuf, int index){return NO_ERROR;};
            status_t enqStillBuffer(int index);
            status_t deqStillBuffer(int* index);
            sp<IMemory> getStillFrame();

            ImageBuf::EngBufReq getPreviewBufReq();
            ImageBuf::EngBufReq getStillBufReq();

            void* getUserData(void* ptr = NULL){return NULL;};
            sp<IMemory> getExifImage(int index);

            status_t autoFocus();
            status_t cancelAutoFocus();

            status_t getBufCnt(int* previewbufcnt,int* stillbufcnt,int* videobufcnt) const;
#if 0
            static int getNumberOfCameras(){
            }
#endif

            static int getNumberOfCameras();
            static void getCameraInfo(int cameraId, struct CameraInfo* cameraInfo);

            static const int kRed = 0xf800;
            static const int kGreen = 0x07c0;
            static const int kBlue = 0x003e;

        private:
            int mCameraId;
            CameraParameters mSetting;
            int kPreviewBufCnt;
            int kStillBufCnt;
            int kVideoBufCnt;

            notify_callback     mNotifyCb;
            void                *mCallbackCookie;
            int32_t             mMsgEnabled;
            sp<ImageBuf>    mPreviewImgBuf;
            sp<ImageBuf>    mStillImgBuf;

            void fillpreviewbuffer(sp<ImageBuf> imagebuf, int i);
            void fillstillbuffer(sp<ImageBuf> imagebuf, int i);
            //for preview fps control
            int mPreviewTimeStamp;//us
            int mPreviewStdDelay;//us
            int mPreviewRealDelay;
            int mPreviewFrameRate;
    };

}; // namespace android
#endif
