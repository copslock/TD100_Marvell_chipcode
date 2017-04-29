/*****************************
 **  (C) Copyright 2011 Marvell International Ltd.
 **  All Rights Reserved
 **  Author Linda
 **  Create Date Sep 02 2011      
 */
package com.marvell.logtools;

import android.os.Bundle;
import android.os.SystemProperties;
import android.preference.CheckBoxPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.util.Log;
import android.widget.Toast;

public class PoweronLogSetting extends PreferenceActivity implements Preference.OnPreferenceChangeListener{
    static final boolean DBG = true;
    static final String TAG = "PoweronLogSetting";
    
    private CheckBoxPreference mMainBoxPreference;
    private CheckBoxPreference mRadioBoxPreference;
    private boolean mMainLogEnable;
    private boolean mRadioLogEnable;
    
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.poweron_log_setting);       
        mMainLogEnable = ((SystemProperties.get("persist.radio.mainlog.enable", "0").equals("1"))?true:false);
        mRadioLogEnable = ((SystemProperties.get("persist.radio.radiolog.enable", "0").equals("1"))?true:false);
        mMainBoxPreference = (CheckBoxPreference)getPreferenceScreen().findPreference("MainLogBox");
        mRadioBoxPreference = (CheckBoxPreference)getPreferenceScreen().findPreference("RadioLogBox");
        mMainBoxPreference.setChecked(mMainLogEnable);
        mMainBoxPreference.setOnPreferenceChangeListener(this);
        mRadioBoxPreference.setChecked(mRadioLogEnable);
        mRadioBoxPreference.setOnPreferenceChangeListener(this);
        if(DBG)
            Log.d(TAG, "getInitValue, Main," + mMainLogEnable + "; Radio," + mRadioLogEnable);
    }
    
    @Override
    public boolean onPreferenceChange(Preference preference, Object objValue) {
        if ((preference.getKey()).equals("MainLogBox")) {
            mMainBoxPreference.setChecked(!mMainBoxPreference.isChecked());
            mMainLogEnable = mMainBoxPreference.isChecked();
            SystemProperties.set("persist.radio.mainlog.enable", mMainLogEnable ? "1" : "0");
            if (mMainLogEnable) {
                Toast.makeText(this, "Please reset Phone to Enable MainLog from booting up", Toast.LENGTH_LONG).show();
            } else {
                Toast.makeText(this, "Please reset Phone to Disable MainLog from booting up", Toast.LENGTH_LONG).show();
            }
        } else if ((preference.getKey()).equals("RadioLogBox")) {
            mRadioBoxPreference.setChecked(!mRadioBoxPreference.isChecked()); 
            mRadioLogEnable = mRadioBoxPreference.isChecked();
            SystemProperties.set("persist.radio.radiolog.enable", mRadioLogEnable ? "1" : "0");
            if (mRadioLogEnable) {
                Toast.makeText(this, "Please reset Phone to Enable RadioLog from booting up", Toast.LENGTH_LONG).show();
            } else {
                Toast.makeText(this, "Please reset Phone to Disable RadioLog from booting up", Toast.LENGTH_LONG).show();
            }
        } else {
            if(DBG)
                Log.d(TAG, "onSharedPreferenceChanged, do nothing");
        }
        return true;
    }
   
}
