#
# This file will be copied to <topdir>/boot and change the name to Makefile by repo.
# If you don't use repo, please copy this file to <topdir>/boot folder manually.  
#

hide:=@
log=@echo [$(shell date "+%Y-%m-%d %H:%M:%S")]

export PATH:=$(shell pwd)/../vendor/marvell/generic/toolchain/arm-linux-4.1.1/bin:$(PATH)

OUTDIR:=$(shell pwd)/out

export ARCH:=arm
export CROSS_COMPILE:=arm-linux-
export OBMDIR:=$(shell pwd)/obm
export UBOOTDIR:=$(shell pwd)/uboot
export SWDOWNLOADER:=$(shell pwd)/../vendor/marvell/generic/software_downloader

.PHONY:help
help:
	$(hide)echo "======================================="
	$(hide)echo "= This file wraps the build of obm and uboot"
	$(hide)echo "= make all: to make both the obm and uboot. The obm and uboot will be output to 'out' directory."
	$(hide)echo "= make obm: only make the obm.The uboot should already been built(need u-boot.bin).Otherwise the build will fail"
	$(hide)echo "= make uboot: only make the uboot."
	$(hide)echo "= make swdownloader: only make the swdownloader.The obm should already been built.Otherwise the build will fail"
	$(hide)echo "= make clean: clean the obm,uboot and swdownloader"
	$(hide)echo "= make clean_obm: clean obm"
	$(hide)echo "= make clean_uboot: clean uboot"
	$(hide)echo "= make clean_swdownloader: clean swdownloader"
	$(hide)echo "======================================="

.PHONY:all
all: uboot uboot_emmc obm swdownloader

.PHONY: uboot uboot_emmc obm swdownloader
uboot:
	$(log) "Starting to build uboot"
	$(hide)cd $(UBOOTDIR) && \
	make distclean && \
	make dkb_config && \
	make
	$(log) "start to copy u-boot.bin file"
	$(hide)mkdir -p $(OUTDIR)
	$(hide)cp $(UBOOTDIR)/u-boot.bin $(OUTDIR)/
	$(log) "cp uboot bin  done."

uboot_emmc:
	$(log) "Starting to build eMMC uboot"
	$(hide)cd $(UBOOTDIR) && \
	make distclean && \
	make dkb_emmc_config && \
	make
	$(log) "start to copy u-boot_emmc.bin file"
	$(hide)mkdir -p $(OUTDIR)
	$(hide)cp $(UBOOTDIR)/u-boot.bin $(OUTDIR)/u-boot_emmc.bin
	$(log) "cp eMMC uboot bin  done."

obm:
	$(log) "starting to build obm files"
	$(hide)chmod +x $(OBMDIR)/make_GUI_release.sh 
	$(hide)chmod +x $(OBMDIR)/Loader/Platforms/TTC2/DKB3_0/*.sh 
	$(hide)chmod +x $(OBMDIR)/Loader/Platforms/TTC2/Dongle/*.sh 
	$(hide)chmod +x $(OBMDIR)/Loader/Platforms/TTC2/EVBIII/*.sh 
	$(hide)cd $(OBMDIR) && \
	./make_GUI_release.sh
	$(log) "start to copy obm files"
	$(hide)mkdir -p $(OUTDIR)
	$(hide)cp -R $(OBMDIR)/Bootloader_*_Linux $(OUTDIR)/
	$(log) "Copy obm files done"
swdownloader:
	$(log) "starting to zip software downloader with new obm"
	$(hide)mkdir -p $(OUTDIR)/Software_Downloader
	$(hide)cp -rp $(SWDOWNLOADER)/* $(OUTDIR)/Software_Downloader/
	$(hide)cp -rf $(OUTDIR)/Bootloader_*_Linux/* $(OUTDIR)/Software_Downloader/
	$(hide)cd $(OUTDIR) && zip -r Software_Downloader.zip Software_Downloader && \
	rm -r Software_Downloader
	$(log) "finished zip software downloader with new obm"

.PHONY:clean clean_obm clean_uboot clean_uboot_emmc clean_swdownloader
clean_uboot:
	$(log) "cleaning uboot..."
	$(hide)cd $(UBOOTDIR) && \
	make distclean
	$(hide)rm -f $(OUTDIR)/u-boot.bin
	$(log) "Uboot cleaned."
clean_uboot_emmc:
	$(log) "cleaning uboot_emmc..."
	$(hide)cd $(UBOOTDIR) && \
	make distclean
	$(hide)rm -f $(OUTDIR)/u-boot_emmc.bin
	$(log) "uboot_emmc cleaned."
clean_obm:
	$(log) "cleaning obm..."
#	$(hide)cd $(OBMDIR) && \
#	make clean
	$(hide)rm -rf $(OUTDIR)/Bootloader_*_Linux
#	$(hide)rm -f $(OUTDIR)/TTC_LINUX_NTOBM.bin
	$(log) "OBM cleaned."
clean_swdownloader:
	$(log) "cleaning swdownloader..."
	$(hide)rm -rf $(OUTDIR)/Software_Downloader.zip
clean: clean_obm clean_uboot clean_uboot_emmc clean_swdownloader
	$(hide)rm -fr $(OUTDIR)


