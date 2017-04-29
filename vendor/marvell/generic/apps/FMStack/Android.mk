TOP_LOCAL_PATH:= $(call my-dir)
MANAGER_PATH:=$(TOP_LOCAL_PATH)/FMManager
SERVICE_PATH:=$(TOP_LOCAL_PATH)/FMService

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

include $(MANAGER_PATH)/Android.mk
include $(SERVICE_PATH)/Android.mk
