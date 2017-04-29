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

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.AdapterView.OnItemClickListener;

import com.marvell.cmmb.R;
import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.common.AppBase;
import com.marvell.cmmb.common.Define;
import com.marvell.cmmb.common.NotifyDataChangeListener;
import com.marvell.cmmb.common.Util;
import com.marvell.cmmb.manager.MenuManager;
import com.marvell.cmmb.parser.ESGParser;
import com.marvell.cmmb.resolver.ChannelItem;
import com.marvell.cmmb.resolver.ServiceParamItem;
import com.marvell.cmmb.service.MBBMSService;
import com.marvell.cmmb.view.adapter.ChannelListAdatpter;
import com.marvell.cmmb.view.adapter.ServiceparamAdapter;
import com.marvell.cmmb.view.dialog.CancelDialog;
import com.marvell.cmmb.view.dialog.CancelDialogCallback;
import com.marvell.cmmb.view.dialog.ConfirmDialog;
import com.marvell.cmmb.view.dialog.ConfirmDialogCallBack;
import com.marvell.cmmb.view.dialog.PromptDialog;
import com.marvell.cmmb.view.dialog.PromptDialogCallBack;
import com.marvell.cmmb.view.dialog.PromptDialog.PromptDialogKeycallback;

/**
 * @description: to show the local sg and update sg from MBBMS or CMMB
 * 
 */
