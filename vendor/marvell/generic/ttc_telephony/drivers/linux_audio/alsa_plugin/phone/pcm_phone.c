/*
 * ALSA <-> Phone PCM I/O plugin
 *
 * Copyright (c) 2007 by Paul Shen <bshen9@marvell.com>
 *
 */

#include <stdio.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <pthread.h>

#include <alsa/asoundlib.h>
#include <alsa/pcm_external.h>

#ifdef BIONIC
#include <utils/Log.h>
#define LOG_TAG "ALSA_PLUGIN--PHONE"
#else
#define LOGE(format, arg ...) printf(format, ##arg)
#define LOGI(format, arg ...) printf(format, ##arg)
#endif

#include "../../audio_ringbuf.h"
#include "../../audio_ipc.h"
#include "../../audio_protocol.h"
#include "../../audio_timer.h"

#define  DOMAIN_SOCKET_NAME "/data/misc/phone/phone_plugin"
#define  VT_RECORD_START 1
#define  VT_RECORD_STOP 0
#define  VT_RECORD_CMD_TYPE 1

/* debug */
#define PCM_DEBUG 0
#if PCM_DEBUG
#define dbg(format, arg ...) printf("pcm_phone: " format "\n", ## arg)
#else
#define dbg(format, arg ...)
#endif

#define UNUSEDPARAM(param) (void)(param);

#define PHONE_PACKET_LEN          320           /* 160 smaples */
#define PCM_SAMPLE_SIZE           2             /* 16 bit */
#define SAMPLE_RATE               8000          /* 8khz sample rate */

#define PHONE_DEBUG 0
#define PHONE_DEBUG_PRINT(format, arg ...) if(PHONE_DEBUG) LOGE(format, ##arg)

typedef struct ipc_packet {
	unsigned char type;
} ipc_packet_t;


typedef struct phone_packet {
	unsigned char pcm_data[PHONE_PACKET_LEN];
} phone_packet_t;

typedef struct snd_pcm_phone {
	snd_pcm_ioplug_t io;
	snd_pcm_sframes_t hw_ptr;
	phone_packet_t pkt;
	unsigned int pcm_data_count;
	int sk_read;
	int sk_write;
	int wr_rd_flag;
	unsigned int rate;
	unsigned int channels;
	unsigned int frame_bytes;
	long playback_which_end;/*1 near_end, 2 far_end, 0 both_end */
	long capture_which_end;/*1 near_end, 2 far_end, 0 both_end */
	long combine_with_call;/*0 not combine with call, 1 combine with call*/
} snd_pcm_phone_t;

//global variabls
typedef struct{
    pthread_mutex_t*         pMutexLock;
    int                      refCnt;
    int                      pipefd[2];
    int                      hSocketThread;
    int                      sk_record;
	ringqueue                ringinfo;
}vp_handle_t;

static vp_handle_t*         g_vpHandle=NULL;

static void send_command(AUDIOIPCHEADER *header, AUDIOIPCDATA *data)
{
	int sockfd = make_client_mainsock();
	int reply;
	int ret;

	if (send_socket(sockfd, header, sizeof(AUDIOIPCHEADER)) == sizeof(AUDIOIPCHEADER))
	{
		if (send_socket(sockfd, data, sizeof(AUDIOIPCDATA)) == sizeof(AUDIOIPCDATA))
		{
			if ((ret = recv_socket(sockfd, &reply, sizeof(int))) != sizeof(int))
			{
				LOGE("************Unable to read reply from audio server, ret is %d, errno is %d\n", ret, errno);
			}
		}
		else
		{
			LOGE("Unable to send data to audio server\n");
		}
	}
	else
	{
		LOGE("Unable to set new value to audio server\n");
	}
	close_socket(sockfd);
}

static void send_ipc_data(int client, int method_type, int method_id, int device, int param)
{
	AUDIOIPCHEADER header = INVALIDIPCHEADER;
	AUDIOIPCDATA AudioIpcData = INVALIDIPCDATA;

	header.client_type = client;
	header.method_type = method_type;

	AudioIpcData.device_type = device;
	AudioIpcData.method_id = method_id;
	AudioIpcData.param = param;
	send_command(&header, &AudioIpcData);
}


static int phone_disconnect(snd_pcm_ioplug_t * io)
{
	snd_pcm_phone_t *pcm = io->private_data;

	if (io->stream == SND_PCM_STREAM_PLAYBACK)
	{
		close_socket(pcm->sk_write);
	}
	else
	{
		close_socket(pcm->sk_read);
	}
	return 0;
}



