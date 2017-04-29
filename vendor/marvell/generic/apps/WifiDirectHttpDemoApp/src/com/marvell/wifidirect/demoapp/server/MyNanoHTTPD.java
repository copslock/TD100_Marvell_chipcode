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
package com.marvell.wifidirect.demoapp.server;

import java.io.IOException;
import java.util.Properties;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

import com.marvell.wifidirect.demoapp.R;
import com.marvell.wifidirect.demoapp.WifiDirectHttp;
import com.marvell.wifidirect.demoapp.WifiDirectHttp.SupportedEvents;

public class MyNanoHTTPD extends NanoHTTPD{

    private static final String LOG_TAG = "MyNanoHTTPD";
    private final Context mContext;
    private final HttpResponseDispatcher mResponseDispatcher;

    public static final int NOTIFICATION_ID = 2;

    public MyNanoHTTPD(int port, String folderName, Context context, Handler handler, HttpResponseDispatcher resopnseDispatcher) throws IOException{
        super(port, folderName, context, handler);
        mContext = context;
        mResponseDispatcher = resopnseDispatcher;
    }

    @Override
    public Response serve( String uri, String method, Properties header, Properties parms, Properties files ){

        if(uri.equals("/wifi/direct/demo/app")){
            if(method.equals("GET")){
                Bundle bundle = new Bundle();
                String peerName = (String)parms.get("peerName");
                String fileName = (String)parms.get("fileName");
                bundle.putString("peerName", peerName);
                bundle.putString("fileName", fileName);

                displayNotification("New incoming file", "Incoming file", peerName + "wants to send you " + fileName);

                Message msg = Message.obtain(mHandler, SupportedEvents.INCOMING_FILE.ordinal(), bundle);
                msg.sendToTarget();
            } else if(method.equals("POST")){
                Message msg = Message.obtain(mHandler, SupportedEvents.INCOMING_FILE_COMPLETE.ordinal());
                msg.sendToTarget();
            }
            return mResponseDispatcher.getResponse();
        }
        else
            return new NanoHTTPD.Response( HTTP_NOTFOUND, MIME_PLAINTEXT, "Nothing here");
    }

    public void displayNotification(String msg, String title, String body){
        Notification notification = new Notification(R.drawable.incoming_file_notification, msg, System.currentTimeMillis());

        Intent intent = new Intent(mContext.getApplicationContext(), WifiDirectHttp.class);

        intent.setAction("android.intent.action.MAIN");
        intent.addCategory("android.intent.category.LAUNCHER");
        intent.addFlags(intent.FLAG_ACTIVITY_REORDER_TO_FRONT);
        PendingIntent pending = PendingIntent.getActivity(mContext, 0,
                intent, PendingIntent.FLAG_ONE_SHOT);

        notification.setLatestEventInfo(mContext, title, body, pending);
        notification.flags |= Notification.FLAG_AUTO_CANCEL;
        notification.defaults |= Notification.DEFAULT_SOUND;

        NotificationManager manager = (NotificationManager)
                mContext.getSystemService(Context.NOTIFICATION_SERVICE);
        manager.notify(NOTIFICATION_ID, notification);
    }
}
