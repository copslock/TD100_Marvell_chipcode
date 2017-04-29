LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES:= \
	com_marvell_powersetting_PowerFreq.cpp

LOCAL_C_INCLUDES := \
	$(JNI_H_INCLUDE) \
	$(LOCAL_PATH)/../../../external/cpufrequtils/lib \
	$(LOCAL_PATH)/../../../external/cpufreqd/src

LOCAL_SHARED_LIBRARIES := \
        libcutils \
	libutils \
	libui \
    	libandroid_runtime \
	libcpufreq
	
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libPowerSetting

include $(BUILD_SHARED_LIBRARY)

