/*
 * Copyright (C) 2011 Marvell International Ltd., All Rights Reserved
 *
 * MARVELL CONFIDENTIAL
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
package com.marvell.wifidirect.demoapp;

import java.io.IOException;

import android.app.Service;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.app.ActivityManager;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Process;
import android.util.Log;
import android.widget.Toast;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;

import android.net.wifi.MrvlWifiManager;
import android.view.WindowManager;
import com.marvell.wifidirect.demoapp.server.HttpResponseDispatcher;
import com.marvell.wifidirect.demoapp.server.MyNanoHTTPD;
import com.marvell.wifidirect.demoapp.server.NanoHTTPD;

public class WifiDirectHttp extends Service {
    /** Called when the activity is first created. */

    private static final String LOG_TAG = "WifiDirectHttp";

    public enum ServerState {
        OFF,
        IDLE_STATE,
        WAIT_FOR_INCOMING_TRANSFER_CONFIRMATION_STATE,
        RECEIVING_FILE,
    }

    //events
    public enum SupportedEvents {
        START_SERVER,
        INCOMING_FILE,
        ACCEPT_INCOMING_FILE,
        REJECT_INCOMING_FILE,
        CANCEL_FILE_TRANSFER,
        INCOMING_FILE_COMPLETE,
        RESTART_SERVER,
        RECEIVED_ERROR
    }

    private ProgressDialog mProgressDialog;
    private static MyNanoHTTPD mWebServer;
    private static ServerState mState;

    private final Handler mHandler;
    private final HttpResponseDispatcher mResponseDispatcher;
    private final BroadcastReceiver mBroadcastReceiver;

    public WifiDirectHttp(){

        mState = ServerState.OFF;

        mHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                handleEvent(msg);
            }
        };

	mBroadcastReceiver = new BroadcastReceiver() {
		public void onReceive(Context context,Intent intent) {
			Log.d(LOG_TAG, "onReceive WifiDirectHttp: " + intent.getAction());
			handleIntent(intent);
		}
	};

        mResponseDispatcher = new HttpResponseDispatcher();
    }

    @Override
    public void onCreate() {
        super.onCreate();
    }


    @Override
    public void onStart(Intent intent,int startId) {
        Log.d(LOG_TAG, "onStart has been invoked!" + "startId:" + startId);
	handleIntent(intent);
    }

    @Override
    public int onStartCommand(Intent intent,int flags,int startId)
    {
	Log.d(LOG_TAG,"onStartCommand  has been invoked!" + "startId:" + startId);
	handleIntent(intent);
	return android.app.Service.START_STICKY;
    }

    @Override
    public void onDestroy() {
	MrvlWifiManager mMrvlWifiManager = (MrvlWifiManager) this.getSystemService(Context.MRVL_WIFI_SERVICE);

	if(mWebServer != null) {
	    mWebServer.stop();
	    mWebServer = null;
	    updateState(ServerState.OFF);
	}

        super.onDestroy();
        Log.d(LOG_TAG, "onDestroy has been invoked!");
        Process.killProcess(Process.myPid());
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private void handleIntent(Intent intent) {
	if(intent != null) {
	    if(intent.getAction().equals("com.marvell.wifidirect.demoapp.WFD_SERVER_START")){
		Message msg = Message.obtain(mHandler, SupportedEvents.START_SERVER.ordinal());
		msg.sendToTarget();
            }
	}
    }

    public void handleEvent(Message msg){

        SupportedEvents event = SupportedEvents.values()[msg.what];
        Log.d(LOG_TAG, "--- start of handleEvent --");
        Log.d(LOG_TAG, "Actual state BEFORE handling the event: " + mState);
        Log.d(LOG_TAG, "Type of event: " + event);
        if(msg.obj != null)
            Log.d(LOG_TAG, "Type of object: " + msg.obj.getClass().toString());

        switch(mState){
            case OFF:
                if(event == SupportedEvents.START_SERVER){
                    try{
                        if (mWebServer == null) {
			    mWebServer = new MyNanoHTTPD(8080, "WifiDirectFiles", this, mHandler, mResponseDispatcher);
                            updateState(ServerState.IDLE_STATE);
                        }
                    }
                    catch (IOException e) {
                        mWebServer = null;
                        e.printStackTrace();
                    }
                }
                break;
            case IDLE_STATE:
                if(event == SupportedEvents.INCOMING_FILE){
	            Bundle bundle = (Bundle) msg.obj;
                    String remotePeer = bundle.getString("peerName");
                    String fileName = bundle.getString("fileName");
                    AlertDialog.Builder builder = new AlertDialog.Builder(this);
                    builder.setIcon(R.drawable.demo_app_icon);
                    builder.setTitle("Incoming File");
                    StringBuilder sb = new StringBuilder();
                    sb.append("\"");
                    sb.append(remotePeer);
                    sb.append("\" wants to send you the file ");
                    sb.append("\"");
                    sb.append(fileName);
                    sb.append("\". Do you want to accept the transfer?");
                    builder.setMessage(sb.toString());
                    builder.setCancelable(false);
                    builder.setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {
                            Message msg = Message.obtain(mHandler, SupportedEvents.ACCEPT_INCOMING_FILE.ordinal());
                            msg.sendToTarget();
                        }
                    });
                    builder.setNegativeButton("No", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {
                            dialog.cancel();
                            Message msg = Message.obtain(mHandler, SupportedEvents.REJECT_INCOMING_FILE.ordinal());
                            msg.sendToTarget();
                        }
                    });
                    AlertDialog alert = builder.create();
		    alert.getWindow().setType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
                    alert.show();
                    updateState(ServerState.WAIT_FOR_INCOMING_TRANSFER_CONFIRMATION_STATE);
                }
                break;

            case WAIT_FOR_INCOMING_TRANSFER_CONFIRMATION_STATE:

                if(event == SupportedEvents.ACCEPT_INCOMING_FILE){
		    if(isExternalStorageMounted()) {
		        mProgressDialog = new ProgressDialog(this);
                        mProgressDialog.setIcon(R.drawable.demo_app_icon);
                        mProgressDialog.setTitle("Incoming file");
                        mProgressDialog.setMessage("Receiving file...");
                        mProgressDialog.setIndeterminate(true);
			mProgressDialog.getWindow().setType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
                        mProgressDialog.show();
                        mResponseDispatcher.putResponse(mWebServer.new Response( NanoHTTPD.HTTP_OK, NanoHTTPD.MIME_PLAINTEXT, "Send me the file" ));
                        updateState(ServerState.RECEIVING_FILE);
		    } else {
			mResponseDispatcher.putResponse(mWebServer.new Response( NanoHTTPD.HTTP_FORBIDDEN, NanoHTTPD.MIME_PLAINTEXT, "I want the file but do not have SD card" ));
			updateState(ServerState.IDLE_STATE);
		    }
                } else if(event == SupportedEvents.REJECT_INCOMING_FILE){
                    mResponseDispatcher.putResponse(mWebServer.new Response( NanoHTTPD.HTTP_FORBIDDEN, NanoHTTPD.MIME_PLAINTEXT, "I do not want the file" ));
                    updateState(ServerState.IDLE_STATE);
                }
                break;

            case RECEIVING_FILE:

                if(event == SupportedEvents.INCOMING_FILE_COMPLETE){
                    if(mProgressDialog != null){
                        mProgressDialog.dismiss();
                        mProgressDialog = null;
                        mResponseDispatcher.putResponse(mWebServer.new Response( NanoHTTPD.HTTP_OK, NanoHTTPD.MIME_PLAINTEXT, "File successfully received!" ));
                        showToastMsg("File successfully received!");
                        updateState(ServerState.IDLE_STATE);
                    }
                } else if( event == SupportedEvents.CANCEL_FILE_TRANSFER){

                }
                break;

        }

        if(event == SupportedEvents.RECEIVED_ERROR){
            if(mProgressDialog != null)
                mProgressDialog.dismiss();
            showToastMsg("An error has ocurred. Restarting the server!");
            Message.obtain(mHandler, SupportedEvents.RESTART_SERVER.ordinal()).sendToTarget();
        }

        if(event == SupportedEvents.RESTART_SERVER){
            if(mWebServer != null) {
	        mWebServer.stop();
                try{
                    mWebServer = new MyNanoHTTPD(8080, "WifiDirectFiles", this, mHandler, mResponseDispatcher);
                    updateState(ServerState.IDLE_STATE);
                    Log.d(LOG_TAG, "Wifi Direct Server Restart Successfully!");
                }
                catch (IOException e) {
                        mWebServer = null;
                        e.printStackTrace();
                }
	    }
        }
    }

    //This function can be called ONLY from handleEvent
    private void updateState(ServerState state) {
        Log.d(LOG_TAG, "Changing state: " + mState + " --> " + state);
        mState = state;
    }

    private boolean isExternalStorageMounted(){
        //Verify that the SD Card is mounted
        String state = android.os.Environment.getExternalStorageState();
        if(!state.equals(android.os.Environment.MEDIA_MOUNTED)){
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setIcon(R.drawable.ic_settings_storage);
            builder.setTitle("WARNING");
            builder.setMessage("The SD card is not mounted. Please, mount it in order to receive files.");
            builder.setCancelable(false);
            builder.setPositiveButton("Accept", null);
            AlertDialog alert = builder.create();
	    alert.getWindow().setType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
            alert.show();
            return false;
        }
        else
            return true;
    }

    private void showToastMsg(String msg){
        Toast.makeText(getBaseContext(), msg, Toast.LENGTH_SHORT).show();
    }
}
