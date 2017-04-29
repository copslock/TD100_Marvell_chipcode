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

package com.marvell.cmmb.view.adapter;

import java.util.ArrayList;
import java.util.HashMap;

import android.content.Context;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Gallery;
import android.widget.TextView;

import com.marvell.cmmb.R;
import com.marvell.cmmb.common.Util;
import com.marvell.cmmb.resolver.ChannelItem;
import com.marvell.cmmb.resolver.MBBMSResolver;
import com.marvell.cmmb.service.MBBMSService;

public class ProgramRefAdapter extends BaseAdapter {
	private Context mContext;

	private MBBMSResolver mResolver;

	public HashMap<Integer, Boolean> mChannelMap;// check which channel is selected

	public ArrayList<ChannelItem> mChannelList = new ArrayList<ChannelItem>();// Store all channels' name

	public ProgramRefAdapter(Context context) {
		mContext = context;
		mChannelMap = new HashMap<Integer, Boolean>();
		mResolver = MBBMSResolver.getInstance(mContext);
		mChannelList = Util.getChannelList(MBBMSService.sCurrentMode, context);
	}

	public ArrayList<ChannelItem> getChannelList() {
		return mChannelList;
	}

	public int getChannelStatus(int position) {
//		String subscriptionId = mResolver.getPurchaseId(mChannelList.get(
//				position).getChannelId());
//		int status = -1;
//		if (!subscriptionId.equals(String.valueOf(0))) {
//			status = mResolver.getPurchaseStatus(subscriptionId);
//		}
//		return status;
		return mChannelList.get(position).getEncryptState();
	}
	
	public int getForFreeStatus(int position) {
		return mChannelList.get(position).getForFreeState();
	}

	public int getCount() {
		return mChannelList.size();
	}

	public Object getItem(int position) {
		return position;
	}

	public long getItemId(int position) {
		return position;
	}

	public View getView(int position, View convertView, ViewGroup parent) {
		TextView channelTextView = new TextView(mContext);
		channelTextView.setText(mChannelList.get(position).getChannelName());
		channelTextView.setLayoutParams(new Gallery.LayoutParams(120, 35));
		channelTextView.setGravity(Gravity.CENTER);
		if (mChannelMap.get(position)) {
			channelTextView.setTextColor(mContext.getResources().getColor(R.color.solid_white));
			channelTextView.setBackgroundResource(R.drawable.channel_selected);
		} else {
			channelTextView.setTextColor(mContext.getResources().getColor(R.color.solid_gray));
			channelTextView.setBackgroundResource(R.drawable.channel_default);
		}
		return channelTextView;
	}

	public int isFree(int position) {
		int forfree = mChannelList.get(position).getFreeState();
		return forfree;
	}
}
