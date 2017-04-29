#######################
# 1) dhcpcd.conf
#######################

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

dhcpcd_conf := $(TARGET_OUT_ETC)/dhcpcd/dhcpcd.conf

$(dhcpcd_conf) : $(LOCAL_PATH)/dhcpcd.conf | $(ACP)
	$(transform-prebuilt-to-target)

ALL_PREBUILT += $(dhcpcd_conf)

#######################
# 2) wpa_supplicant.conf
#######################

local_target_dir := $(TARGET_OUT)/etc/wifi

include $(CLEAR_VARS)
LOCAL_MODULE := wpa_supplicant.conf
LOCAL_MODULE_TAGS := user
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(local_target_dir)
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)


