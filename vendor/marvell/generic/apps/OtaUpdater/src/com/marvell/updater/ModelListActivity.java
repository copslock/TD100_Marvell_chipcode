package com.marvell.updater;

import java.util.Iterator;
import java.util.Set;

import android.app.ListActivity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.marvell.updater.entity.DevelopOtaPackage;
import com.marvell.updater.model.DataModel;
import com.marvell.updater.utils.Constants;

public class ModelListActivity extends ListActivity {

    public static final String TAG = "ModelListActivity";

    private String[] models;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {

	super.onCreate(savedInstanceState);

	Set<String> keys = ((DevelopOtaPackage) DataModel.getInstance().mOtaPackage).mPackages
		.keySet();
	
	models = new String[keys.size()];
	keys.toArray(models);

	getListView().setAdapter(new ModelAdapter(ModelListActivity.this));

    }

    @Override
    protected void onListItemClick(ListView l, View v, int position, long id) {

	String model = (String) v.getTag();
	Intent intent = new Intent(ModelListActivity.this,
		DetailListActivity.class);
	intent.putExtra(Constants.TAG_MODEL, model);
	startActivity(intent);

	super.onListItemClick(l, v, position, id);
    }

    public class ModelAdapter extends ArrayAdapter {

	private Context mContext;

	public ModelAdapter(Context context) {
	    super(context, R.layout.item_model, models);
	    this.mContext = context;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {

	    RelativeLayout layout;
	    if (convertView == null) {
		layout = (RelativeLayout) LayoutInflater.from(mContext)
			.inflate(R.layout.item_model, parent, false);
	    } else {
		layout = (RelativeLayout) convertView;
	    }

	    TextView textView = (TextView) layout.findViewById(R.id.model);

	    textView.setText(models[position]);

	    layout.setTag(models[position]);

	    return layout;
	}

    }

}
