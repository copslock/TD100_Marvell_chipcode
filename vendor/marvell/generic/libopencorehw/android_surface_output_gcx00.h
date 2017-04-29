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

#ifndef ANDROID_SURFACE_OUTPUT_GCx00_H_INCLUDED
#define ANDROID_SURFACE_OUTPUT_GCx00_H_INCLUDED

#include "android_surface_output.h"

// support for shared contiguous physical memory
#if PLATFORM_SDK_VERSION >= 5
#include <binder/MemoryHeapPmem.h>
#else
#include <utils/MemoryHeapPmem.h>
#endif

#include "pv_mime_string_utils.h"
#include "pvmf_video.h"
#include "mrvl_buf_allocator.h"

#define PERF

typedef struct {
    void * pSurface;
    uint8*     pVirAddr;
    uint32     nPhyAddr;
}GCUSurfaceInfo;

class AndroidSurfaceOutputGCx00 : public AndroidSurfaceOutput
{
public:
    AndroidSurfaceOutputGCx00();

    // frame buffer interface
    virtual bool initCheck();
    virtual PVMFStatus writeFrameBuf(uint8* aData, uint32 aDataLen, const PvmiMediaXferHeader& data_header_info);
    void setParametersSync(PvmiMIOSession aSession, PvmiKvp* aParameters, 
            int num_elements, PvmiKvp * & aRet_kvp);
    PVMFStatus getParametersSync(PvmiMIOSession aSession, PvmiKeyType aIdentifier,
                                              PvmiKvp*& aParameters, int& num_parameter_elements,
                                              PvmiCapabilityContext aContext);
    OSCL_IMPORT_REF ~AndroidSurfaceOutputGCx00();

private:
    int gcu_frame_color_convert (uint8_t* aSrcData,uint32_t aSrcPhyAddress, uint8_t* aDstData, uint32_t aDstPhyAddress);
    void *pGcuContext;
    uint32_t GCUOutputColorFormat;
    GCUSurfaceInfo SrcSurfaceInfoQ[16];
    GCUSurfaceInfo DstSurfaceInfoQ[16];
    int32 iGCUDstVideo_W;
    int32 iGCUDstVideo_H;
    sp<MemoryHeapBase> mBackupMemoryHeap;
    PVInterface*                pExtAlloc;
    bool                        bUseMIOBuffer;
    int                         iNumRequestedMIOBuffer;
    int                         iRequestedMIOBufSize;

#ifdef PERFORMANCE_MEASUREMENTS_ENABLED
        PVProfile PVOmapVideoProfile;
#endif

#ifdef PERF
    long long time_tot_base;
    int frame_num_base;
    struct timespec t_start_base;
    struct timespec t_stop_base;
    bool bStartPerf;
#endif
};

#endif // ANDROID_SURFACE_OUTPUT_GCx00_H_INCLUDED
