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

package com.marvell.fmmanager;
import com.marvell.fmmanager.IFMRadioCallback;
import android.os.ParcelFileDescriptor;

// Declare the interface.
interface IFMRadioService {
    // You can pass values in, out, or inout. 
    // Primitive datatypes (such as int, boolean, etc.) can only be passed in.
    boolean isFMTx();
    boolean isFMRx();
    int powerOn();
    int powerOff();
    int enableTx();
    int disableTx();
    int enableRx();
    int disableRx();
    int suspend();
    int resume();
    int scan_all();
    int stop_scan();
    int set_channel(int freq);
    int get_channel();
    int get_rssi();
    float get_volume();
    int set_volume(int volume);
    boolean set_mute(boolean flag);
    int set_band(int band);
    int scan_next();
    int scan_prev();
    int setSpeakerOn(boolean on);
    boolean set_tx_mute(boolean flag);
    boolean set_power_mode(boolean flag);
    boolean set_tx_mono_stereo(boolean flag);
    boolean start_recording(in ParcelFileDescriptor pfd);
    boolean stop_recording();
    byte[] get_record_format();
    void registerCallback(IFMRadioCallback cb);
    void unregisterCallback(IFMRadioCallback cb);
}

