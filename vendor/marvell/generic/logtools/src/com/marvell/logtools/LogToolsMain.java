/*****************************
 ** (C) Copyright 2011 Marvell International Ltd.
 ** All Rights Reserved
 ** Author Linda
 ** Create Date Jul 28 2011
 */
package com.marvell.logtools;

import com.marvell.logtools.R;

import android.os.Bundle;
import android.preference.PreferenceActivity;
import android.preference.PreferenceCategory;
import android.preference.PreferenceScreen;
import android.preference.Preference;
import android.content.Intent;
import android.net.Uri;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.os.Handler;
import android.os.Message;
import android.widget.Toast;
import android.util.Log;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;

public class LogToolsMain extends PreferenceActivity {
    static final boolean DBG = true;
    static final String TAG = "LogToolsMain";
    private Preference mALogcatPref;
    private Preference mSaveAllLogsPref;
    private Preference mMakeCpAssertPref;
    private Preference mLogSettingPref;
    private Preference mOtherSettingPref;

    private Phone mPhone;
    
    private static final int EVENT_SAVE_ALL_COMPLETED = 10;
    private static final int EVENT_MMI_COMPLETED = 11;
    //dialog id
    private static final int DIALOG_SAVE_ALL = 100;
    private static final int DIALOG_MAKE_CP_ASSERT = 101;
    
