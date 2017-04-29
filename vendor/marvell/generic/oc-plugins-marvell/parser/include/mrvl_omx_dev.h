/***************************************************************************************** 
 *   Copyright (c) 2009, Marvell International Ltd. 
 *     All Rights Reserved.
 ****************************************************************************************/

#ifndef MRVL_OMX_DEV_H_INCLUDED
#define MRVL_OMX_DEV_H_INCLUDED

#define NUM_OF_MRVL_VIDEO_PRI_COMP 5
const OMX_STRING mrvl_video_pri_comp[][2] =
{
    {"OMX.MARVELL.VIDEO.MPEG4ASPDECODERMVED", "/dev/mved"},
    {"OMX.MARVELL.VIDEO.H264DECODERMVED", "/dev/mved"},
    {"OMX.MARVELL.VIDEO.VMETADECODER", "/dev/uio0"},
    {"OMX.MARVELL.VIDEO.MPEG4ENCODERMVED", "/dev/mved"},
    {"OMX.MARVELL.VIDEO.H264ENCODERMVED", "/dev/mved"}
};

#endif
