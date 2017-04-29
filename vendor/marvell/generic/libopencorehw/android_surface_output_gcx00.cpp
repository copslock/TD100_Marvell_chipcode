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
#define LOG_TAG "VideoMioGCx00"
#include <utils/Log.h>
#include <cutils/properties.h>

#include "android_surface_output_gcx00.h"
#include <media/PVPlayer.h>

#include <linux/fb.h>
#include "gcu.h"
#include "OMX_IppDef.h"
#include <linux/android_pmem.h>
#include <binder/MemoryHeapPmem.h>


#define MAX_VIDMIO_BUFFER 16
#define MIN_VIDMIO_BUFFER 3

using namespace android;

#ifdef PERF
#include <cutils/properties.h>
#define GET_TIME_INTERVAL_USEC(A, B) (((((B).tv_sec)-((A).tv_sec))*1000000) + ((((B).tv_nsec)-((A).tv_nsec))/1000))
#define GET_TIME_INTERVAL_NSEC(A, B) (((((B).tv_sec)-((A).tv_sec))*1000000000LL) + ((((B).tv_nsec)-((A).tv_nsec))))
#endif
using namespace android;

static GCUContext gcu_init(GCUContext gcuContext_IN){

    GCUenum result;
    GCUContext gcuContext;
    GCU_INIT_DATA initData;
    GCU_CONTEXT_DATA contextData;
    if(gcuContext_IN !=NULL)
        return gcuContext_IN;
    // Init GCU library
    memset(&initData, 0, sizeof(initData));
    gcuInitialize(&initData);
    // Create Context
    memset(&contextData, 0, sizeof(contextData));
    return gcuCreateContext(&contextData);
}
static int gcu_exit(GCUContext gcuContext){
    gcuDestroyContext(gcuContext);
    gcuTerminate();
    return 0;
}
static int gcu_lcd_resolution(int32 *w, int32 *h){
    struct fb_var_screeninfo var;
    int fd = open("/dev/graphics/fb0", O_RDWR);
    if( fd > 0 ) {
        int ret = ioctl(fd, FBIOGET_VSCREENINFO, &var);
        if( ret >= 0 ) {
	    *w = var.xres;
            *h = var.yres;
            close(fd);
         }
    }
    return 0;
}
#define ALIGN64(m) (unsigned char *)((unsigned int)(m+4095)&(~(unsigned int)4095))  //(((m + 63)/64) * 64)
#define ALIGN16(m)  (((m + 15)/16) * 16)
#define ALIGN4(m)  (((m + 3)/4) * 4)

