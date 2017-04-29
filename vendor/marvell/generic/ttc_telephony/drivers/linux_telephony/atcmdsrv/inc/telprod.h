/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: telprod.h
 *
 *  Description: Definition of propriatry AT Commands for production mode.
 *
 *  History:
 *   May 20, 2010 - Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/
#ifndef TEL_PROD_H
#define TEL_PROD_H

#include "teldef.h"

extern RETURNCODE_T  SysSleep (const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
extern RETURNCODE_T  AtMrdRfc (const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
extern RETURNCODE_T  AtMrdAdf (const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
extern RETURNCODE_T  AtMrdCdf (const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
extern RETURNCODE_T  AtMrdSn  (const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
extern RETURNCODE_T  AtMrdAuth(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
extern RETURNCODE_T  AtMrdImei(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
extern RETURNCODE_T  AtMrdMep (const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
extern RETURNCODE_T  AtVbatCal (const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
extern RETURNCODE_T  AtMrdVbatCal (const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
extern RETURNCODE_T  AtGpsTest (const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);

extern RETURNCODE_T  DiagCommand(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);

typedef struct vbat_calib_S {
	int gain;
	int offset;
}vbat_calib_T;
/************************************************************************\
 * GPS Test definitions and prototypes
\************************************************************************/
#define GPS_SHARED_OBJ_PATH				"/system/lib/liblsm_gsd4t.so"

#define GPSTEST_ATTR_OPER				0
#define GPSTEST_ATTR_READ				1

#define GPSTEST_PAR1_DEACT				0
#define GPSTEST_PAR1_ACT				1
#define GPSTEST_PAR1_READ_DEFAULT		0

#define GPSTEST_PAR2_MULTI_CHAN			0
#define GPSTEST_PAR2_SINGLE_CHAN		1

#define GPS_SIRF_CMD_TEST_MODE_OFF		0
#define GPS_SIRF_CMD_TEST_MODE_4		4
#define GPS_SIRF_CMD_TEST_MODE_5		5
#define GPS_SIRF_CMD_TEST_MODE_6		6
#define GPS_SIRF_CMD_TEST_MODE_7		7
#define GPS_SIRF_CMD_TEST_MODE_READ		0x20
#define GPS_SIRF_CMD_TEST_MODE_INIT		0x30
#define GPS_SIRF_CMD_TEST_MODE_DEINIT	0x40

#define SIRF_SUCCESS					0


typedef unsigned long int       tSIRF_BOOL;
typedef unsigned char           tSIRF_UCHAR;

typedef char                    tSIRF_CHAR;

typedef signed   char           tSIRF_INT8;
typedef unsigned char           tSIRF_UINT8;

typedef signed   short int      tSIRF_INT16;
typedef unsigned short int      tSIRF_UINT16;

typedef signed long int         tSIRF_INT32;
typedef unsigned long int       tSIRF_UINT32;

typedef double                  tSIRF_DOUBLE;
typedef float                   tSIRF_FLOAT;

typedef void *                  tSIRF_HANDLE;

typedef tSIRF_BOOL              LSM_BOOL;



typedef tSIRF_CHAR              LSM_CHAR;



typedef tSIRF_UINT8             LSM_UINT8;



typedef tSIRF_INT16             LSM_SINT16;

typedef tSIRF_UINT16            LSM_UINT16;



typedef unsigned int            LSM_UINT;



typedef tSIRF_INT32             LSM_SINT32;

typedef tSIRF_UINT32            LSM_UINT32;



typedef tSIRF_DOUBLE            LSM_DOUBLE;

typedef tSIRF_FLOAT             LSM_FLOAT;



tSIRF_INT32 sirf_gps_test_mode(LSM_SINT32 cmd, LSM_SINT32 svId, LSM_CHAR *data );


typedef struct tSIRF_MSG_SSB_TEST_MODE_DATA_tag
{
	tSIRF_UINT16 svid;               /**< fixed SVid to search on all channels */
	tSIRF_UINT16 period;             /**< number of seconds statistics are accumulated over */
	tSIRF_UINT16 bit_synch_time;     /**< time to first bit synch */
	tSIRF_UINT16 bit_count;          /**< Count of data bits came out during a period */
	tSIRF_UINT16 poor_status_count;  /**< Count of 100 ms periods tracker spent in any status < 3F */
	tSIRF_UINT16 good_status_count;  /**< Count of 100ms periods tracker spent in status 3F */
	tSIRF_UINT16 parity_error_count; /**< Number of word parity errors */
	tSIRF_UINT16 lost_vco_count;     /**< number of msec VCO lock loss was detected */
	tSIRF_UINT16 frame_synch_time;   /**< time to first frame synch */
	tSIRF_INT16 cno_mean;            /**< c/No mean in 0.1 dB-Hz */
	tSIRF_INT16 cno_sigma;           /**< c/No sigma in 0.1 dB */
	tSIRF_INT16 clock_drift;         /**< clock drift in 0.1 Hz */
	tSIRF_INT32 clock_offset_mean;   /**< average clock offset in 0.1 Hz */

	/**<  For bit test at a high c/no - Test Mode 3 only */
	tSIRF_INT16 bad_1khz_bit_count;  /**< bad bit count out of 10,000 (10 seconds * 1000 bits ) */
	tSIRF_INT32 abs_i20ms;           /**< phase noise estimate I20ms sum */
	tSIRF_INT32 abs_q1ms;            /**< phase noise estimate Q1ms sum */
	tSIRF_INT32 reserved[3];
} tSIRF_MSG_SSB_TEST_MODE_DATA;


typedef struct tSIRF_MSG_SSB_TEST_MODE_DATA_7_tag
{
	tSIRF_UINT8  test_status;        /**< 0 = in progress, 1 = complete */
	tSIRF_UINT32 spur1_frequency;    /**< first spur frequency */
	tSIRF_UINT16 spur1_sig_to_noise; /**< first spur power */
	tSIRF_UINT32 spur2_frequency;    /**< second spur frequency */
	tSIRF_UINT16 spur2_sig_to_noise; /**< second spur power */
	tSIRF_UINT32 spur3_frequency;    /**< third spur frequency */
	tSIRF_UINT16 spur3_sig_to_noise; /**< third spur power */
	tSIRF_UINT32 spur4_frequency;    /**< fourth spur frequency */
	tSIRF_UINT16 spur4_sig_to_noise; /**< fourth spur power */
}tSIRF_MSG_SSB_TEST_MODE_DATA_7;
#endif

