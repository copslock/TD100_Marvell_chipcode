package com.marvell.usbsetting;

import android.content.BroadcastReceiver;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceScreen;
import android.util.Log;
import android.os.Handler;
import android.os.Bundle;
import android.os.SystemProperties;
import android.os.AsyncResult;
import android.os.Message;
import android.media.AudioManager;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.TelephonyIntents;
import com.android.internal.telephony.TelephonyProperties;

import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;

public class UsbSettingListActivity extends PreferenceActivity implements
        Preference.OnPreferenceChangeListener {
    private static final boolean DBG = true;
    private static final String TAG = "UsbSetting";
    private static final String KEY_NETWORK_SETTING = "network_setting";
    private static final String KEY_BAND_SETTING = "band_preference";
    private static final String KEY_DIAG_SETTING = "diag_setting";
    private static final String KEY_CAMERA_SETTING = "camera_setting";
    private static final String KEY_LOOPBACK_SETTING = "loopback_setting";

    private AudioManager mAudioManager;
    /* Need to align with arrays.xml */
   
    private String DiagSettingCmds[] = {"diag_usb.cfg", "diag_sd.cfg", "diag_fs.cfg","diag_udp.cfg","diag_tcp.cfg"};
    private String RatSettingCmds[] = { "copy_2G_nvm.sh", "copy_3G_nvm.sh",
            "copy_DRAT_2G_nvm.sh", "copy_DRAT_3G_nvm.sh" };
    private String mCmdLine;
    private UsbSetting mUsbSetting;
    // private Handler mHandler = new Handler();

    private SharedPreferences mPreferences;
    private int mRatSettingValue;
    private int mDiagSettingValue;
    private int mCameraSettingValue;
    private int mLoopbackSettingValue;

    private Phone mPhone = null;
    private MyHandler mHandler;
    private TelephonyManager mTelephonyManager;
    // UI objects
    private ListPreference mButtonPreferredNetworkMode;
    private PreferenceScreen mGsmBandSetting;
    private PreferenceScreen mWcdmaBandSetting;
    private PreferenceScreen mTdBandSetting;

    public static final int PREF_KEY_RAT_SETTING_DEFAULT_VALUE = Phone.PREFERRED_NT_MODE; //4; //index in arrays.xml, default: 3G linda change use android default value.
    public static final String PREF_KEY_RAT_SETTING_VALUE = "PREF_KEY_RAT_SETTING_VALUE";
    public static final int PREF_KEY_DIAG_SETTING_DEFAULT_VALUE = 0; //index in arrays.xml, default:USB
    public static final String PREF_KEY_DIAG_SETTING_VALUE = "PREF_KEY_DIAG_SETTING_VALUE";
    public static final int PREF_KEY_CAMERA_SETTING_DEFAULT_VALUE = 0; //index in arrays.xml, default: MAIN CAMERA
    public static final String PREF_KEY_CAMERA_SETTING_VALUE = "PREF_KEY_CAMERA_SETTING_VALUE";
    public static final int PREF_KEY_LOOPBACK_SETTING_DEFAULT_VALUE = 0; //index in arrays.xml, default: MAIN CAMERA
    public static final String PREF_KEY_LOOPBACK_SETTING_VALUE = "PREF_KEY_LOOPBACK_SETTING_VALUE";

    public void readFromNonVolatile(Context context) {
        // read stored value.
        mPreferences = context.getSharedPreferences("MarvellSetting",
                Context.MODE_WORLD_READABLE | Context.MODE_WORLD_WRITEABLE);

        /*
         * mRatSettingValue = mPreferences.getInt(PREF_KEY_RAT_SETTING_VALUE,
         * PREF_KEY_RAT_SETTING_DEFAULT_VALUE);
         */
        mRatSettingValue = android.provider.Settings.Secure.getInt(mPhone
                .getContext().getContentResolver(),
                android.provider.Settings.Secure.PREFERRED_NETWORK_MODE,
                PREF_KEY_RAT_SETTING_DEFAULT_VALUE);

        mDiagSettingValue = mPreferences.getInt(PREF_KEY_DIAG_SETTING_VALUE,
                PREF_KEY_DIAG_SETTING_DEFAULT_VALUE);

        mCameraSettingValue = mPreferences.getInt(
                PREF_KEY_CAMERA_SETTING_VALUE,
                PREF_KEY_CAMERA_SETTING_DEFAULT_VALUE);

        mLoopbackSettingValue = mPreferences.getInt(
                PREF_KEY_LOOPBACK_SETTING_VALUE,
                PREF_KEY_LOOPBACK_SETTING_DEFAULT_VALUE);
    }

    public void saveToNonVolatile() {
        Editor preferenceEditor = mPreferences.edit();
        preferenceEditor.putInt(PREF_KEY_DIAG_SETTING_VALUE, mDiagSettingValue);
        preferenceEditor.putInt(PREF_KEY_CAMERA_SETTING_VALUE,
                mCameraSettingValue);
        preferenceEditor.commit();
        preferenceEditor.putInt(PREF_KEY_LOOPBACK_SETTING_VALUE,
                mLoopbackSettingValue);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mPhone = PhoneFactory.getDefaultPhone();
        mHandler = new MyHandler();
        mTelephonyManager = (TelephonyManager) getSystemService(TELEPHONY_SERVICE);
        addPreferencesFromResource(R.xml.usb_setting);
        readFromNonVolatile(getBaseContext());
        /*
         * ListPreference ratSettingPreference = (ListPreference)
         * findPreference(KEY_NETWORK_SETTING);
         * ratSettingPreference.setValue(String.valueOf(mRatSettingValue));
         * ratSettingPreference.setOnPreferenceChangeListener(this);
         */
        mButtonPreferredNetworkMode = (ListPreference) findPreference(KEY_NETWORK_SETTING);
        mButtonPreferredNetworkMode.setValue(String.valueOf(mRatSettingValue));
        mButtonPreferredNetworkMode.setOnPreferenceChangeListener(this);

        ListPreference DiagSettingPreference = (ListPreference) findPreference(KEY_DIAG_SETTING);
        DiagSettingPreference.setValue(String.valueOf(mDiagSettingValue));
        DiagSettingPreference.setOnPreferenceChangeListener(this);

        ListPreference CameraSettingPreference = (ListPreference) findPreference(KEY_CAMERA_SETTING);
        CameraSettingPreference.setValue(String.valueOf(mCameraSettingValue));
        CameraSettingPreference.setOnPreferenceChangeListener(this);

        ListPreference LoopbackSettingPreference = (ListPreference) findPreference(KEY_LOOPBACK_SETTING);
        LoopbackSettingPreference.setValue(String
                .valueOf(mLoopbackSettingValue));
        LoopbackSettingPreference.setOnPreferenceChangeListener(this);

        mAudioManager = (AudioManager) this
                .getSystemService(Context.AUDIO_SERVICE);
    }

    @Override
    protected void onResume() {
        super.onResume();

        if (DBG) {
            log("onResume");
        }
        mTelephonyManager.listen(mPhoneStateListener,
                PhoneStateListener.LISTEN_DATA_CONNECTION_STATE);
        // upon resumption from the sub-activity, make sure we re-enable the
        // preferences.
        getPreferenceScreen().setEnabled(true);

        if (getPreferenceScreen().findPreference(KEY_NETWORK_SETTING) != null) {
            mPhone
                    .getPreferredNetworkType(mHandler
                            .obtainMessage(MyHandler.MESSAGE_GET_PREFERRED_NETWORK_TYPE));
        }

    }

    @Override
    protected void onPause() {
        super.onPause();
        log("onPause");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mTelephonyManager.listen(mPhoneStateListener,
                PhoneStateListener.LISTEN_NONE);
        log("onDestroy");
    }

    public void runCmdLine() {
        mUsbSetting = new UsbSetting();
        mHandler.post(new Runnable() {
            public void run() {
                mUsbSetting.run(mCmdLine);
            }
        });
    }

    public boolean onPreferenceChange(Preference preference, Object objValue) {
        if (KEY_NETWORK_SETTING.equals(preference.getKey())) {
            int value = Integer.parseInt((String) objValue);
            // mCmdLine =
            // "cd /data/data/com.marvell.usbsetting/; rm copy_cp_nvm.sh; ln -s /marvell/tel/"
            // + RatSettingCmds[value] + " copy_cp_nvm.sh";
            mRatSettingValue = value;
            // linda use android set preferred network
            int settingsNetworkMode = android.provider.Settings.Secure.getInt(
                    mPhone.getContext().getContentResolver(),
                    android.provider.Settings.Secure.PREFERRED_NETWORK_MODE,
                    PREF_KEY_RAT_SETTING_DEFAULT_VALUE);
            if (value != settingsNetworkMode) {
                // Set the modem network mode
                mPhone
                        .setPreferredNetworkType(
                                value,
                                mHandler
                                        .obtainMessage(MyHandler.MESSAGE_SET_PREFERRED_NETWORK_TYPE));
            }
            return true;
        }

        else if (KEY_DIAG_SETTING.equals(preference.getKey())) {
            int value = Integer.parseInt((String) objValue);
            if (value == 2 || value == 3) {
                // mCmdLine =
                // "cd /data/data/com.marvell.usbsetting/; rm diag_bsp.cfg; rm run.sh;ln -s /marvell/tel/run_diag_net.sh run.sh; ln -s /marvell/tel/"
                // + DiagSettingCmds[value] + " diag_bsp.cfg";
            } else {
                // mCmdLine =
                // "cd /data/data/com.marvell.usbsetting/; rm diag_bsp.cfg; ln -s /marvell/tel/"
                // + DiagSettingCmds[value] + " diag_bsp.cfg;" +
                // "rm run.sh; ln -s /marvell/tel/" +
                // UsbSettingCmds[mUsbSettingValue] + " run.sh";
            }
            mCmdLine = "cd /data/data/com.marvell.usbsetting/; rm diag_bsp.cfg; ln -s /marvell/tel/"
                    + DiagSettingCmds[value] + " diag_bsp.cfg";
            mDiagSettingValue = value;
        }

        else if (KEY_CAMERA_SETTING.equals(preference.getKey())) {
            int value = Integer.parseInt((String) objValue);
            mCmdLine = "cd ./";// no ops

            String a = SystemProperties.get("service.camera.id");
            Log.d(TAG, " getprop=" + a);

            mCameraSettingValue = value;
            Log.d(TAG, " mCameraSettingValue=" + mCameraSettingValue);

            SystemProperties.set("service.camera.id", Integer
                    .toString(mCameraSettingValue));

            // a = SystemProperties.get("service.camera.id");
            // Log.d(TAG," getprop=" + a);

        } else if (KEY_LOOPBACK_SETTING.equals(preference.getKey())) {
            int value = Integer.parseInt((String) objValue);
            mCmdLine = "cd ./";// no ops

            mLoopbackSettingValue = value;
            Log.d(TAG, " mLoopbackSettingValue=" + mLoopbackSettingValue);
            mAudioManager.setParameters("Loopback=" + mLoopbackSettingValue);
        }

        runCmdLine();
        // Log.d(TAG, mCmdLine);

        saveToNonVolatile();
        return true;
    }

    private PhoneStateListener mPhoneStateListener = new PhoneStateListener() {
        @Override
        public void onDataConnectionStateChanged(int state) {
            Log.d(TAG, "onDataConnectionStateChanged");
            updatePrefNetworkButton();
        }
    };

    private void updatePrefNetworkButton() {
        // upon resumption from the sub-activity, make sure we re-enable the
        // preferences.
        getPreferenceScreen().setEnabled(true);

        if (getPreferenceScreen().findPreference(KEY_NETWORK_SETTING) != null) {
            mPhone
                    .getPreferredNetworkType(mHandler
                            .obtainMessage(MyHandler.MESSAGE_GET_PREFERRED_NETWORK_TYPE));
        }
    }

    private class MyHandler extends Handler {

        private static final int MESSAGE_GET_PREFERRED_NETWORK_TYPE = 0;
        private static final int MESSAGE_SET_PREFERRED_NETWORK_TYPE = 1;

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case MESSAGE_GET_PREFERRED_NETWORK_TYPE:
                handleGetPreferredNetworkTypeResponse(msg);
                break;

            case MESSAGE_SET_PREFERRED_NETWORK_TYPE:
                handleSetPreferredNetworkTypeResponse(msg);
                break;
            }
        }

        private void handleGetPreferredNetworkTypeResponse(Message msg) {
            AsyncResult ar = (AsyncResult) msg.obj;

            if (ar.exception == null) {
                int modemNetworkMode = ((int[]) ar.result)[0];

                if (DBG) {
                    log("handleGetPreferredNetworkTypeResponse: modemNetworkMode = "
                            + modemNetworkMode);
                }

                int settingsNetworkMode = android.provider.Settings.Secure
                        .getInt(
                                mPhone.getContext().getContentResolver(),
                                android.provider.Settings.Secure.PREFERRED_NETWORK_MODE,
                                PREF_KEY_RAT_SETTING_DEFAULT_VALUE);

                if (DBG) {
                    log("handleGetPreferredNetworkTypeReponse: settingsNetworkMode = "
                            + settingsNetworkMode);
                }

                // check that modemNetworkMode is from an accepted value
                if (modemNetworkMode == Phone.NT_MODE_WCDMA_PREF
                        || modemNetworkMode == Phone.NT_MODE_GSM_ONLY
                        || modemNetworkMode == Phone.NT_MODE_WCDMA_ONLY
                        || modemNetworkMode == Phone.NT_MODE_GSM_UMTS
                        || modemNetworkMode == Phone.NT_MODE_CDMA
                        || modemNetworkMode == Phone.NT_MODE_CDMA_NO_EVDO
                        || modemNetworkMode == Phone.NT_MODE_EVDO_NO_CDMA
                        || modemNetworkMode == Phone.NT_MODE_GLOBAL
                        || modemNetworkMode == 8) {
                    if (DBG) {
                        log("handleGetPreferredNetworkTypeResponse: if 1: modemNetworkMode = "
                                + modemNetworkMode);
                    }

                    // check changes in modemNetworkMode and updates
                    // settingsNetworkMode
                    if (modemNetworkMode != settingsNetworkMode) {
                        if (DBG) {
                            log("handleGetPreferredNetworkTypeResponse: if 2: "
                                    + "modemNetworkMode != settingsNetworkMode");
                        }

                        settingsNetworkMode = modemNetworkMode;

                        if (DBG) {
                            log("handleGetPreferredNetworkTypeResponse: if 2: "
                                    + "settingsNetworkMode = "
                                    + settingsNetworkMode);
                        }

                        // changes the Settings.System accordingly to
                        // modemNetworkMode
                        android.provider.Settings.Secure
                                .putInt(
                                        mPhone.getContext()
                                                .getContentResolver(),
                                        android.provider.Settings.Secure.PREFERRED_NETWORK_MODE,
                                        settingsNetworkMode);
                    }

                    // changes the mButtonPreferredNetworkMode accordingly to
                    // modemNetworkMode
                    mButtonPreferredNetworkMode.setValue(Integer
                            .toString(modemNetworkMode));
                } else {
                    if (DBG)
                        log("handleGetPreferredNetworkTypeResponse: else: reset to default");
                    resetNetworkModeToDefault();
                }
            }
        }

        private void handleSetPreferredNetworkTypeResponse(Message msg) {
            AsyncResult ar = (AsyncResult) msg.obj;

            if (ar.exception == null) {
                int networkMode = Integer.valueOf(
                        mButtonPreferredNetworkMode.getValue()).intValue();
                android.provider.Settings.Secure
                        .putInt(
                                mPhone.getContext().getContentResolver(),
                                android.provider.Settings.Secure.PREFERRED_NETWORK_MODE,
                                networkMode);
                // changes the mButtonPreferredNetworkMode accordingly to
                // modemNetworkMode
                mButtonPreferredNetworkMode.setValue(Integer
                        .toString(networkMode));
            } else {
                mPhone
                        .getPreferredNetworkType(obtainMessage(MESSAGE_GET_PREFERRED_NETWORK_TYPE));
            }
        }

        private void resetNetworkModeToDefault() {
            // set the mButtonPreferredNetworkMode
            mButtonPreferredNetworkMode.setValue(Integer
                    .toString(PREF_KEY_RAT_SETTING_DEFAULT_VALUE));
            // set the Settings.System
            android.provider.Settings.Secure.putInt(mPhone.getContext()
                    .getContentResolver(),
                    android.provider.Settings.Secure.PREFERRED_NETWORK_MODE,
                    PREF_KEY_RAT_SETTING_DEFAULT_VALUE);
            // Set the Modem
            mPhone
                    .setPreferredNetworkType(
                            PREF_KEY_RAT_SETTING_DEFAULT_VALUE,
                            this
                                    .obtainMessage(MyHandler.MESSAGE_SET_PREFERRED_NETWORK_TYPE));
        }
    }

    private static void log(String msg) {
        Log.d(TAG, msg);
    }
}
