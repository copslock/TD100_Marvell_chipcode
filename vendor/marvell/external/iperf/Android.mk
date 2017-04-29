LOCAL_PATH:= $(call my-dir)

X_CFLAGS := -Wall -O2 -DHAVE_CONFIG_H

# --- libdast ---------
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libdast
LOCAL_SRC_FILES:= lib/Socket.cpp          \
       lib/SocketAddr.cpp      \
       lib/Thread.cpp          \
       lib/delay.cpp           \
       lib/error.c           \
       lib/gnu_getopt.c      \
       lib/gnu_getopt_long.c \
       lib/signal.c          \
       lib/sockets.c         \
       lib/stdio.c           \
       lib/string.c          \
       lib/tcp_window_size.c 

LOCAL_C_INCLUDES := $(LOCAL_PATH)/lib $(LOCAL_PATH)/cfg
LOCAL_CFLAGS := $(X_CFLAGS) -DANDROID_TARGET
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libdast
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES:= lib/Socket.cpp          \
       lib/SocketAddr.cpp      \
       lib/Thread.cpp          \
       lib/delay.cpp           \
       lib/error.c           \
       lib/gnu_getopt.c      \
       lib/gnu_getopt_long.c \
       lib/signal.c          \
       lib/sockets.c         \
       lib/stdio.c           \
       lib/string.c          \
       lib/tcp_window_size.c 

LOCAL_C_INCLUDES := $(LOCAL_PATH)/lib $(LOCAL_PATH)/cfg
LOCAL_CFLAGS := $(X_CFLAGS)
include $(BUILD_HOST_STATIC_LIBRARY)

# --- iperf -----------
include $(CLEAR_VARS)
LOCAL_MODULE := iperf
LOCAL_SRC_FILES := src/Client.cpp          \
       src/Extractor.cpp       \
       src/Listener.cpp        \
       src/PerfSocket.cpp      \
       src/PerfSocket_TCP.cpp  \
       src/PerfSocket_UDP.cpp  \
       src/Server.cpp          \
       src/Settings.cpp        \
       src/Speaker.cpp         \
       src/Notify.cpp          \
       src/Audience.cpp        \
       src/List.cpp            \
       src/main.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/lib $(LOCAL_PATH)/cfg
LOCAL_STATIC_LIBRARIES := libdast
LOCAL_LDFLAGS := -pthread
LOCAL_CFLAGS := $(X_CFLAGS) -DANDROID_TARGET
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := iperf
LOCAL_SRC_FILES := src/Client.cpp          \
       src/Extractor.cpp       \
       src/Listener.cpp        \
       src/PerfSocket.cpp      \
       src/PerfSocket_TCP.cpp  \
       src/PerfSocket_UDP.cpp  \
       src/Server.cpp          \
       src/Settings.cpp        \
       src/Speaker.cpp         \
       src/Notify.cpp          \
       src/Audience.cpp        \
       src/List.cpp            \
       src/main.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/lib $(LOCAL_PATH)/cfg
LOCAL_STATIC_LIBRARIES := libdast
LOCAL_LDFLAGS := -pthread
LOCAL_CFLAGS := $(X_CFLAGS)
LOCAL_MODULE_TAGS := optional
include $(BUILD_HOST_EXECUTABLE)

