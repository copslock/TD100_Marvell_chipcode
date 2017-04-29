/*******************************************************************************
//(C) Copyright [2010 - 2011] Marvell International Ltd.
//All Rights Reserved
*******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "cam_utility.h"
#include "cam_log.h"
#include "misc.h"


/*****************************************************************************
//
//  Queue Management  
//
*****************************************************************************/

CAM_Int32s _InitQueue( _CAM_Queue *pQueue, CAM_Int32s iDataNum )
{
	ASSERT( pQueue != NULL );
	ASSERT( iDataNum > 0 );

	pQueue->iDataNum    = iDataNum;
	pQueue->iDeQueuePos = 0;
	pQueue->iQueueCount = 0;
	pQueue->pData = (void**)malloc( sizeof(void*) * iDataNum );

	if ( pQueue->pData == NULL ) 
	{
		TRACE( CAM_ERROR, "Error: malloc failed( %s, %d )\n", __FILE__, __LINE__ );
		return -1;
	}

	return 0;
}

CAM_Int32s _DeinitQueue( _CAM_Queue *pQueue )
{
	ASSERT( pQueue != NULL );

	if ( pQueue->pData ) 
	{
		free( pQueue->pData );
		pQueue->pData = NULL;
	}

	return 0;
}

CAM_Int32s _EnQueue( _CAM_Queue *pQueue, void *pData )
{
	CAM_Int32s index;
	CAM_Int32s i;

	if ( pQueue == NULL || pData == NULL ) 
	{
		return -1;
	}

	if ( pQueue->iQueueCount >= pQueue->iDataNum )
	{
		TRACE( CAM_ERROR, "Error: no available position in queue for new data [%d - %d]( %s, %s, %d )\n", 
		       pQueue->iDataNum, pQueue->iQueueCount, __FILE__, __FUNCTION__, __LINE__ );
		return -1;
	}

	// check duplicate enqueue
	for ( i = 0; i < pQueue->iQueueCount; i++ ) 
	{
		index = ( i + pQueue->iDeQueuePos ) % pQueue->iDataNum;
		if ( pQueue->pData[index] == pData ) 
		{
			break;
		}
	}
	if ( i < pQueue->iQueueCount )
	{
		TRACE( CAM_WARN, "Warning: enqueue a same element to queue( %s, %s, %d )\n", 
		       __FILE__, __FUNCTION__, __LINE__ );
		return -2;
	}

	index = ( pQueue->iDeQueuePos + pQueue->iQueueCount ) % pQueue->iDataNum;
	pQueue->pData[index] = pData;
	pQueue->iQueueCount++;

	return 0;
}

CAM_Int32s _DeQueue( _CAM_Queue *pQueue, void **ppData )
{
	if ( pQueue == NULL || ppData == NULL ) 
	{
		return -1;
	}

	if ( pQueue->iQueueCount <= 0 ) 
	{
		return -1;
	}

	*ppData = pQueue->pData[pQueue->iDeQueuePos];
	pQueue->iDeQueuePos = (pQueue->iDeQueuePos + 1) % pQueue->iDataNum;
	pQueue->iQueueCount--;

	return 0;
}

CAM_Int32s _FlushQueue( _CAM_Queue *pQueue )
{
	if ( pQueue == NULL ) 
	{
		return -1;
	}

	pQueue->iDeQueuePos = 0;
	pQueue->iQueueCount = 0;

	return 0;
}

CAM_Int32s _GetQueue( _CAM_Queue *pQueue, CAM_Int32s iIndex, void **ppData )
{
	if ( pQueue == NULL || ppData == NULL ) 
	{
		return -1;
	}

	if ( iIndex < 0 || iIndex >= pQueue->iQueueCount) 
	{
		*ppData = NULL;
		return -1;
	}

	*ppData = pQueue->pData[(pQueue->iDeQueuePos + iIndex) % pQueue->iDataNum];

	return 0;
}

