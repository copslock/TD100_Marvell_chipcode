/******************************************************************************
//(C) Copyright [2010 - 2011] Marvell International Ltd.
//All Rights Reserved
******************************************************************************/

#ifndef _MARVELL_BOARD_DEF_H_
#define _MARVELL_BOARD_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
// Dev-platform types
//////////////////////////////////////////////////////////////////////////////
#define PLATFORM_BOARD_TTCDKB           0
#define PLATFORM_BOARD_TDDKB            1
#define PLATFORM_BOARD_PV2EVB           2
#define PLATFORM_BOARD_PV2SAARB         3
#define PLATFORM_BOARD_MG1EVB           4
#define PLATFORM_BOARD_MG1SAARB         5
#define PLATFORM_BOARD_AVLITE           6
#define PLATFORM_BOARD_DOVENB           7
#define PLATFORM_BOARD_BONNELL          8
#define PLATFORM_BOARD_FLINT            9
#define PLATFORM_BOARD_BROWNSTONE       10
#define PLATFORM_BOARD_ABILENE          11
#define PLATFORM_BOARD_G50              12


//////////////////////////////////////////////////////////////////////////////
// Processor types
//////////////////////////////////////////////////////////////////////////////
#define PLATFORM_PROCESSOR_MHP			0
#define PLATFORM_PROCESSOR_MHL			1
#define PLATFORM_PROCESSOR_MHLV			2
#define PLATFORM_PROCESSOR_TAVORP		3
#define PLATFORM_PROCESSOR_TAVORL		4
#define PLATFORM_PROCESSOR_TAVORPV		5
#define PLATFORM_PROCESSOR_TAVORP65		6
#define PLATFORM_PROCESSOR_TTC			7
#define PLATFORM_PROCESSOR_TD			8
#define PLATFORM_PROCESSOR_TAVORPV2		9
#define PLATFORM_PROCESSOR_MG1			10
#define PLATFORM_PROCESSOR_MG2			11
#define PLATFORM_PROCESSOR_ASPEN		12
#define PLATFORM_PROCESSOR_MMP2			13
#define PLATFORM_PROCESSOR_MMP3			14


//////////////////////////////////////////////////////////////////////////////
// Processor steppings
//////////////////////////////////////////////////////////////////////////////
#define PLATFORM_STEPPING_R0	0
#define PLATFORM_STEPPING_R1	1
#define PLATFORM_STEPPING_R2	2
#define PLATFORM_STEPPING_R3	3
#define PLATFORM_STEPPING_S0	4
#define PLATFORM_STEPPING_S1	5
#define PLATFORM_STEPPING_S2	6
#define PLATFORM_STEPPING_S3	7
#define PLATFORM_STEPPING_X0	8
#define PLATFORM_STEPPING_X1	9
#define PLATFORM_STEPPING_X2	10
#define PLATFORM_STEPPING_X3	11
#define PLATFORM_STEPPING_Y0	12
#define PLATFORM_STEPPING_Y1	13
#define PLATFORM_STEPPING_Y2	14
#define PLATFORM_STEPPING_Y3	15
#define PLATFORM_STEPPING_Z0	16
#define PLATFORM_STEPPING_Z1	17
#define PLATFORM_STEPPING_Z2	18
#define PLATFORM_STEPPING_Z3	19
#define PLATFORM_STEPPING_A0	20
#define PLATFORM_STEPPING_A1	21
#define PLATFORM_STEPPING_A2	22
#define PLATFORM_STEPPING_A3	23
#define PLATFORM_STEPPING_B0	24
#define PLATFORM_STEPPING_B1	25
#define PLATFORM_STEPPING_B2	26
#define PLATFORM_STEPPING_B3	27

//////////////////////////////////////////////////////////////////////////////
// LCD pannel size
//////////////////////////////////////////////////////////////////////////////
#define PLATFORM_LCD_QVGA_PORTRAIT          0
#define PLATFORM_LCD_QVGA_LANDSCAPE         1
#define PLATFORM_LCD_HVGA_PORTRAIT          2
#define PLATFORM_LCD_HVGA_LANDSCAPE         3
#define PLATFORM_LCD_VGA_PORTRAIT           4
#define PLATFORM_LCD_VGA_LANDSCAPE          5
#define PLATFORM_LCD_WVGA_PORTRAIT          6
#define PLATFORM_LCD_WVGA_LANDSCAPE         7
#define PLATFORM_LCD_XGA_PORTRAIT           8
#define PLATFORM_LCD_XGA_LANDSCAPE          9
#define PLATFORM_LCD_WXGA_PORTRAIT          10
#define PLATFORM_LCD_WXGA_LANDSCAPE         11
#define PLATFORM_LCD_UWXGA_LANDSCAPE        12


//////////////////////////////////////////////////////////////////////////////
// Camera sensor
//////////////////////////////////////////////////////////////////////////////

#define PLATFORM_CAMERASENSOR_NONE          0
#define PLATFORM_CAMERASENSOR_OV7670        1
#define PLATFORM_CAMERASENSOR_OV3640        2
#define PLATFORM_CAMERASENSOR_OV3647        3
#define PLATFORM_CAMERASENSOR_OV5642        4
#define PLATFORM_CAMERASENSOR_OV5640        5
#define PLATFORM_CAMERASENSOR_M6MO_OV8810   6
#define PLATFORM_CAMERASENSOR_VX6852        7
#define PLATFORM_CAMERASENSOR_LOGI_C500     8




