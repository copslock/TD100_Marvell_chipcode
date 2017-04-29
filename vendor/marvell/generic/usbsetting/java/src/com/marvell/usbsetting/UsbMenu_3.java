/******************************************************************************

*(C) Copyright 2011 Marvell International Ltd.

* All Rights Reserved

******************************************************************************/

package com.marvell.usbsetting;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.os.SystemProperties;

public class UsbMenu_3 extends Activity implements android.widget.AdapterView.OnItemClickListener {

    private final static String strACM = "marvell_modem";

    private final static String strMTP = "mtp";

    private final static String strMassStorage = "mass_storage";

    private final static String strRNDIS = "rndis";

    private final static String strADB = "adb";

    private final static String strDiag = "marvell_diag";

    private static final String TAG = "USB Menu";

    private static final String strEnable = "1";

    private static final String strDisable = "0";

    private static final String strSeperator = ",";

    private static final String strScriptHead = "#!/system/bin/sh\n";

    private static final String strScriptAction = "echo";

    private static final String strScriptChar = ">";

    private static final String strScriptSep = " ";

    private static final String strEndLine = "\n";

    /*
        rndis, acm, diag, mass_storage, adb   0bb4:4e20
        acm, diag, mass_storage, adb          0bb4:4e22
        acm, mass_storage, adb                0bb4:4e2A
        rndis                                 1286:4e25
        mass_storage                          1286:4e26
        acm, diag                             1286:4e27
        acm, mass_storage                     1286:4e2B
        diag                                  1286:4e2C
    */
    private static final String strVid0bb4 = "0bb4";
    private static final String strVid1286 = "1286";

    private static final String strPid4e20 = "4e20";
    private static final String strPid4e22 = "4e22";
    private static final String strPid4e25 = "4e25";
    private static final String strPid4e26 = "4e26";
    private static final String strPid4e27 = "4e27";
    private static final String strPid4e2a = "4e2a";
    private static final String strPid4e2b = "4e2b";
    private static final String strPid4e2c = "4e2c";

