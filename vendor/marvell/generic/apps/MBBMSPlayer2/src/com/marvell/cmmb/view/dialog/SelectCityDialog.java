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

package com.marvell.cmmb.view.dialog;

import java.util.HashMap;

import android.app.AlertDialog;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckedTextView;
import android.widget.ListView;
import android.widget.AdapterView.OnItemClickListener;

import com.marvell.cmmb.R;
import com.marvell.cmmb.common.Define;

public class SelectCityDialog extends AlertDialog implements OnClickListener,
		OnItemClickListener {

	public interface SetScanCity {
		public void sendScanCity(String scanCity);
	}

	private Button mCancelButton;

	private ListView mCityLists;

	private SharedPreferences setting;

	private SetScanCity mSetScanCity;

	private String[] cityLists;

	private static final int[] freqList = new int[] { 20, 43, 35, 20, 32, 23,
			28, 31, 24, 19, 32, 13, 36, 30, 31, 16, 29, 20, 31, 21, 16, 25, 18,
			38, 39, 37, 17, 39, 33, 15

	};

	public static HashMap<String, Integer> freqMap = new HashMap<String, Integer>();

	public SelectCityDialog(Context context) {
		super(context);
	}

	private void focusSelectedCity(String scanCity) {
		int num = cityLists.length;
		for (int i = 0; i < num; i++) {
			String temp = mCityLists.getItemAtPosition(i).toString();
			if (temp.equals(scanCity)) {
				mCityLists.setItemChecked(i, true);
				mCityLists.setSelection(i);
				break;
			}
		}
	}

	public SetScanCity getmSetScanCity() {
		return mSetScanCity;
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.lv_citylists:
			this.dismiss();
			break;

		case R.id.bt_cancel:
			this.dismiss();
			break;
		}

	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.select_city_layout);
		setting = this.getContext().getSharedPreferences(Define.CMMBSETING, 
				Context.MODE_PRIVATE);
		cityLists = this.getContext().getResources().getStringArray(R.array.cities);
		setupView();
		setFreqMap();
	}

	public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
		String scanCity = ((CheckedTextView) view).getText().toString();
		SharedPreferences cmmbSetting = this.getContext().getSharedPreferences(
				Define.CMMBSETING, Context.MODE_PRIVATE);
		SharedPreferences.Editor cmmbEditor = cmmbSetting.edit();
		cmmbEditor.putString("scanCity", scanCity);
		cmmbEditor.putInt("minFreq", freqMap.get(scanCity));
		cmmbEditor.putInt("maxFreq", freqMap.get(scanCity));
		cmmbEditor.commit();
		this.dismiss();
		if (null != mSetScanCity) {
			mSetScanCity.sendScanCity(scanCity);
		}
	}

	@Override
	protected void onStart() {
		super.onStart();
		String scanCity = setting.getString("scanCity", this.getContext()
				.getResources().getString(R.string.defaultcity));
		focusSelectedCity(scanCity);

	}

	private void setFreqMap() {
		for (int i = 0; i < cityLists.length; i++) {
			freqMap.put(cityLists[i], freqList[i]);
		}

	}

	public void setmSetScanCity(SetScanCity mSetScanCity) {
		this.mSetScanCity = mSetScanCity;
	}

	/**
	 * Initialize the view of confirm dialog
	 */
	private void setupView() {
		mCancelButton = (Button) findViewById(R.id.bt_cancel);
		mCancelButton.setOnClickListener(this);
		mCityLists = (ListView) findViewById(R.id.lv_citylists);
		mCityLists.setAdapter(new ArrayAdapter<String>(this.getContext(),
				R.layout.radiobutton_layout, cityLists));
		mCityLists.setOnItemClickListener(this);
		mCityLists.setChoiceMode(ListView.CHOICE_MODE_SINGLE);
		mCityLists.setItemsCanFocus(false);
	}

}
