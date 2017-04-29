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
	$(LOCAL_PATH)/src \
	$(LOCAL_PATH)/../cpufrequtils/lib

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
	$(common_C_INCLUDES)

LOCAL_SRC_FILES:= \
	src/config_parser.c \
	src/cpufreqd_log.c \
	src/cpufreq_utils.c \
	src/daemon_utils.c \
	src/list.c \
	src/main.c \
	src/plugin_utils.c \
	src/sock_utils.c

LOCAL_SHARED_LIBRARIES := libcpufreq libdl
LOCAL_CFLAGS := $(common_CFLAGS)
LOCAL_LDFLAGS := -Wl,--export-dynamic

LOCAL_MODULE_PATH := $(TARGET_OUT_EXECUTABLES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := cpufreqd
include $(BUILD_EXECUTABLE)

# the plugin of cpu
# =====================================================
include $(CLEAR_VARS)
LOCAL_MODULE := cpufreqd_cpu
LOCAL_C_INCLUDES := \
	$(common_C_INCLUDES)

LOCAL_SRC_FILES:= \
	src/$(LOCAL_MODULE).c
LOCAL_CFLAGS := $(common_CFLAGS)
LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
LOCAL_LDFLAGS := -Wl,--allow-shlib-undefined

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/cpufreqd
LOCAL_UNSTRIPPED_PATH := $(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/cpufreqd
include $(BUILD_SHARED_LIBRARY)

# the plugin of exec
# =====================================================
include $(CLEAR_VARS)
LOCAL_MODULE := cpufreqd_exec
LOCAL_C_INCLUDES := \
	$(common_C_INCLUDES)

LOCAL_SRC_FILES:= \
	src/$(LOCAL_MODULE).c
LOCAL_CFLAGS := $(common_CFLAGS)

LOCAL_LDLIBS := -lpthread
LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
LOCAL_LDFLAGS := -Wl,--allow-shlib-undefined

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/cpufreqd
LOCAL_UNSTRIPPED_PATH := $(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/cpufreqd
include $(BUILD_SHARED_LIBRARY)

# the plugin of governor parameters
# =====================================================
include $(CLEAR_VARS)
LOCAL_MODULE := cpufreqd_governor_parameters
LOCAL_C_INCLUDES := \
	$(common_C_INCLUDES) \
	$(LOCAL_PATH)/../sysfsutils/

LOCAL_SRC_FILES:= \
	src/$(LOCAL_MODULE).c
LOCAL_CFLAGS := $(common_CFLAGS)

LOCAL_SHARED_LIBRARIES := libsysfs
LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
LOCAL_LDFLAGS := -Wl,--allow-shlib-undefined

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/cpufreqd
LOCAL_UNSTRIPPED_PATH := $(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/cpufreqd
include $(BUILD_SHARED_LIBRARY)

# the plugin of programs
# =====================================================
include $(CLEAR_VARS)
LOCAL_MODULE := cpufreqd_programs
LOCAL_C_INCLUDES := \
	$(common_C_INCLUDES)

LOCAL_SRC_FILES:= \
	src/$(LOCAL_MODULE).c
LOCAL_CFLAGS := $(common_CFLAGS)

LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
LOCAL_LDFLAGS := -Wl,--allow-shlib-undefined

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/cpufreqd
LOCAL_UNSTRIPPED_PATH := $(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/cpufreqd
include $(BUILD_SHARED_LIBRARY)

# the plugin of acpi
# =====================================================
include $(CLEAR_VARS)
LOCAL_MODULE := cpufreqd_acpi
LOCAL_C_INCLUDES := \
	$(common_C_INCLUDES) \
	$(LOCAL_PATH)/../sysfsutils/

LOCAL_SRC_FILES:= \
	src/$(LOCAL_MODULE).c \
	src/$(LOCAL_MODULE)_ac.c \
	src/$(LOCAL_MODULE)_battery.c \
	src/$(LOCAL_MODULE)_temperature.c \
	src/$(LOCAL_MODULE)_event.c

LOCAL_CFLAGS := $(common_CFLAGS)

LOCAL_SHARED_LIBRARIES := libsysfs
LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
LOCAL_LDFLAGS := -Wl,--allow-shlib-undefined

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/cpufreqd
LOCAL_UNSTRIPPED_PATH := $(TARGET_OUT_SHARED_LIBRARIES_UNSTRIPPED)/cpufreqd
include $(BUILD_SHARED_LIBRARY)

# the util of getspeed
# =====================================================
include $(CLEAR_VARS)

LOCAL_CFLAGS := $(common_CFLAGS)
LOCAL_C_INCLUDES := \
	$(common_C_INCLUDES)

LOCAL_SRC_FILES:= \
	utils/getspeed.c

LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := cpufreqd-get
include $(BUILD_EXECUTABLE)

# the util of setspeed
# =====================================================
include $(CLEAR_VARS)

LOCAL_CFLAGS := $(common_CFLAGS)
LOCAL_C_INCLUDES := \
	$(common_C_INCLUDES)

LOCAL_SRC_FILES:= \
	utils/setspeed.c

LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := cpufreqd-set
include $(BUILD_EXECUTABLE)
