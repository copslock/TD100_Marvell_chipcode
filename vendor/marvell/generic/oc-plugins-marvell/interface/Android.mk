LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

$(call add-prebuilt-files, ETC, mrvl.cfg)

include $(CLEAR_VARS)

include external/opencore/Config.mk

LOCAL_SRC_FILES := \
 	src/mrvl_omx_interface.cpp 

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libomx_mrvl_sharedlibrary

LOCAL_CFLAGS :=  $(PV_CFLAGS)

LOCAL_ARM_MODE := arm

LOCAL_SHARED_LIBRARIES := libutils libcutils libui libdl

LOCAL_C_INCLUDES := \
 	$(PV_TOP)/extern_libs_v2/khronos/openmax/include \
 	$(PV_INCLUDES)
 	
include $(BUILD_SHARED_LIBRARY)
