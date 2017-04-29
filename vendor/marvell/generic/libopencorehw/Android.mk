LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# Set up the OpenCore variables.

include external/opencore/Config.mk
LOCAL_C_INCLUDES := $(PV_INCLUDES) \
	vendor/marvell/generic/phycontmem-lib/phycontmem \
        vendor/marvell/generic/overlay-hal/ \
        vendor/marvell/generic/ipplib/openmax/include\
        vendor/marvell/generic/graphics/include

LOCAL_CFLAGS := $(PV_CFLAGS)

LOCAL_CFLAGS += -DPLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)

ifeq ($(BOARD_ENABLE_FAST_OVERLAY), true)
LOCAL_CFLAGS += -DFAST_OVERLAY
endif

ifeq ($(USE_FASTOVERLAY_CALLBACK), true)
LOCAL_CFLAGS += -DFAST_OVERLAY_CALLBACK
endif


ifneq ($(PLATFORM_SDK_VERSION), 3)
ifneq ($(PLATFORM_SDK_VERSION), 4)
LOCAL_SHARED_LIBRARIES += libbinder libgcu
endif
endif

LOCAL_SRC_FILES := \
        android_surface_output_ov.cpp \
	android_surface_output_3d.cpp \
        android_surface_output_gcx00.cpp \
        android_surface_output_factory.cpp \
	mrvl_buf_allocator.cpp 


LOCAL_STATIC_LIBRARIES := 

LOCAL_SHARED_LIBRARIES += \
    libutils \
    libcutils \
    libui \
    libsurfaceflinger_client \
    libhardware\
    libopencore_common \
    libopencore_player \
    libphycontmem

# do not prelink
LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libopencorehw


include $(BUILD_SHARED_LIBRARY)

