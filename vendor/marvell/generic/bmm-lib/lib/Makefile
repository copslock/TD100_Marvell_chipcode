#
# Makefile for BMM user library
#

ifeq ($(MAKERULES),)
include ../../../Rules.make
else
include $(MAKERULES)
endif

ifeq "$(CONFIG_GEN_DEBUG)" "y"
BMM_DEF := -DBMM_DEBUG

else
BMM_DEF := 
endif




SRC_DIR := .

CFLAGS += $(BMM_DEF) \
	-I${SRC_DIR}/include \

bmm_lib_src=${SRC_DIR}/bmm_lib.c \

bmm_test_src=${SRC_DIR}/bmm_test.c \

bmm_lib_obj=${bmm_lib_src:.c=.o} 

bmm_test_obj=${bmm_test_src:.c=.o} 


.PHONY: all compile install-host install-target clean clean-local \
	uninstall-host uninstall-target

all: compile install-host install-target 

compile: bmm_test libbmm.so libbmm.a

bmm_test: libbmm.so ${bmm_test_obj}
	${CC} -o $@ ${bmm_test_obj} -L. -lbmm

libbmm.a: ${bmm_lib_obj}
	${AR} -rcs $@ ${bmm_lib_obj}

libbmm.so: ${bmm_lib_obj}
	${CC} -g -s -shared -Wl,-soname,$@ -o $@ ${bmm_lib_obj}

install-host:
	cp -f libbmm.so $(PXA_HOST_LIB_DIR)
	cp -f libbmm.a $(PXA_HOST_LIB_DIR)
	cp -f bmm_lib.h $(PXA_HOST_INCLUDE_DIR)
install-target:
	$(SUDO) cp -f libbmm.so $(PXA_TARGET_LIB_DIR)
	$(SUDO) cp -f bmm_test $(PXA_TARGET_BIN_DIR)

clean: clean-local uninstall-host uninstall-target

clean-local:
	-rm -f ${bmm_lib_obj} ${bmm_test_obj}
	-rm -f libbmm.*
	-rm -f bmm_test

uninstall-host:
	-rm -f $(PXA_HOST_LIB_DIR)/libbmm.so
	-rm -f $(PXA_HOST_LIB_DIR)/libbmm.a
	-rm -f $(PXA_HOST_INCLUDE_DIR)/bmm_lib.h

uninstall-target:
	$(SUDO) rm -f $(PXA_TARGET_LIB_DIR)/libbmm.so
	$(SUDO) rm -f $(PXA_TARGET_BIN_DIR)/bmm_test
