/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * (C) Copyright 2006 Marvell International Ltd.
 * All Rights Reserved
 */
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/socket.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/in.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>

#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include <linux/inet.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/ethtool.h>
#include <net/sock.h>
#include <net/checksum.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/percpu.h>
#include <linux/cdev.h>
#include <common_datastub.h>
#include <linux/platform_device.h>
#include "data_channel_kernel.h"
#include <linux/kthread.h>

static int ccichar_open(struct inode *inode, struct file *filp);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
static long ccichar_ioctl(struct file *filp,
			 unsigned int cmd, unsigned long arg);
#else
static int ccichar_ioctl(struct inode *inode, struct file *filp,
			 unsigned int cmd, unsigned long arg);
#endif

#define CINET_BUF_SIZE 2048 //1600//1518
#define MAX_CID_NUM    8

struct ccinet_priv {
	unsigned char cid;
	int status;                                             /* indicate status of the interrupt */
	spinlock_t lock;                                        /* spinlock use to protect critical session	*/
	struct  net_device_stats net_stats;                     /* status of the network device	*/
};
static struct file_operations ccinet_fops = {
	.open	= ccichar_open,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
	.unlocked_ioctl	= ccichar_ioctl,
#else
	.ioctl	= ccichar_ioctl,
#endif
	.owner	= THIS_MODULE
};
struct ccichar_dev {
	struct cdev cdev;                       /* Char device structure */
};

typedef struct _cinetdevlist
{
	struct net_device* ccinet;
	struct _cinetdevlist *next;
}CINETDEVLIST;

struct ccinetdev_dev {
	struct cdev cdev;                  /* Char device structure */
	struct semaphore sem[MAX_CID_NUM]; /* mutual exclusion semaphore */
};

struct ccirx_packet {
	char  *pktdata;
	int length;
	unsigned char cid;
};

typedef struct _cci_rxind_buf_list
{
	struct ccirx_packet rx_packet;
	struct _cci_rxind_buf_list *next;
}RXINDBUFNODE;

static const char* const ccinet_name = "ccinet";
static CINETDEVLIST *netdrvlist[MAX_CID_NUM] = {};
int ccinet_major = CCINET_MAJOR;
int ccinet_minor = 0;
struct ccinetdev_dev *ccinetdev_devices;
static struct class *ccinetdev_class;

//static RXINDBUFNODE *rxbuflist = NULL;
struct completion dataChanRxFlagRef;
#define rxDataMask 0x0010
struct task_struct *cinetDataRcvTaskRef;


#if  1
#define DPRINT(fmt, args ...)     printk(fmt, ## args)
#define DBGMSG(fmt, args ...)     printk(KERN_DEBUG "CIN: " fmt, ## args)
#define ENTER()                 printk(KERN_DEBUG "CIN: ENTER %s\n", __FUNCTION__)
#define LEAVE()                 printk(KERN_DEBUG "CIN: LEAVE %s\n", __FUNCTION__)
#define FUNC_EXIT()                     printk(KERN_DEBUG "CIN: EXIT %s\n", __FUNCTION__)
#define ASSERT(a)  if (!(a)) { \
		while (1) { \
			printk(KERN_ERR "ASSERT FAIL AT FILE %s FUNC %s LINE %d\n", __FILE__, __FUNCTION__, __LINE__); \
		} \
}

#else
#define DPRINT(fmt, args ...)     printk("CIN: " fmt, ## args)
#define DBGMSG(fmt, args ...)     do {} while (0)
#define ENTER()                 do {} while (0)
#define LEAVE()                 do {} while (0)
#define FUNC_EXIT()                     do {} while (0)
#define ASSERT(a) if (!(a)) { \
		while (1) { \
			printk(KERN_CRIT "ASSERT FAIL AT FILE %s FUNC %s LINE %d\n", __FILE__, __FUNCTION__, __LINE__); \
		} \
}
#endif

static void ccinet_setup(struct net_device*);

static inline void set_ccinet_haddr(unsigned char* haddr, unsigned char cid)
{
	unsigned char ha[ETH_ALEN] = {0x36, 0xCE, 0x1E, 0xB0, 0x04, 0};
	ha[ETH_ALEN -1] = cid;
	memcpy(haddr, ha, ETH_ALEN);
}

