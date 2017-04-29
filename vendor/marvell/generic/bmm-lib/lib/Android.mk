LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifeq ($(BOARD_USE_PMEM_TO_SIMULATE_BMM), true)
LOCAL_SRC_FILES := \
	bmm_lib.pmem.c
else
LOCAL_SRC_FILES :=  \
	bmm_lib.c
endif

#
#PLATFORM_SDK_VERSION >= 14, 14 is 4.0.1_r1
#
ifneq ($(findstring $(PLATFORM_SDK_VERSION),1 2 3 4 5 6 7 8 9 10 11 12 13),)
LOCAL_MODULE_TAGS := optional
endif

LOCAL_MODULE := libbmm

LOCAL_SHARED_LIBRARIES := libutils

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

COMPILE_BMM_TEST := true

ifeq ($(COMPILE_BMM_TEST), true)

include $(CLEAR_VARS)

LOCAL_SRC_FILES :=  \
        bmm_test.c

LOCAL_MODULE := bmmtest
#
#PLATFORM_SDK_VERSION >= 14, 14 is 4.0.1_r1
#
ifneq ($(findstring $(PLATFORM_SDK_VERSION),1 2 3 4 5 6 7 8 9 10 11 12 13),)
LOCAL_MODULE_TAGS := debug
endif

LOCAL_SHARED_LIBRARIES += libbmm

include $(BUILD_EXECUTABLE)

endif
