#
# Generate share lib for sirf provided libs
#

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../include


LOCAL_SRC_FILES:= mtelif_test.c

#LOCAL_SRC_FILES:=

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libmtelif

LOCAL_CFLAGS := -D_GNU_SOURCE

LOCAL_CFLAGS += -fno-short-enums

#LOCAL_STATIC_LIBRARIES += \
	libsirf_lsm \
	libsirf_lpl_gsd3tw \
	libsirf_nav \
	libsirf_pal \
	libsirf_rrlp_v5_12 \
	libsirf_supl_v1 \
	libsirf_rtper \
	libsirf_rt 


LOCAL_LDLIBS += -lpthread 

LOCAL_MODULE := mtelif_test
LOCAL_PRELINK_MODULE := false

include $(BUILD_EXECUTABLE)
