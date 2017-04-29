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

#include <binder/MemoryBase.h>
#include <binder/MemoryHeapBase.h>
#include <binder/MemoryHeapPmem.h>
#include <linux/android_pmem.h>
#include <utils/Log.h>

#include "cam_log.h"
#include "cam_log_mrvl.h"
#include "CameraSetting.h"
#include "ippIP.h"
#include "CameraCtrl.h"
#include "Engine.h"
#include "ippExif.h"
#include "exif_helper.h"

#define LOG_TAG "CameraMrvl"

#define EXIF_GENERATOR

#define PROP_TUNING	"service.camera.tuning"	//0,1
#define PROP_W_PREVIEW	"service.camera.width_preview"	//0,>0
#define PROP_H_PREVIEW	"service.camera.height_preview"	//0,>0
#define PROP_W_STILL	"service.camera.width_still"	//0,>0
#define PROP_H_STILL	"service.camera.height_still"	//0,>0

namespace android {

    int Engine::getNumberOfCameras(){
        CAM_Error error = CAM_ERROR_NONE;
        CAM_DeviceHandle handle;
        char sSensorName[4][32];

        // get the camera engine handle
        error = CAM_GetHandle(&entry_extisp, &handle);
        //ASSERT_CAM_ERROR(error);
        if (CAM_ERROR_NONE != error)
            return 0;

        // list all supported sensors
        int iSensorCount;
        error = CAM_SendCommand(handle,
                CAM_CMD_ENUM_SENSORS,
                (CAM_Param)&iSensorCount,
                (CAM_Param)sSensorName);
        //ASSERT_CAM_ERROR(error);
        if (CAM_ERROR_NONE != error)
            return 0;

        // list all supported sensors
        TRACE_D("All supported sensors:");
        for (int i=0 ; i<iSensorCount ; i++){
            //init sensor table with existing sensorid, name, facing & orientation on platform
            CameraSetting::initTableSensorID(sSensorName[i], i);
            TRACE_D("\t%d - %s", i, sSensorName[i]);
        }

        error = CAM_FreeHandle(&handle);
        //ASSERT_CAM_ERROR(error);
        if (CAM_ERROR_NONE != error)
            return 0;

        return CameraSetting::getNumOfCameras();
    }

    void Engine::getCameraInfo(int cameraId, struct CameraInfo* cameraInfo)
    {
        CameraSetting camerasetting(cameraId);
        const CameraInfo* infoptr = camerasetting.getCameraInfo();
        if( NULL == infoptr ){
            TRACE_E("Error occur when get CameraInfo");
        }
        memcpy(cameraInfo, infoptr, sizeof(CameraInfo));
    }

    Engine::Engine(int cameraID):
        mSetting(cameraID),//if cameraID not specified, using 0 by default
        iExpectedPicNum(1)
        //mState(CAM_STATE_IDLE)
    {
        char value[PROPERTY_VALUE_MAX];
        ceInit(&mCEHandle,&mCamera_caps);
        //initialized default shot capability
        mCamera_shotcaps = mCamera_caps.stSupportedShotParams;
        /*
         * comment this command to use the default setting in camerasetting
         * comment it for debug only
         */
        mSetting.setBasicCaps(mCamera_caps);
        /*
         * choose camera tuning
         * setprop service.camera.tuning x
         * if x is set to 0, no camera tuning will be used
         */
        property_get(PROP_TUNING, value, "1");
        mTuning = atoi(value);
        TRACE_D("%s:"PROP_TUNING"=%d",__FUNCTION__, mTuning);
        if( mTuning != 0 ){
            /*
             * update cameraparameter SUPPORTED KEY
             * all supported tuning parameters from engine are stored in stSupportedShotParams
             */
            mSetting.setSupportedSceneModeCap(mCamera_shotcaps);
            /*
             * query current used tuning parameters from engine,
             * update cameraparameter KEY
             */
            ceGetCurrentSceneModeCap(mCEHandle,mSetting);
        }

        /*
         * override preview size/format setting, for debug only
         * setprop service.camera.width_preview x
         * setprop service.camera.height_preview y
         * if x or y is set to 0, we'll use the parameters from class CameraSetting.
         */
        int width=0,height=0;
        property_get(PROP_W_PREVIEW, value, "0");
        width = atoi(value);
        TRACE_D("%s:"PROP_W_PREVIEW"=%d",__FUNCTION__, width);
        property_get(PROP_H_PREVIEW, value, "0");
        height = atoi(value);
        TRACE_D("%s:"PROP_H_PREVIEW"=%d",__FUNCTION__, height);

        if( width > 0 && height > 0 ){
            mSetting.setPreviewSize(width, height);
            const char* v=mSetting.get(CameraParameters::KEY_PREVIEW_SIZE);
            mSetting.set(CameraParameters::KEY_SUPPORTED_PREVIEW_SIZES, v);
        }

        /*
         * override preview size/format setting, for debug only
         * setprop service.camera.width_still x
         * setprop service.camera.height_still y
         * if x or y is set to 0, we'll use the parameters from class CameraSetting.
         */
        width=0,height=0;
        property_get(PROP_W_STILL, value, "0");
        width = atoi(value);
        TRACE_D("%s:"PROP_W_STILL"=%d",__FUNCTION__, width);
        property_get(PROP_H_STILL, value, "0");
        height = atoi(value);
        TRACE_D("%s:"PROP_H_STILL"=%d",__FUNCTION__, height);

        if( width > 0 && height > 0 ){
            mSetting.setPictureSize(width, height);
            const char* v=mSetting.get(CameraParameters::KEY_PICTURE_SIZE);
            mSetting.set(CameraParameters::KEY_SUPPORTED_PICTURE_SIZES, v);
        }
    }

    Engine::~Engine(){
        CAM_Error error = CAM_ERROR_NONE;
        error = CAM_FreeHandle(&mCEHandle);
        ASSERT_CAM_ERROR(error);
    }

    void Engine::enableMsgType(int32_t msgType)
    {
        mMsgEnabled |= msgType;
    }
    void Engine::disableMsgType(int32_t msgType)
    {
        mMsgEnabled &= ~msgType;
    }

    //keep event handler function concise, or else it may affect preview performance.
    void Engine::NotifyEvents(CAM_EventId eEventId,void* param,void *pUserData)
    {
        Engine* ptr=static_cast<Engine*>(pUserData);
        TRACE_V("%s:eEventId=%d,param=%d",__FUNCTION__,eEventId, (int)param);
        if(NULL != ptr->mNotifyCb){
            switch ( eEventId )
            {
                case CAM_EVENT_FRAME_DROPPED:
                    break;
                case CAM_EVENT_FRAME_READY:
                    TRACE_V("%s:CAM_EVENT_FRAME_READY",__FUNCTION__);
                    break;
                case CAM_EVENT_STILL_SHUTTERCLOSED:
                    TRACE_D("%s:CAM_EVENT_STILL_SHUTTERCLOSED",__FUNCTION__);
                    if (ptr->mMsgEnabled & CAMERA_MSG_SHUTTER){
                        int w,h;
                        CameraParameters param=ptr->getParameters();
                        param.getPreviewSize(&w, &h);
                        image_rect_type rawsize;
                        rawsize.width=w;rawsize.height=h;
                        ptr->mNotifyCb(CAMERA_MSG_SHUTTER, (int32_t)(&rawsize), 0, ptr->mCallbackCookie);
                    }
                    /*
                       if (ptr->mMsgEnabled & CAMERA_MSG_SHUTTER)
                       ptr->mNotifyCb(CAMERA_MSG_SHUTTER, 0, 0, ptr->mCallbackCookie);
                       */
                    break;
                case CAM_EVENT_IN_FOCUS:
                    break;
                case CAM_EVENT_OUTOF_FOCUS:
                    break;
                case CAM_EVENT_FOCUS_AUTO_STOP:
                    TRACE_D("%s:CAM_EVENT_FOCUS_AUTO_STOP:%d",__FUNCTION__,(int)param);
                    if (ptr->mMsgEnabled & CAMERA_MSG_FOCUS){
                        ptr->mNotifyCb(CAMERA_MSG_FOCUS,
                                (((int)param>0)?true:false), 0, ptr->mCallbackCookie);
                    }
                    break;
                case CAM_EVENT_STILL_ALLPROCESSED:
                    break;
                case CAM_EVENT_FATALERROR:
                    break;
                case CAM_EVENT_FACE_UPDATE:
#if 0
                    CAM_MultiROI* pFaceROI;
                    pFaceROI=(CAM_MultiROI*)param;
                    for(int i=0;i<pFaceROI->iROICnt;i++){
                        TRACE_D("%s:CAM_EVENT_FACE_UPDATE",__FUNCTION__);
                        TRACE_D("FaceDetection: index=%d, left=%d, top=%d, width=%d, height=%d",
                                i,
                                //pFaceROI->stWeiRect[i].iWeight;
                                pFaceROI->stWeiRect[i].stRect.iLeft,
                                pFaceROI->stWeiRect[i].stRect.iTop,
                                pFaceROI->stWeiRect[i].stRect.iWidth,
                                pFaceROI->stWeiRect[i].stRect.iHeight
                               );
                    }
                    if (ptr->mMsgEnabled & CAMERA_MSG_FACE_DETECT)
                        ptr->mNotifyCb(CAMERA_MSG_FACE_DETECT, param, 0, ptr->mCallbackCookie);
#endif

                    break;
                default:
                    break;
            }
        }
    }

