/*
 *  Copyright (C) 2005-2006  Mattia Dongili<malattia@linux.it>
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
 *
 *  Android Event Plugin
 *  -----------------
 *  This plugin is derived from cpufreqd_acpi_event.c,
 *  which allows cpufreqd to monitor android intents and process them.
 *
 */
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include "cpufreqd_plugin.h"

#define SIGCANCEL SIGUSR1

#define MAX_INTENT_LEN 128
#define MAX_STATE_LEN 64

struct android_event {
	struct android_state *state;
	int value;
	char name[MAX_INTENT_LEN];
};

struct android_state {
	int value;
	int init;
	int expired;
	time_t now;
	char name[MAX_STATE_LEN];
};

struct android_ev {
	int value;
	struct android_state *db;
};

char android_sock[MAX_PATH_LEN];
struct LIST states, events;

/*  Gather global config data
 */
static int android_conf(const char *key, const char *value) {
	struct LIST *list;
	struct NODE *n = NULL;

	if (!strncmp(key, "socket", sizeof("socket")) && value != NULL) {
		snprintf(android_sock, MAX_PATH_LEN, "%s", value);
		clog(LOG_DEBUG, "android socket is %s.\n", android_sock);
		return 0;
	}
	if (!strncmp(key, "state", sizeof("state")) && value != NULL) {
		/* try to parse 'state=name:init:expired' */
		struct android_state i;
		char *buf;
		if (!(buf = strtok(value, ":")))
			return 0;
		strncpy(i.name, buf, sizeof(i.name));
		if (!(buf = strtok(NULL, ":")))
			return 0;
		i.init = atoi(buf);
		if (!(buf = strtok(NULL, "")))
			i.expired = 0;
		else
			i.expired = atoi(buf);
		list = &states;
		LIST_FOREACH_NODE(node, list) {
			struct android_state *s = node->content;
			if (!strncmp(s->name, i.name, sizeof(i.name)))
				return 0;
		}
		i.value = i.init;
		if (i.expired)
			i.now = time(NULL);
		n = node_new(&i, sizeof(i));
	} else if (!strncmp(key, "event", sizeof("event")) && value != NULL) {
		struct android_event i;
		char *buf;
		/* try to parse 'event=intent:state:value' */
		if (!(buf = strtok(value, ":")))
			return 0;
		strncpy(i.name, buf, sizeof(i.name));
		if (!(buf = strtok(NULL, ":")))
			return 0;
		i.state = NULL;
		LIST_FOREACH_NODE(node, &states) {
			struct android_state *s = node->content;
			if (!strncmp(s->name, buf, MAX_STATE_LEN)) {
				i.state = s;
				break;
			}
		}
		if (!i.state)
			return 0;
		if (!(buf = strtok(NULL, "")))
			return 0;
		i.value = atoi(buf);

		list = &events;
		LIST_FOREACH_NODE(node, list) {
			struct android_event *e = node->content;
			if (!strncmp(e->name, i.name, sizeof(i.name)))
				return 0;
		}
		n = node_new(&i, sizeof(i));
	}

	if (n)
		list_append(list, n);
	return 0;
}

static pthread_t event_thread;
static FILE *event_file;
static struct sigaction prev_action;

static void sigcancel_handle(int signum)
{
	if (signum == SIGCANCEL)
		pthread_exit(NULL);
}

static void close_android_event(void) {
	if (event_file) {
		clog(LOG_DEBUG, "closing event handle.\n");
		if (fclose(event_file))
			clog(LOG_ERR, "Couldn't close the android"
					"event handler (%s).\n",
					strerror(errno));
	}
	event_file = NULL;
}

static int open_android_event (void) {
	if (!event_file && android_sock[0]) {
		int event_fd, alen;
		struct sockaddr_un sck;

		sck.sun_family = AF_UNIX;
		/* Use an abstract socket address */
		sck.sun_path[0] = '\0';
		alen = strlen(android_sock);
		if (alen > 107)
			alen = 107;
		strncpy(sck.sun_path + 1, android_sock, alen);
		sck.sun_path[107] = '\0';
		alen += 1 + offsetof(struct sockaddr_un, sun_path);

		if (((event_fd = socket(PF_UNIX, SOCK_STREAM, 0)) == -1)
				|| !(event_file = fdopen(event_fd, "r+"))) {
			clog(LOG_ERR, "Couldn't open android socket (%s).\n",
					strerror(errno));
			if (event_fd >= 0)
				close(event_fd);
			return -1;
		}

		if (connect(event_fd, (struct sockaddr *)&sck, alen) == -1) {
			clog(LOG_NOTICE, "Couldn't connect to android socket %s (%s).\n",
					android_sock, strerror(errno));
			close_android_event();
			return -1;
		}
	} else if (!android_sock[0]) {
		clog(LOG_ERR, "No android socket given.\n");
		return -1;
	} else {
		clog(LOG_ERR, "Unknown error (%p)-(%s).\n",
				event_file, android_sock);
		return -1;
	}

	if (fcntl(fileno(event_file), F_SETFL, O_NONBLOCK) == -1) {
		clog(LOG_ERR, "Couldn't set O_NONBLOCK on android "
				"event handler (%s).\n", strerror(errno));
		close_android_event();
		return -1;
	}
	return 0;
}

/*  Waits for android events on the file descriptor opened previously.
 *  This function uses poll(2) to wait for readable data.
 */
