LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    usbsetting.cpp

#
# define LOCAL_PRELINK_MODULE to false to not use pre-link map
#
LOCAL_PRELINK_MODULE := false

LOCAL_SHARED_LIBRARIES += libutils libcutils libnativehelper

LOCAL_MODULE := libusbsetting

include $(BUILD_SHARED_LIBRARY)