///////////////////////////////////////////////////////////////////////////////////////
// Interface to CCI Stub
///////////////////////////////////////////////////////////////////////////////////////
static int connect_network(void)
{
	return 0;
}

static int disconnect_network(void)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
// Network Operations
///////////////////////////////////////////////////////////////////////////////////////
static int ccinet_open(struct net_device* netdev)
{
	struct ccinet_priv* devobj = netdev_priv(netdev);
	ENTER();

	// need Assign the hardware MAC address of the device driver
	set_ccinet_haddr(netdev->dev_addr, devobj->cid);
	connect_network();
//	netif_carrier_on(netdev);
	netif_start_queue(netdev);
	LEAVE();
	return 0;
}

static int ccinet_stop(struct net_device* netdev)
{
	ENTER();
	netif_stop_queue(netdev);
//	netif_carrier_off(netdev);
	disconnect_network();
	LEAVE();
	return 0;
}

static int add_to_drv_list(unsigned char cid)
{
	CINETDEVLIST *pdrv;
	struct ccinet_priv *devobj;
	int err;
	char intfname[20];
	if (cid >= MAX_CID_NUM)
		return -1;

	ENTER();
	down(&ccinetdev_devices->sem[cid]);
	if (netdrvlist[cid] != NULL) {
		up(&ccinetdev_devices->sem[cid]);
		DPRINT("ccichar_ioctl: cid already in drv list.\n");
	}
	else {
		pdrv = kmalloc(sizeof(CINETDEVLIST), GFP_KERNEL);
		if (!pdrv) {
			up(&ccinetdev_devices->sem[cid]);
			printk(KERN_ERR"ccichar_iotcl: NOMEM. \n");
			return -ENOMEM;
		}
		sprintf(intfname, "ccinet%d", cid);
		DPRINT("ccichar_ioctl: intfname=%s\n", intfname);
		pdrv->ccinet = alloc_netdev(sizeof(struct ccinet_priv), intfname, ccinet_setup);
		ASSERT(pdrv->ccinet);
		DPRINT("ccichar_ioctl: alloc_netdev done.\n");
		err = register_netdev(pdrv->ccinet);
		ASSERT(err == 0);
		DPRINT("ccichar_ioctl: register_netdev done.\n");
		devobj = netdev_priv(pdrv->ccinet);
		devobj->cid = cid;
		pdrv->next = NULL;
		netdrvlist[cid] = pdrv;
		up(&ccinetdev_devices->sem[cid]);
	}
	LEAVE();
	return 0;
}
#ifdef DATA_IND_BUFFERLIST
static int  search_list_by_cid(unsigned char cid, CINETDEVLIST ** ppBuf)
{
	if ((cid >= MAX_CID_NUM) || (ppBuf == NULL))
		return -1;

	down(&ccinetdev_devices->sem[cid]);
	if (netdrvlist[cid] != NULL) {
		*ppBuf = netdrvlist[cid];
		up(&ccinetdev_devices->sem[cid]);
		return 0;
	}
	else {
		up(&ccinetdev_devices->sem[cid]);
		return -1;
	}
}
#endif
static void  remove_node_by_cid(unsigned char cid)
{
	if (cid >= MAX_CID_NUM)
		return;

	down(&ccinetdev_devices->sem[cid]);
	if (netdrvlist[cid] != NULL) {
		unregister_netdev(netdrvlist[cid]->ccinet);
		free_netdev(netdrvlist[cid]->ccinet);
		kfree(netdrvlist[cid]);
		netdrvlist[cid] = NULL;
	}
	up(&ccinetdev_devices->sem[cid]);

	return;
}


static void remove_drv_list(void)
{
	int index;

	for (index = 0; index < MAX_CID_NUM; index ++)
		remove_node_by_cid(index);

	return;
}
static int ccinet_tx(struct sk_buff* skb, struct net_device* netdev)
{
	struct ccinet_priv* devobj = netdev_priv(netdev);
	netdev->trans_start = jiffies;
	sendData(devobj->cid, skb->data + ETH_HLEN, skb->len - ETH_HLEN);
	 
	/* update network statistics */
	devobj->net_stats.tx_packets++;
	devobj->net_stats.tx_bytes += skb->len;
	
	dev_kfree_skb(skb);
	return 0;

}

