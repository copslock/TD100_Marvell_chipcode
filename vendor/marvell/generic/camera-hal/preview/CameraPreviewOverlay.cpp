/*
 * Copyright (C) 2008 The Android Open Source Project
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

#include "cam_log_mrvl.h"

#include "CameraPreviewOverlay.h"
#include "CameraHardware.h"
#include "overlay.marvell.h"
#define LOG_TAG "CameraPreviewOverlay"

namespace android {

    //struct for release
    class UserParcel{
        public:
            UserParcel(wp<CameraHardwareInterface> cam,int idx)
                :camera(cam),
                index(idx)
        {}
            const wp<CameraHardwareInterface> camera;
            const int index;
    };

    CameraPreviewOverlay::CameraPreviewOverlay(wp<CameraHardwareInterface> camera)
        :mOverlay(0),
        mPreviewCallback(0),
        mPreviewCallbackCookie(0)
    {
        sp<CameraHardware> hardware = getInstance(camera);
        if (hardware == 0) {
            TRACE_E("CameraHardware instace already destroyed,promote failed!");
            return;
        }

        int width, height;
        CameraParameters params(hardware->getParameters());
        params.getPreviewSize(&width, &height);

        mCameraHardware = camera;
    }

    CameraPreviewOverlay::~CameraPreviewOverlay()
    {
        TRACE_D("%s", __FUNCTION__);
        if(mOverlay.get() != NULL){
            mOverlay->destroy();
        }
        mOverlay = 0;
        mCameraHardware = NULL;
    }

    status_t CameraPreviewOverlay::setPreviewCallback(data_callback cb,
            void *user,
            const sp<ImageBuf> imagebuf)
    {
        mPreviewCallback = cb;
        mPreviewCallbackCookie = user;
        mImageBuf = imagebuf;

        return NO_ERROR;
    }

    status_t CameraPreviewOverlay::display(const sp<ImageBuf>& imagebuf, int index)
    {
        /*
         * en-queue preview buffer to overlay-hal for display
         */
        overlay_buffer_header_t *bufHdr;
        status_t ret;

        TRACE_V("%s: display buffer #%d", __FUNCTION__, index);

        ret = mOverlay->dequeueBuffer((overlay_buffer_t*)&bufHdr);
        TRACE_V("%s: dequeue buffer header %p", __FUNCTION__, bufHdr);
        if(NO_ERROR != ret){
            TRACE_E("overlay dequeueBuffer fail");
            return UNKNOWN_ERROR;
        }
        if(NULL == bufHdr){
            TRACE_E("%s: invalid buffer header %p", __FUNCTION__, bufHdr);
            return UNKNOWN_ERROR;
        }

        unsigned long paddr[3];
        imagebuf->getPhyAddr(index,paddr+0,paddr+1,paddr+2);
        bufHdr->paddr = (void *)paddr[0];

        //FIXME:check whether need to reserve alignment space for image buf
        int len[3];
        imagebuf->getAllocLen(index,len+0,len+1,len+2);
        bufHdr->len   = len[0]+len[1]+len[2];

        unsigned char* vaddr[3];
        imagebuf->getVirAddr(index,vaddr+0,vaddr+1,vaddr+2);
        bufHdr->vaddr = (void *)vaddr[0];

        bufHdr->user  = new UserParcel(mCameraHardware,index);
        bufHdr->free  = release;

        int w,h;
        imagebuf->getImageSize(index,&w,&h);
        bufHdr->stride_x = w;
        bufHdr->stride_y = h;

        int step[3];
        imagebuf->getStep(index,step+0,step+1,step+2);
        bufHdr->step0 = step[0];
        bufHdr->step1 = step[1];
        bufHdr->step2 = step[2];

        bufHdr->x_off = 0;
        bufHdr->y_off = 0;
        bufHdr->flag = 2;//phy cont buffer, no memcpy

        TRACE_V("%s: queue buffer header %p", __FUNCTION__, bufHdr);
        ret = mOverlay->queueBuffer((overlay_buffer_t)bufHdr);
        if(NO_ERROR != ret){
            TRACE_E("overlay queueBuffer fail");
            return UNKNOWN_ERROR;
        }
        return NO_ERROR;
    }

    status_t CameraPreviewOverlay::startRecording()
    {
        return NO_ERROR;
    }

    status_t CameraPreviewOverlay::stopRecording()
    {
        return NO_ERROR;
    }

    status_t CameraPreviewOverlay::setOverlay(const sp<Overlay> &overlay)
    {
        TRACE_D("%s: overlay=%p",__FUNCTION__,(overlay==NULL)? 0 : overlay.get());
        if(overlay == NULL || overlay.get() == NULL){
            if(mOverlay.get() != NULL){
                mOverlay->destroy();
            }
        }
        mOverlay = overlay;
        return NO_ERROR;
    }


    status_t CameraPreviewOverlay::flush()
    {
        /*
           if (mOverlay == 0){
           TRACE_E("mOverlay is not set, return");
           return UNKNOWN_ERROR;
           }

           overlay_buffer_header_t *bufHdr;
        //int bufCnt = mOverlay->getBufferCount();

        mOverlay->dequeueBuffer((overlay_buffer_t*)&bufHdr);
        if (bufHdr) {
        bufHdr->paddr = 0;
        bufHdr->flag = 3;//flush buffer
        mOverlay->queueBuffer((overlay_buffer_t)bufHdr);
        }
        */
        return NO_ERROR;
    }

    void CameraPreviewOverlay::release(void *private_data)
    {
        if(private_data == NULL){
            TRACE_E("release NULL ptr!");
            return;
        }
        UserParcel* ptr = static_cast<UserParcel*>(private_data);
        wp<CameraHardwareInterface> hardware_weak = ptr->camera;
        int index = static_cast<int>(ptr->index);
        delete ptr;//free parcel memory

        sp<CameraHardware> hardware = getInstance(hardware_weak);
        if (hardware == 0) {
            TRACE_E("CameraHardware instace already destroyed,promote failed!");
            return;
        }
        hardware->releasePreviewBuffer(index);
        TRACE_V("%s: return buffer #%d", __FUNCTION__, index);
    }

}; // namespace android
