LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
        libippexif.a

LOCAL_MODULE_TAGS := optional
include $(BUILD_MULTI_PREBUILT)




