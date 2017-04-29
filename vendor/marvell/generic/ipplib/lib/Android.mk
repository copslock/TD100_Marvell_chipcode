LOCAL_PATH:=$(call my-dir)

MY_IPP_STATIC_LIBRARIES := \
	libcodecaacdec \
	libcodecmp3dec \
	libcodech263dec \
        libcodech263enc \
	libcodech264dec \
	libcodecmpeg4dec \
	libcodecmpeg2dec \
	libcodecwmvdec \
	libcodecamrnbdec \
	libcodecamrwbdec \
	libcodecamrnbenc \
	libcodecamrwbenc \
	libcodecjpegdec \
	libcodecjpegenc \
        libcodecmpeg4enc \
        libcodech264enc \
        libcodecaacenc

define translate-a-to-so
$(foreach t,$(1), \
  $(eval $(call add-prebuilt-files, STATIC_LIBRARIES, $(t).a)) \
  $(eval include $(CLEAR_VARS)) \
  $(eval LOCAL_WHOLE_STATIC_LIBRARIES := $(t)) \
  $(eval LOCAL_MODULE := $(t)) \
  $(eval LOCAL_PRELINK_MODULE := false) \
  $(eval include $(BUILD_SHARED_LIBRARY)) \
 )
endef

$(call translate-a-to-so, $(MY_IPP_STATIC_LIBRARIES))

ifeq ($(USE_MARVELL_MVED), true)
MY_MVED_STATIC_LIBRARIES := \
	libcodech264decmved \
	libcodech264encmved \
	libcodecmpeg4decmved \
	libcodecmpeg4encmved \
	libcodecwmvdecmved

define translate-mved-a-to-so
$(foreach t,$(1), \
  $(eval $(call add-prebuilt-files, STATIC_LIBRARIES, $(t).a)) \
  $(eval include $(CLEAR_VARS)) \
  $(eval LOCAL_WHOLE_STATIC_LIBRARIES := $(t)) \
  $(eval LOCAL_SHARED_LIBRARIES += libMrvlMVED libbmm) \
  $(eval LOCAL_MODULE := $(t)) \
  $(eval LOCAL_PRELINK_MODULE := false) \
  $(eval include $(BUILD_SHARED_LIBRARY)) \
 )
endef

$(call translate-mved-a-to-so, $(MY_MVED_STATIC_LIBRARIES))
endif

#Special handling for WMA because of missing rand API in Android
$(call add-prebuilt-files, STATIC_LIBRARIES, libcodecwmadec.a)
include $(CLEAR_VARS)
LOCAL_WHOLE_STATIC_LIBRARIES := libcodecwmadec
LOCAL_SRC_FILES := rand.c
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libcodecwmadec
include $(BUILD_SHARED_LIBRARY)


#Special handling for C&M
ifeq ($(USE_MARVELL_CNM),true)

$(call add-prebuilt-files, STATIC_LIBRARIES, libcnm.a)
include $(CLEAR_VARS)
LOCAL_WHOLE_STATIC_LIBRARIES := libcnm
LOCAL_SHARED_LIBRARIES := libpmemhelper
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libcnm
include $(BUILD_SHARED_LIBRARY)

$(call add-prebuilt-files, STATIC_LIBRARIES, libcnmhal.a)
include $(CLEAR_VARS)
LOCAL_WHOLE_STATIC_LIBRARIES := libcnmhal
LOCAL_SHARED_LIBRARIES := libpmemhelper libcnm
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libcnmhal
include $(BUILD_SHARED_LIBRARY)

$(call add-prebuilt-files, STATIC_LIBRARIES, libcodeccnmdec.a)
include $(CLEAR_VARS)
LOCAL_WHOLE_STATIC_LIBRARIES := libcodeccnmdec
LOCAL_SHARED_LIBRARIES := libpmemhelper libcnm libcnmhal
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libcodeccnmdec
include $(BUILD_SHARED_LIBRARY)

$(call add-prebuilt-files, STATIC_LIBRARIES, libcodeccnmenc.a)
include $(CLEAR_VARS)
LOCAL_WHOLE_STATIC_LIBRARIES := libcodeccnmenc
LOCAL_SHARED_LIBRARIES := libpmemhelper libcnm libcnmhal
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libcodeccnmenc
include $(BUILD_SHARED_LIBRARY)

endif

$(call add-prebuilt-files, STATIC_LIBRARIES,libippsp.a)
##to become shared library
include $(CLEAR_VARS)
LOCAL_WHOLE_STATIC_LIBRARIES := libippsp
LOCAL_SHARED_LIBRARIES := libmiscgen
LOCAL_MODULE := libippsp
#LOCAL_MODULE_TAGS := eng
LOCAL_PRELINK_MODULE := false
include $(BUILD_SHARED_LIBRARY)


$(call add-prebuilt-files, STATIC_LIBRARIES,libippvp.a)
$(call add-prebuilt-files, STATIC_LIBRARIES,libippcam.a)

##to become shared library
include $(CLEAR_VARS)
LOCAL_WHOLE_STATIC_LIBRARIES := libippvp
LOCAL_SHARED_LIBRARIES := libmiscgen libippsp
LOCAL_MODULE := libippvp
#LOCAL_MODULE_TAGS := eng
LOCAL_PRELINK_MODULE := false
include $(BUILD_SHARED_LIBRARY)