static int phone_connect(snd_pcm_ioplug_t *io)
{
	snd_pcm_phone_t *pcm = io->private_data;
	SOCKETDATA socketdata = INVALIDSOCKETDATA;

	int ret = 0;
	int retry_count = 10;


	int sockfd = make_client_mainsock();
	if (sockfd > 0)
	{
		if (pcm->wr_rd_flag == SND_PCM_STREAM_PLAYBACK)
		{
			socketdata.client_type = PHONE_CLIENT;
			socketdata.method_type = PLUGIN_PCM_WRITE;
		}
		else
		{
			socketdata.client_type = PHONE_CLIENT;
			socketdata.method_type = PLUGIN_PCM_READ;
		}

		if (send_socket(sockfd, &socketdata, sizeof(socketdata)) == sizeof(socketdata))
		{
			//FIXME: we need wait audio server to listen the port
			int newfd;
			if (pcm->wr_rd_flag == SND_PCM_STREAM_PLAYBACK)
			{
				while (retry_count--)
				{
					if ((newfd = make_client_transock(TRANSOCKPORT4WRITE)) > 0)
					{
						break;
					}

					LOGE("make_client_transock failed, try again\n");
					usleep(10000);
				}
			}
			else
			{
				while (retry_count--)
				{
					if ((newfd = make_client_transock(TRANSOCKPORT4READ)) > 0)
					{
						break;
					}

					LOGE("make_client_transock failed, try again\n");
					usleep(10000);
				}
			}
			if (newfd > 0)
			{
				if (pcm->wr_rd_flag == SND_PCM_STREAM_PLAYBACK)
				{
					pcm->sk_write = newfd;
				}
				else
				{
					pcm->sk_read = newfd;
				}
			}
			else
			{
				LOGE("ALSA PLUGIN : can not establish tranfer socket with server\n");
				ret = 3;
			}


		}
		else
		{
			LOGE("ALSA PLUGIN : Connected Audioserver failed\n");
			close_socket(sockfd);
			ret = 2;
		}
		close_socket(sockfd);
	}
	else
	{
		ret = 1;
		LOGE("ALSA PLUGIN : Socket failed phone client\n");
	}

	return ret;
}

static inline snd_pcm_phone_t *phone_alloc(void)
{
	snd_pcm_phone_t *pcm;

	pcm = malloc(sizeof(*pcm));
	if (pcm)
	{
		memset(pcm, 0, sizeof(*pcm));
		pcm->sk_read = -1;
		pcm->sk_write = -1;
	}
	return pcm;
}

static inline void phone_free(snd_pcm_phone_t * pcm)
{
	free(pcm);
}



static int phone_start(snd_pcm_ioplug_t *io)
{
	UNUSEDPARAM(io)
	return 0;
}

static int phone_stop(snd_pcm_ioplug_t *io)
{
	snd_pcm_phone_t *pcm = io->private_data;

	PHONE_DEBUG_PRINT("phone stop \n");

	phone_disconnect(io);

	switch (io->stream)
	{
		case SND_PCM_STREAM_PLAYBACK:
			pcm->sk_write = -1;

			send_ipc_data(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE,
					0, (PCMSTREAMPLAYBACK << 16));
			break;

		case SND_PCM_STREAM_CAPTURE:
			pcm->sk_read = -1;

			send_ipc_data(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE,
					0, (PCMSTREAMCAPTURE << 16));
			break;

		default:
			LOGE("Unexpected ioplug received !!\n");
			return -EINVAL;
	}

	PHONE_DEBUG_PRINT("phone stop done\n");
	return 0;
}

int phone_drain(snd_pcm_ioplug_t *io)
{
	UNUSEDPARAM(io)
	return 0;
}

static snd_pcm_sframes_t phone_pointer(snd_pcm_ioplug_t *io)
{
	snd_pcm_phone_t *pcm = io->private_data;
	unsigned int hw_frames = 0;
	unsigned char cmd = PHONE_CMD_QUERY;
	int skt = (io->stream == SND_PCM_STREAM_PLAYBACK)? pcm->sk_write : pcm->sk_read;
	int ret = 0;

	PHONE_DEBUG_PRINT("phone pointer - %d \n", io->stream);

	/* query hw pointer information */
	ret = send_socket(skt, &cmd, sizeof(unsigned char));
    if(ret > 0) {
		ret = recv_socket(skt, (unsigned char *)&hw_frames, sizeof(unsigned int));
        if(ret > 0) {
			pcm->hw_ptr = hw_frames % io->buffer_size;
		} else {
			pcm->hw_ptr = -1;
			return -EIO;
		}
	}

	PHONE_DEBUG_PRINT("phone pointer - %d, hw ptr: %d\n", io->stream, pcm->hw_ptr);

	return pcm->hw_ptr;
}


