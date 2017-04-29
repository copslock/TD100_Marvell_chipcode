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


BINDIR=../../wps_bin/
ENCRYPT_INC=./encrypt_lib/

TOOLPATH=/usr/local/arm-marvell-linux-gnueabi/bin
CROSS=$(TOOLPATH)/arm-marvell-linux-gnueabi-
ENCRYPT_LIB=$(ENCRYPT_INC)encrypt_esupp_pxa.a

CC=$(CROSS)gcc
LD=$(CROSS)ld
AR=$(CROSS)ar

# Enable to print debug message to standard ouput
CONFIG_STDOUT_DEBUG=n

# Enable uAP support
CONFIG_UAP_SUPPORT=y

# Enable WPS state machine restart feature
CONFIG_RESTART_SM_SUPPORT=y

# Enable PKCS Padding
CONFIG_PKCS_PADDING=y

# Enable to include WPS IE in probe request packet
CONFIG_WPS_IE_NEEDED=n

# Big-endian platform
CONFIG_BIG_ENDIAN=n

# Enable WiFiDirect Support
CONFIG_WFD_SUPPORT=y


# Enable WSC 2.0 Support
CONFIG_WSC2_SUPPORT=y

# Enable WSC 2.0 PF Support
CONFIG_WSC2_PF_BUILD=y


#----------------------------------------
# Compile Option
#----------------------------------------
CFLAGS = -O2 -Wall

ifeq ($(CONFIG_STDOUT_DEBUG), y)
CFLAGS += -g -Wall -DSTDOUT_DEBUG
endif

ifeq ($(CONFIG_UAP_SUPPORT), y)
CFLAGS += -DUAP_SUPPORT
endif

ifeq ($(CONFIG_WSC2_SUPPORT), y)
CFLAGS += -DWSC2
CFLAGS += -DEAP_FRAG

ifeq ($(CONFIG_WSC2_PF_BUILD), y)
CFLAGS += -DWSC2_PF
endif

endif

ifeq ($(CONFIG_WPSE_SUPPORT), y)
CFLAGS += -DWPSE_SUPPORT
endif

ifeq ($(CONFIG_RESTART_SM_SUPPORT),y)
CFLAGS += -DRESTART_SM_SUPPORT
endif

ifeq ($(CONFIG_WFD_SUPPORT), y)
CFLAGS += -DWFD_SUPPORT
endif

ifeq ($(CONFIG_WFD_SIGMA_SUPPORT), y)
CFLAGS += -DWFD_SIGMA_CAPI
endif

ifeq ($(CONFIG_WPS_IE_NEEDED), y)
CFLAGS += -DWPS_IE_NEEDED
endif

ifeq ($(CONFIG_BIG_ENDIAN), y)
CFLAGS += -DBIG_ENDIAN
endif


ifeq ($(CONFIG_PKCS_PADDING), y)
CFLAGS += -DPKCS_PADDING
endif

#----------------------------------------
# Include directories
#----------------------------------------
CFLAGS += -I.. -I$(ENCRYPT_INC)

#----------------------------------------
# Build Target
#----------------------------------------
ALL=libwsc.a

all: $(ALL)

OBJS =  wps_state.o \
	wps_eapol.o \
	wps_msg.o \
	wps_start.o

$(OBJS): Makefile

$(ALL): $(OBJS) Makefile
	$(AR) rcs $(ALL) $(OBJS) $(LIBS)


echo:
	@echo "************************************************"
	@echo "* Start building Marvell WPS library..... *"
	@echo "************************************************"


build: echo all
	@echo "Finished making Marvell WPS library"


clean:
	rm -f *.o *.d .*.swp $(ALL)
