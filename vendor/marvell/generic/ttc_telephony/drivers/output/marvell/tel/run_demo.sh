#!/system/bin/sh
setprop persist.service.adb.enable 0
insmod g_android.ko
setprop persist.service.adb.enable 1

