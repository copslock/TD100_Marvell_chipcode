LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)


LOCAL_CFLAGS +=  -D_IPP_LINUX

LOCAL_SRC_FILES := \
src/jdexample.c \
src/dib.c \
src/jpqueue.c \
../main/src/main.c

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := appjpegdec.exe

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES :=  libcodecjpegdec libmiscgen

LOCAL_C_INCLUDES := \
    ../../include \
    ../misc \
    vendor/marvell/generic/ipplib/include \

include $(BUILD_EXECUTABLE)




