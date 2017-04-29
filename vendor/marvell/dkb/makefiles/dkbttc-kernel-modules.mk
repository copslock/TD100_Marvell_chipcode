#
# This file will be copied to <topdir>/kernel and change the name to Makefile by repo.
# If you don't use repo, please copy this file to <topdir>/kernel folder manually.  
#

hide:=@
log=@echo [$(shell date "+%Y-%m-%d %H:%M:%S")]

MAKE_JOBS?=4
KERNEL_TOOLCHAIN_PREFIX := $(shell pwd)/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-
#KERNEL_TOOLCHAIN_PREFIX := $(shell pwd)/toolchain/arm-marvell-linux-gnueabi-vfp-4.2.0/bin/arm-marvell-linux-gnueabi-
KERNEL_CONFIG?=pxa910_defconfig
KERNEL_KDUMP_CONFIG?=pxa910_kdump_defconfig

OUTDIR:=out

MODULES_BUILT=
MODULES_CLEAN=
define add-module
	MODULES_BUILT+=$(1)
	MODULES_CLEAN+=clean_$(1)
endef

export ARCH:=arm
export CROSS_COMPILE:=$(KERNEL_TOOLCHAIN_PREFIX)
export KERNELDIR:=$(shell pwd)/kernel
export TARGET_PRODUCT:=dkbttc
export PATH:=$(shell pwd)/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin:$(PATH)

.PHONY:help
help:
	$(hide)echo "======================================="
	$(hide)echo "= This file wraps the build of kernel and modules"
	$(hide)echo "= make all: to make  kernel, all modules and telephony. The kernel, modules and telephony will be output to 'out' directory."
	$(hide)echo "= make kernel: only make the kernel. Using KERNEL_CONFIG variable to specify the kernel config file to be used. By default it is: $(KERNEL_CONFIG)"
	$(hide)echo "= make modules: only make all the modules. The kernel should already be built. Otherwise building modules will fail."
	$(hide)echo "= make telephony: only make telephony."
	$(hide)echo "= make clean: clean the kernel, modules and telephony"
	$(hide)echo "======================================="

all: kernel modules telephony

KERNEL_TGT:=kernel/arch/arm/boot/uImage
.PHONY: kernel clean_kernel
kernel:
	$(hide)if [ -z $(shell which mkimage) ]; then echo "please install mkimage first: apt-get install uboot-mkimage"; exit 1; fi

	$(log) "making kernel [$(KERNEL_KDUMP_CONFIG)]..."
	$(hide)cd kernel && \
	make $(KERNEL_KDUMP_CONFIG) &&\
	rm -rf root && \
	cp -rPp ../../vendor/marvell/generic/rdroot_kdump/kdump_rdroot ./ && \
	make -j$(MAKE_JOBS) && \
	make uImage
	$(hide)mkdir -p $(OUTDIR)
	$(hide)mv $(KERNEL_TGT) $(OUTDIR)/uImage_kdump
	$(hide)mv kernel/System.map $(OUTDIR)/System.map_kdump
	$(hide)mv kernel/vmlinux $(OUTDIR)/vmlinux_kdump
	$(log) "kernel [$(KERNEL_KDUMP_CONFIG)] done"

	$(log) "making kernel [$(KERNEL_CONFIG)]..."
	$(hide)cd kernel && \
	make $(KERNEL_CONFIG) &&\
	rm -rf kdump_rdroot && \
	tar zxf ../../vendor/marvell/generic/rdroot/root.tgz && \
	make -j$(MAKE_JOBS) &&\
	echo "update g_ether.ko" && \
#cp drivers/usb/gadget/g_ether.ko root/ && \
#$(KERNEL_TOOLCHAIN_PREFIX)strip --strip-unneeded root/g_ether.ko && \
	make -j$(MAKE_JOBS) && \
	make uImage
	$(hide)mkdir -p $(OUTDIR)
	$(hide)mv $(KERNEL_TGT) $(OUTDIR)/
	$(hide)mv kernel/System.map $(OUTDIR)/
	$(hide)mv kernel/vmlinux $(OUTDIR)/
	$(log) "kernel [$(KERNEL_CONFIG)] done"

.PHONY:clean_kernel clean_modules clean_telephony
clean_kernel:
	$(hide)cd kernel &&\
	make clean
	$(hide)rm -f $(OUTDIR)/zImage
	$(log) "Kernel cleaned."

clean: clean_kernel clean_modules clean_telephony
	$(hide)rm -fr $(OUTDIR)

