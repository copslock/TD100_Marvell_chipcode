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

import com.marvell.cmmb.R;
import com.marvell.cmmb.common.Define;
import com.marvell.cmmb.interfacetest.MBBMSInterfaceTestActivity;
import com.marvell.cmmb.service.MBBMSService;

import android.os.SystemProperties;
import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.graphics.LinearGradient;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.marvell.cmmb.activity.HelpInfoActivity;
import com.marvell.cmmb.activity.MBBMSSettingActivity;
import com.marvell.cmmb.activity.PlaybackSetting;
import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.aidl.MBBMSEngine;
import com.marvell.cmmb.aidl.ModuleInfo;

public class SubMenuDialog extends AlertDialog implements OnClickListener {
	private TextView mInterfaceTextView;// for interface test
	private TextView mHelpTextView;
	private TextView mSettingTextView;
	private TextView mAboutTextView;
	private TextView mPlaybackSetting;
	private LinearLayout mSubmenuLayout;
	private LayoutInflater mInflater;
	private Window mWindow = null;
	private static final String TAG = "SubMenuDialog";

	public SubMenuDialog(Context context) {
		super(context);
		mInflater = LayoutInflater.from(context);

	}

	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		initLayout();
		if (mSubmenuLayout != null) {
			setContentView(mSubmenuLayout);
			setProperty();
			setupView();
			checkTestProperty();
		}

	}

	private void initLayout() {
		if (MBBMSService.sCurrentMode == Define.MBBMS_MODE) {
			mSubmenuLayout = (LinearLayout) mInflater.inflate(
					R.layout.submenu_mbbms, null);
		} else if (MBBMSService.sCurrentMode == Define.CMMB_MODE) {
			mSubmenuLayout = (LinearLayout) mInflater.inflate(
					R.layout.submenu_cmmb, null);
		}
	}

	private void setupView() {

		mHelpTextView = (TextView) findViewById(R.id.tv_help);
		mHelpTextView.setOnClickListener(this);
		mAboutTextView = (TextView) findViewById(R.id.tv_about);
		mAboutTextView.setOnClickListener(this);
		mPlaybackSetting = (TextView)findViewById(R.id.tv_playbackset);
		mPlaybackSetting.setOnClickListener(this);
		if (MBBMSService.sCurrentMode == Define.MBBMS_MODE) {
			mSettingTextView = (TextView) findViewById(R.id.tv_setting);
			mSettingTextView.setOnClickListener(this);
			mInterfaceTextView = (TextView) findViewById(R.id.tv_interfacetest);
			mInterfaceTextView.setOnClickListener(this);
		}

	}

	public void setProperty() {// set the position of the subMenu
		mWindow = getWindow();
		WindowManager.LayoutParams wl = mWindow.getAttributes();
		wl.x = 70;
		wl.y = 140;
		wl.dimAmount = 0.001f;
		mWindow.setAttributes(wl);
	}

	private void checkTestProperty() {
		if (MBBMSService.sInstance != null
				&& MBBMSService.sCurrentMode == MBBMSService.MBBMS_MODE) {// mbbms mode
			if (SystemProperties.get("persist.mbbms.test.enabled").equals("1")) {
				LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
						"interface test enable", LogMask.APP_MAIN);
				mInterfaceTextView.setVisibility(View.VISIBLE);
			} else {
				LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
						"interface test disable", LogMask.APP_MAIN);
				mInterfaceTextView.setVisibility(View.GONE);
				mSubmenuLayout.setPadding(0, 10, 0, 0);
			}
		}
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.tv_help:
			Intent helpIntent = new Intent();
			helpIntent.setClass(this.getContext(), HelpInfoActivity.class);
			this.getContext().startActivity(helpIntent);
			this.dismiss();
			break;
		case R.id.tv_setting:
			Intent settingIntent = new Intent();
			settingIntent.setClass(this.getContext(),
					MBBMSSettingActivity.class);
			this.getContext().startActivity(settingIntent);
			this.dismiss();
			break;
		case R.id.tv_about:
			ModuleInfo mi = MBBMSEngine.getInstance().getModuleInfo();
			String vendorName = "";
			String moduleId = "";
			String firmwareVs = "";
			String cmmbLibv = "";
			String mbbmsLibv = "";
			String buildInfo = "";
			if (mi != null) {
				vendorName = mi.getVendorName();
				moduleId = mi.getModuleID();
				firmwareVs = mi.getFirmwareVersion();
				cmmbLibv = mi.getCmmbLibVersion();
				mbbmsLibv = mi.getMbbmsLibVersion();
				buildInfo = mi.getBuildInfo();
			}
			String version = getContext().getResources().getString(
					R.string.about_info, vendorName + " " + moduleId,
					firmwareVs, cmmbLibv, mbbmsLibv, buildInfo);
			PromptDialog pd = new PromptDialog(this.getContext(),
					R.drawable.dialog_hint, getContext().getResources()
							.getString(R.string.hint), version);
			pd.show();
			this.dismiss();
			break;
		case R.id.tv_interfacetest:
			Intent interfaceTestIntent = new Intent();
			interfaceTestIntent.setClass(SubMenuDialog.this.getContext(),
					MBBMSInterfaceTestActivity.class);
			this.getContext().startActivity(interfaceTestIntent);
			this.dismiss();
			break;
		case R.id.tv_playbackset:
		    this.getContext().startActivity(new Intent().setClass(getContext(), PlaybackSetting.class));
		    this.dismiss();
		    break;
		default:
			break;
		}
	}

	public boolean onKeyDown(int keyCode, KeyEvent event) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_MENU:
			this.dismiss();
			break;
		case KeyEvent.KEYCODE_BACK:
			this.dismiss();
			break;
		}
		return super.onKeyDown(keyCode, event);
	}
}
