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
#include <termios.h>
#include <assert.h>
#include "citty.h"
#include "sim_log.h"
//#define SIM_DEBUG


#define DATABITS       CS8
//#define BAUD        B38400
#define BAUD            B115200
#define STOPBITS        0
#define PARITYON        0
#define PARITY          0

#define SERIAL_PORT1_NAME "/dev/citty10"
static struct at_response resp;
static int AddtoList(struct citty_dev *dev,struct at_request *req,struct sockaddr_un from);
static void check_timeout(struct citty_dev *dev,int sockfd);
static void handle_resp(struct citty_dev *dev,int sockfd);
static void try_write_req(struct citty_dev *dev);

#ifdef SIM_DEBUG
static int create_dev()
{
	int fd = socket(PF_UNIX,SOCK_STREAM,0);
	struct sockaddr_un addr;
	memset(&addr,0,sizeof(addr));
	addr.sun_family = AF_UNIX;
	snprintf(addr.sun_path,sizeof(addr.sun_path),"%s","/dev/dev_test");
	int n = connect(fd,(struct sockaddr *)&addr,sizeof(addr));
	if(n < 0){
		perror("connect server error");
		return -1;
	}
	return fd;
}
#endif
static int open_dev()
{
	int fd = 0;
#ifdef SIM_DEBUG
	fd = create_dev(); 
	return fd;
#endif
    struct termios oldtio, newtio;       //place for old and new port settings for serial port

    /*
     *  Open the Device
     */
    //serialfd = utlOpenIO(acDeviceName);

    /*open the device(com port) to be non-blocking (read will return immediately) */
	sim_printf(MSG_DEBUG,"now we open %s\n",SERIAL_PORT1_NAME);
	fd = open(SERIAL_PORT1_NAME, O_RDWR   ); //| O_NOCTTY

    if (fd <= 0)
    {
        sim_printf(MSG_ERROR,"Error opening device:%s",strerror(errno));
        return -1;
    }


    // Make the file descriptor asynchronous (the manual page says only
    // O_APPEND and O_NONBLOCK, will work with F_SETFL...)

    fcntl(fd, F_SETFL, 0);

    tcgetattr(fd, &oldtio); // save current port settings

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

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);
	return fd;

}
int open_tty(struct citty_dev *dev)
{
	int retry = 0;
	memset(dev,0,sizeof(*dev));
	while(retry++ < 100){
		dev->fd = open_dev();
		if(dev->fd > 0)
			break;
		sleep(1);
	}
	if(dev->fd < 0){
		return -1;
	}
	INIT_LIST_HEAD(&dev->req_head);
	INIT_LIST_HEAD(&dev->ind_head);
	//init list	
	dev->AddtoList = AddtoList;	
	dev->check_timeout = check_timeout;
	dev->handle_resp = handle_resp;
	dev->try_write_req = try_write_req;
	dev->state = DEV_STATE_WRITEABLE;
	return 0;
}

