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


LOCAL_PATH:= $(call my-dir)

# HAL module implemenation, not prelinked and stored in
# hw/<COPYPIX_HARDWARE_MODULE_ID>.<ro.board.platform>.so
include $(CLEAR_VARS)


# put your source files here
LOCAL_SRC_FILES := \
	overlay.marvell.cpp


# put the libraries you depend on here.
LOCAL_SHARED_LIBRARIES := \
        libbinder         \
	liblog            \
        libutils          \
        libcutils         \
        libphycontmem     \
        libgcu            


LOCAL_STATIC_LIBRARIES += libippcam


# put your module name here
LOCAL_MODULE := overlay.default

LOCAL_C_INCLUDES := \
        vendor/marvell/generic/phycontmem-lib/phycontmem   \
        vendor/marvell/generic/ipplib/include \
        vendor/marvell/generic/overlay-hal    \
        vendor/marvell/generic/graphics/include    

LOCAL_CFLAGS += -DPLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)

# Enable IRE engine for DKB platform
ifeq ($(TARGET_PRODUCT),dkb)
LOCAL_CFLAGS += -D_PXA168
LOCAL_CFLAGS += -D_IRE
LOCAL_C_INCLUDES += vendor/marvell/generic/ire/src 
LOCAL_SHARED_LIBRARIES += libire			  
endif

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw


include $(BUILD_SHARED_LIBRARY)
