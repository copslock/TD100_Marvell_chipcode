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


#include "gstaacdec.h"

#include <string.h>

#ifdef DEBUG_PERFORMANCE
#include  <sys/time.h>
	struct timeval tstart, tend;
	struct timezone tz;

#endif



GST_DEBUG_CATEGORY_STATIC (aacdec_debug);
#define GST_CAT_DEFAULT aacdec_debug


/*****************************************************************************/
//
/*****************************************************************************/

/**
 * AAC Frequency Table
 * sampling frequency table, defined in aacdecoder.c
 *
 * { 96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000 }
 */
static const int ippaacdec_fs[12]={	/* sampling frequency table */
	96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050,
	16000, 12000, 11025, 8000
};

/* Various AAC type */
#define AAC_VERSIONS	2

#define MAX_DECODE_ERRORS 40

#define OUTPUT_FRAME_NUMBER 6


/* object types for AAC */
#define MAIN       1
#define LC         2
#define SSR        3
#define LTP        4
#define HE_AAC     5

/* version set by user */
#define AAC_V1     1
#define AAC_V2     2

int 
IPP_auidoGetFrequencyIndex_AAC (int freq)
{
	int index = 0;
	while (freq < ippaacdec_fs[index]) {
		index ++;
	}

	if ((freq == ippaacdec_fs[index]) || index == 0) {
		return index;
	} else {
		if ((freq - ippaacdec_fs[index]) > (ippaacdec_fs[index-1] - freq)) {
			return (index - 1);
		} else {
			return index;
		}
	}
}

/*****************************************************************************/
//
/*****************************************************************************/

enum {

	/* fill above with user defined signals */
	LAST_SIGNAL
};

enum {
	ARG_0,
	/* fill below with user defined arguments */
	/*ARG_VERSION,*/
	
#ifdef DEBUG_PERFORMANCE	
	ARG_TOTALFRAME,	
	ARG_CODECTIME,	
#endif

};


static GstStaticPadTemplate sink_factory = 
	GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK, GST_PAD_ALWAYS, 
	//GST_STATIC_CAPS ("ANY")/*
	GST_STATIC_CAPS ("audio/mpeg, " 
					 "mpegversion = (int) { 2, 4 }, " 
					 "framed = (boolean) { FALSE, TRUE }, " 
					 "rate = (int) [ 8000, 96000 ], "
					 "channels = (int) { 1, 2 , 6 }, " 
					 "bitrate = (int) [ 8000, 320000 ]")//*/
					 );

static GstStaticPadTemplate src_factory = \
	GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS, 
	GST_STATIC_CAPS ("audio/x-raw-int, " 
					 "endianness = (int) "G_STRINGIFY (G_BYTE_ORDER)", " 
					 "signed = (boolean) TRUE, " 
					 "width = (int) 16, " 
					 "depth = (int) 16, " 
					 "rate = (int) [ 8000, 96000 ], " 
					 "channels = (int) [ 1, 2 ]"));


/* use boilerplate to declare the _base_init, _class_init and _init */
GST_BOILERPLATE (GstAACDec, gst_aacdec, GstElement, GST_TYPE_ELEMENT);


static gboolean 
gst_aacdec_sinkpad_setcaps (GstPad *pad, GstCaps *caps)
{
	GstAACDec *aacdec = GST_AACDEC (GST_OBJECT_PARENT (pad));

	guint channel, rate;
	
	int i = 0;
	const GValue *value;


	int stru_num = gst_caps_get_size (caps);
	GstStructure *str = NULL;
	gchar *name;
	
	GST_INFO_OBJECT (aacdec, "****************************");
	for (i = 0; i < stru_num; i++) {
		str = gst_caps_get_structure (caps, i);
		name = gst_structure_get_name (str);
		GST_INFO_OBJECT (aacdec, "name: %s\n", name);

		gchar *sstr;

		sstr = gst_structure_to_string (str);
		GST_INFO_OBJECT (aacdec, "%s\n", sstr);
		g_free (sstr);	
	}
	
	GST_INFO_OBJECT (aacdec,"****************************");

	int format_adts = -1, type_adts = -1;
	const gchar* format_str = gst_structure_get_string(str, "stream-format");
	if(format_str) {
		GST_INFO_OBJECT(aacdec, "found aac: stream-format statement in setcaps, is %s", format_str);
		if(strcmp(format_str, "adts") == 0) {
			format_adts = 1;
		}
	}

	const gchar* type_str = gst_structure_get_string(str, "stream-type");
	if(type_str) {
		GST_INFO_OBJECT(aacdec, "found aac: stream-type statement in setcaps, is %s", type_str);
		if(strcmp(type_str, "adts") == 0) {
			type_adts = 1;
		}
	}
	if((format_adts == 1 && type_adts != 0) || (type_adts == 1 && format_adts != 0)) {
		aacdec->ADTS_judge = 1;
		GST_INFO_OBJECT(aacdec, "in setcaps, judge it's adts stream");
	}

	if(gst_structure_get_boolean(str, "framed", &aacdec->InputFramed) == FALSE) {
		aacdec->InputFramed = FALSE;
	}else{
		GST_INFO_OBJECT(aacdec, "found aac: framed statement in setcaps, is %s", aacdec->InputFramed ? "true":"false");
	}

	int chan = 0;
	if(gst_structure_get_int(str, "channels", &chan) == TRUE) {
		if(chan == 6) {
			aacdec->bIsMultiChanDownMix = 1;
			GST_DEBUG_OBJECT(aacdec, "found 6 channels in aac setcaps");
		}
	}


	if ((value = gst_structure_get_value (str, "codec_data"))) {
		
		guint8 *cdata;
		guint csize;
		GstBuffer *buf;

		int object_index = 0;
		int rate_index = 0;
		int channels = 0;

		/* We have codec data, means packetised stream */
		buf = gst_value_get_buffer (value);

		cdata = GST_BUFFER_DATA (buf);
		csize = GST_BUFFER_SIZE (buf);
		
		if(csize >= 2){
			/*
			* codec_data is LC part information, use to initialize
			* object_index (5 bits + 1)
			* rate_index 4 bits
			* channel 4 bits
			*/

			object_index = (cdata[0]>>3);		
			rate_index = ((cdata[0]&0x7)<<1) + (cdata[1]>>7);
			channels = (cdata[1]>>3)&0xf;

			if(LTP == object_index){
				GST_INFO_OBJECT (aacdec, "LTP stream");
				aacdec->ltp = TRUE;
			}else if(HE_AAC == object_index){
				GST_INFO_OBJECT (aacdec, "HE_AAC stream");
				aacdec->version = HE_AAC;
			}

			/* please use LC part sample rate to initialize, channel is critical, codec could handle itself, even configure channel is wrong */

			aacdec->samplingIndex = rate_index;
			aacdec->channelConfig = channels;

			if(channels == 6) {
				aacdec->bIsMultiChanDownMix = 1;
				GST_DEBUG_OBJECT(aacdec, "found 6 channels in aac setcaps codec_data");
			}

			GST_INFO_OBJECT (aacdec, "object_index=%d,rate_index=%d,channels=%d\n",object_index,rate_index,channels);
			
		}

		aacdec->raw = TRUE;

		 if (aacdec->tempbuf != NULL) {
			gst_buffer_unref (aacdec->tempbuf);
			aacdec->tempbuf = NULL;
		}
	

	} else if ((value = gst_structure_get_value (str, "framed")) &&
		g_value_get_boolean (value) == TRUE) {
		//2009.11.09 modified, framed == TRUE doens't mean it's raw stream, it still could be adts
		//aacdec->raw = TRUE;
	} else {
		aacdec->raw = FALSE;
	}

	
	
	return TRUE;
}

