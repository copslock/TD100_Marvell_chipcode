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
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.TextView;

import com.marvell.cmmb.R;
import com.marvell.cmmb.activity.PurchaseManageActivity;
import com.marvell.cmmb.common.Define;
import com.marvell.cmmb.resolver.MBBMSResolver;
import com.marvell.cmmb.resolver.PurchaseInfoItem;
import com.marvell.cmmb.resolver.PurchaseRefItem;
import com.marvell.cmmb.view.dialog.ConfirmDialog;

public class PurchaseListAdapter extends BaseAdapter implements OnClickListener {
	static class ViewHolder {
		TextView subNameTextView;

		TextView warningTextView;

		TextView includeChannelTextView;

		TextView subTimeTextView;

		TextView ratesTextView;

		TextView descriptionTextView;

		Button subscribeButton;
	}

	private LayoutInflater mSubScribeInflater;

	private Context mContext;

	private ConfirmDialog mConfirmDialog;

	private ArrayList<PurchaseInfoItem> mSubscriptionList = new ArrayList<PurchaseInfoItem>();

	private ArrayList<PurchaseRefItem> mReferenceList = new ArrayList<PurchaseRefItem>();

	private ArrayList<Long> mReferenceIdList = new ArrayList<Long>();

	private ArrayList<String> mReferenceNameList = new ArrayList<String>();

	public static int sSelectedPosition;

	private MBBMSResolver mResolver;

	private String mReferenceName;

	private int mPeriod;

	public PurchaseListAdapter(Context context, ArrayList<PurchaseInfoItem> list) {
		mContext = context;
		mSubScribeInflater = LayoutInflater.from(context);
		mSubscriptionList = list;
		mResolver = MBBMSResolver.getInstance(mContext);
	}

	public int getCount() {
		return mSubscriptionList.size();
	}

