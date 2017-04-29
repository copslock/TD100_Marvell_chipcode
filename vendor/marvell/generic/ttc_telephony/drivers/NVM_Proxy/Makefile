ifeq ($(MAKERULES),)
include ../Rules.make
else
include $(MAKERULES)
endif
BUILDROOT=..

CFLAGS+=-I./include\
	-I$(BUILDROOT)/INC\
	-I$(BUILDROOT)/osa/inc\
	-I$(BUILDROOT)/include\
	-I$(BUILDROOT)/shmem/wrapper\
	-I$(BUILDROOT)/diag/diag_al\
	-I$(BUILDROOT)/errorhandler/inc


CFLAGS+=-DOSA_LINUX\
	-DOSA_USE_ASSERT\
	-DOSA_MSG_QUEUE\
	-DOSA_QUEUE_NAMES\
	-D_FDI_USE_OSA_\
	-DUSE_OSA_SEMA\
	-DENV_LINUX\

	#-DDEBUG_BUILD\
	-DDEBUG_NVM_PROXY

ifeq "$(CONFIG_GEN_DEBUG)" "y"
CFLAGS+=-DDEBUG_NVM_PROXY\
	-DDEBUG
endif

ifeq "$(CONFIG_BIONIC)" "y"
CFLAGS+=-D__linux__ \
	$(BIONICS_LD)
endif

NVMOBJS=source/NVMProxy.o source/Parser.o \
	source/NVMServer_API.o source/npx_main.o source/NVMIPCServer.o \
	source/NVMDiag.o source/NVMIPCClient.o source/BackNVM.o

NVMINTS=source/NVMDiag.i source/BackNVM.i

.PHONY: all

all: $(BUILDROOT)/libnvmproxy.a

$(BUILDROOT)/libnvmproxy.a: $(NVMINTS) $(NVMOBJS)
	$(AR) $(ARFLAGS) $@ $(NVMOBJS)
	echo $(RANLIB)
	$(RANLIB) $@

clean:
	-rm $(NVMOBJS)
	-rm $(NVMINTS)
	-rm $(BUILDROOT)/libnvmproxy.a
