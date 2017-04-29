/******************************************************************************
//(C) Copyright [2010 - 2011] Marvell International Ltd.
//All Rights Reserved
******************************************************************************/
#include "misc.h"

#include "cam_log.h"
#include "cam_trace.h"
#include "cam_extisp_buildopt.h"

CAM_CmdInfo stCmdInfo[] =
{
	// Used for intialization
	{ CAM_CMD_ENUM_SENSORS,             "CAM_CMD_ENUM_SENSORS"},
	{ CAM_CMD_QUERY_CAMERA_CAP,         "CAM_CMD_QUERY_CAMERA_CAP"},
	{ CAM_CMD_SET_EVENTHANDLER,         "CAM_CMD_SET_EVENTHANDLER"},	                                 
	{ CAM_CMD_SET_SENSOR_ID,            "CAM_CMD_SET_SENSOR_ID"},		
	{ CAM_CMD_GET_SENSOR_ID,            "CAM_CMD_GET_SENSOR_ID"},			
	{ CAM_CMD_SET_SENSOR_ORIENTATION,   "CAM_CMD_SET_SENSOR_ORIENTATION"},  
	{ CAM_CMD_GET_SENSOR_ORIENTATION,   "CAM_CMD_GET_SENSOR_ORIENTATION"},  
	// State Machine Management Commands
	{ CAM_CMD_SET_STATE,                "CAM_CMD_SET_STATE"},				
	{ CAM_CMD_GET_STATE,                "CAM_CMD_GET_STATE"},				
	// Buffer Management Commands
	{ CAM_CMD_PORT_GET_BUFREQ,          "CAM_CMD_PORT_GET_BUFREQ"},         
	{ CAM_CMD_PORT_ENQUEUE_BUF,         "CAM_CMD_PORT_ENQUEUE_BUF"},        
	{ CAM_CMD_PORT_DEQUEUE_BUF,         "CAM_CMD_PORT_DEQUEUE_BUF"},        
	{ CAM_CMD_PORT_FLUSH_BUF,           "CAM_CMD_PORT_FLUSH_BUF"},                 
	// Data Port Commands
	{ CAM_CMD_PORT_SET_SIZE,            "CAM_CMD_PORT_SET_SIZE"},           
	{ CAM_CMD_PORT_GET_SIZE,            "CAM_CMD_PORT_GET_SIZE"},           
	{ CAM_CMD_PORT_SET_FORMAT,          "CAM_CMD_PORT_SET_FORMAT"},         
	{ CAM_CMD_PORT_GET_FORMAT,          "CAM_CMD_PORT_GET_FORMAT"}, 
	{ CAM_CMD_PORT_SET_ROTATION,        "CAM_CMD_PORT_SET_ROTATION"}, 
	{ CAM_CMD_PORT_GET_ROTATION,        "CAM_CMD_PORT_GET_ROTATION"},
	// Preview Features
	{ CAM_CMD_SET_PREVIEW_FRAMERATE,    "CAM_CMD_SET_PREVIEW_FRAMERATE"},      				
	{ CAM_CMD_GET_PREVIEW_FRAMERATE,    "CAM_CMD_GET_PREVIEW_FRAMERATE"},    
	{ CAM_CMD_SET_PREVIEW_RESIZEFAVOR,  "CAM_CMD_SET_PREVIEW_RESIZEFAVOR"}, 
	{ CAM_CMD_GET_PREVIEW_RESIZEFAVOR,  "CAM_CMD_GET_PREVIEW_RESIZEFAVOR"}, 
	// Video Recording Features
	{ CAM_CMD_SET_VIDEO_FRAMERATE,      "CAM_CMD_SET_VIDEO_FRAMERATE"},     
	{ CAM_CMD_GET_VIDEO_FRAMERATE,      "CAM_CMD_GET_VIDEO_FRAMERATE"},     
	{ CAM_CMD_SET_VIDEO_STABLIZER,      "CAM_CMD_SET_VIDEO_STABLIZER"},     
	{ CAM_CMD_GET_VIDEO_STABLIZER,      "CAM_CMD_GET_VIDEO_STABLIZER"},     
	{ CAM_CMD_SET_VIDEO_NR,             "CAM_CMD_SET_VIDEO_NR"},            
	{ CAM_CMD_GET_VIDEO_NR,             "CAM_CMD_GET_VIDEO_NR"},                   
	// Still Capture Features
	{ CAM_CMD_SET_JPEGPARAM,            "CAM_CMD_SET_JPEGPARAM"},           
	{ CAM_CMD_GET_JPEGPARAM,            "CAM_CMD_GET_JPEGPARAM"},           
	{ CAM_CMD_SET_STILL_ZSL,            "CAM_CMD_SET_STILL_ZSL"},           
	{ CAM_CMD_GET_STILL_ZSL,            "CAM_CMD_GET_STILL_ZSL"},           
	{ CAM_CMD_SET_STILL_BURST,          "CAM_CMD_SET_STILL_BURST"},         
	{ CAM_CMD_GET_STILL_BURST,          "CAM_CMD_GET_STILL_BURST"},         
	{ CAM_CMD_SET_STILL_HDR,            "CAM_CMD_SET_STILL_HDR"},           
	{ CAM_CMD_GET_STILL_HDR,            "CAM_CMD_GET_STILL_HDR"},                  
	// Shooting Parameter Commands
	{ CAM_CMD_QUERY_SHOTMODE_CAP,	    "CAM_CMD_QUERY_SHOTMODE_CAP"},		
	{ CAM_CMD_SET_SHOTMODE,             "CAM_CMD_SET_SHOTMODE"},            
	{ CAM_CMD_GET_SHOTMODE,             "CAM_CMD_GET_SHOTMODE"},            
	{ CAM_CMD_SET_EXPOSUREMETERMODE,    "CAM_CMD_SET_EXPOSUREMETERMODE"},   
	{ CAM_CMD_GET_EXPOSUREMETERMODE,    "CAM_CMD_GET_EXPOSUREMETERMODE"},   
	{ CAM_CMD_SET_EVCOMPENSATION,       "CAM_CMD_SET_EVCOMPENSATION"},      
	{ CAM_CMD_GET_EVCOMPENSATION,       "CAM_CMD_GET_EVCOMPENSATION"},      
	{ CAM_CMD_SET_ISO,                  "CAM_CMD_SET_ISO"},                 
	{ CAM_CMD_GET_ISO,                  "CAM_CMD_GET_ISO"},                 
	{ CAM_CMD_SET_SHUTTERSPEED,         "CAM_CMD_SET_SHUTTERSPEED"},	    
	{ CAM_CMD_GET_SHUTTERSPEED,         "CAM_CMD_GET_SHUTTERSPEED"},        
	{ CAM_CMD_SET_FNUM,                 "CAM_CMD_SET_FNUM"},                
	{ CAM_CMD_GET_FNUM,                 "CAM_CMD_GET_FNUM"},                
	{ CAM_CMD_SET_BANDFILTER,           "CAM_CMD_SET_BANDFILTER"},          
	{ CAM_CMD_GET_BANDFILTER,           "CAM_CMD_GET_BANDFILTER"},          
	{ CAM_CMD_SET_FLASHMODE,            "CAM_CMD_SET_FLASHMODE"},           
	{ CAM_CMD_GET_FLASHMODE,            "CAM_CMD_GET_FLASHMODE"},          
	// white balance
	{ CAM_CMD_SET_WHITEBALANCEMODE,     "CAM_CMD_SET_WHITEBALANCEMODE"},    
	{ CAM_CMD_GET_WHITEBALANCEMODE,     "CAM_CMD_GET_WHITEBALANCEMODE"},    
	// focus
	{ CAM_CMD_SET_FOCUSMODE,            "CAM_CMD_SET_FOCUSMODE"},
	{ CAM_CMD_GET_FOCUSMODE,            "CAM_CMD_GET_FOCUSMODE"},  
	{ CAM_CMD_START_FOCUS,              "CAM_CMD_START_FOCUS"},   
	{ CAM_CMD_CANCEL_FOCUS,             "CAM_CMD_CANCEL_FOCUS"},          
	// zoom
	{ CAM_CMD_SET_OPTZOOM,              "CAM_CMD_SET_OPTZOOM"},             
	{ CAM_CMD_GET_OPTZOOM,              "CAM_CMD_GET_OPTZOOM"},             
	{ CAM_CMD_SET_DIGZOOM,              "CAM_CMD_SET_DIGZOOM"},             
	{ CAM_CMD_GET_DIGZOOM,              "CAM_CMD_GET_DIGZOOM"},             
	// post color/tone/brightness/sharpness adjustment
	{ CAM_CMD_SET_COLOREFFECT,          "CAM_CMD_SET_COLOREFFECT"},         
	{ CAM_CMD_GET_COLOREFFECT,          "CAM_CMD_GET_COLOREFFECT"},         
	{ CAM_CMD_SET_BRIGHTNESS,           "CAM_CMD_SET_BRIGHTNESS"},          
	{ CAM_CMD_GET_BRIGHTNESS,           "CAM_CMD_GET_BRIGHTNESS"},          
	{ CAM_CMD_SET_CONTRAST,             "CAM_CMD_SET_CONTRAST"},            
	{ CAM_CMD_GET_CONTRAST,             "CAM_CMD_GET_CONTRAST"},            
	{ CAM_CMD_SET_SATURATION,           "CAM_CMD_SET_SATURATION"},          
	{ CAM_CMD_GET_SATURATION,           "CAM_CMD_GET_SATURATION"},    
	{ CAM_CMD_SET_SHARPNESS,            "CAM_CMD_SET_SHARPNESS"},
	{ CAM_CMD_GET_SHARPNESS,            "CAM_CMD_GET_SHARPNESS"},
};

