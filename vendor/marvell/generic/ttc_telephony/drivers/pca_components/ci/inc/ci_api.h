/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/* ===========================================================================
   File        : ci_api.h
   Description : Holds the prototypes of the CI API

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

#if !defined(_CI_API_H_)
#define _CI_API_H_

#ifndef IMPC
#define IMPC
#endif
#ifdef __cplusplus
  #ifdef ENV_SYMBIAN
    #include "e32def.h"
    #undef  IMPC
    #define IMPC  IMPORT_C
  #endif
extern "C" {
#endif

#include "ci_api_types.h"

#define CI_API_VER_MAJOR 2
#define CI_API_VER_MINOR 0

IMPC CiReturnCode  ciShRegisterReq( CiShConfirm ciShConfirm, CiShOpaqueHandle opShHandle, CiShFreeReqMem ciShFreeReqMem, CiShOpaqueHandle opShFreeHandle);
IMPC CiReturnCode  ciShDeregisterReq(CiShHandle handle, CiShOpaqueHandle opShHandle);
IMPC CiReturnCode  ciShRequest(CiShHandle handle, CiShOper oper, void* reqParas, CiShOpaqueHandle opHandle);
IMPC CiReturnCode  ciRequest(CiServiceHandle handle, CiPrimitiveID primId, CiRequestHandle reqHandle, void* paras);
IMPC CiReturnCode  ciRespond(CiServiceHandle handle, CiPrimitiveID primId, CiIndicationHandle indHandle, void* paras);
#ifdef CCI_CMN_CONTIGUOUS
IMPC int cimem_CiSgCnfIndMemSize(CiServiceGroupID id, CiPrimitiveID primId);
#endif

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  /* _CI_API_H_ */


/*                      end of ci_api.h
   --------------------------------------------------------------------------- */




