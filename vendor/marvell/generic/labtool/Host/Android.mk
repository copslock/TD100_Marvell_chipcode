## because of directory structure (i.e. this file should be under Host/)
## clean-labtool will NOT clean drvwrapper and DutCommon folders under
## myandroid/out/target/product/saarbpv2/obj/EXECUTABLES

LOCAL_PATH := $(my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := labtool

# W8787
LOCAL_SRC_FILES := DutApiWiFiBt/DutCalFileHandle.cpp DutApiWiFiBt/DutFuncs.cpp DutApiWiFiBt/W878X_Bt.cpp DutApiWiFiBt/W878X_Fm.cpp DutApiWiFiBt/w878X_Wifi.cpp DutApiWiFiBt/W878XApp.cpp 

#DutBt
LOCAL_SRC_FILES += DutApiWiFiBt/DutBtApi878XDll/DutBrf.cpp DutApiWiFiBt/DutBtApi878XDll/DutBtApiClss.cpp DutApiWiFiBt/DutBtApi878XDll/DutBtIf_IfClss.cpp DutApiWiFiBt/DutBtApi878XDll/DutBtu.cpp

#Common
LOCAL_SRC_FILES += DutCommon/mathUtility.c DutCommon/utilities.cpp DutCommon/utility.cpp DutCommon/utilities_os.cpp DutCommon/MenuDisplay.cpp drvwrapper/drv_wrapper.c

#Wifi
LOCAL_SRC_FILES += DutApiWiFiBt/DutWlanApi878XDll/DutIf_IfClss.cpp DutApiWiFiBt/DutWlanApi878XDll/DutWlanApi.cpp DutApiWiFiBt/DutWlanApi878XDll/DutWlanApiClss.cpp DutApiWiFiBt/DutWlanApi878XDll/DutXpartFileHandle.cpp

#Dut
LOCAL_SRC_FILES += DutApiWiFiBt/DutApi878XDll/DutApi878XDll.cpp DutApiWiFiBt/DutShareApi878XDll/DutShared_spiAccess.cpp DutApiWiFiBt/DutShareApi878XDll/DutSharedCal.cpp DutApiWiFiBt/DutShareApi878XDll/DutSharedCalData.cpp DutApiWiFiBt/DutShareApi878XDll/DutSharedCalFilehandle.cpp DutApiWiFiBt/DutShareApi878XDll/DutSharedCalTxt.cpp DutApiWiFiBt/DutShareApi878XDll/DutSpiImage.cpp

# GruBt
#LOCAL_SRC_FILES += DutApiWiFiBt/GruBtApi878XDll/DutBtApi878XLinkTest.cpp DutApiWiFiBt/GruBtApi878XDll/GruBtApiClss.cpp

# IF
#LOCAL_SRC_FILES += DutApiWiFiBt/DutIf/DutIf_SdioAdapter.cpp

#Fm
LOCAL_SRC_FILES += DutApiWiFiBt/DutFmApi878XDll/DutFmu.cpp

# LOCAL_MODULE_TAGS := eng development
LOCAL_SHARED_LIBRARIES := libbluetooth 

LOCAL_CFLAGS := -DNONPLUG_SUPPORT -DDEBUG -D_LINUX_ -D_LINUX_FEDORA_ -D_NO_CLASS_ -D_CLIENT_BG_ -D_WLAN_ -D_IF_SDIO_ -D_SDIO_ -D_MIMO_ -D_BT_ -D_WIFI_SDIO -D_HCI_PROTOCAL_ -D_W8787_  -D_FM_ -D_CAL_REV_D_ -DBIG_ENDIAN

INCLUDES = $(LOCAL_PATH)/DutApiWiFiBt/GruBtApi878XDll
INCLUDES += $(LOCAL_PATH)/DutApiWiFiBt/DutBtApi878XDll
INCLUDES += $(LOCAL_PATH)/DutApiWiFiBt
INCLUDES += $(LOCAL_PATH)/DutBtApi878XDll
INCLUDES += $(LOCAL_PATH)/DutCommon
INCLUDES += $(LOCAL_PATH)
INCLUDES += $(TOP)/external/bluetooth/bluez/include
INCLUDES += $(TOP)/external/bluetooth/bluez/lib
LOCAL_C_INCLUDES := $(INCLUDES)

include $(BUILD_EXECUTABLE)

setup_ini := $(TARGET_OUT_ETC)/labtool/SetUp.ini
$(setup_ini) : $(LOCAL_PATH)/DutApiWiFiBt/SetUp.ini | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(setup_ini)

eesd8787_sbin := $(TARGET_OUT_ETC)/labtool/eesd8787.sbin
$(eesd8787_sbin) : $(LOCAL_PATH)/DutApiWiFiBt/eesd8787.sbin | $(ACP)
	$(transform-prebuilt-to-target)
ALL_PREBUILT += $(eesd8787_sbin)

include $(LOCAL_PATH)/Android_lib.mk