static void 
gst_aacdec_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	//GstAACDec *aacdec = GST_AACDEC (object);
	//int tmp = 0;
	switch (prop_id)
	{
	/*
	case ARG_VERSION:
		aacdec->version_config = g_value_get_int (value);
		break;
	*/
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void 
gst_aacdec_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
#ifdef DEBUG_PERFORMANCE
	GstAACDec *aacdec = GST_AACDEC (object);
#endif
	switch (prop_id)
	{
/*
	case ARG_VERSION:
		g_value_set_int (value, aacdec->version_config);		
		break;
*/
#ifdef DEBUG_PERFORMANCE
	case ARG_TOTALFRAME:
		g_value_set_int (value, aacdec->totalFrames);		
		break;
	case ARG_CODECTIME:
		g_value_set_int64 (value, aacdec->codec_time);		
		break;
#endif

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static gboolean
_aac_convert_data (GstAACDec *aacdec, GstFormat src_format, gint64 src_val,
    GstFormat dest_format, gint64 * dest_val)
{

	guint64 decodedBytes = 0; 
	guint64 decodedTime = 0;
	gint64 val = 0;

	GST_OBJECT_LOCK (aacdec);
	if(aacdec->gTotalDuration>0){
		decodedBytes = aacdec->gTotalLength;
		decodedTime = aacdec->gTotalDuration;
	}else{
		decodedBytes = aacdec->decodedBytes;
		decodedTime = aacdec->decodedTime;
	}
	GST_OBJECT_UNLOCK (aacdec);
	GST_DEBUG_OBJECT(aacdec, "decodedBytes %lld, decodedTime %lld", decodedBytes, decodedTime);

	if (decodedBytes == 0 || decodedTime == 0)
    		return FALSE;	

	if (src_format == GST_FORMAT_BYTES && dest_format == GST_FORMAT_TIME) {
		val = gst_util_uint64_scale (src_val, decodedTime, decodedBytes);		
	}else if (src_format == GST_FORMAT_TIME && dest_format == GST_FORMAT_BYTES) {
		if(src_val != -1) {
			//if src_val == -1 && format is TIME, it means unknown end time, map it to -1
			val = gst_util_uint64_scale (src_val, decodedBytes, decodedTime);
		}else{
			val = -1;
		}
	}else if (src_format == dest_format){
		val = src_val;
	} else {
		return FALSE;
	}	

	if (dest_val)
		*dest_val = (gint64) val;
	

	return TRUE;
}


/* currently only handle GST_FORMAT_TIME */
/*seems farther object will help to handle ref, we should leave ref = 1*/
static gboolean 
gst_aacdec_srcpad_event (GstPad *pad, GstEvent *event)
{
	gboolean res = FALSE;
	GstAACDec *aacdec = GST_AACDEC (GST_OBJECT_PARENT (pad));

	switch (GST_EVENT_TYPE (event)) {
		
		case GST_EVENT_SEEK:
		
			if (!(res = gst_pad_event_default (pad, event))) {

				gdouble rate;
				GstFormat format;
				GstSeekFlags flags;
				GstSeekType cur_type, stop_type;
				gint64 time_cur, time_stop;
				gint64 bytes_cur, bytes_stop;	

				if(!aacdec->seek_support){
					GST_INFO_OBJECT (aacdec,"ADIF format, we do not support seek");
					return FALSE;
				}
				
				aacdec->seek = TRUE;		

				gst_event_parse_seek (event, &rate, &format, &flags,
      					&cur_type, &time_cur, &stop_type, &time_stop);
				
				if (GST_FORMAT_TIME != format) {
					/* currently only handle GST_FORMAT_TIME */
					break;
				}				

				/* time -> bytes */

				 if ((!_aac_convert_data (aacdec, GST_FORMAT_TIME, time_cur,GST_FORMAT_BYTES, &bytes_cur))
				 	|| (!_aac_convert_data (aacdec, GST_FORMAT_TIME, time_stop,GST_FORMAT_BYTES, &bytes_stop))) {
					return FALSE;
				}				
				
				//gst_event_unref (event);		
				GstEvent *seek_event = NULL;
				
				/* We need not clear repack buffer data, for flush data  */
				//GST_INFO_OBJECT (aacdec, "seek begin, clear output buffer..");
				/* reset codec output pointer to beginning of large buffer */
				/*aacdec->framecnt = 0;
				aacdec->sound.pSndFrame = aacdec->pcmData;*/

				/* clear source repack buffer */
				 if (aacdec->tempbuf != NULL) {
					gst_buffer_unref (aacdec->tempbuf);
					aacdec->tempbuf = NULL;
				}

				seek_event = gst_event_new_seek (rate, GST_FORMAT_BYTES, flags, cur_type,
				    bytes_cur, stop_type, bytes_stop);
				
				res = gst_pad_push_event (aacdec->sinkpad, seek_event);	

			}
		break;
		case GST_EVENT_CUSTOM_UPSTREAM:
		{
			const GstStructure *structure = gst_event_get_structure(event);
			if(gst_structure_has_name(structure, "mrvlipp-average-bps")) {
				GST_OBJECT_LOCK(aacdec);
				aacdec->gTotalDuration = g_value_get_uint64 (gst_structure_get_value (structure,"aac-duration"));
				aacdec->gTotalLength = g_value_get_uint64 (gst_structure_get_value (structure,"aac-length"));
				GST_OBJECT_UNLOCK(aacdec);
				GST_DEBUG_OBJECT(aacdec, "Get aac adts total duration %lld, length %lld from custom event", aacdec->gTotalDuration, aacdec->gTotalLength);
				res = TRUE;	//we processed and not transfer the event to other element, therefore, return true and unref it.
				gst_event_unref(event);
			}else{
				//for other custom event, just call default method
				res = gst_pad_event_default(pad, event);
			}
		}
		break;
		default:
			res = gst_pad_event_default (pad, event);
		break;
	}	
	return res;
}

static gboolean 
gst_aacdec_srcpad_query (GstPad *pad, GstQuery *query)
{
	GstAACDec *aacdec = GST_AACDEC (GST_OBJECT_PARENT (pad));
	GstPad *peerpad = gst_pad_get_peer (aacdec->sinkpad);
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
			
				break;
			}

			format = GST_FORMAT_BYTES;
			if (!gst_pad_query_duration (peerpad, &format, &bytesLength)) {
			
				break;
			}

			/* bytes -> time */

			queryRet = _aac_convert_data (aacdec, GST_FORMAT_BYTES, bytesLength,
		          GST_FORMAT_TIME, &timeDuration);

			if (queryRet) {
				gst_query_set_duration (query, GST_FORMAT_TIME, timeDuration);		        
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
			
			GST_OBJECT_LOCK (aacdec);
			timePosition = aacdec->nextTS;
			GST_OBJECT_UNLOCK (aacdec);

			gst_query_set_position (query, format, timePosition);
			GST_LOG_OBJECT (aacdec, "current stream position is: %" GST_TIME_FORMAT, GST_TIME_ARGS (timePosition));

			queryRet = TRUE;
		}

		break;
	default:
		queryRet = gst_pad_query_default (pad, query);
		break;

	}
	

	return queryRet;
}

