LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../ \
	$(LOCAL_PATH)/../android \
        external/glib                   \
        external/glib/android           \
        external/glib/glib              \
        external/glib/gmodule           \
        external/glib/gobject           \
        external/glib/gthread           \
        $(TARGET_OUT_HEADERS)/glib

LOCAL_SRC_FILES := get.c

LOCAL_SHARED_LIBRARIES += \
	libsoup

LOCAL_MODULE := souptest

include $(BUILD_EXECUTABLE)
