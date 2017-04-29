
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
 * licensors. Tme Material is protected by worldwide copyright and trade secret
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


package com.marvell.fmradio;
import android.content.Context;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.graphics.Paint.Align;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.Log;

import android.view.Display;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.WindowManager;
import android.view.GestureDetector.SimpleOnGestureListener;
import android.view.ViewGroup.LayoutParams;
import android.widget.AbsoluteLayout;
import android.widget.ImageView;
import android.widget.TextView;

import com.marvell.fmradio.util.ChannelHolder;
import com.marvell.fmradio.util.LogUtil;


public class FrequencyView extends ImageView {
	private final static String TAG = "FrequencyView";      

	public final static int STATE_LEFT1 = 0;
	public final static int STATE_LEFT2 = 1;
	public final static int STATE_MID = 2;
	public final static int STATE_RIGHT1 = 3;
	public final static int STATE_RIGHT2 = 4;
	private final static int MSG_SETCHANNEL = 9;
	public final static double percentage = 0.7;
	private ChannelHolder mChannelHolder = null;    
	private Paint mTextPaint = null;
	private String mName = "";

	private int mState = STATE_LEFT1;    
	private int mPosition = 0;
	private int mPositionMove = 0;
	private int mFrequency = 0;
	private int mScrollBarWdith = 0;
	private int mScrollBarHeight = 0;
	private int screenWidth = 320;  

	private Drawable mDrawableLeft1 = null;
	private Drawable mDrawableLeft2 = null;
	private Drawable mDrawableMid = null;
	private Drawable mDrawableRight1 = null;
	private Drawable mDrawableRight2 = null;

	private GestureDetector mGestureDetector = null;
	private SimpleOnGestureListener mGestureListener = null;

	private Handler mHandler = null;
	private int mMsgID = 0;


	public FrequencyView(Context context) {
		super(context);
		init(context);
	}

	public FrequencyView(Context context, AttributeSet attrs) {
		super(context, attrs);      
		init(context);
	}

	public FrequencyView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		init(context);  
	}

	public void setHandler(Handler handler, int msgid)
	{
		mHandler = handler;
		mMsgID = msgid;
	}

	private void init(Context context)
	{       
		mChannelHolder = ChannelHolder.getInstance();
		mTextPaint = new Paint(); 
		mTextPaint.setLinearText(false);
		mTextPaint.setAntiAlias(true);
		mTextPaint.setSubpixelText(false);
		mTextPaint.setColor(Color.WHITE);
		mTextPaint.setTextSize(12); 
		mDrawableLeft1 = context.getResources().getDrawable(R.drawable.box_1);
		mDrawableLeft2 = context.getResources().getDrawable(R.drawable.box_2);
		mDrawableMid = context.getResources().getDrawable(R.drawable.box_3);
		mDrawableRight1 = context.getResources().getDrawable(R.drawable.box_4);
		mDrawableRight2 = context.getResources().getDrawable(R.drawable.box_5);

		mGestureListener = new FrequencyViewListener(context, this);     
		mGestureDetector = new GestureDetector(context, mGestureListener);
	}


	@Override
		protected void onDraw(Canvas canvas)
		{
		screenWidth = MainActivity.screenWidth;
		if(mPosition == 0){
			mPosition = screenWidth/10;
		}
			canvas.save();

			drawContainer(canvas);
			drawName(canvas,mTextPaint);
                        drawRssi(canvas,mTextPaint);
			drawFrequency(canvas,mTextPaint);

			drawBall(canvas,mTextPaint);
			drawBack(canvas,mTextPaint);
			canvas.restore();
		}

	public boolean onTouchEvent(MotionEvent event)
	{
		if (mGestureDetector.onTouchEvent(event))
		{
			return true;
		}

		return super.onTouchEvent(event);
	}

	private void drawName(Canvas canvas, Paint paint)
	{
		Bitmap bitmap = Bitmap.createBitmap(200, 25, Bitmap.Config.ARGB_8888);
		bitmap = drawTextAtBitmap(bitmap,mName);
		canvas.drawBitmap(bitmap, mPositionMove,15, paint);

	}
