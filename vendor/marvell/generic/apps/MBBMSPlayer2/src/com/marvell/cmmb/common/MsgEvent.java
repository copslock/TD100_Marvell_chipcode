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

package com.marvell.cmmb.common;

public interface MsgEvent {
	public static final int EVENT_INIT = 1;

	public static final int EVENT_DEINIT = EVENT_INIT + 1;
	
	public static final int EVENT_RESET = EVENT_DEINIT + 1;

	public static final int EVENT_STARTUPDATEESG = EVENT_RESET + 1;

	public static final int EVENT_STARTUPDATESG = EVENT_STARTUPDATEESG + 1;

	public static final int EVENT_STARTUPDATESUBSCRIPTIONTABLE = EVENT_STARTUPDATESG + 1;

	public static final int EVENT_STARTSUBSCRIBE = EVENT_STARTUPDATESUBSCRIPTIONTABLE + 1;

	public static final int EVENT_UNSUBSCRIBE = EVENT_STARTSUBSCRIBE + 1;

	public static final int EVENT_STARTPLAYING = EVENT_UNSUBSCRIBE + 1;

	public static final int EVENT_STARTSCAN = EVENT_STARTPLAYING + 1;

	public static final int EVENT_AUTHORIZESERVICE = EVENT_STARTSCAN + 1;

	public static final int EVENT_PARSE_SG = EVENT_AUTHORIZESERVICE + 1;

	public static final int EVENT_PARSE_ST = EVENT_PARSE_SG + 1;

	public static final int EVENT_PARSE_ESG = EVENT_PARSE_ST + 1;

	public static final int EVENT_INITGBA = EVENT_PARSE_ESG + 1;

	public static final int EVENT_INQUIRY = EVENT_INITGBA + 1;

	public static final int EVENT_PARSE_INQUIRY = EVENT_INQUIRY + 1;

	public void onEventFinished(int msgEvent, int cookie);
}
