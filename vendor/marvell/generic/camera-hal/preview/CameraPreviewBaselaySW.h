#ifndef ANDROID_HARDWARE_CAMERA_PREVIEW_BASELAY_SW_H
#define ANDROID_HARDWARE_CAMERA_PREVIEW_BASELAY_SW_H

#include "CameraPreviewInterface.h"

namespace android {

    class CameraPreviewBaselaySW : public CameraPreviewInterface {
        private:
            CameraPreviewBaselaySW(){}
        public:
            CameraPreviewBaselaySW(wp<CameraHardwareInterface> camera);
            virtual ~CameraPreviewBaselaySW();

            virtual status_t display(const sp<ImageBuf>& imagebuf, int index);
            virtual status_t flush(){return NO_ERROR;}

            virtual status_t startRecording();
            virtual status_t stopRecording();

            virtual bool     hasOverlay() {return false;}
            virtual status_t setOverlay(const sp<Overlay> &overlay) {return INVALID_OPERATION;}

            virtual status_t setPreviewCallback(data_callback cb,
                    void *user,
                    const sp<ImageBuf> imagebuf);

        private:
#if 0
            CAM_Error        yuv420SPToRGB565(sp<IMemory> buffer,CAM_ImageBuffer *pImgBuf);
#endif
            status_t	yuv420ToRGB565(sp<ImageBuf>& buffer_dst, int j, const sp<ImageBuf>& imagebuf_src, int i);

            status_t	updateBaseBuffer();

            wp<CameraHardwareInterface>	mCameraHardware;
            bool                  mRecordingOn;
            data_callback         mPreviewCallback;
            void *                mPreviewCallbackCookie;

            sp<ImageBuf>     mImageBuf;
            sp<ImageBuf>	  mRGB565ImageBuf;
    };

}; // namespace android
#endif
