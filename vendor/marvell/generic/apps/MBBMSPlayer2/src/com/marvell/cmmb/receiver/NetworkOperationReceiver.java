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

package com.marvell.cmmb.receiver;

import com.marvell.cmmb.R;
import com.marvell.cmmb.activity.MainActivity;
import com.marvell.cmmb.common.Action;
import com.marvell.cmmb.common.Define;
import com.marvell.cmmb.common.Util;
import com.marvell.cmmb.manager.ModeManager;
import com.marvell.cmmb.parser.ESGParser;
import com.marvell.cmmb.service.MBBMSService;
import com.marvell.cmmb.view.dialog.CancelDialog;
import com.marvell.cmmb.view.dialog.CancelDialogCallback;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.widget.Toast;

public class NetworkOperationReceiver extends BroadcastReceiver implements Action {
	private Context mContext;
	private Toast mShortPromptToast;
	private Toast mLongPromptToast;
	private static final String TAG = "NetworkOperationReceiver";

	@Override
	public void onReceive(Context context, Intent intent) {
		mContext = context;
		String action = intent.getAction();
		if (action.equals(ACTION_SG_UPDATING)) {
			getShortPromptToast(R.string.refreshsg).show();
		} else if (action.equals(ACTION_SG_UPDATED)) {
			boolean done = intent.getBooleanExtra(ACTION_SG_UPDATED, false);
			int resCode = intent.getIntExtra("resCode", 0);
			String errorInfo = Util.getErrorCodeInfo(mContext, resCode);
			if (done) {
				if (MBBMSService.isTestMode) {
					getShortPromptToast(R.string.updatesgtestsuccess).show();
				}
			} else {
				if(errorInfo.equals("")) {
					if (MBBMSService.isTestMode) {
						getShortPromptToast(R.string.updatesgtestfail).show();
					} else {
						getShortPromptToast(R.string.updatesgfail).show();
					}
				} else {
					getShortPromptToast(errorInfo).show();
				}
			}
		} else if (action.equals(ACTION_PARSESG_FINISHED)
				|| action.equals(ACTION_PARSEST_FINISHED)) {
			refreshData();
		} else if (action.equals(ACTION_ST_UPDATED)) {
			boolean done = intent.getBooleanExtra(ACTION_ST_UPDATED, false);
			int resCode = intent.getIntExtra("resCode", 0);
			String errorInfo = Util.getErrorCodeInfo(mContext, resCode);
			if (done) {
				if (MBBMSService.isTestMode) {
					getShortPromptToast(R.string.updatesttestsuccess).show();
				}
			} else {
				if(errorInfo.equals("")) {
					if (MBBMSService.isTestMode) {
						getShortPromptToast(mContext.getResources().getString(R.string.updatesttestfail)).show();
					} else {
						getShortPromptToast(mContext.getResources().getString(R.string.updatestfail)).show();
					}
				} else {
					getShortPromptToast(errorInfo).show();
				}
			}
		} else if (action.equals(ACTION_GBA_INIT)) {
			MBBMSService.sInstance.printAllOperate();
			boolean done = intent.getBooleanExtra(ACTION_GBA_INIT, false);
			int resCode = intent.getIntExtra("resCode", 0);
			String errorInfo = Util.getErrorCodeInfo(mContext, resCode);
			if (done) {
				if (MBBMSService.isTestMode) {
					getShortPromptToast(R.string.initgbatestsuccess).show();
				} else {
					getShortPromptToast(R.string.updatesgsuccess).show();
				}
			} else {
				if(errorInfo.equals("")) {
					if (MBBMSService.isTestMode) {
						getShortPromptToast(mContext.getResources().getString(R.string.initgbatestfail)).show();
					} else {
						getShortPromptToast(mContext.getResources().getString(R.string.initgbafailed)).show();
					}
				} else {
					getShortPromptToast(errorInfo).show();
				}				
			}
		} else if (action.equals(ACTION_INQUIRIED)) {
			boolean done = intent.getBooleanExtra(ACTION_INQUIRIED, false);
			int resCode = intent.getIntExtra("resCode", 0);
			String errorInfo = Util.getErrorCodeInfo(mContext, resCode);
			if (done) {
				getShortPromptToast(R.string.inquirysuccess).show();
			} else {
				if(errorInfo.equals("")) {
					getShortPromptToast(mContext.getResources().getString(R.string.inquiryfailed)).show();
				} else {
					getShortPromptToast(errorInfo).show();
				}				
			}
		} else if (action.equals(ACTION_NETWORKCONNECT_FAILED)) {
			getShortPromptToast(R.string.connectfail).show();
		}
	}

	private Toast getShortPromptToast(int resid) {
		if (mShortPromptToast == null) {
			mShortPromptToast = Toast.makeText(mContext, resid,
					Toast.LENGTH_SHORT);
		}
		mShortPromptToast.setText(resid);
		return mShortPromptToast;
	}
	
	private Toast getShortPromptToast(String text) {
		if (mShortPromptToast == null) {
			mShortPromptToast = Toast.makeText(mContext, text, Toast.LENGTH_SHORT);
		}
		mShortPromptToast.setText(text);
		return mShortPromptToast;
	}

	private void refreshData() {
		if (MainActivity.sInstance == null)
			return;
		MainActivity.sInstance.updateDate();
		MainActivity.sInstance.updateViews();

	}

	public Toast getLongPromptToast(int resid) {
		if (mLongPromptToast == null) {
			mLongPromptToast = Toast.makeText(mContext, resid,
					Toast.LENGTH_LONG);
		}
		mLongPromptToast.setText(resid);
		return mLongPromptToast;
	}

}
