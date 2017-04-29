#
# Makefile for Linux Telephony
#
# CopyRight (c) 2006 Intel Corp ( vincent.c.yeung@intel.com )

ifndef CITTY_ROOT

CITTY_ROOT = $(shell pwd)/..

endif

TARGET = gs_modem

$(TARGET)-objs := 

INC_DIR = $(KBUILD_EXTMOD)/../inc
CFLAGS += -I$(INC_DIR)
EXTRA_CFLAGS += -I$(PWD)/pca_components/inc -I$(PWD)/include 

INSTALLDIR = /home/nfs

all: modules

ifneq ($(KERNELRELEASE),)
 
obj-m   := ../src/modem_kernel/$(TARGET).o
#CC =arm-linux-gcc

else

modules: 

ifeq "$(CONFIG_BIONIC)" "y"
export CROSS_COMPILE=arm-eabi-
	$(MAKE) -C $(BIONIC_LINK) $(CFLAGS) $(PXA_SRC_PVK_LNX_DIR) M=$(CITTY_ROOT)/build modules $(BIONIC_POSTLINK)
endif
ifeq "$(CONFIG_GEN_ARM_LINUX)" "y"
export CROSS_COMPILE    := arm-linux-
	$(MAKE) -C $(PXA_SRC_PVK_LNX_DIR) M=$(CITTY_ROOT)/build modules
endif
ifeq "$(CONFIG_GEN_MARVELL_LINUX)" "y"
export CROSS_COMPILE    := arm-marvell-linux-gnueabi-
	$(MAKE) -C $(PXA_SRC_PVK_LNX_DIR) M=$(CITTY_ROOT)/build modules 
endif

install:
	install -d $(INSTALLDIR)
	install -c ../src/modem_kernel/$(TARGET).ko $(INSTALLDIR)

dev:

clean:
	@echo 'Going to cleanup.\n'
	rm -rf                      \
	$(CITTY_ROOT)/src/modem_kernel/*.o       \
	$(CITTY_ROOT)/src/modem_kernel/*~        \
	$(CITTY_ROOT)/src/modem_kernel/core      \
	$(CITTY_ROOT)/src/modem_kernel/.*.d      \
	$(CITTY_ROOT)/src/modem_kernel/.*.cmd    \
	$(CITTY_ROOT)/src/modem_kernle/*.ko      \
	$(CITTY_ROOT)/src/modem_kernel/*.mod.c   \
	$(CITTY_ROOT)/build/.tmp_versions   \
	$(CITTY_ROOT)/src/modem_kernel/*.bak
endif
