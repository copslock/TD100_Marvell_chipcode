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

package com.marvell.cmmb.common;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;

import android.app.Dialog;
import android.content.Context;
import android.view.WindowManager;

import com.marvell.cmmb.R;
import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.resolver.ChannelItem;
import com.marvell.cmmb.resolver.ProgramItem;
import com.marvell.cmmb.resolver.ServiceParamItem;
import com.marvell.cmmb.resolver.MBBMSResolver;
import com.marvell.cmmb.service.MBBMSService;
import com.marvell.cmmb.resolver.ProgramItem;
public class Util implements Define {
	private static final String TAG = "Util";

	public static final SimpleDateFormat sDateTimeFormater = new SimpleDateFormat(
			"yyyy-MM-dd HH:mm:ss");

	public static final SimpleDateFormat sDateFormater = new SimpleDateFormat(
			"yyyy-MM-dd");

	/**
	 * compare the play back time with the current time
	 * 
	 * @param chooseDate
	 *            :play back date
	 * @param beginTime
	 *            :start playing time
	 * @param endTime
	 *            :end playing time
	 * @return -1,0,1
	 * @throws ParseException
	 */
	public static int compareTime(String beginTime, String endTime)
			throws ParseException {
		Date currentDate = new Date();
		Date beginDate = sDateTimeFormater.parse(beginTime);
		Date endDate = sDateTimeFormater.parse(endTime);
		long startTime = beginDate.getTime();
		long finishTime = endDate.getTime();
		long currentTimeMillis = currentDate.getTime();
		if (finishTime < currentTimeMillis) {
			return -1;
		} else if (startTime > currentTimeMillis) {
			return 1;
		} else {
			return 0;
		}
	}

