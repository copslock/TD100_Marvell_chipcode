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

#ifndef ANDROID_HARDWARE_CAMERA_PREVIEW_OVERLAY_H
#define ANDROID_HARDWARE_CAMERA_PREVIEW_OVERLAY_H

#include "CameraPreviewInterface.h"

namespace android {

    class CameraPreviewOverlay : public CameraPreviewInterface {
        private:
            CameraPreviewOverlay(){}
        public:
            CameraPreviewOverlay(wp<CameraHardwareInterface> camera);
            virtual ~CameraPreviewOverlay();

            virtual status_t display(const sp<ImageBuf>& imagebuf, int index);
            virtual status_t flush();

            virtual status_t startRecording();
            virtual status_t stopRecording();

            virtual bool     hasOverlay() {return true;}
            virtual status_t setOverlay(const sp<Overlay> &overlay);

            virtual status_t setPreviewCallback(data_callback cb,
                    void *user,
                    const sp<ImageBuf> imagebuf);

        private:
            static  void     release(void *private_data);

            wp<CameraHardwareInterface>	mCameraHardware;
            sp<Overlay>            mOverlay;

            data_callback         mPreviewCallback;
            void *                mPreviewCallbackCookie;
            sp<ImageBuf>	    mImageBuf;

    };

}; // namespace android
#endif