int close_tty(struct citty_dev *dev)
{
	///XXX : may be here need free all requester
	close(dev->fd);
#if 0
	assert(dev->reqIndH->next == dev->reqIndH->prev);
	free(dev->reqIndH);
	assert(dev->reqRespH->next == dev->reqRespH->prev);
	free(dev->reqRespH);
#endif
	return 0;
}
#define MAX_RESP_TIME 120
static void try_write_req(struct citty_dev *dev)
{
	time_t now = time(NULL);	
	if(dev->state != DEV_STATE_WRITEABLE)
		return;
	if(list_empty(&dev->req_head)){
		return ;
	}
	//get the first node
	struct reqnode *node = container_of((&dev->req_head)->next,typeof(*node),list);
	int n = write(dev->fd,node->req->buffer,node->req->head.len);
	sim_printf(MSG_DEBUG,"write %s %d bytes to dev\n",node->req->buffer,n);
	assert(n == node->req->head.len);
	dev->state = DEV_STATE_WAITRESP;
	dev->expire_time = now + MAX_RESP_TIME; 
	return ;
}
static int ReadAble(int fd,int timeout){
	struct timeval x;
	fd_set rset;
	FD_ZERO(&rset);
	FD_SET(fd,&rset);
	x.tv_sec = timeout;
	x.tv_usec = 0;
	int ret = select(fd+1, &rset, NULL , NULL, &x);
	if(ret == 0)
		return 0;
	return 1;
}
static int ReadAll(int fd,char *buffer,int len)
{
#if 0
	int n = read(fd,buffer,len);
		if(n <= 0)
				return -1;
		return n;
#endif
#if 1
	char *ptr = buffer;
	int count = 0;
	while(1){
		int retry = 0;
		if(ReadAble(fd,1)==0){
			if(retry++ > 3){
				buffer[count] = 0;
				sim_printf(MSG_DEBUG,"read fd timeout n = %d,buffer = %s\n",count,buffer);
				return count;
			} 
		}
		int n = read(fd,ptr,len);
		if(n <= 0){
			sim_printf(MSG_DEBUG,"read fd timeout n = %d\n",count);
			return -1;
		}
		ptr += n;
		count += n;
		//sim_printf(MSG_DEBUG,"n = %d,buffer = %s\n",count,buffer);
		if(count <= 4)
			continue;
//		sim_printf(MSG_DEBUG,"%x,%x,%x,%x,%x,%x\n",buffer[0],buffer[1],buffer[count-1],buffer[count-2],buffer[count-3],buffer[count-4]);
		if((buffer[count-1] == '\n')&&(buffer[count-3] != '\n')){
			buffer[count] = 0;
		//	sim_printf(MSG_DEBUG,"ReadAll finish = %s\n",buffer);
			break;
		}else if(strstr(buffer,"\r\nOK")){
			buffer[count] = 0;
			break;
		}else if(strstr(buffer,"\r\n+CMS ERROR")){
			buffer[count] = 0;
			break;
		}else if(strstr(buffer,"\r\n+CME ERROR")){
			buffer[count] = 0;
		//	sim_printf(MSG_DEBUG,"ReadAll finish = %s\n",buffer);
			break;
		}
	}
	sim_printf(MSG_DEBUG,"read fd timeout n = %d,buffer = %s\n",count,buffer);
	return count;
#endif
}
static int testind(char *req,char *resp)
{
	if(resp[2] != '+')
		return 0;
	char *ptr = &resp[2];
	if(strstr(ptr,"CMS ERROR:")){
		sim_printf(MSG_ERROR,"get error resp,req = %s,resp = %s",req,resp);
		return 0;
	}else if(strstr(ptr,"CME ERROR")){
		sim_printf(MSG_ERROR,"get error resp,req = %s,resp = %s",req,resp);
		return 0;
	}
	char *ptr2 = strstr(ptr,":");
	if(ptr2 == NULL)
		return -1;
	char atcmd[32];
	int len = ptr2 - ptr;
	if(len > 31)
		len = 31;
	memcpy(atcmd,ptr,len);
	atcmd[len] = 0;
	sim_printf(MSG_DEBUG,"atcmd = %s\n",atcmd);
	if(strstr(req,atcmd))
		return 0;

	return 1;
}
static int inIndList(tmp)
{
#if 0
	if(strncmp(tmp,"\r\n+ind",4)==0){
		printf("a ind comming\n");
		return 1;
	}
#endif
	return 0;
}

static int isSkipOk(char *req,char *resp)
{
	if((strncmp(req,"AT+CIMI",7)==0)&&(strstr(resp,"\r\nOK\r\n")==0)){
		sim_printf(MSG_DEBUG,"isSkipOk !\n");
		return 1;
	}
	if((strncmp(req,"AT+CRSM",7)==0)&&(strstr(resp,"\r\nOK\r\n")==0)){
		sim_printf(MSG_DEBUG,"isSkipOk !\n");
		return 1;
	}
	if((strncmp(req,"AT+CSIM",7)==0)&&(strstr(resp,"\r\nOK\r\n")==0)){
		sim_printf(MSG_DEBUG,"isSkipOk !\n");
		return 1;
	}
	if((strncmp(req,"AT+CREG",7)==0)&&(strstr(resp,"\r\nOK\r\n")==0)){
		sim_printf(MSG_DEBUG,"isSkipOk !\n");
		return 1;
	}
	if((strncmp(req,"AT+ERGA",7)==0)&&(strstr(resp,"\r\nOK\r\n")==0)){
		sim_printf(MSG_DEBUG,"isSkipOk !\n");
		return 1;
	}
	if((strncmp(req,"AT+CPIN?",8)==0)&&(strstr(resp,"\r\nOK\r\n")==0)){
		sim_printf(MSG_DEBUG,"isSkipOk !\n");
		return 1;
	}
	if((strncmp(req,"AT*EUICC",8)==0)&&(strstr(resp,"*EUICC: ")!=0)&&(strstr(resp,"\r\nOK\r\n")==0)){
		sim_printf(MSG_DEBUG,"isSkipOk !\n");
		return 1;
	}
	return 0;
}

