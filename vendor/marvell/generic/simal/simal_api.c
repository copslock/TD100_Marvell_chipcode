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
#include <sys/types.h>   
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "assert.h"
#include "simal.h"
#include "list.h"
#define _NO_CALLBACK_
enum { MSG_MSGDUMP, MSG_DEBUG, MSG_INFO, MSG_WARNING, MSG_ERROR };
#ifdef ANDROID
#include <android/log.h>
static void sim_printf(int level, char *format, ...){
	static int sim_debug_level = MSG_DEBUG;
	if (level >= sim_debug_level) {
		va_list ap;
		if (level == MSG_ERROR) {
				level = ANDROID_LOG_ERROR;
		} else if (level == MSG_WARNING) {
				level = ANDROID_LOG_WARN;
		} else if (level == MSG_INFO) {
				level = ANDROID_LOG_INFO;
		} else {
				level = ANDROID_LOG_DEBUG;
		}
		va_start(ap, format);
		__android_log_vprint(level, "simal_api", format, ap);
		va_end(ap);
	}

}
#else
#include <stdarg.h>
//void sim_printf(MSG_DEBUG,int level, char *format, ...)
void sim_printf(int level, char *format, ...)
{
	static int sim_debug_level = MSG_DEBUG;
	if (level >= sim_debug_level) {
		va_list ap;
		va_start(ap, format);
		vprintf(format, ap);
		va_end(ap);
	}
}
#endif
static int SendAndRecv(SimAL_Hd_T *pHd,struct at_request *req,struct at_response *resp,int timeout);
static int SimAL_getSimMNCCount(SimAL_Hd_T * pHd ,const char *imsi, int *mnc);
static void path2addr(char *path,struct sockaddr_un *addr)
{
	addr->sun_family = AF_UNIX;
	snprintf(addr->sun_path,sizeof(addr->sun_path),"%s",path);
	return ;
}

static int create_socket(char *path)
{
	struct sockaddr_un cliaddr;
	int fd = socket(PF_UNIX, SOCK_DGRAM, 0);
	static int count = 0;
	cliaddr.sun_family = AF_UNIX;
	char tmpfile[32];
	snprintf(tmpfile,sizeof(tmpfile),"/data/misc/simal/simal_s_%d_%d",count++,getpid());
	int ret = unlink(tmpfile);
	if (ret != 0 && errno != ENOENT) {
		return -1;
	}

	if(path == NULL)
		strcpy(cliaddr.sun_path,tmpfile);
	else
		snprintf(cliaddr.sun_path,sizeof(cliaddr.sun_path),"%s",path);
	bind(fd,(struct sockaddr *)(&cliaddr),sizeof(cliaddr));
	return fd;
#if 0
	int flags = fcntl(fd,F_GETFL,0);
	flags |= O_NONBLOCK;
	fcntl(fd,F_SETFL,flags);
	return fd;
#endif
}
static void lock_thread(pthread_mutex_t *lock)
{
	pthread_mutex_lock(lock);
}
static void unlock_thread(pthread_mutex_t *lock)
{
	pthread_mutex_unlock(lock);
}

static int buildstr(const unsigned char *_rand,int len,char *str)
{
	int i = 0;
	char *ptr = str;
	for(i = 0;i<len;i++){
		ptr += sprintf(ptr,"%02x",_rand[i]);
	}
	*ptr = 0;
	return 0;
}
static int str2bytes(unsigned char *_rand,int len,char *str)
{
	int i = 0;
	char tmp[4];
	char *ptr = str;
	tmp[2] = 0;
	for(i = 0;i<len;i++){
		memcpy(tmp,ptr,2);
		ptr += 2;
		_rand[i] = strtoul(tmp,NULL,16);
	}
	return 0;
}
struct cb_node{
	struct list_head list;
	SimAL_Status_Cb_T fn;
}; 
typedef struct simal_cb_handle{
	SimAL_Hd_T *sim_phd;
	int fd;
	int Running;
	struct sockaddr_un dest_addr;
	pthread_mutex_t lock;
	pthread_t threadid;
	struct list_head cb_list;
	void (*Lock)(pthread_mutex_t *lock);	
	void (*Unlock)(pthread_mutex_t *lock);
}SIMAL_CB_Hd_T;

static void showlist(SIMAL_CB_Hd_T *phd);
static void do_cb(SIMAL_CB_Hd_T *phd,SimAL_EVENT_TYPE_T type,unsigned int value)
{
	struct cb_node *tmp;
	list_for_each_entry(tmp, &phd->cb_list, list)
	{
		printf("comingggg");
		tmp->fn(phd->sim_phd,type,value);
	}
	return ;
}

static int WaitInd(int fd,struct at_response *resp,struct sockaddr_un *dest_addr)
{
	fd_set rset;
	FD_ZERO(&rset);
	FD_SET(fd,&rset);
	struct timeval x;
	int ret = 0;
	x.tv_sec = 5;
	x.tv_usec = 0;
#if 1
	ret = connect(fd,(struct sockaddr *)dest_addr,sizeof(*dest_addr));
	if(ret < 0){
		sim_printf(MSG_ERROR,"conn error \n");
		return ret;
	}
#endif
	ret = select(fd+1, &rset, NULL , NULL, &x);
	//ret = select(fd+1, &rset, NULL , NULL, NULL);
	if(ret < 0){
		sim_printf(MSG_ERROR,"select error\n");
		return ret;
	}
	if(ret == 0)
		return ret;
	int n = read(fd,resp->buffer,sizeof(resp->buffer));
	resp->buffer[n] = 0;
	printf("ind = %s\n",resp->buffer);
	return 1;
}
static void *cb_main(void *arg)
{
	SimAL_EVENT_TYPE_T type = 0;
	unsigned int value = 0;
	struct at_request *req = malloc(sizeof(*req));
	struct at_response *resp = malloc(sizeof(*resp));
	SIMAL_CB_Hd_T *phd = (SIMAL_CB_Hd_T *)arg;
	if(phd == NULL)
		return NULL;
	phd->Running = 1;
	req->head.timeout = -1;
	req->head.type = TYPE_IND;
	req->head.len = 0;
		
	int n = sendto(phd->fd,req,sizeof(req->head),0,(struct sockaddr *)&phd->dest_addr,sizeof(phd->dest_addr));
	if(n < 0){
		sim_printf(MSG_ERROR,"send error %s, %d",__FILE__,__LINE__);
	}
	while(phd->Running){
		int ret = WaitInd(phd->fd,resp,&phd->dest_addr);
		if(ret < 0)
			break;
		if(ret == 0)
			continue;
		phd->Lock(&phd->lock);
		do_cb(phd,type,value);
		phd->Unlock(&phd->lock);
	}
	free(req);
	free(resp);
	return NULL;
}
static void deinit_cb(SIMAL_CB_Hd_T *sim_phd)
{
	SIMAL_CB_Hd_T *phd = sim_phd;
	phd->Running = 0;
	pthread_join(phd->threadid,NULL);
	close(phd->fd);
}

