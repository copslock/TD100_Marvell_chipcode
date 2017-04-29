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

#ifndef ANDROID_SURFACE_OUTPUT_OV_H_INCLUDED
#define ANDROID_SURFACE_OUTPUT_OV_H_INCLUDED

#include "android_surface_output.h"

#if (defined(IPPCODEC)||defined(IPPOMX))
#include "codecDef.h"
#include "ippIP.h"
#endif

#if PLATFORM_SDK_VERSION >=8
#include <surfaceflinger/ISurface.h>
#include <surfaceflinger/Surface.h>
#include <surfaceflinger/ISurfaceComposer.h>
#include <surfaceflinger/SurfaceComposerClient.h>
#else
#include <ui/ISurface.h>
#include <ui/Surface.h>
#include <ui/SurfaceComposerClient.h>
#endif

#include <ui/Overlay.h>
#include "mrvl_buf_allocator.h"
#include "pv_mime_string_utils.h"
#include "pvmf_video.h"
#include "overlay.marvell.h"

#define PERF

class AndroidSurfaceOutputOverlay : public AndroidSurfaceOutput
{
public:
    AndroidSurfaceOutputOverlay();

    // frame buffer interface
    virtual bool initCheck();
    virtual PVMFStatus writeFrameBuf(uint8* aData, uint32 aDataLen, const PvmiMediaXferHeader& data_header_info);
    virtual void postLastFrame();
    virtual void closeFrameBuf();
    virtual int ParseYUVInfo(PvmiKvp& aParameters);

    virtual status_t setVideoSurface(const sp<ISurface>& surface);

    virtual PVMFStatus getParametersSync(PvmiMIOSession aSession, PvmiKeyType aIdentifier,
        PvmiKvp*& aParameters, int& num_parameter_elements, PvmiCapabilityContext aContext);

    virtual void setParametersSync(PvmiMIOSession aSession, PvmiKvp* aParameters,
        int num_elements, PvmiKvp * & aRet_kvp);

    OSCL_IMPORT_REF ~AndroidSurfaceOutputOverlay();

#ifdef FAST_OVERLAY_CALLBACK
    void processDisplayDone();
#endif

private:
#if 0
    void convertIPPFrame(void* src, void* dst);
#endif

#if PLATFORM_SDK_VERSION >= 5
    sp<SurfaceControl> mBgSurface;
#else
    sp<Surface> mBgSurface;
#endif

    sp<OverlayRef> mOverlayRef;
    sp<Overlay>    mOverlay;

    bool			bUseMIOBuffer;
    int                         iNumRequestedMIOBuffer;
    int                         iRequestedMIOBufSize;
    PVInterface*		pExtAlloc;
   
#ifdef PERF
    long time_tot_base;
    int frame_num_base;
    struct timespec t_start_base;
    struct timespec t_stop_base;
    bool bStartPerf;
#endif

};

#endif // ANDROID_SURFACE_OUTPUT_OV_H_INCLUDED
