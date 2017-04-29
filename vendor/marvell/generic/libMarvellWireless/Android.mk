LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
	lib_marvell_wireless.c

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)   

LOCAL_SHARED_LIBRARIES := \
	libc	\
    libcutils \
    libutils 

LOCAL_STATIC_LIBRARIES += 

LOCAL_MODULE:= libMarvellWireless

LOCAL_PRELINK_MODULE := false
include $(BUILD_SHARED_LIBRARY)





