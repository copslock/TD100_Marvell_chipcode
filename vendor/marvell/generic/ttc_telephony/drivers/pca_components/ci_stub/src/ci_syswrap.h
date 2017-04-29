/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/* ===========================================================================
   File        : ci_syswrap.h
   Description : Holds the prototypes for the CI Client Stub

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

#if !defined(_CI_SYSWRAP_H_)
#define _CI_SYSWRAP_H_

#include "gbl_types.h"
#include "ci_task.h"
//#include "msl_sal_rpc_type.h"
//#include "msldl.h"

#ifdef __cplusplus
extern "C" {
#endif

enum _CiSysWrapReturnCode {
	CISW_SUCCESS=0,
	CISW_FAIL,
	CISW_INTERLINK_FAIL,    /* the link between application subsystem and communication subsystem is broken */
	CISW_SYS_ERROR,         /* General system error */

	CISW_MAX
};

typedef UINT8 CiSysWrapReturnCode;


typedef enum CISW_STATUS {
	CISW_INIT_SUCCESS = 0,
	CISW_INIT_FAIL,
	CISW_INTERLINK_DOWN,
	CISW_INTERLINK_UP,
	CISW_INTERLINK_TEST_IND,

	CISW_NUM_STATUS
} _CiSysWrapStatus;

typedef UINT8 CiSysWrapStatus;


#define CI_CLIENT_NODE_ID 0x01
#define CI_SERVER_NODE_ID 0x02

/* Refered from DL Definition  */
//#define MslSysUseLittleEndian MSL_LITTLE_ENDIAN
//#define MslSysUseBigEndian MSL_BIG_ENDIAN

/* CI Stub Program specification */
//#define CI_SVC_ID_STRING ("{12345678-1234-1234-1122334455667788}")
//#define CI_GUID { 0x12345678, 0x1234, 0x1234, { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 } }

//#define CI_UUID_STRING ("{0010A4A1-39D0-1F77-B314-400000001001}")

/* Data1:  Specifies the first 8 hexadecimal digits of the GUID */
//#define CI_SVC_ID_DATA1 0x 00000001

/* Data2: Specifies the first group of 4 hexadecimal digits. */
//#define CI_SVC_ID_DATA2 0x 0002

/* Data3: Specifies the second group of 4 hexadecimal digits. */
//#define CI_SVC_ID_DATA3 0x 0003

/* Data4: Array of 8 bytes. The first 2 bytes contain the third
						group of 4 hexadecimal digits. The remaining 6 bytes contain
						the final 12 hexadecimal digits. */
//#define CI_SVC_ID_DATA4 ("{0x0004000000000005}")



/* End  CI Stub Program specification */

#ifdef CI_STUB_CLIENT_INCLUDE

/* status indication call-back that is going to be called by the stub after initialization */
typedef void (*CiClientStubStatusInd)( CiSysWrapStatus status );

CiSysWrapReturnCode  ciClientStubInit();
void  ciClientStubDeinit(void);
#endif

#ifdef CI_STUB_SERVER_INCLUDE
//CiSysWrapReturnCode ciServerStubInit(void);
//void  ciServerStubDeinit(void);
#endif


#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  /*_CI_SYS_WRAPPER_H__ */