/*current do not support LTP*/
static IppCodecStatus _aac_init_raw(GstAACDec *aacdec)
{
	
	IppCodecStatus ret = IPP_STATUS_NOERR;
	IppAACDecoderConfig config;
	IppBitstream InputBackup;
	IppAACDecoderConfig	CfgBackup;
	IppSound	SndBackup;
	int bNeedChangeCfg;

    memset(&config, 0, sizeof(IppAACDecoderConfig));
    
#if 0
	if(aacdec->ltp){
        config.profileType = AAC_AOT_LTP;
	}else{
    	/*here first consider user configuration, second consider detection */
	    if(AAC_V1 == aacdec->version_config){
		    /*user configure v1*/
		    config.profileType = AAC_AOT_HE;
	    }else if(AAC_V2 == aacdec->version_config){
		    /*user configure v2*/
		    config.profileType = AAC_AOT_HE_PS;
	    }else if(HE_AAC == aacdec->version){
		    /* autodetect, only effect for profile=5 in mp4 format */
		    config.profileType = AAC_AOT_HE;
		}
	}
#endif
	config.profileType = AAC_AOT_HE_PS;	//aacdec->version is abandoned. we always set config.profileType to AAC_AOT_HE_PS, LTP isn't supported.
	
    config.streamFmt = AAC_SF_RAW;
	config.samplingFrequencyIndex = aacdec->samplingIndex;
	config.channelConfiguration = aacdec->channelConfig;

	InputBackup = aacdec->srcBitStream;
	SndBackup = aacdec->sound;
	CfgBackup = config;
	bNeedChangeCfg = 0;
	ret = DecoderInitAlloc_AAC(&(aacdec->srcBitStream), &(config), aacdec->pCBTable, &(aacdec->pAACDecState));
	GST_DEBUG_OBJECT(aacdec, "Init ipp aac decoder ret %d in %s()", ret, __FUNCTION__);

	//workaround for performance, 2011.07.01
	if(ret == IPP_STATUS_INIT_OK) {
		ret = Decode_AAC(&(aacdec->srcBitStream), &(aacdec->sound), aacdec->pAACDecState);
		GST_DEBUG_OBJECT(aacdec, "Try to decode 1 frame to know more info, ret %d in %s()", ret, __FUNCTION__);
		if(ret == IPP_STATUS_FRAME_COMPLETE) {
			int aacVersion = -1;
			DecodeSendCmd_AACAudio(IPPAC_AAC_GET_VERSION, NULL, &aacVersion, aacdec->pAACDecState);
			GST_DEBUG_OBJECT(aacdec, "After attempt decoding 1 frame, get samplerate %d, aac ver %d", aacdec->sound.sndSampleRate, aacVersion);
			if(aacdec->sound.sndSampleRate > 48000 && aacVersion > 4) {
				//need change configuration
				bNeedChangeCfg = 1;
			}
		}
		DecoderFree_AAC(&aacdec->pAACDecState);
		aacdec->srcBitStream = InputBackup;
		aacdec->sound = SndBackup;
		config = CfgBackup;
		if(bNeedChangeCfg) {
			config.profileType = AAC_AOT_LC;
		}
		aacdec->pAACDecState = NULL;
		ret = DecoderInitAlloc_AAC(&(aacdec->srcBitStream), &(config), aacdec->pCBTable, &(aacdec->pAACDecState));
		GST_DEBUG_OBJECT(aacdec, "Re init ipp aac decoder, ret %d", ret);
	}
	return ret;
}



static IppCodecStatus _aac_init_nonraw(GstAACDec *aacdec, guint8 * buffer, guint len)
{
	IppCodecStatus ret = IPP_STATUS_NOERR;
	IppBitstream InputBackup;
	IppAACDecoderConfig	CfgBackup;
	IppSound	SndBackup;
	int bNeedChangeCfg;
	
	IppAACDecoderConfig config;
	int version = -1;
	int object_type = -1;
	int sr_idx = -1;
	int channels = -1;	
	
    memset(&config, 0, sizeof(IppAACDecoderConfig));
   
	if ((buffer[0] == 'A') && (buffer[1] == 'D') &&
	    (buffer[2] == 'I') && (buffer[3] == 'F')){
	    
		aacdec->bIsADIF = 1;
		GST_INFO_OBJECT (aacdec, "ADIF format");
		aacdec->seek_support = FALSE; 	/*ADIF do not have sync code, we do not support*/

		//????? how to get channel information from ADIF header ??????
		//???use a temporary decoder and decode a frame or just assume it's multi-channel????

		//for adif, we assume it's 6 channels and do down mix. if it's 1 or 2 channel, down mix is bypassed by codec and shouldn't cause side-effect.
		aacdec->bIsMultiChanDownMix = 1;

	}else{
		if((buffer[0] == 'A') && (buffer[1] == 'D') &&
			(buffer[2] == 'T') && (buffer[3] == 'S') &&
			(buffer[4] == 0xFF) && ((buffer[5] & 0xF6) == 0xF0)){

			version = buffer[5] & 0x08;	
			
			object_type = (buffer[6]&0xC0)>>6;
			sr_idx = (buffer[6]&0x3C)>>2;
			channels = ((buffer[6]&0x01)<<2)|((buffer[7]&0xC0)>>6);

			if(channels == 6) {
				aacdec->bIsMultiChanDownMix = 1;
				GST_DEBUG_OBJECT(aacdec, "found 6 channels in _aac_init_nonraw");
			}

			GST_INFO_OBJECT (aacdec, "object_type=%d, sr_idx=%d, channels=%d\n", object_type, sr_idx, channels);	

			
		}else if ((buffer[0] == 0xFF)&&((buffer[1] & 0xF6) == 0xF0)){
		
			version = buffer[1] & 0x08;

			object_type = (buffer[2]&0xC0)>>6;
			sr_idx = (buffer[2]&0x3C)>>2;
			channels = ((buffer[2]&0x01)<<2)|((buffer[3]&0xC0)>>6);

			if(channels == 6) {
				aacdec->bIsMultiChanDownMix = 1;
				GST_DEBUG_OBJECT(aacdec, "found 6 channels in _aac_init_nonraw line %d", __LINE__);
			}

			GST_INFO_OBJECT (aacdec, "object_type=%d, sr_idx=%d, channels=%d\n", object_type, sr_idx, channels);	
			
		}else{
			GST_ERROR_OBJECT(aacdec, "format not support");
			return IPP_STATUS_INIT_ERR;
		}		

	}
#if 0
	if(AAC_V1 == aacdec->version_config){
		/*user configure v1*/
		config.profileType = AAC_AOT_HE;
	}else if(AAC_V2 == aacdec->version_config){
		/*user configure v2*/
		config.profileType = AAC_AOT_HE_PS;
	}else{
	    config.profileType = AAC_AOT_HE_PS;
	}
#endif
	config.profileType = AAC_AOT_HE_PS;	//aacdec->version is abandoned. we always set config.profileType to AAC_AOT_HE_PS

	InputBackup = aacdec->srcBitStream;
	SndBackup = aacdec->sound;
	CfgBackup = config;
	bNeedChangeCfg = 0;
	ret = DecoderInitAlloc_AAC(&(aacdec->srcBitStream), &(config), aacdec->pCBTable, &(aacdec->pAACDecState));
	GST_DEBUG_OBJECT(aacdec, "Init ipp aac decoder ret %d in %s()", ret, __FUNCTION__);

	//workaround for performance, 2011.07.01
	if(ret == IPP_STATUS_INIT_OK) {
		ret = Decode_AAC(&(aacdec->srcBitStream), &(aacdec->sound), aacdec->pAACDecState);
		GST_DEBUG_OBJECT(aacdec, "Try to decode 1 frame to know more info, ret %d in %s()", ret, __FUNCTION__);
		if(ret == IPP_STATUS_FRAME_COMPLETE) {
			int aacVersion = -1;
			DecodeSendCmd_AACAudio(IPPAC_AAC_GET_VERSION, NULL, &aacVersion, aacdec->pAACDecState);
			GST_DEBUG_OBJECT(aacdec, "After attempt decoding 1 frame, get samplerate %d, aac ver %d", aacdec->sound.sndSampleRate, aacVersion);
			if(aacdec->sound.sndSampleRate > 48000 && aacVersion > 4) {
				//need change configuration
				bNeedChangeCfg = 1;
			}
		}
		DecoderFree_AAC(&aacdec->pAACDecState);
		aacdec->srcBitStream = InputBackup;
		aacdec->sound = SndBackup;
		config = CfgBackup;
		if(bNeedChangeCfg) {
			config.profileType = AAC_AOT_LC;
		}
		aacdec->pAACDecState = NULL;
		ret = DecoderInitAlloc_AAC(&(aacdec->srcBitStream), &(config), aacdec->pCBTable, &(aacdec->pAACDecState));
		GST_DEBUG_OBJECT(aacdec, "Re init ipp aac decoder, ret %d", ret);
	}
	return ret;
}

