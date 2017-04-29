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

import java.io.File;
import java.io.IOException;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Handler;
import android.os.Message;
import android.os.ParcelFileDescriptor;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.marvell.cmmb.R;
import com.marvell.cmmb.activity.PlayingActivity;
import com.marvell.cmmb.aidl.MBBMSEngine;
import com.marvell.cmmb.common.FileManager;
import com.marvell.cmmb.common.MemoryStatus;
import com.marvell.cmmb.service.MBBMSService;
import com.marvell.cmmb.service.MBBMSService.State;
import com.marvell.cmmb.view.customview.RecTime;
import com.marvell.cmmb.view.customview.ThumbnailView;
import com.marvell.cmmb.view.dialog.ConfirmDialog;
import com.marvell.cmmb.view.dialog.ConfirmDialogCallBack;
import com.marvell.cmmb.view.dialog.PromptDialog;
import com.marvell.cmmb.aidl.LogMask;
/**
 * 
 * To manager the record behavior
 * 
 * @author DanyangZhou
 * @version [2011-4-1]
 */
public class VideoRecorder implements ConfirmDialogCallBack {
	private Context mContext;

	private int mState;

	private long mStartRecordingMills = 0;

	private String mName;

	private View mControlStart;

	private View mControlStop;

	private RecTime mRecTime;

	private ThumbnailView mThumbnailView;

	private PromptDialog mPromptDialog;

	private ConfirmDialog mConfirmDialog;

	private Toast mStopingToast;

	private File mFile;

//	private WakeLock mPartialWakeLock;

	private final int MAX_FAIL_STOP = 10;

	public static VideoRecorder sInstance;

	public static VideoRecorder sSecondInstance;

	private static final int STARTED = 1;

	private static final int STOPPING = STARTED + 1;

	private static final int STOPPED = STOPPING + 1;

	private static final int NO_SDCARD = STOPPED + 1;

	private static final int FINISH_REC = NO_SDCARD + 1;

	private static final int MESSAGE_TIMEOUT = FINISH_REC + 1;

	private static final int STOP_ERROR = MESSAGE_TIMEOUT + 1;

	private static final int START_REC_ERROR = STOP_ERROR + 1;
	private static final int TIMEOUT = 10 * 1000;

	private int mSession = 0;

	public boolean isMain;
	
	private String TAG = "VideoRecorder";
	
	private boolean isStopByAudioFoucus = false;
	
	private boolean isNeedShowDialogLater = false;

	public static VideoRecorder getInstance(Context context) {
		if (sInstance == null) {
			sInstance = new VideoRecorder(context);
		}
		return sInstance;
	}

	public static VideoRecorder getSecondRecorder(Context context) {
		if (sSecondInstance == null) {
			sSecondInstance = new VideoRecorder(context);
		}
		return sSecondInstance;
	}