static void *event_wait (void __UNUSED__ *arg) {
	struct pollfd rfd;
	char buf[MAX_INTENT_LEN];
	int read_chars, ret;

	clog(LOG_DEBUG, "android event thread running.\n");

	rfd.events = POLLIN | POLLPRI;

	while (1) {
		while (!event_file && open_android_event() != 0) {
			struct timespec req = { .tv_sec = 3, .tv_nsec = 0 };
			nanosleep(&req, NULL);
		}

		rfd.fd = fileno(event_file);
		rfd.revents = 0;
		ret = poll(&rfd, 1, -1);

		if (ret < 0) {
			clog(LOG_ERR, "Error polling android event handler:"
					" %s\n", strerror(errno));
			if (errno == EINTR)
				close_android_event();
			continue;
		}

		/* barf and exit on any error condition */
		/* close the FD and try too reopen it every 3 seconds */
		if (rfd.revents & ~POLLIN) {
			clog(LOG_ERR, "Error polling android event handler"
					"(0x%.4x)", rfd.revents);
			close_android_event();
			continue;
		}

		read_chars = 0;
		while (fgets(buf, MAX_INTENT_LEN, event_file)) {
			int len = strchr(buf, '\n') - buf;
			if (len < 0)
				len = sizeof(buf);
			clog(LOG_DEBUG, "received buf %s\n", buf);
			LIST_FOREACH_NODE(node, &events) {
				struct android_event *e = node->content;
				if (!strncmp(e->name, buf, (size_t) len)) {
					e->state->value = e->value;
					break;
				}
			}
			++read_chars;
		}

		if (read_chars == 0) {
			if (errno == EAGAIN || errno == EINTR)
				continue;
			if (errno) {
				clog(LOG_DEBUG, "Error reading android event"
						" handler: %s\n",
						strerror(errno));
			} else
				clog(LOG_DEBUG,
					"android event handler disappeared.\n");
			close_android_event();
			continue;
		}
		/* Ring the bell!! */
		wake_cpufreqd();
	}
	return NULL;
}

/* Launch the thread that will wait for android events
 */
static int android_event_init (void) {
	struct sigaction act;
	int ret;

	act.sa_handler = sigcancel_handle;
	act.sa_flags = SA_ONESHOT;
	sigemptyset(&act.sa_mask);
	sigaction(SIGCANCEL, &act, &prev_action);

	/* launch event thread */
	if ((ret = pthread_create(&event_thread, NULL, &event_wait, NULL))) {
		clog(LOG_ERR, "Unable to launch thread: %s\n", strerror(ret));
		sigaction(SIGCANCEL, &prev_action, NULL);
		return -1;
	}
	sigaddset(&act.sa_mask, SIGCANCEL);
	return pthread_sigmask(SIG_BLOCK, &act.sa_mask, NULL);
}

static int android_event_exit (void) {
	if (event_thread) {
		clog(LOG_DEBUG, "killing event thread.\n");
		int ret = pthread_kill(event_thread, SIGCANCEL);
		if (ret != 0)
			clog(LOG_ERR, "Couldn't cancel event thread (%s).\n",
					strerror(ret));
		/* cleanup */
		ret = pthread_join(event_thread, NULL);
		if (ret != 0)
			clog(LOG_ERR, "Couldn't join event thread (%s).\n",
					strerror(ret));
		event_thread = 0;
	}

	/* just in case the event thread missed it */
	close_android_event();
	sigaction(SIGCANCEL, &prev_action, NULL);
	clog(LOG_INFO, "android_event exited.\n");
	return 0;
}

/*
 *  parse the 'android_state' keyword
 */
static int android_state_parse(const char *ev, void **obj) {
	char *buf;
	struct android_ev *ret = calloc(sizeof(*ret), 1);
	if (ret == NULL) {
		clog(LOG_ERR, "couldn't make enough room for "
				"android_event (%s)\n", strerror(errno));
		return -1;
	}
	clog(LOG_DEBUG, "called with: %s\n", ev);

	/* try to parse the 'state:value' */
	if ((buf = strtok((char *) ev, ":")) != NULL) {
		LIST_FOREACH_NODE(node, &states) {
			struct android_state *s = node->content;
			if (!strncmp(s->name, buf, MAX_STATE_LEN)) {
				ret->db = s;
				if (!(buf = strtok(NULL, "")))
					break;
				ret->value = atoi(buf);
				*obj = ret;
				return 0;
			}
		}
	}
	clog(LOG_ERR, "couldn't parse %s\n", ev);
	free(ret);
	return -1;
}

/*
 *  evaluate the 'android_state' keyword
 */
static int android_state_evaluate(const void *s) {
	const struct android_ev *e = (const struct android_ev *)s;
	struct android_state *data = e->db;
	if (data->expired && (data->now + data->expired < time(NULL))) {
		data->value = data->init;
		data->now = time(NULL);
	}

	clog(LOG_DEBUG, "called: %s:%d [%d]\n",
			data->name, e->value, data->value);
	return (e->value == data->value) ? MATCH : DONT_MATCH;
}

static struct cpufreqd_keyword kw[] = {
	{
		.word = "android_state",
		.parse = &android_state_parse,
		.evaluate = &android_state_evaluate
	},
	{ .word = NULL, .parse = NULL, .evaluate = NULL, .free = NULL }
};

static int android_init(void) {
	memset(&states, 0, sizeof(states));
	memset(&events, 0, sizeof(events));
	return 0;
}

static int android_exit(void) {
	android_event_exit();
	list_free_sublist(&states, states.first);
	list_free_sublist(&events, events.first);
	return 0;
}

static struct cpufreqd_plugin android = {
	.plugin_name		= "android",
	.keywords		= kw,
	.plugin_init		= &android_init,
	.plugin_exit		= &android_exit,
	.plugin_conf		= &android_conf,
	.plugin_post_conf	= &android_event_init,
};

struct cpufreqd_plugin *create_plugin (void) {
	return &android;
}