static GstFlowReturn 
aac_init(GstAACDec *aacdec)
{
	if(aacdec->raw){
		aacdec->retCode = _aac_init_raw(aacdec);
	}else{
		aacdec->retCode = _aac_init_nonraw(aacdec, aacdec->curr_buf.pdata, aacdec->curr_buf.size);
		
	}		
	if (aacdec->retCode != IPP_STATUS_INIT_OK) {
		GST_ERROR_OBJECT (aacdec, "aac codec init error");

		aacdec->pAACDecState = NULL;
		return GST_FLOW_ERROR;
	}	
	aacdec->streamBegin = TRUE;		
	aacdec->totalFrames = 0;

	if(aacdec->bIsMultiChanDownMix) {
		IppCodecStatus ret;
		int mode = AAC_DOWNMIX_CHUNK;
		ret = DecodeSendCmd_AACAudio(IPPAC_AAC_SET_DOWNMIX_MODE, &mode, NULL, aacdec->pAACDecState);
		GST_DEBUG_OBJECT(aacdec, "DecodeSendCmd_AACAudio(IPPAC_AAC_SET_DOWNMIX_MODE...) ret %d\n", ret);
	}

	return GST_FLOW_OK;
}

/************************************************************************
* optimized output method, it will output OUTPUT_FRAME_NUMBER at one time to save thread switch time
* the less frame size, the more performance is optimized
************************************************************************/
static GstFlowReturn  _aac_out_data(GstAACDec *aacdec, int eosFlag)
{	
	int ret = 0;
	GstClockTime tmpDuration = 0;

	/*If reach the end of file and still have data in outBuf, output it. If no data, return */
	if(eosFlag){
		if(aacdec->framecnt)
			goto push;
		else
			return GST_FLOW_OK;
	}

	aacdec->framecnt ++;

	/*alloc outBuf,set timestamp and offset, reset duration of outBuf.*/
	if(aacdec->framecnt == 1)
	{
		GST_LOG_OBJECT (aacdec, "init new buffer info..");
		aacdec->outBuf = gst_buffer_new();
		GST_BUFFER_OFFSET (aacdec->outBuf) = gst_util_uint64_scale (aacdec->nextTS, aacdec->sound.sndSampleRate, GST_SECOND);
		GST_BUFFER_TIMESTAMP (aacdec->outBuf) = aacdec->nextTS;
		GST_BUFFER_DURATION (aacdec->outBuf) = 0;
		GST_BUFFER_SIZE (aacdec->outBuf) = 0;
		GST_BUFFER_DATA (aacdec->outBuf) = aacdec->pcmData;
		gst_buffer_set_caps (aacdec->outBuf, GST_PAD_CAPS (aacdec->srcpad));
	}


	/* Update codec output pointer and output buffer size */
	/* Codec need 4 byte aligned output buffer pointer. length may be none 4 byte aligned in seek case*/
	if(G_UNLIKELY(aacdec->sound.sndLen % 4)){
		GST_WARNING("none 4 byte aligned codec output! Ignore this buffer.");
		return GST_FLOW_OK;
	}
	{
		char *padd = aacdec->sound.pSndFrame;
		padd += aacdec->sound.sndLen;
		aacdec->sound.pSndFrame = (short*)padd;
		GST_BUFFER_SIZE (aacdec->outBuf) += aacdec->sound.sndLen;
	}

	/* Update duration info. */
	if(0 != aacdec->sound.sndSampleRate){
		tmpDuration = gst_util_uint64_scale ((aacdec->sound.sndLen >> 1), GST_SECOND, aacdec->sound.sndSampleRate*aacdec->sound.sndChannelNum);
		GST_BUFFER_DURATION (aacdec->outBuf) += tmpDuration; 
	}
	GST_OBJECT_LOCK (aacdec);
	aacdec->nextTS += tmpDuration;
	GST_OBJECT_UNLOCK (aacdec);


	/* If outBuf is not full, return. */
	if(aacdec->framecnt < OUTPUT_FRAME_NUMBER ) 
		return GST_FLOW_OK;

push:
	GST_LOG_OBJECT (aacdec, "pushing accumulated buffer..");
	/* reset codec output pointer to beginning of large buffer */
	aacdec->framecnt = 0;
	aacdec->sound.pSndFrame = aacdec->pcmData;

	//direct pushing buffer to downstream plugin doesn't obey Gstreaner rule, because you don't known when the sink finished using this data. So, memcpy is needed.
	
	GstBuffer* downbuf = gst_buffer_new_and_alloc(GST_BUFFER_SIZE(aacdec->outBuf));
	if(downbuf == NULL) {
		GST_BUFFER_DATA (aacdec->outBuf) = NULL;
		GST_BUFFER_SIZE (aacdec->outBuf) = 0;
		gst_buffer_unref(aacdec->outBuf);
		aacdec->outBuf = NULL;
		return GST_FLOW_ERROR;
	}
	GST_BUFFER_SIZE(downbuf) = GST_BUFFER_SIZE(aacdec->outBuf);
	GST_BUFFER_TIMESTAMP(downbuf) = GST_BUFFER_TIMESTAMP(aacdec->outBuf);
	GST_BUFFER_DURATION(downbuf) = GST_BUFFER_DURATION(aacdec->outBuf);
	memcpy(GST_BUFFER_DATA(downbuf), GST_BUFFER_DATA(aacdec->outBuf), GST_BUFFER_SIZE(downbuf));
	gst_buffer_set_caps (downbuf, GST_PAD_CAPS (aacdec->srcpad));
	GST_BUFFER_DATA (aacdec->outBuf) = NULL;
	GST_BUFFER_SIZE (aacdec->outBuf) = 0;
	gst_buffer_unref(aacdec->outBuf);
	aacdec->outBuf = NULL;
	if(aacdec->bFirstPushDownDataAfterNewseg != 0) {
		aacdec->bFirstPushDownDataAfterNewseg = 0;
		GST_BUFFER_FLAG_SET(downbuf, GST_BUFFER_FLAG_DISCONT);
	}
	ret = gst_pad_push (aacdec->srcpad, downbuf);

	return ret;
}

/************************************************************************
* this is old method, output every sameple
* easy to understand, you could choose accordingly to your requirement

static GstFlowReturn  _aac_out_data(GstAACDec *aacdec)
{	
						
	GstBuffer *outBuf = NULL;
	int ret = 0;

	if((1 == aacdec->sound.sndChannelNum) || (2 == aacdec->sound.sndChannelNum)){
		ret = gst_pad_alloc_buffer_and_set_caps (aacdec->srcpad, 0, 
			aacdec->sound.sndLen, GST_PAD_CAPS (aacdec->srcpad), &outBuf);
		if(ret != GST_FLOW_OK)
		{
			GST_ERROR_OBJECT (aacdec, "aac can not malloc buffer");
			
			return ret;								
		}							
		memcpy (GST_BUFFER_DATA (outBuf), aacdec->sound.pSndFrame, aacdec->sound.sndLen);

	}else{
	
		printf("channel number error channel=%d\n",aacdec->sound.sndChannelNum);
		return GST_FLOW_ERROR;								
	}	

	if(0 != aacdec->sound.sndSampleRate){	
		GST_BUFFER_OFFSET (outBuf) = gst_util_uint64_scale (aacdec->nextTS, aacdec->sound.sndSampleRate, GST_SECOND);
		GST_BUFFER_TIMESTAMP (outBuf) = aacdec->nextTS;
		GST_BUFFER_DURATION (outBuf) = gst_util_uint64_scale((aacdec->sound.sndLen >> 1) / aacdec->sound.sndChannelNum, GST_SECOND, aacdec->sound.sndSampleRate);
	}

	GST_OBJECT_LOCK (aacdec);
	aacdec->nextTS += GST_BUFFER_DURATION (outBuf);
	if(!aacdec->raw){		
		aacdec->decodedTime += GST_BUFFER_DURATION (outBuf);
	}
	GST_OBJECT_UNLOCK (aacdec);

	if(GST_FLOW_OK != (ret = gst_pad_push (aacdec->srcpad, outBuf)))
	{
		GST_ERROR_OBJECT (aacdec, "aac push error ret=%d\n",ret);				
		
		//gst_buffer_unref (outBuf);	
		
		return ret;
	}

	return GST_FLOW_OK;
	
}

************************************************************************/

