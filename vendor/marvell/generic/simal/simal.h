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
#ifndef _SIMAL_H
#define _SIMAL_H

#include <sys/types.h>   
#include <sys/socket.h>
#include <pthread.h>
#include <sys/un.h>
#define SIMAL_SRV "/data/misc/simal/simal_s_server"
#define TYPE_IND	0
#define TYPE_RESP	1

struct req_head{
	int len;
	int type;
	int timeout;
};
struct at_request{
	struct req_head head;
	char buffer[512];
};
enum resp_val{
	RESP_OK,
	RESP_TIMEOUT,
	RESP_ERROR,
};
struct resp_head{
	int len;
	int type;
	int ret;
};
struct at_response{
	struct resp_head head;
	char buffer[512];
};
#define TYPE_NETWORK_2G 1	
#define TYPE_NETWORK_3G 2

#define TYPE_SIM	1
#define TYPE_USIM	0
#define TYPE_ERROR	-1

#define SimAL_SUCCESS 		0
#define SimAL_ERR_NOTREADY	-1
#define SimAL_ERR_FAILURE	-2
#define SimAL_ERR_UNKNOWN_RESP	-3
#define SimAL_ERR_READ_RESP		-4
#define SimAL_ERR_UNEXPECT_LEN	-5
#define SimAL_ERR_AUTH_BAD_SEQ	11
#define SimAL_ERR_AUTH_BAD_MAC	12
#define SimAL_ERR_AUTH_FAILED	10
typedef struct simal_handle{
	int fd;
	struct sockaddr_un dest_addr;
	pthread_mutex_t lock;
	void (*Lock)(pthread_mutex_t *lock);	
	void (*Unlock)(pthread_mutex_t *lock);
	void *cbhd;	//call back handle
}SimAL_Hd_T;

typedef int SimAL_EVENT_TYPE_T;
typedef void (*SimAL_Status_Cb_T)(SimAL_Hd_T *phd, SimAL_EVENT_TYPE_T type, unsigned int value);
int SimAL_Init (SimAL_Hd_T *pHd);
int SimAL_Shutdown (SimAL_Hd_T *pHd);
int SimAL_StatusCbRegister(SimAL_Hd_T *phd,SimAL_Status_Cb_T cb_hd);
int SimAL_StatusCbUnRegister(SimAL_Hd_T *phd,SimAL_Status_Cb_T cb_hd);
int SimAL_GetType (SimAL_Hd_T *pHd, int *type);
int SimAL_NeedSetPin(SimAL_Hd_T *pHd,int *ret);
int SimAL_SetPin(SimAL_Hd_T *pHd,const char *pin);
int SimAL_GetImsi (SimAL_Hd_T *pHd, char *imsi, size_t *len);
int SimAL_GsmAuth(SimAL_Hd_T * pHd ,const unsigned char *_rand, unsigned char *sres, unsigned char *kc);
int SimAL_UsimGsmAuth(SimAL_Hd_T * pHd ,const unsigned char *_rand, unsigned char *sres, unsigned char *kc);
/*
    if success , set res,res_len,ck,ik and return SimAL_SUCCESS
	if sync fail, set auts and return SimAL_ERR_AUTH_BAD_SEQ
	if bad mac , return SimAL_ERR_AUTH_BAD_MAC
	else return SimAL_ERR_AUTH_FAILED
*/
int SimAL_UmtsAuth(SimAL_Hd_T * pHd ,const unsigned char *_rand, const unsigned char *autn,unsigned char *res, size_t *res_len, unsigned char *ik, unsigned char *ck, unsigned char *auts);
int SimAL_VUmtsAuth(SimAL_Hd_T * pHd ,const unsigned char *_rand, const unsigned char *autn,unsigned char *res, size_t *res_len, unsigned char *ik, unsigned char *ck, unsigned char *auts);
//int SimAL_ATCmd(SimAL_Hd_T * pHd ,const unsigned char *atcmd, unsigned char *atresp,int resp_len);
int SimAL_getMNCCount(SimAL_Hd_T * pHd ,const char *imsi,int *count);
int SimAL_getRegStatus(SimAL_Hd_T * pHd ,int *status);
int SimAL_getNetworkStatus(SimAL_Hd_T * pHd ,int *cardtype,int *status);
#endif
