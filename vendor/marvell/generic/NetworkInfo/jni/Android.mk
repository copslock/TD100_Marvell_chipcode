LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
        atchannel.c \
        misc.c \
	at_tok.c \
	com_marvell_networkinfo_NetworkInfoService.cpp 


LOCAL_C_INCLUDES := \
	$(JNI_H_INCLUDE)

LOCAL_SHARED_LIBRARIES := \
        libcutils \
	libutils \
	libandroid_runtime
	
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libNetworkInfo

include $(BUILD_SHARED_LIBRARY)

