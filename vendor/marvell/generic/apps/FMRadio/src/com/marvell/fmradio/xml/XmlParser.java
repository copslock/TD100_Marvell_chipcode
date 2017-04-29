
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
package com.marvell.fmradio.xml;
import java.io.FileInputStream;
import java.io.IOException;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParserFactory;

import android.text.TextUtils;

import com.marvell.fmradio.util.ChannelHolder;
import com.marvell.fmradio.util.Station;
import com.marvell.fmradio.util.LogUtil;


public class XmlParser
{
	private static final String TAG = "FMRadioXmlParser";
	private static final boolean mDebug = true;	

	private XmlPullParserFactory mXmlFactory = null;
	private XmlPullParser mXmlParser = null;

	public XmlParser()
	{
		try
		{
			mXmlFactory = XmlPullParserFactory.newInstance();
			mXmlFactory.setNamespaceAware(true);
			mXmlParser = mXmlFactory.newPullParser();
		}
		catch (XmlPullParserException e)
		{
			e.printStackTrace();
		}
	}

	public boolean parserXml(ChannelHolder holder) throws XmlPullParserException, IOException
	{
		String path = XmlConst.FILE_DIR + XmlConst.FILE_NAME;
		boolean mTagRoot = false;
		boolean mTagList = false;
		boolean mTagStation = false;
		boolean mTagWork = false;
		Station stat = null;
		int mWorkFreq = 0;
		
		LogUtil.d(TAG, "Parse file: " + path);

		if (null == mXmlFactory || null == mXmlParser)
		{
			return false;
		}

		mXmlParser.setInput(new FileInputStream(path), "UTF-8");
		String TagName = null;

		int eventType = mXmlParser.getEventType();
		while (true)
		{
			switch (eventType)
			{
			case XmlPullParser.START_DOCUMENT:
				if (mDebug)
					LogUtil.d(TAG, "START_DOCUMENT");
				
				break;

			case XmlPullParser.START_TAG:
				TagName = mXmlParser.getName();

				if (mDebug)
					LogUtil.d(TAG, "TagName: " + TagName);

				if (TagName.equals(XmlConst.TAG_ROOT))
				{
					mTagRoot = true;
					break;
				}

				if (TagName.equals(XmlConst.TAG_LIST))
				{
					mTagList = true;
					break;
				}

				if (TagName.equals(XmlConst.TAG_STATION))
				{					
					String freq = mXmlParser.getAttributeValue(null, XmlConst.ATTR_FREQ).trim();
					int number = Integer.valueOf(freq);
					
					if (TextUtils.isEmpty(freq) || (number < 76000 || 108000 < number))
					{
						stat = null;
						break;						
					}					
					
					mTagStation = true;
					stat = new Station();
					
					stat.mFreq = freq;
					stat.mName = mXmlParser.getAttributeValue(null, XmlConst.ATTR_NAME);
					if (mDebug)
						LogUtil.d(TAG, XmlConst.TAG_STATION + " freq : " + stat.mFreq + " name : " + stat.mName);
					
					break;
				}
				
				if (TagName.equals(XmlConst.TAG_WORK))
				{
					String freq = mXmlParser.getAttributeValue(null, XmlConst.ATTR_FREQ).trim();
					mWorkFreq = Integer.valueOf(freq);
					
					if (TextUtils.isEmpty(freq) || (mWorkFreq < 76000 || 108000 < mWorkFreq))
					{
						mWorkFreq = 0;
						break;
					}
					
					mTagWork = true;
					break;	
				}
				
				break;

			case XmlPullParser.TEXT:
				String TagContent = mXmlParser.getText().trim();

				if (mDebug)
					LogUtil.d(TAG, "TagContent: " + TagContent);				

				if (TextUtils.isEmpty(TagContent))
				{					
					break;
				}

				if (TagName.equals(XmlConst.TAG_STATION))
				{
					LogUtil.d(TAG, XmlConst.TAG_STATION);
					break;
				}

				break;

			case XmlPullParser.END_TAG:
				TagName = mXmlParser.getName();

				if (mDebug)
					LogUtil.d(TAG, "TagName: " + TagName);			

				if (TagName.equals(XmlConst.TAG_ROOT))
				{
					LogUtil.d(TAG, XmlConst.TAG_ROOT);
					break;
				}

				if (TagName.equals(XmlConst.TAG_LIST))
				{
					LogUtil.d(TAG, XmlConst.TAG_LIST);
					break;
				}

				if (TagName.equals(XmlConst.TAG_STATION) && null != stat)
				{
					int pos = ChannelHolder.BinarySearch(holder.mStationList, stat.mFreq);
					holder.mStationList.add(pos, stat);
					LogUtil.d(TAG, XmlConst.TAG_STATION);
					break;
				}

				if (TagName.equals(XmlConst.TAG_WORK))
				{
					holder.mWorkFreq = mWorkFreq;
					break;
				}
				break;

			case XmlPullParser.END_DOCUMENT:
				if (mDebug)
					LogUtil.d(TAG, "END_DOCUMENT");				
				
				if (! mTagRoot)
				{
					LogUtil.e(TAG, "Failed to find tag: " + XmlConst.TAG_ROOT);
					return false;
				}					

				if (! mTagList)
				{
					LogUtil.e(TAG, "Failed to find tag: " + XmlConst.TAG_LIST);
					return false;
				}					

				if (! mTagStation)
				{
					LogUtil.e(TAG, "Failed to find tag: " + XmlConst.TAG_STATION);
					return false;
				}					

				if (! mTagWork)
				{
					LogUtil.d(TAG, "Failed to find tag: " + XmlConst.TAG_WORK);
					LogUtil.d(TAG, "We ignore work freq");
				}
				
				return true;
			}

			eventType = mXmlParser.next();
		}

	}
}
