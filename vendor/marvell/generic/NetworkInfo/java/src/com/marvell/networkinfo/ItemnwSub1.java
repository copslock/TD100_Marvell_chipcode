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

public class ItemnwSub1 extends NetworkInfoActivity{
    
    public static final String TAG = "ItemnwSub1";
    ArrayAdapter<String> aa;
    ArrayList<String> info = new ArrayList<String>();
    
    public void onCreate(Bundle icicle) {

        super.onCreate(icicle);
        setContentView(R.layout.itemnw_sub1);
    
        ListView itemnwSub1ListView = (ListView)this.findViewById(R.id.itemnwSub1ListView);
        int layoutID = android.R.layout.simple_list_item_1;
        aa = new ArrayAdapter<String>(this, layoutID , info);
        itemnwSub1ListView.setAdapter(aa);
//      info.add(this.getResources().getString(R.string.itemnw_sub1));      
//      info.add(this.getResources().getString(R.string.itemnw_sub1_sum));  

    }

    public void updateDisplayInfo(){        
        SharedPreferences prefs = this.getSharedPreferences("NetworkInfo", Context.MODE_WORLD_READABLE | Context.MODE_WORLD_WRITEABLE);
        
        int mm_state = prefs.getInt(NetworkInfoService.ITEM_NW_SUB1_MM_State, 0);
        //int mm_reject = prefs.getInt(NetworkInfoService.ITEM_NW_SUB1_MM_Reject, 0);
        int gmm_state = prefs.getInt(NetworkInfoService.ITEM_NW_SUB1_GMM_State, 0);
        int attach_reject = prefs.getInt(NetworkInfoService.ITEM_NW_SUB1_Attach_Reject, 0);
        int rau_reject = prefs.getInt(NetworkInfoService.ITEM_NW_SUB1_RAU_Reject, 0);
        info.clear();
        String tmp = "MM State: " + mm_state;
        info.add(tmp);

        tmp = "GMM State: " + gmm_state;
        info.add(tmp);
        
        if(attach_reject == 0x7000) {
            tmp = "Attach Reject: null";
        } else {
            tmp = "Attach Reject: " + attach_reject;
        }
        info.add(tmp);
        
        if(rau_reject == 0x7000) {
            tmp = "RAU Reject: null";
        } else {
            tmp = "RAU Reject : " + rau_reject;
        }
        info.add(tmp);

        tmp = "RejHis: " + "mmCau" +" | " + " gmmCau";
        info.add(tmp);

        Log.d(TAG, "item_reject_history.length is " + NetworkInfoService.item_reject_history.length);
        if(NetworkInfoService.item_reject_history.length < 2 ) {
                //donothing
        } else {
            for(int i=0; i< NetworkInfoService.item_reject_history.length; i++) {
                tmp = "            " + NetworkInfoService.item_reject_history[i] + " | " + NetworkInfoService.item_reject_history[i+1];
                info.add(tmp);
                i+=1;
            }
        }
        Log.d(TAG, "MM State: " + mm_state + "GMM State: " + gmm_state + "Attach Reject: " + attach_reject + "RAU Reject : " + rau_reject);
        aa.notifyDataSetChanged();
    }
}
