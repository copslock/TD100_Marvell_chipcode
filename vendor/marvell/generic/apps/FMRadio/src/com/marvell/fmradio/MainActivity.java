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

package com.marvell.fmradio;
import android.view.Window;
import android.graphics.drawable.Drawable;
import android.content.Context;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.media.AudioManager;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.ImageView;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.ProgressBar;
import android.view.WindowManager;
import com.marvell.fmradio.list.BrowserListActivity;
import com.marvell.fmradio.list.EditStationView;
import com.marvell.fmradio.util.ChannelHolder;
import com.marvell.fmradio.util.LogUtil;
import com.marvell.fmradio.util.Station;
import java.lang.Runnable;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.media.AudioSystem;
import java.lang.reflect.Field;
import android.content.Intent;
import android.content.IntentFilter;
import android.widget.Toast;
import java.util.Timer;
import java.util.TimerTask;

public class MainActivity extends Activity implements OnClickListener, OnTouchListener
{
	private final static String TAG = "FMRadioMainUI";
	public final static String WORK_INDEX = "work_index";
    public static int screenWidth = 0;
	private static final int REQ_BROWSER = 0;
	private static final int REQ_DELETE = 1;
	public static int enable;
	private static final int DIALOG_MSG_POWER_ON = 0;
	private static final int DIALOG_MSG_POWER_OFF = 1;	
	private static final int DIALOG_MSG_CLEAR_SCAN = 2;
	private static final int DIALOG_MSG_SCAN_WAIT = 3;	
	private static final int DIALOG_MSG_HEADSET = 4;
	private static final int DIALOG_SCAN_CLEAR = 5;
	private static final int DIALOG_MSG_OPEN_FAILED = 6;
	private static final int DIALOG_MSG_IN_CALL = 8;
	private static final int MENU_RECORD = 4;
	private static final int MENU_SCAN = 3;
	private static final int MENU_SPEAKER = 2;
	private static final int MENU_MONO = 1;
	private static final int MENU_MUTE = 0;
	private static final int MAX_FREQ = 108500;
	private static final int MIN_FREQ = 87500;
	private final static int[] mButtonID = 
	{
		R.id.new_btn_tune_left, R.id.new_btn_tune_right,
		R.id.new_btn_list, R.id.new_btn_tune_prev, R.id.new_btn_tune_next,
		R.id.new_btn_add
	};	
        private int scrolWidth = 0;	
        private boolean mIsHeadsetPlugged = false;
	private boolean mPowerOn = false;
	private boolean scrolOn = false;
	private boolean mMute = false;
	private boolean mSpeaker = false;
	private boolean mIsScanAll = false;
    public static boolean mRecord = false;
	public static ImageView rssi;
	private Button mScrol = null;
	private Button power = null;
	private ImageView record = null;
	private TextView timer = null;
	private ChannelHolder mChannelHolder = null;
    // Broadcast receiver for device connections intent broadcasts
    private final BroadcastReceiver mReceiver = new AudioServiceBroadcastReceiver();
	private AudioManager am = null;
	private TextView mChannelName = null;
	private TextView mScanning = null; 
	public static FrequencyView mFreqView = null;
	private long mCurrentTime = 0;
	private FMCallback mCallback = null;
        private GestureDetector mGestureDetector = null;
        private AlertDialog ma = null;
	private AlertDialog maOn = null;
        private AlertDialog maScan = null;
	private ProgressBar pb,pbScan;
	public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        final Context mContext = this.getApplicationContext();
        setVolumeControlStream(AudioManager.STREAM_FM);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        // Dont start FM when in Phone Call.
	am = (AudioManager)getSystemService(AUDIO_SERVICE);
        if (am.getMode() != AudioManager.MODE_NORMAL){
            mHandler.sendMessage(Message.obtain(mHandler, MSG_IN_CALL));
            return;
        }
        setContentView(R.layout.main_ui);
        mChannelHolder = ChannelHolder.getInstance();
        for (int i = 0; i < mButtonID.length; i++)
        {
           Button btn = (Button)findViewById(mButtonID[i]);
	        btn.setOnClickListener(this);
        }
	initRssi(); 
	mScrol = (Button)findViewById(R.id.new_btn_scrol);
	mScrol.setOnTouchListener(this);
	mGestureDetector = new GestureDetector(this, new ScrolGestureListener());
        power = (Button)findViewById(R.id.btn_power);
        power.setOnClickListener(this);
	record = (ImageView)findViewById(R.id.record);
	timer = (TextView)findViewById(R.id.timer);
        DisplayMetrics dm = new DisplayMetrics();   
        getWindowManager().getDefaultDisplay().getMetrics(dm);   
        screenWidth = dm.widthPixels;   
        mFreqView = (FrequencyView) findViewById(R.id.frequency_view);
        mFreqView.setHandler(mHandler, MSG_UPDATE_RULER);
	mScanning = (TextView) findViewById(R.id.scan);
        String name = mChannelHolder.getWorkFreqName();
        setWorkFreq(name, mChannelHolder.mWorkFreq); 
        mCallback = FMCallback.getInstance();
        mCallback.setHandler(mHandler);
	FMService.setHandler(mHandler);
    // Register for device connection intent broadcasts.
    IntentFilter intentFilter =
        new IntentFilter(Intent.ACTION_HEADSET_PLUG);

