#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/utsname.h>
#include <linux/module.h>

#include <common_datastub.h>

typedef void (*DataRxCallbackFunc)(char* packet, int len,unsigned char cid);
extern int registerRxCallBack(SVCTYPE pdpTye, DataRxCallbackFunc callback);
extern int unregisterRxCallBack(SVCTYPE pdpTye);
extern void sendDataRemote(unsigned char cid, char* buf, int len);
extern void sendCSDataRemote(unsigned char cid, char* buf, int len);

extern int gs_marvell_modem_send(const unsigned char *buf, int count);

typedef void (*marvell_modem_rx_callback)(unsigned char cid, char* packet, int len);
extern marvell_modem_rx_callback gs_marvell_modem_rx_psd_callback;
extern marvell_modem_rx_callback gs_marvell_modem_rx_csd_callback;

static int __init gs_module_init(void)
{
	registerRxCallBack(PDP_PPP + ATCI_REMOTE, (DataRxCallbackFunc)gs_marvell_modem_send);
	gs_marvell_modem_rx_psd_callback = sendDataRemote;
	gs_marvell_modem_rx_csd_callback = sendCSDataRemote;
	return 0;
}

static void __exit gs_module_exit(void)
{
	unregisterRxCallBack(PDP_PPP + ATCI_REMOTE);
	gs_marvell_modem_rx_psd_callback = (marvell_modem_rx_callback)NULL;
	gs_marvell_modem_rx_csd_callback = (marvell_modem_rx_callback)NULL;
}

/* Module */
MODULE_DESCRIPTION("Marvell USB Modem");
MODULE_AUTHOR("Al Borchers");
MODULE_LICENSE("GPL");


module_init(gs_module_init);
module_exit(gs_module_exit);

