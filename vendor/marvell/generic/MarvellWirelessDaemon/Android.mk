LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \

LOCAL_SRC_FILES := \
        marvell_wireless_daemon.c

ifeq ($(shell if [ $(PLATFORM_SDK_VERSION) -ge 9 ]; then echo big9; fi),big9)
LOCAL_C_INCLUDES := \
        external/bluetooth/bluez/lib
else
LOCAL_C_INCLUDES := \
        external/bluetooth/bluez/include
endif

LOCAL_SHARED_LIBRARIES := \
    libc\
    libcutils \
    libutils \
    libnetutils\
    libbluetooth

LOCAL_MODULE:=MarvellWirelessDaemon
ifdef SD8787_NEED_CALIBRATE
LOCAL_CFLAGS += -DSD8787_NEED_CALIBRATE
endif
include $(BUILD_EXECUTABLE)