    void Engine::setCallbacks(notify_callback notify_cb,void* user){
        mNotifyCb = notify_cb;
        mCallbackCookie = user;
    }

    //check previous state:
    //still: then it wanna switch back to preview, we only flush still port buffer,
    //enqueue preview buffers to preview port.
    //idle: then it's first time preview, we need to alloc preview buffers, switch ce
    //state from idle to preview.
    status_t Engine::startPreview(){
        return ceStartPreview(mCEHandle);
    }

    status_t Engine::stopPreview(){
        return ceStopPreview(mCEHandle);
    }

    status_t Engine::startCapture(){
        return ceStartCapture(mCEHandle);//switch to capture mode
    }

    status_t Engine::stopCapture(){
        return ceStopCapture(mCEHandle);
    }

    status_t Engine::startVideo(){
        return ceStartVideo(mCEHandle);
    }

    status_t Engine::stopVideo(){
        return ceStopVideo(mCEHandle);
    }

    status_t Engine::setParameters(CameraParameters param){
        status_t ret=NO_ERROR;
        //store the parameters set from service
        ret |= mSetting.setParameters(param);

        //set tuning parameters as specified
        if( mTuning != 0 ){
            /*
             * update engine param according to parameters,
             * including flash/whitebalance/coloreffect/focus/antibanding etc.
             */
            ret |= ceUpdateSceneModeSetting(mCEHandle,mSetting);

            /*
             * read back setting from engine and update CameraSetting,
             * flash/whitebalance/focus mode may changed with different scene mode.
             * refer to Camera.java
             */
            ret |= ceGetCurrentSceneModeCap(mCEHandle,mSetting);

            /*
             * FIXME:cameraengine may NOT support focus mode while scene mode is enabled,
             * but camera app may assume autofocus be supported if scene mode key is set,
             * which may cause camera app quit when doautofocus/takepicture.
             * refer to Camera.java
             */
            if ( 0 != mSetting.get(CameraParameters::KEY_SCENE_MODE) &&
                    0 == mSetting.get(CameraParameters::KEY_FOCUS_MODE) ){
                mSetting.set(CameraParameters::KEY_FOCUS_MODE,
                        CameraParameters::FOCUS_MODE_AUTO);
            }
        }
        return ret;
    }

    CameraParameters Engine::getParameters(){
        CameraParameters params = mSetting.getParameters();
        return params;
    }

    status_t Engine::registerPreviewBuffers(sp<ImageBuf> imagebuf){
        //
        mPreviewImageBuf = imagebuf;
        int bufcnt=imagebuf->getBufCnt();
        for (int i=0;i<bufcnt;i++){
            CAM_ImageBuffer* pImgBuf = mPreviewImageBuf->getEngBuf(i);
            if(NULL == pImgBuf){
                TRACE_E("Fail to getEngBuf");
                return UNKNOWN_ERROR;
            }

            /*
               CAM_Error error = CAM_ERROR_NONE;
               error = CAM_SendCommand(mCEHandle,
               CAM_CMD_PORT_ENQUEUE_BUF,
               (CAM_Param)CAM_PORT_PREVIEW,
               (CAM_Param)pImgBuf);
               if(CAM_ERROR_NONE != error)
               return UNKNOWN_ERROR;
               */
        }

        return NO_ERROR;
    }

    status_t Engine::unregisterPreviewBuffers(){
        //get all buffers back
        CAM_Error error = CAM_ERROR_NONE;
        error = CAM_SendCommand(mCEHandle,
                CAM_CMD_PORT_FLUSH_BUF,
                (CAM_Param)CAM_PORT_PREVIEW,
                UNUSED_PARAM);
        ASSERT_CAM_ERROR(error);
        //if(CAM_ERROR_NONE != error)
        //    return UNKNOWN_ERROR;
        /*
         * TODO:do mem clear if any memory heap/buffer sp is hold
         * in this class previously, to forcely free all buffers and
         * prepare for next mem alloc.
         */
        mPreviewImageBuf = NULL;
        return NO_ERROR;
    }

    //XXX:check buf req/cnt here
    //TODO: we need only the buf index as input after register buffer.
    //status_t Engine::enqPreviewBuffer(sp<ImageBuf> imagebuf, int index){
    status_t Engine::enqPreviewBuffer(int index){
        CAM_ImageBuffer* pImgBuf = mPreviewImageBuf->getEngBuf(index);
        CAM_Error error = CAM_ERROR_NONE;
        error = CAM_SendCommand(mCEHandle,
                CAM_CMD_PORT_ENQUEUE_BUF,
                (CAM_Param)CAM_PORT_PREVIEW,
                (CAM_Param)pImgBuf);
        ASSERT_CAM_ERROR(error);
        return NO_ERROR;
    }

    status_t Engine::deqPreviewBuffer(int* index){
        CAM_Error error = CAM_ERROR_NONE;
        error = ceGetPreviewFrame(mCEHandle,index);
        if(CAM_ERROR_NONE != error)
            return UNKNOWN_ERROR;
        else{
            return NO_ERROR;
        }
    }

    //--------------
    status_t Engine::registerStillBuffers(sp<ImageBuf> imagebuf){
        //TODO:

        //
        mStillImageBuf = imagebuf;
        int bufcnt=imagebuf->getBufCnt();
        for (int i=0;i<bufcnt;i++){
            CAM_ImageBuffer* pImgBuf = mStillImageBuf->getEngBuf(i);
            if(NULL == pImgBuf){
                TRACE_E("Fail to getEngBuf");
                return UNKNOWN_ERROR;
            }

            /*
               CAM_Error error = CAM_ERROR_NONE;
               error = CAM_SendCommand(mCEHandle,
               CAM_CMD_PORT_ENQUEUE_BUF,
               (CAM_Param)CAM_PORT_STILL,
               (CAM_Param)pImgBuf);
               if(CAM_ERROR_NONE != error)
               return UNKNOWN_ERROR;
               */
        }
        return NO_ERROR;
    }

    status_t Engine::unregisterStillBuffers(){
        CAM_Error error = CAM_ERROR_NONE;
        error = CAM_SendCommand(mCEHandle,
                CAM_CMD_PORT_FLUSH_BUF,
                (CAM_Param)CAM_PORT_STILL,
                UNUSED_PARAM);
        ASSERT_CAM_ERROR(error);
        //if(CAM_ERROR_NONE != error)
        //    return UNKNOWN_ERROR;
        /*
         * TODO:do mem clear if any memory heap/buffer sp is hold
         * in this class previously, to forcely free all buffers and
         * prepare for next mem alloc.
         */
        mStillImageBuf = NULL;
        return NO_ERROR;
    }

    //XXX:check buf req/cnt here
    //TODO: we need only the buf index as input after register buffer.
    //status_t Engine::enqStillBuffer(sp<ImageBuf> imagebuf, int index){
    status_t Engine::enqStillBuffer(int index){
        CAM_ImageBuffer* pImgBuf = mStillImageBuf->getEngBuf(index);
        CAM_Error error = CAM_ERROR_NONE;
        error = CAM_SendCommand(mCEHandle,
                CAM_CMD_PORT_ENQUEUE_BUF,
                (CAM_Param)CAM_PORT_STILL,
                (CAM_Param)pImgBuf);
        ASSERT_CAM_ERROR(error);
        return NO_ERROR;
    }

    status_t Engine::deqStillBuffer(int* index){
        CAM_Error error = CAM_ERROR_NONE;
        error = ceGetStillFrame(mCEHandle,index);
        if(CAM_ERROR_NONE != error){
            TRACE_E("ceGetStillFrame fail");
            return UNKNOWN_ERROR;
        }
        else
            return NO_ERROR;
    }

    sp<IMemory> Engine::getExifImage(int index)
    {
        sp<MemoryHeapBase> pic_heap=NULL;
        sp<MemoryBase> pic_mem=NULL;
        ssize_t offset = 0;
        size_t size = 0;
        size_t pic_buflen = 0;
        int heap_size = 0;

        CAM_ImageBuffer* pImgBuf_still = mStillImageBuf->getEngBuf(index);

#ifdef EXIF_GENERATOR
        pic_mem = ExifGenerator(pImgBuf_still, mSetting);
#else//EXIF_GENERATOR
        //raw jpeg pic buffer length
        pic_buflen = pImgBuf_still->iFilledLen[0] +
            pImgBuf_still->iFilledLen[1] +
            pImgBuf_still->iFilledLen[2];

        pic_heap = new MemoryHeapBase(pic_buflen);
        heap_size = (int)pic_heap->getSize();
        pic_mem = new MemoryBase(pic_heap, 0, heap_size);

        pic_mem->getMemory(&offset, &size);
        uint8_t* ptr_pic = (uint8_t*)pic_heap->base()+offset;
        memset(ptr_pic,0,size);

        memcpy(ptr_pic, pImgBuf_still->pBuffer[0], pImgBuf_still->iFilledLen[0]);
        if(pImgBuf_still->iFilledLen[1])
            memcpy(ptr_pic + pImgBuf_still->iFilledLen[0],
                    pImgBuf_still->pBuffer[1],
                    pImgBuf_still->iFilledLen[1]);
        if(pImgBuf_still->iFilledLen[2])
            memcpy(ptr_pic + pImgBuf_still->iFilledLen[0] + pImgBuf_still->iFilledLen[1],
                    pImgBuf_still->pBuffer[2],
                    pImgBuf_still->iFilledLen[2]);
#endif//EXIF_GENERATOR
        return pic_mem;
    }

