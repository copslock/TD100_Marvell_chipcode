package com.marvell.updater;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceScreen;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Toast;

import com.marvell.updater.entity.BaseOtaPackage;
import com.marvell.updater.entity.BasePackageInfo;
import com.marvell.updater.model.DataModel;
import com.marvell.updater.net.ScanThread;
import com.marvell.updater.utils.Constants;

public class UpdaterActivity extends PreferenceActivity implements
	ScanThread.ScanListener {

    public static final String TAG = "UpdateActivity";

    public static final int FIND_UPDATE = 1;

    public static final int UNFIND_UPDATE = 2;

    public static final int NETWORK_ERROR = 3;
    
    public static final int SYSTEM_ERROR = 4;

    private static final int PACKAGE_HAS_REMOVED = 1;

    private static final String KEY_ANDROID_VERSION = "android_version";

    private static final String KEY_KERNEL_VERSION = "kernel_version";
    
    private static final String KEY_SCAN_UPDATE = "scan_update";
    
    private static final String KEY_NETWORK_SETTING = "network_setting";
    
    private static final String KEY_LOCAL_UPDATE = "local_update";

    private Preference mNetwork;
    
    private ProgressDialog mProgress;
    
    private String mLocalPath;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {

	super.onCreate(savedInstanceState);
	addPreferencesFromResource(R.xml.system_update);
	PreferenceScreen screen = getPreferenceScreen();

	Preference androidVersion = screen.findPreference(KEY_ANDROID_VERSION);
	androidVersion.setSummary(Build.VERSION.RELEASE);
	Preference kernelVersion = screen.findPreference(KEY_KERNEL_VERSION);
	kernelVersion.setSummary(getFormattedKernelVersion());
	Preference branch = screen.findPreference(Constants.TAG_BRANCH);
	branch.setSummary(SystemProperties.get("ro.build.manifest.branch", "Unknown"));
	
	final SharedPreferences preferences = getSharedPreferences(
		Constants.NETWORK_SETTING, Context.MODE_PRIVATE);
	String url = preferences.getString(Constants.KEY_SERVER, Constants.URL);
	mNetwork = screen.findPreference(KEY_NETWORK_SETTING);
	mNetwork.setSummary(url);

	Preference localUpdate = screen.findPreference(KEY_LOCAL_UPDATE);
	if (Environment.getExternalStorageState().equals(
		Environment.MEDIA_MOUNTED)) {

	    String name = "/" + Build.PRODUCT + Constants.SUFFIX;
	    mLocalPath = Environment.getExternalStorageDirectory()
		    .toString()
		    + name;

	    if (Constants.LOG_ON) Log.d(TAG, "==mLocalPath = " + mLocalPath);
	    File localOtaFile = new File(mLocalPath);
	    if (localOtaFile.exists()) {
		localUpdate.setSummary(mLocalPath);
	    } else {
		screen.removePreference(localUpdate);
	    }
	} else {
	    screen.removePreference(localUpdate);
	}
	
    }
    
    public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen,
            Preference preference) {
	
	if (KEY_SCAN_UPDATE.equals(preference.getKey())) {
	    SharedPreferences preferences = getSharedPreferences(
		    Constants.NETWORK_SETTING, Context.MODE_PRIVATE);
	    String url = preferences.getString(Constants.KEY_SERVER, Constants.URL);
	    ScanThread scanThread = new ScanThread(url
		    + Constants.GET_SYSTEM_VERSION);
	    scanThread.setScanListenerListener(UpdaterActivity.this);
	    scanThread.start();
	    mProgress = new ProgressDialog(UpdaterActivity.this);
	    mProgress.setMessage(getString(R.string.scanning));
	    mProgress.show();
	    
	} else if (KEY_NETWORK_SETTING.equals(preference.getKey())) {
	    updateNetwork();
	} else if (KEY_LOCAL_UPDATE.equals(preference.getKey())) {
	    File localOtaFile = new File(mLocalPath);
	    if (localOtaFile.exists()) {
		Intent intent = new Intent("com.marvell.intent.UPDATE_CONFIRM");
		intent.putExtra(Constants.KEY_OTA_PATH, mLocalPath);
		startActivity(intent);
	    } else {
		showDialog(PACKAGE_HAS_REMOVED);
	    }
	}
	
	return false;
    }

    public Dialog onCreateDialog(int id) {
	Dialog dialog = null;
	switch(id) {
	case PACKAGE_HAS_REMOVED :
	    AlertDialog.Builder builder = new AlertDialog.Builder(this);
	    String message = getString(R.string.package_uexist, mLocalPath);
	    builder.setMessage(message);
	    builder.setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
	        public void onClick(DialogInterface dialog, int which) {
	        }
	    });
	    dialog = builder.create();
	}
	return dialog;
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {

	MenuInflater inflater = getMenuInflater();
	inflater.inflate(R.menu.option_menu, menu);
	return true;
	
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {

	switch (item.getItemId()) {
	case R.id.network:
	    updateNetwork();
	}
	return super.onOptionsItemSelected(item);

    }

    public void onUpdateCheck(BaseOtaPackage otaPackage) {

	DataModel.getInstance().mOtaPackage = otaPackage;
	
	if (Constants.CODE_SUCCESS.equals(otaPackage.mCode)) {
	    mHandler.sendEmptyMessage(FIND_UPDATE);
	} else if (Constants.CODE_NO_UPDATE.equals(otaPackage.mCode)) {
	    mHandler.sendEmptyMessage(UNFIND_UPDATE);
	} else {
	    mHandler.sendEmptyMessage(SYSTEM_ERROR);
	}

    }

    public void onNetworkException() {
	mHandler.sendEmptyMessage(NETWORK_ERROR);
    }
    
    private void updateNetwork() {
	    AlertDialog.Builder networkDialog = new AlertDialog.Builder(
		    UpdaterActivity.this);
	    LinearLayout layout = (LinearLayout) LayoutInflater.from(this)
		    .inflate(R.layout.network_setting, null, false);
	    networkDialog.setTitle(R.string.network_title);
	    networkDialog.setView(layout);
	    final SharedPreferences preferences = getSharedPreferences(
		    Constants.NETWORK_SETTING, Context.MODE_PRIVATE);
	    String url = preferences.getString(Constants.KEY_SERVER, Constants.URL);
	    final EditText editText = (EditText) layout
		    .findViewById(R.id.server);
	    editText.setText(url);
	    networkDialog.setPositiveButton(R.string.ok,
		    new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dia, int which) {
			    String url = editText.getText().toString();
			    SharedPreferences.Editor edit = preferences.edit();
			    edit.putString(Constants.KEY_SERVER, url);
			    edit.commit();
			    mNetwork.setSummary(url);
			}
		    });

	    networkDialog.setNegativeButton(R.string.cancel, null);
	    networkDialog.show();
    }

    private String getFormattedKernelVersion() {
        String procVersionStr;

        try {
            BufferedReader reader = new BufferedReader(new FileReader("/proc/version"), 256);
            try {
                procVersionStr = reader.readLine();
            } finally {
                reader.close();
            }

            final String PROC_VERSION_REGEX =
                "\\w+\\s+" + /* ignore: Linux */
                "\\w+\\s+" + /* ignore: version */
                "([^\\s]+)\\s+" + /* group 1: 2.6.22-omap1 */
                "\\(([^\\s@]+(?:@[^\\s.]+)?)[^)]*\\)\\s+" + /* group 2: (xxxxxx@xxxxx.constant) */
                "\\((?:[^(]*\\([^)]*\\))?[^)]*\\)\\s+" + /* ignore: (gcc ..) */
                "([^\\s]+)\\s+" + /* group 3: #26 */
                "(?:PREEMPT\\s+)?" + /* ignore: PREEMPT (optional) */
                "(.+)"; /* group 4: date */

            Pattern p = Pattern.compile(PROC_VERSION_REGEX);
            Matcher m = p.matcher(procVersionStr);

            if (!m.matches()) {
                Log.e(TAG, "Regex did not match on /proc/version: " + procVersionStr);
                return "Unavailable";
            } else if (m.groupCount() < 4) {
                Log.e(TAG, "Regex match on /proc/version only returned " + m.groupCount()
                        + " groups");
                return "Unavailable";
            } else {
                return (new StringBuilder(m.group(1)).append("\n").append(
                        m.group(2)).append(" ").append(m.group(3)).append("\n")
                        .append(m.group(4))).toString();
            }
        } catch (IOException e) {
            Log.e(TAG,
                "IO Exception when getting kernel version for Device Info screen",
                e);

            return "Unavailable";
        }
    }

    private Handler mHandler = new Handler() {

	@Override
	public void handleMessage(Message msg) {

	    if (mProgress != null) {
		mProgress.dismiss();
	    }
	    
	    switch (msg.what) {

	    case FIND_UPDATE:
		
		Intent intent = new Intent(UpdaterActivity.this, ModelListActivity.class);
		startActivity(intent);
		
		break;

	    case UNFIND_UPDATE:
		
		Toast.makeText(UpdaterActivity.this, R.string.unfind_update,
			Toast.LENGTH_SHORT).show();
		break;
		
	    case SYSTEM_ERROR:
		
		Toast.makeText(UpdaterActivity.this, R.string.unfind_update,
			Toast.LENGTH_SHORT).show();
		break;

	    case NETWORK_ERROR:
		
		AlertDialog.Builder dialog = new AlertDialog.Builder(
			UpdaterActivity.this);
		dialog.setMessage(R.string.network_error);
		dialog.setNeutralButton(R.string.ok, null);
		dialog.show();
		break;
		
	    }
	}

    };

}