CAM_Int32s _RemoveQueue( _CAM_Queue *pQueue, void *pData )
{
	CAM_Int32s i, j;

	if ( pQueue == NULL || pData == NULL ) 
	{
		return -1;
	}

	for ( i = 0; i < pQueue->iQueueCount; i++ ) 
	{
		
		CAM_Int32s index = ( i + pQueue->iDeQueuePos ) % pQueue->iDataNum, index1;

		if ( pQueue->pData[index] == pData ) 
		{
			for ( j = i + 1; j < pQueue->iQueueCount; j++ ) 
			{
				index1 = ( j + pQueue->iDeQueuePos ) % pQueue->iDataNum;
				pQueue->pData[index] = pQueue->pData[index1];
				index = index1;
			}

			pQueue->iQueueCount--;

			return 0;
		}
	}

	return -1;
}

CAM_Int32s _RetrieveQueue( _CAM_Queue *pQueue, void *pData, int *pIndex )
{
	CAM_Int32s i, index;

	if ( pQueue == NULL || pData == NULL || pIndex == NULL ) 
	{
		return -1;
	}

	*pIndex = -1;

	for ( i = 0; i < pQueue->iQueueCount; i++ ) 
	{
		index = ( i + pQueue->iDeQueuePos ) % pQueue->iDataNum;
		if ( pQueue->pData[index] == pData ) 
		{
			*pIndex = index;
			break;
		}
	}

	return 0;
}

// working thread
void _CreateThread( _CAM_Thread *pThread, void *pWorkThreadFunc, void *pData )
{
	int ret;

	pThread->hEventWakeup = NULL;
	ret = IPP_EventCreate( &pThread->hEventWakeup );
	ASSERT( ret == 0 );
	ret = IPP_EventReset( pThread->hEventWakeup );
	ASSERT( ret == 0 );

	ret = IPP_ThreadCreate( &(pThread->iThreadID), 0, pWorkThreadFunc, pData );
	ASSERT( ret == 0 );

	pThread->bExitThread = CAM_FALSE;

	pThread->hEventExitAck = NULL;
	ret = IPP_EventCreate( &pThread->hEventExitAck );
	ASSERT( ret == 0 );
	ret = IPP_EventReset( pThread->hEventExitAck );
	ASSERT( ret == 0 );

	return;
}

void _DestroyThread( _CAM_Thread *pThread )
{
	int ret;

	// reset the ack before notify
	ret = IPP_EventReset( pThread->hEventExitAck );
	ASSERT( ret == 0 );

	pThread->bExitThread = CAM_TRUE;
	CELOG( "pThread->bExitThread: %d\n", pThread->bExitThread );

	ret = IPP_EventSet( pThread->hEventWakeup );
	ASSERT( ret == 0 );

	// wait for ack for the event_monitor_thread exit
	ret = IPP_EventWait( pThread->hEventExitAck, INFINITE_WAIT, NULL );
	ASSERT( ret == 0 );

	ret = IPP_EventDestroy( pThread->hEventWakeup );
	ASSERT( ret == 0 );

	ret = IPP_EventDestroy( pThread->hEventExitAck );
	ASSERT( ret == 0 );

	pThread->iThreadID     = -1;
	pThread->hEventWakeup  = NULL;
	pThread->hEventExitAck = NULL;

	return;
}

/*********************************************************************************************************
//
// ultility functions
//
*********************************************************************************************************/


/******************************************************************************************************
//
// Name:         _calcstep
// Description:  calculate the step(s) for the image plane(s) according to 
//               the given the image format, width and buffer alignment reqirement
// Arguments:    eFormat[IN] : image format (UYVY422 etc.)
//               iWidth [IN] : image actual width, how many bytes one image row occupy
//               iAlign [IN] : byte align of each row
//               iStep  [OUT]: row step of each primary image component(0~2)
// Return:       None
// Notes:        N/A
// Version Log:  version      Date          Author     Description
//                v0.1        N/A            N/A         N/A
//                V0.11       07/15/2010   Matrix Yao  add comment coloum  
 *******************************************************************************************************/

