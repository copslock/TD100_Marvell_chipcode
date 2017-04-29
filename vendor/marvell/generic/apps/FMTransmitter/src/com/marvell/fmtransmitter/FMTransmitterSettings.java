package com.marvell.fmtransmitter;

import com.marvell.fmmanager.FMRadioManager;
import com.marvell.fmmanager.FMRadioListener;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.preference.CheckBoxPreference;
import android.preference.EditTextPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceScreen;
import android.widget.Toast;
import android.util.Log;

public class FMTransmitterSettings extends PreferenceActivity implements Preference.OnPreferenceChangeListener {
    private static final String TAG = "FMTransmitterSettings";

    /* local preference name */
    private static String PREF_NAME = "fm_transmitter_settings";

    /* settings name */
    private static final String KEY_FM_TRANSMITTER = "apply_fm_transmitter";
    private static final String KEY_FM_FREQUENCY = "apply_fm_frequency";
    private static final String KEY_FM_MUTE = "apply_fm_mute";

    /* default values */
    private static final boolean DEFAULT_FM_ENABLED = false;
    private static final boolean DEFAULT_FM_MUTE = false;
    private static final String DEFAULT_FREQ = "87.5";

    /* preference elements */
    private CheckBoxPreference mEnablePref;
    private EditTextPreference mFreqPref;
    private CheckBoxPreference mMutePref;

    /* local variables */
    private boolean mEnable;
    private boolean mMute;
    private String mFreq;

    private static FMRadioManager mFMTransmitterManager = new FMRadioManager();
    private FMCallback mCallback = new FMCallback();

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        readInitValueFromResource();

        /* get screen elements */
        addPreferencesFromResource(R.xml.fm_transmitter_settings);

        mEnablePref = (CheckBoxPreference) findPreference(KEY_FM_TRANSMITTER);
        mEnablePref.setChecked(mEnable);

        mFreqPref = (EditTextPreference) findPreference(KEY_FM_FREQUENCY);
        mFreqPref.setOnPreferenceChangeListener(this);
        mFreqPref.setSummary(mFreq + " MHz");

        mMutePref = (CheckBoxPreference) findPreference(KEY_FM_MUTE);
        mMutePref.setChecked(mMute);

        mFMTransmitterManager.registerListener(mCallback);

        if (mEnable) {
            if (mFMTransmitterManager.isFMTx()) {
                mEnablePref.setSummary(null);
            } else {
                mEnablePref.setChecked(false);
                mEnablePref.setSummary(R.string.fm_transmitter);
            }
        }
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        final String key = preference.getKey();
        if (KEY_FM_FREQUENCY.equals(key)) {
            if ("".equals((String)newValue)) {
                Toast.makeText(this, "Input nothing!", Toast.LENGTH_LONG).show();
                return false;
            } else {
                mFreq = (String)newValue;
                // set channel
                setChannel();

                Log.v(TAG, "Freq is set to " + mFreq + " MHz");
                mFreqPref.setSummary(mFreq + " MHz");
            }
        }

        writeValueToPref();
        return true;
    }

    @Override
    public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        if (preference == mEnablePref) {
            mEnable = mEnablePref.isChecked();
            Log.v(TAG, "mEnabled is " + mEnable);

            if (mEnable) {
                int ret = enableFM();
                setChannel();
                setMute();

                if (ret < 0) {
                    mEnablePref.setChecked(false);
                    mEnablePref.setSummary(R.string.error_state);
                } else {
                    mEnablePref.setChecked(true);
                    mEnablePref.setSummary(null);
                }
            } else {
                int ret = disableFM();

                if (ret < 0) {
                    mEnablePref.setChecked(false);
                    mEnablePref.setSummary(R.string.error_state);
                } else {
                    mEnablePref.setChecked(false);
                    mEnablePref.setSummary(R.string.fm_transmitter);
                }
            }
        } else if (preference == mMutePref) {
            mMute = mMutePref.isChecked();
            Log.v(TAG, "mMuted is " + mMute);

            setMute();
        }

        writeValueToPref();
        return true;
    }

    private void readInitValueFromResource() {
        SharedPreferences settings = getSharedPreferences(PREF_NAME, 0);
        mEnable = settings.getBoolean(KEY_FM_TRANSMITTER, DEFAULT_FM_ENABLED);
        mMute = settings.getBoolean(KEY_FM_MUTE, DEFAULT_FM_MUTE);
        mFreq = settings.getString(KEY_FM_FREQUENCY, DEFAULT_FREQ);
    }

    private void writeValueToPref() {
        SharedPreferences settings = getSharedPreferences(PREF_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putBoolean(KEY_FM_TRANSMITTER, mEnable);
        editor.putBoolean(KEY_FM_MUTE, mMute);
        editor.putString(KEY_FM_FREQUENCY, mFreq);
        editor.commit();
    }

    private int enableFM() {
        Log.v(TAG, "Enable FM Transmitter");
        mFMTransmitterManager.powerOn();
        return mFMTransmitterManager.enableTx();
    }

    private int disableFM() {
        Log.v(TAG, "Disable FM Transmitter");
        int ret = mFMTransmitterManager.disableTx();
        mFMTransmitterManager.powerOff();
        return ret;
    }

    private void setMute() {
        Log.v(TAG, "Set FM Mute to " + mMute);
        mFMTransmitterManager.setTxMute(mMute);
    }

    private void setChannel() {
        float f_freq = Float.parseFloat(mFreq);
        if (f_freq < 87.5f) {
            Toast.makeText(this, "Input frequency is less than '87.5', will use 87.5 MHz", Toast.LENGTH_LONG).show();
            f_freq = 87.5f;
            mFreq = "87.5";
        } else if (f_freq > 108.1f) {
            Toast.makeText(this, "Input frequency is greater than '108.1', will use 108.1 MHz", Toast.LENGTH_LONG).show();
            f_freq = 108.1f;
            mFreq = "108.1";
        }

        int i_freq = (int)(f_freq * 10) * 100; // Convert from MHz to KHz
        // reset the restore mFreq
        mFreq = Float.toString((float)i_freq / 1000.0f);
        Log.v(TAG, "Call setchannel to set freq as " + i_freq + " KHz ");
        mFMTransmitterManager.setChannel(i_freq);
    }

    private void handleStateChanged(int mode) {
        Log.v(TAG, "State Changed mode = " + mode);
        if (mode == 1) {
            if (mEnable) {
                mFMTransmitterManager.disableTx();
                mEnablePref.setChecked(false);
                mEnablePref.setSummary(R.string.fm_transmitter);
                mEnablePref.setEnabled(true);
                mEnable = false;

                writeValueToPref();
            }
        }
    }

    private class FMCallback implements FMRadioListener {
        public void onFoundChannel(int freq) {}
        public void onGetRdsPs(byte[] name) {}
        public void onGetRssi(int rssi) {}
        public void onMonoStereoChanged(int monoStereo) {}
        public void onScanFinished() {}
        public void onStateChanged(int mode) {
            handleStateChanged(mode);
        }
        public void onRecordingStop(int reason){
        }
    } 
}
