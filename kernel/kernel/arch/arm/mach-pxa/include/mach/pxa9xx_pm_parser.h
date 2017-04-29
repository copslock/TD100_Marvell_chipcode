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

#define PM_LOG_SYNC_PATTERN 0x7E7E7E00
#define PM_LOG_EVENT_MASK 0xFF
#define PM_LOG_SYNC_MASK 0xFFFFFF00

/* functions */
int get_pm_parser_args_num(int tsIndex);
void pm_parser_display_log(int subsystem);