int AndroidSurfaceOutputGCx00::gcu_frame_color_convert (uint8_t* aSrcData,uint32_t aSrcPhyAddress, uint8_t* aDstData, uint32_t aDstPhyAddress)
{
    GCUSurface pSrcSurface = NULL ,pDstSurface = NULL;
    GCU_BLT_DATA bltData;
    GCU_SURFACE_DATA    surfaceData;
    GCU_RECT            SrcRect;
    GCU_RECT            DstRect;
    int i;
    int SrcQueueSize = sizeof(SrcSurfaceInfoQ)/sizeof(GCUSurfaceInfo);
    int DstQueueSize = sizeof(DstSurfaceInfoQ)/sizeof(GCUSurfaceInfo);
    if(pGcuContext == NULL)
    {
        return -1;
    }
    for(i=0; i<SrcQueueSize; i++){
        if(SrcSurfaceInfoQ[i].pSurface && SrcSurfaceInfoQ[i].pVirAddr == aSrcData){
            pSrcSurface = SrcSurfaceInfoQ[i].pSurface;
            break;
        }
    }
    for(i=0; i<DstQueueSize; i++){
        if(DstSurfaceInfoQ[i].pSurface && DstSurfaceInfoQ[i].pVirAddr == aDstData){
            pDstSurface = DstSurfaceInfoQ[i].pSurface;
            break;
        }
    }
    if(pSrcSurface == NULL){
        GCU_FORMAT format;
        if (iVideoSubFormat == PVMF_MIME_YUV420_PLANAR){
            format = GCU_FORMAT_I420;
        }else if (iVideoSubFormat == PVMF_MIME_YUV422_INTERLEAVED_UYVY){
            format = GCU_FORMAT_UYVY;
        }else{
            LOGE("not support color format");
            return -1;
        }
        pSrcSurface = _gcuCreatePreAllocBuffer(pGcuContext, ALIGN16(iVideoWidth), ALIGN4(iVideoHeight), format , GCU_TRUE, aSrcData, (aSrcPhyAddress==NULL)?GCU_FALSE:GCU_TRUE, (GCUPhysicalAddr)aSrcPhyAddress);
        if(pSrcSurface == NULL){
            LOGE(" GCU create src buffer failed");
            return -1;
        }
        for(i=0;i<SrcQueueSize;i++){
            if(SrcSurfaceInfoQ[i].pSurface == NULL){
                SrcSurfaceInfoQ[i].pSurface = pSrcSurface;
                SrcSurfaceInfoQ[i].pVirAddr = aSrcData;
                SrcSurfaceInfoQ[i].nPhyAddr = (GCUPhysicalAddr)aSrcPhyAddress;
                break;
            }
        }
    }
    if (pDstSurface == NULL){
        GCU_FORMAT format=GCU_FORMAT_RGB565;
        if (GCUOutputColorFormat == PIXEL_FORMAT_RGBA_8888){
            format = GCU_FORMAT_ARGB8888;
        }else if (GCUOutputColorFormat == PIXEL_FORMAT_RGB_565){
            format = GCU_FORMAT_RGB565;
        }
        pDstSurface = _gcuCreatePreAllocBuffer(pGcuContext, iGCUDstVideo_W, iGCUDstVideo_H, format , GCU_TRUE, aDstData, (aDstPhyAddress==NULL)?GCU_FALSE:GCU_TRUE,(GCUPhysicalAddr)aDstPhyAddress);
        if (pDstSurface == NULL){
            LOGE(" GCU create dst buffer failed");
            return -1;
        }
        for(i=0; i<DstQueueSize; i++){
            if(DstSurfaceInfoQ[i].pSurface == NULL){
                DstSurfaceInfoQ[i].pSurface = pDstSurface;
                DstSurfaceInfoQ[i].pVirAddr = aDstData;
                DstSurfaceInfoQ[i].nPhyAddr = (GCUPhysicalAddr)aDstPhyAddress;
                break;
            }
        }
    }
    /*5 Set Blt and gcuBlit*/
    SrcRect.left = 0;
    SrcRect.top  = 0;
    SrcRect.right = SrcRect.left + iVideoDisplayWidth;
    SrcRect.bottom = SrcRect.top + iVideoDisplayHeight;

    DstRect.left = 0;
    DstRect.top  = 0;
    DstRect.right = DstRect.left + iGCUDstVideo_W;
    DstRect.bottom = DstRect.top + iGCUDstVideo_H;

    memset(&bltData, 0, sizeof(bltData));
    bltData.pSrcSurface         = pSrcSurface;
    bltData.pDstSurface         = pDstSurface;
    bltData.pSrcRect        = &SrcRect;
    bltData.pDstRect        = &DstRect;
    bltData.rotation = GCU_ROTATION_0;
    gcuBlit(pGcuContext, &bltData);
    gcuFinish(pGcuContext);

#ifdef GCU_DUMP
    {
        char dumpfile[30];
        static int dump_i = 0;
        sprintf (dumpfile, "/data/gc500%d.bmp",dump_i++);
        if(dump_i<10)
        if(pGcuContext && pDstSurface){
            _gcuDumpSurface(pGcuContext, pDstSurface, dumpfile);
        }
    }
#endif
    return 0;
}

OSCL_EXPORT_REF AndroidSurfaceOutputGCx00::AndroidSurfaceOutputGCx00():AndroidSurfaceOutput()
{
    pGcuContext = gcu_init(NULL);
    for(int i=0;i<sizeof(SrcSurfaceInfoQ)/sizeof(GCUSurfaceInfo);i++){
        SrcSurfaceInfoQ[i].pSurface = NULL;
    }
    for(int i=0;i<sizeof(DstSurfaceInfoQ)/sizeof(GCUSurfaceInfo);i++){
        DstSurfaceInfoQ[i].pSurface = NULL;
    }
    pExtAlloc = NULL;
    iNumRequestedMIOBuffer = MIN_VIDMIO_BUFFER;
}