static void ccinet_tx_timeout(struct net_device* netdev)
{
	struct ccinet_priv* devobj = netdev_priv(netdev);

	ENTER();
	devobj->net_stats.tx_errors++;
//	netif_wake_queue(netdev); // Resume tx
	return;
}

static struct net_device_stats *ccinet_get_stats(struct net_device *dev)
{
	struct ccinet_priv* devobj;

	devobj = netdev_priv(dev);
	ASSERT(devobj);
	return &devobj->net_stats;
}
int ccinet_rx(struct net_device* netdev, char* packet, int pktlen)
{

	struct sk_buff *skb;
	struct ccinet_priv *priv = netdev_priv(netdev);
	struct iphdr* ip_header = (struct iphdr*)packet;
	struct ethhdr ether_header;

	if (ip_header->version == 4) {
		ether_header.h_proto = htons(ETH_P_IP);
	} else if (ip_header->version == 6) {
		ether_header.h_proto = htons(ETH_P_IPV6);
	} else {
		printk(KERN_ERR "ccinet_rx: invalid ip version: %d\n", ip_header->version);
		priv->net_stats.rx_dropped++;
		goto out;
	}
	set_ccinet_haddr(ether_header.h_dest, priv->cid);
	memset(ether_header.h_source, 0, ETH_ALEN);

	//ENTER();
	//DBGMSG("ccinet_rx:pktlen=%d\n", pktlen);
	skb = dev_alloc_skb(pktlen + 2 + sizeof(ether_header));
	ASSERT(skb);

	if (!skb)
	{

		if (printk_ratelimit(  ))

			printk(KERN_NOTICE "ccinet_rx: low on mem - packet dropped\n");

		priv->net_stats.rx_dropped++;

		goto out;

	}

	memcpy(skb_put(skb, sizeof(ether_header)), &ether_header, sizeof(ether_header));

	memcpy(skb_put(skb, pktlen), packet, pktlen);

	/* Write metadata, and then pass to the receive level */

	skb->dev = netdev;
	skb->protocol = eth_type_trans(skb, netdev); //htons(ETH_P_IP);//eth_type_trans(skb, netdev);
	skb->ip_summed = CHECKSUM_UNNECESSARY; /* don't check it */
	priv->net_stats.rx_packets++;
	priv->net_stats.rx_bytes += pktlen + sizeof(ether_header);
	netif_rx(skb);
	//where to free skb?

	return 0;

 out:
	return -1;


}
#ifdef DATA_IND_BUFFERLIST
void addrxnode(RXINDBUFNODE *newBufNode)
{
	RXINDBUFNODE *pCurrNode;

	if (rxbuflist == NULL)
	{
		rxbuflist = newBufNode;
	}
	else
	{
		pCurrNode = rxbuflist;
		while (pCurrNode->next != NULL)
			pCurrNode = pCurrNode->next;

		pCurrNode->next = newBufNode;

	}
	return;
}

int sendDataIndtoInternalList(unsigned char cid, char* buf, int len)
{
	RXINDBUFNODE* rxnode=NULL;
	UINT32 flag;
	int ret=0;

	rxnode = kmalloc(sizeof(RXINDBUFNODE), GFP_KERNEL);
	if (!rxnode) {
		ret = -ENOMEM;
		goto err;
	}
	rxnode->rx_packet.pktdata = kmalloc(len, GFP_KERNEL);
	if (!(rxnode->rx_packet.pktdata)) {
		ret = -ENOMEM;
		goto err;
	}
	memcpy(rxnode->rx_packet.pktdata, buf, len);
	rxnode->rx_packet.length = len;
	rxnode->rx_packet.cid = cid;
	rxnode->next = NULL;
	addrxnode(rxnode);
#if 0
	OSAFlagPeek(dataChanRxFlagRef, &flag);

	if (!(flag & rxDataMask))
	{
		OSAFlagSet(dataChanRxFlagRef, rxDataMask, OSA_FLAG_OR);
	}
#endif
	complete(&dataChanRxFlagRef);
	return 0;

err:
	if (rxnode)
		free(rxnode);
	return ret;
}
void recvDataOptTask(void *data)
{
	CINETDEVLIST *pdrv;
	int err;
	RXINDBUFNODE *tmpnode;
	UINT32 flags;
	OS_STATUS osaStatus;

	//while(!kthread_should_stop())
	while (1)
	{

		if (rxbuflist != NULL)
		{
			err = search_list_by_cid(rxbuflist->rx_packet.cid, &pdrv);
			ASSERT(err == 0);

			ccinet_rx(pdrv->ccinet, rxbuflist->rx_packet.pktdata, rxbuflist->rx_packet.length);
			tmpnode = rxbuflist;
			rxbuflist = rxbuflist->next;
			kfree(tmpnode->rx_packet.pktdata);
			kfree(tmpnode);

		}
		else
		{
			wait_for_completion_interruptible(&dataChanRxFlagRef);
		}
	}

}
void initDataChanRecv()
{

	ENTER();

	init_completion(&dataChanRxFlagRef);

	cinetDataRcvTaskRef = kthread_run(recvDataOptTask, NULL, "recvDataOptTask");
	LEAVE();
}

