package com.android.wapi;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.BufferedReader;
import java.util.ArrayList;
import android.os.Bundle;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;
import android.os.FileUtils;
import android.os.Process;

public class WapiCertMgmtDialog extends AlertDialog implements DialogInterface.OnClickListener,
        AdapterView.OnItemSelectedListener, View.OnClickListener {

    private static final String TAG = "WapiCertMgmtDialog";

    private static final String InstallTitle = "Install";
    private static final String UninstallTitle = "Uninstall";
    public static final int MODE_INSTALL = 0;
    public static final int MODE_UNINSTALL = 1;
    private int mMode = MODE_INSTALL;
    // General views
    private View mView;

    private TextView mCreateSubdirText;
    private EditText mCreateSubdirEdit;

    private TextView mRootCertText;
    private Spinner  mRootCertSpinner;
    private ArrayList<String> mRootCertArray;
    private String   mRootCertName;

    private TextView mUserCertText;
    private Spinner  mUserCertSpinner;
    private ArrayList<String> mUserCertArray;
    private String   mUserCertName;

    private TextView mDeletDirText;
    private Spinner  mDeletDirSpinner;

    private String mUninstallCerts;
    private CharSequence mCustomTitle;

    private static final int INSTALL_BUTTON = BUTTON1;
    private static final int UNINSTALL_BUTTON = BUTTON3;
    private static final int CANCEL_BUTTON = BUTTON2;

    // Button positions, default to impossible values
    private int mInstallButtonPos = Integer.MAX_VALUE;
    private int mUninstallButtonPos = Integer.MAX_VALUE;
    private int mCancelButtonPos = Integer.MAX_VALUE;

    private static final String DEFAULT_CERTIFICATE_PATH =
        "/data/misc/wifi/wapi_certs";
    private static final String DEFAULT_SDCARD_PATH =
        "/sdcard/";

    public WapiCertMgmtDialog(Context context) {
        super(context);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        onLayout();
        super.onCreate(savedInstanceState);
    }
    private void onLayout() {
        int positiveButtonResId = 0;
        int negativeButtonResId = 0;
        int neutralButtonResId = 0;

        setInverseBackgroundForced(true);

        if (mMode == MODE_INSTALL) {
            setLayout(R.layout.wifi_wapi_cert_install);
            positiveButtonResId = R.string.wifi_wapi_cert_install_button;
            mInstallButtonPos = INSTALL_BUTTON;
        } else if (mMode == MODE_UNINSTALL) {
            setLayout(R.layout.wifi_wapi_cert_uninstall);
            neutralButtonResId = R.string.wifi_wapi_cert_uninstall_button;
            mUninstallButtonPos = UNINSTALL_BUTTON;
        }
        negativeButtonResId = R.string.wifi_wapi_cert_cancel_button;
        mCancelButtonPos = CANCEL_BUTTON;

        setButtons(positiveButtonResId, negativeButtonResId, neutralButtonResId);
    }

    private void setLayout(int layoutResId) {
        setView(mView = getLayoutInflater().inflate(layoutResId, null));
        onReferenceViews(mView);
    }

    /** Called when we need to set our member variables to point to the views. */
    private void onReferenceViews(View view) {
        if (mMode == MODE_INSTALL) {
            mCreateSubdirText = (TextView)view.findViewById(R.id.wapi_cert_create_subdir_text);
            mCreateSubdirEdit = (EditText)view.findViewById(R.id.wapi_cert_create_subdir_edit);

            mRootCertText = (TextView)view.findViewById(R.id.wapi_root_cert_text);
            mRootCertSpinner = (Spinner)view.findViewById(R.id.wapi_root_cert_spinner);
            mRootCertSpinner.setOnItemSelectedListener(this);
            setRootCertSpinnerAdapter();

            mUserCertText = (TextView)view.findViewById(R.id.wapi_user_cert_text);
            mUserCertSpinner = (Spinner)view.findViewById(R.id.wapi_user_cert_spinner);
            mUserCertSpinner.setOnItemSelectedListener(this);
            setUserCertSpinnerAdapter();
        } else if (mMode == MODE_UNINSTALL) {
            mDeletDirText = (TextView)view.findViewById(R.id.wifi_wapi_cert_delet_subdir_text);
            mDeletDirSpinner = (Spinner)view.findViewById(R.id.wifi_wapi_cert_delet_subdir_spinner);
            mDeletDirSpinner.setOnItemSelectedListener(this);
            setDeletDirSpinnerAdapter();
        }
    }
    public void setMode(int mode) {
        mMode = mode;
    }

    private void setButtons(int positiveResId, int negativeResId, int neutralResId) {
        final Context context = getContext();

        if (positiveResId > 0) {
            setButton(context.getString(positiveResId), this);
        }

        if (neutralResId > 0) {
            setButton3(context.getString(neutralResId), this);
        }

        if (negativeResId > 0) {
            setButton2(context.getString(negativeResId), this);
        }
    }

    public void onClick(DialogInterface dialog, int which) {
        Log.v(TAG, "onClick which " + which);
        if (which == mInstallButtonPos) {
            handleInstall();
        } else if (which == mUninstallButtonPos) {
            handleUninstall();
        } else if (which == mCancelButtonPos) {
            handleCancle();
        }
    }

    private void handleInstall() {
        Log.v(TAG, "handleInstall");

        String subdir = getInput(mCreateSubdirEdit);
        Log.v(TAG, "handleInstall subdir = " + subdir);
        if (null == subdir || TextUtils.isEmpty(subdir)) {
            new AlertDialog.Builder(getContext())
                    .setTitle(R.string.error_title)
                    .setIcon(android.R.drawable.ic_dialog_alert)
                    .setMessage(R.string.wifi_wapi_cert_mgmt_subdir_name_is_empty)
                    .setPositiveButton(android.R.string.ok, null)
                    .show();
            return;
        }
        String stringDestDir = DEFAULT_CERTIFICATE_PATH + "/" + subdir;
        File destDir = new File(stringDestDir);
        if (destDir.exists()) {
            new AlertDialog.Builder(getContext())
                    .setTitle(R.string.error_title)
                    .setIcon(android.R.drawable.ic_dialog_alert)
                    .setMessage(R.string.wifi_wapi_cert_mgmt_subdir_exist)
                    .setPositiveButton(android.R.string.ok, null)
                    .show();
            return;
        }
        try {
            destDir.mkdir();
            int ret = FileUtils.setPermissions(destDir.toString(), FileUtils.S_IRWXU|FileUtils.S_IRWXG|FileUtils.S_IRWXO, -1, -1);
            Log.d(TAG, "setPermission ret = " + ret);
        } catch (Exception e) {
            setMessage(e.toString());
        }
        if (!destDir.exists()) {
            new AlertDialog.Builder(getContext())
                    .setTitle(R.string.error_title)
                    .setIcon(android.R.drawable.ic_dialog_alert)
                    .setMessage(R.string.wifi_wapi_cert_mgmt_subdir_create_fail)
                    .setPositiveButton(android.R.string.ok, null)
                    .show();
            return;
        }

        String rootCert = mRootCertName;
        Log.v(TAG, "install root cert: " + rootCert);
        if (null == rootCert || TextUtils.isEmpty(rootCert)) {
            new AlertDialog.Builder(getContext())
                    .setTitle(R.string.error_title)
                    .setIcon(android.R.drawable.ic_dialog_alert)
                    .setMessage(R.string.wifi_wapi_cert_mgmt_root_name_is_empty)
                    .setPositiveButton(android.R.string.ok, null)
                    .show();
            deleteAll(stringDestDir);
            return;
        }
        File fileRootCert = new File(DEFAULT_SDCARD_PATH + "/" + rootCert);
        if (!fileRootCert.exists()) {
            new AlertDialog.Builder(getContext())
                    .setTitle(R.string.error_title)
                    .setIcon(android.R.drawable.ic_dialog_alert)
                    .setMessage(R.string.wifi_wapi_cert_mgmt_root_dont_exist)
                    .setPositiveButton(android.R.string.ok, null)
                    .show();
            deleteAll(stringDestDir);
            return;
        }
        if (!isRootCertificate(DEFAULT_SDCARD_PATH + "/" + rootCert)) {
            new AlertDialog.Builder(getContext())
                    .setTitle(R.string.error_title)
                    .setIcon(android.R.drawable.ic_dialog_alert)
                    .setMessage(R.string.wifi_wapi_cert_mgmt_root_format_is_wrong)
                    .setPositiveButton(android.R.string.ok, null)
                    .show();
            deleteAll(stringDestDir);
            return;
        }
        File fileDestRoot = new File(stringDestDir + "/" + "root.cer");
        try {
            fileDestRoot.createNewFile();
            int ret = FileUtils.setPermissions(fileDestRoot.toString(), FileUtils.S_IRWXU|FileUtils.S_IRWXG|FileUtils.S_IRWXO, -1, -1);
            Log.d(TAG, "setPermission ret = " + ret);
        } catch (Exception e) {
            setMessage(e.toString());
        }
        if (fileDestRoot.exists()) {
            if (!copyFile(fileDestRoot, fileRootCert)) {
                deleteAll(stringDestDir);
                return;
            }
        } else {
            deleteAll(stringDestDir);
            return;
        }

        String userCert = mUserCertName;
        Log.v(TAG, "install user: " + userCert);
        if (null == rootCert || TextUtils.isEmpty(rootCert)) {
            new AlertDialog.Builder(getContext())
                    .setTitle(R.string.error_title)
                    .setIcon(android.R.drawable.ic_dialog_alert)
                    .setMessage(R.string.wifi_wapi_cert_mgmt_user_name_is_empty)
                    .setPositiveButton(android.R.string.ok, null)
                    .show();
            deleteAll(stringDestDir);
            return;
        }
        File fileUserCert = new File(DEFAULT_SDCARD_PATH + "/" + userCert);
        if (!fileUserCert.exists()) {
            new AlertDialog.Builder(getContext())
                    .setTitle(R.string.error_title)
                    .setIcon(android.R.drawable.ic_dialog_alert)
                    .setMessage(R.string.wifi_wapi_cert_mgmt_user_dont_exist)
                    .setPositiveButton(android.R.string.ok, null)
                    .show();
            deleteAll(stringDestDir);
            return;
        }
        if (!isUserCertificate(DEFAULT_SDCARD_PATH + "/" + userCert) && isTheSuffix(userCert, ".cer")) {
            new AlertDialog.Builder(getContext())
                    .setTitle(R.string.error_title)
                    .setIcon(android.R.drawable.ic_dialog_alert)
                    .setMessage(R.string.wifi_wapi_cert_mgmt_user_format_is_wrong)
                    .setPositiveButton(android.R.string.ok, null)
                    .show();
            deleteAll(stringDestDir);
            return;
        }
        File fileDestUser = new File(stringDestDir + "/" + "user" + (isTheSuffix(userCert, ".cer") ? ".cer" : ".p12"));
        try {
            fileDestUser.createNewFile();
            int ret = FileUtils.setPermissions(fileDestUser.toString(), FileUtils.S_IRWXU|FileUtils.S_IRWXG|FileUtils.S_IRWXO, -1, -1);
            Log.d(TAG, "setPermission ret = " + ret);
        } catch (Exception e) {
            setMessage(e.toString());
        }
        if (fileDestUser.exists()) {
            if (!copyFile(fileDestUser, fileUserCert)) {
                deleteAll(stringDestDir);
                return;
            }
        } else {
            deleteAll(stringDestDir);
            return;
        }

    }

    private boolean copyFile(File fileDest, File fileSource) {
        FileInputStream fI;
        FileOutputStream fO;
        byte[] buf = new byte[1024];
        int i = 0;

        Log.v(TAG, "copyFile");
        try {
            fI = new FileInputStream(fileSource);
        } catch (Exception e) {
            setMessage(e.toString());
            return false;
        }
        try {
            fO = new FileOutputStream(fileDest);
        } catch (Exception e) {
            setMessage(e.toString());
            return false;
        }
        while (true) {
            try {
                i = fI.read(buf);
            } catch (Exception e) {
                setMessage(e.toString());
                return false;
            }
            if (i == -1) {
                break;
            }
            try {
                fO.write(buf, 0, i);
            } catch (Exception e) {
                setMessage(e.toString());
                return false;
            }
        }
        try {
            fI.close();
            fO.close();
        } catch (Exception e) {
            setMessage(e.toString());
            return false;
        }
        return true;
    }

    public int searchString(String find_str, File file) throws Exception {
        FileReader reader = new FileReader(file);
        BufferedReader reader2 = new BufferedReader(reader, 2048);
        String s = "";
        String buffer = new String("");
        do {
            buffer += s;
        }while(( s = reader2.readLine()) != null);
        return buffer.split(find_str).length - 1;
   }

    private boolean isRootCertificate(String rootcert) {
        String stringCertBegin = "BEGIN CERTIFICATE";
        String stringCertEnd = "END CERTIFICATE";
        String stringECBegin = "BEGIN EC PRIVATE KEY";
        String stringECEnd = "END EC PRIVATE KEY";
        File root = new File(rootcert);
        try {
            if (1 != searchString(stringCertBegin, root)) {
                return false;
            }
        } catch (Exception e) {
            setMessage(e.toString());
            return false;
        }
        try {
            if (1 != searchString(stringCertEnd, root)) {
                return false;
            }
        } catch (Exception e) {
            setMessage(e.toString());
            return false;
        }
        try {
            if (searchString(stringECBegin, root) != 0) {
                return false;
            }
        } catch (Exception e) {
            setMessage(e.toString());
            return false;
        }
        try {
            if (searchString(stringECEnd, root) != 0) {
                return false;
            }
        } catch (Exception e) {
            setMessage(e.toString());
            return false;
        }
        return true;
    }

    private boolean isUserCertificate(String usercert) {
        String stringCertBegin = "BEGIN CERTIFICATE";
        String stringCertEnd = "END CERTIFICATE";
        String stringECBegin = "BEGIN EC PRIVATE KEY";
        String stringECEnd = "END EC PRIVATE KEY";
        File user = new File(usercert);
        try {
            if (1 != searchString(stringCertBegin, user)) {
                return false;
            }
        } catch (Exception e) {
            setMessage(e.toString());
            return false;
        }
        try {
            if (1 != searchString(stringCertEnd, user)) {
                return false;
            }
        } catch (Exception e) {
            setMessage(e.toString());
            return false;
        }
        try {
            if (1 != searchString(stringECBegin, user)) {
                return false;
            }
        } catch (Exception e) {
            setMessage(e.toString());
            return false;
        }
        try {
            if (1 != searchString(stringECEnd, user)) {
                return false;
            }
        } catch (Exception e) {
            setMessage(e.toString());
            return false;
        }
        return true;
    }

    private String getInput(EditText edit) {
        return (edit != null) ? (edit.getText().toString()) : null;
    }

    private void handleUninstall() {
        Log.v(TAG, "handleUninstall");
        if (null != mUninstallCerts) {
            deleteAll(mUninstallCerts);
        }
    }

    private void setDeletDirSpinnerAdapter() {
        Context context = getContext();
        File certificateList [];
        ArrayList<String> cerString= new ArrayList<String>();
        int i;

        //find all certificate
        File certificatePath = new File(DEFAULT_CERTIFICATE_PATH);
        try {
            //build string array
            certificateList = certificatePath.listFiles();
            for (i = 0; i < certificateList.length; i++){
                cerString.add(certificateList[i].getName());
            }

            ArrayAdapter<CharSequence> adapter = new ArrayAdapter<CharSequence>(context,
                android.R.layout.simple_spinner_item,
                (String [])cerString.toArray(new String[0]));
            adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
            mDeletDirSpinner.setAdapter(adapter);
        } catch (Exception e){
            setMessage(e.toString());
        }
    }

    private int getDeletDirFromSpinner() {
        int position = mDeletDirSpinner.getSelectedItemPosition();
        return position;
    }

    private void handleDeletDirChange(int deletDirIdx) {
        File certificateList [];

        //find all certificate
        File certificatePath = new File(DEFAULT_CERTIFICATE_PATH);
        try{
	        if (!certificatePath.isDirectory()){
                return;
	        } else {
                certificateList = certificatePath.listFiles();
                mUninstallCerts = certificateList[deletDirIdx].getAbsolutePath();
            }
        } catch (Exception e){
            setMessage(e.toString());
        }
    }

    private boolean isTheSuffix(String file, String suffix) {
        if (file.length() < suffix.length()) {
            return false;
        }
        for (int i = 0; i < suffix.length(); i++) {
            if (file.charAt(file.length() - suffix.length() + i) != suffix.charAt(i)) {
                return false;
            }
        }
        return true;
    }

    private void setRootCertSpinnerAdapter() {
        Context context = getContext();
        File certificateList [];
        mRootCertArray = new ArrayList<String>();
        int i;

        File certificatePath = new File(DEFAULT_SDCARD_PATH);
        if (certificatePath == null) {
            Log.d(TAG, " setRootCertSpinnerAdapter certificate == null");
            return;
        }
        try {
            certificateList = certificatePath.listFiles();
            for (i = 0; i < certificateList.length; i++) {
                if (!isTheSuffix(certificateList[i].getAbsoluteFile().toString(), ".cer")) {
                    continue;
                }
                if (!certificateList[i].isDirectory() &&
                        isRootCertificate(certificateList[i].getAbsoluteFile().toString())) {
                    mRootCertArray.add(certificateList[i].getName());
                }
            }

            ArrayAdapter<CharSequence> adapter = new ArrayAdapter<CharSequence>(context,
                    android.R.layout.simple_spinner_item,
                    (String [])mRootCertArray.toArray(new String[0]));
            adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
            mRootCertSpinner.setAdapter(adapter);
        } catch (Exception e){
            setMessage(e.toString());
        }
    }

    private int getRootCertFromSpinner() {
        int position = mRootCertSpinner.getSelectedItemPosition();
        return position;
    }

    private void handleRootCertChange(int AsIdx) {
        try {
            String[] StringA = (String[])mRootCertArray.toArray(new String[0]);
	        mRootCertName = StringA[AsIdx];
        } catch (Exception e){
            setMessage(e.toString());
        }
    }

    private void setUserCertSpinnerAdapter() {
        Context context = getContext();
        File certificateList [];
        mUserCertArray = new ArrayList<String>();
        int i;

        File certificatePath = new File(DEFAULT_SDCARD_PATH);
        if (certificatePath == null) {
            Log.d(TAG, "  setUserCertSpinnerAdapter certificate == null");
            return;
        }
        try {
            certificateList = certificatePath.listFiles();
            for (i = 0; i < certificateList.length; i++) {
                if (isTheSuffix(certificateList[i].getAbsoluteFile().toString(), ".cer")) {
                    if (!certificateList[i].isDirectory() &&
                        isUserCertificate(certificateList[i].getAbsoluteFile().toString())) {
                        mUserCertArray.add(certificateList[i].getName());
                    }
                } else if (isTheSuffix(certificateList[i].getAbsoluteFile().toString(), ".p12")
                            && !certificateList[i].isDirectory()) {
                    mUserCertArray.add(certificateList[i].getName());
                }
            }

            ArrayAdapter<CharSequence> adapter = new ArrayAdapter<CharSequence>(context,
                    android.R.layout.simple_spinner_item,
                    (String [])mUserCertArray.toArray(new String[0]));
            adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
            mUserCertSpinner.setAdapter(adapter);
        } catch (Exception e) {
            setMessage(e.toString());
        }
    }

    private int getUserCertFromSpinner() {
        int position = mUserCertSpinner.getSelectedItemPosition();
        return position;
    }

    private void handleUserCertChange(int UserIdx) {
        try {
            String[] StringA = (String[])mUserCertArray.toArray(new String[0]);
	        mUserCertName = StringA[UserIdx];
        } catch (Exception e){
            setMessage(e.toString());
        }
    }

    private void deleteAll(String filepath) {
        File f = new File(filepath);
        Log.v(TAG, "deleteAll filepath " + filepath);

        if (f.exists() && f.isDirectory()) {
            File delFile[] = f.listFiles();
            int fileNum = delFile.length;
            int i;
            if (fileNum == 0) {
                f.delete();
            } else {
                for (i = 0; i < fileNum; i++) {
                    String subdirectory = delFile[i].getAbsolutePath();
                    deleteAll(subdirectory);
                }
            }
            f.delete();
        } else if (f.exists()) {
            f.delete();
        }
    }

    private void handleCancle() {
    }

    public void onClick(View v) {
        Log.v(TAG, "onClick View ");
    }

    public void onNothingSelected(AdapterView parent) {
        Log.v(TAG, "onNothingSelected ");
    }

    public void onItemSelected(AdapterView parent, View view, int position, long id) {
        Log.v(TAG, "onItemSelected ");
        if (parent == mDeletDirSpinner) {
            handleDeletDirChange(getDeletDirFromSpinner());
        } else if (parent == mRootCertSpinner) {
            handleRootCertChange(getRootCertFromSpinner());
        } else if (parent == mUserCertSpinner) {
            handleUserCertChange(getUserCertFromSpinner());
        }
    }

    @Override
    public void setTitle(CharSequence title) {
        super.setTitle(title);
        mCustomTitle = title;
    }

    @Override
    public void setTitle(int titleId) {
        setTitle(getContext().getString(titleId));
    }
}

