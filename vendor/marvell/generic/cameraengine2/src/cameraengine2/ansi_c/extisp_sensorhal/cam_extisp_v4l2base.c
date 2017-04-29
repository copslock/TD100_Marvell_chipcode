/*******************************************************************************
//(C) Copyright [2010 - 2011] Marvell International Ltd.
//All Rights Reserved
*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "cam_extisp_v4l2base.h"

#include "misc.h"
#include "CamOSALBmm.h"

#include "cam_utility.h"
#include "cam_log.h"


CAM_Int32s dbg_ioctl( CAM_Int32s device, CAM_Int32s cmd, void *data,
                      const char *str_device, const char *str_cmd, const char *str_data,
                      const char *file, CAM_Int32s line )
{
	CAM_Int32s ret = 0;
	CAM_Tick   t1  = 0;

#if defined( BUILD_OPTION_DEBUG_DUMP_V4L2_CALLING )
	CELOG( "%s - %d:\n ioctl(%s, %s, %s)\n", file, line, str_device, str_cmd, str_data );
	CELOG( "%s = %d\n", str_device, device );
	CELOG( "%s = 0x%x\n", str_data, (CAM_Int32u)data );

	switch ( cmd )
	{
	case VIDIOC_S_INPUT:
		CELOG( "*(CAM_Int32s*)(%s) = %d\n", str_data, *(CAM_Int32s*)data );
		break;

	case VIDIOC_S_PARM:
		CELOG( "*(struct v4l2_streamparm*)(%s)           = {\n", str_data );
		CELOG( "\t.type                                  = %d,\n", ((struct v4l2_streamparm*)data)->type );
//		CELOG( "\t.parm.capture.capability               = %d,\n", ((struct v4l2_streamparm*)data)->parm.capture.capability );
		CELOG( "\t.parm.capture.capturemode              = %d,\n", ((struct v4l2_streamparm*)data)->parm.capture.capturemode );
		CELOG( "\t.parm.capture.timeperframe.numerator   = %d,\n", ((struct v4l2_streamparm*)data)->parm.capture.timeperframe.numerator );
		CELOG( "\t.parm.capture.timeperframe.denominator = %d,\n", ((struct v4l2_streamparm*)data)->parm.capture.timeperframe.denominator );
//		CELOG( "\t.parm.capture.extendedmode             = %d,\n", ((struct v4l2_streamparm*)data)->parm.capture.extendedmode );
//		CELOG( "\t.parm.capture.readbuffers              = %d,\n", ((struct v4l2_streamparm*)data)->parm.capture.readbuffers );
		CELOG( "}\n" );
		break;

	case VIDIOC_S_FMT:
		CELOG( "*(struct v4l2_format*)(%s) = {\n", str_data );
		CELOG( "\t.type                    = %d,\n", ((struct v4l2_format*)data)->type );
		CELOG( "\t.fmt.pix.pixelformat     = 0x%x,\n", ((struct v4l2_format*)data)->fmt.pix.pixelformat );
		CELOG( "\t.fmt.pix.width           = %d,\n", ((struct v4l2_format*)data)->fmt.pix.width );
		CELOG( "\t.fmt.pix.height          = %d,\n", ((struct v4l2_format*)data)->fmt.pix.height );
		CELOG( "}\n" );
		break;

	case VIDIOC_REQBUFS:
		CELOG( "*(struct v4l2_requestbuffers*)(%s) = {\n", str_data );
		CELOG( "\t.type   = %d,\n", ((struct v4l2_requestbuffers*)data)->type );
		CELOG( "\t.count  = %d,\n", ((struct v4l2_requestbuffers*)data)->count );
		CELOG( "\t.memory = %d,\n", ((struct v4l2_requestbuffers*)data)->memory );
		CELOG( "}\n" );
		break;

	case VIDIOC_STREAMON:
		CELOG( "*(CAM_Int32s*)(%s) = %d\n", str_data, *(CAM_Int32s*)data );
		break;

	case VIDIOC_STREAMOFF:
		CELOG( "*(CAM_Int32s*)(%s) = %d\n", str_data, *(CAM_Int32s*)data );
		break;

	case VIDIOC_QBUF:
		CELOG( "*(struct v4l2_buffer*)(%s) = {\n", str_data );
		CELOG( "\t.type      = %d,\n", ((struct v4l2_buffer*)data)->type );
		CELOG( "\t.index     = %d,\n", ((struct v4l2_buffer*)data)->index );
		CELOG( "\t.bytesused = %d,\n", ((struct v4l2_buffer*)data)->bytesused );
		CELOG( "\t.flags     = %d,\n", ((struct v4l2_buffer*)data)->flags );
		CELOG( "\t.field     = %d,\n", ((struct v4l2_buffer*)data)->field );
		CELOG( "}\n" );
		break;

	case VIDIOC_DQBUF:
		CELOG( "Before Dequeue:\n" );
		CELOG( "*(struct v4l2_buffer*)(%s) = {\n", str_data );
		CELOG( "\t.type      = %d,\n", ((struct v4l2_buffer*)data)->type );
		CELOG( "\t.index     = %d,\n", ((struct v4l2_buffer*)data)->index );
		CELOG( "\t.bytesused = %d,\n", ((struct v4l2_buffer*)data)->bytesused );
		CELOG( "\t.flags     = %d,\n", ((struct v4l2_buffer*)data)->flags );
		CELOG( "\t.field     = %d,\n", ((struct v4l2_buffer*)data)->field );
		break;

	default:
		break;
	}
	t1 = -IPP_TimeGetTickCount();

#endif

	ret = ioctl( device, cmd, data );

#if defined( BUILD_OPTION_DEBUG_DUMP_V4L2_CALLING )
	t1 += IPP_TimeGetTickCount();

	CELOG( "Return value: %d\n", ret );	
	switch ( cmd )
	{
	case VIDIOC_DQBUF:
		CELOG( "After Dequeue:\n" );
		CELOG( "*(struct v4l2_buffer*)(%s) = {\n", str_data );
		CELOG( "\t.type      = %d,\n", ((struct v4l2_buffer*)data)->type );
		CELOG( "\t.index     = %d,\n", ((struct v4l2_buffer*)data)->index );
		CELOG( "\t.bytesused = %d,\n", ((struct v4l2_buffer*)data)->bytesused );
		CELOG( "\t.flags     = %d,\n", ((struct v4l2_buffer*)data)->flags );
		CELOG( "\t.field     = %d,\n", ((struct v4l2_buffer*)data)->field );
		CELOG( "}\n" );
		break;

	default:
		break;
	}
	CELOG( "Perf: ioctl elapse: %.2f ms\n", (double)t1 / 1000.0f );
	CELOG( "\n\n" );

#endif
	
	return ret;
}

CAM_Int32s dbg_open( const char *dev, CAM_Int32s flag, const char *str_dev, const char *str_flag, const char *file, CAM_Int32s line )
{
	CAM_Int32s ret = 0;
   	CAM_Tick   t1  = 0;

#if defined( BUILD_OPTION_DEBUG_DUMP_V4L2_CALLING )
	CELOG( "%s - %d:\n open(\"%s\", %s)\n", file, line, str_dev, str_flag );
	t1 = -IPP_TimeGetTickCount();
#endif

	ret = open( dev, flag );

#if defined( BUILD_OPTION_DEBUG_DUMP_V4L2_CALLING )
	t1 += IPP_TimeGetTickCount();
	CELOG( "Return value: %d\n", ret );
	CELOG( "Perf: open elapse: %.2f ms\n", t1 / 1000.0 );
	CELOG( "\n\n" );
#endif

	return ret;
}

CAM_Int32s dbg_close( CAM_Int32s dev, const char *str_dev, const char *file, CAM_Int32s line )
{
	CAM_Int32s ret = 0;
	CAM_Tick   t1  = 0;

#if defined( BUILD_OPTION_DEBUG_DUMP_V4L2_CALLING )
	CELOG( "%s - %d:\n close(%s)\n", file, line, str_dev );
	t1 = -IPP_TimeGetTickCount();
#endif

	ret = close( dev );

#if defined( BUILD_OPTION_DEBUG_DUMP_V4L2_CALLING )
	t1 += IPP_TimeGetTickCount();
	CELOG( "Return value: %d\n", ret );
	CELOG( "Elapse: %.2f ms\n", t1 / 1000.0 );
	CELOG( "\n\n" );
#endif

	return ret;
}

int dbg_poll( struct pollfd *ufds, unsigned int nfds, int timeout,
              const char *str_ufds, const char *str_nfds, const char *str_timeout,
              const char *file, CAM_Int32s line )
{
	CAM_Int32s ret = 0;
	CAM_Tick   t1  = 0;

#if defined( BUILD_OPTION_DEBUG_DUMP_V4L2_CALLING )
	CELOG( "%s - %d:\n poll(%s, %s, %s) \n", file, line, str_ufds, str_nfds, str_timeout );
	t1 = -IPP_TimeGetTickCount();
#endif

	ret = poll( ufds, nfds, timeout );

#if defined( BUILD_OPTION_DEBUG_DUMP_V4L2_CALLING )
	t1 += IPP_TimeGetTickCount();
	CELOG( "Return value: %d\n", ret );
	CELOG( "Elapse: %.2f ms\n", t1 / 1000.0 );
	CELOG( "\n\n" );
#endif
	
	return ret;
}

#define CAM_V4L2_DEVICE_NUM   2
#define CAM_V4L2_CHANNEL_NUM  2

CAM_Error V4L2_SelfDetect( _SmartSensorAttri *pSensorInfo, CAM_Int8s *sSensorName, _SmartSensorFunc *pCallFunc,
                           CAM_Size stVideoResTable[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT], CAM_Size stStillResTable[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT],
                           CAM_ImageFormat eVideoFormatTable[CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT], CAM_ImageFormat eStillFormatTable[CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT] )
{
	CAM_Int32s              i = 0, j = 0, k= 0;
	CAM_Int32s              iSensorIndex = 0;
	struct v4l2_capability  stV4L2DriverCap;
	struct v4l2_fmtdesc     stV4L2Format;
	struct v4l2_frmsizeenum stV4L2FrmSize;
	CAM_Int32s              ret1 = -1, ret2 = -1, ret = -1;
	CAM_Int32s              cnt = 0, iVideoFmtCnt = 0, iStillFmtCnt = 0;
	static CAM_Int8u        sDevicePool[CAM_V4L2_DEVICE_NUM][12] = { "/dev/video0", "/dev/video1" };
	static CAM_Int32s       bDeviceOccupy[CAM_V4L2_DEVICE_NUM][CAM_V4L2_CHANNEL_NUM] = { {0, 0}, {0, 0}, }; // 0: idle; 1: invalid; 2: occupied

	CAM_Size                stVideoSizeTab[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT]   = { {0, 0}, };
	CAM_Int32s              iVideoSizeCntTab[CAM_MAX_SUPPORT_IMAGE_SIZE_CNT] = { 0, };

	for ( i = 0; i < CAM_V4L2_DEVICE_NUM; i++ )
	{
		CAM_Int32s fd = -1;

		// check if current channel occupied
		if ( bDeviceOccupy[i][0] != 0 && bDeviceOccupy[i][1] != 0 )
		{
			TRACE( CAM_INFO, "Info: device[%s] has been occupied or invalid( %s, %d )\n", sDevicePool[i], __FILE__, __LINE__ );
			continue;
		}

		fd = cam_open( sDevicePool[i], O_RDONLY );
		if ( fd < 0 )
		{
			TRACE( CAM_INFO, "Info: device node %s open failed, take as invalid device ( %s, %d )\n", sDevicePool[i], __FILE__, __LINE__ );
			bDeviceOccupy[i][0] = bDeviceOccupy[i][1] = 1;
			continue;
		}
		for ( j = 0; j < CAM_V4L2_CHANNEL_NUM; j++ )
		{
			CAM_Int32s       channel       = -1;
			_V4L2SensorEntry *pSensorEntry = NULL;

			// check if current channel occupied
			if ( bDeviceOccupy[i][j] != 0 )
			{
				TRACE( CAM_INFO, "Info: channel[%d] of device node %s has been occupied or invalid( %s, %d )\n", 
				       j, sDevicePool[i], __FILE__, __LINE__ );
				continue;
			}

			channel = j;
			if( 0 != cam_ioctl( fd, VIDIOC_S_INPUT, &channel ) )
			{
				TRACE( CAM_INFO, "Info: channel[%d] of device node %s is invalid( %s, %d )\n", channel, sDevicePool[i],  __FILE__, __LINE__ );
				bDeviceOccupy[i][j] = 1;
				continue;
			}

			// send "who are you" to sensor driver 
			ret = cam_ioctl( fd, VIDIOC_QUERYCAP, &stV4L2DriverCap );
			if ( !(stV4L2DriverCap.capabilities | V4L2_CAP_VIDEO_CAPTURE) )
			{
				TRACE( CAM_INFO, "Info: channel[%d] of device node %s is not camera( %s, %d )\n", channel, sDevicePool[i], __FILE__, __LINE__ );
				bDeviceOccupy[i][j] = 1;
				continue;
			}

			if ( strcmp( sSensorName, "basicsensor" ) )
			{
				if ( ret < 0 || strcmp( (const char*)stV4L2DriverCap.driver, (const char*)sSensorName ) )
				{
					TRACE( CAM_INFO, "Info: cannot detect[%s], actual name:%s( %s, %d )\n", sSensorName, stV4L2DriverCap.driver, __FILE__, __LINE__ );
					continue;
				}
				else
				{
					strcpy( pSensorInfo[iSensorIndex].sSensorName, (const char*)stV4L2DriverCap.driver );
				}
			}
			else
			{
				strcpy( pSensorInfo[iSensorIndex].sSensorName, "BasicSensor-" );
				if ( ret < 0 || (stV4L2DriverCap.driver)[0] == '\0' )
				{
					strcat( pSensorInfo[iSensorIndex].sSensorName, "unknown" );
				}
				else
				{
					strncat( pSensorInfo[iSensorIndex].sSensorName, (const char*)stV4L2DriverCap.driver, 15 );
				}
			}

			pSensorInfo[iSensorIndex].pFunc = pCallFunc;

			pSensorEntry = (_V4L2SensorEntry*)(pSensorInfo[iSensorIndex].cReserved);

			strcpy( pSensorEntry->sDeviceName, sDevicePool[i] );
			pSensorEntry->iV4L2SensorID     = channel;
			bDeviceOccupy[i][channel]       = 2;

			iSensorIndex++;

			TRACE( CAM_INFO, "Info: find sensor[%s] in device[%s] channel[%d]\n", stV4L2DriverCap.driver, sDevicePool[i], channel );

			// now, we can get the capability of current sensor
			stV4L2Format.index = 0;
			stV4L2Format.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			do
			{
				CAM_Size *stCurrentSizeTable = NULL;

				ret1 = cam_ioctl( fd, VIDIOC_ENUM_FMT, &stV4L2Format );

				if( ret1 < 0 )
				{
					TRACE( CAM_INFO, "Info: max format num: %d \n", stV4L2Format.index );
					break;
				}

				switch ( stV4L2Format.pixelformat )
				{
				case V4L2_PIX_FMT_YUV420:
					eVideoFormatTable[iVideoFmtCnt] = CAM_IMGFMT_YCbCr420P;
					stCurrentSizeTable              = stVideoSizeTab;
					stV4L2FrmSize.pixel_format      = stV4L2Format.pixelformat;
					iVideoFmtCnt++;
					break;
				case V4L2_PIX_FMT_YUV422P:
					eVideoFormatTable[iVideoFmtCnt] = CAM_IMGFMT_YCbCr422P;
					stCurrentSizeTable              = stVideoSizeTab;
					stV4L2FrmSize.pixel_format      = stV4L2Format.pixelformat;
					iVideoFmtCnt++;
					break;
				case V4L2_PIX_FMT_UYVY:
					eVideoFormatTable[iVideoFmtCnt] = CAM_IMGFMT_CbYCrY;
					stCurrentSizeTable              = stVideoSizeTab;
					stV4L2FrmSize.pixel_format      = stV4L2Format.pixelformat;
					iVideoFmtCnt++;	
					break;
				case V4L2_PIX_FMT_YUYV:
					eVideoFormatTable[iVideoFmtCnt] = CAM_IMGFMT_YCbYCr;
					stCurrentSizeTable              = stVideoSizeTab;
					stV4L2FrmSize.pixel_format      = stV4L2Format.pixelformat;
					iVideoFmtCnt++;
					break;
				case V4L2_PIX_FMT_RGB565:
					eVideoFormatTable[iVideoFmtCnt] = CAM_IMGFMT_RGB565;
					stCurrentSizeTable              = stVideoSizeTab;
					stV4L2FrmSize.pixel_format      = stV4L2Format.pixelformat;
					iVideoFmtCnt++;
					break;
				case V4L2_PIX_FMT_JPEG:
#if 1
					eStillFormatTable[iStillFmtCnt] = CAM_IMGFMT_JPEG;
					stCurrentSizeTable              = stStillResTable;
					stV4L2FrmSize.pixel_format      = stV4L2Format.pixelformat;
					iStillFmtCnt++;
#else
					stV4L2FrmSize.pixel_format      = 0x20202020; // all white spaces
#endif
					break;
				default:
					TRACE( CAM_ERROR, "Error: unrecognized V4L2 pixel format[0x%x]( %s, %d )\n", stV4L2Format.pixelformat, __FILE__, __LINE__ );
					stV4L2FrmSize.pixel_format      = 0x20202020;
					break;
				}

				if ( stV4L2FrmSize.pixel_format == 0x20202020 )
				{
					stV4L2Format.index++;
					continue;
				}

				ret2                = 0;
				stV4L2FrmSize.index = 0;
				// here we adopt the minimun common set of the non-compression format
				cnt = 0;
				do
				{
					ret2 = cam_ioctl( fd, VIDIOC_ENUM_FRAMESIZES, &stV4L2FrmSize );
					if( 0 != ret2 )
					{
						char *pFormatName = (char*)(&stV4L2FrmSize.pixel_format);
						TRACE( CAM_INFO, "Info: max frame size num: %d for format: %c%c%c%c \n",
						       stV4L2FrmSize.index, *pFormatName, *(pFormatName+1), *(pFormatName+2), *(pFormatName+3) );
						break;
					}

					// add supported resolutions to resolution table
					ASSERT( stV4L2FrmSize.type == V4L2_FRMSIZE_TYPE_DISCRETE && stV4L2FrmSize.discrete.width != 0 && stV4L2FrmSize.discrete.height != 0 );
					if ( stV4L2Format.pixelformat == V4L2_PIX_FMT_JPEG )
					{
						stCurrentSizeTable[cnt].iWidth  = stV4L2FrmSize.discrete.width;
						stCurrentSizeTable[cnt].iHeight = stV4L2FrmSize.discrete.height;
						cnt++;
					}
					else
					{
						for ( k = 0; k < CAM_MAX_SUPPORT_IMAGE_SIZE_CNT; k++ )
						{

							if( stCurrentSizeTable[k].iWidth == stV4L2FrmSize.discrete.width && 
								stCurrentSizeTable[k].iHeight == stV4L2FrmSize.discrete.height )
							{
								iVideoSizeCntTab[k]++;
								break;
							}
							else
							{
								if ( stCurrentSizeTable[k].iWidth != 0 && stCurrentSizeTable[k].iHeight != 0 )
								{
									continue;
								}
								else
								{
									stCurrentSizeTable[k].iWidth  = stV4L2FrmSize.discrete.width;
									stCurrentSizeTable[k].iHeight = stV4L2FrmSize.discrete.height;
									iVideoSizeCntTab[k]++;
									break;
								}
							}
						}
					}

					stV4L2FrmSize.index++;
				} while ( stV4L2FrmSize.index < CAM_MAX_SUPPORT_IMAGE_SIZE_CNT );

				if ( stV4L2FrmSize.index == 0 )
				{
					TRACE( CAM_WARN, "Warning: BAD practice to make a supported format's resolution table empty( %s, %d )\n", 
					       __FILE__, __LINE__ );
					if ( stV4L2Format.pixelformat == V4L2_PIX_FMT_JPEG )
					{
						eStillFormatTable[--iStillFmtCnt] = 0;
					}
					else
					{
						eVideoFormatTable[--iVideoFmtCnt] = 0;
					}

				}

				stV4L2Format.index++;		
			} while ( 0 == ret1 && iStillFmtCnt < CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT && iVideoFmtCnt < CAM_MAX_SUPPORT_IMAGE_FORMAT_CNT );

			ASSERT( iVideoFmtCnt != 0 );

			cnt = 0;
			for ( k = 0; k < CAM_MAX_SUPPORT_IMAGE_SIZE_CNT; k++ )
			{
				if ( iVideoSizeCntTab[k] == iVideoFmtCnt )
				{
					stVideoResTable[cnt].iWidth  = stVideoSizeTab[k].iWidth;
					stVideoResTable[cnt].iHeight = stVideoSizeTab[k].iHeight;
					cnt++;
				}
			}

			// if JPEG format is not supported by sensor/driver, we will support raw format in still port
			if ( iStillFmtCnt == 0 )
			{
				// copy video formats to still port
				CAM_Int32s m = 0;
				for ( m = 0; m < iVideoFmtCnt; m++ )
				{
					eStillFormatTable[m] = eVideoFormatTable[m];
				}

				m = 0;
				while ( stVideoResTable[m].iWidth != 0 && stVideoResTable[m].iHeight != 0 )
				{
					stStillResTable[m].iWidth  = stVideoResTable[m].iWidth;
					stStillResTable[m].iHeight = stVideoResTable[m].iHeight;
					m++;
				}
			}
		}

		ret = cam_close( fd );
		ASSERT( ret == 0 );
	}

	return CAM_ERROR_NONE;
}

CAM_Error V4L2_Init( _V4L2SensorState *pSensorState, _V4L2SensorAttribute *pSensorAttri, CAM_Int32s iUnstableFrameCnt )
{
	CAM_Int32s i = 0, ret = 0;
	
	// BAC check
	_CHECK_BAD_POINTER( pSensorState );

	// initialize _V4L2SensorState
	memset( pSensorState, -1, sizeof( _V4L2SensorState ) );

	// open indicated image sensor
	pSensorState->iSensorFD = cam_open( pSensorAttri->stV4L2SensorEntry.sDeviceName, O_RDONLY );
	if ( pSensorState->iSensorFD < 0 )
	{
		TRACE( CAM_ERROR, "Error: V4L2 failed to open device %s ( %s, %s, %d )\n", 
		       pSensorAttri->stV4L2SensorEntry.sDeviceName, __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_DRIVEROPFAILED;
	}

	ret = cam_ioctl( pSensorState->iSensorFD, VIDIOC_S_INPUT, &(pSensorAttri->stV4L2SensorEntry.iV4L2SensorID) );
	if( 0 != ret )
	{
		TRACE( CAM_ERROR, "Error: V4L2 set input (sensor id: %d) failed ( %s, %s, %d )\n", 
		       pSensorAttri->stV4L2SensorEntry.iV4L2SensorID, __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_DRIVEROPFAILED;
	}

	// sensor state is idle
	pSensorState->stConfig.eState = CAM_CAPTURESTATE_IDLE;
	pSensorState->bIsFlashOn      = CAM_FALSE;
	pSensorState->bIsStreamOn     = CAM_FALSE;
	pSensorState->bIsDriverOK     = CAM_TRUE;

	// init event handler
	pSensorState->fnEventHandler  = NULL;
	pSensorState->pUserData       = NULL;

	// init V4L2 buffer
	for ( i = 0; i < CAM_MAX_PORT_BUF_CNT; i++ )
	{
		(pSensorState->stV4L2Buf)[i].iFlag = 0;
	}
	pSensorState->iV4L2BufCnt      = 0;
	pSensorState->iV4L2QueueBufCnt = 0;

	// set callbacks and parameters for sensor resolution switch
	memmove( &(pSensorState->stAttribute), pSensorAttri, sizeof( _V4L2SensorAttribute ) );

	// set unstable frames number, we will drop these frames
	pSensorState->iUnstableFrameCnt = iUnstableFrameCnt;
	pSensorState->iCurrentSkipCnt   = iUnstableFrameCnt;

	return CAM_ERROR_NONE;
}

CAM_Error V4L2_Deinit( _V4L2SensorState *pSensorState )
{
	CAM_Int32s ret = 0;

	// BAC check
	_CHECK_BAD_POINTER( pSensorState );

	// close sensor
	if ( pSensorState->iSensorFD >= 0 ) 
	{
		ret = cam_close( pSensorState->iSensorFD );
		ASSERT( ret == 0 );

		pSensorState->iSensorFD = -1;
	}

	return CAM_ERROR_NONE;
}

CAM_Error V4L2_RegisterEventHandler( _V4L2SensorState *pSensorState, CAM_EventHandler fnEventHandler, void *pUserData )
{
	pSensorState->fnEventHandler = fnEventHandler;
	pSensorState->pUserData      = pUserData;

	return CAM_ERROR_NONE;
}

CAM_Error V4L2_Enqueue( _V4L2SensorState *pSensorState, CAM_ImageBuffer *pImgBuf )
{
	CAM_Int32s         ret        = -1;
	CAM_Bool           bNewBuffer = CAM_TRUE;
	_V4L2Buf           *pV4L2Buf  = NULL;

	// BAC check
	_CHECK_BAD_POINTER( pSensorState );
	_CHECK_BAD_POINTER( pImgBuf );

	// check if the buffer has ever been enqueued before by checking the private index and private data
	if ( pImgBuf->iPrivateIndex >= 0 && pImgBuf->iPrivateIndex < pSensorState->iV4L2BufCnt ) 
	{
		pV4L2Buf = &pSensorState->stV4L2Buf[pImgBuf->iPrivateIndex];

		if ( pImgBuf->pPrivateData == pV4L2Buf && pV4L2Buf->pImage == pImgBuf )
		{
			bNewBuffer = CAM_FALSE;
		}
		else
		{
			bNewBuffer = CAM_TRUE;
		}
	}
	else
	{
		bNewBuffer = CAM_TRUE;
	}

	if ( !bNewBuffer )
	{
		if ( pV4L2Buf->iFlag != 0 )
		{
			TRACE( CAM_ERROR, "state[%d] \n", pSensorState->stConfig.eState );
		}
		ASSERT( pV4L2Buf->iFlag == 0 );

		ret = cam_ioctl( pSensorState->iSensorFD, VIDIOC_QBUF, &pV4L2Buf->stBufNode );
		if ( ret == -1 )
		{
			TRACE( CAM_ERROR, "Error: V4L2 enqueue buffer failed( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );

			TRACE( CAM_ERROR, "stBufNode.type      = %d\n", pV4L2Buf->stBufNode.type );
			TRACE( CAM_ERROR, "stBufNode.memory    = %d\n", pV4L2Buf->stBufNode.memory );
			TRACE( CAM_ERROR, "stBufNode.index     = %d\n", pV4L2Buf->stBufNode.index );
			TRACE( CAM_ERROR, "stBufNode.m.userptr = %p\n", (void*)pV4L2Buf->stBufNode.m.userptr );
			TRACE( CAM_ERROR, "stBufNode.length    = %d\n", pV4L2Buf->stBufNode.length );

			return CAM_ERROR_DRIVEROPFAILED;
		}

		pV4L2Buf->iFlag = 1;
		pSensorState->iV4L2QueueBufCnt++;
	}
	else
	{
		struct v4l2_buffer *pBufNode  = NULL;
			
		// now we know that the buffer been enqueued is a new buffer
		if ( pSensorState->iV4L2BufCnt >= CAM_MAX_PORT_BUF_CNT )
		{
			TRACE( CAM_ERROR, "Error: pSensorState->iV4L2BufCnt >= CAM_MAX_PORT_BUF_CNT( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
			return CAM_ERROR_OUTOFRESOURCE;
		}

		// check back to back buffer layout
		if ( ( pImgBuf->eFormat == CAM_IMGFMT_YCbCr422P || pImgBuf->eFormat == CAM_IMGFMT_YCbCr420P ) &&
		     ( pImgBuf->pBuffer[0] + pImgBuf->iOffset[0] + pImgBuf->iAllocLen[0] != pImgBuf->pBuffer[1] + pImgBuf->iOffset[1] ||
		       pImgBuf->pBuffer[1] + pImgBuf->iOffset[1] + pImgBuf->iAllocLen[1] != pImgBuf->pBuffer[2] + pImgBuf->iOffset[2] ) )
		{
			TRACE( CAM_ERROR, "Error: requires \"back to back\" buffer layout when the input is 422/420 plannar( %s, %s, %d )\n",
			       __FILE__, __FUNCTION__, __LINE__ );
			TRACE( CAM_ERROR, "pBuffer = {%p, %p, %p}, iOffset = {%d, %d, %d}, iAllocLen = {%d, %d, %d}\n",\
			       pImgBuf->pBuffer[0], pImgBuf->pBuffer[1], pImgBuf->pBuffer[2],\
			       pImgBuf->iOffset[0], pImgBuf->iOffset[1], pImgBuf->iOffset[2],\
			       pImgBuf->iAllocLen[0], pImgBuf->iAllocLen[1], pImgBuf->iAllocLen[2] );

			return CAM_ERROR_BADBUFFER;
		}

		pBufNode = &(pSensorState->stV4L2Buf[pSensorState->iV4L2BufCnt].stBufNode);
		memset( pBufNode, 0, sizeof( struct v4l2_buffer ) );

		pBufNode->type      = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		pBufNode->memory    = V4L2_MEMORY_USERPTR;
		pBufNode->index     = pSensorState->iV4L2BufCnt;
		pBufNode->m.userptr = (unsigned long)( pImgBuf->pBuffer[0] + pImgBuf->iOffset[0] );
		pBufNode->length    = pImgBuf->iAllocLen[0] + pImgBuf->iAllocLen[1] + pImgBuf->iAllocLen[2];

		ret = cam_ioctl( pSensorState->iSensorFD, VIDIOC_QBUF, pBufNode );
		if ( ret != -1 )
		{
			pImgBuf->iPrivateIndex = pSensorState->iV4L2BufCnt;
			pImgBuf->pPrivateData  = &pSensorState->stV4L2Buf[pImgBuf->iPrivateIndex];
			pSensorState->stV4L2Buf[pImgBuf->iPrivateIndex].pImage = pImgBuf;
			pSensorState->stV4L2Buf[pImgBuf->iPrivateIndex].iFlag  = 1;

			pSensorState->iV4L2BufCnt++;
			pSensorState->iV4L2QueueBufCnt++;
		}
		else
		{
			TRACE( CAM_ERROR, "Error: V4L2 enqueue buffer failed( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );

			TRACE( CAM_ERROR, "pBufNode->type      = %d\n", pBufNode->type );
			TRACE( CAM_ERROR, "pBufNode->memory    = %d\n", pBufNode->memory );
			TRACE( CAM_ERROR, "pBufNode->index     = %d\n", pBufNode->index );
			TRACE( CAM_ERROR, "pBufNode->m.userptr = %p\n", (void*)pBufNode->m.userptr );
			TRACE( CAM_ERROR, "pBufNode->length    = %d\n", pBufNode->length );

			return CAM_ERROR_DRIVEROPFAILED;
		}
	}

#if !defined( BUILD_OPTION_WORKAROUND_V4L2_STREAMON_AFTER_ALLENQUEUE )
	if ( !pSensorState->bIsStreamOn && pSensorState->iV4L2QueueBufCnt >= 1 )
	{
		// stream on
		CAM_Int32s iBufType = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		ret = cam_ioctl( pSensorState->iSensorFD, VIDIOC_STREAMON, &iBufType );
		if ( 0 != ret )
		{
			TRACE( CAM_ERROR, "Error: V4L2 stream on sensor failed( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
			return CAM_ERROR_DRIVEROPFAILED;
		}

		// driver is OK
		pSensorState->bIsDriverOK = CAM_TRUE;

		pSensorState->bIsStreamOn = CAM_TRUE;
	}
#endif

	return CAM_ERROR_NONE;
}

static CAM_Error _V4L2_Dequeue( _V4L2SensorState *pSensorState, CAM_ImageBuffer **ppImgBuf )
{
	CAM_Int32s         ret       = -10;
	CAM_Int32s         cnt       = 0;
	CAM_Error          error     = CAM_ERROR_NONE;
	struct v4l2_buffer stBufNode;
	struct pollfd      ufds;

	// BAC check
	_CHECK_BAD_POINTER( pSensorState );
	_CHECK_BAD_POINTER( ppImgBuf );

	if ( pSensorState->iV4L2QueueBufCnt < pSensorState->stBufReq.iMinBufCount )
	{
		TRACE( CAM_ERROR, "Error: buffer number in driver[%d] is less than what it requires[%d]( %s, %s, %d )\n", 
		       pSensorState->iV4L2QueueBufCnt, pSensorState->stBufReq.iMinBufCount, __FILE__, __FUNCTION__, __LINE__ );

		return CAM_ERROR_NOTENOUGHBUFFERS;
	}

	// if not stream-on yet, stream-on firstly
	if ( !pSensorState->bIsStreamOn && pSensorState->iV4L2QueueBufCnt >= 1 )
	{

		CAM_Int32s iBufType = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		ret = cam_ioctl( pSensorState->iSensorFD, VIDIOC_STREAMON, &iBufType );
		if ( 0 != ret )
		{
			TRACE( CAM_ERROR, "Error: V4L2 stream on sensor failed( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
			return CAM_ERROR_DRIVEROPFAILED;
		}

		// driver is OK
		pSensorState->bIsDriverOK = CAM_TRUE;

		pSensorState->bIsStreamOn = CAM_TRUE;
	}

	if ( !pSensorState->bIsDriverOK )
	{
		TRACE( CAM_ERROR, "Error: Fatal error happened! Please reset to idle state!\n" );

		return CAM_ERROR_FATALERROR;
	}

	memset( &ufds, 0, sizeof( struct pollfd ) );
	ufds.fd     = pSensorState->iSensorFD;
	ufds.events = POLLIN;

	do 
	{
		ret = cam_poll( &ufds, 1, 100 ); // wait 100ms one time
		cnt++;
		if ( cnt >= 4 && ret <= 0 )
		{
			TRACE( CAM_ERROR, "Error: V4L2 polling failed, return value[%d]( %s, %s, %d )\n", ret, __FILE__, __FUNCTION__, __LINE__ );

			pSensorState->bIsDriverOK = CAM_FALSE;
			
			return CAM_ERROR_FATALERROR;
		}
	} while ( ret <= 0 );

	// if poll success, dequeue a buffer from driver
	memset( &stBufNode, 0, sizeof( struct v4l2_buffer ) );
	stBufNode.type     = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	stBufNode.memory   = V4L2_MEMORY_USERPTR;
	stBufNode.index    = pSensorState->iV4L2BufCnt;

	ret = cam_ioctl( pSensorState->iSensorFD, VIDIOC_DQBUF, &stBufNode );
	if ( ret != 0 )
	{
		TRACE( CAM_ERROR, "Error: V4L2 VIDIOC_DQBUF failed, return code[%d]( %s, %s, %d )\n", ret, __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_DRIVEROPFAILED;
	}

	ASSERT( stBufNode.index < pSensorState->iV4L2BufCnt );

	memcpy( &pSensorState->stV4L2Buf[stBufNode.index].stBufNode, &stBufNode, sizeof( struct v4l2_buffer ) );

	*ppImgBuf = pSensorState->stV4L2Buf[stBufNode.index].pImage;

	pSensorState->stV4L2Buf[stBufNode.index].iFlag = 0;

#if 0
	if( CAM_IMGFMT_JPEG == pSensorState->stConfig.eFormat )
	{
		IPP_Sleep( 200000 );
	}
#endif

	(*ppImgBuf)->iTick = IPP_TimeGetTickCount();

	pSensorState->iV4L2QueueBufCnt--;

	return CAM_ERROR_NONE;
}


CAM_Error V4L2_DequeueWithSkip( _V4L2SensorState *pSensorState, CAM_ImageBuffer **ppImgBuf )
{
	CAM_Error error = CAM_ERROR_NONE;

#if defined( CAM_PERF )
	CAM_Tick tick, total = 0;
#endif

	// skip first N frames to avoid bad quality
	for ( ; ; )
	{
#if defined( CAM_PERF )
		tick = -IPP_TimeGetTickCount();
#endif
		error = _V4L2_Dequeue( pSensorState, ppImgBuf );
		if ( error != CAM_ERROR_NONE )
		{
			return error;
		}

		if ( pSensorState->iCurrentSkipCnt <= 0 )
		{
#if defined( CAM_PERF )
			// CELOG( "Perf: frame skip latency = %llu\n", total );
#endif
			if ( CAM_IMGFMT_JPEG == pSensorState->stConfig.eFormat )
			{
				//do bad SOI check for jpeg
				if ( (*ppImgBuf)->pBuffer[0][0] == 0xFF && (*ppImgBuf)->pBuffer[0][1] == 0xD8 )
				{
					break;
				}
				else
				{
					CELOG( "bad jpeg SOI, skip one more frame in %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
					pSensorState->iCurrentSkipCnt ++;
				}
			}
			else
			{
				break;
			}
		}
#if defined( CAM_PERF )
//		else
//		{
//			tick += IPP_TimeGetTickCount();
//			CELOG( "Perf: skip latency = %llu\n", tick );
//			total += tick;
//		}
#endif

		if ( pSensorState->iCurrentSkipCnt == pSensorState->iUnstableFrameCnt )
		{
			TRACE( CAM_INFO, "Info: Skip %d frames due to sensor stream off/on\n", pSensorState->iUnstableFrameCnt );
		}

		pSensorState->iCurrentSkipCnt--;

		error = V4L2_Enqueue( pSensorState, *ppImgBuf );
		if ( error != CAM_ERROR_NONE )
		{
			return error;
		}
	}

	return error;
}

CAM_Error V4L2_Dequeue( _V4L2SensorState *pSensorState, CAM_ImageBuffer **ppImgBuf )
{
	CAM_Error error = CAM_ERROR_NONE;

#if defined( BUILD_OPTION_WORKAROUND_V4L2_FRAMERATECONTROL_BY_DROPFRAME )
	// frame rate control
	if ( pSensorState->stConfig.eState == CAM_CAPTURESTATE_PREVIEW ||
	     pSensorState->stConfig.eState == CAM_CAPTURESTATE_VIDEO )
	{
		for ( ; ; )
		{
			CAM_Tick sensor_n_frame_interval, target_n_frame_interval, target_n_minus_1_frame_interval;
			CAM_Tick tick;

			error = V4L2_DequeueWithSkip( pSensorState, ppImgBuf );
			if ( error != CAM_ERROR_NONE )
			{
				return error;
			}

			tick = IPP_TimeGetTickCount();

			if ( pSensorState->iValidFrameIndex > 0 )
			{
				sensor_n_frame_interval = tick - pSensorState->tFramesTickStart;

				target_n_frame_interval = (CAM_Tick)( 65536.0 * 1000000.0 / pSensorState->stConfig.iTargetFpsQ16 * pSensorState->iValidFrameIndex );

				target_n_minus_1_frame_interval = (CAM_Tick)( 65536.0 * 1000000.0 / pSensorState->stConfig.iTargetFpsQ16 * ( pSensorState->iValidFrameIndex - 1 ) );

#if 0
				CELOG( "Info: sensor_n_frame_interval: %.2f ms, target_n_frame_interval: %.2f ms, target_n_minus_1_frame_interval: %.2f ms, target fps: %.2f\n",
				        sensor_n_frame_interval / 1000.0, target_n_frame_interval /1000.0, target_n_minus_1_frame_interval / 1000.0, pSensorState->stShotParamSetting.iFpsQ16 / 65536.0 );
#endif

				if ( sensor_n_frame_interval <= target_n_frame_interval && sensor_n_frame_interval > target_n_minus_1_frame_interval )
				{
					if ( pSensorState->iValidFrameIndex % 200 == 0 )
					{
						// update the actual frame rate
						pSensorState->stConfig.iActualFpsQ16 = (CAM_Int32s)( 1000000.0 * 65536.0 / sensor_n_frame_interval * pSensorState->iValidFrameIndex );
					}

					pSensorState->iValidFrameIndex++;
				}
				else if ( sensor_n_frame_interval > target_n_frame_interval )
				{
					// TRACE( CAM_INFO, "Info: sensor droped some frames byitself, will rebase and re-init algorithm!( %s, %d )\n", __FILE__, __LINE__ );
					pSensorState->tFramesTickStart = tick;
					pSensorState->iValidFrameIndex = 1;
				}
				else
				{
					// TRACE( CAM_INFO, "Info: dropped one frame for frame rate control!\n" );

					error = V4L2_Enqueue( pSensorState, *ppImgBuf );
					if ( error != CAM_ERROR_NONE )
					{
						return error;
					}

					continue;
				}
			}
			else
			{
				pSensorState->tFramesTickStart = tick;
				pSensorState->stConfig.iActualFpsQ16 = pSensorState->stConfig.iTargetFpsQ16;
				pSensorState->iValidFrameIndex++;
			}

			break;
		}
	}
	else
#endif
	{
		error = V4L2_DequeueWithSkip( pSensorState, ppImgBuf );
		if ( error != CAM_ERROR_NONE )
		{
			return error;
		}
	}

	if ( (*ppImgBuf)->iFlag & ( BUF_FLAG_L1CACHEABLE | BUF_FLAG_L2CACHEABLE | BUF_FLAG_BUFFERABLE ) )
	{
		CAM_Int32s i = 0;
		for ( i = 0; i < 3; i++ )
		{
			if ( (*ppImgBuf)->iAllocLen[i] > 0 )
			{
				osalbmm_flush_cache_range( (*ppImgBuf)->pBuffer[i], (*ppImgBuf)->iAllocLen[i], OSALBMM_DMA_FROM_DEVICE );
			}
		}
	}

	(void)_calcimglen( *ppImgBuf );

	// if in STILL state, stop flash
	if ( pSensorState->stConfig.eState == CAM_CAPTURESTATE_STILL && 
	     pSensorState->stAttribute.fnStopFlash != NULL )
	{
		error = pSensorState->stAttribute.fnStopFlash( pSensorState );
		if ( error != CAM_ERROR_NONE )
		{
			return error;
		}
	}

	return error;
}


CAM_Error V4L2_Flush( _V4L2SensorState *pSensorState )
{
	CAM_Int32s                 i = 0, ret;
	CAM_Int32s                 iBufType    = 0;
	CAM_Int32u                 pixelformat = 0;
	CAM_Int32u                 sizeimage   = 0;
	struct v4l2_format         format;
	struct v4l2_requestbuffers requestbuffers;

	// BAC check
	_CHECK_BAD_POINTER( pSensorState );

	if ( pSensorState->bIsStreamOn )
	{
		iBufType = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		ret = cam_ioctl( pSensorState->iSensorFD, VIDIOC_STREAMOFF, &iBufType );
		if ( 0 != ret )
		{
			TRACE( CAM_ERROR, "Error: V4L2 stream off sensor failed ( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
			return CAM_ERROR_DRIVEROPFAILED;
		}

		pSensorState->bIsStreamOn = CAM_FALSE;
	}

	memset( &requestbuffers, 0, sizeof( requestbuffers ) );

	requestbuffers.count  = 0;
	requestbuffers.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	requestbuffers.memory = V4L2_MEMORY_USERPTR;

	ret = cam_ioctl( pSensorState->iSensorFD, VIDIOC_REQBUFS, &requestbuffers );
	if ( 0 != ret )
	{
		TRACE( CAM_ERROR, "Error: V4L2 request buffers 0 failed( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_DRIVEROPFAILED;
	}

#if defined( BUILD_OPTION_WORKAROUND_V4L2_FLUSH_BY_REOPEN )

	ret = cam_close( pSensorState->iSensorFD );
	if( 0 != ret )
	{
		TRACE( CAM_ERROR, "Error: V4L2 close camera sensor[%s] failed( %s, %s, %d )\n", 
		       pSensorState->stAttribute.sV4L2SensorEntry.sDeviceName, __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_DRIVEROPFAILED;
	}
	pSensorState->iSensorFD = -1;

	pSensorState->iSensorFD = cam_open( pSensorState->stAttribute.stV4L2SensorEntry.sDeviceName, O_RDONLY );
	if ( pSensorState->iSensorFD < 0 )
	{
		TRACE( CAM_ERROR, "Error: V4L2 open sensor device[%s] failed( %s, %s, %d )\n", 
		       pSensorState->stAttribute.sV4L2SensorEntry.sDeviceName, __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_DRIVEROPFAILED;
	}

	ret = cam_ioctl( pSensorState->iSensorFD, VIDIOC_S_INPUT, &(pSensorState->stAttribute.stV4L2SensorEntry.iV4L2SensorID ) );
	if( 0 != ret )
	{
		TRACE( CAM_ERROR, "Error: V4L2 set input[sensor id: %d] failed( %s, %s, %d )\n", 
		       pSensorState->stAttribute.stV4L2SensorEntry.iV4L2SensorID, __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_DRIVEROPFAILED;
	}

#endif

	// set V4L2 format
	memset( &format, 0, sizeof( format ) );
	switch ( pSensorState->stConfig.eFormat )
	{
	case CAM_IMGFMT_YCbCr420P:
		pixelformat = V4L2_PIX_FMT_YUV420;
		sizeimage   = pSensorState->stConfig.iWidth * 3 / 2 * pSensorState->stConfig.iHeight;
		break;
	case CAM_IMGFMT_YCbCr422P:
		pixelformat = V4L2_PIX_FMT_YUV422P;
		sizeimage   = pSensorState->stConfig.iWidth * 2 * pSensorState->stConfig.iHeight;
		break;
	case CAM_IMGFMT_CbYCrY:
		pixelformat = V4L2_PIX_FMT_UYVY;
		sizeimage   = pSensorState->stConfig.iWidth * 2 * pSensorState->stConfig.iHeight;
		break;
	case CAM_IMGFMT_YCbYCr:
		pixelformat = V4L2_PIX_FMT_YUYV;
		sizeimage   = pSensorState->stConfig.iWidth * 2 * pSensorState->stConfig.iHeight;
		break;
	case CAM_IMGFMT_JPEG:
		pixelformat = V4L2_PIX_FMT_JPEG;
		sizeimage   = _calcjpegbuflen( pSensorState->stConfig.iWidth, pSensorState->stConfig.iHeight, &(pSensorState->stConfig.stJpegParam) );
		break;
	case CAM_IMGFMT_RGB565:
		pixelformat = V4L2_PIX_FMT_RGB565;
		sizeimage   = pSensorState->stConfig.iWidth * 2 * pSensorState->stConfig.iHeight;
		break;
	default:
		TRACE( CAM_ERROR, "Error: unrecognized format[%d]( %s, %s, %d )\n", pSensorState->stConfig.eFormat, __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_DRIVEROPFAILED;
		break;
	}
	format.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format.fmt.pix.sizeimage   = sizeimage;
	format.fmt.pix.pixelformat = pixelformat;
	format.fmt.pix.width       = pSensorState->stConfig.iWidth;
	format.fmt.pix.height      = pSensorState->stConfig.iHeight;

	ret = cam_ioctl( pSensorState->iSensorFD, VIDIOC_S_FMT, &format );
	if ( 0 != ret )
	{
		TRACE( CAM_ERROR, "Error: V4L2 set pixel format failed, error code[%d]( %s, %s, %d )\n", errno, __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_DRIVEROPFAILED;
	}

	// request buffer
	memset( &requestbuffers, 0, sizeof( requestbuffers ) );

	requestbuffers.count    = CAM_MAX_PORT_BUF_CNT;
	requestbuffers.type     = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	requestbuffers.memory   = V4L2_MEMORY_USERPTR;

	ret = cam_ioctl( pSensorState->iSensorFD, VIDIOC_REQBUFS, &requestbuffers );
	if ( 0 != ret )
	{
		TRACE( CAM_ERROR, "Error: V4L2 request buffers failed( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_DRIVEROPFAILED;
	}

	// flush buffers in driver
	for ( i = 0; i < CAM_MAX_PORT_BUF_CNT; i++ )
	{
		memset( &(pSensorState->stV4L2Buf[i]), 0, sizeof( _V4L2Buf ) );
	}
	pSensorState->iV4L2QueueBufCnt = 0;
	pSensorState->iV4L2BufCnt      = 0;

	return CAM_ERROR_NONE;
}


CAM_Error V4L2_Config( _V4L2SensorState *pSensorState, _CAM_SmartSensorConfig *pSensorConfig )
{
	CAM_Int32s                 i, ret;
	CAM_Error                  error = CAM_ERROR_NONE;
	CAM_Int32s                 iBufType;
	CAM_Int32u                 pixelformat;
	CAM_Int32u                 sizeimage = 0;
	struct v4l2_format         format;
	struct v4l2_requestbuffers requestbuffers;

	// BAC check
	_CHECK_BAD_POINTER( pSensorState );
	_CHECK_BAD_POINTER( pSensorConfig );

	pSensorConfig->bFlushed = CAM_FALSE;

	// if configure while idle, do nothing
	if ( pSensorState->stConfig.eState == CAM_CAPTURESTATE_IDLE && pSensorConfig->eState == CAM_CAPTURESTATE_IDLE )
	{
		return CAM_ERROR_NONE;
	}

	// fast path: if before/after state both not idle & their configure are the same & no reset tag, just flush buffer
	else if ( pSensorState->stConfig.eState  != CAM_CAPTURESTATE_IDLE && pSensorConfig->eState != CAM_CAPTURESTATE_IDLE &&
	          pSensorConfig->iResetType      == 0 &&
	          pSensorState->stConfig.eFormat == pSensorConfig->eFormat &&
	          pSensorState->stConfig.iTargetFpsQ16 == pSensorConfig->iTargetFpsQ16 &&
	          pSensorState->stConfig.iWidth  == pSensorConfig->iWidth &&
	          pSensorState->stConfig.iHeight == pSensorConfig->iHeight )
	{
		error = V4L2_Flush( pSensorState );
		
		if( error == CAM_ERROR_NONE )
		{
			pSensorConfig->bFlushed = CAM_TRUE;	
		}

		return error;
	}

	// if switch from preview to still, start flash and 3A statistics firstly
	if ( pSensorState->stConfig.eState == CAM_CAPTURESTATE_PREVIEW && 
	     pSensorConfig->eState         == CAM_CAPTURESTATE_STILL )
	{
		// start flash
		if ( pSensorState->stAttribute.fnStartFlash != NULL )
		{
			error = pSensorState->stAttribute.fnStartFlash( pSensorState );
			if ( error != CAM_ERROR_NONE )
			{
				return error;
			}
		}

		// TODO: may need sleep for a while to wait for 3A convergence

		// get 3A statistcs before switch to capture
		if ( pSensorState->stAttribute.fnSaveAeAwb != NULL )
		{
			error = pSensorState->stAttribute.fnSaveAeAwb( &pSensorState->stConfig, pSensorState->stAttribute.pSaveRestoreUserData );
			if ( error != CAM_ERROR_NONE )
			{
				return error;
			}
		}
	}

	if ( pSensorState->stConfig.eState != CAM_CAPTURESTATE_IDLE )
	{
		// stream off sensor
		if ( pSensorState->bIsStreamOn )
		{
			iBufType = V4L2_BUF_TYPE_VIDEO_CAPTURE;

			ret = cam_ioctl( pSensorState->iSensorFD, VIDIOC_STREAMOFF, &iBufType );
			if ( 0 != ret )
			{
				TRACE( CAM_ERROR, "Error: V4L2 stream off sensor failed( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
				return CAM_ERROR_DRIVEROPFAILED;
			}

			pSensorState->bIsStreamOn = CAM_FALSE;
		}

		// flush buffer from driver
		memset( &requestbuffers, 0, sizeof( requestbuffers ) );
		requestbuffers.count  = 0;
		requestbuffers.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		requestbuffers.memory = V4L2_MEMORY_USERPTR;

		ret = cam_ioctl( pSensorState->iSensorFD, VIDIOC_REQBUFS, &requestbuffers );
		if ( 0 != ret )
		{
			TRACE( CAM_ERROR, "Error: V4L2 request buffer 0 failed( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
			return CAM_ERROR_DRIVEROPFAILED;
		}

		for ( i = 0; i < CAM_MAX_PORT_BUF_CNT; i++ )
		{
			memset( &(pSensorState->stV4L2Buf[i]), 0, sizeof( _V4L2Buf ) );
		}
		pSensorState->iV4L2BufCnt      = 0;
		pSensorState->iV4L2QueueBufCnt = 0;
		pSensorConfig->bFlushed        = CAM_TRUE;

		// if need device reset, will re-open sensor driver
		if ( pSensorConfig->iResetType == CAM_RESET_COMPLETE )
		{
			ret = cam_close( pSensorState->iSensorFD );
			if( 0 != ret )
			{
				TRACE( CAM_ERROR, "Error: V4L2 close camera sensor[%s] failed( %s, %s, %d )\n", 
				       pSensorState->stAttribute.stV4L2SensorEntry.sDeviceName, __FILE__, __FUNCTION__, __LINE__ );
				return CAM_ERROR_DRIVEROPFAILED;
			}
			pSensorState->iSensorFD = -1;

			pSensorState->iSensorFD = cam_open( pSensorState->stAttribute.stV4L2SensorEntry.sDeviceName, O_RDONLY );
			if ( pSensorState->iSensorFD < 0 ) 
			{
				TRACE( CAM_ERROR, "Error: V4L2 open sensor device[%s] failed( %s, %s, %d )\n", 
				       pSensorState->stAttribute.stV4L2SensorEntry.sDeviceName, __FILE__, __FUNCTION__, __LINE__ );
				return CAM_ERROR_DRIVEROPFAILED;
			}

			ret = cam_ioctl( pSensorState->iSensorFD, VIDIOC_S_INPUT, &(pSensorState->stAttribute.stV4L2SensorEntry.iV4L2SensorID) );
			if( 0 != ret ) 
			{
				TRACE( CAM_ERROR, "Error: V4L2 set input[%d] failed( %s, %s, %d )\n", 
				       pSensorState->stAttribute.stV4L2SensorEntry.iV4L2SensorID, __FILE__, __FUNCTION__, __LINE__ );
				return CAM_ERROR_DRIVEROPFAILED;
			}
		}

	}

	if ( pSensorConfig->eState != CAM_CAPTURESTATE_IDLE )
	{
		// set V4L2 format
		memset( &format, 0, sizeof( format ) );
		switch ( pSensorConfig->eFormat )
		{
		case CAM_IMGFMT_YCbCr420P:
			pixelformat = V4L2_PIX_FMT_YUV420;
			sizeimage   = pSensorConfig->iWidth * 3 / 2 * pSensorConfig->iHeight;
			break;

		case CAM_IMGFMT_YCbCr422P:
			pixelformat = V4L2_PIX_FMT_YUV422P;
			sizeimage   = pSensorConfig->iWidth * 2 * pSensorConfig->iHeight;
			break;

		case CAM_IMGFMT_CbYCrY:
			pixelformat = V4L2_PIX_FMT_UYVY;
			sizeimage   = pSensorConfig->iWidth * 2 * pSensorConfig->iHeight;
			break;

		case CAM_IMGFMT_YCbYCr:
			pixelformat = V4L2_PIX_FMT_YUYV;
			sizeimage   = pSensorConfig->iWidth * 2 * pSensorConfig->iHeight;
			break;

		case CAM_IMGFMT_JPEG:
			pixelformat = V4L2_PIX_FMT_JPEG;
			sizeimage   = _calcjpegbuflen( pSensorConfig->iWidth, pSensorConfig->iHeight, &(pSensorConfig->stJpegParam) );
			TRACE( CAM_INFO, "Info: JPEG resolution( w * h ): %d * %d, JPEG quality: %d, transmit size = %d\n", 
			       pSensorConfig->iWidth, pSensorConfig->iHeight, pSensorConfig->stJpegParam.iQualityFactor, sizeimage );
			break;

		case CAM_IMGFMT_RGB565:
			pixelformat = V4L2_PIX_FMT_RGB565;
			sizeimage   = pSensorConfig->iWidth * 2 * pSensorConfig->iHeight;
			break;

		default:
			TRACE( CAM_ERROR, "Error: unrecognized format[%d]( %s, %s, %d )\n", pSensorConfig->eFormat, __FILE__, __FUNCTION__, __LINE__ );
			return CAM_ERROR_DRIVEROPFAILED;
			break;
		}

		format.fmt.pix.sizeimage   = sizeimage;
		format.fmt.pix.pixelformat = pixelformat;
		format.fmt.pix.width       = pSensorConfig->iWidth;
		format.fmt.pix.height      = pSensorConfig->iHeight;
		format.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		ret = cam_ioctl( pSensorState->iSensorFD, VIDIOC_S_FMT, &format );
		if ( 0 != ret )
		{
			TRACE( CAM_ERROR, "Error: V4L2 set pixel format failed, error code[%s] ( %s, %s, %d )\n", 
			       strerror( errno ), __FILE__, __FUNCTION__, __LINE__ );
			return CAM_ERROR_DRIVEROPFAILED;
		}
		
		// XXX: fake actual fps here
		pSensorConfig->iActualFpsQ16 = pSensorConfig->iTargetFpsQ16;

		// request buffer
		memset( &requestbuffers, 0, sizeof( requestbuffers ) );

		requestbuffers.count  = CAM_MAX_PORT_BUF_CNT;
		requestbuffers.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		requestbuffers.memory = V4L2_MEMORY_USERPTR;
		ret = cam_ioctl( pSensorState->iSensorFD, VIDIOC_REQBUFS, &requestbuffers );
		if ( 0 != ret )
		{
			TRACE( CAM_ERROR, "Error: V4L2 request buffer failed( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
			return CAM_ERROR_DRIVEROPFAILED;
		}

		pSensorState->iCurrentSkipCnt = pSensorState->iUnstableFrameCnt;

		// update the buffer requirement
		error = V4L2_GetBufReq( pSensorState, pSensorConfig, &pSensorState->stBufReq );
	}

	if ( pSensorState->stConfig.eState == CAM_CAPTURESTATE_PREVIEW && pSensorConfig->eState  == CAM_CAPTURESTATE_STILL )
	{
		if ( pSensorState->stAttribute.fnRestoreAeAwb != NULL )
		{
			CELOG("pSensorState->bIsStreamOn:%d, go to restore 3A\n",pSensorState->bIsStreamOn);
			error = pSensorState->stAttribute.fnRestoreAeAwb( pSensorConfig, pSensorState->stAttribute.pSaveRestoreUserData ); 
			if ( error != CAM_ERROR_NONE )
			{
				return error;
			}
		}
	}

#if defined( BUILD_OPTION_WORKAROUND_V4L2_FRAMERATECONTROL_BY_DROPFRAME )
	pSensorState->iValidFrameIndex = 0;
#endif

	return error;
}


CAM_Error V4L2_GetBufReq( _V4L2SensorState *pSensorState, _CAM_SmartSensorConfig *pSensorConfig, CAM_ImageBufferReq *pBufReq )
{
	void *pUserData = NULL;

	// BAC check
	_CHECK_BAD_POINTER( pSensorState );
	_CHECK_BAD_POINTER( pBufReq );

	if ( pSensorConfig->eState == CAM_CAPTURESTATE_IDLE )
	{
		memset( pBufReq, 0, sizeof( CAM_ImageBufferReq ) );
		return CAM_ERROR_NONE;
	}

	// V4L2 buffer requirement
	pBufReq->eFormat = pSensorConfig->eFormat;
	pBufReq->iWidth  = pSensorConfig->iWidth;
	pBufReq->iHeight = pSensorConfig->iHeight;

	// 4K alignment is reqired by sensor driver to do virtual --> physical conversion
	pBufReq->iAlignment[0] = 4096;	
	if ( pSensorConfig->eFormat == CAM_IMGFMT_YCbCr444P || pSensorConfig->eFormat == CAM_IMGFMT_YCbCr422P || pSensorConfig->eFormat == CAM_IMGFMT_YCbCr420P )
	{
		// 32 byte alignment is reqired by DMA
		pBufReq->iAlignment[1] = 32;
		pBufReq->iAlignment[2] = 32;	
	}
	else
	{
		pBufReq->iAlignment[1] = 0;
		pBufReq->iAlignment[2] = 0;
	}

	pBufReq->iRowAlign[0] = 1;
	if ( pSensorConfig->eFormat == CAM_IMGFMT_YCbCr444P || pSensorConfig->eFormat == CAM_IMGFMT_YCbCr422P || pSensorConfig->eFormat == CAM_IMGFMT_YCbCr420P )
	{
		pBufReq->iRowAlign[1] = 1;
		pBufReq->iRowAlign[2] = 1;
	}
	else
	{
		pBufReq->iRowAlign[1] = 0;
		pBufReq->iRowAlign[2] = 0;
	}

	pBufReq->iFlagOptimal = BUF_FLAG_PHYSICALCONTIGUOUS |
	                        BUF_FLAG_L1CACHEABLE | BUF_FLAG_L2CACHEABLE | BUF_FLAG_BUFFERABLE |
	                        BUF_FLAG_YUVBACKTOBACK | BUF_FLAG_FORBIDPADDING;

	pBufReq->iFlagAcceptable = pBufReq->iFlagOptimal | BUF_FLAG_L1NONCACHEABLE | BUF_FLAG_L2NONCACHEABLE | BUF_FLAG_UNBUFFERABLE;

	// iMinBufCount is the minimum buffer number that should be enqueued to V4L2 before at least one buffer can be dequeued
	pBufReq->iMinBufCount = 3;
	_calcstep( pBufReq->eFormat, pBufReq->iWidth, pBufReq->iRowAlign, pBufReq->iMinStep );

	pUserData = NULL;
	// print JPEG buffer size infomation
	if ( pSensorConfig->eFormat == CAM_IMGFMT_JPEG )
	{
		TRACE( CAM_INFO, "Info: JPEG width: %d, height: %d, quality: %d\n",
		       pSensorConfig->iWidth, pSensorConfig->iHeight, pSensorConfig->stJpegParam.iQualityFactor );
		pUserData = (void*)&( pSensorConfig->stJpegParam ); 
	}

	(void)_calcbuflen( pBufReq->eFormat, pBufReq->iWidth, pBufReq->iHeight, pBufReq->iMinStep, pBufReq->iMinBufLen, pUserData );
	/*	
	TRACE( CAM_INFO, "Info: format = %d, width = %d, height = %d, align = %d, step = ( %d, %d, %d ), size = ( %d, %d, %d )\n",
	       pBufReq->eFormat, pBufReq->iWidth, pBufReq->iHeight, pBufReq->iRowAlign,
	       pBufReq->iMinStep[0], pBufReq->iMinStep[1], pBufReq->iMinStep[2],
	       pBufReq->iMinBufLen[0], pBufReq->iMinBufLen[1], pBufReq->iMinBufLen[2] );
	*/

	return CAM_ERROR_NONE;
}

