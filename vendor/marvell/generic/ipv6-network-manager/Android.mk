

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:=                                      \
                  main.cpp                             \
                  RouteNetlinkManager.cpp                   \
                  RouteNetlinkHandler.cpp 

LOCAL_MODULE:= ipv6-network-manager

#LOCAL_C_INCLUDES := $(KERNEL_HEADERS) \

LOCAL_CFLAGS :=


LOCAL_SHARED_LIBRARIES := libsysutils libcutils libnetutils

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)


