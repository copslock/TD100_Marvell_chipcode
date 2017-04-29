package com.marvell.usbsetting;

public class UsbSetting {
    static {
        System.loadLibrary("usbsetting");
    }

    public native int run(String cmdLine);

}
