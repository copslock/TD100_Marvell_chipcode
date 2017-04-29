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
import java.util.Date;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
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
import com.marvell.cmmb.resolver.ProgramItem;
import com.marvell.cmmb.resolver.MBBMSResolver;
import com.marvell.cmmb.service.MBBMSService;

public class SearchResultAdapter extends BaseAdapter {
	static class ViewHolder {
		TextView programTextView;

		TextView timeTextView;

		TextView channelTextView;

		TextView dayTextView;

		ImageView programDetailImageView;
	}

	private LayoutInflater mSearchInflater;

	private ArrayList<ProgramItem> mScheduleList = new ArrayList<ProgramItem>();

	private Date mCurrentDate;

	private int mFlag;

	private Context mContext;

	private MBBMSResolver mResolver;

	private static final String TAG = "SearchResultAdapter";

	public SearchResultAdapter(Context context, ArrayList<ProgramItem> list) {
		mContext = context;
		mResolver = MBBMSResolver.getInstance(mContext);
		mSearchInflater = LayoutInflater.from(context);
		mScheduleList = list;
		try {
			Date date = new Date();
			String temp = Util.sDateFormater.format(date);
			mCurrentDate = Util.sDateTimeFormater.parse(temp + " 00:00:00");
		} catch (ParseException e) {
			e.printStackTrace();
		}
	}

	public int getChannelStatus(int position) {
		String subscriptionId = mResolver.getPurchaseId(mScheduleList.get(
				position).getChannelId());
		int status = -1;
		if (!subscriptionId.equals(String.valueOf(0))) {
			status = mResolver.getPurchaseStatus(subscriptionId);
		}
		return status;
	}

	public int getCount() {
		return mScheduleList.size();
	}

	private String getDayState(Context context, Date currentDate,
			ArrayList<ProgramItem> scheduleList, int position) {
		long offset = 0;
		try {
			offset = Util.sDateTimeFormater.parse(
					scheduleList.get(position).getStartTime()).getTime()
					- currentDate.getTime();
		} catch (ParseException e) {
			e.printStackTrace();
		}
		String result = "";
		if (offset < 86400000) {
			result = context.getResources().getString(R.string.today);
		} else if (offset >= 86400000 && offset <= 172800000) {
			result = context.getResources().getString(R.string.tomorrow);
		} else if (offset > 172800000) {
			result = context.getResources().getString(R.string.aftertomorrow);
		}
		return result;
	}

	public Object getItem(int position) {
		return position;
	}

	public long getItemId(int position) {
		return position;
	}

	public ArrayList<ProgramItem> getProgramList() {
		return mScheduleList;
	}

	public View getView(int position, View convertView, ViewGroup parent) {
		ViewHolder holder;
		if (convertView == null) {
			convertView = mSearchInflater.inflate(R.layout.search_list_item,
					null);
			holder = new ViewHolder();
			holder.programTextView = (TextView) convertView.findViewById(R.id.tv_program);
			holder.timeTextView = (TextView) convertView.findViewById(R.id.tv_time);
			holder.channelTextView = (TextView) convertView.findViewById(R.id.tv_channel);
			holder.dayTextView = (TextView) convertView.findViewById(R.id.tv_day);
			holder.programDetailImageView = (ImageView) convertView
					.findViewById(R.id.iv_programdetail);
			convertView.setTag(holder);
		} else {
			holder = (ViewHolder) convertView.getTag();
		}
		holder.programDetailImageView.setTag(position);
		holder.programTextView.setText(mScheduleList.get(position).getProgramName());
		try {
			holder.timeTextView.setText(Util.getPlaybackTime(mScheduleList.get(
					position).getStartTime(), mScheduleList.get(position)
					.getEndTime(), mScheduleList.get(position).getDuration()));
		} catch (ParseException e1) {
			e1.printStackTrace();
		}
		holder.channelTextView
				.setText(mResolver.getChannelNameById(mScheduleList.get(
						position).getChannelId()));
		holder.dayTextView.setText(getDayState(mContext, mCurrentDate,
				mScheduleList, position));
		if (position % 2 != 0) {
			convertView.setBackgroundResource(R.drawable.list_black);
		} else {
			convertView.setBackgroundResource(R.drawable.list_grey);
		}
		try {
			if (MBBMSService.sCurrentMode == Define.MBBMS_MODE) {
				mFlag = Util.compareTime(mScheduleList.get(position)
						.getStartTime(), mScheduleList.get(position).getEndTime());
			} else {
				mFlag = Util.compareTime(mScheduleList.get(position)
						.getStartTime(), Util.getCmmbEndTime(mScheduleList.get(
						position).getStartTime(), mScheduleList.get(position).getDuration()));
			}
		} catch (ParseException e) {
			e.printStackTrace();
		}
		if (mFlag == -1) {
			holder.programTextView.setTextColor(mContext.getResources()
					.getColor(R.color.solid_gray));
		} else if (mFlag == 1) {
			holder.programTextView.setTextColor(mContext.getResources()
					.getColor(R.color.solid_white));
		} else {
			holder.programTextView.setTextColor(mContext.getResources()
					.getColor(R.color.solid_blue));
		}
		holder.programDetailImageView.setOnClickListener(new OnClickListener() {

			public void onClick(View v) {
				int position = Integer.parseInt(v.getTag().toString());
				try {
					mFlag = Util.compareTime(mScheduleList.get(position)
							.getStartTime(), mScheduleList.get(position).getEndTime());
				} catch (ParseException e) {
					e.printStackTrace();
				}
				Intent intent = new Intent();
				intent.setAction("com.marvell.cmmb.VIEW_SEARCH_DETAIL");
				Bundle bundle = new Bundle();
				bundle.putString("ChooseDay", getDayState(mContext,
						mCurrentDate, mScheduleList, position));
				bundle.putString("ChooseChannel", mResolver
						.getChannelNameById(mScheduleList.get(position).getChannelId()));
				bundle.putString("StartTime", mScheduleList.get(position)
						.getStartTime());
				if (MBBMSService.sCurrentMode == Define.MBBMS_MODE) {
					bundle.putString("EndTime", mScheduleList.get(position).getEndTime());
				} else {
					try {
						bundle.putString("EndTime", Util.getCmmbEndTime(
								mScheduleList.get(position).getStartTime(),
								mScheduleList.get(position).getDuration()));
					} catch (ParseException e) {
						e.printStackTrace();
					}
				}
				if (mScheduleList.get(position).getProgramDescription().equals(
						"")) {
					bundle.putString("Description", mContext.getResources()
							.getString(R.string.nomessage));
				} else {
					bundle.putString("Description", mScheduleList.get(position)
							.getProgramDescription());
				}
				bundle.putString("PlaybackProgram", mScheduleList.get(position).getProgramName());
				bundle.putInt("ChannelStatus", getChannelStatus(position));
				bundle.putInt("ForFree", isFree(position));
				bundle.putInt("ViewFlag", mFlag);
				intent.putExtras(bundle);
				v.getContext().startActivity(intent);
			}
		});
		convertView.setId(mFlag);// indicate whether the program can play now.
		return convertView;
	}

	public int isFree(int position) {
		int forfree = mResolver.getFreeStatus(mScheduleList.get(position)
				.getChannelId());
		return forfree;
	}
}
