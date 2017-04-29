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

public class GsmBandSetting extends PreferenceActivity implements
        Preference.OnPreferenceChangeListener {
    private static final boolean DBG = true;
    private static final String LOG_TAG = "GsmBandSetting";

    // GSM band bit definition
    private static final int GSMBAND_PGSM_900 = 0x01;
    private static final int GSMBAND_DCS_GSM_1800 = 0x02;
    private static final int GSMBAND_PCS_GSM_1900 = 0x04;
    private static final int GSMBAND_EGSM_900 = 0x08;
    private static final int GSMBAND_GSM_450 = 0x10;
    private static final int GSMBAND_GSM_480 = 0x20;
    private static final int GSMBAND_GSM_850 = 0x40;

    // Set command controls parameters for GSM/UMTS/TD user mode and optionally
    // band setting
    private int mMode;
    private int mModemBand = 0;
    private int mSettingBand;
    private int mRequest = 0xFF000000;

    // UI objects
    private PreferenceScreen mFirstPrefScreen;
    private CheckBoxPreference mPgsm_900;
    private CheckBoxPreference mGsm_1800;
    private CheckBoxPreference mGsm_1900;
    private CheckBoxPreference mEgsm_900;
    private CheckBoxPreference mGsm_450;
    private CheckBoxPreference mGsm_480;
    private CheckBoxPreference mGsm_850;

    // toast infomations
    private final int toastLongTime = Toast.LENGTH_LONG;
    private final String bandSettingFailure = "Oops, Band Setting failure!";

    // Menu Objects
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
        mPgsm_900 = (CheckBoxPreference) mFirstPrefScreen
                .findPreference("button_pgsm_900");
        mPgsm_900.setChecked((mInitModemBand & GSMBAND_PGSM_900) == 0x01);
        mPgsm_900.setOnPreferenceChangeListener(this);

        mGsm_1800 = (CheckBoxPreference) mFirstPrefScreen
                .findPreference("button_gsm_1800");
        mGsm_1800.setChecked((mInitModemBand & GSMBAND_DCS_GSM_1800) == 0x02);
        mGsm_1800.setOnPreferenceChangeListener(this);

        mGsm_1900 = (CheckBoxPreference) mFirstPrefScreen
                .findPreference("button_gsm_1900");
        mGsm_1900.setChecked((mInitModemBand & GSMBAND_PCS_GSM_1900) == 0x04);
        mGsm_1900.setOnPreferenceChangeListener(this);

        mEgsm_900 = (CheckBoxPreference) mFirstPrefScreen
                .findPreference("button_egsm_900");
        mEgsm_900.setChecked((mInitModemBand & GSMBAND_EGSM_900) == 0x08);
        mEgsm_900.setOnPreferenceChangeListener(this);

        mGsm_450 = (CheckBoxPreference) mFirstPrefScreen
                .findPreference("button_gsm_450");
        mGsm_450.setChecked((mInitModemBand & GSMBAND_GSM_450) == 0x10);
        mGsm_450.setOnPreferenceChangeListener(this);

        mGsm_480 = (CheckBoxPreference) mFirstPrefScreen
                .findPreference("button_gsm_480");
        mGsm_480.setChecked((mInitModemBand & GSMBAND_GSM_480) == 0x20);
        mGsm_480.setOnPreferenceChangeListener(this);

        mGsm_850 = (CheckBoxPreference) mFirstPrefScreen
                .findPreference("button_gsm_850");
        mGsm_850.setChecked((mInitModemBand & GSMBAND_GSM_850) == 0x40);
        mGsm_850.setOnPreferenceChangeListener(this);

    }

    @Override
    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        addPreferencesFromResource(R.xml.gsm_band_setting);
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
        if ((preference.getKey()).equals("button_pgsm_900")) {
            mPgsm_900.setChecked(!mPgsm_900.isChecked());
            if (mPgsm_900.isChecked()) {
                mSettingBand |= GSMBAND_PGSM_900;
            } else {
                mSettingBand &= (~GSMBAND_PGSM_900);
            }
            return true;
        } else if ((preference.getKey()).equals("button_gsm_1800")) {
            mGsm_1800.setChecked(!mGsm_1800.isChecked());
            if (mGsm_1800.isChecked()) {
                mSettingBand |= GSMBAND_DCS_GSM_1800;
            } else {
                mSettingBand &= (~GSMBAND_DCS_GSM_1800);
            }
            return true;
        } else if ((preference.getKey()).equals("button_gsm_1900")) {
            mGsm_1900.setChecked(!mGsm_1900.isChecked());
            if (mGsm_1900.isChecked()) {
                mSettingBand |= GSMBAND_PCS_GSM_1900;
            } else {
                mSettingBand &= (~GSMBAND_PCS_GSM_1900);
            }
            return true;
        } else if ((preference.getKey()).equals("button_egsm_900")) {
            mEgsm_900.setChecked(!mEgsm_900.isChecked());
            if (mEgsm_900.isChecked()) {
                mSettingBand |= GSMBAND_EGSM_900;
            } else {
                mSettingBand &= (~GSMBAND_EGSM_900);
            }
            return true;
        } else if ((preference.getKey()).equals("button_gsm_450")) {
            mGsm_450.setChecked(!mGsm_450.isChecked());
            if (mGsm_450.isChecked()) {
                mSettingBand |= GSMBAND_GSM_450;
            } else {
                mSettingBand &= (~GSMBAND_GSM_450);
            }
            return true;
        } else if ((preference.getKey()).equals("button_gsm_480")) {
            mGsm_480.setChecked(!mGsm_480.isChecked());
            if (mGsm_450.isChecked()) {
                mSettingBand |= GSMBAND_GSM_480;
            } else {
                mSettingBand &= (~GSMBAND_GSM_480);
            }
            return true;
        } else if ((preference.getKey()).equals("button_gsm_850")) {
            mGsm_850.setChecked(!mGsm_850.isChecked());
            if (mGsm_850.isChecked()) {
                mSettingBand |= GSMBAND_GSM_850;
            } else {
                mSettingBand &= (~GSMBAND_GSM_850);
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
                log("handleGetBandBalueResponse ar.exception is "
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
