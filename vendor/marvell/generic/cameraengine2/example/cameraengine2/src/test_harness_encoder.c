/******************************************************************************
*(C) Copyright [2010 - 2011] Marvell International Ltd.
* All Rights Reserved
******************************************************************************/

#include "test_harness.h"
#include "test_harness_encoder.h"
#include "misc.h"

#ifdef _ENABLE_VIDEO_ENCODER_
#include "OMX_Core.h"
#include "OMX_Component.h"
#include "OMX_IppDef.h"
#include "IppOmxLogger_Def.h"
#include "codecVC.h"
#endif

#if defined( BONNELL )

	#define _OMX_MPEG4_ENCODER_ "OMX.MARVELL.VIDEO.VMETAENCODER"
	#define _OMX_H263_ENCODER_  "OMX.MARVELL.VIDEO.VMETAENCODER"
	#define _OMX_H264_ENCODER_  "OMX.MARVELL.VIDEO.VMETAENCODER"

#elif defined( BROWNSTONE )

	#define _OMX_MPEG4_ENCODER_ "OMX.MARVELL.VIDEO.VMETAENCODER"
	#define _OMX_H263_ENCODER_  "OMX.MARVELL.VIDEO.VMETAENCODER"
	#define _OMX_H264_ENCODER_  "OMX.MARVELL.VIDEO.VMETAENCODER"

#elif defined( ABILENE )

	#define _OMX_MPEG4_ENCODER_ "OMX.MARVELL.VIDEO.VMETAENCODER"
	#define _OMX_H263_ENCODER_  "OMX.MARVELL.VIDEO.VMETAENCODER"
	#define _OMX_H264_ENCODER_  "OMX.MARVELL.VIDEO.VMETAENCODER"

#elif defined( FLINT )

	#define _OMX_MPEG4_ENCODER_ "OMX.MARVELL.VIDEO.VMETAENCODER"
	#define _OMX_H263_ENCODER_  "OMX.MARVELL.VIDEO.VMETAENCODER"
	#define _OMX_H264_ENCODER_  "OMX.MARVELL.VIDEO.VMETAENCODER"

#elif defined( TTCDKB )

	#define _OMX_MPEG4_ENCODER_ "OMX.MARVELL.VIDEO.MPEG4ENCODER"
	#define _OMX_H263_ENCODER_  "OMX.MARVELL.VIDEO.H263ENCODER"
	#define _OMX_H264_ENCODER_  "OMX.MARVELL.VIDEO.H264ENCODER"

#elif defined( TDDKB )

	#define _OMX_MPEG4_ENCODER_ "OMX.MARVELL.VIDEO.CODADX8ENCODER"
	#define _OMX_H263_ENCODER_  "OMX.MARVELL.VIDEO.CODADX8ENCODER"
	#define _OMX_H264_ENCODER_  "OMX.MARVELL.VIDEO.CODADX8ENCODER"

#elif defined( PV2EVB )

	#define _OMX_MPEG4_ENCODER_ "OMX.MARVELL.VIDEO.MPEG4ENCODERMVED"
	#define _OMX_H263_ENCODER_  "OMX.MARVELL.VIDEO.H263ENCODERMVED"
	#define _OMX_H264_ENCODER_  "OMX.MARVELL.VIDEO.H264ENCODERMVED"

#elif defined( PV2SAARB )

	#define _OMX_MPEG4_ENCODER_ "OMX.MARVELL.VIDEO.MPEG4ENCODERMVED"
	#define _OMX_H263_ENCODER_  "OMX.MARVELL.VIDEO.H263ENCODERMVED"
	#define _OMX_H264_ENCODER_  "OMX.MARVELL.VIDEO.H264ENCODERMVED"

