#ifndef ANDROID_HARDWARE_CAMERA_SETTING_H
#define ANDROID_HARDWARE_CAMERA_SETTING_H

#if PLATFORM_SDK_VERSION >= 8
#include <camera/CameraHardwareInterface.h>
#else
#include <ui/CameraHardwareInterface.h>
#endif
#include <utils/threads.h>
#include <cutils/properties.h>

#include "CameraEngine.h"

namespace android {

    class MrvlCameraInfo{
        private:
            struct  CameraInfo mCameraInfo;
            const char* sSensorName;
            //MUTABLE, used by cameraengine, chich could be changed
            //according to quired results from camengine
            //NOTES:initialized to -1, for sensors that not detected.
            int iSensorID;

        public:
            MrvlCameraInfo(const char* sensorname="",
                    int facing=-1,
                    int orient=-1,
                    int preview_width=-1,
                    int preview_height=-1,
                    const char* preview_fmt="",
                    const char* video_fmt="",
                    int pic_width=-1,
                    int pic_height=-1,
                    const char* pic_fmt="",
                    int kPreviewBufCnt=-1,
                    int kStillBufCnt=-1,
                    int kVideoBufCnt=-1
                    ):
                iSensorID(-1),
                sSensorName(sensorname),
                def_preview_width(preview_width),
                def_preview_height(preview_height),
                def_preview_fmt(preview_fmt),
                def_video_fmt(video_fmt),
                def_pic_width(pic_width),
                def_pic_height(pic_height),
                def_pic_fmt(pic_fmt),
                def_kPreviewBufCnt(kPreviewBufCnt),
                def_kStillBufCnt(kStillBufCnt),
                def_kVideoBufCnt(kVideoBufCnt)
        {
            mCameraInfo.facing = facing;
            mCameraInfo.orientation = orient;
        }
            /*
               MrvlCameraInfo():
               iSensorID(-1)
               {
               }
               */
            friend class CameraSetting;
        private:
            void setSensorID(int sensorid){
                iSensorID = sensorid;
            }
            int getSensorID() const{return iSensorID;}
            const char* getSensorName() const{return sSensorName;}
            const CameraInfo* getCameraInfo() const;

            const int   def_preview_width;
            const int   def_preview_height;
            const char* def_preview_fmt;
            const char* def_video_fmt;

            const int   def_pic_width;
            const int   def_pic_height;
            const char* def_pic_fmt;

            /*
             * preview/still/video buf cnt for v4l2 USER-POINTER driver,
             * it depends on requirements of camera dirver, overlay-hal and encoder components
             */
            const int def_kPreviewBufCnt;
            const int def_kStillBufCnt;
            const int def_kVideoBufCnt;

    };

    //inherit from android camera parameter, utilize memfunc in class CameraParameters
    class CameraSetting: public CameraParameters/*FIXME:, public RefBase*/{
        public:
            /*
             * NOTES:we put constructor implementation here to isolate the platform
             * dependant parameters.
             */
            CameraSetting(){};
            /*
               CameraSetting(CameraParameters param):
               CameraParameters(param)
               {};
               */
            CameraSetting(const char* sensorname);
            CameraSetting(int sensorid);

            static MrvlCameraInfo mMrvlCameraInfo[];
            static const char* mProductName;
            static status_t initTableSensorID(const char* sensorname, int sensorid);
            static int getNumOfCameras();

            virtual ~CameraSetting(){}

            // set/get standard parameters
            // interface exported for cameraservice
            status_t		setBasicCaps(const CAM_CameraCapability& camera_caps);
            status_t		setSupportedSceneModeCap(const CAM_ShotModeCapability& camera_shotcaps);
            CAM_ShotModeCapability getSupportedSceneModeCap(void) const;

            status_t		setParameters(const CameraParameters& param);
            CameraParameters	getParameters() const;

            struct map_ce_andr{
                String8 andr_str;
                int ce_idx;
            };

            static const map_ce_andr map_whitebalance[];
            static const map_ce_andr map_coloreffect[];
            static const map_ce_andr map_bandfilter[];
            static const map_ce_andr map_flash[];
            static const map_ce_andr map_focus[];
            static const map_ce_andr map_scenemode[];
            static const map_ce_andr map_imgfmt[];
            static const map_ce_andr map_exifrotation[];
            static const map_ce_andr map_exifwhitebalance[];
            static const int AndrZoomRatio;

            String8 map_ce2andr(/*int map_len,*/const map_ce_andr* map_tab, int ce_idx)const;
            int map_andr2ce(/*int map_len,*/const map_ce_andr* map_tab, const char* andr_str)const;

            status_t getBufCnt(int* previewbufcnt,int* stillbufcnt,int* videobufcnt) const;

            const char* getProductName() const;

            int		getSensorID() const;
            const char* getSensorName() const;

            const CameraInfo* getCameraInfo() const;

        private:
            static int iNumOfSensors;

            /*
             * mrvl platform parameters
             * NOTES:there is internal relationship between sensorid & CameraInfo.
             */
            const MrvlCameraInfo*  pMrvlCameraInfo;//reference ptr to MrvlCameraInfo[] table
            CAM_FlipRotate  mPreviewFlipRot;

            mutable Mutex       mParaLock;
            status_t		initDefaultParameters();

            CAM_CameraCapability    mCamera_caps;
            CAM_ShotModeCapability  mCamera_shotcaps;
    };

}; // namespace android

#endif
