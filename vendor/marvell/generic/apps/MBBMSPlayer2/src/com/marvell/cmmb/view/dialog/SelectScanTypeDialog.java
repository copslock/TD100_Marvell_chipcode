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

package com.marvell.cmmb.view.dialog;

import android.app.AlertDialog;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

import com.marvell.cmmb.R;
import com.marvell.cmmb.common.Define;

public class SelectScanTypeDialog extends AlertDialog implements
		OnClickListener {
	public interface SetScanType {
		public void sendScanType(String scanType);
	}

	private TextView mScanAllTextView;

	private TextView mScanCityView;

	private TextView mCoustermizedScanView;

	private Button mCancelButton;

	private Resources mResource;

	private SetScanType mScanType;

	public SelectScanTypeDialog(Context context) {
		super(context);
	}

	public SetScanType getmScanType() {
		return mScanType;
	}

	public void onClick(View v) {
		String scanType;
		SharedPreferences cmmbSetting = v.getContext().getSharedPreferences(
				Define.CMMBSETING, Context.MODE_PRIVATE);
		SharedPreferences.Editor cmmbEditor = cmmbSetting.edit();
		switch (v.getId()) {
		case R.id.tv_scanall:
			mScanAllTextView.setCompoundDrawablesWithIntrinsicBounds(
					R.drawable.radio_on, 0, 0, 0);
			mScanCityView.setCompoundDrawablesWithIntrinsicBounds(
					R.drawable.radio_off, 0, 0, 0);
			mCoustermizedScanView.setCompoundDrawablesWithIntrinsicBounds(
					R.drawable.radio_off, 0, 0, 0);

			scanType = mResource.getString(R.string.scanall);
			cmmbEditor.putString("scanType", scanType);
			cmmbEditor.commit();
			this.dismiss();
			if (null != mScanType) {
				mScanType.sendScanType(scanType);
			}
			break;

		case R.id.tv_scancity:
			mScanAllTextView.setCompoundDrawablesWithIntrinsicBounds(
					R.drawable.radio_off, 0, 0, 0);
			mScanCityView.setCompoundDrawablesWithIntrinsicBounds(
					R.drawable.radio_on, 0, 0, 0);
			mCoustermizedScanView.setCompoundDrawablesWithIntrinsicBounds(
					R.drawable.radio_off, 0, 0, 0);
			scanType = mResource.getString(R.string.scanbycity);
			cmmbEditor.putString("scanType", scanType);
			cmmbEditor.commit();
			this.dismiss();
			if (null != mScanType) {
				mScanType.sendScanType(scanType);
			}
			break;

		case R.id.tv_customizedscan:
			mScanAllTextView.setCompoundDrawablesWithIntrinsicBounds(
					R.drawable.radio_off, 0, 0, 0);
			mScanCityView.setCompoundDrawablesWithIntrinsicBounds(
					R.drawable.radio_off, 0, 0, 0);
			mCoustermizedScanView.setCompoundDrawablesWithIntrinsicBounds(
					R.drawable.radio_on, 0, 0, 0);
			scanType = mResource.getString(R.string.customizedscan);
			cmmbEditor.putString("scanType", scanType);
			cmmbEditor.commit();
			this.dismiss();
			if (null != mScanType) {
				mScanType.sendScanType(scanType);
			}
			break;

		case R.id.bt_cancel:
			this.dismiss();
			break;
		}
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.scan_type_layout);
		mResource = this.getContext().getResources();
		setupView();
	}

	@Override
	protected void onStart() {
		super.onStart();
		SharedPreferences setting = this.getContext().getSharedPreferences(
				Define.CMMBSETING, Context.MODE_PRIVATE);
		String scanType = setting.getString("scanType", mResource
				.getString(R.string.scanall));
		if (scanType.equals(mResource.getString(R.string.scanbycity))) {// scan by city
			mScanAllTextView.setCompoundDrawablesWithIntrinsicBounds(
					R.drawable.radio_off, 0, 0, 0);
			mScanCityView.setCompoundDrawablesWithIntrinsicBounds(
					R.drawable.radio_on, 0, 0, 0);
			mCoustermizedScanView.setCompoundDrawablesWithIntrinsicBounds(
					R.drawable.radio_off, 0, 0, 0);
		} else if (scanType
				.equals(mResource.getString(R.string.customizedscan))) {// customized scan
			mScanAllTextView.setCompoundDrawablesWithIntrinsicBounds(
					R.drawable.radio_off, 0, 0, 0);
			mScanCityView.setCompoundDrawablesWithIntrinsicBounds(
					R.drawable.radio_off, 0, 0, 0);
			mCoustermizedScanView.setCompoundDrawablesWithIntrinsicBounds(
					R.drawable.radio_on, 0, 0, 0);
		} else {// Scan all
			mScanAllTextView.setCompoundDrawablesWithIntrinsicBounds(
					R.drawable.radio_on, 0, 0, 0);
			mScanCityView.setCompoundDrawablesWithIntrinsicBounds(
					R.drawable.radio_off, 0, 0, 0);
			mCoustermizedScanView.setCompoundDrawablesWithIntrinsicBounds(
					R.drawable.radio_off, 0, 0, 0);
		}
	}

	public void setmScanType(SetScanType mScanType) {
		this.mScanType = mScanType;
	}

	/**
	 * Initialize the view of confirm dialog
	 */
	private void setupView() {
		mScanAllTextView = (TextView) findViewById(R.id.tv_scanall);
		mScanCityView = (TextView) findViewById(R.id.tv_scancity);
		mCoustermizedScanView = (TextView) findViewById(R.id.tv_customizedscan);
		mCancelButton = (Button) findViewById(R.id.bt_cancel);
		mCancelButton.setOnClickListener(this);
		mScanAllTextView.setOnClickListener(this);
		mScanCityView.setOnClickListener(this);
		mCoustermizedScanView.setOnClickListener(this);
	}

}
