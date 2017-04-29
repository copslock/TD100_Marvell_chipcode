/*
 *  PM Logger
 *
 *  Support for Power management related event logger over PXAxxx
 *
 *  Author:	Shay Pathov
 *  Created:	Dec 15, 2010
 *  Copyright:	(C) Copyright 2010 Marvell International Ltd.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  publishhed by the Free Software Foundation.
 */

#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/mman.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/moduleparam.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <asm/pgtable.h>
#include <mach/pmu.h>
#include <linux/semaphore.h>
#include <mach/pxa9xx_pm_logger.h>

#define DRIVER_VERSION "v1.0"
#define DRIVER_AUTHOR "MARVELL"
#define DRIVER_DESC "Power management event Logger"

/* module information */
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");

/* global logger descriptor */
struct pm_logger_descriptor pm_logger_app;

/* database for parser print */
u8 *pm_logger_app_db[][MAX_DATA_NUM] = {
	{"D1 ENTRY", "PWRMODE", "CKENA", "CKENB", "CKENC",
		"MIPI", "CPUPWR"},
	{"D2 ENTRY", "PWRMODE", "CKENA", "CKENB", "CKENC",
		"OSCC", "MIPI", "CPUPWR"},
	{"CGM ENTRY", "PWRMODE", "CKENA", "CKENB", "CKENC",
		"MIPI", "CPUPWR"},
	{"C1 ENTRY", "CPUPWR"},
	{"D1 EXIT", "AD1D0SR"},
	{"D2 EXIT", "AD2D0SR"},
	{"CGM EXIT", "ACGD0SR"},
	{"C1 EXIT"},
	{"OP REQ", "OP"},
	{"OP EN", "OP", "DEV"},
	{"OP DIS", "OP", "DEV"},
	{"OP EN NAME", "OP", "DEV"},
	{"OP DIS NAME", "OP", "DEV"},
	{"OP EN NO CHANGE", "OP", "DEV"},
	{"OP DIS NO CHANGE", "OP", "DEV"},
	{"OP SET", "ACCR", "ACSR"}
};

/******************************************************************************
* Function: pm_logger_app_add_trace_short
*******************************************************************************
* Description: add logger trace with 2 parameters
*
* Parameters: event number, time stamp and 2 arguments
*
* Return value: none
*
* Notes:
******************************************************************************/
void pm_logger_app_add_trace_short(int event,
			unsigned int timeStamp,
			unsigned int arg1,
			unsigned int arg2)
{
	register unsigned int entry = pm_logger_app.current_entry;

	if (pm_logger_app.enabled == 0) /* logger is not enabled. do noting */
		return;

	/* add 24 bits of sync pattern and 8 bit of event num */
	pm_logger_app.buffer[entry++] =
	(event & PM_LOG_EVENT_MASK) | PM_LOG_SYNC_PATTERN;
	entry = entry & (pm_logger_app.buffSize - 1); /* like modulo */

	/* add time stamp */
	pm_logger_app.buffer[entry++] = timeStamp;
	entry = entry & (pm_logger_app.buffSize - 1);

	/* add arguements */
	pm_logger_app.buffer[entry++] = arg1;
	entry = entry & (pm_logger_app.buffSize - 1);

	pm_logger_app.buffer[entry++] = arg2;
	entry = entry & (pm_logger_app.buffSize - 1);

	/* update current entry */
	pm_logger_app.current_entry = entry;
}

/******************************************************************************
* Function: pm_logger_app_add_trace
*******************************************************************************
* Description: add logger trace with unknown number of parameters
*
* Parameters: event number, time stamp and the arguments
*
* Return value: none
*
* Notes:
******************************************************************************/
void pm_logger_app_add_trace(unsigned int num_args,
				int event,
				unsigned int timeStamp,
				...)
{
	va_list ap;
	int i = 0;
	unsigned int entry = pm_logger_app.current_entry;

	if (pm_logger_app.enabled == 0) /* logger is not enabled. do noting */
		return;

	/* add 24 bits of sync pattern and 8 bit of event num */
	pm_logger_app.buffer[entry++] =
	(event & PM_LOG_EVENT_MASK) | PM_LOG_SYNC_PATTERN;
	entry = entry & (pm_logger_app.buffSize - 1);

	/* add time stamp */
	pm_logger_app.buffer[entry++] = timeStamp;
	entry = entry & (pm_logger_app.buffSize - 1);

	/* add arguements */
	va_start(ap, timeStamp);
	while (i < num_args) {
		pm_logger_app.buffer[entry++] = va_arg(ap, unsigned int);
		entry = entry & (pm_logger_app.buffSize - 1);
		i++;
	}
	va_end(ap);

	/* update current entry */
	pm_logger_app.current_entry = entry;
}

/******************************************************************************
* Function: pm_logger_app_clear
*******************************************************************************
* Description: clear buffer
*
* Parameters: none
*
* Return value: none
*
* Notes:
******************************************************************************/
void pm_logger_app_clear(void)
{
	/* clear buffer */
	if (pm_logger_app.buffer)
		memset(pm_logger_app.buffer, 0x00,
			pm_logger_app.buffSize * sizeof(unsigned int));

	/* update current entry */
	pm_logger_app.current_entry = 0;
}

