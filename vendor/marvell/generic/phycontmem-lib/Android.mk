LOCAL_PATH := $(call my-dir)

L_PATH := $(LOCAL_PATH)

include $(CLEAR_VARS)

include $(L_PATH)/phycontmem/Android.mk
include $(L_PATH)/pmemhelper/Android.mk

