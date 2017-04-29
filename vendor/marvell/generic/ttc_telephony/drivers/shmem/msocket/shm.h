/*
    shm.h Created on: Aug 2, 2010, Jinhua Huang <jhhuang@marvell.com>

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

#ifndef SHM_H_
#define SHM_H_

#include <linux/skbuff.h>

//#define SHM_BASE_ADDR 		0x07420000	/* share memory base address */
//#define SHM_AREA_SIZE 		0x00020000	/* total 128KB */
#define SHM_SKBUF_SIZE 		2048	/* maximum packet size */
//#define SHM_SKBUF_NUM		(SHM_AREA_SIZE  / SHM_SKBUF_SIZE / 2 -1)
//#define SHM_SKBUF_LOW_WM  	((SHM_SKBUF_NUM + 1) / 4)

extern int shm_base_addr;
extern int shm_area_size;
extern int shm_skbuf_num;
extern int shm_skbuf_low_wm;

/* share memory control block structure */
struct shm_skctl {
	/* up-link control block, AP write, CP read */
	volatile int ap_wptr;
	volatile int cp_rptr;
	volatile unsigned int ap_port_fc;

	/* down-link control block, CP write, AP read */
	volatile int ap_rptr;
	volatile int cp_wptr;
	volatile unsigned int cp_port_fc;

#define PMIC_MASTER_FLAG	0x4D415354
	/* PMIC SSP master status setting query */
	volatile unsigned int ap_pcm_master;
	volatile unsigned int cp_pcm_master;
};

/* share memory socket header structure */
struct shm_skhdr {
	unsigned int address;	/* not used */
	int port;				/* queue port */
	unsigned int checksum;	/* not used */
	int length;				/* payload length */
};

/* share memory socket buffer structure */
struct shm_skbuf {
	struct shm_skhdr skhdr;
	unsigned char payload[SHM_SKBUF_SIZE - sizeof(struct shm_skhdr)];
};

/* the whole share memory layout structure
struct shm_area {
	struct shm_skctl skctl;
	unsigned char __reserved1[SHM_SKBUF_SIZE - sizeof(struct shm_skctl)];
	struct shm_skbuf tx_skbuf[SHM_SKBUF_NUM];
	struct shm_skbuf rx_skbuf[SHM_SKBUF_NUM];
	unsigned char __reserved2[SHM_SKBUF_SIZE];
};*/

/* some global variable */
extern bool shm_is_ap_xmit_stopped;
extern bool shm_is_cp_xmit_stopped;
extern struct shm_skctl *shm_skctl;

/* get the next socket buffer slot */
static inline int shm_get_next_slot(int slot)
{
	return slot + 1 == shm_skbuf_num ? 0 : slot + 1;
}

/* check if up-link free socket buffer available */
static inline bool shm_is_xmit_full(void)
{
	return shm_get_next_slot(shm_skctl->ap_wptr) == shm_skctl->cp_rptr;
}

/* check if down-link socket buffer data received */
static inline bool shm_is_recv_empty(void)
{
	return shm_skctl->cp_wptr == shm_skctl->ap_rptr;
}

/* check if down-link socket buffer has enough free slot */
static inline bool shm_has_enough_free_rx_skbuf(void)
{
	int free = shm_skctl->ap_rptr - shm_skctl->cp_wptr;
	if (free <= 0)
		free += shm_skbuf_num;
	return (free > shm_skbuf_low_wm);
}

/* get free rx skbuf num */
static inline int shm_free_rx_skbuf(void)
{
	int free = shm_skctl->ap_rptr - shm_skctl->cp_wptr;
	if (free <= 0)
		free += shm_skbuf_num;
	return free;
}

/* get free tx skbuf num */
static inline int shm_free_tx_skbuf(void)
{
	int free = shm_skctl->cp_rptr - shm_skctl->ap_wptr;
	if (free <= 0)
		free += shm_skbuf_num;
	return free;
}

/* check if cp pmic is in master mode */
static inline bool shm_is_cp_pmic_master(void)
{
	return shm_skctl->cp_pcm_master == PMIC_MASTER_FLAG;
}

/*
 * functions exported by this module
 */
/* init & exit */
extern int shm_init(void);
extern void shm_exit(void);
extern void shm_data_init(void);

/* xmit and recv */
extern void shm_xmit(struct sk_buff *skb);
extern struct sk_buff *shm_recv(void);

#endif /* SHM_H_ */
