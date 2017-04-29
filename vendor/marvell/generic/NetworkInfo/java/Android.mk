LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_JNI_SHARED_LIBRARIES := libNetworkInfo

LOCAL_PACKAGE_NAME := NetworkInfo
LOCAL_CERTIFICATE := platform


include $(BUILD_PACKAGE)

