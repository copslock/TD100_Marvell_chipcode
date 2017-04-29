# this file is for board dependent configurations

ifeq ($(TARGET_PRODUCT), saarbmg1)
LOCAL_CFLAGS    += -D MG1SAARB
LOCAL_SHARED_LIBRARIES:=libcodecjpegenc
ENABLE_VIDEO_ENCODER = n   #for sample application
endif

ifeq ($(TARGET_PRODUCT), evbnevo)
LOCAL_CFLAGS    += -D MG1SAARB
LOCAL_SHARED_LIBRARIES:=libcodecjpegenc
ENABLE_VIDEO_ENCODER = n   #for sample application
endif

ifeq ($(TARGET_PRODUCT), saarcnevo)
LOCAL_CFLAGS    += -D MG1SAARB
LOCAL_SHARED_LIBRARIES:=libcodecjpegenc
ENABLE_VIDEO_ENCODER = n   #for sample application
endif

ifeq ($(TARGET_PRODUCT), dkbnevo)
LOCAL_CFLAGS    += -D MG1SAARB
LOCAL_SHARED_LIBRARIES:=libcodecjpegenc
ENABLE_VIDEO_ENCODER = n   #for sample application
endif

ifeq ($(TARGET_PRODUCT), jasper)
LOCAL_CFLAGS    += -D BONNELL
LOCAL_SHARED_LIBRARIES:=libcodecjpegenc
ENABLE_VIDEO_ENCODER = n   #for sample application
endif

ifeq ($(TARGET_PRODUCT), brownstone)
LOCAL_CFLAGS    += -D BROWNSTONE
LOCAL_SHARED_LIBRARIES:=libcodecjpegenc
ENABLE_VIDEO_ENCODER = n   #for sample application
endif

ifeq ($(TARGET_PRODUCT), g50)
LOCAL_CFLAGS    += -D G50
LOCAL_SHARED_LIBRARIES:=libcodecjpegenc
ENABLE_VIDEO_ENCODER = n   #for sample application
endif

ifeq ($(TARGET_PRODUCT), abilene)
LOCAL_CFLAGS    += -D ABILENE
LOCAL_SHARED_LIBRARIES:=libcodecjpegenc
ENABLE_VIDEO_ENCODER = n   #for sample application
endif

################################
## can not descriminate between ttc and td, hence we always build as td
################################
ifeq ($(TARGET_PRODUCT), dkb)
LOCAL_CFLAGS    += -D TDDKB
LOCAL_SHARED_LIBRARIES:=libcodecjpegenc
ENABLE_VIDEO_ENCODER = n   #for sample application
endif

ifeq ($(TARGET_PRODUCT), evbmg1)
LOCAL_CFLAGS    += -D MG1EVB
LOCAL_SHARED_LIBRARIES:=libcodecjpegenc
ENABLE_VIDEO_ENCODER = n   #for sample application
endif

ifeq ($(TARGET_PRODUCT), avlite)
LOCAL_CFLAGS    += -D AVLITE
LOCAL_SHARED_LIBRARIES:=libcodecjpegenc
ENABLE_VIDEO_ENCODER = n   #for sample application
endif