static SIMAL_CB_Hd_T * init_cb(SimAL_Hd_T *sim_phd)
{
	SIMAL_CB_Hd_T *phd = malloc(sizeof(*phd));
	if(phd == NULL)
		return phd;
	phd->sim_phd = sim_phd; 
	phd->fd = create_socket(NULL);
	phd->dest_addr = sim_phd->dest_addr;	
	INIT_LIST_HEAD(&phd->cb_list);
	pthread_mutex_init(&phd->lock, NULL);
	phd->Lock = lock_thread;
	phd->Unlock = unlock_thread;
	if(pthread_create(&phd->threadid,NULL,cb_main,(void *)phd)!=0){
		perror("create thread error");
		free(phd);
		return NULL;
	}
	return phd;
}	

int SimAL_Init(SimAL_Hd_T *pHd)
{
	memset(pHd,0,sizeof(*pHd));
	int ret = pthread_mutex_init(&pHd->lock, NULL);
	if(ret){
		perror("pthread_mutex_init");
	}
	pHd->Lock = lock_thread;
	pHd->Unlock = unlock_thread;
	pHd->Lock(&pHd->lock);
	pHd->Unlock(&pHd->lock);
	pHd->fd = create_socket(NULL);
	if(pHd->fd <= 0)
		return SimAL_ERR_FAILURE;
	path2addr(SIMAL_SRV,&pHd->dest_addr);
#ifndef _NO_CALLBACK_
	pHd->cbhd = (void *)init_cb(pHd);
#endif
	return SimAL_SUCCESS;
}
static void del_cb(SIMAL_CB_Hd_T *phd,SimAL_Status_Cb_T cb_hd)
{
	struct cb_node *tmp,*tmp2;
	list_for_each_entry_safe(tmp,tmp2,&phd->cb_list, list){
		if(tmp->fn == cb_hd){
			list_del(&tmp->list);
			free(tmp);
			break;
		}
	}
	return;
}

static void add_cb(SIMAL_CB_Hd_T *phd,SimAL_Status_Cb_T cb_hd)
{
	struct cb_node *node = malloc(sizeof(*node));
	node->fn = cb_hd;
	list_add_tail(&node->list,&phd->cb_list);
}
static void showlist(SIMAL_CB_Hd_T *phd)
{
	struct cb_node *tmp;
	sim_printf(MSG_DEBUG,"show call back list:\n");
	list_for_each_entry(tmp, &phd->cb_list, list)
	{
		sim_printf(MSG_DEBUG,"fn = %x\n",(unsigned int)tmp->fn);
	}
	return ;
}
int SimAL_StatusCbRegister(SimAL_Hd_T *phd,SimAL_Status_Cb_T fn)
{
#ifdef _NO_CALLBACK_
	return 0;
#endif
	SIMAL_CB_Hd_T *cb_hd = (SIMAL_CB_Hd_T *)phd->cbhd;
	cb_hd->Lock(&cb_hd->lock);
	add_cb(cb_hd,fn);
	showlist((SIMAL_CB_Hd_T *)phd->cbhd);
	cb_hd->Unlock(&cb_hd->lock);
	return 0;
}
int SimAL_StatusCbUnRegister(SimAL_Hd_T *phd,SimAL_Status_Cb_T fn)
{
#ifdef _NO_CALLBACK_
	return 0;
#endif
	SIMAL_CB_Hd_T *cb_hd = (SIMAL_CB_Hd_T *)phd->cbhd;
	cb_hd->Lock(&cb_hd->lock);
	del_cb((SIMAL_CB_Hd_T *)phd->cbhd,fn);
	showlist((SIMAL_CB_Hd_T *)phd->cbhd);
	cb_hd->Unlock(&cb_hd->lock);
	return 0;
}
int SimAL_Shutdown (SimAL_Hd_T *pHd)
{
	pHd->Lock(&pHd->lock);
	close(pHd->fd);
	if(pHd->cbhd){
		deinit_cb(pHd->cbhd);
		free(pHd->cbhd);
	}
	pHd->Unlock(&pHd->lock);
	pthread_mutex_destroy(&pHd->lock);
	return SimAL_SUCCESS;
}
static int parse_type(char *buffer,int len,int *type)
{
	char *ptr = buffer;
	int n = 0;
	ptr += 2;
	if(strncmp(ptr,"*EUICC:",7)==0){
		ptr += 7;
		while(*ptr == ' ')
			ptr++;
		sim_printf(MSG_DEBUG,"ptr = %s\n",ptr);
		n  = atoi(ptr);
		if(n == 0)
			*type = TYPE_SIM;
		else
			*type = TYPE_USIM;
		return 0;
	}
	*type = TYPE_ERROR;
	return -1;
}

int SimAL_GetType(SimAL_Hd_T *pHd, int *type)
{
	struct at_response resp;
	struct at_request req;
	int ret = SimAL_ERR_NOTREADY;
#if 0
	*type = TYPE_SIM;
	return SimAL_SUCCESS;
#endif
	if(pHd->fd <= 0)
		return ret; 
	req.head.len = snprintf(req.buffer,sizeof(req.buffer),"AT*EUICC?\r\n");
	req.head.type = TYPE_RESP;
	req.head.timeout = 5;
	pHd->Lock(&pHd->lock);
	int n = SendAndRecv(pHd,&req,&resp,req.head.timeout); 
	if(n == 0){
		if(resp.head.ret == RESP_OK){
			n = parse_type(resp.buffer,resp.head.len,type);
			if(n < 0)
				ret = SimAL_ERR_UNKNOWN_RESP;
			else
				ret = SimAL_SUCCESS; 
		}
	}else{
		ret = SimAL_ERR_FAILURE; 
	}
	pHd->Unlock(&pHd->lock);

	return ret;

}

