package com.marvell.updater.model;

import com.marvell.updater.entity.BaseOtaPackage;

public class DataModel {
    
    private static DataModel mInstance = new DataModel();

    private static Object mLock = new Object(); 
    
    public BaseOtaPackage mOtaPackage;
    
    private DataModel() {
	
    }
    
    public static DataModel getInstance() {
	
	synchronized (mLock) {
	    if(mInstance == null) {
		mInstance = new DataModel();
	    }
	}
	
	return mInstance;
    }
    
}
