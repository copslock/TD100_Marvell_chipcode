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

package com.marvell.fmradio;
import android.content.Context;
import android.view.MotionEvent;
import android.view.GestureDetector.SimpleOnGestureListener;

import com.marvell.fmradio.util.LogUtil;

public class FrequencyViewListener extends SimpleOnGestureListener
{
    private final static String TAG = "RulerViewListener";
    private Context mContext = null;
    private FrequencyView mView = null;
    public static boolean on;    
    public FrequencyViewListener(Context context, FrequencyView view)
    {
        mContext = context;
        mView = view;
    }
    
    public boolean onDoubleTap(MotionEvent e)
    {
        LogUtil.i(TAG, "enter onDoubleTap");
        return true;
    }
    
    public boolean onDoubleTapEvent(MotionEvent e)
    {       
        LogUtil.i(TAG, "enter onDoubleTapEvent");
        return true;
    }
    

    public boolean onDown(MotionEvent e)
    {
        if(on)
	    mView.setPosition((int)e.getRawX());
        return true;
    }

    
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
    {
        if(on)
	    mView.setPosition((int)e2.getRawX());
        return true;
    }
    

    public void onLongPress(MotionEvent e)
    {
        LogUtil.i(TAG, "enter onLongPress");
    }
    

    public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
    {
        if(on)
        mView.setPosition((int)e2.getRawX());
        return true;
    }

    
    public void onShowPress(MotionEvent e)
    {
        
    }

    
    
    public boolean onSingleTapUp(MotionEvent e)
    {
        return true;
    }   

    public boolean onSingleTapConfirmed(MotionEvent e)
    {
        //LogUtil.i(TAG, "enter onSingleTapConfirmed RawX: " + e.getRawX() + " RawY: " + e.getRawY());
        return true;        
    }
    

}
