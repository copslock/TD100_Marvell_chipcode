cmd_/home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/kernel/gc_hal_kernel_heap.o := /home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-gcc -Wp,-MD,/home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/kernel/.gc_hal_kernel_heap.o.d  -nostdinc -isystem /home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include -I/home/yanl/marvell.sp6/kernel/kernel/arch/arm/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-mmp/include -Iarch/arm/plat-pxa/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -marm -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -D__LINUX_ARM_ARCH__=5 -march=armv5te -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -fomit-frame-pointer -g -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -Werror -DLINUX -DDRIVER -DENUM_WORKAROUND=0 -DDBG=0 -DNO_DMA_COHERENT -DENABLE_ARM_L2_CACHE=1 -DgcdNO_POWER_MANAGEMENT=0 -DUSE_PLATFORM_DRIVER=1 -DVIVANTE_PROFILER=0 -DANDROID=1 -DENABLE_GPU_CLOCK_BY_DRIVER=1 -DUSE_NEW_LINUX_SIGNAL=0 -DNO_USER_DIRECT_ACCESS_FROM_KERNEL=0 -I/home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/inc -I/home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/kernel -I/home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/arch/unified/hal/kernel -I/home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/arch/unified/cmodel/inc -I/home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/user  -DMODULE -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(gc_hal_kernel_heap)"  -D"KBUILD_MODNAME=KBUILD_STR(hal/driver/galcore)" -D"DEBUG_HASH=55" -D"DEBUG_HASH2=19" -c -o /home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/kernel/gc_hal_kernel_heap.o /home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/kernel/gc_hal_kernel_heap.c

deps_/home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/kernel/gc_hal_kernel_heap.o := \
  /home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/kernel/gc_hal_kernel_heap.c \
  /home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/kernel/gc_hal_kernel_precomp.h \
  /home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/inc/gc_hal.h \
  /home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/inc/gc_hal_types.h \
  /home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/inc/gc_hal_options.h \
    $(wildcard include/config/cpu/pxa910.h) \
    $(wildcard include/config/pxa95x.h) \
    $(wildcard include/config/cpu/mmp2.h) \
    $(wildcard include/config/dvfm.h) \
    $(wildcard include/config/dvfm/pxa910.h) \
    $(wildcard include/config/dvfm/td.h) \
    $(wildcard include/config/dvfm/mg1.h) \
    $(wildcard include/config/dvfm/mmp2.h) \
    $(wildcard include/config/enable/dvfm.h) \
    $(wildcard include/config/axiclk/control.h) \
    $(wildcard include/config/has/earlysuspend.h) \
    $(wildcard include/config/earlysuspend.h) \
    $(wildcard include/config/enable/earlysuspend.h) \
    $(wildcard include/config/pxa910/dvfm/stats.h) \
  /home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/inc/gc_hal_enum.h \
  /home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/inc/gc_hal_base.h \
    $(wildcard include/config/.h) \
  /home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/inc/gc_hal_dump.h \
  /home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/inc/gc_hal_profiler.h \
  /home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/inc/gc_hal_driver.h \
  /home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/kernel/gc_hal_kernel.h \
  /home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/arch/unified/hal/kernel/gc_hal_kernel_hardware.h \

/home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/kernel/gc_hal_kernel_heap.o: $(deps_/home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/kernel/gc_hal_kernel_heap.o)

$(deps_/home/yanl/marvell.sp6/vendor/marvell/generic/graphics/galcore_src/hal/kernel/gc_hal_kernel_heap.o):
