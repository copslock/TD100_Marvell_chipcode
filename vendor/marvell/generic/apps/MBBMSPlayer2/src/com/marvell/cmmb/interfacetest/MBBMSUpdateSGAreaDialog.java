package com.marvell.cmmb.interfacetest;

import android.app.Dialog;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

import com.marvell.cmmb.R;
import com.marvell.cmmb.common.Action;

public class MBBMSUpdateSGAreaDialog extends Dialog implements OnClickListener{
	private Context mContext;
	private EditText mAreaEditText;
	private Button mConfirmButton;
	private Button mCancelButton;

	public MBBMSUpdateSGAreaDialog(Context context) {
		super(context);
		mContext = context;
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.interfacetestsg_dialog);
		setupView();
	}

	private void setupView() {
		mAreaEditText = (EditText) findViewById(R.id.et_area);
		mConfirmButton = (Button) findViewById(R.id.bt_confirm);
		mCancelButton = (Button) findViewById(R.id.bt_cancel);
		mConfirmButton.setOnClickListener(this);
		mCancelButton.setOnClickListener(this);
	}

	public void onClick(View v) {
		switch(v.getId()) {
		case R.id.bt_confirm:
			String areaName = mAreaEditText.getText().toString();
			Intent areaIntent = new Intent();
			areaIntent.setAction(Action.ACTION_UPDATE_EXTERNAL_SG);
			areaIntent.putExtra("AREA", areaName);
			mContext.sendBroadcast(areaIntent);
			this.dismiss();
			break;
		case R.id.bt_cancel:
			this.dismiss();
			break;
		}
	}
		
}
