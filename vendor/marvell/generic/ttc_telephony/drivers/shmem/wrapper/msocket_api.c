#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>

#include "msocket_api.h"

#define MSOCKET_DEVICE	"/dev/msocket"

/* open msocket */
int msocket(int port)
{
	int sock, rc;

	/* open device */
	if ((sock = open(MSOCKET_DEVICE, O_RDWR)) < 0) {
		printf("open device %s failed: %d\n", MSOCKET_DEVICE, sock);
		return sock;
	}

	/* bind to port */
	rc = ioctl(sock, MSOCKET_IOC_BIND, port);

	/* return the corrected code */
	return rc < 0 ? rc : sock;
}

/* close msocket */
int mclose(int sock)
{
	return close(sock);
}

/* receive packet */
int mrecv(int sock, void *buf, int len, int flags)
{
	/* avoid unused parameter warning */
	(void)flags;

	return read(sock, buf, len);
}

/* send packet */
int msend(int sock, const void *buf, int len, int flags)
{
	/* avoid unused parameter warning */
	(void)flags;

	return write(sock, buf, len);
}

/* send msocket control command */
static int msocket_control(int request)
{
	int sock, rc;

	/* open device */
	if ((sock = open(MSOCKET_DEVICE, O_WRONLY)) < 0) {
		printf("open device %s failed: %d\n", MSOCKET_DEVICE, sock);
		return sock;
	}

	/* control link up */
	rc = ioctl(sock, request);

	/* close msocket */
	close(sock);

	return rc;

}

/* control msocket link down */
int msocket_linkdown(void)
{
	return msocket_control(MSOCKET_IOC_DOWN);
}

/* control msocket link up */
int msocket_linkup(void)
{
	return msocket_control(MSOCKET_IOC_UP);
}
