/*
 * (C) Copyright 2010 Marvell International Ltd.
 * All Rights Reserved
 *
 * MARVELL CONFIDENTIAL
 * Copyright 2008 ~ 2010 Marvell International Ltd All Rights Reserved.
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

package com.marvell.callsetting;

import android.os.Bundle;
import android.preference.CheckBoxPreference;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.PreferenceActivity;
import android.util.Log;
import android.content.Context;
import android.content.pm.PackageManager.NameNotFoundException;

public class CallReplaceImageActivity extends PreferenceActivity {

    private static final String VT_PREFERENCE = "com.android.phone.vt";
    private static final String ENABLE_SEND_TO_REMOTE_KEY = "remote-image-enabled";
    private Context mPhoneContext;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        addPreferencesFromResource(R.xml.call_replace_image);
        //Share with VideoCallScreen......
        //FIXME bettet put the VT relatedd setting to com.android.phone
        CheckBoxPreference cb = (CheckBoxPreference) findPreference(ENABLE_SEND_TO_REMOTE_KEY);
        try{
            mPhoneContext = createPackageContext("com.android.phone",Context.CONTEXT_IGNORE_SECURITY);
        }catch(NameNotFoundException e){
            Log.e("CallReplaceImageActivity","Error in create phone context");
            finish();
        }
        cb.setChecked(mPhoneContext.getSharedPreferences(VT_PREFERENCE,0)
                .getBoolean(ENABLE_SEND_TO_REMOTE_KEY, false));
        cb.setOnPreferenceChangeListener(new OnPreferenceChangeListener(){
            @Override
            public boolean onPreferenceChange(Preference preference, Object newValue){
                CheckBoxPreference cb = (CheckBoxPreference) preference;
                mPhoneContext.getSharedPreferences(VT_PREFERENCE,MODE_WORLD_READABLE).edit()
                    .putBoolean(ENABLE_SEND_TO_REMOTE_KEY, !cb.isChecked()).commit();
                return true;
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
    }
}
