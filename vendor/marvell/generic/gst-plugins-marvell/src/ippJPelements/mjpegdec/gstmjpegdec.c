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

#define mydebug 0

#if    mydebug  == 3
#define key_debug_printf(...)	 {printf(__VA_ARGS__);fflush(stdout);}
#define debug_printf                key_debug_printf
#define debug_dump_bufmap key_dump_bufmap

#elif    mydebug  == 2
#define key_debug_printf(...)	 {printf(__VA_ARGS__);fflush(stdout);}
#define debug_printf                key_debug_printf
#define debug_dump_bufmap(...)

#elif mydebug == 1
#define key_debug_printf(...)	 {printf(__VA_ARGS__);fflush(stdout);}
#define debug_printf(...)
#define debug_dump_bufmap(...)

#elif mydebug == 0
#define key_debug_printf(...)
#define debug_printf(...)
#define debug_dump_bufmap(...)
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstmjpegdec.h"

#include <string.h>	//to include memcpy() memset()


GST_DEBUG_CATEGORY_STATIC (mjpegdec_debug);
#define GST_CAT_DEFAULT mjpegdec_debug


#include  <sys/time.h>

//#define LOG_RAWIPPDECODER_PLUGIN_TIME
#ifdef LOG_RAWIPPDECODER_PLUGIN_TIME
static int T_JDecCostSum_usec = 0;
static struct timeval Clk_WorkingStart;
static struct timeval Clk_WorkingStop;
#endif

/********************************************************************************************
 *Discription: The log_skip_frame is define for log the frame skip result.
 *param1: flag. 0: output '0' to the log; 1: output '1' to the log; other:close the file pointer.
 *return: void;
*******************************************************************************************/
#ifdef LOG_SKIP_FRAME
static void log_skip_frame(int flag)
{

    static FILE *fp = NULL;

    if (NULL == fp)
    {
        if ((fp = fopen("skip_frame.log", "w")) == NULL)
        {
            return;
        }

        fprintf(fp, "This file record the result of frame skip, 1 means one frame have been skiped, 0 means have not. \n");
    }

    switch(flag)
    {
        case 0:
            fputc('0', fp);
            break;
        case 1:
            fputc('1', fp);
            break;
        default:
            fclose(fp);
            fp = NULL;
            break;
    }
}
#endif

/******************************************************************************
 *Marvell IPP Powered function. 
******************************************************************************/

int JPEGSubSamplingTab[][2] = 
{
    /* width, height */
    {0, 0}, /* 444  */
    {1, 0}, /* 422, horizontal sub-sampling  */
    {0, 1}, /* 422I,vertical sub-sampling */
    {1, 1}, /* 411  */
};


static const int SkipFixPatterns[] = {
    0x0,			//skip rate 0
    0x11111111,		//skip rate 1/4, bit "1" means skip, bit "0" means no skip
    0x24924924,		//skip rate about 1/3
    0x55555555,		//skip rate 1/2
    0x6db6db6d,		//skip rate about 2/3
};

static inline int
decide_skippattern(int w, int h, IppJPEGColorFormat srcformat, int skip_strategy)
{
    int size;
    if(skip_strategy >= 3) {
        return skip_strategy-2;
    }
    if((size=w*h) <= 640*480) {
        return 0;
    }else if(size <= 744*480) {
        return 1;
    }else if(size <= 848*480) {
        return 2;
    }else if(size <= 1280*720) {
        return 3;
    }else{
        return 4;
    }
}


#define ADJUST_DEMUXCOST_USEC 1000
static inline void
estimate_spendtime(GstMJPEGDec *mjpeg)
{
#if 0
    //we use a very rough method to estimate spend time of decode/demux one frame
    if(G_UNLIKELY(mjpeg->est_demuxdecCost_usec == 0)) {	//0 means it's the first frame
        mjpeg->est_demuxdecCost_usec = ((int)mjpeg->duration)/1000;
    }
#else
    if(G_UNLIKELY(mjpeg->est_demuxdecCost_usec == 0)) {
        mjpeg->est_demuxdecCost_usec = 1;//((int)mjpeg->duration)/1000;
    }else{
        mjpeg->est_demuxdecCost_usec = mjpeg->t_decCost_usec_avg + ADJUST_DEMUXCOST_USEC;
    }
#endif
}


//ADJUST_MAXLATENESS_NSEC is just for adjust, default max-lateness(20000000) of videosink has been considered
#define	ADJUST_MAXLATENESS_NSEC		11500000
static inline int
should_skip(GstMJPEGDec* mjpeg, GstBuffer* buffer)
{
    if(mjpeg->skip_strategy == 0 || mjpeg->is_playing == 0 ) {
        //only skip when the GST decoder plugin is in the state "play"
        return 0;
    }else{
        if(mjpeg->skip_strategy >= 2) {
            if(mjpeg->skip_patternidx == 0) {
                return 0;
            }else{
                return ((SkipFixPatterns[mjpeg->skip_patternidx]) >> (mjpeg->frameSerialNum_in1stream & 0x1f)) & 1;
            }
        }else{
            //skip_strategy == 1
            if(G_LIKELY(mjpeg->TS_exitSink != 0)) {
                return ((gint64)(mjpeg->TS_exitSink+(mjpeg->est_demuxdecCost_usec*1000)-ADJUST_MAXLATENESS_NSEC) > (gint64)(GST_BUFFER_TIMESTAMP(buffer)));
            }else{
                return 0;
            }
        }
    }
}

/*****************************************************************************/

/******************************************************************************
// IPP-powered GStreamer MJPEG Decode Plug-in Implementations
******************************************************************************/

enum {

    /* fill above with user defined signals */
    LAST_SIGNAL
};

enum {
    ARG_0,

