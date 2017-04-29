LOCAL_PATH:= $(call my-dir)

#cmmbplayengine.conf
include $(CLEAR_VARS)
LOCAL_MODULE := cmmbplayengine.conf
LOCAL_MODULE_TAGS := user
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc
LOCAL_SRC_FILES := cmmbplayengine.conf
include $(BUILD_PREBUILT)

#libcmmb_playengine.so
include $(CLEAR_VARS) 
LOCAL_MODULE_TAGS := user
LOCAL_PREBUILT_LIBS := libcmmb_playengine.so
include $(BUILD_MULTI_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libgstcmmbsplitter
LOCAL_MODULE_SUFFIX = .so
LOCAL_MODULE_TAGS := user
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/gst
LOCAL_SRC_FILES := libgstcmmbsplitter.so
include $(BUILD_PREBUILT)


