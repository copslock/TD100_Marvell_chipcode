/*
 * Copyright (C) 2011 Marvell International Ltd., All Rights Reserved
 *
 * MARVELL CONFIDENTIAL
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

package com.marvell.wifidirect.apitest.tests;

import android.net.wifi.MrvlBitSet;
import android.net.wifi.MrvlWifiDirectPeer;
import android.os.Parcel;
import junit.framework.TestCase;

/**
 * Test cases to exercise the android.net.wifi.MrvlWuControllerWifiDirectTest
 * class
 *
 * @since Marvell-0.01
 */
public class MrvlBitSetTest extends TestCase {

    MrvlBitSet bitSet0;
    MrvlBitSet bitSet1;

    static final String TAG = "MrvlBitSetTest";

    public void setUp() throws Exception {
        super.setUp();
        bitSet0 = new MrvlBitSet();
        bitSet1 = MrvlWifiDirectPeer.CONFIG_METHOD_PBC;
    }

    public void testWriteAndReadMrvlBitSet() {

        Parcel parcel = Parcel.obtain();

        //test default object
        bitSet0.writeToParcel(parcel, 0);
        parcel.setDataPosition(0);
        MrvlBitSet readBitSet = (MrvlBitSet) MrvlBitSet.CREATOR.createFromParcel(parcel);
        assertEquals(bitSet0, readBitSet);

        //test the customized object
        parcel.setDataPosition(0);
        bitSet1.writeToParcel(parcel, 0);
        parcel.setDataPosition(0);
        readBitSet = (MrvlBitSet) MrvlBitSet.CREATOR.createFromParcel(parcel);
        assertEquals(bitSet1, readBitSet);
    }

      @Override
    public void tearDown() throws Exception {
        super.tearDown();
    }
}
