/*
 *  Copyright (C) 2002-2008  Mattia Dongili <malattia@linux.it>
 *                           George Staikos <staikos@0wned.org>
 *  Copyright (C) 2011 Marvell, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sysfs/libsysfs.h>

#include "cpufreqd_plugin.h"

#define ZSP_STATE "/sys/devices/platform/mmp-zsp/zsp_run"

#define RUN  1
#define HALT 0

static struct sysfs_attribute *attr;
static int zsp_state;

/*  static int zsp_init(void)
 *
 *  test if the attribute of zsp state is present
 */
static int zsp_init(void) {
	attr = sysfs_open_attribute(ZSP_STATE);
	if (attr == NULL) {
		clog(LOG_INFO, "No ZSP state found\n");
		return -1;
	}
	return 0;
}

static int zsp_exit(void) {
	if (attr != NULL)
		sysfs_close_attribute(attr);
	clog(LOG_INFO, "exited.\n");
	return 0;
}

/*  static int zsp_update(void)
 *
 *  reads temperature valuse ant compute a medium value
 */
static int zsp_update(void) {
	zsp_state = HALT;
	if (attr != NULL) {
		sysfs_read_attribute(attr);
		clog(LOG_DEBUG, "read %s:%d\n", attr->name, attr->value);
		zsp_state = attr->value[0] == '1' ? RUN : HALT;
	}

	clog(LOG_INFO, "zsp is in %s\n",
			zsp_state==RUN ? "run" : "halt");
	return 0;
}

/*
 *  parse the keyword 'zsp'
 */
static int zsp_parse(const char *ev, void **obj) {
	int *ret = malloc(sizeof(int));
	if (ret == NULL) {
		clog(LOG_ERR, "couldn't make enough room for zsp_state (%s)\n",
				strerror(errno));
		return -1;
	}

	*ret = 0;

	clog(LOG_DEBUG, "called with: %s\n", ev);

	if (strncmp(ev, "on", 2) == 0) {
		*ret = RUN;
	} else if (strncmp(ev, "off", 3) == 0) {
		*ret = HALT;
	} else {
		clog(LOG_ERR, "couldn't parse %s\n", ev);
		free(ret);
		return -1;
	}

	clog(LOG_INFO, "parsed: %s\n", *ret==RUN ? "on" : "off");

	*obj = ret;
	return 0;
}

/*
 *  evaluate the keyword 'zsp'
 */
static int zsp_evaluate(const void *s) {
	const int *zsp = (const int *)s;

	clog(LOG_DEBUG, "called: %s [%s]\n",
			*zsp==RUN ? "on" : "off",
			zsp_state==RUN ? "on" : "off");

	return (*zsp == zsp_state) ? MATCH : DONT_MATCH;
}

static struct cpufreqd_keyword kw[] = {
	{ .word = "zsp", .parse = &zsp_parse, .evaluate = &zsp_evaluate },
	{ .word = NULL, .parse = NULL, .evaluate = NULL, .free = NULL }
};

static struct cpufreqd_plugin zsp = {
	.plugin_name	= "zsp_plugin",	/* plugin_name */
	.keywords	= kw,		/* config_keywords */
	.plugin_init	= &zsp_init,	/* plugin_init */
	.plugin_exit	= &zsp_exit,	/* plugin_exit */
	.plugin_update	= &zsp_update	/* plugin_update */
};

struct cpufreqd_plugin *create_plugin (void) {
	return &zsp;
}
