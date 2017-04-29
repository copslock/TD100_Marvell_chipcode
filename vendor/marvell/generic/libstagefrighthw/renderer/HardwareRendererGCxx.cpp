/***************************************************************************************** 
 * Copyright (c) 2009, Marvell International Ltd. 
 * All Rights Reserved.
 *****************************************************************************************/
//#define LOG_NDEBUG 0

#define LOG_TAG "HardwareRendererGC"
#include <utils/Log.h>
#include "HardwareRendererGCxx.h"
#include <binder/MemoryHeapBase.h>
#include <binder/MemoryHeapPmem.h>
#include <media/stagefright/MediaDebug.h>
#include <surfaceflinger/ISurface.h>

#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <linux/fb.h>
#include "gcu.h"
#include "OMX_IppDef.h"
#include <linux/android_pmem.h>
#include <sys/ioctl.h>
namespace android {

#define ALIGN16(m)  (((m + 15) & ~15))
#define ALIGN4(m)  (((m + 3) & ~3))

#ifdef PERF
#include <cutils/properties.h>
#define GET_TIME_INTERVAL_USEC(A, B) (((((B).tv_sec)-((A).tv_sec))*1000000) + ((((B).tv_nsec)-((A).tv_nsec))/1000))
#define GET_TIME_INTERVAL_NSEC(A, B) (((((B).tv_sec)-((A).tv_sec))*1000000000LL) + ((((B).tv_nsec)-((A).tv_nsec))))
#endif
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
static int gcu_lcd_resolution(int32_t *w, int32_t *h){
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
int HardwareRendererGC::gcu_frame_color_convert (uint8_t* aSrcData,uint32_t aSrcPhyAddress, uint8_t* aDstData, uint32_t aDstPhyAddress)
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
        if (mColorFormat == OMX_COLOR_FormatYUV420Planar){
            format = GCU_FORMAT_I420;
        }else if (mColorFormat == OMX_COLOR_FormatCbYCrY){
            format = GCU_FORMAT_UYVY;
        }else{
            LOGE("not support color format");
            return -1;
        }
        pSrcSurface = _gcuCreatePreAllocBuffer(pGcuContext, ALIGN16(mDecodedWidth), ALIGN4(mDecodedHeight), format , GCU_TRUE, aSrcData, (aSrcPhyAddress==NULL)?GCU_FALSE:GCU_TRUE, (GCUPhysicalAddr)aSrcPhyAddress);
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
    SrcRect.right = SrcRect.left + mDisplayWidth;
    SrcRect.bottom = SrcRect.top + mDisplayHeight;

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

HardwareRendererGC *HardwareRendererGC::CreateInstance(
        OMX_COLOR_FORMATTYPE colorFormat,
        const sp<ISurface> &surface,
        size_t displayWidth, size_t displayHeight,
        size_t decodedWidth, size_t decodedHeight,
        int32_t rotationDegrees){
    HardwareRendererGC * instance = new HardwareRendererGC(
            colorFormat, surface,
            displayWidth, displayHeight,
            decodedWidth, decodedHeight,rotationDegrees);
    if(instance->initCheck() == OK){
        return instance;
    }else {
        delete instance;
        return NULL;
    }
}

HardwareRendererGC::HardwareRendererGC(
        OMX_COLOR_FORMATTYPE colorFormat,
        const sp<ISurface> &surface,
        size_t displayWidth, size_t displayHeight,
        size_t decodedWidth, size_t decodedHeight,
        int32_t rotationDegrees)
    : mInitCheck(NO_INIT),
      mColorFormat(colorFormat),
      mISurface(surface),
      mDisplayWidth(displayWidth),
      mDisplayHeight(displayHeight),
      mDecodedWidth(decodedWidth),
      mDecodedHeight(decodedHeight),
      mFrameSize(mDecodedWidth * mDecodedHeight * 2),  // RGB565
      mIndex(0) {
    LOGD("display width %d", mDisplayWidth);
    LOGD("display height %d",mDisplayHeight);
    LOGD("decoded width %d", mDecodedWidth);
    LOGD("decoded height %d",mDecodedHeight);
    LOGD("container rotation is: %d",rotationDegrees);

    GCUOutputColorFormat = PIXEL_FORMAT_RGBA_8888;
    int iResolutionLCD_Wi=0, iResolutionLCD_Hi=0; //to remove the compile warning...
    
    iGCUDstVideo_W = ALIGN16(mDecodedWidth);
    iGCUDstVideo_H = ALIGN4(mDecodedHeight);

    gcu_lcd_resolution(&iResolutionLCD_Wi, &iResolutionLCD_Hi);
    unsigned int iResolutionLCD_W=iResolutionLCD_Wi;
    unsigned int iResolutionLCD_H=iResolutionLCD_Hi;
   
    if(mDisplayWidth > mDisplayHeight){
        if(iResolutionLCD_W < iResolutionLCD_H){
            unsigned int w=iResolutionLCD_H;
            unsigned int h=iResolutionLCD_W;
            if (mDisplayWidth > w && mDisplayHeight > h){
                iGCUDstVideo_W = ALIGN16(w);
                iGCUDstVideo_H = ALIGN4(h);
            }
        }else{
            if(mDisplayWidth > iResolutionLCD_W && mDisplayHeight > iResolutionLCD_H){
                iGCUDstVideo_W = ALIGN16(iResolutionLCD_W);
                iGCUDstVideo_H = ALIGN4(iResolutionLCD_H);
            }
        }
    }else{
        if (iResolutionLCD_W < iResolutionLCD_H){
            if(mDisplayWidth > iResolutionLCD_W && mDisplayHeight > iResolutionLCD_H){
                iGCUDstVideo_W = ALIGN16(iResolutionLCD_W);
                iGCUDstVideo_H = ALIGN4(iResolutionLCD_H);
            }
        }else{
            unsigned int w=iResolutionLCD_H;
            unsigned int h=iResolutionLCD_W;
            if (mDisplayWidth > w && mDisplayHeight > h){
                iGCUDstVideo_W = ALIGN16(w);
                iGCUDstVideo_H = ALIGN4(h);
            }
        }
    }

    if (GCUOutputColorFormat == PIXEL_FORMAT_RGBA_8888){
        mFrameSize = iGCUDstVideo_W*iGCUDstVideo_H*4;
    }else if (GCUOutputColorFormat == PIXEL_FORMAT_RGB_565){
        mFrameSize = iGCUDstVideo_W*iGCUDstVideo_H*2;
    }   
    mMemoryHeap = new MemoryHeapBase("/dev/pmem_adsp", 2 * mFrameSize);
    if (mMemoryHeap->heapID() < 0) {
        LOGI("Creating physical memory heap failed, reverting to regular heap.");
        mMemoryHeap = new MemoryHeapBase(2 * mFrameSize);
    } else {
        sp<MemoryHeapPmem> pmemHeap = new MemoryHeapPmem(mMemoryHeap);
        pmemHeap->slap();
        mMemoryHeap = pmemHeap;
    }

    CHECK(mISurface.get() != NULL);
    CHECK(mDecodedWidth > 0);
    CHECK(mDecodedHeight > 0);
    CHECK(mMemoryHeap->heapID() >= 0);

    uint32_t orientation;
    switch (rotationDegrees) {
        case 0: orientation = ISurface::BufferHeap::ROT_0; break;
        case 90: orientation = ISurface::BufferHeap::ROT_90; break;
        case 180: orientation = ISurface::BufferHeap::ROT_180; break;
        case 270: orientation = ISurface::BufferHeap::ROT_270; break;
        default: orientation = ISurface::BufferHeap::ROT_0; break;
    }

    pGcuContext = gcu_init(NULL);
    if (pGcuContext == NULL){
        LOGE("GCU init failed");
        return;
    }
    for(unsigned int i=0;i<sizeof(SrcSurfaceInfoQ)/sizeof(GCUSurfaceInfo);i++){
        SrcSurfaceInfoQ[i].pSurface = NULL;
    }
    for(unsigned int i=0;i<sizeof(DstSurfaceInfoQ)/sizeof(GCUSurfaceInfo);i++){
        DstSurfaceInfoQ[i].pSurface = NULL;
    }
    
    ISurface::BufferHeap bufferHeap(
            iGCUDstVideo_W, iGCUDstVideo_H,
            iGCUDstVideo_W, iGCUDstVideo_H,
            GCUOutputColorFormat,
            orientation, 0,
            mMemoryHeap);

    status_t err = mISurface->registerBuffers(bufferHeap);

    if (err != OK) {
        LOGW("ISurface failed to register buffers (0x%08x)", err);
    }

    mInitCheck = err;

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
}

HardwareRendererGC::~HardwareRendererGC() {
    for(unsigned int i=0;i<sizeof(SrcSurfaceInfoQ)/sizeof(GCUSurfaceInfo);i++){
        if(SrcSurfaceInfoQ[i].pSurface){
            _gcuDestroyBuffer(pGcuContext, SrcSurfaceInfoQ[i].pSurface);
            SrcSurfaceInfoQ[i].pSurface = NULL;
        }
    }
    for(unsigned int i=0;i<sizeof(DstSurfaceInfoQ)/sizeof(GCUSurfaceInfo);i++){
        if(DstSurfaceInfoQ[i].pSurface){
            _gcuDestroyBuffer(pGcuContext, DstSurfaceInfoQ[i].pSurface);
            DstSurfaceInfoQ[i].pSurface = NULL;
        }
    }
    gcu_exit(pGcuContext);
    pGcuContext = NULL;
    mISurface->unregisterBuffers();
#ifdef PERF
    LOGD("time_tot=%ldus, frame#=%d, avg=%lfus fps=%lf", time_tot_base, frame_num_base-1, (double)(time_tot_base)/(frame_num_base-1), (double)(frame_num_base-1)*1000000/(time_tot_base));
#endif
}

status_t HardwareRendererGC::initCheck() const {

    return mInitCheck;
}

void HardwareRendererGC::render(
        const void *data, size_t size, void *platformPrivate, sp<MemoryHeapBase> memoryHeap) {
    if (mInitCheck != OK) {
        return;
    }
    
    size_t offset = mIndex * mFrameSize;
    void *dst = (uint8_t *)mMemoryHeap->getBase() + offset;
    void* va_base;
    uint32_t off;
    struct pmem_region region;
    uint32_t pSrcPhyAddress=0;
    uint32_t pDstPhyAddress=0;

    va_base = mMemoryHeap->getBase();
    if(ioctl(mMemoryHeap->getHeapID(), PMEM_GET_PHYS, &region) == 0)
    { 
       off = static_cast<uint8_t*>(mMemoryHeap->base()) + offset - (uint8_t*)va_base;
       pDstPhyAddress = (region.offset + off);
    }

    if( platformPrivate) {
        OMX_BUFFERHEADERTYPE_IPPEXT *header = (OMX_BUFFERHEADERTYPE_IPPEXT*)platformPrivate;
        pSrcPhyAddress = header->nPhyAddr;
    }
    if (((uint32_t)data & 0x3F) || (mDecodedWidth & 0xF) || (mDecodedHeight & 0x3)){
        /*gcu has 64 bytes aligment requirement*/
        if (mBackupMemoryHeap == NULL || mBackupMemoryHeap->heapID() < 0){
            unsigned int MinSize = ALIGN16(mDecodedWidth) * ALIGN4(mDecodedHeight);
            if (mColorFormat == OMX_COLOR_FormatYUV420Planar){
                MinSize = MinSize * 3/2;
            }else if (mColorFormat == OMX_COLOR_FormatCbYCrY) {
                MinSize = MinSize*2;
            }
            if (size < MinSize){
                size = MinSize;
            }
            mBackupMemoryHeap = new MemoryHeapBase("/dev/pmem_adsp", size+1024);
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
           off = static_cast<uint8_t*>(mBackupMemoryHeap->base()) - (uint8_t*)va_base;
           pSrcPhyAddress = (region.offset + off);
        }
        if ((mDecodedWidth & 0xF) || (mDecodedHeight & 0x3)){
            if (mColorFormat == OMX_COLOR_FormatYUV420Planar){
                unsigned int i;
                uint8_t *pY=(uint8_t *)data;
                uint8_t *pU=(uint8_t *)data + mDecodedWidth*mDecodedHeight;
                uint8_t *pV=pU + mDecodedWidth*mDecodedHeight/4;
                uint8_t *pDst= (uint8_t *)mBackupMemoryHeap->getBase();
                for(i=0; i<mDecodedHeight; i++){
                    memcpy(pDst, pY, mDecodedWidth);
                    pY += mDecodedWidth;
                    pDst += ALIGN16(mDecodedWidth);
                }
                pDst += (ALIGN4(mDecodedHeight) - mDecodedHeight)*(ALIGN16(mDecodedWidth));
                for (i=0; i<mDecodedHeight/2; i++){
                   memcpy(pDst, pU, mDecodedWidth/2);
                   pU += mDecodedWidth/2;
                   pDst += ALIGN16(mDecodedWidth)/2;
                }
                pDst += (ALIGN4(mDecodedHeight) - mDecodedHeight)*ALIGN16(mDecodedWidth)/4;
                for (i=0; i<mDecodedHeight/2; i++){
                    memcpy(pDst, pV, mDecodedWidth/2);
                   pV += mDecodedWidth/2;
                   pDst += ALIGN16(mDecodedWidth)/2;
               }
            }else if (mColorFormat == OMX_COLOR_FormatCbYCrY){
                unsigned int i;
                uint8_t *pY= (uint8_t *)data;
                uint8_t *pDst= (uint8_t *)mBackupMemoryHeap->getBase();
                for(i=0; i<mDecodedHeight; i++){
                    memcpy(pDst, pY, mDecodedWidth*2);
                    pY += mDecodedWidth*2;
                    pDst += ALIGN16(mDecodedWidth)*2;
                }
            }
        }else{
            memcpy((uint8_t *)mBackupMemoryHeap->getBase(), data, size);
        }
        gcu_frame_color_convert((uint8_t *)(uint8_t *)mBackupMemoryHeap->getBase(), pSrcPhyAddress, (uint8_t *)dst,pDstPhyAddress);
    }else{
        gcu_frame_color_convert((uint8_t *)data, pSrcPhyAddress, (uint8_t *)dst,pDstPhyAddress);
    }
    mISurface->postBuffer(offset);
    mIndex = 1 - mIndex;

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
}

HardwareRendererDirectGC *HardwareRendererDirectGC::CreateInstance(
        OMX_COLOR_FORMATTYPE colorFormat,
        const sp<ISurface> &surface,
        size_t displayWidth, size_t displayHeight,
        size_t decodedWidth, size_t decodedHeight,
        int32_t rotationDegrees){
    HardwareRendererDirectGC * instance = new HardwareRendererDirectGC(
            colorFormat, surface,
            displayWidth, displayHeight,
            decodedWidth, decodedHeight,rotationDegrees);
    if(instance->initCheck() == OK){
        return instance;
    }else {
        delete instance;
        return NULL;
    }
}

HardwareRendererDirectGC::HardwareRendererDirectGC(
        OMX_COLOR_FORMATTYPE colorFormat,
        const sp<ISurface> &surface,
        size_t displayWidth, size_t displayHeight,
        size_t decodedWidth, size_t decodedHeight,
        int32_t rotationDegrees)
    : mInitCheck(NO_INIT),
      mColorFormat(colorFormat),
      mISurface(surface),
      mDisplayWidth(displayWidth),
      mDisplayHeight(displayHeight),
      mDecodedWidth(decodedWidth),
      mDecodedHeight(decodedHeight),
      mFrameSize(mDecodedWidth * mDecodedHeight * 2),  // RGB565
      orientation(0),
      memoryHeap_registered(false),
      mIndex(0) {
    LOGD("display width %d", mDisplayWidth);
    LOGD("display height %d",mDisplayHeight);
    LOGD("decoded width %d", mDecodedWidth);
    LOGD("decoded height %d",mDecodedHeight);

	switch(mColorFormat){
	case OMX_COLOR_FormatCbYCrY:
    	GCUOutputColorFormat = PIXEL_FORMAT_YCbCr_422_I ;
		break;

	case OMX_COLOR_FormatYUV420Planar:
    	GCUOutputColorFormat = PIXEL_FORMAT_YCbCr_420_P ;
		break;

	default :
		LOGE("%s, Invalid input format %d, only support OMX_COLOR_FormatCbYCrY and OMX_COLOR_FormatYUV420Planar", __FUNCTION__,mColorFormat);
		break; 
	}

    
    iGCUDstVideo_W = ALIGN16(mDecodedWidth);
    iGCUDstVideo_H = ALIGN4(mDecodedHeight);

    if (GCUOutputColorFormat == PIXEL_FORMAT_RGBA_8888){
        mFrameSize = iGCUDstVideo_W*iGCUDstVideo_H*4;
    }else if (GCUOutputColorFormat == PIXEL_FORMAT_RGB_565){
        mFrameSize = iGCUDstVideo_W*iGCUDstVideo_H*2;
    }else if (GCUOutputColorFormat == PIXEL_FORMAT_YCbCr_422_I){
        mFrameSize = iGCUDstVideo_W*iGCUDstVideo_H*2;
	}else if (GCUOutputColorFormat == PIXEL_FORMAT_YCbCr_420_P){
        mFrameSize = iGCUDstVideo_W*iGCUDstVideo_H*3/2;
	}

    CHECK(mISurface.get() != NULL);
    CHECK(mDecodedWidth > 0);
    CHECK(mDecodedHeight > 0);

    switch (rotationDegrees) {
        case 0: orientation = ISurface::BufferHeap::ROT_0; break;
        case 90: orientation = ISurface::BufferHeap::ROT_90; break;
        case 180: orientation = ISurface::BufferHeap::ROT_180; break;
        case 270: orientation = ISurface::BufferHeap::ROT_270; break;
        default: orientation = ISurface::BufferHeap::ROT_0; break;
    }

    mInitCheck = OK;

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
}

HardwareRendererDirectGC::~HardwareRendererDirectGC() {
    mISurface->unregisterBuffers();
#ifdef PERF
    LOGD("time_tot=%ldus, frame#=%d, avg=%lfus fps=%lf", time_tot_base, frame_num_base-1, (double)(time_tot_base)/(frame_num_base-1), (double)(frame_num_base-1)*1000000/(time_tot_base));
#endif
}

status_t HardwareRendererDirectGC::initCheck() const {

    return mInitCheck;
}

void HardwareRendererDirectGC::render(
        const void *data, size_t size, void *platformPrivate, sp<MemoryHeapBase> memoryHeap) {
    if (mInitCheck != OK) {
        return;
    }

    size_t		offset 			= 0; 
    uint32_t	pSrcPhyAddress	= 0;

    if( platformPrivate) {
        OMX_BUFFERHEADERTYPE_IPPEXT *header = (OMX_BUFFERHEADERTYPE_IPPEXT*)platformPrivate;
        pSrcPhyAddress = header->nPhyAddr;
    }

    if (((uint32_t)data & 0x3F) || (mDecodedWidth & 0xF) || (mDecodedHeight & 0x3) || (!pSrcPhyAddress) ){
        /*gcu has 64 bytes aligment requirement*/
        if (mBackupMemoryHeap == NULL || mBackupMemoryHeap->heapID() < 0){
            int MinSize = ALIGN16(mDecodedWidth) * ALIGN4(mDecodedHeight);
            if (mColorFormat == OMX_COLOR_FormatYUV420Planar){
                MinSize = MinSize * 3/2;
            }else if (mColorFormat == OMX_COLOR_FormatCbYCrY) {
                MinSize = MinSize*2;
            }
            if (size < MinSize){
                size = MinSize;
            }
            mBackupMemoryHeap = new MemoryHeapBase("/dev/pmem_adsp", 2*mFrameSize+1024);
            if (mBackupMemoryHeap->heapID() < 0) {
                LOGI("Creating physical memory heap failed, reverting to regular heap.");
            } else {
                sp<MemoryHeapPmem> pmemHeap = new MemoryHeapPmem(mBackupMemoryHeap);
                pmemHeap->slap();
                mBackupMemoryHeap = pmemHeap;
            }
            LOGW("The Framebuffer is not 64 bytes aligned");
        }

		//For width or height unaligned streams, do memcpy here
        if ((mDecodedWidth & 0xF) || (mDecodedHeight & 0x3)){
			//caculate the current buffer offset
    		offset 		= mIndex * mFrameSize;

            if (mColorFormat == OMX_COLOR_FormatYUV420Planar){
                int i;
                uint8_t *pY=(uint8_t *)data;
                uint8_t *pU=(uint8_t *)data + mDecodedWidth*mDecodedHeight;
                uint8_t *pV=pU + mDecodedWidth*mDecodedHeight/4;
                uint8_t *pDst= (uint8_t *)mBackupMemoryHeap->getBase() + offset;
                for(i=0; i<mDecodedHeight; i++){
                    memcpy(pDst, pY, mDecodedWidth);
                    pY += mDecodedWidth;
                    pDst += ALIGN16(mDecodedWidth);
                }
                pDst += (ALIGN4(mDecodedHeight) - mDecodedHeight)*(ALIGN16(mDecodedWidth));
                for (i=0; i<mDecodedHeight/2; i++){
                   memcpy(pDst, pU, mDecodedWidth/2);
                   pU += mDecodedWidth/2;
                   pDst += ALIGN16(mDecodedWidth)/2;
                }
                pDst += (ALIGN4(mDecodedHeight) - mDecodedHeight)*ALIGN16(mDecodedWidth)/4;
                for (i=0; i<mDecodedHeight/2; i++){
                    memcpy(pDst, pV, mDecodedWidth/2);
                   pV += mDecodedWidth/2;
                   pDst += ALIGN16(mDecodedWidth)/2;
               }
            }else if (mColorFormat == OMX_COLOR_FormatCbYCrY){
                int i;
                uint8_t *pY= (uint8_t *)data;
                uint8_t *pDst= (uint8_t *)mBackupMemoryHeap->getBase() + offset ;
                for(i=0; i<mDecodedHeight; i++){
                    memcpy(pDst, pY, mDecodedWidth*2);
                    pY += mDecodedWidth*2;
                    pDst += ALIGN16(mDecodedWidth)*2;
                }
            }
        }else{

			//For start address unaligned or virtual memory buffer, do memcpy here
            memcpy((uint8_t *)mBackupMemoryHeap->getBase(), (uint8_t *)data, size);
        }

		//Move to next buffer index
	    mIndex 		= 1 - mIndex;

		mMemoryHeap = mBackupMemoryHeap;
	
    }else{

        offset 				= (uint8_t*)data - (uint8_t*)memoryHeap->getBase();
        mMemoryHeap = memoryHeap;	

    }

    if (!memoryHeap_registered){

        if ((mMemoryHeap.get() != NULL))
 		   	mISurface->unregisterBuffers();

   		ISurface::BufferHeap bufferHeap(
   			mDisplayWidth, mDisplayHeight,
      		mDecodedWidth, mDecodedHeight,
      		GCUOutputColorFormat,
      		0, 0,
      		mMemoryHeap);

     	status_t err = mISurface->registerBuffers(bufferHeap);
     	if (err != OK) {
       		LOGW("ISurface failed to register buffers (0x%08x)", err);
   		}

	   	memoryHeap_registered = true;
	}

	mISurface->postBuffer(offset);

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
}


}  // namespace android

