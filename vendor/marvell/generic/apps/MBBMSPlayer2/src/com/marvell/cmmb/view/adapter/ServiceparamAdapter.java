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

import com.marvell.cmmb.R;
import com.marvell.cmmb.resolver.ChannelItem;
import com.marvell.cmmb.resolver.MBBMSResolver;
import com.marvell.cmmb.resolver.ProgramItem;
import com.marvell.cmmb.resolver.ServiceParamItem;
import com.marvell.cmmb.service.MBBMSService;
import com.marvell.cmmb.view.adapter.ChannelListAdatpter.ViewHolder;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

public class ServiceparamAdapter extends BaseAdapter {

	static class ViewHolder {
		TextView channelTextView;

		ImageView cryptImgeView;

		ImageView playState;
	}

	private LayoutInflater mInflater;

	private ArrayList<ServiceParamItem> mServiceParamList = new ArrayList<ServiceParamItem>();

	private static final String TAG = "ServiceparamAdapter";

	public ServiceparamAdapter(Context context,
			ArrayList<ServiceParamItem> serviceparamList) {
		mInflater = LayoutInflater.from(context);
		mServiceParamList = serviceparamList;
	}

	public int getCount() {
		return mServiceParamList.size();
	}

	public Object getItem(int position) {
		return position;
	}

	public long getItemId(int position) {
		return position;
	}

	public View getView(int position, View convertView, ViewGroup parent) {
		ViewHolder holder;
		if (convertView == null) {
			holder = new ViewHolder();
			convertView = mInflater.inflate(R.layout.channel_list_item, null);
			holder.channelTextView = (TextView) convertView.findViewById(R.id.tv_channel);
			holder.cryptImgeView = (ImageView) convertView.findViewById(R.id.iv_crypt);
			holder.playState = (ImageView) convertView.findViewById(R.id.iv_playing);
			convertView.setTag(holder);
		} else {
			holder = (ViewHolder) convertView.getTag();
		}
		holder.channelTextView.setText(String.valueOf(mServiceParamList.get(
				position).getServiceId()));
		holder.cryptImgeView.setVisibility(View.INVISIBLE);
		if (position == MBBMSService.sCurrentPlayPosition) {
			holder.playState.setVisibility(View.VISIBLE);
		} else {
			holder.playState.setVisibility(View.GONE);
		}
		return convertView;
	}

}
