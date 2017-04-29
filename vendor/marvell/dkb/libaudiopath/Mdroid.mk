LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
        AudioPath.c \
	MarvellAmixer.cpp \
	UICal_APIs.c

LOCAL_SHARED_LIBRARIES := \
        libasound \
        libutils \
        libdl \
        libcutils \
        libhardware_legacy


LOCAL_MODULE:= libaudiopath

LOCAL_C_INCLUDES += \
        vendor/marvell/generic/libaudio \
        external/alsa-lib/include \
        vendor/marvell/external/alsa/alsa-lib/include

LOCAL_CPPFLAGS += -mabi=aapcs-linux

ifeq ($(BOARD_HAVE_BLUETOOTH),true)
  LOCAL_CFLAGS += -DWITH_A2DP
endif

ifeq ($(BOARD_ENABLE_VTOVERBT),false)
ifeq ($(BOARD_ENABLE_ECPATH),true)
  LOCAL_CFLAGS += -DWITH_ECPATH
endif
endif

ifeq ($(BOARD_ENABLE_VTOVERBT),true)
  LOCAL_CFLAGS += -DWITH_VTOVERBT
endif

ifeq ($(BOARD_ENABLE_BLUETOOTH_NREC),true)
  LOCAL_CFLAGS += -DWITH_NREC
endif

LOCAL_CFLAGS += -D_POSIX_SOURCE

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

