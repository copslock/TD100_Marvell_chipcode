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

import com.marvell.cmmb.aidl.MBBMSEngine;

public interface Define {
	public static final String CMMBSETING = "CMMB_SETTING";

	public static final int UNPURCHASED = 0;

	public static final int PURCHASED = 1;

	public static final int DELETE_FLAG_TRUE = 1;// never change the value

	public static final int PROGRAM_PER_TIME = 128;// never change the value

	public static final int CHANNEL_PER_MONTH = 129;// never change the value

	public static final int CHANNELSET_PER_MONTH = 130;// never change the value

	public static final int CHANNELSET_MULTI_MONTH = 131;// never change the value

	public static final int ERROR_UAM_AUTHENTICATION_INVALID = 130;// never change the value

	public static final int ERROR_DEMUX_LOST_FRAME = 120;// never change the value

	public static final int ERROR_DEMUX_DATA_CRC_ERROR = 121;// never change the value

	public static final int ERROR_UNKNOWN = 100;// never change the value

	public static final int ERROR_CMMB_MODULE_OTHER_ERROR = 110;// never change the value

	public static final int ERROR_CMMB_MODULE_CAN_NOT_OPEN = 111;// never change the value

	public static final int ERROR_CMMB_MODULE_NOT_DETECTED = 112;// never change the value

	public static final int ERROR_CMMB_MODULE_FIRMWARE_DOWNLOAD_FAILED = 113;// never change the value

	public static final int ERROR_PATH_NOT_EXIST = 140;// never change the value

	public static final int ERROR_PATH_NO_PERMISSION = 141;// never change the value

	public static final int ERROR_MSG_REC_SDCARD_NOT_FOUND = 151;

	public static final int ERROR_MSG_REC_STORAGE_FULL = 152;

	public static final int ERROR_MSG_REC_EXCEED_MAX_SIZE = 153;

	public static final int ERROR_MSG_REC_UNKNOWN_ERROR = 154;

	public static final int INFO_MSG_CAP_SUCCESS = 161;

	public static final int ERROR_MSG_CAP_ERROR = 162;

	public static final int MESSAGE_ON_ERROR = 42;

	public static final int TOAST_TYPE_PLAYEXC = 43;

	public static final int TOAST_TYPE_WEEK_SIGNAL = 44;

	public static final int TOAST_TYPE_WAIT_SYNC = 45;

	public static final int MESSAGE_INQUIRY_SUCCESS = 46;

	public static final int MESSAGE_INQUIRY_FAIL = 47;

	public static final int MESSAGE_TS0_PREPARED = 48;

	public static final int MESSAGE_EBMESSAGE_RECEIVED = 49;

	public static final int MESSAGE_INQUIRY_FINISHED = 50;

	public static final int MESSAGE_INDEX_DISMISS = 51;

	public static final int SERVICE_SWITCHING = 1;// preview usage type

	public static final int SERVICE_GUIDE = 2;// preview usage type

	public static final int SERVICE_PREVIEW = 3;// preview usage type

	public static final int BAKER = 4;// preview usage type

	public static final int MESSAGE_IMAGE_PATH = 52; // notify the captured image path

	public static final int TOAST_IMAGE_PATH = 53;

	public static final int DIALOG_PROMPT_NO_SDCARD = 54;
	
	public static final int STARTUP = 55;

	public static final int AUTHORIZE = 56;

	public static final int AUTOREFRESH = 57;

	public static final int MANUALREFRESH = 58;

	public static final int INQUIRE = 59;

	public static final int SUBSCRIBE = 60;

	public static final int UNSUBSCRIBE = 61;

	public static final int UPDATESG = 62;

	public static final int UPDATEST = 63;

