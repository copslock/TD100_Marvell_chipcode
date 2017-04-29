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

package com.marvell.cmmb.service;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Currency;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import com.marvell.cmmb.R;
import com.marvell.cmmb.activity.PlaybackSetting;
import com.marvell.cmmb.activity.PlayingActivity;
import com.marvell.cmmb.aidl.CaInfo;
import com.marvell.cmmb.aidl.EmergencyMessage;
import com.marvell.cmmb.aidl.IsmaPara;
import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.aidl.MBBMSEngine;
import com.marvell.cmmb.aidl.MBBMSPlayer;
import com.marvell.cmmb.aidl.MrvlMBBMSEngineCallback;
import com.marvell.cmmb.aidl.MrvlMBBMSEngineCallbackIF;
import com.marvell.cmmb.aidl.ServiceParam;
import com.marvell.cmmb.aidl.Preference;
import com.marvell.cmmb.aidl.SignalStatus;
import com.marvell.cmmb.common.Action;
import com.marvell.cmmb.common.AppBase;
import com.marvell.cmmb.common.Define;
import com.marvell.cmmb.common.Msg;
import com.marvell.cmmb.common.MsgEvent;
import com.marvell.cmmb.common.NotifyDataChangeListener;
import com.marvell.cmmb.common.ProcessMsg;
import com.marvell.cmmb.common.Util;
import com.marvell.cmmb.entity.EsgArg;
import com.marvell.cmmb.entity.FreTsArg;
import com.marvell.cmmb.entity.InitArg;
import com.marvell.cmmb.entity.ScanArg;
import com.marvell.cmmb.entity.SgParserArg;
import com.marvell.cmmb.entity.SubArg;
import com.marvell.cmmb.manager.AudioFocusManager;
import com.marvell.cmmb.manager.ModeManager;
import com.marvell.cmmb.manager.ModeManager.ModeGetListener;
import com.marvell.cmmb.manager.ImageCaptureManager;
import com.marvell.cmmb.manager.VideoRecorder;
import com.marvell.cmmb.manager.SignalManager;
import com.marvell.cmmb.parser.ESGParser;
import com.marvell.cmmb.parser.SGParser;
import com.marvell.cmmb.resolver.ChannelItem;
import com.marvell.cmmb.resolver.EmergencyItem;
import com.marvell.cmmb.resolver.IsmaparaItem;
import com.marvell.cmmb.resolver.MBBMSResolver;
import com.marvell.cmmb.resolver.PurchaseInfoItem;
import com.marvell.cmmb.resolver.ServiceFreqItem;
import com.marvell.cmmb.resolver.ServiceParamItem;
import com.marvell.cmmb.view.customview.PlayFrameView;
import com.marvell.cmmb.view.dialog.BinderDiedDialog;
import com.marvell.cmmb.view.dialog.ConfirmDialog;
import com.marvell.cmmb.view.dialog.PromptDialog;
import com.marvell.cmmb.view.dialog.PromptDialogCallBack;
import com.marvell.cmmb.view.dialog.PromptDialog.PromptDialogKeycallback;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.database.ContentObserver;
import android.media.MediaPlayer;
import android.net.ConnectivityManager;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.PowerManager;
import android.util.Log;
import android.telephony.TelephonyManager;
import android.view.Gravity;
import android.view.Menu;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.SurfaceHolder.Callback;
import android.widget.ImageView;
import android.widget.Toast;
import android.os.SystemProperties;
import android.os.IBinder.DeathRecipient;
import com.android.internal.telephony.TelephonyIntents;
import android.media.AudioSystem;
import android.provider.Telephony;

