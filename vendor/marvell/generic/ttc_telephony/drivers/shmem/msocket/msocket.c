/*
    msocket.c Created on: Aug 2, 2010, Jinhua Huang <jhhuang@marvell.com>

    Marvell PXA9XX ACIPC-MSOCKET driver for Linux
    Copyright (C) 2010 Marvell International Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as
    published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/completion.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>

#include "acipcd.h"
#include "shm.h"
#include "portqueue.h"
#include "msocket.h"
#include "shm_share.h"

/* forward msocket sync related static function prototype */
static void msocket_sync_worker(struct work_struct *work);
static void msocket_connect(void);
static void msocket_disconnect(void);

static DECLARE_WORK(sync_work, msocket_sync_worker);
static volatile bool msocket_is_sync_canceled;
static spinlock_t msocket_sync_lock;

bool msocket_is_synced = false;
bool msocket_recv_up_ioc = false;
DECLARE_COMPLETION(msocket_peer_sync);
extern int NVM_wake_lock_num;
extern int NVM_wake_unlock_num;
/* open a msocket in kernel */
int msocket(int port)
{
	struct portq *portq;

	portq = portq_open(port);

	if (IS_ERR(portq)) {
		printk(KERN_ERR "MSOCK: can't open queue port %d\n", port);
		return -1;
	}

	return port;
}

EXPORT_SYMBOL(msocket);

/* close a msocket */
int mclose(int sock)
{
	struct portq *portq;

	portq = (struct portq *)portq_array[sock];

	if (!portq) {
		printk(KERN_ERR "MSOCK: closed socket %d failed\n", sock);
		return -1;
	}

	portq_close(portq);

	return 0;
}

EXPORT_SYMBOL(mclose);

/* send packet to msocket */
int msend(int sock, const void *buf, int len, int flags)
{
	struct portq *portq;
	struct sk_buff *skb;
	struct shm_skhdr *hdr;
	bool block = flags == MSOCKET_KERNEL;

	portq = (struct portq *)portq_array[sock];
	if (!portq) {
		printk(KERN_ERR "MSOCK: %s: sock %d not opened!\n",
		       __func__, sock);
		return -1;
	}

	/* check the len first */
	if (len > (SHM_SKBUF_SIZE - sizeof(*hdr))) {
		printk(KERN_ERR "MSOCK: %s: port %d, len is %d!!\n",
		       __func__, portq->port, len);
		portq->stat_tx_drop ++;
		return -1;
	}

	/* alloc space */
	if (block)
		skb = alloc_skb(len + sizeof(*hdr), GFP_KERNEL);
	else
		skb = alloc_skb(len + sizeof(*hdr), GFP_ATOMIC);

	if (!skb) {
		printk(KERN_ERR "MSOCK: %s: out of memory.\n", __func__);
		return -ENOMEM;
	}
	skb_reserve(skb, sizeof(*hdr));	/* reserve header space */

	memcpy(skb_put(skb, len), buf, len);

	/* push header back */
	hdr = (struct shm_skhdr *)skb_push(skb, sizeof(*hdr));

	hdr->address = 0;
	hdr->port = portq->port;
	hdr->checksum = 0;
	hdr->length = len;

	if (!msocket_is_synced || portq_xmit(portq, skb, block) < 0) {
		kfree_skb(skb);
		printk(KERN_ERR "MSOCK: %s: port %d xmit error.\n",
		       __func__, portq->port);
		return -1;
	}

	return len;
}

EXPORT_SYMBOL(msend);

