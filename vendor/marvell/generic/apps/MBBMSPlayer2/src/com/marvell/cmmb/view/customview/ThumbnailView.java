/*
 * (C) Copyright 2010 Marvell International Ltd.
 * All Rights Reserved
 *
 * MARVELL CONFIDENTIAL
 * Copyright 2008 ~ 2010 Marvell International Ltd All Rights Reserved.
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Marvell International Ltd or its
 * suppliers or licensors. Title to the Material remains with Marvell International Ltd
 * or its suppliers and licensors. The Material contains trade secrets and
 * proprietary and confidential information of Marvell or its suppliers and
 * licensors. The Material is protected by worldwide copyright and trade secret
 * laws and treaty provisions. No part of the Material may be used, copied,
 * reproduced, modified, published, uploaded, posted, transmitted, distributed,
 * or disclosed in any way without Marvell's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Marvell in writing.
 *
 */

package com.marvell.cmmb.view.customview;

import java.io.File;

import com.marvell.cmmb.R;
import com.marvell.cmmb.aidl.LogMask;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.drawable.BitmapDrawable;
import android.media.ThumbnailUtils;
import android.net.Uri;
import android.os.Handler;
import android.os.Message;
import android.provider.MediaStore.Video.Thumbnails;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.ImageView;

/**
 * To show the video or the image's thumbnail.
 * 
 * @author DanyangZhou
 * @version [2011-8-4]
 */
public class ThumbnailView extends ImageView implements OnClickListener {

	public static final int THUMBNAIL_TYPEVIDEO = 1;
	public static final int THUMBNAIL_TYPEIMAGE = 2;
	public static final int DISMISS_THUMBNAIL = 3;
	public static final String SHAREDPREFERENCE_THUMBNAIL = "thumbnail";
	public static final String LAST_THUMBNAIL = "last_thumbnail";
	public static final String LAST_TYPE = "last_type";

	private int mType = -1;

	private Context mContext;

	private String mFilePath;

	private Bitmap mVideoThumbnail;

	private Bitmap mLastImageThumbnail;

	private Bitmap mLastVideoTumbnail;

	private Animation mThumbnailUpAnim;

	private Animation mThumbnailDownAnim;

	private SharedPreferences mSharedPreferences;

	private ThumbnailClickListener mThumbnailListener;

