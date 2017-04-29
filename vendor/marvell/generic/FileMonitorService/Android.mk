LOCAL_PATH:= $(call my-dir)

#build libFileMonitorService
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	IFileMonitorCallback.cpp \
	IFileMonitorService.cpp \
	FileMonitorService.cpp

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
    libbinder

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libFileMonitorService

include $(BUILD_SHARED_LIBRARY)

#build FileMonitorService
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	Main_FileMonitor.cpp

LOCAL_SHARED_LIBRARIES := \
    libutils \
    libbinder \
    libFileMonitorService

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := FileMonitorService

include $(BUILD_EXECUTABLE)

#build FileMonitorServiceTest
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	FileMonitorServiceTest.cpp

LOCAL_SHARED_LIBRARIES := \
    libutils \
    libbinder \
    libFileMonitorService

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := FileMonitorServiceTest

include $(BUILD_EXECUTABLE)
