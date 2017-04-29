/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/* ===========================================================================
   File        : ci_opt_dat_types.h
   Description : Data types file for the CI STUB Data Plane

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

#if !defined(_CI_OPT_DAT_TYPES_H_)
#define _CI_OPT_DAT_TYPES_H_

//#include "msldl.h"

#define CI_DAT_TYPES_VER_MAJOR 1
#define CI_DAT_TYPES_VER_MINOR 0

#include "ci_api_types.h"
#include "ci_dat.h"

#if defined(ENV_LINUX)
#include "common_datastub.h"
#endif /*ENV_LINUX*/

#define CI_DAT_DL_HEADER_SIZE   4 //MSL_UMD_PDU_HEADER_SIZE

typedef enum _CiDatPduType_
{
	CI_DATA_PDU = 1,
	CI_CTRL_PDU
}_CiDatPduType;
typedef UINT8 CiDatPduType;

/* TBD: CI client/server common internal structures for DATA service, move to some where*/
typedef union _CiGroupHandle
{
	CiServiceHandle svcHandle;           /* Ci Service Handle */
	CiSgOpaqueHandle sgHandle;
}CiGroupHandle;

typedef union _CiCallHandle
{
	CiRequestHandle reqHandle;
	CiIndicationHandle indHandle;
}CiCallHandle;

typedef struct CiAciInfo_struct {
	UINT8 dlHeader[CI_DAT_DL_HEADER_SIZE];                          /* 4 bytes aligned */
	CiDatPduType type;
	UINT8 param1;                                                   /* param1,2,3: pdu type depended parameters */
	UINT8 param2;
	UINT8 param3;
} CiAciInfo;

typedef struct CiStubInfo_struct {
	CiAciInfo info;                                 /* data, buf management PDUs*/
	CiGroupHandle gHandle;                          /* CiService, or CiSgOpaque Handle */
	CiCallHandle cHandle;                           /* CiRequest or CiIndicate Handle */
	UINT32 pad[4];                                  /* Add pad to ensure that the size of CiStubInfo is 32 bytes aligned, which is required by Comm memory cache line */
}CiStubInfo;
/* Note: sizeof (CiAciPduInfo) has to be equal to CI_DAT_ACI_HEADER_SIZE in ci_dat.h */

/* Begin of ACI optimization for data channel */

#define CI_DAT_ACI_HEADER_SIZE 32
/* DL Header(4) + Stub PDU Info(4) + service handle(4) + req/ind handle(4) + pad(16)  */


typedef struct CiDatPduHeader_struct {
	UINT32 connId;                  /* link id: call id for CS connection; context id for PS connection */
	CiDatConnType connType;         /* connection type */
	CiDatType datType;              /* data type */
	CiBoolean isLast;               /* is this the last segmentation of the packet */
	UINT8 seqNo;                    /* sequence number for each CI Dat Pdu */
	UINT32 datLen;                  /* length of payload data to follow */
	UINT8*                  pPayload;       /* point to the payload buffer for separated header buffer and payload buffer,
						 * NULL for contiguous pdu buffer */
} CiDatPduHeader;                       /* This structure must be 32 bits aligned */


typedef struct CiDatPduInfo_struct {
	/* ACI header space: ACI will fill it */
	UINT8 aciHeader[CI_DAT_ACI_HEADER_SIZE];
	/* PDU Header description */
	CiDatPduHeader ciHeader;
	/* After this point start the payload data */
} CiDatPduInfo; /* This structure must be 32 bits aligned */


typedef UINT32 CiDatBufSize;

/* Array of buffer pointers - configurable */
typedef UINT8 *CiDatBufArray[CI_DAT_MAX_BUFS];

typedef struct CiDatBufInfo_struct {
	/* number of available buffers */
	UINT32 numBufs;
	/* pointer to the array of data buffer pointers */
	CiDatBufArray   *pBufArray;
//    CiDatBufArray   bufArray;
} CiDatBufInfo;

/* End of ACI optimization for data channel */
#endif /* _CI_DAT_TYPES_H_ */


/*                      end of ci_opt_dat_types.h
   --------------------------------------------------------------------------- */




