package com.marvell.updater.net;

import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.net.HttpURLConnection;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.protocol.HTTP;

import android.os.Build;
import android.util.Log;

import com.marvell.updater.entity.BaseOtaPackage;
import com.marvell.updater.utils.Constants;
import com.marvell.updater.xml.DevelopOtaPackageParser;

public class ScanThread extends Thread {

    public static final String TAG = "ScanThread";

    private String mUrl;

    private ScanListener mListener;

    private boolean mCancel = false;

    public ScanThread(String url) {
	this.mUrl = url;
    }

    public void cancel() {
	if(Constants.LOG_ON) Log.d(TAG, "cancel thread");
	mCancel = true;
    }

    public void run() {

	DefaultHttpClient httpClient = new DefaultHttpClient();
	String url;
	try {
	    url = encode(mUrl, "UTF-8");
	} catch (Exception e1) {
	    e1.printStackTrace();
	    url = mUrl;
	}

	if(Constants.LOG_ON) Log.d(TAG, "encode after url = " + url);
	
	HttpPost request = new HttpPost(url);

	InputStream ins = null;
	
	try {

	    List<NameValuePair> param = new ArrayList<NameValuePair>();
	    param.add(new BasicNameValuePair(Constants.TAG_VERSION, Build.VERSION.RELEASE));
	    param.add(new BasicNameValuePair(Constants.TAG_MODEL, Build.PRODUCT));
	    
	    request.setEntity(new UrlEncodedFormEntity(param, HTTP.UTF_8));
	    
	    HttpResponse response = httpClient.execute(request);
	    if (response.getStatusLine().getStatusCode() == HttpURLConnection.HTTP_OK) {
		ins = response.getEntity().getContent();
		
		BaseOtaPackage otaPackage = DevelopOtaPackageParser.loadOtaPackage(ins);
		
		if (mCancel) {
		    return;
		}
		
		if(Constants.LOG_ON) Log.d(TAG, "==ota.mCode = " + otaPackage.mCode);
		
		mListener.onUpdateCheck(otaPackage);
		if(Constants.LOG_ON) Log.d(TAG, otaPackage.mMessage);

	    } else {
		Log.e(TAG, "Cann't connect to Server");
		if (mListener != null) {
		    mListener.onNetworkException();
		}
	    }

	} catch (ClientProtocolException e) {
	    e.printStackTrace();
	    if (mListener != null) {
		mListener.onNetworkException();
	    }
	} catch (UnsupportedEncodingException e) {
	    e.printStackTrace();
	} catch (IOException e) {
	    e.printStackTrace();
	    mListener.onNetworkException();
	} catch (Exception e) {
	    e.printStackTrace();
	    mListener.onNetworkException();
	} finally {
	    try {
		if (ins != null) {
		ins.close();
		}
	    } catch (IOException e) {
		e.printStackTrace();
	    }
	}
    }

    private static String zhPattern = "[\u4E00-\u9FA5]";

    public String encode(String str, String charset)
	    throws UnsupportedEncodingException {
	Pattern p = Pattern.compile(zhPattern);
	Matcher m = p.matcher(str);
	StringBuffer b = new StringBuffer();
	while (m.find()) {
	    m.appendReplacement(b, URLEncoder.encode(m.group(0), charset));
	}
	m.appendTail(b);
	return b.toString();
    }

    public void setScanListenerListener(ScanListener listener) {
	this.mListener = listener;
    }

    public interface ScanListener {

	public void onNetworkException();

	public void onUpdateCheck(BaseOtaPackage otaPackage);

    }

}
