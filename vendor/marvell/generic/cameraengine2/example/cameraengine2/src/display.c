/******************************************************************************
*(C) Copyright [2010 - 2011] Marvell International Ltd.
* All Rights Reserved
******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/poll.h>
#include <errno.h>

#include <getopt.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/types.h>
#include <unistd.h>
#include <sys/time.h>

#include "test_harness.h"
#include "display.h"


#if defined( DISPLAY_USERPOINTER )
#define MAX_LCD_BUF_NUM 30
int     ovly_buf_cnt = 0;
#endif

extern CAM_ImageBuffer         *pPreviewBuf;
extern int                     *pPreviewBufStatus; 
extern int                     iPrevBufNum;

#ifndef DISABLE_OVERLAY

int overlay2_open( char *dev, display_cfg *pDispCfg )
{
	struct fb_var_screeninfo var;
	struct fb_fix_screeninfo fix;
	int    err;

	static struct _sViewPortInfo   gViewPortInfo;
	static struct _sViewPortOffset gViewPortOffset;
	struct _sColorKeyNAlpha        colorkey;

	if ( !pDispCfg || !dev )
	{
		return -1;
	}

	memset( &fix, 0, sizeof(struct fb_fix_screeninfo) );
	memset( &var, 0, sizeof(struct fb_var_screeninfo) );
	memset( &gViewPortInfo, 0, sizeof(struct _sViewPortInfo) );
	memset( &gViewPortOffset, 0, sizeof(struct _sViewPortOffset) );

	pDispCfg->fd_lcd = open( dev, O_RDWR );
	if ( pDispCfg->fd_lcd < 0 ) 
	{
		return -2;
	}


#if defined ( PXA168 )
    colorkey.mode              = FB_ENABLE_RGB_COLORKEY_MODE;
    colorkey.alphapath         = FB_GRA_PATH_ALPHA;
    colorkey.config            = 0xff;
    colorkey.Y_ColorAlpha      = 0x4;
    colorkey.U_ColorAlpha      = 0x4;
    colorkey.V_ColorAlpha      = 0x4;
    if ( ioctl( pDispCfg->fd_lcd, FB_IOCTL_SET_COLORKEYnALPHA, &colorkey ) ) 
    {
		TRACE( "Error: FB_IOCTL_SET_COLORKEYnALPHA( %s, %d )\n", __FILE__, __LINE__ );
		
		close( pDispCfg->fd_lcd );
		return -1;
    }
#endif

	gViewPortInfo.srcWidth  = pDispCfg->frame_width;
	gViewPortInfo.srcHeight = pDispCfg->frame_height;
	gViewPortInfo.zoomXSize = pDispCfg->screen_width;
	gViewPortInfo.zoomYSize = pDispCfg->screen_height;

	if ( ioctl( pDispCfg->fd_lcd, FB_IOCTL_SET_VIEWPORT_INFO, &gViewPortInfo ) ) 
	{
		TRACE( "Error: set view port info fail\n" );
		close( pDispCfg->fd_lcd );
		return -1;
	}

	gViewPortOffset.xOffset = pDispCfg->screen_pos_x;
	gViewPortOffset.yOffset = pDispCfg->screen_pos_y;
	if ( ioctl( pDispCfg->fd_lcd, FB_IOCTL_SET_VID_OFFSET, &gViewPortOffset ) ) 
	{
		TRACE( "Error: set view port offset fail\n" );
		close( pDispCfg->fd_lcd );
		return -1;
	}

	// always set VideoMode after ViewPortInfo, or driver will overwrite it
	if ( ioctl( pDispCfg->fd_lcd, FB_IOCTL_SET_VIDEO_MODE, &pDispCfg->format ) ) 
	{
		TRACE( "Error: set video mode fail\n" );
		close( pDispCfg->fd_lcd );
		return -1;
	}

	pDispCfg->on = 0;
	if ( ioctl( pDispCfg->fd_lcd, FB_IOCTL_SWITCH_VID_OVLY, &pDispCfg->on ) ) 
	{
		TRACE( "Error: switch video overlay fail\n" );
		close( pDispCfg->fd_lcd );
		return -1;
	}

	/* get updated "fix" screeninfo */
	err = ioctl( pDispCfg->fd_lcd, FBIOGET_FSCREENINFO, &fix );
	if ( err ) 
	{
		close( pDispCfg->fd_lcd );
		return -4;
	}

	/* get updated "var" screeninfo */
	err = ioctl( pDispCfg->fd_lcd, FBIOGET_VSCREENINFO, &var );
	if ( err ) 
	{
		close( pDispCfg->fd_lcd );
		return -5;
	}
