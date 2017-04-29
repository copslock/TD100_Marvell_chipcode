LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE:=dhcp_announce

LOCAL_SRC_FILES := dhcp_announce.c activity.cpp

LOCAL_SHARED_LIBRARIES := libcutils liblog libandroid_runtime libbinder libutils

include $(BUILD_EXECUTABLE)
