/*
Copyright (c) 2009, Marvell International Ltd.
All Rights Reserved.

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
*/


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "gstmp3dec.h"
//#include "ippmp3table.h"		/* IPP MP3 Decoder Constant Tables */

#include <string.h>
#ifdef DEBUG_PERFORMANCE
#include  <sys/time.h>

        struct timeval tstart, tend;
        struct timezone tz;

#endif


GST_DEBUG_CATEGORY_STATIC (mp3dec_debug);
#define GST_CAT_DEFAULT mp3dec_debug

/******************************************************************************
//
******************************************************************************/

#define MPEGVERSION 2
#define BITRATEINDEX 15

#define MP3_MAX_INIT 5
#define MP3_MAX_DEC 40
#define MP3CODEC_CONTINUEOKCNT_MAX	30

#define OUTPUT_FRAME_NUMBER 2

const int MP3_L3_bitRateTable[MPEGVERSION][BITRATEINDEX] = { \
	{ 0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320 }, \
	{ 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160 } };

/* codec include these table, wrapper no longer need it. */
#if 0
// MPEG-2 modified long block SFB table

IppMP3ScaleFactorBandTableLong Modified_Sfb_Table_Long =
{
	/* MPEG-2.5 @ 11.025 kHz */
    0,		6,		12,		18,		24,		30,		36,		44,		54,		66,		80,		96,
			116,	140,	168,	200,	238,	284,	336,	396,	464,	522,	576,

	/* MPEG-2.5 @ 12 kHz */
		0,   6,	  12,  18,	24,  30,  36,  44,  54,  66,  80,   96,		
		116, 140, 168, 200,	238, 284, 336, 396,	464, 522, 576,

		/* MPEG-2.5
		* 8 kHz
		*/
		0,	 12,  24,  36,  48,	 60,  72,  88,  108, 132, 160, 192,
		232, 280, 336, 400, 476, 566, 568, 570, 572, 574, 576,

		/* MPEG-1
		* 44.1 kHz
		*/
		0,   4,	  8,   12,	16,  20,  24,  30,	36,  44,  52,  62,		
		74,  90,  110, 134,	162, 196, 238, 288,	342, 418, 576,

		/* MPEG-1
		* 48 kHz
		*/
		0,	 4,   8,   12,  16,	 20,  24,  30,  36,	 42,  50,  60,
		72,	 88,  106, 128, 156, 190, 230, 276, 330, 384, 576,

		/* MPEG-1
		* 32 kHz
		*/
		0,   4,	  8,   12,  16,  20,  24,  30,  36,  44,  54,  66,		
		82,  102, 126, 156,	194, 240, 296, 364,	448, 550, 576 
};

/* MPEG-2 modified short block SFB table */
IppMP3ScaleFactorBandTableShort Modified_Sfb_Table_Short = 
{
	/* MPEG-2.5
	* 11.025 kHz
	*/
	0,	 4,   8,   12,  18,	 26,  36,		
		48,  62,  80,  104,	134, 174, 192,

		/* MPEG-2.5
		* 12 kHz
		*/
		0,	 4,   8,   12,  18,  26,  36,		
		48,	 62,  80,  104,	134, 174, 192,

		/* MPEG-2.5
		* 8 kHz
		*/
		0,	 8,   16,  24,  36,	 52,  72,		
		96,  124, 160, 162,	164, 166, 192,

		/* MPEG-1
		* 44.1 kHz
		*/
		0,	 4,   8,   12,  16,  22,  30,  
		40,  52,  66,  84,	106, 136, 192,

		/* MPEG-1
		* 48 kHz
		*/
		0,	 4,   8,   12,  16,	 22,  28,		
		38,  50,  64,  80,	100, 126, 192,

		/* MPEG-1
		* 32 kHz
		*/
		0,	 4,   8,   12,  16,	 22,  30,		
		42,  58,  78,  104, 138, 180, 192
};


/* Example mixed block partition table

For mixed blocks only, this partition table informs 
the Huffman decoder of how many SFBs to count for region0.
*/

IppMP3MixedBlockPartitionTable Modified_Mbp_Table = 
{
	// MPEG-2
	// 22.050 kHz
	// Long block SFBs		Short block SFBs
	6,					2,

		// MPEG-2
		// 24 kHz
		// Long block SFBs		Short block SFBs
		6,					2,		

		// MPEG-2
		// 16 kHz
		// Long block SFBs		Short block SFBs
		6,					2,		

		// MPEG-1
		// 44.1 kHz
		// Long block SFBs		Short block SFBs
		8,					0,

		// MPEG-1
		// 48 kHz
		// Long block SFBs		Short block SFBs
		8,					0,

		// MPEG-1
		// 32 kHz
		// Long block SFBs		Short block SFBs
		8,					0
};
#endif

static GstFlowReturn  _mp3_out_data(GstMP3Dec *mp3dec, int eosFlag);

int 
IPP_audioIndex2BitRate_MP3 (GstMP3Dec *mp3dec, int version, int index)
{
	if(index < 0 || index >= sizeof(MP3_L3_bitRateTable[0])/sizeof(MP3_L3_bitRateTable[0][0])) {
		GST_ERROR_OBJECT (mp3dec, "bitrate index %d has exceed the range!", index);
		return 0;
	}
	if(version < 0 || version > 2) {
		GST_ERROR_OBJECT (mp3dec, "mpeg version %d has exceed the range!", version);
		return 0;
	}
	if(version == 2) {
		version = 1;	//mpeg version 2.5 has the same table as that of mpeg version 2
	}
	return (MP3_L3_bitRateTable[version][index] * 1000);
}

/******************************************************************************
//
******************************************************************************/

enum {

	/* fill above with user defined signals */
	LAST_SIGNAL
};

enum {
	ARG_0,
	/* fill below with user defined arguments */
#ifdef DEBUG_PERFORMANCE	
	ARG_TOTALFRAME,	
	ARG_CODECTIME,	
#endif

};

/*
static GstStaticPadTemplate sink_factory = \
	GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK, GST_PAD_ALWAYS, \
	GST_STATIC_CAPS ("audio/mpeg, " \
					 "mpegversion = (int) 1, " \
					 "framed = (boolean) { FALSE, TRUE }, " \
					 "layer = (int) 3, " \
					 "rate = (int) [ 8000, 48000 ], " \
					 "channels = (int) { 1, 2 }, " \
					 "bitrate = (int) [ 32000, 320000 ]"));
*/

