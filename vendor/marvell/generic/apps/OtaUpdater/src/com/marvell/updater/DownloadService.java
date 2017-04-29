package com.marvell.updater;

import java.util.ArrayList;

import android.app.AlertDialog;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.widget.RemoteViews;
import android.widget.Toast;

import com.marvell.updater.net.DownloadThread;
import com.marvell.updater.net.DownloadThread.DownloadListener;
import com.marvell.updater.utils.Constants;

public class DownloadService extends Service {
    
    private static final String TAG = "DownloadService";
    
    public static final int DOWNLOAD_STARTED = 0;
    
    public static final int DOWNLOAD_FINISH = 1;

    public static final int DOWNLOAD_PERCENT = 2;

    public static final int NETWORK_ERROR = 3;
    
    public static final int SYSTEM_ERROR = 4;
    
    private ArrayList<Thread> mDownloadList = new ArrayList<Thread>();
    
    private DownloadThread mDownloadThread;
    
    private String mLocalPath;
    
    private NotificationManager mNotificationManager;
    
    private Notification mNotification;
    
    private long mTotalSize;
    
    private long mDownloadSize;
    
    private int mPercent;
    
    private String mModel;
    
    private String mBranch;
    
    private String mDate;
    
    private DownloadServiceListener mListener;
    
    private Binder mLocalBinder = new LocalBinder();
    
    public class LocalBinder extends Binder {
        DownloadService getService() {
            return DownloadService.this;
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
	if (Constants.LOG_ON) Log.d(TAG, "onBind");
	return mLocalBinder;
    }
    
    public void onCreate() {
	super.onCreate();
	if (Constants.LOG_ON) Log.d(TAG, "onCreate");
    }
    
    @Override
    public void onStart(Intent intent, int startId) {
	super.onStart(intent, startId);

	if (Constants.LOG_ON) Log.d(TAG, "onStart");
	
	if (mDownloadList.size() > 0) {
	    Toast.makeText(DownloadService.this, R.string.downloading_alert, Toast.LENGTH_SHORT).show();
	    return;
	}
	
	String url = intent.getStringExtra(Constants.KEY_OTA_PATH);
	mModel = intent.getStringExtra(Constants.TAG_MODEL);
	mBranch = intent.getStringExtra(Constants.TAG_BRANCH);
	mDate = intent.getStringExtra(Constants.TAG_DATE);
	
	mDownloadThread = new DownloadThread(url);
	mDownloadList.add(mDownloadThread);
	mDownloadThread.setOnDownloadListener(new OtaDownloadListener());
	mDownloadThread.start();
	
    }
    
    public void cancel() {
	
	if(mDownloadThread != null && mDownloadThread.isAlive()) {
	    mDownloadThread.cancel();
	    mDownloadList.clear();
	}
	
	if (mNotificationManager != null) {
	    mNotificationManager.cancel(TAG, 1024);
	}
	
    }
    
    public void setListener(DownloadServiceListener listener) {
	this.mListener = listener;
    }
    
    public class OtaDownloadListener implements DownloadListener {
	
	public void onDownloadFinish(String localPath) {
	    
	    mDownloadList.clear();
	    mLocalPath = localPath;
	    if(mListener != null) {
		mListener.onDownloadFinish();
	    }
	    mHandler.sendEmptyMessage(DOWNLOAD_FINISH);
	    
	}

	public void onNetworkException() {
	    mHandler.sendEmptyMessage(NETWORK_ERROR);
	}
	
	public void onDownloadStarted() {
	    
	    mHandler.sendEmptyMessage(DOWNLOAD_STARTED);
	    
	}
	
	public void onDownloadPercent(int percent) {
	    
	    if(mListener != null) {
		mListener.onDownloadPercent(percent);
	    }
	    
	    mPercent = percent;
	    mHandler.sendEmptyMessage(DOWNLOAD_PERCENT);
	    
	}
    }
    
    private Handler mHandler = new Handler(Looper.myLooper()) {

	@Override
	public void handleMessage(Message msg) {
	    super.handleMessage(msg);
	    
	    switch (msg.what) {
	    
	    case DOWNLOAD_FINISH:
		
		if (mNotificationManager != null) {
		    mNotificationManager.cancel(TAG, 1024);
		}
		
		if (Constants.LOG_ON) Log.d(TAG, "=download finish");
		Intent intent = new Intent("com.marvell.intent.UPDATE_CONFIRM");
		intent.putExtra(Constants.KEY_OTA_PATH, mLocalPath);
		intent.setFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP  | Intent.FLAG_ACTIVITY_NEW_TASK);
		startActivity(intent);
		
		stopSelf();
		
		break;
		
	    case DOWNLOAD_STARTED:	
		
		if (Constants.LOG_ON) Log.d(TAG, "=download started");
		mNotificationManager = (NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE);
		mNotification = new Notification();
		mNotification.icon = R.drawable.download_icon;
		mNotification.contentView = new RemoteViews(getPackageName(), R.layout.download_notification);
		Intent intent1 = new Intent(DownloadService.this, DownloadActivity.class);
		intent1.setFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
		intent1.putExtra(Constants.TAG_MODEL, mModel);
		intent1.putExtra(Constants.TAG_BRANCH, mBranch);
		intent1.putExtra(Constants.TAG_DATE, mDate);
		PendingIntent contentIntent = PendingIntent.getActivity(DownloadService.this, 0, intent1, 0);
		mNotification.contentIntent = contentIntent;
		mNotification.contentView.setProgressBar(R.id.progress, 100, mPercent, false);
		mNotification.contentView.setTextViewText(R.id.title, mDate);
		mNotification.contentView.setTextViewText(R.id.percent, mPercent + "%");
		mNotificationManager.notify(TAG, 1024, mNotification);
		
	    case DOWNLOAD_PERCENT:
		
		if (Constants.LOG_ON) Log.d(TAG, "====mPercent = " + mPercent);
		mNotification.contentView.setProgressBar(R.id.progress, 100, mPercent, false);
		mNotification.contentView.setTextViewText(R.id.percent, mPercent + "%");
		mNotificationManager.notify(TAG, 1024, mNotification);
		
		break;

	    case NETWORK_ERROR:
		Toast.makeText(DownloadService.this, R.string.network_error, Toast.LENGTH_SHORT).show();
		break;
		
	    }
	}
	
    };
    
    public interface DownloadServiceListener {
	
	public void onDownloadPercent(int percent);
	
	public void onDownloadFinish();
	
    }

}
