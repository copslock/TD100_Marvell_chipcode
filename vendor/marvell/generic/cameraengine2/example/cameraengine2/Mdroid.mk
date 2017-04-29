#
# MrvlCameraDemo
#

LOCAL_PATH:=$(call my-dir)
include $(CLEAR_VARS)

include vendor/marvell/generic/cameraengine2/CameraConfig.mk

################################
## turn on the video encoder macro
## and link the OpenMAX IL lib for video encoding
################################
ifeq ($(ENABLE_VIDEO_ENCODER), y)
CFLAGS    +=   -I vendor/marvell/generic/ipplib/openmax/include \
               -D _ENABLE_VIDEO_ENCODER_
LOCAL_SHARED_LIBRARIES+= \
		libMrvlOmx
endif

# put your source files here.
LOCAL_SRC_FILES:= \
    src/main.c \
    src/display.c \
    src/test_camera_speed.c \
    src/test_dequeue_single_port.c \
    src/test_dv_dsc_actively_dequeue.c \
    src/test_dv_dsc_on_event_dequeue.c \
    src/test_harness.c \
    src/test_harness_encoder.c \
    src/test_video_fps.c \
    src/test_resolution_switch.c \
    src/test_digitalzoom_stress.c \


LOCAL_CFLAGS +=   -I vendor/marvell/generic/cameraengine2/example/cameraengine2/src \
                  -I vendor/marvell/generic/cameraengine2/include \
                  -I vendor/marvell/generic/ipplib/include \
                  -I vendor/marvell/generic/overlay-hal \
                  -I vendor/marvell/generic/phycontmem-lib/phycontmem

# put the MACROS you depend on here
LOCAL_CFLAGS +=   -D ANDROID \
                  -D CAM_LOG_VERBOSE

#    -mabi=aapcs-linux
					
# put static libraies used here
LOCAL_STATIC_LIBRARIES+=libcameraengine \
                        libsensorhal_extisp \
                        libippcam \
                        FaceTrack

# put shared objects used here
LOCAL_SHARED_LIBRARIES+=libphycontmem \
			libcodecjpegdec \
			libcodecjpegenc \
			libmiscgen \
			libGAL \
			libgcu \
			libutils


# put your module name here
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:=MrvlCameraDemo	

include $(BUILD_EXECUTABLE)