static snd_pcm_sframes_t phone_write(snd_pcm_ioplug_t *io,
				     const snd_pcm_channel_area_t *areas,
				     snd_pcm_uframes_t offset,
				     snd_pcm_uframes_t size)
{
	snd_pcm_phone_t *pcm = io->private_data;
	snd_pcm_uframes_t frames_to_read;
	snd_pcm_sframes_t ret = 0;
    RINGINFO ringbuffer;
	unsigned char cmd = PHONE_CMD_WRITE;
    unsigned char * buffer = NULL;
	unsigned int pcm_count = pcm->pcm_data_count % PHONE_PACKET_LEN;

	PHONE_DEBUG_PRINT("phone write - offset: %d, size: %d \n", offset, size);

	if ((pcm_count + 2 * size) <= PHONE_PACKET_LEN)
	{
		frames_to_read = size;
	}
	else
	{
		frames_to_read = (PHONE_PACKET_LEN - pcm_count) / 2;
	}

	unsigned char *buff = (unsigned char *)areas->addr + (areas->first + areas->step * offset) / 8;
	memcpy(pcm->pkt.pcm_data + pcm_count, buff, areas->step / 8 * frames_to_read);
	pcm->pcm_data_count += areas->step / 8 * frames_to_read;

	if ((pcm->pcm_data_count > 0) && (pcm->pcm_data_count % PHONE_PACKET_LEN == 0))
	{
		/* transfer data to Audio Server */
		int rsend = send_socket(pcm->sk_write, &cmd, sizeof(unsigned char));
		if(rsend > 0) {
			rsend = send_socket(pcm->sk_write, &pcm->pkt, sizeof(phone_packet_t));
		}

		if (rsend > 0)
		{
			ret = frames_to_read;
		}
		else
		{
			LOGE("ALSA PLUGIN Write: Lost contact with audioserver, pcm->sk_write %d, errno %d\n", pcm->sk_write, errno);
			ret = -EIO;
		}

		/* save in queue */
		if(!pthread_mutex_lock(g_vpHandle->pMutexLock)){
			if(g_vpHandle->sk_record > 0) {
				buffer = (unsigned char *)malloc(PHONE_PACKET_LEN);
				if(buffer != NULL) {
					memcpy(buffer, &pcm->pkt, PHONE_PACKET_LEN);

					ringbuffer.buf = (char *) buffer;
					ringbuffer.len = PHONE_PACKET_LEN;
					if(ring_buffer_in(&g_vpHandle->ringinfo, ringbuffer) <= 0) {
						LOGE("ring buffer is full\n");
					}
				} else {
					LOGE("can not allocate memory\n");
				}
			}
			pthread_mutex_unlock(g_vpHandle->pMutexLock);
		}
	}
	else
	{
		/* Ask for more */
		ret = frames_to_read;
	}

	PHONE_DEBUG_PRINT("phone write - offset: %d, size: %d done\n", offset, size);

	return ret;
}


