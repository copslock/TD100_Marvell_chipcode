LOCAL_PATH := $(call my-dir)

wifi_firmware1 := $(TARGET_OUT_ETC)/firmware/mrvl/sd8787_Ax.bin
$(wifi_firmware1) : $(LOCAL_PATH)/w8787_mfg_wlan_sdio_bt_sdio_ax.bin | $(ACP)
	$(transform-prebuilt-to-target)

wifi_firmware2 := $(TARGET_OUT_ETC)/firmware/mrvl/sd8787_wx.bin
$(wifi_firmware2) : $(LOCAL_PATH)/w8787_mfg_wlan_sdio_bt_sdio_wx.bin | $(ACP)
	$(transform-prebuilt-to-target)

ALL_PREBUILT += $(wifi_firmware1)
ALL_PREBUILT += $(wifi_firmware2)