CAM_FlipRotInfo stFlipInfo[] =
{
	{ CAM_FLIPROTATE_NORMAL,"CAM_FLIPROTATE_NORMAL"},
	{ CAM_FLIPROTATE_90L,	"CAM_FLIPROTATE_90L"},
	{ CAM_FLIPROTATE_90R,	"CAM_FLIPROTATE_90R"},
	{ CAM_FLIPROTATE_180,	"CAM_FLIPROTATE_180"},
	{ CAM_FLIPROTATE_HFLIP, "CAM_FLIPROTATE_HFLIP"},             
	{ CAM_FLIPROTATE_VFLIP, "CAM_FLIPROTATE_VFLIP"},                
	{ CAM_FLIPROTATE_DFLIP, "CAM_FLIPROTATE_DFLIP"},                 
	{ CAM_FLIPROTATE_ADFLIP,"CAM_FLIPROTATE_ADFLIP"},                 
};

CAM_StateInfo stStateInfo[] = 
{
	{ CAM_CAPTURESTATE_NULL,        "CAM_CAPTURESTATE_NULL"},
	{ CAM_CAPTURESTATE_STANDBY,     "CAM_CAPTURESTATE_STANDBY"},			
	{ CAM_CAPTURESTATE_IDLE,        "CAM_CAPTURESTATE_IDLE"},				
	{ CAM_CAPTURESTATE_PREVIEW,     "CAM_CAPTURESTATE_PREVIEW"},			
	{ CAM_CAPTURESTATE_VIDEO,       "CAM_CAPTURESTATE_VIDEO"},				
	{ CAM_CAPTURESTATE_STILL,       "CAM_CAPTURESTATE_STILL"},			
};

