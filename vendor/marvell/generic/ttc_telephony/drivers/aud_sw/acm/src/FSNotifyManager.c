/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/
//
//
//	FSNotifyManager.c
// Israael Davidenko israel.davidenko@marvell.com

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <signal.h>
#include <sys/inotify.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <linux/netlink.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <asm/page.h>

#include "FSNotifyManager.h"

#define MAX_BUF_SIZE  4096

typedef struct SNotifyData
{
	char*		chFilePath;
	char*       chFileName;
	FSNotifyCb	pNotyfyCb;
	int			iUserParam;
} SNotifyData_;

static void *notyfy_thread_handler(void *param);

int addfileNotifyCb(const char *chFilePath, const char *chFileName, FSNotifyCb pNotyfyCb, int iUserParam)
{
	int				ret;
	pthread_attr_t	attr;
	pthread_t		threadHandle;
	struct SNotifyData*	pThreadParam;
	int				iStringLen;

	pThreadParam = (struct SNotifyData*)malloc(sizeof(struct SNotifyData));
	if (pThreadParam == NULL)
	{
		return -1;
	}
	pThreadParam->iUserParam = iUserParam;

	iStringLen = strlen(chFilePath);
	pThreadParam->chFilePath = (char*)malloc(iStringLen + 1);
	if (pThreadParam->chFilePath == NULL)
	{
		free(pThreadParam);
		return -1;
	}
	memcpy(pThreadParam->chFilePath, chFilePath, iStringLen + 1);

	iStringLen = strlen(chFileName);
	pThreadParam->chFileName = (char*)malloc(iStringLen + 1);
	if (pThreadParam->chFileName == NULL)
	{
		free(pThreadParam);
		return -1;
	}
	memcpy(pThreadParam->chFileName, chFileName, iStringLen + 1);

	pThreadParam->pNotyfyCb = pNotyfyCb;

	ret = pthread_attr_init(&attr);
	ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);

	pthread_create(&threadHandle, NULL, notyfy_thread_handler, pThreadParam);
	pthread_attr_destroy(&attr);

	return 0;
}

static void *notyfy_thread_handler(void *param)
{
	int		fd;
	int		wd;
	int		iRet = -1;
	char	buffer[MAX_BUF_SIZE];
	struct inotify_event*	event;
	struct SNotifyData*		pThreadParam = (struct SNotifyData*)param;
	FSNotifyCb	pNotyfyCb = pThreadParam->pNotyfyCb;

	fd = inotify_init();
	if (fd < 0)
	{
		(pNotyfyCb)(-1);
		goto clean;
	}

	wd = inotify_add_watch(fd, pThreadParam->chFilePath, IN_ALL_EVENTS);

	while (1)
	{
		int event_pos = 0;

		int num_bytes = read(fd, buffer, MAX_BUF_SIZE);


		if (num_bytes < (int)sizeof(*event))
		{
			if (errno == EINTR)
				continue;

			(pNotyfyCb)(-1);
			goto clean;
		}

		while (num_bytes >= (int)sizeof(*event))
		{
			int event_size;
			event = (struct inotify_event *)(buffer + event_pos);

			if(strcmp(event->name, pThreadParam->chFileName) == 0)
			{
				if((event->mask == IN_CLOSE_WRITE) || (event->mask == IN_MOVED_TO)|| (event->mask == IN_CREATE) || (event->mask == IN_DELETE) || (event->mask == IN_DELETE_SELF))
				{
					(pNotyfyCb)(pThreadParam->iUserParam);
				}
			}

			event_size = sizeof(*event) + event->len;
			num_bytes -= event_size;
			event_pos += event_size;
		}
	}
clean:
	(pNotyfyCb)(-2);
	free(pThreadParam->chFilePath);
	free(pThreadParam->chFileName);
	free(pThreadParam);

	return (void *)iRet;
}
