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
#ifndef _CITTY_DEV_H_
#define _CITTY_DEV_H_
#include <time.h>
#include "simal.h"
#include "list.h"
struct reqnode{
	struct at_request *req;
	struct sockaddr_un from;
	time_t expire_time;
	struct list_head list;
#if 0
	struct reqnode *next;
	struct reqnode *prev;
#endif
};
#define DEV_STATE_WRITEABLE		0
#define DEV_STATE_WAITRESP		1
struct citty_dev{
	int fd;
	time_t expire_time;
	int state;
	struct list_head req_head;
	struct list_head ind_head;
	int (*AddtoList)(struct citty_dev *dev,struct at_request *req,struct sockaddr_un from);
	void (*check_timeout)(struct citty_dev *dev,int sockfd);
	void (*handle_resp)(struct citty_dev *dev,int sockfd);
	void (*try_write_req)(struct citty_dev *dev);
};
extern int open_tty(struct citty_dev *dev);
#endif