/* send sk_buf packet to msocket */
int msendskb(int sock, struct sk_buff * skb, int len, int flags)
{
	struct portq *portq;
	struct shm_skhdr *hdr;
	int length;
	bool block = flags == MSOCKET_KERNEL;
	if(NULL == skb){
		printk(KERN_ERR"MSOCK:%s:skb buff is NULL!\n",__func__);
		return -1;
	}
	portq = (struct portq *)portq_array[sock];
	if (!portq) {
		printk(KERN_ERR "MSOCK: %s: sock %d not opened!\n",
		       __func__, sock);
		kfree_skb(skb);
		return -1;
	}

	length = skb->len;
	if (length > (SHM_SKBUF_SIZE - sizeof(*hdr))) {
		printk(KERN_ERR "MSOCK: %s: port %d, len is %d larger than SHM_SKBUF_SIZE\n",
		       __func__, portq->port, len);
		kfree_skb(skb);
		portq->stat_tx_drop ++;
		return -1;
	}

	hdr = (struct shm_skhdr *)skb_push(skb, sizeof(*hdr));
	hdr->address = 0;
	hdr->port = portq->port;
	hdr->checksum = 0;
	hdr->length = len;

	if (!msocket_is_synced || portq_xmit(portq, skb, block) < 0) {
		kfree_skb(skb);
		printk(KERN_ERR "MSOCK: %s: port %d xmit error.\n",
		       __func__, portq->port);
		return -1;
	}
	return 0;
}
EXPORT_SYMBOL(msendskb);
/* receive packet from msocket */
int mrecv(int sock, void *buf, int len, int flags)
{
	struct portq *portq;
	struct sk_buff *skb;
	struct shm_skhdr *hdr;
	int packet_len;
	bool block = flags == MSOCKET_KERNEL;

	portq = (struct portq *)portq_array[sock];
	if (!portq) {
		printk(KERN_ERR "MSOCK: %s: sock %d not opened!\n",
		       __func__, sock);
		return -1;
	}

	skb = portq_recv(portq, block);
	if (IS_ERR(skb)) {
		printk(KERN_DEBUG "MSOCK: %s: portq_recv returns %d\n",
		       __func__, (int)skb);
		return -1;
	}

	if (!skb)
		return 0;

	hdr = (struct shm_skhdr *)skb->data;
	packet_len = hdr->length;
	if (packet_len > len) {
		printk(KERN_ERR "MSOCK: %s: error: no enough space.\n",
		       __func__);
		kfree_skb(skb);
		return -1;	//error
	}

	memcpy(buf, skb_pull(skb, sizeof(*hdr)), hdr->length);

	kfree_skb(skb);

	return packet_len;
}

EXPORT_SYMBOL(mrecv);

struct sk_buff * mrecvskb(int sock, int len, int flags)
{
	struct portq *portq;
	struct sk_buff *skb;
	struct shm_skhdr *hdr;
	bool block = flags == MSOCKET_KERNEL;

	portq = (struct portq *)portq_array[sock];
	if (!portq) {
		printk(KERN_ERR "MSOCK: %s: sock %d not opened!\n",
		       __func__, sock);
		return NULL;
	}

	skb = portq_recv(portq, block);
	if (IS_ERR(skb)) {
		printk(KERN_DEBUG "MSOCK: %s: portq_recv returns %d\n",
		       __func__, (int)skb);
		return NULL;
	}

	if (!skb)
		return NULL;

	hdr = (struct shm_skhdr *)skb->data;
	if (hdr->length > len) {
		printk(KERN_ERR "MSOCK: %s: error: no enough space.\n",
		       __func__);
		kfree_skb(skb);
		return NULL;	//error
	}
	skb_pull(skb, sizeof(*hdr));
	return skb;
}
EXPORT_SYMBOL(mrecvskb);
static void msocket_sync_worker(struct work_struct *work)
{
	/* acquire lock first */
	spin_lock(&msocket_sync_lock);

	while (!msocket_is_sync_canceled) {
		/* send peer sync notify */
		acipc_notify_peer_sync();

		/* unlock before wait completion */
		spin_unlock(&msocket_sync_lock);

		if (wait_for_completion_timeout(&msocket_peer_sync, HZ)) {
			/* we get CP sync response here */
			printk("msocket connection sync with CP O.K.!\n");
			/* acquire lock again */
			spin_lock(&msocket_sync_lock);

			if (!msocket_is_sync_canceled) {
				/* if no one cancel me */
				msocket_is_synced = true;
				/* Only when we have received linkup ioctl can we report the linkup message*/
				if (msocket_recv_up_ioc == true)
				{
					portq_broadcase_msg(MsocketLinkupProcId);
					msocket_recv_up_ioc = false;
				}
			}
			break;
		}
		/* acquire lock again */
		spin_lock(&msocket_sync_lock);
	}

	/* unlock before return */
	spin_unlock(&msocket_sync_lock);
}

/* start msocket sync */
static void msocket_connect(void)
{
	spin_lock(&msocket_sync_lock);
	msocket_is_sync_canceled = false;
	spin_unlock(&msocket_sync_lock);

	portq_schedule_sync(&sync_work);
}

