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

import java.util.ArrayList;
import java.util.Date;
import java.util.LinkedList;
import java.util.Queue;

import android.content.Context;

import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.aidl.MBBMSEngine;
import com.marvell.cmmb.entity.EsgArg;
import com.marvell.cmmb.entity.FreTsArg;
import com.marvell.cmmb.entity.InitArg;
import com.marvell.cmmb.entity.ScanArg;
import com.marvell.cmmb.entity.SgParserArg;
import com.marvell.cmmb.entity.SubArg;
import com.marvell.cmmb.parser.ESGParser;
import com.marvell.cmmb.parser.SGParser;
import com.marvell.cmmb.resolver.ChannelItem;
import com.marvell.cmmb.resolver.MBBMSResolver;
import com.marvell.cmmb.resolver.PurchaseInfoItem;
import com.marvell.cmmb.service.MBBMSService;

/**
 * 
 * message will be Process
 * 
 * 
 * @author Danyang Zhou
 * @version [2010-5-14]
 */
public class ProcessMsg {
	public static final String TAG = "ProcessMsg";

	Queue<Msg> msgQueue = new LinkedList<Msg>();

	MsgThread pMsgThread;

	private static ProcessMsg processMsg;

	private int currentEvent;

	private Context mContext;

	private ArrayList<PurchaseInfoItem> mInquiryList;

	public synchronized static ProcessMsg getInstance(Context context) {
		if (null == processMsg) {
			processMsg = new ProcessMsg(context);
		}
		return processMsg;
	}

	// constructor of ProcessMsg
	private ProcessMsg(Context context) {
		mContext = context;
	}

	public void toStart() {
		pMsgThread = new MsgThread();
		pMsgThread.startTread();
	}

	public void toStop() {
		clear();
		cancelEvent(currentEvent);
		pMsgThread.stopTread();
		pMsgThread = null;
	}

	private boolean hasMoreAcquire() {
		synchronized (msgQueue) {
			return !msgQueue.isEmpty();
		}
	}

	/**
	 * add msg to queue
	 */
	public void addMsg(Msg msg) {
		synchronized (msgQueue) {
			msgQueue.add(msg);
		}
	}

	public Msg poll() {
		synchronized (msgQueue) {
			return msgQueue.poll();
		}
	}

	public int getSize() {
		synchronized (msgQueue) {
			return msgQueue.size();
		}
	}

	public void clear() {
		synchronized (msgQueue) {
			msgQueue.clear();
		}
	}

	public void cancelEvent(int event) {
		switch (event) {
		case MsgEvent.EVENT_INIT:
		case MsgEvent.EVENT_DEINIT:
		case MsgEvent.EVENT_RESET:
		case MsgEvent.EVENT_PARSE_INQUIRY:
			break;
		case MsgEvent.EVENT_INQUIRY:
		case MsgEvent.EVENT_AUTHORIZESERVICE:
			MBBMSEngine.getInstance().cancelMBBMSRequest();
			break;
		case MsgEvent.EVENT_STARTSCAN:
			MBBMSEngine.getInstance().stopScan();
			break;
		case MsgEvent.EVENT_STARTSUBSCRIBE:
			MBBMSEngine.getInstance().cancelMBBMSRequest();
			break;
		case MsgEvent.EVENT_STARTUPDATEESG:
			MBBMSEngine.getInstance().stopUpdateESG();
			break;
		case MsgEvent.EVENT_STARTUPDATESG:
			MBBMSEngine.getInstance().cancelMBBMSRequest();
			break;
		case MsgEvent.EVENT_STARTUPDATESUBSCRIPTIONTABLE:
			MBBMSEngine.getInstance().cancelMBBMSRequest();
			break;
		case MsgEvent.EVENT_UNSUBSCRIBE:
			MBBMSEngine.getInstance().cancelMBBMSRequest();
			break;
		case MsgEvent.EVENT_PARSE_SG:
		case MsgEvent.EVENT_PARSE_ST:
			SGParser.getInstance(mContext).stopParseSG();
			break;
		case MsgEvent.EVENT_PARSE_ESG:
			ESGParser.getInstance(mContext).stopParseESG();
			break;
		case MsgEvent.EVENT_INITGBA:
			MBBMSEngine.getInstance().cancelMBBMSRequest();
			break;
		}
	}

