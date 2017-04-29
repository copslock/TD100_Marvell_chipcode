ifeq ($(MRVL_RECOVERY_UPDATE), true)

LOCAL_PATH := $(call my-dir)
RECOVERY_PATH := $(LOCAL_PATH)

include $(RECOVERY_PATH)/ui/Mdroid.mk
include $(CLEAR_VARS)

#include $(RECOVERY_PATH)/updater/Mdroid.mk
#include $(CLEAR_VARS)

# -----------------------------------------------------------------
# Recovery image

MRVL_RECOVERY_IMAGE := $(PRODUCT_OUT)/ramdisk-recovery.img

recovery_initrc := $(call include-path-for, recovery)/etc/init.rc
#Magic recovery_kernel := $(INSTALLED_KERNEL_TARGET) # same as a non-recovery system
recovery_ramdisk := $(PRODUCT_OUT)/ramdisk-recovery.img
recovery_build_prop := $(INSTALLED_BUILD_PROP_TARGET)
recovery_binary := $(call intermediates-dir-for,EXECUTABLES,recovery)/recovery
recovery_resources_common := $(call include-path-for, recovery)/res
recovery_resources_private := $(strip $(wildcard $(TARGET_DEVICE_DIR)/recovery/res))
recovery_command_private := $(strip $(wildcard $(TARGET_DEVICE_DIR)/recovery/sbin))
#recovery_lib_private := $(strip $(wildcard $(TARGET_DEVICE_DIR)/recovery/lib))
recovery_resource_deps := $(shell find $(recovery_resources_common) \
  $(recovery_resources_private) -type f)
recovery_fstab := $(strip $(wildcard $(TARGET_DEVICE_DIR)/recovery/recovery.fstab))

ifeq ($(recovery_resources_private),)
  $(info No private recovery resources for TARGET_DEVICE $(TARGET_DEVICE))
endif

ifeq ($(recovery_fstab),)
  $(info No recovery.fstab for TARGET_DEVICE $(TARGET_DEVICE))
endif

#Magic INTERNAL_RECOVERYIMAGE_ARGS := \
#Magic 	$(addprefix --second ,$(INSTALLED_2NDBOOTLOADER_TARGET)) \
#Magic 	--kernel $(recovery_kernel) \
#Magic 	--ramdisk $(recovery_ramdisk)
#Magic
#Magic # Assumes this has already been stripped
#Magic ifdef BOARD_KERNEL_CMDLINE
#Magic   INTERNAL_RECOVERYIMAGE_ARGS += --cmdline "$(BOARD_KERNEL_CMDLINE)"
#Magic endif
#Magic ifdef BOARD_KERNEL_BASE
#Magic   INTERNAL_RECOVERYIMAGE_ARGS += --base $(BOARD_KERNEL_BASE)
#Magic endif
#Magic BOARD_KERNEL_PAGESIZE := $(strip $(BOARD_KERNEL_PAGESIZE))
#Magic ifdef BOARD_KERNEL_PAGESIZE
#Magic   INTERNAL_RECOVERYIMAGE_ARGS += --pagesize $(BOARD_KERNEL_PAGESIZE)
#Magic endif

# Keys authorized to sign OTA packages this build will accept.  The
# build always uses test-keys for this; release packaging tools will
# substitute other keys for this one.
OTA_PUBLIC_KEYS := $(SRC_TARGET_DIR)/product/security/testkey.x509.pem

# Generate a file containing the keys that will be read by the
# recovery binary.
RECOVERY_INSTALL_OTA_KEYS := \
	$(call intermediates-dir-for,PACKAGING,ota_keys)/keys
DUMPKEY_JAR := $(HOST_OUT_JAVA_LIBRARIES)/dumpkey.jar
$(RECOVERY_INSTALL_OTA_KEYS): PRIVATE_OTA_PUBLIC_KEYS := $(OTA_PUBLIC_KEYS)
$(RECOVERY_INSTALL_OTA_KEYS): $(OTA_PUBLIC_KEYS) $(DUMPKEY_JAR)
	@echo "DumpPublicKey: $@ <= $(PRIVATE_OTA_PUBLIC_KEYS)"
	@rm -rf $@
	@mkdir -p $(dir $@)
	java -jar $(DUMPKEY_JAR) $(PRIVATE_OTA_PUBLIC_KEYS) > $@

