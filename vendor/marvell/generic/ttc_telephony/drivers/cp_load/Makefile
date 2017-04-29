#
# Makefile for Linux Telephony
#
# CopyRight (c) 2006 Intel Corp ( vincent.c.yeung@intel.com )

TARGET = cploaddev

$(TARGET)-objs := pxa910_cp.o ../errorhandler/src/watchdog.o

EXTRA_CFLAGS += -DENV_LINUX \
			 -I$(BUILDROOT)/errorhandler/inc\
			 -I$(BUILDROOT)/include \
			 -DLOAD_SEAGULL_ADDR=$(SEAGULL_ADDR)\
			 -DLOAD_SEAGULL_SIZE=$(SEAGULL_SIZE)\
			 -DLOAD_MSA_ADDR=$(MSA_ADDR)\
			 -DLOAD_RDATA_ADDR=$(RDATA_ADDR)

INSTALLDIR = $(PXA_TARGET_FS_DIR)

all: modules 

ifneq ($(KERNELRELEASE),) 
obj-m	:= $(TARGET).o

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
