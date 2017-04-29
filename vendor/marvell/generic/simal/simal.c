/*
* All Rights Reserved
*
* MARVELL CONFIDENTIAL
* Copyright 2008 ~ 2010 Marvell International Ltd All Rights Reserved.
* The source code contained or described herein and all documents related to
* the source code ("Material") are owned by Marvell International Ltd or its
* suppliers or licensors. Title to the Material remains with Marvell International Ltd
* or its suppliers and licensors. The Material contains trade secrets and
* proprietary and confidential information of Marvell or its suppliers and
* licensors. The Material is protected by worldwide copyright and trade secret
* laws and treaty provisions. No part of the Material may be used, copied,
* reproduced, modified, published, uploaded, posted, transmitted, distributed,
* or disclosed in any way without Marvell's prior express written permission.
*
* No license under any patent, copyright, trade secret or other intellectual
* property right is granted to or conferred upon you by disclosure or delivery
* of the Materials, either expressly, by implication, inducement, estoppel or
* otherwise. Any license under such intellectual property rights must be
* express and approved by Marvell in writing.
*
*/
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include "simal.h"
#include "citty.h"
#include <sys/time.h>
#include <sys/resource.h>
#include "sim_log.h"
extern int close_tty(struct citty_dev *dev);
static int print_req(struct at_request *req)
{
//	assert(n == req->head.len + sizeof(req->head));
	//sim_printf(MSG_DEBUG,"type = %d len = %d timeout = %d,buffer = %s\n",req->head.type,req->head.len,req->head.timeout,req->buffer);
	sim_printf(MSG_DEBUG,"type = %d len = %d timeout = %d,buffer = %s\n",req->head.type,req->head.len,req->head.timeout,req->buffer);
	return 0;
}

static int create_socket(char *path)
{
	struct sockaddr_un addr;
	int fd = socket(PF_UNIX, SOCK_DGRAM, 0);
	if(fd < 0){
		sim_printf(MSG_ERROR,"socket build error %s,%d",__FILE__,__LINE__);
		return -1;
	}
	addr.sun_family = AF_UNIX;
	if(path == NULL)
		snprintf(addr.sun_path,sizeof(addr.sun_path),"/data/misc/simal/simal_s_%d",getpid());
	else{
		snprintf(addr.sun_path,sizeof(addr.sun_path),"%s",path);
	}
	int ret = unlink(addr.sun_path);
	if (ret != 0 && errno != ENOENT) {
		return -1;
	}
	if(bind(fd,(struct sockaddr *)(&addr),sizeof(addr))<0)
		return -1;
	return fd;
#if 0
	int flags = fcntl(fd,F_GETFL,0);
	flags |= O_NONBLOCK;
	fcntl(fd,F_SETFL,flags);
	return fd;
#endif
}
static int Running = 0;
static void signal_cancel(int sig)
{
	Running = 0;
}
#if ENABLE_COREDUMP
static void enable_coredump()
{
	struct rlimit l;
	l.rlim_cur = l.rlim_max = RLIM_INFINITY;
	setrlimit(RLIMIT_CORE,&l);
	system("echo \'/data/data/core_%e.%p\' > /proc/sys/kernel/core_pattern ");
	sim_printf(MSG_DEBUG,"core dump enable");
}
#endif
int main()
{
#ifdef ENABLE_COREDUMP
	enable_coredump();
#endif
	struct citty_dev *dev = malloc(sizeof(struct citty_dev));
	int sockfd = create_socket(SIMAL_SRV);
	if(sockfd < 0){
		sim_printf(MSG_ERROR,"sockfd create fail = %s\n",strerror(errno));
		return 0;
	}
	if(open_tty(dev)<0){
		sim_printf(MSG_ERROR,"open dev fail\n");
		return 0;
	}

	Running = 1;
	fd_set rset;
	socklen_t clilen = 0;
	int ret = 0;
	int maxfd;
	struct timeval tm;
	
	struct sigaction sa;
	memset(&sa,0,sizeof(sa));
	sa.sa_handler = signal_cancel;
	sigaction(SIGINT, &sa, 0);
//	sigaction(SIGTERM, &sa, 0);
	
	while(Running){
		tm.tv_sec = 1;
		tm.tv_usec = 0;
#if 0
		if(dev->fd < 0){
			open_tty
		}
#endif
		///set the read set
		FD_ZERO(&rset);
		FD_SET(sockfd,&rset);
		FD_SET(dev->fd,&rset);
		maxfd = sockfd>dev->fd?sockfd:dev->fd;
		ret = select(maxfd+1, &rset, NULL , NULL, &tm);
		if(ret < 0){
			perror("select error");
			continue;
		}
		if(ret == 0){
			dev->check_timeout(dev,sockfd);
			continue;
		}	
		
		if(FD_ISSET(dev->fd,&rset))
		{
			//1.read one line from dev fd
			//2.handle this line
			//if it's an ind then:
			// send to ind receiver
			//if it's a resp then:
			// if it match a request,then:
			//   send resp to requester
			// else
			//	 discard
			dev->handle_resp(dev,sockfd);
		}
		if(FD_ISSET(sockfd,&rset))
		{
			sim_printf(MSG_DEBUG,"socket readable\n");
			struct sockaddr_un cliaddr;
			memset(&cliaddr,0,sizeof(cliaddr));
			struct at_request *req = malloc(sizeof(struct at_request));
			assert(req != NULL);
			if(req == NULL){
				sim_printf(MSG_ERROR,"crash ,out of memory in %s,%d\n",__FILE__,__LINE__);
				exit(0);
			}
			clilen = sizeof(cliaddr);
			int n = recvfrom(sockfd,req,sizeof(*req),0,(struct sockaddr *)&cliaddr,&clilen);
			req->buffer[req->head.len] = 0;
			assert(n >= req->head.len + sizeof(req->head));
			print_req(req);
			dev->AddtoList(dev,req,cliaddr);
		}
		dev->try_write_req(dev);
		dev->check_timeout(dev,sockfd);
	}
	close_tty(dev);
	close(sockfd);
	return 0;
}

