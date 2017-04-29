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

import java.text.ParseException;
import java.util.ArrayList;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.marvell.cmmb.R;
import com.marvell.cmmb.common.Define;
import com.marvell.cmmb.common.Util;
import com.marvell.cmmb.resolver.MBBMSResolver;
import com.marvell.cmmb.resolver.ProgramItem;
import com.marvell.cmmb.service.MBBMSService;

public class ProgramListAdapter extends BaseAdapter {
	static class ViewHolder {
		TextView programTextView;

		TextView playtimeTextView;

		ImageView proDetailImageView;
	}

	private ArrayList<ProgramItem> mScheduleTable;// store the schedule table

	private LayoutInflater mInflater;

	private String mChooseDay;

	private int mFlag;

	private Context mContext;

	private MBBMSResolver mResolver;

	private int mChannelStatus;

	private int isForfree;
	
	private int mFreeStatus;

	public ProgramListAdapter(Context context,
			ArrayList<ProgramItem> scheduleTable, String chooseDay,
			int channelStatus, int forFree, int freeStatus) {
		mContext = context;
		mInflater = LayoutInflater.from(context);
		mScheduleTable = scheduleTable;
		mChooseDay = chooseDay;
		mResolver = MBBMSResolver.getInstance(mContext);
		mChannelStatus = channelStatus;
		isForfree = forFree;
		mFreeStatus = freeStatus;
	}

	public int getCount() {
		return mScheduleTable.size();
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
			convertView = mInflater.inflate(R.layout.program_list_item, null);
			holder = new ViewHolder();
			holder.programTextView = (TextView) convertView.findViewById(R.id.tv_program);
			holder.playtimeTextView = (TextView) convertView.findViewById(R.id.tv_time);
			holder.proDetailImageView = (ImageView) convertView.findViewById(R.id.iv_programdetail);
			convertView.setTag(holder);
		} else {
			holder = (ViewHolder) convertView.getTag();
		}
		holder.proDetailImageView.setTag(position);
		holder.programTextView.setText(mScheduleTable.get(position).getProgramName());
		try {
			holder.playtimeTextView.setText(Util.getPlaybackTime(mScheduleTable
					.get(position).getStartTime(), mScheduleTable.get(position)
					.getEndTime(), mScheduleTable.get(position).getDuration()));
		} catch (ParseException e1) {
			e1.printStackTrace();
		}
		if (position % 2 != 0) {
			convertView.setBackgroundResource(R.drawable.list_black);
		} else {
			convertView.setBackgroundResource(R.drawable.list_grey);
		}
		try {
			if (MBBMSService.sCurrentMode == Define.MBBMS_MODE) {
				mFlag = Util.compareTime(mScheduleTable.get(position)
						.getStartTime(), mScheduleTable.get(position).getEndTime());
			} else {
				mFlag = Util.compareTime(mScheduleTable.get(position)
						.getStartTime(), Util.getCmmbEndTime(mScheduleTable
						.get(position).getStartTime(), mScheduleTable.get(
						position).getDuration()));
			}
		} catch (ParseException e) {
			e.printStackTrace();
		}
		if (mFlag == -1) {
			holder.programTextView.setTextColor(mContext.getResources().getColor(R.color.solid_gray));
		} else if (mFlag == 1) {
			holder.programTextView.setTextColor(mContext.getResources().getColor(R.color.solid_white));
		} else {
			holder.programTextView.setTextColor(mContext.getResources().getColor(R.color.solid_blue));
		}

		holder.proDetailImageView.setOnClickListener(new OnClickListener() {

			public void onClick(View v) {
				int position = Integer.parseInt(v.getTag().toString());
				try {
					if (MBBMSService.sCurrentMode == Define.MBBMS_MODE) {
						mFlag = Util.compareTime(mScheduleTable.get(position)
								.getStartTime(), mScheduleTable.get(position).getEndTime());
					} else {
						mFlag = Util.compareTime(mScheduleTable.get(position)
								.getStartTime(), Util.getCmmbEndTime(
								mScheduleTable.get(position).getStartTime(),
								mScheduleTable.get(position).getDuration()));
					}
				} catch (ParseException e) {
					e.printStackTrace();
				}
				Intent intent = new Intent();
				intent.setAction("com.marvell.cmmb.VIEW_PROGRAM_DETAIL");
				Bundle bundle = new Bundle();
				bundle.putString("ChooseDay", mChooseDay);
				bundle.putString("ChooseChannel", mResolver
						.getChannelNameById(mScheduleTable.get(position).getChannelId()));
				bundle.putString("StartTime", mScheduleTable.get(position).getStartTime());
				if (MBBMSService.sCurrentMode == Define.MBBMS_MODE) {
					bundle.putString("EndTime", mScheduleTable.get(position).getEndTime());
				} else {
					try {
						bundle.putString("EndTime", Util.getCmmbEndTime(
								mScheduleTable.get(position).getStartTime(),
								mScheduleTable.get(position).getDuration()));
					} catch (ParseException e) {
						e.printStackTrace();
					}
				}
				bundle.putString("PlaybackProgram", mScheduleTable
						.get(position).getProgramName());
				if (mScheduleTable.get(position).getProgramDescription().equals("")) {
					bundle.putString("Description", mContext.getResources()
							.getString(R.string.nomessage));
				} else {
					bundle.putString("Description", mScheduleTable
							.get(position).getProgramDescription());
				}
				bundle.putInt("ViewFlag", mFlag);
				bundle.putInt("ChannelStatus", mChannelStatus);
				bundle.putInt("ForFree", isForfree);
				bundle.putInt("FreeStatus", mFreeStatus);
				bundle.putLong("PreviewId", mResolver.getPreviewId(
						mScheduleTable.get(position).getProgramId(),Define.SERVICE_PREVIEW));
				intent.putExtras(bundle);
				v.getContext().startActivity(intent);
			}
		});
		convertView.setId(mFlag);
		return convertView;
	}
}