#if !defined( DISPLAY_USERPOINTER )
	TRACE( "fix.smem_len is %d\n", fix.smem_len );

	pDispCfg->map = (unsigned char*)mmap( 0, fix.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, pDispCfg->fd_lcd, 0 );
	if ( pDispCfg->map == MAP_FAILED ) 
	{
		close( pDispCfg->fd_lcd );
		return -6;
	}
#endif

	pDispCfg->yoff  = var.red.offset;
	pDispCfg->ylen  = var.red.length;
	pDispCfg->cboff = var.green.offset;
	pDispCfg->cblen = var.green.length;
	pDispCfg->croff = var.blue.offset;
	pDispCfg->crlen = var.blue.length;

	pDispCfg->fix_smem_len = fix.smem_len;

	return 0;
}

void overlay2_close( display_cfg *pDispCfg )
{
#if !defined( DISPLAY_USERPOINTER )
	munmap( pDispCfg->map, pDispCfg->fix_smem_len );
#endif

	if ( pDispCfg->fd_lcd >= 0 )
	{
		int ret;
		pDispCfg->on = 0;

		ret = ioctl( pDispCfg->fd_lcd, FB_IOCTL_SWITCH_VID_OVLY, &pDispCfg->on );
		if ( ret != 0 )
		{
			TRACE( "FB_IOCTL_SWITCH_VID_OVLY fail ( %s, %d )", __FILE__, __LINE__ );
		}

		close( pDispCfg->fd_lcd );
		pDispCfg->fd_lcd = -1;
	}
	return;
}

