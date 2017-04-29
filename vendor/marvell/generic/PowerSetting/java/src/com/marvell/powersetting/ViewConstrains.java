/*
 * * (C) Copyright 2010 Marvell International Ltd.
 * * All Rights Reserved
 * *
 * * MARVELL CONFIDENTIAL
 * * Copyright 2008 ~ 2010 Marvell International Ltd All Rights Reserved.
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

package com.marvell.powersetting;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.os.PowerManager;
import android.util.Log;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

public class ViewConstrains extends Activity{

	public static final String TAG = "ViewConstrains";
	String text;
	String idleWakelocks[];
	String wakelocks[];
	public void onCreate(Bundle icicle) {

		super.onCreate(icicle);
		setContentView(R.layout.viewconstrains);

		TextView tv = (TextView)this.findViewById(R.id.constrainsTextView);


		//get all constrains
		text = "Current Status:\n\n";

		int cpufreqStatus = PowerFreq.getCpufreqStatus();
		if(cpufreqStatus != 0){
			text = text + "Cpufreq disabled\n\n";
		}else{
			text = text + "Cpufreq enabled\n\n";
		}

	
		int minFreq = PowerFreq.getMinFreq();
		text = text + "Drivers have set min freq requirment: " + minFreq + "\n\n";

		//get all wakelocks from kernel's perspective
		text = text + "Active wakelocks from kernel level:\n";
		wakelocks = PowerFreq.getActiveWakelocks();
		if(wakelocks == null){
			Log.i(TAG, "wakelocks info not provided");
		}else{
			for(int i = 0; i<wakelocks.length; i++){
				text = text + wakelocks[i] + "\n";
			}
		}

		//get from java level, need to modify PowerManager
		text = text + "\n" + "Active wakelocks from java level:\n";
		PowerManager pm = (PowerManager)getSystemService(Context.POWER_SERVICE);
		int num = pm.getWakelockNum();

		for(int i = 0; i < num; i++){
			String name  = pm.getWakelockName(i);
			int type = pm.getWakelockType(i);
			String strType = lockType(type);

			boolean active = pm.getWakelockState(i);

			text = text + name + "(" + strType + ")\n";
		}

		int idleStatus = PowerFreq.getIdleStatus();
		if(idleStatus != 0){
			text = text + "\n\n\n\n" + "Idle is disabled\n\n";
		}else{

			text = text + "\n\n\n\n" + "Idle is enabled\n\n";
		}

		idleWakelocks = PowerFreq.getIdleWakelocks();
		if(idleWakelocks == null){
			Log.i(TAG, "idleWakelocks info not provided");
		}else{
			for(int i = 0; i<idleWakelocks.length; i++){
				if(idleWakelocks[i] != null){
					text = text + idleWakelocks[i] + "\n";
				}
			}
		}


		tv.setText(text);

	}
	
    private static String lockType(int type)
    {
        switch (type)
        {
            case PowerManager.FULL_WAKE_LOCK:
                return "FULL_WAKE_LOCK";
            case PowerManager.SCREEN_BRIGHT_WAKE_LOCK:
                return "SCREEN_BRIGHT_WAKE_LOCK";
            case PowerManager.SCREEN_DIM_WAKE_LOCK:
                return "SCREEN_DIM_WAKE_LOCK";
            case PowerManager.PARTIAL_WAKE_LOCK:
                return "PARTIAL_WAKE_LOCK";
            case PowerManager.PROXIMITY_SCREEN_OFF_WAKE_LOCK:
                return "PROXIMITY_SCREEN_OFF_WAKE_LOCK";
            default:
                return "???";
        }
    }

}
