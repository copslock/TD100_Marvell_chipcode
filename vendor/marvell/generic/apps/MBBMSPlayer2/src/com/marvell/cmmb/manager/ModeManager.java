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

import java.net.InetAddress;
import java.net.UnknownHostException;

import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.os.Handler;
import android.os.Message;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;

import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.common.Action;
import com.marvell.cmmb.common.Define;
import com.marvell.cmmb.common.Util;
import com.marvell.cmmb.service.MBBMSService;
import com.android.internal.telephony.Phone;
import com.android.common.NetworkConnectivityListener;

/**
 * 
 * To manager the mbbms mode and cmmb mode
 * 
 * @author Danyang Zhou
 * 
 * @version 2010-01-14
 * 
 */
public class ModeManager implements Action, Define {

	private int mConnectivityResult = -1;

	private ConnectivityManager mConnectivityManager;

	private NetworkConnectivityListener mConnectivityListener;

	private boolean isSimCardIn = false;

	private int mOperationType;

	public static class APNNet {
		public static String CMWAP = "cmwap";

		public static String CMNET = "cmnet";
	}

	/**
	 * called when get the sim card state or networkstate
	 * 
	 * @author Danyang Zhou
	 * @version 2010-01-14
	 */
	public interface ModeGetListener {

		public void onAPNReset();

		public void onSetMode(int mode);

		public void onNetworkConnected();

		public void onNetworkDisconnected();

		public void onSimReady(boolean isSimReady);

		public void onModeExchange();
	}

	private PhoneStateListener mPhoneStateListener;

	private Context mContext;

	private ModeGetListener mModeGetListener;

	private static final String TAG = "ModeManager";

	private String mCurrentAPN = "";

	private String mCurrentProxy = "";

	private String mCurrentPort = "";

	private static ModeManager sInstance;

	private static final int APN_EXTENSION_WAIT = 30 * 1000;

	public static final int NETWORK_EXTENDION_WAIT = 5 * 1000;

	public static ModeManager getInstance(Context context) {
		if (sInstance == null) {
			sInstance = new ModeManager(context);
		}
		return sInstance;
	}

	public Handler getNetworkHandler() {
		return mNetworkListenHandler;
	}

	public int getOperationType() {
		return mOperationType;
	}

	public void setOperationType(int operationType) {
		mOperationType = operationType;
	}

