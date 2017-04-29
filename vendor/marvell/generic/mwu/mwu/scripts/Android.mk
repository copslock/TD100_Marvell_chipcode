LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# Any prebuilt files with default TAGS can use the below:
prebuilt_files := wifidir_init.conf uap.conf

$(call add-prebuilt-files, EXECUTABLES, $(prebuilt_files))
$(call add-prebuilt-files, ETC, wifidirect_defaults.conf)
