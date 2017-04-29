/*
 *
 *  Audio server ipc
 *
 */


#ifndef __AUDIO_IPC_H__
#define __AUDIO_IPC_H__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#define MAINSOCKIP    "127.0.0.1"
#define MAINSOCKPORT  21000
#define TRANSOCKIP    "127.0.0.2"
#define TRANSOCKPORT4READ  21001
#define TRANSOCKPORT4WRITE  21002
#define TRANSOCKPORT4CALIBRATE  21003
#define BROADCASTIP  "127.0.0.255"

#define PHONE_CMD_QUERY 0x01
#define PHONE_CMD_WRITE 0x02
#define PHONE_CMD_READ  0x03
#define PHONE_CMD_POLL  0x04
#define PHONE_CMD_WBUF  0x05

#define STREAM_NOXRUN   0x00
#define STREAM_UNDERRUN 0x01
#define STREAM OVERRUN  0x02

/*
	Timeout Setting

	CP indication timeout 600ms
	PCM Write Polling Timeout 600ms
	PCM Read  Polling Timeout 600ms
*/
#define CP_INDICATE_TIMEOUT 600
#define PCM_WRITE_POLL_TIMEOUT 600
#define PCM_READ_POLL_TIMEOUT 600

// Ipc shared
int make_udp_mainsock();
int make_client_mainsock();
int make_server_mainsock();
/*add port by Raul, use different port to support simultaneously playback and record during voice call*/
int make_client_transock(int port);
int make_server_transock(int port);
void setup_socket(int sockfd);
void close_socket(int sockfd);
int accept_socket(int sockfd);
int send_socket(int sockfd, void* buffer, int size);
int recv_socket(int sockfd, void* buffer, int size);


#endif

