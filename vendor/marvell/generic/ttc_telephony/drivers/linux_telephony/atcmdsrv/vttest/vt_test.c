/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*
 * vt_test.c
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
#include <sys/ioctl.h>

#define DATABITS       CS8
#define BAUD            B115200
#define STOPBITS        0
#define PARITYON        0
#define PARITY          0
#define SERIAL_PORT1_NAME        "/dev/citty8"
#define DATA_PORT_NAME "/dev/cidatatty0"
char acDeviceName1[255];

#define TIOPPPON _IOW('T', 208, int)
#define TIOPPPOFF _IOW('T', 209, int)
#define IO_FLUSH_PENDING_BUFFERS _IOW('T', 99, int) /* flush pending buffer */

//static int tty_disc = N_PPP;//N_SYNC_PPP;//N_TTY;	/* The TTY discipline */

int serialfd1;
int datafd;
int cctdatafd;

int loopback = 1;

pthread_t DataThread1;


#define SERVER_PORT 1500
#define MAX_MSG 100

#define BUFFSIZE    512
char buffer[BUFFSIZE];
char databuf[2048];

void ReceiveDataFromChan1(void) __attribute__((noreturn));
void ReceiveDataFromDataChan(void) __attribute__((noreturn));

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

void ReceiveDataFromDataChan(void)
{
	fd_set readfds;
	struct timeval tv;
	int fdcount;
	ssize_t dwBytesTransferred = -1;


	printf("listen on cidatatty: \n");

//	write(datafd, "hello", 5);
	FILE* output_file = fopen("vtoutput.log", "w+");

#if 1
	FD_ZERO(&readfds);
	
	if (datafd >= 0)
	{
		/* load the buffer received before diag init */
//		ioctl(datafd, IO_FLUSH_PENDING_BUFFERS, 0);
		FD_SET(datafd, &readfds);
	}

	while(1)
	{

		tv.tv_sec = 30; //timeout is needed to allow FD_SET updates
		tv.tv_usec = 0;//250000;
		fdcount = select(datafd + 1, &readfds, NULL, NULL, &tv);
		if (fdcount < 0)
		{
			printf("select cidatatty0 fail\n");
			continue;
		}
		else if (fdcount == 0)
		{
			printf("select cidatatty0 timeout\n");
			continue;
		}
		else
		{
			//printf("data received \n");
			if (datafd > 0 && FD_ISSET(datafd, &readfds))
			{
				dwBytesTransferred = read(datafd, databuf, 2048 - 1);//recv(datafd, databuf, 2047, 0);
				if (dwBytesTransferred <= 0)
					continue;

				//databuf[dwBytesTransferred] = '\0'; /* Assure null terminated string */
				
//				printf("receive %d: %x \n", dwBytesTransferred, databuf[0]);
				
				fwrite(databuf, 1, dwBytesTransferred, output_file);

				if(loopback)
				{
					int rc = write(datafd, databuf, dwBytesTransferred);
					if(rc < 0)
						printf("send datatty error");
				}
			}
		}

	}

#else
	while (1)
	{

		int bytes = 0;
		if ((bytes = read(datafd, databuf, 2048 - 1)) < 1)
		{
			//printf("Failed to receive bytes from server");
			usleep(1);
			continue;
		}
		databuf[bytes] = '\0';        /* Assure null terminated string */

		//printf("Data In length (%d).\n", bytes);

		//fprintf(stdout, buffer);
		printf("receive %d: %x \n", bytes, databuf[0]);
	}
#endif
}


int startDataReceive(void)
{
	struct termios oldtio;
	/*open the device */
	datafd = open(DATA_PORT_NAME, O_NONBLOCK | O_RDWR); 

	if (datafd <= 0)
	{
		printf("Error opening device %s\n", DATA_PORT_NAME);
		return 0;
	}

#if 1
	fcntl(datafd, F_SETFL, 0);

	tcgetattr(datafd, &oldtio); // save current port settings

	
   // oldtio.c_cflag     &= ~(CSIZE | CSTOPB | PARENB | CLOCAL);
    oldtio.c_cflag     |= BAUD | CS8 | CREAD | HUPCL | CLOCAL;

    oldtio.c_iflag      = IGNBRK | IGNPAR;
    oldtio.c_oflag      = 0;
    oldtio.c_lflag      = 0;
    oldtio.c_cc[VMIN]   = 1;
    oldtio.c_cc[VTIME]  = 0;

	//oldtio.c_cflag ^= (CLOCAL | HUPCL);

	oldtio.c_cflag |= ~CRTSCTS;

	tcflush(datafd, TCIFLUSH);
	tcsetattr(datafd, TCSANOW, &oldtio);


	if ( pthread_create( &DataThread1, NULL, (void *)ReceiveDataFromDataChan, NULL) != 0 )
	{
		perror("pthread_create");
		return 0;
	}
#endif

	return 1;


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
	printf("Starting VT test with argc : %d\n", argc);
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

	printf(" use 'help' to get usage. \n");

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

		recieved = read(0, buf, 512);
		if(recieved > 0)
		{
		if (buf[recieved - 1] == '\a' || buf[recieved - 1] == 0xa )
		{

			if(recieved == 4 && buf[0]=='c' && buf[1]=='n')
			{
				buf[3] = '\0';
				int id = atoi((const char *)&(buf[2]));
				printf("try to connect cctdata by id = %d\n", id);
				
				if ((cctdatafd = open("/dev/cctdatadev0", O_RDWR)) == -1)
				{
					printf("Error open name %s\n", "/dev/cctdatadev0");
					continue;
				}

				
				ioctl(cctdatafd, TIOPPPON, id);

				startDataReceive();

				close(cctdatafd);
	
				continue;
			}
			else if(recieved == 3 && buf[0]=='s' && buf[1]=='n')
			{
				if(datafd > 0)
				{
					rc =write(datafd, databuf, 160);
					if(rc < 0)
						printf("send datatty error");
				}
				
				continue;
			}
			else if(recieved == 4 && buf[0]=='l' && buf[1]=='p' )
			{
				buf[3] = '\0';
				int isloop = atoi((const char *)&(buf[2]));
				printf("set loopback is %d\n", isloop);

				loopback = isloop?1:0;

				continue;
			}
			else if(recieved > 6 && buf[0]=='s' && buf[1]=='e' && buf[2]=='n' && buf[3]=='d' && buf[4]==' ')
			{
				if(datafd <=0)
				{
					printf("no data fd\n ");
					continue;
				}

				buf[recieved - 1] = '\0';

				FILE* input_file = fopen((const char *)&buf[5], "r");
				char buffer_send[161];

				if(input_file == NULL)
				{
					printf("can not find file: %s \n", &buf[5]);
					continue;

				}
				printf("file sending... \n");

				while(fread(buffer_send, 1, 160, input_file) > 0)
				{
					//printf("send 160 bytes");
					
					rc =write(datafd, buffer_send, 160);
					if(rc < 0)
						printf("send datatty error \n");

					usleep(20); 
				
				}

				printf("file send finished \n");

				fclose(input_file);

				continue;
			}
			else if(recieved == 5 && buf[0]=='h' && buf[1]=='e' && buf[2]=='l' && buf[3]=='p')
			{
				printf(" you can use all AT command here on test port \n");
				printf(" use 'lp0' or 'lp1' to disable or enable local data loopback \n" );
				printf(" use 'cnX' to connect user plane after received CONNECT, X is cid \n");
				printf(" use 'send filename' to send a raw data file \n");
				printf(" use 'sn' to send a single 160 byte radom data \n");

				continue;
			}
			

		
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