/* stop msocket sync */
static void msocket_disconnect(void)
{
	spin_lock(&msocket_sync_lock);

	/* flag used to cancel any new packet activity */
	msocket_is_synced = false;

	/* flag used to cancel potential peer sync worker */
	msocket_is_sync_canceled = true;

	spin_unlock(&msocket_sync_lock);
	/* ensure that any scheduled work has run to completion */
	portq_flush_workqueue();

	/*
	 * and now no work is active or will be schedule, so we can
	 * cleanup any packet queued and initialize some key data
	 * structure to the beginning state
	 */
	shm_data_init();
	portq_flush_init();

}

/*
 * msocket device driver <-------------------------------------->
 */

#define PROC_FILE_NAME		"driver/msocket"

/*
 * This function is called at the beginning of a sequence.
 */
static void *msocket_seq_start(struct seq_file *s, loff_t * pos)
{
	int i = *pos;

	spin_lock_irq(&portq_list_lock);

	if (!i)
		return SEQ_START_TOKEN;

	while (i < PORTQ_NUM_MAX && !portq_array[i])
		i++;

	/* return a non null value to begin the sequence */
	return i >= PORTQ_NUM_MAX ? NULL : (*pos = i, portq_array[i]);
}

/*
 * This function is called after the beginning of a sequence.
 * It's called untill the return is NULL (this ends the sequence).
 */
static void *msocket_seq_next(struct seq_file *s, void *v, loff_t * pos)
{
	int i = *pos + 1;

	while (i < PORTQ_NUM_MAX && !portq_array[i])
		i++;

	/* return a non null value to step the sequence */
	return i >= PORTQ_NUM_MAX ? NULL : (*pos = i, portq_array[i]);
}

/*
 * This function is called at the end of a sequence
 */
static void msocket_seq_stop(struct seq_file *s, void *v)
{
	spin_unlock_irq(&portq_list_lock);
}

/*
 * This function is called for each "step" of a sequence
 */
static int msocket_seq_show(struct seq_file *s, void *v)
{
	struct portq *portq = (struct portq *)v;

	if (v == SEQ_START_TOKEN) {
		seq_printf(s, "shm_is_ap_xmit_stopped: %d\n",
			   shm_is_ap_xmit_stopped);
		seq_printf(s, "shm_is_cp_xmit_stopped: %d\n",
			   shm_is_cp_xmit_stopped);
		seq_printf(s, "acipc_ap_stopped_num:   %ld\n",
			   acipc_ap_stopped_num);
		seq_printf(s, "acipc_ap_resumed_num:   %ld\n",
			   acipc_ap_resumed_num);
		seq_printf(s, "acipc_cp_stopped_num:   %ld\n",
			   acipc_cp_stopped_num);
		seq_printf(s, "acipc_cp_resumed_num:   %ld\n",
			   acipc_cp_resumed_num);
		seq_printf(s, "tx_socket_total:        %d\n", shm_skbuf_num);
		seq_printf(s, "tx_socket_free:         %d\n",
			   shm_free_tx_skbuf());
		seq_printf(s, "rx_socket_total:        %d\n", shm_skbuf_num);
		seq_printf(s, "rx_socket_free:         %d\n",
			   shm_free_rx_skbuf());
		seq_printf(s,"NVM_wake_lock_num:       %d\n", NVM_wake_lock_num);
		seq_printf(s,"NVM_wake_unlock_num:     %d\n", NVM_wake_unlock_num);
		seq_puts(s, "\nport  ");
		seq_puts(s, "tx_current  tx_request  tx_sent  tx_drop"
			 "  tx_queue_max"
			 "  rx_current  rx_indicate   rx_got  rx_queue_max"
			 "  ap_throttle_cp  ap_unthrottle_cp"
			 "  cp_throttle_ap  cp_unthrottle_ap\n");
	} else {
		spin_lock(&portq->lock);
		seq_printf(s, "%4d", portq->port);
		seq_printf(s, "%12d", skb_queue_len(&portq->tx_q));
		seq_printf(s, "%12ld", portq->stat_tx_request);
		seq_printf(s, "%9ld", portq->stat_tx_sent);
		seq_printf(s, "%9ld", portq->stat_tx_drop);
		seq_printf(s, "%14ld", portq->stat_tx_queue_max);
		seq_printf(s, "%12d", skb_queue_len(&portq->rx_q));
		seq_printf(s, "%13ld", portq->stat_rx_indicate);
		seq_printf(s, "%9ld", portq->stat_rx_got);
		seq_printf(s, "%14ld", portq->stat_rx_queue_max);
		seq_printf(s, "%16ld", portq->stat_fc_ap_throttle_cp);
		seq_printf(s, "%18ld", portq->stat_fc_ap_unthrottle_cp);
		seq_printf(s, "%16ld", portq->stat_fc_cp_throttle_ap);
		seq_printf(s, "%18ld\n", portq->stat_fc_cp_unthrottle_ap);
		spin_unlock(&portq->lock);
	}

	return 0;
}