#elif defined( MG1SAARB )

	#define _OMX_MPEG4_ENCODER_ "OMX.MARVELL.VIDEO.VMETAENCODER"
	#define _OMX_H263_ENCODER_  "OMX.MARVELL.VIDEO.VMETAENCODER"
	#define _OMX_H264_ENCODER_  "OMX.MARVELL.VIDEO.VMETAENCODER"

#else

	#error no platform is defined!

#endif


typedef void *HANDLE;

typedef struct 
{
	int                          iEncoderType;

	CAM_DeviceHandle             hCameraEngine;
	FILE                         *hFile;

	CAM_ImageBuffer              *pImgBuf;
	int                          iImgBufCnt;

	void                         *hOmxEncoder;
#ifdef _ENABLE_VIDEO_ENCODER_
	// omx related
	OMX_U8                       sOmxCompName[128];
	OMX_VERSIONTYPE              unOmxCompVersion;
	OMX_VERSIONTYPE              unOmxSpecVersion;
	OMX_UUIDTYPE                 sOmxUUID;
	OMX_PARAM_PORTDEFINITIONTYPE stInPortDef;
	OMX_PARAM_PORTDEFINITIONTYPE stOutPortDef;
	int                          iCompBufCnt;
	int                          iCompBufSize;
	HANDLE                       hEvent;
#endif

} VideoEncoderChain;


#ifdef _ENABLE_VIDEO_ENCODER_


#define INPUTPORT	0
#define OUTPUTPORT	1

#define MAX_WAIT_TIME	500
#define INFINITE		0xFFFFFFFF

#define MAX_INPUT_BUF_NUM   32
#define MAX_OUTPUT_BUF_NUM  32


// #define TRACK_BUFFER_CNT


OMX_BUFFERHEADERTYPE *g_pInBufHeader[MAX_INPUT_BUF_NUM], *g_pOutBufHeader[MAX_OUTPUT_BUF_NUM];

static int g_iEOSStatus      = 0;     // 0 - normal processing, 1 - eos is required, 2 - eos is sent to input, 3 - eos is received from output
static int g_iEmptyBufferCnt = 0;


OMX_ERRORTYPE IppOMXComp_EventHandler( OMX_IN OMX_HANDLETYPE hCompHandle,
                                       OMX_IN OMX_PTR pAppData, 
                                       OMX_IN OMX_EVENTTYPE eEvent, 
                                       OMX_IN OMX_U32 nData1, 
                                       OMX_IN OMX_U32 nData2, 
                                       OMX_IN OMX_PTR pEventData )
{
	VideoEncoderChain *hVideo = (VideoEncoderChain*)pAppData;

	switch ( eEvent )
	{
	case OMX_EventCmdComplete:
		switch ( nData1 )
		{
		case OMX_CommandStateSet:
			TRACE( "Successfully transition into state %d\r\n", (int)nData2 );
			IPP_EventSet( hVideo->hEvent );
			break;
		case OMX_CommandFlush:
			TRACE( "Successfully flush port %d\r\n", (int)nData2 );
			break;
		default:
			TRACE( "Unhandled OMX_EventCmdComplete event( nData1 == %d )\n", (int)nData1 );
			break;
		}
		break;

	case OMX_EventError:
		switch ( nData1 )
		{
		case OMX_ErrorPortUnpopulated:
			TRACE( "Port has lost one or more of its buffers and it thus unpopulated.\n" );
			break;
		case OMX_ErrorInvalidState:
			TRACE( "Component is in Invalid State\n" );
			IPP_EventSet( hVideo->hEvent );
			break;  
		default:
			TRACE( "Encounter Error!!! Error code: %x( %s, %d )\r\n", (int)nData1, __FUNCTION__, __LINE__ );
			break;
		}
		break;

	case OMX_EventPortSettingsChanged:
		TRACE( "Port setting change is not handled!\n" );
		break;

	case OMX_EventBufferFlag:
		TRACE( "Port %d has \n", (int)nData1 );
		TRACE( "flag %d\r\n", (int)nData2 );
		break;

	default:
		TRACE( "EventType is %x\r\n", eEvent );
		TRACE( "Info: %s\r\n", (OMX_STRING)pEventData );
		break;
	}

	return OMX_ErrorNone;
}



