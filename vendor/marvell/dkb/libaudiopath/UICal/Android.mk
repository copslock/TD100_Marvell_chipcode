LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
        UICal_Test.c\
	../MarvellAmixer.cpp \
	../UICal_APIs.c

LOCAL_MODULE:= uical_test

LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES += \
        vendor/marvell/generic/libaudio \
        external/alsa-lib/include \
        vendor/marvell/external/alsa/alsa-lib/include

LOCAL_SHARED_LIBRARIES := \
        libasound \
        libcutils

LOCAL_CFLAGS += -D_POSIX_SOURCE

include $(BUILD_EXECUTABLE)
