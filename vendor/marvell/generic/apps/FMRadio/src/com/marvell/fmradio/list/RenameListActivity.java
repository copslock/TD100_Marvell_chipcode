
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
import android.content.Intent;
import android.os.Bundle;

import android.util.Log;
import android.view.View;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;

import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ListView;

import android.view.Window;
import com.marvell.fmradio.MainActivity;
import com.marvell.fmradio.R;
import com.marvell.fmradio.util.ChannelHolder;
import com.marvell.fmradio.util.LogUtil;
import com.marvell.fmradio.util.Station;


public class RenameListActivity extends Activity implements OnItemClickListener
{
	private final static String TAG = "RenameListActivity";
	private final static int REQ_BROWSER = 1;
	
	private ListView mListView = null;
	private ChannelHolder mChannelHolder = null;
	private RenameListAdapter mListAdapter = null;

	
	public static void startActivity(Activity parent, int reqCode)
	{
		Intent intent = new Intent(parent, RenameListActivity.class);
		parent.startActivityForResult(intent, reqCode);
	}
	
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.browser_station_list);

		mChannelHolder = ChannelHolder.getInstance();
		mListAdapter = new RenameListAdapter(this, mChannelHolder.getStationList());

		mListView = (ListView) findViewById(R.id.list_view_station);
		mListView.setAdapter(mListAdapter);
		mListView.setOnItemClickListener(this);		
	}

	public void onDestroy()
	{
		super.onDestroy();
	}
	
	public void onItemClick(AdapterView<?> parent, View v, int position, long id)
	{
		EditStationView.startActivity(this, REQ_BROWSER, position, String.valueOf(mChannelHolder.getItem(position).mFreq));

		 setResult(RESULT_OK);
		 mChannelHolder.flush();
	}


	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data)
	{
	    
		switch (requestCode)
		{
		case REQ_BROWSER:			
			if (RESULT_OK == resultCode ){
				mListAdapter.notifyDataSetChanged();
				//int index = data.getExtras().getInt(MainActivity.WORK_INDEX);
				//mChannelHolder.delete(index);
				setResult(RESULT_OK, data);
			}
			
			break;			
		}
	}		

}
