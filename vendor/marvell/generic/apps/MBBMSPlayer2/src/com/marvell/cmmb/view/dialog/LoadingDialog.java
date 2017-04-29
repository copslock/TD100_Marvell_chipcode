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
import android.widget.ImageView;
import android.widget.TextView;

import com.marvell.cmmb.R;
import com.marvell.cmmb.common.Define;
import com.marvell.cmmb.common.Util;

public class LoadingDialog extends AlertDialog {
	public interface LoadBack {
		public void onBack();
	}

	private TextView mTitleTextView;

	private TextView mPromptTextView;

	private ImageView mLoadImageView;

	private String mTitle;

	private String mPromptMessage;

	private int mDrawableResId;

	private int mOffset;

	private LoadBack mCallBack;

	private static final String TAG = "LoadingDialog";

	Handler mHandler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			int[] iconIds = { R.drawable.load_01, R.drawable.load_02,
					R.drawable.load_03, R.drawable.load_04, R.drawable.load_05,
					R.drawable.load_06, R.drawable.load_07, R.drawable.load_08 };
			int index = 0;
			switch (msg.what) {
			case Define.MESSAGE_LOADING:
				index = mOffset % iconIds.length;
				mLoadImageView.setImageResource(iconIds[index]);
				mHandler.sendMessageDelayed(Message.obtain(mHandler,
						Define.MESSAGE_LOADING), 80);
				break;
			}
			mOffset++;
			super.handleMessage(msg);
		}
	};

	public LoadingDialog(Context context, int resId, String title,
			String promptMessage) {
		super(context);
		this.mTitle = title;
		this.mPromptMessage = promptMessage;
		this.mDrawableResId = resId;
	}

	@Override
	public void dismiss() {
		if (mHandler.hasMessages(Define.MESSAGE_LOADING)) {
			mHandler.removeMessages(Define.MESSAGE_LOADING);
		}
		super.dismiss();
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.tvplayer_progressdialog);
		Util.setProperty(this);
		setupView();
		mHandler.sendEmptyMessage(Define.MESSAGE_LOADING);

	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_BACK:
			if (mCallBack != null) {
				mCallBack.onBack();
			}
			return true;
		}
		return super.onKeyDown(keyCode, event);
	}

	public void setCallBack(LoadBack lb) {
		mCallBack = lb;
	}

	/**
	 * Initialize the view of loading dialog
	 */
	private void setupView() {
		mTitleTextView = (TextView) findViewById(R.id.tv_hint);
		mTitleTextView.setText(mTitle);
		mTitleTextView.setCompoundDrawablesWithIntrinsicBounds(mDrawableResId,
				0, 0, 0);
		mPromptTextView = (TextView) findViewById(R.id.tv_promptmessage);
		mPromptTextView.setText(mPromptMessage);
		mLoadImageView = (ImageView) findViewById(R.id.iv_loading);
	}

}
