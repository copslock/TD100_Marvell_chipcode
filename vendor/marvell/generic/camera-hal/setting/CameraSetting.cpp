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

#include "cam_log_mrvl.h"
#include "CameraSetting.h"
#include "ippExif.h"
#define LOG_TAG "CameraSetting"

namespace android {

    //iSensorID in this table should be firstly initialized
    //in CameraHardware::getNumberOfCameras
    /*
     * MrvlCameraInfo(const char* sensorname,int facing,int orient,
     * default preview width,height,format,
     * video format,
     * defalut picture width,height,format,
     * default preview/still/video buffer count)
     */

#if defined( DKB )
    MrvlCameraInfo CameraSetting::mMrvlCameraInfo[]={
        MrvlCameraInfo("ov5642",CAMERA_FACING_BACK,0,
                720,480,CameraSetting::PIXEL_FORMAT_YUV420SP,
                CameraSetting::PIXEL_FORMAT_YUV420P,
                2048, 1536,CameraParameters::PIXEL_FORMAT_JPEG,
                8,3,6
                ),
	MrvlCameraInfo("ov5640",CAMERA_FACING_BACK,90,
                720,480,CameraSetting::PIXEL_FORMAT_YUV420SP,
                CameraSetting::PIXEL_FORMAT_YUV420P,
                2048, 1536,CameraParameters::PIXEL_FORMAT_JPEG,
                8,3,6
                ),
	MrvlCameraInfo("mt9v113",CAMERA_FACING_FRONT,0,
                640,480,CameraSetting::PIXEL_FORMAT_YUV420SP,
                 CameraSetting::PIXEL_FORMAT_YUV420SP,
                640, 480,CameraParameters::PIXEL_FORMAT_JPEG,
                8,3,6
                ),

        MrvlCameraInfo("ov3640",CAMERA_FACING_FRONT,90,
                640,480,CameraSetting::PIXEL_FORMAT_YUV420SP,
                CameraSetting::PIXEL_FORMAT_YUV420P,
                2048, 1536,CameraParameters::PIXEL_FORMAT_JPEG,
                8,3,6
                ),
        MrvlCameraInfo("BasicSensor",CAMERA_FACING_BACK,90,
                640,480,CameraSetting::PIXEL_FORMAT_YUV420SP,
                CameraSetting::PIXEL_FORMAT_YUV420P,
                2048, 1536,CameraParameters::PIXEL_FORMAT_JPEG,
                8,3,6
                ),
    };
    const char* CameraSetting::mProductName="dkb";

#elif defined ( SAARBMG1 )
    MrvlCameraInfo CameraSetting::mMrvlCameraInfo[]={
        MrvlCameraInfo("ov5642",CAMERA_FACING_BACK,180,
                720,480,CameraSetting::PIXEL_FORMAT_YUV420SP,
                CameraSetting::PIXEL_FORMAT_YUV422I_UYVY,
                2048, 1536,CameraParameters::PIXEL_FORMAT_JPEG,
                8,3,6
                ),
        MrvlCameraInfo("ov3640",CAMERA_FACING_FRONT,90,
                640,480,CameraSetting::PIXEL_FORMAT_YUV420SP,
                CameraSetting::PIXEL_FORMAT_YUV422I_UYVY,
                2048, 1536,CameraParameters::PIXEL_FORMAT_JPEG,
                8,3,6
                ),
        MrvlCameraInfo("BasicSensor",CAMERA_FACING_FRONT,90,
                640,480,CameraSetting::PIXEL_FORMAT_YUV420SP,
                CameraSetting::PIXEL_FORMAT_YUV422I_UYVY,
                2048, 1536,CameraParameters::PIXEL_FORMAT_JPEG,
                8,3,6
                ),
    };
    const char* CameraSetting::mProductName="saarbmg1";

#elif defined ( BROWNSTONE )
    MrvlCameraInfo CameraSetting::mMrvlCameraInfo[]={
        MrvlCameraInfo("ov5642",CAMERA_FACING_BACK,0,
                720,480,CameraSetting::PIXEL_FORMAT_YUV420SP,
                CameraSetting::PIXEL_FORMAT_YUV422I_UYVY,
                2048, 1536,CameraParameters::PIXEL_FORMAT_JPEG,
                10,3,6
                ),
        MrvlCameraInfo("ov3640",CAMERA_FACING_FRONT,90,
                640,480,CameraSetting::PIXEL_FORMAT_YUV420SP,
                CameraSetting::PIXEL_FORMAT_YUV422I_UYVY,
                2048, 1536,CameraParameters::PIXEL_FORMAT_JPEG,
                10,3,6
                ),
        MrvlCameraInfo("BasicSensor",CAMERA_FACING_FRONT,90,
                640,480,CameraSetting::PIXEL_FORMAT_YUV420SP,
                CameraSetting::PIXEL_FORMAT_YUV422I_UYVY,
                2048, 1536,CameraParameters::PIXEL_FORMAT_JPEG,
                10,3,6
                ),
    };
    const char* CameraSetting::mProductName="brownstone";

#elif defined ( ABILENE )
    MrvlCameraInfo CameraSetting::mMrvlCameraInfo[]={
        MrvlCameraInfo("ov5642",CAMERA_FACING_BACK,0,
                720,480,CameraSetting::PIXEL_FORMAT_YUV420SP,
                CameraSetting::PIXEL_FORMAT_YUV422I_UYVY,
                2048, 1536,CameraParameters::PIXEL_FORMAT_JPEG,
                10,3,6
                ),
        MrvlCameraInfo("ov3640",CAMERA_FACING_FRONT,90,
                640,480,CameraSetting::PIXEL_FORMAT_YUV420SP,
                CameraSetting::PIXEL_FORMAT_YUV422I_UYVY,
                2048, 1536,CameraParameters::PIXEL_FORMAT_JPEG,
                10,3,6
                ),
        MrvlCameraInfo("BasicSensor",CAMERA_FACING_FRONT,90,
                640,480,CameraSetting::PIXEL_FORMAT_YUV420SP,
                CameraSetting::PIXEL_FORMAT_YUV422I_UYVY,
                2048, 1536,CameraParameters::PIXEL_FORMAT_JPEG,
                10,3,6
                ),
    };
    const char* CameraSetting::mProductName="abilene";
#else
#error no platform setting available!
#endif