#if defined (TTCDKB)

	#define PLATFORM_BOARD_VALUE        PLATFORM_BOARD_TTCDKB
	#define PLATFORM_PROCESSOR_VALUE    PLATFORM_PROCESSOR_TTC
	#define PLATFORM_LCD_VALUE          PLATFORM_LCD_VGA_PORTRAIT
	#define PLATFORM_CCIC_SENSOR_MCLK   24

#elif defined (TDDKB)

	#define PLATFORM_BOARD_VALUE        PLATFORM_BOARD_TDDKB
	#define PLATFORM_PROCESSOR_VALUE    PLATFORM_PROCESSOR_TD
	#define PLATFORM_LCD_VALUE          PLATFORM_LCD_VGA_PORTRAIT
	#define PLATFORM_CCIC_SENSOR_MCLK   24

#elif defined (PV2EVB)

	#define PLATFORM_BOARD_VALUE        PLATFORM_BOARD_PV2EVB
	#define PLATFORM_PROCESSOR_VALUE    PLATFORM_PROCESSOR_TAVORPV2
	#define PLATFORM_LCD_VALUE          PLATFORM_LCD_WVGA_PORTRAIT
	#define PLATFORM_CCIC_SENSOR_MCLK   26

#elif defined (PV2SAARB)

	#define PLATFORM_BOARD_VALUE        PLATFORM_BOARD_PV2SAARB
	#define PLATFORM_PROCESSOR_VALUE    PLATFORM_PROCESSOR_TAVORPV2
	#define PLATFORM_LCD_VALUE          PLATFORM_LCD_VGA_PORTRAIT
	#define PLATFORM_CCIC_SENSOR_MCLK   26

#elif defined (MG1SAARB)

	#define PLATFORM_BOARD_VALUE        PLATFORM_BOARD_MG1SAARB
	#define PLATFORM_PROCESSOR_VALUE    PLATFORM_PROCESSOR_MG1
	#define PLATFORM_LCD_VALUE          PLATFORM_LCD_VGA_PORTRAIT
	#define PLATFORM_CCIC_SENSOR_MCLK   26

#elif defined (MG1EVB)

	#define PLATFORM_BOARD_VALUE        PLATFORM_BOARD_MG1EVB
	#define PLATFORM_PROCESSOR_VALUE    PLATFORM_PROCESSOR_MG1
	#define PLATFORM_LCD_VALUE          PLATFORM_LCD_VGA_PORTRAIT
	#define PLATFORM_CCIC_SENSOR_MCLK   26

    #warning UNIT TEST IS NOT DONE ON THIS PLATFORM

#elif defined (AVLITE)

	#define PLATFORM_BOARD_VALUE        PLATFORM_BOARD_AVLITE
	#define PLATFORM_PROCESSOR_VALUE    PLATFORM_PROCESSOR_ASPEN
	#define PLATFORM_LCD_VALUE          PLATFORM_LCD_XVGA_LANDSCAPE
	#define PLATFORM_CCIC_SENSOR_MCLK   26

	#warning UNIT TEST IS NOT DONE ON THIS PLATFORM

#elif defined (BONNELL)

	#define PLATFORM_BOARD_VALUE        PLATFORM_BOARD_BONNELL
	#define PLATFORM_PROCESSOR_VALUE    PLATFORM_PROCESSOR_MMP2
	#define PLATFORM_LCD_VALUE          PLATFORM_LCD_WVGA_PORTRAIT
	#define PLATFORM_CCIC_SENSOR_MCLK   25

#elif defined (BROWNSTONE)

	#define PLATFORM_BOARD_VALUE        PLATFORM_BOARD_BROWNSTONE
	#define PLATFORM_PROCESSOR_VALUE    PLATFORM_PROCESSOR_MMP2
	#define PLATFORM_LCD_VALUE          PLATFORM_LCD_WXGA_LANDSCAPE
	#define PLATFORM_CCIC_SENSOR_MCLK   25

#elif defined (FLINT)

	#define PLATFORM_BOARD_VALUE        PLATFORM_BOARD_FLINT
	#define PLATFORM_PROCESSOR_VALUE    PLATFORM_PROCESSOR_MMP2
	#define PLATFORM_LCD_VALUE          PLATFORM_LCD_WVGA_PORTRAIT
	#define PLATFORM_CCIC_SENSOR_MCLK   26

	#warning UNIT TEST IS NOT DONE ON THIS PLATFORM

#elif defined (ABILENE)

	#define PLATFORM_BOARD_VALUE        PLATFORM_BOARD_ABILENE
	#define PLATFORM_PROCESSOR_VALUE    PLATFORM_PROCESSOR_MMP3
	#define PLATFORM_LCD_VALUE          PLATFORM_LCD_WXGA_LANDSCAPE
	#define PLATFORM_CCIC_SENSOR_MCLK   25

	#warning UNIT TEST IS NOT DONE ON THIS PLATFORM

#elif defined (G50)

	#define PLATFORM_BOARD_VALUE        PLATFORM_BOARD_G50
	#define PLATFORM_PROCESSOR_VALUE    PLATFORM_PROCESSOR_MMP2
	#define PLATFORM_LCD_VALUE          PLATFORM_LCD_UWXGA_LANDSCAPE
	#define PLATFORM_CCIC_SENSOR_MCLK   25

#else

	#error Platform is not defined!

#endif



#ifdef __cplusplus
}
#endif

#endif  // _MARVELL_BOARD_DEF_H_


