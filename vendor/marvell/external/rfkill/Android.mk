LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

#LOCAL_CFLAGS += -DHAVE_CONFIG_H 

LOCAL_SRC_FILES:=rfkill.c

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= rfkill

include $(BUILD_EXECUTABLE)

