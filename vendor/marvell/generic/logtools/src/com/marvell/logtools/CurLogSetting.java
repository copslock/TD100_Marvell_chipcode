/*****************************
 **  (C) Copyright 2011 Marvell International Ltd.
 **  All Rights Reserved
 **  Author Linda
 **  Create Date Sep 02 2011      
 */
package com.marvell.logtools;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import java.io.File;

public class CurLogSetting extends Activity {
    static final boolean DBG = true;
    private static final String TAG = "CurrentLogSetting";
    ViewGroup mLayout;

    static class Item {
        View mRoot;
        Button mStart;
        Button mStop;
    }

    Item mExternalDumpTcp;
    Item mExternalDumpMain;
    Item mExternalDumpRadio;

    Process mProcess1 = null;
    Process mProcess2 = null;
    Process mProcess3 = null;
   
    File path = new File("/sdcard/Mlog");
    
    private LogService mMyService;
    
    private ServiceConnection mServiceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName arg0, IBinder arg1) {
            if(DBG) Log.d(TAG, "LogService Connected");
            mMyService = ((LogService.MyBinder)arg1).getService(); 
            updateState();
        }
        @Override
        public void onServiceDisconnected(ComponentName arg0) {
            if(DBG) Log.d(TAG, "LogService Disconnected");            
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.save_dump);
        mLayout = (ViewGroup)findViewById(R.id.layout);
        
        File file = new File(path, "tcpdump.pcap");
        mExternalDumpTcp = createDumpControls(
                "Tcp Dump",
                file,
                new View.OnClickListener() {
                    public void onClick(View v) {
                        mMyService.startExternalDumpTcp();
                        updateState();
                    }
                },
                new View.OnClickListener() {
                    public void onClick(View v) {
                         mMyService.stopExternalDumpTcp();
                         updateState();
                    }
                });
        mLayout.addView(mExternalDumpTcp.mRoot);
        
        file = new File(path, "curMain");
        mExternalDumpMain = createDumpControls(
                "Android Main log dump",
                file,
                new View.OnClickListener() {
                    public void onClick(View v) {
                        mMyService.startExternalDumpMain();
                        updateState();
                    }
                },
                new View.OnClickListener() {
                    public void onClick(View v) {
                        mMyService.stopExternalDumpMain();
                        updateState();
                    }
                });
        mLayout.addView(mExternalDumpMain.mRoot);
        
        file = new File(path, "curRadio");
        mExternalDumpRadio = createDumpControls(
                "Android Radio log dump",
                file,
                new View.OnClickListener() {
                    public void onClick(View v) {
                        mMyService.startExternalDumpRadio();
                        updateState();
                    }
                },
                new View.OnClickListener() {
                    public void onClick(View v) {
                        mMyService.stopExternalDumpRadio();
                        updateState();
                    }
                });
        mLayout.addView(mExternalDumpRadio.mRoot);
    }

    @Override 
    protected void onResume() {
        super.onResume();
        Intent service = new Intent(CurLogSetting.this, LogService.class);
        bindService(service, mServiceConnection, 0);
        startService(service);
        if(DBG) Log.d(TAG, "onResume and start the LogService");
    }

    @Override
    protected void onPause() {
        super.onPause();
        if(DBG) Log.d(TAG, "onPause process"); 
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    void updateState() {
        boolean has = mMyService.getDumpTcpProcess();
        mExternalDumpTcp.mStart.setEnabled(!has);
        mExternalDumpTcp.mStop.setEnabled(has);
        if(DBG) Log.d(TAG, "updateState Tcp Process run " + has);
        has = mMyService.getDumpMainProcess();
        mExternalDumpMain.mStart.setEnabled(!has);
        mExternalDumpMain.mStop.setEnabled(has);
        if(DBG) Log.d(TAG, "updateState Main Process run " + has);
        has = mMyService.getDumpRadioProcess();
        mExternalDumpRadio.mStart.setEnabled(!has);
        mExternalDumpRadio.mStop.setEnabled(has);
        if(DBG) Log.d(TAG, "updateState Radio Process run " + has);
    }

    boolean hasExternalDumpTcp() {
       File file = new File(path, "tcpdump.pcap");
       return file.exists();
    }

    boolean hasExternalDumpMain() {
        File file = new File(path, "curMain");
        return file.exists();
    }

    boolean hasExternalDumpRadio() {
        File file = new File(path, "curRadio");
        return file.exists();
    }
    
    Item createDumpControls(CharSequence label, File file,
            View.OnClickListener startClick,
            View.OnClickListener stopClick) {
        LayoutInflater inflater = (LayoutInflater)getSystemService(LAYOUT_INFLATER_SERVICE);
        Item item = new Item();
        item.mRoot = inflater.inflate(R.layout.save_dump_item, null);
        TextView tv = (TextView)item.mRoot.findViewById(R.id.label);
        tv.setText(label);
        if (file != null) {
            tv = (TextView)item.mRoot.findViewById(R.id.path);
            tv.setText(file.toString());
        }
        item.mStart = (Button)item.mRoot.findViewById(R.id.start);
        item.mStart.setOnClickListener(startClick);
        item.mStop = (Button)item.mRoot.findViewById(R.id.stop);
        item.mStop.setOnClickListener(stopClick);
        return item;
    }
}
