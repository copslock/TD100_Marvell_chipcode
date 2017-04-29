/*****************************
 **  (C) Copyright 2011 Marvell International Ltd.
 **  All Rights Reserved
 **  Author Linda
 **  Create Date Aug 02 2011      
 */
package com.marvell.logtools;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.os.SystemProperties;
import android.preference.PreferenceActivity;
import android.util.Log;

import java.io.File;

public class OtherSetting  extends PreferenceActivity  {
    static final boolean DBG = true;
    static final String TAG = "OtherSetting";
    
    public NVMEditorService mBoundService;
    
    private final String NVMFolder = "/data/Linux/Marvell/NVM";
    private final String NVMFileName = "EEHandlerConfig_Linux.nvm";
    
    private ServiceConnection mConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder service) {
            if(DBG) Log.d(TAG, "NVMEditor Service connected");
            mBoundService = ((NVMEditorService.NVMEditorBinder)service).getService();
        }
        
        public void onServiceDisconnected(ComponentName className) {
            if(DBG) Log.d(TAG, "NVMEditor Service disconnected");
            mBoundService = null;
        }
    };
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState); 
        File nvmFile = new File(NVMFolder, NVMFileName);
        addPreferencesFromResource(R.xml.other_setting);
        if(nvmFile.exists()){
            findPreference("CpFinalActionSetting").setEnabled(true);
        }
        bindService(new Intent(this, NVMEditorService.class), mConnection, Context.BIND_AUTO_CREATE);
        
    }
    
    @Override
    public void onResume() {
        super.onResume();
        if(mBoundService != null){
            String lastCpFinalAction = mBoundService.getFinalAct();
            SystemProperties.set("gsm.cp.final.act",lastCpFinalAction);
            if(DBG) Log.d(TAG, "onResume get lastCpFinalAction is" + lastCpFinalAction);
        }
    }

    @Override
    public void onPause() {
        super.onPause();
        if(mBoundService != null) {
            //avoid onResume called before service connected
            String lastCpFinalAction = mBoundService.getFinalAct();
            SystemProperties.set("gsm.cp.final.act", lastCpFinalAction);
            if(DBG) Log.d(TAG, "onPause get lastCpFinalAction is" + lastCpFinalAction);
        }
    }
    
    @Override
    public void onDestroy() {
        super.onDestroy();
        unbindService(mConnection);
    }
    
    
}