TELEPHONY:=../vendor/marvell/generic/ttc_telephony
TELEPHONY_DRVSRC:=$(TELEPHONY)/drivers
.PHONY: telephony clean_telephony
telephony:
	$(log) "making ttc telephony..."
	$(hide)cd $(TELEPHONY_DRVSRC)/ && \
	make
	$(hide)mkdir -p $(OUTDIR)/telephony/
	$(hide)cp $(TELEPHONY_DRVSRC)/output/pxafs.img $(OUTDIR)/telephony/
	$(hide)cp $(TELEPHONY_DRVSRC)/output/pxa_symbols.tgz $(OUTDIR)/telephony/
	$(hide)cp $(TELEPHONY_DRVSRC)/Boerne_DIAG.mdb.txt $(OUTDIR)/telephony/
	$(hide)cp $(TELEPHONY)/cp/reliabledata_trusted.rnd $(OUTDIR)/telephony/ReliableData.bin
	$(hide)cp $(TELEPHONY)/cp/tavor_p_ulc_dkb/Arbel* $(OUTDIR)/telephony/
	$(hide)cp $(TELEPHONY)/cp/tavor_p_ulc_dkb/TT* $(OUTDIR)/telephony/
	$(hide)cp $(TELEPHONY)/../rdroot/ramdisk_no_gui.img $(OUTDIR)/telephony
	$(log) "ttc telephony done."
clean_telephony:
	rm -rf $(OUTDIR)/telephony
	$(log) "ttc telephony cleaned."
GC500_DRVSRC:=../vendor/marvell/generic/graphics/galcore_src
export KERNEL_DIR:=$(KERNELDIR)
.PHONY: gc500 clean_gc500
gc500:
	$(log) "make g500 driver..."
	$(hide)cd $(GC500_DRVSRC) &&\
	make all -j$(MAKE_JOBS)
	$(hide)mkdir -p $(OUTDIR)/modules/
	$(hide)cp $(GC500_DRVSRC)/hal/driver/galcore.ko $(OUTDIR)/modules
	$(log) "gc500 driver done."

clean_gc500:
	$(hide)cd $(GC500_DRVSRC) &&\
	make clean 
	$(hide)rm -f $(OUTDIR)/modules/galcore.ko
	$(log) "gc500 driver cleaned."

$(eval $(call add-module,gc500) )

SD8787_DRVSRC:=../vendor/marvell/generic/sd8787/
.PHONY: sd8787_wifi clean_sd8787_wifi
sd8787_wifi:
	$(log) "making sd8787 wifi driver..."
	$(hide)cd $(SD8787_DRVSRC)/wlan_src && \
	make -j$(MAKE_JOBS) default
	$(hide)mkdir -p $(OUTDIR)/modules/
	$(hide)cp $(SD8787_DRVSRC)/wlan_src/sd8xxx.ko $(OUTDIR)/modules/sd8787.ko
	$(hide)cp $(SD8787_DRVSRC)/wlan_src/mlan.ko $(OUTDIR)/modules/mlan.ko
	$(log) "sd8787 wifi driver done."

clean_sd8787_wifi:
	$(hide)cd $(SD8787_DRVSRC)/wlan_src &&\
	make clean
	rm -f $(OUTDIR)/modules/sd8787.ko
	$(log) "sd8787 wifi driver cleaned."

$(eval $(call add-module,sd8787_wifi) )

.PHONY: sd8787_bt clean_sd8787_bt
sd8787_bt:
	$(log) "making sd8787 BT driver..."
	$(hide)cd $(SD8787_DRVSRC)/bt_src && \
	make -j$(MAKE_JOBS) default
	$(hide)mkdir -p $(OUTDIR)/modules/
	$(hide)cp $(SD8787_DRVSRC)/bt_src/bt8xxx.ko $(OUTDIR)/modules/bt8787.ko
	$(log) "sd8787 bt driver done."

clean_sd8787_bt:
	$(hide)cd $(SD8787_DRVSRC)/bt_src &&\
	make clean
	$(hide)rm -f $(OUTDIR)/modules/bt8787.ko
	$(log) "sd8787 bt driver cleaned."

$(eval $(call add-module,sd8787_bt) )

