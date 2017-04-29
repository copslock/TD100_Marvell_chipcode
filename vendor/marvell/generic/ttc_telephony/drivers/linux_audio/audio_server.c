/*
 *
 *  Audio manager for Tavor EVB Platform
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <linux/input.h>
#if !defined (BIONIC)
#include <linux/uinput.h>
#endif
#include <time.h>
#include <alsa/asoundlib.h>
#include <sys/ioctl.h>

#if defined (BIONIC)
#include <linux/capability.h>
#include <linux/prctl.h>
#include <private/android_filesystem_config.h>
#endif


#undef BT_SUPPORT

#ifdef BT_SUPPORT
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/sco.h>
#include <bluetooth/rfcomm.h>
#endif

/* used for openning the MSL device */
#include <osa.h>
#include "utilities.h"

/* used for operate eeh */
#include <eeh_ioctl.h>
#include "pxa_dbg.h"

/* used for audio_stub and acm */
#include <acm.h>
#include <audiolevanteapi.h>

/* used for alsa plugin */
#include "audio_protocol.h"
#include "audio_ipc.h"
#include "audio_file.h"
#include "audio_control.h"
#include "audio_ringbuf.h"
#include "audio_server.h"
#include "audio_micco.h"
#include "audio_timer.h"
#include "diag_al.h"
/* used for NVM IPC */
#include "NVMServer_defs.h"
#include "NVMIPCClient.h"
#include "diag.h"
#include "diag_buff.h"
//#include "diag_init.h"
/* debug */
#define dbg DPRINTF //DBGMSG
#define printf DPRINTF

extern int loglevel;
extern FILE *logfile;

char log_tag[256] = "MARVELL_TEL_AUDIO";

//BT addr address
unsigned char addr[256];

// count the number of client running
static sig_atomic_t iThreadsRunning = 0;

static sig_atomic_t run_level = RUN_MAX;
static int level = 1;

static int MUTE_STATE = 0;

/* Global variable */
pthread_attr_t m_attr;
#if !defined (BIONIC)
size_t m_size = PTHREAD_STACK_MIN;
#else
size_t m_size = 4096;
#endif
char g_srcfilename[128];
/* define communication socket */
int main_socket_enable = 0;
int tranfer_socket_enable = 0;

int b_launch_diag = 0;
int b_test_mode = 0;

/* Data Stream Test */
#define PCM_BUFF_SIZE           160
#define DELAY_FRAMES_IN_LOOPBACK            5
#define NUM_OF_LOOPBACK_FRAMES              (DELAY_FRAMES_IN_LOOPBACK + 100)
#define GETREPORTOPCODE     0x44

extern OSAMsgQRef msgQ4Calibration;

typedef struct {
	int procId;
	int msglen;
	void *pData;
} StubMsg;

typedef struct {
	int sock;
	int reportNumber;
} calibrationParam;

/*
	pipe is used to communicate between call-back functions and
	pcm control thread.
*/
int pipeWriteFd[2] = {-1, -1};
int pipeReadFd[2] = {-1, -1};

/* stream read from alsa plugin */
ringqueue ringrecv;
RINGINFO buffer;
static int start_flag = 0;
static int write_indicate = 0;
static int read_indicate = 0;

static void pcm_stream_write(unsigned int *buff)
{
	int i;
	unsigned char msg[1] = {'w'};
	int ret = 0;
	int notifyTrigger = 0;

	if ( buff != NULL)
	{
		if (start_flag == 1 ) {
			if (ring_buffer_out(&ringrecv, &buffer) != 0) {
				memcpy(buff, buffer.buf, PCM_BUFF_SIZE * 2);
				free(buffer.buf);
			} else {
				for (i = 0; i < 160; i++)
					((UINT16 *)buff)[i] = 0;

				msg[0] = 'U';
				dbg("underrun happens \n");
			}

			notifyTrigger = 1;
		}
		else
		{
			for (i = 0; i < 160; i++)
				((UINT16 *)buff)[i] = 0;
		}
	}

	if ((start_flag == 1) && (notifyTrigger == 1)) {
	/*
	  notify pcm write thread
	*/
	if(pipeWriteFd[1] > 0) {
		ret = write(pipeWriteFd[1], msg, 1);
		if(ret <= 0)
			dbg("WRITE notitication - can not send notification %d \n",ret);
	} else {
		dbg("WRITE notification is missing \n");
	}

	  notifyTrigger = 0;
	}
}

/***************************************************
*
***************************************************/

ringqueue ringsend;
RINGINFO buffer_send;
static void pcm_stream_read(unsigned int *buff)
{
	unsigned char msg[1] = {'r'};

	if (buff != NULL) {
		unsigned char * buffer = (unsigned char *)malloc(PCM_BUFF_SIZE*2);
		if(buffer == NULL)
		{
			ERRMSG("malloc error\n");
			return;
		}
		memcpy(buffer, buff, PCM_BUFF_SIZE*2);
		buffer_send.buf = (char *) buffer;
		buffer_send.len = PCM_BUFF_SIZE * 2;
		if(ring_buffer_in(&ringsend, buffer_send) <= 0) {
			dbg("ring buffer is full\n");
		}

		/*
		   notify pcm read thread
		 */
		if(pipeReadFd[1] > 0) {
			if(write(pipeReadFd[1], msg, 1) <= 0)
				dbg("WRITE notitication - can not send notification \n");
		} else {
			dbg("WRITE notification is missing \n");
		}
	} else {
		dbg("read buffer is null \n");
	}
}

/**************************************************************
 * audio bluetooth
 *************************************************************/
#ifdef BT_SUPPORT
int rf_handle, sco_handle;

int rfcomm_connect(bdaddr_t *src, bdaddr_t *dst, uint8_t channel)
{
	struct sockaddr_rc addr;
	int s;

	if ((s = socket(PF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)) < 0)
	{
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.rc_family = AF_BLUETOOTH;
	bacpy(&addr.rc_bdaddr, src);
	addr.rc_channel = 0;
	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		close(s);
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.rc_family = AF_BLUETOOTH;
	bacpy(&addr.rc_bdaddr, dst);
	addr.rc_channel = channel;
	if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0 )
	{
		close(s);
		return -1;
	}

	return s;
}

int sco_connect(bdaddr_t *src, bdaddr_t *dst, uint16_t *handle, uint16_t *mtu)
{
	struct sockaddr_sco addr;
	struct sco_conninfo conn;
	struct sco_options opts;
	socklen_t size;
	int s;

	if ((s = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_SCO)) < 0)
	{
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sco_family = AF_BLUETOOTH;
	bacpy(&addr.sco_bdaddr, src);

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		close(s);
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sco_family = AF_BLUETOOTH;
	bacpy(&addr.sco_bdaddr, dst);

	if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0 )
	{
		close(s);
		return -1;
	}

	memset(&conn, 0, sizeof(conn));
	size = sizeof(conn);

	if (getsockopt(s, SOL_SCO, SCO_CONNINFO, &conn, &size) < 0)
	{
		close(s);
		return -1;
	}

	memset(&opts, 0, sizeof(opts));
	size = sizeof(opts);

	if (getsockopt(s, SOL_SCO, SCO_OPTIONS, &opts, &size) < 0)
	{
		close(s);
		return -1;
	}

	if (handle)
		*handle = conn.hci_handle;

	if (mtu)
		*mtu = opts.mtu;

	return s;
}

int headset_connect(unsigned char *addr, int channel)
{
	bdaddr_t local;
	bdaddr_t bdaddr;
	int dd;
	uint16_t sco_handle, sco_mtu, vs;

	str2ba(addr, &bdaddr);

	hci_devba(0, &local);
	dd = hci_open_dev(0);
	hci_read_voice_setting(dd, &vs, 1000);
	vs = htobs(vs);
	DPRINTF("Voice setting: 0x%04x\n", vs);
	close(dd);
	if (vs != 0x0060)
	{
		ERRMSG("The voice setting must be 0x0060\n");
		return -1;
	}

	if ((rf_handle = rfcomm_connect(&local, &bdaddr, channel)) < 0)
	{
		ERRMSG("Can't connect RFCOMM channel");
		return -2;
	}

	DPRINTF("RFCOMM channel connected\n");

	if ((sco_handle = sco_connect(&local, &bdaddr, &sco_handle, &sco_mtu)) < 0)
	{
		printf("Can't connect SCO audio channel");
		close(rf_handle);
		return -3;
	}
	DPRINTF("SCO audio channel connected (handle %d, mtu %d)\n", sco_handle, sco_mtu);
	return 0;
}

int headset_disconnect()
{
	close(sco_handle);
	sleep(5);
	close(rf_handle);
	dbg("headset_disconnected\n");
}

#endif

/***************************************************************
 * Audio acm function wrapper
 **************************************************************/
int audio_set_client0(audioclient* client, int flag)
{
	if (flag == 1)
	{
		client->status = AP_CLIENT_ENABLE;
		/* Enable AP AUDIO */
		if (client->laststatus != AP_CLIENT_ENABLE)
		{
			if (client->control.mono_enable == 1)
			{
				/* mono_enable */
				dbg("mono_enable\n");
				ACMAudioDeviceEnable(ACM_AUX_SPEAKER, ACM_I2S,
						     client->control.playback_vol);
			}

			if (client->control.bear_enable == 1)
			{
				dbg("bear_enable\n");
				ACMAudioDeviceEnable(ACM_MAIN_SPEAKER, ACM_I2S,
						     client->control.playback_vol);
			}

			if (client->control.stereo_enable == 1)
			{
				dbg("stereo_enable\n");
				ACMAudioDeviceEnable(ACM_STEREO_SPEAKER, ACM_I2S,
						     client->control.playback_vol);
			}
			if (client->control.mic1_enable == 1)
			{
				dbg("Enable mic1 for hifi record\n");
				ACMAudioDeviceEnable(ACM_MIC, ACM_I2S,
						     client->control.capture_vol);
			}

			if (client->control.mic2_enable == 1)
			{
				dbg("Enable mic2 for hifi record\n");
				ACMAudioDeviceEnable(ACM_HEADSET_MIC, ACM_I2S,
						     client->control.capture_vol);
			}
		}
		client->laststatus = AP_CLIENT_ENABLE;

	}
	else
	{
		client->status = AP_CLIENT_DISABLE;
		/* Disable AP AUDIO */
		if (client->control.mono_enable == 1)
		{
			/* mono_disable */
			dbg("mono_disable\n");
			ACMAudioDeviceDisable(ACM_AUX_SPEAKER, ACM_I2S);
		}

		if (client->control.bear_enable == 1)
		{
			/* bear_disable */
			dbg("bear_disable\n");
			ACMAudioDeviceDisable(ACM_MAIN_SPEAKER, ACM_I2S);
		}

		if (client->control.stereo_enable == 1)
		{
			/* stereo_disable */
			dbg("stereo_disable\n");
			ACMAudioDeviceDisable(ACM_STEREO_SPEAKER, ACM_I2S);
		}
		if (client->control.mic1_enable == 1)
		{
			dbg("Disable mic1 \n");
			ACMAudioDeviceDisable(ACM_MIC, ACM_I2S);
		}

		if (client->control.mic2_enable == 1)
		{
			dbg("Disable mic2 \n");
			ACMAudioDeviceDisable(ACM_HEADSET_MIC, ACM_I2S);
		}
		client->laststatus = AP_CLIENT_DISABLE;
	}

	return 0;
}


static void ACMVoidCB(unsigned int *buff)
{
	UNUSEDPARAM(buff)
}

#if 0
static char *device = "hw:0,2,0";
#endif

snd_pcm_t *handle;

