LOCAL_PATH := $(call my-dir)

OBJS_simal = simal.c citty.c sim_log.c
OBJS_libsimal = simal_api.c
LIBS += -lpthread 

include $(CLEAR_VARS)
LOCAL_MODULE := simal
LOCAL_MODULE_TAGS := optional
LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := $(L_CFLAGS)
LOCAL_SRC_FILES := $(OBJS_simal)
LOCAL_C_INCLUDES := $(INCLUDES)
LOCAL_SHARED_LIBRARIES := libc libcutils 
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE = libsimal
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_SRC_FILES = $(OBJS_libsimal)
LOCAL_C_INCLUDES = $(INCLUDES)
LOCAL_SHARED_LIBRARIES := libc libcutils 
LOCAL_COPY_HEADERS_TO := libsimal
LOCAL_COPY_HEADERS := simal.h
include $(BUILD_SHARED_LIBRARY)