static snd_pcm_sframes_t phone_read(snd_pcm_ioplug_t *io,
				    const snd_pcm_channel_area_t *areas,
				    snd_pcm_uframes_t offset,
				    snd_pcm_uframes_t size)
{
	snd_pcm_phone_t *pcm = io->private_data;
	snd_pcm_sframes_t ret = 0;
	unsigned short recordPCM[PHONE_PACKET_LEN];
	RINGINFO buffer;
	unsigned char *buff = NULL;
	unsigned char cmd = PHONE_CMD_READ;

	int i = 0;
	int skret = 0;

	PHONE_DEBUG_PRINT("phone read - offset: %d, size: %d \n", offset, size);
	/* if size is zero, return directly */
	if(size == 0) return 0;

	if (pcm->pcm_data_count == 0)
	{
		/* read data from Audio Server */
		int rsend = send_socket(pcm->sk_read, &cmd, sizeof(unsigned char));
		if(rsend > 0) {
			rsend = recv_socket(pcm->sk_read, &pcm->pkt, sizeof(phone_packet_t));
		}

		if(rsend <= 0) {
			LOGE("ALSA PLUGIN : Lost contact with audioserver, pcm->sk_read %d, errno %d\n", pcm->sk_read, errno);
			ret = -EIO;
		}
	}

	if (ret == 0)
	{
		/* Still ok, proceed */

		snd_pcm_uframes_t frames_to_write;
		buff = (unsigned char *)areas->addr + (areas->first + areas->step * offset) / 8;

		if ((pcm->pcm_data_count + 2 * size) <= PHONE_PACKET_LEN)
		{
			frames_to_write = size;
		}
		else
		{
			frames_to_write = (PHONE_PACKET_LEN - pcm->pcm_data_count) / 2;
	    }

		memcpy(buff, pcm->pkt.pcm_data + pcm->pcm_data_count,
		       areas->step / 8 * frames_to_write);

		pcm->pcm_data_count += (areas->step / 8 * frames_to_write);
		pcm->pcm_data_count %= PHONE_PACKET_LEN;
		ret = frames_to_write;
	}

	/* transfer data to record plugin */
	if(!pthread_mutex_lock(g_vpHandle->pMutexLock)){
		if(g_vpHandle->sk_record > 0) {
			buffer.buf = NULL;
			if (ring_buffer_out(&g_vpHandle->ringinfo, &buffer) == 0) {
				LOGE("write ring queue is empty\n");
			}
			/* mix the read/write data */
			for(i=0; i<PHONE_PACKET_LEN/2; i++) {
				recordPCM[i*2] = (buffer.buf)? ((short *)(buffer.buf))[i]: 0;
				recordPCM[i*2+1] =((short *)(buff))[i];
			}

			if(buffer.buf) free(buffer.buf);

			skret = send(g_vpHandle->sk_record, recordPCM, PHONE_PACKET_LEN*2, MSG_DONTWAIT);
			if(skret < PHONE_PACKET_LEN * 2)
				LOGW("Cannot transfer data to vtrec plugin\n");
		}
	    pthread_mutex_unlock(g_vpHandle->pMutexLock);
	}

	PHONE_DEBUG_PRINT("phone read - offset: %d, size: %d done\n", offset, size);
	return ret;
}


static int phone_prepare(snd_pcm_ioplug_t *io)
{
	snd_pcm_phone_t *pcm = io->private_data;
	int ret = 0;
	unsigned char cmd = PHONE_CMD_WBUF;
	unsigned int combine_with_call, playback_which_end, capture_which_end;
	unsigned int write_buffer_count = 0;

	PHONE_DEBUG_PRINT("phone prepare - %d \n", (io->stream == SND_PCM_STREAM_PLAYBACK)? pcm->sk_write : pcm->sk_read);

	if (pcm->combine_with_call == 1)
		combine_with_call = COMBWITHCALL;
	else if (pcm->combine_with_call == 0)
		combine_with_call = 0;
	else
		combine_with_call = COMBWITHCALL;/*default combine with call*/

	if (pcm->playback_which_end == 0)
		playback_which_end = 0;
	else if (pcm->playback_which_end == 1)
		playback_which_end = NEAREND;
	else if (pcm->playback_which_end == 2)
		playback_which_end = FAREND;
	else
		playback_which_end = 0;/*defaul both ends*/

	if (pcm->capture_which_end == 0)
		capture_which_end = 0;
	else if (pcm->capture_which_end == 1)
		capture_which_end = NEAREND;
	else if (pcm->capture_which_end == 2)
		capture_which_end = FAREND;
	else
		capture_which_end = 0;/*defaul both ends*/

	if (io->stream == SND_PCM_STREAM_PLAYBACK)
	{
		if(pcm->sk_write > 0) {
			LOGE("Plugin should be closed before prepare: - %d \n", pcm->sk_write);
			phone_disconnect(io);
			send_ipc_data(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE,
					0, (PCMSTREAMPLAYBACK << 16));
		}

		/* If not null for playback, xmms doesn't display time correctly */
		pcm->hw_ptr = 0;
		pcm->sk_write = -1;
		pcm->wr_rd_flag = SND_PCM_STREAM_PLAYBACK;

		send_ipc_data(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE,
			      0, ((PCMSTREAMPLAYBACK | combine_with_call | playback_which_end) << 16));
	}
	else
	{
		if(pcm->sk_read > 0) {
			LOGE("Plugin should be closed before prepare: - %d \n", pcm->sk_read);
			return -EIO;
		}

		pcm->hw_ptr = 0;
		pcm->sk_read = -1;
		pcm->wr_rd_flag = SND_PCM_STREAM_CAPTURE;

		send_ipc_data(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE,
			      0, ((PCMSTREAMCAPTURE | capture_which_end) << 16));
	}

	ret = phone_connect(io);
	if (ret != 0)
	{
		LOGE("Error prepare: socket connect failed\n");
		return -EIO;
	}

	PHONE_DEBUG_PRINT("phone prepare - %d done\n", (io->stream == SND_PCM_STREAM_PLAYBACK)? pcm->sk_write : pcm->sk_read);

	/* share write buffer count */
	if (io->stream == SND_PCM_STREAM_PLAYBACK)
	{
		write_buffer_count = io->buffer_size;
		ret = send_socket(pcm->sk_write, &cmd, sizeof(unsigned char));
		if(ret > 0) {
			ret = send_socket(pcm->sk_write, (unsigned char *)&write_buffer_count, sizeof(unsigned int));
		}

		if(ret <= 0) {
			LOGE("ALSA PLUGIN prepare: can not share write buffer count with Audio server - %d, ret - %d \n", pcm->sk_write, ret);
			return -EIO;
		}
	}

	return 0;
}

