ifeq ($(MAKERULES),)
include ../../Rules.make
else
include $(MAKERULES)
endif
BUILDROOT=../..

CFLAGS+=-I./inc\
	-I$(BUILDROOT)/INC\
	-I$(BUILDROOT)/include\
	-I$(BUILDROOT)/errorhandler/inc\
	-I$(BUILDROOT)/aud_sw/audio_stub/src\


ACMOBJS=src/acm_control.o src/acm_stream.o src/audiolevante.o src/audiobluetooth.o src/audionull.o src/amixer.o src/FSNotifyManager.o


.PHONY: libacm.a

$(BUILDROOT)/libacm.a: $(ACMOBJS)
	$(AR) $(ARFLAGS) $@ $(ACMOBJS)
	echo $(RANLIB)
	$(RANLIB) $@

clean:
	-rm $(ACMOBJS)
	-rm $(BUILDROOT)/libacm.a
