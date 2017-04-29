/*****************************
 **  (C) Copyright 2011 Marvell International Ltd.
 **  All Rights Reserved
 **  Author Linda
 **  Create Date Jul 28 2011      
 */
package com.marvell.logtools;

import com.marvell.logtools.R;

import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceScreen;
import android.widget.Toast;
import android.content.Intent;
import android.os.Bundle;

public class LogSetting extends PreferenceActivity {    
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.log_setting);       
    }
    
    @Override
    public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        if(preference.getKey().equals("CurrentLogSetting")) {
            if (android.os.Environment.getExternalStorageState().equals(android.os.Environment.MEDIA_MOUNTED)){
                Intent intent = new Intent(this, CurLogSetting.class);
                this.startActivity(intent);
            } else {
                Toast.makeText(this, R.string.no_sdcard, Toast.LENGTH_LONG).show();
            }
        }else if(preference.getKey().equals("PoweronLogSetting")) {
            Intent intent = new Intent(this, PoweronLogSetting.class);
            this.startActivity(intent);
        }
        return true;
    }
   
}
