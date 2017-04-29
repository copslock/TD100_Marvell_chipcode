/*****************************
 **  (C) Copyright 2011 Marvell International Ltd.
 **  All Rights Reserved
 **  Author Aric
 **  Create Date Aug 29 2011      
 */
package com.marvell.logtools;

import android.provider.Telephony;
import static android.provider.Telephony.Intents.SECRET_CODE_ACTION;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class NVMEditorRecevier extends BroadcastReceiver {
     public static final String LOGTAG = "NVMEditorRecevier";

    @Override
    public void onReceive(Context context, Intent intent) {
        Log.i(LOGTAG, " onReceive");
        String host = intent.getData() != null ? intent.getData().getHost() : null;
        Log.d(LOGTAG, "host is " + host);
        if (intent.getAction().equals(SECRET_CODE_ACTION) )
        {
            if(host.equals("789012"))
            {   
                Intent i = new Intent();
            
                i.setClassName("com.marvell.logtools", "com.marvell.logtools.NVMEditorService");
                i.setFlags(7455);
                context.startService(i);
            }else if(host.equals("789013"))
            {   
                Intent i = new Intent();
            
                i.setClassName("com.marvell.logtools", "com.marvell.logtools.NVMEditorService");
                i.setFlags(7451);
                context.startService(i);
            }
        }

    }

}
