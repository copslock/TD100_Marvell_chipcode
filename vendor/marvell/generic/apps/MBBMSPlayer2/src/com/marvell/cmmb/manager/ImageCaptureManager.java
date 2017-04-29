package com.marvell.cmmb.manager;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.ThumbnailUtils;
import android.net.Uri;
import android.os.Handler;
import android.os.Message;
import android.os.ParcelFileDescriptor;
import android.provider.MediaStore;
import android.provider.MediaStore.Images.Thumbnails;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ImageView;
import android.widget.Toast;

import com.marvell.cmmb.R;
import com.marvell.cmmb.aidl.MBBMSEngine;
import com.marvell.cmmb.common.FileManager;
import com.marvell.cmmb.common.MemoryStatus;
import com.marvell.cmmb.view.customview.ThumbnailView;
import com.marvell.cmmb.view.dialog.ConfirmDialog;
import com.marvell.cmmb.view.dialog.ConfirmDialogCallBack;

public class ImageCaptureManager implements OnClickListener, ConfirmDialogCallBack {
	private Context mContext;

	private int mCaptureState;

	private File mImageFile;

	private String mImageName;

	private View mCaptureView;

	private ConfirmDialog mConfirmDialog;
	   
	public static ImageCaptureManager sInstance;

	private static final int CAP_STATUS_IDLE = 0;

	private static final int CAP_STATUS_NOSDCARD = CAP_STATUS_IDLE - 1;

	private static final int CAP_STATUS_CAPTURE = CAP_STATUS_IDLE + 1;

	private static final int CAP_STATUS_CAPSUCCESS = CAP_STATUS_CAPTURE + 1;

	private static final int CAP_STATUS_CAPFAIL = CAP_STATUS_CAPSUCCESS + 1;

	public static ImageCaptureManager getInstance(Context context) {
		if (sInstance == null) {
			sInstance = new ImageCaptureManager(context);
		}
		return sInstance;
	}

	private Toast mNoSDCard;

	private Toast mCapSuccess;

	private Toast mCapFail;

	private ThumbnailView mThumbnailView;

	private final int showTime = 5 * 1000;

	public static final int MESSAGE_DELAY = 1;