public class MBBMSService extends Service implements Action, Define,
		MrvlMBBMSEngineCallbackIF, MsgEvent, ModeGetListener, DeathRecipient{

	private static final String TAG = "MBBMSService";

	public static String sMBBMSDatePath = "/data/app/cmmb/";

	public static String sMBBMSImagePath = "/data/app/cmmb/media/";
	
	public static String sUserAgent = "default";

	public static int sBSMSelectorId;

	public static final String LOCAL_CITY = "local";

	private String mCityName = LOCAL_CITY;

	private String mChannelId = "all";

	public static final int[] sFrequecyList = { 13, 14, 15, 16, 17, 18, 19, 20,
			21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
			38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48 };

	private static final int MIN_FREQUECY = 13;

	private static final int MAX_FREQUECY = 48;

	public static int sCurrentMode = CMMB_MODE;

	private ArrayList<Integer> mFreqList = new ArrayList<Integer>();
	
	
	

	/**
	 * 
	 * the state of the application
	 * 
	 */
	public enum State {
		PlaybackLoading, PlaybackPlaying, PlaybackStoped
	}

	public HashMap<String, Integer> mOperateList = new HashMap<String, Integer>();// -1

	private ArrayList<PurchaseInfoItem> mInquiryList = new ArrayList<PurchaseInfoItem>();

	private SharedPreferences mFreqPrefer;

	public static State sPlayBackState = State.PlaybackStoped;

	public static State sPlayer2State = State.PlaybackStoped;

	private MrvlMBBMSEngineCallback mEngineCallback;

	private ModeManager mModeManager;

	private ProcessMsg mProcessMsg;

	private NotifyDataChangeListener mNotifyDataChangeListener;

	public static int sCurrentFreq = 0;

	public static int authorizeCount = 0;// this is used to check whether authorize needed if UAM invalid
	
	public static boolean isEngineInit;
	
	public static boolean isEngineReset;
	
	public static boolean isSettingMode;

	public static boolean isUpdatingSG;

	public static boolean isUpdatingESG;

	public static boolean isUpdatingST;

	public static boolean isParsingSG;

	public static boolean isParsingST;

	public static boolean isUpdateSTFinished;

	public static boolean isGBAInited;

	public static boolean isAuthorizing;

	public static boolean isUpdateTS0;

	public static boolean isInquiring;

	public static boolean isParsingInquiry;

	public static boolean isSubscribing;

	public static boolean isUnsubscribing;

	public static boolean isLoadingExit;
	
	public static boolean isSGUpdateFailed;

	public boolean isAuthorizedOneTime;
	
	private boolean isUpdateStforsubscribe;

	private static ConcurrentHashMap<ServiceFreqItem, ServiceParam> mServiceParamMap = new ConcurrentHashMap<ServiceFreqItem, ServiceParam>();

	private ArrayList<ServiceParamItem> mESGParamList;

	public static boolean isPIPOn;

	public static boolean isTestMode;

	public static int sCurrentPlayPosition = 0;

	public static boolean isCMMBSettingChanged = false;
	
	public static boolean isNetworkNeedStop = false;

	public static int sVicePlayPosition = 0;
	
	public static boolean screenOff2MainplayerStop;
	
	public static boolean screenOff2ViceplayerStop;

	private int mFreqCount = 0;

	private int mESGXmlCount = 0;

	private int mEcmdataType = 0;

	private String[] mPurchaseIds;

	private static final int DELETE_FLAG_FALSE = 0;// emergency message has not been deleted
	private static final int DELETE_FLAG_TRUE = 1;// emergency message has been deleted

	private static final int XML_UNPARSED = 0;
	private static final int XML_PARSED = 1;

	public static final int SUCCESS = 1; // this flag used to indicate the operate is success
	public static final int FAIL = 0; // this flag used to indicate the operate is fail
	public static final int UNDONE = -1;// this flag used to indicate the operate is undone

	private static final int PLAYER1WAITAUTHORIZE = 1001;
	private static final int PLAYER2WAITAUTHORIZE = 1002;
	private static final int PLAYER1TIMEOUT = 1003;
	private static final int PLAYER2TIMEOUT = 1004;

	private boolean isGotSupportedSession;

	private boolean isSupportPIP;

	public static boolean isScanCurrentFrequency;
	
	
   public boolean isSpeakerOn = false;//default output is headphone;
	
   

	private HashMap<String, Integer> ESGXMLParsedMap = new HashMap<String, Integer>();
	

	private Handler mServiceHandler = new Handler() {

		public void handleMessage(Message msg) {
			switch (msg.what) {
			case MESSAGE_UPDATE_TS0:
				updateTS0();
				break;
			case MESSAGE_UPDATE_ST:
				updateST();
				break;
			case MESSAGE_INITGBA:
				initGBA();
				break;
			case MESSAGE_NETWORK_CHECK:
				if (sCurrentMode == CMMB_MODE) {
					sendBroadcast(new Intent().setAction(ACTION_MODE_CMMB));
				}
				break;
			case MESSAGE_SAVE_SERVICEPARAM:
				int freq = msg.arg1;
				if (sCurrentMode == MBBMS_MODE) {
					saveServiceParam(freq);
				}
				break;
			case MESSAGE_SCAN_FINISH:
				break;
			case MESSAGE_PARSEESG_FINISHED:
				break;
			case MESSAGE_CAINFO:
				CaInfo[] ca = (CaInfo[]) msg.obj;
				msg.obj = null;
				int size = msg.arg1;
				int caFreq = msg.arg2;
				if (size != 0) {
					mResolver.deleteCaInfo();
				}
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "BSMSelectorId is "
						+ sBSMSelectorId, LogMask.APP_COMMON);
				for (int i = 0; i < size; i++) {
					LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "CASystemID is "
							+ ca[i].getCASystemID() + " freq is " + caFreq, LogMask.APP_COMMON);
					if (ca[i].getCASystemID() == sBSMSelectorId) {
						mEcmdataType = ca[i].getECMDataType();
						mResolver.updateIsma(mEcmdataType, caFreq);
					}
					mResolver.insertCaInfo(ca[i].getCASystemID(), ca[i]
							.getServiceID(), ca[i].getEMMDataType(), ca[i]
							.getECMDataType(), ca[i].getECMTransportType());
				}
				ca = null;
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "ECMDataType is "
						+ mEcmdataType, LogMask.APP_COMMON);
				break;
			case MESSAGE_NOTIFY_DATA_CHANGED:
				if (mNotifyDataChangeListener != null) {
					mNotifyDataChangeListener.onDataChanged();
				}
				mServiceHandler.sendMessageDelayed(mServiceHandler
						.obtainMessage(MESSAGE_NOTIFY_DATA_CHANGED), 600 * 1000);
				break;
			case MESSAGE_INIT_FINISH:
				LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "engine int is " 
						+ isEngineInit + " mode setting is " + isSettingMode, LogMask.APP_MAIN);
				if(!isEngineInit && !isSettingMode) {
					isModeSeted = true;
					MBBMSEngine.getInstance().setMode(sCurrentMode);
					sendBroadcast(new Intent().setAction(ACTION_LOADING_FINISH));
					mServiceHandler.sendMessageDelayed(mServiceHandler
							.obtainMessage(MESSAGE_NOTIFY_DATA_CHANGED), 600 * 1000);
					if (sCurrentMode == MBBMS_MODE) {
						LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "MBBMS_MODE",
								LogMask.APP_MAIN);
						setOperationtype(STARTUP);
						onNetworkConnected();
					} else {
						LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "CMMB_MODE",
								LogMask.APP_MAIN);
						sendBroadcast(new Intent().setAction(ACTION_MODE_CMMB));
					}
				}			
				break;
			case MESSAGE_INIT_TIMEOUT:
				if(isEngineInit) {
					LogMask.LOGM(TAG, LogMask.LOG_APP_ERROR, LogMask.getLineInfo(), "engine init timeout",
							LogMask.APP_ERROR);
					sendBroadcast(new Intent(ACTION_LOADING_ERROR_QUIT));
				}
				break;
			case MESSAGE_ON_ERROR:
				switch (msg.arg1) {
				case ERROR_UAM_AUTHENTICATION_INVALID:
					if (sCurrentMode == MBBMS_MODE && authorizeCount != 0) {
						keepTimeout();
						startUsingNetwork(AUTHORIZE);
						sendBroadcast(new Intent()
								.setAction(ACTION_AUTHORIZE_STARTED));
						authorizeCount = authorizeCount - 1;
					}
					break;
				case ERROR_UNKNOWN:
				case ERROR_CMMB_MODULE_CAN_NOT_OPEN:
				case ERROR_CMMB_MODULE_NOT_DETECTED:
				case ERROR_CMMB_MODULE_OTHER_ERROR:
				case ERROR_CMMB_MODULE_FIRMWARE_DOWNLOAD_FAILED:
				case ERROR_PATH_NO_PERMISSION:
				case ERROR_PATH_NOT_EXIST:
					break;
				case ERROR_DEMUX_DATA_CRC_ERROR:
				case ERROR_DEMUX_LOST_FRAME:
					if(sPlayBackState == State.PlaybackPlaying){
						if(msg.arg2 == 2) {
							getLongPromptToast(R.string.weak_signal).show();
						}
					}
					break;
				case ERROR_MSG_REC_SDCARD_NOT_FOUND:
					if (VideoRecorder.sInstance != null) {
						VideoRecorder.sInstance.onError();
						getShortPromptToast(R.string.nosdcard).show();
					}
					break;
				case ERROR_MSG_REC_STORAGE_FULL:
					if (VideoRecorder.sInstance != null) {
						getShortPromptToast(R.string.fullstorage).show();
						VideoRecorder.sInstance.onRecordFinished();
					}
					break;
				case ERROR_MSG_REC_EXCEED_MAX_SIZE:
					if (VideoRecorder.sInstance != null) {
						getShortPromptToast(R.string.exceedmaxsize).show();
						VideoRecorder.sInstance.onRecordFinished();
					}
					break;
				case ERROR_MSG_REC_UNKNOWN_ERROR:
					if (VideoRecorder.sInstance != null) {
						getShortPromptToast(R.string.unknowerror).show();
						VideoRecorder.sInstance.onError();
					}
					break;
				case INFO_MSG_CAP_SUCCESS:
					if (ImageCaptureManager.sInstance != null) {
						if (ImageCaptureManager.sInstance.isInBackgoundCapture) {
							ImageCaptureManager.sInstance.onCaptured(msg.arg2,
									true);
						} else {
							ImageCaptureManager.sInstance.onImageCapture(true);
						}
					}
					break;
				case ERROR_MSG_CAP_ERROR:
					if (ImageCaptureManager.sInstance != null) {
						if (ImageCaptureManager.sInstance.isInBackgoundCapture) {
							ImageCaptureManager.sInstance.onCaptured(msg.arg2,
									false);
						} else {
							ImageCaptureManager.sInstance.onImageCapture(false);
						}
					}
					break;
				}
				break;
			case MESSAGE_EBMESSAGE_RECEIVED:
				EmergencyMessage msg1 = (EmergencyMessage) msg.obj;
				if(duplicateEbMsg(msg1)) {
					LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),"this EmergencyMessage is duplicated", LogMask.APP_MAIN);
					}else {
						storeEmergencyMessage(msg1);
						}
					msg1 = null;
				break;
			case PLAYER1TIMEOUT:
				if (null != mMediaListener
						&& sPlayBackState == State.PlaybackLoading) {
					Message msgTimeOut1 = mMediaListener.obtainMessage();
					msgTimeOut1.what = MESSAGE_PREPARE_TIMEOUT;
					msgTimeOut1.arg1 = PLAYER1;
					mMediaListener.sendMessage(msgTimeOut1);
				}
				break;
			case PLAYER2TIMEOUT:
				if (null != mMediaListener
						&& sPlayer2State == State.PlaybackLoading) {
					Message msgTimeOut2 = mMediaListener.obtainMessage();
					msgTimeOut2.what = MESSAGE_PREPARE_TIMEOUT;
					msgTimeOut2.arg1 = PLAYER2;
					mMediaListener.sendMessage(msgTimeOut2);
				}
				break;
			case MESSAGE_SET_MODE:
				mModeManager.setMode();
				break;
			case EVENT_APN_CHANGED:
			    onAPNChanged();
			    break;
			case MESSAGE_PLAY_AFTER_UPDATETS0:
			    playTVAfterUpdateTS0();
			    break;
			}
			super.handleMessage(msg);
		}

	};
	public static final int PLAYER1 = 0;

	public static final int PLAYER2 = 1;

	public MBBMSPlayer mPlayer1;

	public MBBMSPlayer mPlayer2;

	public static int mMainPlayer = PLAYER1;

	public static int mVicePlayer = PLAYER2;

	public static MBBMSService sInstance;

	public Handler mMediaListener;
	public TelephonyManager mTelephonyMgr;

	public Callback mPlayer1Holder = new Callback() {

		public void surfaceChanged(SurfaceHolder holder, int format, int width,
				int height) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"mPlayer1Holder, surfaceChanged isValidState "
							+ mPlayer1.isValidState() + " hasValidSize"
							+ mPlayer1.hasValidSize(width, height) + "width "
							+ width + "height " + height, LogMask.APP_COMMON);
			if (mPlayer1.hasValidSize(width, height)) {
				mPlayer1.setSurfaceHolder(holder);
			}
			if (null != mMediaListener) {
				Message msg = mMediaListener.obtainMessage(MESSAGE_SETLAYERFORPIP);
				mMediaListener.sendMessage(msg);
			}
		}

		public void surfaceCreated(SurfaceHolder holder) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"mPlayer1Holder, surfaceCreated", LogMask.APP_COMMON);
			mPlayer1.setSurfaceHolder(holder);
		}

		public void surfaceDestroyed(SurfaceHolder holder) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"mPlayer1Holder, surfaceDestroyed", LogMask.APP_COMMON);
			mPlayer1.setSurfaceHolder(null);
		}

	};

	public Callback mPlayer2Hold = new Callback() {

		public void surfaceChanged(SurfaceHolder holder, int format, int width,
				int height) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"mPlayer2Holder, surfaceChanged isValidState "
							+ mPlayer2.isValidState() + " hasValidSize"
							+ mPlayer2.hasValidSize(width, height) + "width "
							+ width + "height " + height, LogMask.APP_COMMON);
			if (mPlayer2.hasValidSize(width, height)) {
				mPlayer2.setSurfaceHolder(holder);
			}
			if (null != mMediaListener) {
				Message msg = mMediaListener.obtainMessage(MESSAGE_SETLAYERFORPIP);
				mMediaListener.sendMessage(msg);
			}
		}

		public void surfaceCreated(SurfaceHolder holder) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"mPlayer2Holder, surfaceCreated", LogMask.APP_COMMON);
			mPlayer2.setSurfaceHolder(holder);
		}

		public void surfaceDestroyed(SurfaceHolder holder) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"mPlayer2Holder, surfaceDestroyed", LogMask.APP_COMMON);
			mPlayer2.setSurfaceHolder(null);
		}

	};

	private MediaPlayer.OnErrorListener mPlayer1ErrorListener = new MediaPlayer.OnErrorListener() {

		public boolean onError(MediaPlayer mp, int what, int extra) {
			sPlayBackState = State.PlaybackLoading;
			Message msg = mMediaListener.obtainMessage(MESSAGE_PLAY_ERROR);
			msg.arg1 = PLAYER1;
			mMediaListener.sendMessage(msg);
			return false;
		}
	};

	private MediaPlayer.OnPreparedListener mPlayer1PreparedListener = new MediaPlayer.OnPreparedListener() {

		public void onPrepared(MediaPlayer mp) {
			sPlayBackState = State.PlaybackPlaying;
			onPlayStateChaned(PLAYER1);
			if (null != mMediaListener) {
				Message msg = mMediaListener.obtainMessage(MESSAGE_PLAY);
				msg.arg1 = PLAYER1;
				mMediaListener.sendMessage(msg);
			} else {
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
						"mMediaListener is null", LogMask.APP_COMMON);
			}
			removeTimeOutMessage(PLAYER1);
			if(sCurrentMode == MBBMS_MODE && !isAuthorizedOneTime) {
				isAuthorizedOneTime = true;
				startUsingNetwork(AUTHORIZE);
			}			
		}

	};

	private MediaPlayer.OnErrorListener mPlayer2ErrorListener = new MediaPlayer.OnErrorListener() {

		public boolean onError(MediaPlayer mp, int what, int extra) {
			sPlayer2State = State.PlaybackLoading;
			Message msg = mMediaListener.obtainMessage(MESSAGE_PLAY_ERROR);
			msg.arg1 = PLAYER2;
			mMediaListener.sendMessage(msg);
			return false;
		}

	};

	private MediaPlayer.OnPreparedListener mPlayer2PreparedListener = new MediaPlayer.OnPreparedListener() {

		public void onPrepared(MediaPlayer mp) {
			sPlayer2State = State.PlaybackPlaying;
			onPlayStateChaned(PLAYER2);
			if (null != mMediaListener) {
				Message msg = mMediaListener.obtainMessage(MESSAGE_PLAY);
				msg.arg1 = PLAYER2;
				mMediaListener.sendMessage(msg);
			} else {
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
						"mMediaListener is null", LogMask.APP_COMMON);
			}
			removeTimeOutMessage(PLAYER2);
			if(sCurrentMode == MBBMS_MODE && !isAuthorizedOneTime) {
				isAuthorizedOneTime = true;
				startUsingNetwork(AUTHORIZE);
			}			
		}

	};

	public MBBMSResolver mResolver;

	public SignalManager mSignalManager;

	private Toast mShortPromptToast;

	private Toast mLongPromptToast;

	private ArrayList<EmergencyItem> mEmergencyMessageList;

	private ArrayList<EmergencyItem> mEmergencyVisbleList;

	public static boolean isModeSeted;

	public static boolean isDataGet;

	private AudioFocusManager mAudioFocusManager;

	public boolean isEbMsgStopMainPlayer;

	public boolean isEbMsgStopVicePlayer;

	public String mLastMainServiceId;
	
	public long mLastMainServiceReferenceId;

	public int mLastMainFreq;

	public String mLastViceServiceId;
	
	public long mLastViceServiceReferenceId;

	public int mLastViceFreq;

	public Toast mMuteTost;

	public static boolean enableScreenOffPlayback;
	
	private BroadcastReceiver mAPNChange;
	
	private ApnChangeObserver apnObserver;
	
	private boolean bNeedUpdateTS0;
	
	public IBinder onBind(Intent intent) {
		// TODO Auto-generated method stub
		return null;
	}

	public void onCreate() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "onCreate", LogMask.APP_MAIN);
		super.onCreate();
		setConfigToDefalt();
		init();
		initEngine();
		startSettingMode();
		setCallback();
		if(AppBase.isNeedHeadPhone) {//only need headphone,register a receiver
			registerHeadPhoneReceiver();
		}		
	}

	public void onDestroy() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "onDestroy", LogMask.APP_MAIN);
		super.onDestroy();
		deinit();
		MBBMSEngine.getInstance().deinit();
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "onStartCommand",
				LogMask.APP_MAIN);
		if (intent != null) {
			super.onStart(intent, startId);
		}
		return START_NOT_STICKY;
	}

	private void releaseAudioFocusManager() {
		mAudioFocusManager.unRegisterReceiver();

	}

	public void onStart(Intent intent, int startId) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onStart",
				LogMask.APP_COMMON);
		super.onStart(intent, startId);
	}

	private void initEngine() {
		if(!isEngineInit) {
			isEngineInit = true;
			Msg msg = new Msg();
			msg.setHandler(this);
			msg.setEvent(EVENT_INIT);
			InitArg ia = new InitArg();
			ia.mCB = MrvlMBBMSEngineCallback.getInstance();
			ia.dtPath = sMBBMSDatePath;
			ia.mAgent = sUserAgent;
			msg.setData(ia);
			mProcessMsg.addMsg(msg);
		}	
	}
	
	public void resetEngine() {
		if(!isEngineReset) {
			isEngineReset = true;
			Msg msg = new Msg();
			msg.setHandler(this);
			msg.setEvent(EVENT_RESET);
			mProcessMsg.addMsg(msg);
		}
	}
	
	private void startSettingMode() {
		if(!isSettingMode) {
			isSettingMode = true;
			mModeManager.checkSimcardIn();
		}
	}

	/*
	 * do some initialize work
	 */
	private void init() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "init, app build 1223", LogMask.APP_MAIN);
		sInstance = this;
		isLoadingExit = false;
		mResolver = MBBMSResolver.getInstance(this);
		mFreqPrefer = getSharedPreferences("CurrentFrequency", MODE_PRIVATE);
		getPlayer(PLAYER1);
		getPlayer(PLAYER2);
		mProcessMsg = ProcessMsg.getInstance(getBaseContext());
		mProcessMsg.toStart();
		mModeManager = ModeManager.getInstance(getBaseContext());
		mModeManager.setModeGetListener(this);
		mModeManager.setOperationType(STARTUP);
		mModeManager.getCurrentAPN();
		if (mSignalManager == null) {
			mSignalManager = new SignalManager(this);
		}
		mSignalManager.hungUp();
		mAudioFocusManager = AudioFocusManager.getIntance(this);
		MBBMSEngine.getInstance().linkToDeath(this);
		enableScreenOffPlayback = getSharedPreferences(
                PlaybackSetting.SHAREDPREFERENCE_PLAYBACK_CHECK, Context.MODE_PRIVATE).getBoolean(PlaybackSetting.PLAYBACK_CHEKED, false);
		registAPNChange();
		mTelephonyMgr = (TelephonyManager)(getBaseContext().getSystemService(Context.TELEPHONY_SERVICE));
		registAudioFocus();
	}

	private void registAPNChange(){
//	    IntentFilter filter = new IntentFilter();
//	    filter.addAction(ConnectivityManager.CONNECTIVITY_ACTION);
//	    filter.addAction(TelephonyIntents.ACTION_ANY_DATA_CONNECTION_STATE_CHANGED);
//	    mAPNChange = new ApnChangeBroadcastRecieved();
//	    this.registerReceiver(mAPNChange, filter);
	    apnObserver = new ApnChangeObserver();
	    this.getContentResolver().registerContentObserver(Telephony.Carriers.CONTENT_URI, true, apnObserver);
	}
	
	private void unRegistAPNChange(){
//	    this.unregisterReceiver(mAPNChange);
	    this.getContentResolver().unregisterContentObserver(apnObserver);
	}
	
	private void deinit() {
	    unRegistAPNChange();
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "deinit", LogMask.APP_COMMON);
		MrvlMBBMSEngineCallback.getInstance().setCallBack(null);
		removePrepareTimeoutByauthorize();
		getPlayer(PLAYER1).release(true);
		getPlayer(PLAYER2).release(true);
		mPlayer1 = null;
		mPlayer2 = null;
		isModeSeted = false;
		mSignalManager.clearNotification();
		mSignalManager = null;
		if (mModeManager.isNetworkConnected()) {
			mModeManager.stopWapConnectivity();// disconnect the network when exit
		}
		if (mServiceHandler.hasMessages(MESSAGE_NOTIFY_DATA_CHANGED)) {
			mServiceHandler.removeMessages(MESSAGE_NOTIFY_DATA_CHANGED);
		}
		if(mServiceHandler.hasMessages(MESSAGE_INIT_TIMEOUT)) {
			mServiceHandler.removeMessages(MESSAGE_INIT_TIMEOUT);
		}
		stopModeListen();// stop listen network for mode exchange
		mProcessMsg.toStop();
		mProcessMsg = null;
		mModeManager.deinit();
		mModeManager = null;
		sInstance = null;
	//	resetOutputDeviceState();
		setForceUseNone();
		setHeadPhoneState(false);//reset the flags;
		setSpeaker(false);
		releaseAudioFocusManager();
	}

	private void setConfigToDefalt() {
		sPlayBackState = State.PlaybackStoped;
		sPlayer2State = State.PlaybackStoped;
		sCurrentFreq = 0;
		authorizeCount = 0;
		isEngineInit = false;
		isEngineReset = false;
		isSettingMode = false;
		isUpdatingSG = false;
		isUpdatingESG = false;
		isUpdatingST = false;
		isParsingSG = false;
		isParsingST = false;
		isUpdateSTFinished = false;
		isGBAInited = false;
		isAuthorizing = false;
		isUpdateTS0 = false;
		isInquiring = false;
		isParsingInquiry = false;
		isSubscribing = false;
		isUnsubscribing = false;
		isPIPOn = false;
		isTestMode = false;
		sCurrentPlayPosition = 0;
		isCMMBSettingChanged = false;
		sVicePlayPosition = 0;
		mMainPlayer = PLAYER1;
		mVicePlayer = PLAYER2;
		isModeSeted = false;
		isDataGet = false;
		isScanCurrentFrequency = false;
		isNetworkNeedStop = false;
		isSGUpdateFailed = false;
		screenOff2MainplayerStop = false;
		screenOff2ViceplayerStop = false;
	}

	private void setCallback() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "setcallback() called",
				LogMask.APP_COMMON);
		mEngineCallback = MrvlMBBMSEngineCallback.getInstance();
		mEngineCallback.setCallBack(this);
	}

	public void setPreference() {
		SharedPreferences preference = getSharedPreferences("MBBMSSETTING",
				MODE_PRIVATE);
//		String userAgent = SystemProperties.get("persist.mbbms.useragent");
//		if (userAgent.equals("")) {
//			userAgent = "Marvell MBBMSEngine 2.0";
//		}
		Preference prefer = new Preference();
		prefer.setMApn(mModeManager.getApn());
//		prefer.setMUserAgent(userAgent);
		prefer.setMSgDomain(preference.getString("sgAddress",
				"sg.mbbms.chinamobile.com"));
		try {
			prefer.setMSgPort(Integer.parseInt(preference.getString("sgPort", "80")));
		} catch (Exception e) {
			e.printStackTrace();
			prefer.setMSgPort(80);
		}
		prefer.setMWapGateway(mModeManager.getProxy());
		prefer.setMWapPort(mModeManager.getPort());
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "set preference start",
				LogMask.APP_MAIN);
		MBBMSEngine.getInstance().setPreference(prefer);
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "set preference end",
				LogMask.APP_MAIN);
	}

	private void saveServiceParam(ArrayList<Integer> list) {
		for (int i = 0; i < list.size(); i++) {
			saveServiceParam(list.get(i));
		}
	}

	/**
	 * @param freq
	 *            : the frequency of a service parameter
	 * 
	 *            this method save the service parameter information to database
	 *            of specific frequency
	 */
	public void saveServiceParam(int freq) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "save new param",
				LogMask.APP_COMMON);
		MBBMSResolver.getInstance(getBaseContext()).deleteServiceParam(freq);
		ServiceParam param = new ServiceParam();
		for (ServiceFreqItem freqItem : mServiceParamMap.keySet()) {
			param = mServiceParamMap.get(freqItem);
			if (param.getFreq() == freq) {
				MBBMSResolver.getInstance(getBaseContext()).insertServiceparam(
						param.getServiceid(), param.getServiceType(),
						param.getServiceName(), param.getDemod(),
						param.getFreq(), param.getStartTs(), param.getNts(),
						param.getFId(), param.getSubFrame(),
						param.getEsgTotal(), param.isFree(),
						param.isUpdateFlag());
			}
		}
		param = null;
	}

	private boolean inquiry() {
		if (!isInquiring) {
			isInquiring = true;
			Msg msg = new Msg();
			msg.setHandler(this);
			msg.setEvent(EVENT_INQUIRY);
			mProcessMsg.addMsg(msg);
			return true;
		}
		return false;
	}

	public void startInquiryParser(String inquiryXml) {
		if (!isParsingInquiry) {
			isParsingInquiry = true;
			Msg msg = new Msg();
			msg.setHandler(this);
			msg.setEvent(EVENT_PARSE_INQUIRY);
			msg.setData(inquiryXml);
			mProcessMsg.addMsg(msg);
		}
	}

	public boolean updateSG(String cityName) {
		if (!isUpdatingSG) {
			isUpdatingSG = true;
			sendBroadcast(new Intent().setAction(ACTION_SG_UPDATING));
			Msg msg = new Msg();
			msg.setHandler(this);
			msg.setEvent(EVENT_STARTUPDATESG);
			msg.setData(cityName);
			mProcessMsg.addMsg(msg);
			return true;
		}
		return false;
	}

	public boolean updateESG() {
		if (mFreqCount >= 1) {// to avoid outofbound error
			if (!isUpdatingESG) {
				isUpdatingESG = true;
				Msg msg = new Msg();
				msg.setHandler(this);
				msg.setEvent(EVENT_STARTUPDATEESG);
				FreTsArg esgParam = new FreTsArg();
				esgParam.freq = mESGParamList.get(mFreqCount - 1).getFrequecy();
				esgParam.startTs = mESGParamList.get(mFreqCount - 1)
						.getStartTs();
				esgParam.countTs = mESGParamList.get(mFreqCount - 1)
						.getCountTs();
				esgParam.mod = mESGParamList.get(mFreqCount - 1).getDemod();
				msg.setData(esgParam);
				mProcessMsg.addMsg(msg);
				return true;
			}
		}
		return false;
	}

	public void stopUpdateESG() {
		MBBMSEngine.getInstance().stopUpdateESG();
		isUpdatingESG = false;
	}

	/**
	 * To parse xml files of service guide ,if parser successfully,update the
	 * View, else give the prompt.
	 * 
	 */
	public void startSGParser(String sgddXml, String sgXml) {
		if (!isParsingSG) {
			isParsingSG = true;
			Msg msg = new Msg();
			msg.setHandler(this);
			msg.setContext(getBaseContext());
			msg.setEvent(EVENT_PARSE_SG);
			SgParserArg sgParserXml = new SgParserArg();
			sgParserXml.sgddXml = sgddXml;
			sgParserXml.sgXml = sgXml;
			msg.setData(sgParserXml);
			msg.setContext(getBaseContext());
			mProcessMsg.addMsg(msg);
		}
	}

	public void startESGParser(int type, String fname, int freq, int cookie) {
		Msg msg = new Msg();
		msg.setHandler(this);
		msg.setContext(getBaseContext());
		msg.setEvent(EVENT_PARSE_ESG);
		EsgArg esgxml = new EsgArg();
		esgxml.freq = freq;
		esgxml.fname = fname;
		esgxml.type = type;
		esgxml.cookie = cookie;
		msg.setData(esgxml);
		mProcessMsg.addMsg(msg);
		Util.addItem(ESGXMLParsedMap, String.valueOf(cookie), XML_UNPARSED);
	}

	/**
	 * To parse xml files of subscribe table ,if parser successfully,update the
	 * View, else give the prompt.
	 */
	public void startSTParser(String stXml) {
		if (!isParsingST) {
			isParsingST = true;
			Msg msg = new Msg();
			msg.setHandler(this);
			msg.setContext(getBaseContext());
			msg.setEvent(EVENT_PARSE_ST);
			msg.setData(stXml);
			mProcessMsg.addMsg(msg);
		}
	}

	public void updateST() {
		if (!isUpdatingST) {
			isUpdatingST = true;
			Msg msg = new Msg();
			msg.setHandler(this);
			msg.setEvent(EVENT_STARTUPDATESUBSCRIPTIONTABLE);
			mProcessMsg.addMsg(msg);
		}
	}

	public void startSubscribe(String[] purchaseIds) {
		if (!isSubscribing) {
			isSubscribing = true;
			Msg msg = new Msg();
			msg.setHandler(this);
			msg.setEvent(EVENT_STARTSUBSCRIBE);
			SubArg subArg = new SubArg();
			subArg.purchaseID = purchaseIds;
			subArg.count = purchaseIds.length;
			msg.setData(subArg);
			mProcessMsg.addMsg(msg);
		}
	}

	public void startUnsubscribe(String[] purchaseIds) {
		if (!isUnsubscribing) {
			isUnsubscribing = true;
			Msg msg = new Msg();
			msg.setHandler(this);
			msg.setEvent(EVENT_UNSUBSCRIBE);
			SubArg subArg = new SubArg();
			subArg.purchaseID = purchaseIds;
			subArg.count = purchaseIds.length;
			msg.setData(subArg);
			mProcessMsg.addMsg(msg);
		}
	}

	public void initGBA() {
		if (!isGBAInited) {
			isGBAInited = true;
			Msg msg = new Msg();
			msg.setHandler(this);
			msg.setEvent(EVENT_INITGBA);
			mProcessMsg.addMsg(msg);
		}
	}

	public void startAuthorize(String serviceId) {
		if (!isAuthorizing) {
			isAuthorizing = true;
			Msg msg = new Msg();
			msg.setHandler(this);
			msg.setEvent(EVENT_AUTHORIZESERVICE);
			msg.setData(serviceId);
			mProcessMsg.addMsg(msg);
		}
	}

	public void updateTS0() {
		if (!isUpdateTS0) {
			isUpdateTS0 = true;
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "update ts0, freq is "
					+ sCurrentFreq, LogMask.APP_COMMON);
			Msg msg = new Msg();
			msg.setHandler(this);
			msg.setEvent(EVENT_STARTSCAN);
			ScanArg sa = new ScanArg();
			int[] currentFreqs = mResolver.getAllFrequency(MBBMS_MODE);
			if (currentFreqs != null) {
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "freq size is "
						+ currentFreqs.length, LogMask.APP_COMMON);
				sa.freqs = currentFreqs;
				sa.count = currentFreqs.length;
				msg.setData(sa);
				sa = null;
				mProcessMsg.addMsg(msg);
			}
		}
	}

	public void startScan() {
		sendBroadcast(new Intent().setAction(ACTION_SCAN_START));
		mServiceParamMap.clear();
		mFreqList.clear();
		Util.clearItems(ESGXMLParsedMap);
		ESGParser.getInstance(getBaseContext()).init();
		int freqs[] = getCMMBSettingFreqs();
		startScan(freqs);
	}

	private void scanFromCurrentFreq() {
		String freqs = mFreqPrefer.getString("CurrentFreq", "39");
		String[] currentFreqs = freqs.split(",");
		int[] freq = new int[currentFreqs.length];
		for (int i = 0; i < currentFreqs.length; i++) {
			freq[i] = Integer.parseInt(currentFreqs[i]);
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"the freq of sharepreference include " + freq[i],
					LogMask.APP_COMMON);
		}
		startScan(freq);
	}

	private void startScan(int[] freqs) {
		if (!isUpdateTS0) {
			isUpdateTS0 = true;
			Msg msg = new Msg();
			msg.setHandler(this);
			msg.setEvent(EVENT_STARTSCAN);
			ScanArg sa = new ScanArg();
			sa.freqs = freqs;
			sa.count = freqs.length;
			msg.setData(sa);
			mProcessMsg.addMsg(msg);
		}
	}

	public void stopScan() {
		MBBMSEngine.getInstance().stopScan();
		isUpdateTS0 = false;
	}

	/**
	 * try to start network and check the mode with the result
	 * 
	 * @return the result of start using network
	 */
	public boolean checkMode() {
		boolean result = mModeManager.startUsingNetwork(MESSAGE_NETWORK_CHECK);
		if (result) {
			mServiceHandler.sendEmptyMessageDelayed(MESSAGE_NETWORK_CHECK,
					ModeManager.NETWORK_EXTENDION_WAIT);
		} else {
			onSetMode(CMMB_MODE);
		}
		return result;
	}

	/**
	 * stop connect network and set cmmb mode
	 */
	public void cancelCheckMode() {
		if (mServiceHandler.hasMessages(MESSAGE_NETWORK_CHECK)) {
			mServiceHandler.removeMessages(MESSAGE_NETWORK_CHECK);
		}
		onSetMode(CMMB_MODE);
	}

	/**
	 * fetch cmmb setting information, get the frequency information for scan
	 * 
	 * @return the frequency need to be scaned
	 */
	public int[] getCMMBSettingFreqs() {
		int[] freq = null;
		SharedPreferences prefer = getSharedPreferences(CMMBSETING,
				MODE_PRIVATE);
		String scanType = prefer.getString("scanType", getResources()
				.getString(R.string.scanall));
		String scanCity = prefer.getString("scanCity", getResources()
				.getString(R.string.defaultcity));
		int minFreq = prefer.getInt("minFreq", 13);
		int maxFreq = prefer.getInt("maxFreq", 48);
		if (scanType.equals(getResources().getString(R.string.scanall))) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "scan all frequency",
					LogMask.APP_COMMON);
			freq = sFrequecyList;
		} else if (scanType.equals(getResources()
				.getString(R.string.scanbycity))) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "scan by city",
					LogMask.APP_COMMON);
			String[] cities = getResources().getStringArray(R.array.cities);
			if (prefer.getString("scanCity", "").equals(cities[0])) {
				freq = new int[] { 20, 43 };
			} else {
				freq = new int[1];
				freq[0] = prefer.getInt("minFreq", 39);
			}
		} else if (scanType.equals(getResources().getString(
				R.string.customizedscan))) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "custom can",
					LogMask.APP_COMMON);
			if (minFreq < 13) {
				minFreq = 13;
			}
			if (maxFreq > 48) {
				maxFreq = 48;
			}
			freq = new int[maxFreq - minFreq + 1];
			for (int i = 0; i < freq.length; i++) {
				freq[i] = minFreq + i;
			}
		}
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
				"freq size is " + freq.length, LogMask.APP_COMMON);
		return freq;
	}

	/**
	 * get the ESG Parameter List
	 */
	private void getESGParam() {
		mESGParamList = MBBMSResolver.getInstance(getBaseContext())
				.getServiceParamById(254);
		mFreqCount = mESGParamList.size();
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
				"mFreqCount is " + mFreqCount, LogMask.APP_COMMON);
	}

	private void clearCMMBChannelList() {
		MBBMSResolver.getInstance(getBaseContext()).deleteChannel(CMMB_MODE);
		MBBMSResolver.getInstance(getBaseContext()).deleteProgram(CMMB_MODE);
	}

	/**
	 * stop checking network connecting state for mode exchange
	 */
	public void stopModeListen() {
		mModeManager.stopModeListen();
	}

	public void stopWapConnectivity() {
		if(isNetworkNeedStop) {
			mModeManager.stopWapConnectivity();
		}		
	}

	public void startUsingNetwork(int reason) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
				"start using network , reason is " + reason, LogMask.APP_MAIN);
		setOperationtype(reason);
		boolean result = mModeManager.startUsingNetwork(MESSAGE_NETWORK_CHECK);
		if (!result) {
			onNetworkDisconnected();
		}
	}

	public void exitapplication() {
		MrvlMBBMSEngineCallback.getInstance().setCallBack(null);
		MBBMSService.sInstance.stopSelf();
	}

	public void cancelEvent(int event) {
		mProcessMsg.cancelEvent(event);
	}

	public void onEventFinished(int msgEvent, int cookie) {
		switch (msgEvent) {
		case EVENT_INIT:
			LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "init finish",
					LogMask.APP_MAIN);
			isEngineInit = false;
			if(cookie != 0) {
				sendBroadcast(new Intent(ACTION_LOADING_ERROR_QUIT));
			} else {
				mServiceHandler.sendEmptyMessage(MESSAGE_INIT_FINISH);
			}
			break;
		case EVENT_PARSE_SG:
			LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "parse sg finish",
					LogMask.APP_MAIN);
			isParsingSG = false;
			if(!isTestMode) {
				mServiceHandler.sendEmptyMessage(MESSAGE_UPDATE_ST);
				mServiceHandler.sendEmptyMessage(MESSAGE_UPDATE_TS0);
			}
			sendBroadcast(new Intent().setAction(ACTION_PARSESG_FINISHED));
			Util.setItem(mOperateList, OPERATION_PARSE_SG, SUCCESS);
			break;
		case EVENT_PARSE_ST:
			LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "parse st finish",
					LogMask.APP_MAIN);
			isParsingST = false;
			isUpdateSTFinished = true;
			if(isUpdateStforsubscribe) {
				isUpdateStforsubscribe = false;
			} else {
				if(!isTestMode) {
					mServiceHandler.sendEmptyMessage(MESSAGE_INITGBA);
				}
			}
			sendBroadcast(new Intent().setAction(ACTION_PARSEST_FINISHED));
			Util.setItem(mOperateList, OPERATION_PARSE_ST, SUCCESS);
			break;
		case EVENT_PARSE_ESG:
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "parse esg, cookie is "
					+ cookie, LogMask.APP_COMMON);
			Util.setItem(ESGXMLParsedMap, String.valueOf(cookie), XML_PARSED);
			if (cookie == mESGXmlCount && !isUpdatingESG) {
				LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "fetch esg finish",
						LogMask.APP_MAIN);
				saveParserResult();
				isCompleted();
			}
			break;
		case EVENT_PARSE_INQUIRY:
			LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "parse inquiry finish",
					LogMask.APP_MAIN);
			isParsingInquiry = false;
			mInquiryList = mProcessMsg.getInquiryList();
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "mInquiry list size is "
					+ mInquiryList.size(), LogMask.APP_COMMON);
			sendBroadcast(new Intent().setAction(ACTION_PARSEINQUIRY_FINISHED));
			break;
		case EVENT_RESET:
			LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "reset finish",
					LogMask.APP_MAIN);
			isEngineReset = false;
			sendBroadcast(new Intent().setAction(ACTION_RESET_FINISHED));
			break;
		}

	}

	public void onAPNReset() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "apn need re-set",
				LogMask.APP_MAIN);
		if (isLoadingExit)
			return;
		sendBroadcast(new Intent().setAction(ACTION_SET_APN));
	}

	public void onSetMode(int mode) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "mode set finish, mode is "
				+ mode, LogMask.APP_MAIN);
		if (isLoadingExit)
			return;
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "engine is now init is " + isEngineInit,
				LogMask.APP_MAIN);
		sCurrentMode = mode;
		isSettingMode = false;
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "mode set finish",
				LogMask.APP_MAIN);
		mServiceHandler.sendEmptyMessage(MESSAGE_INIT_FINISH);
		if(isEngineInit) {
			mServiceHandler.sendEmptyMessageDelayed(MESSAGE_INIT_TIMEOUT, 10 * 1000);
		}	
	}

	public void onSimReady(boolean isSimReady) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "sim card ready is " + isSimReady,
				LogMask.APP_MAIN);
		if (isLoadingExit)
			return;
		if (isSimReady) {
			mServiceHandler.sendEmptyMessage(MESSAGE_SET_MODE);
		} else {
			Intent simIntent = new Intent();
			simIntent.setAction(ACTION_SIMCARD_OUT);
			sendBroadcast(simIntent);
			onSetMode(CMMB_MODE);
		}
	}

	/**
	 * connect network for different operation
	 */
	public void onNetworkConnected() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "wap network connected",
				LogMask.APP_MAIN);
		if (isLoadingExit)
			return;
		mModeManager.continueWapConnectivity();
		mModeManager.requestRouteToHost();
		switch (mModeManager.getOperationType()) {
		case STARTUP:
			setPreference();
		case AUTOREFRESH:
		case MANUALREFRESH:
			updateSG(mCityName);
			break;
		case INQUIRE:
			inquiry();
			break;
		case SUBSCRIBE:
			startSubscribe(mPurchaseIds);
			break;
		case UNSUBSCRIBE:
			startUnsubscribe(mPurchaseIds);
			break;
		case AUTHORIZE:
			if (!isAuthorizing) {
				if (isTestMode) {
					LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
							"authorize start for interface test",
							LogMask.APP_MAIN);
				} else {
					setAuthorizeId("all");
				}
				startAuthorize(mChannelId);
			}
			break;
		case UPDATESG:
			updateSG(mCityName);
			break;
		case UPDATEST:
			updateST();
			break;
		case INITGBA:
			initGBA();
			break;
		default:
			break;
		}
	}

	public void onNetworkDisconnected() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "wap network disconnected",
				LogMask.APP_MAIN);
		if (isLoadingExit)
			return;
		sendBroadcast(new Intent().setAction(ACTION_NETWORKCONNECT_FAILED));
	}

	/**
	 * get the MBBMSPlayer
	 * 
	 * @param player
	 *            the main player or the second
	 * @return MBBMSPlayer
	 */
	public MBBMSPlayer getPlayer(int player) {
		switch (player) {
		case PLAYER2:
			if (mPlayer2 == null) {
				LogMask.LOGM(TAG, LogMask.LOG_APP_ERROR, LogMask.getLineInfo(),
						"the second MBBMSPlayer is null", LogMask.APP_COMMON);
				mPlayer2 = new MBBMSPlayer(this);
			}
			return mPlayer2;
		default:
			if (mPlayer1 == null) {
				LogMask.LOGM(TAG, LogMask.LOG_APP_ERROR, LogMask.getLineInfo(),
						"the main MBBMSPlayer is null", LogMask.APP_COMMON);
				mPlayer1 = new MBBMSPlayer(this);
			}
			return mPlayer1;
		}
	}

	/**
	 * set surface to the player
	 * 
	 * @param player
	 *            Main or the second
	 * @param surfaceHolder
	 *            the surface to set to
	 */
	public void setSurface(int player, SurfaceHolder surfaceHolder) {
		switch (player) {
		case PLAYER1:
			surfaceHolder.addCallback(mPlayer1Holder);
			break;
		case PLAYER2:
			surfaceHolder.addCallback(mPlayer2Hold);
			break;
		}
		surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
	}

	/**
	 * startPlaying on the player
	 * 
	 * @param player
	 *            Main or the second
	 * @param serviceID
	 *            the serviceID get from the channel item
	 */
	public void startPlaying(int player, String serviceID, long serviceReferenceId, int freq) {
		if (player == PLAYER1) {
			sPlayBackState = State.PlaybackLoading;
		} else {
			sPlayer2State = State.PlaybackLoading;
		}
		if (player == mMainPlayer) {
			mLastMainServiceId = serviceID;
			mLastMainServiceReferenceId = serviceReferenceId;
			mLastMainFreq = freq;
		} else {
			mLastViceServiceId = serviceID;
			mLastViceServiceReferenceId = serviceReferenceId;
			mLastViceFreq = freq;
		}
		onPlayStateChaned(player);
		IsmaPara para = null;
		ServiceParamItem serviceParam = null;
		serviceParam = getServiceItem(serviceID, freq);
		para = getIsma(serviceID, serviceReferenceId);
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "serviceId is "
				+ para.getServiceID() + " serviceReferenceId is " + serviceReferenceId, LogMask.APP_COMMON);
		if (serviceParam == null || para == null) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "serviceParam is "
					+ serviceParam + "ismaPara is " + para, LogMask.APP_COMMON);

			if(sCurrentMode==MBBMS_MODE && !isDataGet ){
			    bNeedUpdateTS0 = true;
			    updateTS0();
	            prepareTimeOut(player);
			}else{
	           if (mMediaListener != null) {
                  Message msg = mMediaListener.obtainMessage();
                  msg.what = MESSAGE_PLAYBACK_ERROR;
                  msg.arg1 = player;
                  mMediaListener.sendMessage(msg);
	           }
			}
			return;
		}
		MBBMSEngine.getInstance().stopTVService(player);
		prepareTimeOut(player);
		authorizeCount = 1;
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "SaltKey length is "
				+ para.getSKLength1(), LogMask.APP_MAIN);
		MBBMSEngine.getInstance().startTVService(player,
				serviceParam.getFrequecy(), serviceParam.getStartTs(),
				serviceParam.getCountTs(), serviceParam.getDemod(), para);
		if (!getPlayer(player).isInitialized()) {
			getPlayer(player).init(
					MBBMSEngine.getInstance().getTVServiceUrl(player));
		}
		getPlayer(player).play();
	}

	private void prepareTimeOut(int player) {
		if (player == PLAYER1) {
			mServiceHandler.removeMessages(PLAYER1TIMEOUT);
			mServiceHandler.sendEmptyMessageDelayed(PLAYER1TIMEOUT, 10 * 1000);
		} else if (player == PLAYER2) {
			mServiceHandler.removeMessages(PLAYER2TIMEOUT);
			mServiceHandler.sendEmptyMessageDelayed(PLAYER2TIMEOUT, 10 * 1000);
		}
	}

	private void keepTimeout() {
		if (mServiceHandler.hasMessages(PLAYER1TIMEOUT)) {
			mServiceHandler.removeMessages(PLAYER1TIMEOUT);
			mServiceHandler.sendEmptyMessageDelayed(PLAYER1WAITAUTHORIZE, 60 * 1000);
		}
		if (mServiceHandler.hasMessages(PLAYER2TIMEOUT)) {
			mServiceHandler.removeMessages(PLAYER2TIMEOUT);
			mServiceHandler.sendEmptyMessageDelayed(PLAYER2WAITAUTHORIZE, 60 * 1000);
		}
	}

	private void prepareTimeoutByAuthorize() {
		if (mServiceHandler.hasMessages(PLAYER1WAITAUTHORIZE)) {
			mServiceHandler.removeMessages(PLAYER1WAITAUTHORIZE);
			prepareTimeOut(PLAYER1);
		}
		if (mServiceHandler.hasMessages(PLAYER2WAITAUTHORIZE)) {
			mServiceHandler.removeMessages(PLAYER2WAITAUTHORIZE);
			prepareTimeOut(PLAYER2);
		}
	}

	private void removePrepareTimeoutByauthorize() {
		mServiceHandler.removeMessages(PLAYER1WAITAUTHORIZE);
		mServiceHandler.removeMessages(PLAYER2WAITAUTHORIZE);
	}

	private void removeTimeOutMessage(int player) {
		if (player == PLAYER1) {
			mServiceHandler.removeMessages(PLAYER1TIMEOUT);
		} else if (player == PLAYER2) {
			mServiceHandler.removeMessages(PLAYER2TIMEOUT);
		}
	}

	private IsmaPara getIsma(String serviceId, long serviceReferenceId) {
		IsmaparaItem paraItem = mResolver.getIsmaPara(serviceId, serviceReferenceId);
		IsmaPara para = new IsmaPara();
		if (paraItem != null) {
			para.setIsBase64((char) paraItem.getIsBase64());
			para.setMBBMSECMDataType((char) paraItem.getMBBMSECMDataType());
			para.setISMACrypSalt1(paraItem.getISMACrypSalt1());
			para.setISMACrypSalt2(paraItem.getISMACrypSalt2());
			para.setSKLength1((char) paraItem.getSKLength1());
			para.setSKLength2((char) paraItem.getSKLength2());
			para.setServiceID(Integer.parseInt(paraItem.getServiceID()));
		} else {
			para.setIsBase64((char) 0);
			para.setMBBMSECMDataType((char) 0);
			para.setISMACrypSalt1("");
			para.setISMACrypSalt2("");
			para.setSKLength1((char) 0);
			para.setSKLength2((char) 0);
			para.setServiceID(Integer.parseInt(serviceId));
		}
		return para;
	}

	private ServiceParamItem getServiceItem(String serviceId, int freq) {
		if (serviceId == null)
			return null;
		ServiceParamItem serviceParam = null;
		serviceParam = mResolver.getServiceParam(Long.parseLong(serviceId), freq);
		return serviceParam;
	}

	/**
	 * stopPlaying on the player
	 * 
	 * @param player
	 *            Main or the second
	 */
	public void stopPlaying(int player) {
		removeTimeOutMessage(player);
		if (player == PLAYER1) {
			sPlayBackState = State.PlaybackStoped;
		} else {
			sPlayer2State = State.PlaybackStoped;
		}
		onPlayStateChaned(player);
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "stop playing",
				LogMask.APP_MAIN);
		getPlayer(player).stop();
		MBBMSEngine.getInstance().stopTVService(player);
		authorizeCount = 0;
	}

	public void setMediaListener(Handler handler) {
		mMediaListener = handler;
		getPlayer(PLAYER1).setOnPreparedListener(mPlayer1PreparedListener);
		getPlayer(PLAYER2).setOnPreparedListener(mPlayer2PreparedListener);
		getPlayer(PLAYER1).setOnErrorListener(mPlayer1ErrorListener);
		getPlayer(PLAYER2).setOnErrorListener(mPlayer2ErrorListener);
	}

	public void onModeExchange() {
		if (isLoadingExit)
			return;
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "change to mbbms mode",
				LogMask.APP_MAIN);
		Intent intent = new Intent();
		ComponentName comp = new ComponentName("com.marvell.cmmb",
				"com.marvell.cmmb.view.dialog.ChangeToMBBMSDialog");
		intent.setComponent(comp);
		intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK
				| Intent.FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);
		startActivity(intent);
	}

	public void setMute(int player, boolean isMute) {
		getPlayer(player).setMute(isMute);
		if (isMute && player == mMainPlayer) {
			showMuteToast(player);
		}
	}

	public void showMuteToast(int player) {
		if (mMuteTost == null) {
			mMuteTost = new Toast(this);
			mMuteTost.setDuration(Toast.LENGTH_SHORT);
			mMuteTost.setGravity(Gravity.CENTER, 0, 0);
			ImageView im = new ImageView(this);
			im.setImageResource(R.drawable.sound_muted);
			mMuteTost.setView(im);
		}
		mMuteTost.show();
	}

	public boolean isMute(int player) {
		return getPlayer(player).isMute();
	}

	public static State getMainPlayerState() {
		return mMainPlayer == PLAYER1 ? sPlayBackState : sPlayer2State;
	}

	public static State getVicePlayerState() {
		return mVicePlayer == PLAYER1 ? sPlayBackState : sPlayer2State;
	}

	private void setOperationtype(int type) {
		mModeManager.setOperationType(type);
		mOperateList.clear();
		switch (type) {
		case STARTUP:
			mOperateList.put(OPERATION_UPDATE_SG, UNDONE);
			mOperateList.put(OPERATION_PARSE_SG, UNDONE);
			mOperateList.put(OPERATION_UPDATE_ST, UNDONE);
			mOperateList.put(OPERATION_PARSE_ST, UNDONE);
			mOperateList.put(OPERATION_INIT_GBA, UNDONE);
			mOperateList.put(OPERATION_UPDATE_TS0, UNDONE);
			break;

		case AUTOREFRESH:
		case MANUALREFRESH:
			mOperateList.put(OPERATION_UPDATE_SG, UNDONE);
			mOperateList.put(OPERATION_PARSE_SG, UNDONE);
			mOperateList.put(OPERATION_UPDATE_ST, UNDONE);
			mOperateList.put(OPERATION_PARSE_ST, UNDONE);
			mOperateList.put(OPERATION_INIT_GBA, UNDONE);
			mOperateList.put(OPERATION_UPDATE_TS0, UNDONE);
			break;
		default:
			break;
		}

	}

	public void setCityName(String name) {
		mCityName = name;
	}

	public String getCityName() {
		return mCityName;
	}

	public void setAuthorizeId(String channelId) {
		mChannelId = channelId;
	}

	public String getAuthorizeId() {
		return mChannelId;
	}
	
	public void setUpdateSTStatus(boolean status) {
		isUpdateStforsubscribe = status;
	}
	
	public boolean getUpdateSTStatus() {
		return isUpdateStforsubscribe;
	}

	public void setPurchaseIds(String[] purchaseIds) {
		mPurchaseIds = purchaseIds;
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "purchase size is "
				+ mPurchaseIds.length + " purchaseId is " + mPurchaseIds[0],
				LogMask.APP_MAIN);
	}

	public ArrayList<PurchaseInfoItem> getInquiryList() {
		return mInquiryList;
	}

	public void onPlayStateChaned(int player) {
		if (sPlayBackState != State.PlaybackStoped
				|| sPlayer2State != State.PlaybackStoped) {
			mSignalManager.active();
			mSignalManager.setRemoteActivity(SignalManager.PLAYER);
		} else {
			mSignalManager.hungUp();
		}
		if (player == mMainPlayer
				&& getMainPlayerState() == State.PlaybackPlaying) {
//			registAudioFocus(); //register audioFocus again,
			if(AudioFocusManager.sInstance != null){
				AudioFocusManager.sInstance.requestAudioListener();
			}
		}
//		if (player == mMainPlayer
//				&& getMainPlayerState() == State.PlaybackStoped) {
//			releaseAudioFocusManager();
//		}

		if (player == mVicePlayer
				&& getVicePlayerState() == State.PlaybackPlaying) {
			setMute(MBBMSService.mVicePlayer, true);
		}
//		if(player == mMainPlayer
//				&& getMainPlayerState() == State.PlaybackLoading) {//request the audioFocus at loading process
//			registAudioFocus();
//		}
		if (player == mMainPlayer
			&& getMainPlayerState() == State.PlaybackPlaying) {
			{
				int state=getPhoneState();
				if(state==mTelephonyMgr.CALL_STATE_OFFHOOK||state==mTelephonyMgr.CALL_STATE_RINGING){
					if (MBBMSService.sInstance.mMediaListener != null) {
						MBBMSService.sInstance.mMediaListener
								.sendEmptyMessage(Define.MESSAGE_MUTE);
					}
				}
			}
		}
		
		sendBroadcast(new Intent().setAction(ACTION_ON_PLAYSTATE_CHANGE));
	}

	public Toast getShortPromptToast(int resid) {
		if (mShortPromptToast == null) {
			mShortPromptToast = Toast.makeText(this, resid, Toast.LENGTH_SHORT);
		}
		mShortPromptToast.setText(resid);
		return mShortPromptToast;
	}

	public Toast getLongPromptToast(int resid) {
		if (mLongPromptToast == null) {
			mLongPromptToast = Toast.makeText(this, resid, Toast.LENGTH_LONG);
		}
		mLongPromptToast.setText(resid);
		return mLongPromptToast;
	}
   
	/**
	 * note :this AudioFocus can be request more times,if the current app is on top of focusStack,it will 
	 *       do nothing, or else, it will request again and remove the former.
	 */
	public void registAudioFocus() {
		mAudioFocusManager.registerReceiver();
	}

	private boolean duplicateEbMsg(EmergencyMessage msg) {
		if (msg != null) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "emergency message is "
					+ msg.getText(), LogMask.APP_MAIN);
			mEmergencyMessageList = mResolver.emergencyQuery();
			for (int i = 0; i < mEmergencyMessageList.size(); i++) {
				if ((mEmergencyMessageList.get(i).getNetLevel() == msg.getNetLevel())
						&& (mEmergencyMessageList.get(i).getNetId() == msg.getNetID())
						&& (mEmergencyMessageList.get(i).getMsgId() == msg.getMsgID())
						&& (mEmergencyMessageList.get(i).getEmergencyDate().equals(msg.getNYear()
								+ "-" + msg.getNMonth() + "-" + msg.getNDay()))
						&& (mEmergencyMessageList.get(i).getEmergencyTime().equals(msg.getNHour()
								+ ":" + msg.getNMinute() + ":" + msg.getNsecond()))
						&& (mEmergencyMessageList.get(i).getEmergencyLevel() == msg.getMsgLevel())) {
					return true;
				}
			}
		}
		return false;
	}

	private void storeEmergencyMessage(EmergencyMessage msg) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
				"start store emergency message", LogMask.APP_MAIN);
		if (msg != null) {
			String date = msg.getNYear() + "-" + msg.getNMonth() + "-" + msg.getNDay();
			String time = msg.getNHour() + ":" + msg.getNMinute() + ":" + msg.getNsecond();
			mEmergencyVisbleList = mResolver.emergencyQuery(DELETE_FLAG_FALSE);
			if (mEmergencyVisbleList.size() >= 20) {
				long[] emergencyId = new long[mEmergencyVisbleList.size()];
				for (int i = 0; i < mEmergencyVisbleList.size(); i++) {
					emergencyId[i] = mEmergencyVisbleList.get(i).getEmergencyId();
				}
				EmergencyItem item = mResolver.getEmergencyItem(getMinNumber(emergencyId));
				mResolver.updateEmergencyById(DELETE_FLAG_TRUE, item
						.getNetLevel(), item.getNetId(), item.getMsgId(), 
						item.getEmergencyDate(), item.getEmergencyTime(),
						item.getEmergencyLevel());
			}
			if(msg.getMsgLevel() > 0 && msg.getMsgLevel() < 3) {
				mResolver.insertEmergency(msg.getNetLevel(), msg.getNetID(), msg
						.getMsgID(), date, time, msg.getMsgLevel(), msg.getText(),
						0, msg.getDuration(), 1);
			} else {
				mResolver.insertEmergency(msg.getNetLevel(), msg.getNetID(), msg
						.getMsgID(), date, time, msg.getMsgLevel(), msg.getText(),
						0, msg.getDuration(), 0);
				ArrayList<EmergencyItem> al = mResolver.emergencyQueryByReadFlag(0);
				if(al.size()>0)
				mSignalManager.showUnReadEbmsgNot(al.size());
			}			
			ifStopPlaying(msg.getMsgLevel());
			sendEmergencyMessage(msg);
			msg = null;
		} else {
			LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
					"emergency message is null", LogMask.APP_MAIN);
		}
	}

	private void ifStopPlaying(int level) {
		if (level > 0 && level < 3) {
			// stop playing
			if (getMainPlayerState() != State.PlaybackStoped) {
				isEbMsgStopMainPlayer = true;
				if(PlayingActivity.sInstance!=null){
				    PlayingActivity.sInstance.stop(mMainPlayer);
				}else{
				    stopPlaying(mMainPlayer);
				}
			}
			if (getVicePlayerState() != State.PlaybackStoped) {
				isEbMsgStopVicePlayer = true;
                if(PlayingActivity.sInstance!=null){
                    PlayingActivity.sInstance.stop(mVicePlayer);
                }else{
                    stopPlaying(mVicePlayer);
                }
			}
		}
	}

	public void resumeToPlay() {
		if (isEbMsgStopMainPlayer) {
            if(PlayingActivity.sInstance!=null){
                PlayingActivity.sInstance.play(mMainPlayer);
            }else{
                startPlaying(mMainPlayer, mLastMainServiceId, mLastMainServiceReferenceId, mLastMainFreq);
            }
			isEbMsgStopMainPlayer = false;
		}
		if (isEbMsgStopVicePlayer) {
            if(PlayingActivity.sInstance!=null){
                PlayingActivity.sInstance.play(mVicePlayer);
            }else{
                startPlaying(mVicePlayer, mLastViceServiceId, mLastViceServiceReferenceId, mLastViceFreq);
            }
			isEbMsgStopVicePlayer = false;
		}

	}

	private void sendEmergencyMessage(EmergencyMessage msg) {
		Intent ebmsg = new Intent();
		String date = msg.getNYear() + "-" + msg.getNMonth() + "-"
				+ msg.getNDay();
		String time = msg.getNHour() + ":" + msg.getNMinute() + ":"
				+ msg.getNsecond();
		ebmsg.putExtra(INTENT_MESSAGE_NETLEVEL, msg.getNetLevel());
		ebmsg.putExtra(INTENT_MESSAGE_NETID, msg.getNetID());
		ebmsg.putExtra(INTENT_MESSAGE_MSGID, msg.getMsgID());
		ebmsg.putExtra(INTENT_MESSGAE_DATE, date);
		ebmsg.putExtra(INTENT_MESSGAE_TIME, time);
		ebmsg.putExtra(INTENT_MESSGAE_LEVEL, msg.getMsgLevel());
		ebmsg.putExtra(INTENT_MESSGAE_TEXT, msg.getText());
		ebmsg.setAction(ACTION_VIEW_EBMESSAGE);
		ebmsg.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		startActivity(ebmsg);
	}

	private long getMinNumber(long[] num) {
		long min = 0;
		if (num.length > 0) {
			min = num[0];
		}
		for (int i = 0; i < num.length; i++) {
			if (min > num[i]) {
				min = num[i];
			} else {
				min = min;
			}
		}
		return min;
	}

	private synchronized void saveParserResult() {
		mFreqCount = mFreqCount - 1;
		ESGParser parser = ESGParser.getInstance(getBaseContext());
		parser.mergeProgram();
		parser.updateCMMBChannelList(mESGParamList.get(mFreqCount)
				.getFrequecy());
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
				"mFreqCount is " + mFreqCount, LogMask.APP_COMMON);
	}

	private void isCompleted() {
		if (isMultiFreq()) {
			updateESG();
		} else {
			isDataGet = true;
			resetESGXmlCount();
			notifyUIESGParsed(true);
		}
	}

	private boolean isMultiFreq() {
		if (mFreqCount != 0) {
			return true;
		} else {
			return false;
		}
	}

	private void resetESGXmlCount() {
		mESGXmlCount = 0;
	}

	private void notifyUIESGParsed(boolean result) {
		Intent esgIntent = new Intent();
		esgIntent.setAction(ACTION_ESG_PARSED);
		esgIntent.putExtra(ACTION_ESG_PARSED, result);
		sendBroadcast(esgIntent);
	}

	private boolean isServiceParam() {

		if (mServiceParamMap.isEmpty()) {
			return false;
		}
		return true;
	}

	private void notifyUIScanResult(boolean result) {
		Intent esgIntent = new Intent();
		esgIntent.setAction(ACTION_SCAN_FINISHED);
		esgIntent.putExtra(ACTION_SCAN_FINISHED, result);
		sendBroadcast(esgIntent);
	}

	private void notifyUIESGUpdate(boolean result) {
		Intent esgIntent = new Intent();
		esgIntent.setAction(ACTION_ESG_UPDATED);
		esgIntent.putExtra(ACTION_ESG_UPDATED, result);
		sendBroadcast(esgIntent);
	}

	private void removeOperItem(String key) {
		if (mOperateList.get(key) != null) {
			mOperateList.remove(key);
		}

	}

	private boolean checkCurrentFreq() {
		String lastFreq = mFreqPrefer.getString("CurrentFreq", "39");
		String[] lastFreqs = lastFreq.split(",");
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
				"mFreqList.size() is " + mFreqList.size()
						+ " lastFreqs.length is " + lastFreqs.length,
				LogMask.APP_COMMON);
		if (mFreqList.size() != lastFreqs.length) {
			return true;
		} else {
			for (int i = 0; i < mFreqList.size(); i++) {
				if (mFreqList.get(i) != Integer.parseInt(lastFreqs[i])) {
					return true;
				}
			}
			return false;
		}
	}

	private void clearOldESG() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "start clear old esg",
				LogMask.APP_MAIN);
		String lastFreq = mFreqPrefer.getString("CurrentFreq", "39");
		String[] lastFreqs = lastFreq.split(",");
		for (int i = 0; i < lastFreqs.length; i++) {
			mResolver.deleteChannel(CMMB_MODE, Integer.parseInt(lastFreqs[i]));
			mResolver.deleteProgram(CMMB_MODE, Integer.parseInt(lastFreqs[i]));
			mResolver.deleteServiceParam(Integer.parseInt(lastFreqs[i]));
		}
	}

	private void resetFreqPreference() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "reset frequency preference",
				LogMask.APP_COMMON);
		SharedPreferences.Editor editor = mFreqPrefer.edit();
		String freq = "";
		if (mFreqList.size() == 0) {
			freq = "39";
		} else {
			for (int i = 0; i < mFreqList.size(); i++) {
				if (i != mFreqList.size() - 1) {
					freq = String.valueOf(mFreqList.get(i)) + ",";
				} else {
					freq = String.valueOf(mFreqList.get(i));
				}
			}
		}
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
				"current freqency is " + freq, LogMask.APP_COMMON);
		editor.putString("CurrentFreq", freq);
		editor.commit();
	}

	public void cancelSetMode() {
		isLoadingExit = true;
	}

	public boolean isOperationFinish() {
		printAllOperate();
		Iterator iter = mOperateList.entrySet().iterator();
		while (iter.hasNext()) {
			Map.Entry<String, Integer> entry = (Map.Entry<String, Integer>) iter.next();
			if (entry.getValue() == MBBMSService.UNDONE) {
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
						"operation undone is: " + entry.getKey(),
						LogMask.APP_COMMON);
				return false;
			}
		}
		return true;

	}

	public void printAllOperate() {// this is for debugging

		Iterator iter = mOperateList.entrySet().iterator();
		while (iter.hasNext()) {
			Map.Entry<String, Integer> entry = (Map.Entry<String, Integer>) iter.next();
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "operation result : "
					+ entry.getKey() + " : " + entry.getValue() + "\n",
					LogMask.APP_COMMON);
		}
	}

	/**
	 * All of these below are MBBMSEngine callback
	 */
	public void onRequestAuthorizeService(boolean done, int resCode) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
				"onRequestAuthorizeService, result is " + done + " resCode is " + resCode,
				LogMask.APP_MAIN);
		isAuthorizing = false;
		Intent authorizeIntent = new Intent();
		authorizeIntent.setAction(ACTION_AUTHORIZED);
		authorizeIntent.putExtra("AuthorizeResult", done);
		authorizeIntent.putExtra("resCode", resCode);
		sendBroadcast(authorizeIntent);
		stopWapConnectivity();
		if (done) {
			Util.setItem(mOperateList, Define.OPERATION_AUTHORIZE, SUCCESS);
		} else {
			Util.setItem(mOperateList, Define.OPERATION_AUTHORIZE, FAIL);
		}
		prepareTimeoutByAuthorize();
	}

	public void onCaInfo(CaInfo[] ca, int size, int freq) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "onCaInfo", LogMask.APP_MAIN);
		mServiceHandler.sendMessage(mServiceHandler.obtainMessage(
				MESSAGE_CAINFO, size, freq, ca));
	}

	public void onESGUpdated(boolean done) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "onESGUpdated, result is "
				+ done, LogMask.APP_MAIN);
		isUpdatingESG = false;
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "xml file count is "
				+ mESGXmlCount, LogMask.APP_MAIN);
		if (mESGXmlCount == 0 || done == false) {
			notifyUIESGUpdate(false);
		} else if (done == true && mESGXmlCount != 0) {
			notifyUIESGUpdate(true);
			if (Util.isItemSetValue(ESGXMLParsedMap, String
					.valueOf(mESGXmlCount), XML_PARSED)) {
				saveParserResult();
				isCompleted();
			}
		}
	}

	public void onESGXml(int datatype, String fname) {
		mESGXmlCount = mESGXmlCount + 1;
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "onESGxml, fname is " + fname,
				LogMask.APP_MAIN);
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "mFreqCount is " + mFreqCount
				+ " mESGXmlCount is " + mESGXmlCount, LogMask.APP_COMMON);
		if (mFreqCount >= 1) {
			startESGParser(datatype, fname, mESGParamList.get(mFreqCount - 1)
					.getFrequecy(), mESGXmlCount);
		}
	}

	public void onEmergencyMessage(EmergencyMessage message) {
		mServiceHandler.sendMessage(Message.obtain(mServiceHandler, MESSAGE_EBMESSAGE_RECEIVED, message));
		message = null;
	}

	public void onEvent(int type, int param) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onEvent called, type is "
				+ type + " param is " + param, LogMask.APP_COMMON);
		mServiceHandler.sendMessage(Message.obtain(mServiceHandler,
				MESSAGE_ON_ERROR, type, param));
	}

	public void onRequestInitGBA(boolean done, int resCode) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "onRequestInitGBA, result is "
				+ done + " resCode is " + resCode, LogMask.APP_MAIN);
		isGBAInited = false;
		if (!isTestMode) {
			if (done) {
				Util.setItem(mOperateList, OPERATION_INIT_GBA, SUCCESS);
			} else {
				Util.setItem(mOperateList, OPERATION_INIT_GBA, FAIL);
			}
		} else {
			LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
					"initgba for interface test finish, result is " + done,
					LogMask.APP_MAIN);
		}
		Intent gbaInitialized = new Intent();
		gbaInitialized.setAction(ACTION_GBA_INIT);
		gbaInitialized.putExtra(ACTION_GBA_INIT, done);
		gbaInitialized.putExtra("resCode", resCode);
		sendBroadcast(gbaInitialized);
		stopWapConnectivity();

	}

	public void onRequestInquiryAccount(boolean done, String inquiryXml, int resCode) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
				"onRequestInquiryAccount, result is " + done + " resCode is " + resCode, LogMask.APP_MAIN);
		isInquiring = false;
		stopWapConnectivity();
		if (done) {
			startInquiryParser(inquiryXml);
		}
		Intent inquiryIntent = new Intent();
		inquiryIntent.setAction(ACTION_INQUIRIED);
		inquiryIntent.putExtra(ACTION_INQUIRIED, done);
		inquiryIntent.putExtra("resCode", resCode);
		sendBroadcast(inquiryIntent);
		stopWapConnectivity();
	}

	public void onRequestUpdateSG(boolean done, String sgddXml, String sgXml, int resCode) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "onRequestUpdateSG, result is "
				+ done + " sgddXml is " + sgddXml + " sgXml is " + sgXml + " resCode is " + resCode,
				LogMask.APP_MAIN);
		isUpdatingSG = false;
		if(done) {
			isSGUpdateFailed = false;
			Util.setItem(mOperateList, OPERATION_UPDATE_SG, SUCCESS);
			startSGParser(sgddXml, sgXml);
		} else {
			isSGUpdateFailed = true;
			if(!isTestMode) {
				isUpdateSTFinished = true;
				Util.setItem(mOperateList, OPERATION_UPDATE_SG, FAIL);
				Util.setItem(mOperateList, OPERATION_PARSE_SG, FAIL);
				Util.setItem(mOperateList, OPERATION_UPDATE_ST, FAIL);
				Util.setItem(mOperateList, OPERATION_PARSE_ST, FAIL);
				Util.setItem(mOperateList, OPERATION_INIT_GBA, FAIL);
				updateTS0();
				stopWapConnectivity();
			} else {
				LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
						"onRequestUpdateSG for interface test, result is " + done
								+ " sgddXml is " + sgddXml + " sgXml is " + sgXml,
						LogMask.APP_MAIN);
			}
		}
