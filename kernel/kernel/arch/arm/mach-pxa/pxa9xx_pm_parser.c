/*
 *  PM Parser
 *
 *  Support for Power management related event parser over PXAxxx
 *
 *  Author:	Moran Raviv
 *  Created:	Dec 24, 2010
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
#include <mach/pxa9xx_pm_parser.h>
#include <mach/pxa9xx_pm_logger.h>

#define DRIVER_VERSION "v1.0"
#define DRIVER_AUTHOR "MARVELL"
#define DRIVER_DESC "Power management event Parser"

#define GET_DB_STRING(db, row, col) \
	(u8 *)(*(db + row*MAX_DATA_NUM + col))

/* module information */
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");

struct pm_logger_descriptor pm_logger;
static u8 ***database;


/* this is how the pm logger looks like:
sync(24)+event num(8) - time stamp - arguments ...
sync(24)+event num(8) - time stamp - arguments ... */


/******************************************************************************
* Function: get_pm_parser_args_num
*******************************************************************************
* Description: calculates the number of event arguments by counting the number
*			   entries between ts and next sync pattern
*
* Parameters: entry of timestamp
*
* Return value: event argument's num
*
* Notes:
******************************************************************************/
int get_pm_parser_args_num(int tsIndex)
{

	int args_num = 0;
	int entry = tsIndex;
	int size = pm_logger.buffSize;

	while (size != 0) {
		entry = (entry+1) & (pm_logger.buffSize-1); /* like modulo */

		/* this is sync pattern. stop */
		if ((pm_logger.buffer[entry] & PM_LOG_SYNC_MASK) ==
			PM_LOG_SYNC_PATTERN)
			break;

		args_num++; /* still arguments. increment count and continue  */
		size--;
	}

	return args_num;
}

/******************************************************************************
* Function: get_pm_parser_start_entry
*******************************************************************************
* Description: find start entry using current entry
*
* Parameters: none
*
* Return value: start entry
*
* Notes:
******************************************************************************/
static int get_pm_parser_start_entry(void)
{

	int entry = pm_logger.current_entry;
	int size = pm_logger.buffSize;

	/* search first sync pattern starting from current entry */
	while (size != 0) {
		/* this is sync pattern. stop */
		if ((pm_logger.buffer[entry] & PM_LOG_SYNC_MASK) ==
			PM_LOG_SYNC_PATTERN)
			return entry;
		else {
			/* not sync pattern yet. continue searching */
			entry = (entry+1) & (pm_logger.buffSize-1);
			size--;
		}
	}

	/* loop only once on buffer.
	this is for cases the buffer empty and no sync pattern is found */
	return -1;
}

/******************************************************************************
* Function: pm_parser_display_log
*******************************************************************************
* Description: display buffer content. use database to display event and
*			   registers' names
*
* Parameters: 1 for apps logger display, 2 for comm logger display
*
* Return value: none
*
* Notes:
******************************************************************************/
void pm_parser_display_log(int subsystem)
{

	int first_entry, entry, i, args_num;
	u32 ts = 0, event, first_ts, total_time;

	printk("\n");

	if (subsystem == 1) { /* APPS */
		if (get_pm_logger_app_status() == 0) {
			printk("PM Parser: apps pm logger is disabled");
			return;
		}
		/* stop logger tracing until praser prints all */
		pm_logger_app_start(0);

		pm_logger.buffSize = get_pm_logger_app_buffSize();
		pm_logger.current_entry = get_pm_logger_app_current_entry();
		pm_logger.buffer = get_pm_logger_app_buffer();
		database = get_pm_logger_app_db();
	} else {
		printk("PM Parser: not supported yet\n");
		return;
	}

	/* get start entry to print from it */
	first_entry = get_pm_parser_start_entry();
	if (first_entry == -1) {
		printk("PM Parser: Buffer is empty\n");
		if (subsystem == 1)
			pm_logger_app_start(1); /* restart logger tracing */
		return;
	}

	/* get first ts for the header at the end */
	first_ts = pm_logger.buffer[(first_entry+1) & (pm_logger.buffSize-1)];

	entry = first_entry;
	do {
		/* event num */
		event = pm_logger.buffer[entry] & PM_LOG_EVENT_MASK;
		entry = (entry+1) & (pm_logger.buffSize-1);

		/* ts */
		/* from this point there are no more events */
		if (pm_logger.buffer[entry] == 0)
			break;
		ts = pm_logger.buffer[entry];
		printk("%d", ts);

		/* args num */
		args_num = get_pm_parser_args_num(entry);
		entry = (entry+1) & (pm_logger.buffSize-1);

		/* print data strings using database */
		i = 0;
		if (GET_DB_STRING(database, event, i) != NULL) {
			/* event name */
			printk(",%s",
				GET_DB_STRING(database, event, i));
			i++;
		} else
			printk(",%x", event); /* event number */

		while (args_num) {
			/* print strings from database */
			if (GET_DB_STRING(database, event, i) != NULL) {
				printk(",%s:0x%x",
					GET_DB_STRING(database, event, i),
					pm_logger.buffer[entry]);
				i++;
			} else
				/* information is not found in database.
				print raw data instead */
				printk(",raw data:0x%x",
					pm_logger.buffer[entry]);

			entry = (entry+1) & (pm_logger.buffSize-1);
			args_num--;
		}
		printk("\n");
	} while (entry != first_entry);

	/* restart logger tracing */
	if (subsystem == 1) { /* APPS */
		pm_logger_app_start(1);
	}

	/* calculate total log time for header */
	total_time = (ts-first_ts)/32;

	/* print header */
	printk("\nTotal Log Time: %d milisecs",
		total_time);
	printk("\nTotal Buffer Size: %d bytes\n",
		pm_logger.buffSize * sizeof(unsigned int));
}