void _calcstep( CAM_ImageFormat eFormat, CAM_Int32s iWidth, CAM_Int32s iAlign[3], CAM_Int32s iStep[3] )
{
	switch ( eFormat )
	{
	case CAM_IMGFMT_RGGB8:
	case CAM_IMGFMT_BGGR8:
	case CAM_IMGFMT_GRBG8:
	case CAM_IMGFMT_GBRG8:
		iStep[0] = _ALIGN_TO( iWidth, iAlign[0] );
		iStep[1] = iStep[2] = 0;
		break;

	case CAM_IMGFMT_RGGB10:
	case CAM_IMGFMT_BGGR10:
	case CAM_IMGFMT_GRBG10:
	case CAM_IMGFMT_GBRG10:
		iStep[0] = _ALIGN_TO( iWidth * 2, iAlign[0] );
		iStep[1] = iStep[2] = 0;
		break;

	case CAM_IMGFMT_RGB888:
	case CAM_IMGFMT_RGB666:
	case CAM_IMGFMT_BGR888:
	case CAM_IMGFMT_BGR666:
		iStep[0] = _ALIGN_TO( iWidth * 3, iAlign[0] );
		iStep[1] = iStep[2] = 0;
		break;

	case CAM_IMGFMT_RGB444:
	case CAM_IMGFMT_RGB555:
	case CAM_IMGFMT_RGB565:
	case CAM_IMGFMT_BGR444:
	case CAM_IMGFMT_BGR555:
	case CAM_IMGFMT_BGR565:
		iStep[0] = _ALIGN_TO( iWidth * 2, iAlign[0] );
		iStep[1] = iStep[2] = 0;
		break;

	case CAM_IMGFMT_YCbCr444P:
		iStep[0] = _ALIGN_TO( iWidth, iAlign[0] );
		iStep[1] = _ALIGN_TO( iWidth, iAlign[1] );
		iStep[2] = _ALIGN_TO( iWidth, iAlign[2] );
		break;

	case CAM_IMGFMT_YCbCr444I:
		iStep[0] = _ALIGN_TO( iWidth * 3, iAlign[0] );
		iStep[1] = iStep[2] = 0;
		break;

	case CAM_IMGFMT_YCbCr422P:
		iStep[0] = _ALIGN_TO( iWidth, iAlign[0] );
		iStep[1] = _ALIGN_TO( iWidth >> 1, iAlign[1] );
		iStep[2] = _ALIGN_TO( iWidth >> 1, iAlign[2] );
		break;

	case CAM_IMGFMT_YCbYCr:
	case CAM_IMGFMT_CbYCrY:
	case CAM_IMGFMT_YCrYCb:
	case CAM_IMGFMT_CrYCbY:
		iStep[0] = _ALIGN_TO( iWidth * 2, iAlign[0] );
		iStep[1] = iStep[2] = 0;
		break;

	case CAM_IMGFMT_YCbCr420P:
		iStep[0] = _ALIGN_TO( iWidth, iAlign[0] );
		iStep[1] = _ALIGN_TO( iWidth >> 1, iAlign[1] );
		iStep[2] = _ALIGN_TO( iWidth >> 1, iAlign[2] );
		break;

	case CAM_IMGFMT_YCbCr420SP:
	case CAM_IMGFMT_YCrCb420SP:
		iStep[0] = _ALIGN_TO( iWidth, iAlign[0] );
		iStep[1] = _ALIGN_TO( iWidth, iAlign[1] );
		iStep[2] = 0;
		break;

	case CAM_IMGFMT_JPEG:
		iStep[0] = iStep[1] = iStep[2] = 0;
		break;

	case CAM_IMGFMT_BGRA8888:
	case CAM_IMGFMT_RGBA8888:
		iStep[0] = _ALIGN_TO( iWidth * 4, iAlign[0] );
		iStep[1] = iStep[2] = 0;
		break;

	default:
		TRACE( CAM_ERROR, "Error: bad format( %d )\n", eFormat );
		ASSERT( 0 );
		break;
	}
	
	return;
}