	public static ArrayList<ChannelItem> getChannelList(int mode,
			Context context) {
		ArrayList<ChannelItem> channelList = MBBMSResolver.getInstance(context)
				.getChannelList(mode);
		ArrayList<ChannelItem> serviceList = new ArrayList<ChannelItem>();
		ArrayList<ServiceParamItem> serviceparamList = MBBMSResolver
				.getInstance(context).getServiceParam();
		long serviceId = -1;
		for (int i = 0; i < channelList.size(); i++) {
			boolean containService = false;
			if (mode == MBBMS_MODE) {
				long id = channelList.get(i).getChannelId();
				try {
					serviceId = Long.parseLong(MBBMSResolver.getInstance(
							context).getServiceId(id));
				} catch (Exception e) {
					e.printStackTrace();
					serviceId = 0;
				}
			} else {
				serviceId = channelList.get(i).getChannelId();
			}
			for (int j = 0; j < serviceparamList.size(); j++) {
				if (serviceId == serviceparamList.get(j).getServiceId()) {
					containService = true;
					break;
				}
			}
			if (containService) {
				serviceList.add(channelList.get(i));
			}
		}
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "channelList.size() is "
				+ channelList.size(), LogMask.APP_COMMON);
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "serviceparamList.size() is "
				+ serviceparamList.size(), LogMask.APP_COMMON);
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "serviceList.size() is "
				+ serviceList.size(), LogMask.APP_COMMON);
		serviceparamList = null;
		if (MBBMSService.sCurrentMode == MBBMS_MODE) {
			return channelList;
		} else {
			return serviceList;
		}
	}

	/**
	 * @param startTime
	 *            : the start time of a program
	 * @param duration
	 *            : the program's duration
	 * @return the end time of the program
	 * @throws ParseException
	 */
	public static String getCmmbEndTime(String startTime, int duration)
			throws ParseException {
		long endTimeMillis = sDateTimeFormater.parse(startTime).getTime()
				+ duration * 1000;
		return sDateTimeFormater.format(endTimeMillis);
	}

	/**
	 * @param startTime
	 *            : the start time of a program
	 * @param endTime
	 *            : the end time of a program
	 * @param duration
	 *            : the program's duration
	 * @return: the playback time of the program
	 * @throws ParseException
	 */
	public static String getPlaybackTime(String startTime, String endTime,
			int duration) throws ParseException {
		String cmmbEndTime;
		String playbackTime = "";
		try {
			if (MBBMSService.sCurrentMode == MBBMS_MODE) {
				playbackTime = startTime.substring(11, 16) + "-"
						+ endTime.substring(11, 16);
			} else {
				if (duration == 0) {
					playbackTime = startTime.substring(11, 16);
				} else {
					cmmbEndTime = sDateTimeFormater.format(sDateTimeFormater
							.parse(startTime).getTime()
							+ duration * 1000);
					playbackTime = startTime.substring(11, 16) + "-"
							+ cmmbEndTime.substring(11, 16);
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return playbackTime;
	}

	/**
	 * @param programList
	 *            : the program list for schedule or search
	 * @return the position of program which is playing
	 */
	public static int getPosition(ArrayList<ProgramItem> programList) {
		int position = 0;
		for (int i = 0; i < programList.size(); i++) {
			int flag = -2;
			try {
				if (MBBMSService.sCurrentMode == MBBMS_MODE) {
					flag = compareTime(programList.get(i).getStartTime(),
							programList.get(i).getEndTime());
				} else {
					flag = compareTime(programList.get(i).getStartTime(),
							getCmmbEndTime(programList.get(i).getStartTime(),
									programList.get(i).getDuration()));
				}
			} catch (ParseException e) {
				e.printStackTrace();
			}
			if (flag == 0) {
				position = i;
			}
		}
		return position;
	}

	public static void setProperty(Dialog dialog) {
		WindowManager.LayoutParams wl = dialog.getWindow().getAttributes();
		wl.dimAmount = 0.001f;
		dialog.getWindow().setAttributes(wl);
	}

	public static void addItem(HashMap<String, Integer> mMap, String key,
			int value) {
		mMap.put(key, value);
	}

	public static synchronized void setItem(HashMap<String, Integer> mMap,
			String key, int value) {
		if (mMap.get(key) != null) {
			mMap.put(key, value);
		}
	}

	public static synchronized boolean isItemSetValue(
			HashMap<String, Integer> mMap, String key, int expectValue) {
		if (mMap.get(key) != null) {
			int value = mMap.get(key);
			if (value == expectValue) {
				return true;
			}
		}
		return false;
	}

	public static void clearItems(HashMap<String, Integer> mMap) {
		mMap.clear();
	}
	
	public static String getErrorCodeInfo(Context context, int resCode) {
		String errorCode = "";
		switch(resCode) {
		case DEVICE_AUTHENTICATION_FAILED:
			errorCode = context.getResources().getString(R.string.device_authentication_failed);
			break;
		case PURCHASE_ITEM_UNKNOWN:
			errorCode = context.getResources().getString(R.string.purchaseitem_unknown);
			break;
		case SERVER_ERROR:
			errorCode = context.getResources().getString(R.string.server_error);
			break;
		case MAL_FORMED_MESSAGE_ERROR:
			errorCode = context.getResources().getString(R.string.mal_formed_message_error);
			break;
		case NO_SUBSCRIPTION:
			errorCode = context.getResources().getString(R.string.no_subscription);
			break;
		case OPERATION_NOT_PERMITTED:
			errorCode = context.getResources().getString(R.string.operation_not_permitted);
			break;
		case REQUESTED_SERVICE_UNAVALIABLE:
			errorCode = context.getResources().getString(R.string.requested_service_unavaliable);
			break;
		case REQUESTED_SERVICE_ACCEPTED:
			errorCode = context.getResources().getString(R.string.requested_service_accepted);
			break;
		case INVALID_REQUEST:
			errorCode = context.getResources().getString(R.string.invalid_request);
			break;
		case DELIVERY_WRONG_KEY_INFO:
			errorCode = context.getResources().getString(R.string.delivery_wrong_key_info);
			break;
		case ALREADY_IN_USE:
			errorCode = context.getResources().getString(R.string.already_in_use);
			break;
		case NO_MATCH_FRAGMENT:
			errorCode = context.getResources().getString(R.string.no_match_fragment);
			break;
		case NOW_SUBSCRIBED:
			errorCode = context.getResources().getString(R.string.now_subscribed);
			break;
		case INSUFFICIENT_CHARGE:
			errorCode = context.getResources().getString(R.string.insuffcient_charge);
			break;
		default:
			break;
		}
		return errorCode;
	}
	
	public static void filterProgramByDate(String selectDate, ArrayList <ProgramItem> srcList,ArrayList <ProgramItem> destList)
	throws ParseException {		
		String selectStartTime = selectDate + " 00:00:00";
		String selectEndTime = selectDate + " 23:59:59";
		for(ProgramItem item : srcList) {
			Date startDate = sDateTimeFormater.parse(item.getStartTime());
			Date endDate;
			if(MBBMSService.sCurrentMode == MBBMSService.MBBMS_MODE) {
				endDate = sDateTimeFormater.parse(item.getEndTime());
			} else {
				endDate = sDateTimeFormater.parse(sDateTimeFormater.format(sDateTimeFormater
						.parse(item.getStartTime()).getTime() + item.getDuration() * 1000));
			}
			Date currentStartDate = sDateTimeFormater.parse(selectStartTime);
			Date currentEndDate = sDateTimeFormater.parse(selectEndTime);
			if((startDate.before(currentEndDate)) && (endDate.after(currentStartDate))) {
				destList.add(item);
			}
			
		}
	}
	
}
