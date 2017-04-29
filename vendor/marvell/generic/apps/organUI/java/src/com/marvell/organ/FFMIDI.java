/*
 * * (C) Copyright 2010 Marvell International Ltd.
 * * All Rights Reserved
 * *
 * * MARVELL CONFIDENTIAL
 * * Copyright 2008 ~ 2010 Marvell International Ltd All Rights Reserved.
 * * The source code contained or described herein and all documents related to
 * * the source code ("Material") are owned by Marvell International Ltd or its
 * * suppliers or licensors. Title to the Material remains with Marvell International Ltd
 * * or its suppliers and licensors. The Material contains trade secrets and
 * * proprietary and confidential information of Marvell or its suppliers and
 * * licensors. The Material is protected by worldwide copyright and trade secret
 * * laws and treaty provisions. No part of the Material may be used, copied,
 * * reproduced, modified, published, uploaded, posted, transmitted, distributed,
 * * or disclosed in any way without Marvell's prior express written permission.
 * *
 * * No license under any patent, copyright, trade secret or other intellectual
 * * property right is granted to or conferred upon you by disclosure or delivery
 * * of the Materials, either expressly, by implication, inducement, estoppel or
 * * otherwise. Any license under such intellectual property rights must be
 * * express and approved by Marvell in writing.
 * *
 * */

package com.marvell.organ;

import android.os.Bundle;
import android.widget.EditText;
import android.widget.Toast;

public class FFMIDI extends FormatActivity {
	/**
	 * @see android.app.Activity#onCreate(Bundle)
	 */
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		FormatActivity.FormatType=4;
		if("MUSIC".equals(FormatActivity.stream_type_flag))
		{
			this.setContentView(R.layout.playback_index_0_15);
		}
		else{
			this.setContentView(R.layout.playback_index_0_7);
		}
			this.show();
	}

	@Override
	public void save() {
		if("MUSIC".equals(FormatActivity.stream_type_flag))
		{
			short[] step=new short[16];
			for(int flag=0;flag<=15;flag++)
			{
				EditText et =(EditText)this.findViewById(editID[flag]);
				if(false == FormatActivity.match(et.getText().toString()))
				{
					Toast.makeText(this, "Please input integer between 1 and 120 to Step"+flag, Toast.LENGTH_LONG).show();
					return;
				}
				step[flag]=Short.parseShort(et.getText().toString());
			}
			NVMFileTool.write(ParamType,DeviceType,StreamType,FormatType,step);
		}
		else
		{
			short step[]=new short[8];
			for(int flag=0;flag<=7;flag++)
			{
				EditText et =(EditText)this.findViewById(editID[flag]);
				if(false == FormatActivity.match(et.getText().toString()))
				{
					Toast.makeText(this, "Please input integer between 1 and 120 to Step"+flag, Toast.LENGTH_LONG).show();
					return;
				}
				step[flag]=Short.parseShort(et.getText().toString());	
			}
			NVMFileTool.write(ParamType,DeviceType,StreamType,FormatType,step);
		}
	}
}
