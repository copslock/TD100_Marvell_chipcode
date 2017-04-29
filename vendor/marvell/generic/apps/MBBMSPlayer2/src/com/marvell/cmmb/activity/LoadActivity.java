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

package com.marvell.cmmb.activity;

import java.util.ArrayList;
import java.util.Calendar;


import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.media.AudioManager;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.widget.TextView;

import com.marvell.cmmb.R;
import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.common.Action;
import com.marvell.cmmb.common.AppBase;
import com.marvell.cmmb.common.Util;
import com.marvell.cmmb.resolver.EmergencyItem;
import com.marvell.cmmb.service.MBBMSService;
import com.marvell.cmmb.view.customview.LoadingView;
import com.marvell.cmmb.view.dialog.ConfirmDialog;
import com.marvell.cmmb.view.dialog.ConfirmDialogCallBack;
import com.marvell.cmmb.view.dialog.PromptDialog;
import com.marvell.cmmb.view.dialog.PromptDialogCallBack;
import com.marvell.cmmb.view.dialog.PromptDialog.PromptDialogKeycallback;

public class LoadActivity extends AppBase implements ConfirmDialogCallBack,PromptDialogCallBack, PromptDialogKeycallback{
	private static final String TAG = "LoadActivity";
	
	public static LoadActivity sInstance = null;

	private ConfirmDialog mConfirmDialog = null;

	private LoadReceiver mLoadReceiver;

	private LoadingView mLoadingView;

	private String mWelcomInfo;
	
	private TextView mNoteTextView;
	

	/** Called when the activity is first created. */
	public void onCreate(Bundle savedInstanceState) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onCreate", LogMask.APP_COMMON);
		
		super.onCreate(savedInstanceState);
		sInstance = this;		