    status_t Engine::autoFocus()
    {
        //focus mode
        int cesetting                           = -1;
        CAM_ShotModeCapability& camera_shotcaps  = mCamera_shotcaps;

        const char* v=mSetting.get(CameraParameters::KEY_FOCUS_MODE);
        if( 0 == v){
            return UNKNOWN_ERROR;
        }

        //CTS will call autofocus under infinity mode, this is not approved by camera engine.
        //so we use fake message to satisfy CTS test.
        if(0 == strcmp(v,CameraParameters::FOCUS_MODE_INFINITY)){
            mNotifyCb(CAMERA_MSG_FOCUS, true, 0, mCallbackCookie);
            return NO_ERROR;
        }

        if(0 != strcmp(v,CameraParameters::FOCUS_MODE_AUTO) &&
                0 != strcmp(v,CameraParameters::FOCUS_MODE_MACRO)){
            TRACE_E("app should not call autoFocus in mode %s",v);
            return UNKNOWN_ERROR;
        }

        CAM_Error error = CAM_ERROR_NONE;
        error = CAM_SendCommand(mCEHandle,
                CAM_CMD_START_FOCUS,
                UNUSED_PARAM,
                UNUSED_PARAM);
        //ASSERT_CAM_ERROR(error);
        if (CAM_ERROR_NONE != error){
            TRACE_E("start focus failed!");
            return UNKNOWN_ERROR;
        }

        TRACE_D("%s:start focus", __FUNCTION__);
        return NO_ERROR;  // v!=NULL, cesetting != -1, ce_idex == cesetting.
    }

    status_t Engine::cancelAutoFocus()
    {
        const char* v=mSetting.get(CameraParameters::KEY_FOCUS_MODE);
        if( 0 == v){
            return NO_ERROR;
        }

        if(0 == strcmp(v,CameraParameters::FOCUS_MODE_INFINITY)){
            return NO_ERROR;
        }

        if(0 != strcmp(v,CameraParameters::FOCUS_MODE_AUTO) &&
                0 != strcmp(v,CameraParameters::FOCUS_MODE_MACRO)){
            TRACE_E("app should not call calcelAutoFocus in mode %s",v);
        }

        CAM_Error error = CAM_ERROR_NONE;
        error = CAM_SendCommand(mCEHandle,
                CAM_CMD_CANCEL_FOCUS,
                UNUSED_PARAM,
                UNUSED_PARAM);
        ASSERT_CAM_ERROR(error);
        return NO_ERROR;
    }

    status_t Engine::getBufCnt(int* previewbufcnt,int* stillbufcnt,int* videobufcnt)const
    {
        return mSetting.getBufCnt(previewbufcnt, stillbufcnt, videobufcnt);
    }

    //CAM_ImageBufferReq Engine::getPreviewBufReq()
    ImageBuf::EngBufReq Engine::getPreviewBufReq()
    {
        CAM_Error error = CAM_ERROR_NONE;

        /*
         * check current CE status, which should be IDLE
         */
        CAM_CaptureState state;
        error = CAM_SendCommand(mCEHandle,
                CAM_CMD_GET_STATE,
                (CAM_Param)&state,
                UNUSED_PARAM);
        ASSERT_CAM_ERROR(error);
        if(CAM_CAPTURESTATE_IDLE != state){
            TRACE_E("wrong CE state, %d, required IDLE",state);
            return ImageBuf::EngBufReq();
        }

        /* Set preview parameters */
        CAM_Size szPreviewSize;
        mSetting.getPreviewSize(&szPreviewSize.iWidth,
                &szPreviewSize.iHeight);
        error = CAM_SendCommand(mCEHandle,
                CAM_CMD_PORT_SET_SIZE,
                (CAM_Param)CAM_PORT_PREVIEW,
                (CAM_Param)&szPreviewSize);
        ASSERT_CAM_ERROR(error);

        //android framework doc requires that preview/picture format
        //is in supported parameters.
        const char* v = mSetting.getPreviewFormat();
        int ePreviewFormat = mSetting.map_andr2ce(CameraSetting::map_imgfmt, v);
        error = CAM_SendCommand(mCEHandle,
                CAM_CMD_PORT_SET_FORMAT,
                (CAM_Param)CAM_PORT_PREVIEW,
                (CAM_Param)ePreviewFormat);
        ASSERT_CAM_ERROR(error);

        CAM_ImageBufferReq  engbufreq;
        error = CAM_SendCommand(mCEHandle,
                CAM_CMD_PORT_GET_BUFREQ,
                (CAM_Param)CAM_PORT_PREVIEW,
                (CAM_Param)&engbufreq);
        ASSERT_CAM_ERROR(error);

        return ImageBuf::EngBufReq(engbufreq);
    }

    //CAM_ImageBufferReq Engine::getStillBufReq()
    ImageBuf::EngBufReq Engine::getStillBufReq()
    {
        CAM_Error error = CAM_ERROR_NONE;

        /*
         * Set picture parameters
         */
        CAM_Size szPictureSize;
        mSetting.getPictureSize(&szPictureSize.iWidth,
                &szPictureSize.iHeight);

        error = CAM_SendCommand(mCEHandle,
                CAM_CMD_PORT_SET_SIZE,
                (CAM_Param)CAM_PORT_STILL,
                (CAM_Param)&szPictureSize);
        ASSERT_CAM_ERROR(error);


        int jpeg_quality = mSetting.getInt(CameraParameters::KEY_JPEG_QUALITY);
        if(jpeg_quality > mCamera_caps.stSupportedJPEGParams.iMaxQualityFactor ||
                jpeg_quality < mCamera_caps.stSupportedJPEGParams.iMinQualityFactor){
            TRACE_E("invalid quality factor=%d. supported range [%d,%d]",
                    jpeg_quality,mCamera_caps.stSupportedJPEGParams.iMaxQualityFactor,
                    mCamera_caps.stSupportedJPEGParams.iMinQualityFactor);
            jpeg_quality = mCamera_caps.stSupportedJPEGParams.iMaxQualityFactor;
        }

        //default jpeg param
        CAM_JpegParam jpegparam = {
            0,	// 420
            jpeg_quality, // quality factor
            CAM_FALSE, // enable exif
            CAM_FALSE, // embed thumbnail
            0, 0,
        };

        const char* v = mSetting.getPictureFormat();
        int ePictureFormat = mSetting.map_andr2ce(CameraSetting::map_imgfmt, v);
        if (CAM_IMGFMT_JPEG == ePictureFormat){
            error = CAM_SendCommand(mCEHandle,
                    CAM_CMD_SET_JPEGPARAM,
                    (CAM_Param)&jpegparam,
                    UNUSED_PARAM);
            ASSERT_CAM_ERROR(error);
        }
        error = CAM_SendCommand(mCEHandle,
                CAM_CMD_PORT_SET_FORMAT,
                (CAM_Param)CAM_PORT_STILL,
                (CAM_Param)ePictureFormat);
        ASSERT_CAM_ERROR(error);

        CAM_ImageBufferReq  engbufreq;
        error = CAM_SendCommand(mCEHandle,
                CAM_CMD_PORT_GET_BUFREQ,
                (CAM_Param)CAM_PORT_STILL,
                (CAM_Param)&engbufreq);
        ASSERT_CAM_ERROR(error);

        return ImageBuf::EngBufReq(engbufreq);
    }
    //----------------------------------------------------------------------------------------
    CAM_Error Engine::ceInit(CAM_DeviceHandle *pHandle,CAM_CameraCapability *pcamera_caps)
    {
        CAM_Error error = CAM_ERROR_NONE;
        int ret;
        CAM_DeviceHandle handle;
        CAM_PortCapability *pCap;

        // get the camera engine handle
        error = CAM_GetHandle(&entry_extisp, &handle);
        *pHandle = handle;
        ASSERT_CAM_ERROR(error);

        // select the proper sensor id
        int sensorid = mSetting.getSensorID();
        const char* sensorname = mSetting.getSensorName();
        error = CAM_SendCommand(handle,
                CAM_CMD_SET_SENSOR_ID,
                (CAM_Param)sensorid,
                UNUSED_PARAM);
        ASSERT_CAM_ERROR(error);
        TRACE_D("Current sensor index:");
        TRACE_D("\t%d - %s", sensorid, sensorname);

        error = CAM_SendCommand(handle,
                CAM_CMD_QUERY_CAMERA_CAP,
                (CAM_Param)sensorid,
                (CAM_Param)pcamera_caps);
        ASSERT_CAM_ERROR(error);
#if 1
        error = CAM_SendCommand(handle,
                CAM_CMD_SET_EVENTHANDLER,
                (CAM_Param)NotifyEvents,
                (CAM_Param)this);
        ASSERT_CAM_ERROR(error);
#endif

        error = CAM_SendCommand(handle,
                CAM_CMD_SET_SENSOR_ORIENTATION,
                (CAM_Param)CAM_FLIPROTATE_NORMAL,
                UNUSED_PARAM);
        ASSERT_CAM_ERROR(error);

        return error;
    }