int displaydemo_open( int screen_x, int screen_y, int screen_w, int screen_h,
				      int frame_w, int frame_h, CAM_ImageFormat eFrameFormat, display_cfg *pDispCfg )
{
	int ret;
	memset( pDispCfg, 0, sizeof( display_cfg ) );

	pDispCfg->screen_pos_x = screen_x;
	pDispCfg->screen_pos_y = screen_y;

	pDispCfg->screen_width  = screen_w;
	pDispCfg->screen_height = screen_h;

	pDispCfg->frame_width  = frame_w;
	pDispCfg->frame_height = frame_h;

	switch ( eFrameFormat )
	{
	case CAM_IMGFMT_CbYCrY: 
		pDispCfg->format = FB_VMODE_YUV422PACKED;
		pDispCfg->bpp    = 16;
		pDispCfg->step   = pDispCfg->frame_width * 2;
		break;
	case CAM_IMGFMT_YCbYCr:
		pDispCfg->format = FB_VMODE_YUV422PACKED_SWAPYUorV;
		pDispCfg->bpp    = 16;
		pDispCfg->step   = pDispCfg->frame_width * 2;
		break;
	case CAM_IMGFMT_YCbCr422P:
		pDispCfg->format = FB_VMODE_YUV422PLANAR;
		pDispCfg->bpp    = 16;
		pDispCfg->step   = pDispCfg->frame_width * 2;
		break;
	case CAM_IMGFMT_YCbCr420P:
	// hack for display velidation, overlay don't support sp display
	case CAM_IMGFMT_YCbCr420SP:
	case CAM_IMGFMT_YCrCb420SP:
		pDispCfg->format = FB_VMODE_YUV420PLANAR;
		pDispCfg->bpp    = 12;
		pDispCfg->step   = pDispCfg->frame_width;
		break;
	case  CAM_IMGFMT_BGR565:
		pDispCfg->format = FB_VMODE_RGB565;
		pDispCfg->bpp    = 16;
		pDispCfg->step   = pDispCfg->frame_width * 2;
		break;
	case CAM_IMGFMT_BGRA8888:
		pDispCfg->format = FB_VMODE_RGB888UNPACK;
		pDispCfg->bpp    = 32;
		pDispCfg->step   = pDispCfg->frame_width * 4;
		break;
	default:
		TRACE( "Error: Not supported format( %s, %d )\n", __FILE__, __LINE__ );
		return -1;
	}

	ret = overlay2_open( DEFAULT_FB_DEVICE, pDispCfg );
	if ( ret != 0 )
	{
		if ( overlay2_open( DEFAULT_FB_DEVICE2, pDispCfg ) != 0 )
		{
			return -1;
		}
	}

#if !defined( DISPLAY_USERPOINTER ) 
	switch ( eFrameFormat )
	{
	case CAM_IMGFMT_CbYCrY: 
	case CAM_IMGFMT_YCbYCr: 
	case CAM_IMGFMT_BGR565:
	case CAM_IMGFMT_BGRA8888:
		pDispCfg->overlay[0] = pDispCfg->map;
		pDispCfg->overlay[1] = NULL;
		pDispCfg->overlay[2] = NULL;
		break;
	case  CAM_IMGFMT_YCbCr422P:
		pDispCfg->overlay[0] = pDispCfg->map;
		pDispCfg->overlay[1] = pDispCfg->overlay[0] + frame_w * frame_h;
		pDispCfg->overlay[2] = pDispCfg->overlay[1] + frame_w * frame_h / 2;
		break;
	case CAM_IMGFMT_YCbCr420P:
	case CAM_IMGFMT_YCbCr420SP:
	case CAM_IMGFMT_YCrCb420SP:
		pDispCfg->overlay[0] = pDispCfg->map;
		pDispCfg->overlay[1] = pDispCfg->overlay[0] + frame_w * frame_h;
		pDispCfg->overlay[2] = pDispCfg->overlay[1] + frame_w * frame_h / 4;
		break;
	default:
		TRACE( "Error: Not supprted format( %s, %d )\n", __FILE__, __LINE__ );
		return -1;		
	}
	if ( pDispCfg->fd_lcd > 0 )
	{
		// clear the frame buffer
		clear_overlay( pDispCfg );
	}
#endif

	return ( pDispCfg->fd_lcd > 0 ) ? 0 : -1;
}

int displaydemo_close( display_cfg *pDispCfg )
{
	if ( !pDispCfg )
	{
		return -1;
	}

	if ( pDispCfg->fd_lcd > 0 )
	{
		// clear the frame buffer
		clear_overlay( pDispCfg );
		overlay2_close( pDispCfg );

		pDispCfg->fd_lcd = -1;

		return 0;
	}

	return -1;
}