CAM_PortInfo stPortInfo[] = 
{ 
	{ CAM_PORT_ANY     , "CAM_PORT_ANY"},
	{ CAM_PORT_NONE    , "CAM_PORT_NONE"},    
	{ CAM_PORT_PREVIEW , "CAM_PORT_PREVIEW"}, 
	{ CAM_PORT_VIDEO   , "CAM_PORT_VIDEO"},   
	{ CAM_PORT_STILL   , "CAM_PORT_STILL"},
};

CAM_ResInfo stErrorNoInfo[] = 
{
	{CAM_ERROR_NONE,                 "CAM_ERROR_NONE"},
	{CAM_ERROR_BADPOINTER,           "CAM_ERROR_BADPOINTER"},
	{CAM_ERROR_BADARGUMENT,          "CAM_ERROR_BADARGUMENT"},
	{CAM_ERROR_BADSENSORID,          "CAM_ERROR_BADSENSORID"},
	{CAM_ERROR_BADPORTID,            "CAM_ERROR_BADPORTID"},
	{CAM_ERROR_PORTNOTVALID,         "CAM_ERROR_PORTNOTVALID"},	
	{CAM_ERROR_PORTNOTCONFIGUREABLE, "CAM_ERROR_PORTNOTCONFIGUREABLE"},
	{CAM_ERROR_BADBUFFER,            "CAM_ERROR_BADBUFFER"},
	{CAM_ERROR_BADSTATETRANSITION,   "CAM_ERROR_BADSTATETRANSITION"},
	{CAM_ERROR_STATELIMIT,           "CAM_ERROR_STATELIMIT"},
	{CAM_ERROR_SHOTMODELIMIT,        "CAM_ERROR_SHOTMODELIMIT"},
	{CAM_ERROR_NOTSUPPORTEDCMD,      "CAM_ERROR_NOTSUPPORTEDCMD"},	
	{CAM_ERROR_NOTSUPPORTEDARG,      "CAM_ERROR_NOTSUPPORTEDARG"},
	{CAM_ERROR_NOTIMPLEMENTED,       "CAM_ERROR_NOTIMPLEMENTED"},
	{CAM_ERROR_NOTENOUGHBUFFERS,	 "CAM_ERROR_NOTENOUGHBUFFERS"},	
	{CAM_ERROR_PORTNOTACTIVE,        "CAM_ERROR_PORTNOTACTIVE"},
	{CAM_ERROR_OUTOFMEMORY,	         "CAM_ERROR_OUTOFMEMORY"},		
	{CAM_ERROR_OUTOFRESOURCE,        "CAM_ERROR_OUTOFRESOURCE"},	
	{CAM_ERROR_TIMEOUT,              "CAM_ERROR_TIMEOUT"},	
	{CAM_ERROR_DRIVEROPFAILED,       "CAM_ERROR_DRIVEROPFAILED"},
	{CAM_ERROR_PPUFAILED,            "CAM_ERROR_PPUFAILED"},
	{CAM_ERROR_FATALERROR,           "CAM_ERROR_FATALERROR"},
};

