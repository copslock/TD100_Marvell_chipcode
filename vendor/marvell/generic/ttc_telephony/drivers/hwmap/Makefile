#
# Makefile for hwmap module
#
# CopyRight (c) 2008 Marvell (anton.eidelman@marvell.com)

TARGET = hwmap
APPS = hwacc
MODULE_ROOT = $(shell pwd)

# Rules.mak should be included to import PXA_SRC_PVK_LNX_DIR, PXA_HOST_DIR etc
ifeq ($(MAKERULES),)
include ../../../../Rules.make
else
include $(MAKERULES)
endif

$(TARGET)-objs := 
ifeq "$(CONFIG_BIONIC)" "y"
MODULES_CROSS_COMPILE=arm-eabi-
BIONIC_EXTRA_OBJECTS_DIR=../
endif

ifeq "$(CONFIG_GEN_MARVELL_LINUX)" "y"
MODULES_CROSS_COMPILE=arm-marvell-linux-gnueabi-
endif

ifeq "$(CONFIG_GEN_ARM_LINUX)" "y"
MODULES_CROSS_COMPILE=arm-linux-
endif

#TBD use of install - why parent (Drivers) should do this?
all: modules applications

ifneq ($(KERNELRELEASE),)
 
obj-m   := $(TARGET).o
CC =$(CROSS_COMPILE)gcc

else

modules: 
	$(MAKE) -C $(PXA_SRC_PVK_LNX_DIR) ARCH=arm CROSS_COMPILE=$(MODULES_CROSS_COMPILE) M=$(MODULE_ROOT) modules 

endif

applications:
	$(CC) $(BIONIC_LINK) $(CFLAGS) -o hwacc hwacc.c -lstdc++ -L$(PXA_HOST_DIR)/lib $(BIONIC_POSTLINK) $(BIONIC_EXTRA_OBJECTS_DIR)$(BIONIC_EXTRA_OBJECTS:.o=.c)
	@echo "DONE"

# The following does not take effect as the parent Makefile removes all the /tel contents and re-installs all sub-components
install:
	$(SUDO) cp -f hwacc $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f hwmap.ko $(PXA_TARGET_FS_DIR)/tel
	$(SUDO) cp -f hw.sh $(PXA_TARGET_FS_DIR)/tel

dev:

clean:
	@echo 'Going to cleanup.\n'
	rm -rf $(MODULE_ROOT)/*.o   \
	$(MODULE_ROOT)/*~        \
	$(MODULE_ROOT)/.*.d      \
	$(MODULE_ROOT)/.*.cmd    \
	$(MODULE_ROOT)/*.ko      \
	$(MODULE_ROOT)/*.mod.c   \
	$(MODULE_ROOT)/.tmp_versions   \
	$(MODULE_ROOT)/Module.symvers   \
	$(MODULE_ROOT)/*.bak \
	$(MODULE_ROOT)/hwacc