/******************************************************************************************************
//
// Name:         _calcjpegbuflen
// Description:  calculate the minimum required buffer length(s) according to quality, resolution
// Arguments:    N/A
// Return:       CAM_Int32s
// Notes:        This is an experimental formula, you can modify it according to your sensor capability
// Version Log:  version      Date          Author     Description
//                V0.11       11/25/2010   Matrix Yao   create
 *******************************************************************************************************/
CAM_Int32s _calcjpegbuflen( CAM_Int32s iWidth, CAM_Int32s iHeight, CAM_JpegParam *pJpegParam )
{
	// image size = raw size / 2 ( basic compression ratio ) * jpeg quality factor, here basic compression ratio
	// is only a empirical value which based on the observation that the max JPEG bpp is ~7bpp.  
	CAM_Int32s iBufSize = _ALIGN_TO( iWidth * iHeight * pJpegParam->iQualityFactor / 200, 32 );
	ASSERT( iBufSize != 0 );
	return iBufSize;
}


/******************************************************************************************************
//
// Name:         _calcbuflen
// Description:  calculate the minimum required buffer length(s) according to
//               the given image format, width, height, step(s)
// Arguments:    N/A
// Return:       None
// Notes:        for DMA coherency, the buffer len should be 32 byte align
// Version Log:  version      Date          Author     Description
//                v0.1        N/A            N/A         N/A
//                V0.11       07/15/2010   Matrix Yao  add comment coloum  
 *******************************************************************************************************/

void _calcbuflen( CAM_ImageFormat eFormat, CAM_Int32s iWidth, CAM_Int32s iHeight, CAM_Int32s iStep[3], CAM_Int32s iLength[3], void *pUserData )
{
	switch ( eFormat )
	{
		case CAM_IMGFMT_RGGB8:
		case CAM_IMGFMT_BGGR8:
		case CAM_IMGFMT_GRBG8:
		case CAM_IMGFMT_GBRG8:

		case CAM_IMGFMT_RGGB10:
		case CAM_IMGFMT_BGGR10:
		case CAM_IMGFMT_GRBG10:
		case CAM_IMGFMT_GBRG10:

		case CAM_IMGFMT_RGB888:
		case CAM_IMGFMT_RGB666:

		case CAM_IMGFMT_BGR888:
		case CAM_IMGFMT_BGR666:

		case CAM_IMGFMT_RGB444:
		case CAM_IMGFMT_RGB555:
		case CAM_IMGFMT_RGB565:

		case CAM_IMGFMT_BGR444:
		case CAM_IMGFMT_BGR555:
		case CAM_IMGFMT_BGR565:

		case CAM_IMGFMT_BGRA8888:
		case CAM_IMGFMT_RGBA8888:
			iLength[0] = _ALIGN_TO( iHeight * iStep[0], 32 );
			iLength[1] = iLength[2] = 0;
			break;

		case CAM_IMGFMT_YCbCr444P:
			iLength[0] = _ALIGN_TO( iHeight * iStep[0], 32 );
			iLength[1] = _ALIGN_TO( iHeight * iStep[1], 32 );
			iLength[2] = _ALIGN_TO( iHeight * iStep[2], 32 );
			break;

		case CAM_IMGFMT_YCbCr444I:
			iLength[0] = _ALIGN_TO( iHeight * iStep[0], 32 );
			iLength[1] = iLength[2] = 0;
			break;

		case CAM_IMGFMT_YCbCr422P:
			iLength[0] = _ALIGN_TO( iHeight * iStep[0], 32 );
			iLength[1] = _ALIGN_TO( iHeight * iStep[1], 32 );
			iLength[2] = _ALIGN_TO( iHeight * iStep[2], 32 );
 			break;

		case CAM_IMGFMT_YCbYCr:
		case CAM_IMGFMT_CbYCrY:
		case CAM_IMGFMT_YCrYCb:
		case CAM_IMGFMT_CrYCbY:
			iLength[0] = _ALIGN_TO( iHeight * iStep[0], 32 );
			iLength[1] = iLength[2] = 0;
			break;

		case CAM_IMGFMT_YCbCr420P:
			iLength[0] = _ALIGN_TO( iHeight * iStep[0], 32 );
			iLength[1] = _ALIGN_TO( ( iHeight >> 1 ) * iStep[1], 32 );
			iLength[2] = _ALIGN_TO( ( iHeight >> 1 ) * iStep[2], 32 );
			break;

		case CAM_IMGFMT_YCbCr420SP:
		case CAM_IMGFMT_YCrCb420SP:
			iLength[0] = _ALIGN_TO( iHeight * iStep[0], 32 );
			iLength[1] = _ALIGN_TO( ( iHeight >> 1 ) * iStep[1], 32 );
			iLength[2] = 0;
			break;

		case CAM_IMGFMT_JPEG:
			{
				CAM_JpegParam *pJpegParam = (CAM_JpegParam*)pUserData;
				CAM_Int32s iJpegLen = 0;
				ASSERT( pUserData != NULL );
				iJpegLen = _calcjpegbuflen( iWidth, iHeight, pJpegParam );
				ASSERT( iJpegLen != 0 );
				iLength[0] = _ALIGN_TO( iJpegLen, 32 ); 
				iLength[1] = iLength[2] = 0;
			}
			break;
 
		default:
			TRACE( CAM_ERROR, "Error: bad format( %d )\n", eFormat );
			ASSERT( 0 );
			break;
	}
	return;
}

