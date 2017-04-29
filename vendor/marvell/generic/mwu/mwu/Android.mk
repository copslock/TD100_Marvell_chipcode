LOCAL_PATH:= $(call my-dir)

# mwu (formerly wfdd).  The main program is in the process of being refactored
# to permit access to a number of different marvell wireless utilities (mwu).
include $(CLEAR_VARS)
LOCAL_MODULE:=mwu

CONFIG_STDOUT_DEBUG=y
CONFIG_UAP_SUPPORT=y
CONFIG_WPS_IE_NEEDED=n
CONFIG_BIG_ENDIAN=n
CONFIG_WIFIDIR_SUPPORT=y
CONFIG_WSC2_SUPPORT=y
CONFIG_WSC2_PF_BUILD=y
CONFIG_OS_LINUX=y

ifeq ($(CONFIG_STDOUT_DEBUG), y)
COMMON_CFLAGS += -DSTDOUT_DEBUG
endif

ifeq ($(CONFIG_WSC2_SUPPORT), y)
COMMON_CFLAGS += -DWSC2
COMMON_CFLAGS += -DEAP_FRAG

ifeq ($(CONFIG_WSC2_PF_BUILD), y)
COMMON_CFLAGS += -DWSC2_PF
endif

endif

ifeq ($(CONFIG_WPSE_SUPPORT), y)
COMMON_CFLAGS += -DWPSE_SUPPORT
endif

ifeq ($(CONFIG_WIFIDIR_SUPPORT), y)
COMMON_CFLAGS += -DWIFIDIR_SUPPORT
ifeq ($(CONFIG_WIFIDIR_SIGMA_SUPPORT), y)
COMMON_CFLAGS += -DWIFIDIR_SIGMA_CAPI
endif
ENCRYPT_LIB=$(ENCRYPT_INC)encrypt_esupp_pxa.a
endif

ifeq ($(CONFIG_UAP_SUPPORT), y)
COMMON_CFLAGS += -DUAP_SUPPORT
endif

ifeq ($(CONFIG_WPS_IE_NEEDED), y)
COMMON_CFLAGS += -DWPS_IE_NEEDED
endif

ifeq ($(CONFIG_BIG_ENDIAN), y)
COMMON_CFLAGS += -DBIG_ENDIAN
endif

ifeq ($(CONFIG_OS_LINUX), y)
COMMON_CFLAGS += -DOS_LINUX
endif

LOCAL_CFLAGS += -DWIFIDIR_UI_SERVER
LOCAL_CFLAGS += -DMWU_SERVER
LOCAL_CFLAGS += $(COMMON_CFLAGS)

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/../os/include \
	$(LOCAL_PATH)/../ \
	$(LOCAL_PATH)/../mwu \
	$(LOCAL_PATH)/../mwu_test \
	$(LOCAL_PATH)/../wifidir \
	$(LOCAL_PATH)/../mwpamod \
	$(LOCAL_PATH)/../mwpsmod \
	$(LOCAL_PATH)/../wsc/encrypt_src

LOCAL_SRC_FILES := \
	../wps_main.c \
	mwu_main.c \
	mwu.c \
	../mwu_test/mwu_test.c \
	../wifidir/wifidir_mwu.c \
	../wifidir/wifidir.c \
	../mwpamod/mwpamod_mwu.c \
	../mwpamod/mwpamod_os.c \
	../mwpamod/mwpamod.c \
	../mwpsmod/mwpsmod_mwu.c \
	../mwpsmod/mwpsmod.c

ifeq ($(CONFIG_OS_LINUX), y)
LOCAL_SRC_FILES += ../os/linux/mwu_ioctl.c \
	../os/linux/wps_os.c \
	../os/linux/wps_wlan.c \
	../os/linux/wps_l2.c \
        ../os/linux/mwu_timer.c

ifeq ($(CONFIG_STDOUT_DEBUG), y)
LOCAL_SRC_FILES += ../os/linux/mwu_log.c
endif

endif

LOCAL_SHARED_LIBRARIES := \
	libcrypto \
	libssl \
	libcutils

LOCAL_STATIC_LIBRARIES   := \
	libwsc \
	libwifidir \
	encrypt_pxa

include $(BUILD_EXECUTABLE)

# libmwu_client
include $(CLEAR_VARS)
LOCAL_MODULE:=libmwu_client

LOCAL_C_INCLUDES += \
       $(LOCAL_PATH)/. \
       $(LOCAL_PATH)/../os/include \
       $(LOCAL_PATH)/../ \
       $(LOCAL_PATH)/../wsc/encrypt_src

LOCAL_CFLAGS :=

ifeq ($(CONFIG_STDOUT_DEBUG), y)
LOCAL_CFLAGS += -DSTDOUT_DEBUG
endif

LOCAL_SRC_FILES := mwu.c

ifeq ($(CONFIG_OS_LINUX), y)
LOCAL_CFLAGS += -DOS_LINUX

ifeq ($(CONFIG_STDOUT_DEBUG), y)
LOCAL_SRC_FILES += ../os/linux/mwu_log.c
endif

endif

LOCAL_SHARED_LIBRARIES   := libc \
	libcutils

LOCAL_PRELINK_MODULE := false

LOCAL_COPY_HEADERS_TO := libmwu_client
LOCAL_COPY_HEADERS := mwu.h

include $(BUILD_SHARED_LIBRARY)

# mwu_cli
include $(CLEAR_VARS)
LOCAL_MODULE:=mwu_cli

LOCAL_CFLAGS :=

ifeq ($(CONFIG_STDOUT_DEBUG), y)
LOCAL_CFLAGS += -DSTDOUT_DEBUG
endif

ifeq ($(CONFIG_OS_LINUX), y)
LOCAL_CFLAGS += -DOS_LINUX
endif

LOCAL_C_INCLUDES += \
       $(LOCAL_PATH)/. \
       $(LOCAL_PATH)/../os/include \
       $(LOCAL_PATH)/../ \
       $(LOCAL_PATH)/../wsc/encrypt_src

LOCAL_SRC_FILES := mwu_cli.c

LOCAL_PREBUILT_LIBS := $(LOCAL_PATH)/$(ENCRYPT_LIB)

LOCAL_SHARED_LIBRARIES   := libc \
	libcutils \
	libmwu_client

include $(BUILD_EXECUTABLE)

include $(call all-makefiles-under,$(LOCAL_PATH))
