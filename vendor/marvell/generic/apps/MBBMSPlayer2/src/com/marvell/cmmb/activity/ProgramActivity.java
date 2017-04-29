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
import java.util.Date;

import android.content.Intent;
import android.os.Bundle;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.Gallery;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.TextView;
import android.widget.AdapterView.OnItemClickListener;

import com.marvell.cmmb.R;
import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.common.AppBase;
import com.marvell.cmmb.common.Define;
import com.marvell.cmmb.common.Util;
import com.marvell.cmmb.manager.MenuManager;
import com.marvell.cmmb.resolver.ProgramItem;
import com.marvell.cmmb.service.MBBMSService;
import com.marvell.cmmb.view.adapter.ProgramListAdapter;
import com.marvell.cmmb.view.adapter.ProgramRefAdapter;
import com.marvell.cmmb.view.dialog.ConfirmDialog;
import com.marvell.cmmb.view.dialog.ConfirmDialogCallBack;

public class ProgramActivity extends AppBase implements OnClickListener,
		ConfirmDialogCallBack

{
	private TextView mDateTextView;

	private TextView mTodayTextView;

	private TextView mTomorrowTextView;

	private TextView mAftertomTextView;

	private Button mDateButton;

	private Gallery mChannellistGallery;

	private PopupWindow mDatePopupWindow;

	private int mSelectPosition = 1;

	private boolean isPopShow = false;

	private ProgramRefAdapter mChannelAdapter;

	private ProgramListAdapter mProgramAdapter;

	private ListView mProgramListView;

	private ArrayList<ProgramItem> mScheduleArrayList;

	private LayoutInflater mScheduleInflater;

	private String mSelectedDate;

	public static ProgramActivity sScheduleInstance = null;

	private ConfirmDialog mConfirmDialog;

	private int mChannelStatus = -1;//check is subscribed

	private int isForfree = -1;// check is clear stream
	
	private int freeStatus = -1;// check free status

	private int mPosition = 0;

	private static final String TAG = "ProgramActivity";

	private MenuManager mMenuManager;

	/**
	 * set the date user select and refresh the schedule table
	 * 
	 * @param offset
	 *            : the offset to today
	 * @param resId
	 *            : the resource id of the string to be set
	 * @throws ParseException
	 */
	private void chooseDate(int offset, int resId) {
		if (offset == 0) {
			Calendar calendar = Calendar.getInstance();
			mSelectedDate = Util.sDateFormater.format(calendar.getTime());
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "today is "
					+ mSelectedDate, LogMask.APP_COMMON);
			mDateTextView.setText(mSelectedDate);
		} else if (offset == 1) {
			Calendar calendar = Calendar.getInstance();
			calendar.add(Calendar.DATE, 1);
			mSelectedDate = Util.sDateFormater.format(calendar.getTime());
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "tomorrow is "
					+ mSelectedDate, LogMask.APP_COMMON);
			mDateTextView.setText(mSelectedDate);
		} else if (offset == 2) {
			Calendar calendar = Calendar.getInstance();
			calendar.add(Calendar.DATE, 2);
			mSelectedDate = Util.sDateFormater.format(calendar.getTime());
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"the day after tomorrow is " + mSelectedDate,
					LogMask.APP_COMMON);
			mDateTextView.setText(mSelectedDate);
		}
		mDateButton.setText(resId);
		if (mChannelAdapter.getCount() != 0) {// check whether channel list is
			// empty
//			mScheduleArrayList = mResolver.getProgramList(
//					mChannelAdapter.mChannelList.get(mSelectPosition).getChannelId(),
//					mChannelAdapter.mChannelList.get(mSelectPosition).getFrequecy(),
//					mSelectedDate, MBBMSService.sCurrentMode);
			try {
				mScheduleArrayList = getCurrentProgramList(mSelectedDate);
			} catch (ParseException e) {
				e.printStackTrace();
			}			
			mProgramAdapter = new ProgramListAdapter(this, mScheduleArrayList,
					mDateButton.getText().toString(), mChannelStatus, isForfree, freeStatus);
			mProgramListView.setAdapter(mProgramAdapter);
			mPosition = Util.getPosition(mScheduleArrayList);
			mProgramListView.setSelection(mPosition);
		}
		mDatePopupWindow.dismiss();
		isPopShow = false;
	}

	public void confirmDialogCancel(int type) {
		// TODO Auto-generated method stub

	}

	public void confirmDialogOk(int type) {
		if (type == DIALOG_CONFIRM_SUB) {
			Intent subIntent = new Intent();
			subIntent.putExtra("SubscribeStatus", 0);
			subIntent.setClass(this, PurchaseManageActivity.class);
			startActivity(subIntent);
		} else if (type == DIALOG_CONFIRM_PROGRAM_EXPIRE
				|| type == DIALOG_CONFIRM_PROGRAM_FUTURE) {
			Intent viewIntent = new Intent();
			viewIntent.setAction("com.marvell.cmmb.VIEW_SCHEDULE_ACTION");
			viewIntent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "mSelectPosition is "
					+ mSelectPosition, LogMask.APP_COMMON);
			viewIntent.putExtra("SchedulePosition", mSelectPosition);
			startActivity(viewIntent);
			ProgramActivity.this.finish();
		}
	}

	/**
	 * initialize the popupWindow of choose date
	 */
	private void initPopDate() {
		View mView = mScheduleInflater.inflate(R.layout.popmenu_date, null);
		mTodayTextView = (TextView) mView.findViewById(R.id.tv_today);
		mTodayTextView.setOnClickListener(this);
		mTomorrowTextView = (TextView) mView.findViewById(R.id.tv_tomorrow);
		mTomorrowTextView.setOnClickListener(this);
		mAftertomTextView = (TextView) mView.findViewById(R.id.tv_aftertomorrow);
		mAftertomTextView.setOnClickListener(this);
		mDatePopupWindow = new PopupWindow(mView, 50, 90);
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.bt_date:
			if (isPopShow) {
				mDatePopupWindow.dismiss();
				isPopShow = false;
			} else {
				mDatePopupWindow.showAtLocation((View) v.getParent(),
						Gravity.TOP | Gravity.LEFT, 256, 50);
				isPopShow = true;
			}
			break;
		case R.id.tv_today:
			chooseDate(0, R.string.today);
			break;
		case R.id.tv_tomorrow:
			chooseDate(1, R.string.tomorrow);
			break;
		case R.id.tv_aftertomorrow:
			chooseDate(2, R.string.aftertomorrow);
			break;
		}
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.setContentView(R.layout.schedule_view);
		initViews();
		setupViews();
		init();
	}

	public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);
		if (mMenuManager != null) {
			if (MBBMSService.sCurrentMode == Define.CMMB_MODE) {
				mMenuManager.InflatMenu(R.menu.menu_program_cmmb, menu);
			} else if (MBBMSService.sCurrentMode == Define.MBBMS_MODE) {
				mMenuManager.InflatMenu(R.menu.menu_program_mbbms, menu);
			}
		}
		return true;
	}

	public boolean onOptionsItemSelected(MenuItem item) {
		mMenuManager.onMenuClick(item.getItemId());
		return super.onOptionsItemSelected(item);
	}

	private void init() {
		mMenuManager = new MenuManager(this);
		sScheduleInstance = this;
		Calendar calendar = Calendar.getInstance();
		mSelectedDate = Util.sDateFormater.format(calendar.getTime());
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		return super.onKeyDown(keyCode, event);
	}

	@Override
	protected void onPause() {
		super.onPause();

	}

	@Override
	protected void onResume() {
		super.onResume();
		updateDate();
		updateViews();
	}

	/**
	 * show confirm dialog ,the message displayed depends on the type.
	 */

	private void showConfirmDialog(int type) {
		String promptMsg = " ";
		String channelName = " ";
		if (type == DIALOG_CONFIRM_SUB) {
			channelName = mChannelAdapter.getChannelList().get(mSelectPosition)
					.getChannelName();
			promptMsg = getResources().getString(R.string.promptsubscribestart)
					+ " '" + channelName + "' "
					+ getResources().getString(R.string.promptsubscribeend);

		} else if (type == DIALOG_CONFIRM_PROGRAM_EXPIRE) {
			promptMsg = this.getResources().getString(R.string.promgramexpire);
		} else if (type == DIALOG_CONFIRM_PROGRAM_FUTURE) {
			promptMsg = this.getResources()
					.getString(R.string.promgramnotstart);
		}
		if (mConfirmDialog != null) {
			mConfirmDialog.dismiss();
			mConfirmDialog = null;
		}
		mConfirmDialog = new ConfirmDialog(this, R.drawable.dialog_hint,
				getResources().getString(R.string.hint), promptMsg);
		mConfirmDialog.setCallBack(this, type);
		if (!mConfirmDialog.isShowing()) {
			mConfirmDialog.show();
		}

	}

	@Override
	public void initViews() {
		mScheduleInflater = LayoutInflater.from(this);
		mDateTextView = (TextView) findViewById(R.id.tv_date);
		mDateButton = (Button) findViewById(R.id.bt_date);
		mChannellistGallery = (Gallery) findViewById(R.id.gl_channellist);
		mProgramListView = (ListView) findViewById(R.id.lv_program);
		initPopDate();
	}

	@Override
	public void setupViews() {
		Intent intent = this.getIntent();
		if (intent.getExtras() != null) {
			mSelectPosition = intent.getExtras().getInt("Position");
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "mSelectPosition is "
					+ mSelectPosition, LogMask.APP_COMMON);
		}
		mChannelAdapter = new ProgramRefAdapter(this);
		mDateButton.setOnClickListener(this);
		mChannellistGallery.setSpacing(5);
		mChannellistGallery.setOnItemClickListener(new OnItemClickListener() {

			public void onItemClick(AdapterView<?> arg0, View arg1, int arg2, long arg3) {
				mSelectPosition = arg2;
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "mSelectPosition is "
						+ mSelectPosition, LogMask.APP_COMMON);
				mChannelStatus = mChannelAdapter
						.getChannelStatus(mSelectPosition);
				isForfree = mChannelAdapter.isFree(mSelectPosition);
				freeStatus = mChannelAdapter.getForFreeStatus(mSelectPosition);
//				mScheduleArrayList = mResolver.getProgramList(
//						mChannelAdapter.mChannelList.get(mSelectPosition)
//								.getChannelId(), mChannelAdapter.mChannelList
//								.get(mSelectPosition).getFrequecy(),
//						mSelectedDate, MBBMSService.sCurrentMode);
				try {
					mScheduleArrayList = getCurrentProgramList(mSelectedDate);
				} catch (ParseException e) {
					e.printStackTrace();
				}
				mProgramAdapter = new ProgramListAdapter(arg0.getContext(),
						mScheduleArrayList, mDateButton.getText().toString(),
						mChannelStatus, isForfree, freeStatus);
				mProgramListView.setAdapter(mProgramAdapter);
				mPosition = Util.getPosition(mScheduleArrayList);
				mProgramListView.setSelection(mPosition);
				for (int i = 0; i < mChannelAdapter.mChannelList.size(); i++) {
					if (i == arg2) {
						mChannelAdapter.mChannelMap.put(i, true);
					} else {
						mChannelAdapter.mChannelMap.put(i, false);
					}
				}
				for (int j = 0; j < mChannellistGallery.getChildCount(); j++) {
					TextView chanelTextView = (TextView) mChannellistGallery.getChildAt(j);
					if (chanelTextView == arg1) {
						chanelTextView.setTextColor(ProgramActivity.this
								.getResources().getColor(R.color.solid_white));
						chanelTextView.setBackgroundResource(R.drawable.channel_selected);
					} else {
						chanelTextView.setTextColor(ProgramActivity.this
								.getResources().getColor(R.color.solid_gray));
						chanelTextView.setBackgroundResource(R.drawable.channel_default);
					}
				}

			}
		});
		mProgramListView.setOnItemClickListener(new OnItemClickListener() {

			public void onItemClick(AdapterView<?> arg0, View arg1, int arg2,
					long arg3) {
				if (mChannelStatus == 0 && isForfree == 0 && freeStatus == 0
						&& MBBMSService.sCurrentMode == MBBMS_MODE) {
					showConfirmDialog(DIALOG_CONFIRM_SUB);

				} else {
					if (arg1.getId() == -1) {
						showConfirmDialog(DIALOG_CONFIRM_PROGRAM_EXPIRE);

					} else if (arg1.getId() == 1) {
						showConfirmDialog(DIALOG_CONFIRM_PROGRAM_FUTURE);
					} else {
						Intent intent = new Intent();
						intent.setAction("com.marvell.cmmb.VIEW_SCHEDULE_ACTION");
						intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
						LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
								"mSelectPosition is " + mSelectPosition, LogMask.APP_COMMON);
						intent.putExtra("SchedulePosition", mSelectPosition);
						startActivity(intent);
						ProgramActivity.this.finish();
					}
				}
			}
		});
	}

	@Override
	public void updateDate() {
		if (mChannelAdapter.getCount() != 0) {// check whether channel list is
			// empty
			mChannelStatus = mChannelAdapter.getChannelStatus(mSelectPosition);
			isForfree = mChannelAdapter.isFree(mSelectPosition);
			freeStatus = mChannelAdapter.getForFreeStatus(mSelectPosition);
//			mScheduleArrayList = mResolver.getProgramList(
//					mChannelAdapter.mChannelList.get(mSelectPosition)
//							.getChannelId(), mChannelAdapter.mChannelList.get(
//							mSelectPosition).getFrequecy(), mSelectedDate,
//					MBBMSService.sCurrentMode);
			try {
				mScheduleArrayList = getCurrentProgramList(mSelectedDate);
			} catch (ParseException e) {
				e.printStackTrace();
			}
			mProgramAdapter = new ProgramListAdapter(this, mScheduleArrayList,
					mDateButton.getText().toString(), mChannelStatus, isForfree, freeStatus);
			mProgramListView.setAdapter(mProgramAdapter);
			mPosition = Util.getPosition(mScheduleArrayList);
			mProgramListView.setSelection(mPosition);
		}
		for (int i = 0; i < mChannelAdapter.mChannelList.size(); i++) {
			if (i == mSelectPosition)
				mChannelAdapter.mChannelMap.put(i, true);
			else
				mChannelAdapter.mChannelMap.put(i, false);
		}
	}

	@Override
	public void updateViews() {
		mDateTextView.setText(mSelectedDate);
		mChannellistGallery.setAdapter(mChannelAdapter);
		mChannellistGallery.setSelection(mSelectPosition);
	}
	
	private ArrayList<ProgramItem> getCurrentProgramList(String selectDate) throws ParseException{
		ArrayList <ProgramItem> programList = mResolver.getProgramList(mChannelAdapter.mChannelList.get(mSelectPosition)
				.getChannelId(), mChannelAdapter.mChannelList.get(mSelectPosition).getFrequecy(), MBBMSService.sCurrentMode);
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "programList size is "
				+ programList.size(), LogMask.APP_COMMON);
		ArrayList <ProgramItem> resultProgramList = new ArrayList<ProgramItem>();
		String selectStartTime = selectDate + " 00:00:00";
		String selectEndTime = selectDate + " 23:59:59";
		for(ProgramItem item : programList) {
			Date startDate = Util.sDateTimeFormater.parse(item.getStartTime());
			Date endDate;
			if(MBBMSService.sCurrentMode == MBBMSService.MBBMS_MODE) {
				endDate = Util.sDateTimeFormater.parse(item.getEndTime());
			} else {
				endDate = Util.sDateTimeFormater.parse(Util.sDateTimeFormater.format(Util.sDateTimeFormater
						.parse(item.getStartTime()).getTime() + item.getDuration() * 1000));
			}
			Date currentStartDate = Util.sDateTimeFormater.parse(selectStartTime);
			Date currentEndDate = Util.sDateTimeFormater.parse(selectEndTime);
			if((startDate.before(currentEndDate)) && (endDate.after(currentStartDate))) {
				resultProgramList.add(item);
			}
		}
		programList.clear();
		return resultProgramList;
	}

}
