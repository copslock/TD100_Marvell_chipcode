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

import java.io.File;
import java.io.FileInputStream;
import java.util.ArrayList;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Configuration;
import android.graphics.BitmapFactory;
import android.graphics.PixelFormat;
import android.graphics.drawable.BitmapDrawable;
import android.inputmethodservice.Keyboard.Key;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.ScaleGestureDetector;
import android.view.View;
import android.view.WindowManager;
import android.view.ScaleGestureDetector.OnScaleGestureListener;
import android.view.View.OnClickListener;
import android.view.ViewGroup.MarginLayoutParams;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.view.animation.Interpolator;
import android.view.animation.LayoutAnimationController;
import android.view.animation.ScaleAnimation;
import android.view.animation.TranslateAnimation;
import android.view.animation.Animation.AnimationListener;
import android.widget.AdapterView;
import android.widget.FrameLayout;
import android.widget.Gallery;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.FrameLayout.LayoutParams;

import com.marvell.cmmb.R;
import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.aidl.EmergencyMessage;
import com.marvell.cmmb.common.AppBase;
import com.marvell.cmmb.common.Define;
import com.marvell.cmmb.common.Util;
import com.marvell.cmmb.manager.ImageCaptureManager;
import com.marvell.cmmb.manager.MenuManager;
import com.marvell.cmmb.manager.ModeManager;
import com.marvell.cmmb.manager.VicePlayerManager;
import com.marvell.cmmb.manager.VideoRecorder;
import com.marvell.cmmb.manager.ImageCaptureManager.ImageCaptureListener;
import com.marvell.cmmb.resolver.ChannelItem;
import com.marvell.cmmb.service.MBBMSService;
import com.marvell.cmmb.service.MBBMSService.State;
import com.marvell.cmmb.view.adapter.ChannelAdapter;
import com.marvell.cmmb.view.customview.PlayFrameView;
import com.marvell.cmmb.view.customview.RecTime;
import com.marvell.cmmb.view.customview.PlayFrameView.OnScaleListener;
import com.marvell.cmmb.view.customview.PlayFrameView.PipListener;
import com.marvell.cmmb.view.customview.PlayFrameView.ScrollListener;
import com.marvell.cmmb.view.customview.ThumbnailView.ThumbnailClickListener;
import com.marvell.cmmb.view.customview.ThumbnailView;
import com.marvell.cmmb.view.dialog.CancelDialog;
import com.marvell.cmmb.view.dialog.CancelDialogCallback;
import com.marvell.cmmb.view.dialog.ConfirmDialog;
import com.marvell.cmmb.view.dialog.ConfirmDialogCallBack;

