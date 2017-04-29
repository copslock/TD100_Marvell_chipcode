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
package com.marvell.networkinfo;

import java.util.ArrayList;

import android.os.Bundle;
import android.util.Log;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.content.SharedPreferences;
import android.content.Context;

public class Item2gSub0 extends NetworkInfoActivity{
    
	public static final String TAG = "Item2gSub0";
	ArrayAdapter<String> aa;
	ArrayList<String> info = new ArrayList<String>();
	public void onCreate(Bundle icicle) {

	    super.onCreate(icicle);
	    setContentView(R.layout.item2g_sub0);
	
		ListView item2gSub0ListView = (ListView)this.findViewById(R.id.item2gSub0ListView);
		int layoutID = android.R.layout.simple_list_item_1;
		aa = new ArrayAdapter<String>(this, layoutID , info);
		item2gSub0ListView.setAdapter(aa);
	}
	
	public void updateDisplayInfo(){

		SharedPreferences prefs = this.getSharedPreferences("NetworkInfo", Context.MODE_WORLD_READABLE | Context.MODE_WORLD_WRITEABLE);
		
		info.clear();
		String tmp = "ARFCN: " + prefs.getInt(NetworkInfoService.ITEM_2G_SUB0_ARFCN, 0);
		info.add(tmp);

		tmp = "RL_Timeout_v: " + prefs.getInt(NetworkInfoService.ITEM_2G_SUB0_RL_Timeout_v, 0);
		info.add(tmp);

		tmp = "RXLEV: " + prefs.getInt(NetworkInfoService.ITEM_2G_SUB0_RXLEV, 0);
		info.add(tmp);
		
		tmp = "C1: " + prefs.getInt(NetworkInfoService.ITEM_2G_SUB0_C1, 0);
		info.add(tmp);
		
		tmp = "C2: " + prefs.getInt(NetworkInfoService.ITEM_2G_SUB0_C2, 0);
		info.add(tmp);
		
		Log.d(TAG, "ARFCN is " + prefs.getInt(NetworkInfoService.ITEM_2G_SUB0_ARFCN, 0)
				+ "RL_Timeout_v is " + prefs.getInt(NetworkInfoService.ITEM_2G_SUB0_RL_Timeout_v, 0)
				+ "RXLEV is " + prefs.getInt(NetworkInfoService.ITEM_2G_SUB0_RXLEV, 0)
				+"C1 is"+prefs.getInt(NetworkInfoService.ITEM_2G_SUB0_C1, 0)
				+"C2 is"+prefs.getInt(NetworkInfoService.ITEM_2G_SUB0_C2, 0));
	}

}
