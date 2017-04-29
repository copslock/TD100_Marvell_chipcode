ifeq ($(MAKERULES),)
include ../Rules.make
else
include $(MAKERULES)
endif
BUILDROOT=../

CFLAGS+=-I$(BUILDROOT)/pca_components/msl_utils/inc\
	-I$(BUILDROOT)/pca_components/msl_dl/inc\
	-I$(BUILDROOT)/pca_components/pca_components/inc\
	-I$(BUILDROOT)/pca_components/gpc/inc\
	-I$(BUILDROOT)/pca_components/gpc/src\
	-I$(BUILDROOT)/pca_components/ci/inc\
	-I$(BUILDROOT)/linux_telephony/atcmdsrv/inc\
	-I$(BUILDROOT)/genlib/atparser/inc\
	-I./include\
	-I$(BUILDROOT)/INC\
	-I$(BUILDROOT)/NVM/Driver/include\
	-I$(BUILDROOT)/osa/inc\
	-I$(BUILDROOT)/include\
	-I$(BUILDROOT)/diag/diag_comm/inc\
	-I$(BUILDROOT)/diag/diag_logic/inc\
	-I$(BUILDROOT)/diag/diag_logic/src\
	-I$(BUILDROOT)/genlib/diag/inc\
	-I$(PXA_SRC_PVK_LNX_DIR)/arch/arm/mach-pxa/include\
	-I$(PXA_SRC_PVK_LNX_DIR)

CFLAGS+=-I./inc\
	-I./src

CFLAGS+=

MRDOBJS += ./src/MRD.o

MRDINTS = ./src/MRD.i

.PHONY: all

all: $(BUILDROOT)/libmrd.a

$(BUILDROOT)/libmrd.a: $(MRDINTS) $(MRDOBJS)
	$(AR) $(ARFLAGS) $@ $(MRDOBJS)
	echo $(RANLIB)
	$(RANLIB) $@

clean:
	-rm $(MRDOBJS)
	-rm $(MRDINTS)
	-rm $(BUILDROOT)/libmrd.a
