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

import android.net.wifi.MrvlWuControllerWifiDirect;
import junit.framework.TestCase;

/**
 * Test cases to exercise the android.net.wifi.MrvlWuControllerWifiDirectTest
 * class
 *
 * @since Marvell-0.01
 */
public class MrvlWuControllerWifiDirectTest extends TestCase {

    // Declare variables ()
    // MrvlWuControllerWifiDirect controller;

    static final String TAG = "MrvlWuControllerWifiDirectTest";

    public void setUp() throws Exception {
        super.setUp();

        // Create the object to be tested (if necessary )
        // MrvlWuControllerWifiDirect controller = new
        // MrvlWuControllerWifiDirect();
    }

    public void testRandomWifiDirectPin() {

        String pin = MrvlWuControllerWifiDirect.getRandomWifiDirectPin();
        // implement the test
        // assertEquals(expected, actual);
        // etc.
    }

    @Override
    public void tearDown() throws Exception {
        super.tearDown();
    }
}