static int SendAndRecv(SimAL_Hd_T *pHd,struct at_request *req,struct at_response *resp,int timeout)
{
	// return 0 when read a resp
	// return -1 when timeout or any socket error
	int ret = 0,n = 0;
	struct timeval tm;
	ret = connect(pHd->fd,(struct sockaddr *)&pHd->dest_addr,sizeof(pHd->dest_addr));
	if(ret < 0){
		sim_printf(MSG_DEBUG,"connect error = [%d]%s\n", errno, strerror(errno));
		return -1;
//		perror("connect error");
	}
	n = sizeof(req->head)+req->head.len;
	ret = sendto(pHd->fd,req,n,0,(struct sockaddr *)&pHd->dest_addr,sizeof(pHd->dest_addr));
	if(ret < 0){
		sim_printf(MSG_DEBUG,"send req fail");
		return -1;
	}
//	printf("send ret = %d\n",ret);
	tm.tv_sec = timeout+10;
	tm.tv_usec = 0;
	fd_set rset;
	FD_ZERO(&rset);
	FD_SET(pHd->fd,&rset);
	ret = select(pHd->fd+1,&rset,NULL,NULL,&tm);

	sim_printf(MSG_DEBUG,"select ret = %d\n",ret);
	if(ret == 0)
		return -1;
	if(ret < 0)
		return -1;
	n = read(pHd->fd,resp,sizeof(*resp));
	sim_printf(MSG_DEBUG,"read ret = %d\n",ret);
	if(n < 0)
		return -1;
	resp->buffer[resp->head.len] = 0;
	sim_printf(MSG_DEBUG,"SendAndRecv read %d byte from srv = %s\n",n,resp->buffer);
	return 0;

}
static int parse_needpin(char *buffer,int len,int *need)
{
	char *ptr = NULL;
	buffer += 2;
	if(strncmp(buffer,"+CPIN: ",7)==0){
		ptr = buffer + 7;
		if(strstr(ptr,"READY")){
			*need = 0;
		}else{
			*need = 1;
		}
		return 0;
	}else{
		return -1;
	}
	return -1;
}
int SimAL_NeedSetPin(SimAL_Hd_T *pHd,int *needpin)
{
#if 1
	*needpin = 0;
	return SimAL_SUCCESS;
#endif
	struct at_response resp;
	struct at_request req;
	int ret = SimAL_ERR_NOTREADY;
	req.head.len = snprintf(req.buffer,sizeof(req.buffer),"AT+CPIN?\r\n");
	req.head.type = TYPE_RESP;
	req.head.timeout = 5;

	pHd->Lock(&pHd->lock);
	int n = SendAndRecv(pHd,&req,&resp,req.head.timeout); 
	if(n == 0){
		if(resp.head.ret == RESP_OK){
			n = parse_needpin(resp.buffer,resp.head.len,needpin);
			if(n < 0)
				ret = SimAL_ERR_UNKNOWN_RESP;
			else
				ret = SimAL_SUCCESS; 
		}
	}else{
		ret = SimAL_ERR_FAILURE; 
	}
	pHd->Unlock(&pHd->lock);
	return ret;
}
#if 0
static int varify_pin(char *buffer,int len)
{
	buffer += 2;
	if(strncmp(buffer,"OK",2)== 0)
		return 0;
	return -1;
}
int SimAL_VarifyPin(SimAL_Hd_T *pHd,const char *pin)
{
	struct at_response resp;
	struct at_request req;
	int ret = SimAL_ERR_NOTREADY;
	req.head.len = snprintf(req.buffer,sizeof(req.buffer),"AT+CLCK=\"PN\",0,\"%s\"\r\n",pin);
	req.head.type = TYPE_RESP;
	req.head.timeout = 5;

	pHd->Lock(&pHd->lock);
	int n = SendAndRecv(pHd,&req,&resp,req.head.timeout); 
	if(n == 0){
		if(resp.head.ret == RESP_OK){
			n = parse_setpin(resp.buffer,resp.head.len);
			if(n < 0)
				ret = SimAL_ERR_UNKNOWN_RESP;
			else
				ret = SimAL_SUCCESS; 
		}
	}else{
		ret = SimAL_ERR_FAILURE; 
	}
	pHd->Unlock(&pHd->lock);

	return ret;

}
#endif
static int parse_setpin(char *buffer,int len)
{
	buffer += 2;
	if(strncmp(buffer,"OK",2)== 0)
		return 0;
	return -1;
}
int SimAL_SetPin(SimAL_Hd_T *pHd,const char *pin)
{
	struct at_response resp;
	struct at_request req;
	int ret = SimAL_ERR_NOTREADY;
	req.head.len = snprintf(req.buffer,sizeof(req.buffer),"AT+CPIN=\"%s\"\r\n",pin);
	req.head.type = TYPE_RESP;
	req.head.timeout = 5;

	pHd->Lock(&pHd->lock);
	int n = SendAndRecv(pHd,&req,&resp,req.head.timeout); 
	if(n == 0){
		if(resp.head.ret == RESP_OK){
			n = parse_setpin(resp.buffer,resp.head.len);
			if(n < 0)
				ret = SimAL_ERR_UNKNOWN_RESP;
			else
				ret = SimAL_SUCCESS; 
		}
	}else{
		ret = SimAL_ERR_FAILURE; 
	}
	pHd->Unlock(&pHd->lock);

	return ret;

}
static int parse_imsi(char *buffer,int len,char *imsi, size_t *imsi_len)
{
	///AT+CIMI's resp
	///<IMSI>\r\nOK\r\n
	buffer += 2;
	sim_printf(MSG_DEBUG,"buffer = %s\n",buffer);
	if(strstr(buffer,"+CME ERROR")){
		return -1;
	}
	char *ptr = strstr(buffer,"\r\n");
	if(ptr == NULL)
		return -1;
	int len2 = ptr - buffer;
#if 0
	if(len2+4  != len)
		return -1;
#endif
	sim_printf(MSG_DEBUG,"len2 = %d\n",len2);
	if(len2 >= *imsi_len){
		sim_printf(MSG_DEBUG,"imsi buffer too small,need %d bytes\n",len2);
	}
	memcpy(imsi,buffer,len2);
	imsi[len2] = 0;
	*imsi_len = len2;
	return 0;
}