OMX_ERRORTYPE IppOMXComp_EmptyBufferDone( OMX_IN OMX_HANDLETYPE hComponent,
                                          OMX_IN OMX_PTR pAppData,
                                          OMX_IN OMX_BUFFERHEADERTYPE *pBuffer )
{
	VideoEncoderChain *hVideo = (VideoEncoderChain*)pAppData;
	CAM_Error         error   = CAM_ERROR_NONE;
	CAM_ImageBuffer   *pImage = (CAM_ImageBuffer*)pBuffer->pAppPrivate;

	error = CAM_SendCommand( hVideo->hCameraEngine, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)CAM_PORT_VIDEO, (CAM_Param)pImage );
	ASSERT_CAM_ERROR( error );

#ifdef TRACK_BUFFER_CNT
	TRACE( "Encoder buffer cnt: %d\n", --g_iEmptyBufferCnt );
#endif

	return OMX_ErrorNone;
}



OMX_ERRORTYPE IppOMXComp_FillBufferDone( OMX_IN OMX_HANDLETYPE hComponent,
                                         OMX_IN OMX_PTR pAppData,
                                         OMX_IN OMX_BUFFERHEADERTYPE *pBuffer )
{
	VideoEncoderChain *hVideo = (VideoEncoderChain*)pAppData;

	if ( g_iEOSStatus == 2 )
	{
		TRACE( "FillBufferDone after EOS!\n" );
	}

	if ( pBuffer->nFlags & OMX_BUFFERFLAG_EOS )
	{
		TRACE( "EOS received from the output port!\n" );
		g_iEOSStatus = 3;
	}

	fwrite( pBuffer->pBuffer, 1, pBuffer->nFilledLen, hVideo->hFile );
	pBuffer->nFilledLen = 0;

	if ( g_iEOSStatus == 0 || g_iEOSStatus == 1 )
	{
		OMX_ERRORTYPE err = OMX_FillThisBuffer( hComponent, pBuffer );
		ASSERT( err == OMX_ErrorNone );
	}

	return OMX_ErrorNone;
}


