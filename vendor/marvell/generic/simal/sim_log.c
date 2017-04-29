/*
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
#include <stdio.h>
#include "sim_log.h"


static int sim_debug_level = MSG_INFO;
#ifdef ANDROID
#include <android/log.h>
void sim_printf(int level, char *format, ...)
{
	if (level >= sim_debug_level) {
		va_list ap;
		if (level == MSG_ERROR) {
				level = ANDROID_LOG_ERROR;
		} else if (level == MSG_WARNING) {
				level = ANDROID_LOG_WARN;
		} else if (level == MSG_INFO) {
				level = ANDROID_LOG_INFO;
		} else {
				level = ANDROID_LOG_DEBUG;
		}
		va_start(ap, format);
		__android_log_vprint(level, "simal", format, ap);
		va_end(ap);
	}

}
#else
#include <stdarg.h>
//void sim_printf(MSG_DEBUG,int level, char *format, ...)
void sim_printf(int level, char *format, ...)
{
	if (level >= sim_debug_level) {
		va_list ap;
		va_start(ap, format);
		vprintf(format, ap);
		va_end(ap);
	}
}
#endif