int SimAL_GetImsi (SimAL_Hd_T *pHd, char *imsi, size_t *len)
{
	struct at_response resp;
	struct at_request req;
	int ret = SimAL_ERR_NOTREADY;
	if(pHd->fd <= 0)
		return ret;
	req.head.len = snprintf(req.buffer,sizeof(req.buffer),"AT+CIMI\r\n");
	req.head.type = TYPE_RESP;
	req.head.timeout = 5;	

	pHd->Lock(&pHd->lock);
	int n = SendAndRecv(pHd,&req,&resp,req.head.timeout); 
	if(n == 0){
		if(resp.head.ret == RESP_OK){
			n = parse_imsi(resp.buffer,resp.head.len,imsi,len);
			if(n != 0)
				ret = SimAL_ERR_UNKNOWN_RESP;
			else
				ret = SimAL_SUCCESS; 
		}
	}else{
		ret = SimAL_ERR_FAILURE; 
	}
	pHd->Unlock(&pHd->lock);

	return ret;

}
static int parse_gsmauth(char *buffer,int len,unsigned char *sres, unsigned char *kc)
{
	buffer += 2;//skip \r\n
	if(strncmp(buffer,"+ERGA",5)==0){
		sim_printf(MSG_DEBUG,"RES= %s\n",buffer + 6);
		str2bytes(sres,4,buffer + 6);
		sim_printf(MSG_DEBUG,"RES= %s\n",buffer + 6 +9);
		str2bytes(kc,8,buffer + 6 + 9);
		return 0;
	}
	return -1;
}
int SimAL_SimGsmAuth(SimAL_Hd_T * pHd ,const unsigned char *_rand, unsigned char *sres, unsigned char *kc)
{
	struct at_response resp;
	struct at_request req;
	char randstr[64];
	int ret = SimAL_ERR_NOTREADY;
	if(pHd->fd <= 0)
		return ret; 
	buildstr(_rand,16,randstr);
	req.head.len = snprintf(req.buffer,sizeof(req.buffer),"AT+ERGA=\"%s\"\r\n",randstr);
	req.head.type = TYPE_RESP;
	req.head.timeout = 5;	

	pHd->Lock(&pHd->lock);
	int n = SendAndRecv(pHd,&req,&resp,req.head.timeout); 
	if(n == 0){
		if(resp.head.ret == RESP_OK){
			n = parse_gsmauth(resp.buffer,resp.head.len,sres,kc);
			if(n < 0)
				ret = SimAL_ERR_UNKNOWN_RESP;
			else
				ret = SimAL_SUCCESS; 
		}
	}else{
		ret = SimAL_ERR_FAILURE; 
	}
	pHd->Unlock(&pHd->lock);

	return ret;
}
#if 0
static int parse_utmsauth(char *buffer,int len,int *status,unsigned char *res, size_t *res_len, unsigned char *ik, unsigned char *ck, unsigned char *auts)
{
	
	buffer += 2;
	if(strncmp(buffer,"+ERTCA",6)==0){
		sim_printf(MSG_DEBUG,"resp = %s\n",buffer);
		sim_printf(MSG_DEBUG,"status  = %s\n",buffer+7);
		*status = atoi(buffer+7);
		if(*status == 0){
			char *ptr = strstr(buffer+9,",");
			*res_len = (ptr-buffer-9)/2;
			sim_printf(MSG_DEBUG,"res_len = %d\n",*res_len);
			str2bytes(res,*res_len,buffer+9);
			ptr = buffer+9+(*res_len)*2+1;
			sim_printf(MSG_DEBUG,"ck = %s\n",ptr);
			str2bytes(ck,16,ptr);
			int i = 0;
			for(i = 0;i<16;i++){
				printf("%02x ",ck[i]);
			}
			printf("\n");
			ptr += 33;
			sim_printf(MSG_DEBUG,"ik = %s\n",ptr);
			str2bytes(ik,16,ptr);
			for(i = 0;i<16;i++){
				printf("%02x ",ik[i]);
			}
			printf("\n");
		}else if(*status == 1){
			*res_len = 0;
			str2bytes(auts,14,buffer + 8 + 2);
		}else if(*status == 2){
			return -2;
		}else
			return -3;
//		str2bytes(ck,8,buffer + 7 + 8);
		
		return 0;
	}
	sim_printf(MSG_DEBUG,"unknown resp = %s\n",buffer);
	return -1;

}
int SimAL_UmtsAuth(SimAL_Hd_T * pHd ,const unsigned char *_rand, const unsigned char *autn,unsigned char *res, size_t *res_len, unsigned char *ik, unsigned char *ck, unsigned char *auts)
{
	struct at_response resp;
	struct at_request req;
	char randstr[64];
	char autnstr[64];
	int ret = SimAL_ERR_NOTREADY;
	if(pHd->fd <= 0)
		return ret; 
	buildstr(_rand,16,randstr);
	buildstr(autn,16,autnstr);
	req.head.len = snprintf(req.buffer,sizeof(req.buffer),"AT+ERTCA=\"%s\",\"%s\"\r\n",randstr,autnstr);
	req.head.type = TYPE_RESP;
	req.head.timeout = 5;	
	printf("req = %s,len = %d\n",req.buffer,req.head.len);
	pHd->Lock(&pHd->lock);
	int n = SendAndRecv(pHd,&req,&resp,req.head.timeout); 
	if(n == 0){
		if(resp.head.ret == RESP_OK){
			int status;
			n = parse_utmsauth(resp.buffer,resp.head.len,&status,res,res_len,ik,ck,auts);
			if(n < 0)
				ret = SimAL_ERR_UNKNOWN_RESP;
			else
				ret = SimAL_SUCCESS; 
		}
	}else{
		ret = SimAL_ERR_FAILURE; 
	}
	pHd->Unlock(&pHd->lock);

	return ret;
}
#endif
#if 1
static int parse_utmsauth(char *buffer,int len,int *sync,unsigned char *res, size_t *res_len, unsigned char *ik, unsigned char *ck, unsigned char *auts)
{
	//+CSIM:12,000000FF9000
#if 1
	char tmp[8];
	unsigned char tmphex[256];
	buffer += 2;
	if(strncmp(buffer,"+CSIM:",6)==0){
		char *ptr = buffer + 6;
		char *ptr2 = strstr(ptr,",");
		if(ptr2 == NULL){
			sim_printf(MSG_DEBUG,"error resp = %s\n",buffer);
			return -1;
		}
		int len = ptr2-ptr;
		memcpy(tmp,ptr,len);
		tmp[len] = 0;
		len = atoi(tmp);
		len = len/2;
#if 0
		printf("len = %d\n",len);
#endif
		ptr = ptr2+1;
		str2bytes(tmphex,len,ptr);
		int i = 0;
		for(i =0;i<len;i++){
			if(i%16==0)
				printf("\n");
			printf("%x ",tmphex[i]);
		}
		if(len < 2){
			sim_printf(MSG_DEBUG,"unknown resp = %s\n",buffer);
			return -1;
		}
		if((tmphex[0] == 0x98)&&(tmphex[1] == 0x62)){
			*sync = -1;
			return -2;	
		}
		if((tmphex[0]==0xdc) && (tmphex[1]== 0x0e)){
			*res_len = 0;
			*sync = 0;
			memcpy(auts,tmphex+2,14);
			return 0;
		}
		
		if(tmphex[0] == 0xdb){
			int i = 0;
			int offset = 1;
			*sync = 1;
			*res_len = tmphex[offset];
			printf("res_len = %d\n",*res_len);
			offset += 1;
			memcpy(res,&tmphex[offset],*res_len);
			offset += *res_len;

			int ck_len = tmphex[offset];
			printf("res_len = %x\n",ck_len);//*res_len);
			offset += 1;
			memcpy(ck,&tmphex[offset],ck_len);
			offset += ck_len;
			int ik_len = tmphex[offset];
			printf("res_len = %x\n",ik_len);
			offset += 1;
			memcpy(ik,&tmphex[offset],ik_len);
		}
		return 0;
	}
#endif
	sim_printf(MSG_DEBUG,"unknown resp = %s\n",buffer);
	return -1;

}

