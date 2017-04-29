/*
 * Copyright (C) 2011 Marvell International Ltd.
 * All Rights Reserved
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
 */
package com.marvell.cpugadget;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.OutputStream;

import java.lang.InterruptedException;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.os.IBinder;
import android.util.Log;

public class CpufreqPlugin extends Service {
    static final String INTENT_PREFIX = "android.intent.action.";
    static final String TAG = "CpufreqPlugin";
    static final String CONFIGURATION = "/system/etc/cpufreqd.conf";

    BroadcastReceiver mReceiver;
    LocalServerSocket mSrvSock;

    String sockPath;

    BlockingQueue<Intent> mPendingIntents = new LinkedBlockingQueue<Intent>();

    private IntentFilter parseConfiguration()
            throws FileNotFoundException, IOException {
        IntentFilter filter = new IntentFilter();
        BufferedReader file = new BufferedReader(
            new FileReader(CONFIGURATION));
        String data;
        boolean android_section = false;

        while ((data = file.readLine()) != null) {
            data.trim();
            if (data.equals(new String("")) || data.length() < 6)
                continue;
            if (data.equals(new String("[android]"))) {
                android_section = true;
            } else if (android_section == true) {
                if (data.substring(0,6).equals(new String("socket"))) {
                    int index = data.indexOf("=");
                    sockPath = data.substring(index + 1);
                    sockPath.trim();
                    Log.d(TAG, "sockPath is " + sockPath);
                } else if (data.substring(0, 5).equals(new String("event"))) {
                    String intentName;
                    int index = data.indexOf("=");
                    String event = data.substring(index + 1);
                    event.trim();
                    index = event.indexOf(":");
                    intentName = event.substring(0, index);
                    intentName.trim();
                    Log.d(TAG, "intentName is " + intentName);
                    if (intentName.equals(new String("BOOT_COMPLETED")))
                        //If we are interested in BOOT_COMPLETED, queue it.
                        mPendingIntents.offer(new Intent(Intent.ACTION_BOOT_COMPLETED));
                    else {
                        //add to intent filer
                        if (intentName.indexOf(".") == -1)
                            intentName = INTENT_PREFIX + intentName;
                        filter.addAction(intentName);
                    }
                } else if (data.equals(new String("[/android]"))) {
                    break;
                }
            }
        }
        return filter;
    }

    public void onCreate() {
        IntentFilter filter;
        mReceiver = new IntentReceiver(mPendingIntents);

        //parse the configuration of cpufreqd
        try {
            filter = parseConfiguration();
        } catch (IOException e) {
            Log.d(TAG, "onCreate: read configuration: " + e);
            return;
        }

        //register receiver
        registerReceiver(mReceiver, filter);
        //create server socket
        try {
            mSrvSock = new LocalServerSocket(sockPath);
        } catch (IOException e) {
            Log.d(TAG, "onCreate: create server socket: " + e);
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        //Spawn the bridger
        Thread bridger = new Thread() {
            @Override
            public void run() {
                LocalSocket socket;
                while (true) {
                    Log.d(TAG, "Waiting for connection...");
                    try {
                        socket = mSrvSock.accept();
                    } catch (IOException e) {
                        Log.w(TAG, "accept: " + e);
                        continue;
                    }
                    Log.d(TAG, "Got socket: " + socket);

                    try {
                        OutputStream os = socket.getOutputStream();
                        while (true) {
                            String action = mPendingIntents.take().getAction();
                            if (action.indexOf(INTENT_PREFIX) >= 0)
                                action = action.substring(INTENT_PREFIX.length());
                            action += '\n';
                            os.write(action.getBytes());
                            Log.i(TAG, "write action " + action);
                        }
                    } catch (InterruptedException e) {
                        Log.w(TAG, "bridger: " + e);
                    } catch (IOException e) {
                        Log.w(TAG, "bridger: " + e);
                    }
                }
            }
        };
        bridger.start();
        return START_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onDestroy() {
        unregisterReceiver(mReceiver);

	try {
            mSrvSock.close();
	} catch(IOException e) {
            Log.w(TAG, "onDestroy close socket: " + e);
	}
    }
}
