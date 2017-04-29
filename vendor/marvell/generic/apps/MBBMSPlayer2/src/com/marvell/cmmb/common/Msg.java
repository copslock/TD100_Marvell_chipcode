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

import java.util.Map;

import android.content.Context;

/**
 * 
 * set message format and content
 * 
 * @author DanyangZhou
 * @version [2010-5-14]
 */
public final class Msg {
	private int mEvent;

	private Object mData;

	private int arg1;

	private int arg2;

	private Map<String, String> mMap;

	private MsgEvent mHandler;

	private Context mContext;

	public Msg() {
		mEvent = 0;
		mMap = null;
		mHandler = null;
	}

	// get Action
	public int getEvent() {
		return mEvent;
	}

	// set Action
	public void setEvent(int action) {
		mEvent = action;
	}

	// get map
	public Map<String, String> getMap() {
		return mMap;
	}

	// set map
	public void setMap(Map<String, String> map) {
		mMap = map;
	}

	// get DataTransmit
	public MsgEvent getHandler() {
		return mHandler;
	}

	// set DataTransmit
	public void setHandler(MsgEvent handler) {
		mHandler = handler;
	}

	public Context getContext() {
		return mContext;
	}

	public void setContext(Context context) {
		mContext = context;
	}

	public Object getData() {
		return mData;
	}

	public void setData(Object data) {
		mData = data;
	}

	public Object getArg1() {
		return arg1;
	}

	public void setArg1(int arg1) {
		this.arg1 = arg1;
	}

	public Object getArg2() {
		return arg2;
	}

	public void setArg2(int arg2) {
		this.arg2 = arg2;
	}
}