/******************************************************************************
* Function: pm_logger_app_alloc_buffer
*******************************************************************************
* Description: allocate buffer
*
* Parameters: none
*
* Return value: 0 on success, -1 otherwise
*
* Notes:
******************************************************************************/
int pm_logger_app_alloc_buffer(void)
{
	pm_logger_app.buffer =
		kzalloc(pm_logger_app.buffSize * sizeof(unsigned int),
		GFP_KERNEL);
	if (pm_logger_app.buffer == NULL) {
		printk(KERN_ERR "PM Logger: failed to allocate buffer\n");
		return -1;
	}

	return 0;
}

/******************************************************************************
* Function: pm_logger_app_change_buffSize
*******************************************************************************
* Description: change buffer size
*
* Parameters: buffer size
*
* Return value: 0 on success, -1 if buffer size exceeded it's limits
*
* Notes:
******************************************************************************/
int pm_logger_app_change_buffSize(unsigned int new_buffSize)
{
	int ret;

	if (new_buffSize == pm_logger_app.buffSize)
		return 0;

	if ((new_buffSize < PM_LOGGER_BUFFER_SZ_MIN) ||
		(new_buffSize > PM_LOGGER_BUFFER_SZ_MAX)) {
		printk(KERN_ERR "PM Logger: buffer size is too small or too big\n");
		return -1;
	}

	/* verify the buffer size is an exponent of 2 */
	if (is_valid_size(new_buffSize) == -1) {
		printk(KERN_ERR "PM Logger: buffer size must be exponent of 2\n");
		return -1;
	}

	local_fiq_disable();
	/* free old buffer */
	kfree(pm_logger_app.buffer);

	/* update buffer size var */
	pm_logger_app.buffSize = new_buffSize;

	/* allocate new buffer with new size and update current entry 0 */
	ret = pm_logger_app_alloc_buffer();
	if (ret == -1)
		return -1;

	pm_logger_app.current_entry = 0;
	local_fiq_enable();

	return 0;
}

/******************************************************************************
* Function: pm_logger_app_start
*******************************************************************************
* Description: enable or disable logger tracing
*
* Parameters: 1 - start, 0 - stop
*
* Return value: none
*
* Notes:
******************************************************************************/
void pm_logger_app_start(int status)
{
	pm_logger_app.enabled = status;
}

/******************************************************************************
* Function: get_pm_logger_app_buffer
*******************************************************************************
* Description: get pm logger buffer
*
* Parameters: none
*
* Return value: pm logger buffer
*
* Notes:
******************************************************************************/
u32 *get_pm_logger_app_buffer(void)
{
	return pm_logger_app.buffer;
}

/******************************************************************************
* Function: get_pm_logger_app_buffSize
*******************************************************************************
* Description: get pm logger buffer size
*
* Parameters: none
*
* Return value: pm logger buffer size
*
* Notes:
******************************************************************************/
int get_pm_logger_app_buffSize(void)
{
	return pm_logger_app.buffSize;
}

/******************************************************************************
* Function: get_pm_logger_app_current_entry
*******************************************************************************
* Description: get pm logger current entry
*
* Parameters: none
*
* Return value: pm logger current enrty
*
* Notes:
******************************************************************************/
int get_pm_logger_app_current_entry(void)
{
	return pm_logger_app.current_entry;
}

/******************************************************************************
* Function: get_pm_logger_app_db
*******************************************************************************
* Description: get pm logger database
*
* Parameters: none
*
* Return value: pm logger database
*
* Notes:
******************************************************************************/
u8 ***get_pm_logger_app_db(void)
{
	return (u8 ***)pm_logger_app_db;
}

/******************************************************************************
* Function: get_pm_logger_app_status
*******************************************************************************
* Description: get pm logger status
*
* Parameters: none
*
* Return value: 1 if enabled, 0 otherwise
*
* Notes:
******************************************************************************/
int get_pm_logger_app_status(void)
{
	return pm_logger_app.enabled;
}

/******************************************************************************
* Function: is_valid_size
*******************************************************************************
* Description: check if size is an exponent of 2
*
* Parameters: buffer new size
*
* Return value: 0 if valid, -1 otherwise
*
* Notes:
******************************************************************************/
int is_valid_size(unsigned int size)
{
	int i, flag = 0;

	for (i = 0; i < 32; i++) {
		if (size & 0x1) {
			if (flag)
				return -1;
			else
				flag = 1;
		}
		size = size>>1;
	}

	return 0;
}

/******************************************************************************
* Function: pm_logger_init
*******************************************************************************
* Description: logger init
*
* Parameters: none
*
* Return value: 0 on success, -1 otherwise
*
* Notes:
******************************************************************************/
static int __init pm_logger_app_init(void)
{
	int ret;

	/* set buffer size to default */
	pm_logger_app.buffSize = PM_LOGGER_BUFFER_SZ_DEFAULT;

	/* allocate buffer */
	ret = pm_logger_app_alloc_buffer();
	if (ret == -1)
		return -1;

	/* start pm logger */
	pm_logger_app_start(1);

    return 0;
}

/******************************************************************************
* Function: pm_logger_exit
*******************************************************************************
* Description: logger exit
*
* Parameters: none
*
* Return value: none
*
* Notes:
******************************************************************************/
static void __exit pm_logger_app_exit(void)
{
	kfree(pm_logger_app.buffer);
	pm_logger_app.buffer = NULL;
}


subsys_initcall(pm_logger_app_init);
