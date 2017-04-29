#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/sockios.h>
#include <linux/if.h>
#include <pthread.h>
#include <fcntl.h>
#include <linux/netlink.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>

#include "linux_types.h"
#include "mdemo.h"
#include "msocket.h"


int main()
{
	int sock;
	int Port = 1;
	char buffer[2048];
	int i;

	sock = msocket( Port );
	if ( sock < 0 )
	{
		printf("socket error");
		return -1;
	}

	//sprintf( buffer , "the sample string sent to CP "  );
	//msend( sock , buffer , 100 , 0 );
	for ( i = 0; i < 2000; i++)
	{

		sprintf( buffer, "the %d sample string sent to CP ", i  );
		msend( sock, buffer, 100, 0 );

		//printf("demo mrecv started...\n");
		//mrecv( sock , buffer , sizeof( buffer ) , 0);

		//printf("demo  received = %s\r\n", buffer );
		//usleep(500);

	}

	printf("demo finished\n");
	mclose( sock );
	return 0;


}


