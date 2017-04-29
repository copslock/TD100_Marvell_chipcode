package com.marvell.updater;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.View;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.marvell.updater.DownloadService.DownloadServiceListener;
import com.marvell.updater.utils.Constants;

public class DownloadActivity extends Activity {
    
    private static final String TAG = "DownloadActivity";
    
    private ProgressBar mProgress;
    
    private TextView mTitleView;
    
    private TextView mPercentView;
    
    private DownloadService mService;
    
    @Override
    public void onCreate(Bundle savedInstanceState) {
	
	super.onCreate(savedInstanceState);
	
	setContentView(R.layout.download_main);
	
	Intent intent = getIntent();
	
	String model = intent.getStringExtra(Constants.TAG_MODEL);
	String branch = intent.getStringExtra(Constants.TAG_BRANCH);
	String date = intent.getStringExtra(Constants.TAG_DATE);
	
	
	final Intent service = new Intent(DownloadActivity.this, DownloadService.class);
	bindService(service, mConnection, Context.BIND_AUTO_CREATE);
	
	TextView title = (TextView) findViewById(R.id.download_message);
	title.setText(getString(R.string.downloading_message, date + "-" + model + "-" +branch));
	
	mProgress = (ProgressBar)findViewById(R.id.progress);
	mTitleView = (TextView)findViewById(R.id.title);
	mPercentView = (TextView)findViewById(R.id.percent);
	
	findViewById(R.id.cancel).setOnClickListener(new View.OnClickListener() {
	    
	    public void onClick(View v) {
		if (mService != null) {
		    mService.cancel();
		    finish();
		}
	    }
	});

	mTitleView.setText(date);
	
    }
    
    public void onDestroy() {
	unbindService(mConnection);
	if (mService != null) {
    	    mService.setListener(null);
            mService = null;
        }
	super.onDestroy();
    }
    
    private ServiceConnection mConnection = new ServiceConnection() {
	
        public void onServiceConnected(ComponentName className, IBinder service) {
            if (Constants.LOG_ON) Log.d(TAG, "=onServiceConnected");
            mService = ((DownloadService.LocalBinder)service).getService();
            mService.setListener(mListener);
        }

        public void onServiceDisconnected(ComponentName className) {
            if (Constants.LOG_ON) Log.d(TAG, "=onServiceDisconnected");
            if (mService != null) {
        	mService.setListener(null);
                mService = null;
            }
        }
    };
    
    private DownloadServiceListener mListener = new DownloadServiceListener() {
	
	public void onDownloadPercent(final int percent) {
	    
	    if (Constants.LOG_ON) Log.d(TAG, "= activity download percent");
	    runOnUiThread(new Runnable() {
		public void run() {
		    mProgress.setProgress(percent);
		    mPercentView.setText(percent + "%");
		}
	    });
	}
	
	public void onDownloadFinish() {
	    
	    if (Constants.LOG_ON) Log.d(TAG, "= activity download finish");
	    runOnUiThread(new Runnable() {
		public void run() {
		   finish();
		}
	    });
	}
    };
    
}