	public static final int INITGBA = 64;
	
	
	public static final int DEVICE_AUTHENTICATION_FAILED = 1;
	public static final int PURCHASE_ITEM_UNKNOWN = 3;
	public static final int SERVER_ERROR = 7;
	public static final int MAL_FORMED_MESSAGE_ERROR = 8;
	public static final int NO_SUBSCRIPTION = 10;
	public static final int OPERATION_NOT_PERMITTED = 11;
	public static final int REQUESTED_SERVICE_UNAVALIABLE = 15;
	public static final int REQUESTED_SERVICE_ACCEPTED = 16;
	public static final int INVALID_REQUEST = 22;
	public static final int DELIVERY_WRONG_KEY_INFO = 24;
	public static final int ALREADY_IN_USE = 27;
	public static final int NO_MATCH_FRAGMENT = 28;
	public static final int NOW_SUBSCRIBED = 29;
	public static final int INSUFFICIENT_CHARGE = 131;

	/**
	 * The MBBMS mode
	 */
	public static final int MBBMS_MODE = MBBMSEngine.CMMode.MBBMS_MODE
			.ordinal();

	/**
	 * The CMMB mode
	 */
	public static final int CMMB_MODE = MBBMSEngine.CMMode.CMMB_MODE.ordinal();

	public static final int DEFAULT_MODE = 3;

	public static final String INTENT_MESSAGE_NETID = "ebmsg_netid";

	public static final String INTENT_MESSAGE_NETLEVEL = "ebmsg_netlevel";

	public static final String INTENT_MESSAGE_MSGID = "ebmsg_msgid";

	public static final String INTENT_MESSGAE_DATE = "ebmsg_date";

	public static final String INTENT_MESSGAE_TIME = "ebmsg_time";

	public static final String INTENT_MESSGAE_LEVEL = "ebmsg_msglevel";

	public static final String INTENT_MESSGAE_TEXT = "ebmsg_msginfo";

	public static final String INTENT_CHANNEL_POSITION = "ChannelPosition";

	public static final String INTENT_SCHEDULE_POSITION = "SchedulePosition";

	public static final String INTENT_DETAIL_POSITION = "ChooseChannel";

	public static final String INTENT_SEARCH_POSITION = "Channel";

	public static final String INTENT_CHANNEL_ID_FREQ = "ChannelIDAndReq";

	public static final String INTENT_CHANNEL_SERVICEID = "ServiceID";

	public static final String INTENT_CHANNEL_FREQ = "Freq";

	public static final String ACTION_EBMESSAGE = "com.marvell.cmmb.EB_MESSAGE";

	public static final String ACTION_SIGNAL_SERVICE_DESTROY = "com.marvell.cmmb.SIGNAL_SERVICE_DESTROY_ACTION";

	public static final int MESSAGE_HIDE_MENU = 1;

	public static final int MESSAGE_LOADING = MESSAGE_HIDE_MENU + 1;// use to switch image
	
	public static final int MESSAGE_SET_MODE = MESSAGE_LOADING + 1;

	public static final int MESSAGE_UPDATED_SG_ESG = MESSAGE_SET_MODE + 1;// updated service guide

	public static final int MESSAGE_UPDATE_ST = MESSAGE_UPDATED_SG_ESG + 1;// update subscriber table

	public static final int MESSAGE_PARSEESG_FINISHED = MESSAGE_UPDATE_ST + 1;

	public static final int MESSAGE_UPDATE_TS0 = MESSAGE_PARSEESG_FINISHED + 1;

	public static final int MESSAGE_ERROR_SG_ESG = MESSAGE_UPDATE_TS0 + 1;

	public static final int MESSAGE_ERROR_ST = MESSAGE_ERROR_SG_ESG + 1;

	public static final int MESSAGE_FINISH_LOADING = MESSAGE_ERROR_ST + 1;

	public static final int MESSAGE_UNSUBSCRIBE = MESSAGE_FINISH_LOADING + 1;

	public static final int MESSAGE_UPDATE_ERROR = MESSAGE_UNSUBSCRIBE + 1;

	public static final int MESSAGE_SUB_FILE_READY = MESSAGE_UPDATE_ERROR + 1;