    mContext.registerReceiver(mReceiver, intentFilter);
        // Show dialog "please plug your headset" once found headset was unpluged.
        if(am.isWiredHeadsetOn() == false){
            showDialog(DIALOG_MSG_HEADSET);
        } else {
            enableProgressBar();
            startServiceByAction(FMService.WAITFOR_ACTION);
        }
    }
     private void enableProgressBar(){

	showDialog(DIALOG_MSG_POWER_ON);
	Field field;
	        try {

		field = maOn.getClass().getSuperclass().getDeclaredField("mShowing" );		
		field.setAccessible( true );
						            
		field.set(maOn, false );
		}
		catch (Exception e)
		{
		}
	pb = (ProgressBar)maOn.findViewById(R.id.power_percent); 
        pb.setProgress(0);
	Thread t = new Thread(){

	public void run(){
									 
	int i = 0;
	while(i<=100){
		if(pb!=null){

		
		pb.incrementProgressBy(1);
		i++;
		}
		else
			break;
	try{
	Thread.sleep(60);       } 
	catch (InterruptedException e) {

		// TODO Auto-generated catch block
	e.printStackTrace();                        
	}
       }
     }
     };
	t.start();
    }
    private void initRssi(){
        rssi = (ImageView)findViewById(R.id.rssi);
    }
    private void showNotification() {
	NotificationManager mNotificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
	Intent intent = new Intent(getApplicationContext(),MainActivity.class);
intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP
		        | Intent.FLAG_ACTIVITY_NEW_TASK);
	PendingIntent contentIntent = PendingIntent.getActivity(getApplicationContext(),0,intent, 0); 
	Notification mNotification = new Notification(
		 R.drawable.fm_app_icon,  "FMRadio is on!", 
			            System.currentTimeMillis());
	mNotification.setLatestEventInfo(
		 getApplicationContext(),"FMRadio","FMRadio is running",contentIntent);
	mNotificationManager.notify(1, mNotification);
    }
    private void startServiceByAction(String action){

	Intent mIntent = new Intent(this,FMService.class);
	mIntent.setAction(action);
	if(action==FMService.MUTE_ACTION)
	mIntent.putExtra("mute", mMute);
	else if(action==FMService.SPEAKER_ACTION)
	mIntent.putExtra("speaker", mSpeaker);
	this.startService(mIntent);
    }

    private void startServiceByActionAndValue(String action, boolean enable){

	Intent mIntent = new Intent(this,FMService.class);
	mIntent.setAction(action);
	if(action==FMService.MUTE_ACTION)
	mIntent.putExtra("mute", enable);
	else if(action==FMService.SPEAKER_ACTION)
	mIntent.putExtra("speaker", enable);
	this.startService(mIntent);
    }
    private void operateScrol(MotionEvent e1, MotionEvent e2){

	mScrol.setPressed(!scrolOn);
	scrolOn=!scrolOn;
	scrolWidth = mScrol.getWidth();
	if(e2.getX()>e1.getX()){

	   float f;
							          
		if(e2.getX()<=(float)scrolWidth)
		   f = e2.getX()-e1.getX();
		else
		   f = scrolWidth-e1.getX();
		if(screenWidth==800)
		mChannelHolder.mWorkFreq += ((int)f/24)*100;
		else
		mChannelHolder.mWorkFreq += ((int)f/12)*100;
		if(mChannelHolder.  mWorkFreq<=108500)
		{
		startServiceByAction(FMService.SETCHANNEL_ACTION);   
		}
		else{
			mChannelHolder.mWorkFreq = 108500;
		startServiceByAction(FMService.SETCHANNEL_ACTION);
		}
	}
	 else if(e2.getX()<e1.getX()){

		float f;
		if(e2.getX()>=0)
		   f = e1.getX()-e2.getX();
		else
		   f = e1.getX();
		if(screenWidth==800)
			mChannelHolder.mWorkFreq -= ((int)f/24)*100;
		else
			mChannelHolder.mWorkFreq -= ((int)f/12)*100;
		if(mChannelHolder.mWorkFreq>=87500)
		{
			startServiceByAction(FMService.SETCHANNEL_ACTION);
		}
		else{
			mChannelHolder.mWorkFreq = 87500;
			startServiceByAction(FMService.SETCHANNEL_ACTION);
		} 
	 }			 
    }

    class ScrolGestureListener extends GestureDetector.SimpleOnGestureListener {
                    @Override
		public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX,  float velocityY)
		{
                              operateScrol(e1,e2);
			      return true;
			}
		public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
		{
			operateScrol(e1,e2);
			return true;
			}
    }
    public void onClick(View v)
    {
		switch (v.getId())
		{
		case R.id.btn_power:
			if (! mPowerOn)
			{
                onPowerOn();
			}
			else
			{
                onPowerOff();
			}
			break;
            
		case R.id.new_btn_list:				
                        if (!mPowerOn) {
                             break;
                        }
			BrowserListActivity.startActivity(MainActivity.this, REQ_BROWSER);
			break;

		case R.id.new_btn_tune_prev:
                        if (!mPowerOn) {
                             break;
                        }
			mScanning.setVisibility(View.VISIBLE);
			startServiceByAction(FMService.SCANPREV_ACTION);
			break;
			
		case R.id.new_btn_tune_next:
                        if (!mPowerOn) {
                             break;
                        }
			mScanning.setVisibility(View.VISIBLE);
			startServiceByAction(FMService.SCANNEXT_ACTION);
			break;
			
		case R.id.new_btn_add:	
                        if (!mPowerOn) {
                             break;
                        }
			EditStationView.startActivity(MainActivity.this, REQ_BROWSER, -1, String.valueOf(mChannelHolder.mWorkFreq));
			break;
			
		case R.id.new_btn_tune_left:
                        if (!mPowerOn) {
                             break;
                        }
			if (87500 < mChannelHolder.mWorkFreq)
			{
			    mScrol.setPressed(!scrolOn);
			    scrolOn=!scrolOn;
				mChannelHolder.mWorkFreq -= 100;
			startServiceByAction(FMService.SETCHANNEL_ACTION); 
			}
			break;
			
		case R.id.new_btn_tune_right:
                        if (!mPowerOn) {
                             break;
                        }
			if (mChannelHolder.mWorkFreq < 108500)
			{
			    mScrol.setPressed(!scrolOn);
			    scrolOn=!scrolOn;
				mChannelHolder.mWorkFreq += 100;
			startServiceByAction(FMService.SETCHANNEL_ACTION); 
			}
			break;				

		default:
			break;
		}
    }

    public boolean onCreateOptionsMenu(Menu menu)
    {
        super.onCreateOptionsMenu(menu);
        menu.add(Menu.NONE, MENU_SCAN, 1, R.string.scan_save)
        .setIcon(R.drawable.icon_menu_scan_save);
        menu.add(Menu.NONE, MENU_RECORD, 2, R.string.record)
        .setIcon(R.drawable.icon_menu_record);
        menu.add(Menu.NONE, MENU_MUTE, 3, R.string.mute)
        .setIcon(R.drawable.icon_menu_mute);
        menu.add(Menu.NONE, MENU_SPEAKER, 4, R.string.speaker)
        .setIcon(R.drawable.icon_menu_speaker);

        return true;
    }
    
    
    public boolean onPrepareOptionsMenu(Menu menu) 
    {
        MenuItem item = menu.findItem(MENU_MUTE);
    	
    	if (mMute)
    	{
    		item.setIcon(R.drawable.icon_menu_unmute);
    		item.setTitle(R.string.unmute);
    	}
    	else
    	{
    		item.setIcon(R.drawable.icon_menu_mute);
    		item.setTitle(R.string.mute);
    	}    	
    	
        item = menu.findItem(MENU_SPEAKER);
    	
    	if (mSpeaker)
    	{
    		item.setIcon(R.drawable.icon_menu_headset);
    		item.setTitle(R.string.headset);
    	}
    	else
    	{
    		item.setIcon(R.drawable.icon_menu_speaker);
    		item.setTitle(R.string.speaker);
    	}    	

        item = menu.findItem(MENU_RECORD);

        if (mRecord)
        {
            item.setIcon(R.drawable.icon_menu_stoprecord);
            item.setTitle(R.string.stopRecord);
        }
        else
        {
            item.setIcon(R.drawable.icon_menu_record);
            item.setTitle(R.string.record);
        }

    	return super.onPrepareOptionsMenu(menu);
    }

	public boolean onOptionsItemSelected(MenuItem item)
	{
		switch (item.getItemId())
		{
		case MENU_MUTE:
			mMute = ! mMute;
                        /*
                         ** FM run in record mode, mute audioTrack
                         ** FM run in playback mode, mute 8787 and codec
                        */
                        am.setStreamMute(AudioManager.STREAM_FM, mMute);
                        startServiceByAction(FMService.MUTE_ACTION);
			return true;
			
		case MENU_SPEAKER:
			mSpeaker = ! mSpeaker;
			startServiceByAction(FMService.SPEAKER_ACTION);
			return true;
			
		case MENU_SCAN:
			showDialog(DIALOG_MSG_CLEAR_SCAN);
			return true;

		case MENU_RECORD:
                        mRecord = ! mRecord;
                        startServiceByAction(FMService.RECORD_ACTION);
                        return true;

		default:
			break;
		}

		return super.onOptionsItemSelected(item);
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event)
	{
		switch (keyCode)
		{
		case KeyEvent.KEYCODE_BACK:
			if(!mPowerOn){
				Intent i = new Intent(this, FMService.class);
				this.stopService(i);
				finish();
			}
			else
			moveTaskToBack(true);
			return true;

		default:
			break;
		}

		return super.onKeyDown(keyCode, event);
	}	
	
	private final static int MSG_UPDATE_RULER = 0;

	private final static int MSG_TUNE = 1;
	private final static int MSG_SCAN_FINISHED = 2;
	private final static int MSG_ENABLE = 3;
	private final static int MSG_SCAN_FAILED = 4;
	private final static int MSG_SCANALL = 5;
	private final static int MSG_DISMISS = 6;
	private final static int MSG_INITIATE = 7;
	private final static int MSG_FINISH = 8;
	private final static int MSG_SETCHANNEL = 9;
	private final static int MSG_DISABLE = 10;
	private final static int MSG_FOUNDCHANNEL = 13;
	private final static int MSG_DISMISS_SCAN = 15;
	private final static int MSG_STATE_CHANGED = 16;
        private final static int MSG_POWER_ON = 17;
        private final static int MSG_POWER_OFF = 18;
        private final static int MSG_RECORD_SUCCESS = 19;
	private final static int MSG_RECORD_FAILED = 20;
	private final static int MSG_RECORD_NO_SDCARD = 21;
	private final static int MSG_RECORD_STOPPED = 22;
	private final static int MSG_UPDATE_TIMER = 23;
	private final static int MSG_INTERNAL_ERROR = 24;
	private final static int MSG_MAX_FILESIZE_REACHED = 25;
	private final static int MSG_UNKOWN_ERROR = 26;
	private final static int MSG_IN_CALL = 27;
	private static final int EVENT_WIRED_HEADSET_PLUG = 1;

	private final Handler mHandler = new Handler()
	{
		public void handleMessage(Message msg)
		{
			switch (msg.what)
			{
			case MSG_UPDATE_RULER:
				updateRuler(msg);
				break;
			case MSG_TUNE:
				String name = mChannelHolder.getWorkFreqName();		
				setWorkFreq(name, mChannelHolder.mWorkFreq);
				break;
			case MSG_SCAN_FINISHED:
				startServiceByAction(FMService.GETCHANNEL_ACTION);
				mScanning.setVisibility(View.INVISIBLE);
                mIsScanAll = false;
				break;
			case MSG_SCAN_FAILED:
				mScanning.setVisibility(View.INVISIBLE);
                mIsScanAll = false;
				break;
			case MSG_INITIATE:
				startServiceByAction(FMService.INITIATE_ACTION);
				break;
			case MSG_FOUNDCHANNEL:
                if (!mIsScanAll)
                    break;
                if (!maScan.isShowing())
                {
                    pbScan.setProgress(0);
                    mScanning.setVisibility(View.INVISIBLE);
                    startServiceByAction(FMService.STOPSCAN);
                    break;
                }
                int freq = msg.arg1;
                pbScan.setProgress((freq - MIN_FREQ) * 100/(MAX_FREQ - MIN_FREQ));
                break;
			case MSG_ENABLE:
				Field field1;
				try {
			        field1 = maOn.getClass().getSuperclass().getDeclaredField("mShowing");		                
				field1.setAccessible( true );
		
			        field1.set(maOn, true);
		                 }
				catch (Exception e)
				{

				}
				maOn.dismiss();
				if(pb!=null){

				pb.setProgress(0);   
				pb = null;
				}
				if(enable == -1){
					showDialog(DIALOG_MSG_OPEN_FAILED);
					mPowerOn = false;
					FrequencyViewListener.on = false;
				}
				 else{
				mPowerOn =true;
				Drawable on = getResources().getDrawable(R.drawable.power_on);
				power.setBackgroundDrawable(on);
				showNotification();
				FrequencyViewListener.on = true;
				mScrol.setClickable(true);
				startServiceByAction(FMService.SETCHANNEL_ACTION); 
				 }
				break;
			case MSG_SCANALL:
				 if(maOn!=null){
					Field field2;
					try {

					field2 = maOn.getClass().getSuperclass().                    getDeclaredField("mShowing");					
					field2.setAccessible( true );
					field2.set(maOn, true);
					}
					catch (Exception e)
					{
					}
					maOn.dismiss();
					if(pb!=null){
					pb.setProgress(0);   
					pb = null;
					}
				}
				showDialog(DIALOG_SCAN_CLEAR);
				pbScan = (ProgressBar)maScan.findViewById(R.id.power_percent);   
				pbScan.setProgress(0);
				for (int pos = mChannelHolder.getCount() - 1 ; 0 <= pos ; pos--) 
		                {
					mChannelHolder.delete(pos);
				}
				setWorkFreq(null, mChannelHolder.mWorkFreq);
				startServiceByAction(FMService.SCANALL);
                mIsScanAll = true;
				break;
			case MSG_SETCHANNEL:
				startServiceByAction(FMService.SETCHANNEL_ACTION);
				break;
			case MSG_FINISH:
				{
				mPowerOn = false;
				NotificationManager mNotificationManager =		    (NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE);
				mNotificationManager.cancel(1); 
				Intent mIntent = new Intent(MainActivity.this,FMService.class);  
				stopService(mIntent);
				finish();
				System.exit(0);
				}
				break;
			case MSG_DISABLE:
				startServiceByAction(FMService.DISABLE_ACTION);
				break;
			case MSG_IN_CALL:
				showDialog(DIALOG_MSG_IN_CALL);
				break;
			case MSG_DISMISS_SCAN:
				if(maScan!=null){

				if(maScan.isShowing()){
				maScan.dismiss();
				pbScan.setProgress(0);
				}
				}
				break;
			case MSG_DISMISS:
				if(ma!=null){
				if(ma.isShowing())
				ma.dismiss();
				if(mRecord){
				startServiceByAction(FMService.RECORD_ACTION);
				 mRecord = false;
				displayRecordImage(false);
				}
				break;
				}
				if(maOn!=null){
				Field field;
				try {

					 
					field = maOn.getClass().getSuperclass().getDeclaredField("mShowing" );
					field.setAccessible( true );
					 
					field.set(maOn, true);
				}
				catch (Exception e)
				{

				}
				maOn.dismiss();
				if(pb!=null){
				pb.setProgress(0);   
				pb = null;
				}
				}
				 if(enable == -1){
				showDialog(DIALOG_MSG_OPEN_FAILED);
				mPowerOn = false;
				FrequencyViewListener.on = false;
				Drawable off = getResources().getDrawable(R.drawable.power);
				power.setBackgroundDrawable(off);
				 }
				 else{
                    startServiceByAction(FMService.SPEAKER_ACTION);
					mPowerOn = true;
					Drawable on = getResources().getDrawable(R.drawable.power_on);
					power.setBackgroundDrawable(on);
					showNotification();
					FrequencyViewListener.on = true;
					startServiceByAction(FMService.SETCHANNEL_ACTION); 
				}
				break;
			case MSG_STATE_CHANGED:
				Log.v(TAG, "State Changed mode = " + msg.arg1);
				if (msg.arg1 == 0 && mPowerOn) {
					mPowerOn = false;
					Drawable off = getResources().getDrawable(R.drawable.power);
					power.setBackgroundDrawable(off);
					showDialog(DIALOG_MSG_POWER_OFF);
					startServiceByAction(FMService.DISABLE_ONLY_ACTION);
					NotificationManager mNotificationManager = (NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE);
					mNotificationManager.cancel(1);
					FrequencyViewListener.on = false;
					mScrol.setClickable(false);
					rssi.setBackgroundResource(R.drawable.xinhao_00);
                }
				break;
            case MSG_POWER_ON:
                onPowerOn();
                break;
            case MSG_POWER_OFF:
                onPowerOff();
                break;
            case MSG_RECORD_SUCCESS:
                mCurrentTime = System.currentTimeMillis();
                displayRecordImage(true);
                break;
            case MSG_RECORD_FAILED:
                mRecord = false;
                Toast.makeText(MainActivity.this,R.string.toast_record_fail, Toast.LENGTH_LONG).show();
                break;
            case MSG_RECORD_STOPPED:
                String filePath = null;
                if(msg.obj!=null)
                filePath = String.valueOf(msg.obj);
                long time = System.currentTimeMillis();
                long elapseTime = time - mCurrentTime;
                Toast.makeText(MainActivity.this,getResources().getString(R.string.toast_record_file_path) + filePath +". " +getResources().getString(R.string.recordTime) + updateTime(elapseTime), Toast.LENGTH_LONG).show();
                displayRecordImage(false);
                break;
            case MSG_RECORD_NO_SDCARD:
                Toast.makeText(MainActivity.this, R.string.toast_record_no_sdcard, Toast.LENGTH_LONG).show();
                break;
            case MSG_UPDATE_TIMER:
                long time_update = System.currentTimeMillis();
                long lap = time_update - mCurrentTime;
                timer.setText(updateTime(lap));
                break;
            case MSG_INTERNAL_ERROR:
                Toast.makeText(MainActivity.this, R.string.internal_error, Toast.LENGTH_LONG).show();
                break;
            case MSG_MAX_FILESIZE_REACHED:
                Toast.makeText(MainActivity.this, R.string.max_filesize_reached, Toast.LENGTH_LONG).show();
                break;
            case  MSG_UNKOWN_ERROR:
                Toast.makeText(MainActivity.this, R.string.unknown_error, Toast.LENGTH_LONG).show();
                break;
			default:
				break;

			}
		}
		
		private void updateRuler(Message msg)
		{
			mChannelHolder.mWorkFreq = msg.arg1;
			String name = mChannelHolder.getWorkFreqName();
			setWorkFreq(name, mChannelHolder.mWorkFreq);
		}
	};
	
	private String updateTime(long time){
        long  t = time/1000;
        long hour = t/3600;
        long hourResi = t%3600;
        long minute = hourResi/60;
        long minuteResi = hourResi%60;
        StringBuilder s = new StringBuilder();
        if(hour<10)
            s.append('0');
        s.append(hour);
        s.append(':');

        if(minute<10)
            s.append('0');

        s.append(minute);
        s.append(':');
        if(minuteResi<10)
            s.append('0');

        s.append(minuteResi);
        return s.toString();
	}

	class MyTask extends TimerTask{
		public void run(){
			if(mRecord)
			mHandler.sendMessage(Message.obtain(mHandler, MSG_UPDATE_TIMER));
		}
	}

	private void displayRecordImage(boolean display){
		if(display){
			record.setVisibility(View.VISIBLE);
			timer.setVisibility(View.VISIBLE);
			Timer tim = new Timer();
			tim.schedule(new MyTask(), 1000, 1000);
		}else{
			timer.setText("00:00:00");
			record.setVisibility(View.INVISIBLE);
			timer.setVisibility(View.INVISIBLE);
		}
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data)
	{
	    
		switch (requestCode)
		{
		case REQ_BROWSER:
		    
			if (resultCode == RESULT_CANCELED)
				break;
			if(data!=null){
			int index = data.getExtras().getInt(WORK_INDEX);
			
			Station item = mChannelHolder.getItem(index);	
			
			mChannelHolder.mWorkFreq = Integer.valueOf(item.mFreq);
			startServiceByAction(FMService.SETCHANNEL_ACTION); 
			setWorkFreq(item.mName, mChannelHolder.mWorkFreq);}
			else{
                            startServiceByAction(FMService.SETCHANNEL_ACTION); 
			    setWorkFreq(null, mChannelHolder.mWorkFreq);
			}
			break;
		}
	}

	@Override
	protected Dialog onCreateDialog(int id)
	{
		switch (id)
		{
		case DIALOG_MSG_POWER_ON:
			return createPowerOnDialog();
			
		case DIALOG_MSG_POWER_OFF:
			return createPowerOffDialog();
		
		case DIALOG_MSG_CLEAR_SCAN:
			return createClearScanDialog();
			
		case DIALOG_MSG_SCAN_WAIT:
			return createScanWaitDialog();
			
		case DIALOG_MSG_HEADSET:
			return createHeadsetDialog();
		
		case DIALOG_SCAN_CLEAR:
			return createScanClearDialog();
	
		case DIALOG_MSG_OPEN_FAILED:
			return createOpenFailedDialog();

		case DIALOG_MSG_IN_CALL:
			return createInCallDialog();
		default:
			return null;
		}
	}

        private AlertDialog createInCallDialog()
        {
            LayoutInflater factory = LayoutInflater.from(this);
            AlertDialog.Builder mAlertDialog = new AlertDialog.Builder(this);
            return mAlertDialog.setView(factory.inflate(R.layout.dialog_incall, null)).setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int whichButton) {
                    mHandler.sendMessage(Message.obtain(mHandler, MSG_FINISH));
                }}).create();
        }

	private AlertDialog createOpenFailedDialog()
		{

		LayoutInflater factory = LayoutInflater.from(this);
		AlertDialog.Builder mAlertDialog = new AlertDialog.Builder(this);
		mAlertDialog.setView(factory.inflate(R.layout.dialog_open_failed, null));
					            
		return mAlertDialog.create();
					                    
		}
	private AlertDialog createScanClearDialog()
		{
	         
		LayoutInflater factory = LayoutInflater.from(this);
		AlertDialog.Builder mAlertDialog = new AlertDialog.Builder(this);
		mAlertDialog.setView(factory.inflate(R.layout.dialog_scan_clear, null));
				
		return maScan = mAlertDialog.create();
				    
		}

	private AlertDialog createHeadsetDialog()
	{
		LayoutInflater factory = LayoutInflater.from(this);
		AlertDialog.Builder mAlertDialog = new AlertDialog.Builder(this);
		mAlertDialog.setView(factory.inflate(R.layout.headset_on, null));
						            
		return mAlertDialog.create();
	}
	
	private AlertDialog createPowerOnDialog()
	{		
		if(maOn!=null)
					return maOn;
				else
							{

							
	        LayoutInflater factory = LayoutInflater.from(this);
		AlertDialog.Builder mAlertDialog = new AlertDialog.Builder(this);
		mAlertDialog.setView(factory.inflate(R.layout.dialog_power_on, null));
		
		return maOn = mAlertDialog.create();
							}
		
	}
	
	private AlertDialog createPowerOffDialog()
	{		
		LayoutInflater factory = LayoutInflater.from(this);
		AlertDialog.Builder mAlertDialog = new AlertDialog.Builder(this);
		mAlertDialog.setView(factory.inflate(R.layout.dialog_power_off, null));
		
		return ma = mAlertDialog.create();
	}
	
	private AlertDialog createClearScanDialog()
	{		
		LayoutInflater factory = LayoutInflater.from(this);
		AlertDialog.Builder mAlertDialog = new AlertDialog.Builder(this);
		mAlertDialog.setTitle(R.string.app_title);
		
		return mAlertDialog.setView(factory.inflate(R.layout.dialog_clear_scan, null)).setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                if(mPowerOn){
		    showDialog(DIALOG_SCAN_CLEAR);
		for (int pos = mChannelHolder.getCount() - 1 ; 0 <= pos ; pos--) 
                {
                 mChannelHolder.delete(pos);
                 }
                setWorkFreq(null, mChannelHolder.mWorkFreq);
		 mScanning.setVisibility(View.VISIBLE);
		startServiceByAction(FMService.SCANALL);
            mIsScanAll = true;
		 pbScan = (ProgressBar)maScan.findViewById(R.id.power_percent);   
		pbScan.setProgress(0);
            }
	    }
        }).setNegativeButton(R.string.cancel, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
            }
        }).create();
		
	}	
    
	
	private AlertDialog createScanWaitDialog()
	{		
		LayoutInflater factory = LayoutInflater.from(this);
		AlertDialog.Builder mAlertDialog = new AlertDialog.Builder(this);
		mAlertDialog.setView(factory.inflate(R.layout.dialog_scanning, null));
		
		return mAlertDialog.create();
	}
    
    private void setWorkFreq(String name, int freq)
    {if(name!=null)
        mFreqView.setName(name);
    else
        mFreqView.setName("");
    
     mFreqView.setFrequency(freq);
    }

    public boolean onTouch(View v, MotionEvent event) {
            return mGestureDetector.onTouchEvent(event);
    }

    private void onPowerOff(){
        if (!mPowerOn){
            return;
        }
        mPowerOn = false;
        Drawable off = getResources().getDrawable(R.drawable.power);
        power.setBackgroundDrawable(off);
        showDialog(DIALOG_MSG_POWER_OFF);
        startServiceByAction(FMService.DISABLE_ACTION);
        NotificationManager mNotificationManager = (NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE);
        mNotificationManager.cancel(1);
        FrequencyViewListener.on = false;
        mScrol.setClickable(false);
        rssi.setBackgroundResource(R.drawable.xinhao_00);
        Intent mIntent = new Intent(this,FMService.class);
        stopService(mIntent);
    }

    private void onPowerOn(){
        if (mPowerOn){
            return;
        }
        // Show dialog "please plug your headset" once found headset was unpluged.
        if(am.isWiredHeadsetOn() == false){
            showDialog(DIALOG_MSG_HEADSET);
        } else {
            WindowManager.LayoutParams lp = getWindow().getAttributes();
            lp.screenBrightness = 0.1f;
            getWindow().setAttributes(lp);
            startServiceByAction(FMService.ENABLE_ACTION);
            lp.screenBrightness = 1.0f;
            getWindow().setAttributes(lp);
            enableProgressBar();
        }
    }
    /**
     * Receiver for headset intent broadcasts the FM app cares about.
     */
    private class AudioServiceBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (action.equals(Intent.ACTION_HEADSET_PLUG)) {
                int state = intent.getIntExtra("state", 0);
                int microphone = intent.getIntExtra("microphone", 0);
                if (state == 0){
                    // headset unplug, only draw the dialog when activity is
                    // running on the forground
                    if (isResumed() == true)
                    showDialog(DIALOG_MSG_HEADSET);
                    // Power off FM
                    onPowerOff();
                }
                else if (state == 1){
                    // headset plugged
                    removeDialog(DIALOG_MSG_HEADSET);
                }
            }
        }
    }

}