#define V4L2_CID_PRIVATE_FIRMWARE_DOWNLOAD ( V4L2_CID_PRIVATE_BASE + 0 )
CAM_Error V4L2_AFDownload( _V4L2SensorState *pSensorState )
{
	struct v4l2_control stCtrlParam = { V4L2_CID_PRIVATE_FIRMWARE_DOWNLOAD, 0 };
	CAM_Int32s          ret         = 0;
		
	ret = cam_ioctl( pSensorState->iSensorFD, VIDIOC_S_CTRL, &stCtrlParam );
	if ( ret != 0 )
	{
		TRACE( CAM_ERROR, "Error: AF firmware download failed( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_DRIVEROPFAILED;
	}

	TRACE( CAM_INFO, "Info: AF firmware download success( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
	return CAM_ERROR_NONE;
}


/*
 * sensor utility functions
 */
/******************************************************************************************************
//
// Name:         _get_sensor_reg
// Description:  get sensor register value
// Arguments:    iSensorFD   - sensor file descriptor
//               reg         - register's address
//               lbit, hbit  - start and end bit in src 8-bit register to get, 0<=lbit<=hbit<=15
//               val_lbit    - start bit in dest 16-bit val storing the bits gotten from src register
//               pVal         - pointer to the value to store the bits got from the register
// Return:       0: success, 1: fail
// Notes:        
// Version Log:  version      Date          Author    Description

*******************************************************************************************************/
CAM_Int32s _get_sensor_reg( CAM_Int32s iSensorFD, CAM_Int16u reg, CAM_Int32s lbit, CAM_Int32s hbit, CAM_Int32s val_lbit, CAM_Int16u *pVal )
{
	CAM_Int16u               mask;
	CAM_Int32s               nbits = hbit - lbit + 1;
	struct v4l2_dbg_register s;

	s.reg = (__u64)reg;
	s.val = 0;

	if ( 0 != cam_ioctl( iSensorFD, VIDIOC_DBG_G_REGISTER, &s ) )
	{
		TRACE( CAM_ERROR, "Error: V4L2 get register value failed, error code[%d]( %s, %s, %d )\n", errno, __FILE__, __FUNCTION__, __LINE__ );
		return 1;
	}

	// CELOG( "Info: 0x%x[0x%x]( %s )\n", (__u16)s.reg, (__u16)s.val, __FUNCTION__ );

	mask = ( ((1 << nbits) - 1) << val_lbit );
	(*pVal) &= ~mask;
	(*pVal) |= ( ( ((__u16)s.val >> lbit) << val_lbit ) & mask );



	TRACE( CAM_INFO, "Info: 0x%x[0x%x]( %s )\n", (__u16)s.reg, *pVal, __FUNCTION__ );

	return 0;
}



/******************************************************************************************************
//
// Name:         _set_sensor_reg
// Description:  set sensor register value
// Arguments:    iSensorFD   - sensor file descriptor
//               reg         - register's address
//               lbit, hbit  - start and end bit in src 8-bit register to get, 0<=lbit<=hbit<=15
//               val_lbit    - start bit in dest 16-bit val storing the bits gotten from src register
//               val         - the value that contains the bits that need to set to the register
// Return:       0: success, 1: fail
// Notes:        
// Version Log:  version      Date          Author    Description

*******************************************************************************************************/
CAM_Int32s _set_sensor_reg( CAM_Int32s iSensorFD, CAM_Int16u reg, CAM_Int32s lbit, CAM_Int32s hbit, CAM_Int32s val_lbit, CAM_Int16u val )
{
	CAM_Int64u               mask;
	CAM_Int32s               nbits = hbit - lbit + 1;
	struct v4l2_dbg_register s, s1;
	CAM_Int32s               ret = 0;

	s.reg = (__u64)reg;
	s.val = 0;
	
	if ( 0 != cam_ioctl( iSensorFD, VIDIOC_DBG_G_REGISTER, &s ) )
	{
		TRACE( CAM_ERROR, "Error: V4L2 get register value failed, error code[%d]( %s, %s, %d )\n", errno,  __FILE__, __FUNCTION__, __LINE__ );
		return 1;
	}

	s1 = s;

	mask = ( ((1 << nbits) - 1) << lbit );
	s.val &= (~mask);
	s.val |= ( ( ( (__u64)val >> val_lbit ) << lbit ) & mask );

	if ( s1.val != s.val )
	{
		ret = cam_ioctl( iSensorFD, VIDIOC_DBG_S_REGISTER, &s );

		if ( 0 != ret )
		{
			TRACE( CAM_ERROR, "Error: V4L2 set register value failed, error code[%d]( %s, %s, %d ) \n", errno,  __FILE__, __FUNCTION__, __LINE__ );
			return 1;
		}
#if  0
		{
			CAM_Int16u val1 = val;
			_get_sensor_reg( iSensorFD, reg, lbit, hbit, val_lbit, &val1 );
			val &= 0xff;
			val1 &= 0xff;
			if ( val1 != val )
			{
				ASSERT( val1 == val );
				TRACE( CAM_ERROR, "Error: _SET_SENSOR_REG failed: 0x%x[%d:%d] <-- 0x%x[%d:%d]\n", 
				       reg, lbit, hbit, val, val_lbit, val_lbit + hbit - lbit );
				TRACE( CAM_ERROR, "target: 0x%x, actual: 0x%x\n", val, val1 );
				return 1;
			}
		}
#endif
	}

#if 0
	{
		s.reg = reg;
		s.val = 0;
		ret = cam_ioctl( iSensorFD, VIDIOC_DBG_G_REGISTER, &s );
		if ( 0 == ret )
		{
			TRACE( CAM_INFO, "Info: %s: 0x%x[0x%x]-->0x%x[0x%x]\n", __FUNCTION__, (__u16)s1.reg, (__u16)s1.val, (__u16)s.reg, (__u16)s.val );
		}
		else
		{
			TRACE( CAM_ERROR, "Error: V4L2 get register value failed, error code[%d]( %s, %s, %d ) \n", 
			       errno,  __FILE__, __FUNCTION__, __LINE__ );
			return 1;
		}
	}
#endif

	return ret;
}


CAM_Int32s _set_reg_array( CAM_Int32s iSensorFD, const _CAM_RegEntry *p, CAM_Int32s size )
{
	CAM_Int32s ret = 0;
	CAM_Int32s i   = 0;

	ASSERT( p );

	// CELOG( "size: %d\n", size );
	for ( i = 0; i < size; i++ )
	{
		ret = _set_sensor_reg( iSensorFD, p[i].reg, p[i].lbit, p[i].hbit,p[i].val_lbit, p[i].val );
		if ( 0 != ret )
		{
			return ret;
		}
	} 

	return 0;   
}

