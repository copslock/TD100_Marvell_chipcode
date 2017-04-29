# make file for new hardware  from
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

file := $(TARGET_OUT_ETC)/ppp/ip-up
$(file) : $(LOCAL_PATH)/ip-up | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)

file := $(TARGET_OUT_ETC)/ppp/ip-down
$(file) : $(LOCAL_PATH)/ip-down | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)

file := $(TARGET_OUT_ETC)/ppp/chap-secrets
$(file) : $(LOCAL_PATH)/chap-secrets | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)

file := $(TARGET_OUT_ETC)/ppp/chat_conf
$(file) : $(LOCAL_PATH)/chat_conf | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)

file := $(TARGET_OUT_ETC)/ppp/pap-secrets
$(file) : $(LOCAL_PATH)/pap-secrets | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)

file := $(TARGET_OUT_ETC)/ppp/peers/ondialer
$(file) : $(LOCAL_PATH)/ondialer | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)

file := $(TARGET_OUT_ETC)/ppp/pppondialer
$(file) : $(LOCAL_PATH)/pppondialer | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)

file := $(TARGET_OUT_ETC)/ppp/ppp-startup
$(file) : $(LOCAL_PATH)/ppp-startup | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)

