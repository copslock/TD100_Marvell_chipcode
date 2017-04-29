LOCAL_PATH:= $(call my-dir)

common_CFLAGS := -O2 -W -Wall -Wshadow -W -Wpointer-arith \
	-Wcast-qual -Wcast-align -Wwrite-strings \
	-Wconversion -Wstrict-prototypes -Wmissing-prototypes \
	-Wmissing-declarations -Wredundant-decls \
	-Wnested-externs -Winline -Wundef -Wbad-function-cast \
	-Waggregate-return \
	-std=c99

common_CFLAGS += -D_POSIX_SOURCE -D_GNU_SOURCE
common_CFLAGS += -DHAVE_CONFIG_H

common_C_INCLUDES := \
	$(LOCAL_PATH)/../../external/cpufreqd/ \
	$(LOCAL_PATH)/../../external/cpufreqd/src \
	$(LOCAL_PATH)/../../external/cpufrequtils/lib \

# the plugin of android
# =====================================================
include $(CLEAR_VARS)
LOCAL_MODULE := cpufreqd_android
LOCAL_C_INCLUDES := \
	$(common_C_INCLUDES)

LOCAL_SRC_FILES:= \
        $(LOCAL_MODULE).c
LOCAL_CFLAGS := $(common_CFLAGS)

LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
LOCAL_LDFLAGS := -Wl,--allow-shlib-undefined

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/cpufreqd
LOCAL_UNSTRIPPED_PATH := $(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/cpufreqd
include $(BUILD_SHARED_LIBRARY)

# the plugin of zsp
# =====================================================
include $(CLEAR_VARS)
LOCAL_MODULE := cpufreqd_zsp
LOCAL_C_INCLUDES := \
	$(common_C_INCLUDES) \
	$(LOCAL_PATH)/../../external/sysfsutils/

LOCAL_SRC_FILES:= \
        $(LOCAL_MODULE).c
LOCAL_CFLAGS := $(common_CFLAGS)

LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
LOCAL_LDFLAGS := -Wl,--allow-shlib-undefined

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/cpufreqd
LOCAL_UNSTRIPPED_PATH := $(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/cpufreqd
include $(BUILD_SHARED_LIBRARY)

# the plugin of input event
# =====================================================
include $(CLEAR_VARS)
LOCAL_MODULE := cpufreqd_input
LOCAL_C_INCLUDES := \
	$(common_C_INCLUDES) \
	$(LOCAL_PATH)/../../external/sysfsutils/

LOCAL_SRC_FILES:= \
        $(LOCAL_MODULE).cpp
#LOCAL_CFLAGS := $(common_CFLAGS)

LOCAL_SHARED_LIBRARIES := \
        libui \
	libutils

LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
LOCAL_LDFLAGS := -Wl,--allow-shlib-undefined

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/cpufreqd
LOCAL_UNSTRIPPED_PATH := $(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/cpufreqd
include $(BUILD_SHARED_LIBRARY)

include $(LOCAL_PATH)/java/Android.mk
