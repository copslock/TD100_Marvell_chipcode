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

import android.content.SharedPreferences;
import android.graphics.Color;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.marvell.cmmb.R;
import com.marvell.cmmb.common.AppBase;
import com.marvell.cmmb.service.MBBMSService;
import com.marvell.cmmb.view.dialog.InputFreqDialog;
import com.marvell.cmmb.view.dialog.SelectCityDialog;
import com.marvell.cmmb.view.dialog.SelectScanTypeDialog;
import com.marvell.cmmb.view.dialog.SelectCityDialog.SetScanCity;
import com.marvell.cmmb.view.dialog.SelectScanTypeDialog.SetScanType;

public class CMMBSettingActivity extends AppBase implements OnClickListener {
	private TextView mByCityTextView;

	private TextView mSelectedTypeTextView;

	private TextView mSelectedCityTextView;

	private Button mCustomizeButton;

	private LinearLayout mScanTypeLLayout;

	private LinearLayout mScanCityLLayout;

	private SelectScanTypeDialog mSelectScanType;

	private InputFreqDialog mInputFreqDialog;

	private SelectCityDialog mSelectCityDialog;

	private SharedPreferences setting;

	private String mOrginalScanType;

	private String mOrginalScanCity;

	private int mOrginalMinFreg;

	private int mOrginalMaxFreq;

	private SetScanType mSetScanType = new SetScanType() {

		public void sendScanType(String scanType) {
			mSelectedTypeTextView.setText(scanType);
			setViewByScanType(scanType, CMMBSettingActivity.this.getResources()
					.getString(R.string.scanbycity), CMMBSettingActivity.this
					.getResources().getString(R.string.customizedscan));
		}
	};

	private SetScanCity mSetScanCity = new SetScanCity() {

		public void sendScanCity(String scanCity) {
			mSelectedCityTextView.setText(scanCity);
		}
	};

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.ll_scantype:
			if (!mSelectScanType.isShowing()) {
				mSelectScanType.show();
			}
			break;
		case R.id.ll_city:
			if (!mSelectCityDialog.isShowing()) {
				mSelectCityDialog.show();
			}
			break;
		case R.id.bt_customize:
			if (!mInputFreqDialog.isShowing()) {
				mInputFreqDialog.show();
			}
			break;
		}
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.cmmb_setting);
		initViews();
		setupViews();
	}

	protected void onResume() {
		updateDate();
		updateViews();
		super.onResume();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_BACK:
			getBack();
			return true;
		}
		return super.onKeyDown(keyCode, event);
	}

	private void getBack() {
		String scanType = setting.getString("scanType", this.getResources()
				.getString(R.string.scanall));
		String scanCity = setting.getString("scanCity", this.getResources()
				.getString(R.string.defaultcity));
		int minFreq = setting.getInt("minFreq", 13);
		int maxFreq = setting.getInt("maxFreq", 48);
		if (!mOrginalScanType.equals(scanType)) {
			MBBMSService.isCMMBSettingChanged = true;
		} else {
			if (mOrginalScanType.equals(this.getResources().getString(
					R.string.scanbycity)) && !mOrginalScanCity.equals(scanCity)) {
				MBBMSService.isCMMBSettingChanged = true;
			} else if (mOrginalScanType.equals(this.getResources().getString(
					R.string.customizedscan))
					&& (mOrginalMinFreg != minFreq || mOrginalMaxFreq != maxFreq)) {
				MBBMSService.isCMMBSettingChanged = true;
			} else {
				MBBMSService.isCMMBSettingChanged = false;
			}
		}
		this.finish();
	}

	private void setOrginalSetting() {
		setting = getSharedPreferences(CMMBSETING, MODE_PRIVATE);
		mOrginalScanType = setting.getString("scanType", this.getResources()
				.getString(R.string.scanall));
		mOrginalScanCity = setting.getString("scanCity", this.getResources()
				.getString(R.string.defaultcity));
		mOrginalMinFreg = setting.getInt("minFreq", 13);
		mOrginalMaxFreq = setting.getInt("maxFreq", 48);
	}

	private void setViewByScanType(String scanType, String byCityString,
			String customizeString) {
		if (scanType.equals(byCityString)) {// scan by city
			String city = setting.getString("scanCity", this.getResources()
					.getString(R.string.defaultcity));
			mSelectedCityTextView.setText(city);
			mCustomizeButton.setTextColor(Color.GRAY);
			mByCityTextView.setTextColor(Color.BLACK);
			mCustomizeButton.setClickable(false);
			mScanCityLLayout.setClickable(true);
		} else if (scanType.equals(customizeString)) {// customized scan
			String city = setting.getString("scanCity", this.getResources()
					.getString(R.string.defaultcity));
			mByCityTextView.setTextColor(Color.GRAY);
			mSelectedCityTextView.setText(city);
			mCustomizeButton.setTextColor(Color.BLACK);
			mCustomizeButton.setClickable(true);
			mScanCityLLayout.setClickable(false);
		} else {// Scan all
			String city = setting.getString("scanCity", this.getResources()
					.getString(R.string.defaultcity));
			mSelectedCityTextView.setText(city);
			mByCityTextView.setTextColor(Color.GRAY);
			mCustomizeButton.setTextColor(Color.GRAY);
			mCustomizeButton.setClickable(false);
			mScanCityLLayout.setClickable(false);
		}
	}

	@Override
	public void initViews() {
		mByCityTextView = (TextView) findViewById(R.id.tv_by_city);
		mSelectedTypeTextView = (TextView) findViewById(R.id.tv_type_selected);
		mSelectedCityTextView = (TextView) findViewById(R.id.tv_city);
		mCustomizeButton = (Button) findViewById(R.id.bt_customize);
		mScanTypeLLayout = (LinearLayout) findViewById(R.id.ll_scantype);
		mScanCityLLayout = (LinearLayout) findViewById(R.id.ll_city);
	}

	@Override
	public void setupViews() {
		mCustomizeButton.setOnClickListener(this);
		mScanTypeLLayout.setOnClickListener(this);
		mScanCityLLayout.setOnClickListener(this);
	}

	@Override
	public void updateDate() {
		setOrginalSetting();
	}

	@Override
	public void updateViews() {
		String scanType = mOrginalScanType;
		mSelectedTypeTextView.setText(scanType);
		setViewByScanType(scanType, this.getResources().getString(
				R.string.scanbycity), this.getResources().getString(
				R.string.customizedscan));

		mSelectScanType = new SelectScanTypeDialog(this);
		mSelectScanType.setmScanType(mSetScanType);
		mInputFreqDialog = new InputFreqDialog(this, R.style.dialog_Theme);
		mSelectCityDialog = new SelectCityDialog(this);
		mSelectCityDialog.setmSetScanCity(mSetScanCity);
	}

}
