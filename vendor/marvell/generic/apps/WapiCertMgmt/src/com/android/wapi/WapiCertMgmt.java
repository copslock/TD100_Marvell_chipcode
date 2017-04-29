package com.android.wapi;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;   
import android.view.View.OnClickListener; 
import android.widget.Button;

public class WapiCertMgmt extends Activity implements OnClickListener {
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        Button install = (Button)findViewById(R.id.install);
        install.setOnClickListener(this);
        Button uninstall = (Button)findViewById(R.id.uninstall);
        uninstall.setOnClickListener(this);
    }
    
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.install:
            	showWapiCertInstallDialog();
                break;   
            case R.id.uninstall:
            	showWapiCertUninstallDialog();
                break;   
        }
    }
    
    private void showWapiCertInstallDialog() {
        WapiCertMgmtDialog dialog = new WapiCertMgmtDialog(this);
        dialog.setMode(WapiCertMgmtDialog.MODE_INSTALL);
        dialog.setTitle(R.string.wifi_wapi_cert_install);
        dialog.show();
    }
    
    private void showWapiCertUninstallDialog() {
        WapiCertMgmtDialog dialog = new WapiCertMgmtDialog(this);
        dialog.setMode(WapiCertMgmtDialog.MODE_UNINSTALL);
        dialog.setTitle(R.string.wifi_wapi_cert_uninstall);
        dialog.show();
    }
}