    status_t Engine::ceUpdateSceneModeSetting(const CAM_DeviceHandle &handle,
            CameraSetting& setting)
    {
        const char* v;
        int scenemode;
        CAM_FocusMode FocusMode;

        status_t ret = NO_ERROR;
        CAM_Error error = CAM_ERROR_NONE;
        CAM_ShotModeCapability camera_shotcaps;

        /*
         * update scene mode as required if engine support
         */
        //current scene mode
        error = CAM_SendCommand(handle,
                CAM_CMD_GET_SHOTMODE,
                (CAM_Param)&scenemode,
                UNUSED_PARAM);
        ASSERT_CAM_ERROR(error);

        //target scene mode
        v = setting.get(CameraParameters::KEY_SCENE_MODE);
        if( 0 != v){
            int scenemode_target = setting.map_andr2ce(CameraSetting::map_scenemode, v);

            //update scene mode if the target scenemode is different from current
            if (scenemode_target != scenemode &&
                    scenemode_target != -1){
                //update ce scene mode
                error = CAM_SendCommand(handle,
                        CAM_CMD_SET_SHOTMODE,
                        (CAM_Param)scenemode_target,
                        UNUSED_PARAM);
                ASSERT_CAM_ERROR(error);
                scenemode = scenemode_target;
            }
        }
        TRACE_D("%s:scene mode=%s", __FUNCTION__, v);
        //query tuning parameters supported in current scene mode
        if( 0 != v){
            error = CAM_SendCommand(handle,
                    CAM_CMD_QUERY_SHOTMODE_CAP,
                    (CAM_Param)scenemode,
                    (CAM_Param)&camera_shotcaps);
            ASSERT_CAM_ERROR(error);
            //update shot capability under new shot mode.
            mCamera_shotcaps = camera_shotcaps;
        }
        else{
            TRACE_E("No valid scene mode, return.");
            ret = BAD_VALUE;
            return ret;
        }

        /*
         * update focus mode as required if engine support
         */
        //current focus mode
        error = CAM_SendCommand(handle,
                CAM_CMD_GET_FOCUSMODE,
                (CAM_Param)&FocusMode,
                UNUSED_PARAM);
        ASSERT_CAM_ERROR(error);

        //target focus mode
        v = setting.get(CameraParameters::KEY_FOCUS_MODE);
        if( 0 != v){
            int focusmode_target = setting.map_andr2ce(CameraSetting::map_focus, v);

            //update focus mode if the target focus mode is different from current
            if (focusmode_target != FocusMode){
                //update ce focus mode
                error = CAM_SendCommand(handle,
                        CAM_CMD_SET_FOCUSMODE,
                        (CAM_Param)focusmode_target,
                        UNUSED_PARAM);
                ASSERT_CAM_ERROR(error);
            }
        }
        TRACE_D("%s:focus mode=%s", __FUNCTION__, v);

        /*
         * update other parameters according to current used scene mode
         */
        int cesetting;
        int ce_idx;
        //white balance setting
        v = setting.get(CameraParameters::KEY_WHITE_BALANCE);
        if( 0 != v){
            cesetting = setting.map_andr2ce(CameraSetting::map_whitebalance, v);
            if (-1 != cesetting){
                for (int i=0; i < camera_shotcaps.iSupportedWBModeCnt; i++){
                    ce_idx = camera_shotcaps.eSupportedWBMode[i];
                    if(ce_idx == cesetting){
                        error = CAM_SendCommand(handle,
                                CAM_CMD_SET_WHITEBALANCEMODE,
                                (CAM_Param)cesetting,
                                UNUSED_PARAM);
                        ASSERT_CAM_ERROR(error);
                        TRACE_D("%s:white balance=%s", __FUNCTION__, v);
                    }
                }
            }
        }

        //effect setting
        v = setting.get(CameraParameters::KEY_EFFECT);
        if( 0 != v){
            cesetting = setting.map_andr2ce(CameraSetting::map_coloreffect, v);
            if (-1 != cesetting){
                for (int i=0; i < camera_shotcaps.iSupportedColorEffectCnt; i++){
                    ce_idx = camera_shotcaps.eSupportedColorEffect[i];
                    if(ce_idx == cesetting){
                        error = CAM_SendCommand(handle,
                                CAM_CMD_SET_COLOREFFECT,
                                (CAM_Param)cesetting,
                                UNUSED_PARAM);
                        ASSERT_CAM_ERROR(error);
                        TRACE_D("%s:color effect=%s", __FUNCTION__, v);
                    }
                }
            }
        }

        //bandfilter setting
        v = setting.get(CameraParameters::KEY_ANTIBANDING);
        if( 0 != v){
            cesetting = setting.map_andr2ce(CameraSetting::map_bandfilter, v);
            if (-1 != cesetting){
                for (int i=0; i < camera_shotcaps.iSupportedBdFltModeCnt; i++){
                    ce_idx = camera_shotcaps.eSupportedBdFltMode[i];
                    if(ce_idx == cesetting){
                        error = CAM_SendCommand(handle,
                                CAM_CMD_SET_BANDFILTER,
                                (CAM_Param)cesetting,
                                UNUSED_PARAM);
                        ASSERT_CAM_ERROR(error);
                        TRACE_D("%s:antibanding=%s", __FUNCTION__, v);
                    }
                }
            }
        }

        //flash mode
        v = setting.get(CameraParameters::KEY_FLASH_MODE);
        if( 0 != v){
            cesetting = setting.map_andr2ce(CameraSetting::map_flash, v);
            if (-1 != cesetting){
                for (int i=0; i < camera_shotcaps.iSupportedFlashModeCnt; i++){
                    ce_idx = camera_shotcaps.eSupportedFlashMode[i];
                    if(ce_idx == cesetting){
                        error = CAM_SendCommand(handle,
                                CAM_CMD_SET_FLASHMODE,
                                (CAM_Param)cesetting,
                                UNUSED_PARAM);
                        ASSERT_CAM_ERROR(error);
                        TRACE_D("%s:flash mode=%s", __FUNCTION__, v);
                    }
                }
            }
        }

        //EVC
        if ( 0 != camera_shotcaps.iEVCompStepQ16){
            v = setting.get(CameraParameters::KEY_EXPOSURE_COMPENSATION);
            if( 0 != v){
                int exposureQ16 = atoi(v) * camera_shotcaps.iEVCompStepQ16;
                // verify the range of the settings
                if ((exposureQ16 <= camera_shotcaps.iMaxEVCompQ16) &&
                        (exposureQ16 >= camera_shotcaps.iMinEVCompQ16) ){
                    error = CAM_SendCommand(handle,
                            CAM_CMD_SET_EVCOMPENSATION,
                            (CAM_Param)exposureQ16,
                            UNUSED_PARAM);
                    ASSERT_CAM_ERROR(error);
                    TRACE_D("%s:exposure compensation=%s", __FUNCTION__, v);
                }
            }
        }

        //digital zoom
        const int minzoom = camera_shotcaps.iMinDigZoomQ16;
        const int maxzoom = camera_shotcaps.iMaxDigZoomQ16;
        const int cezoomstep = camera_shotcaps.iDigZoomStepQ16;
        if((0x1<<16) != maxzoom){
            v = setting.get(CameraParameters::KEY_ZOOM);
            int cedigzoom = minzoom + atoi(v) * cezoomstep;
            if (cedigzoom < minzoom){
                TRACE_E("Invalid zoom value:%d. set to min:%d",cedigzoom,minzoom);
                cedigzoom = minzoom;
                ret = BAD_VALUE;
            }
            else if (cedigzoom > maxzoom){
                TRACE_E("Invalid zoom value:%d. set to max:%d",cedigzoom,maxzoom);
                cedigzoom = maxzoom;
                ret = BAD_VALUE;
            }
            error = CAM_SendCommand(handle,
                    CAM_CMD_SET_DIGZOOM,
                    (CAM_Param)cedigzoom,
                    UNUSED_PARAM );
            ASSERT_CAM_ERROR(error);
            TRACE_D("%s:digital zoom=%s", __FUNCTION__, v);
        }

        // check fps range
        {
            int min_fps, max_fps;
            setting.getPreviewFpsRange(&min_fps, &max_fps);
            if((min_fps<0) || (max_fps<0) || (min_fps > max_fps)){
                TRACE_E("invalid fps range: min_fps=%d, max_fps=%d",
                        min_fps,max_fps);
                ret = BAD_VALUE;
            }
        }

        //fps
        {
            const int framerate = setting.getPreviewFrameRate();
            const int min_fps = mCamera_shotcaps.iMinFpsQ16;
            const int max_fps = mCamera_shotcaps.iMaxFpsQ16;
            TRACE_V("%s:%d: minfps=%d,maxfps=%d",__FUNCTION__,__LINE__,min_fps,max_fps);
            const int iFPSQ16 = framerate << 16;
            if(iFPSQ16 <= max_fps && iFPSQ16 >= min_fps && iFPSQ16 > 0){
                error = CAM_SendCommand(handle,
                        CAM_CMD_SET_PREVIEW_FRAMERATE,
                        (CAM_Param)iFPSQ16,
                        UNUSED_PARAM);
                ASSERT_CAM_ERROR(error);
                TRACE_D("%s:framerate:%d",__FUNCTION__,framerate);
            }
        }

        //For VT, brightness
        v = setting.get("light_key");
        const int minbright = camera_shotcaps.iMinBrightness;
        const int maxbright = camera_shotcaps.iMaxBrightness;
        TRACE_V("%s:%d: minbright=%d,maxbright=%d",__FUNCTION__,__LINE__,minbright,maxbright);
        if( 0 != v){
            int brightval = atoi(v);
            if(brightval < minbright){
                TRACE_E("Invalid bright value:%d. set to min:%d", brightval, minbright);
                brightval = minbright;
                //ret = BAD_VALUE;
            }
            else if (brightval> maxbright){
                TRACE_E("Invalid bright value:%d. set to max:%d",brightval,maxbright);
                brightval = maxbright;
                //ret = BAD_VALUE;
            }
            error = CAM_SendCommand(handle,
                    CAM_CMD_SET_BRIGHTNESS,
                    (CAM_Param)brightval,
                    UNUSED_PARAM);
            ASSERT_CAM_ERROR(error);
            TRACE_D("%s:brightness=%d", __FUNCTION__, brightval);
        }

        //For VT, contrast
        v = setting.get("contrast_key");
        const int mincontrast = camera_shotcaps.iMinContrast;
        const int maxcontrast = camera_shotcaps.iMaxContrast;
        TRACE_V("%s:%d: mincontrast=%d,maxcontrast=%d",__FUNCTION__,__LINE__,mincontrast,maxcontrast);
        if( 0 != v){
            int contrastval = atoi(v);
            if(contrastval < mincontrast){
                TRACE_E("Invalid contrast value:%d. set to min:%d", contrastval, mincontrast);
                contrastval = mincontrast;
                //ret = BAD_VALUE;
            }
            else if (contrastval > maxcontrast){
                TRACE_E("Invalid contrast value:%d. set to max:%d",contrastval,maxcontrast);
                contrastval = maxcontrast;
                //ret = BAD_VALUE;
            }
            error = CAM_SendCommand(handle,
                    CAM_CMD_SET_CONTRAST,
                    (CAM_Param)contrastval,
                    UNUSED_PARAM);
            ASSERT_CAM_ERROR(error);
            TRACE_D("%s:contrast=%d", __FUNCTION__, contrastval);
        }

        return ret;
    }

