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

import java.io.FileInputStream;
import java.io.InputStreamReader;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.SystemClock;
import android.util.Log;
import android.os.SystemProperties;
import android.content.ServiceConnection;
import android.content.ComponentName;
import android.os.IBinder;

public class BootCompleteReceiver extends BroadcastReceiver{
	
	public static final String LOG_TAG = "PowerSettingBootCompleteReceiver";
	public static final String PREF_SETTING_ENABLE = "PREF_SETTING_ENABLE";
	public static final String PREF_POLICY_ENABLE = "PREF_POLICY_ENABLE";
	public static final String PREF_FULL_WAKELOCK = "PREF_FULL_WAKELOCK";
	public static final String PREF_PARTIAL_WAKELOCK = "PREF_PARTIAL_WAKELOCK";
	public static final String PREF_IDLE = "PREF_IDLE";
	public static final String PREF_FREQ_SET = "PREF_FREQ_SET";
	public static final String PREF_FREQ_SET_DEFAULT = "624";

	
	SharedPreferences mPreferences;

	@Override
	public void onReceive(Context context, Intent intent) {
		if (intent.getAction().equals(Intent.ACTION_BOOT_COMPLETED)) {
			//first check if we will remember setting after boot up
			Log.i(LOG_TAG, "PowerSettingBootComplete receiver");	
			mPreferences = context.getSharedPreferences("PowerSetting", Context.MODE_WORLD_READABLE | Context.MODE_WORLD_WRITEABLE);
			if(mPreferences == null){
				Log.e(LOG_TAG, "get shared preference failed");
				return;
			}
			
			boolean powerSettingEnable = mPreferences.getBoolean(PREF_SETTING_ENABLE, false);
			Log.i(LOG_TAG, "powerSettingEnable is " + powerSettingEnable);

			if(powerSettingEnable){
				//start PowerSetting Service
				Intent sIntent = new Intent();
				sIntent.setClassName("com.marvell.powersetting", "com.marvell.powersetting.PowerSettingService");

				context.startService(sIntent);
				Log.i(LOG_TAG, "start PowerSettingService" );
			}
			
		}
	}

}
