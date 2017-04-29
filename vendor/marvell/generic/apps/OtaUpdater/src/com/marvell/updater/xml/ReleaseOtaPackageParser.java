package com.marvell.updater.xml;

import java.io.InputStream;

import com.marvell.updater.entity.BaseOtaPackage;
import com.marvell.updater.entity.ReleaseOtaPackage;

public class ReleaseOtaPackageParser extends AbstractParse {

    public static ReleaseOtaPackage loadOtaPackage(InputStream in) {

	ReleaseOtaPackage otaPackage = new ReleaseOtaPackage();
	new ReleaseOtaPackageParser().parseXml(otaPackage, in);
	return otaPackage;
    }

    @Override
    public void setHandler(BaseOtaPackage obj) {
	mHander = new ReleasePackageHandler((ReleaseOtaPackage) obj);
    }

}
