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

import java.io.File;

import com.marvell.cmmb.aidl.LogMask;

import android.os.Environment;
import android.os.StatFs;

/**
 * The function is used to manage memory status.
 * 
 * @author DanyangZhou
 * @version [2010-9-8]
 */
public class MemoryStatus {

	private static final String TAG = "MemoryStatus";

	/**
	 * The error.
	 */
	private static final long ERROR = -1;

	/**
	 * The available memory size.
	 */
	private static long mAvailableMemorySize;

	/**
	 * The total memory size.
	 */
	private static long mTotalMemorySize;

	/**
	 * The block size.
	 */
	private static long mBlockSize;

	/**
	 * The block count.
	 */
	private static long mBlockCount;

	/**
	 * The application directory for store the data
	 */
	public static String mDirectory = "/MBBMSPlayer";

	/**
	 * the method is used to justify the sdcard being writtable or not
	 */
	public static boolean checkFsWrittable() {
		String mDirectoryName = Environment.getExternalStorageDirectory()
				+ mDirectory;
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(), "mDirectoryName = "
				+ mDirectoryName, LogMask.APP_MAIN);
		File mDirectory = new File(mDirectoryName);
		if (!mDirectory.isDirectory()) {
			LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
					"mDirectory is not a diretory", LogMask.APP_COMMON);
			if (!mDirectory.mkdirs()) {
				LogMask.LOGM(TAG, LogMask.LOG_APP_COMMON, LogMask.getLineInfo(),
						"create diretory failed", LogMask.APP_COMMON);
				return false;
			}
		}
		File mNewFile = new File(mDirectoryName, ".probe");
		try {
			if (mNewFile.exists()) {
				try {
					mNewFile.delete();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}

			if (!mNewFile.createNewFile()) {
				return false;
			}

			try {
				mNewFile.delete();
			} catch (Exception e) {
				e.printStackTrace();
			}

			return true;
		} catch (Exception ex) {
			return false;
		}
	}

	/**
	 * the method is used to justify the sdcard being mounted or not
	 */
	public static boolean externalMemoryAvailable() {
		LogMask.LOGM(TAG, LogMask.LOG_APP_MAIN, LogMask.getLineInfo(),
				"the state of external storage is "
						+ Environment.getExternalStorageState(),
				LogMask.APP_MAIN);
		return Environment.getExternalStorageState().equals(
				Environment.MEDIA_MOUNTED);
	}

	/**
	 * the method is used to transform memory size to formated usage.
	 */
	public static String formatSize(long size) {
		String suffix = null;
		if (size >= 1024) {
			suffix = "KB";
			size /= 1024;
		}
		if (size >= 1024) {
			suffix = "MB";
			size /= 1024;
		}

		StringBuilder mResultString = new StringBuilder(Long.toString(size));
		int mCommaOffset = mResultString.length() - 3;
		while (mCommaOffset > 0) {
			mResultString.insert(mCommaOffset, ',');
			mCommaOffset -= 3;
		}
		if (suffix != null)
			mResultString.append(suffix);

		if ("0".equals(mResultString.toString())) {
			return "0MB";
		}
		return mResultString.toString();
	}

	/**
	 * the method is used to justify the sdcard's available memory size
	 */
	public static long getAvailableExternalMemorySize() {
		if (externalMemoryAvailable()) {
			File mPath = Environment.getExternalStorageDirectory();
			StatFs mStat = new StatFs(mPath.getPath());
			mBlockSize = mStat.getBlockSize();
			mBlockCount = mStat.getAvailableBlocks();
			mAvailableMemorySize = mBlockSize * mBlockCount;
			return mAvailableMemorySize;
		} else {
			return ERROR;
		}
	}

	/**
	 * the method is used to justify the internal available memory size
	 */
	public static long getAvailableInternalMemorySize() {
		File mPath = Environment.getDataDirectory();
		StatFs mStat = new StatFs(mPath.getPath());
		mBlockSize = mStat.getBlockSize();
		mBlockCount = mStat.getAvailableBlocks();
		return mBlockSize * mBlockCount;
	}

	/**
	 * the method is used to get number of a string,starting with decimal digit
	 * and ending with 'B'.
	 */
	public static long getNumberInString(String str) {
		String mTempString = str;
		mTempString = mTempString.substring(0, mTempString.indexOf("B") - 1);
		return Long.valueOf(mTempString);
	}

	/**
	 * the method is used to justify the sdcard's total memory size
	 */
	public static long getTotalExternalMemorySize() {
		if (externalMemoryAvailable()) {
			File mPath = Environment.getExternalStorageDirectory();
			StatFs mStat = new StatFs(mPath.getPath());
			mBlockSize = mStat.getBlockSize();
			mBlockCount = mStat.getBlockCount();
			mTotalMemorySize = mBlockSize * mBlockCount;
			return mTotalMemorySize;
		} else {
			return ERROR;
		}
	}

	/**
	 * the method is used to justify the internal total memory size
	 */
	public static long getTotalInternalMemorySize() {
		File mPath = Environment.getDataDirectory();
		StatFs mStat = new StatFs(mPath.getPath());
		mBlockSize = mStat.getBlockSize();
		mBlockCount = mStat.getBlockCount();
		return mBlockSize * mBlockCount;
	}

}
