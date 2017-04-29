/*
 *  Audio Server for Tavor EVB Platform
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
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifndef __AUDIO_CONTROL_H__
#define __AUDIO_CONTROL_H__

#include <pthread.h>

#define MAX_CLIENT_NUM  3

typedef enum {
	CLIENT0 = 0,
	CLIENT1,
	CLIENT2,
	CLIENT_INVALID,
} CLI_PRI;

typedef enum {
	DEV_DISABLE = 0,
	DEV_ENABLE,
} PORT_STATUS;

typedef enum {
	DEV_NOT_PRESENT = 0,
	DEV_DETECTED,
} DEV_DETECT;

typedef struct {
	PORT_STATUS mono_enable;
	PORT_STATUS bear_enable;
	PORT_STATUS stereo_enable;
	PORT_STATUS mic1_enable;
	PORT_STATUS mic2_enable;

	int playback_vol;
	int capture_vol;

} controlblock;

typedef struct {
	PORT_STATUS bt_speaker_enable;
	PORT_STATUS bt_mic_enable;

	int bt_playback_vol;
	int bt_capture_vol;
} btcontrolblock;

typedef enum {
	CODEC_MICCO,
	CODEC_BT,
} CODEC_TYPE;

/* Data to keep per audio device */
typedef struct {
	char cliname[20];

	pthread_t sendthread;
	pthread_attr_t send_attr;
	unsigned int send_status;   /* 0 for init, 1 for start, 2 for stop */

	pthread_t recvthread;
	pthread_attr_t recv_attr;
	unsigned int recv_status;   /* 0 for init, 1 for start, 2 for stop */

	pthread_mutex_t mutex;
	controlblock control;
	btcontrolblock btcontrol;
	CODEC_TYPE codec_type;

	int status;
	int laststatus;

	int sockfd;

} audioclient;

/* struct for every Audio Client */
typedef struct {
	audioclient* clients[MAX_CLIENT_NUM];
	int clinum;
	CLI_PRI curclient;
	CLI_PRI lastclient;
	controlblock control;
	int status;

	DEV_DETECT bt_detect;
	DEV_DETECT hp_detect;
	char* bt_addr;

	int sockfd;
	pthread_t thread;
	pthread_t thread_calibration;

	pthread_mutex_t mutex;

	int recovery_level;

} audioserver;

#define CHECKVAL ((unsigned int)0xFDFDFDFD)
#define safefree(buf) do { if (buf) { myfree(buf, __LINE__); (buf) = NULL; } } while (0)
#define checkbuffer(buf) checkbuffer__(buf, __LINE__)

void* mymalloc(int size);
void myfree(void* p, int line);
int checkbuffer__(void* p, int line);
audioclient* audiodevicenew(char * name);
void audiodevicefree(audioclient* client);


#endif

