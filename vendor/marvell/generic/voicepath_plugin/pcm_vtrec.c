/******************************************************************************
// (C) Copyright 2010 Marvell International Ltd.
// All Rights Reserved
******************************************************************************/


#include <stdio.h>
#include <sys/poll.h>
#include <time.h>

#include <alsa/asoundlib.h>
#include <alsa/pcm_external.h>

#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

/* debug */
#define PCM_DEBUG 0
#if PCM_DEBUG
#define dbg(format, arg ...) printf("pcm_vtrec: " format "\n", ## arg)
#else
#define dbg(format, arg ...)
#endif

#include <utils/Log.h>
#define LOG_TAG "ALSA_PLUGIN--VTREC"

#define VTREC_PACKET_LEN          320           /* 160 smaples */
#define PCM_SAMPLE_SIZE           2             /* 16 bit */
#define SAMPLE_RATE               8000          /* 8khz sample rate */

#define VTREC_PACKET_NSEC         20000000      /* 20 ms */

#define  DOMAIN_SOCKET_NAME "/data/misc/phone/phone_plugin"
#define  VT_RECORD_START 1
#define  VT_RECORD_STOP 0
#define  VT_RECORD_CMD_TYPE 1


typedef struct ipc_packet {
	unsigned char type;
} ipc_packet_t;


typedef struct vtrec_packet {
	unsigned char pcm_data[VTREC_PACKET_LEN*2];
} vtrec_packet_t;


typedef struct snd_pcm_vtrec {
	snd_pcm_ioplug_t io;
	snd_pcm_sframes_t hw_ptr;
	vtrec_packet_t pkt;
	unsigned int pcm_data_count;
	int sk_read;
	int sk_write;
	int wr_rd_flag;
	unsigned int rate;
	unsigned int channels;
	unsigned int frame_bytes;
	int started;
	int factorN;
	int factorF;
} snd_pcm_vtrec_t;



static int vtrec_disconnect(snd_pcm_ioplug_t * io)
{
	snd_pcm_vtrec_t *pcm = io->private_data;

	if(pcm->sk_read > 0) close(pcm->sk_read);
  pcm->sk_read = -1;
  
  if(pcm->sk_write > 0) close(pcm->sk_write);
  pcm->sk_write = -1;
	return 0;
}



static int vtrec_connect(snd_pcm_ioplug_t *io)
{
  snd_pcm_vtrec_t *pcm = io->private_data;
  int  servlen,sockfd;
  struct sockaddr_un  serv_addr;
  int buf[2];
  if (pcm->sk_read <= 0)
  {
      bzero((char *)&serv_addr,sizeof(serv_addr));
      serv_addr.sun_family = AF_UNIX;
   
      strcpy(serv_addr.sun_path, DOMAIN_SOCKET_NAME);
      servlen=strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
      if ((sockfd = socket(AF_UNIX, SOCK_STREAM,0)) < 0)
          LOGE("Creating socket error");
      if (connect(sockfd, (struct sockaddr *) &serv_addr, servlen) < 0)
          LOGE("error Connecting");
      pcm->sk_read = sockfd;
      
	    buf[0] = VT_RECORD_CMD_TYPE;
	    buf[1] = VT_RECORD_START;
      send(pcm->sk_read,buf,8,0);   
       
  }
  return 0;
}



static int vtrec_start(snd_pcm_ioplug_t *io)
{
	return 0;
}

static int vtrec_stop(snd_pcm_ioplug_t *io)
{
	return 0;
}

int vtrec_drain(snd_pcm_ioplug_t *io)
{
	return 0;
}

static snd_pcm_sframes_t vtrec_pointer(snd_pcm_ioplug_t *io)
{
	snd_pcm_vtrec_t *pcm = io->private_data;
	
	return pcm->hw_ptr;
}


static snd_pcm_sframes_t vtrec_write(snd_pcm_ioplug_t *io,
				     const snd_pcm_channel_area_t *areas,
				     snd_pcm_uframes_t offset,
				     snd_pcm_uframes_t size)
{
	int ret=0;

	return ret;
}