static int phone_hw_params(snd_pcm_ioplug_t *io, snd_pcm_hw_params_t *params)
{
	UNUSEDPARAM(io)
	UNUSEDPARAM(params)
	return 0;
}

static int phone_close(snd_pcm_ioplug_t *io)
{
	snd_pcm_phone_t *pcm = io->private_data;

       PHONE_DEBUG_PRINT("phone close - %d \n", io->stream);

	phone_disconnect(io);
	switch (io->stream)
	{
	case SND_PCM_STREAM_PLAYBACK:
		if (pcm->sk_write > 0)
		{
			pcm->sk_write = -1;

			send_ipc_data(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE,
				      0, (PCMSTREAMPLAYBACK << 16));
		}
		break;

	case SND_PCM_STREAM_CAPTURE:
		if(pcm->sk_read > 0)
		{
			pcm->sk_read = -1;

			send_ipc_data(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE,
				      0, (PCMSTREAMCAPTURE << 16));
		}
		break;

	default:
		LOGE("Unexpected ioplug received !!\n");
		return -EINVAL;
	}

	/* If not any opened stream anymore, close files */
	io->private_data = 0;
	free(pcm);

	/* close global handle */
	if(!pthread_mutex_lock(g_vpHandle->pMutexLock)){
		g_vpHandle->refCnt--;
		if(g_vpHandle->refCnt== 0){
			LOGD("try to free vplib resource");

			pthread_mutex_unlock(g_vpHandle->pMutexLock);
			write(g_vpHandle->pipefd[1], "bye", 4);
			if(g_vpHandle->hSocketThread > 0)
				pthread_join(*((pthread_t *)&g_vpHandle->hSocketThread), NULL);
			close(g_vpHandle->pipefd[0]);
			close(g_vpHandle->pipefd[1]);
			pthread_mutex_destroy(g_vpHandle->pMutexLock);
			free(g_vpHandle->pMutexLock);
			g_vpHandle->pMutexLock = NULL;

			free(g_vpHandle);
			g_vpHandle = NULL;
			LOGD("free vpHandle and close log file\n");
		}else{
			pthread_mutex_unlock(g_vpHandle->pMutexLock);
		}
	}

       PHONE_DEBUG_PRINT("phone close - %d done\n", io->stream);

	return 0;
}

static int phone_pcm_poll_descriptors_count(snd_pcm_ioplug_t *io)
{
	return 1;
}

static int phone_pcm_poll_descriptors(snd_pcm_ioplug_t *io, struct pollfd *pfd, unsigned int space)
{
	snd_pcm_phone_t *pcm = io->private_data;
	unsigned char cmd = PHONE_CMD_POLL;
	int ret = 0;

	PHONE_DEBUG_PRINT("poll descriptors - %d\n", io->stream);

	if (io->stream == SND_PCM_STREAM_PLAYBACK)
		pfd[0].fd = pcm->sk_write;
	else
		pfd[0].fd = pcm->sk_read;

	pfd[0].events = POLLIN;
	pfd[0].revents = 0;

	ret = send_socket(pfd[0].fd, &cmd, sizeof(unsigned char));
	if(ret <= 0) {
		pcm->hw_ptr = -1;
		return -EIO;
	}

	PHONE_DEBUG_PRINT("poll descriptors - %d done\n", io->stream);

	return 1;
}

