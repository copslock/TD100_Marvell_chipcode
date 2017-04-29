ifeq ($(MAKERULES),)
include Rules.make
else
include $(MAKERULES)
endif

SRC = vt_test.c

ifneq "$(CONFIG_BIONIC)" "y"
LDLIBS= -lpthread
else
EXTRA_OBJECTS= ../../../fake.o
endif

all: vt_test

clean:
	rm -f $(OBJS)  $(TARGET) 

vt_test:vt_test.o $(EXTRA_OBJECTS)
	$(CC) $(BIONIC_LINK) $(LDLIBS) -o $@ vt_test.o $(EXTRA_OBJECTS) $(BIONIC_POSTLINK)