static inline int
parse_id3tag_len(unsigned char* pData, int len)
{
	if(len < 10) {
		return 0;	//if data is too short, we consider there is no ID3 tag
	}else{
		if(pData[0] == 'I' && pData[1] == 'D' && pData[2] == '3') {
			int id3sz = ((int)pData[6]<<21) | ((int)pData[7]<<14) | ((int)pData[8]<<7) | ((int)pData[9]);
			return id3sz + 10;
		}else{
			return 0;
		}
	}
}


static __inline int
judge_stream_is_ADTS(unsigned char* pData, int len)
{
	if(len < 8) {
		return 0;
	}
	if(pData[0] == 0xff && (pData[1]>>4) == 0xf) {
		return 1;
	}
	return 0;
}

static unsigned char*
seek_0xfffx(unsigned char* pData, int len)
{
	unsigned int c;
	if(len < 2) {
		return NULL;
	}
	c = *pData++;
	len--;
	while(len > 0) {
		c = (c<<8) | (*pData++);
		len--;
		if((c&0xfff0) == 0xfff0) {
			return pData - 2;
		}
	}
	return NULL;
}

static int
analyse_adts_framelens(unsigned char* pData, int len)
{
	unsigned char* p;
	int len0 = len;
	int adts1framelen = 0, couldconsume = 0;
	unsigned char * p1stSync = NULL, * plastSync = NULL, * origin = pData;
	
	for(;;) {
		p = seek_0xfffx(pData, len);
		if(p == NULL || len - (p-pData) < 6) {
			break;
		}
		if(p1stSync == NULL) {
			p1stSync = p;
		}
		plastSync = p;
		adts1framelen = (((unsigned int)p[3]&0x3)<<11) | ((unsigned int)p[4]<<3) | ((unsigned int)p[5]>>5);
		if(adts1framelen < 8) {
			adts1framelen = 8;	//at least, adts length should > 7
		}
		len -= (p-pData) + adts1framelen;
		pData = p + adts1framelen;
	}

	if(plastSync == NULL) {
		return 0;
	}

	if((plastSync-origin) + adts1framelen == len0) {
		couldconsume = len0;
	}else if((plastSync-origin) + adts1framelen > len0) {
		if(p1stSync == plastSync) {	//only one un-complete adts frame in buf
			couldconsume = 0;
		}else{
			couldconsume = plastSync-origin;
		}
	}else{
		if(p != NULL) {
			//found sync code but only a part of header(< 6 bytes) occur in this buf
			couldconsume = p-origin;
		}else{
			couldconsume = origin[len0-1] == 0xff ? len0-1 : len0;
		}
	}

	return couldconsume;
}

static int
ADTS_form_frame(GstAACDec* aacdec, GstBuffer** inoutBuf)
{
	GstBuffer* buf = *inoutBuf;
	if(aacdec->ADTS_repackGstBuf) {
		GstClockTime ts = GST_BUFFER_TIMESTAMP(aacdec->ADTS_repackGstBuf);
		if(ts == GST_CLOCK_TIME_NONE) {
			ts = GST_BUFFER_TIMESTAMP(buf);
		}
		buf = gst_buffer_join(aacdec->ADTS_repackGstBuf, *inoutBuf);
		GST_BUFFER_TIMESTAMP(buf) = ts;
		GST_BUFFER_DURATION(buf) = GST_CLOCK_TIME_NONE;
		aacdec->ADTS_repackGstBuf = NULL;
	}

	int sz = GST_BUFFER_SIZE(buf);
	int couldconsume_sz = analyse_adts_framelens(GST_BUFFER_DATA(buf), sz);
	if(couldconsume_sz == 0) {
		aacdec->ADTS_repackGstBuf = buf;
		return 1;
	}
	if(couldconsume_sz == sz) {
		*inoutBuf = buf;
	}else{
		*inoutBuf = gst_buffer_create_sub(buf, 0, couldconsume_sz);
		GST_BUFFER_TIMESTAMP(*inoutBuf) = GST_BUFFER_TIMESTAMP(buf);
		GST_BUFFER_DURATION(*inoutBuf) = GST_CLOCK_TIME_NONE;
		aacdec->ADTS_repackGstBuf = gst_buffer_create_sub(buf, couldconsume_sz, sz - couldconsume_sz);
		GST_BUFFER_TIMESTAMP(aacdec->ADTS_repackGstBuf) = GST_CLOCK_TIME_NONE;
		GST_BUFFER_DURATION(aacdec->ADTS_repackGstBuf) = GST_CLOCK_TIME_NONE;
		gst_buffer_unref(buf);	//created two sub buffer, the buffer isn't needed.
	}
	return 0;
}

