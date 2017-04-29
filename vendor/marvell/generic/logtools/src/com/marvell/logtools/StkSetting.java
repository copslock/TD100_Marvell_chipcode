/*****************************
 **  (C) Copyright 2011 Marvell International Ltd.
 **  All Rights Reserved
 **  Author Linda
 **  Create Date Aug 02 2011      
 */
package com.marvell.logtools;

import com.marvell.logtools.R;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Toast;
import com.android.internal.telephony.gsm.stk.AppInterface;

public class StkSetting extends Activity{
 
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.stk_setting);
        // Watch for button clicks.
        Button buttonOn = (Button)findViewById(R.id.buttonScreenBusyOn);
        buttonOn.setOnClickListener(mScreenBusyOn);
        Button buttonOff = (Button)findViewById(R.id.buttonScreenBusyOff);
        buttonOff.setOnClickListener(mScreenBusyOff);
        
    }

    private OnClickListener mScreenBusyOn = new OnClickListener() {
        public void onClick(View v) {
            Intent intent = new Intent(AppInterface.STK_SCREEN_BUSY);
            intent.putExtra("STK ScreenBusy",true);
            sendBroadcast(intent);
            showStkScreenToastMessage(true); 
        }
    };
    
    private OnClickListener mScreenBusyOff = new OnClickListener() {
        public void onClick(View v) {
            Intent intent = new Intent(AppInterface.STK_SCREEN_BUSY);
            intent.putExtra("STK ScreenBusy",false);
            sendBroadcast(intent);
            showStkScreenToastMessage(false);  
        }
    };

    private void showStkScreenToastMessage(boolean ScreenState) {
        if(ScreenState) {
           Toast.makeText(this, "Send STK ScreenBusy ON to StkAppService", Toast.LENGTH_LONG).show();
        } else {
           Toast.makeText(this, "Send STK ScreenBusy OFF to StkAppService", Toast.LENGTH_LONG).show();
        }
    }
}
