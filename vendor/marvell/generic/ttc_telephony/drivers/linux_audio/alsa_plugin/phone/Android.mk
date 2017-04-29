
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

# Alsa config file
alsa_conf := $(TARGET_OUT)/etc/asound.conf
$(alsa_conf) : $(LOCAL_PATH)/asound.conf | $(ACP)
		$(transform-prebuilt-to-target)
ALL_PREBUILT += $(alsa_conf)

#
# Phone CTL Plugin 
#
LOCAL_SRC_FILES:= \
        ctl_phone.c \
	../../audio_ipc.c 

LOCAL_SHARED_LIBRARIES := \
        libasound \
        libutils \
        libcutils

LOCAL_MODULE:= libasound_module_ctl_phone
LOCAL_MODULE_PATH:= $(TARGET_OUT)/usr/lib/alsa-lib

LOCAL_C_INCLUDES += \
	external/alsa-lib/include \
	vendor/marvell/generic/ttc_telephony/drivers/linux_audio \
	vendor/marvell/generic/ttc_telephony/drivers/linux_telephony/atcmdsrv/inc/

LOCAL_CFLAGS += -mabi=aapcs-linux -DPIC -DBIONIC -D_POSIX_C_SOURCE

# do not prelink
LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)

#
# Phone PCM Plugin 
#
LOCAL_SRC_FILES:= \
        pcm_phone.c \
	../../audio_ipc.c \
	../../audio_timer.c \
	../../audio_ringbuf.c

LOCAL_SHARED_LIBRARIES := \
        libasound \
        libutils \
        libcutils

LOCAL_MODULE:= libasound_module_pcm_phone
LOCAL_MODULE_PATH:= $(TARGET_OUT)/usr/lib/alsa-lib

LOCAL_C_INCLUDES += \
	external/alsa-lib/include

LOCAL_CFLAGS += -mabi=aapcs-linux -DPIC -DBIONIC -D_POSIX_C_SOURCE
# do not prelink
LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)