    ARG_FORCE422TO420,
    ARG_SKIP_STRATEGY,/* frame skip strategy during the decoding */
    ARG_SCALE, /* scale ratio */

#ifdef DEBUG_PERFORMANCE
    ARG_TOTALFRAME,
    ARG_CODECTIME,
#endif
};


#define MAX_INPUT_WIDTH 4096
#define MAX_INPUT_HEIGHT 4096
#define MIN_INPUT_WIDTH 16
#define MIN_INPUT_HEIGHT 16
#define MAX_OUTPUT_WIDTH 2048
#define MAX_OUTPUT_HEIGHT 2048
#define MIN_OUTPUT_WIDTH 16
#define MIN_OUTPUT_HEIGHT 16

static GstStaticPadTemplate sink_factory = \
    GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK, GST_PAD_ALWAYS, \
            GST_STATIC_CAPS ("image/jpeg, width = (int) [" G_STRINGIFY(MIN_INPUT_WIDTH) "," G_STRINGIFY(MAX_INPUT_WIDTH) "], height = (int) [" G_STRINGIFY(MIN_INPUT_HEIGHT) "," G_STRINGIFY(MAX_INPUT_HEIGHT) "];"
                "video/x-jpeg, width = (int) [" G_STRINGIFY(MIN_INPUT_WIDTH) "," G_STRINGIFY(MAX_INPUT_WIDTH) "], height = (int) [" G_STRINGIFY(MIN_INPUT_HEIGHT) "," G_STRINGIFY(MAX_INPUT_HEIGHT) "]"));

/*As I know, no fourcc registered for jpeg 422I planner format until 2009.8.11. Using J42I is just a temporary and internal only solution*/
static GstStaticPadTemplate src_factory = 
    GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS,
        GST_STATIC_CAPS ("video/x-raw-yuv, "
            "format = (fourcc) { I420, Y42B, J42I, Y444 },"
            "width = (int) [" G_STRINGIFY(MIN_OUTPUT_WIDTH) "," G_STRINGIFY(MAX_OUTPUT_WIDTH) "],"
            "height = (int) [" G_STRINGIFY(MIN_OUTPUT_HEIGHT) "," G_STRINGIFY(MAX_OUTPUT_HEIGHT) "],"
            "framerate = (fraction) [ 0, MAX ]"));


/******************************************************************************
// GStreamer plug-in functions
******************************************************************************/

GST_BOILERPLATE (GstMJPEGDec, gst_mjpegdec, GstElement, GST_TYPE_ELEMENT);

static void gst_mjpegdec_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void gst_mjpegdec_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);

static gboolean gst_mjpegdec_sinkpad_event (GstPad *pad, GstEvent *event);

static gboolean gst_mjpegdec_srcpad_event (GstPad *pad, GstEvent *event);

static gboolean gst_mjpegdec_setcaps (GstPad *pad, GstCaps *caps);
static GstFlowReturn gst_mjpegdec_chain (GstPad *pad, GstBuffer *buf);

static GstStateChangeReturn gst_mjpegdec_change_state (GstElement *element, GstStateChange transition);

static void
gst_mjpegdec_base_init (gpointer klass)
{
    debug_printf("enter %s \n", __FUNCTION__);
    static GstElementDetails mjpegdec_details = {
        "MJPEG Decoder",
        "Codec/Decoder/Image",	/* ??? or "Codec/Decoder/Video", ffdec_mjpeg use Codec/Decoder/Video but libgstjpeg.so use  Codec/Decoder/Image*/
        "MJPEG Decoder based-on IPP & CODEC",
        ""
    };
    GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

    /* create pad templates */
    gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));
    gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));

    /* set JPEGDEC element's details */
    gst_element_class_set_details (element_class, &mjpegdec_details);
}

static void gst_mjpegdec_class_init (GstMJPEGDecClass *klass)
{
    debug_printf("enter %s \n", __FUNCTION__);
    GObjectClass *gobject_class = (GObjectClass *) klass;
    GstElementClass *gstelement_class = (GstElementClass *) klass;

    //parent_class = g_type_class_ref (GST_TYPE_ELEMENT);	//has been done in GST_BOILERPLATE

    gobject_class->set_property = gst_mjpegdec_set_property;
    gobject_class->get_property = gst_mjpegdec_get_property;

    g_object_class_install_property (gobject_class, ARG_FORCE422TO420, \
            g_param_spec_int ("force-422to420", "enable yuv422 to yuv420 conversion", \
                "force convert yuv422 plannar(Y42B) to yuv420 plannar(I420), could be 1<convert>, 0<not convert>", \
                0/* range_MIN */, 1/* range_MAX */, 1/* default_INIT */, G_PARAM_READWRITE));

    /* add skip-frame argument to the element */
    g_object_class_install_property (gobject_class, ARG_SKIP_STRATEGY, \
            g_param_spec_int ("frameskip-strategy", "set strategy of frame skipping", \
                "frame skipping strategy index (could be 0<no skip>, 1<auto skip based on realtime measure>, 2<skip based on frame size>, 3<fixed skip rate=1/4>, 4<skip rate=1/3>, 5<skip rate=1/2>, 6<skip rate=2/3>)", \
                0/* range_MIN */, 6/* range_MAX */, 0/* default_INIT */, G_PARAM_READWRITE));

    /* add scale argument to the element */
    g_object_class_install_property (gobject_class, ARG_SCALE, \
            g_param_spec_int ("scale-ratio", "set size scale ratio", \
                "could be 1*65536<no scale>, 2*65536<width and height down size 1/2>, 4*65536<width and height down size 1/4>...", \
                0/* range_MIN */, G_MAXINT/* range_MAX */, 65536/* default_INIT */, G_PARAM_READWRITE));

#ifdef DEBUG_PERFORMANCE
    g_object_class_install_property (gobject_class, ARG_TOTALFRAME,
            g_param_spec_int ("totalframes", "Number of frame",
                "Number of total decoded frames for all tracks", 0, G_MAXINT, 0, G_PARAM_READABLE));
    g_object_class_install_property (gobject_class, ARG_CODECTIME,
            g_param_spec_int64 ("codectime", "codec time (microsecond)",
                "Total pure decoding spend system time for all tracks (microsecond)", 0, G_MAXINT64, 0, G_PARAM_READABLE));
#endif

    gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_mjpegdec_change_state);

    GST_DEBUG_CATEGORY_INIT (mjpegdec_debug, "mjpegdec", 0, "IPP MJPEG Decoder Element");
}

