package com.marvell.cmmb.view.customview;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.drawable.Drawable;
import android.os.Handler;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.SurfaceView;
import android.view.View;
import android.view.ScaleGestureDetector.OnScaleGestureListener;
import android.view.View.OnClickListener;
import android.view.View.OnLongClickListener;
import android.view.View.OnTouchListener;
import android.view.animation.Animation;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.marvell.cmmb.R;
import com.marvell.cmmb.aidl.LogMask;
import com.marvell.cmmb.common.Define;
import com.marvell.cmmb.service.MBBMSService.State;

public class PlayFrameView extends FrameLayout implements OnClickListener,
		OnTouchListener, OnLongClickListener {

	private static final String TAG = "PlayFrameView";

	private ImageView mCMMBLogo;

	private LoadingView mLoadView;

	private SurfaceView mSurfaceView;

	private TextView mOpenPip;

	private TextView mClosePip;

	private boolean isBig = true;

	private ScrollListener mScrollListener;

	private PipListener mPipListener;

	private int mTouchDownPosition;

	private static int SCROLL_DISTANCE = 15;

	private State mCurrentState;

	private State mPrevState;

	public static final int DOUBLE_CLICK = 1;

	public static final int DISMISS_INDEX = DOUBLE_CLICK + 1;

	public static final int ANIMATION_END = DISMISS_INDEX + 1;

	private static final long DOUBLE_CLICK_TIME = 500;

	private static final long SHOW_INDEX_TIME = 6 * 1000;

	private int mClickCounts = 0;

	private boolean mEnable = true;

	private TextView mIndex;

	private RecTime mRecTime;

	private ScaleGestureDetector mScaleGestureDetector;

	private OnScaleListener mOnScaleListener;

	private Context mContext;

	private boolean mIsGesture;// to indicate whether it is switch frame gesture

	private final static int GESTURE_DISTANCE_ZOOM_IN = -50;

	private final static int GESTURE_DISTANCE_ZOOM_OUT = 20;

	private boolean isInMoveMode;

	private boolean isLOGOsetted;

	public Handler mHandler = new Handler() {
		public void handleMessage(android.os.Message msg) {
			switch (msg.what) {
			case DOUBLE_CLICK:
				if (mClickCounts >= 2) {
					if (mScrollListener != null)
						mScrollListener.onFrameDoubleClick(PlayFrameView.this);
				} else {
					if (mScrollListener != null)
						mScrollListener.onFrameClick(PlayFrameView.this);
				}
				mClickCounts = 0;
				break;
			case DISMISS_INDEX:
				if (null != mIndex) {
					mIndex.setVisibility(View.GONE);
				}
				break;
			case ANIMATION_END:

				break;
			}

		};
	};

	public PlayFrameView(Context context) {
		super(context);
		mContext = context;
	}

	public PlayFrameView(Context context, AttributeSet attrs) {
		super(context, attrs);
		mContext = context;
	}

	protected void onFinishInflate() {
		super.onFinishInflate();
		findItem();
		setUpViews();
	}

	private void setUpViews() {
		mLoadView.setOnTouchListener(this);
		mCMMBLogo.setOnTouchListener(this);
		mSurfaceView.setOnTouchListener(this);
		mLoadView.setOnLongClickListener(this);
		mCMMBLogo.setOnLongClickListener(this);
		mSurfaceView.setOnLongClickListener(this);
		changeState(State.PlaybackStoped);
		mScaleGestureDetector = new ScaleGestureDetector(mContext, listener);

	}

	private void findItem() {
		mLoadView = (LoadingView) findViewById(R.id.loading);
		mCMMBLogo = (ImageView) findViewById(R.id.iv_cmmb);
		mSurfaceView = (SurfaceView) findViewById(R.id.surface);
		mIndex = (TextView) findViewById(R.id.tv_index);
		mRecTime = (RecTime) findViewById(R.id.tv_recordtime);
	}

	public SurfaceView getSurfaceView() {
		return mSurfaceView;
	}

	public ImageView getCMMBLogView() {
		return mCMMBLogo;
	}

	public LoadingView getLoadView() {
		return mLoadView;
	}

	public void onClick(View v) {
		if (!mEnable) {
			return;
		}
	}

	private void changeToPip() {
		if (null != mPipListener)
			mPipListener.changeToPIP(this);
	}

	private void closePip() {
		if (null != mPipListener)
			mPipListener.closePip(this);

	}

	private float x = 0.0f;
	private float y = 0.0f;
	private boolean isFirstMove = true;

	private int mTOuchDownYPosition;

	public boolean onTouch(View v, MotionEvent event) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onTouch event is "
				+ event.toString(), LogMask.APP_COMMON);
		if (!mEnable) {
			return false;
		}
		if (isInMoveMode) {
			switch (event.getAction()) {
			case MotionEvent.ACTION_DOWN:
				break;
			case MotionEvent.ACTION_UP:
				this.setBackgroundResource(R.drawable.pipbg);
				x = 0.0f;
				y = 0.0f;
				isFirstMove = true;
				isInMoveMode = false;
				break;
			case MotionEvent.ACTION_MOVE:
				float moveMentX = 0f;
				float moveMentY = 0f;

				if (isFirstMove) {
					moveMentX = event.getX() - mTouchDownPosition;
					moveMentY = event.getY() - mTOuchDownYPosition;
				} else {
					moveMentX = event.getX() - x;
					moveMentY = event.getY() - y;
				}
				x = event.getX();
				y = event.getY();
				if (null != mScrollListener) {
					mScrollListener.onFrameMove(this, moveMentX, moveMentY);
				}
				break;
			}
			return false;
		}
		switch (event.getAction()) {
		case MotionEvent.ACTION_DOWN:
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "action down",
					LogMask.APP_COMMON);
			mTouchDownPosition = (int) event.getX();
			mTOuchDownYPosition = (int) event.getY();
			break;
		// return true;
		case MotionEvent.ACTION_UP:
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"action up, Scale gesture is " + mIsGesture,
					LogMask.APP_COMMON);
			if (mIsGesture) {
				setGestureFlag(false);// Gesture finish ,reset the flag
			} else {
				int touchUpPosition = (int) event.getX();
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "action up",
						LogMask.APP_COMMON);
				if (touchUpPosition < mTouchDownPosition - SCROLL_DISTANCE) {
					// scroll to right
					if (this.mScrollListener != null)
						this.mScrollListener.onScrollRight(this);
				} else if (touchUpPosition - SCROLL_DISTANCE > mTouchDownPosition) {
					// scroll to left, switch to left channel
					if (this.mScrollListener != null)
						this.mScrollListener.onScrollLeft(this);
				} else if (Math.abs(touchUpPosition - mTouchDownPosition) < SCROLL_DISTANCE) {
					mClickCounts++;
					if (!mHandler.hasMessages(DOUBLE_CLICK)) {
						mHandler.sendEmptyMessageDelayed(DOUBLE_CLICK, DOUBLE_CLICK_TIME);
					}
				}
			}
			break;
		// return true;
		}
		mScaleGestureDetector.onTouchEvent(event);
		return super.onTouchEvent(event);
	}

	public void setScrollListener(ScrollListener l) {
		this.mScrollListener = l;
	}

	public void setPipListener(PipListener l) {
		this.mPipListener = l;
	}

	/**
	 * if it is the main playback frame
	 * 
	 * @param isBig
	 */
	public void setMain(boolean isBig) {
		this.isBig = isBig;
		// changePipButton();
		setIndexSize();
		setPadding();
	}

	private void setIndexSize() {
		if (mIndex != null) {
			if (isBig) {
				mIndex.setTextSize(40);
				mLoadView.setTitleSize(22);
			} else {
				mIndex.setTextSize(20);
				mLoadView.setTitleSize(12);
			}
		}
	}

	private void setPadding() {
		if (isBig) {
			this.setPadding(0, 0, 0, 0);
			this.setBackgroundColor(R.color.solid_black);
		} else {
			this.setPadding(3, 3, 7, 3);
			this.setBackgroundResource(R.drawable.pipbg);
		}
	}

	public void changePipButton() {
		if (isBig) {
			mOpenPip.setVisibility(VISIBLE);
			mClosePip.setVisibility(GONE);
		} else {
			mOpenPip.setVisibility(GONE);
			mClosePip.setVisibility(VISIBLE);
		}
	}

	/**
	 * enable or disable the PIP button
	 * 
	 * @param state
	 *            the playback state
	 * 
	 */
	public void changeState(State state) {
		mCurrentState = state;
		if (state == State.PlaybackLoading) {
			mLoadView.setVisibility(View.VISIBLE);
			mSurfaceView.setVisibility(View.GONE);
			mCMMBLogo.setVisibility(View.GONE);
		} else if (state == State.PlaybackPlaying) {
			mSurfaceView.setVisibility(View.VISIBLE);
			mCMMBLogo.setVisibility(View.GONE);
			mLoadView.setVisibility(View.GONE);
		} else if (state == State.PlaybackStoped) {
			mCMMBLogo.setVisibility(View.VISIBLE);
			mSurfaceView.setVisibility(View.GONE);
			mLoadView.setVisibility(View.GONE);
		}
	}

	public View getPipButton() {
		return isBig ? mOpenPip : mClosePip;
	}

	public void enableAll(boolean enable) {
		this.mEnable = enable;
	}

	public void showIndex(int index, long time) {
		if (mIndex != null) {
			mIndex.setText(String.valueOf(index));
			mIndex.setVisibility(View.VISIBLE);
			mHandler.removeMessages(DISMISS_INDEX);
			if (time != 0) {
				mHandler.sendEmptyMessageDelayed(DISMISS_INDEX, time);
			} else {
				mHandler.sendEmptyMessageDelayed(DISMISS_INDEX, SHOW_INDEX_TIME);
			}
		}
	}

	OnScaleGestureListener listener = new OnScaleGestureListener() {
		public void onScaleEnd(ScaleGestureDetector detector) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"onScaleEnd detector.getCurrentSpan() is "
							+ detector.getCurrentSpan(), LogMask.APP_COMMON);
		}

		public boolean onScaleBegin(ScaleGestureDetector detector) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"onScaleBegin detector.getCurrentSpan() is "
							+ detector.getCurrentSpan(), LogMask.APP_COMMON);
			setGestureFlag(true);
			return true;
		}

		public boolean onScale(ScaleGestureDetector detector) {
			float cur = detector.getCurrentSpan();
			float pre = detector.getPreviousSpan();
			float cp = cur - pre;
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"onScale detector.getCurrentSpan() is " + cur
							+ " detector.getPreviousSpan() is " + pre
							+ " cur - pre = " + (cur - pre) + " isBig = "
							+ isBig, LogMask.APP_COMMON);
			if (cp > GESTURE_DISTANCE_ZOOM_OUT && !isBig) {
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "change to main",
						LogMask.APP_COMMON);
				mOnScaleListener.onScaled();

			} else if (cp < GESTURE_DISTANCE_ZOOM_IN && isBig) {
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "change to vice",
						LogMask.APP_COMMON);
				mOnScaleListener.onScaled();

			}
			return false;
		}
	};

	private void setGestureFlag(boolean isGesture) {
		mIsGesture = isGesture;

	}

	public void setOnScaleListener(OnScaleListener l) {
		mOnScaleListener = l;
	}

	public RecTime getRecTime() {
		return mRecTime;
	}

	public interface ScrollListener {
		/**
		 * when touch the frame, it will be called
		 */
		public void onFrameClick(View v);

		/**
		 * when double click the frame, it will be called
		 */
		public void onFrameDoubleClick(View v);

		/**
		 * touch the frame from left to right, it will be called
		 */
		public void onScrollRight(View v);

		/**
		 * touch the frame from right to left, it will be called
		 */
		public void onScrollLeft(View v);

		/**
		 * after long click, when move on touch, it will be called
		 * 
		 * @param v
		 *            the view touched
		 * @param x
		 *            the x point
		 * @param y
		 *            the y point
		 */
		public void onFrameMove(View v, float x, float y);
	}

	public interface PipListener {
		/**
		 * change to PIP
		 */
		public void changeToPIP(View v);

		/**
		 * close PIP
		 */
		public void closePip(View v);
	}

	public interface OnScaleListener {
		public void onScaled();
	}

	public boolean onLongClick(View v) {
		LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(), "onLongClick",
				LogMask.APP_COMMON);
		if (!isBig) {
			this.setBackgroundResource(R.drawable.pipbgon);
			isInMoveMode = true;
			mClickCounts = 0;
			mHandler.removeMessages(DOUBLE_CLICK);
		}
		return false;
	}

	public void startSwitchAnimation(Animation animation, State state) {
		mPrevState = state;
		if (state == State.PlaybackLoading) {
			mLoadView.setVisibility(View.VISIBLE);
			mCMMBLogo.setVisibility(View.GONE);
		} else {
			mLoadView.setVisibility(View.GONE);
			mCMMBLogo.setVisibility(View.VISIBLE);
		}
		this.startAnimation(animation);
	}

	public void setCMMBLOGO(Bitmap bm) {
		if (bm != null)
			mCMMBLogo.setImageBitmap(bm);
		isLOGOsetted = true;
	}

	public void resetCMMBLOGO() {
		if (isLOGOsetted) {
			mCMMBLogo.setImageBitmap(null);
			mCMMBLogo.setImageResource(R.drawable.mbbms);
			isLOGOsetted = false;
		}
	}
	
	public State getCurrentState() {
		return mCurrentState;
	}
}
