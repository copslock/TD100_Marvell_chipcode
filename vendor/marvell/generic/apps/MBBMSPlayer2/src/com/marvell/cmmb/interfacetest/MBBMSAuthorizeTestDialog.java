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
import com.marvell.cmmb.resolver.ChannelItem;
import com.marvell.cmmb.resolver.MBBMSResolver;
import com.marvell.cmmb.service.MBBMSService;

public class MBBMSAuthorizeTestDialog extends AlertDialog implements
		OnItemClickListener {
	private TextView mAuthorizeTextView;

	private ListView mAuthorizeListView;

	private Context mContext;

	private MBBMSResolver mResolver;

	private ArrayList<ChannelItem> mChannelList;

	private MBBMSAuthorizeTestAdapter mAdapter;

	private String mPromptMessage;

	private String mServiceId;
	
	private String mCurrentChannel;

	private static final int NOT_FREE = 0;

	private Thread mAuthorizeThread;

	private Toast mShortPromptToast;

	public static MBBMSAuthorizeTestDialog sInstance = null;

	private static final String TAG = "MBBMSAuthorizeTestDialog";

	private Handler mHandler = new Handler() {

		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case Define.MESSAGE_FINISH_LOADING:
				MBBMSInterfaceTestActivity.sInstance.dismissDialog();
				if (msg.arg1 == 1) {
					getShortPromptToast(R.string.getmsksuccess).show();
				} else {
					getShortPromptToast(R.string.getmskfail).show();
				}
				playAfterAuthorize();
				break;
			}
			super.handleMessage(msg);
		}

	};

	protected MBBMSAuthorizeTestDialog(Context context) {
		super(context);
		mContext = context;
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
	
	private void playAfterAuthorize() {
		Intent authorizeIntent = new Intent();
		authorizeIntent.setAction("com.marvell.cmmb.VIEW_DETAIL_ACTION");
		authorizeIntent.putExtra("ChooseChannel", mCurrentChannel);
		mContext.startActivity(authorizeIntent);
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
		mPromptMessage = mContext.getResources().getString(R.string.getmsk);
		mCurrentChannel = mChannelList.get(position).getChannelName();
		startAuthorize(mChannelList.get(position).getChannelId());
		MBBMSInterfaceTestActivity.sInstance.showLoadingDialog(mPromptMessage);
	}

	private void setupView() {
		mAuthorizeTextView = (TextView) findViewById(R.id.tv_title);
		mAuthorizeListView = (ListView) findViewById(R.id.lv_intefacetest);
		mAuthorizeTextView.setText(R.string.authorizetest);
		mChannelList = mResolver.getChannelList(Define.MBBMS_MODE, NOT_FREE);
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "mChannelList.size() is "
				+ mChannelList.size(), LogMask.APP_COMMON);
		mAdapter = new MBBMSAuthorizeTestAdapter(mContext, mChannelList);
		mAuthorizeListView.setAdapter(mAdapter);
		mAuthorizeListView.setOnItemClickListener(this);
	}

	private void startAuthorize(long channelId) {
		MBBMSService.sInstance.setAuthorizeId(String.valueOf(channelId));
		MBBMSService.sInstance.startUsingNetwork(Define.AUTHORIZE);
	}
}
