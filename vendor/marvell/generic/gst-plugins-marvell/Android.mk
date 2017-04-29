LOCAL_PATH := $(call my-dir)

GST_PLUGINS_MARVELL_TOP := $(LOCAL_PATH)

GST_MAJORMINOR := 0.10

GST_PLUGINS_PATH := gst

GST_PLUGINS_MARVELL_DEP_INCLUDES := \
	external/icu4c/common	\
        vendor/marvell/generic/ipplib/include \
        external/gst-plugins-base/gst-libs      \
        external/gstreamer              \
        external/gstreamer/android      \
        external/gstreamer/gst/android  \
        external/gstreamer/libs         \
        external/gstreamer/gst          \
        external/glib                   \
        external/glib/android           \
        external/glib/glib              \
        external/glib/gmodule           \
        external/glib/gobject           \
        external/glib/gthread           \
        external/libxml2/include        \
	$(TARGET_OUT_HEADERS)/gstreamer \
	$(TARGET_OUT_HEADERS)/glib

include $(CLEAR_VARS)

include $(GST_PLUGINS_MARVELL_TOP)/src/ippACelements/aacdec/Android.mk
include $(GST_PLUGINS_MARVELL_TOP)/src/ippACelements/mp3dec/Android.mk
include $(GST_PLUGINS_MARVELL_TOP)/src/ippACelements/aacenc/Android.mk

include $(GST_PLUGINS_MARVELL_TOP)/src/ippVCelements/h263dec/Android.mk
include $(GST_PLUGINS_MARVELL_TOP)/src/ippVCelements/h264dec/Android.mk
include $(GST_PLUGINS_MARVELL_TOP)/src/ippVCelements/mpeg4dec/Android.mk
include $(GST_PLUGINS_MARVELL_TOP)/src/ippVCelements/mpeg2dec/Android.mk

include $(GST_PLUGINS_MARVELL_TOP)/src/ippWMelements/wmadec/Android.mk
include $(GST_PLUGINS_MARVELL_TOP)/src/ippWMelements/wmvdec/Android.mk

include $(GST_PLUGINS_MARVELL_TOP)/src/ippSPelements/amrnbdec/Android.mk
include $(GST_PLUGINS_MARVELL_TOP)/src/ippSPelements/amrwbdec/Android.mk
include $(GST_PLUGINS_MARVELL_TOP)/src/ippSPelements/amrnbenc/Android.mk
include $(GST_PLUGINS_MARVELL_TOP)/src/ippSPelements/amrwbenc/Android.mk

include $(GST_PLUGINS_MARVELL_TOP)/src/ippJPelements/mjpegdec/Android.mk

include $(GST_PLUGINS_MARVELL_TOP)/src/cnm/Android.mk

include $(GST_PLUGINS_MARVELL_TOP)/src/ippcnmassist/Android.mk