FILE       *pTraceFp = NULL;
CAM_Int32u uiRefCnt  = 0;


#if defined BUILD_OPTION_DEBUG_DUMP_CE_FILE
	#define CAMTRACE(fp,...)  CE_FPRINT(fp,__VA_ARGS__)
	#define TRACEFLUSH(fp)    CE_FFLUSH(fp)
#else
	#define CAMTRACE(fp,...)  CELOG( __VA_ARGS__ )
	#define TRACEFLUSH(a)   
#endif

int _cam_inittrace()
{
#if defined BUILD_OPTION_DEBUG_DUMP_CE_FILE
	if ( pTraceFp == NULL )
	{
		pTraceFp = CE_FOPEN( BUILD_OPTION_DEBUG_DUMP_CE_FILE, "a" );
		if ( NULL == pTraceFp ) 
		{
			return -1;
		}
		uiRefCnt++;
	}
#else
	if ( pTraceFp == NULL )
	{
		pTraceFp = stdout;
	}
#endif

	return 0;
}

void _cam_devtrace( void *pCameraState )
{
	if ( pTraceFp == NULL )
	{
		return;
	}
	if ( NULL == pCameraState ) 
	{
		CAMTRACE( pTraceFp, "Error: your device is NULL\n" );
		TRACEFLUSH( pTraceFp );
	}
	return;
}

