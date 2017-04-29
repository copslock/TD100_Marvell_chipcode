/*
 * (C) Copyright 2010 Marvell International Ltd.
 * All Rights Reserved
 *
 * MARVELL CONFIDENTIAL
 * Copyright 2008 ~ 2010 Marvell International Ltd All Rights Reserved.
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Marvell International Ltd or its
 * suppliers or licensors. Title to the Material remains with Marvell International Ltd
 * or its suppliers and licensors. The Material contains trade secrets and
 * proprietary and confidential information of Marvell or its suppliers and
 * licensors. The Material is protected by worldwide copyright and trade secret
 * laws and treaty provisions. No part of the Material may be used, copied,
 * reproduced, modified, published, uploaded, posted, transmitted, distributed,
 * or disclosed in any way without Marvell's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Marvell in writing.
 *
 */
/* cameraengine state machine
 **************************************************************************

 standby
 |
 idle
 |
 video - preview - still

 **************************************************************************
 */

#include <linux/ioctl.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <utils/Timers.h>
#include <codecDef.h>
#include <codecJP.h>
#include <misc.h>
#include <hardware_legacy/power.h>
#include "ippIP.h"
#include "CameraSetting.h"

#include "CameraPreviewBaselaySW.h"
#include "CameraPreviewOverlay.h"
#include "CameraCtrl.h"
#include "Engine.h"
#include "FakeCam.h"
#include "cam_log_mrvl.h"
#include "CameraHardware.h"

#define LOG_TAG "CameraHardware"

#define LIKELY(exp)   __builtin_expect(!!(exp), 1)
#define UNLIKELY(exp) __builtin_expect(!!(exp), 0)

/*
 * property
 */
#define PROP_FAKE	"persist.service.camera.fake"	//0,1
#define PROP_CNT	"persist.service.camera.cnt"	//0,>0,<0
#define PROP_HW		"service.camera.hw"	//overlay,base
#define PROP_AF		"service.camera.af"	//0,>0
#define PROP_PMEM	"service.camera.pmem"	//NULL,/dev/pmem,/dev/pmem_adsp
#define PROP_DUMP	"service.camera.dump"	//0 for nodump, >0 specify num of frames to dump, for each preview/still cycle
#define PROP_DISPLAY    "service.camera.display"    //>0 display by default, 0 nodisplay for debug

/*
 * variables for performance optimization
 * show preview/camcorder fps
 */
#define OPTIMIZE
#define CAMERA_WAKELOCK "CameraHardware"
#define DUMP_IMAGE

namespace android {

#ifdef DUMP_IMAGE
    int preview_cycle=0;
    int dump_frame_buffer(const char* dump_name, int counter, const sp<ImageBuf>& imagebuf, int index){
        if(imagebuf == NULL || index < 0){
            TRACE_E("Invalid imagebuf or index.");
            return 0;
        }

        const char* fmt = "";
        fmt = imagebuf->getImageFormat(index);

        int w=0,h=0;
        imagebuf->getImageSize(index,&w,&h);

        unsigned char* bufptr[3];
        imagebuf->getVirAddr(index,bufptr+0,bufptr+1,bufptr+2);
        TRACE_D("vaddr: %p,%p,%p",bufptr[0],bufptr[1],bufptr[2]);

        int len[3];
        imagebuf->getFilledLen(index, len+0, len+1, len+2);
        TRACE_D("len: %d,%d,%d",len[0],len[1],len[2]);

        char fname[100];
        sprintf(fname,"/data/%s_%dx%d_%d.%s",dump_name, w, h, counter, fmt);
        TRACE_D("dump-preview-buf to:%s",fname);

        FILE *fp_dump = fopen(fname, "wb");
        //FILE *fp_dump = fopen(fname, "ab");
        if (fp_dump) {
            fwrite(bufptr[0], 1, len[0], fp_dump);
            fwrite(bufptr[1], 1, len[1], fp_dump);
            fwrite(bufptr[2], 1, len[2], fp_dump);
            fclose(fp_dump);
        }
        else{
            TRACE_E("Fail to open %s", fname);
        }
        return 0;
    }
#endif

#ifdef OPTIMIZE
    static long long video_count=0;
    static double preview_start=0;
    static double video_start=0;
#endif

    static int mEnableFake;

    CameraHardware::CameraHardware(int cameraID):
        mRawHeap(NULL),
        mMsgEnabled(0),
        mPreviewFrameCount(0),
        mRecordingEnabled(false),
        mPreviewDevice(NULL),
        mOverlay(NULL),
        mUseOverlay(false),
        kPreviewBufCnt(6),
        kStillBufCnt(2),
        kVideoBufCnt(6),
        mCamPtr(NULL),
        mPreviewImageBuf(NULL),
        mPreviewImageBuf_base(NULL),
        mPreviewHeap(NULL),
        mStillImageBuf(NULL),
        mState(CAM_STATE_IDLE),
        mDisplayEnabled(1),
        mMinDriverBufCnt(0),
        mDumpNum(0),
        iSkipFrame(0)
    {
        FUNC_TAG;
        if (acquire_wake_lock(PARTIAL_WAKE_LOCK, CAMERA_WAKELOCK) > 0) {
            TRACE_D("%s:acquire_wake_lock success",__FUNCTION__);
        }
        else{
            TRACE_E("acquire_wake_lock fail!");
        }

        /*
         * initialize the default parameters for the first time.
         */

        property_get(PROP_PMEM, PMEM_DEVICE, "/dev/pmem");
        TRACE_D("%s:"PROP_PMEM"=%s",__FUNCTION__,PMEM_DEVICE);

        /*
         * get property for overlay/base selection
         * setprop service.camera.hw x
         * x=base or overlay
         */
        char value[PROPERTY_VALUE_MAX];
        property_get(PROP_HW, value, "base");
        if (0 == strcmp(value, "overlay"))
            mUseOverlay = true;
        else
            mUseOverlay = false;
        TRACE_D("%s:"PROP_HW"=%s",__FUNCTION__,value);

        if(mEnableFake != 0){
            mCamPtr = new FakeCam(cameraID);
        }
        else{
            mCamPtr = new Engine(cameraID);
        }

        property_get(PROP_DISPLAY, value, "1");
        TRACE_D("%s:"PROP_DISPLAY"=%s",__FUNCTION__, value);
        mDisplayEnabled = atoi(value);

        property_get(PROP_DUMP, value, "0");
        mDumpNum = atoi(value);
        TRACE_D("%s:"PROP_DUMP"=%d",__FUNCTION__, mDumpNum);

        /*
         * get preview/still/video buf cnt from camerasetting,
         * all buf cnt should be more than 2
         * use the default one if not specified.
         */
        int previewbufcnt,stillbufcnt,videobufcnt;
        mCamPtr->getBufCnt(&previewbufcnt,&stillbufcnt,&videobufcnt);
        if(previewbufcnt >= 2)
            kPreviewBufCnt = previewbufcnt;
        if(stillbufcnt >= 2)
            kStillBufCnt = stillbufcnt;
        if(videobufcnt >= 2)
            kVideoBufCnt = videobufcnt;
    }

