
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

import com.marvell.fmradio.MainActivity;
import com.marvell.fmradio.R;
import com.marvell.fmradio.list.RemoveListAdapter.FMHolder;
import com.marvell.fmradio.util.ChannelHolder;
import android.view.Window;
import android.util.Log;
import android.view.View;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View.OnClickListener;

import android.widget.Button;
import android.widget.ListView;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;




public class RemoveListActivity extends Activity implements OnItemClickListener
{
	private final static String TAG = "RemoveListActivity";
	private final static int REQ_BROWSER = 1;
	private ListView mListView = null;
	private RemoveListAdapter mRemoveAdapter = null;
	private Button mBtnOK;
	private Button mBtnCl;
	private ChannelHolder mChannelHoler = null;
	
	public static void startActivity(Activity parent, int reqCode)
	{
		Intent intent = new Intent(parent, RemoveListActivity.class);
		parent.startActivityForResult(intent, reqCode);
	}
	
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);		
		setContentView(R.layout.remove_list_station);

		mChannelHoler = ChannelHolder.getInstance();
		mRemoveAdapter = new RemoveListAdapter(this, R.layout.remove_station_item, mChannelHoler.getStationList());
		
		mListView = (ListView) findViewById(R.id.list_view_station);
		mListView.setAdapter(mRemoveAdapter);
		mListView.setOnItemClickListener(this);
		
		mBtnOK = (Button) findViewById(R.id.button_ok);
		mBtnOK.setOnClickListener(mListener);
		
		
		mBtnCl = (Button) findViewById(R.id.button_cancel);
		mBtnCl.setOnClickListener(mListener);
	}


	 private OnClickListener mListener = new OnClickListener()
	    {
	        public void onClick(View v)
	        {
				switch (v.getId())
				{
				case R.id.button_ok:
					removeStation();
					break;
					
				case R.id.button_cancel:
					finish();
					break;
					
					
				}
	        }
	    };
	    
	public void onDestroy()
	{
		super.onDestroy();
	}



	@Override
	public boolean onCreateOptionsMenu(Menu menu)
	{
    	super.onCreateOptionsMenu(menu);
    	MenuInflater inflater = getMenuInflater();
    	inflater.inflate(R.menu.remove_ui, menu);
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
		case R.id.menu_select_all:
			mRemoveAdapter.selectAll();
			break;

		case R.id.menu_cancel_all:
			mRemoveAdapter.cancelAll();
			break;
		}

		return super.onOptionsItemSelected(item);
	}

	private void removeStation()
	{
		for (int pos = mChannelHoler.getCount() - 1 ; 0 <= pos ; pos--) 
		{
			if (mRemoveAdapter.getChecked(pos)){
				mChannelHoler.delete(pos);
				
			}
		}
		
        setResult(RESULT_OK);
		finish();
		mChannelHoler.flush();
	}

	public void onItemClick(AdapterView<?> parent, View v, int position, long id)
	{
		if (position < 0 || position > mChannelHoler.getCount())
			return;

		mRemoveAdapter.setChecked(position);
		
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event)
	{
		switch (keyCode)
		{
		case KeyEvent.KEYCODE_BACK:
			finish();
			return true;

		default:
			break;

		}

		return super.onKeyDown(keyCode, event);
	}
	
}
