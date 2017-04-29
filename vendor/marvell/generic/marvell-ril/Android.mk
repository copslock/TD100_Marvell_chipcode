# Copyright 2006 The Android Open Source Project

#/******************************************************************************
#*(C) Copyright 2008 Marvell International Ltd.
#* All Rights Reserved
#******************************************************************************/

# XXX using libutils for simulator build only...
#
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    marvell-ril.c \
    ril-cc.c \
    ril-mm.c \
    ril-ps.c \
    ril-ss.c \
    ril-msg.c \
    ril-sim.c \
    ril-dev.c \
    atchannel.c \
    dataapi.c \
    misc.c \
    work-queue.c \
    ril-requestdatahandler.c \
    at_tok.c

LOCAL_SHARED_LIBRARIES := \
	libcutils libutils libril libnetutils

	# for asprinf
LOCAL_CFLAGS := -D_GNU_SOURCE 

LOCAL_CFLAGS += -DPLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)

ifeq ($(MARVELL_EXTENDED_RIL_REQUSTS),true)
LOCAL_CFLAGS += -DMARVELL_EXTENDED
endif

ifeq ($(HAS_TD_MODEM),true)
LOCAL_CFLAGS += -DBROWNSTONE_CP
else
ifeq ($(HAS_WUKONG_MODEM),true)
LOCAL_CFLAGS += -DDKB_WUKONG
else
LOCAL_CFLAGS += -DDKB_CP
endif
endif

LOCAL_C_INCLUDES += $(KERNEL_HEADERS) $(TOP)/hardware/ril/libril/

#build shared library
LOCAL_SHARED_LIBRARIES += \
	libcutils libutils
LOCAL_LDLIBS += -lpthread
LOCAL_CFLAGS += -DRIL_SHLIB
LOCAL_MODULE:= libmarvell-ril
include $(BUILD_SHARED_LIBRARY)