public class PlayingActivity extends AppBase implements OnClickListener,
		ScrollListener, PipListener, CancelDialogCallback,
		ConfirmDialogCallBack, OnScaleListener, ThumbnailClickListener,
		ImageCaptureListener, AnimationListener {
	private static final String TAG = "PlayingActivity";

	private ImageButton mPlayImgButton;

	private ImageButton mRecordImgButton;

	private ImageButton mPauseImgButton;

	private ImageButton mStopRecordImgButton;

	private TextView mChannelTextView;

	private Gallery mChannelGallery;

	private LinearLayout mTitleBar;

	private ImageView mIVPadUp;

	private ImageView mIVPadDown;

	private ThumbnailView mThumbnails;

	private LinearLayout mLLControlPanel;

	private MarginLayoutParams mGalleryParams;

	private MarginLayoutParams mControlLayoutParams;

	private ImageButton mMuteImgButton;

	private ImageButton mNoisyImgButton;

	private ImageButton mCaptureImgButton;

	private FrameLayout mPlaybackFrame;

	private PlayFrameView mPlayerFrame1;

	private PlayFrameView mPlayerFrame2;

	private PlayFrameView mMain;

	private PlayFrameView mVice;

	private static int sScreenOrientation;

	private boolean isFullScreen;// to set the screen type full screen when play or not

	private MenuManager mMenuManager;

	ArrayList<ChannelItem> mChannelList;

	private ChannelAdapter mChannelAdapter;

	public Intent mPlayIntent;

	private static boolean isChannelChanged;

	private boolean isOnNewIntent;

	private PlayReceiver mPlayReceiver;

	private CancelDialog mCancelDialog;

	private ConfirmDialog mConfirmDialog;

	private static final int MENU_FULL_SCREEN_INDEX = 0;// index

	private final String SERVICE_ID = "serviceId=";

	private final String FREQ = "freq=";

	private Animation mDisapearAnim;

	private Animation mAppearAnim;

	private Animation mScaleSmallAnim;

	private Animation mScaleBigAnim;

	private Animation mScaleDisappearAnim;

	private Animation mScaleAppearAnim;

	private Animation mControlDownAnim;

	private Animation mControlUpAnim;

	private float mLastXPoint = 0.0f;

	private float mLastYPoint = 0.0f;

	private boolean isAnimationEnd = true;
	
	private VicePlayerManager mVicePlayerManager;
	
	private int mLastFreq;
	
	private int mCurrentFreq;

	private static final int DISAPPEAR_PIPCONTROLER = 7001;
	private static final int ANIMATION_END = 7002;
	private boolean isNeedShowRecordDialog = false;
	private boolean isOnPaused = false;

	private Handler mHandler = new Handler() {
		private boolean isScanNeeded;
		
		private int counts = 0;

		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case MESSAGE_PLAY_ERROR:
				if (msg.arg1 == MBBMSService.PLAYER1) {
					mPlayerFrame1.changeState(State.PlaybackLoading);
				} else if (msg.arg1 == MBBMSService.PLAYER2) {
					mPlayerFrame2.changeState(State.PlaybackLoading);
				}
				getShortPromptToast(R.string.playbackexception).show();
				break;
			case MESSAGE_SETLAYERFORPIP:
				setLayerforPip();
				break;
			case MESSAGE_PLAY:
				if (msg.arg1 == MBBMSService.PLAYER1) {
					mPlayerFrame1.changeState(State.PlaybackPlaying);
				} else if (msg.arg1 == MBBMSService.PLAYER2) {
					mPlayerFrame2.changeState(State.PlaybackPlaying);
				}
				break;
			case MESSAGE_PLAY_BY_ID_FREQ:
				if (isServiceStarted && MBBMSService.isModeSeted) {
					if (MBBMSService.isDataGet) {
						Intent serviceIdandFreq = (Intent) msg.obj;
						writeServiceIdAndFreq(serviceIdandFreq);
						playByIntent(serviceIdandFreq);
						break;
					}
				}
				this.sendMessageDelayed(mHandler.obtainMessage(
						MESSAGE_PLAY_BY_ID_FREQ, msg.obj), 1000);
				break;
			case MESSAGE_MUTE:
				madeMute();
				break;
			case MESSAGE_UNMUTE:
				madeNoisy();
				break;
			case MESSAGE_STOP:
				if (MBBMSService.getMainPlayerState() != State.PlaybackStoped) {
					stop(MBBMSService.mMainPlayer);
				}
				if (MBBMSService.getVicePlayerState() != State.PlaybackStoped) {
					stop(MBBMSService.mVicePlayer);
				}
				break;
			case MESSAGE_CHANGE_FULL_SCREEN:
				changeSurfaceSize();
				updateMenu();
				break;
			case MESSAGE_PLAYBACK_ERROR:
				stop(msg.arg1);
				break;
			case MESSAGE_PREPARE_TIMEOUT:
				getShortPromptToast(R.string.weak_signal).show();
				stop(msg.arg1);
				break;
			case DISAPPEAR_PIPCONTROLER:
				if (mPIPControler != null
						&& mPIPControler.getVisibility() == View.VISIBLE) {
					mPIPControler.startAnimation(mControlUpAnim);
					mPIPControler.setVisibility(View.GONE);
				}
				break;
			case ANIMATION_END:
				switchFrame();
				updateViews();
				mMain.resetCMMBLOGO();
				mMain.changeState(MBBMSService.getMainPlayerState());
				mVice.resetCMMBLOGO();
				mVice.changeState(MBBMSService.getVicePlayerState());
				isAnimationEnd = true;
				break;
			default:
				break;
			}
			super.handleMessage(msg);
		}

	};

	private LinearLayout mPIPControler;

	private TextView mSwitchPip;

	private TextView mOpenPip;

	private TextView mFullScreen;

	private Interpolator mInterpolator;
	
	private String mMainPreviewInfo;
	
	private String mVicePreviewInfo;
	
	public static PlayingActivity sInstance;

	public void onCreate(Bundle savedInstanceState) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onCreate", LogMask.APP_COMMON);
		super.onCreate(savedInstanceState);
		setContentView(R.layout.player);
		sInstance = this;
		getIntentAction(getIntent());
		registerPlayReceiver();
		initViews();
		setupViews();
		init();

	}

	@Override
	protected void onPause() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onPause", LogMask.APP_COMMON);
		isOnPaused = true;
		super.onPause();
	}

	public void onResume() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onResume", LogMask.APP_COMMON);
		isOnPaused = false;
		super.onResume();
		updateDate();
		updateViews();
		if(!isResetEngine) {
			checkScreenOffState();
		}
		if (isOnNewIntent) {
			dealNewAction();
			isOnNewIntent = false;
		}
		mHandler.sendMessageDelayed(mHandler.obtainMessage(MESSAGE_SETLAYERFORPIP), 500);
		if (VideoRecorder.getInstance(this).isNeedShowRecordDialog()) {
			VideoRecorder.getInstance(this).showConfirmDialog(0);
		}
	}

	private void writeServiceIdAndFreq(Intent uri) {
		String idAndFreq = uri.getDataString();
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "writeServiceIdAndFreq is "
				+ idAndFreq, LogMask.APP_COMMON);
		if (idAndFreq != null) {
			String sep = "&";
			int idAndFreqLen = idAndFreq.length();
			if (idAndFreq.contains(SERVICE_ID)) {
				int idIndex = idAndFreq.indexOf(SERVICE_ID);
				int sepIndex = idAndFreq.lastIndexOf(sep);
				if (idAndFreqLen > idIndex && idAndFreqLen > sepIndex) {
					String serviceId = idAndFreq.substring(idIndex
							+ SERVICE_ID.length(), sepIndex);
					uri.putExtra(INTENT_CHANNEL_SERVICEID, serviceId);
				}
			} else {

			}
			if (idAndFreq.contains(FREQ)) {
				int freqIndex = FREQ.length() + idAndFreq.indexOf(FREQ);
				if (idAndFreqLen > freqIndex) {
					String freq = idAndFreq.substring(freqIndex);
					uri.putExtra(INTENT_CHANNEL_FREQ, Integer.valueOf(freq));
				}
			} else {

			}
		} else {

		}
	}
	
	private void checkScreenOffState() {
		if(MBBMSService.screenOff2MainplayerStop) {
			if(MBBMSService.getMainPlayerState() == State.PlaybackStoped) {
				MBBMSService.screenOff2MainplayerStop = false;
				showChannelIndex(MBBMSService.mMainPlayer);
				play(MBBMSService.mMainPlayer);	
			}
		}
		if(MBBMSService.screenOff2ViceplayerStop) {
			if(MBBMSService.getVicePlayerState() == State.PlaybackStoped) {
				MBBMSService.screenOff2ViceplayerStop = false;
				showChannelIndex(MBBMSService.mVicePlayer);
				play(MBBMSService.mVicePlayer);	
			}
		}
	}

	private void dealNewAction() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
				"MBBMSService.mMainPlayer is " + MBBMSService.mMainPlayer
						+ " MBBMSService.getMainPlayerState() is "
						+ MBBMSService.getMainPlayerState()
						+ " isChannelChanged is " + isChannelChanged,
				LogMask.APP_COMMON);
		if (MBBMSService.getMainPlayerState() != State.PlaybackStoped
				&& !isChannelChanged) {
			// already playing the channel no need to restart playing
			mMain.changeState(MBBMSService.getMainPlayerState());
			switchButtons();
		} else {
			// if not playing or the channel is switched, need to restart playing
			showChannelIndex(MBBMSService.mMainPlayer);
			play(MBBMSService.mMainPlayer);			
		}
		if (MBBMSService.getVicePlayerState() != State.PlaybackStoped) {
			mVice.changeState(MBBMSService.getVicePlayerState());
		}
	}

	protected void onDestroy() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onDestroy", LogMask.APP_COMMON);
		mThumbnails.recycleAllBitmap();
		unRegisterPlayReceiver();		
		if(VideoRecorder.getInstance(this) != null) {
			VideoRecorder.getInstance(this).resetShowRecordDialog();
		}
		sInstance = null;
		super.onDestroy();
	}

	public void onBackDestroy() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onBackDestroy", LogMask.APP_COMMON);
		super.onBackDestroy();
		stop(MBBMSService.mMainPlayer);
		closePip();
	}

	public boolean onKeyDown(int keyCode, KeyEvent event) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), 
				"onKeyDown() called, keyCode is " + keyCode, LogMask.APP_COMMON);
		switch (keyCode) {
		case KeyEvent.KEYCODE_BACK:
		    if(!ImageCaptureManager.getInstance(this).canExit()||VideoRecorder.getInstance(this).isShowingDialog()||ImageCaptureManager.getInstance(this).isShowingDialog()){
		        
		        return false;
		    }
			if (VideoRecorder.getInstance(this).canExit()) {
				finishPlayingActivity();
			} else {
				showConfirmDialog(DIALOG_CONFIRM_STOP_RECORD, 0);
			}
			return false;
		}
		return super.onKeyDown(keyCode, event);
	}

	protected void onNewIntent(Intent intent) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onNewIntent", LogMask.APP_COMMON);
		super.onNewIntent(intent);
		getIntentAction(intent);
	}

	public void getIntentAction(Intent intent) {
		isOnNewIntent = true;
		mPlayIntent = intent;
		String action = mPlayIntent.getAction();
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
				"getIntentAction() called, action is " + action,
				LogMask.APP_COMMON);
		int position = MBBMSService.sCurrentPlayPosition;
		if (action.equals(ACTION_VIEW_PROGRAM_ACTION)) {
			position = mPlayIntent.getExtras().getInt(INTENT_CHANNEL_POSITION);
		} else if (action.equals(ACTION_VIEW_SCHEDULE_ACTION)) {
			position = mPlayIntent.getExtras().getInt(INTENT_SCHEDULE_POSITION);
		} else if (action.equals(ACTION_VIEW_DETAIL_ACTION)) {
			updateDate();
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "ChannelName is "
					+ mPlayIntent.getExtras().getString(
							INTENT_DETAIL_POSITION), LogMask.APP_COMMON);
			for (int i = 0; i < mChannelList.size(); i++) {
				if (mChannelList.get(i).getChannelName().equals(
						mPlayIntent.getExtras().getString(
								INTENT_DETAIL_POSITION))) {
					position = i;
					break;
				}
			}
		} else if (action.equals(ACTION_VIEW_SEARCH_ACTION)) {
			updateDate();
			for (int i = 0; i < mChannelList.size(); i++) {
				if (mChannelList.get(i).getChannelName().equals(
						mPlayIntent.getExtras().getString(
								INTENT_SEARCH_POSITION))) {
					position = i;
					break;
				}
			}
		} else if (action.equals(ACTION_VIEW_PROGRAME_BY_ID_FREQ)) {
			MBBMSService.sPlayBackState = State.PlaybackLoading;
			mHandler.removeMessages(MESSAGE_PLAY_BY_ID_FREQ);
			Message msg = mHandler.obtainMessage();
			msg.obj = intent;
			msg.what = MESSAGE_PLAY_BY_ID_FREQ;
			mHandler.sendMessage(msg);
		}
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "position is " + position,
				LogMask.APP_COMMON);
		if (position != MBBMSService.sCurrentPlayPosition) {
			isChannelChanged = true;
			MBBMSService.sCurrentPlayPosition = position;
		} else {
			isChannelChanged = false;
		}
	}

	public void playByIntent(Intent intent) {
		if (onPlayByIDAndFreq(intent)) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "play by intent",
					LogMask.APP_MAIN);
			showChannelIndex(MBBMSService.PLAYER1);
			play(MBBMSService.PLAYER1);// play();
		} else {
			getShortPromptToast(R.string.cannotplay).show();
			stop(MBBMSService.PLAYER1);
			// notify user that can't play the program by the given serviceID and frequency;
		}
	}

	private boolean onPlayByIDAndFreq(Intent intent) {
		setupViews();
		updateDate();
		String serviceID = intent.getExtras().getString(
				INTENT_CHANNEL_SERVICEID);
		int freq = intent.getExtras().getInt(INTENT_CHANNEL_FREQ);
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
				"onPlayByIDAndFreq serviceID is " + serviceID + " freq = "
						+ freq, LogMask.APP_COMMON);
		if (null == serviceID || serviceID.equals(""))
			return false;
		if (!checkValidChannelList()) {
			return false;
		}
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "ValidChannelList",
				LogMask.APP_COMMON);
		for (int i = 0; i < mChannelList.size(); i++) {
			if (serviceID.equals(getServiceIDByPosition(i))
					&& freq == getFreqByPosition(i)) {
				if (MBBMSService.mMainPlayer == MBBMSService.PLAYER1) {
					MBBMSService.sCurrentPlayPosition = i;
				} else {
					MBBMSService.sVicePlayPosition = i;
				}
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
						"get the serviceid and freq prepared to play",
						LogMask.APP_COMMON);
				updateViews();
				return true;
			}
		}
		return false;
	}

	public boolean onCreateOptionsMenu(Menu menu) {
		Log.d("onCreateOptionMenu", "create menu");
		super.onCreateOptionsMenu(menu);
		if (mMenuManager != null) {
			if (!isFullScreen) {
				if(MBBMSService.sInstance != null) {
					if(MBBMSService.sInstance.isSpeakerOn) {
						mMenuManager.InflatMenu(R.menu.menu_play_normalscreen_headphone, menu);
					}else {
						mMenuManager.InflatMenu(R.menu.menu_play_normalscreen_speaker, menu);
					}
					if(!MBBMSService.sInstance.getHeadPhoneState()) {//if headphone is pull out
						mMenuManager.setEnabled(3, false);
					}
				}
			} else {
				if(MBBMSService.sInstance != null) {
					if(MBBMSService.sInstance.isSpeakerOn) {
						mMenuManager.InflatMenu(R.menu.menu_play_fullscreen_headphone, menu);
					}else {
						mMenuManager.InflatMenu(R.menu.menu_play_fullscreen_speaker, menu);
					}
					if(!MBBMSService.sInstance.getHeadPhoneState()) {
							mMenuManager.setEnabled(3, false);
					}
				}
			}
		}
		return true;
	}

	public boolean onOptionsItemSelected(MenuItem item) {
		mMenuManager.onMenuClick(item.getItemId());
		return super.onOptionsItemSelected(item);
	}

	private void init() {				 				
		mMenuManager = new MenuManager(this);
		mMenuManager.setPlayHandler(mHandler);		
		VideoRecorder.getInstance(this).setMain(true);
		VideoRecorder.getInstance(this).setContext(this);
		VideoRecorder.getInstance(this).setControlStart(mRecordImgButton);
		VideoRecorder.getInstance(this).setControlStop(mStopRecordImgButton);
		VideoRecorder.getInstance(this).setRecTime(mMain.getRecTime());
		VideoRecorder.getInstance(this).setThumbnailView(mThumbnails);
		ImageCaptureManager.getInstance(this).setCaptureView(mCaptureImgButton);
		ImageCaptureManager.getInstance(this).setThumbnailView(mThumbnails);
		ImageCaptureManager.getInstance(this).setListener(this);
		ImageCaptureManager.getInstance(this).setContext(this);
		mDisapearAnim = AnimationUtils.loadAnimation(this, R.anim.disappear);
		mAppearAnim = AnimationUtils.loadAnimation(this, R.anim.appear);
		mScaleSmallAnim = AnimationUtils.loadAnimation(this, R.anim.scalesmall);
		mScaleBigAnim = AnimationUtils.loadAnimation(this, R.anim.scalebig);
		mScaleAppearAnim = AnimationUtils.loadAnimation(this,
				R.anim.scaleappear);
		mScaleDisappearAnim = AnimationUtils.loadAnimation(this,
				R.anim.scaledisappear);
		mControlUpAnim = AnimationUtils.loadAnimation(this, R.anim.controlup);
		mControlDownAnim = AnimationUtils.loadAnimation(this,
				R.anim.controldown);
		mScaleSmallAnim.setAnimationListener(this);
		mInterpolator = AnimationUtils.loadInterpolator(this,
				android.R.anim.accelerate_decelerate_interpolator);
	}

	@Override
	public void initViews() {
		mPlayImgButton = (ImageButton) findViewById(R.id.ib_play);
		mRecordImgButton = (ImageButton) findViewById(R.id.ib_record);
		mPauseImgButton = (ImageButton) findViewById(R.id.ib_pause);
		mStopRecordImgButton = (ImageButton) findViewById(R.id.ib_stop_record);
		mMuteImgButton = (ImageButton) findViewById(R.id.ib_silent);
		mNoisyImgButton = (ImageButton) findViewById(R.id.ib_noisy);
		mCaptureImgButton = (ImageButton) findViewById(R.id.ib_capture);
		mChannelTextView = (TextView) findViewById(R.id.tv_channelname);
		mChannelGallery = (Gallery) findViewById(R.id.gl_channel);
		mTitleBar = (LinearLayout) findViewById(R.id.title_bar);
		mIVPadUp = (ImageView) findViewById(R.id.iv_uppad);
		mIVPadDown = (ImageView) findViewById(R.id.iv_downpad);
		mThumbnails = (ThumbnailView) findViewById(R.id.iv_thunbnails);
		mLLControlPanel = (LinearLayout) findViewById(R.id.control_panel);
		mPlaybackFrame = (FrameLayout) findViewById(R.id.playbackframe);
		mPlayerFrame1 = (PlayFrameView) findViewById(R.id.firstframe);
		mPlayerFrame2 = (PlayFrameView) findViewById(R.id.secondframe);
		mOpenPip = (TextView) findViewById(R.id.tv_openpip);
		mSwitchPip = (TextView) findViewById(R.id.tv_switchpip);
		mFullScreen = (TextView) findViewById(R.id.tv_full_screen);
		mPIPControler = (LinearLayout) findViewById(R.id.pip_controler);
		mPIPControler.setVisibility(View.GONE);

		mGalleryParams = (MarginLayoutParams) mChannelGallery.getLayoutParams();
		mControlLayoutParams = (MarginLayoutParams) mLLControlPanel.getLayoutParams();

		mMain = mPlayerFrame1;
		mVice = mPlayerFrame2;
		mMain.setMain(true);
		mVice.setMain(false);
		if (MBBMSService.mMainPlayer != MBBMSService.PLAYER1) {
			switchFrame();
			mMain.setVisibility(View.VISIBLE);
			mVice.setVisibility(View.GONE);
		}
		initPip();
		mChannelAdapter = new ChannelAdapter(this);
		prepareScreen();
		prepareSurfaceSize();
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.ib_play:
			showChannelIndex(MBBMSService.mMainPlayer);
			play(MBBMSService.mMainPlayer);			
			break;
		case R.id.ib_pause:
			stop(MBBMSService.mMainPlayer);
			break;
		case R.id.ib_record:
			record(MBBMSService.mMainPlayer);
			break;
		case R.id.ib_stop_record:
			stopRecord(VideoRecorder.getInstance(this).getSession());
			break;
		case R.id.ib_noisy:
			madeMute();
			break;
		case R.id.ib_silent:
			madeNoisy();
			break;
		case R.id.ib_capture:
			imageCapture();
			break;
		case R.id.tv_openpip:
			if (MBBMSService.isPIPOn) {
				if(isAnimationEnd) {
					LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "pip is on, close pip",
							LogMask.APP_COMMON);
					mOpenPip.setText(R.string.openpip);
					mOpenPip.setCompoundDrawablesWithIntrinsicBounds(0,
							R.drawable.pip, 0, 0);
					closePip();
				}
			} else {
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "pip is off, open pip",
						LogMask.APP_COMMON);
				if (!isFullScreen) {
					changeSurfaceSize();
					updateMenu();
				}
				mOpenPip.setText(R.string.closepip);
				mOpenPip.setCompoundDrawablesWithIntrinsicBounds(0,
						R.drawable.closepip, 0, 0);
				openPip();
			}
			mThumbnails.resumeTimeout();
			mHandler.removeMessages(DISAPPEAR_PIPCONTROLER);
			mHandler.sendEmptyMessageDelayed(DISAPPEAR_PIPCONTROLER, 4 * 1000);
			break;
		case R.id.tv_switchpip:
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "pip on is " + MBBMSService.isPIPOn,
					LogMask.APP_COMMON);
			if (MBBMSService.isPIPOn){
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "switch pip start",
						LogMask.APP_COMMON);
				switchPlayerAndFrame();
			}
			mThumbnails.resumeTimeout();
			mHandler.removeMessages(DISAPPEAR_PIPCONTROLER);
			mHandler.sendEmptyMessageDelayed(DISAPPEAR_PIPCONTROLER, 4 * 1000);
			break;
		case R.id.tv_full_screen:
			changeSurfaceSize();
			updateMenu();
			mThumbnails.resumeTimeout();
			mHandler.removeMessages(DISAPPEAR_PIPCONTROLER);
			mHandler.sendEmptyMessageDelayed(DISAPPEAR_PIPCONTROLER, 4 * 1000);
			break;
		default:
			break;
		}
	}

	public boolean dispatchKeyEvent(KeyEvent event) {
		if (event.getAction() == KeyEvent.ACTION_UP) {
			switch (event.getKeyCode()) {
			case KeyEvent.KEYCODE_VOLUME_UP:
				madeNoisy();
				break;
			}
		}
		switch(event.getKeyCode()){
		    case KeyEvent.KEYCODE_DPAD_LEFT:
		        if(event.getAction()==KeyEvent.ACTION_UP){
		            switchChannel(false, MBBMSService.mMainPlayer);
		        }
		        return false;
		    case KeyEvent.KEYCODE_DPAD_RIGHT:
		        if(event.getAction()==KeyEvent.ACTION_UP){
		            switchChannel(true, MBBMSService.mMainPlayer);
		        }
		        return false;
		}
		return super.dispatchKeyEvent(event);
	}

	public void imageCapture() {
		if (MBBMSService.getMainPlayerState() == State.PlaybackPlaying) {
			ImageCaptureManager.getInstance(this).startImageCapture(
					MBBMSService.mMainPlayer);
		}
	}

	public void stopRecord(int player) {
		if (VideoRecorder.getInstance(this).isSession(player)){
			VideoRecorder.getInstance(this).stopRecord();	
		}
	}

	public void record(int player) {
		if (MBBMSService.getMainPlayerState() == State.PlaybackPlaying) {
			VideoRecorder.getInstance(this).setRecTime(mMain.getRecTime());
			VideoRecorder.getInstance(this).startRecord(
					MBBMSService.mMainPlayer);
		}
	}

	public void madeMute() {
		if (isServiceStarted) {
			mMuteImgButton.setVisibility(View.VISIBLE);
			mNoisyImgButton.setVisibility(View.GONE);
			MBBMSService.sInstance.setMute(MBBMSService.mMainPlayer, true);
		}
	}

	public void madeNoisy() {
		if (isServiceStarted) {
			mMuteImgButton.setVisibility(View.GONE);
			mNoisyImgButton.setVisibility(View.VISIBLE);
			MBBMSService.sInstance.setMute(MBBMSService.mMainPlayer, false);
		}
	}

	public boolean checkPlayCondition(int player) {
		int position = -1;
		if (player == MBBMSService.mMainPlayer) {
			position = MBBMSService.sCurrentPlayPosition;
		} else {
			position = MBBMSService.sVicePlayPosition;
		}
		if (mChannelList != null && mChannelList.size() > position) {

		} else {
			return false;
		}
		if(mChannelList.get(position).getFreeState() == 1) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "channel is clear stream",
					LogMask.APP_COMMON);
			return true;//channel is clear stream
		} else {
			if(mChannelList.get(position).getEncryptState() == 1 || (mChannelList.get(position).getForFreeState() == 1)) {
				if(MBBMSService.isSGUpdateFailed) {
					LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "sg update failed, this encrypted channel can not be played",
							LogMask.APP_COMMON);
					return false;
				} else {
					LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "channel is unencrypted, no need to subscribe",
							LogMask.APP_COMMON);
					return true;//channel is unEncrypte, can be play
				}		
			} else {
				if(MBBMSService.sCurrentMode == MBBMS_MODE) {
					LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
							"channel is not subscribed", LogMask.APP_COMMON);
					showConfirmDialog(DIALOG_CONFIRM_SUBSCRIBE, position);
				}
			}
		}
		return false;
	}
	
	private void showChannelIndex(int player) {
		if (player == MBBMSService.mMainPlayer) {
			mMain.showIndex(MBBMSService.sCurrentPlayPosition + 1, 0);
		} else if (player == MBBMSService.mVicePlayer) {
			mVice.showIndex(MBBMSService.sVicePlayPosition + 1, 0);
		}
	}
	
	public void play(int player) {
		if (isServiceStarted && checkPlayCondition(player)) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "play() called",
					LogMask.APP_COMMON);
			stopPlayByIDFreq(player);
			if (player == MBBMSService.mMainPlayer) {
				mPlayImgButton.setVisibility(View.GONE);
				mPauseImgButton.setVisibility(View.VISIBLE);
				mPauseImgButton.setClickable(false);
				mMain.changeState(State.PlaybackLoading);
				MBBMSService.sInstance.startPlaying(player,getServiceId(player), getServiceReferenceId(player), getFreq(player));
				mPauseImgButton.setClickable(true);
			} else {
				mVice.changeState(State.PlaybackLoading);
				MBBMSService.sInstance.startPlaying(player,
						getServiceId(player), getServiceReferenceId(player), getFreq(player));
			}
		} else {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"this channel can not play, stopped", LogMask.APP_COMMON);
			stop(player);
			getShortPromptToast(R.string.cannotplay).show();
		}
	}

	private int getFreq(int player) {
		int position = -1;
		if (player == MBBMSService.mMainPlayer) {
			position = MBBMSService.sCurrentPlayPosition;
		} else {
			position = MBBMSService.sVicePlayPosition;
		}
		if (mChannelList != null && mChannelList.size() > position) {
			return getFreqByPosition(position);
		}
		return -1;
	}

	private int getFreqByPosition(int position) {
		if (MBBMSService.sCurrentMode == MBBMS_MODE) {
			return mResolver.getIsmaPara(
					mChannelList.get(position).getChannelId()).getFrequecy();
		} else {
			return mChannelList.get(position).getFrequecy();
		}
	}

	private String getServiceId(int player) {
		int position = -1;
		if (player == MBBMSService.mMainPlayer) {
			position = MBBMSService.sCurrentPlayPosition;
		} else {
			position = MBBMSService.sVicePlayPosition;
		}
		if (mChannelList != null && mChannelList.size() > position) {
			return getServiceIDByPosition(position);
		}
		return null;
	}

	private String getServiceIDByPosition(int position) {
		if (MBBMSService.sCurrentMode == MBBMS_MODE) {
			return mResolver.getServiceId(mChannelList.get(position).getChannelId());
		} else {
			return String.valueOf(mChannelList.get(position).getChannelId());
		}
	}
	
	private long getServiceReferenceId(int player) {
		int position  = -1;
		if (player == MBBMSService.mMainPlayer) {
			position = MBBMSService.sCurrentPlayPosition;
		} else {
			position = MBBMSService.sVicePlayPosition;
		}
		if (mChannelList != null && mChannelList.size() > position) {
			return mChannelList.get(position).getChannelId();
		}
		return 0;
	}

	public void stop(int player) {
		stopPlayByIDFreq(player);
		if (isServiceStarted) {
			stopRecord(player);
			if (player == MBBMSService.mMainPlayer) {
				mPauseImgButton.setVisibility(View.GONE);
				mPlayImgButton.setVisibility(View.VISIBLE);
				mMain.changeState(State.PlaybackStoped);
				mPlayImgButton.setClickable(false);
				MBBMSService.sInstance.stopPlaying(player);
				mPlayImgButton.setClickable(true);
			} else {
				mVice.changeState(State.PlaybackStoped);
				MBBMSService.sInstance.stopPlaying(player);
			}
		}
	}

	/*
	 * get the surface holder and add call back functions.
	 */
	private void setFirstSurfaceHolder() {
		if (MBBMSService.sInstance != null)
			MBBMSService.sInstance.setSurface(MBBMSService.PLAYER1,
					mPlayerFrame1.getSurfaceView().getHolder());
	}

	private void setSecondSurfaceHolder() {
		if (MBBMSService.sInstance != null)
			MBBMSService.sInstance.setSurface(MBBMSService.PLAYER2,
					mPlayerFrame2.getSurfaceView().getHolder());
	}

	private void setMediaListener() {
		if (MBBMSService.sInstance != null) {
			MBBMSService.sInstance.setMediaListener(mHandler);
		}
	}

	private void switchFrame() {
		PlayFrameView tmp = null;
		tmp = mMain;
		mMain = mVice;
		mVice = tmp;
		prepareSurfaceSize();
		mMain.setMain(true);
		mVice.setMain(false);
		mPlaybackFrame.bringChildToFront(mVice);
		mPlaybackFrame.bringChildToFront(mPIPControler);
		mPlaybackFrame.bringChildToFront(mThumbnails);
	}

	private void switchPlayer() {
		int tmp = 0;
		tmp = MBBMSService.mMainPlayer;
		MBBMSService.mMainPlayer = MBBMSService.mVicePlayer;
		MBBMSService.mVicePlayer = tmp;
	}

	private void switchPosition() {
		int tmp = 0;
		tmp = MBBMSService.sCurrentPlayPosition;
		MBBMSService.sCurrentPlayPosition = MBBMSService.sVicePlayPosition;
		MBBMSService.sVicePlayPosition = tmp;
	}

	public void switchPlayerAndFrame() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "animation end is  " + isAnimationEnd,
				LogMask.APP_COMMON);
		if (isAnimationEnd) {
			isAnimationEnd = false;
			switchAudio();
			switchPosition();
			switchPlayer();
			startSwitchAnimation();
			switchButtons();
		}
	}

	private void switchButtons() {
		if (MBBMSService.getMainPlayerState() != State.PlaybackStoped) {
			mPlayImgButton.setVisibility(View.GONE);
			mPauseImgButton.setVisibility(View.VISIBLE);
		} else {
			mPlayImgButton.setVisibility(View.VISIBLE);
			mPauseImgButton.setVisibility(View.GONE);
		}
	}

	private void switchAudio() {
		if (MBBMSService.sInstance.getPlayer(MBBMSService.mMainPlayer).isMute() == true) {

		} else {
			MBBMSService.sInstance.getPlayer(MBBMSService.mVicePlayer).setMute(
					false);
			MBBMSService.sInstance.getPlayer(MBBMSService.mMainPlayer).setMute(
					true);
		}

	}

	private void initPip() {
		if (MBBMSService.isPIPOn) {
			mSwitchPip.setEnabled(true);
			mVice.setVisibility(View.VISIBLE);
			mOpenPip.setText(R.string.closepip);
			mOpenPip.setCompoundDrawablesWithIntrinsicBounds(0, R.drawable.closepip, 0, 0);
		} else {
			mSwitchPip.setEnabled(false);
			mVice.setVisibility(View.GONE);
			mOpenPip.setText(R.string.openpip);
			mOpenPip.setCompoundDrawablesWithIntrinsicBounds(0, R.drawable.pip, 0, 0);
		}
	}

	public void onConfigurationChanged(Configuration newConfig) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onConfigurationChanged", LogMask.APP_COMMON);
		super.onConfigurationChanged(newConfig);
		prepareScreen();
		prepareSurfaceSize();
		mHandler.sendEmptyMessage(MESSAGE_SETLAYERFORPIP);
		mHandler.sendMessageDelayed(mHandler.obtainMessage(500), 1000);
	}

	private void prepareScreen() {
		if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"onConfigurationChanged, orientation change to portrait",
					LogMask.APP_COMMON);
			sScreenOrientation = Configuration.ORIENTATION_PORTRAIT;
			changeToPort();
		} else {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"onConfigurationChanged, orientation change to landscape",
					LogMask.APP_COMMON);
			sScreenOrientation = Configuration.ORIENTATION_LANDSCAPE;
			changeToLand();
		}
	}

	private void changeToLand() {
		mTitleBar.setVisibility(View.GONE);
		mLLControlPanel.setOrientation(LinearLayout.VERTICAL);
		mControlLayoutParams.width = 100;
		mControlLayoutParams.height = 235;
		mControlLayoutParams.rightMargin = 5;
		mControlLayoutParams.bottomMargin = 45;
		mLLControlPanel.setLayoutParams(mControlLayoutParams);
		mLLControlPanel.setBackgroundResource(R.drawable.button_port_bg);
		mIVPadUp.setVisibility(View.GONE);
		mIVPadDown.setVisibility(View.GONE);
		mGalleryParams.bottomMargin = 4;
		mChannelGallery.setLayoutParams(mGalleryParams);
	}

	private void changeToPort() {
		mTitleBar.setVisibility(View.VISIBLE);
		mLLControlPanel.setOrientation(LinearLayout.HORIZONTAL);
		mControlLayoutParams.width = WindowManager.LayoutParams.FILL_PARENT;
		mControlLayoutParams.height = 90;
		mControlLayoutParams.rightMargin = 0;
		mControlLayoutParams.bottomMargin = 1;
		mLLControlPanel.setLayoutParams(mControlLayoutParams);
		mLLControlPanel.setBackgroundResource(R.drawable.button_bg);
		mIVPadUp.setVisibility(View.VISIBLE);
		mIVPadDown.setVisibility(View.VISIBLE);
		mGalleryParams.bottomMargin = 105;
		mChannelGallery.setLayoutParams(mGalleryParams);
	}

	/*
	 * Change the surface's size when play the tv. Set the flag isFullScreen to
	 * the opposite value when click the item.
	 */
	private void changeSurfaceSize() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), 
				"changeSurfaceSize() load, flag isFullScreen is " 
				+ isFullScreen, LogMask.APP_COMMON);
		isFullScreen = !isFullScreen;
		prepareSurfaceSize();
	}

	/*
	 * Change the surface's size by the screen type. When the flag isFullScreen
	 * is changed reset the surface.
	 */
	private void prepareSurfaceSize() {
		MarginLayoutParams params = (MarginLayoutParams) mPlaybackFrame.getLayoutParams();
		LayoutParams mainspm = (LayoutParams) mMain.getLayoutParams();
		LayoutParams childspm = (LayoutParams) mVice.getLayoutParams();
		if (!isFullScreen) {
			if (sScreenOrientation == Configuration.ORIENTATION_LANDSCAPE) {
				params.width = ((WindowManager) getSystemService(WINDOW_SERVICE))
						.getDefaultDisplay().getHeight();
				params.height = params.width * 3 / 4;
				params.leftMargin = 30;
				params.topMargin = 10;
				mTitleBar.setVisibility(View.GONE);
				mLLControlPanel.setVisibility(View.VISIBLE);
				mChannelGallery.setVisibility(View.VISIBLE);
				mIVPadUp.setVisibility(View.GONE);
				mIVPadDown.setVisibility(View.GONE);
			} else {
				params.width = ((WindowManager) getSystemService(WINDOW_SERVICE))
						.getDefaultDisplay().getWidth();
				params.height = params.width * 3 / 4;
				params.leftMargin = 0;
				params.topMargin = 56;
				mTitleBar.setVisibility(View.VISIBLE);
				mLLControlPanel.setVisibility(View.VISIBLE);
				mChannelGallery.setVisibility(View.VISIBLE);
				mIVPadUp.setVisibility(View.VISIBLE);
				mIVPadDown.setVisibility(View.VISIBLE);
			}
			mainspm.height = params.height;
			mainspm.width = params.width;
			getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
		} else {
			params.width = ((WindowManager) getSystemService(WINDOW_SERVICE))
					.getDefaultDisplay().getWidth();
			params.height = ((WindowManager) getSystemService(WINDOW_SERVICE))
					.getDefaultDisplay().getHeight();
			params.leftMargin = 0;
			params.topMargin = 0;
			if (sScreenOrientation == Configuration.ORIENTATION_LANDSCAPE) {
				mainspm.height = params.height;
				mainspm.width = params.height * 4 / 3;
			} else {
				mainspm.width = params.width;
				mainspm.height = params.width * 3 / 4;
			}
			getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
			mTitleBar.setVisibility(View.GONE);
			mLLControlPanel.setVisibility(View.GONE);
			mChannelGallery.setVisibility(View.GONE);
			mIVPadUp.setVisibility(View.GONE);
			mIVPadDown.setVisibility(View.GONE);
		}
		childspm.height = mainspm.height / 2;
		childspm.width = mainspm.width / 2;
		mainspm.gravity = Gravity.CENTER;
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "default margin is "
				+ mainspm.bottomMargin, LogMask.APP_COMMON);
		mainspm.bottomMargin = 0;
		mainspm.rightMargin = 0;
		childspm.gravity = Gravity.RIGHT | Gravity.BOTTOM;
		if (mLastXPoint < 0) {
			mLastXPoint = 0;
		}
		if (mLastXPoint > params.width - childspm.width) {
			mLastXPoint = params.width - childspm.width;
		}
		if (mLastYPoint < 0) {
			mLastYPoint = 0;
		}
		if (mLastYPoint > params.height - childspm.height) {
			mLastYPoint = params.height - childspm.height;
		}
		childspm.rightMargin = (int) (mLastXPoint);
		childspm.bottomMargin = (int) (mLastYPoint);
		mPlaybackFrame.requestLayout();
	}

	public void setupViews() {
		mThumbnails.setListener(this);
		mPlayerFrame1.setScrollListener(this);
		mPlayerFrame2.setScrollListener(this);
		mPlayerFrame1.setPipListener(this);
		mPlayerFrame2.setPipListener(this);

		mPlayerFrame1.setOnScaleListener(this);
		mPlayerFrame2.setOnScaleListener(this);

		mPlayImgButton.setOnClickListener(this);
		mRecordImgButton.setOnClickListener(this);
		mPauseImgButton.setOnClickListener(this);
		mStopRecordImgButton.setOnClickListener(this);
		mMuteImgButton.setOnClickListener(this);
		mNoisyImgButton.setOnClickListener(this);
		mCaptureImgButton.setOnClickListener(this);
		mChannelGallery.setSpacing(5);
		mChannelGallery.setOnItemClickListener(new OnItemClickListener() {

			public void onItemClick(AdapterView<?> arg0, View arg1,
					int position, long id) {
				checkFreqChange(position);
				// switch channel
				switchChannelTo(position, MBBMSService.mMainPlayer);
			}
		});
		mOpenPip.setOnClickListener(this);
		mSwitchPip.setOnClickListener(this);
		mFullScreen.setOnClickListener(this);
		setFirstSurfaceHolder();
		setSecondSurfaceHolder();
		setMediaListener();

	}

	private void switchChannelTo(int position, int player) {
		if (player == MBBMSService.mMainPlayer) {
			if (position != MBBMSService.sCurrentPlayPosition) {
				MBBMSService.sCurrentPlayPosition = position;
				stopRecord(player);
				updateViews();
				play(player);
			}
		} else {
			if (position != MBBMSService.sVicePlayPosition || MBBMSService.sPlayer2State == State.PlaybackStoped) {
				MBBMSService.sVicePlayPosition = position;
				stopRecord(player);
				updateViews();
				play(player);
			}
		}
		showChannelIndex(player);
	}

	private void switchChannel(boolean right, int player) {
		if (checkValidChannelList()) {
			int position = -1;
			if (player == MBBMSService.mMainPlayer) {
				position = MBBMSService.sCurrentPlayPosition;
				if (right) {
					position++;
					if (position == mChannelList.size())
						position = 0;
				} else {
					position--;
					if (position == -1)
						position = mChannelList.size() - 1;
				}
				checkFreqChange(position);
//				mCurrentFreq = mChannelList.get(position).getFrequecy();
//				if(mCurrentFreq != mLastFreq) {
//					if(MBBMSService.isPIPOn) {
//						stop(MBBMSService.mVicePlayer);
//						mVicePlayerManager.resetViceChannelList(mCurrentFreq);
//						MBBMSService.sVicePlayPosition = position;
//						getShortPromptToast(R.string.piperror).show();
//					}
//					mLastFreq = mCurrentFreq;
//				}
			} else {
				position = MBBMSService.sVicePlayPosition;
				ChannelItem item = mChannelList.get(MBBMSService.sVicePlayPosition);
				mVicePlayerManager.setViceListPosition(item);
				mVicePlayerManager.switchToRight(right);
				item = mVicePlayerManager.getCurrentChannel();
				position = mVicePlayerManager.getVicePosition(item);
			}			
			switchChannelTo(position, player);
		}
	}
	
	private void checkFreqChange(int position) {
		mCurrentFreq = mChannelList.get(position).getFrequecy();
		if(mCurrentFreq != mLastFreq) {
			if(MBBMSService.isPIPOn) {
				stop(MBBMSService.mVicePlayer);
				mVicePlayerManager.resetViceChannelList(mCurrentFreq);
				MBBMSService.sVicePlayPosition = position;
				getShortPromptToast(R.string.piperror).show();
			}
			mLastFreq = mCurrentFreq;
		}
	}

	private void stopPlayByIDFreq(int player) {
		if (player != MBBMSService.PLAYER1)
			return;
		if (mHandler.hasMessages(MESSAGE_PLAY_BY_ID_FREQ)) {
			mHandler.removeMessages(MESSAGE_PLAY_BY_ID_FREQ);
			MBBMSService.sPlayBackState = State.PlaybackStoped;
			mPlayerFrame1.changeState(State.PlaybackStoped);
		}
	}

	private boolean checkValidChannelList() {
		return (mChannelList != null && mChannelList.size() > 0);
	}

	public void updateDate() {
		mChannelList = Util.getChannelList(MBBMSService.sCurrentMode, this);
		mVicePlayerManager = VicePlayerManager.getInstance(this);
		mVicePlayerManager.init(mChannelList);
		mCurrentFreq = mChannelList.get(MBBMSService.sCurrentPlayPosition).getFrequecy();
		mLastFreq = mCurrentFreq;
		mVicePlayerManager.resetViceChannelList(mCurrentFreq);
	}

	@Override
	public void updateViews() {
		mChannelAdapter.setData(mChannelList);
		mChannelAdapter.setCurrentPosition(MBBMSService.sCurrentPlayPosition);
		mChannelGallery.setAdapter(mChannelAdapter);
		mChannelGallery.setSelection(MBBMSService.sCurrentPlayPosition);
		if (checkValidChannelList()) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "mChannelList.size() is "
					+ mChannelList.size() + " sCurrentPlayPosition is "
					+ MBBMSService.sCurrentPlayPosition, LogMask.APP_COMMON);
			mChannelTextView.setText(mChannelList.get(
					MBBMSService.sCurrentPlayPosition).getChannelName());
			mMain.getLoadView().setTitle(
					mChannelList.get(MBBMSService.sCurrentPlayPosition)
					.getChannelName());
			mMainPreviewInfo = getPreviewInfo(MBBMSService.sCurrentPlayPosition);
			mMain.getLoadView().setInfo(mMainPreviewInfo);
			if (MBBMSService.sVicePlayPosition > mChannelList.size())
				return;
			mVice.getLoadView().setTitle(
					mChannelList.get(MBBMSService.sVicePlayPosition)
					.getChannelName());
		}
		if (MBBMSService.sInstance != null
				&& MBBMSService.sInstance.isMute(MBBMSService.mMainPlayer)) {
			mMuteImgButton.setVisibility(View.VISIBLE);
			mNoisyImgButton.setVisibility(View.GONE);
		} else {
			mMuteImgButton.setVisibility(View.GONE);
			mNoisyImgButton.setVisibility(View.VISIBLE);
		}
		showChannelIndex(MBBMSService.mMainPlayer);
	}

	public void onFrameClick(View v) {
		if (v.equals(mMain)) {

		} else if (v.equals(mVice)) {

		}
		if (mPIPControler.getVisibility() == View.VISIBLE) {
			mHandler.removeMessages(DISAPPEAR_PIPCONTROLER);
			if (MBBMSService.sInstance != null
					&& MBBMSService.sInstance.isSupportPip()) {
				mPIPControler.startAnimation(mControlUpAnim);
				mPIPControler.setVisibility(View.GONE);
			}
			if (mThumbnails.isValidFile()) {
				mThumbnails.setVisibility(View.GONE);
			}

		} else {
			if (MBBMSService.sInstance != null
					&& MBBMSService.sInstance.isSupportPip()) {
				mPIPControler.startAnimation(mControlDownAnim);
				mPIPControler.setVisibility(View.VISIBLE);
			}
			if (mThumbnails.isValidFile()) {
				mThumbnails.setVisibility(View.VISIBLE);
			}
			mHandler.sendEmptyMessageDelayed(DISAPPEAR_PIPCONTROLER, 4 * 1000);
		}
	}

	public void onFrameDoubleClick(View v) {
		if (v.equals(mMain)) {
			changeSurfaceSize();
			updateMenu();
		} else if (v.equals(mVice)) {
			switchPlayerAndFrame();
		}
	}

	public void onScrollLeft(View v) {
		if (v.equals(mPlayerFrame1)) {
			switchChannel(false, MBBMSService.PLAYER1);
		} else if (v.equals(mPlayerFrame2)) {
			switchChannel(false, MBBMSService.PLAYER2);
		}
	}

	public void onScrollRight(View v) {
		if (v.equals(mPlayerFrame1)) {
			switchChannel(true, MBBMSService.PLAYER1);
		} else if (v.equals(mPlayerFrame2)) {
			switchChannel(true, MBBMSService.PLAYER2);
		}
	}

	public void changeToPIP(View v) {
		if (v.equals(mMain)) {
			openPip();
		} else if (v.equals(mVice)) {

		}
	}

	public void closePip(View v) {
		if (v.equals(mMain)) {

		} else if (v.equals(mVice)) {
			closePip();
		}
	}

	private void showCancelDialog(int type) {
		String promptMsg = "";
		if (type == DIALOG_CANCEL_AUTHORIZE) {
			promptMsg = getResources().getString(R.string.getmsk);
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

	public void cancelDialogCancelled(int type) {
		// TODO Auto-generated method stub

	}

	private void registerPlayReceiver() {
		IntentFilter filter = new IntentFilter();
		filter.addAction(ACTION_NETWORKCONNECT_FAILED);
		filter.addAction(ACTION_AUTHORIZE_STARTED);
		filter.addAction(ACTION_AUTHORIZED);
		filter.addAction(Intent.ACTION_SCREEN_OFF);
		filter.addAction(Intent.ACTION_HEADSET_PLUG);
		filter.addAction(ACTION_RESET_FINISHED);
		mPlayReceiver = new PlayReceiver();
		this.registerReceiver(mPlayReceiver, filter);
	}

	private void unRegisterPlayReceiver() {
		if (mPlayReceiver != null) {
			this.unregisterReceiver(mPlayReceiver);
			mPlayReceiver = null;
		}
	}

	private class PlayReceiver extends BroadcastReceiver {

		@Override
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();
			int operateType = 0;
			if (ModeManager.getInstance(context) != null) {
				operateType = ModeManager.getInstance(context).getOperationType();
			}

			if (action.equals(ACTION_AUTHORIZE_STARTED)) {
				if (mCancelDialog == null || !mCancelDialog.isShowing()) {
					showCancelDialog(DIALOG_CANCEL_AUTHORIZE);
				}
			} else if (action.equals(ACTION_AUTHORIZED)
					&& isOperAuthorize(operateType)) {
				boolean result = intent.getBooleanExtra("AuthorizeResult",
						false);
				int resCode = intent.getIntExtra("resCode", 0);
				String errorInfo = Util.getErrorCodeInfo(context, resCode);
				if (result) {
					getShortPromptToast(R.string.getmsksuccess).show();
				} else {
					if(errorInfo.equals("")) {
						getShortPromptToast(context.getResources().getString(R.string.getmskfail)).show();
					} else {
						getShortPromptToast(errorInfo).show();
					}
				}
				dismissCancelDialog();
			} else if (action.equals(ACTION_NETWORKCONNECT_FAILED)) {
				dismissCancelDialog();
			} else if (action.equals(Intent.ACTION_SCREEN_OFF)) {
			    if(MBBMSService.enableScreenOffPlayback)
			        return;
				if (VideoRecorder.getInstance(PlayingActivity.this).getState() == 3) {
					if (MBBMSService.getMainPlayerState() != State.PlaybackStoped) {
						MBBMSService.screenOff2MainplayerStop = true;
						stop(MBBMSService.mMainPlayer);
					}
					if (MBBMSService.getVicePlayerState() != State.PlaybackStoped) {
						MBBMSService.screenOff2ViceplayerStop = true;
						stop(MBBMSService.mVicePlayer);
					}
				}
			} else if (action.equals(ACTION_RESET_FINISHED)) {
				checkScreenOffState();
			}
		}
	}


	private void updateMenu() {
		if (isFullScreen) {
			mMenuManager.updateMenu(MENU_FULL_SCREEN_INDEX,
					R.string.normalscreen);
			mFullScreen.setText(R.string.normalscreen);
		} else {
			mMenuManager
					.updateMenu(MENU_FULL_SCREEN_INDEX, R.string.fullscreen);
			mFullScreen.setText(R.string.fullscreen);
		}
	}

	private void setLayerforPip() {
		int layer1 = mVice.getSurfaceView().getSurfaceLayer();
		int layer2 = mMain.getSurfaceView().getSurfaceLayer();
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "layer1 is " + layer1
				+ " layer2 is " + layer2, LogMask.APP_COMMON);
		if (MBBMSService.sPlayBackState == State.PlaybackPlaying
				&& MBBMSService.sPlayer2State == State.PlaybackPlaying) {
			if (layer1 < layer2) {
				mVice.getSurfaceView().setSurfaceLayer(layer2);
				mMain.getSurfaceView().setSurfaceLayer(layer1);
			} else {
				mVice.getSurfaceView().setSurfaceLayer(layer1);
				mMain.getSurfaceView().setSurfaceLayer(layer2);
			}
		}
		int layer3 = mVice.getSurfaceView().getSurfaceLayer();
		int layer4 = mMain.getSurfaceView().getSurfaceLayer();
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
				"after switch layer, layer1 is " + layer3 + " layer2 is "
						+ layer4, LogMask.APP_COMMON);
	}

	private void openPip() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "openPip() called ", 
				LogMask.APP_COMMON);
		mVicePlayerManager.resetViceChannelList(mCurrentFreq);
		MBBMSService.sVicePlayPosition = MBBMSService.sCurrentPlayPosition;
		if (!MBBMSService.isPIPOn) {
			mSwitchPip.setEnabled(true);
			MBBMSService.isPIPOn = true;
			mVice.startAnimation(mScaleAppearAnim);
			mVice.setVisibility(View.VISIBLE);
			showChannelIndex(MBBMSService.mVicePlayer);
			play(MBBMSService.mVicePlayer);
		}
	}

	private void closePip() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "closePip() called ", 
				LogMask.APP_COMMON);
		if (MBBMSService.isPIPOn) {
			mSwitchPip.setEnabled(false);
			MBBMSService.isPIPOn = false;
			mVice.startAnimation(mScaleDisappearAnim);
			mVice.setVisibility(View.GONE);
			stop(MBBMSService.mVicePlayer);
		}
	}

	public void confirmDialogCancel(int type) {
		// TODO Auto-generated method stub

	}

	public void confirmDialogOk(int type) {
		if (type == DIALOG_CONFIRM_SUBSCRIBE) {
			startPurchaseActvity();
		} else if (type == DIALOG_CONFIRM_STOP_RECORD) {
			finishPlayingActivity();
		}
	}

	private void finishPlayingActivity() {
		if (MBBMSService.isPIPOn) {
			mScaleDisappearAnim.setAnimationListener(new AnimationListener() {

				public void onAnimationEnd(Animation animation) {
					onBackDestroy();
					PlayingActivity.this.finish();
				}

				public void onAnimationRepeat(Animation animation) {

				}

				public void onAnimationStart(Animation animation) {

				}

			});
			closePip();
		} else {
			onBackDestroy();
			this.finish();
		}
	}

	private void showConfirmDialog(int type, int position) {
		String promptMsg = "";
		if (type == DIALOG_CONFIRM_SUBSCRIBE) {
			promptMsg = this.getResources().getString(R.string.promptsubscribestart)
					+ "'" + mChannelList.get(position).getChannelName()
					+ "'" + this.getResources().getString(R.string.promptsubscribeend);
		} else if (type == DIALOG_CONFIRM_STOP_RECORD) {
			promptMsg = this.getResources().getString(R.string.ifexitrecording);
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

	private void startPurchaseActvity() {
		Intent intent = new Intent();
		intent.setClass(this, PurchaseManageActivity.class);
		intent.putExtra("SubscribeStatus", UNPURCHASED);
		this.startActivity(intent);
	}
	
	private String getPreviewInfo(int position) {
		long referenceId = mChannelList.get(position).getChannelId();
		long previewId = mResolver.getPreviewId(referenceId, 1);
		String previewText = "";
		if(previewId != 0) {
			previewText = mResolver.getPreviewText(previewId);
			if(previewText.equals("")) {
				previewText = getSharedPreferences("WELCOMINFO", MODE_PRIVATE).getString("WelcomInfo", "");
			}
		} else {
			previewText = getSharedPreferences("WELCOMINFO", MODE_PRIVATE).getString("WelcomInfo", "");
		}
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "preview text is " + previewText, LogMask.APP_COMMON);
		return previewText;
	}

	private boolean isOperAuthorize(int type) {
		if (type == AUTHORIZE) {
			return true;
		}
		return false;
	}

	/**
	 * when a gesture to switch frame is identified, this will be called.
	 */
	public void onScaled() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onScaled, switch frame", LogMask.APP_COMMON);
		if (MBBMSService.isPIPOn)
			switchPlayerAndFrame();
	}

	MarginLayoutParams childParam;

	public void onFrameMove(View v, float x, float y) {
		childParam = (MarginLayoutParams) mVice.getLayoutParams();
		childParam.rightMargin = childParam.rightMargin - (int) x;
		childParam.bottomMargin = childParam.bottomMargin - (int) y;
		if (childParam.bottomMargin < 0) {
			childParam.bottomMargin = 0;
		}
		if (childParam.bottomMargin > mPlaybackFrame.getHeight()
				- childParam.height) {
			childParam.bottomMargin = mPlaybackFrame.getHeight()
					- childParam.height;
		}
		if (childParam.rightMargin < 0) {
			childParam.rightMargin = 0;
		}
		if (childParam.rightMargin > mPlaybackFrame.getWidth()
				- childParam.width) {
			childParam.rightMargin = mPlaybackFrame.getWidth()
					- childParam.width;
		}
		mLastXPoint = childParam.rightMargin;
		mLastYPoint = childParam.bottomMargin;
		mPlaybackFrame.requestLayout();
	}

	public void onThumbnailClicked(View v) {
		mThumbnails.resumeTimeout();
		if (MBBMSService.getMainPlayerState() != State.PlaybackStoped) {
			stop(MBBMSService.mMainPlayer);
		}
		if (MBBMSService.getVicePlayerState() != State.PlaybackStoped) {
			stop(MBBMSService.mVicePlayer);
		}
	}

	private boolean captureALL;

	public void onCaptured(int session, boolean done) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onCaptured, session is "
				+ session, LogMask.APP_COMMON);
		if (captureALL) {
			if (session == MBBMSService.mMainPlayer) {
				ImageCaptureManager.getInstance(this).captrueImage(
						MBBMSService.mVicePlayer);
				try {
					FileInputStream fis = openFileInput("animation" + session);
					mMain.setCMMBLOGO(BitmapFactory.decodeStream(fis));
					fis.close();
				} catch (Exception e) {
					LogMask.LOGM(TAG, LogMask.LOG_APP_ERROR, LogMask.getLineInfo(),
							"captureALL session main exception", LogMask.APP_ERROR);
				}
			} else {
				captureALL = false;
				try {
					FileInputStream fis = openFileInput("animation" + session);
					mVice.setCMMBLOGO(BitmapFactory.decodeStream(fis));
					fis.close();
				} catch (Exception e) {
					LogMask.LOGM(TAG, LogMask.LOG_APP_ERROR, LogMask.getLineInfo(),
							"captureALL session vice exception", LogMask.APP_ERROR);
				}
				getNewAnimation();
				mMain.startSwitchAnimation(mScaleBigAnim, MBBMSService.getMainPlayerState());
				mVice.startSwitchAnimation(mScaleSmallAnim, MBBMSService.getVicePlayerState());
			}

		} else {
			try {
				if (session == MBBMSService.mMainPlayer) {
					FileInputStream fis = openFileInput("animation" + session);
					mMain.setCMMBLOGO(BitmapFactory.decodeStream(fis));
					fis.close();
				} else {
					FileInputStream fis = openFileInput("animation" + session);
					mVice.setCMMBLOGO(BitmapFactory.decodeStream(fis));
					fis.close();
				}
			} catch (Exception e) {
				LogMask.LOGM(TAG, LogMask.LOG_APP_ERROR, LogMask.getLineInfo(),
						"captureALL session exception", LogMask.APP_ERROR);
			}
			getNewAnimation();
			mMain.startSwitchAnimation(mScaleBigAnim, MBBMSService
					.getMainPlayerState());
			mVice.startSwitchAnimation(mScaleSmallAnim, MBBMSService
					.getVicePlayerState());
		}
	}

	public void startSwitchAnimation() {
		if (MBBMSService.getMainPlayerState() == State.PlaybackPlaying
				&& MBBMSService.getVicePlayerState() == State.PlaybackPlaying) {
			captureALL = true;
			ImageCaptureManager.getInstance(this).captrueImage(
					MBBMSService.mMainPlayer);
		} else {
			if (MBBMSService.getMainPlayerState() == State.PlaybackPlaying) {
				captureALL = false;
				ImageCaptureManager.getInstance(this).captrueImage(
						MBBMSService.mMainPlayer);
			} else if (MBBMSService.getVicePlayerState() == State.PlaybackPlaying) {
				captureALL = false;
				ImageCaptureManager.getInstance(this).captrueImage(
						MBBMSService.mVicePlayer);
			} else {
				getNewAnimation();
				mMain.startSwitchAnimation(mScaleBigAnim, MBBMSService.getMainPlayerState());
				mVice.startSwitchAnimation(mScaleSmallAnim, MBBMSService.getVicePlayerState());
			}
		}

	}

	private void getNewAnimation() {
		float pivotX = 1 - mLastXPoint / (mPlaybackFrame.getWidth() - mVice.getWidth());
		float pivotY = 1 - mLastYPoint / (mPlaybackFrame.getHeight() - mVice.getHeight());
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "pivotX = " + pivotX
				+ " pivotY = " + pivotY, LogMask.APP_COMMON);
		mScaleBigAnim = null;
		mScaleSmallAnim = null;
		mScaleBigAnim = new ScaleAnimation(0.5f, 1.0f, 0.5f, 1.0f,
				Animation.RELATIVE_TO_SELF, pivotX, Animation.RELATIVE_TO_SELF,
				pivotY);
		mScaleSmallAnim = new ScaleAnimation(2.0f, 1.0f, 2.0f, 1.0f,
				Animation.RELATIVE_TO_SELF, pivotX, Animation.RELATIVE_TO_SELF,
				pivotY);
		mScaleBigAnim.setInterpolator(mInterpolator);
		mScaleBigAnim.setDuration(700);
		mScaleSmallAnim.setInterpolator(mInterpolator);
		mScaleSmallAnim.setDuration(700);
		mScaleSmallAnim.setAnimationListener(this);
	}

	public void onAnimationEnd(Animation animation) {
		if (animation.equals(mScaleSmallAnim)) {
			mHandler.sendEmptyMessage(ANIMATION_END);
		}
	}

	public void onAnimationRepeat(Animation animation) {
		// TODO Auto-generated method stub

	}

	public void onAnimationStart(Animation animation) {

	}
	public boolean isOnPaused() {
		return isOnPaused;
	}
	
	public MenuManager getMenuManager(){
		return mMenuManager;
	}
	
}
