#
# Generate share lib for sirf provided libs
#

LOCAL_PATH:= $(call my-dir)

# copy sirf shared library file
#PRODUCT_COPY_FILES += \
        $(LOCAL_PATH)/gsd4t_lib/liblsm_gsd4t.so:system/lib/liblsm_gsd4t.so

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/../include


LOCAL_SRC_FILES:= \
		cpaclient.c \
		agpsrilif.c \
		mtelif_lib.c \
		at_tok.c

#LOCAL_SRC_FILES:=

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils

LOCAL_CFLAGS := -D_GNU_SOURCE

LOCAL_CFLAGS += -fno-short-enums

LOCAL_CFLAGS += -DPLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)

#LOCAL_STATIC_LIBRARIES += \
	libsirf_lsm \
	libsirf_lpl_gsd3tw \
	libsirf_nav \
	libsirf_pal \
	libsirf_rrlp_v5_12 \
	libsirf_supl_v1 \
	libsirf_rtper \
	libsirf_rt 


LOCAL_LDLIBS += -lpthread 

LOCAL_MODULE := libmtelif
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
# Install the file liblsm_gsd4t.so in the target image
include $(CLEAR_VARS)
local_target_dir := $(TARGET_OUT)/lib
ifeq (1,$(shell expr $$(echo $(PLATFORM_SDK_VERSION) | sed "s/[^0-9]//") \< 9))
local_src_dir := froyo
else
local_src_dir := gingerbread
endif
LOCAL_MODULE := liblsm_gsd4t.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := LIB
LOCAL_MODULE_PATH := $(local_target_dir)
LOCAL_SRC_FILES := gsd4t_lib/$(local_src_dir)/$(LOCAL_MODULE)
include $(BUILD_PREBUILT)

# Install the file sirfgps.conf in the target image
include $(CLEAR_VARS)
local_target_dir := $(TARGET_OUT)/etc
LOCAL_MODULE := sirfgps.conf
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(local_target_dir)
LOCAL_SRC_FILES := gsd4t_cfg/google_supl_srv/$(LOCAL_MODULE)
include $(BUILD_PREBUILT)

# Install the file csr_sirf.conf in the target image
include $(CLEAR_VARS)
local_target_dir := $(TARGET_OUT)/etc
LOCAL_MODULE := csr_gps.conf
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(local_target_dir)
LOCAL_SRC_FILES := gsd4t_cfg/google_supl_srv/$(LOCAL_MODULE)
include $(BUILD_PREBUILT)

# Install the file csr_sirf.conf in the target image
include $(CLEAR_VARS)
local_target_dir := $(TARGET_OUT)/etc
LOCAL_MODULE := AGPS_CA_CMCC.pem
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(local_target_dir)
LOCAL_SRC_FILES := gsd4t_cfg/cmcc_supl_srv/$(LOCAL_MODULE)
include $(BUILD_PREBUILT)
