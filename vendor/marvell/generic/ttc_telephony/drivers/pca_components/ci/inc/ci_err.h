/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/* ===========================================================================
   File        : ci_err.h
   Description : Common error primitives file for the CI

   Notes       : This file should have the same verison as the ci_api.h

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

#if !defined(_CI_ERR_H_)
#define _CI_ERR_H_

#ifdef __cplusplus
extern "C" {
#endif

#define CI_ERR_VER_MAJOR 2
#define CI_ERR_VER_MINOR 0

//ICAT EXPORTED ENUM
typedef enum CI_ERR_PRIM {
	CI_ERR_PRIM_HASNOSUPPORT_CNF = 0xF000,
	CI_ERR_PRIM_HASINVALIDPARAS_CNF,
	CI_ERR_PRIM_ISINVALIDREQUEST_CNF,
	CI_ERR_PRIM_SIMNOTREADY_CNF,
	CI_ERR_PRIM_ACCESSDENIED_CNF,
	CI_ERR_PRIM_INTERLINKDOWN_IND,
	CI_ERR_PRIM_INTERLINKDOWN_RSP,
	CI_ERR_PRIM_INTERLINKUP_IND,

	/* This should always be the last enum entry */
	CI_ERR_PRIM_NEXTAVAIL
} _CiErrPrim;

#define CI_ERR_NUM_PRIM (CI_ERR_PRIM_NEXTAVAIL - 0xF000)

typedef UINT16 CiErrPrim;

typedef CiEmptyPrim CiErrPrimHasNoSupportCnf;

//ICAT EXPORTED STRUCT
typedef struct CiErrPrimHasInvalidParasCnf_struct {
	CiErrInputCode err;
} CiErrPrimHasInvalidParasCnf;

typedef CiEmptyPrim CiErrPrimIsInvalidRequestCnf;

typedef CiEmptyPrim CiErrPrimSimNotReadyCnf;

//ICAT EXPORTED STRUCT
typedef struct CiErrPrimAccessDeniedCnf_struct {
	CiErrAccType type;
	CiErrAccCause cause;
} CiErrPrimAccessDeniedCnf;

//ICAT EXPORTED STRUCT
typedef struct CiErrPrimInterlinkDownInd_struct {
	CiErrInterLinkCause cause;
} CiErrPrimInterlinkDownInd;

//ICAT EXPORTED STRUCT
typedef struct CiErrPrimInterlinkDownRsp_struct {
	CiErrInterLinkCause cause;
} CiErrPrimInterlinkDownRsp;

typedef CiEmptyPrim CiErrPrimInterlinkUpInd;

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  /* _CI_ERR_H_ */


/*                      end of ci_err.h
   --------------------------------------------------------------------------- */




