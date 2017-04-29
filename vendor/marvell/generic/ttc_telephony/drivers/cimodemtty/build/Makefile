#
# Makefile for Linux Telephony
#
# CopyRight (c) 2006 Intel Corp ( vincent.c.yeung@intel.com )

ifndef CITTY_ROOT

CITTY_ROOT = $(shell pwd)/..

endif

TARGET = gs_modem

$(TARGET)-objs := ../src/modem_kernel/modem.o

INC_DIR = $(KBUILD_EXTMOD)/../inc
EXTRA_CFLAGS += -I$(INC_DIR)
EXTRA_CFLAGS += -I$(PWD)/pca_components/inc -I$(PWD)/include 
EXTRA_CFLAGS+=-I./inc/\
	-I$(BUILDROOT)/include\
	-I$(BUILDROOT)/fsm/inc\
	-I$(BUILDROOT)/pca_components/pca_components/inc\
	-I$(BUILDROOT)/pca_components/ci/inc\
	-I$(BUILDROOT)/pca_components/ci_stub/inc\
	-I$(BUILDROOT)/pca_components/ci_stub/src\
	-I$(BUILDROOT)/aud_sw/audio_stub/src\
	-I$(BUILDROOT)/linux_telephony/atcmdsrv/inc\
	-I$(BUILDROOT)/genlib/atparser/inc\
	-I$(BUILDROOT)/aud_sw/acm/inc\
	-I$(PXA_HOST_DIR)/include\
        -I$(BUILDROOT)/diag/diag_logic/inc \
        -I$(BUILDROOT)/diag/diag_logic/src \
        -I$(BUILDROOT)/diag/diag_comm/inc \
        -I$(BUILDROOT)/diag/diag_comm/src \
        -I$(BUILDROOT)/genlib/diag/inc \
	-I$(BUILDROOT)/pca_components/inc \
	-I$(BUILDROOT)/linux_audio\
	-I$(BUILDROOT)/cidatastub

EXTRA_CFLAGS+=\
	-DENV_LINUX\
	-DCI_STUB_CLIENT_INCLUDE\
	-DUPGRADE_R99\
	-DTAVOR_AUDIO\
	-D_MICCO_B0_ \
	-DDEBUG_BUILD\
	-DACI_KERNEL_DEBUG \

	
	#-DDEBUG_BUILD\
	-DATCMDSRV_DEBUG\
	-DutlDEBUG\
	-DDEBUG_BUILD\
	-DMSL_TRACE_ON \
	-DMSLDL_DIRECT_ACCESS

KBUILD_EXTRA_SYMBOLS = $(PWD)/cidatastub/Module.symvers


INSTALLDIR = /home/nfs

all: modules

ifneq ($(KERNELRELEASE),)
 
obj-m   := $(TARGET).o
#CC =arm-linux-gcc

else

modules: 
ifeq "$(CONFIG_BIONIC)" "y"
	$(MAKE) -C $(PXA_SRC_PVK_LNX_DIR) ARCH=arm CROSS_COMPILE=arm-eabi- M=$(CITTY_ROOT)/build modules 
else
ifeq "$(CONFIG_GEN_ARM_LINUX)" "y"
	$(MAKE) -C $(PXA_SRC_PVK_LNX_DIR) ARCH=arm CROSS_COMPILE=arm-linux- M=$(CITTY_ROOT)/build modules 
endif
ifeq "$(CONFIG_GEN_MARVELL_LINUX)" "y"
	$(MAKE) -C $(PXA_SRC_PVK_LNX_DIR) ARCH=arm CROSS_COMPILE=arm-marvell-linux-gnueabi- M=$(CITTY_ROOT)/build modules
endif
endif
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
	$(CITTY_ROOT)/src/modem_kernel/*.ko      \
	$(CITTY_ROOT)/src/modem_kernel/*.mod.c   \
	$(CITTY_ROOT)/build/*.ko      \
	$(CITTY_ROOT)/build/*.mod.c   \
	$(CITTY_ROOT)/build/Module.symvers \
	$(CITTY_ROOT)/build/.tmp_versions   \
	$(CITTY_ROOT)/src/modem_kernel/*.bak
