LOCAL_PATH:= $(call my-dir)

#
# libcamera
#

include $(CLEAR_VARS)

ifneq ($(strip $(USE_CAMERA_STUB)),true)
# put your source files here
LOCAL_SRC_FILES:=               \
	hal/CameraHardware.cpp \
	hal/ImageBuf.cpp \
	camctrl/Engine.cpp \
	camctrl/FakeCam.cpp \
	camctrl/exif_helper.cpp \
	setting/CameraSetting.cpp \
	preview/CameraPreviewOverlay.cpp \
	preview/CameraPreviewBaselaySW.cpp

# put the libraries you depend on here.
LOCAL_SHARED_LIBRARIES:= \
    libbinder            \
    libui                \
    libutils             \
    libcutils            \
    liblog               \
    libmedia		\
    libcamera_client	\
    libmiscgen           \
    libcodecjpegdec      \
    libcodecjpegenc	 \
    libphycontmem

# put your module name here
LOCAL_MODULE:= libcamera

LOCAL_CFLAGS += -I vendor/marvell/generic/ipplib/include \
                -I vendor/marvell/generic/camera-hal/hal \
                -I vendor/marvell/generic/camera-hal/camctrl \
                -I vendor/marvell/generic/camera-hal/preview \
		-I vendor/marvell/generic/camera-hal/setting \
                -I vendor/marvell/generic/cameraengine2/include	\
		-I vendor/marvell/generic/cameraengine2/src/cameraengine2/ansi_c/_include \
                -I vendor/marvell/generic/bmm-lib/lib \
		-I vendor/marvell/generic/overlay-hal \
		-I vendor/marvell/generic/cameraengine2/tool/include

LOCAL_CFLAGS += -DPLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)

# for cameraengine log
LOCAL_CFLAGS += -D ANDROID_CAMERAENGINE \
		-D CAM_LOG_VERBOSE

#LOCAL_PRELINK_MODULE:=false

#-----------------------------------------
#platform dependant macro

ifeq ($(TARGET_PRODUCT), evbnevo)
    LOCAL_CFLAGS    += -D SAARBMG1
endif

ifeq ($(TARGET_PRODUCT), saarbmg1)
    LOCAL_CFLAGS    += -D SAARBMG1
endif

ifeq ($(TARGET_PRODUCT), jasper)
    LOCAL_CFLAGS    += -D JASPER
endif

ifeq ($(TARGET_PRODUCT), brownstone)
    LOCAL_CFLAGS    += -D BROWNSTONE
endif

ifeq ($(TARGET_PRODUCT), abilene)
    LOCAL_CFLAGS    += -D ABILENE
endif

ifeq ($(TARGET_PRODUCT), dkb)
    LOCAL_CFLAGS    += -D DKB
endif

ifeq ($(TARGET_PRODUCT), evbmg1)
    LOCAL_CFLAGS    += -D EVBMG1
endif

ifeq ($(TARGET_PRODUCT), saarcnevo)
    LOCAL_CFLAGS    += -D SAARBMG1
endif

ifeq ($(TARGET_PRODUCT), dkbnevo)
    LOCAL_CFLAGS    += -D SAARBMG1
endif
#-----------------------------------------

LOCAL_STATIC_LIBRARIES += libcameraengine \
			  libcodecjpegdec \
			  libsensorhal_extisp \
			  libippcam \
			  libippexif \
                          FaceTrack \
			  libpower

include $(BUILD_SHARED_LIBRARY)

endif