static int phone_pcm_poll_revents(snd_pcm_ioplug_t *io, struct pollfd *pfd, unsigned intnfds, unsigned short *revents )
{
	snd_pcm_phone_t *pcm = io->private_data;
	unsigned char cmd = 0;
	int skt = (io->stream == SND_PCM_STREAM_PLAYBACK)? pcm->sk_write : pcm->sk_read;
	int ret = 0;

	PHONE_DEBUG_PRINT("poll revents - %d\n", io->stream);

	*revents = (pfd[0].revents & POLLIN) ? POLLIN : 0;

	ret = recv_socket(skt, (unsigned char *)&cmd, sizeof(unsigned char));
	if(ret <= 0) {
		pcm->hw_ptr = -1;
		return -EIO;
	}

	PHONE_DEBUG_PRINT("poll revents - %d done\n", io->stream);

    return 0;

}

static snd_pcm_ioplug_callback_t phone_playback_callback = {
	.start			= phone_start,
	.stop			= phone_stop,
	.drain			= phone_drain,
	.pointer		= phone_pointer,
	.transfer		= phone_write,
	.prepare		= phone_prepare,
	.hw_params		= phone_hw_params,
	.poll_descriptors_count = phone_pcm_poll_descriptors_count,
	.poll_descriptors	= phone_pcm_poll_descriptors,
	.poll_revents		= phone_pcm_poll_revents,
	.close			= phone_close,
};


static snd_pcm_ioplug_callback_t phone_capture_callback = {
	.start			= phone_start,
	.stop			= phone_stop,
	.drain			= phone_drain,
	.pointer		= phone_pointer,
	.transfer		= phone_read,
	.prepare		= phone_prepare,
	.hw_params		= phone_hw_params,
	.poll_descriptors_count = phone_pcm_poll_descriptors_count,
	.poll_descriptors	= phone_pcm_poll_descriptors,
	.poll_revents		= phone_pcm_poll_revents,
	.close			= phone_close,
};


static int phone_hw_constraint(snd_pcm_phone_t *pcm)
{
	snd_pcm_ioplug_t *io = &pcm->io;
	snd_pcm_access_t access_list[] = {
		SND_PCM_ACCESS_RW_INTERLEAVED,
		/* If we don't support mmap, we cannot use any plugin before this plugin.
		E.g. we cannot aplay -Dplug:phone wav*/
		SND_PCM_ACCESS_MMAP_INTERLEAVED
	};

	static const unsigned int format[] = {
		SND_PCM_FORMAT_S16_LE
	};

	int err;

	/* Access type */
	err = snd_pcm_ioplug_set_param_list(io, SND_PCM_IOPLUG_HW_ACCESS,
					    2, access_list);
	if (err < 0)
		return err;

	/* supported formats */
	err = snd_pcm_ioplug_set_param_list(io, SND_PCM_IOPLUG_HW_FORMAT,
					    1, format);
	if (err < 0)
		return err;

	/* supported channels */
	err = snd_pcm_ioplug_set_param_minmax(io, SND_PCM_IOPLUG_HW_CHANNELS, 1, 1);
	if (err < 0)
		return err;

	/* supported rates */
	err = snd_pcm_ioplug_set_param_minmax(io, SND_PCM_IOPLUG_HW_RATE, SAMPLE_RATE, SAMPLE_RATE);
	if (err < 0)
		return err;

	/* period size */
	err = snd_pcm_ioplug_set_param_minmax(io,
					      SND_PCM_IOPLUG_HW_PERIOD_BYTES,
					      PHONE_PACKET_LEN, PHONE_PACKET_LEN);
	if (err < 0)
		return err;

	/* periods */
	err = snd_pcm_ioplug_set_param_minmax(io, SND_PCM_IOPLUG_HW_PERIODS, 2, 10);
	if (err < 0)
		return err;
	return 0;
}

#define MAX_CLIENT_NUMBER 5