$(MRVL_RECOVERY_IMAGE): $(MKBOOTFS) $(MINIGZIP) \
		$(INSTALLED_RAMDISK_TARGET) \
		$(recovery_binary) \
		$(recovery_initrc) \
		$(recovery_build_prop) $(recovery_resource_deps) \
		$(recovery_fstab) \
		$(RECOVERY_INSTALL_OTA_KEYS)
	@echo ----- Making recovery image ------
	rm -rf $(TARGET_RECOVERY_OUT)
	mkdir -p $(TARGET_RECOVERY_OUT)
	mkdir -p $(TARGET_RECOVERY_ROOT_OUT)
	mkdir -p $(TARGET_RECOVERY_ROOT_OUT)/etc
	mkdir -p $(TARGET_RECOVERY_ROOT_OUT)/tmp
	echo Copying baseline ramdisk...
	cp -R $(TARGET_ROOT_OUT) $(TARGET_RECOVERY_OUT)
	rm $(TARGET_RECOVERY_ROOT_OUT)/init*.rc
	echo Modifying ramdisk contents...
	cp -f $(recovery_initrc) $(TARGET_RECOVERY_ROOT_OUT)/
	cp -rf $(TARGET_DEVICE_DIR)/recovery/init* $(TARGET_RECOVERY_ROOT_OUT)/
	cp -f $(recovery_binary) $(TARGET_RECOVERY_ROOT_OUT)/sbin/
	cp -rf $(recovery_resources_common) $(TARGET_RECOVERY_ROOT_OUT)/
#	$(foreach item,$(recovery_lib_private), \
#	  cp -rf $(item) $(TARGET_RECOVERY_ROOT_OUT)/)
	$(foreach item,$(recovery_resources_private), \
	  cp -rf $(item) $(TARGET_RECOVERY_ROOT_OUT)/)
	$(foreach item,$(recovery_command_private), \
	  cp -rf $(item) $(TARGET_RECOVERY_ROOT_OUT)/)
	$(foreach item,$(recovery_fstab), \
	  cp -f $(item) $(TARGET_RECOVERY_ROOT_OUT)/etc/recovery.fstab)
	cp $(RECOVERY_INSTALL_OTA_KEYS) $(TARGET_RECOVERY_ROOT_OUT)/res/keys
	cat $(INSTALLED_DEFAULT_PROP_TARGET) $(recovery_build_prop) \
	        > $(TARGET_RECOVERY_ROOT_OUT)/default.prop
	$(MKBOOTFS) $(TARGET_RECOVERY_ROOT_OUT) | $(MINIGZIP) > $(recovery_ramdisk)
#	$(MKBOOTIMG) $(INTERNAL_RECOVERYIMAGE_ARGS) --output $@
	@echo ----- Made recovery image -------- $@
#	$(hide) $(call assert-max-image-size,$@,$(BOARD_RECOVERYIMAGE_PARTITION_SIZE),raw)

.PHONY: mrvlrecoveryimg
mrvlrecoveryimg: $(MRVL_RECOVERY_IMAGE)

# Run the mrvlrecoveryimg rules by default.
droidcore: mrvlrecoveryimg

# -----------------------------------------------------------------
# A zip of the directories that map to the target filesystem.
# This zip can be used to create an OTA package or filesystem image
# as a post-build step.
#
name := $(TARGET_PRODUCT)
ifeq ($(TARGET_BUILD_TYPE),debug)
  name := $(name)_debug
endif
name := $(name)-target_files-$(FILE_NAME_TAG)

intermediates := $(call intermediates-dir-for,PACKAGING,target_files)
MRVL_BUILT_TARGET_FILES_PACKAGE := $(intermediates)/$(name).zip
$(MRVL_BUILT_TARGET_FILES_PACKAGE): intermediates := $(intermediates)
$(MRVL_BUILT_TARGET_FILES_PACKAGE): \
		zip_root := $(intermediates)/$(name)