int SimAL_UmtsAuth(SimAL_Hd_T * pHd ,const unsigned char *_rand, const unsigned char *autn,unsigned char *res, size_t *res_len, unsigned char *ik, unsigned char *ck, unsigned char *auts)
{
// AT+CSIM=78,"00880081221023553CBE9637A89D218AE64DAE47BF351055F328B43577B9B94A9FFAC354DFAFB3"

	struct at_response resp;
	struct at_request req;
	char randstr[64];
	char autnstr[64];
	int ret = SimAL_ERR_NOTREADY;
	if(pHd->fd <= 0)
		return ret; 
	buildstr(_rand,16,randstr);
	buildstr(autn,16,autnstr);
	req.head.len = snprintf(req.buffer,sizeof(req.buffer),"AT+CSIM=78,\"008800812210%s10%s\"\r\n",randstr,autnstr);
	//req.head.len = snprintf(req.buffer,sizeof(req.buffer),"AT+CSIM=78,\"008800812210%s10%s\"\r\n",autnstr,randstr);
	req.head.type = TYPE_RESP;
	req.head.timeout = 5;	
	printf("req = %s,len = %d\n",req.buffer,req.head.len);
	pHd->Lock(&pHd->lock);
	int n = SendAndRecv(pHd,&req,&resp,req.head.timeout); 
	if(n == 0){
		if(resp.head.ret == RESP_OK){
			int status;
			n = parse_utmsauth(resp.buffer,resp.head.len,&status,res,res_len,ik,ck,auts);
			if(n == -1)
				ret = SimAL_ERR_AUTH_FAILED;
			else if(n == -2)
				ret = SimAL_ERR_AUTH_BAD_MAC;
			else{
#if 1
				if(status == 0)
					ret = SimAL_ERR_AUTH_BAD_SEQ;
				else
					ret = SimAL_SUCCESS; 
#endif
			}
		}
	}else{
		ret = SimAL_ERR_FAILURE; 
	}
	pHd->Unlock(&pHd->lock);

	return ret;
}
#endif
static int parse_vutmsauth(char *buffer,int len,int *status,unsigned char *res, size_t *res_len, unsigned char *ik, unsigned char *ck, unsigned char *auts)
{
#if 1
	
	buffer += 2;
	if(strncmp(buffer,"*ERTCA",6)==0){
		sim_printf(MSG_DEBUG,"resp = %s\n",buffer);
		sim_printf(MSG_DEBUG,"status  = %s\n",buffer+7);
		*status = atoi(buffer+7);
		if(*status == 0){
			char *ptr = strstr(buffer+9,",");
			*res_len = (ptr-buffer-9)/2;
			sim_printf(MSG_DEBUG,"res_len = %d\n",*res_len);
			str2bytes(res,*res_len,buffer+9);
			ptr = buffer+9+(*res_len)*2+1;
			sim_printf(MSG_DEBUG,"ck = %s\n",ptr);
			str2bytes(ck,16,ptr);
#if 0
			int i = 0;
			for(i = 0;i<16;i++){
				printf("%02x ",ck[i]);
			}
			printf("\n");
#endif
			ptr += 33;
			sim_printf(MSG_DEBUG,"ik = %s\n",ptr);
			str2bytes(ik,16,ptr);
#if 0
			for(i = 0;i<16;i++){
				printf("%02x ",ik[i]);
			}
			printf("\n");
#endif
		}else if(*status == 1){
			str2bytes(auts,14,buffer + 8 + 2);
		}else if(*status == 2){
			return -2;
		}else
			return -3;
//		str2bytes(ck,8,buffer + 7 + 8);
		
		return 0;
	}
#endif
	sim_printf(MSG_DEBUG,"parse_vutmsauth unknown resp = %s\n",buffer);
	return -1;

}

