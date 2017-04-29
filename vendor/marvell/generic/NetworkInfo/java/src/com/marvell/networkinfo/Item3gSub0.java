/*
 * (C) Copyright 2011 Marvell International Ltd.
 * All Rights Reserved
 *
 * MARVELL CONFIDENTIAL
 * Copyright 2011 ~ 2014 Marvell International Ltd All Rights Reserved.
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
package com.marvell.networkinfo;

import java.util.ArrayList;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.util.Log;
import android.widget.ArrayAdapter;
import android.widget.ListView;

public class Item3gSub0 extends NetworkInfoActivity{
    
    public static final String TAG = "Item3gSub0";
    ArrayAdapter<String> aa;
    ArrayList<String> info = new ArrayList<String>();
    
    public void onCreate(Bundle icicle) {

        super.onCreate(icicle);
        setContentView(R.layout.item3g_sub0);
    
        ListView item3gSub0ListView = (ListView)this.findViewById(R.id.item3gSub0ListView);
        int layoutID = android.R.layout.simple_list_item_1;
        aa = new ArrayAdapter<String>(this, layoutID , info);
        item3gSub0ListView.setAdapter(aa);
    }

    public void updateDisplayInfo(){

        SharedPreferences prefs = this.getSharedPreferences("NetworkInfo", Context.MODE_WORLD_READABLE | Context.MODE_WORLD_WRITEABLE);

        int rrc_state = prefs.getInt(NetworkInfoService.ITEM_3G_SUB0_RRC_state, 0);
        int s_freq = prefs.getInt(NetworkInfoService.ITEM_3G_SUB0_S_FREQ, 0);
        int s_psc = prefs.getInt(NetworkInfoService.ITEM_3G_SUB0_S_PSC, 0);
        int s_ecio = prefs.getInt(NetworkInfoService.ITEM_3G_SUB0_S_ECIO, 0);
        int s_rscp = prefs.getInt(NetworkInfoService.ITEM_3G_SUB0_S_RSCP, 0);
        info.clear();
        String tmp = "RRC state: " + rrc_state;
        info.add(tmp);

        tmp = "S_FREQ: " + s_freq;
        info.add(tmp);

        tmp = "S_PSC: " + s_psc;
        info.add(tmp);
       
        tmp = "S_ECIO: " + s_ecio;
        info.add(tmp);
        
        tmp = "S_RSCP: " + s_rscp;
        info.add(tmp);
        aa.notifyDataSetChanged();
        Log.d(TAG, "RRC state: " + rrc_state + "S_FREQ: " + s_freq + "S_PSC: " + s_psc + "S_ECIO: " + s_ecio + "S_RSCP: " + s_rscp);
    }

}