    int CameraSetting::iNumOfSensors=0;

    const CameraSetting::map_ce_andr CameraSetting::map_imgfmt[]={
        /* filter out yuv422p, for no support of it in display componet.
         *  {String8(CameraSetting::PIXEL_FORMAT_YUV422P),
         *  CAM_IMGFMT_YCbCr422P},
         */
        {String8(CameraSetting::PIXEL_FORMAT_YUV420P),
            CAM_IMGFMT_YCbCr420P},
        {String8(CameraSetting::PIXEL_FORMAT_YUV422I_UYVY),
            CAM_IMGFMT_CbYCrY},
        {String8(CameraSetting::PIXEL_FORMAT_YUV420SP),
            CAM_IMGFMT_YCrCb420SP},
        {String8(CameraSetting::PIXEL_FORMAT_RGB565),
            CAM_IMGFMT_BGR565},
        {String8(""),
            CAM_IMGFMT_YCrYCb},
        {String8(""),
            CAM_IMGFMT_CrYCbY},
        {String8(CameraParameters::PIXEL_FORMAT_JPEG),
            CAM_IMGFMT_JPEG},
        {String8(""),
            -1},	//end flag
    };

    const CameraSetting::map_ce_andr CameraSetting::map_whitebalance[]={
        {String8(CameraParameters::WHITE_BALANCE_AUTO),
            CAM_WHITEBALANCEMODE_AUTO},
        {String8(CameraParameters::WHITE_BALANCE_INCANDESCENT),
            CAM_WHITEBALANCEMODE_INCANDESCENT},
        {String8(CameraParameters::WHITE_BALANCE_FLUORESCENT),
            CAM_WHITEBALANCEMODE_COOLWHITE_FLUORESCENT},
        {String8(CameraParameters::WHITE_BALANCE_WARM_FLUORESCENT),
            CAM_WHITEBALANCEMODE_DAYWHITE_FLUORESCENT},
        {String8(""),
            CAM_WHITEBALANCEMODE_DAYLIGHT_FLUORESCENT},
        {String8(CameraParameters::WHITE_BALANCE_DAYLIGHT),
            CAM_WHITEBALANCEMODE_DAYLIGHT},
        {String8(CameraParameters::WHITE_BALANCE_CLOUDY_DAYLIGHT),
            CAM_WHITEBALANCEMODE_CLOUDY},
        {String8(CameraParameters::WHITE_BALANCE_SHADE),
            CAM_WHITEBALANCEMODE_SHADOW},
        {String8(CameraParameters::WHITE_BALANCE_TWILIGHT),
            -1},
        {String8(""),
            -1},	//end flag
    };

    const CameraSetting::map_ce_andr CameraSetting::map_coloreffect[]={
        {String8(CameraParameters::EFFECT_NONE),
            CAM_COLOREFFECT_OFF},
        {String8(""),
            CAM_COLOREFFECT_VIVID},
        {String8(CameraParameters::EFFECT_SEPIA),
            CAM_COLOREFFECT_SEPIA},
        {String8(CameraParameters::EFFECT_MONO),
            CAM_COLOREFFECT_GRAYSCALE},
        {String8(CameraParameters::EFFECT_NEGATIVE),
            CAM_COLOREFFECT_NEGATIVE},
        {String8(CameraParameters::EFFECT_AQUA),
            -1},
        {String8(CameraParameters::EFFECT_POSTERIZE),
            -1},
        {String8(CameraParameters::EFFECT_SOLARIZE),
            CAM_COLOREFFECT_SOLARIZE},
        {String8(CameraParameters::EFFECT_BLACKBOARD),
            -1},
        {String8(CameraParameters::EFFECT_WHITEBOARD),
            -1},
        {String8(""),
            -1},	//end flag
    };

    const CameraSetting::map_ce_andr CameraSetting::map_bandfilter[]={
        {String8(CameraParameters::ANTIBANDING_AUTO),
            CAM_BANDFILTER_AUTO},
        {String8(CameraParameters::ANTIBANDING_OFF),
            CAM_BANDFILTER_OFF},
        {String8(CameraParameters::ANTIBANDING_50HZ),
            CAM_BANDFILTER_50HZ},
        {String8(CameraParameters::ANTIBANDING_60HZ),
            CAM_BANDFILTER_60HZ},
        {String8(""),
            -1},	//end flag
    };

    const CameraSetting::map_ce_andr CameraSetting::map_flash[]={
        {String8(CameraParameters::FLASH_MODE_AUTO),
            CAM_FLASH_AUTO},
        {String8(CameraParameters::FLASH_MODE_OFF),
            CAM_FLASH_OFF},
        {String8(CameraParameters::FLASH_MODE_ON),
            CAM_FLASH_ON},
        {String8(CameraParameters::FLASH_MODE_TORCH),
            CAM_FLASH_TORCH},
        {String8(CameraParameters::FLASH_MODE_RED_EYE),
            -1},
        {String8(""),
            -1},	//end flag
    };

    const CameraSetting::map_ce_andr CameraSetting::map_focus[]={
        {String8(CameraParameters::FOCUS_MODE_CONTINUOUS_VIDEO),
            CAM_FOCUS_AUTO_CONTINUOUS_FACE},
        {String8(CameraParameters::FOCUS_MODE_AUTO),
            CAM_FOCUS_AUTO_ONESHOT_CENTER},
        {String8(CameraParameters::FOCUS_MODE_MACRO),
            CAM_FOCUS_MACRO},
        {String8(""),
            CAM_FOCUS_HYPERFOCAL},
        {String8(CameraParameters::FOCUS_MODE_INFINITY),
            CAM_FOCUS_INFINITY},
        {String8(""),
            CAM_FOCUS_MANUAL},
        {String8(""),
            -1},	//end flag
    };