	public String getIncludedChannel(int position) {
		mReferenceList = mResolver.getReferenceList(mSubscriptionList.get(position).getPurchaseId());
		mReferenceIdList = mResolver.getReferenceIdList(mSubscriptionList.get(position).getPurchaseId());
		String referenceName = "";
		if (mReferenceList.size() != 0) {
			if (mSubscriptionList.get(position).getPurchaseType() != Define.PROGRAM_PER_TIME) {
				mReferenceNameList = mResolver.getChannelName(mReferenceIdList);
			} else {
				mReferenceNameList = mResolver.getProgramName(mReferenceIdList);
			}
			if (mReferenceNameList.size() == 0) {
				referenceName = "";
			} else if (mReferenceNameList.size() == 1) {
				referenceName = mReferenceNameList.get(0);
			} else {
				referenceName = mReferenceNameList.get(0);
				for (int i = 1; i < mReferenceNameList.size(); i++) {
					referenceName = referenceName + "," + mReferenceNameList.get(i);
				}
			}
		}
		return referenceName;
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
			convertView = mSubScribeInflater.inflate(R.layout.subscription_item, null);
			holder = new ViewHolder();
			holder.subNameTextView = (TextView) convertView.findViewById(R.id.tv_subscrptionname);
			holder.warningTextView = (TextView) convertView.findViewById(R.id.tv_explace);
			holder.includeChannelTextView = (TextView) convertView.findViewById(R.id.tv_includechannel);
			holder.subTimeTextView = (TextView) convertView.findViewById(R.id.tv_subscriptiontime);
			holder.ratesTextView = (TextView) convertView.findViewById(R.id.tv_rates);
			holder.descriptionTextView = (TextView) convertView.findViewById(R.id.tv_description);
			holder.subscribeButton = (Button) convertView.findViewById(R.id.bt_subscribe);
			convertView.setTag(holder);
		} else {

			holder = (ViewHolder) convertView.getTag();
		}
		mReferenceName = getIncludedChannel(position);
		holder.subscribeButton.setTag(position);
		if (mResolver.getPurchaseStatus(mSubscriptionList.get(position).getPurchaseId()) == 1) {
			holder.subNameTextView.setText(mSubscriptionList.get(position).getPurchaseName()
					+ " " + mContext.getResources().getString(R.string.hasset));
			holder.subscribeButton.setText(mContext.getResources().getString(R.string.unset));
			if (mSubscriptionList.get(position).getPlaceStatus() == 1) {
				holder.warningTextView.setVisibility(View.INVISIBLE);
			} else {
				holder.warningTextView.setVisibility(View.VISIBLE);
			}
		} else {
			holder.subNameTextView.setText(mSubscriptionList.get(position).getPurchaseName()
					+ " " + mContext.getResources().getString(R.string.notset));
			holder.subscribeButton.setText(mContext.getResources().getString(R.string.set));
			holder.warningTextView.setVisibility(View.INVISIBLE);
		}
		if (mSubscriptionList.get(position).getPurchaseType() != Define.PROGRAM_PER_TIME) {
			holder.includeChannelTextView.setText(mContext.getResources()
					.getString(R.string.includechannel) + mReferenceName);
		} else {
			holder.includeChannelTextView.setText(mContext.getResources()
					.getString(R.string.includeprogram) + mReferenceName);
		}
		if (mSubscriptionList.get(position).getPurchasePeriod() == 0) {
			mPeriod = 1;
		} else {
			mPeriod = mSubscriptionList.get(position).getPurchasePeriod();
		}
		if(mSubscriptionList.get(position).getPurchaseType() == Define.CHANNELSET_MULTI_MONTH) {
			holder.subTimeTextView.setText(mContext.getResources().getString(
					R.string.multisubscriptiontime) + " " + mPeriod
					+ mContext.getResources().getString(R.string.geyue));
			holder.ratesTextView.setText((mContext.getResources().getString(
				R.string.multirates) + " "
				+ mSubscriptionList.get(position).getPurchasePrice()
				+ mContext.getResources().getString(R.string.yuan)));
		} else {
			holder.subTimeTextView.setText(mContext.getResources().getString(
					R.string.subscriptiontime) + " "
					+ mContext.getResources().getString(R.string.baoyue));
			holder.ratesTextView.setText(mContext.getResources().getString(
					R.string.rates) + " "
					+ mSubscriptionList.get(position).getPurchasePrice()
					+ mContext.getResources().getString(R.string.yuanyue));
		}
		holder.descriptionTextView.setText(mSubscriptionList.get(position).getPurchaseDescription());
		if (position % 2 != 0) {
			convertView.setBackgroundResource(R.drawable.list_black_big);
		} else {
			convertView.setBackgroundResource(R.drawable.list_grey_big);
		}
		holder.subscribeButton.setOnClickListener(this);
		return convertView;
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.bt_subscribe:
			int status;
			String promptMessage;
			sSelectedPosition = Integer.parseInt(v.getTag().toString());
			status = mResolver.getPurchaseStatus(mSubscriptionList.get(
					sSelectedPosition).getPurchaseId());
			if (status == 0) {
				promptMessage = v.getContext().getResources().getString(
						R.string.subscriptionconfirm)
						+ mSubscriptionList.get(sSelectedPosition).getPurchaseName() + "?";
			} else {
				promptMessage = v.getContext().getResources().getString(
						R.string.unsubscriptionconfirm)
						+ mSubscriptionList.get(sSelectedPosition).getPurchaseName() + "?";
			}
			showConfirmDialog(promptMessage, status);
			break;
		}
	}

	private void showConfirmDialog(String promptMsg, int status) {
		if (mConfirmDialog != null) {
			mConfirmDialog.dismiss();
			mConfirmDialog = null;
		}
		mConfirmDialog = new ConfirmDialog(mContext, R.drawable.dialog_hint,
				mContext.getResources().getString(R.string.hint), promptMsg);
		mConfirmDialog.setCallBack(PurchaseManageActivity.sInstance, status);
		if (!mConfirmDialog.isShowing()) {
			mConfirmDialog.show();
		}
	}

}