public class MainActivity extends AppBase implements OnClickListener,
		ConfirmDialogCallBack, CancelDialogCallback, NotifyDataChangeListener, PromptDialogCallBack, PromptDialogKeycallback {

	private static final String TAG = "MainActivity";

	private ListView mChannelListView;

	private ChannelListAdatpter mChannelAdapter;

	private ServiceparamAdapter mServiceparamAdapter;

	private ArrayList<ChannelItem> mServiceList;

	private ArrayList<ServiceParamItem> mServiceParamList;

	private String mClickChannelName;

	private MainReceiver mMainReceiver;

	private ConfirmDialog mConfirmDialog = null;

	private CancelDialog mCancelDialog = null;

	private MenuManager mMenuManager;

	public static MainActivity sInstance;

	protected void onCreate(Bundle savedInstanceState) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onCreate", LogMask.APP_COMMON);
		super.onCreate(savedInstanceState);
		sInstance = this;
		setContentView(R.layout.mainview);
		registerMainReceiver();
		initViews();
		setupViews();
		init();
		showCMMBConfirmDialog();
	}

	private void showCMMBConfirmDialog()
    {
        if(MBBMSService.isModeSeted && MBBMSService.sCurrentMode == CMMB_MODE){
            PromptDialog pd =  new PromptDialog(this,R.drawable.dialog_hint,getResources().getString(R.string.hint),getResources().getString(R.string.cmmbmode));
            pd.setCallBack(this);
            pd.setKeyCallback(this);
            pd.show();
        }
    }

    protected void onDestroy() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onDestroy", LogMask.APP_COMMON);
		sInstance = null;
		super.onDestroy();
		unRegisterMainReceiver();
	}

	protected void onResume() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onResume", LogMask.APP_COMMON);
		super.onResume();
		updateDate();
		updateViews();
		showCancelDialog();
		showSettingChangedDialog();
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_BACK:
			showConfirmDialog(DIALOG_CONFIRM_EXIT);
			return true;
		}
		return super.onKeyDown(keyCode, event);
	}

	public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);
		if (mMenuManager != null) {
			if (MBBMSService.sCurrentMode == Define.CMMB_MODE) {
				mMenuManager.InflatMenu(R.menu.menu_cmmb, menu);
			} else if (MBBMSService.sCurrentMode == Define.MBBMS_MODE) {
				mMenuManager.InflatMenu(R.menu.menu_mbbms, menu);
			}
		}
		return true;
	}

	private void init() {
		mMenuManager = new MenuManager(this);
		if(MBBMSService.sInstance != null) {
			MBBMSService.sInstance.setNotifyDataChangeListener(this);
		}
		if(LoadActivity.sInstance != null) {
			LoadActivity.sInstance.finish();
		}
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		mMenuManager.onMenuClick(item.getItemId());
		return super.onOptionsItemSelected(item);
	}

	public void initViews() {
		mChannelListView = (ListView) findViewById(R.id.lv_channel);
	}

	public void setupViews() {
		mChannelListView.setOnItemClickListener(new OnItemClickListener() {
			public void onItemClick(AdapterView<?> arg0, View arg1, int arg2, long arg3) {
				TextView clickChannel = (TextView) arg1.findViewById(R.id.tv_channel);
				mClickChannelName = clickChannel.getText().toString();
				startPlayingActivity(arg2);

			}
		});
	}

	public void startPlayingActivity(int position) {
		if (MBBMSService.sCurrentMode == MBBMS_MODE) { // under mbbms_mode
			if (!MBBMSService.sInstance.isOperationFinish()) {// wait for the updating to finish
				getShortPromptToast(R.string.waitsync).show();
				return;
			} else {
				if (isChannelEncryptUnsub(position)) {
					showConfirmDialog(DIALOG_CONFIRM_SUBSCRIBE);
					return;
				}
			}

		}
		Intent playIntent = new Intent();
		playIntent.setAction(ACTION_VIEW_PROGRAM_ACTION);
		playIntent.putExtra(INTENT_CHANNEL_POSITION, position);
		startActivity(playIntent);
	}

	public void updateDate() {
		mServiceList = Util.getChannelList(MBBMSService.sCurrentMode, this);
		mChannelAdapter = new ChannelListAdatpter(this, mServiceList);
	}

	public void updateViews() {
		if (mChannelListView != null && mChannelAdapter != null) {
			if (MBBMSService.isUpdatingESG) {
				mChannelListView.setAdapter(mServiceparamAdapter);
			} else {
				mChannelListView.setAdapter(mChannelAdapter);
			}
		}
	}

	private void showCancelDialog() {
		if (MBBMSService.sCurrentMode == CMMB_MODE) {
			if (MBBMSService.isUpdateTS0) {
				if (MBBMSService.isScanCurrentFrequency) {
					showCancelDialog(DIALOG_CANCEL_SCAN);
				}
			} else if (MBBMSService.isUpdatingESG) {
				showCancelDialog(DIALOG_CANCEL_GET_ESG);
			}
		}
	}

	private void showSettingChangedDialog() {
		if (MBBMSService.isCMMBSettingChanged
				&& MBBMSService.sCurrentMode == CMMB_MODE) {
			showConfirmDialog(DIALOG_CONFIRM_SETTING_CHANGE);
			MBBMSService.isCMMBSettingChanged = false;
		}
	}

	public void onClick(View v) {
		// TODO Auto-generated method stub

	}

	/*
	 * to judge whether the channel is encrypted and is not subscribed .
	 */
	private boolean isChannelEncryptUnsub(int channelIndex) {
		int forFreeStatus = mServiceList.get(channelIndex).getFreeState();
		int encryptStatus = mServiceList.get(channelIndex).getEncryptState();
		int freeStatus = mServiceList.get(channelIndex).getForFreeState();
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), 
				"clear status is " + forFreeStatus + " encrypt status is " + encryptStatus + " free status is " + freeStatus, 
				LogMask.APP_COMMON);
		if (forFreeStatus == 0 && encryptStatus == 0 && freeStatus == 0
				&& MBBMSService.sCurrentMode == MBBMS_MODE) {
			return true;
		}
		return false;
	}

	private void showConfirmDialog(int type) {
		String promptMsg = "";
		if (type == DIALOG_CONFIRM_SETTING_CHANGE) {
			promptMsg = this.getResources().getString(R.string.updatescanfrequecy);
		} else if (type == DIALOG_CONFIRM_EXIT) {
			promptMsg = this.getResources().getString(R.string.exitappprompt);
		} else if (type == DIALOG_CONFIRM_SUBSCRIBE) {
			promptMsg = this.getResources().getString(
					R.string.promptsubscribestart)
					+ "'" + mClickChannelName
					+ "'" + this.getResources()
					.getString(R.string.promptsubscribeend);
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

	private void showCancelDialog(int type) {
		String promptMsg = "";
		if (type == DIALOG_CHECK_APN_SETTING) {
			promptMsg = getResources().getString(R.string.connectnetwork);
		} else if (type == DIALOG_CANCEL_SCAN) {
			promptMsg = getResources().getString(R.string.scanning);
		} else if (type == DIALOG_CANCEL_GET_ESG) {
			promptMsg = getResources().getString(R.string.upadateesg);
		}
		if (mCancelDialog != null) {
			mCancelDialog.dismiss();
			mCancelDialog = null;
		}
		mCancelDialog = new CancelDialog(this, getResources().getString(
				R.string.hint), promptMsg);
		mCancelDialog.setCallBack(this, type);
		if (!mCancelDialog.isShowing()) {
			mCancelDialog.show();
		}
	}

	private void dismissCancelDialog() {
		if (mCancelDialog != null && mCancelDialog.isShowing()) {
			mCancelDialog.dismiss();
			mCancelDialog = null;
		}
	}

	private void registerMainReceiver() {
		IntentFilter filter = new IntentFilter();
		filter.addAction(ACTION_ESG_UPDATED);
		filter.addAction(ACTION_ESG_PARSED);
		filter.addAction(ACTION_SCAN_FINISHED);
		filter.addAction(ACTION_SCAN_FAILED);
		filter.addAction(ACTION_SCAN_START);
		filter.addAction(ACTION_SG_UPDATED);
		filter.addAction(ACTION_LOADING_ERROR_QUIT);
		mMainReceiver = new MainReceiver();
		this.registerReceiver(mMainReceiver, filter);
	}

	private void unRegisterMainReceiver() {
		if (mMainReceiver != null) {
			this.unregisterReceiver(mMainReceiver);
			mMainReceiver = null;
		}
	}

	private class MainReceiver extends BroadcastReceiver {

		@Override
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();
			if (action.equals(ACTION_SCAN_START)) {
				showCancelDialog(DIALOG_CANCEL_SCAN);
			} else if (action.equals(ACTION_ESG_UPDATED)) {
				boolean done = intent.getBooleanExtra(ACTION_ESG_UPDATED, false);
				if (done) {
					// getShortPromptToast(R.string.updateesgsuccess).show();
				} else {
					dismissCancelDialog();
					getShortPromptToast(R.string.updateesgfailed).show();
					updateDate();
					updateViews();
				}

			} else if (action.equals(ACTION_SCAN_FINISHED)) {
				boolean result = intent.getBooleanExtra(ACTION_SCAN_FINISHED,
						false);
				if (result) {
					if (MBBMSService.sCurrentMode == MBBMS_MODE) {
						updateDate();
						updateViews();
					} else {
						dismissCancelDialog();
						showScanResultViews();
						showCancelDialog(DIALOG_CANCEL_GET_ESG);
					}
				} else {
					getLongPromptToast(R.string.scanfail).show();
					dismissCancelDialog();
				}

			} else if (action.equals(ACTION_SCAN_FAILED)) {
				getLongPromptToast(R.string.scanfail).show();
				dismissCancelDialog();
				updateDate();
				updateViews();
			} else if (action.equals(ACTION_ESG_PARSED)) {
				dismissCancelDialog();
				boolean done = intent.getBooleanExtra(ACTION_ESG_PARSED, false);
				if (done) {
					getShortPromptToast(R.string.updateesgsuccess).show();
					updateDate();
					updateViews();
				}
			} else if (action.equals(ACTION_SG_UPDATED)) {
				boolean done = intent.getBooleanExtra(ACTION_SG_UPDATED, false);
				if (!done) {// if sg update fail,need to update view.(because of cancel the other operations)
					updateDate();
					updateViews();
				}
			} else if (action.equals(ACTION_LOADING_ERROR_QUIT)) {
				MainActivity.this.finish();
				getShortPromptToast(R.string.starterror).show();
			}
		}

	}

	public void cancelDialogCancelled(int type) {
		if (type == DIALOG_CHECK_APN_SETTING) {
			MBBMSService.sInstance.cancelCheckMode();
		} else if (type == DIALOG_CANCEL_SCAN) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "cancel scan", LogMask.APP_COMMON);
			MBBMSService.sInstance.stopScan();
		} else if (type == DIALOG_CANCEL_GET_ESG) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "cancel get esg", LogMask.APP_COMMON);
			MBBMSService.sInstance.stopUpdateESG();
			ESGParser.getInstance(this).stopParseESG();
		}
	}

	public void confirmDialogCancel(int type) {
		// TODO Auto-generated method stub

	}

	public void confirmDialogOk(int type) {
		if (type == DIALOG_CONFIRM_SETTING_CHANGE) {
			MBBMSService.sInstance.startScan();
		} else if (type == DIALOG_CONFIRM_EXIT) {
			this.finish();
		} else if (type == DIALOG_CONFIRM_SUBSCRIBE) {
			startPurchaseActvity();
		}
	}

	private void showScanResultViews() {
		mServiceParamList = mResolver.getServiceParamByDemod(108);
		if (!mServiceParamList.isEmpty()) {
			mServiceparamAdapter = new ServiceparamAdapter(this, mServiceParamList);
			mChannelListView.setAdapter(mServiceparamAdapter);
		}
	}

	private void startPurchaseActvity() {
		Intent intent = new Intent();
		intent.setClass(this, PurchaseManageActivity.class);
		intent.putExtra("SubscribeStatus", UNPURCHASED);
		this.startActivity(intent);
	}

	public void onDataChanged() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onDataChanged", LogMask.APP_COMMON);
		updateDate();
		updateViews();
	}

    public void promptDialogOk(int type)
    {
        if(MBBMSService.sInstance!=null)
        MBBMSService.sInstance.enterCMMBMode();
    }

    public void promptDialogOnKeyBack(int type)
    {
        if(MBBMSService.sInstance!=null)
            MBBMSService.sInstance.enterCMMBMode();
    }
}
