ifeq ($(MAKERULES),)
include ../../../Rules.make
else
include $(MAKERULES)
endif
BUILDROOT=../../..

CFLAGS+=-I./inc\
	-I$(BUILDROOT)/INC\
	-I$(BUILDROOT)/include\
	-I$(BUILDROOT)/linux_telephony/atcmdsrv/inc\
	-I$(BUILDROOT)/genlib/atparser/inc\
	-I$(BUILDROOT)/osa/inc\

#	-DDEBUG_BUILD

MODEMOBJS=modem_al.o


.PHONY: libmodem.a

$(BUILDROOT)/libmodem.a: $(MODEMOBJS)
	$(AR) $(ARFLAGS) $@ $(MODEMOBJS)
	echo $(RANLIB)
	$(RANLIB) $@

clean:
	-rm $(MODEMOBJS)
	-rm $(BUILDROOT)/libmodem.a
