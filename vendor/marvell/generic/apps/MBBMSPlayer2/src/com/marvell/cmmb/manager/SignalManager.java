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
package com.marvell.cmmb.manager;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.widget.RemoteViews;

import com.marvell.cmmb.R;
import com.marvell.cmmb.activity.EbMsgManageActivity;
import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.aidl.MBBMSEngine;
import com.marvell.cmmb.aidl.SignalStatus;
import com.marvell.cmmb.common.Action;
import com.marvell.cmmb.common.Define;
import com.marvell.cmmb.service.MBBMSService;

/**
 * 
 * Use to control the signal icon in the status bar
 * 
 * @author Danyang Zhou
 * @version [version, 2010-12-3]
 * @see [SignalService]
 */
public class SignalManager {
	private Context mContext;

	private NotificationManager mNM;

	private Notification mNotification;
	
	private Notification mEbmsgNotification;

	private int mSignalWeekFreq = 3;

	public static final int SIGNAL_NOTIFICATIONS = 0x1001;// R.layout.status_bar_notifications;

	public static final int UNREAD_EBMSG_NOTIFICATIONS = 0x1002;
	
	private int[] mIcons = new int[] { R.drawable.signal0, R.drawable.signal1,
			R.drawable.signal2, R.drawable.signal3 };

	private SignalStatus mSignalStatus;

	private RemoteViews mContentView;

	private final String TAG = getClass().getSimpleName();

	private int mPreLevl = -1;

	private int mState;

	public static final int STATE_HUNGUP = 1;

	public static final int STATE_ACTIVE = 2;

	public static final int STATE_EXITED = 3;

	private String mHungup;

	private String mPlaying;

	private String PKGNAME = "com.marvell.cmmb";

	public static final String LAUNCHER = ".activity.LoadActivity";

	public static final String PLAYER = ".activity.PlayingActivity";

	public SignalManager(Context context) {
		this.mContext = context;
		initNotification();
	}

	/**
	 * remove the notification
	 */
	public void clearNotification() {
		deinitNotification();
		mNM.cancel(SIGNAL_NOTIFICATIONS);
		mNM.cancel(UNREAD_EBMSG_NOTIFICATIONS);
	}

	public int getmState() {
		return mState;
	}

	public int getSignalWeekFreq() {
		return mSignalWeekFreq;
	}

	public void hungUp() {
		if (mState != STATE_HUNGUP) {
			mPreLevl = -1;
			mState = STATE_HUNGUP;
			mContentView.setTextViewText(R.id.text, mHungup);
			setRemoteActivity(LAUNCHER);
			showNotification(R.drawable.hungup);
		}
	}

	public void active() {
		if (mState != STATE_ACTIVE) {
			mPreLevl = -1;
			mState = STATE_ACTIVE;
			mContentView.setTextViewText(R.id.text, mPlaying);
			setRemoteActivity(PLAYER);
			setSignalIcon();
		}
	}

	private void initNotification() {
		mState = STATE_EXITED;
		mHungup = mContext.getResources().getString(R.string.hungup);
		mPlaying = mContext.getResources().getString(R.string.signalPrompt);
		if (null == mNM) {
			mNM = (NotificationManager) mContext
					.getSystemService(Context.NOTIFICATION_SERVICE);
		}

		if (null == mNotification) {
			mNotification = new Notification();
			mNotification.flags = Notification.FLAG_NO_CLEAR;
		}
		setRemoteActivity(LAUNCHER);
		setContentView();
	}

	private void deinitNotification() {
		mState = STATE_EXITED;
	}

	public void refreshNotification() {
		mNM.notify(SIGNAL_NOTIFICATIONS, mNotification);
	}

	private void setContentView() {
		mContentView = new RemoteViews(mContext.getPackageName(),
				R.layout.status_bar_signal);
		mContentView.setTextViewText(R.id.text, mPlaying);
		mContentView.setImageViewResource(R.id.icon, R.drawable.icon_tvplayer);
		mNotification.contentView = mContentView;
	}

	public void setmState(int mState) {
		this.mState = mState;
	}

