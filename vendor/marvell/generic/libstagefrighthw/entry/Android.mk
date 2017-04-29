LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

ifeq ($(shell if [ $(PLATFORM_SDK_VERSION) -ge 9 ]; then echo big9; fi),big9)
LOCAL_C_INCLUDES += $(JNI_H_INCLUDE) \
    frameworks/base/include/media/stagefright/openmax\
    vendor/marvell/generic/libstagefrighthw/renderer\
    vendor/marvell/generic/ipplib/openmax/include\
    frameworks/base/include/binder\
    frameworks/base/include/utils

else
# Set up the OpenCore variables.
include external/opencore/Config.mk
LOCAL_C_INCLUDES := $(PV_INCLUDES)
LOCAL_CFLAGS := $(PV_CFLAGS_MINUS_VISIBILITY)

LOCAL_C_INCLUDES += $(JNI_H_INCLUDE) \
    $(TOP)/external/opencore/extern_libs_v2/khronos/openmax/include \
    vendor/marvell/generic/libstagefrighthw/renderer
endif

LOCAL_CFLAGS += -DPLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)

ifeq ($(BOARD_ENABLE_FAST_OVERLAY), true)
LOCAL_CFLAGS += -DFAST_OVERLAY
endif

ifeq ($(ENABLE_MARVELL_DRMPLAY),true)
LOCAL_CFLAGS += -D_MARVELL_DRM_PLAYER
endif

LOCAL_SRC_FILES += \
        stagefright_mrvl_omx_plugin.cpp \
        stagefright_create_vendor_renderer.cpp \

LOCAL_SHARED_LIBRARIES :=       \
        libbinder               \
        libutils                \
        libcutils               \


ifeq ($(shell if [ $(PLATFORM_SDK_VERSION) -ge 9 ]; then echo big9; fi),big9)
LOCAL_SHARED_LIBRARIES += \
        libMrvlOmx \
        libstagefright_hardware_renderer
else
LOCAL_SHARED_LIBRARIES += \
        libMrvlOmxWrapper \
        libstagefright_hardware_renderer

endif

ifeq ($(TARGET_OS)-$(TARGET_SIMULATOR),linux-true)
        LOCAL_LDLIBS += -lpthread -ldl
endif

ifneq ($(TARGET_SIMULATOR),true)
LOCAL_SHARED_LIBRARIES += libdl
endif

LOCAL_MODULE:= libstagefrighthw

include $(BUILD_SHARED_LIBRARY)