static OMX_ERRORTYPE IppOMXComp_Loaded2Idle( VideoEncoderChain *hVideo )
{
	OMX_ERRORTYPE omx_error = OMX_ErrorNone;
	OMX_BOOL      bTimeOut  = CAM_FALSE;
	OMX_STATETYPE eState;
	int           i;

	TRACE( "%s start!\n", __FUNCTION__ );
	IPP_EventReset( hVideo->hEvent );

	// TRACE( "InputPort.nBufferSize = %d\n", g_InputPort.nBufferSize );

	for ( i = 0; i < hVideo->iImgBufCnt; i++ )
	{
		CAM_Int32s iBufferLen = hVideo->pImgBuf[i].iAllocLen[0] + hVideo->pImgBuf[i].iAllocLen[1] + hVideo->pImgBuf[i].iAllocLen[2];
		omx_error = OMX_UseBuffer( hVideo->hOmxEncoder, (OMX_BUFFERHEADERTYPE**)&g_pInBufHeader[i],
		                           INPUTPORT, &hVideo->pImgBuf[i], iBufferLen,
		                           hVideo->pImgBuf[i].pBuffer[0] );
		if ( omx_error != OMX_ErrorNone )
		{
			TRACE( "hVideo->stInPortDef.format.video.nFrameWidth  = %u\n", hVideo->stInPortDef.format.video.nFrameWidth );
			TRACE( "hVideo->stInPortDef.format.video.nFrameHeight = %u\n", hVideo->stInPortDef.format.video.nFrameHeight );
			TRACE( "hVideo->pImgBuf[i].iAllocLen[0]               = %d\n", hVideo->pImgBuf[i].iAllocLen[0] );
			TRACE( "hVideo->pImgBuf[i].iAllocLen[1]               = %d\n", hVideo->pImgBuf[i].iAllocLen[1] );
			TRACE( "hVideo->pImgBuf[i].iAllocLen[2]               = %d\n", hVideo->pImgBuf[i].iAllocLen[2] );	
			TRACE( "hVideo->pImgBuf[i].pBuffer[0]                 = %p\n", hVideo->pImgBuf[i].pBuffer[0] );
			TRACE( "Use Buffer( Input Port ) Error! Error Index   = 0x%x\n", omx_error );

			return omx_error;
		}
		TRACE( "use input buffer id: 0x%x\n", (int)g_pInBufHeader[i] );
	}

	TRACE( "hVideo->iCompBufCnt: %d\n", hVideo->iCompBufCnt );
	for ( i = 0; i < hVideo->iCompBufCnt; i++ )
	{
		omx_error = OMX_AllocateBuffer( hVideo->hOmxEncoder, (OMX_BUFFERHEADERTYPE**)&g_pOutBufHeader[i], OUTPUTPORT, NULL, hVideo->iCompBufSize );
		if ( omx_error != OMX_ErrorNone )
		{
			TRACE( "Allocate Buffer( Output Port ) Error! Error Index = 0x%x\r\n", omx_error );
			return omx_error;
		}
		TRACE( "allocate output buffer id: 0x%x( %s, %d )\n", (int)g_pOutBufHeader[i], __FILE__, __LINE__ );
	}

	omx_error = OMX_SendCommand( hVideo->hOmxEncoder, OMX_CommandStateSet, OMX_StateIdle, NULL );
	if ( omx_error != OMX_ErrorNone )
	{
		TRACE( "Error Index = 0x%x( %s, %d )\n", omx_error, __FILE__, __LINE__ );
		return omx_error;
	}

	IPP_EventWait( hVideo->hEvent, 1000, &bTimeOut );
	// IPP_EventWait( hVideo->hEvent, INFINITE_WAIT, NULL );
	if ( bTimeOut )
	{
		TRACE( "fail to transit to idle!\n" );
		return OMX_ErrorIncorrectStateTransition;
	}

	OMX_GetState( hVideo->hOmxEncoder, &eState );
	TRACE( "Current Component state is %d\r\n", eState );

	TRACE( "%s end!\n", __FUNCTION__ );
	return omx_error;
}

static OMX_ERRORTYPE IppOMXComp_Idle2Exe( VideoEncoderChain *hVideo )
{
	OMX_ERRORTYPE omx_error = OMX_ErrorNone;
	OMX_BOOL      bTimeOut;
	OMX_STATETYPE eState;
	int           i;

	TRACE( "Idle2Exe start!\n" );

	g_iEOSStatus      = 0;
	g_iEmptyBufferCnt = 0;

	IPP_EventReset( hVideo->hEvent );
	omx_error = OMX_SendCommand( hVideo->hOmxEncoder, OMX_CommandStateSet, OMX_StateExecuting, NULL );
	IPP_EventWait( hVideo->hEvent, INFINITE_WAIT, &bTimeOut );
	if ( bTimeOut )
	{
		TRACE("fail to transit to exe!\n");
		return OMX_ErrorIncorrectStateTransition;
	}

	OMX_GetState( hVideo->hOmxEncoder, &eState );
	TRACE( "Current Component state is %d\r\n", eState );

	// Enqueue all the buffers
	for ( i = 0; i < hVideo->iCompBufCnt; i++ )
	{
		OMX_FillThisBuffer( hVideo->hOmxEncoder, g_pOutBufHeader[i] );
	}

	TRACE( "Idle2Exe end!\n" );
	return omx_error;
}

