
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
package com.marvell.fmradio.list;
import android.app.Activity;
import android.os.Bundle;

import android.content.Intent;
import android.text.TextUtils;
import android.util.Log;

import android.widget.EditText;
import android.widget.TextView;
import android.widget.Button;

import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import com.marvell.fmradio.MainActivity;
import com.marvell.fmradio.R;
import com.marvell.fmradio.util.ChannelHolder;
import com.marvell.fmradio.util.LogUtil;
import com.marvell.fmradio.util.Station;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

public class EditStationView extends Activity 
{

	final static String TAG = "EditStationView";

	private ChannelHolder mChannelFile = null;
	private TextView mFreq = null;
	private EditText mEditName = null;
	private Button mBtnOK;
	private Button mBtnCl;
	private final static String INDEX_POS = "index_pos";
	private final static String INDEX_FREQ = "index_freq";
	private static int mPos = 0;
	
	
	public static void startActivity(Activity parent, int reqCode, int pos, String freq)
	{
		Intent intent = new Intent(parent, EditStationView.class);
		intent.putExtra(INDEX_POS, pos);
		intent.putExtra(INDEX_FREQ, freq);
		parent.startActivityForResult(intent, reqCode);
	}
	
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.add_view_station); 

		mChannelFile = ChannelHolder.getInstance();

		mFreq = (TextView) findViewById(R.id.station_freq);
		mEditName = (EditText)findViewById(R.id.edit_text_name);
		
		mPos = getIntent().getExtras().getInt(INDEX_POS);
		mFreq.setText(getIntent().getExtras().getString(INDEX_FREQ));
		
        mBtnOK = (Button) findViewById(R.id.button_ok);
        mBtnOK.setOnClickListener(mListener);        
		
		mBtnCl = (Button) findViewById(R.id.button_cancel);
		mBtnCl.setOnClickListener(mListener);
	}

	
	public void onDestroy()
	{
		super.onDestroy();
	}

    private OnClickListener mListener = new OnClickListener()
    {
        public void onClick(View v)
        {
			switch (v.getId())
			{
			case R.id.button_ok:
				save();
				break;
				
			case R.id.button_cancel:
				finish();
				break;
			}
        }
    };
    
	public void save()
	{
		String freq = mFreq.getText().toString();
		String name = mEditName.getText().toString();
                Pattern p = Pattern.compile("\t|\r|\n");
                Matcher m = p.matcher(name);
                name = m.replaceAll("");
		
		if (TextUtils.isEmpty(name))
		{
			LogUtil.d(TAG, "Station name cannot be empty!");
			return;
		}
		
		if (mPos == -1)
			mPos = mChannelFile.add(new Station(freq, name));
		else{
			mPos = mChannelFile.edit(mPos, new Station(freq, name));
		}
		
		Intent intent = new Intent();
		intent.putExtra(MainActivity.WORK_INDEX, mPos);
		setResult(RESULT_OK, intent);
		finish();		
	}
}
