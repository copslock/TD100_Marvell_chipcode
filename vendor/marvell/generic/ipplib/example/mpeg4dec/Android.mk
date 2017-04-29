LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false

LOCAL_SRC_FILES := \
src/vgbuffer.c\
src/decoderc.c \
../main/src/main.c

LOCAL_MODULE := appmpeg4dec.exe

LOCAL_CFLAGS :=  -D_IPP_LINUX 

LOCAL_SHARED_LIBRARIES :=  libcodecmpeg4dec libcodech263dec libmiscgen

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/../misc \
    vendor/marvell/generic/ipplib/include \

 	
LOCAL_MODULE_PATH :=$(LOCAL_PATH)

-include $(PV_TOP)/Android_system_extras.mk

include $(BUILD_EXECUTABLE)
