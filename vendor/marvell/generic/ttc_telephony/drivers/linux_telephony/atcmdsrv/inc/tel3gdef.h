/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
 *  INTEL CONFIDENTIAL
 *  Copyright 2006 Intel Corporation All Rights Reserved.
 *  The source code contained or described herein and all documents related to the source code (“Material”) are owned
 *  by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
 *  its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
 *  Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
 *  treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual property right is granted to or
 *  conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
 *  estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
 *  Intel in writing.
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: tel3gdef.h
 *
 *  Description: This file contains the 2G / 3G related definition
 *
 *  Notes:
 *
 ******************************************************************************/



#ifndef TEL3GDEF_H
#define TEL3GDEF_H

#define PLMN_NAME_FULL_LENGTH     (26)  //Network Operator Length

#define STRING_LENGTH_40       (40)

#define MIN_USER_CALL_ID            (1)
#define MAX_USER_CALL_ID            (7)
#define NEVER_USED_USER_CALL_ID     (0)

/* AT+COPS*/
#define NUM_OF_COPS_FORMAT                      (4)

/*  Mobile Network Code */
typedef INT16 Mnc;

/* Mobile Country Code */
typedef INT16 Mcc;

typedef enum DelayClassTag
{
	TEL_GPRS_DELAY_CLASS_SUBSCRIBED = 0,
	TEL_GPRS_DELAY_CLASS_1 = 1,
	TEL_GPRS_DELAY_CLASS_2 = 2,
	TEL_GPRS_DELAY_CLASS_3 = 3,
	TEL_GPRS_DELAY_CLASS_4 = 4,
	TEL_GPRS_DELAY_CLASS_RESERVED = 7
} DelayClass;


typedef enum ReliabilityClassTag
{
	TEL_GPRS_RELIAB_CLASS_SUBSCRIBED = 0,
	TEL_GPRS_RELIAB_CLASS_1 = 1,
	TEL_GPRS_RELIAB_CLASS_2 = 2,
	TEL_GPRS_RELIAB_CLASS_3 = 3,
	TEL_GPRS_RELIAB_CLASS_4 = 4,
	TEL_GPRS_RELIAB_CLASS_5 = 5,
	TEL_GPRS_RELIAB_CLASS_RESERVED = 7
}
ReliabilityClass;


typedef enum PeakThroughputTag
{
	TEL_GPRS_PEAK_THRPT_SUBSCRIBED = 0,
	TEL_GPRS_PEAK_THRPT_UPTO_1KOCT = 1,
	TEL_GPRS_PEAK_THRPT_UPTO_2KOCT = 2,
	TEL_GPRS_PEAK_THRPT_UPTO_4KOCT = 3,
	TEL_GPRS_PEAK_THRPT_UPTO_8KOCT = 4,
	TEL_GPRS_PEAK_THRPT_UPTO_16KOCT = 5,
	TEL_GPRS_PEAK_THRPT_UPTO_32KOCT = 6,
	TEL_GPRS_PEAK_THRPT_UPTO_64KOCT = 7,
	TEL_GPRS_PEAK_THRPT_UPTO_128KOCT = 8,
	TEL_GPRS_PEAK_THRPT_UPTO_256KOCT = 9,
	TEL_GPRS_PEAK_THRPT_RESERVED = 0x0f,
	TEL_GPRS_PEAK_THRPT_HIGHEST = TEL_GPRS_PEAK_THRPT_UPTO_256KOCT
}
PeakThroughput;

typedef enum PrecedenceClassTag
{
	TEL_GPRS_PRECED_CLASS_SUBSCRIBED = 0,
	TEL_GPRS_PRECED_CLASS_1 = 1,
	TEL_GPRS_PRECED_CLASS_2 = 2,
	TEL_GPRS_PRECED_CLASS_3 = 3,
	TEL_GPRS_PRECED_CLASS_RESERVED = 7
}
PrecedenceClass;

typedef enum MeanThroughputTag
{
	TEL_GPRS_MEAN_THRPT_SUBSCRIBED  = 0,
	TEL_GPRS_MEAN_THRPT_100_OPH     = 1,
	TEL_GPRS_MEAN_THRPT_200_OPH     = 2,
	TEL_GPRS_MEAN_THRPT_500_OPH     = 3,
	TEL_GPRS_MEAN_THRPT_1K_OPH      = 4,
	TEL_GPRS_MEAN_THRPT_2K_OPH      = 5,
	TEL_GPRS_MEAN_THRPT_5K_OPH      = 6,
	TEL_GPRS_MEAN_THRPT_10K_OPH     = 7,
	TEL_GPRS_MEAN_THRPT_20K_OPH     = 8,
	TEL_GPRS_MEAN_THRPT_50K_OPH     = 9,
	TEL_GPRS_MEAN_THRPT_100K_OPH    = 10,
	TEL_GPRS_MEAN_THRPT_200K_OPH    = 11,
	TEL_GPRS_MEAN_THRPT_500K_OPH    = 12,
	TEL_GPRS_MEAN_THRPT_1M_OPH      = 13,
	TEL_GPRS_MEAN_THRPT_2M_OPH      = 14,
	TEL_GPRS_MEAN_THRPT_5M_OPH      = 15,
	TEL_GPRS_MEAN_THRPT_10M_OPH     = 16,
	TEL_GPRS_MEAN_THRPT_20M_OPH     = 17,
	TEL_GPRS_MEAN_THRPT_50M_OPH     = 18,
	TEL_GPRS_MEAN_THRPT_RESERVED    = 30,

	TEL_GPRS_MEAN_THRPT_BEST_EFFORT = 31
}
MeanThroughput;

