LOCAL_PATH:= $(call my-dir)

#build BatteryCalibration 
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	BatteryCalibration.cpp

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../FileMonitorService

LOCAL_SHARED_LIBRARIES := \
    libFileMonitorService \
	libbinder \
	liblog \
	libutils\
	libcutils


LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := BatteryCalibration

include $(BUILD_EXECUTABLE)
