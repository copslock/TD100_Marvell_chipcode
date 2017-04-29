/*****************************
 **  (C) Copyright 2011 Marvell International Ltd.
 **  All Rights Reserved
 **  Author Linda
 **  Create Date Sep 02 2011      
 */
package com.marvell.logtools;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

import java.io.File;
import java.io.IOException;

public class LogService extends Service{
    static final boolean DBG = true;
    private static final String TAG = "LogService";
    
    private MyBinder mBinder = new MyBinder();
    
    Process mProcess1 = null;
    Process mProcess2 = null;
    Process mProcess3 = null;
    
    File path = new File("/sdcard/Mlog");
    
    @Override
    public IBinder onBind(Intent intent) {
        Log.d(TAG, "start IBinder");
        return mBinder;
    }
    
    @Override
    public void onCreate() {
        Log.d(TAG, "start onCreate");
        super.onCreate();
    }
    
    @Override
    public void onStart(Intent intent, int startId) {
        Log.d(TAG, "start onStart");
        super.onStart(intent, startId);
    }
    
    @Override
    public void onDestroy() {
        Log.d(TAG, "start onDestroy");
        super.onDestroy();
    }
    
    @Override
    public boolean onUnbind(Intent intent) {
        Log.d(TAG, "start onUnbind");
        return super.onUnbind(intent);
    }
    
    public class MyBinder extends Binder {
        LogService getService()
        {
            return LogService.this;
        }
    }
    
    public boolean getDumpTcpProcess(){
        return (mProcess1!=null)?true:false;
    }
    
    public boolean getDumpMainProcess(){
        return (mProcess2!=null)?true:false;
    }
    
    public boolean getDumpRadioProcess(){
        return (mProcess3!=null)?true:false;
    }
    
    public void startExternalDumpTcp() {
        if(!path.exists()) {
            path.mkdir();
        }
        File file = new File(path, "tcpdump.pcap");
        if(!file.exists()) {
            try {
                file.createNewFile();
            }catch (IOException e) {
                if (DBG) Log.e(TAG, "error create file", e);
            }
        }
        String [] command = new String[] {"tcpdump", "-i", "any", "-p", "-s", "0","-w","/sdcard/Mlog/tcpdump.pcap"};
        try {
            mProcess1 = Runtime.getRuntime().exec(command);
            showDumpToFile("tcpdump.pcap");
        } catch (IOException e) {
            mProcess1 = null;
            if(DBG) Log.e(TAG, "mProcess1 get exception ", e);
            showErrorMsg("tcpdump");
        } 
    }
    
    public void startExternalDumpMain() {
        if(!path.exists()) {
            path.mkdir();
        }
        File file = new File(path, "curMain");
        if(!file.exists()) {
            try {
                file.createNewFile();
            }catch (IOException e) {
                if (DBG) Log.e(TAG, "error create file", e);
            }
        }
        String[] command = new String[] {"logcat", "-f", "/sdcard/Mlog/curMain", "-v", "threadtime"};
        try {
            mProcess2 = Runtime.getRuntime().exec(command);
            showDumpToFile("curMain");
        } catch (IOException e) {
            mProcess2 = null;
            if(DBG) Log.e(TAG, "mProcess2 get exception ", e);
            showErrorMsg("logcat -v threadtime");
        } 
    }
    
    public void startExternalDumpRadio() {
        if(!path.exists()) {
            path.mkdir();
        }
        File file = new File(path, "curRadio");
        if(!file.exists()) {
            try {
                file.createNewFile();
            }catch (IOException e) {
                Log.e("alogcat", "error create file", e);
            }
        }
        String[] command = new String[] {"logcat", "-b", "radio", "-f" ,"/sdcard/Mlog/curRadio" ,"-v", "time"};
        try {
            mProcess3 = Runtime.getRuntime().exec(command);
            showDumpToFile("curRadio");
        } catch (IOException e) {
            mProcess3 = null;
            if(DBG) Log.e(TAG, "mProcess3 get exception ", e);
            showErrorMsg("logcat -b radio");
        } 
    }
    
    public void stopExternalDumpTcp() {
       disableDumpToFile("tcpdump.pcap");
       if(mProcess1 != null) {
           mProcess1.destroy();
           mProcess1 = null;
       }
    }
    
    public void stopExternalDumpMain() {
        disableDumpToFile("curMain");
        if(mProcess2 != null) {
            mProcess2.destroy();
            mProcess2 = null;
        }
    }
    
    public void stopExternalDumpRadio() {
        disableDumpToFile("curRadio");
        if(mProcess3 != null) {
           mProcess3.destroy();
           mProcess3 = null;
       }
    }
    
    private void showDumpToFile(String file) {
        final File targetFile = new File("/sdcard/Mlog", file);                    
        String msg = getResources().getString(R.string.saving_log, targetFile.toString());
        Toast.makeText(this, msg, Toast.LENGTH_LONG).show();
    }
    
    private void disableDumpToFile(String file) {
        final File targetFile = new File("/sdcard/Mlog", file);                    
        String msg = getResources().getString(R.string.stopping_log,targetFile.toString());
        Toast.makeText(this, msg, Toast.LENGTH_LONG).show();
        targetFile.renameTo(new File("/sdcard/Mlog", file + ".bak")); 
    }
    
    private void showErrorMsg(String command) {
        String msg = getResources().getString(R.string.command_error_log, command);
        Toast.makeText(this, msg, Toast.LENGTH_LONG).show();
    }

}

