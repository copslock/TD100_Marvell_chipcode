LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	lib/dlist.c \
	lib/sysfs_attr.c \
	lib/sysfs_bus.c \
	lib/sysfs_class.c \
	lib/sysfs_device.c \
	lib/sysfs_driver.c \
	lib/sysfs_module.c \
	lib/sysfs_utils.c

WARNINGS := -Wall -Wchar-subscripts -Wpointer-arith -Wsign-compare
WARNINGS += $(call cc-supports,-Wno-pointer-sign)
WARNINGS += $(call cc-supports,-Wdeclaration-after-statement)
WARNINGS += -Wshadow

# use '-Os' optimization if available, else use -O2
OPTIMIZATION := $(call cc-supports,-Os,-O2)

LOCAL_CFLAGS += $(WARNINGS)
LOCAL_CFLAGS += $(OPTIMIZATION) -fomit-frame-pointer
LOCAL_CFLAGS += -D_POSIX_SOURCE -D_GNU_SOURCE

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/lib
#
# define LOCAL_PRELINK_MODULE to false to not use pre-link map
#
LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libsysfs
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)