	public static final int MESSAGE_SG_ESG_READY = MESSAGE_SUB_FILE_READY + 1;

	public static final int MESSAGE_SCAN_FINISH = MESSAGE_SG_ESG_READY + 1;

	public static final int DIALOG_CANCEL_SCAN = MESSAGE_SCAN_FINISH + 1;

	public static final int DIALOG_CANCEL_GET_ESG = DIALOG_CANCEL_SCAN + 1;

	public static final int MESSAGE_SERVICE_PARAM = DIALOG_CANCEL_GET_ESG + 1;

	public static final int DIALOG_PROMPT_NO_SUB = MESSAGE_SERVICE_PARAM + 1;

	public static final int DIALOG_PROMPT_NO_SIMCARD = DIALOG_PROMPT_NO_SUB + 1;

	public static final int DIALOG_PROMPT_NO_SINGNAL = DIALOG_PROMPT_NO_SIMCARD + 1;

	public static final int DIALOG_CONFIRM_SUB = DIALOG_PROMPT_NO_SINGNAL + 1;

	public static final int DIALOG_CONFIRM_EXIT = DIALOG_CONFIRM_SUB + 1;

	public static final int MESSAGE_ON_ESG_XML = DIALOG_CONFIRM_EXIT + 1;

	public static final int DIALOG_CONFIRM_SETTING_CHANGE = MESSAGE_ON_ESG_XML + 1;

	public static final int DIALOG_PROMPT_UPDATE_ERR = DIALOG_CONFIRM_SETTING_CHANGE + 1;

	public static final int MESSAGE_SUB_CONFIRM_SURE = DIALOG_PROMPT_UPDATE_ERR + 1;

	public static final int MESSAGE_SUB_LOADING_FINISH = MESSAGE_SUB_CONFIRM_SURE + 1;

	public static final int DIALOG_CONFIRM_PROGRAM_EXPIRE = MESSAGE_SUB_LOADING_FINISH + 1;

	public static final int DIALOG_CONFIRM_PROGRAM_FUTURE = DIALOG_CONFIRM_PROGRAM_EXPIRE + 1;

	public static final int DIALOG_CONFIRM_EMERG_DELETE_ITEM = DIALOG_CONFIRM_PROGRAM_FUTURE + 1;

	public static final int MESSAGE_TS0_READY = DIALOG_CONFIRM_EMERG_DELETE_ITEM + 1;

	public static final int MESSAGE_SAVE_SERVICEPARAM = MESSAGE_TS0_READY + 1;

	public static final int MESSAGE_CAINFO = MESSAGE_SAVE_SERVICEPARAM + 1;

	public static final int MESSAGE_INITGBA = MESSAGE_CAINFO + 1;

	public static final int MESSAGE_PLAY = MESSAGE_INITGBA + 1;

	public static final int MESSAGE_PLAY_ERROR = MESSAGE_PLAY + 1;

	public static final int MESSAGE_SETLAYERFORPIP = MESSAGE_PLAY_ERROR + 1;

	public static final int MESSAGE_AUTHORIZE_FINISH = MESSAGE_SETLAYERFORPIP + 1;

	public static final int DIALOG_CONFIRM_SETTING_APN = MESSAGE_AUTHORIZE_FINISH + 1;// prompt user to setting a right APN

	public static final int DIALOG_CHECK_APN_SETTING = DIALOG_CONFIRM_SETTING_APN + 1;// prompt user to wait APN setting and network connecting

	public static final int DIALOG_CANCEL_AUTHORIZE = DIALOG_CHECK_APN_SETTING + 1;

	public static final int MESSAGE_APN_WAIT = DIALOG_CANCEL_AUTHORIZE + 1;

	public static final int MESSAGE_INIT_FINISH = MESSAGE_APN_WAIT + 1;

	public static final int MESSAGE_PREPARE_TIMEOUT = MESSAGE_INIT_FINISH + 1;

