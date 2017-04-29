PRODUCT_PROPERTY_OVERRIDES :=

PRODUCT_PACKAGES := \
	AccountAndSyncSettings \
	CarHome \
	DeskClock \
    AlarmProvider \
	Bluetooth \
	Calculator \
    Calendar \
	Camera \
    CertInstaller \
    DrmProvider \
	Email \
	Gallery3D \
	Launcher2 \
	Mms \
	Music \
    Protips \
	QuickSearchBox \
	Settings \
    Sync \
    Updater \
    CalendarProvider \
    SyncProvider \
	LiveWallpapers \
	LiveWallpapersPicker \
	MagicSmokeWallpapers \
	VisualizationWallpapers \
	Fallback \
	SdkSetup \
	NotePad \
	Stk \
	SoundRecorder \
	SystemUI \
	libext4_utils \
	make_ext4fs \
	LatinIME
#	setup_fs
#	OtaUpdater

include vendor/marvell/dkb/optional_packages.mk
# include DBJTech
include vendor/marvell/dkb/dbjtech.mk

$(call inherit-product, $(SRC_TARGET_DIR)/product/core.mk)

#include the ringtones.
include frameworks/base/data/sounds/AudioPackage2.mk

# Get a full list of languages.
$(call inherit-product, build/target/product/languages_full.mk)

# # Get the TTS language packs
$(call inherit-product-if-exists, external/svox/pico/lang/all_pico_languages.mk)

# Overrides
PRODUCT_BRAND := marvell
PRODUCT_NAME := dkb 
PRODUCT_DEVICE := dkb
PRODUCT_LOCALES := zh_CN en_US hdpi

PRODUCT_COPY_FILES += \
	vendor/marvell/dkb/etc/apns-conf.xml:system/etc/apns-conf.xml

PRODUCT_COPY_FILES += \
	 vendor/marvell/dkb/bootanimation.zip:system/media/bootanimation.zip

