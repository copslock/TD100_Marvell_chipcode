#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>


#include "msocket_api.h"

int main(int argc, char *argv[])
{
	int sock;
	int case_num;

	if (argc < 2)
	{
		printf("Usage: %s <case_num>\n", argv[0]);
		return -1;
	}

	case_num = atoi(argv[1]);

	sock = open( "/dev/msocket", O_RDWR);
	if (sock < 0)
	{
		printf("open device failed:%d\n", sock);
		return -1;
	}

//	ioctl( sock, MSOCKET_TEST , &case_num );

	close(sock);

}
