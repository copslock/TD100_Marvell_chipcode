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

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

import com.marvell.cmmb.R;
import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.common.Action;
import com.marvell.cmmb.service.MBBMSService;

public class NoHeadPhoneDialog extends Activity implements OnClickListener, Action{
	private  final String TAG = getClass().getSimpleName();

	private TextView mPromptTextView;

	private Button mConfirmButton;

	
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.bt_confirm:
			LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "confirm no headphone",
					LogMask.APP_MAIN);	
			this.finish();
			break;

		}
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.tvplayer_promptdialog);
		setupView();
	}
	
	@Override
	protected void onDestroy() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "onDestroy", LogMask.APP_MAIN);
		super.onDestroy();
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_BACK:	
			this.finish();
			break;
		}
		return super.onKeyDown(keyCode, event);
	}

	private void setupView() {
		mPromptTextView = (TextView) findViewById(R.id.tv_promptmessage);
		mConfirmButton = (Button) findViewById(R.id.bt_confirm);		
		mConfirmButton.setOnClickListener(this);		
		mPromptTextView.setText(this.getResources().getString(R.string.earphone_pull_out));
	}

}
