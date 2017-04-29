ifeq ($(MAKERULES),)
include Rules.make
else
include $(MAKERULES)
endif

SRC = serial_client.c serial_cfun0.c

ifneq "$(CONFIG_BIONIC)" "y"
LDLIBS= -lpthread
else
EXTRA_OBJECTS= ../../../fake.o
endif

all: serial_client serial_cfun0

clean:
	rm -f $(OBJS)  serial_client serial_cfun0

serial_client:serial_client.o $(EXTRA_OBJECTS)
	$(CC) $(BIONIC_LINK) $(LDLIBS) -o $@ serial_client.o $(EXTRA_OBJECTS) $(BIONIC_POSTLINK)

serial_cfun0:serial_cfun0.o $(EXTRA_OBJECTS)
	$(CC) $(BIONIC_LINK) $(LDLIBS) -o $@ serial_cfun0.o $(EXTRA_OBJECTS) $(BIONIC_POSTLINK)
