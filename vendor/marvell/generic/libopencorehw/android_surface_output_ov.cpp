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
#define LOG_TAG "VideoMioOV"
#include <utils/Log.h>
#include <cutils/properties.h>

#include "android_surface_output_ov.h"
#include <media/PVPlayer.h>
#include "phycontmem.h"
#include "OMX_IppDef.h"
#define MAX_VIDMIO_BUFFER 16
#define MIN_VIDMIO_BUFFER 3

using namespace android;

#ifdef PERF
#include <cutils/properties.h>
#define GET_TIME_INTERVAL_USEC(A, B) (((((B).tv_sec)-((A).tv_sec))*1000000) + ((((B).tv_nsec)-((A).tv_nsec))/1000))
#define GET_TIME_INTERVAL_NSEC(A, B) (((((B).tv_sec)-((A).tv_sec))*1000000000LL) + ((((B).tv_nsec)-((A).tv_nsec))))
#endif


namespace android {
class Test
{
public:
#if PLATFORM_SDK_VERSION >= 5
    static sp<ISurface> getSurface(const SurfaceControl* surface)
#else
    static sp<ISurface> getSurface(const Surface* surface)
#endif
    {
        return surface->getISurface();
    };
};
};


#ifdef FAST_OVERLAY_CALLBACK
void cb_free(void *user)
{
    AndroidSurfaceOutputOverlay *output = (AndroidSurfaceOutputOverlay*)user;
    if( output != NULL ) {
        output->processDisplayDone();
    } else {
        LOGE("cb_free output === NULL\n");
    }
}
#endif

OSCL_IMPORT_REF AndroidSurfaceOutputOverlay::AndroidSurfaceOutputOverlay():AndroidSurfaceOutput()
{
#if PLATFORM_SDK_VERSION >= 5
    mNumberOfFramesToHold = 1;
    char value[PROPERTY_VALUE_MAX];
    property_get("disable.pvplayer.framelock", value, "0");
    if (atoi(value)){
	mNumberOfFramesToHold = 0;
    }
    pExtAlloc = NULL;
#endif
}

OSCL_IMPORT_REF AndroidSurfaceOutputOverlay::~AndroidSurfaceOutputOverlay()
{
#ifdef PERF
    LOGD("time_tot=%ldus, frame#=%d, avg=%lfus fps=%lf", time_tot_base, frame_num_base-1, (double)(time_tot_base)/(frame_num_base-1), (double)(frame_num_base-1)*1000000/(time_tot_base));
#endif

}

#ifdef FAST_OVERLAY_CALLBACK
void AndroidSurfaceOutputOverlay::processDisplayDone() {
    if( (iWriteResponseQueue.size() > 0) && (mNumberOfFramesToHold > 0) ) {
	mNumberOfFramesToHold--;
    } else {
        mNumberOfFramesToHold = 1;
    }
    return;
}
#endif

status_t AndroidSurfaceOutputOverlay::setVideoSurface(const sp<ISurface>& surface)
{
    LOGD("setVideoSurface(%p)", surface.get());
    // unregister buffers for the old surface
    if (mSurface != NULL) {
        LOGV("unregisterBuffers from old surface");
        mSurface->unregisterBuffers();
        // clean up old surface
        if( mOverlay.get() ) {
            mOverlay.clear();
            LOGV("clear overlay");
        }
        if( mOverlayRef.get() ) {
            mOverlayRef.clear();
            LOGV("clear overlayRef");
        }
        if( mBgSurface.get() ) {
            mBgSurface.clear();
        }       
    }
    mSurface = surface;
    LOGD("mInitialized = %d", mInitialized);
    if( mSurface != NULL && mInitialized) {
    if( mOverlayRef.get() == 0 ) {
        if (iVideoSubFormat == PVMF_MIME_YUV420_PLANAR)
        {
            mOverlayRef = mSurface->createOverlay(iVideoWidth, iVideoHeight, OVERLAY_FORMAT_YCbCr_420_P, 0);
        }
#if PLATFORM_SDK_VERSION >= 5
        else if (iVideoSubFormat == PVMF_MIME_YUV422_INTERLEAVED_UYVY)
        {
            mOverlayRef = mSurface->createOverlay(iVideoWidth/2, iVideoHeight, OVERLAY_FORMAT_YCbYCr_422_I, 0);
        }
#endif
        else
        {
            LOGD("VideoSubFormat %s is not supported", iVideoSubFormat.getMIMEStrPtr());
            bUseMIOBuffer = false;
            //return false;
        }
        LOGD("Create mOverlayRef");
        if( mOverlay.get() == NULL ) {
	    LOGD("Create mOverlay----------");
            mOverlay = new Overlay(mOverlayRef);
        } else {
            LOGD("mOverlay is exist-----------");
        }
    } else {
        LOGD("overlayRef already exist");
    }
    if (!pExtAlloc && bUseMIOBuffer)
    {
	pExtAlloc = new MrvlBufferAlloc(iRequestedMIOBufSize, iNumRequestedMIOBuffer, 1);
    }
	
    }       
    return NO_ERROR;
}