int display_to_overlay( CAM_DeviceHandle handle, display_cfg *pDispCfg, CAM_ImageBuffer *pImgBuf ) 
{
	int       width, height;
	int       screen_width, screen_height, format;
	CAM_Error error;

	int i;
	int iHeightY, iHeightCb, iHeightCr;
	int iDstHeightY, iDstHeightCb, iDstHeightCr;
	int iStepY, iStepCb, iStepCr;
	int iDstStepY, iDstStepCb, iDstStepCr;
	unsigned char *pSrcPtr, *pDstPtr;
	int iMinStep, iMinHeight;

	if ( !pDispCfg )
	{
		return -1;
	}

	width         = pDispCfg->frame_width;
	height        = pDispCfg->frame_height;
	screen_width  = pDispCfg->screen_width;
	screen_height = pDispCfg->screen_height;

	format = pDispCfg->format;

	if ( format == FB_VMODE_YUV422PACKED || format == FB_VMODE_YUV422PACKED_SWAPYUorV ) 
	{
		iHeightY      = height;
		iHeightCb     = 0;
		iHeightCr     = 0;
		iDstHeightY   = height;
		iDstHeightCb  = 0;
		iDstHeightCr  = 0;
		iStepY        = pImgBuf->iStep[0];
		iStepCb       = 0;
		iStepCr       = 0;
		iDstStepY     = screen_width * 2;
		iDstStepCb    = 0;
		iDstStepCr    = 0;
	}
	else if ( format == FB_VMODE_YUV422PLANAR ) 
	{
		iHeightY      = height;
		iHeightCb     = height;
		iHeightCr     = height;
		iDstHeightY   = height;
		iDstHeightCb  = height;
		iDstHeightCr  = height;
		iStepY        = pImgBuf->iStep[0];
		iStepCb       = pImgBuf->iStep[1];
		iStepCr       = pImgBuf->iStep[2];
		iDstStepY     = screen_width;
		iDstStepCb    = screen_width / 2;
		iDstStepCr    = screen_width / 2;
	} 
	else if ( format == FB_VMODE_YUV420PLANAR ) 
	{
		iHeightY      = height;
		iHeightCb     = height / 2;
		iHeightCr     = height / 2;
		iDstHeightY   = height;
		iDstHeightCb  = height / 2;
		iDstHeightCr  = height / 2;
		iStepY        = pImgBuf->iStep[0];
		iStepCb       = pImgBuf->iStep[1];
		iStepCr       = pImgBuf->iStep[2];
		iDstStepY     = screen_width;
		iDstStepCb    = screen_width / 2;
		iDstStepCr    = screen_width / 2;
	} 
	else if ( format == FB_VMODE_RGB565 ) 
	{
		iHeightY      = height;
		iHeightCb     = 0;
		iHeightCr     = 0;
		iDstHeightY   = height;
		iDstHeightCb  = 0;
		iDstHeightCr  = 0;
		iStepY        = pImgBuf->iStep[0];
		iStepCb       = pImgBuf->iStep[1];
		iStepCr       = pImgBuf->iStep[2];
		iDstStepY     = screen_width * 2;
		iDstStepCb    = 0;
		iDstStepCr    = 0;
	} 
	else if ( format == FB_VMODE_RGB888UNPACK ) 
	{
		iHeightY      = height;
		iHeightCb     = 0;
		iHeightCr     = 0;
		iDstHeightY   = height;
		iDstHeightCb  = 0;
		iDstHeightCr  = 0;
		iStepY        = pImgBuf->iStep[0];
		iStepCb       = pImgBuf->iStep[1];
		iStepCr       = pImgBuf->iStep[2];
		iDstStepY     = screen_width * 4;
		iDstStepCb    = 0;
		iDstStepCr    = 0;
	} 
	else 
	{
		TRACE( "Error: Not supported format( %s, %d )\n", __FILE__,__LINE__ );
		return -1;
	}
#if !defined( DISPLAY_USERPOINTER )
	// copy Y component
	pSrcPtr = pImgBuf->pBuffer[0];
	pDstPtr = pDispCfg->overlay[0];
	iMinStep = MIN( iStepY, iDstStepY );
	iMinHeight = MIN( iHeightY, iDstHeightY );
	for ( i = 0; i < iMinHeight; i++ ) 
	{
		memcpy( pDstPtr, pSrcPtr, iMinStep );
		pSrcPtr += iStepY;
		pDstPtr += iDstStepY;
	}

	// copy Cb component
	pSrcPtr = pImgBuf->pBuffer[1];
	pDstPtr = pDispCfg->overlay[1];
	iMinStep = MIN( iStepCb, iDstStepCb );
	iMinHeight = MIN( iHeightCb, iDstHeightCb );
	for ( i = 0; i < iMinHeight; i++ ) 
	{
		memcpy( pDstPtr, pSrcPtr, iMinStep );
		pSrcPtr += iStepCb;
		pDstPtr += iDstStepCb;
	}

	// copy Cr component
	pSrcPtr = pImgBuf->pBuffer[2];
	pDstPtr = pDispCfg->overlay[2];
	iMinStep = MIN( iStepCr, iDstStepCr );
	iMinHeight = MIN( iHeightCr, iDstHeightCr );
	for ( i = 0; i < iMinHeight; i++ ) 
	{
		memcpy( pDstPtr, pSrcPtr, iMinStep );
		pSrcPtr += iStepCr;
		pDstPtr += iDstStepCr;
	}

	error = CAM_SendCommand( handle, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)CAM_PORT_PREVIEW, (CAM_Param)pImgBuf );
	ASSERT_CAM_ERROR( error );