	private synchronized int dealMsg(Msg msg) {
		this.currentEvent = msg.getEvent();
		int rt = 0;
		switch (msg.getEvent()) {
		case MsgEvent.EVENT_INIT:
			InitArg mc = (InitArg) msg.getData();
			rt = MBBMSEngine.getInstance().init(mc.mCB, mc.dtPath, mc.mAgent);
			break;
		case MsgEvent.EVENT_RESET:
			rt = MBBMSEngine.getInstance().reset();
			break;
		case MsgEvent.EVENT_AUTHORIZESERVICE:
			String sId = (String) msg.getData();
			if (sId.equals("all")) {
				ArrayList<ChannelItem> list = MBBMSResolver.getInstance(
						mContext).getChannelList(Define.MBBMS_MODE);
				int count = list.size();
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "authorize size = "
						+ count, LogMask.APP_COMMON);
				int freq[] = new int[count];
				int serviceId[] = new int[count];
				String refServiceId[] = new String[count];
				for (int i = 0; i < count; i++) {
					freq[i] = list.get(i).getFrequecy();
					serviceId[i] = Integer.parseInt(MBBMSResolver.getInstance(
							mContext).getServiceId(list.get(i).getChannelId()));
					refServiceId[i] = String.valueOf(list.get(i).getChannelId());
					LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "freq = "
							+ freq[i] + " serviceId = " + serviceId[i]
							+ " refServiceId = " + refServiceId[i],
							LogMask.APP_COMMON);
				}
				MBBMSEngine.getInstance().requestAuthorizeService(freq,
						serviceId, refServiceId, count);
			} else {
				int count = 1;
				int freq[] = new int[count];
				int serviceId[] = new int[count];
				String refServiceId[] = new String[count];
				long channelId = Long.parseLong(sId);
				serviceId[0] = Integer.parseInt(MBBMSResolver.getInstance(
						mContext).getServiceId(channelId));
				refServiceId[0] = sId;
				freq[0] = MBBMSResolver.getInstance(mContext).getFrequency(
						channelId);
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "freq = " + freq[0]
						+ " serviceId = " + serviceId[0]
						+ " refServiceId = " + refServiceId[0],
						LogMask.APP_COMMON);
				MBBMSEngine.getInstance().requestAuthorizeService(freq,
						serviceId, refServiceId, count);
			}
			break;
		case MsgEvent.EVENT_DEINIT:
			MBBMSEngine.getInstance().deinit();
			break;
		case MsgEvent.EVENT_STARTSCAN:
			ScanArg sa = (ScanArg) msg.getData();
			MBBMSEngine.getInstance().startScan(sa.freqs, sa.count);
			break;
		case MsgEvent.EVENT_STARTSUBSCRIBE:
			SubArg sba = (SubArg) msg.getData();
			MBBMSEngine.getInstance().requestSubscribeService(sba.purchaseID,
					sba.count);
			break;
		case MsgEvent.EVENT_STARTUPDATEESG:
			FreTsArg upesg = (FreTsArg) msg.getData();
			MBBMSEngine.getInstance().startUpdateESG(upesg.freq, upesg.startTs,
					upesg.countTs, upesg.mod);
			break;
		case MsgEvent.EVENT_STARTUPDATESG:
			String areaName = (String) msg.getData();
			MBBMSEngine.getInstance().requestUpdateSG(areaName);
			break;
		case MsgEvent.EVENT_STARTUPDATESUBSCRIPTIONTABLE:
			MBBMSEngine.getInstance().requestUpdateST();
			break;
		case MsgEvent.EVENT_UNSUBSCRIBE:
			SubArg usa = (SubArg) msg.getData();
			MBBMSEngine.getInstance().requestUnsubscribeService(usa.purchaseID,
					usa.count);
			break;
		case MsgEvent.EVENT_PARSE_SG:
			SgParserArg sgArg = (SgParserArg) msg.getData();
			LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
					"start parse sg, sgdd name is " + sgArg.sgddXml
							+ " sg name is " + sgArg.sgXml, LogMask.APP_MAIN);
			SGParser sgParser = SGParser.getInstance(mContext);
			sgParser.init();
			MBBMSService.sBSMSelectorId = sgParser.parseSGDD(sgArg.sgddXml);
			boolean sgResult = sgParser.startParseSG(sgArg.sgXml);
			LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
					"parse sg result is " + sgResult, LogMask.APP_MAIN);
			if (sgResult) {
				sgParser.updateSGDatabaseInfo();
			}
			break;
		case MsgEvent.EVENT_INITGBA:
			if (MBBMSService.isTestMode) {
				MBBMSEngine.getInstance().requestInitGBA(1);
			} else {
				MBBMSEngine.getInstance().requestInitGBA(0);
			}
			break;
		case MsgEvent.EVENT_INQUIRY:
			MBBMSEngine.getInstance().requestInquiryAccount();
			break;
		case MsgEvent.EVENT_PARSE_INQUIRY:
			SGParser inquiryParser = SGParser.getInstance(mContext);
			mInquiryList = inquiryParser.parseInquiry(msg.getData().toString());
			break;
		case MsgEvent.EVENT_PARSE_ST:
			LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
					"start parse st, st name is " + msg.getData().toString(),
					LogMask.APP_MAIN);
			SGParser stParser = SGParser.getInstance(mContext);
			boolean stResult = stParser.startParseST(msg.getData().toString());
			if (stResult) {
				stParser.updateSTDatabaseInfo();
			}
			break;
		case MsgEvent.EVENT_PARSE_ESG:
			ESGParser esgParser = ESGParser.getInstance(mContext);
			EsgArg esg = (EsgArg) msg.getData();
			LogMask.LOGM(TAG, LogMask.LOG_APP_PARSER, LogMask.getLineInfo(),
					"start parse esg, esg name is " + esg.fname
							+ " esg type is " + esg.type, LogMask.APP_PARSER);
			switch (esg.type) {
			case 1:
				esgParser.startParseService(esg.fname, esg.freq);
				break;
			case 2:
				esgParser.startParseServiceAux(esg.fname);
				break;
			case 3:
				esgParser.startParseSchedule(esg.fname, esg.freq);
				break;
			case 4:
				esgParser.startParseContent(esg.fname);
				break;
			}
			break;
		}
		return rt;
	}

	public ArrayList<PurchaseInfoItem> getInquiryList() {
		return mInquiryList;
	}

	public int getCurrentEvent() {
		return currentEvent;
	}

	public void setCurrentEvent(int currentEvent) {
		this.currentEvent = currentEvent;
	}

	/**
	 * 
	 * inner class Process msg
	 * 
	 * @author DanyangZhou
	 * @version [2010-5-14]
	 */
	private class MsgThread extends Thread {
		boolean loop;

		public void startTread() {
			loop = true;
			this.start();
		}

		public void stopTread() {
			loop = false;
			try {
				this.join();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}

		public void run() {
			while (loop) {
				if (hasMoreAcquire()) {
					Msg msg = null;
					synchronized (ProcessMsg.this) {
						msg = poll();
					}

					if (null != msg) {
						int rt = 0;
						try {
							rt = dealMsg(msg);
						} catch (Exception e) {
							e.printStackTrace();
						} finally {
							Date date = new Date();
							String time = date.toLocaleString();
							LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
									"current event is " + msg.getEvent()
											+ " current time is " + time,
									LogMask.APP_MAIN);
							if (loop) {
								int cookie = 0;
								if (msg.getEvent() == MsgEvent.EVENT_PARSE_ESG) {
									cookie = ((EsgArg) msg.getData()).cookie;
								}
								if (msg.getEvent() == MsgEvent.EVENT_INIT) {
									cookie = rt;
								}
								if (null != msg.getHandler()) {
									msg.getHandler().onEventFinished(
											msg.getEvent(), cookie);
								}
							}
						}
					}
				} else {
					try {
						Thread.sleep(50);
					} catch (Exception err) {
						LogMask.LOGM(TAG, LogMask.LOG_APP_ERROR, LogMask.getLineInfo(), err
								.getMessage(), LogMask.APP_MAIN);
					}
				}
			}
		}
	}
}
