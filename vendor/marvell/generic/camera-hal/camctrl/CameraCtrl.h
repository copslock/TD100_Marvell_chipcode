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

#ifndef ANDROID_HARDWARE_CAMERA_CTRL_H
#define ANDROID_HARDWARE_CAMERA_CTRL_H

#include <binder/MemoryBase.h>
#include <binder/MemoryHeapBase.h>
#include <utils/threads.h>
#include <binder/MemoryHeapPmem.h>
#include <linux/android_pmem.h>
#if PLATFORM_SDK_VERSION >= 8
#include <camera/CameraHardwareInterface.h>
#else
#include <ui/CameraHardwareInterface.h>
#endif
#include "ImageBuf.h"

namespace android {

    class CameraCtrl: public virtual RefBase{
        public:
            CameraCtrl(int cameraID=0){}
            virtual ~CameraCtrl(){}

            virtual void setCallbacks(notify_callback notify_cb=NULL,
                    void* user=NULL)=0;
            virtual void enableMsgType(int32_t msgType)=0;
            virtual void disableMsgType(int32_t msgType)=0;

            virtual status_t startPreview()=0;
            virtual status_t stopPreview()=0;

            virtual status_t startCapture()=0;
            virtual status_t stopCapture()=0;

            virtual status_t startVideo()=0;
            virtual status_t stopVideo()=0;

            virtual status_t setParameters(CameraParameters param)=0;
            virtual CameraParameters getParameters()=0;

            //preview
            virtual status_t registerPreviewBuffers(sp<ImageBuf> imagebuf)=0;
            virtual status_t unregisterPreviewBuffers()=0;

            //virtual status_t enqPreviewBuffer(sp<ImageBuf> imagebuf, int index){return NO_ERROR;};
            virtual status_t enqPreviewBuffer(int index)=0;
            virtual status_t deqPreviewBuffer(int* index)=0;
            //virtual sp<IMemory> getPreviewFrame()=0;

            //still
            virtual status_t registerStillBuffers(sp<ImageBuf> imagebuf)=0;
            virtual status_t unregisterStillBuffers()=0;

            //virtual status_t enqStillBuffer(sp<ImageBuf> imagebuf, int index){return NO_ERROR;};
            virtual status_t enqStillBuffer(int index)=0;
            virtual status_t deqStillBuffer(int* index)=0;
            //virtual sp<IMemory> getStillFrame()=0;

#if 0
            virtual CAM_ImageBufferReq getPreviewBufReq()=0;
            virtual CAM_ImageBufferReq getStillBufReq()=0;
#else
            virtual ImageBuf::EngBufReq getPreviewBufReq()=0;
            virtual ImageBuf::EngBufReq getStillBufReq()=0;
#endif

            virtual void* getUserData(void* ptr = NULL){return NULL;};
            virtual sp<IMemory> getExifImage(int index){return NULL;};

            virtual status_t autoFocus()=0;
            virtual status_t cancelAutoFocus()=0;

            virtual status_t getBufCnt(int* previewbufcnt,int* stillbufcnt,int* videobufcnt) const=0;

#if 0
            static int getNumberOfCameras()
#endif

                //----------------------------------------
                enum CamState{
                    IDLE,
                    PREVIEW,
                    STILL,
                };
            //virtual CamState getState()=0;

        private:
            Mutex   mCamCtrlLock;
    };

}; // namespace android
#endif
