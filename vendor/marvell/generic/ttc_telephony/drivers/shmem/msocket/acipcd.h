/*
    acipcd.h Created on: Aug 3, 2010, Jinhua Huang <jhhuang@marvell.com>

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

#ifndef ACIPCD_H_
#define ACIPCD_H_

#include <linux/types.h>
#include <linux/version.h>

#include <plat/pxa9xx_acipc.h>

/* some arbitrary definitions */
#define ACIPC_MUDP_KEY  	ACIPC_SHM_PACKET_NOTIFY
#define PACKET_SENT 		0x11
#define PEER_SYNC 		0x5


/*
 * the following acipc_notify_* inline functions are used to generate
 * interrupt from AP to CP.
 */
/* generate peer sync interrupt */
static inline void acipc_notify_peer_sync(void)
{
	ACIPCDataSend(ACIPC_MUDP_KEY, PEER_SYNC << 8);
}

/* notify cp that new packet available in the socket buffer */
static inline void acipc_notify_packet_sent(void)
{
	ACIPCDataSend(ACIPC_MUDP_KEY, PACKET_SENT << 8);
}

/* notify cp that flow control state has been changed */
static inline void acipc_notify_port_fc(void)
{
	ACIPCEventSet(ACIPC_PORT_FLOWCONTROL);
}

/* notify cp that cp can continue transmission */
static inline void acipc_notify_cp_tx_resume(void)
{
        printk(KERN_WARNING
               "MSOCK: acipc_notify_cp_tx_resume!!!\n");
	ACIPCEventSet(ACIPC_RINGBUF_TX_RESUME);
}

/*notify cp that ap transmission is stopped, please resume me later */
static inline void acipc_notify_ap_tx_stopped(void)
{
        printk(KERN_WARNING
               "MSOCK: acipc_notify_ap_tx_stopped!!!\n");
	ACIPCEventSet(ACIPC_RINGBUF_TX_STOP);
}

/* statistics variable for acipc flow control interrupts */
extern unsigned long acipc_ap_stopped_num;
extern unsigned long acipc_ap_resumed_num;
extern unsigned long acipc_cp_stopped_num;
extern unsigned long acipc_cp_resumed_num;

extern int acipc_init(void);
extern void acipc_exit(void);

#endif /* ACIPCD_H_ */
