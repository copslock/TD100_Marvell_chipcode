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

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import com.marvell.cmmb.R;
import com.marvell.cmmb.common.AppBase;
import com.marvell.cmmb.view.dialog.ConfirmDialog;
import com.marvell.cmmb.view.dialog.ConfirmDialogCallBack;

public class EbMsgDetailActivity extends AppBase implements OnClickListener,
		ConfirmDialogCallBack {
	private ImageView mLevelImageView;

	private TextView mDateTextView;

	private TextView mTimeTextView;

	private TextView mMessageTextView;

	private Button mDeleteButton;

	private int mNetId;

	private int mMsgId;

	private int mNetLevel;
	
	private String mEmergencyDate;
	
	private String mEmergencyTime;
	
	private int mMsgLevel;

	private Intent mIntent;

	private ConfirmDialog mConfirmDialog;

	private static final int DELETE_FLAG_FALSE = 0;// emergency message has not been deleted

	private static final int DELETE_FLAG_TRUE = 1;// emergency message has been deleted

	private Handler mEbMsgHandler;

	public void confirmDialogCancel(int type) {

	}

	public void confirmDialogOk(int type) {
		mResolver.updateEmergencyById(DELETE_FLAG_TRUE, mNetLevel, mNetId,
				mMsgId, mEmergencyDate, mEmergencyTime, mMsgLevel);
		this.finish();
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.bt_delete:
			showConfirmDialog(DIALOG_CONFIRM_EMERG_DELETE_ITEM);
			break;
		default:
			break;
		}
	}

	private void showConfirmDialog(int type) {

		if (mConfirmDialog == null) {
			mConfirmDialog = new ConfirmDialog(this, R.drawable.dialog_hint,
					getResources().getString(R.string.hint), getResources()
							.getString(R.string.deleteoneconfirm));
		}
		mConfirmDialog.setCallBack(this, type);
		if (!mConfirmDialog.isShowing()) {
			mConfirmDialog.show();
		}

	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.emergency_message_view);
		initViews();
		setupViews();
		setEbmsgRead();
	}

	private void setEbmsgRead()
    {
	    mIntent = getIntent();
	    mResolver.updateEmergencyByReadFlag(1,
	            mIntent.getExtras().getInt(INTENT_MESSAGE_NETID),
	            mIntent.getExtras().getInt(INTENT_MESSAGE_NETLEVEL), 
	            mIntent.getExtras().getInt(INTENT_MESSAGE_MSGID),
	            mIntent.getExtras().getString(INTENT_MESSGAE_DATE),
	            mIntent.getExtras().getString(INTENT_MESSGAE_TIME),
	            mIntent.getExtras().getInt(INTENT_MESSGAE_LEVEL));
    }

    @Override
	protected void onDestroy() {
		super.onDestroy();
	}

	protected void onResume() {
		super.onResume();
		updateDate();
		updateViews();
	}

	@Override
	public void initViews() {
		mLevelImageView = (ImageView) findViewById(R.id.iv_emergencylevel);
		mDateTextView = (TextView) findViewById(R.id.tv_emergencydate);
		mTimeTextView = (TextView) findViewById(R.id.tv_emergencytime);
		mMessageTextView = (TextView) findViewById(R.id.tv_emergencymessage);
		mDeleteButton = (Button) findViewById(R.id.bt_delete);
	}

	@Override
	public void setupViews() {
		mDeleteButton.setOnClickListener(this);
	}

	@Override
	public void updateDate() {
		mIntent = getIntent();
		mNetLevel = mIntent.getExtras().getInt(INTENT_MESSAGE_NETLEVEL);
		mNetId = mIntent.getExtras().getInt(INTENT_MESSAGE_NETID);
		mMsgId = mIntent.getExtras().getInt(INTENT_MESSAGE_MSGID);
		mEmergencyDate = mIntent.getExtras().getString(INTENT_MESSGAE_DATE);
		mEmergencyTime = mIntent.getExtras().getString(INTENT_MESSGAE_TIME);
		mMsgLevel = mIntent.getExtras().getInt(INTENT_MESSGAE_LEVEL);
	}

	@Override
    protected void onNewIntent(Intent intent)
    {
        super.onNewIntent(intent);
        setEbmsgRead();
    }

    @Override
	public void updateViews() {
		switch (mIntent.getExtras().getInt(INTENT_MESSGAE_LEVEL)) {
		case 1:
			mLevelImageView.setImageResource(R.drawable.emergency_level_01);
			break;
		case 2:
			mLevelImageView.setImageResource(R.drawable.emergency_level_02);
			break;
		case 3:
			mLevelImageView.setImageResource(R.drawable.emergency_level_03);
			break;
		case 4:
			mLevelImageView.setImageResource(R.drawable.emergency_level_04);
			break;
		}
		mDateTextView.setText(mIntent.getExtras()
				.getString(INTENT_MESSGAE_DATE));
		mTimeTextView.setText(mIntent.getExtras()
				.getString(INTENT_MESSGAE_TIME));
		mMessageTextView.setText(mIntent.getExtras().getString(
				INTENT_MESSGAE_TEXT));
	}

}
