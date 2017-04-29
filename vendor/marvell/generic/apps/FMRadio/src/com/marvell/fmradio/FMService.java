package com.marvell.fmradio;

import com.marvell.fmmanager.FMRadioManager;
import com.marvell.fmradio.util.ChannelHolder;
import android.media.AudioManager;
import android.media.AudioManager.OnAudioFocusChangeListener;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import java.util.Calendar;
import java.io.File;
import android.os.Environment;

public class FMService extends Service{
    private static final String LOGTAG = "FMService";
    private FMCallback mCallback = null;
    private ChannelHolder mChannelHolder = null;    
    private static Handler mHandler = null;
    private static boolean mute;
    private static boolean speaker;
    public static boolean isRecording = false;
    private final static int MSG_TUNE = 1;
    private final static int MSG_ENABLE = 3;
    private final static int MSG_SCAN = 2;
    private final static int MSG_SCAN_FAILED = 4;
    private final static int MSG_SCANALL = 5;
    private final static int MSG_DISMISS = 6;
    private final static int MSG_INITIATE = 7;
    private final static int MSG_FINISH = 8;
    private final static int MSG_DISMISS_SCAN = 15;
    private final static int MSG_POWER_ON = 17;
    private final static int MSG_POWER_OFF = 18;
    private final static int MSG_RECORD_SUCCESS = 19;
    private final static int MSG_RECORD_FAILED = 20;
    private final static int MSG_RECORD_NO_SDCARD = 21;
    private final static int MSG_RECORD_STOPPED = 22;
    private FMRadioManager mFMRadioManager = null;
    private AudioManager mAudioManager;
    public static final String WAITFOR_ACTION = "com.marvell.fmradio.WAIT_FOR";  
    public static final String ENABLE_ACTION = "com.marvell.fmradio.ENABLE";  
    public static final String SETCHANNEL_ACTION = "com.marvell.fmradio.SETCHANNEL";  
    public static final String DISABLE_ACTION = "com.marvell.fmradio.DISABLE"; 
    public static final String DISABLE_ONLY_ACTION = "com.marvell.fmradio.DISABLE_ONLY";
    public static final String SCANPREV_ACTION = "com.marvell.fmradio.SCANPREV"; 
    public static final String SCANNEXT_ACTION = "com.marvell.fmradio.SCANNEXT";
    public static final String MUTE_ACTION = "com.marvell.fmradio.MUTE";
    public static final String SPEAKER_ACTION = "com.marvell.fmradio.SPEAKER";
    public static final String INITIATE_ACTION = "com.marvell.fmradio.INITIATE";
    public static final String SCANALL = "com.marvell.fmradio.SCANALL";
    public static final String GETCHANNEL_ACTION = "com.marvell.fmradio.GETCHANNEL";
    public static final String STOPSCAN = "com.marvell.fmradio.STOPSCAN";
    public static final String SUSPEND_ACTION = "com.marvell.fmradio.SUSPEND";
    public static final String RESUME_ACTION = "com.marvell.fmradio.RESUME";
    public static final String RECORD_ACTION = "com.marvell.fmradio.RECORD";
    public static final String STOPRECORD_ACTION = "com.marvell.fmradio.STOPRECORD";
    private boolean mPausedByTransientLossOfFocus;

    @Override
    public IBinder onBind(Intent intent) {
        // TODO Auto-generated method stub
        return null;
    }
    @Override
    public void onCreate(){
        super.onCreate();  
        mFMRadioManager = new FMRadioManager();
        mCallback = FMCallback.getInstance();

        mChannelHolder = ChannelHolder.getInstance();
        mFMRadioManager.registerListener(mCallback);
        mAudioManager = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
       
    }
   
