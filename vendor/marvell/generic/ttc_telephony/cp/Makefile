#
# Makefile for Tavor CP image
#

ifeq ($(MAKERULES),)
include ../../../Rules.make
else
include $(MAKERULES)
endif


ifeq "$(CONFIG_CPU_TAVOR_L)" "y"
PXA_SRC_PVK_CP_DIR=$(PXA_SRC_PVK_DIR)/ttc_telephony/cp
else
ifeq "$(CONFIG_CPU_TAVOR_TD)" "y"
PXA_SRC_PVK_CP_DIR=$(PXA_SRC_PVK_DIR)/ttc_telephony/cp
else
PXA_SRC_PVK_CP_DIR=$(PXA_SRC_PVK_DIR)/tavor_telephony/cp
endif
endif

ifeq "$(CONFIG_PLATFORM_SAAR)" "y"
CONFIG_PVK_CP_POLEGG=y
endif
ifeq "$(CONFIG_PLATFORM_EVB2)" "y"
CONFIG_PVK_CP_POLEG=y
endif

ifeq "$(CONFIG_CPU_TAVOR_B0)" "y"
NAME_ARBEL_BIN=Arbel_NVM_SAC_NOCOMMRTC.bin
NAME_GB_BIN=Tavor_M03_Poleg_AI_B0_Flash.bin
endif

ifeq "$(CONFIG_CPU_TAVOR_B1)" "y"
NAME_ARBEL_BIN=Arbel_NVM_SAC_NOCOMMRTC.bin
NAME_GB_BIN=Tavor_M05_Poleg_AI_B0_Flash.bin
endif

ifeq "$(CONFIG_CPU_TAVOR_PV)" "y"
NAME_ARBEL_BIN=Arbel_NVM_SAC_NOCOMMRTC.bin
NAME_GB_BIN=Tavor_M05_Poleg_AI_B0_Flash.bin
endif

.PHONY: all compile install-host install-target clean clean-local \
	uninstall-host uninstall-target

all: compile install-host install-target 

compile:

install-host:
ifeq "$(CONFIG_CPU_TAVOR_L)" "y"
ifeq "$(CONFIG_PLATFORM_EVB2)" "y"
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_p_ulc_evb/Arbel.bin $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_p_ulc_evb/Arbel_DIAG.mdb $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_p_ulc_evb/Arbel_NVM.mdb $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_p_ulc_evb/TTC1_M05_A0_AI_DDR.bin  $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_p_ulc_evb/TTC1_M05_A0_AI_Flash.bin  $(PXA_TARGET_IMAGE_DIR)
endif
ifeq "$(CONFIG_PLATFORM_DKB)" "y"
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_p_ulc_dkb/Arbel* $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_p_ulc_dkb/TTC1_M05_AI_A1_Flash.bin  $(PXA_TARGET_IMAGE_DIR)
endif
endif
ifeq "$(CONFIG_CPU_TAVOR_TD)" "y"
ifeq "$(CONFIG_PLATFORM_EVB2)" "y"
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_td_evb/Arbel.bin $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_td_evb/Arbel_DIAG.mdb $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_td_evb/Arbel_NVM.mdb $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_td_evb/TTC1_M05_A0_AI_DDR.bin  $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_td_evb/TTC1_M05_A0_AI_Flash.bin  $(PXA_TARGET_IMAGE_DIR)
endif
ifeq "$(CONFIG_PLATFORM_DKB)" "y"
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_td_dkb/Arbel* $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_td_dkb/TTD*  $(PXA_TARGET_IMAGE_DIR)
endif
endif

install-target:
ifeq "$(CONFIG_CPU_TAVOR_PV)" "y"
ifeq "$(CONFIG_PVK_CP_75)" "y"
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_pv_7.7/Poleg/Arbel.bin $(PXA_TARGET_IMAGE_DIR)/$(NAME_ARBEL_BIN)
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_pv_7.7/Poleg/Arbel_DIAG.mdb $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_pv_7.7/Poleg/Arbel_NVM.mdb $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_pv_7.7/Tavor_PV_M05_AI_A0_Flash.bin  $(PXA_TARGET_IMAGE_DIR)/$(NAME_GB_BIN)
endif
endif

ifeq "$(CONFIG_CPU_TAVOR_B0)" "y"
	cp -f $(PXA_SRC_PVK_CP_DIR)/B1.61/Poleg/Arbel_NVM_SAC_NOCOMMRTC.bin $(PXA_TARGET_IMAGE_DIR)/$(NAME_ARBEL_BIN)
	cp -f $(PXA_SRC_PVK_CP_DIR)/B1.61/Poleg/Arbel_NVM_SAC_NOCOMMRTC_DIAG.mdb $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/B1.61/Poleg/Arbel_NVM_SAC_NOCOMMRTC_NVM.mdb $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/B0/Tavor_M03_Poleg_AI_B0_Flash.bin  $(PXA_TARGET_IMAGE_DIR)/$(NAME_GB_BIN)
