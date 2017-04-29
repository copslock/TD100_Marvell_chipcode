LOCAL_PATH := $(my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := mlanevent.exe
LOCAL_SRC_FILES := mlanevent.c
LOCAL_MODULE_TAGS := optional

CONFIG_WFD_SUPPORT=y
ifeq ($(CONFIG_WFD_SUPPORT), y)
LOCAL_CFLAGS += -DWFD_SUPPORT
endif

include $(BUILD_EXECUTABLE)
