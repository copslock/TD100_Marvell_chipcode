#
# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PROGUARD_FLAGS := -include $(LOCAL_PATH)/proguard.flags

LOCAL_MODULE_TAGS := user

LOCAL_STATIC_JAVA_LIBRARIES := libmime4j libhttpclient libhttpcore libhtpmime

# Include all test java files.
LOCAL_SRC_FILES := $(call all-subdir-java-files, src)

LOCAL_PACKAGE_NAME := WifiDirectHttpDemoApp
LOCAL_CERTIFICATE := platform

include $(BUILD_PACKAGE)

LOCAL_PREBUILT_STATIC_JAVA_LIBRARIES := libmime4j:lib/apache-mime4j-0.6.1.jar libhttpclient:lib/httpclient-4.1.1.jar libhttpcore:lib/httpcore-4.1.jar libhtpmime:lib/httpmime-4.1.1.jar

include $(BUILD_MULTI_PREBUILT)
