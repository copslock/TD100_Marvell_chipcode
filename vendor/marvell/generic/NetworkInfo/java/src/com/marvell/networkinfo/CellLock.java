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

import com.marvell.networkinfo.R;

//import android.content.ComponentName;
//import android.content.ServiceConnection;
import android.os.Bundle;
//import android.os.IBinder;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceCategory;
import android.preference.PreferenceScreen;
import android.util.Log;
//import android.content.Intent;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

public class CellLock extends PreferenceActivity implements Preference.OnPreferenceChangeListener {
        public static final String TAG = "CellLock";
//        public NetworkInfoService mService = null;
        private static final int MENU_SAVE_ARFCN = Menu.FIRST;
        private static final int MENU_SAVE_ARFCN_CELL = Menu.FIRST + 1;
        private static final int MENU_UNLOCK = Menu.FIRST + 2;
        private int mResultValue;
        
/*        private ServiceConnection mConnection = new ServiceConnection() {
            public void onServiceConnected(ComponentName className, IBinder service) {
                Log.i(TAG, "Service connected");
                mService = ((NetworkInfoService.NetworkInfoBinder)service).getService();
                mService.query();
            }
            public void onServiceDisconnected(ComponentName className) {
                Log.i(TAG, "Service disconnected");
                mService = null;
            }
        };*/
        
