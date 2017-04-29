LOCAL_PATH:=$(call my-dir)


include $(CLEAR_VARS)

#
# libippexif helper example
#


# put your source files here.
LOCAL_SRC_FILES:= exif/exif_generator.c 


LOCAL_CFLAGS +=   -I vendor/marvell/generic/cameraengine2/tool/include \
                  -I vendor/marvell/generic/ipplib/include

#    -mabi=aapcs-linux
					
# put static libraies used here
LOCAL_STATIC_LIBRARIES+= libippexif

# put shared objects used here
LOCAL_SHARED_LIBRARIES+=libcodecjpegdec \
                        libcodecjpegenc \
						libmiscgen \
						libutils  

# put your module name here
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:=MrvlExifGenerator

include $(BUILD_EXECUTABLE)

