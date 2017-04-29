/*****************************************************************************************
 * Copyright (c) 2009, Marvell International Ltd.
 * All Rights Reserved.
 *****************************************************************************************/
//#define LOG_NDEBUG 0

#define LOG_TAG "DrmHardwareRendererOverlay"
#include <utils/Log.h>

#include "DrmHardwareRendererOverlay.h"

#include <media/stagefright/MediaDebug.h>
#include <surfaceflinger/ISurface.h>
#include "overlay.marvell.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include "OMX_IppDef.h"

#ifdef PERF
#include <cutils/properties.h>
#define GET_TIME_INTERVAL_USEC(A, B) (((((B).tv_sec)-((A).tv_sec))*1000000) + ((((B).tv_nsec)-((A).tv_nsec))/1000))
#define GET_TIME_INTERVAL_NSEC(A, B) (((((B).tv_sec)-((A).tv_sec))*1000000000LL) + ((((B).tv_nsec)-((A).tv_nsec))))
#endif

namespace android {

///////////////////////////////////////////////////////
// DrmHardwareRendererOverlay impl
///////////////////////////////////////////////////////

DrmHardwareRendererOverlay *DrmHardwareRendererOverlay::CreateInstance(
        const char *componentName,
        OMX_COLOR_FORMATTYPE colorFormat,
        const sp<ISurface> &surface,
        size_t displayWidth, size_t displayHeight,
        size_t decodedWidth, size_t decodedHeight ){
    DrmHardwareRendererOverlay * instance = new DrmHardwareRendererOverlay( componentName,
            colorFormat, surface,
            displayWidth, displayHeight,
            decodedWidth, decodedHeight );
    if( instance->initCheck() ){
        return instance;
    }else {
        delete instance;
        return NULL;
    }
}


DrmHardwareRendererOverlay::DrmHardwareRendererOverlay(
        const char *componentName,
        OMX_COLOR_FORMATTYPE colorFormat,
        const sp<ISurface> &surface,
        size_t displayWidth, size_t displayHeight,
        size_t decodedWidth, size_t decodedHeight)
    : mComponentName(componentName),
      mOMXColorFmt(colorFormat),
      mISurface(surface),
      mDisplayWidth(displayWidth),
      mDisplayHeight(displayHeight),
      mDecodedWidth(decodedWidth),
      mDecodedHeight(decodedHeight),
      mNumberOfFramesAlreadySent(0),
      virtDataInDisplay(NULL), physDataInDisplay(NULL){
    if( mDisplayWidth == 0 || mDisplayHeight == 0 ) {
        mDisplayWidth = decodedWidth;
        mDisplayHeight = decodedHeight;
    }

    CHECK(mISurface.get() != NULL);
    CHECK(mDecodedWidth > 0);
    CHECK(mDecodedHeight > 0);
}

int DrmHardwareRendererOverlay::_getOvlyFmtFromOMXFmt( OMX_COLOR_FORMATTYPE omxFmt )
{
    int ret = -1;
    switch( omxFmt ) {
        case OMX_COLOR_FormatYUV420Planar:
            ret = OVERLAY_FORMAT_YCbCr_420_P;
            break;
        case OMX_COLOR_FormatCbYCrY:
            ret = OVERLAY_FORMAT_YCbYCr_422_I;
            break;
        default:
            LOGD("%s#%d. unknown omxFmt=0x%x", __FUNCTION__, __LINE__, omxFmt);
            break;
    }
    return ret;
}

bool DrmHardwareRendererOverlay::createOverlay() {
    if (mOMXColorFmt == OMX_COLOR_FormatCbYCrY) {
        if((0 != mDecodedWidth) && (0 != (mDecodedWidth % 16))) {
               mDecodedWidth = ((mDisplayWidth + 15) & (~15));
        }
        if((0 != mDecodedHeight) && (0 != (mDecodedHeight % 4))) {
               mDecodedHeight = (mDisplayHeight + 3) & (~3);
        }
    }
    // create RGB565 overlay, since we always do resize through GCU
    mOverlayRef = mISurface->createOverlay( mDisplayWidth,
                                            mDisplayHeight,
                                            _getOvlyFmtFromOMXFmt( mOMXColorFmt )
                                            , 0 );
    if( mOverlayRef.get() == NULL ) {
        LOGE("%s() createOverlay failed for mISurface=%p", __FUNCTION__,
                    mISurface.get());
        return false;
    }
    if( mOverlay.get() == NULL ) {
        mOverlay = new Overlay( mOverlayRef );
    }

#ifdef PERF
    bStartPerf = false;
    char value[PROPERTY_VALUE_MAX];
    property_get("enable.drmplay.profiling", value, "0");
    if (atoi(value)){
        bStartPerf = true;
    }
    time_tot_base = 0;
    frame_num_base = 0;
#endif

    return true;
}

void DrmHardwareRendererOverlay::destroyOverlay() {
    if( mOverlay.get() ) {
        mOverlay->destroy();
        mOverlay.clear();
        LOGD("DrmHardwareRendererOverlay::~DrmHardwareRendererOverlay clear overlay");
    }
    if( mOverlayRef.get() ) {
        mOverlayRef.clear();
        LOGD("DrmHardwareRendererOverlay::~DrmHardwareRendererOverlay clear overlayRef");
    }
#ifdef PERF
    LOGD("time_tot=%ldus, frame#=%d, avg=%lfus fps=%lf", time_tot_base, frame_num_base-1, (double)(time_tot_base)/(frame_num_base-1), (double)(frame_num_base-1)*1000000/(time_tot_base));
#endif
}

bool DrmHardwareRendererOverlay::initCheck() {
    return createOverlay();
}

DrmHardwareRendererOverlay::~DrmHardwareRendererOverlay() {
    destroyOverlay();
}

void DrmHardwareRendererOverlay::displayDone( void * user ) {
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
int DrmHardwareRendererOverlay::hasMrvlExtension() {
    return 1;
}

void DrmHardwareRendererOverlay::render( const void *data,
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

    /* pass valid width/height/fmt to overlay-hal */
    ptr->width = mDisplayWidth;
    ptr->height = mDisplayHeight;
    ptr->fmt = _getOvlyFmtFromOMXFmt( mOMXColorFmt );

#ifndef FAST_OVERLAY // not fast overlay path
	void* address = mOverlay->getBufferAddress(buffer);
    if ( address == NULL ) {
        LOGE("DrmHardwareRendererOverlay::render getBufferAddress failed with NULL ptr");
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
            ptr->free = DrmHardwareRendererOverlay::displayDone;
    }
    else {
            LOGD("%s() warning: rendering in virt addr path", __FUNCTION__);
            void* address = mOverlay->getBufferAddress(buffer);
            if ( address == NULL ) {
                    LOGE("DrmHardwareRendererOverlay::render getBufferAddress failed with NULL ptr");
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

            displayDone( rbi );
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

void DrmHardwareRendererOverlay::getDisplayInfo(OMX_COLOR_FORMATTYPE &ColorFormat, size_t &DisplayWidth, size_t &DisplayHeight, size_t &DecodedWidth, size_t &DecodedHeight){
    ColorFormat = mOMXColorFmt;
    DisplayWidth = mDisplayWidth;
        DisplayHeight = mDisplayHeight;
    DecodedWidth = mDecodedWidth;
        DecodedHeight = mDecodedHeight;
}
void DrmHardwareRendererOverlay::Reconfig(OMX_COLOR_FORMATTYPE ColorFormat, size_t DisplayWidth, size_t DisplayHeight, size_t DecodedWidth, size_t DecodedHeight, bool recreateOvly){
    this->mOMXColorFmt = ColorFormat;
    this->mDisplayWidth = DisplayWidth;
    this->mDisplayHeight = DisplayHeight;
    this->mDecodedWidth = DecodedWidth;
    this->mDecodedHeight = DecodedHeight;
    LOGD("%s#%d: DisplayWidth=%d, DisplayHeight=%d, DecodedWidth=%d, DecodedHeight=%d, recreateOvly=%d", __FUNCTION__, __LINE__,
        DisplayWidth, DisplayHeight, DecodedWidth, DecodedHeight, recreateOvly );
    if( recreateOvly ) {
        destroyOverlay(  );
        createOverlay();
    }
}


}  // namespace android