static inline void
gst_mjpegdec_init_members(GstMJPEGDec *mjpeg)
{
    /* IPP JPEG decoder specifics initial */
    mjpeg->pJPEGDecState = NULL;
    mjpeg->pCBTable = NULL;

    memset(&(mjpeg->jpegDecPar), 0, sizeof(IppJPEGDecoderParam));
    mjpeg->outGstBufDataFormat = JPEG_YUV411;
    mjpeg->jpegDecPar.nDesiredColor = JPEG_YUV411;
    mjpeg->jpegDecPar.nScale = 65536; /* no scale */
    mjpeg->jpegDecPar.nAlphaValue = 255;
    mjpeg->outputWidth = 0;
    mjpeg->outputHeight = 0;
    mjpeg->jpegDecPar.srcROI.x      = 0;
    mjpeg->jpegDecPar.srcROI.y      = 0;
    mjpeg->jpegDecPar.srcROI.width  = MAX_INPUT_WIDTH;
    mjpeg->jpegDecPar.srcROI.height = MAX_INPUT_HEIGHT;

    mjpeg->bCapSet = 0;

    mjpeg->nextTS = GST_CLOCK_TIME_NONE;
    mjpeg->duration = GST_CLOCK_TIME_NONE;
    mjpeg->iFRateNum = 0;
    mjpeg->iFRateDen = 1;

    mjpeg->bJpegDstBufAllocatedBySelf = 0;

    mjpeg->bForce_422to420 = 1;

    mjpeg->skip_strategy = 1;
    mjpeg->is_playing = 0;
    mjpeg->is_newSeg = 1;
    mjpeg->frameSerialNum_in1stream = 0;
    mjpeg->skip_patternidx = -1;

    mjpeg->TS_exitSink = 0;
    mjpeg->est_demuxdecCost_usec = 0;
    mjpeg->t_decCost_usec = 0;
    mjpeg->t_decCost_usec_avg = 0;
#ifdef LOG_MJPEGDEC_INFO
    mjpeg->skipframecnt_in1stream = 0;
#endif

#ifdef DEBUG_PERFORMANCE
    mjpeg->totalFrames = 0;
    mjpeg->codec_time = 0;
#endif

    return;
}


static void gst_mjpegdec_init (GstMJPEGDec *mjpeg, GstMJPEGDecClass *klass)
{
    debug_printf("enter %s \n", __FUNCTION__);
    mjpeg->sinkpad = gst_pad_new_from_template (\
            gst_element_class_get_pad_template ((GstElementClass*)klass, "sink"), "sink");

    /* sinkpad specifying link & caps function pointers */
    gst_pad_set_setcaps_function (mjpeg->sinkpad, GST_DEBUG_FUNCPTR(gst_mjpegdec_setcaps));
    gst_pad_set_event_function(mjpeg->sinkpad, GST_DEBUG_FUNCPTR(gst_mjpegdec_sinkpad_event));
    gst_pad_set_chain_function (mjpeg->sinkpad, GST_DEBUG_FUNCPTR(gst_mjpegdec_chain));
    gst_element_add_pad (GST_ELEMENT (mjpeg), mjpeg->sinkpad);


    mjpeg->srcpad = gst_pad_new_from_template (\
            gst_element_class_get_pad_template ((GstElementClass*)klass, "src"), "src");
    /* srcpad specifying link & caps function pointers */

    gst_pad_set_event_function(mjpeg->srcpad, GST_DEBUG_FUNCPTR(gst_mjpegdec_srcpad_event));
    gst_element_add_pad (GST_ELEMENT (mjpeg), mjpeg->srcpad);

    gst_mjpegdec_init_members(mjpeg);
}

static gboolean gst_mjpegdec_setcaps (GstPad *pad, GstCaps *caps)
{
    debug_printf("enter %s \n", __FUNCTION__);
    GstMJPEGDec *mjpeg = GST_MJPEGDEC (GST_OBJECT_PARENT (pad));

    GstStructure *structure = gst_caps_get_structure (caps, 0);

    const char *mime = gst_structure_get_name (structure);
    if (strcmp (mime, "image/jpeg") != 0 && strcmp(mime, "video/x-jpeg") != 0) {
        return FALSE;
    }

    if(FALSE == gst_structure_get_fraction(structure, "framerate", &mjpeg->iFRateNum, &mjpeg->iFRateDen)) {
        mjpeg->iFRateNum = 0;		//if this information isn't in caps, we assume it is 0
        mjpeg->iFRateDen = 1;
    }

    return TRUE;
}

