/*******************************************************************************
//(C) Copyright [2010 - 2011] Marvell International Ltd.
//All Rights Reserved
*******************************************************************************/

#ifndef _CAM_UTILITY_H_
#define _CAM_UTILITY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "CameraEngine.h"

/*
* some basic calculation MACROs 
*/
#define _ALIGN_TO(x,iAlign) ( (((CAM_Int32s)(x)) + (iAlign) - 1) & (~((iAlign) - 1)) )

#define _ROUND_TO(x,iAlign) ( (((CAM_Int32s)(x)) + (iAlign) / 2) & (~((iAlign) - 1)) )

#define _ARRAY_SIZE(array)  ( sizeof(array) / sizeof((array)[0]) )

#define _MAX(a,b)           ( (a) > (b) ? (a) : (b) )

#define _MIN(a,b)           ( (a) > (b) ? (b) : (a) )

#define _ABSDIFF(a,b)       ( (a) > (b) ? ((a) - (b)) : ((b) - (a)) )

#define _DIV_ROUND(a, b)    (((a) + ((b) >> 1)) / (b))

#define _DIV_CEIL(a, b)     (((a) + (b) - 1) / (b))

/*
* build optimization related MACROs 
*/
#define LIKELY(x)       __builtin_expect((x),1)
#define UNLIKELY(x)     __builtin_expect((x),0)

/*
 *@ data queue
 */
/*
 *@ data queue
 */
typedef struct _cam_queue 
{
    void       **pData;
    CAM_Int32s iDataNum;
    CAM_Int32s iDeQueuePos;
    CAM_Int32s iQueueCount;
} _CAM_Queue;

CAM_Int32s _InitQueue( _CAM_Queue *pQueue, CAM_Int32s iDataNum );
CAM_Int32s _DeinitQueue( _CAM_Queue *pQueue );
CAM_Int32s _EnQueue( _CAM_Queue *pQueue, void *pData );
CAM_Int32s _DeQueue( _CAM_Queue *pQueue, void **ppData );
CAM_Int32s _FlushQueue( _CAM_Queue *pQueue );
CAM_Int32s _GetQueue( _CAM_Queue *pQueue, CAM_Int32s iIndex, void **ppData );
CAM_Int32s _RemoveQueue( _CAM_Queue *pQueue, void *pData );
CAM_Int32s _RetrieveQueue( _CAM_Queue *pQueue, void *pData, int *pIndex );

/*
 *@ work thread
 */
typedef struct 
{
	CAM_Bool        bExitThread;    // flag to notify the work thread to exit
	void            *hEventWakeup;  // event to activate work thread
	void            *hEventExitAck;	// event to notify manager thread that the work thread has exited
	CAM_Int32s      iThreadID;      // work thread id
}_CAM_Thread;

void _CreateThread( _CAM_Thread *pThread, void *pWorkThreadFunc, void *pData );
void _DestroyThread( _CAM_Thread *pThread );

/*
 * basic ulitity functions
 */
CAM_Int32s  _calcjpegbuflen( CAM_Int32s iWidth, CAM_Int32s iHeight, CAM_JpegParam *pJpegParam );
CAM_Int32s  _calcbyteperpix( CAM_ImageFormat eFormat );
void        _calcimglen( CAM_ImageBuffer *pImgBuf );
void        _calcstep( CAM_ImageFormat eFormat, CAM_Int32s iWidth, CAM_Int32s iAlign[3], CAM_Int32s iStep[3] );
void        _calcbuflen( CAM_ImageFormat eFormat, CAM_Int32s iWidth, CAM_Int32s iHeight, CAM_Int32s iStep[3], CAM_Int32s iLength[3], void *pUserData );
void        _calccrop( CAM_Int32s iWidth, CAM_Int32s iHeight, CAM_Int32s iRatioW, CAM_Int32s iRatioH, CAM_Rect *pRect );
void        _calcpixpersample( CAM_ImageFormat eFormat, CAM_Int32s *pPixH, CAM_Int32s *pPixV );

#ifdef __cplusplus
}
#endif

#endif  // _CAM_UTILITY_H_