	private Handler mNetworkListenHandler = new Handler() {

		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case MESSAGE_DATA_STATE_CHANGED:// send this message when network
				// state changed
				LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(),
						"data state changed", LogMask.APP_NETWORK);
			case MESSAGE_NETWORK_CHECK:
				LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(), "network state is "
						+ mConnectivityListener.getNetworkInfo().getState() + " network type is"
						+ mConnectivityListener.getNetworkInfo().getTypeName(),
						LogMask.APP_NETWORK);
				LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(),
						"apn connected time is " + System.currentTimeMillis(), LogMask.APP_NETWORK);
				if (mConnectivityListener.getNetworkInfo().getState().toString().equals("CONNECTED")) {
					if (mConnectivityListener.getNetworkInfo().getTypeName().equals("mobile_wap")) {
						if (mNetworkListenHandler.hasMessages(MESSAGE_WAIT_NETWORK_RESULT)) {
							mNetworkListenHandler.removeMessages(MESSAGE_WAIT_NETWORK_RESULT);
						}
						LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(),
								"operation type is " + mOperationType,
								LogMask.APP_NETWORK);
						if (mOperationType == STARTUP) {
							mModeGetListener.onSetMode(MBBMS_MODE);
						} else {
							mModeGetListener.onNetworkConnected();
						}

					} else {
						LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(),
								"network connected but not cmwap",
								LogMask.APP_NETWORK);
					}
				} else {
					LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(),
							"network disconnected", LogMask.APP_NETWORK);
				}
				break;
			case MESSAGE_DATA_STATE_CHANGED_MODEEXCHANGE:
				if (mConnectivityListener.getNetworkInfo().getState().toString().equals(
						"CONNECTED")) {
					if (mConnectivityListener.getNetworkInfo().getTypeName()
							.equals("mobile_wap")) {
						if (mNetworkListenHandler.hasMessages(MESSAGE_WAIT_NETWORK_RESULT)) {
							mNetworkListenHandler.removeMessages(MESSAGE_WAIT_NETWORK_RESULT);
						}
						if (mNetworkListenHandler.hasMessages(MESSAGE_MODE_CHECKING)) {
							mNetworkListenHandler.removeMessages(MESSAGE_MODE_CHECKING);
						}
						mModeGetListener.onModeExchange();
					}
				}
				break;
			case MESSAGE_CONTINUE_WAP_CONNECTIVITY:
				continueWapConnectivity();
				break;
			case MESSAGE_MODE_CHECKING:
				connectNetworkforModeExchange(MESSAGE_DATA_STATE_CHANGED_MODEEXCHANGE);
				break;
			case MESSAGE_WAIT_NETWORK_RESULT:
				LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(),
						"start using network timeout", LogMask.APP_NETWORK);
				clearOperateMap();
				stopWapConnectivity();
				if (mOperationType == STARTUP) {
					mModeGetListener.onSetMode(CMMB_MODE);
				} else {
					mModeGetListener.onNetworkDisconnected();
				}
				break;
			}
			super.handleMessage(msg);
		}

	};

	public ModeManager(Context context) {
		mContext = context;
		sInstance = this;
		mConnectivityManager = (ConnectivityManager) mContext
				.getSystemService(Context.CONNECTIVITY_SERVICE);
		mConnectivityListener = new NetworkConnectivityListener();
		mConnectivityListener.startListening(mContext);
	}

	public String getApn() {
		return mCurrentAPN;
	}

	private boolean isAPNTypeWap() {
		Uri uri = Uri.parse("content://telephony/carriers");
		Cursor cr = mContext.getContentResolver().query(uri, null,
				"type like '%wap%' and current = 1", null, null);
		if (cr != null && cr.getCount() != 0) {
			int cursorCount = 0;
			do {
				cr.moveToNext();
				cursorCount = cursorCount + 1;
				if(isWAPType(cr.getString(cr.getColumnIndex("type")))) {
					LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(), "apn type is wap",
							LogMask.APP_NETWORK);
					return true;
				}
			} while(cursorCount < cr.getCount());
		}
		LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(), "apn type is not wap",
				LogMask.APP_NETWORK);
		cr.close();
		return false;
	}

	public boolean isNetworkConnected() {
		if (mConnectivityListener != null) {
			if(mConnectivityListener.getNetworkInfo() != null) {
				if (mConnectivityListener.getNetworkInfo().getState().toString().equals("CONNECTED")
						&& mConnectivityListener.getNetworkInfo().getTypeName().equals("mobile_wap")) {
					return true;
				} else {
					return false;
				}
			} else {
				LogMask.LOGM(TAG, LogMask.LOG_APP_ERROR, LogMask.getLineInfo(), "network info is null",
						LogMask.APP_ERROR);
				return false;
			}
		}
		LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(), "network connectivity listener is null",
				LogMask.APP_NETWORK);
		return false;
	}

	public void getCurrentAPN() {
		Uri uri = Uri.parse("content://telephony/carriers");
		Cursor cr = mContext.getContentResolver().query(uri, null,
				"type like '%wap%' and current = 1", null, null);
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "wap apn count is "
				+ cr.getCount(), LogMask.APP_COMMON);
		if (cr != null && cr.getCount() != 0) {
			int cursorCount = 0;
			do {
				cr.moveToNext();
				cursorCount = cursorCount + 1;
			} while (!isWAPType(cr.getString(cr.getColumnIndex("type")))&& cursorCount < cr.getCount());
			LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(), "wap apn type is "
					+ cr.getString(cr.getColumnIndex("type")), LogMask.APP_NETWORK);
			mCurrentAPN = cr.getString(cr.getColumnIndex("apn"));
			mCurrentProxy = cr.getString(cr.getColumnIndex("proxy"));
			mCurrentPort = cr.getString(cr.getColumnIndex("port"));
		}
		cr.close();
		LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(), "current apn is "
				+ mCurrentAPN, LogMask.APP_NETWORK);
	}

	public void setMode() {
		if (getNetWorkReachable()) {
			if (isAPNTypeWap()) {
				broadcastNetConnect();
				mConnectivityResult = startWapConnectivity();
				registerModeSetListener(MESSAGE_DATA_STATE_CHANGED);
				if (mConnectivityResult == Phone.APN_ALREADY_ACTIVE) {
					LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(),
							"apn already active", LogMask.APP_NETWORK);
					mModeGetListener.onSetMode(MBBMS_MODE);
					unregisterListener();
				} else if (mConnectivityResult == Phone.APN_REQUEST_STARTED) {
					LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(),
							"apn request started", LogMask.APP_NETWORK);
					LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(),
							"apn request start time is " + System.currentTimeMillis(), LogMask.APP_NETWORK);
					mNetworkListenHandler.sendEmptyMessageDelayed(
									MESSAGE_WAIT_NETWORK_RESULT,
									NETWORK_EXTENDION_WAIT);
				}
			} else {
				LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(),
						"apn type is not wap, need to reset",
						LogMask.APP_NETWORK);
				mModeGetListener.onAPNReset();
			}
		} else {
			LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(),
					"network is not reachable", LogMask.APP_NETWORK);
			mModeGetListener.onSetMode(CMMB_MODE);
		}
	}

	public boolean startUsingNetwork(int reason) {
		if (getNetWorkReachable() && isAPNTypeWap()) {
			mConnectivityResult = startWapConnectivity();
			registerModeSetListener(reason);
			if (mConnectivityResult == Phone.APN_ALREADY_ACTIVE) {
				LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(),
						"apn already active", LogMask.APP_NETWORK);
				mModeGetListener.onNetworkConnected();
			} else if (mConnectivityResult == Phone.APN_REQUEST_STARTED) {
				LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(),
						"apn request start time is " + System.currentTimeMillis(), LogMask.APP_NETWORK);
				mNetworkListenHandler.sendEmptyMessageDelayed(
						MESSAGE_WAIT_NETWORK_RESULT, NETWORK_EXTENDION_WAIT);
			}
			return true;
		}
		return false;
	}

	public void connectNetworkforModeExchange(int reason) {
		mNetworkListenHandler.sendEmptyMessageDelayed(MESSAGE_MODE_CHECKING,
				APN_EXTENSION_WAIT);
		if (getNetWorkReachable() && isAPNTypeWap()) {
			mConnectivityResult = startWapConnectivity();
			registerModeSetListener(reason);
		}
	}

	public void stopModeListen() {
		if (mNetworkListenHandler.hasMessages(MESSAGE_MODE_CHECKING)) {
			mNetworkListenHandler.removeMessages(MESSAGE_MODE_CHECKING);
		}
		unregisterListener();
	}

	public void continueWapConnectivity() {
		mConnectivityResult = startWapConnectivity();
		if (mConnectivityResult == Phone.APN_ALREADY_ACTIVE) {
			mNetworkListenHandler.sendMessageDelayed(mNetworkListenHandler
					.obtainMessage(MESSAGE_CONTINUE_WAP_CONNECTIVITY),
					APN_EXTENSION_WAIT);
		}
	}

	public int startWapConnectivity() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "start using wap network",
				LogMask.APP_MAIN);
		int result = mConnectivityManager.startUsingNetworkFeature(
				ConnectivityManager.TYPE_MOBILE, Phone.FEATURE_ENABLE_WAP);
		return result;
	}

	public void stopWapConnectivity() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "stop using wap network",
				LogMask.APP_MAIN);
		if (mNetworkListenHandler.hasMessages(MESSAGE_CONTINUE_WAP_CONNECTIVITY)) {
			mNetworkListenHandler.removeMessages(MESSAGE_CONTINUE_WAP_CONNECTIVITY);
		}
		unregisterListener();
	}

	private void registerModeSetListener(int reason) {
		if (mConnectivityListener == null) {
			mConnectivityListener = new NetworkConnectivityListener();
			mConnectivityListener.startListening(mContext);
		}
		mConnectivityListener.registerHandler(mNetworkListenHandler, reason);
	}

	private void unregisterListener() {
		if (mConnectivityListener != null) {
			mConnectivityListener.unregisterHandler(mNetworkListenHandler);
		}
	}

	/**
	 * get the call back
	 * 
	 * @return the call back where is seted
	 */
	public ModeGetListener getModeGetListener() {
		return mModeGetListener;
	}

	/**
	 * get the networkinfo,if has apn or is available
	 */
	public boolean getNetWorkReachable() {
		NetworkInfo networkinfo = mConnectivityManager
				.getNetworkInfo(ConnectivityManager.TYPE_MOBILE);
		if (networkinfo != null && networkinfo.isAvailable()) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(),
					"mobile network is available", LogMask.APP_NETWORK);
			return true;
		}
		LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(),
				"mobile network is " + networkinfo, LogMask.APP_NETWORK);
		LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(),
				"mobile network is not available", LogMask.APP_NETWORK);
		return false;

	}

	/**
	 * get the phone's simcard and signal state
	 * 
	 */
	public void checkSimcardIn() {
		TelephonyManager tm = ((TelephonyManager) mContext
				.getSystemService(Context.TELEPHONY_SERVICE));
		if (TelephonyManager.SIM_STATE_READY == tm.getSimState()) {
			isSimCardIn = true;
			LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(), "sim card is ready",
					LogMask.APP_NETWORK);
		} else if (TelephonyManager.SIM_STATE_ABSENT == tm.getSimState()) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(),
					"sim card is out of state", LogMask.APP_NETWORK);
			isSimCardIn = false;
		} else {
			LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(),
					"sim card state is not SIM_STATE_READY & SIM_STATE_ABSENT",
					LogMask.APP_NETWORK);
		}
		if (null != mModeGetListener) {
			mModeGetListener.onSimReady(isSimCardIn);
		}
	}

	public int getPort() {
		if (mCurrentPort.trim().equals("")) {
			return 0;
		} else {
			return Integer.parseInt(mCurrentPort);
		}
	}

	public String getProxy() {
		return mCurrentProxy;
	}

	public boolean isAPNWAP() {
		getCurrentAPN();
		if (mCurrentAPN.startsWith(APNNet.CMWAP)
				|| mCurrentAPN.startsWith("labwap3")) {
			return true;
		}
		return false;
	}

	/**
	 * set the call back
	 * 
	 * @param mModeGetListener
	 */
	public void setModeGetListener(ModeGetListener mModeGetListener) {
		this.mModeGetListener = mModeGetListener;
	}

	public void deinit() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(), "ModeManager deinit",
				LogMask.APP_NETWORK);
		if (mNetworkListenHandler.hasMessages(MESSAGE_DATA_STATE_CHANGED))
			mNetworkListenHandler.removeMessages(MESSAGE_DATA_STATE_CHANGED);
		if (mNetworkListenHandler.hasMessages(MESSAGE_NETWORK_CHECK))
			mNetworkListenHandler.removeMessages(MESSAGE_NETWORK_CHECK);
		if (mNetworkListenHandler.hasMessages(MESSAGE_DATA_STATE_CHANGED_MODEEXCHANGE))
			mNetworkListenHandler.removeMessages(MESSAGE_DATA_STATE_CHANGED_MODEEXCHANGE);
		if (mNetworkListenHandler.hasMessages(MESSAGE_CONTINUE_WAP_CONNECTIVITY))
			mNetworkListenHandler.removeMessages(MESSAGE_CONTINUE_WAP_CONNECTIVITY);
		if (mNetworkListenHandler.hasMessages(MESSAGE_MODE_CHECKING))
			mNetworkListenHandler.removeMessages(MESSAGE_MODE_CHECKING);
		if (mNetworkListenHandler.hasMessages(MESSAGE_WAIT_NETWORK_RESULT))
			mNetworkListenHandler.removeMessages(MESSAGE_WAIT_NETWORK_RESULT);
		unregisterListener();
		mConnectivityManager = null;
		if (mConnectivityListener != null) {
			mConnectivityListener.stopListening();
			mConnectivityListener = null;
		}
		sInstance = null;
	}

	private int lookupHost(String hostname) {
		InetAddress inetAddress;
		try {
			inetAddress = InetAddress.getByName(hostname);
		} catch (UnknownHostException e) {
			return -1;
		}
		byte[] addrBytes;
		int addr;
		addrBytes = inetAddress.getAddress();
		addr = ((addrBytes[3] & 0xff) << 24) | ((addrBytes[2] & 0xff) << 16)
				| ((addrBytes[1] & 0xff) << 8) | (addrBytes[0] & 0xff);
		return addr;
	}

	private void clearOperateMap() {
		if (MBBMSService.sInstance != null) {
			Util.clearItems(MBBMSService.sInstance.mOperateList);
		}

	}
	
	private void broadcastNetConnect() {
		if(mContext!=null){
			mContext.sendBroadcast(new Intent().setAction(ACTION_START_CONNECT_NET));
		}
	}
	
	private boolean isWAPType(String apnType) {
		String[] apntypes = apnType.split(",");
		for(int i = 0; i < apntypes.length; i++) {
			if(apntypes[i].equals("wap")) {
				return true;
			}
		}
		return false;
	}
	
	public void requestRouteToHost() {
		boolean connectivityResult = mConnectivityManager.requestRouteToHost(
				ConnectivityManager.TYPE_MOBILE_WAP, lookupHost(mCurrentProxy));
		LogMask.LOGM(TAG, LogMask.LOG_APP_NETWORK, LogMask.getLineInfo(),
				"request route to host result is "+ connectivityResult,
				LogMask.APP_NETWORK);
	}
	
}
