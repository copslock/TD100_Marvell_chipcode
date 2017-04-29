LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

file := $(TARGET_OUT_ETC)/permissions/hw-features.xml
$(file) : $(LOCAL_PATH)/hw-features.xml | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)

file := $(TARGET_OUT_ETC)/permissions/sw-features.xml
$(file) : $(LOCAL_PATH)/sw-features.xml | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)

file := $(TARGET_OUT_ETC)/vold.fstab
$(file) : $(LOCAL_PATH)/vold.fstab | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)

file := $(TARGET_OUT_ETC)/media_profiles.xml
$(file) : $(LOCAL_PATH)/media_profiles.xml | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)

file := $(TARGET_OUT_ETC)/FileMonitorService.conf
$(file) : $(LOCAL_PATH)/FileMonitorService.conf | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)

file := $(TARGET_OUT_ETC)/rep.yuv
$(file) : $(LOCAL_PATH)/rep.yuv | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)
