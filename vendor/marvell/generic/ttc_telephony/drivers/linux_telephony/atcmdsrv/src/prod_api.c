#ifdef AT_PRODUCTION_CMNDS
#include "teldef.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <osa.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include "prod_api.h"

static void prod_conn_task(void *arg);

void create_prod_conn_task(PROD_CTRL *pc)
{
	OSTaskRef taskRef = NULL;
	void *taskStack = NULL;
	OSA_STATUS osaStatus;

	taskStack = malloc(OSA_STACK_SIZE);
	if(taskStack == NULL)
	{
		DBGMSG("%s: malloc OSA_STACK error!\n", __FUNCTION__);
		return;
	}
	osaStatus = OSATaskCreate(&taskRef,
		taskStack, OSA_STACK_SIZE,
		AT_TASK_PRIORITY, (CHAR *)"PROD_CONN_TASK",
		(void *)prod_conn_task,
		(void *)pc);

	if ( osaStatus != OS_SUCCESS)
	{
		DBGMSG("%s: OSATaskCreate error!\n", __FUNCTION__);
		free(taskStack);
		return ;
	}
}

static void prod_conn_task(void *arg)
{
	int fd, len;
	struct sockaddr_un  un;
	int b_server = 0;
	int received = -1;
	unsigned char buffer[64];
	int rc;
	fd_set fds;
	PROD_CTRL *pc = (PROD_CTRL*)arg;

	DBGMSG("%s: try to create socket\n", __FUNCTION__);

	/* create a UNIX domain stream socket */
	if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
	{
		DBGMSG("%s: create socket error\n", __FUNCTION__);
		return;
	}

	/* fill in socket address structure */
	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	strcpy(un.sun_path, PROD_SOCKET_NAME);
	len = offsetof(struct sockaddr_un, sun_path) + strlen(PROD_SOCKET_NAME);

	/* bind the name to the descriptor */
	if(bind(fd, (struct sockaddr *)&un, len) < 0)
	{
		if(errno == EADDRINUSE) /* socket addres already exist */
		{
			DBGMSG("%s: socket already exist, try to connect it\n", __FUNCTION__);
			/* try to connect to the server */
			if(connect(fd, (struct sockaddr *)&un, len) < 0)
			{
				DBGMSG("%s: connect error\n", __FUNCTION__);
				return;
			}
			DBGMSG("%s: connect to server success\n", __FUNCTION__);
			b_server = 0;
		}
	}
	else
	{
		DBGMSG("%s: create server success\n", __FUNCTION__);
		if(listen(fd, 2) < 0)
		{
			DBGMSG("%s: listen error\n", __FUNCTION__);
			return;
		}
		b_server = 1;
		DBGMSG("%s: listen to connections\n", __FUNCTION__);

		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		rc = select(fd + 1, &fds, NULL, NULL, NULL);
		if (rc <= 0)
		{
			DBGMSG("%s: select error!\n", __FUNCTION__);
			return;
		}
		memset(&un, 0, sizeof(un));
		len = sizeof(un);
		if ((fd = accept(fd, (struct sockaddr *)&un, &len)) < 0)
		{
			DBGMSG("%s: accept error\n", __FUNCTION__);
			return;
		}
		DBGMSG("%s: connection established\n", __FUNCTION__);
	}

	pc->fd = fd;

	while(1)
	{
		received = -1;

		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		rc = select(fd + 1, &fds, NULL, NULL, NULL); // block until a byte is received
		if (rc <= 0)
		{
			DBGMSG("%s: select error!\n", __FUNCTION__);
			continue;
		}

		memset(buffer, 0x00, sizeof(buffer));
		received = read(fd, buffer, sizeof(buffer)/sizeof(buffer[0])-1);
		if(received <= 0)
		{
			DBGMSG("%s: read error!, error: %s\n", __FUNCTION__, strerror(errno));
			continue;
		}
		DBGMSG("%s: receive %d bytes\n", __FUNCTION__, received);
		DBGMSG("%s: body: 0x%02x%02x%02x%02x\n", __FUNCTION__, buffer[0], buffer[1], buffer[2], buffer[3]);

		pc->fp(buffer, received);
	}
}
#endif

