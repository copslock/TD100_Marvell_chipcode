package com.marvell.updater.xml;

import java.io.InputStream;

import com.marvell.updater.entity.BaseOtaPackage;
import com.marvell.updater.entity.DevelopOtaPackage;

public class DevelopOtaPackageParser extends AbstractParse {

    public static DevelopOtaPackage loadOtaPackage(InputStream in) {

	DevelopOtaPackage otaPackage = new DevelopOtaPackage();
	new DevelopOtaPackageParser().parseXml(otaPackage, in);
	return otaPackage;
    }

    @Override
    public void setHandler(BaseOtaPackage obj) {
	mHander = new DevelopPackageHandler((DevelopOtaPackage) obj);
    }

}