static GstFlowReturn 
gst_aacdec_chain (GstPad *pad, GstBuffer *buf)
{
	GstAACDec *aacdec = GST_AACDEC (GST_OBJECT_PARENT (pad));
	GstFlowReturn ret = GST_FLOW_OK;

	Ipp8u *frameBegin, *frameEnd;
	gboolean run_loop = TRUE;
	int consumed_len = 0;

	GST_LOG_OBJECT (aacdec, "input aac buffer size=%d", GST_BUFFER_SIZE (buf));
	if(aacdec->ltp == TRUE) {
		GST_ERROR_OBJECT(aacdec, "current ipp aac decoder don't support LTP");
		GST_ELEMENT_ERROR(aacdec, STREAM, DECODE, (NULL), ("current IPP aac dec don't support LTP"));
		ret = GST_FLOW_ERROR;
		goto chain_ret;
	}

	if(aacdec->iIsInEOS == 0) {
		if(aacdec->iID3TagLen == -1) {
			aacdec->iID3TagLen = parse_id3tag_len(GST_BUFFER_DATA(buf), GST_BUFFER_SIZE(buf));
		}
		if(aacdec->iID3TagLen > 0) {
			if(aacdec->iID3TagLen >= GST_BUFFER_SIZE(buf)) {
				//total buf data is ID3tag
				aacdec->iID3TagLen -= GST_BUFFER_SIZE(buf);
				gst_buffer_unref(buf);
				return GST_FLOW_OK;
			}else{
				GstBuffer* oldbuf = buf;
				buf = gst_buffer_new_and_alloc(GST_BUFFER_SIZE(oldbuf) - aacdec->iID3TagLen);
				memcpy(GST_BUFFER_DATA(buf), GST_BUFFER_DATA(oldbuf)+aacdec->iID3TagLen, GST_BUFFER_SIZE(buf));
				aacdec->iID3TagLen = 0;
				GST_BUFFER_TIMESTAMP(buf) = GST_BUFFER_TIMESTAMP(oldbuf);
				GST_BUFFER_DURATION(buf) = GST_BUFFER_DURATION(oldbuf);
				gst_buffer_unref(oldbuf);
			}
		}
	}

	if(aacdec->iIsInEOS == 0) {
		if(aacdec->ADTS_judge == -1) {
			aacdec->ADTS_judge = judge_stream_is_ADTS(GST_BUFFER_DATA(buf), GST_BUFFER_SIZE(buf));
			GST_DEBUG_OBJECT(aacdec, "judge adts frame result %d", aacdec->ADTS_judge);
		}
		if(aacdec->ADTS_judge == 1 && aacdec->InputFramed == FALSE) {
			if(ADTS_form_frame(aacdec, &buf) != 0) {
				//return non-0 means haven't form 1 or more complete ADTS frames, need more data
				return GST_FLOW_OK;
			}
		}
	}

	if (GST_BUFFER_TIMESTAMP (buf) != GST_CLOCK_TIME_NONE) {
		if (GST_BUFFER_TIMESTAMP (buf) != aacdec->lastTS) {
			aacdec->nextTS = GST_BUFFER_TIMESTAMP (buf);
			aacdec->lastTS = GST_BUFFER_TIMESTAMP (buf);
		}
	}

	 /* buffer + remaining data */
	if (aacdec->tempbuf) {
		GST_LOG_OBJECT (aacdec, "pack the previous buffer data");
		buf = gst_buffer_join (aacdec->tempbuf, buf);
		aacdec->tempbuf = NULL;
	}


	aacdec->curr_buf.pdata = GST_BUFFER_DATA (buf);	
	aacdec->curr_buf.size = GST_BUFFER_SIZE (buf);
	aacdec->curr_buf.offset = 0;
	aacdec->curr_buf.left = GST_BUFFER_SIZE (buf);

	aacdec->srcBitStream.pBsBuffer = GST_BUFFER_DATA (buf);	
	aacdec->srcBitStream.pBsCurByte = aacdec->srcBitStream.pBsBuffer;
	aacdec->srcBitStream.bsByteLen = GST_BUFFER_SIZE (buf);	
	aacdec->srcBitStream.bsCurBitOffset = 0;


	if (aacdec->raw) {
		/* Only one packet per buffer, no matter how much is really consumed */
		run_loop = FALSE;
	} 

	if (aacdec->pAACDecState == NULL) {
		GST_DEBUG_OBJECT (aacdec, "aac initing");
		if(GST_FLOW_ERROR == aac_init(aacdec)){
			GST_ELEMENT_ERROR(aacdec, STREAM, DECODE, (NULL), ("IPP aac init fail"));
			return GST_FLOW_ERROR;
		}
	}
	if (aacdec->pAACDecState != NULL) {
		do
		{
			if(TRUE == aacdec->seek && FALSE == aacdec->raw && aacdec->seek_support == TRUE){

				int NextSyncCode = 0;
				IppCodecStatus codecAPIret = IPP_STATUS_NOERR;
				aacdec->seek = FALSE;
				codecAPIret = DecodeSendCmd_AACAudio(IPPAC_AAC_FINDNEXT_SYNCWORD, &(aacdec->srcBitStream), (&NextSyncCode), aacdec->pAACDecState);

				aacdec->srcBitStream.pBsCurByte += NextSyncCode;
				if(aacdec->srcBitStream.pBsCurByte - aacdec->srcBitStream.pBsBuffer > aacdec->srcBitStream.bsByteLen){
					break;
				}
			}

			frameBegin = aacdec->srcBitStream.pBsCurByte;
			
#ifdef DEBUG_PERFORMANCE
			gettimeofday (&tstart, &tz);
#endif
			aacdec->retCode = Decode_AAC(&(aacdec->srcBitStream), &(aacdec->sound), aacdec->pAACDecState);
#ifdef DEBUG_PERFORMANCE

			gettimeofday (&tend, &tz);
			aacdec->codec_time += (tend.tv_sec - tstart.tv_sec)*1000000 + (tend.tv_usec - tstart.tv_usec);
#endif	
			switch (aacdec->retCode)
			{
			case IPP_STATUS_FRAME_COMPLETE:
				{
					GST_LOG_OBJECT (aacdec, "aac decode frame complete");
                                        aacdec->error_count = 0;  //clear this error count when one good frame appears
					if (G_UNLIKELY(aacdec->streamBegin)) {
						GstCaps *Tmp;
						aacdec->streamBegin = FALSE;

						GST_INFO_OBJECT (aacdec, "aacdec->sound.sndChannelNum=%d\n",aacdec->sound.sndChannelNum);
						GST_INFO_OBJECT (aacdec, "aacdec->sound.sndSampleRate=%d\n",aacdec->sound.sndSampleRate);

                        			/* set srcpad caps */
						Tmp = gst_caps_new_simple ("audio/x-raw-int", 
							"endianness", G_TYPE_INT, G_BYTE_ORDER, 
							"signed", G_TYPE_BOOLEAN, TRUE, 
							"width", G_TYPE_INT, 16, 
							"depth", G_TYPE_INT, 16, 
							"rate", G_TYPE_INT, aacdec->sound.sndSampleRate, 
							"channels", G_TYPE_INT, aacdec->sound.sndChannelNum, 
							NULL);
						gst_pad_set_caps(aacdec->srcpad, Tmp);
						gst_caps_unref(Tmp);
						
						/* realloc pcm output buffer to repack output into large buffer */
						if(aacdec->bIsMultiChanDownMix == 0) {
							IPP_MemMalloc(&(aacdec->pcmData), aacdec->sound.sndLen * OUTPUT_FRAME_NUMBER, 4);
							GST_DEBUG_OBJECT (aacdec, "alloc repack buf size=%d\n", aacdec->sound.sndLen * OUTPUT_FRAME_NUMBER);
						}else{
							//for multi channel, though codec did down-mix, codec still request the output buffer is multi-channel size. It's codec limitation.
							int sz = aacdec->sound.sndLen * (OUTPUT_FRAME_NUMBER-1) + aacdec->sound.sndLen * (6/2);//currently, we only support 6 channel at most
							IPP_MemMalloc(&(aacdec->pcmData), sz, 4);
							GST_DEBUG_OBJECT (aacdec, "6 channel downmix, alloc repack buf size=%d\n", sz);
						}
						
						if(!aacdec->pcmData){
							GST_ERROR_OBJECT (aacdec, "alloc repack buf error!\n");
							ret = GST_FLOW_ERROR;
							goto chain_ret;
						}
						memcpy (aacdec->pcmData, aacdec->sound.pSndFrame, aacdec->sound.sndLen);
						IPP_MemFree(&(aacdec->sound.pSndFrame));
						aacdec->sound.pSndFrame = aacdec->pcmData;
					}

					/* Update src bitstream info. */
					frameEnd = aacdec->srcBitStream.pBsCurByte;

					if(!aacdec->raw){
						GST_OBJECT_LOCK (aacdec);
						aacdec->decodedBytes += frameEnd - frameBegin;
						if(0 != aacdec->sound.sndSampleRate) {
							aacdec->decodedTime += gst_util_uint64_scale((aacdec->sound.sndLen >> 1), GST_SECOND, aacdec->sound.sndSampleRate*aacdec->sound.sndChannelNum);
						}
						GST_OBJECT_UNLOCK (aacdec);
					}

					consumed_len = aacdec->srcBitStream.pBsCurByte - aacdec->srcBitStream.pBsBuffer;

					aacdec->curr_buf.offset = consumed_len;
					aacdec->curr_buf.left = aacdec->curr_buf.size-consumed_len;

					if(GST_FLOW_OK != (ret=_aac_out_data(aacdec, 0))){

						goto chain_ret;
					}

					aacdec->totalFrames ++;
				}
				break;

			case IPP_STATUS_BUFFER_UNDERRUN:
			case IPP_STATUS_SYNCNOTFOUND_ERR:

				run_loop = FALSE;

				 /* Keep the leftovers in raw stream */
				if (aacdec->curr_buf.left > 0 && !aacdec->raw) {

					if (aacdec->curr_buf.left < aacdec->curr_buf.size) {
						aacdec->tempbuf = gst_buffer_create_sub (buf, aacdec->curr_buf.offset, aacdec->curr_buf.left );
					} else {
						aacdec->tempbuf = buf;
						gst_buffer_ref (buf);
					}
					GST_LOG_OBJECT (aacdec, "aac left data is kept for repack");
				}

				break;

			case IPP_STATUS_FRAME_ERR:
			case IPP_STATUS_NOTSUPPORTED:
				run_loop = FALSE;

				aacdec->error_count++;
				GST_ERROR_OBJECT(aacdec, "aacdec->error_count=%d, ret %d",aacdec->error_count, aacdec->retCode);
				if (aacdec->error_count >= MAX_DECODE_ERRORS){
					GST_ELEMENT_ERROR(aacdec, STREAM, DECODE, (NULL), ("IPP Decode_AAC() return continuous error exceeds %d times", MAX_DECODE_ERRORS));
					ret = GST_FLOW_ERROR;
					goto chain_ret;
				}

				break;

			default:
				run_loop = FALSE;
				GST_ELEMENT_ERROR(aacdec, STREAM, DECODE, (NULL), ("IPP Decode_AAC() return un-expected error %d", aacdec->retCode));
				ret = GST_FLOW_ERROR;
				goto chain_ret;
			}		
		}while((aacdec->retCode == IPP_STATUS_FRAME_COMPLETE) && run_loop);
	}

chain_ret:
	gst_buffer_unref(buf);
	return ret;
}

