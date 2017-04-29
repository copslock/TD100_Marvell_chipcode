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

/* gstippcnmassist.h */

#ifndef __GST_IPPCNMASSIST_H__
#define __GST_IPPCNMASSIST_H__

#include <gst/gst.h>


G_BEGIN_DECLS

#define GST_TYPE_IPPCNMASSIST			(gst_ippcnmassist_get_type())
#define GST_IPPCNMASSIST(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_IPPCNMASSIST, GstIppCnmAssist))
#define GST_IPPCNMASSIST_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_IPPCNMASSIST, GstIppCnmAssistClass))
#define GST_IS_IPPCNMASSIST(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_IPPCNMASSIST))
#define GST_IS_IPPCNMASSIST_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_IPPCNMASSIST))

GType gst_ippcnmassist_get_type(void);

typedef struct _GstIppCnmAssist GstIppCnmAssist;
typedef struct _GstIppCnmAssistClass GstIppCnmAssistClass;

struct _GstIppCnmAssist
{
	GstElement					element;
	GstPad						*sinkpad;
	GstPad						*srcpad;
	
	int							kind;	//-1: unknown, 0:h264, 1:mpeg4, 2:wmv(vc1sp&mp)
	int							bDistinguished;	//0: not distinguished, 1: cnm support, 2: cnm not support
	GstCaps*					srcCap;
	
	int							bSrcPadAddedCap;
	GList*						cached_events;

	int							UpAdjacentNonQueueEle;
	GstBuffer*					AccumulatedBuf;
};

struct _GstIppCnmAssistClass
{
	GstElementClass parent_class;
};

G_END_DECLS


#endif

/* EOF */
