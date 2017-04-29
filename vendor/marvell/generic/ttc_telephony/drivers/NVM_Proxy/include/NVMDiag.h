// (C) Copyright 2006 Marvell International Ltd.
// All Rights Reserved
//
/*
** INTEL CONFIDENTIAL
** Copyright 2000-2006 Intel Corporation All Rights Reserved.
**
** The source code contained or described herein and all documents
** related to the source code (Material) are owned by Intel Corporation
** or its suppliers or licensors.  Title to the Material remains with
** Intel Corporation or its suppliers and licensors. The Material contains
** trade secrets and proprietary and confidential information of Intel
** or its suppliers and licensors. The Material is protected by worldwide
** copyright and trade secret laws and treaty provisions. No part of the
** Material may be used, copied, reproduced, modified, published, uploaded,
** posted, transmitted, distributed, or disclosed in any way without Intel’s
** prior express written permission.
**
** No license under any patent, copyright, trade secret or other intellectual
** property right is granted to or conferred upon you by disclosure or
** delivery of the Materials, either expressly, by implication, inducement,
** estoppel or otherwise. Any license under such intellectual property rights
** must be express and approved by Intel in writing.
*/
#include <NVMServer_api.h>

extern NVM_CLIENT_ID nvm_acat_client_id;

/* E.5.0.690 Begin */
/*remove fdi_type.h */
/* #include "fdi_type.h"*/
/* E.5.0.690 End */
#if defined (OSA_LINUX)

typedef enum
{
	ERR_NONE = 0,           /* 0x00 - command was successful */
	ERR_READ,               /* 0x01 - error reading the flash */
	ERR_WRITE,              /* 0x02 - error writing to the flash */
	ERR_PARAM,              /* 0x03 - incorrect parameter to a function */
	ERR_OPEN,               /* 0x04 - a data has been opened already */
	ERR_EXISTS,             /* 0x05 - data already exists */
	ERR_NOTEXISTS,          /* 0x06 - data does not exist */
	ERR_SPACE,              /* 0x07 - no free space left. needs reclaim */
	ERR_NOTOPEN,            /* 0x08 - accessed an unopened data stream */
	ERR_ERASE,              /* 0x09 - error erasing the flash block */
	ERR_MAX_EXISTS,         /* 0x0A - defined maximum parameter limit */
	ERR_FORMAT,             /* 0x0B - unformatted media */
	ERR_MEDIA_TYPE,         /* 0x0C - unsupported media type */
	ERR_NOT_DONE,           /* 0x0D - aborted before completion */
	ERR_WRITE_PROTECT,      /* 0x0E - attempt write on protected media */
	ERR_FLASH_FULL,         /* 0x0F - flash media is full */
	ERR_SYSTEM,             /* 0x10 - system error */
	ERR_MAX_OPEN,          /* 0x11 - tried to open more than maximum
				*        number of data that can be opened at
				*        any given time. */
	ERR_INIT,               /* 0x12 - error during initialization */
	ERR_Q_FULL,             /* 0x13 - FDI queue is full */
	ERR_TIMEOUT,            /* 0x14 - system timeout */
	ERR_SUSPEND,            /* 0x15 - flash is in program suspended mode. */
	ERR_NOTRDY,             /* 0x16 - flash device not ready */
	ERR_LOCK,               /* 0x17 - error while locking */
	ERR_Q_NOT_EMPTY,        /* 0x18 - error while queue is not empty */
	ERR_PCKTID_MUTEX,      /* 0x19 - Try to Operate to packet object when
				*        it is not finished */
	ERR_WIP,               /* 0x1A - Indicates that there is already a
				*        WIP object */
	ERR_CRASH_N_BURN,       /* 0x1B - not sure what this error code means */
	ERR_NO_MORE_ENTRIES,    /* 0x1C - err code used by DAV internally */
	ERR_PLR_TEST_FAILURE,   /* 0x1D - err code used by DAV PLR */
	ERR_DIFF_SIZE,          /* 0x1E - err code used by DAV Compare Object */
	ERR_NOT_COMPARE,        /* 0x1F - err code used by DAV Compare Object */
	ERR_ACCESS,             /* 0x20 - file system access error */
	ERR_OPENMODE,           /* 0x21 - file was opened in the wrong mode */
	ERR_EOF,               /* 0x22 - error return from test gsm if end of
				*        file is true */
	ERR_MALLOC,             /* 0x23 - error code for malloc error */
	ERR_PERF,               /* 0x24 - error code for perfromance test */
	ERR_MAXITEMSIZE,        /* 0x25 - maximum data size limit reached */
	ERR_MAX_CODE           /* This entry should always be the
				* LAST ENTRY in this enum */
}  ERR_CODE;

/* E.5.0.690 Begin */
/*moved from fdi_lowl.h */
/* Hardware Error Values */

typedef enum
{
	HW_ERR_NONE = 0,
	HW_ERR_PARAM,
	HW_ERR_READ,
	HW_ERR_WRITE,
	HW_ERR_ERASE,
	HW_ERR_ABORT,
	HW_ERR_JEDEC,
	HW_ERR_EMPTY_LKUP,
	HW_ERR_SPACE,
	HW_ERR_FLASH_FULL,
	HW_ERR_SYSTEM,
	HW_ERR_SUSPEND,
	HW_ERR_DELETE,
	HW_ERR_NOTRDY,
	HW_ERR_LOCK,
	HW_ERR_COMPAT,
/* E 5.1.849 START */
	HW_ERR_PLR_TEST_FAILURE = 0x1D
/* E 5.1.849 END */
} HW_ERROR;


/* E.5.0.690 End */

#endif

