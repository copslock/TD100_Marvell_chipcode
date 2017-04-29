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

public class ItemnwSub2 extends NetworkInfoActivity{
    
    public static final String TAG = "ItemnwSub2";
    ArrayAdapter<String> aa;
    ArrayList<String> info = new ArrayList<String>();
    
    public void onCreate(Bundle icicle) {

        super.onCreate(icicle);
        setContentView(R.layout.itemnw_sub2);
    
        ListView itemnwSub2ListView = (ListView)this.findViewById(R.id.itemnwSub2ListView);
        int layoutID = android.R.layout.simple_list_item_1;
        aa = new ArrayAdapter<String>(this, layoutID , info);
        itemnwSub2ListView.setAdapter(aa);
//      info.add(this.getResources().getString(R.string.itemnw_sub2));      
//      info.add(this.getResources().getString(R.string.itemnw_sub2_sum));
    }

    public void updateDisplayInfo(){
SharedPreferences prefs = this.getSharedPreferences("NetworkInfo", Context.MODE_WORLD_READABLE | Context.MODE_WORLD_WRITEABLE);
        
        int ciphering =prefs.getInt(NetworkInfoService.ITEM_NW_SUB2_CIPHERING, 0);
        int UEA1 = prefs.getInt(NetworkInfoService.ITEM_NW_SUB2_UEA1, 0);
        int GEA1 = prefs.getInt(NetworkInfoService.ITEM_NW_SUB2_GEA1, 0);
          int GEA2 =prefs.getInt(NetworkInfoService.ITEM_NW_SUB2_GEA2, 0);
        int A5_1 =prefs.getInt(NetworkInfoService.ITEM_NW_SUB2_A5_1, 0);
        int A5_2 =prefs.getInt(NetworkInfoService.ITEM_NW_SUB2_A5_2, 0);
        int A5_3 =prefs.getInt(NetworkInfoService.ITEM_NW_SUB2_A5_3, 0);
        info.clear();
        String tmp = "Ciphering: " + ciphering;
        info.add(tmp);

        tmp = "UEA1: " + UEA1;
        info.add(tmp);

                //now can't support follow params
        //tmp = "GEA1: " + GEA1;
                tmp = "GEA1: not support";
        info.add(tmp);
        
        //tmp = "GEA2: " + GEA2;
                tmp = "GEA2: not support";
        info.add(tmp);
        
        //tmp = "A5_1: " + A5_1;
                tmp = "A5_1: not support";
        info.add(tmp);
        
        //tmp = "A5_2: " + A5_2;
                tmp = "A5_2: not support";
        info.add(tmp);
        
        //tmp = "A5_3: " + A5_3;
                tmp = "A5_3: not support";
        info.add(tmp);
        Log.d(TAG, "Ciphering: " + ciphering + "UEA1: " + UEA1 +
                "GEA1: " + GEA1 + "GEA2: " + GEA2 + "A5_1: " + A5_1 + 
                "A5_2: " + A5_2 + "A5_3: " + A5_3);
        aa.notifyDataSetChanged();
    }       
}

