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

public class Item2gSub4 extends NetworkInfoActivity{
    
    public static final String TAG = "Item2gSub0";
    ArrayAdapter<String> aa;
    ArrayList<String> info = new ArrayList<String>();
    
    public void onCreate(Bundle icicle) {

        super.onCreate(icicle);
        setContentView(R.layout.item2g_sub4);
    
        ListView item2gSub4ListView = (ListView)this.findViewById(R.id.item2gSub4ListView);
        int layoutID = android.R.layout.simple_list_item_1;
        aa = new ArrayAdapter<String>(this, layoutID , info);
        item2gSub4ListView.setAdapter(aa);
    }

    public void updateDisplayInfo(){
        
        SharedPreferences prefs = this.getSharedPreferences("NetworkInfo", Context.MODE_WORLD_READABLE | Context.MODE_WORLD_WRITEABLE);
        
        int amr_ch = prefs.getInt(NetworkInfoService.ITEM_2G_SUB4_AMR_Ch, 0);
        int channel_mode = prefs.getInt(NetworkInfoService.ITEM_2G_SUB4_Channel_Mode, 0);
        info.clear();
        String tmp = "AMR Ch: " + amr_ch;
        info.add(tmp);

        tmp = "Channel Mode: " + channel_mode;
        info.add(tmp);
        aa.notifyDataSetChanged();
        Log.d(TAG, "AMR Ch: " + amr_ch + "Channel Mode: " + channel_mode);
    }

}