	/*
	 * set the activity to go when click the item in the notification area.
	 */
	public void setRemoteActivity(String launcher) {
		ComponentName comp = new ComponentName(PKGNAME, PKGNAME + launcher);
		Intent notificationIntent = new Intent();
		notificationIntent.setComponent(comp);
		if (launcher.equals(LAUNCHER)) {
			notificationIntent.addCategory("android.intent.category.LAUNCHER");
			notificationIntent.setAction("android.intent.action.MAIN");
			notificationIntent.setFlags(0x10200000);
		} else {
			notificationIntent.putExtra(Define.INTENT_CHANNEL_POSITION,
					MBBMSService.sCurrentPlayPosition);
			notificationIntent.setAction(Action.ACTION_VIEW_PROGRAM_ACTION);
		}
		mNotification.contentIntent = PendingIntent.getActivity(mContext, 0,
				notificationIntent, PendingIntent.FLAG_UPDATE_CURRENT);
	}

	/**
	 * Set the icon to the status bar by the signal strength get from
	 * mbbmsengine.
	 * 
	 */
	public void setSignalIcon() {
		if (mSignalStatus != null) {
			setSignalLevel(getSignalIconLevel(mSignalStatus.getLevel()));
			LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(),
					"signalstatus strenth is" + mSignalStatus.getStrength(),
					LogMask.APP_NETWORK);
		} else {
			setSignalLevel(0);
			LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(), "mSignalStatus is null",
					LogMask.APP_NETWORK);
		}
	}

	private int getSignalIconLevel(int level) {
		int iconLevel = 0;
		switch (level) {
		case 0:
			iconLevel = 0;
			break;
		case 1:
		case 2:
			iconLevel = 1;
			break;
		case 3:
		case 4:
			iconLevel = 2;
			break;
		case 5:
			iconLevel = 3;
			break;
		default:
			iconLevel = 0;
			break;
		}
		return iconLevel;
	}

	public void setSignalIconByStrength(SignalStatus status) {
		mSignalStatus = status;
		if (mState == STATE_ACTIVE) {
			setSignalIcon();
		}
	}

	/**
	 * set the signal level by default icons
	 * 
	 * @param level
	 *            from 0-3,0 no signal,3 full signal.
	 */
	public void setSignalLevel(int level) {
		if (level != mPreLevl) {
			mPreLevl = level;
		} else {
			return;
		}

		if (level >= 0 && level < mIcons.length) {
			showNotification(mIcons[level]);
		} else {
			// set signal error
			showNotification(R.drawable.hungup);
		}
	}
 
	/**
	 * set the icons that you want to replace with the default
	 * 
	 * @param icons
	 *            the source icon id
	 * 
	 */
	public void setSourceIcons(int[] icons) {
		mIcons = icons;
	}

	/**
	 * set the icon to the status bar directly
	 * 
	 * @param moodId
	 *            the icon id
	 * 
	 */
	public void showNotification(int moodId) {
		mNotification.icon = moodId;
		refreshNotification();
	}
	
	public void showUnReadEbmsgNot(int total){
	    if(mEbmsgNotification==null){
	        mEbmsgNotification = new Notification();
	        mEbmsgNotification.icon = R.drawable.icon_tvplayer;
	        Intent it = new Intent(mContext,EbMsgManageActivity.class);
	        mEbmsgNotification.contentIntent = PendingIntent.getActivity(mContext, 0,
	                it, PendingIntent.FLAG_UPDATE_CURRENT);
	        mEbmsgNotification.contentView = new RemoteViews(mContext.getPackageName(), R.layout.ebmsg_notification);
	        mEbmsgNotification.flags = Notification.FLAG_AUTO_CANCEL;
	    }
	    mEbmsgNotification.number = total;
	    mEbmsgNotification.contentView.setTextViewText(R.id.ebmsg, mContext.getString(R.string.uhave_unreadebmsg, total));
//	    mEbmsgNotification.tickerText = mContext.getString(R.string.uhave_unreadebmsg, total);
	    mNM.notify(UNREAD_EBMSG_NOTIFICATIONS, mEbmsgNotification);
	}

	public void cancelUnReadEbmsgNot(){
	    mNM.cancel(UNREAD_EBMSG_NOTIFICATIONS);
	}
	
}
