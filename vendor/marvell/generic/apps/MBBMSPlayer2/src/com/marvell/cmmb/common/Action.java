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

public interface Action {
	public static final String ACTION_NETWORKCONNECT_FAILED = "com.marvell.cmmb.NETWORKCONNECT_FAILED";

	public static final String ACTION_LOADING_FINISH = "com.marvell.cmmb.LOADING_FINISH";

	public static final String ACTION_MODE_CHANGED = "com.marvell.cmmb.MODE_CHANGED";

	public static final String ACTION_ESG_UPDATED = "com.marvell.cmmb.ESG_UPDATED";

	public static final String ACTION_SG_UPDATING = "com.marvell.cmmb.SG_UPDATING";

	public static final String ACTION_SG_UPDATED = "com.marvell.cmmb.SG_UPDATED";

	public static final String ACTION_PARSESG_FINISHED = "com.marvell.cmmb.PARSESG_FINISHED";

	public static final String ACTION_ST_UPDATED = "com.marvell.cmmb.ST_UPDATED";

	public static final String ACTION_PARSEST_FINISHED = "com.marvell.cmmb.PARSEST_FINISHED";

	public static final String ACTION_SCAN_START = "com.marvell.cmmb.SCAN_START";

	public static final String ACTION_SCAN_FINISHED = "com.marvell.cmmb.SCAN_FINISHED";

	public static final String ACTION_SCAN_FAILED = "com.marvell.cmmb.SCAN_FAILED";

	public static final String ACTION_GBA_INIT = "com.marvell.cmmb.GBA_INIT";

	public static final String ACTION_AUTHORIZE_STARTED = "com.marvell.cmmb.AUTHORIZE_STARTED";

	public static final String ACTION_AUTHORIZED = "com.marvell.cmmb.AUTHORIZED";

	public static final String ACTION_INQUIRIED = "com.marvell.cmmb.INQUIRIED";

	public static final String ACTION_PARSEINQUIRY_FINISHED = "com.marvell.cmmb.PARSEINQUIRY_FINISHED";

	public static final String ACTION_SUBSCRIBE_FINISH = "com.marvell.cmmb.SUBSCRIBE_FINISH";

	public static final String ACTION_UNSUBSCRIBE_FINISH = "com.marvell.cmmb.UNSUBSCRIBE_FINISH";

	public static final String ACTION_SERVICE_STARTED = "com.marvell.cmmb.SERVICE_STARTED";

	public static final String ACTION_SERVICE_DESTROED = "com.marvell.cmmb.SERVICE_DESTROED";

	public static final String ACTION_EBMESSAGE = "com.marvell.cmmb.EB_MESSAGE";

	public static final String ACTION_SIGNAL_SERVICE_DESTROY = "com.marvell.cmmb.SIGNAL_SERVICE_DESTROY_ACTION";

	public static final String ACTION_VIEW_PROGRAM_ACTION = "com.marvell.cmmb.VIEW_PROGRAM_ACTION";

	public static final String ACTION_VIEW_SCHEDULE_ACTION = "com.marvell.cmmb.VIEW_SCHEDULE_ACTION";

	public static final String ACTION_VIEW_DETAIL_ACTION = "com.marvell.cmmb.VIEW_DETAIL_ACTION";

	public static final String ACTION_VIEW_SEARCH_ACTION = "com.marvell.cmmb.VIEW_SEARCH_ACTION";

	public static final String ACTION_ON_PLAYSTATE_CHANGE = "com.marvell.cmmb.PLAYSTATE_CHANGE";

	public static final String ACTION_SG_PARSED = "com.marvell.cmmb.SGPARSED";

	public static final String ACTION_ESG_PARSED = "com.marvell.cmmb.ESGPARSED";

	public static final String ACTION_MODE_CMMB = "com.marvell.cmmb.MODE_CMMB";

	public static final String ACTION_MODE_MBBMS = "com.marvell.cmmb.MODE_MBBMS";

	public static final String ACTION_SET_APN = "com.marvell.cmmb.SET_APN";

	public static final String ACTION_SIMCARD_OUT = "com.marvell.cmmb.SIMCARD_OUT";

	public static final String ACTION_DISMISS_CANCELDIALOG = "com.marvell.cmmb.DISMISS_CANCELDIALOG";

	public static final String ACTION_VIEW_PROGRAME_BY_ID_FREQ = "com.marvell.cmmb.VIEW_PROGRAME_BY_ID_FREQ";

	public static final String ACTION_VIEW_EBMESSAGE = "com.marvell.cmmb.VIEW_EBMESSAGE";

	public static final String ACTION_LOADING_ERROR_QUIT = "com.marvell.cmmb.LOADING_ERROR_QUIT";

	public static final String ACTION_BINDER_DIED = "com.marvell.cmmb.BINDER_DIED";

	public static final String ACTION_SHOW_BINDER_DIED_DIALOG = "com.marvell.cmmb.SHOW_BINDER_DIED_DIALOG";

	public static final String ACTION_START_CONNECT_NET = "com.marvell.cmmb.START_CONNECT_NET";
	
	public static final String ACTION_BINDER_DIED_EXIT = "com.marvell.cmmb.BINDER_DIED_EXIT";
	
	public static final String ACTION_UPDATE_EXTERNAL_SG = "com.marvell.cmmb.UPDATE_EXTERNAL_SG";
	
	public static final String ACTION_RESET_FINISHED = "com.marvell.cmmb.ACTION_RESET_FINISHED";

}
