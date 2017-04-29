package com.marvell.cpugadget;

import android.content.Context;
import android.content.Intent;
import android.util.Log;

import android.telephony.TelephonyManager;
import com.android.internal.telephony.Phone;

public class PluginIntent extends Intent {

	static final String TAG = "PluginIntent";
	static final String INTENT_PREFIX = "android.intent.action.";
	private String mAction;

	public static Intent createIntent(Intent intent){
		if(intent.getAction().equals(TelephonyManager.ACTION_PHONE_STATE_CHANGED)){
			return new PluginIntent(intent);
		}else{
			return intent;
		}

	}

	public PluginIntent(Intent intent){
		if(intent.getAction().equals(TelephonyManager.ACTION_PHONE_STATE_CHANGED)){
			String phoneState = intent.getStringExtra(Phone.STATE_KEY);
			Phone.State mPhoneState = (Phone.State) Enum.valueOf(Phone.State.class, phoneState);
			if(mPhoneState == Phone.State.OFFHOOK){
				//OFFHOOK
				Log.i(TAG, "phone active");
				mAction = INTENT_PREFIX + "PHONE_ACTIVE";
			}else{
				//ringing or idle
				Log.i(TAG, "phone idle or ringing state");
				mAction = INTENT_PREFIX + "PHONE_IDLE";
			}
		}else{
			Log.w(TAG, "Unknown intent");
		}
	}

	public String getAction(){
		return mAction;
	}
}
