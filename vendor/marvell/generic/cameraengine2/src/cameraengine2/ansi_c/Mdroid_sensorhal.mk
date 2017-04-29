#
## libsensorhal_extisp
#
#

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

include vendor/marvell/generic/cameraengine2/CameraConfig.mk

LOCAL_SRC_FILES := \
     extisp_sensorhal/cam_extisp_v4l2base.c \
     extisp_sensorhal/cam_extisp_sensorhal.c \
     extisp_sensorhal/ov5642/cam_extisp_ov5642.c \
     extisp_sensorhal/ov5640/cam_extisp_ov5640.c \
     extisp_sensorhal/gt2005/cam_extisp_gt2005.c \
     extisp_sensorhal/mt9v113/cam_extisp_mt9v113.c \
     extisp_sensorhal/basicsensor/cam_extisp_basicsensor.c \
     extisp_sensorhal/fakesensor/cam_extisp_fakesensor.c \


LOCAL_CFLAGS += \
    -I vendor/marvell/generic/cameraengine2/include \
    -I vendor/marvell/generic/cameraengine2/src/cameraengine2/ansi_c/_include \
    -I vendor/marvell/generic/cameraengine2/src/cameraengine2/ansi_c/extisp_sensorhal \
    -D LINUX \
    -D ANDROID \
    -D CAM_LOG_ERROR

LOCAL_WHOLE_STATIC_LIBRARIES := FaceTrack
    
# external dependencies
LOCAL_C_INCLUDES := vendor/marvell/generic/phycontmem-lib/phycontmem \
                    vendor/marvell/generic/ipplib/include

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libsensorhal_extisp

include $(BUILD_STATIC_LIBRARY)
#include $(BUILD_SHARED_LIBRARY)
