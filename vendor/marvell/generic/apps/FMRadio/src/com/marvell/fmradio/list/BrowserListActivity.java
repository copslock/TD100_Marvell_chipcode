
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
import android.view.Window;
import android.util.Log;
import android.view.View;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;

import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ListView;


import com.marvell.fmradio.MainActivity;
import com.marvell.fmradio.R;
import com.marvell.fmradio.util.ChannelHolder;
import com.marvell.fmradio.util.LogUtil;
import com.marvell.fmradio.util.Station;


public class BrowserListActivity extends Activity implements OnItemClickListener
{
	private final static String TAG = "BrowserListActivity";
	private final static int REQ_DELETE = 0;
	private final static int REQ_BROWSER = 1;
	
	private ListView mListView = null;
	private ChannelHolder mChannelHolder = null;
	private BrowserListAdapter mListAdapter = null;
	private RemoveListAdapter mRemoveAdapter = null;
	
	public static void startActivity(Activity parent, int reqCode)
	{
		Intent intent = new Intent(parent, BrowserListActivity.class);
		parent.startActivityForResult(intent, reqCode);
	}
	
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.browser_station_list);

		mChannelHolder = ChannelHolder.getInstance();
		mListAdapter = new BrowserListAdapter(this, mChannelHolder.getStationList());
		
       mRemoveAdapter = new RemoveListAdapter(this, R.layout.remove_station_item, mChannelHolder.getStationList());
		mListView = (ListView) findViewById(R.id.list_view_station);
		mListView.setAdapter(mListAdapter);
		mListView.setOnItemClickListener(this);		
	}

	public void onDestroy()
	{
		super.onDestroy();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu)
	{
    	super.onCreateOptionsMenu(menu);
    	MenuInflater inflater = getMenuInflater();
    	inflater.inflate(R.menu.menu, menu);
    	return true;
	}

	@Override
	public boolean onPrepareOptionsMenu(Menu menu)
	{
		return super.onPrepareOptionsMenu(menu);
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		switch (item.getItemId())
		{
		case R.id.menu_delete:
		    RemoveListActivity.startActivity(this, REQ_DELETE);
			break;

		case R.id.menu_rename:
			RenameListActivity.startActivity(this, REQ_BROWSER);
			break;
		}

		return super.onOptionsItemSelected(item);
	}

	
	
	public void onItemClick(AdapterView<?> parent, View v, int position, long id)
	{
		if (0 == position)
		{
			EditStationView.startActivity(this, REQ_BROWSER, -1, String.valueOf(mChannelHolder.mWorkFreq));
			return;
		}		

		Intent intent = new Intent();
		intent.putExtra(MainActivity.WORK_INDEX, position - 1);
		setResult(RESULT_OK, intent);
		finish();
	}


	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		switch (requestCode)
		{
		case REQ_DELETE:
		    mListAdapter.notifyDataSetChanged();
		   
		    setResult(RESULT_OK);
		    break;
		case REQ_BROWSER:			
			if (RESULT_OK == resultCode ){
				mListAdapter.notifyDataSetChanged();
			setResult(RESULT_OK, data);}
			break;			
		}
	}		

}
