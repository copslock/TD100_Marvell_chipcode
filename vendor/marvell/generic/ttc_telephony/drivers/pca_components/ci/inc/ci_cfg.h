/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/* ===========================================================================
   File        : ci_cfg.h
   Description : Configurable Definitions for SAC Shell

   Notes       :

   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.

   Unless otherwise agreed by Intel in writing, you may not remove or alter this notice or any other notice embedded
   in Materials by Intel or Intel's suppliers or licensors in any way.
   =========================================================================== */

#if !defined(_CI_CFG_H_)
#define _CI_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Special Dial String characters */
#define CI_INTERNATIONAL_PREFIX     ( (UINT8)'+' )

/* Special Dial String characters for Extended BCD String decoding */
#define CI_BCD_SEPARATOR_CHAR       ( (UINT8)'p' )
#define CI_BCD_WILDCARD_CHAR        ( (UINT8)'?' )
#define CI_BCD_EXPANSION_BASE       ( (UINT8)'A' )

#define CI_NUM_SUPPORTED_PHONEBOOKS       8 // moved from sac_cfg.h

#define CI_MAX_CI_STRING_LENGTH            100
#define CI_MAX_CI_MSG_PDU_SIZE                     180
#define CI_MAX_CI_DATA_PDU_SIZE            800
#define CI_CURRENT_SUPPORT_PDP_NUM_TYPE        1
#define CI_CURRENT_SUPPORT_MEANCAP_RANGE       1
#define CI_CURRENT_SUPPORT_MEANCAP_INDV_LIST   1


#define CI_MAX_CI_PRIMITIVE_LIST_SIZE      156
#define CI_MAX_CI_NUMERIC_RANGE_LIST_SIZE  20
#define CI_MAX_INDV_LIST_SIZE              20

#define CI_PS_DEFAULT_COUNTER_INTERVAL 30       /* 30 sec */

#define CI_DAT_MAX_BUFS 10                      /* CI data plane: maximum number of rx buffers to register */

#define CI_CFG_PS_MAX_PDP_CTX_NUM 11

#define CI_MAX_IMEI_SV_LEN              16 /*Michal Bukai - IMEI SV support*/

#if defined(_GBL_CONFIG_H_)
#undef _GBL_CONFIG_H_
#endif
#include "gbl_config.h"


#ifdef __cplusplus
}
#endif  /*__cplusplus*/

#endif  /* _CI_CFG_H_ */


/*                      end of ci_cfg.h
   --------------------------------------------------------------------------- */




