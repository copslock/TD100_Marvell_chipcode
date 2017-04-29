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
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import com.marvell.cmmb.R;
import com.marvell.cmmb.common.Define;
import com.marvell.cmmb.common.Util;

public class CancelDialog extends AlertDialog implements OnClickListener, Define {
	private TextView mTitleTextView;

	private TextView mPromptTextView;

	private Button mCancelButton;

	private String mTitle;

	private String mPromptMessage;

	private int mDrawableResId;

	private CancelDialogCallback mCallBack;

	private ImageView mLoadImageView;

	private int mType;

	private int mIndex;

	private static final String TAG = "CancelDialog";

	Handler mHandler = new Handler() {

		// handle event for loading
		@Override
		public void handleMessage(Message msg) {
			int[] iconIds = { R.drawable.load_01, R.drawable.load_02,
					R.drawable.load_03, R.drawable.load_04, R.drawable.load_05,
					R.drawable.load_06, R.drawable.load_07, R.drawable.load_08 };
			switch (msg.what) {
			case MESSAGE_LOADING:
				mIndex = mIndex % iconIds.length;
				mLoadImageView.setImageResource(iconIds[mIndex]);
				mHandler.sendMessageDelayed(Message.obtain(mHandler, MESSAGE_LOADING), 80);
				break;
			}
			mIndex++;
			super.handleMessage(msg);
		}
	};

	public CancelDialog(Context context, String title, String promptMessage) {
		super(context);
		this.mTitle = title;
		this.mPromptMessage = promptMessage;

	}

	@Override
	public void dismiss() {
		if (mHandler.hasMessages(MESSAGE_LOADING)) {
			mHandler.removeMessages(MESSAGE_LOADING);
		}
		super.dismiss();
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.bt_cancel:
			this.dismiss();
			if (null != mCallBack) {
				mCallBack.cancelDialogCancelled(mType);
			}
			break;
		}

	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.tvplayer_canceldialog);
		Util.setProperty(this);
		setupView();
		mHandler.sendEmptyMessage(MESSAGE_LOADING);
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_BACK:
			if (null != mCallBack) {
				mCallBack.cancelDialogCancelled(mType);
			}
			break;
		}
		return super.onKeyDown(keyCode, event);
	}

	public void setCallBack(CancelDialogCallback callBack) {
		mCallBack = callBack;
	}

	public void setCallBack(CancelDialogCallback callBack, int type) {
		mCallBack = callBack;
		mType = type;
	}

	/**
	 * Initialize the view of prompt dialog
	 */
	private void setupView() {
		mTitleTextView = (TextView) findViewById(R.id.tv_hint);
		mTitleTextView.setText(mTitle);
		mTitleTextView.setCompoundDrawablesWithIntrinsicBounds(mDrawableResId, 0, 0, 0);
		mPromptTextView = (TextView) findViewById(R.id.tv_promptmessage);
		mPromptTextView.setText(mPromptMessage);
		mLoadImageView = (ImageView) findViewById(R.id.iv_loading);
		mCancelButton = (Button) findViewById(R.id.bt_cancel);
		mCancelButton.setOnClickListener(this);

	}
}
