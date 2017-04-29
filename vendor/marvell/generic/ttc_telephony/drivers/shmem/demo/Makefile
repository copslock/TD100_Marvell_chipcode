#
# (C) Copyright 2006 Marvell International Ltd.  
# All Rights Reserved 
#
ifeq ($(MAKERULES),)
include ../../Rules.make
else
include $(MAKERULES)
endif
BUILDROOT=../../

#Fill component Name
COMPONENT = mdemo
#Fill  special CFLAGS
CFLAGS	+= -I./ \
		     -I../wrapper \
		     -I$(BUILDROOT)/include

#Fill object name (For example: battery)
OBJECTS = mdemo

#Fill all shell files
SHELL_FILES	= 
DATA_FILES	= 

.PHONY:all
all: $(OBJECTS)

mdemo: mdemo.c ../wrapper/msocket.c
	$(CC) $(CFLAGS) -o $@ $?
	cp mdemo ../msocket/mdemo

clean:
	rm -f $(OBJECTS) *.o 