    const CameraSetting::map_ce_andr CameraSetting::map_scenemode[]={
        {String8(CameraParameters::SCENE_MODE_PORTRAIT),
            CAM_SHOTMODE_PORTRAIT},
        {String8(CameraParameters::SCENE_MODE_LANDSCAPE),
            CAM_SHOTMODE_LANDSCAPE},
        {String8(CameraParameters::SCENE_MODE_NIGHT_PORTRAIT),
            CAM_SHOTMODE_NIGHTPORTRAIT},
        {String8(CameraParameters::SCENE_MODE_NIGHT),
            CAM_SHOTMODE_NIGHTSCENE},
        {String8(""),
            CAM_SHOTMODE_CHILD},
        {String8(""),
            CAM_SHOTMODE_INDOOR},
        {String8(""),
            CAM_SHOTMODE_PLANTS},
        {String8(CameraParameters::SCENE_MODE_SNOW),
            CAM_SHOTMODE_SNOW},
        {String8(CameraParameters::SCENE_MODE_FIREWORKS),
            CAM_SHOTMODE_FIREWORKS},
        {String8(""),
            CAM_SHOTMODE_SUBMARINE},
        {String8(CameraParameters::SCENE_MODE_ACTION),
            -1},
        {String8(CameraParameters::SCENE_MODE_AUTO),
            CAM_SHOTMODE_AUTO},
        {String8(CameraParameters::SCENE_MODE_BARCODE),
            -1},
        {String8(CameraParameters::SCENE_MODE_CANDLELIGHT),
            -1},
        {String8(CameraParameters::SCENE_MODE_STEADYPHOTO),
            -1},
        {String8(CameraParameters::SCENE_MODE_THEATRE),
            -1},
        {String8(CameraParameters::SCENE_MODE_SUNSET),
            -1},
        {String8(CameraParameters::SCENE_MODE_SPORTS),
            CAM_SHOTMODE_SPORTS},
        /*
           {String8(CameraParameters::SCENE_MODE_MANUAL),
           CAM_SHOTMODE_MANUAL},//FIXME
           */
        {String8(""),
            -1},	//end flag
    };

    const int CameraSetting::AndrZoomRatio=100;//The zoom ratio is in 1/100 increments.

    //IppExifOrientation
    // The rotation angle in degrees relative to the orientation of the camera.
    // Rotation can only be 0, 90, 180 or 270.
    const CameraSetting::map_ce_andr CameraSetting::map_exifrotation[]={
        {String8("0"),
            ippExifOrientationNormal},
        {String8("90"),
            ippExifOrientation90L},
        {String8("180"),
            ippExifOrientation180},
        {String8("270"),
            ippExifOrientation90R},
        {String8(""),
            ippExifOrientationVFLIP},
        {String8(""),
            ippExifOrientationHFLIP},
        {String8(""),
            ippExifOrientationADFLIP},
        {String8(""),
            ippExifOrientationDFLIP},
        {String8(""),
            -1},	//end flag
    };

    //IppExifWhiteBalance
    const CameraSetting::map_ce_andr CameraSetting::map_exifwhitebalance[]={
        {String8(CameraParameters::WHITE_BALANCE_AUTO),
            ippExifWhiteBalanceAuto},
        {String8(CameraParameters::WHITE_BALANCE_INCANDESCENT),
            ippExifWhiteBalanceManual},
        {String8(CameraParameters::WHITE_BALANCE_FLUORESCENT),
            ippExifWhiteBalanceManual},
        {String8(CameraParameters::WHITE_BALANCE_WARM_FLUORESCENT),
            ippExifWhiteBalanceManual},
        {String8(CameraParameters::WHITE_BALANCE_DAYLIGHT),
            ippExifWhiteBalanceManual},
        {String8(CameraParameters::WHITE_BALANCE_CLOUDY_DAYLIGHT),
            ippExifWhiteBalanceManual},
        {String8(CameraParameters::WHITE_BALANCE_TWILIGHT),
            ippExifWhiteBalanceManual},
        {String8(CameraParameters::WHITE_BALANCE_SHADE),
            ippExifWhiteBalanceManual},
        {String8(""),
            -1},	//end flag
    };

    static String8 size_to_str(int width,int height){
        String8 v = String8("");
        char val1[10];
        char val2[10];
        sprintf(val1,"%d", width);
        sprintf(val2,"%d", height);
        v = String8(val1)+
            String8("x")+
            String8(val2);
        return v;
    };

    CameraSetting::CameraSetting(const char* sensorname)
        : mCamera_caps(),
        mCamera_shotcaps()
    {
        int max_camera = sizeof(mMrvlCameraInfo)/sizeof(mMrvlCameraInfo[0]);
        int i;
        for(i=0; i<max_camera; i++){
            const char* detected_sensor = mMrvlCameraInfo[i].getSensorName();
            int detected_sensorid = mMrvlCameraInfo[i].getSensorID();
            //sensor detected and name matched
            if(-1 != detected_sensorid &&
                    0 == strncmp(detected_sensor,sensorname,strlen(detected_sensor))){
                pMrvlCameraInfo = &mMrvlCameraInfo[i];
                break;
            }
        }
        if(max_camera == i){
            TRACE_E("No matching sensor in list. Pls check mMrvlCameraInfo[]");
            pMrvlCameraInfo = NULL;
            return;
        }
        initDefaultParameters();

    }

    CameraSetting::CameraSetting(int sensorid)
        : mCamera_caps(),
        mCamera_shotcaps()
    {
        int max_camera = sizeof(mMrvlCameraInfo)/sizeof(mMrvlCameraInfo[0]);
        int i;
        for(i=0; i<max_camera; i++){
            int detected_sensor = mMrvlCameraInfo[i].getSensorID();
            //sensor detected and id matched
            if(detected_sensor != -1 &&
                    sensorid == detected_sensor){
                pMrvlCameraInfo = &mMrvlCameraInfo[i];
                break;
            }
        }
        if(max_camera == i){
            TRACE_E("No matching sensor in list. Pls check mMrvlCameraInfo[]");
            pMrvlCameraInfo = NULL;
            return;
        }
        initDefaultParameters();
    }

