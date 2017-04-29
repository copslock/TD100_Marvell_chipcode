/*****************************
 * * (C) Copyright 2011 Marvell International Ltd.
 * * All Rights Reserved
 * * Author Lincoln
 * * Create Date Oct 14 2011
 */
package com.marvell.usbsetting;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import android.app.LauncherActivity;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.ServiceManager;
import android.preference.CheckBoxPreference;
import android.preference.EditTextPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceCategory;
import android.preference.PreferenceScreen;
import android.preference.Preference.OnPreferenceClickListener;
import android.util.Log;
import android.widget.Toast;

import com.android.internal.telephony.TelephonyIntents;
import com.android.internal.telephony.TelephonyProperties;
import com.android.internal.telephony.ISms;
import com.android.internal.telephony.gsm.SmsBroadcastConfigInfo;


public class CellBroadcastActivity extends PreferenceActivity implements
Preference.OnPreferenceChangeListener {

    /** Called when the activity is first created. */
    PreferenceCategory channelListPrefCategory;
    EditTextPreference channelPref;
    EditTextPreference channelPrefDel;
    private static final String KEY_CELL_BROADCAST_PREF = "cell_broadcast_preference";
    private static final String KEY_LANGUAGE_PREF = "language_preference";
    private static final String KEY_CHANNEL_PREF = "channel_preference";
//    private static final String KEY_CHANNEL_PREF_DEL = "channel_preference_del";
    private static final String KEY_CHANNEL_LIST_PREF = "channel_list_preference";
    private static final String TAG = "CellBroadcastActivity";
    private static final String[] LANGUAGES_0000 = new String[]{
        "German",
        "English",
        "Italian",
        "French",
        "Spanish",
        "Dutch",
        "Swedish",
        "Danish",
        "Portuguese",
        "Finnish",
        "Norwegian",
        "Greek",
        "Turkish",
        "Hungarian",
        "Polish"
    };
    private static final String[] LANGUAGES_0001 = new String[]{
        "GSM 7bit code",
        "UCS2 code"
    };
    private static final String[] LANGUAGES_0010 = new String[]{
        "Czech",
        "Hebrew",
        "Arabic",
        "Russian",
        "Icelandic"
    };
    private static HashMap<String, Integer> LanguageMap=new HashMap<String, Integer>();
    private static List<Integer> LanguageSelected = new ArrayList<Integer>();
    private static HashMap<Integer, Integer> ChannelMap=new HashMap<Integer, Integer>();
    private static final int OP_ERROR = -1;
    private static final int QUERY_CHANNEL = 0;
    private static final int ADD_CHANNEL = 1;
    private static final int DEL_CHANNEL = 2;
    private static final int ADD_LANGUAGE = 3;
    private static final int DEL_LANGUAGE = 4;
    private static final int SET_ACTIVATION = 5;
    private SharedPreferences mPreferences;
    private Handler mHandler;
    private CheckBoxPreference cellBroadcastPref;
    private PreferenceScreen languagePref;
    private static boolean pending ;

    public void readFromNonVolatile(Context context){
        mPreferences = context.getSharedPreferences("CellBroadcastConfig", 
                Context.MODE_WORLD_READABLE | Context.MODE_WORLD_WRITEABLE);
    }
    public void saveToNonVolatile(){
        SharedPreferences settings = getSharedPreferences("CellBroadcastConfig", 
                Context.MODE_WORLD_READABLE | Context.MODE_WORLD_WRITEABLE);
        SharedPreferences.Editor preferenceEditor = mPreferences.edit();
        preferenceEditor.commit();
    }    
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mHandler = new MyHandler();
        pending = false;
        addPreferencesFromResource(R.xml.cell_broadcast_setting);
        readFromNonVolatile(getBaseContext());
        channelListPrefCategory = (PreferenceCategory) findPreference(KEY_CHANNEL_LIST_PREF);
        
        cellBroadcastPref = (CheckBoxPreference) findPreference(KEY_CELL_BROADCAST_PREF);
        cellBroadcastPref.setEnabled(!pending);
        cellBroadcastPref.setOnPreferenceChangeListener(this);
        
        languagePref = (PreferenceScreen) findPreference(KEY_LANGUAGE_PREF);
        initLanguagePref();
        
        channelPref = (EditTextPreference) findPreference(KEY_CHANNEL_PREF);
        channelPref.setDialogMessage("Format:\r\n 8 or 1-8");
        channelPref.setOnPreferenceChangeListener(this);
        
        initStatus();
    }
    
    private void initLanguagePref() {
        int num = LANGUAGES_0000.length;
        for (int i = 0; i < num; i++) {
            CheckBoxPreference nextScreenCheckBoxPref = new CheckBoxPreference(this);
            nextScreenCheckBoxPref.setKey(LANGUAGES_0000[i]);
            nextScreenCheckBoxPref.setTitle(LANGUAGES_0000[i]);
            nextScreenCheckBoxPref.setOnPreferenceChangeListener(this);
            languagePref.addPreference(nextScreenCheckBoxPref);
            LanguageMap.put(LANGUAGES_0000[i], i);
        }
        num = LANGUAGES_0001.length ;
        for (int i = 0; i < num; i++) {
            CheckBoxPreference nextScreenCheckBoxPref = new CheckBoxPreference(this);
            nextScreenCheckBoxPref.setKey(LANGUAGES_0001[i]);
            nextScreenCheckBoxPref.setTitle(LANGUAGES_0001[i]);
            nextScreenCheckBoxPref.setOnPreferenceChangeListener(this);
//            languagePref.addPreference(nextScreenCheckBoxPref);
            LanguageMap.put(LANGUAGES_0001[i], i+16);
        }
        num = LANGUAGES_0010.length ;
        for (int i = 0; i < num; i++) {
            CheckBoxPreference nextScreenCheckBoxPref = new CheckBoxPreference(this);
            nextScreenCheckBoxPref.setKey(LANGUAGES_0010[i]);
            nextScreenCheckBoxPref.setTitle(LANGUAGES_0010[i]);
            nextScreenCheckBoxPref.setOnPreferenceChangeListener(this);
            languagePref.addPreference(nextScreenCheckBoxPref);
            LanguageMap.put(LANGUAGES_0010[i], i + 32);
        }
    }
    private void initStatus() {
        checkCellBroadcastConfig(cellBroadcastPref.isChecked() ); 
    }
    @Override
    public boolean onPreferenceChange(Preference preference, Object objValue) {
        Log.d(TAG, "--> go here onPreferenceChange()");
        int mode;
        if (KEY_CHANNEL_PREF.equals(preference.getKey())) {
            int[] values = parserString((String) objValue);
            if(values[0] != -1){
                mode = ADD_CHANNEL;
                final PersistTask task = new PersistTask(this, mode);
                task.execute(values);
            }
        } else if (KEY_CELL_BROADCAST_PREF.equals(preference.getKey())) {
            cellBroadcastPref.setChecked((Boolean) objValue);
            pending = true;
            cellBroadcastPref.setEnabled(!pending);
            setCellBroadcastActivation((Boolean) objValue);

        }else if(LanguageMap.containsKey(preference.getKey())){
            Log.d(TAG, "--> in lan setting preference.getKey() = " + preference.getKey());
            int i = LanguageMap.get(preference.getKey());
            mode = ((Boolean) objValue) ? ADD_LANGUAGE : DEL_LANGUAGE;
            int[] values = new int[2];
            values[0] = i;
            values[1] = i;
            final PersistTask task = new PersistTask(this, mode);
            task.execute(values);
        }
//        saveToNonVolatile();
        return true;
    }
    
    private void setCellBroadcastActivation(boolean checked) {
        try{
            final PersistTask task = new PersistTask(this, SET_ACTIVATION);
            int[] c = new int[1];
            c[0] = (checked ? 1: 0);
            task.execute(c);
        }catch(Exception e){
            e.printStackTrace();
        }
    }
    private int[] parserString(String objValue) {
        int valueFrom = -1;
        int valueTo = -1;
        int[] ret = new int[2];
        ret[0] = valueFrom;
        ret[1] = valueTo;
        try{
            if(objValue == null )
                return ret;
            String[] sArray=((String) objValue).split("-");
            if(sArray.length == 1){
                valueFrom = Integer.parseInt( sArray[0]);
                valueTo = valueFrom;
            }else if(sArray.length == 2){
                valueFrom = Integer.parseInt( sArray[0]);
                valueTo = Integer.parseInt( sArray[1]);
            }
            ret[0] = valueFrom;
            ret[1] = valueTo;
        }catch (Exception e){
            e.printStackTrace();
            Toast.makeText(getBaseContext(), "num format pls!", Toast.LENGTH_SHORT).show();
        }
        return ret;
    }
    
    private void checkCellBroadcastConfig(boolean checked){
        channelListPrefCategory.removeAll();
        if(!checked){
            return;
        }
        if (checked) Log.d(TAG, "click it true");
        final PersistTask task = new PersistTask(this, QUERY_CHANNEL);
        task.execute();
    }
    
    public class PersistTask<Params,Progress,Result>  extends
    AsyncTask<Params,Progress,Result>  {
        private int mMode;
        private int result;
        ISms iccISms;
        List<SmsBroadcastConfigInfo> info;
        boolean success ;

        public PersistTask(CellBroadcastActivity cellBroadcastActivity, int mode) {
            mMode = mode;
        }

        @Override
        protected List<SmsBroadcastConfigInfo> doInBackground(Object... params) {
            try {
                result = OP_ERROR;
                iccISms = ISms.Stub.asInterface(ServiceManager.getService("isms"));
                if (iccISms == null) {
                    return null;
                }
                if (mMode == QUERY_CHANNEL) {
                    Log.d(TAG, "trying info .. ");
                    info = iccISms.getCellBroadcastConfig();
                    Log.d(TAG, "info query over ");
                    Log.d(TAG, "info = " + info);
                    result = (info != null)? QUERY_CHANNEL:OP_ERROR;
                    return info;
                } else if (mMode == ADD_CHANNEL) {
                    int[] values = (int[]) params[0];
                    Log.d(TAG, "values[0] = " + values[0]);
                    Log.d(TAG, "values[1] = " + values[1]);
                    success = iccISms.enableCellBroadcastRangeParameter(values[0], values[1],-1,-1);
                    Log.d(TAG, "---> success = " + success);
                    result = success ? ADD_CHANNEL:OP_ERROR;
                    return null;
                } else if (mMode == DEL_CHANNEL) {
                    int[] values = (int[]) params[0];
                    Log.d(TAG, "values[0] = " + values[0]);
                    Log.d(TAG, "values[1] = " + values[1]);
                    success = iccISms.disableCellBroadcastRangeParameter(values[0], values[1],-1,-1);
                    Log.d(TAG, "---> success del = " + success);
                    result = success ? DEL_CHANNEL:OP_ERROR;
                    return null;
                } else if (mMode == ADD_LANGUAGE) {
                    int[] values = (int[]) params[0];
                    Log.d(TAG, "values[0] = " + values[0]);
                    Log.d(TAG, "values[1] = " + values[1]);
                    success = iccISms.enableCellBroadcastRangeParameter(-1,-1,values[0], values[1]);
                    result = success? ADD_LANGUAGE:OP_ERROR;
                    return null;
                } else if (mMode == DEL_LANGUAGE) {
                    int[] values = (int[]) params[0];
                    Log.d(TAG, "values[0] = " + values[0]);
                    Log.d(TAG, "values[1] = " + values[1]);
                    success = iccISms.disableCellBroadcastRangeParameter(-1,-1,values[0], values[1]);
                    result = success?DEL_LANGUAGE:OP_ERROR;
                    return null;
                } else if (mMode == SET_ACTIVATION) {
                    int[] values = (int[]) params[0];
                    success = iccISms.setCellBroadcastActivation( values[0] == 1? true:false);
                    Log.d(TAG, "--> main success = " + success);
                    result = success?SET_ACTIVATION:OP_ERROR;
                    if (success) {
                        checkCellBroadcastConfig(values[0] == 1 ? true : false);
                        pending = false;
                    }
                    return null;
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
            return null;
        }
        
        @Override
        protected void onPostExecute(Result r) {
            switch(result){
                case OP_ERROR:
                    Log.d(TAG, "--> onPostExecute result = " + result);
                    Toast.makeText(getBaseContext(), "operation failed.", Toast.LENGTH_SHORT)
                            .show();
                    break;
                case QUERY_CHANNEL:
                    Log.d(TAG, "--> onPostExecute QUERY_CHANNEL result = " + result);
                    updateUI(info, iccISms);
                    break;
                case ADD_CHANNEL:
                    Log.d(TAG, "--> onPostExecute ADD_CHANNEL result = " + result);
                    if(success){
                        Toast.makeText(getBaseContext(), "Add OK.", Toast.LENGTH_SHORT).show();
                    }else{
                        Toast.makeText(getBaseContext(), "operation failed.", Toast.LENGTH_SHORT).show();
                    }
                    initStatus();
                    break;
                case DEL_CHANNEL:
                    Log.d(TAG, "--> onPostExecute DEL_CHANNEL result = " + result);
                    if(success){
                        Toast.makeText(getBaseContext(), "Remove OK.", Toast.LENGTH_SHORT).show();
                    }else{
                        Toast.makeText(getBaseContext(), "Operation failed.", Toast.LENGTH_SHORT).show();
                    }
                    initStatus();
                    break;
                case ADD_LANGUAGE:
                case DEL_LANGUAGE:
                    Log.d(TAG, "--> onPostExecute LANGUAGE result = " + result);
                    break;
                case SET_ACTIVATION:
                    cellBroadcastPref.setEnabled(!pending);
                    break;
            }
            info = null;
            success = false;
        }

        private void updateUI( List<SmsBroadcastConfigInfo> info, final ISms iccISms) {
            Iterator<SmsBroadcastConfigInfo> it = info.iterator();
            int preFromSrvId = -1;
            int preToSrvId = -1;
//            int fromCodeScheme = -1;
//            int toCodeScheme = -1;
            LanguageSelected.clear();
            ChannelMap.clear();
            channelListPrefCategory.removeAll();
            Preference preP = new Preference(getBaseContext());;
            while(it.hasNext()){
                SmsBroadcastConfigInfo item = (SmsBroadcastConfigInfo) it.next();
                int fromSrvId = item.getFromServiceId();
                int toSrvId = item.getToServiceId();
                int fromCodeScheme = item.getFromCodeScheme();
                int toCodeScheme = item.getToCodeScheme();
                Log.d(TAG, "fromCodeScheme = " + fromCodeScheme);
                Log.d(TAG, "toCodeScheme = " + toCodeScheme);
                for (int i = fromCodeScheme; i <= toCodeScheme; i++) {
                    if(LanguageMap.containsValue(i)){
                        LanguageSelected.add(i);
                    }
                }
                if (preToSrvId == toSrvId)
                    continue;
                Log.d(TAG, "fromSrvId = " + fromSrvId);
                Log.d(TAG, "toSrvId = " + toSrvId);
                if (!ChannelMap.containsKey(fromSrvId))
                    ChannelMap.put(fromSrvId, toSrvId);
                preFromSrvId = fromSrvId;
                preToSrvId = toSrvId;
            }
            
            int num = LANGUAGES_0000.length;
            for (int i = 0; i < num; i++) {
                ((CheckBoxPreference) languagePref.findPreference(LANGUAGES_0000[i]))
                        .setChecked(LanguageSelected.contains(LanguageMap.get(LANGUAGES_0000[i])));
            }
           /* CP don't support Gsm7bit and UCS2 */
           /* num = LANGUAGES_0001.length ;
            for (int i = 0; i < num; i++) {
                ((CheckBoxPreference) languagePref.findPreference(LANGUAGES_0001[i]))
                        .setChecked(LanguageSelected.contains(LanguageMap.get(LANGUAGES_0001[i])));
            }*/
            num = LANGUAGES_0010.length ;
            for (int i = 0; i < num; i++) {
                ((CheckBoxPreference) languagePref.findPreference(LANGUAGES_0010[i]))
                        .setChecked(LanguageSelected.contains(LanguageMap.get(LANGUAGES_0010[i])));
            }
            Log.d(TAG, "--> hash map = " + ChannelMap);
            Iterator<Map.Entry<Integer, Integer>> i = ChannelMap.entrySet().iterator();
            while(i.hasNext()){
                Map.Entry<Integer, Integer> item = i.next();
                int fromSrvId = item.getKey();
                int toSrvId = item.getValue();
                Preference p = new Preference(getBaseContext());
                if(fromSrvId == toSrvId)
                    p.setTitle(fromSrvId+"");
                else
                    p.setTitle(fromSrvId+"-"+toSrvId);
                p.setSummary("enable, press to remove.");
                try{
                    iccISms.enableCellBroadcastRangeParameter(fromSrvId, toSrvId,-1,-1);
                }catch(Exception e){
                    e.printStackTrace();
                }
                p.setOnPreferenceClickListener(new OnPreferenceClickListener() {
                    
                    @Override
                    public boolean onPreferenceClick(Preference preference) {
                        boolean success;
                        int delFrom = -1;
                        int delTo = -1;
                       String s = (String) preference.getTitle();
                      String[] ss = s.split("-");
                      Log.d(TAG, "ss = " + ss[0]);
                    
                      try{
                          if(ss.length == 1){
                              delFrom = Integer.valueOf(ss[0]).intValue();
                              delTo = delFrom;
                          }else if(ss.length == 2){
                              delFrom = Integer.valueOf(ss[0]).intValue();
                              delTo = Integer.valueOf(ss[1]).intValue();
                          }
                          success = iccISms.disableCellBroadcastRangeParameter(delFrom, delTo,-1,-1);
                          if(success){
                              channelListPrefCategory.removePreference(preference);
                              Toast.makeText(getBaseContext(), "Remove O.K.", Toast.LENGTH_SHORT).show();
                          }else{
                              Toast.makeText(getBaseContext(), "Operation failed.", Toast.LENGTH_SHORT).show();
                          }
                              
                          Log.d(TAG, "---> success del = " + success);
                      }catch(Exception e){
                          e.printStackTrace();
                      }
                        return false;
                    }
                });
                channelListPrefCategory.addPreference(p);
                preP = p;
            }
        }
    }
    @Override
    protected void onPause() {
        super.onPause();
        Log.d(TAG, "onPause");
    }
    
    private class MyHandler extends Handler {

    }
}