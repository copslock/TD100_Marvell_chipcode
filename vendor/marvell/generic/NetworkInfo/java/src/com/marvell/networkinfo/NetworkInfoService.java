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

import java.lang.String;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.Context;
import android.content.Intent;
import android.app.Service;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;

public class NetworkInfoService extends Service {

        public static final String TAG = "NetworkInfoService";
        private static final String INFORM_UPDATE_BROADCAST="com.marvell.action.INFORM_UPDATE_BROADCAST";
        public static native void start_ee_mode();
        public native int query_ee_data();
        public static native void stop_ee_mode();
        public native int[] get_neighboring_cell(int type);
        public static native boolean lock_cell(int mode, int arfcn, int cellId);
        public native int[] get_reject_history();
        
        public static boolean lock_result;
        //item 2g sub0 data
        public int item_2g_sub0_arfcn;
        public int item_2g_sub0_rl_timeout_v;
        public int item_2g_sub0_rxlev;
        public int item_2g_sub0_c1;
        public int item_2g_sub0_c2;
        public int item_gsm_edge_support;
        public int item_gsm_ci;
        public int item_gsm_bsic;
        public int item_2g_t3212;
       
        public static int item_reject_history[]; //10*2 
        //item 2g sub1 data
        public static int item_2g_sub1[]; //4*8
        
        //item 2g sub2 data
        public static int item_2g_sub2[]; //5*8
        
        //item 2g sub3 data
        public int item_2g_sub3_rxlevel_full;
        public int item_2g_sub3_rxlevel_sub;
        public int item_2g_sub3_rxqual_full;
        public int item_2g_sub3_rxqual_sub;
        public int item_2g_sub3_tx_power_level;
        //item 2g sub4 data
        public int item_2g_sub4_amr_ch;
        public int item_2g_sub4_channel_mode;
        //item 3g sub0 data
        public int item_3g_sub0_rcc_state;
        public int item_3g_sub0_s_freq;
        public int item_3g_sub0_s_psc;
        public int item_3g_sub0_s_ecio;
        public int item_3g_sub0_s_rscp;
        //item 3g sub1 data
        public static int item_3g_sub1[]; //4*32
        
        //item 3g sub2 data
        public static int item_3g_sub2[];
        
        //item nw sub0 data
        //public int item_nw_sub0_plmn;
        public int item_nw_sub0_mnc;
        public int item_nw_sub0_mcc;
        public int item_nw_sub0_lac;
        public int item_nw_sub0_rac;
        public int item_nw_sub0_hplmn;
        public int item_td_pid;
        public int item_3g_t3212;
        public int item_3g_tmsi;

        //item nw sub1 data
        public int item_nw_sub1_mm_state;
        public int item_nw_sub1_mm_reject;
        public int item_nw_sub1_gmm_state;
        public int item_nw_sub1_attach_reject;
        public int item_nw_sub1_rau_reject;
        //item nw sub2 data
        public int item_nw_sub2_ciphering;
        public int item_nw_sub2_UEA1;
        public int item_nw_sub2_GEA1;
        public int item_nw_sub2_GEA2;
        public int item_nw_sub2_A5_1;
        public int item_nw_sub2_A5_2;
        public int item_nw_sub2_A5_3;
        
