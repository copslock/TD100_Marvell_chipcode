/*
    shm.c Created on: Aug 2, 2010, Jinhua Huang <jhhuang@marvell.com>

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

#include "shm.h"
#include "acipcd.h"
#include "pxa910_cp.h"
/*
 * I'm sure these two global variable will never enter race condition,
 * so we needn't any exclusive lock mechanism to access them.
 */
bool shm_is_ap_xmit_stopped;
bool shm_is_cp_xmit_stopped;

int shm_base_addr;
int shm_area_size;
int shm_skbuf_num;
int shm_skbuf_low_wm;

/* share memory area base address */
static void *shm_area;

/* these pointers for convenience */
struct shm_skctl *shm_skctl;	/* control block base address */
static struct shm_skbuf *tx_skbuf;	/* tx socket buffer base address */
static struct shm_skbuf *rx_skbuf;	/* rx socket buffer base address */


extern struct cp_load_table_head * get_cp_load_table(void);

int shm_pram_init(void)
{
	struct cp_load_table_head * ptable = get_cp_load_table();
	if(!ptable)
		return -1;
#ifdef TD_SUPPORT
	shm_base_addr = 0x07420000;
	shm_area_size = 0x00020000;
#else
	shm_base_addr = ptable->ACIPCBegin;
	shm_area_size = ptable->ACIPCEnd - ptable->ACIPCBegin+4;
#endif
	shm_skbuf_num = shm_area_size / SHM_SKBUF_SIZE / 2 - 1 ;
	shm_skbuf_low_wm = (shm_skbuf_num+ 1) / 4;
	printk("CP image base addr:0x%08x, Share mem addr:0x%08x, Share mem end:0x%08x,shm size :%d,shm_buf_num:%d\n", ptable->imageBegin,shm_base_addr,ptable->ACIPCEnd,shm_area_size,shm_skbuf_num);
	return 0;
}
void shm_data_init(void)
{
	shm_is_ap_xmit_stopped = false;
	shm_is_cp_xmit_stopped = false;
	shm_skctl->ap_wptr = shm_skctl->cp_rptr = shm_skctl->ap_port_fc = 0;
	shm_skctl->ap_rptr = shm_skctl->cp_wptr = shm_skctl->cp_port_fc = 0;
	shm_skctl->ap_pcm_master = PMIC_MASTER_FLAG;
}

/* shm_init */
int shm_init(void)
{
	int ret;
	ret = shm_pram_init();
	if(ret < 0)
		return ret;
	/* map to non-cache virtual address */
	shm_area = ioremap_nocache(shm_base_addr, shm_area_size);

	if (!shm_area)
		return -1;

	/* initialize share memory area */
	shm_skctl = (struct shm_skctl *)shm_area;
	tx_skbuf = (struct shm_skbuf *)(shm_area+ SHM_SKBUF_SIZE);
	rx_skbuf = tx_skbuf + shm_skbuf_num;
	printk("shm_area:%p,shm_skctl:%p, tx_skbuf:%p,rx_skbuf:%p\n",shm_area,shm_skctl,tx_skbuf,rx_skbuf);
	shm_data_init();

	return 0;
}

void shm_exit(void)
{
	/* release memory */
	iounmap(shm_area);
}

/* write packet to share memory socket buffer */
void shm_xmit(struct sk_buff *skb)
{
	/*
	 * we always write to the next slot !?
	 * thinking the situation of the first slot in the first accessing
	 */
	int slot = shm_get_next_slot(shm_skctl->ap_wptr);

	if (!skb) {
		printk(KERN_ERR "shm_xmit skb is null..\n");
		return;
	}
	memcpy(tx_skbuf + slot, skb->data, skb->len);
	shm_skctl->ap_wptr = slot;	/* advance pointer index */
}

/* read packet from share memory socket buffer */
struct sk_buff *shm_recv(void)
{
	/* yes, we always read from the next slot either */
	int slot = shm_get_next_slot(shm_skctl->ap_rptr);

	/* get the total packet size first for memory allocate */
	struct shm_skhdr *hdr = (struct shm_skhdr *)(rx_skbuf + slot);
	int count = hdr->length + sizeof(*hdr);
	struct sk_buff *skb = NULL;

	if (hdr->length <= 0 || count > SHM_SKBUF_SIZE) {
		printk(KERN_EMERG "MSOCK: shm_recv: hdr->length = %d\n",
		       hdr->length);
		goto error_length;
	}

	skb = alloc_skb(count, GFP_ATOMIC);
	if (!skb)
		return NULL;

	/* write all the packet data including header to sk_buff */
	memcpy(skb_put(skb, count), hdr, count);

error_length:
	/* advance reader pointer */
	shm_skctl->ap_rptr = slot;

	return skb;
}
