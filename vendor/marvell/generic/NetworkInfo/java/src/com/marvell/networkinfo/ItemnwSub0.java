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
import android.telephony.TelephonyManager;

public class ItemnwSub0 extends NetworkInfoActivity{
    
        public static final String TAG = "ItemnwSub0";
        private TelephonyManager mTelephonyManager;
        ArrayAdapter<String> aa;
        ArrayList<String> info = new ArrayList<String>();
        
        public void onCreate(Bundle icicle) {

            super.onCreate(icicle);
            setContentView(R.layout.itemnw_sub0);
        
                ListView itemnwSub0ListView = (ListView)this.findViewById(R.id.itemnwSub0ListView);
                int layoutID = android.R.layout.simple_list_item_1;
                mTelephonyManager = (TelephonyManager)getSystemService(TELEPHONY_SERVICE);
                aa = new ArrayAdapter<String>(this, layoutID , info);
                itemnwSub0ListView.setAdapter(aa);     
        }

        public void updateDisplayInfo(){
                SharedPreferences prefs = this.getSharedPreferences("NetworkInfo", Context.MODE_WORLD_READABLE | Context.MODE_WORLD_WRITEABLE);
                
                //int plmn = prefs.getInt(NetworkInfoService.ITEM_NW_SUB0_PLMN, 0);
                int mnc = prefs.getInt(NetworkInfoService.ITEM_NW_SUB0_MNC, 0);
                int mcc = prefs.getInt(NetworkInfoService.ITEM_NW_SUB0_MCC, 0);
                int lac = prefs.getInt(NetworkInfoService.ITEM_NW_SUB0_LAC, 0);
                int rac = prefs.getInt(NetworkInfoService.ITEM_NW_SUB0_RAC, 0);
                //int hplmn = prefs.getInt(NetworkInfoService.ITEM_NW_SUB0_HPLMN, 0);
                String plmn = Integer.toHexString((((mcc<<8)&0x000FFF00)|(mnc&0xFF)));
                String hplmn = mTelephonyManager.getSimOperator();
                info.clear();
                String tmp = "PLMN: " + plmn;
                info.add(tmp);

                tmp = "LAC:  " + (lac&0xFFFF);
                info.add(tmp);

                tmp = "RAC: " + rac;
                info.add(tmp);
      
                tmp ="HPLMN: " +hplmn;
                info.add(tmp);
                Log.d(TAG, "PLMN: " + plmn + "LAC:  " + lac  + "RAC: " + rac + "HPLMN: " +hplmn);
                
                aa.notifyDataSetChanged();
        }
}
