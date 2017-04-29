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

public class UsbMenu extends Activity implements android.widget.AdapterView.OnItemClickListener {

    private final static String strACM = "acm";

    private final static String strMTP = "mtp";

    private final static String strMassStorage = "usb_mass_storage";

    private final static String strRNDIS = "rndis";

    private final static String strADB = "adb";

    private final static String strDiag = "diag";

    private static final String TAG = "USB Menu";

    private static final String strSeperator = ",";

    private static final String strScriptHead = "#!/system/bin/sh\r\n";

    private static final String strScriptAction = "echo";

    private static final String strScriptChar = ">";

    private static final String strScriptSep = " ";

    private static final String strEndLine = "\n";

    // private static final String strUSBFSInterfaceFile =
    // "/sys/devices/platform/pxa-u2o/composite";

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
        StringBuilder builder = new StringBuilder();
        switch ((int) id) {
            case 0:// Modem+ADB+Mass Storage
                builder.append(strACM);
                builder.append(strSeperator);
                builder.append(strADB);
                builder.append(strSeperator);
                builder.append(strMassStorage);
                break;
            case 1: // Modem+MassStorage
                builder.append(strACM);
                builder.append(strSeperator);
                builder.append(strMassStorage);
                break;
            case 2: // Mass Storage
                builder.append(strMassStorage);
                break;
            case 3: // RNDIS
                builder.append(strRNDIS);
                break;
            case 4: // ACM+DIAG
                builder.append(strACM);
                builder.append(strSeperator);
                builder.append(strDiag);
                break;
            case 5: // DIAG
                builder.append(strDiag);
                break;
            case 6:// Modem+DIAG+ADB+Mass Storage
                builder.append(strACM);
                builder.append(strSeperator);
                builder.append(strDiag);
                builder.append(strSeperator);
                builder.append(strADB);
                builder.append(strSeperator);
                builder.append(strMassStorage);
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
                break;
            default:
                Log.e(TAG, "received unknown item id (" + id + " )");
                return;
        }

        strMessage = builder.toString();
        sendUSBModeSelection(strMessage);
        saveUSBModeSelection(strMessage);

        this.finish();
    }

    private void sendUSBModeSelection(String strMessage) {
        Log.i(TAG, "sending configuration " + strMessage + " to the USB driver");
        String strFilePath;
        strFilePath = this.getString(R.string.USB_driver_fs_interface);
        File file = new File(strFilePath);
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
    }

    private void saveUSBModeSelection(String strMessage) {
        Log.i(TAG, "saving configuration " + strMessage + " to flash");
        String drvFileName, strFilePath, strScript;
        String strRndisConfig;
        StringBuilder builder = new StringBuilder();

        drvFileName = this.getString(R.string.USB_driver_fs_interface);
        strFilePath = this.getString(R.string.USB_mode_fs_interface);

        builder.append(strScriptHead);
        builder.append(strScriptAction);
        builder.append(strScriptSep);
        builder.append(strMessage);
        builder.append(strScriptSep);
        builder.append(strScriptChar);
        builder.append(strScriptSep);
        builder.append(drvFileName);

        // If RNDIS is in composite combination, set configuration to usbmode.sh
        if (strMessage.indexOf(strRNDIS, 0) >= 0) {
            strRndisConfig = this.getString(R.string.RNDIS_configure_setting);
            builder.append(strEndLine);
            builder.append(strRndisConfig);
        }

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
