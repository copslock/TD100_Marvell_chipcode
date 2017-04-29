LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	com_marvell_organ_NVMFileTool.cpp

LOCAL_C_INCLUDES := \
	$(JNI_H_INCLUDE)

LOCAL_SHARED_LIBRARIES := \
        libcutils \
	libutils \
	libui \
    	libandroid_runtime
	
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libOrgan

include $(BUILD_SHARED_LIBRARY)

