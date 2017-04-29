LOCAL_PATH:=$(call my-dir)

$(call add-prebuilt-file, lib_il_aacdec_wmmx2lnx.a, STATIC_LIBRARIES)
$(call add-prebuilt-file, lib_il_mp3dec_wmmx2lnx.a, STATIC_LIBRARIES)
$(call add-prebuilt-file, lib_il_basecore_wmmx2lnx.a, STATIC_LIBRARIES)
$(call add-prebuilt-file, lib_il_ippomxmem_wmmx2lnx.a, STATIC_LIBRARIES)
$(call add-prebuilt-file, lib_il_mpeg4aspdec_wmmx2lnx.a, STATIC_LIBRARIES)
$(call add-prebuilt-file, lib_il_h264dec_wmmx2lnx.a, STATIC_LIBRARIES)
$(call add-prebuilt-file, lib_il_mpeg4enc_wmmx2lnx.a, STATIC_LIBRARIES)
$(call add-prebuilt-file, lib_il_h264enc_wmmx2lnx.a, STATIC_LIBRARIES)
$(call add-prebuilt-file, lib_il_aacenc_wmmx2lnx.a, STATIC_LIBRARIES)
$(call add-prebuilt-file, lib_il_amrnbenc_wmmx2lnx.a, STATIC_LIBRARIES)
$(call add-prebuilt-file, lib_il_amrnbdec_wmmx2lnx.a, STATIC_LIBRARIES)
$(call add-prebuilt-file, lib_il_h263enc_wmmx2lnx.a, STATIC_LIBRARIES)
$(call add-prebuilt-file, lib_il_h263dec_wmmx2lnx.a, STATIC_LIBRARIES)
$(call add-prebuilt-file, lib_il_amrwbdec_wmmx2lnx.a, STATIC_LIBRARIES)

ifeq ($(USE_MARVELL_MVED),true)
$(call add-prebuilt-file, lib_il_mpeg4aspdecmved_wmmx2lnx.a, STATIC_LIBRARIES)
$(call add-prebuilt-file, lib_il_h264decmved_wmmx2lnx.a, STATIC_LIBRARIES)
$(call add-prebuilt-file, lib_il_mpeg4encmved_wmmx2lnx.a, STATIC_LIBRARIES)
$(call add-prebuilt-file, lib_il_h264encmved_wmmx2lnx.a, STATIC_LIBRARIES)
endif

ifeq ($(USE_MARVELL_CNM),true)
$(call add-prebuilt-file, lib_il_codadx8dec_wmmx2lnx.a, STATIC_LIBRARIES)
$(call add-prebuilt-file, lib_il_codadx8enc_wmmx2lnx.a, STATIC_LIBRARIES)
endif

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false

LOCAL_WHOLE_STATIC_LIBRARIES := \
        lib_il_aacdec_wmmx2lnx \
        lib_il_mp3dec_wmmx2lnx \
        lib_il_basecore_wmmx2lnx \
        lib_il_ippomxmem_wmmx2lnx \
        lib_il_mpeg4aspdec_wmmx2lnx \
        lib_il_h264dec_wmmx2lnx \
        lib_il_mpeg4enc_wmmx2lnx \
        lib_il_h264enc_wmmx2lnx \
        lib_il_aacenc_wmmx2lnx \
        lib_il_amrnbenc_wmmx2lnx \
        lib_il_amrnbdec_wmmx2lnx \
        lib_il_h263enc_wmmx2lnx \
        lib_il_h263dec_wmmx2lnx \
        lib_il_amrwbdec_wmmx2lnx

LOCAL_SHARED_LIBRARIES := \
        libcodecaacdec \
        libcodecmp3dec \
	libmiscgen \
	libdl \
        libcodecaacenc \
        libcodecamrnbenc \
        libcodecamrnbdec \
        libcodech263dec \
        libcodech263enc \
        libcodecamrwbdec

LOCAL_CFLAGS += \
        -D_MARVELL_VIDEO_MPEG4ASPDECODER \
        -D_MARVELL_VIDEO_H264DECODER \
        -D_MARVELL_AUDIO_MP3DECODER \
        -D_MARVELL_AUDIO_AACDECODER \
        -D_MARVELL_VIDEO_MPEG4ENCODER \
        -D_MARVELL_VIDEO_H264ENCODER \
        -D_MARVELL_AUDIO_AACENCODER \
        -D_MARVELL_AUDIO_AMRNBENCODER \
        -D_MARVELL_AUDIO_AMRNBDECODER \
        -D_MARVELL_VIDEO_H263DECODER \
        -D_MARVELL_VIDEO_H263ENCODER \
        -D_MARVELL_AUDIO_AMRWBDECODER

ifeq ($(USE_MARVELL_CNM),true)
LOCAL_WHOLE_STATIC_LIBRARIES += \
    lib_il_codadx8dec_wmmx2lnx\
    lib_il_codadx8enc_wmmx2lnx

LOCAL_CFLAGS += \
        -D_MARVELL_VIDEO_CODADX8DECODER\
        -D_MARVELL_VIDEO_CODADX8ENCODER

LOCAL_SHARED_LIBRARIES += \
        libbmm \
        libcnm \


endif

ifeq ($(USE_MARVELL_MVED),true)
LOCAL_WHOLE_STATIC_LIBRARIES += \
        lib_il_mpeg4aspdecmved_wmmx2lnx \
        lib_il_h264decmved_wmmx2lnx \
        lib_il_mpeg4encmved_wmmx2lnx \
        lib_il_h264encmved_wmmx2lnx

LOCAL_CFLAGS += \
        -D_MARVELL_VIDEO_MPEG4ASPDECODERMVED \
        -D_MARVELL_VIDEO_H264DECODERMVED \
	-D_MARVELL_VIDEO_MPEG4ENCMVED \
	-D_MARVELL_VIDEO_H264ENCMVED

#LOCAL_SHARED_LIBRARIES := \
        libcodecaacdec \
        libcodecaacenc \
        libcodecmp3dec \
	libmiscgen \
	libdl

LOCAL_SHARED_LIBRARIES += \
	libbmm \
	libMrvlMVED
endif

LOCAL_SRC_FILES := \
	IppOmxComponentRegistry.c

ifeq ($(shell if [ $(PLATFORM_SDK_VERSION) -ge 9 ]; then echo big9; fi),big9)
LOCAL_C_INCLUDES := \
	frameworks/base/include/media/stagefright/openmax
else
LOCAL_C_INCLUDES := \
        external/opencore/extern_libs_v2/khronos/openmax/include
endif

LOCAL_COPY_HEADERS := \
	include/OMX_IppDef.h

LOCAL_COPY_HEADERS_TO := \
	libipp

LOCAL_MODULE := libMrvlOmx

include $(BUILD_SHARED_LIBRARY)