static void msocket_dump_port(void)
{
	struct portq *portq;
	int i;

	printk(KERN_ERR "shm_is_ap_xmit_stopped: %d\n",
			   shm_is_ap_xmit_stopped);
	printk(KERN_ERR "shm_is_cp_xmit_stopped: %d\n",
			   shm_is_cp_xmit_stopped);
	printk(KERN_ERR "acipc_ap_stopped_num:   %ld\n",
			   acipc_ap_stopped_num);
	printk(KERN_ERR "acipc_ap_resumed_num:   %ld\n",
			   acipc_ap_resumed_num);
	printk(KERN_ERR "acipc_cp_stopped_num:   %ld\n",
			   acipc_cp_stopped_num);
	printk(KERN_ERR "acipc_cp_resumed_num:   %ld\n",
			   acipc_cp_resumed_num);
	printk(KERN_ERR "tx_socket_total:        %d\n", shm_skbuf_num);
	printk(KERN_ERR "tx_socket_free:         %d\n",
			   shm_free_tx_skbuf());
	printk(KERN_ERR "rx_socket_total:        %d\n", shm_skbuf_num);
	printk(KERN_ERR "rx_socket_free:         %d\n",
			   shm_free_rx_skbuf());

	printk(KERN_ERR "NVM_wake_lock_num:      %d\n", NVM_wake_lock_num);
	printk(KERN_ERR "NVM_wake_unlock_num:    %d\n", NVM_wake_unlock_num);
	printk(KERN_ERR "\nport  tx_current  tx_request  tx_sent  tx_drop"
			 "  tx_queue_max"
			 "  rx_current  rx_indicate   rx_got  rx_queue_max"
			 "  ap_throttle_cp  ap_unthrottle_cp"
			 "  cp_throttle_ap  cp_unthrottle_ap\n");
	for (i = 1; i < 7; i ++) {
		portq = portq_array[i];
		if (!portq)
			continue;
		printk (KERN_ERR "%4d %12d %12ld %9ld %9ld %14ld %12d %13ld %9ld %14ld %16ld %18ld %16ld %18ld\n",
			portq->port, skb_queue_len(&portq->tx_q), portq->stat_tx_request, portq->stat_tx_sent,
			portq->stat_tx_drop, portq->stat_tx_queue_max, skb_queue_len(&portq->rx_q), portq->stat_rx_indicate,
			portq->stat_rx_got, portq->stat_rx_queue_max, portq->stat_fc_ap_throttle_cp,
			portq->stat_fc_ap_unthrottle_cp, portq->stat_fc_cp_throttle_ap, portq->stat_fc_cp_unthrottle_ap);
	}
}
/**
 * This structure gather "function" to manage the sequence
 *
 */
static struct seq_operations msocket_seq_ops = {
	.start = msocket_seq_start,
	.next = msocket_seq_next,
	.stop = msocket_seq_stop,
	.show = msocket_seq_show
};

/**
 * This function is called when the /proc file is open.
 *
 */
static int msocket_seq_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &msocket_seq_ops);
};

/**
 * This structure gather "function" that manage the /proc file
 *
 */
static struct file_operations msocket_proc_fops = {
	.owner = THIS_MODULE,
	.open = msocket_seq_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release
};

/* open msocket */
static int msocket_open(struct inode *inode, struct file *filp)
{
	/*
	 * explicit set private_data to NULL, we'll use this pointer to
	 * associate file and portq
	 */
	filp->private_data = NULL;

	return 0;
}

/* close msocket */
static int msocket_close(struct inode *inode, struct file *filp)
{
	struct portq *portq = filp->private_data;

	if (portq) {		/* file already bind to portq */
		printk(KERN_INFO "MSOCK: port %d is closed by process id:%d (\"%s\")\n", portq->port, current->tgid,current->comm);
		portq_close(portq);
	}

	return 0;
}