static OMX_ERRORTYPE IppOMXComp_Exe2Idle(VideoEncoderChain *hVideo)
{
	OMX_ERRORTYPE omx_error = OMX_ErrorNone;
	OMX_BOOL      bTimeOut;
	OMX_STATETYPE eState;

	TRACE( "Exe2Idle start!\n" );
//	if ( g_iEOSStatus == 0 )
//	{
//		g_iEOSStatus = 1;
//	}

	IPP_EventReset( hVideo->hEvent );
	omx_error = OMX_SendCommand( hVideo->hOmxEncoder, OMX_CommandStateSet, OMX_StateIdle, NULL );
	IPP_EventWait( hVideo->hEvent, INFINITE_WAIT, &bTimeOut );
	OMX_GetState( hVideo->hOmxEncoder, &eState );
	TRACE( "Current Component state is %d\r\n", eState );

	TRACE( "Exe2Idle end!\n" );
	return omx_error;
}

static OMX_ERRORTYPE IppOMXComp_Idle2Loaded( VideoEncoderChain *hVideo )
{
	OMX_ERRORTYPE omx_error = OMX_ErrorNone;
	OMX_BOOL      bTimeOut;
	OMX_STATETYPE eState;
	int           i;

	TRACE( "Idle2Loaded start!\n" );
	IPP_EventReset( hVideo->hEvent );
	omx_error = OMX_SendCommand( hVideo->hOmxEncoder, OMX_CommandStateSet, OMX_StateLoaded, NULL );
	for ( i = 0; i < hVideo->iImgBufCnt; i++ )
	{
		TRACE( "free input buffer header %d\n", i );
		OMX_FreeBuffer( hVideo->hOmxEncoder, INPUTPORT, (OMX_BUFFERHEADERTYPE*)g_pInBufHeader[i] );
	}

	for ( i = 0; i < hVideo->iCompBufCnt; i++ )
	{
		TRACE( "free output buffer header %d\n", i );
		OMX_FreeBuffer( hVideo->hOmxEncoder, OUTPUTPORT, (OMX_BUFFERHEADERTYPE*)g_pOutBufHeader[i] );
	}

	IPP_EventWait( hVideo->hEvent, INFINITE_WAIT, &bTimeOut );
	OMX_GetState( hVideo->hOmxEncoder, &eState );
	TRACE( "Current Component state is %d\r\n", eState );

	TRACE( "Idle2Loaded end!\n" );
	return omx_error;
}

#endif // _ENABLE_VIDEO_ENCODER_




