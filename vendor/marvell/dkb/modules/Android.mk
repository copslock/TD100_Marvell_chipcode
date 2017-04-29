LOCAL_PATH:=$(call my-dir)

#ANDROID_PREBUILT_MODULES:= kernel/out/modules/ 
LOCAL_MODULE_TAGS := optional
MODULE_TARGET:=$(TARGET_OUT)/lib/modules

#add the modules here
modules:=
modules+=sd8787.ko
modules+=bt8787.ko
modules+=galcore.ko
modules+=mlan.ko
modules+=bmm.ko
#pmem is build-in From beta3.
#modules+=pmem.ko
modules+=mpdc_cm.ko
modules+=mpdc_css.ko
modules+=mpdc_hs.ko
modules+=cnm.ko
modules+=cmmb_if228.ko
modules+=smsmdtv.ko

define get-module-list
$(strip $(foreach mod, $(modules), $(word 1, $(subst :, ,$(mod))) ) )
endef

ifeq ($(strip $(ANDROID_PREBUILT_MODULES)),)
$(info ================================================)
$(info ANDROID_PREBUILT_MODULES variable is not set.)
$(info Please set the variable to the folder where prebuilt kernel modules locate.)
$(info Or you can set it to no_kernel_modules, if you don't care about the modules right now. )
$(info But you should be clear that the generated image won't include the kernel modules and android may not be able to boot up. )
$(info ================================================)
$(error Unable to decide where kernel modules locate.)

else

ifeq ($(strip $(ANDROID_PREBUILT_MODULES)),no_kernel_modules)

$(info ==================================================================)
$(info Kernel Modules: no kernel modules will be included in the generated package.)
$(info ==================================================================)

else

$(info ============================================================================)
$(info Kernel Modules: use prebuilt kernel modules from:$(ANDROID_PREBUILT_MODULES))
$(info ============================================================================)

define define-kernel-module-target
tw:=$$(subst :, ,$(1) )
mod_name:=$$(word 1, $$(tw) )
$$(MODULE_TARGET)/$$(mod_name): private_mod_name:=$$(mod_name)
$$(MODULE_TARGET)/$$(mod_name): $$(ANDROID_PREBUILT_MODULES)/$$(mod_name)
	@echo "prebuilt module: $$(ANDROID_PREBUILT_MODULES)/$$(private_mod_name)"
	$$(copy-file-to-target-with-cp)

#always copy the prebuilt modules as we may use the prebuilt modules from a tarball which may have an old time.
.PHONY:$$(MODULE_TARGET)/$$(mod_name)
ALL_PREBUILT += $$(MODULE_TARGET)/$$(mod_name)
endef
$(foreach m, $(modules), $(eval $(call define-kernel-module-target, $m) ) )

endif

endif


