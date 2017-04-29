/***************************************************************************************** 
 * Copyright (c) 2009, Marvell International Ltd. 
 * All Rights Reserved.
 *****************************************************************************************/
//#define LOG_NDEBUG 0

#define LOG_TAG "HardwareRendererOverlay"
#include <utils/Log.h>

#include "HardwareRendererOverlay.h"

#include <binder/MemoryHeapBase.h>
#include <binder/MemoryHeapPmem.h>
#include <media/stagefright/MediaDebug.h>
#include <surfaceflinger/ISurface.h>
#include "bmm_lib.h"
#include "overlay.marvell.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "OMX_IppDef.h"

#ifdef PERF
#include <cutils/properties.h>
#define GET_TIME_INTERVAL_USEC(A, B) (((((B).tv_sec)-((A).tv_sec))*1000000) + ((((B).tv_nsec)-((A).tv_nsec))/1000))
#define GET_TIME_INTERVAL_NSEC(A, B) (((((B).tv_sec)-((A).tv_sec))*1000000000LL) + ((((B).tv_nsec)-((A).tv_nsec))))
#endif

namespace android {

HardwareRendererOverlay *HardwareRendererOverlay::CreateInstance(
        const char *componentName,
        OMX_COLOR_FORMATTYPE colorFormat,
        const sp<ISurface> &surface,
        size_t displayWidth, size_t displayHeight,
        size_t decodedWidth, size_t decodedHeight,
        int32_t rotationDegrees){
    HardwareRendererOverlay * instance = new HardwareRendererOverlay( componentName,
            colorFormat, surface,
            displayWidth, displayHeight,
            decodedWidth, decodedHeight, rotationDegrees );
    if( instance->initCheck() ){
        return instance;
    }else {
        delete instance;
        return NULL;
    }
}


HardwareRendererOverlay::HardwareRendererOverlay(
        const char *componentName,
        OMX_COLOR_FORMATTYPE colorFormat,
        const sp<ISurface> &surface,
        size_t displayWidth, size_t displayHeight,
        size_t decodedWidth, size_t decodedHeight,
        int32_t rotationDegrees)
    : mComponentName(componentName),
      mColorFormat(colorFormat),
      mISurface(surface),
      mDisplayWidth(displayWidth),
      mDisplayHeight(displayHeight),
      mDecodedWidth(decodedWidth),
      mDecodedHeight(decodedHeight),
      mRotationDegrees(rotationDegrees),
      mNumberOfFramesAlreadySent(0),
      virtDataInDisplay(NULL), physDataInDisplay(NULL){
    CHECK(mISurface.get() != NULL);
    CHECK(mDecodedWidth > 0);
    CHECK(mDecodedHeight > 0);
}

static uint32_t _getHalRotateDegree( int32_t rotDegree ) {
    uint32_t ret = 0;
    switch( rotDegree ) {
        case 90:
            ret = HAL_TRANSFORM_ROT_90;
            break;
        case 180:
            ret = HAL_TRANSFORM_FLIP_H | HAL_TRANSFORM_FLIP_V;
            break;
        case 270:
            ret = HAL_TRANSFORM_FLIP_H | HAL_TRANSFORM_FLIP_V | HAL_TRANSFORM_ROT_90;
            break;
        default:
            LOGD("%s()#%d - currently not support rotDegree=%d", __FUNCTION__, __LINE__, rotDegree);
            break;
    }
    return ret;
}

bool HardwareRendererOverlay::initCheck() {
    LOGV("%s(), mComponentName=%s", __FUNCTION__, mComponentName);

    if (mColorFormat == OMX_COLOR_FormatCbYCrY) {
	if((0 != mDecodedWidth) && (0 != (mDecodedWidth % 16))) {
               mDecodedWidth = ((mDisplayWidth + 15) & (~15));
	}
	if((0 != mDecodedHeight) && (0 != (mDecodedHeight % 4))) {
	       mDecodedHeight = (mDisplayHeight + 3) & (~3);
	}
    }

    uint32_t halRotate = _getHalRotateDegree( mRotationDegrees );
    if( mOverlayRef.get() == NULL ) {
        if (mColorFormat == OMX_COLOR_FormatYUV420Planar) {
            LOGD("%s() colorFormat is OMX_COLOR_FormatYUV420Planar", __FUNCTION__);
            mOverlayRef = mISurface->createOverlay(mDisplayWidth, mDisplayHeight, OVERLAY_FORMAT_YCbCr_420_P, halRotate);
        }
#if PLATFORM_SDK_VERSION >= 5
        else if (mColorFormat == OMX_COLOR_FormatCbYCrY) {
            LOGD("%s() colorFormat is OMX_COLOR_FormatCbYCrY", __FUNCTION__);
            mOverlayRef = mISurface->createOverlay(mDisplayWidth, mDisplayHeight, OVERLAY_FORMAT_YCbYCr_422_I, halRotate);
        }
#endif
        else {
            LOGE("%s() colorFormat %d is not supported", __FUNCTION__, mColorFormat);
            return false;
        }
        if( mOverlayRef.get() == NULL ) {
            LOGE("%s() createOverlay failed for mISurface=%p", __FUNCTION__, mISurface.get());
            return false;
        }
        LOGD("%s() Create mOverlayRef", __FUNCTION__);
        if( mOverlay.get() == NULL ) {
            mOverlay = new Overlay(mOverlayRef);
        }
    } else {
        LOGD("%s() overlayRef already exist", __FUNCTION__);
        return false;
    }
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

    return true;
}

HardwareRendererOverlay::~HardwareRendererOverlay() {
    if( mOverlay.get() ) {
        mOverlay->destroy();
        mOverlay.clear();
        LOGV("HardwareRendererOverlay::~HardwareRendererOverlay clear overlay");
    }
    if( mOverlayRef.get() ) {
        mOverlayRef.clear();
        LOGV("HardwareRendererOverlay::~HardwareRendererOverlay clear overlayRef");
    }
#ifdef PERF
    LOGD("time_tot=%lldus, frame#=%d, avg=%lfus fps=%lf", time_tot_base, frame_num_base-1, (double)(time_tot_base)/(frame_num_base-1), (double)(frame_num_base-1)*1000000/(time_tot_base));
#endif
}

void HardwareRendererOverlay::displayDone( void * user ) {
    LOGV("displayDone(%p)", user);
    RenderBufferInfo *renderBuf = (RenderBufferInfo*)user;
    if( !renderBuf )
        return;
    if( renderBuf->hook ) {
        renderBuf->hook( renderBuf->hook_priv );
    }
    if( renderBuf->renderer ) {
        renderBuf->renderer->mNumberOfFramesAlreadySent--;
    }
    delete renderBuf;
}

/**
 * return 1 if buffer header is extended by marvell
 */
int HardwareRendererOverlay::hasMrvlExtension() {
    return 1;
}

void HardwareRendererOverlay::render( const void *data,
                                      size_t size,
                                      void *platformPrivate,
										sp<MemoryHeapBase> memoryHeap) {

    OMX_BUFFERHEADERTYPE_IPPEXT *ippExt = NULL;
    struct timeval tv;
    overlay_buffer_t buffer = NULL;
    struct overlay_buffer_header_t* ptr = NULL;

    LOGV("%s() platformPrivate=%p", __FUNCTION__, platformPrivate );

    if( platformPrivate && hasMrvlExtension() ) {
        ippExt = (OMX_BUFFERHEADERTYPE_IPPEXT*)platformPrivate;
    }

#ifdef FAST_OVERLAY
    if( ippExt && physDataInDisplay &&
                    (void*)ippExt->nPhyAddr==physDataInDisplay) {
       LOGD("render(...) found input data == current buf in display.");
       return;
    }
#endif
    mOverlay->dequeueBuffer(&buffer);
    if ( buffer == NULL ) {
        LOGE("render() dequeueBuffer from overlay hal failed with NULL ptr");
        return;
    }

    ptr = (struct overlay_buffer_header_t*)buffer;
    ptr->stride_x = mDecodedWidth;
    ptr->stride_y = mDecodedHeight;

    ptr->x_off = 0;
    ptr->y_off = 0;

#ifndef FAST_OVERLAY // not fast overlay path 
	void* address = mOverlay->getBufferAddress(buffer);
    if ( address == NULL ) {
        LOGE("HardwareRendererOverlay::render getBufferAddress failed with NULL ptr");
        return;
    }
    memcpy( address, data, size );
#else // fast overlay path
    void *pVidOut = (void *)data;
    size_t iVidLen = size;
    if (ippExt && ippExt->nPhyAddr){
            LOGV("%s() ippExt=%p, ippExt->nPhyAddr=%p", __FUNCTION__, ippExt, (void*)ippExt->nPhyAddr);
            ptr = (struct overlay_buffer_header_t*)buffer;
            ptr->paddr = (void*)ippExt->nPhyAddr;
            ptr->len   = iVidLen;
            ptr->vaddr = pVidOut;
            ptr->reserved = NULL;
            ptr->flag = 2;
            ptr->deinterlace = 0;
            // set callback for completion
            RenderBufferInfo *rbi = new RenderBufferInfo;
            rbi->renderer = this;
            rbi->data = pVidOut;
            rbi->data_size = size;

            ptr->user = rbi;
            ptr->free = HardwareRendererOverlay::displayDone;
    }
    else {
            LOGV("%s() rendering in virt addr path", __FUNCTION__);
            void* address = mOverlay->getBufferAddress(buffer);
            if ( address == NULL ) {
                    LOGE("HardwareRendererOverlay::render getBufferAddress failed with NULL ptr");
                    return;
            }
            memcpy (address, data, size);
            ptr = (struct overlay_buffer_header_t*)buffer;
            ptr->len   = iVidLen;
            ptr->vaddr = address;
            ptr->reserved = NULL;
            ptr->flag = 1;//since data is virt addr
            ptr->deinterlace = 0;
            // set callback for completion
            RenderBufferInfo *rbi = new RenderBufferInfo;
            rbi->renderer = this;
            rbi->data = address;
            rbi->data_size = size;

            displayDone(rbi);

    }
#endif

    if (ippExt) {
        if (ippExt->nExtFlags & IPP_OMX_BUFFERFLAG_INTERLACEFRAME) {
            ptr->deinterlace = 1;
        }
    }

    gettimeofday(&tv, NULL);
    int startTime = tv.tv_usec;
    mNumberOfFramesAlreadySent++;
    mOverlay->queueBuffer(buffer);

    // wait for previous buffer done
    do {
        LOGV("%s(...)@%d: wait for previous buffers displayed. mNumberOfFramesAlreadySent=%d", __FUNCTION__, __LINE__, mNumberOfFramesAlreadySent);
        struct overlay_buffer_header_t toPoll;
        memset(&toPoll, 0, sizeof(struct overlay_buffer_header_t));
        toPoll.flag = 4;
        mOverlay->queueBuffer( &toPoll );
        if( mNumberOfFramesAlreadySent <= 1 )
            break;
        else
            usleep( 1000000/200 );
    }while( 1 );
    LOGV("%s(...)@%d: after wait for previous buffers displayed. mNumberOfFramesAlreadySent=%d", __FUNCTION__, __LINE__, mNumberOfFramesAlreadySent);
    this->virtDataInDisplay = (void*)data;
    if(ippExt) {
        this->physDataInDisplay = (void*)ippExt->nPhyAddr;
    }else {
        this->physDataInDisplay = (void*)NULL;
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
}

void HardwareRendererOverlay::getDisplayInfo(OMX_COLOR_FORMATTYPE &ColorFormat, size_t &DisplayWidth, size_t &DisplayHeight, size_t &DecodedWidth, size_t &DecodedHeight){
    ColorFormat = mColorFormat;
    DisplayWidth = mDisplayWidth;
        DisplayHeight = mDisplayHeight;
    DecodedWidth = mDecodedWidth;
        DecodedHeight = mDecodedHeight;
}
void HardwareRendererOverlay::Reconfig(OMX_COLOR_FORMATTYPE ColorFormat, size_t DisplayWidth, size_t DisplayHeight, size_t DecodedWidth, size_t DecodedHeight){

    if(mISurface.get() != NULL){
        mISurface->unregisterBuffers();
    }
    if( mOverlay.get() ) {
        mOverlay.clear();
        LOGE("HardwareRendererOverlay::Reconfig clear overlay");
    }
    if( mOverlayRef.get() ) {
        mOverlayRef.clear();
        LOGE("HardwareRendererOverlay::Reconfig clear overlayRef");
    }
    mColorFormat = ColorFormat;
    mDisplayWidth = DisplayWidth;
        mDisplayHeight = DisplayHeight;
    mDecodedWidth = DecodedWidth;
        mDecodedHeight = DecodedHeight;
        /*re-create the overlay*/
        LOGD("HardwareRendererOverlay::Reconfig the render w/ video resolution: %dx%d",mDecodedWidth, mDecodedHeight);
        if( mOverlayRef.get() == NULL ) {
        if (mColorFormat == OMX_COLOR_FormatYUV420Planar) {
            LOGD("HardwareRendererOverlay::Reconfig colorFormat is OMX_COLOR_FormatYUV420Planar");
            mOverlayRef = mISurface->createOverlay(mDecodedWidth, mDecodedHeight, OVERLAY_FORMAT_YCbCr_420_P, 0);
        }
#if PLATFORM_SDK_VERSION >= 5
        else if (mColorFormat == OMX_COLOR_FormatCbYCrY) {
            LOGD("HardwareRendererOverlay::Reconfig colorFormat is OMX_COLOR_FormatCbYCrY");
            mOverlayRef = mISurface->createOverlay(mDecodedWidth, mDecodedHeight, OVERLAY_FORMAT_YCbYCr_422_I, 0);   //TODO need to check the OMX_COLORFORMAT correct or not
        }
#endif
        else {
            LOGD("colorFormat %d is not supported", mColorFormat);
            return;
        }
        if (mOverlayRef.get() == NULL){
            LOGE("HardwareRendererOverlay::Reconfig Create mOverlayRef failed");
        }
        if( mOverlay.get() == NULL ) {
            mOverlay = new Overlay(mOverlayRef);
        }
    } else {
        LOGD("HardwareRendererOverlay::Reconfig overlayRef already exist");
    }
}
}  // namespace android
