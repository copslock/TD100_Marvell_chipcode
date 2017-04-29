/*****************************
 * * (C) Copyright 2011 Marvell International Ltd.
 * * All Rights Reserved
 * * Author Linda
 * * Create Date Jan 26 2011
 */
package com.marvell.usbsetting;

import android.os.Bundle;
import android.preference.PreferenceActivity;
import android.preference.CheckBoxPreference;
import android.preference.PreferenceScreen;
import android.preference.Preference;
import android.content.Intent;
import android.util.Log;
import android.os.Handler;
import android.os.SystemProperties;
import android.os.AsyncResult;
import android.os.Message;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.TelephonyIntents;
import com.android.internal.telephony.TelephonyProperties;

public class WcdmaBandSetting extends PreferenceActivity implements
        Preference.OnPreferenceChangeListener {
    private static final boolean DBG = true;
    private static final String LOG_TAG = "WcdmaBandSetting";

    // UMTS band bit
    private static final int UMTSBAND_BAND_1 = 0x01; // IMT-2100
    private static final int UMTSBAND_BAND_2 = 0x02; // PCS-1900
    private static final int UMTSBAND_BAND_3 = 0x04; // DCS-1800
    private static final int UMTSBAND_BAND_4 = 0x08; // AWS-1700
    private static final int UMTSBAND_BAND_5 = 0x10; // CLR-850
    private static final int UMTSBAND_BAND_6 = 0x20; // 800Mhz
    private static final int UMTSBAND_BAND_7 = 0x40; // IMT-E 2600
    private static final int UMTSBAND_BAND_8 = 0x80; // GSM-900
    private static final int UMTSBAND_BAND_9 = 0x100; // not used

    // Set command controls parameters for GSM/UMTS/TD user mode and optionally
    // band setting
    private int mMode;
    private int mModemBand = 0;
    private int mSettingBand;
    private int mRequest = 0xFF000000;

    // UI objects
    private PreferenceScreen mFirstPrefScreen;
    private CheckBoxPreference mUmts_1;
    private CheckBoxPreference mUmts_2;
    private CheckBoxPreference mUmts_3;
    private CheckBoxPreference mUmts_4;
    private CheckBoxPreference mUmts_5;
    private CheckBoxPreference mUmts_6;
    private CheckBoxPreference mUmts_7;
    private CheckBoxPreference mUmts_8;
    private CheckBoxPreference mUmts_9;

    // toast infomations
    private final int toastLongTime = Toast.LENGTH_LONG;
    private final String bandSettingFailure = "Oops, Band Setting failure!";

    private static final int MENU_SAVE = Menu.FIRST;
    private static final int MENU_CANCEL = Menu.FIRST + 1;

    private Phone mPhone;
    private MyHandler mHandler;

    public void getInitValue() {
        mMode = android.provider.Settings.Secure.getInt(mPhone.getContext()
                .getContentResolver(),
                android.provider.Settings.Secure.PREFERRED_NETWORK_MODE,
                Phone.PREFERRED_NT_MODE);
        mModemBand = android.provider.Settings.Secure.getInt(mPhone
                .getContext().getContentResolver(), "gsm.marvell.band", 0);
        mSettingBand = mModemBand;
        if (DBG) {
            log("getInitValue mMode=" + mMode + ",mModemBand=" + mModemBand
                    + ",mSettingBand=" + mSettingBand);
        }
        InitAfterGetBand(mModemBand);
    }

    public void InitAfterGetBand(int mInitModemBand) {
        mUmts_1 = (CheckBoxPreference) mFirstPrefScreen
                .findPreference("button_umts_1");
        mUmts_1.setChecked((mInitModemBand & UMTSBAND_BAND_1) == 0x01);
        mUmts_1.setOnPreferenceChangeListener(this);

        mUmts_2 = (CheckBoxPreference) mFirstPrefScreen
                .findPreference("button_umts_2");
        mUmts_2.setChecked((mInitModemBand & UMTSBAND_BAND_2) == 0x02);
        mUmts_2.setOnPreferenceChangeListener(this);

        mUmts_3 = (CheckBoxPreference) mFirstPrefScreen
                .findPreference("button_umts_3");
        mUmts_3.setChecked((mInitModemBand & UMTSBAND_BAND_3) == 0x04);
        mUmts_3.setOnPreferenceChangeListener(this);

        mUmts_4 = (CheckBoxPreference) mFirstPrefScreen
                .findPreference("button_umts_4");
        mUmts_4.setChecked((mInitModemBand & UMTSBAND_BAND_4) == 0x08);
        mUmts_4.setOnPreferenceChangeListener(this);

        mUmts_5 = (CheckBoxPreference) mFirstPrefScreen
                .findPreference("button_umts_5");
        mUmts_5.setChecked((mInitModemBand & UMTSBAND_BAND_5) == 0x10);
        mUmts_5.setOnPreferenceChangeListener(this);

        mUmts_6 = (CheckBoxPreference) mFirstPrefScreen
                .findPreference("button_umts_6");
        mUmts_6.setChecked((mInitModemBand & UMTSBAND_BAND_6) == 0x20);
        mUmts_6.setOnPreferenceChangeListener(this);

        mUmts_7 = (CheckBoxPreference) mFirstPrefScreen
                .findPreference("button_umts_7");
        mUmts_7.setChecked((mInitModemBand & UMTSBAND_BAND_7) == 0x40);
        mUmts_7.setOnPreferenceChangeListener(this);

        mUmts_8 = (CheckBoxPreference) mFirstPrefScreen
                .findPreference("button_umts_8");
        mUmts_8.setChecked((mInitModemBand & UMTSBAND_BAND_8) == 0x80);
        mUmts_8.setOnPreferenceChangeListener(this);

        mUmts_9 = (CheckBoxPreference) mFirstPrefScreen
                .findPreference("button_umts_9");
        mUmts_9.setChecked((mInitModemBand & UMTSBAND_BAND_9) == 0x100);
        mUmts_9.setOnPreferenceChangeListener(this);
    }

    @Override
    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        addPreferencesFromResource(R.xml.wcdma_band_setting);
        mPhone = PhoneFactory.getDefaultPhone();
        mHandler = new MyHandler();
        mFirstPrefScreen = getPreferenceScreen();
        getInitValue();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        super.onCreateOptionsMenu(menu);
        menu.add(0, MENU_SAVE, 0, R.string.menu_save).setIcon(
                android.R.drawable.ic_menu_save);
        menu.add(0, MENU_CANCEL, 0, R.string.menu_cancel).setIcon(
                android.R.drawable.ic_menu_close_clear_cancel);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
        case MENU_SAVE:
            if (mSettingBand != mModemBand) {
                // set modem band value
                // ril-mm.c case RIL_REQUEST_SELECT_BAND:
                // 24~31bits indicate the setting rule, FF is the general
                // setting, 00 is the LGE setting, here we consider general
                // 23~16bits is the mode, 15~0bits is the settingBand
                mRequest = ((mMode << 16) | 0xFF000000)
                        | (mSettingBand & 0x0000FFFF);
                if (DBG) {
                    log("menu set Band mRequest=" + mRequest + ",mMode="
                            + mMode + ",mSettingBand" + mSettingBand);
                }
                mPhone.selectBand(mRequest, mHandler
                        .obtainMessage(MyHandler.MESSAGE_SET_BAND_VALUE));
            }
            finish();
            return true;
        case MENU_CANCEL:
            finish();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    public boolean onPreferenceChange(Preference preference, Object objValue) {
        boolean enable = (Boolean) objValue;
        if ((preference.getKey()).equals("button_umts_1")) {
            mUmts_1.setChecked(!mUmts_1.isChecked());
            if (mUmts_1.isChecked()) {
                mSettingBand |= UMTSBAND_BAND_1;
            } else {
                mSettingBand &= (~UMTSBAND_BAND_1);
            }
            return true;
        } else if ((preference.getKey()).equals("button_umts_2")) {
            mUmts_2.setChecked(!mUmts_2.isChecked());
            if (mUmts_2.isChecked()) {
                mSettingBand |= UMTSBAND_BAND_2;
            } else {
                mSettingBand &= (~UMTSBAND_BAND_2);
            }
            return true;
        } else if ((preference.getKey()).equals("button_umts_3")) {
            mUmts_3.setChecked(!mUmts_3.isChecked());
            if (mUmts_3.isChecked()) {
                mSettingBand |= UMTSBAND_BAND_3;
            } else {
                mSettingBand &= (~UMTSBAND_BAND_3);
            }
            return true;
        } else if ((preference.getKey()).equals("button_umts_4")) {
            mUmts_4.setChecked(!mUmts_4.isChecked());
            if (mUmts_4.isChecked()) {
                mSettingBand |= UMTSBAND_BAND_4;
            } else {
                mSettingBand &= (~UMTSBAND_BAND_4);
            }
            return true;
        } else if ((preference.getKey()).equals("button_umts_5")) {
            mUmts_5.setChecked(!mUmts_5.isChecked());
            if (mUmts_5.isChecked()) {
                mSettingBand |= UMTSBAND_BAND_5;
            } else {
                mSettingBand &= (~UMTSBAND_BAND_5);
            }
            return true;
        } else if ((preference.getKey()).equals("button_umts_6")) {
            mUmts_6.setChecked(!mUmts_6.isChecked());
            if (mUmts_6.isChecked()) {
                mSettingBand |= UMTSBAND_BAND_6;
            } else {
                mSettingBand &= (~UMTSBAND_BAND_6);
            }
            return true;
        } else if ((preference.getKey()).equals("button_umts_7")) {
            mUmts_7.setChecked(!mUmts_7.isChecked());
            if (mUmts_7.isChecked()) {
                mSettingBand |= UMTSBAND_BAND_7;
            } else {
                mSettingBand &= (~UMTSBAND_BAND_7);
            }
            return true;
        } else if ((preference.getKey()).equals("button_umts_8")) {
            mUmts_8.setChecked(!mUmts_8.isChecked());
            if (mUmts_8.isChecked()) {
                mSettingBand |= UMTSBAND_BAND_8;
            } else {
                mSettingBand &= (~UMTSBAND_BAND_8);
            }
            return true;
        } else if ((preference.getKey()).equals("button_umts_9")) {
            mUmts_9.setChecked(!mUmts_9.isChecked());
            if (mUmts_9.isChecked()) {
                mSettingBand |= UMTSBAND_BAND_9;
            } else {
                mSettingBand &= (~UMTSBAND_BAND_9);
            }
            return true;
        }
        return true;
    }

    private class MyHandler extends Handler {

        private static final int MESSAGE_GET_BAND_VALUE = 0;
        private static final int MESSAGE_SET_BAND_VALUE = 1;

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case MESSAGE_GET_BAND_VALUE:
                handleGetBandValueResponse(msg);
                break;

            case MESSAGE_SET_BAND_VALUE:
                handleSetBandValueResponse(msg);
                break;
            }
        }

        private void handleGetBandValueResponse(Message msg) {
            AsyncResult ar = (AsyncResult) msg.obj;
            if (DBG) {
                log("handleGetBandBalueResponse ar.exception is"
                        + (ar.exception == null));
            }
            if (ar.exception == null) {
                mModemBand = ((int[]) ar.result)[0];

                if (DBG) {
                    log("handleGetBandValueResponse: modem bandValue = "
                            + mModemBand);
                }
            } else {
                // if (DBG)
                // log("handleGetBandValueResponse: else: reset to default");
                // resetBandToDefault();
                if (DBG)
                    log("handleGetBandValueResponse: else: GET failure, do nothing ");
            }
        }

        private void handleSetBandValueResponse(Message msg) {
            AsyncResult ar = (AsyncResult) msg.obj;

            if (ar.exception == null) {
                if (DBG) {
                    log("handleSetBandValueResponse: set success");
                }
            } else {
                Toast t1 = Toast.makeText(getApplicationContext(),
                        bandSettingFailure, toastLongTime);
                t1.show();
                mPhone.getBand(obtainMessage(MESSAGE_GET_BAND_VALUE));
            }
        }

        private void resetBandToDefault() {
            // Set the Modem
            // ril-mm.c case RIL_REQUEST_SELECT_BAND:
            // 24~31bits indicate the setting rule, FF is the general setting,
            // 00 is the LGE setting, here we consider general
            // 23~16bits is the mode, 15~0bits is the settingBand
            mRequest = ((mMode << 16) | 0xFF000000) | (mModemBand & 0x0000FFFF);
            if (DBG) {
                log("reset Band to Default mRequest=" + mRequest + ",mMode="
                        + mMode + ",mModemBand" + mModemBand);
            }
            mPhone.selectBand(mRequest, this
                    .obtainMessage(MyHandler.MESSAGE_SET_BAND_VALUE));
        }
    }

    private static void log(String msg) {
        Log.d(LOG_TAG, msg);
    }
}
