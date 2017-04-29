package com.marvell.updater.xml;

import java.io.IOException;
import java.io.InputStream;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.DefaultHandler;

import com.marvell.updater.entity.BaseOtaPackage;

public abstract class AbstractParse {

    public static final String TAG = "AbstractParse";
    
    public DefaultHandler mHander;

    public boolean parseXml(BaseOtaPackage obj, InputStream in) {

	setHandler(obj);
	SAXParserFactory spf = SAXParserFactory.newInstance();
	try {
	    SAXParser sp = spf.newSAXParser();
	    XMLReader xr = sp.getXMLReader();
	    xr.setContentHandler(mHander);
	    InputSource is = new InputSource(in);
	    xr.parse(new InputSource(in));
	    return true;
	} catch (ParserConfigurationException e) {
	    // TODO Auto-generated catch block
	    e.printStackTrace();
	} catch (SAXException e) {
	    // TODO Auto-generated catch block
	    e.printStackTrace();
	} catch (IOException e) {
	    // TODO Auto-generated catch block
	    e.printStackTrace();
	}
	return false;
    }

    public abstract void setHandler(BaseOtaPackage object);

}
