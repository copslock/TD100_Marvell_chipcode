#
# Makefile for Linux Telephony
#
# CopyRight (c) 2006 Intel Corp ( vincent.c.yeung@intel.com )

TARGET = cidatattydev

$(TARGET)-objs := cidatatty.o


EXTRA_CFLAGS += -I$(PWD)/include -I$(PWD)/pca_components/ci_stub/src -I$(PWD)/pca_components/ci/inc -I$(PWD)/pca_components/pca_components/inc -I$(PWD)/pca_components/inc -I$(PWD)/cidatastub
EXTRA_CFLAGS += -DENV_LINUX
INSTALLDIR = $(PXA_TARGET_FS_DIR)

KBUILD_EXTRA_SYMBOLS = $(PWD)/cidatastub/Module.symvers


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

install:
	install -d $(INSTALLDIR)
	install -c $(TARGET).ko $(INSTALLDIR)

dev:
	
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.bak *.symvers *.order
