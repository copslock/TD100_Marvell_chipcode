/*******************************************************************************
//(C) Copyright [2010 - 2011] Marvell International Ltd.
//All Rights Reserved
*******************************************************************************/

#ifndef _CAM_OV5640_H_
#define _CAM_OV5640_H_

#ifdef __cplusplus
extern "C" {
#endif


// OV5640's registers

/*************************not validated *************************************/
//AE/AG controller
#define OV5640_REG_AECAGC         0x3503,0,1,0
//gain
#define OV5640_REG_GAINHIGH       0x350a,0,0,0  /* no use in practice */
#define OV5640_REG_GAINLOW        0x350b,0,7,0
//exposure time
#define OV5640_REG_EXPOSUREHIGH   0x3500,0,3,0
#define OV5640_REG_EXPOSUREMID    0x3501,0,7,0
#define OV5640_REG_EXPOSURELOW    0x3502,0,7,0
// VTS(Vertical Total Size)
#define OV5640_REG_VTSHIGH        0x350c,0,7,0
#define OV5640_REG_VTSLOW         0x350d,0,7,0

// AWB controller
#define OV5640_REG_AWBC_22        0x5196,0,7,0
// AWB  getting
#define OV5640_REG_CURRENT_AWBRHIGH       0x519f,0,3,0
#define OV5640_REG_CURRENT_AWBRLOW        0x51A0,0,7,0
#define OV5640_REG_CURRENT_AWBGHIGH       0x51A1,0,3,0
#define OV5640_REG_CURRENT_AWBGLOW        0x51A2,0,7,0
#define OV5640_REG_CURRENT_AWBBHIGH       0x519f,0,3,0
#define OV5640_REG_CURRENT_AWBBLOW        0x51A0,0,7,0
// WB manual setting
#define OV5640_REG_SET_AWBRHIGH           0x5198,0,3,0
#define OV5640_REG_SET_AWBRLOW            0x5199,0,7,0
#define OV5640_REG_SET_AWBGHIGH           0x519A,0,3,0
#define OV5640_REG_SET_AWBGLOW            0x519B,0,7,0
#define OV5640_REG_SET_AWBBHIGH           0x519C,0,3,0
#define OV5640_REG_SET_AWBBLOW            0x519D,0,7,0

/*************************not validated************************/

// ISO setting
#define OV5640_REG_SET_ISOHIGH            0x3a18,0,1,0
#define OV5640_REG_SET_ISOLOW             0x3a19,0,7,0

// color effect setting
#define OV5640_REG_SDE_CTL                0x5001,7,7,7
#define OV5640_REG_SDE_CTL2               0x5588,6,6,6
#define OV5640_REG_TONE_CTL               0x5580,3,4,3
#define OV5640_REG_NEG_CTL                0x5580,6,6,6
#define OV5640_REG_SAT_CTL                0x5580,1,1,1

#define OV5640_REG_U_CTL                  0x5583,0,7,0
#define OV5640_REG_V_CTL                  0x5584,0,7,0

// white balance setting
#define OV5640_REG_WB_CTL                 0x3406,0,0,0
#define OV5640_REG_WB_RHIGH               0x3400,0,3,0
#define OV5640_REG_WB_RLOW                0x3401,0,7,0
#define OV5640_REG_WB_GHIGH               0x3402,0,3,0
#define OV5640_REG_WB_GLOW                0x3403,0,7,0
#define OV5640_REG_WB_BHIGH               0x3404,0,3,0
#define OV5640_REG_WB_BLOW                0x3405,0,7,0

// metering windows weighting
#define OV5640_REG_EM_WIN0WEI             0x5688,0,3,0
#define OV5640_REG_EM_WIN1WEI             0x5688,4,7,4
#define OV5640_REG_EM_WIN2WEI             0x5689,0,3,0
#define OV5640_REG_EM_WIN3WEI             0x5689,4,7,4
#define OV5640_REG_EM_WIN10WEI            0x568a,0,3,0
#define OV5640_REG_EM_WIN11WEI            0x568a,4,7,4
#define OV5640_REG_EM_WIN12WEI            0x568b,0,3,0
#define OV5640_REG_EM_WIN13WEI            0x568b,4,7,4
#define OV5640_REG_EM_WIN20WEI            0x568c,0,3,0
#define OV5640_REG_EM_WIN21WEI            0x568c,4,7,4
#define OV5640_REG_EM_WIN22WEI            0x568d,0,3,0
#define OV5640_REG_EM_WIN23WEI            0x568d,4,7,4
#define OV5640_REG_EM_WIN30WEI            0x568e,0,3,0
#define OV5640_REG_EM_WIN31WEI            0x568e,4,7,4
#define OV5640_REG_EM_WIN32WEI            0x568f,0,3,0
#define OV5640_REG_EM_WIN33WEI            0x568f,4,7,4

// JPEG Quality
#define OV5640_REG_JPEG_QSCTL             0x4407,0,5,0

typedef struct 
{
	CAM_Int16u  gainQ4; // means that the lowest 4 bit of gainQ4 are fractional part
	CAM_Int16u  exposure;// uint is t_row_interval
	CAM_Int16u  awbRgain;
	CAM_Int16u  awbGgain;
	CAM_Int16u  awbBgain;
	CAM_Int32u  maxLines;
} _OV5640_3AState;


typedef struct 
{
	_V4L2SensorState    stV4L2;
	_OV5640_3AState     st3Astat;
} OV5640State;

static CAM_Error _OV5640SaveAeAwb( const _CAM_SmartSensorConfig *pOldConfig, void *pUserData );
static CAM_Error _OV5640RestoreAeAwb( const _CAM_SmartSensorConfig *pNewConfig, void *pUserData );

typedef void (*OV5640_ShotModeCap)( CAM_ShotModeCapability *pShotModeCap );

#ifdef __cplusplus
}
#endif

#endif  // _CAM_OV5640_H_
