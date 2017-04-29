
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

import com.marvell.fmradio.R;
import com.marvell.fmradio.util.Station;
import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;


public class RemoveListAdapter extends ArrayAdapter<Station>
{
	private static final String TAG = "RemoveListAdapter";
	public class FMHolder
	{
		public TextView mFreq;
		public TextView mName;
		public ImageView mIcon;
		public boolean mSelect;	
	}

	private int mListItemLayoutId = -1;
	private LayoutInflater mInflater = null;
	private boolean mRemoveArray[] = null;
	private static int mSize = -1;

	public RemoveListAdapter(Context context, int textViewResourceId, List<Station> objects)
	{
		super(context, textViewResourceId, objects);
		mListItemLayoutId = textViewResourceId;
		mInflater = LayoutInflater.from(context);
		
		mSize = objects.size();
		mRemoveArray = new boolean[mSize];
		for (int i = 0; i < mSize; i++)
			mRemoveArray[i] = false;		
	}

	public void setChecked(int position)
	{
		mRemoveArray[position] = ! mRemoveArray[position];
		notifyDataSetChanged();
	}
	
	public void selectAll()
	{
		for (int i = 0; i < mSize; i++)
			mRemoveArray[i] = true;	
		notifyDataSetChanged();
	}
	
	public void cancelAll()
	{
		for (int i = 0; i < mSize; i++)
			mRemoveArray[i] = false;	
		notifyDataSetChanged();
	}
	
	public boolean getChecked(int position)
	{
		return mRemoveArray[position];
	}
	
	public View getView(int position, View convertView, ViewGroup parent)
	{
		View view = null;
		FMHolder holder = null;

		if (null == convertView)
		{
			view = mInflater.inflate(mListItemLayoutId, parent, false);
			holder = new FMHolder();
			holder.mFreq = (TextView) view.findViewById(R.id.text_view_freq);
			holder.mName = (TextView) view.findViewById(R.id.text_view_name);
			holder.mIcon = (ImageView) view.findViewById(R.id.image_view_remove);
			holder.mSelect = false;
			view.setTag(holder);
		}
		else
		{
			view = convertView;
			holder = (FMHolder) view.getTag();
		}

		Station item = (Station) getItem(position);
		
		if (null == item)
			return null;

		holder.mFreq.setText(item.mFreq);
		holder.mName.setText(item.mName);
		
		if (mRemoveArray[position])
			holder.mIcon.setImageResource(R.drawable.btn_dialog_selected);
		else
			holder.mIcon.setImageResource(R.drawable.btn_dialog_normal);
		return view;

	}
}