#else
	{
		struct _sOvlySurface stOvlySurface;
#if defined( PXA168 )
		unsigned char *pFreeBufAddr[MAX_LCD_BUF_NUM][3];  /* To store buffer which could be released */	
		memset( pFreeBufAddr, 0, 3 * MAX_LCD_BUF_NUM * sizeof( unsigned char* ) );
#else
		unsigned char *pFreeBufAddr[MAX_LCD_BUF_NUM][1];	
		memset( pFreeBufAddr, 0, MAX_LCD_BUF_NUM * sizeof( unsigned char* ) );
#endif
		int i, j;

		memset( &stOvlySurface, 0, sizeof( struct _sOvlySurface ) );
        
		if ( ioctl( pDispCfg->fd_lcd, FB_IOCTL_GET_VIDEO_MODE, &stOvlySurface.videoMode ) )
		{
			TRACE( "Error: FB_IOCTL_GET_VIDEO_MODE( %s, %d )\n", __FUNCTION__, __LINE__ );
			return -2;
		}

		if ( ioctl( pDispCfg->fd_lcd, FB_IOCTL_GET_VIEWPORT_INFO, &stOvlySurface.viewPortInfo ) )
		{
			TRACE( "Error: FB_IOCTL_GET_VIEWPORT_INFO( %s, %d )\n", __FUNCTION__, __LINE__ );
			return -2;
		}

		if ( ioctl( pDispCfg->fd_lcd, FB_IOCTL_GET_VID_OFFSET, &stOvlySurface.viewPortOffset ) ) 
		{
			TRACE( "Error: FB_IOCTL_GET_VID_OFFSET( %s, %d )\n", __FUNCTION__, __LINE__ );
			return -2;
		}

#if defined( PXA168 )
		switch ( stOvlySurface.videoMode ) 
		{
		case FB_VMODE_YUV420PLANAR:
			stOvlySurface.videoBufferAddr.startAddr[0] = (unsigned char*)pImgBuf->iPhyAddr[0];
			stOvlySurface.videoBufferAddr.startAddr[1] = (unsigned char*)pImgBuf->iPhyAddr[1];
			stOvlySurface.videoBufferAddr.startAddr[2] = (unsigned char*)pImgBuf->iPhyAddr[2];
			break;
		case FB_VMODE_YUV422PACKED:
		case FB_VMODE_YUV422PACKED_SWAPYUorV:
		case FB_VMODE_RGB565:
			stOvlySurface.videoBufferAddr.startAddr[0] = (unsigned char*)pImgBuf->iPhyAddr[0];
			stOvlySurface.videoBufferAddr.startAddr[1] = NULL;
			stOvlySurface.videoBufferAddr.startAddr[2] = NULL;
			break;
		default:
			TRACE( "Error: Not supported format( %s, %d )\n", __FILE__, __LINE__ );
			return -1;
			break;
		}
		stOvlySurface.viewPortInfo.yPitch = pImgBuf->iStep[0];
		stOvlySurface.viewPortInfo.uPitch = pImgBuf->iStep[1];
		stOvlySurface.viewPortInfo.vPitch = pImgBuf->iStep[2];
#else //PXA950
		switch ( stOvlySurface.videoMode ) 
		{
		case FB_VMODE_YUV422PACKED:
		case FB_VMODE_YUV422PACKED_SWAPYUorV:
		case FB_VMODE_RGB565:
			stOvlySurface.videoBufferAddr.startAddr = (unsigned char*)pImgBuf->iPhyAddr[0];
			break;
		default:
			TRACE( "Error: Not supprted format( %s, %d )\n", __FILE__, __LINE__ );
			return -1;
			break;
		}
		stOvlySurface.viewPortInfo.ycPitch = pImgBuf->iStep[0];
		stOvlySurface.viewPortInfo.uvPitch = pImgBuf->iStep[1];
#endif

		/* initialize ovly surface parameter. */
		stOvlySurface.viewPortInfo.srcWidth  = width;
		stOvlySurface.viewPortInfo.srcHeight = height;

		stOvlySurface.viewPortInfo.zoomXSize = MIN( screen_width, width );
		stOvlySurface.viewPortInfo.zoomYSize = MIN( screen_height, height );
		stOvlySurface.viewPortOffset.xOffset = pDispCfg->screen_pos_x ;
		stOvlySurface.viewPortOffset.yOffset = pDispCfg->screen_pos_y;


		stOvlySurface.videoBufferAddr.frameID    = 0;
		stOvlySurface.videoBufferAddr.inputData  = 0;	//set as NULL when buf is allocated in user space
		stOvlySurface.videoBufferAddr.length	 = pImgBuf->iFilledLen[0] + pImgBuf->iFilledLen[1] + pImgBuf->iFilledLen[2];

		if ( ioctl( pDispCfg->fd_lcd, FB_IOCTL_FLIP_VID_BUFFER, &stOvlySurface ) )
		{
			TRACE( "Can't FB_IOCTL_FLIP_VID_BUFFER\n" );
			return -2;
		}

		if ( pDispCfg->on == 0 ) 
		{
			pDispCfg->on = 1;
			if ( ioctl( pDispCfg->fd_lcd, FB_IOCTL_SWITCH_VID_OVLY, &pDispCfg->on ) )
			{
				TRACE( "Error: switch video overlay fail\n" );
				close( pDispCfg->fd_lcd );
				return -1;
			}
		}

		// TRACE( "----------------- from camera: iPrivateIndex: %d, iPhyAddr[0]: %x---------------\n", pImgBuf->iPrivateIndex, pImgBuf->iPhyAddr[0] );

		for ( i = 0; i < iPrevBufNum; i++ )
		{
			if ( pPreviewBuf[i].iPhyAddr[0] == pImgBuf->iPhyAddr[0] )
			{
				pPreviewBufStatus[i] = 1; // 1 is in overlay
				ovly_buf_cnt++;
				break;
			}
		}
		// TRACE( "overlay buf cnt: %d\n", ovly_buf_cnt );

		/* get buffer list which could be released by overlay */
		if ( ioctl( pDispCfg->fd_lcd, FB_IOCTL_GET_FREELIST, &pFreeBufAddr ) )
		{
			TRACE( "Can't FB_IOCTL_GET_BUFF_ADDR\n" );
			return -1;
		} 	   

		for( i = 0; i < MAX_LCD_BUF_NUM; i++ )
		{
			if( pFreeBufAddr[i][0] ) // we use only Y-planar addr as match index
			{
				for ( j = 0; j < MAX_LCD_BUF_NUM; j++ )
				{

					if ( pFreeBufAddr[i][0] == (unsigned char*)pPreviewBuf[j].iPhyAddr[0] && pPreviewBufStatus[j] == 1 )
					{ 
						error = CAM_SendCommand( handle, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)CAM_PORT_PREVIEW, (CAM_Param)&pPreviewBuf[j] );
						// TRACE( "****************from overlay: iPrivateIndex: %d, iPhyAddr[0]: %x**************\n" ,pPreviewBuf[j].iPrivateIndex, pPreviewBuf[j].iPhyAddr[0] );
						ASSERT_CAM_ERROR( error );
						ovly_buf_cnt--;
						pPreviewBufStatus[j] = 0;
						break;
					}
				}
				if ( j >= MAX_LCD_BUF_NUM )
				{
					TRACE( "Error: cannot find match between overlay free_list and app buffer list\n" );
					return -2;
				}
			}
		}

	}