//In this plug-in, the support for id3 tag and ape tag is very limitted, we only skip those tags, don't extract the metadata from those tags. In real system, we suggest to use id3demux and apedemux.
static GstStaticPadTemplate sink_factory = \
	GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK, GST_PAD_ALWAYS, GST_STATIC_CAPS ("audio/mpeg," "mpegversion = (int) { 1, 2 }, " "layer = (int) 3;"
	"application/x-id3;"
	"application/x-apetag"));


static GstStaticPadTemplate src_factory = \
	GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS, \
	GST_STATIC_CAPS ("audio/x-raw-int, " \
					 "endianness = (int) "G_STRINGIFY (G_BYTE_ORDER)", " \
					 "signed = (boolean) TRUE, " \
					 "width = (int) 16, " \
					 "depth = (int) 16, " \
					 "rate = (int) [ 8000, 48000 ], " \
					 "channels = (int) [ 1, 2 ]"));


/* use boilerplate to declare the _base_init, _class_init and _init */
GST_BOILERPLATE (GstMP3Dec, gst_mp3dec, GstElement, GST_TYPE_ELEMENT);



static void 
gst_mp3dec_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	//GstMP3Dec *mp3dec = GST_MP3DEC (object);

	switch (prop_id)
	{
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void 
gst_mp3dec_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
#ifdef DEBUG_PERFORMANCE
	GstMP3Dec *mp3dec = GST_MP3DEC (object);
#endif
	switch (prop_id)
	{
#ifdef DEBUG_PERFORMANCE
	case ARG_TOTALFRAME:
		g_value_set_int (value, mp3dec->totalFrames);		
		break;
	case ARG_CODECTIME:
		g_value_set_int64 (value, mp3dec->codec_time);		
		break;
#endif
		
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static gboolean 
gst_mp3dec_sinkpad_setcaps (GstPad *pad, GstCaps *caps)
{
	GstMP3Dec *mp3dec = GST_MP3DEC (GST_OBJECT_PARENT (pad));

	int i = 0;
	gchar *name;
	GstStructure *str;
	const GValue *value;
	
	int stru_num = gst_caps_get_size (caps);
	GST_INFO_OBJECT (mp3dec, "gst_mp3dec_sinkpad_setcaps stru_num =%d\n",stru_num);
	
	GST_INFO_OBJECT (mp3dec, "****************************");
	for (i = 0; i < stru_num; i++) {
		str = gst_caps_get_structure (caps, i);
		name = gst_structure_get_name (str);
		GST_INFO_OBJECT (mp3dec, "name: %s\n", name);

		gchar *sstr;

		sstr = gst_structure_to_string (str);
		GST_INFO_OBJECT (mp3dec, "%s\n", sstr);
		g_free (sstr);		
	}
	
	GST_INFO_OBJECT (mp3dec, "****************************");

	if (mp3dec->tempbuf != NULL) {
		gst_buffer_unref (mp3dec->tempbuf);
		mp3dec->tempbuf = NULL;
	}	
	
	return TRUE;
}

static gboolean 
gst_mp3dec_sinkpad_event (GstPad *pad, GstEvent *event)
{
	GstMP3Dec *mp3dec = GST_MP3DEC (GST_OBJECT_PARENT (pad));
	gboolean eventRet;		

	switch (GST_EVENT_TYPE (event))
	{
		case GST_EVENT_FLUSH_STOP:
			  //give up those candidate push down buffer data
			mp3dec->framecnt = 0;
			mp3dec->sound.pSndFrame = mp3dec->pcmData;
			if(mp3dec->outBuf) {
				gst_buffer_unref(mp3dec->outBuf);
				mp3dec->outBuf = NULL;
			}
			mp3dec->lastTS = GST_CLOCK_TIME_NONE;
			mp3dec->nextTS = 0;
			eventRet = gst_pad_push_event (mp3dec->srcpad, event);
			break;
		case GST_EVENT_NEWSEGMENT:
		{			
		      GstFormat fmt;
		      gboolean is_update;
		      gint64 start, end, base;
		      gdouble rate;

			  mp3dec->bFirstPushDownDataAfterNewseg = 1;

			  //give up those candidate push down buffer data, in fact, it has been done in GST_EVENT_FLUSH_STOP.
			mp3dec->framecnt = 0;
			mp3dec->sound.pSndFrame = mp3dec->pcmData;
			if(mp3dec->outBuf) {
				gst_buffer_unref(mp3dec->outBuf);
				mp3dec->outBuf = NULL;
			}

			mp3dec->lastTS = GST_CLOCK_TIME_NONE;
			mp3dec->nextTS = 0;

		      gst_event_parse_new_segment (event, &is_update, &rate, &fmt, &start,
		          &end, &base);
		      if (fmt == GST_FORMAT_TIME) {
				if(start >= 0) {
					mp3dec->nextTS = start;
					//printf("<<<<<<<<<<<<<<<<new seg, mp3 TS %lld\n", mp3dec->nextTS);
				}
		      } else if (fmt == GST_FORMAT_BYTES) {
			      
				gint64 new_start = 0;
				gint64 new_end = -1;
				
				/* bytes -> time */

				if(0 != mp3dec->bitRate){					
					new_start = gst_util_uint64_scale (start*8, GST_SECOND, mp3dec->bitRate);
					new_end = gst_util_uint64_scale (end*8, GST_SECOND, mp3dec->bitRate);					
				}else{				
					new_start = 0;
					new_end = -1;			
				}				
				gst_event_unref (event);
				event = gst_event_new_new_segment (is_update, rate,
					GST_FORMAT_TIME, new_start, new_end, new_start);	

				GST_OBJECT_LOCK (mp3dec);

				mp3dec->nextTS = new_start;
				mp3dec->lastTS = GST_CLOCK_TIME_NONE;
				GST_OBJECT_UNLOCK (mp3dec);
				
				
		      }

			if (NULL != mp3dec->pMP3DecState)
			{					

				if (mp3dec->tempbuf != NULL) {
					gst_buffer_unref (mp3dec->tempbuf);
					mp3dec->tempbuf = NULL;
				}


				/*handle curr_buf*/
				mp3dec->curr_buf.left = 0;
				mp3dec->curr_buf.offset= 0;
				mp3dec->srcBitStream.bsCurBitOffset = 0;


			}					  
			eventRet = gst_pad_push_event (mp3dec->srcpad, event);		    
		      break;
		    }	    
		case GST_EVENT_EOS:
			_mp3_out_data(mp3dec, 1);
			GST_DEBUG_OBJECT (mp3dec, "Decoding Over, Total %5d frames", mp3dec->totalFrames);

			if (mp3dec->tempbuf != NULL) {
				gst_buffer_unref (mp3dec->tempbuf);
				mp3dec->tempbuf = NULL;
			}	
			eventRet = gst_pad_push_event (mp3dec->srcpad, event);
			break;
		default:
			eventRet = gst_pad_event_default (pad, event);
			break;
	}

	return eventRet;
}


/* currently only handle GST_FORMAT_TIME */
static gboolean 
gst_mp3dec_srcpad_event (GstPad *pad, GstEvent *event)
{
	gboolean res = FALSE;
	GstMP3Dec *mp3dec = GST_MP3DEC (GST_OBJECT_PARENT (pad));

	switch (GST_EVENT_TYPE (event)) {
		
		case GST_EVENT_SEEK:
			
			if (!(res = gst_pad_event_default (pad, event))) {

				gdouble rate;
				GstFormat format;
				GstSeekFlags flags;
				GstSeekType cur_type, stop_type;
				gint64 time_cur, time_stop;
				gint64 bytes_cur, bytes_stop;	

				gst_event_parse_seek (event, &rate, &format, &flags,
      					&cur_type, &time_cur, &stop_type, &time_stop);

				if (GST_FORMAT_TIME != format) {
					/* currently only handle GST_FORMAT_TIME */
					break;
				}
				mp3dec->seek = TRUE;					

				/* time -> bytes */

				bytes_cur = gst_util_uint64_scale (time_cur, mp3dec->bitRate, 8 * GST_SECOND);
				bytes_stop = gst_util_uint64_scale (time_stop, mp3dec->bitRate, 8 * GST_SECOND);

				GstEvent *seek_event = NULL;

				//memset (GST_BUFFER_DATA (mp3dec->outBuf), 0, GST_BUFFER_SIZE(mp3dec->outBuf));

				seek_event = gst_event_new_seek (rate, GST_FORMAT_BYTES, flags, cur_type,
				    bytes_cur, stop_type, bytes_stop);
				
				res = gst_pad_push_event (mp3dec->sinkpad, seek_event);
				
			}
		break;

		default:
			res = gst_pad_event_default (pad, event);
		break;
	}
	
	return res;
}

/* currently only handle GST_FORMAT_TIME */
static gboolean 
gst_mp3dec_srcpad_query (GstPad *pad, GstQuery *query)
{
	GstMP3Dec *mp3dec = GST_MP3DEC (GST_OBJECT_PARENT (pad));
	GstPad *peerpad = gst_pad_get_peer (mp3dec->sinkpad);
	gboolean queryRet = FALSE;


	switch (GST_QUERY_TYPE (query))
	{
	case GST_QUERY_DURATION:
		{
			GstFormat format;
			guint64 bytesLength, timeDuration;

			/* try upstream first, in case there's a demuxer */
			if ((queryRet = gst_pad_query_default (pad, query))){				
				break;
			}

			gst_query_parse_duration (query, &format, NULL);
			if (GST_FORMAT_TIME != format) {
				/* currently only handle GST_FORMAT_TIME */
				break;
			}

			format = GST_FORMAT_BYTES;
			if (!gst_pad_query_duration (peerpad, &format, &bytesLength)) {
				break;
			}

			if (mp3dec->bitRate != 0) {

				/* bytes -> time */
				
				timeDuration = gst_util_uint64_scale (bytesLength, GST_SECOND*8, mp3dec->bitRate);
				gst_query_set_duration (query, GST_FORMAT_TIME, timeDuration);
				GST_LOG_OBJECT (mp3dec, "total stream duration is: %" GST_TIME_FORMAT, GST_TIME_ARGS (timeDuration));
				
				queryRet = TRUE;
			} 			
		}

		break;
	case GST_QUERY_POSITION:
		{
			GstFormat format = GST_FORMAT_TIME;
			guint64 timePosition;

			  /* try upstream first, in case there's a demuxer */
			if ((queryRet = gst_pad_query_default (pad, query))){				
				break;
			}

			GST_OBJECT_LOCK (mp3dec);
			timePosition = mp3dec->nextTS;
			GST_OBJECT_UNLOCK (mp3dec);

			gst_query_set_position (query, format, timePosition);
			GST_LOG_OBJECT (mp3dec, "current stream position is: %" GST_TIME_FORMAT, GST_TIME_ARGS (timePosition));
		}

		break;
	default:
		queryRet = gst_pad_query_default (pad, query);
		break;
	}

	if(peerpad) {
		gst_object_unref(peerpad);
	}
	return queryRet;
}

/************************************************************************
* optimized output method, it will output OUTPUT_FRAME_NUMBER at one time to save thread switch time
* the less frame size, the more performance is optimized
************************************************************************/
static GstFlowReturn  _mp3_out_data(GstMP3Dec *mp3dec, int eosFlag)
{						

	int ret = 0;	
	GstClockTime tmpDuration = GST_CLOCK_TIME_NONE;

	/*If reach the end of file and still have data in outBuf, output it. If no data, return */
	if(eosFlag){
		if(mp3dec->framecnt)
			goto push;
		else
			return GST_FLOW_OK;
	}

	mp3dec->framecnt ++;

	/*alloc outBuf,set timestamp and offset, reset duration of outBuf.*/
	if(mp3dec->framecnt == 1)
	{
		GST_LOG_OBJECT (mp3dec, "init new buffer info..");
		mp3dec->outBuf = gst_buffer_new();
		GST_BUFFER_OFFSET (mp3dec->outBuf) = gst_util_uint64_scale (mp3dec->nextTS, mp3dec->samplingRate, GST_SECOND);
		GST_BUFFER_TIMESTAMP (mp3dec->outBuf) = mp3dec->nextTS;
		GST_BUFFER_DURATION (mp3dec->outBuf) = 0;
		GST_BUFFER_SIZE (mp3dec->outBuf) = 0;
		GST_BUFFER_DATA (mp3dec->outBuf) = mp3dec->pcmData;
		gst_buffer_set_caps (mp3dec->outBuf, GST_PAD_CAPS (mp3dec->srcpad));
	}

	/* Update codec output pointer and output buffer size */
	/* Codec need 4 byte aligned output buffer pointer. length may be none 4 byte aligned in seek case*/
	if(G_UNLIKELY(mp3dec->sound.sndLen % 4)){
		GST_WARNING("none 4 byte aligned codec output! Ignore this buffer.");
		return GST_FLOW_OK;
	}
	{
		char *padd = mp3dec->sound.pSndFrame;
		padd += mp3dec->sound.sndLen;
		mp3dec->sound.pSndFrame = (short*)padd;
		GST_BUFFER_SIZE (mp3dec->outBuf) += mp3dec->sound.sndLen;
	}

	/* Update duration info. */
	if(0 != mp3dec->samplingRate){
		tmpDuration = gst_util_uint64_scale
			((mp3dec->sound.sndLen >> 1) / mp3dec->sound.sndChannelNum, GST_SECOND, mp3dec->samplingRate);
		GST_BUFFER_DURATION (mp3dec->outBuf) += tmpDuration; 
	}

	GST_OBJECT_LOCK (mp3dec);
	mp3dec->nextTS += tmpDuration;        
	GST_OBJECT_UNLOCK (mp3dec);

	/* If outBuf is not full, return. */
	if(mp3dec->framecnt < OUTPUT_FRAME_NUMBER ) 
		return GST_FLOW_OK;

push:
	GST_LOG_OBJECT (mp3dec, "pushing accumulated buffer..");
	/* reset codec output pointer to beginning of large buffer */
	mp3dec->framecnt = 0;
	mp3dec->sound.pSndFrame = mp3dec->pcmData;
	
	//direct pushing buffer to downstream plugin doesn't obey Gstreaner rule, because you don't known when the sink finished using this data. So, memcpy is needed.

#if 1	//there is no obvious performance difference between calling gst_buffer_new_and_alloc or gst_pad_alloc_buffer_and_set_caps
	GstBuffer* downbuf = gst_buffer_new_and_alloc(GST_BUFFER_SIZE(mp3dec->outBuf));
	if(downbuf == NULL) {
		gst_buffer_unref(mp3dec->outBuf);
		mp3dec->outBuf = NULL;
		return GST_FLOW_ERROR;
	}
	GST_BUFFER_SIZE(downbuf) = GST_BUFFER_SIZE(mp3dec->outBuf);
	GST_BUFFER_TIMESTAMP(downbuf) = GST_BUFFER_TIMESTAMP(mp3dec->outBuf);
	GST_BUFFER_DURATION(downbuf) = GST_BUFFER_DURATION(mp3dec->outBuf);
	memcpy(GST_BUFFER_DATA(downbuf), GST_BUFFER_DATA(mp3dec->outBuf), GST_BUFFER_SIZE(downbuf));
	gst_buffer_set_caps (downbuf, GST_PAD_CAPS (mp3dec->srcpad));
#else
	GstBuffer* downbuf = NULL;
	GstFlowReturn askbuf_ret = gst_pad_alloc_buffer_and_set_caps(mp3dec->srcpad, -1, GST_BUFFER_SIZE(mp3dec->outBuf), GST_PAD_CAPS (mp3dec->srcpad), &downbuf);
	if(askbuf_ret != GST_FLOW_OK) {
		gst_buffer_unref(mp3dec->outBuf);
		mp3dec->outBuf = NULL;
		return askbuf_ret;
	}
	GST_BUFFER_TIMESTAMP(downbuf) = GST_BUFFER_TIMESTAMP(mp3dec->outBuf);
	GST_BUFFER_DURATION(downbuf) = GST_BUFFER_DURATION(mp3dec->outBuf);
	memcpy(GST_BUFFER_DATA(downbuf), GST_BUFFER_DATA(mp3dec->outBuf), GST_BUFFER_SIZE(downbuf));
#endif

	gst_buffer_unref(mp3dec->outBuf);
	mp3dec->outBuf = NULL;
	if(mp3dec->bFirstPushDownDataAfterNewseg != 0) {
		mp3dec->bFirstPushDownDataAfterNewseg = 0;
		GST_BUFFER_FLAG_SET(downbuf, GST_BUFFER_FLAG_DISCONT);
	}
	ret = gst_pad_push (mp3dec->srcpad, downbuf);

	return ret;
}

/************************************************************************
* this is old method, output every sameple
* easy to understand, you could choose accordingly to your requirement

static GstFlowReturn  _mp3_out_data(GstMP3Dec *mp3dec)
{						
	GstBuffer *outBuf = NULL;
	int ret = 0;	

	if((2 == mp3dec->channelNumber)||(1 == mp3dec->channelNumber)){	
		ret = gst_pad_alloc_buffer_and_set_caps (mp3dec->srcpad, 0, 
			mp3dec->sound.sndLen, GST_PAD_CAPS (mp3dec->srcpad), &outBuf);
		if(ret != GST_FLOW_OK)
		{								
			GST_ERROR_OBJECT (mp3dec, "mp3 can not malloc buffer ");
			return ret;
		}		
		memcpy (GST_BUFFER_DATA (outBuf), mp3dec->sound.pSndFrame, mp3dec->sound.sndLen);
		
	}else{	
		printf("channel number error");
		return GST_FLOW_ERROR;								
	}	

	if(0 != mp3dec->samplingRate){
		GST_BUFFER_OFFSET (outBuf) = gst_util_uint64_scale (mp3dec->nextTS, mp3dec->samplingRate, GST_SECOND);
		GST_BUFFER_TIMESTAMP (outBuf) = mp3dec->nextTS;
		GST_BUFFER_DURATION (outBuf) = gst_util_uint64_scale
			((mp3dec->sound.sndLen >> 1) / mp3dec->sound.sndChannelNum, GST_SECOND, mp3dec->samplingRate);
	}
	
	GST_OBJECT_LOCK (mp3dec);
	mp3dec->nextTS += GST_BUFFER_DURATION (outBuf);        
	GST_OBJECT_UNLOCK (mp3dec);

	if((ret = gst_pad_push (mp3dec->srcpad, outBuf))!= GST_FLOW_OK)
	{							
		GST_ERROR_OBJECT (mp3dec, "mp3 push error ret=%d\n",ret);	
		return ret;
	}	
	
	return GST_FLOW_OK;
	
}

************************************************************************/

static __inline int
parse_id3tag_len(unsigned char* pData, int len)
{
	if(len < 10) {
		return 0;	//if data is too short, we consider there is no ID3 tag
	}else{
		if(pData[0] == 'I' && pData[1] == 'D' && pData[2] == '3') {
			int id3sz = ((int)pData[6]<<21) | ((int)pData[7]<<14) | ((int)pData[8]<<7) | ((int)pData[9]);
			//printf("----id3 tag length %d\n", id3sz+10);
			return id3sz + 10;
		}else{
			return 0;
		}
	}
}

static __inline int
parse_apetag_len(unsigned char* pData, int len)
{
	char Preamble[] = "APETAGEX";
	if(len < 16) {
		return -1;
	}else{
		if(memcmp(pData, Preamble, 8) == 0) {
			int apesz = (int)((pData[12])|(pData[13]<<8)|(pData[14]<<16)|(pData[15]<<24));
			//printf("----ape tag length %d\n", apesz+32);
			return apesz+32;
		}else{
			return 0;	//no APE tag in stream
		}
	}
}

static GstFlowReturn 
gst_mp3dec_chain (GstPad *pad, GstBuffer *buf)
{
	GstMP3Dec *mp3dec = GST_MP3DEC (GST_OBJECT_PARENT (pad));
	
	int consumed_len = 0;
	int offset = 0;
	int ret = GST_FLOW_OK;

	if (GST_BUFFER_TIMESTAMP (buf) != GST_CLOCK_TIME_NONE) {
		if (GST_BUFFER_TIMESTAMP (buf) != mp3dec->lastTS) {
			mp3dec->nextTS = GST_BUFFER_TIMESTAMP (buf);
			mp3dec->lastTS = GST_BUFFER_TIMESTAMP (buf);
		}
	}
	//if(mp3dec->bFirstPushDownDataAfterNewseg == 1) {printf("<<<<<<<<<<<chain after new seg, mp3 TS %lld\n", mp3dec->nextTS);}

	 /* buffer + remaining data */
	if (mp3dec->tempbuf) {		
		buf = gst_buffer_join (mp3dec->tempbuf, buf);
		mp3dec->tempbuf = NULL;
	}

	if(mp3dec->iID3TagLen == -1) {
		mp3dec->iID3TagLen = parse_id3tag_len(GST_BUFFER_DATA(buf), GST_BUFFER_SIZE(buf));	//only care the tag before real mp3 stream data
	}
	if(mp3dec->iID3TagLen > 0) {
		if(mp3dec->iID3TagLen >= (int)GST_BUFFER_SIZE(buf)) {
			//total buf data is ID3tag
			mp3dec->iID3TagLen -= GST_BUFFER_SIZE(buf);
			gst_buffer_unref(buf);
			return GST_FLOW_OK;
		}else{
			GstBuffer* oldbuf = buf;
			buf = gst_buffer_new_and_alloc(GST_BUFFER_SIZE(oldbuf) - mp3dec->iID3TagLen);
			memcpy(GST_BUFFER_DATA(buf), GST_BUFFER_DATA(oldbuf)+mp3dec->iID3TagLen, GST_BUFFER_SIZE(buf));
			mp3dec->iID3TagLen = 0;
			GST_BUFFER_TIMESTAMP(buf) = GST_BUFFER_TIMESTAMP(oldbuf);
			GST_BUFFER_DURATION(buf) = GST_BUFFER_DURATION(oldbuf);
			gst_buffer_unref(oldbuf);
		}
	}

	if(mp3dec->iAPETagLen == -1) {
		mp3dec->iAPETagLen = parse_apetag_len(GST_BUFFER_DATA(buf), GST_BUFFER_SIZE(buf));	//only care the tag before real mp3 stream data
		if(mp3dec->iAPETagLen == -1) {
			mp3dec->tempbuf = buf;	//accumulate the data
			return GST_FLOW_OK;
		}
	}
	if(mp3dec->iAPETagLen > 0) {
		if(mp3dec->iAPETagLen >= (int)GST_BUFFER_SIZE(buf)) {
			//total buf data is APEtag
			mp3dec->iAPETagLen -= GST_BUFFER_SIZE(buf);
			gst_buffer_unref(buf);
			return GST_FLOW_OK;
		}else{
			GstBuffer* oldbuf = buf;
			buf = gst_buffer_new_and_alloc(GST_BUFFER_SIZE(oldbuf) - mp3dec->iAPETagLen);
			memcpy(GST_BUFFER_DATA(buf), GST_BUFFER_DATA(oldbuf)+mp3dec->iAPETagLen, GST_BUFFER_SIZE(buf));
			mp3dec->iAPETagLen = 0;
			GST_BUFFER_TIMESTAMP(buf) = GST_BUFFER_TIMESTAMP(oldbuf);
			GST_BUFFER_DURATION(buf) = GST_BUFFER_DURATION(oldbuf);
			gst_buffer_unref(oldbuf);
		}
	}


	mp3dec->curr_buf.pdata = GST_BUFFER_DATA (buf);	
	mp3dec->curr_buf.size = GST_BUFFER_SIZE (buf);
	mp3dec->curr_buf.offset = 0;
	mp3dec->curr_buf.left = GST_BUFFER_SIZE (buf);	

	mp3dec->srcBitStream.pBsBuffer = GST_BUFFER_DATA (buf);	
	mp3dec->srcBitStream.pBsCurByte = mp3dec->srcBitStream.pBsBuffer;
	mp3dec->srcBitStream.bsByteLen = GST_BUFFER_SIZE (buf);		

	if (mp3dec->pMP3DecState == NULL) 
	{				
		mp3dec->retCode = DecodeSendCmd_MP3Audio(IPPAC_MP3_FINDNEXT_SYNCWORD,&(mp3dec->srcBitStream), &offset, NULL);
		mp3dec->srcBitStream.pBsCurByte += offset;

		if(mp3dec->retCode != IPP_STATUS_NOERR){
			GST_INFO_OBJECT (mp3dec, "this is id3 header, next");
			goto chain_exit;
		}

		mp3dec->iCodecContinueOkCnt = 0;
		mp3dec->retCode = DecoderInitAlloc_MP3(&(mp3dec->srcBitStream), mp3dec->pCBTable, &(mp3dec->sound), &(mp3dec->pMP3DecState));
		mp3dec->streamBegin = TRUE; 	/*first time*/

		if (mp3dec->retCode != IPP_STATUS_INIT_OK) {
			if((mp3dec->init_error ++) > MP3_MAX_INIT){
				GST_ERROR_OBJECT (mp3dec, "mp3 init error, do not try any more");
				ret = GST_FLOW_ERROR;
				goto chain_exit;
			}
			mp3dec->pMP3DecState = NULL;

			if(mp3dec->curr_buf.left < INPUT_BUF_SIZE_MP3){
				if (mp3dec->curr_buf.left < mp3dec->curr_buf.size) {					
					mp3dec->tempbuf = gst_buffer_create_sub (buf, mp3dec->curr_buf.offset, mp3dec->curr_buf.left );
				} else {
					mp3dec->tempbuf = buf;
					gst_buffer_ref (buf);
				}
			}

			GST_INFO_OBJECT (mp3dec, "mp3dec plugin init error, try next buffer again..");
			goto dec_err;
		}else{
			mp3dec->init_error = 0;
		}
		
		mp3dec->totalFrames = 0;
	}

	if (mp3dec->pMP3DecState) {
		do
		{      
			if(TRUE == mp3dec->seek ){
				int NextSyncCode = 0;	

				GST_DEBUG_OBJECT (mp3dec, "Sending cmd to seek next sync code");
				mp3dec->retCode = DecodeSendCmd_MP3Audio(IPPAC_MP3_FINDNEXT_SYNCWORD, &(mp3dec->srcBitStream), (&NextSyncCode), mp3dec->pMP3DecState);
				if(IPP_STATUS_NOERR != mp3dec->retCode){
					/* find again in next buffer. */
					GST_WARNING_OBJECT (mp3dec, "Finding sync code fail!");
					goto dec_err;
				}

				mp3dec->srcBitStream.pBsCurByte += NextSyncCode;
				if(mp3dec->srcBitStream.pBsCurByte - mp3dec->srcBitStream.pBsBuffer > mp3dec->srcBitStream.bsByteLen){
					goto dec_err;
				}
				mp3dec->seek = FALSE;
				mp3dec->seeked = TRUE;
			}

#ifdef DEBUG_PERFORMANCE
            gettimeofday (&tstart, &tz);
#endif
			mp3dec->retCode = Decode_MP3(&(mp3dec->srcBitStream), &(mp3dec->sound), mp3dec->pMP3DecState);               
#ifdef DEBUG_PERFORMANCE
            gettimeofday (&tend, &tz);
            mp3dec->codec_time += (tend.tv_sec - tstart.tv_sec)*1000000 + (tend.tv_usec - tstart.tv_usec);
#endif

			switch (mp3dec->retCode)
			{
			case IPP_STATUS_FRAME_COMPLETE:
				{
					if(mp3dec->iCodecContinueOkCnt >= MP3CODEC_CONTINUEOKCNT_MAX) {
						mp3dec->init_error = 0;	//clear init_error
					}else{
						mp3dec->iCodecContinueOkCnt++;
					}

					if (G_UNLIKELY(mp3dec->streamBegin)) {
						GstCaps *Tmp;
						/* get stream information from IPP MP3 decoder */
						void *pIN = NULL;
						DecodeSendCmd_MP3Audio(IPPAC_MP3_GET_VERSION, pIN, &(mp3dec->mpegVersion), mp3dec->pMP3DecState);
						DecodeSendCmd_MP3Audio(IPPAC_MP3_GET_LAYER, pIN, &(mp3dec->mpegLayer), mp3dec->pMP3DecState);
						DecodeSendCmd_MP3Audio(IPPAC_MP3_GET_CHANNEL, pIN, &(mp3dec->channelNumber), mp3dec->pMP3DecState);
						DecodeSendCmd_MP3Audio(IPPAC_MP3_GET_SAMPLERATE, pIN, &(mp3dec->samplingRate), mp3dec->pMP3DecState);
						DecodeSendCmd_MP3Audio(IPPAC_MP3_GET_BITRATE, pIN, &(mp3dec->bitRateIndex), mp3dec->pMP3DecState);

						mp3dec->bitRate = IPP_audioIndex2BitRate_MP3(mp3dec, mp3dec->mpegVersion, mp3dec->bitRateIndex);
						
						mp3dec->framesize = mp3dec->sound.sndLen;
						
						GST_INFO_OBJECT (mp3dec, "mp3dec->samplingRate=%d\n",mp3dec->samplingRate);
						GST_INFO_OBJECT (mp3dec, "mp3dec->channelNumber=%d\n",mp3dec->channelNumber);

						Tmp = gst_caps_new_simple ("audio/x-raw-int", 
							"endianness", G_TYPE_INT, G_BYTE_ORDER, 
							"signed", G_TYPE_BOOLEAN, TRUE, 
							"width", G_TYPE_INT, 16, 
							"depth", G_TYPE_INT, 16, 
							"rate", G_TYPE_INT, mp3dec->samplingRate, 
							"channels", G_TYPE_INT, mp3dec->channelNumber,
							NULL);
						gst_pad_set_caps(mp3dec->srcpad, Tmp);
						gst_caps_unref(Tmp);

						IPP_MemMalloc(&(mp3dec->pcmData), mp3dec->sound.sndLen * OUTPUT_FRAME_NUMBER, 4);
						GST_DEBUG_OBJECT (mp3dec, "alloc repack buf size=%d\n", mp3dec->sound.sndLen * OUTPUT_FRAME_NUMBER);
						if(!mp3dec->pcmData){
							GST_ERROR_OBJECT (mp3dec, "alloc repack buf error!\n");
							ret = GST_FLOW_ERROR;
							goto chain_exit;
						}
						memcpy (mp3dec->pcmData, mp3dec->sound.pSndFrame, mp3dec->sound.sndLen);
						IPP_MemFree(&(mp3dec->sound.pSndFrame));
						mp3dec->sound.pSndFrame = mp3dec->pcmData;

						mp3dec->streamBegin = FALSE;
					}

					consumed_len = mp3dec->srcBitStream.pBsCurByte - mp3dec->srcBitStream.pBsBuffer;

					mp3dec->curr_buf.offset = consumed_len;
					mp3dec->curr_buf.left = mp3dec->curr_buf.size-consumed_len;

					/* if channel number changes during playback, stop playing. */
					/* if seek just happened, output length may be different with normal value, so it is not error. */
					if(mp3dec->sound.sndLen != mp3dec->framesize && mp3dec->seeked == FALSE){
						_mp3_out_data(mp3dec, 1);
						gst_buffer_unref (buf);
						GST_ERROR_OBJECT (mp3dec, "mp3 This stream is not supported");
						ret = GST_FLOW_NOT_SUPPORTED;
						goto chain_exit;
					}
					mp3dec->seeked = FALSE;

					if(GST_FLOW_OK != (ret=_mp3_out_data(mp3dec, 0))){
						goto chain_exit;
					}

					mp3dec->totalFrames ++;
				}
				break;

			case IPP_STATUS_FRAME_UNDERRUN:
				break;

			case IPP_STATUS_BUFFER_UNDERRUN:					
			case IPP_STATUS_SYNCNOTFOUND_ERR:				
			
				 /* Keep the leftovers in raw stream */
				if (mp3dec->curr_buf.left > 0) {
					
					if (mp3dec->curr_buf.left < mp3dec->curr_buf.size) {
						mp3dec->tempbuf = gst_buffer_create_sub (buf, mp3dec->curr_buf.offset, mp3dec->curr_buf.left );
					} else {
						mp3dec->tempbuf = buf;
						gst_buffer_ref (buf);
					}
				}
				goto chain_exit;
				break;

				/*seek will add IPP_STATUS_FRAME_ERR and IPP_STATUS_NOTSUPPORTED_ERR, omit */

			case IPP_STATUS_FRAME_ERR:
				GST_ERROR_OBJECT (mp3dec, "Mp3 IPP_STATUS_FRAME_ERR");				
				goto dec_err;
				
			case IPP_STATUS_NOTSUPPORTED_ERR:
				GST_ERROR_OBJECT (mp3dec, "mp3 This stream is not supported");
				goto dec_err;

			case IPP_STATUS_FRAME_HEADER_INVALID:
				GST_ERROR_OBJECT (mp3dec, "Frame header invalid!");
				{
					int NextSyncCode = 0;
					/* After seek operation, codec may take some data as sync code, we re-find sync code. */
					ret = DecodeSendCmd_MP3Audio(IPPAC_MP3_FINDNEXT_SYNCWORD, &(mp3dec->srcBitStream), (&NextSyncCode), mp3dec->pMP3DecState);
					if(IPP_STATUS_NOERR != ret){
						/* find again in next buffer. increase error counter */
						GST_WARNING_OBJECT (mp3dec, "Re-finding sync code fail!");
						mp3dec->seek = TRUE;
						goto dec_err;
					}
					mp3dec->srcBitStream.pBsCurByte += NextSyncCode;
					if(mp3dec->srcBitStream.pBsCurByte - mp3dec->srcBitStream.pBsBuffer > mp3dec->srcBitStream.bsByteLen){
						mp3dec->seek = TRUE;
						goto dec_err;
					}
				}
				break;
				
			case IPP_STATUS_BS_END:
				GST_DEBUG_OBJECT (mp3dec, "The stream will meet END soon");
				break;

			default:
				goto dec_err;
			}
		} while((mp3dec->retCode == IPP_STATUS_FRAME_COMPLETE || mp3dec->retCode == IPP_STATUS_FRAME_UNDERRUN));
	}

chain_exit:
	gst_buffer_unref(buf);
	if(ret < GST_FLOW_UNEXPECTED) {
		GST_ELEMENT_ERROR(mp3dec, STREAM, DECODE, (NULL), ("%s() return un-accepted error %d", __FUNCTION__, ret));
	}
	return ret;

dec_err:
	GST_ERROR_OBJECT (mp3dec,"codec retcode error %d\n",mp3dec->retCode);
	mp3dec->iCodecContinueOkCnt = 0;
	/* We take init_error as error counter to count decode error. If dec error happen 40 times, quit. */
	if((mp3dec->init_error ++) > (MP3_MAX_DEC)){
		/* push out data in internal buffer */
		GST_ERROR_OBJECT (mp3dec,"Error times exceed maximum times!");
		_mp3_out_data(mp3dec, 1);
		ret = GST_FLOW_ERROR;
	}
	goto chain_exit;
}

static gboolean
gst_mp3dec_start (GstMP3Dec *mp3dec)
{

	
	mp3dec->pMP3DecState = NULL;
	
	mp3dec->bitRateIndex = 0;
	mp3dec->bitRate = 0;
	
	mp3dec->totalFrames = 0;

	/*handle curr_buf*/
	mp3dec->curr_buf.left = 0;
	mp3dec->curr_buf.offset= 0;
	
	mp3dec->tempbuf = NULL;
	mp3dec->init_error = 0;

	mp3dec->framesize = 0;

	mp3dec->nextTS = 0;
	mp3dec->lastTS = GST_CLOCK_TIME_NONE;

	mp3dec->seek = FALSE;	
	mp3dec->seeked = FALSE;	
	mp3dec->codec_time = 0;	

	miscInitGeneralCallbackTable (&(mp3dec->pCBTable));

	/* output buffer for decoded frame */
	IPP_MemMalloc(&(mp3dec->sound.pSndFrame), OUTPUT_BUF_SIZE_MP3, 4);
	mp3dec->pcmData = mp3dec->sound.pSndFrame;
	memset(mp3dec->sound.pSndFrame, 0x00, OUTPUT_BUF_SIZE_MP3);
	/* associate sound with out_buf */
	mp3dec->sound.pSndFrame = (Ipp16s*)(mp3dec->sound.pSndFrame);
	mp3dec->sound.sndLen = 0;	

	mp3dec->framecnt = 0;
	mp3dec->outBuf = NULL;
	mp3dec->lastBuf = NULL;   /* the subBuffer containing the last frames */

	mp3dec->iCodecContinueOkCnt = 0;
	return TRUE;
}

static gboolean
gst_mp3dec_stop (GstMP3Dec *mp3dec)
{

	if (mp3dec->pMP3DecState != NULL) {
		DecoderFree_MP3 (&(mp3dec->pMP3DecState));
	}
	IPP_MemFree (&(mp3dec->pcmData));
	miscFreeGeneralCallbackTable (&(mp3dec->pCBTable));

	if (mp3dec->tempbuf != NULL) {
		gst_buffer_unref (mp3dec->tempbuf);
		mp3dec->tempbuf = NULL;
	}

	if(mp3dec->outBuf) {
		gst_buffer_unref(mp3dec->outBuf);
		mp3dec->outBuf = NULL;
	}
	mp3dec->framecnt = 0;

	mp3dec->iCodecContinueOkCnt = 0;

	return TRUE;
}

static GstStateChangeReturn 
gst_mp3dec_change_state (GstElement *element, GstStateChange transition)
{
	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
	GstMP3Dec *mp3dec = GST_MP3DEC (element);

	switch (transition)
	{
	case GST_STATE_CHANGE_NULL_TO_READY:

		if(!gst_mp3dec_start(mp3dec)){
			goto statechange_failed;
		}	
		
		break;

	case GST_STATE_CHANGE_READY_TO_PAUSED:
		//ready to decode a new stream
		mp3dec->iID3TagLen = -1;
		mp3dec->iAPETagLen = -1;
		break;
	
			
	default:
		break;
	}

	ret = parent_class->change_state (element, transition);
	if (ret == GST_STATE_CHANGE_FAILURE){
		return ret;
	}

	switch (transition)
	{
	case GST_STATE_CHANGE_READY_TO_NULL:
		
		if(!gst_mp3dec_stop(mp3dec)){
			goto statechange_failed;
		}        

		break;		
	default:
		break;
	}

	return ret;

statechange_failed:
	{
		/* subclass must post a meaningfull error message */
		GST_ERROR_OBJECT (mp3dec, "state change failed");
		return GST_STATE_CHANGE_FAILURE;
	}

	return ret;
}

static void 
gst_mp3dec_base_init (gpointer g_klass)
{
	static GstElementDetails mp3dec_details = {
		"MP3 Audio Decoder", 
		"Codec/Decoder/Audio", 
		"MP3 Audio Decoder based-on IPP & CODEC", 
		""
	};
	GstElementClass *element_class = GST_ELEMENT_CLASS (g_klass);
	
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));

	gst_element_class_set_details (element_class, &mp3dec_details);
}

