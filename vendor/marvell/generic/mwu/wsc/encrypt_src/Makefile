#
# File:	Makefile
#
# Copyright (C) 2011 Marvell International Ltd., All Rights Reserved
#
# MARVELL CONFIDENTIAL
# The source code contained or described herein and all documents related to
# the source code ("Material") are owned by Marvell International Ltd or its
# suppliers or licensors. Title to the Material remains with Marvell International Ltd
# or its suppliers and licensors. The Material contains trade secrets and
# proprietary and confidential information of Marvell or its suppliers and
# licensors. The Material is protected by worldwide copyright and trade secret
# laws and treaty provisions. No part of the Material may be used, copied,
# reproduced, modified, published, uploaded, posted, transmitted, distributed,
# or disclosed in any way without Marvell's prior express written permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or delivery
# of the Materials, either expressly, by implication, inducement, estoppel or
# otherwise. Any license under such intellectual property rights must be
# express and approved by Marvell in writing.
#

CONFIG_FILE := $(PWD)/../../.config
-include $(CONFIG_FILE)

ifeq ($(CONFIG_ARM), y)
	TOOLPATH=/usr/local/arm/4.1.1/bin
	CROSS=$(TOOLPATH)/arm-linux-
	ifeq ($(CONFIG_EMBEDDED_SUPPLICANT), y)
	TARGET=encrypt_esupp_arm.a
	else
	TARGET=encrypt_arm.a
	endif
endif

ifeq ($(CONFIG_ARM_SCM), y)
	TOOLPATH=/usr/local/arm/3.4.3/v6_vfp_le/bin
	CROSS=$(TOOLPATH)/arm_v6_vfp_le-
	TARGET=encrypt_arm_scm.a
endif

ifeq ($(CONFIG_ARM_KRKWD), y)
	CFLAGS += -DCONFIG_ARM_KRKWD
	TOOLPATH=/usr/local/arm-marvell-krkwd-linux-gnueabi/gcc/bin
	CROSS=$(TOOLPATH)/arm-none-linux-gnueabi-
	ifeq ($(CONFIG_EMBEDDED_SUPPLICANT),y)
	TARGET=encrypt_esupp_arm.a
	else
	TARGET=encrypt_arm.a
    endif
endif

ifeq ($(CONFIG_ARM_KRKWD_BE), y)
	CFLAGS += -DCONFIG_ARM_KRKWD_BE
	TOOLPATH=/usr/local/armeb-marvell-mv5sft-linux-gnueabi/cross/bin
	CROSS=$(TOOLPATH)/armeb-mv5sft-linux-gnueabi-
	ifeq ($(CONFIG_EMBEDDED_SUPPLICANT),y)
	TARGET=encrypt_esupp_arm_krkwd_be.a
	else
	TARGET=encrypt_arm_krkwd_be.a
    endif
endif

ifeq ($(CONFIG_PXA), y)
	CFLAGS += -DCONFIG_PXA
	TOOLPATH=/usr/local/arm-marvell-linux-gnueabi/bin
	CROSS=$(TOOLPATH)/arm-marvell-linux-gnueabi-
	ifeq ($(CONFIG_EMBEDDED_SUPPLICANT),y)
	CFLAGS += -DEMBEDDED_SUPPLICANT
	endif
	TARGET=encrypt_esupp_pxa.a
endif

ifeq ($(CONFIG_X86), y)
	TOOLPATH=
	CROSS=
	ifeq ($(CONFIG_EMBEDDED_SUPPLICANT), y)
	TARGET=encrypt_esupp_x86.a
	else
	TARGET=encrypt_x86.a
	endif
endif

ifeq ($(CONFIG_ORION), y)
	TOOLPATH=/usr/local/arm/4.2.0-be/bin
	CROSS=$(TOOLPATH)/armeb-none-linux-gnueabi-
	ifeq ($(CONFIG_EMBEDDED_SUPPLICANT), y)
	TARGET=encrypt_esupp_orion.a
	else
	TARGET=encrypt_orion.a
	endif
endif

ENCRYPT_LIB=../encrypt_lib/

CC=		$(CROSS)gcc
LD=		$(CROSS)ld
AR=		$(CROSS)ar

ifndef CFLAGS
CFLAGS = -O3 -Wall
endif

# Compiler option for debugging
#CFLAGS += -DDBG_PRINT_FUNC

# Compliler option for sha256.o
CFLAGS += -DINTERNAL_SHA256

# Compliler option for not including unused function in aes_wrap.o
CFLAGS += -DCONFIG_NO_AES_WRAP
CFLAGS += -DCONFIG_NO_AES_OMAC1
CFLAGS += -DCONFIG_NO_AES_CTR
CFLAGS += -DCONFIG_NO_AES_EAX

# Compliler option for DH big number math
CFLAGS += -DCONFIG_CRYPTO_INTERNAL
CFLAGS += -DCONFIG_INTERNAL_LIBTOMMATH
CFLAGS += -DLTM_FAST
CFLAGS += -DCONFIG_WPS

ENC_LIB_SUPP_OBJS =	\
	sha256.o	\
	aes.o	\
	aes_wrap.o

ENC_LIB_OBJS =	\
	key_algorim.o	\
	bignum.o	\
	crypto_internal.o	\
	dh.o	\
	encrypt.o

ifeq ($(CONFIG_EMBEDDED_SUPPLICANT), y)
	CFLAGS += -DINTERNAL_SHA1 -DINTERNAL_MD5
	ENC_LIB_SUPP_OBJS += sha1.o md5.o
endif

encrypt.a:  $(ENC_LIB_OBJS) $(ENC_LIB_SUPP_OBJS)
	$(AR) rcs encrypt.a $(ENC_LIB_OBJS) $(ENC_LIB_SUPP_OBJS)


build: encrypt.a
	@if [ ! -d $(ENCRYPT_LIB) ]; then \
		mkdir $(ENCRYPT_LIB); \
		cp -f encrypt.h $(ENCRYPT_LIB); \
		cp -f type_def.h $(ENCRYPT_LIB); \
	fi
	cp -f encrypt.a $(ENCRYPT_LIB)$(TARGET)
	@echo "Finished making Marvell Encryption Library"


clean:
	rm -f encrypt.a *.o *.d *~ core
