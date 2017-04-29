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

import java.util.ArrayList;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.AdapterView.OnItemClickListener;

import com.marvell.cmmb.R;
import com.marvell.cmmb.common.AppBase;
import com.marvell.cmmb.manager.MenuManager;
import com.marvell.cmmb.manager.SignalManager;
import com.marvell.cmmb.resolver.EmergencyItem;
import com.marvell.cmmb.view.adapter.EmergencyAdapter;
import com.marvell.cmmb.view.dialog.ConfirmDialog;
import com.marvell.cmmb.view.dialog.ConfirmDialogCallBack;

public class EbMsgManageActivity extends AppBase implements
		ConfirmDialogCallBack {
	private ListView mEmergencyListView;

	private ArrayList<EmergencyItem> mEmergencyList = new ArrayList<EmergencyItem>();

	private LayoutInflater mEmergencyInflater;

	private RelativeLayout mMenuView;

	private EmergencyAdapter mEmergencyAdapter;

	private ConfirmDialog mConfirmDialog = null;
	
	private SignalManager mSignalManager;

	public static EbMsgManageActivity sEmergManage;

	private static final int DELETE_FLAG_FALSE = 0;// emergency message has not been deleted

	private static final int DELETE_FLAG_TRUE = 1;// emergency message has been deleted

	private MenuManager mMenuManager;

	private Handler mEbMsgHandler = new Handler() {

		public void handleMessage(Message msg) {
			switch (msg.what) {
			case DIALOG_CONFIRM_EMERG_DELETE_ALL:
				showConfirmDialog(DIALOG_CONFIRM_EMERG_DELETE_ALL);
				break;
			case DIALOG_CONFIRM_EMERG_DELETE_ITEM:
				showConfirmDialog(DIALOG_CONFIRM_EMERG_DELETE_ITEM);
			default:
				break;
			}

		}
	};

	public void confirmDialogCancel(int type) {

	}

	public void confirmDialogOk(int type) {

		if (type == DIALOG_CONFIRM_EMERG_DELETE_ALL) {
			mResolver.updateEmergency(DELETE_FLAG_TRUE);
		} else {
			mResolver.updateEmergencyById(DELETE_FLAG_TRUE, mEmergencyList.get(
					EmergencyAdapter.sEmergencyPosition).getNetLevel(),
					mEmergencyList.get(EmergencyAdapter.sEmergencyPosition).getNetId(),
					mEmergencyList.get(EmergencyAdapter.sEmergencyPosition).getMsgId(), 
					mEmergencyList.get(EmergencyAdapter.sEmergencyPosition).getEmergencyDate(),
					mEmergencyList.get(EmergencyAdapter.sEmergencyPosition).getEmergencyTime(),
					mEmergencyList.get(EmergencyAdapter.sEmergencyPosition).getEmergencyLevel());
		}
		updateDate();
		updateViews();
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.emergency_manage);
		sEmergManage = this;
		initViews();
		setupViews();
		init();
	}

	public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);
		if (mMenuManager != null) {
			mMenuManager.InflatMenu(R.menu.menu_ebmsg, menu);
		}

		return true;
	}

	public boolean onOptionsItemSelected(MenuItem item) {
		mMenuManager.onMenuClick(item.getItemId());
		return super.onOptionsItemSelected(item);
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
	}

	private void init() {
		mMenuManager = new MenuManager(this);
		if (mMenuManager != null) {
			mMenuManager.setEbMsgHandler(mEbMsgHandler);
		}
		mEmergencyAdapter = new EmergencyAdapter(this, mEmergencyList);
		if (mEmergencyAdapter != null) {
			mEmergencyAdapter.setEbMsgHandler(mEbMsgHandler);
		}
		if(mSignalManager == null) {
			mSignalManager = new SignalManager(this);
		}
		mSignalManager.clearNotification();
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		return super.onKeyDown(keyCode, event);
	}

	@Override
	protected void onResume() {
		super.onResume();
		updateDate();
		updateViews();
	}

	private void showConfirmDialog(int type) {

		String str = "";
		if (mConfirmDialog != null) {
			if (mConfirmDialog.isShowing()) {
				mConfirmDialog.dismiss();

			}
		}

		if (type == DIALOG_CONFIRM_EMERG_DELETE_ALL) {
			str = getResources().getString(R.string.deleteallconfirm);
		} else if (type == DIALOG_CONFIRM_EMERG_DELETE_ITEM) {
			str = getResources().getString(R.string.deleteoneconfirm);
		}

		mConfirmDialog = new ConfirmDialog(this, R.drawable.dialog_hint,
				getResources().getString(R.string.hint), str);
		mConfirmDialog.setCallBack(this, type);
		if (!mConfirmDialog.isShowing()) {
			mConfirmDialog.show();
		}

	}

	@Override
	public void initViews() {
		mEmergencyListView = (ListView) findViewById(R.id.lv_emergency);
	}

	@Override
	public void setupViews() {
		mEmergencyListView.setOnItemClickListener(new OnItemClickListener() {

			public void onItemClick(AdapterView<?> arg0, View arg1, int arg2,
					long arg3) {
				Intent intent = new Intent();
				Bundle bundle = new Bundle();
				bundle.putInt(INTENT_MESSAGE_NETLEVEL, mEmergencyList.get(arg2).getNetLevel());
				bundle.putInt(INTENT_MESSAGE_NETID, mEmergencyList.get(arg2).getNetId());
				bundle.putInt(INTENT_MESSAGE_MSGID, mEmergencyList.get(arg2).getMsgId());
				bundle.putInt(INTENT_MESSGAE_LEVEL, mEmergencyList.get(arg2).getEmergencyLevel());
				bundle.putString(INTENT_MESSGAE_DATE, mEmergencyList.get(arg2).getEmergencyDate());
				bundle.putString(INTENT_MESSGAE_TIME, mEmergencyList.get(arg2).getEmergencyTime());
				bundle.putString(INTENT_MESSGAE_TEXT, mEmergencyList.get(arg2).getEmergencyMessage());
				intent.putExtras(bundle);
				intent.setClass(EbMsgManageActivity.this,EbMsgDetailActivity.class);
				startActivity(intent);
			}
		});
	}

	@Override
	public void updateDate() {
		mEmergencyList = mResolver.emergencyQuery(DELETE_FLAG_FALSE);
		mEmergencyAdapter.setData(mEmergencyList);
	}

	@Override
	public void updateViews() {
		mEmergencyListView.setAdapter(mEmergencyAdapter);
	}

	public Handler getEbMsgHandler() {

		return mEbMsgHandler;

	}

}
