LOCAL_PATH:= $(call my-dir)

#
# libfmradio
#
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	FMRadio.cpp 

LOCAL_C_INCLUDES := \
	$(JNI_H_INCLUDE)

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils 


LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libfmstub

include $(BUILD_STATIC_LIBRARY)


#
# test application
#
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	fmtest.cpp


LOCAL_STATIC_LIBRARIES := \
	libfmstub

LOCAL_SHARED_LIBRARIES := \
        libcutils \
        libutils

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := fmtest

include $(BUILD_EXECUTABLE)
