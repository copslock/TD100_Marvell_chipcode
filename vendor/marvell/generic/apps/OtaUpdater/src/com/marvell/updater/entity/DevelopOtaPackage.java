package com.marvell.updater.entity;

import java.util.ArrayList;
import java.util.HashMap;

public class DevelopOtaPackage extends BaseOtaPackage {
    
    public HashMap<String, HashMap<String, ArrayList<DevelopPackageInfo>>> mPackages = 
	new HashMap<String, HashMap<String, ArrayList<DevelopPackageInfo>>>();
    
}
