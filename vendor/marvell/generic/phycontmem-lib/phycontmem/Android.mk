LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES :=  \
	phycontmem.c

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../pmemhelper

LOCAL_MODULE := libphycontmem

LOCAL_SHARED_LIBRARIES := libutils libpmemhelper

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

