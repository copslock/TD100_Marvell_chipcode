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

import java.io.InputStream;

import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.mime.MultipartEntity;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.BasicHttpParams;
import org.apache.http.params.HttpConnectionParams;
import org.apache.http.params.HttpParams;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.net.wifi.MrvlWifiDirectConfiguration;
import android.net.wifi.MrvlWifiDirectInfo;
import android.net.wifi.MrvlWifiDirectPeer;
import android.net.wifi.MrvlWifiManager;
import android.net.wifi.MrvlWifiDirectInfo.WifiDirectConnectionState;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.Settings;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.marvell.wifidirect.demoapp.http.InputStreamKnownSizeBody;

public class WifiDirectHttpDialog extends Activity {

    private static final String LOG_TAG = "WifiDirectHttpDialog";

    private static final int POSITIVE_BUTTON_KEY = R.id.positive;
    private static final int NEGATIVE_BUTTON_KEY = R.id.negative;

    private static final String PROTOCOL = "http://";
    private static int PORT = 8080;

    //states
    private enum SenderState {
        INIT,
        WAIT_FOR_LOCAL_CONFIRMATION_STATE,
        WAITING_FOR_CONFIRMATION_STATE,
        SENDING_FILE_STATE,
        CANCELING_STATE
    }

    //events
    private enum SupportedEvents {
        CHECK_CONNECTION,
        START_TRANSER,
        CANCEL_TRANSFER,
        FILE_REQUEST_ACCEPTED,
        FILE_REQUEST_DENIED,
        CANCELED,
        RECEIVED_ERROR,
        FILE_SENT
    }

    //actual & last state
    private SenderState mState;

    //handles the messages send to the state machine
    private Handler mHandler;

    private TextView mDialogText;
    private Button mPositiveButton;
    private Button mNegativeButton;
    private LinearLayout mInnerContent;
    private TextView mProgressText;

    private MrvlWifiDirectPeer mPeer;

    private HttpClient mHttpClient;
    private HttpPost mHttpPost;
    private HttpGet mHttpGet;
    private HttpResponse mHttpResponse;
    private boolean mTransferCanceled = false;

    private Intent mIntent;

    private final OnClickListener mButtonListener = new OnClickListener() {
        public void onClick(View view) {
            handleOnClick( view );
        }
    };

