/*
 * (C) Copyright 2010 Marvell International Ltd.
 * All Rights Reserved
 *
 * MARVELL CONFIDENTIAL
 * Copyright 2008 ~ 2010 Marvell International Ltd All Rights Reserved.
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Marvell International Ltd or its
 * suppliers or licensors. Title to the Material remains with Marvell International Ltd
 * or its suppliers and licensors. The Material contains trade secrets and
 * proprietary and confidential information of Marvell or its suppliers and
 * licensors. The Material is protected by worldwide copyright and trade secret
 * laws and treaty provisions. No part of the Material may be used, copied,
 * reproduced, modified, published, uploaded, posted, transmitted, distributed,
 * or disclosed in any way without Marvell's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Marvell in writing.
 *
 */

package com.marvell.cmmb.common;

import java.util.List;
import java.util.TimeZone;
import android.app.Activity;
import android.app.ActivityManager;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.AudioManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Window;
import android.widget.Toast;

import com.marvell.cmmb.R;
import com.marvell.cmmb.activity.PurchaseManageActivity;
import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.resolver.MBBMSResolver;
import com.marvell.cmmb.service.MBBMSService;
import com.marvell.cmmb.service.MBBMSService.State;
import com.marvell.cmmb.view.dialog.LoadingDialog;
import com.marvell.cmmb.view.dialog.LoadingDialog.LoadBack;
import com.marvell.cmmb.view.dialog.PromptDialog;
import android.os.SystemProperties;

/**
 * 
 * The base of application
 * 
 * @author DanyangZhou
 * @version [version, 2011-5-12]
 */
public abstract class AppBase extends Activity implements Define, Action {
	public static final String TAG = "AppBase";

	public static long INSTANCE_COUNT;

	public MBBMSResolver mResolver;

	public AppBaseReceiver mReceiver;

	public static boolean isServiceStarted;

	public static boolean isResetApplication;
	
	public static boolean isResetEngine;
	
	private LoadingDialog mLoadingDialog;
	
	public static boolean isHeadPhonePlugged;
	
	public static boolean isNeedHeadPhone;

