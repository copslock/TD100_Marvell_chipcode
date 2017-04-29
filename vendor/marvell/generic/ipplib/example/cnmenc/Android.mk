LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false

LOCAL_SRC_FILES := \
src/appCMEnc.c \
../main/src/main.c

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := appCnmEnc.exe

LOCAL_CFLAGS :=  -D_IPP_LINUX

LOCAL_SHARED_LIBRARIES :=  libmiscgen libpmemhelper libcnm libcnmhal libcodeccnmenc

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/../misc \
    vendor/marvell/generic/ipplib/include \


LOCAL_MODULE_PATH :=$(LOCAL_PATH)

-include $(PV_TOP)/Android_system_extras.mk

include $(BUILD_EXECUTABLE)