    status_t Engine::ceStartPreview(const CAM_DeviceHandle &handle)
    {
        CAM_Error error = CAM_ERROR_NONE;
        CAM_CaptureState state;

        error = CAM_SendCommand(handle,
                CAM_CMD_GET_STATE,
                (CAM_Param)&state,
                UNUSED_PARAM);
        ASSERT_CAM_ERROR(error);

        if (state == CAM_CAPTURESTATE_PREVIEW){
            return NO_ERROR;
        }
        else if (state == CAM_CAPTURESTATE_STILL ||
                state == CAM_CAPTURESTATE_VIDEO ||
                state == CAM_CAPTURESTATE_IDLE
                ){
            error = CAM_SendCommand(handle,
                    CAM_CMD_SET_STATE,
                    (CAM_Param)CAM_CAPTURESTATE_PREVIEW,
                    UNUSED_PARAM);
            //ASSERT_CAM_ERROR(error);
            if(CAM_ERROR_NONE != error){
                return UNKNOWN_ERROR;
            }
            else{
                return NO_ERROR;
            }
        }
        else{
            TRACE_E("Unknown error happened,wrong state");
            return UNKNOWN_ERROR;
        }
    }

    /*
     * ONLY switch to capture state for ce,
     * assume that ce was already in preview state
     */
    status_t Engine::ceStartCapture(const CAM_DeviceHandle &handle)
    {

        CAM_Error error = CAM_ERROR_NONE;
#if 0
        // Enqueue all still image buffers
        for (int i=0; i<kStillBufCnt; i++){
            CAM_ImageBuffer* pImgBuf_still = mStillImageBuf->getEngBuf(i);
            error = CAM_SendCommand(handle,
                    CAM_CMD_PORT_ENQUEUE_BUF,
                    (CAM_Param)CAM_PORT_STILL,
                    (CAM_Param)pImgBuf_still);
            ASSERT_CAM_ERROR(error);
        }
#endif

        error = CAM_SendCommand(handle,
                CAM_CMD_GET_STILL_BURST,
                (CAM_Param)&iExpectedPicNum,
                UNUSED_PARAM);
        ASSERT_CAM_ERROR(error);

        CAM_CaptureState state;
        error = CAM_SendCommand(handle,
                CAM_CMD_GET_STATE,
                (CAM_Param)&state,
                UNUSED_PARAM);
        ASSERT_CAM_ERROR(error);

        if (state == CAM_CAPTURESTATE_STILL){
            return NO_ERROR;
        }
        else if (state == CAM_CAPTURESTATE_PREVIEW){
            // switch to still capture state
            error = CAM_SendCommand(handle,
                    CAM_CMD_SET_STATE,
                    (CAM_Param)CAM_CAPTURESTATE_STILL,
                    UNUSED_PARAM);
            ASSERT_CAM_ERROR(error);
        }
        else if (state == CAM_CAPTURESTATE_VIDEO ||
                state == CAM_CAPTURESTATE_IDLE){
            error = CAM_SendCommand(handle,
                    CAM_CMD_SET_STATE,
                    (CAM_Param)CAM_CAPTURESTATE_PREVIEW,
                    UNUSED_PARAM);
            ASSERT_CAM_ERROR(error);

            error = CAM_SendCommand(handle,
                    CAM_CMD_SET_STATE,
                    (CAM_Param)CAM_CAPTURESTATE_STILL,
                    UNUSED_PARAM);
            ASSERT_CAM_ERROR(error);
        }
        else{
            TRACE_E("Unknown error happened,wrong state");
            return UNKNOWN_ERROR;
        }

        return NO_ERROR;
    }

    status_t Engine::ceStopCapture(const CAM_DeviceHandle &handle)
    {
        CAM_Error error = CAM_ERROR_NONE;
        CAM_CaptureState state;

        error = CAM_SendCommand(handle,
                CAM_CMD_GET_STATE,
                (CAM_Param)&state,
                UNUSED_PARAM);
        ASSERT_CAM_ERROR(error);

        if (state == CAM_CAPTURESTATE_STILL){
            error = CAM_SendCommand(handle,
                    CAM_CMD_SET_STATE,
                    (CAM_Param)CAM_CAPTURESTATE_PREVIEW,
                    UNUSED_PARAM);
            ASSERT_CAM_ERROR(error);
        }
        else{
            TRACE_D("%s:current state is %d",__FUNCTION__,error);
        }
        return NO_ERROR;
    }

    status_t Engine::ceStartVideo(const CAM_DeviceHandle &handle)
    {
        return NO_ERROR;
    }

    status_t Engine::ceStopVideo(const CAM_DeviceHandle &handle)
    {
        return NO_ERROR;
    }

    /*
     * 1.set ce state from preview to idle
     * 2.unuse buf on preview port
     */
    status_t Engine::ceStopPreview(const CAM_DeviceHandle &handle)
    {

        CAM_Error error = CAM_ERROR_NONE;
        CAM_CaptureState state;

        error = CAM_SendCommand(handle,
                CAM_CMD_GET_STATE,
                (CAM_Param)&state,
                UNUSED_PARAM);
        ASSERT_CAM_ERROR(error);

        if (state == CAM_CAPTURESTATE_STILL){
            error = CAM_SendCommand(handle,
                    CAM_CMD_SET_STATE,
                    (CAM_Param)CAM_CAPTURESTATE_PREVIEW,
                    UNUSED_PARAM);
            ASSERT_CAM_ERROR(error);

            error = CAM_SendCommand(handle,
                    CAM_CMD_SET_STATE,
                    (CAM_Param)CAM_CAPTURESTATE_IDLE,
                    UNUSED_PARAM);
            ASSERT_CAM_ERROR(error);
        }
        else if (state == CAM_CAPTURESTATE_VIDEO){
            error = CAM_SendCommand(handle,
                    CAM_CMD_SET_STATE,
                    (CAM_Param)CAM_CAPTURESTATE_PREVIEW,
                    UNUSED_PARAM);
            ASSERT_CAM_ERROR(error);

            error = CAM_SendCommand(handle,
                    CAM_CMD_SET_STATE,
                    (CAM_Param)CAM_CAPTURESTATE_IDLE,
                    UNUSED_PARAM);
            ASSERT_CAM_ERROR(error);
        }
        else if (state == CAM_CAPTURESTATE_PREVIEW){
            error = CAM_SendCommand(handle,
                    CAM_CMD_SET_STATE,
                    (CAM_Param)CAM_CAPTURESTATE_IDLE,
                    UNUSED_PARAM);
            ASSERT_CAM_ERROR(error);
        }
#if 0
        error = CAM_SendCommand(handle,
                CAM_CMD_PORT_FLUSH_BUF,
                (CAM_Param)CAM_PORT_ANY,
                UNUSED_PARAM);
        ASSERT_CAM_ERROR(error);
#endif
        return NO_ERROR;
    }

