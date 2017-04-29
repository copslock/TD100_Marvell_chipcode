/*******************************************************************************
//(C) Copyright [2009 - 2011] Marvell International Ltd.
//All Rights Reserved
*******************************************************************************/

#ifndef _CAM_LOG_H_
#define _CAM_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include "CameraEngine.h"

#define CAM_ERROR 3
#define CAM_WARN  2
#define CAM_INFO  1

#if defined( ANDROID )
#include <utils/Log.h>
#define LOG_TAG "MrvlCameraEngine"
#define TRACE(dbgLevel,...)\
	do{\
		if ( dbgLevel == CAM_INFO )\
		{\
			LOGV( __VA_ARGS__ );\
		}\
		else if ( dbgLevel == CAM_WARN )\
		{\
			LOGW( __VA_ARGS__ );\
		}\
		else if ( dbgLevel == CAM_ERROR )\
		{\
			LOGE( __VA_ARGS__ );\
		}\
	}while(0)
#define CELOG LOGD

#else
#if defined( CAM_LOG_INFO )
#define TRACE( dbgLevel,... )\
do{\
	if ( dbgLevel >= CAM_INFO )\
	{\
		printf( __VA_ARGS__ );\
		fflush( NULL );\
	}\
}while ( 0 )

#elif defined( CAM_LOG_WARN )
#define TRACE( dbgLevel,... )\
do{\
	if ( dbgLevel >= CAM_WARN )\
	{\
		printf( __VA_ARGS__ );\
		fflush( NULL );\
	}\
}while ( 0 )
#else
#define TRACE( dbgLevel,... )\
	do{\
		if( dbgLevel >= CAM_ERROR )\
		{\
			printf( __VA_ARGS__ );\
			fflush( NULL );\
		}\
}while ( 0 )
#endif
#define CELOG(...) do{ printf( __VA_ARGS__ ); fflush( NULL ); }while( 0 )

#endif //ANDROID

#define ASSERT( v )\
if ( !(v) )\
{\
	CELOG( "assertion failed: '%s' at File %s, Line %d\n", #v, __FILE__, __LINE__ );\
	exit( -1 );\
}

#define CE_ECHO CELOG( "%s, %d\n", __FILE__, __LINE__ );

#define CE_FOPEN  fopen
#define CE_FPRINT fprintf
#define CE_FFLUSH fflush

//#if defined (WIN32) || defined (WINCE)
//#define assert_static(cond) typedef char __C_ASSERT_[(cond)?1:-1]
//#elif defined (LINUX)
#define assert_static(cond) extern char static_assertion[(cond)?1:-1]
//#else
//#error NOT SUPPORTED OS!!
//#endif

#define _CHECK_BAD_POINTER( p ) \
do {\
if ( (p) == NULL )\
{\
	TRACE( CAM_ERROR, "Error: input pointer is NULL( %s, %d )\n", __FILE__, __LINE__ );\
	return CAM_ERROR_BADPOINTER;\
}\
} while ( 0 )

#define _CHECK_STATE_LIMIT( s, state_mask ) \
do {\
if ( !((s) & (state_mask)) )\
{\
	TRACE( CAM_ERROR, "Error: CAM_ERROR_STATELIMIT( %s, %d )\n", __FILE__, __LINE__ );\
	return CAM_ERROR_STATELIMIT;\
}\
}while ( 0 )

#define _CHECK_BAD_SINGLE_PORT_ID( port )\
do {\
	if ((port) != CAM_PORT_PREVIEW &&\
	    (port) != CAM_PORT_VIDEO &&\
	    (port) != CAM_PORT_STILL)\
	{\
		TRACE( CAM_ERROR, "Error: bad port ID[%d]( %s, %d )\n", port, __FILE__, __LINE__ );\
		return CAM_ERROR_BADPORTID;\
	}\
} while ( 0 )

#define _CHECK_BAD_ANY_PORT_ID( port )\
do {\
	if ((port) != CAM_PORT_PREVIEW &&\
	    (port) != CAM_PORT_VIDEO &&\
	    (port) != CAM_PORT_STILL &&\
	    (port) != CAM_PORT_ANY)\
	{\
		TRACE( CAM_ERROR, "Error: bad port ID[%d]( %s, %d )\n", port, __FILE__, __LINE__ );\
		return CAM_ERROR_BADPORTID;\
	}\
} while ( 0 )

#define _CHECK_SUPPORT_ENUM( val, list, len )\
	do {\
		CAM_Int32s i;\
		for ( i = 0; i < (len); i++ )\
		{\
		if ( (list)[i] == (val) )\
	        {\
				break;\
			}\
		}\
		if ( i >= (len) )\
		{\
			TRACE( CAM_ERROR, "Error: enum[%d] is not supported( %s, %d )\n", val, __FILE__, __LINE__ );\
			return CAM_ERROR_NOTSUPPORTEDARG;\
		}\
	} while ( 0 )

#define _CHECK_SUPPORT_RANGE( val, min, max )\
	if ( (CAM_Int32s)(val) < (CAM_Int32s)(min) || (CAM_Int32s)(val) > (CAM_Int32s)(max) )\
	{\
		TRACE( CAM_ERROR, "Error: input out of range, should be in[%d, %d], but input is %d( %s, %d ) \n", min, max, val, __FILE__, __LINE__ );\
		return CAM_ERROR_NOTSUPPORTEDARG;\
	}


#define ASSERT_CAM_ERROR( err )\
	do {\
	switch ( err )\
	{\
	case CAM_ERROR_NONE:\
		break;\
	case CAM_WARN_DUPLICATEBUFFER:\
		TRACE( CAM_WARN, "Error Code: CAM_WARN_DUPLICATEBUFFER, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_BADPOINTER:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_BADPOINTER, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_BADARGUMENT:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_BADARGUMENT, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_BADSENSORID:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_BADSENSORID, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_BADPORTID:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_BADPORTID, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_PORTNOTVALID:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_PORTNOTVALID, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_PORTNOTCONFIGUREABLE:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_PORTNOTCONFIGUREABLE, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_BADBUFFER:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_BADBUFFER, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_BADSTATETRANSITION:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_BADSTATETRANSITION, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_STATELIMIT:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_STATELIMIT, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_SHOTMODELIMIT:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_SHOTMODELIMIT, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_NOTSUPPORTEDCMD:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_NOTSUPPORTEDCMD, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_NOTSUPPORTEDARG:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_NOTSUPPORTEDARG, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_NOTIMPLEMENTED:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_NOTIMPLEMENTED, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_NOTENOUGHBUFFERS:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_NOTENOUGHBUFFERS, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_BUFFERPREEMPTED:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_BUFFERPREEMPTED, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_PORTNOTACTIVE:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_PORTNOTACTIVE, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_OUTOFMEMORY:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_OUTOFMEMORY, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_OUTOFRESOURCE:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_OUTOFRESOURCE, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_TIMEOUT:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_TIMEOUT, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_DRIVEROPFAILED:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_DRIVEROPFAILED, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_PPUFAILED:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_PPUFAILED, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	case CAM_ERROR_FATALERROR:\
		TRACE( CAM_ERROR, "Error Code: CAM_ERROR_FATALERROR, %s - %d\n", __FILE__, __LINE__ );\
		break;\
	default:\
		TRACE( CAM_ERROR, "Unknown Error - %d, %s - %d\n", err, __FILE__, __LINE__ );\
		break;\
	}\
	if ( err > CAM_ERROR_STARTCODE ) {exit( -1 );}\
	} while ( 0 )

#ifdef __cplusplus
}
#endif

#endif  // _CAM_LOG_H_
