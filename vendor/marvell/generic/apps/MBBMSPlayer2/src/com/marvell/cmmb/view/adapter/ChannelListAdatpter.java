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
import java.util.Calendar;
import java.util.HashMap;

import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.marvell.cmmb.R;
import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.common.Util;
import com.marvell.cmmb.resolver.ChannelItem;
import com.marvell.cmmb.resolver.MBBMSResolver;
import com.marvell.cmmb.resolver.ProgramItem;
import com.marvell.cmmb.service.MBBMSService;

public class ChannelListAdatpter extends BaseAdapter {
	static class ViewHolder {
		TextView channelTextView;

		TextView timeTextView;

		TextView programTextView;

		ImageView cryptImgeView;

		ImageView playState;
	}

	private LayoutInflater mInflater;

	private ArrayList<ChannelItem> mChannelList = new ArrayList<ChannelItem>();

	private MBBMSResolver mResolver;

	private HashMap<Integer, ProgramItem> mCurrentProgramList = new HashMap<Integer, ProgramItem>();

	private int mChannelCount;

	private static final String TAG = "ChannelListAdapter";

	public ChannelListAdatpter(Context context,
			ArrayList<ChannelItem> serviceList) {
		mInflater = LayoutInflater.from(context);
		mChannelList = serviceList;
		mResolver = MBBMSResolver.getInstance(context);
		mChannelCount = mChannelList.size();
		getProgramList();
	}

	public int getChannelStatus(int position) {
		String subscriptionId = mResolver.getPurchaseId(mChannelList.get(position).getChannelId());
		int status = -1;
		if (!subscriptionId.equals(String.valueOf(0))) {
			status = mResolver.getPurchaseStatus(subscriptionId);
		}
		return status;
	}

	public int getCount() {
		return mChannelCount;
	}

	public Object getItem(int position) {
		return position;
	}

	public long getItemId(int position) {
		return position;
	}

	private void getProgramList() {
		ArrayList<ProgramItem> programList = new ArrayList<ProgramItem>();
		long channelId;
		int freq;
		int flag = -2;
		ProgramItem program = new ProgramItem();
		for (int i = 0; i < mChannelCount; i++) {
			channelId = mChannelList.get(i).getChannelId();
			freq = mChannelList.get(i).getFrequecy();
			Calendar calendar = Calendar.getInstance();
			String currentDate = Util.sDateFormater.format(calendar.getTime());
			programList = mResolver.getProgramList(channelId, freq,
					currentDate, MBBMSService.sCurrentMode);
			for (int j = 0; j < programList.size(); j++) {
				program = programList.get(j);
				try {
					if (MBBMSService.sCurrentMode == Util.MBBMS_MODE) {
						flag = Util.compareTime(program.getStartTime(), program.getEndTime());
					} else {
						flag = Util.compareTime(program.getStartTime(), Util
								.getCmmbEndTime(program.getStartTime(), program.getDuration()));
					}
				} catch (ParseException e) {
					e.printStackTrace();
				}
				if (flag == 0) {
					mCurrentProgramList.put(i, program);
				}
			}
		}
	}

	public View getView(int position, View convertView, ViewGroup parent) {
		ViewHolder holder;
		if (convertView == null) {
			holder = new ViewHolder();
			convertView = mInflater.inflate(R.layout.channel_list_item, null);
			holder.channelTextView = (TextView) convertView.findViewById(R.id.tv_channel);
			holder.timeTextView = (TextView) convertView.findViewById(R.id.tv_time);
			holder.programTextView = (TextView) convertView.findViewById(R.id.tv_program);
			holder.cryptImgeView = (ImageView) convertView.findViewById(R.id.iv_crypt);
			holder.playState = (ImageView) convertView.findViewById(R.id.iv_playing);
			convertView.setTag(holder);
		} else {
			holder = (ViewHolder) convertView.getTag();
		}
		holder.channelTextView.setText(mChannelList.get(position).getChannelName());
		String playTime = null;
		String programName = null;
		if (mCurrentProgramList.containsKey(position)) {
			try {
				playTime = Util.getPlaybackTime(mCurrentProgramList.get(
						position).getStartTime(), mCurrentProgramList.get(
						position).getEndTime(), mCurrentProgramList.get(
						position).getDuration());
			} catch (ParseException e) {
				e.printStackTrace();
			}
			programName = mCurrentProgramList.get(position).getProgramName();
			holder.timeTextView.setText(playTime);
			if (programName.length() <= 12) {
				holder.programTextView.setText(programName);
			} else {
				programName = programName.substring(0, 11) + "...";
				holder.programTextView.setText(programName);
			}
		} else {
			holder.timeTextView.setText("");
			holder.programTextView.setText("");
		}
		
		if(mChannelList.get(position).getFreeState() == 1) {
			holder.cryptImgeView.setVisibility(View.INVISIBLE);
		} else {
			if(mChannelList.get(position).getForFreeState() == 1) {
				if(MBBMSService.isSGUpdateFailed) {
					holder.cryptImgeView.setImageResource(R.drawable.icon_encrpt);
				} else {
					holder.cryptImgeView.setImageResource(R.drawable.icon_unencrpt);
				}	
			} else {
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), 
						"isSGUpdateFailed is " + MBBMSService.isSGUpdateFailed, LogMask.APP_COMMON);
				if(mChannelList.get(position).getEncryptState() == 1 && MBBMSService.isUpdateSTFinished && !MBBMSService.isSGUpdateFailed) {
					holder.cryptImgeView.setImageResource(R.drawable.icon_unencrpt);
				} else {
					holder.cryptImgeView.setImageResource(R.drawable.icon_encrpt);
				}
			}
			holder.cryptImgeView.setVisibility(View.VISIBLE);
		}

		if (position == MBBMSService.sCurrentPlayPosition) {
			holder.playState.setVisibility(View.VISIBLE);
		} else {
			holder.playState.setVisibility(View.GONE);
		}

		if (position % 2 == 0) {
			convertView.setBackgroundResource(R.drawable.list_grey);

		} else {
			convertView.setBackgroundResource(R.drawable.list_black);
		}
		return convertView;
	}

}
