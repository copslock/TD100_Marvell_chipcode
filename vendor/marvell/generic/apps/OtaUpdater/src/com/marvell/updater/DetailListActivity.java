package com.marvell.updater;

import java.util.ArrayList;
import java.util.HashMap;

import android.app.ExpandableListActivity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseExpandableListAdapter;
import android.widget.ExpandableListView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.marvell.updater.entity.DevelopOtaPackage;
import com.marvell.updater.entity.DevelopPackageInfo;
import com.marvell.updater.model.DataModel;
import com.marvell.updater.utils.Constants;

public class DetailListActivity extends ExpandableListActivity {

    public static final String TAG = "DetailListActivity";
    
    private HashMap<String, ArrayList<DevelopPackageInfo>> mData;
    
    private String[] mGroups;
    
    private String mModel;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {

	super.onCreate(savedInstanceState);
	
	Intent intent = getIntent();
	mModel = intent.getStringExtra(Constants.TAG_MODEL);
	
	if (Constants.LOG_ON) Log.d(TAG, "The boad model is = " + mModel);
	
	setTitle(mModel);
	
	mData = ((DevelopOtaPackage)DataModel.getInstance().mOtaPackage).mPackages.get(mModel);
	
	mGroups = new String[mData.keySet().size()];
	
	mData.keySet().toArray(mGroups);
	
	getExpandableListView().setGroupIndicator(getResources().getDrawable(R.drawable.expander_group));
	
	setListAdapter(new ItemExpandableListAdapter(DetailListActivity.this));
	
    }
    
    @Override
    public boolean onChildClick(ExpandableListView parent, View v,
	    int groupPosition, int childPosition, long id) {
	
	boolean sdMount = Environment.getExternalStorageState().equals(
		Environment.MEDIA_MOUNTED);

	if (sdMount) {

	    SharedPreferences preferences = getSharedPreferences(
		    Constants.NETWORK_SETTING, Context.MODE_PRIVATE);
	    String url = preferences.getString(Constants.KEY_SERVER, Constants.URL);

	    String branch = mGroups[groupPosition];
	    String path = mData.get(branch).get(childPosition).mPath;
	    String date = mData.get(branch).get(childPosition).mDate;

	    if (Constants.LOG_ON)
		Log.d(TAG, "path = " + path);

	    Intent intent = new Intent(DetailListActivity.this,
		    DownloadService.class);
	    intent.putExtra(Constants.KEY_OTA_PATH, url + path);
	    intent.putExtra(Constants.TAG_MODEL, mModel);
	    intent.putExtra(Constants.TAG_BRANCH, branch);
	    intent.putExtra(Constants.TAG_DATE, date);

	    startService(intent);

	} else {
	    Toast.makeText(DetailListActivity.this, R.string.sdcard_unmount,
		    Toast.LENGTH_SHORT).show();
	}
	
	return super.onChildClick(parent, v, groupPosition, childPosition, id);
    }
    
    public class ItemExpandableListAdapter extends BaseExpandableListAdapter {
	
	public Context mContext; 
	
	public ItemExpandableListAdapter(Context context) {
	    this.mContext = context;
	}

	public Object getChild(int groupPosition, int childPosition) {
	    return mData.get(mGroups[groupPosition]).get(childPosition);
	}

	public long getChildId(int groupPosition, int childPosition) {
	    return childPosition;
	}

	public int getChildrenCount(int groupPosition) {
	    return mData.get(mGroups[groupPosition]).size();
	}

	public View getChildView(int groupPosition, int childPosition,
		boolean isLastChild, View convertView, ViewGroup parent) {
	    
	    RelativeLayout layout;
            if (convertView == null) {
                    layout = (RelativeLayout) LayoutInflater
                                    .from(mContext)
                                    .inflate(R.layout.item_child, parent, false);
            } else {
                    layout = (RelativeLayout) convertView;
            }
            
            TextView date = (TextView)layout.findViewById(R.id.date);
	    date.setText(mData.get(mGroups[groupPosition]).get(childPosition).mDate);
            
	    return layout;
	    
	}

	public Object getGroup(int groupPosition) {
	    return mGroups[groupPosition];
	}

	public int getGroupCount() {
	    return mGroups.length;
	}

	public long getGroupId(int groupPosition) {
	    return groupPosition;
	}

	public View getGroupView(int groupPosition, boolean isExpanded,
		View convertView, ViewGroup parent) {
	    
	    RelativeLayout layout;
            if (convertView == null) {
                    layout = (RelativeLayout) LayoutInflater
                                    .from(mContext)
                                    .inflate(R.layout.item_group, parent, false);
            } else {
                    layout = (RelativeLayout) convertView;
            }
            
            TextView branch = (TextView)layout.findViewById(R.id.branch);
	    branch.setText(mGroups[groupPosition]);
            
	    return layout;
	}

	public boolean hasStableIds() {
	    return false;
	}

	public boolean isChildSelectable(int groupPosition, int childPosition) {
	    return true;
	}
	
    }
    
}