void _cam_cmdtrace( CAM_Int32s iSensorId, CAM_Command cmd, void *arg1, void *arg2, CAM_Tick *pTick )
{
	int i, j;


	if ( pTraceFp == NULL )
	{
		return;
	}

	CAMTRACE( pTraceFp,"--------------------------------------------\n" );
	CAMTRACE( pTraceFp, "Sensor ID: %d\n", iSensorId );
	CAMTRACE( pTraceFp, "CMD: %s\n", stCmdInfo[cmd].pInfo );
	switch ( cmd )
	{
	case CAM_CMD_SET_SENSOR_ID:
		CAMTRACE( pTraceFp, "IN1: %d\n", (int)arg1 );
		break;

	case CAM_CMD_SET_SENSOR_ORIENTATION:
//		 CAMTRACE( pTraceFp, "IN1: %d\n", (int)arg1 );
		 CAMTRACE( pTraceFp, "IN1: %s\n", stFlipInfo[(int)arg1].pInfo );
		break;

	case CAM_CMD_SET_STATE:
		j = (int)arg1;
		i = -1;
		if ( j > 0 && j < (int)( sizeof( stStateInfo ) / sizeof( stStateInfo[0] ) ) )
		{
			while ( j ) 
			{
				i++;
				j >>=  1;
			}
		}
		if ( i < 0 )
		{
			CAMTRACE( pTraceFp, "IN1: %d\n", j );
		}
		else
		{
			CAMTRACE( pTraceFp, "IN1: %s\n", stStateInfo[i].pInfo );
		}
		break;

	case CAM_CMD_PORT_ENQUEUE_BUF:
		if ( (int)arg1 >= 3 || (int)arg1 < 0 ) 
		{
			CAMTRACE( pTraceFp, "IN1: %d\n", (int)arg1 );		    
		}
		else 
		{
			CAMTRACE( pTraceFp, "IN1: %s\n", stPortInfo[((int)arg1 - (CAM_PORT_ANY))].pInfo );
		}

		CAMTRACE( pTraceFp, "IN2: %p\n", arg2 );
		break;

	case CAM_CMD_PORT_DEQUEUE_BUF:

		if ( *(int*)arg1 >= 3 || *(int*)arg1 < 0 ) 
		{
			CAMTRACE( pTraceFp, "IN1: %s\n", stPortInfo[0].pInfo );		    
		}
		else 
		{
			CAMTRACE( pTraceFp, "IN1: %s\n", stPortInfo[(*(int*)arg1 - (CAM_PORT_ANY))].pInfo );
		}

		break;

	case CAM_CMD_PORT_FLUSH_BUF:
	case CAM_CMD_PORT_GET_BUFREQ:
		if ( (int)arg1 >= 3 || (int)arg1 < 0 ) 
		{
			CAMTRACE( pTraceFp, "IN1: %d\n", (int)arg1 );		    
		}
		else 
		{
			CAMTRACE( pTraceFp, "IN1: %s\n", stPortInfo[(int)arg1 - CAM_PORT_ANY].pInfo );
		}
		break;

	case CAM_CMD_PORT_SET_SIZE:
		{
			if ( (int)arg1 >= 3 || (int)arg1 < 0 ) 
			{
				CAMTRACE( pTraceFp, "IN1: %d\n", (int)arg1 );		    
			}
			else 
			{
				CAMTRACE( pTraceFp, "IN1: %s\n", stPortInfo[((int)arg1 - (CAM_PORT_ANY))].pInfo );
			}

			CAMTRACE( pTraceFp, "IN2: %d * %d\n", ((CAM_Size*)arg2)->iWidth, ((CAM_Size*)arg2)->iHeight );
		}
		break;

	case CAM_CMD_PORT_SET_FORMAT:

		if ( (int)arg1 >= 3 || (int)arg1 < 0 ) 
		{
			CAMTRACE( pTraceFp, "IN1: %d\n", (int)arg1 );		    
		}
		else 
		{
			CAMTRACE( pTraceFp, "IN1: %s\n", stPortInfo[((int)arg1 - (CAM_PORT_ANY))].pInfo );
		}

		CAMTRACE( pTraceFp, "IN2: %d\n", (int)arg2 );
		break;


	case CAM_CMD_SET_PREVIEW_FRAMERATE:
	case CAM_CMD_SET_VIDEO_FRAMERATE:

		CAMTRACE( pTraceFp, "IN1:%d\n", (CAM_Int32u)arg1 );
		CAMTRACE( pTraceFp, "COMMENT: in Q16 style\n" );

		break;

	case CAM_CMD_SET_JPEGPARAM:
		{
			CAM_JpegParam *pJpgParam = (CAM_JpegParam*)arg1; 	

			CAMTRACE( pTraceFp, "IN1:\n Sampling: %d (0-yuv420, 1-yuv422, 2-yuv444)\n QualityFactor: %d \n isEnableExif: %d \n \
			          isEnableThumnail: %d \n ThumbWidth: %d \n ThumbHeight: %d\n",\
			          pJpgParam->iSampling, pJpgParam->iQualityFactor, pJpgParam->bEnableExif, 
			          pJpgParam->bEnableThumb, pJpgParam->iThumbWidth, pJpgParam->iThumbHeight );

		}
		break;


	case CAM_CMD_SET_STILL_BURST:
	case CAM_CMD_SET_BANDFILTER:
	case CAM_CMD_SET_FLASHMODE:
	case CAM_CMD_SET_WHITEBALANCEMODE:
	case CAM_CMD_SET_OPTZOOM:
	case CAM_CMD_SET_DIGZOOM:
	case CAM_CMD_SET_COLOREFFECT:
	case CAM_CMD_SET_BRIGHTNESS:
	case CAM_CMD_SET_CONTRAST:
	case CAM_CMD_SET_SATURATION:
	case CAM_CMD_SET_ISO:
	case CAM_CMD_SET_EXPOSUREMETERMODE:
	case CAM_CMD_SET_EVCOMPENSATION:
	case CAM_CMD_SET_SHOTMODE:
	case CAM_CMD_SET_FOCUSMODE:
		CAMTRACE( pTraceFp, "IN1:%d\n", (int)arg1 );
		break;

	case CAM_CMD_GET_BANDFILTER:
	case CAM_CMD_GET_FLASHMODE:
	case CAM_CMD_GET_WHITEBALANCEMODE:
	case CAM_CMD_START_FOCUS:
	case CAM_CMD_CANCEL_FOCUS:
	case CAM_CMD_GET_FOCUSMODE:
	case CAM_CMD_GET_OPTZOOM:
	case CAM_CMD_GET_DIGZOOM:
	case CAM_CMD_GET_COLOREFFECT:
	case CAM_CMD_GET_BRIGHTNESS:
	case CAM_CMD_GET_CONTRAST:
	case CAM_CMD_GET_SATURATION:
	case CAM_CMD_ENUM_SENSORS:
	case CAM_CMD_QUERY_CAMERA_CAP:
	case CAM_CMD_GET_SENSOR_ID:
	case CAM_CMD_GET_SENSOR_ORIENTATION:
	case CAM_CMD_GET_STATE:
	case CAM_CMD_PORT_GET_FORMAT:
	case CAM_CMD_PORT_GET_SIZE:
	case CAM_CMD_PORT_GET_ROTATION:
	case CAM_CMD_GET_PREVIEW_FRAMERATE:
	case CAM_CMD_SET_PREVIEW_RESIZEFAVOR:
	case CAM_CMD_GET_PREVIEW_RESIZEFAVOR:
	case CAM_CMD_GET_VIDEO_FRAMERATE:
	case CAM_CMD_SET_VIDEO_STABLIZER:
	case CAM_CMD_GET_VIDEO_STABLIZER:
	case CAM_CMD_SET_VIDEO_NR:
	case CAM_CMD_GET_VIDEO_NR:
	case CAM_CMD_GET_JPEGPARAM:
	case CAM_CMD_GET_STILL_BURST:
	case CAM_CMD_SET_STILL_ZSL:
	case CAM_CMD_GET_STILL_ZSL:
	case CAM_CMD_SET_STILL_HDR:
	case CAM_CMD_GET_STILL_HDR:
	case CAM_CMD_QUERY_SHOTMODE_CAP:
	case CAM_CMD_GET_SHOTMODE:
	case CAM_CMD_GET_EXPOSUREMETERMODE:
	case CAM_CMD_GET_EVCOMPENSATION:
	case CAM_CMD_GET_ISO:
	case CAM_CMD_SET_SHUTTERSPEED:
	case CAM_CMD_GET_SHUTTERSPEED:
	case CAM_CMD_SET_FNUM:
	case CAM_CMD_GET_FNUM:
	case CAM_CMD_SET_EVENTHANDLER:
	default:
		break;
	}
	TRACEFLUSH( pTraceFp );

	*pTick = IPP_TimeGetTickCount();
	return;
}


