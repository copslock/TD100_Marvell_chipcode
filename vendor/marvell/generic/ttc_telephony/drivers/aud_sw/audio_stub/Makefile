ifeq ($(MAKERULES),)
include ../../Rules.make
else
include $(MAKERULES)
endif

BUILDROOT=../..

CFLAGS+=-I$(BUILDROOT)/include\
	-I$(BUILDROOT)/osa/inc\
	-I$(BUILDROOT)/shmem/wrapper\
	-I$(BUILDROOT)/shmem/msocket\
	-I$(BUILDROOT)/genlib/acipc/inc/\
	-I$(BUILDROOT)/aud_sw/acm/inc/\
	-I$(PXA_HOST_DIR)/include\

AUDOBJS=src/audio_stub_api_ttc.o src/audio_stub_ttc.o


.PHONY:	libmaudio.a

$(BUILDROOT)/libmaudio.a:$(AUDOBJS)
	$(AR) $(ARFLAGS) $@ $(AUDOBJS)
	echo $(RANLIB)
	$(RANLIB) $@
clean:
	-rm $(AUDOBJS)
	-rm $(BUILDROOT)/libmaudio.a

