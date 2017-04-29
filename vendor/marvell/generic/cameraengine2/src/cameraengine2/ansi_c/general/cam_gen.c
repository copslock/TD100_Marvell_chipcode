/******************************************************************************
//(C) Copyright [2009 - 2011] Marvell International Ltd.
//All Rights Reserved
******************************************************************************/


#include <stdlib.h>
#include <string.h>

#include "CameraEngine.h"

#include "cam_log.h"
#include "cam_trace.h"
#include "cam_extisp_buildopt.h"

typedef struct 
{
    const CAM_DriverEntry *pEntry;
    void  *pDeviceData;
}_CAM_DeviceHandle;


CAM_Error CAM_GetHandle( const CAM_DriverEntry *pDriverEntry, CAM_DeviceHandle *pHandle )
{
	CAM_Error         error   = CAM_ERROR_NONE;
	_CAM_DeviceHandle *handle = NULL;

	CELOG( "Welcome to use Marvell Camera Engine version %d.%d, deliverd by APSE-IPP Shanghai\n", CAM_ENGINE_VERSION_MAJOR, CAM_ENGINE_VERSION_MINOR );

	if ( pDriverEntry == NULL || pHandle == NULL ) 
	{
		TRACE( CAM_ERROR, "Error: NULL input( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
        return CAM_ERROR_BADARGUMENT;
	}

	handle = (_CAM_DeviceHandle*)malloc( sizeof(_CAM_DeviceHandle) );
	if ( handle == NULL ) 
	{
		TRACE( CAM_ERROR, "Error: out of memory to allocate for Camera Engine handle( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
		*pHandle = NULL;     
		return CAM_ERROR_OUTOFMEMORY;
	}

	handle->pEntry = pDriverEntry;
	error = pDriverEntry->pOpen( &handle->pDeviceData );

#if defined BUILD_OPTION_DEBUG_DUMP_CE_CALLING
	// init trace dump
	{
		int ret = 0;
		ret = _cam_inittrace();
		if ( ret != 0 )
		{
			TRACE( CAM_WARN, "Warning: calling Sequence Trace initial failed\n" );
		}
	}
#endif

	if ( error != CAM_ERROR_NONE )
	{
		free( handle );
		handle = NULL;
	}

	*pHandle = (CAM_DeviceHandle)handle;

    return error;
}

CAM_Error CAM_FreeHandle( CAM_DeviceHandle *pHandle )
{
	CAM_Error         error;
	_CAM_DeviceHandle *handle = NULL;

	if ( pHandle == NULL || *pHandle == NULL ) 
	{
		TRACE( CAM_ERROR, "Error: Camera Engine handle shouldn't be NULL( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
		return CAM_ERROR_BADARGUMENT;
	}

	handle = (_CAM_DeviceHandle*)*pHandle;
	error = handle->pEntry->pClose( &handle->pDeviceData );
	handle->pEntry = NULL;
	free( handle );
	*pHandle = NULL;

#if defined BUILD_OPTION_DEBUG_DUMP_CE_CALLING
	_cam_closetrace();
#endif

    return CAM_ERROR_NONE;
}

CAM_Error CAM_SendCommand( CAM_DeviceHandle handle, CAM_Command cmd, CAM_Param param1, CAM_Param param2 )
{
	CAM_Error         error;
	_CAM_DeviceHandle *pHandle = (_CAM_DeviceHandle*)handle;

#if defined BUILD_OPTION_DEBUG_DUMP_CE_CALLING

	CAM_Tick tick;
	_cam_devtrace( (void*)(pHandle->pDeviceData) );

#endif

	if ( pHandle == NULL ) 
	{
		TRACE( CAM_ERROR, "Error: Camera Engine handle shouldn't be NULL( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__  );
		return CAM_ERROR_BADARGUMENT;
	}

#if defined BUILD_OPTION_DEBUG_DUMP_CE_CALLING
	{
		CAM_Int32s sensor_id;
		pHandle->pEntry->pCommand( pHandle->pDeviceData, CAM_CMD_GET_SENSOR_ID, &sensor_id, NULL );

		_cam_cmdtrace( sensor_id, cmd, (void*)param1, (void*)param2, &tick );
	}
#endif

	error = pHandle->pEntry->pCommand( pHandle->pDeviceData, cmd, param1, param2 );

#if defined BUILD_OPTION_DEBUG_DUMP_CE_CALLING
	
	_cam_rettrace( cmd, (void*)param1, (void*)param2, error, tick );

#endif

    return error;
}

