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

package com.marvell.cmmb.interfacetest;

import java.util.ArrayList;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.marvell.cmmb.R;
import com.marvell.cmmb.resolver.PurchaseInfoItem;

public class MBBMSSubscribeTestAdapter extends BaseAdapter {
	static class ViewHolder {
		TextView mPurchaseTextView;
	}

	private LayoutInflater mInflater;

	private ArrayList<PurchaseInfoItem> mSubscriptionList;

	public MBBMSSubscribeTestAdapter(Context context,
			ArrayList<PurchaseInfoItem> list) {
		mInflater = LayoutInflater.from(context);
		mSubscriptionList = list;
	}

	public int getCount() {
		return mSubscriptionList.size();
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
			convertView = mInflater.inflate(R.layout.interfacetest_item, null);
			holder = new ViewHolder();
			holder.mPurchaseTextView = (TextView) convertView
					.findViewById(R.id.tv_interfacetest);
			convertView.setTag(holder);
		} else {
			holder = (ViewHolder) convertView.getTag();
		}
		holder.mPurchaseTextView.setText(mSubscriptionList.get(position)
				.getPurchaseName());
		if (position % 2 != 0) {
			convertView.setBackgroundResource(R.drawable.list_black);
		} else {
			convertView.setBackgroundResource(R.drawable.list_grey);
		}
		return convertView;
	}

}