private void drawRssi(Canvas canvas, Paint paint)
	    {
	  if(mChannelHolder.rssi<180){
	        MainActivity.rssi.setBackgroundResource(R.drawable.xinhao_00);

			}
		else if(180<=mChannelHolder.rssi&&mChannelHolder.rssi<190){

		MainActivity.rssi.setBackgroundResource(R.drawable.xinhao_01);

		}
		else if(190<=mChannelHolder.rssi&&mChannelHolder.rssi<200){

		MainActivity.rssi.setBackgroundResource(R.drawable.xinhao_02);

		}
		else if(200<=mChannelHolder.rssi&&mChannelHolder.rssi<210){

	        MainActivity.rssi.setBackgroundResource(R.drawable.xinhao_03);

		}
		else if(210<=mChannelHolder.rssi&&mChannelHolder.rssi<220){

		MainActivity.rssi.setBackgroundResource(R.drawable.xinhao_04);

		}
		else if(220<=mChannelHolder.rssi){

		MainActivity.rssi.setBackgroundResource(R.drawable.xinhao_05);

		}
	    }
	private void drawFrequency(Canvas canvas, Paint paint)
	{
		int dev=mFrequency/100;

		if(dev/1000>=1)
		{ 
			Bitmap bitmap = BitmapFactory.decodeResource(getResources(), getMap(1));
			canvas.drawBitmap(bitmap, mPositionMove+0, 60, paint);
			Bitmap bitmap0 = BitmapFactory.decodeResource(getResources(), getMap(0));
			canvas.drawBitmap(bitmap0, mPositionMove+25, 60, paint);
		} else {
			Bitmap bitmap = BitmapFactory.decodeResource(getResources(), getMap(dev/100));
			canvas.drawBitmap(bitmap, mPositionMove+25, 60, paint);
		}

		Bitmap bitmap1 = BitmapFactory.decodeResource(getResources(), getMap((dev%100)/10));
		canvas.drawBitmap(bitmap1, mPositionMove+55, 60, paint);
		Bitmap bitmap2 = BitmapFactory.decodeResource(getResources(), getMap(10));
		canvas.drawBitmap(bitmap2, mPositionMove+90, 90, paint);
		Bitmap bitmap3 = BitmapFactory.decodeResource(getResources(), getMap((dev%100)%10));
		canvas.drawBitmap(bitmap3, mPositionMove+100, 60, paint);
		Bitmap bitmap4 = BitmapFactory.decodeResource(getResources(), R.drawable.mhz);
		canvas.drawBitmap(bitmap4, mPositionMove+135, 75, paint);
	}

	private void drawBall(Canvas canvas, Paint paint)
	{

		Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.ball);
		Bitmap bitmaplight = BitmapFactory.decodeResource(getResources(), R.drawable.ball_light);
		Bitmap line = BitmapFactory.decodeResource(getResources(), R.drawable.beijing_line);
		canvas.drawBitmap(bitmaplight, mPosition, 154, paint);
		canvas.drawBitmap(bitmap, screenWidth/10, 160, paint);
		canvas.drawBitmap(bitmap, new Float(percentage*screenWidth*(3.5/21)+screenWidth/10), 160, paint);
		canvas.drawBitmap(bitmap, new Float(percentage*screenWidth*(7.5/21)+screenWidth/10), 160, paint);
		canvas.drawBitmap(bitmap, new Float(percentage*screenWidth*(11.5/21)+screenWidth/10), 160, paint);
		canvas.drawBitmap(bitmap, new Float(percentage*screenWidth*(16.5/21)+screenWidth/10), 160, paint);
		canvas.drawBitmap(bitmap, new Float(percentage*screenWidth+screenWidth/10), 160, paint);
		canvas.drawBitmap(line, 0, 160, paint);
	}
	private void drawBack(Canvas canvas, Paint paint)
	{
		Bitmap start = BitmapFactory.decodeResource(getResources(), R.drawable.start);
		Bitmap f1 = BitmapFactory.decodeResource(getResources(), R.drawable.f1);
		Bitmap f2 = BitmapFactory.decodeResource(getResources(), R.drawable.f2);
		Bitmap f3 = BitmapFactory.decodeResource(getResources(), R.drawable.f3);
		Bitmap f4 = BitmapFactory.decodeResource(getResources(), R.drawable.f4);
		Bitmap end2 = BitmapFactory.decodeResource(getResources(), R.drawable.end2);
		canvas.drawBitmap(start, screenWidth/10, 170, paint);
		canvas.drawBitmap(f1, new Float(percentage*screenWidth*(3.5/21)+screenWidth/10), 170, paint);
		canvas.drawBitmap(f2, new Float(percentage*screenWidth*(7.5/21)+screenWidth/10), 170, paint);
		canvas.drawBitmap(f3, new Float(percentage*screenWidth*(11.5/21)+screenWidth/10), 170, paint);
		canvas.drawBitmap(f4, new Float(percentage*screenWidth*(16.5/21)+screenWidth/10), 170, paint);
		canvas.drawBitmap(end2, new Float(percentage*screenWidth+screenWidth/10), 170, paint);
	}
	private int getMap(int i){
		switch (i){
			case 0:
				return R.drawable.n0;
			case 1:
				return R.drawable.n1;
			case 2:
				return R.drawable.n2;
			case 3:
				return R.drawable.n3;
			case 4:
				return R.drawable.n4;
			case 5:
				return R.drawable.n5;
			case 6:
				return R.drawable.n6;
			case 7:
				return R.drawable.n7;
			case 8:
				return R.drawable.n8;
			case 9:
				return R.drawable.n9;
			case 10:
				return R.drawable.dian;
			default:
				return 0;
		}
	}

	private void drawContainer(Canvas canvas)
	{
		Rect r = new Rect();
		r.left = mPosition ;
		r.top = 0;
		r.right = mPosition + 220;
		r.bottom = r.top + 150;

		switch (mState){
			case STATE_LEFT1:
				mPositionMove=mPosition;
				mDrawableLeft1.setBounds(r);
				mDrawableLeft1.draw(canvas);
				break;

			case STATE_LEFT2:
				mPositionMove=mPosition-40;
				r.left=mPositionMove;
				r.right = mPositionMove + 220;
				mDrawableLeft2.setBounds(r);
				mDrawableLeft2.draw(canvas);
				break
					;
			case STATE_MID:
				mPositionMove=mPosition-90;
				r.left=mPositionMove;
				r.right = mPositionMove + 220;
				mDrawableMid.setBounds(r);
				mDrawableMid.draw(canvas);
				break;

			case STATE_RIGHT1:
				mPositionMove=mPosition-140;
				r.left=mPositionMove;
				r.right = mPositionMove+220;
				mDrawableRight1.setBounds(r);
				mDrawableRight1.draw(canvas);
				break;

			case STATE_RIGHT2:
				mPositionMove=mPosition-170;
				r.left=mPositionMove;
            r.right = mPositionMove+220;
            mDrawableRight2.setBounds(r);
            mDrawableRight2.draw(canvas);
            break;

	default:
	    break;
        }
    }
    
    public void setFrequency(int value)
    {
        mFrequency = value;
        mPosition = (value - 87500) * (int)(percentage*(MainActivity.screenWidth)) / (108500 - 87500)+MainActivity.screenWidth/10;
        setState(mPosition);
        invalidate();
    }
    
    public void setName(String name)
    {
        mName=name;
        invalidate();
    }
    
    
    public void setState(int pos)
    {
	int w = (int)(percentage*screenWidth);
	if(0<=pos&&pos<=w/5)
        mState = STATE_LEFT1;
	else if(w/5<pos&&pos<=w*2/5)
        mState = STATE_LEFT2;
        else if(w*2/5<pos&&pos<=w*13/20)
        mState = STATE_MID;
        else if(w*13/20<pos&&pos<=w*4/5)
        mState = STATE_RIGHT1;
        else if(w*4/5<pos)
        mState = STATE_RIGHT2;
        invalidate();
    }
    
    public void setPosition(int pos)
    {
        if(getLeft()+screenWidth/10<=pos&&pos<=getLeft()+percentage*screenWidth+screenWidth/10){
            if(mChannelHolder.getCount() != 0){
                mPosition = pos-getLeft();
            }
            setState(mPosition);
            updateRuler();
        }
    }   
    
    private void updateRuler()
    {
        
        int freq = 87500 + (mPosition-screenWidth/10) * (108000 - 87500) / (int)(percentage*screenWidth);
        
        freq = (freq / 100) * 100;
	int swap = freq;
	if(mChannelHolder.getCount() == 0){
		return;
	}
	for (int pos = mChannelHolder.getCount() - 1 ; 0 <= pos ; pos--)
	{

		int a = Integer.valueOf(mChannelHolder.getItem(pos).mFreq);
		if(a<freq){
			 if(pos==mChannelHolder.getCount() - 1)
			{
			freq = a;
			break;
			}
			else{

int b = Integer.valueOf(mChannelHolder.getItem(pos+1).mFreq);
if((freq-a)<=(b-freq))
	freq = a;
else
	freq = b;
break;
		}
		}
	}
	if(freq == swap){

		freq =  Integer.valueOf(mChannelHolder.getItem(0).mFreq);
	}
        LogUtil.d(TAG, "freq: " + freq + " KHz");
        mChannelHolder.mWorkFreq=freq;
    mHandler.sendMessage(Message.obtain(mHandler, MSG_SETCHANNEL)); 
    }
    
    private Bitmap drawTextAtBitmap(Bitmap bitmap,String text){
        int x = bitmap.getWidth();  
        int y = bitmap.getHeight();  
		                    
        Bitmap newbit = Bitmap.createBitmap(x, y, Bitmap.Config.ARGB_8888);  
        Canvas canvas = new Canvas(newbit);  
        Paint paint = new Paint();  
	canvas.drawBitmap(bitmap, 0, 0, paint);  
	paint.setColor(Color.parseColor("#dedbde"));  
	paint.setTextSize(20);  
	paint.setTextAlign(Align.CENTER);
	canvas.drawText(text, x>>1 , 15, paint);  
											            
	return newbit; 

    }
}
