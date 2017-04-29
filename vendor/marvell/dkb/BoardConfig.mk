# config.mk
#
# Product-specific compile-time definitions.
#

# The generic product target doesn't have any hardware-specific pieces.
TARGET_NO_BOOTLOADER := true
TARGET_BOOTLOADER_BOARD_NAME := dkb

TARGET_NO_KERNEL := true
TARGET_PROVIDES_INIT_RC := true
HAVE_HTC_AUDIO_DRIVER := false
BOARD_USES_GENERIC_AUDIO :=false
BOARD_USES_RECORD_OVER_PCM := true
BOARD_ENABLE_ECPATH := false
BOARD_ENABLE_SRSWITCH := false
BOARD_ENABLE_VTOVERBT := true
BOARD_ENABLE_LOOPBACK := true
BOARD_ENABLE_BLUETOOTH_NREC := true
BOARD_ENABLE_HIFIEQ := true
BOARD_SUPPORT_MEDIA_SET_PROPERTY := false
USE_CAMERA_STUB := false
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_DUN := true
BOARD_WPA_SUPPLICANT_DRIVER := MARVELL
TARGET_CPU_ABI:=armeabi
TARGET_PROVIDE_GRALLOC:= true
USE_MARVELL_RIL := true
USE_MARVELL_MVED := false
USE_MARVELL_CNM := true
BOARD_ENABLE_FAST_OVERLAY := true
USE_FASTOVERLAY_CALLBACK := true
MRVL_SKIA_OPT := true

BOARD_ENABLE_GSTREAMER := true
BOARD_ENABLE_CMMB_PLAYENGINE := true

BOARD_GPS_LIBRARIES := liblsm_gsd4t

WPA_SUPPLICANT_VERSION := VER_0_6_X
BOARD_ENABLE_MARVELL_WIFI := true
BOARD_ENABLE_MARVELL_BLUETOOTH := true
#true: means not including helix libs in the build. but user can install the libraries later. This is to solve the license issue.
#Must have this declaration for autobuild.
BOARD_NO_HELIX_LIBS:=  

#true: means not including flash plugin lib in the build. but user can install the library later. This is to solve the license issue.
#Must have this declaration for autobuild.
BOARD_NO_FLASH_PLUGIN:=

MRVL_BGRA_HACK := false

BUILD_WITH_FULL_STAGEFRIGHT := false

#V8, JIT and WML
JS_ENGINE := v8
MARVELL_JIT_ENABLE := true
ENABLE_WML := true

#OpenCore 2way
BUILD_WITHOUT_PV := false
BUILD_PV_2WAY    := 1

#ALSA lib/utils support
BOARD_USES_ALSA_AUDIO := true
BUILD_WITH_ALSA_UTILS := true

#Enable OrganV2 feature
ORGAN_VOLUME_CALIBRATION:=true

FM_NOT_USES_RECORD := false

BOARD_ADDITIONAL_BOOTCLASSES += com.marvell.fmmanager:com.android.nfc_extras
BOARD_USES_SANREMO := true

#for DKB recovery
MRVL_RECOVERY_UPDATE := true
TARGET_RECOVERY_UI_LIB := librecovery_ui_dkb
TARGET_RELEASETOOLS_EXTENSIONS := vendor/marvell/dkb/recovery

#for EAP-SIM
BOARD_ENABLE_MARVELL_EAP_SIM := true

MARVELL_EXTENDED_RIL_REQUSTS := true
