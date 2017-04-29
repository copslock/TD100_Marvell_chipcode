package com.marvell.cmmb.manager;

import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.common.Define;
import com.marvell.cmmb.service.MBBMSService;
import android.media.AudioManager.OnAudioFocusChangeListener;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.AudioManager;
import com.marvell.cmmb.aidl.MBBMSPlayer;

public class AudioFocusManager {
	private static final String TAG = "AudioFocusManager";
	private AlarmReceiver mAlarmReceiver;
	private AudioManager mAudioManager;
	private Context mContext;
	public static AudioFocusManager sInstance;

	private boolean mIsAlarmOn = false;
	private boolean mIsPlayerAudioFocused = false;

	private boolean isRegisted;
	private boolean isMuteByAudioFocus = false;

	AudioFocusManager(Context context) {
		mContext = context;
		init();

	}

	public static AudioFocusManager getIntance(Context context) {
		if (sInstance == null) {
			sInstance = new AudioFocusManager(context);
			return sInstance;
		}
		return sInstance;
	}

	private void init() {
		mAlarmReceiver = new AlarmReceiver();
		mAudioManager = (AudioManager) mContext
				.getSystemService(Context.AUDIO_SERVICE);
	}

	public void registerReceiver() {
		if (!isRegisted) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"register audio focus listener!", LogMask.APP_COMMON);
			isRegisted = true;
			mIsPlayerAudioFocused = true;
			isMuteByAudioFocus = false;
			registerAlarmReceiver();
			requestAudioListener();
		} else {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"Audio focus listener has registed!", LogMask.APP_COMMON);
		}
	}

	public void unRegisterReceiver() {
		if (isRegisted) {
			isRegisted = false;
			mIsPlayerAudioFocused = true;
			mIsAlarmOn = false;
			setUnMute();
			abandonAudioListener();
			unregAlarmReceiver();
		}
	}

	public void registerAlarmReceiver() {

		IntentFilter filter = new IntentFilter();
		filter.addAction("com.android.deskclock.ALARM_ALERT");
		filter.addAction("com.android.deskclock.ALARM_DONE");
		mContext.registerReceiver(mAlarmReceiver, filter);

	}

	public void unregAlarmReceiver() {
		try {
			mContext.unregisterReceiver(mAlarmReceiver);	
		} catch(Exception e) {
			e.printStackTrace();
		}
	}

	public void requestAudioListener() {
		mAudioManager.requestAudioFocus(mAudioFocusListener,
				AudioManager.STREAM_MUSIC, AudioManager.AUDIOFOCUS_GAIN);
	}

	public void abandonAudioListener() {

		mAudioManager.abandonAudioFocus(mAudioFocusListener);

	}

	private class AlarmReceiver extends BroadcastReceiver {

		@Override
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"isMuteByAudioFocus = " + isMuteByAudioFocus, 
					LogMask.APP_COMMON);
			if (action.equals("com.android.deskclock.ALARM_ALERT")) {
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "alarm start!",
						LogMask.APP_COMMON);
				mIsAlarmOn = true;
				if(!isMute() && !isMuteByAudioFocus) {
					isMuteByAudioFocus = true;
					setMute();
				}				
			} else if (action.equals("com.android.deskclock.ALARM_DONE")) {
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "alarm close!",
						LogMask.APP_COMMON);
				mIsAlarmOn = false;
				setUnMute();
			}
		}

	}

	private OnAudioFocusChangeListener mAudioFocusListener = new OnAudioFocusChangeListener() {
		public void onAudioFocusChange(int focusChange) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "audio focus change!",
					LogMask.APP_MAIN);
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"isMuteByAudioFocus = " + isMuteByAudioFocus, 
					LogMask.APP_COMMON);
			// AudioFocus is a new feature: focus updates are made verbose on purpose
			switch (focusChange) {
			case AudioManager.AUDIOFOCUS_LOSS:
				LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
						"AudioFocus: received AUDIOFOCUS_LOSS",
						LogMask.APP_MAIN);	
				if (MBBMSService.sInstance
						.getPlayer((MBBMSService.mMainPlayer)).isPlaying()
						|| MBBMSService.sInstance.getPlayer(
								(MBBMSService.mVicePlayer)).isPlaying()) {
//					VideoRecorder.getInstance(mContext).setFlagByAudioFocus(true);
					stop();
				}
				break;
			case AudioManager.AUDIOFOCUS_LOSS_TRANSIENT:
			case AudioManager.AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK:
				LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
						"AudioFocus:AUDIOFOCUS_LOSS_TRANSIENT",
						LogMask.APP_MAIN);
				mIsPlayerAudioFocused = false;
				if(!isMute() && !isMuteByAudioFocus) {
					isMuteByAudioFocus = true;
					setMute();
				}			
				break;
			case AudioManager.AUDIOFOCUS_GAIN:
				LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
						"AudioFocus: received AUDIOFOCUS_GAIN",
						LogMask.APP_MAIN);
				mIsPlayerAudioFocused = true;
				setUnMute();
				break;
			default:
				LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
						"Unknown audio focus change code", LogMask.APP_MAIN);
				break;
			}
		}
	};

	private void setMute() {
		if (MBBMSService.sInstance != null) {
			if (MBBMSService.sInstance.mMediaListener != null) {
				MBBMSService.sInstance.mMediaListener
						.sendEmptyMessage(Define.MESSAGE_MUTE);
			} else {
				MBBMSService.sInstance.getPlayer(MBBMSService.mMainPlayer).setMute(true);
			}
		}
	}

	private void setUnMute() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
				"isMuteByAudioFocus = " + isMuteByAudioFocus + " mIsAlarmOn = " 
				+ mIsAlarmOn + " mIsPlayerAudioFocused = " + mIsPlayerAudioFocused, 
				LogMask.APP_COMMON);
		if(isMute() && isMuteByAudioFocus && !mIsAlarmOn && mIsPlayerAudioFocused) {
			if (MBBMSService.sInstance != null) {
				if (MBBMSService.sInstance.mMediaListener != null) {
					MBBMSService.sInstance.mMediaListener
							.sendEmptyMessage(Define.MESSAGE_UNMUTE);
				} else {
					MBBMSService.sInstance.getPlayer(MBBMSService.mMainPlayer).setMute(false);
				}
			}
			isMuteByAudioFocus = false;
		}			
	}

	private void stop() {
		if (MBBMSService.sInstance != null) {
			if (MBBMSService.sInstance.mMediaListener != null) {
				MBBMSService.sInstance.mMediaListener
						.sendEmptyMessage(Define.MESSAGE_STOP);
			}
		}

	}

	private boolean isMute() {
		if(MBBMSService.sInstance != null) {
			return MBBMSService.sInstance.getPlayer(MBBMSService.mMainPlayer).isMute();
		}
		return false;
	}
}
