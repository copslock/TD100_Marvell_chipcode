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

package com.marvell.cmmb.view.customview;

import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.view.View;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.marvell.cmmb.R;

public class LoadingView extends LinearLayout {
	private static final int MSG_SECOND = 1;

	private TextView mTitle;

	private TextView mInfo;

	private ImageView mLogo;

	private ImageView mLoading;

	private long mScrollTime = 100;

	private int count;

	int[] iconIds = { R.drawable.load_01, R.drawable.load_02,
			R.drawable.load_03, R.drawable.load_04, R.drawable.load_05,
			R.drawable.load_06, R.drawable.load_07, R.drawable.load_08 };

	private Handler mHandler = new Handler() {

		public void handleMessage(Message msg) {
			switch (msg.what) {
			case MSG_SECOND:
				timeTick();
				loadImage();
				break;
			}
			super.handleMessage(msg);
		}

	};

	public LoadingView(Context context) {
		super(context);
	}

	public LoadingView(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	protected void onFinishInflate() {
		super.onFinishInflate();
		findItem();
		startLoading();
	}

	private void findItem() {
		mTitle = (TextView) findViewById(R.id.tv_title);
		mInfo = (TextView) findViewById(R.id.tv_info);
		mLogo = (ImageView) findViewById(R.id.iv_logo);
		mLoading = (ImageView) findViewById(R.id.iv_load);
	}

	private void timeTick() {
		mHandler.sendEmptyMessageDelayed(MSG_SECOND, mScrollTime);
	}

	private void loadImage() {
		if (count == iconIds.length) {
			count = 0;
		}
		mLoading.setImageResource(iconIds[count]);
		count++;
	}

	public void setTitle(int resid) {
		mTitle.setText(resid);
	}

	public void setTitle(String res) {
		mTitle.setText(res);
	}

	public void setTitleSize(float size) {
		mTitle.setTextSize(size);
	}

	public void setInfoSize(float size) {
		mInfo.setTextSize(size);
	}

	public void setInfo(int resid) {
		mInfo.setText(resid);
	}

	public void setInfo(String res) {
		mInfo.setText(res);
	}

	public void setLogo(int resId) {
		mLogo.setImageResource(resId);
	}

	public void startLoading() {
		count = 0;
		timeTick();
	}

	public void stopLoading() {
		mHandler.removeMessages(MSG_SECOND);
		count = 0;
	}

	protected void onVisibilityChanged(View changedView, int visibility) {
		super.onVisibilityChanged(changedView, visibility);
		if (changedView == this) {
			switch (visibility) {
			case VISIBLE:
				startLoading();
				break;
			case INVISIBLE:
			case GONE:
				stopLoading();
				break;
			}
		}
	}

}