int SimAL_VUmtsAuth(SimAL_Hd_T * pHd ,const unsigned char *_rand, const unsigned char *autn,unsigned char *res, size_t *res_len, unsigned char *ik, unsigned char *ck, unsigned char *auts)
{
	struct at_response resp;
	struct at_request req;
	char randstr[64];
	char autnstr[64];
	int ret = SimAL_ERR_NOTREADY;
	if(pHd->fd <= 0)
		return ret; 
	buildstr(_rand,16,randstr);
	buildstr(autn,16,autnstr);
	req.head.len = snprintf(req.buffer,sizeof(req.buffer),"AT*ERTCA=\"%s\",\"%s\"\r\n",randstr,autnstr);
	req.head.type = TYPE_RESP;
	req.head.timeout = 5;	
	sim_printf(MSG_DEBUG,"SimAL_VUmtsAuth req = %s,len = %d\n",req.buffer,req.head.len);
	pHd->Lock(&pHd->lock);
	int n = SendAndRecv(pHd,&req,&resp,req.head.timeout); 
	if(n == 0){
		if(resp.head.ret == RESP_OK){
			int status;
			n = parse_vutmsauth(resp.buffer,resp.head.len,&status,res,res_len,ik,ck,auts);
			if(n < 0)
				ret = SimAL_ERR_UNKNOWN_RESP;
			else
				ret = SimAL_SUCCESS; 
		}
	}else{
		ret = SimAL_ERR_FAILURE; 
	}
	pHd->Unlock(&pHd->lock);

	return ret;
}
#if 0
static int parse_atcmd(char *buffer,int len,char *resp,int resp_len)
{
	snprintf(resp,resp_len,"%s",buffer);
	return 0;
}
int SimAL_ATCmd(SimAL_Hd_T * pHd ,const unsigned char *cmd, unsigned char *resp_str,int resp_len)
{
	struct at_response resp;
	struct at_request req;
	int ret = SimAL_ERR_NOTREADY;
	if(pHd->fd <= 0)
		return ret; 
	req.head.len = snprintf(req.buffer,sizeof(req.buffer),"%s\r\n",cmd);
	req.head.type = TYPE_RESP;
	req.head.timeout = 5;	
	sim_printf(MSG_DEBUG,"SimAL_VUmtsAuth req = %s,len = %d\n",req.buffer,req.head.len);
	pHd->Lock(&pHd->lock);
	int n = SendAndRecv(pHd,&req,&resp,req.head.timeout); 
	if(n == 0){
		if(resp.head.ret == RESP_OK){
			int status;
			n = parse_atcmd(resp.buffer,resp.head.len,resp_str,resp_len);
			if(n < 0)
				ret = SimAL_ERR_UNKNOWN_RESP;
			else
				ret = SimAL_SUCCESS; 
		}
	}else{
		ret = SimAL_ERR_FAILURE; 
	}
	pHd->Unlock(&pHd->lock);

	return ret;
}
#endif
static int parse_crsm192(char *buffer,int len,int *count)
{
	char str[8];	
	buffer += 2;
	if(strncmp(buffer,"+CRSM: 144",10)==0){
		char *ptr = buffer + 19;
		memcpy(str,ptr,2);
		str[2] = 0;
		int n = strtoul(str,NULL,10);
		*count = n;
		return 0;
	}
	sim_printf(MSG_ERROR,"parse_crsm192 ERROR = %s\n",buffer);
	return -1;
}
static int parse_crsm176(char *buffer,int len,int *count)
{
	char str[8];	
	buffer += 2;
	if(strncmp(buffer,"+CRSM: 144",10)==0){
		char *ptr = buffer + 19;
		memcpy(str,ptr,2);
		str[2] = 0;
		int n = strtoul(str,NULL,10);
		*count = n;
		return 0;
	}
	sim_printf(MSG_ERROR,"parse_crsm176 ERROR = %s\n",buffer);
	return -1;
}
const static int abmm3DigitsMncCountries[] =
{
		200,
		216,		
		302,   /*Canada*/
		310,   /*US*/
		311,   /*US*/
		316,   /*US*/
		338,   /*Jamaica*/
		342,
		344,   /*Antigua and Barbuda */
		346,   /*Cayman Islands */
		348,   /*Virgin Islands, British */
		352,   /*Grenada*/
		356,   /*Saint Kitts and Nevis   */
		358,   /*St Lucia*/
		360,   /* Saint Vincent and the Grenadines */
		364,
		365,   /*Anguilla*/
		366,   /*Dominica*/
		376,   /*Turks and Caicos Islands*/
		405,
		467,   /*Korea*/
		722,
		732,
		890,
		001,   /*AT&T test 2G SIM */
		002,   /*AT&T test 3G SIM */
};
static int searchtab(const char *imsi)
{
	///return 1: find
	///return 0: not find
	int i = 0;
	char str[8];
	memcpy(str,imsi,3);
	str[3] = 0;
	int n = strtoul(str,0,10); 
	for(i = 0;i<sizeof(abmm3DigitsMncCountries);i++){
		if(n == abmm3DigitsMncCountries[i])
			return 1;
	}
	return 0;
}

