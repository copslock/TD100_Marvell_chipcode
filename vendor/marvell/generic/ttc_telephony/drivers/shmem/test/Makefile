#
# (C) Copyright 2006 Marvell International Ltd.  
# All Rights Reserved 
#
ifeq ($(MAKERULES),)
include ../../Rules.make
else
include $(MAKERULES)
endif
BUILDROOT=../..

#Fill component Name
#COMPONENT = msocket_test
OBJS=\
	msocketk_test.o\
	
#Fill  special CFLAGS
CFLAGS	+= -I./ \
		     -I../wrapper \
		     -I$(BUILDROOT)/include\
		     -I$(BUILDROOT)/osa/inc\
		     -I../msocket \
		-I../../errorhandler/inc\


CFLAGS	+= -DTEST_USER\
	   -DMUDP_AP\
	   -DMSOCKET_AP\
		-DOSA_LINUX\
	-DOSA_USE_ASSERT\
	-DOSA_MSG_QUEUE\
	-DOSA_QUEUE_NAMES\
	-DUSE_OSA_SEMA\
	-DSHMEM_TEST

CFLAGS1	= $(CFLAGS)
CFLAGS1 += -DSHMEM_TEST

#Fill object name (For example: battery)
OBJECTS = msocket_test msocket_test_from_kernel

#Fill all shell files
SHELL_FILES	= 
DATA_FILES	= 

.PHONY:all
all: $(OBJECTS)

msocket_test: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) ../../errorhandler/src/eeh_assert_notify.o  -lpthread -L../../ -losa -lshm

msocket_test_from_kernel: msocketk_test_from_kernel.o 
	$(CC) $(CFLAGS1) -o $@ msocketk_test_from_kernel.o

clean:
	rm -f $(OBJECTS) *.o 