		if(canContinue()){
			setContentView(R.layout.load);
			initViews();
			setupViews();
			registerLoadReceiver();					
		}else if(AppBase.isNeedHeadPhone &&(!AppBase.isHeadPhonePlugged)) {
			showAlertDialog();			
		}

	}

	protected void onResume() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onResume", LogMask.APP_COMMON);
		if(canContinue()) {
			updateDate();
			updateViews();
		}		
		super.onResume();
	}

	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
		if (requestCode == 1) {
			MBBMSService.sInstance.checkMode();
		}
	}

	protected void onDestroy() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onDestroy", LogMask.APP_COMMON);
		if(canContinue()){
			mLoadingView.stopLoading();
			unRegisterLoadReceiver();
			sInstance = null;
		}		
		super.onDestroy();
	}

	public boolean onKeyDown(int keyCode, KeyEvent event) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_BACK:
			MBBMSService.sInstance.cancelSetMode();
			break;
		}
		return super.onKeyDown(keyCode, event);
	}

	public void initViews() {
		mLoadingView = (LoadingView) findViewById(R.id.loading);
		mNoteTextView = (TextView)findViewById(R.id.tv_note);
	}

	public void setupViews() {

	}

	public void updateDate() {
		deleteExpiredEmergency();
		SharedPreferences reference = getSharedPreferences("WELCOMINFO", MODE_PRIVATE);
		mWelcomInfo = reference.getString("WelcomInfo", "");
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "welcom info is "
				+ mWelcomInfo, LogMask.APP_COMMON);
	}

	public void updateViews() {
		mLoadingView.setInfo(mWelcomInfo);		
		mNoteTextView.setText(R.string.loading_note_init);
	}

	private void deleteExpiredEmergency() {
		Calendar calendar = Calendar.getInstance();
		String date = "";
		String time = "";
		long expireTime = 0;
		ArrayList<EmergencyItem> deleteEmergencyList = mResolver
				.emergencyQuery(DELETE_FLAG_TRUE);
		for (int i = 0; i < deleteEmergencyList.size(); i++) {
			date = deleteEmergencyList.get(i).getEmergencyDate();
			time = deleteEmergencyList.get(i).getEmergencyTime();
			try {
				expireTime = Util.sDateTimeFormater.parse(date + " " + time)
						.getTime() + deleteEmergencyList.get(i).getDuration() * 1000;
			} catch (Exception e) {
				LogMask.LOGM(TAG, LogMask.LOG_APP_ERROR, LogMask.getLineInfo(),
						"parse got Exception", LogMask.APP_ERROR);
			}
			if (calendar.getTime().getTime() >= expireTime) {
				mResolver.deleteEmergency(deleteEmergencyList.get(i).getEmergencyId());
			}
		}
	}

	private void startAPNSetting() {
		if (mConfirmDialog.isShowing()) {
			mConfirmDialog.dismiss();
			mConfirmDialog = null;
		}
		Intent apnSettingIntent = new Intent(Intent.ACTION_MAIN);
		ComponentName componentName = new ComponentName("com.android.phone",
				"com.android.phone.Settings");
		apnSettingIntent.setComponent(componentName);
		this.startActivityForResult(apnSettingIntent, 1);
	}

	private void showConfirmDialog(int type) {
		String promptMsg = "";
		if (type == DIALOG_CONFIRM_SETTING_APN) {
			promptMsg = this.getResources().getString(R.string.settingapn);
		}
		if (mConfirmDialog != null) {
			mConfirmDialog.dismiss();
			mConfirmDialog = null;
		}
		mConfirmDialog = new ConfirmDialog(this, R.drawable.dialog_hint,
				getResources().getString(R.string.hint), promptMsg);
		mConfirmDialog.setCallBack(this, type);
		if (!mConfirmDialog.isShowing()) {
			mConfirmDialog.show();
		}
	}

	class LoadReceiver extends BroadcastReceiver {

		@Override
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();
			if (action.equals(ACTION_SIMCARD_OUT)) {
				getShortPromptToast(R.string.nocard).show();
			} else if (action.equals(ACTION_SET_APN)) {
				showConfirmDialog(DIALOG_CONFIRM_SETTING_APN);
			} else if (action.equals(ACTION_MODE_CMMB)) {
//				getShortPromptToast(R.string.cmmbmode).show();
			} else if (action.equals(ACTION_LOADING_FINISH)) {
//				LoadActivity.this.finish();				
					startMainActivity();				
			} else if (action.equals(ACTION_LOADING_ERROR_QUIT)) {
				LoadActivity.this.finish();
				getShortPromptToast(R.string.starterror).show();
			}else if(action.equals(ACTION_START_CONNECT_NET)) {
				refreshNote();
			}
		}

	}

	private void startMainActivity() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "start MainActvity",
				LogMask.APP_MAIN);
		Intent intent = new Intent();
		intent.setClass(LoadActivity.this, MainActivity.class);
		startActivity(intent);
	}

	private void registerLoadReceiver() {
		IntentFilter filter = new IntentFilter();
		filter.addAction(ACTION_LOADING_FINISH);
		filter.addAction(ACTION_SIMCARD_OUT);
		filter.addAction(ACTION_SET_APN);
		filter.addAction(ACTION_MODE_CMMB);
		filter.addAction(ACTION_LOADING_ERROR_QUIT);
		filter.addAction(ACTION_START_CONNECT_NET);
		
		mLoadReceiver = new LoadReceiver();
		this.registerReceiver(mLoadReceiver, filter);
	}

	private void unRegisterLoadReceiver() {
		if (mLoadReceiver != null) {
			this.unregisterReceiver(mLoadReceiver);
			mLoadReceiver = null;
		}
	}

	public void confirmDialogCancel(int type) {
		if (type == DIALOG_CONFIRM_SETTING_APN) {
			MBBMSService.sInstance.cancelCheckMode();
		}
	}

	public void confirmDialogOk(int type) {
		if (type == DIALOG_CONFIRM_SETTING_APN) {
			startAPNSetting();
		}
	}

	private void refreshNote(){
		 mNoteTextView.setText(R.string.loading_note_connectnet);
	}
		

	private void showAlertDialog() {
		PromptDialog dialog = new PromptDialog(this,R.drawable.dialog_hint, this.getResources().getString(R.string.hint),
				this.getResources().getString(R.string.check_headphone));
		dialog.setCallBack(this);
		dialog.setKeyCallback(this);
		dialog.show();
	}

	public void promptDialogOk(int type) {
		// TODO Auto-generated method stub
		this.finish();
	}

	public void promptDialogOnKeyBack(int type) {
		// TODO Auto-generated method stub
		this.finish();
	}
	
	private boolean canContinue() {		
		return (!AppBase.isNeedHeadPhone||(AppBase.isNeedHeadPhone && AppBase.isHeadPhonePlugged));
	}

}