    status_t CameraSetting::initDefaultParameters()
    {
        // (refer to CameraParameters.h)
        int width = pMrvlCameraInfo->def_preview_width;
        int height = pMrvlCameraInfo->def_preview_height;
        String8 v = size_to_str(width, height);
        set(CameraParameters::KEY_SUPPORTED_PREVIEW_SIZES, v.string());
        setPreviewSize(width, height);

        const char *fmt = pMrvlCameraInfo->def_preview_fmt;
        set(CameraParameters::KEY_SUPPORTED_PREVIEW_FORMATS,fmt);
        setPreviewFormat(fmt);

        //NOTES:the internal variable "def_video_fmt" is preferred to be used as the preview format
        //for performance consideration, though default keys "KEY_PREVIEW_FORMATS/KEY_VIDEO_FRAME_FORMAT"
        //may be initialized here to YUV420SP/NV21 for Android API requirements.
        set(CameraParameters::KEY_VIDEO_FRAME_FORMAT,fmt);

        width = pMrvlCameraInfo->def_pic_width;
        height = pMrvlCameraInfo->def_pic_height;
        v = size_to_str(width, height);
        set(CameraParameters::KEY_SUPPORTED_PICTURE_SIZES, v.string());
        setPictureSize(width, height);

        fmt = pMrvlCameraInfo->def_pic_fmt;
        set(CameraParameters::KEY_SUPPORTED_PICTURE_FORMATS,fmt);
        setPictureFormat(fmt);

        /*
         * the following parameters are not supported,
         * return fake values for CTS tests
         */
        set(CameraParameters::KEY_ZOOM_RATIOS, "100");
        set(CameraParameters::KEY_ZOOM, "0");
        set(CameraParameters::KEY_MAX_ZOOM, "0");
        set(CameraParameters::KEY_ZOOM_SUPPORTED, "false");
        set(CameraParameters::KEY_SMOOTH_ZOOM_SUPPORTED, "false");

        set(CameraParameters::KEY_FOCAL_LENGTH, "0.1");
        set(CameraParameters::KEY_HORIZONTAL_VIEW_ANGLE, "50");
        set(CameraParameters::KEY_VERTICAL_VIEW_ANGLE, "50");
        set(CameraParameters::KEY_FOCUS_DISTANCES, "0.05,2,Infinity");

        set(CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH, "160");
        set(CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT, "120");
        set(CameraParameters::KEY_SUPPORTED_JPEG_THUMBNAIL_SIZES, "160x120,0x0");
        set(CameraParameters::KEY_JPEG_THUMBNAIL_QUALITY, "80");

        set(CameraParameters::KEY_PREVIEW_FPS_RANGE, "10000,30000");
        set(CameraParameters::KEY_SUPPORTED_PREVIEW_FPS_RANGE, "(10000,30000)");

        set(CameraParameters::KEY_PREVIEW_FRAME_RATE, "30");
        set(CameraParameters::KEY_SUPPORTED_PREVIEW_FRAME_RATES, "15,30");

        return NO_ERROR;
    }

