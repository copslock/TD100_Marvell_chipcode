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

package com.marvell.cmmb.activity;

import java.text.ParseException;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import com.marvell.cmmb.R;
import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.common.AppBase;
import com.marvell.cmmb.common.Util;
import com.marvell.cmmb.resolver.ChannelItem;
import com.marvell.cmmb.resolver.ProgramItem;
import com.marvell.cmmb.service.MBBMSService;

public class SearchProgramActivity extends AppBase implements OnClickListener {
	private Spinner mDateSpinner;

	private Spinner mChannelSpinner;

	private ArrayAdapter<String> mChannelAapter;

	private ArrayAdapter<String> mDateAapter;

	private Button mSearchButton;

	private EditText mKeywordEditText;

	private String mKeyword;

	private String mChannelName;

	private String mDate;

	private Calendar mCalendar;

	private HashMap<String, String> mDateMap = new HashMap<String, String>();

	public static ArrayList<ProgramItem> mScheduleList = new ArrayList<ProgramItem>();

	private static final String TAG = "SearchProgramActivity";

	private Toast mNoProToast;

	/**
	 * return the search result list
	 */
	private ArrayList<ProgramItem> fetchProgramList() {
		ArrayList<ProgramItem> list;
		ArrayList<ProgramItem> destList= new ArrayList<ProgramItem>();
		if (mChannelName.equals(SearchProgramActivity.this.getResources()
				.getString(R.string.all))) {
			if (mDate.equals(SearchProgramActivity.this.getResources()
					.getString(R.string.all))) {
				list = getAllDateProgramList();
			} else {
				list = mResolver.searchProgramByDate(mKeyword, mDateMap
						.get(mDate), MBBMSService.sCurrentMode);
				try {
					Util.filterProgramByDate(mDateMap.get(mDate),list, destList);//to filter the endtime like 2011-11-23 00:00:00
				}catch (ParseException e) {
					e.printStackTrace();
				}	
				list.clear();
				list = destList;	
				destList = null;
			}
		} else {
			if (mDate.equals(SearchProgramActivity.this.getResources()
					.getString(R.string.all))) {
				list = getAllDateSpecificProgramList();
			} else {
				list = mResolver.searchSpecificProgram(mKeyword, mDateMap
						.get(mDate), mResolver.getChannelId(mChannelName,
						MBBMSService.sCurrentMode), MBBMSService.sCurrentMode);
				try {
					Util.filterProgramByDate(mDateMap.get(mDate),list, destList);//to filter the endtime like 2011-11-23 00:00:00
				}catch (ParseException e) {
					e.printStackTrace();
				}	
				list.clear();
				list = destList;
				destList = null;
			}
		}
		return list;
	}

