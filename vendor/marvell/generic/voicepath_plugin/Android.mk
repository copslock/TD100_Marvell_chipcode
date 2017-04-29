LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

#
# Phone PCM Plugin 
#
LOCAL_SRC_FILES:= \
        pcm_ippvp.c 

LOCAL_SHARED_LIBRARIES := \
        libasound \
        libippvp \
        libippsp \
        libmiscgen \
        libcutils 
      

#LOCAL_LDLIBS := -lpthread -ldl
LOCAL_LDLIBS := -lpthread

ifeq ($(shell if [ $(PLATFORM_SDK_VERSION) -ge 9 ]; then echo big9; fi),big9)
LOCAL_MODULE_PATH:= $(TARGET_OUT)/usr/lib/alsa-lib
endif

LOCAL_MODULE:= libasound_module_pcm_ippvp

ifeq ($(shell if [ $(PLATFORM_SDK_VERSION) -ge 9 ]; then echo big9; fi),big9)
LOCAL_C_INCLUDES := \
        external/alsa-lib/include \
        vendor/marvell/generic/libaudio/ \
        vendor/marvell/generic/ipplib/include/ 
else
LOCAL_C_INCLUDES := \
        vendor/marvell/external/alsa/alsa-lib/include \
        vendor/marvell/generic/libaudio/ \
        vendor/marvell/generic/ipplib/include/ 
endif

LOCAL_CFLAGS += -mabi=aapcs-linux -DPIC -D_POSIX_SOURCE

# do not prelink

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := user

include $(BUILD_SHARED_LIBRARY)




include $(CLEAR_VARS)

#
# Phone PCM Plugin 
#
LOCAL_SRC_FILES:= \
        pcm_vtrec.c 

LOCAL_SHARED_LIBRARIES := \
        libasound \
        libcutils 
      

#LOCAL_LDLIBS := -lpthread -ldl
LOCAL_LDLIBS := -lpthread

ifeq ($(shell if [ $(PLATFORM_SDK_VERSION) -ge 9 ]; then echo big9; fi),big9)
LOCAL_MODULE_PATH:= $(TARGET_OUT)/usr/lib/alsa-lib
endif

LOCAL_MODULE:= libasound_module_pcm_vtrec

ifeq ($(shell if [ $(PLATFORM_SDK_VERSION) -ge 9 ]; then echo big9; fi),big9)
LOCAL_C_INCLUDES += \
	external/alsa-lib/include
else
LOCAL_C_INCLUDES += \
        vendor/marvell/external/alsa/alsa-lib/include
endif

LOCAL_CFLAGS += -mabi=aapcs-linux -DPIC -D_POSIX_SOURCE

# do not prelink
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := user

include $(BUILD_SHARED_LIBRARY)


