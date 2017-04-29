/*
 * (C) Copyright 2010 Marvell International Ltd.
 * All Rights Reserved
 */
/* ------------------------------------------------------------------
 * Copyright (C) 2009 Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * -------------------------------------------------------------------
 */

/*
 * This is a sample video sink using frame buffer push model. The
 * video output is NV21 (YUV 420 semi-planar with VUVU ordering).
 * This requires re-ordering the YUV 420 planar output from the
 * software codec to match the hardware color converter. The
 * encoder outputs its frames in NV21, so no re-order is necessary.
 *
 * The hardware decoder and hardware video unit share pmem buffers
 * (tunneling mode). For software codecs, we allocate a pmem buffer
 * and convert the decoded YUV frames while copying to them to the
 * pmem frame buffers used by the hardware output.
 * 
 * This code should be compiled into a libopencorehw.so module.
 * Here is a sample makefile to build the library:
 *
 * LOCAL_PATH := $(call my-dir)
 * include $(CLEAR_VARS)
 *
 * # Set up the OpenCore variables.
 * include external/opencore/Config.mk
 * LOCAL_C_INCLUDES := $(PV_INCLUDES)
 *
 * LOCAL_SRC_FILES := android_surface_output_fb.cpp
 * 
 * LOCAL_CFLAGS := $(PV_CFLAGS)
 * 
 * LOCAL_SHARED_LIBRARIES := \
 *     libutils \
 *     libcutils \
 *     libui \
 *     libhardware\
 *     libandroid_runtime \
 *     libmedia \
 *     libsgl \
 *     libopencorecommon \
 *     libicuuc \
 *     libopencoreplayer
 * 
 * LOCAL_MODULE := libopencorehw
 * 
 * LOCAL_LDLIBS += 
 * 
 * include $(BUILD_SHARED_LIBRARY)
 *
 */

//#define LOG_NDEBUG 0
#define LOG_TAG "VideoMio3D"
#include <utils/Log.h>
#include <cutils/properties.h>

#include <surfaceflinger/ISurface.h>
#include "android_surface_output_3d.h"
#include <media/PVPlayer.h>

using namespace android;

OSCL_IMPORT_REF AndroidSurfaceOutput3D::AndroidSurfaceOutput3D():AndroidSurfaceOutput()
{
#if PLATFORM_SDK_VERSION >= 5
    mNumberOfFramesToHold = 0; 
#endif
    iYUV422toYUV420PConverter = NULL;
    pTempBuf = NULL;
}

OSCL_IMPORT_REF AndroidSurfaceOutput3D::~AndroidSurfaceOutput3D()
{
}

// create a frame buffer for software codecs
OSCL_IMPORT_REF bool AndroidSurfaceOutput3D::initCheck()
{

    // initialize only when we have all the required parameters
    if (!checkVideoParameterFlags())
        return mInitialized;

    // release resources if previously initialized
    closeFrameBuf();

    // reset flags in case display format changes in the middle of a stream
    resetVideoParameterFlags();

    // copy parameters in case we need to adjust them
    int displayWidth = iVideoDisplayWidth;
    int displayHeight = iVideoDisplayHeight;
    int frameWidth = iVideoWidth;
    int frameHeight = iVideoHeight;
    int frameSize;

    if (iVideoSubFormat == PVMF_MIME_YUV422_INTERLEAVED_UYVY)
    {
	frameWidth >>= 1;
    }

    // RGB-565 frames are 2 bytes/pixel
    displayWidth = (displayWidth + 1) & -2;
    displayHeight = (displayHeight + 1) & -2;
    frameWidth = (frameWidth + 1) & -2;
    frameHeight = (frameHeight + 1) & -2;
    frameSize = frameWidth * frameHeight * 2;

    // create frame buffer heap and register with surfaceflinger
    sp<MemoryHeapBase> heap = new MemoryHeapBase(frameSize * kBufferCount);
    if (heap->heapID() < 0) {
        LOGE("Error creating frame buffer heap");
        return false;
    }
    
    mBufferHeap = ISurface::BufferHeap(displayWidth, displayHeight,
            frameWidth, frameHeight, PIXEL_FORMAT_RGB_565, heap);
    mSurface->registerBuffers(mBufferHeap);

    // create frame buffers
    for (int i = 0; i < kBufferCount; i++) {
        mFrameBuffers[i] = i * frameSize;
    }

    // initialize software color converter
    iColorConverter = ColorConvert16::NewL();
    iColorConverter->Init(displayWidth, displayHeight, frameWidth, displayWidth, displayHeight, displayWidth, CCROTATE_NONE);
    iColorConverter->SetMemHeight(frameHeight);
    iColorConverter->SetMode(1);

    if (iVideoSubFormat == PVMF_MIME_YUV422_INTERLEAVED_UYVY)
    {
	iYUV422toYUV420PConverter = CCYUV422toYUV420::New();
	LOGD("iYUV422toYUV420PConverter init pic [%d %d %d]", frameWidth, frameHeight, frameWidth);
    	iYUV422toYUV420PConverter->Init(frameWidth, frameHeight, frameWidth, frameWidth, frameHeight, frameWidth, CCROTATE_NONE);
    	if (pTempBuf == NULL)
	    pTempBuf = malloc(iYUV422toYUV420PConverter->GetOutputBufferSize());
    	LOGD("mid 420p buffer size %d", iYUV422toYUV420PConverter->GetOutputBufferSize());
    }

    LOGD("video = %d x %d", displayWidth, displayHeight);
    LOGD("frame = %d x %d", frameWidth, frameHeight);
    LOGD("frame #bytes = %d", frameSize);

    // register frame buffers with SurfaceFlinger
    mFrameBufferIndex = 0;
    mInitialized = true;
    mPvPlayer->sendEvent(MEDIA_SET_VIDEO_SIZE, iVideoDisplayWidth, iVideoDisplayHeight);

    return mInitialized;
}

PVMFStatus AndroidSurfaceOutput3D::writeFrameBuf(uint8* aData, uint32 aDataLen, const PvmiMediaXferHeader& data_header_info)
{
    if ((mSurface != NULL) && (mBufferHeap.heap != NULL)) {
        if (++mFrameBufferIndex == kBufferCount) mFrameBufferIndex = 0;
	if (iYUV422toYUV420PConverter && iVideoSubFormat == PVMF_MIME_YUV422_INTERLEAVED_UYVY)
	{
	    iYUV422toYUV420PConverter->Convert(aData, static_cast<uint8*>(pTempBuf));
	    iColorConverter->Convert(static_cast<uint8*>(pTempBuf), static_cast<uint8*>(mBufferHeap.heap->base()) + mFrameBuffers[mFrameBufferIndex]);
	}
	else 
	    iColorConverter->Convert(aData, static_cast<uint8*>(mBufferHeap.heap->base()) + mFrameBuffers[mFrameBufferIndex]);
        mSurface->postBuffer(mFrameBuffers[mFrameBufferIndex]);
    }
    return PVMFSuccess;
}

// post the last video frame to refresh screen after pause
void AndroidSurfaceOutput3D::postLastFrame()
{
}

void AndroidSurfaceOutput3D::closeFrameBuf()
{
    LOGV("CloseFrameBuf");
    if (!mInitialized) return;

    mInitialized = false;
    if (mSurface.get()) {
        LOGV("unregisterBuffers:0x%x", mSurface.get() );
        mSurface->unregisterBuffers();
    }

    if (pTempBuf)
    {
	free(pTempBuf);
    }
}

