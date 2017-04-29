#ifndef _ASM_ARCH_IMMCONTROL_H
#define _ASM_ARCH_IMMCONTROL_H

#define IMM_CLIENT_MAX_PRIORITY	1
#define IMM_CLIENT_MIN_PRIORITY	5
#define IMM_CLIENT_NUM_PRIORITIES	(IMM_CLIENT_MIN_PRIORITY - IMM_CLIENT_MAX_PRIORITY + 1)
#define IMM_DEFAULT_KERNEL_PRIORITY	IMM_CLIENT_MAX_PRIORITY
#define IMM_DEFAULT_USER_PRIORITY	IMM_CLIENT_MIN_PRIORITY
#define VALID_PRIORITY(x)		(((x) >= IMM_CLIENT_MAX_PRIORITY)&&((x) <= IMM_CLIENT_MIN_PRIORITY))
#define PRIORITY1_IS_GREATER(p1, p2)	((p2) > (p1))
#define PRIORITY_EQUAL(p1, p2)		((p2) == (p1))

#define IMM_GET_NUM_CLIENTS		0
#define IMM_GET_INFO_BY_INDEX		1
#define IMM_GET_INFO_BY_IMMID		2
#define IMM_GET_INFO_BY_NAME		3
#define IMM_SET_PRIORITY_BY_IMMID	4
#define IMM_SET_PRIORITY_BY_NAME	5

struct imm_client_info {
	char name[IMM_CLIENT_NAME_SIZE];     /* in/out: name of the client */  
	u32 immid;      /* in/out: immid of the client */
	u32 priority;   /* in/out: priority of the client */
	u32 clientnum;  /* in: client index, out: number of clients */
};

extern int imm_client_control(struct imm_client_info *, u32);

#endif
