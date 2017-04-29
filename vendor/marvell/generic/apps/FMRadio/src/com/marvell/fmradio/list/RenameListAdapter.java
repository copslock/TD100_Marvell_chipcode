
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
package com.marvell.fmradio.list;
import java.util.List;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.marvell.fmradio.R;
import com.marvell.fmradio.util.ChannelHolder;
import com.marvell.fmradio.util.LogUtil;
import com.marvell.fmradio.util.Station;


public class RenameListAdapter extends BaseAdapter
{
	private static final String TAG = "RenameListAdapter";
	private List<Station> mList = null;
	private LayoutInflater mInflater = null;
	private String mWorkFreq = null;


	public RenameListAdapter(Context context, List<Station> list)
	{
		mList = list;
		mInflater = LayoutInflater.from(context);
		mWorkFreq = String.valueOf(ChannelHolder.getInstance().mWorkFreq);
	}

	public View getView(int position, View convertView, ViewGroup parent)
	{
		View view = null;
		FMHolder holder = null;
		
		LogUtil.d(TAG, "pos: " + position);
		LogUtil.d(TAG, "convertView: " + convertView);
		
		if (null == convertView || null == convertView.getTag())
		{
			LogUtil.d(TAG, "new station layout!");
			view = mInflater.inflate(R.layout.browse_station_item, parent, false);
			holder = newHolder(view);
		}
		else
		{
			view = convertView;
			holder = (FMHolder) view.getTag();		
		}
		
		LogUtil.d(TAG, "holder: " + holder);

		Station item = (Station) getItem(position);

		if (item != null) {
			holder.mFreq.setText(String.format("%6s", item.mFreq));
			holder.mName.setText(item.mName);
		}

		if (mWorkFreq.equals(item.mFreq))
			holder.mIconRun.setVisibility(View.VISIBLE);
		else
			holder.mIconRun.setVisibility(View.INVISIBLE);			

		return view;

	}

	public int getCount()
	{	
		return mList.size();
	}

	public Object getItem(int position)
	{
		return mList.get(position);
	}

	public long getItemId(int position)
	{
		return 0;
	}
	
	private FMHolder newHolder(View view)
	{
		FMHolder holder = new FMHolder();
		holder.mFreq = (TextView) view.findViewById(R.id.text_view_freq);
		holder.mName = (TextView) view.findViewById(R.id.text_view_name);
		holder.mIconRun = (ImageView) view.findViewById(R.id.image_view_run);
		view.setTag(holder);
		
		return holder;
	}
	
	class FMHolder
	{
		TextView mFreq;
		TextView mName;
		ImageView mIconRun;
	}	
	
}
