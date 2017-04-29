/******************************************************************************
*(C) Copyright [2010 - 2011] Marvell International Ltd.
* All Rights Reserved
******************************************************************************/


#include "test_harness.h"
#include <fcntl.h>

typedef int (*TestFunc)( CAM_DeviceHandle );

typedef struct _TestEntry 
{
	const char	*sName;
	TestFunc	pFunc;
} TestEntry;

extern int test_video_fps( CAM_DeviceHandle );
extern int test_dequeue_singleport( CAM_DeviceHandle );
extern int test_flush( CAM_DeviceHandle );
extern int test_state_transition( CAM_DeviceHandle );
extern int test_dv_dsc_actively_dequeue( CAM_DeviceHandle );
extern int test_dv_dsc_on_event_dequeue( CAM_DeviceHandle );
extern int test_camera_speed( CAM_DeviceHandle );
extern int test_res_switch_stress( CAM_DeviceHandle );
extern int test_digitalzoom_stress( CAM_DeviceHandle handle );

char dump_file_name[256] = "preivew.yuv";
int  iSensorID;

TestEntry all_tests[] = {
	{"DV/DSC (actively dequeue) test", test_dv_dsc_actively_dequeue},
	{"DV/DSC (dequeue based on event) test", test_dv_dsc_on_event_dequeue},
	{"Video fps test", test_video_fps},
	{"Camera speed", test_camera_speed},
	{"Dequeue single port test", test_dequeue_singleport},
	{"Flush port test", test_flush},
	{"State transition test", test_state_transition},
	{"Stress test of resolution switch", test_res_switch_stress},
	{"Stress test of sensor digital zoom", test_digitalzoom_stress },
};

int main( int argc, char* argv[] )
{
	char             str[32] = {0};
	int              i, ret = 0;
	CAM_Error        error  = CAM_ERROR_NONE;
	CAM_DeviceHandle handle = NULL;
	int              iTestNum = (int)( sizeof( all_tests ) / sizeof( all_tests[0] ) );

	setbuf( stdout, NULL );
	setbuf( stdin, NULL );

	do
	{
		if ( argc <= 1 )
		{
			TRACE( "Select the desired test\n" );
			for ( i = 0; i < iTestNum; i++ )
			{
				TRACE( "\t%d - %s\n", i, all_tests[i].sName );
			}
			TRACE( "\ta - all\n" );
			TRACE( "\tq - quit\n" );

			i = -1;
			str[0] = '\0';
			scanf( "%s", str );
			if ( strcmp( str, "q" ) == 0 || strcmp( str, "Q" ) == 0 )
			{
				return -1;
			}
			if ( strcmp( str, "a" ) == 0 || strcmp( str, "A" ) == 0 )
			{
				break;
			}

			i = str[0] - '0';
		}
		else
		{
			i = atoi( argv[1] );
			printf( "%d, %s\n", i, argv[2] );
		}
	} while( i < 0 || i >= iTestNum );

	if ( i >= 0 && i < iTestNum )
	{
		// enum sensors and get camera capability
		error = InitEngine( &handle );
		ASSERT_CAM_ERROR( error );

		// set current sensor ID 
		TRACE( "Pls set the sensor ID you want to test:\n" );
		scanf( "%d", &iSensorID );
		if ( i != 3 )
		{
			error = SetSensor( handle, iSensorID );
			ASSERT_CAM_ERROR( error );
		}
		ret = all_tests[i].pFunc( handle );
	}
	else
	{
		for ( i = 2; i < iTestNum; i++ )
		{
			ret = all_tests[i].pFunc( handle );
		}
		if ( ret != 0 )
		{
			return ret;
		}
	}

	error = DeinitEngine( handle );
	ASSERT_CAM_ERROR( error );

	return ret;
}
