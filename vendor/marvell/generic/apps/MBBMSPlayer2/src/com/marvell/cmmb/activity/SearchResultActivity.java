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
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.AdapterView.OnItemClickListener;

import com.marvell.cmmb.R;
import com.marvell.cmmb.common.AppBase;
import com.marvell.cmmb.service.MBBMSService;
import com.marvell.cmmb.view.adapter.SearchResultAdapter;
import com.marvell.cmmb.view.dialog.ConfirmDialog;
import com.marvell.cmmb.view.dialog.ConfirmDialogCallBack;

public class SearchResultActivity extends AppBase implements
		ConfirmDialogCallBack {
	private TextView mTotalResultTextView;

	private ListView mSearchResultListView;

	private SearchResultAdapter mSearchResultAdapter;

	private ConfirmDialog mConfirmDialog;

	private int mSelectPosition = 1;

	private int channelStatus = -1;

	private int forfreeStatus = -1;

	private int mPosition = 0;

	private static final String TAG = "SearchResultActivity";

	public void confirmDialogCancel(int type) {
		// TODO Auto-generated method stub

	}

	public void confirmDialogOk(int type) {
		if (type == DIALOG_CONFIRM_SUB) {
			Intent subIntent = new Intent();
			subIntent.putExtra("SubscribeStatus", 0);
			subIntent.setClass(SearchResultActivity.this, PurchaseManageActivity.class);
			startActivity(subIntent);
		} else if (type == DIALOG_CONFIRM_PROGRAM_EXPIRE
				|| type == DIALOG_CONFIRM_PROGRAM_FUTURE) {
			Intent viewIntent = new Intent();
			viewIntent.setAction("com.marvell.cmmb.VIEW_SEARCH_ACTION");
			viewIntent.putExtra("Channel", mResolver
					.getChannelNameById(SearchProgramActivity.mScheduleList
							.get(mSelectPosition).getChannelId()));
			startActivity(viewIntent);

		}
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.searchresult);
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

	/**
	 * show confirm dialog ,the message displayed depends on the type.
	 */

	private void showConfirmDialog(int type) {
		String promptMsg = " ";
		String channelName = " ";
		if (type == DIALOG_CONFIRM_SUB) {
			channelName = mResolver.getChannelNameById(mSearchResultAdapter
					.getProgramList().get(mSelectPosition).getChannelId());
			promptMsg = getResources().getString(R.string.promptsubscribestart)
					+ " '" + channelName + "' "
					+ getResources().getString(R.string.promptsubscribeend);

		} else if (type == DIALOG_CONFIRM_PROGRAM_EXPIRE) {
			promptMsg = this.getResources().getString(R.string.promgramexpire);
		} else if (type == DIALOG_CONFIRM_PROGRAM_FUTURE) {
			promptMsg = this.getResources()
					.getString(R.string.promgramnotstart);
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
		mTotalResultTextView = (TextView) findViewById(R.id.tv_total);
		mSearchResultListView = (ListView) findViewById(R.id.lv_searchresult);
	}

	@Override
	public void setupViews() {
		mSearchResultListView.setOnItemClickListener(new OnItemClickListener() {

			public void onItemClick(AdapterView<?> arg0, View arg1, int arg2,
					long arg3) {
				int flag = arg1.getId();
				mSelectPosition = arg2;
				channelStatus = mSearchResultAdapter
						.getChannelStatus(mSelectPosition);
				forfreeStatus = mSearchResultAdapter.isFree(mSelectPosition);
				if (channelStatus == 0 && forfreeStatus == 0
						&& MBBMSService.sCurrentMode == MBBMS_MODE) {
					showConfirmDialog(DIALOG_CONFIRM_SUB);

				} else {
					if (flag == -1) {
						showConfirmDialog(DIALOG_CONFIRM_PROGRAM_EXPIRE);
					} else if (flag == 1) {
						showConfirmDialog(DIALOG_CONFIRM_PROGRAM_FUTURE);
					} else {
						Intent intent = new Intent();
						intent.setAction("com.marvell.cmmb.VIEW_SEARCH_ACTION");
						intent.putExtra("Channel", mResolver.getChannelNameById(SearchProgramActivity
								.mScheduleList.get(arg2).getChannelId()));
						startActivity(intent);
					}
				}
			}
		});
	}

	@Override
	public void updateDate() {
		Intent intent = getIntent();
		mTotalResultTextView.setText(String
				.valueOf(SearchProgramActivity.mScheduleList.size())
				+ this.getResources().getString(R.string.list));
		mSearchResultAdapter = new SearchResultAdapter(this,
				SearchProgramActivity.mScheduleList);
	}

	@Override
	public void updateViews() {
		mSearchResultListView.setAdapter(mSearchResultAdapter);
	}

}