	private ArrayList<ProgramItem> getAllDateProgramList() {
		ArrayList<ProgramItem> list = new ArrayList<ProgramItem>();
		ArrayList<ProgramItem> subList = new ArrayList<ProgramItem>();
		String date = mDateMap.get(this.getResources().getString(R.string.today));
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "keyword is " + mKeyword
				+ " date is " + date, LogMask.APP_COMMON);
		subList = mResolver.searchProgramByDate(mKeyword, date, MBBMSService.sCurrentMode);
		list.addAll(subList);
		date = mDateMap.get(this.getResources().getString(R.string.tomorrow));
		subList = mResolver.searchProgramByDate(mKeyword, date, MBBMSService.sCurrentMode);
		list.addAll(subList);
		date = mDateMap.get(this.getResources().getString(R.string.aftertomorrow));
		subList = mResolver.searchProgramByDate(mKeyword, date, MBBMSService.sCurrentMode);
		list.addAll(subList);
		subList.clear();
		return list;
	}

	private ArrayList<ProgramItem> getAllDateSpecificProgramList() {
		ArrayList<ProgramItem> list = new ArrayList<ProgramItem>();
		ArrayList<ProgramItem> subList = new ArrayList<ProgramItem>();
		String date = mDateMap.get(this.getResources()
				.getString(R.string.today));
		subList = mResolver.searchSpecificProgram(mKeyword, date, mResolver
				.getChannelId(mChannelName, MBBMSService.sCurrentMode),
				MBBMSService.sCurrentMode);
		list.addAll(subList);
		date = mDateMap.get(this.getResources().getString(R.string.tomorrow));
		subList = mResolver.searchSpecificProgram(mKeyword, date, mResolver
				.getChannelId(mChannelName, MBBMSService.sCurrentMode), MBBMSService.sCurrentMode);
		list.addAll(subList);
		date = mDateMap.get(this.getResources().getString(
				R.string.aftertomorrow));
		subList = mResolver.searchSpecificProgram(mKeyword, date, mResolver
				.getChannelId(mChannelName, MBBMSService.sCurrentMode), MBBMSService.sCurrentMode);
		list.addAll(subList);
		subList.clear();
		return list;
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.search);
		initViews();
		setupViews();
		putMap();
	}

	@Override
	protected void onResume() {
		super.onResume();
		updateDate();
		updateViews();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
	}

	/**
	 * insert data to mDataMap
	 */
	private void putMap() {
		mCalendar = Calendar.getInstance();
		mDateMap.put(this.getResources().getString(R.string.today),
				Util.sDateFormater.format(mCalendar.getTime()));
		mCalendar.add(Calendar.DATE, 1);
		mDateMap.put(this.getResources().getString(R.string.tomorrow),
				Util.sDateFormater.format(mCalendar.getTime()));
		mCalendar.add(Calendar.DATE, 1);
		mDateMap.put(this.getResources().getString(R.string.aftertomorrow),
				Util.sDateFormater.format(mCalendar.getTime()));
	}

	@Override
	public void initViews() {
		mKeywordEditText = (EditText) findViewById(R.id.et_keyword);
		mSearchButton = (Button) findViewById(R.id.bt_search);
		mDateSpinner = (Spinner) findViewById(R.id.sp_date);
		mChannelSpinner = (Spinner) findViewById(R.id.sp_channel);

	}

	@Override
	public void setupViews() {
		mSearchButton.setOnClickListener(this);
	}

	@Override
	public void updateDate() {
		ArrayList<ChannelItem> channelList = new ArrayList<ChannelItem>();
		channelList = Util.getChannelList(MBBMSService.sCurrentMode, this);
		final String[] channels = new String[channelList.size() + 1];
		channels[0] = this.getResources().getString(R.string.all);
		for (int i = 1; i < channels.length; i++) {
			channels[i] = channelList.get(i - 1).getChannelName();
		}
		final String[] date = { this.getResources().getString(R.string.all),
				this.getResources().getString(R.string.today),
				this.getResources().getString(R.string.tomorrow),
				this.getResources().getString(R.string.aftertomorrow) };

		mChannelAapter = new ArrayAdapter<String>(this,
				android.R.layout.simple_spinner_item, channels);
		mDateAapter = new ArrayAdapter<String>(this,
				android.R.layout.simple_spinner_item, date);
		mChannelAapter.setDropDownViewResource(R.layout.setting_spinner);
		mDateAapter.setDropDownViewResource(R.layout.setting_spinner);
	}

	@Override
	public void updateViews() {
		mDateSpinner.setAdapter(mDateAapter);
		mChannelSpinner.setAdapter(mChannelAapter);

	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.bt_search:
			mKeyword = mKeywordEditText.getText().toString().replaceAll("'", "''");
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "keyword is " + mKeyword,
					LogMask.APP_COMMON);
			TextView channelView = (TextView) mChannelSpinner.getSelectedView();
			TextView dateView = (TextView) mDateSpinner.getSelectedView();
			mChannelName = channelView.getText().toString();
			mDate = dateView.getText().toString();
			mScheduleList = fetchProgramList();
			if (mScheduleList.size() == 0) {
				if (mNoProToast == null) {
					mNoProToast = Toast.makeText(SearchProgramActivity.this,
							SearchProgramActivity.this.getResources()
							.getString(R.string.noprogramstart) + " '"
							+ mKeywordEditText.getText().toString()
							+ "' " + getResources().getString(R.string.noprogramend),
							Toast.LENGTH_SHORT);
				}
				if (mKeywordEditText.getText().toString().equals("")) {
					mNoProToast.setText(getResources().getString(
							R.string.searchnoprogram)
							+ getResources().getString(R.string.noprogramend));
				} else {
					mNoProToast.setText(getResources().getString(
							R.string.noprogramstart)
							+ " '" + mKeywordEditText.getText().toString()
							+ "' " + getResources().getString(R.string.noprogramend));
				}
				mNoProToast.show();
			} else {
				Intent resultIntent = new Intent();
				resultIntent.setClass(SearchProgramActivity.this, SearchResultActivity.class);
				resultIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
				SearchProgramActivity.this.startActivity(resultIntent);
			}
			break;
		}

	}

}
