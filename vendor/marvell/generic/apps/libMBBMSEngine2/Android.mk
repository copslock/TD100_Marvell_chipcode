LOCAL_PATH:= $(call my-dir)

#all shared libraries
include $(CLEAR_VARS) 
LOCAL_PREBUILT_LIBS := libsmsmbbms.so \
	libmbbmsengine.so  \
	libinno.so  \
	libsiano.so  \
	libsmscontrol.so

LOCAL_MODULE_TAGS := user
include $(BUILD_MULTI_PREBUILT) 

include $(CLEAR_VARS)
LOCAL_PREBUILT_LIBS := libmbbmslog.so

LOCAL_MODULE_TAGS := optional
include $(BUILD_MULTI_PREBUILT)

#com.marvell.cmmb.aidl.xml
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := user
LOCAL_MODULE := com.marvell.cmmb.aidl.xml
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

#com.marvell.cmmb.aidl.jar
include $(CLEAR_VARS)
LOCAL_MODULE := com.marvell.cmmb.aidl
LOCAL_MODULE_TAGS := user
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
LOCAL_SRC_FILES := com.marvell.cmmb.aidl.jar
include $(BUILD_PREBUILT)

#inno firmwares
include $(CLEAR_VARS)
LOCAL_MODULE := IF228FW.bin
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := user
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/firmware/cmmb/
include $(BUILD_PREBUILT)

#siano firmwares
include $(CLEAR_VARS)
LOCAL_MODULE := cmmb_ming_app.inp
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := user
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)
include $(BUILD_PREBUILT)

#testif228
include $(CLEAR_VARS)
LOCAL_MODULE := testif228
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

#testmbbms
include $(CLEAR_VARS)
LOCAL_MODULE := testmbbms
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

#testsim
include $(CLEAR_VARS)
LOCAL_MODULE := testsim
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

#MBBMSService
include $(CLEAR_VARS)
LOCAL_MODULE := MBBMSService
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_TAGS := user
LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

#SmsCmmbTest
include $(CLEAR_VARS)
LOCAL_MODULE := SmsCmmbTest
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)
