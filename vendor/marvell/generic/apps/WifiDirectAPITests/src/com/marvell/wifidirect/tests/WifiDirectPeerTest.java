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

import java.util.Vector;

import android.net.wifi.MrvlBitSet;
import android.net.wifi.MrvlWifiDirectPeer;
import android.os.Parcel;
import android.util.Log;
import junit.framework.TestCase;

/**
 * Test cases to exercise the android.net.wifi.MrvlWifiDirectPeer class
 *
 * @since Marvell-0.01
 */
public class WifiDirectPeerTest extends TestCase {

    MrvlWifiDirectPeer wfdp0;

    MrvlWifiDirectPeer wfdp1;

    static final String LOG_TAG = "MrvlWifiDirectPeerTest";

    public void setUp() throws Exception {
        super.setUp();

        // Create the object 1 to be tested
        wfdp0 = new MrvlWifiDirectPeer();

        // Create object 2 to be tested
        // Set attributes with specific values
        wfdp1 = new MrvlWifiDirectPeer();
        wfdp1.SSID = "Joe";
        // deviceCapabilityBitmap set to CONFIG_METHOD_DISPLAY_VIRT (real value)
        wfdp1.deviceCapabilityBitmap.or(MrvlWifiDirectPeer.CONFIG_METHOD_DISPLAY_VIRT);
        // groupCapabilityBitmap
        wfdp1.groupCapabilityBitmap.set(1);
        wfdp1.groupCapabilityBitmap.set(6);
        wfdp1.groupCapabilityBitmap.set(11);
        wfdp1.groupCapabilityBitmap.set(16);
        // configMethods with cardinality 0
        wfdp1.primaryDeviceTypeCategory = MrvlWifiDirectPeer.DEVTYPE_PRIMARY_COMPUTER;
        // TODOMVP2P implement along with support for the full Device Type class
        wfdp1.secondaryDeviceTypeList = new Vector<Integer>();
        wfdp1.secondaryDeviceTypeList.add(new Integer(10));
        wfdp1.secondaryDeviceTypeList.add(new Integer(30));
        wfdp1.deviceName = "test";

        MrvlWifiDirectPeer wfdp2 = new MrvlWifiDirectPeer();
        wfdp2.SSID = "Moe";
        wfdp2.deviceCapabilityBitmap.or(MrvlWifiDirectPeer.CONFIG_METHOD_DISPLAY_VIRT);
        wfdp2.groupCapabilityBitmap.set(1);
        wfdp2.groupCapabilityBitmap.set(6);
        wfdp2.groupCapabilityBitmap.set(11);
        wfdp2.configMethods.set(2);
        wfdp2.configMethods.set(7);
        wfdp2.configMethods.set(12);
        wfdp2.primaryDeviceTypeCategory = wfdp1.DEVTYPE_PRIMARY_COMPUTER;
        // TODOMVP2P implement along with support for the full Device Type class
        wfdp2.secondaryDeviceTypeList = new Vector<Integer>();
        wfdp2.secondaryDeviceTypeList.add(new Integer(20));
        wfdp2.secondaryDeviceTypeList.add(new Integer(40));
        wfdp2.deviceName = "test";
    }

    public void testWriteAndReadParceledWfdPeer() {

        Parcel parcel = Parcel.obtain();

        // test the default object
        wfdp0.writeToParcel(parcel, 0);
        parcel.setDataPosition(0);
        MrvlWifiDirectPeer wfdp2 = (MrvlWifiDirectPeer) MrvlWifiDirectPeer.CREATOR
                .createFromParcel(parcel);
        comparePeers(wfdp0, wfdp2);

        // test the customized object
        parcel.setDataPosition(0);
        wfdp1.writeToParcel(parcel, 0);
        // done writing, now reset parcel for reading
        parcel.setDataPosition(0);
        // finish round trip
        wfdp2 = (MrvlWifiDirectPeer) MrvlWifiDirectPeer.CREATOR.createFromParcel(parcel);
        comparePeers(wfdp1, wfdp2);
    }

    // TODOMVP2P Move the essence of this into an equals() method in
    // MrvlWifiDirectPeer and rewrite this test to call it
    private void comparePeers(MrvlWifiDirectPeer expectedPeer, MrvlWifiDirectPeer actualPeer) {

        assertEquals(expectedPeer.SSID, actualPeer.SSID);
        assertEquals(expectedPeer.deviceCapabilityBitmap.toString(),
        actualPeer.deviceCapabilityBitmap.toString());
        assertEquals(expectedPeer.groupCapabilityBitmap.toString(),
        actualPeer.groupCapabilityBitmap.toString());
        assertEquals(expectedPeer.configMethods.toString(), actualPeer.configMethods.toString());
        assertEquals(expectedPeer.primaryDeviceTypeCategory, actualPeer.primaryDeviceTypeCategory);
        // TODOMVP2P implement along with support for the full Device Type class
        assertEquals(expectedPeer.secondaryDeviceTypeList, actualPeer.secondaryDeviceTypeList);
        assertEquals(expectedPeer.deviceName, actualPeer.deviceName);
    }

    public void testConfigMethodSimpleMask() {
        MrvlBitSet copy = (MrvlBitSet) wfdp1.deviceCapabilityBitmap.clone();
        copy.xor(MrvlWifiDirectPeer.CONFIG_METHOD_DISPLAY_VIRT);
        assertEquals(0, copy.cardinality());

        copy = (MrvlBitSet) wfdp1.deviceCapabilityBitmap.clone();
        copy.xor(MrvlWifiDirectPeer.CONFIG_METHOD_KEYPAD);
        assertFalse((copy.cardinality() == 0));
    }

    @Override
    public void tearDown() throws Exception {
        super.tearDown();
    }
}