    status_t CameraSetting::setBasicCaps(const CAM_CameraCapability& camera_caps)
    {
        mCamera_caps = camera_caps;
        String8 v = String8("");
        int width = 0;
        int height = 0;
        int width_min = 0;
        int height_min = 0;

        //preview port negotiation
        if(mCamera_caps.stPortCapability[CAM_PORT_PREVIEW].bArbitrarySize){
            if(mCamera_caps.stPortCapability[CAM_PORT_PREVIEW].stMin.iWidth < 320 &&
                    mCamera_caps.stPortCapability[CAM_PORT_PREVIEW].stMin.iHeight < 240 &&
                    mCamera_caps.stPortCapability[CAM_PORT_PREVIEW].stMax.iWidth > 640 &&
                    mCamera_caps.stPortCapability[CAM_PORT_PREVIEW].stMax.iHeight > 480){
                set(CameraParameters::KEY_SUPPORTED_PREVIEW_SIZES,"640x480,320x240");
            }
        }
        else{
            char val1[10];
            char val2[10];
            v = String8("");
            width_min = 0;
            height_min = 0;
            for(int i=0; i<mCamera_caps.stPortCapability[CAM_PORT_PREVIEW].iSupportedSizeCnt; i++){
                width = mCamera_caps.stPortCapability[CAM_PORT_PREVIEW].stSupportedSize[i].iWidth;
                height = mCamera_caps.stPortCapability[CAM_PORT_PREVIEW].stSupportedSize[i].iHeight;
                if( width_min <= 0 || height_min <= 0 ||
                        width < width_min){
                    width_min = width;
                    height_min = height;
                }
                sprintf(val1,"%d", width);
                sprintf(val2,"%d", height);
                v += String8(val1)+
                    String8("x")+
                    String8(val2);
                /*
                   v = String8(itoa(mCamera_caps.stPortCapability[CAM_PORT_PREVIEW].stSupportedSize[i].iWidth))+
                   String8("x")+
                   String8(itoa(mCamera_caps.stPortCapability[CAM_PORT_PREVIEW].stSupportedSize[i].iHeight));
                   */
                if(i != (mCamera_caps.stPortCapability[CAM_PORT_PREVIEW].iSupportedSizeCnt-1))
                    v += String8(",");
            }
            set(CameraParameters::KEY_SUPPORTED_PREVIEW_SIZES,v);
            if( width_min > 0 && height_min > 0 ){
                setPreviewSize(width_min,height_min);
            }
        }
        TRACE_D("%s:CE supported preview size:%s",__FUNCTION__,
                get(CameraParameters::KEY_SUPPORTED_PREVIEW_SIZES));
        TRACE_D("%s:default preview size:%dx%d",__FUNCTION__,width_min,height_min);

        /*
         * NOTES:we may only support some special formats on current platform
         * here list all supported parameters
         */
        v = String8("");
        for(int i=0; i<mCamera_caps.stPortCapability[CAM_PORT_PREVIEW].iSupportedFormatCnt; i++){
            int ce_idx = mCamera_caps.stPortCapability[CAM_PORT_PREVIEW].eSupportedFormat[i];
            String8 ret = map_ce2andr(map_imgfmt,ce_idx);
            if (ret == String8(""))
                continue;

            //filter out the preferred preview format and put it HEAD.
            if( v == String8("") ){
                v += ret;
                continue;
            }
            if (ret == String8(pMrvlCameraInfo->def_video_fmt)){
                v = ret + String8(",") + v;
            }
            else{
                v += String8(",") + ret;
            }
        }
        if(v == String8("")){
            set(CameraParameters::KEY_SUPPORTED_PREVIEW_FORMATS,CameraSetting::KEY_PREVIEW_FORMAT);
            TRACE_E("CE support NO preview format, use default:%s",CameraSetting::KEY_PREVIEW_FORMAT);
        }
        else{
            set(CameraParameters::KEY_SUPPORTED_PREVIEW_FORMATS,v.string());
            TRACE_D("%s:CE supported preview format:%s",__FUNCTION__,v.string());
            TRACE_D("%s:default preview format:%s",__FUNCTION__,get(CameraParameters::KEY_PREVIEW_FORMAT));
        }

#ifdef DEBUG_BASE_RGB565
        {
            set(CameraParameters::KEY_SUPPORTED_PREVIEW_FORMATS,CameraSetting::PIXEL_FORMAT_YUV420P);
            setPreviewFormat(CameraSetting::PIXEL_FORMAT_YUV420P);
            TRACE_D("%s:DEBUG_BASE_RGB565 enabled! Use preview format:%s",__FUNCTION__,get(CameraParameters::KEY_PREVIEW_FORMAT));
        }
#endif

        //still port negotiation
        if(mCamera_caps.stPortCapability[CAM_PORT_STILL].bArbitrarySize){
            if(mCamera_caps.stPortCapability[CAM_PORT_STILL].stMin.iWidth <= 320 &&
                    mCamera_caps.stPortCapability[CAM_PORT_STILL].stMin.iHeight <= 240 &&
                    mCamera_caps.stPortCapability[CAM_PORT_STILL].stMax.iWidth >= 2560 &&
                    mCamera_caps.stPortCapability[CAM_PORT_STILL].stMax.iHeight >= 1920){
                set(CameraParameters::KEY_SUPPORTED_PICTURE_SIZES, "640x480,320x240,2048x1536,2560x1920");
                setPictureSize(640, 480);
            }
        }
        else{
            char val1[10];
            char val2[10];
            v = String8("");
            width_min = 0;
            height_min = 0;
            for(int i=0; i<mCamera_caps.stPortCapability[CAM_PORT_STILL].iSupportedSizeCnt; i++){
                width = mCamera_caps.stPortCapability[CAM_PORT_STILL].stSupportedSize[i].iWidth;
                height = mCamera_caps.stPortCapability[CAM_PORT_STILL].stSupportedSize[i].iHeight;
                if( width_min <= 0 || height_min <= 0 ||
                        width < width_min){
                    width_min = width;
                    height_min = height;
                }
                sprintf(val1,"%d", width);
                sprintf(val2,"%d", height);
                v += String8(val1)+
                    String8("x")+
                    String8(val2);
                /*
                   v = String8(itoa(mCamera_caps.stPortCapability[CAM_PORT_STILL].stSupportedSize[i].iWidth))+
                   String8("x")+
                   String8(itoa(mCamera_caps.stPortCapability[CAM_PORT_STILL].stSupportedSize[i].iHeight));
                   */
                if(i != (mCamera_caps.stPortCapability[CAM_PORT_STILL].iSupportedSizeCnt-1))
                    v += String8(",");
            }
            set(CameraParameters::KEY_SUPPORTED_PICTURE_SIZES,v);
            if( width_min > 0 && height_min > 0 ){
                setPictureSize(width_min,height_min);
            }
        }
        TRACE_D("%s:CE supported pic size:%s",__FUNCTION__,
                get(CameraParameters::KEY_SUPPORTED_PICTURE_SIZES));
        TRACE_D("%s:default picture size:%dx%d",__FUNCTION__,width_min,height_min);

        // still capture format negotiation
        v = String8("");
        for(int i=0; i<mCamera_caps.stPortCapability[CAM_PORT_STILL].iSupportedFormatCnt; i++){
            int ce_idx = mCamera_caps.stPortCapability[CAM_PORT_STILL].eSupportedFormat[i];
            String8 ret = map_ce2andr(map_imgfmt,ce_idx);
            if (ret == String8(""))
                continue;
            if( v != String8("") )
                v += String8(",");
            v += ret;
        }
        if(v == String8("")){
            set(CameraParameters::KEY_SUPPORTED_PICTURE_FORMATS,CameraSetting::KEY_PICTURE_FORMAT);
            TRACE_E("CE support NO still format, use default:%s",CameraSetting::KEY_PICTURE_FORMAT);
        }
        else{
            set(CameraParameters::KEY_SUPPORTED_PICTURE_FORMATS,v.string());
            TRACE_D("%s:CE supported picture format:%s",__FUNCTION__,v.string());
            TRACE_D("%s:default picture format:%s",__FUNCTION__,get(CameraParameters::KEY_PICTURE_FORMAT));
        }

        TRACE_D("%s:default video format:%s",__FUNCTION__,get(CameraParameters::KEY_VIDEO_FRAME_FORMAT));

        set(CameraParameters::KEY_SUPPORTED_PREVIEW_FRAME_RATES, "30,15");
        set(CameraParameters::KEY_JPEG_QUALITY, camera_caps.stSupportedJPEGParams.iMaxQualityFactor);

        /*
         * TODO:Platform special val, use the following parameters by force
         * if platform/android not support some sensor settings.
         * It means we may not use all sensor supported settings here.
         */
        /*
           set(CameraParameters::KEY_SUPPORTED_PREVIEW_FORMATS,"yuv422i");
           setPreviewFormat("yuv422i");

           set(CameraParameters::KEY_SUPPORTED_PICTURE_FORMATS,"jpeg");
           setPictureFormat("jpeg");
           */

        return NO_ERROR;
    }

