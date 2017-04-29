LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	src/ire_lib.c 

LOCAL_C_INCLUDES := \
	vendor/marvell/generic/ire/src     \
	vendor/marvell/generic/bmm-lib/lib

LOCAL_SHARED_LIBRARIES := libutils libcutils

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE:= libire

include $(BUILD_SHARED_LIBRARY)
