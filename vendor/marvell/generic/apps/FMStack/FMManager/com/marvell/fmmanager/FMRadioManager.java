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

package com.marvell.fmmanager;

import android.content.Context;
import android.os.Binder;
import android.os.Bundle;
import android.os.RemoteException;
import android.os.IBinder;
import android.os.ServiceManager;
import android.util.Log;
import android.os.Handler;
import android.os.Message;
import java.io.File;
import android.os.ParcelFileDescriptor;


public class FMRadioManager {
    private IFMRadioService mFMRadioService;
    private static final String TAG = "FMRadioManager";  
    private FMRadioListener mFMRadioListener = null;
     
    public FMRadioManager(){
        mFMRadioService = IFMRadioService.Stub.asInterface(
                ServiceManager.getService("FMRadioService"));
        if (mFMRadioService == null){
            Log.d(FMRadioManager.TAG, "mFMRadioService == NULL");
            return;
        }else{
            Log.d(FMRadioManager.TAG, "Got FMRadioervice instance.");
        }
    }

    public boolean isFMTx(){
        boolean res = true;
        try {
          res = mFMRadioService.isFMTx();
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "isFMTx() failed with: " + e);
          e.printStackTrace();
        }
        return res;
    }
    public boolean isFMRx(){
        boolean res = true;
        try {
          res = mFMRadioService.isFMRx();
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "isFMRx() failed with: " + e);
          e.printStackTrace();
        }
        return res;
    }

    public int powerOn(){
        int res = 0;
        try {
          res = mFMRadioService.powerOn();
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "powerOn() failed with: " + e);
          e.printStackTrace();
        }
        return res;
    }
    public int powerOff(){
        int res = 0;
        try {
          res = mFMRadioService.powerOff();
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "powerOff() failed with: " + e);
          e.printStackTrace();
        }
        return res;
    }

    public int enableTx(){
        int res = 0;

        try {
          res = mFMRadioService.enableTx();
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "enableTx() failed with: " + e);
          e.printStackTrace();
        }
        // enable failed
        if (res < 0){
            return res;
        }

        // enable success, register the callbacks.
        try {
            Log.d(FMRadioManager.TAG, "mFMRadioService.registerCallback(mCallback)");
            mFMRadioService.registerCallback(mCallback);
        } catch (RemoteException e) {
            Log.d(FMRadioManager.TAG, "Got exception when registerCallback.");
        }

        return res;
    }
    public int disableTx(){
        int res = 0;

        try {
            Log.d(FMRadioManager.TAG, "mFMRadioService.unregisterCallback(mCallback)");
            mFMRadioService.unregisterCallback(mCallback);
        } catch (RemoteException e) {
            Log.d(FMRadioManager.TAG, "Got exception when unregisterCallback.");
        }

        try {
          res = mFMRadioService.disableTx();
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "disableTx() failed with: " + e);
          e.printStackTrace();
        }

        return res;
    }

    public int enableRx(){
        int res = 0;

        try {
          res = mFMRadioService.enableRx();
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "enableRx() failed with: " + e);
          e.printStackTrace();
        }
        // enable failed
        if (res < 0){
            return res;
        }

        // enable success, register the callbacks.
        try {
            Log.d(FMRadioManager.TAG, "mFMRadioService.registerCallback(mCallback)");
            mFMRadioService.registerCallback(mCallback);
        } catch (RemoteException e) {
            Log.d(FMRadioManager.TAG, "Got exception when registerCallback.");
        }

        return res;
    }
    public int disableRx(){
        int res = 0;

        try {
            Log.d(FMRadioManager.TAG, "mFMRadioService.unregisterCallback(mCallback)");
            mFMRadioService.unregisterCallback(mCallback);
        } catch (RemoteException e) {
            Log.d(FMRadioManager.TAG, "Got exception when unregisterCallback.");
        }

        try {
          res = mFMRadioService.disableRx();
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "disableRx() failed with: " + e);
          e.printStackTrace();
        }

        return res;
    }

    public int suspend(){
        int res = 0;
        try {
          res = mFMRadioService.suspend();
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "suspend() failed with: " + e);
          e.printStackTrace();
        }
        return res;
    }
    public int resume(){
        int res = 0;
        try {
          res = mFMRadioService.resume();
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "resume() failed with: " + e);
          e.printStackTrace();
        }
        return res;
    }
    public int scanAll(){
        int res = 0;
        try {
          res = mFMRadioService.scan_all();
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "scan_all() failed with: " + e);
          e.printStackTrace();
        }
        return res;
    }
    public int stopScan(){
        int res = 0;
        try {
          res = mFMRadioService.stop_scan();
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "stop_scan() failed with: " + e);
          e.printStackTrace();
        }
        return res;
    }
    public int setChannel(int freq){
        int res = 0;
        try {
          res = mFMRadioService.set_channel(freq);
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "set_channel() failed with: " + e);
          e.printStackTrace();
        }
        return res;
    }
    public int getChannel(){
        int res = 0;
        try {
          res = mFMRadioService.get_channel();
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "get_channel() failed with: " + e);
          e.printStackTrace();
        }
        return res;
    }
    public int getRssi(){
        int res = 0;
        try {
          res = mFMRadioService.get_rssi();
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "get_rssi() failed with: " + e);
          e.printStackTrace();
        }
        return res;
    }
    public float getVolume(){
        float res = 0;
        try {
          res = mFMRadioService.get_volume();
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "get_volume() failed with: " + e);
          e.printStackTrace();
        }
        return res;
    }
    public int setVolume(int volume){
        int res = 0;
        try {
          res = mFMRadioService.set_volume(volume);
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "set_volume() failed with: " + e);
          e.printStackTrace();
        }
        return res;
    }
    public boolean setMute(boolean flag){
        boolean res = true;
        try {
          res = mFMRadioService.set_mute(flag);
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "set_mute() failed with: " + e);
          e.printStackTrace();
        }
        return res;
    }
    public int setBand(int band){
        int res = 0;
        try {
          res = mFMRadioService.set_band(band);
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "set_band() failed with: " + e);
          e.printStackTrace();
        }
        return res;
    }
    public int scanNext(){
        int res = 0;
        try {
          res = mFMRadioService.scan_next();
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "enable() failed with: " + e);
          e.printStackTrace();
        }
        return res;
    }
    public int scanPrev(){
        int res = 0;
        try {
          res = mFMRadioService.scan_prev();
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "enable() failed with: " + e);
          e.printStackTrace();
        }
        return res;
    }
    public int setSpeakerOn(boolean on){
        int res = 0;
        try {
          res = mFMRadioService.setSpeakerOn(on);
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "enable() failed with: " + e);
          e.printStackTrace();
        }
        return res;
    }     
    public boolean setTxMute(boolean flag){
        boolean res = true;
        try {
          res = mFMRadioService.set_tx_mute(flag);
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "set_tx_mute() failed with: " + e);
          e.printStackTrace();
        }
        return res;
    }
    public boolean setPowerMode(boolean flag){
        boolean res = true;
        try {
          res = mFMRadioService.set_power_mode(flag);
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "set_power_mode() failed with: " + e);
          e.printStackTrace();
        }
        return res;
    }
    public boolean setTxMonoStereo(boolean flag){
        boolean res = true;
        try {
          res = mFMRadioService.set_tx_mono_stereo(flag);
        } catch (RemoteException e) {
          Log.d(FMRadioManager.TAG, "set_tx_mono_stereo() failed with: " + e);
          e.printStackTrace();
        }
        return res;
    }

    public int startRecording(String path){
        ParcelFileDescriptor pfd = null;
        int ret = 0;

        try {
            File file = new File(path);
            pfd = ParcelFileDescriptor.open(file,
                ParcelFileDescriptor.MODE_READ_WRITE | ParcelFileDescriptor.MODE_CREATE | ParcelFileDescriptor.MODE_TRUNCATE);
            Log.d(FMRadioManager.TAG, "startRecording(), pfd = " + pfd + ", FilePath = " + path);
        } catch (Exception e) {
            Log.e(FMRadioManager.TAG, "startRecording() failed when opening " + path +" with exception: " + e);
            return -2;
        }

        try {
            boolean ret_val = mFMRadioService.start_recording(pfd);
            if (!ret_val){
                Log.e(FMRadioManager.TAG, "startRecording() failed when starting record");
                ret = -1;
            }
        } catch (Exception e) {
            Log.e(FMRadioManager.TAG, "startRecording() failed when starting record with exception:" + e);
            ret = -1;
        }

        try {
            pfd.close();
        } catch (Exception e){
            Log.e(FMRadioManager.TAG, "startRecording() failed when closing fd." + pfd);
        }

        return ret;
    }
    public int stopRecording(){
        try {
          boolean ret = mFMRadioService.stop_recording();
          if (!ret){
              Log.e(FMRadioManager.TAG, "stopRecording() failed.");
              return -1;
          }
        } catch (RemoteException e) {
            Log.e(FMRadioManager.TAG, "startRecording() failed with exception:" + e);
            return -1;
        }

        return 0;
    }
    public byte[] getRecordFormat(){
        byte[] format = null;
        try {
            format = mFMRadioService.get_record_format();
        } catch (RemoteException e) {
            Log.e(FMRadioManager.TAG, "getRecordFormat() failed with exception:" + e);
        }

        return format;
    }

    public boolean registerListener(FMRadioListener listener){
        mFMRadioListener = listener;
        return true;
    }   

    private Handler mHandler = new Handler() {
        public void handleMessage(Message msg){
            if (mFMRadioListener == null){
                 return;
            }
            switch (msg.what) {
                case M_SCAN_FINISHED:
                    mFMRadioListener.onScanFinished();
                    break;
                case M_FOUND_CHANNEL:
                    mFMRadioListener.onFoundChannel(msg.arg1);
                    break;
                case M_MONO_CHANGED:
                    mFMRadioListener.onMonoStereoChanged(msg.arg1);
                    break;
                case M_GET_RDSPS_NAME:
                    mFMRadioListener.onGetRdsPs((byte [])msg.obj);
                    break;
                case M_GET_CUR_RSSI:
                    mFMRadioListener.onGetRssi(msg.arg1);
                    break;
                case M_STATE_CHANGED:
                    mFMRadioListener.onStateChanged(msg.arg1);
                    break;
                case M_RECORDING_STOP:
                    mFMRadioListener.onRecordingStop(msg.arg1);
                    break;
                default:
                    super.handleMessage(msg);
            }
        }  
    };  
    private static final int M_SCAN_FINISHED = 1;
    private static final int M_FOUND_CHANNEL = 2;
    private static final int M_MONO_CHANGED = 3;
    private static final int M_GET_CUR_RSSI = 4;
    private static final int M_GET_RDSPS_NAME = 5;
    private static final int M_STATE_CHANGED = 6;
    private static final int M_RECORDING_STOP = 7;

    private IFMRadioCallback mCallback = new IFMRadioCallback.Stub() {
        /**
         * This is called by the remote service regularly to tell us about
         * new values.  Note that IPC calls are dispatched through a thread
         * pool running in each process, so the code executing here will
         * NOT be running in our main thread like most other things -- so,
         * to update the UI, we need to use a Handler to hop over there.
         */
        public void scan_finished(){
            mHandler.sendMessage(mHandler.obtainMessage(M_SCAN_FINISHED));
            return;
        }
        public void found_channel(int channel){
            mHandler.sendMessage(mHandler.obtainMessage(M_FOUND_CHANNEL, channel, 0));
            return;
        }
        public void mono_changed(int mono){
            mHandler.sendMessage(mHandler.obtainMessage(M_MONO_CHANGED, mono, 0));
            return;
        }
        public void get_cur_rssi(int rssi){
            mHandler.sendMessage(mHandler.obtainMessage(M_GET_CUR_RSSI, rssi, 0));
            return;
        }
        public void get_rdsps_name(byte[] name){
            mHandler.sendMessage(mHandler.obtainMessage(M_GET_RDSPS_NAME, name));
            return;
        }
        public void state_changed(int mode){
            mHandler.sendMessage(mHandler.obtainMessage(M_STATE_CHANGED, mode, 0));
            return;
        }
        public void recording_stop(int reason){
            mHandler.sendMessage(mHandler.obtainMessage(M_RECORDING_STOP, reason, 0));
            return;
        }
    };

     
}