        @Override
        protected void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
//            bindService(new Intent(CellLock.this, NetworkInfoService.class), mConnection, 0);
            setPreferenceScreen(createPreferenceHierarchy());
        }
        
        @Override
        protected void onDestroy() {
                super.onDestroy();
//                unbindService(mConnection);
        }

        @Override
        public boolean onCreateOptionsMenu(Menu menu) {
            super.onCreateOptionsMenu(menu);
            menu.add(0, MENU_SAVE_ARFCN, 0, R.string.save_arfcn);
            menu.add(0, MENU_SAVE_ARFCN_CELL, 0, R.string.save_arfcn_cell);
            menu.add(0, MENU_UNLOCK, 0, R.string.unlock);
            return true;
        }

        @Override
        public boolean onOptionsItemSelected(MenuItem item) {
            super.onOptionsItemSelected(item);
            switch (item.getItemId()) {
                case MENU_SAVE_ARFCN:
                    int arfcn = (mResultValue&0xFFFF0000)>>16;
                    Log.d(TAG, "save arfcn = " + arfcn);
                    if(NetworkInfoService.lock_cell(1,arfcn,255)) {
                        Toast.makeText(this, "lock Arfcn success,please reset the phone take effect.",Toast.LENGTH_LONG).show();
                    } else {
                        Toast.makeText(this, "lock Arfcn false",Toast.LENGTH_LONG).show();
                    }
                    return true;
                case MENU_SAVE_ARFCN_CELL:
                    arfcn = (mResultValue&0xFFFF0000)>>16;
                    int cell = (mResultValue)&0xFFFF;
                    Log.d(TAG, "save arfcn = " + arfcn + " and cell = " + cell);
                    if(NetworkInfoService.lock_cell(2,arfcn,cell)) {
                        Toast.makeText(this, "lock Arfcn and Cell success,please reset the phone take effect.",Toast.LENGTH_LONG).show();
                    } else {
                        Toast.makeText(this, "lock Arfcn and Cell false",Toast.LENGTH_LONG).show();
                    }
                    return true;
                case MENU_UNLOCK:
                    if(NetworkInfoService.lock_cell(0,65535,255)) {
                        Toast.makeText(this, "Unlock success,please reset the phone take effect.",Toast.LENGTH_LONG).show();
                    } else {
                        Toast.makeText(this, "Unlock false",Toast.LENGTH_LONG).show();
                    }
                    return true;
            }
            return false;
        }

        public boolean onPreferenceChange(Preference preference, Object objValue) {
            Log.d(TAG, "onPreferenceChange key is" + preference.getKey());
            if("list_itra_preference".equals(preference.getKey())) {
                mResultValue = Integer.parseInt((String) objValue);
                Log.d(TAG, "choose itra, and the resultValue is" + mResultValue);
                return true; 
            } else if ("list_iter_preference".equals(preference.getKey())) {
                mResultValue = Integer.parseInt((String) objValue);
                Log.d(TAG, "choose iter, and the resultValue is" + mResultValue);
                return true;
            } else {
                Log.d(TAG, "can't get focus");
                return true;
            }
        }

        private PreferenceScreen createPreferenceHierarchy() {
            // Root
            PreferenceScreen root = getPreferenceManager().createPreferenceScreen(this);
        
            // Dialog based preferences
            PreferenceCategory dialogBasedPrefCat = new PreferenceCategory(this);
            dialogBasedPrefCat.setTitle(R.string.dialog_based_preferences);
            root.addPreference(dialogBasedPrefCat);
        
            // List preference
            ListPreference listItraPref = new ListPreference(this);
            if(NetworkInfoService.item_3g_sub1.length < 4 ) {
                Log.d(TAG, "itraCounts is 0");
                listItraPref.setEntries(R.array.entries_list_preference);
                listItraPref.setEntryValues(R.array.entryvalues_list_preference);
            } else {
                int[] validItraCounts = NetworkInfoService.item_3g_sub1;
                String[] entries = new String[validItraCounts.length/4];
                String[] entryValues = new String[validItraCounts.length/4];
                Log.d(TAG, "itraCounts is " + NetworkInfoService.item_3g_sub1.length);
                for(int i = 0; i < validItraCounts.length/4; i++) {
                    entryValues[i] = String.valueOf((((NetworkInfoService.item_3g_sub1[i*4]<<16)&0xFFFF0000))|(NetworkInfoService.item_3g_sub1[i*4+1]&0x0000FFFF));
                    Log.d(TAG, "itraEntryValue" + i + "is " + entryValues[i]); 
                    entries[i] = "arfcn " + String.valueOf(NetworkInfoService.item_3g_sub1[i*4]) + ", cellPID " + String.valueOf(NetworkInfoService.item_3g_sub1[i*4+1]);
                }
                listItraPref.setEntries(entries);
                listItraPref.setEntryValues(entryValues);
            }
            listItraPref.setDialogTitle(R.string.dialog_title_list_preference);
            listItraPref.setKey("list_itra_preference");
            listItraPref.setTitle(R.string.title_itra_list_preference);
            listItraPref.setSummary(R.string.summary_list_preference);
            listItraPref.setOnPreferenceChangeListener(this);
            dialogBasedPrefCat.addPreference(listItraPref);

           // List preference
            ListPreference listIterPref = new ListPreference(this);
            if(NetworkInfoService.item_3g_sub2.length < 4 ) {
                Log.d(TAG, "iterCounts is 0");
                listIterPref.setEntries(R.array.entries_list_preference);
                listIterPref.setEntryValues(R.array.entryvalues_list_preference);
            } else {
                int[] validIterCounts = NetworkInfoService.item_3g_sub2;
                String[] entries = new String[validIterCounts.length/4];
                String[] entryValues = new String[validIterCounts.length/4];
                Log.d(TAG, "iterCounts is " + NetworkInfoService.item_3g_sub2.length);
                for(int i = 0; i < validIterCounts.length/4; i++) {
                    entryValues[i] = String.valueOf((((NetworkInfoService.item_3g_sub2[i*4]<<16)&0xFFFF0000))|(NetworkInfoService.item_3g_sub2[i*4+1]&0x0000FFFF));
                    Log.d(TAG, "iterEntryValue" + i + "is " + entryValues[i]);
                    entries[i] = "arfcn " + String.valueOf(NetworkInfoService.item_3g_sub2[i*4]) + ", cellPID " + String.valueOf(NetworkInfoService.item_3g_sub2[i*4+1]);
                }
                listIterPref.setEntries(entries);
                listIterPref.setEntryValues(entryValues);
            }
            listIterPref.setDialogTitle(R.string.dialog_title_list_preference);
            listIterPref.setKey("list_iter_preference");
            listIterPref.setTitle(R.string.title_iter_list_preference);
            listIterPref.setSummary(R.string.summary_list_preference);
            listIterPref.setOnPreferenceChangeListener(this);
            dialogBasedPrefCat.addPreference(listIterPref);
        
            return root;
        }
}
