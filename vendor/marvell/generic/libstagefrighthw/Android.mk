LOCAL_PATH := $(call my-dir)

STAGEFRIGHTHW_TOP := $(LOCAL_PATH)

include $(CLEAR_VARS)

include $(STAGEFRIGHTHW_TOP)/renderer/Android.mk
include $(STAGEFRIGHTHW_TOP)/entry/Android.mk
