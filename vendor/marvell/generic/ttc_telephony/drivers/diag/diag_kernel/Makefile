#
# Makefile for Linux Telephony
#
# CopyRight (c) 2006 Intel Corp ( vincent.c.yeung@intel.com )

#TARGET = gs_diag
#$(TARGET)-objs :=  serial.o usbstring.o config.o epautoconf.o

#obj-m   := gs_diag.o diag.o
diag-objs := diag_drv.o diag_channel_kernel.o
gs_diag-objs := diag_usb.o

EXTRA_CFLAGS += -I$(PWD)/include -I$(PWD)/shmem/msocket -DEXPORT_SYMTAB 
#EXTRA_CFLAGS += -DDIAG_USB_TEST

KBUILD_EXTRA_SYMBOLS = $(PWD)/shmem/msocket/Module.symvers

all: modules

ifneq ($(KERNELRELEASE),)
 
#obj-m	:= $(TARGET).o
obj-m	:= diag.o gs_diag.o


else

modules: 
ifeq "$(CONFIG_BIONIC)" "y"
	$(MAKE) -C $(PXA_SRC_PVK_LNX_DIR) ARCH=arm CROSS_COMPILE=arm-eabi- M=$(shell pwd) modules 
else
ifeq "$(CONFIG_GEN_ARM_LINUX)" "y"
	$(MAKE) -C $(PXA_SRC_PVK_LNX_DIR) ARCH=arm CROSS_COMPILE=arm-linux- M=$(shell pwd) modules 
endif
ifeq "$(CONFIG_GEN_MARVELL_LINUX)" "y"
	$(MAKE) -C $(PXA_SRC_PVK_LNX_DIR) ARCH=arm CROSS_COMPILE=arm-marvell-linux-gnueabi- M=$(shell pwd) modules
endif
endif
endif


clean:
	@echo 'Going to cleanup.\n'
	rm -rf    \
	*.o       \
	*~        \
	core      \
	.*.d      \
	.*.cmd    \
	*.ko      \
	*.mod.c   \
	*.mod.c   \
	Module.symvers \
	.tmp_versions   \
	*.bak
