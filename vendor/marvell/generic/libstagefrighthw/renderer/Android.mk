LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

ifeq ($(shell if [ $(PLATFORM_SDK_VERSION) -ge 9 ]; then echo big9; fi),big9)
LOCAL_C_INCLUDES += $(JNI_H_INCLUDE) \
    frameworks/base/include/media/stagefright/openmax\
    vendor/marvell/generic/overlay-hal \
    vendor/marvell/generic/ire/src \
    vendor/marvell/generic/bmm-lib/lib \
    vendor/marvell/generic/ipplib/openmax/include \
    vendor/marvell/generic/graphics/include

else
# Set up the OpenCore variables.
include external/opencore/Config.mk
LOCAL_C_INCLUDES := $(PV_INCLUDES)
LOCAL_CFLAGS := $(PV_CFLAGS_MINUS_VISIBILITY)

LOCAL_C_INCLUDES += $(JNI_H_INCLUDE) \
    $(TOP)/external/opencore/extern_libs_v2/khronos/openmax/include \
    $(LOCAL_PATH) \
    vendor/marvell/generic/overlay-hal \
    vendor/marvell/generic/ire/src \
    vendor/marvell/generic/bmm-lib/lib
endif

LOCAL_CFLAGS += -DPLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)

ifeq ($(BOARD_ENABLE_FAST_OVERLAY), true)
LOCAL_CFLAGS += -DFAST_OVERLAY
endif

LOCAL_SRC_FILES += \
        HardwareRendererOverlay.cpp \
        HardwareRendererGCxx.cpp

ifeq ($(ENABLE_MARVELL_DRMPLAY),true)
    LOCAL_SRC_FILES += DrmHardwareRendererOverlay.cpp
endif

LOCAL_SHARED_LIBRARIES :=       \
        libbinder               \
        libmedia                \
        libutils                \
        libui                   \
        libcutils               \
        libstagefright_color_conversion \
        libsurfaceflinger_client \
        libhardware \
        libgcu

ifeq ($(TARGET_OS)-$(TARGET_SIMULATOR),linux-true)
        LOCAL_LDLIBS += -lpthread -ldl
endif

LOCAL_PRELINK_MODULE := false

ifneq ($(TARGET_SIMULATOR),true)
LOCAL_SHARED_LIBRARIES += libdl
endif

LOCAL_MODULE:= libstagefright_hardware_renderer

include $(BUILD_SHARED_LIBRARY)
