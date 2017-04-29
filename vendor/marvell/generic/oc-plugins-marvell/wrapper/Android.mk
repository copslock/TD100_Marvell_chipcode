LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

# Set up the OpenCore variables.

include external/opencore/Config.mk
 
LOCAL_SRC_FILES := \
        IppMrvlOmxWrapper.cpp

LOCAL_C_INCLUDES := \
        $(PV_INCLUDES) \
        external/opencore/extern_libs_v2/khronos/openmax/include \
	vendor/marvell/generic/ipplib/openmax/include

LOCAL_COPY_HEADERS := \

LOCAL_SHARED_LIBRARIES := \
        libdl \
        libutils \
        libcutils

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libMrvlOmxWrapper
include $(BUILD_SHARED_LIBRARY)