void _cam_rettrace( CAM_Command cmd, void *arg1, void *arg2, CAM_Error eErrorNo, CAM_Tick tTick )
{
	if ( pTraceFp == NULL )
	{
		return;
	}

	tTick = IPP_TimeGetTickCount() - tTick;

	CAMTRACE( pTraceFp, "RES: %s\n", stErrorNoInfo[eErrorNo].pInfo );
	switch ( cmd )
	{
	case CAM_CMD_PORT_DEQUEUE_BUF:
		CAMTRACE( pTraceFp, "OUT1: %p\n", *(CAM_ImageBuffer**)arg2 );		    
		break;

 	case CAM_CMD_PORT_GET_BUFREQ:
		{	
			CAM_ImageBufferReq *bufReq = (CAM_ImageBufferReq*)arg2;
			CAMTRACE( pTraceFp, "OUT1: Format: %d, \n Buffer Requirement: %d, %d, %d, \n Min Buffer Count: %d,\n \
			          Min Buffer len: %d, %d,%d \n Image Size: %d * %d \n ",\
			          bufReq->eFormat, bufReq->iAlignment[0],bufReq->iAlignment[1],bufReq->iAlignment[2],\
			          bufReq->iMinBufCount, bufReq->iMinBufLen[0],bufReq->iMinBufLen[1],bufReq->iMinBufLen[2],\
			          bufReq->iWidth, bufReq->iHeight );
		}
		break;

	default:
		break;
	}
	CAMTRACE( pTraceFp, "Elapse: %.2f ms\n", tTick / 1000.0 );
	CAMTRACE( pTraceFp, "--------------------------------------------\n \n" );
	TRACEFLUSH( pTraceFp );

	return;
}


void _cam_closetrace()
{
	if ( pTraceFp == NULL ) 
	{
		return;
	}
#if defined BUILD_OPTION_DEBUG_DUMP_CE_FILE
	if ( --uiRefCnt == 0 )
	{
		(void)fclose( pTraceFp );
		pTraceFp = NULL;
	}
#endif

	return ;
}
