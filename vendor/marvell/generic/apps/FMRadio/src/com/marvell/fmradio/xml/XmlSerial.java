
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
import java.io.File;

import java.io.FileOutputStream;
import java.io.IOException;

import org.xmlpull.v1.XmlSerializer;
import android.util.Xml;

import com.marvell.fmradio.util.LogUtil;
import com.marvell.fmradio.util.Station;
import com.marvell.fmradio.util.ChannelHolder;


public class XmlSerial
{
	private static final String TAG = "CmdXmlSerializer";
	private XmlSerializer mXmlSerializer = null;	

	public XmlSerial()
	{
		mXmlSerializer = Xml.newSerializer();
	}

	
	public void flush(ChannelHolder holder)
	{
		try
		{
			File newxmlfile = new File(XmlConst.FILE_DIR + XmlConst.FILE_NAME);
			newxmlfile.createNewFile();
			FileOutputStream fileos = new FileOutputStream(newxmlfile);

			mXmlSerializer.setOutput(fileos, "UTF-8");
			mXmlSerializer.startDocument(null, null);

			mXmlSerializer.setFeature("http://xmlpull.org/v1/doc/features.html#indent-output", true);
			mXmlSerializer.startTag(null, XmlConst.TAG_ROOT);
			mXmlSerializer.startTag(null, XmlConst.TAG_LIST);

			for (int i = 0; i < holder.mStationList.size(); i++)
			{
				Station item = holder.mStationList.get(i);
				mXmlSerializer.startTag(null, XmlConst.TAG_STATION);
				mXmlSerializer.attribute(null, XmlConst.ATTR_FREQ, item.mFreq);
				mXmlSerializer.attribute(null, XmlConst.ATTR_NAME, item.mName);
				mXmlSerializer.endTag(null, XmlConst.TAG_STATION);
			}

			mXmlSerializer.endTag(null, XmlConst.TAG_LIST);
			
			mXmlSerializer.startTag(null, XmlConst.TAG_WORK);
			mXmlSerializer.attribute(null, XmlConst.ATTR_FREQ, String.valueOf(holder.mWorkFreq));
			mXmlSerializer.endTag(null, XmlConst.TAG_WORK);	
			
			mXmlSerializer.endTag(null, XmlConst.TAG_ROOT);
			mXmlSerializer.endDocument();

			mXmlSerializer.flush();
			fileos.close();
		}
		catch (IOException e)
		{
			LogUtil.e(TAG, "Fail to save data to " + XmlConst.FILE_DIR + XmlConst.FILE_NAME);
			e.printStackTrace();
		}
		catch (IllegalArgumentException e1)
		{
			LogUtil.e(TAG, "Fail to save data to " + XmlConst.FILE_DIR + XmlConst.FILE_NAME + "due to illegal character.");
			e1.printStackTrace();
		}

	}	
}