    /*
     * stop CE
     * flush all CE buffers
     * free CE file handler
     */
    CameraHardware::~CameraHardware()
    {
        FUNC_TAG_E;
        //destroy preview devices
        if (mPreviewDevice != NULL){
            mPreviewDevice->flush();
            mPreviewDevice->setOverlay(0);//force clear overlay ref
            mPreviewDevice.clear();
        }
        //destroy camera device
        if (mCamPtr != NULL){
            mCamPtr->stopCapture();
            mCamPtr->unregisterStillBuffers();
            mCamPtr->stopPreview();
            mCamPtr->unregisterPreviewBuffers();
            mCamPtr.clear();
        }
        //destroy still buffers.
        if (mStillImageBuf != NULL){
            mStillImageBuf.clear();
        }
        //destroy preview buffers.
        if (mPreviewImageBuf != NULL){
            mPreviewImageBuf.clear();
            mPreviewHeap.clear();
        }
        TRACE_D("%s:release_wake_lock",__FUNCTION__);
        release_wake_lock(CAMERA_WAKELOCK);

        FUNC_TAG_X;
        singleton.clear();
    }

    sp<IMemoryHeap> CameraHardware::getPreviewHeap() const
    {
        FUNC_TAG;
        return mPreviewHeap;
    }

    sp<IMemoryHeap> CameraHardware::getRawHeap() const
    {
        FUNC_TAG;
        return mPreviewHeap;
        //return mRawHeap;
    }

    void CameraHardware::setCallbacks(notify_callback notify_cb,
            data_callback data_cb,
            data_callback_timestamp data_cb_timestamp,
            void* user)
    {
        FUNC_TAG;
        Mutex::Autolock lock(mLock);
        mNotifyCb = notify_cb;
        mDataCb = data_cb;
        mDataCbTimestamp = data_cb_timestamp;
        mCallbackCookie = user;

        //TODO:sensor instance may only sendout notification msg, no datacallback should be
        //called, so here only set notify_callback;
        //set NULL if no eventnotification needed by sensor instance
        //mCamPtr->setCallbacks();
        mCamPtr->setCallbacks(notify_cb,user);
    }