/* event handler for mjpeg element's sinkpad */
static gboolean gst_mjpegdec_sinkpad_event (GstPad *pad, GstEvent *event)
{
    debug_printf("enter %s \n", __FUNCTION__);
    //GstMJPEGDec *mjpeg = GST_MJPEGDEC (GST_OBJECT_PARENT (pad));
    switch(GST_EVENT_TYPE (event))
    {
    case GST_EVENT_NEWSEGMENT:
        {
            GstMJPEGDec *mjpeg = GST_MJPEGDEC( GST_OBJECT_PARENT(pad) );
            mjpeg->is_newSeg = 1;
            mjpeg->TS_exitSink = 0;
            mjpeg->est_demuxdecCost_usec = 0;
            mjpeg->t_decCost_usec = 0;
            mjpeg->t_decCost_usec_avg = 0;
        }
        break;
    case GST_EVENT_EOS:
        {
            GstMJPEGDec *mjpeg = GST_MJPEGDEC( GST_OBJECT_PARENT(pad) );
            mjpeg->is_playing = 0;

#ifdef LOG_SKIP_FRAME
            log_skip_frame(-1);
#endif

        }
        break;
    default:
        break;
    }

    return gst_pad_event_default(pad, event);
}

/* event handler for mjpeg element's srcpad */
static gboolean gst_mjpegdec_srcpad_event (GstPad *pad, GstEvent *event)
{
    gboolean eventRet;
    debug_printf("enter %s \n", __FUNCTION__);
    //GstMJPEGDec *mjpeg = GST_MJPEGDEC (GST_OBJECT_PARENT (pad));
    switch(GST_EVENT_TYPE (event))
    {
    case GST_EVENT_QOS:
        {
            GstMJPEGDec *mjpeg = GST_MJPEGDEC( GST_OBJECT_PARENT(pad) );
            //gdouble proportion;
            GstClockTimeDiff diff;
            //GstClockTime timestamp;
            //gst_event_parse_qos(event, &proportion, &diff, &timestamp);
            gst_event_parse_qos(event, NULL, &diff, NULL);
            //If there is queue between mjpegdec and sink, the qos probably reflect some frame formerly not current frame. But it's still OK because the previous diff is close to current diff.
            if(diff == 0) {
                mjpeg->TS_exitSink = 0;	//if sink's sync=0, the diff in qos event fired by sink equals to 0. Therefore, we do not skip.
            }else if(diff < 0){
                mjpeg->TS_exitSink = mjpeg->nextTS;	//according to Gstreamer document, gst_event_parse_qos() returned timestamp is based on running_time not buffer's timestamp. Therefore, we use buffer's timestamp.
            }else{
                mjpeg->TS_exitSink = mjpeg->nextTS + diff;
            }
            //printf("qos TS %lldns, jitter %lldns, rate %.3f\n", timestamp, diff, proportion);
            eventRet = gst_pad_push_event (mjpeg->sinkpad, event);
        }

        break;
    default:
        eventRet = gst_pad_event_default(pad, event);
        break;
    }

    return eventRet;
}


static IppCodecStatus gst_mjpegdec_codec_init(GstMJPEGDec * mjpeg)
{
    IppCodecStatus ret;
    debug_printf("enter %s \n", __FUNCTION__);

    ret = DecoderInitAlloc_JPEG(&(mjpeg->srcBitStream), &(mjpeg->dstPicture), mjpeg->pCBTable, &(mjpeg->pJPEGDecState), NULL);

    if(ret != IPP_STATUS_NOERR) {
        GST_WARNING("Jpeg codec init fail ret=%d\n", ret);
        return ret;
    }

    debug_printf("input picture width %d, height %d\n", mjpeg->dstPicture.picWidth, mjpeg->dstPicture.picHeight);

    if(mjpeg->dstPicture.picWidth > MAX_INPUT_WIDTH ||
            mjpeg->dstPicture.picWidth < MIN_INPUT_WIDTH ||
            mjpeg->dstPicture.picHeight < MIN_INPUT_HEIGHT ||
            mjpeg->dstPicture.picHeight > MAX_INPUT_HEIGHT){
        GST_ERROR("Input jpeg picture exceed the maximum size");
        return IPP_STATUS_ERR;
    }

    //set ROI according to input picture's dimension
    mjpeg->jpegDecPar.srcROI.x = 0;
    mjpeg->jpegDecPar.srcROI.y = 0;
    mjpeg->jpegDecPar.srcROI.width = mjpeg->dstPicture.picWidth;
    mjpeg->jpegDecPar.srcROI.height = mjpeg->dstPicture.picHeight;

    if(mjpeg->skip_patternidx == -1) {
        mjpeg->skip_patternidx = decide_skippattern(mjpeg->dstPicture.picWidth, mjpeg->dstPicture.picHeight, mjpeg->dstPicture.picFormat + JPEG_YUV444, mjpeg->skip_strategy);
    }

    return IPP_STATUS_NOERR;
}

static GstFlowReturn
gst_mjpegdec_set_check_param(GstMJPEGDec* mjpeg, IppJPEGDecoderParam * DecoderPar)
{
    debug_printf("enter %s \n", __FUNCTION__);

    /* Scaled output width/height */
    int nScale = DecoderPar->nScale;
    int nDstWidth = ((DecoderPar->srcROI.width << 16) + nScale - 1)/nScale;
    int nDstHeight = ((DecoderPar->srcROI.height << 16) + nScale - 1)/nScale;

    if(G_UNLIKELY(nDstWidth > MAX_OUTPUT_WIDTH)) {
        GST_WARNING_OBJECT(mjpeg, "output width %d exceeds the max output width, reset to %d", nDstWidth, MAX_OUTPUT_WIDTH);
        nDstWidth = MAX_OUTPUT_WIDTH;
    }
    if(G_UNLIKELY(nDstHeight > MAX_OUTPUT_HEIGHT)) {
        GST_WARNING_OBJECT(mjpeg, "output height %d exceeds the max output height, reset to %d", nDstHeight, MAX_OUTPUT_HEIGHT);
        nDstHeight = MAX_OUTPUT_HEIGHT;
    }

    mjpeg->outputWidth = (nDstWidth + 1) & 0xFFFFFFFE;
    mjpeg->outputHeight = (nDstHeight + 1) & 0xFFFFFFFE;

    //set the jpeg decoder dst picture format parameter as input picture's format
    mjpeg->jpegDecPar.nDesiredColor = mjpeg->dstPicture.picFormat;

    /*********** set output format ***********/
    switch(mjpeg->dstPicture.picFormat) {
    case JPEG_YUV444:
    case JPEG_YUV422I:
    case JPEG_YUV411:
        mjpeg->outGstBufDataFormat = mjpeg->dstPicture.picFormat;
        break;
    case JPEG_YUV422:
        if(mjpeg->bForce_422to420 == 0) {
            mjpeg->outGstBufDataFormat = JPEG_YUV422;
        }else{
            mjpeg->outGstBufDataFormat = JPEG_YUV411;	//in decoder, convert 422 to 420
            mjpeg->jpegDecPar.nDesiredColor = JPEG_YUV411;
        }
        break;
    case JPEG_GRAY8:
        //for gray8 jpeg, we do trick to convert it to yuv420 planner in gst_mjpegdec_init_dst()
        mjpeg->outGstBufDataFormat = JPEG_YUV411;
        break;
    default:
        g_warning("the input jpeg's format is un-supported %d\n", mjpeg->dstPicture.picFormat);
        GST_ERROR_OBJECT(mjpeg, "the input jpeg's format is un-supported %d\n", mjpeg->dstPicture.picFormat);
        return GST_FLOW_ERROR;
    }

    return GST_FLOW_OK;

}

