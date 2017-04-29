package com.marvell.updater.net;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.net.HttpURLConnection;
import java.net.URLEncoder;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.http.HttpResponse;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;

import com.marvell.updater.utils.Constants;

import android.os.Environment;
import android.util.Log;

public class DownloadThread extends Thread {

    public static final String TAG = "DownloadThread";

    private String mUrl;

    private DownloadListener mListener;

    private boolean mCancel = false;

    public DownloadThread(String url) {
	this.mUrl = url;
    }

    public void cancel() {
	if (Constants.LOG_ON) Log.d(TAG, "cancel thread");
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

	HttpPost request = new HttpPost(url);

	if (Constants.LOG_ON) Log.d(TAG, "encode after url = " + url);
	request.setHeader("Content-Type", "text/html;charset=UTF-8");

	String name = mUrl.substring(mUrl.lastIndexOf("/"));

	String localPath = Environment.getExternalStorageDirectory().toString() + name;
	
	if (Constants.LOG_ON) Log.d(TAG, "local path = " + localPath);
	OutputStream os = null;

	try {

	    File file = new File(localPath);
	    if (!file.exists()) {
		file.createNewFile();
	    }

	    Runtime.getRuntime().exec("chmod 777 " + localPath);

	    os = new FileOutputStream(file);

	    HttpResponse response = httpClient.execute(request);
	    if (response.getStatusLine().getStatusCode() == HttpURLConnection.HTTP_OK) {
		InputStream ins = response.getEntity().getContent();
		
		long tatalSize = response.getEntity().getContentLength();
		
		if (mListener != null) {
		    mListener.onDownloadStarted();
		}
		
		byte[] buffer = new byte[2048];
		int index = 0;
		int percent = 0;
		long time = System.currentTimeMillis();
		while ((index = ins.read(buffer)) != -1 && !mCancel) {
		    
		    os.write(buffer, 0, index);
		    long len = file.length();
		    int percentNow = (int)(len * 100 / tatalSize);
		    
		    if (percentNow != percent && System.currentTimeMillis() - time > 1500 ) {
			
			time = System.currentTimeMillis();
			percent = percentNow;
			if (mListener != null) {
				mListener.onDownloadPercent(percent);
			}
		    }
		    
		}
		os.flush();
		if (os != null) {
		    os.close();
		}

		if (mCancel) {
		    file.delete();
		    return;
		}
		
		if (mListener != null) {
		    mListener.onDownloadFinish(localPath);
		}
		
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
	} catch (IOException e) {
	    e.printStackTrace();
	    mListener.onNetworkException();
	} finally {
	    if (os != null) {
		try {
		    os.close();
		} catch (IOException e) {
		    e.printStackTrace();
		}
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

    public void setOnDownloadListener(DownloadListener listener) {
	this.mListener = listener;
    }

    public interface DownloadListener {
	
	public void onDownloadStarted();
	
	public void onDownloadPercent(int downloadByte);

	public void onDownloadFinish(String localPath);

	public void onNetworkException();

    }

}