static int getcount(SimAL_Hd_T * pHd,int *mnc)
{
	struct at_response resp;
	struct at_request req;
	int ret = SimAL_ERR_NOTREADY;
	if(pHd->fd <= 0)
		return ret; 
	req.head.len = snprintf(req.buffer,sizeof(req.buffer),"%s\r\n","AT+CRSM=176,28589,0,0,4");
	req.head.type = TYPE_RESP;
	req.head.timeout = 5;	
	sim_printf(MSG_DEBUG,"SimAL_mnc req = %s,len = %d\n",req.buffer,req.head.len);
	int n = SendAndRecv(pHd,&req,&resp,req.head.timeout);
	if(n == 0){
		if(resp.head.ret == RESP_OK){
			int count;
			n = parse_crsm176(resp.buffer,resp.head.len,&count);
			if(n < 0)
				ret = SimAL_ERR_UNKNOWN_RESP;
			else if( n == 0 ){
				*mnc = count;
				ret = SimAL_SUCCESS;
			}
		}
	}else{
		ret = SimAL_ERR_FAILURE; 
	}
	return ret;

}
static int parse_usimmnc(char *buffer,int buf_len,int *count)
{
	//+CSIM:60,621a8202412183026fad8a01058b066f0600050105 80 0200 04 88011891a5058b
	char tmp[8];
	buffer += 2;
	if(strncmp(buffer,"+CSIM:",6)==0){
		return 0;
	}
	buffer += 6;
	char *ptr = strstr(buffer,",");
	if(ptr == NULL)
		return -1;
	int len = strtoul(buffer,NULL,10);
	if(len < 50){
		sim_printf(MSG_ERROR,"len too small = %d\n",len);
		return -1;
	}
	return 0;
	buffer += 42;
	if((buffer[0] != '8') || (buffer[1] != '0'))
		return -1;
	buffer += 2;
	memcpy(tmp,buffer,2);
	tmp[2] = 0;
	len = strtoul(tmp,NULL,16);
	printf("len = %d\n",len);
	buffer += 2;
	memcpy(tmp,buffer,len);
	tmp[len] = 0;
	*count = strtoul(tmp,NULL,16);
	return 0;

}
static int parse_usimcount(char *buffer,int len,int *count)
{
	//+CSIM:12,000000FF9000
	buffer += 2;//skip "\r\n"
	len -= 2;
	if(strncmp(buffer,"+CSIM:12",8)==0){
		if((buffer[len - 3] == '0')&&(buffer[len - 4] == '0')&&(buffer[len - 5] == '0')&&(buffer[len - 6] == '9')){
			buffer += 15;
			printf("buffer = %s\n",buffer);
			if((buffer[0] == '0')&&(buffer[1] == '2')){
				*count = 2;
				return 0;
			}else if((buffer[0] == '0')&&(buffer[1] == '3')){
				*count = 3;
				return 0;
			}else{
				*count = 0;
				return 0;
			}

		}
		return -1;
	}
	return -1;
}
static int get_usimcount(SimAL_Hd_T * pHd ,int *mnc)
{
	//getcount
	struct at_response resp;
	struct at_request req;
	int ret = SimAL_ERR_NOTREADY;
	if(pHd->fd <= 0)
		return ret; 
	req.head.len = snprintf(req.buffer,sizeof(req.buffer),"%s\r\n","AT+CSIM=10,\"00b0000004\"");
	req.head.type = TYPE_RESP;
	req.head.timeout = 5;	
	sim_printf(MSG_DEBUG,"SimAL_mnc req = %s,len = %d\n",req.buffer,req.head.len);
	int n = SendAndRecv(pHd,&req,&resp,req.head.timeout);
	if(n == 0){
		if(resp.head.ret == RESP_OK){
			int count;
			n = parse_usimcount(resp.buffer,resp.head.len,&count);
			if(n < 0)
				ret = SimAL_ERR_UNKNOWN_RESP;
			else if( n == 0 ){
				*mnc = count;
				ret = SimAL_SUCCESS;
			}
		}
	}else{
		ret = SimAL_ERR_FAILURE; 
	}
	return ret;

}
#if 0
int SimAL_getUSimMNCCount(SimAL_Hd_T * pHd ,const char *imsi, int *mnc)
{
	struct at_response resp;
	struct at_request req;
	int ret = SimAL_ERR_NOTREADY;
	if(pHd->fd <= 0)
		return ret; 
	req.head.len = snprintf(req.buffer,sizeof(req.buffer),"%s\r\n","AT+CSIM=18,\"00A40804047FFF6FAD\"");
	req.head.type = TYPE_RESP;
	req.head.timeout = 5;	
	sim_printf(MSG_DEBUG,"SimAL_simmnc req = %s,len = %d\n",req.buffer,req.head.len);
	pHd->Lock(&pHd->lock);
	int n = SendAndRecv(pHd,&req,&resp,req.head.timeout);
	if(n == 0){
		if(resp.head.ret == RESP_OK){
			int count;
			n = parse_usimmnc(resp.buffer,resp.head.len,&count);
			if(n < 0){
				if(searchtab(imsi) == 0){
					*mnc = 2;
					ret = SimAL_SUCCESS;
				}else{
					*mnc = 3;
					ret = SimAL_SUCCESS;
				}
		
			}
			else if( n == 0 ){

				if(count == 4){
					n = get_usimcount(pHd,&count);
					if(n == 0){
						if((count ==2) || (count == 3)){
							*mnc = count;
							ret = SimAL_SUCCESS;
						}else{
							if(searchtab(imsi) == 0){
								*mnc = 2;
								ret = SimAL_SUCCESS;
							}else{
								*mnc = 3;
								ret = SimAL_SUCCESS;
							}
						}
					}else{
						if(searchtab(imsi) == 0){
								*mnc = 2;
								ret = SimAL_SUCCESS;
						}else{
								*mnc = 3;
								ret = SimAL_SUCCESS;
						}

					}
						
				}else{
					if(searchtab(imsi) == 0){
						*mnc = 2;
						ret = SimAL_SUCCESS;
					}else{
						*mnc = 3;
						ret = SimAL_SUCCESS;
					}
				}	
			}
		}
	}else{
		ret = SimAL_ERR_FAILURE; 
	}
	pHd->Unlock(&pHd->lock);

	return ret;

}
#endif
int SimAL_getUSimMNCCount(SimAL_Hd_T * pHd ,const char *imsi, int *mnc)
{
	struct at_response resp;
	struct at_request req;
	int ret = SimAL_ERR_NOTREADY;
	if(pHd->fd <= 0)
		return ret; 
	req.head.len = snprintf(req.buffer,sizeof(req.buffer),"%s\r\n","AT+CSIM=18,\"00A40804047FFF6FAD\"");
	req.head.type = TYPE_RESP;
	req.head.timeout = 5;	
	sim_printf(MSG_DEBUG,"SimAL_simmnc req = %s,len = %d\n",req.buffer,req.head.len);
	pHd->Lock(&pHd->lock);
	int n = SendAndRecv(pHd,&req,&resp,req.head.timeout);
	if(n == 0){
		if(resp.head.ret == RESP_OK){
			int count;
			n = parse_usimmnc(resp.buffer,resp.head.len,&count);
			if(n < 0){
				if(searchtab(imsi) == 0){
					*mnc = 2;
					ret = SimAL_SUCCESS;
				}else{
					*mnc = 3;
					ret = SimAL_SUCCESS;
				}
		
			}
			else if( n == 0 ){
				n = get_usimcount(pHd,&count);
				if(n == 0){
					if((count ==2) || (count == 3)){
						*mnc = count;
						ret = SimAL_SUCCESS;
					}else{
						if(searchtab(imsi) == 0){
							*mnc = 2;
							ret = SimAL_SUCCESS;
						}else{
							*mnc = 3;
							ret = SimAL_SUCCESS;
						}
					}
				}else{
					if(searchtab(imsi) == 0){
							*mnc = 2;
							ret = SimAL_SUCCESS;
					}else{
							*mnc = 3;
							ret = SimAL_SUCCESS;
					}
				}
			}
		}
	}else{
		ret = SimAL_ERR_FAILURE; 
	}
	pHd->Unlock(&pHd->lock);

	return ret;

}
int SimAL_getMNCCount(SimAL_Hd_T * pHd ,const char *imsi, int *mnc)
{
	int cardtype = 0;
	if(SimAL_GetType(pHd,&cardtype) != SimAL_SUCCESS){
		return SimAL_ERR_UNKNOWN_RESP;
	}
	if(cardtype == TYPE_SIM){
		return SimAL_getSimMNCCount(pHd,imsi,mnc);
	}else if(cardtype == TYPE_USIM){
		return SimAL_getUSimMNCCount(pHd,imsi,mnc);
	}
	return SimAL_ERR_UNKNOWN_RESP;
}
static int SimAL_getSimMNCCount(SimAL_Hd_T * pHd ,const char *imsi, int *mnc)
{
	struct at_response resp;
	struct at_request req;
	int ret = SimAL_ERR_NOTREADY;
	if(pHd->fd <= 0)
		return ret; 
	req.head.len = snprintf(req.buffer,sizeof(req.buffer),"%s\r\n","AT+CRSM=192,28589,0,0,15");
	req.head.type = TYPE_RESP;
	req.head.timeout = 5;	
	sim_printf(MSG_DEBUG,"SimAL_mnc req = %s,len = %d\n",req.buffer,req.head.len);
	pHd->Lock(&pHd->lock);
	int n = SendAndRecv(pHd,&req,&resp,req.head.timeout);
	if(n == 0){
		if(resp.head.ret == RESP_OK){
			int count;
			n = parse_crsm192(resp.buffer,resp.head.len,&count);
			if(n < 0){
				if(searchtab(imsi) == 0){
					*mnc = 2;
					ret = SimAL_SUCCESS;
				}else{
					*mnc = 3;
					ret = SimAL_SUCCESS;
				}
			}
			else if( n == 0 ){

				if(count == 4){
					n = getcount(pHd,&count);
					if(n == 0){
						if((count ==2) || (count == 3)){
							*mnc = count;
							ret = SimAL_SUCCESS;
						}else{
							if(searchtab(imsi) == 0){
								*mnc = 2;
								ret = SimAL_SUCCESS;
							}else{
								*mnc = 3;
								ret = SimAL_SUCCESS;
							}
						}
					}else{
						if(searchtab(imsi) == 0){
							*mnc = 2;
							ret = SimAL_SUCCESS;
						}else{
							*mnc = 3;
							ret = SimAL_SUCCESS;
						}
					}
						
				}else{
					if(searchtab(imsi) == 0){
						*mnc = 2;
						ret = SimAL_SUCCESS;
					}else{
						*mnc = 3;
						ret = SimAL_SUCCESS;
					}
				}	
			}
		}
	}else{
		ret = SimAL_ERR_FAILURE; 
	}
	pHd->Unlock(&pHd->lock);

	return ret;

}
static int CountSplit(char *buffer,char *split)
{
	int count = 0;
	char *ptr = buffer;
	char *tmp = strstr(ptr,split);
	while(tmp){
		count++;
		ptr = tmp+1;
		tmp = strstr(ptr,split);
	}
	return count;	
}
static int parse_netstatus(char *buffer,int len,int *status)
{
	if(strstr(buffer,"+CREG")){
		int count = CountSplit(buffer,",");
		if(count == 4){
			if(buffer[len-4] != ','){
				sim_printf(MSG_ERROR,"error resp buffer[len-4] = %x",buffer[len-4]);
				return -1;
			}
			if(buffer[len-3] == '2'){
				*status = TYPE_NETWORK_3G;
				return 0;
			}else{
				*status = TYPE_NETWORK_2G;
				return 0;
			}
		}else{
			*status = TYPE_NETWORK_3G;
			return 0;
		}
	}

	*status = TYPE_NETWORK_3G;
	return 0;
}
int SimAL_getRegStatus(SimAL_Hd_T * pHd ,int *type)
{
	int cardtype;
	int ret = SimAL_GetType(pHd,&cardtype);
	if(ret != SimAL_SUCCESS)
		return ret;
	if(cardtype == TYPE_SIM){
		*type = TYPE_NETWORK_2G;
		return SimAL_SUCCESS;
	}
	*type = TYPE_NETWORK_3G;
	return SimAL_SUCCESS;
#if 0
	struct at_response resp;
	struct at_request req;
	int ret = SimAL_ERR_NOTREADY;
	if(pHd->fd <= 0)
		return ret; 
	req.head.len = snprintf(req.buffer,sizeof(req.buffer),"%s\r\n","AT+CREG?");
	req.head.type = TYPE_RESP;
	req.head.timeout = 5;	
	sim_printf(MSG_DEBUG,"SimAL_getNetworkStatus req = %s,len = %d\n",req.buffer,req.head.len);
	pHd->Lock(&pHd->lock);
	int n = SendAndRecv(pHd,&req,&resp,req.head.timeout);
	if(n == 0){
		if(resp.head.ret == RESP_OK){
			n = parse_netstatus(resp.buffer,resp.head.len,type);
			if(n < 0){
				ret = SimAL_ERR_UNKNOWN_RESP;
			}
			else {
				ret = SimAL_SUCCESS;
			}
		}
	}else{
		ret = SimAL_ERR_FAILURE; 
	}
	pHd->Unlock(&pHd->lock);

	return ret;
#endif
}
int SimAL_getNetworkStatus(SimAL_Hd_T * pHd ,int *cardtype,int *status)
{
	// set status to 1 if in 2g network
	// set status to 2 if in 3g network
	int ret = SimAL_GetType(pHd,cardtype);
	if(ret != SimAL_SUCCESS)
		return ret;
	if(*cardtype == TYPE_SIM){
		*status = TYPE_NETWORK_2G;
		return SimAL_SUCCESS;
	}
	*status = TYPE_NETWORK_3G;
	return SimAL_SUCCESS;
#if 0 
	ret = SimAL_getRegStatus(pHd,status);
	return ret;
#endif
}