OSCL_EXPORT_REF AndroidSurfaceOutputGCx00::~AndroidSurfaceOutputGCx00()
{
#ifdef PERF
    LOGD("time_tot=%lldus, frame#=%d, avg=%lfus fps=%lf", time_tot_base, frame_num_base-1, (double)(time_tot_base)/(frame_num_base-1), (double)(frame_num_base-1)*1000000/(time_tot_base));
#endif
    for(int i=0;i<sizeof(SrcSurfaceInfoQ)/sizeof(GCUSurfaceInfo);i++){
        if(SrcSurfaceInfoQ[i].pSurface){
            _gcuDestroyBuffer(pGcuContext, SrcSurfaceInfoQ[i].pSurface);
            SrcSurfaceInfoQ[i].pSurface = NULL;
        }
    }
    for(int i=0;i<sizeof(DstSurfaceInfoQ)/sizeof(GCUSurfaceInfo);i++){
        if(DstSurfaceInfoQ[i].pSurface){
            _gcuDestroyBuffer(pGcuContext, DstSurfaceInfoQ[i].pSurface);
            DstSurfaceInfoQ[i].pSurface = NULL;
        }
    }
    gcu_exit(pGcuContext);
    pGcuContext = NULL;
}

PVMFStatus AndroidSurfaceOutputGCx00::getParametersSync(PvmiMIOSession aSession, PvmiKeyType aIdentifier,
                                              PvmiKvp*& aParameters, int& num_parameter_elements,
                                              PvmiCapabilityContext aContext)
{
    OSCL_UNUSED_ARG(aSession);
    OSCL_UNUSED_ARG(aContext);
    aParameters=NULL;

    // This is a query for the list of supported formats.
    if(pv_mime_strcmp(aIdentifier, INPUT_FORMATS_CAP_QUERY) == 0)
    {
        aParameters=(PvmiKvp*)oscl_malloc(sizeof(PvmiKvp));
        if (aParameters == NULL) return PVMFErrNoMemory;
        aParameters[num_parameter_elements++].value.pChar_value=(char*) PVMF_MIME_YUV420;

        return PVMFSuccess;
    }
    else if (pv_mime_strcmp(aIdentifier, PVMF_BUFFER_ALLOCATOR_KEY) == 0)
    {
        aParameters=(PvmiKvp*)oscl_malloc(sizeof(PvmiKvp));
        if (aParameters == NULL) return PVMFErrNoMemory;
        aParameters[num_parameter_elements++].value.key_specific_value = (OsclAny*)pExtAlloc;

        return PVMFSuccess;
    }
    //unrecognized key.
    return PVMFFailure;
}
void AndroidSurfaceOutputGCx00::setParametersSync(PvmiMIOSession aSession, PvmiKvp* aParameters,
                                        int num_elements, PvmiKvp * & aRet_kvp)
{
    OSCL_UNUSED_ARG(aSession);

    aRet_kvp = NULL;

    LOGV("setParametersSync");

#ifdef PERF
    bStartPerf = false;
    char value[PROPERTY_VALUE_MAX];
    property_get("enable.pvplayer.profiling", value, "0");
    if (atoi(value)){
        bStartPerf = true;
    }
    time_tot_base = 0;
    frame_num_base = 0;
#endif

    for (int32 i=0;i<num_elements;i++)
    {
        //Check against known video parameter keys...
        if (pv_mime_strcmp(aParameters[i].key, MOUT_VIDEO_FORMAT_KEY) == 0)
        {
            iVideoFormatString=aParameters[i].value.pChar_value;
            iVideoFormat=iVideoFormatString.get_str();
            PVLOGGER_LOGMSG(PVLOGMSG_INST_LLDBG, iLogger, PVLOGMSG_STACK_TRACE,
                (0,"AndroidSurfaceOutput::setParametersSync() Video Format Key, Value %s",iVideoFormatString.get_str()));
        }
        else if (pv_mime_strcmp(aParameters[i].key, MOUT_VIDEO_WIDTH_KEY) == 0)
        {
            iVideoWidth=(int32)aParameters[i].value.uint32_value;
            iVideoParameterFlags |= VIDEO_WIDTH_VALID;
            LOGV("iVideoWidth=%d", iVideoWidth);
            PVLOGGER_LOGMSG(PVLOGMSG_INST_LLDBG, iLogger, PVLOGMSG_STACK_TRACE,
                (0,"AndroidSurfaceOutput::setParametersSync() Video Width Key, Value %d",iVideoWidth));
        }
        else if (pv_mime_strcmp(aParameters[i].key, MOUT_VIDEO_HEIGHT_KEY) == 0)
        {
            iVideoHeight=(int32)aParameters[i].value.uint32_value;
            iVideoParameterFlags |= VIDEO_HEIGHT_VALID;
            LOGV("iVideoHeight=%d", iVideoHeight);
            PVLOGGER_LOGMSG(PVLOGMSG_INST_LLDBG, iLogger, PVLOGMSG_STACK_TRACE,
                (0,"AndroidSurfaceOutput::setParametersSync() Video Height Key, Value %d",iVideoHeight));
        }
        else if (pv_mime_strcmp(aParameters[i].key, MOUT_VIDEO_DISPLAY_HEIGHT_KEY) == 0)
        {
            iVideoDisplayHeight=(int32)aParameters[i].value.uint32_value;
            iVideoParameterFlags |= DISPLAY_HEIGHT_VALID;
            LOGV("iVideoDisplayHeight=%d", iVideoDisplayHeight);
            PVLOGGER_LOGMSG(PVLOGMSG_INST_LLDBG, iLogger, PVLOGMSG_STACK_TRACE,
                (0,"AndroidSurfaceOutput::setParametersSync() Video Display Height Key, Value %d",iVideoDisplayHeight));
        }
        else if (pv_mime_strcmp(aParameters[i].key, MOUT_VIDEO_DISPLAY_WIDTH_KEY) == 0)
        {
            iVideoDisplayWidth=(int32)aParameters[i].value.uint32_value;
            iVideoParameterFlags |= DISPLAY_WIDTH_VALID;
            LOGV("iVideoDisplayWidth=%d", iVideoDisplayWidth);
            PVLOGGER_LOGMSG(PVLOGMSG_INST_LLDBG, iLogger, PVLOGMSG_STACK_TRACE,
                (0,"AndroidSurfaceOutput::setParametersSync() Video Display Width Key, Value %d",iVideoDisplayWidth));
        }
        else if (pv_mime_strcmp(aParameters[i].key, MOUT_VIDEO_SUBFORMAT_KEY) == 0)
        {
            iVideoSubFormat=aParameters[i].value.pChar_value;
            iVideoParameterFlags |= VIDEO_SUBFORMAT_VALID;
            PVLOGGER_LOGMSG(PVLOGMSG_INST_LLDBG, iLogger, PVLOGMSG_STACK_TRACE,
                    (0,"AndroidSurfaceOutput::setParametersSync() Video SubFormat Key, Value %s",iVideoSubFormat.getMIMEStrPtr()));

LOGV("VIDEO SUBFORMAT SET TO %s\n",iVideoSubFormat.getMIMEStrPtr());
        }
        else if (pv_mime_strcmp(aParameters[i].key, PVMF_FORMAT_SPECIFIC_INFO_KEY_YUV) == 0)
        {
            PVMFYuvFormatSpecificInfo0* fsiInfo = OSCL_STATIC_CAST(PVMFYuvFormatSpecificInfo0*, aParameters[i].value.key_specific_value);
            
            iVideoFormat = fsiInfo->video_format;
            iVideoSubFormat = fsiInfo->video_format;
            iVideoParameterFlags |= VIDEO_SUBFORMAT_VALID;
            LOGD("Video Format = %s", iVideoSubFormat.getMIMEStrPtr());

            iVideoWidth = fsiInfo->width;
            iVideoParameterFlags |= VIDEO_WIDTH_VALID;
            LOGD("Video Width = %d", iVideoWidth);

            iVideoHeight = fsiInfo->height;
            iVideoParameterFlags |= VIDEO_HEIGHT_VALID;
            LOGD("Video Height = %d", iVideoHeight);

            iVideoDisplayWidth = fsiInfo->display_width;
            iVideoParameterFlags |= DISPLAY_WIDTH_VALID;
            LOGD("Display Width = %d", iVideoDisplayWidth);

            iVideoDisplayHeight = fsiInfo->display_height;
            iVideoParameterFlags |= DISPLAY_HEIGHT_VALID;
            LOGD("Display Height = %d", iVideoDisplayHeight);

            if (iVideoSubFormat == PVMF_MIME_YUV422_INTERLEAVED_UYVY){
                /*convert the unit from bytes to pixel*/
                iVideoWidth = iVideoWidth >> 1;
            }
            iNumRequestedMIOBuffer = fsiInfo->num_buffers;
            if (iNumRequestedMIOBuffer > MAX_VIDMIO_BUFFER)
                iNumRequestedMIOBuffer = MAX_VIDMIO_BUFFER;
            else if (iNumRequestedMIOBuffer < MIN_VIDMIO_BUFFER)
                iNumRequestedMIOBuffer = MIN_VIDMIO_BUFFER;
            LOGD("MIO would allocate %d buffers", iNumRequestedMIOBuffer);

            iRequestedMIOBufSize = fsiInfo->buffer_size;
            LOGD("Requested MIO Buffer Size = %d", iRequestedMIOBufSize);

            bUseMIOBuffer = true;
            pExtAlloc = NULL;
            LOGD("MIO finished parsing compact YUV info");
        }
        else
        {
            //if we get here the key is unrecognized.

            PVLOGGER_LOGMSG(PVLOGMSG_INST_LLDBG, iLogger, PVLOGMSG_STACK_TRACE,
                (0,"AndroidSurfaceOutput::setParametersSync() Error, unrecognized key = %s", aParameters[i].key));

            //set the return value to indicate the unrecognized key
            //and return.
            aRet_kvp = &aParameters[i];
            return;
        }
    }
    uint32 mycache = iVideoParameterFlags ;
    // if initialization is complete, update the app display info
    if( checkVideoParameterFlags() )
    {
       if (initCheck() == false)
       {
           LOGD("initCheck failed");
           aRet_kvp = &aParameters[0];
           return;
        }
        LOGD("initCheck succeeded");
    }

    iVideoParameterFlags = mycache;

    // when all necessary parameters are received, send 
    // PVMFMIOConfigurationComplete event to observer
    if(!iIsMIOConfigured && checkVideoParameterFlags() )
    {
        iIsMIOConfigured = true;
        if(iObserver)
        {
            iObserver->ReportInfoEvent(PVMFMIOConfigurationComplete);
        }
    }
}
// create a frame buffer for software codecs
OSCL_EXPORT_REF bool AndroidSurfaceOutputGCx00::initCheck()
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
    int iResolutionLCD_W=0, iResolutionLCD_H=0;
    GCUOutputColorFormat = PIXEL_FORMAT_RGBA_8888; 
    iGCUDstVideo_W = ALIGN16(frameWidth);
    iGCUDstVideo_H = ALIGN4(frameHeight);

    gcu_lcd_resolution(&iResolutionLCD_W, &iResolutionLCD_H);
   
    
    if(displayWidth > displayHeight){
        if(iResolutionLCD_W < iResolutionLCD_H){
            int w=iResolutionLCD_H;
            int h=iResolutionLCD_W;
            if (displayWidth > w && displayHeight > h){
                iGCUDstVideo_W = ALIGN16(w);
                iGCUDstVideo_H = ALIGN4(h);
            }
        }else{
            if(displayWidth > iResolutionLCD_W && displayHeight > iResolutionLCD_H){
                iGCUDstVideo_W = ALIGN16(iResolutionLCD_W);
                iGCUDstVideo_H = ALIGN4(iResolutionLCD_H);
            }
        }
    }else{
        if (iResolutionLCD_W < iResolutionLCD_H){
            if(displayWidth > iResolutionLCD_W && displayHeight > iResolutionLCD_H){
                iGCUDstVideo_W = ALIGN16(iResolutionLCD_W);
                iGCUDstVideo_H = ALIGN4(iResolutionLCD_H);
            }
        }else{
            int w=iResolutionLCD_H;
            int h=iResolutionLCD_W;
            if (displayWidth > w && displayHeight > h){
                iGCUDstVideo_W = ALIGN16(w);
                iGCUDstVideo_H = ALIGN4(h);
            }
        }
    }
    // RGB-565 frames are 2 bytes/pixel
    displayWidth = (displayWidth + 1) & -2;
    displayHeight = (displayHeight + 1) & -2;
    frameWidth = (frameWidth + 1) & -2;
    frameHeight = (frameHeight + 1) & -2;
    if (GCUOutputColorFormat == PIXEL_FORMAT_RGBA_8888){
        frameSize = iGCUDstVideo_W*iGCUDstVideo_H*4;
    }else if (GCUOutputColorFormat == PIXEL_FORMAT_RGB_565){
        frameSize = iGCUDstVideo_W*iGCUDstVideo_H*2;
    }

    // create frame buffer heap and register with surfaceflinger
    //sp<MemoryHeapBase> heap = new MemoryHeapBase(frameSize * kBufferCount);
    sp<MemoryHeapBase> heap = new MemoryHeapBase("/dev/pmem_adsp", frameSize * kBufferCount);
    if (heap->heapID() < 0) {
        LOGE("Error creating frame buffer heap");
        return false;
    }else {
        sp<MemoryHeapPmem> pmemHeap = new MemoryHeapPmem(heap);
        pmemHeap->slap();
        heap = pmemHeap;
    }
    memset(static_cast<uint8*>(heap->base()), 0, frameSize * kBufferCount);
    mBufferHeap = ISurface::BufferHeap(iGCUDstVideo_W, iGCUDstVideo_H,
              iGCUDstVideo_W, iGCUDstVideo_H, GCUOutputColorFormat, heap); 
    //mBufferHeap = ISurface::BufferHeap(displayWidth, displayHeight,
    //        frameWidth, frameHeight, HAL_PIXEL_FORMAT_YCbCr_420_P, heap);
    mSurface->registerBuffers(mBufferHeap);

    // create frame buffers
    for (int i = 0; i < kBufferCount; i++) {
        mFrameBuffers[i] = i * frameSize;
    }

    // initialize software color converter
    LOGV("video = %d x %d", displayWidth, displayHeight);
    LOGV("frame = %d x %d", frameWidth, frameHeight);
    LOGV("frame #bytes = %d", frameSize);

    // register frame buffers with SurfaceFlinger
    mFrameBufferIndex = 0;

    if (!pExtAlloc && bUseMIOBuffer)
    {
        pExtAlloc = new MrvlBufferAlloc(iRequestedMIOBufSize, iNumRequestedMIOBuffer, 1);
    }

    mInitialized = true;
    mPvPlayer->sendEvent(MEDIA_SET_VIDEO_SIZE, iVideoDisplayWidth, iVideoDisplayHeight);

    return mInitialized;
}