int audio_set_client2(audioclient* client, int flag)
{
	if (flag == 1)
	{
		/* Enable CP Audio */

		if (client->status == PHONE_VOICECALL_ENABLE && client->laststatus != PHONE_VOICECALL_ENABLE)
		{
			/* Enable voice call audio setting */
			dbg("Phone_voicecall_enable \n");
#if 0
			int err;

			if ((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
			{
				ERRMSG("open default device error\n");
			}
#endif
			ACMAudioStreamInStart( ACM_VOICE_CALL, (ACM_SrcDst)0, ACMVoidCB);
			ACMAudioStreamOutStart( ACM_VOICE_CALL, (ACM_SrcDst)0,
						(ACM_CombWithCall)0, ACMVoidCB);

			if (client->codec_type == CODEC_MICCO)
			{
				if (client->control.mono_enable == 1 || client->control.bear_enable == 1)
				{
					dbg("Enable main speaker during voicecall\n");
					ACMAudioDeviceEnable(ACM_MAIN_SPEAKER, ACM_MSA_PCM,
							     client->control.playback_vol);
				}

				if (client->control.stereo_enable == 1)
				{
					dbg("Enable headset during voicecall\n");
					ACMAudioDeviceEnable(ACM_STEREO_SPEAKER, ACM_MSA_PCM,
							     client->control.playback_vol);
				}

				if (client->control.mic1_enable == 1)
				{
					dbg("Enable mic1 during voicecall\n");
					ACMAudioDeviceEnable(ACM_MIC, ACM_MSA_PCM,
							     client->control.capture_vol);
				}

				if (client->control.mic2_enable == 1)
				{
					dbg("Enable mic2 during voicecall\n");
					ACMAudioDeviceEnable(ACM_HEADSET_MIC, ACM_MSA_PCM,
							     client->control.capture_vol);
				}
			}
			else
			{
			#ifdef BT_SUPPORT
				system("hcitool cmd 0x3f 0x1d 0x01");
				system("hcitool cmd 0x3f 0x28 0x00 0x00 0x03");
				system("hcitool cmd 0x3f 0x07 0x00");
				dbg("bt addr %s\n", &addr);
				if (client->btcontrol.bt_speaker_enable = 1)
				{
					/* bt_speaker_enable */
					dbg("voice call bt_speaker_enable\n");
					ACMAudioDeviceEnable(ACM_BLUETOOTH_SPEAKER, ACM_MSA_PCM,
							     client->btcontrol.bt_playback_vol);
				}

				if (client->btcontrol.bt_mic_enable = 1)
				{
					/* bt_mic_enable */
					dbg("voice call bt_mic_enable\n");
					ACMAudioDeviceEnable(ACM_BLUETOOTH_MIC, ACM_MSA_PCM,
							     client->btcontrol.bt_capture_vol);
				}
				if (headset_connect(&addr, 2) != 0)
				{
					headset_connect(&addr, 2);
				}
			#endif
			}
			client->laststatus = PHONE_VOICECALL_ENABLE;
		}

		if (client->status == PHONE_PCMSTREAM_PLAYBACK_ENABLE && client->laststatus !=  PHONE_PCMSTREAM_PLAYBACK_ENABLE)
		{
			/* Enable voice call audio setting */

			dbg("Pcm stream playback enable\n");
#if 0
			if ((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
			{
				ERRMSG("open default device error\n");
			}
#endif
			ACMAudioStreamOutStart(ACM_PCM, ACM_NEAR_END, ACM_NOT_COMB_WITH_CALL, pcm_stream_write);

			if (client->codec_type == CODEC_MICCO)
			{
				if (client->control.mono_enable == 1 || client->control.bear_enable == 1)
				{
					dbg("Enable main speaker during pcm stream playback\n");
					ACMAudioDeviceEnable(ACM_MAIN_SPEAKER, ACM_MSA_PCM,
							     client->control.playback_vol);
				}

				if (client->control.stereo_enable == 1)
				{
					dbg("Enable stereo speaker during pcm stream playback\n");
					ACMAudioDeviceEnable(ACM_STEREO_SPEAKER, ACM_MSA_PCM,
							     client->control.playback_vol);
				}
			}
			else
			{
			#ifdef BT_SUPPORT
				if (client->btcontrol.bt_speaker_enable = 1)
				{
					/* bt_speaker_enable */
					dbg("Enable bt_speaker during pcm stream playback\n");
					ACMAudioDeviceEnable(ACM_BLUETOOTH_SPEAKER, ACM_MSA_PCM,
							     client->btcontrol.bt_playback_vol);
				}
			#endif
			}


			client->laststatus = PHONE_PCMSTREAM_PLAYBACK_ENABLE;
		}

		if (client->status == PHONE_PCMSTREAM_CAPTURE_ENABLE && client->laststatus != PHONE_PCMSTREAM_CAPTURE_ENABLE)
		{

			/* Enable voice call audio setting */
			dbg("Pcm stream capture enable\n");
#if 0
			if ((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
			{
				ERRMSG("open default device error\n");
			}
#endif
		if (client->codec_type == CODEC_MICCO)
			{
				if (client->control.mic1_enable == 1)
				{
					dbg("Enable mic1 during pcm stream capture\n");
					ACMAudioDeviceEnable(ACM_MIC, ACM_MSA_PCM,
							     client->control.capture_vol);
				}

				if (client->control.mic2_enable == 1)
				{
					dbg("Enable mic2 during pcm stream capture\n");
					ACMAudioDeviceEnable(ACM_HEADSET_MIC, ACM_MSA_PCM,
							     client->control.capture_vol);
				}
			}
			else
			{
			#ifdef BT_SUPPROT
				if (client->btcontrol.bt_mic_enable = 1)
				{
					/* bt_mic_enable */
					dbg("Enable bt_mic during pcm stream capture\n");
					ACMAudioDeviceEnable(ACM_BLUETOOTH_MIC, ACM_MSA_PCM,
							     client->btcontrol.bt_capture_vol);
				}
			#endif
			}

			client->laststatus = PHONE_PCMSTREAM_CAPTURE_ENABLE;
		}


	}
	else
	{
		/* Disable CP Audio */

		if (client->status == PHONE_VOICECALL_DISABLE && client->laststatus != PHONE_VOICECALL_DISABLE)
		{
			/* Disable voice call audio setting */
			dbg("Phone_voicecall_disable\n");
			ACMAudioStreamInStop(ACMVoidCB);
			ACMAudioStreamOutStop(ACMVoidCB);

			if (client->codec_type == CODEC_MICCO)
			{
				if (client->control.mono_enable == 1 || client->control.bear_enable == 1)
				{
					dbg("Disable main speaker during voicecall 1\n");
					ACMAudioDeviceDisable(ACM_MAIN_SPEAKER, ACM_MSA_PCM);
				}

				if (client->control.stereo_enable == 1)
				{
					dbg("Disable headset during voicecall 1\n");
					ACMAudioDeviceDisable(ACM_STEREO_SPEAKER, ACM_MSA_PCM);
				}

				if (client->control.mic1_enable == 1)
				{
					dbg("Disable mic1 during voicecall 1\n");
					ACMAudioDeviceDisable(ACM_MIC, ACM_MSA_PCM);
				}

				if (client->control.mic2_enable == 1)
				{
					dbg("Disable mic2 during voicecall 1\n");
					ACMAudioDeviceDisable(ACM_HEADSET_MIC, ACM_MSA_PCM);
				}
			}
			else
			{
			#ifdef BT_SUPPORT
				if (client->btcontrol.bt_speaker_enable = 1)
				{
					/* bt_speaker_enable */
					dbg("bt_speaker_Disable during voicecall \n");
					ACMAudioDeviceDisable(ACM_BLUETOOTH_SPEAKER, ACM_MSA_PCM);
				}
				if (client->btcontrol.bt_mic_enable = 1)
				{
					/* bt_mic_enable */
					dbg("bt_mic_disable during voicecall\n");
					ACMAudioDeviceDisable(ACM_BLUETOOTH_MIC, ACM_MSA_PCM);
				}
				headset_disconnect();
			#endif
			}
			//snd_pcm_close(handle);
			client->laststatus = PHONE_VOICECALL_DISABLE;
		}


		if (client->status == PHONE_PCMSTREAM_PLAYBACK_DISABLE && client->laststatus != PHONE_PCMSTREAM_PLAYBACK_DISABLE )
		{
			/* Enable voice call audio setting */
			dbg("Pcm stream playback disable\n");
			ACMAudioStreamOutStop(pcm_stream_write);
			if (client->control.mono_enable == 1 || client->control.bear_enable == 1)
			{
				dbg("Disable main speaker during pcm stream playback\n");
				ACMAudioDeviceDisable(ACM_MAIN_SPEAKER, ACM_MSA_PCM);
			}

			if (client->control.stereo_enable == 1)
			{
				dbg("Disable headset during pcm stream playback\n");
				ACMAudioDeviceDisable(ACM_STEREO_SPEAKER, ACM_MSA_PCM);
			}
			//snd_pcm_close(handle);
			client->laststatus = PHONE_PCMSTREAM_PLAYBACK_DISABLE;
		}


		if (client->status == PHONE_PCMSTREAM_CAPTURE_DISABLE && client->laststatus != PHONE_PCMSTREAM_CAPTURE_DISABLE)
		{
			/* Enable voice call audio setting */
			dbg("Pcm stream capture disable\n");
			ACMAudioStreamInStop(pcm_stream_read);
			if (client->control.mic1_enable == 1)
			{
				dbg("Disable mic1 during pcm stream capture\n");
				ACMAudioDeviceDisable(ACM_MIC, ACM_MSA_PCM);
			}

			if (client->control.mic2_enable == 1)
			{
				dbg("Disable mic2 during pcm stream capture\n");
				ACMAudioDeviceDisable(ACM_HEADSET_MIC, ACM_MSA_PCM);
			}

			//snd_pcm_close(handle);
			client->laststatus = PHONE_PCMSTREAM_CAPTURE_DISABLE;
		}
	}

	return 0;
}

int audio_set_client(audioserver* server)
{
	pthread_mutex_lock(&server->mutex);

	if ( server->bt_detect == DEV_DETECTED)
	{
		//dbg("bluetooth codec usage detected\n");
		server->clients[CLIENT0]->codec_type = CODEC_BT;
		server->clients[CLIENT0]->btcontrol.bt_speaker_enable = 1;
		server->clients[CLIENT0]->btcontrol.bt_mic_enable = 1;
		server->clients[CLIENT0]->btcontrol.bt_playback_vol = 75;
		server->clients[CLIENT0]->btcontrol.bt_capture_vol = 75;

		server->clients[CLIENT2]->codec_type = CODEC_BT;
		server->clients[CLIENT2]->btcontrol.bt_speaker_enable = 1;
		server->clients[CLIENT2]->btcontrol.bt_mic_enable = 1;
		server->clients[CLIENT2]->btcontrol.bt_playback_vol = 75;
		server->clients[CLIENT2]->btcontrol.bt_capture_vol = 75;

		strncpy((char *)addr, server->bt_addr, 256);
	}
	else
	{
		//dbg("micco codec usage detected\n");
		server->clients[CLIENT0]->codec_type = CODEC_MICCO;
		server->clients[CLIENT2]->codec_type = CODEC_MICCO;
	}

	if ( server->control.mono_enable == 1)
	{
		server->clients[CLIENT0]->control.mono_enable = 1;
		server->clients[CLIENT2]->control.mono_enable = 1;
	}

	if ( server->control.bear_enable == 1)
	{
		server->clients[CLIENT0]->control.bear_enable = 1;
		server->clients[CLIENT2]->control.bear_enable = 1;
	}

	if ( server->control.stereo_enable == 1)
	{
		server->clients[CLIENT0]->control.stereo_enable = 1;
		server->clients[CLIENT2]->control.stereo_enable = 1;
	}

	if ( server->control.mic1_enable == 1)
	{
		server->clients[CLIENT0]->control.mic1_enable = 1;
		server->clients[CLIENT2]->control.mic1_enable = 1;
	}

	if ( server->control.mic2_enable == 1)
	{
		server->clients[CLIENT0]->control.mic2_enable = 1;
		server->clients[CLIENT2]->control.mic2_enable = 1;
	}

	if ( server->control.playback_vol != 0 )
	{
		server->clients[CLIENT0]->control.playback_vol = server->control.playback_vol;
		server->clients[CLIENT2]->control.playback_vol = server->control.playback_vol;
	}

	if ( server->control.capture_vol != 0 )
	{
		server->clients[CLIENT0]->control.capture_vol = server->control.capture_vol;
		server->clients[CLIENT2]->control.capture_vol = server->control.capture_vol;
	}



	if ( server->hp_detect == DEV_DETECTED)
	{
		server->clients[CLIENT0]->control.stereo_enable = 1;
		server->clients[CLIENT2]->control.stereo_enable = 1;
		server->clients[CLIENT2]->control.mic2_enable = 1;
		server->clients[CLIENT2]->control.mic1_enable = 0;
	}

	if (server->curclient == CLIENT0)
	{
		if (server->status == AP_ENABLE)
		{
			dbg("audio_set_client 2\n");
			if (server->lastclient != CLIENT0)
			{
				dbg("audio_set_client 3\n");
				audio_set_client0(server->clients[CLIENT0], 1);
				server->lastclient = CLIENT0;
			}
		}
		else if (server->status == AP_DISABLE)
		{
			audio_set_client0(server->clients[CLIENT0], 0);
			server->lastclient = CLIENT0;
		}
	}
	else if (server->curclient == CLIENT2)
	{
		if (server->status == CP_ENABLE)
		{
			if (server->lastclient == CLIENT0)
			{
				if (server->clients[CLIENT0]->status == AP_CLIENT_ENABLE)
				{
					audio_set_client0(server->clients[CLIENT0], 0);
					server->clients[CLIENT0]->status = AP_CLIENT_DISABLE;
				}
			}

			audio_set_client2(server->clients[CLIENT2], 1);
			server->lastclient = CLIENT2;
		}
		else if (server->status == CP_DISABLE)
		{
			audio_set_client2(server->clients[CLIENT2], 0);
			audio_set_client0(server->clients[CLIENT0], 1);
			server->lastclient = CLIENT2;
			server->curclient = CLIENT0;
			server->status = AP_ENABLE;
		}

	}
	pthread_mutex_unlock(&server->mutex);

	return 0;
}


/* Plugin processing */
void phone_plugin_ctl_write(audioserver* server)
{
	AUDIOIPCDATA data = INVALIDIPCDATA;
	int reply = 0;

	if (recv_socket(server->sockfd, &data,
			sizeof(data)) == sizeof(data))
	{
		if (b_test_mode && ((data.param & (TESTMODE << 16)) == 0))
		{
			dbg("$$$$$$$$$$$$ test mode, param: 0x%x, Drop the IPC from Elair! $$$$$$$$$$\n", data.param);
			usleep(20000);
			send_socket(server->sockfd, &reply, sizeof(int));
			return;
		}
		int high = (data.param & 0xFFFF0000) >> 16;
		int low = data.param & 0x0000FFFF;
		dbg("---------method_id:%d,device_type:%d,param:%x\n", data.method_id, data.device_type, data.param);
		pthread_mutex_lock(&server->clients[CLIENT1]->mutex);

		/*switch calibration state if needed, 0 indicate normal state, 1 indicate calibration state for low value*/
		if(high & CALSTATESWITCH)
		{
			ACMAudioSetCalibrationStatus(low);
		}
		switch (data.method_id)
		{
		case METHOD_ENABLE:
			dbg("\n\nPhone_voicecall_enable \n\n");
			server->clients[CLIENT1]->status = CP_ENABLE;
			server->clients[CLIENT1]->control.playback_vol = low;

			switch (data.device_type)
			{
			case EARPIECE_DEVICE:
				dbg("bear_enable:h(%d)l(%d)\n", high, low);
				if (high & OUTPUT)
				{
					ACMAudioDeviceEnable((high & LOOPBACKTEST) ?
						ACM_MAIN_SPEAKER__LOOP : ACM_MAIN_SPEAKER,
						ACM_MSA_PCM, low);
				}
				if (high & INPUT)
				{
					ACMAudioDeviceEnable((high & LOOPBACKTEST) ?
						ACM_MIC__LOOP : ACM_MIC,
						ACM_MSA_PCM, low);
					ACMAudioDeviceMute((high & LOOPBACKTEST) ?
						ACM_MIC__LOOP : ACM_MIC,
						ACM_MSA_PCM, MUTE_STATE);
				}
				break;
			case SPEAKER_DEVICE:
				dbg("mono_enable\n");
				if (high & OUTPUT)
				{
					ACMAudioDeviceEnable((high & LOOPBACKTEST) ?
						ACM_AUX_SPEAKER__LOOP : ACM_AUX_SPEAKER,
						ACM_MSA_PCM, low);
				}
				if (high & INPUT)
				{
					ACMAudioDeviceEnable((high & LOOPBACKTEST) ?
						ACM_AUX_MIC__LOOP : ACM_AUX_MIC,
						ACM_MSA_PCM, low);
					ACMAudioDeviceMute((high & LOOPBACKTEST) ?
						ACM_AUX_MIC__LOOP : ACM_AUX_MIC,
						ACM_MSA_PCM, MUTE_STATE);
				}
				break;
			case BLUETOOTH_DEVICE:
				dbg("bluetooth_enable\n");
#ifdef BT_SUPPORT
				system("hcitool cmd 0x3f 0x1d 0x01");
				system("hcitool cmd 0x3f 0x28 0x00 0x00 0x03");
				system("hcitool cmd 0x3f 0x07 0x00");
				printf("bt addr %s\n", &addr);
#endif
				if (high & OUTPUT)
				{
					ACMAudioDeviceEnable(ACM_BLUETOOTH_SPEAKER, ACM_MSA_PCM, low);
				}
				if (high & INPUT)
				{
					ACMAudioDeviceEnable(ACM_BLUETOOTH_MIC, ACM_MSA_PCM, low);
					ACMAudioDeviceMute(ACM_BLUETOOTH_MIC, ACM_MSA_PCM, MUTE_STATE);
				}
#ifdef BT_SUPPORT
				if (headset_connect(&addr, 2) != 0)
				{
					headset_connect(&addr, 2);
				}
#endif
				break;
			case BT_NREC_DEVICE:
				dbg("BT_NREC_device_enable\n");
				if (high & OUTPUT)
				{
					ACMAudioDeviceEnable(ACM_BLUETOOTH9_SPEAKER, ACM_MSA_PCM, low);
				}
				if (high & INPUT)
				{
					ACMAudioDeviceEnable(ACM_BLUETOOTH9_MIC, ACM_MSA_PCM, low);
					ACMAudioDeviceMute(ACM_BLUETOOTH9_MIC, ACM_MSA_PCM, MUTE_STATE);
				}
				break;
			case HEADSET_DEVICE:
				dbg("stereo_enable\n");
				if (high & OUTPUT)
				{
					ACMAudioDeviceEnable((high & LOOPBACKTEST) ?
						ACM_HEADSET_SPEAKER__LOOP : ACM_HEADSET_SPEAKER,
						ACM_MSA_PCM, low);
				}
				if (high & INPUT)
				{
					ACMAudioDeviceEnable((high & LOOPBACKTEST) ?
						ACM_HEADSET_MIC__LOOP : ACM_HEADSET_MIC,
						ACM_MSA_PCM, low);
					ACMAudioDeviceMute((high & LOOPBACKTEST) ?
						ACM_HEADSET_MIC__LOOP : ACM_HEADSET_MIC,
						ACM_MSA_PCM, MUTE_STATE);
				}
				break;
			case HEADPHONE_DEVICE:
				dbg("headphone_enable\n");
				if (high & OUTPUT)
				{
					ACMAudioDeviceEnable((high & LOOPBACKTEST) ?
						ACM_HEADSET_SPEAKER__LOOP : ACM_HEADSET_SPEAKER,
						ACM_MSA_PCM, low);
				}
				if (high & INPUT)
				{
					ACMAudioDeviceEnable((high & LOOPBACKTEST) ?
						ACM_AUX_MIC__LOOP : ACM_AUX_MIC,
						ACM_MSA_PCM, low);
					ACMAudioDeviceMute((high & LOOPBACKTEST) ?
						ACM_AUX_MIC__LOOP : ACM_AUX_MIC,
						ACM_MSA_PCM, MUTE_STATE);
				}
				break;
			default:
				break;
			}

			if (high & PCMSTREAMPLAYBACK)
			{
				ACM_SrcDst whichEnd = 0;
				ACM_CombWithCall needComb = 0;
				if ((high & NEAREND) && (high & FAREND))
					whichEnd = ACM_BOTH_ENDS;
				else if (high & NEAREND)
					whichEnd = ACM_NEAR_END;
				else if (high & FAREND)
					whichEnd = ACM_FAR_END;
				if (high & COMBWITHCALL)
					needComb = ACM_COMB_WITH_CALL;
				else
					needComb = ACM_NOT_COMB_WITH_CALL;
				ACMAudioStreamOutStart(ACM_PCM, whichEnd, needComb, pcm_stream_write);
			}
			else if (high & PCMSTREAMCAPTURE)
			{
				ACM_SrcDst whichEnd = 0;
				if ((high & NEAREND) && (high & FAREND))
					whichEnd = ACM_BOTH_ENDS;
				else if (high & NEAREND)
					whichEnd = ACM_NEAR_END;
				else if (high & FAREND)
					whichEnd = ACM_FAR_END;
				ACMAudioStreamInStart(ACM_PCM, whichEnd, pcm_stream_read);
			}
			else if (!(high & LOOPBACKTEST))
			{
				ACMAudioStreamInStart(ACM_VOICE_CALL, ACM_NEAR_END, ACMVoidCB);
				ACMAudioStreamOutStart( ACM_VOICE_CALL, ACM_NEAR_END, ACM_NOT_COMB_WITH_CALL, ACMVoidCB);
			}

			break;
		case METHOD_DISABLE:
			dbg("\n\nPhone_voicecall_disable \n\n");
			server->clients[CLIENT1]->status = CP_DISABLE;

			if (high & PCMSTREAMPLAYBACK)
			{
				ACMAudioStreamOutStop(pcm_stream_write);
			}
			else if (high & PCMSTREAMCAPTURE)
			{
				ACMAudioStreamInStop(pcm_stream_read);
			}
			else if (!(high & LOOPBACKTEST))
			{
				ACMAudioStreamInStop(ACMVoidCB);
				ACMAudioStreamOutStop(ACMVoidCB);
			}

			switch (data.device_type)
			{
			case EARPIECE_DEVICE:
				dbg("bear_disable\n");
				if (high & OUTPUT)
				{
					ACMAudioDeviceDisable((high & LOOPBACKTEST) ?
						ACM_MAIN_SPEAKER__LOOP : ACM_MAIN_SPEAKER,
						ACM_MSA_PCM);
				}
				if (high & INPUT)
				{
					ACMAudioDeviceDisable((high & LOOPBACKTEST) ?
						ACM_MIC__LOOP : ACM_MIC,
						ACM_MSA_PCM);
				}
				break;
			case SPEAKER_DEVICE:
				dbg("mono_disable\n");
				if (high & OUTPUT)
				{
					ACMAudioDeviceDisable((high & LOOPBACKTEST) ?
						ACM_AUX_SPEAKER__LOOP : ACM_AUX_SPEAKER,
						ACM_MSA_PCM);
				}
				if (high & INPUT)
				{
					ACMAudioDeviceDisable((high & LOOPBACKTEST) ?
						ACM_AUX_MIC__LOOP : ACM_AUX_MIC,
						ACM_MSA_PCM);
				}
				break;
			case BLUETOOTH_DEVICE:
				dbg("bluetooth_disable\n");
				if (high & OUTPUT)
				{
					ACMAudioDeviceDisable(ACM_BLUETOOTH_SPEAKER, ACM_MSA_PCM);
				}
				if (high & INPUT)
				{
					ACMAudioDeviceDisable(ACM_BLUETOOTH_MIC, ACM_MSA_PCM);
				}
#ifdef BT_SUPPORT
				headset_disconnect();
#endif
				break;
			case BT_NREC_DEVICE:
				dbg("BT_NREC_disable\n");
				if (high & OUTPUT)
				{
					ACMAudioDeviceDisable(ACM_BLUETOOTH9_SPEAKER, ACM_MSA_PCM);
				}
				if (high & INPUT)
				{
					ACMAudioDeviceDisable(ACM_BLUETOOTH9_MIC, ACM_MSA_PCM);
				}

				break;
			case HEADSET_DEVICE:
				dbg("stereo_disable\n");
				if (high & OUTPUT)
				{
					ACMAudioDeviceDisable((high & LOOPBACKTEST) ?
						ACM_HEADSET_SPEAKER__LOOP : ACM_HEADSET_SPEAKER,
						ACM_MSA_PCM);
				}
				if (high & INPUT)
				{
					ACMAudioDeviceDisable((high & LOOPBACKTEST) ?
						ACM_HEADSET_MIC__LOOP : ACM_HEADSET_MIC,
						ACM_MSA_PCM);
				}
				break;
			case HEADPHONE_DEVICE:
				dbg("headphone_disable\n");
				if (high & OUTPUT)
				{
					ACMAudioDeviceDisable((high & LOOPBACKTEST) ?
						ACM_HEADSET_SPEAKER__LOOP : ACM_HEADSET_SPEAKER,
						ACM_MSA_PCM);
				}
				if (high & INPUT)
				{
					ACMAudioDeviceDisable((high & LOOPBACKTEST) ?
						ACM_AUX_MIC__LOOP : ACM_AUX_MIC,
						ACM_MSA_PCM);
				}
				break;
			default:
				break;
			}
			break;
		case METHOD_MUTE:
			dbg("\n\nPhone_voicecall_mute \n\n");
			server->clients[CLIENT1]->control.playback_vol = 0;
			MUTE_STATE = low;
			switch (data.device_type)
			{
			case EARPIECE_DEVICE:
				dbg("bear_mute\n");
				if (high & OUTPUT)
				{
					ACMAudioDeviceMute((high & LOOPBACKTEST) ?
						ACM_MAIN_SPEAKER__LOOP : ACM_MAIN_SPEAKER,
						ACM_MSA_PCM, low);
				}
				if (high & INPUT)
					ACMAudioDeviceMute((high & LOOPBACKTEST) ?
						ACM_MIC__LOOP : ACM_MIC,
						ACM_MSA_PCM, low);
				break;
			case SPEAKER_DEVICE:
				dbg("mono_mute\n");
				if (high & OUTPUT)
					ACMAudioDeviceMute((high & LOOPBACKTEST) ?
						ACM_AUX_SPEAKER__LOOP : ACM_AUX_SPEAKER,
						ACM_MSA_PCM, low);
				if (high & INPUT)
					ACMAudioDeviceMute((high & LOOPBACKTEST) ?
						ACM_AUX_MIC__LOOP : ACM_AUX_MIC,
						ACM_MSA_PCM, low);
				break;
			case BLUETOOTH_DEVICE:
				dbg("bluetooth_mute\n");
				if (high & OUTPUT)
					ACMAudioDeviceMute(ACM_BLUETOOTH_SPEAKER, ACM_MSA_PCM, low);
				if (high & INPUT)
					ACMAudioDeviceMute(ACM_BLUETOOTH_MIC, ACM_MSA_PCM, low);
				break;
			case BT_NREC_DEVICE:
				dbg("bT_nrec_mute\n");
				if (high & OUTPUT)
					ACMAudioDeviceMute(ACM_BLUETOOTH9_SPEAKER, ACM_MSA_PCM, low);
				if (high & INPUT)
					ACMAudioDeviceMute(ACM_BLUETOOTH9_MIC, ACM_MSA_PCM, low);
				break;
			case HEADSET_DEVICE:
				dbg("stereo_mute\n");
				if (high & OUTPUT)
					ACMAudioDeviceMute((high & LOOPBACKTEST) ?
						ACM_HEADSET_SPEAKER__LOOP : ACM_HEADSET_SPEAKER,
						ACM_MSA_PCM, low);
				if (high & INPUT)
					ACMAudioDeviceMute((high & LOOPBACKTEST) ?
						ACM_HEADSET_MIC__LOOP : ACM_HEADSET_MIC,
						ACM_MSA_PCM, low);
				break;
			case HEADPHONE_DEVICE:
				dbg("headphone_mute\n");
				if (high & OUTPUT)
					ACMAudioDeviceMute((high & LOOPBACKTEST) ?
						ACM_HEADSET_SPEAKER__LOOP : ACM_HEADSET_SPEAKER,
						ACM_MSA_PCM, low);
				if (high & INPUT)
					ACMAudioDeviceMute((high & LOOPBACKTEST) ?
						ACM_AUX_MIC__LOOP : ACM_AUX_MIC,
						ACM_MSA_PCM, low);
				break;
			default:
				break;
			}
			break;
		case METHOD_VOLUME_SET:
			dbg("\n\nPhone_voicecall_volume_set \n\n");
			server->clients[CLIENT1]->control.playback_vol = low;
			switch (data.device_type)
			{
			case EARPIECE_DEVICE:
				dbg("bear_volume_set\n");
				if (high & OUTPUT)
				{
					ACMAudioDeviceVolumeSet((high & LOOPBACKTEST) ?
						ACM_MAIN_SPEAKER__LOOP : ACM_MAIN_SPEAKER,
						ACM_MSA_PCM, low);
				}
				if (high & INPUT)
					ACMAudioDeviceVolumeSet((high & LOOPBACKTEST) ?
						ACM_MIC__LOOP : ACM_MIC,
						ACM_MSA_PCM, low);
				break;
			case SPEAKER_DEVICE:
				dbg("mono_volume_set\n");
				if (high & OUTPUT)
					ACMAudioDeviceVolumeSet((high & LOOPBACKTEST) ?
						ACM_AUX_SPEAKER__LOOP : ACM_AUX_SPEAKER,
						ACM_MSA_PCM, low);
				if (high & INPUT)
					ACMAudioDeviceVolumeSet((high & LOOPBACKTEST) ?
						ACM_AUX_MIC__LOOP : ACM_AUX_MIC,
						ACM_MSA_PCM, low);
				break;
			case BLUETOOTH_DEVICE:
				dbg("bluetooth_volume_set\n");
				if (high & OUTPUT)
					ACMAudioDeviceVolumeSet(ACM_BLUETOOTH_SPEAKER, ACM_MSA_PCM, low);
				if (high & INPUT)
					ACMAudioDeviceVolumeSet(ACM_BLUETOOTH_MIC, ACM_MSA_PCM, low);
				break;
			case BT_NREC_DEVICE:
				dbg("bT_nrec_volume_set\n");
				if (high & OUTPUT)
					ACMAudioDeviceVolumeSet(ACM_BLUETOOTH9_SPEAKER, ACM_MSA_PCM, low);
				if (high & INPUT)
					ACMAudioDeviceVolumeSet(ACM_BLUETOOTH9_MIC, ACM_MSA_PCM, low);
				break;
			case HEADSET_DEVICE:
				dbg("stereo_volume_set\n");
				if (high & OUTPUT)
					ACMAudioDeviceVolumeSet((high & LOOPBACKTEST) ?
						ACM_HEADSET_SPEAKER__LOOP : ACM_HEADSET_SPEAKER,
						ACM_MSA_PCM, low);
				if (high & INPUT)
					ACMAudioDeviceVolumeSet((high & LOOPBACKTEST) ?
						ACM_HEADSET_MIC__LOOP : ACM_HEADSET_MIC,
						ACM_MSA_PCM, low);
				break;
			case HEADPHONE_DEVICE:
				dbg("headphone_volume_set\n");
				if (high & OUTPUT)
					ACMAudioDeviceVolumeSet((high & LOOPBACKTEST) ?
						ACM_HEADSET_SPEAKER__LOOP : ACM_HEADSET_SPEAKER,
						ACM_MSA_PCM, low);
				if (high & INPUT)
					ACMAudioDeviceVolumeSet((high & LOOPBACKTEST) ?
						ACM_AUX_MIC__LOOP : ACM_AUX_MIC,
						ACM_MSA_PCM, low);
				break;
			default:
				break;
			}
			break;
		case PATH_ENABLE:
			{
				unsigned short componentnumber = (high & 0xff00) >> 8;
				unsigned short pathnumber = high & 0xff;
				dbg("\n\nACMAudioPathEnable, use high to pass component and path number parameter, component = %d, path=%d, low=%d\n\n", componentnumber, pathnumber, low);
				ACMAudioPathEnable(componentnumber, pathnumber, low);
			}
			break;
		case PATH_DISABLE:
			{
				unsigned short componentnumber = (high & 0xff00) >> 8;
				unsigned short pathnumber = high & 0xff;
				dbg("\n\nACMAudioPathDisable, use high to pass component and path number parameter, component = %d, path=%d\n\n", componentnumber, pathnumber);
				ACMAudioPathDisable(componentnumber, pathnumber);
			}
			break;

		default:
			break;
		}
		pthread_mutex_unlock(&server->clients[CLIENT1]->mutex);

		send_socket(server->sockfd, &reply, sizeof(int));
	}
}

void phone_plugin_ctl_read(audioserver* server)
{
	AUDIOCONTROLINFOS AudioMixerData = {0, 0};

	AudioMixerData.volume = server->clients[CLIENT1]->control.playback_vol;
	AudioMixerData.status = server->clients[CLIENT1]->status;

	send_socket(server->sockfd, &AudioMixerData, sizeof(AudioMixerData));

}

/****************************************************************************
 *
 * phone_plugin_pcm_write
 *
 ***************************************************************************/
void *phone_pcm_write_thread(void * param)
{
	audioclient* client = (audioclient*)param;
	int berror = 0;
	int ret = 0;
	struct pollfd fds[2];
	unsigned char cmd = 0;
	RINGINFO buffer;
	unsigned int write_frames = 0;
	unsigned int underrun_count = 0;
	unsigned int write_poll = 0;
	unsigned int write_buffer_count = 0;

	ring_buffer_init(&ringrecv);
	setup_socket(client->sockfd);
	start_flag = 0;
	write_indicate = 0;


	if(pipe(pipeWriteFd) < 0) {
		dbg("can not create pipe\n");
		berror = 1;
	}

	fds[0].fd     =  pipeWriteFd[0];
	fds[0].events =  POLLIN;
	fds[0].revents=  0;
	fds[1].fd     =  client->sockfd;
	fds[1].events =  POLLIN;
	fds[1].revents=  0;

	while ( (run_level >= RUN_LEVEL6) && !berror )
	{
		ret = poll(fds, 2, PCM_WRITE_POLL_TIMEOUT);
		if (ret <= 0) {
			dbg("error - write poll return %d \n", ret);
			break;
		}

		if(fds[0].revents & POLLIN) {
			if(write_poll) {
				if((ret = send_socket(client->sockfd, (unsigned char *)&cmd, sizeof(unsigned char)) <= 0)) {
					berror = 1;
					DPRINTF("phone_pcm_write_thread send poll notification return %d, errno %d, sockfd %d, thread %d\n", ret, errno, client->sockfd, client->recvthread);
					break;
				}
				write_poll = 0;
			}

			ret = read(pipeWriteFd[0], (unsigned char *)&cmd, 1);
			if(ret <= 0) {
				dbg("error - can not get write indicate \n");
				break;
			}

			if(cmd == 'U') {
				underrun_count ++;
				dbg("underrun is found - %d \n", underrun_count);
			}

			write_indicate ++;
		}

		if(fds[1].revents & POLLIN) {
			/* read command */
			if ((ret = recv_socket(client->sockfd, &cmd, sizeof(unsigned char))) <= 0) {
				dbg("phone_pcm_write_thread recv cmd - ret: %d, errno %d, sockfd %d, thread %d\n", ret, errno, client->sockfd, client->recvthread);
				break;
			}

			if(cmd == PHONE_CMD_WRITE) {
				buffer.buf = malloc(320);
				if ((ret = recv_socket(client->sockfd, buffer.buf, 320)) > 0)
				{
					if(start_flag <= 0) start_flag = 1;

					if(ring_buffer_in(&ringrecv, buffer) == 0)
					{
						dbg("phone_pcm_write_thread: ringbuffer overflow - %d\n", ring_buffer_length(&ringrecv));
						free(buffer.buf);
						berror = 1;
					}
				}
				else
				{
					dbg("phone_pcm_write_thread recv return - ret %d, errno %d, sockfd %d, thread %d\n", ret, errno, client->sockfd, client->recvthread);
					free(buffer.buf);
					berror = 1;
				}
			}

			if(cmd == PHONE_CMD_QUERY) {
				write_frames = write_indicate * 160;
				if((ret = send_socket(client->sockfd, (unsigned char *)&write_frames, sizeof(unsigned int)) <= 0)) {
					berror = 1;
					DPRINTF("phone_pcm_write_thread recv return, errno %d, sockfd %d, thread %d\n", errno, client->sockfd, client->recvthread);
				}
			}

			if(cmd == PHONE_CMD_POLL) {
				write_poll = 1;
			}

			if(cmd == PHONE_CMD_WBUF) {
				if ((ret = recv_socket(client->sockfd, (unsigned char *)&write_buffer_count, sizeof(unsigned int))) <= 0)
				{
					dbg("phone_pcm_write_thread recv return - ret %d, errno %d, sockfd %d, thread %d\n", ret, errno, client->sockfd, client->recvthread);
					berror = 1;
				}

				write_buffer_count = write_buffer_count/160;
				dbg("phone write buffer count: %d \n", write_buffer_count);
			}
		}

	    /* Underrun Handling
	    Drop packets during underrun */
		while((underrun_count) && ((UINT32)ring_buffer_length(&ringrecv) > write_buffer_count)) {
			if(ring_buffer_out(&ringrecv, &buffer) != 0) {
				underrun_count --;
			} else break;
		}
	}

	start_flag = 0;
	write_indicate = 0;
	client->recv_status = 2;
	close_socket(client->sockfd);
	ring_buffer_destroy(&ringrecv);

	/* clear pipe */
	if(pipeWriteFd[0] > 0) {
		close(pipeWriteFd[0]);
		pipeWriteFd[0] = -1;
	}

	if(pipeWriteFd[1] > 0) {
		close(pipeWriteFd[1]);
		pipeWriteFd[1] = -1;
	}

	client->recvthread = 0;
	pthread_exit(NULL);
	return 0;
}

int check_recv_thread_status(audioserver* server)
{
	audioclient* client = server->clients[CLIENT2];

	if (client->recv_status == 2)
	{
		/* destory the phone_pcm_write_thread */
		pthread_attr_destroy(&client->recv_attr);
		client->recv_status = 0;
	}

	return 0;
}

int phone_plugin_pcm_write(audioserver* server)
{

	audioclient * client = server->clients[CLIENT1]; /*FIXME, Raul: temporary we use CLIENT1 for playback voice call*/
	int sockfd = 0;


	sockfd = make_server_transock(TRANSOCKPORT4WRITE); /*Raul: use TRANSOCKPORT4WRITE for playback voice call*/

	if (sockfd > 0)
	{
		int new_fd = -1;
		new_fd = accept_socket(sockfd);
		//printf("phone_plugin_pcm_write accept_socket %d\n", new_fd);
		if (new_fd > 0)
		{

			size_t size = m_size; //PTHREAD_STACK_MIN;
			int ret = pthread_attr_init(&client->recv_attr);
			ret = pthread_attr_setstacksize(&client->recv_attr, size);
			//we need to wait the prior thread to exit
			if (client->recvthread != 0)
			{
				pthread_join(client->recvthread, NULL);
				client->recvthread = 0;
			}
			client->sockfd = new_fd;

			setup_socket(client->sockfd);
			pthread_create(&client->recvthread, &client->recv_attr, phone_pcm_write_thread, client);
			/* start flag for recv thread */
			pthread_attr_destroy(&client->recv_attr);
			client->recv_status = 1;

		}
		else
		{
			ERRMSG("Audioserver :phone_plugin_pcm_write :error connect with plugin\n");
			close_socket(new_fd);

		}

		close_socket(sockfd);
	}
	else
	{

		close_socket(sockfd);
		ERRMSG("phone_plugin_pcm_write: Cannot get the socket");
	}

	close_socket(sockfd);
	return 0;
}

/********************************************************************
 *
 * phone_plugin_pcm_read
 *
 *******************************************************************/
#define SEND_PACKET_NSEC         20000000    /* 20 ms */
const struct timespec send_pkt_interval = { .tv_sec = 0, .tv_nsec = SEND_PACKET_NSEC };


int check_send_thread_status(audioserver* server)
{
	audioclient* client = server->clients[CLIENT2];

	if (client->send_status == 2)
	{
		/* destory the phone_pcm_read_thread */
		pthread_attr_destroy(&client->send_attr);
		client->send_status = 0;
	}

	return 0;
}


void *phone_pcm_read_thread(void * param)
{
	audioclient* client = (audioclient*)param;
	unsigned char cmd = 0;
	int berror = 0;
	int ret = 0;
	RINGINFO readbuffer;
	struct pollfd fds[2];
	unsigned int read_frames = 0;
	unsigned int read_poll = 0;

	ring_buffer_init(&ringsend);
	setup_socket(client->sockfd);
	read_indicate = 0;

	if(pipe(pipeReadFd) < 0) {
		dbg("can not create pipe\n");
	}

    fds[0].fd     =  pipeReadFd[0];
	fds[0].events =  POLLIN;
	fds[0].revents=  0;
	fds[1].fd     =  client->sockfd;
	fds[1].events =  POLLIN;
	fds[1].revents=  0;

	while ( (run_level >= RUN_LEVEL6) && !berror )
	{
		ret = poll(fds, 2, PCM_READ_POLL_TIMEOUT);
		if (ret <= 0) {
			dbg("error - read poll return %d \n", ret);
			break;
		}

		if(fds[0].revents & POLLIN) {
			if(read_poll) {
				if((ret = send_socket(client->sockfd, (unsigned char *)&cmd, sizeof(unsigned char)) <= 0)) {
					berror = 1;
					dbg("phone_pcm_read_thread send poll notification return %d, errno %d, sockfd %d, thread %d\n", ret, errno, client->sockfd, client->sendthread);
				}
				read_poll = 0;
			}

			ret = read(pipeReadFd[0], (unsigned char *)&cmd, 1);
			if(ret < 1) {
				berror = 1;
				dbg("can not read notification command \n");
				break;
			}

			read_indicate ++;
		}

		if(fds[1].revents & POLLIN) {
			/* read command */
			if ((ret = recv_socket(client->sockfd, &cmd, sizeof(unsigned char))) <= 0) {
				dbg("phone_pcm_read_thread recv cmd %d - errno %d, sockfd %d, thread %d\n", ret, errno, client->sockfd, client->sendthread);
				break;
			}

			/* read data */
			if(cmd == PHONE_CMD_READ) {
				if (ring_buffer_out(&ringsend, &readbuffer) != 0) {
					if (send_socket(client->sockfd, readbuffer.buf, PCM_BUFF_SIZE * 2) <= 0)
					{
						berror = 1;
						dbg("phone_pcm_read_thread send return, errno %d, sockfd %d\n", errno, client->sockfd);
					}
					free(readbuffer.buf);
				} else {
					berror = 1;
					dbg("ring buffer is empty... \n");
				}
			}

			/* hw pointer query */
			if(cmd == PHONE_CMD_QUERY) {
				read_frames = read_indicate * 160;
				if((ret = send_socket(client->sockfd, (unsigned char *)&read_frames, sizeof(unsigned int)) <= 0)) {
					berror = 1;
					dbg("phone_pcm_read_thread recv return, errno %d, sockfd %d, thread %d\n", errno, client->sockfd, client->sendthread);
				}
			}

			/* polling */
			if(cmd == PHONE_CMD_POLL) {
				read_poll = 1;
			}
		}
	}

	/* clear pipe */
	if(pipeReadFd[0] > 0) {
		close(pipeReadFd[0]);
		pipeReadFd[0] = -1;
	}

	if(pipeReadFd[1] > 0) {
		close(pipeReadFd[1]);
		pipeReadFd[1] = -1;
	}

	ring_buffer_destroy(&ringsend);
	client->send_status = 2;
	close_socket(client->sockfd);

	client->sendthread = 0;
	pthread_exit(NULL);
	return 0;
}


void hifi_plugin_ctl_write(audioserver* server)
{
	AUDIOIPCDATA data = INVALIDIPCDATA;
	int reply = 0;
	int ret;

	if ((ret = recv_socket(server->sockfd, &data,
			       sizeof(data))) == sizeof(data))
	{
		if (b_test_mode && ((data.param & (TESTMODE << 16)) == 0))
		{
			dbg("$$$$$$$$$$$$ test mode, param: 0x%x, Drop the IPC from Elair! $$$$$$$$$$\n", data.param);
			usleep(20000);
			send_socket(server->sockfd, &reply, sizeof(int));
			return;
		}
		int high = (data.param & 0xFFFF0000) >> 16;
		int low = data.param & 0x0000FFFF;
		dbg("---------method_id:%d,device_type:%d,param:%x\n", data.method_id, data.device_type, data.param);
		pthread_mutex_lock(&server->clients[CLIENT0]->mutex);

		// switch sample rate if needed, between 8k(vt) and 44.1k(hifi)
		if (high & SRSWITCH)
		{
			printf("ACMSwitchingSampleRate %d\n", low);
			ACMSwitchingSampleRate(low);
		}

		switch (data.method_id)
		{
		case METHOD_ENABLE:
			dbg("\n\nhifi_enable\n\n");
			server->clients[CLIENT0]->status = AP_ENABLE;
			server->clients[CLIENT0]->control.playback_vol = low;
			switch (data.device_type)
			{
			case EARPIECE_DEVICE:
				dbg("bear_enable\n");
				if (high & OUTPUT)
					ACMAudioDeviceEnable(ACM_MAIN_SPEAKER, ACM_I2S, low);
				if (high & INPUT)
					ACMAudioDeviceEnable(((high & ECENABLE) ?
						ACM_MIC_EC : ((high & LOOPBACKTEST) ?
						ACM_MIC_LOOP_EARPIECE : ACM_MIC)),
						ACM_I2S, low);
				break;
			case SPEAKER_DEVICE:
				dbg("mono_enable\n");
				if (high & OUTPUT)
					ACMAudioDeviceEnable(ACM_AUX_SPEAKER, ACM_I2S, low);
				if (high & INPUT)
					ACMAudioDeviceEnable(((high & ECENABLE) ?
						ACM_AUX_MIC_EC : ((high & LOOPBACKTEST) ?
						ACM_MIC_LOOP_SPEAKER : ACM_AUX_MIC)),
						ACM_I2S, low);
				break;
			case BLUETOOTH_DEVICE:
				dbg("bluetooth_enable\n");
				if (high & OUTPUT)
					ACMAudioDeviceEnable(ACM_BLUETOOTH_SPEAKER, ACM_I2S, low);
				if (high & INPUT)
					ACMAudioDeviceEnable(ACM_BLUETOOTH_MIC, ACM_I2S, low);
				break;
			case HEADSET_DEVICE:
				dbg("stereo_enable\n");
				if (high & OUTPUT)
					ACMAudioDeviceEnable(ACM_HEADSET_SPEAKER, ACM_I2S, low);
				if (high & INPUT)
					ACMAudioDeviceEnable(((high & ECENABLE) ?
						ACM_HEADSET_MIC_EC : ((high & LOOPBACKTEST) ?
						ACM_HEADSET_MIC_LOOP : ACM_HEADSET_MIC)),
						ACM_I2S, low);
				break;
			case HEADPHONE_DEVICE:
				dbg("headphone_enable\n");
				if (high & OUTPUT)
					ACMAudioDeviceEnable(ACM_HEADSET_SPEAKER, ACM_I2S, low);
				if (high & INPUT)
					ACMAudioDeviceEnable(((high & ECENABLE) ?
						ACM_AUX_MIC_EC : ACM_AUX_MIC),
						ACM_I2S, low);
				break;
			case FORCE_SPEAKER_DEVICE:
				dbg("force_speaker_enable\n");
				ACMAudioDeviceEnable(ACM_FORCE_SPEAKER, ACM_I2S, low);
				break;
			default:
				break;
			}
			break;
		case METHOD_DISABLE:
			dbg("\n\nhifi_disable\n\n");
			server->clients[CLIENT0]->status = AP_DISABLE;
			switch (data.device_type)
			{
			case EARPIECE_DEVICE:
				dbg("bear_disable\n");
				if (high & OUTPUT)
					ACMAudioDeviceDisable(ACM_MAIN_SPEAKER, ACM_I2S);
				if (high & INPUT)
					ACMAudioDeviceDisable(((high & ECENABLE) ?
						ACM_MIC_EC : ((high & LOOPBACKTEST) ?
						ACM_MIC_LOOP_EARPIECE : ACM_MIC)), ACM_I2S);
				break;
			case SPEAKER_DEVICE:
				dbg("mono_disable\n");
				if (high & OUTPUT)
					ACMAudioDeviceDisable(ACM_AUX_SPEAKER, ACM_I2S);
				if (high & INPUT)
					ACMAudioDeviceDisable(((high & ECENABLE) ?
						ACM_AUX_MIC_EC : ((high & LOOPBACKTEST) ?
						ACM_MIC_LOOP_SPEAKER : ACM_AUX_MIC)), ACM_I2S);
				break;
			case BLUETOOTH_DEVICE:
				dbg("bluetooth_disable\n");
				if (high & OUTPUT)
					ACMAudioDeviceDisable(ACM_BLUETOOTH_SPEAKER, ACM_I2S);
				if (high & INPUT)
					ACMAudioDeviceDisable(ACM_BLUETOOTH_MIC, ACM_I2S);
				break;
			case HEADSET_DEVICE:
				dbg("stereo_disable\n");
				if (high & OUTPUT)
					ACMAudioDeviceDisable(ACM_HEADSET_SPEAKER, ACM_I2S);
				if (high & INPUT)
					ACMAudioDeviceDisable(((high & ECENABLE) ?
						ACM_HEADSET_MIC_EC : ((high & LOOPBACKTEST) ?
						ACM_HEADSET_MIC_LOOP : ACM_HEADSET_MIC)), ACM_I2S);
				break;
			case HEADPHONE_DEVICE:
				dbg("headphone_disable\n");
				if (high & OUTPUT)
					ACMAudioDeviceDisable(ACM_HEADSET_SPEAKER, ACM_I2S);
				if (high & INPUT)
					ACMAudioDeviceDisable(((high & ECENABLE) ?
						ACM_AUX_MIC_EC : ACM_AUX_MIC), ACM_I2S);
				break;
			case FORCE_SPEAKER_DEVICE:
				dbg("FORCE_SPEAKER_DEVICE_disable\n");
				ACMAudioDeviceDisable(ACM_FORCE_SPEAKER, ACM_I2S);
				break;
			default:
				break;
			}
			break;
		case METHOD_MUTE:
			dbg("\n\nhifi_mute\n\n");
			server->clients[CLIENT0]->control.playback_vol = 0;
			switch (data.device_type)
			{
			case EARPIECE_DEVICE:
				dbg("bear_mute\n");
				if (high & OUTPUT)
					ACMAudioDeviceMute(ACM_MAIN_SPEAKER, ACM_I2S, low);
				if (high & INPUT)
					ACMAudioDeviceMute(((high & ECENABLE) ?
						ACM_MIC_EC : ((high & LOOPBACKTEST) ?
						ACM_MIC_LOOP_EARPIECE : ACM_MIC)),
						ACM_I2S, low);
				break;
			case SPEAKER_DEVICE:
				dbg("mono_mute\n");
				if (high & OUTPUT)
					ACMAudioDeviceMute(ACM_AUX_SPEAKER, ACM_I2S, low);
				if (high & INPUT)
					ACMAudioDeviceMute(((high & ECENABLE) ?
						ACM_AUX_MIC_EC : ((high & LOOPBACKTEST) ?
						ACM_MIC_LOOP_SPEAKER : ACM_AUX_MIC)),
						ACM_I2S, low);
				break;
			case BLUETOOTH_DEVICE:
				dbg("bluetooth_mute\n");
				if (high & OUTPUT)
					ACMAudioDeviceMute(ACM_BLUETOOTH_SPEAKER, ACM_I2S, low);
				if (high & INPUT)
					ACMAudioDeviceMute(ACM_BLUETOOTH_MIC, ACM_I2S, low);
				break;
			case HEADSET_DEVICE:
				dbg("stereo_mute\n");
				if (high & OUTPUT)
					ACMAudioDeviceMute(ACM_HEADSET_SPEAKER, ACM_I2S, low);
				if (high & INPUT)
					ACMAudioDeviceMute(((high & ECENABLE) ?
						ACM_HEADSET_MIC_EC : ((high & LOOPBACKTEST) ?
						ACM_HEADSET_MIC_LOOP : ACM_HEADSET_MIC)),
						ACM_I2S, low);
				break;
			case HEADPHONE_DEVICE:
				dbg("headphone_mute\n");
				if (high & OUTPUT)
					ACMAudioDeviceMute(ACM_HEADSET_SPEAKER, ACM_I2S, low);
				if (high & INPUT)
					ACMAudioDeviceMute(((high & ECENABLE) ?
						ACM_AUX_MIC_EC : ACM_AUX_MIC),
						ACM_I2S, low);
				break;
			case FORCE_SPEAKER_DEVICE:
				dbg("force_speaker_mute\n");
				ACMAudioDeviceMute(ACM_FORCE_SPEAKER, ACM_I2S, low);
				break;
			default:
				break;
			}
			break;
		case METHOD_VOLUME_SET:
			dbg("\n\nhifi_volume_set\n\n");
			server->clients[CLIENT0]->control.playback_vol = low;
			switch (data.device_type)
			{
			case EARPIECE_DEVICE:
				dbg("bear_volume_set\n");
				if (high & OUTPUT)
					ACMAudioDeviceVolumeSet(ACM_MAIN_SPEAKER, ACM_I2S, low);
				if (high & INPUT)
					ACMAudioDeviceVolumeSet(((high & ECENABLE) ?
						ACM_MIC_EC : ((high & LOOPBACKTEST) ?
						ACM_MIC_LOOP_EARPIECE : ACM_MIC)),
						ACM_I2S, low);
				break;
			case SPEAKER_DEVICE:
				dbg("mono_volume_set\n");
				if (high & OUTPUT)
					ACMAudioDeviceVolumeSet(ACM_AUX_SPEAKER, ACM_I2S, low);
				if (high & INPUT)
					ACMAudioDeviceVolumeSet(((high & ECENABLE) ?
						ACM_AUX_MIC_EC : ((high & LOOPBACKTEST) ?
						ACM_MIC_LOOP_SPEAKER : ACM_AUX_MIC)),
						ACM_I2S, low);
				break;
			case BLUETOOTH_DEVICE:
				dbg("bluetooth_volume_set\n");
				if (high & OUTPUT)
					ACMAudioDeviceVolumeSet(ACM_BLUETOOTH_SPEAKER, ACM_I2S, low);
				if (high & INPUT)
					ACMAudioDeviceVolumeSet(ACM_BLUETOOTH_MIC, ACM_I2S, low);
				break;
			case HEADSET_DEVICE:
				dbg("stereo_volume_set\n");
				if (high & OUTPUT)
					ACMAudioDeviceVolumeSet(ACM_HEADSET_SPEAKER, ACM_I2S, low);
				if (high & INPUT)
					ACMAudioDeviceVolumeSet(((high & ECENABLE) ?
						ACM_HEADSET_MIC_EC : ((high & LOOPBACKTEST) ?
						ACM_HEADSET_MIC_LOOP : ACM_HEADSET_MIC)),
						ACM_I2S, low);
				break;
			case HEADPHONE_DEVICE:
				dbg("headphone_volume_set\n");
				if (high & OUTPUT)
					ACMAudioDeviceVolumeSet(ACM_HEADSET_SPEAKER, ACM_I2S, low);
				if (high & INPUT)
					ACMAudioDeviceVolumeSet(((high & ECENABLE) ?
						ACM_AUX_MIC_EC : ACM_AUX_MIC),
						ACM_I2S, low);
				break;
			case FORCE_SPEAKER_DEVICE:
				dbg("force_speaker_volume_set\n");
				 /* arrive here at volume change (and after the enable) */
					/* FORCE_SPEAKER_DEVICE gets slope+offset for gain IDs 2-3-4
					 * Gain IDs  9-10: write headsetVolumeIdx
					 * Gain IDs 12-13: write speakerVolumeIdx */
				LevanteForceSpeakerGainSettingsData forceSpeakerGainSettingsData;
				forceSpeakerGainSettingsData.headsetVolumeIdx = (UINT8)(data.param & 0x00FF);
				forceSpeakerGainSettingsData.speakerVolumeIdx = (UINT8)((data.param & 0xFF00) >> 8);
				dbg("FORCE_SPEAKER_DEVICE data.param= 0x%x; headsetVolumeIdx= %d, speakerVolumeIdx= %d\n",
					data.param, forceSpeakerGainSettingsData.headsetVolumeIdx, forceSpeakerGainSettingsData.speakerVolumeIdx);
				LevanteAdditionalSettingsSet(FORCE_SPEAKER_GAIN_SETTINGS, (void *)&forceSpeakerGainSettingsData);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		pthread_mutex_unlock(&server->clients[CLIENT0]->mutex);

		send_socket(server->sockfd, &reply, sizeof(int));
	}
	else
	{
		printf("\n\n---------------------------ret is %d, errno is %d------------------------------\n\n", ret, errno);
	}
}


int phone_plugin_pcm_read(audioserver* server)
{
	audioclient * client = server->clients[CLIENT2]; /*FIXME, Raul: temporary we use CLIENT2 for record voice call*/
	int sockfd = make_server_transock(TRANSOCKPORT4READ); /*Raul: use TRANSOCKPORT4READ for record voice call*/

	if (sockfd > 0)
	{
		int new_fd = -1;
		new_fd = accept_socket(sockfd);
		//printf("phone_plugin_pcm_read accept_socket %d\n", new_fd);
		if (new_fd > 0)
		{
			size_t size = m_size; //PTHREAD_STACK_MIN;
			int ret = pthread_attr_init(&client->send_attr);
			ret = pthread_attr_setstacksize(&client->send_attr, size);

			//we need to wait the prior thread to exit
			if (client->sendthread != 0)
			{
				pthread_join(client->sendthread, NULL);
				client->sendthread = 0;
			}

			client->sockfd = new_fd;
			pthread_create(&client->sendthread, &client->send_attr, phone_pcm_read_thread, client);

			/* start flag for send thread */
			pthread_attr_destroy(&client->send_attr);
			client->send_status = 1;

		}
		else
		{
			ERRMSG("Audioserver :phone_plugin_pcm_read :error connect with plugin\n");
			close_socket(new_fd);

		}

		close_socket(sockfd);
	}
	else
	{

		ERRMSG("phone_plugin_pcm_read: Cannot get the socket");
	}

	return 0;
}


int hifi_plugin_pcm_write(audioserver* server)
{
	UNUSEDPARAM(server)
	return 0;
}


int hifi_plugin_pcm_read(audioserver* server)
{
	UNUSEDPARAM(server)
	return 0;
}


void hifi_plugin_ctl_read(audioserver* server)
{
	AUDIOCONTROLINFOS AudioMixerData = {0, 0};

	AudioMixerData.volume = server->clients[CLIENT0]->control.playback_vol;
	AudioMixerData.status = server->clients[CLIENT0]->status;

	send_socket(server->sockfd, &AudioMixerData, sizeof(AudioMixerData));
}


void FM_plugin_ctl_write(audioserver* server)
{
	AUDIOIPCDATA data = INVALIDIPCDATA;
	int reply = 0;
	int ret;
	LevanteChargePumpClockSettingsData chargePumpClockSettingsData;

	if ((ret = recv_socket(server->sockfd, &data,
			       sizeof(data))) == sizeof(data))
	{

	/* [15:0]   Volume
	 * [19:16]   OUTPUT/INPUT
	 * [30:20]  Frequency (in 100 kHz units) */
		int high = (data.param & 0x000F0000) >> 16;
		int low = data.param & 0x0000FFFF;
		unsigned short  fmFreq = (unsigned short)( (data.param & 0xFFF00000) >> 20);

		dbg("---------method_id:%d,device_type:%d,param:%x(volume= %d, fmFreq= %d)\n", data.method_id, data.device_type, data.param, low, fmFreq);
		pthread_mutex_lock(&server->clients[CLIENT0]->mutex);
		switch (data.method_id)
		{
		case METHOD_ENABLE:
			dbg("\n\nFM_enable\n\n");
			server->clients[CLIENT0]->status = AP_ENABLE;
			server->clients[CLIENT0]->control.playback_vol = low;
			switch (data.device_type)
			{
			case EARPIECE_DEVICE:
				dbg("bear_enable\n");
				if (high & OUTPUT)
					ACMAudioDeviceEnable(ACM_MAIN_SPEAKER, ACM_AUX_FM, low);
				if (high & INPUT)
					ACMAudioDeviceEnable(ACM_AUX_MIC, ACM_AUX_FM, low);
				break;
			case SPEAKER_DEVICE:
				dbg("mono_enable\n");
				if (high & OUTPUT)
					ACMAudioDeviceEnable(ACM_AUX_SPEAKER, ACM_AUX_FM, low);
				if (high & INPUT)
					ACMAudioDeviceEnable(ACM_AUX_MIC, ACM_AUX_FM, low);
				break;
			case BLUETOOTH_DEVICE:
				dbg("bluetooth_enable\n");
				if (high & OUTPUT)
					ACMAudioDeviceEnable(ACM_BLUETOOTH_SPEAKER, ACM_AUX_FM, low);
				if (high & INPUT)
					ACMAudioDeviceEnable(ACM_AUX_MIC, ACM_AUX_FM, low);
				break;
			case HEADSET_DEVICE:
			case HEADPHONE_DEVICE:
				dbg("SET FM Radio to HEADSET (fmFreq= %d)", fmFreq);

				chargePumpClockSettingsData.fmFrequency = fmFreq;
				LevanteAdditionalSettingsSet(CHARGE_PUMP_CLOCK_SETTINGS, (void *)&chargePumpClockSettingsData);

				if (high & OUTPUT)
					ACMAudioDeviceEnable(ACM_HEADSET_SPEAKER, ACM_AUX_FM, low);
				if (high & INPUT)
					ACMAudioDeviceEnable(ACM_AUX_MIC, ACM_AUX_FM, low);
				break;
			default:
				break;
			}
			break;
		case METHOD_DISABLE:
			dbg("\n\nFM_disable\n\n");
			server->clients[CLIENT0]->status = AP_DISABLE;
			switch (data.device_type)
			{
			case EARPIECE_DEVICE:
				dbg("bear_disable\n");
				if (high & OUTPUT)
					ACMAudioDeviceDisable(ACM_MAIN_SPEAKER, ACM_AUX_FM);
				if (high & INPUT)
					ACMAudioDeviceDisable(ACM_AUX_MIC, ACM_AUX_FM);
				break;
			case SPEAKER_DEVICE:
				dbg("mono_disable\n");
				if (high & OUTPUT)
					ACMAudioDeviceDisable(ACM_AUX_SPEAKER, ACM_AUX_FM);
				if (high & INPUT)
					ACMAudioDeviceDisable(ACM_AUX_MIC, ACM_AUX_FM);
				break;
			case BLUETOOTH_DEVICE:
				dbg("bluetooth_disable\n");
				if (high & OUTPUT)
					ACMAudioDeviceDisable(ACM_BLUETOOTH_SPEAKER, ACM_AUX_FM);
				if (high & INPUT)
					ACMAudioDeviceDisable(ACM_AUX_MIC, ACM_AUX_FM);
				break;
			case HEADSET_DEVICE:
			case HEADPHONE_DEVICE:
				dbg("stereo_disable\n");
				if (high & OUTPUT)
					ACMAudioDeviceDisable(ACM_HEADSET_SPEAKER, ACM_AUX_FM);
				if (high & INPUT)
					ACMAudioDeviceDisable(ACM_AUX_MIC, ACM_AUX_FM);

				chargePumpClockSettingsData.fmFrequency = 0;
				LevanteAdditionalSettingsSet(CHARGE_PUMP_CLOCK_SETTINGS, (void *)&chargePumpClockSettingsData);

				break;
			default:
				break;
			}
			break;
		case METHOD_MUTE:
			dbg("\n\nFM_mute\n\n");
			server->clients[CLIENT0]->control.playback_vol = 0;
			switch (data.device_type)
			{
			case EARPIECE_DEVICE:
				dbg("bear_mute\n");
				if (high & OUTPUT)
					ACMAudioDeviceMute(ACM_MAIN_SPEAKER, ACM_AUX_FM, low);
				if (high & INPUT)
					ACMAudioDeviceMute(ACM_AUX_MIC, ACM_AUX_FM, low);
				break;
			case SPEAKER_DEVICE:
				dbg("mono_mute\n");
				if (high & OUTPUT)
					ACMAudioDeviceMute(ACM_AUX_SPEAKER, ACM_AUX_FM, low);
				if (high & INPUT)
					ACMAudioDeviceMute(ACM_AUX_MIC, ACM_AUX_FM, low);
				break;
			case BLUETOOTH_DEVICE:
				dbg("bluetooth_mute\n");
				if (high & OUTPUT)
					ACMAudioDeviceMute(ACM_BLUETOOTH_SPEAKER, ACM_AUX_FM, low);
				if (high & INPUT)
					ACMAudioDeviceMute(ACM_AUX_MIC, ACM_AUX_FM, low);
				break;
			case HEADSET_DEVICE:
			case HEADPHONE_DEVICE:
				dbg("stereo_mute\n");
				if (high & OUTPUT)
					ACMAudioDeviceMute(ACM_HEADSET_SPEAKER, ACM_AUX_FM, low);
				if (high & INPUT)
					ACMAudioDeviceMute(ACM_AUX_MIC, ACM_AUX_FM, low);
				break;
			default:
				break;
			}
			break;
		case METHOD_VOLUME_SET:
			dbg("\n\nFM_volume_set\n\n");
			server->clients[CLIENT0]->control.playback_vol = low;
			switch (data.device_type)
			{
			case EARPIECE_DEVICE:
				dbg("bear_volume_set\n");
				if (high & OUTPUT)
					ACMAudioDeviceVolumeSet(ACM_MAIN_SPEAKER, ACM_AUX_FM, low);
				if (high & INPUT)
					ACMAudioDeviceVolumeSet(ACM_AUX_MIC, ACM_AUX_FM, low);
				break;
			case SPEAKER_DEVICE:
				dbg("mono_volume_set\n");
				if (high & OUTPUT)
					ACMAudioDeviceVolumeSet(ACM_AUX_SPEAKER, ACM_AUX_FM, low);
				if (high & INPUT)
					ACMAudioDeviceVolumeSet(ACM_AUX_MIC, ACM_AUX_FM, low);
				break;
			case BLUETOOTH_DEVICE:
				dbg("bluetooth_volume_set\n");
				if (high & OUTPUT)
					ACMAudioDeviceVolumeSet(ACM_BLUETOOTH_SPEAKER, ACM_AUX_FM, low);
				if (high & INPUT)
					ACMAudioDeviceVolumeSet(ACM_AUX_MIC, ACM_AUX_FM, low);
				break;
			case HEADSET_DEVICE:
			case HEADPHONE_DEVICE:
				dbg("stereo_volume_set\n");
				if (high & OUTPUT)
					ACMAudioDeviceVolumeSet(ACM_HEADSET_SPEAKER, ACM_AUX_FM, low);
				if (high & INPUT)
					ACMAudioDeviceVolumeSet(ACM_AUX_MIC, ACM_AUX_FM, low);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		pthread_mutex_unlock(&server->clients[CLIENT0]->mutex);

		send_socket(server->sockfd, &reply, sizeof(int));
	}
	else
	{
		printf("\n\n---------------------------ret is %d, errno is %d------------------------------\n\n", ret, errno);
	}
}


void FM_plugin_ctl_read(audioserver* server)
{
	AUDIOCONTROLINFOS AudioMixerData = {0, 0};

	AudioMixerData.volume = server->clients[CLIENT0]->control.playback_vol;
	AudioMixerData.status = server->clients[CLIENT0]->status;

	send_socket(server->sockfd, &AudioMixerData, sizeof(AudioMixerData));
}


int FM_plugin_pcm_write(audioserver* server)
{
	UNUSEDPARAM(server)
	return 0;
}


int FM_plugin_pcm_read(audioserver* server)
{
	UNUSEDPARAM(server)
	return 0;
}


static int get_calibration_request_num(unsigned short num)
{
	int i, j;

	for (i = 0, j = 0; i < 16; i++)
	{
		if (num & 0x1)
			j++;
		num /= 2;
	}

	return j;
}

void *calibration_send_reports(void *param)
{
	calibrationParam *pCaliParam = (calibrationParam *)param;
	int socket_id = pCaliParam->sock;
	int loopNum = pCaliParam->reportNumber;
	free(param);
	char dataBuf[MSASETTINGDATALEN];

	OS_STATUS status;
	StubMsg msgq;

	//FIXME: we will enter calibration mode only one time, so we needn't consider the case to exit the thread?
	while (loopNum--)
	{
		//FIXME: We know the reports number by parsing the request data, but if we cannot get all the reports
		//in 1 second, we will exit also.
		status = OSAMsgQRecv(msgQ4Calibration, (UINT8 *)&msgq,  sizeof(StubMsg), MSGQ_TIMEOUT);
		if (status == OS_SUCCESS)
		{
			memcpy(dataBuf, msgq.pData, MSASETTINGDATALEN);
			free(msgq.pData);
			DPRINTF("calibration_send_reports: got report from CP\n");
		}
		else if (status == OS_TIMEOUT)
		{
			DPRINTF("calibration_send_reports: got report from CP TIMEOUT, EXIT!!\n");
			close_socket(socket_id);
			return 0;
		}
		else if (status == OS_QUEUE_EMPTY)
		{
			DPRINTF("calibration_send_reports: got report from CP OS_QUEUE_EMPTY, EXIT!!\n");
			close_socket(socket_id);
			return 0;
		}
		else
		{
			DPRINTF("Shouldn't be here: calibration_send_reports: status is %d\n", status);
			close_socket(socket_id);
			return 0;
		}

		if (send_socket(socket_id, dataBuf, MSASETTINGDATALEN) > 0)
		{
			DPRINTF("calibration_send_reports: sent the data to caller!\n");
		}
		else
		{
			DPRINTF("calibration_send_reports: send error, errno %d, sockfd %d\n", errno, socket_id);
			close_socket(socket_id);
		}
	}

	close_socket(socket_id);
	printf("## calibration_send_reports thread exit! ##\n");

	return 0;
}


void calibration_plugin_ctl_write(audioserver* server)
{
	ACM_ReturnCode ret;
	int reply = 0;

	int sockfd = make_server_transock(TRANSOCKPORT4CALIBRATE); /*Raul: use TRANSOCKPORT4CALIBRATE for calibration*/

	if (sockfd > 0)
	{
		int new_fd = -1;
		new_fd = accept_socket(sockfd);

		if (new_fd > 0)
		{
			char dataBuf[MSASETTINGDATALEN];
			if (recv_socket(new_fd, dataBuf, MSASETTINGDATALEN) > 0)
			{
				pthread_mutex_lock(&server->clients[CLIENT0]->mutex);
				//First, get the calibration data from caller
				ret = ACMAudioSetMSASettings(dataBuf);
				pthread_mutex_unlock(&server->clients[CLIENT0]->mutex);

				//If OK and the opcode is to get report, create the thread to send the response data to caller
				if (ret == ACM_RC_OK && dataBuf[0] == GETREPORTOPCODE)
				{
					calibrationParam *pparam = malloc(sizeof(calibrationParam));
					pparam->reportNumber = get_calibration_request_num(*(unsigned short*)(dataBuf + 4));
					pparam->sock = new_fd;
					printf("request reportsNumber is %d\n", pparam->reportNumber);
					pthread_create(&server->thread_calibration, NULL,
						       calibration_send_reports, (void *)pparam);
				}
			}
			else
			{
				DPRINTF("AUDIO_SERVER : calibration_plugin_ctl_write recv error, errno %d, sockfd %d\n", errno, new_fd);
				close_socket(new_fd);
			}
		}
		else
		{
			ERRMSG("Audioserver :calibration_plugin_ctl_write :error connect\n");
			close_socket(new_fd);

		}
		close_socket(new_fd);
		close_socket(sockfd);
		send_socket(server->sockfd, &reply, sizeof(int));
	}
	else
	{

		ERRMSG("phone_plugin_pcm_read: Cannot get the socket");
	}
}


// This function handles a client
void *plugin_client_handler(void *param)
{
	int ret;
	AUDIOIPCHEADER header = INVALIDIPCHEADER;
	audioserver* lpServer = (audioserver*)param;

	pthread_mutex_lock(&lpServer->mutex);

	// We should not terminate the process if clients are still running
	iThreadsRunning++;

	setup_socket(lpServer->sockfd);

	// Receive type of client
	ret = recv_socket(lpServer->sockfd, &header, sizeof(header));
	if (ret != sizeof(header))
		printf("\n\n++++++++ ret is %d, errno is %d, lpServer->sockfd is %d+++++++++++\n\n", ret, errno, lpServer->sockfd);

	dbg("Audioserver: client_type = %d, method_type = %d\n", header.client_type, header.method_type);
	if (header.client_type == PHONE_CLIENT )
	{

		switch (header.method_type)
		{
		case PLUGIN_CTL_WRITE:
			phone_plugin_ctl_write(lpServer);
			break;

		case PLUGIN_CTL_READ:
			phone_plugin_ctl_read(lpServer);
			break;

		case PLUGIN_PCM_WRITE:
			phone_plugin_pcm_write(lpServer);
			break;

		case PLUGIN_PCM_READ:
			phone_plugin_pcm_read(lpServer);
			break;

		default:
			ERRMSG("Audioserver: error input type = %d\n", header.method_type);
			break;

		}
	}
	else if (header.client_type == HIFI_CLIENT )
	{
		switch (header.method_type)
		{
		case PLUGIN_CTL_WRITE:
			hifi_plugin_ctl_write(lpServer);
			break;

		case PLUGIN_CTL_READ:
			hifi_plugin_ctl_read(lpServer);
			break;

		case PLUGIN_PCM_WRITE:
			hifi_plugin_pcm_write(lpServer);
			break;

		case PLUGIN_PCM_READ:
			hifi_plugin_pcm_read(lpServer);
			break;

		default:
			DPRINTF("Audioserver: error input type\n");
			break;
		}
	}
	else if (header.client_type == FMRADIO_CLINET )
	{
		switch (header.method_type)
		{
		case PLUGIN_CTL_WRITE:
			FM_plugin_ctl_write(lpServer);
			break;

		case PLUGIN_CTL_READ:
			FM_plugin_ctl_read(lpServer);
			break;

		case PLUGIN_PCM_WRITE:
			FM_plugin_pcm_write(lpServer);
			break;

		case PLUGIN_PCM_READ:
			FM_plugin_pcm_read(lpServer);
			break;

		default:
			DPRINTF("Audioserver: error input type\n");
			break;
		}
	}
	else if (header.client_type == CALIBRATION_CLIENT )
	{
		switch (header.method_type)
		{
		case PLUGIN_CALIBRATE_MSA:
			calibration_plugin_ctl_write(lpServer);
			break;

		default:
			DPRINTF("Audioserver: error input type\n");
			break;
		}
	}

	// Close socket
	close_socket(lpServer->sockfd);


	// Decrease thread count
	iThreadsRunning--;

	//audio_set_client(lpServer);
	pthread_mutex_unlock(&lpServer->mutex);
	return 0;
}


// server processing loop
int socket_loop(audioserver* server)
{
	while (run_level >= RUN_LEVEL6)
	{
		// Master socket
		int sockfd = make_server_mainsock();

		if (sockfd > 0)
		{
			// Set pthread stack size to decrease unused memory usage
			pthread_attr_t tattr;
			size_t size = m_size; //PTHREAD_STACK_MIN;
			int ret = pthread_attr_init(&tattr);
			ret = pthread_attr_setstacksize(&tattr, size);

			if (b_test_mode)
			{
				dbg("$$$$$$$$$$$$ test mode, Init to enable HIFI headset path! $$$$$$$$$$\n");
				ACMAudioDeviceDisable(ACM_MAIN_SPEAKER, ACM_I2S);
				ACMAudioDeviceDisable(ACM_MIC, ACM_I2S);
				ACMAudioDeviceEnable(ACM_HEADSET_SPEAKER, ACM_I2S, 70);
				ACMAudioDeviceEnable(ACM_HEADSET_MIC, ACM_I2S, 70);
			}

			while (run_level >= RUN_LEVEL6)
			{
				FILE *fd = fopen("/tmp/audioserver_ok", "w");
				if (fd)
					fclose(fd);

				int new_fd = -1;
				new_fd = accept_socket(sockfd);

				// Handle connection if it is not the final dummy client
				if ((run_level >= RUN_LEVEL6) && new_fd > 0)
				{
					server->sockfd = new_fd;
					plugin_client_handler(server);
				}
				else if (new_fd > 0)
				{
					close_socket(new_fd);
				}
				usleep(10000);
			}

			close_socket(sockfd);

			// Very minor race condition here
			// No dramatic consequences
			// But we Must wait all client termination
			// We will pthread_join one day
			int icount = 0;
			while (iThreadsRunning > 0 /*&& icount < 30*/)
			{
				icount++;
				sleep(1);
			}

			// Free informations on the device
			pthread_attr_destroy(&tattr);
		}
		else
		{
			perror("audioserver: Cannot get the socket");
		}

		sleep(1);
	}

	return 0;
}

struct inotify_event {
	__s32 wd;                       /* watch descriptor */
	__u32 mask;                     /* watch mask */
	__u32 cookie;                   /* cookie to synchronize two events */
	__u32 len;                      /* length (including nulls) of name */
	char name[0];                   /* stub for possible name */
};

#define MAX_BUF_SIZE  1024
#define EVENT_NUM     16
#ifndef __NR_inotify_init
#define __NR_inotify_init (__NR_SYSCALL_BASE + 316)
#endif
#ifndef __NR_inotify_add_watch
#define __NR_inotify_add_watch (__NR_SYSCALL_BASE + 317)
#endif
#ifndef __NR_inotify_rm_watch
#define __NR_inotify_rm_watch (__NR_SYSCALL_BASE + 318)
#endif

/* the following are legal, implemented events that user-space can watch for */
#define IN_ACCESS               0x00000001      /* File was accessed */
#define IN_MODIFY               0x00000002      /* File was modified */
#define IN_ATTRIB               0x00000004      /* Metadata changed */
#define IN_CLOSE_WRITE          0x00000008      /* Writtable file was closed */
#define IN_CLOSE_NOWRITE        0x00000010      /* Unwrittable file closed */
#define IN_OPEN                 0x00000020      /* File was opened */
#define IN_MOVED_FROM           0x00000040      /* File was moved from X */
#define IN_MOVED_TO             0x00000080      /* File was moved to Y */
#define IN_CREATE               0x00000100      /* Subfile was created */
#define IN_DELETE               0x00000200      /* Subfile was deleted */
#define IN_DELETE_SELF          0x00000400      /* Self was deleted */
#define IN_MOVE_SELF            0x00000800      /* Self was moved */


#define IN_ALL_EVENTS   (IN_ACCESS | IN_MODIFY | IN_ATTRIB | IN_CLOSE_WRITE | \
			 IN_CLOSE_NOWRITE | IN_OPEN | IN_MOVED_FROM | \
			 IN_MOVED_TO | IN_DELETE | IN_CREATE | IN_DELETE_SELF |	\
			 IN_MOVE_SELF)


static inline int inotify_init (void)
{
	return syscall(__NR_inotify_init);
}

static inline int inotify_add_watch (int fd, const char *name, uint32_t mask)
{
	return syscall(__NR_inotify_add_watch, fd, name, mask);
}

static inline int inotify_rm_watch (int fd, uint32_t wd)
{
	return syscall(__NR_inotify_rm_watch, fd, wd);
}

void *detect_thread_handler(void *param)
{
	audioserver* server = (audioserver *)param;

	int fd;
	int wd;
	char buffer[1024];
	char * offset = NULL;
	struct inotify_event * event;
	int len, tmp_len;
	int f_read = 0;

#if !defined (BIONIC)
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
#endif

	dbg("audio_set_client for first time\n");
	read_runtime_info(g_srcfilename, server);
	audio_set_client(server);

	dbg("inotify_init()\n");
	fd = inotify_init();
	if (fd < 0)
	{
		ERRMSG("Fail to initialize inotify.\n");
		exit(-1);
	}

	dbg("inotify_add_watch()\n");
	#if !defined (BIONIC)
	wd = inotify_add_watch(fd, "/root/.audiorc", IN_MODIFY );
	#else
	wd = inotify_add_watch(fd, "/marvell/root/.audiorc", IN_MODIFY );
	#endif
	if (wd < 0)
	{
		ERRMSG("Can't add watch for /root/.audiorc.\n");
		exit(-1);
	}

	dbg("do while......\n");

	while ((len = read(fd, buffer, MAX_BUF_SIZE)))
	{

		offset = buffer;
		dbg("sizeof(inotify_event) = %d\n",  sizeof(struct inotify_event));
		dbg("Some event happens, len = %d.\n", len);
		event = (struct inotify_event *)buffer;

		while (((char *)event - buffer) < len)
		{
			dbg("Event mask: %08X\n", event->mask);

			if ( event->mask & IN_MODIFY)
			{
				f_read = 1;
			}

			dbg("event->len =%d\n", event->len);
			tmp_len = sizeof(struct inotify_event) + event->len;
			dbg("tmp_len =%d\n", tmp_len);
			event = (struct inotify_event *)(offset + tmp_len);
			offset += tmp_len;
		}

		if (f_read != 0)
		{
			DPRINTF("read_runtime_info\n");
			read_runtime_info(g_srcfilename, server);
			audio_set_client(server);
			f_read = 0;
		}
	}

	return 0;

}
int dev_detect_loop(audioserver *server)
{
	//Raul, in Android, we donot need detect loop, but in Generic, we need it to do the init
#if defined (BIONIC)
	return 0;
#endif
	/* detect_loop recovery level = 5 */
	dbg("detect_loop\n");
	if (run_level >= RUN_LEVEL5)
	{
		dbg("detect_loop startup\n");
		int ret = pthread_attr_init(&m_attr);
		ret = pthread_attr_setstacksize(&m_attr, m_size);
		pthread_create(&server->clients[CLIENT0]->sendthread, NULL, detect_thread_handler, server);
		pthread_attr_destroy(&m_attr);
	}
	else
	{
		dbg("detect_loop cleanup\n");
	#if !defined (BIONIC)
		pthread_cancel(&server->clients[CLIENT0]->sendthread);
	#endif
	}

	return 0;
}


int main_loop(audioserver* server)
{
	/* main_loop recovery level = 1 */
	int i;

	server->clinum = 3;
	get_config_filename(g_srcfilename, sizeof(g_srcfilename));

	if ( run_level > RUN_LEVEL1)
	{
		dbg("main_loop startup, g_srcfilename is %s\n", g_srcfilename);
		for (i = 0; i < server->clinum; i++)
		{
			server->clients[i] = audiodevicenew("");
			pthread_mutex_lock(&server->clients[i]->mutex);
			read_client_info(g_srcfilename, i, server->clients[i]);
			server->clients[i]->status = 0;
			server->clients[i]->laststatus = 0;
			pthread_mutex_unlock(&server->clients[i]->mutex);
		}

		server->bt_addr = (char *)malloc(256);
	}
	else
	{
		dbg("main_loop cleanup\n");
		for (i = 0; i < server->clinum; i++)
		{
			audiodevicefree(server->clients[i]);
		}

		free(server->bt_addr);
	}

	return 0;
}

int ap_audio_loop(audioserver * server)
{
	/* ap_init_loop recovery level = 2 */
//	dbg("ap_init_loop\n");
	if (run_level > RUN_LEVEL2)
	{
		/* do ap audio init here */
		dbg("ap_init_loop startup\n");
		server->curclient = CLIENT0;
		server->lastclient = CLIENT_INVALID;
		server->status = AP_ENABLE;
	}
	else if (run_level == RUN_LEVEL2)
	{
		//dbg("do ap_audio_setting\n");
		server->curclient = CLIENT0;
		server->lastclient = CLIENT0;
		server->status = AP_ENABLE;
	}
	else
	{
		/* do ap audio cleanup here */
		dbg("ap_init_loop cleanup\n");

		server->curclient = CLIENT0;
		server->lastclient = CLIENT_INVALID;
		server->status = STATUS_INIT;
	}
	return 0;
}

extern int Deinit_callback_manager();
int first_time = 0;
int cp_audio_loop(audioserver * server)
{
	UNUSEDPARAM(server)
	/* acm_loop recovery level = 3 */
	dbg("cp_audio_loop\n");
	if ( run_level >= RUN_LEVEL3)
	{
#if 0
		dbg("cp_audio_loop startup\n");

		while (NVMIPCClient_InitClient(NVM_IPC_ACM) == FALSE)
			sleep(1);
		DPRINTF("NVMIPCClient: %s inited\n", NVM_IPC_ACM);
#endif

		ACMInit();

	}
	else
	{
		dbg("cp_audio_loop cleanup\n");

		/* Do cp audio deinit */
		ACMDeinit();
#if 0
		int seh_fd;
		dbg("send audioserver deinit finish msg to eeh\n");
		seh_fd = open("/dev/seh", O_RDWR | O_SYNC);
		if (seh_fd != -1)
		{
			ioctl(seh_fd, SEH_IOCTL_AUDIOSERVER_REL_COMP, NULL);
			close(seh_fd);
		}

#endif
	}
	return 0;
}

int diag_loop(audioserver * server)
{
	UNUSEDPARAM(server)
#if defined(ENABLE_DIAG)
	/* diag_loop revocery level 4 */
	dbg("diag_loop\n");
	if (run_level >= RUN_LEVEL4)
	{
		dbg("diag_loop startup\n");
		if (first_time == 0)
		{
			if (b_launch_diag == 1)
			{
				diagPhase1Init();
				diagPhase2Init();
			}
		}
		else
		{
			if (b_launch_diag == 1)
				diagExtIFstatusConnectNotify();
		}
		first_time++;
	}
	else
	{
		dbg("diag_loop_cleanup\n");
		if (b_launch_diag == 1)
			diagExtIFStatusDiscNotify();
	}
#endif
	return 0;
}

void print_usage()
{
	printf( "audioserver  --  Audio Server.  Call with audioserver [inputs [output]]\n");
	printf( "CopyRight (c) 2008 Marvell \n");
	printf( "Usage: audioserver -D yes/no -d loglevel -f logfile \n");
	printf( "Options:\n");
	printf( "    -D        By default, audioserver will not enable Diag;\n");
	printf( "    -h      Print this message\n");
	printf( "    -L      debug level (same as log level:0-7, default: 5) \n");
	printf( "    -f      Specify a log file to log to\n");
}


void handle_arg(int argc, char * argv[] )
{
	int i;
	char * cp, opt;

	for (i = 1; i < argc; i++)
	{
		cp = argv[i];
		if (*cp == '-' || *cp == '/')
		{
			opt = *(++cp);

			switch (opt)
			{
			case 'D':
			{

				if ( (i + 1)  < argc )
				{
					i++;
					if (strcmp("yes", argv[i]) == 0)
					{
						b_launch_diag = 1;
					}
				}
				else
				{
					DPRINTF("Not enough argument.\n");
					exit(-1);
				}
				break;
			}
			case 't':
			case 'T':
			{

				if ( (i + 1)  < argc )
				{
					i++;
					if (strcmp("yes", argv[i]) == 0)
					{
						b_test_mode = 1;
					}
				}
				else
				{
					DPRINTF("Not enough argument.\n");
					exit(-1);
				}
				break;
			}
			case 'L':
			case 'l':
			{
				if ( (i + 1)  < argc )
				{
					i++;
					loglevel = atoi( argv[i]);
				}
				else
				{
					DPRINTF("Not enough argument.\n");
					exit(-1);
				}
				break;
			}
			case 'f':
			case 'F':
			{
				char filename[255];
				if ( (i + 1)  < argc )
				{
					i++;
					sprintf(filename, "%s", argv[i]);
					logfile = fopen(filename, "w");
					if (logfile == NULL)
					{
						DPRINTF( "can't open logfile `%s'\n", filename);
						exit(-1);
					}
				}
				else
				{
					DPRINTF("Not enough argument.\n");
					exit(-1);
				}
				break;
			}

			case '?':
			case 'H':
			case 'U':
			case 'h':
			case 'u':
				print_usage();
				exit(0);
			default:
				DPRINTF("Invalid argument %s.\n", cp);
				exit(-1);
			}       // end switch
		}               // end if
	}                       // end for
}

void sigint_handler(int sig)
{
	UNUSEDPARAM(sig)

	run_level = RUN_MAX;
	dbg("handling SIGINT to recovery\n");

}

void sigterm_handler(int sig)
{
	UNUSEDPARAM(sig)

	if (run_level < RUN_MAX )
	{
		dbg("handling SIGINT again: exit forced\n");
	}
	else
	{
		run_level = RUN_LEVEL2;
		close_socket(make_client_mainsock());
		dbg("handling SIGTERM\n");

	}
}

#if defined (BIONIC)
void __noreturn sig_action(int signum, siginfo_t *info, void *p)
#else
void sig_action(int signum, siginfo_t *info, void *p)
#endif
{
	DPRINTF( "as[%d]%s(%d, %p, %p)\n", getpid(), __func__, signum, info, p);
	exit(-1);
}

#if defined (BIONIC)
/*
 *  Switches UID to system, preserving CAP_NET_RAW capabilities.
 *
 */
static void switchUser()
{
	prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0);
	setuid(AID_SYSTEM);

	struct __user_cap_header_struct header;
	struct __user_cap_data_struct cap;
	header.version = _LINUX_CAPABILITY_VERSION;
	header.pid = 0;
	cap.effective = cap.permitted = 1 << CAP_NET_RAW;
	cap.inheritable = 0;
	capset(&header, &cap);
}
#endif

#if defined(BIONIC)
extern int init_stdfiles(void);
#endif
/* main function */
int main(int argc, char *argv[])
{
	int(*loop[RUN_MAX + 1]) ( audioserver*);

#if defined (BIONIC)
	if(InProduction_Mode() != TRUE)
		switchUser();
#endif

	#if defined (BIONIC)
	init_stdfiles();
	#endif
	handle_arg(argc, argv);
	DPRINTF("\n\nAUDIO_SERVER : running\n\n");

#if defined (USE_SPEAKER_AS_EARPIECE)
	printf("************** We will use speakser as earpiece in voice call! ****************\n");
#endif

#if 0
	struct sigaction act;
	int i;

	/* set up the handler */
	act.sa_sigaction = sig_action;
	act.sa_flags = SA_SIGINFO;
	sigemptyset(&act.sa_mask);
	for (i = 1; i <= 31; i++)
		if (i != SIGALRM && i != SIGCHLD)
			sigaction(i, &act, NULL);
#endif

	signal(SIGINT, sigint_handler);
	signal(SIGTERM, sigterm_handler);

	/* Init OSA there */
	OSAInitialize();
	diag_init();

	audioserver* server = mymalloc(sizeof(audioserver));
	if (server == NULL )
	{
		ERRMSG("audioserver init failed\n");
		return 0;
	}



	pthread_mutex_init(&server->mutex, NULL);

	loop[RUN_LEVEL1] = &main_loop;
	loop[RUN_LEVEL2] = &ap_audio_loop;
	loop[RUN_LEVEL3] = &cp_audio_loop;
	loop[RUN_LEVEL4] = &diag_loop;
	loop[RUN_LEVEL5] = &dev_detect_loop;
	loop[RUN_LEVEL6] = &socket_loop;
	loop[RUN_MAX] = NULL;

	pthread_mutex_init(&(ringrecv.ring_mutex), NULL);

	while ( level <= RUN_MAX)
	{
		if (run_level == RUN_MAX)
		{
			if (loop[level] != NULL)
			{
				dbg("level=%d\n", level);
				while ((loop[level])(server) != 0) ;
				level++;

				if (level == RUN_MAX)
				{
					dbg("level = RUN_MAX\n");
					for (level = RUN_MAX - 1; level > run_level; level-- )
						(loop[level])(server);
				}
			}
		}
		else
		{
			//dbg("run loop[%d]\n",run_level);
			(loop[run_level])(server);
			sleep(1);
		}

	}

	pthread_mutex_destroy(&server->mutex);
	pthread_mutex_destroy(&(ringrecv.ring_mutex));
	safefree(server);
	pthread_attr_destroy(&m_attr);

	return 0;
}