    @Override
    public void onDestroy(){
        super.onDestroy();
        mAudioManager.abandonAudioFocus(mAudioFocusListener); 
        mHandler.sendMessage(Message.obtain(mHandler, MSG_POWER_OFF));
    }
    @Override
   public int onStartCommand(Intent intent, int flags, int startId) {  
    super.onStart(intent, startId);  
        if(intent == null){
            stopSelf();
            return START_NOT_STICKY;
        }
        
       String action = intent.getAction();  
     if(action.equals(WAITFOR_ACTION)){  
            waitFor();  
           
            
         }
     else if(action.equals(INITIATE_ACTION)){
         Thread t = new Thread(){
             public void run(){
             mFMRadioManager.powerOn();
             MainActivity.enable = mFMRadioManager.enableRx();
            Intent intent = new Intent(ENABLE_ACTION);
            sendBroadcast(intent);
             mAudioManager.requestAudioFocus(mAudioFocusListener, AudioManager.STREAM_MUSIC,
                AudioManager.AUDIOFOCUS_GAIN);
             mFMRadioManager.stopScan();
		if(MainActivity.enable==0&&mChannelHolder.getStationList().size()==0)
		mHandler.sendMessage(Message.obtain(mHandler, MSG_SCANALL));
		else if(MainActivity.enable == -1||(MainActivity.enable==0&&mChannelHolder.getStationList().size()>0))
		mHandler.sendMessage(Message.obtain(mHandler, MSG_DISMISS));
		}
     
         };
         t.start();
     }
     else if(action.equals(ENABLE_ACTION)){

	              Thread t = new Thread(){

			public void run(){
			mFMRadioManager.powerOn();
			MainActivity.enable = mFMRadioManager.enableRx();
                        mAudioManager.requestAudioFocus(mAudioFocusListener, AudioManager.STREAM_MUSIC,
                            AudioManager.AUDIOFOCUS_GAIN);
			mHandler.sendMessage(Message.obtain(mHandler, MSG_ENABLE));
            Intent intent = new Intent(ENABLE_ACTION);
            sendBroadcast(intent);
			}
		      };
		      t.start();
     }else if(action.equals(GETCHANNEL_ACTION)){
	Thread t = new Thread(){

	public void run(){

	     mChannelHolder.mWorkFreq = mFMRadioManager.getChannel();
	mHandler.sendMessage(Message.obtain(mHandler, MSG_TUNE));	
	}
	};
	t.start();
	}
     else if(action.equals(SETCHANNEL_ACTION)){
      Thread t = new Thread(){
			public void run(){
	mFMRadioManager.setChannel(mChannelHolder.mWorkFreq);
	mHandler.sendMessage(Message.obtain(mHandler, MSG_TUNE));
			}
      };
      t.start();
     }else if(action.equals(DISABLE_ACTION)){
	        Thread t = new Thread(){
		public void run(){
         mFMRadioManager.disableRx();
         mFMRadioManager.powerOff();
	 mHandler.sendMessage(Message.obtain(mHandler, MSG_DISMISS));
            Intent intent = new Intent(DISABLE_ACTION);
            sendBroadcast(intent);
		}
		};
		t.start();
     }else if(action.equals(DISABLE_ONLY_ACTION)){
	        Thread t = new Thread(){
		public void run(){
         mFMRadioManager.disableRx();
	 mHandler.sendMessage(Message.obtain(mHandler, MSG_DISMISS));
            Intent intent = new Intent(DISABLE_ACTION);
            sendBroadcast(intent);
		}
		};
		t.start();
     }else if(action.equals(SCANPREV_ACTION)){
         Thread t = new Thread(){
             public void run(){

         int i = mFMRadioManager.scanPrev();
         if(i == -1)
	mHandler.sendMessage(Message.obtain(mHandler, MSG_SCAN_FAILED));
	        }
         };
         t.start();
        
     }else if(action.equals(SCANNEXT_ACTION)){
         Thread t = new Thread(){
             public void run(){
             int i = mFMRadioManager.scanNext();
		if(i == -1)
	        mHandler.sendMessage(Message.obtain(mHandler, MSG_SCAN_FAILED));
             }
	     
     
         };
         t.start();
        
     }else if(action.equals(MUTE_ACTION)){
          mute = intent.getBooleanExtra("mute", false);
	     Thread t = new Thread(){
		public void run(){
         mFMRadioManager.setMute(mute);
		}
	  };
	  t.start();
     }else if(action.equals(SPEAKER_ACTION)){
          speaker = intent.getBooleanExtra("speaker", false);
	     Thread t = new Thread(){

		public void run(){
		mFMRadioManager.setSpeakerOn(speaker);
		}
	  };
	  t.start();
     }else if(action.equals(SCANALL)){
         
         Thread t = new Thread(){
             public void run(){
		     int i = mFMRadioManager.scanAll();
		mFMRadioManager.setChannel(mChannelHolder.mWorkFreq);
		mHandler.sendMessage(Message.obtain(mHandler, MSG_DISMISS_SCAN));
		mHandler.sendMessage(Message.obtain(mHandler, MSG_DISMISS)); 
		if(i == -1)
	               mHandler.sendMessage(Message.obtain(mHandler, MSG_SCAN_FAILED));
         }
     
         };
         t.start();
     }else if(action.equals(STOPSCAN)){
		Thread t = new Thread(){
			public void run(){
		mFMRadioManager.stopScan();
		}
	};
		t.start();
     } else if (action.equals(SUSPEND_ACTION)){
        Thread t = new Thread(){
            public void run(){
                mFMRadioManager.suspend();
            }
        };
        t.start();
     } else if (action.equals(RESUME_ACTION)){
        Thread t = new Thread(){
            public void run(){
                mFMRadioManager.resume();
            }
        };
        t.start();
    } else if (action.equals(RECORD_ACTION)){
            Calendar c = Calendar.getInstance();
            int year   = c.get(Calendar.YEAR);
            //Android Jan ~ Dec : 0 ~ 11
            int month  = c.get(Calendar.MONTH) + 1;
            int date   = c.get(Calendar.DATE);
            int hour   = c.get(Calendar.HOUR);
            int minute = c.get(Calendar.MINUTE);
            int second = c.get(Calendar.SECOND);

            byte[] format = mFMRadioManager.getRecordFormat();
            String strFormat = new String(format);
            String fileName = year + "-" + month +"-" + date + "-" + hour + "-" + minute + "-" + second + "-" + "FM." + strFormat;
            if (Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)){
                File sdCardDir = Environment.getExternalStorageDirectory();
                final String filePath = sdCardDir.getAbsolutePath() + "/" + fileName;

                if(!isRecording){
                    Thread t = new Thread(){
                        public void run(){
                            int recRet = mFMRadioManager.startRecording(filePath);
                            Intent intent = new Intent(RECORD_ACTION);
                            sendBroadcast(intent);
                            if (0 == recRet){
                                mHandler.sendMessage(Message.obtain(mHandler, MSG_RECORD_SUCCESS));
                                isRecording = true;
                            } else {
                                mHandler.sendMessage(Message.obtain(mHandler, MSG_RECORD_FAILED));
                            }
                        }
                    };
                    t.start();
                }else{
                    Thread t = new Thread(){
                        public void run(){
                            int ret = mFMRadioManager.stopRecording();
                            Intent intent = new Intent(STOPRECORD_ACTION);
                            sendBroadcast(intent);
                            if(ret == 0){
                                Message m = mHandler.obtainMessage(MSG_RECORD_STOPPED, filePath);
                                mHandler.sendMessage(m);
                                isRecording = false;
                            }
                        }
                    };
                    t.start();
                }
            }
            else{
                mHandler.sendMessage(Message.obtain(mHandler, MSG_RECORD_NO_SDCARD));
            }
        }
        return START_NOT_STICKY;
}
   public static void setHandler(Handler handler)
	{

		         mHandler = handler;            
	}
   public void waitFor()
   {
                Runnable eventHandler = new Runnable()
                {
            public void run()
                        {
                mFMRadioManager.registerListener(mCallback);
               mHandler.sendMessage(Message.obtain(mHandler, MSG_INITIATE)); 
            }
                };

                Thread watcher = new Thread(eventHandler);
                watcher.start();
        }
    private OnAudioFocusChangeListener mAudioFocusListener = new OnAudioFocusChangeListener() {
        public void onAudioFocusChange(int focusChange) {
            // AudioFocus is a new feature: focus updates are made verbose on purpose
            switch (focusChange) {
                case AudioManager.AUDIOFOCUS_LOSS:
                    Log.v(LOGTAG, "AudioFocus: received AUDIOFOCUS_LOSS");
                    mPausedByTransientLossOfFocus = false;
                    mHandler.sendMessage(Message.obtain(mHandler, MSG_POWER_OFF));
                    break;
                case AudioManager.AUDIOFOCUS_LOSS_TRANSIENT:
                case AudioManager.AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK:
                    Log.v(LOGTAG, "AudioFocus: received AUDIOFOCUS_LOSS_TRANSIENT");
                    mPausedByTransientLossOfFocus = true;
                    mFMRadioManager.suspend();
                    break;
                case AudioManager.AUDIOFOCUS_GAIN:
                    Log.v(LOGTAG, "AudioFocus: received AUDIOFOCUS_GAIN");
                    if (mPausedByTransientLossOfFocus) {
                        mPausedByTransientLossOfFocus = false;
                        mFMRadioManager.resume();
                    }
                    break;
            }
        }
    };

}