static int parse_csim(char *buffer,int len,unsigned char *sres,unsigned char *kc)
{
	buffer += 2;//skip \r\n
	if(strncmp(buffer,"+CSIM:32,",9)==0){
		buffer += 9;
		buffer += 2; // skip sres len
		sim_printf(MSG_DEBUG,"RES= %s\n",buffer);
		str2bytes(sres,4,buffer);
		buffer += 8; // skip sres
		buffer += 2; // skip kc len
		sim_printf(MSG_DEBUG,"kc= %s\n",buffer);
		str2bytes(kc,8,buffer);
		return 0;
	}
	return -1;
}
int SimAL_USimGsmAuth(SimAL_Hd_T * pHd ,const unsigned char *_rand, unsigned char *sres, unsigned char *kc)
{
	struct at_response resp;
	struct at_request req;
	char randstr[64];
	int ret = SimAL_ERR_NOTREADY;
	buildstr(_rand,16,randstr);
	if(pHd->fd <= 0)
		return ret; 
	req.head.len = snprintf(req.buffer,sizeof(req.buffer),"AT+CSIM=44,\"008800801110%s\"\r\n",randstr);
	req.head.type = TYPE_RESP;
	req.head.timeout = 5;	
	sim_printf(MSG_DEBUG,"SimAL_getNetworkStatus req = %s,len = %d\n",req.buffer,req.head.len);
	pHd->Lock(&pHd->lock);
	int n = SendAndRecv(pHd,&req,&resp,req.head.timeout);
	if(n == 0){
		if(resp.head.ret == RESP_OK){
			n = parse_csim(resp.buffer,resp.head.len,sres,kc);
			if(n < 0){
				ret = SimAL_ERR_UNKNOWN_RESP;
			}
			else {
				ret = SimAL_SUCCESS;
			}
		}
	}else{
		ret = SimAL_ERR_FAILURE; 
	}
	pHd->Unlock(&pHd->lock);

	return ret;

}
int SimAL_GsmAuth(SimAL_Hd_T * pHd,const unsigned char *_rand, unsigned char *sres, unsigned char *kc)
{
	int cardtype;	
	if(SimAL_GetType(pHd,&cardtype) != SimAL_SUCCESS){
		return SimAL_ERR_UNKNOWN_RESP;
	}
	if(cardtype == TYPE_SIM){
		return SimAL_SimGsmAuth(pHd,_rand,sres,kc);	
	}else if(cardtype == TYPE_USIM){
		return SimAL_USimGsmAuth(pHd,_rand,sres,kc);	
	}
	return SimAL_ERR_UNKNOWN_RESP; 
}
