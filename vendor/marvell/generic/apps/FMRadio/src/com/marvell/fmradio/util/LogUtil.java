
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
package com.marvell.fmradio.util;
import android.util.Log;



public class LogUtil
{
	public static final String APP_TAG = "FMRadioApp";
	public static boolean mDebug = true;



	private LogUtil()
	{

	}



	public static void d(String tag, String msg)
	{
		if (mDebug)
			Log.d(APP_TAG, "[" + tag + "]" + msg);
	}



	public static void i(String tag, String msg)
	{
		if (mDebug)
			Log.i(APP_TAG, "[" + tag + "]" + msg);
	}



	public static void e(String tag, String msg)
	{
		if (mDebug)
			Log.e(APP_TAG, "[" + tag + "]" + msg);
	}



	public static void w(String tag, String msg)
	{
		if (mDebug)
			Log.w(APP_TAG, "[" + tag + "]" + msg);
	}



	public static void e(String tag, String msg, Throwable tr)
	{
		if (mDebug)
			Log.e(APP_TAG, "[" + tag + "]" + msg, tr);
	}

}