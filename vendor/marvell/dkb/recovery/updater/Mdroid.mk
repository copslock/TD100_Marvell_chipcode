ifneq ($(TARGET_SIMULATOR),true)
ifeq ($(TARGET_ARCH),arm)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# librecovery_update_mrvl is a set of edify extension functions for
# doing updates on Marvell MMP2 devices.
LOCAL_SRC_FILES := recovery_updater.c
LOCAL_STATIC_LIBRARIES += libmtdutils
LOCAL_C_INCLUDES += bootable/recovery
LOCAL_MODULE := librecovery_updater_mrvl

LOCAL_MODULE_TAGS := optional
include $(BUILD_STATIC_LIBRARY)

endif   # TARGET_ARCH == arm
endif   # !TARGET_SIMULATOR
