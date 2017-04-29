ifeq ($(MAKERULES),)
include ../../Rules.make
else
include $(MAKERULES)
endif

BUILDROOT=../..

CFLAGS+=-I$(BUILDROOT)/include\
	

CFLAGS+=

SHMOBJS=msocket_api.o

.PHONY:libshm.a

$(BUILDROOT)/libshm.a:$(SHMOBJS)
	$(AR) $(ARFLAGS) $@ $(SHMOBJS)
	echo $(RANLIB)
	$(RANLIB) $@

clean:
	-rm $(SHMOBJS)
	-rm $(BUILDROOT)/libshm.a
