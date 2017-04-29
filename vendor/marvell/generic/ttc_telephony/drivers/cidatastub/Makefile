#
#(C) Copyright 2007 Marvell International Ltd.
#All Rights Reserved
#

TARGET = cci_datastub


$(TARGET)-objs := ccidatastub.o data_channel_kernel.o 

EXTRA_CFLAGS += -I$(PWD)/include -I$(PWD)/pca_components/pca_components/inc -I$(PWD)/pca_components/ci/inc -I$(PWD)/shmem/msocket
EXTRA_CFLAGS += -I$(PWD)/pca_components/ci_stub/src -I$(PWD)/pca_components/ci/inc -I$(PWD)/pca_components/inc 
EXTRA_CFLAGS += -DSAL_LITTLE_ENDIAN
EXTRA_CFLAGS += -DENV_LINUX -DACI_LNX_KERNEL -DCS_PS_PORTS 

INSTALLDIR = /tftpboot/
KERNELDIR = $(PXA_SRC_PVK_LNX_DIR)
KBUILD_EXTRA_SYMBOLS = $(PWD)/shmem/msocket/Module.symvers

all: modules

ifneq ($(KERNELRELEASE),)
 
obj-m	:= $(TARGET).o
#CC =arm-linux-gcc

else

modules: 
ifeq "$(CONFIG_BIONIC)" "y"
	$(MAKE) -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=arm-eabi- M=$(shell pwd) modules
else
ifeq "$(CONFIG_GEN_ARM_LINUX)" "y"
	$(MAKE) -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=arm-linux- M=$(shell pwd) modules
endif
ifeq "$(CONFIG_GEN_MARVELL_LINUX)" "y"
	$(MAKE) -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=arm-marvell-linux-gnueabi- M=$(shell pwd) modules
endif  
endif
endif

install:
	install -d $(INSTALLDIR)
	install -c $(TARGET).ko $(INSTALLDIR)

dev:
	su -c "rm -f $(INSTALLDIR)/dev/ccidatastub && mknod $(INSTALLDIR)/dev/ccidatastub c 246 0"

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.bak Module.symvers
