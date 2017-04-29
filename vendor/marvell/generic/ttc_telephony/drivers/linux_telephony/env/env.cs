element * CHECKEDOUT

element /pca_components/ci_stub/... CF_LINUX_CBA_CHGS_3
#====Columbia ===================================

element /columbia/... ENV_COLUMBIA_1.1.0

#====LINUX TELPHONEY===================================
element /linux_telephony/...  ENV_LINUX_TEL_2.1.0

#====LINUX BSP===================================

#element \linux_bsp\... VB_LINUX_BSP_1.0.0
#element \linux_bsp\... FL_BSP_CONFIG_FOR_PPP
#element \linux_bsp\... MATHIS_LINUX_BSP_ALPHA_1

#==== ENV and PrePass ====================================
element   /env/...  LINUX_CBA_1.1.1

#==== 2CHIP =============================================
element /pca_components/msl_dl/src/... LINUX_MSLDRV_1.0.0
element /pca_components/... PCAC_10.1.0 -nco

#==== GENLIB  =============================================
element      /genlib/...        PK_ATP_2.1.0
element      /genlib/...        GENLIB_2.0.0

#==== OS OSA ============================================

element -dir /os OS_ROOT -nco
element /os/osa/src/osa.c VCY_LINUX_OSA_ASSERT_FIX
element /os/... ...\vcy_linux_osa_msgrcv_fix\LATEST
element /os/... PK_OSA_3.4.0 -mkbranch vcy_linux_osa_msgrcv_fix

load \columbia
load \linux_telephony
#load \linux_bsp
load \pca_components
load \os\osa
load \genlib
load \env


