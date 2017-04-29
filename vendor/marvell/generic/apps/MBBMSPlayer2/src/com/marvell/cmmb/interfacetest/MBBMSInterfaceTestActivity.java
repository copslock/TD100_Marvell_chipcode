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

package com.marvell.cmmb.interfacetest;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.AdapterView.OnItemClickListener;

import com.marvell.cmmb.R;
import com.marvell.cmmb.activity.PurchaseManageActivity;
import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.aidl.MBBMSEngine;
import com.marvell.cmmb.common.AppBase;
import com.marvell.cmmb.common.Define;
import com.marvell.cmmb.service.MBBMSService;
import com.marvell.cmmb.view.dialog.LoadingDialog;
import com.marvell.cmmb.view.dialog.LoadingDialog.LoadBack;

public class MBBMSInterfaceTestActivity extends AppBase implements
		OnItemClickListener, LoadBack {
	private ListView mInterfaceListView;

	private MBBMSInterfaceTestAdapter mAdapter;

	public static MBBMSInterfaceTestActivity sInstance = null;

	private LoadingDialog mLoadingDialog;

	private MBBMSSubscribeTestDialog mSubscribeDialog;

	private MBBMSSubscribeTestDialog mUnsubscribeDialog;

	private MBBMSAuthorizeTestDialog mAuthorizeDialog;
	
	private MBBMSUpdateSGAreaDialog mUpdateExternalSGDialog;

	private String mPromptMessage;

	private static final int UPDATE_SG = 0;

	private static final int UPDATE_EXTERNAL_SG = 1;

	private static final int UPDATE_ST = 2;

	private static final int INIT_GBA = 3;

	private static final int SUBSCRIBE = 4;

	private static final int UNSUBSCRIBE = 5;

	private static final int INQUIRY = 6;

	private static final int AUTHORIZE = 7;

	public MBBMSTestReceiver mTestReceiver;

	private static final String TAG = "MBBMSInterfaceTestActivity";

	private Handler mHandler = new Handler() {

		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case Define.MESSAGE_FINISH_LOADING:
				dismissDialog();
				break;
			}
			super.handleMessage(msg);
		}
	};

	public void dismissDialog() {
		if (mLoadingDialog != null && mLoadingDialog.isShowing()) {
			mLoadingDialog.dismiss();
			mLoadingDialog = null;
		}
	}

	public Handler getHandler() {
		return mHandler;
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		registerTestReceiver();
		setContentView(R.layout.interfacetest_mbbms);
		initViews();
		setupViews();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		unRegisterTestReceiver();
		MBBMSService.isTestMode = false;
		sInstance = null;
	}

	public void onItemClick(AdapterView<?> parent, View view, int position,
			long id) {
		switch (position) {
		case UPDATE_SG:
			mPromptMessage = view.getContext().getString(R.string.updatesg);
			MBBMSService.sInstance.setCityName("local");
			MBBMSService.sInstance.startUsingNetwork(Define.UPDATESG);
			showLoadingDialog(mPromptMessage);
			break;
		case UPDATE_EXTERNAL_SG:
			if(mUpdateExternalSGDialog != null) {
				mUpdateExternalSGDialog = null;
			}
			mUpdateExternalSGDialog = new MBBMSUpdateSGAreaDialog(this);
			mUpdateExternalSGDialog.show();
//			mPromptMessage = view.getContext().getString(R.string.updatesg);
//			MBBMSService.sInstance.setCityName("shanghai");
//			MBBMSService.sInstance.startUsingNetwork(Define.UPDATESG);
//			showLoadingDialog(mPromptMessage);
			break;
		case UPDATE_ST:
			mPromptMessage = view.getContext().getString(R.string.updatest);
			MBBMSService.sInstance.startUsingNetwork(Define.UPDATEST);
			showLoadingDialog(mPromptMessage);
			break;
		case INIT_GBA:
			mPromptMessage = view.getContext().getString(R.string.initgba);
			MBBMSService.sInstance.startUsingNetwork(Define.INITGBA);
			showLoadingDialog(mPromptMessage);
			break;
		case SUBSCRIBE:
			if (mSubscribeDialog != null) {
				mSubscribeDialog = null;
			}
			mSubscribeDialog = new MBBMSSubscribeTestDialog(this, 0);
			mSubscribeDialog.show();
			break;
		case UNSUBSCRIBE:
			if (mUnsubscribeDialog != null) {
				mUnsubscribeDialog = null;
			}
			mUnsubscribeDialog = new MBBMSSubscribeTestDialog(this, 1);
			mUnsubscribeDialog.show();
			break;
		case INQUIRY:
			Intent inquiryIntent = new Intent();
			inquiryIntent.setClass(MBBMSInterfaceTestActivity.this, PurchaseManageActivity.class);
			inquiryIntent.putExtra("SubscribeStatus", 1);
			startActivity(inquiryIntent);
//			mPromptMessage = view.getContext().getString(R.string.inquiry);
//			MBBMSService.sInstance.startUsingNetwork(Define.INQUIRE);
//			showLoadingDialog(mPromptMessage);
			break;
		case AUTHORIZE:
			if (mAuthorizeDialog != null) {
				mAuthorizeDialog = null;
			}
			mAuthorizeDialog = new MBBMSAuthorizeTestDialog(this);
			mAuthorizeDialog.show();
			break;
		}
	}

	public void showLoadingDialog(String promptMessage) {
		if (mLoadingDialog != null) {
			mLoadingDialog.dismiss();
			mLoadingDialog = null;
		}
		mLoadingDialog = new LoadingDialog(MBBMSInterfaceTestActivity.this,
				R.drawable.dialog_hint,
				getResources().getString(R.string.hint), promptMessage);
		mLoadingDialog.setCallBack(this);
		if (!mLoadingDialog.isShowing()) {
			mLoadingDialog.show();
		}
	}

	public void onBack() {
		MBBMSEngine.getInstance().cancelMBBMSRequest();
		dismissDialog();
	}

	@Override
	public void initViews() {
		MBBMSService.isTestMode = true;
		sInstance = this;
		mInterfaceListView = (ListView) findViewById(R.id.lv_intefacetest);
		mAdapter = new MBBMSInterfaceTestAdapter(this);
	}

	@Override
	public void setupViews() {
		mInterfaceListView.setAdapter(mAdapter);
		mInterfaceListView.setOnItemClickListener(this);
	}

	@Override
	public void updateDate() {

	}

	@Override
	public void updateViews() {

	}

	class MBBMSTestReceiver extends BroadcastReceiver {

		@Override
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();
			if (action.equals(ACTION_AUTHORIZED)) {
				boolean result = intent.getBooleanExtra("AuthorizeResult", false);
				if(result) {
					mAuthorizeDialog.getHandler().sendMessage(mAuthorizeDialog.getHandler().obtainMessage(MESSAGE_FINISH_LOADING, 1, 0));
				} else {
					mAuthorizeDialog.getHandler().sendMessage(mAuthorizeDialog.getHandler().obtainMessage(MESSAGE_FINISH_LOADING, 0, 0));
				}
			} else if (action.equals(ACTION_SUBSCRIBE_FINISH)) {
				boolean result = intent.getBooleanExtra(
						"SubscribeResult", false);
				LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "action is " + action
						+ " subscribe result is " + result, LogMask.APP_MAIN);
				if (result) {
					mSubscribeDialog.getHandler().sendMessage(
							mSubscribeDialog.getHandler().obtainMessage(
									MESSAGE_FINISH_LOADING, 1, 0));
				} else {
					mSubscribeDialog.getHandler().sendMessage(
							mSubscribeDialog.getHandler().obtainMessage(
									MESSAGE_FINISH_LOADING, 0, 0));
				}
			} else if (action.equals(ACTION_UNSUBSCRIBE_FINISH)) {
				boolean result = intent.getBooleanExtra(
						"SubscribeResult", false);
				LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "action is " + action
						+ " unsubscribe result is " + result, LogMask.APP_MAIN);
				if (result) {
					mUnsubscribeDialog.getHandler().sendMessage(
							mUnsubscribeDialog.getHandler().obtainMessage(
									MESSAGE_FINISH_LOADING, 1, 0));
				} else {
					mUnsubscribeDialog.getHandler().sendMessage(
							mUnsubscribeDialog.getHandler().obtainMessage(
									MESSAGE_FINISH_LOADING, 0, 0));
				}
			} else if(action.equals(ACTION_UPDATE_EXTERNAL_SG)) {
				mPromptMessage = context.getString(R.string.updatesg);
				String areaName = intent.getStringExtra("AREA");
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), " areaName is " + areaName, LogMask.APP_COMMON);
				MBBMSService.sInstance.setCityName(areaName);
				MBBMSService.sInstance.startUsingNetwork(Define.UPDATESG);
				showLoadingDialog(mPromptMessage);
			} else {
				mHandler.sendEmptyMessage(MESSAGE_FINISH_LOADING);
			}
		}

	}

	private void registerTestReceiver() {
		mTestReceiver = new MBBMSTestReceiver();
		IntentFilter filter = new IntentFilter();
		filter.addAction(ACTION_SG_UPDATED);
		filter.addAction(ACTION_ST_UPDATED);
		filter.addAction(ACTION_GBA_INIT);
//		filter.addAction(ACTION_INQUIRIED);
		filter.addAction(ACTION_AUTHORIZED);
		filter.addAction(ACTION_UNSUBSCRIBE_FINISH);
		filter.addAction(ACTION_SUBSCRIBE_FINISH);
		filter.addAction(ACTION_UPDATE_EXTERNAL_SG);
		filter.addAction(ACTION_NETWORKCONNECT_FAILED);
		this.registerReceiver(mTestReceiver, filter);
	}

	private void unRegisterTestReceiver() {
		if (mTestReceiver != null) {
			unregisterReceiver(mTestReceiver);
			mTestReceiver = null;
		}
	}

}
