/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*
 * serial_client.c
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
#include <unistd.h> /* close */
#include <pthread.h>
#include <termios.h>
#include <fcntl.h>


#define DATABITS       CS8
//#define BAUD        B38400
#define BAUD            B115200
#define STOPBITS        0
#define PARITYON        0
#define PARITY          0
//#define SERIAL_PORT_NAME			"/dev/ttyS1"
#define SERIAL_PORT1_NAME        "/dev/citty8"
char acDeviceName1[255];

int serialfd1;

#define SERVER_PORT 1500
#define MAX_MSG 100

#define BUFFSIZE    512
char buffer[BUFFSIZE];
void ReceiveDataFromChan1(void) __attribute__((noreturn));

void ReceiveDataFromChan1(void)
{
	/*
	 *  check if there is response
	 */
	while (1)
	{

		int bytes = 0;
		if ((bytes = read(serialfd1, buffer, BUFFSIZE - 1)) < 1)
		{
			//printf("Failed to receive bytes from server");
			usleep(1);
			continue;
		}
		buffer[bytes] = '\0';        /* Assure null terminated string */

		//printf("Data In length (%d).\n", bytes);

		//fprintf(stdout, buffer);
		printf("%s", buffer);
	}
}


int main (int argc, char *argv[])
{

	int rc, i;
	int recieved;

	/*
	    if(argc < 3) {
	    printf("usage: %s <server> <data1> <data2> ... <dataN>\n",argv[0]);
	    exit(1);
	    }
	 */

	/*
	 * if there are the right number of parameters
	 */
	printf("Starting Serial Client with argc : %d\n", argc);
	if (argc > 2)
	{

		i = sscanf(argv[1], "%s", acDeviceName1);
		if (i != 1)
			return -1;
	}
	else
	{
		/* use Default */
		sprintf(acDeviceName1, "%s", SERIAL_PORT1_NAME );
	}

	struct termios oldtio, newtio;       //place for old and new port settings for serial port
	memset(&newtio, 0x0, sizeof(newtio));
	/*
	 *  Open the Device
	 */
	//serialfd = utlOpenIO(acDeviceName);

	/*open the device(com port) to be non-blocking (read will return immediately) */
	serialfd1 = open(acDeviceName1, O_RDWR   ); //| O_NOCTTY

	if (serialfd1 <= 0)
	{
		printf("Error opening device %s\n", acDeviceName1);
		return 0;
	}


	// Make the file descriptor asynchronous (the manual page says only
	// O_APPEND and O_NONBLOCK, will work with F_SETFL...)

	fcntl(serialfd1, F_SETFL, 0);

	tcgetattr(serialfd1, &oldtio); // save current port settings

	// set new port settings for canonical input processing
	newtio.c_cflag = BAUD | CRTSCTS | DATABITS | STOPBITS | PARITYON | PARITY | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	///newtio.c_lflag =  ECHOE | ECHO | ICANON;       //ICANON;
	newtio.c_lflag =  0;       //ICANON;

	newtio.c_cc[VMIN] = 1;
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VERASE] = 0x8;
	newtio.c_cc[VEOL] = 0xD;

	newtio.c_cc[VINTR]    = 0;      /* Ctrl-c */
	newtio.c_cc[VQUIT]    = 0;      /* Ctrl-\ */
	newtio.c_cc[VERASE]   = 0;      /* del */
	newtio.c_cc[VKILL]    = 0;      /* @ */
	newtio.c_cc[VEOF]     = 4;      /* Ctrl-d */
	newtio.c_cc[VTIME]    = 0;      /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 1;      /* blocking read until 1 character arrives */
	newtio.c_cc[VSWTC]    = 0;      /* '\0' */
	newtio.c_cc[VSTART]   = 0;      /* Ctrl-q */
	newtio.c_cc[VSTOP]    = 0;      /* Ctrl-s */
	newtio.c_cc[VSUSP]    = 0;      /* Ctrl-z */
	newtio.c_cc[VEOL]     = 0;      /* '\0' */
	newtio.c_cc[VREPRINT] = 0;      /* Ctrl-r */
	newtio.c_cc[VDISCARD] = 0;      /* Ctrl-u */
	newtio.c_cc[VWERASE]  = 0;      /* Ctrl-w */
	newtio.c_cc[VLNEXT]   = 0;      /* Ctrl-v */
	newtio.c_cc[VEOL2]    = 0;      /* '\0' */

	newtio.c_cc[VMIN] = 1;
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VERASE] = 0x8;
	newtio.c_cc[VEOL] = 0xD;



	tcflush(serialfd1, TCIFLUSH);
	tcsetattr(serialfd1, TCSANOW, &newtio);

	/* indicate the port is open */
	//write(serialfd, "COMM port ready.\n", 16);


	/*
	 *  Create a thread to recieve
	 */

	pthread_t InputThread1;
	if ( pthread_create( &InputThread1, NULL, (void *)ReceiveDataFromChan1, NULL) != 0 )
	{
		perror("pthread_create");
		exit( 1 );
	}

	unsigned char buf[512];

	while (1)  /* loop forever */
	{

		//read(0, buf, 1);
		recieved = read(0, buf, 512);

		//printf("data read length: %d.\n", recieved);
		/*
		 *  Fixing the carriage, in unix, it treat it as line feed
		 */
		//printf("input %c\n", buf);
		if(recieved > 0)
		{
		if (buf[recieved - 1] == '\a' || buf[recieved - 1] == 0xa )
		{
			//memset(buf[recieved-1], 0xD, 1);
			buf[recieved - 1] = 0xd;
		}

		rc = write(serialfd1, buf, recieved);

		if (rc < 0)
		{
			perror("cannot send data ");
			//close(sd);
			//exit(1);

		}

		//write(0, buf, 1);
		}
	} /* end while */
	return 0;

}
