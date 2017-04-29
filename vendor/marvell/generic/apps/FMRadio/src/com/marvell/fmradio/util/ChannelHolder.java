
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
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.xmlpull.v1.XmlPullParserException;

import android.text.TextUtils;
import android.util.Log;

import com.marvell.fmradio.util.LogUtil;
import com.marvell.fmradio.xml.XmlConst;
import com.marvell.fmradio.xml.XmlParser;
import com.marvell.fmradio.xml.XmlSerial;

public class ChannelHolder
{
	private static final String TAG = "ChannelHolder";
	private static final int WORK_FREQ = 87500;
	
	public List<Station> mStationList = null;
	
	public int mWorkFreq = 0;
        public int rssi ;	
	private XmlParser mXmlParser = null;
	private XmlSerial mXmlSerial = null;
	
	private static ChannelHolder mInstance = null;

	public static ChannelHolder getInstance()
	{
		if (null == mInstance)
			mInstance = new ChannelHolder();

		return mInstance;
	}
	
	private ChannelHolder()
	{
		mStationList = new ArrayList<Station>(); 
		mXmlSerial = new XmlSerial();
		mXmlParser = new XmlParser();
        
		File dir = new File(XmlConst.FILE_DIR);
		if (! dir.exists())
			dir.mkdir();			
		
		File file = new File(XmlConst.FILE_DIR + XmlConst.FILE_NAME);
		if (! file.exists())
		{
			mWorkFreq = WORK_FREQ;
			return;
		}

		try
		{
			if (! mXmlParser.parserXml(this))
				LogUtil.d(TAG, "Failed to parse file");
			else
				LogUtil.d(TAG, "channel count: " + mStationList.size());
			
			if (0 == mWorkFreq)
				mWorkFreq = WORK_FREQ;
		}
		catch (XmlPullParserException e)
		{
			e.printStackTrace();
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
	}



	public int add(Station item)
	{
		if (null == item || TextUtils.isEmpty(item.mFreq))
		{
			LogUtil.d(TAG, "Command name/content mustn't be empty!");
			return -1;
		}
		
		int pos = BinarySearch(mStationList, item.mFreq);
		if(pos<=mStationList.size()-1&&mStationList.get(pos).mFreq.equals(item.mFreq))
		delete(pos);
		mStationList.add(pos, item);
		mXmlSerial.flush(this);
		return pos;
	}



	public int edit(int index, Station item)
	{
		delete(index);
		return add(item);
	}



	public void delete(int index)
	{
		if (index < 0 || mStationList.size() <= index)
		{
			LogUtil.d(TAG, "Index " + index + "is invalid, it must between 0 and " + (mStationList.size() - 1));
			return;
		}
       
		mStationList.remove(index);
	}
	
	public void flush()
	{
		mXmlSerial.flush(this);
	}
	
	public void setWorkFreq(int value)
	{
		mWorkFreq = value;
	}
	
	public int getCount()
	{
		return mStationList.size();
	}

	
	public List<Station> getStationList()
	{
		return mStationList;
	}
	
	public Station getItem(int index)
	{
		if (index < 0 || mStationList.size() <= index)
		{
			LogUtil.d(TAG, "Index " + index + "is invalid, it must between 0 and " + (mStationList.size() - 1));
			return null;
		}
		
		LogUtil.d(TAG, "getItemFreq " + index);
		return mStationList.get(index);
	}
	
	public String getWorkFreqName()
	{
		String workFreq = String.valueOf(mWorkFreq);
		
		for (int i = 0; i < mStationList.size(); i++)
		{
			Station item = mStationList.get(i);
			
			if (item.mFreq.equals(workFreq))
				return item.mName;
		}
		
		return null;
	}
	
	
	public static int BinarySearch(List<Station> list, String freq)
	{
		if (0 == list.size())
			return 0;

		int left = 0;
		int right = list.size() - 1;
		Station item = list.get(0);
		
		if (compare(freq, item.mFreq) < 0)
		{
			return 0;
		}

		item = list.get(right);

		if (compare(freq, item.mFreq) > 0)
		{
			return list.size();
		}

		while (left <= right)
		{
			int middle = (left + right) / 2;
			item = list.get(middle);

			if (compare(freq, item.mFreq) > 0)
				left = middle + 1;
			else
				right = middle - 1;
		}		

		return left;
	}
	
	private static int compare(String value1, String value2)
	{
		int v1 = Integer.valueOf(value1);
		int v2 = Integer.valueOf(value2);
		
		return v1 - v2;
	}
}
