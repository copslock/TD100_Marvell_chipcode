package com.marvell.cmmb.activity;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;

import com.marvell.cmmb.R;
import com.marvell.cmmb.common.AppBase;
import com.marvell.cmmb.service.MBBMSService;

public class PlaybackSetting extends AppBase implements OnCheckedChangeListener
{
    private CheckBox mCheck;
    private SharedPreferences mSharedPreferences;
    private boolean isChecked;
    public static final String SHAREDPREFERENCE_PLAYBACK_CHECK = "playback_setting";
    public static final String PLAYBACK_CHEKED = "playback_on";
    
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.playback_setting);
        initViews();
        setupViews();
        updateDate();
        updateViews();
    }
    
    public void onDestroy(){
        super.onDestroy();
    }
    
    public void initViews()
    {
        mCheck = (CheckBox) findViewById(R.id.cb_playback);
    }

    public void setupViews()
    {
        mCheck.setOnCheckedChangeListener(this);
        mSharedPreferences = getSharedPreferences(
                SHAREDPREFERENCE_PLAYBACK_CHECK, Context.MODE_PRIVATE);
    }

    public void updateDate()
    {
        isChecked = mSharedPreferences.getBoolean(PLAYBACK_CHEKED, false);
    }

    public void updateViews()
    {
        mCheck.setChecked(isChecked);
    }

    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked)
    {
        Editor ed = mSharedPreferences.edit();
        ed.putBoolean(PLAYBACK_CHEKED, isChecked);
        ed.commit();
        MBBMSService.enableScreenOffPlayback = isChecked;
    }
    
}
