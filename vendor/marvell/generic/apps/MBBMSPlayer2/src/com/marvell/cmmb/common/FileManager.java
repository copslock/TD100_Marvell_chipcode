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
import java.io.FileNotFoundException;
import java.text.SimpleDateFormat;
import java.util.Date;

import android.content.Context;
import android.net.Uri;
import android.os.Environment;
import android.os.ParcelFileDescriptor;

/**
 * To manager the file's create behavior
 * 
 * @author DanyangZhou
 * @version [2011-4-8]
 * 
 */
public class FileManager {
	public static final String APP_FOLDER = Environment
			.getExternalStorageDirectory()
			+ "/MBBMSPlayer/";
	/**
	 * The video record directory for store the data
	 */
	public static final String VIDEORECORDDIRECTORY = "/MBBMSPlayer/Video";

	/**
	 * The image capture directory for store the data
	 */
	public static final String IMAGECAPTUREDIRECTORY = "/MBBMSPlayer/Image";

	/**
	 * the video recorder name
	 */
	public static final String VIDEORECORDFILENAME = "REC";

	public static final String IMAGECAPTUREFILENAM = "IMG";

	public static final String RENAME = "(1)";

	public static String VIDEORECORDTYPE = ".3gp";

	public static String IMAGECAPTURETYPE = ".jpg";

	/**
	 * change the time format that used by file name,example :
	 * "yyyy-MM-dd-HH-mm-ss" will show "2010-02-03-23-33-23"
	 */
	public static final String FORMAT_NAME_TYPE = "yyyy-MM-dd-HH-mm-ss";

	/**
	 * 
	 * @param parentDirectory
	 *            the directory name under CMMBPlayer
	 * @param filename
	 *            the file name
	 * @return
	 */
	public static File createFile(String parentDirectory, String filename) {
		String head = "";
		String newname = filename;
		String format = "";
		String mDirectoryName = Environment.getExternalStorageDirectory()
				+ parentDirectory;
		File mDirectory = new File(mDirectoryName);
		if (!mDirectory.isDirectory()) {
			if (!mDirectory.mkdirs()) {
				return null;
			}
		}
		if (parentDirectory == VIDEORECORDDIRECTORY) {
			head = VIDEORECORDFILENAME;
			format = VIDEORECORDTYPE;
		} else if (parentDirectory == IMAGECAPTUREDIRECTORY) {
			head = IMAGECAPTUREFILENAM;
			format = IMAGECAPTURETYPE;
		}
		File mNewFile = null;
		try {
			do {
				mNewFile = new File(mDirectoryName, head + newname + format);
				newname += RENAME;
			} while (mNewFile.exists());
			if (!mNewFile.createNewFile()) {
				return null;
			}
			return mNewFile;
		} catch (Exception ex) {
			return null;
		}
	}

	/**
	 * open the created image capture file and return the File
	 * 
	 * @param context
	 * @return the file
	 */
	public static File createImageCaptureFile() {
		return createFile(IMAGECAPTUREDIRECTORY, getTimeString());
	}

	/**
	 * open the created video record file and return the ParcelFileDescriptor
	 * 
	 * @param context
	 * @return the opened ParcelFileDescriptor
	 */
	public static File createVideoRecordFile() {
		return createFile(VIDEORECORDDIRECTORY, getTimeString());
	}

	/**
	 * return the ParcelFileDescriptor
	 * 
	 * @param context
	 * @return the opened ParcelFileDescriptor
	 */
	public static ParcelFileDescriptor getFileParcelable(Context context,
			File file) {
		ParcelFileDescriptor rt = null;
		if (null != file) {
			try {
				ParcelFileDescriptor pfd = context.getContentResolver()
						.openFileDescriptor(Uri.fromFile(file), "rw");
				if (null != pfd) {
					rt = pfd;
				} else {
					file.delete();
				}
			} catch (FileNotFoundException e) {
				file.delete();
				e.printStackTrace();
			}
		}
		return rt;
	}

	public static String getTimeString() {
		SimpleDateFormat sdf = new SimpleDateFormat(FORMAT_NAME_TYPE);
		return sdf.format(new Date(System.currentTimeMillis()));
	}

}