void * socket_server_handler(void * parm)
{
	int sockfd, newsockfd, servlen, clilen, ret;
    struct sockaddr_un  cli_addr, serv_addr;
    int i=0;
    int conn_count=0;
    fd_set fdsr;
    int maxsock;
    int	fd_conn[MAX_CLIENT_NUMBER] = {0, };
    int dataBuf[2];

    LOGD("enter socket_server_handler\n");

	/* create socket */
    if ((sockfd = socket(AF_UNIX,SOCK_STREAM,0)) < 0){
       LOGE("Error--creating socket\n");
       return NULL;
    }

    unlink(DOMAIN_SOCKET_NAME);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, DOMAIN_SOCKET_NAME);
    servlen=strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
    if(bind(sockfd,(struct sockaddr *)&serv_addr,servlen)<0){
       LOGE("Error--binding socket\n");
       return NULL;
    }

    listen(sockfd,MAX_CLIENT_NUMBER);

	maxsock = (sockfd > g_vpHandle->pipefd[0])? sockfd:g_vpHandle->pipefd[0];

	while (1) {
		FD_ZERO(&fdsr);
		FD_SET(sockfd,&fdsr);
		FD_SET(g_vpHandle->pipefd[0], &fdsr);

		for (i = 0; i < MAX_CLIENT_NUMBER; i++) {
			if (fd_conn[i] != 0) {
				FD_SET(fd_conn[i], &fdsr);
			}
		}

		LOGD("begin select.....\n");
		ret = select(maxsock+1, &fdsr, NULL, NULL, NULL);
		if(ret < 0){
			LOGE("Error--select accept socket\n");
			break;
		}else if(ret == 0){
			LOGW("Warning--select nothing\n");
			continue;
		}

		if(FD_ISSET(g_vpHandle->pipefd[0], &fdsr)){
			//receive bye msg from leader thread
			LOGD("Receive bye--exit select thread\n");
			break;
		}

		for(i = 0; i <MAX_CLIENT_NUMBER ; i++){
			if(FD_ISSET(fd_conn[i], &fdsr)){
				ret = recv(fd_conn[i], dataBuf, 8, 0);
				if(ret < 0){
					LOGW("Warning--recv <0 bytes, socket error, i:%d, error:%d\n", i,errno);
				}else if(ret == 0){
					close(fd_conn[i]);
					if(!pthread_mutex_lock(g_vpHandle->pMutexLock)){
						if(g_vpHandle->sk_record == fd_conn[i]){
							g_vpHandle->sk_record = -1;
						}
						pthread_mutex_unlock(g_vpHandle->pMutexLock);
					}
					fd_conn[i] = 0;
					conn_count--;
					LOGW("Warning--recv 0 bytes, close socket, i:%d\n", i);
				}else if(ret == 8){
					if(dataBuf[0] == VT_RECORD_CMD_TYPE){
						if(!pthread_mutex_lock(g_vpHandle->pMutexLock)){
							if(	dataBuf[1] == VT_RECORD_START){
								ring_buffer_init(&g_vpHandle->ringinfo);
								g_vpHandle->sk_record = fd_conn[i];
								LOGD("record socket is connected, i:%d\n",i);
							}else if(dataBuf[1] == VT_RECORD_STOP){
								LOGD("record socket is disconnected\n");
								ring_buffer_destroy(&g_vpHandle->ringinfo);
								g_vpHandle->sk_record = -1;
							}
							pthread_mutex_unlock(g_vpHandle->pMutexLock);
						}
					}//other header type can be extended here
				}
			}
		}

		if(FD_ISSET(sockfd, &fdsr)){
			clilen = sizeof(cli_addr);
			newsockfd = accept(sockfd,(struct sockaddr *)&cli_addr,&clilen);
			if (newsockfd < 0){
				LOGE("Error--accepting\n");
				continue;
			}

			LOGE("accept socket, conn_count:%d",conn_count);
			if(conn_count < MAX_CLIENT_NUMBER){
				for(i = 0; i < MAX_CLIENT_NUMBER; i++){
					if(fd_conn[i] == 0){
						fd_conn[i] =  newsockfd;
						conn_count++;
						LOGE("put socket to location i:%d, sock: %d",i, newsockfd);
						break;
					}
				}
				if(newsockfd > maxsock)
					maxsock = newsockfd;
			}else{
				LOGE("Error--max connections, exit\n");
				close(newsockfd);
			}
		}
	}

    for (i = 0; i < MAX_CLIENT_NUMBER; i++) {
        if (fd_conn[i] != 0) {
            close(fd_conn[i]);
        }
    }

    close(sockfd);
    unlink(DOMAIN_SOCKET_NAME);
    LOGE("exit socket_server_handler\n");

	return NULL;
}