    /**
     * Initialization of the Activity after it is first created.  Must at least
     * call {@link android.app.Activity#setContentView setContentView()} to
     * describe what is to be displayed in the screen.
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_LEFT_ICON);

        mHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                handleEvent(msg);
            }
        };

        mState = SenderState.INIT;

        setContentView(R.layout.dialog_activity);

        getWindow().setFeatureDrawableResource(Window.FEATURE_LEFT_ICON,
                R.drawable.demo_app_icon);

        mDialogText = (TextView) findViewById(R.id.dialog_activity_text);

        mPositiveButton = (Button)findViewById(POSITIVE_BUTTON_KEY);
        mPositiveButton.setOnClickListener(mButtonListener);
        mNegativeButton = (Button)findViewById(NEGATIVE_BUTTON_KEY);
        mNegativeButton.setOnClickListener(mButtonListener);

        mInnerContent = (LinearLayout) findViewById(R.id.inner_content);
        mProgressText = (TextView) findViewById(R.id.progress_bar_text);

        mInnerContent.setVisibility(View.GONE);
    }

    @Override
    protected void onResume() {
        super.onResume();

        Message msg = Message.obtain(mHandler, SupportedEvents.CHECK_CONNECTION.ordinal());
        msg.sendToTarget();

        Intent intent = this.getIntent();
        if(intent != null){
            String action = intent.getAction();
            if(Intent.ACTION_SEND.equals(action)){
                mIntent = intent;
            }
        }
   }

    @Override
    protected void onPause() {
        super.onPause();
    }

    private void handleOnClick(View v){

        if( v.getId() == POSITIVE_BUTTON_KEY ){
            if(mPositiveButton.getText().equals("Send")){
                Message msg = Message.obtain(mHandler, SupportedEvents.START_TRANSER.ordinal());
                msg.sendToTarget();
            }
            else if(mPositiveButton.getText().equals("Settings")){
                this.startActivity(new Intent(Settings.ACTION_WIFI_DIRECT_SETTINGS));
            }
        }
        else if( v.getId() == NEGATIVE_BUTTON_KEY ){
            Message msg = Message.obtain(mHandler, SupportedEvents.CANCEL_TRANSFER.ordinal());
            msg.sendToTarget();
        }
    }

    private void handleEvent(Message msg){

        SupportedEvents event = SupportedEvents.values()[msg.what];
        Log.d(LOG_TAG, "Actual state BEFORE handling the event: " + mState);
        Log.d(LOG_TAG, "Type of event: " + event);

        switch(mState){

            case INIT:

                if(event == SupportedEvents.CHECK_CONNECTION){
                    MrvlWifiManager mMrvlWifiManager = (MrvlWifiManager) getSystemService(Context.MRVL_WIFI_SERVICE);

                    if(mMrvlWifiManager.isWifiDirectEnabled()){
                        MrvlWifiDirectInfo mInfo = mMrvlWifiManager.getWifiDirectConnectionInfo();

                        if(mInfo.mConnectedPeers.size() ==0){
                            mDialogText.setText("There is no peers connected. Please connect to one");
                            mPositiveButton.setText("Settings");
                        }
                        else{
                            mDialogText.setText("Do you want to send this file?");
                            mPositiveButton.setText("Send");
                            updateState(SenderState.WAIT_FOR_LOCAL_CONFIRMATION_STATE);
                        }
                    }
                    else{
                        mDialogText.setText("Wifi Direct is turned off. Do you want to turn it on?");
                        mPositiveButton.setText("Settings");
                    }
                }
                else if(event == SupportedEvents.CANCEL_TRANSFER){
                    this.finish();
                }

                break;

            case WAIT_FOR_LOCAL_CONFIRMATION_STATE:

                if(event == SupportedEvents.START_TRANSER){

                    mPeer = getDestinationPeer();

					MrvlWifiManager mMrvlWifiManager = (MrvlWifiManager) getSystemService(Context.MRVL_WIFI_SERVICE);

					MrvlWifiDirectConfiguration config = mMrvlWifiManager.getWifiDirectConfiguration();

                    if(mPeer != null) {

                        //Hide the main text and show the progress spinner
                        mDialogText.setVisibility(View.GONE);
                        mProgressText.setText("Waiting for authorization");
                        mInnerContent.setVisibility(View.VISIBLE);
                        mPositiveButton.setVisibility(View.GONE);
                        mNegativeButton.setVisibility(View.GONE);

                        Uri uriPath = (Uri) mIntent.getParcelableExtra(Intent.EXTRA_STREAM);
                        updateState(SenderState.WAITING_FOR_CONFIRMATION_STATE);

                        sendGetRequest(config.mName,uriPath.getLastPathSegment(), mPeer.ipAddress, PORT);
                    }
                    else{
                        mDialogText.setText("Cann't connect. No peers.");
                        mPositiveButton.setVisibility(View.GONE);
                        mNegativeButton.setText("Ok");
                    }
                } else if(event == SupportedEvents.CANCEL_TRANSFER){
                    this.finish();
                }

                break;

            case WAITING_FOR_CONFIRMATION_STATE:

                if( event == SupportedEvents.FILE_REQUEST_ACCEPTED ){
                    mProgressText.setText("Sending...");
                    mInnerContent.refreshDrawableState();

                    String mimeType = mIntent.getType();
                    Uri uriPath = (Uri) mIntent.getParcelableExtra(Intent.EXTRA_STREAM);
                    updateState(SenderState.SENDING_FILE_STATE);
                    sendFileTo(uriPath, mimeType, uriPath.getLastPathSegment(), mPeer.ipAddress, PORT);

                } else if( event == SupportedEvents.FILE_REQUEST_DENIED ){
                    mDialogText.setText("File transfer rejected");
                    mDialogText.setVisibility(View.VISIBLE);
                    mInnerContent.setVisibility(View.GONE);
                    mNegativeButton.setText("OK");
                    mNegativeButton.setVisibility(View.VISIBLE);
                    updateState(SenderState.CANCELING_STATE);
                } else if( event == SupportedEvents.CANCEL_TRANSFER ) {
                    updateState(SenderState.CANCELING_STATE);
                    mTransferCanceled = true;
                    mNegativeButton.setFocusable(false);
                    mHttpGet.abort();
                } else if(event == SupportedEvents.RECEIVED_ERROR){
                    showToastMsg("An error has occurred");
                    mInnerContent.setVisibility(View.GONE);
                    mDialogText.setVisibility(View.VISIBLE);
                    mPositiveButton.setVisibility(View.VISIBLE);
                    mNegativeButton.setVisibility(View.VISIBLE);
                    updateState(SenderState.WAIT_FOR_LOCAL_CONFIRMATION_STATE);
                }

                break;

            case SENDING_FILE_STATE:

                if(event == SupportedEvents.FILE_SENT ){
                    showToastMsg("File successfully transfered!");
                    this.finish();

                } else if(event == SupportedEvents.RECEIVED_ERROR){
                    showToastMsg("An error has occurred");
                    mInnerContent.setVisibility(View.GONE);
                    mDialogText.setVisibility(View.VISIBLE);
                    mPositiveButton.setVisibility(View.VISIBLE);
                    mNegativeButton.setVisibility(View.VISIBLE);
                    updateState(SenderState.WAIT_FOR_LOCAL_CONFIRMATION_STATE);

                } else if(event == SupportedEvents.CANCEL_TRANSFER){
                    updateState(SenderState.CANCELING_STATE);
                    mTransferCanceled = true;
                    mNegativeButton.setClickable(false);
                    mNegativeButton.setFocusable(false);
                    mHttpPost.abort();
                }
                break;

            case CANCELING_STATE:

                if(event == SupportedEvents.CANCELED ){
                    showToastMsg("Transfer Canceled!");
                    this.finish();
                } else if(event == SupportedEvents.CANCEL_TRANSFER){
                    this.finish();
                }
                break;
        }
    }

    //This function can be called ONLY from handleEvent
    private void updateState(SenderState state) {
        Log.d(LOG_TAG, "Changing state: " + mState + " --> " + state);
        mState = state;
    }

    private void showToastMsg(String msg){
        Toast.makeText(getBaseContext(), msg, Toast.LENGTH_SHORT).show();
    }

    private MrvlWifiDirectPeer getDestinationPeer(){
        MrvlWifiManager mMrvlWifiManager = (MrvlWifiManager) getSystemService(Context.MRVL_WIFI_SERVICE);
        MrvlWifiDirectInfo mInfo = mMrvlWifiManager.getWifiDirectConnectionInfo();
        if(mInfo.mState == WifiDirectConnectionState.CONNECTED){
            /* TODO: Eventually, multiple peers will be supported and we will
             * have to preset a list to the user so they can pick a destination
             * peer.
             */
            return mInfo.mConnectedPeers.get(0);
        }
        else{
            return null;
        }
    }

    private void sendGetRequest(String localPeerName, String fileName, String ip, int port){

        mHttpClient = new DefaultHttpClient();

        StringBuilder url = new StringBuilder();
        url.append(PROTOCOL);
        url.append(ip);
        url.append(":");
        url.append(port);
        url.append("/wifi/direct/demo/app?peerName=");
        url.append(localPeerName);
        url.append("&fileName=");
        url.append(fileName);
        mHttpGet = new HttpGet(url.toString());

        new Thread(){

            @Override
            public void run() {
                try {
                    this.sleep(3000);
                    mHttpResponse = mHttpClient.execute(mHttpGet);
                    mHttpClient.getConnectionManager().shutdown();
                    mHttpClient = null;
                    int statusCode = mHttpResponse.getStatusLine().getStatusCode();
                    if(statusCode == 200){
                        Message msg = Message.obtain(mHandler, SupportedEvents.FILE_REQUEST_ACCEPTED.ordinal());
                        msg.sendToTarget();
                    } else if( statusCode == 403 ){
                        Message msg = Message.obtain(mHandler, SupportedEvents.FILE_REQUEST_DENIED.ordinal());
                        msg.sendToTarget();
                    }

                } catch (Exception e){
                    mHttpClient.getConnectionManager().shutdown();
                    mHttpClient = null;
                    if(e instanceof InterruptedException || mTransferCanceled == true){
                        Log.d(LOG_TAG, "OPS! Exception: InterruptedException -> Sending CANCELED");
                        Message msg = Message.obtain(mHandler, SupportedEvents.CANCELED.ordinal());
                        msg.sendToTarget();
                    } else {
                        Log.e(LOG_TAG, "Exception: " + e.getClass());
                        e.printStackTrace();
                        Message msg = Message.obtain(mHandler, SupportedEvents.RECEIVED_ERROR.ordinal());
                        msg.sendToTarget();
                    }
                } finally{
                    mHttpGet = null;
                }
            }
        }.start();
    }

    private void sendFileTo(Uri path, String mimeType, String fileName, String ip, int port){

        try {

            HttpParams httpParams = new BasicHttpParams();
            HttpConnectionParams.setConnectionTimeout(httpParams, 10000);
            mHttpClient = new DefaultHttpClient(httpParams);

            StringBuilder url = new StringBuilder();
            url.append(PROTOCOL);
            url.append(ip);
            url.append(":");
            url.append(port);
            url.append("/wifi/direct/demo/app");
            Log.d(LOG_TAG, "URL to send -->" + url.toString());
            Log.d(LOG_TAG, "path -->" + path.toString());
            mHttpPost = new HttpPost(url.toString());

            InputStream is = getContentResolver().openInputStream(path);
            MultipartEntity reqEntity = new MultipartEntity();
            InputStreamKnownSizeBody isb = new InputStreamKnownSizeBody(is, mimeType, fileName);

            reqEntity.addPart("bin", isb);
            mHttpPost.setEntity(reqEntity);

            new Thread(){

                @Override
                public void run() {

                    try {
                        mHttpResponse = mHttpClient.execute(mHttpPost);
                        int statusCode = mHttpResponse.getStatusLine().getStatusCode();
                        if(statusCode == 200){
                            Message msg = Message.obtain(mHandler, SupportedEvents.FILE_SENT.ordinal());
                            msg.sendToTarget();
                        } else {
                            Message msg = Message.obtain(mHandler, SupportedEvents.RECEIVED_ERROR.ordinal());
                            msg.sendToTarget();
                        }
                    } catch(Exception e) {
                        if(e instanceof InterruptedException || mTransferCanceled == true){
                            Log.d(LOG_TAG, "OPS! Exception: InterruptedException -> Sending CANCELED");
                            Message msg = Message.obtain(mHandler, SupportedEvents.CANCELED.ordinal());
                            msg.sendToTarget();
                        } else{
                            e.printStackTrace();
                            Log.e(LOG_TAG, "Exception e: " + e.getClass());
                            Message msg = Message.obtain(mHandler, SupportedEvents.RECEIVED_ERROR.ordinal());
                            msg.sendToTarget();
                        }
                    }
                    finally{
                        mHttpClient.getConnectionManager().shutdown();
                        mHttpClient = null;
                        mHttpPost = null;
                    }
                }
            }.start();

        } catch (Exception e){
            e.printStackTrace();
            Log.e(LOG_TAG, "Exception e: " + e.getClass());
            Message msg = Message.obtain(mHandler, SupportedEvents.RECEIVED_ERROR.ordinal());
            msg.sendToTarget();
        }
    }
}
