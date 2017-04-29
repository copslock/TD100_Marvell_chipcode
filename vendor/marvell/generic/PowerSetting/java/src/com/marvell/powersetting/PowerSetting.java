/*
 * * (C) Copyright 2010 Marvell International Ltd.
 * * All Rights Reserved
 * *
 * * MARVELL CONFIDENTIAL
 * * Copyright 2008 ~ 2010 Marvell International Ltd All Rights Reserved.
 * * The source code contained or described herein and all documents related to
 * * the source code ("Material") are owned by Marvell International Ltd or its
 * * suppliers or licensors. Title to the Material remains with Marvell International Ltd
 * * or its suppliers and licensors. The Material contains trade secrets and
 * * proprietary and confidential information of Marvell or its suppliers and
 * * licensors. The Material is protected by worldwide copyright and trade secret
 * * laws and treaty provisions. No part of the Material may be used, copied,
 * * reproduced, modified, published, uploaded, posted, transmitted, distributed,
 * * or disclosed in any way without Marvell's prior express written permission.
 * *
 * * No license under any patent, copyright, trade secret or other intellectual
 * * property right is granted to or conferred upon you by disclosure or delivery
 * * of the Materials, either expressly, by implication, inducement, estoppel or
 * * otherwise. Any license under such intellectual property rights must be
 * * express and approved by Marvell in writing.
 * *
 * */

package com.marvell.powersetting;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.os.PowerManager;
import android.preference.CheckBoxPreference;
import android.preference.ListPreference;
import android.preference.PreferenceActivity;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceClickListener;
import android.util.Log;
import android.content.ServiceConnection;
import android.content.ComponentName;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;


public class PowerSetting extends PreferenceActivity implements Preference.OnPreferenceChangeListener, OnPreferenceClickListener{

	private static final String TAG = "PowerSetting";
	private static final String KEY_POWER_SETTING = "power_setting";
	private static final String KEY_POLICY_SETTING = "policy_setting";
	private static final String KEY_FULL_WAKELOCK = "full_wakelock";
	private static final String KEY_PARTIAL_WAKELOCK = "partial_wakelock";
	private static final String KEY_FREQ_SETTING = "freq_setting";
	private static final String KEY_IDLE = "idle_setting";
	
	public static final String PREF_SETTING_ENABLE = "PREF_SETTING_ENABLE";
	public static final String PREF_POLICY_ENABLE = "PREF_POLICY_ENABLE";
	public static final String PREF_FULL_WAKELOCK = "PREF_FULL_WAKELOCK";
	public static final String PREF_PARTIAL_WAKELOCK = "PREF_PARTIAL_WAKELOCK";
	public static final String PREF_IDLE = "PREF_IDLE";
	public static final String PREF_FREQ_SET = "PREF_FREQ_SET";
	public static final String PREF_FREQ_SET_DEFAULT = "624000";

	private SharedPreferences mPreferences;
	private boolean mPowerPolicyEnable;
	private boolean mPowerSettingEnable;
	private boolean mFullWakelock;
	private boolean mPartialWakelock;
	private boolean mIdle;
	private String mFreqSet = "624000";
	
	BroadcastReceiver mPolicyStateReceiver;	
	
	int[] enabledFreqs;
	CheckBoxPreference powerPolicyPreference;
	CheckBoxPreference powerSettingPreference;
	CheckBoxPreference fullWakelockPreference;
	CheckBoxPreference partialWakelockPreference;
	CheckBoxPreference idlePreference;
	ListPreference freqSettingPreference;
	
	private PowerSettingService mBoundService;

	
	public void readFromNonVolatile(Context context){

		mPreferences = context.getSharedPreferences("PowerSetting",
                Context.MODE_WORLD_READABLE | Context.MODE_WORLD_WRITEABLE);
		if(mPreferences == null){
			Log.e(TAG, "get shared preference failed");
			return;
		}
		
		mPowerSettingEnable = mPreferences.getBoolean(PREF_SETTING_ENABLE, false);
		mPowerPolicyEnable = mPreferences.getBoolean(PREF_POLICY_ENABLE, true);
		mFullWakelock = mPreferences.getBoolean(PREF_FULL_WAKELOCK, false);
		mPartialWakelock = mPreferences.getBoolean(PREF_PARTIAL_WAKELOCK, false);
		mIdle = mPreferences.getBoolean(PREF_IDLE, false);
		mFreqSet = mPreferences.getString(PREF_FREQ_SET, PREF_FREQ_SET_DEFAULT);
	}
	