// create a frame buffer for software codecs
OSCL_IMPORT_REF bool AndroidSurfaceOutputOverlay::initCheck()
{
    // initialize only when we have all the required parameters
    if (((iVideoParameterFlags & VIDEO_SUBFORMAT_VALID) == 0) || !checkVideoParameterFlags())
        return mInitialized;

    // release resources if previously initialized
    if( mBgSurface.get() == NULL ) {
        // if mBgSurface is not NULL, it's background surface, don't reinitialze the surface
        closeFrameBuf();
    }
    pExtAlloc = NULL;

    //create an background surface, we may need some control to enable/disable it
    if( mSurface.get() == NULL ) {
        status_t state;
        int pid = getpid();

        sp<SurfaceComposerClient> videoClient = new SurfaceComposerClient;
        if (videoClient.get() == NULL)
        { 
            return OK;
        }

        mBgSurface = videoClient->createSurface(
            pid, 
            0, 
            2, //background surface, the width and height make no sense 
            2, 
            PIXEL_FORMAT_RGB_565, 
            ISurfaceComposer::eFXSurfaceNormal|ISurfaceComposer::ePushBuffers);
 
        if (mBgSurface.get() == NULL)
        {
            return OK;
        }

        videoClient->openTransaction();

       
        /* get ISurface interface */
        mSurface = Test::getSurface(mBgSurface.get());
        //state = mBgSurface->show();
        //state = mBgSurface->setLayer(500);
        if( mBgSurface.get() ) {
            mBgSurface->setLayer(500);
            mBgSurface->show();
        }
       
        if (state != NO_ERROR)
        {
            mBgSurface.clear();
            return OK;
        }

        videoClient->closeTransaction();
    }
    
    // reset flags in case display format changes in the middle of a stream
    resetVideoParameterFlags();
    if( mOverlayRef.get() == 0 ) {
        if (iVideoSubFormat == PVMF_MIME_YUV420_PLANAR)
        {
            mOverlayRef = mSurface->createOverlay(iVideoWidth, iVideoHeight, OVERLAY_FORMAT_YCbCr_420_P, 0);
        }
#if PLATFORM_SDK_VERSION >= 5
        else if (iVideoSubFormat == PVMF_MIME_YUV422_INTERLEAVED_UYVY)
        {
            mOverlayRef = mSurface->createOverlay(iVideoWidth/2, iVideoHeight, OVERLAY_FORMAT_YCbYCr_422_I, 0);
        }
#endif
#if 0
#ifdef IPPOMX
        else if (iVideoSubFormat == PVMF_IPPOMX_CUSTOMIZED)
        {
            mOverlayRef = mSurface->createOverlay(iVideoWidth, iVideoHeight, OVERLAY_FORMAT_YCbCr_420_P);
        }
#endif
#endif
        else
        {
            LOGD("VideoSubFormat %s is not supported", iVideoSubFormat.getMIMEStrPtr());
            bUseMIOBuffer = false;
            return false;
        }
        LOGD("Create mOverlayRef");
        if( mOverlay.get() == NULL ) {
            mOverlay = new Overlay(mOverlayRef);
        }

    } else {
        LOGD("overlayRef already exist");
    }
    if (!pExtAlloc && bUseMIOBuffer)
    {
#if 0
#ifdef FAST_OVERLAY
	pExtAlloc = new MrvlBufferAlloc(iRequestedMIOBufSize, iNumRequestedMIOBuffer, (!(iVideoSubFormat == PVMF_IPPOMX_CUSTOMIZED)));
#else
	pExtAlloc = new MrvlBufferAlloc(iRequestedMIOBufSize, iNumRequestedMIOBuffer, 0);
#endif
#else
	pExtAlloc = new MrvlBufferAlloc(iRequestedMIOBufSize, iNumRequestedMIOBuffer, 1);
#endif
    }
	

    mInitialized = true;
    mPvPlayer->sendEvent(MEDIA_SET_VIDEO_SIZE, iVideoDisplayWidth, iVideoDisplayHeight);
 
    return mInitialized;
}

