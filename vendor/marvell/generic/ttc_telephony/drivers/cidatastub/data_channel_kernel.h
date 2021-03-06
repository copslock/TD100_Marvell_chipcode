#ifndef _DATA_CHANNEL_KERNEL_H_
#define _DATA_CHANNEL_KERNEL_H_

#include <linux/list.h>

typedef enum {
	TX_SUCCESS,
	CI_INTERLINK_FAIL = -1,
	DATA_CHAN_NOT_READY = -2,
	DATA_SIZE_EXCEED = -3,
	NO_RX_BUFS = -4,
	NO_CID = -5,
	PREVIOUS_SEND_ERROR = -6,
	DATA_SEND_ERROR = -7,
	MEM_ERROR = -8
} TXSTATUS;
#define txDataMask 0x0001
#define MAX_CI_DATA_STUB_RXMSG_LEN 2048
typedef int (*DataRxCallbackFunc) (char *packet, int len, unsigned char cid);

void InitDataChannel(void);
void DeInitDataChannel(void);
void DeInitCsdChannel(void);
int sendData(unsigned char cid, const char *buf, int len);
int sendCSData(unsigned char cid, const char *buf, int len);
int sendDataRemote(unsigned char cid, const char *buf, int len);
int sendCSDataRemote(unsigned char cid, const char *buf, int len);
int registerRxCallBack(SVCTYPE pdpTye, DataRxCallbackFunc callback);
int unregisterRxCallBack(SVCTYPE pdpTye);
DATAHANDLELIST *search_handlelist_by_cid(DATAHANDLELIST * pHeader,
					 unsigned char cid);

#endif
