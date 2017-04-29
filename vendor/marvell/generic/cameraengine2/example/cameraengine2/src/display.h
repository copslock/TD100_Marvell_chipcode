/******************************************************************************
*(C) Copyright [2010 - 2011] Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CameraEngine.h"

// #define DISPLAY_USERPOINTER

#if defined( BONNELL )
	#include <pxa168fb.h>
	#define DEFAULT_FB_DEVICE  "/dev/fb2"
	#define DEFAULT_FB_DEVICE2 "/dev/graphics/fb2"
	#define SCREEN_WIDTH       800
	#define SCREEN_HEIGHT      480
	#define PXA168
#elif defined ( BROWNSTONE ) || defined( ABILENE )
	#include <pxa168fb.h>
	#define DEFAULT_FB_DEVICE  "/dev/fb2"
	#define DEFAULT_FB_DEVICE2 "/dev/graphics/fb2"
	#define SCREEN_WIDTH        1280
	#define SCREEN_HEIGHT       720
	#define PXA168
	#define DISPLAY_USERPOINTER
#elif defined ( G50 )
        #include <pxa168fb.h>
        #define DEFAULT_FB_DEVICE  "/dev/fb2"
        #define DEFAULT_FB_DEVICE2 "/dev/graphics/fb2"
        #define SCREEN_WIDTH        1024
        #define SCREEN_HEIGHT       600
        #define PXA168
        #define DISPLAY_USERPOINTER
#elif defined ( FLINT )
	#include <pxa168fb.h>
	#define DEFAULT_FB_DEVICE  "/dev/fb3"  // HDMI
	#define DEFAULT_FB_DEVICE2 "/dev/graphics/fb3"
	#define SCREEN_WIDTH        640
	#define SCREEN_HEIGHT       480
	#define PXA168
	#define DISPLAY_USERPOINTER
#elif defined ( TTCDKB ) || defined ( TDDKB )
	#include <pxa168fb.h>
	#define DEFAULT_FB_DEVICE  "/dev/fb1"
	#define DEFAULT_FB_DEVICE2 "/dev/graphics/fb1"
	#define SCREEN_WIDTH       320
	#define SCREEN_HEIGHT      480
	#define PXA168
	#define DISPLAY_USERPOINTER
#elif defined ( AVLITE )
	#include <pxa168fb.h>
	#define DEFAULT_FB_DEVICE  "/dev/fb1"
	#define DEFAULT_FB_DEVICE2 "/dev/graphics/fb1"
	#define SCREEN_WIDTH       480
	#define SCREEN_HEIGHT      640
	#define PXA168
	#define DISPLAY_USERPOINTER
#elif defined ( PV2EVB ) || defined ( MG1EVB )
	#include <pxa950fb.h>
	#define DEFAULT_FB_DEVICE  "/dev/fb1"
	#define DEFAULT_FB_DEVICE2 "/dev/graphics/fb1"
	#define SCREEN_WIDTH       480
	#define SCREEN_HEIGHT      640
	#define PXA950
#elif defined ( PV2SAARB ) || defined ( MG1SAARB )
	#include <pxa950fb.h>
	#define DEFAULT_FB_DEVICE  "/dev/fb1"
	#define DEFAULT_FB_DEVICE2 "/dev/graphics/fb1"
	#define SCREEN_WIDTH       480
	#define SCREEN_HEIGHT      640
	#define DISPLAY_USERPOINTER
	#define PXA950
#else
	#error no platform is defined!
#endif


typedef struct 
{
	int             fd_lcd;
	int             on;
	int             bpp;
	int             format;
	int             step;
	int             screen_width;
	int             screen_height;
	int             frame_width;
	int             frame_height;
	int             screen_pos_x;
	int             screen_pos_y;
	int             yoff;
	int             ylen;
	int             cboff;
	int             cblen;
	int             croff;
	int             crlen;
	int             fix_smem_len;
	unsigned char   *map;
	unsigned char   *overlay[3];
} display_cfg;

int displaydemo_open( int screen_x, int screen_y, int screen_w, int screen_h,
                      int frame_w, int frame_h, CAM_ImageFormat eFrameFormat, display_cfg *pDispCfg );
int displaydemo_close( display_cfg *pDispCfg );
int display_to_overlay( CAM_DeviceHandle handle, display_cfg *pDispCfg, CAM_ImageBuffer *pImgBuf );
int clear_overlay( display_cfg *pDispCfg );


#ifdef __cplusplus
}
#endif

#endif // _DISPLAY_H_