    protected boolean mIsForeground = false;
    String mSaveAllMsg;
    String mMakeCpAssertMsg;
    LogSaver mLogSaver;
    
    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg){
            switch (msg.what) {
            case EVENT_SAVE_ALL_COMPLETED:
                //boolean result = mLogSaver.saveall();
                removeDialog(DIALOG_SAVE_ALL);
                getPreferenceScreen().setEnabled(true);
                break;
            case EVENT_MMI_COMPLETED:
                mPhone.unregisterForMmiComplete(mHandler);
                removeDialog(DIALOG_MAKE_CP_ASSERT);
                getPreferenceScreen().setEnabled(true);
                break;
            }
            return;
        }
    };
    
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //addPreferencesFromResource(R.xml.logtools_main);
        mLogSaver = new LogSaver(this);
        setPreferenceScreen(createPreferenceHierarchy());
        mPhone = PhoneFactory.getDefaultPhone();
    }
    
    @Override
    public void onResume() {
        super.onResume();
        mIsForeground = true;
    }
    
    @Override
    public void onPause() {
        super.onPause();
        mIsForeground = false;
    }
    
    @Override
    protected Dialog onCreateDialog(int id) {
        if((id == DIALOG_SAVE_ALL) || (id == DIALOG_MAKE_CP_ASSERT)) {
            ProgressDialog dialog = new ProgressDialog(this);
            switch (id) {
            case DIALOG_MAKE_CP_ASSERT:
                dialog.setMessage(mMakeCpAssertMsg);
                dialog.setCancelable(false);
                dialog.setIndeterminate(true);
                break;
            case DIALOG_SAVE_ALL:
            default:
                dialog.setMessage(mSaveAllMsg);
                dialog.setCancelable(false);
                dialog.setIndeterminate(true);
                break;
            }
            return dialog;
        }
        return null;
    }
    
    @Override
    protected void onPrepareDialog(int id, Dialog dialog) {
        if (id == DIALOG_SAVE_ALL || id == DIALOG_MAKE_CP_ASSERT) {
            getPreferenceScreen().setEnabled(false);
        }
    }
    
    @Override
    public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        if(preference == mALogcatPref){
            Intent alogIntent = new Intent(this, LogActivity.class);
            this.startActivity(alogIntent);
        } else if(preference == mSaveAllLogsPref){
            if (android.os.Environment.getExternalStorageState().equals(android.os.Environment.MEDIA_MOUNTED)){
                String saveAllStr = mSaveAllLogsPref.getTitle().toString();
                saveAll();
                displaySaveAllInProgress (saveAllStr);
            } else {
                Toast.makeText(this, R.string.no_sdcard, Toast.LENGTH_LONG).show();
            }
        } else if(preference == mMakeCpAssertPref) {
            String makeCpAssertStr = mMakeCpAssertPref.getTitle().toString();
            mPhone.sendUssdResponse("##3424*9#");
            mPhone.registerForMmiComplete(mHandler,EVENT_MMI_COMPLETED,null);
            displayMakeCpAssertInProgress(makeCpAssertStr);
           
        } else if (preference == mLogSettingPref) {
            Intent logSettingIntent = new Intent(this, LogSetting.class);
            this.startActivity(logSettingIntent);
        } else if (preference == mOtherSettingPref) {
            Intent otherSettingIntent = new Intent(this, OtherSetting.class);
            this.startActivity(otherSettingIntent);
        }
        else {
            return true;
        }
        return true;
    }
    
    private void displaySaveAllInProgress(String saveAllStr) {
        mSaveAllMsg = getResources().getString(R.string.saving_all_log, saveAllStr);
        if (mIsForeground) {
            showDialog(DIALOG_SAVE_ALL);
        }
    }

    private void displayMakeCpAssertInProgress(String makeCpAssertStr) {
        mMakeCpAssertMsg = getResources().getString(R.string.make_cp_assert, makeCpAssertStr);
        if (mIsForeground) {
            showDialog(DIALOG_MAKE_CP_ASSERT);
        }
    }

    private PreferenceScreen createPreferenceHierarchy() {      
        // Root
        PreferenceScreen root = getPreferenceManager().createPreferenceScreen(this);
        
        // aLogcat
        PreferenceCategory aLogcatPrefCat = new PreferenceCategory(this);
        aLogcatPrefCat.setTitle(R.string.aLogcat_prefs);
        root.addPreference(aLogcatPrefCat);
        
        // aLogcat preference
        mALogcatPref = getPreferenceManager().createPreferenceScreen(this);
        mALogcatPref.setKey("alogcat_preference");
        mALogcatPref.setTitle(R.string.android_alog);
        mALogcatPref.setSummary(R.string.summary_android_alog);
        aLogcatPrefCat.addPreference(mALogcatPref);
                
        // SaveAllLog
        PreferenceCategory saveAllLogsPrefCat = new PreferenceCategory(this);
        saveAllLogsPrefCat.setTitle(R.string.saveAllLogs_prefs);
        root.addPreference(saveAllLogsPrefCat);
        
        // saveAllLogs preference
        mSaveAllLogsPref = getPreferenceManager().createPreferenceScreen(this);
        mSaveAllLogsPref.setKey("saveAllLogs_preference");
        mSaveAllLogsPref.setTitle(R.string.save_all);
        mSaveAllLogsPref.setSummary(R.string.summary_save_all_logs);
        saveAllLogsPrefCat.addPreference(mSaveAllLogsPref);
        
        // MakeCpAssert
        PreferenceCategory makeCpAssertPrefCat = new PreferenceCategory(this);
        makeCpAssertPrefCat.setTitle(R.string.makeCpAssert_prefs);
        root.addPreference(makeCpAssertPrefCat);
        
        // MakeCpAssert preference
        mMakeCpAssertPref = getPreferenceManager().createPreferenceScreen(this);
        mMakeCpAssertPref.setKey("makeCpAssert_preference");
        mMakeCpAssertPref.setTitle(R.string.make_cp_assert);
        mMakeCpAssertPref.setSummary(R.string.summary_make_cp_assert);
        makeCpAssertPrefCat.addPreference(mMakeCpAssertPref);
        
        //LogSetting
        PreferenceCategory logSettingPrefCat = new PreferenceCategory(this);
        logSettingPrefCat.setTitle(R.string.logSetting_prefs);
        root.addPreference(logSettingPrefCat);
        
        //LogSetting preference
        mLogSettingPref = getPreferenceManager().createPreferenceScreen(this);
        mLogSettingPref.setKey("logSetting_preference");
        mLogSettingPref.setTitle(R.string.log_setting_app);
        mLogSettingPref.setSummary(R.string.summary_log_setting);
        logSettingPrefCat.addPreference(mLogSettingPref);
        
        //OtherSettings
        PreferenceCategory otherSettingPrefCat = new PreferenceCategory(this);
        otherSettingPrefCat.setTitle(R.string.otherSetting_prefs);
        root.addPreference(otherSettingPrefCat);
        
        //OtherSetting preference
        mOtherSettingPref = getPreferenceManager().createPreferenceScreen(this);
        mOtherSettingPref.setKey("otherSetting_preference");
        mOtherSettingPref.setTitle(R.string.other_setting_app);
        mOtherSettingPref.setSummary(R.string.summary_other_setting);
        otherSettingPrefCat.addPreference(mOtherSettingPref);
        
        return root;
    }
    
    private void saveAll() {
        Thread mTasks = new Thread(){
            public void run(){
                boolean result = mLogSaver.saveall();
                if(DBG) Log.d(TAG, "saving result is" + result);
                Message msg = new Message();
                msg.what = EVENT_SAVE_ALL_COMPLETED;
                mHandler.sendMessage(msg);
            };
        };
        mTasks.start(); 
    }
}
