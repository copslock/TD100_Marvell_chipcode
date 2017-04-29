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

import android.app.Dialog;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.marvell.cmmb.R;
import com.marvell.cmmb.common.Define;

public class InputFreqDialog extends Dialog implements OnClickListener {
	private EditText mFreSpanEditText;

	private Button mCancelButton;

	private Button mConfirmButton;

	private Toast mInputVaToast;

	private Toast mInterToast;

	public InputFreqDialog(Context context, int theme) {
		super(context, theme);
	}

	private boolean isInputValueResonable(int minFreq, int maxFreq) {
		if ((minFreq >= 13 && minFreq < 48) && (maxFreq >= 13 && maxFreq <= 48)
				&& (minFreq <= maxFreq)) {
			return true;
		} else {
			return false;
		}
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.bt_confirm:
			try {
				String input = mFreSpanEditText.getText().toString();
				String[] freqSpan = new String[2];
				freqSpan = input.split(",");
				int minFreq = Integer.parseInt(freqSpan[0]);
				int maxFreq = Integer.parseInt(freqSpan[1]);
				if (isInputValueResonable(minFreq, maxFreq)) {
					SharedPreferences cmmbSetting = v.getContext()
							.getSharedPreferences(Define.CMMBSETING,Context.MODE_PRIVATE);
					SharedPreferences.Editor cmmbEditor = cmmbSetting.edit();
					cmmbEditor.putInt("minFreq", minFreq);
					cmmbEditor.putInt("maxFreq", maxFreq);
					cmmbEditor.commit();
					this.dismiss();
				} else {
					mFreSpanEditText.setText("");
					if (mInputVaToast == null) {
						mInputVaToast = Toast.makeText(this.getContext(), this
								.getContext().getResources().getString(R.string.inputvaluleerr),
								Toast.LENGTH_SHORT);
					}
					mInputVaToast.show();

				}

			} catch (Exception e) {
				if (mInterToast == null) {
					mInterToast = Toast.makeText(this.getContext(), this
							.getContext().getResources().getString(R.string.inputerroralert),
							Toast.LENGTH_SHORT);
				}
				mInterToast.show();
			}
			break;

		case R.id.bt_cancel:
			this.dismiss();
			break;
		}
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.freq_span_layout);
		SharedPreferences cmmbSetting = this.getContext().getSharedPreferences(
				Define.CMMBSETING, Context.MODE_PRIVATE);
		mFreSpanEditText = (EditText) findViewById(R.id.ev_inputfreq);
		mFreSpanEditText.setText(cmmbSetting.getInt("minFreq", 13) + ","
				+ cmmbSetting.getInt("maxFreq", 48));
		mConfirmButton = (Button) findViewById(R.id.bt_confirm);
		mCancelButton = (Button) findViewById(R.id.bt_cancel);
		mCancelButton.setOnClickListener(this);
		mConfirmButton.setOnClickListener(this);
	}

}
