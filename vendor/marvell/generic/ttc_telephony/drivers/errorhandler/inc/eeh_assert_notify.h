/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * (C) Copyright 2008 Marvell International Ltd.
 * All Rights Reserved
 */
#ifndef __EEH_ASSERT_NOTIFY_H__
#define __EEH_ASSERT_NOTIFY_H__
#include <unistd.h>

extern FILE *logfile;
extern int loglevel;

#ifdef __cplusplus
extern "C" {
#endif

extern int eeh_draw_panic_text(const char *panic_text, size_t panic_len, int do_timer);
extern int cp_silent_reset_on_req(const char *reset_info);

#ifdef __cplusplus
}
#endif
#endif
#define CP_ERROR_FILE "CpErrorStatistic.log"
#define AP_ERROR_FILE "AppErrorStatistic.log"
#if defined (BIONIC)
#define TELEPHONY_MODULE_LOG_LEVEL_PROPERTY   "sys.telephonymoduleloglevel"
#endif
#define TELEPHONY_MODULE_LOG_SILENT 0
#define TELEPHONY_MODULE_LOG_EMERG 1
#define TELEPHONY_MODULE_LOG_ALERT 2
#define TELEPHONY_MODULE_LOG_CRIT 3
#define TELEPHONY_MODULE_LOG_ERR 4
#define TELEPHONY_MODULE_LOG_WARNING 5
#define TELEPHONY_MODULE_LOG_NOTICE 6
#define TELEPHONY_MODULE_LOG_INFO 7
#define TELEPHONY_MODULE_LOG_DEBUG 8

