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

#ifndef ANDROID_HARDWARE_ENGINE_H
#define ANDROID_HARDWARE_ENGINE_H

namespace android {

    class Engine:public CameraCtrl{
        public:
            //Engine(const CameraSetting& setting);
            Engine(int cameraID=0);
            virtual ~Engine();
            int	    mTuning;

            status_t startPreview();
            status_t stopPreview();

            status_t startCapture();
            status_t stopCapture();

            status_t startVideo();
            status_t stopVideo();

            status_t setParameters(CameraParameters param);
            CameraParameters getParameters();

            //preview
            status_t registerPreviewBuffers(sp<ImageBuf> imagebuf);
            //status_t enqPreviewBuffer(sp<ImageBuf> imagebuf, int index);//XXX:check buf req/cnt here
            status_t enqPreviewBuffer(int index);
            status_t deqPreviewBuffer(int* index);
            status_t unregisterPreviewBuffers();

            //still
            status_t registerStillBuffers(sp<ImageBuf> imagebuf);
            //status_t enqStillBuffer(sp<ImageBuf> imagebuf, int index);
            status_t enqStillBuffer(int index);
            status_t deqStillBuffer(int* index);
            status_t unregisterStillBuffers();

            sp<IMemory> getExifImage(int index);

            ImageBuf::EngBufReq getPreviewBufReq();
            ImageBuf::EngBufReq getStillBufReq();

            status_t autoFocus();
            status_t cancelAutoFocus();

            status_t getBufCnt(int* previewbufcnt,int* stillbufcnt,int* videobufcnt) const;

            static int getNumberOfCameras();
            static void getCameraInfo(int cameraId, struct CameraInfo* cameraInfo);

            virtual void setCallbacks(notify_callback notify_cb=NULL,
                    void* user=NULL);
            virtual void enableMsgType(int32_t msgType);
            virtual void disableMsgType(int32_t msgType);

            static void NotifyEvents(CAM_EventId eEventId,void* param,void *pUserData);
        private:
            CameraSetting mSetting;

            notify_callback     mNotifyCb;
            void                *mCallbackCookie;
            int32_t             mMsgEnabled;

            CAM_DeviceHandle	    mCEHandle;

            CAM_CameraCapability    mCamera_caps;
            CAM_ShotModeCapability  mCamera_shotcaps;

            static const int kMaxBufCnt = 20;//max preview/still/video buffer count

            sp<ImageBuf>    mPreviewImageBuf;
            sp<ImageBuf>    mStillImageBuf;

#if 1
            int iExpectedPicNum;
#endif
            CAM_Error ceInit(CAM_DeviceHandle *pHandle,CAM_CameraCapability *pcamera_caps);
            status_t ceStartPreview(const CAM_DeviceHandle &handle);
            status_t ceUpdateSceneModeSetting(const CAM_DeviceHandle &handle,
                    CameraSetting& setting);
            status_t ceStopPreview(const CAM_DeviceHandle &handle);

            status_t ceStartCapture(const CAM_DeviceHandle &handle);
            status_t ceStopCapture(const CAM_DeviceHandle &handle);

            status_t ceStartVideo(const CAM_DeviceHandle &handle);
            status_t ceStopVideo(const CAM_DeviceHandle &handle);

            CAM_Error ceGetPreviewFrame(const CAM_DeviceHandle &handle,int *index);
            CAM_Error ceGetStillFrame(const CAM_DeviceHandle &handle,int *index);
            CAM_Error ceGetCurrentSceneModeCap(const CAM_DeviceHandle &handle,
                    CameraSetting& setting);

            //used to draw rect
            void DrawRect(int xLeft, int yLeft, int xRight, int yRight,
                    ImageBuf * image, int index);
            void DrawHorizontalLine_YUV420P(int yAxis, int xLeft, int xRight,
                    int previewWidth, int previewHeight, uint8_t * buf_start);
            void DrawVerticalLine_YUV420P(int xAxis, int yTop, int yBottom,
                    int previewWidth, int previewHeight, uint8_t * buf_start);
            void DrawHorizontalLine_YUV422I_UYVY(int yAxis, int xLeft, int xRight,
                    int previewWidth, int previewHeight, uint8_t * buf_start);
            void DrawVerticalLine_YUV422I_UYVY(int xAxis, int yTop, int yBottom,
                    int previewWidth, int previewHeight, uint8_t * buf_start);
            void DrawHorizontalLine_YUV420SP(int yAxis, int xLeft, int xRight,
                    int previewWidth, int previewHeight, uint8_t * buf_start);
            void DrawVerticalLine_YUV420SP(int xAxis, int yTop, int yBottom,
                    int previewWidth, int previewHeight, uint8_t * buf_start);

    };

}; // namespace android
#endif
