/*
 *
 *  Audio server ipc
 *
 */

#include "audio_ipc.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const char path_server_mainsock[] = "/tmp/sever_mainsock"; /*main sock*/
const char path_socket4read[] = "/tmp/socket4read";  /*read sock*/
const char path_socket4write[] = "/tmp/socket4write";  /*write sock*/
const char path_socket4calibration[] = "/tmp/socket4calibration";  /*calibration sock*/

void close_socket(int sockfd)
{
	if (sockfd > 0)
	{
		shutdown(sockfd, SHUT_RDWR);
		close(sockfd);
	}
}

int make_udp_mainsock()
{
	int sockfd = socket(PF_INET, SOCK_DGRAM, 0);

	if (sockfd > 0)
	{
		int broadcast = 1;
		if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) == 0)
		{
			struct sockaddr_in peer_addr;
			memset(&peer_addr, 0, sizeof(peer_addr));
			peer_addr.sin_family = AF_LOCAL;
			peer_addr.sin_port = htons(MAINSOCKPORT);
			peer_addr.sin_addr.s_addr = inet_addr(BROADCASTIP);
			// Connect on a datagram socket simulate recvfrom with the address specified
			if (bind(sockfd, (struct sockaddr *)&peer_addr, sizeof(peer_addr)) == 0)
			{
				if (connect(sockfd, (struct sockaddr*)&peer_addr, sizeof(peer_addr)) == 0)
				{
				}
				else
				{
					close(sockfd);
					sockfd = -1;
				}
			}
			else
			{
				close(sockfd);
				sockfd = -2;
			}
		}
		else
		{
			close(sockfd);
			sockfd = -1;
		}
	}
	return sockfd;
}

int make_client_mainsock()
{
	int sockfd = socket(PF_LOCAL, SOCK_STREAM, 0);

	if (sockfd > 0)
	{
		struct sockaddr_un peer_addr;

		peer_addr.sun_family = AF_LOCAL;
		strncpy(&peer_addr.sun_path[0], path_server_mainsock, (strlen(path_server_mainsock)+1));

		connect(sockfd, (struct sockaddr*)&peer_addr, sizeof(peer_addr));
	}
	return sockfd;
}

int make_client_transock(int port)
{
	int sockfd = socket(PF_LOCAL, SOCK_STREAM, 0);

	if (sockfd > 0)
	{
		struct sockaddr_un peer_addr;
		peer_addr.sun_family = AF_LOCAL;
		if (port == TRANSOCKPORT4READ)
			strncpy(&peer_addr.sun_path[0], path_socket4read, (strlen(path_socket4read)+1));
		else if (port == TRANSOCKPORT4WRITE)
			strncpy(&peer_addr.sun_path[0], path_socket4write, (strlen(path_socket4write)+1));
		else if (port == TRANSOCKPORT4CALIBRATE)
			strncpy(&peer_addr.sun_path[0], path_socket4calibration, (strlen(path_socket4calibration)+1));
		else
		{
			close(sockfd);
			return -1;
		}

		if (connect(sockfd, (struct sockaddr*)&peer_addr, sizeof(peer_addr)) != 0)
		{
			close(sockfd);
			return -1;
		}
	}
	return sockfd;
}


int make_server_mainsock()
{
	int sockfd = socket(PF_LOCAL, SOCK_STREAM, 0);
	struct sockaddr_un my_addr;

	unlink (path_server_mainsock);

	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sun_family = AF_LOCAL;
	strncpy(&my_addr.sun_path[0], path_server_mainsock, (strlen(path_server_mainsock)+1));

	if (sockfd > 0)
	{

		if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == 0)
		{
			if (listen(sockfd, 0) == 0)
			{
				// No error
			}
			else
			{
				unlink (path_server_mainsock);
				close(sockfd);
				sockfd = -3;
			}
		}
		else
		{
			close(sockfd);
			sockfd = -2;
		}
	}

	return sockfd;
}


int make_server_transock(int port)
{
	int sockfd = socket(PF_LOCAL, SOCK_STREAM, 0);

	struct sockaddr_un my_addr;

	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sun_family = AF_LOCAL;
	if (port == TRANSOCKPORT4READ)
	{
		unlink (path_socket4read);
		strncpy(&my_addr.sun_path[0], path_socket4read, (strlen(path_socket4read)+1));
	}
	else if (port == TRANSOCKPORT4WRITE)
	{
		unlink (path_socket4write);
		strncpy(&my_addr.sun_path[0], path_socket4write, (strlen(path_socket4write)+1));
	}
	else if (port == TRANSOCKPORT4CALIBRATE)
	{
		unlink (path_socket4calibration);
		strncpy(&my_addr.sun_path[0], path_socket4calibration, (strlen(path_socket4calibration)+1));
	}
	else
	{
		if(sockfd > 0)
			close(sockfd);
		return -1;
	}

	if (sockfd > 0)
	{
		if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == 0)
		{
			if (listen(sockfd, 0) == 0)
			{
				// No error
			}
			else
			{
				close(sockfd);
				sockfd = -3;
			}
		}
		else
		{
			close(sockfd);
			sockfd = -2;
		}
	}

	return sockfd;
}


int accept_socket(int sockfd)
{
	// Block until connections
	struct sockaddr_un peer_addr;
	socklen_t sin_size = sizeof(peer_addr);
	int new_fd = accept(sockfd, (struct sockaddr *)&peer_addr, &sin_size);

	return new_fd;
}

void setup_socket(int sockfd)
{
	// Timeouts
	struct timeval t = { 1, 0 };

	setsockopt( sockfd, SOL_SOCKET, SO_SNDTIMEO, &t, sizeof(t));
	setsockopt( sockfd, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(t));
}


int send_socket(int sockfd, void* buffer, int size)
{
	int result = -1;
	int ioffset = 0;

	while (sockfd > 0 && ioffset < size)
	{
		result = send(sockfd, ((char*)buffer) + ioffset, size - ioffset, MSG_NOSIGNAL);
		if (result > 0)
		{
			ioffset += result;
		}
		else
		{
			break;
		}
	}
	return result;

}

int recv_socket(int sockfd, void* buffer, int size)
{
	int received = 0;

	while (buffer && received < size)
	{
		int result = recv(sockfd, buffer + received, size - received, MSG_NOSIGNAL);
		if (result > 0)
		{
			received += result;
		}
		else
		{
			received = result;
			break;
		}
	}
	return received;
}