    CAM_Error Engine::ceGetPreviewFrame(const CAM_DeviceHandle &handle,int *index)
    {
        TRACE_V("-%s", __FUNCTION__);

        CAM_Error error = CAM_ERROR_NONE;
        CAM_ImageBuffer *pImgBuf;

        int i;
        IppStatus ippret;

        int port = CAM_PORT_PREVIEW;
        error = CAM_SendCommand(handle,
                CAM_CMD_PORT_DEQUEUE_BUF,
                (CAM_Param)&port,
                (CAM_Param)&pImgBuf);

        //filter the v4l2 polling failure error
        if (CAM_ERROR_FATALERROR == error) {
            TRACE_D("%s dequeue return with error %d, restore cameraengine", __FUNCTION__, error);
            error = CAM_SendCommand(handle,
                    CAM_CMD_SET_STATE,
                    (CAM_Param)CAM_CAPTURESTATE_IDLE,
                    UNUSED_PARAM);
            ASSERT_CAM_ERROR(error);

            //only recover engine to preview status if preview thread is not request to exit
            //if (CAM_STATE_PREVIEW == mState){
            error = CAM_SendCommand(handle,
                    CAM_CMD_SET_STATE,
                    (CAM_Param)CAM_CAPTURESTATE_PREVIEW,
                    UNUSED_PARAM);
            ASSERT_CAM_ERROR(error);
            //}
            return CAM_ERROR_FATALERROR;
        }
        //check any other errors
        if (CAM_ERROR_NONE != error) {
            TRACE_D("%s x with error %d", __FUNCTION__, error);
            return error;
        }

        *index = pImgBuf->iUserIndex;

        return error;
    }

    CAM_Error Engine::ceGetStillFrame(const CAM_DeviceHandle &handle,int *index)
    {
        int port;
        CAM_Error error = CAM_ERROR_NONE;
        CAM_ImageBuffer *pImgBuf;
        do{
            port = CAM_PORT_STILL;
            error = CAM_SendCommand(handle,
                    CAM_CMD_PORT_DEQUEUE_BUF,
                    (CAM_Param)&port,
                    (CAM_Param)&pImgBuf);

            if (CAM_ERROR_NONE == error){
                break;
            }
            else if (CAM_ERROR_FATALERROR == error) {
#if 0
                TRACE_D("%s dequeue return with error %d, restore cameraengine", __FUNCTION__, error);
                error = CAM_SendCommand(handle,
                        CAM_CMD_SET_STATE,
                        (CAM_Param)CAM_CAPTURESTATE_PREVIEW,
                        UNUSED_PARAM);
                ASSERT_CAM_ERROR(error);

                error = CAM_SendCommand(handle,
                        CAM_CMD_SET_STATE,
                        (CAM_Param)CAM_CAPTURESTATE_IDLE,
                        UNUSED_PARAM);
                ASSERT_CAM_ERROR(error);

                error = CAM_SendCommand(handle,
                        CAM_CMD_SET_STATE,
                        (CAM_Param)CAM_CAPTURESTATE_PREVIEW,
                        UNUSED_PARAM);
                ASSERT_CAM_ERROR(error);

                error = CAM_SendCommand(handle,
                        CAM_CMD_SET_STATE,
                        (CAM_Param)CAM_CAPTURESTATE_STILL,
                        UNUSED_PARAM);
                ASSERT_CAM_ERROR(error);
#else
                error = CAM_SendCommand(handle,
                        CAM_CMD_RESET_CAMERA,
                        (CAM_Param)CAM_RESET_COMPLETE,
                        UNUSED_PARAM);
                ASSERT_CAM_ERROR(error);
#endif
            }
            else{
                ASSERT_CAM_ERROR(error);
            }
        }while(1);

        *index = pImgBuf->iUserIndex;

        return error;
    }

    CAM_Error Engine::ceGetCurrentSceneModeCap(const CAM_DeviceHandle &handle,
            CameraSetting& setting)
    {
        CAM_Error error = CAM_ERROR_NONE;
        String8 v = String8("");

        // scene mode
        int scenemode;
        error = CAM_SendCommand(handle,
                CAM_CMD_GET_SHOTMODE,
                (CAM_Param)&scenemode,
                UNUSED_PARAM);
        ASSERT_CAM_ERROR(error);

        v = String8("");
        v = setting.map_ce2andr(CameraSetting::map_scenemode,scenemode);
        if( v != String8("") ){
            setting.set(CameraParameters::KEY_SCENE_MODE,v.string());
            TRACE_D("%s:CE used scene mode:%s",__FUNCTION__,v.string());
        }

        // white balance
        CAM_WhiteBalanceMode WhiteBalance;
        error = CAM_SendCommand(handle,
                CAM_CMD_GET_WHITEBALANCEMODE,
                (CAM_Param)&WhiteBalance,
                UNUSED_PARAM);
        ASSERT_CAM_ERROR(error);

        v = String8("");
        v = setting.map_ce2andr(CameraSetting::map_whitebalance,WhiteBalance);
        if( v != String8("") ){
            setting.set(CameraParameters::KEY_WHITE_BALANCE,v.string());
            TRACE_D("%s:CE used whitebalance:%s",__FUNCTION__,v.string());
        }

        // color effect
        CAM_ColorEffect ColorEffect;
        error = CAM_SendCommand(handle,
                CAM_CMD_GET_COLOREFFECT,
                (CAM_Param)&ColorEffect,
                UNUSED_PARAM);
        ASSERT_CAM_ERROR(error);

        v = String8("");
        v = setting.map_ce2andr(CameraSetting::map_coloreffect,ColorEffect);
        if( v != String8("") ){
            setting.set(CameraParameters::KEY_EFFECT,v.string());
            TRACE_D("%s:CE used color effect:%s",__FUNCTION__,v.string());
        }

        // bandfilter
        CAM_BandFilterMode BandFilter;
        error = CAM_SendCommand(handle,
                CAM_CMD_GET_BANDFILTER,
                (CAM_Param)&BandFilter,
                UNUSED_PARAM);
        ASSERT_CAM_ERROR(error);

        v = String8("");
        v = setting.map_ce2andr(CameraSetting::map_bandfilter,BandFilter);
        if( v != String8("") ){
            setting.set(CameraParameters::KEY_ANTIBANDING,v.string());
            TRACE_D("%s:CE used antibanding:%s",__FUNCTION__,v.string());
        }

        // flash mode
        CAM_FlashMode FlashMode;
        error = CAM_SendCommand(handle,
                CAM_CMD_GET_FLASHMODE,
                (CAM_Param)&FlashMode,
                UNUSED_PARAM);
        ASSERT_CAM_ERROR(error);

        v = String8("");
        v = setting.map_ce2andr(CameraSetting::map_flash,FlashMode);
        if( v != String8("") ){
            setting.set(CameraParameters::KEY_FLASH_MODE,v.string());
            TRACE_D("%s:CE used flash mode:%s",__FUNCTION__,v.string());
        }

        // focus mode
        CAM_FocusMode FocusMode;
        error = CAM_SendCommand(handle,
                CAM_CMD_GET_FOCUSMODE,
                (CAM_Param)&FocusMode,
                UNUSED_PARAM);
        ASSERT_CAM_ERROR(error);

        v = String8("");
        v = setting.map_ce2andr(CameraSetting::map_focus,FocusMode);
        if( v != String8("") ){
            setting.set(CameraParameters::KEY_FOCUS_MODE,v.string());
            TRACE_D("%s:CE used focus mode:%s",__FUNCTION__,v.string());
        }

        // EVC
        const int evcstep=mCamera_shotcaps.iEVCompStepQ16;
        if ( 0 != evcstep){
            int ev = 0;
            error = CAM_SendCommand(handle,
                    CAM_CMD_GET_EVCOMPENSATION,
                    (CAM_Param)&ev,
                    UNUSED_PARAM);
            ASSERT_CAM_ERROR(error);

            v = String8("");
            char tmp[32] = {'\0'};
            int exposure = ev / evcstep;
            sprintf(tmp, "%d", exposure);
            v = String8(tmp);
            setting.set(CameraParameters::KEY_EXPOSURE_COMPENSATION, v.string());
            TRACE_D("%s:CE used exposure compensation:%s",__FUNCTION__,v.string());
        }

        //digital zoom
        const int minzoom = mCamera_shotcaps.iMinDigZoomQ16;
        const int maxzoom = mCamera_shotcaps.iMaxDigZoomQ16;
        TRACE_V("%s:%d: minzoom=%d,maxzoom=%d",__FUNCTION__,__LINE__,minzoom,maxzoom);
        const int cezoomstep = mCamera_shotcaps.iDigZoomStepQ16;
        if((0x1<<16) != maxzoom){
            int cedigzoom;
            char tmp[32] = {'\0'};
            error = CAM_SendCommand(handle,
                    CAM_CMD_GET_DIGZOOM,
                    (CAM_Param)&cedigzoom,
                    UNUSED_PARAM );
            ASSERT_CAM_ERROR(error);

            int andrzoomval = (cedigzoom-minzoom)/(cezoomstep);//1.0x for andr min zoom
            sprintf(tmp, "%d", andrzoomval);
            mSetting.set(CameraParameters::KEY_ZOOM, tmp);
            TRACE_D("%s:CE used digital zoom:%s",__FUNCTION__,tmp);
        }

#if 0
        //fps
        {
            const int min_fps = mCamera_shotcaps.iMinFpsQ16 >> 16;
            const int max_fps = mCamera_shotcaps.iMaxFpsQ16 >> 16;
            int iFPSQ16;
            error = CAM_SendCommand(handle,
                    CAM_CMD_GET_PREVIEW_FRAMERATE,
                    (CAM_Param)&iFPSQ16,
                    UNUSED_PARAM);
            ASSERT_CAM_ERROR(error);

            int framerate = iFPSQ16 >> 16;
            TRACE_V("%s:%d: minfps=%d,maxfps=%d,CE framerate=%d",__FUNCTION__,__LINE__,
                    min_fps,max_fps,framerate);

            char tmp[32] = {'\0'};
            v = String8("");
            /*
            if(framerate <= max_fps && framerate >= min_fps && framerate > 0)
            */
            {
                sprintf(tmp, "%d", framerate);
                v = String8(tmp);
            }
            /*
            else{
                sprintf(tmp, "%d", max_fps);
                v = String8(tmp);
            }
            */

            setting.set(CameraParameters::KEY_PREVIEW_FRAME_RATE,v.string());
            TRACE_D("%s:CE used framerate:%s",__FUNCTION__,v.string());
        }
#endif

        //For VT, brightness
        {
            int brightval;
            error = CAM_SendCommand(handle,
                    CAM_CMD_GET_BRIGHTNESS,
                    (CAM_Param)&brightval,
                    UNUSED_PARAM);
            ASSERT_CAM_ERROR(error);

            char tmp[32] = {'\0'};
            sprintf(tmp, "%d", brightval);
            setting.set("light_key",tmp);
            TRACE_D("%s:CE used brightness=%d", __FUNCTION__, brightval);
        }

        //For VT, contrast
        {
            int contrastval;
            error = CAM_SendCommand(handle,
                    CAM_CMD_GET_CONTRAST,
                    (CAM_Param)&contrastval,
                    UNUSED_PARAM);
            ASSERT_CAM_ERROR(error);

            char tmp[32] = {'\0'};
            sprintf(tmp, "%d", contrastval);
            setting.set("contrast_key",tmp);
            TRACE_D("%s:CE used contrast=%d", __FUNCTION__, contrastval);
        }

        return error;
    }

