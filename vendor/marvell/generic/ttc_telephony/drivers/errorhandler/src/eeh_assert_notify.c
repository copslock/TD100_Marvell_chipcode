/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * (C) Copyright 2008 Marvell International Ltd.
 * All Rights Reserved
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#if !defined (BIONIC)
#include <execinfo.h>
#endif

#include <stdarg.h>
#include <syslog.h>
#include <time.h>
#include "eeh_assert_notify.h"
#include "eeh_ioctl.h"

#if defined (BIONIC)
#include <utils/Log.h>
#include <android/log.h>
#include <cutils/properties.h>
#endif

#define UNUSEDPARAM(param) (void)param;

FILE *logfile = NULL;
char log_tag[256];
int loglevel = TELEPHONY_MODULE_LOG_DEBUG;

/* Control the log output */
void eeh_log(int level, const char *format, ...)
{
	char *timestr;
	char buf[1024];
	va_list ap;
	struct timeval log_time;
	FILE *outfd;
	BOOL logprint = TRUE;
	int prio = 0;
#if defined (BIONIC)
	char loglevelBuf[PROPERTY_VALUE_MAX];
	property_get(TELEPHONY_MODULE_LOG_LEVEL_PROPERTY, loglevelBuf, "");
	loglevel = atoi(loglevelBuf);
#endif
	if (level >= loglevel)
		logprint = FALSE;

	if (logfile)
		outfd = logfile;
	else
		outfd = stdout;

	/* Don't print time information for LOG_INFO level */
	if(logprint)
	{
		gettimeofday(&log_time, NULL);
		timestr = ctime(&(log_time.tv_sec));
		timestr[strlen(timestr) - 1] = '\0';
		fprintf(outfd, "%s(%d us) <%1.1d> ", timestr, (int)(log_time.tv_usec), level);
	}
	va_start(ap, format);

	if(logprint)
		vfprintf(outfd, format, ap);
#if defined (BIONIC)
	if(logprint)
	{
		vsnprintf(buf,1024,format,ap);

		switch(level)
		{
		case LOG_EMERG:
		case LOG_ALERT:
		case LOG_CRIT:
			prio = ANDROID_LOG_FATAL;
			break;
		case LOG_ERR:
			prio = ANDROID_LOG_ERROR;
			break;
		case LOG_WARNING:
			prio = ANDROID_LOG_WARN;
			break;
		case LOG_NOTICE:
		case LOG_INFO:
			prio = ANDROID_LOG_INFO;
			break;
		case LOG_DEBUG:
			prio = ANDROID_LOG_VERBOSE;
			break;
		default:
			prio = ANDROID_LOG_DEFAULT;
			break;
		}

		android_printLog(prio, log_tag, "%s", buf);
	}
#endif
	va_end(ap);
	fflush(outfd);
}

/* Obtain a backtrace and print it to stdout. */
#if !defined (BIONIC)
void print_trace (char *buf)
{
	void *array[20];
	size_t size;
	char **strings;
	size_t i;

	size = backtrace(array, 20);
	strings = backtrace_symbols(array, size);

	sprintf(buf, "\nObtained %zd stack frames.\n", size);

	for (i = 0; i < size; i++)
	{
		strcat(buf, strings[i]);
		strcat(buf, "\n");
	}

	eeh_log(LOG_ERR, "%s", buf);

	free(strings);
}
#endif

/* Output the ASSERT Information to NVM file */
int eeh_draw_panic_text(const char *panic_text, size_t panic_len, int do_timer)
{
	UNUSEDPARAM(panic_len)
#if !defined (BIONIC)
	char command_buf[512];
#endif
	struct timeval log_time;
	char *timestr;

	static FILE *fd = 0;
	char fullPath[256] = { 0 };
	char fileName[] = AP_ERROR_FILE;
	char fileNameCP[] = CP_ERROR_FILE;
	char *nvm_root_dir = getenv("NVM_ROOT_DIR");

	//for the AP_ASSERT, trigger error handler process it
	if((EehAssertType) do_timer == EEH_AP_ASSERT)
	{
		int fd;
		/* Notify EEH that APP assertion occurs */
		fd = open("/dev/seh",O_RDWR);
		if (fd != -1)
		{
			EehAppAssertParam param;	//param.msgDesc[MAX_DESCRIPTOR_SIZE]
			memset(param.msgDesc, 0, sizeof(param.msgDesc));
			strncpy(param.msgDesc, panic_text, sizeof(param.msgDesc)-1);
#ifndef WIN32
			ioctl(fd, SEH_IOCTL_APP_ASSERT, &param);
			close(fd);
#endif
		}
	
#ifndef WIN32
		sleep(1);
#endif
		// Here can be implemented core dump generation and destroy the failed process.
		// But the EEH-process may need this one alive for a short period.
		// So the sleep() is required
#ifndef WIN32
		while(1)
			sleep(1);
#endif
	}


	gettimeofday(&log_time, NULL);
	timestr = ctime(&(log_time.tv_sec));
	timestr[strlen(timestr) - 1] = '\0';

	eeh_log(LOG_CRIT, "%s\n", panic_text);

	if (nvm_root_dir != NULL)
	{
		strcpy(fullPath, nvm_root_dir);
		strcat(fullPath, "/");
	}

	switch((EehAssertType) do_timer)
	{
		case EEH_CP_EXCEPTION:
			strcat(fullPath,fileNameCP);
			break;
		default:
			strcat(fullPath,fileName);
			break;
	}

	if ((fd = fopen(fullPath, "a")) != 0)
	{
		fprintf(fd, "%s(%d us) %s\n", timestr, (int)log_time.tv_usec, panic_text);
		#if !defined (BIONIC)
		memset(command_buf, 0, sizeof(command_buf));
		print_trace(command_buf);
		fprintf(fd, "Stack:\n");
		fprintf(fd, "%s\n", command_buf);
		#endif
		fclose(fd);
	}

	return 0;
}
int cp_silent_reset_on_req(const char *reset_info)
{
	int fd;
	int ret = -1;
	fd = open("/dev/seh",O_RDWR);
	if (fd != -1)
	{
		EehCpSilentResetParam param;	//param.msgDesc[MAX_DESCRIPTOR_SIZE]
		memset(&param, 0x0, sizeof(param));
		if(reset_info != NULL)
			strncpy(param.msgDesc, reset_info, sizeof(param.msgDesc)-1);
		ret = ioctl(fd, SEH_IOCTL_CP_SILENT_RESET, &param);
		close(fd);
		return ret;
	}
	return ret;
}

