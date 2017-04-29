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

import android.content.Context;
import android.os.Handler;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.marvell.cmmb.R;
import com.marvell.cmmb.activity.EbMsgManageActivity;
import com.marvell.cmmb.common.Define;
import com.marvell.cmmb.resolver.EmergencyItem;
import com.marvell.cmmb.view.dialog.ConfirmDialog;

public class EmergencyAdapter extends BaseAdapter implements Define {
	static class ViewHolder {
		ImageView levelImageView;

		TextView dateTextView;

		TextView messageTextView;

		ImageView deleteImageView;
	}

	private ArrayList<EmergencyItem> mEmergencyTable = new ArrayList<EmergencyItem>();

	private LayoutInflater mEmergencyInflater;

	public static int sEmergencyPosition;

	private static final int EMERGENCY_MAXLENGTH = 8;

	private ConfirmDialog mConfirmDialog = null;

	private Context mContext;

	private Handler mEbMsgHandler;

	public EmergencyAdapter(Context context, ArrayList<EmergencyItem> list) {
		mEmergencyInflater = LayoutInflater.from(context);
		mEmergencyTable = list;
		mContext = context;
	}

	public int getCount() {
		return mEmergencyTable.size();
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
			convertView = mEmergencyInflater.inflate(R.layout.emergency_item,
					null);
			holder = new ViewHolder();
			holder.levelImageView = (ImageView) convertView.findViewById(R.id.iv_emergencylevel);
			holder.dateTextView = (TextView) convertView.findViewById(R.id.tv_emergencydate);
			holder.messageTextView = (TextView) convertView.findViewById(R.id.tv_emergencymessage);
			holder.deleteImageView = (ImageView) convertView.findViewById(R.id.iv_emergencydelete);
			convertView.setTag(holder);
		} else {
			holder = (ViewHolder) convertView.getTag();
		}
		holder.deleteImageView.setTag(position);
		switch (mEmergencyTable.get(position).getEmergencyLevel()) {
		case 1:
			holder.levelImageView.setImageResource(R.drawable.emergency_level_01);
			break;
		case 2:
			holder.levelImageView.setImageResource(R.drawable.emergency_level_02);
			break;
		case 3:
			holder.levelImageView.setImageResource(R.drawable.emergency_level_03);
			break;
		case 4:
			holder.levelImageView.setImageResource(R.drawable.emergency_level_04);
			break;
		}
		holder.dateTextView.setText(mEmergencyTable.get(position).getEmergencyDate());
//		if (mEmergencyTable.get(position).getEmergencyMessage().length() > EMERGENCY_MAXLENGTH) {
//			holder.messageTextView.setText(mEmergencyTable.get(position).getEmergencyMessage()
//					.substring(0, EMERGENCY_MAXLENGTH - 1) + "...");
//		} else {
			holder.messageTextView.setText(mEmergencyTable.get(position).getEmergencyMessage());
//		}
		holder.deleteImageView.setOnClickListener(new OnClickListener() {

			public void onClick(View v) {
				sEmergencyPosition = Integer.parseInt(v.getTag().toString());
				mEbMsgHandler.sendEmptyMessage(DIALOG_CONFIRM_EMERG_DELETE_ITEM);
			}
		});
		if (position % 2 != 0) {
			convertView.setBackgroundResource(R.drawable.list_black);
		} else {
			convertView.setBackgroundResource(R.drawable.list_grey);
		}
		return convertView;
	}

	public void setData(ArrayList<EmergencyItem> list) {
		mEmergencyTable = list;
	}

	public void setEbMsgHandler(Handler handler) {
		mEbMsgHandler = handler;
	}
}