void* VideoEncoder_Init( CAM_DeviceHandle handle, CAM_ImageBuffer *pImgBuf, int iImgBufCnt, int iEncoderType, const char *sFileName )
{
	VideoEncoderChain *hVideo = NULL;
		
	hVideo = (VideoEncoderChain*)malloc( sizeof( VideoEncoderChain ) );
	if ( hVideo == NULL )
	{
		return hVideo;
	}

	memset( hVideo, 0, sizeof( VideoEncoderChain ) );
	hVideo->hCameraEngine = handle;
	hVideo->iEncoderType  = iEncoderType;
	hVideo->iImgBufCnt    = iImgBufCnt;
	hVideo->pImgBuf       = pImgBuf;

	ASSERT( iEncoderType >= 0 );

	hVideo->hFile = fopen( sFileName, "wb" );
	if ( hVideo->hFile == NULL )
	{
		free( hVideo );
		hVideo = NULL;
		return hVideo;
	}

	if ( iEncoderType == 0 )
	{
		hVideo->hOmxEncoder = NULL;
		return hVideo;
	}

#ifdef _ENABLE_VIDEO_ENCODER_
	else
	{
		OMX_ERRORTYPE    omx_error;
		OMX_CALLBACKTYPE stCallBack      = { IppOMXComp_EventHandler, IppOMXComp_EmptyBufferDone, IppOMXComp_FillBufferDone };
		char             *pComponentName = NULL;
		
		omx_error = OMX_Init();

		if ( OMX_ErrorNone != omx_error )
		{
			TRACE( "fail to init OpenMax!\n" );
			goto quit;
		}

		switch ( iEncoderType )
		{
		case 1: // mpeg4
			pComponentName = _OMX_MPEG4_ENCODER_;
			break;

		case 2: // h.263
			pComponentName = _OMX_H263_ENCODER_;
			break;

		case 3: // h.264
			pComponentName = _OMX_H264_ENCODER_;
			break;
		}

		// Get video encoder handle
		omx_error = OMX_GetHandle( &hVideo->hOmxEncoder, pComponentName, hVideo, &stCallBack );
		if ( OMX_ErrorNone != omx_error )
		{
			TRACE( "fail to load component!\n" );
			goto quit;
		}
		else
		{
			TRACE( "getHandle successful( %p )!\n", hVideo->hOmxEncoder );
		}

		// Get component version
		omx_error = OMX_GetComponentVersion( hVideo->hOmxEncoder, hVideo->sOmxCompName, &hVideo->unOmxCompVersion, &hVideo->unOmxSpecVersion, &hVideo->sOmxUUID );
		if ( OMX_ErrorNone == omx_error )
		{
			TRACE( "Component Version = %x.", hVideo->unOmxCompVersion.s.nVersionMajor );
			TRACE( "%x.", hVideo->unOmxCompVersion.s.nVersionMinor );
			TRACE( "%x.", hVideo->unOmxCompVersion.s.nRevision );
			TRACE( "%x\r\n", hVideo->unOmxCompVersion.s.nStep );
		}

		// Get component input port info
		hVideo->stInPortDef.nSize                     = sizeof( OMX_PARAM_PORTDEFINITIONTYPE );
		hVideo->stInPortDef.nVersion.nVersion         = hVideo->unOmxCompVersion.nVersion;
		hVideo->stInPortDef.nPortIndex                = INPUTPORT;
		omx_error = OMX_GetParameter( hVideo->hOmxEncoder, OMX_IndexParamPortDefinition, &hVideo->stInPortDef );
		if ( OMX_ErrorNone == omx_error )
		{
			TRACE( "Input Port Media Type = %s\r\n", hVideo->stInPortDef.format.video.cMIMEType );
		}
		else
		{
			TRACE( "Get InputPort Parameter Error! Error Index = %x\r\n", omx_error );
			goto quit;
		}

		// Set component input port info
		hVideo->stInPortDef.format.video.nFrameWidth  = pImgBuf[0].iWidth;
		hVideo->stInPortDef.format.video.nFrameHeight = pImgBuf[0].iHeight;
		omx_error = OMX_SetParameter( hVideo->hOmxEncoder, OMX_IndexParamPortDefinition, &hVideo->stInPortDef );
		if ( OMX_ErrorNone != omx_error )
		{
			TRACE( "Set InputPort Parameter Error! Error Index = %x\r\n", omx_error );
			goto quit;
		}

		// Get component input port info
		omx_error = OMX_GetParameter( hVideo->hOmxEncoder, OMX_IndexParamPortDefinition, &hVideo->stInPortDef );
		if ( OMX_ErrorNone == omx_error )
		{
			TRACE( "Input Port Media Type = %s\r\n", hVideo->stInPortDef.format.video.cMIMEType );
		}
		else
		{
			TRACE( "Get InputPort Parameter Error! Error Index = %x\r\n", omx_error );
			goto quit;
		}

		TRACE( "Minimum required buffer num by encoder: %u, required buffer size: %u\n", hVideo->stInPortDef.nBufferCountMin, hVideo->stInPortDef.nBufferSize );

		if ( iImgBufCnt < hVideo->stInPortDef.nBufferCountMin )
		{
			TRACE( "Too few video buffers for open max encoder!\n" );
			goto quit;
		}


		// Get component output port info
		hVideo->stOutPortDef.nSize                = sizeof( OMX_PARAM_PORTDEFINITIONTYPE );
		hVideo->stOutPortDef.nVersion.nVersion    = hVideo->unOmxCompVersion.nVersion;
		hVideo->stOutPortDef.nPortIndex           = OUTPUTPORT;
		omx_error = OMX_GetParameter( hVideo->hOmxEncoder, OMX_IndexParamPortDefinition, &hVideo->stOutPortDef );
		if ( OMX_ErrorNone == omx_error )
		{
			if ( OMX_PortDomainVideo == hVideo->stOutPortDef.eDomain )
			{
				TRACE( "default output frame width:       %d\n", (int)hVideo->stOutPortDef.format.video.nFrameWidth );
				TRACE( "default output frame height:      %d\n", (int)hVideo->stOutPortDef.format.video.nFrameHeight );
				TRACE( "hVideo->stOutPortDef.nBufferSize: %d\n", hVideo->stOutPortDef.nBufferSize );
			}
		}
		else
		{
			TRACE( "Get OutputPort Parameter Error! Error Index = %x\r\n", omx_error );
			goto quit;
		}

		hVideo->iCompBufCnt  = hVideo->stOutPortDef.nBufferCountMin;
		hVideo->iCompBufSize = hVideo->stOutPortDef.nBufferSize;

		// Create events
		IPP_EventCreate( &(hVideo->hEvent) );

		omx_error = IppOMXComp_Loaded2Idle( hVideo );
		if ( OMX_ErrorNone != omx_error )
		{
			TRACE( "Loaded --> Idle state transition failed!\n" );
			goto quit;
		}

		omx_error = IppOMXComp_Idle2Exe( hVideo );
		if ( OMX_ErrorNone != omx_error )
		{
			TRACE( "Idle --> Executing state transition failed!\n" );
			goto quit;
		}
	}

	return hVideo;

quit:
	if ( hVideo != NULL )
	{
		if ( hVideo->hFile != NULL );
		{
			fclose( hVideo->hFile );
		}

		if ( hVideo->hOmxEncoder != NULL )
		{
			OMX_FreeHandle( hVideo->hOmxEncoder );
		}

		if ( hVideo->hEvent != NULL )
		{
			IPP_EventDestroy( hVideo->hEvent );
		}

		free( hVideo );
		hVideo = NULL;
	}

	OMX_Deinit();
#endif // _ENABLE_VIDEO_ENCODER_

	return hVideo;

}