static void 
gst_mp3dec_class_init (GstMP3DecClass *klass)
{
	GObjectClass *gobject_class = (GObjectClass*) klass;
	GstElementClass *gstelement_class = (GstElementClass*) klass;

	parent_class = g_type_class_peek_parent(klass);

	gobject_class->set_property = GST_DEBUG_FUNCPTR (gst_mp3dec_set_property);
	gobject_class->get_property = GST_DEBUG_FUNCPTR (gst_mp3dec_get_property);

#ifdef DEBUG_PERFORMANCE	
	g_object_class_install_property (gobject_class, ARG_TOTALFRAME,
  		g_param_spec_int ("totalframes", "Totalframe of mp3stream",
          	"Number of frame", 0, G_MAXINT, 0, G_PARAM_READABLE));
	g_object_class_install_property (gobject_class, ARG_CODECTIME,
  		g_param_spec_int64 ("codectime", "mp3 decode time",
          	"codec time (microsecond)", 0, G_MAXINT64, 0, G_PARAM_READABLE));
#endif
    
	gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_mp3dec_change_state);

	GST_DEBUG_CATEGORY_INIT (mp3dec_debug, "mp3dec", 0, "MP3 Decode Element");
}

static void 
gst_mp3dec_init (GstMP3Dec *mp3dec, GstMP3DecClass *mp3dec_klass)
{
	GstElementClass *klass = GST_ELEMENT_CLASS (mp3dec_klass);
	

	mp3dec->sinkpad = gst_pad_new_from_template (\
		gst_element_class_get_pad_template (klass, "sink"), "sink");

	gst_pad_set_setcaps_function (mp3dec->sinkpad, GST_DEBUG_FUNCPTR (gst_mp3dec_sinkpad_setcaps));
	gst_pad_set_event_function (mp3dec->sinkpad, GST_DEBUG_FUNCPTR (gst_mp3dec_sinkpad_event));
	gst_pad_set_chain_function (mp3dec->sinkpad, GST_DEBUG_FUNCPTR (gst_mp3dec_chain));

	mp3dec->srcpad = gst_pad_new_from_template (\
		gst_element_class_get_pad_template (klass, "src"), "src");

	gst_pad_set_event_function(mp3dec->srcpad, GST_DEBUG_FUNCPTR (gst_mp3dec_srcpad_event));
	gst_pad_set_query_function(mp3dec->srcpad, GST_DEBUG_FUNCPTR (gst_mp3dec_srcpad_query));

	gst_element_add_pad (GST_ELEMENT (mp3dec), mp3dec->sinkpad);
	gst_element_add_pad (GST_ELEMENT (mp3dec), mp3dec->srcpad);
	

	mp3dec->bFirstPushDownDataAfterNewseg = 0;
}

static gboolean 
plugin_init (GstPlugin *plugin)
{	
	return gst_element_register (plugin, "mp3dec", GST_RANK_PRIMARY-1, GST_TYPE_MP3DEC);	//force mp3dec's rank to GST_RANK_PRIMARY, which is higher than other mp3 decoder like ffdec_mp3 and lower than mp3parse
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR, 
                   GST_VERSION_MINOR, 
                   "mvl_mp3dec", 
                   "mp3dec decoder based on IPP, "__DATE__, 
                   plugin_init, 
                   VERSION, 
                   "LGPL", 
                   "", 
                   "");




/* EOF */