static gboolean gst_mjpegdec_caps_create_and_set(GstMJPEGDec * mjpeg)
{
    guint32 fmt;
    GstCaps *Tmp;

    debug_printf("enter %s \n", __FUNCTION__);

    if(mjpeg->bCapSet != 0) {
        return TRUE; /* data caps bas been set, just return it. */
    }

    switch(mjpeg->outGstBufDataFormat) {
    case JPEG_YUV411:
        fmt =  GST_MAKE_FOURCC ('I', '4', '2', '0');
        break;
    case JPEG_YUV422:
        fmt = GST_MAKE_FOURCC ('Y', '4', '2', 'B');
        break;
    case JPEG_YUV422I:
        fmt = GST_MAKE_FOURCC ('J', '4', '2', 'I');	//J42I is our defined fourcc
        break;
    case JPEG_YUV444:
        fmt = GST_MAKE_FOURCC ('Y', '4', '4', '4');
        break;
    default:
        g_warning("unsupport output format %d!", mjpeg->outGstBufDataFormat);
        GST_ERROR_OBJECT(mjpeg, "unsupport output format %d!", mjpeg->outGstBufDataFormat);
        return FALSE;
    }

    Tmp = gst_caps_new_simple ("video/x-raw-yuv", \
            "format", GST_TYPE_FOURCC, fmt, \
            "width", G_TYPE_INT, mjpeg->outputWidth, \
            "height", G_TYPE_INT, mjpeg->outputHeight, \
            "framerate", GST_TYPE_FRACTION, mjpeg->iFRateNum, mjpeg->iFRateDen, \
            NULL);

    if(FALSE == gst_pad_set_caps(mjpeg->srcpad, Tmp)) {
        gchar* cap_string = gst_caps_to_string(Tmp);
        g_warning("set mjpeg srcpad cap fail, cap is %s",cap_string);
        g_free(cap_string);
        gst_caps_unref(Tmp);
        return FALSE;
    }

    gst_caps_unref(Tmp);
    mjpeg->bCapSet = 1;
    return TRUE;
}


static GstFlowReturn gst_mjpegdec_init_dst(GstMJPEGDec * mjpeg, IppPicture * DstPicture)
{
    int nChromaSize = 0;
    int iOutBufDataSz;

    debug_printf("enter %s \n", __FUNCTION__);

    nChromaSize = (mjpeg->outputWidth >>JPEGSubSamplingTab[mjpeg->outGstBufDataFormat-JPEG_YUV444][0]) * (mjpeg->outputHeight>>JPEGSubSamplingTab[mjpeg->outGstBufDataFormat-JPEG_YUV444][1]);

    debug_printf(" %s: nChromaSize=%d \n", __FUNCTION__, nChromaSize);


    iOutBufDataSz = (mjpeg->outputWidth * mjpeg->outputHeight) + (nChromaSize << 1);

    GST_LOG_OBJECT (mjpeg, "Mallocing YUV buffer=%d.", iOutBufDataSz);

    // now request downstream such as overlaysink to get buf with allocated space
    if(gst_pad_alloc_buffer_and_set_caps(mjpeg->srcpad, 0, iOutBufDataSz, GST_PAD_CAPS (mjpeg->srcpad), &(mjpeg->outGstBuf)) != GST_FLOW_OK) {
        g_warning("mjpeg can not get buffer from down element");
        GST_ERROR_OBJECT (mjpeg, "mjpeg can not get buffer from down element");
        return GST_FLOW_ERROR;
    }


    if(G_LIKELY(((unsigned int)(GST_BUFFER_DATA(mjpeg->outGstBuf)) & 7) == 0)) {
        //8 alignment
        DstPicture->ppPicPlane[0] = GST_BUFFER_DATA(mjpeg->outGstBuf);
    }else{
        //not 8 alignment
        GST_INFO_OBJECT(mjpeg, "the buffer allocated from down stream isn't 8 alignment, allocate 8 alignment memory by plug-in self\n");
        mjpeg->pCBTable->fMemMalloc(&(DstPicture->ppPicPlane[0]), iOutBufDataSz, 8);
        if(G_UNLIKELY(DstPicture->ppPicPlane[0] == NULL)) {
            GST_ERROR_OBJECT (mjpeg, "mjpeg can not allocate buffer by itself");
            return GST_FLOW_ERROR;
        }
        mjpeg->bJpegDstBufAllocatedBySelf = 1;
    }

    DstPicture->picWidth  = mjpeg->outputWidth;
    DstPicture->picHeight = mjpeg->outputHeight;
    DstPicture->picPlaneStep[0] = mjpeg->outputWidth;

    DstPicture->picPlaneStep[2] = DstPicture->picPlaneStep[1] = (mjpeg->outputWidth>>JPEGSubSamplingTab[mjpeg->outGstBufDataFormat-JPEG_YUV444][0]);
    DstPicture->ppPicPlane[1]   = (Ipp8u*)DstPicture->ppPicPlane[0] + mjpeg->outputWidth * mjpeg->outputHeight;
    DstPicture->ppPicPlane[2]   = (Ipp8u*)DstPicture->ppPicPlane[1] + nChromaSize;

    if(mjpeg->jpegDecPar.nDesiredColor == JPEG_GRAY8) {
        //for JPEG_GRAY8, manual fill 128 in U, V planner to make it convert to yuv 420 planner format
        memset(DstPicture->ppPicPlane[1], 128, nChromaSize<<1);
    }

    GST_INFO_OBJECT (mjpeg, "Dst buffer alloc OK. Begin decoding output picture width=%d, height=%d", DstPicture->picWidth,DstPicture->picHeight);

    debug_printf("leave %s OK\n", __FUNCTION__);
    return GST_FLOW_OK;

}