    status_t CameraSetting::setSupportedSceneModeCap(const CAM_ShotModeCapability& camera_shotcaps)
    {
        mCamera_shotcaps = camera_shotcaps;

        int ce_idx = -1;
        String8 v = String8("");
        String8 ret;
        int len=0;

        /*
         * TODO: currently only support scene mode-manual by defalut,
         * donot notify upper layer here.
         * refer to ceInit():CAM_SHOTMODE_MANUAL
         */

        //map_scenemode/shotmode
        v = String8("");
        for (int i=0; i < mCamera_caps.iSupportedShotModeCnt; i++){
            ce_idx = mCamera_caps.eSupportedShotMode[i];
            ret = map_ce2andr(map_scenemode,ce_idx);
            if (ret == String8(""))
                continue;
            if( v != String8("") )
                v += String8(",");
            v += ret;
        }
        if( v != String8("") ){
            set(CameraParameters::KEY_SUPPORTED_SCENE_MODES,v.string());
            TRACE_D("%s:CE supported scene mode:%s",__FUNCTION__,v.string());
            set(CameraParameters::KEY_SCENE_MODE,CameraParameters::SCENE_MODE_AUTO);
        }

        //map_whitebalance
        v = String8("");
        for (int i=0; i < camera_shotcaps.iSupportedWBModeCnt; i++){
            ce_idx = camera_shotcaps.eSupportedWBMode[i];
            ret = map_ce2andr(map_whitebalance,ce_idx);
            if (ret == String8(""))
                continue;
            if (v != String8(""))
                v += String8(",");
            v += ret;
        }
        if( v != String8("") ){
            set(CameraParameters::KEY_SUPPORTED_WHITE_BALANCE,v.string());
            TRACE_D("%s:CE supported white balance:%s",__FUNCTION__,v.string());
        }

        //map_coloreffect
        v = String8("");
        for (int i=0; i < camera_shotcaps.iSupportedColorEffectCnt; i++){
            ce_idx = camera_shotcaps.eSupportedColorEffect[i];
            ret = map_ce2andr(map_coloreffect,ce_idx);
            if (ret == String8(""))
                continue;
            if (v != String8(""))
                v += String8(",");
            v += ret;
        }
        if( v != String8("") ){
            set(CameraParameters::KEY_SUPPORTED_EFFECTS,v.string());
            TRACE_D("%s:CE supported color effect:%s",__FUNCTION__,v.string());
        }

        //map_bandfilter
        v = String8("");
        for (int i=0; i < camera_shotcaps.iSupportedBdFltModeCnt; i++){
            ce_idx = camera_shotcaps.eSupportedBdFltMode[i];
            ret = map_ce2andr(map_bandfilter,ce_idx);
            if (ret == String8(""))
                continue;
            if (v != String8(""))
                v += String8(",");
            v += ret;
        }
        if( v != String8("") ){
            set(CameraParameters::KEY_SUPPORTED_ANTIBANDING,v.string());
            TRACE_D("%s:CE supported antibanding:%s",__FUNCTION__,v.string());
        }

        //map_flash
        v = String8("");
        for (int i=0; i < camera_shotcaps.iSupportedFlashModeCnt; i++){
            ce_idx = camera_shotcaps.eSupportedFlashMode[i];
            ret = map_ce2andr(map_flash,ce_idx);
            if (ret == String8(""))
                continue;
            if (v != String8(""))
                v += String8(",");
            v += ret;
        }
        if( v != String8("") ){
            set(CameraParameters::KEY_SUPPORTED_FLASH_MODES,v.string());
            TRACE_D("%s:CE supported flash mode:%s",__FUNCTION__,v.string());
        }

        //map_focus
        v = String8("");
        for (int i=0; i < camera_shotcaps.iSupportedFocusModeCnt; i++){
            ce_idx = camera_shotcaps.eSupportedFocusMode[i];
            ret = map_ce2andr(map_focus,ce_idx);
            if (ret == String8(""))
                continue;
            if (v != String8(""))
                v += String8(",");
            v += ret;
        }
        if( v != String8("") ){
            set(CameraParameters::KEY_SUPPORTED_FOCUS_MODES,v.string());
            TRACE_D("%s:CE supported focus mode:%s",__FUNCTION__,v.string());
        }

        // evc
        if ( 0 != camera_shotcaps.iEVCompStepQ16)
        {
            v = String8("");
            char tmp[32] = {'\0'};
            int max_exposure = camera_shotcaps.iMaxEVCompQ16 / camera_shotcaps.iEVCompStepQ16;
            sprintf(tmp, "%d", max_exposure);
            v = String8(tmp);
            set(CameraParameters::KEY_MAX_EXPOSURE_COMPENSATION, v.string());
            TRACE_D("%s:CE supported max exposure compensation:%s",__FUNCTION__,v.string());

            int min_exposure = camera_shotcaps.iMinEVCompQ16 / camera_shotcaps.iEVCompStepQ16;
            sprintf(tmp, "%d", min_exposure);
            v = String8(tmp);
            set(CameraParameters::KEY_MIN_EXPOSURE_COMPENSATION, v.string());
            TRACE_D("%s:CE supported min exposure compensation:%s",__FUNCTION__,v.string());

            set(CameraParameters::KEY_EXPOSURE_COMPENSATION_STEP, "1");
            TRACE_D("%s:CE supported exposure compensation step:%s",__FUNCTION__,"1");
        }

        //digital zoom
        // KEY_ZOOM_RATIOS stands for the zoom ratios of all zoom values.
        // The zoom ratio is in 1/100 increments.
        // Ex: a zoom of 3.2x is returned as 320.
        // The number of list elements is KEY_MAX_ZOOM + 1. ??
        // The first element is always 100.
        // The last element is the zoom ratio of zoom value KEY_MAX_ZOOM. ??
        // Example value: "100,150,200,250,300,350,400". Read only.
        // NOTES:
        // 1>android app use the slider ratio index for zoom setting index,
        // which starts from 0 instead of 1.
        //
        v=String8("");
        const int minzoom = camera_shotcaps.iMinDigZoomQ16;
        const int maxzoom = camera_shotcaps.iMaxDigZoomQ16;
        const int cezoomstep = camera_shotcaps.iDigZoomStepQ16;
        if ((0x1<<16) == maxzoom){
            set(CameraParameters::KEY_ZOOM_SUPPORTED, "false");
        }
        else{
            for (int i=minzoom; i<=maxzoom; i+=cezoomstep){
                //1.0x for andr min zoom, the first element is always 100
                int zoomratio = (i-minzoom)*AndrZoomRatio/65536 + 100;
                char tmp[32] = {'\0'};
                sprintf(tmp, "%d", zoomratio);
                if (String8("") == v)
                    v = String8(tmp);
                else
                    v = v + String8(",") + String8(tmp);
            }
            if( v != String8("") ){
                set(CameraParameters::KEY_ZOOM_RATIOS, v.string());
                TRACE_D("%s:CE supported digital zoom:%s",__FUNCTION__,v.string());

                char tmp[32] = {'\0'};
                //1.0x for andr min zoom
                const int andrmaxzoom = (maxzoom-minzoom)/(cezoomstep);
                sprintf(tmp, "%d", andrmaxzoom);
                set(CameraParameters::KEY_MAX_ZOOM, tmp);
                set(CameraParameters::KEY_ZOOM_SUPPORTED, CameraParameters::TRUE);
            }
            else{
                set(CameraParameters::KEY_ZOOM_SUPPORTED, "false");
            }
        }

        // add fps range support
        {
            int min_fps = (camera_shotcaps.iMinFpsQ16 >> 16);
            int max_fps = (camera_shotcaps.iMaxFpsQ16 >> 16);
            TRACE_D("%s:CE supported framerate:max=%d,min=%d",__FUNCTION__,max_fps,min_fps);

            v = String8("");
            char tmp[32] = {'\0'};
            sprintf(tmp, "%d,%d", min_fps*1000,max_fps*1000);
            v = String8(tmp);
            set(CameraParameters::KEY_PREVIEW_FPS_RANGE, v.string());

            sprintf(tmp, "(%d,%d)", min_fps*1000,max_fps*1000);
            v = String8(tmp);
            set(CameraParameters::KEY_SUPPORTED_PREVIEW_FPS_RANGE, v.string());

            sprintf(tmp, "%d,%d", min_fps,max_fps);
            v = String8(tmp);
            set(CameraParameters::KEY_SUPPORTED_PREVIEW_FRAME_RATES, v.string());

            sprintf(tmp, "%d", max_fps);
            v = String8(tmp);
            set(CameraParameters::KEY_PREVIEW_FRAME_RATE, v.string());
        }

        return NO_ERROR;
    }

