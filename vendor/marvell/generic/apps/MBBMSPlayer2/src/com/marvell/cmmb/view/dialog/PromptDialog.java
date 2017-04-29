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

import android.app.AlertDialog;
import android.content.Context;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

import com.marvell.cmmb.R;
import com.marvell.cmmb.common.Util;

public class PromptDialog extends AlertDialog implements OnClickListener {
	private TextView mTitleTextView;

	private TextView mPromptTextView;

	private Button mConfirmButton;

	private String mTitle;

	private String mPromptMessage;

	private int mDrawableResId;

	private PromptDialogCallBack mCallBack;
	
	private PromptDialogKeycallback mKeyBack;

	private int mType = 0;

	public PromptDialog(Context context, int resId, String title,
			String promptMessage) {
		super(context);
		this.mTitle = title;
		this.mPromptMessage = promptMessage;
		mDrawableResId = resId;
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.bt_confirm:
			this.dismiss();
			if (null != mCallBack) {
				mCallBack.promptDialogOk(mType);
			}
			break;

		}

	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.tvplayer_promptdialog);
		Util.setProperty(this);
		setupView();
	}

	public void setCallBack(PromptDialogCallBack callBack) {
		mCallBack = callBack;
	}

	public void setCallBack(PromptDialogCallBack callBack, int type) {
		mCallBack = callBack;
		mType = type;
	}

	/**
	 * Initialize the view of prompt dialog
	 */
	private void setupView() {
		mTitleTextView = (TextView) findViewById(R.id.tv_hint);
		mTitleTextView.setText(mTitle);
		mTitleTextView.setCompoundDrawablesWithIntrinsicBounds(mDrawableResId,
				0, 0, 0);
		mPromptTextView = (TextView) findViewById(R.id.tv_promptmessage);
		mPromptTextView.setText(mPromptMessage);
		mConfirmButton = (Button) findViewById(R.id.bt_confirm);
		mConfirmButton.setOnClickListener(this);

	}

    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        switch(keyCode){
            case KeyEvent.KEYCODE_BACK:
            	this.dismiss();
                if(mKeyBack!=null)
                    mKeyBack.promptDialogOnKeyBack(mType);
                break;
        }
        return super.onKeyDown(keyCode, event);
    }
    
    public void setKeyCallback(PromptDialogKeycallback keyback){
        this.mKeyBack = keyback;
    }
	
    public interface PromptDialogKeycallback{
        public void promptDialogOnKeyBack(int type);
    }
}