static snd_pcm_sframes_t vtrec_read(snd_pcm_ioplug_t *io,
				    const snd_pcm_channel_area_t *areas,
				    snd_pcm_uframes_t offset,
				    snd_pcm_uframes_t size)
{
	snd_pcm_vtrec_t *pcm = io->private_data;
	snd_pcm_sframes_t ret = 0;
	int i;
	short* pBuf;

	if (pcm->pcm_data_count == 0)
	{

		int nrecv = recv(pcm->sk_read, pcm->pkt.pcm_data, sizeof(vtrec_packet_t), 0);
                if (nrecv > 0)
		{
			ret = 0;
			pBuf = (short*)pcm->pkt.pcm_data;
			for(i = 0; i < (VTREC_PACKET_LEN/2); i++){
			  pBuf[i] = (((int)pBuf[i*2] * pcm->factorN) >> 16) + (((int)pBuf[i*2+1] * pcm->factorF) >> 16);
			}
			/* Increment hardware transmition pointer */
			pcm->hw_ptr = (pcm->hw_ptr + VTREC_PACKET_LEN / 2) % io->buffer_size;
		}
		else if (nrecv == -1 && errno == EAGAIN)
		{
			LOGE("ALSA PLUGIN: vtrec_read ERROR -EAGAIN");
			ret = -EAGAIN;
		}
		else
		{
			/* nrecv < 0 */
			ret = (errno == EPIPE ? -EIO : -errno);
			LOGE("ALSA PLUGIN : Lost contact with vp plug-in, pcm->sk_read %d, errno %d", pcm->sk_read, errno);
		}
	}

	if (ret == 0)
	{
		/* Still ok, proceed */

		snd_pcm_uframes_t frames_to_write;
		unsigned char *buff;
		buff = (unsigned char *)areas->addr + (areas->first + areas->step * offset) / 8;


		if ((pcm->pcm_data_count + 2 * size) <= VTREC_PACKET_LEN)
		{
			frames_to_write = size;
		}
		else
		{
			frames_to_write = (VTREC_PACKET_LEN - pcm->pcm_data_count) / 2;
		}

		memcpy(buff, pcm->pkt.pcm_data + pcm->pcm_data_count,
		       areas->step / 8 * frames_to_write);

		pcm->pcm_data_count += (areas->step / 8 * frames_to_write);
		pcm->pcm_data_count %= VTREC_PACKET_LEN;
		ret = frames_to_write;
	}

	return ret;
}


static int vtrec_prepare(snd_pcm_ioplug_t *io)
{
	snd_pcm_vtrec_t *pcm = io->private_data;
	int ret = 0;
	
	

	if (io->stream == SND_PCM_STREAM_PLAYBACK)
	{
		/* If not null for playback, xmms doesn't display time correctly */
		pcm->hw_ptr = 0;
	}
	else
	{
		pcm->hw_ptr = io->period_size;

	}
	
	ret = vtrec_connect(io);
	
	
	if (ret != 0)
		LOGE("Error prepare: socket connect failed");

	return 0;
}

static int vtrec_hw_params(snd_pcm_ioplug_t *io, snd_pcm_hw_params_t *params)
{
	return 0;
}

static int vtrec_close(snd_pcm_ioplug_t *io)
{
	snd_pcm_vtrec_t *pcm = io->private_data;
	int buf[2];

	switch (io->stream)
	{
	case SND_PCM_STREAM_PLAYBACK:
		vtrec_disconnect(io);

		break;

	case SND_PCM_STREAM_CAPTURE:
	  buf[0] = VT_RECORD_CMD_TYPE;
	  buf[1] = VT_RECORD_STOP;
    send(pcm->sk_read,buf,8,0);   
		vtrec_disconnect(io);
		break;

	default:
		LOGE("Unexpected ioplug received !!");
		return -EINVAL;
	}

	/* If not any opened stream anymore, close files */


	io->private_data = 0;
	free(pcm);
	return 0;
}

static int vtrec_pcm_poll_descriptors_count(snd_pcm_ioplug_t *io)
{
	return 0;
}

static int vtrec_pcm_poll_descriptors(snd_pcm_ioplug_t *io, struct pollfd *pfd, unsigned int space)
{
	return 0;
}

static int vtrec_pcm_poll_revents(snd_pcm_ioplug_t *io, struct pollfd *pfd, unsigned intnfds, unsigned short *revents )
{
	return 0;
}

static snd_pcm_ioplug_callback_t vtrec_playback_callback = {
	.start			= vtrec_start,
	.stop			= vtrec_stop,
	.drain			= vtrec_drain,
	.pointer		= vtrec_pointer,
	.transfer		= vtrec_write,
	.prepare		= vtrec_prepare,
	.hw_params		= vtrec_hw_params,
	.poll_descriptors_count = vtrec_pcm_poll_descriptors_count,
	.poll_descriptors	= vtrec_pcm_poll_descriptors,
	.poll_revents		= vtrec_pcm_poll_revents,
	.close			= vtrec_close,
};