    private ListView lstUSBMenu = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.xml.usb_menu);

        lstUSBMenu = (ListView) findViewById(R.id.lstUSBModes);
        lstUSBMenu.setOnItemClickListener(this);
    }

    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        Log.d(TAG, "received Item Click event. ID is " + id);
        String strMessage;
		String strVid, strPid;

        StringBuilder builder = new StringBuilder();
        switch ((int) id) {
            case 0:// Modem+ADB+Mass Storage
                builder.append(strACM);
                builder.append(strSeperator);
                builder.append(strADB);
                builder.append(strSeperator);
                builder.append(strMassStorage);
                //vid:pid:  0bb4:4e2A
                strVid = strVid0bb4;
                strPid = strPid4e2a;
                break;
            case 1: // Modem+MassStorage
                builder.append(strACM);
                builder.append(strSeperator);
                builder.append(strMassStorage);
                // vid:pid 1286:4e2B
                strVid = strVid1286;
                strPid = strPid4e2b;
                break;
            case 2: // Mass Storage
                builder.append(strMassStorage);
                //vid:pid 1286:4e26
                strVid = strVid1286;
                strPid = strPid4e26;
                break;
            case 3: // RNDIS
                builder.append(strRNDIS);
                // vid:pid 1286:4e25
                strVid = strVid1286;
                strPid = strPid4e25;
                break;
            case 4: // ACM+DIAG
                builder.append(strACM);
                builder.append(strSeperator);
                builder.append(strDiag);
                // vid:pid 1286:4e27
                strVid = strVid1286;
                strPid = strPid4e27;
                break;
            case 5: // DIAG
                builder.append(strDiag);
                // vid:pid 1286:4e2C
                strVid = strVid1286;
                strPid = strPid4e2c;
                break;
            case 6:// Modem+DIAG+ADB+Mass Storage
                builder.append(strACM);
                builder.append(strSeperator);
                builder.append(strDiag);
                builder.append(strSeperator);
                builder.append(strADB);
                builder.append(strSeperator);
                builder.append(strMassStorage);
                // vid:pid 0bb4:4e22
                strVid = strVid0bb4;
                strPid = strPid4e22;
                break;
            case 7:// RNDIS+Modem+DIAG+ADB+Mass Storage
                builder.append(strRNDIS);
                builder.append(strSeperator);
                builder.append(strACM);
                builder.append(strSeperator);
                builder.append(strDiag);
                builder.append(strSeperator);
                builder.append(strADB);
                builder.append(strSeperator);
                builder.append(strMassStorage);
                // vid:pid 0bb4:4e20
                strVid = strVid0bb4;
                strPid = strPid4e20;
                break;
            default:
                Log.e(TAG, "received unknown item id (" + id + " )");
                return;
        }

        strMessage = builder.toString();
        sendUSBModeSelection(strMessage, strVid, strPid);
        saveUSBModeSelection(strMessage, strVid, strPid);

        this.finish();
    }

    private void sendUSBModeSelection(String strMessage, String strVid, String strPid) {
        Log.i(TAG, "sending configuration " + strMessage + " to the USB driver");
        String strFilePath;
        File file;

        // disable all usb functions
        strFilePath = this.getString(R.string.USB_driver_fs_enable);
        file = new File(strFilePath);
        if (!file.exists()) {
            Log.e(TAG, "file " + strFilePath + " does not exists");
            return;
        }
        try {
            FileWriter writer = new FileWriter(file);
            writer.write(strDisable);
            writer.close();
        } catch (IOException e) {
            Log.e(TAG, "error writing to USB driver fs interface");
            e.printStackTrace();
        }

        // set new usb combination string
        strFilePath = this.getString(R.string.USB_driver_fs_functions);
        file = new File(strFilePath);
        if (!file.exists()) {
            Log.e(TAG, "file " + strFilePath + " does not exists");
            return;
        }
        try {
            FileWriter writer = new FileWriter(file);
            writer.write(strMessage);
            writer.close();
        } catch (IOException e) {
            Log.e(TAG, "error writing to USB driver fs interface");
            e.printStackTrace();
        }

        // set new usb Vid string
        strFilePath = this.getString(R.string.USB_driver_fs_idvendor);
        file = new File(strFilePath);
        if (!file.exists()) {
            Log.e(TAG, "file " + strFilePath + " does not exists");
            return;
        }
        try {
            FileWriter writer = new FileWriter(file);
            writer.write(strVid);
            writer.close();
        } catch (IOException e) {
            Log.e(TAG, "error writing to USB driver fs interface");
            e.printStackTrace();
        }

        // set new usb Vid string
        strFilePath = this.getString(R.string.USB_driver_fs_idproductor);
        file = new File(strFilePath);
        if (!file.exists()) {
            Log.e(TAG, "file " + strFilePath + " does not exists");
            return;
        }
        try {
            FileWriter writer = new FileWriter(file);
            writer.write(strPid);
            writer.close();
        } catch (IOException e) {
            Log.e(TAG, "error writing to USB driver fs interface");
            e.printStackTrace();
        }

        // enable all usb functions
        strFilePath = this.getString(R.string.USB_driver_fs_enable);
        file = new File(strFilePath);
        if (!file.exists()) {
            Log.e(TAG, "file " + strFilePath + " does not exists");
            return;
        }
        try {
            FileWriter writer = new FileWriter(file);
            writer.write(strEnable);
            writer.close();
        } catch (IOException e) {
            Log.e(TAG, "error writing to USB driver fs interface");
            e.printStackTrace();
        }

    }

    private void saveUSBModeSelection(String strMessage, String strVid, String strPid) {
        Log.i(TAG, "saving configuration " + strMessage + " to flash");
        String drvFileName, strFilePath, strScript;
        String strRndisConfig;
        StringBuilder builder = new StringBuilder();
        strFilePath = this.getString(R.string.USB_mode_fs_interface);

        builder.append(strScriptHead);

        //save disable functions
        drvFileName = this.getString(R.string.USB_driver_fs_enable);

        builder.append(strScriptAction);
        builder.append(strScriptSep);
        builder.append(strDisable);
        builder.append(strScriptSep);
        builder.append(strScriptChar);
        builder.append(strScriptSep);
        builder.append(drvFileName);
        builder.append(strEndLine);

        //save usb combination
        drvFileName = this.getString(R.string.USB_driver_fs_functions);

        builder.append(strScriptAction);
        builder.append(strScriptSep);
        builder.append(strMessage);
        builder.append(strScriptSep);
        builder.append(strScriptChar);
        builder.append(strScriptSep);
        builder.append(drvFileName);
        builder.append(strEndLine);

        // save vid
        drvFileName = this.getString(R.string.USB_driver_fs_idvendor);

        builder.append(strScriptAction);
        builder.append(strScriptSep);
        builder.append(strVid);
        builder.append(strScriptSep);
        builder.append(strScriptChar);
        builder.append(strScriptSep);
        builder.append(drvFileName);
        builder.append(strEndLine);

        // save pid
        drvFileName = this.getString(R.string.USB_driver_fs_idproductor);

        builder.append(strScriptAction);
        builder.append(strScriptSep);
        builder.append(strPid);
        builder.append(strScriptSep);
        builder.append(strScriptChar);
        builder.append(strScriptSep);
        builder.append(drvFileName);
        builder.append(strEndLine);

        // If RNDIS is in composite combination, set configuration to usbmode.sh
        if (strMessage.indexOf(strRNDIS, 0) >= 0) {
            strRndisConfig = this.getString(R.string.RNDIS_configure_setting);
            builder.append(strEndLine);
            builder.append(strRndisConfig);
            builder.append(strEndLine);
        }

        //save enable functions
        drvFileName = this.getString(R.string.USB_driver_fs_enable);

        builder.append(strScriptAction);
        builder.append(strScriptSep);
        builder.append(strEnable);
        builder.append(strScriptSep);
        builder.append(strScriptChar);
        builder.append(strScriptSep);
        builder.append(drvFileName);
        builder.append(strEndLine);

        // message to string and save it to file (usbmode.sh)
        strScript = builder.toString();

        File file = new File(strFilePath);

        try {
            FileWriter writer = new FileWriter(file);
            writer.write(strScript);
            writer.close();
        } catch (IOException e) {
            Log.e(TAG, "error writing to USB driver fs interface");
            e.printStackTrace();
        }
    }

}
