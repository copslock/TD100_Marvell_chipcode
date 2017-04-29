LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := setup_fs.c
LOCAL_MODULE := setup_fs
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES += libcutils libutils
include $(BUILD_EXECUTABLE)

file := $(PRODUCT_OUT)/primary_mbr
$(file) : $(LOCAL_PATH)/primary_mbr | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)

file := $(PRODUCT_OUT)/second_mbr
$(file) : $(LOCAL_PATH)/second_mbr | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)