/******************************************************************************************************
//
// Name:         _calcimglen
// Description:  calculate the actual image size according to
//               the given image format, width, height, step(s)
// Arguments:    N/A
// Return:       None
// Notes:        N/A
// Version Log:  version      Date          Author     Description
//                V0.1       02/11/2011   Matrix Yao     create  
 *******************************************************************************************************/

void _calcimglen( CAM_ImageBuffer *pImgBuf )
{
	switch ( pImgBuf->eFormat )
	{
		case CAM_IMGFMT_RGGB8:
		case CAM_IMGFMT_BGGR8:
		case CAM_IMGFMT_GRBG8:
		case CAM_IMGFMT_GBRG8:

		case CAM_IMGFMT_RGGB10:
		case CAM_IMGFMT_BGGR10:
		case CAM_IMGFMT_GRBG10:
		case CAM_IMGFMT_GBRG10:

		case CAM_IMGFMT_RGB888:
		case CAM_IMGFMT_RGB666:

		case CAM_IMGFMT_BGR888:
		case CAM_IMGFMT_BGR666:

		case CAM_IMGFMT_RGB444:
		case CAM_IMGFMT_RGB555:
		case CAM_IMGFMT_RGB565:

		case CAM_IMGFMT_BGR444:
		case CAM_IMGFMT_BGR555:
		case CAM_IMGFMT_BGR565:

		case CAM_IMGFMT_BGRA8888:
		case CAM_IMGFMT_RGBA8888:

		case CAM_IMGFMT_YCbYCr:
		case CAM_IMGFMT_CbYCrY:
		case CAM_IMGFMT_YCrYCb:
		case CAM_IMGFMT_CrYCbY:
		
		case CAM_IMGFMT_YCbCr444I:
			pImgBuf->iFilledLen[0] = pImgBuf->iHeight * pImgBuf->iStep[0];
			pImgBuf->iFilledLen[1] = pImgBuf->iFilledLen[2] = 0;
			break;

		case CAM_IMGFMT_YCbCr444P:
			pImgBuf->iFilledLen[0] = _ALIGN_TO( pImgBuf->iHeight * pImgBuf->iStep[0], 32 );
 			pImgBuf->iFilledLen[1] = _ALIGN_TO( pImgBuf->iHeight * pImgBuf->iStep[1], 32 );
 			pImgBuf->iFilledLen[2] = pImgBuf->iHeight * pImgBuf->iStep[2];
			break;

		case CAM_IMGFMT_YCbCr422P:
 			pImgBuf->iFilledLen[0] = _ALIGN_TO( pImgBuf->iHeight * pImgBuf->iStep[0], 32 );
 			pImgBuf->iFilledLen[1] = _ALIGN_TO( pImgBuf->iHeight * pImgBuf->iStep[1], 32 );
			pImgBuf->iFilledLen[2] = pImgBuf->iHeight * pImgBuf->iStep[2];
			break;

		case CAM_IMGFMT_YCbCr420P:
			pImgBuf->iFilledLen[0] = _ALIGN_TO( pImgBuf->iHeight * pImgBuf->iStep[0], 32 );
			pImgBuf->iFilledLen[1] = _ALIGN_TO( ( pImgBuf->iHeight >> 1 ) * pImgBuf->iStep[1], 32 );
			pImgBuf->iFilledLen[2] = ( pImgBuf->iHeight >> 1 ) * pImgBuf->iStep[2];
			break;

		case CAM_IMGFMT_YCbCr420SP:
		case CAM_IMGFMT_YCrCb420SP:
			pImgBuf->iFilledLen[0] = _ALIGN_TO( pImgBuf->iHeight * pImgBuf->iStep[0], 32 );
			pImgBuf->iFilledLen[1] = ( pImgBuf->iHeight >> 1 ) * pImgBuf->iStep[1];
			pImgBuf->iFilledLen[2] = 0;
			break;

		case CAM_IMGFMT_JPEG:
		{
			CAM_Int32s i;

			// check if it's JPEG data( by check SOI tag( 0xFFD8 ) )
			if ( pImgBuf->pBuffer[0][0] != 0xFF || pImgBuf->pBuffer[0][1] != 0xD8  )
			{
				FILE *fp = NULL;
#if defined( ANDROID )
				char fname[256] = "/data/badjpeg.dat";
#else
				char fname[256] = "badjpeg.dat";
#endif
				fp = fopen( fname, "wb" );

				fwrite( pImgBuf->pBuffer[0], pImgBuf->iAllocLen[0], 1, fp );

				fflush( fp );

				fclose( fp );

				fp = NULL;

				TRACE( CAM_ERROR, "Error: bad jpeg data, didn't find SOI tag, you can find the dumped data in: %s[%s, %d]\n",
				       fname, __FILE__, __LINE__ );
				ASSERT( 0 );
			}
			// get size by search JPEG EOI tag( 0xFFD9 )
			for ( i = 0; i < (pImgBuf->iAllocLen[0] - 1); i++ )
			{
				if ( pImgBuf->pBuffer[0][i] == 0xFF && pImgBuf->pBuffer[0][i+1] == 0xD9 )
				{
					break;
				}
			}

			if ( i < (pImgBuf->iAllocLen[0] - 1) )
			{
				pImgBuf->iFilledLen[0] = i + 2;
				pImgBuf->iFilledLen[1] = 0;
				pImgBuf->iFilledLen[2] = 0;
			}
			else
			{
				// JPEG is error in this case. 
				// Currently we do not report error and do not modify iFilledLen so that the application will not know about this error.
				TRACE( CAM_WARN, "Warn: something must be wrong, cannot find the EOI tag of JPEG image\n" );
				pImgBuf->iFilledLen[0] = pImgBuf->iAllocLen[0];
				pImgBuf->iFilledLen[1] = 0;
				pImgBuf->iFilledLen[2] = 0;
			}
			TRACE( CAM_INFO, "Info: Filled JPEG len: %d\n", pImgBuf->iFilledLen[0] );
		}
		break;
 
	default:
		TRACE( CAM_ERROR, "Error: bad format( %d )\n", pImgBuf->eFormat );
		ASSERT( 0 );
		break;
	}

	return;
}



