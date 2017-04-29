package com.marvell.updater.utils;

public class Constants {
    
    public static final boolean LOG_ON = true;
    
    // Host
    public static final String URL = "http://10.38.164.28:8080";
    public static final String SEP = "#";
    public static final String GET_SYSTEM_VERSION = "/AppServer/ScanUpdate";
    public static final String TARGET_PATH = "/sdcard/";
    
    public static final String KEY_OTA_PATH = "ota_package_path";
    public static final String NETWORK_SETTING = "server";
    public static final String KEY_SERVER = "host_ip";
    public static final String SUFFIX = "-ota-mrvl.zip";

    public static final int CONFIRM_DELAY = 600000;
    
    public static final String TAG_STATUS = "status";
    public static final String TAG_CODE = "code";
    public static final String TAG_MESSAGE = "message";
    public static final String TAG_PACKAGE = "package";
    public static final String TAG_VERSION = "version";
    public static final String TAG_URL = "url";
    public static final String TAG_TYPE = "type";
    public static final String TAG_NAME = "name";
    public static final String TAG_MODEL = "model";
    public static final String TAG_BRANCH = "branch";
    public static final String TAG_DATE = "date";
    
    public static final String CODE_SUCCESS = "E0000";
    public static final String CODE_NO_UPDATE = "E0001";
    public static final String CODE_SYSTEM_ERROR = "E0002";
    
}
