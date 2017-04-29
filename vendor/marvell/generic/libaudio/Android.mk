LOCAL_PATH:= $(call my-dir)


ifneq ($(BOARD_USES_GENERIC_AUDIO),true)

#build libaudio.so
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	AudioHardware.cpp \

ifeq ($(BOARD_ENABLE_HIFIEQ),true)
LOCAL_SRC_FILES += \
	IPPEqWrapper.c \
	HifiEqCalibration.cpp
endif


LOCAL_C_INCLUDES += \
        vendor/marvell/external/alsa/alsa-lib/include \
        external/alsa-lib/include \
        vendor/marvell/$(TARGET_PRODUCT)/libaudiopath \

ifeq ($(BOARD_ENABLE_HIFIEQ),true)
LOCAL_C_INCLUDES += \
        vendor/marvell/generic/ipplib/include \
        vendor/marvell/generic/FileMonitorService
endif

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libmedia \
    libasound \
    libhardware_legacy \
    libaudiopath \

ifeq ($(BOARD_ENABLE_HIFIEQ),true)
LOCAL_SHARED_LIBRARIES += \
    libmiscgen \
    libippsp \
    libbinder \
    libFileMonitorService
endif

LOCAL_STATIC_LIBRARIES += libaudiointerface

LOCAL_MODULE:= libaudio

ifeq ($(BOARD_HAVE_BLUETOOTH),true)
  LOCAL_SHARED_LIBRARIES += liba2dp
endif

ifeq ($(strip $(BOARD_USES_RECORD_OVER_PCM)),true)
  LOCAL_CFLAGS += -DRECORD_OVER_PCM
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

ifeq ($(BOARD_ENABLE_SRSWITCH),true)
  LOCAL_CFLAGS += -DWITH_SRSWITCH
endif

ifeq ($(strip $(BOARD_USES_SANREMO)), true)
  LOCAL_CFLAGS += -DBOARD_USES_SANREMO
endif

ifeq ($(BOARD_ENABLE_LOOPBACK),true)
  LOCAL_CFLAGS += -DWITH_LOOPBACK
endif

ifeq ($(BOARD_ENABLE_HIFIEQ),true)
  LOCAL_CFLAGS += -DWITH_HIFIEQ
endif

ifeq ($(BOARD_SUPPORT_MEDIA_SET_PROPERTY),true)
  LOCAL_CFLAGS += -DUSE_PROPERTY
endif

ifeq ($(strip $(BOARD_USES_AUDIO_WITH_HDMI)),true)
  LOCAL_CFLAGS += -DAUDIO_WITH_HDMI
endif

ifeq ($(strip $(BOARD_USES_AUDIO_WITH_ABU)),true)
  LOCAL_CFLAGS += -DAUDIO_WITH_ABU
endif

ifeq ($(strip $(ORGAN_VOLUME_CALIBRATION)),true)
LOCAL_CFLAGS += -DVOLUME_CALIBRATION
endif

LOCAL_CFLAGS += -DPLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION) -D_POSIX_SOURCE
ifeq ($(BOARD_ENABLE_HIFIEQ),true)
LOCAL_CFLAGS += -D_IPP_LINUX
endif

include $(BUILD_SHARED_LIBRARY)

#build libaudiopolicy.so
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
        AudioPolicyManager.cpp

LOCAL_C_INCLUDES += \
        vendor/marvell/external/alsa/alsa-lib/include \
        external/alsa-lib/include \
        vendor/marvell/$(TARGET_PRODUCT)/libaudiopath

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libmedia \
    libhardware_legacy \
    libaudiopath

ifneq ($(PLATFORM_SDK_VERSION), 7)
LOCAL_WHOLE_STATIC_LIBRARIES += libaudiopolicybase
endif

LOCAL_MODULE:= libaudiopolicy

ifeq ($(BOARD_HAVE_BLUETOOTH),true)
  LOCAL_CFLAGS += -DWITH_A2DP
endif

ifeq ($(strip $(BOARD_USES_AUDIO_WITH_HDMI)),true)
  LOCAL_CFLAGS += -DAUDIO_WITH_HDMI
endif

ifeq ($(strip $(BOARD_USES_AUDIO_WITH_ABU)),true)
  LOCAL_CFLAGS += -DAUDIO_WITH_ABU
endif

ifeq ($(BOARD_ENABLE_VTOVERBT),true)
  LOCAL_CFLAGS += -DWITH_VTOVERBT
endif

ifeq ($(strip $(ORGAN_VOLUME_CALIBRATION)),true)
LOCAL_CFLAGS += -DVOLUME_CALIBRATION
endif

ifeq ($(strip $(BOARD_USES_RECORD_OVER_PCM)),true)
LOCAL_CFLAGS += -DRECORD_OVER_PCM
endif

LOCAL_CFLAGS += -DPLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION) -D_POSIX_SOURCE

include $(BUILD_SHARED_LIBRARY)
endif