static gboolean 
gst_aacdec_sinkpad_event (GstPad *pad, GstEvent *event)
{
	GstAACDec *aacdec = GST_AACDEC (GST_OBJECT_PARENT (pad));
	gboolean eventRet;

	GST_DEBUG_OBJECT(aacdec, "aac get event(%s) from upstream element's pad\n", GST_EVENT_TYPE_NAME(event));

	switch (GST_EVENT_TYPE (event))
	{
	case GST_EVENT_FLUSH_STOP:
		if(aacdec->ADTS_repackGstBuf) {
			gst_buffer_unref(aacdec->ADTS_repackGstBuf);
			aacdec->ADTS_repackGstBuf = NULL;
		}
		//give up those candidate push down buffer data
		aacdec->framecnt = 0;
		aacdec->sound.pSndFrame = aacdec->pcmData;
		if(aacdec->outBuf) {
			GST_BUFFER_DATA (aacdec->outBuf) = NULL;
			GST_BUFFER_SIZE (aacdec->outBuf) = 0;
			gst_buffer_unref(aacdec->outBuf);
			aacdec->outBuf = NULL;
		}
		aacdec->lastTS = GST_CLOCK_TIME_NONE;
		aacdec->nextTS = 0;

		eventRet = gst_pad_push_event (aacdec->srcpad, event);
		break;
	case GST_EVENT_NEWSEGMENT:
	{
		GstFormat fmt;
		gboolean is_update;
		gint64 start, end, base;
		gdouble rate;

		aacdec->bFirstPushDownDataAfterNewseg = 1;

		//in fact, following code has been done in GST_EVENT_FLUSH_STOP
		if(aacdec->ADTS_repackGstBuf) {
			gst_buffer_unref(aacdec->ADTS_repackGstBuf);
			aacdec->ADTS_repackGstBuf = NULL;
		}
		//give up those candidate push down buffer data
		aacdec->framecnt = 0;
		aacdec->sound.pSndFrame = aacdec->pcmData;
		if(aacdec->outBuf) {
			GST_BUFFER_DATA (aacdec->outBuf) = NULL;
			GST_BUFFER_SIZE (aacdec->outBuf) = 0;
			gst_buffer_unref(aacdec->outBuf);
			aacdec->outBuf = NULL;
		}
		aacdec->lastTS = GST_CLOCK_TIME_NONE;
		aacdec->nextTS = 0;

		GST_INFO_OBJECT (aacdec, "New segment event. parsing");
		gst_event_parse_new_segment (event, &is_update, &rate, &fmt, &start,
		  &end, &base);
		if (fmt == GST_FORMAT_TIME) {
			if(start >= 0) {
				aacdec->nextTS = start;
			}

		} else if (fmt == GST_FORMAT_BYTES) {
		  
			gint64 new_start = 0;
			gint64 new_end = -1;

			/* bytes -> time */
			GST_INFO_OBJECT (aacdec, "New seg: Converting byte to time");
			if ((!_aac_convert_data (aacdec, GST_FORMAT_BYTES, start,GST_FORMAT_TIME, &new_start))
			 	|| (!_aac_convert_data (aacdec, GST_FORMAT_BYTES, end,GST_FORMAT_TIME, &new_end))) {
				new_start = 0;
				new_end = -1;				
			}
			gst_event_unref (event);
			event = gst_event_new_new_segment (is_update, rate,
				GST_FORMAT_TIME, new_start, new_end, new_start);	

			GST_OBJECT_LOCK (aacdec);
			aacdec->nextTS = new_start;
			aacdec->lastTS = GST_CLOCK_TIME_NONE;
			GST_OBJECT_UNLOCK (aacdec);


		}

		if (NULL != aacdec->pAACDecState)
		{
			if (aacdec->tempbuf != NULL) {
				gst_buffer_unref (aacdec->tempbuf);
				aacdec->tempbuf = NULL;
			}

			/*handle curr_buf*/
			aacdec->curr_buf.left = 0;
			aacdec->curr_buf.offset= 0;
			aacdec->srcBitStream.bsCurBitOffset = 0;

		}
		eventRet = gst_pad_push_event (aacdec->srcpad, event);

		break;
	}
	case GST_EVENT_EOS:
		if(aacdec->ADTS_repackGstBuf) {
			if(aacdec->tempbuf == NULL) {
				aacdec->tempbuf = aacdec->ADTS_repackGstBuf;
			}else{
				aacdec->tempbuf = gst_buffer_join(aacdec->tempbuf, aacdec->ADTS_repackGstBuf);
			}
			aacdec->ADTS_repackGstBuf = NULL;
		}

		if (aacdec->tempbuf != NULL) {
			GST_INFO_OBJECT (aacdec, "EOS event. Re-call chain to output all data for codec bug");

			aacdec->iIsInEOS = 1;
			gst_aacdec_chain(pad, gst_buffer_new());
			aacdec->iIsInEOS = 0;

			if (aacdec->tempbuf != NULL){
				 gst_buffer_unref (aacdec->tempbuf);
				aacdec->tempbuf = NULL;
			}
		}
		_aac_out_data(aacdec, 1);
		
		GST_DEBUG_OBJECT (aacdec, "Decoding Over, Total %5d frames", aacdec->totalFrames);
		eventRet = gst_pad_push_event (aacdec->srcpad, event);
		break;
	default:
		eventRet = gst_pad_event_default (pad, event);
		break;
	}

	return eventRet;
}

static gboolean
gst_aacdec_start (GstAACDec *aacdec)
{
	aacdec->decodedBytes = 0;
	aacdec->decodedTime = 0;	
	aacdec->totalFrames = 0;
	aacdec->nextTS = 0;
	aacdec->lastTS = GST_CLOCK_TIME_NONE;
	
	aacdec->codec_time = 0;	


	aacdec->raw = FALSE;
	aacdec->streamBegin = TRUE;
	aacdec->ltp = FALSE;

	aacdec->version = 0;

	aacdec->error_count = 0;
	
	/*handle curr_buf*/
	aacdec->curr_buf.left = 0;
	aacdec->curr_buf.offset= 0;
	
	aacdec->tempbuf = NULL;
	aacdec->seek = FALSE;
	aacdec->seek_support = TRUE;	

	aacdec->pAACDecState = NULL;
	//aacdec->aacdecConfig.frameLengthFlag = GST_AACDEC_1024BYTE_FRAME;	

	miscInitGeneralCallbackTable (&(aacdec->pCBTable));

	/* AAC optimization library introduce assembly, requires output buffer 4 bytes align */
	//IPP_MemMalloc(&(aacdec->pcmData), OUTPUT_BUF_SIZE_AAC * 2, 4);	//for aac output one frame, if no SBR, the output data size at most is 1024*2(16bit pcm)*2(channels) = 4096 = OUTPUT_BUF_SIZE_AAC, if has SBR, double.
	IPP_MemMalloc(&(aacdec->pcmData), OUTPUT_BUF_SIZE_AAC * 2 * (6/2), 4);	//to support 6 channels, allocate 3 multiple size of 2 channels buffer
	aacdec->sound.pSndFrame = aacdec->pcmData;
	//memset(aacdec->sound.pSndFrame, 0x00, OUTPUT_BUF_SIZE_AAC * 2);
	memset(aacdec->sound.pSndFrame, 0x00, OUTPUT_BUF_SIZE_AAC * 2 * (6/2));

	aacdec->sound.sndLen = 0;
	
	aacdec->framecnt = 0;
	aacdec->outBuf = NULL;
	aacdec->lastBuf = NULL;   /* the subBuffer containing the last frames */

	return TRUE;
}