static GstFlowReturn
gst_mjpegdec_push(GstMJPEGDec * mjpeg)
{
    GstFlowReturn ret;
    GstBuffer *outBuf = mjpeg->outGstBuf;

    debug_printf("enter %s, output color format=%d \n", __FUNCTION__, mjpeg->outGstBufDataFormat);

    if(G_UNLIKELY(mjpeg->bJpegDstBufAllocatedBySelf != 0)) {
        memcpy(GST_BUFFER_DATA(outBuf), mjpeg->dstPicture.ppPicPlane[0], GST_BUFFER_SIZE(outBuf));
    }
    GST_BUFFER_TIMESTAMP (outBuf) = mjpeg->nextTS;
    GST_BUFFER_DURATION (outBuf) = mjpeg->duration;


    ret = gst_pad_push (mjpeg->srcpad, outBuf);
    if(ret != GST_FLOW_OK) {
        GST_ERROR("pad push frame to down stream error=%d", ret);
        return ret;
    }

    debug_printf("leave %s \n", __FUNCTION__);
    return GST_FLOW_OK;

}


static GstFlowReturn
gst_mjpegdec_chain (GstPad * pad, GstBuffer * buffer)
{
    IppCodecStatus decoding_ret;
    //GstMJPEGDec *mjpeg = GST_MJPEGDEC (gst_pad_get_parent (pad));	//gst_pad_get_parent will increase the refcount of mjpeg
    GstMJPEGDec *mjpeg = GST_MJPEGDEC (GST_OBJECT_PARENT (pad));

    debug_printf("enter %s \n", __FUNCTION__);

    IppJPEGDecoderParam * DecoderPar = &(mjpeg->jpegDecPar);
    IppPicture * DstPicture = &(mjpeg->dstPicture);
    IppBitstream * SrcBitStream = &(mjpeg->srcBitStream);
    struct timeval Clk_0, Clk_1;

    mjpeg->frameSerialNum_in1stream ++;		//we assume that demux provide a whole frame to decoder each time


    mjpeg->srcBitStream.bsByteLen = GST_BUFFER_SIZE(buffer);
    mjpeg->srcBitStream.pBsCurByte = GST_BUFFER_DATA(buffer);
    mjpeg->srcBitStream.pBsBuffer = GST_BUFFER_DATA(buffer);
    mjpeg->srcBitStream.bsCurBitOffset = 0;


    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //We assume for each buffer provided by demuxer, there is valid TS and duration attached on it. And the duration is equal to the 1/fps
    if (GST_BUFFER_TIMESTAMP (buffer) != GST_CLOCK_TIME_NONE) {
        mjpeg->nextTS = GST_BUFFER_TIMESTAMP (buffer);
        mjpeg->duration= GST_BUFFER_DURATION (buffer);
        if(mjpeg->skip_strategy == 1) {
            estimate_spendtime(mjpeg);
        }
        //printf("TS %lldns, Dur %lldns", mjpeg->nextTS, mjpeg->duration);
    }


    //skip the frame
    if(G_UNLIKELY(mjpeg->is_newSeg == 0)) {
        if(should_skip(mjpeg, buffer) != 0) {
#ifdef LOG_MJPEGDEC_INFO
            mjpeg->skipframecnt_in1stream ++;
#endif

#ifdef LOG_SKIP_FRAME
            log_skip_frame(1);
#endif

            gst_buffer_unref(buffer);
            //printf(" skipped frame num %d\n",mjpeg->frameSerialNum_in1stream);
            return GST_FLOW_OK;
        }
    }else{
        //for the first frame in one stream, we don't skip it
        mjpeg->is_newSeg = 0;
    }

#ifdef LOG_SKIP_FRAME
    log_skip_frame(0);
#endif

    //printf("notskip frame num %d\n",mjpeg->frameSerialNum_in1stream);

    //exclude padding bytes
    {
#define MAX_PADDEDBYTE_LENGTH	128*1024	//for qtdemuxer and mov file, there are probably a lot of padding bytes. so define MAX_PADDEDBYTE_LENGTH as a big value
        unsigned short eoiFlag = 0;
        int iPaddedBytes = 0;
        while(0xd9ff !=eoiFlag && iPaddedBytes < SrcBitStream->bsByteLen){
            eoiFlag = (eoiFlag << 8) | (SrcBitStream->pBsCurByte[SrcBitStream->bsByteLen-iPaddedBytes-1]);
            iPaddedBytes++;
        }
        if(G_LIKELY(iPaddedBytes <= MAX_PADDEDBYTE_LENGTH && iPaddedBytes-2 < SrcBitStream->bsByteLen)) {
            SrcBitStream->bsByteLen -= iPaddedBytes-2;
        }else{
            //if too much padded bytes exist, it means this stream is error stream
            GST_ERROR_OBJECT(mjpeg, "Couldn't found jpeg eos flag 0xffd9, give up decoding this frame");
            goto chain_error;
        }
    }


    {
        mjpeg->bJpegDstBufAllocatedBySelf = 0;
        mjpeg->outGstBuf = NULL;
        DstPicture->ppPicPlane[0] = NULL;

        if( gst_mjpegdec_codec_init(mjpeg) != IPP_STATUS_NOERR ) {
            goto chain_error;
        }

        if(gst_mjpegdec_set_check_param(mjpeg, DecoderPar) != GST_FLOW_OK) {
            goto chain_error;
        }

        /* set output format info. */
        if(gst_mjpegdec_caps_create_and_set(mjpeg) != TRUE) {
            goto chain_error;
        }

        if(gst_mjpegdec_init_dst(mjpeg, DstPicture) != GST_FLOW_OK) {
            goto chain_error;
        }
    }

    debug_printf("  %s : before Decode_JPEG \n", __FUNCTION__);

    gettimeofday(&Clk_0, NULL);
    decoding_ret = Decode_JPEG(DstPicture, DecoderPar, mjpeg->pJPEGDecState);
    gettimeofday(&Clk_1, NULL);

    debug_printf("  %s : after Decode_JPEG, ret=%d \n", __FUNCTION__, decoding_ret);

    mjpeg->t_decCost_usec = 1000000*(Clk_1.tv_sec-Clk_0.tv_sec) + Clk_1.tv_usec-Clk_0.tv_usec;

#ifdef DEBUG_PERFORMANCE
    mjpeg->codec_time += mjpeg->t_decCost_usec;
#endif


    if(G_UNLIKELY(mjpeg->t_decCost_usec_avg == 0)) {
        mjpeg->t_decCost_usec_avg = mjpeg->t_decCost_usec;
    }else{
        //clip too big value, consider they are burr
        int clipped = mjpeg->t_decCost_usec < (mjpeg->t_decCost_usec_avg<<1) ? mjpeg->t_decCost_usec : (mjpeg->t_decCost_usec_avg<<1);
#define FILTER_COEFEXP	2
        mjpeg->t_decCost_usec_avg = (clipped + ((1<<FILTER_COEFEXP)-1)*mjpeg->t_decCost_usec_avg)>>FILTER_COEFEXP;
    }

#ifdef LOG_RAWIPPDECODER_PLUGIN_TIME
    T_JDecCostSum_usec += mjpeg->t_decCost_usec;
#endif



    switch(decoding_ret) {
    case IPP_STATUS_NOERR:
        debug_printf(" %s: JPEG decode OK. \n",__FUNCTION__);
        break;
    case IPP_STATUS_NEED_INPUT:
        g_warning("need more data for decode one picture, demuxer should provide the whole picture each time");
        GST_WARNING_OBJECT(mjpeg, "More data is needed, exit chain func.");
        goto chain_error;
    default:
        g_warning("jpeg codec err ret=%d\n", decoding_ret);
        GST_WARNING_OBJECT(mjpeg, "jpeg codec err ret=%d\n", decoding_ret);
        goto chain_error;
    }
    gst_buffer_unref(buffer);

#ifdef DEBUG_PERFORMANCE
    mjpeg->totalFrames++;
#endif
    /* Push the output */
    gst_mjpegdec_push(mjpeg);

    DecoderFree_JPEG(&(mjpeg->pJPEGDecState));
    mjpeg->pJPEGDecState = NULL;

    if(mjpeg->bJpegDstBufAllocatedBySelf != 0) {
        mjpeg->pCBTable->fMemFree(&(DstPicture->ppPicPlane[0]));
        DstPicture->ppPicPlane[0] = NULL;
        mjpeg->bJpegDstBufAllocatedBySelf = 0;
    }
    mjpeg->outGstBuf = NULL;	//down stream element will gst_buffer_unref(mjpeg->outGstBuf)


    debug_printf("leave %s OK\n\n", __FUNCTION__);

    return GST_FLOW_OK;


chain_error:
    //since for each frame, we init jpeg decoder, we needn't to return a fatal error to cause pipeline stopping.
    gst_buffer_unref(buffer);
    if(mjpeg->pJPEGDecState) {
        DecoderFree_JPEG(&(mjpeg->pJPEGDecState));
        mjpeg->pJPEGDecState = NULL;
    }
    if(mjpeg->bJpegDstBufAllocatedBySelf != 0) {
        mjpeg->pCBTable->fMemFree(&(DstPicture->ppPicPlane[0]));
        DstPicture->ppPicPlane[0] = NULL;
        mjpeg->bJpegDstBufAllocatedBySelf = 0;
    }
    if(mjpeg->outGstBuf != NULL) {
        gst_buffer_unref(mjpeg->outGstBuf);
        mjpeg->outGstBuf = NULL;
    }

    debug_printf("leave %s (chain_error)\n", __FUNCTION__);
    return GST_FLOW_OK;
}

