#
# Makefile for Linux System Error Handler
#

ifndef SEH_ROOT

SEH_ROOT = $(shell pwd)/..

endif

TARGET = seh

$(TARGET)-objs := ../src/seh_linux.o \
				 ../src/watchdog.o

BUILDROOT=$(PWD)/..
INC_DIR = $(KBUILD_EXTMOD)/../inc
EXTRA_CFLAGS += -I$(INC_DIR)
EXTRA_CFLAGS +=-I$(BUILDROOT)/include

INSTALLDIR = /tftpboot

KBUILD_EXTRA_SYMBOLS = $(BUILDROOT)/cp_load/Module.symvers

all: modules

ifneq ($(KERNELRELEASE),)

obj-m   := $(TARGET).o
#CC =arm-linux-gcc

else

modules: 
ifeq "$(CONFIG_BIONIC)" "y"
	$(MAKE) -C $(PXA_SRC_PVK_LNX_DIR) ARCH=arm CROSS_COMPILE=arm-eabi- M=$(SEH_ROOT)/build "$(EXTRA_CFLAGS)" modules
else
ifeq "$(CONFIG_GEN_ARM_LINUX)" "y"
	$(MAKE) -C $(PXA_SRC_PVK_LNX_DIR) ARCH=arm CROSS_COMPILE=arm-linux- M=$(SEH_ROOT)/build "$(EXTRA_CFLAGS)" modules
endif
ifeq "$(CONFIG_GEN_MARVELL_LINUX)" "y"
	$(MAKE) -C $(PXA_SRC_PVK_LNX_DIR) ARCH=arm CROSS_COMPILE=arm-marvell-linux-gnueabi- M=$(SEH_ROOT)/build "$(EXTRA_CFLAGS)" modules
endif  
endif
endif

install:
	install -d $(INSTALLDIR)
	install -c ../src/$(TARGET).ko $(INSTALLDIR)

clean:
	@echo 'Going to cleanup.\n'
	rm -rf                      \
	$(SEH_ROOT)/src/*.o       \
	$(SEH_ROOT)/src/*~        \
	$(SEH_ROOT)/src/core      \
	$(SEH_ROOT)/src/.*.d      \
	$(SEH_ROOT)/src/.*.cmd    \
	$(SEH_ROOT)/src/*.ko      \
	$(SEH_ROOT)/src/*.mod.c   \
	$(SEH_ROOT)/build/.tmp_versions   \
	$(SEH_ROOT)/build/Module.symvers   \
	$(SEH_ROOT)/src/*.bak \
	$(SEH_ROOT)/build/*.ko \
	$(SEH_ROOT)/build/*.mod.c