void VideoEncoder_Deinit( void *hVideoEncoder )
{
	VideoEncoderChain *hVideo = (VideoEncoderChain*)hVideoEncoder;

	if ( hVideo == NULL )
	{
		return;
	}
    
	// YUV
	if ( hVideo->iEncoderType == 0 )
	{ 
		fclose( hVideo->hFile );
	}

#ifdef _ENABLE_VIDEO_ENCODER_
	else
	{
		OMX_ERRORTYPE omx_error = IppOMXComp_Exe2Idle( hVideo );

		if ( OMX_ErrorNone != omx_error )
		{
			TRACE( "Executing --> Idle state transition failed!\n" );
			goto quit;
		}

		omx_error = IppOMXComp_Idle2Loaded( hVideo );
		if ( OMX_ErrorNone != omx_error )
		{
			TRACE( "Idle --> Loaded state transition failed!\n" );
			goto quit;
		}

quit:
		omx_error = OMX_FreeHandle( hVideo->hOmxEncoder );
		ASSERT( omx_error == OMX_ErrorNone );

		IPP_EventDestroy( hVideo->hEvent );

		fclose( hVideo->hFile );

		OMX_Deinit();
	}

	free( hVideo );
	hVideo = NULL;
#endif // _ENABLE_VIDEO_ENCODER_

	return;
}

