package com.marvell.updater.xml;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

import com.marvell.updater.entity.BasePackageInfo;
import com.marvell.updater.entity.ReleaseOtaPackage;
import com.marvell.updater.entity.ReleasePackageInfo;
import com.marvell.updater.utils.Constants;

public class ReleasePackageHandler extends DefaultHandler {
    private boolean inStatus = false;
    private boolean inPackage = false;

    private ReleaseOtaPackage mOtaPackage;
    
    private ReleasePackageInfo mPackageInfo;

    private StringBuffer buf = new StringBuffer();

    public ReleasePackageHandler(ReleaseOtaPackage otaPackage) {
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
	    mOtaPackage.mPackages.add(mPackageInfo);
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
	} else if (localName.equals(Constants.TAG_VERSION)) {
	    if (inPackage) {
		mPackageInfo.mVersion = buf.toString().trim();
		buf.setLength(0);
	    }
	} else if (localName.equals(Constants.TAG_TYPE)) {
	    if (inPackage) {
		mPackageInfo.mType = buf.toString().trim();
		buf.setLength(0);
	    }
	} else if (localName.equals(Constants.TAG_URL)) {
	    if (inPackage) {
		mPackageInfo.mPath = buf.toString().trim();
		buf.setLength(0);
	    }
	} else if (localName.equals(Constants.TAG_NAME)) {
	    if (inPackage) {
		mPackageInfo.mName = buf.toString().trim();
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
	    mPackageInfo = new ReleasePackageInfo();
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
