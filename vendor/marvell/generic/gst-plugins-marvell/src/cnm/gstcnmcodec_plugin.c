/*
Copyright (c) 2010, Marvell International Ltd.
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

/* gstcnmcodec_plugin.c */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>

#ifdef ENABLE_WMV3DEC_CNM_IPP_GSTPLUGIN
#include "gstcnmdec_wmv3.h"
#endif

#ifdef ENABLE_H264DEC_CNM_IPP_GSTPLUGIN
#include "gstcnmdec_h264.h"
#endif

#ifdef ENABLE_MPEG4DEC_CNM_IPP_GSTPLUGIN
#include "gstcnmdec_mpeg4.h"
#endif

#ifdef ENABLE_MPEG2DEC_CNM_IPP_GSTPLUGIN
#include "gstcnmdec_mpeg2.h"
#endif

#ifdef ENABLE_H263DEC_CNM_IPP_GSTPLUGIN
#include "gstcnmdec_h263.h"
#endif

#ifdef ENABLE_ENCODER_CNM_IPP_GSTPLUGIN
#include "gstcnmenc_h264.h"
#include "gstcnmenc_h263.h"
#include "gstcnmenc_mpeg4.h"
#endif

static gboolean
plugin_init (GstPlugin *plugin)
{
#ifdef ENABLE_WMV3DEC_CNM_IPP_GSTPLUGIN
	if(!gst_element_register(plugin, "cnmdec_wmv3", GST_RANK_PRIMARY+3, GST_TYPE_CNMWMVDEC)) {
		return FALSE;
	}
#endif

#ifdef ENABLE_H264DEC_CNM_IPP_GSTPLUGIN
	if(!gst_element_register(plugin, "cnmdec_h264", GST_RANK_PRIMARY+3, GST_TYPE_CNMH264DEC)) {
		return FALSE;
	}
#endif

#ifdef ENABLE_MPEG4DEC_CNM_IPP_GSTPLUGIN
	if(!gst_element_register(plugin, "cnmdec_mpeg4", GST_RANK_PRIMARY+3, GST_TYPE_CNMMPEG4DEC)) {
		return FALSE;
	}
#endif

#ifdef ENABLE_MPEG2DEC_CNM_IPP_GSTPLUGIN
	if(!gst_element_register(plugin, "cnmdec_mpeg2", GST_RANK_PRIMARY+3, GST_TYPE_CNMMPEG2DEC)) {
		return FALSE;
	}
#endif

#ifdef ENABLE_H263DEC_CNM_IPP_GSTPLUGIN
	if(!gst_element_register(plugin, "cnmdec_h263", GST_RANK_PRIMARY+3, GST_TYPE_CNMH263DEC)) {
		return FALSE;
	}
#endif

#ifdef ENABLE_ENCODER_CNM_IPP_GSTPLUGIN
	if(!gst_element_register(plugin, "cnmenc_h264", GST_RANK_MARGINAL+3, GST_TYPE_CNMH264ENC)) {
		return FALSE;
	}
	if(!gst_element_register(plugin, "cnmenc_h263", GST_RANK_MARGINAL+3, GST_TYPE_CNMH263ENC)) {
		return FALSE;
	}
	if(!gst_element_register(plugin, "cnmenc_mpeg4", GST_RANK_MARGINAL+3, GST_TYPE_CNMMPEG4ENC)) {
		return FALSE;
	}
#endif

	return TRUE;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
				   GST_VERSION_MINOR,
				   "mvl_cnmcodec",
				   "hardware CNM codec plugin based on IPP, build date "__DATE__,
				   plugin_init,
				   VERSION,
				   "LGPL",
				   "",
				   "");



/* EOF */