/* read from msocket */
static ssize_t
msocket_read(struct file *filp, char __user * buf, size_t len, loff_t * f_pos)
{
	struct portq *portq;
	struct sk_buff *skb;
	struct shm_skhdr *hdr;
	int rc = -EFAULT;

	portq = (struct portq *)filp->private_data;
	if (!portq) {
		printk(KERN_ERR "MSOCK: %s: port not bind.\n", __func__);
		return rc;
	}

	skb = portq_recv(portq, true);
	if (IS_ERR(skb)) {
		printk(KERN_DEBUG "MSOCK: %s: portq_recv returns %d\n",
		       __func__, (int)skb);
		return PTR_ERR(skb);
	}

	hdr = (struct shm_skhdr *)skb->data;
	if (hdr->length > len) {
		printk(KERN_ERR "MSOCK: %s: error: no enough space.\n",
		       __func__);
		goto err_exit;
	}

	if (copy_to_user(buf, skb_pull(skb, sizeof(*hdr)), hdr->length))
		printk(KERN_ERR "MSOCK: %s: copy_to_user failed.\n", __func__);
	else
		rc = hdr->length;

err_exit:
	kfree_skb(skb);
	return rc;
}

/* write to msocket */
static ssize_t
msocket_write(struct file *filp, const char __user * buf, size_t len,
	      loff_t * f_pos)
{
	struct portq *portq;
	struct sk_buff *skb;
	struct shm_skhdr *hdr;
	int rc = -EFAULT;

	portq = (struct portq *)filp->private_data;
	if (!portq) {
		printk(KERN_ERR "MSOCK: %s: port not bind.\n", __func__);
		return rc;
	}

	if (len > (SHM_SKBUF_SIZE - sizeof(*hdr))) {
		printk(KERN_ERR "MSOCK: %s: port %d, len is %d!!\n",
		       __func__, portq->port, len);
		return rc;
	}

	skb = alloc_skb(len + sizeof(*hdr), GFP_KERNEL);
	if (!skb) {
		printk(KERN_ERR "MSOCK: %s: out of memory.\n", __func__);
		return -ENOMEM;
	}
	skb_reserve(skb, sizeof(*hdr));	/* reserve header space */

	if (copy_from_user(skb_put(skb, len), buf, len)) {
		kfree_skb(skb);
		printk(KERN_ERR "MSOCK: %s: copy_from_user failed.\n",
		       __func__);
		return rc;
	}

	skb_push(skb, sizeof(*hdr));
	hdr = (struct shm_skhdr *)skb->data;
	hdr->address = 0;
	hdr->port = portq->port;
	hdr->checksum = 0;
	hdr->length = len;

	if (!msocket_is_synced || portq_xmit(portq, skb, true) < 0) {
		kfree_skb(skb);
		printk(KERN_ERR "MSOCK: %s: portq xmit error.\n", __func__);
		return -1;
	}

	return len;
}

/*  the ioctl() implementation */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
static long msocket_ioctl(struct file *filp,
			 unsigned int cmd, unsigned long arg)
#else
static int msocket_ioctl(struct inode *inode, struct file *filp,
			 unsigned int cmd, unsigned long arg)
#endif

{
	struct portq *portq;
	int port, status;

	/*
	 * extract the type and number bitfields, and don't decode
	 * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
	 */
	if (_IOC_TYPE(cmd) != MSOCKET_IOC_MAGIC)
		return -ENOTTY;
	if (_IOC_NR(cmd) > MSOCKET_IOC_MAXNR)
		return -ENOTTY;

	switch (cmd) {
	case MSOCKET_IOC_BIND:
		port = arg;
		if (IS_ERR(portq = portq_open(port))) {
			printk(KERN_INFO "MSOCK: binding port %d error, %d\n",
			       port, (int)portq);
			return (int)portq;
		} else {
			filp->private_data = portq;
			printk(KERN_INFO "MSOCK: binding port %d, success.\n",
			       port);
			printk(KERN_INFO "MSOCK: port %d is opened by process id:%d (\"%s\")\n", port, current->tgid,current->comm);
			return 0;
		}

	case MSOCKET_IOC_UP:
		printk(KERN_INFO "MSOCK: MSOCKET_UP is received!\n");
		spin_lock(&msocket_sync_lock);
		msocket_recv_up_ioc = true;
		spin_unlock(&msocket_sync_lock);
		/* ensure completion cleared before start */
		INIT_COMPLETION(msocket_peer_sync);
		msocket_connect();
		if(wait_for_completion_timeout(&msocket_peer_sync, 5*HZ) == 0)
		{
			printk(KERN_INFO "MSOCK: sync with CP FAIL\n");
			return -1;
		}
		return 0;

	case MSOCKET_IOC_DOWN:
		printk(KERN_INFO "MSOCK: MSOCKET_DOWN is received!\n");
		msocket_dump_port();
		msocket_disconnect();
		/* ok! the world's silent then notify the upper layer */
		portq_broadcase_msg(MsocketLinkdownProcId);
		return 0;

	case MSOCKET_IOC_PMIC_QUERY:
		printk(KERN_INFO "MSOCK: MSOCKET_PMIC_QUERY is received!\n");
		status = shm_is_cp_pmic_master()? 1 : 0;
		if (copy_to_user((void *)arg, &status, sizeof(int)))
			return -1;
		else
			return 0;

	default:
		return -ENOTTY;
	}
}

