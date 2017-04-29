/*--------------------------------------------------------------------------------------------------------------------
(C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
-------------------------------------------------------------------------------------------------------------------*/

/*
 * serial_cfun0.c
 *
 * Use to test AT Command Server 
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <pthread.h>
#include <termios.h>
#include <fcntl.h>


#define DATABITS       CS8
#define BAUD        	B115200
#define STOPBITS        0
#define PARITYON        0
#define PARITY          0
#define SERIAL_PORT1_NAME        "/dev/citty8"
#define SERVER_PORT 1500
#define MAX_MSG 100
#define BUFFSIZE    512
#define UNUSEDPARAM(param) (void)param;

char acDeviceName1[255]; 
int serialfd1;
char buffer[BUFFSIZE];
void receivedatafromchan1(void) __attribute__((noreturn));

void receivedatafromchan1(void)
{
	/*
	 *  check if there is response
	 */
	while (1) 
	{
		int bytes = 0;
		if ((bytes = read(serialfd1, buffer, BUFFSIZE-1)) < 1) 
		{
			/* printf("Failed to receive bytes from server"); */
			usleep(1);          
			continue;
		}
		/* Assure null terminated string */
		buffer[bytes] = '\0';
		printf("%s", buffer);
	}
}


int main (int argc, char *argv[]) {

	UNUSEDPARAM(argc);
	UNUSEDPARAM(argv);
	int rc;
	int len;
	struct termios oldtio, newtio;
	memset(&newtio, 0x0, sizeof(newtio));
	/* use Default */
	sprintf(acDeviceName1, "%s", SERIAL_PORT1_NAME);

	/*
	 *  Open the Device
	 */

	/*open the device(com port) to be non-blocking (read will return immediately) */
	serialfd1 = open(acDeviceName1, O_RDWR);
	if (serialfd1 <= 0) 
	{
		printf("Error opening device %s\n", acDeviceName1);
		return 0;
	}
	
	/* Make the file descriptor asynchronous (the manual page says only */
	fcntl(serialfd1, F_SETFL, 0);
	/* save current port settings */
	tcgetattr(serialfd1,&oldtio);

	/* set new port settings for canonical input processing */
	newtio.c_cflag = BAUD | CRTSCTS | DATABITS | STOPBITS | PARITYON | PARITY | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	newtio.c_lflag =  0;

	newtio.c_cc[VMIN] = 1;
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VERASE] = 0x8;
	newtio.c_cc[VEOL] = 0xD;

	newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */
	newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
	newtio.c_cc[VERASE]   = 0;     /* del */
	newtio.c_cc[VKILL]    = 0;     /* @ */
	newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
	newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
	newtio.c_cc[VSWTC]    = 0;     /* '\0' */
	newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */
	newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
	newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
	newtio.c_cc[VEOL]     = 0;     /* '\0' */
	newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
	newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
	newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
	newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
	newtio.c_cc[VEOL2]    = 0;     /* '\0' */

	newtio.c_cc[VMIN] = 1;
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VERASE] = 0x8;
	newtio.c_cc[VEOL] = 0xD;

	tcflush(serialfd1, TCIFLUSH);
	tcsetattr(serialfd1,TCSANOW,&newtio);

	/*
	 *  Create a thread to recieve
	 */

	pthread_t inputthread1;
	if( pthread_create( &inputthread1, NULL, (void *)receivedatafromchan1 ,NULL) != 0 )
	{
		perror("pthread_create");
		exit( 1 );
	}

	unsigned char buf[12] = "AT+CFUN=0";
	len = strlen((const char *)buf);
	buf[len] = 0xd;
	printf("The input string is  : %s\n", buf);

	rc = write(serialfd1, buf, len+1);
	if(rc < 0) 
	{
		perror("cannot send data ");

	}

	return 0;
}
