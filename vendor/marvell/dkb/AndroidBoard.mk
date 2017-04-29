# make file for new hardware  from 
#
LOCAL_PATH := $(call my-dir)
#

L_PATH := $(LOCAL_PATH)

#put additional library here
include $(L_PATH)/etc/Mdroid.mk
include $(L_PATH)/ext2fs/Mdroid.mk

ifneq ($(strip $(BOARD_USES_GENERIC_AUDIO)),true)
include $(L_PATH)/libaudiopath/Mdroid.mk
endif