MPDC_DRVSRC := ../vendor/marvell/generic/mpdc
export PXALINUX := $(KERNELDIR)
export CPU_TYPE := PJ1
export SOC_TYPE := PXA91x
.PHONY: mpdc clean_mpdc
mpdc:
	$(log) "make mpdc driver ..."
	$(hide) cd $(MPDC_DRVSRC) &&\
	make -C src/driver/ PXALINUX=$(PXALINUX) CROSS_COMPILE=$(CROSS_COMPILE) CPU_TYPE=$(CPU_TYPE) SOC_TYPE=$(SOC_TYPE)
	$(hide) mkdir -p $(OUTDIR)/modules/
	$(hide) cp $(MPDC_DRVSRC)/src/driver/out/mpdc_cm.ko $(OUTDIR)/modules
	$(hide) cp $(MPDC_DRVSRC)/src/driver/out/mpdc_css.ko $(OUTDIR)/modules
	$(hide) cp $(MPDC_DRVSRC)/src/driver/out/mpdc_hs.ko $(OUTDIR)/modules
	$(log) "mpdc driver done"

clean_mpdc:
	$(hide)cd $(MPDC_DRVSRC)/src/driver &&\
	make clean
	$(hide)rm -f $(OUTDIR)/modules/mpdc_cm.ko
	$(hide)rm -f $(OUTDIR)/modules/mpdc_css.ko
	$(hide)rm -f $(OUTDIR)/modules/mpdc_hs.ko
	$(log) "mpdc driver cleaned."
$(eval $(call add-module,mpdc) )

BMM_DRVSRC:=../vendor/marvell/generic/bmm-lib
.PHONY: bmm clean_bmm
bmm:
	$(log) "make bmm driver..."
	$(hide)cd $(BMM_DRVSRC)/drv &&\
	make -f Makefile_Android
	$(hide)mkdir -p $(OUTDIR)/modules/
	$(hide)cp $(BMM_DRVSRC)/drv/bmm.ko $(OUTDIR)/modules
	$(log) "bmm driver done."

clean_bmm:
	$(hide)cd $(BMM_DRVSRC)/drv &&\
	make clean -f Makefile_Android
	$(hide)rm -f $(OUTDIR)/modules/bmm.ko
	$(log) "bmm driver cleaned."
$(eval $(call add-module,bmm) )

CNM_DRVSRC:=../vendor/marvell/generic/cnm-drv
.PHONY: cnm clean_cnm
cnm:
	$(log) "make cnm driver..."
	$(hide)cd $(CNM_DRVSRC)/ &&\
	make KDIR=../../../../kernel/kernel
	$(hide)mkdir -p $(OUTDIR)/modules/
	$(hide)cp $(CNM_DRVSRC)/cnm.ko $(OUTDIR)/modules
	$(log) "cnm driver done."

clean_cnm:
	$(hide)cd $(CNM_DRVSRC)/ &&\
	make clean
	$(hide)rm -f $(OUTDIR)/modules/cnm.ko
	$(log) "cnm driver cleaned."
$(eval $(call add-module,cnm) )

PMEM_DRVSRC:=./kernel/arch/arm/plat-pxa
.PHONY: pmem clean_pmem
pmem:
	$(log) "make pmem driver..."
	$(hide)cp $(PMEM_DRVSRC)/pmem.ko $(OUTDIR)/modules
	$(log) "pmem driver done."

clean_pmem:
	$(hide)rm -f $(PMEM_DRVSRC)/pmem.ko
	$(log) "pmem driver cleaned."
#$(eval $(call add-module,pmem) )

CMMB_DRVSRC:=./kernel/drivers/media/video/cmmb
.PHONY: cmmb clean_cmmb
cmmb:
	$(log) "make cmmb driver..."
	$(hide)cp $(CMMB_DRVSRC)/cmmb_if228.ko $(OUTDIR)/modules
	$(log) "cmmb driver done."

clean_cmmb:
	$(hide)rm -f $(CMMB_DRVSRC)/cmmb_if228.ko
	$(log) "cmmb driver cleaned."
$(eval $(call add-module,cmmb) )

SMSMDTV_DRVSRC:=./kernel/drivers/media/dvb/siano/
.PHONY: smsmdtv clean_smsmdtv
smsmdtv:
	$(log) "make smsmdtv driver..."
	$(hide)cp $(SMSMDTV_DRVSRC)/smsmdtv.ko $(OUTDIR)/modules
	$(log) "smsmdtv driver done."

clean_smsmdtv:
	$(hide)rm -f $(SMSMDTV_DRVSRC)/smsmdtv.ko
	$(log) "smsmdtv driver cleaned."
$(eval $(call add-module,smsmdtv) )

#insert any module declaration above

.PHONY: modules
modules:$(MODULES_BUILT)

clean_modules: $(MODULES_CLEAN)

