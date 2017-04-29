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

import android.os.Bundle;
import android.util.Log;
import android.widget.ArrayAdapter;
import android.widget.ListView;

public class Item3gSub1 extends NetworkInfoActivity{
    
    public static final String TAG = "Item3gSub1";
    ArrayAdapter<String> aa;
    ArrayList<String> info = new ArrayList<String>();
    
    public void onCreate(Bundle icicle) {

        super.onCreate(icicle);
        setContentView(R.layout.item3g_sub1);
    
        ListView item3gSub1ListView = (ListView)this.findViewById(R.id.item3gSub1ListView);
        int layoutID = android.R.layout.simple_list_item_1;
        aa = new ArrayAdapter<String>(this, layoutID , info);
        item3gSub1ListView.setAdapter(aa);
    }

    public void updateDisplayInfo(){
        info.clear();
        String tmp = " UARFCN "+" PSC "+" RSCP "+" Ec/Lo ";
        info.add(tmp);
 
        //SharedPreferences prefs = this.getSharedPreferences("NetworkInfo", Context.MODE_WORLD_READABLE | Context.MODE_WORLD_WRITEABLE);
        if(NetworkInfoService.item_3g_sub1.length < 4)
            return;
        Log.d(TAG, "item_3g_sub1.length is " + NetworkInfoService.item_3g_sub1.length);
        for(int i=0;i<NetworkInfoService.item_3g_sub1.length;i++)
        {
                tmp=NetworkInfoService.item_3g_sub1[i]+"|"+NetworkInfoService.item_3g_sub1[i+1]+"|"
                +NetworkInfoService.item_3g_sub1[i+2]+"|"+NetworkInfoService.item_3g_sub1[i+3];
            
            info.add(tmp);
            i+=3;
        }
        aa.notifyDataSetChanged();
    }

}