/******************************************************************************************************
//
// Name:         _calccrop
// Description:  sensor output size may mismatch LCD screen in width-height-ratio, in
//               which case we need to crop sensor output to fit the latter.This  
//               function will crop image to keep aspect-ratio.     
// Arguments:    iWidth, iHeight[IN] :  original size of sensor output
//               iRatioW, iRatioH[IN]:  LCD screen width-height-ratio 
//               pRect[OUT]          :  pointer to the cropping window
// Return:       None
// Notes:        N/A
// Version Log:  version      Date          Author     Description
//                v0.1        N/A            N/A         N/A
//                V0.11       07/15/2010   Matrix Yao  add comment coloum  
 *******************************************************************************************************/

void _calccrop( CAM_Int32s iWidth, CAM_Int32s iHeight, CAM_Int32s iRatioW, CAM_Int32s iRatioH, CAM_Rect *pRect )
{
	// case w/h >= ratioW/ratioH: cut width
	if ( iWidth * iRatioH >= iHeight * iRatioW ) 
	{
		pRect->iHeight = iHeight;
		pRect->iWidth  = iHeight * iRatioW / iRatioH;
		pRect->iLeft   = ( iWidth - pRect->iWidth ) >> 1;
		pRect->iTop    = 0;
	}
	//case w/h < ratioW/ratioH: cut height
	else 
	{
		pRect->iHeight = iWidth * iRatioH / iRatioW;
		pRect->iWidth  = iWidth;
		pRect->iLeft   = 0;
		pRect->iTop    = ( iHeight - pRect->iHeight ) >> 1;
	}
	return;
}

