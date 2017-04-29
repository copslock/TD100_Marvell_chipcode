/*
 * * (C) Copyright 2011 Marvell International Ltd.
 * * All Rights Reserved
 * *
 * * MARVELL CONFIDENTIAL
 * * Copyright 2011 ~ 2014 Marvell International Ltd All Rights Reserved.
 * * The source code contained or described herein and all documents related to
 * * the source code ("Material") are owned by Marvell International Ltd or its
 * * suppliers or licensors. Title to the Material remains with Marvell International Ltd
 * * or its suppliers and licensors. The Material contains trade secrets and
 * * proprietary and confidential information of Marvell or its suppliers and
 * * licensors. The Material is protected by worldwide copyright and trade secret
 * * laws and treaty provisions. No part of the Material may be used, copied,
 * * reproduced, modified, published, uploaded, posted, transmitted, distributed,
 * * or disclosed in any way without Marvell's prior express written permission.
 * *
 * * No license under any patent, copyright, trade secret or other intellectual
 * * property right is granted to or conferred upon you by disclosure or delivery
 * * of the Materials, either expressly, by implication, inducement, estoppel or
 * * otherwise. Any license under such intellectual property rights must be
 * * express and approved by Marvell in writing.
 * *
 * */
package com.marvell.networkinfo;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.content.BroadcastReceiver;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.ComponentName;
import android.content.Intent;
import android.content.Context;
import android.os.IBinder;
import android.widget.Toast;

public abstract class NetworkInfoActivity extends Activity{
    
    public static final String TAG = "NetworkInfoActivity";
    public NetworkInfoService mBoundService = null;
    public static final String INFORM_UPDATE_BROADCAST="com.marvell.action.INFORM_UPDATE_BROADCAST";
    public abstract void updateDisplayInfo(); 

        private void showMessage(int result) {
            if(result == 3) {
                Toast.makeText(this, "Initialization fail, Please Restart Application", Toast.LENGTH_LONG).show();
            }
            else if(result == 2) {
                Toast.makeText(this, "Initialization, Please waiting...", Toast.LENGTH_LONG).show();
            } else if (result == 1) {
                Toast.makeText(this, "Query failed, waiting next query", Toast.LENGTH_LONG).show();
            } else {
                Toast.makeText(this, "Query unknown error, ignore it", Toast.LENGTH_LONG).show();
            }
        }

        private ServiceConnection mConnection = new ServiceConnection() {
            public void onServiceConnected(ComponentName className, IBinder service) {
                // This is called when the connection with the service has been
                // established, giving us the service object we can use to
                // interact with the service.  Because we have bound to a explicit
                // service that we know is running in our own process, we can
                // cast its IBinder to a concrete class and directly access it.
                Log.i(TAG, "Service connected");
                mBoundService = ((NetworkInfoService.NetworkInfoBinder)service).getService();
                if(mBoundService != null) {
                    int result;
                    result = mBoundService.query();
                    Log.d(TAG, "query result is " + result);
                    if(result == 0) {
                        updateDisplayInfo();
                    } else {
                        showMessage(result);
                    }
                }
            }

            public void onServiceDisconnected(ComponentName className) {
                // This is called when the connection with the service has been
                // unexpectedly disconnected -- that is, its process crashed.
                // Because it is running in our same process, we should never
                // see this happen.
                Log.i(TAG, "Service disconnected");
                mBoundService = null;
            }
    };
    public void onCreate(Bundle icicle) {
        Log.i(TAG, "onCreate ");
        super.onCreate(icicle);
        //start and bind to service
        bindService(new Intent(NetworkInfoActivity.this, NetworkInfoService.class), mConnection, Context.BIND_AUTO_CREATE);
        Log.i(TAG, "bind done");
        registerReceiver();
    }
    public void onDestroy(){
        Log.i(TAG, "onDestory");
        super.onDestroy();
        unbindService(mConnection);
        unregisterReceiver(InformUpdateReceiver);
    }
    //register informUpdate intent receiver
    public void registerReceiver(){
        registerReceiver(InformUpdateReceiver, new IntentFilter(INFORM_UPDATE_BROADCAST));
    }   
    
    public BroadcastReceiver InformUpdateReceiver = new BroadcastReceiver() {    
           @Override
        public void onReceive(Context context, Intent intent) {
        if (INFORM_UPDATE_BROADCAST.equals(intent.getAction())) {
          if(mBoundService != null) {
                int result;
                result = mBoundService.query();
                Log.d(TAG, "query result is " + result);
                if(result == 0) {
                    updateDisplayInfo();
                } else {
                    showMessage(result);
                }
          }
          updateDisplayInfo();
       }
   }
};

    static final private int MENU_UPDATE = Menu.FIRST;
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        super.onCreateOptionsMenu(menu);
        menu.add(0, MENU_UPDATE, Menu.NONE, R.string.menu_update);
        return true;
    }
  
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        super.onOptionsItemSelected(item);
        switch (item.getItemId()) {
            case (MENU_UPDATE): {
                mBoundService.query();
                updateDisplayInfo();
                return true;
            }
        }
        return false;
    }
}