//		if (!isTestMode) {
//			if (done) {
//				Util.setItem(mOperateList, OPERATION_UPDATE_SG, SUCCESS);
//				startSGParser(sgddXml, sgXml);
//			} else {
//				isUpdateSTFinished = true;
//				Util.setItem(mOperateList, OPERATION_UPDATE_SG, FAIL);
//				Util.setItem(mOperateList, OPERATION_PARSE_SG, FAIL);
//				Util.setItem(mOperateList, OPERATION_UPDATE_ST, FAIL);
//				Util.setItem(mOperateList, OPERATION_PARSE_ST, FAIL);
//				Util.setItem(mOperateList, OPERATION_INIT_GBA, FAIL);
//				updateTS0();
//				stopWapConnectivity();
//			}
//		} else {
//			LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
//					"onRequestUpdateSG for interface test, result is " + done
//							+ " sgddXml is " + sgddXml + " sgXml is " + sgXml,
//					LogMask.APP_MAIN);
//		}
		Intent sgUpdate = new Intent();
		sgUpdate.setAction(ACTION_SG_UPDATED);
		sgUpdate.putExtra(ACTION_SG_UPDATED, done);
		sgUpdate.putExtra("resCode", resCode);
		MBBMSService.this.sendBroadcast(sgUpdate);

	}

	public void onScanFinished(boolean done) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "onScanFinished, result is "
				+ done, LogMask.APP_MAIN);
		isUpdateTS0 = false;
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "mFreqList size is "
				+ mFreqList.size(), LogMask.APP_COMMON);
		if (done && isServiceParam()) {
			if (sCurrentMode == CMMB_MODE) {
				boolean isFreqChanged = checkCurrentFreq();
				if (isFreqChanged) {
					clearOldESG();
					resetFreqPreference();
				}
				saveServiceParam(mFreqList);
				if (!isScanCurrentFrequency) {
				    /*
				     * update esg anyway 11.02
				     */
					notifyUIScanResult(true);
					getESGParam();
					if(!updateESG()) {
						notifyUIESGUpdate(false);
                    }
					isScanCurrentFrequency = true;
				} else {
					notifyUIScanResult(true);
					getESGParam();
					if(!updateESG()) {
						notifyUIESGUpdate(false);
                    }
				}
			} else if (sCurrentMode == MBBMS_MODE) {
				isDataGet = true;
				Util.setItem(mOperateList, OPERATION_UPDATE_TS0, SUCCESS);
				mServiceHandler.sendEmptyMessage(MESSAGE_PLAY_AFTER_UPDATETS0);
			}
		} else {
			if (sCurrentMode == CMMB_MODE) {
				if (!isScanCurrentFrequency) {
					isScanCurrentFrequency = true;
					startScan();
				} else {
					notifyUIScanResult(false);
				}
			} else if (sCurrentMode == MBBMS_MODE) {
				isDataGet = false;
				Util.setItem(mOperateList, OPERATION_UPDATE_TS0, FAIL);
			}
		}

	}

	public void onServiceParam(ServiceParam service) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
				"onServiceParam, service name is " + service.getServiceName(),
				LogMask.APP_COMMON);
		ServiceFreqItem freqItem = new ServiceFreqItem();
		freqItem.setFrequency(service.getFreq());
		freqItem.setServiceId(service.getServiceid());
		for (ServiceFreqItem item : mServiceParamMap.keySet()) {
			if (freqItem.getFrequency() == item.getFrequency()
					&& freqItem.getServiceId() == item.getServiceId()) {
				freqItem = null;
				break;
			}
		}

		if (freqItem != null) {
			mServiceParamMap.put(freqItem, service);
			freqItem = null;
			service = null;
		}
	}

	public void onRequestSubscribeService(boolean done, int resCode) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
				"onRequestSubscribeService, result is " + done + " resCode is " + resCode,
				LogMask.APP_MAIN);
		isSubscribing = false;
		Intent intent = new Intent();
		intent.setAction(ACTION_SUBSCRIBE_FINISH);
		intent.putExtra("SubscribeResult", done);
		intent.putExtra("resCode", resCode);
		sendBroadcast(intent);
		stopWapConnectivity();
	}

	public void onRequestUpdateST(boolean done, String stXml, int resCode) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "onRequestUpdateST, result is "
				+ done + " stXml is " + stXml + " resCode is " + resCode, LogMask.APP_MAIN);
		isUpdatingST = false;
		if(done) {
			startSTParser(stXml);
			Util.setItem(mOperateList, OPERATION_UPDATE_ST, SUCCESS);
		} else {
			if(!isTestMode) {
				isUpdateSTFinished = true;
				Util.setItem(mOperateList, OPERATION_UPDATE_ST, FAIL);
				Util.setItem(mOperateList, OPERATION_PARSE_ST, FAIL);
				Util.setItem(mOperateList, OPERATION_INIT_GBA, FAIL);
				stopWapConnectivity();
			} else {
				LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
						"onRequestUpdateST for interface test, result is " + done
								+ " stXml is " + stXml + " resCode is " + resCode, LogMask.APP_MAIN);
			}
		}
