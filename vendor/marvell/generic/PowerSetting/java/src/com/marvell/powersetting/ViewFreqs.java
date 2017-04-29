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

import java.util.ArrayList;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.ArrayAdapter;
import android.widget.ListView;

public class ViewFreqs extends Activity{
    
	public static final String TAG = "ViewFreqs";
	ArrayList<String> freqs = new ArrayList<String>();
	ArrayAdapter<String> aa;
	
	public void onCreate(Bundle icicle) {

	    super.onCreate(icicle);
	    setContentView(R.layout.viewfreqs);
	
	    ListView viewFreqListView = (ListView)this.findViewById(R.id.viewFreqListView);
	    int layoutID = android.R.layout.simple_list_item_1;
        aa = new ArrayAdapter<String>(this, layoutID , freqs);

        viewFreqListView.setAdapter(aa);
        refreshFreqs();

    }

	private void refreshFreqs() {
		//get cpu freq
		int cpuFreq = PowerFreq.getCurrentCpuFreq();
		String strFreq = "Cpu Freq: " + String.valueOf(cpuFreq/1000);
		
		freqs.add(strFreq);
		
		//get other freq
		String info = PowerFreq.getCurrentOtherFreqs(cpuFreq);
		if(info == null){
			Log.i(TAG, "Other freq info not provided");
			return;
		}		
		int start = 0;
		int delimit = -1;
		while(true){
			delimit = info.indexOf(' ', start);
			if(delimit == -1){
				strFreq = info.substring(start);
				freqs.add(strFreq);
				break;
			}
			strFreq = info.substring(start, delimit);
			freqs.add(strFreq);
			start = delimit + 1;
		}
		
	}

}