# $(1): Directory to copy
# $(2): Location to copy it to
# The "ls -A" is to prevent "acp s/* d" from failing if s is empty.
define package_files-copy-root
  if [ -d "$(strip $(1))" -a "$$(ls -A $(1))" ]; then \
    mkdir -p $(2) && \
    $(ACP) -rd $(strip $(1))/* $(2); \
  fi
endef

built_ota_tools := \
	$(call intermediates-dir-for,EXECUTABLES,applypatch)/applypatch \
	$(call intermediates-dir-for,EXECUTABLES,applypatch_static)/applypatch_static \
	$(call intermediates-dir-for,EXECUTABLES,check_prereq)/check_prereq \
	$(call intermediates-dir-for,EXECUTABLES,updater)/updater
$(MRVL_BUILT_TARGET_FILES_PACKAGE): PRIVATE_OTA_TOOLS := $(built_ota_tools)

$(MRVL_BUILT_TARGET_FILES_PACKAGE): PRIVATE_RECOVERY_API_VERSION := $(RECOVERY_API_VERSION)

MSA_IMG := $(shell ls $(PRODUCT_OUT)/../../../../kernel/out/telephony | grep "TT.*AI_A1_Flash.bin")

ARBEL_IMG := $(shell ls $(PRODUCT_OUT)/../../../../kernel/out/telephony | grep "Arbel.*.bin")

ifeq ($(TARGET_RELEASETOOLS_EXTENSIONS),)
# default to common dir for device vendor
$(MRVL_BUILT_TARGET_FILES_PACKAGE): tool_extensions := $(TARGET_DEVICE_DIR)/../common
else
$(MRVL_BUILT_TARGET_FILES_PACKAGE): tool_extensions := $(TARGET_RELEASETOOLS_EXTENSIONS)
endif

# Depending on the various images guarantees that the underlying
# directories are up-to-date.
$(MRVL_BUILT_TARGET_FILES_PACKAGE): \
		$(INSTALLED_RADIOIMAGE_TARGET) \
		$(MRVL_RECOVERY_IMAGE) \
		$(INSTALLED_SYSTEMIMAGE) \
		$(INSTALLED_USERDATAIMAGE_TARGET) \
		$(INSTALLED_ANDROID_INFO_TXT_TARGET) \
		$(built_ota_tools) \
		$(APKCERTS_FILE) \
		$(HOST_OUT_EXECUTABLES)/fs_config \
		| $(ACP)
	@echo "Package target files: $@"
	$(hide) rm -rf $@ $(zip_root)
	$(hide) mkdir -p $(dir $@) $(zip_root)
	@# Components of the recovery image
	$(hide) mkdir -p $(zip_root)/RECOVERY
	$(hide) $(call package_files-copy-root, \
		$(TARGET_RECOVERY_ROOT_OUT),$(zip_root)/RECOVERY/RAMDISK)
ifdef INSTALLED_KERNEL_TARGET
	$(hide) $(ACP) $(INSTALLED_KERNEL_TARGET) $(zip_root)/RECOVERY/kernel
endif
ifdef INSTALLED_2NDBOOTLOADER_TARGET
	$(hide) $(ACP) \
		$(INSTALLED_2NDBOOTLOADER_TARGET) $(zip_root)/RECOVERY/second
endif
ifdef BOARD_KERNEL_CMDLINE
	$(hide) echo "$(BOARD_KERNEL_CMDLINE)" > $(zip_root)/RECOVERY/cmdline
endif
ifdef BOARD_KERNEL_BASE
	$(hide) echo "$(BOARD_KERNEL_BASE)" > $(zip_root)/RECOVERY/base
endif
ifdef BOARD_KERNEL_PAGESIZE
	$(hide) echo "$(BOARD_KERNEL_PAGESIZE)" > $(zip_root)/RECOVERY/pagesize
endif
	@# Components of the boot image
	$(hide) mkdir -p $(zip_root)/BOOT
	$(hide) $(call package_files-copy-root, \
		$(TARGET_ROOT_OUT),$(zip_root)/BOOT/RAMDISK)
ifdef INSTALLED_KERNEL_TARGET
	$(hide) $(ACP) $(INSTALLED_KERNEL_TARGET) $(zip_root)/BOOT/kernel
endif
ifdef INSTALLED_2NDBOOTLOADER_TARGET
	$(hide) $(ACP) \
		$(INSTALLED_2NDBOOTLOADER_TARGET) $(zip_root)/BOOT/second
endif
ifdef BOARD_KERNEL_CMDLINE
	$(hide) echo "$(BOARD_KERNEL_CMDLINE)" > $(zip_root)/BOOT/cmdline
endif
ifdef BOARD_KERNEL_BASE
	$(hide) echo "$(BOARD_KERNEL_BASE)" > $(zip_root)/BOOT/base
endif
ifdef BOARD_KERNEL_PAGESIZE
	$(hide) echo "$(BOARD_KERNEL_PAGESIZE)" > $(zip_root)/BOOT/pagesize
endif
	$(hide) $(foreach t,$(INSTALLED_RADIOIMAGE_TARGET),\
	            mkdir -p $(zip_root)/RADIO; \
	            $(ACP) $(t) $(zip_root)/RADIO/$(notdir $(t));)
	@# Contents of the system image
	$(hide) $(call package_files-copy-root, \
		$(SYSTEMIMAGE_SOURCE_DIR),$(zip_root)/SYSTEM)
	@# Contents of the data image
	$(hide) $(call package_files-copy-root, \
		$(TARGET_OUT_DATA),$(zip_root)/DATA)
	@# Extra contents of the OTA package
	$(hide) mkdir -p $(zip_root)/OTA/bin
	$(hide) $(ACP) $(INSTALLED_ANDROID_INFO_TXT_TARGET) $(zip_root)/OTA/
	$(hide) $(ACP) $(PRIVATE_OTA_TOOLS) $(zip_root)/OTA/bin/
	@# Files that do not end up in any images, but are necessary to
	@# build them.
	$(hide) mkdir -p $(zip_root)/META
	$(hide) $(ACP) $(APKCERTS_FILE) $(zip_root)/META/apkcerts.txt
	$(hide)	echo "$(PRODUCT_OTA_PUBLIC_KEYS)" > $(zip_root)/META/otakeys.txt
	$(hide) echo "recovery_api_version=$(PRIVATE_RECOVERY_API_VERSION)" > $(zip_root)/META/misc_info.txt
ifdef BOARD_FLASH_BLOCK_SIZE
	$(hide) echo "blocksize=$(BOARD_FLASH_BLOCK_SIZE)" >> $(zip_root)/META/misc_info.txt
endif
ifdef BOARD_BOOTIMAGE_PARTITION_SIZE
	$(hide) echo "boot_size=$(BOARD_BOOTIMAGE_PARTITION_SIZE)" >> $(zip_root)/META/misc_info.txt
endif
ifdef BOARD_RECOVERYIMAGE_PARTITION_SIZE
	$(hide) echo "recovery_size=$(BOARD_RECOVERYIMAGE_PARTITION_SIZE)" >> $(zip_root)/META/misc_info.txt
endif
ifdef BOARD_SYSTEMIMAGE_PARTITION_SIZE
	$(hide) echo "system_size=$(BOARD_SYSTEMIMAGE_PARTITION_SIZE)" >> $(zip_root)/META/misc_info.txt
endif
ifdef BOARD_USERDATAIMAGE_PARTITION_SIZE
	$(hide) echo "userdata_size=$(BOARD_USERDATAIMAGE_PARTITION_SIZE)" >> $(zip_root)/META/misc_info.txt
endif
	$(hide) echo "tool_extensions=$(tool_extensions)" >> $(zip_root)/META/misc_info.txt
ifdef mkyaffs2_extra_flags
	$(hide) echo "mkyaffs2_extra_flags=$(mkyaffs2_extra_flags)" >> $(zip_root)/META/misc_info.txt
endif
ifdef INTERNAL_USERIMAGES_SPARSE_EXT_FLAG
	$(hide) echo "extfs_sparse_flag=$(INTERNAL_USERIMAGES_SPARSE_EXT_FLAG)" >> $(zip_root)/META/misc_info.txt
endif
	$(hide) echo "default_system_dev_certificate=$(DEFAULT_SYSTEM_DEV_CERTIFICATE)" >> $(zip_root)/META/misc_info.txt
ifdef PRODUCT_EXTRA_RECOVERY_KEYS
	$(hide) echo "extra_recovery_keys=$(PRODUCT_EXTRA_RECOVERY_KEYS)" >> $(zip_root)/META/misc_info.txt
endif
	@# Copy the images to the package
	$(hide) $(ACP) $(PRODUCT_OUT)/ramdisk.img $(zip_root)/
	$(hide) $(ACP) $(PRODUCT_OUT)/ramdisk-recovery.img $(zip_root)/
	$(hide) $(ACP) $(PRODUCT_OUT)/../../../../boot/out/u-boot.bin $(zip_root)/
	$(hide) $(ACP) $(PRODUCT_OUT)/../../../../kernel/out/uImage $(zip_root)/
	$(hide) $(ACP) $(PRODUCT_OUT)/../../../../kernel/out/telephony/$(MSA_IMG) $(zip_root)/
	$(hide) $(ACP) $(PRODUCT_OUT)/../../../../kernel/out/telephony/$(ARBEL_IMG) $(zip_root)/
	$(hide) $(ACP) -r $(PRODUCT_OUT)/../../../../vendor/marvell/generic/ttc_telephony/drivers/output/marvell $(zip_root)/marvell
	@# Zip everything up, preserving symlinks
	$(hide) (cd $(zip_root) && zip -qry ../$(notdir $@) .)
	@# Run fs_config on all the system, boot ramdisk, and recovery ramdisk files in the zip, and save the output
	$(hide) zipinfo -1 $@ | awk 'BEGIN { FS="SYSTEM/" } /^SYSTEM\// {print "system/" $$2}' | $(HOST_OUT_EXECUTABLES)/fs_config > $(zip_root)/META/filesystem_config.txt
	$(hide) zipinfo -1 $@ | awk 'BEGIN { FS="BOOT/RAMDISK/" } /^BOOT\/RAMDISK\// {print $$2}' | $(HOST_OUT_EXECUTABLES)/fs_config > $(zip_root)/META/boot_filesystem_config.txt
	$(hide) zipinfo -1 $@ | awk 'BEGIN { FS="RECOVERY/RAMDISK/" } /^RECOVERY\/RAMDISK\// {print $$2}' | $(HOST_OUT_EXECUTABLES)/fs_config > $(zip_root)/META/recovery_filesystem_config.txt
	$(hide) (cd $(zip_root) && zip -q ../$(notdir $@) META/*filesystem_config.txt)

mrvl_target-files-package: $(MRVL_BUILT_TARGET_FILES_PACKAGE)


ifneq ($(TARGET_PRODUCT),sdk)
ifeq ($(filter generic%,$(TARGET_DEVICE)),)
#ifneq ($(TARGET_NO_KERNEL),true)
ifneq ($(recovery_fstab),)

# -----------------------------------------------------------------
# OTA update package

name := $(TARGET_PRODUCT)
ifeq ($(TARGET_BUILD_TYPE),debug)
  name := $(name)_debug
endif
name := $(name)-ota-mrvl

MRVL_INTERNAL_OTA_PACKAGE_TARGET := $(PRODUCT_OUT)/$(name).zip

$(MRVL_INTERNAL_OTA_PACKAGE_TARGET): KEY_CERT_PAIR := $(DEFAULT_KEY_CERT_PAIR)

$(MRVL_INTERNAL_OTA_PACKAGE_TARGET): $(MRVL_BUILT_TARGET_FILES_PACKAGE) $(OTATOOLS)
	@echo "Marvell Package OTA: $@"
	$(hide) ./build/tools/releasetools/ota_from_target_files -v \
	   -p $(HOST_OUT) \
           -k $(KEY_CERT_PAIR) \
           $(MRVL_BUILT_TARGET_FILES_PACKAGE) $@

.PHONY: mrvlotapackage
mrvlotapackage: $(MRVL_INTERNAL_OTA_PACKAGE_TARGET)

#----------------------------------------------------------------
# OTA increment update package

inc_name := $(TARGET_PRODUCT)
ifeq ($(TARGET_BUILD_TYPE),debug)
  inc_name := $(inc_name)_debug
endif
inc_name := $(inc_name)-inc_ota-mrvl

INC_SRC_ZIP :=
INC_TAR_ZIP :=

MRVL_INTERNAL_INC_OTA_PACKAGE_TARGET := $(PRODUCT_OUT)/$(inc_name).zip

$(MRVL_INTERNAL_INC_OTA_PACKAGE_TARGET): KEY_CERT_PAIR := $(DEFAULT_KEY_CERT_PAIR)

$(MRVL_INTERNAL_INC_OTA_PACKAGE_TARGET): $(MRVL_BUILT_TARGET_FILES_PACKAGE) $(OTATOOLS)
	$(hide) if [ "$(INC_SRC_ZIP)x" == "x" ] || [ "$(INC_TAR_ZIP)x" == "x" ];then \
	  echo "INC_SRC_ZIP or INC_TAR_ZIP not set, please set them before make increment OTA package"; \
	  exit 1;fi
	$(hide) if (! [ -f $(INC_SRC_ZIP) ]) || (! [ -f $(INC_TAR_ZIP) ]);then \
	  echo "$(INC_SRC_ZIP) or $(INC_TAR_ZIP) not exist, please reset them"; \
	  exit 2;fi
	@echo "Marvell Package OTA: $@"
	$(hide) ./build/tools/releasetools/ota_from_target_files -v \
	  -p $(HOST_OUT) \
	  -k $(KEY_CERT_PAIR) \
	  -i $(INC_SRC_ZIP) \
	  $(INC_TAR_ZIP) $@

.PHONY: mrvlincotapackage
mrvlincotapackage: $(MRVL_INTERNAL_INC_OTA_PACKAGE_TARGET)

endif    # recovery_fstab is defined
#endif    # TARGET_NO_KERNEL != true
endif    # TARGET_DEVICE != generic*
endif    # TARGET_PRODUCT != sdk

endif #MRVL_RECOVERY_UPDATE=true?
