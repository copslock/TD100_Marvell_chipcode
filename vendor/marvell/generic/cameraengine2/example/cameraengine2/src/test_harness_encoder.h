/******************************************************************************
*(C) Copyright [2010 - 2011] Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
#ifndef _TEST_HARNESS_ENCODER_H_
#define _TEST_HARNESS_ENCODER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CameraEngine.h"


void *VideoEncoder_Init( CAM_DeviceHandle handle, CAM_ImageBuffer *pImgBuf, int iImgBufCnt, int iEncodeType, const char *sFileName );
void VideoEncoder_Deinit( void *hVideoEncoder );
void VideoEncoder_DeliverInput( void *hVideoEncoder, CAM_ImageBuffer *pImgBuf );
void VideoEncoder_SendEOS( void *hVideoEncoder );
CAM_Bool VideoEncoder_EOSReceived( void *hVideoEncoder );

#ifdef __cplusplus
}
#endif

#endif // _TEST_HARNESS_ENCODER_H_
