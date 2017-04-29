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

import android.provider.Telephony;
import static android.provider.Telephony.Intents.SECRET_CODE_ACTION;

import android.content.Context;
import android.content.Intent;
import android.content.BroadcastReceiver;
import android.util.Log;

public class NetworkInfoReceiver extends BroadcastReceiver {
        public static final String TAG = "NetworkInfoReceiver";
    public NetworkInfoReceiver() {
    }
    
    @Override
    public void onReceive(Context context, Intent intent) {
        Log.d(TAG, "onReceive called");
        String host = intent.getData() != null ? intent.getData().getHost() : null;
        Log.d(TAG, "host is " + host);
        if (intent.getAction().equals(SECRET_CODE_ACTION) && host.equals("6201"))
        {
                Intent i = new Intent();
                i.setClassName("com.marvell.networkinfo", "com.marvell.networkinfo.MainItem");
                i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);

                context.startActivity(i);
        }
    }
}