#endif

	return 0;
}

int clear_overlay( display_cfg *pDispCfg )
{
	int width, height;
	int screen_width, screen_height, format;

	int i, j;

	ASSERT( pDispCfg != NULL );

	width         = pDispCfg->frame_width;
	height        = pDispCfg->frame_height;
	screen_width  = pDispCfg->screen_width;
	screen_height = pDispCfg->screen_height;

	format        = pDispCfg->format;

	// user pointer overlay
	if ( pDispCfg->overlay[0] == NULL )
	{
		return 0;
	}

	switch( format )
	{
	case FB_VMODE_YUV422PACKED:
	case FB_VMODE_YUV422PACKED_SWAPYUorV:
		{
			int           iDstHeight;
			int           iDstStep;
			unsigned char *pDstPtr;

			iDstHeight   = height;
			iDstStep     = width * 2;

			// copy Y component
			pDstPtr = pDispCfg->overlay[0];
			for ( i = 0; i < iDstHeight; i++ )
			{
				for ( j = 0; j < width * 2; j += 4 )
				{
					pDstPtr[j+0] = 128;
					pDstPtr[j+1] = 0;
					pDstPtr[j+2] = 128;
					pDstPtr[j+3] = 0;
				}
				pDstPtr += iDstStep;
			}
		} 
		break;

	case FB_VMODE_YUV422PLANAR:
		{
			memset( pDispCfg->overlay[0], 0, width * height );
			memset( pDispCfg->overlay[1], 128, width * height / 2 );
			memset( pDispCfg->overlay[2], 128, width * height / 2 );
		} 
		break;

	case FB_VMODE_YUV420PLANAR:
		{
			memset( pDispCfg->overlay[0], 0, width * height );
			memset( pDispCfg->overlay[1], 128, width * height / 4 );
			memset( pDispCfg->overlay[2], 128, width * height / 4 );
		}
		break;
	
	case FB_VMODE_RGB565:
		{
			memset( pDispCfg->overlay[0], 0, width * height * 2 );
		}
		break;

	default:
		return -1;
	}

	return 0;
}

#else

int overlay2_open( char *dev, display_cfg *pDispCfg )
{
	return 0;
}

void overlay2_close( display_cfg *pDispCfg )
{
	return;
}

int displaydemo_open( int screen_x, int screen_y, int screen_w, int screen_h,
                      int frame_w, int frame_h, CAM_ImageFormat eFrameFormat, display_cfg *pDispCfg )
{
	return 0;
}

int displaydemo_close( display_cfg *pDispCfg )
{
	return 0;
}

int display_to_overlay( CAM_DeviceHandle handle, display_cfg *pDispCfg, CAM_ImageBuffer *pImgBuf )
{
	CAM_Error error;
	error = CAM_SendCommand( handle, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)CAM_PORT_PREVIEW, (CAM_Param)pImgBuf );
	ASSERT_CAM_ERROR( error );

	return 0;
}

int clear_overlay( display_cfg *pDispCfg )
{
	return 0;
}

#endif