PVMFStatus AndroidSurfaceOutputOverlay::getParametersSync(PvmiMIOSession aSession, PvmiKeyType aIdentifier,
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

void AndroidSurfaceOutputOverlay::setParametersSync(PvmiMIOSession aSession, PvmiKvp* aParameters,
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
            if (ParseYUVInfo(aParameters[i]))
            {
                PVLOGGER_LOGMSG(PVLOGMSG_INST_LLDBG, iLogger, PVLOGMSG_STACK_TRACE,
                    (0,"AndroidSurfaceOutput::setParametersSync() Error, unrecognized key = %s", aParameters[i].key));
                aRet_kvp = &aParameters[i];
                return;
            }
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

PVMFStatus AndroidSurfaceOutputOverlay::writeFrameBuf(uint8* aData, uint32 aDataLen, const PvmiMediaXferHeader& data_header_info)
{

    if( mSurface.get() == NULL ) {
        return PVMFSuccess;
    }
    if( mOverlay.get() == NULL ) {
        mOverlay = new Overlay(mOverlayRef);
    }
    overlay_buffer_t buffer = NULL;
    struct overlay_buffer_header_t* ptr = NULL;
    mOverlay->dequeueBuffer(&buffer);
    if ( buffer == NULL )
    {
	LOGE("AndroidSurfaceOutputOverlay::writeFrameBuf dequeueBuffer from overlay hal failed with NULL ptr");
	return PVMFFailure;
    }

     ptr = (struct overlay_buffer_header_t*)buffer;
     if(strcmp(iVideoFormat.getMIMEStrPtr(), "X-YUV-422-INTERLEAVED-UYVY") == 0)
     {
     	  if((0 != iVideoWidth) && (0 == (iVideoWidth % 16)))
       	ptr->stride_x = iVideoWidth >> 1;
         else
       	ptr->stride_x = (iVideoDisplayWidth + 15) & (~15);
         if((0 != iVideoHeight) && (0 == (iVideoHeight % 4)))
       	ptr->stride_y = iVideoHeight;
         else
       	ptr->stride_y = (iVideoDisplayHeight + 3) & (~3);        	
       }else{
        	ptr->stride_x = iVideoDisplayWidth;
        	ptr->stride_y = iVideoDisplayHeight;
        }  
      	ptr->x_off = 0;
       ptr->y_off = 0;
       //LOGD("stride_x=%d, stride_y=%d, x_position=%d, y_position=%d \n", ptr->stride_x, ptr->stride_y, ptr->x_off, ptr->y_off);
#ifndef FAST_OVERLAY // ask overlay-hal for render buffer to copy decoder output data
    void* address = mOverlay->getBufferAddress(buffer);
    if ( address == NULL )
    {
	LOGE("AndroidSurfaceOutputOverlay::writeFrameBuf getBufferAddress failed with NULL ptr");
	return PVMFFailure;
    }
#if 0
    if (iVideoSubFormat == PVMF_IPPOMX_CUSTOMIZED) {
        convertIPPFrame(aData, address);
    } else {
#endif
        memcpy( address, aData, aDataLen );
#if 0
    }
#endif

#else // fast overlay path, i.e. push codec output buffer into overlay-hal
    void *pVidOut = aData;
    uint32 iVidLen = aDataLen;
#if 0 
    if (iVideoSubFormat == PVMF_IPPOMX_CUSTOMIZED) {
	pVidOut = (void*)(((IppPicture*)(aData))->ppPicPlane[0]);
	iVidLen = (((IppPicture*)(aData))->picHeight)*(((IppPicture*)(aData))->picWidth)*3>>1;
    }
#endif

    if (phy_cont_getpa(pVidOut))
    {
	//ptr = (struct overlay_buffer_header_t*)buffer;
	ptr->paddr = (void *)phy_cont_getpa((void*)pVidOut);
	ptr->len   = iVidLen;
	ptr->vaddr = pVidOut;
	ptr->user  = this;
	ptr->reserved = NULL;
        ptr->flag = 2;
#ifdef FAST_OVERLAY_CALLBACK
        ptr->free = cb_free;
        mNumberOfFramesToHold++;
#else
        ptr->free = NULL;
#endif
    }else if (data_header_info.private_data_ptr){
        
        OMX_BUFFERHEADERTYPE_IPPEXT *header = (OMX_BUFFERHEADERTYPE_IPPEXT*)data_header_info.private_data_ptr;
        if (header->nPhyAddr){
            ptr->paddr = (void *)header->nPhyAddr;
            ptr->len   = iVidLen;
            ptr->vaddr = pVidOut;
            ptr->user  = this;
            ptr->reserved = NULL;
            ptr->flag = 2;
#ifdef FAST_OVERLAY_CALLBACK
            ptr->free = cb_free;
            mNumberOfFramesToHold++;
#else
            ptr->free = NULL;
#endif
        }
    }
    else
    {
	LOGE("AndroidSurfaceOutputOverlay::writeFrameBuf video buffer %p is not allocated by pmem", pVidOut);
	return PVMFFailure;
    }
#endif
    mOverlay->queueBuffer(buffer);

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

// post the last video frame to refresh screen after pause
void AndroidSurfaceOutputOverlay::postLastFrame()
{
}

void AndroidSurfaceOutputOverlay::closeFrameBuf()
{
    LOGV("CloseFrameBuf");
    if (!mInitialized) return;

    mInitialized = false;
    if (mSurface.get()) {
        LOGV("unregisterBuffers:0x%x", mSurface.get() );
        mSurface->unregisterBuffers();
    }

    if( mOverlay.get() ) {
        mOverlay.clear();
        LOGV("clear overlay");
    }
    if( mOverlayRef.get() ) {
        mOverlayRef.clear();
        LOGV("clear overlayRef");
    }
    if( mBgSurface.get() ) {
        mBgSurface.clear();
    }
}

#if 0
void AndroidSurfaceOutputOverlay::convertIPPFrame(void* src, void* dst)
{
    IppPicture* ptr = (IppPicture*)src;
    int32 srcStep[3];
    uint8* srcAddr[3];
    uint8* dstAddr = (uint8*)dst;
    int i;
    /* YUV plane step */
    srcStep[0] = ptr->picPlaneStep[0];
    srcStep[1] = ptr->picPlaneStep[1];
    srcStep[2] = ptr->picPlaneStep[2];
    /* YUV plane start address */
    srcAddr[0] = (uint8*)ptr->ppPicPlane[0] + srcStep[0]*ptr->picROI.y + ptr->picROI.x;
    srcAddr[1] = (uint8*)ptr->ppPicPlane[1] + srcStep[1]*(ptr->picROI.y>>1) + (ptr->picROI.x>>1);
    srcAddr[2] = (uint8*)ptr->ppPicPlane[2] + srcStep[2]*(ptr->picROI.y>>1) + (ptr->picROI.x>>1);

    //LOGD("step %d %d %d ROI w %d h %d", srcStep[0], srcStep[1], srcStep[2], ptr->picWidth, ptr->picHeight);

    for(i=0; i<(ptr->picHeight); i++){
    	memcpy(dstAddr, static_cast<void*>(srcAddr[0]), ptr->picWidth);
	srcAddr[0] += srcStep[0];
	dstAddr += ptr->picWidth;
    }

    for(i=0; i<(ptr->picHeight>>1); i++){
	memcpy(dstAddr, static_cast<void*>(srcAddr[1]), ptr->picWidth>>1);
	srcAddr[1] += srcStep[1];
	dstAddr += (ptr->picWidth>>1);
    }

    for(i=0; i<(ptr->picHeight>>1); i++){
	memcpy(dstAddr, static_cast<void*>(srcAddr[2]), ptr->picWidth>>1);
	srcAddr[2] += srcStep[2];
	dstAddr += (ptr->picWidth>>1);
    }
}
#endif

int AndroidSurfaceOutputOverlay::ParseYUVInfo(PvmiKvp& aParameters)
{
    if (pv_mime_strcmp(aParameters.key, PVMF_FORMAT_SPECIFIC_INFO_KEY_YUV) == 0)
    {
        PVMFYuvFormatSpecificInfo0* fsiInfo = OSCL_STATIC_CAST(PVMFYuvFormatSpecificInfo0*, aParameters.value.key_specific_value);
        LOGD("Compact parameters sync");
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
	//iVideoHeight = (iVideoHeight + 15) & (~15);
	LOGD("Video Height got rounded to be %d", iVideoHeight);

        iVideoDisplayWidth = fsiInfo->display_width;
        iVideoParameterFlags |= DISPLAY_WIDTH_VALID;
        LOGD("Display Width = %d", iVideoDisplayWidth);

        iVideoDisplayHeight = fsiInfo->display_height;
        iVideoParameterFlags |= DISPLAY_HEIGHT_VALID;
        LOGD("Display Height = %d", iVideoDisplayHeight);

        iNumRequestedMIOBuffer = fsiInfo->num_buffers;
        LOGD("Requested MIO Buffer Num = %d", iNumRequestedMIOBuffer);
	if (iNumRequestedMIOBuffer > MAX_VIDMIO_BUFFER)
	    iNumRequestedMIOBuffer = MAX_VIDMIO_BUFFER;
	else if (iNumRequestedMIOBuffer < MIN_VIDMIO_BUFFER)
	    iNumRequestedMIOBuffer = MIN_VIDMIO_BUFFER;
	LOGD("MIO would allocate %d buffers", iNumRequestedMIOBuffer);

        iRequestedMIOBufSize = fsiInfo->buffer_size;
        LOGD("Requested MIO Buffer Size = %d", iRequestedMIOBufSize);

        bUseMIOBuffer = true;

        LOGD("MIO finished parsing compact YUV info");
        return 0;
    }
    else
    {
        LOGD("unrecognized key %s", aParameters.key);
        return -1;
    }
}
