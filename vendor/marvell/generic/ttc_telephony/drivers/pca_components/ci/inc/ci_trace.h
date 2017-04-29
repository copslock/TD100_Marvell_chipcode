/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/* ===========================================================================
   File        : ci_trace.h
   Description : Header file for tracing information

   Notes       :

   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.

   Unless otherwise agreed by Intel in writing, you may not remove or alter this notice or any other notice embedded
   in Materials by Intel or Intel’s suppliers or licensors in any way.
   =========================================================================== */

#if !defined(_CI_TRACE_H_)
#define _CI_TRACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ci_api_types.h"

char* CiDiagGetShOperStr(CiShOper oper);
char* CiDiagGetSgIdStr(CiServiceGroupID sgId);
char* CiDiagGetPrimIdStr(CiServiceGroupID sgId, CiPrimitiveID primId);

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  /*_CI_TRACE_H_ */

/*                      end of ci_trace.h
   --------------------------------------------------------------------------- */

