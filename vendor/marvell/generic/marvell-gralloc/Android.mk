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


LOCAL_PATH := $(call my-dir)

# HAL module implemenation, not prelinked and stored in
# hw/<OVERLAY_HARDWARE_MODULE_ID>.<ro.product.board>.so
include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_SHARED_LIBRARIES := liblog libcutils libEGL libbmm libgcu

LOCAL_C_INCLUDES +=\
	vendor/marvell/generic/graphics/include \
	vendor/marvell/generic/graphics

LOCAL_SRC_FILES := 	\
	gralloc.cpp 	\
	framebuffer.cpp \
	mapper.cpp  \
	RasterSwapa_mrvl.S

LOCAL_MODULE := gralloc.default
LOCAL_CFLAGS:= -DLOG_TAG=\"gralloc\"
LOCAL_CFLAGS += -I$(ANDROID_BUILD_TOP)/vendor/marvell/generic/bmm-lib/lib/

ifeq ($(BOARD_LCD_CONTROLLER_BPP),32)
LOCAL_CFLAGS += -DLCD_BPP=32
else
ifeq ($(BOARD_LCD_CONTROLLER_BPP),24)
LOCAL_CFLAGS += -DLCD_BPP=24
endif
endif

LOCAL_CFLAGS += -DPLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)
	
ifeq ($(strip $(MRVL_BGRA_HACK)),true)
LOCAL_CFLAGS += -DMRVL_BGRA_HACK
endif

LOCAL_CFLAGS +=  -Wa,-mcpu=iwmmxt2

include $(BUILD_SHARED_LIBRARY)
