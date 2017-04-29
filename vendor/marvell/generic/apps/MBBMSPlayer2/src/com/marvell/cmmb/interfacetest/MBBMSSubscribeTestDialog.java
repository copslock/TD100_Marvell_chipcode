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

import java.util.ArrayList;

import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.AdapterView.OnItemClickListener;

import com.marvell.cmmb.R;
import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.common.Define;
import com.marvell.cmmb.resolver.MBBMSResolver;
import com.marvell.cmmb.resolver.PurchaseInfoItem;
import com.marvell.cmmb.service.MBBMSService;
import com.marvell.cmmb.view.adapter.PurchaseListAdapter;

public class MBBMSSubscribeTestDialog extends AlertDialog implements
		OnItemClickListener {
	private int mStatus = -1;

	private TextView mSubscriptionTextView;

	private ListView mSubscriptionListView;

	private ArrayList<PurchaseInfoItem> mSubscriptionList;

	public static MBBMSSubscribeTestDialog sInstance = null;

	private MBBMSResolver mResolver;

	private Context mContext;

	private MBBMSSubscribeTestAdapter mAdapter;

	private String mPromptMessage;

	private String[] purchaseID;

	private Thread mStartSubscribeThread;

	private Thread mUnsubscribeThread;

	private Toast mShortPromptToast;

	private int mPosition;

	private static final String TAG = "MBBMSSubscribeTestDialog";

	private Handler mHandler = new Handler() {

		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case Define.MESSAGE_FINISH_LOADING:
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
						"subscribe/unscribe finish", LogMask.APP_COMMON);
				MBBMSInterfaceTestActivity.sInstance.dismissDialog();
				if (mStatus == 0) {
					if (msg.arg1 == 1) {
						getShortPromptToast(R.string.subscribetestsuccess).show();
						mResolver.updatePurchaseManage(1, mSubscriptionList
								.get(mPosition).getPurchaseId());
						ArrayList<Long> referenceIdList = mResolver.getReferenceIdList(mSubscriptionList.get(
								mPosition).getPurchaseId());
						LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),"mStatus is " + mStatus
								+ " referenceIdList size is " + referenceIdList.size(), LogMask.APP_COMMON);
						for(int i = 0; i < referenceIdList.size(); i++) {
							mResolver.updateChannelEncryptStatus(referenceIdList.get(i), 1);
						}
					} else {
						getShortPromptToast(R.string.subscribetestfail).show();
					}
				} else if (mStatus == 1) {
					if (msg.arg1 == 1) {
						getShortPromptToast(R.string.unsubscribetestsuccess).show();
						mResolver.updatePurchaseManage(0, mSubscriptionList
								.get(mPosition).getPurchaseId());
						ArrayList<Long> referenceIdList = mResolver.getReferenceIdList(mSubscriptionList.get(
								mPosition).getPurchaseId());
						LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),"mStatus is " + mStatus
								+ " referenceIdList size is " + referenceIdList.size(), LogMask.APP_COMMON);
						for(int i = 0; i < referenceIdList.size(); i++) {
							mResolver.updateChannelEncryptStatus(referenceIdList.get(i), 0);
						}
					} else {
						getShortPromptToast(R.string.unsubscribetestfail).show();
					}
				}
				mSubscriptionList = mResolver
						.getPurchaseItemListByStatus(mStatus);
				mAdapter = new MBBMSSubscribeTestAdapter(mContext,
						mSubscriptionList);
				mSubscriptionListView.setAdapter(mAdapter);
				Intent updateServiceIntent = new Intent();
				updateServiceIntent.setAction("com.marvell.cmmb.UPDATE_SERVICE_ACTION");
				mContext.sendBroadcast(updateServiceIntent);
				break;
			}
			super.handleMessage(msg);
		}

	};

	protected MBBMSSubscribeTestDialog(Context context, int status) {
		super(context);
		mContext = context;
		mStatus = status;
	}

	public Handler getHandler() {
		return mHandler;
	}

	private Toast getShortPromptToast(int resid) {
		if (mShortPromptToast == null) {
			mShortPromptToast = Toast.makeText(mContext, resid,
					Toast.LENGTH_SHORT);
		}
		mShortPromptToast.setText(resid);
		return mShortPromptToast;
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		sInstance = this;
		mResolver = MBBMSResolver.getInstance(getContext());
		setContentView(R.layout.interfacetest_dialog);
		setupView();
	}

	public void onItemClick(AdapterView<?> parent, View view, int position,
			long id) {
		mPosition = position;
		if (mStatus == 1) {
			mPromptMessage = mContext.getResources().getString(
					R.string.unsubscribing);
			MBBMSInterfaceTestActivity.sInstance
			.showLoadingDialog(mPromptMessage);
			if(MBBMSService.isSGUpdateFailed) {
				MBBMSInterfaceTestActivity.sInstance.dismissDialog();
				getShortPromptToast(R.string.unsubscribetestfail).show();
				return;
			}
			unSubscribe(mSubscriptionList);
		} else if (mStatus == 0) {
			mPromptMessage = mContext.getResources().getString(
					R.string.subscribing);
			MBBMSInterfaceTestActivity.sInstance
			.showLoadingDialog(mPromptMessage);
			if(MBBMSService.isSGUpdateFailed) {
				MBBMSInterfaceTestActivity.sInstance.dismissDialog();
				getShortPromptToast(R.string.unsubscribetestfail).show();
				return;
			}
			startSubscribe(mSubscriptionList);
		}
	}

	private void setupView() {
		mSubscriptionTextView = (TextView) findViewById(R.id.tv_title);
		mSubscriptionListView = (ListView) findViewById(R.id.lv_intefacetest);
		if (mStatus == 1) {
			mSubscriptionTextView.setText(mContext.getResources().getString(
					R.string.unsubscribetest));
		} else if (mStatus == 0) {
			mSubscriptionTextView.setText(mContext.getResources().getString(
					R.string.subscribetest));
		}
		mSubscriptionList = mResolver.getPurchaseItemListByStatus(mStatus);
		mAdapter = new MBBMSSubscribeTestAdapter(mContext, mSubscriptionList);
		mSubscriptionListView.setAdapter(mAdapter);
		mSubscriptionListView.setOnItemClickListener(this);
	}

	private void startSubscribe(ArrayList<PurchaseInfoItem> list) {
		purchaseID = new String[1];
		purchaseID[0] = mSubscriptionList.get(mPosition).getPurchaseId();
		MBBMSService.sInstance.setPurchaseIds(purchaseID);
		MBBMSService.sInstance.startUsingNetwork(Define.SUBSCRIBE);
	}

	private void unSubscribe(ArrayList<PurchaseInfoItem> list) {
		purchaseID = new String[1];
		purchaseID[0] = mSubscriptionList.get(mPosition).getPurchaseId();
		MBBMSService.sInstance.setPurchaseIds(purchaseID);
		MBBMSService.sInstance.startUsingNetwork(Define.UNSUBSCRIBE);
	}

}
