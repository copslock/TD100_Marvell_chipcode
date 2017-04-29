LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# $(1): src directory
# $(2): output file
# $(3): label (if any)
# $(4): if true, add journal
# $(5): num_blocks
define my-build-userimage-ext2-target
	@mkdir -p $(dir $(2))
	$(hide) num_inodes=`find $(1) | wc -l` ; num_inodes=`expr $$num_inodes + 8192    `; \
	$(MKEXT2IMG) -a -d $(1) -b $(5) -N $$num_inodes -m 0 $(2)
	$(if $(strip $(3)),\
		$(hide) $(TUNE2FS) -L $(strip $(3)) $(2))
	$(if $(strip $(4)),\
		$(hide) $(TUNE2FS) -j $(2))
	$(TUNE2FS) -C 1 $(2) -O filetype
	$(E2FSCK) -fy $(2) ; [ $$? -lt 4 ]
endef

#Temp use for froyo, file name is *_ext4.img, because we will use it on GB in ext4 file system.
file := $(PRODUCT_OUT)/system_ext4.img
$(file): $(PRODUCT_OUT)/system.img
	$(MAKE_EXT4FS) -s -l 163840k -L system -a system $@ $(PRODUCT_OUT)/system

droidcore: $(file)

file := $(PRODUCT_OUT)/userdata_ext4.img
$(file): $(PRODUCT_OUT)/userdata.img
	$(MAKE_EXT4FS) -s -l 225280k -L userdata -a data $@ $(PRODUCT_OUT)/data


droidcore: $(file)