        //flag
        public static final String ITEM_2G_SUB0_ARFCN = "ITEM_2G_SUB0_ARFCN";
        public static final String ITEM_2G_SUB0_RL_Timeout_v = "ITEM_2G_SUB0_RL_Timeout_v";
        public static final String ITEM_2G_SUB0_RXLEV = "TEM_2G_SUB0_RXLEV";
        public static final String ITEM_2G_SUB0_C1 = "TEM_2G_SUB0_C1";
        public static final String ITEM_2G_SUB0_C2 = "TEM_2G_SUB0_C2";
        public static final String ITEM_GSM_EDGE_SUPPORT = "ITEM_GSM_EDGE_SUPPORT";
        public static final String ITEM_GSM_CI = "ITEM_GSM_CI";
        public static final String ITEM_GSM_BSIC = "ITEM_GSM_BSIC";
        public static final String ITEM_2G_T3212 = "ITEM_2G_T3212";
        //
        public static final String ITEM_2G_SUB3_RXLevel_Full = "TEM_2G_SUB2_RXLevel_Full";
        public static final String ITEM_2G_SUB3_RXLevel_Sub = "ITEM_2G_SUB2_RXLevel_Sub";
        public static final String ITEM_2G_SUB3_RXQual_Full = "ITEM_2G_SUB2_RXQual_Full";
        public static final String ITEM_2G_SUB3_RXQual_Sub = "ITEM_2G_SUB2_RXQual_Sub";
        public static final String ITEM_2G_SUB3_Tx_Power_Level = "ITEM_2G_SUB3_Tx_Power_Level";
        public static final String ITEM_2G_SUB4_AMR_Ch = "ITEM_2G_SUB4_AMR_Ch";
        public static final String ITEM_2G_SUB4_Channel_Mode = "ITEM_2G_SUB2_Channel_Mode";
        public static final String ITEM_3G_SUB0_RRC_state = "ITEM_3G_SUB0_RRC_state";
        public static final String ITEM_3G_SUB0_S_FREQ = "ITEM_3G_SUB0_S_FREQ";
        public static final String ITEM_3G_SUB0_S_PSC = "ITEM_3G_SUB0_S_PSC";
        public static final String ITEM_3G_SUB0_S_ECIO = "ITEM_3G_SUB0_S_ECIO";
        public static final String ITEM_3G_SUB0_S_RSCP = "ITEM_3G_SUB0_S_RSCP";
        //
        public static final String ITEM_NW_SUB0_PLMN = "ITEM_NW_SUB0_PLMN";
        public static final String ITEM_NW_SUB0_LAC = "ITEM_NW_SUB0_LAC";
        public static final String ITEM_NW_SUB0_RAC = "ITEM_NW_SUB0_RAC";
        public static final String ITEM_NW_SUB0_HPLMN = "ITEM_NW_SUB0_HPLMN";
        public static final String ITEM_NW_SUB0_MNC = "ITEM_NW_SUB0_MNC";
        public static final String ITEM_NW_SUB0_MCC = "ITEM_NW_SUB0_MCC";
        public static final String ITEM_TD_PID = "ITEM_TD_PID";
        public static final String ITEM_3G_T3212 = "ITEM_3G_T3212";
        public static final String ITEM_3G_TMSI = "ITEM_3G_TMSI";
        //nw sub1
        public static final String ITEM_NW_SUB1_MM_State = "ITEM_NW_SUB1_MM_Stat";
        public static final String ITEM_NW_SUB1_MM_Reject = "ITEM_NW_SUB1_MM_Reject";
        public static final String ITEM_NW_SUB1_GMM_State = "ITEM_NW_SUB1_GMM_State";
        public static final String ITEM_NW_SUB1_Attach_Reject = "ITEM_NW_SUB1_Attach_Reject";
        public static final String ITEM_NW_SUB1_RAU_Reject = "ITEM_NW_SUB1_RAU_Reject";
        //nw sub2
        public static final String ITEM_NW_SUB2_CIPHERING = "ITEM_NW_SUB2_CIPHERING";
        public static final String ITEM_NW_SUB2_UEA1 = "ITEM_NW_SUB2_UEA1";
        public static final String ITEM_NW_SUB2_GEA1 = "ITEM_NW_SUB2_GEA1";
        public static final String ITEM_NW_SUB2_GEA2 = "ITEM_NW_SUB2_GEA2";
        public static final String ITEM_NW_SUB2_A5_1 = "ITEM_NW_SUB2_A5_1";
        public static final String ITEM_NW_SUB2_A5_2 = "ITEM_NW_SUB2_A5_2";
        public static final String ITEM_NW_SUB2_A5_3 = "ITEM_NW_SUB2_A5_3";
        
        static { 
            System.loadLibrary("NetworkInfo");
        }

        public void onCreate(){
            Log.i(TAG, "onCreate called");
            super.onCreate();
        }

        public void onStart(Intent intent, int startId) {
            Log.d(TAG, "onStart called");
            super.onStart(intent, startId);
        }

        public void onDestroy() {
            Log.d(TAG, "onDestroy called");
            super.onDestroy();
        }

        @Override
        public IBinder onBind(Intent intent) {
            Log.i(TAG, "onBind ~~~~~~~~~~~~");
            return mBinder;
        }

        @Override
        public boolean onUnbind(Intent intent) {
            Log.d(TAG, "onUnbind ~~~~~~~~~~~");
            return super.onUnbind(intent);
        }       

        /** 
        ** Class for clients to access.  Because we know this service always
        ** runs in the same process as its clients, we don't need to deal with
        ** IPC.
        **/
        public class NetworkInfoBinder extends Binder {
            NetworkInfoService getService() {
                return NetworkInfoService.this;
            }
        }

        private final IBinder mBinder = new NetworkInfoBinder();

        private void update_ee_data_callback(){
            Log.d(TAG, "update_ee_data_callback called");

            //TODO: send intent broadcast to inform GUI update
            Intent informUpdate = new Intent(INFORM_UPDATE_BROADCAST);
            sendBroadcast(informUpdate);
        }

