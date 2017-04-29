LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_SRC_FILES := lights.c

LOCAL_SHARED_LIBRARIES := \
    libutils \
    libcutils \
    libandroid_runtime \
    libhardware\

# do not prelink
LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := lights.default

include $(BUILD_SHARED_LIBRARY)