    CAM_ShotModeCapability CameraSetting::getSupportedSceneModeCap(void) const
    {
        return mCamera_shotcaps;
    }

    /*
     * map ce parameters to android parameters
     * INPUT
     * map_tab:map table, for wb, focus, color effect...
     * ce_index:cameraengine parameter
     * RETURN
     * String8:android parameters, String8("") if no mapping
     */
    String8 CameraSetting::map_ce2andr(/*int map_len,*/const map_ce_andr* map_tab, int ce_idx)const
    {
        if (-1 == ce_idx)
            return String8("");

        String8 v = String8("");
        for (int j=0; ; j++){
            if (map_tab[j].ce_idx == -1 && String8("") == map_tab[j].andr_str)
                break;//get end
            if (ce_idx == map_tab[j].ce_idx && String8("") != map_tab[j].andr_str){
                v = map_tab[j].andr_str;
                break;
            }
        }
        return v;
    }

    /*
     * map android parameters to ce parameters
     * INPUT
     * map_tab:map table, for wb, focus, color effect...
     * andr_str:android parameter
     * RETURN
     * int:ce parameters, -1 if no mapping
     */
    int CameraSetting::map_andr2ce(/*int map_len,*/const map_ce_andr* map_tab, const char* andr_str)const
    {
        if (0 == andr_str)
            return -1;

        int ce_idx = -1;
        for (int j=0; ; j++){
            if (map_tab[j].ce_idx == -1 && String8("") == map_tab[j].andr_str)
                break;//get end
            if (String8(andr_str) ==  map_tab[j].andr_str && -1 != map_tab[j].ce_idx){
                ce_idx = map_tab[j].ce_idx;
                break;
            }
        }
        return ce_idx;
    }