//		if (!isTestMode) {
//			if (done) {
//				startSTParser(stXml);
//				Util.setItem(mOperateList, OPERATION_UPDATE_ST, SUCCESS);
//			} else { // update ST fail ,remove the following operation;
//				isUpdateSTFinished = true;
//				Util.setItem(mOperateList, OPERATION_UPDATE_ST, FAIL);
//				Util.setItem(mOperateList, OPERATION_PARSE_ST, FAIL);
//				Util.setItem(mOperateList, OPERATION_INIT_GBA, FAIL);
//				stopWapConnectivity();
//			}
//		} else {
//			LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
//					"onRequestUpdateST for interface test, result is " + done
//							+ " stXml is " + stXml + " resCode is " + resCode, LogMask.APP_MAIN);
//		}
		Intent stUpdate = new Intent();
		stUpdate.setAction(ACTION_ST_UPDATED);
		stUpdate.putExtra(ACTION_ST_UPDATED, done);
		stUpdate.putExtra("resCode", resCode);
		sendBroadcast(stUpdate);
	}

	public void onTS0Parsed(int freq) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "onTS0Parsed, freq is " + freq,
				LogMask.APP_MAIN);
		mFreqList.add(freq);
		mServiceHandler.sendMessage(mServiceHandler.obtainMessage(
				MESSAGE_SAVE_SERVICEPARAM, freq, 0));
	}

	public void onRequestUnsubscribeService(boolean done, int resCode) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
				"onRequestUnsubscribeService, result is " + done + " resCode is " + resCode,
				LogMask.APP_MAIN);
		isUnsubscribing = false;
		Intent intent = new Intent();
		intent.setAction(ACTION_UNSUBSCRIBE_FINISH);
		intent.putExtra("SubscribeResult", done);
		intent.putExtra("resCode", resCode);
		sendBroadcast(intent);
		stopWapConnectivity();
	}

	public void onSignalUpdated(SignalStatus status) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "onSignalUpdated, level is "
				+ status.getLevel() + " strength is " + status.getStrength(),
				LogMask.APP_MAIN);
		mSignalManager.setSignalIconByStrength(status);
	}

	public boolean isSupportPip() {
		if (!isGotSupportedSession) {
			int rt = MBBMSEngine.getInstance().getMaxSupportedSessions();
			isSupportPIP = rt > 1;
			isGotSupportedSession = true;
			LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "max support is " + rt
					+ " support pip is " + isSupportPIP, LogMask.APP_MAIN);
		}
		return isSupportPIP;
	}

	public void setNotifyDataChangeListener(
			NotifyDataChangeListener notifyDataChangeListener) {
		mNotifyDataChangeListener = notifyDataChangeListener;
	}

	private boolean isServiceExist() {
		ArrayList<ChannelItem> serviceList = Util.getChannelList(
				MBBMSService.sCurrentMode, this);
		if (serviceList.size() == 0) {
			return false;
		}
		return true;
	}

	public void binderDied() {
		Intent intent = new Intent();
		ComponentName comp = new ComponentName("com.marvell.cmmb",
				"com.marvell.cmmb.view.dialog.BinderDiedDialog");
		intent.setComponent(comp);
		intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK
				| Intent.FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);
		startActivity(intent);
	}
	/**
	 * @param type  indicate the output device, like earphone speaker,etc
	 * 
	 * 
	 */

	public void setForceUseSpeaker() {
		AudioSystem.setForceUse(AudioSystem.FOR_MEDIA, AudioSystem.FORCE_SPEAKER);
		isSpeakerOn = true;
	}
	public void setForceUseHeadPhone() {
		AudioSystem.setForceUse(AudioSystem.FOR_MEDIA,AudioSystem.FORCE_HEADPHONES);
		isSpeakerOn = false;
	}
	
	public void setForceUseNone() {	
		AudioSystem.setForceUse(AudioSystem.FOR_MEDIA, AudioSystem.FORCE_NONE);
//		isSpeakerOn = false;
		
	}
	public void resetSpeaker() {
		isSpeakerOn = false;
	}
	
	public void setHeadPhoneState(boolean isExist) {
		AppBase.isHeadPhonePlugged = isExist;
	}
	
	public boolean getHeadPhoneState() {
		return AppBase.isHeadPhonePlugged;
	}
	
	public int getPhoneState() {	
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "get callstate" + mTelephonyMgr.getCallState(), LogMask.APP_MAIN);
		return mTelephonyMgr.getCallState();
		
	}
	

	public class ApnChangeBroadcastRecieved extends BroadcastReceiver {
	    public void onReceive(Context context, Intent intent) {
	        String action = intent.getAction();
	        if (action.equals(ConnectivityManager.CONNECTIVITY_ACTION)){
	            
	        }else if(action.equals(TelephonyIntents.ACTION_ANY_DATA_CONNECTION_STATE_CHANGED)){
	            LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "ACTION_ANY_DATA_CONNECTION_STATE_CHANGED mModeManager.getCurrentAPN() "
                , LogMask.APP_COMMON);
	            mModeManager.getCurrentAPN();
	            if(isModeSeted && sCurrentMode==MBBMS_MODE){
	                LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "ACTION_ANY_DATA_CONNECTION_STATE_CHANGED setPreference()"
	                        , LogMask.APP_COMMON);
	                setPreference();
	            }
	        }
	    }
	}
	
	private class ApnChangeObserver extends ContentObserver
    {
        public ApnChangeObserver()
        {
            super(mServiceHandler);
        }
        
        public void onChange(boolean selfChange)
        {
            LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "ApnChangeObserver"
                    , LogMask.APP_COMMON);
            mServiceHandler.sendEmptyMessage(EVENT_APN_CHANGED);
        }
    }  

	private void onAPNChanged()
    {
        LogMask.LOGM(TAG,
                LogMask.LOG_APP_COMMON,
                LogMask.getLineInfo(),
                "onAPNChanged",
                LogMask.APP_COMMON);
        mModeManager.getCurrentAPN();
        if (isModeSeted && sCurrentMode == MBBMS_MODE)
        {
            LogMask.LOGM(TAG,
                    LogMask.LOG_APP_COMMON,
                    LogMask.getLineInfo(),
                    "onAPNChanged setPreference()",
                    LogMask.APP_COMMON);
            setPreference();
        }
    }
	
	private void playTVAfterUpdateTS0(){
	    if(bNeedUpdateTS0){
	        LogMask.LOGM(TAG,
                    LogMask.LOG_APP_COMMON,
                    LogMask.getLineInfo(),
                    "playTVAfterUpdateTS0()",
                    LogMask.APP_COMMON);
	        bNeedUpdateTS0 = false;
	        if(mServiceHandler.hasMessages(PLAYER1TIMEOUT)){
	            mServiceHandler.removeMessages(PLAYER1TIMEOUT);
	            if(PLAYER1==mMainPlayer)
	                startPlaying(mMainPlayer, mLastMainServiceId, mLastMainServiceReferenceId, mLastMainFreq);
	            else
	                startPlaying(mVicePlayer, mLastViceServiceId, mLastViceServiceReferenceId, mLastViceFreq);
	        }
	        if(mServiceHandler.hasMessages(PLAYER2TIMEOUT)){
	            mServiceHandler.removeMessages(PLAYER2TIMEOUT);
	            if(PLAYER2==mMainPlayer)
                    startPlaying(mMainPlayer, mLastMainServiceId, mLastMainServiceReferenceId, mLastMainFreq);
                else
                    startPlaying(mVicePlayer, mLastViceServiceId, mLastViceServiceReferenceId, mLastViceFreq);
	        }
	    }
	}
	
	public void enterCMMBMode(){
	    mModeManager.connectNetworkforModeExchange(MESSAGE_DATA_STATE_CHANGED_MODEEXCHANGE);
        isScanCurrentFrequency = isServiceExist();
        if (!isScanCurrentFrequency) {
            isScanCurrentFrequency = true;
            startScan();
        } else {
            scanFromCurrentFreq();
            isScanCurrentFrequency = false;
        }
	}
	
	/**
	 * check system has entered susbend state
	 */
	public int isNeedReset() {
		int result = MBBMSEngine.getInstance().needReset();
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "result is " + result, LogMask.APP_COMMON);
		return result;
	}
	
	private void registerHeadPhoneReceiver() {
		IntentFilter filter = new IntentFilter();
		filter.addAction(Intent.ACTION_HEADSET_PLUG);
		Receiver receiver = new Receiver();
		this.registerReceiver(receiver, filter);
		
	}
	private class Receiver extends BroadcastReceiver {
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();			
			if(action.equals(Intent.ACTION_HEADSET_PLUG)) {
				int state= intent.getIntExtra("state", 0);
				if(state == 0){// headphone is unplugged.
					setHeadPhoneState(false);				
					setForceUseNone();
					setSpeaker(true);
					showAlertDialog();
					if(PlayingActivity.sInstance != null) {
						if(PlayingActivity.sInstance.getMenuManager() != null) {
							PlayingActivity.sInstance.getMenuManager().setEnabled(3, false);//make it unable.
						}							
					}
					
				}else {
                    setHeadPhoneState(true);
                    setSpeaker(false);
						if(PlayingActivity.sInstance != null) {
							if(PlayingActivity.sInstance.getMenuManager() != null && PlayingActivity.sInstance.getMenuManager().isMenuInflated()) {
								PlayingActivity.sInstance.getMenuManager().setEnabled(3, true);
								PlayingActivity.sInstance.getMenuManager().updateMenu(3, R.string.speaker);
								PlayingActivity.sInstance.getMenuManager().updateIcon(3, R.drawable.icon_speaker);									
							}
						}
				}
			}
		}
	}
	private void showAlertDialog() {
		Intent intent = new Intent();
		ComponentName comp = new ComponentName("com.marvell.cmmb",
				"com.marvell.cmmb.view.dialog.NoHeadPhoneDialog");
		intent.setComponent(comp);
		intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK|Intent.FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);
		startActivity(intent);
	}
	private void setSpeaker(boolean isOn){
		isSpeakerOn = isOn;
	}

}
