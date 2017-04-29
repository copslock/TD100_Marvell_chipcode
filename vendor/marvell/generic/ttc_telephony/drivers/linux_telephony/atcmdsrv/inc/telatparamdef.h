/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
 *  INTEL CONFIDENTIAL
 *  Copyright 2006 Intel Corporation All Rights Reserved.
 *  The source code contained or described herein and all documents related to the source code (“Material? are owned
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
 *  Filename: telatparamdef.h
 *
 *  Authors:  Vincent Yeung
 *
 *  Description: Defines for AT Command Parameters
 *
 *  History:
 *   May 19, 2006 - Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/



#ifndef TELATPARAMDEF_H
#define TELATPARAMDEF_H

#define TEL_AT_CMDSTR_MAX_LEN   (1024)

/* AT+CVHU */
#define TEL_AT_CVHU_0_MODE_MIN      0
#define TEL_AT_CVHU_0_MODE_MAX      2
#define TEL_AT_CVHU_0_MODE_DEFAULT  2
#define TEL_AT_CVHU_0_MODE_0        0
#define TEL_AT_CVHU_0_MODE_1        1
#define TEL_AT_CVHU_0_MODE_2        2

/* AT+CACM */
#define TEL_AT_CACM_0_PASSWD_STR_DEFAULT        NULL
#define TEL_AT_CACM_0_PASSWD_STR_MAX_LEN        TEL_AT_CMDSTR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH

/* AT+CAMM */
#define TEL_AT_CAMM_0_ACMMAX_STR_DEFAULT        NULL
#define TEL_AT_CAMM_0_ACMMAX_STR_MAX_LEN        TEL_AT_CMDSTR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH

#define TEL_AT_CAMM_1_PASSWD_STR_DEFAULT        NULL
#define TEL_AT_CAMM_1_PASSWD_STR_MAX_LEN        TEL_AT_CMDSTR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH

/* AT+CHLD */
#define TEL_AT_CHLD_0_N_VAL_MIN                         ATCI_CHLD_MODE_0
#define TEL_AT_CHLD_0_N_VAL_MAX                         (ATCI_CHLD_MODE_2X + 5)
#define TEL_AT_CHLD_0_N_VAL_DEFAULT                     ATCI_CHLD_MODE_0

/* AT+CBST */
#define TEL_AT_CBST_0_SPEED_VAL_MIN                             0
#define TEL_AT_CBST_0_SPEED_VAL_MAX                             134
#define TEL_AT_CBST_0_SPEED_VAL_DEFAULT                 7       /* 9600 bps */

#define TEL_AT_CBST_1_NAME_VAL_MIN                              0
#define TEL_AT_CBST_1_NAME_VAL_MAX                              7
#define TEL_AT_CBST_1_NAME_VAL_DEFAULT                  0 /* for asynchronous modem */

#define TEL_AT_CBST_2_CE_VAL_MIN                                0
#define TEL_AT_CBST_2_CE_VAL_MAX                                3
#define TEL_AT_CBST_2_CE_VAL_DEFAULT                    1 /* for the non-transparent */

/* AT+FCLASS */
#define TEL_AT_FCLASS_0_N_VAL_MIN                               0
#define TEL_AT_FCLASS_0_N_VAL_MAX                               1
#define TEL_AT_FCLASS_0_N_VAL_DEFAULT                   1

#define TEL_AT_FCLASS_FAX                                               1       /* According to TS27.007, it should be 1 */
#define TEL_AT_FCLASS_DATA                                              0       /* According to TS27.007, it should be 0 */

/* AT+CMOD */
#define TEL_AT_CMOD_0_MODE_VAL_MIN                              0
#define TEL_AT_CMOD_0_MODE_VAL_MAX                              3
#define TEL_AT_CMOD_0_MODE_VAL_DEFAULT                  0

/* AT+CLCC */

/* AT+CCFC */
#define TEL_AT_CCFC_0_REASON_VAL_MIN                            0
#define TEL_AT_CCFC_0_REASON_VAL_MAX                            5
#define TEL_AT_CCFC_0_REASON_VAL_DEFAULT                        0

#define TEL_AT_CCFC_1_MODE_VAL_MIN                              0
#define TEL_AT_CCFC_1_MODE_VAL_MAX                              4
#define TEL_AT_CCFC_1_MODE_VAL_DEFAULT                  0

#define TEL_AT_CCFC_2_NUMBER_STR_DEFAULT                NULL
#define TEL_AT_CCFC_2_NUMBER_STR_MAX_LEN                20

#define TEL_AT_CCFC_3_TYPE_VAL_MIN              0
#define TEL_AT_CCFC_3_TYPE_VAL_MAX              145
#define TEL_AT_CCFC_3_TYPE_VAL_DEFAULT          129

#define TEL_AT_CCFC_4_CLASSX_VAL_MIN                0
#define TEL_AT_CCFC_4_CLASSX_VAL_MAX                255
#define TEL_AT_CCFC_4_CLASSX_VAL_DEFAULT            7  /* CISS_BSMAP_DEFAULT */

#define TEL_AT_CCFC_5_SUBADDR_STR_DEFAULT       NULL
#define TEL_AT_CCFC_5_SUBADDR_STR_MAX_LEN       20

#define TEL_AT_CCFC_6_SATYPE_VAL_MIN              0
#define TEL_AT_CCFC_6_SATYPE_VAL_MAX              145
#define TEL_AT_CCFC_6_SATYPE_VAL_DEFAULT          128

#define TEL_AT_CCFC_7_TIME_VAL_MIN                1
#define TEL_AT_CCFC_7_TIME_VAL_MAX                30
#define TEL_AT_CCFC_7_TIME_VAL_DEFAULT            20 /* CI_CFNRY_TIMER_DEFAULT */


/* AT+CCWA */
#define TEL_AT_CCWA_0_N_VAL_MIN                0
#define TEL_AT_CCWA_0_N_VAL_MAX                1
#define TEL_AT_CCWA_0_N_VAL_DEFAULT            2  /* Empty */

#define TEL_AT_CCWA_1_MODE_VAL_MIN                0
#define TEL_AT_CCWA_1_MODE_VAL_MAX                2
#define TEL_AT_CCWA_1_MODE_VAL_DEFAULT            255 /* CCWA_MODE_DEFAULT, just to indicate not used*/

#define TEL_AT_CCWA_2_CLASSX_VAL_MIN                0
#define TEL_AT_CCWA_2_CLASSX_VAL_MAX                255
#define TEL_AT_CCWA_2_CLASSX_VAL_DEFAULT            7  /* CISS_BSMAP_DEFAULT */

/* AT+COLP */
#define TEL_AT_COLP_0_N_VAL_MIN                0
#define TEL_AT_COLP_0_N_VAL_MAX                1
#define TEL_AT_COLP_0_N_VAL_DEFAULT            0  /* disable */

/* AT+CLIP */
#define TEL_AT_CLIP_0_N_VAL_MIN                0
#define TEL_AT_CLIP_0_N_VAL_MAX                1
#define TEL_AT_CLIP_0_N_VAL_DEFAULT            0  /* disable */

/* AT+CLIR */
#define TEL_AT_CLIR_0_N_VAL_MIN                0
#define TEL_AT_CLIR_0_N_VAL_MAX                2
#define TEL_AT_CLIR_0_N_VAL_DEFAULT            0  /* disable */

/* AT+CNAP */
#define TEL_AT_CNAP_0_N_VAL_MIN                0
#define TEL_AT_CNAP_0_N_VAL_MAX                1
#define TEL_AT_CNAP_0_N_VAL_DEFAULT            0  /* disable */

/* AT+CAOC */
#define TEL_AT_CAOC_0_MODE_VAL_MIN                0
#define TEL_AT_CAOC_0_MODE_VAL_MAX                2
#define TEL_AT_CAOC_0_MODE_VAL_DEFAULT            0  /* query */

/* AT+VTS*/
#define TEL_AT_VTS_0_DTMF_STR_DEFAULT           NULL
#define TEL_AT_VTS_0_DTMF_STR_MAX_LEN           32

#define TEL_AT_VTS_1_DURATION_VAL_MIN           300
#define TEL_AT_VTS_1_DURATION_VAL_MAX           600
#define TEL_AT_VTS_1_DURATION_VAL_DEFAULT       300

/* AT+CRLP - michal Bukai - modified the default, max and min values*/
#define TEL_AT_CRLP_0_IWS_VAL_MIN                0
#define TEL_AT_CRLP_0_IWS_VAL_MAX                61             /* please refer to TS 24.022 [18] */
#define TEL_AT_CRLP_0_IWS_VAL_DEFAULT            61             /*  */

#define TEL_AT_CRLP_1_MWS_VAL_MIN                0
#define TEL_AT_CRLP_1_MWS_VAL_MAX                61             /* please refer to TS 24.022 [18] */
#define TEL_AT_CRLP_1_MWS_VAL_DEFAULT            61             /*  */

#define TEL_AT_CRLP_2_T1_VAL_MIN                39
#define TEL_AT_CRLP_2_T1_VAL_MAX                255             /* please refer to TS 24.022 [18] */
#define TEL_AT_CRLP_2_T1_VAL_DEFAULT            48              /*  */

#define TEL_AT_CRLP_3_N2_VAL_MIN                1
#define TEL_AT_CRLP_3_N2_VAL_MAX                255             /* please refer to TS 24.022 [18] */
#define TEL_AT_CRLP_3_N2_VAL_DEFAULT            6               /*  */

#define TEL_AT_CRLP_4_VER_VAL_MIN                0
#define TEL_AT_CRLP_4_VER_VAL_MAX                1              /* please refer to TS 24.022 [18] */
#define TEL_AT_CRLP_4_VER_VAL_DEFAULT            1              /*  */

#define TEL_AT_CRLP_5_T4_VAL_MIN                3
#define TEL_AT_CRLP_5_T4_VAL_MAX                255             /* please refer to TS 24.022 [18] */
#define TEL_AT_CRLP_5_T4_VAL_DEFAULT            3               /*  */

/* AT+CSSN */
#define TEL_AT_CSSN_0_N_VAL_MIN                0
#define TEL_AT_CSSN_0_N_VAL_MAX                1
#define TEL_AT_CSSN_0_N_VAL_DEFAULT            0  /* disable */

#define TEL_AT_CSSN_1_M_VAL_MIN                0
#define TEL_AT_CSSN_1_M_VAL_MAX                1
#define TEL_AT_CSSN_1_M_VAL_DEFAULT            0  /* disable */

/* AT+CUSD */
#define TEL_AT_CUSD_0_N_VAL_MIN                0
#define TEL_AT_CUSD_0_N_VAL_MAX                2
#define TEL_AT_CUSD_0_N_VAL_DEFAULT            0  /* disable */

#define TEL_AT_CUSD_1_STR_DEFAULT               NULL
#define TEL_AT_CUSD_1_STR_MAX_LEN               229        /* CI_MAX_USSD_LENGTH */

#define TEL_AT_CUSD_2_DCS_VAL_MIN                0
//change the MAX value for +CUSD since <dcs>: 3GPP TS 23.038 [25] Cell Broadcast Data Coding Scheme in integer format (default 0)
//#define TEL_AT_CUSD_0_N_VAL_MAX                1
#define TEL_AT_CUSD_2_DCS_VAL_MAX                0xF7   /* 3GPP TS 23.038 [25] */
#define TEL_AT_CUSD_2_DCS_VAL_DEFAULT            0x11   /* default is UCS2*/


/* AT+CLCK */
#define TEL_AT_CLCK_0_FAC_STR_DEFAULT                           NULL
#define TEL_AT_CLCK_0_FAC_STR_MAX_LEN                           2          /*  */

#define TEL_AT_CLCK_1_MODE_VAL_MIN                0
#define TEL_AT_CLCK_1_MODE_VAL_MAX                2
#define TEL_AT_CLCK_1_MODE_VAL_DEFAULT            0  /* unlock */

#define TEL_AT_CLCK_2_PASSWD_STR_DEFAULT                                NULL
#define TEL_AT_CLCK_2_PASSWD_STR_MAX_LEN                                16         /* CI_MAX_PASSWORD_LENGTH */

#define TEL_AT_CLCK_3_CLASSX_VAL_MIN                0
#define TEL_AT_CLCK_3_CLASSX_VAL_MAX                255
#define TEL_AT_CLCK_3_CLASSX_VAL_DEFAULT            7  /* CISS_BSMAP_DEFAULT */

/* AT+CCWE */
#define TEL_AT_CCWE_0_CLASSX_VAL_MIN                0
#define TEL_AT_CCWE_0_CLASSX_VAL_MAX                1
#define TEL_AT_CCWE_0_CLASSX_VAL_DEFAULT            0  /* disable */

/* AT+CPWD */
#define TEL_AT_CPWD_0_FAC_STR_DEFAULT                           NULL
#define TEL_AT_CPWD_0_FAC_STR_MAX_LEN                           2          /*  */

#define TEL_AT_CPWD_1_OLDPW_STR_DEFAULT                         NULL
#define TEL_AT_CPWD_1_OLDPW_STR_MAX_LEN                         16         /*  */

#define TEL_AT_CPWD_2_NEWPW_STR_DEFAULT                         NULL
#define TEL_AT_CPWD_2_NEWPW_STR_MAX_LEN                         16         /*  */

#define TEL_AT_CPWD_3_NEWPWVER_STR_DEFAULT                              NULL
#define TEL_AT_CPWD_3_NEWPWVER_STR_MAX_LEN                              16         /*  */

/* AT+COPS */
#define TEL_AT_COPS_0_MODE_VAL_MIN                0
#define TEL_AT_COPS_0_MODE_VAL_MAX                4
#define TEL_AT_COPS_0_MODE_VAL_DEFAULT            0  /* disable */

#define TEL_AT_COPS_1_FORMAT_VAL_MIN                0
#define TEL_AT_COPS_1_FORMAT_VAL_MAX                2
#define TEL_AT_COPS_1_FORMAT_VAL_DEFAULT            2  /* numeric */

#define TEL_AT_COPS_2_OPER_STR_DEFAULT                          NULL
#define TEL_AT_COPS_2_OPER_STR_MAX_LEN                          16         /* ATCI_PLMN_LONG_NAME_LENGTH */

#define TEL_AT_COPS_3_ACT_VAL_MIN                       0
#define TEL_AT_COPS_3_ACT_VAL_MAX                       2
#define TEL_AT_COPS_3_ACT_VAL_DEFAULT                   0  /* GSM */

/* AT+CPOL */
#define TEL_AT_CPOL_0_INDEX_VAL_MIN                1
#define TEL_AT_CPOL_0_INDEX_VAL_MAX                254
#define TEL_AT_CPOL_0_INDEX_VAL_DEFAULT            255 /*empty */

#define TEL_AT_CPOL_1_FORMAT_VAL_MIN                0
#define TEL_AT_CPOL_1_FORMAT_VAL_MAX                2
#define TEL_AT_CPOL_1_FORMAT_VAL_DEFAULT            2  /* NUM*/

#define TEL_AT_CPOL_2_OPER_STR_DEFAULT                          NULL
#define TEL_AT_CPOL_2_OPER_STR_MAX_LEN                          16         /* ATCI_PLMN_LONG_NAME_LENGTH */
/*Added by Michal Bukai - at support for AccTchMode parameter*/
#define TEL_AT_CPOL_3_GSM_ACT_VAL_MIN                0
#define TEL_AT_CPOL_3_GSM_ACT_VAL_MAX                1
#define TEL_AT_CPOL_3_GSM_ACT_VAL_DEFAULT            0

#define TEL_AT_CPOL_4_GSM_COMPACT_ACT_VAL_MIN                0
#define TEL_AT_CPOL_4_GSM_COMPACT_ACT_VAL_MAX                1
#define TEL_AT_CPOL_4_GSM_COMPACT_ACT_VAL_DEFAULT            0

#define TEL_AT_CPOL_5_UTRAN_ACT_VAL_MIN                0
#define TEL_AT_CPOL_5_UTRAN_ACT_VAL_MAX                1
#define TEL_AT_CPOL_5_UTRAN_ACT_VAL_DEFAULT            0

/* AT+CFUN */
#define TEL_AT_CFUN_0_FUN_VAL_MIN                0
#define TEL_AT_CFUN_0_FUN_VAL_MAX                127
#define TEL_AT_CFUN_0_FUN_VAL_DEFAULT            1  /* full functionality */

#define TEL_AT_CFUN_1_RST_VAL_MIN                0
#define TEL_AT_CFUN_1_RST_VAL_MAX                1
#define TEL_AT_CFUN_1_RST_VAL_DEFAULT            0 /* To be compatible with Anite GCF script, Phone will always be reset for both 0->1 or 1->0 power level setting circle. */


/*AT*CFUN - Add a generic comm. feature configuration option in CI_DEV_PRIM_SET_FUNC_REQ*/
#define TEL_AT_CFUN_2_EN_CONFIG_VAL_MIN                0
#define TEL_AT_CFUN_2_EN_CONFIG_VAL_MAX                1
#define TEL_AT_CFUN_2_EN_CONFIG_VAL_DEFAULT            0

#define TEL_AT_CFUN_3_CONFIG_VAL_MIN                0
#define TEL_AT_CFUN_3_CONFIG_VAL_MAX                63 /* bitmap for comm. configurations for CSD, FAX, PRODUCTION, CONVENTIONAL_GPS, MS_BASED_GPS, MS_ASSISTED_GPS */
#define TEL_AT_CFUN_3_CONFIG_VAL_DEFAULT            0


/* AT+CGED */
#define TEL_AT_CGED_0_MODE_VAL_MIN                0
#define TEL_AT_CGED_0_MODE_VAL_MAX                2
#define TEL_AT_CGED_0_MODE_VAL_DEFAULT            0  /* reprot the EM information once */

#define TEL_AT_CGED_1_INTERVAL_VAL_MIN                1
#define TEL_AT_CGED_1_INTERVAL_VAL_MAX                65535
#define TEL_AT_CGED_1_INTERVAL_VAL_DEFAULT            15 /* default time interval 15 seconds  */


/*
 *  PS Related
 */
#define TEL_AT_PS_CID_VAL_MAX                                   11      /* CI_PS_MAX_PDP_CTX_NUM */
#define TEL_AT_PS_CID_VAL_DEFAULT                               1       /* CI_PS_MAX_PDP_CTX_NUM */

/* AT+CGATT */
#define TEL_AT_CGATT_0_STATE_VAL_MIN                0
#define TEL_AT_CGATT_0_STATE_VAL_MAX                1
#define TEL_AT_CGATT_0_STATE_VAL_DEFAULT            1  /* attached */

/* AT+CGACT */
#define TEL_AT_CGACT_0_STATE_VAL_MIN                0
#define TEL_AT_CGACT_0_STATE_VAL_MAX                1
#define TEL_AT_CGACT_0_STATE_VAL_DEFAULT            1  /* activated */

#define TEL_AT_CGACT_1_CID_VAL_MIN                0
#define TEL_AT_CGACT_1_CID_VAL_MAX                TEL_AT_PS_CID_VAL_MAX
#define TEL_AT_CGACT_1_CID_VAL_DEFAULT            TEL_AT_PS_CID_VAL_DEFAULT

/* AT+CGDATA */
#define TEL_AT_CGDATA_0_L2P_STR_DEFAULT                         "PPP"
#define TEL_AT_CGDATA_0_L2P_STR_MAX_LEN                         TEL_AT_CMDSTR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH

#define TEL_AT_CGDATA_1_CID_VAL_MIN                0
#define TEL_AT_CGDATA_1_CID_VAL_MAX                TEL_AT_PS_CID_VAL_MAX
#define TEL_AT_CGDATA_1_CID_VAL_DEFAULT            TEL_AT_PS_CID_VAL_DEFAULT

/* AT+CGDCONT */
#define TEL_AT_CGDCONT_0_CID_VAL_MIN                0
#define TEL_AT_CGDCONT_0_CID_VAL_MAX                TEL_AT_PS_CID_VAL_MAX
#define TEL_AT_CGDCONT_0_CID_VAL_DEFAULT            TEL_AT_PS_CID_VAL_DEFAULT

#define TEL_AT_CGDCONT_1_PDPTYPE_STR_DEFAULT             "IP"
#define TEL_AT_CGDCONT_1_PDPTYPE_STR_MAX_LEN             TEL_AT_CMDSTR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH

#define TEL_AT_CGDCONT_2_APN_STR_DEFAULT             NULL
#define TEL_AT_CGDCONT_2_APN_STR_MAX_LEN             100

#define TEL_AT_CGDCONT_3_PDPADDRESS_STR_DEFAULT             NULL
#define TEL_AT_CGDCONT_3_PDPADDRESS_STR_MAX_LEN             16 /* CI_PS_PDP_IP_V6_SIZE */

#define TEL_AT_CGDCONT_4_DCOMP_VAL_MIN                0
#define TEL_AT_CGDCONT_4_DCOMP_VAL_MAX                3
#define TEL_AT_CGDCONT_4_DCOMP_VAL_DEFAULT            0  /*  */

#define TEL_AT_CGDCONT_5_HCOMP_VAL_MIN                0
#define TEL_AT_CGDCONT_5_HCOMP_VAL_MAX                4
#define TEL_AT_CGDCONT_5_HCOMP_VAL_DEFAULT            0  /*  */

/* AT+CGDSCONT */
#define TEL_AT_CGDSCONT_0_CID_VAL_MIN                0
#define TEL_AT_CGDSCONT_0_CID_VAL_MAX                TEL_AT_PS_CID_VAL_MAX
#define TEL_AT_CGDSCONT_0_CID_VAL_DEFAULT            TEL_AT_PS_CID_VAL_DEFAULT

#define TEL_AT_CGDSCONT_1_PCID_VAL_MIN                0
#define TEL_AT_CGDSCONT_1_PCID_VAL_MAX                255
#define TEL_AT_CGDSCONT_1_PCID_VAL_DEFAULT            0  /*  */

#define TEL_AT_CGDSCONT_2_DCOMP_VAL_MIN                0
#define TEL_AT_CGDSCONT_2_DCOMP_VAL_MAX                3
#define TEL_AT_CGDSCONT_2_DCOMP_VAL_DEFAULT            0  /*  */

#define TEL_AT_CGDSCONT_3_HCOMP_VAL_MIN                0
#define TEL_AT_CGDSCONT_3_HCOMP_VAL_MAX                4
#define TEL_AT_CGDSCONT_3_HCOMP_VAL_DEFAULT            0  /*  */


/* AT+CGQMIN */
#define TEL_AT_CGQMIN_0_CID_VAL_MIN                0
#define TEL_AT_CGQMIN_0_CID_VAL_MAX                255
#define TEL_AT_CGQMIN_0_CID_VAL_DEFAULT            0

#define TEL_AT_CGQMIN_1_PRECEDENCE_VAL_MIN                0
#define TEL_AT_CGQMIN_1_PRECEDENCE_VAL_MAX                255
#define TEL_AT_CGQMIN_1_PRECEDENCE_VAL_DEFAULT            0  /* 3GPP TS 23.107 [46] */

#define TEL_AT_CGQMIN_2_DELAY_VAL_MIN                0
#define TEL_AT_CGQMIN_2_DELAY_VAL_MAX                255
#define TEL_AT_CGQMIN_2_DELAY_VAL_DEFAULT            0  /*  3GPP TS 23.107 [46]*/

#define TEL_AT_CGQMIN_3_RELIABILITY_VAL_MIN                0
#define TEL_AT_CGQMIN_3_RELIABILITY_VAL_MAX                255
#define TEL_AT_CGQMIN_3_RELIABILITY_VAL_DEFAULT            0  /* 3GPP TS 23.107 [46] */

#define TEL_AT_CGQMIN_4_PEAK_VAL_MIN                0
#define TEL_AT_CGQMIN_4_PEAK_VAL_MAX                255
#define TEL_AT_CGQMIN_4_PEAK_VAL_DEFAULT            0  /*  3GPP TS 23.107 [46] */

#define TEL_AT_CGQMIN_5_MEAN_VAL_MIN                0
#define TEL_AT_CGQMIN_5_MEAN_VAL_MAX                255
#define TEL_AT_CGQMIN_5_MEAN_VAL_DEFAULT            0  /* 3GPP TS 23.107 [46] */

/* AT+CGQREQ */

#define TEL_AT_CGQREQ_0_CID_VAL_MIN                0
#define TEL_AT_CGQREQ_0_CID_VAL_MAX                255
#define TEL_AT_CGQREQ_0_CID_VAL_DEFAULT            0

/* AT+CGEQREQ and AT+CGEQMIN */
#define TEL_AT_CGEQREQ_0_CID_VAL_MIN                0
#define TEL_AT_CGEQREQ_0_CID_VAL_MAX                255
#define TEL_AT_CGEQREQ_0_CID_VAL_DEFAULT            0  /*  */

#define TEL_AT_CGEQREQ_1_TRAFFIC_VAL_MIN                0
#define TEL_AT_CGEQREQ_1_TRAFFIC_VAL_MAX                4       /*  */
#define TEL_AT_CGEQREQ_1_TRAFFIC_VAL_DEFAULT            4       /*  CI_PS_3G_TRAFFIC_CLASS_SUBSCRIBED */

#define TEL_AT_CGEQREQ_2_MAX_UL_VAL_MIN                0
#define TEL_AT_CGEQREQ_2_MAX_UL_VAL_MAX                8640             /* in kbs */
#define TEL_AT_CGEQREQ_2_MAX_UL_VAL_DEFAULT            0                /* (refer TS 24.008 [8] subclause 10.5.6.5 */

#define TEL_AT_CGEQREQ_3_MAX_DL_VAL_MIN                0
#define TEL_AT_CGEQREQ_3_MAX_DL_VAL_MAX                8640
#define TEL_AT_CGEQREQ_3_MAX_DL_VAL_DEFAULT            0  /*  */

#define TEL_AT_CGEQREQ_4_GUA_UL_VAL_MIN                0
#define TEL_AT_CGEQREQ_4_GUA_UL_VAL_MAX                8640
#define TEL_AT_CGEQREQ_4_GUA_UL_VAL_DEFAULT            0  /*  */

#define TEL_AT_CGEQREQ_5_GUA_DL_VAL_MIN                0
#define TEL_AT_CGEQREQ_5_GUA_DL_VAL_MAX                8640
#define TEL_AT_CGEQREQ_5_GUA_DL_VAL_DEFAULT            0  /*  */

#define TEL_AT_CGEQREQ_6_DELORDER_VAL_MIN               0
#define TEL_AT_CGEQREQ_6_DELORDER_VAL_MAX               2
#define TEL_AT_CGEQREQ_6_DELORDER_VAL_DEFAULT           2  /* CI_PS_3G_DLV_ORDER_SUBSCRIBED */

#define TEL_AT_CGEQREQ_7_MAXSDU_VAL_MIN                0
#define TEL_AT_CGEQREQ_7_MAXSDU_VAL_MAX                1520
#define TEL_AT_CGEQREQ_7_MAXSDU_VAL_DEFAULT            0  /*  subscribed value */

#define TEL_AT_CGEQREQ_8_SDUERR_VAL_MIN                0
#define TEL_AT_CGEQREQ_8_SDUERR_VAL_MAX                15       /* GPRS_SDU_ERROR_RATIO_RESERVED */
#define TEL_AT_CGEQREQ_8_SDUERR_VAL_DEFAULT            0        /* GPRS_SDU_ERROR_RATIO_SUBSCRIBED */

#define TEL_AT_CGEQREQ_9_RBERR_VAL_MIN                0
#define TEL_AT_CGEQREQ_9_RBERR_VAL_MAX                3
#define TEL_AT_CGEQREQ_9_RBERR_VAL_DEFAULT            0  /* GPRS_RESIDUAL_BER_SUBSCRIBED */

#define TEL_AT_CGEQREQ_10_DERSDU_VAL_MIN                0
#define TEL_AT_CGEQREQ_10_DERSDU_VAL_MAX                3
#define TEL_AT_CGEQREQ_10_DERSDU_VAL_DEFAULT            0  /*  */

#define TEL_AT_CGEQREQ_11_TRDELAY_VAL_MIN                0
#define TEL_AT_CGEQREQ_11_TRDELAY_VAL_MAX                1024           /* ?? */
#define TEL_AT_CGEQREQ_11_TRDELAY_VAL_DEFAULT            0              /* refer TS 24.008 [8] subclause 10.5.6.5 */

#define TEL_AT_CGEQREQ_12_PRIORITY_VAL_MIN                0
#define TEL_AT_CGEQREQ_12_PRIORITY_VAL_MAX                3
#define TEL_AT_CGEQREQ_12_PRIORITY_VAL_DEFAULT            0  /*  CI_PS_3G_SDU_TRAFFIC_PRIORITY_SUBSCRIBED */

/* AT+CGSMS */
#define TEL_AT_CGSMS_0_SERVICE_VAL_MIN                                          0
#define TEL_AT_CGSMS_0_SERVICE_VAL_MAX                                          3
#define TEL_AT_CGSMS_0_SERVICE_VAL_DEFAULT                                      3 /* CI_MSG_MOSMS_SRV_CS_PREFERRED */

/* AT+CSIM */
#define TEL_AT_CSIM_0_LENGTH_VAL_MIN                                            0
#define TEL_AT_CSIM_0_LENGTH_VAL_MAX
#define TEL_AT_CSIM_0_LENGTH_VAL_DEFAULT                                        3 /* CI_MSG_MOSMS_SRV_CS_PREFERRED */

/*
 *  PB related
 */
#define TEL_AT_PB_INDEX_VAL_MIN                                         0
#define TEL_AT_PB_INDEX_VAL_MAX                                         65535
#define TEL_AT_PB_INDEX_VAL_DEFAULT                                     0

/* AT+CPBS */
#define TEL_AT_CPBS_0_STORAGE_STR_DEFAULT                               "ME"
#define TEL_AT_CPBS_0_STORAGE_STR_MAX_LEN                               4

#define TEL_AT_CPBS_1_PASSWORD_STR_DEFAULT                              NULL
#define TEL_AT_CPBS_1_PASSWORD_STR_MAX_LEN                              16


/*
 *  SMS Related
 */

#define TEL_SMS_MAX_ADDR_LEN 80
#define TEL_AT_SMS_PDUMODE_MAX_LEN 232

#define TEL_AT_SMS_TEXTMODE_MAX_LEN 75
#define TEL_AT_SMS_TEXTMODE_ADDR_MAX_LEN 32
#define TEL_AT_SMS_TEXTMODE_ADDRTYPE_MAX 255

#define TEL_AT_SMS_INDEX_VAL_MIN                                                0
#define TEL_AT_SMS_INDEX_VAL_MAX                                                255
#define TEL_AT_SMS_INDEX_VAL_DEFAULT                                    0

/* AT+CMGW */
#define TEL_AT_CMGW_1_STAT_VAL_MIN                                              0
#define TEL_AT_CMGW_1_STAT_VAL_MAX                                              3
#define TEL_AT_CMGW_1_STAT_VAL_DEFAULT                                          2
#define TEL_AT_CMGW_1_STAT_STR_DEFAULT                                          "STO UNSENT"

/* AT+CMGL */
#define TEL_AT_CMGL_0_STATE_VAL_MIN                                             0
#define TEL_AT_CMGL_0_STATE_VAL_MAX                                             255
#define TEL_AT_CMGL_0_STATE_VAL_DEFAULT                                 255

/* AT+CMGD */
//#define TEL_AT_SMS_INDEX_VAL_MIN						0
//#define TEL_AT_SMS_INDEX_VAL_MAX						255
//#define TEL_AT_SMS_INDEX_VAL_DEFAULT					0

#define TEL_AT_CMGD_1_DEFFLAG_VAL_MIN                                           0
#define TEL_AT_CMGD_1_DEFFLAG_VAL_MAX                                           4
#define TEL_AT_CMGD_1_DEFFLAG_VAL_DEFAULT                                       0

/* AT+CMGF */
#define TEL_AT_CMGF_0_MODE_VAL_MIN                                              0
#define TEL_AT_CMGF_0_MODE_VAL_MAX                                              1
#define TEL_AT_CMGF_0_MODE_VAL_DEFAULT                                  0

/* AT+CSMP */
#define TEL_AT_CSMP_0_FO_VAL_MIN                                                0
#define TEL_AT_CSMP_0_FO_VAL_MAX                                                1
#define TEL_AT_CSMP_0_FO_VAL_DEFAULT                                    1

#define TEL_AT_CSMP_1_VP_VAL_MIN                                                0
#define TEL_AT_CSMP_1_VP_VAL_MAX                                                255
#define TEL_AT_CSMP_1_VP_VAL_DEFAULT                                    167

#define TEL_AT_CSMP_2_PID_VAL_MIN                                               0
#define TEL_AT_CSMP_2_PID_VAL_MAX                                               1
#define TEL_AT_CSMP_2_PID_VAL_DEFAULT                                   0

#define TEL_AT_CSMP_3_DCS_VAL_MIN                                               0
#define TEL_AT_CSMP_3_DCS_VAL_MAX                                               1
#define TEL_AT_CSMP_3_DCS_VAL_DEFAULT                                   0

/* AT+CSCB */
#define TEL_AT_CSCB_0_MODE_VAL_MIN                                              0
#define TEL_AT_CSCB_0_MODE_VAL_MAX                                              1
#define TEL_AT_CSCB_0_MODE_VAL_DEFAULT                                  0

/* AT+CNMI */
#define TEL_AT_CNMI_0_MODE_VAL_MIN                                              0
#define TEL_AT_CNMI_0_MODE_VAL_MAX                                              3
#define TEL_AT_CNMI_0_MODE_VAL_DEFAULT                                  0

#define TEL_AT_CNMI_1_MT_VAL_MIN                                                0
#define TEL_AT_CNMI_1_MT_VAL_MAX                                                3
#define TEL_AT_CNMI_1_MT_VAL_DEFAULT                                    0

#define TEL_AT_CNMI_2_BM_VAL_MIN                                                0
#define TEL_AT_CNMI_2_BM_VAL_MAX                                                3
#define TEL_AT_CNMI_2_BM_VAL_DEFAULT                                    0

#define TEL_AT_CNMI_3_DS_VAL_MIN                                                0
#define TEL_AT_CNMI_3_DS_VAL_MAX                                                2
#define TEL_AT_CNMI_3_DS_VAL_DEFAULT                                    0

#define TEL_AT_CNMI_4_BFR_VAL_MIN                                               0
#define TEL_AT_CNMI_4_BFR_VAL_MAX                                               1
#define TEL_AT_CNMI_4_BFR_VAL_DEFAULT                                   0

/* AT+CSMS */
#define TEL_AT_CSMS_0_SERVICE_VAL_MIN                                           0
#define TEL_AT_CSMS_0_SERVICE_VAL_MAX                                           127
#define TEL_AT_CSMS_0_SERVICE_VAL_DEFAULT                                       0 /* CI_MSG_SERVICE_NORMAL */

/* AT+GETIP */
#define TEL_AT_IP_INDEX_VAL_MIN                                         1
#define TEL_AT_IP_INDEX_VAL_MAX                                         8
#define TEL_AT_IP_INDEX_VAL_DEFAULT                                     1


/* Danfeng: AT*CCIREG */
#define TEL_AT_CCIREG_0_N_VAL_MIN                0
#define TEL_AT_CCIREG_0_N_VAL_MAX                1
#define TEL_AT_CCIREG_0_N_VAL_DEFAULT            0  /* disable */

/* AT+CMMS */
#define TEL_AT_CMMS_MODE_VAL_MIN                 0
#define TEL_AT_CMMS_MODE_VAL_MAX                 2
#define TEL_AT_CMMS_MODE_VAL_DEFAULT             0


/* AT+CGREG */
#define TEL_AT_CGREG_MODE_0_N_VAL_MIN                 0
#define TEL_AT_CGREG_MODE_0_N_VAL_MAX                 2
#define TEL_AT_CGREG_MODE_0_N_VAL_DEFAULT             0

/* Michal Bukai - AT+CMUT */
#define TEL_AT_CMUT_0_MODE_VAL_MIN                                              0
#define TEL_AT_CMUT_0_MODE_VAL_MAX                                              1
#define TEL_AT_CMUT_0_MODE_VAL_DEFAULT                                  0

#define ATCI_CMUT_NUM_MODE                                                              2

/* Michal Bukai - AT+CPUC */
#define TEL_AT_CPUC_0_CURRENCY_STR_DEFAULT          NULL
#define TEL_AT_CPUC_0_CURRENCY_STR_MAX_LEN          3 /* CICC_MAX_CURR_LENGTH */

#define TEL_AT_CPUC_1_PPU_STR_DEFAULT               NULL
#define TEL_AT_CPUC_1_PPU_STR_MAX_LEN               8

#define TEL_AT_CPUC_2_PASSWD_STR_DEFAULT                NULL
#define TEL_AT_CPUC_2_PASSWD_STR_MAX_LEN                TEL_AT_CMDSTR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH

/* Michal Bukai - AT+CTZR */
#define TEL_AT_CTZR_MODE_VAL_MIN                                                0
#define TEL_AT_CTZR_MODE_VAL_MAX                                                1
#define TEL_AT_CTZR_MODE_VAL_DEFAULT                                    0

#define ATCI_CTZR_NUM_MODE                                              2

#define SWAP_NIBBLE(x) ((((x) << 4) & 0xF0) + (((x) >> 4) & 0x0F))
#define SWAP_NIBBLE_INT(x)      ((((x) & 0x0F) * 10) + (((x) >> 4) & 0x0F))

/* Michal Bukai - AT+CTZU */
#define TEL_AT_CTZU_MODE_VAL_MIN                                                0
#define TEL_AT_CTZU_MODE_VAL_MAX                                                1
#define TEL_AT_CTZU_MODE_VAL_DEFAULT                                    0

#define ATCI_CTZU_NUM_MODE                                              2

/* AT+CTTY */
#define TEL_AT_CTTY_STATUS_VAL_MIN                              0       /* Voice. TTY is desabled*/
#define TEL_AT_CTTY_STATUS_VAL_MAX                              3       /* TTY is enabled */
#define TEL_AT_CTTY_STATUS_VAL_DEFAULT                  0
#define TEL_AT_CTTY_BaudotRate_VAL_MIN                  0               /* 45.45 */
#define TEL_AT_CTTY_BaudotRate_VAL_MAX                  1               /* 50 */
#define TEL_AT_CTTY_BaudotRate_VAL_DEFAULT              0               /* 45.45 */
/* Michal Bukai - AT+CSTA */
#define TEL_AT_CSTA_ADDRESS_TYPE_VAL_MIN                                                0
#define TEL_AT_CSTA_ADDRESS_TYPE_VAL_MAX                                                241
#define TEL_AT_CSTA_ADDRESS_TYPE_VAL_DEFAULT                                    129

/* Michal Bukai - AT+CRC */
#define TEL_AT_CRC_MODE_VAL_MIN                                         0
#define TEL_AT_CRC_MODE_VAL_MAX                                         1
#define TEL_AT_CRC_MODE_VAL_DEFAULT                                     0

/* Michal Bukai - AT+CR */
#define TEL_AT_CR_MODE_VAL_MIN                                          0
#define TEL_AT_CR_MODE_VAL_MAX                                          1
#define TEL_AT_CR_MODE_VAL_DEFAULT                                      0

/* Michal Bukai - AT+CMEE */
#define TEL_AT_CMEE_MODE_VAL_MIN                                                0
#define TEL_AT_CMEE_MODE_VAL_MAX                                                2
#define TEL_AT_CMEE_MODE_VAL_DEFAULT                                    1
/* Michal Bukai - AT+CIND */
#define TEL_AT_CIND_SIGNAL_VAL_MIN                  0
#define TEL_AT_CIND_SIGNAL_VAL_MAX                  5
#define TEL_AT_CIND_SIGNAL_VAL_DEFAULT              0

#define TEL_AT_CIND_SERVICE_VAL_MIN                 0
#define TEL_AT_CIND_SERVICE_VAL_MAX                 1
#define TEL_AT_CIND_SERVICE_VAL_DEFAULT             0

#define TEL_AT_CIND_MESSAGE_VAL_MIN                 0
#define TEL_AT_CIND_MESSAGE_VAL_MAX                 1
#define TEL_AT_CIND_MESSAGE_VAL_DEFAULT             0

#define TEL_AT_CIND_CALL_VAL_MIN                    0
#define TEL_AT_CIND_CALL_VAL_MAX                    1
#define TEL_AT_CIND_CALL_VAL_DEFAULT                0

#define TEL_AT_CIND_ROAM_VAL_MIN                    0
#define TEL_AT_CIND_ROAM_VAL_MAX                    1
#define TEL_AT_CIND_ROAM_VAL_DEFAULT                0

/* Michal Bukai - AT+CMER - only parameter #4 (ind) is configurable */
#define TEL_AT_CMER_MODE_VAL_DEFAULT                    3
#define TEL_AT_CMER_KEY_VAL_DEFAULT                     0
#define TEL_AT_CMER_DIS_VAL_DEFAULT                     0
#define TEL_AT_CMER_BUFFER_VAL_DEFAULT              0

#define TEL_AT_CMER_IND_VAL_MIN                         0
#define TEL_AT_CMER_IND_VAL_MAX                         2
#define TEL_AT_CMER_IND_VAL_DEFAULT                     0

#define TEL_AT_CMER_IND_MODE1                           1
#define TEL_AT_CMER_IND_MODE2                           2
/* Michal Bukai - AT+CGCMOD */
#define TEL_AT_CGCMOD_CID_VAL_MIN                                       0
#define TEL_AT_CGCMOD_CID_VAL_MAX                       TEL_AT_PS_CID_VAL_MAX
#define TEL_AT_CGCMOD_CID_VAL_DEFAULT                   0
/* Michal Bukai - AT+DS */
#define TEL_AT_DS_0_P0_VAL_MIN                  0
#define TEL_AT_DS_0_P0_VAL_MAX                  3
#define TEL_AT_DS_0_P0_VAL_DEFAULT              3

#define TEL_AT_DS_1_N_VAL_MIN                   0
#define TEL_AT_DS_1_N_VAL_MAX                   1
#define TEL_AT_DS_1_N_VAL_DEFAULT               0

#define TEL_AT_DS_2_P1_VAL_MIN                  512
#define TEL_AT_DS_2_P1_VAL_MAX                  65535
#define TEL_AT_DS_2_P1_VAL_DEFAULT              512

#define TEL_AT_DS_3_P2_VAL_MIN                  6
#define TEL_AT_DS_3_P2_VAL_MAX                  250
#define TEL_AT_DS_3_P2_VAL_DEFAULT              6
/* Michal Bukai - AT+CDIP */
#define TEL_AT_CDIP_0_N_VAL_MIN                0
#define TEL_AT_CDIP_0_N_VAL_MAX                1
#define TEL_AT_CDIP_0_N_VAL_DEFAULT            0  /* disable */
/* Michal Bukai - AT+CPLS */
#define TEL_AT_CPLS_0_LIST_VAL_MIN                                      0
#define TEL_AT_CPLS_0_LIST_VAL_MAX                                      2
#define TEL_AT_CPLS_0_LIST_VAL_DEFAULT                  0
/* Michal Bukai - AT+CGTFT */
#define TEL_AT_CGTFT_CID_VAL_MIN                                        0
#define TEL_AT_CGTFT_CID_VAL_MAX                                        TEL_AT_PS_CID_VAL_MAX
#define TEL_AT_CGTFT_CID_VAL_DEFAULT                    TEL_AT_PS_CID_VAL_DEFAULT

#define TEL_AT_CGTFT_PFID_VAL_MIN                                       1
#define TEL_AT_CGTFT_PFID_VAL_MAX                                       8
#define TEL_AT_CGTFT_PFID_VAL_DEFAULT                   0

#define TEL_AT_CGTFT_EPINDEX_VAL_MIN                                    0
#define TEL_AT_CGTFT_EPINDEX_VAL_MAX                                    255
#define TEL_AT_CGTFT_EPINDEX_VAL_DEFAULT                0

#define TEL_AT_CGTFT_ADDRESS_STR_MIN_LEN                                0
#define TEL_AT_CGTFT_ADDRESS_STR_MAX_LEN                                128
#define TEL_AT_CGTFT_ADDRESS_STR_DEFAULT                NULL

#define TEL_AT_CGTFT_PROTOCOLNUM_VAL_MIN                                        0
#define TEL_AT_CGTFT_PROTOCOLNUM_VAL_MAX                                        255
#define TEL_AT_CGTFT_PROTOCOLNUM_VAL_DEFAULT                    0

#define TEL_AT_CGTFT_PORT_STR_MAX_LEN                           12
#define TEL_AT_CGTFT_PORT_STR_DEFAULT                   NULL

#define TEL_AT_CGTFT_IPSEC_STR_MAX_LEN                          9
#define TEL_AT_CGTFT_IPSEC_STR_DEFAULT                  NULL

#define TEL_AT_CGTFT_TOS_STR_MAX_LEN                            8
#define TEL_AT_CGTFT_TOS_STR_DEFAULT                    NULL

#define TEL_AT_CGTFT_FLOWLABLE_STR_MAX_LEN                              6
#define TEL_AT_CGTFT_FLOWLABLE_STR_DEFAULT              NULL
#define TEL_AT_MEP_CLCK_0_FAC_STR_DEFAULT                               NULL
#define TEL_AT_MEP_CLCK_0_FAC_STR_MAX_LEN                               2          /*  */
#define TEL_AT_MEP_CLCK_1_MODE_VAL_MIN                0
#define TEL_AT_MEP_CLCK_1_MODE_VAL_MAX                2
#define TEL_AT_MEP_CLCK_1_MODE_VAL_DEFAULT            2                                 /*query*/
#define TEL_AT_MEP_CLCK_2_PASSWD_STR_DEFAULT                            NULL
#define TEL_AT_MEP_CLCK_2_PASSWD_STR_MAX_LEN                            16              /* CI_MAX_PASSWORD_LENGTH */
#define TEL_AT_ULCK_0_FAC_VAL_MIN                0
#define TEL_AT_ULCK_0_FAC_VAL_MAX                1
#define TEL_AT_ULCK_0_FAC_VAL_DEFAULT            0
#define TEL_AT_ULCK_1_MODE_VAL_MIN                0
#define TEL_AT_ULCK_1_MODE_VAL_MAX                2
#define TEL_AT_ULCK_1_MODE_VAL_DEFAULT            2                                     /*query*/
#define TEL_AT_ULCK_2_PASSWD_STR_DEFAULT                                NULL
#define TEL_AT_ULCK_2_PASSWD_STR_MAX_LEN                                16              /* CI_MAX_PASSWORD_LENGTH */
#define TEL_AT_UCPWD_PASSWD_STR_DEFAULT                         NULL
#define TEL_AT_UCPWD_PASSWD_STR_MAX_LEN                         16                      /* CI_MAX_PASSWORD_LENGTH */
#define TEL_AT_UASLM_0_OPER_VAL_MIN                0
#define TEL_AT_UASLM_0_OPER_VAL_MAX                3
#define TEL_AT_UASLM_0_OPER_VAL_DEFAULT            3                                    /*query*/
#define TEL_AT_UASLM_1_ICCID_STR_DEFAULT                                NULL
#define TEL_AT_UASLM_1_ICCID_STR_MAX_LEN                                10              /* CI_SIM_UDP_ICCID_LEN */
/* Michal Bukai - AT*BAND */
#define TEL_AT_BAND_NW_MODE_VAL_MIN                                             0
#define TEL_AT_BAND_NW_MODE_VAL_MAX                                             4
#define TEL_AT_BAND_NW_MODE_VAL_DEFAULT                         2

#define TEL_AT_BAND_BAND_VAL_MIN                                        0
#define TEL_AT_BAND_BAND_VAL_MAX                                        511
#define TEL_AT_BAND_BAND_VAL_DEFAULT                    0

#define TEL_AT_BAND_ROAMING_CONFIG_VAL_MIN         0
#define TEL_AT_BAND_ROAMING_CONFIG_VAL_MAX         2
#define TEL_AT_BAND_ROAMING_CONFIG_VAL_DEFAULT     2

#define TEL_AT_BAND_SRV_DOMAIN_VAL_MIN             0
#define TEL_AT_BAND_SRV_DOMAIN_VAL_MAX             4
#define TEL_AT_BAND_SRV_DOMAIN_VAL_DEFAULT         4

/* Michal Bukai - AT*BANDIND */
#define TEL_AT_BANDIND_MODE_VAL_MIN                                             0
#define TEL_AT_BANDIND_MODE_VAL_MAX                                             1
#define TEL_AT_BANDIND_MODE_VAL_DEFAULT                         0
/* Michal Bukai - AT*READ_CAL_STATUS */
#define TEL_AT_CAL_STATUS_VAL_MIN                                       0
#define TEL_AT_CAL_STATUS_VAL_MAX                                       1
#define TEL_AT_CAL_STATUS_VAL_DEFAULT                   0
/* Michal Bukai - AT*CNMA */
#define TEL_AT_NEW_MSG_ACK_VAL_MIN                                      0
#define TEL_AT_NEW_MSG_ACK_VAL_MAX                                      2
#define TEL_AT_NEW_MSG_ACK_VAL_DEFAULT                  0
/* AT*POWERIND */
#define TEL_AT_POWERIND_MODE_VAL_MIN                                             0
#define TEL_AT_POWERIND_MODE_VAL_MAX                                             1
#define TEL_AT_POWERIND_MODE_VAL_DEFAULT                         1
/* AT+C */

/* AT+C */

/* AT+EEMOPT */
#define TEL_AT_EEMOPT_0_MODE_VAL_MIN                0
#define TEL_AT_EEMOPT_0_MODE_VAL_MAX                4
#define TEL_AT_EEMOPT_0_MODE_VAL_DEFAULT            0  /* Turn off indicator*/

#define TEL_AT_EEMOPT_1_INTERVAL_VAL_MIN                1
#define TEL_AT_EEMOPT_1_INTERVAL_VAL_MAX                65535
#define TEL_AT_EEMOPT_1_INTERVAL_VAL_DEFAULT            15 /* default time interval 15 seconds  */

/* AT+ECHUPVT for Video telephony */
#define TEL_AT_ECHUPVT_CAUSE_VAL_MIN      0   /* unknown */
#define TEL_AT_ECHUPVT_CAUSE_VAL_MAX      127 /* TS24.008 Cause value 127 interworking, unspecified */
#define TEL_AT_ECHUPVT_CAUSE_VAL_DEFAULT  0    /* unknown */

#ifdef AT_PRODUCTION_CMNDS
/* AT*TDTR */
#define TEL_AT_TDTR_OPTION_MAX                   2
#define TEL_AT_TDTR_OPTION_MIN                   0
#define TEL_AT_TDTR_OPTION_DEFAULT               0
#define TEL_AT_TDTR_TX_RX_GAIN_STR_LEN_MAX       4
#define TEL_AT_TDTR_TX_GAIN_MAX                  157
#define TEL_AT_TDTR_TX_GAIN_MIN                  0
#define TEL_AT_TDTR_TX_GAIN_DEFAULT              0
#define TEL_AT_TDTR_RX_GAIN_MAX                  (-20)
#define TEL_AT_TDTR_RX_GAIN_MIN                  (-110)
#define TEL_AT_TDTR_RX_GAIN_DEFAULT              (-20)
#define TEL_AT_TDTR_FREQ_RANGE2_MAX              20242
#define TEL_AT_TDTR_FREQ_RANGE2_MIN              20108
#define TEL_AT_TDTR_FREQ_RANGE1_MAX              19192
#define TEL_AT_TDTR_FREQ_RANGE1_MIN              18808
#define TEL_AT_TDTR_FREQ_DEFAULT                 18808
/*Michal Bukai - AT*AUTHReq support*/
#define TEL_AT_AUTH_0_CID_VAL_MIN                0
#define TEL_AT_AUTH_0_CID_VAL_MAX                TEL_AT_PS_CID_VAL_MAX
#define TEL_AT_AUTH_0_CID_VAL_DEFAULT            TEL_AT_PS_CID_VAL_DEFAULT

#define TEL_AT_AUTH_1_TYPE_VAL_MIN                0
#define TEL_AT_AUTH_1_TYPE_VAL_MAX                2
#define TEL_AT_AUTH_1_TYPE_VAL_DEFAULT            0

#define TEL_AT_AUTH_2_USERNAME_STR_MIN_LEN				0
#define TEL_AT_AUTH_2_USERNAME_STR_MAX_LEN				50
#define TEL_AT_AUTH_2_USERNAME_STR_DEFAULT            	NULL

#define TEL_AT_AUTH_3_PASSWORD_STR_MIN_LEN				0
#define TEL_AT_AUTH_3_PASSWORD_STR_MAX_LEN				50
#define TEL_AT_AUTH_3_PASSWORD_STR_DEFAULT            	NULL
/* AT*GSMTR */
#define TEL_AT_GSMTR_OPTION_MAX                  3
#define TEL_AT_GSMTR_OPTION_MIN                  0
#define TEL_AT_GSMTR_OPTION_DEFAULT              0
#define TEL_AT_GSMTR_BAND_MODE_MAX               127
#define TEL_AT_GSMTR_BAND_MODE_MIN               1
#define TEL_AT_GSMTR_BAND_MODE_DEFAULT           1
#define TEL_AT_GSMTR_ARFCN_MAX                   1023
#define TEL_AT_GSMTR_ARFCN_MIN                   0
#define TEL_AT_GSMTR_ARFCN_DEFAULT               0
#define TEL_AT_GSMTR_AFCDAC_MAX                  32000
#define TEL_AT_GSMTR_AFCDAC_MIN                  5000
#define TEL_AT_GSMTR_AFCDAC_DEFAULT              5000
#define TEL_AT_GSMTR_TXRAMP_SCALE_MAX            1023
#define TEL_AT_GSMTR_TXRAMP_SCALE_MIN            0
#define TEL_AT_GSMTR_TXRAMP_SCALE_DEFAULT        200
#define TEL_AT_GSMTR_RXGAIN_CODE_MAX             49
#define TEL_AT_GSMTR_RXGAIN_CODE_MIN             0
#define TEL_AT_GSMTR_RXGAIN_CODE_DEFAULT         0

/* AT*TGCTRL */
#define TEL_AT_TGCTRL_NETWORK_MAX                1
#define TEL_AT_TGCTRL_NETWORK_MIN                0
#define TEL_AT_TGCTRL_NETWORK_DEFAULT            0
#define TEL_AT_TGCTRL_NETWORK_TD                 0
#define TEL_AT_TGCTRL_NETWORK_GSM                1
#define TEL_AT_TGCTRL_TD_OPTION_MAX              2
#define TEL_AT_TGCTRL_TD_OPTION_MIN              2
#define TEL_AT_TGCTRL_TD_OPTION_DEFAULT          2
#define TEL_AT_TGCTRL_GSM_OPTION_MAX             2
#define TEL_AT_TGCTRL_GSM_OPTION_MIN             0
#define TEL_AT_TGCTRL_GSM_OPTION_DEFAULT         0
#define TEL_AT_TGCTRL_GSM_REG_ADDR_MAX           0x02BB
#define TEL_AT_TGCTRL_GSM_REG_ADDR_MIN           0x0280
#define TEL_AT_TGCTRL_GSM_REG_ADDR_DEFAULT       0x0280

#endif /* AT_PRODUCTION_CMNDS */

//Michal Bukai - AT*CellLock
#define TEL_AT_CELLLOCK_0_MODE_VAL_MIN						0
#define TEL_AT_CELLLOCK_0_MODE_VAL_MAX						2
#define TEL_AT_CELLLOCK_0_MODE_VAL_DEFAULT					TEL_AT_CELLLOCK_0_MODE_VAL_MIN

#if 0
#define TEL_AT_CELLLOCK_1_ACT_VAL_MIN						0 //gsm
#define TEL_AT_CELLLOCK_1_ACT_VAL_MAX						2 //umts
#define TEL_AT_CELLLOCK_1_ACT_VAL_DEFAULT					TEL_AT_CELLLOCK_1_ACT_VAL_MIN

//according to CiGsmBandsType
#define TEL_AT_CELLLOCK_2_BAND_VAL_MIN						0
#define TEL_AT_CELLLOCK_2_BAND_VAL_MAX						255
#define TEL_AT_CELLLOCK_2_BAND_VAL_DEFAULT					TEL_AT_CELLLOCK_2_BAND_VAL_MIN
#endif

#define TEL_AT_CELLLOCK_3_ARFCN_VAL_MIN						0
#define TEL_AT_CELLLOCK_3_ARFCN_VAL_MAX						0xffff
#define TEL_AT_CELLLOCK_3_ARFCN_VAL_DEFAULT					TEL_AT_CELLLOCK_3_ARFCN_VAL_MIN


#define TEL_AT_CELLLOCK_4_SC_VAL_MIN						0
#define TEL_AT_CELLLOCK_4_SC_VAL_MAX						127
#define TEL_AT_CELLLOCK_4_SC_VAL_DEFAULT					TEL_AT_CELLLOCK_4_SC_VAL_MIN

#define TEL_AT_HTCCTO_2_MODE_VAL_MIN 1
#define TEL_AT_HTCCTO_2_MODE_VAL_MAX 7
#define TEL_AT_HTCCTO_2_MODE_VAL_DEFAULT 1
#define TEL_AT_HTCCTO_2_MODE_SET 1
#define TEL_AT_HTCCTO_2_MODE_GET 2
#define TEL_AT_HTCCTO_2_MODE_TEST 4

/*AT*EHSDPA*/
#define TEL_AT_EHSDPA_0_MODE_VAL_MIN 0
#define TEL_AT_EHSDPA_0_MODE_VAL_MAX 2
#define TEL_AT_EHSDPA_0_MODE_VAL_DEFAULT 0
#ifdef TD_SUPPORT
#define TEL_AT_EHSDPA_1_DL_CATEGORY_VAL_MIN 1
#define TEL_AT_EHSDPA_1_DL_CATEGORY_VAL_MAX 14
#define TEL_AT_EHSDPA_1_DL_CATEGORY_VAL_DEFAULT 14
//category 12 is not supported on TD
#define TEL_AT_EHSDPA_1_DL_CATEGORY_VAL_EXCEPTION 12
#define TEL_AT_EHSDPA_2_UL_CATEGORY_VAL_MIN 6
#define TEL_AT_EHSDPA_2_UL_CATEGORY_VAL_MAX 6
#define TEL_AT_EHSDPA_2_UL_CATEGORY_VAL_DEFAULT 6
#define TEL_AT_EHSDPA_3_CPC_STATE_VAL_MIN 0
#define TEL_AT_EHSDPA_3_CPC_STATE_VAL_MAX 0
#define TEL_AT_EHSDPA_3_CPC_STATE_VAL_DEFAULT 0
#else
#define TEL_AT_EHSDPA_1_DL_CATEGORY_VAL_MIN 1
#define TEL_AT_EHSDPA_1_DL_CATEGORY_VAL_MAX 6
#define TEL_AT_EHSDPA_1_DL_CATEGORY_VAL_DEFAULT 6
#define TEL_AT_EHSDPA_2_UL_CATEGORY_VAL_MIN 0
#define TEL_AT_EHSDPA_2_UL_CATEGORY_VAL_MAX 0
#define TEL_AT_EHSDPA_2_UL_CATEGORY_VAL_DEFAULT 0
#define TEL_AT_EHSDPA_3_CPC_STATE_VAL_MIN 0
#define TEL_AT_EHSDPA_3_CPC_STATE_VAL_MAX 0
#define TEL_AT_EHSDPA_3_CPC_STATE_VAL_DEFAULT 0
#endif



#ifdef AT_CUSTOMER_HTC
#define TEL_AT_TPCN_0_OPTION_VAL_MIN 0
#define TEL_AT_TPCN_0_OPTION_VAL_MAX 4
#define TEL_AT_TPCN_0_OPTION_VAL_DEFAULT 0

/* AT*FDY */
#define TEL_AT_FDY_0_TYPE_VAL_MIN 0
#define TEL_AT_FDY_0_TYPE_VAL_MAX 1
#define TEL_AT_FDY_0_TYPE_VAL_DEFAULT 0
#define TEL_AT_FDY_1_INTERVAL_VAL_MIN 1
#define TEL_AT_FDY_1_INTERVAL_VAL_MAX 65535
#define TEL_AT_FDY_1_INTERVAL_VAL_DEFAULT 3

/*AT+FWDB*/
#define TEL_AT_FWDB_0_TYPE_VAL_MIN 1
#define TEL_AT_FWDB_0_TYPE_VAL_MAX 1
#define TEL_AT_FWDB_0_TYPE_VAL_DEFAULT 1
#define TEL_AT_FWDB_1_NUMBER_VAL_MIN 1
#define TEL_AT_FWDB_1_NUMBER_VAL_MAX 5
#define TEL_AT_FWDB_1_NUMBER_VAL_DEFAULT 1
#define TEL_AT_FWDB_INDEX_VAL_MIN 0
#define TEL_AT_FWDB_INDEX_VAL_MAX 99
#define TEL_AT_FWDB_INDEX_VAL_DEFAULT -1
#define TEL_AT_FWDB_PHONE_NUMBER_MAX_LENGTH 30
#define TEL_AT_FWDB_PHONE_NUMBER_VALID_CHARACTERS "0123456789?"
#endif

/*AT^HVER*/
#define TEL_AT_HVER_MAX_HARDWARE_VERSION_LEN 31

/* AT^DCTS */
#define TEL_AT_DCTS_OPTION_MAX                0
#define TEL_AT_DCTS_OPTION_MIN                0
#define TEL_AT_DCTS_OPTION_DEFAULT           0
#define TEL_AT_DCTS_MODE_MAX                96
#define TEL_AT_DCTS_MODE_MIN                16
#define TEL_AT_DCTS_MODE_DEFAULT           16

/* AT^DEELS */
#define TEL_AT_DEELS_OPTION_MAX                1
#define TEL_AT_DEELS_OPTION_MIN                0
#define TEL_AT_DEELS_OPTION_DEFAULT           0

/* AT^DEVEI */
#define TEL_AT_DEVEI_OPTION_MAX                1
#define TEL_AT_DEVEI_OPTION_MIN                0
#define TEL_AT_DEVEI_OPTION_DEFAULT           0

/* AT^DNPR */
#define TEL_AT_DNPR_OPTION_MAX                1
#define TEL_AT_DNPR_OPTION_MIN                0
#define TEL_AT_DNPR_OPTION_DEFAULT           0
#define TEL_AT_DNPR_INTERVAL_MAX                65535
#define TEL_AT_DNPR_INTERVAL_MIN                0
#define TEL_AT_DNPR_INTERVAL_DEFAULT           3

/* AT^DUSR */
#define TEL_AT_DUSR_OPTION_MAX                1
#define TEL_AT_DUSR_OPTION_MIN                0
#define TEL_AT_DUSR_OPTION_DEFAULT           0
#define TEL_AT_DUSR_MODE_MAX                1
#define TEL_AT_DUSR_MODE_MIN                0
#define TEL_AT_DUSR_MODE_DEFAULT           0

#endif
/* END OF FILE */
