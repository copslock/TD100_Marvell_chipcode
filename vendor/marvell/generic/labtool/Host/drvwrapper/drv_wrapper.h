/** @file drv_wrapper.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#ifndef _DRVWRAPPER_H
#define _DRVWRAPPER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

/* Command type */
/** Command type: Local */
#define TYPE_LOCAL 0x0001
/** Command type: WLAN */
#define TYPE_WLAN  0x0002
/** Command type: BT */
#define TYPE_HCI   0x0003

/* Command sub-type */
/** Command sub-type: Load Driver */
#define SUB_TYPE_LOAD_DRV         0x0001
/** Command sub-type: Unload Driver */
#define SUB_TYPE_UNLOAD_DRV       0x0002
/** Command sub-type: Get bridge config */
#define SUB_TYPE_GET_BRIDGE_CFG   0x0003
/** Command sub-type: Set bridge config */
#define SUB_TYPE_SET_BRIDGE_CFG   0x0004
/** Command sub-type: upgrade fw image*/
#define SUB_TYPE_UPDT_FW 0x0005

/** Labtool command header */
typedef struct _cmd_header {
    /** Command Type */
    short type;
    /** Command Sub-type */
    short sub_type;
    /** Command length (header+payload) */
    short length;
    /** Command status */
    short status;
    /** reserved */
    int reserved;
} cmd_header;

/** HostCmd_DS_Gen */
typedef struct _HostCmd_DS_Gen
{
   /** Command */
    short command;
   /** Size */
    short size;
   /** Sequence number */
    short seq_num;
   /** Result */
    short result;
} HostCmd_DS_Gen;

int drv_wrapper_get_info(char *cmdname);
int drv_wrapper_init(char *cmdname);
int drv_wrapper_deinit();
int drv_wrapper_ioctl(char *cmdname, unsigned char *p_buf, int *resp_len);
int drv_wrapper_bridge_command(char *cmdname, unsigned char *buf, int *length, char *script);
#ifdef NONPLUG_SUPPORT
int drv_wrapper_get_hci_info(char *cmdname);
int drv_wrapper_init_hci(char *cmdname);
int drv_wrapper_deinit_hci();
int drv_wrapper_send_hci_command(short ogf, short ocf,  unsigned char *in_buf, int in_buf_len,
		unsigned char *out_buf, int *out_buf_len);
#endif
#ifdef __cplusplus
}
#endif

#endif /* _DRVWRAPPER_H */