static void handle_resp(struct citty_dev *dev,int sockfd)
{
	int ind = 0;
	static char buffer[1024];
	char tmp[512];
	static int SkipOk = 0;
	int n = ReadAll(dev->fd,buffer,sizeof(buffer));
	if(n < 0){
		close(dev->fd);
		dev->fd = -1;
		return ;
	}
	int len = 0;
	int count = n;
	char *cur = buffer;
	int finish = 0;
//	sim_printf(MSG_DEBUG,"buffer = %s\n",buffer);
	while(finish == 0){
		char *ptr = strstr(cur,"\r\n\r\n");
		if((ptr == NULL)||(strncmp(ptr+4,"+CPIN",5)==0)){
			memcpy(tmp,cur,count);
			tmp[count] = 0;
			finish = 1;
		}else{
			sim_printf(MSG_DEBUG,"had not finish\n");
			len = (ptr - cur) + 2;
			count -= len;
			memcpy(tmp,cur,len);
			tmp[len] = 0;
			sim_printf(MSG_DEBUG,"tmp = %s\n",tmp);
			cur = ptr + 2;
		}
		int len2 = strlen(tmp);
		if(len2 < 5){
			continue;
		}
		if( (SkipOk == 1) && (tmp[2] == 'O') && (tmp[3] == 'K')){
			sim_printf(MSG_DEBUG,"skip a ok \n");
			continue;
		}
		if(list_empty(&dev->req_head)){
			///must be ind
			if(inIndList(tmp)){
				resp.head.type = TYPE_IND;
				resp.head.len = snprintf(resp.buffer,sizeof(resp.buffer),"%s",tmp);
				resp.head.ret = RESP_OK;
				//send to all ind listener
				struct reqnode *node,*tmp;
				list_for_each_entry_safe(node,tmp,&dev->ind_head, list){
					int len = sizeof(resp.head) + resp.head.len;
					sim_printf(MSG_DEBUG,"send ind to = %s\n",node->from.sun_path);
					n = sendto(sockfd,&resp,len,0,(struct sockaddr *)&node->from,sizeof(node->from));
					if(n < 0){
						list_del(&node->list);
						free(node->req);
						free(node);	
					}
				}
			}

		}else{
			struct reqnode *node = container_of((&dev->req_head)->next,typeof(*node),list);
			ind = testind(node->req->buffer,tmp);
			if(ind < 0){
				sim_printf(MSG_ERROR,"error str from dev = %s\n",tmp);	
			}else if(ind == 0){
				///resp
				resp.head.type = TYPE_RESP;
				resp.head.len = snprintf(resp.buffer,sizeof(resp.buffer),"%s",tmp);
				resp.head.ret = RESP_OK;
				SkipOk = isSkipOk(node->req->buffer,resp.buffer);
				sim_printf(MSG_DEBUG,"write resp to clie %d bytes : %s\n",resp.head.len,resp.buffer);
#if 0	
				for(i = 0;i<resp.head.len;i++){
					if(resp.head.len % 16 == 0)
						printf("\n");
					printf("%02x ",resp.buffer[i]);
				}
				printf("\n");
#endif
				// send to requester
				struct reqnode *node = container_of((&dev->req_head)->next,typeof(*node),list);
				int len = sizeof(resp.head) + resp.head.len;
				sendto(sockfd,&resp,len,0,(struct sockaddr *)&node->from,sizeof(node->from));
				list_del(&node->list);
				free(node->req);
				free(node);	
				dev->state = DEV_STATE_WRITEABLE;	
			}else{
				///ind
				if(inIndList(tmp)){
					resp.head.type = TYPE_IND;
					resp.head.len = snprintf(resp.buffer,sizeof(resp.buffer),"%s",tmp);
					resp.head.ret = RESP_OK;
					//send to all ind listener
					struct reqnode *node,*tmp;
					list_for_each_entry_safe(node,tmp,&dev->ind_head, list){
							int len = sizeof(resp.head) + resp.head.len;
							n = sendto(sockfd,&resp,len,0,(struct sockaddr *)&node->from,sizeof(node->from));
							if(n < 0){
									list_del(&node->list);
									free(node->req);
									free(node);	
							}
					}
				}
			}
		}
	}
}
static void check_timeout(struct citty_dev *dev,int sockfd)
{
	time_t now = time(NULL);
	struct reqnode *node,*tmp;
	int n = 0;
	if(list_empty(&dev->req_head))
		return ;
	if(dev->expire_time < now){
		//1.build_resp
		//2.send to every requester
		//3.del requester from list
		sim_printf(MSG_ERROR,"dev timeout now = %d,expired at %d\n",(int)now,(int)dev->expire_time);
		resp.head.type = TYPE_RESP;
		resp.head.len = 0;
		resp.head.ret = RESP_ERROR;
		int len = sizeof(resp.head) + resp.head.len;
		list_for_each_entry_safe(node,tmp,&dev->req_head, list){
			n = sendto(sockfd,&resp,len,0,(struct sockaddr *)&node->from,sizeof(node->from));
			if( n < 0){
				sim_printf(MSG_ERROR,"send RESP_ERROR to %s error:%s\n",node->req->buffer,strerror(errno));
			}else{
				sim_printf(MSG_ERROR,"send RESP_ERROR to %s success\n",node->req->buffer);
			}
			list_del(&node->list);
			free(node->req);
			free(node);	
		}
		dev->state = DEV_STATE_WRITEABLE;
		return ;	
	}
	list_for_each_entry_safe(node,tmp,&dev->req_head, list){
		if(node->expire_time < now){
			//1.build resp
			//2.send to the request
			//3.del requester from list
			sim_printf(MSG_ERROR,"node timeout now = %d,expired at %d\n",(int)now,(int)node->expire_time);
			resp.head.type = TYPE_RESP;
			resp.head.len = 0;
			resp.head.ret = RESP_TIMEOUT;
			int len = sizeof(resp.head) + resp.head.len;
			n = sendto(sockfd,&resp,len,0,(struct sockaddr *)&node->from,sizeof(node->from));
			if( n < 0){
				sim_printf(MSG_ERROR,"send RESP_ERROR to %s error:%s\n",node->req->buffer,strerror(errno));
			}else{
				sim_printf(MSG_ERROR,"send RESP_ERROR to %s success\n",node->req->buffer);
			}
			list_del(&node->list);
			free(node->req);
			free(node);	
		}
	}
	return ;
}
static int AddtoList(struct citty_dev *dev,struct at_request *req,struct sockaddr_un from)
{
	//add new request to tail
	struct reqnode *newnode = malloc(sizeof(struct reqnode));
	if(newnode == NULL)
		return -1;
	memset(newnode,0,sizeof(*newnode));
	newnode->req = req;
	newnode->from = from;
	if(req->head.type == TYPE_RESP){
		list_add_tail(&newnode->list,&dev->req_head);
		newnode->expire_time = time(NULL)+newnode->req->head.timeout;
	}else if(req->head.type == TYPE_IND){
		list_add_tail(&newnode->list,&dev->ind_head);
	}
	sim_printf(MSG_DEBUG,"show current requester list:\n");
	struct reqnode *node;
	list_for_each_entry(node,&dev->req_head, list){
		sim_printf(MSG_DEBUG,"req = %s\n",node->req->buffer);
	}
	return 0;

}
