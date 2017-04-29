/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/* ===========================================================================
   File        : ci_pb_cust.h
   Description : Data types file for the PB Service Group; customer extensions
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

#if !defined(_CI_PB_CUST_H_)
#define _CI_PB_CUST_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ONLY ONE CUSTOM EXTENSION IS VALID AT ONE TIME */
/* So only one custom extension compile flag shall be defined at one time */

/* currently there are no PB customer extension primitives required,
 * so must set the number of primitives to none.
 * If estensions need to be added => must comment out
 * this define line AND must use the template below for including
 * the customer specific definitions file.
 * The CI_PB_NUM_CUST_PRIM will be redefined in that file
 */
#define CI_PB_NUM_CUST_PRIM 0

/* template to add new customer include file */
/*
 #ifdef CI_CUST_newCustomerAcronym_EXTENSION
 #include "ci_pb_cust_newCustomerAcronym.h"
 #endif
 */

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  /* _CI_PB_CUST_H_ */