	public void saveToNonVolatile(){
		Editor preferenceEditor = mPreferences.edit();
		preferenceEditor.putBoolean(PREF_SETTING_ENABLE, mPowerSettingEnable);
		preferenceEditor.putBoolean(PREF_POLICY_ENABLE, mPowerPolicyEnable);
		preferenceEditor.putBoolean(PREF_FULL_WAKELOCK, mFullWakelock);
		preferenceEditor.putBoolean(PREF_PARTIAL_WAKELOCK, mPartialWakelock);
		preferenceEditor.putBoolean(PREF_IDLE, mIdle);
		preferenceEditor.putString(PREF_FREQ_SET, mFreqSet);
		preferenceEditor.commit();
	}

	public void updateFreqValues(){
		//get entries from enabled freq, first cancel all constrains
		enabledFreqs = PowerFreq.getAllEnabledFreqs();
		
		int i = 0;
		String[] entries;
		
		if(enabledFreqs == null){
			Log.e(TAG, "get freqs from native error");
		}else{
			
			entries = new String[enabledFreqs.length];
			while(i < enabledFreqs.length){
				entries[i] = String.valueOf(enabledFreqs[i]);
				i++;
			}
		 
		       
			freqSettingPreference.setEntries(entries);
			freqSettingPreference.setEntryValues(entries);
		}
	}

	private void setRelatedEnabled(boolean enable){
	
		powerPolicyPreference.setEnabled(enable);
		fullWakelockPreference.setEnabled(enable);
		partialWakelockPreference.setEnabled(enable);
		idlePreference.setEnabled(enable);
		freqSettingPreference.setEnabled(enable);

	}

	private void clearRelatedCheckbox(){
		if(!mPowerPolicyEnable){
			mPowerPolicyEnable = true;
			powerPolicyPreference.setChecked(mPowerPolicyEnable);

			mBoundService.setCpufreqdAuto();
		}
		if(mFullWakelock){
			mFullWakelock = false;
			fullWakelockPreference.setChecked(mFullWakelock);

			mBoundService.releaseFullWakelock();
		}
		if(mPartialWakelock){
			mPartialWakelock = false;
			partialWakelockPreference.setChecked(mPartialWakelock);

			mBoundService.releasePartialWakelock();
		}
		if(mIdle){
			mIdle = false;
			idlePreference.setChecked(mIdle);

			mBoundService.enableIdle();		
		}

	}

	private ServiceConnection mConnection = new ServiceConnection() {
	        public void onServiceConnected(ComponentName className, IBinder service) {
	            // This is called when the connection with the service has been
	            // established, giving us the service object we can use to
	            // interact with the service.  Because we have bound to a explicit
	            // service that we know is running in our own process, we can
	            // cast its IBinder to a concrete class and directly access it.
		Log.i(TAG, "Service connected");
	            mBoundService = ((PowerSettingService.PowerSettingBinder)service).getService();
	        }

	        public void onServiceDisconnected(ComponentName className) {
	            // This is called when the connection with the service has been
	            // unexpectedly disconnected -- that is, its process crashed.
	            // Because it is running in our same process, we should never
	            // see this happen.
	        	Log.i(TAG, "Service disconnected");
	            mBoundService = null;
	        }
   };

	
	
