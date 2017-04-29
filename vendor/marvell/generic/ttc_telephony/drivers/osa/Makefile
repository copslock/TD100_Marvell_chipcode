ifeq ($(MAKERULES),)
include ../Rules.make
else
include $(MAKERULES)
endif

BUILDROOT=..

CFLAGS+=-I./inc/ -I $(BUILDROOT)/osa/inc -I $(BUILDROOT)/include 
CFLAGS+=-DOSA_LINUX\
	-DOSA_USE_ASSERT\
	-DOSA_MSG_QUEUE\
	-DOSA_QUEUE_NAMES\
	-D_FDI_USE_OSA_\
	-DUSE_OSA_SEMA\
	-DENV_LINUX\
	-DOSA_USE_INTERNAL_MESSAGING\


ifeq "$(CONFIG_GEN_DEBUG)" "y"
CFLAGS+=-DOSA_DEBUG\
	-DDEBUG_BUILD
endif

OSAOBJS=\
	src/osa.o src/osa_linux.o src/utilities.o

.PHONY:libosa.a

$(BUILDROOT)/libosa.a:$(OSAOBJS)
	$(AR) $(ARFLAGS) $@ $(OSAOBJS)
	$(RANLIB) $@

clean:
	-rm $(BUILDROOT)/libosa.a
	-rm $(OSAOBJS)
