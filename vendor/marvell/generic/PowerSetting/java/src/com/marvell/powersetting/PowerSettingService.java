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

import java.io.IOException;
import java.util.Arrays;
import java.util.Timer;

import android.app.Service;
import android.app.NotificationManager;
import android.app.Notification;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.os.Binder;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.os.PowerManager;
import android.os.SystemClock;
import android.util.Log;
import android.content.SharedPreferences;

public class PowerSettingService extends Service{

	BroadcastReceiver receiver;
	static final String TAG = "PowerSettingService";
    
	public static final String PREF_POLICY_ENABLE = "PREF_POLICY_ENABLE";
	public static final String PREF_FULL_WAKELOCK = "PREF_FULL_WAKELOCK";
	public static final String PREF_PARTIAL_WAKELOCK = "PREF_PARTIAL_WAKELOCK";
	public static final String PREF_IDLE = "PREF_IDLE";
	public static final String PREF_FREQ_SET = "PREF_FREQ_SET";
	public static final String PREF_FREQ_SET_DEFAULT = "624000";

	private static final int POWERSETTING_ID = 1;
	
	PowerManager mPm;
	PowerManager.WakeLock mFwl = null;
	PowerManager.WakeLock mPwl = null;
	NotificationManager mNotificationManager;
	SharedPreferences mPreferences;

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {	
		
		Log.i(TAG, "PowerSettingService onStartCommand called");
		mPm = (PowerManager)getSystemService(Context.POWER_SERVICE);
		mNotificationManager = (NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE);
		
		int icon = R.drawable.icon;
		CharSequence tickerText = "PowerSettingService";
		long when = System.currentTimeMillis();
		Notification notification = new Notification(icon, tickerText, when);
		
		Context context = getApplicationContext();
		CharSequence contentTitle = "PowerSettingService notification";
		CharSequence contentText = "PowerSettingService is started";
		Intent notificationIntent = new Intent(this, PowerSetting.class);
		PendingIntent contentIntent = PendingIntent.getActivity(this, 0, notificationIntent, 0);
		notification.setLatestEventInfo(context, contentTitle, contentText, contentIntent);

		mNotificationManager.notify(POWERSETTING_ID, notification);
	
		mPreferences = getSharedPreferences("PowerSetting", Context.MODE_WORLD_READABLE | Context.MODE_WORLD_WRITEABLE);
		if(mPreferences == null){
			Log.e(TAG, "get shared preference failed");
			return -1;
		}

		boolean powerPolicyEnable = mPreferences.getBoolean(PREF_POLICY_ENABLE, true);
		Log.i(TAG, "powerPolicyEnable is " + powerPolicyEnable);
		
		if(!powerPolicyEnable){
			//powerplicy is disabled, set to user previous chosen freq
			Log.i(TAG, "powerpolicy is disabled, set to user chosen freq in last boot");	
			String freqSet = mPreferences.getString(PREF_FREQ_SET, PREF_FREQ_SET_DEFAULT);

			setCpufreqdManual(freqSet);

		}

		boolean partialWakelock = mPreferences.getBoolean(PREF_PARTIAL_WAKELOCK, false);	
		boolean fullWakelock = mPreferences.getBoolean(PREF_FULL_WAKELOCK, false);	
		boolean idle = mPreferences.getBoolean(PREF_IDLE, false);
		if(partialWakelock){
			holdPartialWakelock();
		}
		if(fullWakelock){
			holdFullWakelock();		
		}
		if(idle){
			disableIdle();
		}
		
		return START_STICKY;
	}

	@Override
	public IBinder onBind(Intent intent) {
		return mBinder;
	}

	/**
 	** The IRemoteInterface is defined through IDL
	**/
	/*private final IPowerSettingService.Stub mBinder = new IPowerSettingService.Stub() {
		public int holdFullWakelock() {
			mFwl = mPm.newWakeLock(PowerManager.FULL_WAKE_LOCK, TAG);
			mFwl.acquire();
			Log.i(TAG, "full wakelock aquired");
		}

		public int holdPartialWakelock() {
			mPwl = mPm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, TAG);
			mPwl.acquire();
			Log.i(TAG, "partial wakelock aquired");
		}

		public int releaseFullWakelock() {
			if(mFwl != null){
				mFwl.release();
				Log.i(TAG, "full wakelock released");
				mFwl = null;
			}
		}


		ipublic int releasePartialWakelock() {
			if(mPwl != null){
				mPwl.release();
				Log.i(TAG, "full wakelock released");
				mPwl = null;
			}
		}
	};*/


	/** 
	** Class for clients to access.  Because we know this service always
	** runs in the same process as its clients, we don't need to deal with
	** IPC.
	**/
	public class PowerSettingBinder extends Binder {
		PowerSettingService getService() {
			return PowerSettingService.this;
		}
	}

	private final IBinder mBinder = new PowerSettingBinder();

	public int holdFullWakelock() {
		mFwl = mPm.newWakeLock(PowerManager.FULL_WAKE_LOCK, TAG);
		mFwl.acquire();
		Log.i(TAG, "full wakelock aquired");
		
		return 0;
	}

	public int holdPartialWakelock() {
		mPwl = mPm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, TAG);
		mPwl.acquire();
		Log.i(TAG, "partial wakelock aquired");
		
		return 0;
	}

	public int releaseFullWakelock() {
		if(mFwl != null){
			mFwl.release();
			Log.i(TAG, "full wakelock released");
			mFwl = null;
		}
		
		return 0;
	}


	public int releasePartialWakelock() {
		if(mPwl != null){
			mPwl.release();
			Log.i(TAG, "partial wakelock released");
			mPwl = null;
		}
		
		return 0;
	}

	public int disableIdle() {
		PowerFreq.disableIdle();	
		return 0;
	}

	public int enableIdle() {
		PowerFreq.enableIdle();	
		return 0;
	}

	public int setCpufreqdManual(String freq){

		PowerFreq.setCpufreqdManual();
		PowerFreq.setProfile(freq);

		return 0;
	}

	public int setCpufreqdAuto(){

		PowerFreq.setCpufreqdAuto();
		return 0;
	}

    @Override
    public void onDestroy() {
		Log.i(TAG, "PowerSettingService onDestroy called");
		mNotificationManager.cancel(POWERSETTING_ID);
    }

}