	private Handler mHandler = new Handler() {

		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case MESSAGE_APP_STARTED:
				isServiceStarted = true;
				AppBase.this.onAppStarted();
				break;
			case MESSAGE_APP_EXITED:
				isServiceStarted = false;
				AppBase.this.onAppExit();
				break;
			}
			super.handleMessage(msg);
		}

	};

	Thread mInitThread = new Thread() {
		@Override
		public void run() {
			startService(new Intent(AppBase.this, MBBMSService.class));
			mHandler.sendEmptyMessage(MESSAGE_APP_STARTED);
			LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "MBBMSService started",
					LogMask.APP_MAIN);
		}
	};

	private Toast mShortPromptToast;

	private Toast mLongPromptToast;

	private boolean isBack;

	protected void onAppExit() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "onAppExit()",
						LogMask.APP_MAIN);
		if (isResetApplication) {
			resetApp();
		}
	}

	protected void onAppStarted() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "onAppStarted()",
				LogMask.APP_MAIN);
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onCreate()",
				LogMask.APP_COMMON);
		INSTANCE_COUNT++;
		LogMask.setLogFlagFromProperty();
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		
		getProperty();		
		getHeadPhoneState();
		mResolver = MBBMSResolver.getInstance(this);
		registerReceiver();
		if(canContinue()) {			
			startApp();			
		}		
		
		super.onCreate(savedInstanceState);
	}

	@Override
	protected void onDestroy() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onDestroy()",
				LogMask.APP_COMMON);
		if (isBack) {
			isBack = false;
			onBackDestroy();
		}
		INSTANCE_COUNT--;
		if (INSTANCE_COUNT == 0) {
			exitApp();
		}
		unRegisterReceiver();
		super.onDestroy();
	}

	public void onBackDestroy() {

	}

	public boolean onKeyDown(int keyCode, KeyEvent event) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_BACK:
			isBack = true;
			break;
		}

		return super.onKeyDown(keyCode, event);
	}

	@Override
	protected void onPause() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onPause()",
				LogMask.APP_COMMON);
		super.onPause();
	}

	@Override
	protected void onResume() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onResume()",
				LogMask.APP_COMMON);
		setFormaterTimeZone();
		isResetEngine = checkIfEngineNeedReset();
		super.onResume();
	}

	@Override
	protected void onStart() {
		super.onStart();
	}

	@Override
	protected void onStop() {
		super.onStop();
	}

	public boolean startApp() {
		if (!checkMBBMSService()) {
			mInitThread.start();
			return true;
		}
		return false;
	}

	public boolean exitApp() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "exit application",
				LogMask.APP_MAIN);
		if (checkMBBMSService()
				&& MBBMSService.sPlayBackState == State.PlaybackStoped) {
			if (mInitThread != null && mInitThread.isAlive()) {
				try {
					mInitThread.join();
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
			stopService(new Intent(AppBase.this, MBBMSService.class));
			LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "MBBMSService stopped",
					LogMask.APP_MAIN);
			mHandler.sendEmptyMessage(MESSAGE_APP_EXITED);
			return true;
		}
		return false;
	}

	public boolean checkMBBMSService() {
		ActivityManager mActivityManager = (ActivityManager) getSystemService(ACTIVITY_SERVICE);
		List<ActivityManager.RunningServiceInfo> mServiceList = mActivityManager
				.getRunningServices(99);
		final String serviceClassName = MBBMSService.class.getName();
		if (mServiceList.size() > 0)
			for (int i = 0; i < mServiceList.size(); i++) {
				if (mServiceList.get(i).service.getClassName().equals(
						serviceClassName))
					return true;
			}
		return false;
	}

	private void registerReceiver() {
		mReceiver = new AppBaseReceiver();
		IntentFilter filter = new IntentFilter();
		filter.addAction(ACTION_MODE_CHANGED);
		filter.addAction(ACTION_BINDER_DIED);
		filter.addAction(ACTION_BINDER_DIED_EXIT);
		filter.addAction(ACTION_SHOW_BINDER_DIED_DIALOG);
		filter.addAction(ACTION_RESET_FINISHED);
		registerReceiver(mReceiver, filter);
	}

	private void unRegisterReceiver() {
		if (mReceiver != null) {
			unregisterReceiver(mReceiver);
			mReceiver = null;
		}
	}

	public Toast getShortPromptToast(int resid) {
		if (mShortPromptToast == null) {
			mShortPromptToast = Toast.makeText(this, resid, Toast.LENGTH_SHORT);
		}
		mShortPromptToast.setText(resid);
		return mShortPromptToast;
	}
	
	public Toast getShortPromptToast(String text) {
		if (mShortPromptToast == null) {
			mShortPromptToast = Toast.makeText(this, text, Toast.LENGTH_SHORT);
		}
		mShortPromptToast.setText(text);
		return mShortPromptToast;
	}

	public Toast getLongPromptToast(int resid) {
		if (mLongPromptToast == null) {
			mLongPromptToast = Toast.makeText(this, resid, Toast.LENGTH_LONG);
		}
		mLongPromptToast.setText(resid);
		return mLongPromptToast;
	}

	public abstract void initViews();

	public abstract void setupViews();

	public abstract void updateDate();

	public abstract void updateViews();

	class AppBaseReceiver extends BroadcastReceiver {

		@Override
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();
			if (action.equals(ACTION_MODE_CHANGED)) {
				LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
						"received mode exchange action, exit application",
						LogMask.APP_MAIN);
				isResetApplication = true;
				onBackDestroy();
				AppBase.this.finish();
			} else if (action.equals(ACTION_BINDER_DIED)) {
				LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
						"binder died, exit application", LogMask.APP_MAIN);
				isResetApplication = true;
				onBackDestroy();
				AppBase.this.finish();
			} else if (action.equals(ACTION_SHOW_BINDER_DIED_DIALOG)) {

			} else if (action.equals(ACTION_BINDER_DIED_EXIT)){
			    onBackDestroy();
                AppBase.this.finish();
			} else if (action.equals(ACTION_RESET_FINISHED)) {
				LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "reset finish, dismissDialog",
						LogMask.APP_MAIN);
				dismissLoadingDialog();
			}
		}
	}

	private void resetApp() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "restart application",
				LogMask.APP_MAIN);
		isResetApplication = false;
		startLoadingActivity();
	}

	private void startLoadingActivity() {
		Intent loadingIntent = new Intent();
		ComponentName comp = new ComponentName("com.marvell.cmmb",
				"com.marvell.cmmb.activity.LoadActivity");
		loadingIntent.addCategory("android.intent.category.LAUNCHER");
		loadingIntent.setAction("android.intent.action.MAIN");
		loadingIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK
				| Intent.FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);
		loadingIntent.setComponent(comp);
		startActivity(loadingIntent);
	}

	private void setFormaterTimeZone() {
		Util.sDateTimeFormater.setTimeZone(TimeZone.getDefault());
		Util.sDateFormater.setTimeZone(TimeZone.getDefault());
	}
	
	private boolean checkIfEngineNeedReset() {
		if(checkMBBMSService() && MBBMSService.sInstance != null) {
			if(MBBMSService.sInstance.isNeedReset() == 1) {
				showLoadingDialog(getResources().getString(R.string.reset_engine));
				MBBMSService.sInstance.resetEngine();
				return true;
			}
		} else {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "MBBMSService not started",
					LogMask.APP_COMMON);
		}
		return false;
	}
	
	private void showLoadingDialog(String promptMessage) {
		if (mLoadingDialog != null) {
			mLoadingDialog.dismiss();
			mLoadingDialog = null;
		}
		mLoadingDialog = new LoadingDialog(AppBase.this,
				R.drawable.dialog_hint,
				getResources().getString(R.string.hint), promptMessage);
		if (!mLoadingDialog.isShowing()) {
			mLoadingDialog.show();
		}
	}
	
	private void dismissLoadingDialog() {
		if (mLoadingDialog != null && mLoadingDialog.isShowing()) {
			mLoadingDialog.dismiss();
			mLoadingDialog = null;
		}
	}
	
	private void getHeadPhoneState() {
		if(isNeedHeadPhone) {
			AudioManager audioManager = (AudioManager) this.getSystemService(Context.AUDIO_SERVICE);
			Log.d("LoadActivity","headState" + audioManager.isWiredHeadsetOn());
			isHeadPhonePlugged = audioManager.isWiredHeadsetOn();
		}
	}
	
	private void getProperty() {
//		boolean test = true;
		String value = SystemProperties.get("persist.mbbms.no-buildin-antena");
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
				"persist.mbbms.no-buildin-antena:"+value,
				LogMask.APP_MAIN);
		if (value != null && (!value.equals(""))) {
			int flag = Integer.parseInt(value);
			if(flag == 1){
				isNeedHeadPhone = true;
			}else {
				isNeedHeadPhone = false;
			}
		}else {
			isNeedHeadPhone = false;
		}
	}
	private boolean canContinue() {
		return (!AppBase.isNeedHeadPhone||(AppBase.isNeedHeadPhone && AppBase.isHeadPhonePlugged));
	}

}
