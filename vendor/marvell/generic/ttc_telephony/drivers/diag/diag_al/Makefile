ifeq ($(MAKERULES),)
include ../../Rules.make
else
include $(MAKERULES)
endif
BUILDROOT=../..
DIAG_LINUX_AR = ../../libdiag.a

CFLAGS+=-I./inc\
	-I$(BUILDROOT)/INC\
	-I$(BUILDROOT)/include\
	-I$(BUILDROOT)/osa/inc\
	-I../diag_kernel\
	-I$(BUILDROOT)/linux_telephony/atcmdsrv/inc\
	-I$(BUILDROOT)/linux_audio\
	-I$(BUILDROOT)/aud_sw/acm/inc\

#	-DDEBUG_BUILD

ifeq "$(ENABLE_MRD_CMNDS)" "y"
CFLAGS+=-DAT_PRODUCTION_CMNDS
endif

DIAGOBJS= \
	diag_API.o \
	diag_API_var.o \
	diag_mmi_if_OSA_LINUX.o \
	diag_comm_L2.o \
	diag_comm_L4.o \
	diag_comm_EXTif.o \
	diag_comm_EXTif_OSA_LINUX.o \
	diag_al.o \
	diag_osif.o \
	diag_tx.o \
	diag_organ.o\

DIAGINTS= \
	diag_API.i \
	diag_API_var.i \
	diag_comm_EXTif.i \
	diag_comm_EXTif_OSA_LINUX.i \
	diag_al.i \
	diag_osif.i \
	diag_tx.i \
	diag_aud.i \
	diag_organ.i

#.PHONY: libdiag.a

all:   diag_aud.o
	make -f Makefile gen_diag_pre
	make -f Makefile build_diag_al

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<


#$(BUILDROOT)/libdiag.a: $(DIAGOBJS)
#	$(AR) $(ARFLAGS) $@ $(DIAGOBJS)
#	echo $(RANLIB)
#	$(RANLIB) $@

clean:
	-rm $(DIAGOBJS)
	-rm $(BUILDROOT)/libdiag.a

build_diag_al: $(DIAGOBJS)
	$(AR) rcs $(DIAG_LINUX_AR) $(DIAGOBJS)

#Generate intermediate files for prepass
gen_diag_pre: $(DIAGINTS)
	mv diag_aud.i $(BUILDROOT)/errorhandler/src