        public int query(){

            int result = 0;
            item_2g_sub1 = get_neighboring_cell(0);
            //Log.d(TAG, "item_2g_sub1 length: " + item_2g_sub1.length );
            item_2g_sub2 = get_neighboring_cell(1);
            //Log.d(TAG, "item_2g_sub2 length: " + item_2g_sub2.length );
            item_3g_sub1 = get_neighboring_cell(2);
            //Log.d(TAG, "item_3g_sub1 length: " + item_3g_sub1.length );
            item_3g_sub2 = get_neighboring_cell(3);
            //Log.d(TAG, "item_3g_sub2 length: " + item_3g_sub2.length );
            item_reject_history = get_reject_history();
            //Log.d(TAG, "item_reject_history length: " + item_reject_history.length);
            
            result = query_ee_data();

            if(result == 0) {
                //write to shared preference for easy access
                SharedPreferences prefs = getSharedPreferences("NetworkInfo", Context.MODE_WORLD_READABLE | Context.MODE_WORLD_WRITEABLE);
                Editor editor = prefs.edit();
                editor.putInt(ITEM_2G_SUB0_ARFCN, item_2g_sub0_arfcn);
                editor.putInt(ITEM_2G_SUB0_C1, item_2g_sub0_c1);
                editor.putInt(ITEM_2G_SUB0_C2, item_2g_sub0_c2);
                editor.putInt(ITEM_2G_SUB0_RL_Timeout_v, item_2g_sub0_rl_timeout_v);
                editor.putInt(ITEM_2G_SUB0_RXLEV, item_2g_sub0_rxlev);
                editor.putInt(ITEM_GSM_EDGE_SUPPORT, item_gsm_edge_support);
                editor.putInt(ITEM_GSM_CI, item_gsm_ci);
                editor.putInt(ITEM_GSM_BSIC, item_gsm_bsic);
                editor.putInt(ITEM_2G_T3212, item_2g_t3212);
                //
                editor.putInt(ITEM_2G_SUB3_RXLevel_Full, item_2g_sub3_rxlevel_full);
                editor.putInt(ITEM_2G_SUB3_RXLevel_Sub, item_2g_sub3_rxlevel_sub);
                editor.putInt(ITEM_2G_SUB3_RXQual_Full, item_2g_sub3_rxqual_full);
                editor.putInt(ITEM_2G_SUB3_RXQual_Sub, item_2g_sub3_rxqual_sub);
                editor.putInt(ITEM_2G_SUB3_Tx_Power_Level, item_2g_sub3_tx_power_level);
                //
                editor.putInt(ITEM_2G_SUB4_AMR_Ch, item_2g_sub4_amr_ch);
                editor.putInt(ITEM_2G_SUB4_Channel_Mode, item_2g_sub4_channel_mode);
                //
                editor.putInt(ITEM_3G_SUB0_RRC_state, item_3g_sub0_rcc_state);
                editor.putInt(ITEM_3G_SUB0_S_ECIO, item_3g_sub0_s_ecio);
                editor.putInt(ITEM_3G_SUB0_S_FREQ, item_3g_sub0_s_freq);
                editor.putInt(ITEM_3G_SUB0_S_PSC, item_3g_sub0_s_psc);
                editor.putInt(ITEM_3G_SUB0_S_RSCP, item_3g_sub0_s_rscp);
                //nw sub0
                editor.putInt(ITEM_NW_SUB0_HPLMN, item_nw_sub0_hplmn);
                editor.putInt(ITEM_NW_SUB0_LAC, item_nw_sub0_lac);
                //editor.putInt(ITEM_NW_SUB0_PLMN, item_nw_sub0_plmn);
                editor.putInt(ITEM_NW_SUB0_MNC, item_nw_sub0_mnc);
                editor.putInt(ITEM_NW_SUB0_MCC, item_nw_sub0_mcc);
                editor.putInt(ITEM_NW_SUB0_RAC, item_nw_sub0_rac);
                editor.putInt(ITEM_TD_PID, item_td_pid);
                editor.putInt(ITEM_3G_T3212, item_3g_t3212);
                editor.putInt(ITEM_3G_TMSI, item_3g_tmsi);
                //
                editor.putInt(ITEM_NW_SUB1_Attach_Reject, item_nw_sub1_attach_reject);
                editor.putInt(ITEM_NW_SUB1_GMM_State, item_nw_sub1_gmm_state);
                editor.putInt(ITEM_NW_SUB1_MM_Reject, item_nw_sub1_mm_reject);
                editor.putInt(ITEM_NW_SUB1_MM_State, item_nw_sub1_mm_state);
                editor.putInt(ITEM_NW_SUB1_RAU_Reject, item_nw_sub1_rau_reject);
                //
                editor.putInt(ITEM_NW_SUB2_A5_1, item_nw_sub2_A5_1);
                editor.putInt(ITEM_NW_SUB2_A5_2, item_nw_sub2_A5_2);
                editor.putInt(ITEM_NW_SUB2_A5_3, item_nw_sub2_A5_3);
                editor.putInt(ITEM_NW_SUB2_CIPHERING, item_nw_sub2_ciphering);
                editor.putInt(ITEM_NW_SUB2_GEA1, item_nw_sub2_GEA1);
                editor.putInt(ITEM_NW_SUB2_GEA2, item_nw_sub2_GEA2);
                editor.putInt(ITEM_NW_SUB2_UEA1, item_nw_sub2_UEA1);
                editor.commit();
            } else {
                //use last time values
            }   
            return result;
        }   
}