	public static final int MESSAGE_EB_MESSAGE = MESSAGE_PREPARE_TIMEOUT + 1;

	public static final int MESSAGE_NETWORK_CHECK = MESSAGE_EB_MESSAGE + 1;

	public static final int MESSAGE_NETWORK_REACH = MESSAGE_NETWORK_CHECK + 1;

	public static final int MESSAGE_PLAYBACK_ERROR = MESSAGE_NETWORK_REACH + 1;

	public static final int MESSAGE_APP_STARTED = MESSAGE_PLAYBACK_ERROR + 1;

	public static final int MESSAGE_APP_EXITED = MESSAGE_APP_STARTED + 1;

	public static final int MESSAGE_DATA_STATE_CHANGED = MESSAGE_APP_EXITED + 1;

	public static final int MESSAGE_CONTINUE_WAP_CONNECTIVITY = MESSAGE_DATA_STATE_CHANGED + 1;

	public static final int MESSAGE_WAIT_NETWORK_RESULT = MESSAGE_CONTINUE_WAP_CONNECTIVITY + 1;

	public static final int MESSAGE_DATA_STATE_CHANGED_FORATHOURIZE = MESSAGE_WAIT_NETWORK_RESULT + 1;

	public static final int MESSAGE_DATA_STATE_CHANGED_FORINQUIRY = MESSAGE_DATA_STATE_CHANGED_FORATHOURIZE + 1;

	public static final int MESSAGE_DATA_STATE_CHANGED_FORREFRESH = MESSAGE_DATA_STATE_CHANGED_FORINQUIRY + 1;

	public static final int MESSAGE_DATA_STATE_CHANGED_MODEEXCHANGE = MESSAGE_DATA_STATE_CHANGED_FORREFRESH + 1;

	public static final int MESSAGE_MODE_CHECKING = MESSAGE_DATA_STATE_CHANGED_MODEEXCHANGE + 1;

	public static final int MESSAGE_PLAY_BY_ID_FREQ = MESSAGE_MODE_CHECKING + 1;

	public static final int MESSAGE_MUTE = MESSAGE_PLAY_BY_ID_FREQ + 1;

	public static final int MESSAGE_UNMUTE = MESSAGE_MUTE + 1;

	public static final int MESSAGE_STOP = MESSAGE_UNMUTE + 1;

	public static final int DIALOG_CONFIRM_SUBSCRIBE = MESSAGE_STOP + 1;

	public static final int MESSAGE_CHANGE_FULL_SCREEN = DIALOG_CONFIRM_SUBSCRIBE + 1;

	public static final int DIALOG_CONFIRM_EMERG_DELETE_ALL = MESSAGE_CHANGE_FULL_SCREEN + 1;

	public static final int DIALOG_CONFIRM_STOP_RECORD = DIALOG_CONFIRM_EMERG_DELETE_ALL + 1;

	public static final int MESSAGE_NOTIFY_DATA_CHANGED = DIALOG_CONFIRM_STOP_RECORD + 1;

	public static final int EVENT_APN_CHANGED = MESSAGE_NOTIFY_DATA_CHANGED + 1;
	
	public static final int MESSAGE_PLAY_AFTER_UPDATETS0 = EVENT_APN_CHANGED + 1;
	
	public static final int MESSAGE_INIT_TIMEOUT = MESSAGE_PLAY_AFTER_UPDATETS0 + 1;
	
	public static final String OPERATION_UPDATE_SG = "update SG";
	public static final String OPERATION_UPDATE_ST = "update ST";
	public static final String OPERATION_INIT_GBA = "init GBA";
	public static final String OPERATION_PARSE_SG = "parse SG";
	public static final String OPERATION_PARSE_ST = "parse ST";
	public static final String OPERATION_UPDATE_TS0 = "update TS0";
	public static final String OPERATION_AUTHORIZE = "authorize";

}
