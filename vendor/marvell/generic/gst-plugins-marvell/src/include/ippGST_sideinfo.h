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

#ifndef __IPP_GST_SIDEINFO_H__
#define __IPP_GST_SIDEINFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IPPGST_BUFFER_CUSTOMDATA
#define IPPGST_BUFFER_CUSTOMDATA(buf)	(((GstBuffer*)buf)->_gst_reserved[sizeof(((GstBuffer*)buf)->_gst_reserved)/sizeof(((GstBuffer*)buf)->_gst_reserved[0]) - 1])
#endif

typedef struct {
	int x_off;	//in pixel
	int y_off;	//in pixel
	int x_stride;	//in byte
	int y_stride;	//in pixel row
	int NeedDeinterlace;	//1: need deinterlace; 0: needn't deinterlace. This flag is just a suggestion for sink, probably not very accurate.
	unsigned int phyAddr;
	void* virAddr;
	void* reserve[3];
}IPPGSTDecDownBufSideInfo;

#ifdef __cplusplus
}
#endif

#endif /* __IPP_GST_SIDEINFO_H__ */

/* EOF */
