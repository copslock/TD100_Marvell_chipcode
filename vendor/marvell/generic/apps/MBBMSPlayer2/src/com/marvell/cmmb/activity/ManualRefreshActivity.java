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
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.TextView;

import com.marvell.cmmb.R;
import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.aidl.MBBMSEngine;
import com.marvell.cmmb.common.AppBase;
import com.marvell.cmmb.service.MBBMSService;
import com.marvell.cmmb.view.dialog.LoadingDialog;

/**
 * @description: provide a view to select a city to refresh
 * 
 */
public class ManualRefreshActivity extends AppBase implements OnClickListener {
	private String TAG = "ManualRefreshActivity";

	private ArrayAdapter<String> mCityAdapter;

	public static ManualRefreshActivity sInstance;

	private Spinner mCitySpinner;

	private LoadingDialog mLoadDialog = null;

	private String[] cityList;

	private String currentCityName;

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
			mLoadDialog = null;
		}
	}

	public Handler getHandler() {
		return mHandler;
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.bt_refresh:
			setCurrentCityName();
			refreshCitySG();
			break;
		default:
			break;
		}
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.setContentView(R.layout.refresh_manual);
		sInstance = this;
		showRefreshPrompt();
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
		// TODO Auto-generated method stub
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
	 * @description: get the specific city's sg and subscription information set
	 *               the operation type to indicate the operation
	 * @param cityName
	 * 
	 * 
	 */
	private void refreshCitySG() {

		// if(MBBMSService.sInstance.operateList.size() != 0) {
		if (MBBMSService.sInstance.isOperationFinish()) {
			if (currentCityName != null) {
				MBBMSService.sInstance.setCityName(currentCityName);
				MBBMSService.sInstance.startUsingNetwork(MANUALREFRESH);
			} else {
				// do nothing
			}
		}
	}

	private void setCurrentCityName() {
		TextView cityView = (TextView) mCitySpinner.getSelectedView();
		currentCityName = cityView.getText().toString();
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "currentCityName is "
				+ currentCityName, LogMask.APP_COMMON);
	}

	private void showRefreshPrompt() {
		getShortPromptToast(R.string.refresh_prompt).show();
	}

	@Override
	public void initViews() {
		cityList = this.getResources().getStringArray(R.array.cities);
		mCityAdapter = new ArrayAdapter<String>(this,
				android.R.layout.simple_spinner_item, cityList);
		mCityAdapter.setDropDownViewResource(R.layout.setting_spinner);
		mCitySpinner = (Spinner) findViewById(R.id.sp_city);
		mCitySpinner.setAdapter(mCityAdapter);
	}

	@Override
	public void setupViews() {
		Button refreshButton = (Button) findViewById(R.id.bt_refresh);
		refreshButton.setOnClickListener(this);
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
