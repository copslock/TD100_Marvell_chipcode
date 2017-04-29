LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= 	\
        gstcnmcodec_plugin.c gcnmcodec_com.c \
	gstcnmdec_mpeg4.c gstcnmdec_h263.c gstcnmdec_h264.c gstcnmdec_wmv3.c gstcnmdec_mpeg2.c gstcnmenc_com.c gstcnmenc_h263.c gstcnmenc_h264.c gstcnmenc_mpeg4.c

LOCAL_SHARED_LIBRARIES :=	\
	libgstreamer-0.10	\
	libgstbase-0.10		\
	libglib-2.0		\
	libgthread-2.0		\
	libgmodule-2.0		\
	libgobject-2.0

LOCAL_SHARED_LIBRARIES += 	\
	libcodeccnmdec \
	libcodeccnmenc \
	libcnm \
	libcnmhal \
	libmiscgen	\
	libpmemhelper

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/$(GST_PLUGINS_PATH)

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE:= libgstcnmcodec

LOCAL_C_INCLUDES := 				\
	$(LOCAL_PATH)				\
        $(LOCAL_PATH)/../include     \
	$(GST_PLUGINS_MARVELL_DEP_INCLUDES)

LOCAL_CFLAGS := \
	-DHAVE_CONFIG_H \
	-DENABLE_MPEG4DEC_CNM_IPP_GSTPLUGIN \
	-DENABLE_H263DEC_CNM_IPP_GSTPLUGIN \
	-DENABLE_H264DEC_CNM_IPP_GSTPLUGIN \
	-DENABLE_MPEG2DEC_CNM_IPP_GSTPLUGIN \
	-DENABLE_WMV3DEC_CNM_IPP_GSTPLUGIN \
	-DENABLE_ENCODER_CNM_IPP_GSTPLUGIN

include $(BUILD_SHARED_LIBRARY)
