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

import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.marvell.cmmb.R;
import com.marvell.cmmb.activity.EbMsgDetailActivity;
import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.common.Define;
import com.marvell.cmmb.service.MBBMSService;

public class EbMessageDialog extends Activity implements OnClickListener,
		Define {
	private final String TAG = getClass().getSimpleName();

	private TextView mTvHint;

	private TextView mTvMessage;

	private TextView mPageInfo;

	private ImageView mIvLevel;

	private Button mBtOk;

	private Button mBtCancel;

	private LinearLayout mLLPageInfo;

	private ImageView mPrevious;

	private ImageView mNext;

	private Intent mIntent;

	private int[] mImageLevelSrc = new int[] { R.drawable.ebmessage_1,
			R.drawable.ebmessage_2, R.drawable.ebmessage_3,
			R.drawable.ebmessage_4, };

	private List<Intent> mMessages = new ArrayList<Intent>();

	private int mCurrentMessage = 0;

	private void cancel() {

	}

	private void confirm() {
		Intent emergencyIntent = new Intent();
		Bundle bundle = new Bundle();
		bundle.putInt(INTENT_MESSAGE_NETLEVEL, mIntent.getIntExtra(
				INTENT_MESSAGE_NETLEVEL, 0));
		bundle.putInt(INTENT_MESSAGE_NETID, mIntent.getIntExtra(
				INTENT_MESSAGE_NETID, 0));
		bundle.putInt(INTENT_MESSAGE_MSGID, mIntent.getIntExtra(
				INTENT_MESSAGE_MSGID, 0));
		bundle.putInt(INTENT_MESSGAE_LEVEL, mIntent.getIntExtra(
				INTENT_MESSGAE_LEVEL, 0));
		bundle.putString(INTENT_MESSGAE_DATE, mIntent
				.getStringExtra(INTENT_MESSGAE_DATE));
		bundle.putString(INTENT_MESSGAE_TIME, mIntent
				.getStringExtra(INTENT_MESSGAE_TIME));
		bundle.putString(INTENT_MESSGAE_TEXT, mIntent
				.getStringExtra(INTENT_MESSGAE_TEXT));
		emergencyIntent.putExtras(bundle);
		emergencyIntent.setClass(this.getApplicationContext(),
				EbMsgDetailActivity.class);
		this.startActivity(emergencyIntent);
	}

	private void ifShowArrow() {
		if (mMessages != null && mMessages.size() > 1) {
			mLLPageInfo.setVisibility(View.VISIBLE);
		} else {
			mLLPageInfo.setVisibility(View.GONE);
		}
	}

	private void ifShowNewMessage() {
		if (mMessages != null && mMessages.size() > 1) {
			int level = mMessages.get(mMessages.size() - 1).getIntExtra(
					INTENT_MESSGAE_LEVEL, 0);
			if (level == 1 || level == 2) {
				mCurrentMessage = mMessages.size() - 1;
			}
		}
	}

	public void init() {
		mMessages.clear();
		mMessages.add(getIntent());
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.tvplayer_ebmessage);

		mIvLevel = (ImageView) findViewById(R.id.iv_level);
		mTvHint = (TextView) findViewById(R.id.tv_hint);
		mTvMessage = (TextView) findViewById(R.id.tv_promptmessage);
		mBtOk = (Button) findViewById(R.id.bt_confirm);
		mBtCancel = (Button) findViewById(R.id.bt_cancel);
		mLLPageInfo = (LinearLayout) findViewById(R.id.page_info_layout);
		mPrevious = (ImageView) findViewById(R.id.iv_previous);
		mNext = (ImageView) findViewById(R.id.iv_next);
		mPageInfo = (TextView) findViewById(R.id.tv_page_info);
		mBtOk.setOnClickListener(this);
		mBtCancel.setOnClickListener(this);
		mPrevious.setOnClickListener(this);
		mNext.setOnClickListener(this);
	}

	private void loadData() {
		if (mMessages != null && mMessages.size() > 0) {
			mPageInfo.setText((mCurrentMessage + 1) + "/" + mMessages.size());
			mIntent = mMessages.get(mCurrentMessage);
			if (null != mIntent) {
				int level = mIntent.getIntExtra(INTENT_MESSGAE_LEVEL, 0) - 1;
				if (level >= 0 && level < mImageLevelSrc.length) {
					mIvLevel.setImageResource(mImageLevelSrc[level]);
				} else {
					mIvLevel.setImageBitmap(null);
				}
				String message = mIntent.getStringExtra(INTENT_MESSGAE_TEXT);
				String date = mIntent.getStringExtra(INTENT_MESSGAE_DATE);
				String time = mIntent.getStringExtra(INTENT_MESSGAE_TIME);
				String info = "";
				if (level >= 2 && level < 4) {
					info = getResources().getString(R.string.emergencytoread);
					mBtOk.setVisibility(View.VISIBLE);
					mBtCancel.setVisibility(View.VISIBLE);
				} else {
					info = (date == null ? "" : date) + " "
							+ (time == null ? "" : time) + " "
							+ (message == null ? "" : message);
					mBtOk.setVisibility(View.GONE);
					mBtCancel.setVisibility(View.GONE);
				}
				mTvMessage.setText(info);
			}
		}
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.bt_confirm:
			confirm();
			this.finish();
			break;
		case R.id.bt_cancel:
			cancel();
			this.finish();
			break;
		case R.id.iv_previous:
			toPreviousMessage();
			break;
		case R.id.iv_next:
			toNextMessage();
			break;
		default:
			break;
		}
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		init();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
	}

	@Override
	protected void onNewIntent(Intent intent) {
		super.onNewIntent(intent);
		mMessages.add(intent);
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onNewIntent", LogMask.APP_COMMON);
	}

	@Override
	protected void onPause() {
		super.onPause();
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onPause", LogMask.APP_COMMON);
	}

	@Override
	protected void onResume() {
		super.onResume();
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onResume", LogMask.APP_COMMON);
		ifShowArrow();
		ifShowNewMessage();
		loadData();
	}

	public boolean onKeyDown(int keyCode, KeyEvent event) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_BACK:
			if (null != MBBMSService.sInstance) {
				MBBMSService.sInstance.resumeToPlay();
			}
			break;
		default:
			break;
		}
		return super.onKeyDown(keyCode, event);
	}

	private void toNextMessage() {
		if (mCurrentMessage < mMessages.size() - 1) {
			mCurrentMessage++;
			loadData();
		}
	}

	private void toPreviousMessage() {
		if (mCurrentMessage > 0) {
			mCurrentMessage--;
			loadData();
		}

	}
}
