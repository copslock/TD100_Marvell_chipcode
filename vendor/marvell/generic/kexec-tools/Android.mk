LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
	purgatory/purgatory.c \
	purgatory/printf.c \
	purgatory/string.c \
	util_lib/sha256.c

LOCAL_CFLAGS := -g -O2 -fno-strict-aliasing -Wall -Wstrict-prototypes -fno-zero-initialized-in-bss  -Os -fno-builtin -ffreestanding -O0 

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/purgatory/include \
	$(LOCAL_PATH)/util_lib/include \
	$(LOCAL_PATH)/purgatory/arch/arm/include \
	$(LOCAL_PATH)/include

LOCAL_MODULE:=purgatory
LOCAL_MODULE_TAGS:=optional

# Build a partial linked object file, it's used to generate purgatory.c below.
intermediates:=$(call intermediates-dir-for,STATIC_LIBRARIES,purgatory,,)
PURGATORY_GEN1:=$(LOCAL_PATH)/purgatory/purgatory.ro
$(PURGATORY_GEN1): PRIVATE_CUSTOM_TOOL = $(TARGET_CC) -o $(PURGATORY_GEN1) -g -O2 -fno-strict-aliasing -Wall -Wstrict-prototypes -fno-zero-initialized-in-bss  -Os -fno-builtin -ffreestanding  -Wl,--no-undefined -nostartfiles -nostdlib -nodefaultlibs -e purgatory_start -r $^
$(PURGATORY_GEN1):$(intermediates)/purgatory/purgatory.o $(intermediates)/purgatory/printf.o $(intermediates)/purgatory/string.o $(intermediates)/util_lib/sha256.o
	$(transform-generated-source)

PURGATORY_GEN2:=$(LOCAL_PATH)/kexec/purgatory.c
$(PURGATORY_GEN2): PRIVATE_INPUT_FILE := $(LOCAL_PATH)/purgatory/purgatory.ro
$(PURGATORY_GEN2): PRIVATE_BIN_TO_HEX := $(HOST_OUT_EXECUTABLES)/bin-to-hex
$(PURGATORY_GEN2): PRIVATE_CUSTOM_TOOL = $(PRIVATE_BIN_TO_HEX) purgatory <$(PRIVATE_INPUT_FILE) >$(PURGATORY_GEN2)
$(PURGATORY_GEN2): $(LOCAL_PATH)/purgatory/purgatory.ro $(PRIVATE_BIN_TO_HEX)
	$(transform-generated-source)

LOCAL_ADDITIONAL_DEPENDENCIES:=bin-to-hex

# Include $(BUILD_STATIC_LIBRARY) to build a static library. In fact, the static
#  library is not need, but the object files of the source code is needed. So a 
# static library is built as a work around.
include $(BUILD_STATIC_LIBRARY)

#
# bin-to-hex
# This binary is used to generate purgatory.c below.
#
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	util/bin-to-hex.c

LOCAL_CFLAGS := -O2 -g
LOCAL_MODULE:=bin-to-hex
LOCAL_MODULE_TAGS=optional

include $(BUILD_HOST_EXECUTABLE)


include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
	util_lib/compute_ip_checksum.c \
	util_lib/sha256.c

LOCAL_CFLAGS := -g -fno-strict-aliasing -Wall -Wstrict-prototypes -fno-zero-initialized-in-bss  -Os -fno-builtin -ffreestanding -O0

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/util_lib/include \
	$(LOCAL_PATH)/kexec/arch/arm/libfdt \
	$(LOCAL_PATH)/kexec/arch/arm/include \
	$(LOCAL_PATH)/util_lib/include

LOCAL_MODULE:=libkexecutil
LOCAL_MODULE_TAGS=optional

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	kexec/kexec.c \
	kexec/ifdown.c \
	kexec/kexec-elf.c \
	kexec/kexec-elf-exec.c \
	kexec/kexec-elf-core.c \
	kexec/kexec-elf-rel.c \
	kexec/kexec-elf-boot.c \
	kexec/kexec-iomem.c \
	kexec/firmware_memmap.c \
	kexec/crashdump.c \
	kexec/crashdump-xen.c \
	kexec/phys_arch.c \
	kexec/kernel_version.c \
	kexec/lzma.c \
	kexec/zlib.c \
	kexec/proc_iomem.c \
	kexec/virt_to_phys.c \
	kexec/arch/arm/phys_to_virt.c \
	kexec/add_segment.c \
	kexec/add_buffer.c \
	kexec/arch_reuse_initrd.c \
	kexec/arch/arm/kexec-elf-rel-arm.c \
	kexec/arch/arm/kexec-zImage-arm.c \
	kexec/arch/arm/kexec-uImage-arm.c \
	kexec/arch/arm/kexec-arm.c \
	kexec/arch/arm/crashdump-arm.c \
	kexec/kexec-uImage.c \
	kexec/purgatory.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/util_lib/include \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/kexec/arch/arm/libfdt \
	$(LOCAL_PATH)/kexec/arch/arm/include

LOCAL_CFLAGS := -g -O2 -fno-strict-aliasing -Wall -Wstrict-prototypes
LOCAL_MODULE:=kexec.exe
LOCAL_MODULE_TAGS=optional
LOCAL_STATIC_LIBRARIES := libkexecutil
LOCAL_UNSTRIPPED_PATH:=$(LOCAL_PATH)/build/sbin

# Without below addtional dependency, build system will generate 
# kexec/purgatory.c by default rules !
LOCAL_ADDITIONAL_DEPENDENCIES:=purgatory

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	kdump/kdump.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/util_lib/include \
	$(LOCAL_PATH)/kexec/arch/arm/libfdt

LOCAL_CFLAGS := -g -O2 -fno-strict-aliasing -Wall -Wstrict-prototypes
LOCAL_MODULE:=kdump.exe
LOCAL_MODULE_TAGS=optional

include $(BUILD_EXECUTABLE)