endif


ifeq "$(CONFIG_CPU_TAVOR_B1)" "y"

ifeq "$(CONFIG_PVK_CP_75)" "y"
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_p_7.7/Poleg/Arbel.bin $(PXA_TARGET_IMAGE_DIR)/$(NAME_ARBEL_BIN)
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_p_7.7/Poleg/Arbel_DIAG.mdb $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_p_7.7/Poleg/Arbel_NVM.mdb $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_p_7.7/Tavor_M05_B0_AI_Flash.bin  $(PXA_TARGET_IMAGE_DIR)/$(NAME_GB_BIN)
endif

ifeq "$(CONFIG_PVK_CP_70)" "y"
	cp -f $(PXA_SRC_PVK_CP_DIR)/B1.70/Poleg/Arbel.bin $(PXA_TARGET_IMAGE_DIR)/$(NAME_ARBEL_BIN)
	cp -f $(PXA_SRC_PVK_CP_DIR)/B1.70/Poleg/Arbel_DIAG.mdb $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/B1.70/Poleg/Arbel_NVM.mdb $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/B1.70/Tavor_M05_Poleg_AI_B0_Flash.bin  $(PXA_TARGET_IMAGE_DIR)/$(NAME_GB_BIN)
endif

ifeq "$(CONFIG_PVK_CP_65)" "y"
ifeq "$(CONFIG_PVK_CP_POLEG)" "y"
	cp -f $(PXA_SRC_PVK_CP_DIR)/B1.65/Poleg/Arbel.bin $(PXA_TARGET_IMAGE_DIR)/$(NAME_ARBEL_BIN)
	cp -f $(PXA_SRC_PVK_CP_DIR)/B1.65/Poleg/Arbel_DIAG.mdb $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/B1.65/Poleg/Arbel_NVM.mdb $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/B1.65/Tavor_M05_Poleg_AI_B0_Flash.bin  $(PXA_TARGET_IMAGE_DIR)/$(NAME_GB_BIN)
endif
ifeq "$(CONFIG_PVK_CP_POLEGG)" "y"
	cp -f $(PXA_SRC_PVK_CP_DIR)/B1.65/PolegG/Arbel_PLGG.bin $(PXA_TARGET_IMAGE_DIR)/$(NAME_ARBEL_BIN)
	cp -f $(PXA_SRC_PVK_CP_DIR)/B1.65/PolegG/Arbel_PLGG_DIAG.mdb $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/B1.65/PolegG/Arbel_PLGG_NVM.mdb $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/B1.65/Tavor_M05_Poleg_AI_B0_Flash.bin  $(PXA_TARGET_IMAGE_DIR)/$(NAME_GB_BIN)
endif

endif

ifeq "$(CONFIG_PVK_CP_61)" "y"
	cp -f $(PXA_SRC_PVK_CP_DIR)/B1.61/Poleg/Arbel_NVM_SAC_NOCOMMRTC.bin $(PXA_TARGET_IMAGE_DIR)/$(NAME_ARBEL_BIN)
	cp -f $(PXA_SRC_PVK_CP_DIR)/B1.61/Poleg/Arbel_NVM_SAC_NOCOMMRTC_DIAG.mdb $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/B1.61/Poleg/Arbel_NVM_SAC_NOCOMMRTC_NVM.mdb $(PXA_TARGET_IMAGE_DIR)
	cp -f $(PXA_SRC_PVK_CP_DIR)/B1.61/Tavor_M05_Poleg_AI_B0_Flash.bin  $(PXA_TARGET_IMAGE_DIR)/$(NAME_GB_BIN)
endif


endif
ifeq "$(CONFIG_CPU_TAVOR_L)" "y"

#	if [ ! -e $(PXA_TARGET_FS_DIR)/tel ];then mkdir $(PXA_TARGET_FS_DIR)/tel;fi
#	$(SUDO) cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_p_ulc/Arbel.bin $(PXA_TARGET_FS_DIR)/tel
#	$(SUDO) cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_p_ulc/TTC1_M05_A0_AI_DDR.bin  $(PXA_TARGET_FS_DIR)/tel
#	$(SUDO) cp -f $(PXA_SRC_PVK_CP_DIR)/tavor_p_ulc/TTC1_M05_A0_AI_Flash.bin  $(PXA_TARGET_FS_DIR)/tel
endif

clean: clean-local uninstall-host uninstall-target

clean-local:

uninstall-host:

uninstall-target:
	-rm -f $(PXA_TARGET_IMAGE_DIR)/Arbel*
	-rm -f $(PXA_TARGET_IMAGE_DIR)/Tavor_M*Flash.bin
	-rm -f $(PXA_TARGET_IMAGE_DIR)/Tavor_M*Flash_h.bin
