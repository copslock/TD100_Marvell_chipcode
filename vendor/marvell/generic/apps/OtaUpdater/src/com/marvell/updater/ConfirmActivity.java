package com.marvell.updater;

import java.io.File;

import android.app.Activity;
import android.app.AlarmManager;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.PendingIntent;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;

import com.marvell.updater.utils.Constants;

public class ConfirmActivity extends Activity {
    
    private static final String TAG = "ConfirmActivity";
    private static final int PACKAGE_HAS_REMOVED = 1;

    private String mPath = null;
    
    @Override
    public void onCreate(Bundle savedInstanceState) {
	
	super.onCreate(savedInstanceState);
	
	Intent intent = getIntent();
	mPath = intent.getStringExtra(Constants.KEY_OTA_PATH);
	final File otaFile = new File(mPath);
	if (!otaFile.exists()) {
	    finish();
	    return;
	}

	requestWindowFeature(Window.FEATURE_NO_TITLE);
	setContentView(R.layout.update_confirm);
	
	if (Constants.LOG_ON) Log.d(TAG, "ota package path = " + mPath);
	
	
	findViewById(R.id.confirm_yes).setOnClickListener(new View.OnClickListener() {
	    
	    public void onClick(View v) {
		if (!otaFile.exists()) {
		    showDialog(PACKAGE_HAS_REMOVED);
		    return;
		}
		Intent intent = new Intent("com.marvell.intent.OTA_UPDATE");
		intent.putExtra(Constants.KEY_OTA_PATH, mPath);
		sendBroadcast(intent);
		if (Constants.LOG_ON) Log.d(TAG, "broadcast has been sent out, system will reboot");
		finish();
	    }
	});
	
	findViewById(R.id.confirm_later).setOnClickListener(new View.OnClickListener() {
	    
	    public void onClick(View v) {
		
		AlarmManager am = (AlarmManager) ConfirmActivity.this.getSystemService(Context.ALARM_SERVICE);
		Intent intent = new Intent("com.marvell.intent.UPDATE_CONFIRM");
		intent.putExtra(Constants.KEY_OTA_PATH, mPath);
                PendingIntent contentIntent = PendingIntent.getActivity(ConfirmActivity.this, 0,
                                intent, PendingIntent.FLAG_UPDATE_CURRENT);
                am.cancel(contentIntent);
		am.set(AlarmManager.RTC_WAKEUP, System.currentTimeMillis() + 
                        + Constants.CONFIRM_DELAY, contentIntent);
		finish();
	    }
	    
	});
	
    }

    public Dialog onCreateDialog(int id) {
	Dialog dialog = null;
	switch(id) {
	case PACKAGE_HAS_REMOVED :
	    AlertDialog.Builder builder = new AlertDialog.Builder(this);
	    String message = getString(R.string.package_uexist, mPath);
	    builder.setMessage(message);
	    builder.setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
	        public void onClick(DialogInterface dialog, int which) {
	            finish();
	        }
	    });
	    builder.setOnCancelListener(new DialogInterface.OnCancelListener() {
	        public void onCancel(DialogInterface dialog) {
	            finish();
	        }
	    });
	    dialog = builder.create();
	}
	return dialog;
    }

}
