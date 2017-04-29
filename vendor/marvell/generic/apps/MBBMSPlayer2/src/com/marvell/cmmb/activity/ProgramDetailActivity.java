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

import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.InetSocketAddress;
import java.net.Proxy;
import java.net.URL;
import java.util.Calendar;

import org.apache.http.HttpStatus;

import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Base64;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.TextView;

import com.marvell.cmmb.R;
import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.common.AppBase;
import com.marvell.cmmb.common.Util;
import com.marvell.cmmb.manager.ModeManager;
import com.marvell.cmmb.resolver.PreviewDataItem;
import com.marvell.cmmb.service.MBBMSService;
import com.marvell.cmmb.view.dialog.ConfirmDialog;
import com.marvell.cmmb.view.dialog.ConfirmDialogCallBack;

public class ProgramDetailActivity extends AppBase implements
		ConfirmDialogCallBack {
	private TextView mDateTextView;

	private TextView mDayTextView;

	private TextView mPlayTimeTextView;

	private TextView mProgramNameTextView;

	private TextView mInfoDetailTextView;

	private ImageButton mDetailImageButton;

	private Button mViewButton;

	private String mChooseDay;

	private String mChooseDate;

	private String mPlaybackTime;

	private String mPlaybackProgram;

	private String mChooseChannel;

	private String mDescription;

	private long mPreviewId;

	private int mFlag;

	private int mChannelStatus = -1;

	private int isForfree = -1;
	
	private int mFreeStatus = -1;

	private ConfirmDialog mConfirmDialog = null;

	private static final String TAG = "ProgramDetail";

	private static final int CONNECTION_TIMEOUT = 500;

	public void confirmDialogCancel(int type) {
		// TODO Auto-generated method stub

	}

	public void confirmDialogOk(int type) {
		if (type == DIALOG_CONFIRM_PROGRAM_EXPIRE
				|| type == DIALOG_CONFIRM_PROGRAM_FUTURE) {
			Intent viewIntent = new Intent();
			viewIntent.setAction("com.marvell.cmmb.VIEW_DETAIL_ACTION");
			viewIntent.putExtra("ChooseChannel", mChooseChannel);
			startActivity(viewIntent);
			ProgramDetailActivity.this.finish();
		} else if (type == DIALOG_CONFIRM_SUB) {
			Intent subIntent = new Intent();
			subIntent.putExtra("SubscribeStatus", 0);
			subIntent.setClass(ProgramDetailActivity.this, PurchaseManageActivity.class);
			startActivity(subIntent);
		}

	}

	private String getChooseDate(Context context, String chooseDay) {
		String chooseDate = "";
		Calendar calendar = Calendar.getInstance();
		if (chooseDay.equals(context.getResources().getString(R.string.today))) {
			chooseDate = Util.sDateFormater.format(calendar.getTime());
		} else if (chooseDay.equals(context.getResources().getString(R.string.tomorrow))) {
			calendar.add(Calendar.DATE, 1);
			chooseDate = Util.sDateFormater.format(calendar.getTime());
		} else if (chooseDay.equals(context.getResources().getString(R.string.aftertomorrow))) {
			calendar.add(Calendar.DATE, 2);
			chooseDate = Util.sDateFormater.format(calendar.getTime());
		} else {

		}
		return chooseDate;
	}

	private void getInfomation() {
		Intent intent = getIntent();
		mChooseDay = intent.getExtras().getString("ChooseDay");
		mChooseDate = getChooseDate(this, mChooseDay);
		mChooseChannel = intent.getExtras().getString("ChooseChannel");
		try {
			mPlaybackTime = intent.getExtras().getString("StartTime").substring(11, 16)
					+ "-" + intent.getExtras().getString("EndTime").substring(11, 16);
		} catch (Exception e) {
			mPlaybackTime = "";
		}
		mDescription = intent.getExtras().getString("Description");
		mPlaybackProgram = intent.getExtras().getString("PlaybackProgram");
		mFlag = intent.getExtras().getInt("ViewFlag");
		mChannelStatus = intent.getExtras().getInt("ChannelStatus");
		isForfree = intent.getExtras().getInt("ForFree");
		mFreeStatus = intent.getExtras().getInt("FreeStatus");
		mPreviewId = intent.getExtras().getLong("PreviewId");
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.program_detail);
		initViews();
		setupViews();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
	}

	@Override
	protected void onPause() {
		super.onPause();

	}

	@Override
	protected void onResume() {
		super.onResume();
		updateDate();
		updateViews();
	}

	private Bitmap post(String pictureUri) {
		Bitmap bt = null;
		try {
			URL imageURL = new URL(pictureUri);
			Proxy proxy = new Proxy(Proxy.Type.HTTP, new InetSocketAddress(
					ModeManager.getInstance(this).getProxy(), ModeManager.getInstance(this).getPort()));
			HttpURLConnection conn = (HttpURLConnection) imageURL.openConnection(proxy);
			conn.setDoInput(true);
			conn.connect();
			int responseCode = conn.getResponseCode();
			if (responseCode == HttpStatus.SC_OK) {
				InputStream is = conn.getInputStream();
				BitmapFactory.Options ops = new BitmapFactory.Options();
				ops.inJustDecodeBounds = true;
				bt = BitmapFactory.decodeStream(is);
				is.close();
			}
			conn.disconnect();
		} catch (Exception e) {
			e.printStackTrace();
		}
		return bt;
	}

	private void setImage() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "setImage() called", LogMask.APP_COMMON);
		PreviewDataItem previewdata = mResolver.getPreviewData(mPreviewId);
		if (previewdata != null) {
			if (previewdata.getPictureType() == 1) {
				Bitmap bt = post(previewdata.getPictureUri());
				mDetailImageButton.setImageBitmap(bt);
			} else if (previewdata.getPictureType() == 2) {
				byte[] b = Base64.decode(previewdata.getPictureUri(), Base64.DEFAULT);
				if (b.length != 0) {
					mDetailImageButton.setImageBitmap(BitmapFactory
							.decodeByteArray(b, 0, b.length));
				}
			} else {
				mDetailImageButton.setImageResource(R.drawable.detail_content);
			}
		} else {
			mDetailImageButton.setImageResource(R.drawable.detail_content);
		}
	}

	/**
	 * show confirm dialog ,the message displayed depends on the type.
	 */

	private void showConfirmDialog(int type) {
		String promptMsg = " ";
		String channelName = " ";
		if (type == DIALOG_CONFIRM_SUB) {
			promptMsg = getResources().getString(R.string.promptsubscribestart)
					+ " '" + mChooseChannel + "' "
					+ getResources().getString(R.string.promptsubscribeend);
		} else if (type == DIALOG_CONFIRM_PROGRAM_EXPIRE) {
			promptMsg = this.getResources().getString(R.string.promgramexpire);
		} else if (type == DIALOG_CONFIRM_PROGRAM_FUTURE) {
			promptMsg = this.getResources().getString(R.string.promgramnotstart);
		}
		if (mConfirmDialog != null) {
			mConfirmDialog.dismiss();
			mConfirmDialog = null;
		}
		mConfirmDialog = new ConfirmDialog(this, R.drawable.dialog_hint,
				getResources().getString(R.string.hint), promptMsg);
		mConfirmDialog.setCallBack(this, type);
		if (!mConfirmDialog.isShowing()) {
			mConfirmDialog.show();
		}

	}

	@Override
	public void initViews() {
		mDateTextView = (TextView) findViewById(R.id.tv_date);
		mDayTextView = (TextView) findViewById(R.id.tv_day);
		mPlayTimeTextView = (TextView) findViewById(R.id.tv_playbacktime);
		mProgramNameTextView = (TextView) findViewById(R.id.tv_playbackprogram);
		mInfoDetailTextView = (TextView) findViewById(R.id.tv_infodetail);
		mDetailImageButton = (ImageButton) findViewById(R.id.iv_detail);
		mViewButton = (Button) findViewById(R.id.bt_viewprogram);
	}

	@Override
	public void setupViews() {
		mViewButton.setOnClickListener(new OnClickListener() {

			public void onClick(View v) {
				if (mChannelStatus == 0 && isForfree == 0 && mFreeStatus == 0
						&& MBBMSService.sCurrentMode == MBBMS_MODE) {
					showConfirmDialog(DIALOG_CONFIRM_SUB);
				} else {
					if (mFlag == -1) {
						showConfirmDialog(DIALOG_CONFIRM_PROGRAM_EXPIRE);

					} else if (mFlag == 1) {
						showConfirmDialog(DIALOG_CONFIRM_PROGRAM_FUTURE);
					} else {
						Intent intent = new Intent();
						intent.setAction("com.marvell.cmmb.VIEW_DETAIL_ACTION");
						intent.putExtra("ChooseChannel", mChooseChannel);
						startActivity(intent);
						ProgramDetailActivity.this.finish();
					}
				}
			}
		});
	}

	@Override
	public void updateDate() {
		getInfomation();
	}

	@Override
	public void updateViews() {
		mDayTextView.setText(mChooseDay);
		mDateTextView.setText(mChooseDate);
		mPlayTimeTextView.setText(mPlaybackTime);
		mProgramNameTextView.setText(mPlaybackProgram);
		mInfoDetailTextView.setText(mDescription);
		setImage();
	}

}