	protected void onCreate(Bundle savedInstanceState) {
    	
		Log.i(TAG, "onCreate called");
		super.onCreate(savedInstanceState);
		addPreferencesFromResource(R.layout.main);
	       
		readFromNonVolatile(getBaseContext());
	 
		powerSettingPreference = (CheckBoxPreference)findPreference(KEY_POWER_SETTING);
		powerSettingPreference.setChecked(mPowerSettingEnable);
		powerSettingPreference.setOnPreferenceChangeListener(this);
	       
		powerPolicyPreference = (CheckBoxPreference)findPreference(KEY_POLICY_SETTING);
		powerPolicyPreference.setChecked(mPowerPolicyEnable);
		powerPolicyPreference.setOnPreferenceChangeListener(this);
		
		fullWakelockPreference = (CheckBoxPreference)findPreference(KEY_FULL_WAKELOCK);
		fullWakelockPreference.setChecked(mFullWakelock);
		fullWakelockPreference.setOnPreferenceChangeListener(this);
		
		partialWakelockPreference = (CheckBoxPreference)findPreference(KEY_PARTIAL_WAKELOCK);
		partialWakelockPreference.setChecked(mPartialWakelock);
		partialWakelockPreference.setOnPreferenceChangeListener(this);
	 
		idlePreference = (CheckBoxPreference)findPreference(KEY_IDLE);
		idlePreference.setChecked(mIdle);
		idlePreference.setOnPreferenceChangeListener(this);
	       
		freqSettingPreference =
		    (ListPreference) findPreference(KEY_FREQ_SETTING);
		updateFreqValues();
		freqSettingPreference.setValue(mFreqSet);

		freqSettingPreference.setOnPreferenceChangeListener(this);
		freqSettingPreference.setOnPreferenceClickListener(this);
		
		if(mPowerSettingEnable){
			Log.i(TAG, "bindService called");
			bindService(new Intent(PowerSetting.this, PowerSettingService.class), mConnection, Context.BIND_AUTO_CREATE);
			setRelatedEnabled(true);
		}
	}

	public void onDestroy(){
		Log.i(TAG, "onDestroy called");
		super.onDestroy();

		if(mPowerSettingEnable){
			Log.i(TAG, "unbindservice called");
			unbindService(mConnection);
			setRelatedEnabled(false);
		}
	}
	
	public boolean onPreferenceChange(Preference preference, Object newValue) {
		if(KEY_POWER_SETTING.equals(preference.getKey())){
			boolean enable = (Boolean)newValue;

			Intent sIntent = new Intent(PowerSetting.this, PowerSettingService.class);

			if(enable){
				//start and bind to service
				startService(sIntent);
				bindService(new Intent(PowerSetting.this, PowerSettingService.class), mConnection, Context.BIND_AUTO_CREATE);
				setRelatedEnabled(true);
			}else{
				//service related checkbox should be cleared
				clearRelatedCheckbox();
    			
				unbindService(mConnection);
				stopService(sIntent);
				
				setRelatedEnabled(false);
			}

			mPowerSettingEnable = enable;


		}else if(KEY_POLICY_SETTING.equals(preference.getKey())){
		
           		 boolean enable = (Boolean)newValue;

		    	if(enable){
				mBoundService.setCpufreqdAuto();
		    	}else{
				mBoundService.setCpufreqdManual(mFreqSet);
			}

		    	mPowerPolicyEnable = enable;
		}else if(KEY_FULL_WAKELOCK.equals(preference.getKey())){
			boolean checked = (Boolean)newValue;
			
			if(checked){			
				mBoundService.holdFullWakelock();
			}else{
				mBoundService.releaseFullWakelock();
			}

			mFullWakelock = checked;
		}else if(KEY_PARTIAL_WAKELOCK.equals(preference.getKey())){
			boolean checked = (Boolean)newValue;
			
			if(checked){
				mBoundService.holdPartialWakelock();
			}else{
				mBoundService.releasePartialWakelock();
			}

			mPartialWakelock = checked;
		}else if(KEY_IDLE.equals(preference.getKey())){
			boolean checked = (Boolean)newValue;
			
			if(checked){
				mBoundService.disableIdle();
			}else{
				mBoundService.enableIdle();
			}

			mIdle = checked;

		}else if(KEY_FREQ_SETTING.equals(preference.getKey())){
			mFreqSet = (String)newValue;
			    
			//first disable power policy if power policy service is enabled
			if(mPowerPolicyEnable){
				//uncheck
				mPowerPolicyEnable = false;
				powerPolicyPreference.setChecked(mPowerPolicyEnable);
			}
			
			mBoundService.setCpufreqdManual(mFreqSet);
		}

		saveToNonVolatile();
		return true;

	}

	public boolean onPreferenceClick(Preference preference) {
		updateFreqValues();
		return false;
	}
}
