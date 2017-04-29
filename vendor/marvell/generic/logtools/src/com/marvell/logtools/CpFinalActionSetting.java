/*****************************
 **  (C) Copyright 2011 Marvell International Ltd.
 **  All Rights Reserved
 **  Author Linda
 **  Create Date Aug 29 2011      
 */
package com.marvell.logtools;

import android.content.Intent;
import android.os.Bundle;
import android.os.SystemProperties;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.util.Log;

public class CpFinalActionSetting extends PreferenceActivity implements
        Preference.OnPreferenceChangeListener{
    
    static final boolean DBG = true;
    static final String TAG = "CpFinalActionSetting";
    
    private String mLastCpFinalAction;
    
    private static final String KEY_CP_FINAL_ACT_LIST = "CpFinalActionList";
    private ListPreference mPreferenceCpFinalActionSetting;
    
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.cp_final_action_setting); 
        mLastCpFinalAction = SystemProperties.get("gsm.cp.final.act", "0");
        if(DBG) Log.d(TAG, "get LastCpFinalAction = " + mLastCpFinalAction);
        mPreferenceCpFinalActionSetting = (ListPreference) findPreference(KEY_CP_FINAL_ACT_LIST);
        mPreferenceCpFinalActionSetting.setValue(mLastCpFinalAction);
        mPreferenceCpFinalActionSetting.setOnPreferenceChangeListener(this);
    }
    
    @Override
    public boolean onPreferenceChange(Preference preference, Object objValue) {
        if(KEY_CP_FINAL_ACT_LIST.equals(preference.getKey())) {
            if (DBG) Log.d(TAG, "getKey objValue is " + (String)objValue);
            int value = Integer.parseInt((String)objValue);
            Intent i = new Intent();                
            i.setClassName("com.marvell.logtools", "com.marvell.logtools.NVMEditorService");
            i.setFlags(7450+value);
            this.startService(i);
            return true;
        }
        return true;
    }

}