#if defined (UPGRADE_R99)

typedef enum DeliveryOfErroneousSduTag
{
	TEL_GPRS_DLV_ERROR_SDU_SUBSCRIBED  = 0,
	TEL_GPRS_DLV_ERROR_SDU_NODETECT    = 1,
	TEL_GPRS_DLV_ERROR_SDU_YES         = 2,
	TEL_GPRS_DLV_ERROR_SDU_NO          = 3,
	TEL_GPRS_DLV_ERROR_SDU_RESERVED    = 7
}
DeliveryOfErroneousSdu;

typedef enum DeliveryOrderTag
{
	TEL_GPRS_DLV_ORDER_SUBSCRIBED  = 0,
	TEL_GPRS_DLV_ORDER_WITH        = 1,
	TEL_GPRS_DLV_ORDER_WITHOUT     = 2,
	TEL_GPRS_DLV_ORDER_RESERVED    = 3
}
DeliveryOrder;

typedef enum TrafficClassTag
{
	TEL_GPRS_TRAFFIC_CLASS_SUBSCRIBED       = 0,
	TEL_GPRS_TRAFFIC_CLASS_CONVERSATIONAL   = 1,
	TEL_GPRS_TRAFFIC_CLASS_STREAMING        = 2,
	TEL_GPRS_TRAFFIC_CLASS_INTERACTIVE      = 3,
	TEL_GPRS_TRAFFIC_CLASS_BACKGROUND       = 4,
	TEL_GPRS_TRAFFIC_CLASS_RESERVED         = 7
}
TrafficClass;

#define TEL_GPRS_MAX_SDU_SIZE_SUBSCRIBED    0
#define TEL_GPRS_MAX_SDU_SIZE_RESERVED      0xFF

#define TEL_GPRS_BIT_RATE_SUBSCRIBED        0

typedef enum ResidualBERTag
{
	TEL_GPRS_RESIDUAL_BER_SUBSCRIBED  = 0,
	TEL_GPRS_RESIDUAL_BER_5102        = 1,
	TEL_GPRS_RESIDUAL_BER_1102        = 2,
	TEL_GPRS_RESIDUAL_BER_5103        = 3,
	TEL_GPRS_RESIDUAL_BER_4103        = 4,
	TEL_GPRS_RESIDUAL_BER_1103        = 5,
	TEL_GPRS_RESIDUAL_BER_1104        = 6,
	TEL_GPRS_RESIDUAL_BER_1105        = 7,
	TEL_GPRS_RESIDUAL_BER_1106        = 8,
	TEL_GPRS_RESIDUAL_BER_6108        = 9,
	TEL_GPRS_RESIDUAL_BER_RESERVED    = 15
}
ResidualBER;

typedef enum SduErrorRatioTag
{
	TEL_GPRS_SDU_ERROR_RATIO_SUBSCRIBED  = 0,
	TEL_GPRS_SDU_ERROR_RATIO_1102        = 1,
	TEL_GPRS_SDU_ERROR_RATIO_7103        = 2,
	TEL_GPRS_SDU_ERROR_RATIO_1103        = 3,
	TEL_GPRS_SDU_ERROR_RATIO_1104        = 4,
	TEL_GPRS_SDU_ERROR_RATIO_1105        = 5,
	TEL_GPRS_SDU_ERROR_RATIO_1106        = 6,
	TEL_GPRS_SDU_ERROR_RATIO_1101        = 7,
	TEL_GPRS_SDU_ERROR_RATIO_RESERVED    = 15
}
SduErrorRatio;

typedef enum TrafficHandlingPriorityTag
{
	TEL_GPRS_TRAFFIC_PRIORITY_LEVEL_SUBSCRIBED  = 0,
	TEL_GPRS_TRAFFIC_PRIORITY_LEVEL_1           = 1,
	TEL_GPRS_TRAFFIC_PRIORITY_LEVEL_2           = 2,
	TEL_GPRS_TRAFFIC_PRIORITY_LEVEL_3           = 3
}
TrafficHandlingPriority;

#define TEL_GPRS_TRANSFER_DELAY_SUBSCRIBED  0
#define TEL_GPRS_TRANSFER_DELAY_RESERVED    0x3F

#endif

typedef struct
{
	unsigned short mcc;
	unsigned short mnc;
	unsigned short accessTechnology;
}Plmn;


#define INTERNATIONAL_PREFIX        '+'
#define MAX_ADDR_LEN                20
#define CCFC_NO_REPLY_TIME_DEFAULT  (20)

/* CREG */
typedef enum AtCregTag
{
	AT_CREG_DISABLED,
	AT_CREG_ENABLED,
	AT_CREG_ENABLED_WITH_LOCATION_INFO,
	AT_CREG_NUMBER_OF_SETTINGS
} AtCreg;

#endif


/* END OF FILE */
