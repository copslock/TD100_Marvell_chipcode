/*
    acipcd.c Created on: Aug 3, 2010, Jinhua Huang <jhhuang@marvell.com>

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

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/wakelock.h>


#include "acipcd.h"
#include "shm.h"
#include "portqueue.h"
#include "msocket.h"

struct wake_lock acipc_wakeup; //used to ensure Workqueue scheduled.
/* forward static function prototype, these all are interrupt call-backs */
static u32 acipc_cb_rb_stop(u32 status);
static u32 acipc_cb_rb_resume(u32 status);
static u32 acipc_cb_port_fc(u32 status);
static u32 acipc_cb(u32 status);



/* statistics variable for acipc flow control interrupts */
unsigned long acipc_ap_stopped_num;	/* AP tx stopped num */
unsigned long acipc_ap_resumed_num;	/* AP resumed num by CP */
unsigned long acipc_cp_stopped_num;	/* CP tx stopped num */
unsigned long acipc_cp_resumed_num;	/* CP resumed num by AP */
/* acipc_init is used to register interrupt call-back function */
int acipc_init(void)
{
	acipc_ap_stopped_num = 0;
	acipc_ap_resumed_num = 0;
	acipc_cp_stopped_num = 0;
	acipc_cp_resumed_num = 0;
	wake_lock_init(&acipc_wakeup, WAKE_LOCK_SUSPEND, "acipc_wakeup");

	/* we do not check any return value */
	ACIPCEventBind(ACIPC_MUDP_KEY, acipc_cb, ACIPC_CB_NORMAL, NULL);
	ACIPCEventBind(ACIPC_RINGBUF_TX_STOP, acipc_cb_rb_stop,
		       ACIPC_CB_NORMAL, NULL);
	ACIPCEventBind(ACIPC_RINGBUF_TX_RESUME, acipc_cb_rb_resume,
		       ACIPC_CB_NORMAL, NULL);
	ACIPCEventBind(ACIPC_PORT_FLOWCONTROL, acipc_cb_port_fc,
		       ACIPC_CB_NORMAL, NULL);

	return 0;
}

/* acipc_exit used to unregister interrupt call-back function */
void acipc_exit(void)
{
	ACIPCEventUnBind(ACIPC_PORT_FLOWCONTROL);
	ACIPCEventUnBind(ACIPC_RINGBUF_TX_RESUME);
	ACIPCEventUnBind(ACIPC_RINGBUF_TX_STOP);
	ACIPCEventUnBind(ACIPC_MUDP_KEY);

	wake_lock_destroy(&acipc_wakeup);
}

/* cp xmit stopped notify interrupt */
static u32 acipc_cb_rb_stop(u32 status)
{
	wake_lock_timeout(&acipc_wakeup, HZ * 5);
	acipc_cp_stopped_num++;
	shm_is_cp_xmit_stopped = true;

	/*
	 * schedule portq rx_worker to try potential wakeup, since share memory
	 * maybe already empty when we acqiure this interrupt event
	 */
	printk(KERN_WARNING
	       "MSOCK: acipc_cb_rb_stop!!!\n");
	portq_schedule_rx();

	return 0;
}

/* cp wakeup ap xmit interrupt */
static u32 acipc_cb_rb_resume(u32 status)
{
	wake_lock_timeout(&acipc_wakeup, HZ * 2);
	acipc_ap_resumed_num++;
	shm_is_ap_xmit_stopped = false;

	/* schedule portq tx_worker to try potential transmission */
	printk(KERN_WARNING
	       "MSOCK: acipc_cb_rb_resume!!!\n");
	portq_schedule_tx();

	return 0;
}

/* cp notify ap port flow control */
static u32 acipc_cb_port_fc(u32 status)
{
	unsigned int temp_cp_port_fc = shm_skctl->cp_port_fc;
	unsigned int changed = portq_cp_port_fc ^ temp_cp_port_fc;
	int port = 0;
	struct portq *portq;

	wake_lock_timeout(&acipc_wakeup, HZ * 2);
	spin_lock(&portq_list_lock);

	while ((changed >>= 1)) {
		port++;
		if (changed & 1) {
			portq = portq_array[port];
			if (!portq)
				continue;

			spin_lock(&portq->lock);
			if (temp_cp_port_fc & (1 << port)) {
				portq->stat_fc_cp_throttle_ap++;
				printk(KERN_WARNING
				       "MSOCK: port %d is throttled by CP!!!\n",
				       port);
			} else {
				portq->stat_fc_cp_unthrottle_ap++;
				printk(KERN_WARNING
				       "MSOCK: port %d is unthrottled by CP!!!\n",
				       port);
			}
			spin_unlock(&portq->lock);
		}
	}

	spin_unlock(&portq_list_lock);

	portq_cp_port_fc = temp_cp_port_fc;

	/* schedule portq tx_worker to try potential transmission */
	portq_schedule_tx();

	return 0;
}

/* new packet arrival interrupt */
static u32 acipc_cb(u32 status)
{
	u32 data;
	u16 event;

	ACIPCDataRead(&data);
	event = (data & 0xFF00) >> 8;

	switch (event) {
	case PACKET_SENT:
		wake_lock_timeout(&acipc_wakeup, HZ * 5);
		spin_lock(&portq_rx_work_lock);
		if (!portq_is_rx_work_running) {
			portq_is_rx_work_running = true;
			portq_schedule_rx();
		}
		spin_unlock(&portq_rx_work_lock);
		break;

	case PEER_SYNC:
		complete_all(&msocket_peer_sync);
		break;

	default:
		break;
	}

	return 0;
}