    /*
     * FIXME: complete the setting map
     */
    status_t CameraSetting::setParameters(const CameraParameters& param)
    {
        FUNC_TAG;
        Mutex::Autolock lock(mParaLock);

        const char* v;
        int cesetting;
        int ce_idx = -1;
        //check preview format
        {
            v = param.getPreviewFormat();
            if (0 == v){
                TRACE_E("preview format not set");
                return BAD_VALUE;
            }
            cesetting = map_andr2ce(CameraSetting::map_imgfmt,v);
            if (-1 != cesetting){
                ce_idx = -1;
                for(int i=0; i<mCamera_caps.stPortCapability[CAM_PORT_PREVIEW].iSupportedFormatCnt; i++){
                    ce_idx = mCamera_caps.stPortCapability[CAM_PORT_PREVIEW].eSupportedFormat[i];
                    if(ce_idx == cesetting){
                        TRACE_D("%s:preview format=%s", __FUNCTION__, v);
                        break;
                    }
                }
            }
            if (cesetting != ce_idx || -1 == cesetting){
                TRACE_E("preview format not supported,%s",v);
                return BAD_VALUE;
            }
        }

        //check picture format
        {
            v = param.getPictureFormat();
            if (0 == v){
                TRACE_E("picture format not set");
                return BAD_VALUE;
            }
            cesetting = map_andr2ce(CameraSetting::map_imgfmt,v);
            if (-1 != cesetting){
                ce_idx = -1;
                for(int i=0; i<mCamera_caps.stPortCapability[CAM_PORT_STILL].iSupportedFormatCnt; i++){
                    ce_idx = mCamera_caps.stPortCapability[CAM_PORT_STILL].eSupportedFormat[i];
                    if(ce_idx == cesetting){
                        TRACE_D("%s:picture format=%s", __FUNCTION__, v);
                        break;
                    }
                }
            }
            if (cesetting != ce_idx || -1 == cesetting){
                TRACE_E("picture format not supported,%s",v);
                return BAD_VALUE;
            }
        }

        // update platform specific parameters according
        // to the camera parameters passed in
        {
            int prev_width, prev_height;
            param.getPreviewSize(&prev_width, &prev_height);
            TRACE_D("%s:preview size=%dx%d", __FUNCTION__, prev_width, prev_height);

            //check whether the preview size is in supported list
            const Size prev_size(prev_width,prev_height);
            Vector<Size> supported_sizes;
            getSupportedPreviewSizes(supported_sizes);
            Size size;
            for(unsigned int i=0;i<supported_sizes.size();i++){
                size = supported_sizes.itemAt(i);
                if(size.width == prev_size.width && size.height == prev_size.height){
                    break;//found
                }
            }
            if(size.width != prev_size.width || size.height != prev_size.height){
                TRACE_E("preview size is not supported, return!");
                return BAD_VALUE;
            }
        }

        {
            int pic_width, pic_height;
            param.getPictureSize(&pic_width, &pic_height);
            TRACE_D("%s:picture size=%dx%d\n", __FUNCTION__, pic_width, pic_height);

            //check whether the picture size is in supported list
            const Size pic_size(pic_width,pic_height);
            Vector<Size> supported_sizes;
            getSupportedPictureSizes(supported_sizes);
            Size size;
            for(unsigned int i=0;i<supported_sizes.size();i++){
                size = supported_sizes.itemAt(i);
                if(size.width == pic_size.width && size.height == pic_size.height){
                    break;//found
                }
            }
            if(size.width != pic_size.width || size.height != pic_size.height){
                TRACE_E("pic size is not supported, return!");
                return BAD_VALUE;
            }
        }

        /*
         * copy to local setting and check it, the platform ones should be kept unchanged
         */
        //*(static_cast<CameraParameters *>(this)) = param;
        CameraParameters::operator=(param);

        //FIXME:to support mutil-preview/video formats including NV21, here we have to
        //update video-frame-format again after initialization, as preview/video happens
        //in the same thread and using the same buffers.
        //NOTES:we could reset KEY_VIDEO_FRAME_FORMAT here based on the assumption that
        //camcorder will get video-frame-format after setting the preview parameters.
        v = param.getPreviewFormat();
        set(CameraParameters::KEY_VIDEO_FRAME_FORMAT,v);
        TRACE_D("%s:video format=%s", __FUNCTION__, v);

        mPreviewFlipRot = CAM_FLIPROTATE_NORMAL;
        return NO_ERROR;
    }

    CameraParameters CameraSetting::getParameters() const
    {
        FUNC_TAG;
        Mutex::Autolock lock(mParaLock);
        return (*this);
        //return static_cast<CameraParameters>(*this);
    }

    status_t CameraSetting::getBufCnt(int* previewbufcnt,int* stillbufcnt,int* videobufcnt) const
    {
        Mutex::Autolock lock(mParaLock);
        *previewbufcnt  =	pMrvlCameraInfo->def_kPreviewBufCnt;
        *stillbufcnt    =	pMrvlCameraInfo->def_kStillBufCnt;
        *videobufcnt    =	pMrvlCameraInfo->def_kVideoBufCnt;
        return NO_ERROR;
    }

    int CameraSetting::getSensorID() const
    {
        Mutex::Autolock lock(mParaLock);
        return pMrvlCameraInfo->getSensorID();
    }

    const char* CameraSetting::getProductName() const
    {
        return mProductName;
    }

    const char* CameraSetting::getSensorName() const
    {
        Mutex::Autolock lock(mParaLock);
        return pMrvlCameraInfo->getSensorName();
    }

    const CameraInfo* CameraSetting::getCameraInfo() const{
        return pMrvlCameraInfo->getCameraInfo();
    }

#if 0
    CAM_FlipRotate CameraSetting::getPreviewFlipRot() const
    {
        Mutex::Autolock lock(mParaLock);
        return mPreviewFlipRot;
    }
#endif

    status_t CameraSetting::initTableSensorID(const char* sensorname, int sensorid)
    {
        int max_camera = sizeof(mMrvlCameraInfo)/sizeof(mMrvlCameraInfo[0]);
        int i;
        for(i=0; i<max_camera; i++){
            const char* detected_sensor = mMrvlCameraInfo[i].getSensorName();
            int detected_sensorid = mMrvlCameraInfo[i].getSensorID();
            //NOTES:only cmp the basic sensor name.eg."ov5642" or "BasicSensor"
            if(0 == strncmp(detected_sensor,sensorname,strlen(detected_sensor))){
                if(-1 == detected_sensorid){
                    mMrvlCameraInfo[i].setSensorID(sensorid);
                    iNumOfSensors++;
                }
                else{
                    TRACE_E("Sensor %s already init",detected_sensor);
                }
                return NO_ERROR;
            }
        }

        TRACE_E("No matching sensor in list. Pls check mMrvlCameraInfo[]");
        //FIXME:mMrvlCameraInfo[i].setSensorID(-1);
        return UNKNOWN_ERROR;
    }

    int CameraSetting::getNumOfCameras(){
        TRACE_D("%s:iNumOfSensors=%d",__FUNCTION__,iNumOfSensors);
        return iNumOfSensors;
    }

    const CameraInfo* MrvlCameraInfo::getCameraInfo() const{
        return &mCameraInfo;
    }

}; //namespace android
