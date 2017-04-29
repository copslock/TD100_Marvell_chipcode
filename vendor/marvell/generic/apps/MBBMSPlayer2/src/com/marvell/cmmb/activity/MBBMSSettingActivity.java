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

import android.content.SharedPreferences;
import android.database.Cursor;
import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.ScrollView;
import android.widget.TextView;

import com.marvell.cmmb.R;
import com.marvell.cmmb.common.AppBase;
import com.marvell.cmmb.service.MBBMSService;

public class MBBMSSettingActivity extends AppBase implements OnClickListener {
	private EditText mSGAdressEditText;

	private Button mInitButton;

	private Button mSaveButton;
	
	private Button mShowAPNList;

	private static final String SETTING = "MBBMSSETTING";

	private String sgAddress;

	private String sgPort;

	private static final String TAG = "TVPlayerSettingActivity";

	private ArrayList<String> mAPNList = new ArrayList<String>();
	
	private LayoutInflater mInflater;
	
	private PopupWindow pw;
	
	private TextView mDefaultAPNView;
	
	private ScrollView sView;

	private LinearLayout mAPNLayout;

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.bt_initial:
			setDefault();
			break;
		case R.id.bt_save:
			savaData();
			break;

		}
	}

	private void init() {
		Uri uri = Uri.parse("content://telephony/carriers");
		Cursor cr = this.getContentResolver().query(uri, null, "type = 'wap'", null, null);
		String apn = "";
		while (cr != null && cr.moveToNext()) {
			apn = cr.getString(cr.getColumnIndex("name"));
			mAPNList.add(apn);
		}
		if (mAPNList.size() > 0) {
			for (int i = 0; i < mAPNList.size(); i++) {
				addTextView(i);
			}
		}
	}

	private void savaData() {
		SharedPreferences setting = getSharedPreferences(SETTING, MODE_PRIVATE);
		SharedPreferences.Editor editor = setting.edit();
		String address = mSGAdressEditText.getText().toString();
		if (address.contains(":")) {
			int index = address.lastIndexOf(":");
			sgAddress = address.substring(0, index);
			sgPort = address.substring(index + 1);
		} else {
			sgAddress = address;
			sgPort = "";
		}
		editor.putString("sgAddress", sgAddress);
		editor.putString("sgPort", sgPort);
		editor.commit();
		getShortPromptToast(R.string.savesettingsuccess).show();
		MBBMSService.sInstance.setPreference();
	}

	private void setDefault() {
		sgAddress = "sg.mbbms.chinamobile.com";
		sgPort = "80";
		mSGAdressEditText.setText(sgAddress + ":" + sgPort);
		SharedPreferences initsetting = getSharedPreferences(SETTING, MODE_PRIVATE);
		SharedPreferences.Editor initeditor = initsetting.edit();
		initeditor.putString("sgAddress", sgAddress);
		initeditor.putString("sgPort", sgPort);
		initeditor.commit();
		getShortPromptToast(R.string.initialsetting).show();
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.setting);
		initViews();
		setupViews();
		init();
	}

	protected void onResume() {
		super.onResume();
		updateDate();
		updateViews();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
	}

	private void addTextView(int i) {
		TextView tView = new TextView(this);//  
		tView.setText(mAPNList.get(i));//  
		tView.setTextSize((float) 19.0);
		tView.setTextColor(Color.BLACK);
		LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
				LinearLayout.LayoutParams.FILL_PARENT,
				LinearLayout.LayoutParams.WRAP_CONTENT);
		mAPNLayout.addView(tView, params);
		mAPNLayout.setPadding(10, 0, 0, 0);

	}

	@Override
	public void initViews() {
		mSGAdressEditText = (EditText) findViewById(R.id.et_sgaddress);
		mInitButton = (Button) findViewById(R.id.bt_initial);
		mSaveButton = (Button) findViewById(R.id.bt_save);
		mAPNLayout = (LinearLayout) this.findViewById(R.id.ll_apnlist);
	}

	@Override
	public void setupViews() {
		mInitButton.setOnClickListener(this);
		mSaveButton.setOnClickListener(this);

	}

	@Override
	public void updateDate() {
		SharedPreferences setting = getSharedPreferences(SETTING, MODE_PRIVATE);
		sgAddress = setting.getString("sgAddress", "sg.mbbms.chinamobile.com");
		sgPort = setting.getString("sgPort", "80");
	}

	@Override
	public void updateViews() {
		if (sgPort.equals("")) {
			mSGAdressEditText.setText(sgAddress);
		} else {
			mSGAdressEditText.setText(sgAddress + ":" + sgPort);
		}
	}

}