static gboolean
gst_aacdec_stop (GstAACDec *aacdec)
{
	if (aacdec->pAACDecState != NULL) {
		DecoderFree_AAC (&(aacdec->pAACDecState));
	}

	 if (aacdec->tempbuf != NULL) {
		gst_buffer_unref (aacdec->tempbuf);
		aacdec->tempbuf = NULL;
	}
	
	if(aacdec->pcmData) IPP_MemFree(&(aacdec->pcmData));
	if(aacdec->pCBTable) miscFreeGeneralCallbackTable (&(aacdec->pCBTable));

	//aacdec->totalFrames = 0;
	aacdec->nextTS = 0;
	aacdec->lastTS = GST_CLOCK_TIME_NONE;

	/*handle curr_buf*/
	aacdec->curr_buf.left = 0;
	aacdec->curr_buf.offset= 0;
	aacdec->ltp = FALSE;

	aacdec->framecnt = 0;
	if(aacdec->outBuf) {
		GST_BUFFER_DATA (aacdec->outBuf) = NULL;
		GST_BUFFER_SIZE (aacdec->outBuf) = 0;
		gst_buffer_unref(aacdec->outBuf);
		aacdec->outBuf = NULL;
	}

	if(aacdec->ADTS_repackGstBuf) {
		gst_buffer_unref(aacdec->ADTS_repackGstBuf);
		aacdec->ADTS_repackGstBuf = NULL;
	}

	return TRUE;
	
}



static GstStateChangeReturn 
gst_aacdec_change_state (GstElement *element, GstStateChange transition)
{
	GstAACDec *aacdec = GST_AACDEC (element);
	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
	GST_DEBUG_OBJECT(aacdec, "aac state change %d to %d\n", transition>>3, transition&7);

	switch (transition)
	{
	case GST_STATE_CHANGE_NULL_TO_READY:

		//if(!gst_aacdec_start(aacdec)){
			//goto statechange_failed;
		//}
		
		break;
	case GST_STATE_CHANGE_READY_TO_PAUSED:
		if(!gst_aacdec_start(aacdec)){
			goto statechange_failed;
                }
                //ready to decode a new stream
		aacdec->iID3TagLen = -1;
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
		
		//if(!gst_aacdec_stop(aacdec)){
			//goto statechange_failed;
		//}

		break;		
        case GST_STATE_CHANGE_PAUSED_TO_READY:
            GST_LOG_OBJECT (aacdec, "state change from Paused to Ready ");
            if(!gst_aacdec_stop(aacdec)){
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
		GST_ERROR_OBJECT (aacdec, "state change failed");
		return GST_STATE_CHANGE_FAILURE;
	}

}

static void 
gst_aacdec_base_init (gpointer g_klass)
{
	static GstElementDetails aacdec_details = {
		"AAC Audio Decoder", 
		"Codec/Decoder/Audio", 
		"AAC Audio Decoder based-on IPP & CODEC", 
		""
	};
	GstElementClass *element_class = GST_ELEMENT_CLASS (g_klass);


	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));

	gst_element_class_set_details (element_class, &aacdec_details);
}

static void 
gst_aacdec_class_init (GstAACDecClass *klass)
{
	GObjectClass *gobject_class = (GObjectClass*) klass;
	GstElementClass *gstelement_class = (GstElementClass*) klass;

	parent_class = g_type_class_peek_parent(klass);
	gobject_class->set_property = GST_DEBUG_FUNCPTR (gst_aacdec_set_property);
	gobject_class->get_property = GST_DEBUG_FUNCPTR (gst_aacdec_get_property);

	//version property is legacy, we don't support it any more. 2011.04.18
	//g_object_class_install_property (gobject_class, ARG_VERSION, 
	//	g_param_spec_int ("version", "version of aac, version 1 is aac+, version 2 is aac++", 
	//	"version of aac, 1 is aac+ or HEAAC, 2 is aac++ or (H)EAAC+", 
	//	0/* range_MIN */, 2/* range_MAX */, 0/* default_INIT */, G_PARAM_READWRITE));

	
#ifdef DEBUG_PERFORMANCE	
	g_object_class_install_property (gobject_class, ARG_TOTALFRAME,
  		g_param_spec_int ("totalframes", "Totalframe of aacstream",
          	"Number of frame", 0, G_MAXINT, 0, G_PARAM_READABLE));
	g_object_class_install_property (gobject_class, ARG_CODECTIME,
  		g_param_spec_int64 ("codectime", "aac decode time",
          	"codec time (microsecond)", 0, G_MAXINT64, 0, G_PARAM_READABLE));
#endif

	gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_aacdec_change_state);

	GST_DEBUG_CATEGORY_INIT (aacdec_debug, "aacdec", 0, "AAC Decode Element");
}


static void 
gst_aacdec_init (GstAACDec *aacdec, GstAACDecClass *aacdec_klass)
{
	GstElementClass *klass = GST_ELEMENT_CLASS (aacdec_klass);


	aacdec->sinkpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template (klass, "sink"), "sink");

	gst_pad_set_setcaps_function (aacdec->sinkpad, GST_DEBUG_FUNCPTR (gst_aacdec_sinkpad_setcaps));
	gst_pad_set_event_function (aacdec->sinkpad, GST_DEBUG_FUNCPTR (gst_aacdec_sinkpad_event));
	gst_pad_set_chain_function (aacdec->sinkpad, GST_DEBUG_FUNCPTR (gst_aacdec_chain));

	aacdec->srcpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template (klass, "src"), "src");

	gst_pad_set_event_function(aacdec->srcpad, GST_DEBUG_FUNCPTR (gst_aacdec_srcpad_event));
	gst_pad_set_query_function(aacdec->srcpad, GST_DEBUG_FUNCPTR (gst_aacdec_srcpad_query));


	gst_element_add_pad (GST_ELEMENT (aacdec), aacdec->sinkpad);
	gst_element_add_pad (GST_ELEMENT (aacdec), aacdec->srcpad);

	aacdec->version_config = 0;

	aacdec->bFirstPushDownDataAfterNewseg = 0;

	aacdec->InputFramed = FALSE;
	aacdec->ADTS_judge = -1;
	aacdec->ADTS_repackGstBuf = NULL;
	aacdec->iIsInEOS = 0;
	aacdec->bIsMultiChanDownMix = 0;
	aacdec->bIsADIF = 0;

	aacdec->gTotalDuration = 0;
	aacdec->gTotalLength = 0;
}

static gboolean 
plugin_init (GstPlugin *plugin)
{	
	return gst_element_register (plugin, "aacdec", GST_RANK_PRIMARY+2, GST_TYPE_AACDEC);
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR, 
                   GST_VERSION_MINOR, 
                   "mvl_aacdec", 
                   "aacdec decoder based on IPP, "__DATE__, 
                   plugin_init, 
                   VERSION, 
                   "LGPL", 
                   "", 
                   "");




/* EOF */
