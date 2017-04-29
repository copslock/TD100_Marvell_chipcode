LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES :=  \
	pmem_helper_lib.c

LOCAL_MODULE := libpmemhelper

LOCAL_SHARED_LIBRARIES := libutils

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

