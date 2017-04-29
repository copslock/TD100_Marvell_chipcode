LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= 	\
        gstamrnbenc.c

LOCAL_SHARED_LIBRARIES :=	\
	libgstreamer-0.10	\
	libgstbase-0.10		\
	libglib-2.0		\
	libgthread-2.0		\
	libgmodule-2.0		\
	libgobject-2.0

LOCAL_SHARED_LIBRARIES += 	\
	libcodecamrnbenc \
	libmiscgen

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/$(GST_PLUGINS_PATH)

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE:= libgstippamrnbenc

LOCAL_C_INCLUDES := 				\
	$(LOCAL_PATH)				\
        $(LOCAL_PATH)/../../include     \
	$(GST_PLUGINS_MARVELL_DEP_INCLUDES)

LOCAL_CFLAGS := \
	-DHAVE_CONFIG_H			

include $(BUILD_SHARED_LIBRARY)