static void gst_mjpegdec_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
    GstMJPEGDec *mjpeg = GST_MJPEGDEC (object);
    debug_printf("enter %s \n", __FUNCTION__);
    switch (prop_id)
    {
    case ARG_FORCE422TO420:
        mjpeg->bForce_422to420 = g_value_get_int(value);
        if(mjpeg->bForce_422to420 > 1 || mjpeg->bForce_422to420 < 0) {
            mjpeg->bForce_422to420 = 1;
            GST_ERROR("force-422to420 exceed range, reset to 1");
        }
        break;
    case ARG_SKIP_STRATEGY:
        mjpeg->skip_strategy = g_value_get_int(value);
        if(mjpeg->skip_strategy > 6 || mjpeg->skip_strategy < 0) {
            mjpeg->skip_strategy = 0;
            GST_ERROR("frameskip-strategy exceed range, reset to 0");
        }
        break;
    case ARG_SCALE:
        mjpeg->jpegDecPar.nScale = g_value_get_int(value);
        if(mjpeg->jpegDecPar.nScale < 0) {
            mjpeg->jpegDecPar.nScale = 65536;
            GST_ERROR("scale-ratio exceed range, reset to 65536");
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void gst_mjpegdec_get_property (GObject *object, guint prop_id, 
    GValue *value, GParamSpec *pspec)
{
    GstMJPEGDec *mjpeg = GST_MJPEGDEC (object);
    debug_printf("enter %s \n", __FUNCTION__);
    switch (prop_id)
    {
    case ARG_FORCE422TO420:
        g_value_set_int(value, mjpeg->bForce_422to420);
        break;
    case ARG_SKIP_STRATEGY:
        g_value_set_int(value, mjpeg->skip_strategy);
        break;
    case ARG_SCALE:
        g_value_set_int(value, mjpeg->jpegDecPar.nScale);
        break;
#ifdef DEBUG_PERFORMANCE
    case ARG_TOTALFRAME:
        g_value_set_int (value, mjpeg->totalFrames);
        break;
    case ARG_CODECTIME:
        g_value_set_int64 (value, mjpeg->codec_time);
        break;
#endif

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static GstStateChangeReturn gst_mjpegdec_change_state (
    GstElement *element, GstStateChange transition)
{
    GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
    GstMJPEGDec *mjpeg = GST_MJPEGDEC (element);
    //printf("mjpeg state change trans from %d to %d, skip strategy is %d, sz %d %d\n", transition>>3, transition&7, mjpeg->skip_strategy, mjpeg->jpegDecPar.srcROI.width, mjpeg->jpegDecPar.srcROI.height);
    debug_printf("enter %s \n", __FUNCTION__);

    //only after all element in the pipeline changed their state to playing, we set the mjpeg->is_playing to 1. By default, set it to 0.
    mjpeg->is_playing = 0;

    switch (transition)
    {
    case GST_STATE_CHANGE_NULL_TO_READY:
        GST_LOG_OBJECT (mjpeg, "Initializing call back table...");
        miscInitGeneralCallbackTable (&(mjpeg->pCBTable));
        break;
    case GST_STATE_CHANGE_READY_TO_PAUSED:
        //prepare to decode a new stream
#ifdef LOG_RAWIPPDECODER_PLUGIN_TIME
        gettimeofday(&Clk_WorkingStart, NULL);
#endif
        mjpeg->frameSerialNum_in1stream = 0;	//when ready to pause, it means the plug-in will to decode a new stream
        mjpeg->skip_patternidx = -1;
#ifdef LOG_MJPEGDEC_INFO
        mjpeg->skipframecnt_in1stream = 0;
#endif
        mjpeg->bCapSet = 0;
        break;
    default:
        break;
    }

    ret = parent_class->change_state (element, transition);
    if (ret != GST_STATE_CHANGE_SUCCESS) {
        return ret;
    }

    switch (transition)
    {
    case GST_STATE_CHANGE_READY_TO_NULL:
        GST_LOG_OBJECT (mjpeg, "Freeing call back table and resource ...");
        if(mjpeg->pJPEGDecState != NULL){
            DecoderFree_JPEG(&(mjpeg->pJPEGDecState));
            mjpeg->pJPEGDecState = NULL;
        }
        miscFreeGeneralCallbackTable (&(mjpeg->pCBTable));
#ifdef DEBUG_PERFORMANCE
        printf("codec system time %lld usec, frame number %d\n", mjpeg->codec_time, mjpeg->totalFrames);
#endif
        break;

    case GST_STATE_CHANGE_PAUSED_TO_READY:
#ifdef LOG_RAWIPPDECODER_PLUGIN_TIME
        gettimeofday(&Clk_WorkingStop, NULL);
        printf("Pure dec sys time %.3f ms, plugin working time %.3f ms\n", T_JDecCostSum_usec/1000.,(Clk_WorkingStop.tv_sec-Clk_WorkingStart.tv_sec)*1000+(Clk_WorkingStop.tv_usec-Clk_WorkingStart.tv_usec)/1000.);
#endif
#ifdef LOG_MJPEGDEC_INFO
        printf("++++++++++++++++\n");
        printf("input jpeg file format is %d\n", mjpeg->dstPicture.picFormat + JPEG_YUV444);
        printf("resize dst width is %d, resize dst height is %d\n", mjpeg->jpegDecPar.nDstWidth, mjpeg->jpegDecPar.nDstHeight);
        printf("Demuxer provide %d frames, decoder skipped %d frames(rate=%.3f)\n", mjpeg->frameSerialNum_in1stream, mjpeg->skipframecnt_in1stream, mjpeg->skipframecnt_in1stream/(double)mjpeg->frameSerialNum_in1stream);
#endif
        break;

        //only after all element in the pipeline changed their state to playing, we set the mjpeg->is_playing to 1.
    case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
        mjpeg->is_playing = 1;
        break;
    default:
        break;
    }

    return ret;
}



static gboolean plugin_init (GstPlugin *plugin)
{
    return gst_element_register (plugin, "mjpegdec", GST_RANK_PRIMARY+2, GST_TYPE_MJPEGDEC);
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
        GST_VERSION_MINOR,
        "mvl_mjpegdec",
        "mjpeg decoder based on IPP, "__DATE__,
        plugin_init,
        VERSION,
        "Proprietary",
        "",
        "");
/* EOF */
