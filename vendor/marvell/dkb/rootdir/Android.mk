LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

file := $(TARGET_ROOT_OUT)/initlogo.rle
$(file) : $(LOCAL_PATH)/initlogo.rle | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)

file := $(TARGET_ROOT_OUT)/init.rc
$(file) : $(LOCAL_PATH)/init.rc | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)

file := $(TARGET_ROOT_OUT)/init.pxa910-based.rc
$(file) : $(LOCAL_PATH)/init.pxa910-based.rc | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)

file := $(TARGET_ROOT_OUT)/ueventd.pxa910-based.rc
$(file) : $(LOCAL_PATH)/ueventd.pxa910-based.rc | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)

file := $(TARGET_ROOT_OUT)/init_bsp.rc
$(file) : $(LOCAL_PATH)/init_bsp.rc | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)

file := $(TARGET_ROOT_OUT)/init.ext4.rc
$(file) : $(LOCAL_PATH)/init.rc | $(ACP)
	$(transform-prebuilt-to-target)
	$(hide) sed -i "/^[ tab]*# mount[ tab]*.\+ext4[ tab]*.\+[ tab]*\(\/system\|\/data\|\/cache\|\/marvell\)/ s/# mount/mount/" $(TARGET_ROOT_OUT)/init.ext4.rc
	$(hide) sed -i "/^[ tab]*mount[ tab]*.\+yaffs2[ tab]*.\+[ tab]*\(\/system\|\/data\|\/cache\|\/marvell\)/ s/mount/# mount/" $(TARGET_ROOT_OUT)/init.ext4.rc
ALL_PREBUILT += $(file)

file := $(TARGET_ROOT_OUT)/init.pxa910-based.ext4.rc
$(file) : $(LOCAL_PATH)/init.pxa910-based.rc | $(ACP)
	$(transform-prebuilt-to-target)
	$(hide) sed -i '/insmod \/system\/lib\/modules\/galcore.ko/ s;gpu_frequency=403;gpu_frequency=501;;' $(TARGET_ROOT_OUT)/init.pxa910-based.ext4.rc
	$(hide) sed -i '/exec \/system\/bin\/kexec.exe -p \/system\/etc\/kdump\/uImage --append=\"rdinit=\/busybox\/rdinit td_dkb emmc_boot console=ttyS0,115200\" --timestamp/ s/# exec/exec/' $(TARGET_ROOT_OUT)/init.pxa910-based.ext4.rc
	$(hide) sed -i '/exec \/system\/bin\/kexec.exe -p \/system\/etc\/kdump\/uImage --append=\"rdinit=\/busybox\/rdinit td_dkb console=ttyS0,115200\" --timestamp/ s/exec/# exec/' $(TARGET_ROOT_OUT)/init.pxa910-based.ext4.rc
	$(hide) echo "" >> $(TARGET_ROOT_OUT)/init.pxa910-based.ext4.rc
	$(hide) echo "service setup_fs /system/bin/setup_fs /dev/block/platform/sdhci-pxa.2/by-num/p10 /dev/block/platform/sdhci-pxa.2/by-num/p12 /dev/block/platform/sdhci-pxa.2/by-num/p13" >> $(TARGET_ROOT_OUT)/init.pxa910-based.ext4.rc
	$(hide) echo "	user root" >> $(TARGET_ROOT_OUT)/init.pxa910-based.ext4.rc
	$(hide) echo "	group root" >> $(TARGET_ROOT_OUT)/init.pxa910-based.ext4.rc
	$(hide) echo "	oneshot" >> $(TARGET_ROOT_OUT)/init.pxa910-based.ext4.rc
ALL_PREBUILT += $(file)

file := $(TARGET_ROOT_OUT)/init_bsp.pxa910-based.rc
$(file) : $(LOCAL_PATH)/init_bsp.pxa910-based.rc | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)

file := $(TARGET_ROOT_OUT)/init_bsp.ext4.rc
$(file) : $(LOCAL_PATH)/init_bsp.rc | $(ACP)
	$(transform-prebuilt-to-target)
	$(hide) sed -i "/^[ tab]*# mount[ tab]*.\+ext4[ tab]*.\+[ tab]*\(\/system\|\/data\|\/cache\|\/marvell\)/ s/# mount/mount/" $(TARGET_ROOT_OUT)/init_bsp.ext4.rc
	$(hide) sed -i "/^[ tab]*mount[ tab]*.\+yaffs2[ tab]*.\+[ tab]*\(\/system\|\/data\|\/cache\|\/marvell\)/ s/mount/# mount/" $(TARGET_ROOT_OUT)/init_bsp.ext4.rc
ALL_PREBUILT += $(file)

file := $(TARGET_ROOT_OUT)/init_bsp.pxa910-based.ext4.rc
$(file) : $(LOCAL_PATH)/init_bsp.pxa910-based.rc | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(file)

file := $(PRODUCT_OUT)/system/etc/kdump/uImage
$(file) :
	@mkdir -p $(PRODUCT_OUT)/system/etc/kdump/
	@if [ -e "kernel/out/uImage_kdump" ];then echo "Copy kdump images" && \
	cp kernel/out/uImage_kdump $(file) && echo "Copy kdump images done"; fi
ALL_PREBUILT += $(file)
