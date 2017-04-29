LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

#keylayout and keypad character mapping			  
file := $(TARGET_OUT_KEYLAYOUT)/pxa27x-keypad.kl
ALL_PREBUILT += $(file)
$(file): $(LOCAL_PATH)/pxa27x-keypad.kl | $(ACP)
	$(transform-prebuilt-to-target)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := pxa27x-keypad.kcm
include $(BUILD_KEY_CHAR_MAP)