void VideoEncoder_SendEOS( void *hVideoEncoder )
{
#ifdef _ENABLE_VIDEO_ENCODER_
	if ( g_iEOSStatus == 0 )
	{
		g_iEOSStatus = 1;
	}
#endif
	return;
}

CAM_Bool VideoEncoder_EOSReceived( void *hVideoEncoder )
{
#ifdef _ENABLE_VIDEO_ENCODER_
	return ( g_iEOSStatus >= 2 );
#else
	return CAM_TRUE;
#endif
}

void VideoEncoder_DeliverInput( void *hVideoEncoder, CAM_ImageBuffer *pImgBuf )
{
	VideoEncoderChain *hVideo = (VideoEncoderChain*)hVideoEncoder;
	CAM_Error         error  = CAM_ERROR_NONE;

    if ( hVideo->iEncoderType == 0 ) // yuv
	{
		if ( hVideo->hFile != NULL )
		{
			fwrite( pImgBuf->pBuffer[0], pImgBuf->iFilledLen[0], 1, hVideo->hFile );
			fwrite( pImgBuf->pBuffer[1], pImgBuf->iFilledLen[1], 1, hVideo->hFile );
			fwrite( pImgBuf->pBuffer[2], pImgBuf->iFilledLen[2], 1, hVideo->hFile );
		}

		error = CAM_SendCommand( hVideo->hCameraEngine, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)CAM_PORT_VIDEO, (CAM_Param)pImgBuf );
		ASSERT_CAM_ERROR( error );
	}
#ifdef _ENABLE_VIDEO_ENCODER_
	else
	{
		int                  i;
		OMX_ERRORTYPE        omx_err;
		OMX_BUFFERHEADERTYPE *pBuffer = NULL;
		
		if ( g_iEOSStatus == 0 || g_iEOSStatus == 1 )
		{
			for ( i = 0; i < sizeof( g_pInBufHeader ) / sizeof( g_pInBufHeader[0] ); i++ )
			{
				if ( g_pInBufHeader[i]->pAppPrivate == pImgBuf )
				{
					pBuffer = g_pInBufHeader[i];
					break;
				}
			}

			ASSERT( pBuffer != NULL );

			if ( g_iEOSStatus == 0 )
			{
				pBuffer->nFilledLen = pImgBuf->iFilledLen[0] + pImgBuf->iFilledLen[1] + pImgBuf->iFilledLen[2];
				omx_err = OMX_EmptyThisBuffer( hVideo->hOmxEncoder, pBuffer );
				ASSERT( omx_err == OMX_ErrorNone );
#ifdef TRACK_BUFFER_CNT
				TRACE( "Encoder buffer cnt: %d, %s\n", ++g_iEmptyBufferCnt, __FUNCTION__ );
#endif
			}
			else// if ( g_iEOSStatus == 1 )
			{
				pBuffer->nFlags |= OMX_BUFFERFLAG_EOS;
				pBuffer->nFilledLen = pImgBuf->iFilledLen[0] + pImgBuf->iFilledLen[1] + pImgBuf->iFilledLen[2];
				omx_err = OMX_EmptyThisBuffer( hVideo->hOmxEncoder, pBuffer );
				ASSERT( omx_err == OMX_ErrorNone );
				g_iEOSStatus = 2;
#ifdef TRACK_BUFFER_CNT
				TRACE( "Encoder buffer cnt: %d, %s\n", ++g_iEmptyBufferCnt, __FUNCTION__ );
#endif
				TRACE( "The EOS flag is sent to OMX!\n" );
			}
		}
		else
		{
			error = CAM_SendCommand( hVideo->hCameraEngine, CAM_CMD_PORT_ENQUEUE_BUF, (CAM_Param)CAM_PORT_VIDEO, (CAM_Param)pImgBuf );
			ASSERT_CAM_ERROR( error );
			TRACE( "One input image is sent to encoder after eos and is ignored!\n" );
		}
	}
#endif // _ENABLE_VIDEO_ENCODER_
	return;
}