    //debug only
#define CHECK_MSG(msg)	do{ \
    if(mMsgEnabled == CAMERA_MSG_ALL_MSGS) \
    {TRACE_V("\tEnabled-msg:CAMERA_MSG_ALL_MSGS");} \
    else if(mMsgEnabled & msg) \
    {TRACE_D("\tEnabled-msg:" #msg );} \
}while(0)
#if 0
#define CHECK_MSG_ALL() do{ \
    CHECK_MSG(CAMERA_MSG_ERROR); \
    CHECK_MSG(CAMERA_MSG_SHUTTER); \
    CHECK_MSG(CAMERA_MSG_FOCUS); \
    CHECK_MSG(CAMERA_MSG_ZOOM); \
    CHECK_MSG(CAMERA_MSG_PREVIEW_FRAME); \
    CHECK_MSG(CAMERA_MSG_VIDEO_FRAME); \
    CHECK_MSG(CAMERA_MSG_POSTVIEW_FRAME); \
    CHECK_MSG(CAMERA_MSG_RAW_IMAGE); \
    CHECK_MSG(CAMERA_MSG_COMPRESSED_IMAGE); \
}while(0)
#else
#define CHECK_MSG_ALL() do{}while(0)
#endif
void CameraHardware::enableMsgType(int32_t msgType)
{
    TRACE_V("-%s:0x%x",__FUNCTION__,msgType);
    Mutex::Autolock lock(mLock);
    mMsgEnabled |= msgType;
    //TODO:if notification callback is not enabled in sensor instance,
    //then msgtype is not necessary
    mCamPtr->enableMsgType(msgType);
    CHECK_MSG_ALL();
}

void CameraHardware::disableMsgType(int32_t msgType)
{
    TRACE_V("-%s:0x%x",__FUNCTION__,msgType);
    Mutex::Autolock lock(mLock);
    mMsgEnabled &= ~msgType;
    mCamPtr->disableMsgType(msgType);
    CHECK_MSG_ALL();
}

bool CameraHardware::msgTypeEnabled(int32_t msgType)
{
    TRACE_V("-%s:0x%x",__FUNCTION__,msgType);
    Mutex::Autolock lock(mLock);
    return (mMsgEnabled & msgType);
}
#undef CHECK_MSG

int CameraHardware::previewThread()
{
    TRACE_V("-%s e", __FUNCTION__);
    /*
     * show preview/camcorder buf status, for debug purpose only
     */
    mBuffersLock.lock();
    //mPreviewImageBuf->showStatus();

    /*
     * loop all preview buf status
     * 1.increase ref count
     * 2.if free, enqueue to driver
     */
    //loop all buf and enqueue free buf to engine
    for (int i = 0; i < kPreviewBufCnt; i++) {
        if(mPreviewImageBuf->isFree(i)){
            status_t stat=mCamPtr->enqPreviewBuffer(i);
            if(NO_ERROR == stat)
                mPreviewImageBuf->toDriver(i);
            else
                TRACE_E("enqPreviewBuffer fail!");
        }
    }
    /*
     * sum over all buf in driver.
     * if 0, flush overlay buf, wait and loop back the previewthread.
     * or else, dequeue from CE
     */
    int preview,still,video,driver;
    int driver_bufcnt=0;
    for (int i = 0; i < kPreviewBufCnt; i++) {
        mPreviewImageBuf->getRef(i,&preview,&still,&video,&driver);
        driver_bufcnt += driver;
    }
    mBuffersLock.unlock();//unlock before return

    if(driver_bufcnt < mMinDriverBufCnt){//we must leave some buf in v4l2 for user-pointer
        TRACE_D("%s:driver required bufcnt:%d:current bufcnt:%d; waiting for buf...",
                __FUNCTION__,driver_bufcnt,mMinDriverBufCnt);
        mPreviewImageBuf->showStatus();
        /*
         * TODO:
         * flush preview buf, push overlay to call 'free' callback
         * as to encoder buf, we can do nothing but waiting for the callback
         */
        /*
         * NOTES: mBuffersLock should be locked before call waitrelative
         */
        mBuffersLock.lock();
        mBuffersCond.waitRelative(mBuffersLock, 20000000);//20ms timeout
        mBuffersLock.unlock();//unlock before return
        return NO_ERROR;
    }

    /*
     * at least one buf could be dequeue from engine
     */
    int index = -1;
    status_t error = NO_ERROR;

    error = mCamPtr->deqPreviewBuffer(&index);
    if(index<0){
        TRACE_E("dequeue preview buffer index invalid: %d",index);
        return NO_ERROR;
    }

    if (LIKELY(NO_ERROR == error)) {
        mBuffersLock.lock();
        mPreviewImageBuf->fromDriver(index);
        mBuffersLock.unlock();
#if 0
        if(iSkipFrame++ <= kPreviewBufCnt){
            TRACE_D("Skip frame: %d, index=%d",iSkipFrame,index);
            return NO_ERROR;
        }
#endif

        mPreviewFrameCount++;
        sp<IMemory> buffer = mPreviewImageBuf->getIMemory(index);
        //flush bufferable/cachable buffers
        mPreviewImageBuf->flush(index);

#ifdef DUMP_IMAGE
        {
            if (UNLIKELY(mDumpNum) > 0) {
                TRACE_D("%s:"PROP_DUMP"=%d",__FUNCTION__, mDumpNum);
                if(mPreviewFrameCount <= mDumpNum){
                    dump_frame_buffer("preview",mPreviewFrameCount,mPreviewImageBuf,index);
                }
            }
        }
#endif
        /*
         * preview callback to surfaceflinger or overlay
         */
        if (LIKELY((mOverlay.get() != NULL || mUseOverlay == false) && mDisplayEnabled)){
            if (LIKELY(mUseOverlay || mMsgEnabled & CAMERA_MSG_PREVIEW_FRAME)) {
                /*
                 * If we don't use overlay, we always need the preview frame for display.
                 * If we do use overlay, we only need the preview frame if the user
                 * wants the data.
                 */
                status_t ret=NO_ERROR;
                //increase the ref cnt beforehand
                mBuffersLock.lock();
                mPreviewImageBuf->toPreview(index);
                mBuffersLock.unlock();
                //for overlay & baselay display
                ret=mPreviewDevice->display(mPreviewImageBuf, index);

                //decrease the ref cnt if fail
                if(NO_ERROR != ret){
                    mBuffersLock.lock();
                    mPreviewImageBuf->fromPreview(index);
                    mBuffersLock.unlock();
                }
            }
            else {
                TRACE_D("CAMERA_MSG_PREVIEW_FRAME is disabled in preview mode");
            }
        }
        //for user required overlay data callback only
        if(mUseOverlay && (mMsgEnabled & CAMERA_MSG_PREVIEW_FRAME) && mDisplayEnabled){
            mDataCb(CAMERA_MSG_PREVIEW_FRAME, buffer,
                    mCallbackCookie);
        }

        /*
         * recorder callback to opencore
         */
        if (mRecordingEnabled){
            if (mMsgEnabled & CAMERA_MSG_VIDEO_FRAME) {
#ifdef OPTIMIZE
                video_count++;
#endif
                nsecs_t timestamp = systemTime();
                mBuffersLock.lock();
                mPreviewImageBuf->toVideo(index);
                mBuffersLock.unlock();
                mDataCbTimestamp(timestamp, CAMERA_MSG_VIDEO_FRAME,
                        buffer, mCallbackCookie);
            }
            else {
                TRACE_E("CAMERA_MSG_VIDEO_FRAME is disabled in recording mode");
            }
        }
    }
    else{
        TRACE_E("Fail to dequeue buf from CE... sth. must be wrong...");
    }
    usleep(1);

    TRACE_V("-%s x", __FUNCTION__);
    return NO_ERROR;
}

status_t CameraHardware::RegisterPreviewBuffers()
{
    mPreviewFrameCount = 0;

    CameraParameters params = mCamPtr->getParameters();
    int w,h;
    params.getPreviewSize(&w, &h);
    const char* format = params.getPreviewFormat();

    int previous_w=-1,previous_h=-1;
    const char* previous_format="";
    mBuffersLock.lock();
    if(mPreviewImageBuf != NULL)
    {
        previous_format = mPreviewImageBuf->getImageFormat(0);
        mPreviewImageBuf->getImageSize(0,&previous_w,&previous_h);
    }

    if(CAM_STATE_STILL == mState &&
            previous_w == w &&
            previous_h == h &&
            0 == strcmp(format, previous_format)){//still->preview, and preview param not changed.
        mPreviewImageBuf->showStatus();
        TRACE_D("%s:preview size/format not changed, switch to preview directly, using previous buffers!",__FUNCTION__);
    }
    else{//idle->preview, or preview param changed.
        TRACE_D("%s:preview size/format changed, switch to idle then preview, re-alloc buffers!",__FUNCTION__);

        mCamPtr->stopPreview();
        mCamPtr->unregisterPreviewBuffers();

        mPreviewImageBuf.clear();
        mPreviewHeap.clear();

        const ImageBuf::EngBufReq bufreq = mCamPtr->getPreviewBufReq();
        mPreviewImageBuf = new ImageBuf(
                bufreq,
                kPreviewBufCnt,
                1,
                PMEM_DEVICE
                );

        mMinDriverBufCnt = bufreq.getMinBufCount();
        TRACE_D("%s:driver required min buffer cnt: %d",__FUNCTION__,mMinDriverBufCnt);

        status_t stat=mCamPtr->registerPreviewBuffers(mPreviewImageBuf);
        if(NO_ERROR != stat)
        {
            mBuffersLock.unlock();
            TRACE_E("registerPreviewBuffers fail!");
            return UNKNOWN_ERROR;
        }
    }
    mBuffersLock.unlock();

    //update mPreviewHeap
    if (mUseOverlay){
        mPreviewHeap = mPreviewImageBuf->getDupBufHeap();//use dup bufheap for IPC.
    }
    else{
        TRACE_D("%s:prepare baselayer buffer",__FUNCTION__);
        /*
         * If the format is supported directly by surface,
         * we post imagebuffer without any processing.
         *
         * 1>surface supported:UYVY,YUV420P,RGB565
         * 2>surface not supported temporarily: YUV420SP, do color conversion.
         * 3>DEBUG: memcpy to special baseheap buffer, yuv->rgb color conversion before callback.
         * For case 2&3, just introduce another buffer.
         */
        mPreviewImageBuf_base.clear();
        mPreviewHeap.clear();
#ifdef DEBUG_BASE_RGB565
        mPreviewImageBuf_base = new ImageBuf(
                CameraParameters::PIXEL_FORMAT_RGB565,
                Size(w,h),
                1,
                0
                );//use ashmem for baselayer preview
        mPreviewHeap = mPreviewImageBuf_base->getBufHeap();
#else
        if(0 == strcmp(format, CameraParameters::PIXEL_FORMAT_YUV420P) ||
                0 == strcmp(format, CameraParameters::PIXEL_FORMAT_YUV420SP) ||
                0 == strcmp(format, CameraParameters::PIXEL_FORMAT_YUV422I_UYVY)){
            //put surface supported formats here
            mPreviewImageBuf_base = NULL;
            mPreviewHeap = mPreviewImageBuf->getDupBufHeap();
        }
        /*
           else if (0 == strcmp(format, CameraParameters::PIXEL_FORMAT_YUV420SP)){
           mPreviewImageBuf_base = new ImageBuf(
           CameraParameters::PIXEL_FORMAT_YUV420P,
           Size(w,h),
           1,
           0
           );
           mPreviewHeap = mPreviewImageBuf_base->getBufHeap();
           }
           */
        else{//unsupported by surface
            mPreviewImageBuf_base = new ImageBuf(
                    CameraParameters::PIXEL_FORMAT_RGB565,//here we use rgb565 bydefaut for unsupported format
                    Size(w,h),
                    1,
                    0
                    );
            mPreviewHeap = mPreviewImageBuf_base->getBufHeap();
        }
#endif

    }
    return NO_ERROR;
}


/*
 * stop CE to idle,
 * alloc buf & enqueue all CE buf
 * create preview thread
 */
status_t CameraHardware::startPreview()
{
    FUNC_TAG_E;

    mLock.lock();

    if (mPreviewThread != 0) {
        TRACE_E("previous mPreviewThread not clear");
        mLock.unlock();
        return INVALID_OPERATION;
    }
    mLock.unlock();

    disableMsgType(CAMERA_MSG_SHUTTER|CAMERA_MSG_COMPRESSED_IMAGE|CAMERA_MSG_RAW_IMAGE);
    cancelPicture();

    RegisterPreviewBuffers();

    mBuffersLock.lock();
    for (int i = 0; i < kPreviewBufCnt; i++) {
        if(mPreviewImageBuf->isFree(i)){
            status_t stat=mCamPtr->enqPreviewBuffer(i);
            if(NO_ERROR == stat)
                mPreviewImageBuf->toDriver(i);
            else
                TRACE_E("enqPreviewBuffer fail!");
        }
    }
    mBuffersLock.unlock();
    if (NO_ERROR != mCamPtr->startPreview()){
        TRACE_E("Fail to startPreview.");
        return -1;
    }

    //reset preview device & recycle buf
    if(mPreviewDevice != NULL){
        // flush & recycle all buffers in display
        mPreviewDevice->flush();
    }
    else{
        if (mUseOverlay)
            mPreviewDevice = new CameraPreviewOverlay(this);
        else
            mPreviewDevice = new CameraPreviewBaselaySW(this);
    }
    mPreviewDevice->setOverlay(mOverlay);
    mPreviewDevice->setPreviewCallback(mDataCb, mCallbackCookie, mPreviewImageBuf);

#ifdef OPTIMIZE
    preview_start = systemTime();
#endif

#ifdef DUMP_IMAGE
    preview_cycle++;
#endif

    mState = CAM_STATE_PREVIEW;
    mPreviewImageBuf->showStatus();
    iSkipFrame = 0;
    mPreviewThread = new PreviewThread(this);
    FUNC_TAG_X;
    return NO_ERROR;
}

bool CameraHardware::useOverlay()
{
    FUNC_TAG;
    if(mUseOverlay){
        TRACE_D("use overlay for preview");
        return true;
    }
    TRACE_D("DONT use overlay for preview");
    return false;
}

status_t CameraHardware::setOverlay(const sp<Overlay> &overlay)
{
    FUNC_TAG;
    TRACE_D("%s: overlay=%p",__FUNCTION__,(overlay==NULL)? 0 : overlay.get());
    mOverlay = overlay;
    if (mPreviewDevice != NULL){
        mPreviewDevice->setOverlay(overlay);
    }
    return NO_ERROR;
}

/*
 * this function maybe called multi-times by cameraservice.
 * this function maybe called without calling startpreview beforehand.
 */
void CameraHardware::stopPreview()
{
    FUNC_TAG_E;
#if 1
    {//check previewthread, we simply assume stoppreview would be called at any moment
        sp<PreviewThread> previewThread;
        {
            Mutex::Autolock lock(mLock);
            previewThread = mPreviewThread;
        }
        if (previewThread == NULL) {
            TRACE_D("%s:preview thread already exit",__FUNCTION__);
            mState = CAM_STATE_IDLE;
            return;
        }
    }
#endif
    __stopPreviewThread();

    mCamPtr->stopPreview();
    mCamPtr->unregisterPreviewBuffers();
    mBuffersLock.lock();
    if(mPreviewImageBuf != NULL){
        mPreviewImageBuf->showStatus();
        for (int i = 0; i < kPreviewBufCnt; i++) {
            mPreviewImageBuf->setFree(i);
        }
        //mPreviewImageBuf.clear();//FIXME
        //mPreviewHeap.clear();
    }
    mBuffersLock.unlock();

    mState = CAM_STATE_IDLE;

    FUNC_TAG_X;
}

//donot stop sensor preview, only cancel the preview thread
void CameraHardware::__stopPreviewThread()
{
    FUNC_TAG_E;

#ifdef OPTIMIZE
    double preview_dur = (systemTime() - preview_start)/1000000000;
    if (preview_dur > 0 && mPreviewFrameCount > 10 && preview_start > 0){
        TRACE_D("preview_dur=%f,preview_count=%lld",preview_dur,mPreviewFrameCount);
        long long preview_fps = (long long)(mPreviewFrameCount/preview_dur);
        preview_start = 0;
        TRACE_D("preview fps=%lld", preview_fps);
    }
#endif
    mPreviewFrameCount = 0;

    sp<PreviewThread> previewThread;
    {//scope for the lock
        Mutex::Autolock lock(mLock);
        previewThread = mPreviewThread;
    }

    //don't hold the lock while waiting for the thread to quit
    if (previewThread != NULL) {
        TRACE_D("%s(pid=%d): request preview thread to exit",
                __FUNCTION__, gettid());
        previewThread->requestExitAndWait();
    }
    else{
        TRACE_D("%s:preview thread already exit",__FUNCTION__);
        return;
    }

    {
        Mutex::Autolock lock(mLock);
        mPreviewThread.clear();
    }
    FUNC_TAG_X;
}

bool CameraHardware::previewEnabled()
{
    return (mPreviewThread != 0);
}

status_t CameraHardware::startRecording()
{
    FUNC_TAG;
    mBuffersLock.lock();
    mPreviewImageBuf->showStatus();
    mBuffersLock.unlock();
    Mutex::Autolock lock(mLock);
    mRecordingEnabled = true;

#ifdef OPTIMIZE
    video_start = systemTime();
    video_count = 0;
#endif
    /*
     * notify preview device that recording is on
     */
    mPreviewDevice->startRecording();
    return NO_ERROR;
}

void CameraHardware::stopRecording()
{
    FUNC_TAG;
    Mutex::Autolock lock(mLock);
    mRecordingEnabled = false;

#ifdef OPTIMIZE
    double video_dur = (systemTime() - video_start)/1000000000;
    if( video_dur > 0 && video_count > 0 ){
        long long video_fps = (long long)(video_count/video_dur);
        video_count = 0;
        video_start = 0;
        TRACE_D("video fps = %lld", video_fps);
    }
#endif

    /*
     * notify preview device that recording is off
     */
    mPreviewDevice->stopRecording();
    mBuffersLock.lock();
    mPreviewImageBuf->showStatus();

    //TODO:recycle all recording buffers when stoprecording
    int preview,still,video,driver;
    int driver_bufcnt=0;
    for (int i = 0; i < kPreviewBufCnt; i++) {
        mPreviewImageBuf->getRef(i,&preview,&still,&video,&driver);
        if(0 < video){
            mPreviewImageBuf->fromVideo(i);
        }
    }
    mBuffersLock.unlock();
}

void CameraHardware::releasePreviewBuffer(int index)
{
    if (index>=0 && index<kPreviewBufCnt) {
        Mutex::Autolock buflock(mBuffersLock);
        TRACE_V("%s: buffer #%d returned", __FUNCTION__, index);
        mPreviewImageBuf->fromPreview(index);
        mBuffersCond.signal();
    }
    else {
        TRACE_E("invalid index %d", index);
    }
}

void CameraHardware::releaseRecordingFrame(const sp<IMemory>& mem)
{
    int index;

    index = mPreviewImageBuf->getBufIndex(mem);

    if (index >= 0 && index < kPreviewBufCnt) {
        Mutex::Autolock buflock(mBuffersLock);
        TRACE_V("%s: buffer #%d returned", __FUNCTION__, index);
        mPreviewImageBuf->fromVideo(index);
        mBuffersCond.signal();
    }
    else {
        TRACE_E("invalid index %d", index);
    }
}

int CameraHardware::beginAutoFocusThread(void *cookie)
{
    FUNC_TAG;
    CameraHardware *c = (CameraHardware *)cookie;
    return c->autoFocusThread();
}

status_t CameraHardware::autoFocusThread()
{
    int cesetting                           = -1;
    int iEnableAutoFocus                    = 0;
    char value[PROPERTY_VALUE_MAX]          = {0};

    property_get(PROP_AF, value, "0");
    TRACE_D("%s:"PROP_AF"=%s",__FUNCTION__, value);
    iEnableAutoFocus = atoi(value);
    if(iEnableAutoFocus == 0){
        mNotifyCb(CAMERA_MSG_FOCUS, true, 0, mCallbackCookie);
        return NO_ERROR;
    }
    status_t stat = mCamPtr->autoFocus();
    if(NO_ERROR != stat){
        TRACE_E("focus failed.");
        mNotifyCb(CAMERA_MSG_FOCUS, false, 0, mCallbackCookie);
    }
    return NO_ERROR;
}

status_t CameraHardware::autoFocus()
{
    FUNC_TAG;
    Mutex::Autolock lock(mLock);

    if ((mNotifyCb == NULL) || !(mMsgEnabled & CAMERA_MSG_FOCUS)){
        TRACE_E("mNotifyCb not set or CAMERA_MSG_FOCUS not enabled for autofocus");
        return INVALID_OPERATION;
    }
    //NOTES:donot trigger fake focus callback in MAIN thread, or else it will call deadlock.
    //but it should be safe if real focus is trigger.
    //return beginAutoFocusThread(this);
    if (createThreadEtc(beginAutoFocusThread, this,
                "focusthread", PRIORITY_URGENT_DISPLAY) == false){
        TRACE_E("fail to create autofocus thread");
        return UNKNOWN_ERROR;
    }
    return NO_ERROR;
}

status_t CameraHardware::cancelAutoFocus()
{
    FUNC_TAG;
    int iEnableAutoFocus                    = 0;
    char value[PROPERTY_VALUE_MAX]          = {0};

    property_get(PROP_AF, value, "0");
    TRACE_D("%s:"PROP_AF"=%s",__FUNCTION__, value);
    iEnableAutoFocus = atoi(value);
    if(iEnableAutoFocus == 0){
        return NO_ERROR;
    }

    return mCamPtr->cancelAutoFocus();
}

/*
 * when enter picturethread,
 * CE should be at STILL state,
 * all free preview buf should have been enqueue to preview port for snapshot
 * when exit,
 * CE should be at preview state,
 * preview buf status are recorder by bufstate[]
 */
int CameraHardware::pictureThread()
{
    FUNC_TAG_E;

#ifdef OPTIMIZE
    double pic_start = systemTime();
#endif

    status_t ret_code=NO_ERROR;
    sp<IMemory> jpeg_buffer = NULL;
    sp<IMemory> raw_buffer = NULL;
    sp<IMemory> postview_buffer = NULL;
    status_t stat = NO_ERROR;

    //First, dequeue jpeg data, without callback temporarily
    int bufidx=-1;
    int buflen=-1;
    stat = mCamPtr->deqStillBuffer(&bufidx);
    //dequeue still fail, return directly.
    if(NO_ERROR != stat || bufidx < 0){
        TRACE_E("deqStillBuffer failed");
        TRACE_D("%s:NULL jpeg will be delivered.",__FUNCTION__);
        jpeg_buffer = NULL;
        ret_code = UNKNOWN_ERROR;
    }
    else{//still buf generated, try exif based on it
        mBuffersLock.lock();
        mStillImageBuf->setFree(bufidx);
        mBuffersLock.unlock();

#ifdef DUMP_IMAGE
        {
            static int still_counter=0;
            if (UNLIKELY(mDumpNum) > 0) {
                dump_frame_buffer("stilljpeg",still_counter++,mStillImageBuf,bufidx);
            }
        }
#endif
        jpeg_buffer = mCamPtr->getExifImage(bufidx);
        //exif fail, return still image
        if( jpeg_buffer == NULL ){
            TRACE_E("getExifImage failed");
            TRACE_D("%s:jpeg will be delivered.",__FUNCTION__);
            mBuffersLock.lock();
            jpeg_buffer = mStillImageBuf->getIMemory(bufidx);
            mBuffersLock.unlock();
            ret_code = UNKNOWN_ERROR;
        }
        else{
            TRACE_D("%s:exif will be delivered.",__FUNCTION__);
            ret_code = NO_ERROR;
        }
    }

    //raw data callback is not handled by cameraservice when use overlay
    bufidx=-1;
    status_t error = mCamPtr->deqPreviewBuffer(&bufidx);
    if(NO_ERROR != error){
        raw_buffer = NULL;
        TRACE_E("failed to dequeue snapshot image");
    }
    else{
        TRACE_D("%s:get snapshot done.",__FUNCTION__);
        mBuffersLock.lock();
        mPreviewImageBuf->fromDriver(bufidx);
        mPreviewImageBuf->flush(bufidx);
        raw_buffer = mPreviewImageBuf->getIMemory(bufidx);
        mBuffersLock.unlock();
    }

#ifdef OPTIMIZE
    double pic_dur = (systemTime() - pic_start)/1000000;
    if (pic_dur > 0){
        TRACE_D("still catpure duration=%f ms", pic_dur);
    }
#endif

    /*
    //postview callback is not handled by camera apk layer
    if ((postview_buffer != NULL) && (mMsgEnabled & CAMERA_MSG_POSTVIEW_FRAME)){
    TRACE_D("%s:CAMERA_MSG_POSTVIEW_FRAME",__FUNCTION__);
    mDataCb(CAMERA_MSG_POSTVIEW_FRAME,
    NULL,
    mCallbackCookie);
    }
    */

    //raw data callback is not handled by cameraservice when use overlay
    if ((raw_buffer != NULL) && (mMsgEnabled & CAMERA_MSG_RAW_IMAGE)){
        TRACE_D("%s:CAMERA_MSG_RAW_IMAGE",__FUNCTION__);
        mBuffersLock.lock();
        mPreviewImageBuf->toPreview(bufidx);
        mBuffersLock.unlock();
        mDataCb(CAMERA_MSG_RAW_IMAGE,
                raw_buffer,
                mCallbackCookie);
        mBuffersLock.lock();
        mPreviewImageBuf->fromPreview(bufidx);
        mBuffersLock.unlock();
    }

    //callback jpeg data, after postview and rawdata callback.
    if ((jpeg_buffer != NULL) && (mMsgEnabled & CAMERA_MSG_COMPRESSED_IMAGE)) {
        TRACE_D("%s:CAMERA_MSG_COMPRESSED_IMAGE",__FUNCTION__);
        mDataCb(CAMERA_MSG_COMPRESSED_IMAGE,
                jpeg_buffer,
                mCallbackCookie);
    }

    if (NO_ERROR != mCamPtr->stopCapture()){
        TRACE_E("Fail to stopCapture.");
        return -1;
    }
    //free still buf here, to avoid out-of-memory issue
    mCamPtr->unregisterStillBuffers();
    mBuffersLock.lock();
    for (int i = 0; i < kPreviewBufCnt; i++) {
        mStillImageBuf->setFree(i);
    }
    mBuffersLock.unlock();
    //mStillImageBuf.clear();

    FUNC_TAG_X;
    return ret_code;
}

status_t CameraHardware::takePicture()
{
    FUNC_TAG;

    mLock.lock();
    if (mPictureThread != 0) {
        TRACE_E("previous mPictureThread not clear");
        mLock.unlock();
        return INVALID_OPERATION;
    }
    mLock.unlock();

    if(CAM_STATE_IDLE == mState){
        /*
         * If takepicture is invoked at idle status
         */
        if (NO_ERROR != RegisterPreviewBuffers()){
            TRACE_E("Fail to RegisterPreviewBuffers.");
            return -1;
        }
        mBuffersLock.lock();
        for (int i = 0; i < kPreviewBufCnt; i++) {
            if(mPreviewImageBuf->isFree(i)){
                status_t stat=mCamPtr->enqPreviewBuffer(i);
                if(NO_ERROR == stat)
                    mPreviewImageBuf->toDriver(i);
                else
                    TRACE_E("enqPreviewBuffer fail!");
            }
        }
        mBuffersLock.unlock();
        if (NO_ERROR != mCamPtr->startPreview()){
            TRACE_E("Fail to startPreview.");
            return -1;
        }
    }
    else if(CAM_STATE_PREVIEW == mState){
        /*
         * If takepicture is invoked at preview status.
         * DONOT stop ce preview, switch to still caputre mode directly,
         * BUT we do need to stop the camera-hal preview thread.
         * all preview bufs are flushed out.
         * Check the preview thread status on next StartPreview() calling
         */
        __stopPreviewThread();//XXX

        //enqueue preview buffer for snapshot
        mBuffersLock.lock();
        for (int i = 0; i < kPreviewBufCnt; i++) {
            //increase preview/recorder buf ref count
            if(mPreviewImageBuf->isFree(i)){
                status_t stat = mCamPtr->enqPreviewBuffer(i);
                if(NO_ERROR == stat)
                    mPreviewImageBuf->toDriver(i);
                else
                    TRACE_E("enqPreviewBuffer fail!");
            }
        }
        mBuffersLock.unlock();

    }
    else{
        TRACE_E("Invalid state, mState=%d",mState);
        return UNKNOWN_ERROR;
    }

    //force de-ref to avoid out-of-memory
    mStillImageBuf.clear();

    const ImageBuf::EngBufReq bufreq = mCamPtr->getStillBufReq();
    mStillImageBuf = new ImageBuf(
            bufreq,
            kStillBufCnt,
            0,
            PMEM_DEVICE
            );

    //register still buffer
    status_t stat=mCamPtr->registerStillBuffers(mStillImageBuf);
    if(NO_ERROR != stat)
        TRACE_E("registerStillBuffers fail!");
    mBuffersLock.lock();
    for(int i=0;i<kStillBufCnt;i++){
        if(mStillImageBuf->isFree(i)){
            status_t stat = mCamPtr->enqStillBuffer(i);
            if(NO_ERROR == stat)
                mStillImageBuf->toDriver(i);
            else
                TRACE_E("enqStillBuffer fail!");
        }
    }
    mBuffersLock.unlock();
    if (NO_ERROR != mCamPtr->startCapture()){
        TRACE_E("Fail to startCapture.");
        return -1;
    }

    /*
     * switch sensor from preview to capture mode,
     * prepare for capture thread
     */

    mPictureThread = new PictureThread(this);
    if(mPictureThread == NULL){
        TRACE_E("%s:fail to create picturethread",__FUNCTION__);
        return -1;
    }
    mState = CAM_STATE_STILL;
    return NO_ERROR;
}

status_t CameraHardware::cancelPicture()
{
    FUNC_TAG_E;
    sp<PictureThread> pictureThread;
    {//scope for the lock
        Mutex::Autolock lock(mLock);
        pictureThread = mPictureThread;
    }

    //don't hold the lock while waiting for the thread to quit
    if (pictureThread != NULL) {
        TRACE_D("%s(pid=%d): request picture thread to exit",
                __FUNCTION__, gettid());
        pictureThread->requestExitAndWait();
    }

    {
        Mutex::Autolock lock(mLock);
        mPictureThread.clear();
    }

    FUNC_TAG_X;
    return NO_ERROR;
}

status_t CameraHardware::dump(int fd, const Vector<String16>& args) const
{
    FUNC_TAG;
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;
    AutoMutex lock(&mLock);
    write(fd, result.string(), result.size());
    return NO_ERROR;
}

//NOTES: setparameters may not be called during some camera usage.
status_t CameraHardware::setParameters(const CameraParameters& params)
{
    FUNC_TAG;
    Mutex::Autolock lock(mLock);
    TRACE_D("%s:(%s)",__FUNCTION__,params.flatten().string());
    return mCamPtr->setParameters(params);
}

CameraParameters CameraHardware::getParameters() const
{
    FUNC_TAG;
    Mutex::Autolock lock(mLock);
    CameraParameters params = mCamPtr->getParameters();
    TRACE_D("%s:(%s)",__FUNCTION__,params.flatten().string());
    return params;
}

void CameraHardware::release()
{
    cancelPicture();
    stopPreview();

    FUNC_TAG;
}

/*
 * -----------------------------------------------------------
 */
wp<CameraHardwareInterface> CameraHardware::singleton;

sp<CameraHardwareInterface> CameraHardware::createInstance()
{
    FUNC_TAG;
    if (singleton != 0) {
        sp<CameraHardwareInterface> hardware = singleton.promote();
        if (hardware != 0) {
            return hardware;
        }
    }
    sp<CameraHardwareInterface> hardware(new CameraHardware());
    singleton = hardware;
    return hardware;
}

extern "C" sp<CameraHardwareInterface> openCameraHardware()
{
    FUNC_TAG;
    return CameraHardware::createInstance();
}

/*
 * -----------------------------------------------------------
 */
wp<CameraHardwareInterface> CameraHardware::multiInstance[MAX_CAMERAS];

extern sp<CameraHardware> getInstance(wp<CameraHardwareInterface> camera){
    if (camera != 0) {
        sp<CameraHardwareInterface> hardware = camera.promote();
        if (hardware != 0) {
            //'dynamic_cast' not permitted with -fno-rtti,
            //so here we use static_cast instead.
            return static_cast<CameraHardware*>(hardware.get());
        }
    }
    TRACE_E("CameraHardware instace already destroyed,promote failed!");
    return NULL;
}

extern "C" int HAL_getNumberOfCameras()
{
    FUNC_TAG;
    /*
     * if set to nonzero, then enable fake camera, for debug purpose only.
     */
    //DEBUG property: persist.service.camera.fake,
    //if set to 0, do not use fake camera
    //else, use fake camera
    //default: 0;
    char value[PROPERTY_VALUE_MAX];
    property_get(PROP_FAKE, value, "0");
    mEnableFake = atoi(value);
    TRACE_D("%s:"PROP_FAKE"=%d",__FUNCTION__, mEnableFake);

    if(mEnableFake != 0){
        TRACE_D("%s:Enable fake camera!",__FUNCTION__);
        int fakecnt = FakeCam::getNumberOfCameras();
        return fakecnt;
    }

    //DEBUG property: persist.service.camera.cnt,
    //if set to "0", do not probe sensor, return 0;
    //if set to positive, probe sensor, return specified value;
    //if set to negative, probe sensor, return probed sensor count;
    //default: -1;
    property_get(PROP_CNT, value, "-1");
    TRACE_D("%s:"PROP_CNT"=%s",__FUNCTION__,value);
    const int dbg_sensorcnt=atoi(value);

    if (dbg_sensorcnt == 0){
        TRACE_D("%s:use fake cnt=%d",__FUNCTION__,dbg_sensorcnt);
        return dbg_sensorcnt;
    }

    // get the camera engine handle
    int sensorcnt = Engine::getNumberOfCameras();

    if (dbg_sensorcnt > 0){
        TRACE_D("%s:actually probed sensor cnt=%d; but use fake sensor cnt=%d",
                __FUNCTION__,sensorcnt,dbg_sensorcnt);
        return dbg_sensorcnt;
    }

    TRACE_D("%s:use actually probed sensor cnt=%d",__FUNCTION__,sensorcnt);
    return sensorcnt;
}

extern "C" void HAL_getCameraInfo(int cameraId, struct CameraInfo* cameraInfo)
{
    FUNC_TAG;
    if(mEnableFake != 0){
        FakeCam::getCameraInfo(cameraId, cameraInfo);
    }
    else{
        Engine::getCameraInfo(cameraId, cameraInfo);
    }
}

sp<CameraHardwareInterface> CameraHardware::createInstance(int cameraId)
{
    FUNC_TAG;
    TRACE_D("%s:cameraId=%d",__FUNCTION__,cameraId);
    if( MAX_CAMERAS<=cameraId )
    {
        TRACE_E("only suppport %d cameras, required %d",
                MAX_CAMERAS,cameraId);
        return NULL;
    }
    if (multiInstance[cameraId] != 0) {
        sp<CameraHardwareInterface> hardware = multiInstance[cameraId].promote();
        if (hardware != 0) {
            return hardware;
        }
    }
    sp<CameraHardwareInterface> hardware(new CameraHardware(cameraId));
    multiInstance[cameraId] = hardware;
    return hardware;
}

extern "C" sp<CameraHardwareInterface> HAL_openCameraHardware(int cameraId)
{
    TRACE_D("%s:cameraId=%d",__FUNCTION__,cameraId);
    return CameraHardware::createInstance(cameraId);
}

}; // namespace android