SND_PCM_PLUGIN_DEFINE_FUNC(phone)
{
	UNUSEDPARAM(root)
	snd_config_iterator_t i, next;
	int err;
    pthread_mutexattr_t      oAttr;
    pthread_mutexattr_t      *pAttr = NULL;

	snd_pcm_phone_t *pcm;
	pcm = calloc(1, sizeof(*pcm));

	snd_config_for_each(i, next, conf)
	{
		snd_config_t *n = snd_config_iterator_entry(i);
		const char *id;

		if (snd_config_get_id(n, &id) < 0)
			continue;
		if (strcmp(id, "comment") == 0 || strcmp(id, "type") == 0)
			continue;

		if (strcmp(id, "playback_which_end") == 0)
		{
			if (snd_config_get_integer(n, &(pcm->playback_which_end)) < 0)
			{
				LOGE("Invalid value for playback_which_end\n");
				pcm->playback_which_end = 0;
			}
			LOGI("playback_which_end is %ld\n", pcm->playback_which_end);
			continue;
		}

		if (strcmp(id, "capture_which_end") == 0)
		{
			if (snd_config_get_integer(n, &(pcm->capture_which_end)) < 0)
			{
				LOGE("Invalid value for capture_which_end\n");
				pcm->capture_which_end = 0;
			}
			LOGI("capture_which_end is %ld\n", pcm->capture_which_end);
			continue;
		}

		if (strcmp(id, "combine_with_call") == 0)
		{
			if (snd_config_get_integer(n, &(pcm->combine_with_call)) < 0)
			{
				LOGE("Invalid value for combine_with_call\n");
				pcm->combine_with_call = 1;
			}
			LOGI("combine_with_call is %ld\n", pcm->combine_with_call);
			continue;
		}
	}

	pcm->io.version = SND_PCM_IOPLUG_VERSION;
	pcm->io.name = "ALSA <-> phone PCM I/O Plugin";

	/* modified by Paul Jan 11th */
	//pcm->io.poll_fd = 1;
	//pcm->io.poll_events = POLLOUT;
	pcm->io.poll_fd = -1;
	pcm->io.poll_events = 0;

	pcm->io.mmap_rw = 0;

	pcm->io.callback = stream == SND_PCM_STREAM_PLAYBACK ?
			   &phone_playback_callback : &phone_capture_callback;

	pcm->io.private_data = pcm;

	err = snd_pcm_ioplug_create(&pcm->io, name, stream, mode);
	if (err < 0)
		goto error;


	err = phone_hw_constraint(pcm);
	if (err < 0)
	{
		snd_pcm_ioplug_delete(&pcm->io);
		goto error;
	}

	//init vp processing
    if(!g_vpHandle){
        g_vpHandle = calloc(1,sizeof(*g_vpHandle));
        if(!g_vpHandle) LOGE("malloc g_vpHandle Error\n");

        g_vpHandle->sk_record = -1;
		LOGD("create mutex ");
        g_vpHandle->pMutexLock = calloc(1, sizeof(*g_vpHandle->pMutexLock));
        if (!pthread_mutexattr_init(&oAttr) && !pthread_mutexattr_settype(&oAttr, PTHREAD_MUTEX_RECURSIVE_NP)) {
            pAttr = &oAttr;
        }

        if (pthread_mutex_init(g_vpHandle->pMutexLock, pAttr)) {
			LOGE("init mutex fail \n");
        }
    }

    if(!pthread_mutex_lock(g_vpHandle->pMutexLock)){
        g_vpHandle->refCnt++;
        pthread_mutex_unlock(g_vpHandle->pMutexLock);
    }
	/*
	    create socket thread
	*/
	if(!g_vpHandle->hSocketThread) {
		if(pipe(g_vpHandle->pipefd) < 0) {
			LOGE("can not create pipe for socket thread\n");
		}
		pthread_create((pthread_t *)(&g_vpHandle->hSocketThread), NULL, socket_server_handler, NULL);
	}

	*pcmp = pcm->io.pcm;
	return 0;

 error:
	LOGE("error\n");
//	phone_free(pcm);
//	phone_disconnect(pcm);
	free(pcm);
	return err;
}

SND_PCM_PLUGIN_SYMBOL(phone);

