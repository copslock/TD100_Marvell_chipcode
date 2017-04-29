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

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.marvell.cmmb.R;
import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.common.AppBase;
import com.marvell.cmmb.common.Define;
import com.marvell.cmmb.common.MsgEvent;
import com.marvell.cmmb.common.Util;
import com.marvell.cmmb.resolver.PurchaseInfoItem;
import com.marvell.cmmb.service.MBBMSService;
import com.marvell.cmmb.view.adapter.PurchaseListAdapter;
import com.marvell.cmmb.view.dialog.ConfirmDialogCallBack;
import com.marvell.cmmb.view.dialog.LoadingDialog;
import com.marvell.cmmb.view.dialog.LoadingDialog.LoadBack;

public class PurchaseManageActivity extends AppBase implements OnClickListener,
		ConfirmDialogCallBack, LoadBack {
	private static final String TAG = "Subscription";

	private LayoutInflater mSubManageInflater;

	private TextView mSubscribeTextView;

	private TextView mUnsubscribeTextView;

	private ListView mSubscribeListView;

	private TextView mPromptToastTextView;

	private PurchaseListAdapter mSubscribeAdapter;

	private int mSubscriptionStatus = -1;// check whether the channel set has been subscribed

	private ArrayList<PurchaseInfoItem> mPurchaseList = new ArrayList<PurchaseInfoItem>();

	private ArrayList<PurchaseInfoItem> mInquiryList = new ArrayList<PurchaseInfoItem>();

	private LoadingDialog mLoadingDialog;

	private Toast mPromptToast;

	public static PurchaseManageActivity sInstance = null;

	private Toast mUpdateToast;

	private Thread mStartSubscribeThread;

	private Thread mUnsubscribeThread;

	private PurchaseReceiver mPurchaseReceiver;
	
	private boolean isInquirying;
	
	private boolean mInquiryResult;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.subscription_manage);
		init();
		initViews();
		registerPurchaseListener();
		setupViews();
		inquiry();
	}

	@Override
	protected void onResume() {
		super.onResume();
		refreshTabViewBySatus();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		unRegisterPurchaseListener();
		sInstance = null;
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_BACK:
			Intent updateServiceIntent = new Intent();
			updateServiceIntent.setAction("com.marvell.cmmb.UPDATE_SERVICE_ACTION");
			this.sendBroadcast(updateServiceIntent);
			this.finish();
			break;
		}
		return super.onKeyDown(keyCode, event);
	}

	private void init() {
		sInstance = this;
		isInquirying = false;
	}

	private void registerPurchaseListener() {
		mPurchaseReceiver = new PurchaseReceiver();
		IntentFilter filter = new IntentFilter();
		filter.addAction(ACTION_NETWORKCONNECT_FAILED);
		filter.addAction(ACTION_PARSEINQUIRY_FINISHED);
		filter.addAction(ACTION_INQUIRIED);
		filter.addAction(ACTION_SUBSCRIBE_FINISH);
		filter.addAction(ACTION_UNSUBSCRIBE_FINISH);
		filter.addAction(ACTION_PARSEST_FINISHED);
		filter.addAction(ACTION_AUTHORIZED);
		this.registerReceiver(mPurchaseReceiver, filter);
	}

	private void unRegisterPurchaseListener() {
		if (mPurchaseReceiver != null) {
			this.unregisterReceiver(mPurchaseReceiver);
			mPurchaseReceiver = null;
		}
	}

	public void confirmDialogCancel(int type) {

	}

	/**
	 * show confirm dialog to user when user try to subscribe/unsubscribe a
	 * channel set
	 * 
	 */
	public void confirmDialogOk(int type) {
		mSubscriptionStatus = type;
		String promptMessage = getPromptMessage();
		if (!mPurchaseList.isEmpty()) {
			showLoadingDialog(promptMessage);
		}
		if(MBBMSService.isSGUpdateFailed) {
			dismissLoadingDialog();
			showUpdateToast(0, 0);
			return;
		}
		MBBMSService.sInstance.setPurchaseIds(getPurchaseIds());
		if (mSubscriptionStatus == 1) {
			MBBMSService.sInstance.startUsingNetwork(Define.UNSUBSCRIBE);
		} else {
			MBBMSService.sInstance.startUsingNetwork(Define.SUBSCRIBE);
		}
	}

	private String[] getPurchaseIds() {
		String[] purchaseID = new String[1];
		if (mPurchaseList.size() != 0) {
			purchaseID[0] = mPurchaseList.get(
					PurchaseListAdapter.sSelectedPosition).getPurchaseId();
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "purchaseID[0] is "
					+ purchaseID[0], LogMask.APP_COMMON);
		}
		return purchaseID;
	}

	private String getPromptMessage() {
		String promptMessage = null;
		if (mSubscriptionStatus == 0) {
			promptMessage = getResources().getString(R.string.subscribing);
		} else {
			promptMessage = getResources().getString(R.string.unsubscribing);
		}
		return promptMessage;
	}

	private void inquiry() {
		String promptMessage = this.getResources().getString(R.string.getinquiryinfo);
		showLoadingDialog(promptMessage);
		isInquirying = true;
		if(!MBBMSService.isAuthorizing) {
			MBBMSService.sInstance.startUsingNetwork(INQUIRE);
		} else {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "start request inquiry but now is authorizing, wait...", LogMask.APP_COMMON);
		}
	}

	public void onBack() {
		if (MBBMSService.isSubscribing) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"cancel MBBMS_REQUEST_SUBSCRIBE", LogMask.APP_COMMON);
			MBBMSService.sInstance.cancelEvent(MsgEvent.EVENT_STARTSUBSCRIBE);
		} else if (MBBMSService.isUnsubscribing) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"cancel MBBMS_REQUEST_UNSUBSCRIBE", LogMask.APP_COMMON);
			MBBMSService.sInstance.cancelEvent(MsgEvent.EVENT_UNSUBSCRIBE);
		} else {
			dismissLoadingDialog();
		}
		MBBMSService.sInstance.stopWapConnectivity();
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.tv_issubscribed:
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "subscribe", LogMask.APP_COMMON);
			mSubscriptionStatus = 1;
			updatePurchaseList();
			updateViews();
			break;
		case R.id.tv_isunsubscribed:
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "unsubscribe", LogMask.APP_COMMON);
			mSubscriptionStatus = 0;
			updatePurchaseList();
			updateViews();
			break;
		}
	}

	private void refreshListView() {
		mSubscribeAdapter = new PurchaseListAdapter(this, mPurchaseList);
		mSubscribeListView.setAdapter(mSubscribeAdapter);
	}

	/**
	 * refresh the Tab View
	 * 
	 */
	private void refreshTabViewBySatus() {
		if (mSubscriptionStatus == 1) {// subscribed
			mSubscribeTextView.setTextColor(this.getResources().getColor(R.color.solid_white));
			mUnsubscribeTextView.setTextColor(this.getResources().getColor(R.color.solid_gray));
			mSubscribeTextView.setBackgroundResource(R.drawable.subscription_on);
			mUnsubscribeTextView.setBackgroundResource(R.drawable.subscription_off);
		} else { // unsubscribed

			mSubscribeTextView.setTextColor(this.getResources().getColor(R.color.solid_gray));
			mUnsubscribeTextView.setTextColor(this.getResources().getColor(R.color.solid_white));
			mSubscribeTextView.setBackgroundResource(R.drawable.subscription_off);
			mUnsubscribeTextView.setBackgroundResource(R.drawable.subscription_on);
		}

	}

	private void showLoadingDialog(String promptMessage) {
		if (mLoadingDialog != null) {
			mLoadingDialog.dismiss();
			mLoadingDialog = null;
		}
		mLoadingDialog = new LoadingDialog(PurchaseManageActivity.this,
				R.drawable.dialog_hint,
				getResources().getString(R.string.hint), promptMessage);
		mLoadingDialog.setCallBack(PurchaseManageActivity.this);
		if (!mLoadingDialog.isShowing()) {
			mLoadingDialog.show();
		}
	}

	private void dismissLoadingDialog() {
		if (mLoadingDialog != null && mLoadingDialog.isShowing()) {
			mLoadingDialog.dismiss();
			mLoadingDialog = null;
		}
		isInquirying = false;
	}

	/**
	 * show a toast when subscribe/unsubscribe successfully.
	 * 
	 */
	private void showUpdateToast(int status, int resCode) {
		if (mUpdateToast == null) {
			mUpdateToast = new Toast(PurchaseManageActivity.this);
			View v = mSubManageInflater.inflate(R.layout.tvplayer_toast, null);
			mUpdateToast.setView(v);
			mPromptToastTextView = (TextView) v.findViewById(R.id.tv_promptmessage);
			mUpdateToast.setDuration(Toast.LENGTH_LONG);
			mUpdateToast.setGravity(Gravity.CENTER, 0, 0);
		}
		String toastMsg = "";
		String errorInfo = Util.getErrorCodeInfo(this, resCode);
		if (status == 1) {
			if (mSubscriptionStatus == 0) {
				toastMsg = getResources().getString(R.string.hassubscribe)
						+ mPurchaseList.get(PurchaseListAdapter.sSelectedPosition)
								.getPurchaseName() + ".";

			} else {
				toastMsg = getResources().getString(R.string.hasunsubscribe)
						+ mPurchaseList.get(PurchaseListAdapter.sSelectedPosition)
								.getPurchaseName() + ", "
						+ getResources().getString(R.string.welcometosubscribe);
			}
		} else {
			if(errorInfo.equals("")) {
				if (mSubscriptionStatus == 0) {
					toastMsg = getResources().getString(R.string.subscribefailed);

				} else {
					toastMsg = getResources().getString(R.string.unsubscribefailed);
				}
			} else {
				toastMsg = errorInfo;
			}
			
		}
		mPromptToastTextView.setText(toastMsg);
		mUpdateToast.show();

	}

	/**
	 * update the the status when user subscribe a channelset/unsubscribe a
	 * channelset
	 * 
	 */
	private void updatePurchaseManage() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "mSubscriptionStatus is " + mSubscriptionStatus, LogMask.APP_COMMON);
		if (mSubscriptionStatus == 0) {
			mResolver.updatePurchaseManage(1, mPurchaseList.get(
					PurchaseListAdapter.sSelectedPosition).getPurchaseId());
		} else {
			mResolver.updatePurchaseManage(0, mPurchaseList.get(
					PurchaseListAdapter.sSelectedPosition).getPurchaseId());
		}
	}
	
	private void updateChannelEncryptStatus() {
		ArrayList<Long> referenceIdList = mResolver.getReferenceIdList(mPurchaseList.get(
				PurchaseListAdapter.sSelectedPosition).getPurchaseId());
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "referenceIdList size is " + referenceIdList.size(), LogMask.APP_COMMON);
		if(mSubscriptionStatus == 0) {
			for(int i = 0; i < referenceIdList.size(); i++) {
				mResolver.updateChannelEncryptStatus(referenceIdList.get(i), 1);
			}
		} else {
			for(int i = 0; i < referenceIdList.size(); i++) {
				mResolver.updateChannelEncryptStatus(referenceIdList.get(i), 0);
			}
		}
	}

	private void updatePurchaseDatabase() {
//		ArrayList<PurchaseInfoItem> purchaseList = mResolver.getPurchaseItemListByStatus(1);
		ArrayList<PurchaseInfoItem> purchaseList = mResolver.getPurchaseItemList();
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), 
				"mInquiryList size is " + mInquiryList.size() + " purchaseList size is " + purchaseList.size(),
				LogMask.APP_COMMON);
		if(!mInquiryResult){
			return;
		}
		if (!mInquiryList.isEmpty()) {
			for (PurchaseInfoItem item : purchaseList) {
				for (PurchaseInfoItem inquiryItem : mInquiryList) {
					if ((item.getPurchaseId().equals(inquiryItem.getPurchaseId()) && (item.getPlaceStatus() == 1))) {
						inquiryItem.setPlaceStatus(1);
					}
				}
			}
			mResolver.deletePurchaseInfo(0);
			for (PurchaseInfoItem item : mInquiryList) {
				if (item.getPlaceStatus() == 0) {
					mResolver.insertPurchaseInfo(item.getPurchaseId(), item
							.getPurchaseName(), item.getPlaceStatus(), item
							.getPurchasePeriod(), item.getPurchaseType(), item
							.getPurchasePrice(), item.getPurchaseDescription());
				}
				mResolver.deletePurchaseManage(item.getPurchaseId());
				mResolver.insertPurchaseManage(item.getPurchaseId(), 1);
			}
		} else {
			for(PurchaseInfoItem item : purchaseList) {
				mResolver.updatePurchaseManage(0, item.getPurchaseId());
			}
		}
	}

	private void initPurchaseList() {
		if (mSubscriptionStatus == 1) {
			if (!mInquiryList.isEmpty()) {
				mPurchaseList = mInquiryList;
			}
		} else {
			mPurchaseList = mResolver.getPurchaseItemListByStatus(1, mSubscriptionStatus);
		}
	}

	private void updatePurchaseList() {
		if (mSubscriptionStatus == 1) {
			mPurchaseList = mResolver .getPurchaseItemListByStatus(mSubscriptionStatus);
		} else {
			mPurchaseList = mResolver.getPurchaseItemListByStatus(1, mSubscriptionStatus);
		}
	}

	@Override
	public void initViews() {
		mSubscribeTextView = (TextView) findViewById(R.id.tv_issubscribed);
		mUnsubscribeTextView = (TextView) findViewById(R.id.tv_isunsubscribed);
		mSubscribeListView = (ListView) findViewById(R.id.lv_subscription);
	}

	@Override
	public void setupViews() {
		mUnsubscribeTextView.setOnClickListener(this);
		mSubscribeTextView.setOnClickListener(this);
		Intent intent = getIntent();
		mSubscriptionStatus = intent.getExtras().getInt("SubscribeStatus");
		mSubManageInflater = LayoutInflater.from(this);
	}

	@Override
	public void updateDate() {
		mInquiryList = MBBMSService.sInstance.getInquiryList();
		mPurchaseList = mResolver.getPurchaseItemListByStatus(mSubscriptionStatus);
		updatePurchaseDatabase();
//		initPurchaseList();
		updatePurchaseList();
	}

	@Override
	public void updateViews() {
		refreshTabViewBySatus();
		refreshListView();
	}

	class PurchaseReceiver extends BroadcastReceiver {

		@Override
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "action is " + action, LogMask.APP_COMMON);
			if (action.equals(ACTION_PARSEINQUIRY_FINISHED)) {
				mInquiryResult = true;
				dismissLoadingDialog();
				updateDate();
				updateViews();
				MBBMSService.sInstance.stopWapConnectivity();
			} else if (action.equals(ACTION_NETWORKCONNECT_FAILED)
					|| action.equals(ACTION_INQUIRIED) && (!intent
					.getBooleanExtra(ACTION_INQUIRIED, false))) {
				mInquiryResult = false;
				dismissLoadingDialog();
				updateDate();
				updateViews();
				MBBMSService.sInstance.stopWapConnectivity();				
			} else if (action.equals(ACTION_SUBSCRIBE_FINISH)
					|| action.equals(ACTION_UNSUBSCRIBE_FINISH)) {
				dismissLoadingDialog();
				boolean result = intent.getBooleanExtra("SubscribeResult", false);
				int resCode = intent.getIntExtra("resCode", 0);
				if (result) {	
					MBBMSService.sInstance.setUpdateSTStatus(true);
					MBBMSService.sInstance.startUsingNetwork(Define.UPDATEST);
					showUpdateToast(1, resCode);
					updatePurchaseManage();
					updateChannelEncryptStatus();
					updatePurchaseList();
					updateViews();
				} else {
					showUpdateToast(0, resCode);
				}
//				MBBMSService.sInstance.stopWapConnectivity();
			} else if (action.equals(ACTION_PARSEST_FINISHED)) {//modify to avoid two threads querying tables concurrently.
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "update st finished after subscribe/unsubscribe", LogMask.APP_COMMON);
				Intent finishIntent = new Intent();
				finishIntent.setClass(PurchaseManageActivity.this, MainActivity.class);
				finishIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
				startActivity(finishIntent);
				PurchaseManageActivity.this.finish();
			} else if (action.equals(ACTION_AUTHORIZED)) {
				if(isInquirying) {
					LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "start request inquiry after authorize", LogMask.APP_COMMON);
					MBBMSService.sInstance.startUsingNetwork(INQUIRE);
				}
			}
		}

	}

}
