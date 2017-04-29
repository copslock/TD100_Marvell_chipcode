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
import android.widget.TextView;

import com.marvell.cmmb.R;

/**
 * The view to show the recording time
 * 
 * @author DanyangZhou
 * @version [version, 2011-4-1]
 */
public class RecTime extends TextView {
	private int mHour;

	private int mMin;

	private int mSec;

	private int mRecordingTime;

	private String mTime;

	private int[] mRecTag = new int[] { R.drawable.recording_tag_on,
			R.drawable.recording_tag_off };

	private static final int MSG_SECOND = 1;

	public RecTime(Context context) {
		super(context);
	}

	public RecTime(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	protected void onFinishInflate() {
		super.onFinishInflate();
	}

	public void start() {
		reset();
		this.setVisibility(View.VISIBLE);
		mHandler.sendEmptyMessageDelayed(MSG_SECOND, 1000);
	}

	public void stop() {
		this.setVisibility(View.GONE);
		mHandler.removeMessages(MSG_SECOND);
		reset();
	}

	public void reset() {
		mHour = 0;
		mMin = 0;
		mSec = 0;
		mRecordingTime = 0;
		mTime = "00:00:00";
		setTime();
	}

	private void setTime() {
		mTime = (mHour < 10 ? ("0" + mHour) : mHour) + ":"
				+ (mMin < 10 ? ("0" + mMin) : mMin) + ":"
				+ (mSec < 10 ? ("0" + mSec) : mSec);
		this.setText(this.getResources().getString(R.string.recordingtime, mTime));
		this.setCompoundDrawablesWithIntrinsicBounds(mRecTag[mSec % 2], 0, 0, 0);
	}

	private void timeTick() {
		mSec++;
		if (mSec > 59) {
			mSec = 0;
			mMin++;
			if (mMin > 59) {
				mMin = 0;
				mHour++;
			}
		}
		mHandler.sendEmptyMessageDelayed(MSG_SECOND, 1000);
	}

	private Handler mHandler = new Handler() {

		public void handleMessage(Message msg) {
			switch (msg.what) {
			case MSG_SECOND:
				timeTick();
				setTime();
				break;
			}
			super.handleMessage(msg);
		}

	};

	/**
	 * 
	 * seconds of total recording time
	 */
	public void setRecordingTime(int seconds) {
		mHour = seconds >= (60 * 60) ? seconds / (60 * 60) : 0;
		mMin = (seconds - mHour * 60 * 60) >= 60 ? (seconds - mHour * 60 * 60) / 60 : 0;
		mSec = seconds % 60;
	}

	public void resume(int seconds) {
		setRecordingTime(seconds);
		setTime();
		this.setVisibility(View.VISIBLE);
		mHandler.removeMessages(MSG_SECOND);
		mHandler.sendEmptyMessageDelayed(MSG_SECOND, 1000);
	}
}
