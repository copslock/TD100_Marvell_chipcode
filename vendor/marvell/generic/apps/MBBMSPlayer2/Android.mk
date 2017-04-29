LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
# build MBBMSPlayer
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(call all-subdir-java-files)
#LOCAL_JAVA_LIBRARIES := com.marvell.cmmb.aidl
LOCAL_PACKAGE_NAME := MBBMSPlayer
#LOCAL_CERTIFICATE:=platform
LOCAL_STATIC_JAVA_LIBRARIES += android-common com.marvell.cmmb.aidl
LOCAL_PROGUARD_FLAGS := -include $(LOCAL_PATH)/proguard.flags
include $(BUILD_PACKAGE)

# additionally, build tests in sub-folders in a separate .apk
include $(call all-makefiles-under,$(LOCAL_PATH))
