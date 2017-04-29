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

public class Item2gSub2 extends NetworkInfoActivity{
    
    public static final String TAG = "Item2gSub2";
    ArrayAdapter<String> aa;
    ArrayList<String> info = new ArrayList<String>();
    
    public void onCreate(Bundle icicle) {

        super.onCreate(icicle);
        setContentView(R.layout.item2g_sub2);
    
        ListView item2gSub2ListView = (ListView)this.findViewById(R.id.item2gSub2ListView);
        int layoutID = android.R.layout.simple_list_item_1;
        aa = new ArrayAdapter<String>(this, layoutID , info);
        item2gSub2ListView.setAdapter(aa);
    }

    public void updateDisplayInfo(){
        info.clear();
        String tmp=" AR "+" RXL "+" C1 "+" C 31 "+" C32 ";
        info.add(tmp);

        if(NetworkInfoService.item_2g_sub2.length < 5)
            return;

        Log.d(TAG, "item_2g_sub2.length is " + NetworkInfoService.item_2g_sub2.length);
        for(int i=0;i<NetworkInfoService.item_2g_sub2.length;i++)
        {
            tmp=NetworkInfoService.item_2g_sub2[i]+"|"+NetworkInfoService.item_2g_sub2[i+1]
             +"|"+NetworkInfoService.item_2g_sub2[i+2]+"|"+NetworkInfoService.item_2g_sub2[i+3]
             +"|"+NetworkInfoService.item_2g_sub2[i+4]; 
            info.add(tmp);
            i+=4;
        }
        aa.notifyDataSetChanged();
    }

}
