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

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.EditText;
import android.widget.Toast;
import android.util.Log;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

public abstract class FormatActivity extends Activity {
	private static final String TAG = "FormatActivity";
	public static String device_type_flag;
	public static String stream_type_flag;
	public static String format_type_flag;
	public static short ParamType;
	public static short DeviceType;
	public static short StreamType;
	public static short FormatType;
	public int[] editID={R.id.EditText01,R.id.EditText02,R.id.EditText03,
			R.id.EditText04,R.id.EditText05,R.id.EditText06,R.id.EditText07,R.id.EditText08,
			R.id.EditText09,R.id.EditText10,R.id.EditText11,R.id.EditText12,R.id.EditText13,
			R.id.EditText14,R.id.EditText15,R.id.EditText16,};
	public abstract void save();
	public static boolean match(String index){
		Pattern p = Pattern.compile("^\\d{1,3}$");
		Matcher m = p.matcher(index);
		return (m.matches()&&(Integer.parseInt(index)>=0&&Integer.parseInt(index)<=120)); 
	}
	public void show(){
		short step[]=NVMFileTool.read(ParamType,DeviceType,StreamType,FormatType);
		Log.i(TAG, "read success!"+"ParamType:"+ParamType+"DeviceType:"+DeviceType+"StreamType:"+StreamType+"FormatType:"+FormatType);
		if(null!=step)
		{
			for(int flag=0;flag<step.length;flag++)
			{
				EditText et =(EditText)this.findViewById(editID[flag]);
				et.setText(step[flag]+"");
				
			}
		}
		else
		 Toast.makeText(this, "Read failed !", Toast.LENGTH_LONG).show();
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.setTitle(device_type_flag+"-->"+(stream_type_flag!=null?(stream_type_flag+"-->"):"")+this.getTitle());
		//this.show();
	}
	static final private int MENU_SAVE = Menu.FIRST;
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);
		menu.add(0, MENU_SAVE, Menu.NONE, "Save");
		return true;
	}
  
	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		super.onOptionsItemSelected(item);
		switch (item.getItemId()) {
			case (MENU_SAVE): {
				this.save();
				return true;
			}
		}
		return false;
	}
}
