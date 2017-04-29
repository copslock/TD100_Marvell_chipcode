ifeq ($(MAKERULES),)
include ../Rules.make
else
include $(MAKERULES)
endif
BUILDROOT=..

CFLAGS+=-I./inc\
	-I$(BUILDROOT)/INC\
	-I$(BUILDROOT)/osa/inc\
	-I$(BUILDROOT)/include\
	-I$(BUILDROOT)/shmem/wrapper\
	-I$(BUILDROOT)/diag/diag_al\
	-I$(BUILDROOT)/NVM_Proxy/include


CFLAGS+=-DOSA_LINUX\
	-DOSA_USE_ASSERT\
	-DOSA_MSG_QUEUE\
	-DOSA_QUEUE_NAMES\
	-D_FDI_USE_OSA_\
	-DUSE_OSA_SEMA\
	-DENV_LINUX\
	-D_TAVOR_BOERNE_\
	-DCOMPILER_DOES_NOT_SUPPORT_SHORT_ENUMS \
	-DEE_COPY_COMM_IMAGE_FROM_FLASH \

#	-DDEBUG_BUILD

ifneq "$(TARGET_PRODUCT)" "dkbttc"
CFLAGS+=-DTD_SUPPORT
endif

EEHOBJS=src/EE_Postmortem.o\
	src/EEHandler_config.o\
	src/loadTable.o\
	src/eeh_api.o\
	src/eeh_assert_notify.o\
	src/EEHandler_fatal.o\
	src/fdi_file.o

EEHINTS= \
	 src/EE_Postmortem.i \
	 src/EEHandler_config.i \
	 src/loadTable.i \
	 src/eeh_api.i \
	 src/EEHandler_fatal.i

.PHONY: all


all: $(BUILDROOT)/libeeh.a gen_eeh_pre

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(BUILDROOT)/libeeh.a: $(EEHOBJS)
	$(AR) rcs $@ $(EEHOBJS)

clean:
	-rm $(EEHOBJS)
	-rm $(BUILDROOT)/libeeh.a


#Generate intermediate files for prepass
gen_eeh_pre: $(EEHINTS)