	private Handler mHandler = new Handler() {
		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			switch (msg.what) {
			case DISMISS_THUMBNAIL:
				ThumbnailView.this.setVisibility(View.GONE);
				break;
			}
		}
	};

	public ThumbnailView(Context context) {
		super(context);
		mContext = context;
	}

	public ThumbnailView(Context context, AttributeSet attrs) {
		super(context, attrs);
		mContext = context;
	}

	public ThumbnailView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		mContext = context;
	}

	protected void onFinishInflate() {
		super.onFinishInflate();
		setUpViews();
	}

	private void setUpViews() {
		this.setOnClickListener(this);
		this.setPadding(3, 3, 7, 3);
		mVideoThumbnail = ((BitmapDrawable) mContext.getResources()
				.getDrawable(R.drawable.videothumb)).getBitmap();
		LogMask.LOGM("Thumbnail", LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
				"Thumbnail is recycled is " + mVideoThumbnail.isRecycled(),
				LogMask.APP_COMMON);
		mSharedPreferences = mContext.getSharedPreferences(
				SHAREDPREFERENCE_THUMBNAIL, Context.MODE_PRIVATE);
		mThumbnailUpAnim = AnimationUtils.loadAnimation(mContext,
				R.anim.thumbnailup);
		mThumbnailDownAnim = AnimationUtils.loadAnimation(mContext,
				R.anim.thumbnaildown);
	}

	public void onClick(View arg0) {
		if (mContext == null) {
			return;
		}
		if (mType == THUMBNAIL_TYPEIMAGE) {
			mContext.startActivity(getImageFileIntent(mFilePath));
		} else if (mType == THUMBNAIL_TYPEVIDEO) {
			if (mThumbnailListener != null) {
				mThumbnailListener.onThumbnailClicked(arg0);
			}
			mContext.startActivity(getVideoFileIntent(mFilePath));
		}
	}

	public void showThumbnail(String path, int type) {
		mFilePath = path;
		mType = type;
		writeFilepathAndType(path, type);
		Bitmap bm = getThumbnail(path, type);
		if (bm != null) {
			if (type == THUMBNAIL_TYPEIMAGE) {
			} else if (type == THUMBNAIL_TYPEVIDEO) {
				getVideoThumbnail(bm);
			}
			this.setImageBitmap(bm);
		} else {
			this.setImageResource(R.drawable.icon_tvplayer);
		}
		this.setVisibility(View.VISIBLE);
	}

	public void showThumbnailByVisible(String path, int type) {
		this.mFilePath = path;
		this.mType = type;
		Bitmap bm = getThumbnail(path, type);
		if (bm != null) {
			if (type == THUMBNAIL_TYPEIMAGE) {

			} else if (type == THUMBNAIL_TYPEVIDEO) {
				getVideoThumbnail(bm);
			}
			this.setImageBitmap(bm);
		} else {
			this.setImageResource(R.drawable.icon_tvplayer);
		}
	}

	public Bitmap getVideoThumbnail(Bitmap src) {
		Canvas cv = new Canvas(src);
		cv.drawBitmap(mVideoThumbnail, src.getWidth() / 4, src.getHeight() / 4,
				null);
		cv.save(Canvas.ALL_SAVE_FLAG);
		cv.restore();
		return src;
	}

	public Bitmap getThumbnail(String path, int type) {
		if (path != null && !path.equals("")) {
			recycleThumbnailBitmap();
			if (type == THUMBNAIL_TYPEIMAGE) {
				BitmapFactory.Options bitmapOptions = new BitmapFactory.Options();
				bitmapOptions.inSampleSize = 4;
				Bitmap bm = BitmapFactory.decodeFile(path, bitmapOptions);
				mLastImageThumbnail = ThumbnailUtils.extractThumbnail(bm, 80,
						60);
				bm = null;
				return mLastImageThumbnail;
			} else if (type == THUMBNAIL_TYPEVIDEO) {
				mLastVideoTumbnail = ThumbnailUtils.createVideoThumbnail(path,
						Thumbnails.MINI_KIND);
				return mLastVideoTumbnail;
			}
		}
		return null;
	}

	private void recycleThumbnailBitmap() {
		this.setImageBitmap(null);
		if (mLastImageThumbnail != null && !mLastImageThumbnail.isRecycled()) {
			mLastImageThumbnail.recycle();
			mLastImageThumbnail = null;
		}
		if (mLastVideoTumbnail != null && !mLastVideoTumbnail.isRecycled()) {
			mLastVideoTumbnail.recycle();
			mLastVideoTumbnail = null;
		}
		System.gc();
	}

	public void recycleAllBitmap() {
		recycleThumbnailBitmap();
		if (null != mVideoThumbnail && !mVideoThumbnail.isRecycled()) {
			mVideoThumbnail = null;
		}
	}

	public static Intent getImageFileIntent(String param) {
		Intent intent = new Intent(Intent.ACTION_VIEW);
		intent.addCategory(Intent.CATEGORY_DEFAULT);
		intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		Uri uri = Uri.fromFile(new File(param));
		intent.setDataAndType(uri, "image/*");
		return intent;
	}

	public static Intent getVideoFileIntent(String param) {
		Intent intent = new Intent(Intent.ACTION_VIEW);
		intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
		intent.putExtra("oneshot", 0);
		intent.putExtra("configchange", 0);
		Uri uri = Uri.fromFile(new File(param));
		intent.setDataAndType(uri, "video/*");
		return intent;
	}

	protected void onVisibilityChanged(View changedView, int visibility) {
		super.onVisibilityChanged(changedView, visibility);
		switch (visibility) {
		case VISIBLE:
			break;
		case GONE:
		case INVISIBLE:
			break;
		default:
			break;
		}
	}

	public void writeFilepathAndType(String filepath, int type) {
		Editor ed = mSharedPreferences.edit();
		ed.putString(LAST_THUMBNAIL, filepath);
		ed.putInt(LAST_TYPE, type);
		ed.commit();
	}

	public boolean isValidFile() {
		String path = mSharedPreferences.getString(LAST_THUMBNAIL, "");
		int type = mSharedPreferences.getInt(LAST_TYPE, -1);
		if (!path.equals("")) {
			File pt = new File(path);
			if (pt.exists()) {
				return true;
			}
		}
		return false;
	}

	public void setVisibility(int visibility) {
		if (this.getVisibility() != VISIBLE && visibility == VISIBLE) {
			String path = mSharedPreferences.getString(LAST_THUMBNAIL, "");
			int type = mSharedPreferences.getInt(LAST_TYPE, -1);
			if (!path.equals("") && !path.equals(mFilePath)) {
				showThumbnailByVisible(path, type);
			}
			this.startAnimation(mThumbnailUpAnim);
			mHandler.removeMessages(DISMISS_THUMBNAIL);
			mHandler.sendEmptyMessageDelayed(DISMISS_THUMBNAIL, 4 * 1000);
		} else if (this.getVisibility() == VISIBLE && visibility != VISIBLE) {
			mHandler.removeMessages(DISMISS_THUMBNAIL);
			this.startAnimation(mThumbnailDownAnim);
		}
		super.setVisibility(visibility);
	}

	public void resumeTimeout() {
		mHandler.removeMessages(DISMISS_THUMBNAIL);
		mHandler.sendEmptyMessageDelayed(DISMISS_THUMBNAIL, 4 * 1000);
	}

	public void setListener(ThumbnailClickListener thumbnailListener) {
		this.mThumbnailListener = thumbnailListener;
	}

	public interface ThumbnailClickListener {
		public void onThumbnailClicked(View v);
	}
}
