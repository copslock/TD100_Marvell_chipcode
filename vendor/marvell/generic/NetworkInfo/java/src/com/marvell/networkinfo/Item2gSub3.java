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

public class Item2gSub3 extends NetworkInfoActivity{
    
    public static final String TAG = "Item2gSub3";
    ArrayAdapter<String> aa;
    ArrayList<String> info = new ArrayList<String>();
    
    public void onCreate(Bundle icicle) {

        super.onCreate(icicle);
        setContentView(R.layout.item2g_sub3);
    
        ListView item2gSub3ListView = (ListView)this.findViewById(R.id.item2gSub3ListView);
        int layoutID = android.R.layout.simple_list_item_1;
        aa = new ArrayAdapter<String>(this, layoutID , info);
        item2gSub3ListView.setAdapter(aa);
    }

    public void updateDisplayInfo(){

        SharedPreferences prefs = this.getSharedPreferences("NetworkInfo", Context.MODE_WORLD_READABLE | Context.MODE_WORLD_WRITEABLE);
        
        int rxlevel_full = prefs.getInt(NetworkInfoService.ITEM_2G_SUB3_RXLevel_Full, 0);
        int rxlevel_sub = prefs.getInt(NetworkInfoService.ITEM_2G_SUB3_RXLevel_Sub, 0);
        int rxqual_full= prefs.getInt(NetworkInfoService.ITEM_2G_SUB3_RXQual_Full, 0);
        int rxqual_sub = prefs.getInt(NetworkInfoService.ITEM_2G_SUB3_RXQual_Sub, 0);
        int tx_power_level = prefs.getInt(NetworkInfoService.ITEM_2G_SUB3_Tx_Power_Level, 0);
        info.clear();
        String tmp = "RXLevel Full: " + rxlevel_full;
        info.add(tmp);

        tmp = "RXLevel Sub: " + rxlevel_sub;
        info.add(tmp);

        tmp = "RXQual Full: " + rxqual_full;
        info.add(tmp);
        
        tmp = "RxQual Sub: " + rxqual_sub;
        info.add(tmp);
        
        tmp = "Tx Power Level (Index/dBm): " + tx_power_level;
        info.add(tmp);
        aa.notifyDataSetChanged();
        Log.d(TAG, "RXLevel Full: " + rxlevel_full + "RXLevel Sub: " + rxlevel_sub + "RXQual Full: " + rxqual_full + "RxQual Sub: " + rxqual_sub + "Tx Power Level (Index/dBm): " + tx_power_level);
    }

}
