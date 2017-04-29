#
## libcameraengine
#
#

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

include vendor/marvell/generic/cameraengine2/CameraConfig.mk

LOCAL_SRC_FILES := \
     extisp_cameraengine/cam_extisp_eng.c \
     extisp_cameraengine/cam_extisp_ppu.c \
     general/cam_gen.c \
     general/cam_utility.c \
     general/cam_trace.c \

LOCAL_CFLAGS += \
    -I vendor/marvell/generic/cameraengine2/include \
    -I vendor/marvell/generic/cameraengine2/src/cameraengine2/ansi_c/extisp_cameraengine \
    -I vendor/marvell/generic/cameraengine2/src/cameraengine2/ansi_c/_include \
    -D LINUX \
    -D ANDROID \
    -D CAM_LOG_ERROR
    
LOCAL_SHARED_LIBRARIES +=libmiscgen \
                         libphycontmem \
                         libutils
    
# external dependencies
LOCAL_C_INCLUDES := vendor/marvell/generic/phycontmem-lib/phycontmem \
                    vendor/marvell/generic/graphics \
                    vendor/marvell/generic/ipplib/include
                    

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libcameraengine

include $(BUILD_STATIC_LIBRARY)
#include $(BUILD_SHARED_LIBRARY)

