/*
 *  Copyright (C) 2002-2008  Mattia Dongili <malattia@linux.it>
 *                           George Staikos <staikos@0wned.org>
 *
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ui/EventHub.h>

extern "C"
{
#include "cpufreqd_plugin.h"
}


#define SIGCANCEL SIGUSR1
#define INPUTEVENT_EXPIRE_TIME 2
#define INPUT_EVENT_CLASS (INPUT_DEVICE_CLASS_KEYBOARD | INPUT_DEVICE_CLASS_ALPHAKEY | INPUT_DEVICE_CLASS_DPAD | INPUT_DEVICE_CLASS_TRACKBALL | INPUT_DEVICE_CLASS_TOUCHSCREEN_MT | INPUT_DEVICE_CLASS_TOUCHSCREEN)


using namespace android;

static pthread_t event_thread;
static struct sigaction prev_action;


struct __input_event{
	int value;
	time_t time;
};
struct __input_event inputEvent;


static void sigcancel_handle(int signum)
{
	if (signum == SIGCANCEL)
		pthread_exit(NULL);
}

/*  Waits for android events on the file descriptor opened previously.
 *  This function uses poll(2) to wait for readable data.
 */
static void *event_wait (void __UNUSED__ *arg) {

	sp<EventHub> eventHub = new EventHub();
	RawEvent rawEvent;
	clog(LOG_DEBUG, "input event thread running.\n");

	while (1) {
		eventHub->getEvent(& rawEvent);


		uint32_t classes = eventHub->getDeviceClasses(rawEvent.deviceId);
		if(classes & INPUT_EVENT_CLASS){
			clog(LOG_DEBUG, "Input event: device=0x%x type=0x%x scancode=%d keycode=%d value=%d",
					rawEvent.deviceId, rawEvent.type, rawEvent.scanCode, rawEvent.keyCode, rawEvent.value);
			//all keypad and touchsceen event
			inputEvent.value = 1;
			inputEvent.time = time(NULL);
			clog(LOG_DEBUG, "inputEvent value %d, time %d", inputEvent.value, inputEvent.time);
		}
		/* Ring the bell!! */
		wake_cpufreqd();
	}
	return NULL;
}
/*  static int input_event_init(void)
 *
 *  create a thread to read input event
 */
static int input_event_init(void) {

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

static int input_event_exit(void) {
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

	sigaction(SIGCANCEL, &prev_action, NULL);
	clog(LOG_INFO, "android_event exited.\n");
	return 0;

}


/*
 *  parse the 'input_event' keywork
 */
int input_event_parse(const char *ev, void **obj) {
	int *ret = (int*)malloc(sizeof(int));
	if (ret == NULL) {
		clog(LOG_ERR, "couldn't make enough room for ac_status (%s)\n",
				strerror(errno));
		return -1;
	}

	*ret = 0;

	if (strncmp(ev, "1", 1) == 0) {
		*ret = 1;
	} else if (strncmp(ev, "0", 1) == 0) {
		*ret = 0;
	} else {
		clog(LOG_ERR, "couldn't parse %s\n", ev);
		free(ret);
		return -1;
	}

	clog(LOG_INFO, "parsed: %s\n", *ret==1 ? "1" : "0");

	*obj = ret;
	return 0;
}

/*
 *  evaluate the 'input_event' keywork
 */
int input_event_evaluate(const void *s) {
	const int *input = (const int *)s;
	if(inputEvent.time + INPUTEVENT_EXPIRE_TIME < time(NULL)){
		inputEvent.value = 0;
	}

	clog(LOG_DEBUG, "called: %s [%s]\n",
			*input==1 ? "1" : "0", inputEvent.value == 1 ? "1" : "0");

	return (*input == inputEvent.value) ? MATCH : DONT_MATCH;
}

static struct cpufreqd_keyword kw[] = {
	{ "input_event", &input_event_parse, &input_event_evaluate, NULL, NULL, NULL, NULL, NULL},
	{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
};

static struct cpufreqd_plugin input_event = {
	"input_plugin",	/* plugin_name */
	 kw,			/* config_keywords */
	 &input_event_init,	/* plugin_init */
	 &input_event_exit,	/* plugin_exit */
	 NULL
};

struct cpufreqd_plugin *create_plugin (void) {
	return &input_event;
}
