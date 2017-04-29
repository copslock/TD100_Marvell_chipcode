package com.marvell.cmmb.manager;

import java.util.ArrayList;
import java.util.HashMap;

import android.content.Context;
import com.marvell.cmmb.resolver.ChannelItem;
import com.marvell.cmmb.aidl.LogMask;

public class VicePlayerManager {
	private Context mContext;
	private static VicePlayerManager sInstance;
	private ArrayList<ChannelItem> mMainChannelList;
	private ArrayList<ChannelItem> mViceChannelList;
	private int mViceListPosition;
	private static final String TAG = "VicePlayerManager";
	
	private VicePlayerManager(Context context) {
		mContext = context;
	}
	
	public static VicePlayerManager getInstance(Context context) {
		if(sInstance == null) {
			sInstance = new VicePlayerManager(context);
		}
		return sInstance;
	}
	
	public void init(ArrayList<ChannelItem> channelList) {
		if(mMainChannelList == null) {
			mMainChannelList = new ArrayList<ChannelItem>();
		}
		if(mViceChannelList == null) {
			mViceChannelList = new ArrayList<ChannelItem>();
		}
		mMainChannelList = channelList;
	}
	
	public void resetViceChannelList(int freq) {
		mViceChannelList.clear();
		for(int i = 0; i < mMainChannelList.size(); i++) {
			if(mMainChannelList.get(i).getFrequecy() == freq) {
				mViceChannelList.add(mMainChannelList.get(i));
			} 
		}
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "vice channel list size is " + mViceChannelList.size(), LogMask.APP_COMMON);
	}
	
	public int getVicePosition(ChannelItem item) {
		return mMainChannelList.indexOf(item);
	}
	
	public void setViceListPosition(ChannelItem item) {
		mViceListPosition = mViceChannelList.indexOf(item);
	}
	
	public ChannelItem getCurrentChannel() {
		return mViceChannelList.get(mViceListPosition);
	}
	
	public void switchToRight(boolean right) {
		if(right) {
			if(mViceListPosition < mViceChannelList.size() - 1) {
				mViceListPosition = mViceListPosition + 1;
			} else {
				mViceListPosition = 0;
			}
		} else {
			if(mViceListPosition > 0) {
				mViceListPosition = mViceListPosition - 1;
			} else {
				mViceListPosition = mViceChannelList.size() - 1;
			}
		}	
	}
	
}
