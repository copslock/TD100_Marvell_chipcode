/******************************************************************************
//(C) Copyright [2010 - 2011] Marvell International Ltd.
//All Rights Reserved
******************************************************************************/

#ifndef _CAM_SOC_DEF_H_
#define _CAM_SOC_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "CamBoardDefs.h"


//////////////////////////////////////////////////////////////////////////////
// Graphics accelerator
//////////////////////////////////////////////////////////////////////////////
#define PLATFORM_GRAPHICS_WMMX			0
#define PLATFORM_GRAPHICS_WMMX2			1
#define PLATFORM_GRAPHICS_GCU			2
#define PLATFORM_GRAPHICS_GC200			3
#define PLATFORM_GRAPHICS_GC300			4
#define PLATFORM_GRAPHICS_GC500			5
#define PLATFORM_GRAPHICS_GC600			6
#define PLATFORM_GRAPHICS_GC800			7
#define PLATFORM_GRAPHICS_GC860			8
#define PLATFORM_GRAPHICS_GC1000		9
#define PLATFORM_GRAPHICS_GC1500		10
#define PLATFORM_GRAPHICS_GC2000		11

//////////////////////////////////////////////////////////////////////////////
// Video accelerator
//////////////////////////////////////////////////////////////////////////////
#define PLATFORM_VIDEO_WMMX                     0
#define PLATFORM_VIDEO_WMMX2                    1
#define PLATFORM_VIDEO_MVED                     2
#define PLATFORM_VIDEO_CNM                      3
#define PLATFORM_VIDEO_VMETA_V12                4
#define PLATFORM_VIDEO_VMETA_V13                5
#define PLATFORM_VIDEO_VMETA_V14                6
#define PLATFORM_VIDEO_VMETA_V15                7
#define PLATFORM_VIDEO_VMETA_V16                8
#define PLATFORM_VIDEO_VMETA_V17                9
#define PLATFORM_VIDEO_VMETA_V18                10


//////////////////////////////////////////////////////////////////////////////
// Camera interface
//////////////////////////////////////////////////////////////////////////////
#define PLATFORM_CAMERAIF_QCI			0x1
#define PLATFORM_CAMERAIF_CCIC			0x2
#define PLATFORM_CAMERAIF_ISPDMA		0x4


//////////////////////////////////////////////////////////////////////////////
// Processor features
//////////////////////////////////////////////////////////////////////////////
#if (PLATFORM_PROCESSOR_VALUE==PLATFORM_PROCESSOR_MHP) ||\
    (PLATFORM_PROCESSOR_VALUE==PLATFORM_PROCESSOR_MHL) ||\
    (PLATFORM_PROCESSOR_VALUE==PLATFORM_PROCESSOR_TAVORP) ||\
    (PLATFORM_PROCESSOR_VALUE==PLATFORM_PROCESSOR_TAVORL)

	#define PLATFORM_GRAPHICS_VALUE	    PLATFORM_GRAPHICS_GCU
	#define PLATFORM_VIDEO_VALUE        PLATFORM_VIDEO_WMMX2
	#define PLATFORM_CAMERAIF_VALUE	    PLATFORM_CAMERAIF_QCI

#elif (PLATFORM_PROCESSOR_VALUE==PLATFORM_PROCESSOR_MHLV) ||\
      (PLATFORM_PROCESSOR_VALUE==PLATFORM_PROCESSOR_TAVORPV) ||\
      (PLATFORM_PROCESSOR_VALUE==PLATFORM_PROCESSOR_TAVORP65)

	#define PLATFORM_GRAPHICS_VALUE     PLATFORM_GRAPHICS_GCU
	#define PLATFORM_VIDEO_VALUE        PLATFORM_VIDEO_MVED
	#define PLATFORM_CAMERAIF_VALUE	    PLATFORM_CAMERAIF_QCI

#elif (PLATFORM_PROCESSOR_VALUE==PLATFORM_PROCESSOR_TTC) ||\
      (PLATFORM_PROCESSOR_VALUE==PLATFORM_PROCESSOR_TAVORPV2)

	#define PLATFORM_GRAPHICS_VALUE	    PLATFORM_GRAPHICS_GC500
	#define PLATFORM_VIDEO_VALUE        PLATFORM_VIDEO_WMMX2
	#define PLATFORM_CAMERAIF_VALUE	    PLATFORM_CAMERAIF_CCIC

#elif (PLATFORM_PROCESSOR_VALUE==PLATFORM_PROCESSOR_TD)

	#define PLATFORM_GRAPHICS_VALUE	    PLATFORM_GRAPHICS_GC500
	#define PLATFORM_VIDEO_VALUE        PLATFORM_VIDEO_CNM
	#define PLATFORM_CAMERAIF_VALUE	    PLATFORM_CAMERAIF_CCIC

#elif (PLATFORM_PROCESSOR_VALUE==PLATFORM_PROCESSOR_MG1)

	#define PLATFORM_GRAPHICS_VALUE     PLATFORM_GRAPHICS_GC800
	#define PLATFORM_VIDEO_VALUE        PLATFORM_VIDEO_VMETA_V17
	#define PLATFORM_CAMERAIF_VALUE	    PLATFORM_CAMERAIF_CCIC

#elif (PLATFORM_PROCESSOR_VALUE==PLATFORM_PROCESSOR_MG2)

	#define PLATFORM_GRAPHICS_VALUE     PLATFORM_GRAPHICS_GC1000
	#define PLATFORM_VIDEO_VALUE        PLATFORM_VIDEO_VMETA_V18
	#define PLATFORM_CAMERAIF_VALUE	    PLATFORM_CAMERAIF_CCIC

#elif (PLATFORM_PROCESSOR_VALUE==PLATFORM_PROCESSOR_ASPEN)

	#define PLATFORM_GRAPHICS_VALUE	    PLATFORM_GRAPHICS_GC300
	#define PLATFORM_VIDEO_VALUE        PLATFORM_VIDEO_WMMX2
	#define PLATFORM_CAMERAIF_VALUE	    PLATFORM_CAMERAIF_CCIC

#elif (PLATFORM_PROCESSOR_VALUE==PLATFORM_PROCESSOR_MMP2)

	#define PLATFORM_GRAPHICS_VALUE	    PLATFORM_GRAPHICS_GC860
	#define PLATFORM_VIDEO_VALUE        PLATFORM_VIDEO_VMETA_V18
	#define PLATFORM_CAMERAIF_VALUE	    ( PLATFORM_CAMERAIF_ISPDMA | PLATFORM_CAMERAIF_CCIC )

#elif (PLATFORM_PROCESSOR_VALUE==PLATFORM_PROCESSOR_MMP3)

	#define PLATFORM_GRAPHICS_VALUE	    PLATFORM_GRAPHICS_GC2000
	#define PLATFORM_VIDEO_VALUE        PLATFORM_VIDEO_VMETA_V18
	#define PLATFORM_CAMERAIF_VALUE	    ( PLATFORM_CAMERAIF_ISPDMA | PLATFORM_CAMERAIF_CCIC )

#else

	#error can not define features for unknown processor

#endif





#ifdef __cplusplus
}
#endif

#endif  // _CAM_SOC_DEF_H_


