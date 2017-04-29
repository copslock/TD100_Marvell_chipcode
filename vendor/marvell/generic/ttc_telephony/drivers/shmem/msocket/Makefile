#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2 as
#  published by the Free Software Foundation.
# (C) Copyright 2007 Marvell International Ltd.

TARGET = msocketk

BASEDIR = $(PWD)

$(TARGET)-objs := portqueue.o shm.o msocket.o acipcd.o

#KERNELDIR = ../../../linux
KERNELDIR = $(PXA_SRC_PVK_LNX_DIR)

EXTRA_CFLAGS += -I$(KERNELDIR)/include \
				 -I$(BASEDIR)/include \
				 -I$(BUILDROOT)/diag/diag_kernel \
				 -I$(BUILDROOT)/cp_load \
				 -I./

EXTRA_CFLAGS += -DEXPORT_SYMTAB \
	-DENV_LINUX\
	-DACI_LNX_KERNEL \
	-DEXPORT_SYMTAB \
	
	#-DTX_TASK_ENABLE \

ifeq "$(TARGET_PRODUCT)" "dkbtd"
EXTRA_CFLAGS+=-DTD_SUPPORT
endif

KBUILD_EXTRA_SYMBOLS = $(BUILDROOT)/cp_load/Module.symvers

all: modules

ifneq ($(KERNELRELEASE),)
 
obj-m	:= $(TARGET).o
#CC =arm-linux-gcc
						     
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
	rm -rf msocket.o
	rm -rf *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.bak *.o *.symvers *.order *.mod
