LOCAL_PATH:= $(call my-dir)

#
# libfmradio
#
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	FMRadio.cpp \
	FMEventParser.cpp \
	IFMRadioService.cpp \
	IFMRadioCallback.cpp \
	Main_FMRadio.cpp \
    utils/FMRingBuffer.cpp

LOCAL_C_INCLUDES := \
        external/bluetooth/bluez/include\
        vendor/marvell/generic/libMarvellWireless\

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libmedia \
        libbinder \
	libMarvellWireless \
        libhardware_legacy

ifeq ($(strip $(FM_NOT_USES_RECORD)),true)
LOCAL_CFLAGS += -DFM_NOT_USES_RECORD
else
LOCAL_C_INCLUDES += vendor/marvell/generic/ipplib/include
LOCAL_SHARED_LIBRARIES += libcodecaacenc libmiscgen
endif

ifeq ($(strip $(BOARD_USES_SANREMO)),true)
LOCAL_CFLAGS += -DBOARD_USES_SANREMO
endif

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := FMRadioServer

include $(BUILD_EXECUTABLE)
