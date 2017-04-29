ifeq ($(CONFIG_CRASH_DUMP),y)
	zreladdr-y	:= 0x06008000
else
	zreladdr-y	:= 0x00008000
endif
