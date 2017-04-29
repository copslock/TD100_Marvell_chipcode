/*****************************
 * * (C) Copyright 2011 Marvell International Ltd.
 * * All Rights Reserved
 * * Author Linda
 * * Create Date Jan 26 2011
 */
package com.marvell.usbsetting;

import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceScreen;
import android.os.Bundle;
import android.content.Intent;
import android.os.SystemProperties;
import android.os.Handler;
import android.os.AsyncResult;
import android.os.Message;
import android.util.Log;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.TelephonyIntents;
import com.android.internal.telephony.TelephonyProperties;

import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;

public class ClassBandSetting extends PreferenceActivity {
    private static final boolean DBG = true;
    private static final String LOG_TAG = "BandSetting";
    // String keys for preference lookup
    private static final String BUTTON_PREFERED_GSM = "gsm_prefered_key";
    private static final String BUTTON_PREFERED_WCDMA = "wcdma_prefered_key";
    private static final String BUTTON_PREFERED_TD = "td_prefered_key";

    // Set command controls parameters for GSM/UMTS/TD user mode and optionally
    // band setting
    private int mMode;
    private String mBasebandInfo;

    private Phone mPhone;
    private MyHandler mHandler;
    private TelephonyManager mTelephonyManager;

    public void getInitValue() {
        mBasebandInfo = SystemProperties.get("gsm.version.baseband");
        mMode = android.provider.Settings.Secure.getInt(mPhone.getContext()
                .getContentResolver(),
                android.provider.Settings.Secure.PREFERRED_NETWORK_MODE,
                Phone.PREFERRED_NT_MODE);

        if (mMode == Phone.NT_MODE_GSM_ONLY) {
            findPreference(BUTTON_PREFERED_GSM).setEnabled(true);
        } else if ((mMode == Phone.NT_MODE_WCDMA_ONLY)
                && (mBasebandInfo.startsWith("TTD"))) {
            findPreference(BUTTON_PREFERED_TD).setEnabled(true);
        } else if ((mMode == Phone.NT_MODE_WCDMA_ONLY)
                && (mBasebandInfo.startsWith("TTC"))) {
            findPreference(BUTTON_PREFERED_WCDMA).setEnabled(true);
        }

        // move the get band value from UsbSettingListActivity to here
        // because user confused by screen change.
        mPhone
                .getBand(mHandler
                        .obtainMessage(MyHandler.MESSAGE_GET_BAND_VALUE));
    }

    @Override
    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        mPhone = PhoneFactory.getDefaultPhone();
        mHandler = new MyHandler();
        mTelephonyManager = (TelephonyManager) getSystemService(TELEPHONY_SERVICE);
        addPreferencesFromResource(R.xml.band_setting);
        getInitValue();
    }

    @Override
    protected void onResume() {
        super.onResume();

        if (DBG) {
            log("onResume");
        }
        mTelephonyManager.listen(mPhoneStateListener,
                PhoneStateListener.LISTEN_DATA_CONNECTION_STATE);

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mTelephonyManager.listen(mPhoneStateListener,
                PhoneStateListener.LISTEN_NONE);
        log("onDestroy");
    }

    private PhoneStateListener mPhoneStateListener = new PhoneStateListener() {
        @Override
        public void onDataConnectionStateChanged(int state) {
            if (DBG) {
                log("onDataConnectionStateChanged in BandSetting");
            }
            updateBandValue();
        }
    };

    private void updateBandValue() {
        mPhone
                .getBand(mHandler
                        .obtainMessage(MyHandler.MESSAGE_GET_BAND_VALUE));
    }

    private class MyHandler extends Handler {

        private static final int MESSAGE_GET_BAND_VALUE = 0;

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case MESSAGE_GET_BAND_VALUE:
                handleGetBandValueResponse(msg);
                break;
            }
        }

        private void handleGetBandValueResponse(Message msg) {
            AsyncResult ar = (AsyncResult) msg.obj;
            if (DBG) {
                log("handleGetBandValueResponse ar.exception is "
                        + (ar.exception == null));
            }

            if (ar.exception == null) {
                int modemBand = ((int[]) ar.result)[0];
                if (DBG) {
                    log("handleGetBandValueResponse : modemBand = " + modemBand);
                }
                // set Propertie
                android.provider.Settings.Secure.putInt(mPhone.getContext()
                        .getContentResolver(), "gsm.marvell.band", modemBand);
            } else {
                android.provider.Settings.Secure.putInt(mPhone.getContext()
                        .getContentResolver(), "gsm.marvell.band", 0);
            }
        }
    }

    private static void log(String msg) {
        Log.d(LOG_TAG, msg);
    }
}