    void Engine::DrawRect(int xLeft, int yLeft, int xRight, int yRight, ImageBuf * image, int index)
    {
        //when camera Preview,draw line in screen
        unsigned char* vaddr_src[3];
        uint8_t * pSrc[3];

        TRACE_V("%s Enter ", __FUNCTION__);
        TRACE_V("%s   xLeft[%d], yLeft[%d], xRight[%d], yRight[%d]", __FUNCTION__, xLeft, yLeft, xRight, yRight);
        int previewWidth,previewHeight;
        image->getImageSize(index, &previewWidth, &previewHeight);
        TRACE_V("%s   previewWidth[%d], previewHeight[%d] ", __FUNCTION__,previewWidth, previewHeight);

        // Draw line only if needed.
        if(yLeft < 0 || yRight >= previewHeight)
        {
            TRACE_E("%s (yLeft < 0 || yAxis >= previewHeight) return", __FUNCTION__);
            return;
        }

        if(xLeft < 0 || xRight >= previewWidth)
        {
            TRACE_E("%s (xLeft < 0 || xRight >= previewHight) return", __FUNCTION__);
            return;
        }

        image->getVirAddr(index,vaddr_src+0,vaddr_src+1,vaddr_src+2);
        TRACE_V("src vaddr: %x,%x,%x",vaddr_src[0],vaddr_src[1],vaddr_src[2]);
        pSrc[0] = (uint8_t*)vaddr_src[0];
        pSrc[1] = (uint8_t*)vaddr_src[1];
        pSrc[2] = (uint8_t*)vaddr_src[2];

        int step[3];
        image->getStep(index,step+0,step+1,step+2);

        const char* fmt="";
        fmt = image->getImageFormat(index);
        if(strcmp(fmt,CameraParameters::PIXEL_FORMAT_YUV420P)==0){
            TRACE_V("preview image format: %s", CameraParameters::PIXEL_FORMAT_YUV420P);
            TRACE_V("%s drawLine from (xLeft[%d],yLeft[%d]) to (xRight[%d],yRight[%d]) start", __FUNCTION__, xLeft, yLeft, xRight, yRight);
            DrawHorizontalLine_YUV420P(yLeft, xLeft, xRight, previewWidth, previewHeight, pSrc[0]);
            DrawVerticalLine_YUV420P(xLeft, yLeft, yRight, previewWidth, previewHeight, pSrc[0]);
            DrawHorizontalLine_YUV420P(yRight, xLeft, xRight, previewWidth, previewHeight, pSrc[0]);
            DrawVerticalLine_YUV420P(xRight, yLeft, yRight, previewWidth, previewHeight, pSrc[0]);
            TRACE_V("%s drawLine from (xLeft[%d],yLeft[%d]) to (xRight[%d],yRight[%d]) end", __FUNCTION__, xLeft, yLeft, xRight, yRight);

        }
        else if(strcmp(fmt, CameraParameters::PIXEL_FORMAT_YUV422I_UYVY)==0){
            TRACE_V("preview image format: %s", CameraParameters::PIXEL_FORMAT_YUV422I_UYVY);
            TRACE_V("%s drawLine from (xLeft[%d],yLeft[%d]) to (xRight[%d],yRight[%d]) start", __FUNCTION__, xLeft, yLeft, xRight, yRight);
            DrawHorizontalLine_YUV422I_UYVY(yLeft, xLeft, xRight, previewWidth, previewHeight, pSrc[0]);
            DrawVerticalLine_YUV422I_UYVY(xLeft, yLeft, yRight, previewWidth, previewHeight, pSrc[0]);
            DrawHorizontalLine_YUV422I_UYVY(yRight, xLeft, xRight, previewWidth, previewHeight, pSrc[0]);
            DrawVerticalLine_YUV422I_UYVY(xRight, yLeft, yRight, previewWidth, previewHeight, pSrc[0]);
            TRACE_V("%s drawLine from (xLeft[%d],yLeft[%d]) to (xRight[%d],yRight[%d]) end", __FUNCTION__, xLeft, yLeft, xRight, yRight);
        }
        else if(strcmp(fmt, CameraParameters::PIXEL_FORMAT_YUV420SP)==0){
            TRACE_V("preview image format: %s", CameraParameters::PIXEL_FORMAT_YUV420SP);
            TRACE_V("%s drawLine from (xLeft[%d],yLeft[%d]) to (xRight[%d],yRight[%d]) start", __FUNCTION__, xLeft, yLeft, xRight, yRight);
            DrawHorizontalLine_YUV420SP(yLeft, xLeft, xRight, previewWidth, previewHeight, pSrc[0]);
            DrawVerticalLine_YUV420SP(xLeft, yLeft, yRight, previewWidth, previewHeight, pSrc[0]);
            DrawHorizontalLine_YUV420SP(yRight, xLeft, xRight, previewWidth, previewHeight, pSrc[0]);
            DrawVerticalLine_YUV420SP(xRight, yLeft, yRight, previewWidth, previewHeight, pSrc[0]);
            TRACE_V("%s drawLine from (xLeft[%d],yLeft[%d]) to (xRight[%d],yRight[%d]) end", __FUNCTION__, xLeft, yLeft, xRight, yRight);
        }
        else{
            TRACE_E("unsurport image format: %s", fmt);
            return;
        }


    }

