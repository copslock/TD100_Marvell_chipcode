CC=gcc
CFLAGS+=-I../inc\
	-I../../include\
	-DOSA_LINUX\
	-DENV_LINUX\
        -DOSA_USE_ASSERT\
        -DOSA_MSG_QUEUE\
        -DOSA_QUEUE_NAMES\
        -D_FDI_USE_OSA_\
        -DUSE_OSA_SEMA\
        -DOSA_USE_INTERNAL_MESSAGING\

OUTPUT=osatest
	
OBJS=\
	osa_linux.o\
	osa.o\
	osa_test.o\
	
$(OUTPUT):$(OBJS)
	$(CC) -lpthread -o $@ $(OBJS)
	@echo "DONE"

osa_linux.o : osa_linux.c
	$(CC) $(CFLAGS) -c $^ -o $@

osa.o : osa.c
	$(CC) $(CFLAGS) -c $^ -o $@
	
osa_test.o : osa_test.c
	$(CC) $(CFLAGS) -c $^ -o $@
	
clean:
	-rm $(OUTPUT)
	-rm $(OBJS)
