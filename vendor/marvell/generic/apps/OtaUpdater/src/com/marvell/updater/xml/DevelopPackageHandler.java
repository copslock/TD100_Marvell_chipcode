package com.marvell.updater.xml;

import java.util.ArrayList;
import java.util.HashMap;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

import com.marvell.updater.entity.DevelopOtaPackage;
import com.marvell.updater.entity.DevelopPackageInfo;
import com.marvell.updater.utils.Constants;

public class DevelopPackageHandler extends DefaultHandler {
    private boolean inStatus = false;
    private boolean inPackage = false;

    private DevelopOtaPackage mOtaPackage;
    
    private DevelopPackageInfo mPackageInfo;

    private StringBuffer buf = new StringBuffer();

    public DevelopPackageHandler(DevelopOtaPackage otaPackage) {
	this.mOtaPackage = otaPackage;
    }

    @Override
    public void endDocument() throws SAXException {
	super.endDocument();
    }

    @Override
    public void endElement(String uri, String localName, String name)
	    throws SAXException {
	if (localName.equals(Constants.TAG_STATUS)) {
	    inStatus = false;
	} else if (localName.equals(Constants.TAG_PACKAGE)) {
	    mOtaPackage.mPackages.get(mPackageInfo.mModel).get(mPackageInfo.mBranch).add(mPackageInfo);
	    inPackage = false;
	} else if (localName.equals(Constants.TAG_CODE)) {
	    if (inStatus) {
		mOtaPackage.mCode = buf.toString().trim();
		buf.setLength(0);
	    }
	} else if (localName.equals(Constants.TAG_MESSAGE)) {
	    if (inStatus) {
		mOtaPackage.mMessage = buf.toString().trim();
		buf.setLength(0);
	    }
	} else if (localName.equals(Constants.TAG_URL)) {
	    if (inPackage) {
		mPackageInfo.mPath = buf.toString().trim();
		buf.setLength(0);
	    }
	} else if (localName.equals(Constants.TAG_MODEL)) {
	    if (inPackage) {
		mPackageInfo.mModel = buf.toString().trim();
		if (!mOtaPackage.mPackages.containsKey(mPackageInfo.mModel)) {
		    mOtaPackage.mPackages.put(buf.toString().trim(), new HashMap<String, ArrayList<DevelopPackageInfo>>()) ;
		}
		buf.setLength(0);
	    }
	} else if (localName.equals(Constants.TAG_BRANCH)) {
	    if (inPackage) {
		mPackageInfo.mBranch = buf.toString().trim();
		if (!mOtaPackage.mPackages.get(mPackageInfo.mModel).containsKey(mPackageInfo.mBranch)) {
		    mOtaPackage.mPackages.get(mPackageInfo.mModel).put(mPackageInfo.mBranch, new ArrayList<DevelopPackageInfo>());
		}
		buf.setLength(0);
	    }
	} else if (localName.equals(Constants.TAG_DATE)) {
	    if (inPackage) {
		mPackageInfo.mDate = buf.toString().trim();
		buf.setLength(0);
	    }
	} 
    }

    @Override
    public void startDocument() throws SAXException {
	super.startDocument();
    }

    @Override
    public void startElement(String uri, String localName, String name,
	    Attributes attributes) throws SAXException {
	if (localName.equals(Constants.TAG_STATUS)) {
	    inStatus = true;
	} else if (localName.equals(Constants.TAG_PACKAGE)) {
	    mPackageInfo = new DevelopPackageInfo();
	    inPackage = true;
	} 
    }

    @Override
    public void characters(char[] ch, int start, int length)
	    throws SAXException {
	if (inStatus || inPackage) {
	    buf.append(ch, start, length);
	}
    }

}
