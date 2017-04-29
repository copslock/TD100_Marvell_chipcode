#include "diag_al.h"
#include <pthread.h>
#include "linux_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <linux/netlink.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include "diag.h"
#include <sys/time.h>

#define LOOP_CUNT 1024 * 2
int main(int argc, char *argv[])
{
#ifndef DIAG_USB_TEST
	UNUSEDPARAM(argc);
	UNUSEDPARAM(argv);
#endif
	int testUSBHandle;
	struct timespec tm;

	tm.tv_sec = 0;
	tm.tv_nsec = 5000000L;


	testUSBHandle = open("/dev/ttydiag0", O_RDWR | O_NOCTTY | O_NDELAY);
	if (testUSBHandle == -1)
	{
		printf("failed to init USB connection\n");
		return -1;
	}

#ifdef DIAG_USB_TEST
	if (argc < 2)
	{
		printf("Usage: %s <case_num>\n", argv[0]);
		return -1;
	}

	case_num = atoi(argv[1]);

	ioctl(testUSBHandle, DIAGSTUB_TESTSTART, case_num);     // only case 3 supported now
#endif
	{
		time_t timep;
		time(&timep);


		ioctl(testUSBHandle, DIAGSTUB_DUMPCOUNT, 0);
		printf("%s", ctime(&timep));
	}
	return 0;
}
