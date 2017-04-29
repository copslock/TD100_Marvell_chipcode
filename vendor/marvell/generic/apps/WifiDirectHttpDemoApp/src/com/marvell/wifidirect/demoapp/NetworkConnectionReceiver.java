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

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.wifi.MrvlWifiManager;
import android.net.wifi.MrvlWifiDirectInfo;

import android.util.Log;

public class NetworkConnectionReceiver extends BroadcastReceiver
{
	private static final String LOG_TAG = "NetworkConnectionReceiver";

	@Override
	public void onReceive(Context context, Intent intent) {
		String action = intent.getAction();
		Log.d(LOG_TAG, "onReceive NetworkConnectionReceiver: " + intent.getAction());
		if ("android.net.wifi.MRVL_WIFI_DIRECT_CONNECTED".equals(action)) {
		        Log.d(LOG_TAG, intent.getAction()+ "Wifi Direct Connection Success! " );
			startServer(context);
		} else if ("android.net.wifi.MRVL_WIFI_DIRECT_DISCONNECTED".equals(action)) {
			Log.d(LOG_TAG, intent.getAction()+ "Wifi Direct Disconnection! " );
			stopServer(context);
		}
	}

	private void startServer(Context context){

		Intent intent = new Intent("com.marvell.wifidirect.demoapp.WFD_SERVER_START");
		context.startService(intent);
        }

	private void stopServer(Context context){

		Intent intent = new Intent("com.marvell.wifidirect.demoapp.WFD_SERVER_STOP");
		boolean bServiceStopped = context.stopService(intent);
		Log.d(LOG_TAG, "stopServer has been invoked!" + "bServiceStopped is " + bServiceStopped);
	}
}