static snd_pcm_ioplug_callback_t vtrec_capture_callback = {
	.start			= vtrec_start,
	.stop			= vtrec_stop,
	.drain			= vtrec_drain,
	.pointer		= vtrec_pointer,
	.transfer		= vtrec_read,
	.prepare		= vtrec_prepare,
	.hw_params		= vtrec_hw_params,
	.poll_descriptors_count = vtrec_pcm_poll_descriptors_count,
	.poll_descriptors	= vtrec_pcm_poll_descriptors,
	.poll_revents		= vtrec_pcm_poll_revents,
	.close			= vtrec_close,
};


static int vtrec_hw_constraint(snd_pcm_vtrec_t *pcm)
{
	snd_pcm_ioplug_t *io = &pcm->io;
	static const snd_pcm_access_t access_list[] = {
		SND_PCM_ACCESS_RW_INTERLEAVED,
	};

	static const unsigned int format[] = {
		SND_PCM_FORMAT_S16_LE
	};

	int err;

	/* Access type */
	err = snd_pcm_ioplug_set_param_list(io, SND_PCM_IOPLUG_HW_ACCESS,
					    1, access_list);
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
					      VTREC_PACKET_LEN, VTREC_PACKET_LEN);
	if (err < 0)
		return err;

	/* periods */
	err = snd_pcm_ioplug_set_param_minmax(io, SND_PCM_IOPLUG_HW_PERIODS, 2, 2);
	if (err < 0)
		return err;
	return 0;
}

static int get_int_parm(snd_config_t *n, const char *id, const char *str,int *val_ret)
{
    long val;
    int err;
    if (strcmp(id, str))
        return 0;
    err = snd_config_get_integer(n, &val);
    if (err < 0) {
        SNDERR("Invalid value for %s parameter", id);
        return err;
    }
    *val_ret = val;
    return 1;
}

SND_PCM_PLUGIN_DEFINE_FUNC(vtrec)
{
	snd_config_iterator_t i, next;
	int err;
	snd_pcm_vtrec_t *pcm;


  pcm = calloc(1, sizeof(*pcm));

	snd_config_for_each(i, next, conf)
	{
		snd_config_t *n = snd_config_iterator_entry(i);
		const char *id;

		if (snd_config_get_id(n, &id) < 0)
			continue;
		if (strcmp(id, "comment") == 0 || strcmp(id, "type") == 0)
			continue;
		if(get_int_parm(n, id, "nearEndMixFactor_Q16", &pcm->factorN) < 0){
		  free(pcm);
		  return -EINVAL;
		}
		if(get_int_parm(n, id, "farEndMixFactor_Q16", &pcm->factorF) < 0){
		  free(pcm);
		  return -EINVAL;
		}
        	
	}


  LOGE("pcm->factorN :%d, pcm->factorF:%d", pcm->factorN, pcm->factorF);

	pcm->io.version = SND_PCM_IOPLUG_VERSION;
	pcm->io.name = "ALSA <-> vtrec PCM I/O Plugin";

	/* modified by Paul Jan 11th */
	pcm->io.poll_fd = -1;
	pcm->io.poll_events = 0;

	pcm->io.mmap_rw = 0;

	pcm->io.callback = stream == SND_PCM_STREAM_PLAYBACK ?
			   &vtrec_playback_callback : &vtrec_capture_callback;

	pcm->io.private_data = pcm;

  if (stream == SND_PCM_STREAM_PLAYBACK)
	{
		/* If not null for playback, xmms doesn't display time correctly */
		pcm->sk_write = -1;
		pcm->wr_rd_flag = SND_PCM_STREAM_PLAYBACK;
	}
	else
	{
		pcm->sk_read = -1;
		pcm->wr_rd_flag = SND_PCM_STREAM_CAPTURE;

	}
	
	err = snd_pcm_ioplug_create(&pcm->io, name, stream, mode);
	if (err < 0)
		goto error;


	err = vtrec_hw_constraint(pcm);
	if (err < 0)
	{
		snd_pcm_ioplug_delete(&pcm->io);
		goto error;
	}

	*pcmp = pcm->io.pcm;
	return 0;

 error:
	LOGE("error creating plug-in");

	free(pcm);
	return err;
}

SND_PCM_PLUGIN_SYMBOL(vtrec);

