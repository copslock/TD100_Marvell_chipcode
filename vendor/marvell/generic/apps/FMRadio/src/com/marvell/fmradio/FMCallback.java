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

package com.marvell.fmradio;

import com.marvell.fmmanager.FMRadioListener;
import java.lang.Object;
import com.marvell.fmradio.util.ChannelHolder;
import com.marvell.fmradio.util.Station;
import android.os.Handler;
import android.os.Message;
public class FMCallback implements FMRadioListener {

	private static FMCallback mInstance = null;
	private static int mFreq = 0;
	private static String mName = "";
	private static int mRssi = 0;
	private static int mValue = 0;
private ChannelHolder mChannelHolder = ChannelHolder.getInstance();
        private final static int MSG_SCAN_FINISHED = 2;
        private final static int MSG_TUNE = 1;
        private final static int MSG_FOUNDCHANNEL = 13;
        private final static int MSG_STATE_CHANGED = 16;
	private final static int MSG_INTERNAL_ERROR = 24;
	private final static int MSG_MAX_FILESIZE_REACHED = 25;
	private final static int MSG_UNKOWN_ERROR = 26;

	private boolean flag = false;
	private Handler mHandler = null;
	public static FMCallback getInstance()
	{
		if (null == mInstance)
			mInstance = new FMCallback();
		return mInstance;
	}
	
	public void onFoundChannel(int freq)
	{
		flag = true;
		mFreq = freq;
		boolean test = true;
		for (int pos = mChannelHolder.getCount() - 1 ; 0 <= pos ; pos--) 
	{

	if(mChannelHolder.getItem(pos).mFreq.equals(String.valueOf(mFreq)))
	{	
		test = false;   
	break;
	}
	}
        if(test){
            mChannelHolder.add(new Station(String.valueOf(mFreq),""));
            mHandler.sendMessage(Message.obtain(mHandler, MSG_FOUNDCHANNEL, freq, 0));
        }
		//return mFreq;
	}
	
	public void onGetRdsPs(byte[] name)
	{
		String str = new String(name);
		//mValue = value;
		mName = str;
		if(flag)
		mChannelHolder.edit(ChannelHolder.BinarySearch(mChannelHolder.getStationList(),     String.valueOf(mFreq)),new Station(String.valueOf(mFreq),mName));
		else{
		mChannelHolder.edit(ChannelHolder.BinarySearch(mChannelHolder.getStationList(), String.valueOf(mChannelHolder.mWorkFreq)),new Station(String.valueOf(mChannelHolder.mWorkFreq),mName));
		mHandler.sendMessage(Message.obtain(mHandler, MSG_TUNE));
		}
		//return mName;
	}
	
	public void onGetRssi(int rssi)
	{
		mRssi = rssi;
		mChannelHolder.rssi = mRssi;
		//return mRssi;
	}

	public void onMonoStereoChanged(int monoStereo)
	{
	}

	public void onScanFinished()
	{
		mHandler.sendMessage(Message.obtain(mHandler, MSG_SCAN_FINISHED));
		flag = false;
		return ;
	}

	public void onStateChanged(int mode)
	{
		mHandler.sendMessage(Message.obtain(mHandler, MSG_STATE_CHANGED, mode));
	}

        public void setHandler(Handler handler)
	{
	mHandler = handler;	           
	}
	
	public void onRecordingStop(int error){
		switch(error){
		case 1:
			mHandler.sendMessage(Message.obtain(mHandler, MSG_INTERNAL_ERROR));
			break;
		case 2:
			 mHandler.sendMessage(Message.obtain(mHandler, MSG_MAX_FILESIZE_REACHED));
			break;
		case 3:
			 mHandler.sendMessage(Message.obtain(mHandler, MSG_UNKOWN_ERROR));
			break;
		default:
			break;
	}
	}
}