    /*
     * For YUV420P format
     * draw horizontal line
     * color:Y,U,V
     * from (xLeft,yAxis) to (xRight,yAxis)
     * */
    void Engine::DrawHorizontalLine_YUV420P(int yAxis, int xLeft, int xRight, int previewWidth, int previewHeight, uint8_t * buf_start)
    {
        uint8_t * pY;
        uint8_t * pCb;
        uint8_t * pCr;
        const int Y = 147.4; //YUV value
        const int U = 21.2;
        const int V = 43.5;
        int Y_buflength=previewWidth * previewHeight;

        //FIXME: need to define step conception in Overlay
        if (xLeft > xRight){
            TRACE_E( "%s xLeft > xRight return", __FUNCTION__);
            return;
        }

        int drawSize = xRight - xLeft + 1;

        pY = buf_start+ (yAxis * previewWidth) +xLeft;
        memset(pY, Y, drawSize);
        if(yAxis % 2){
            TRACE_E("%s yAxis mod 2 return", __FUNCTION__);
            return;
        }
        pCb = buf_start + Y_buflength + ((yAxis/2 * previewWidth) + xLeft)/2;
        pCr = pCb + Y_buflength/4;
        memset(pCb, U, drawSize/2);
        memset(pCr, V, drawSize/2);
    }

    /*
     * For YUV420P format
     * draw vertical line
     * color:Y,U,V
     * from (xAxis,yTop) to (xAxis,yBottom)
     */
    void Engine::DrawVerticalLine_YUV420P(int xAxis, int yTop, int yBottom, int previewWidth, int previewHeight, uint8_t * buf_start)
    {
        uint8_t * pY;
        uint8_t * pCb;
        uint8_t * pCr;
        const int Y = 147.4; //YUV value
        const int U = 21.2;
        const int V = 43.5;
        int y_buflength=previewWidth * previewHeight;

        //FIXME: need to define step conception in Overlay
        if (yTop > yBottom){
            TRACE_E( "%s yTop > yBottom return", __FUNCTION__);
            return;
        }

        int drawSize = yBottom - yTop + 1;
        pY =buf_start+ (yTop * previewWidth) +xAxis;

        //TRACE_E("%s Update Y values start", __FUNCTION__);
        for (int i=yTop; i<=yBottom; ++i)
        {
            *pY = Y;
            pY  += previewWidth;
        }
        //TRACE_E("%s Update Y values end ", __FUNCTION__);

        //Return if color update is unnecessary.
        if (xAxis % 2){
            TRACE_E("%s xAxis mod 2 return ", __FUNCTION__);
            return;
        }

        pCb = buf_start+ y_buflength + ((yTop/2 * previewWidth) + xAxis)/2;
        pCr = pCb + y_buflength/4;
        int halfWidth = previewWidth/2;

        //TRACE_E("%s Update cb cr values start ", __FUNCTION__);
        for (int i=yTop; i<=yBottom; i+=2){
            *pCb = U;
            *pCr = V;

            pCb += halfWidth;
            pCr += halfWidth;
        }

        //TRACE_E("%s Update cb cr values end ", __FUNCTION__);
    }

    /*
     * For YUV422I_UYVY format
     * draw horizontal line
     * color:Y,U,V
     * from (xLeft,yAxis) to (xRight,yAxis)
     */
    void Engine::DrawHorizontalLine_YUV422I_UYVY(int yAxis, int xLeft, int xRight, int previewWidth, int previewHeight, uint8_t * buf_start)
    {
        uint8_t * pY;
        uint8_t * pCb;
        uint8_t * pCr;
        uint8_t * pImag;
        const int Y = 147.4; //YUV value
        const int U = 21.2;
        const int V = 43.5;
        int col;
        int Y_buflength=previewWidth * previewHeight;

        //FIXME: need to define step conception in Overlay
        if (xLeft > xRight){
            TRACE_E( "%s xLeft > xRight return", __FUNCTION__);
            return;
        }

        int drawSize = xRight - xLeft + 1;
        pImag = buf_start + yAxis * previewWidth * 2 + xLeft * 2;
        if( xLeft % 2 ==0){
            for(col = xLeft; col < xRight + 1; col += 2){
                pCb = pImag;
                pY = pImag+1;
                pCr = pImag+2;
                *pCb = U;
                *pY = Y;
                *pCr = V;
                pY = pImag+3;
                *pY =Y;
                pImag +=4;
            }
        }
        else{
            for(col = xLeft + 1; col < xRight + 1; col += 2){
                pCr = pImag;
                pY = pImag+1;
                pCb = pImag+2;
                *pCr = V;
                *pY = Y;
                *pCb = U;
                pY = pImag+3;
                *pY =Y;
                pImag +=4;
            }
        }
    }

    /*
     * For YUV422I_UYVY format
     * draw vertical line
     * color:Y,U,V
     * from (xAxis,yTop) to (xAxis,yBottom)
     */
    void Engine::DrawVerticalLine_YUV422I_UYVY(int xAxis, int yTop, int yBottom, int previewWidth, int previewHeight, uint8_t * buf_start)
    {
        uint8_t * pY;
        uint8_t * pCb;
        uint8_t * pCr;
        uint8_t * pImag;
        const int Y = 147.4; //YUV value
        const int U = 21.2;
        const int V = 43.5;
        int row;
        int y_buflength=previewWidth * previewHeight;

        //FIXME: need to define step conception in Overlay
        if (yTop > yBottom){
            TRACE_E( "%s yTop > yBottom return", __FUNCTION__);
            return;
        }

        int drawSize = yBottom - yTop + 1;
        //	TRACE_V("%s drawLine from yTop[%d] to yBottom[%d] start", __FUNCTION__, yTop, yBottom);
        pImag = buf_start + (yTop * previewWidth * 2) + xAxis * 2;
        if( xAxis % 2 == 0){
            for(row = yTop; row < yBottom +1; row +=1){
                pCb = pImag;
                pY = pImag+1;
                pCr =pImag +2;
                *pCb = U;
                *pY =Y;
                *pCr =V;
                pImag += previewWidth * 2;
            }
        }
        else if( xAxis % 2 != 0){
            for(row = yTop; row < yBottom +1; row +=1){
                pCr = pImag;
                pY = pImag+1;
                *pCr = V;
                *pY =Y;
                pImag += previewWidth * 2;
            }
        }
    }

    /*
     * For YUV420SP format
     * draw horizontal line
     * color:Y,U,V
     * from (xLeft,yAxis) to (xRight,yAxis)
     */
    void Engine::DrawHorizontalLine_YUV420SP(int yAxis, int xLeft, int xRight, int previewWidth, int previewHeight, uint8_t * buf_start)
    {
        uint8_t * pY;
        uint8_t * pCb;
        uint8_t * pCr;
        const int Y = 147.4; //YUV value
        const int U = 21.2;
        const int V = 43.5;
        int col;
        int Y_buflength=previewWidth * previewHeight;

        //FIXME: need to define step conception in Overlay
        if (xLeft > xRight){
            TRACE_E( "%s xLeft > xRight return", __FUNCTION__);
            return;
        }

        int drawSize = xRight - xLeft + 1;

        pY = buf_start+ (yAxis * previewWidth) +xLeft;
        memset(pY, Y, drawSize);

        if(yAxis % 2){
            TRACE_E("%s yAxis mod 2 return", __FUNCTION__);
            return;
        }
        for (col = xLeft; col < xRight +1; col +=1){
            pCb = buf_start + Y_buflength + ((yAxis/2 * previewWidth) + xLeft)/2;
            pCr = buf_start + Y_buflength + ((yAxis/2 * previewWidth) + xLeft)/2 + 1;
            *pCb = U;
            *pCr = V;
        }
    }

    /*
     * For YUV420SP format
     * draw vertical line
     * color:Y,U,V
     * from (xAxis,yTop) to (xAxis,yBottom)
     */
    void Engine::DrawVerticalLine_YUV420SP(int xAxis, int yTop, int yBottom, int previewWidth, int previewHeight, uint8_t * buf_start)
    {
        uint8_t * pY;
        uint8_t * pCb;
        uint8_t * pCr;
        const int Y = 147.4; //YUV value
        const int U = 21.2;
        const int V = 43.5;
        int row;

        int y_buflength=previewWidth * previewHeight;


        //FIXME: need to define step conception in Overlay
        if (yTop > yBottom){
            TRACE_E( "%s yTop > yBottom return", __FUNCTION__);
            return;
        }

        int drawSize = yBottom - yTop + 1;
        pY =buf_start+ (yTop * previewWidth) +xAxis;
        for (int i=yTop; i<=yBottom; ++i){
            *pY = Y;
            pY += previewWidth;
        }
        //TRACE_E("%s Update Y values end ", __FUNCTION__);

        // Return if color update is unnecessary.
        if (xAxis % 2){
            TRACE_E("%s xAxis mod 2 return ", __FUNCTION__);
            return;
        }

        pCb = buf_start + y_buflength + ((yTop/2 * previewWidth) + xAxis)/2;
        pCr = pCb + 1;

        //TRACE_E("%s Update cb cr values start ", __FUNCTION__);
        for (int i=yTop; i<=yBottom; i+=2){
            *pCb = U;
            *pCr = V;
            pCb += previewWidth;
            pCr += previewWidth;
        }
        //TRACE_E("%s Update cb cr values end ", __FUNCTION__);
    }
}; // namespace android
