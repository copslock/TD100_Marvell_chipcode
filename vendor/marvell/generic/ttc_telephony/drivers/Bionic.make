PSDK_BASE := $(PXA_SRC_ANDROID_DEVICE_DIR)
TARGET_PATH := dkb
BIONICS_LD = -L$(PSDK_BASE)/out/target/product/$(TARGET_PATH)/system/lib -Wl,-rpath-link=$(PSDK_BASE)/out/target/product/$(TARGET_PATH)/system/lib
BIONICS_LDLIBS = -L$(PSDK_BASE)/out/target/product/$(TARGET_PATH)/system/lib -rpath-link=$(PSDK_BASE)/out/target/product/$(TARGET_PATH)/system/lib -lc
BIONICS_LIBS=-L$(PSDK_BASE)/out/target/product/$(TARGET_PATH)/system/lib

BIONICS_INCS  = -I$(PSDK_BASE)/bionic/libc/arch-arm/include \
        -I$(PSDK_BASE)/bionic/libc/include \
	-I$(PSDK_BASE)/bionic/libstdc++/include \
	-I$(PSDK_BASE)/bionic/libc/kernel/common \
	-I$(PSDK_BASE)/bionic/libc/kernel/arch-arm \
        -I$(PSDK_BASE)/bionic/libm/include \
        -I$(PSDK_BASE)/bionic/libm/include/arm\
	-I$(PSDK_BASE)/frameworks/base/include\
	-I$(PSDK_BASE)/system/core/include\
	-I$(PSDK_BASE)/hardware/libhardware_legacy/include

BIONIC_LINK := -nostdlib -Bdynamic \
      -Wl,-T,$(PSDK_BASE)/build/core/armelf.x \
      -Wl,-dynamic-linker,/system/bin/linker \
      -Wl,-z,nocopyreloc \
      -Wl,--gc-sections \
      -Wl,-z,noexecstack \
      -Wl,--no-undefined \
      -L$(PSDK_BASE)/out/target/product/$(TARGET_PATH)/system/lib \
      -Wl,-rpath-link=$(PSDK_BASE)/out/target/product/$(TARGET_PATH)/system/lib \
     $(PSDK_BASE)/out/target/product/$(TARGET_PATH)/obj/lib/crtbegin_dynamic.o

BIONIC_POSTLINK := $(PSDK_BASE)/out/target/product/$(TARGET_PATH)/obj/lib/crtend_android.o -lm -lc -lcutils -lutils -lhardware_legacy -lnetutils
BIONIC_EXTRA_OBJECTS := fake.o
#CFLAGS+=-D__linux__ \
#        -DBIONIC
#CCOMPILER=$(CPP)