#endif
int data_rx(char* packet, int len, unsigned char cid)
{
	CINETDEVLIST *pdrv;
#ifdef DATA_IND_BUFFERLIST
	int err;
#endif
	if (cid >= MAX_CID_NUM)
		return -1;
#ifndef DATA_IND_BUFFERLIST

	down(&ccinetdev_devices->sem[cid]);
	if (netdrvlist[cid] != NULL) {
		pdrv = netdrvlist[cid];
	}
	else {
		up(&ccinetdev_devices->sem[cid]);
		DPRINT("%s: cid %d is not ready!\n", __FUNCTION__, cid);
		return -1;
	}
	ccinet_rx(pdrv->ccinet, packet, len);
	up(&ccinetdev_devices->sem[cid]);
#else
	err = sendDataIndtoInternalList(cid, packet, len);
	if (err != 0)
		return -1;
#endif
	return len;
}


static int validate_addr(struct net_device* netdev)
{
	ENTER();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
// Initialization
///////////////////////////////////////////////////////////////////////////////////////

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 32)
static const struct net_device_ops cci_netdev_ops = {
	.ndo_open		= ccinet_open,
	.ndo_stop		= ccinet_stop,
	.ndo_start_xmit 	= ccinet_tx,
	.ndo_tx_timeout		= ccinet_tx_timeout,
	.ndo_get_stats 	= ccinet_get_stats,
	.ndo_validate_addr	= validate_addr
};
#endif
static void ccinet_setup(struct net_device* netdev)
{

	struct ccinet_priv* devpriv;

	ENTER();
	ether_setup(netdev);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 32)
	netdev->netdev_ops = &cci_netdev_ops;
#else
	netdev->open = ccinet_open;
	netdev->stop = ccinet_stop;
	netdev->hard_start_xmit = ccinet_tx;
	netdev->tx_timeout = ccinet_tx_timeout;
	netdev->get_stats = ccinet_get_stats;
	netdev->validate_addr = validate_addr;
#endif

	netdev->watchdog_timeo = 5;  //jiffies
	netdev->flags |= IFF_NOARP;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 21)
	netdev->hard_header_cache = NULL;      /* Disable caching */