/******************************************************************************************************
//
// Name:         _calcbyteperpix
// Description:  calc each image format's bytes per pixel    
// Arguments:    eFormat: image format
// Return:       None
// Notes:        N/A
// Version Log:  version      Date          Author     Description
//                V0.11       11/16/2010   Matrix Yao    create  
 *******************************************************************************************************/
CAM_Int32s _calcbyteperpix( CAM_ImageFormat eFormat )
{
	CAM_Int32s iBytePerPixel = 0;

	switch ( eFormat )
	{
	case CAM_IMGFMT_YCbYCr:
	case CAM_IMGFMT_CbYCrY:
	case CAM_IMGFMT_YCrYCb:
	case CAM_IMGFMT_CrYCbY:
	case CAM_IMGFMT_BGR565:
		iBytePerPixel = 2;
		break;

	case CAM_IMGFMT_RGB888:
	case CAM_IMGFMT_BGR888:
		iBytePerPixel = 3;
		break;

	case CAM_IMGFMT_BGRA8888:
	case CAM_IMGFMT_RGBA8888:
		iBytePerPixel = 4;
		break;

	default:
		TRACE( CAM_ERROR, "Error: No suitable BytePerPixel formula for format[%d]\n", eFormat );
		ASSERT( 0 );
		break;
	
	}

	return iBytePerPixel;
}

/***************************************************************************************
 * functionality: get the number of pixels per sample coordiated to color space layout and sample rate
 * input        : eFormat    : image data format
 * output       : pPixH      : pointer to horizontal pixels per sample     
                  pPixV      : pointer to vertical pixels per sample
 **************************************************************************************/
void _calcpixpersample( CAM_ImageFormat eFormat, CAM_Int32s *pPixH, CAM_Int32s *pPixV )
{
	switch ( eFormat )
	{
	case CAM_IMGFMT_YCbYCr:
	case CAM_IMGFMT_CbYCrY:
	case CAM_IMGFMT_YCrYCb:
	case CAM_IMGFMT_CrYCbY:
		*pPixH = 2;
		*pPixV = 1;
		break;
		
	case CAM_IMGFMT_RGGB8:
	case CAM_IMGFMT_BGGR8:
	case CAM_IMGFMT_GRBG8:
	case CAM_IMGFMT_GBRG8:
		*pPixH = 2;
		*pPixV = 2;
		break;

	default:
		*pPixH = -1;
		*pPixV = -1;
		break;
	}

	return;
}