	private Handler mHandler = new Handler() {

		@Override
		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			switch (msg.what) {
			case MESSAGE_TIMEOUT:
				if (mState == STOPPING) {
					mState = STOPPED;
					stopRecTime();
					setControlButton();
				}
				break;
			}
		}

	};

	private Toast mNoSDCard;

	private Toast mRecFinish;

	private Toast mStopError;

	private Toast mStartError;

	public VideoRecorder(Context context) {
		mContext = context;
		mState = STOPPED;
	}

	/**
	 * if the recorder is working
	 * 
	 * @return true the recorder is not working
	 * 
	 */
	public boolean canExit() {
		boolean rt = false;
		switch (mState) {
		case STARTED:
			rt = false;
			break;
		case STOPPING:
			rt = false;
			showStopingToast();
			break;
		case STOPPED:
			rt = true;
			break;
		}
		return rt;
	}

	public void confirmDialogCancel(int type) {
		if(isNeedShowDialogLater) {
			isNeedShowDialogLater = false;
		}
	}

	public void confirmDialogOk(int type) {
		if (null != mContext){
		    mContext.startActivity(getVideoFileIntent(mName));
		    if(isNeedShowDialogLater) {
		    	isNeedShowDialogLater = false;
		    }
		    if (MBBMSService.getMainPlayerState() != State.PlaybackStoped) {
		        ((PlayingActivity)mContext).stop(MBBMSService.mMainPlayer);
	        }
	        if (MBBMSService.getVicePlayerState() != State.PlaybackStoped) {
	            ((PlayingActivity)mContext).stop(MBBMSService.mVicePlayer);
	        }
		}
			
	}

	/**
	 * force exit without tips.
	 * 
	 */
	public void forceExit() {
		if (mState == STARTED) {
			mState = STOPPING;
			if (MBBMSEngine.getInstance().stopRecording(mSession)) {
				mState = STOPPED;
//				if (mPartialWakeLock != null) {
//					if (mPartialWakeLock.isHeld()) {
//						mPartialWakeLock.release();
//					}
//					mPartialWakeLock = null;
//				}
				stopRecTime();
				setControlButton();
			} else {
				// stop error
				onError();
			}
		}
	}

	public View getControlStart() {
		return mControlStart;
	}

	public View getControlStop() {
		return mControlStop;
	}

	public TextView getRecTime() {
		return mRecTime;
	}

	/**
	 * get the record state
	 * 
	 * @return 1 started, 2 stopping, 3 stopped
	 */
	public int getState() {
		return mState;
	}

	public void onError() {
		mState = STOPPED;
		if (null != mFile && mFile.exists()) {
			mFile.delete();
		}
		stopRecTime();
		setControlButton();
	}

	/**
	 * be sure call this by instance when recording finished.
	 * 
	 * @param name
	 *            the file's name
	 */
	public void onRecordFinished() {

		if (mState != STOPPED) {
			mState = STOPPED;
//			if (mPartialWakeLock != null) {
//				if (mPartialWakeLock.isHeld()) {
//					mPartialWakeLock.release();
//				}
//				mPartialWakeLock = null;
//			}
			stopRecTime();
			setControlButton();
//			showToast(FINISH_REC);
			showThumbnail();
			if(PlayingActivity.sInstance.isOnPaused()) {// if is stopped by AudioFocus and PlayingActivity is at background.then set a falg to show dialog at PlayingActivity's onResume.
				isNeedShowDialogLater = true;
			}else {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "before call showConfirmDialog:",
					LogMask.APP_COMMON);
				showConfirmDialog(FINISH_REC);
			}			
		}
	}

	public void onResumeTime() {
		if (mState == STARTED && mStartRecordingMills != 0 && mRecTime != null) {
			int seconds = (int) ((System.currentTimeMillis() - mStartRecordingMills) / 1000);
			mRecTime.resume(seconds);
		}
	}

	/**
	 * set the buttons state by the recording state
	 * 
	 */
	public void setControlButton() {
		if (!isMain) {
			return;
		}
		if (mState != STOPPED) {
			if (null != mControlStart)
				mControlStart.setVisibility(View.GONE);
			if (null != mControlStop)
				mControlStop.setVisibility(View.VISIBLE);
		} else {
			if (null != mControlStop)
				mControlStop.setVisibility(View.GONE);
			if (null != mControlStart)
				mControlStart.setVisibility(View.VISIBLE);
		}
	}

	public void setControlStart(View controlStart) {
		mControlStart = controlStart;
	}

	public void setControlStop(View controlStop) {
		mControlStop = controlStop;
	}

	public void setRecTime(RecTime recTime) {
		mRecTime = recTime;
		onResumeTime();
	}

	public void showConfirmDialog(int type) {
				
		if (mContext == null)
			return;
		String promptMsg = " ";
		promptMsg = mContext.getResources().getString(
				R.string.recordingfinished, mName);				
		if(mConfirmDialog!=null && mConfirmDialog.isShowing()) {
		    mConfirmDialog.dismiss();
		}
		mConfirmDialog = null;
		mConfirmDialog = new ConfirmDialog(mContext,
				R.drawable.dialog_hint, mContext.getResources().getString(
						R.string.hint), promptMsg);
		mConfirmDialog.setCallBack(this, 0);
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "dialog create state:"+mConfirmDialog, LogMask.APP_COMMON);
		if (!mConfirmDialog.isShowing()) {
			mConfirmDialog.show();
		}
	}

	/**
	 * show prompt dialog,the message displayed depends on the type
	 * 
	 */
	public void showPromptDialog(int type) {
		String promptMsg = " ";
		switch (type) {
		case NO_SDCARD:
			promptMsg = mContext.getResources().getString(R.string.nosdcard);
			break;
		case FINISH_REC:
			promptMsg = mContext.getResources().getString(
					R.string.recordingfinished, mName);
			break;
		default:
			break;
		}
		mPromptDialog = new PromptDialog(mContext, R.drawable.dialog_hint,
				mContext.getResources().getString(R.string.hint), promptMsg);
		if (!mPromptDialog.isShowing()) {
			mPromptDialog.show();
		}
	}

	private void showStopingToast() {
		if (mState == STOPPING) {
			if (mStopingToast == null) {
				mStopingToast = Toast.makeText(mContext, mContext
						.getResources().getString(R.string.isstopingrecording),
						Toast.LENGTH_SHORT);
			}
			mStopingToast.show();
		}
	}

	private void showToast(int type) {
		switch (type) {
		case NO_SDCARD:
			if (mNoSDCard == null) {
				mNoSDCard = Toast.makeText(mContext, mContext.getResources()
						.getString(R.string.nosdcard), Toast.LENGTH_SHORT);
			}
			mNoSDCard.show();
			break;
		case FINISH_REC:
			if (mRecFinish == null) {
				mRecFinish = Toast.makeText(mContext, mContext.getResources()
						.getString(R.string.recordingfinished, mName),
						Toast.LENGTH_LONG);
			}
			mRecFinish.setText(mContext.getResources().getString(
					R.string.recordingfinished, mName));
			mRecFinish.show();
			break;
		case STOP_ERROR:
			if (mStopError == null) {
				mStopError = Toast
						.makeText(mContext, mContext.getResources().getString(
								R.string.stoprecorderror), Toast.LENGTH_LONG);
			}
			mStopError.show();
			break;
		case START_REC_ERROR:
			if (mStartError == null) {
				mStartError = Toast.makeText(mContext, mContext.getResources()
						.getString(R.string.startrecerror), Toast.LENGTH_LONG);
			}
			mStartError.show();
			break;
		}
	}

	/**
	 * to start recording. if need manager to control the UI button,set the the
	 * view to manager.
	 */
	public void startRecord(int session) {
		if (mState == STOPPED) {
			if (MemoryStatus.getAvailableExternalMemorySize() > 500 * 1000) {
				mFile = FileManager.createVideoRecordFile();
				if (mFile != null) {
					mName = mFile.getAbsolutePath();
					ParcelFileDescriptor pfd = FileManager.getFileParcelable(
							mContext, mFile);
					if (pfd != null&& MBBMSEngine.getInstance().startRecording(session, pfd)) {
						mSession = session;
						try {
							pfd.close();
						} catch (IOException e) {
							e.printStackTrace();
						}
						mState = STARTED;
//						PowerManager pm = (PowerManager) mContext
//								.getSystemService(Context.POWER_SERVICE);
//						mPartialWakeLock = pm.newWakeLock(
//								PowerManager.PARTIAL_WAKE_LOCK, "PlayingView");
//						mPartialWakeLock.acquire();
						startRecTime();
					} else {
						showToast(START_REC_ERROR);
						onError();
					}
				} else {
					// the sdcard can't write
					mState = STOPPED;
				}

			} else {
				showToast(NO_SDCARD);
			}
		}
		setControlButton();
	}

	public void startRecTime() {
		if (null != mRecTime) {
			mRecTime.start();
		}
		mStartRecordingMills = System.currentTimeMillis();
	}

	/**
	 * to stop recording. if need manager to control the UI button,set the the
	 * view to manager.
	 */
	public void stopRecord() {
		if (mState == STARTED) {
			mState = STOPPING;
			if (MBBMSEngine.getInstance().stopRecording(mSession)) {
				onRecordFinished();
			} else {
				// stop error
				onError();
				showToast(STOP_ERROR);
			}
		}
	}

	public void stopRecTime() {
		if (null != mRecTime) {
			mRecTime.stop();
		}
		mStartRecordingMills = 0;
	}

	public static Intent getVideoFileIntent(String param) {
		Intent intent = new Intent(Intent.ACTION_VIEW);
		intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
		intent.putExtra("oneshot", 0);
		intent.putExtra("configchange", 0);
		Uri uri = Uri.fromFile(new File(param));
		intent.setDataAndType(uri, "video/*");
		return intent;
	}

	public void setMain(boolean isMain) {
		this.isMain = isMain;
	}

	public boolean getMain() {
		return isMain;
	}

	public static VideoRecorder getMainVideoRecorder() {
		if (sInstance != null && sInstance.isMain) {
			return sInstance;
		}
		if (sSecondInstance != null && sSecondInstance.isMain) {
			return sSecondInstance;
		}
		return null;
	}

	public static VideoRecorder getViceVideoRecorder() {
		if (sInstance != null && !sInstance.isMain) {
			return sInstance;
		}
		if (sSecondInstance != null && !sSecondInstance.isMain) {
			return sSecondInstance;
		}
		return null;
	}

	public void setContext(Context context) {
		mContext = context;
		mConfirmDialog = null;
	}

	public boolean isSession(int session) {
		return session == mSession;
	}

	public int getSession() {
		return mSession;
	}

	public void setThumbnailView(ThumbnailView view) {
		mThumbnailView = view;
	}

	public void showThumbnail() {
		if (null != mThumbnailView) {
			mThumbnailView.showThumbnail(mName, ThumbnailView.THUMBNAIL_TYPEVIDEO);
		}
	}
	
    public boolean isShowingDialog() {
        if(mConfirmDialog!=null) {
        	return mConfirmDialog.isShowing();
        }       
        return false;
    }

    public boolean isNeedShowRecordDialog() {
    	return isNeedShowDialogLater;
    }
    public void resetShowRecordDialog() {
    	isNeedShowDialogLater = false;
    }

}