#endif
	devpriv = netdev_priv(netdev);
	memset(devpriv, 0, sizeof(struct ccinet_priv));
	spin_lock_init(&devpriv->lock);
	LEAVE();
}
static int ccinetdev_setup_cdev(struct ccinetdev_dev *dev, int index)
{
	int i;
	int err = 0;
	int devno = MKDEV(ccinet_major, ccinet_minor + index);

	ENTER();
	cdev_init(&dev->cdev, &ccinet_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &ccinet_fops;
	for(i = 0; i < MAX_CID_NUM; i++)
		sema_init(&dev->sem[i], 1);
	err = cdev_add(&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding ccichar%d", err, index);

	LEAVE();
	return err;
}

void ccinetdev_cleanup_module(void)
{
	dev_t devno = MKDEV(ccinet_major, ccinet_minor);
#ifdef  DATA_IND_BUFFERLIST
	kthread_stop(cinetDataRcvTaskRef);
#endif
	/* Get rid of our char dev entries */
	if (ccinetdev_devices)
	{
		{
			cdev_del(&ccinetdev_devices[0].cdev);
			device_destroy(ccinetdev_class,
				       MKDEV(ccinet_major, ccinet_minor));
		}
		kfree(ccinetdev_devices);
	}

	class_destroy(ccinetdev_class);

	/* cleanup_module is never called if registering failed */
	unregister_chrdev_region(devno, 1);

}

static int __init ccinet_init(void)
{
	int result = 0;
	dev_t dev = 0;
	char name[256];

#if 0
	ret = platform_device_register(&ccinet_device);
	if (!ret)
	{
		ret = platform_driver_register(&ccinet_driver);
		if (ret)
			platform_device_unregister(&ccinet_device);
	}
	else
	{
		printk("Cannot register CCINET platform device\n");
	}
#else
	if (ccinet_major)
	{
		dev = MKDEV(ccinet_major, ccinet_minor);
		result = register_chrdev_region(dev, 1, "ccichar");
	}
	else
	{
		result = alloc_chrdev_region(&dev, ccinet_minor, 1, "ccichar");
		ccinet_major = MAJOR(dev);
	}

	if (result < 0)
	{
		printk(KERN_WARNING "ccichar: can't get major %d\n", ccinet_major);
		return result;
	}

	ccinetdev_devices = kmalloc(sizeof(struct ccinetdev_dev), GFP_KERNEL);
	if (!ccinetdev_devices)
	{
		result = -ENOMEM;
		goto fail;
	}
	memset(ccinetdev_devices, 0, sizeof(struct ccinetdev_dev));

	/* Initialize each device. */
	ccinetdev_class = class_create(THIS_MODULE, "ccichar");
	sprintf(name, "%s", "ccichar");
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 28)
	device_create(ccinetdev_class, NULL,
		      MKDEV(ccinet_major, ccinet_minor), NULL, name);
#else
	device_create(ccinetdev_class, NULL,
		      MKDEV(ccinet_major, ccinet_minor), name);
#endif

	result = ccinetdev_setup_cdev(ccinetdev_devices, 0);
	if (result)
		goto fail;

	registerRxCallBack(PDP_DIRECTIP, data_rx);

#ifdef  DATA_IND_BUFFERLIST
	initDataChanRecv();
#endif

#endif
	return 0;
 fail:
	ccinetdev_cleanup_module();

	return result;


};

static void __exit ccinet_exit(void)
{
#if 0
	if (ccinet)
	{
		unregister_netdev(ccinet);
		free_netdev(ccinet);
	}
#endif
	remove_drv_list();
#if 0
	platform_driver_unregister(&ccinet_driver);
	platform_device_unregister(&ccinet_device);
#endif
	ccinetdev_cleanup_module();
}

static int ccichar_open(struct inode *inode, struct file *filp)
{
	struct ccichar_dev *dev;

	dev = container_of(inode->i_cdev, struct ccichar_dev, cdev);

	filp->private_data = dev;

	return nonseekable_open(inode, filp);
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
static long ccichar_ioctl(struct file *filp,
			 unsigned int cmd, unsigned long arg)
#else
static int ccichar_ioctl(struct inode *inode, struct file *filp,
			 unsigned int cmd, unsigned long arg)
#endif
{
	unsigned char cid;
	int err = 0;

	if (_IOC_TYPE(cmd) != CCINET_IOC_MAGIC)
	{
		DPRINT("ccichar_ioctl: cci magic number is wrong!\n");
		return -ENOTTY;
	}
	switch (cmd)
	{
	case CCINET_IP_ENABLE:
		cid = (unsigned char)arg;
		if (cid >= MAX_CID_NUM)
			return -ENODEV;
		DPRINT("ccichar_ioctl: CCINET_IP_ENABLE,cid=%d\n", cid);
		err = add_to_drv_list(cid);
		break;
	case CCINET_IP_DISABLE:
		cid = (unsigned char)arg;
		if (cid >= MAX_CID_NUM)
			return -ENODEV;
		DPRINT("ccichar_ioctl: CCINET_IP_DISABLE,cid=%d\n", cid);
		remove_node_by_cid(cid);
		break;
	case CCINET_ALL_IP_DISABLE:
		remove_drv_list();
		break;
	}
	LEAVE();
	return err;
}

module_init(ccinet_init);
module_exit(ccinet_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Marvell");
MODULE_DESCRIPTION("Marvell CI Network Driver");

