LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

include external/opencore/Config.mk

LOCAL_SRC_FILES := \
	src/mrvl_omx_config_parser.cpp \
	src/mrvl_video_config_parser.cpp \
	src/mrvl_m4v_config_parser.cpp

LOCAL_COPY_HEADERS := \
	include/mrvl_omx_config_parser.h \
	include/mrvl_video_config_parser.h \
	include/mrvl_m4v_config_parser.h \
	include/mrvl_omx_dev.h

LOCAL_COPY_HEADERS_TO := libipp

LOCAL_PRELINK_MODULE := false

LOCAL_C_INCLUDES :=\
	$(PV_INCLUDES) \
	$(LOCAL_PATH)/include

LOCAL_SHARED_LIBRARIES += libopencore_common libutils libcutils

LOCAL_MODULE := libMrvlOmxConfig

include $(BUILD_SHARED_LIBRARY)