	private Handler mHandler = new Handler() {

		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			if (null != mThumbnailView) {
				mThumbnailView.setVisibility(View.GONE);
			}
		}

	};

	private int mSession = 0;

	public ImageCaptureManager(Context context) {
		mContext = context;
		mCaptureState = CAP_STATUS_IDLE;
	}

	public void onImageCapture(boolean done) {
	    if (done) {
//			showToast(CAP_STATUS_CAPSUCCESS);
			showImageThumbnail();
			insertTODatabase();
			showConfirmDialog(0);
		} else {
			if (mImageFile != null && mImageFile.exists()) {
				mImageFile.delete();
			}
			showToast(CAP_STATUS_CAPFAIL);
		}
		mCaptureState = CAP_STATUS_IDLE;
		setCaptureViewStatus();
	}

	private void insertTODatabase()
    {
//	    File outputFile = new File(mImageName);
//	    ContentValues values = new ContentValues();
//	    values.put(MediaStore.MediaColumns.DATA, outputFile.toString());
//	    values.put(MediaStore.MediaColumns.TITLE, outputFile.getName());
//	    values.put(MediaStore.MediaColumns.DATE_ADDED, System.currentTimeMillis());
//	    values.put(MediaStore.MediaColumns.MIME_TYPE, "image/jpeg");
//	    Uri uri = mContext.getContentResolver().insert(android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI,values);
	    mContext.sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE,Uri.parse("file://"+mImageName)));
    }

    private void showImageThumbnail() {
		if (null != mThumbnailView) {
			mThumbnailView.showThumbnail(mImageName,
					ThumbnailView.THUMBNAIL_TYPEIMAGE);
		}
	}

	public void setCaptureView(View captureview) {
		mCaptureView = captureview;
	}

	private void setCaptureViewStatus() {
		if (mCaptureState == CAP_STATUS_IDLE) {
			mCaptureView.setClickable(true);
		} else {
			mCaptureView.setClickable(false);
		}
	}

	private void showToast(int type) {
		switch (type) {
		case CAP_STATUS_NOSDCARD:
			if (mNoSDCard == null) {
				mNoSDCard = Toast.makeText(mContext, mContext.getResources()
						.getString(R.string.nosdcard), Toast.LENGTH_SHORT);
			}
			mNoSDCard.show();
			break;
		case CAP_STATUS_CAPSUCCESS:
			if (mCapSuccess == null) {
				mCapSuccess = Toast.makeText(mContext, mContext.getResources()
						.getString(R.string.capturesuccess, mImageName),
						Toast.LENGTH_LONG);
			}
			mCapSuccess.setText(mContext.getResources().getString(
					R.string.capturesuccess, mImageName));
			mCapSuccess.show();
			break;
		case CAP_STATUS_CAPFAIL:
			if (mCapFail == null) {
				mCapFail = Toast.makeText(mContext, mContext.getResources()
						.getString(R.string.capturefail), Toast.LENGTH_LONG);
			}
			mCapFail.show();
			break;
		}
	}

	public void startImageCapture(int session) {
		if (mCaptureState == CAP_STATUS_IDLE) {
			if (MemoryStatus.getAvailableExternalMemorySize() > 100 * 1000) {
				mImageFile = FileManager.createImageCaptureFile();
				if (mImageFile != null) {
					mImageName = mImageFile.getAbsolutePath();
					ParcelFileDescriptor pfd = FileManager.getFileParcelable(
							mContext, mImageFile);
					if (pfd != null) {
						mCaptureState = CAP_STATUS_CAPTURE;
						MBBMSEngine.getInstance().captureImage(session, pfd);// just for test
						mSession = session;
						try {
							pfd.close();
						} catch (IOException e) {
							e.printStackTrace();
						}
					}
				} else {
					// the sdcard can't write
					mCaptureState = CAP_STATUS_IDLE;
				}

			} else {
				showToast(CAP_STATUS_NOSDCARD);
			}
		}
		setCaptureViewStatus();
	}

	public static Intent getImageFileIntent(String param) {
		Intent intent = new Intent(Intent.ACTION_VIEW);
		intent.addCategory(Intent.CATEGORY_DEFAULT);
		intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		Uri uri = Uri.fromFile(new File(param));
		intent.setDataAndType(uri, "image/*");
		return intent;
	}

	public void setThumbnailView(ThumbnailView view) {
		mThumbnailView = view;
	}

	public void onClick(View v) {
		if (v == mThumbnailView) {
			mContext.startActivity(getImageFileIntent(mImageName));
		}
	}

	public ImageCaptureListener mListener;
	public boolean isInBackgoundCapture;

	public void captrueImage(int session) {
		if (mCaptureState == CAP_STATUS_IDLE) {
			isInBackgoundCapture = true;
			try {
				FileOutputStream fos = mContext.openFileOutput("animation"
						+ session, Context.MODE_WORLD_READABLE
						| Context.MODE_WORLD_WRITEABLE);
				fos.close();
				File file = mContext.getFileStreamPath("animation" + session);
				ParcelFileDescriptor pfd = FileManager.getFileParcelable(
						mContext, file);
				mCaptureState = CAP_STATUS_CAPTURE;
				MBBMSEngine.getInstance().captureImage(session, pfd);// just for test
				mSession = session;
				pfd.close();
			} catch (Exception e) {
				mCaptureState = CAP_STATUS_IDLE;
			}
		}
		setCaptureViewStatus();
	}

	public void onCaptured(int session, boolean done) {
		isInBackgoundCapture = false;
		mCaptureState = CAP_STATUS_IDLE;
		setCaptureViewStatus();
		if (null != mListener)
			mListener.onCaptured(mSession, done);
	}

	public void setListener(ImageCaptureListener l) {
		mListener = l;
	}

	public interface ImageCaptureListener {
		public void onCaptured(int session, boolean done);
	}
	
	public void setContext(Context context){
	    mContext = context;
	    mConfirmDialog = null;
	}
	
    private void showConfirmDialog(int type){
        if (mContext == null)
            return;
        String promptMsg = " ";
        promptMsg = mContext.getResources()
                .getString(R.string.capturesuccess, mImageName);
        if(mConfirmDialog!=null && mConfirmDialog.isShowing()){
            mConfirmDialog.dismiss();
        }
        mConfirmDialog = null;
        mConfirmDialog = new ConfirmDialog(mContext,
                    R.drawable.dialog_hint, mContext.getResources()
                            .getString(R.string.hint), promptMsg);
        mConfirmDialog.setCallBack(this, 0);
        if (!mConfirmDialog.isShowing())
        {
            mConfirmDialog.show();
        }
    }

    public void confirmDialogCancel(int type)
    {

    }

    public void confirmDialogOk(int type)
    {
        if (null != mContext)
            mContext.startActivity(getImageFileIntent(mImageName));
    }
    
    public boolean isShowingDialog(){
        
        if(mConfirmDialog!=null)
        return mConfirmDialog.isShowing();
        return false;
    }
    
    public boolean canExit(){
        return mCaptureState == CAP_STATUS_IDLE;
    }
   
}