PVMFStatus AndroidSurfaceOutputGCx00::writeFrameBuf(uint8* aData, uint32 aDataLen, const PvmiMediaXferHeader& data_header_info)
{
    uint32 pSrcPhyAddress=0;
    uint32 pDstPhyAddress=0;
    void* va_base;
    uint32 offset;
    struct pmem_region region;
    if (phy_cont_getpa(aData)){
        pSrcPhyAddress = phy_cont_getpa(aData);
    }else if (data_header_info.private_data_ptr){
        OMX_BUFFERHEADERTYPE_IPPEXT *header = (OMX_BUFFERHEADERTYPE_IPPEXT*)data_header_info.private_data_ptr;
        pSrcPhyAddress = header->nPhyAddr;
    }
    va_base = mBufferHeap.heap->getBase();
    if(ioctl(mBufferHeap.heap->getHeapID(), PMEM_GET_PHYS, &region) == 0) {
       offset = static_cast<uint8*>(mBufferHeap.heap->base()) + mFrameBuffers[mFrameBufferIndex] - (uint8*)va_base;
       pDstPhyAddress = (region.offset + offset);
    }
    // post to SurfaceFlinger
    if ((mSurface != NULL) && (mBufferHeap.heap != NULL)) {
        if (++mFrameBufferIndex == kBufferCount) mFrameBufferIndex = 0;
        //iColorConverter->Convert(aData, static_cast<uint8*>(mBufferHeap.heap->base()) + mFrameBuffers[mFrameBufferIndex]);
        if (((uint32_t)aData & 0x3F) || (iVideoWidth & 0xF) || (iVideoHeight & 0x3)){
            /*gcu has 64 bytes aligment requirement*/
            if (mBackupMemoryHeap == NULL || mBackupMemoryHeap->heapID() < 0){
                int MinSize = (ALIGN16(iVideoWidth) * ALIGN4(iVideoHeight));
                if (iVideoSubFormat == PVMF_MIME_YUV420_PLANAR){
                    MinSize = MinSize * 3/2;
                }else if (iVideoSubFormat == PVMF_MIME_YUV422_INTERLEAVED_UYVY){
                    MinSize = MinSize*2;
                }
                if (aDataLen < MinSize){
                    aDataLen = MinSize;
                }
                mBackupMemoryHeap = new MemoryHeapBase("/dev/pmem_adsp", aDataLen+1024);
                if (mBackupMemoryHeap->heapID() < 0) {
                    LOGI("Creating physical memory heap failed, reverting to regular heap.");
                } else {
                    sp<MemoryHeapPmem> pmemHeap = new MemoryHeapPmem(mBackupMemoryHeap);
                    pmemHeap->slap();
                    mBackupMemoryHeap = pmemHeap;
                }
                LOGW("The Framebuffer is not 64 bytes aligned");
            }
            va_base = mBackupMemoryHeap->getBase();
            if(ioctl(mBackupMemoryHeap->getHeapID(), PMEM_GET_PHYS, &region) == 0)
            {
               offset = static_cast<uint8_t*>(mBackupMemoryHeap->base()) - (uint8_t*)va_base;
               pSrcPhyAddress = (region.offset + offset);
            }
            if ((iVideoWidth & 0xF) || (iVideoHeight & 0x3)){
                if (iVideoSubFormat == PVMF_MIME_YUV420_PLANAR){
                    int i;
                    uint8_t *pY=aData;
                    uint8_t *pU=aData + iVideoWidth*iVideoHeight;
                    uint8_t *pV=pU + iVideoWidth*iVideoHeight/4;
                    uint8_t *pDst= (uint8_t *)mBackupMemoryHeap->getBase();
                    for(i=0; i<iVideoHeight; i++){
                        memcpy(pDst, pY, iVideoWidth);
                        pY += iVideoWidth;
                        pDst += ALIGN16(iVideoWidth);
                    }
                    pDst += (ALIGN4(iVideoHeight) - iVideoHeight)*ALIGN16(iVideoWidth);
                    for (i=0; i<iVideoHeight/2; i++){
                       memcpy(pDst, pU, iVideoWidth/2);
                       pU += iVideoWidth/2;
                       pDst += ALIGN16(iVideoWidth)/2;
                    }
                    pDst += (ALIGN4(iVideoHeight) - iVideoHeight)*ALIGN16(iVideoWidth)/4;
                    for (i=0; i<iVideoHeight/2; i++){
                        memcpy(pDst, pV, iVideoWidth/2);
                        pV += iVideoWidth/2;
                        pDst += ALIGN16(iVideoWidth)/2;
                    }
                }else if (iVideoSubFormat == PVMF_MIME_YUV422_INTERLEAVED_UYVY){
                    int i;
                    uint8_t *pY=aData;
                    uint8_t *pDst= (uint8_t *)mBackupMemoryHeap->getBase();
                    for(i=0; i<iVideoHeight; i++){
                        memcpy(pDst, pY, iVideoWidth*2);
                        pY += iVideoWidth*2;
                        pDst += ALIGN16(iVideoWidth)*2;
                    }
                }
            }else{
                memcpy((uint8_t *)mBackupMemoryHeap->getBase(), aData, aDataLen);
            }
            gcu_frame_color_convert((uint8_t *)(uint8_t *)mBackupMemoryHeap->getBase(), pSrcPhyAddress, static_cast<uint8*>(mBufferHeap.heap->base()) + mFrameBuffers[mFrameBufferIndex],pDstPhyAddress);
        }else{
            gcu_frame_color_convert(aData, pSrcPhyAddress, static_cast<uint8*>(mBufferHeap.heap->base()) + mFrameBuffers[mFrameBufferIndex],pDstPhyAddress);
        }
        mSurface->postBuffer(mFrameBuffers[mFrameBufferIndex]);
    }
#ifdef PERF
    if (bStartPerf){
        clock_gettime(CLOCK_REALTIME, &t_stop_base);
        if (frame_num_base > 0){
            time_tot_base += ((GET_TIME_INTERVAL_USEC(t_start_base, t_stop_base)));
        }
        t_start_base = t_stop_base;
        frame_num_base++;
    }
#endif
    return PVMFSuccess;
}




