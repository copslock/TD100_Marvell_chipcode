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

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.TextView;

import com.marvell.cmmb.R;
import com.marvell.cmmb.aidl.MBBMSEngine;
import com.marvell.cmmb.common.AppBase;
import com.marvell.cmmb.service.MBBMSService;
import com.marvell.cmmb.view.dialog.LoadingDialog;

/**
 * @description : provide two mode to refresh *
 */

public class RefreshManageActivity extends AppBase implements OnClickListener {
	private int mUpdateFlag = 0;// indicate whether update successfully

	private LoadingDialog mLoadDialog;

	public static RefreshManageActivity sInstance = null;

	private TextView mManulTV;

	private TextView mAutoTV;

	/**
	 * @description: handle the refresh result,give the proper prompt
	 * 
	 */

	private Handler mHandler = new Handler() {

		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case MESSAGE_UPDATED_SG_ESG:
				dismissDialog();
				break;
			case MESSAGE_ERROR_SG_ESG:
				dismissDialog();
				break;
			default:
				break;
			}
			super.handleMessage(msg);
		}

	};

	private void dismissDialog() {
		if (mLoadDialog != null) {
			mLoadDialog.dismiss();
		}
		mLoadDialog = null;
	}

	public Handler getHandler() {
		return mHandler;
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.tv_manualrefresh:
			if (MBBMSService.sInstance != null) {
				if (MBBMSService.sInstance.isOperationFinish()) {
					Intent manulIntent = new Intent();
					manulIntent.setClass(RefreshManageActivity.this,ManualRefreshActivity.class);
					manulIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
					RefreshManageActivity.this.startActivity(manulIntent);
					RefreshManageActivity.sInstance = null;
					this.finish();
				}
			}

			break;
		case R.id.tv_autorefresh:
			startRefresh();
			break;

		}
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.setContentView(R.layout.refresh);
		sInstance = this;
		getShortPromptToast(R.string.refreshnote).show();
		initViews();
		setupViews();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
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

	@Override
	protected void onPause() {
		MBBMSEngine.getInstance().cancelMBBMSRequest();
		super.onPause();
	}

	@Override
	protected void onResume() {
		super.onResume();
		updateDate();
		updateViews();
	}

	/**
	 * call the function in main activity to start refresh
	 */
	private void startRefresh() {
		if (MBBMSService.sInstance != null) {
			if (MBBMSService.sInstance.isOperationFinish()) {
				MBBMSService.sInstance.setCityName(MBBMSService.LOCAL_CITY);
				MBBMSService.sInstance.startUsingNetwork(AUTOREFRESH);
			} else {

			}
		}

	}

	@Override
	public void initViews() {
		mManulTV = (TextView) findViewById(R.id.tv_manualrefresh);
		mAutoTV = (TextView) findViewById(R.id.tv_autorefresh);

	}

	@Override
	public void setupViews() {
		mManulTV.setOnClickListener(this);
		mAutoTV.setOnClickListener(this);
	}

	@Override
	public void updateDate() {
		// TODO Auto-generated method stub

	}

	@Override
	public void updateViews() {
		// TODO Auto-generated method stub

	}

}