/* driver methods */
static struct file_operations msocket_fops = {
	.owner = THIS_MODULE,
	.open = msocket_open,
	.release = msocket_close,
	.read = msocket_read,
	.write = msocket_write,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
	.unlocked_ioctl = msocket_ioctl
#else
	.ioctl = msocket_ioctl
#endif
};

/* misc structure */
static struct miscdevice msocket_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "msocket",
	.fops = &msocket_fops
};

static int msocketDump_open(struct inode *inode, struct file *filp)
{
	filp->private_data = (void *)portq_get_dumpflag();
	return 0;
}
static int msocketDump_close(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t msocketDump_read(struct file *filp, char __user * buf, size_t len, loff_t * f_pos)
{
	char temp[256];
	int flag =  portq_get_dumpflag();

	sprintf(temp,"0x%08x",flag);
	if(copy_to_user(buf,(void *)&temp,strlen(temp)+1))
	{
		printk(KERN_ERR "MSOCKDUMP: %s: copy_to_user failed.\n", __func__);
		return -EFAULT;
	}
	printk("msocketDump:get flag :%s\n", temp);
	//return strlen(temp)+1;
	return 0;
}

static ssize_t msocketDump_write(struct file *filp, const char __user * buf, size_t len, loff_t * f_pos)
{
	char temp[256];
	int flag = 0;
	if(copy_from_user((void *)&temp, buf, len))
	{
		printk(KERN_ERR "MSOCKDUMP: %s: copy_from_user failed.\n", __func__);
		return -EFAULT;
	}
	flag = simple_strtol(temp,NULL,0);
	printk("msocketDump:set flag :%08x\n", flag);
	portq_set_dumpflag(flag);
	return len;
}

static struct file_operations msocketDump_fops = {
	.owner = THIS_MODULE,
	.open = msocketDump_open,
	.release = msocketDump_close,
	.read = msocketDump_read,
	.write = msocketDump_write
};
static struct miscdevice msocketDump_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "msocket_dump",
	.fops = &msocketDump_fops
};

/* module initialization */
static int __init msocket_init(void)
{
	struct proc_dir_entry *proc_entry;
	int rc;

	/* init lock */
	spin_lock_init(&msocket_sync_lock);

	/* create proc file */
	proc_entry = create_proc_entry(PROC_FILE_NAME, 0666, NULL);
	if (proc_entry) {
		proc_entry->proc_fops = &msocket_proc_fops;
	}

	/* port queue init */
	if ((rc = portq_init()) < 0) {
		return rc;
	}

	/* share memory area init */
	if ((rc = shm_init()) < 0) {
		goto shm_err;
	}

	/* acipc init */
	if ((rc = acipc_init()) < 0) {
		goto acipc_err;
	}

	/* register misc device */
	if ((rc = misc_register(&msocket_dev)) < 0) {
		goto misc_err;
	}
	if ((rc = misc_register(&msocketDump_dev)) < 0) {
		goto misc_err;
	}
	/* start msocket peer sync */
	msocket_connect();

	return 0;

misc_err:
	acipc_exit();
acipc_err:
	shm_exit();
shm_err:
	portq_exit();

	return rc;
}

/* module exit */
static void __exit msocket_exit(void)
{
	/* reverse order of initialization */
	misc_deregister(&msocketDump_dev);
	misc_deregister(&msocket_dev);
	acipc_exit();
	shm_exit();
	portq_exit();
	remove_proc_entry(PROC_FILE_NAME, NULL);
}

module_init(msocket_init);
module_exit(msocket_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Marvell");
MODULE_DESCRIPTION("Marvell MSocket Driver");
